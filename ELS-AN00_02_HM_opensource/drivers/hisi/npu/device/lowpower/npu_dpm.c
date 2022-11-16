/*
 * npu_dpm.c
 *
 * about npu dpm
 *
 * Copyright (c) 2012-2020 Huawei Technologies Co., Ltd.
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
#include "npu_dpm.h"

#include <dpm_hwmon.h>
#include <asm/compiler.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/semaphore.h>
#include <linux/version.h>
#include <linux/clk.h>
#include <linux/time.h>

#include "npu_platform_register.h"
#include "npu_log.h"
#include "npu_common.h"
#include "npu_pm.h"
#include "npu_adapter.h"

unsigned int dpm_npu_states;
static DEFINE_MUTEX(dpm_npu_lock);
#if defined(CONFIG_DPM_HWMON_V1)
static int npu_dpm_get_volt(void);
#endif

struct dpm_hwmon_ops npu_dpm_ops = {
	.dpm_module_id = DPM_NPU_ID,
	.dpm_type = DPM_NPU_MODULE,
	.hi_dpm_update_counter = npu_dpm_update_counter,
#if defined(CONFIG_DPM_HWMON_V1)
	.dpm_cnt_len = DPM_TABLE_LEN,
	.dpm_fit_len = DPM_TABLE_LEN,
	.hi_dpm_fitting_coff = npu_dpm_fitting_coff,
	/* compensate_coff: 110000 * 1000000 / (735 * 735) */
	.compensate_coff = 203618,
	.hi_dpm_get_voltage = npu_dpm_get_volt,
#endif
};

#if defined(CONFIG_DPM_HWMON_V1)
void __iomem *npu_base_vir_addr;
void __iomem *npu_dpm_base_vir_addr;

extern int memcpy_s(void *dest, size_t destMax, const void *src, size_t count);

/* npu dpm aicore fitting ratio, raw_data * 1000 */
static int g_dpm_aicore_ratio_table[DPM_LOAD_NUM] = {
	0, 0, 162409, 878303, 1053963, 321457, 62124, 52290,
	0, 100926, 0, 25669, 1071591, 89425, 406761, 116247
};

static void npu_clk_open(void)
{
	unsigned int val;

	if (npu_base_vir_addr == NULL) {
		npu_drv_err("failed: npu_base_vir_addr is null\n");
		return;
	}

	val = DPM_CLK_VAL;
	writel(val, aicore_clk_en_addr(npu_base_vir_addr));
	npu_drv_warn("npu_base_vir_addr value to write = 0x%x\n", val);
	udelay(DPM_CLK_SLEEP_TIME);

	val = (unsigned int)readl(aicore_clk_stat_addr(npu_base_vir_addr));
	udelay(DPM_CLK_SLEEP_TIME);
	npu_drv_warn("value read = 0x%x\n", val);
	if (dpm_chk_clk(val))
		npu_drv_err("npu clk set fail val = %d\n", val);
}

static void npu_clk_close(void)
{
	unsigned int val;

	if (npu_base_vir_addr == NULL) {
		npu_drv_err("npu_base_vir_addr is null\n");
		return;
	}

	/* module clk off */
	val = DPM_CLK_VAL;
	writel(val, aicore_clk_dis_addr(npu_base_vir_addr));
	npu_drv_warn("val to write = ox%x\n", val);
	udelay(DPM_CLK_SLEEP_TIME);

	val = (unsigned int)readl(aicore_clk_stat_addr(npu_base_vir_addr));
	npu_drv_warn("value read = 0x%x\n", val);
	udelay(DPM_CLK_SLEEP_TIME);
}

static void npu_dpm_enable(void)
{
	if (npu_base_vir_addr == NULL) {
		npu_base_vir_addr = ioremap(SOC_ACPU_NPU_CRG_BASE_ADDR,
			SOC_NPU_CRG_SIZE);
		if (npu_base_vir_addr == NULL) {
			npu_drv_err("ioremap SOC_ACPU_NPU_CRG_BASE fail\n");
			return;
		}
	}

	mutex_lock(&dpm_npu_lock);
	if (npu_dpm_base_vir_addr == NULL) {
		npu_dpm_base_vir_addr = ioremap(SOC_ACPU_NPU_DPM_BASE_ADDR,
			SOC_NPU_DPM_SIZE);
		if (npu_dpm_base_vir_addr == NULL) {
			iounmap(npu_base_vir_addr);
			npu_base_vir_addr = NULL;
			mutex_unlock(&dpm_npu_lock);
			npu_drv_err("ioremap SOC_ACPU_DPM_NPU_BASE fail\n");
			return;
		}
	}

	writel(DPM_ENABLE_VAL, npu_base_vir_addr + DPM_UNSET_OFFSET);
	npu_clk_open();
	dpm_monitor_enable(npu_dpm_base_vir_addr, AICORE_DPMONITOR_SIGNAL_MODE);
	mutex_unlock(&dpm_npu_lock);
	npu_drv_warn("dpm_monitor_enable finish\n");
}

