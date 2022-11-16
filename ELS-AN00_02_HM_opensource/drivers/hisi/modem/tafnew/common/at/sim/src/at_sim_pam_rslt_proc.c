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
#include "at_sim_pam_rslt_proc.h"
#include "securec.h"
#include "at_check_func.h"
#include "ppp_interface.h"
#include "taf_mmi_str_parse.h"
#include "at_oam_interface.h"
#include "at_data_proc.h"
#include "at_cmd_msg_proc.h"
#include "taf_std_lib.h"
#include "mn_comm_api.h"
#include "at_sim_comm.h"
#include "at_common.h"
#include "dms_msg_chk.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_SIM_PAM_RSLT_PROC_C

#define AT_ADDITION_NUMBER_MAX_NUM 3
#define AT_U16_BYTE_LEN 2
#define AT_HEX_0X_LENGTH 2
#define AT_SYMBOL_CR_LF_LEN 2
#define AT_DECODE_MAX_LENGTH 500

extern TAF_UINT32 At_SetDialNumPara(TAF_UINT8 ucIndex, MN_CALL_TypeUint8 CallType);


#if ((FEATURE_MBB_CUST == FEATURE_ON) && (FEATURE_PHONE_USIM == FEATURE_OFF))
TAF_VOID At_PbReadyIndProc(TAF_UINT8 indexNum)
{
    TAF_UINT16 length;

    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s^CPBREADY%s", g_atCrLf, g_atCrLf);

    At_SendResultData(indexNum, g_atSndCodeAddress, length);
}
#endif

/*
 * Description: �绰�������ϱ���Ϣ������
 *  1.Date: 2013-05-29
 *    Modification: Created function
 */
TAF_VOID At_PbIndMsgProc(TAF_UINT8 indexNum, SI_PB_EventInfo *event)
{
    if (event->pbEventType == SI_PB_EVENT_INFO_IND) {
        /* ��SIM����FDNʹ��ʱĬ�ϴ洢����FD */
        if ((event->pbEvent.pbInfoInd.cardType == 0) &&
            (event->pbEvent.pbInfoInd.fdnState == SI_PIH_STATE_FDN_BDN_ENABLE)) {
            g_pbatInfo.nameMaxLen = event->pbEvent.pbInfoInd.fdnTextLen;
            g_pbatInfo.numMaxLen  = event->pbEvent.pbInfoInd.fdnNumberLen;
            g_pbatInfo.total      = event->pbEvent.pbInfoInd.fdnRecordNum;
            g_pbatInfo.anrNumLen  = event->pbEvent.pbInfoInd.anrNumberLen;
            g_pbatInfo.emailLen   = event->pbEvent.pbInfoInd.emailTextLen;
        } else {
            g_pbatInfo.nameMaxLen = event->pbEvent.pbInfoInd.adnTextLen;
            g_pbatInfo.numMaxLen  = event->pbEvent.pbInfoInd.adnNumberLen;
            g_pbatInfo.total      = event->pbEvent.pbInfoInd.adnRecordNum;
            g_pbatInfo.anrNumLen  = event->pbEvent.pbInfoInd.anrNumberLen;
            g_pbatInfo.emailLen   = event->pbEvent.pbInfoInd.emailTextLen;
        }

#if ((FEATURE_MBB_CUST == FEATURE_ON) && (FEATURE_PHONE_USIM == FEATURE_OFF))
        At_PbReadyIndProc(indexNum);
#endif
    }
}

VOS_VOID AT_PB_ReadContinueProc(AT_ClientIdUint16 indexNum)
{
    AT_CommPbCtx *commPbCntxt = VOS_NULL_PTR;
    AT_UartCtx   *uartCtx     = VOS_NULL_PTR;
    VOS_UINT32    result;

    commPbCntxt = AT_GetCommPbCtxAddr();
    uartCtx     = AT_GetUartCtxAddr();
    result      = AT_SUCCESS;

    /* ������ͻ����ˮ�߻ص� */
    uartCtx->wmLowFunc = VOS_NULL_PTR;

    /* ���µ�ǰ��ȡ�ĵ绰������ */
    commPbCntxt->currIdx++;

    if (SI_PB_Read(g_atClientTab[indexNum].clientId, 0, SI_PB_STORAGE_UNSPECIFIED, commPbCntxt->currIdx,
                   commPbCntxt->currIdx) == TAF_SUCCESS) {
        return;
    } else {
        result = AT_ERROR;
    }

    g_atSendDataBuff.bufLen = 0;
    AT_StopTimerCmdReady((VOS_UINT8)indexNum);
    At_FormatResultData((VOS_UINT8)indexNum, result);
}

VOS_VOID AT_PB_ReadRspProc(VOS_UINT8 indexNum, SI_PB_EventInfo *event)
{
    AT_CommPbCtx *commPbCntxt = VOS_NULL_PTR;
    AT_UartCtx   *uartCtx     = VOS_NULL_PTR;
    VOS_UINT32    result;

    commPbCntxt = AT_GetCommPbCtxAddr();
    uartCtx     = AT_GetUartCtxAddr();
    result      = AT_SUCCESS;

    /*
     * ��ȡ����: ���մ����뷵�ؽ��
     * ��ȡ����: ����δ�ҵ��ĵ绰��
     */
    if (commPbCntxt->singleReadFlg == VOS_TRUE) {
        result = (event->pbError == TAF_ERR_NO_ERROR) ? AT_OK :
                                                        At_ChgTafErrorCode(indexNum, (VOS_UINT16)event->pbError);
    } else if ((event->pbError == TAF_ERR_NO_ERROR) || (event->pbError == TAF_ERR_PB_NOT_FOUND)) {
        /* ��鵱ǰ��ȡλ���Ƿ��Ѿ��������һ�� */
        if (commPbCntxt->currIdx == commPbCntxt->lastIdx) {
            result = AT_OK;
        } else {
            result = AT_WAIT_ASYNC_RETURN;
        }
    } else {
        result = At_ChgTafErrorCode(indexNum, (VOS_UINT16)event->pbError);
    }

    /* �绰��δ����, ��ȡ��һ���绰�� */
    if (result == AT_WAIT_ASYNC_RETURN) {
        /*
         * ������ͻ����Ѿ������ˮ��:
         * ע���ˮ�߻ص�, �����ͻ��浽���ˮ�ߺ������ȡ��һ���绰��
         */
        if (DMS_PORT_GetUartWaterMarkFlg() == VOS_TRUE) {
            uartCtx->wmLowFunc = AT_PB_ReadContinueProc;
#if (FEATURE_IOT_CMUX == FEATURE_ON)
#if (FEATURE_AT_HSUART == FEATURE_ON)
            if (DMS_PORT_GetCmuxPhyPort() == DMS_PORT_HSUART) {
                /*
                 * CMUXʹ��HSUART�˿�ע���ˮ�߻ص���
                 * ˮ�߻ص�������δЯ��CMUX�˿ںţ�
                 * ��Ҫ��¼��ǰ���ĸ��˿��ڶ��绰����
                 */
                uartCtx->index = indexNum;
            }
#endif
#endif
            return;
        }

        /* ���µ�ǰ��ȡ�ĵ绰������ */
        commPbCntxt->currIdx++;

        if (SI_PB_Read(g_atClientTab[indexNum].clientId, 0, SI_PB_STORAGE_UNSPECIFIED, commPbCntxt->currIdx,
                       commPbCntxt->currIdx) == TAF_SUCCESS) {
            return;
        } else {
            result = AT_ERROR;
        }
    }

    g_atSendDataBuff.bufLen = 0;
    AT_StopTimerCmdReady(indexNum);
    At_FormatResultData(indexNum, result);
}
/*
 * ��ʽ�����PbQueryCnf��AT����cpbs�����Ϣ
 */
LOCAL VOS_VOID AT_FormatReportPbQueryCnfCmdCpbs(TAF_UINT8 indexNum, SI_PB_EventInfo *event, TAF_UINT16 *length)
{
    if (event->storage == SI_PB_STORAGE_SM) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%s", g_atStringTab[AT_STRING_SM].text);
    } else if (event->storage == SI_PB_STORAGE_FD) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%s", g_atStringTab[AT_STRING_FD].text);
    } else if (event->storage == SI_PB_STORAGE_ON) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%s", g_atStringTab[AT_STRING_ON].text);
    } else if (event->storage == SI_PB_STORAGE_BD) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%s", g_atStringTab[AT_STRING_BD].text);
    } else {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%s", g_atStringTab[AT_STRING_EN].text);
    }

    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + *length, ",%d", event->pbEvent.pbQueryCnf.inUsedNum);
    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + *length, ",%d", event->pbEvent.pbQueryCnf.totalNum);
}
/*
 * ����at�����ʽ�����PbQueryCnf�������Ϣ
 */
LOCAL VOS_VOID AT_PrintPbQueryCnfAtcmdType(TAF_UINT8 indexNum, SI_PB_EventInfo *event, TAF_UINT16 *length)
{
    switch (g_parseContext[indexNum].cmdElement->cmdIndex) {
        case AT_CMD_CPBR:
        case AT_CMD_CPBR2:
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "(1-%d),%d,%d", g_pbatInfo.total,
                g_pbatInfo.numMaxLen, g_pbatInfo.nameMaxLen);
            break;
        case AT_CMD_CPBW:
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "(1-%d),%d,(128-255),%d", g_pbatInfo.total,
                g_pbatInfo.numMaxLen, g_pbatInfo.nameMaxLen);
            break;
        case AT_CMD_CPBW2:
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "(1-%d),%d,(128-255),%d", g_pbatInfo.total,
                g_pbatInfo.numMaxLen, g_pbatInfo.nameMaxLen);
            break;
        case AT_CMD_SCPBR:
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "(1-%d),%d,%d,%d", g_pbatInfo.total,
                g_pbatInfo.numMaxLen, g_pbatInfo.nameMaxLen, g_pbatInfo.emailLen);
            break;
        case AT_CMD_SCPBW:
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "(1-%d),%d,(128-255),%d,%d", g_pbatInfo.total,
                g_pbatInfo.numMaxLen, g_pbatInfo.nameMaxLen, g_pbatInfo.emailLen);
            break;
        case AT_CMD_CPBS:
            AT_FormatReportPbQueryCnfCmdCpbs(indexNum, event, length);
            break;
        case AT_CMD_CPBF:
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "%d,%d", g_pbatInfo.numMaxLen, g_pbatInfo.nameMaxLen);
            break;

        default:
            break;
    }
}
/*
 * ��ʽ�����PbQueryCnf�������Ϣ
 */
LOCAL VOS_UINT32 AT_FormatReportPbQueryCnf(TAF_UINT8 indexNum, SI_PB_EventInfo *event, TAF_UINT16 *length)
{
    if (event->opId == 1) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%s: (\"SM\",\"EN\",\"ON\",\"FD\")",
            g_parseContext[indexNum].cmdElement->cmdName);
        AT_StopTimerCmdReady(indexNum);
        return AT_OK;
    }

    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + *length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    g_pbatInfo.nameMaxLen = event->pbEvent.pbQueryCnf.textLen;
    g_pbatInfo.numMaxLen  = event->pbEvent.pbQueryCnf.numLen;
    g_pbatInfo.total      = event->pbEvent.pbQueryCnf.totalNum;
    g_pbatInfo.used       = event->pbEvent.pbQueryCnf.inUsedNum;
    g_pbatInfo.anrNumLen  = event->pbEvent.pbQueryCnf.anrNumberLen;
    g_pbatInfo.emailLen   = event->pbEvent.pbQueryCnf.emailTextLen;

    AT_PrintPbQueryCnfAtcmdType(indexNum, event, length);

    AT_StopTimerCmdReady(indexNum);

    return AT_OK;
}
/*
 * ���pb rsp����Ĳ���ָ���Ƿ����Ԥ��
 */
