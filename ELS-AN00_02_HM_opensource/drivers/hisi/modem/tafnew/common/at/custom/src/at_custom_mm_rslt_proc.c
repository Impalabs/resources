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
#include "at_custom_mm_rslt_proc.h"
#include "securec.h"

#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_event_report.h"
#include "taf_app_mma.h"
#include "at_external_module_msg_proc.h"

#include "at_sim_pam_rslt_proc.h"
#include "at_phone_event_report.h"
#include "at_general_mm_rslt_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_MM_RSLT_PROC_C
#define AT_PUC_IMSI_ARRAY_INDEX_1 1
#define AT_PUC_IMSI_ARRAY_INDEX_2 2

#if (FEATURE_MBB_CUST == FEATURE_ON)
#define AT_HCSQ_ECIO_VALUE_MIN (-32)
#define AT_HCSQ_ECIO_VALUE_MAX 0
#define AT_HCSQ_ECIO_LEVEL_MAX 65
#define AT_HCSQ_SINR_VALUE_MAX 30
#define AT_HCSQ_SINR_VALUE_MIN (-20)
#define AT_HCSQ_SINR_LEVEL_MAX 251
#define AT_HCSQ_INVALID_SIG_VALUE 99

#define AT_CMD_RESSI_ECIO_MULTI_TWO 2
#define AT_CMD_RESSI_SINR_MULTI_FIVE 5

/* ��ASCII�ַ�ת����Unicode��ת����λ */
#define AT_EONS_ASCII_TO_UNICODE_UINT 4

#endif
VOS_UINT32 AT_QryParaHomePlmnProc(VOS_UINT8 indexNum, VOS_UINT8 opId, const VOS_UINT8 *para)
{
    errno_t    memResult;
    VOS_UINT16 length = 0;
    TAF_MMA_HplmnWithMncLen hplmn;

    (VOS_VOID)memset_s(&hplmn, sizeof(hplmn), 0x00, sizeof(hplmn));
    memResult = memcpy_s(&hplmn, sizeof(hplmn), para, sizeof(TAF_MMA_HplmnWithMncLen));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(hplmn), sizeof(TAF_MMA_HplmnWithMncLen));

    /* �ϱ�MCC��MNC */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:", (VOS_INT8 *)g_parseContext[indexNum].cmdElement->cmdName);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%X%X%X", (0x0f & hplmn.hplmn.mcc),
        (0x0f00 & hplmn.hplmn.mcc) >> 8, (0x0f0000 & hplmn.hplmn.mcc) >> 16);

    if (hplmn.hplmnMncLen == AT_MNC_LENGTH_TWO_BYTES) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%X%X", (0x0f & hplmn.hplmn.mnc),
            (0x0f00 & hplmn.hplmn.mnc) >> 8);
    } else if (hplmn.hplmnMncLen == AT_MNC_LENGTH_THREE_BYTES) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%X%X%X", (0x0f & hplmn.hplmn.mnc),
            (0x0f00 & hplmn.hplmn.mnc) >> 8, (0x0f0000 & hplmn.hplmn.mnc) >> 16);
    } else {
        AT_WARN_LOG("AT_QryParaHomePlmnProc HPLMN MNC LEN INVAILID");
    }

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_RcvMmaApHplmnQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_HomePlmnQryCnf *hplmnCnf = VOS_NULL_PTR;
    TAF_MMA_HplmnWithMncLen hplmn;

    /* ��ʼ�� */
    hplmnCnf = (TAF_MMA_HomePlmnQryCnf *)msg;

    if (hplmnCnf->errorCause == TAF_ERR_NO_ERROR) {
        (VOS_VOID)memset_s(&hplmn, sizeof(hplmn), 0x00, sizeof(hplmn));
        hplmn.hplmnMncLen = hplmnCnf->eHplmnInfo.hplmnMncLen;

        hplmn.hplmn.mcc = (hplmnCnf->eHplmnInfo.imsi[1] & 0xF0) >> 4;
        hplmn.hplmn.mcc |= (hplmnCnf->eHplmnInfo.imsi[2] & 0x0F) << 8;
        hplmn.hplmn.mcc |= (hplmnCnf->eHplmnInfo.imsi[2] & 0xF0) << 12;

        hplmn.hplmn.mnc = (hplmnCnf->eHplmnInfo.imsi[3] & 0x0F);
        hplmn.hplmn.mnc |= (hplmnCnf->eHplmnInfo.imsi[3] & 0xF0) << 4;
        hplmn.hplmn.mnc |= (hplmnCnf->eHplmnInfo.imsi[4] & 0x0F) << 16;

        return AT_QryParaHomePlmnProc(indexNum, hplmnCnf->ctrl.opId, (VOS_UINT8 *)&hplmn);
    } else {
        g_atSendDataBuff.bufLen = 0;
        return At_ChgTafErrorCode(indexNum, hplmnCnf->errorCause);
    }
}

VOS_VOID AT_ConvertImsiDigit2String(VOS_UINT8 *imsi, VOS_UINT8 *imsiString, VOS_UINT32 strBufLen)
{
    VOS_UINT8  imsiLen;
    VOS_UINT32 tempNum;
    VOS_UINT8  imsiNum[NAS_IMSI_STR_LEN];
    VOS_UINT32 i;

    AT_NORM_LOG("AT_ConvertImsiDigit2String enter.");

    if (strBufLen <= NAS_IMSI_STR_LEN) {
        return;
    }

    (VOS_VOID)memset_s(imsiNum, sizeof(imsiNum), 0x00, NAS_IMSI_STR_LEN);
    tempNum = 0;

    imsiLen = TAF_MIN(imsi[0], NAS_MAX_IMSI_LENGTH - 1);

    imsiNum[tempNum] = (imsi[AT_PUC_IMSI_ARRAY_INDEX_1] & 0xf0) >> 4;
    tempNum++;

    for (i = AT_PUC_IMSI_ARRAY_INDEX_2; i <= imsiLen; i++) {
        imsiNum[tempNum] = imsi[i] & 0x0f;
        tempNum++;
        imsiNum[tempNum] = (imsi[i] & 0xf0) >> 4;
        tempNum++;
    }

    /* ��'f'��IMSI��ת��Ϊ�ַ� */
    i = 0;

    AT_LOG1("AT_ConvertImsiDigit2String: ulTempNum", tempNum);

    while (i < tempNum) {
        if (imsiNum[i] != 0x0f) {
            /* ����֧��A~Eת�룬��10~15������ת���ɶ�Ӧ��A~E */
            if ((imsiNum[i] >= 10) && (imsiNum[i] < 15)) {
                imsiString[i] = imsiNum[i] - 10 + 'A'; /* ��ʮ������ת�����ַ� */
            } else {
                imsiString[i] = imsiNum[i] + '0';
            }

            i++;
        } else {
            break;
        }
    }

    AT_LOG1("AT_ConvertImsiDigit2String: i", i);

    imsiString[i] = '\0';

    /* GDPR�Ų鲻�ܴ�ӡIMSI */
}

