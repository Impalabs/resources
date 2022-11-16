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
#include <bsp_dt.h>
#include <linux/of_device.h> /* for of_dma_configure */
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#include <linux/device.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <asm/cacheflush.h>
#include <securec.h>
#include <osl_types.h>
#include <osl_bio.h>
#include <osl_io.h>
#include <osl_malloc.h>
#include <bsp_slice.h>
#include <bsp_nvim.h>
#include <bsp_rfile.h>
#include <bsp_socp.h>
#include <bsp_dump.h>
#include <bsp_ddr.h>
#include <bsp_reset.h>
#include <bsp_sec_call.h>
#include "dump_logs.h"
#include "dump_config.h"
#include "dump_sec_mem.h"
#include "dump_log_agent.h"
#include "dump_area.h"
#undef THIS_MODU
#define THIS_MODU mod_dump

int dump_sec_channel_init(void);

#define DUMP_SECMEM_WAIT_CNT 100

struct platform_device *g_dump_sec_pdev = NULL;
u8 *g_sec_dump_debug = NULL;
dump_sec_secos_packet_s *g_sec_dump_secos_packet = NULL;

dump_sec_drvdata_s g_sec_dump_drvdata = {
    .socp_dst_info = {
        1,
        SOCP_CODER_DST_DUMP,
        DUMP_SEC_CODER_DST_IND_SIZE,
        DUMP_SEC_CODER_DST_THRESHOLD,
        SOCP_TIMEOUT_TFR_LONG,
        NULL,
        NULL,
        NULL,
        0,
    }
};

static inline void dump_sec_invalid_cache(void *ptr, u32 size)
{
    (void)dma_map_single(&g_dump_sec_pdev->dev, ptr, size, DMA_FROM_DEVICE);
}

dump_ddr_trans_head_info_s *dump_sec_get_modem_trance_node(dump_ddr_trans_head_info_s *trancs_list,
                                                           dump_ddr_trans_head_info_s *dump_head)
{
    u32 i = 0;
    dump_ddr_trans_head_info_s *trans_head = NULL;
    if (trancs_list == NULL || dump_head == NULL) {
        dump_error("get sec dump field fail\n");
        return NULL;
    }

    for (i = 0; i < DUMP_SEC_DEBUG_SIZE / sizeof(dump_ddr_trans_head_info_s); i++) {
        trans_head = &trancs_list[i];
        if (trans_head->magic == DUMP_SEC_TRANS_FILE_MAGIC &&
            0 == strncmp(trans_head->file_name, dump_head->file_name, strlen(dump_head->file_name))) {
            return trans_head;
        }
    }
    return NULL;
}
int dump_sec_check_save_done(dump_ddr_trans_head_info_s *trans_head)
{
    u32 i;
    if (trans_head == NULL) {
        return true;
    }
    for (i = 0; i < DUMP_SEC_DEBUG_SIZE / sizeof(dump_ddr_trans_head_info_s); i++) {
        if (trans_head[i].magic == DUMP_SEC_TRANS_FILE_MAGIC && trans_head[i].total_length != 0 &&
            0 != strlen(trans_head[i].file_name)) {
            if (trans_head[i].resv != DUMP_SEC_FILE_SAVE_END) {
                dump_error("%s not save done\n", trans_head[i].file_name);
                return false;
            }
        }
    }
    dump_print("sec log save done\n");
    return true;
}
dump_ddr_trans_head_info_s *dump_sec_get_trance_info(dump_ddr_trans_head_info_s *dump_head)
{
    u8 *addr = NULL;

    dump_ddr_trans_head_info_s *node = NULL;
    addr = bsp_dump_get_field_addr(DUMP_LRCCPU_SEC_DUMP_TARNS);
    node = dump_sec_get_modem_trance_node((dump_ddr_trans_head_info_s *)addr, dump_head);
    if (node == NULL) {
        addr = bsp_dump_get_field_addr(DUMP_NRCCPU_SEC_DUMP_TARNS);
        node = dump_sec_get_modem_trance_node((dump_ddr_trans_head_info_s *)addr, dump_head);
    }
    if (node == NULL) {
        node = dump_sec_get_modem_trance_node(g_sec_dump_drvdata.socp_src_info.dump_sec_header_vaddr, dump_head);
    }
    return node;
}
int dump_check_lr_sec_dump(void)
{
    u8 *addr = NULL;

    dump_print("start to check lr sec dump\n");

    /* 未使能安全dump，直接退出即可 */
    if (!g_sec_dump_drvdata.feature_on) {
        return true;
    }

    addr = bsp_dump_get_field_addr(DUMP_LRCCPU_SEC_DUMP_TARNS);
    return dump_sec_check_save_done((dump_ddr_trans_head_info_s *)addr);
}
int dump_check_nr_sec_dump(void)
{
    u8 *addr = NULL;
    dump_print("start to check nr sec dump\n");

    /* 未使能安全dump，直接退出即可 */
    if (!g_sec_dump_drvdata.feature_on) {
        return true;
    }
    addr = bsp_dump_get_field_addr(DUMP_NRCCPU_SEC_DUMP_TARNS);
    return dump_sec_check_save_done((dump_ddr_trans_head_info_s *)addr);
}
void dump_sec_debug(dump_ddr_trans_head_info_s *head_info, u32 flag)
{
    dump_ddr_trans_head_info_s *dump_head = NULL;
    dump_ddr_trans_head_info_s *node = NULL;
    u32 i = 0;
    if (g_sec_dump_debug == NULL) {
        dump_error("no debug space\n");
        return;
    }

    if (head_info == NULL) {
        dump_error("head_info is null\n");
        return;
    }
    dump_head = (dump_ddr_trans_head_info_s *)(uintptr_t)g_sec_dump_debug;
    for (i = 0; i < DUMP_SEC_DEBUG_SIZE / (sizeof(dump_ddr_trans_head_info_s)); i++) {
        if (dump_head[i].magic != DUMP_SEC_TRANS_FILE_MAGIC) {
            if (memcpy_s(&dump_head[i], sizeof(dump_ddr_trans_head_info_s), head_info,
                         sizeof(dump_ddr_trans_head_info_s))) {
                dump_debug("error");
            }
            break;
        }
    }
    node = dump_sec_get_trance_info(head_info);
    if (node) {
        node->resv = flag;
        dump_error("file name=%s\n", node->file_name);
    } else {
        dump_error("can not find this node\n");
    }
}

