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
#include <linux/kernel.h>
#include <linux/rtc.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include "mdrv_errno.h"
#include "osl_types.h"
#include "osl_thread.h"
#include "osl_sem.h"
#include "bsp_dump.h"
#include "bsp_dump_mem.h"
#include "bsp_slice.h"
#include "bsp_reset.h"
#include "bsp_coresight.h"
#include "bsp_wdt.h"
#include "bsp_noc.h"
#include "bsp_cold_patch.h"
#include "dump_config.h"
#include "dump_baseinfo.h"
#include "dump_cp_agent.h"
#include "dump_area.h"
#include "dump_cp_wdt.h"
#include "dump_logs.h"
#include "dump_area.h"
#include "dump_exc_handle.h"
#include "dump_cp_core.h"
#include "dump_mdmap_core.h"
#include "nrrdr_core.h"
#include "nrrdr_agent.h"
#include "dump_m3_agent.h"
#include "dump_logs.h"
#include "dump_core.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

rdr_exc_info_s g_rdr_exc_info[EXC_INFO_BUTT];

dump_exception_ctrl_s g_exception_ctrl;

dump_exception_info_s g_curr_excption[EXC_INFO_BUTT];

dump_cp_reset_ctrl_s g_dump_mdm_reset_record;

dump_mod_id_s g_dump_cp_mod_id[] = {
    { RDR_MODEM_CP_DRV_MOD_ID_START, RDR_MODEM_CP_DRV_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_DRV_MOD_ID) },
    { RDR_MODEM_CP_OSA_MOD_ID_START, RDR_MODEM_CP_OSA_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_OSA_MOD_ID) },
    { RDR_MODEM_CP_OAM_MOD_ID_START, RDR_MODEM_CP_OAM_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_OAM_MOD_ID) },
    { RDR_MODEM_CP_GUL2_MOD_ID_START, RDR_MODEM_CP_GUL2_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_GUL2_MOD_ID) },
    { RDR_MODEM_CP_CTTF_MOD_ID_START, RDR_MODEM_CP_CTTF_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_CTTF_MOD_ID) },
    { RDR_MODEM_CP_GUWAS_MOD_ID_START, RDR_MODEM_CP_GUWAS_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_GUWAS_MOD_ID) },
    { RDR_MODEM_CP_CAS_MOD_ID_START, RDR_MODEM_CP_CAS_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_CAS_MOD_ID) },
    { RDR_MODEM_CP_CPROC_MOD_ID_START, RDR_MODEM_CP_CPROC_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_CPROC_MOD_ID) },
    { RDR_MODEM_CP_GUGAS_MOD_ID_START, RDR_MODEM_CP_GUGAS_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_GUGAS_MOD_ID) },
    { RDR_MODEM_CP_GUCNAS_MOD_ID_START, RDR_MODEM_CP_GUCNAS_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_GUCNAS_MOD_ID) },
    { RDR_MODEM_CP_GUDSP_MOD_ID_START, RDR_MODEM_CP_GUDSP_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_GUDSP_MOD_ID) },
    { RDR_MODEM_CP_EASYRF_MOD_ID_START, RDR_MODEM_CP_EASYRF_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_EASYRF_MOD_ID) },
    { RDR_MODEM_CP_MSP_MOD_ID_START, RDR_MODEM_CP_MSP_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_LMSP_MOD_ID) },
    { RDR_MODEM_CP_LPS_MOD_ID_START, RDR_MODEM_CP_LPS_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_LPS_MOD_ID) },
    { RDR_MODEM_CP_TLDSP_MOD_ID_START, RDR_MODEM_CP_TLDSP_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_TLDSP_MOD_ID) },
    { RDR_MODEM_CP_NRDSP_MOD_ID_START, RDR_MODEM_CP_NRDSP_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_NRDSP_MOD_ID) },
    { RDR_MODEM_CP_CPHY_MOD_ID_START, RDR_MODEM_CP_CPHY_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_CPHY_MOD_ID) },
    { RDR_MODEM_CP_IMS_MOD_ID_START, RDR_MODEM_CP_IMS_MOD_ID_END, RDR_MODID_TO_EXCNAME(RDR_MODEM_CP_IMS_MOD_ID) },
};

dump_mod_id_s g_dump_nr_mod_id[] = {
    { RDR_MODEM_NR_DRV_MOD_ID_START, RDR_MODEM_NR_DRV_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_DRV_MOD_ID) },
    { RDR_MODEM_NR_OSA_MOD_ID_START, RDR_MODEM_NR_OSA_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_OSA_MOD_ID) },
    { RDR_MODEM_NR_OAM_MOD_ID_START, RDR_MODEM_NR_OAM_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_OAM_MOD_ID) },
    { RDR_MODEM_NR_GUL2_MOD_ID_START, RDR_MODEM_NR_GUL2_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_GUL2_MOD_ID) },
    { RDR_MODEM_NR_CTTF_MOD_ID_START, RDR_MODEM_NR_CTTF_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_CTTF_MOD_ID) },
    { RDR_MODEM_NR_GUWAS_MOD_ID_START, RDR_MODEM_NR_GUWAS_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_GUWAS_MOD_ID) },
    { RDR_MODEM_NR_CPROC_MOD_ID_START, RDR_MODEM_NR_CPROC_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_CPROC_MOD_ID) },
    { RDR_MODEM_NR_GUGAS_MOD_ID_START, RDR_MODEM_NR_GUGAS_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_GUGAS_MOD_ID) },
    { RDR_MODEM_NR_GUCNAS_MOD_ID_START, RDR_MODEM_NR_GUCNAS_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_NRNAS_MOD_ID) },
    { RDR_MODEM_NR_GUDSP_MOD_ID_START, RDR_MODEM_NR_GUDSP_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_NRPHY_MOD_ID) },
    { RDR_MODEM_NR_LPS_MOD_ID_START, RDR_MODEM_NR_LPS_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_NRPS_MOD_ID) },
    { RDR_MODEM_NR_TLDSP_MOD_ID_START, RDR_MODEM_NR_TLDSP_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_NRDSP_MOD_ID) },
    { RDR_MODEM_NR_IMS_MOD_ID_START, RDR_MODEM_NR_IMS_MOD_ID_END, RDR_MODID_TO_EXCNAME(NRRDR_EXC_CCPU_IMS_MOD_ID) },
};

