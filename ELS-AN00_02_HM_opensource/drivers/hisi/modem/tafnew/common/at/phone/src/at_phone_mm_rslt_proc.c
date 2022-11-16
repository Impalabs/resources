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
#include "at_phone_mm_rslt_proc.h"
#include "securec.h"
#include "at_cmd_proc.h"
#include "at_phone_comm.h"
#include "at_lte_common.h"
#include "at_external_module_msg_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_MM_RSLT_PROC_C

#define AT_SYS_MODE_NAME_ARRAY_LEN 255
#define AT_SUB_SYS_MODE_NAME_ARRAY_LEN 255
#define AT_PLMN_TYPE_NUM 2

/* begin V7R1 PhaseI Modify */
static const AT_PH_SysModeTbl g_sysModeTbl[] = {
    { MN_PH_SYS_MODE_EX_NONE_RAT, "NO SERVICE" },
    { MN_PH_SYS_MODE_EX_GSM_RAT, "GSM" },
    { MN_PH_SYS_MODE_EX_CDMA_RAT, "CDMA" },
    { MN_PH_SYS_MODE_EX_WCDMA_RAT, "WCDMA" },
    { MN_PH_SYS_MODE_EX_TDCDMA_RAT, "TD-SCDMA" },
    { MN_PH_SYS_MODE_EX_WIMAX_RAT, "WIMAX" },
    { MN_PH_SYS_MODE_EX_LTE_RAT, "LTE" },
    { MN_PH_SYS_MODE_EX_EVDO_RAT, "EVDO" },
    { MN_PH_SYS_MODE_EX_HYBRID_RAT, "CDMA1X+EVDO(HYBRID)" },
    { MN_PH_SYS_MODE_EX_SVLTE_RAT, "CDMA1X+LTE" },
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    { MN_PH_SYS_MODE_EX_EUTRAN_5GC_RAT, "EUTRAN-5GC" },
    { MN_PH_SYS_MODE_EX_NR_5GC_RAT, "NR-5GC" },
    { MN_PH_SYS_MODE_EX_1X_NR_5GC_RAT, "CDMA1X+NR-5GC" },
#endif
};

static const AT_PH_SubSysModeTbl g_subSysModeTbl[] = {
    { MN_PH_SUB_SYS_MODE_EX_NONE_RAT, "NO SERVICE" },
    { MN_PH_SUB_SYS_MODE_EX_GSM_RAT, "GSM" },
    { MN_PH_SUB_SYS_MODE_EX_GPRS_RAT, "GPRS" },
    { MN_PH_SUB_SYS_MODE_EX_EDGE_RAT, "EDGE" },
    { MN_PH_SUB_SYS_MODE_EX_WCDMA_RAT, "WCDMA" },
    { MN_PH_SUB_SYS_MODE_EX_HSDPA_RAT, "HSDPA" },
    { MN_PH_SUB_SYS_MODE_EX_HSUPA_RAT, "HSUPA" },
    { MN_PH_SUB_SYS_MODE_EX_HSPA_RAT, "HSPA" },
    { MN_PH_SUB_SYS_MODE_EX_HSPA_PLUS_RAT, "HSPA+" },
    { MN_PH_SUB_SYS_MODE_EX_DCHSPA_PLUS_RAT, "DC-HSPA+" },
    { MN_PH_SUB_SYS_MODE_EX_TDCDMA_RAT, "TD-SCDMA" },
    { MN_PH_SUB_SYS_MODE_EX_TD_HSDPA_RAT, "HSDPA" },
    { MN_PH_SUB_SYS_MODE_EX_TD_HSUPA_RAT, "HSUPA" },
    { MN_PH_SUB_SYS_MODE_EX_TD_HSPA_RAT, "HSPA" },
    { MN_PH_SUB_SYS_MODE_EX_TD_HSPA_PLUS_RAT, "HSPA+" },

    { MN_PH_SUB_SYS_MODE_EX_LTE_RAT, "LTE" },

    { MN_PH_SUB_SYS_MODE_EX_CDMA20001X_RAT, "CDMA2000 1X" },

    { MN_PH_SUB_SYS_MODE_EX_EVDOREL0_RAT, "EVDO Rel0" },
    { MN_PH_SUB_SYS_MODE_EX_EVDORELA_RAT, "EVDO RelA" },
    { MN_PH_SUB_SYS_MODE_EX_HYBIRD_EVDOREL0_RAT, "HYBRID(EVDO Rel0)" },
    { MN_PH_SUB_SYS_MODE_EX_HYBIRD_EVDORELA_RAT, "HYBRID(EVDO RelA)" },

    { MN_PH_SUB_SYS_MODE_EX_EHRPD_RAT, "EHRPD" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    { MN_PH_SUB_SYS_MODE_EX_EUTRAN_5GC_RAT, "EUTRAN-5GC" },
    { MN_PH_SUB_SYS_MODE_EX_NR_5GC_RAT, "NR-5GC" },
#endif
};
/* end V7R1 PhaseI Modify */

VOS_UINT32 AT_RcvMmaPhoneModeQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_PhoneModeQryCnf *phoneModeQryCnf = (TAF_MMA_PhoneModeQryCnf *)msg;
    VOS_UINT32               result;

    g_atSendDataBuff.bufLen = 0;

    if (phoneModeQryCnf->errorCause == TAF_ERR_NO_ERROR) {
        g_atSendDataBuff.bufLen =
            (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
                phoneModeQryCnf->phMode);
        result = AT_OK;
    } else {
        result = At_ChgTafErrorCode(indexNum, phoneModeQryCnf->errorCause);
    }

    return result;
}

VOS_UINT16 AT_GetOperNameLengthForCops(TAF_CHAR *pstr, TAF_UINT8 maxLen)
{
    VOS_UINT16 rsltLen = 0;
    TAF_UINT8  i;

    for (i = 0; i < maxLen; i++) {
        if (pstr[i] != '\0') {
            rsltLen = i + 1;
        }
    }

    return rsltLen;
}

LOCAL VOS_VOID AT_PrintCopsLongAlphTypeOper(TAF_PH_Networkname *cops, VOS_UINT16 *length)
{
    VOS_UINT16         nameLength = 0;
    errno_t            memResult;

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, ",\"");

    /* �����ʾSPN�д���0x00��Ч�ַ������,��ȡ��ʵ�ʳ���,At_sprintf��0x00��β�������� */
    nameLength = AT_GetOperNameLengthForCops(cops->name.operatorNameLong, TAF_PH_OPER_NAME_LONG);

    if (nameLength > 0) {
        memResult = memcpy_s(g_atSndCodeAddress + *length,
                             AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_INDEX_3 - *length,
                             cops->name.operatorNameLong, nameLength);
        TAF_MEM_CHK_RTN_VAL(memResult,
                            AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_INDEX_3 - *length,
                            nameLength);
    }

    *length = *length + nameLength;
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, "\"");
}

LOCAL VOS_VOID AT_PrintCopsShortAlphTypeOper(TAF_PH_Networkname *cops, VOS_UINT16 *length)
{
    VOS_UINT16         nameLength = 0;
    errno_t            memResult;

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, ",\"");

    /* �����ʾSPN�д���0x00��Ч�ַ������,��ȡ��ʵ�ʳ���,At_sprintf��0x00��β�������� */
    nameLength = AT_GetOperNameLengthForCops(cops->name.operatorNameShort, TAF_PH_OPER_NAME_SHORT);

    if (nameLength > 0) {
        memResult = memcpy_s(g_atSndCodeAddress + *length,
                             AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_INDEX_3 - *length,
                             cops->name.operatorNameShort, nameLength);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_INDEX_3 - *length,
                            nameLength);
    }

    *length = *length + nameLength;
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, "\"");
}

LOCAL VOS_VOID AT_PrintCopsDefaultTypeOper(TAF_PH_Networkname *cops, VOS_UINT16 *length)
{
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, ",\"%X%X%X", (AT_MCC_DIGIT3_MASK & cops->name.plmnId.mcc) >>
        AT_MCC_DIGIT3_OFFSET, (AT_MCC_DIGIT2_MASK & cops->name.plmnId.mcc) >> AT_MCC_DIGIT2_OFFSET,
        (AT_MCC_DIGIT1_MASK & cops->name.plmnId.mcc));

    if (((AT_MNC_DIGIT3_MASK & cops->name.plmnId.mnc) >> AT_MNC_DIGIT3_OFFSET) != AT_MNC_DIGIT1_MASK) {
        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + *length, "%X", (AT_MNC_DIGIT3_MASK & cops->name.plmnId.mnc) >>
            AT_MNC_DIGIT3_OFFSET);
    }

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + *length, "%X%X\"", (AT_MNC_DIGIT2_MASK & cops->name.plmnId.mnc) >>
        AT_MNC_DIGIT2_OFFSET, (AT_MNC_DIGIT1_MASK & cops->name.plmnId.mnc));
}

LOCAL VOS_VOID AT_PrintCopsRatMode(TAF_PH_Networkname *cops, VOS_UINT16 *length)
{
    /* <AcT> */
    if (cops->raMode == TAF_PH_RA_GSM) { /* GSM */
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, ",0");
    } else if (cops->raMode == TAF_PH_RA_WCDMA) { /* CDMA */
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, ",2");
    } else if (cops->raMode == TAF_PH_RA_LTE) { /* LTE */
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, ",7");
    }
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    else if (cops->raMode == TAF_PH_RA_LTE_ENDC) {/* LTE ENDC */
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, ",13");
    } else if (cops->raMode == TAF_PH_RA_NR) { /* NR */
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, ",12");
    }
#endif
    else {
    }
}