s32 dump_sec_check_head_info(dump_ddr_trans_head_info_s *head_info)
{
    dump_ddr_trans_head_info_s *node = NULL;
    node = dump_sec_get_trance_info(head_info);
    if (node != NULL) {
        if (node->total_length == head_info->total_length) {
            dump_print("%s total_len(0x%x) match\n", node->file_name, node->total_length);
            return BSP_OK;
        }
        return BSP_ERROR;
    }
    dump_print("can not get node \n");
    return BSP_ERROR;
}

void dump_sec_mask_file_saved(dump_ddr_trans_head_info_s *head_info)
{
    u32 i;
    for (i = 0; (i < SEC_DUMP_FILE_LIST_NUM) && g_sec_dump_secos_packet->file_list[i].name[0]; i++) {
        if (strncmp(head_info->file_name, g_sec_dump_secos_packet->file_list[i].name, DUMP_SECDUMP_NAME_LEN) == 0) {
            g_sec_dump_secos_packet->file_list[i].attribute &= ~DUMP_ATTR_SAVE_DEFAULT;
            break;
        }
    }
}

int dump_sec_get_file(const char *path, dump_ddr_trans_head_info_s *head_info)
{
    int fd, ret, total_length;
    struct dump_file_save_strategy log_strategy;
    char file_name[MODEM_DUMP_FILE_NAME_LENGTH] = {0};

    total_length = (int)(head_info->total_length);

    ret = dump_match_log_strategy(head_info->file_name, &log_strategy);
    if (ret) {
        if (snprintf_s(file_name, MODEM_DUMP_FILE_NAME_LENGTH, (strlen(path) + strlen(head_info->file_name)), "%s%s",
                       path, head_info->file_name) < 0) {
            dump_error("er\n");
            return -1;
        }
        fd = bsp_open((s8 *)file_name, RFILE_CREAT | RFILE_RDWR, DUMP_LOG_FILE_AUTH);
    } else {
        log_strategy.data_head.filelength = total_length;
        if (snprintf_s(file_name, MODEM_DUMP_FILE_NAME_LENGTH,
                       (strlen(path) + strlen(log_strategy.data_head.dstfilename)), "%s%s", path,
                       log_strategy.data_head.dstfilename) < 0) {
            dump_error("er\n");
            return -1;
        }
        if (dump_append_file(path, file_name, &log_strategy.data_head, sizeof(log_strategy.data_head), 0xFFFFFFFF)) {
            dump_error("add head err\n");
            return BSP_ERROR;
        }
        fd = bsp_open((s8 *)file_name, RFILE_APPEND | RFILE_RDWR, DUMP_LOG_FILE_AUTH);
    }
    return fd;
}

