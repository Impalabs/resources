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

#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <mdrv_memory_layout.h>
#include "osl_malloc.h"
#include "bsp_sram.h"
#include "bsp_shared_ddr.h"
#include "bsp_ddr.h"
#include "bsp_dump.h"
#include "bsp_slice.h"
#include "bsp_nvim.h"
#include "bsp_dt.h"
#include "bsp_coresight.h"
#include "dump_logs.h"
#include "dump_config.h"
#include "dump_cp_agent.h"
#include "dump_area.h"
#include "dump_cp_logs.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

#define DUMP_CP_DSS_NUM 4
#define DUMP_BYTES_TO_U32S(bytes)  ((bytes) >> 2)

u8 *g_modem_ddr_map_addr = NULL;
struct dump_mem_backup_struct g_dump_tsp_l1_mem[DUMP_CP_DSS_NUM] = {
};

/*
 * 功能描述: 映射c核使用的ddr
 */
void dump_map_mdm_ddr(void)
{
    unsigned long modem_addr;
    unsigned int modem_size = 0;
    dump_product_type_e type = dump_get_product_type();
    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();

    dump_print("mdm_ddr= %d,type=%d\n", cfg->file_list.file_bits.mdm_ddr, type);

    modem_addr = mdrv_mem_region_get("mdm_ddr", &modem_size);
    if (modem_addr == 0 || modem_size == 0) {
        return;
    }
    if (cfg->file_list.file_bits.mdm_ddr == 1 &&
        (type == DUMP_PHONE && (DUMP_ACCESS_MDD_DDR_NON_SEC == dump_get_access_mdmddr_type()))) {
        g_modem_ddr_map_addr = (u8 *)ioremap_wc((phys_addr_t)(uintptr_t)(MDDR_FAMA((uintptr_t)modem_addr)),
                                                (size_t)(modem_size));
        if (g_modem_ddr_map_addr == NULL) {
            dump_error("fail to map g_modem_ddr_map_addr\n");
        }
    }
    dump_print("dump_memmap_modem_ddr finish\n");
}

void dump_save_mdm_ddr_file(const char *dir_name)
{
    unsigned long modem_addr;
    unsigned int modem_size = 0;
    dump_load_info_s dump_load = {
        0,
    };
    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();

    modem_addr = mdrv_mem_region_get("mdm_ddr", &modem_size);
    if (modem_addr == 0 || modem_size == 0) {
        return;
    }
    if (cfg->file_list.file_bits.mdm_ddr == 1 &&
        (((dump_get_product_type() == DUMP_PHONE) && (EDITION_INTERNAL_BETA == dump_get_edition_type()) &&
          (DUMP_ACCESS_MDD_DDR_NON_SEC == dump_get_access_mdmddr_type())))) {
        if (g_modem_ddr_map_addr == NULL) {
            dump_print("ioremap MODEM DDR fail\n");
        } else {
            if (BSP_OK == dump_get_load_info(&dump_load)) {
                dump_log_save(dir_name, "modem_ddr.bin", (u8 *)g_modem_ddr_map_addr + dump_load.mdm_ddr_saveoff,
                              (void *)(uintptr_t)((uintptr_t)modem_addr + dump_load.mdm_ddr_saveoff),
                              modem_size - dump_load.mdm_ddr_saveoff);
            } else {
                dump_log_save(dir_name, "modem_ddr.bin", (u8 *)g_modem_ddr_map_addr, (void *)((uintptr_t)modem_addr),
                              modem_size);
            }
            dump_print("save modem_ddr.bin finished\n");
        }
    }
}

void dump_save_mdm_dts_file(const char *dir_name)
{
    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();
    u8 *addr = NULL;
    unsigned long modem_dts_addr;
    unsigned int modem_dts_size = 0;

    modem_dts_addr = mdrv_mem_region_get("mdm_dts_ddr", &modem_dts_size);
    if (modem_dts_addr == 0 || modem_dts_size == 0) {
        return;
    }

    if (cfg->file_list.file_bits.mdm_dts == 1 &&
        (((dump_get_product_type() == DUMP_PHONE) && (EDITION_INTERNAL_BETA == dump_get_edition_type()) &&
          (DUMP_ACCESS_MDD_DDR_NON_SEC == dump_get_access_mdmddr_type())))) {
        addr = (u8 *)ioremap_wc((phys_addr_t)(uintptr_t)(MDDR_FAMA((uintptr_t)modem_dts_addr)),
                                (size_t)(modem_dts_size));
        if (addr == NULL) {
            dump_error("fail to ioremap DDR_MCORE_DTS_ADDR\n");
        } else {
            dump_log_save(dir_name, "modem_dts.bin", addr, (void *)((uintptr_t)modem_dts_addr), modem_dts_size);
            dump_print("save modem_dts.bin finished\n");
            iounmap(addr);
        }
    }
}