LOCAL VOS_UINT32 AT_CheckPbRspCmdCurrentOpt(TAF_UINT8 indexNum, SI_PB_EventInfo *event)
{
    VOS_UINT32 result = AT_FAILURE;
    TAF_UINT16 length = 0;

    if (!((AT_CheckHsUartUser(indexNum) == VOS_TRUE) && ((g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR2_SET) ||
                                                         (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR_SET)))) {
        if (event->pbError != TAF_ERR_NO_ERROR) { /* ���� */
            result                  = At_ChgTafErrorCode(indexNum, (TAF_UINT16)event->pbError);
            g_atSendDataBuff.bufLen = length;
            AT_StopTimerCmdReady(indexNum);
            At_FormatResultData(indexNum, result);
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}

LOCAL VOS_VOID AT_SetPbRspPbError(TAF_UINT8 indexNum, SI_PB_EventInfo *event)
{
    if ((g_pbPrintTag == VOS_FALSE) && ((g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR_SET) ||
                                        (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR2_SET))) {
        event->pbError = TAF_ERR_ERROR;
    }
}

LOCAL VOS_UINT32 AT_PbRspCmdPrint(TAF_UINT8 indexNum, SI_PB_EventInfo *event, VOS_UINT32 *result,
    TAF_UINT16 *length)
{
    if (g_pbPrintTag == TAF_FALSE) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
            (TAF_CHAR *)g_atSndCrLfAddr, "%s", "\r\n");
    }

    g_pbPrintTag = TAF_TRUE;

    if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR_SET) { /* ���� ^CPBR �ķ�ʽ���д�ӡ */
        *result = At_PbCpbrCmdPrint(indexNum, length, g_atSndCrLfAddr, event);
    } else if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR2_SET) { /* ���� +CPBR �ķ�ʽ���д�ӡ */
        *result = At_PbCpbr2CmdPrint(indexNum, length, g_atSndCrLfAddr, event);
    } else if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_SCPBR_SET) { /* ���� ^SCPBR �ķ�ʽ���д�ӡ */
        *result = At_PbScpbrCmdPrint(indexNum, length, event);
    } else if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CNUM_READ) { /* ���� CNUM �ķ�ʽ���д�ӡ */
        *result = At_PbCnumCmdPrint(indexNum, length, g_atSndCrLfAddr, event);
    } else {
        AT_ERR_LOG1("At_PbRspProc: the Cmd Current Opt %d is Unknow",
                    g_atClientTab[indexNum].cmdCurrentOpt);

        return VOS_FALSE;
    }

    if (*result == AT_SUCCESS) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
            (TAF_CHAR *)g_atSndCrLfAddr + *length, "%s", "\r\n");
    }

    At_SendResultData(indexNum, g_atSndCrLfAddr, *length);

    return VOS_TRUE;
}

VOS_UINT32 At_DialNumByIndexFromPb(VOS_UINT8 indexNum, SI_PB_EventInfo *event)
{
    errno_t    memResult;
    VOS_UINT16 numberLength;

    numberLength = TAF_MIN(event->pbEvent.pbReadCnf.pbRecord.numberLength, SI_PB_PHONENUM_MAX_LEN);

    /* ���ӵ绰�������ĺ���д��g_atParaList[1].aucPara */
    if (event->pbEvent.pbReadCnf.pbRecord.numberLength > 0) {
        memResult = memcpy_s(&g_atParaList[1].para[0], AT_PARA_MAX_LEN + 1, event->pbEvent.pbReadCnf.pbRecord.number,
                             numberLength);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_PARA_MAX_LEN + 1, numberLength);
    }
    g_atParaList[1].para[numberLength] = '\0';
    g_atParaList[1].paraLen            = numberLength;
    /* D[<dial_string>][I][G][;] */
    return At_SetDialNumPara(indexNum, g_atClientTab[indexNum].atCallInfo.callType);
}

LOCAL VOS_UINT32 AT_ProcCmdDGetNumberBeforeCall(TAF_UINT8 indexNum, SI_PB_EventInfo *event, VOS_UINT32 *result)
{
    if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_D_GET_NUMBER_BEFORE_CALL) {
        *result = At_DialNumByIndexFromPb(indexNum, event);
        if (*result == AT_WAIT_ASYNC_RETURN) {
            g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_PEND;

            /* ����ʱ�� */
            if (At_StartTimer(g_parseContext[indexNum].cmdElement->setTimeOut, indexNum) != AT_SUCCESS) {
                AT_ERR_LOG("AT_ProcCmdDGetNumberBeforeCall:ERROR:Start Timer");
            }
            *result = AT_SUCCESS;

        } else {
            *result = AT_ERROR;
        }

        return VOS_TRUE;
    }

    return VOS_FALSE;
}
/*
 * ��ʽ�����Pb READ Cnf�������Ϣ
 */
LOCAL VOS_UINT32 AT_FormatReportPbReadCnf(TAF_UINT8 indexNum, SI_PB_EventInfo *event, VOS_UINT32 *result,
    TAF_UINT16 *length)
{
    errno_t    memResult;

    if ((event->pbError == TAF_ERR_NO_ERROR) &&
        (event->pbEvent.pbReadCnf.pbRecord.validFlag == SI_PB_CONTENT_VALID)) {
        /* ��ǰ��������Ч */
        if (AT_ProcCmdDGetNumberBeforeCall(indexNum, event, result) == VOS_TRUE) {
            return VOS_TRUE;
        }

        if (AT_PbRspCmdPrint(indexNum, event, result, length) != VOS_TRUE) {
            return VOS_FALSE;
        }

        *length = 0;
    }

    memResult = memcpy_s((TAF_CHAR *)g_atSndCrLfAddr, AT_SEND_CRLF_ADDR_MAX_LEN, (TAF_CHAR *)g_atCrLf, AT_CRLF_STR_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CRLF_ADDR_MAX_LEN, AT_CRLF_STR_LEN);

    if ((AT_CheckHsUartUser(indexNum) == VOS_TRUE) &&
        ((g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR2_SET) ||
         (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR_SET))) {
        AT_PB_ReadRspProc(indexNum, event);
        return VOS_FALSE;
    }

    AT_SetPbRspPbError(indexNum, event);

    if (event->pbError != TAF_ERR_NO_ERROR) {                              /* ���� */
        *result = At_ChgTafErrorCode(indexNum, (TAF_UINT16)event->pbError); /* �������� */

        g_atSendDataBuff.bufLen = *length;
    } else {
        *result = AT_OK;
    }

    AT_StopTimerCmdReady(indexNum);

    return VOS_TRUE;
}

TAF_VOID At_PbRspProc(TAF_UINT8 indexNum, SI_PB_EventInfo *event)
{
    VOS_UINT32 result = AT_FAILURE;
    TAF_UINT16 length = 0;

    if (AT_CheckPbRspCmdCurrentOpt(indexNum, event) != VOS_TRUE) {
        return;
    }

    switch (event->pbEventType) {
        case SI_PB_EVENT_SET_CNF:
            g_pbatInfo.nameMaxLen = event->pbEvent.pbSetCnf.textLen;
            g_pbatInfo.numMaxLen  = event->pbEvent.pbSetCnf.numLen;
            g_pbatInfo.total      = event->pbEvent.pbSetCnf.totalNum;
            g_pbatInfo.used       = event->pbEvent.pbSetCnf.inUsedNum;
            g_pbatInfo.anrNumLen  = event->pbEvent.pbSetCnf.anrNumberLen;
            g_pbatInfo.emailLen   = event->pbEvent.pbSetCnf.emailTextLen;

            AT_StopTimerCmdReady(indexNum);

            result = AT_OK;

            break;

        case SI_PB_EVENT_READ_CNF:
        case SI_PB_EVENT_SREAD_CNF:
            if (AT_FormatReportPbReadCnf(indexNum, event, &result, &length) != VOS_TRUE) {
                return;
            }
            break;

        case SI_PB_EVENT_SEARCH_CNF:
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%s", "\r");
            AT_StopTimerCmdReady(indexNum);
            result = AT_OK;
            break;

        case SI_PB_EVENT_ADD_CNF:
        case SI_PB_EVENT_SADD_CNF:
        case SI_PB_EVENT_MODIFY_CNF:
        case SI_PB_EVENT_SMODIFY_CNF:
        case SI_PB_EVENT_DELETE_CNF:
            AT_StopTimerCmdReady(indexNum);
            result = AT_OK;
            break;

        case SI_PB_EVENT_QUERY_CNF:

            result = AT_FormatReportPbQueryCnf(indexNum, event, &length);
            break;

        default:
            AT_ERR_LOG1("At_PbRspProc Unknow Event %d", event->pbEventType);
            break;
    }

    g_atSendDataBuff.bufLen = length;

    At_FormatResultData(indexNum, result);
}

TAF_VOID At_TAFPbMsgProc(TAF_UINT8 *data, TAF_UINT16 len)
{
    SI_PB_EventInfo *event = TAF_NULL_PTR;
    errno_t          memResult;
    TAF_UINT8        indexNum = 0;

    if (len == 0) {
        return;
    }

    /*lint -save -e516 */
    event = (SI_PB_EventInfo *)PS_MEM_ALLOC(WUEPS_PID_AT, len);
    /*lint -restore */
    if (event == TAF_NULL_PTR) {
        return;
    }
    (VOS_VOID)memset_s(event, len, 0x00, len);

    memResult = memcpy_s(event, len, data, len);
    TAF_MEM_CHK_RTN_VAL(memResult, len, len);

    AT_LOG1("At_PbMsgProc pEvent->ClientId", event->clientId);
    AT_LOG1("At_PbMsgProc PBEventType", event->pbEventType);
    AT_LOG1("At_PbMsgProc Event Error", event->pbError);

    if (At_ClientIdToUserId(event->clientId, &indexNum) == AT_FAILURE) {
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, event);
        /*lint -restore */
        AT_WARN_LOG("At_TAFPbMsgProc At_ClientIdToUserId FAILURE");
        return;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, event);
        /*lint -restore */
        AT_WARN_LOG("At_TAFPbMsgProc: AT_BROADCAST_INDEX.");
        return;
    }

    AT_LOG1("At_PbMsgProc ucIndex", indexNum);
    AT_LOG1("g_atClientTab[ucIndex].cmdCurrentOpt", g_atClientTab[indexNum].cmdCurrentOpt);

    At_PbRspProc(indexNum, event);
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, event);
    /*lint -restore */
}

TAF_VOID At_PbMsgProc(struct MsgCB *msg)
{
    MN_APP_PbAtCnf *msgTemp  = VOS_NULL_PTR;
    TAF_UINT8       indexNum = 0;

    if (AT_ChkPbMsgLen((MSG_Header *)msg) != VOS_TRUE) {
        AT_ERR_LOG("At_PbMsgProc: message length is invalid!");
        return;
    }

    msgTemp = (MN_APP_PbAtCnf *)msg;

    AT_LOG1("At_PbMsgProc pEvent->ClientId", msgTemp->pbAtEvent.clientId);
    AT_LOG1("At_PbMsgProc PBEventType", msgTemp->pbAtEvent.pbEventType);
    AT_LOG1("At_PbMsgProc Event Error", msgTemp->pbAtEvent.pbError);

    if (At_ClientIdToUserId(msgTemp->pbAtEvent.clientId, &indexNum) == AT_FAILURE) {
        AT_ERR_LOG1("At_PbMsgProc At_ClientIdToUserId FAILURE", msgTemp->pbAtEvent.clientId);
        return;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("At_PbMsgProc: AT_BROADCAST_INDEX.");
        At_PbIndMsgProc(indexNum, &msgTemp->pbAtEvent);
        return;
    }

    AT_LOG1("At_PbMsgProc ucIndex", indexNum);
    AT_LOG1("g_atClientTab[ucIndex].cmdCurrentOpt", g_atClientTab[indexNum].cmdCurrentOpt);

    At_PbRspProc(indexNum, &msgTemp->pbAtEvent);
}