struct rdr_exception_info_s g_modem_exc_info[] = {
#ifdef BSP_CONFIG_PHONE_TYPE

    {
        .e_modid = (unsigned int)RDR_MODEM_AP_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_AP_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_NOW,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_MODEMAP,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMAP",
        .e_desc = "modem ap reset system",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_AP_DRV_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_AP_DRV_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_NOW,
        .e_notify_core_mask = RDR_AP | RDR_CP,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_MODEMAP,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMAP",
        .e_desc = "modem ap drv reset system",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_LPM3_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_LPM3_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMLPM3",
        .e_desc = "modem lpm3 exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_DRV_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_DRV_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_DRV_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp drv exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_OSA_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_OSA_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_PAM_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp osa exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_OAM_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_OAM_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_PAM_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp oam exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUL2_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUL2_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_GUAS_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp gul2 exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_CTTF_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_CTTF_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_CTTF_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp cttf exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUWAS_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUWAS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_GUAS_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp guwas exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_CAS_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_CAS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_CAS_CPROC_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp cas exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_CPROC_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_CPROC_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_CAS_CPROC_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp cproc exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUGAS_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUGAS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_GUAS_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp guas exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUCNAS_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUCNAS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_GUCNAS_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp gucnas exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUDSP_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUDSP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_GUDSP_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp gudsp exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_EASYRF_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_EASYRF_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_GUDSP_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp easyRF exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_LPS_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_LPS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_TLPS_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp tlps exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_LMSP_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_LMSP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_DRV_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp lmsp exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_TLDSP_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_TLDSP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_TLDSP_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp tldsp exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_CPHY_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_CPHY_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_CPHY_EXC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp cphy exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_IMS_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_IMS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem cp ims exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_RESET_SIM_SWITCH_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_RESET_SIM_SWITCH_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = 0,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_NORMALRESET,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem normal reboot",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_RESET_FAIL_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_RESET_FAIL_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_RESETFAIL,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem self-reset fail",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_RESET_FREQUENTLY_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_RESET_FREQUENTLY_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_RESETFAIL,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem reset frequently",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_WDT_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_WDT_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem self-reset wdog",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_RESET_RILD_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_RESET_RILD_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_RILD_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem reset by rild",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_RESET_3RD_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_RESET_3RD_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_3RD_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem reset by 3rd modem",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_NOC_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_NOC_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem noc reset",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_NORMALRESET,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem reset stub",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_NOC_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_NOC_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_NOW,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_MODEMNOC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem noc error",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_AP_NOC_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_AP_NOC_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_NOW,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_MODEMNOC,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem noc reset system",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_RESET_USER_RESET_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_RESET_USER_RESET_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = 0,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_NORMALRESET,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem user reset without log",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_DMSS_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_DMSS_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_NOW,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_MODEMDMSS,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem dmss error",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_RESET_DLOCK_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_RESET_DLOCK_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem reset by bus error",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CODE_PATCH_REVERT_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CODE_PATCH_REVERT_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_NOW,
        .e_notify_core_mask = 0,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_MODEMAP,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMAP",
        .e_desc = "modem cold patch revert",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_NR_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_NR_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "NR",
        .e_desc = "modem nr exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_NR_L2HAC_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_NR_L2HAC_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "NR",
        .e_desc = "modem l2hac exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_NR_CCPU_WDT,
        .e_modid_end = (unsigned int)RDR_MODEM_NR_CCPU_WDT,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "NR",
        .e_desc = "modem nr wdt exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_NRDSP_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_NRDSP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "NR",
        .e_desc = "modem nr dsp exc",
        .e_save_log_flags = RDR_SAVE_LOGBUF,
    },
#else
    {
        .e_modid = (unsigned int)RDR_MODEM_AP_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_AP_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_AP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type = CP_S_MODEMAP,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMAP",
        .e_desc = "MDMAP reset system",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_DRV_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_DRV_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP DRV reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_OSA_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_OSA_MOD_ID_END,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP OSA reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_OAM_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_OAM_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP OAM reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUL2_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUL2_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP GUL2 reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_CTTF_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_CTTF_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP CTTF reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUWAS_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUWAS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP GUWAS reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_CAS_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_CAS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP CAS reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_CPROC_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_CPROC_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP CPROC reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUGAS_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUGAS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP GUGAS reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUCNAS_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUCNAS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP GUCNAS reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_GUDSP_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_GUDSP_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP GUDSP reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_EASYRF_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_EASYRF_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP easyRF reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_LPS_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_LPS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP LPS reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_TLDSP_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_TLDSP_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP TLDSP reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_NRDSP_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_NRDSP_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP NRDSP reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_CPHY_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_CPHY_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP CPHY reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_IMS_MOD_ID_START,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_IMS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "MDMCP IMS reset",
    },
    {
        .e_modid = (unsigned int)RDR_MODEM_CP_WDT_MOD_ID,
        .e_modid_end = (unsigned int)RDR_MODEM_CP_WDT_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_CP,
        .e_from_core = RDR_CP,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module = "MDMCP",
        .e_desc = "modem self-reset wdog",
    },
    {
        .e_modid = (unsigned int)NRRDR_MODEM_NR_CCPU_START,
        .e_modid_end = (unsigned int)NRRDR_MODEM_NR_CCPU_END,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_NR,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .excinfo_callbak = (rdr_exc_info_callback)dump_get_nr_excinfo,
        .e_from_module = "NRCCPU",
        .e_desc = "nrccpu exception",
    },
    {
        .e_modid = (unsigned int)NRRDR_MODEM_NR_L2HAC_START,
        .e_modid_end = (unsigned int)NRRDR_MODEM_NR_L2HAC_END,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_NR,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .excinfo_callbak = (rdr_exc_info_callback)dump_get_nr_excinfo,
        .e_from_module = "NRL2HAC",
        .e_desc = "l2hac exception",
    },
    {
        .e_modid = (unsigned int)NRRDR_MODEM_NR_CCPU_WDT,
        .e_modid_end = (unsigned int)NRRDR_MODEM_NR_CCPU_WDT,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask = RDR_AP,
        .e_from_core = RDR_NR,
        .e_reentrant = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type = CP_S_EXCEPTION,
        .e_upload_flag = (unsigned int)RDR_UPLOAD_YES,
        .excinfo_callbak = (rdr_exc_info_callback)dump_get_nr_excinfo,
        .e_from_module = "NRCCPU",
        .e_desc = "nrccpu wdt",
    },
#endif

};

