/*
 * blpwm.c
 *
 * blpwm driver
 *
 * Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include "blpwm.h"
#include <linux/device.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pm_wakeup.h>
#include <linux/regulator/consumer.h>
#include <linux/suspend.h>
#include <linux/fs.h>
#include <linux/mtd/hisi_nve_interface.h>
#include <linux/hisi/hisi_adc.h>
#include <linux/uaccess.h>
#include <huawei_platform/log/hw_log.h>
#include "contexthub_boot.h"
#include "contexthub_recovery.h"
#include "contexthub_route.h"

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif

#define HWLOG_TAG blpwm
HWLOG_REGIST();

#define ns2cycles(X) (((((X) / 1000 * 0x10C7UL + \
	(X) % 1000 * 0x5UL) * loops_per_jiffy * HZ) + \
	(1UL << 31)) >> 32)

static struct blpwm_data *g_data;

static void blpwm_adc_dsm_report(int volt)
{
	struct dsm_client *blpwm_dsm_client;

	blpwm_dsm_client = dsm_find_client(BLPWM_DSM_CLIENT_NAME);
	if (!blpwm_dsm_client)
		hwlog_err("%s: dsm_find_client fail\n", __func__);
	else
		hwlog_info("%s: dsm_find_client succ\n", __func__);

	if (!dsm_client_ocuppy(blpwm_dsm_client)) {
		hwlog_info("%s: blpwm_dsm_client occupy succ\n", __func__);
		dsm_client_record(blpwm_dsm_client, "ADC abnormal, volt = %dmV\n", volt);
		dsm_client_notify(blpwm_dsm_client, BLPWM_DSM_ERROR_NO_ADC);
	} else {
		hwlog_err("%s: adc dsm report fail\n", __func__);
	}
}

static void blpwm_gpio_enable(int enable)
{
	if (enable)
		gpio_set_value(g_data->gpio_enable, 1);
	else
		gpio_set_value(g_data->gpio_enable, 0);

	hwlog_info("%s: %d\n", __func__, enable);
}

static int blpwm_set_hiz(int enable)
{
	int ret;

	if (enable) {
		ret = gpio_direction_input(g_data->gpio_hiz);
		if (ret < 0)
			hwlog_err("%s: can not set gpio-%d's direction to input\n",
				__func__, g_data->gpio_hiz);
	} else {
		ret = gpio_direction_output(g_data->gpio_hiz, 0);
		if (ret < 0)
			hwlog_err("%s can not set gpio-%d's direction to output\n",
				__func__, g_data->gpio_hiz);
	}
	hwlog_info("%s: %d", __func__, enable);

	return ret;
}

static void blpwm_open_sensorhub(void)
{
	int ret;
	struct write_info pkg_ap = {0};

	pkg_ap.tag = TAG_BLPWM;
	pkg_ap.cmd = CMD_CMN_OPEN_REQ;

	ret = write_customize_cmd(&pkg_ap, NULL, true);
	if (ret)
		hwlog_err(" %s: send open_req fail, ret = %d", __func__, ret);
	hwlog_info("%s", __func__);
}

static void blpwm_config_sensorhub(enum obj_sub_cmd sub_cmd)
{
	int ret;
	struct write_info pkg_ap = {0};
	pkt_blpwm_req_t pkt_blpwm = { {0} };
	struct pkt_header *hd = (struct pkt_header *)&pkt_blpwm;

	pkg_ap.tag = TAG_BLPWM;
	pkg_ap.cmd = CMD_CMN_CONFIG_REQ;
	pkg_ap.wr_buf = &hd[1];
	pkg_ap.wr_len = sizeof(pkt_blpwm) - sizeof(*hd);
	pkt_blpwm.sub_cmd = sub_cmd;
	pkt_blpwm.pwm_cycle = g_data->duty_cycle_value;
	pkt_blpwm.usage = g_data->usage;
	pkt_blpwm.usage_stop = g_data->usage_stop;

	hwlog_info("%s: sub_cmd is %d, duty is %d, usage is %d, usage_stop is %d\n",
		__func__, pkt_blpwm.sub_cmd, pkt_blpwm.pwm_cycle, pkt_blpwm.usage,
		pkt_blpwm.usage_stop);

	ret = write_customize_cmd(&pkg_ap, NULL, true);
	if (ret)
		hwlog_err("%s: set pwm_cycle fail, ret = %d", __func__, ret);
	hwlog_info("%s", __func__);
}

static void blpwm_close_sensorhub(void)
{
	int ret;
	struct write_info pkg_ap = {0};

	pkg_ap.tag = TAG_BLPWM;
	pkg_ap.cmd = CMD_CMN_CLOSE_REQ;

	ret = write_customize_cmd(&pkg_ap, NULL, true);
	if (ret)
		hwlog_err("%s: send close_req fail, ret = %d", __func__, ret);
	hwlog_info("%s", __func__);
}

static int blpwm_read_nv_info(int *pcolor)
{
	int ret;
	struct hisi_nve_info_user user_info = {0};

	user_info.nv_operation = NV_READ;
	user_info.nv_number = NV_NUMBER;
	user_info.valid_size = NV_VALID_SIZE;
	strncpy(user_info.nv_name, "BLPWM", sizeof(user_info.nv_name) - 1);
	user_info.nv_name[sizeof(user_info.nv_name) - 1] = '\0';
	memset(user_info.nv_data, 0, sizeof(user_info.nv_data));
	ret = hisi_nve_direct_access(&user_info);
	if (ret) {
		hwlog_err("hisi_nve_direct_access read error %d\n", ret);
		return -1;
	}

	memcpy(pcolor, user_info.nv_data, sizeof(int));

	hwlog_info("%s: color read from nv is: %d", __func__, *pcolor);
	return 0;
}

static int blpwm_write_nv_info(int color)
{
	int ret;
	struct hisi_nve_info_user user_info = {0};

	user_info.nv_operation = NV_WRITE;
	user_info.nv_number = NV_NUMBER;
	user_info.valid_size = NV_VALID_SIZE;

	strncpy(user_info.nv_name, "BLPWM", sizeof(user_info.nv_name) - 1);
	user_info.nv_name[sizeof(user_info.nv_name) - 1] = '\0';
	memcpy(user_info.nv_data, &color, sizeof(color));

	ret = hisi_nve_direct_access(&user_info);
	if (ret) {
		hwlog_err("hisi_nve_direct_access write error %d\n", ret);
		return -1;
	}

	hwlog_info("%s", __func__);
	return 0;
}

static void blpwm_close(void)
{
	blpwm_gpio_enable(DISABLE);
	blpwm_close_sensorhub();

	if (g_data->bom_version == BOM_VERSION_V1)
		blpwm_set_hiz(DISABLE);
	else
		blpwm_set_hiz(ENABLE);
}

static inline bool blpwm_is_valid_color(uint32_t color)
{
	return (color > COLOR_ID_MIN && color <= COLOR_ID_MAX);
}

static void blpwm_delay(unsigned long ns)
{
	cycles_t start;
	unsigned long cycles = ns2cycles(ns);

	/* make sure IO write finished */
	wmb();
	cycles = (cycles > 1) ? (cycles - 1) : cycles;
	start = get_cycles();
	while ((get_cycles() - start) < cycles)
		;
}

