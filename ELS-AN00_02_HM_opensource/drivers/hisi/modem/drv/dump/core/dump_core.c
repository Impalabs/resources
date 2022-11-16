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
#include "securec.h"
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include "mdrv_om.h"
#include <bsp_cold_patch.h>
#include "osl_types.h"
#include "osl_thread.h"
#include "bsp_version.h"
#include "bsp_sram.h"
#include "osl_sem.h"
#include "bsp_dump_mem.h"
#include "bsp_coresight.h"
#include "bsp_nvim.h"
#include "bsp_dump.h"
#include "bsp_fiq.h"
#include "bsp_slice.h"
#include "nv_stru_drv.h"
#include "dump_config.h"
#include "dump_logs.h"
#include "dump_apr.h"
#include "dump_exc_handle.h"
#include "dump_cp_agent.h"
#include "dump_area.h"
#include "dump_cp_core.h"
#include "dump_mdmap_core.h"
#include "dump_sec_mem.h"
#include "nrrdr_agent.h"
#include "dump_cp_logs.h"
#include "dump_core.h"
#include "dump_logzip.h"
#include "nrrdr_core.h"
#include "dump_m3_agent.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

modem_dump_ctrl_s g_dump_ctrl[EXC_INFO_BUTT];
u32 g_dump_init_phase = DUMP_INIT_FLAG_CONFIG;
u32 g_dump_version;

void dump_set_verion(void)
{
    g_dump_version = DUMP_VERSON;
}

u32 dump_get_init_phase(void)
{
    return g_dump_init_phase;
}

void dump_set_init_phase(u32 phase)
{
    g_dump_init_phase = phase;
}

u32 dump_fill_rdr_exc_core(u32 core)
{
    u32 rdr_core;

    switch (core) {
        case DUMP_CPU_LRCCPU:
            rdr_core = RDR_CP;
            break;
        case DUMP_CPU_APP:
            rdr_core = RDR_MODEMAP;
            break;
        case DUMP_CPU_MDMM3:
            rdr_core = RDR_LPM3;
            break;
        default:
            rdr_core = RDR_CP;
    }
    return rdr_core;
}
void dump_save_global_baseinfo(struct dump_global_base_info_s *global_base_info, dump_exception_info_s *excption_node,
                               struct rdr_exception_info_s *rdr_exc_info)
{
    struct timex txc = {0};
    struct rtc_time tm = {0};
    char temp[DUMP_DATETIME_LEN] = {0};
    struct dump_global_top_head_s *top_head = (struct dump_global_top_head_s *)dump_get_rdr_top_head();
    do_gettimeofday(&(txc.time));
    rtc_time_to_tm((unsigned long)(txc.time.tv_sec), &tm);
    if (snprintf_s(temp, sizeof(temp), (sizeof(temp) - 1), "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year +
                   DUMP_LOG_YEAR_START, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec) < 0) {
        bsp_debug("er\n");
    }

    if (memset_s(global_base_info, sizeof(*global_base_info), 0, sizeof(*global_base_info)) != EOK) {
        bsp_debug("err\n");
    }
    global_base_info->modid = rdr_exc_info->e_modid;
    global_base_info->arg1 = 0;
    global_base_info->arg2 = 0;
    if (memcpy_s(global_base_info->e_module, sizeof(global_base_info->e_module), rdr_exc_info->e_from_module,
                 (unsigned long)DUMP_MAX_MODULE_LEN) != EOK) {
        bsp_debug("err\n");
    }
    global_base_info->e_type = rdr_exc_info->e_exce_type;

    global_base_info->e_core = dump_fill_rdr_exc_core(excption_node->core);
    global_base_info->start_flag = DUMP_START_EXCH;
    global_base_info->savefile_flag = DUMP_SAVE_FILE_END;
    if (memset_s(global_base_info->e_desc, sizeof(global_base_info->e_desc), 0, sizeof(global_base_info->e_desc)) !=
        EOK) {
        bsp_debug("err\n");
    }
    if (EOK != memcpy_s((void *)(global_base_info->e_desc), sizeof(global_base_info->e_desc), rdr_exc_info->e_desc,
                        (u32)(strlen((const char *)(rdr_exc_info->e_desc)) < DUMP_MAX_DESC_LEN
                                    ? strlen((const char *)(rdr_exc_info->e_desc))
                                    : DUMP_MAX_DESC_LEN))) {
        bsp_debug("err\n");
    }
    if (memcpy_s(global_base_info->datetime, sizeof(global_base_info->datetime), temp,
                 (unsigned long)DUMP_DATETIME_LEN) != EOK) {
        bsp_debug("err\n");
    }
    global_base_info->timestamp = bsp_get_slice_value();
    if (top_head != NULL) {
        top_head->codepatch = bsp_modem_cold_patch_is_exist();
    }
}
void dump_save_balong_rdr_info(u32 mod_id)
{
    struct dump_global_base_info_s *global_base_info = NULL;
    struct rdr_exception_info_s *rdr_exc_info = NULL;
    dump_exception_info_s *excption_node = NULL;

    global_base_info = (struct dump_global_base_info_s *)dump_get_global_baseinfo();
    if (unlikely(global_base_info == NULL)) {
        dump_error("fail to get global_base_info\n");
        return;
    }

    rdr_exc_info = dump_get_exception_info_node(mod_id);
    if (unlikely(rdr_exc_info == NULL)) {
        dump_error("fail to get exception node\n");
        return;
    }
    excption_node = dump_get_current_excpiton_info(rdr_exc_info->e_modid);
    if (unlikely(excption_node == NULL)) {
        dump_error("fail to get current exception node\n");
        return;
    }

    dump_save_global_baseinfo(global_base_info, excption_node, rdr_exc_info);
    dump_print("save rdr global info ok\n");
}

