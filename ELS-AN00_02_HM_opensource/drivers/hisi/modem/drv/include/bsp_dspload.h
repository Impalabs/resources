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

#ifndef __BSP_DSPLOAD_H__
#define __BSP_DSPLOAD_H__

#include <bsp_shared_ddr.h>
#include <of.h>

struct bbe_dump_proc_flag
{
    unsigned int dsp_dump_flag;         /* 异常流程中，记录dsp保存tcm的过程标志 */
};

struct dsp_img_head_info
{
    u8 dsp_img_type :4;                 /*dsp镜像类型*/
    u8 reserved :4;
};

typedef enum{
    NORMAL = 0,
    WAITI = 1,
}P_WAIT_MODE;

typedef struct
{
    u8                          ucLoadType  :2;             /* 加载类型: 0-每次上电时都加载; 1-上电时只加载一次; 2-不需要底软加载 */
    u8                          ucStoreType :2;             /* 保存类型 */
    u8                          ucSectOnSite:2;             /* 段在位标志 */
    u8                          ucRsv       :2;             /* 保留比特 */
}UNIPHY_SECT_LOAD_STORE_TYPE_STRU;

struct dsp_sect_desc_stru
{
    u8                               usNo;                   /* 段序号 */
    u8                               ucCrc8;                 /* 改为校验和 2017/08/25 */
    u8                               ucTcmType;              /* 段类型: 0-代码(text); 1-rodata; 2-data; 3-bss */
    UNIPHY_SECT_LOAD_STORE_TYPE_STRU ucLoadStoreType;   /* 扩展为结构体 */
    u32                              ulFileOffset;           /* 段在文件内的偏移 */
    u32                              ulTargetAddr;           /* 加载的目标地址 */
    u32                              ulSectSize;             /* 段的大小 */
};
/*lint -e43*/
/* TLBBE16/CBBE16镜像头 */
struct dsp_bin_header_stru
{
    s8                          acDescription[24];      /* 由工具生成，内容为处理器标记和日期、时间 */
    u32                         ulFileSize;             /* 文件大小 */
    u32                         ulSectNum;              /* 段个数 */
    struct dsp_sect_desc_stru   astSect[0];             /* 段信息 */
};
/*lint +e43*/
#ifndef CONFIG_CBBE

#ifdef __KERNEL__
static inline int his_load_cdsp_image(void)
{
    return 0;
}
#endif

#else
#ifdef __KERNEL__
int his_load_cdsp_image(void);
#endif
#endif

#endif