unsigned int dump_sec_socp_receive(socp_buffer_rw_s *socp_buff, u8 *writebuff, int total_length)
{
    void *buffer = NULL;
    void *rb_buffer = NULL;
    unsigned int rd_length = 0;

    buffer = (void *)phys_to_virt((uintptr_t)socp_buff->buffer);
    dump_sec_invalid_cache(buffer, socp_buff->size);
    if ((u32)total_length >= socp_buff->size) {
        rd_length = socp_buff->size;
        total_length -= socp_buff->size;
        if (memcpy_s(writebuff, DUMP_SEC_CODER_DST_IND_SIZE, buffer, rd_length) != EOK) {
            bsp_debug("err\n");
        }
    } else {
        rd_length = total_length;
        if (memcpy_s(writebuff, DUMP_SEC_CODER_DST_IND_SIZE, buffer, rd_length) != EOK) {
            bsp_debug("err\n");
        }
        goto recv_done;
    }

    /* 回卷长度有可能为0 */
    if ((socp_buff->rb_size) && (socp_buff->rb_buffer)) {
        rb_buffer = (void *)phys_to_virt((phys_addr_t)(uintptr_t)socp_buff->rb_buffer);
        dump_sec_invalid_cache(rb_buffer, socp_buff->rb_size);
        if (total_length >= socp_buff->rb_size) {
            if (memcpy_s((void *)(writebuff + rd_length), (DUMP_SEC_CODER_DST_IND_SIZE - rd_length), rb_buffer,
                         socp_buff->rb_size) != EOK) {
                bsp_debug("err\n");
            }
            rd_length += socp_buff->rb_size;
        } else if (total_length > 0) {
            if (memcpy_s((void *)(writebuff + rd_length), (DUMP_SEC_CODER_DST_IND_SIZE - rd_length), rb_buffer,
                         total_length) != EOK) {
                bsp_debug("err\n");
            }
            rd_length += total_length;
        }
    }
recv_done:
    /* first : read done */
    (void)bsp_socp_read_data_done(g_sec_dump_drvdata.socp_dst_info.dst_channel_id, rd_length);
    return rd_length;
}

int dump_sec_save_single_file_soc(const char *path, dump_ddr_trans_head_info_s *head_info)
{
    socp_buffer_rw_s socp_buff = {0};
    unsigned int rd_length = 0;
    unsigned int start, end;
    int wait_conter = DUMP_SECMEM_WAIT_CNT;
    int fd, ret, total_length;
    u8 *writebuff = g_sec_dump_drvdata.socp_dst_info.temp_buf;
    total_length = (int)(head_info->total_length);

    start = bsp_get_slice_value();
    fd = dump_sec_get_file(path, head_info);
    if (fd < 0) {
        dump_error("open %s  error %d!\n", head_info->file_name, fd);
        return -1;
    }
    while (total_length > 0) {
        ret = bsp_socp_get_read_buff(g_sec_dump_drvdata.socp_dst_info.dst_channel_id, &socp_buff);
        if (ret) {
            bsp_close(fd);
            dump_error("get socp buffer error!\n");
            return -1;
        }

        if ((!(socp_buff.rb_size + socp_buff.size)) && (total_length > 0)) {
            dump_print("socp buffer space is null,total len is 0x%x,wait\n", total_length);

            if (wait_conter > 0) {
                msleep(DUMP_SLEEP_30MS_NUM);
                wait_conter--;
                continue;
            } else {
                bsp_close(fd);
                dump_error("socp buffer space is null!\n");
                return -1;
            }
        }
        rd_length = dump_sec_socp_receive(&socp_buff, writebuff, total_length);
        total_length -= rd_length;
        /* second : then write file ,need to keep time for socp encoder */
        ret = (u32)bsp_write_sync((u32)fd, writebuff, rd_length);
        if (ret != rd_length) {
            dump_error("write error %d,%d!\n", ret, rd_length);
        }
    }
    (void)bsp_close((u32)fd);
    dump_sec_debug(head_info, DUMP_SEC_FILE_SAVE_END);
    end = bsp_get_slice_value();
    dump_error("%s save done! ,slice: 0x%x\n", head_info->file_name, (end - start));
    dump_sec_mask_file_saved(head_info);
    return 0;
}