s32 dump_creat_log_dir(rdr_exc_info_s *rdr_exc_info)
{
    s32 ret;
    char *dir_name = NULL;

    if (unlikely(rdr_exc_info == NULL)) {
        dump_error("dir_name is null\n");
        return BSP_ERROR;
    }

    dir_name = rdr_exc_info->log_path;
    ret = dump_create_dir(dir_name);
    if (unlikely(ret != BSP_OK)) {
        dump_error("fail to creat dir  \n");
        return BSP_ERROR;
    }
    return BSP_OK;
}

void dump_save_lr_sec_log_files(const char *dir_name)
{
    if (dump_sec_channel_init() != BSP_OK) {
        return;
    }
    dump_sec_enable_trans(MODEM_SEC_DUMP_ENABLE_LR_CHANNEL_CMD);
    dump_sec_save_file(dir_name);
    dump_sec_disable_trans(MODEM_SEC_DUMP_STOP_LR_CHANNEL_CMD);
    dump_sec_dst_channel_free();
}

void dump_save_nr_sec_log_files(const char *dir_name)
{
#ifdef ENABLE_BUILD_NRRDR
    if (dump_sec_channel_init() != BSP_OK) {
        return;
    }
    dump_sec_enable_trans(MODEM_SEC_DUMP_ENABLE_NR_CHANNEL_CMD);
    dump_sec_save_file(dir_name);
    dump_sec_disable_trans(MODEM_SEC_DUMP_STOP_NR_CHANNEL_CMD);
    dump_sec_dst_channel_free();
#endif
}

void dump_save_sec_log_files(const char *dir_name)
{
    dump_save_lr_sec_log_files(dir_name);
    dump_save_nr_sec_log_files(dir_name);
}

s32 dump_get_resave_sec_logs(dump_area_id_e areaid)
{
    s32 flag;
    struct dump_area_mntn_addr_info_s area_info = {
        NULL,
    };
    struct dump_area_s *dump_area_info = NULL;

    flag = dump_get_area_info(areaid, &area_info);
    if (unlikely(flag == BSP_ERROR || area_info.vaddr == NULL)) {
        dump_error("fail to find cp area head\n");
        return BSP_ERROR;
    }
    dump_area_info = ((struct dump_area_s *)(area_info.vaddr));
    flag = dump_area_info->area_head.share_info.resave_sec_log;
    if (flag == DUMP_SEC_FILE_RESAVE_DONE) {
        dump_print("sec dump_reasve ok\n");
        return BSP_OK;
    }
    return BSP_ERROR;
}

s32 dump_notify_resave_sec_logs(dump_area_id_e areaid)
{
    s32 flag;
    struct dump_area_mntn_addr_info_s area_info = {
        NULL,
    };
    struct dump_area_s *dump_area_info = NULL;

    flag = dump_get_area_info(areaid, &area_info);
    if (unlikely(flag == BSP_ERROR || area_info.vaddr == NULL)) {
        dump_error("fail to find cp area head\n");
        return BSP_ERROR;
    }
    dump_area_info = ((struct dump_area_s *)(area_info.vaddr));
    dump_area_info->area_head.share_info.resave_sec_log = DUMP_NEED_RESAVE_SEC_DUMP;

    return BSP_OK;
}

void dump_resave_lr_sec_logs(const char *dir_name)
{
    s32 ret = 0;
    u32 i = 0;
    if (false == dump_check_lr_sec_dump()) {
        dump_notify_resave_sec_logs(DUMP_AREA_LR);
        dump_print("notify lr resave sec dump\n");
#ifdef CONFIG_DUMP_LOG_ESCAPE_FIQ
        ret = bsp_send_cp_fiq(FIQ_SEC_DUMP);
#endif
        if (ret) {
            dump_error("send fiq error");
            return;
        }
        for (i = 0; i < DUMP_WAIT_SEC_LOG_DONE_COUNT; i++) {
            ret = dump_get_resave_sec_logs(DUMP_AREA_LR);
            if (ret == BSP_OK) {
                break;
            } else {
                dump_print("wait sec dumo finsh\n");
                msleep(DUMP_WAIT_SEC_LOG_DONE_TIME);
            }
        }
        dump_save_lr_sec_log_files(dir_name);
    }
}