VOS_UINT32 AT_RcvMmaCopsQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CopsQryCnf *copsQryCnf = (TAF_MMA_CopsQryCnf *)msg;
    TAF_PH_Networkname *cops = &(copsQryCnf->copsInfo);
    VOS_UINT16         length     = 0;

    /*
     * A32D07158
     * +COPS: <mode>[,<format>,<oper>[,<AcT>]], get the PLMN selection mode from msg sent by MMA
     */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        cops->plmnSelMode);

    /* MBB����֧��COPS=2��ѯʱ����2���˳���ͬ��ֻ��Ҫ��ʾsel mode */
    if ((AT_PH_IsPlmnValid(&(cops->name.plmnId)) == VOS_FALSE)
    ) {
        /* ��Ч PLMNId ֻ��ʾ sel mode */
        g_atSendDataBuff.bufLen = length;
        return AT_OK;
    }

    /* <format> */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, ",%d", cops->nameFormat);

    /* <oper> */
    switch (cops->nameFormat) {
        /* �����֣��ַ������� */
        case AT_COPS_LONG_ALPH_TYPE:
            AT_PrintCopsLongAlphTypeOper(cops, &length);
            break;

        /* �����֣��ַ������� */
        case AT_COPS_SHORT_ALPH_TYPE:
            AT_PrintCopsShortAlphTypeOper(cops, &length);
            break;

        /* BCD���MCC��MNC����Ҫת�����ַ��� */
        default:
            AT_PrintCopsDefaultTypeOper(cops, &length);
            break;
    }

    AT_PrintCopsRatMode(cops, &length);
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_RcvTafMmaCsqQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CsqQryCnf *pstrCsqQryCnf = (TAF_MMA_CsqQryCnf *)msg;
    errno_t            memResult;
    VOS_UINT32         result;
    TAF_UINT16         length   = 0;
    TAF_PH_Rssi        csq;

    result        = AT_OK;

    memResult = memcpy_s(&csq, sizeof(csq), &pstrCsqQryCnf->csq, sizeof(TAF_PH_Rssi));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(csq), sizeof(TAF_PH_Rssi));
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    if (pstrCsqQryCnf->errorCause != TAF_ERR_NO_ERROR) {
        result = AT_ERROR;
    } else {
        result = AT_OK;
        if (csq.rssiNum > 0) {
            /* ���ӷ�Χ���� */
            if ((csq.aRssi[0].rssiValue >= TAF_PH_CSQ_RSSI_HIGH) && (csq.aRssi[0].rssiValue < TAF_PH_BER_UNKNOWN)) {
                length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                    (TAF_CHAR *)g_atSndCodeAddress + length, "%d", TAF_PH_CSQ_RSSI_HIGH);
            } else {
                length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                    (TAF_CHAR *)g_atSndCodeAddress + length, "%d", csq.aRssi[0].rssiValue);
            }
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", csq.aRssi[0].channalQual);
        }
    }

    g_atSendDataBuff.bufLen = length;
    return result;
}

VOS_UINT32 AT_RcvMmaDetachCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_DetachCnf *detachCnf = (TAF_MMA_DetachCnf *)msg;
    VOS_UINT32         result;

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvMmaDetachCnf : AT_BROADCAST_INDEX.");
        return AT_CMD_NO_NEED_FORMAT_RSLT;
    }

    g_atSendDataBuff.bufLen = 0;

    if (detachCnf->rslt == TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        result = AT_OK;
    } else {
        result = AT_ERROR;
    }

    AT_StopTimerCmdReady(indexNum);
    At_FormatResultData(indexNum, result);

    return AT_CMD_NO_NEED_FORMAT_RSLT;
}

VOS_UINT32 AT_RcvMmaAttachCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_AttachCnf *attachCnf = (TAF_MMA_AttachCnf *)msg;
    VOS_UINT32         result;

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvMmaAttachCnf : AT_BROADCAST_INDEX.");
        return AT_CMD_NO_NEED_FORMAT_RSLT;
    }

    g_atSendDataBuff.bufLen = 0;

    if (attachCnf->rslt == TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        result = AT_OK;
    } else {
        result = AT_ERROR;
    }

    AT_StopTimerCmdReady(indexNum);
    At_FormatResultData(indexNum, result);

    return AT_CMD_NO_NEED_FORMAT_RSLT;
}

VOS_UINT32 AT_RcvMmaAttachStatusQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_AttachStatusQryCnf *attachStatusQryCnf = (TAF_MMA_AttachStatusQryCnf *)msg;

    /* ����ѯ��� */
    if (attachStatusQryCnf->rslt != TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        return AT_ERROR;
    }

    /* ����Domain Type������ؽ�� */
    if (attachStatusQryCnf->domainType == TAF_MMA_SERVICE_DOMAIN_CS_PS) {
        g_atSendDataBuff.bufLen =
            (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress, "%s: %d,%d", g_parseContext[indexNum].cmdElement->cmdName,
                (VOS_UINT32)attachStatusQryCnf->psStatus, (VOS_UINT32)attachStatusQryCnf->csStatus);

        return AT_OK;
    } else if (attachStatusQryCnf->domainType == TAF_MMA_SERVICE_DOMAIN_PS) {
        g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            (VOS_UINT32)attachStatusQryCnf->psStatus);

        return AT_OK;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_QryParaRspSysinfoProc(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_SysinfoQryCnf *sysInfoCnf = (TAF_MMA_SysinfoQryCnf *)msg;
    TAF_PH_Sysinfo *sysInfo = &(sysInfoCnf->sysInfo);
    VOS_UINT8 *systemAppConfig = AT_GetSystemAppConfigAddr();
    VOS_UINT16 length = 0;

    if (sysInfoCnf->isSupport == VOS_FALSE) {
        return AT_CMD_NOT_SUPPORT;
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
        sysInfo->srvStatus);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->srvDomain);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->roamStatus);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->sysMode);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->simStatus);

    if (*systemAppConfig == SYSTEM_APP_WEBUI) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->simLockStatus);
    } else {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",");
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->sysSubMode);

    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}

VOS_UINT32 AT_GetSysModeName(MN_PH_SysModeExUint8 sysMode, VOS_CHAR *sysModeName, VOS_UINT32 maxMemLength)
{
    VOS_UINT32 i;
    errno_t    stringRet;

    for (i = 0; i < sizeof(g_sysModeTbl) / sizeof(AT_PH_SysModeTbl); i++) {
        if (g_sysModeTbl[i].sysMode == sysMode) {
            stringRet = strncpy_s(sysModeName, maxMemLength, g_sysModeTbl[i].strSysModeName,
                                  VOS_StrLen(g_sysModeTbl[i].strSysModeName));
            TAF_STRCPY_CHK_RTN_VAL_CONTINUE(stringRet, maxMemLength, VOS_StrLen(g_sysModeTbl[i].strSysModeName));
            return VOS_OK;
        }
    }

    return VOS_ERR;
}

VOS_UINT32 AT_GetSubSysModeName(MN_PH_SubSysModeExUint8 subSysMode, VOS_CHAR *subSysModeName, VOS_UINT32 maxMemLength)
{
    VOS_UINT32 i;
    errno_t    stringRet;

    for (i = 0; i < sizeof(g_subSysModeTbl) / sizeof(AT_PH_SubSysModeTbl); i++) {
        if (g_subSysModeTbl[i].subSysMode == subSysMode) {
            stringRet = strncpy_s(subSysModeName, maxMemLength, g_subSysModeTbl[i].strSubSysModeName,
                                  VOS_StrLen(g_subSysModeTbl[i].strSubSysModeName));
            TAF_STRCPY_CHK_RTN_VAL_CONTINUE(stringRet, maxMemLength, VOS_StrLen(g_subSysModeTbl[i].strSubSysModeName));

            return VOS_OK;
        }
    }

    return VOS_ERR;
}

VOS_UINT32 AT_QryParaRspSysinfoExProc(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_SysinfoQryCnf *sysInfoCnf = (TAF_MMA_SysinfoQryCnf *)msg;
    TAF_PH_Sysinfo *sysInfo = &(sysInfoCnf->sysInfo);
    VOS_UINT8 *systemAppConfig = AT_GetSystemAppConfigAddr();
    VOS_CHAR sysModeName[AT_SYS_MODE_NAME_ARRAY_LEN];
    VOS_CHAR subSysModeName[AT_SUB_SYS_MODE_NAME_ARRAY_LEN];
    VOS_UINT16 length = 0;

    if (sysInfoCnf->isSupport == VOS_FALSE) {
        return AT_CMD_NOT_SUPPORT;
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
        sysInfo->srvStatus);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->srvDomain);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->roamStatus);

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->simStatus);

    if (*systemAppConfig == SYSTEM_APP_WEBUI) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->simLockStatus);
    } else {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",");
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->sysMode);

    (VOS_VOID)memset_s(sysModeName, sizeof(sysModeName), 0x00, sizeof(sysModeName));
    (VOS_VOID)memset_s(subSysModeName, sizeof(subSysModeName), 0x00, sizeof(subSysModeName));

    /* ��ȡSysMode������ */
    AT_GetSysModeName(sysInfo->sysMode, sysModeName, (TAF_UINT32)sizeof(sysModeName));

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",\"%s\"", sysModeName);

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", sysInfo->sysSubMode);

    /* ��ȡSubSysMode������ */
    AT_GetSubSysModeName(sysInfo->sysSubMode, subSysModeName, (TAF_UINT32)sizeof(subSysModeName));

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",\"%s\"", subSysModeName);
    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}
/* end V7R1 PhaseI Modify */

VOS_UINT32 AT_RcvTafMmaSyscfgTestCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_SyscfgTestCnf *sysCfgTestCnf = (TAF_MMA_SyscfgTestCnf *)msg;
    AT_ModemNetCtx        *netCtx        = AT_GetModemNetCtxAddrFromClientId(indexNum);
    VOS_CHAR              *pcRoamPara    = VOS_NULL_PTR;
    VOS_CHAR              *pcRatPara     = VOS_NULL_PTR;

    if (netCtx->roamFeature == AT_ROAM_FEATURE_OFF) {
        pcRoamPara = "(0-2)";
    } else {
        pcRoamPara = "(0-3)";
    }

#if ((FEATURE_UE_MODE_CDMA == FEATURE_ON) && (FEATURE_UE_MODE_NR == FEATURE_ON))
    pcRatPara = "(\"01\",\"02\",\"03\",\"04\",\"07\",\"08\")";
#elif (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    pcRatPara = "(\"01\",\"02\",\"03\",\"04\",\"07\")";
#elif (FEATURE_UE_MODE_NR == FEATURE_ON)
    pcRatPara = "(\"01\",\"02\",\"03\",\"08\")";
#else
    pcRatPara = "(\"01\",\"02\",\"03\")";
#endif

    /* SYSCFG */
    sysCfgTestCnf->bandInfo.strSysCfgBandGroup[TAF_PH_SYSCFG_GROUP_BAND_LEN - 1] = '\0';
    if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_SYSCFG_TEST) {
        g_atSendDataBuff.bufLen =
            (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress, "%s: (2,13,14,16),(0-3),%s,%s,(0-4)",
                g_parseContext[indexNum].cmdElement->cmdName, sysCfgTestCnf->bandInfo.strSysCfgBandGroup, pcRoamPara);
    }
    /* SYSCFGEX */
    else {
#if (FEATURE_MBB_CUST == FEATURE_ON)
        AT_FormatSysCfgExTestResult(netCtx, sysCfgTestCnf, pcRatPara, indexNum);
#else
        g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %s,%s,%s,(0-4),((7fffffffffffffff,\"All bands\"))",
            g_parseContext[indexNum].cmdElement->cmdName, pcRatPara, sysCfgTestCnf->bandInfo.strSysCfgBandGroup,
            pcRoamPara);
#endif
    }

    return AT_OK;
}

