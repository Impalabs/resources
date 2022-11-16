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
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include "osl_types.h"
#include "bsp_sysctrl.h"
#include "bsp_slice.h"
#include "bsp_wdt.h"
#include "bsp_ipc.h"
#include "bsp_fiq.h"
#include "bsp_coresight.h"
#include "bsp_dump.h"
#include "bsp_adump.h"
#include "bsp_ddr.h"
#include "bsp_slice.h"
#include "bsp_noc.h"
#include "dump_area.h"
#include "dump_cp_wdt.h"
#include "dump_config.h"
#include "dump_baseinfo.h"
#include "dump_lphy_tcm.h"
#include "dump_cphy_tcm.h"
#include "dump_sec_mem.h"
#include "dump_exc_handle.h"
#include "dump_cp_agent.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

#define DUMP_CP_CPUINFO_START DUMP_LRCCPU_CPUINFO

s32 dump_get_mdmcp_save_done(void)
{
    s32 flag;
    struct dump_area_mntn_addr_info_s area_info = {
        NULL,
    };
    struct dump_area_s *dump_area_info = NULL;

    flag = dump_get_area_info(DUMP_AREA_LR, &area_info);
    if (unlikely(flag == BSP_ERROR || area_info.vaddr == NULL)) {
        dump_error("fail to find cp area head\n");
        return BSP_ERROR;
    }

    dump_area_info = ((struct dump_area_s *)(area_info.vaddr));
    /*lint -e650 -esym(650,*)*/
    flag = dump_area_info->area_head.share_info.done_flag;
    if (flag == DUMP_SAVE_SUCCESS) {
        return BSP_OK;
    }
    /*lint -e650 +esym(650,*)*/
    return BSP_ERROR;
}

void dump_clear_cpboot_area(void)
{
#ifdef MNTN_AREA_CBOOT_ADDR
    uintptr_t offset, base;
    offset = MNTN_AREA_CBOOT_ADDR - MNTN_BASE_ADDR;
    base = (uintptr_t)dump_get_mntn_base_addr();
    if (base == 0) {
        return;
    }
    if (EOK != memset_s((void *)(uintptr_t)(base + offset), MNTN_AREA_CBOOT_SIZE, 0, MNTN_AREA_CBOOT_SIZE)) {
        bsp_debug("err\n");
    }
#endif
}

void dump_save_mdmboot_info(void)
{
#ifdef MNTN_AREA_CBOOT_ADDR
    u8 *addr = NULL;
    uintptr_t offset, base;
    offset = MNTN_AREA_CBOOT_ADDR - MNTN_BASE_ADDR;
    base = (uintptr_t)dump_get_mntn_base_addr();
    if (base == 0) {
        return;
    }
    addr = bsp_dump_register_field(DUMP_MODEMAP_CBOOT, "boot_info", MNTN_AREA_CBOOT_SIZE, 0);
    if (addr != NULL) {
        if (EOK != memcpy_s(addr, MNTN_AREA_CBOOT_SIZE, (void *)(uintptr_t)(base + offset), MNTN_AREA_CBOOT_SIZE)) {
            dump_debug("err\n");
        }
    }
#endif
}

void dump_get_cp_task_name_by_id(u32 task_id, char *task_name, u32 task_name_len)
{
    dump_task_name_s *temp_task_name = NULL;
    dump_cp_queue_s *task_name_table = NULL;
    uintptr_t addr;
    u32 task_cnt;
    u32 task_index = 0;

    task_name_table = (dump_cp_queue_s *)bsp_dump_get_field_addr(DUMP_LRCCPU_ALLTASK_NAME);
    if (unlikely(task_name_table == NULL)) {
        dump_error("fail to get cp task name field\n");
        return;
    }

    task_cnt = task_name_table->maxNum / DUMP_CP_QUEUE_NODE_SIZE;
    addr = (uintptr_t)task_name_table;

    addr += sizeof(dump_cp_queue_s);
    dump_print(" sizeof(dump_cp_queue_s) = 0x%x\n", (u32)sizeof(dump_cp_queue_s));
    temp_task_name = (dump_task_name_s *)(uintptr_t)addr;

    for (task_index = 0; task_index < task_cnt; task_index++) {
        if (temp_task_name[task_index].task_id == task_id) {
            if (EOK != memcpy_s(task_name, task_name_len, temp_task_name[task_index].task_name,
                                sizeof(temp_task_name[task_index].task_name))) {
                bsp_debug("err\n");
            }
            dump_print("reboot task: task_id=0x%x,task_name=%s\n", temp_task_name[task_index].task_id,
                       temp_task_name[task_index].task_name);
            break;
        }
    }
}