VOS_UINT32 AT_RcvMmaHplmnQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_HomePlmnQryCnf *hplmnQryCnf = VOS_NULL_PTR;
    VOS_UINT16              length;
    VOS_UINT32              i;
    VOS_UINT8               imsiString[NAS_IMSI_STR_LEN + 1];

    /* �ֲ�������ʼ�� */
    (VOS_VOID)memset_s(imsiString, sizeof(imsiString), 0x00, sizeof(imsiString));
    hplmnQryCnf = (TAF_MMA_HomePlmnQryCnf *)msg;
    length      = 0;

    if ((hplmnQryCnf->eHplmnInfo.eHplmnNum == 0) || (hplmnQryCnf->errorCause != TAF_ERR_NO_ERROR)) {
        AT_WARN_LOG("AT_RcvMmaHplmnQryCnf : EHPLMN num error.");
        g_atSendDataBuff.bufLen = 0;
        return At_ChgTafErrorCode(indexNum, hplmnQryCnf->errorCause);
    }

    /* ��IMSI����ת��Ϊ�ַ��� */
    AT_ConvertImsiDigit2String(hplmnQryCnf->eHplmnInfo.imsi, imsiString, sizeof(imsiString));

    /* ��ѯ����ɹ����ϱ�AT^HPLMN */
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %s,%d,%d,", g_parseContext[indexNum].cmdElement->cmdName,
        imsiString, hplmnQryCnf->eHplmnInfo.hplmnMncLen, hplmnQryCnf->eHplmnInfo.eHplmnNum);

    for (i = 0; i < (VOS_UINT8)(hplmnQryCnf->eHplmnInfo.eHplmnNum - 1); i++) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x%x",
            (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mcc & 0x0f00) >> 8,
            (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mcc & 0xf0) >> 4,
            (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mcc & 0x0f));

        if ((hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0x0f00) == 0x0f00) {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x,",
                (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0xf0) >> 4,
                (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0x0f));
        } else {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x%x,",
                (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0x0f00) >> 8,
                (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0xf0) >> 4,
                (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0x0f));
        }
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x%x", (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mcc & 0x0f00) >> 8,
        (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mcc & 0xf0) >> 4, (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mcc & 0x0f));

    if ((hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0x0f00) == 0x0f00) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x", (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0xf0) >> 4,
            (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0x0f));
    } else {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x%x",
            (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0x0f00) >> 8,
            (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0xf0) >> 4,
            (hplmnQryCnf->eHplmnInfo.eHplmnList[i].mnc & 0x0f));
    }

    g_atSendDataBuff.bufLen = length;

    /* ��λAT״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    g_logPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_HPLMN;
    /* ������ */
    At_FormatResultData(indexNum, AT_OK);
    g_logPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_BUTT;

    return AT_CMD_NO_NEED_FORMAT_RSLT;
}

VOS_UINT32 AT_RcvMmaDplmnSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_DplmnSetCnf *dplmnSetCnf = (TAF_MMA_DplmnSetCnf *)msg;

    if (dplmnSetCnf->rslt == VOS_OK) {
        return AT_OK;
    } else {
        return AT_CME_UNKNOWN;
    }
}

VOS_UINT32 AT_RcvMmaDplmnQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    errno_t              memResult;
    TAF_MMA_DplmnQryCnf *dplmnQryCnf = (TAF_MMA_DplmnQryCnf *)msg;
    VOS_UINT8            tempVersion[NAS_VERSION_LEN + 1];

    /* ��version����ת��Ϊ�ַ��� */
    memResult = memcpy_s(tempVersion, sizeof(tempVersion), dplmnQryCnf->versionId, NAS_VERSION_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(tempVersion), NAS_VERSION_LEN);

    /* ���ַ��������� */
    tempVersion[NAS_VERSION_LEN] = '\0';

    /* ��ѯ����ɹ����ϱ�^DPLMN�汾�� */
    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %s", g_parseContext[indexNum].cmdElement->cmdName, tempVersion);

    return AT_OK;
}

VOS_UINT32 AT_RcvMmaBorderInfoSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_BorderInfoSetCnf *setCnf = (TAF_MMA_BorderInfoSetCnf *)msg;

    if (setCnf->rslt == VOS_OK) {
        return AT_OK;
    } else {
        return AT_CME_UNKNOWN;
    }
}

VOS_UINT32 AT_RcvMmaBorderInfoQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    errno_t                   memResult;
    TAF_MMA_BorderInfoQryCnf *qryCnf = (TAF_MMA_BorderInfoQryCnf *)msg;
    VOS_UINT8                 tempVersion[NAS_VERSION_LEN + 1];

    /* ��version����ת��Ϊ�ַ��� */
    memResult = memcpy_s(tempVersion, sizeof(tempVersion), qryCnf->versionId, NAS_VERSION_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(tempVersion), NAS_VERSION_LEN);

    /* ���ַ��������� */
    tempVersion[NAS_VERSION_LEN] = '\0';

    /* ��ѯ����ɹ����ϱ�^BORDERINFO�汾�� */
    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %s", g_parseContext[indexNum].cmdElement->cmdName, tempVersion);

    return AT_OK;
}

VOS_UINT32 AT_RcvFratIgnitionQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    AT_MTA_Msg                *mtaMsg      = VOS_NULL_PTR;
    MTA_AT_FratIgnitionQryCnf *ignitionCnf = VOS_NULL_PTR;
    VOS_UINT32                 ret;

    /* ��ʼ����Ϣ���� */
    mtaMsg      = (AT_MTA_Msg *)msg;
    ignitionCnf = (MTA_AT_FratIgnitionQryCnf *)mtaMsg->content;
    ret         = AT_OK;

    g_atSendDataBuff.bufLen = 0;

    if (ignitionCnf->result != MTA_AT_RESULT_NO_ERROR) {
        ret = AT_ERROR;
    } else {
        g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %u", g_parseContext[indexNum].cmdElement->cmdName,
            ignitionCnf->fratIgnitionState);
    }

    return ret;
}

VOS_UINT32 AT_RcvMmaEmRssiRptQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_EmrssirptQryCnf *emRssiRptQryCnf = (TAF_MMA_EmrssirptQryCnf *)msg;
    AT_RreturnCodeUint32     result          = AT_FAILURE;
    TAF_UINT16               length          = 0;

    g_atSendDataBuff.bufLen = 0;

    if (emRssiRptQryCnf->errorCause != TAF_ERR_NO_ERROR) {
        result = AT_ERROR;
    } else {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            emRssiRptQryCnf->emRssiRptSwitch);

        result = AT_OK;
    }

    g_atSendDataBuff.bufLen = length;

    return result;
}

