/*
 * platform.h
 *
 * This is vdec lock interface.
 *
 * Copyright (c) 2017-2020 Huawei Technologies CO., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/kthread.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#endif

#include "regulator.h"

#define return_fail_if_cond_is_ture(cond, str) \
	do { \
		if (cond) { \
			dprint(PRN_FATAL, "[%s : %d]- %s\n", __func__, __LINE__, str); \
			return HI_FAILURE; \
		} \
	} while (0)

#define vdec_init_mutex(lock) \
	do { \
		mutex_init(lock); \
	} while (0)

#define vdec_mutex_lock(lock) \
	do { \
		mutex_lock(lock); \
	} while (0)

#define vdec_mutex_unlock(lock) \
	do { \
		mutex_unlock(lock); \
	} while (0)

#endif
