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
#include "osl_types.h"
#include "bsp_dump.h"
#include "bsp_reset.h"
#include "bsp_coresight.h"
#include "nv_stru_drv.h"
#include "dump_config.h"
#include "dump_cp_agent.h"
#include "dump_cp_logs.h"
#include "dump_cp_core.h"
#include "dump_easyrf_tcm.h"
#include "dump_lphy_tcm.h"
#include "dump_cphy_tcm.h"
#include "dump_sec_mem.h"
#include "dump_exc_handle.h"
#include "dump_area.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

s32 dump_mdmcp_init(void)
{
    s32 ret;

    ret = dump_cp_agent_init();
    if (ret != BSP_OK) {
        dump_error("fail to init mdmcp \n");
        return BSP_ERROR;
    }
    dump_rfdsp_init();
    dump_lphy_init();
    dump_cphy_init();
    dump_sec_init();

    dump_print("dump mdmcp init ok\n");

    return BSP_OK;
}

u32 dump_dmss_noc_proc(u32 modid)
{
    dump_reboot_reason_e reason;
    u32 drv_id = DRV_ERRNO_NOC_PHONE;
    dump_exception_info_s dump_exception_info = {
        0,
    };
    char *desc = NULL;
    dump_exception_info_s *current_info = dump_get_current_excpiton_info(modid);

    if (modid == RDR_MODEM_NOC_MOD_ID) {
        drv_id = DRV_ERRNO_NOC_PHONE;
        desc = "MODEM NOC";
        dump_print("[0x%x] modem NOC process\n", bsp_get_slice_value());
    } else if (modid == RDR_MODEM_DMSS_MOD_ID) {
        drv_id = DRV_ERRNO_DMSS_PHONE;
        desc = "MODEM DMSS";
        dump_print("[0x%x] modem DMSS process\n", bsp_get_slice_value());
    }
    reason = ((modid == RDR_MODEM_NOC_MOD_ID) ? DUMP_REASON_NOC : DUMP_REASON_DMSS);

    dump_fill_excption_info(&dump_exception_info, drv_id, 0, 0, NULL, 0, DUMP_CPU_APP, reason, desc, DUMP_CTX_TASK, 0,
                            0, NULL);
    if (EOK !=
        memcpy_s(current_info, sizeof(dump_exception_info_s), &dump_exception_info, sizeof(dump_exception_info_s))) {
        bsp_debug("err\n");
    }
    return drv_id;
}
EXPORT_SYMBOL(dump_dmss_noc_proc);

u32 dump_mdmcp_callback(u32 modid, u32 etype, u64 coreid, char *logpath, pfn_cb_dump_done fndone)
{
    dump_print("enter dump callback, mod id:0x%x\n", modid);

    if (bsp_reset_ccore_is_reboot() == 0) {
        dump_notify_cp(modid);
    } else {
        dump_print("modem is reseting now,do not notify\n");
    }
    dump_print("dump_mdmcp_callback finish\n");
    return BSP_OK;
}

s32 dump_check_single_reset_by_nv(void)
{
    NV_DUMP_STRU *cfg = NULL;
    cfg = dump_get_feature_cfg();
    if (cfg != NULL && cfg->dump_cfg.Bits.sysErrReboot == 0) {
        dump_print("close modem sigle reset\n");
        return BSP_ERROR;
    }

    dump_print(" modem sigle reset open\n");

    return BSP_OK;
}

s32 dump_check_single_reset_by_modid(u32 modid)
{
    struct rdr_exception_info_s *exception_info_s = dump_get_exception_info_node(modid);
    if (exception_info_s != NULL) {
        if (exception_info_s->e_reset_core_mask == RDR_CP) {
            dump_print("go to modem reset\n");
            return BSP_OK;
        }
        if (exception_info_s->e_reset_core_mask == RDR_AP) {
            dump_print("need reset system: 0x%x\n", modid);
            return BSP_ERROR;
        }
    }
    dump_error("invalid mod id: 0x%x\n", modid);
    return BSP_ERROR;
}

s32 dump_check_cp_reset(u32 modid)
{
    if (DUMP_PHONE != dump_get_product_type()) {
        dump_print("mbb not support cp_reset\n");
        return BSP_ERROR;
    }
    if (BSP_ERROR == dump_check_single_reset_by_modid(modid)) {
        dump_print("modid not support cp_reset\n");
        return BSP_ERROR;
    }
    if (BSP_ERROR == dump_check_single_reset_by_nv()) {
        dump_print("dump_check_single_reset_by_nv retun not support\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

dump_reset_result_e dump_mdmcp_reset(u32 modid, u32 etype, u64 coreid)
{
    s32 ret;
    dump_print("enter dump reset, mod id:0x%x\n", modid);
    dump_clear_cpboot_area();
    if (BSP_OK == dump_check_cp_reset(modid)) {
        ret = bsp_cp_reset();
        if (ret == -1) {
            return RESET_NOT_SUPPORT;
        }
        if (!bsp_reset_is_successful(RDR_MODEM_CP_RESET_TIMEOUT)) {
            return RESET_NOT_SUCCES;
        }
        bsp_print_save_early_log(DUMP_CP_RESET_EARLY_LOG_WAIT);
        return RESET_SUCCES;
    }
    return RESET_NOT_SUPPORT;
}