VOS_UINT32 AT_RcvDrvAgentCpnnQryRsp(struct MsgCB *msg)
{
    VOS_UINT8             indexNum = 0;
    DRV_AGENT_CpnnQryCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg        *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_CpnnQryCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentCpnnQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentCpnnQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�CPNN��ѯ����Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CPNN_READ) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    if (event->normalSrvStatus == VOS_TRUE) {
        At_FormatResultData(indexNum, AT_OK);
    } else {
        At_FormatResultData(indexNum, AT_ERROR);
    }

    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentCpnnTestRsp(struct MsgCB *msg)
{
    VOS_UINT8              indexNum = 0;
    VOS_INT                bufLen;
    DRV_AGENT_CpnnTestCnf *event  = VOS_NULL_PTR;
    DRV_AGENT_Msg         *rcvMsg = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_CpnnTestCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentCpnnTestRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentCpnnTestRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�CPNN��������Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CPNN_TEST) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    if ((event->oplExistFlg == PS_USIM_SERVICE_AVAILIABLE) && (event->pnnExistFlg == PS_USIM_SERVICE_AVAILIABLE) &&
        (event->normalSrvStatus == VOS_TRUE)) {
        bufLen = snprintf_s((VOS_CHAR *)g_atSndCodeAddress,
                            AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_INDEX_3,
                            (AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_INDEX_3) - 1,
                            "%s:(0,1)", g_parseContext[indexNum].cmdElement->cmdName);
        if (bufLen < 0) {
            At_FormatResultData(indexNum, AT_ERROR);
        } else {
            g_atSendDataBuff.bufLen = (VOS_UINT16)bufLen;
            At_FormatResultData(indexNum, AT_OK);
        }
    } else {
        At_FormatResultData(indexNum, AT_ERROR);
    }

    return VOS_OK;
}

VOS_UINT32 AT_RcvMmaSpnQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_SpnQryCnf *spnCnf = (TAF_MMA_SpnQryCnf *)msg;
    AT_ModemNetCtx    *netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);
    VOS_UINT8         *systemAppConfig = AT_GetSystemAppConfigAddr();
    VOS_UINT32         result = AT_OK;
    VOS_UINT16         length = 0;
    VOS_UINT16         datalen = 0;
    errno_t            memResult;
    TAF_PH_UsimSpn     atSPNRslt;

   (VOS_VOID)memset_s(&atSPNRslt, (VOS_SIZE_T)sizeof(atSPNRslt), 0x00, (VOS_SIZE_T)sizeof(atSPNRslt));
    if (netCtx->spnType == 1) {
        memResult = memcpy_s(&atSPNRslt, sizeof(atSPNRslt), &(spnCnf->mnMmaSpnInfo.usimSpnInfo),
                             sizeof(TAF_PH_UsimSpn));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(atSPNRslt), sizeof(TAF_PH_UsimSpn));
    } else {
        memResult = memcpy_s(&atSPNRslt, sizeof(atSPNRslt), &(spnCnf->mnMmaSpnInfo.simSpnInfo), sizeof(TAF_PH_UsimSpn));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(atSPNRslt), sizeof(TAF_PH_UsimSpn));
    }

    if (atSPNRslt.length == 0) {
        /*  SPN file not exist */
        result = AT_CME_SPN_FILE_NOT_EXISTS;
    } else {
        /* display format: ^SPN:disp_rplmn,coding,spn_name */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s:", g_parseContext[indexNum].cmdElement->cmdName);

        if (*systemAppConfig == SYSTEM_APP_WEBUI) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%d,%d,\"", atSPNRslt.dispRplmnMode & 0x03, atSPNRslt.coding);
        } else {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%d,%d,\"", atSPNRslt.dispRplmnMode & 0x01, atSPNRslt.coding);
        }

        if (atSPNRslt.coding == TAF_PH_GSM_7BIT_DEFAULT) {
            datalen = TAF_MIN(atSPNRslt.length, TAF_PH_SPN_NAME_MAXLEN);
            if ((length + TAF_PH_SPN_NAME_MAXLEN) <= AT_SEND_CODE_ADDR_MAX_LEN) {
                memResult = memcpy_s(g_atSndCodeAddress + length, AT_SEND_CODE_ADDR_MAX_LEN - length, atSPNRslt.spnName,
                                     datalen);
                TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CODE_ADDR_MAX_LEN - length,
                                    datalen);
            }
            length = length + datalen;
        } else {
            datalen = atSPNRslt.length;

            length += (VOS_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (VOS_INT8 *)g_atSndCodeAddress,
                                                          (VOS_UINT8 *)g_atSndCodeAddress + length, atSPNRslt.spnName,
                                                          (VOS_UINT16)datalen);
        }
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "\"");
    }

    g_atSendDataBuff.bufLen = length;

    return result;
}

VOS_UINT32 At_ProcPihHvsstSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_HVSST_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_HVSST_SET) {
        AT_WARN_LOG("At_ProcPihHvsstSetCnf : CmdCurrentOpt is not AT_CMD_HVSST_SET!");
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 At_HvsstQueryCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    ModemIdUint16        modemId;
    VOS_UINT32           rslt;
    TAF_NvSciCfg         sCICfg;
    VOS_UINT32           slot;
    SI_PIH_SimIndexUint8 simIndex;

    rslt = AT_GetModemIdFromClient(indexNum, &modemId);

    if (rslt != VOS_OK) {
        AT_ERR_LOG("At_HvsstQueryCnf: Get modem id fail.");
        return AT_ERROR;
    }

    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_HVSST_QRY */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_HVSST_QRY) {
        AT_WARN_LOG("At_HvsstQueryCnf : CmdCurrentOpt is not AT_CMD_HVSST_QRY!");
        return AT_ERROR;
    }

    /* ��NV�ж�ȡ��ǰSIM����SCI���� */
    (VOS_VOID)memset_s(&sCICfg, sizeof(sCICfg), 0x00, sizeof(sCICfg));

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_SCI_DSDA_CFG, &sCICfg, sizeof(sCICfg)) != NV_OK) {
        AT_ERR_LOG("At_HvsstQueryCnf: NV_ITEM_SCI_DSDA_CFG read fail!");
        return AT_ERROR;
    }

    if (modemId == MODEM_ID_0) {
        slot = sCICfg.bitCard0;
    } else if (modemId == MODEM_ID_1) {
        slot = sCICfg.bitCard1;
    } else {
        slot = sCICfg.bitCard2;
    }

    if (event->pihEvent.hvsstQueryCnf.vSimState == SI_PIH_SIM_ENABLE) {
        simIndex = SI_PIH_SIM_VIRT_SIM1;
    } else {
        simIndex = SI_PIH_SIM_REAL_SIM1;
    }

    (*length) += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + (*length), "%s: %d,%d,%d,%d", g_parseContext[indexNum].cmdElement->cmdName,
        simIndex, 1, slot, event->pihEvent.hvsstQueryCnf.cardUse);

    return AT_OK;
}

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_UiccAccessFileCnf(TAF_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CURSM_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CURSM_SET) {
        AT_WARN_LOG("AT_UiccAccessFileCnf : CmdCurrentOpt is not AT_CMD_CURSM_SET!");
        return AT_ERROR;
    }

    if ((event->pihEvent.uiccAcsFileCnf.dataLen != 0) &&
        (event->pihEvent.uiccAcsFileCnf.cmdType == SI_PIH_ACCESS_READ)) {
        (*length) += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + (*length), "^CURSM:");

        (*length) += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + (*length), "\"");
        (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                         (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                         event->pihEvent.uiccAcsFileCnf.command,
                                                         (VOS_UINT16)event->pihEvent.uiccAcsFileCnf.dataLen);
        (*length) += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + (*length), "\"");
    }

    return AT_OK;
}
#endif

VOS_UINT32 At_ProcPihCardTypeQryCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CARDTYPE_QUERY */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CARDTYPE_QUERY) {
        AT_WARN_LOG("At_ProcPihCardTypeQryCnf : CmdCurrentOpt is not AT_CMD_CARDTYPE_QUERY!");
        return AT_ERROR;
    }

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: %d, %d, %d", g_parseContext[indexNum].cmdElement->cmdName,
        event->pihEvent.cardTypeCnf.mode, event->pihEvent.cardTypeCnf.hasCModule,
        event->pihEvent.cardTypeCnf.hasGModule);

    return AT_OK;
}

VOS_UINT32 At_ProcPihCardTypeExQryCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CARDTYPEEX_QUERY */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CARDTYPEEX_QUERY) {
        AT_WARN_LOG("At_ProcPihCardTypeExQryCnf : CmdCurrentOpt is not AT_CMD_CARDTYPEEX_QUERY!");
        return AT_ERROR;
    }

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: %d, %d, %d", g_parseContext[indexNum].cmdElement->cmdName,
        event->pihEvent.cardTypeCnf.mode, event->pihEvent.cardTypeCnf.hasCModule,
        event->pihEvent.cardTypeCnf.hasGModule);

    return AT_OK;
}