void dump_resave_nr_sec_logs(const char *dir_name)
{
#ifdef ENABLE_BUILD_NRRDR
    s32 ret = 0;
    u32 i = 0;
    if (false == dump_check_nr_sec_dump()) {
        dump_print("notify nr resave sec dump\n");
        dump_notify_resave_sec_logs(DUMP_AREA_NR);
#ifdef CONFIG_DUMP_LOG_ESCAPE_FIQ
        ret = bsp_send_nr_fiq(FIQ_SEC_DUMP);
        if (ret) {
            dump_error("send fiq error");
            return;
        }
#endif
        for (i = 0; i < DUMP_WAIT_SEC_LOG_DONE_COUNT; i++) {
            ret = dump_get_resave_sec_logs(DUMP_AREA_NR);
            if (ret == BSP_OK) {
                break;
            } else {
                dump_print("wait sec dumo finsh\n");
                msleep(DUMP_WAIT_SEC_LOG_DONE_TIME);
            }
        }
        dump_save_nr_sec_log_files(dir_name);
    }
#endif
}

void dump_resave_sec_logs_files(const char *dir_name)
{
    dump_resave_lr_sec_logs(dir_name);
    dump_resave_nr_sec_logs(dir_name);
    dump_save_default_sec_log_force(dir_name);
}

void dump_create_save_done_file(const char *dir)
{
    char filename[MODEM_DUMP_FILE_NAME_LENGTH] = {
        0,
    };
    if (0 > snprintf_s(filename, sizeof(filename), sizeof(filename) - 1, "%s%s", dir, DUMP_SAVE_FLAG)) {
        bsp_info("snp err\n");
    }
    (void)dump_create_file(filename);
}

void dump_compress_all_logs(rdr_exc_info_s *rdr_exc_info)
{
    struct dump_file_save_info_s data_info;
    const char *dir_name = (const char *)rdr_exc_info->log_path;

    if (EOK != memset_s(&data_info, sizeof(struct dump_file_save_info_s), 0, sizeof(struct dump_file_save_info_s))) {
        dump_error("set err\n");
        return;
    }
    if (0 > strncpy_s(data_info.dstfilename, sizeof(data_info.dstfilename), "modem_dump.bin",
                      strnlen("modem_dump.bin", DUMP_MAX_FILE_NAME_LEN))) {
        dump_error("cpy err\n");
        return;
    }
    data_info.magic = DUMP_FILE_TRANS_MAGIC;
    data_info.links = DUMP_FILE_LINKS_MAGIC;
    if (BSP_OK == dump_trigger_compress(dir_name, sizeof(rdr_exc_info->log_path), &(data_info))) {
        dump_wait_compress_done(rdr_exc_info->log_path);
    }
}

void dump_resave_mntn_log(const char *dir_name)
{
}

void dump_save_normal_log_files(rdr_exc_info_s *rdr_exc_info)
{
    const char *dir_name = (const char *)rdr_exc_info->log_path;

    dump_print("%s start to save log file\n", rdr_exc_info->log_path);

    dump_save_mdm_mandatory_logs(dir_name);

    bsp_dump_log_notifer_callback(SUB_SYS_LR, dir_name);

    bsp_dump_log_notifer_callback(SUB_SYS_NR, dir_name);

    dump_save_apr_log(rdr_exc_info);

    dump_save_sec_log_files(dir_name);

    dump_resave_sec_logs_files(dir_name);

    dump_save_nr_mandatory_logs(dir_name);

    dump_resave_mntn_log(dir_name);

    dump_compress_all_logs(rdr_exc_info);

    dump_create_save_done_file(rdr_exc_info->log_path);

    dump_print("%s all logs files save ok\n", rdr_exc_info->log_path);
}

void dump_save_and_reboot(u32 modid)
{
    u32 index = dump_get_exc_index(modid);
    modem_dump_ctrl_s *dump_ctrl = NULL;
    if (index >= EXC_INFO_BUTT) {
        dump_error("fail to get index\n");
        return;
    }
    dump_ctrl = &g_dump_ctrl[index];

    dump_ctrl->dump_task_job = DUMP_TASK_JOB_SAVE_REBOOT;
    up(&dump_ctrl->sem_dump_task);

    dump_print("triger save log task \n");

    return;
}

