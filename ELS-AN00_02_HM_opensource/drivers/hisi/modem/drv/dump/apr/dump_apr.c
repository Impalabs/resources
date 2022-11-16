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
#include "product_config.h"
#include <bsp_dt.h>
#include <linux/delay.h>
#include <linux/device.h>
#include "osl_types.h"
#include "osl_malloc.h"
#include "bsp_dump.h"
#include "bsp_slice.h"
#include "bsp_dump_mem.h"
#include "dump_area.h"
#include "dump_config.h"
#include "dump_exc_handle.h"
#include "dump_logs.h"
#include "dump_apr.h"

#define THIS_MODU mod_dump

dump_apr_info_s g_dump_apr_info[NUM_OF_MODEM];

dump_apr_addr_s g_dump_apr_addr;

__init s32 dump_alloc_modidinfo_size(device_node_s *dev_node, dump_apr_info_s *dump_apr_info)
{
    device_node_s *child = NULL;
    unsigned long alloc_size;
    u32 modid_sum = 0;
    u32 sum = 0;

    bsp_dt_for_each_child_of_node(dev_node, child)
    {
        if (bsp_dt_property_read_u32(child, "modid_sum", &sum)) {
            sum = 0;
            dump_error("fail to read modid_sum!\n");
        }
        modid_sum += sum;
    }
    if (modid_sum == 0) {
        dump_error("modid_sum:0x%x\n", (u32)modid_sum);
        return BSP_OK;
    }
    alloc_size = (modid_sum * (sizeof(modid_info_s) - sizeof(u32)) + sizeof(u32));
    dump_apr_info->modid_table = (modid_info_s *)(uintptr_t)osl_malloc((u32)alloc_size);
    if (unlikely(dump_apr_info->modid_table == NULL)) {
        dump_error("fail to malloc dump_apr_info->modid_table space , size:0x%x\n", (u32)alloc_size);
        return BSP_ERROR;
    }
    if (EOK != memset_s(dump_apr_info->modid_table, alloc_size, 0, alloc_size)) {
        bsp_debug("err\n");
    }
    return BSP_OK;
}
__init s32 dump_alloc_taskinfo_size(device_node_s *dev_node, dump_apr_info_s *dump_apr_info)
{
    device_node_s *child = NULL;
    unsigned long alloc_size;
    u32 task_sum = 0;
    u32 sum = 0;

    bsp_dt_for_each_child_of_node(dev_node, child)
    {
        if (bsp_dt_property_read_u32(child, "task_sum", &sum)) {
            sum = 0;
            dump_error("fail to read task_sum !\n");
        }
        task_sum += sum;
    }
    if (task_sum == 0) {
        dump_error("task_sum:0x%x\n", (u32)task_sum);
        return BSP_OK;
    }
    alloc_size = task_sum * (sizeof(task_table_s) - sizeof(u32)) + sizeof(u32);
    dump_apr_info->cp_task_table = (task_table_s *)(uintptr_t)osl_malloc((u32)alloc_size);
    if (unlikely(dump_apr_info->cp_task_table == NULL)) {
        dump_error("fail to malloc dump_apr_info->cp_task_table space, size:0x%x\n", (u32)alloc_size);
        return BSP_ERROR;
    }

    if (EOK != memset_s(dump_apr_info->cp_task_table, alloc_size, 0, alloc_size)) {
        bsp_debug("err\n");
    }
    return BSP_OK;
}

__init s32 dump_alloc_intinfo_size(device_node_s *dev_node, dump_apr_info_s *dump_apr_info)
{
    device_node_s *child = NULL;
    unsigned long alloc_size;
    u32 int_sum = 0;
    u32 sum = 0;

    bsp_dt_for_each_child_of_node(dev_node, child)
    {
        if (bsp_dt_property_read_u32(child, "int_sum", &sum)) {
            sum = 0;
            dump_error("fail to read int_sum !\n");
        }
        int_sum += sum;
    }
    if (int_sum == 0) {
        dump_error("int_sum:0x%x\n", (u32)int_sum);
        return BSP_OK;
    }
    alloc_size = int_sum * (sizeof(interupt_table_s) - sizeof(u32)) + sizeof(u32);
    dump_apr_info->cp_interupt_table = (interupt_table_s *)(uintptr_t)osl_malloc((u32)alloc_size);
    if (unlikely(dump_apr_info->cp_interupt_table == NULL)) {
        dump_error("fail to malloc dump_apr_info->cp_interupt_table space , size:0x%x\n", (u32)alloc_size);
        return BSP_ERROR;
    }

    if (EOK != memset_s(dump_apr_info->cp_interupt_table, alloc_size, 0, alloc_size)) {
        bsp_debug("err\n");
    }
    return BSP_OK;
}