static void npu_dpm_disable(void)
{
	mutex_lock(&dpm_npu_lock);
	if (npu_dpm_base_vir_addr != NULL)
		dpm_monitor_disable(npu_dpm_base_vir_addr);

	npu_clk_close();

	if (npu_dpm_base_vir_addr != NULL) {
		iounmap(npu_dpm_base_vir_addr);
		npu_dpm_base_vir_addr = NULL;
	}
	mutex_unlock(&dpm_npu_lock);

	if (npu_base_vir_addr != NULL) {
		iounmap(npu_base_vir_addr);
		npu_base_vir_addr = NULL;
	}
}


static int npu_dpm_get_volt(void)
{
	if (dpm_npu_states == 0) // if power is off, direct return
		return 0;

	return hisi_npu_get_voltage();
}

static unsigned int npu_dpm_fill_ratio_table(int *fitting_table,
	const int *ratio_table)
{
	unsigned int load;
	unsigned int load_type;
	unsigned int offset = 0;

	for (load_type = 0; load_type < DPM_LOAD_TYPE_NUM; load_type++) {
		for (load = 0; load < DPM_LOAD_NUM; load++) {
			*(fitting_table + offset) = *(ratio_table + load);
			offset++;
		}
	}
	return offset;
}

int npu_dpm_fitting_coff(void)
{
	int *dpm_fitting_table = npu_dpm_ops.dpm_fitting_table;
	unsigned int len = npu_dpm_ops.dpm_fit_len;
	unsigned int i;

	if (dpm_fitting_table == NULL) {
		npu_drv_err("dpm_fitting_table is null\n");
		return 0;
	}

	for (i = 0; i < len; i++)
		npu_dpm_ops.dpm_fitting_table[i] = EARLIEST_LAYER_VALUE;

	npu_dpm_fill_ratio_table(dpm_fitting_table, g_dpm_aicore_ratio_table);

	return len;
}

int npu_dpm_update_counter(void)
{
	unsigned int i;
	unsigned int j = 0;
	unsigned int val;
	struct npu_dev_ctx *cur_dev_ctx = NULL;

	// if power is off, direct return
	if (dpm_npu_states == 0)
		return 0;

	// if g_dpm_report_enabled is false, direct return
	if (g_dpm_report_enabled == false)
		return 0;

	if (npu_dpm_ops.dpm_counter_table == NULL)
		return 0;

	cur_dev_ctx = get_dev_ctx_by_id(0);
	if (cur_dev_ctx == NULL) {
		npu_drv_err("get current device failed");
		return -1;
	}

	mutex_lock(&dpm_npu_lock);
	if (npu_dpm_base_vir_addr == NULL) {
		mutex_unlock(&dpm_npu_lock);
		return 0;
	}

	if (atomic_read(&cur_dev_ctx->power_access) == 0) {
		/* low normal high voltage 0-15 */
		writel(0x1, dpmonitor_soft_sample_pulse_addr(npu_dpm_base_vir_addr));
		udelay(1);
		/* low/normal/high voltage for counter SUM */
		for (i = 0; i < NPU_READ_REG_NUM; i++) {
			val = readl(dpmonitor_low_load0_addr(npu_dpm_base_vir_addr +
				DPM_REG_BASE_OFFSET * i));
			npu_dpm_ops.dpm_counter_table[j++] += val;
		}
	} else {
		npu_drv_err("npu is not power up\n");
	}
	mutex_unlock(&dpm_npu_lock);

	return j;
}
#endif

// npu powerup
void npu_dpm_init(void)
{
	npu_dpm_enable();

	dpm_npu_states = 1;
	npu_drv_info("dpm init success\n");
}

// npu powerdown
void npu_dpm_exit(void)
{

	dpm_npu_states = 0;

	npu_dpm_disable();
	npu_drv_warn("[dpm_npu]dpm_npu is successfully deinitialized\n");
}

static int __init dpm_npu_init(void)
{
	if (npu_plat_bypass_status() == NPU_BYPASS)
		return -1;

	return dpm_hwmon_register(&npu_dpm_ops);
}
module_init(dpm_npu_init);

static void __exit dpm_npu_exit(void)
{
	if (npu_plat_bypass_status() == NPU_BYPASS)
		return;

	dpm_hwmon_unregister(&npu_dpm_ops);
}
module_exit(dpm_npu_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DPM");
MODULE_VERSION("V1.0");

