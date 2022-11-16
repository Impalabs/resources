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

#ifndef __CP_DUMP_AREA_H__
#define __CP_DUMP_AREA_H__

#include <product_config.h>
#include "mntn_interface.h"
#include "osl_types.h"
#include "osl_list.h"
#include "osl_spinlock.h"
#include "bsp_memmap.h"
#include "bsp_s_memory.h"
#include "bsp_dump_mem.h"

/* field number supported by area */
#define DUMP_AREA_MAGICNUM 0x4e656464

#define DUMP_MAX_AREA_EXTEND (5)
#define DUMPA_REA_EXTEND_MAGIC (0xabcdabcd)
#define MDMAP_AREA (DUMP_AREA_MDMAP)
#define MDMAP_AREA_NAME "AP"
#define MDMAP_FIELD_ID_START (0x01000000)
#define MDMAP_FIELD_ID_END (0x01ffffff)

#define DUMP_MODEMAP_BASE_INFO_SIZE (0x180)
#define DUMP_MODEMAP_USER_DATA_SIZE (0x1000)
#define DUMP_MODEMAP_REG_SIZE (0x3000)
#define DUMP_MODEMAP_REG_SINGLE_SIZE (0x1000)

struct dump_area_extend {
    u32 magic;
    u32 area_id;
    void *phy_addr;
    void *virt_addr;
    u32 length;
};

struct dump_global_area_ctrl_s {
    u32 init_state;
    u32 length;
    struct dump_global_struct_s *virt_addr;
    unsigned long phy_addr;
};

struct dump_field_ctrl_info_s {
    struct dump_area_s *virt_area_addr;
    void *phy_area_addr;
    u32 total_length;
    u32 free_offset;
    u32 free_length;
    u32 init_flag;
    u32 field_num;
    spinlock_t lock;
};

void dump_fill_save_done(void);
s32 dump_get_lr_cp_save_done(void);
void dump_mdmap_field_init(void);
s32 dump_get_load_info(dump_load_info_s *load_info);
s32 dump_get_area_info(dump_area_id_e areaid, struct dump_area_mntn_addr_info_s *area_info);
s32 dump_area_init(void);
s32 dump_get_global_info(struct dump_global_area_ctrl_s *global_area);
void *dump_get_global_baseinfo(void);
u32 dump_get_mdm_voice_status(void);
void dump_clear_cpboot_area(void);
void *dump_get_mntn_base_addr(void);
void *dump_get_rdr_top_head(void);
void bsp_dump_get_avaiable_size(u32 *size);
void dump_unmap_mntn_addr(void);
void dump_map_mntn_addr(void);
u32 dump_get_mntn_length(void);
void dump_reset_area_info(void);
u32 dump_get_all_extend_area_len(void);
int dump_update_gloabl_area_info(u32 filed_id);

#endif