VOS_UINT32 AT_RcvMmaEflociInfoSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_EflociinfoSetCnf *eflociInfoSetCnf = (TAF_MMA_EflociinfoSetCnf *)msg;

    g_atSendDataBuff.bufLen = 0;

    if (eflociInfoSetCnf->rslt == TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        return AT_OK;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_RcvMmaEflociInfoQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_EflociinfoQryCnf *efloiinfoQryCnf = (TAF_MMA_EflociinfoQryCnf *)msg;
    VOS_UINT16                length = 0;

    /* ��ѯʧ��ֱ�ӷ���ERROR */
    if (efloiinfoQryCnf->rslt != TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        return AT_ERROR;
    }

    /* <tmsi> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: 0x%08x", g_parseContext[indexNum].cmdElement->cmdName,
        (VOS_UINT32)efloiinfoQryCnf->eflociInfo.tmsi);

    /* <plmn> */
    if (efloiinfoQryCnf->eflociInfo.plmnId.mcc == 0xfff && efloiinfoQryCnf->eflociInfo.plmnId.mnc == 0xfff) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",\"ffffff\"");
    } else {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",\"%x%x%x",
            (0x0f00 & efloiinfoQryCnf->eflociInfo.plmnId.mcc) >> 8,
            (0x00f0 & efloiinfoQryCnf->eflociInfo.plmnId.mcc) >> 4, (0x000f & efloiinfoQryCnf->eflociInfo.plmnId.mcc));

        if (((0x0f00 & efloiinfoQryCnf->eflociInfo.plmnId.mnc) >> 8) != 0x0F) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%x", (0x0f00 & efloiinfoQryCnf->eflociInfo.plmnId.mnc) >> 8);
        }

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%x%x\"", (0x00f0 & efloiinfoQryCnf->eflociInfo.plmnId.mnc) >> 4,
            (0x000f & efloiinfoQryCnf->eflociInfo.plmnId.mnc));
    }
    /* <lac> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",0x%04x", efloiinfoQryCnf->eflociInfo.lac);

    /* <location_update_status> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",%d", efloiinfoQryCnf->eflociInfo.locationUpdateStatus);

    /* <rfu> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",0x%02x", efloiinfoQryCnf->eflociInfo.rfu);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_RcvMmaPsEflociInfoSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_EfpslociinfoSetCnf *psEflociInfoSetCnf = (TAF_MMA_EfpslociinfoSetCnf *)msg;

    g_atSendDataBuff.bufLen = 0;

    if (psEflociInfoSetCnf->rslt == TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        return AT_OK;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_RcvMmaPsEflociInfoQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_EfpslociinfoQryCnf *psefloiinfoQryCnf = (TAF_MMA_EfpslociinfoQryCnf *)msg;
    VOS_UINT16                  length = 0;

    /* ��ѯʧ��ֱ�ӷ���ERROR */
    if (psefloiinfoQryCnf->rslt != TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        return AT_ERROR;
    }

    /* <ptmsi> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: 0x%08x", g_parseContext[indexNum].cmdElement->cmdName,
        psefloiinfoQryCnf->psEflociInfo.pTmsi);

    /* <ptmsi_signature> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",0x%6x", psefloiinfoQryCnf->psEflociInfo.pTmsiSignature);

    /* <plmn> */
    if (psefloiinfoQryCnf->psEflociInfo.plmnId.mcc == 0xfff && psefloiinfoQryCnf->psEflociInfo.plmnId.mnc == 0xfff) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",\"ffffff\"");
    } else {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",\"%x%x%x",
            (0x0f00 & psefloiinfoQryCnf->psEflociInfo.plmnId.mcc) >> 8,
            (0x00f0 & psefloiinfoQryCnf->psEflociInfo.plmnId.mcc) >> 4,
            (0x000f & psefloiinfoQryCnf->psEflociInfo.plmnId.mcc));

        if (((0x0f00 & psefloiinfoQryCnf->psEflociInfo.plmnId.mnc) >> 8) != 0x0F) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%x",
                (0x0f00 & psefloiinfoQryCnf->psEflociInfo.plmnId.mnc) >> 8);
        }

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%x%x\"",
            (0x00f0 & psefloiinfoQryCnf->psEflociInfo.plmnId.mnc) >> 4,
            (0x000f & psefloiinfoQryCnf->psEflociInfo.plmnId.mnc));
    }
    /* <lac> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",0x%04x", psefloiinfoQryCnf->psEflociInfo.lac);

    /* <rac> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",0x%02x", psefloiinfoQryCnf->psEflociInfo.rac);

    /* <location_update_status> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",%d", psefloiinfoQryCnf->psEflociInfo.psLocationUpdateStatus);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_ProcPihCchoSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CCHO_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CCHO_SET) {
        AT_WARN_LOG("At_ProcPihCchoSetCnf : CmdCurrentOpt is not AT_CMD_CCHO_SET!");
        return AT_ERROR;
    }

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    /* <sessionid>, */
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%u", event->pihEvent.openChannelCnf.sessionID);

    return AT_OK;
}

VOS_UINT32 At_ProcPihCchpSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CCHP_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CCHP_SET) {
        AT_WARN_LOG("At_ProcPihCchpSetCnf : CmdCurrentOpt is not AT_CMD_CCHP_SET!");
        return AT_ERROR;
    }

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    /* <sessionid>, */
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%u", event->pihEvent.openChannelCnf.sessionID);

    return AT_OK;
}

VOS_UINT32 At_ProcPihPrivateCchoSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_PRIVATECCHO_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_PRIVATECCHO_SET) {
        AT_WARN_LOG("At_ProcPihPrivateCchoSetCnf : CmdCurrentOpt is not AT_CMD_PRIVATECCHO_SET!");

        return AT_ERROR;
    }

    event->pihEvent.openChannelCnf.rspDataLen = TAF_MIN(event->pihEvent.openChannelCnf.rspDataLen, SI_APDU_MAX_LEN);

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    /* <sessionid> */
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%u,", event->pihEvent.openChannelCnf.sessionID);
    /* <response len> */
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%d,\"",
        (event->pihEvent.openChannelCnf.rspDataLen + AT_DOUBLE_QUO_MARKS_LENGTH) * AT_DOUBLE_LENGTH);
    /* <response data> */
    (*length) += (VOS_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                     (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                     (TAF_UINT8 *)event->pihEvent.openChannelCnf.rspDate,
                                                     event->pihEvent.openChannelCnf.rspDataLen);
    /* SW1 */
    (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                     (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                     &event->pihEvent.openChannelCnf.sw1, sizeof(TAF_UINT8));
    /* SW2 */
    (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                     (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                     &event->pihEvent.openChannelCnf.sw2, sizeof(TAF_UINT8));
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                                   (TAF_CHAR *)g_atSndCodeAddress + (*length), "\"");

    return AT_OK;
}

VOS_UINT32 At_ProcPihPrivateCchpSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *len)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CCHP_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_PRIVATECCHP_SET) {
        AT_WARN_LOG("At_ProcPihPrivateCchpSetCnf : CmdCurrentOpt is not AT_CMD_PRIVATECCHP_SET!");

        return AT_ERROR;
    }

    (*len) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*len), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    /* <sessionid> */
    (*len) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*len), "%u,", event->pihEvent.openChannelCnf.sessionID);
    /* <response len> */
    (*len) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*len), "%d,\"",
        (event->pihEvent.openChannelCnf.rspDataLen + AT_DOUBLE_QUO_MARKS_LENGTH) * AT_DOUBLE_LENGTH);
    /* <response data> */
    (*len) += (VOS_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                  (TAF_UINT8 *)g_atSndCodeAddress + (*len),
                                                  (TAF_UINT8 *)event->pihEvent.openChannelCnf.rspDate,
                                                  event->pihEvent.openChannelCnf.rspDataLen);
    /* SW1 */
    (*len) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                  (TAF_UINT8 *)g_atSndCodeAddress + (*len),
                                                  &event->pihEvent.openChannelCnf.sw1, sizeof(TAF_UINT8));
    /* SW2 */
    (*len) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                  (TAF_UINT8 *)g_atSndCodeAddress + (*len),
                                                  &event->pihEvent.openChannelCnf.sw2, sizeof(TAF_UINT8));
    (*len) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                                (TAF_CHAR *)g_atSndCodeAddress + (*len), "\"");

    return AT_OK;
}

VOS_UINT32 At_ProcPihCchcSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CCHC_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CCHC_SET) {
        AT_WARN_LOG("At_ProcPihCchcSetCnf : CmdCurrentOpt is not AT_CMD_CCHC_SET!");
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 At_ProcPihBwtSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_BWT_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_BWT_SET) {
        AT_WARN_LOG("At_ProcPihBwtSetCnf : CmdCurrentOpt is not AT_CMD_BWT_SET!");
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 At_ProcPihCardHistoryAtrQryCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CARDHISTORY_ATR_READ */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CARD_HISTORY_ATR_READ) {
        AT_WARN_LOG("At_ProcPihCardHistoryAtrQryCnf : CmdCurrentOpt is not AT_CMD_CARDHISTORY_ATR_READ!");
        return AT_ERROR;
    }

    event->pihEvent.atrQryCnf.len = TAF_MIN(event->pihEvent.atrQryCnf.len, SI_ATR_MAX_LEN);

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s:\"", g_parseContext[indexNum].cmdElement->cmdName);

    (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                     (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                     event->pihEvent.atrQryCnf.command,
                                                     (VOS_UINT16)event->pihEvent.atrQryCnf.len);

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                                   (TAF_CHAR *)g_atSndCodeAddress + (*length), "\"");

    return AT_OK;
}

VOS_UINT32 At_ProcPihCrsmSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CRSM_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CRSM_SET) {
        AT_WARN_LOG("At_ProcPihCrsmSetCnf : CmdCurrentOpt is not AT_CMD_CRSM_SET!");
        return AT_ERROR;
    }

    event->pihEvent.rAccessCnf.len = TAF_MIN(event->pihEvent.rAccessCnf.len, USIMM_APDU_RSP_MAX_LEN);

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    /* <sw1, sw2>, */
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%d,%d", event->pihEvent.rAccessCnf.sw1,
        event->pihEvent.rAccessCnf.sw2);

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), ",\"");

    if (event->pihEvent.rAccessCnf.len != 0) {
        /* <response> */
        (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                         (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                         event->pihEvent.rAccessCnf.content,
                                                         event->pihEvent.rAccessCnf.len);
    }

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "\"");

    return AT_OK;
}

TAF_UINT32 At_PB_Unicode2UnicodePrint(TAF_UINT32 maxLength, TAF_INT8 *headaddr, TAF_UINT8 *dst, TAF_UINT8 *src,
                                      TAF_UINT16 srcLen)
{
    TAF_UINT16 len    = 0;
    TAF_UINT16 chkLen = 0;
    TAF_UINT8  high1  = 0;
    TAF_UINT8  high2  = 0;
    TAF_UINT8 *write  = dst;
    TAF_UINT8 *read   = src;

    if (((TAF_UINT32)(dst - (TAF_UINT8 *)headaddr) + (AT_ASCII_AND_HEX_CONVERSION_FACTOR * srcLen)) >= maxLength) {
        AT_ERR_LOG("At_Unicode2UnicodePrint too long");
        return 0;
    }

    /* ɨ�������ִ� */
    while (chkLen < srcLen) {
        /* ��һ���ֽ� */
        high1 = 0x0F & (*read >> 4);
        high2 = 0x0F & *read;

        if (high1 <= 0x09) { /* 0-9 */
            *write++ = high1 + 0x30;
        } else if (high1 >= 0x0A) { /* A-F */
            *write++ = high1 + 0x37;
        } else {
        }

        if (high2 <= 0x09) { /* 0-9 */
            *write++ = high2 + 0x30;
        } else if (high2 >= 0x0A) { /* A-F */
            *write++ = high2 + 0x37;
        } else {
        }

        /* ��һ���ַ� */
        chkLen++;
        read++;

        len += 2; /* ��¼���ȣ�������λ����Ϊ2 */
    }

    return len;
}

TAF_UINT16 At_PbGsmFormatPrint(TAF_UINT16 maxLength, TAF_UINT8 *dst, TAF_UINT8 *src, TAF_UINT16 srcLen)
{
    TAF_UINT32 i;

    if (srcLen > maxLength) {
        AT_ERR_LOG("At_PbGsmFormatPrint error: too long");

        return 0;
    }

    for (i = 0; i < srcLen; i++) {
        /* �����bit���� */
        dst[i] = src[i] & AT_PB_GSM7_CODE_MAX_VALUE;
    }

    return srcLen;
}

TAF_UINT32 At_PbOneUnicodeToIra(TAF_UINT16 unicodeChar, TAF_UINT8 *dst)
{
    TAF_UINT16 indexNum;

    for (indexNum = 0; indexNum < AT_PB_IRA_MAX_NUM; indexNum++) {
        if (unicodeChar == g_iraToUnicode[indexNum].unicode) {
            *dst = g_iraToUnicode[indexNum].octet;

            return AT_SUCCESS;
        }
    }

    /* δ�ҵ�UCS2��Ӧ��IRA���� */
    AT_LOG1("At_PbOneUnicodeToIra error: no found [%x]", unicodeChar);

    return AT_FAILURE;
}

TAF_UINT16 At_PbUnicodeToIraFormatPrint(TAF_UINT16 maxLength, TAF_UINT8 *dst, TAF_UINT8 *src, TAF_UINT16 srcLen)
{
    TAF_UINT16 indexNum;
    TAF_UINT16 retLen;
    TAF_UINT8 *data = src;
    TAF_UINT16 unicodeChar;

    /* IRA�����ĳ�����UCS2�������ȵ�һ�� */
    retLen = srcLen >> 1;

    /* ���洢�ռ��Ƿ��㹻 */
    if (retLen > maxLength) {
        AT_ERR_LOG("At_PbUnicodeToIraFormatPrint error: too long");

        return 0;
    }

    /* �����UCS2�ַ�ͨ�����ת��ΪIRA���� */
    for (indexNum = 0; indexNum < retLen; indexNum++) {
        /* �Ƚ�UINT8����תΪUINT16 */
        unicodeChar = (*data << 8) | (*(data + 1));
        data += AT_U16_BYTE_LEN;

        if (At_PbOneUnicodeToIra(unicodeChar, &dst[indexNum]) != AT_SUCCESS) {
            AT_ERR_LOG("At_PbUnicodeToIraFormatPrint error: no found");

            return 0;
        }
    }

    return retLen;
}

