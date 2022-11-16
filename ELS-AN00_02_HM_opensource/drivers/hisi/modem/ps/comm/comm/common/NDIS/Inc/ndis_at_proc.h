/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Create: 2012/10/20
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 and
 *  only version 2 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3) Neither the name of Huawei nor the names of its contributors may
 *     be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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

#ifndef __NDISATPROC_H__
#define __NDISATPROC_H__

/*
 * 1 Include Headfile
 */
#include "vos.h"

/*
 * 1.1 Cplusplus Announce
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*
 * #pragma pack(*)    �����ֽڶ��뷽ʽ
 */
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif


#define NDIS_SET_INITIALSETUP_NDISENTITY_INFO(ndisEntity) do { \
        ndisEntity->rabType    = NDIS_RAB_NULL; \
        ndisEntity->handle     = NDIS_INVALID_HANDLE; \
        ndisEntity->spePort     = NDIS_INVALID_SPEPORT; \
        ndisEntity->speIpfFlag = PS_FALSE; \
} while (0)

#define NDIS_SET_EXIST_NDIS_INFO(ndisEntity, exRabId, inHandle, inSpePort, inSpeIpfFlag) do { \
        ndisEntity->used       = PS_TRUE;   /* ���ø�NDISʵ��Ϊʹ��״̬ */ \
        ndisEntity->rabId      = exRabId; /* ����չRabId�浽��ӦNDISʵ���� */ \
        ndisEntity->handle     = inHandle;  /* ����Handle��NDISʵ���� */ \
        ndisEntity->spePort     = inSpePort; /* ����SPE Port��NDISʵ���� */ \
        ndisEntity->speIpfFlag = inSpeIpfFlag; \
} while (0)

VOS_VOID Ndis_AtMsgProc(const MsgBlock *msgBlock);

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of ndis_at_proc.h */




