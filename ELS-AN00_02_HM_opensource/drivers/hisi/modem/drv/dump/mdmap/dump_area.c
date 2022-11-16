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

#include <product_config.h>
#include <mdrv_memory_layout.h>
#include <bsp_ddr.h>
#include <bsp_version.h>
#include <bsp_dump_mem.h>
#include "mntn_interface.h"
#include "dump_area.h"
#include "dump_config.h"


#undef THIS_MODU
#define THIS_MODU mod_dump
struct dump_global_area_ctrl_s g_st_dump_area_ctrl;
struct dump_field_ctrl_info_s g_st_field_ctrl;
u32 g_dump_mem_init_flag = 0;

s32 dump_get_load_info(dump_load_info_s *load_info)
{
    dump_load_info_s *dump_load = NULL;
    if (unlikely(!g_st_dump_area_ctrl.init_state)) {
        return BSP_ERROR;
    }
    if (unlikely(load_info == NULL)) {
        return BSP_ERROR;
    }
    dump_load = (dump_load_info_s *)((u8 *)g_st_dump_area_ctrl.virt_addr + (MNTN_AREA_RESERVE_ADDR - MNTN_BASE_ADDR));

    if (EOK != memcpy_s(load_info, sizeof(dump_load_info_s), dump_load, sizeof(dump_load_info_s))) {
        bsp_debug("memcpy error\n");
    }

    return BSP_OK;
}

s32 dump_get_area_info(dump_area_id_e areaid, struct dump_area_mntn_addr_info_s *area_info)
{
    if (unlikely(!g_st_dump_area_ctrl.init_state)) {
        dump_error("dump area has not init\n");
        return BSP_ERROR;
    }

    if (unlikely((areaid >= DUMP_AREA_BUTT) || (area_info == NULL))) {
        dump_error("invaild areaid = %d\n", areaid);
        return BSP_ERROR;
    }

    area_info->vaddr = (void *)(uintptr_t)((uintptr_t)g_st_dump_area_ctrl.virt_addr +
                                           g_st_dump_area_ctrl.virt_addr->area_info[areaid].offset);
    area_info->paddr =
        (void *)(uintptr_t)(g_st_dump_area_ctrl.phy_addr + g_st_dump_area_ctrl.virt_addr->area_info[areaid].offset);
    area_info->len = g_st_dump_area_ctrl.virt_addr->area_info[areaid].length;
    return BSP_OK;
}

s32 bsp_dump_get_area_info(dump_area_id_e areaid, struct dump_area_mntn_addr_info_s *area_info)
{
    return dump_get_area_info(areaid, area_info);
}

s32 dump_get_global_info(struct dump_global_area_ctrl_s *global_area)
{
    if (unlikely(g_st_dump_area_ctrl.init_state == 0 || global_area == NULL)) {
        return BSP_ERROR;
    }
    global_area->virt_addr = g_st_dump_area_ctrl.virt_addr;
    global_area->phy_addr = g_st_dump_area_ctrl.phy_addr;
    global_area->length = g_st_dump_area_ctrl.length;
    return BSP_OK;
}

void *dump_get_global_baseinfo(void)
{
    if (unlikely(!g_st_dump_area_ctrl.init_state)) {
        dump_error("dump area has not init\n");
        return NULL;
    }

    return &(g_st_dump_area_ctrl.virt_addr->base_info);
}