__init s32 dump_apr_read_module_size(device_node_s *dev_node, dump_apr_info_s *dump_apr_info)
{
    s32 ret;

    ret = dump_alloc_modidinfo_size(dev_node, dump_apr_info);
    if (ret == BSP_ERROR) {
        return BSP_ERROR;
    }

    ret = dump_alloc_intinfo_size(dev_node, dump_apr_info);
    if (ret == BSP_ERROR) {
        goto alloc_fail;
    }

    ret = dump_alloc_taskinfo_size(dev_node, dump_apr_info);
    if (ret == BSP_ERROR) {
        goto alloc_fail;
    }

    return BSP_OK;

alloc_fail:
    if (dump_apr_info->modid_table != NULL) {
        osl_free(dump_apr_info->modid_table);
        dump_apr_info->modid_table = NULL;
    }

    if (dump_apr_info->modid_table != NULL) {
        osl_free(dump_apr_info->modid_table);
        dump_apr_info->modid_table = NULL;
    }

    if (dump_apr_info->cp_interupt_table != NULL) {
        osl_free(dump_apr_info->cp_interupt_table);
        dump_apr_info->cp_interupt_table = NULL;
    }
    return BSP_ERROR;
}

__init void dump_apr_read_module_config(device_node_s *child, dump_apr_info_s *dump_apr_info)
{
    u32 sum = 0;
    u32 u = 0;
    u32 i = 0;
    struct property *prop = NULL;
    const __be32 *p = NULL;
    u32 module_index;
    char *module_name = NULL;

    if (bsp_dt_property_read_string(child, (const char *)"module_name", (const char **)&module_name)) {
        dump_error("fail to read module_name\n");
        return;
    }
    if (bsp_dt_property_read_u32(child, "modid_sum", &sum)) {
        sum = 0;
        dump_error("fail to read modid_sum !\n");
    }
    bsp_dt_property_for_each_u32(child, "modid_range", prop, p, u)
    {
        module_index = dump_apr_info->modid_table->moudid_sum;
        if ((i & 0x1) == 0) {
            dump_apr_info->modid_table->modid_info[module_index].modid_start = u;
        } else {
            dump_apr_info->modid_table->modid_info[module_index].modid_end = u;

            if (0 > snprintf_s(dump_apr_info->modid_table->modid_info[module_index].module,
                               sizeof(dump_apr_info->modid_table->modid_info[module_index].module),
                               (sizeof(dump_apr_info->modid_table->modid_info[module_index].module) - 1), "%s",
                               module_name)) {
                bsp_debug("er\n");
            }
            dump_apr_info->modid_table->moudid_sum++;
        }
        i++;
    }
}
__init void dump_apr_read_task_config(device_node_s *child, dump_apr_info_s *dump_apr_info)
{
    u32 sum = 0;
    u32 module_index;
    u32 index = 0;
    char *module_name = NULL;
    void *temp = NULL;

    if (bsp_dt_property_read_u32(child, "task_sum", &sum)) {
        sum = 0;
        dump_error("fail to read task_sum !\n");
    }
    for (index = 0; (index < sum) && (dump_apr_info->cp_task_table); index++) {
        module_index = dump_apr_info->cp_task_table->task_sum;
        if (bsp_dt_property_read_string_index(child, (const char *)"task_name", (int)index, (const char **)&temp)) {
            dump_error("fail to read task_name\n");
        } else {
            if (0 > snprintf_s(dump_apr_info->cp_task_table->task_info[module_index].task_name,
                               sizeof(dump_apr_info->cp_task_table->task_info[module_index].task_name),
                               (sizeof(dump_apr_info->cp_task_table->task_info[module_index].task_name) - 1), "%s",
                               (char *)temp)) {
                bsp_debug("er\n");
            }
            if (0 > snprintf_s(dump_apr_info->cp_task_table->task_info[module_index].module,
                               sizeof(dump_apr_info->cp_task_table->task_info[module_index].module),
                               (sizeof(dump_apr_info->cp_task_table->task_info[module_index].module) - 1), "%s",
                               module_name)) {
                bsp_debug("er\n");
            }
            dump_apr_info->cp_task_table->task_sum++;
        }
    }
}
__init void dump_apr_read_int_config(device_node_s *child, dump_apr_info_s *dump_apr_info)
{
    u32 sum = 0;
    u32 module_index;
    char *module_name = NULL;
    u32 index = 0;

    if (bsp_dt_property_read_u32(child, "int_sum", &sum)) {
        sum = 0;
        dump_error("fail to read int_sum !\n");
    }
    for (index = 0; (index < sum) && (dump_apr_info->cp_interupt_table); index++) {
        module_index = dump_apr_info->cp_interupt_table->intrupt_sum;
        if (bsp_dt_property_read_u32_index(
                child, "int_id", index, &(dump_apr_info->cp_interupt_table->interupt_info[module_index].interrpt_id))) {
            dump_error("fail to read int_id\n");
        } else {
            if (0 > snprintf_s(dump_apr_info->cp_interupt_table->interupt_info[module_index].module,
                               sizeof(dump_apr_info->cp_interupt_table->interupt_info[module_index].module),
                               (sizeof(dump_apr_info->cp_interupt_table->interupt_info[module_index].module) - 1), "%s",
                               module_name)) {
                bsp_debug("er\n");
            }
            dump_apr_info->cp_interupt_table->intrupt_sum++;
        }
    }
}

