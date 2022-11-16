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

#include <linux/slab.h>
#include <linux/rtc.h>
#include <linux/timer.h>
#include "securec.h"
#include "osl_types.h"
#include "osl_thread.h"
#include "osl_sem.h"
#include "bsp_dump_mem.h"
#include "bsp_dump.h"
#include "bsp_slice.h"
#include "dump_config.h"
#include "dump_area.h"
#include "dump_exc_handle.h"
#include "dump_logs.h"
#include "dump_boot_check.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

struct dump_resave_info g_dump_boot_check_info;

s32 dump_get_reboot_save_path(u8 **path, u32 *path_len)
{
    u32 len;
    if (path == NULL || path_len == NULL) {
        dump_error("err param\n");
        return BSP_ERROR;
    }
    *path = rdr_get_reboot_logpath();
    if (*path == NULL) {
        dump_error("rdr path is null\n");
        return BSP_ERROR;
    }
    len = strlen(*path);
    if (len == 0) {
        dump_error("path vail len is 0\n");
        return BSP_ERROR;
    }
    *path_len = len;
    dump_print("get path from rdr ok\n");
    return BSP_OK;
}

void dump_create_resave_dir(u8 *path, u32 len)
{
    s32 ret = 0;
    char *dst_path = g_dump_boot_check_info.dump_backup_path;
    if (path[len - 1] == '/') {
        ret = snprintf_s(g_dump_boot_check_info.dump_backup_path, sizeof(g_dump_boot_check_info.dump_backup_path),
                         (sizeof(g_dump_boot_check_info.dump_backup_path) - 1), "%s%s", path, RDR_DUMP_FILE_CP_PATH);
    } else {
        ret = snprintf_s(g_dump_boot_check_info.dump_backup_path, sizeof(g_dump_boot_check_info.dump_backup_path),
                         (sizeof(g_dump_boot_check_info.dump_backup_path) - 1), "%s/%s", path, RDR_DUMP_FILE_CP_PATH);
    }
    if (ret != 0) {
        dump_debug("er\n");
    }
    dump_print("resave path is %s\n", g_dump_boot_check_info.dump_backup_path);
    if (BSP_OK != dump_create_dir(dst_path)) {
        dump_error("creat dump_backup_path dir error\n");
    }
}

int dump_save_reboot_task(void *data)
{
    u32 i = 0;
    u8 *path = NULL;
    u32 len = 0;
    for (;;) {
        down(&g_dump_boot_check_info.sem_save_task);
        dump_error("enter resave log task\n");
        if (!is_reboot_reason_from_modem()) {
            dump_error("last reboot is not modem\n");
            continue;
        }
        for (i = 0; i < DUMP_SAVE_REBOOT_TASK_WAIT_CNTS; i++) {
            if (dump_get_reboot_save_path(&path, &len) != BSP_OK) {
                msleep(DUMP_SAVE_REBOOT_TASK_WAIT_PER_CNT);
                continue;
            }
            if (path == NULL || len == 0) {
                dump_print("wait rdr path ready\n");
                msleep(DUMP_SAVE_REBOOT_TASK_WAIT_PER_CNT);
                continue;
            }
            dump_create_resave_dir(path, len);

            if (g_dump_boot_check_info.dump_backup_buf != NULL && g_dump_boot_check_info.dump_backup_len != 0) {
                dump_log_save(g_dump_boot_check_info.dump_backup_path, "modem_dump.bin",
                              g_dump_boot_check_info.dump_backup_buf, NULL, g_dump_boot_check_info.dump_backup_len);
            }
            if (g_dump_boot_check_info.dump_backup_buf != NULL) {
                vfree(g_dump_boot_check_info.dump_backup_buf);
                g_dump_boot_check_info.dump_backup_buf = NULL;
            }
            dump_print("dump resave log ok");
            break;
        }
        if (i == DUMP_SAVE_REBOOT_TASK_WAIT_CNTS) {
            dump_error("rdr resave path not ready\n");
        }
    }
}