/*lint -save -e574*/
int dump_sec_save_single_file(const char *path, dump_ddr_trans_head_info_s *head_info)
{
    if (head_info == NULL) {
        dump_debug("error\n");
        return -1;
    }

    if (BSP_ERROR == dump_sec_check_head_info(head_info)) {
        return -1;
    }
    dump_sec_debug(head_info, DUMP_SEC_FILE_SAVE_START);
    dump_sec_save_single_file_soc(path, head_info);
    return BSP_OK;
}

/*lint -restore +e574*/
void dump_sec_save_file(const char *path)
{
    socp_buffer_rw_s socp_buff;
    int ret = 0;
    u32 waitcount = 10;
    dump_ddr_trans_head_info_s head_info;

    /* 未使能安全dump，直接退出即可 */
    if (!g_sec_dump_drvdata.feature_on) {
        dump_print("sec dump not support\n");
        return;
    }

    if (g_sec_dump_drvdata.socp_dst_info.init_state) {
        dump_error("sec dump has not init\n");
        return;
    }
    dump_error("start to save sec log\n");
    if (memset_s(&socp_buff, sizeof(socp_buff), 0, sizeof(socp_buff)) != EOK) {
        dump_debug("err\n");
    }
    g_sec_dump_drvdata.socp_dst_info.dump_state = true;

    /* 保存过程中，如果单独复位流程启动，停止保存dump信息 */
    while (g_sec_dump_drvdata.socp_dst_info.dump_state) {
        ret = bsp_socp_get_read_buff(g_sec_dump_drvdata.socp_dst_info.dst_channel_id, &socp_buff);
        if (ret) {
            dump_error("no file need save\n");
            return;
        }

        if ((socp_buff.size + socp_buff.rb_size) <= sizeof(dump_ddr_trans_head_info_s)) { /* 是否readdone执行 */
            (void)bsp_socp_read_data_done(g_sec_dump_drvdata.socp_dst_info.dst_channel_id,
                                          (socp_buff.size + socp_buff.rb_size));
            dump_print("wait socp data!\n");
            if (waitcount--) {
                msleep(DUMP_SLEEP_5MS_NUM);
                continue;
            } else {
                dump_error("log save stop\n");
                return;
            }
        }

        /* we have already check socp rd length before, so just ignore the ret val */
        (void)dump_sec_socp_receive(&socp_buff, (u8 *)&head_info, sizeof(dump_ddr_trans_head_info_s));

        if (head_info.magic != DUMP_SEC_TRANS_FILE_MAGIC) { /* 是否readdone执行 */
            dump_error("file magic num error,readdone data 0x%x\n", head_info.magic);
            return;
        }

        (void)dump_sec_save_single_file(path, &head_info);
        waitcount = DUMP_SECDUMP_WAIT_CNTS;
    }
    return;
}

int dump_sec_trans_mem_init(void)
{
    void *virt_addr = NULL;
    dma_addr_t phy_addr = 0;

    if (g_sec_dump_drvdata.socp_dst_info.dst_buf != NULL)
        return 0;

    g_sec_dump_drvdata.socp_dst_info.temp_buf = (void *)vmalloc(g_sec_dump_drvdata.socp_dst_info.buf_len);
    if (g_sec_dump_drvdata.socp_dst_info.temp_buf == NULL) {
        dump_error("tempucBuff vmalloc fail\n");
        return -1;
    }

    if (g_dump_sec_pdev) {
        virt_addr = dma_alloc_coherent(&g_dump_sec_pdev->dev,
                                       g_sec_dump_drvdata.socp_dst_info.buf_len +
                                           g_sec_dump_secos_packet->dump_sec_bd_size +
                                           g_sec_dump_secos_packet->dump_sec_header_size,
                                       &phy_addr, GFP_KERNEL);
    }

    if (virt_addr == NULL) {
        dump_error("dma_alloc_coherent error\n");
        return -1;
    }

    g_sec_dump_drvdata.socp_dst_info.dst_buf = virt_addr;
    g_sec_dump_drvdata.socp_dst_info.dst_buf_phy = (u8 *)(uintptr_t)phy_addr;

    g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr =
        (u8 *)((uintptr_t)virt_addr + g_sec_dump_drvdata.socp_dst_info.buf_len);
    g_sec_dump_drvdata.socp_src_info.dump_sec_header_vaddr =
        (dump_ddr_trans_head_info_s *)((uintptr_t)virt_addr + g_sec_dump_drvdata.socp_dst_info.buf_len +
                                       g_sec_dump_secos_packet->dump_sec_bd_size);
    g_sec_dump_drvdata.socp_src_info.dump_sec_bd_paddr = phy_addr + g_sec_dump_drvdata.socp_dst_info.buf_len;

    return 0;
}