__init void dump_apr_init_module_info(device_node_s *dev_node, dump_apr_info_s *dump_apr_info)
{
    device_node_s *child = NULL;

    bsp_dt_for_each_child_of_node(dev_node, child)
    {
        dump_apr_read_module_config(child, dump_apr_info);
        dump_apr_read_task_config(child, dump_apr_info);
        dump_apr_read_int_config(child, dump_apr_info);
    }
}

__init s32 dump_apr_init(void)
{
    device_node_s *dev_node = NULL;
    dump_apr_info_s *dump_apr_info = NULL;
    char *node_name[NUM_OF_MODEM] = { "hisilicon,apr_config", "hisilicon,nr_apr_config" };
    u32 i = 0;

    if (dump_get_product_type() == DUMP_MBB) {
        return BSP_OK;
    }

    g_dump_apr_addr.virt_addr = bsp_dump_register_field(DUMP_MODEMAP_APR, "apr", DUMP_ARP_LOG_BUF_LEN, 0);
    g_dump_apr_addr.phy_addr = bsp_dump_get_field_phy_addr(DUMP_MODEMAP_APR);

    for (i = 0; i < NUM_OF_MODEM; i++) {
        dump_apr_info = &(g_dump_apr_info[i]);
        dev_node = bsp_dt_find_compatible_node(NULL, NULL, node_name[i]);
        if (unlikely(dev_node == NULL)) {
            dump_error("fail to read apr_config node !\n");
            continue;
        }
        if (unlikely(BSP_ERROR == dump_apr_read_module_size(dev_node, dump_apr_info))) {
            dump_error("fail to read module node !\n");
            continue;
        }
        dump_apr_init_module_info(dev_node, dump_apr_info);
    }

    dump_print("apr init success\n");
    return BSP_OK;
}

s32 dump_apr_get_normal_module(dump_except_info_s *dump_except_info, u8 *module, size_t module_len,
                               dump_apr_info_s *dump_apr_info)
{
    u32 index = 0;

    if (unlikely(dump_apr_info->modid_table == NULL)) {
        return BSP_ERROR;
    }
    for (index = 0; index < dump_apr_info->modid_table->moudid_sum; index++) {
        if ((dump_except_info->mod_id >= dump_apr_info->modid_table->modid_info[index].modid_start) &&
            (dump_except_info->mod_id < dump_apr_info->modid_table->modid_info[index].modid_end)) {
            if ((module_len >= 1) &&
                (0 > snprintf_s((char *)module, (unsigned long)module_len, (unsigned long)(module_len - 1), "%s",
                                dump_apr_info->modid_table->modid_info[index].module))) {
                bsp_debug("er\n");
            }

            return BSP_OK;
        }
    }
    /* 如果查找不到复位的责任组件则用复位任务和中断来判断责任组件 */
    if (index == dump_apr_info->modid_table->moudid_sum) {
        dump_error("can't find the modid in dts config\n");
        dump_error("reboot modid:0x%x\n", dump_except_info->mod_id);
    }
    return BSP_ERROR;
}

