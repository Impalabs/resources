/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
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
#ifndef __ATLTEVASSETCMDPROC_H__
#define __ATLTEVASSETCMDPROC_H__

#include "vos.h"
#include "taf_type_def.h"
#if (FEATURE_LTEV == FEATURE_ON)
#include "taf_v2x_api.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(push, 4)
#if (FEATURE_LTEV == FEATURE_ON)

VOS_UINT32 AT_SetCatm(VOS_UINT8 indexNum);
VOS_UINT32 AT_SetCv2xL2Id(VOS_UINT8 indexNum);
VOS_UINT32 AT_SetCcutle(VOS_UINT8 indexNum);
VOS_UINT32 AT_SetCuspcreq(VOS_UINT8 indexNum);
VOS_UINT32 AT_SetCutcr(VOS_UINT8 indexNum);
VOS_UINT32 AT_SetCcbrreq(VOS_UINT8 indexNum);
VOS_UINT32 AT_SetCv2xdts(VOS_UINT8 indexNum);
VOS_UINT32 AT_SetVsyncsrcrpt(VOS_UINT8 indexNum);
VOS_UINT32 AT_SetVsyncmode(VOS_UINT8 indexNum);
VOS_UINT32 VRRC_SetGnssInfo(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId);
extern VOS_UINT32 AT_SetGnssInfo(VOS_UINT8 indexNum);
VOS_UINT32 VRRC_GetGnssInfo(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId);
extern VOS_UINT32 AT_GnssInfo(VOS_UINT8 indexNum);
VOS_UINT32 VRRC_SetRsuVPhyStatClr(VOS_UINT32 senderPid, MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId,
                                  const AT_VRRC_VphyStatClrSetReq *setParams);
extern VOS_UINT32 AT_SetVPhyStatClr(VOS_UINT8 indexNum);
VOS_UINT32 VRRC_SetTxPower(VOS_UINT32 ulSenderPid, MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId,
                           const AT_VRRC_SetTxPowerReq *setParams);
VOS_UINT32 AT_SetVTxPower(VOS_UINT8 indexNum);

#endif

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif