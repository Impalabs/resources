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

#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/rtc.h>
#include <linux/thread_info.h>
#include <linux/syslog.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/kdebug.h>
#include <linux/reboot.h>
#include <linux/slab.h>
#include <bsp_dt.h>
#include <linux/delay.h>
#include <asm/string.h>
#include <asm/traps.h>
#include "product_config.h"
#include <linux/syscalls.h>
#include "osl_types.h"
#include "osl_io.h"
#include "osl_bio.h"
#include "osl_malloc.h"
#include "bsp_dump.h"
#include "bsp_ipc.h"
#include "bsp_memmap.h"
#include "bsp_wdt.h"
#include "bsp_icc.h"
#include "bsp_onoff.h"
#include "bsp_nvim.h"
#include "bsp_softtimer.h"
#include "bsp_version.h"
#include "bsp_sram.h"
#include "bsp_dump_mem.h"
#include "bsp_dump.h"
#include "bsp_coresight.h"
#include "bsp_reset.h"
#include "nv_stru_drv.h"
#include "mdrv_om.h"
#include <gunas_errno.h>
#include "bsp_adump.h"
#include "bsp_wdt.h"
#include "dump_config.h"
#include "dump_baseinfo.h"
#include "dump_apr.h"
#include "dump_area.h"
#include "dump_exc_handle.h"
#include "dump_logs.h"
#include "dump_sec_mem.h"
#include "nrrdr_agent.h"
#include "nrrdr_logs.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

/*
 * 功能描述: mdmcp 功能初始化
 */
__init s32 dump_nrrdr_init(void)
{
    s32 ret;

    ret = nrrdr_agent_init();
    if (ret != BSP_OK) {
        dump_error("nrrdr_agent_init fail\n");
        return BSP_ERROR;
    }
#ifdef CONFIG_MODEM_MINI_DUMP
#endif
    dump_print("dump_nrrdr_init ok\n");

    return BSP_OK;
}

s32 dump_nr_reset(u32 modid)
{
    return BSP_OK;
}

/*
 * 函 数 名  : dump_nr_callback
 * 功能描述  : nr的回调函数
 * 输入参数  :
 * 输出参数  :
 * 返 回 值  :
 * 修改记录  : 2016年1月4日17:05:33     creat
 */
#ifdef CONFIG_MODEM_MINI_DUMP
s32 dump_nr_callback(u32 modid, u32 etype, u64 coreid, char *logpath, pfn_cb_dump_done fndone)
{
    return dump_notify_nr(modid, etype, coreid, logpath, fndone);
}
#endif