VOS_CHAR* AT_ConvertRatModeForQryParaPlmnList(TAF_PH_RA_MODE raMode)
{
    VOS_CHAR *pcCopsRat = VOS_NULL_PTR;

    switch (raMode) {
        case TAF_PH_RA_GSM:
            pcCopsRat = "0";
            break;

        case TAF_PH_RA_WCDMA:
            pcCopsRat = "2";
            break;

        case TAF_PH_RA_LTE:
            pcCopsRat = "7";
            break;
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        case TAF_PH_RA_NR:
            pcCopsRat = "12";
            break;
#endif
        default:
            pcCopsRat = "";
    }

    return pcCopsRat;
}
/*
 * ��������: ��ӡ�б��ѵ�plmn�б�
 */
LOCAL VOS_VOID AT_PrintQryPlmnListPara(TAF_MMA_PlmnListCnfPara *plmnList, VOS_UINT16 *length)
{
    VOS_CHAR                *pcRatMode = VOS_NULL_PTR;
    VOS_UINT32               tmp;

    plmnList->plmnNum = AT_MIN(plmnList->plmnNum, TAF_MMA_MAX_PLMN_NAME_LIST_NUM);
    for (tmp = 0; tmp < plmnList->plmnNum; tmp++) {
        if ((tmp != 0) || (plmnList->currIndex != 0)) { /* ����һ���⣬������ǰҪ�Ӷ��� */
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, ",");
        }

        plmnList->plmnName[tmp].operatorNameLong[TAF_PH_OPER_NAME_LONG - 1]   = '\0';
        plmnList->plmnName[tmp].operatorNameShort[TAF_PH_OPER_NAME_SHORT - 1] = '\0';
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "(%d,\"%s\",\"%s\"", plmnList->plmnInfo[tmp].plmnStatus,
            plmnList->plmnName[tmp].operatorNameLong, plmnList->plmnName[tmp].operatorNameShort);

        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, ",\"%X%X%X", (0x0f00 & plmnList->plmnName[tmp].plmnId.mcc) >> 8,
            (0x00f0 & plmnList->plmnName[tmp].plmnId.mcc) >> 4, (0x000f & plmnList->plmnName[tmp].plmnId.mcc));

        if (((0x0f00 & plmnList->plmnName[tmp].plmnId.mnc) >> 8) != 0x0F) {
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "%X", (0x0f00 & plmnList->plmnName[tmp].plmnId.mnc) >> 8);
        }

        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%X%X\"", (0x00f0 & plmnList->plmnName[tmp].plmnId.mnc) >> 4,
            (0x000f & plmnList->plmnName[tmp].plmnId.mnc));

        pcRatMode = AT_ConvertRatModeForQryParaPlmnList(plmnList->plmnInfo[tmp].raMode);
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, ",%s)", pcRatMode);
    }
}

VOS_UINT32 At_QryParaPlmnListProc(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_PlmnListCnf     *plmnListCnf = (TAF_MMA_PlmnListCnf *)msg;
    TAF_MMA_PlmnListCnfPara *plmnList    = &plmnListCnf->plmnListCnfPara;
    TAF_MMA_PlmnListPara     plmnListPara;
    VOS_UINT16               length = 0;

    /* �����ʧ���¼�,ֱ���ϱ�ERROR */
    if (plmnList->opError == 1) {
        g_atSendDataBuff.bufLen = 0;
        return (AT_RreturnCodeUint32)At_ChgTafErrorCode(indexNum, plmnList->phoneError);
    }
    if (plmnList->currIndex == 0) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)(g_atSndCodeAddress + length), "%s", g_atCrLf);

        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);
    }

    AT_PrintQryPlmnListPara(plmnList, &length);

    At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);
    length = 0;

    /* ��������ϱ���plmn��Ŀ��Ҫ�����ͬ������ΪC������Plmn listû���ϱ���Ҫ��������������в�ѯ */
    if (plmnList->plmnNum == TAF_MMA_MAX_PLMN_NAME_LIST_NUM) {
        plmnListPara.qryNum    = TAF_MMA_MAX_PLMN_NAME_LIST_NUM;
        plmnListPara.currIndex = (VOS_UINT16)(plmnList->currIndex + plmnList->plmnNum);

        if (Taf_PhonePlmnList(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &plmnListPara) == VOS_TRUE) {
            /* ���õ�ǰ�������� */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_TEST;
            return AT_CMD_NO_NEED_FORMAT_RSLT;
        }

        g_atSendDataBuff.bufLen = 0;
        return AT_ERROR;
    }

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, ",,(0,1,2,3,4),(0,1,2)\r\n");

    At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);
    length = 0;

    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}

VOS_UINT32 AT_RcvMmaAccessModeQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_AccessModeQryCnf *accessModeCnf = (TAF_MMA_AccessModeQryCnf *)msg;
    VOS_UINT32                result;

    g_atSendDataBuff.bufLen = 0;

    /* �����ѯ�Ľ���ģʽΪ��Чֵ���򷵻�ERROR�����統ǰLTE only��֧��GU */
    if (accessModeCnf->accessMode >= MN_MMA_CPAM_RAT_TYPE_BUTT) {
        result = AT_ERROR;
    } else {
        result = AT_OK;

        g_atSendDataBuff.bufLen += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d,%d", g_parseContext[indexNum].cmdElement->cmdName,
            accessModeCnf->accessMode, accessModeCnf->plmnPrio);
    }

    return result;
}

VOS_UINT32 AT_RcvMmaCFPlmnSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CfplmnSetCnf *cFPLmnCnf = (TAF_MMA_CfplmnSetCnf *)msg;

    g_atSendDataBuff.bufLen = 0;

    if (cFPLmnCnf->rslt == TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        return AT_OK;
    } else {
        /* ��δ�����޸��漰�ӿڵ��ع���Ϊ�˱��ֺ�ԭ��GU�����һ���ԣ����ؽ������ʱ���������ΪAT_CME_UNKNOWN  */
        return AT_CME_UNKNOWN;
    }
}

TAF_VOID At_QryParaRspCfplmnProc(TAF_UINT8 indexNum, TAF_UINT8 opId, TAF_UINT8 *para)
{
    TAF_UINT16         length       = 0;
    TAF_USER_PlmnList *userPlmnList = VOS_NULL_PTR;
    TAF_UINT32         i;

    userPlmnList = (TAF_USER_PlmnList *)para;

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        userPlmnList->plmnNum);
    userPlmnList->plmnNum = AT_MIN(userPlmnList->plmnNum, TAF_USER_MAX_PLMN_NUM);
    for (i = 0; i < userPlmnList->plmnNum; i++) {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, ",\"%X%X%X", (0x0f00 & userPlmnList->plmn[i].mcc) >> 8,
            (0x00f0 & userPlmnList->plmn[i].mcc) >> 4, (0x000f & userPlmnList->plmn[i].mcc));

        if ((0x0f00 & userPlmnList->plmn[i].mnc) == 0x0f00) {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%X%X\"", (0x00f0 & userPlmnList->plmn[i].mnc) >> 4,
                (0x000f & userPlmnList->plmn[i].mnc));
        } else {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%X%X%X\"", (0x0f00 & userPlmnList->plmn[i].mnc) >> 8,
                (0x00f0 & userPlmnList->plmn[i].mnc) >> 4, (0x000f & userPlmnList->plmn[i].mnc));
        }
    }

    g_atSendDataBuff.bufLen = length;
}

VOS_UINT32 AT_RcvMmaCFPlmnQueryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CfplmnQueryCnf *cFPlmnQrynf = (TAF_MMA_CfplmnQueryCnf *)msg;
    TAF_USER_PlmnList      *cFPlmnList  = VOS_NULL_PTR;
    VOS_UINT32              result;
    VOS_UINT32              loop;

    /*lint -save -e516 */
    cFPlmnList = (TAF_USER_PlmnList *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(TAF_USER_PlmnList));
    /*lint -restore */
    if (cFPlmnList == VOS_NULL_PTR) {
        AT_WARN_LOG("AT_RcvMmaCFPlmnQueryCnf : alloc fail");
        return AT_CMD_NO_NEED_FORMAT_RSLT;
    }

    (VOS_VOID)memset_s(cFPlmnList, sizeof(TAF_USER_PlmnList), 0x00, sizeof(TAF_USER_PlmnList));

    if (cFPlmnQrynf->rslt != TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        g_atSendDataBuff.bufLen = 0;
        /* ��δ�����޸��漰�ӿڵ��ع���Ϊ�˱��ֺ�ԭ��GU�����һ���ԣ����ؽ������ʱ���������ΪAT_CME_UNKNOWN  */
        result = AT_CME_UNKNOWN;
    } else {
        if (cFPlmnQrynf->plmnNum > TAF_USER_MAX_PLMN_NUM) {
            cFPlmnQrynf->plmnNum = TAF_USER_MAX_PLMN_NUM;
        }

        cFPlmnList->plmnNum = cFPlmnQrynf->plmnNum;

        for (loop = 0; (loop < cFPlmnQrynf->plmnNum); loop++) {
            cFPlmnList->plmn[loop].mcc = cFPlmnQrynf->plmn[loop].mcc;
            cFPlmnList->plmn[loop].mnc = cFPlmnQrynf->plmn[loop].mnc;
        }

        At_QryParaRspCfplmnProc(indexNum, cFPlmnQrynf->ctrl.opId, (TAF_UINT8 *)cFPlmnList);
        result = AT_OK;
    }
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, cFPlmnList);
    /*lint -restore */
    return result;
}

VOS_UINT32 AT_RcvMmaPrefPlmnTypeSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_PrefPlmnTypeSetCnf *prefPlmnTypeSetCnf = (TAF_MMA_PrefPlmnTypeSetCnf *)msg;
    AT_ModemNetCtx             *netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    netCtx->prefPlmnType = prefPlmnTypeSetCnf->prefPlmnType;
    g_atSendDataBuff.bufLen = 0;

    return AT_OK;
}

