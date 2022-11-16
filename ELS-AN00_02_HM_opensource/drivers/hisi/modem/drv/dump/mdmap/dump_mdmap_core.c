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
#include "osl_types.h"
#include "osl_thread.h"
#include "osl_malloc.h"
#include "bsp_dump.h"
#include "bsp_ipc.h"
#include "bsp_memmap.h"
#include "bsp_wdt.h"
#include "bsp_dump_mem.h"
#include "bsp_coresight.h"
#include "bsp_reset.h"
#include "bsp_adump.h"
#include "bsp_wdt.h"
#include "dump_config.h"
#include "dump_baseinfo.h"
#include "dump_logs.h"
#include "dump_cp_agent.h"
#include "dump_apr.h"
#include "dump_exc_handle.h"
#include "dump_area.h"
#include "dump_mdmap_core.h"
#include "dump_logs.h"
#include "dump_hook.h"
#include "dump_area.h"
#include "dump_ko.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

/*
 * 功能描述: 日志明文化使用，生成保存完成文件
 */
void dump_notify_save_done(const char *path)
{
    dump_log_save(DUMP_LOG_PATH, DUMP_STATUS_FILE, path, NULL, strlen(path));
    dump_print("save %s done\n", DUMP_STATUS_FILE);
}

/*
 * 功能描述: 保存modem_dump.bin
 */
void dump_save_mntn_bin(const char *dir_name)
{
    struct dump_global_area_ctrl_s global_area = {
        0,
    };
    s32 ret;

    if (dump_get_product_type() == DUMP_PHONE) {
        ret = dump_get_global_info(&global_area);
        if (ret == BSP_OK && global_area.virt_addr != NULL) {
            dump_log_save(dir_name, "modem_dump.bin", (u8 *)global_area.virt_addr,
                          (u8 *)(uintptr_t)global_area.phy_addr, global_area.length);
            dump_print("save modem_dump.bin finished\n");
        }
    }
}

/*
 * 功能描述: 保存调用bsp_print勾取的临终打印
 */
void dump_save_last_kmsg(void)
{
    u32 size = 0;
    u8 *addr = NULL;
    addr = bsp_dump_get_field_addr(DUMP_MODEMAP_LAST_KMSG);
    if (addr == NULL) {
        bsp_dump_get_avaiable_size(&size);
        if (size != 0) {
            size = size & 0xfffff000;
            addr = bsp_dump_register_field(DUMP_MODEMAP_LAST_KMSG, "lastkmsg", size, 0);
        }
    }
    if (addr != NULL) {
        bsp_store_log_buf(addr, size);
    }
}

void dump_update_extend_area(void)
{
}

int dump_save_extend_single_area(const char *dir_name, u32 filed_id)
{
    return BSP_OK;
}
int dump_save_extend_area(const char *dir_name)
{
    return BSP_OK;
}
/*
 * 功能描述:保存lr系统的必选文件
 */
void dump_save_mdm_mandatory_logs(const char *dir_name)
{
    dump_print("start to save mandatory logs \n");

    dump_save_mdmboot_info();

    if (dump_get_product_type() == DUMP_PHONE) {
        dump_save_last_kmsg();
    }

    dump_update_extend_area();
    dump_save_mntn_bin(dir_name);
}

/*
 * 功能描述: 打印调用栈，用于非arm异常
 */
void dump_show_stack(u32 modid, u32 reason)
{
    if (!(reason == AP_DUMP_REASON_ARM)) {
        if (modid == 0x11000025 || modid == 0x1100002A) {
            dump_error("not current task exc\n");
            show_stack(find_task_by_vpid(reason), NULL);
        } else {
            show_stack(current, NULL);
        }
    }
}

/*
 * 功能描述: 保存用户数据区
 */
void dump_save_usr_data(const char *data, u32 length)
{
    u32 len = 0;
    void *addr = NULL;
    dump_field_map_s *pfield = NULL;

    if ((data != NULL) && (length)) {
        pfield = (dump_field_map_s *)bsp_dump_get_field_map(DUMP_MODEMAP_USER_DATA);
        addr = (void *)bsp_dump_get_field_addr(DUMP_MODEMAP_USER_DATA);
        len = (length > DUMP_MODEMAP_USER_DATA_SIZE) ? DUMP_MODEMAP_USER_DATA_SIZE : length;

        if (addr != NULL) {
            if (EOK !=
                memcpy_s((void *)addr, DUMP_MODEMAP_USER_DATA_SIZE, (const void *)(uintptr_t)data, (size_t)len)) {
                bsp_debug("err\n");
            }
        }

        if (pfield != NULL) {
            pfield->length = len;
        }
    }
    dump_print("dump save usr data finish\n");
    return;
}

/*
 * 功能描述: modem 异常函数入口
 */
void system_error(u32 mod_id, u32 arg1, u32 arg2, char *data, u32 length)
{
    dump_exception_info_s dump_exception_info = {
        0,
    };
    u32 reboot_contex;

    if (in_interrupt()) {
        reboot_contex = DUMP_CTX_INT;
    } else {
        reboot_contex = DUMP_CTX_TASK;
    }

    dump_print("modem acore enter system error! timestamp:0x%x\n", bsp_get_slice_value());
    dump_print("mod_id=0x%x arg1=0x%x arg2=0x%x  len=0x%x\n", mod_id, arg1, arg2, length);

    /* AT+TOMODE=3和切卡执行的Modem单独复位 */
    if (mod_id == DRV_ERROR_USER_RESET || mod_id == DRV_ERRNO_RESET_SIM_SWITCH) {
        dump_error("rdr mod id no need to save log,enter reboot modem directly\n");
        dump_fill_excption_info(&dump_exception_info, mod_id, arg1, arg2, data, length, DUMP_CPU_APP,
                                DUMP_REASON_NORMAL, "Modem NOR RESET", reboot_contex, current->pid, 0, current->comm);
        (void)dump_register_exception(&dump_exception_info);
        return;
    }

    dump_fill_excption_info(&dump_exception_info, mod_id, arg1, arg2, data, length, DUMP_CPU_APP, DUMP_REASON_NORMAL,
                            "Modem AP EXC", reboot_contex, current->pid, 0, current->comm);

    dump_register_exception(&dump_exception_info);

    dump_save_usr_data(data, length);


    dump_save_base_info(mod_id, arg1, arg2, data, length, NULL);

    if (DUMP_PHONE == dump_get_product_type()) {
        dump_show_stack(mod_id, arg1);
    }

    return;
}