static bool blpwm_adc_detect_v1(void)
{
	unsigned long flags;
	int loop = ADC_LOOP_TIMES;

	while (loop--) {
		mdelay(ADC_LOOP_PERIOD); /* 5ms */
		blpwm_config_sensorhub(SUB_CMD_BLPWM_ADC);
		mdelay(ADC_SENSORHUB_PREPARE_TIME);
		blpwm_gpio_enable(ENABLE);
		blpwm_set_hiz(ENABLE);
		mdelay(ADC_PREPARE_TIME);

		raw_spin_lock_irqsave(&g_data->spin_lock, flags);
		blpwm_set_hiz(DISABLE);
		blpwm_delay(ADC_DETECT_TIME); /* 15ms */
		raw_spin_unlock_irqrestore(&g_data->spin_lock, flags);

		g_data->volt = hisi_adc_get_value(ADC_CHANNEL);
		if (g_data->volt < 0)
			hwlog_err("%s: get adc fail, err: %d\n", __func__, g_data->volt);
		hwlog_info("volt is %d\n", g_data->volt);
		blpwm_close();
		if ((g_data->volt >= ADC_MIN_SAFE_THRESH) &&
			(g_data->volt <= ADC_MAX_SAFE_THRESH)) {
			hwlog_info("volt is normal, loop is %d\n", loop);
			blpwm_open_sensorhub();
			return false;
		} else if (g_data->volt < ADC_MIN_SAFE_THRESH) {
			hwlog_info("capacity is large, loop is %d\n", loop);
		} else {
			hwlog_info("shortcurcuit, loop is %d\n", loop);
		}
	}

	blpwm_open_sensorhub();
	blpwm_adc_dsm_report(g_data->volt);
	return true;
}