void dump_apr_get_module_by_taskname(dump_except_info_s *dump_except_info, const u8 *task_name, u8 *module,
                                     size_t module_len, dump_apr_info_s *dump_apr_info)
{
    u32 index = 0;
    unsigned long size = 0;

    /* wdg abort根据临终任务名或中断号确定责任组件 */
    /* 如果为中断复位 */
    if (dump_except_info->reboot_context == DUMP_CTX_INT) {
        if (unlikely(dump_apr_info->cp_interupt_table == NULL)) {
            return;
        }
        for (index = 0; index < dump_apr_info->cp_interupt_table->intrupt_sum; index++) {
            if (dump_except_info->reboot_int != dump_apr_info->cp_interupt_table->interupt_info[index].interrpt_id) {
                continue;
            }
            if ((module_len >= 1) &&
                (snprintf_s((char *)module, (unsigned long)module_len, (unsigned long)(module_len - 1), "%s",
                            dump_apr_info->cp_interupt_table->interupt_info[index].module)) < 0) {
                bsp_debug("er\n");
            }
            break;
        }
        if (index == dump_apr_info->cp_interupt_table->intrupt_sum) {
            dump_error("can't find the int in dts config\n");
            dump_error("reboot int num:0x%x\n", dump_except_info->reboot_int);
        }
        return;
    }
    if (dump_except_info->reboot_context == DUMP_CTX_TASK) {
        /* 否则为任务复位 */
        if (unlikely(dump_apr_info->cp_task_table == NULL)) {
            return;
        }
        /* 查找责任组件 */
        for (index = 0; index < dump_apr_info->cp_task_table->task_sum; index++) {
            size = strlen((const char *)(dump_apr_info->cp_task_table->task_info[index].task_name));
            size = size > strlen((const char *)task_name) ? strlen((const char *)task_name) : size;
            if (strncmp((const char *)task_name, dump_apr_info->cp_task_table->task_info[index].task_name, size)) {
                continue;
            }
            if ((module_len >= 1) &&
                (snprintf_s((char *)module, (unsigned long)module_len, (unsigned long)(module_len - 1), "%s",
                            dump_apr_info->cp_task_table->task_info[index].module)) < 0) {
                bsp_debug("er\n");
            }
            break;
        }
        /* 如果在已有任务中查找不到，则责任组件为空 */
        if (index == dump_apr_info->cp_task_table->task_sum) {
            dump_error("can't find the task in dts config\n");
            dump_error("reboot task name %s\n", task_name);
            return;
        }
    }
}
/*
 * 功能描述: 解析复位的责任组件
 */
void dump_apr_get_reset_module(dump_except_info_s *dump_except_info, u8 *task_name, u8 *module, size_t module_len,
                               dump_apr_info_s *dump_apr_info)
{
    u32 reboot_reason = dump_except_info->except_reason;

    /* 主动复位根据modid确定责任组件 */
    if (reboot_reason == DUMP_REASON_NORMAL) {
        if (BSP_OK == dump_apr_get_normal_module(dump_except_info, module, module_len, dump_apr_info)) {
            return;
        }
    }
    dump_apr_get_module_by_taskname(dump_except_info, (const u8 *)task_name, module, module_len, dump_apr_info);
}

void dump_apr_get_reset_voice(u32 voice, u8 *reboot_voice, size_t reboot_voice_len)
{
    if (reboot_voice_len < 1) {
        bsp_err("voice_len err\n");
        return;
    }
    if (voice == DUMP_OUT_VOICE) {
        if (0 > snprintf_s((char *)reboot_voice, (unsigned long)reboot_voice_len, (unsigned long)(reboot_voice_len - 1),
                           "no")) {
            bsp_debug("er\n");
        }
    } else {
        if (0 > snprintf_s((char *)reboot_voice, (unsigned long)reboot_voice_len, (unsigned long)(reboot_voice_len - 1),
                           "yes")) {
            bsp_debug("er\n");
        }
    }
    return;
}