void dump_save_cp_baseinfo_single_core(dump_base_info_s *modem_cp_base_info)
{
    dump_cpu_info_s *modem_cp_cpuinfo = NULL;

    dump_print("modem cp is single core \n");
    modem_cp_base_info->reboot_cpu = 0;

    modem_cp_cpuinfo = (dump_cpu_info_s *)bsp_dump_get_field_addr(DUMP_CP_CPUINFO_START);
    if (unlikely(modem_cp_cpuinfo == NULL)) {
        dump_error("fail to get modem_cp_cpuinfo\n");
        return;
    }

    if ((modem_cp_cpuinfo->current_int & 0xFFFF0000) == DUMP_FLAG_INT_ENTER) {
        modem_cp_cpuinfo->current_ctx = DUMP_CTX_INT;
    } else if ((modem_cp_cpuinfo->current_int & 0xFFFF0000) == DUMP_FLAG_INT_EXIT) {
        modem_cp_cpuinfo->current_ctx = DUMP_CTX_TASK;
    }
    modem_cp_cpuinfo->current_int &= 0xFFFF;

    if (modem_cp_cpuinfo->current_ctx == DUMP_CTX_TASK) {
        modem_cp_base_info->reboot_task = modem_cp_cpuinfo->current_task;
        dump_get_cp_task_name_by_id(modem_cp_base_info->reboot_task, (char *)(modem_cp_base_info->task_name),
                                    sizeof(modem_cp_base_info->task_name));
        modem_cp_base_info->reboot_int = (u32)(-1);
        modem_cp_base_info->reboot_context = DUMP_CTX_TASK;
    } else {
        modem_cp_base_info->reboot_task = (u32)(-1);
        modem_cp_base_info->reboot_int = modem_cp_cpuinfo->current_int;
        modem_cp_base_info->reboot_context = DUMP_CTX_INT;
    }
}
void dump_save_cp_baseinfo_multi_core(dump_base_info_s *modem_cp_base_info)
{
    dump_cp_reboot_contex_s *reboot_contex = NULL;
    dump_cpu_info_s *modem_cp_cpuinfo = NULL;
    u8 *addr = NULL;
    u32 i;

    dump_print("modem has %d core \n", modem_cp_base_info->cpu_max_num);
    modem_cp_base_info->reboot_cpu = 0xe;
    addr = (u8 *)bsp_dump_get_field_addr(DUMP_LRCCPU_REBOOTCONTEX);
    if (unlikely(addr == NULL)) {
        dump_error("fail to get cp reboot field\n");
        return;
    }

    for (i = 0; i < modem_cp_base_info->cpu_max_num; i++) {
        modem_cp_cpuinfo = (dump_cpu_info_s *)bsp_dump_get_field_addr(DUMP_CP_CPUINFO_START + i);
        if (unlikely(modem_cp_cpuinfo == NULL)) {
            dump_error("fail to get modem_cp_cpuinfo field\n");
            return;
        }
        if ((modem_cp_cpuinfo->current_int & 0xFFFF0000) == DUMP_FLAG_INT_ENTER) {
            modem_cp_cpuinfo->current_ctx = DUMP_CTX_INT;
        } else if ((modem_cp_cpuinfo->current_int & 0xFFFF0000) == DUMP_FLAG_INT_EXIT) {
            modem_cp_cpuinfo->current_ctx = DUMP_CTX_TASK;
        }
        modem_cp_cpuinfo->current_int &= 0xFFFF;
        reboot_contex = (dump_cp_reboot_contex_s *)((uintptr_t)(addr) + i * sizeof(dump_cp_reboot_contex_s));
        if (modem_cp_cpuinfo->current_ctx == DUMP_CTX_TASK) {
            reboot_contex->reboot_context = DUMP_CTX_TASK;
            reboot_contex->reboot_task = modem_cp_cpuinfo->current_task;
            dump_get_cp_task_name_by_id(reboot_contex->reboot_task, (char *)(reboot_contex->task_name),
                                        sizeof(modem_cp_base_info->task_name));
            reboot_contex->reboot_int = (u32)(-1);
        } else {
            reboot_contex->reboot_task = (u32)(-1);
            reboot_contex->reboot_int = modem_cp_cpuinfo->current_int;
            reboot_contex->reboot_context = DUMP_CTX_INT;
        }
        reboot_contex->task_name[DUMP_NAME_LEN - 1] = '\0';

        dump_print("reboot_context = 0x%x", reboot_contex->reboot_context);
        dump_print("reboot_int = 0x%x", reboot_contex->reboot_int);
        dump_print("reboot_task = 0x%x", reboot_contex->reboot_task);
        dump_print("taskname is %s", reboot_contex->task_name);
    }
}

