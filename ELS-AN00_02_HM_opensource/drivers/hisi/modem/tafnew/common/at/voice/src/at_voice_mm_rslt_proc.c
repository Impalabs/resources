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
#include "at_voice_mm_rslt_proc.h"
#include "securec.h"
#include "at_cmd_proc.h"
#include "taf_type_def.h"
#include "at_data_proc.h"
#include "at_device_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_VOICE_MM_RSLT_PROC_C

VOS_UINT32 AT_RcvMmaCemodeQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CemodeQryCnf *cemodeQryCnf = (TAF_MMA_CemodeQryCnf *)msg;
    VOS_UINT32            result = 0;

    if (cemodeQryCnf->result == TAF_ERR_NO_ERROR) {
        g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s%s: %d%s", g_atCrLf, g_parseContext[indexNum].cmdElement->cmdName,
            cemodeQryCnf->ceMode, g_atCrLf);
        result = AT_OK;
    } else {
        result = AT_ERROR;
        g_atSendDataBuff.bufLen = 0;
    }

    return result;
}

#if (FEATURE_LTE == FEATURE_ON)
VOS_UINT32 atReadCemodeCnfProc(VOS_UINT8 indexNum, TAF_Ctrl *evtInfo)
{
    TAF_PS_CemodeCnf *cemodeReadCnf = NULL;
    VOS_UINT16        length        = 0;
    VOS_UINT32        result;

    cemodeReadCnf = (TAF_PS_CemodeCnf *)evtInfo;

    if (cemodeReadCnf->cause == VOS_OK) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "+CEMODE:");
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%d", cemodeReadCnf->cemode.currentUeMode);

        result                  = AT_OK;
        g_atSendDataBuff.bufLen = length;
    } else {
        result                  = AT_ERROR;
        g_atSendDataBuff.bufLen = 0;
    }

    AT_StopTimerCmdReady(indexNum);
    At_FormatResultData(indexNum, result);
    return VOS_OK;
}
#endif