int dump_sec_dst_channel_free(void)
{
    /* 未使能安全dump，直接退出即可 */
    if (!g_sec_dump_drvdata.feature_on) {
        return BSP_OK;
    }

    bsp_socp_free_enc_dst_chan(g_sec_dump_drvdata.socp_dst_info.dst_channel_id);
    if (g_dump_sec_pdev) {
        dma_free_coherent(&g_dump_sec_pdev->dev,
                          g_sec_dump_drvdata.socp_dst_info.buf_len + g_sec_dump_secos_packet->dump_sec_bd_size +
                              g_sec_dump_secos_packet->dump_sec_header_size,
                          g_sec_dump_drvdata.socp_dst_info.dst_buf,
                          (dma_addr_t)(uintptr_t)g_sec_dump_drvdata.socp_dst_info.dst_buf_phy);
    }
    g_sec_dump_drvdata.socp_dst_info.dst_buf = NULL;
    g_sec_dump_drvdata.socp_dst_info.dst_buf_phy = 0;
    if (g_sec_dump_drvdata.socp_dst_info.temp_buf != NULL) {
        vfree(g_sec_dump_drvdata.socp_dst_info.temp_buf);
        g_sec_dump_drvdata.socp_dst_info.temp_buf = NULL;
    }

    return BSP_OK;
}

int bsp_dump_sec_channel_free(drv_reset_cb_moment_e eparam, s32 usrdata)
{
    socp_buffer_rw_s socp_buff = {0};
    int count = 10000;
    UNUSED(eparam);
    UNUSED(usrdata);

    if (!g_sec_dump_drvdata.feature_on || g_sec_dump_drvdata.socp_dst_info.init_state)
        return 0;

    /* 防止单独复位与dump保存流程耦合 */
    if (g_sec_dump_drvdata.socp_dst_info.dump_state)
        g_sec_dump_drvdata.socp_dst_info.dump_state = false;

    g_sec_dump_secos_packet->sec_dump_cmd = 0;
    /* 通知teeos清空证书 */
    (void)bsp_sec_call(FUNC_SEC_DUMP_CHANNEL_ENABLE, (unsigned int)g_sec_dump_drvdata.secos_packet_paddr);

    /* 确保下次单独复位之后，socp目的端通道恢复到初始状态 */
    while (count > 0) {
        if ((bsp_socp_get_read_buff(g_sec_dump_drvdata.socp_dst_info.dst_channel_id, &socp_buff) == BSP_OK) &&
            (socp_buff.rb_size + socp_buff.size)) {
            dump_error("sec dump socp encDst is not null,need to delete data!\n");
            bsp_socp_read_data_done(g_sec_dump_drvdata.socp_dst_info.dst_channel_id,
                                    socp_buff.size + socp_buff.rb_size);
        } else {
            break;
        }
        count--;
    }
    (void)dump_sec_dst_channel_free();
    return 0;
}

void dump_sec_enable_trans(u32 cmd_type)
{
    int ret;

    /* 未使能安全dump，直接退出即可 */
    if (!g_sec_dump_drvdata.feature_on) {
        dump_print("sec dump not support\n");
        return;
    }
    socp_dst_channel_enable(SOCP_CODER_DST_DUMP);

    msleep(2); /* 等待2ms */

    dump_error("notify teeos enable socp channel\n");

    g_sec_dump_secos_packet->sec_dump_cmd = cmd_type;
    ret = bsp_sec_call(FUNC_SEC_DUMP_CHANNEL_ENABLE, (unsigned int)g_sec_dump_drvdata.secos_packet_paddr);
    if (ret) {
        dump_error("notify teeos error!\n");
    }
    msleep(2); /* 等待2ms */
}

void dump_sec_disable_trans(u32 cmd_type)
{
    int ret;

    /* 未使能安全dump，直接退出即可 */
    if (!g_sec_dump_drvdata.feature_on) {
        dump_print("sec dump not support\n");
        return;
    }

    dump_error("notify teeos stop socp channel\n");

    g_sec_dump_secos_packet->sec_dump_cmd = cmd_type;
    ret = bsp_sec_call(FUNC_SEC_DUMP_CHANNEL_ENABLE, (unsigned int)g_sec_dump_drvdata.secos_packet_paddr);
    if (ret) {
        dump_error("notify teeos error!\n");
    }
    msleep(2); /* 等待2ms */

    socp_dst_channel_disable(SOCP_CODER_DST_DUMP);
    msleep(2); /* 等待2ms */
}