VOS_VOID AT_ReportQryPrefPlmnCmdPara(TAF_MMA_PrefPlmnQueryCnf *cpolQryCnf, AT_ModemNetCtx *netCtx, VOS_UINT16 *length,
                                     VOS_UINT32 indexNum, VOS_UINT32 loop)
{
    TAF_PLMN_NameList  *availPlmnInfo = VOS_NULL_PTR;
    AT_COPS_FORMAT_TYPE currFormat;

    availPlmnInfo = (TAF_PLMN_NameList *)&cpolQryCnf->plmnName;

    /* +CPOL:   */
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + (*length), "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    /* <indexNum> */
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + (*length), "%d", ((loop + cpolQryCnf->fromIndex) + 1));

    currFormat = netCtx->cpolFormatType;

    if ((availPlmnInfo->plmnName[loop].operatorNameLong[0] == '\0') && (currFormat == AT_COPS_LONG_ALPH_TYPE)) {
        currFormat = AT_COPS_NUMERIC_TYPE;
    }

    if ((availPlmnInfo->plmnName[loop].operatorNameShort[0] == '\0') && (currFormat == AT_COPS_SHORT_ALPH_TYPE)) {
        currFormat = AT_COPS_NUMERIC_TYPE;
    }

    /* <format> */
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + (*length), ",%d", currFormat);

    /* <oper1> */
    if (currFormat == AT_COPS_LONG_ALPH_TYPE) {
        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + (*length), ",\"%s\"", availPlmnInfo->plmnName[loop].operatorNameLong);
    } else if (currFormat == AT_COPS_SHORT_ALPH_TYPE) {
        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + (*length), ",\"%s\"", availPlmnInfo->plmnName[loop].operatorNameShort);
    } else {
        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + (*length), ",\"%X%X%X",
            (0x0f00 & availPlmnInfo->plmnName[loop].plmnId.mcc) >> 8,
            (0x00f0 & availPlmnInfo->plmnName[loop].plmnId.mcc) >> 4,
            (0x000f & availPlmnInfo->plmnName[loop].plmnId.mcc));

        if (((0x0f00 & availPlmnInfo->plmnName[loop].plmnId.mnc) >> 8) != 0x0F) {
            *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + (*length), "%X",
                (0x0f00 & availPlmnInfo->plmnName[loop].plmnId.mnc) >> 8);
        }

        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + (*length), "%X%X\"",
            (0x00f0 & availPlmnInfo->plmnName[loop].plmnId.mnc) >> 4,
            (0x000f & availPlmnInfo->plmnName[loop].plmnId.mnc));
    }
}

VOS_VOID AT_ReportQryPrefPlmnCmd(TAF_MMA_PrefPlmnQueryCnf *cpolQryCnf, VOS_UINT32 *validPlmnNum, AT_ModemNetCtx *netCtx,
                                 VOS_UINT16 *length, VOS_UINT32 indexNum)
{
    VOS_UINT32         i;
    TAF_PLMN_NameList *availPlmnInfo = VOS_NULL_PTR;
    TAF_MMC_USIM_RAT   plmnRat;

    availPlmnInfo = (TAF_PLMN_NameList *)&cpolQryCnf->plmnName;

    availPlmnInfo->plmnNum = AT_MIN(availPlmnInfo->plmnNum, TAF_MMA_MAX_PLMN_NAME_LIST_NUM);
    for (i = 0; i < availPlmnInfo->plmnNum; i++) {
        if (AT_PH_IsPlmnValid(&(availPlmnInfo->plmnName[i].plmnId)) == VOS_FALSE) {
            continue;
        }

        (*validPlmnNum)++;

        if ((cpolQryCnf->validPlmnNum == 0) && (*validPlmnNum == 1)) {
            /* �ο�V.250Э��5.7.2: ����V��������ø���Ϣ�ֶε�ͷβ���ӻس����з� */
            if (g_atVType == AT_V_ENTIRE_TYPE) {
                *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                    (VOS_CHAR *)g_atSndCodeAddress + (*length), "%s", g_atCrLf);
            }
        } else {
            *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + (*length), "%s", g_atCrLf);
        }

        AT_ReportQryPrefPlmnCmdPara(cpolQryCnf, netCtx, length, indexNum, i);

        if ((netCtx->prefPlmnType == MN_PH_PREF_PLMN_UPLMN) && (availPlmnInfo->plmnSelFlg == VOS_TRUE)) {
            continue;
        }

        /* <GSM_AcT1> */
        /* <GSM_Compact_AcT1> */
        /* <UTRAN_AcT1> */
        plmnRat = availPlmnInfo->plmnRat[i];

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        *length +=
            (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + (*length), ",%d,%d,%d,%d,%d", (plmnRat & TAF_MMC_USIM_GSM_RAT) >> 7,
                (plmnRat & TAF_MMC_USIM_GSM_COMPACT_RAT) >> 6, (plmnRat & TAF_MMC_USIM_UTRN_RAT) >> 15,
                (plmnRat & TAF_MMC_USIM_E_UTRN_RAT) >> 14, (plmnRat & TAF_MMC_USIM_NR_RAT) >> 11);
#else
        *length +=
            (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + (*length), ",%d,%d,%d,%d", (plmnRat & TAF_MMC_USIM_GSM_RAT) >> 7,
                (plmnRat & TAF_MMC_USIM_GSM_COMPACT_RAT) >> 6, (plmnRat & TAF_MMC_USIM_UTRN_RAT) >> 15,
                (plmnRat & TAF_MMC_USIM_E_UTRN_RAT) >> 14);
#endif
    }
}

VOS_UINT32 AT_RcvMmaPrefPlmnSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_PrefPlmnSetCnf *prefPlmnCnf = (TAF_MMA_PrefPlmnSetCnf *)msg;

    g_atSendDataBuff.bufLen = 0;

    if (prefPlmnCnf->rslt != TAF_MMA_APP_PREF_PLMN_OPER_RESULT_SUCCESS) {
        return AT_CME_UNKNOWN;
    } else {
        return AT_OK;
    }
}

VOS_UINT32 AT_RcvMmaPrefPlmnQueryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    VOS_UINT32                result        = AT_FAILURE;
    VOS_UINT16                length        = 0;
    TAF_PLMN_NameList        *availPlmnInfo = VOS_NULL_PTR;
    AT_ModemNetCtx           *netCtx        = VOS_NULL_PTR;
    VOS_UINT32                validPlmnNum;
    TAF_MMA_CpolInfoQueryReq  cpolInfo;
    TAF_MMA_PrefPlmnQueryCnf *cpolQryCnf = VOS_NULL_PTR;

    cpolQryCnf = (TAF_MMA_PrefPlmnQueryCnf *)msg;

    availPlmnInfo = (TAF_PLMN_NameList *)&cpolQryCnf->plmnName;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    if (cpolQryCnf->rslt != TAF_MMA_APP_PREF_PLMN_OPER_RESULT_SUCCESS) {
        g_atSendDataBuff.bufLen = 0;
        return AT_CME_UNKNOWN;
    }

    validPlmnNum = 0;
    AT_ReportQryPrefPlmnCmd(cpolQryCnf, &validPlmnNum, netCtx, &length, indexNum);

    /*
     * ATģ�����MMA�ϱ�����Ӫ�̸�����ȷ����Ӫ����Ϣ�Ƿ��ռ����: С���������Ӫ��
     * ��������Ϊ��Ӫ����Ϣ�Ѿ��ռ���ɣ�ԭ����������:
     * ATģ��ÿ������37����Ӫ����Ϣ����ʼλ��Ϊ�Ѿ���������һ����Ӫ�̵���һ����Ӫ����Ϣ����
     * ������������Ӫ����Ϣ��������37������ʵ�ʵ������
     * �����Ӫ����Ϊ37�ı�������AT���ٷ�һ����Ӫ����Ϣ����MMA�ظ�����Ӫ����Ϣ����Ϊ0
     */
    if (availPlmnInfo->plmnNum < TAF_MMA_MAX_PLMN_NAME_LIST_NUM) {
        /* �ο�V.250Э��5.7.2: ����V��������ø���Ϣ�ֶε�ͷβ���ӻس����з� */
        if ((g_atVType == AT_V_ENTIRE_TYPE) && ((cpolQryCnf->validPlmnNum + validPlmnNum) != 0)) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);
        }

        At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);

        g_atSendDataBuff.bufLen = 0;
        return AT_OK;
    } else {
        At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);

        cpolInfo.prefPLMNType = netCtx->prefPlmnType;
        cpolInfo.fromIndex    = (cpolQryCnf->fromIndex + availPlmnInfo->plmnNum);
        cpolInfo.plmnNum      = TAF_MMA_MAX_PLMN_NAME_LIST_NUM;
        cpolInfo.validPlmnNum = (cpolQryCnf->validPlmnNum + validPlmnNum);
        result                = TAF_MMA_QueryCpolReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &cpolInfo);
        if (result != VOS_TRUE) {
            g_atSendDataBuff.bufLen = 0;
            return AT_CME_UNKNOWN;
        }
    }

    return AT_CMD_NO_NEED_FORMAT_RSLT;
}

VOS_UINT32 AT_RcvMmaPrefPlmnTestCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_PrefPlmnTestCnf *prefPlmnCnf = (TAF_MMA_PrefPlmnTestCnf *)msg;
    VOS_UINT16               length = 0;
    VOS_UINT32               result = AT_ERROR;

    if (prefPlmnCnf->rslt != TAF_MMA_APP_PREF_PLMN_OPER_RESULT_SUCCESS) {
        g_atSendDataBuff.bufLen = length;
        return AT_CME_UNKNOWN;
    }

    /* ��ʽ��AT+CPOL��������� */
    g_atSendDataBuff.bufLen = 0;
    if (prefPlmnCnf->plmnNum == 0) {
        result = AT_CME_OPERATION_NOT_ALLOWED;
    } else {
        length = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (1-%d),(0-2)", g_parseContext[indexNum].cmdElement->cmdName,
            prefPlmnCnf->plmnNum);

        result = AT_OK;
    }

    g_atSendDataBuff.bufLen = length;
    return result;
}

VOS_UINT32 AT_RcvTafMmaQuickStartSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_QuickstartSetCnf *quickStartSetCnf = (TAF_MMA_QuickstartSetCnf *)msg;

    AT_PR_LOGI("Rcv Msg");

    if (quickStartSetCnf->result == TAF_PARA_OK) {
        return AT_OK;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_RcvTafMmaQuickStartQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_QuickstartQryCnf *quickStartQryCnf = (TAF_MMA_QuickstartQryCnf *)msg;

    g_atSendDataBuff.bufLen = 0;

    /* �����ѯ�Ľ���ģʽΪ��Чֵ���򷵻�ERROR�����統ǰLTE only��֧��GU */
    if (quickStartQryCnf->errorCause != TAF_ERR_NO_ERROR) {
        return AT_ERROR;
    } else {
        g_atSendDataBuff.bufLen += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            quickStartQryCnf->quickStartMode);

        return AT_OK;
    }
}