VOS_UINT32 AT_RcvMmaPsSceneQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_PsSceneQryCnf *psSceneQryCnf = (TAF_MMA_PsSceneQryCnf *)msg;
    VOS_UINT32             result = 0;

    if (psSceneQryCnf->result == TAF_ERR_NO_ERROR) {
        g_atSendDataBuff.bufLen =
            (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
                psSceneQryCnf->psSceneState);
        result = AT_OK;
    } else {
        result = AT_ERROR;
        g_atSendDataBuff.bufLen = 0;
    }

    return result;
}

VOS_UINT32 AT_RcvMmaCarOosGearsQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CarOosGearsQryCnf *oosGearsQryCnf = (TAF_MMA_CarOosGearsQryCnf *)msg;
    VOS_UINT32                 result = 0;
    VOS_UINT32                 atCarGears = 0;

    if (oosGearsQryCnf->result == TAF_ERR_NO_ERROR) {
        atCarGears = (oosGearsQryCnf->carOosGears == TAF_MMA_STRATEGY_SCENE_BUTT) ? 0 : oosGearsQryCnf->carOosGears;
        g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName, atCarGears);
        result = AT_OK;
    } else {
        result = AT_ERROR;
        g_atSendDataBuff.bufLen = 0;
    }

    return result;
}

VOS_UINT32 AT_RcvSmsAntiAttackCapQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_SmsAntiAttackCapQryCnf *qryCnf = (TAF_MMA_SmsAntiAttackCapQryCnf *)msg;

    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %d,%d", g_parseContext[indexNum].cmdElement->cmdName, qryCnf->result,
        qryCnf->cause);

    return AT_OK;
}

VOS_UINT32 AT_RcvTafMmaAutoAttachSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_AutoAttachSetCnf *autoAttachSetCnf = (TAF_MMA_AutoAttachSetCnf *)msg;

    if (autoAttachSetCnf->result == TAF_PARA_OK) {
        return AT_OK;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_RcvMmaAutoAttachQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_AutoAttachQryCnf *autoAttachCnf = (TAF_MMA_AutoAttachQryCnf *)msg;
    VOS_UINT32                result;

    g_atSendDataBuff.bufLen = 0;

    if (autoAttachCnf->errorCause == TAF_ERR_NO_ERROR) {
        result = AT_OK;

        g_atSendDataBuff.bufLen += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            autoAttachCnf->autoAttachFlag);
    } else {
        result = AT_ERROR;
    }

    return result;
}

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_RcvMmaImsSwitchSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_ImsSwitchSetCnf *cnfMsg = (TAF_MMA_ImsSwitchSetCnf *)msg;
    VOS_UINT32               result;
    VOS_BOOL                 flag;

    flag = VOS_FALSE;
    /* �жϵ�ǰ�������� */
    if (cnfMsg->bitOpImsSwitch == VOS_TRUE) {
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        if ((cnfMsg->imsSwitchType == TAF_MMA_IMS_SWITCH_TYPE_NR) &&
            (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_NR_IMS_SWITCH_SET)) {
            flag = VOS_TRUE;
        }
#endif
        if ((cnfMsg->imsSwitchType == TAF_MMA_IMS_SWITCH_TYPE_GUL) &&
            (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_GUL_IMS_SWITCH_SET)) {
            flag = VOS_TRUE;
        }
    } else if (cnfMsg->bitOpRcsSwitch == VOS_TRUE) {
        if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_RCSSWITCH_SET) {
            flag = VOS_TRUE;
        }
    } else {
    }

    if (flag == VOS_FALSE) {
        AT_WARN_LOG("AT_RcvMmaImsSwitchSetCnf: WARNING:option id or type error!");
        return AT_CMD_NO_NEED_FORMAT_RSLT;
    }

    if (cnfMsg->result == TAF_ERR_NO_ERROR) {
        result = AT_OK;
    } else {
        result = At_ChgTafErrorCode(indexNum, cnfMsg->result);
    }

    g_atSendDataBuff.bufLen = 0;
    return result;
}

LOCAL VOS_UINT32 AT_RcvMmaGulImsSwitchQryCnf(TAF_MMA_ImsSwitchQryCnf *switchQryCnf, VOS_UINT8 indexNum)
{
    VOS_UINT32               result;

    if (switchQryCnf->result == TAF_ERR_NO_ERROR) {
        g_atSendDataBuff.bufLen =
            (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress, "%s: %d,%d,%d", g_parseContext[indexNum].cmdElement->cmdName,
                switchQryCnf->imsSwitchRat.lteEnable, switchQryCnf->imsSwitchRat.utranEnable,
                switchQryCnf->imsSwitchRat.gsmEnable);
        result = AT_OK;
    } else {
        g_atSendDataBuff.bufLen = 0;
        result = At_ChgTafErrorCode(indexNum, switchQryCnf->result);
    }

    return result;
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
LOCAL VOS_UINT32 AT_RcvMmaNrImsSwitchQryCnf(TAF_MMA_ImsSwitchQryCnf *switchQryCnf, VOS_UINT8 indexNum)
{
    VOS_UINT32               result;

    if (switchQryCnf->result == TAF_ERR_NO_ERROR) {
        g_atSendDataBuff.bufLen =
            (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
                switchQryCnf->imsSwitchRat.nrEnable);
        result = AT_OK;
    } else {
        g_atSendDataBuff.bufLen = 0;
        result = At_ChgTafErrorCode(indexNum, switchQryCnf->result);
    }

    return result;
}
#endif

VOS_UINT32 AT_RcvMmaImsSwitchQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_ImsSwitchQryCnf *switchQryCnf = (TAF_MMA_ImsSwitchQryCnf *)msg;

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    if (switchQryCnf->imsSwitchRat.imsSwitchType == TAF_MMA_IMS_SWITCH_TYPE_NR) {
        return AT_RcvMmaNrImsSwitchQryCnf(switchQryCnf, indexNum);
    }
#endif

    return AT_RcvMmaGulImsSwitchQryCnf(switchQryCnf, indexNum);
}

VOS_UINT32 AT_RcvMmaRcsSwitchQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_RcsSwitchQryCnf *rcsSwitchQryCnf = (TAF_MMA_RcsSwitchQryCnf *)msg;

    g_atSendDataBuff.bufLen = 0;

    if (rcsSwitchQryCnf->result == TAF_ERR_NO_ERROR) {
        g_atSendDataBuff.bufLen =
            (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
                rcsSwitchQryCnf->rcsSwitch);

        return AT_OK;
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_UINT32 AT_RcvMmaPwrOnAndRegTimeQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_PowerOnAndRegTimeQryCnf *appwronregCnf = (TAF_MMA_PowerOnAndRegTimeQryCnf *)msg;
    VOS_UINT32                       costTime = appwronregCnf->costTime;
    VOS_UINT16                       length = 0;
    VOS_UINT32                       result = AT_OK;

    /*
     * MMA����ATʱ����slice�ϱ���(32 * 1024)��slice��1S
     * ���sliceΪ0����ʾû��ע��ɹ������sliceС��1S,AT��1S�ϱ�
     */

    /* ����ѯʱ���ϱ���APP */
    if (costTime == 0) {
        result = AT_ERROR;
    } else {
        costTime = costTime / (32 * 1024); /* MMA����ATʱ����slice�ϱ���(32 * 1024)��slice��1S */

        if (costTime == 0) {
            costTime = 1;
        }

        result = AT_OK;

        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, costTime);
    }

    g_atSendDataBuff.bufLen = length;

    return result;
}