int dump_sec_channel_init(void)
{
    socp_dst_chan_cfg_s channel;

    if (dump_sec_trans_mem_init()) {
        return -1;
    }

    /* 扩大编码目的通道1阈值门限 */
    channel.encdst_thrh = 0x2000;

    channel.coder_dst_setbuf.output_start = g_sec_dump_drvdata.socp_dst_info.dst_buf_phy;

    channel.coder_dst_setbuf.output_end =
        (g_sec_dump_drvdata.socp_dst_info.dst_buf_phy + g_sec_dump_drvdata.socp_dst_info.buf_len) - 1;

    channel.coder_dst_setbuf.threshold = g_sec_dump_drvdata.socp_dst_info.threshold;

    channel.encdst_timeout_mode = g_sec_dump_drvdata.socp_dst_info.timeout_mode;

    if (bsp_socp_coder_set_dst_chan(g_sec_dump_drvdata.socp_dst_info.dst_channel_id, &channel)) {
        g_sec_dump_drvdata.socp_dst_info.init_state = 1;
        return -1;
    }
    bsp_socp_dst_trans_id_disable(SOCP_CODER_DST_DUMP);
    /* disable encdst interupt */
    (void)bsp_socp_encdst_dsm_init(g_sec_dump_drvdata.socp_dst_info.dst_channel_id, SOCP_DEST_DSM_DISABLE);

    dump_error("sec dump channel init ok!\n");

    g_sec_dump_drvdata.socp_dst_info.init_state = 0;

    return 0;
}

static inline void dump_sec_file_extra_info_probe(dump_sec_file_info_s *dump_file_info)
{
    dump_load_info_s *dump_load = NULL;
    (void)dump_get_load_info(dump_load);
    if ((dump_load != NULL) && (dump_file_info->dumpfile == DUMP_SEC_MDM_DDR)) {
        dump_file_info->save_offset = dump_load->mdm_ddr_saveoff;
    }
}

int dump_sec_get_bd_buffer(void)
{
    u32 size = g_sec_dump_drvdata.socp_src_info.dump_sec_bd_size +
               g_sec_dump_drvdata.socp_src_info.dump_sec_header_size;
    g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr = (u8 *)kmalloc(size, GFP_KERNEL);
    if (g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr == NULL) {
        return BSP_ERROR;
    }
    (void)memset_s(g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr, size, 0, size);
    g_sec_dump_drvdata.socp_src_info.dump_sec_bd_paddr =
        virt_to_phys(g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr);
    g_sec_dump_secos_packet->dump_sec_bd_paddr = g_sec_dump_drvdata.socp_src_info.dump_sec_bd_paddr;
    g_sec_dump_drvdata.socp_src_info.dump_sec_header_vaddr =
        (dump_ddr_trans_head_info_s *)((uintptr_t)g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr +
                                       g_sec_dump_drvdata.socp_src_info.dump_sec_bd_size);
    return BSP_OK;
}

void dump_sec_free_bd_buffer(void)
{
    if (g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr != NULL) {
        kfree(g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr);
        g_sec_dump_drvdata.socp_src_info.dump_sec_bd_vaddr = NULL;
        g_sec_dump_drvdata.socp_src_info.dump_sec_bd_paddr = 0;
        g_sec_dump_secos_packet->dump_sec_bd_paddr = 0;
    }
}