int dump_save_task(void *data)
{
    modem_dump_ctrl_s *dump_ctrl = (modem_dump_ctrl_s *)data;
    u32 index = ((uintptr_t)dump_ctrl - (uintptr_t)(&g_dump_ctrl)) / sizeof(modem_dump_ctrl_s);
    rdr_exc_info_s *rdr_exc_info = dump_get_rdr_exc_info_by_index(index);
    while (1) {
        down(&dump_ctrl->sem_dump_task);

        dump_print("enter save log task\n");

        if ((dump_ctrl->dump_task_job & DUMP_TASK_JOB_SAVE_REBOOT) != DUMP_TASK_JOB_SAVE_REBOOT) {
            dump_ctrl->dump_task_job = 0;
            continue;
        }

        if (rdr_exc_info == NULL) {
            continue;
        }
        if (dump_get_product_type() == DUMP_PHONE) {
            dump_save_balong_rdr_info(rdr_exc_info->modid);
        }
        if (dump_wait_mdmcp_done(false) == BSP_OK && dump_wait_nr_done(false) == BSP_OK) {
            dump_update_exception_info(rdr_exc_info);
            if (BSP_OK != dump_creat_log_dir(rdr_exc_info)) {
                dump_error("creat dir error\n");
                continue;
            }
            dump_save_normal_log_files(rdr_exc_info);
        }

        if (rdr_exc_info->dump_done != BSP_NULL) {
            rdr_exc_info->dump_done(rdr_exc_info->modid, rdr_exc_info->coreid);
            dump_print("notify rdr dump handle finished\n");
            if (rdr_exc_info->modid == RDR_MODEM_NOC_MOD_ID || rdr_exc_info->modid == RDR_MODEM_DMSS_MOD_ID) {
                dump_excption_handle_done(rdr_exc_info->modid);
            }
        }

        dump_ctrl->dump_task_job = 0;
    }

    /*lint -e527 -esym(527,*)*/
    return BSP_OK;
    /*lint -e527 +esym(527,*)*/
}

__init s32 dump_creat_save_task(dump_exc_info_index_e index)
{
    struct task_struct *pid = NULL;
    char *task_name = NULL;

    if (index == EXC_INFO_NOC) {
        task_name = "dump_noc_save";
    } else if (index == EXC_INFO_DMSS) {
        task_name = "dump_dmss_save";
    } else {
        task_name = "dump_save";
    }
    g_dump_ctrl[index].dump_task_job = 0;
    osl_sem_init(0, &g_dump_ctrl[index].sem_dump_task);
    if (osl_task_init(task_name, DUMP_SAVE_TASK_PRI, 0x2000, (OSL_TASK_FUNC)dump_save_task, &g_dump_ctrl[index],
                      &pid)) {
        dump_error("creat save_modem_bootlog task error!\n");
        return BSP_ERROR;
    }

    g_dump_ctrl[index].dump_task_id = (uintptr_t)pid;

    return BSP_OK;
}

__init s32 dump_save_task_init(void)
{
    u32 i = 0;
    s32 ret;
    for (i = 0; i < EXC_INFO_BUTT; i++) {
        ret = dump_creat_save_task(i);
        if (ret != BSP_OK) {
            dump_error("creat task fail\n");
        }
    }
    dump_print("save log task init ok\n");

    return BSP_OK;
}

__init s32 bsp_dump_init(void)
{
    s32 ret;
    dump_set_verion();

    dump_feature_config_init();

    dump_file_cfg_init();

    ret = dump_exception_handler_init();
    if (ret == BSP_ERROR) {
        dump_error("fail to init exception handler\n");
        return BSP_ERROR;
    }

    ret = dump_register_modem_exc_info();
    if (ret == BSP_ERROR) {
        return BSP_ERROR;
    }
    ret = dump_save_task_init();
    if (ret == BSP_ERROR) {
        return BSP_ERROR;
    }
    dump_set_init_phase(DUMP_INIT_FLAG_SAVETASK);

    ret = dump_mdmap_init();
    if (ret != BSP_OK) {
        return BSP_ERROR;
    }
    dump_set_init_phase(DUMP_INIT_FLAG_MDMAP);

    ret = dump_mdmcp_init();
    if (ret != BSP_OK) {
        return BSP_ERROR;
    }
    dump_set_init_phase(DUMP_INIT_FLAG_MDMCP);

    ret = dump_nrrdr_init();
    if (ret != BSP_OK) {
        return BSP_ERROR;
    }

    dump_log_agent_init();

    ret = dump_lpm3_agent_init();
    if (ret != BSP_OK) {
        dump_error("dump lpm3_agent init error!\n");
    }

    ret = dump_apr_init();
    if (ret != BSP_OK) {
        dump_error("dump apr init error!\n");
    }
    dump_print("bsp_dump_init ok");
    return BSP_OK;
}
#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
module_init(bsp_dump_init);
#endif