TAF_VOID At_PbUnicode82FormatPrint(TAF_UINT8 *puc82Code, VOS_UINT32 srcCodeLen,
                                   TAF_UINT8 *dst, VOS_UINT32 decBufLen, TAF_UINT16 *dstLen)
{
    TAF_UINT32  indexNum;
    TAF_UINT16  baseCode;
    TAF_UINT16  unicodeValue;
    TAF_UINT16  srcLen;
    TAF_UINT16  dstIndex = 0;
    TAF_UINT16  gsmCodeLen;
    TAF_UINT16  gsm2UCS2CodeLen;
    TAF_UINT16 *unicode = (TAF_UINT16 *)dst;
    TAF_UINT8  *src     = VOS_NULL_PTR;

    /* ȡ��82����ĳ��� */
    srcLen = puc82Code[0];

    /* ȡ��82�����Basecode */
    baseCode = (puc82Code[1] << 8) | puc82Code[2];

    if ((srcLen > srcCodeLen) || (baseCode == 0xFFFF)) { /* ���ݳ��ȴ��� */
        *dstLen = 0;

        return;
    }

    /* ȡ��82���������, ���ĸ����Ժ��Byte��ʾ82ѹ�������������3��Ϊ�˽�ָ��ƫ�Ƶ����ĸ���ַ */
    src = puc82Code + 3;

    /* �ӵ��ĸ��ֽڿ�ʼ��82��������� */
    for (indexNum = 0; indexNum < srcLen; indexNum++) {
        gsmCodeLen = 0;
        while (((indexNum + gsmCodeLen) < srcLen) && (src[indexNum + gsmCodeLen] <= AT_PB_GSM7_CODE_MAX_VALUE)) {
            gsmCodeLen++;
        }

        if (gsmCodeLen != 0) {
            gsm2UCS2CodeLen = 0;
            At_PbGsmToUnicode((src + indexNum), gsmCodeLen, (TAF_UINT8 *)(unicode + dstIndex),
                              (decBufLen - (dstIndex << 1)), &gsm2UCS2CodeLen);

            indexNum += (gsmCodeLen - 1);
            dstIndex += (gsm2UCS2CodeLen >> 1);

            continue;
        }

        unicodeValue = baseCode + (TAF_UINT16)(src[indexNum] & AT_PB_GSM7_CODE_MAX_VALUE);

        AT_UNICODE2VALUE(unicodeValue);

        unicode[dstIndex] = unicodeValue;
        dstIndex++;
    }

    *dstLen = (TAF_UINT16)(dstIndex << 1);
}

TAF_VOID At_PbUnicode81FormatPrint(TAF_UINT8 *puc81Code, VOS_UINT32 srcCodeLen,
                                   TAF_UINT8 *dst, VOS_UINT32 decBufLen, TAF_UINT16 *dstLen)
{
    TAF_UINT32  indexNum;
    TAF_UINT16  unicodeValue;
    TAF_UINT16  baseCode;
    TAF_UINT16  srcLen;
    TAF_UINT16  dstIndex = 0;
    TAF_UINT16  gsmCodeLen;
    TAF_UINT16  gsm2UCS2CodeLen;
    TAF_UINT16 *unicode = (TAF_UINT16 *)dst;
    TAF_UINT8  *src     = VOS_NULL_PTR;

    if (puc81Code[0] > srcCodeLen) { /* �ַ��������ܴ������������ֵ */
        *dstLen = 0;

        return;
    }

    /* ȡ��81����ĳ��� */
    srcLen = puc81Code[0];

    /* ȡ��81����Ļ�ָ�� */
    baseCode = puc81Code[1] << 7;

    /* ȡ��81���������, ���������Ժ��Byte��ʾ81ѹ�������������2��Ϊ�˽�ָ��ƫ�Ƶ���������ַ */
    src = puc81Code + 2;

    /* �ӵ������ֽڿ�ʼ��81��������� */
    for (indexNum = 0; indexNum < srcLen; indexNum++) {
        gsmCodeLen = 0;
        while ((src[indexNum + gsmCodeLen] <= AT_PB_GSM7_CODE_MAX_VALUE) && ((indexNum + gsmCodeLen) < srcLen)) {
            gsmCodeLen++;
        }

        if (gsmCodeLen != 0) {
            gsm2UCS2CodeLen = 0;
            At_PbGsmToUnicode((src + indexNum), gsmCodeLen, (TAF_UINT8 *)(unicode + dstIndex),
                              (decBufLen - (dstIndex << 1)), &gsm2UCS2CodeLen);

            indexNum += (gsmCodeLen - 1);
            dstIndex += (gsm2UCS2CodeLen >> 1);

            continue;
        }

        /* 81�����Ƚ���8bit���������������� */
        unicodeValue = baseCode + (TAF_UINT16)(src[indexNum] & AT_PB_GSM7_CODE_MAX_VALUE);

        AT_UNICODE2VALUE(unicodeValue);

        unicode[dstIndex] = unicodeValue;
        dstIndex++;
    }

    *dstLen = (TAF_UINT16)(dstIndex << 1);
}

TAF_VOID At_PbUnicode80FormatPrint(TAF_UINT8 *src, TAF_UINT16 srcLen, TAF_UINT8 *dst, VOS_UINT32 decMaxBufLen, TAF_UINT16 *dstLen)
{
    errno_t memResult;

    if ((src[0] == 0xFF) && (src[1] == 0xFF)) { /* �����ַ�ΪFFFF����Ϊ��ǰ����Ϊ�� */
        *dstLen = 0;
    } else {
        *dstLen = 0;

        /* ����80��־λ */
        if (srcLen <= decMaxBufLen) {
            if (srcLen > 0) {
                memResult = memcpy_s(dst, decMaxBufLen, src, srcLen);
                TAF_MEM_CHK_RTN_VAL(memResult, decMaxBufLen, srcLen);
            }

            *dstLen = srcLen;
        }
    }
}

TAF_UINT32 At_PbGsmExtToUnicode(TAF_UINT8 gsmExtChar, TAF_UINT16 *unicodeChar)
{
    TAF_UINT32 indexNum;

    /* ����GSM��UNICODE��չ���ҵ��򷵻سɹ������򷵻�ʧ�� */
    for (indexNum = 0; indexNum < AT_PB_GSM7EXT_MAX_NUM; indexNum++) {
        if (gsmExtChar == g_gsm7extToUnicode[indexNum].octet) {
            *unicodeChar = g_gsm7extToUnicode[indexNum].unicode;

            return AT_SUCCESS;
        }
    }

    AT_LOG1("At_PbGsmExtToUnicode warning: no found [0x%x]", gsmExtChar);

    return AT_FAILURE;
}

TAF_VOID At_PbGsmToUnicode(TAF_UINT8 *src, TAF_UINT16 srcLen, TAF_UINT8 *dst, VOS_UINT32 dstBufLen, TAF_UINT16 *dstLen)
{
    TAF_UINT32  indexNum;
    TAF_UINT16  unicodeLen = 0;
    TAF_UINT16  unicodeChar;
    VOS_UINT32  dstBufUniLen = dstBufLen >> 1;
    TAF_UINT16 *unicode      = (TAF_UINT16 *)dst;

    for (indexNum = 0; ((indexNum < srcLen) && (unicodeLen < dstBufUniLen)); indexNum++) {
        /* Ϊ������Щƽ̨��GSMģʽ��д���¼ʱδ�����BIT���� */
        src[indexNum] = src[indexNum] & AT_PB_GSM7_CODE_MAX_VALUE;

        /* ��0x1B�ַ���ֱ�Ӳ�GSM��UNICODE������ */
        if (src[indexNum] != AT_PB_GSM7EXT_SYMBOL) {
            unicodeChar = g_gsmToUnicode[src[indexNum]].unicode;
            AT_UNICODE2VALUE(unicodeChar);
            TAF_PUT_HOST_UINT16(unicode, unicodeChar);
            unicode++;
            unicodeLen++;
            continue;
        }

        /* ��ǰGSM����Ϊ0x1bʱ,����Ϊ��չ��־ */
        if ((srcLen - indexNum) < AT_HEX_0X_LENGTH) {
            /* ���һ���ֽ�Ϊ0x1B����GSM��UNICODE������, ��0x1B��ΪSPACE */
            unicodeChar = g_gsmToUnicode[src[indexNum]].unicode;
            AT_UNICODE2VALUE(unicodeChar);
            TAF_PUT_HOST_UINT16(unicode, unicodeChar);
            unicode++;
            unicodeLen++;
            continue;
        }

        /*
         * ����0x1B 0x1B���������Ϊ���ǲ�֧�ֵ�������չ�����ֱ�ӽ�0x1B
         * 0x1B��ΪSPACE SPACE
         */
        if (src[indexNum + 1] == AT_PB_GSM7EXT_SYMBOL) {
            unicodeChar = g_gsmToUnicode[src[indexNum]].unicode;
            AT_UNICODE2VALUE(unicodeChar);
            TAF_PUT_HOST_UINT16(unicode, unicodeChar);
            unicode++;

            unicodeChar = g_gsmToUnicode[src[indexNum + 1]].unicode;
            AT_UNICODE2VALUE(unicodeChar);
            TAF_PUT_HOST_UINT16(unicode, unicodeChar);
            unicode++;
            indexNum++;
            unicodeLen += AT_HEX_0X_LENGTH;
            continue;
        }

        /* 0x1B����Ϊ��־������GSM��UNICODE��չ�� */
        if (At_PbGsmExtToUnicode(src[indexNum + 1], &unicodeChar) == AT_SUCCESS) {
            AT_UNICODE2VALUE(unicodeChar);
            TAF_PUT_HOST_UINT16(unicode, unicodeChar);
            unicode++;
            indexNum++;
            unicodeLen++;
            continue;
        }

        /* ����1BXX��δ����չ���У���XX��Ϊ0x1B���������ΪSPACE+XX��Ӧ���ַ� */
        unicodeChar = g_gsmToUnicode[src[indexNum]].unicode;
        AT_UNICODE2VALUE(unicodeChar);
        TAF_PUT_HOST_UINT16(unicode, unicodeChar);
        unicode++;
        unicodeLen++;
    }

    /* GSM7BIT �����п�������չ�����ַ���GSM7bit��UNICODEת�����Ȳ�һ����ԭ�������ȵ�2�� */
    *dstLen = (TAF_UINT16)(unicodeLen << 1);
}

