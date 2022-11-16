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

#ifndef __BSP_NOC_H__
#define __BSP_NOC_H__

#include <osl_types.h>
#include <osl_list.h>
#include <osl_sem.h>
#include <hi_noc_idle.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MST_NAME_MAX_LEN        (32)
#define NOC_BUS_NAME_MAX_LEN    (16)
#define NOC_ERR_PROBE_REG_SIZE  (64)
#define NOC_MAX_OFFSET          (0x3c)
#define NOC_MNTN_REG_NUM        (16)

#define NOC_RESET_GUC_MODID       0x70000009
#define NOC_RESET_NXP_MODID       0xcb000001
#define NOC_RESET_BBP_DMA0_MODID  0xcb000002
#define NOC_RESET_BBP_DMA1_MODID  0xcb000003
#define NOC_RESET_HARQ_MODID      0xcb000004
#define NOC_RESET_CPHY_MODID      0xd0000001
#define NOC_RESET_GUL2_MODID      0x31000000
#define NOC_RESET_MODID_ERR       0xffffffff
#define NOC_IDLE_TIMEOUT_CYCLE    1000

enum {
    NOC_DISABLE = 0,
    NOC_ENABLE = 1
};

enum {
    NOC_CP_RESET  = 0x0,
    NOC_AP_RESET  = 0x1
};

typedef struct {
    u32    total          :16;
    u32    max_record_num :8;
    u32    offset         :8;
} noc_record_num_s;

typedef struct {
    u32 magic_num;
    u32 noc_base_addr;
    noc_record_num_s record_num_info;
    u32 slice;
    u32 noc_reg[NOC_MNTN_REG_NUM];
} noc_dump_record_s;


#ifdef __cplusplus
}
#endif
#endif