rdr_exc_info_s *dump_get_rdr_exc_info_by_modid(u32 modid)
{
    rdr_exc_info_s *rdr_info = NULL;

    if (modid == RDR_MODEM_NOC_MOD_ID) {
        rdr_info = &g_rdr_exc_info[EXC_INFO_NOC];
    } else if (modid == RDR_MODEM_DMSS_MOD_ID) {
        rdr_info = &g_rdr_exc_info[EXC_INFO_DMSS];
    } else {
        rdr_info = &g_rdr_exc_info[EXC_INFO_NORMAL];
    }

    return rdr_info;
}

rdr_exc_info_s *dump_get_rdr_exc_info_by_index(u32 index)
{
    rdr_exc_info_s *rdr_info = NULL;
    if (unlikely(index >= EXC_INFO_BUTT)) {
        return NULL;
    }

    rdr_info = &g_rdr_exc_info[index];

    return rdr_info;
}

u32 dump_get_exc_index(u32 modid)
{
    u32 index = EXC_INFO_NORMAL;
    if (modid == RDR_MODEM_NOC_MOD_ID) {
        index = EXC_INFO_NOC;
    } else if (modid == RDR_MODEM_DMSS_MOD_ID) {
        index = EXC_INFO_DMSS;
    } else {
        index = EXC_INFO_NORMAL;
    }
    return index;
}

void dump_save_rdr_callback_info(u32 modid, u32 etype, u64 coreid, char *logpath, pfn_cb_dump_done fndone)
{
    rdr_exc_info_s *rdr_info = NULL;

    if (unlikely(logpath == NULL)) {
        dump_error("logpath is null\n");
        return;
    }

    rdr_info = dump_get_rdr_exc_info_by_modid(modid);
    if (unlikely(rdr_info == NULL)) {
        dump_error("rdr_info is null\n");
        return;
    }

    rdr_info->modid = modid;
    rdr_info->coreid = coreid;
    rdr_info->dump_done = fndone;

    if (unlikely((strlen(logpath) + strlen(RDR_DUMP_FILE_CP_PATH)) >= RDR_DUMP_FILE_PATH_LEN - 1)) {
        dump_error("log path is too long %s\n", logpath);
        return;
    }

    if (EOK != memset_s(rdr_info->log_path, sizeof(rdr_info->log_path), '\0', sizeof(rdr_info->log_path))) {
        bsp_debug("err\n");
    }
    if (EOK != memcpy_s(rdr_info->log_path, sizeof(rdr_info->log_path), logpath, strlen(logpath))) {
        bsp_debug("err\n");
    }
    if (EOK != memcpy_s(rdr_info->log_path + strlen(logpath), (sizeof(rdr_info->log_path) - strlen(logpath)),
                        RDR_DUMP_FILE_CP_PATH, strlen(RDR_DUMP_FILE_CP_PATH))) {
        bsp_debug("err\n");
    }

    dump_print("this exception logpath is %s\n", rdr_info->log_path);
}

char *dump_get_nr_exc_desc(u32 nrrdr_modid)
{
    char *desc = NULL;
    switch (nrrdr_modid) {
        case NRRDR_EXC_CCPU_DRV_MOD_ID:
            desc = "NRCCPU_DRV_EXC";
            break;
        case NRRDR_EXC_CCPU_OSA_MOD_ID:
        case NRRDR_EXC_CCPU_OAM_MOD_ID:
            desc = "NRCCPU_PAM_EXC";
            break;
        case NRRDR_EXC_CCPU_NRNAS_MOD_ID:
            desc = "NRCCPU_NAS_EXC";
            break;
        case NRRDR_EXC_CCPU_NRDSP_MOD_ID:
        case NRRDR_EXC_CCPU_NRPHY_MOD_ID:
            desc = "NRCCPU_PHY_EXC";
            break;
        case NRRDR_EXC_CCPU_IMS_MOD_ID:
            desc = "NRCCPU_IMS_EXC";
            break;
        case NRRDR_EXC_CCPU_NRPS_MOD_ID:
            desc = "NRCCPU_L2&L3_EXC";
            break;
        default:
            desc = "NRCCPU_EXC";
            break;
    }
    return desc;
}

void dump_get_nr_excinfo(u32 modid, void *exc_sub_type, void *desc)
{
    char *exc_desc = NULL;
    if (exc_sub_type == NULL || desc == NULL) {
        dump_error("error param");
        return;
    }
    if (modid < RDR_MODEM_NR_MOD_ID_START || modid > RDR_MODEM_NR_MOD_ID_END) {
        return;
    }
    if (modid >= NRRDR_MODEM_NR_CCPU_START && modid <= NRRDR_MODEM_NR_CCPU_END) {
        (*(u32 *)exc_sub_type) = DUMP_CPU_NRCCPU;
        exc_desc = dump_get_nr_exc_desc(modid);
        if (exc_desc == NULL) {
            return;
        }
        if (memcpy_s(desc, DUMP_NR_EXCINFO_SIZE_48, exc_desc, strlen(exc_desc)) != EOK) {
            dump_debug("copy error\n");
        }
    } else if (modid >= NRRDR_MODEM_NR_L2HAC_START && modid <= NRRDR_MODEM_NR_L2HAC_END) {
        (*(u32 *)exc_sub_type) = DUMP_CPU_NRL2HAC;
        if (memcpy_s(desc, DUMP_NR_EXCINFO_SIZE_48, NRL2HAC_EXCEPTION, strlen(NRL2HAC_EXCEPTION)) != EOK) {
            dump_debug("copy error\n");
        }
    }
}

