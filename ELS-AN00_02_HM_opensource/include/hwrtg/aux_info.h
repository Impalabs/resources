/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
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
 * aux grp info header
 */

#ifndef AUX_INFO_H
#define AUX_INFO_H

#include <linux/types.h>
#include <linux/sched.h>

struct aux_info {
	int min_util;
	int boost_util;
	int prio;
};

#endif
