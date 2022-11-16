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
#include <bsp_dt.h>
#include <securec.h>
#include <osl_list.h>
#include <osl_malloc.h>
#include <bsp_print.h>
#include "dump_config.h"
#include "dump_logzip.h"
#undef THIS_MODU
#define THIS_MODU mod_dump

struct dump_log_agent_stru g_strategy_ctrl = { false, 0, NULL };
bool dump_get_link_status(void)
{
    return g_strategy_ctrl.link_enable;
}
s32 dump_match_log_strategy(const char *file_name, struct dump_file_save_strategy *output)
{
    s32 index = 0;
    u32 log_sum = g_strategy_ctrl.log_sum;
    struct dump_file_save_strategy *save_strategy = g_strategy_ctrl.log_strategy;
    if (file_name == NULL || output == NULL || save_strategy == NULL) {
        return BSP_ERROR;
    }
    if (memset_s(output, sizeof(struct dump_file_save_strategy), 0, sizeof(struct dump_file_save_strategy)) != EOK) {
        dump_error("set err\n");
        return BSP_ERROR;
    }
    for (index = 0; index < log_sum; index++) {
        if (strncmp(file_name, save_strategy[index].data_head.filename,
                    sizeof(save_strategy[index].data_head.filename)) != 0) {
            continue;
        }
        if (memcpy_s(output, sizeof(struct dump_file_save_strategy), &save_strategy[index],
                     sizeof(save_strategy[index])) != EOK) {
            dump_error("cpy err\n");
            return BSP_ERROR;
        }
        /* 拼接非压缩时，直接拼接；拼接且压缩时，最后在压缩应用中压缩再拼接 */
        if (output->is_append == 0 || output->is_compress == 1 || output->data_head.magic != DUMP_FILE_TRANS_MAGIC ||
            output->data_head.links != DUMP_FILE_LINKS_MAGIC) {
            return BSP_ERROR;
        }
        return BSP_OK;
    }
    return BSP_ERROR;
}
void dump_show_strategy(void)
{
    u32 size;
    s32 i = 0;
    struct dump_file_save_strategy *log_strategy = g_strategy_ctrl.log_strategy;
    dump_print("enable:%d\n", g_strategy_ctrl.link_enable);
    if (log_strategy == NULL) {
        dump_error("g_strategy_ctrl.log_strategy is null\n");
        return;
    }
    size = g_strategy_ctrl.log_sum;
    dump_error("log_sum:%u\n", size);
    for (i = 0; i < size; i++) {
        dump_print("filename: %s, dst_file: %s\n", log_strategy[i].data_head.filename,
                   log_strategy[i].data_head.dstfilename);
        dump_print("compress: %u, append: %u\n", log_strategy[i].is_compress, log_strategy[i].is_append);
    }
}

int dump_link_probe_src_list(device_node_s *child, struct dump_file_save_strategy *log_strategy, uintptr_t srclist_size,
                             struct dump_dstname_list *dstname_list, u32 dstlist_size)
{
    s32 i, j;
    u32 u = 0;
    char *temp_char = NULL;
    const __be32 *p = NULL;
    struct property *prop = NULL;

    for (i = 0; i < srclist_size; i++) {
        if (bsp_dt_property_read_string_index(child, "dump_src_name", i, (const char **)&temp_char)) {
            dump_error("get src_name err\n");
            return BSP_ERROR;
        }
        if (strncpy_s(log_strategy[i].data_head.filename, sizeof(log_strategy[i].data_head.filename), temp_char,
                      strnlen(temp_char, DUMP_FILE_NAME_MAX_SIZE)) < 0) {
            dump_error("strncpy err\n");
            return BSP_ERROR;
        }
        log_strategy[i].data_head.magic = DUMP_FILE_TRANS_MAGIC;
        log_strategy[i].data_head.links = DUMP_FILE_LINKS_MAGIC;
    }

    i = -1;
    j = -1;
    bsp_dt_property_for_each_u32(child, "dump_src_attr", prop, p, u) {
        i++;
        if (i % DUMP_LOG_STRATEGY_ATTR_SIZE == 0) {
            j++;
            log_strategy[j].is_compress = u;
        } else if (i % DUMP_LOG_STRATEGY_ATTR_SIZE == 1) {
            log_strategy[j].is_append = u;
        } else {
            if (log_strategy[j].is_append == 0) {
                continue;
            }
            if (u <= 0 || u > dstlist_size) {
                dump_error("u=%u is err\n", u);
                return BSP_ERROR;
            }
            if (strncpy_s(log_strategy[j].data_head.dstfilename, sizeof(log_strategy[j].data_head.dstfilename),
                          dstname_list[u - 1].filename, sizeof(dstname_list[u - 1].filename)) < 0) {
                dump_error("cpy err\n");
                return BSP_ERROR;
            }
        }
    }

