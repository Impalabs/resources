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
#include <mdrv_memory_layout.h>
#include "osl_types.h"
#include "osl_bio.h"
#include "osl_io.h"
#include "bsp_dump.h"
#include "bsp_ddr.h"
#include "dump_config.h"
#include "dump_easyrf_tcm.h"
#include "dump_logs.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

void dump_save_rfdsp_tcm(const char *dst_path)
{
#if (FEATURE_EASYRF == FEATURE_ON)
    unsigned long rfic_dump_addr;
    unsigned int rfic_dump_size = 0;
    u8 *data = NULL;
    dump_log_head_s *easyrf_dump_head = NULL;
    u32 size;

    if (DUMP_PHONE == dump_get_product_type() && DUMP_ACCESS_MDD_DDR_NON_SEC != dump_get_access_mdmddr_type()) {
        return;
    }
#if defined(DDR_RFIC_DUMP_ADDR) && defined(DDR_RFIC_DUMP_SIZE)
    rfic_dump_addr = mdrv_mem_region_get("rf_dump_ddr", &rfic_dump_size);
#elif defined(DDR_RFIC_SUB6G_IMAGE_ADDR) && defined(DDR_RFIC_SUB6G_IMAGE_SIZE)
    rfic_dump_addr = mdrv_mem_region_get("rf_sub6g_ddr", &rfic_dump_size);
#endif
    if (rfic_dump_addr == 0 || rfic_dump_size == 0) {
        return;
    }

    data = (u8 *)ioremap_wc(MDDR_FAMA(rfic_dump_addr), rfic_dump_size);
    if (data == NULL) {
        dump_error("fail to map DDR_RFIC_DUMP_ADDR\n");
        return;
    }

    easyrf_dump_head = (dump_log_head_s *)data;
    size = easyrf_dump_head->dump_size < rfic_dump_size ? easyrf_dump_head->dump_size : rfic_dump_size;
    dump_log_save(dst_path, "rfdsp_dump.bin", data, (void *)((uintptr_t)rfic_dump_addr), size);

    dump_print("save rfdsp_dump.bin ok\n");

    iounmap(data);
#endif
}

void dump_save_tvp_log(const char *dst_path)
{
}

__init void dump_rfdsp_init(void)
{
    s32 ret;

    NV_DUMP_STRU *cfg = dump_get_feature_cfg();
    if (cfg->dump_cfg.Bits.secDump)
        return;

    ret = bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_rfdsp_tcm, "rfdsp");
    if (ret == BSP_ERROR) {
        dump_error("dump rfdsp int fail\n");
    }
    ret = bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_tvp_log, "tvp");
    if (ret == BSP_ERROR) {
        dump_error("dump rfdsp int fail\n");
    }
}