void dump_save_cp_base_info(u32 mod_id, u32 arg1, u32 arg2, const char *data, u32 length)
{
    dump_base_info_s *modem_cp_base_info = NULL;
    u8 *addr = NULL;

    addr = bsp_dump_get_field_addr(DUMP_LRCCPU_BASE_INFO_SMP);
    if (unlikely(addr == NULL)) {
        dump_error("fail to get cp base info\n");
        return;
    }
    modem_cp_base_info = (dump_base_info_s *)addr;

    modem_cp_base_info->mod_id = mod_id;
    modem_cp_base_info->arg1 = arg1;
    modem_cp_base_info->arg2 = arg2;
    modem_cp_base_info->arg3 = (u32)(uintptr_t)data;
    modem_cp_base_info->arg3_length = length;
    modem_cp_base_info->reboot_time = bsp_get_slice_value();

    if (modem_cp_base_info->cpu_max_num == 1) {
        dump_save_cp_baseinfo_single_core(modem_cp_base_info);
    } else {
        dump_save_cp_baseinfo_multi_core(modem_cp_base_info);
    }

    dump_print("save cp base info ok\n");
    return;
}

s32 dump_wait_cp_save_done(u32 ms, bool block)
{
    u32 time_start;

    time_start = bsp_get_elapse_ms();

    do {
        if (BSP_OK == dump_get_mdmcp_save_done()) {
            dump_print("mdmcp save done\n");
            return BSP_OK;
        }

        if (ms <= (bsp_get_elapse_ms() - time_start)) {
            dump_error("dump wait mdmcp done time out\n");
            return BSP_ERROR;
        }

        if (block) {
            udelay(DUMP_UDELAY_1MS_NUM * 1);
        } else {
            msleep(DUMP_SLEEP_5MS_NUM);
        }
    } while (1);
    /*lint -e527 -esym(527,*)*/
    return BSP_ERROR;
    /*lint -e527 +esym(527,*)*/
}

void dump_notify_cp(u32 mod_id)
{
    s32 ret;
    dump_exception_info_s *exception = dump_get_current_excpiton_info(mod_id);
    if (unlikely(exception == NULL)) {
        return;
    }

    if (exception->core == DUMP_CPU_LRCCPU &&
        (exception->reason != DUMP_REASON_WDT && exception->reason != DUMP_REASON_DLOCK)) {
        dump_print("CP exception ,no need to notify C core 0x%x\n", mod_id);
    } else {
        ret = bsp_ipc_int_send(IPC_CORE_CCORE, IPC_CCPU_SRC_ACPU_DUMP);
        if (ret == BSP_OK) {
            dump_print("notify modem ccore success \n");
        } else {
            dump_error("notify modem ccore fail,please let ipc check \n");
        }
    }
}

void dump_cp_wdt_dlock_handle(u32 mod_id, u32 arg1, u32 arg2, char *data, u32 length)
{
    u32 reason;
    char *desc = NULL;
    dump_base_info_s *modem_cp_base_info = NULL;
    dump_exception_info_s exception_info_s = {
        0,
    };

    dump_print("[0x%x]modem cp wdt or pdlock enter system error! \n", bsp_get_slice_value());
    dump_print("mod_id=0x%x arg1=0x%x arg2=0x%x len=0x%x\n", mod_id, arg1, arg2, length);

    bsp_coresight_stop_cp();

    dump_save_cp_base_info(mod_id, arg1, 0, NULL, 0);

    modem_cp_base_info = (dump_base_info_s *)bsp_dump_get_field_addr(DUMP_LRCCPU_BASE_INFO_SMP);
    if (modem_cp_base_info == NULL) {
        dump_error("modem_cp_base_info is NULL\n");
        return;
    }

    if (arg1 == DUMP_REASON_WDT) {
        desc = "Modem CP WDT";
    } else if (arg1 == DUMP_REASON_DLOCK) {
        desc = "Modem CP DLOCK";
    }
    reason = arg1;
    modem_cp_base_info->reboot_reason = reason;
    dump_fill_excption_info(&exception_info_s, mod_id, arg1, arg2, NULL, 0, DUMP_CPU_LRCCPU, reason, desc,
                            modem_cp_base_info->reboot_context, modem_cp_base_info->reboot_task,
                            modem_cp_base_info->reboot_int, modem_cp_base_info->task_name);

    dump_register_exception(&exception_info_s);
}

