/*
 * blpwm.h
 *
 * blpwm driver
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
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

#ifndef _BLPWM_H_
#define _BLPWM_H_

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/ioctl.h>
#include <linux/timex.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#ifdef CONFIG_HUAWEI_DSM
#include <dsm/dsm_pub.h>
#endif /* CONFIG_HUAWEI_DSM */
#include "protocol.h"

#define PINCTRL_STATE_IDLE             "idle"
#define PINCTRL_STATE_DEFAULT          "default"
#define DISABLE                        0
#define ENABLE                         1
/* AT */
#define IOCTL_BLPWM_PLAN1_CMD          _IO('Q', 0x01)
#define IOCTL_BLPWM_PLAN2_CMD          _IO('Q', 0x02)
#define IOCTL_BLPWM_PLAN3_CMD          _IO('Q', 0x03)
#define IOCTL_BLPWM_PLAN4_CMD          _IO('Q', 0x04)
#define IOCTL_BLPWM_RESET_CMD          _IO('Q', 0x05)
#define IOCTL_COVER_COLOR_CMD          _IO('Q', 0x0A)
#define IOCTL_COVER_LOOKUP_CMD         _IO('Q', 0x0E)
#define IOCTL_ADC_VOLT_CMD             _IO('Q', 0x0F)

#define AT_PLAN1_CMD                   1
#define AT_PLAN2_CMD                   2
#define AT_PLAN3_CMD                   3
#define AT_PLAN4_CMD                   4
/* B2 */
#define IOCTL_BLPWM_DUTY               _IO('Q', 0x06)
#define IOCTL_BLPWM_COLOR              _IO('Q', 0x07)
#define IOCTL_BLPWM_USEAGE             _IO('Q', 0x08)
#define IOCTL_BLPWM_USEAGE_STOP        _IO('Q', 0x09)
#define APP_PWM_CYCLE_MAX              100
#define APP_PWM_CYCLE_MIN              0
#define SENSOR_PWM_CYCLE_MAX           0
#define SENSOR_PWM_CYCLE_MIN           100
#define NV_NUMBER                      449
#define NV_VALID_SIZE                  15
#define DEBUG_BUFFER_SIZE              10
#define BLPWM_BOOT_DELAY               10000
#define DUTY_CHANNEL                   1
#define USAGE_CHANNEL                  2
#define USAGE_STOP_CHANNEL             3
/* ADC */
#define ADC_LOOP_TIMES                 3
#define ADC_LOOP_PERIOD                5
#define ADC_CHANNEL                    9
#define BLPWM_CLOSE_RESET_TIME         300
#define ADC_PREPARE_TIME               10
#define ADC_SENSORHUB_PREPARE_TIME     5
#define ADC_DETECT_TIME                15000000
#define ADC_MIN_SAFE_THRESH            100
#define ADC_MAX_SAFE_THRESH            300
/* ADC v2 */
#define ADC_DETECT_NUMBER              50
#define BOM_VERSION_V1                 0
#define BOM_VERSION_V2                 1
#define ADC_V2_MIN_THRESH              200
#define ADC_V2_MAX_THRESH              1800
/* COVER INFO */
#define SUPPORT_WITH_COLOR             3
#define ABNORMAL_WITH_COLOR            2
#define UNSUPPORT_WITH_NOCOLOR         1
#define SUPPORT_WITH_NOCOLOR           0
#define COVER_STATUS_DEBUG             2
#define COVER_INFO_ALREADY_READ        1
#define COVER_INFO_NOT_READ            0
/* IPC */
#define AP_CLOSE                       1
#define AP_OPEN                        2

#define BLPWM_DSM_CLIENT_NAME          "dsm_audio_info"
#define BLPWM_DSM_ERROR_NO_ADC         921001058

enum {
	COLOR_ID_MIN = 0,
	NOCOLOR = COLOR_ID_MIN,
	DARKBLUE,
	LIGHTBLUE,
	COLOR_ID_MAX = 5
};

struct blpwm_data {
	int gpio_enable;
	int gpio_hiz;
	int duty_cycle_value;
	int usage;
	int usage_stop;
	struct delayed_work blpwm_boot_work;
	uint32_t color;
	uint32_t support;
	uint32_t volt;
	uint32_t bom_version;
	int cover_status;
	int channel_status;
	raw_spinlock_t spin_lock;
	int fail_count;
	bool (*blpwm_adc_detect)(void);
};

#ifdef CONFIG_HUAWEI_BLPWM
void blpwm_enable_from_sensorhub(uint32_t enable);
#else
static inline void blpwm_enable_from_sensorhub(uint32_t enable) {}
#endif /* CONFIG_HUAWEI_BLPWM */

#endif /* _BLPWM_H_ */