s32 dump_check_reset_fail(u32 rdr_id)
{
    return BSP_OK;
}

void dump_reset_ctrl_int(void)
{
    if (EOK !=
        memset_s(&g_dump_mdm_reset_record, sizeof(g_dump_mdm_reset_record), 0, sizeof(g_dump_mdm_reset_record))) {
        bsp_debug("err\n");
    }
}

s32 dump_check_reset_freq(u32 rdr_id)
{
    u32 diff = 0;
    NV_DUMP_STRU *cfg = NULL;
    cfg = dump_get_feature_cfg();

    if (DUMP_MBB == dump_get_product_type()) {
        return BSP_OK;
    }
    if (unlikely(cfg != NULL && cfg->dump_cfg.Bits.fetal_err == 0)) {
        dump_error("no need check mdm reset times\n");
        return BSP_OK;
    }
    if (BSP_OK == dump_check_single_reset_by_modid(rdr_id)) {
        if (g_dump_mdm_reset_record.count % DUMP_CP_REST_TIME_COUNT == 0 && g_dump_mdm_reset_record.count != 0) {
            diff = (g_dump_mdm_reset_record.reset_time[DUMP_CP_REST_TIME_COUNT - 1] -
                    g_dump_mdm_reset_record.reset_time[0]);
            if (diff < DUMP_CP_REST_TIME_COUNT * DUMP_CP_REST_TIME_SLICE) {
                dump_error("stop modem single reset\n ");
                return BSP_ERROR;
            }

            if (EOK != memset_s(&g_dump_mdm_reset_record, sizeof(g_dump_mdm_reset_record), 0,
                                sizeof(g_dump_mdm_reset_record))) {
                bsp_debug("err\n");
            }
        }
        if (rdr_id != RDR_MODEM_CP_RESET_SIM_SWITCH_MOD_ID && rdr_id != RDR_MODEM_CP_RESET_USER_RESET_MOD_ID) {
            g_dump_mdm_reset_record.reset_time[g_dump_mdm_reset_record.count % DUMP_CP_REST_TIME_COUNT] =
                bsp_get_slice_value();
            g_dump_mdm_reset_record.count++;
        }
        return BSP_OK;
    } else {
        dump_print("no need check this modid\n");
    }
    return BSP_OK;
}

u32 dump_match_ccpu_rdr_id(u32 mdmcp_mod_id)
{
    u32 i = 0;
#ifdef BSP_CONFIG_PHONE_TYPE
    u32 rdr_id = NRRDR_EXC_CCPU_DRV_MOD_ID;
#else
    u32 rdr_id = RDR_MODEM_CP_DRV_MOD_ID;
#endif
    for (i = 0; i < ARRAY_SIZE(g_dump_cp_mod_id); i++) {
        if (mdmcp_mod_id >= g_dump_cp_mod_id[i].mdm_id_start && mdmcp_mod_id <= g_dump_cp_mod_id[i].mdm_id_end) {
            rdr_id = g_dump_cp_mod_id[i].rdr_id;
        }
    }
    return rdr_id;
}

/* mbb used only */
u32 dump_match_nrccpu_rdr_id(u32 mdmnr_mod_id)
{
    u32 i = 0;
    u32 rdr_id = RDR_MODEM_NR_MOD_ID_START;
    for (i = 0; i < ARRAY_SIZE(g_dump_nr_mod_id); i++) {
        if (mdmnr_mod_id >= g_dump_nr_mod_id[i].mdm_id_start && mdmnr_mod_id <= g_dump_nr_mod_id[i].mdm_id_end) {
            rdr_id = g_dump_nr_mod_id[i].rdr_id;
        }
    }
    return rdr_id;
}

s32 dump_match_noc_rdr_id(u32 modid, u32 arg)
{
    u32 rdr_id = RDR_MODEM_DRV_BUTT_MOD_ID;

    if ((modid == DRV_ERRNO_MODEM_NOC) || (modid == NOC_RESET_GUC_MODID) || (modid == NOC_RESET_NXP_MODID) ||
        (modid == NOC_RESET_BBP_DMA0_MODID) || (modid == NOC_RESET_BBP_DMA1_MODID) || (modid == NOC_RESET_HARQ_MODID) ||
        (modid == NOC_RESET_CPHY_MODID) || (modid == NOC_RESET_GUL2_MODID)) {
        if (arg == NOC_AP_RESET) {
            rdr_id = RDR_MODEM_AP_NOC_MOD_ID;
        } else if (arg == NOC_CP_RESET) {
            rdr_id = RDR_MODEM_CP_NOC_MOD_ID;
        }
    }
    return rdr_id;
}

void dump_print_mdm_error(u32 rdr_id)
{
    u32 i = 0;
    for (i = 0; i < ARRAY_SIZE(g_dump_cp_mod_id); i++) {
        if (rdr_id == g_dump_cp_mod_id[i].rdr_id) {
            dump_print("exc_id:%s\n", g_dump_cp_mod_id[i].exc_desc);
            return;
        }
    }
    dump_print("exc_id: RDR_MODEM_DRV_MOD_ID\n");
}

s32 dump_match_mdmcp_rdr_id(dump_exception_info_s *dump_exception_info)
{
    u32 rdr_id = RDR_MODEM_DRV_BUTT_MOD_ID;

    /* wdt和dlock错误 */
    if (dump_exception_info->reason == DUMP_REASON_WDT) {
        rdr_id = RDR_MODEM_CP_WDT_MOD_ID;
        return rdr_id;
    } else if (dump_exception_info->mod_id == DRV_ERRNO_DLOCK) {
        rdr_id = RDR_MODEM_CP_RESET_DLOCK_MOD_ID;
        return rdr_id;
    }

#ifdef BSP_CONFIG_PHONE_TYPE
    /* 手机版本noc错误 */
    rdr_id = dump_match_noc_rdr_id(dump_exception_info->mod_id, dump_exception_info->arg1);
    if (rdr_id == RDR_MODEM_DRV_BUTT_MOD_ID) {
        rdr_id = dump_match_ccpu_rdr_id(dump_exception_info->mod_id);
        dump_print_mdm_error(rdr_id);
    }
#else
    /* mbb产品cp的错误码匹配 */
    rdr_id = dump_exception_info->mod_id;
#endif

    return rdr_id;
}