VOS_UINT32 AT_RcvMmaAcInfoQueryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_AcInfoQryCnf *acInfoQueryCnf = (TAF_MMA_AcInfoQryCnf *)msg;
    VOS_UINT32            result;
    VOS_UINT16            length;

    length         = 0;
    result         = AT_OK;

    /* �жϲ�ѯ����Ƿ�ʧ��,���ʧ���򷵻�ERROR */
    if (acInfoQueryCnf->rslt != VOS_OK) {
        result = AT_ERROR;
    } else {
        result = AT_OK;

        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s:%d,%d,%d,%d", g_parseContext[indexNum].cmdElement->cmdName,
            (VOS_UINT8)(acInfoQueryCnf->cellCsAcInfo.srvDomain),          /* �ϱ������� */
            (VOS_UINT8)(acInfoQueryCnf->cellCsAcInfo.cellAcType),         /* �ϱ�С����ֹ�������� */
            (VOS_UINT8)(acInfoQueryCnf->cellCsAcInfo.restrictRegister),   /* �ϱ��Ƿ�ע������ */
            (VOS_UINT8)(acInfoQueryCnf->cellCsAcInfo.restrictPagingRsp)); /* �ϱ��Ƿ�Ѱ������ */

        /* ���� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s:%d,%d,%d,%d", g_parseContext[indexNum].cmdElement->cmdName,
            (VOS_UINT8)(acInfoQueryCnf->cellPsAcInfo.srvDomain),          /* �ϱ������� */
            (VOS_UINT8)(acInfoQueryCnf->cellPsAcInfo.cellAcType),         /* �ϱ�С����ֹ�������� */
            (VOS_UINT8)(acInfoQueryCnf->cellPsAcInfo.restrictRegister),   /* �ϱ��Ƿ�ע������ */
            (VOS_UINT8)(acInfoQueryCnf->cellPsAcInfo.restrictPagingRsp)); /* �ϱ��Ƿ�Ѱ������ */

        g_atSendDataBuff.bufLen = length;
    }

    return result;
}

VOS_UINT32 AT_RcvMmaMMPlmnInfoQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_MmplmninfoQryCnf *mMPlmnInfoCnf = (TAF_MMA_MmplmninfoQryCnf *)msg;
    VOS_UINT32                result = AT_ERROR;
    VOS_UINT16                length = 0;
    VOS_UINT32                i;

    /* ת��LongName��ShortName */
    if (mMPlmnInfoCnf->mmPlmnInfo.longNameLen <= TAF_PH_OPER_NAME_LONG &&
        mMPlmnInfoCnf->mmPlmnInfo.shortNameLen <= TAF_PH_OPER_NAME_SHORT) {
        /* ^MMPLMNINFO:<long name>,<short name> */
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);

        for (i = 0; i < mMPlmnInfoCnf->mmPlmnInfo.longNameLen; i++) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%02X", mMPlmnInfoCnf->mmPlmnInfo.longName[i]);
        }

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",");

        for (i = 0; i < mMPlmnInfoCnf->mmPlmnInfo.shortNameLen; i++) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%02X", mMPlmnInfoCnf->mmPlmnInfo.shortName[i]);
        }

        result                  = AT_OK;
    }

    g_atSendDataBuff.bufLen = length;

    return result;
}

VOS_UINT32 AT_RcvMmaCopnInfoQueryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CopnInfoQryCnf *copnInfo = VOS_NULL_PTR;
    TAF_PH_OperatorName    *plmnName = VOS_NULL_PTR;
    VOS_UINT16              fromIndex;
    VOS_UINT16              length;
    VOS_UINT32              loop;
    VOS_UINT32              ret;

    /* ��ʼ�� */
    copnInfo = (TAF_MMA_CopnInfoQryCnf *)msg;
    length   = 0;

    /* ����+COPS��^MMPLMNINFO����� */
    plmnName = (TAF_PH_OperatorName *)copnInfo->content;

    for (loop = 0; loop < copnInfo->plmnNum; loop++) {
        /*  +COPN: <operator in numeric format><operator in long alphanumeric format>     */
        /* ��������� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

        /* ������ָ�ʽ��Ӫ������ */
        length += (VOS_UINT16)AT_FormatReportString( AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)(g_atSndCodeAddress + length), "\"%X%X%X",
            (TAF_MMA_PLMN_MCC_DIGIT3_MASK & plmnName->plmnId.mcc) >> TAF_MMA_PLMN_MCC_DIGIT3_OFFSET,
            (TAF_MMA_PLMN_MCC_DIGIT2_MASK & plmnName->plmnId.mcc) >> TAF_MMA_PLMN_MCC_DIGIT2_OFFSET,
            (TAF_MMA_PLMN_MCC_DIGIT1_MASK & plmnName->plmnId.mcc) >> TAF_MMA_PLMN_MCC_DIGIT1_OFFSET);

        if ((TAF_MMA_PLMN_MNC_DIGIT3_MASK & plmnName->plmnId.mnc) != TAF_MMA_PLMN_MNC_DIGIT3_MASK) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)(g_atSndCodeAddress + length), "%X",
                (TAF_MMA_PLMN_MNC_DIGIT3_MASK & plmnName->plmnId.mnc) >> TAF_MMA_PLMN_MNC_DIGIT3_OFFSET);
        }

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%X%X\"",
            (TAF_MMA_PLMN_MNC_DIGIT2_MASK & plmnName->plmnId.mnc) >> TAF_MMA_PLMN_MNC_DIGIT2_OFFSET,
            (TAF_MMA_PLMN_MNC_DIGIT1_MASK & plmnName->plmnId.mnc) >> TAF_MMA_PLMN_MNC_DIGIT1_OFFSET);

        /* ��Ӷ��ŷָ��� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",\"");

        /* �����Ӫ�̳��� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s", plmnName->operatorNameLong);

        /* ������� */
        /* ��Ӷ��ŷָ��� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "\"%s", g_atCrLf);

        At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);

        length = 0;

        plmnName++;
    }

    /*
     * ATģ�����MMA�ϱ�����Ӫ�̸�����ȷ����Ӫ����Ϣ�Ƿ��ռ����: С���������Ӫ��
     * ��������Ϊ��Ӫ����Ϣ�Ѿ��ռ���ɣ�ԭ����������:
     * ATģ��ÿ������50����Ӫ����Ϣ����ʼλ��Ϊ�Ѿ���������һ����Ӫ�̵���һ����Ӫ����Ϣ����
     * ������������Ӫ����Ϣ��������50������ʵ�ʵ������
     * �����Ӫ����Ϊ50�ı�������AT���ٷ�һ����Ӫ����Ϣ����MMA�ظ�����Ӫ����Ϣ����Ϊ0
     */
    if (copnInfo->plmnNum < TAF_MMA_COPN_PLMN_MAX_NUM) {
        g_atSendDataBuff.bufLen = 0;
        return AT_OK;
    } else {
        fromIndex = copnInfo->fromIndex + copnInfo->plmnNum;

        ret = TAF_MMA_QryCopnInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, fromIndex, 0);

        if (ret != VOS_TRUE) {
            g_atSendDataBuff.bufLen = 0;
            return AT_ERROR;
        }
    }
    return AT_CMD_NO_NEED_FORMAT_RSLT;
}

VOS_UINT32 AT_RcvMmaRejinfoQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_RejinfoQryCnf *rejinfoQryCnf = (TAF_MMA_RejinfoQryCnf *)msg;
    VOS_UINT16             length = 0;

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    /* ��ӡrejinfo AT������� */
    length = AT_PrintRejinfo(length, &(rejinfoQryCnf->phoneRejInfo));

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_VOID AT_ReportNetScanInfo(VOS_UINT8 indexNum, VOS_UINT8 freqNum, TAF_MMA_NetScanInfo *netScanInfo)
{
    VOS_CHAR  *pcBandFormat = VOS_NULL_PTR;
    VOS_UINT16 length = 0;
    VOS_UINT32 i;
    VOS_INT32  j;
    VOS_CHAR   cellIdStr[AT_CELLID_STRING_MAX_LEN];
    VOS_BOOL   bFirstBand;
    VOS_UINT8  freqNumTmp;

    freqNumTmp = (VOS_UINT8)AT_MIN(freqNum, TAF_MMA_NET_SCAN_MAX_FREQ_NUM);
    (VOS_VOID)memset_s(cellIdStr, sizeof(cellIdStr), 0x00, sizeof(cellIdStr));

    for (i = 0; i < freqNumTmp; i++) {
        bFirstBand = VOS_FALSE;

        /* ^NETSCAN: [<arfcn>],[<c1>],[<c2>],[<lac>],[<mcc>],[<mnc>] */
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %u,,,%x,%x%x%x,%x%x",
            g_parseContext[indexNum].cmdElement->cmdName, netScanInfo[i].arfcn, netScanInfo[i].lac,
            (netScanInfo[i].mcc & 0x0f), (netScanInfo[i].mcc & 0x0f00) >> 8, (netScanInfo[i].mcc & 0x0f0000) >> 16,
            (netScanInfo[i].mnc & 0x0f), (netScanInfo[i].mnc & 0x0f00) >> 8);

        /* MNC������һ������F������Ҫ��ʾ */
        if (((netScanInfo[i].mnc & 0x0f0000) >> 16) != 0x0f) {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%x", (netScanInfo[i].mnc & 0x0f0000) >> 16);
        }

        /* NR��CellId������չΪU64�ٴ�ӡ��4�ֽ� */
        /* ,[<bsic>],[<rxlev>][,<cid>, */
        AT_ConvertCellIdToHexStrFormat(netScanInfo[i].cellId.cellIdLowBit, netScanInfo[i].cellId.cellIdHighBit,
                                       cellIdStr);

        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",%d,%d,%s,", netScanInfo[i].bsic, netScanInfo[i].rxlev,
            cellIdStr);

        /* [<band>] */
        for (j = TAF_MMA_BAND_MAX_LEN - 1; j >= 0; j--) {
            if ((netScanInfo[i].band.band[j] == 0) && (bFirstBand == VOS_FALSE)) {
                continue;
            }

            if (bFirstBand == VOS_FALSE) {
                pcBandFormat = "%X";
                bFirstBand   = VOS_TRUE;
            } else {
                pcBandFormat = "%08X";
            }

            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, pcBandFormat, netScanInfo[i].band.band[j]);
        }

        /* ,[psc],[pci] */
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",%d,%u", netScanInfo[i].psc, netScanInfo[i].phyId);

        if (netScanInfo[i].nrFlg == VOS_TRUE) {
            /* [,<5GSCS>,<5GRSRP>,<5GRSRQ>,<5GSINR>]] */
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, ",%d,%d,%d,%d", netScanInfo[i].nrScsInfo,
                netScanInfo[i].nrRsrp, netScanInfo[i].nrRsrq, netScanInfo[i].nrSinr);
        }

        if (i != (freqNum - 1)) {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);
        }
    }

    AT_StopTimerCmdReady(indexNum);

    /* ���FREQNUM ==0����ֻ�ϱ�OK */
    g_atSendDataBuff.bufLen = length;
    At_FormatResultData(indexNum, AT_OK);
}