int dump_save_default_sec_log_force(const char *dir_name)
{
    u32 i;
    dump_sec_file_info_s *file_info = g_sec_dump_secos_packet->file_list;
    int ret;

    /* 未使能安全dump，直接退出即可 */
    if (!g_sec_dump_drvdata.feature_on) {
        dump_print("sec dump not support\n");
        return BSP_OK;
    }

    if (file_info->name[0] == '\0') {
        return BSP_OK;
    }

    if (dump_sec_channel_init() != BSP_OK) {
        return BSP_ERROR;
    }

    for (i = 0; (i < SEC_DUMP_FILE_LIST_NUM) && file_info->name[0]; i++, file_info++) {
        dump_sec_file_extra_info_probe(file_info);
    }
    if (g_sec_dump_drvdata.socp_src_info.dump_sec_bd_size != 0) {
        if (dump_sec_get_bd_buffer() != BSP_OK) {
            dump_error("get bd_size failed.\n");
            dump_sec_dst_channel_free();
            return BSP_ERROR;
        }
    }

    g_sec_dump_secos_packet->sec_dump_cmd = MODEM_SEC_DUMP_RETRANS_LOG_CMD;
    ret = bsp_sec_call(FUNC_SEC_DUMP_CHANNEL_ENABLE, (unsigned int)g_sec_dump_drvdata.secos_packet_paddr);
    if (ret) {
        dump_error("dump_save_default_sec_log_force failed!\n");
        dump_sec_dst_channel_free();
        dump_sec_free_bd_buffer();
        return BSP_ERROR;
    }
    dump_sec_enable_trans(MODEM_SEC_DUMP_ENABLE_LR_CHANNEL_CMD);
    dump_sec_save_file(dir_name);
    dump_sec_disable_trans(MODEM_SEC_DUMP_STOP_LR_CHANNEL_CMD);
    dump_sec_free_bd_buffer();
    dump_sec_dst_channel_free();
    return BSP_OK;
}

int dump_sec_file_probe(device_node_s *dev_node)
{
    int ret;
    u32 i;
    dump_sec_file_info_s *file_info = g_sec_dump_secos_packet->file_list;
    const char *file_name = NULL;
    for (i = 0; (i < SEC_DUMP_FILE_LIST_NUM) && file_info->name[0]; i++, file_info++) {
    };
    if (i == SEC_DUMP_FILE_LIST_NUM) {
        dump_error("file_info_filed not enough, file:%s will not save!\n", dev_node->name);
        return BSP_ERROR;
    }

    if (bsp_dt_property_read_u32(dev_node, "file_id", &file_info->dumpfile)) {
        dump_error("file_id not found!\n");
        return BSP_ERROR;
    }

    if (bsp_dt_property_read_string(dev_node, "file_name", &file_name)) {
        dump_error("file_name not found!\n");
        return BSP_ERROR;
    }

    ret = (u32)strncpy_s(file_info->name, sizeof(file_info->name), file_name, strlen(file_name));
    if (ret != EOK) {
        dump_error("file_name strncpy filed!\n");
        return ret;
    }

    if (bsp_dt_property_read_u32(dev_node, "attribute", &file_info->attribute)) {
        dump_error("attribute not found!\n");
        return BSP_ERROR;
    }

    file_info->length = 0;
    file_info->phy_addr = 0;

    return BSP_OK;
}

void dump_sec_file_cfg_init(device_node_s *parent_node)
{
    device_node_s *dev_node = NULL;
    device_node_s *child = NULL;
    dev_node = bsp_dt_find_compatible_node(parent_node, NULL, "hisilicon,secdump_file_list");
    if (dev_node == NULL) {
        dump_error("secdump_file_list not found");
    }

    bsp_dt_for_each_child_of_node(dev_node, child)
    {
        (void)dump_sec_file_probe(child);
    }
}

int dump_sec_socp_cfg_init(device_node_s *parent_node)
{
    device_node_s *dev_node = NULL;
    dev_node = bsp_dt_find_compatible_node(parent_node, NULL, "hisilicon,secdump_socp_cfg");
    if (dev_node == NULL) {
        dump_error("secdump_socp_cfg not found");
        return BSP_ERROR;
    }
    if (bsp_dt_property_read_u32(dev_node, "base_addr", &g_sec_dump_drvdata.socp_src_info.socp_base_addr)) {
        dump_error("base_addr not found!\n");
        return BSP_ERROR;
    }
    if (bsp_dt_property_read_u32(dev_node, "cp_src_chnl", &g_sec_dump_drvdata.socp_src_info.socp_cpsrc_chnl_id)) {
        dump_error("cpsrc not found!\n");
        return BSP_ERROR;
    }
    if (bsp_dt_property_read_u32(dev_node, "nr_src_chnl", &g_sec_dump_drvdata.socp_src_info.socp_nrsrc_chnl_id)) {
        dump_error("no socp_nrsrc!\n");
        g_sec_dump_drvdata.socp_src_info.socp_nrsrc_chnl_id = 0;
    }
    if (bsp_dt_property_read_u32(dev_node, "bd_size", &g_sec_dump_drvdata.socp_src_info.dump_sec_bd_size)) {
        dump_error("cpsrc not found!\n");
        return BSP_ERROR;
    }
    g_sec_dump_drvdata.socp_src_info.dump_sec_header_size = SEC_DUMP_FILE_HEADER_SIZE;

    g_sec_dump_secos_packet->socp_base_addr = g_sec_dump_drvdata.socp_src_info.socp_base_addr;
    g_sec_dump_secos_packet->socp_cpsrc_chnl_id = g_sec_dump_drvdata.socp_src_info.socp_cpsrc_chnl_id;
    g_sec_dump_secos_packet->socp_nrsrc_chnl_id = g_sec_dump_drvdata.socp_src_info.socp_nrsrc_chnl_id;
    g_sec_dump_secos_packet->dump_sec_bd_size = g_sec_dump_drvdata.socp_src_info.dump_sec_bd_size;
    g_sec_dump_secos_packet->dump_sec_header_size = SEC_DUMP_FILE_HEADER_SIZE;
    g_sec_dump_secos_packet->socp_dst_channel_id = g_sec_dump_drvdata.socp_dst_info.dst_channel_id;

    return BSP_OK;
}