u32 dump_match_special_rdr_id(u32 modid)
{
    u32 rdr_mod_id = RDR_MODEM_DRV_BUTT_MOD_ID;
    if (DUMP_PHONE == dump_get_product_type()) {
        switch (modid) {
            case DRV_ERRNO_RESET_SIM_SWITCH:
                rdr_mod_id = RDR_MODEM_CP_RESET_SIM_SWITCH_MOD_ID;
                break;
            case RILD_REBOOT_MOD_ID:
                rdr_mod_id = RDR_MODEM_CP_RESET_RILD_MOD_ID;
                break;
            case DRV_ERRNO_RESET_3RD_MODEM:
                rdr_mod_id = RDR_MODEM_CP_RESET_3RD_MOD_ID;
                break;
            case DRV_ERRNO_RESET_REBOOT_REQ:
                rdr_mod_id = RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID;
                break;
            case DRV_ERROR_USER_RESET:
                rdr_mod_id = RDR_MODEM_CP_RESET_USER_RESET_MOD_ID;
                break;
            case DRV_ERRNO_RST_FAIL:
                rdr_mod_id = RDR_MODEM_CP_RESET_FAIL_MOD_ID;
                break;
            case DRV_ERRNO_NOC_PHONE:
                rdr_mod_id = RDR_MODEM_NOC_MOD_ID;
                break;
            case DRV_ERRNO_DMSS_PHONE:
                rdr_mod_id = RDR_MODEM_DMSS_MOD_ID;
                break;
            default:
                break;
        }
    }
    return rdr_mod_id;
}

s32 dump_match_mdmap_rdr_id(dump_exception_info_s *dump_exception_info)
{
    u32 rdr_mod_id;
    rdr_mod_id = dump_match_special_rdr_id(dump_exception_info->mod_id);
    if (rdr_mod_id == RDR_MODEM_DRV_BUTT_MOD_ID) {
        /* 底软的错误，并且在商用版本上都走单独复位，不允许执行整机复位 */
        if ((dump_exception_info->mod_id <= (u32)RDR_MODEM_CP_DRV_MOD_ID_END) &&
            EDITION_INTERNAL_BETA != dump_get_edition_type()) {
            rdr_mod_id = RDR_MODEM_AP_DRV_MOD_ID;
        } else {
            rdr_mod_id = RDR_MODEM_AP_MOD_ID;
        }
    }
    return rdr_mod_id;
}

u32 dump_match_mdmnrl2hac_rdr_id(dump_exception_info_s *dump_exception_info)
{
#ifndef BSP_CONFIG_PHONE_TYPE
    return NRRDR_MODEM_NR_L2HAC_START;
#else
    dump_print("nr_exc_id: RDR_MODEM_NR_L2HAC_MOD_ID\n");
    return RDR_MODEM_NR_L2HAC_MOD_ID;
#endif
}

u32 dump_match_mdmnrccpu_rdr_id(dump_exception_info_s *dump_exception_info)
{
#ifndef BSP_CONFIG_PHONE_TYPE
    return dump_match_nrccpu_rdr_id(dump_exception_info->mod_id);
#else
    if (dump_exception_info->mod_id == DRV_ERRNO_NRCCPU_WDT) {
        dump_print("nr_exc_id: RDR_MODEM_NR_WDT_MOD_ID\n");
        return RDR_MODEM_NR_CCPU_WDT;
    } else {
        dump_print_mdm_error(dump_match_ccpu_rdr_id(dump_exception_info->mod_id));
        return RDR_MODEM_NR_MOD_ID;
    }
#endif
}

u32 dump_match_rdr_mod_id(dump_exception_info_s *dump_exception_info)
{
    u32 rdr_id = RDR_MODEM_AP_MOD_ID;
    if (unlikely(dump_exception_info == NULL)) {
        return rdr_id;
    }
    if (dump_exception_info->core == DUMP_CPU_NRCCPU) {
        rdr_id = dump_match_mdmnrccpu_rdr_id(dump_exception_info);
    } else if (dump_exception_info->core == DUMP_CPU_NRL2HAC) {
        rdr_id = dump_match_mdmnrl2hac_rdr_id(dump_exception_info);
    } else if (dump_exception_info->core == DUMP_CPU_LRCCPU) {
        rdr_id = dump_match_mdmcp_rdr_id(dump_exception_info);
    } else if (dump_exception_info->core == DUMP_CPU_APP) {
        rdr_id = dump_match_mdmap_rdr_id(dump_exception_info);
    } else if (dump_exception_info->core == DUMP_CPU_MDMM3) {
        rdr_id = dump_match_mdm_lpm3_rdr_id(dump_exception_info);
    }
    return rdr_id;
}

void dump_show_excption_info(dump_exception_info_s *exception_info_s)
{
    dump_print("mod_id = 0x%x\n", exception_info_s->mod_id);
    dump_print("rdr_mod_id = 0x%x\n", exception_info_s->rdr_mod_id);
    dump_print("arg1 = 0x%x\n", exception_info_s->arg1);
    dump_print("arg2 = 0x%x\n", exception_info_s->arg2);
    dump_print("voice = 0x%x\n", exception_info_s->voice);
    dump_print("int_no = 0x%x\n", exception_info_s->int_no);
    dump_print("task_id = 0x%x\n", exception_info_s->task_id);
    dump_print("reboot_contex = 0x%x\n", exception_info_s->reboot_contex);
    dump_print("timestamp = 0x%x\n", exception_info_s->timestamp);
    dump_print("reason = 0x%x\n", exception_info_s->reason);
    if (strlen(exception_info_s->task_name) != 0) {
        dump_print("task_name = %s\n", exception_info_s->task_name);
    }
    if (strlen(exception_info_s->exc_desc) != 0) {
        dump_print("exc_desc = %s\n", exception_info_s->exc_desc);
    }
}