TAF_VOID At_PbRecordToUnicode(SI_PB_EventInfo *event, TAF_UINT8 *decode, VOS_UINT32 decBufLen, TAF_UINT16 *decodeLen)
{
    TAF_UINT16 decodeLenTemp = 0;

    switch (event->pbEvent.pbReadCnf.pbRecord.alphaTagType) {
        case SI_PB_ALPHATAG_TYPE_GSM:
            At_PbGsmToUnicode(event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                              event->pbEvent.pbReadCnf.pbRecord.alphaTagLength, decode, decBufLen, &decodeLenTemp);
            break;
        case SI_PB_ALPHATAG_TYPE_UCS2_80:
            At_PbUnicode80FormatPrint(event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                      event->pbEvent.pbReadCnf.pbRecord.alphaTagLength,
                                      decode,
                                      decBufLen,
                                      &decodeLenTemp);
            break;
        case SI_PB_ALPHATAG_TYPE_UCS2_81:
            At_PbUnicode81FormatPrint(event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                      sizeof(event->pbEvent.pbReadCnf.pbRecord.alphaTag),
                                      decode, decBufLen, &decodeLenTemp);
            break;
        case SI_PB_ALPHATAG_TYPE_UCS2_82:
            At_PbUnicode82FormatPrint(event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                      sizeof(event->pbEvent.pbReadCnf.pbRecord.alphaTag),
                                      decode, decBufLen, &decodeLenTemp);
            break;
        default:
            AT_ERR_LOG("At_PbRecordToUnicode error: not support code type");

            break;
    }

    *decodeLen = decodeLenTemp;
}

TAF_UINT32 At_Pb_AlaphPrint(TAF_UINT16 *dataLen, SI_PB_EventInfo *event, TAF_UINT8 *data)
{
    TAF_UINT16 length                       = *dataLen;
    TAF_UINT16 decodeLen                    = 0;
    TAF_UINT16 returnLen                    = 0;
    TAF_UINT8  decode[AT_DECODE_MAX_LENGTH] = {0};

    if (g_atCscsType == AT_CSCS_GSM_7Bit_CODE) {
        /* ��GSM�ַ����£���֧��UCS2���� */
        if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagType == SI_PB_ALPHATAG_TYPE_UCS2_80) ||
            (event->pbEvent.pbReadCnf.pbRecord.alphaTagType == SI_PB_ALPHATAG_TYPE_UCS2_81) ||
            (event->pbEvent.pbReadCnf.pbRecord.alphaTagType == SI_PB_ALPHATAG_TYPE_UCS2_82)) {
            AT_LOG1("At_Pb_AlaphPrint error: GSM7 doesn't read UCS code[%d]",
                    event->pbEvent.pbReadCnf.pbRecord.alphaTagType);
            return AT_FAILURE;
        }

        returnLen = At_PbGsmFormatPrint((AT_CMD_MAX_LEN - length), (data + length),
                                        event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                        event->pbEvent.pbReadCnf.pbRecord.alphaTagLength);
    } else { /* ��ǰ�ַ���ΪUCS2��IRAʱ */
        /* ��ǰ�ַ���ΪUCS2��IRAʱ���ȸ��ݴ洢��ʽת��ΪUCS2���� */
        At_PbRecordToUnicode(event, decode, sizeof(decode), &decodeLen);

        /* ��ǰ�ַ���ΪIRAʱ����UCS2ת��ΪIRA������� */
        if (g_atCscsType == AT_CSCS_IRA_CODE) {
            returnLen = At_PbUnicodeToIraFormatPrint((AT_CMD_MAX_LEN - length), (data + length), decode, decodeLen);
        }

        /* ��ǰ�ַ���ΪUCS2ʱ����UCS2ת��Ϊ��ӡ��ʽ��� */
        if (g_atCscsType == AT_CSCS_UCS2_CODE) {
            returnLen = (TAF_UINT16)At_Unicode2UnicodePrint(AT_CMD_MAX_LEN, (TAF_INT8 *)data, (data + length), decode,
                                                            decodeLen);
        }
    }

    if (returnLen == 0) {
        g_atSendDataBuff.bufLen = 0;

        AT_LOG1("At_Pb_AlaphPrint error: usReturnLen = 0,g_atCscsType = [%d]", g_atCscsType);

        return AT_FAILURE;
    }

    length += returnLen;
    *dataLen = length;

    return AT_SUCCESS;
}

TAF_UINT32 At_Pb_CnumAlaphPrint(TAF_UINT16 *dataLen, SI_PB_EventInfo *event, TAF_UINT8 *data)
{
    TAF_UINT16 length                       = *dataLen;
    TAF_UINT16 decodeLen                    = 0;
    TAF_UINT16 returnLen                    = 0;
    TAF_UINT8  decode[AT_DECODE_MAX_LENGTH] = {0};

    if (g_atCscsType == AT_CSCS_GSM_7Bit_CODE) {
        /* ��GSM�ַ����£���֧��UCS2���� */
        if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagType == SI_PB_ALPHATAG_TYPE_UCS2_80) ||
            (event->pbEvent.pbReadCnf.pbRecord.alphaTagType == SI_PB_ALPHATAG_TYPE_UCS2_81) ||
            (event->pbEvent.pbReadCnf.pbRecord.alphaTagType == SI_PB_ALPHATAG_TYPE_UCS2_82)) {
            AT_LOG1("At_Pb_AlaphPrint error: GSM7 doesn't read UCS code[%d]",
                    event->pbEvent.pbReadCnf.pbRecord.alphaTagType);
            return AT_SUCCESS;
        }

        returnLen = At_PbGsmFormatPrint((AT_CMD_MAX_LEN - length), (data + length),
                                        event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                        event->pbEvent.pbReadCnf.pbRecord.alphaTagLength);
    } else { /* ��ǰ�ַ���ΪUCS2��IRAʱ */
        /* ��ǰ�ַ���ΪUCS2��IRAʱ���ȸ��ݴ洢��ʽת��ΪUCS2���� */
        At_PbRecordToUnicode(event, decode, sizeof(decode), &decodeLen);

        /* ��ǰ�ַ���ΪIRAʱ����UCS2ת��ΪIRA������� */
        if (g_atCscsType == AT_CSCS_IRA_CODE) {
            returnLen = At_PbUnicodeToIraFormatPrint((AT_CMD_MAX_LEN - length), (data + length), decode, decodeLen);
        }

        /* ��ǰ�ַ���ΪUCS2ʱ����UCS2ת��Ϊ��ӡ��ʽ��� */
        if (g_atCscsType == AT_CSCS_UCS2_CODE) {
            returnLen = (TAF_UINT16)At_Unicode2UnicodePrint(AT_CMD_MAX_LEN, (TAF_INT8 *)data, (data + length), decode,
                                                            decodeLen);
        }
    }

    if (returnLen == 0) {
        AT_LOG1("At_Pb_AlaphPrint error: usReturnLen = 0,g_atCscsType = [%d]", g_atCscsType);

        return AT_SUCCESS;
    }

    length += returnLen;
    *dataLen = length;

    return AT_SUCCESS;
}

TAF_VOID AT_Pb_NumberPrint(TAF_UINT16 *dataLen, SI_PB_EventInfo *event, TAF_UINT8 *data)
{
    errno_t    memResult;
    TAF_UINT16 length = *dataLen;

    if (event->pbEvent.pbReadCnf.pbRecord.numberLength == 0) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length,
            ",\"\",129");
    } else {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length,
            ",\"");

        if (event->pbEvent.pbReadCnf.pbRecord.numberType == PB_NUMBER_TYPE_INTERNATIONAL) {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length,
                "+");
        }

        if (event->pbEvent.pbReadCnf.pbRecord.numberLength <= (AT_SEND_CRLF_ADDR_MAX_LEN - length)) {
            memResult = memcpy_s(data + length, AT_SEND_CRLF_ADDR_MAX_LEN - length,
                                 event->pbEvent.pbReadCnf.pbRecord.number,
                                 event->pbEvent.pbReadCnf.pbRecord.numberLength);
            TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CRLF_ADDR_MAX_LEN - length,
                                event->pbEvent.pbReadCnf.pbRecord.numberLength);

            length += (TAF_UINT16)event->pbEvent.pbReadCnf.pbRecord.numberLength;
        }

        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "\"");

        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, ",%d",
            event->pbEvent.pbReadCnf.pbRecord.numberType);
    }

    *dataLen = length;
}

TAF_UINT32 At_PbCnumCmdPrint(VOS_UINT8 indexNum, TAF_UINT16 *dataLen, TAF_UINT8 *data, SI_PB_EventInfo *event)
{
    TAF_UINT16 length = *dataLen;
    TAF_UINT32 result;

    if (event->pbEvent.pbReadCnf.pbRecord.validFlag == SI_PB_CONTENT_INVALID) {
        return AT_SUCCESS;
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "%s: ",
        g_parseContext[indexNum].cmdElement->cmdName);

    /* ע��:������Ҫ���ݱ������ͽ�������������ASCII��ֱ�Ӵ�ӡ�����UNICODE��Ҫת�� */
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "\"");

    if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagLength != 0) &&
        (event->pbEvent.pbReadCnf.pbRecord.alphaTagLength <= SI_PB_ALPHATAG_MAX_LEN)) {
        result = At_Pb_CnumAlaphPrint(&length, event, data);

        if (result != AT_SUCCESS) {
            At_FormatResultData(indexNum, AT_CME_INVALID_CHARACTERS_IN_TEXT_STRING);

            return AT_FAILURE;
        }
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "\"");

    /* ���绰���뼰�������ʹ����װΪ���� */
    AT_Pb_NumberPrint(&length, event, data);

    *dataLen = length;

    return AT_SUCCESS;
}

TAF_UINT32 At_PbCpbr2CmdPrint(VOS_UINT8 indexNum, TAF_UINT16 *dataLen, TAF_UINT8 *data, SI_PB_EventInfo *event)
{
    TAF_UINT16 length = *dataLen;
    TAF_UINT32 result;

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "%s: ",
        g_parseContext[indexNum].cmdElement->cmdName);

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "%d",
        event->pbEvent.pbReadCnf.pbRecord.index);

    /* ���绰���뼰�������ʹ����װΪ���� */
    AT_Pb_NumberPrint(&length, event, data);

    /* ע��:������Ҫ���ݱ������ͽ�������������ASCII��ֱ�Ӵ�ӡ�����UNICODE��Ҫת�� */
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, ",\"");

    if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagLength != 0) &&
        (event->pbEvent.pbReadCnf.pbRecord.alphaTagLength <= SI_PB_ALPHATAG_MAX_LEN)) {
        result = At_Pb_AlaphPrint(&length, event, data);

        if (result != AT_SUCCESS) {
            At_FormatResultData(indexNum, AT_CME_INVALID_CHARACTERS_IN_TEXT_STRING);

            return AT_FAILURE;
        }
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "\"");

    *dataLen = length;

    return AT_SUCCESS;
}

TAF_UINT32 At_PbCpbrCmdPrint(VOS_UINT8 indexNum, TAF_UINT16 *dataLen, TAF_UINT8 *data, SI_PB_EventInfo *event)
{
    errno_t    memResult;
    TAF_UINT16 length = *dataLen;

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "%s: ",
        g_parseContext[indexNum].cmdElement->cmdName);

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "%d",
        event->pbEvent.pbReadCnf.pbRecord.index);

    /* ���绰���뼰�������ʹ����װΪ���� */
    AT_Pb_NumberPrint(&length, event, data);

    /* ע��:������Ҫ���ݱ������ͽ�������������ASCII��ֱ�Ӵ�ӡ�����UNICODE��Ҫת�� */
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, ",\"");

    if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagLength != 0) &&
        (event->pbEvent.pbReadCnf.pbRecord.alphaTagLength <= SI_PB_ALPHATAG_MAX_LEN)) {
        if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagType & 0x80) == SI_PB_ALPHATAG_TYPE_UCS2) { /* DATA:IRA */
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length,
                "%X", event->pbEvent.pbReadCnf.pbRecord.alphaTagType);

            length += (TAF_UINT16)At_PB_Unicode2UnicodePrint(AT_CMD_MAX_LEN, (TAF_INT8 *)data, data + length,
                                                             event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                                             event->pbEvent.pbReadCnf.pbRecord.alphaTagLength);

            if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagLength % 2) == 0) { /* ����ż����������Ҫ���油��FF */
                length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data,
                    (TAF_CHAR *)data + length, "FF");
            }
        } else { /* DATA:UCS2 */
            if (event->pbEvent.pbReadCnf.pbRecord.alphaTagLength <= (AT_SEND_CRLF_ADDR_MAX_LEN - length)) {
                memResult = memcpy_s((TAF_CHAR *)data + length, AT_SEND_CRLF_ADDR_MAX_LEN - length,
                                     event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                     event->pbEvent.pbReadCnf.pbRecord.alphaTagLength);
                TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CRLF_ADDR_MAX_LEN - length,
                                    event->pbEvent.pbReadCnf.pbRecord.alphaTagLength);

                length += event->pbEvent.pbReadCnf.pbRecord.alphaTagLength;
            }
        }
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, "\"");

    if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagType & 0x80) == SI_PB_ALPHATAG_TYPE_UCS2) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, ",1");
    } else {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)data, (TAF_CHAR *)data + length, ",0");
    }

    *dataLen = length;

    return AT_SUCCESS;
}