    if ((i + 1) != DUMP_LOG_STRATEGY_ATTR_SIZE * srclist_size || srclist_size != (j + 1)) {
        return BSP_ERROR;
    }
    return BSP_OK;
}

int dump_link_fill_list(device_node_s *child, struct dump_file_save_strategy *log_strategy, uintptr_t srclist_size)
{
    u32 dstlist_size = 0;
    int i;
    int ret = BSP_ERROR;
    struct dump_dstname_list *dstname_list = NULL;
    char *temp_char = NULL;
    if (bsp_dt_property_read_u32(child, "dstlistsize", &dstlist_size)) {
        dump_error("get size err\n");
        return BSP_ERROR;
    }
    dstname_list = (struct dump_dstname_list *)kmalloc(dstlist_size * (sizeof(struct dump_dstname_list)), GFP_KERNEL);
    if (dstname_list == NULL) {
        dump_error("malloc err\n");
        return BSP_ERROR;
    }
    if (EOK != memset_s(dstname_list, dstlist_size * (sizeof(struct dump_dstname_list)), 0,
                        dstlist_size * (sizeof(struct dump_dstname_list)))) {
        dump_error("set err\n");
        goto out;
    }
    for (i = 0; i < dstlist_size; i++) {
        if (bsp_dt_property_read_string_index(child, "dump_dst_name", i, (const char **)&temp_char)) {
            dump_error("get dst_name err\n");
            goto out;
        }
        if (strncpy_s(dstname_list[i].filename, sizeof(dstname_list[i].filename), temp_char,
                      strnlen(temp_char, DUMP_FILE_NAME_MAX_SIZE)) < 0) {
            dump_error("strncpy err\n");
            goto out;
        }
    }

    if (dump_link_probe_src_list(child, log_strategy, srclist_size, dstname_list, dstlist_size) != BSP_OK) {
        dump_error("get src_attr err!\n");
        goto out;
    }
    ret = BSP_OK;
out:
    if (dstname_list != NULL) {
        kfree(dstname_list);
        dstname_list = NULL;
    }
    return ret;
}

int dump_link_list_init(void)
{
    device_node_s *dev = NULL;
    u32 srclist_size = 0;
    s32 ret;
    struct dump_file_save_strategy *log_strategy = NULL;
    dev = bsp_dt_find_compatible_node(NULL, NULL, "hisilicon,dump_linkup_list");
    if (dev == NULL) {
        dump_error("list dts no find\n");
        return BSP_ERROR;
    }
    ret = bsp_dt_property_read_u32(dev, "srclistsize", &srclist_size);
    if (ret) {
        dump_error("get size err\n");
        return BSP_ERROR;
    }
    log_strategy = (struct dump_file_save_strategy *)kmalloc(srclist_size * (sizeof(struct dump_file_save_strategy)),
                                                             GFP_KERNEL);
    if (log_strategy == NULL) {
        dump_error("alloc err\n");
        return BSP_ERROR;
    }
    g_strategy_ctrl.log_sum = (u32)srclist_size;
    if (EOK != memset_s(log_strategy, srclist_size * (sizeof(struct dump_file_save_strategy)), 0,
                        srclist_size * (sizeof(struct dump_file_save_strategy)))) {
        dump_error("set err\n");
        if (log_strategy != NULL) {
            kfree(log_strategy);
            log_strategy = NULL;
        }
        return BSP_ERROR;
    }
    ret = dump_link_fill_list(dev, log_strategy, srclist_size);
    if (ret) {
        if (log_strategy != NULL) {
            kfree(log_strategy);
            log_strategy = NULL;
        }
        return BSP_ERROR;
    }
    g_strategy_ctrl.log_strategy = log_strategy;
    return BSP_OK;
}
void dump_log_agent_init(void)
{
    device_node_s *dev = NULL;
    u32 link_enable = 0;
    s32 ret;

    dev = bsp_dt_find_compatible_node(NULL, NULL, "hisilicon,dump_log_strategy");
    if (dev == NULL) {
        dump_error("strategy dts no find\n");
        g_strategy_ctrl.link_enable = false;
        return;
    }

    ret = bsp_dt_property_read_u32(dev, "link_enable", &link_enable);
    if (ret || 0 == link_enable) {
        dump_error("link disable\n");
        g_strategy_ctrl.link_enable = false;
        return;
    }

    ret = dump_link_list_init();
    if (ret == BSP_OK) {
        g_strategy_ctrl.link_enable = true;
    } else {
        g_strategy_ctrl.link_enable = false;
    }
    dump_logzip_init();
    dump_print("[init]link ok\n");
    return;
}