VOS_UINT32 AT_ConvertMmaNetScanCauseToAt(TAF_MMA_NetScanCauseUint8 netScanCause)
{
    VOS_UINT32 result;

    /* ����NetScan�ϱ��Ĵ�����ת����AT��ʾ�Ĵ����� */
    switch (netScanCause) {
        case TAF_MMA_NET_SCAN_CAUSE_SIGNAL_EXIST:
            result = AT_ERROR;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_STATE_NOT_ALLOWED:
            result = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_FREQ_LOCK:
            result = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_PARA_ERROR:
            result = AT_CME_INCORRECT_PARAMETERS;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_CONFLICT:
            result = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_SERVICE_EXIST:
            result = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_NOT_CAMPED:
            result = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_TIMER_EXPIRED:
            result = AT_ERROR;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_RAT_TYPE_ERROR:
            result = AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case TAF_MMA_NET_SCAN_CAUSE_MMA_STATE_DISABLE:
            result = AT_ERROR;
            break;

        default:
            result = AT_ERROR;
            break;
    }

    return result;
}

VOS_UINT32 AT_RcvMmaNetScanCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_NetScanCnf *netScanCnf = (TAF_MMA_NetScanCnf *)msg;
    VOS_UINT32          result;

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvMmaNetScanCnf : AT_BROADCAST_INDEX.");
        return AT_CMD_NO_NEED_FORMAT_RSLT;
    }

    if (netScanCnf->result == TAF_MMA_NET_SCAN_RESULT_SUCCESS) {
        AT_ReportNetScanInfo(indexNum, netScanCnf->freqNum, netScanCnf->netScanInfo);
    } else {
        AT_StopTimerCmdReady(indexNum);

        g_atSendDataBuff.bufLen = 0;
        result                  = AT_ConvertMmaNetScanCauseToAt(netScanCnf->cause);
        At_FormatResultData(indexNum, result);
    }

    return AT_CMD_NO_NEED_FORMAT_RSLT;
}

VOS_UINT32 AT_RcvMmaEmRssiCfgQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_EmrssicfgQryCnf *emRssiCfgQryCnf = (TAF_MMA_EmrssicfgQryCnf *)msg;
    AT_RreturnCodeUint32     result          = AT_FAILURE;
    VOS_UINT16               length          = 0;

    g_atSendDataBuff.bufLen = 0;

    if (emRssiCfgQryCnf->errorCause != TAF_ERR_NO_ERROR) {
        result = AT_ERROR;
    } else {
        /* GSM EMRSSICFG��� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d,%d%s", g_parseContext[indexNum].cmdElement->cmdName,
            TAF_MMA_RAT_GSM, emRssiCfgQryCnf->emRssiCfgPara.emRssiCfgGsmThreshold, g_atCrLf);

        /* WCDMA EMRSSICFG��� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d,%d%s", g_parseContext[indexNum].cmdElement->cmdName,
            TAF_MMA_RAT_WCDMA, emRssiCfgQryCnf->emRssiCfgPara.emRssiCfgWcdmaThreshold, g_atCrLf);

        /* LTE EMRSSICFG��� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d,%d", g_parseContext[indexNum].cmdElement->cmdName,
            TAF_MMA_RAT_LTE, emRssiCfgQryCnf->emRssiCfgPara.emRssiCfgLteThreshold);

        result = AT_OK;
    }

    g_atSendDataBuff.bufLen = length;
    return result;
}

VOS_VOID At_QryEonsUcs2RspProc(VOS_UINT8 indexNum, VOS_UINT8 opId,
                               TAF_MMA_OperateName *eonsUcs2PlmnName, TAF_MMA_EonsUcs2HnbName *eonsUcs2HNBName)
{
    TAF_MMA_OperateName      *plmnName = VOS_NULL_PTR;
    VOS_UINT16                length;
    VOS_UINT32                loop;

#if (FEATURE_CSG == FEATURE_ON)
    TAF_MMA_EonsUcs2HnbName *hnbName = VOS_NULL_PTR;
    VOS_UINT8                homeNodeBLen;

    hnbName = (TAF_MMA_EonsUcs2HnbName *)eonsUcs2HNBName;

    length = 0;

    if (hnbName != VOS_NULL_PTR) {
        if (hnbName->homeNodeBNameLen != 0) {
            length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);

            /* ��ӡhome NodeB Name, ucs2���룬��󳤶�48�ֽ� */
            homeNodeBLen = AT_MIN(hnbName->homeNodeBNameLen, TAF_MMA_MAX_HOME_NODEB_NAME_LEN);

            for (loop = 0; loop < homeNodeBLen; loop++) {
                length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                    (VOS_CHAR *)g_atSndCodeAddress + length, "%02X", hnbName->homeNodeBName[loop]);
            }
            g_atSendDataBuff.bufLen = length;

            return;
        }
    }

#endif

    /* ������ʼ�� */
    plmnName = (TAF_MMA_OperateName *)eonsUcs2PlmnName;

    if (plmnName == VOS_NULL_PTR) {
        return;
    }

    /* ת��LongName��ShortName */
    if (plmnName->longNameLen <= TAF_PH_OPER_NAME_LONG && plmnName->shortNameLen <= TAF_PH_OPER_NAME_SHORT) {
        /* ^EONSUCS2:<long name>,<short name> */
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);

        for (loop = 0; loop < plmnName->longNameLen; loop++) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%02X", plmnName->longName[loop]);
        }

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",");

        for (loop = 0; loop < plmnName->shortNameLen; loop++) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%02X", plmnName->shortName[loop]);
        }

        g_atSendDataBuff.bufLen = length;
    } else {
        g_atSendDataBuff.bufLen = 0;
    }
}

VOS_UINT32 AT_RcvMmaEonsUcs2Cnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_EonsUcs2Cnf *eonsUcs2QryCnfMsg = (TAF_MMA_EonsUcs2Cnf *)msg;

    /* ��ʽ��AT^EONSUCS2��ѯ����� */
    if (eonsUcs2QryCnfMsg->rslt != TAF_ERR_NO_ERROR) {
        g_atSendDataBuff.bufLen = 0;
        return At_ChgTafErrorCode(indexNum, (VOS_UINT16)eonsUcs2QryCnfMsg->errorCause);
    }

    At_QryEonsUcs2RspProc(indexNum, eonsUcs2QryCnfMsg->ctrl.opId, &eonsUcs2QryCnfMsg->eonsUcs2PlmnName,
                          &eonsUcs2QryCnfMsg->eonsUcs2HNBName);

    return AT_OK;
}

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_RcvMmaVoiceDomainSetCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_VoiceDomainSetCnf *cnfMsg = (TAF_MMA_VoiceDomainSetCnf *)msg;

    g_atSendDataBuff.bufLen = 0;

    if (cnfMsg->result == TAF_MMA_APP_OPER_RESULT_SUCCESS) {
        return AT_OK;
    } else {
        return At_ChgTafErrorCode(indexNum, cnfMsg->errorCause);
    }
}

VOS_UINT32 AT_VoiceDomainTransToOutputValue(TAF_MMA_VoiceDomainUint32 voiceDoman, VOS_UINT32 *value)
{
    VOS_UINT32 rst = VOS_TRUE;

    switch (voiceDoman) {
        case TAF_MMA_VOICE_DOMAIN_CS_ONLY:
            *value = AT_VOICE_DOMAIN_TYPE_CS_ONLY;
            break;

        case TAF_MMA_VOICE_DOMAIN_IMS_PS_ONLY:
            *value = AT_VOICE_DOMAIN_TYPE_IMS_PS_ONLY;
            break;

        case TAF_MMA_VOICE_DOMAIN_CS_PREFERRED:
            *value = AT_VOICE_DOMAIN_TYPE_CS_PREFERRED;
            break;

        case TAF_MMA_VOICE_DOMAIN_IMS_PS_PREFERRED:
            *value = AT_VOICE_DOMAIN_TYPE_IMS_PS_PREFERRED;
            break;

        default:
            *value = AT_VOICE_DOMAIN_TYPE_BUTT;
            rst    = VOS_FALSE;
            break;
    }

    return rst;
}

VOS_UINT32 AT_RcvMmaVoiceDomainQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_VoiceDomainQryCnf *domainQryCnf = (TAF_MMA_VoiceDomainQryCnf *)msg;
    VOS_UINT32                 value = 0;
    VOS_UINT32                 result;

    if (AT_VoiceDomainTransToOutputValue(domainQryCnf->voiceDomain, &value) != VOS_TRUE) {
        result = AT_ERROR;
    } else {
        result = AT_OK;
        g_atSendDataBuff.bufLen = 0;
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName, value);
    }

    return result;
}
#endif