static int dump_sec_probe(struct platform_device *pdev)
{
    int ret;
    u8 *secos_pkt_paddr = NULL;
    u64 mask = (u64)DMA_BIT_MASK(64); /*lint !e598 !e648*/
    device_node_s *node = NULL;

    dma_set_mask_and_coherent(&pdev->dev, mask); /*lint !e598 !e648*/
    g_dump_sec_pdev = pdev;

    g_sec_dump_debug = bsp_dump_register_field(DUMP_MODEMAP_SEC_DUMP, "sec_dump",
                                               DUMP_SEC_DEBUG_SIZE + DUMP_SEC_SECOS_SIZE, 0);
    if (g_sec_dump_debug != NULL) {
        g_sec_dump_secos_packet = (dump_sec_secos_packet_s *)((uintptr_t)g_sec_dump_debug + DUMP_SEC_DEBUG_SIZE);
        if (memset_s(g_sec_dump_debug, DUMP_SEC_DEBUG_SIZE, 0, DUMP_SEC_DEBUG_SIZE) != EOK) {
            dump_debug("err\n");
        }
    } else {
        dump_debug("get sec_dump field err\n");
        return BSP_ERROR;
    }
    secos_pkt_paddr = bsp_dump_get_field_phy_addr(DUMP_MODEMAP_SEC_DUMP);
    if (secos_pkt_paddr == NULL) {
        dump_debug("err\n");
        return BSP_ERROR;
    }
    g_sec_dump_drvdata.secos_packet_paddr = (u64)(uintptr_t)secos_pkt_paddr + DUMP_SEC_DEBUG_SIZE;

    node = bsp_dt_find_compatible_node(NULL, NULL, "hisilicon,sec_dump");
    if (node == NULL) {
        dump_error("not find dump dev node\n");
        return BSP_ERROR;
    }

    dump_sec_file_cfg_init(node);
    ret = dump_sec_socp_cfg_init(node);
    if (ret != BSP_OK) {
        return ret;
    }

    g_sec_dump_drvdata.feature_on = true;

    dump_error("sec dump init ok!\n");

    return 0;
}

static int dump_sec_remove(struct platform_device *pdev)
{
    return 0;
}

static const struct of_device_id g_dump_sec_match_table[] = {
    { .compatible = "hisilicon,sec_dump" },
    {},
};

struct platform_device g_dump_sec_device = {
    .name = "dump_sec",
    .id = -1,
};

static struct platform_driver g_dump_sec_driver = {
    .probe = dump_sec_probe,
    .remove = dump_sec_remove,
    .driver = {
        .name = "dump_sec",
        .of_match_table = g_dump_sec_match_table,
        .probe_type = PROBE_FORCE_SYNCHRONOUS,
    },
};

int dump_sec_init(void)
{
    int ret;
    device_node_s *node = bsp_dt_find_compatible_node(NULL, NULL, "hisilicon,sec_dump");

    if (node == NULL) {
        dump_error("no sec_dump node");
        return BSP_OK;
    }

    if (bsp_dt_device_is_available(node) == false) {  // 如果为false时，自己创建
        ret = platform_device_register(&g_dump_sec_device);
        if (ret) {
            dump_error("platform_device_register fail");
            return ret;
        }
        of_dma_configure(&g_dump_sec_device.dev, NULL);
        dma_set_mask_and_coherent(&g_dump_sec_device.dev, DUMP_SEC_DEV_DMAMASK);
    }

    ret = platform_driver_register(&g_dump_sec_driver);
    dump_error("dump_sec_init");
    return ret;
}