VOS_UINT32 AT_RcvMmaCmmSetCmdRsp(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CmmSetCnf *mnMmTestCmdRspMsg = (TAF_MMA_CmmSetCnf *)msg;
    VOS_UINT32         result = AT_OK;
    VOS_UINT32         i;
    VOS_UINT16         length = 0;

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvMmaCmmSetCmdRsp : AT_BROADCAST_INDEX.");
        return AT_CMD_NO_NEED_FORMAT_RSLT;
    }

    /* ��ʽ��AT^CMM��ѯ����� */
    g_atSendDataBuff.bufLen = 0;
    if (mnMmTestCmdRspMsg->result != VOS_OK) {
        result = AT_ERROR;
    } else {
        result = AT_OK;

        if (mnMmTestCmdRspMsg->atCmdRslt.rsltNum > 0) {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%d", mnMmTestCmdRspMsg->atCmdRslt.rslt[0]);

            mnMmTestCmdRspMsg->atCmdRslt.rsltNum = AT_MIN(mnMmTestCmdRspMsg->atCmdRslt.rsltNum, AT_CMD_PADDING_LEN);
            for (i = 1; i < mnMmTestCmdRspMsg->atCmdRslt.rsltNum; i++) {
                length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                    (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", mnMmTestCmdRspMsg->atCmdRslt.rslt[i]);
            }
            g_atSendDataBuff.bufLen = length;
        }
    }

    AT_StopTimerCmdReady(indexNum);
    At_FormatResultData(indexNum, result);

    return AT_CMD_NO_NEED_FORMAT_RSLT;
}

VOS_UINT32 AT_RcvMmaUlFreqRptQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_UlfreqrptQryCnf *ulFreqInfoInd = (TAF_MMA_UlfreqrptQryCnf *)msg;
    VOS_UINT32               result;

    if (ulFreqInfoInd->errorCause == TAF_ERR_NO_ERROR) {
        result                  = AT_OK;
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "^ULFREQRPT: %d,%d,%d,%d", ulFreqInfoInd->mode, ulFreqInfoInd->rat,
            ulFreqInfoInd->ulFreq, ulFreqInfoInd->bandwidth);
    } else {
        result                  = AT_ERROR;
        g_atSendDataBuff.bufLen = 0;
    }

    return result;
}
#if (FEATURE_CSG == FEATURE_ON)
VOS_VOID AT_ReportCsgListSearchCnfResult(TAF_MMA_CsgListCnfPara *csgList, VOS_UINT16 *length)
{
    VOS_UINT16 lengthTemp;
    VOS_UINT8  homeNodeBLen;
    VOS_UINT8  csgTypeLen;
    VOS_UINT32 i;
    VOS_UINT32 j;

    lengthTemp = *length;

    for (i = 0; i < (VOS_UINT32)AT_MIN(csgList->plmnWithCsgIdNum, TAF_MMA_MAX_CSG_ID_LIST_NUM); i++) {
        /* ����һ���⣬������ǰҪ�Ӷ��� */
        if ((i != 0) || (csgList->currIndex != 0)) {
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",");
        }

        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, "(");

        /* ��ӡ������ */
        if ((csgList->csgIdListInfo[i].operatorNameLong[0] == '\0') ||
            (csgList->csgIdListInfo[i].operatorNameShort[0] == '\0')) {
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, "\"\"");
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",\"\"");
        } else {
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, "\"%s\"", csgList->csgIdListInfo[i].operatorNameLong);
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",\"%s\"", csgList->csgIdListInfo[i].operatorNameShort);
        }

        /* ��ӡ���ָ�ʽ����Ӫ������  */
        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",\"%X%X%X",
            (0x0f00 & csgList->csgIdListInfo[i].plmnId.mcc) >> AT_OCTET_MOVE_EIGHT_BITS,
            (AT_OCTET_HIGH_FOUR_BITS & csgList->csgIdListInfo[i].plmnId.mcc) >> AT_OCTET_MOVE_FOUR_BITS,
            (AT_OCTET_LOW_FOUR_BITS & csgList->csgIdListInfo[i].plmnId.mcc));

        if (((0x0f00 & csgList->csgIdListInfo[i].plmnId.mnc) >> AT_OCTET_MOVE_EIGHT_BITS) != AT_OCTET_LOW_FOUR_BITS) {
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, "%X",
                (0x0f00 & csgList->csgIdListInfo[i].plmnId.mnc) >> AT_OCTET_MOVE_EIGHT_BITS);
        }

        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, "%X%X\"",
            (AT_OCTET_HIGH_FOUR_BITS & csgList->csgIdListInfo[i].plmnId.mnc) >> AT_OCTET_MOVE_FOUR_BITS,
            (AT_OCTET_LOW_FOUR_BITS & csgList->csgIdListInfo[i].plmnId.mnc));

        /* ��ӡCSG ID */
        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",\"%X\"", csgList->csgIdListInfo[i].csgId);

        /*
         * ��ӡCSG ID TYPE, 1��CSG ID��Allowed CSG List��; 2��CSG ID��Operator CSG List�в��ڽ�ֹCSG ID�б���;
         * 3��CSG ID��Operator CSG List�в����ڽ�ֹCSG ID�б���; 4��CSG ID����Allowed CSG List��Operator CSG List��
         */
        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",%d,", csgList->csgIdListInfo[i].plmnWithCsgIdType);

        /* ��ӡhome NodeB Name, ucs2���룬��󳤶�48�ֽ� */
        homeNodeBLen = AT_MIN(csgList->csgIdListInfo[i].csgIdHomeNodeBName.homeNodeBNameLen,
                              TAF_MMA_MAX_HOME_NODEB_NAME_LEN);

        for (j = 0; j < homeNodeBLen; j++) {
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + lengthTemp, "%02X",
                csgList->csgIdListInfo[i].csgIdHomeNodeBName.homeNodeBName[j]);
        }

        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + lengthTemp, ",");

        /* ��ӡCSG���ͣ���UCS-2 ��ʽ����, ��󳤶�12�ֽ� */
        csgTypeLen = AT_MIN(csgList->csgIdListInfo[i].csgType.csgTypeLen, TAF_MMA_MAX_CSG_TYPE_LEN);

        for (j = 0; j < csgTypeLen; j++) {
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + lengthTemp, "%02X", csgList->csgIdListInfo[i].csgType.csgType[j]);
        }

        if (csgList->csgIdListInfo[i].raMode == TAF_PH_RA_GSM) { /* GSM */
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",0");
        } else if (csgList->csgIdListInfo[i].raMode == TAF_PH_RA_WCDMA) { /* W */
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",2");
        } else if (csgList->csgIdListInfo[i].raMode == TAF_PH_RA_LTE) { /* LTE */
            lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ",7");
        } else {
        }

        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + lengthTemp, ",%d", csgList->csgIdListInfo[i].signalValue1);
        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + lengthTemp, ",%d", csgList->csgIdListInfo[i].signalValue2);
        lengthTemp += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + lengthTemp, ")");
    }

    *length = lengthTemp;
}