static bool blpwm_adc_detect_v2(void)
{
	unsigned long flags;
	int max_volt = INT_MIN;
	int min_volt = INT_MAX;
	int tmp;
	int diff;
	int cnt = 0;
	int loop = ADC_LOOP_TIMES;

	while (loop > 0) {
		loop--;
		blpwm_open_sensorhub();
		blpwm_config_sensorhub(SUB_CMD_BLPWM_ADC_V2);
		blpwm_set_hiz(ENABLE);
		blpwm_gpio_enable(ENABLE);
		mdelay(5); /* sleep 5ms to finsh config_sensorhub */
		blpwm_set_hiz(DISABLE);
		while (cnt <= ADC_DETECT_NUMBER) {
			tmp = hisi_adc_get_value(ADC_CHANNEL);
			if (tmp > max_volt)
				max_volt = tmp;
			else if (tmp < min_volt)
				min_volt = tmp;
			cnt++;
			blpwm_delay(2000000); /* interval 2ms to read adc volt */
		}

		blpwm_gpio_enable(DISABLE);
		blpwm_set_hiz(ENABLE);
		hwlog_info("loop: %d, max volt: %d, min volt: %d\n", loop, max_volt, min_volt);
		diff = max_volt - min_volt;
		if (diff >= ADC_V2_MIN_THRESH) {
			hwlog_info("blpwm detect ok, diff is %d\n", diff);
			return false;
		} else {
			hwlog_err("blpwm not support, diff is %d\n", diff);
		}
	}

	blpwm_adc_dsm_report(diff);
	return true;
}

static void blpwm_adc_init(void)
{
	if (g_data->bom_version == BOM_VERSION_V1)
		g_data->blpwm_adc_detect = blpwm_adc_detect_v1;
	else if (g_data->bom_version == BOM_VERSION_V2)
		g_data->blpwm_adc_detect = blpwm_adc_detect_v2;
}

static void blpwm_cover_detect(void)
{
	if (g_data->cover_status == COVER_STATUS_DEBUG) {
		hwlog_info("%s: cover status has been debuged\n", __func__);
		return;
	}

	if (blpwm_is_valid_color(g_data->color)) {
		if (g_data->blpwm_adc_detect()) {
			hwlog_info("%s: B2 abnormal with color\n", __func__);
			g_data->support = ABNORMAL_WITH_COLOR;
		} else {
			hwlog_info("%s: B2 normal with color\n", __func__);
			g_data->support = SUPPORT_WITH_COLOR;
		}
	} else {
		if (g_data->blpwm_adc_detect()) {
			hwlog_info("%s: B2 unsupprt with color null\n", __func__);
			g_data->support = UNSUPPORT_WITH_NOCOLOR;
		} else {
			hwlog_info("%s: B2 normal with color null\n", __func__);
			g_data->support = SUPPORT_WITH_NOCOLOR;
		}
	}

	hwlog_info("%s: detect done\n", __func__);
}

