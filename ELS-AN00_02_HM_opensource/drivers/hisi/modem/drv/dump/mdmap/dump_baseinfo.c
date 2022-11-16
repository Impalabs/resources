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
#include "securec.h"
#include <linux/sched.h>
#include "bsp_dump.h"
#include "bsp_slice.h"
#include "dump_config.h"
#include "dump_baseinfo.h"

#undef THIS_MODU
#define THIS_MODU mod_dump
dump_base_info_s *g_mdm_dump_base_info = NULL;

void dump_save_int_contex_info(void)
{
    if (g_mdm_dump_base_info == NULL) {
        return;
    }
    g_mdm_dump_base_info->reboot_task = (u32)(-1);
    if (EOK != memset_s(g_mdm_dump_base_info->task_name, sizeof(g_mdm_dump_base_info->task_name), 0,
                        sizeof(g_mdm_dump_base_info->task_name))) {
        bsp_debug("err\n");
    }

    if (DUMP_MBB == dump_get_product_type()) {
    } else {
        g_mdm_dump_base_info->reboot_int = 0xAAAABBBB;
    }
    g_mdm_dump_base_info->reboot_context = DUMP_CTX_INT;
}
void dump_save_task_contex_info(void)
{
    struct task_struct *task = NULL;

    if (g_mdm_dump_base_info == NULL) {
        return;
    }

    g_mdm_dump_base_info->reboot_task_tcb = (u32)(uintptr_t)current;
    if (g_mdm_dump_base_info->mod_id == 0x11000025 || g_mdm_dump_base_info->mod_id == 0x1100002A) {
        /* A核VOS只记录的任务的pid */
        g_mdm_dump_base_info->reboot_task_tcb = g_mdm_dump_base_info->arg1;
        task = find_task_by_vpid(g_mdm_dump_base_info->arg1);
    } else {
        g_mdm_dump_base_info->reboot_task = (u32)(((struct task_struct *)(current))->pid);
        task = (struct task_struct *)(current);
    }
    if (task != NULL) {
        if (EOK != memset_s(g_mdm_dump_base_info->task_name, sizeof(g_mdm_dump_base_info->task_name), 0,
                            sizeof(g_mdm_dump_base_info->task_name))) {
            bsp_debug("err\n");
        }
        if (EOK != memcpy_s(g_mdm_dump_base_info->task_name, sizeof(g_mdm_dump_base_info->task_name),
                            ((struct task_struct *)(task))->comm, strlen(((struct task_struct *)(task))->comm))) {
            bsp_debug("err\n");
        }
        dump_print("exception task name is %s\n", g_mdm_dump_base_info->task_name);
    }
    g_mdm_dump_base_info->reboot_int = (u32)(-1);
    g_mdm_dump_base_info->reboot_context = DUMP_CTX_TASK;
}

void dump_save_exc_contex(void)
{
    if (g_mdm_dump_base_info == NULL) {
        return;
    }

    if (g_mdm_dump_base_info->mod_id == BSP_MODU_OTHER_CORE) {
        g_mdm_dump_base_info->reboot_task = (u32)(-1);
        g_mdm_dump_base_info->reboot_int = (u32)(-1);
    } else {
        g_mdm_dump_base_info->reboot_time = bsp_get_slice_value();

        if (in_interrupt()) {
            dump_save_int_contex_info();
        } else {
            dump_save_task_contex_info();
        }
    }

    return;
}

void dump_save_base_info(u32 mod_id, u32 arg1, u32 arg2, const char *data, u32 length, const char *sence)
{
    u32 len = 0;
    if (unlikely(g_mdm_dump_base_info == NULL)) {
        return;
    }

    g_mdm_dump_base_info->mod_id = mod_id;
    g_mdm_dump_base_info->arg1 = arg1;
    g_mdm_dump_base_info->arg2 = arg2;
    g_mdm_dump_base_info->arg3 = (u32)(uintptr_t)data;
    g_mdm_dump_base_info->arg3_length = length;
    g_mdm_dump_base_info->reboot_time = bsp_get_slice_value();

    if (unlikely(sence != NULL)) {
        len = strlen(sence);
        if (EOK != memcpy_s(g_mdm_dump_base_info->task_name, sizeof(g_mdm_dump_base_info->task_name), sence,
                            len > DUMP_BASEINFO_SENCE_LEN ? DUMP_BASEINFO_SENCE_LEN : len)) {
            bsp_debug("err\n");
        }
        g_mdm_dump_base_info->reboot_context = DUMP_CTX_TASK;
        g_mdm_dump_base_info->reboot_int = 0xFFFFFFFF;
    } else {
        dump_save_exc_contex();
    }
    dump_print("input modid = 0x%x\n", mod_id);

    return;
}

s32 dump_base_info_init(void)
{
    g_mdm_dump_base_info = (dump_base_info_s *)bsp_dump_register_field(DUMP_MODEMAP_BASE_INFO_SMP, "MDMAP_BASE_INFO",
                                                                       sizeof(dump_base_info_s), 0);
    if (g_mdm_dump_base_info == NULL) {
        return BSP_ERROR;
    }

    if (EOK != memset_s(g_mdm_dump_base_info, sizeof(dump_base_info_s), 0, sizeof(dump_base_info_s))) {
        bsp_debug("err\n");
    }
    g_mdm_dump_base_info->vec = 0xff;
    dump_print("base info init ok\n");
    return BSP_OK;
}