VOS_UINT32 AT_RcvMmaCsgListSearchCnfProc(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CsgListSearchCnf *csgListCnf = VOS_NULL_PTR;
    TAF_MMA_CsgListCnfPara   *csgList    = VOS_NULL_PTR;
    VOS_UINT16                length = 0;
    TAF_MMA_PlmnListPara      csgListPara;

    csgListCnf = (TAF_MMA_CsgListSearchCnf *)msg;
    csgList    = &csgListCnf->csgListCnfPara;
    memset_s(&csgListPara, sizeof(csgListPara), 0x00, sizeof(csgListPara));

    /* �����ʧ���¼�,ֱ���ϱ�ERROR */
    if (csgList->opError == VOS_TRUE) {
        g_atSendDataBuff.bufLen = 0;
        return (AT_RreturnCodeUint32)At_ChgTafErrorCode(indexNum, csgList->phoneError);
    }

    /* �״β�ѯ�ϱ����ʱ��Ҫ��ӡ^CSGIDSRCH: */
    if (csgList->currIndex == 0) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)(g_atSndCodeAddress + length), "%s", g_atCrLf);

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    }

    AT_ReportCsgListSearchCnfResult(csgList, &length);

    At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);
    length = 0;

    /* ��������ϱ���plmn��Ŀ��Ҫ�����ͬ������ΪC������Plmn listû���ϱ���Ҫ��������������в�ѯ */
    if (csgList->plmnWithCsgIdNum == TAF_MMA_MAX_CSG_ID_LIST_NUM) {
        csgListPara.qryNum    = TAF_MMA_MAX_CSG_ID_LIST_NUM;
        csgListPara.currIndex = (VOS_UINT16)(csgList->currIndex + csgList->plmnWithCsgIdNum);

        if (TAF_MMA_CsgListSearchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &csgListPara) == VOS_TRUE) {
            /* ���õ�ǰ�������� */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSG_LIST_SEARCH;
            return AT_CMD_NO_NEED_FORMAT_RSLT;
        }

        g_atSendDataBuff.bufLen = 0;
        return AT_ERROR;
    }

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "\r\n");

    At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);
    length = 0;

    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}

VOS_UINT32 AT_RcvMmaQryCampCsgIdInfoCnfProc(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_QryCampCsgIdInfoCnf *qryCnfMsg = (TAF_MMA_QryCampCsgIdInfoCnf *)msg;
    VOS_UINT16                   length    = 0;

    /*
     * <CR><LF>^CSGIDSRCH: [<oper>[,<CSG ID>][,<rat>]]<CR><LF>
     * <CR>OK<LF>
     */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    /* ���PLMN ID�Ƿ�����뼼���Ƿ���CSG ID��Ч���������ֻ��ʾOK */
    if ((qryCnfMsg->plmnId.mcc == TAF_MMA_INVALID_MCC) || (qryCnfMsg->plmnId.mnc == TAF_MMA_INVALID_MNC) ||
        (qryCnfMsg->ratType >= TAF_MMA_RAT_BUTT)) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "\"\",\"\",");
        g_atSendDataBuff.bufLen = length;
        return AT_OK;
    }

    /* BCD���MCC��MNC�����ʱ��Ҫת�����ַ��� */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "\"%X%X%X", (0x0f00 & qryCnfMsg->plmnId.mcc) >> 8,
        (0x00f0 & qryCnfMsg->plmnId.mcc) >> 4, (0x000f & qryCnfMsg->plmnId.mcc));

    if (((0x0f00 & qryCnfMsg->plmnId.mnc) >> 8) != 0x0F) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%X", (0x0f00 & qryCnfMsg->plmnId.mnc) >> 8);
    }

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%X%X\"", (0x00f0 & qryCnfMsg->plmnId.mnc) >> 4,
        (0x000f & qryCnfMsg->plmnId.mnc));

    /* ���CSG ID */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",\"%X\"", qryCnfMsg->csgId);

    /* <rat> */
    if (qryCnfMsg->ratType == TAF_MMA_RAT_LTE) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",7");
    }
    if (qryCnfMsg->ratType == TAF_PH_RA_GSM) { /* GSM */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",0");
    } else if (qryCnfMsg->ratType == TAF_PH_RA_WCDMA) { /* W */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",2");
    } else {
    }

    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}

#endif

#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
 * ����˵��: ��ʽ�����EONS���
 * �������: index: ATͨ��
 *           eonsResp: EONS���
 * ���ؽ��: ����ַ�������
 */
LOCAL VOS_UINT16 AT_PrintEonsInfo(VOS_UINT8 indexNum, TafMmaEonsResponsePara *eonsResp)
{
    VOS_UINT16 length = 0;

    /* ��ӡ���EONS��Type */
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        eonsResp->eonsType);

    if ((eonsResp->serviceStatus == TAF_REPORT_SRVSTA_NORMAL_SERVICE) || (eonsResp->plmnLen != 0)) {
        /* ����MNC�ĳ��ȣ���ӡ���PLMN */
        eonsResp->plmn.mcc &= 0x0FFF;

        if (eonsResp->plmnLen == TAF_PLMN_ID_LEN_6) {
            eonsResp->plmn.mnc &= 0x0FFF;

            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, ",%03X%03X", eonsResp->plmn.mcc, eonsResp->plmn.mnc);
        } else {
            eonsResp->plmn.mnc &= 0x00FF;

            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, ",%03X%02X", eonsResp->plmn.mcc, eonsResp->plmn.mnc);
        }

        /* ������� */
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",\"");

        length += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
            (TAF_UINT8 *)g_atSndCodeAddress + length, eonsResp->nameInfo.longName.name,
            eonsResp->nameInfo.longName.nameLen);

        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "\"");

        /* ������� */
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",\"");

        length += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
            (TAF_UINT8 *)g_atSndCodeAddress + length, eonsResp->nameInfo.shortName.name,
            eonsResp->nameInfo.shortName.nameLen);

        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "\"");

        /* ���SPN��Ч�Ļ����SPN������ */
        if (eonsResp->spnInfo.spnLen > 0) {
            /* ��SPN��ʾ������־λ��װ���ַ��� */
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", eonsResp->spnInfo.dispMode);

            /* ��SPN��Ϣ��װ���ַ��� */
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, ",\"");

            length += (TAF_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN, (TAF_INT8 *)g_atSndCodeAddress,
                (TAF_UINT8 *)g_atSndCodeAddress + length, eonsResp->spnInfo.spn, eonsResp->spnInfo.spnLen);

            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "\"");
        }
    }

    return length;
}
/*
 * ����˵��: ASCIIת����Unicode
 * �������: src: Դ�ַ���
 *           srcLen: Դ�ַ�������
 * �������: dst: Ŀ���ַ���
 *           dstLen: Ŀ���ַ�������
 */