void dump_save_mdm_sram_file(const char *dir_name)
{
    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();
    if ((cfg->file_list.file_bits.mdm_sram == 1) && (EDITION_INTERNAL_BETA == dump_get_edition_type())) {
        dump_log_save(dir_name, "modem_sram.bin", (u8 *)g_mem_ctrl.sram_virt_addr, (void *)g_mem_ctrl.sram_phy_addr,
                      g_mem_ctrl.sram_mem_size);
        dump_print("save modem_sram.bin finished\n");
    }
}

void dump_save_mdm_secshare_file(const char *dir_name)
{
#ifdef DDR_SEC_SHARED_ADDR
    unsigned long sec_sha_addr;
    unsigned int sec_sha_size = 0;
    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();
    void *addr = NULL;
    sec_sha_addr = mdrv_mem_region_get("sec_share_ddr", &sec_sha_size);
    if (sec_sha_addr == 0 || sec_sha_size == 0) {
        return;
    }

    /*lint -save -e835*/
    if (cfg->file_list.file_bits.mdm_secshare == 1 &&
        (((dump_get_product_type() == DUMP_PHONE) && (EDITION_INTERNAL_BETA == dump_get_edition_type()) &&
          (DUMP_ACCESS_MDD_DDR_NON_SEC == dump_get_access_mdmddr_type())))) {
        addr = ioremap_wc((phys_addr_t)(uintptr_t)(MDDR_FAMA((uintptr_t)sec_sha_addr)), (size_t)(sec_sha_size));
        if (addr == NULL) {
            return;
        }
        dump_log_save(dir_name, "modem_secshared.bin", addr, (void *)((uintptr_t)sec_sha_addr), sec_sha_size);
        dump_print("save modem_secshared.bin finished\n");
        iounmap(addr);
    }
    /*lint -restore +e835*/
#endif
}

void dump_save_mdm_share_file(const char *dir_name)
{
    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();
    void *addr = NULL;
    if (cfg->file_list.file_bits.mdm_share == 1 &&
        ((dump_get_product_type() == DUMP_PHONE) && (EDITION_INTERNAL_BETA == dump_get_edition_type()))) {
        if ((DDR_SHARED_MEM_SIZE - DDR_SHA_NV_SIZE) < 0) {
            dump_error("share mem size error\n");
            return;
        }
        addr = ioremap_wc((phys_addr_t)(DDR_SHARED_MEM_ADDR + DDR_SHA_NV_SIZE),
                          (DDR_SHARED_MEM_SIZE - DDR_SHA_NV_SIZE));
        if (addr == NULL) {
            dump_error("remap share ddr fail\n");
            return;
        }

        dump_log_save(dir_name, "modem_share.bin", (u8 *)addr,
                      (void *)(uintptr_t)(DDR_SHARED_MEM_ADDR + DDR_SHA_NV_SIZE),
                      (DDR_SHARED_MEM_SIZE - DDR_SHA_NV_SIZE));
        iounmap(addr);
        dump_print("save modem_share.bin finished\n");
    }
}

void dump_save_mdm_llram_file(const char *dir_name)
{
#ifdef CCPU_LLRAM_BASE_ADDR

    DUMP_FILE_CFG_STRU *cfg = dump_get_file_cfg();
    void *addr = NULL;

    if (cfg->file_list.file_bits.llram_share == 1 &&
        (((dump_get_product_type() == DUMP_PHONE) && (EDITION_INTERNAL_BETA == dump_get_edition_type())))) {
        addr = ioremap_wc((phys_addr_t)(uintptr_t)(MDDR_FAMA((uintptr_t)CCPU_LLRAM_BASE_ADDR)),
                          (size_t)(CCPU_LLRAM_BASE_SIZE));
        if (addr == NULL) {
            return;
        }
        dump_log_save(dir_name, "modem_llram.bin", (u8 *)addr, (void *)((uintptr_t)CCPU_LLRAM_BASE_ADDR),
                      CCPU_LLRAM_BASE_SIZE);
        dump_print("save modem_llram.bin finished\n");
        iounmap(addr);
    }
#endif
}