static bool blpwm_is_support(void)
{
	if (g_data->cover_status == COVER_INFO_NOT_READ) {
		blpwm_read_nv_info(&g_data->color);
		blpwm_cover_detect();
		g_data->cover_status = COVER_INFO_ALREADY_READ;
		hwlog_info("after %s: g_data->cover_status is: %d",
			__func__, g_data->cover_status);
	}

	return g_data->support == SUPPORT_WITH_COLOR;
}

void blpwm_enable_from_sensorhub(uint32_t enable)
{
	if (!g_data)
		return;

	if (!blpwm_is_support())
		return;

	if (enable == AP_CLOSE) {
		blpwm_gpio_enable(DISABLE);
		if (g_data->bom_version == BOM_VERSION_V1)
			blpwm_set_hiz(DISABLE);
		else
			blpwm_set_hiz(ENABLE);
		hwlog_info("%s: close blpwm done\n", __func__);
	} else if (enable == AP_OPEN) {
		blpwm_gpio_enable(ENABLE);
		blpwm_set_hiz(ENABLE);
		hwlog_info("%s: open blpwm done\n", __func__);
	} else {
		hwlog_err("%s: invalid status\n", __func__);
	}
}

static void blpwm_init_work(struct work_struct *work)
{
	if (!blpwm_is_support()) {
		blpwm_close_sensorhub();
		hwlog_err("%s: blpwm unsupport", __func__);
		return;
	}

	blpwm_cover_detect();
	blpwm_open_sensorhub();
	hwlog_info("%s: adc boot detect done\n", __func__);
}

static ssize_t blpwm_hal_get_info(struct file *file, char __user *buf, size_t count,
	loff_t *pos)
{
	char ret_buf[DEBUG_BUFFER_SIZE] = {0};

	if (!buf) {
		hwlog_err("buf is null\n");
		return -1;
	}

	blpwm_is_support();

	ret_buf[0] = g_data->color;
	ret_buf[1] = g_data->support;
	hwlog_info("%s: color is %d, support is %d",
		__func__, g_data->color, g_data->support);

	if (copy_to_user(buf, ret_buf, sizeof(ret_buf))) {
		hwlog_err("%s: copy_to_user failed\n", __func__);
		return -1;
	}

	hwlog_info("%s: get nv info from hal\n", __func__);
	return 0;
}

static void blpwm_store_duty(uintptr_t arg)
{
	if (!arg) {
		hwlog_err("%s: input arg is null\n", __func__);
		return;
	}

	if (copy_from_user(&g_data->duty_cycle_value, (void __user *)arg, sizeof(int))) {
		hwlog_err("blpwm %s: copy from user fail\n", __func__);
		return;
	}

	g_data->channel_status = DUTY_CHANNEL;
	hwlog_info("%s: g_data->duty is: %d\n", __func__, g_data->duty_cycle_value);
}

static void blpwm_store_useage(uintptr_t arg)
{
	if (!arg) {
		hwlog_err("%s: input arg is null\n", __func__);
		return;
	}

	if (copy_from_user(&g_data->usage, (void __user *)arg, sizeof(int))) {
		hwlog_err("%s: copy from user fail\n", __func__);
		return;
	}

	g_data->channel_status = USAGE_CHANNEL;
	hwlog_info("%s: g_data->usage is %d\n", __func__, g_data->usage);
}

static void blpwm_store_useage_stop(uintptr_t arg)
{
	if (!arg) {
		hwlog_err("%s: input arg is null\n", __func__);
		return;
	}

	if (copy_from_user(&g_data->usage_stop, (void __user *)arg, sizeof(int))) {
		hwlog_err("%s: copy from user fail\n", __func__);
		return;
	}

	g_data->channel_status = USAGE_STOP_CHANNEL;
	hwlog_info("%s: g_data->usage_stop is %d\n", __func__, g_data->usage_stop);
}

static void blpwm_ringstone_dark(void)
{
	g_data->duty_cycle_value = SENSOR_PWM_CYCLE_MIN;
	blpwm_open_sensorhub();
	blpwm_config_sensorhub(SUB_CMD_BLPWM_PWM);
	blpwm_gpio_enable(ENABLE);
	blpwm_set_hiz(ENABLE);
}