LOCAL VOS_VOID AT_Ascii2Unicode(const VOS_UINT8 *src, VOS_UINT16 srcLen,
    VOS_UINT8 *dst, VOS_UINT16 *dstLen)
{
    VOS_UINT16 srcOffSet = 0;
    VOS_UINT16 dstOffSet = 0;

    while ((*src != '\0') && (srcOffSet < srcLen)) {
        *(dst + dstOffSet) = 0x00;
        dstOffSet++;
        *(dst + dstOffSet) = (*(src + srcOffSet));

        dstOffSet++;
        srcOffSet++;
    }

    *dstLen = dstOffSet;
}

LOCAL VOS_VOID AT_GetConvertLen(VOS_UINT16 *srcLenth, VOS_UINT16 maxLen)
{
    if (((*srcLenth) * AT_EONS_ASCII_TO_UNICODE_UINT) > maxLen) {
        *srcLenth = maxLen / AT_EONS_ASCII_TO_UNICODE_UINT;
    }
}

/*
 * ����˵��: ת��EONS���Ƶ�Unicode
 * �������: src: Դ�ַ���
 *           srcLen: Դ�ַ�������
 *           code: ���뷽ʽ
 *           srcMaxLen: ת��������̶�
 * �������: src: Ŀ���ַ���
 * ���ؽ��: Ŀ���ַ�������
 */
LOCAL VOS_UINT16 AT_ConvertEonsNameToUnicode(VOS_UINT8 *src, VOS_UINT16 srcLen,
    VOS_UINT8 code, VOS_UINT8 srcMaxLen)
{
    errno_t memResult;
    VOS_UINT16 tmpLen = 0;
    VOS_UINT16 tmpSrcLen = srcLen;
    VOS_UINT8 tmpSrc[TAF_PH_OPER_PNN_USC2_CODE_LEN + 1] = {0};

    /* �Ѳ�ͬ�ı���ת��Ϊunicode���� */
    switch (code) {
        case TAF_EONS_GSM_7BIT_DEFAULT: {
            /* ת����ܳ�����󳤶� */
            AT_GetConvertLen(&tmpSrcLen, srcMaxLen);
            At_PbGsmToUnicode(src, tmpSrcLen, &tmpSrc[0], sizeof(tmpSrc), &tmpLen);
            break;
        }
        case TAF_EONS_UCS2_80: {
            At_PbUnicode80FormatPrint(src, srcLen, &tmpSrc[0], sizeof(tmpSrc), &tmpLen);
            break;
        }
        case TAF_EONS_UCS2_81: {
            At_PbUnicode81FormatPrint(src, srcLen, &tmpSrc[0], sizeof(tmpSrc), &tmpLen);
            break;
        }
        case TAF_EONS_UCS2_82: {
            At_PbUnicode82FormatPrint(src, srcLen, &tmpSrc[0], sizeof(tmpSrc), &tmpLen);
            break;
        }
        case TAF_EONS_ASCII: {
            /* ת����ܳ�����󳤶� */
            AT_GetConvertLen(&tmpSrcLen, srcMaxLen);
            AT_Ascii2Unicode(src, tmpSrcLen, &tmpSrc[0], &tmpLen);
            break;
        }
        default: {
            break;
        }
    }

    /* ת�������ݱ��� */
    if ((tmpLen > 0) && (tmpLen <= srcMaxLen)) {
        (VOS_VOID)memset_s(src, srcMaxLen, 0, srcMaxLen);
        memResult = memcpy_s(src, srcMaxLen, tmpSrc, tmpLen);
        TAF_MEM_CHK_RTN_VAL(memResult, srcMaxLen, tmpLen);
        tmpSrcLen = tmpLen;
    }

    return tmpSrcLen;
}

/*
 * ����˵��: ��ʽ��EONS���
 * �������: eonsResp: EONS��ѯ���
 */
LOCAL VOS_VOID AT_EonsInfoFormat(TafMmaEonsResponsePara *eonsResp)
{
    VOS_UINT8 strLen;

    /* ת������Ϊuincode */
    strLen = (VOS_UINT8)AT_ConvertEonsNameToUnicode(eonsResp->nameInfo.longName.name,
        eonsResp->nameInfo.longName.nameLen, eonsResp->nameInfo.longName.nameCode, TAF_PH_OPER_PNN_USC2_CODE_LEN);
    eonsResp->nameInfo.longName.nameLen = strLen;

    /* ת������Ϊuincode */
    strLen = (VOS_UINT8)AT_ConvertEonsNameToUnicode(eonsResp->nameInfo.shortName.name,
        eonsResp->nameInfo.shortName.nameLen, eonsResp->nameInfo.shortName.nameCode, TAF_PH_OPER_PNN_USC2_CODE_LEN);
    eonsResp->nameInfo.shortName.nameLen = strLen;

    /* ת��SPNΪuincode */
    strLen = (VOS_UINT8)AT_ConvertEonsNameToUnicode(eonsResp->spnInfo.spn, eonsResp->spnInfo.spnLen,
        eonsResp->spnInfo.spnCode, TAF_PH_SPN_NAME_MAXLEN);
    eonsResp->spnInfo.spnLen = strLen;
}

/*
 * ����˵��: ��EONS�����ʹ�C�˻�ȡ������Ӫ�����ƽ��и�ʽ�����
 * �������: msg: EONS��ѯ���
 * ���ؽ��: VOS_ERR: ִ��ʧ��
 *           VOS_OK: ִ�гɹ�
 */
VOS_UINT32 AT_RcvMmaEonsInfoQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TafMmaEonsResponse *eonsQryResponse = (TafMmaEonsResponse *)msg;

    /* �Ի�ȡ�������ݽ��и�ʽ�� */
    AT_EonsInfoFormat(&(eonsQryResponse->eonsResponsePara));

    /* ����EONS��Ϣ */
    g_atSendDataBuff.bufLen = AT_PrintEonsInfo(indexNum, &(eonsQryResponse->eonsResponsePara));

    return AT_OK;
}

/*
 * ����˵��: ��ʽ��GSM��HCSQ���
 * �������: curLength: ��ӡ�ַ������еĳ���
 *           signalInfo: �ź�����
 * ���ؽ��: ����HCSQ��ӡ����ַ�������
 */
LOCAL VOS_VOID AT_Format2GHcsqResult(VOS_UINT16 *length, const TAF_PH_Rssi *signalInfo)
{
    VOS_UINT8 convertRssi = 0;

    /* ��ʵ�ʵ�RSSIֵת����HCSQ���ϱ�ֵ */
    AT_ConvertRssiLevel(signalInfo->aRssi[0].u.gCellSignInfo.rssiValue, &convertRssi);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, "^HCSQ: \"GSM\",%d", convertRssi);
}

