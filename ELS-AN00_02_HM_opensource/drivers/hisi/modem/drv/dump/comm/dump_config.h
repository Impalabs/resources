/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __DUMP_EXC_CTRL_H__
#define __DUMP_EXC_CTRL_H__

#include <product_config.h>
#include "osl_types.h"
#include "bsp_dump.h"
#ifdef BSP_CONFIG_PHONE_TYPE
#include <adrv.h>
#else
#include <bsp_rdr.h>
#endif
#include <bsp_print.h>
#include "acore_nv_stru_drv.h"
#include "nv_stru_drv.h"

/*
 * OTHERS定义
 */
#define dump_debug(fmt, ...) (bsp_debug("<%s> " fmt, __FUNCTION__, ##__VA_ARGS__))
#define dump_warning(fmt, ...) (bsp_wrn("<%s> " fmt, __FUNCTION__, ##__VA_ARGS__))
#define dump_error(fmt, ...) (bsp_err("<%s> " fmt, __FUNCTION__, ##__VA_ARGS__))
#define dump_print(fmt, ...) (bsp_err(fmt, ##__VA_ARGS__))

#define DUMP_UDELAY_1MS_NUM 1000
#define DUMP_SLEEP_5MS_NUM 5 /* 5ms */
#define DUMP_SLEEP_30MS_NUM 30 /* 5ms */
#define DUMP_WAIT_15S_NUM 15000 /* 15000 ms */

struct dump_config_info {
    NV_DUMP_STRU dump_cfg;
    DUMP_FILE_CFG_STRU dump_file_cfg;
    dump_product_type_e dump_product_type;
    dump_core_type_e dump_core_type;
    enum EDITION_KIND dump_edition;
    dump_access_mdmddr_type_e dump_access_ddr_type;
};

dump_product_type_e dump_get_product_type(void);
dump_access_mdmddr_type_e dump_get_access_mdmddr_type(void);
void dump_copy_register(u32 *dst, const u32 *src, u32 len);
enum EDITION_KIND dump_get_edition_type(void);
void dump_feature_config_init(void);
NV_DUMP_STRU *dump_get_feature_cfg(void);
DUMP_FILE_CFG_STRU *dump_get_file_cfg(void);
int dump_file_cfg_init(void);

#endif