TAF_VOID At_PbEmailPrint(TAF_UINT8 *src, TAF_UINT16 srcLen, TAF_UINT8 *dst, TAF_UINT16 *dstLen)
{
    TAF_UINT8  uCS2Code[2 * SI_PB_EMAIL_MAX_LEN];
    TAF_UINT16 uCS2CodeLen;
    TAF_UINT16 eMailLen = srcLen;

    TAF_UINT16 returnLen;

    (VOS_VOID)memset_s(uCS2Code, sizeof(uCS2Code), 0x00, sizeof(uCS2Code));

    /* ��EMAIL���ȴ���64Byte�����нضϴ��� */
    if (eMailLen > SI_PB_EMAIL_MAX_LEN) {
        eMailLen = SI_PB_EMAIL_MAX_LEN;
    }

    /* �Ƚ�GSMģʽת��UCS2ģʽ */
    At_PbGsmToUnicode(src, eMailLen, uCS2Code, sizeof(uCS2Code), &uCS2CodeLen);

    /* �ٽ�UCS2ģʽת��IRAģʽ */
    returnLen = At_PbUnicodeToIraFormatPrint(srcLen, dst, uCS2Code, uCS2CodeLen);
    if (returnLen == 0) {
        AT_INFO_LOG("At_PbEmailPrint error: usReturnLen = 0");
        return;
    }

    *dstLen = (TAF_UINT16)(uCS2CodeLen >> 1);
}

VOS_VOID At_PbScpbrCmdPrintAdditionNum(SI_PB_EventInfo *event, TAF_UINT32 arrayIndex, TAF_UINT16 *length)
{
    errno_t    memResult;

    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + *length, ",\"");

    /* +2��Ϊ�˷�ֹдԽ�� */
    if ((event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].numberLength == 0) ||
        (event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].numberLength > (SI_PB_PHONENUM_MAX_LEN + 2))) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
            (TAF_CHAR *)g_atSndCrLfAddr + *length, "\",129");

        return;
    }

    if (event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].numberType == PB_NUMBER_TYPE_INTERNATIONAL) {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
            (TAF_CHAR *)g_atSndCrLfAddr + *length, "+");
    }

    if (event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].numberLength <= (AT_SEND_CRLF_ADDR_MAX_LEN -
        *length)) {
        memResult = memcpy_s(g_atSndCrLfAddr + *length, AT_SEND_CRLF_ADDR_MAX_LEN - *length,
                             event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].number,
                             event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].numberLength);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CRLF_ADDR_MAX_LEN - *length,
                            event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].numberLength);

        *length += (TAF_UINT16)event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].numberLength;
    }

    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + *length, "\"");

    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + *length, ",%d",
        event->pbEvent.pbReadCnf.pbRecord.additionNumber[arrayIndex].numberType);
    return;
}

VOS_VOID At_PbScpbrCmdPrintAlphaTag(SI_PB_EventInfo *event, TAF_UINT16 *length)
{
    errno_t    memResult;

    if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagLength != 0) &&
        (event->pbEvent.pbReadCnf.pbRecord.alphaTagLength <= SI_PB_ALPHATAG_MAX_LEN)) {
        if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagType & 0x80) == SI_PB_ALPHATAG_TYPE_UCS2) { /* DATA:IRA */
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
                (TAF_CHAR *)g_atSndCrLfAddr + *length, "%X", event->pbEvent.pbReadCnf.pbRecord.alphaTagType);

            *length += (TAF_UINT16)At_PB_Unicode2UnicodePrint(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCrLfAddr,
                                                             g_atSndCrLfAddr + *length,
                                                             event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                                             event->pbEvent.pbReadCnf.pbRecord.alphaTagLength);

            if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagLength % 2) == 0) { /* ����ż����������Ҫ���油��FF */
                *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
                    (TAF_CHAR *)g_atSndCrLfAddr + *length, "FF");
            }
        } else { /* DATA:UCS2 */
            if (event->pbEvent.pbReadCnf.pbRecord.alphaTagLength <= (AT_SEND_CRLF_ADDR_MAX_LEN - *length)) {
                memResult = memcpy_s((TAF_CHAR *)g_atSndCrLfAddr + *length, AT_SEND_CRLF_ADDR_MAX_LEN - *length,
                                     event->pbEvent.pbReadCnf.pbRecord.alphaTag,
                                     event->pbEvent.pbReadCnf.pbRecord.alphaTagLength);
                TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CRLF_ADDR_MAX_LEN - *length,
                                    event->pbEvent.pbReadCnf.pbRecord.alphaTagLength);

                *length += event->pbEvent.pbReadCnf.pbRecord.alphaTagLength;
            }
        }
    }
}

TAF_UINT32 At_PbScpbrCmdPrint(VOS_UINT8 indexNum, TAF_UINT16 *dataLen, SI_PB_EventInfo *event)
{
    TAF_UINT16 length = *dataLen;
    TAF_UINT32 i;
    TAF_UINT16 emailLen = 0;

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + length, "%d", event->pbEvent.pbReadCnf.pbRecord.index);

    /* ���绰���뼰�������ʹ����װΪ���� */
    AT_Pb_NumberPrint(&length, event, g_atSndCrLfAddr);

    for (i = 0; i < AT_ADDITION_NUMBER_MAX_NUM; i++) {
        At_PbScpbrCmdPrintAdditionNum(event, i, &length);
    }

    /* ע��:������Ҫ���ݱ������ͽ�������������ASCII��ֱ�Ӵ�ӡ�����UNICODE��Ҫת�� */
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + length, ",\"");

    At_PbScpbrCmdPrintAlphaTag(event, &length);

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + length, "\"");

    if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_SCPBR_SET) {
        if ((event->pbEvent.pbReadCnf.pbRecord.alphaTagType & 0x80) == SI_PB_ALPHATAG_TYPE_UCS2) {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
                (TAF_CHAR *)g_atSndCrLfAddr + length, ",1");
        } else {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
                (TAF_CHAR *)g_atSndCrLfAddr + length, ",0");
        }
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + length, ",\"");

    if (event->pbEvent.pbReadCnf.pbRecord.email.emailLen != 0) {
        At_PbEmailPrint(event->pbEvent.pbReadCnf.pbRecord.email.email,
                        (TAF_UINT16)event->pbEvent.pbReadCnf.pbRecord.email.emailLen,
                        (TAF_UINT8 *)(g_atSndCrLfAddr + length), &emailLen);

        length += emailLen;
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
        (TAF_CHAR *)g_atSndCrLfAddr + length, "\"");

    *dataLen = length;

    return AT_SUCCESS;
}

VOS_VOID At_PB_Convert2UnicodePrint(SI_PB_EventInfo *event, TAF_UINT16 *length)
{
    errno_t    memResult;

    if ((event->pbEvent.pbSearchCnf.pbRecord.alphaTagType & 0x80) == SI_PB_ALPHATAG_TYPE_UCS2) { /* DATA:IRA */
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%X", event->pbEvent.pbSearchCnf.pbRecord.alphaTagType);

        *length += (TAF_UINT16)At_PB_Unicode2UnicodePrint(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                         g_atSndCodeAddress + *length,
                                                         event->pbEvent.pbSearchCnf.pbRecord.alphaTag,
                                                         event->pbEvent.pbSearchCnf.pbRecord.alphaTagLength);

        if ((event->pbEvent.pbSearchCnf.pbRecord.alphaTagLength % 2) == 0) { /* ����������������Ҫ���油��FF */
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "FF");
        }
    } else {                                     /* DATA:UCS2 */
        if (g_atCscsType == AT_CSCS_UCS2_CODE) { /* +CSCS:UCS2 */
            *length += (TAF_UINT16)At_Ascii2UnicodePrint(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                        g_atSndCodeAddress + *length,
                                                        event->pbEvent.pbSearchCnf.pbRecord.alphaTag,
                                                        event->pbEvent.pbSearchCnf.pbRecord.alphaTagLength);
        } else {
            if (event->pbEvent.pbSearchCnf.pbRecord.alphaTagLength <= (AT_SEND_CODE_ADDR_MAX_LEN - *length)) {
                if (event->pbEvent.pbSearchCnf.pbRecord.alphaTagLength > 0) {
                    memResult = memcpy_s((TAF_CHAR *)g_atSndCodeAddress + *length, AT_SEND_CODE_ADDR_MAX_LEN - *length,
                                         event->pbEvent.pbSearchCnf.pbRecord.alphaTag,
                                         event->pbEvent.pbSearchCnf.pbRecord.alphaTagLength);
                    TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CODE_ADDR_MAX_LEN - *length,
                                        event->pbEvent.pbSearchCnf.pbRecord.alphaTagLength);
                }

                *length += (TAF_UINT16)event->pbEvent.pbSearchCnf.pbRecord.alphaTagLength;
            }
        }
    }
}
/*
 * Description: �绰���������ݴ�ӡ����
 *  1.Date: 2009-03-14
 *    Author:
 *    Modification: Created function
 */
TAF_VOID At_PbSearchCnfProc(VOS_UINT8 indexNum, SI_PB_EventInfo *event)
{
    errno_t    memResult;
    TAF_UINT16 length = 0;

    g_pbPrintTag = TAF_TRUE;

    if (event->pbEvent.pbSearchCnf.pbRecord.validFlag == SI_PB_CONTENT_INVALID) { /* ��ǰ��������Ч */
        return;                                                                   /* ���ز����� */
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%d", event->pbEvent.pbSearchCnf.pbRecord.index);

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",\"");

    if (event->pbEvent.pbSearchCnf.pbRecord.numberType == PB_NUMBER_TYPE_INTERNATIONAL) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "+");
    }

    if (event->pbEvent.pbSearchCnf.pbRecord.numberLength <= (AT_SEND_CODE_ADDR_MAX_LEN - length)) {
        if (event->pbEvent.pbSearchCnf.pbRecord.numberLength > 0) {
            memResult = memcpy_s(g_atSndCodeAddress + length, AT_SEND_CODE_ADDR_MAX_LEN - length,
                                 event->pbEvent.pbSearchCnf.pbRecord.number,
                                 event->pbEvent.pbSearchCnf.pbRecord.numberLength);
            TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CODE_ADDR_MAX_LEN - length,
                                event->pbEvent.pbSearchCnf.pbRecord.numberLength);
        }

        length += (TAF_UINT16)event->pbEvent.pbSearchCnf.pbRecord.numberLength;
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "\"");

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d,", event->pbEvent.pbSearchCnf.pbRecord.numberType);

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "\"");

    At_PB_Convert2UnicodePrint(event, &length);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "\"");

    if (g_atVType == AT_V_ENTIRE_TYPE) {
        memResult = memcpy_s((TAF_CHAR *)g_atSndCrLfAddr, AT_SEND_CRLF_ADDR_MAX_LEN, (TAF_CHAR *)g_atCrLf,
                             AT_SYMBOL_CR_LF_LEN);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CRLF_ADDR_MAX_LEN, AT_SYMBOL_CR_LF_LEN);
        At_SendResultData(indexNum, g_atSndCrLfAddr, length + AT_SYMBOL_CR_LF_LEN);
    } else {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

        At_SendResultData(indexNum, g_atSndCodeAddress, length);
    }
}