static void blpwm_ringstone_stop(void)
{
	if (!blpwm_is_support())
		return;

	blpwm_gpio_enable(DISABLE);
	blpwm_open_sensorhub();
	blpwm_config_sensorhub(SUB_CMD_BLPWM_USAGE_STOP);
	if (g_data->bom_version == BOM_VERSION_V1)
		blpwm_set_hiz(DISABLE);
	else
		blpwm_set_hiz(ENABLE);

	mdelay(BLPWM_CLOSE_RESET_TIME);
	g_data->blpwm_adc_detect();
}

static void blpwm_set_duty(void)
{
	if (!blpwm_is_support())
		return;

	g_data->duty_cycle_value = APP_PWM_CYCLE_MAX - g_data->duty_cycle_value;

	if (g_data->duty_cycle_value == SENSOR_PWM_CYCLE_MIN) {
		blpwm_ringstone_dark();
	} else {
		blpwm_open_sensorhub();
		blpwm_config_sensorhub(SUB_CMD_BLPWM_PWM);
		blpwm_set_hiz(ENABLE);
		blpwm_gpio_enable(ENABLE);
	}
}

static void blpwm_set_usage(void)
{
	if (!blpwm_is_support())
		return;

	blpwm_open_sensorhub();
	blpwm_config_sensorhub(SUB_CMD_BLPWM_USAGE);
}

#ifdef FACTORY_BLPWM
static void blpwm_enable_sensorhub_by_at(int sub_cmd)
{
	int ret;
	struct write_info pkg_ap;
	pkt_subcmd_req_t cpkt;
	struct pkt_header *hd = (struct pkt_header *)&cpkt;

	memset(&pkg_ap, 0, sizeof(pkg_ap));
	memset(&cpkt, 0, sizeof(cpkt));
	pkg_ap.tag = TAG_BLPWM;
	pkg_ap.cmd = CMD_CMN_CONFIG_REQ;
	pkg_ap.wr_buf = &hd[1];
	pkg_ap.wr_len = sizeof(int);

	if (sub_cmd == AT_PLAN1_CMD) {
		cpkt.subcmd = SUB_CMD_BLPWM_FACTORY_AT1;
	} else if (sub_cmd == AT_PLAN2_CMD) {
		cpkt.subcmd = SUB_CMD_BLPWM_FACTORY_AT2;
	} else if (sub_cmd == AT_PLAN3_CMD) {
		cpkt.subcmd = SUB_CMD_BLPWM_FACTORY_AT3;
	} else if (sub_cmd == AT_PLAN4_CMD) {
		cpkt.subcmd = SUB_CMD_BLPWM_FACTORY_AT4;
	} else {
		hwlog_info("invalid atCmd");
		return;
	}

	ret = write_customize_cmd(&pkg_ap, NULL, true);
	if (ret)
		hwlog_err("%s: send config req fail, ret = %d", __func__, ret);
	hwlog_info("blpwm: %s", __func__);
}

static void blpwm_dbc_plan1(void)
{
	blpwm_gpio_enable(ENABLE);
	blpwm_enable_sensorhub_by_at(AT_PLAN1_CMD);
	blpwm_set_hiz(ENABLE);
}

static void blpwm_dbc_plan2(void)
{
	blpwm_gpio_enable(ENABLE);
	blpwm_enable_sensorhub_by_at(AT_PLAN2_CMD);
	blpwm_set_hiz(ENABLE);
}

static void blpwm_dbc_plan3(void)
{
	blpwm_gpio_enable(ENABLE);
	blpwm_enable_sensorhub_by_at(AT_PLAN3_CMD);
	blpwm_set_hiz(ENABLE);
}

static void blpwm_dbc_plan4(void)
{
	blpwm_gpio_enable(ENABLE);
	blpwm_enable_sensorhub_by_at(AT_PLAN4_CMD);
	blpwm_set_hiz(ENABLE);
}

static void blpwm_dbc_reset(void)
{
	blpwm_gpio_enable(DISABLE);
	blpwm_enable_sensorhub_by_at(AT_PLAN1_CMD);
	blpwm_set_hiz(ENABLE);
}
#endif /* FACTORY_BLPWM */

