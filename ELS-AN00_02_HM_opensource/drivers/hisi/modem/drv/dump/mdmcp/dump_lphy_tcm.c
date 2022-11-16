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
#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/syslog.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <bsp_dt.h>
#include <asm/string.h>
#include "securec.h"
#include "osl_types.h"
#include "osl_bio.h"
#include "osl_io.h"
#include "bsp_rfile.h"
#include "bsp_dspload.h"
#include "bsp_dsp.h"
#include "bsp_dump.h"
#include "bsp_ddr.h"
#include "dump_config.h"
#include "dump_logs.h"
#include <bsp_slice.h>
#include <dump_lphy_tcm.h>
#include <mdrv_memory_layout.h>

#undef THIS_MODU
#define THIS_MODU mod_dump
#if defined(DDR_TLPHY_IMAGE_SIZE) && defined(DDR_TLPHY_IMAGE_ADDR)
char *g_lphy_image_ddr_addr = NULL;

char *g_lphy1_image_ddr_addr = NULL;

/*
 * 功能描述: 保存lphy全部的log文件
 */
void dump_save_lphy_log(u8 *data, u8 *phy_addr, const char *dst_path, u32 file_size)
{
    unsigned long ddr_addr;
    unsigned int ddr_size = 0;

    ddr_addr = mdrv_mem_region_get("tlphy_ddr", &ddr_size);
    if (ddr_addr == 0 || ddr_size == 0) {
        return;
    }

    if (file_size > ddr_size) {
        dump_error("save lphy_dump.bin failed, file_size: 0x%x\n", file_size);
        file_size = ddr_size;
    }
    dump_log_save(dst_path, "lphy_dump.bin", data, phy_addr, file_size);
    dump_print("save lphy_dump.bin ok\n");
}
/*
 * 功能描述: 保存全部的tcm文件
 */
void dump_save_all_tcm(u8 *data, u8 *phy_addr, char *dst_path)
{
#if ((!defined(CONFIG_CBBE)) && (!defined(CONFIG_TLDSP_NRFPGA)))
    /* DTCM PUB */
    dump_log_save(dst_path, "lphy_pub_dtcm.bin", data, phy_addr, LPHY_PUB_DTCM_SIZE);
    dump_print("save lphy_pub_dtcm.bin ok!\n");

    /* DTCM MOD */
    dump_log_save(dst_path, "lphy_mode_dtcm.bin", data + LPHY_PUB_DTCM_SIZE, phy_addr + LPHY_PUB_DTCM_SIZE,
                  LPHY_PRV_DTCM_SIZE);
    dump_print("save lphy_mode_dtcm.bin ok!\n");

    if (dump_get_edition_type() == EDITION_INTERNAL_BETA) {
        /* ITCM PUB */
        dump_log_save(dst_path, "lphy_pub_itcm.bin", data + LPHY_PUB_DTCM_SIZE + LPHY_PRV_DTCM_SIZE,
                      phy_addr + LPHY_PUB_DTCM_SIZE + LPHY_PRV_DTCM_SIZE, LPHY_PUB_ITCM_SIZE);
        dump_print("save lphy_pub_itcm.bin ok!\n");

        /* ITCM MOD */
        dump_log_save(dst_path, "lphy_mode_itcm.bin",
                      data + LPHY_PUB_DTCM_SIZE + LPHY_PUB_ITCM_SIZE + LPHY_PRV_DTCM_SIZE,
                      phy_addr + LPHY_PUB_DTCM_SIZE + LPHY_PUB_ITCM_SIZE + LPHY_PRV_DTCM_SIZE, LPHY_PRV_ITCM_SIZE);
        dump_print("save lphy_mode_itcm.bin ok!\n");
    }
#endif
}

/*
 * 功能描述: 保存全部的dtcm和itcm文件
 */
void dump_save_dtcm(char *data, char *dst_path)
{
#if ((!defined(CONFIG_CBBE)) && (!defined(CONFIG_TLDSP_NRFPGA)))
    int fd;
    int ret;
    char file_name[RDR_DUMP_FILE_PATH_LEN] = {0};

    /* MBB与PHONE均保存DTCM */
    if (EOK != memset_s(file_name, sizeof(file_name), 0, sizeof(file_name))) {
        bsp_debug("err\n");
    }
    if (0 > snprintf_s(file_name, sizeof(file_name), (sizeof(file_name) - 1), "%slphy_dtcm.bin", dst_path)) {
        bsp_debug("er\n");
    }
    file_name[RDR_DUMP_FILE_PATH_LEN - 1] = '\0';
    fd = bsp_open(file_name, RFILE_RDWR | RFILE_CREAT, DUMP_LOG_FILE_AUTH);
    if (fd < 0) {
        dump_error("open %s failed ,save lphy_dtcm failed!\n", file_name);
        return;
    }
    ret = bsp_write(fd, data, LPHY_PUB_DTCM_SIZE);
    if (ret != LPHY_PUB_DTCM_SIZE)
        goto err0;
    ret = bsp_write(fd, data + LPHY_PUB_DTCM_SIZE + LPHY_PUB_ITCM_SIZE, LPHY_PRV_DTCM_SIZE);
    if (ret != LPHY_PRV_DTCM_SIZE)
        goto err0;
    ret = bsp_write(fd, data + LPHY_PUB_DTCM_SIZE + LPHY_PUB_ITCM_SIZE + LPHY_PRV_DTCM_SIZE + LPHY_PRV_ITCM_SIZE,
                    LPHY_PRV_DTCM_SIZE);
    if (ret != LPHY_PRV_DTCM_SIZE)
        goto err0;
    dump_print("save %s ok!\n", file_name);
err0:
    bsp_close(fd);
#endif
}

