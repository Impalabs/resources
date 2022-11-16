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

#ifndef __ACORE_NV_STRU_GUCTTF_H__
#define __ACORE_NV_STRU_GUCTTF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "vos.h"
#include "acore_nv_id_gucttf.h"

#pragma pack(push, 4)

enum TTF_AcoreBool {
    TTF_ACORE_FALSE = 0,
    TTF_ACORE_TRUE  = 1,

    TTF_ACORE_BOOL_BUTT
};
typedef VOS_UINT8 TTF_AcoreBoolUint8;


/*
 * ��ӦNV_ITEM_NETFILTER_HOOK_MASK��Ľṹ,���ù������NV��
 * �ṹ˵��  : ���ù������NV�Ԥ��5��������ϣ�netfilterPara1-netfilterPara5��ÿ������ȡֵ��ΧΪ0-FFFFFFFF,
 * ��������λΪ1����������λ��Ӧ�Ĺ��Ӻ������ܻᱻע�ᵽ�ں���
 */
typedef struct {
    VOS_UINT32 netfilterPara1; /* ���Ӻ����������1����Ӧ�������ź�ARP�Ĺ��Ӻ���������������ʱ������ֵ����Ϊ100����û������ʱ������ֵ����Ϊ96�� */
    VOS_UINT32 netfilterPara2; /* ���Ӻ����������2����Ӧ����IPЭ��ջ��ڴ��Ĺ��Ӻ�����ȡֵΪ4224�� */
    VOS_UINT32 netfilterPara3; /* ���Ӻ����������3����Ӧ����IPЭ��ջ���ڴ��Ĺ��Ӻ�����ȡֵΪ8448�� */

    VOS_UINT32 netfilterPara4; /* ���Ӻ����������4����Ӧ�����������صĹ��Ӻ�������������������ʱ������ֵ����Ϊ131072����û����������ʱ������ֵ����Ϊ0�� */
    VOS_UINT32 netfilterPara5; /* ���Ӻ����������5��Ԥ��������ֵ����Ϊ0�� */
} NfExtNv;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