static long blpwm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	void __user *data = (void __user *)(uintptr_t)arg;

	switch (cmd) {
#ifdef FACTORY_BLPWM
	case IOCTL_BLPWM_PLAN1_CMD:
		blpwm_dbc_plan1();
		break;
	case IOCTL_BLPWM_PLAN2_CMD:
		blpwm_dbc_plan2();
		break;
	case IOCTL_BLPWM_PLAN3_CMD:
		blpwm_dbc_plan3();
		break;
	case IOCTL_BLPWM_PLAN4_CMD:
		blpwm_dbc_plan4();
		break;
	case IOCTL_BLPWM_RESET_CMD:
		blpwm_dbc_reset();
		break;
#endif /* FACTORY_BLPWM */
	case IOCTL_BLPWM_DUTY:
		blpwm_store_duty((uintptr_t)data);
		blpwm_set_duty();
		break;
	case IOCTL_BLPWM_USEAGE:
		blpwm_store_useage((uintptr_t)data);
		blpwm_set_usage();
		break;
	case IOCTL_BLPWM_USEAGE_STOP:
		blpwm_store_useage_stop((uintptr_t)data);
		blpwm_ringstone_stop();
		break;
	case IOCTL_COVER_COLOR_CMD:
		g_data->color = (uint32_t)(arg);
		ret = blpwm_write_nv_info(g_data->color);
		break;
	case IOCTL_COVER_LOOKUP_CMD:
		blpwm_read_nv_info(&g_data->color);
		blpwm_cover_detect();
		copy_to_user((uint32_t *)(uintptr_t)arg, &g_data->support,
			sizeof(g_data->support));
		break;
	case IOCTL_ADC_VOLT_CMD:
		copy_to_user((uint32_t *)(uintptr_t)arg, &g_data->volt,
			sizeof(g_data->volt));
		break;
	default:
		hwlog_err("unsupport cmd\n");
		ret = -EINVAL;
		break;
	}

	return (long)ret;
}

static int sensorhub_recovery_notifier(struct notifier_block *nb,
	unsigned long foo, void *bar)
{
	hwlog_info("%s %lu\n", __func__, foo);
	switch (foo) {
	case IOM3_RECOVERY_IDLE:
	case IOM3_RECOVERY_START:
	case IOM3_RECOVERY_MINISYS:
	case IOM3_RECOVERY_3RD_DOING:
	case IOM3_RECOVERY_FAILED:
		break;
	case IOM3_RECOVERY_DOING:
		if (blpwm_is_support())
			blpwm_open_sensorhub();
		break;
	default:
		hwlog_err("%s -unknow state %lu\n", __func__, foo);
		break;
	}
	hwlog_info("%s end\n", __func__);
	return 0;
}

static struct notifier_block sensorhub_recovery_notify = {
	.notifier_call = sensorhub_recovery_notifier,
	.priority = -2, /* SENSORHUB_RECOVERY_PRIORITY */
};

static ssize_t blpwm_support_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	int value = -1;

	if (!buf)
		return -EINVAL;

	if (!g_data)
		return -ENODEV;

	ret = kstrtoint(buf, DEBUG_BUFFER_SIZE, &value);
	if (ret) {
		hwlog_err("%s: convert to int type failed\n", __func__);
		return ret;
	}
	if ((value < SUPPORT_WITH_NOCOLOR) || (value > SUPPORT_WITH_COLOR)) {
		hwlog_err("invalid value\n");
		return -EINVAL;
	} else {
		g_data->support = value;
		g_data->cover_status = COVER_STATUS_DEBUG;
		hwlog_info("value set to %d\n", value);
	}

	return count;
}

static DEVICE_ATTR(blpwm_support, 0220, NULL, blpwm_support_store);