void dump_init_loadinfo(void)
{
    dump_load_info_s *dump_load = NULL;
    u32 ddr_size = 0;

    dump_load =
        (dump_load_info_s *)((uintptr_t)g_st_dump_area_ctrl.virt_addr + (MNTN_AREA_RESERVE_ADDR - MNTN_BASE_ADDR));
    dump_load->mdm_ddr = mdrv_mem_region_get("mdm_ddr", &ddr_size);
    dump_load->mdm_share = DDR_SHARED_MEM_ADDR;
#ifdef DDR_MNTN_ADDR
    dump_load->mdm_dump = mdrv_mem_region_get("mntn_ddr", &ddr_size);
#endif
    dump_load->mdm_sram = HI_SRAM_MEM_ADDR;
    dump_load->mdm_dts = mdrv_mem_region_get("mdm_dts_ddr", &ddr_size);
    dump_load->mdm_ddr_saveoff = 0;
    dump_load->mdm_share_ddr_size = HI_SHARED_DDR_SIZE;
    dump_load->magic_num = DUMP_LOAD_MAGIC;

#ifdef DDR_MCORE_NR_ADDR
    dump_load->nrccpu_ddr = mdrv_mem_region_get("mdm_nr_ddr", &ddr_size);
#endif
#ifdef NRCCPU_LLRAM_ADDR
    dump_load->nrccpu_llram = NRCCPU_LLRAM_ADDR;
#endif
#ifdef HAC_LLRAM_ADDR
    dump_load->nrl2hac_llram = HAC_LLRAM_ADDR;
#endif
}

void dump_alloc_area(void)
{

    dump_error("enter phone area init\n");
    if (EOK != memset_s(g_st_dump_area_ctrl.virt_addr, sizeof(*g_st_dump_area_ctrl.virt_addr), 0,
                        sizeof(*g_st_dump_area_ctrl.virt_addr))) {
        bsp_debug("memcpy error\n");
    }
    g_st_dump_area_ctrl.virt_addr->top_head.area_number = DUMP_AREA_BUTT;
    g_st_dump_area_ctrl.virt_addr->top_head.magic = DUMP_GLOBALE_TOP_HEAD_MAGIC;

    if (strcpy_s((char *)g_st_dump_area_ctrl.virt_addr->top_head.product_name,
                 sizeof(g_st_dump_area_ctrl.virt_addr->top_head.product_name), PRODUCT_NAME)) {
        bsp_debug("err\n");
    }
    if (strcpy_s((char *)g_st_dump_area_ctrl.virt_addr->top_head.product_version,
                 sizeof(g_st_dump_area_ctrl.virt_addr->top_head.product_version), PRODUCT_FULL_VERSION_STR)) {
        bsp_debug("err\n");
    }

    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_MDMAP].offset = MNTN_AREA_MDMAP_ADDR - MNTN_BASE_ADDR;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_MDMAP].length = MNTN_AREA_MDMAP_SIZE;

    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_LR].offset = MNTN_AREA_LR_ADDR - MNTN_BASE_ADDR;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_LR].length = MNTN_AREA_LR_SIZE;
#ifdef ENABLE_BUILD_NRRDR
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_NR].offset = MNTN_AREA_NR_CCPU_ADDR - MNTN_BASE_ADDR;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_NR].length = MNTN_AREA_NR_CCPU_SIZE;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_NRL2HAC].offset = MNTN_AREA_NR_L2HAC_ADDR - MNTN_BASE_ADDR;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_NRL2HAC].length = MNTN_AREA_NR_L2HAC_SIZE;
#else
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_NR].offset = 0;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_NR].length = 0;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_NRL2HAC].offset = 0;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_NRL2HAC].length = 0;
#endif
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_LPM3].offset = 0;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_LPM3].length = 0;
}

void dump_reset_area_info(void)
{
    if (EOK != memset_s(&g_st_dump_area_ctrl, sizeof(g_st_dump_area_ctrl), 0, sizeof(g_st_dump_area_ctrl))) {
        dump_debug("err\n");
    }
    dump_print("reset area info ok\n");
}

void dump_map_mntn_addr(void)
{
    if (g_st_dump_area_ctrl.virt_addr != NULL && g_st_dump_area_ctrl.length != 0 && g_st_dump_area_ctrl.phy_addr != 0) {
        return;
    }
    g_st_dump_area_ctrl.phy_addr = (unsigned long)(uintptr_t)MDDR_FAMA(MNTN_BASE_ADDR);
    g_st_dump_area_ctrl.length = MNTN_BASE_SIZE;
    g_st_dump_area_ctrl.virt_addr = (struct dump_global_struct_s *)ioremap_wc(g_st_dump_area_ctrl.phy_addr,
                                                                              g_st_dump_area_ctrl.length);
    dump_print("map mntn addr ok");
}