void dump_save_itcm(char *data, char *dst_path)
{
#if ((!defined(CONFIG_CBBE)) && (!defined(CONFIG_TLDSP_NRFPGA)))
    int fd;
    int ret;
    char file_name[RDR_DUMP_FILE_PATH_LEN] = {0};

    if (memset_s(file_name, sizeof(file_name), 0, sizeof(file_name)) != EOK) {
        bsp_debug("err\n");
    }
    if (snprintf_s(file_name, sizeof(file_name), (sizeof(file_name) - 1), "%slphy_itcm.bin", dst_path) < 0) {
        bsp_debug("er\n");
    }
    file_name[RDR_DUMP_FILE_PATH_LEN - 1] = '\0';
    fd = bsp_open(file_name, RFILE_RDWR | RFILE_CREAT, DUMP_LOG_FILE_AUTH);
    if (fd < 0) {
        dump_error("open %s failed ,save lphy_itcm failed!\n", file_name);
        return;
    }
    ret = bsp_write(fd, data + LPHY_PUB_DTCM_SIZE, LPHY_PUB_ITCM_SIZE);
    if (ret != LPHY_PUB_ITCM_SIZE)
        goto err1;
    ret = bsp_write(fd, data + LPHY_PUB_DTCM_SIZE + LPHY_PUB_ITCM_SIZE + LPHY_PRV_DTCM_SIZE, LPHY_PRV_ITCM_SIZE);
    if (ret != LPHY_PRV_ITCM_SIZE)
        goto err1;
    ret = bsp_write(fd, data + LPHY_PUB_DTCM_SIZE + LPHY_PUB_ITCM_SIZE + LPHY_PRV_DTCM_SIZE + LPHY_PRV_ITCM_SIZE +
                    LPHY_PRV_DTCM_SIZE, LPHY_PRV_ITCM_SIZE);
    if (ret == LPHY_PRV_ITCM_SIZE) {
        dump_print("save %s ok!\n", file_name);
    }
err1:
    bsp_close(fd);
#endif
}

void dump_save_some_tcm(char *data, char *dst_path)
{
#if ((!defined(CONFIG_CBBE)) && (!defined(CONFIG_TLDSP_NRFPGA)))
    dump_save_dtcm(data, dst_path);

    /* 如果是PHONE产品则同时保存LPHY ITCM，MBB受空间限制则只保存DTCM部分 */
    if (DUMP_PHONE == dump_get_product_type()) {
        dump_save_itcm(data, dst_path);
    }
#endif
}
/*
 * 功能描述: 保存tldsp的镜像
 */
void dump_save_lphy_tcm(const char *dst_path)
{
    const char *tmp_path = dst_path;
    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();
    unsigned long tlphy_addr;
    unsigned int tlphy_size = 0;

    if (dump_get_product_type() == DUMP_PHONE && dump_get_access_mdmddr_type() != DUMP_ACCESS_MDD_DDR_NON_SEC) {
        return;
    }

    if (cfg->file_list.file_bits.lphy_tcm == 0) {
        return;
    }

    tlphy_addr = mdrv_mem_region_get("tlphy_ddr", &tlphy_size);
    if (tlphy_addr == 0 || tlphy_size == 0) {
        return;
    }

    g_lphy_image_ddr_addr = (char *)ioremap_wc(NXDSP_MDDR_FAMA(tlphy_addr), tlphy_size);
    if (g_lphy_image_ddr_addr == NULL) {
        dump_error("ioremap DDR_TLPHY_IMAGE_ADDR fail\n");
        return;
    }
    dump_save_lphy_log(g_lphy_image_ddr_addr, (void *)((uintptr_t)tlphy_addr), tmp_path,
                       (u32)readl(g_lphy_image_ddr_addr + 0x18));
/* ltev存log */

    iounmap(g_lphy_image_ddr_addr);
    return;
}
#else
void dump_save_lphy_tcm(const char *dst_path)
{
}
#endif
__init void dump_lphy_init(void)
{
/* NR时域测试版本没有TLPHY */
    s32 ret;
    NV_DUMP_STRU *cfg = dump_get_feature_cfg();
    /* 未使能安全dump，直接退出即可 */
    if (cfg->dump_cfg.Bits.secDump)
        return;
    ret = bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_lphy_tcm, "lr_lphy");
    if (ret == BSP_ERROR) {
        dump_error("[init]dump_save_lphy_tcm register failed\n");
    }
}
