/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
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

#include <linux/of.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/string.h>
#include "osl_types.h"
#include "osl_io.h"
#include "bsp_nrdsp.h"
#include "bsp_ddr.h"
#include <bsp_nvim.h>
#include "bsp_dump.h"
#include <nv_stru_drv.h>
#include "nrdsp_dump_tcm.h"
#include <securec.h>
#include <mdrv_memory_layout.h>

#define THIS_MODU mod_nrdsp
#if defined(CONFIG_MODEM_MINI_DUMP)

/**
 * 函 数 名  : secdump_is_enable
 * 功能描述  : 判断安全dump是否使能(后续由dump模块提供接口)
 * 输入参数  :
 * 输出参数  :
 * 返 回 值  :
 */
s32 secdump_is_enable(void)
{
    NV_DUMP_STRU *cfg = dump_get_feature_cfg();
    if (cfg->dump_cfg.Bits.secDump) {
        return 1;
    }

    return 0;
}

void bsp_nrdsp_dump_tcm(const char *dst_path)
{
    char *image_addr = NULL;
    u32 file_size;
    unsigned long nrphy_addr;
    u32 nrphy_size = 0;

    if (dst_path == NULL) {
        return;
    }

    /* 接口后续替换为bsp，还需要考虑MBB内存安全属性配置 */
    if (DUMP_PHONE == bsp_dump_get_product_type() && DUMP_ACCESS_MDD_DDR_NON_SEC != dump_get_access_mdmddr_type()) {
        return;
    }
    nrphy_addr = mdrv_mem_region_get("nrphy_ddr", &nrphy_size);
    if (nrphy_addr == 0 || nrphy_size == 0){
        return;
    }

    image_addr = (char *)ioremap_wc(NXDSP_MDDR_FAMA(nrphy_addr), nrphy_size);
    if (NULL == image_addr) {
        nrdsp_print("ioremap DDR_NRPHY_IMAGE_ADDR fail\n");
        return;
    }
    file_size = ((struct nrdsp_bin_header *)image_addr)->ulFileSize;
    if (file_size > nrphy_size) {
        file_size = nrphy_size;
        nrdsp_print("save nrphy_dump.bin failed, file_size: 0x%x\n", file_size);
    }
    bsp_dump_save_file(dst_path, "nrphy_dump.bin", image_addr, (void *)((uintptr_t)nrphy_addr),
                       file_size);
    bsp_err("save nrphy_dump.bin finished\n");

    iounmap(image_addr);
    return;
}

s32 bsp_nrdsp_dump_tcm_init(void)
{
    s32 ret;

    if (bsp_dump_is_secdump()) {
        return BSP_OK;
    }

    ret = bsp_dump_register_log_notifier(SUB_SYS_NR, bsp_nrdsp_dump_tcm, "nrdsp");
    if (ret == BSP_ERROR) {
        nrdsp_print("nrdsp_dump_tcm fail\n");
        return BSP_ERROR;
    }
    bsp_err("nrdsp_dump_tcm init OK\n");

    return ret;
}

#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
module_init(bsp_nrdsp_dump_tcm_init);
#endif
#else
s32 bsp_nrdsp_dump_tcm_init(void)
{
    return 0;
}
#endif
