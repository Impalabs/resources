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

#ifndef __DUMP_SEC_MEM_H__
#define __DUMP_SEC_MEM_H__

#include <bsp_dump.h>

#define DUMP_SEC_TRANS_SIGNLE_MAX_SIZE (0xfff0)
#define DUMP_SEC_TRANS_FILE_MAGIC (0x5678fedc)
#define SEC_DUMP_FILE_HEADER_SIZE (0x1000)

#define MODEM_SEC_DUMP_ENABLE_LR_CHANNEL_CMD (0x56781234)
#define MODEM_SEC_DUMP_ENABLE_NR_CHANNEL_CMD (0x78563412)
#define MODEM_SEC_DUMP_STOP_LR_CHANNEL_CMD (0x12345678)
#define MODEM_SEC_DUMP_STOP_NR_CHANNEL_CMD (0x34127856)
#define MODEM_SEC_DUMP_INIT_CMD (0x9ABCDEF0)
#define MODEM_SEC_DUMP_RETRANS_LOG_CMD (0xDEF09ABC)

#define DUMP_ATTR_SAVE_DEFAULT BIT(0)
#define DUMP_ATTR_SAVE_EXTRA_MDM BIT(1)
#define DUMP_ATTR_SAVE_MBB BIT(2)
#define DUMP_ATTR_SAVE_PHONE BIT(3)
#define DUMP_ATTR_SAVE_MINIDUMP BIT(4)
#define DUMP_ATTR_SAVE_FULLDUMP BIT(5)
#define DUMP_ATTR_SAVE_FEATURE_LTEV BIT(6)

#define SEC_DUMP_FILE_LIST_NUM DUMP_SEC_FILE_BUTT
#define DUMP_SECDUMP_WAIT_CNTS 10

typedef struct {
    u32 init_state;     /* 通道初始化状态，初始化后自动修改 */
    u32 dst_channel_id; /* 编码目的通道ID，固定配置 */
    size_t buf_len;     /* 编码目的通道数据空间大小 */
    u32 threshold;      /* 编码目的通道阈值 */
    u32 timeout_mode;   /* 编码目的通道超时类型 */
    u8 *dst_buf;        /* 编码目的通道数据空间指针 */
    u8 *dst_buf_phy;
    void *temp_buf; /* 乒乓buff用于数据保存 */
    bool dump_state;
} dump_sec_socp_dst_cfg_s;

typedef struct {
    u32 socp_base_addr;
    u64 dump_sec_bd_paddr;
    u8 *dump_sec_bd_vaddr;
    dump_ddr_trans_head_info_s *dump_sec_header_vaddr;
    u32 dump_sec_bd_size;
    u32 dump_sec_header_size;
    u32 socp_cpsrc_chnl_id;
    u32 socp_nrsrc_chnl_id;
} dump_sec_socp_src_cfg_s;

typedef struct {
    bool feature_on;
    u32 resv;
    u64 secos_packet_paddr;
    dump_sec_socp_src_cfg_s socp_src_info;
    dump_sec_socp_dst_cfg_s socp_dst_info;
} dump_sec_drvdata_s;

/* dump_sec_file_info_s和dump_sec_secos_packet_s用于和secos交互，
 * 不能使用void*等长度和架构相关的类型，
 * 注意字节对齐，防止插入非预期的空字导致数据交互不正确 */
typedef struct __attribute__((packed, aligned(4))) {
    dump_sec_file_e dumpfile;
    u32 attribute;
    u64 phy_addr;
    u32 save_offset;
    u32 length;
    char name[DUMP_SECDUMP_NAME_LEN];
    u32 resv; /* 8byte allign */
} dump_sec_file_info_s;

typedef struct __attribute__((packed, aligned(4))) {
    u32 sec_dump_cmd;
    u32 socp_base_addr;
    u64 dump_sec_bd_paddr;
    u32 dump_sec_bd_size;
    u32 dump_sec_header_size;
    u32 dump_sec_bd_woffset;
    u32 socp_dst_channel_id;
    u32 socp_cpsrc_chnl_id;
    u32 socp_nrsrc_chnl_id;
    dump_sec_file_info_s file_list[SEC_DUMP_FILE_LIST_NUM];
} dump_sec_secos_packet_s;

#define DUMP_SEC_CODER_DST_IND_SIZE (2 * 1024 * 1024)
#define DUMP_SEC_CODER_DST_THRESHOLD (0x600)
#define DUMP_SEC_DEBUG_SIZE (0x1000)
#define DUMP_SEC_SECOS_SIZE (0x1000)
#define DUMP_SEC_FILE_SAVE_START (0xA5A5A5A5)
#define DUMP_SEC_FILE_SAVE_END (0x5A5A5A5A)
#define DUMP_SEC_FILE_RESAVE_DONE (0x6a6a6a6a)
#define DUMP_SEC_DEV_DMAMASK 0xFFFFFFF8UL
int dump_sec_init(void);
void dump_sec_enable_trans(u32 channel_id);
void dump_sec_disable_trans(u32 channel_id);
void dump_sec_save_file(const char *path);
int dump_sec_channel_init(void);
int dump_sec_dst_channel_free(void);
s32 dump_sec_free_src_channel(u32 src_channel);
int dump_check_lr_sec_dump(void);
int dump_check_nr_sec_dump(void);
int dump_save_default_sec_log_force(const char *dir_name);

#endif