void dump_fill_excption_info(dump_exception_info_s *exception_info_s, u32 mod_id, u32 arg1, u32 arg2, char *data,
                             u32 length, u32 core, u32 reason, const char *desc, dump_reboot_ctx_e contex, u32 task_id,
                             u32 int_no, u8 *task_name)
{
    if (unlikely(exception_info_s == NULL)) {
        dump_error("exception_info_s is null\n");
        return;
    }
    exception_info_s->core = core;
    exception_info_s->mod_id = mod_id;
    exception_info_s->rdr_mod_id = dump_match_rdr_mod_id(exception_info_s);
    exception_info_s->arg1 = arg1;
    exception_info_s->arg2 = arg2;
    exception_info_s->data = data;
    exception_info_s->length = length;
    exception_info_s->voice = dump_get_mdm_voice_status();
    exception_info_s->reboot_contex = contex;
    exception_info_s->reason = reason;
    if (exception_info_s->reboot_contex == DUMP_CTX_INT) {
        exception_info_s->int_no = int_no;
    } else {
        exception_info_s->task_id = task_id;
        if (task_name != NULL) {
            if (EOK != memcpy_s(exception_info_s->task_name, sizeof(exception_info_s->task_name), task_name,
                                strlen(task_name))) {
                bsp_debug("err\n");
            }
        }
    }

    if (desc != NULL) {
        if (EOK != memcpy_s(exception_info_s->exc_desc, sizeof(exception_info_s->exc_desc), desc, strlen(desc))) {
            bsp_debug("err\n");
        }
    }
    dump_print("fill excption info done\n");
}

struct rdr_exception_info_s *dump_get_exception_info_node(u32 mod_id)
{
    u32 i = 0;
    struct rdr_exception_info_s *rdr_exc_info = NULL;

    for (i = 0; i < (sizeof(g_modem_exc_info) / sizeof(g_modem_exc_info[0])); i++) {
        if (g_modem_exc_info[i].e_modid <= mod_id && mod_id <= g_modem_exc_info[i].e_modid_end) {
            rdr_exc_info = &g_modem_exc_info[i];
        }
    }
    return rdr_exc_info;
}

s32 dump_check_need_report_excption(u32 rdr_id)
{
    if (rdr_id == RDR_MODEM_NOC_MOD_ID || rdr_id == RDR_MODEM_DMSS_MOD_ID) {
        dump_print("rdr_id = 0x%x\n", rdr_id);
        return BSP_ERROR;
    }
    return BSP_OK;
}

dump_exception_info_s *dump_get_current_excpiton_info(u32 modid)
{
    u32 index = dump_get_exc_index(modid);
    if (unlikely(index >= EXC_INFO_BUTT)) {
        return NULL;
    }
    return &(g_curr_excption[index]);
}

void dump_wait_excption_handle_done(dump_exception_info_s *dest_exc_info)
{
    s32 ret;
    if (dest_exc_info == NULL) {
        return;
    }
    dump_print("start to wait exception handler done\n");
    ret = down_timeout(&dest_exc_info->sem_wait, (unsigned long)msecs_to_jiffies(WAIT_EXCEPTION_HANDLE_TIME));
    if (ret != 0) {
        dump_print("wait exception handler timeout\n");
    } else {
        dump_print("exception handler done\n");
    }
}

void dump_mdm_wdt_disable(void)
{
    bsp_wdt_irq_disable(WDT_CCORE_ID);
    dump_print("stop cp wdt\n");

    bsp_wdt_irq_disable(WDT_NRCCPU_ID);
    dump_print("stop nrccpu wdt finish\n");
}

void dump_mdm_wdt_enable(void)
{
    bsp_wdt_irq_enable(WDT_CCORE_ID);
    dump_print("enbale lr wdt\n");

    bsp_wdt_irq_enable(WDT_NRCCPU_ID);
    dump_print("enbale nr wdt\n");
}

void dump_excption_handle_done(u32 modid)
{
    u32 index = dump_get_exc_index(modid);
    dump_exception_info_s curr_info_s = {
        0,
    };
    unsigned long flags;
    if (index >= EXC_INFO_BUTT) {
        return;
    }

    if (index == EXC_INFO_NORMAL) {
        dump_print("clear last excinfo");
        if (EOK != memcpy_s(&curr_info_s, sizeof(curr_info_s), &g_curr_excption[EXC_INFO_NORMAL],
                            sizeof(g_curr_excption[EXC_INFO_NORMAL]))) {
            dump_debug("error\n");
        }
        spin_lock_irqsave(&g_exception_ctrl.lock, flags);
        if (EOK !=
            memset_s(&g_curr_excption[index], sizeof(g_curr_excption[index]), sizeof(g_curr_excption[index]), 0)) {
            bsp_debug("err\n");
        }
        if (curr_info_s.status == DUMP_STATUS_REGISTER) {
            g_curr_excption[index].status = DUMP_STATUS_NONE;
            dump_print("alloc next excinfo enter\n");
        }
        spin_unlock_irqrestore(&g_exception_ctrl.lock, flags);
    }

    dump_print("dump_excption_handle_done\n");
}

int dump_handle_excption_task(void *data)
{
    unsigned long flags;
    dump_exception_info_s *curr_info_s = NULL;
    s32 ret = BSP_ERROR;

    for (;;) {
        osl_sem_down(&g_exception_ctrl.sem_exception_task);

        dump_print("enter excption handler task \n");
        curr_info_s = &g_curr_excption[EXC_INFO_NORMAL];
        if (curr_info_s == NULL) {
            continue;
        }

        dump_mdm_wdt_disable();

        spin_lock_irqsave(&g_exception_ctrl.lock, flags);

        ret = dump_check_reset_freq(curr_info_s->rdr_mod_id);
        if (ret == BSP_OK && BSP_OK == dump_check_need_report_excption(curr_info_s->rdr_mod_id)) {
            dump_show_excption_info(curr_info_s);
            /* 每个节点都是添加到队尾，实际上每次取到的就是时间戳最早的 */
            rdr_system_error(curr_info_s->rdr_mod_id, curr_info_s->arg1, curr_info_s->arg2);
        } else {
            /* 当前这个rdrid是要触发modem单独复位的，但是此时modem单独复位过多，直接关闭modem */
            if ((dump_check_single_reset_by_modid(curr_info_s->rdr_mod_id) == BSP_OK) &&
                (g_exception_ctrl.modem_off != DUMP_MODEM_OFF)) {
#ifdef BSP_CONFIG_PHONE_TYPE
                bsp_modem_power_off();
                bsp_wdt_irq_enable(WDT_CCORE_ID);
                bsp_wdt_irq_enable(WDT_NRCCPU_ID);
                g_exception_ctrl.modem_off = DUMP_MODEM_OFF;
                dump_print("modem reset too many times,shut down\n");
#endif
            }
        }
        spin_unlock_irqrestore(&g_exception_ctrl.lock, flags);

        dump_print("exit excption handler task \n");
    }
    return BSP_OK;
}

