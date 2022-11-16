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
#include "at_ltev_taf_rslt_proc.h"
#include "securec.h"

#include "at_cmd_proc.h"
#include "at_cmd_msg_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_LTEV_TAF_RSLT_PROC_C

#define AT_OTHERS_FILE_TYPE 0
#define AT_FILE_TYPE_ELABEL 1
#define AT_FILE_TYPE_INDEX 2

#if (FEATURE_LTEV == FEATURE_ON)

VOS_UINT32 AT_RcvMtaVModeSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MTA_VMODE_SetCnf *rcvMsg = VOS_NULL_PTR;
    VOS_UINT32            result;
    rcvMsg                = (TAF_MTA_VMODE_SetCnf *)msg;

    /* ��ʽ��AT^VMODE��������� */
    if (rcvMsg->result != TAF_MTA_OK) {
        result = AT_ERROR;
    } else {
        result = AT_OK;
    }

    return result;
}

VOS_UINT32 AT_RcvMtaVModeQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MTA_VMODE_QryCnf *rcvMsg = VOS_NULL_PTR;
    VOS_UINT32            result;

    /* ��ʼ�� */
    rcvMsg = (TAF_MTA_VMODE_QryCnf *)msg;

    /* ��ʽ��AT^VMODE��ѯ����� */
    g_atSendDataBuff.bufLen = 0;
    if (rcvMsg->result != TAF_MTA_OK) {
        result = AT_ERROR;
    } else {
        result                  = AT_OK;
        g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "^VMODE: %d", rcvMsg->mode);
    }

    return result;
}
#endif