void dump_unmap_mntn_addr(void)
{
    if (likely(g_st_dump_area_ctrl.virt_addr != NULL && g_st_dump_area_ctrl.length != 0)) {
        iounmap((void *)g_st_dump_area_ctrl.virt_addr);
    }
}

s32 dump_area_init(void)
{
    if (unlikely(g_st_dump_area_ctrl.init_state)) {
        dump_error("dump area has init\n");
        return BSP_OK;
    }

    dump_map_mntn_addr();

    if (unlikely(g_st_dump_area_ctrl.virt_addr == NULL)) {
        dump_error("fail to ioremap MNTN_BASE_ADDR!\n");
        return BSP_ERROR;
    }

    dump_alloc_area();

    dump_init_loadinfo();

    g_st_dump_area_ctrl.init_state = true;

    dump_print("dump area init success\n");

    return BSP_OK;
}

void *dump_get_rdr_top_head(void)
{
    if (unlikely(!g_st_dump_area_ctrl.init_state)) {
        dump_error("dump area has not init\n");
        return NULL;
    }

    return &(g_st_dump_area_ctrl.virt_addr->top_head);
}

void *dump_get_mntn_base_addr(void)
{
    return g_st_dump_area_ctrl.virt_addr;
}

u32 dump_get_mntn_length(void)
{
    return g_st_dump_area_ctrl.length;
}

void dump_get_areaid_by_fieldid(u32 field_id, u32 *area_id)
{
    if (((field_id >= DUMP_MODEMAP_FIELD_BEGIN) && (field_id < DUMP_MODEMAP_FIELD_END)) ||
        ((field_id >= DUMP_KERNEL_FIELD_BEGIN) && (field_id < DUMP_KERNEL_FIELD_END))) {
        *area_id = DUMP_AREA_MDMAP;
    } else if ((field_id >= DUMP_LRCCPU_FIELD_BEGIN) && (field_id <= DUMP_LRCCPU_FIELD_END)) {
        *area_id = DUMP_AREA_LR;
    } else if ((field_id >= DUMP_MDM_LPMCU_FIELD_BEGIN) && (field_id <= DUMP_MDM_LPMCU_FIELD_END)) {
        *area_id = DUMP_AREA_LPM3;
    } else if ((field_id >= DUMP_NRCCPU_FIELD_BEGIN) && (field_id <= DUMP_NRCCPU_FIELD_END)) {
        *area_id = DUMP_AREA_NR;
    } else if ((field_id >= DUMP_NRL2HAC_FIELD_BEGIN) && (field_id <= DUMP_NRL2HAC_FIELD_END)) {
        *area_id = DUMP_AREA_NRL2HAC;
    } else if ((field_id >= DUMP_M3_FIELD_BEGIN) && (field_id < DUMP_M3_FIELD_END)) {
        *area_id = DUMP_AREA_LPM3; /* 现在手机上也有M3的AREA了 */
    }
}

s32 dump_get_area_info_by_filed_id(u32 field_id, struct dump_area_mntn_addr_info_s *area_info)
{
    u32 area_id = DUMP_AREA_BUTT;

    if (unlikely(area_info == NULL)) {
        dump_error("area_info is null \n");
        return BSP_ERROR;
    }

    dump_get_areaid_by_fieldid(field_id, &area_id);
    if (unlikely(area_id == DUMP_AREA_BUTT)) {
        dump_error("fail to find area id \n");
        return BSP_ERROR;
    }

    if (unlikely(BSP_ERROR == dump_get_area_info((dump_area_id_e)area_id, area_info))) {
        dump_error("fail to get area info \n");
        return BSP_ERROR;
    }
    return BSP_OK;
}