void dump_int_handler(void)
{
    dump_base_info_s *modem_cp_base_info = NULL;
    dump_reboot_reason_e reboot_reason;
    dump_exception_info_s exception_info_s = { 0, DUMP_REASON_NORMAL };
    char *desc = NULL;
    u8 task_name[DUMP_NAME_LEN] = {0};

    dump_print("modem ccore enter system error! timestamp:0x%x\n", bsp_get_slice_value());

    modem_cp_base_info = (dump_base_info_s *)bsp_dump_get_field_addr(DUMP_LRCCPU_BASE_INFO_SMP);
    if (modem_cp_base_info == NULL) {
        dump_error("modem_cp_base_info is NULL\n");
        return;
    }

    reboot_reason = modem_cp_base_info->mod_id != DRV_ERRNO_DUMP_ARM_EXC ? DUMP_REASON_NORMAL : DUMP_REASON_ARM;

    if (reboot_reason == DUMP_REASON_NORMAL) {
        desc = "Modem CP SYSERROR";
    } else if (reboot_reason == DUMP_REASON_ARM) {
        desc = "Modem CP ABORT";
    }

    dump_get_cp_task_name_by_id(modem_cp_base_info->reboot_task, task_name, sizeof(task_name));

    dump_fill_excption_info(&exception_info_s, modem_cp_base_info->mod_id, modem_cp_base_info->arg1,
                            modem_cp_base_info->arg2, NULL, 0, DUMP_CPU_LRCCPU, reboot_reason, desc,
                            modem_cp_base_info->reboot_context, modem_cp_base_info->reboot_task,
                            modem_cp_base_info->reboot_int, task_name);

    dump_register_exception(&exception_info_s);

    return;
}

void dump_fusion_agent_handle(void *arg)
{
    dump_int_handler();
}

void dump_cp_agent_handle(u32 arg)
{
    dump_int_handler();
}

void dump_cp_timeout_proc(bool block)
{
    s32 ret;
    ret = bsp_send_cp_fiq(FIQ_DUMP);
    if (ret == BSP_ERROR) {
        dump_error("fail to send fiq\n");
        return;
    } else {
        dump_print("trig fiq process success\n");
    }
    ret = dump_wait_cp_save_done(DUMP_WAIT_15S_NUM, block);
    if (ret == BSP_ERROR) {
        dump_error("ipc fiq save log both fail\n");
    } else {
        dump_print("fiq save log success\n");
    }
}

s32 dump_wait_mdmcp_done(bool block)
{
    s32 ret;

    dump_print("begin to wait cp log save done\n");

    ret = dump_wait_cp_save_done(DUMP_WAIT_15S_NUM, block);
    if (ret == BSP_ERROR) {
        dump_cp_timeout_proc(block);
    }
    return BSP_OK;
}

void dump_cp_wdt_hook(unsigned int mod_id)
{
    dump_cp_wdt_dlock_handle(mod_id, DUMP_REASON_WDT, 0, 0, 0);
}

__init s32 dump_cp_agent_init(void)
{
    int ret;
    ret = bsp_ipc_int_connect(IPC_ACPU_SRC_CCPU_DUMP, (voidfuncptr)dump_cp_agent_handle, 0);
    if (unlikely(ret != BSP_OK)) {
        dump_error("fail to connect ipc int\n");
        return BSP_ERROR;
    }

    ret = bsp_ipc_int_enable(IPC_ACPU_SRC_CCPU_DUMP);
    if (unlikely(ret != BSP_OK)) {
        dump_error("fail to enbale ipc int\n");
        return BSP_ERROR;
    }
    ret = bsp_wdt_register_hook(WDT_CCORE_ID, dump_cp_wdt_hook);
    if (unlikely(ret != BSP_OK)) {
        dump_error("fail to register wdt hook\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}