void dump_apr_get_reset_modid(u32 reason, u32 reboot_modid, u32 *mod_id, dump_apr_info_s *dump_apr_info)
{
    if (reason == DUMP_REASON_WDT) {
        *mod_id = DRV_ERRNO_DUMP_CP_WDT;
    } else if (reason == DUMP_REASON_DLOCK) {
        *mod_id = DRV_ERRNO_DLOCK;
    } else {
        *mod_id = reboot_modid;
    }
}

void dump_apr_get_reset_core(u32 core, char *reboot_core, size_t reboot_core_len, dump_apr_info_s *dump_apr_info)
{
    if (reboot_core_len < 1) {
        bsp_err("reboot_core_len err\n");
        return;
    }
    if (core == DUMP_CPU_LRCCPU) {
        if (0 > snprintf_s(reboot_core, (unsigned long)reboot_core_len, (unsigned long)(reboot_core_len - 1), "%s",
                           CCORE)) {
            bsp_debug("er\n");
        }
    } else if (core == DUMP_CPU_APP) {
        if (0 > snprintf_s(reboot_core, (unsigned long)reboot_core_len, (unsigned long)(reboot_core_len - 1), "%s",
                           ACORE)) {
            bsp_debug("er\n");
        }
    } else if (core == DUMP_CPU_NRCCPU) {
        if (0 > snprintf_s(reboot_core, (unsigned long)reboot_core_len, (unsigned long)(reboot_core_len - 1), "%s",
                           NRCCORE)) {
            bsp_debug("er\n");
        }
    } else if (core == DUMP_CPU_NRL2HAC) {
        if (0 > snprintf_s(reboot_core, (unsigned long)reboot_core_len, (unsigned long)(reboot_core_len - 1), "%s",
                           NRL2HACORE)) {
            bsp_debug("er\n");
        }
    } else {
        if (0 > snprintf_s(reboot_core, (unsigned long)reboot_core_len, (unsigned long)(reboot_core_len - 1), "%s",
                           UNKNOW_CORE)) {
            bsp_debug("er\n");
        }
    }
    return;
}

void dump_apr_show_int_info(u32 type, dump_apr_info_s *dump_apr_info)
{
    u32 index = 0;
    if ((type == DUMP_SHOW_APR_CP_INT_TABLE) || (type == DUMP_SHOW_APR_CP_ALL_TABLE)) {
        if (unlikely(dump_apr_info->cp_interupt_table == NULL)) {
            dump_error("dump_apr_info->cp_interupt_table is NULL\n");
            return;
        }
        dump_print("interupt sum :0x%x\n", dump_apr_info->cp_interupt_table->intrupt_sum);
        for (index = 0; index < dump_apr_info->cp_interupt_table->intrupt_sum; index++) {
            dump_print("index:0x%-2x interrpt_id:0x%-8x module_name:%s\n", index,
                       dump_apr_info->cp_interupt_table->interupt_info[index].interrpt_id,
                       dump_apr_info->cp_interupt_table->interupt_info[index].module);
        }
    }
}

void dump_apr_show_task_info(u32 type, dump_apr_info_s *dump_apr_info)
{
    u32 index = 0;

    if ((type == DUMP_SHOW_APR_CP_TSK_TABLE) || (type == DUMP_SHOW_APR_CP_ALL_TABLE)) {
        if (unlikely(dump_apr_info->cp_task_table == NULL)) {
            dump_error("dump_apr_info->cp_task_table is NULL\n");
            return;
        }
        dump_print("task sum :0x%x\n", dump_apr_info->cp_task_table->task_sum);
        for (index = 0; index < dump_apr_info->cp_task_table->task_sum; index++) {
            dump_print("index:0x%-2x task:%-16s  module_name:%s\n", index,
                       dump_apr_info->cp_task_table->task_info[index].task_name,
                       dump_apr_info->cp_task_table->task_info[index].module);
        }
    }
}
void dump_apr_show_mod_info(u32 type, dump_apr_info_s *dump_apr_info)
{
    u32 index = 0;

    if ((type == DUMP_SHOW_APR_CP_TSK_TABLE) || (type == DUMP_SHOW_APR_CP_ALL_TABLE)) {
        if (unlikely(dump_apr_info->cp_task_table == NULL)) {
            dump_error("dump_apr_info->cp_task_table is NULL\n");
            return;
        }
        dump_print("task sum :0x%x\n", dump_apr_info->cp_task_table->task_sum);
        for (index = 0; index < dump_apr_info->cp_task_table->task_sum; index++) {
            dump_print("index:0x%-2x task:%-16s  module_name:%s\n", index,
                       dump_apr_info->cp_task_table->task_info[index].task_name,
                       dump_apr_info->cp_task_table->task_info[index].module);
        }
    }
}