VOS_VOID AT_PrintfPlmnInfoByMmaCrpnQryCnf(TAF_MMA_CrpnQryInfo *mnMmaCrpnQryCnf, VOS_UINT32 loop,
    VOS_UINT8 indexNum, VOS_UINT16 *length)
{
    errno_t              memResult;
    VOS_UINT8            plmn[TAF_MAX_AUTHDATA_PLMN_LEN + 1];

    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + *length, "%s", g_atCrLf);

    /* ^CRPN:  */
    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + *length, "%s: ", g_parseContext[indexNum].cmdElement->cmdName);

    /* <PLMN> */
    /* ����������������PLMN ID�������ʱֱ�Ӳ��������PLMN ID */
    if (mnMmaCrpnQryCnf->mnMmaCrpnQry.plmnType == AT_PLMN_TYPE_NUM) {
        (VOS_VOID)memset_s(plmn, sizeof(plmn), 0x00, sizeof(plmn));
        if (mnMmaCrpnQryCnf->mnMmaCrpnQry.plmnNameLen > 0) {
            memResult = memcpy_s(plmn, sizeof(plmn), mnMmaCrpnQryCnf->mnMmaCrpnQry.plmnName,
                                 TAF_MIN(TAF_MAX_AUTHDATA_PLMN_LEN, mnMmaCrpnQryCnf->mnMmaCrpnQry.plmnNameLen));
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(plmn),
                                TAF_MIN(TAF_MAX_AUTHDATA_PLMN_LEN, mnMmaCrpnQryCnf->mnMmaCrpnQry.plmnNameLen));
        }
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "\"%s\"", plmn);
    } else {
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "\"%X%X%X",
            (0x0f00 & mnMmaCrpnQryCnf->operNameList[loop].plmnId.mcc) >> 8,
            (0x00f0 & mnMmaCrpnQryCnf->operNameList[loop].plmnId.mcc) >> 4,
            (0x000f & mnMmaCrpnQryCnf->operNameList[loop].plmnId.mcc));

        if (mnMmaCrpnQryCnf->maxMncLen == AT_MAX_MNC_LEN) {
            *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + *length, "%X",
                (0x0f00 & mnMmaCrpnQryCnf->operNameList[loop].plmnId.mnc) >> 8);
        }
        *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + *length, "%X%X\"",
            (0x00f0 & mnMmaCrpnQryCnf->operNameList[loop].plmnId.mnc) >> 4,
            (0x000f & mnMmaCrpnQryCnf->operNameList[loop].plmnId.mnc));
    }

    /* <short name> */
    mnMmaCrpnQryCnf->operNameList[loop].operatorNameShort[TAF_PH_OPER_NAME_SHORT - 1] = '\0';
    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + *length, ",\"%s\"", mnMmaCrpnQryCnf->operNameList[loop].operatorNameShort);

    /* <long name> */
    mnMmaCrpnQryCnf->operNameList[loop].operatorNameLong[TAF_PH_OPER_NAME_LONG - 1] = '\0';
    *length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + *length, ",\"%s\"", mnMmaCrpnQryCnf->operNameList[loop].operatorNameLong);
}

VOS_UINT32 AT_RcvTafMmaCrpnQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_CrpnQryCnf  *crpnQryCnf = (TAF_MMA_CrpnQryCnf *)msg;
    TAF_MMA_CrpnQryInfo *mnMmaCrpnQryCnf = &crpnQryCnf->crpnQryInfo;
    TAF_MMA_CrpnQryPara  mnMmaCrpnQry;
    VOS_UINT32           loop = 0;
    errno_t              memResult;
    VOS_UINT16           length = 0;

    /* ��ʽ��AT^CRPN��ѯ����� */
    g_atSendDataBuff.bufLen = 0;
    if (crpnQryCnf->errorCause != TAF_ERR_NO_ERROR) {
        return At_ChgTafErrorCode(indexNum, (VOS_UINT16)crpnQryCnf->errorCause);
    }

    mnMmaCrpnQryCnf->totalNum = AT_MIN(mnMmaCrpnQryCnf->totalNum, TAF_PH_CRPN_PLMN_MAX_NUM);
    for (loop = 0; loop < mnMmaCrpnQryCnf->totalNum; loop++) {
        AT_PrintfPlmnInfoByMmaCrpnQryCnf(mnMmaCrpnQryCnf, loop, indexNum, &length);
    }

    g_atSendDataBuff.bufLen = length;
    At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);
    length = 0;

    /* ��������ϱ���CRPN��Ŀ��Ҫ�����ͬ������ΪC������Plmn listû���ϱ���Ҫ��������������в�ѯ */
    if (mnMmaCrpnQryCnf->totalNum == TAF_PH_CRPN_PLMN_MAX_NUM) {
        memResult = memcpy_s(&mnMmaCrpnQry, sizeof(mnMmaCrpnQry), &mnMmaCrpnQryCnf->mnMmaCrpnQry,
                             sizeof(TAF_MMA_CrpnQryPara));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(mnMmaCrpnQry), sizeof(TAF_MMA_CrpnQryPara));
        mnMmaCrpnQry.currIndex = mnMmaCrpnQryCnf->currIndex + mnMmaCrpnQryCnf->totalNum;
        mnMmaCrpnQry.qryNum    = TAF_PH_CRPN_PLMN_MAX_NUM;

        /* ����ID_TAF_MMA_CRPN_QRY_REQ��Ϣ��MMA���� */
        if (TAF_MMA_QryCrpnReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &mnMmaCrpnQry) == VOS_TRUE) {
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CRPN_QUERY;
            return AT_CMD_NO_NEED_FORMAT_RSLT;
        } else {
            g_atSendDataBuff.bufLen = 0;
            return AT_ERROR;
        }
    } else {
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);
        At_BufferorSendResultData(indexNum, g_atSndCodeAddress, length);
        length = 0;
        g_atSendDataBuff.bufLen = length;
        return AT_OK;
    }
}

VOS_UINT32 AT_RcvMmaUserSrvStateQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    TAF_MMA_UserSrvStateQryCnf *userSrvStateCnf = (TAF_MMA_UserSrvStateQryCnf *)msg;
    VOS_UINT16                  length = 0;

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "^USERSRVSTATE: ");

    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%d,%d", userSrvStateCnf->csSrvExistFlg,
        userSrvStateCnf->psSrvExistFlg);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 atSetCsqCnfSameProc(struct MsgCB *msgBlock)
{
    L4A_CSQ_InfoCnf *setCsqCnf   = NULL;
    VOS_UINT8        rssiValue   = 0;
    VOS_UINT8        channalQual = 0;

    VOS_UINT16 length = 0;

    setCsqCnf = (L4A_CSQ_InfoCnf *)msgBlock;

    if (setCsqCnf->errorCode == ERR_MSP_SUCCESS) {
        if (setCsqCnf->rssi == AT_RSSI_UNKNOWN) {
            rssiValue = AT_RSSI_UNKNOWN;
        } else if (setCsqCnf->rssi >= AT_RSSI_HIGH) {
            rssiValue = AT_CSQ_RSSI_HIGH;
        } else if (setCsqCnf->rssi <= AT_RSSI_LOW) {
            rssiValue = AT_CSQ_RSSI_LOW;
        } else {
            rssiValue = (VOS_UINT8)((setCsqCnf->rssi - AT_RSSI_LOW) / 2);
        }

        channalQual = AT_BER_UNKNOWN;
    } else {
        rssiValue   = AT_RSSI_UNKNOWN;
        channalQual = AT_BER_UNKNOWN;
    }

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddr,
        (VOS_CHAR *)g_atSndCodeAddr + length, "%s: %d,%d%s", "+CSQ", rssiValue, channalQual, g_atCrLf);

    CmdErrProc((VOS_UINT8)(setCsqCnf->clientId), ERR_MSP_SUCCESS, length, g_atSndCodeAddr);

    return AT_FW_CLIENT_STATUS_READY;
}
#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
 * ����˵��: �ж�LTEƵ���Ƿ���Ч
 * �������: ueSupportLteBand: UE֧�ֵ�Ƶ����Ϣ
 * ���ؽ��: VOS_TRU: Ƶ����Ч
 *           VOS_FALSE: Ƶ����Ч
 */
LOCAL VOS_UINT32 AT_SysCfgExTestCnfLteBandValid(const TAF_USER_SetLtePrefBandInfo *ueSupportLteBand)
{
    VOS_UINT32 bandIdx;
      /* �����һ��Ƶ�η�0������ΪƵ������Ч�� */
    for (bandIdx = 0; bandIdx < TAF_MMA_LTE_BAND_MAX_LENGTH; bandIdx++) {
        if (ueSupportLteBand->bandInfo[bandIdx] != 0) {
            return VOS_TRUE;
        }
    }
    return VOS_FALSE;
}

/*
 * ����˵��: LTEƵ����ϵ������ȡƵ�������ַ���
 * �������: sysCfgTestCnf: ��������صĽ��
 * �������: lteBandNameGroup: LTEƵ���������
 */
LOCAL VOS_VOID AT_GetLteBandNameGroup(const TAF_MMA_SyscfgTestCnf *sysCfgTestCnf,
    TAF_PH_SysCfgBand *lteBandNameGroup)
{
    VOS_UINT32 lteBandIdx;
    VOS_UINT32 syscfgBandIdx;
    VOS_UINT32 bandSet;
    VOS_UINT32 bandMask;
    VOS_UINT32 bandInd;
    VOS_UINT32 length = 0;
    VOS_CHAR bandNameGroup[AT_BAND_NAME_GROUP_MAX_VALUE];
    VOS_CHAR *tmpLteBandNameGroup = VOS_NULL_PTR;

    tmpLteBandNameGroup = lteBandNameGroup->strSysCfgBandGroup;

    (VOS_VOID)memset_s(lteBandNameGroup, sizeof(TAF_PH_SysCfgBand), 0x00, sizeof(TAF_PH_SysCfgBand));

    /* ���һ��˫���� */
    length += (TAF_UINT16)AT_FormatReportString(TAF_PH_SYSCFG_GROUP_BAND_LEN, tmpLteBandNameGroup,
        tmpLteBandNameGroup + length, "\"");

    /* ѭ������BAND���룬���֧�ָ�BAND������ַ�����ת�� */
    for (lteBandIdx = 0; lteBandIdx < TAF_MMA_LTE_BAND_MAX_LENGTH; lteBandIdx++) {
        bandSet = sysCfgTestCnf->ueSupportLteBand.bandInfo[lteBandIdx];

        /* ����һ����������ֵ�ڵ����е�Ƶ�� */
        for (syscfgBandIdx = 0; syscfgBandIdx < AT_SYSCFGEX_BAND_NUM_PER_SET; syscfgBandIdx++) {
            (VOS_VOID)memset_s(bandNameGroup, sizeof(bandNameGroup), 0x00, sizeof(bandNameGroup));

            bandMask = (VOS_UINT32)0x01 << syscfgBandIdx;
            bandInd  = lteBandIdx * AT_SYSCFGEX_BAND_NUM_PER_SET + syscfgBandIdx + 1;

            /* �����ǰBit��ʾ��Ƶ�β��ڷ��ؽ���У������������һ��Ƶ�� */
            if ((bandSet & bandMask) == 0) {
                continue;
            }
            (VOS_VOID)AT_FormatReportString(sizeof(bandNameGroup), bandNameGroup, bandNameGroup, "LTE BC%d/", bandInd);
            /* �ַ����������Ϊ256�ֽڣ�������ˣ����������Ƶ������ */
            if ((strlen(tmpLteBandNameGroup) + strlen(bandNameGroup)) >= TAF_PH_SYSCFG_GROUP_BAND_LEN) {
                (VOS_VOID)AT_FormatReportString(TAF_PH_SYSCFG_GROUP_BAND_LEN, tmpLteBandNameGroup,
                    tmpLteBandNameGroup + length - 1, "\"");
                return;
            }
            length += (TAF_UINT16)AT_FormatReportString(TAF_PH_SYSCFG_GROUP_BAND_LEN, tmpLteBandNameGroup,
                tmpLteBandNameGroup + length, "%s", bandNameGroup);
        }
    }
    /* �����һ��б���滻Ϊ���� */
    (VOS_VOID)AT_FormatReportString(TAF_PH_SYSCFG_GROUP_BAND_LEN, tmpLteBandNameGroup,
        tmpLteBandNameGroup + length - 1, "\"");
}