u8 *bsp_dump_get_field_addr(u32 field_id)
{
    u32 i = 0;
    u8 *addr = NULL;
    struct dump_area_s *parea = NULL;
    struct dump_area_mntn_addr_info_s area_info;

    if (EOK != memset_s(&area_info, sizeof(area_info), 0, sizeof(area_info))) {
        bsp_debug("err\n");
    }
    if (unlikely(BSP_ERROR == dump_get_area_info_by_filed_id(field_id, &area_info))) {
        dump_error("fail to get area info\n");
        return NULL;
    }

    if (unlikely((area_info.vaddr == NULL) || (!area_info.len) || (area_info.paddr == NULL))) {
        dump_error("area info wrong\n");
        return NULL;
    }
    parea = (struct dump_area_s *)area_info.vaddr;
    /* search field addr by field id */
    for (i = 0; i < parea->area_head.field_num; i++) {
        if (field_id == parea->fields[i].field_id) {
            addr = (u8 *)parea + parea->fields[i].offset_addr;
            return addr;
        }
    }

    return NULL;
}

u8 *bsp_dump_get_field_map(u32 field_id)
{
    u32 i = 0;
    struct dump_area_s *parea = NULL;
    struct dump_area_mntn_addr_info_s area_info;

    if (unlikely(EOK != memset_s(&area_info, sizeof(area_info), 0, sizeof(area_info)))) {
        return NULL;
    }
    if (unlikely(BSP_ERROR == dump_get_area_info_by_filed_id(field_id, &area_info))) {
        return NULL;
    }

    if (unlikely((area_info.vaddr == NULL) || (!area_info.len) || (area_info.paddr == NULL))) {
        dump_error("fail to get area info\n");
        return NULL;
    }

    parea = (struct dump_area_s *)area_info.vaddr;
    for (i = 0; i < parea->area_head.field_num; i++) {
        if (field_id == parea->fields[i].field_id) {
            return (u8 *)&parea->fields[i];
        }
    }
    return NULL;
}

s32 dump_check_field_param(u32 field_id, const char *name)
{
    if (name == NULL) {
        return BSP_ERROR;
    }

    if (((field_id >= DUMP_MODEMAP_FIELD_BEGIN) && (field_id < DUMP_MODEMAP_FIELD_END)) ||
        ((field_id >= DUMP_KERNEL_FIELD_BEGIN) && (field_id < DUMP_KERNEL_FIELD_END))) {
        return BSP_OK;
    }
    dump_error("invaild param\n");
    return BSP_ERROR;
}

s32 dump_check_availd_space(struct dump_field_ctrl_info_s *field_ctrl, u32 field_index, u32 length)
{
    if (field_index >= DUMP_FIELD_MAX_NUM) {
        dump_error("there is not enough id\n");
        return BSP_ERROR;
    }

    if (field_ctrl->free_length < length) {
        dump_error("there is not enough space\n");
        return BSP_ERROR;
    }
    return BSP_OK;
}

u8 *bsp_dump_register_field(u32 field_id, const char *name, u32 length, u16 version_id)
{
    struct dump_area_s *area_info = NULL;
    u8 *ret = NULL;
    u32 field_index;
    u32 i;
    unsigned long flags = 0;
    dump_field_map_s *pfiled = NULL;

    struct dump_field_ctrl_info_s *field_ctrl = &g_st_field_ctrl;

    if (!g_dump_mem_init_flag) {
        if (BSP_OK != bsp_dump_mem_init()) {
            dump_error("fail to init dump mem\n");
            return NULL;
        }
    }
    if (BSP_ERROR == dump_check_field_param(field_id, name)) {
        return NULL;
    }

    spin_lock_irqsave(&field_ctrl->lock, flags);

    area_info = (struct dump_area_s *)field_ctrl->virt_area_addr;
    field_index = area_info->area_head.field_num;
    for (i = 0; i < field_index; i++) {
        if (area_info->fields[i].field_id == field_id) {
            spin_unlock_irqrestore(&field_ctrl->lock, flags);
            return (u8 *)area_info + area_info->fields[i].offset_addr;
        }
    }

    if (BSP_ERROR == dump_check_availd_space(field_ctrl, field_index, length)) {
        spin_unlock_irqrestore(&field_ctrl->lock, flags);
        return NULL;
    }

    ret = (u8 *)area_info + field_ctrl->free_offset;
    DUMP_FIXED_FIELD(((void *)&(area_info->fields[field_index])), (field_id), (name), (field_ctrl->free_offset),
                     (length), version_id);
    area_info->area_head.field_num++;
    pfiled = &(area_info->fields[field_index]);

    field_ctrl->free_length -= length;
    field_ctrl->free_offset += length;
    field_ctrl->field_num++;

    area_info->area_head.field_num = field_ctrl->field_num;
    spin_unlock_irqrestore(&field_ctrl->lock, flags);

    return ret;
}