static ssize_t blpwm_gpio_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	int value = 0;

	if (!buf)
		return -EINVAL;

	if (!g_data)
		return -ENODEV;

	ret = kstrtoint(buf, DEBUG_BUFFER_SIZE, &value);
	if (ret) {
		hwlog_err("%s: convert to int type failed\n", __func__);
		return ret;
	}
	if (value) {
		blpwm_set_hiz(ENABLE);
		blpwm_gpio_enable(ENABLE);
		blpwm_open_sensorhub();
	} else {
		blpwm_close();
	}

	return count;
}

static DEVICE_ATTR(blpwm_gpio, 0220, NULL, blpwm_gpio_store);

static struct attribute *blpwm_attributes[] = {
	&dev_attr_blpwm_gpio.attr,
	&dev_attr_blpwm_support.attr,
	NULL
};

static const struct attribute_group blpwm_attr_group = {
	.attrs = blpwm_attributes,
};

static const struct file_operations blpwm_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = blpwm_hal_get_info,
	.unlocked_ioctl = blpwm_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = blpwm_ioctl,
#endif /* CONFIG_COMPAT */
};

static struct miscdevice blpwm_misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "blpwm",
	.fops = &blpwm_fops,
};

static int blpwm_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int ret;

	g_data = devm_kzalloc(dev, sizeof(*g_data), GFP_KERNEL);
	if (!g_data)
		return -ENOMEM;

	g_data->gpio_enable = of_get_named_gpio(dev->of_node, "gpio_enable", 0);
	if (g_data->gpio_enable < 0) {
		hwlog_err("%s can not get gpio_enable number!", __func__);
		goto get_enable_gpio_fail;
	}

	g_data->gpio_hiz = of_get_named_gpio(dev->of_node, "gpio_hiz", 0);
	if (g_data->gpio_hiz < 0) {
		hwlog_err("%s can not get gpio_hiz number!", __func__);
		goto get_hiz_gpio_fail;
	}

	if (of_property_read_u32(dev->of_node, "bom_version", &g_data->bom_version)) {
		hwlog_err("read bom_version error\n");
		goto get_hiz_gpio_fail;
	}

	blpwm_adc_init();

	raw_spin_lock_init(&g_data->spin_lock);
	register_iom3_recovery_notifier(&sensorhub_recovery_notify);

	INIT_DELAYED_WORK(&g_data->blpwm_boot_work, blpwm_init_work);
	schedule_delayed_work(&g_data->blpwm_boot_work,
		msecs_to_jiffies(BLPWM_BOOT_DELAY));

	ret = misc_register(&blpwm_misc_dev);
	if (ret) {
		hwlog_err("%s: misc register failed %d", __func__, ret);
		goto misc_register_fail;
	}

	ret = sysfs_create_group(&dev->kobj, &blpwm_attr_group);
	if (ret < 0)
		hwlog_err("%s: failed to register sysfs blpwm\n", __func__);

	return ret;

misc_register_fail:
	gpio_free(g_data->gpio_hiz);
get_hiz_gpio_fail:
	gpio_free(g_data->gpio_enable);
get_enable_gpio_fail:
	devm_kfree(dev, g_data);
	g_data = NULL;

	return -1;
}

static int blpwm_remove(struct platform_device *pdev)
{
	if (!g_data)
		return -1;

	gpio_free(g_data->gpio_enable);
	gpio_free(g_data->gpio_hiz);
	devm_kfree(&pdev->dev, g_data);
	g_data = NULL;

	return 0;
}

static const struct of_device_id blpwm_of_match[] = {
	{ .compatible = "huawei,blpwm", },
	{},
};
MODULE_DEVICE_TABLE(of, blpwm_of_match);

static struct platform_driver blpwm_driver = {
	.driver = {
		.name = "blpwm",
		.owner = THIS_MODULE,
		.of_match_table = blpwm_of_match,
	},
	.probe = blpwm_probe,
	.remove = blpwm_remove,
};

static int __init blpwm_init(void)
{
	return platform_driver_register(&blpwm_driver);
}

static void __exit blpwm_exit(void)
{
	platform_driver_unregister(&blpwm_driver);
}

device_initcall(blpwm_init);
module_exit(blpwm_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("blpwm driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");