TAF_UINT32 At_PbReadCnfProc(VOS_UINT8 indexNum, SI_PB_EventInfo *event)
{
    errno_t    memResult;
    VOS_UINT32 result = AT_FAILURE;
    VOS_UINT16 pBReadPrintLength = 0;

    if (event->pbEvent.pbReadCnf.pbRecord.validFlag == SI_PB_CONTENT_INVALID) { /* ��ǰ��������Ч */
        return AT_SUCCESS;                                                      /* ���ز����� */
    } else {
        if (g_pbPrintTag == TAF_FALSE) {
            pBReadPrintLength += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
                (TAF_CHAR *)g_atSndCrLfAddr, "%s", "\r\n");
        }

        g_pbPrintTag = TAF_TRUE;

        if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR_SET) { /* ���� ^CPBR �ķ�ʽ���д�ӡ */
            result = At_PbCpbrCmdPrint(indexNum, &pBReadPrintLength, g_atSndCrLfAddr, event);
        } else if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CPBR2_SET) { /* ���� +CPBR �ķ�ʽ���д�ӡ */
            result = At_PbCpbr2CmdPrint(indexNum, &pBReadPrintLength, g_atSndCrLfAddr, event);
        } else if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_SCPBR_SET) { /* ���� ^SCPBR �ķ�ʽ���д�ӡ */
            result = At_PbScpbrCmdPrint(indexNum, &pBReadPrintLength, event);
        } else if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CNUM_READ) { /* ���� CNUM �ķ�ʽ���д�ӡ */
            result = At_PbCnumCmdPrint(indexNum, &pBReadPrintLength, g_atSndCrLfAddr, event);
        } else {
            AT_ERR_LOG1("At_PbReadCnfProc: the Cmd Current Opt %d is Unknow", g_atClientTab[indexNum].cmdCurrentOpt);

            return AT_FAILURE;
        }
    }

    if (result == AT_SUCCESS) {
        pBReadPrintLength += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCrLfAddr,
            (TAF_CHAR *)g_atSndCrLfAddr + pBReadPrintLength, "%s", "\r\n");
    }

    At_SendResultData(indexNum, g_atSndCrLfAddr, pBReadPrintLength);

    memResult = memcpy_s((TAF_CHAR *)g_atSndCrLfAddr, AT_SEND_CRLF_ADDR_MAX_LEN, (TAF_CHAR *)g_atCrLf,
                         AT_SYMBOL_CR_LF_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, AT_SEND_CRLF_ADDR_MAX_LEN, AT_SYMBOL_CR_LF_LEN);

    return result;
}

VOS_UINT32 At_PbCallBackParamCheck(SI_PB_EventInfo *event, VOS_UINT8 *indexNum)
{
    if (event == TAF_NULL_PTR) { /* �������� */
        AT_WARN_LOG("At_PbCallBackFunc pEvent NULL");

        return VOS_ERR;
    }

    if (At_ClientIdToUserId(event->clientId, indexNum) == AT_FAILURE) { /* �ظ��Ŀͻ������ݴ��� */
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(*indexNum)) {
        AT_WARN_LOG("At_PbMsgProc : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }
    return VOS_OK;
}

VOS_UINT32 At_PbCallBackProc(SI_PB_EventInfo *event, VOS_UINT8 indexNum)
{
    if (event->pbError != TAF_ERR_NO_ERROR) { /* ����������Ϣ���� */
        return VOS_OK;
    }

    if (((event->pbEventType == SI_PB_EVENT_READ_CNF) || (event->pbEventType == SI_PB_EVENT_SREAD_CNF))
               /* ���ڶ�ȡ������Ҫ����˲���ͨ��һ���Ե���Ϣ�������ݽ��д��� */
               && (event->pbLastTag != SI_PB_LAST_TAG_TRUE)) {
        if (At_PbReadCnfProc(indexNum, event) != AT_SUCCESS) {
            AT_WARN_LOG("At_PbCallBackFunc:At_PbReadCnfProc Error");
        }

        return VOS_ERR;
    } else if (event->pbEventType == SI_PB_EVENT_SEARCH_CNF) {
        if (event->pbLastTag != SI_PB_LAST_TAG_TRUE) {
            At_PbSearchCnfProc(indexNum, event);
            return VOS_ERR;
        }
    } else { /* �����Ļظ�����ͨ����Ϣ����һ���Է��ʹ��� */
    }

    return VOS_OK;
}

MODULE_EXPORTED TAF_VOID At_PbCallBackFunc(SI_PB_EventInfo *event)
{
    TAF_UINT8  indexNum = 0;

    if (At_PbCallBackParamCheck(event, &indexNum) != VOS_OK) {
        return;
    }

    if (g_atClientTab[indexNum].cmdIndex == AT_CMD_CNUM) {
        event->pbError = ((event->pbError == TAF_ERR_PB_NOT_FOUND) ? TAF_ERR_NO_ERROR : event->pbError);
    }

    if (At_PbCallBackProc(event, indexNum) != VOS_OK) {
        return;
    }

    At_SendReportMsg(MN_CALLBACK_PHONE_BOOK, (TAF_UINT8 *)event, sizeof(SI_PB_EventInfo));
}

VOS_UINT32 At_ProcPihSessionQryCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CARDSESSION_QRY */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CARDSESSION_QRY) {
        AT_WARN_LOG("At_ProcPihSessionQryCnf: CmdCurrentOpt is not AT_CMD_CARDSESSION_QRY!");
        return AT_ERROR;
    }

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    /* <CSIM,USIM,ISIM> */
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "CSIM,%d,", event->pihEvent.sessionID[USIMM_CDMA_APP]);
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "USIM,%d,", event->pihEvent.sessionID[USIMM_GUTL_APP]);
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "ISIM,%d", event->pihEvent.sessionID[USIMM_IMS_APP]);

    return AT_OK;
}

VOS_UINT32 At_ProcPihIsdbAccessCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CISA_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CISA_SET) {
        AT_WARN_LOG("At_ProcPihIsdbAccessCnf : CmdCurrentOpt is not AT_CMD_CISA_SET!");
        return AT_ERROR;
    }

    event->pihEvent.isdbAccessCnf.len = TAF_MIN(event->pihEvent.isdbAccessCnf.len, SI_APDU_MAX_LEN);

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    /* <length>, */
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%d,\"",
        (event->pihEvent.isdbAccessCnf.len + AT_DOUBLE_QUO_MARKS_LENGTH) * AT_DOUBLE_LENGTH);
    if (event->pihEvent.isdbAccessCnf.len != 0) {
        /* <command>, */
        (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                         (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                         event->pihEvent.isdbAccessCnf.command,
                                                         event->pihEvent.isdbAccessCnf.len);
    }

    /* SW1 */
    (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                     (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                     &event->pihEvent.isdbAccessCnf.sw1, sizeof(TAF_UINT8));

    /* SW2 */
    (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                     (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                     &event->pihEvent.isdbAccessCnf.sw2, sizeof(TAF_UINT8));
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "\"");

    return AT_OK;
}

VOS_UINT32 At_ProcPihCrlaSetCnf(VOS_UINT8 indexNum, SI_PIH_EventInfo *event, VOS_UINT16 *length)
{
    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_CRLA_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_CRLA_SET) {
        AT_WARN_LOG("At_ProcPihCrlaSetCnf: CmdCurrentOpt is not AT_CMD_CRLA_SET!");
        return AT_ERROR;
    }

    event->pihEvent.rAccessCnf.len = TAF_MIN(event->pihEvent.rAccessCnf.len, USIMM_APDU_RSP_MAX_LEN);

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    /* <sw1, sw2>, */
    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "%d,%d", event->pihEvent.rAccessCnf.sw1,
        event->pihEvent.rAccessCnf.sw2);

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), ",\"");

    if (event->pihEvent.rAccessCnf.len != 0) {
        /* <response> */
        (*length) += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                         (TAF_UINT8 *)g_atSndCodeAddress + (*length),
                                                         event->pihEvent.rAccessCnf.content,
                                                         event->pihEvent.rAccessCnf.len);
    }

    (*length) += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + (*length), "\"");

    return AT_OK;
}

TAF_UINT16 At_PrintPrivateCglaResult(TAF_UINT8 indexNum, SI_PIH_EventInfo *event)
{
    VOS_UINT16 length = 0;

    /* �����һ����ӡ�ϱ�IND����Ҫ��������ӻس����� */
    if (event->pihEvent.cglaHandleCnf.lastDataFlag != VOS_TRUE) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "\r\n");
    }

    /* ^CGLA: <flag>,<length>,"[<command>]<SW1><SW2>" */
    length +=
        (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d,%d,\"", g_parseContext[indexNum].cmdElement->cmdName,
            event->pihEvent.cglaHandleCnf.lastDataFlag,
            (event->pihEvent.cglaHandleCnf.len + AT_DOUBLE_QUO_MARKS_LENGTH) * AT_DOUBLE_LENGTH);

    if (event->pihEvent.cglaHandleCnf.len != 0) {
        /* <command>, */
        length += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                      (TAF_UINT8 *)g_atSndCodeAddress + length,
                                                      event->pihEvent.cglaHandleCnf.command,
                                                      event->pihEvent.cglaHandleCnf.len);
    }

    /* <SW1> */
    length += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                  (TAF_UINT8 *)g_atSndCodeAddress + length,
                                                  &event->pihEvent.cglaHandleCnf.sw1, (VOS_UINT16)sizeof(TAF_UINT8));

    /* <SW2> */
    length += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                                                  (TAF_UINT8 *)g_atSndCodeAddress + length,
                                                  &event->pihEvent.cglaHandleCnf.sw2, (VOS_UINT16)sizeof(TAF_UINT8));
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "\"");

    return length;
}

VOS_UINT32 AT_RcvDrvAgentSimlockWriteExSetCnf(struct MsgCB *msg)
{
    DRV_AGENT_Msg                  *rcvMsg = VOS_NULL_PTR;
    DRV_AGENT_SimlockwriteexSetCnf *event  = VOS_NULL_PTR;
    VOS_UINT32                      result;
    VOS_UINT16                      length;
    VOS_UINT8                       indexNum;

    AT_PR_LOGI("enter");

    /* ��ʼ����Ϣ���� */
    rcvMsg   = (DRV_AGENT_Msg *)msg;
    event    = (DRV_AGENT_SimlockwriteexSetCnf *)rcvMsg->content;
    indexNum = 0;

    /* ͨ��ClientId��ȡucIndex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentSimlockWriteExSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentSimlockWriteExSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_SIMLOCKWRITEEX_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_SIMLOCKDATAWRITEEX_SET) {
        AT_WARN_LOG("AT_RcvDrvAgentSimlockWriteExSetCnf: CmdCurrentOpt ERR.");
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);

    length = 0;

    /* �жϲ�ѯ�����Ƿ�ɹ� */
    if (event->result == DRV_AGENT_PERSONALIZATION_NO_ERROR) {
        /* ������ý�� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, event->total);
        result = AT_OK;
    } else {
        /* �쳣���, ת�������� */
        result = AT_PERSONALIZATION_ERR_BEGIN + event->result;
    }

    g_atSendDataBuff.bufLen = length;

    /* ����AT_FormATResultDATa���������� */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}