u8 *bsp_dump_get_field_phy_addr(u32 field_id)
{
    u8 *addr = 0;
    struct dump_area_mntn_addr_info_s area_info = { NULL, NULL, 0 };
    struct dump_area_s *parea = NULL;

    addr = bsp_dump_get_field_addr(field_id);
    if (unlikely(addr == NULL)) {
        return NULL;
    }
    if (unlikely(BSP_ERROR == dump_get_area_info_by_filed_id(field_id, &area_info))) {
        return NULL;
    }

    if (unlikely((area_info.vaddr == NULL) || (area_info.len == 0) || (area_info.paddr == NULL))) {
        return NULL;
    }
    parea = (struct dump_area_s *)area_info.vaddr;

    return (u8 *)(uintptr_t)((uintptr_t)addr - (uintptr_t)area_info.vaddr + (uintptr_t)area_info.paddr);
}

s32 dump_mdmap_filed_init(void)
{
    struct dump_area_mntn_addr_info_s area_info;

    if (unlikely(g_st_field_ctrl.init_flag)) {
        return BSP_OK;
    }

    spin_lock_init(&g_st_field_ctrl.lock);

    if (EOK != memset_s(&area_info, sizeof(area_info), 0, sizeof(area_info))) {
        bsp_debug("err\n");
    }
    if (unlikely(BSP_ERROR == dump_get_area_info(DUMP_AREA_MDMAP, &area_info))) {
        dump_error("fail to get DUMP_AREA_MDMAP area\n");
        return BSP_ERROR;
    }
    if (unlikely((area_info.vaddr == NULL) || (!area_info.len) || (area_info.paddr == NULL))) {
        dump_error("area info is error\n");
        return BSP_ERROR;
    }

    g_st_field_ctrl.virt_area_addr = area_info.vaddr;
    g_st_field_ctrl.phy_area_addr = area_info.paddr;
    g_st_field_ctrl.total_length = area_info.len;
    g_st_field_ctrl.free_offset = sizeof(struct dump_area_s);
    g_st_field_ctrl.free_length = g_st_field_ctrl.total_length - g_st_field_ctrl.free_offset;

    if (EOK != memset_s(g_st_field_ctrl.virt_area_addr, sizeof(*g_st_field_ctrl.virt_area_addr), 0,
                        sizeof(*g_st_field_ctrl.virt_area_addr))) {
        bsp_debug("err\n");
    }

    g_st_field_ctrl.virt_area_addr->area_head.magic_num = DUMP_LEVEL1_AREA_MAGICNUM;

    if (EOK != memcpy_s(g_st_field_ctrl.virt_area_addr->area_head.name,
                        sizeof(g_st_field_ctrl.virt_area_addr->area_head.name), MDMAP_AREA_NAME,
                        strlen(MDMAP_AREA_NAME))) {
        bsp_debug("err\n");
    }

    g_st_field_ctrl.virt_area_addr->area_head.field_num = 0;
    g_st_field_ctrl.init_flag = true;
    dump_print("mdm ap field init ok\n");
    return BSP_OK;
}