void dump_show_apr_all_cfg(void)
{
    u32 i = 0;
    for (i = 0; i < NUM_OF_MODEM; i++) {
        dump_apr_show_int_info(DUMP_SHOW_APR_CP_ALL_TABLE, &(g_dump_apr_info[i]));
        dump_apr_show_task_info(DUMP_SHOW_APR_CP_ALL_TABLE, &(g_dump_apr_info[i]));
        dump_apr_show_mod_info(DUMP_SHOW_APR_CP_ALL_TABLE, &(g_dump_apr_info[i]));
    }
}
void dump_apr_get_reset_reason(u32 reason, u8 *reboot_reason, u32 reboot_reason_len, dump_apr_info_s *dump_apr_info)
{
    char *reason_str = NULL;
    if (reason == DUMP_REASON_NORMAL) {
        reason_str = NOARMAL;
    } else if (reason == DUMP_REASON_WDT) {
        reason_str = WDT;
    } else if (reason == DUMP_REASON_ARM) {
        reason_str = DATA_ABORT;
    } else if (reason == DUMP_REASON_NOC) {
        reason_str = NOC;
    } else if (reason == DUMP_REASON_DMSS) {
        reason_str = DMSS;
    } else if (reason == DUMP_REASON_RST_FAIL) {
        reason_str = RST_FAIL;
    } else if (reason == DUMP_REASON_RST_NOT_SUPPORT) {
        reason_str = RST_NOT_SUPPORT;
    } else if (reason == DUMP_REASON_UNDEF) {
        reason_str = UNDEFFINE;
    } else if (reason == DUMP_REASON_DLOCK) {
        reason_str = DLOCK;
    } else {
        reason_str = UNKNOW;
    }

    if ((reboot_reason_len >= 1) && (0 > snprintf_s((char *)reboot_reason, (unsigned long)reboot_reason_len,
                                                    (unsigned long)(reboot_reason_len - 1), "%s", reason_str))) {
        bsp_debug("er\n");
    }
}

void dump_apr_get_reset_brief(u32 voice, const u8 *module_name, u8 *brief, u32 brief_len)
{
    if (brief_len < 1) {
        bsp_debug("brief_len err\n");
        return;
    }

    if (voice == DUMP_OUT_VOICE) {
        if (0 > snprintf_s((char *)(brief), brief_len, (brief_len - 1), "%s", module_name)) {
            bsp_debug("er\n");
        }
    } else {
        if (0 > snprintf_s((char *)(brief), brief_len, (brief_len - 1), "%s_VOICE", module_name)) {
            bsp_debug("er\n");
        }
    }
}

void dump_get_exception_info(dump_except_info_s *dump_except_info, dump_exception_info_s *exception)
{
    dump_except_info->except_core = exception->core;
    dump_except_info->voice = exception->voice;
    dump_except_info->mod_id = exception->mod_id;
    dump_except_info->except_reason = exception->reason;

    dump_except_info->reboot_context = exception->reboot_contex;
    dump_except_info->reboot_task = exception->task_id;
    dump_except_info->reboot_int = exception->int_no;

    dump_print("except_core = 0x%x\n", dump_except_info->except_core);
    dump_print("voice = 0x%x\n", dump_except_info->voice);
    dump_print("mod_id = 0x%x\n", dump_except_info->mod_id);
    dump_print("except_reason = 0x%x\n", dump_except_info->except_reason);
    dump_print("reboot_int = 0x%x\n", dump_except_info->reboot_int);
    dump_print("reboot_context = 0x%x\n", dump_except_info->reboot_context);
    dump_print("reboot_task = 0x%x\n", dump_except_info->reboot_task);
}