/*
 * ����˵��: ת��ECIOֵ��HCSQ���ϱ�ֵ
 * �������: realEcio: ʵ�ʵ�ECIOֵ
 * �������: convertEcio: ת�����ECIOֵ
 */
LOCAL VOS_VOID AT_EcioConvert(VOS_INT16 realEcio, VOS_UINT8 *convertEcio)
{
    if (realEcio >= AT_HCSQ_VALUE_INVALID) {
        /* ��Чֵ */
        *convertEcio = AT_HCSQ_VALUE_INVALID;
    } else if (realEcio >= AT_HCSQ_ECIO_VALUE_MAX) {
        /* ʵ��ECIO���ڵ���0db */
        *convertEcio = AT_HCSQ_ECIO_LEVEL_MAX;
    } else if (realEcio < AT_HCSQ_ECIO_VALUE_MIN) {
        /* ʵ��ECIOС��-32db */
        *convertEcio = AT_HCSQ_LEVEL_MIN;
    } else {
        /* ʵ��ECIO��0db��-32db֮�� */
        *convertEcio = (VOS_UINT8)(((realEcio - AT_HCSQ_ECIO_VALUE_MIN) * AT_CMD_RESSI_ECIO_MULTI_TWO) + 1);
    }
}

/*
 * ����˵��: ��ʽ��WCDMA/TDSCDMA��HCSQ���
 * �������: curLength:  ��ӡ�ַ������еĳ���
 *           signalInfo: �ź�����
 * ���ؽ��: ����HCSQ��ӡ����ַ�������
 */
LOCAL VOS_VOID AT_Format3GHcsqResult(VOS_UINT16 *length, const TAF_PH_Rssi *signalInfo)
{
    VOS_INT16 realRssi;
    VOS_INT16 realRscp;
    VOS_INT16 realEcio;
    VOS_UINT8 convertRssi = 0;
    VOS_UINT8 convertRscp = 0;
    VOS_UINT8 convertEcio = 0;

    /* ��ʵ�ʵ�RSCPֵת����HCSQ���ϱ�ֵ */
    convertRscp = (VOS_INT8)AT_ConvertCerssiRscpToCesqRscp(signalInfo->aRssi[0].u.wCellSignInfo.rscpValue);

    /* ��ʵ�ʵ�ECIOֵת����HCSQ���ϱ�ֵ */
    AT_EcioConvert(signalInfo->aRssi[0].u.wCellSignInfo.ecioValue, &convertEcio);

    /* RSCP��ECIOֵ���˴�ֱ��͸���������쳣��������ײ㱣֤��ֻ����RSCP��ECIO��Ϊ��Чֵʱ����� */
    realRscp = signalInfo->aRssi[0].u.wCellSignInfo.rscpValue;
    realEcio = signalInfo->aRssi[0].u.wCellSignInfo.ecioValue;
    if ((realRscp >= AT_HCSQ_VALUE_INVALID) && (realEcio >= AT_HCSQ_VALUE_INVALID)) {
        realRssi = AT_HCSQ_VALUE_INVALID;
    } else {
        /* RSSI = RSCP - ECIO */
        realRssi = realRscp - realEcio;
    }
    /* ��ʵ�ʵ�RSSIֵת����HCSQ���ϱ�ֵ */
    AT_ConvertRssiLevel(realRssi, &convertRssi);
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, "^HCSQ: \"WCDMA\",%d,%d,%d", convertRssi, convertRscp, convertEcio);
}

/*
 * ����˵��: ת��SINRֵ��HCSQ���ϱ�ֵ
 * �������: realSinr: ʵ�ʵ�SINRֵ
 * �������: convertSinr: ת�����SINRֵ
 */
LOCAL VOS_VOID AT_SinrConvert(VOS_INT32 realSinr, VOS_UINT8 *convertSinr)
{
    if ((realSinr >= AT_HCSQ_VALUE_INVALID) ||
        (realSinr == AT_HCSQ_INVALID_SIG_VALUE)) {
        /* ��Чֵ */
        *convertSinr = AT_HCSQ_VALUE_INVALID;
    } else if (realSinr >= AT_HCSQ_SINR_VALUE_MAX) {
        /* ʵ��SINR����30db */
        *convertSinr = AT_HCSQ_SINR_LEVEL_MAX;
    } else if (realSinr < AT_HCSQ_SINR_VALUE_MIN) {
        /* ʵ��SINRС��-20db */
        *convertSinr = AT_HCSQ_LEVEL_MIN;
    } else {
        /* ʵ��SINR��-20db��30db֮�� */
        *convertSinr = (VOS_UINT8)((realSinr - AT_HCSQ_SINR_VALUE_MIN) * AT_CMD_RESSI_SINR_MULTI_FIVE + 1);
    }
}
/*
 * ����˵��: ��ʽ��LTE��HCSQ���
 * �������: curLength: ��ӡ�ַ������еĳ���
 *           signalInfo: �ź�����
 * ���ؽ��: ����HCSQ��ӡ����ַ�������
 */
LOCAL VOS_VOID AT_Format4GHcsqResult(VOS_UINT16 *length, const TAF_PH_Rssi *signalInfo, VOS_UINT32 rpt)
{
    VOS_UINT8 convertRssi = 0;
    VOS_UINT8 convertRsrp = 0;
    VOS_UINT8 convertRsrq = 0;
    VOS_UINT8 convertSinr = 0;

    /* ��ʵ�ʵ�RSSIֵת����HCSQ���ϱ�ֵ */
    AT_ConvertRssiLevel(signalInfo->aRssi[0].u.lCellSignInfo.rssi, &convertRssi);
    /* ��ʵ�ʵ�SINRֵת����HCSQ���ϱ�ֵ */
    AT_SinrConvert(signalInfo->aRssi[0].u.lCellSignInfo.sinr, &convertSinr);

    if (rpt == VOS_FALSE) {
        convertRsrp = (VOS_INT8)AT_ConvertCerssiRsrpToCesqRsrp(signalInfo->aRssi[0].u.lCellSignInfo.rsrp);
        convertRsrq = (VOS_INT8)AT_ConvertCerssiRsrqToCesqRsrq(signalInfo->aRssi[0].u.lCellSignInfo.rsrq);
    } else {
        /* ��ʵ�ʵ�RSRPֵת����HCSQ���ϱ�ֵ */
        convertRsrp = (VOS_INT8)AT_ConvertCerssiRsrp(signalInfo->aRssi[0].u.lCellSignInfo.rsrp);
        /* ��ʵ�ʵ�RSRPֵת����HCSQ���ϱ�ֵ */
        convertRsrq = (VOS_INT8)AT_ConvertCerssiRsrq(signalInfo->aRssi[0].u.lCellSignInfo.rsrq);
    }
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, "^HCSQ: \"LTE\",%d,%d,%d,%d", convertRssi, convertRsrp,
        convertSinr, convertRsrq);
}