void bsp_dump_get_avaiable_size(u32 *size)
{
    unsigned long flags = 0;
    if (unlikely(size != NULL)) {
        spin_lock_irqsave(&g_st_field_ctrl.lock, flags);
        *size = g_st_field_ctrl.free_length;
        spin_unlock_irqrestore(&g_st_field_ctrl.lock, flags);
    }
}

s32 bsp_dump_field_init(void)
{
    if (BSP_OK != dump_mdmap_filed_init()) {
        dump_error("fail to init mdmap fields\n");
        return BSP_ERROR;
    }

    dump_print("dump field init ok\n");
    return BSP_OK;
}

s32 bsp_dump_mem_init(void)
{
    if (g_dump_mem_init_flag == 1) {
        return BSP_OK;
    }
    if (unlikely(BSP_ERROR == dump_area_init())) {
        return BSP_ERROR;
    }

    if (unlikely(BSP_ERROR == bsp_dump_field_init())) {
        return BSP_ERROR;
    }

    dump_print("dump mem init ok\n");
    g_dump_mem_init_flag = 1;
    return BSP_OK;
}

void dump_show_l1_field(struct dump_area_s *parea, struct dump_area_mntn_addr_info_s *area_info)
{
    u32 j = 0;
    u32 used_size = 0;

    dump_print("AREA[%s] \n", parea->area_head.name);

    for (j = 0; j < parea->area_head.field_num; j++) {
        used_size += parea->fields[j].length;
        dump_print("FILED %-16s: id=0x%-8x offset=0x%-8x size=0x%x\n", parea->fields[j].field_name,
                   parea->fields[j].field_id, parea->fields[j].offset_addr, parea->fields[j].length);
    }
    dump_print("AREA[%s] total len 0x%x,used len = 0x%x,free len = 0x%x\n", parea->area_head.name, area_info->len,
               used_size, area_info->len - used_size);
}

void dump_show_field(void)
{
    u32 i = 0;
    struct dump_area_s *parea = NULL;
    struct dump_area_mntn_addr_info_s area_info;

    for (i = 0; i < DUMP_AREA_BUTT; i++) {
        if (EOK != memset_s(&area_info, sizeof(area_info), 0, sizeof(area_info))) {
            dump_debug("err\n");
        }

        if (dump_get_area_info((dump_area_id_e)i, &area_info)) {
            continue;
        }
        if ((area_info.vaddr == NULL) || (area_info.len == 0) || (area_info.paddr == NULL)) {
            continue;
        }
        parea = (struct dump_area_s *)area_info.vaddr;
        if (parea->area_head.magic_num == DUMP_LEVEL1_AREA_MAGICNUM) {
            dump_show_l1_field(parea, &area_info);
        }
    }
    return;
}

u32 dump_get_mdm_voice_status(void)
{
    u32 flag = DUMP_OUT_VOICE;
    dump_area_head_s *area_head = NULL;
    s32 ret;
    struct dump_area_mntn_addr_info_s area_info;

    if (EOK != memset_s(&area_info, sizeof(area_info), 0, sizeof(area_info))) {
        dump_debug("err\n");
    }

    ret = dump_get_area_info(DUMP_AREA_LR, &area_info);
    if (likely(ret == BSP_OK && area_info.vaddr != NULL)) {
        area_head = (dump_area_head_s *)(area_info.vaddr);
        flag = area_head->share_info.voice_flag;
    }
    return flag;
}

int bsp_dump_register_extend_area(dump_area_id_e area_id, void *virt_addr, void *phy_addr, u32 length)
{
    return BSP_OK;
}

int dump_update_gloabl_area_info(u32 filed_id)
{
    return BSP_OK;
}

u32 dump_get_all_extend_area_len(void)
{
    return 0;
}

#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
arch_initcall(bsp_dump_mem_init);
#endif
EXPORT_SYMBOL_GPL(bsp_dump_register_field);
EXPORT_SYMBOL_GPL(bsp_dump_get_field_addr);
EXPORT_SYMBOL(dump_show_field);

