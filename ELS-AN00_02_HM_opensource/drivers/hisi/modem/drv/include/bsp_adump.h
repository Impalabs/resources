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

#ifndef __AP_DUMP_H__
#define __AP_DUMP_H__

#include <mdrv_om.h>
#include "bsp_om_enum.h"

typedef enum {
    DUMP_ARM_VEC_RESET = 0x0,
    DUMP_ARM_VEC_UNDEF = 0x4,
    DUMP_ARM_VEC_SWI = 0x8,
    DUMP_ARM_VEC_PREFETCH = 0xc,
    DUMP_ARM_VEC_DATA = 0x10,
    DUMP_ARM_VEC_IRQ = 0x18,
    DUMP_ARM_VEC_FIQ = 0x1c,
    DUMP_ARM_VEC_UNKNOW = 0xff,
} adump_arm_vec_e;

typedef enum {
    AP_DUMP_REASON_NORMAL = 0x0,
    AP_DUMP_REASON_ARM = 0x1,
    AP_DUMP_REASON_STACKFLOW = 0x2,
    AP_DUMP_REASON_UNDEF = 0xff
} adump_reboot_reason_e;

/* 0x00~0xff linux kernel */
#define RDR_AP_DUMP_ARM_MOD_ID_START (0x80000000)
#define RDR_AP_DUMP_ARM_RESET_MOD_ID (0x80000000)
#define RDR_AP_DUMP_AP_WDT_MOD_ID (0x80000002)
#define RDR_AP_DUMP_TEEOS_WDT_MOD_ID (0x80000003)
#define RDR_AP_DUMP_ARM_UNDEF_MOD_ID (0x80000004)
#define RDR_AP_DUMP_ARM_SWI_MOD_ID (0x80000008)
#define RDR_AP_DUMP_ARM_PREFETCH_MOD_ID (0x8000000c)
#define RDR_AP_DUMP_ARM_DATA_MOD_ID (0x80000010)
#define RDR_AP_DUMP_ARM_IRQ_MOD_ID (0x80000018)
#define RDR_AP_DUMP_ARM_FIQ_MOD_ID (0x8000001c)
#define RDR_AP_DUMP_PANIC_MOD_ID (0x8000001d)

#define RDR_AP_DUMP_ARM_UNKNOW_MOD_ID (0x80000100)
#define RDR_AP_DUMP_NORMAL_EXC_MOD_ID (0x80000101)
#define RDR_AP_DUMP_AP_DMSS_MOD_ID (0x80000103)
#define RDR_AP_DUMP_AP_PCIE_MOD_ID (0x80000104)

#define RDR_AP_DUMP_ARM_MOD_ID_END (0x81FFFFFF)

#ifdef CONFIG_HISI_DUMP
void bsp_adump_save_exc_scene(u32 mod_id, u32 arg1, u32 arg2);
void ap_system_error(u32 mod_id, u32 arg1, u32 arg2, const char *data, u32 length);
dump_handle bsp_adump_register_hook(const char *name, dump_hook func);
#else
static inline void bsp_adump_save_exc_scene(u32 mod_id, u32 arg1, u32 arg2)
{
    return;
}
static inline void ap_system_error(u32 mod_id, u32 arg1, u32 arg2, const char *data, u32 length)
{
    return;
}
static inline dump_handle bsp_adump_register_hook(const char *name, dump_hook func)
{
    return -1;
}
#endif

#endif