s32 dump_creat_resave_task(void)
{
    OSL_TASK_ID pid;

    osl_sem_init(0, &g_dump_boot_check_info.sem_save_task);
    if (osl_task_init("save_modem_bootlog", DUMP_SAVE_REBOOT_TASK_PRIO, 0x2000, (OSL_TASK_FUNC)dump_save_reboot_task,
                      NULL, &pid)) {
        dump_error("creat save_modem_bootlog task error!\n");
        return BSP_ERROR;
    }
    dump_print("creat resave task ok\n");
    return BSP_OK;
}

s32 dump_back_mntn_bin(void)
{
    char *dump_addr = NULL;
    u32 dump_len;
    dump_addr = dump_get_mntn_base_addr();
    dump_len = dump_get_mntn_length();
    if (dump_addr != NULL && dump_len != 0) {
        g_dump_boot_check_info.dump_backup_buf = vmalloc(dump_len);
        g_dump_boot_check_info.dump_backup_len = dump_len;
        if (g_dump_boot_check_info.dump_backup_buf != NULL) {
            if (EOK != memset_s(g_dump_boot_check_info.dump_backup_buf, g_dump_boot_check_info.dump_backup_len, 0,
                                g_dump_boot_check_info.dump_backup_len)) {
                dump_debug("err\n");
            }
            if (EOK != memcpy_s(g_dump_boot_check_info.dump_backup_buf, g_dump_boot_check_info.dump_backup_len,
                                dump_addr, dump_len)) {
                dump_debug("err\n");
            }
            dump_error("back up dump ok");
            return BSP_OK;
        }
    }
    return BSP_ERROR;
}

void dump_fill_reboot_excpetion_info(void)
{
    struct timex txc = {0};
    struct rtc_time tm = {0};
    char temp[DUMP_DATETIME_LEN] = {0};
    struct dump_global_base_info_s *global_base_info = (struct dump_global_base_info_s *)dump_get_global_baseinfo();

    if (unlikely(global_base_info == NULL)) {
        return;
    }
    global_base_info->modid = RDR_PHONE_MDMAP_PANIC_MOD_ID;
    do_gettimeofday(&(txc.time));
    rtc_time_to_tm((unsigned long)(txc.time.tv_sec), &tm);
    if (snprintf_s(temp, sizeof(temp), (sizeof(temp) - 1), "%04d-%02d-%02d %02d:%02d:%02d",
                   tm.tm_year + DUMP_LOG_YEAR_START, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec) < 0) {
        bsp_debug("er\n");
    }
    if (EOK != memcpy_s(global_base_info->datetime, sizeof(global_base_info->datetime), temp, sizeof(temp))) {
        bsp_debug("err\n");
    }
    if (EOK != memcpy_s((void *)(global_base_info->e_desc), sizeof(global_base_info->e_desc), DUMP_PHONE_MDMAP_PANIC,
                        (u32)(strlen((const char *)(DUMP_PHONE_MDMAP_PANIC)) < sizeof(global_base_info->e_desc)
                                    ? strlen((const char *)(DUMP_PHONE_MDMAP_PANIC))
                                    : sizeof(global_base_info->e_desc)))) {
        bsp_debug("err\n");
    }
}

s32 bsp_dump_resave_modem_log(void)
{
    s32 ret;
    if (memset_s(&g_dump_boot_check_info, sizeof(g_dump_boot_check_info), 0, sizeof(g_dump_boot_check_info)) != EOK) {
        dump_debug("err\n");
    }

    dump_reset_area_info();

    dump_map_mntn_addr();

    if (dump_get_product_type() != DUMP_PHONE) {
        return BSP_OK;
    }
    if (!is_reboot_reason_from_modem()) {
        return BSP_OK;
    }

    dump_fill_reboot_excpetion_info();
    dump_print("enter reboot step\n");
    ret = dump_back_mntn_bin();
    if (ret != BSP_OK) {
        return BSP_ERROR;
    }
    ret = dump_creat_resave_task();
    if (ret == BSP_OK) {
        dump_print("triger resave task\n");
        up(&g_dump_boot_check_info.sem_save_task);
        return BSP_OK;
    }
    if (g_dump_boot_check_info.dump_backup_buf != NULL) {
        vfree(g_dump_boot_check_info.dump_backup_buf);
        g_dump_boot_check_info.dump_backup_buf = NULL;
    }
    return BSP_OK;
}

#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
core_initcall(bsp_dump_resave_modem_log);
#endif