void dump_apr_get_reset_info(char *reset_info, u32 size, dump_exception_info_s *exception,
                             dump_apr_info_s *dump_apr_info)
{
    dump_except_info_s dump_except_info = { 0 };
    dump_reset_log_s dump_reset_info = { { 0 } };
    if (unlikely(reset_info == NULL || size == 0 || exception == NULL)) {
        return;
    }

    dump_get_exception_info(&dump_except_info, exception);

    if (EOK != memcpy_s(dump_reset_info.task_name, sizeof(dump_reset_info.task_name), exception->task_name,
                        sizeof(exception->task_name))) {
        bsp_debug("err\n");
    }
    dump_print("task_name = %s\n", dump_reset_info.task_name);

    dump_reset_info.modid = dump_except_info.mod_id;
    dump_apr_get_reset_core(dump_except_info.except_core, (char *)(dump_reset_info.reboot_core),
                            sizeof(dump_reset_info.reboot_core), dump_apr_info);
    dump_apr_get_reset_reason(dump_except_info.except_reason, dump_reset_info.reboot_reson,
                              sizeof(dump_reset_info.reboot_reson), dump_apr_info);
    dump_apr_get_reset_voice(dump_except_info.voice, dump_reset_info.voice, sizeof(dump_reset_info.voice));
    dump_apr_get_reset_module(&dump_except_info, dump_reset_info.task_name, dump_reset_info.module_name,
                              sizeof(dump_reset_info.module_name), dump_apr_info);
    dump_apr_get_reset_brief(dump_except_info.voice, (const u8 *)dump_reset_info.module_name, dump_reset_info.brieftype,
                             sizeof(dump_reset_info.brieftype));

    /* 复位信息写入数据流 */
    if ((size >= 1) &&
        (snprintf_s(
             reset_info, (unsigned long)size, (unsigned long)(size - 1),
             "brief:%s\ncmpnt:%s\nreset_reason:%s\ntask_name:%s\nmodid:0x%x\nreboot_int:0x%x\nvoice:%s\nreset_core:%s\n",
             dump_reset_info.brieftype, dump_reset_info.module_name, dump_reset_info.reboot_reson,
             dump_reset_info.task_name, dump_reset_info.modid, dump_reset_info.reboot_int, dump_reset_info.voice,
             dump_reset_info.reboot_core) < 0)) {
        bsp_debug("er\n");
    }
}

void dump_save_apr_log(rdr_exc_info_s *rdr_exc_info)
{
    dump_exception_info_s *exception = NULL;
    u8 *temp_reset_info = NULL;
    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();
    char *dir_name = rdr_exc_info->log_path;
    dump_apr_info_s *dump_apr_info = NULL;
    void *apr_phy_addr = NULL;

    if (dump_get_product_type() == DUMP_MBB) {
        return;
    }

    if (unlikely(rdr_exc_info == NULL)) {
        return;
    }

    exception = (dump_exception_info_s *)dump_get_current_excpiton_info(rdr_exc_info->modid);
    if (unlikely(exception == NULL)) {
        return;
    }

    temp_reset_info = g_dump_apr_addr.virt_addr;
    if (temp_reset_info == NULL) {
        return;
    }
    apr_phy_addr = g_dump_apr_addr.phy_addr;

    if (EOK != memset_s(temp_reset_info, DUMP_ARP_LOG_BUF_LEN, 0, DUMP_ARP_LOG_BUF_LEN)) {
        dump_debug("err\n");
    }

    if (exception->core == DUMP_CPU_NRCCPU || exception->core == DUMP_CPU_NRL2HAC) {
        dump_apr_info = &(g_dump_apr_info[DUMP_NR_MODEM]);
    } else {
        dump_apr_info = &(g_dump_apr_info[DUMP_LR_MODEM]);
    }
    if (cfg->file_list.file_bits.reset_log == 1) {
        dump_apr_get_reset_info(temp_reset_info, DUMP_ARP_LOG_BUF_LEN, exception, dump_apr_info);
        dump_log_save(dir_name, "reset.log", temp_reset_info, (void *)apr_phy_addr, strlen(temp_reset_info));
        dump_print("reset.log save finished\n");
    }
}

EXPORT_SYMBOL(dump_show_apr_all_cfg);