/*
 * ����˵��: ���ݷ��ص�LTEƵ������BitString
 * �������: sysCfgTestCnf: ��������صĽ��
 * �������: lteBandBitString: LTEƵ��BitString
 */
LOCAL VOS_VOID AT_GetLteBandBitString(const TAF_MMA_SyscfgTestCnf *sysCfgTestCnf,
    AtSysCfgExBandBitMask *lteBandBitString)
{
    VOS_INT32 lteBandIdx;
    VOS_UINT32 length = 0;
    VOS_UINT32 find = VOS_FALSE;
    VOS_CHAR *tmpBitString = lteBandBitString->bandBitString;

    (VOS_VOID)memset_s(lteBandBitString, sizeof(AtSysCfgExBandBitMask), 0x00, sizeof(AtSysCfgExBandBitMask));

    /* �Ӹߵ��ͱ���֧�ֵ�bandmask���������ַ���ת����� */
    for (lteBandIdx = TAF_MMA_LTE_BAND_MAX_LENGTH - 1; lteBandIdx >= 0; lteBandIdx--) {
        if ((sysCfgTestCnf->ueSupportLteBand.bandInfo[lteBandIdx] != 0) || (find == VOS_TRUE)) {
            if (find == VOS_FALSE) {
                /* �ҵ���һ����0Ƶ�Σ����ñ�־ΪTRUE */
                find = VOS_TRUE;
                /* ���ڵ�һ����0��Ƶ�Σ�����Ӹ�Ƶ�ο�ʼ����Ƶ�δ�ӡ */
                length += (TAF_UINT16)AT_FormatReportString(AT_SYSCFGEX_BAND_BIT_STRING_LEN, tmpBitString,
                    tmpBitString + length, "%x", sysCfgTestCnf->ueSupportLteBand.bandInfo[lteBandIdx]);
            } else {
                /* ֮ǰ�Ѿ�������ȵ�ǰƵ�θߵ�Ƶ�Σ�����ͳһ����8λ������� */
                length += (TAF_UINT16)AT_FormatReportString(AT_SYSCFGEX_BAND_BIT_STRING_LEN, tmpBitString,
                    tmpBitString + length, "%08x", sysCfgTestCnf->ueSupportLteBand.bandInfo[lteBandIdx]);
            }
        }
    }
}

/*
 * ����˵��: ����һ����ʽ�������
 * �������: len: ����ڴ泤��
 *           offset: �Ѵ��ڵ����ݳ���
 *           rat: Ҫ���ӵĽ��뼼��
 * �������: ratGroup: �����ʽ���
 * ���ؽ��: ���ӽ��뼼����ĳ���
 */
LOCAL VOS_UINT32 AT_AddOneRatToRatGroup(VOS_CHAR *ratGroup, VOS_UINT32 len, VOS_UINT32 offset, VOS_UINT8 rat)
{
    VOS_UINT32 length = offset;

    /* ���ÿ�����뼼�������Ӷ�Ӧ���ַ��� */
    switch (rat) {
        case AT_SYSCFGEX_ACT_AUTO:
        case AT_SYSCFGEX_ACT_GSM:
        case AT_SYSCFGEX_ACT_WCDMA:
        case AT_SYSCFGEX_ACT_LTE:
        case AT_SYSCFGEX_ACT_1XRTT:
        case AT_SYSCFGEX_ACT_EVDO:
        case AT_SYSCFGEX_ACT_NR:
        case AT_SYSCFGEX_ACT_NOT_CHANGE:
            length += (VOS_UINT32)AT_FormatReportString((VOS_INT32)len, ratGroup, ratGroup + offset, "%02x", rat);
            break;
        default:
            AT_WARN_LOG1("AT_AddOneRatToRatGroup:rat is invalid.", rat);
            break;
    }
    return length;
}

/*
 * ����˵��: ����NV50198�������õ���ʽ������ɷ��ؽ��
 * �������: len: ����ڴ泤��
 * �������: ratGroup: �����ʽ���
 * ����ֵ�� VOS_TRUE ��ʾ�Ѿ����û�NV���������÷��ؽ��
 *          VOS_FALSE ��ʾ����Ҫʹ���û�nv�������÷���ֵ����Ҫʹ��ԭ���߼����
 */
LOCAL VOS_UINT32 AT_GenerateRatGroupFormCustCfg(VOS_CHAR *ratGroup, VOS_UINT32 len)
{
    VOS_UINT32  loop;
    VOS_UINT32  length = 0;
    VOS_UINT8   ratIdx;
    VOS_UINT8  *tmpRatOrder = VOS_NULL_PTR;
    AT_ModemMbbSysCfgExRatGroupCtrl *mbbSysCfgExCtrl = AT_GetModemMbbSysCfgExRatGroupCtrl(MODEM_ID_0);
    if (mbbSysCfgExCtrl->itemNum == 0) {
        return VOS_FALSE;
    }
    /* ��������� */
    length += (VOS_UINT32)AT_FormatReportString((VOS_INT32)len, ratGroup, ratGroup + length, "%s", "(");

    for (loop = 0; loop < mbbSysCfgExCtrl->itemNum; loop++) {
        tmpRatOrder = mbbSysCfgExCtrl->ratGroupList[loop];
        /* ��ǰNV�����õ���ʽ��Ч */
        if (*tmpRatOrder == AT_SYSCFGEX_ACT_INVALID) {
            continue;
        }
        ratIdx = 0;
        /* ÿ�����ǰ������� */
        length += (VOS_UINT32)AT_FormatReportString((VOS_INT32)len, ratGroup, ratGroup + length, "%s", "\"");
        /* ÿ����ʽ��������6����ʽ */
        while ((ratIdx < AT_SYSCFGEX_RAT_MAX) && (*tmpRatOrder != AT_SYSCFGEX_ACT_INVALID)) {
            /* ���ÿ�����뼼�������Ӷ�Ӧ���ַ��� */
            length = AT_AddOneRatToRatGroup(ratGroup, len, length, *tmpRatOrder);
            ratIdx++;
            tmpRatOrder++;
        }
        /* ���֮��������� */
        length += (VOS_UINT32)AT_FormatReportString((VOS_INT32)len, ratGroup, ratGroup + length, "%s", "\"");
        if (loop != (mbbSysCfgExCtrl->itemNum - 1)) {
            /* ÿ����Ͻ�������Ӷ��� */
            length += (VOS_UINT32)AT_FormatReportString((VOS_INT32)len, ratGroup, ratGroup + length, "%s", ",");
        }
    }
    /* ��������� */
    length += (VOS_UINT32)AT_FormatReportString((VOS_INT32)len, ratGroup, ratGroup + length, "%s", ")");
    return VOS_TRUE;
}

/*
 * ����˵��: ��ʽ��AT^SYSCFGEX��������ؽ���е���ʽ���
 * �������: netCtx: ������������Ϣ
 *           testCnf: ����������
 *           index: �˿�ָʾ
 * �������: ratPara: �����ʽ���
 */
VOS_VOID AT_FormatSysCfgExTestResult(AT_ModemNetCtx *netCtx, const TAF_MMA_SyscfgTestCnf *testCnf,
    VOS_CHAR *ratPara, VOS_UINT8 index)
{
    errno_t memResult;
    VOS_UINT8 roamPara;
    TAF_PH_SysCfgBand lteBandNameGroup;
    AtSysCfgExBandBitMask lteBandBitString;
    VOS_CHAR ratGroupRslt[AT_SYSCFGEX_RAT_GROUP_MAX_LEN] = {0};
    VOS_CHAR lteBandInfo[AT_SYSCFGEX_BAND_NAME_GROUP_LEN] = {0};

    /* ������� */
    if (ratPara == VOS_NULL_PTR) {
        return;
    }
    /* ��ʼ�� */
    (VOS_VOID)memset_s(&lteBandBitString, sizeof(lteBandBitString), 0x00, sizeof(lteBandBitString));
    (VOS_VOID)memset_s(&lteBandNameGroup, sizeof(lteBandNameGroup), 0x00, sizeof(lteBandNameGroup));

    /* ����ն�֧��LTE�ҷ��صĽ����LTEƵ����Ч */
    if ((AT_IsModemSupportRat(MODEM_ID_0, TAF_MMA_RAT_LTE) == VOS_TRUE) &&
        (AT_SysCfgExTestCnfLteBandValid(&testCnf->ueSupportLteBand) == VOS_TRUE)) {
        /* ͨ��LTEƵ������Band BitString��NameGroup */
        AT_GetLteBandNameGroup(testCnf, &lteBandNameGroup);
        AT_GetLteBandBitString(testCnf, &lteBandBitString);
        (VOS_VOID)AT_FormatReportString(AT_SYSCFGEX_BAND_NAME_GROUP_LEN, lteBandInfo,
            lteBandInfo, "((%s,%s),(7fffffffffffffff,\"All bands\"))",
            lteBandBitString.bandBitString, lteBandNameGroup.strSysCfgBandGroup);
    } else {
        (VOS_VOID)AT_FormatReportString(AT_SYSCFGEX_BAND_NAME_GROUP_LEN, lteBandInfo,
            lteBandInfo, "((7fffffffffffffff,\"All bands\"))");
    }

    if (netCtx->roamFeature == AT_ROAM_FEATURE_OFF) {
        roamPara = 2; /* ���������ǿû�п��������β�����ȡֵΪ0/1/2 */
    } else {
        roamPara = 3; /* ���������ǿ�򿪣������β�����ȡֵΪ0/1/2/3 */
    }

    /* ����NV50198��������뼼����Ϸ�ʽ */
    if (AT_GenerateRatGroupFormCustCfg(ratGroupRslt, (VOS_UINT32)sizeof(ratGroupRslt)) == VOS_FALSE) {
        memResult = memcpy_s(ratGroupRslt, sizeof(ratGroupRslt), ratPara, strlen(ratPara));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(ratGroupRslt), strlen(ratPara));
    }
    /* ��������ַ��� */
    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %s,%s,(0-%d),(0-4),%s", g_parseContext[index].cmdElement->cmdName,
        ratGroupRslt, testCnf->bandInfo.strSysCfgBandGroup, roamPara, lteBandInfo);
}

#endif