s32 dump_exception_handler_init(void)
{
    struct task_struct *pid = NULL;
    struct sched_param param = {
        0,
    };

    spin_lock_init(&g_exception_ctrl.lock);

    sema_init(&g_exception_ctrl.sem_exception_task, 0);

    INIT_LIST_HEAD(&g_exception_ctrl.exception_list);

    g_curr_excption[EXC_INFO_NORMAL].status = DUMP_STATUS_NONE;

    dump_reset_ctrl_int();

    pid = (struct task_struct *)kthread_run(dump_handle_excption_task, 0, "Modem_exception");
    if (IS_ERR((void *)pid)) {
        dump_error("fail to create kthread task failed! \n");
        return BSP_ERROR;
    }
    g_exception_ctrl.exception_task_id = (uintptr_t)pid;

    param.sched_priority = DUMP_EXC_TASK_PRI;
    if (BSP_OK != sched_setscheduler(pid, SCHED_FIFO, &param)) {
        dump_error("fail to set scheduler failed!\n");
        return BSP_ERROR;
    }
    g_exception_ctrl.init_flag = true;

    dump_print("exception handler init ok\n");

    return BSP_OK;
}

bool dump_check_need_reboot_sys(u32 rdr_id)
{
    if (rdr_id == RDR_MODEM_AP_MOD_ID || rdr_id == RDR_MODEM_AP_MOD_ID || rdr_id == RDR_MODEM_NOC_MOD_ID ||
        rdr_id == RDR_MODEM_DMSS_MOD_ID || rdr_id == RDR_MODEM_CP_RESET_FAIL_MOD_ID ||
        rdr_id == RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID) {
        return true;
    }
    return false;
}

void dump_update_exception_info(rdr_exc_info_s *rdr_exc_info)
{
    dump_exception_info_s *exception = NULL;
    dump_base_info_s *modem_cp_base_info = NULL;
    dump_cp_reboot_contex_s *reboot_contex = NULL;
    void *addr1 = NULL;
    void *addr = NULL;
    if (unlikely(rdr_exc_info == NULL)) {
        return;
    }

    exception = (dump_exception_info_s *)dump_get_current_excpiton_info(rdr_exc_info->modid);
    if (unlikely(exception == NULL)) {
        return;
    }
    if (exception->reason != DUMP_REASON_WDT) {
        return;
    }
    if (exception->core == DUMP_CPU_NRCCPU) {
        addr = bsp_dump_get_field_addr(DUMP_NRCCPU_BASE_INFO_SMP);
        addr1 = (u8 *)bsp_dump_get_field_addr(DUMP_NRCCPU_REBOOTCONTEX);
    } else if (exception->core == DUMP_CPU_LRCCPU) {
        addr = bsp_dump_get_field_addr(DUMP_LRCCPU_BASE_INFO_SMP);
        addr1 = (u8 *)bsp_dump_get_field_addr(DUMP_LRCCPU_REBOOTCONTEX);
    }
    if (unlikely(addr == NULL || addr1 == NULL)) {
        return;
    }

    modem_cp_base_info = (dump_base_info_s *)(uintptr_t)addr;
    if (modem_cp_base_info->cpu_max_num > 1 && modem_cp_base_info->reboot_cpu == BSP_MODU_OTHER_CORE) {
        modem_cp_base_info->reboot_cpu = 0;
        reboot_contex = (dump_cp_reboot_contex_s *)((uintptr_t)(addr1));
        if (memcpy_s(modem_cp_base_info->task_name, sizeof(modem_cp_base_info->task_name),
                     reboot_contex->task_name, sizeof(reboot_contex->task_name)) != EOK) {
            dump_debug("err\n");
        }
        modem_cp_base_info->reboot_context = reboot_contex->reboot_context;
        modem_cp_base_info->reboot_int = reboot_contex->reboot_int;
        modem_cp_base_info->reboot_task = reboot_contex->reboot_task;
    }
    if (memcpy_s(exception->task_name, sizeof(exception->task_name), modem_cp_base_info->task_name,
                 sizeof(modem_cp_base_info->task_name)) != EOK) {
        dump_debug("err\n");
    }
    exception->task_id = modem_cp_base_info->reboot_task;
    exception->int_no = modem_cp_base_info->reboot_int;
    exception->reboot_contex = modem_cp_base_info->reboot_context;
}

s32 dump_register_exception(dump_exception_info_s *current_exception)
{
    dump_exception_info_s *exception_info_s = NULL;
    unsigned long flags;
    dump_exception_info_s *curr_info_s = &g_curr_excption[EXC_INFO_NORMAL];
    u32 status = 0;
    if (unlikely(current_exception == NULL)) {
        dump_error("param exception_info is null\n");
        return BSP_ERROR;
    }
    /* 中断上下文也可能出现异常 */
    exception_info_s = kmalloc(sizeof(dump_exception_info_s), GFP_ATOMIC);
    if (exception_info_s != NULL) {
        if (EOK != memset_s(exception_info_s, sizeof(*exception_info_s), 0, sizeof(*exception_info_s))) {
            bsp_debug("err\n");
        }
        if (EOK !=
            memcpy_s(exception_info_s, sizeof(*exception_info_s), current_exception, sizeof(*current_exception))) {
            bsp_debug("err\n");
        }

        spin_lock_irqsave(&g_exception_ctrl.lock, flags);

        if (curr_info_s->status == DUMP_STATUS_NONE) {
            curr_info_s->status = DUMP_STATUS_REGISTER;
        } else {
            spin_unlock_irqrestore(&g_exception_ctrl.lock, flags);
            dump_print("now is handle exception exit\n");
            if (exception_info_s != NULL) {
                kfree(exception_info_s);
                exception_info_s = NULL;
            }
            return BSP_ERROR;
        }
        status = curr_info_s->status;
        if (EOK !=
            memcpy_s(curr_info_s, sizeof(dump_exception_info_s), current_exception, sizeof(dump_exception_info_s))) {
            dump_debug("error");
        }
        curr_info_s->status = status;
        spin_unlock_irqrestore(&g_exception_ctrl.lock, flags);

        dump_print("register exception ok \n");

        if (exception_info_s != NULL) {
            kfree(exception_info_s);
            exception_info_s = NULL;
        }
        up(&g_exception_ctrl.sem_exception_task);
    } else {
        dump_error("malloc error\n");
        return BSP_ERROR;
    }

    /*lint -save -e429*/
    return BSP_OK;
    /*lint +restore +e429*/
}