void dump_save_earlylog_file(const char *dir_name)
{
#ifdef MNTN_AREA_LOG_ADDR
    struct bsp_earlylog_info info;
    s32 ret = 0;
    if (EDITION_INTERNAL_BETA == dump_get_edition_type()) {
        ret = bsp_get_earlylog_info((struct bsp_earlylog_info *)(uintptr_t)&info);
        if (ret || info.data_addr == NULL || info.data_len == 0) {
            return;
        }
        dump_log_save(dir_name, "modem_boot_log.bin", (u8 *)info.data_addr, (void *)((uintptr_t)MNTN_AREA_LOG_ADDR),
                      MNTN_AREA_LOG_SIZE);
        dump_print("save modem_boot_log.bin finished\n");
    }
#endif
}

void dump_save_share_sec_log(const char *dir_name)
{
}

void dump_save_share_nsro_log(const char *dir_name)
{
}

void dump_save_share_unsec_log(const char *dir_name)
{
}

void dump_save_l2mem_file(const char *dir_name)
{
}

void dump_tsp_l1mem_init(device_node_s *node)
{
    return;
}
void dump_tsp_l2mem_init(device_node_s *node)
{
}
void dump_tsp_mem_init(void)
{
    device_node_s *node = NULL;
    node = bsp_dt_find_compatible_node(NULL, NULL, "hisilicon,tsp_mem");
    /* 找不到节点 */
    if (node == NULL) {
        bsp_err("can't find tsp_mem node\n");
        return;
    }

    dump_tsp_l1mem_init(node);
    dump_tsp_l2mem_init(node);
}

s32 dump_check_l1mem_accessable(u32 dssid)
{
    u32 ret;

    dump_load_info_s load_info = {0};
    u32 load_offset = 0;

    if (dssid >= ARRAY_SIZE(g_dump_tsp_l1_mem)) {
        return BSP_ERROR;
    }
    if (g_dump_tsp_l1_mem[dssid].mem_addr != 0 && g_dump_tsp_l1_mem[dssid].mem_size != 0) {
        return BSP_OK;
    }
    ret = dump_get_load_info(&load_info);
    if (ret != BSP_OK) {
        return BSP_ERROR;
    }
    switch (dssid) {
        case 0:
            load_offset = load_info.ds0l1mem;
            break;
        case 1:
            load_offset = load_info.ds1l1mem;
            break;
        case 2:
            load_offset = load_info.ds2l1mem;
            break;
        case 3:
            load_offset = load_info.ds3l1mem;
            break;
        default:
            load_offset = 0;
            break;
    }

    dump_print("load_offset = 0x%x\n", load_offset);
    if (load_offset) {
        return BSP_OK;
    }
    return BSP_ERROR;
}

void dump_save_l1mem(const char *dir_name)
{
}
void dump_save_nxpde_file(const char *dir_name)
{
#ifdef DDR_PDE_IMAGE_ADDR
    unsigned long pde_addr;
    unsigned int pde_size = 0;
    void *addr = NULL;
    pde_addr = mdrv_mem_region_get("pde_ddr", &pde_size);
    if (pde_addr == 0 || pde_size == 0) {
        return;
    }

    if (DUMP_PHONE == bsp_dump_get_product_type() && DUMP_ACCESS_MDD_DDR_NON_SEC != dump_get_access_mdmddr_type()) {
        return;
    }
    addr = ioremap_wc((phys_addr_t)(uintptr_t)(MDDR_FAMA((uintptr_t)pde_addr)), (size_t)(pde_size));
    if (addr == NULL) {
        dump_error("fail to map DDR_PDE_IMAGE_ADDR\n");
        return;
    }
    dump_log_save(dir_name, "pde.bin", (u8 *)addr, (u8 *)((uintptr_t)pde_addr), pde_size);
    dump_print("save pde.bin finished\n");
    iounmap(addr);
#endif
}

/*
 * 功能描述: cp可选log初始化
 */
__init void dump_optional_log_init(void)
{
    NV_DUMP_STRU *cfg = dump_get_feature_cfg();

    (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_mdm_share_file, "mdm_share");
    (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_mdm_sram_file, "sram_share");
    (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_mdm_llram_file, "mdm_llram");
    (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_earlylog_file, "early_log");
    (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_share_unsec_log, "share_unsec");
    (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_share_nsro_log, "share_nsro");

    if (!cfg->dump_cfg.Bits.secDump) {
        (void)dump_map_mdm_ddr();
        (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_mdm_ddr_file, "mdm_ddr");
        (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_mdm_secshare_file, "sec_share");
        (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_share_sec_log, "share_sec");
        (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_nxpde_file, "nx_pde");
        (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_l1mem, "l1mem");
        (void)bsp_dump_register_log_notifier(SUB_SYS_LR, dump_save_l2mem_file, "l2mem");
    }
    dump_tsp_mem_init();
}