/*
 * ����˵��: ��ʽ��NR��HCSQ���
 * �������: curLength: ��ӡ�ַ������еĳ���
 *           signalInfo: �ź�����
 * ���ؽ��: ����HCSQ��ӡ����ַ�������
 */
LOCAL VOS_VOID AT_Format5GHcsqResult(VOS_UINT16 *length, const TAF_PH_Rssi *signalInfo)
{
    VOS_UINT8 convertRsrp = 0;
    VOS_UINT8 convertRsrq = 0;
    VOS_UINT8 convertSinr = 0;

    /* ��ʵ�ʵ�RSRPֵת����HCSQ���ϱ�ֵ */
    convertRsrp = (VOS_UINT8)AT_ConvertCerssiRsrp(signalInfo->aRssi[0].u.nrCellSignInfo.s5GRsrp);
    /* ��ʵ�ʵ�RSRQֵת����HCSQ���ϱ�ֵ */
    convertRsrq = (VOS_UINT8)AT_ConvertCerssiRsrq(signalInfo->aRssi[0].u.nrCellSignInfo.s5GRsrq);
    /* ��ʵ�ʵ�SINRֵת����HCSQ���ϱ�ֵ */
    AT_SinrConvert(signalInfo->aRssi[0].u.nrCellSignInfo.l5GSinr, &convertSinr);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, "^HCSQ: \"NR\",%d,%d,%d", convertRsrp, convertSinr, convertRsrq);
}

/*
 * ����˵��: ��ʽ������ʽ��HCSQ���
 * �������: curLength: ��ӡ�ַ������еĳ���
 *           signalInfo: �ź�����
 *           rpt: �Ƿ��������ϱ����
 * ���ؽ��: ����HCSQ��ӡ����ַ�������
 */
VOS_VOID AT_FormatHcsqResult(VOS_UINT16 *length, TAF_PH_Rssi *signalInfo, VOS_UINT32 rpt)
{
    switch (signalInfo->ratType) {
        case TAF_MMA_RAT_GSM: {
            AT_Format2GHcsqResult(length, signalInfo);
            break;
        }
        case TAF_MMA_RAT_WCDMA: {
            AT_Format3GHcsqResult(length, signalInfo);
            break;
        }
        case TAF_MMA_RAT_LTE: {
            AT_Format4GHcsqResult(length, signalInfo, rpt);
            break;
        }
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        case TAF_MMA_RAT_NR: {
            AT_Format5GHcsqResult(length, signalInfo);
            break;
        }
#endif
        default: {
            *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + *length, "^HCSQ: \"NOSERVICE\"");
            break;
        }
    }
}

/*
 * ����˵��: ��ʽ��HCSQ��ѯ���
 * �������: hcsqResponsePara: �������
 * ���ؽ��: ����ַ�������
 */
VOS_UINT16 AT_FormatHcsqQryResult(TafMmaHcsqQryResponsePara *hcsqResponsePara)
{
    VOS_UINT16 length = 0;

    /* ��ӡMN�Ľ�� */
    AT_FormatHcsqResult(&length, &(hcsqResponsePara->mnSignal), VOS_FALSE);

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /* ��ӡSN�Ľ�� */
    if (hcsqResponsePara->mrdcFlag == VOS_TRUE) {
        /* ����һ������ */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

        AT_FormatHcsqResult(&length, &(hcsqResponsePara->snSignal), VOS_FALSE);
    }
#endif

    return length;
}

/*
 * ����˵��: A���յ�HCSQ��ѯ����Ĵ�����
 * �������: msg: �����Ϣ
 * ���ؽ��: VOS_ERR: ִ��ʧ��
 *           VOS_OK: ִ�гɹ�
 */
VOS_UINT32 AT_RcvMmaHcsqInfoQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TafMmaHcsqQryResponse *hcsqResponse = (TafMmaHcsqQryResponse *)msg;
    /* ��ʽ�������� */
    g_atSendDataBuff.bufLen = AT_FormatHcsqQryResult(&hcsqResponse->hcsqResponsePara);
    return AT_OK;
}

/*
 * ����˵��: ��������ظ�ʽ��AT����ֵ���и�ʽ��
 * �������: netSelOptInfo: C�˷��ص��ϱ�����
 *           index: AT����ִ��ͨ��index
 * �������: length: ��ǰ�������ַ����е��ַ�����
 * ���ؽ��: AT_OK: AT����OK
 *           AT_ERROR: AT����ERROR
 */
LOCAL VOS_UINT32 AT_FormatNetSelOptResult(const TafMmaNetSelOptSetCnf *netSelOptInfo, VOS_UINT32 indexNum,
    VOS_UINT32 *length)
{
    VOS_UINT32 result = AT_ERROR;
    VOS_UINT32 len = 0;

    /* ��ǰ��֧��SCENEΪ1�ĳ�����������ѡ������ */
    if (netSelOptInfo->scene == SCENE_TYPE_INTELLIGENT_NET_SEL) {
        if (netSelOptInfo->operate == INTELLIGENT_NET_SEL_OPERATE_READ) {
            /* ��ȡ��ѡƵ�εĴ�����Ҫ�ϱ���Ӧ��Ϣ */
            len += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress, "%s: %d,%d,%d",
                g_parseContext[indexNum].cmdElement->cmdName, netSelOptInfo->scene,
                netSelOptInfo->netInfo.intelligentNetSel.rat, netSelOptInfo->netInfo.intelligentNetSel.band);
        }
        result = AT_OK;
    }

    /* ���÷����ַ������ȣ�������ATִ�н�� */
    *length = len;
    return result;
}
/*
 * ����˵��: �յ�AT^NETSELOPT����������
 * �������: msg: �յ�������������
 * ���ؽ��: VOS_ERR: ִ��ʧ��
 *           VOS_OK: ִ�гɹ�
 */
VOS_UINT32 AT_RcvMmaNetSelOptSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TafMmaNetSelOptSetCnf *netSelOptSetCnf = (TafMmaNetSelOptSetCnf *)msg;
    VOS_UINT32 result = AT_ERROR;
    VOS_UINT32 length = 0;

    if (netSelOptSetCnf->errorCause == TAF_ERR_NO_ERROR) {
        result = AT_FormatNetSelOptResult(netSelOptSetCnf, indexNum, &length);
    }
    /* ����AT״̬��ִ�н���ϱ� */
    g_atSendDataBuff.bufLen = (VOS_UINT16)length;

    return result;
}

VOS_VOID AT_QryParaRspCellRoamProc(TAF_UINT8 indexNum, TAF_UINT8 opId, TAF_UINT8 *para)
{
    TAF_PH_Cellroam *cellRoam = (TAF_PH_Cellroam *)para;
    TAF_UINT16 length = 0;

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d,%d", g_parseContext[indexNum].cmdElement->cmdName,
        cellRoam->roamMode, cellRoam->ratMode);

    g_atSendDataBuff.bufLen = length;
    At_FormatResultData(indexNum, AT_OK);
}
#endif