void dump_mdm_callback(unsigned int modid, unsigned int etype, unsigned long long coreid, char *logpath,
                       pfn_cb_dump_done fndone)
{
    u32 ret = BSP_OK;
    dump_print("enter modem dump callback\n");
    dump_save_rdr_callback_info(modid, etype, coreid, logpath, fndone);

    if (modid == RDR_MODEM_NOC_MOD_ID || modid == RDR_MODEM_DMSS_MOD_ID) {
        dump_mdm_wdt_disable();
        ret = dump_dmss_noc_proc(modid);
        dump_save_base_info(ret, 0, 0, 0, 0, "noc or dmss");
    }

    ret = (u32)dump_mdmcp_callback(modid, etype, coreid, logpath, fndone);
    ret |= (u32)dump_mdmap_callback(modid, etype, coreid, logpath, fndone);
    ret |= (u32)dump_nr_callback(modid, etype, coreid, logpath, fndone);
    ret |= (u32)dump_mdm_lpm3_callback(modid, etype, coreid, logpath, fndone);

    if (modid != RDR_PHONE_MDMAP_PANIC_MOD_ID) {
        dump_save_and_reboot(modid);
    } else {
        /* 当前如果logpath是空，不触发保存log的流程，但是要等待Modem log保存完成 */
        if (BSP_OK == dump_wait_mdmcp_done(true) && BSP_OK == dump_wait_nr_done(true) &&
            BSP_OK == dump_wait_mdm_lpm3_done(true)) {
            dump_print("mdm log save done\n");
        }
    }
    if (ret != BSP_OK) {
        dump_error("callback error\n");
    }
}

void dump_mdm_reset(unsigned int modid, unsigned int etype, unsigned long long coreid)
{
    s32 ret;
    char *desc = NULL;
    u32 drv_mod_id = DRV_ERRNO_RESET_REBOOT_REQ;

    dump_exception_info_s exception_info_s = {
        0,
    };

    if (bsp_modem_is_reboot_machine()) {
        dump_print("modem need reboot whole system,without logs\n");
        rdr_system_error(RDR_MODEM_CODE_PATCH_REVERT_MOD_ID, 0, 0);
        return;
    }

    ret = dump_mdmcp_reset(modid, etype, coreid);
    if (ret != RESET_SUCCES) {
        if (ret == RESET_NOT_SUPPORT) {
            dump_fill_excption_info(&exception_info_s, DRV_ERRNO_RESET_REBOOT_REQ, 0, 0, NULL, 0, DUMP_CPU_APP,
                                    DUMP_REASON_RST_NOT_SUPPORT, "reset not support", DUMP_CTX_TASK, 0, 0,
                                    "modem_reset");
            drv_mod_id = DRV_ERRNO_RESET_REBOOT_REQ;
            desc = "MDM_RST_FREQ";
        } else {
            dump_fill_excption_info(&exception_info_s, DRV_ERRNO_RST_FAIL, 0, 0, NULL, 0, DUMP_CPU_APP,
                                    DUMP_REASON_RST_FAIL, "reset fail", DUMP_CTX_TASK, 0, 0, "modem_reset");
            drv_mod_id = DRV_ERRNO_RST_FAIL;
            desc = "MDM_RST_FAIL";
        }

        dump_save_base_info(drv_mod_id, 0, 0, NULL, 0, desc);
    } else {
        dump_base_info_init();
    }

    if (ret == RESET_SUCCES) {
        dump_mdm_wdt_enable();
    }

    dump_excption_handle_done(modid);

    if (ret != RESET_SUCCES) {
        (void)dump_register_exception(&exception_info_s);
    }
}

s32 dump_register_modem_exc_info(void)
{
    u32 i = 0;
    struct rdr_module_ops_pub soc_ops = {
        .ops_dump = NULL,
        .ops_reset = NULL
    };
    struct rdr_register_module_result soc_rst = { 0, 0, 0 };

    for (i = 0; i < sizeof(g_modem_exc_info) / sizeof(struct rdr_exception_info_s); i++) {
        if ((bsp_reset_is_feature_on() != MDM_RESET_SUPPORT) && (g_modem_exc_info[i].e_reset_core_mask == RDR_CP)) {
            g_modem_exc_info[i].e_reset_core_mask = RDR_AP;
        }
        if (rdr_register_exception(&g_modem_exc_info[i]) == 0) {
            dump_error("modid:0x%x register rdr exception failed\n", g_modem_exc_info[i].e_modid);
            return BSP_ERROR;
        }
    }

    if (EOK != memset_s(&g_rdr_exc_info, sizeof(g_rdr_exc_info), 0, sizeof(g_rdr_exc_info))) {
        bsp_debug("err\n");
    }
    soc_ops.ops_dump = (pfn_dump)dump_mdm_callback;
    soc_ops.ops_reset = (pfn_reset)dump_mdm_reset;

    if (rdr_register_module_ops(RDR_CP, &soc_ops, &(soc_rst)) != BSP_OK) {
        dump_error("fail to register  rdr ops \n");
        return BSP_ERROR;
    }
    dump_print("register modem exc info ok");
    return BSP_OK;
}