/*
 * 功能描述: 保存预定的寄存器信息
 */
void dump_save_reg(void)
{
    NV_DUMP_STRU *dump_nv_cfg = dump_get_feature_cfg();
    void *dump_addr = bsp_dump_get_field_addr(DUMP_MODEMAP_REGS);
    void *reg_addr = NULL;
    u32 size = 0;
    if (dump_nv_cfg != NULL && dump_addr != NULL) {
        if (dump_nv_cfg->dump_cfg.Bits.appRegSave1) {
            reg_addr = ioremap(dump_nv_cfg->appRegAddr1, dump_nv_cfg->appRegSize1);
            if (reg_addr != NULL) {
                size = dump_nv_cfg->appRegSize1 < DUMP_MODEMAP_REG_SINGLE_SIZE ? dump_nv_cfg->appRegSize1
                                                                                : DUMP_MODEMAP_REG_SINGLE_SIZE;
                dump_copy_register(dump_addr, reg_addr, size / DUMP_REG_READ_DATA_SIZE);
            }
            iounmap(reg_addr);
        }

        if (dump_nv_cfg->dump_cfg.Bits.appRegSave2) {
            reg_addr = ioremap(dump_nv_cfg->appRegAddr2, dump_nv_cfg->appRegSize2);
            if (reg_addr != NULL) {
                size = dump_nv_cfg->appRegSize2 < DUMP_MODEMAP_REG_SINGLE_SIZE ? dump_nv_cfg->appRegSize2
                                                                                : DUMP_MODEMAP_REG_SINGLE_SIZE;
                dump_copy_register(dump_addr + DUMP_MODEMAP_REG_SINGLE_SIZE, reg_addr, size / DUMP_REG_READ_DATA_SIZE);
            }
            iounmap(reg_addr);
        }

        if (dump_nv_cfg->dump_cfg.Bits.appRegSave3) {
            reg_addr = ioremap(dump_nv_cfg->appRegAddr3, dump_nv_cfg->appRegSize3);
            if (reg_addr != NULL) {
                size = dump_nv_cfg->appRegSize3 < DUMP_MODEMAP_REG_SINGLE_SIZE ? dump_nv_cfg->appRegSize3
                                                                                : DUMP_MODEMAP_REG_SINGLE_SIZE;
                dump_copy_register(dump_addr + DUMP_MODEMAP_REG_SINGLE_SIZE + DUMP_MODEMAP_REG_SINGLE_SIZE, reg_addr,
                                   size / DUMP_REG_READ_DATA_SIZE);
            }
            iounmap(reg_addr);
        }
    }
}

/*
 * 功能描述: 初始化modem ap需要使用filed 空间
 */
void dump_self_use_field_init(void)
{
    void *addr = NULL;

    NV_DUMP_STRU *dump_nv_cfg = dump_get_feature_cfg();

    addr = bsp_dump_register_field(DUMP_MODEMAP_USER_DATA, "USER_DATA", DUMP_MODEMAP_USER_DATA_SIZE, 0);
    if (unlikely(addr == NULL)) {
        dump_error("fail to get mdmap user data field addr\n");
    }

    if (dump_nv_cfg != NULL && (dump_nv_cfg->dump_cfg.Bits.appRegSave1 || dump_nv_cfg->dump_cfg.Bits.appRegSave2 ||
                                dump_nv_cfg->dump_cfg.Bits.appRegSave3)) {
        addr = bsp_dump_register_field(DUMP_MODEMAP_REGS, "REG_DATA", DUMP_MODEMAP_REG_SIZE, 0);
        if (unlikely(addr == NULL)) {
            dump_error("fail to get mdmap reg data field addr\n");
        } else {
            (void)bsp_dump_register_hook("dump_save_reg", dump_save_reg);
        }
    }
    dump_print("mdmap use field init ok\n");
}

/*
 * 功能描述: 初始化modem ap初始化
 */
s32 dump_mdmap_init(void)
{
    s32 ret;

    dump_self_use_field_init();

    ret = dump_base_info_init();
    if (unlikely(ret == BSP_ERROR)) {
        return BSP_ERROR;
    }

    dump_ko_load_info_init();

    dump_error("dump mdmap init ok\n");

    return BSP_OK;
}

/*
 * 功能描述: 初始化modem ap初始化
 */
s32 dump_mdmap_callback(u32 modid, u32 etype, u64 coreid, char *logpath, pfn_cb_dump_done fndone)
{
    dump_exception_info_s *exc_node = dump_get_current_excpiton_info(modid);
    if (exc_node != NULL) {
        if (exc_node->core != DUMP_CPU_APP) {
            dump_save_base_info(BSP_MODU_OTHER_CORE, 0, 0, NULL, 0, NULL);
        }
    }

    bsp_dump_hook_callback();

    dump_print("mdmap callback done\n");

    return BSP_OK;
}
EXPORT_SYMBOL(system_error);
