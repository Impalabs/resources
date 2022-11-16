/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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

#include "at_set_para_cmd.h"
#include "securec.h"
#include "at_mdrv_interface.h"
#include "taf_type_def.h"
#include "at_cmd_proc.h"
#include "at_input_proc.h"
#include "at_check_func.h"
#include "at_parse_cmd.h"
#include "ppp_interface.h"
#include "at_msg_print.h"
#include "si_app_stk.h"
#include "si_app_pih.h"
#include "at_phy_interface.h"
#include "at_data_proc.h"
#include "taf_drv_agent.h"
#include "mn_comm_api.h"
#include "at_cmd_msg_proc.h"
#include "at_rabm_interface.h"
#include "at_snd_msg.h"
#include "at_device_cmd.h"
#include "at_rnic_interface.h"
#include "at_taf_agent_interface.h"
#include "taf_agent_interface.h"
#include "taf_app_xsms_interface.h"

#include "taf_std_lib.h"
#include "mnmsgcbencdec.h"

#if (FEATURE_LTE == FEATURE_ON)
#include "mdrv_nvim.h"
#include "at_common.h"
#include "gen_msg.h"
#include "at_lte_common.h"
#endif
#if (FEATURE_LTEV == FEATURE_ON)
#include "vnas_at_interface.h"
#endif

#include "nv_stru_gucnas.h"
#include "nv_stru_sys.h"
#include "nv_stru_gas.h"
#include "nv_stru_pam.h"
#include "nv_stru_msp_interface.h"
#include "nv_stru_msp.h"

#include "mn_call_api.h"
#include "at_ctx.h"
#include "app_vc_api.h"
#include "at_imsa_interface.h"
#include "at_init.h"
#include "at_file_handle.h"

#include "gunas_errno.h"

#include "css_at_interface.h"
#include "taf_app_mma.h"
#include "taf_mtc_api.h"
#include "taf_ccm_api.h"

#if (FEATURE_ACORE_MODULE_TO_CCORE == FEATURE_OFF)
#if (VOS_OS_VER != VOS_LINUX)
#include "Linuxstub.h"
#endif
#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
#include "ads_dev_i.h"
#else
#include "ads_device_interface.h"
#endif
#else
#include "cds_interface.h"
#endif

#if (VOS_OS_VER == VOS_WIN32)
#include "NasStub.h"
#include "UsimmApi.h"
#endif

#include "imm_interface.h"
#include "throt_at_interface.h"
#include "at_voice_taf_set_cmd_proc.h"
#include "at_device_comm.h"

/*lint -esym(516,free,malloc)*/


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_SETPARACMD_C

#define AT_S_NSSAI_DIGIT_INDEX_2 2
#define AT_S_NSSAI_DIGIT_INDEX_3 3
#define AT_S_NSSAI_DIGITE_INDEX_0 0
#define AT_S_NSSAI_DIGITE_INDEX_1 1
#define AT_S_NSSAI_PUNC_NUM 2

AT_SetPortParaMap g_setPortParaMap[AT_SETPORT_DEV_MAP_LEN] = {
    { "A1", AT_DEV_CDROM, "CDROM" },
    { "A2", AT_DEV_SD, "SD" },
    { "A3", AT_DEV_RNDIS, "RNDIS" },
    { "A", AT_DEV_BLUE_TOOTH, "BLUE TOOTH" },
    { "B", AT_DEV_FINGER_PRINT, "FINGER PRINT" },
    { "D", AT_DEV_MMS, "MMS" },
#if (FEATURE_LTE == FEATURE_ON)
    { "E", AT_DEV_PC_VOICE, "3G PC VOICE" },
    { "1", AT_DEV_MODEM, "3G MODEM" },
    { "2", AT_DEV_PCUI, "3G PCUI" },
    { "3", AT_DEV_DIAG, "3G DIAG" },
#else
    { "E", AT_DEV_PC_VOICE, "PC VOICE" },
    { "1", AT_DEV_MODEM, "MODEM" },
    { "2", AT_DEV_PCUI, "PCUI" },
    { "3", AT_DEV_DIAG, "DIAG" },
#endif
    { "4", AT_DEV_PCSC, "PCSC" },
#if (FEATURE_LTE == FEATURE_ON)
    { "5", AT_DEV_GPS, "3G GPS" },
#else
    { "5", AT_DEV_GPS, "GPS" },
#endif
    { "6", AT_DEV_GPS_CONTROL, "GPS CONTROL" },
    { "7", AT_DEV_NDIS, "3G NDIS" },
    { "16", AT_DEV_NCM, "NCM" },
    { "10", AT_DEV_4G_MODEM, "4G MODEM" },
    { "11", AT_DEV_4G_NDIS, "4G NDIS" },
    { "12", AT_DEV_4G_PCUI, "4G PCUI" },
    { "13", AT_DEV_4G_DIAG, "4G DIAG" },
    { "14", AT_DEV_4G_GPS, "4G GPS" },
    { "15", AT_DEV_4G_PCVOICE, "4G PCVOICE" },
    { "FF", AT_DEV_NONE, "NO FIRST PORT" }
};

/* AT/OMͨ������·���� */
TAF_UINT8 g_atOmIndex = AT_MAX_CLIENT_NUM;

#if (FEATURE_LTE == FEATURE_ON)
AT_TmodeRatFlag g_tmodeRat      = {0};
VOS_UINT32      g_tmodeNum      = 0;
VOS_UINT32      g_guTmodeCnfNum = 0;
VOS_UINT32      g_lteOnly       = 0;
VOS_UINT32      g_guOnly        = 0;
VOS_UINT32      g_lteIsSend2Dsp = 0;

#endif

/* +CLCK�������CLASS��Service Type Code��Ӧ�� */
static const AT_ClckClassServiceTbl g_clckClassServiceTbl[] = {
    { AT_CLCK_PARA_CLASS_VOICE, TAF_SS_TELE_SERVICE, TAF_ALL_SPEECH_TRANSMISSION_SERVICES_TSCODE },
    { AT_CLCK_PARA_CLASS_DATA, TAF_SS_BEARER_SERVICE, TAF_ALL_BEARERSERVICES_BSCODE },
    { AT_CLCK_PARA_CLASS_FAX, TAF_SS_TELE_SERVICE, TAF_ALL_FACSIMILE_TRANSMISSION_SERVICES_TSCODE },
    { AT_CLCK_PARA_CLASS_VOICE_FAX, TAF_SS_TELE_SERVICE, TAF_ALL_TELESERVICES_EXEPTSMS_TSCODE },
    { AT_CLCK_PARA_CLASS_VOICE_DATA_FAX, TAF_SS_TELE_SERVICE, TAF_ALL_TELESERVICES_EXEPTSMS_TSCODE },
    { AT_CLCK_PARA_CLASS_SMS, TAF_SS_TELE_SERVICE, TAF_ALL_SMS_SERVICES_TSCODE },
    { AT_CLCK_PARA_CLASS_VOICE_FAX_SMS, TAF_SS_TELE_SERVICE, TAF_ALL_TELESERVICES_TSCODE },
    { AT_CLCK_PARA_CLASS_VOICE_DATA_FAX_SMS, TAF_SS_TELE_SERVICE, TAF_ALL_TELESERVICES_TSCODE },
    { AT_CLCK_PARA_CLASS_DATA_SYNC, TAF_SS_BEARER_SERVICE, TAF_ALL_DATA_CIRCUIT_SYNCHRONOUS_BSCODE },
    { AT_CLCK_PARA_CLASS_DATA_ASYNC, TAF_SS_BEARER_SERVICE, TAF_ALL_DATA_CIRCUIT_ASYNCHRONOUS_BSCODE },
    { AT_CLCK_PARA_CLASS_DATA_PKT, TAF_SS_BEARER_SERVICE, TAF_ALL_DATAPDS_SERVICES_BSCODE },
    { AT_CLCK_PARA_CLASS_DATA_SYNC_PKT, TAF_SS_BEARER_SERVICE, TAF_ALL_SYNCHRONOUS_SERVICES_BSCODE },
    { AT_CLCK_PARA_CLASS_DATA_PAD, TAF_SS_BEARER_SERVICE, TAF_ALL_PADACCESSCA_SERVICES_BSCODE },
    { AT_CLCK_PARA_CLASS_DATA_ASYNC_PAD, TAF_SS_BEARER_SERVICE, TAF_ALL_ASYNCHRONOUS_SERVICES_BSCODE },
    { AT_CLCK_PARA_CLASS_DATA_SYNC_ASYNC_PKT_PKT, TAF_SS_BEARER_SERVICE, TAF_ALL_BEARERSERVICES_BSCODE },
};

TAF_WRITE_AcoreNv g_writeAcoreNv = {0};

AT_MTA_BodySarStateUint16 g_atBodySarState = AT_MTA_BODY_SAR_OFF;

#if (VOS_WIN32 == VOS_OS_VER)
EXTERN const AT_USIMM_FileNumToId g_atUsimFileNumToIdTab[];
EXTERN const VOS_UINT32 g_atUsimFileNumToIdTabLen;

EXTERN const AT_USIMM_FileNumToId g_atSimFileNumToIdTab[];
EXTERN const VOS_UINT32 g_atSimFileNumToIdTabLen;

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
EXTERN const AT_USIMM_FileNumToId g_atCsimFileNumToIdTab[];

EXTERN VOS_UINT32 g_atCsimFileNumToIdTabLen;
#endif

#endif

/*
 * 3 ��������������
 */
#if (VOS_WIN32 == VOS_OS_VER)
/* ���TAF�Ļط����� */
VOS_VOID NAS_MSG_SndOutsideContextData_Part1(VOS_VOID);
VOS_VOID NAS_MSG_SndOutsideContextData_Part2(VOS_VOID);
VOS_VOID NAS_MSG_SndOutsideContextData_Part3(VOS_VOID);
#endif

VOS_BOOL AT_IsNVWRAllowedNvId(VOS_UINT16 nvId)
{
    TAF_NV_NvwrSecCtrl nvwrSecCtrlNV;
    VOS_UINT32         loop;
    VOS_UINT8          blockListNum;

    /* ������ʼ�� */
    memset_s(&nvwrSecCtrlNV, sizeof(nvwrSecCtrlNV), 0x00, sizeof(nvwrSecCtrlNV));

    /* ��ȡ��ȫ����NV */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_NVWR_SEC_CTRL, &nvwrSecCtrlNV, sizeof(nvwrSecCtrlNV)) != NV_OK) {
        AT_ERR_LOG("AT_IsNVWRAllowedNvId: TAF_ACORE_NV_READ fail!");
        return VOS_FALSE;
    }

    switch (nvwrSecCtrlNV.secType) {
        case AT_NVWR_SEC_TYPE_OFF:
            return VOS_TRUE;

        case AT_NVWR_SEC_TYPE_ON:
            return VOS_FALSE;

        case AT_NVWR_SEC_TYPE_BLOCKLIST:
            blockListNum = (nvwrSecCtrlNV.blockListNum <= TAF_NV_BLOCK_LIST_MAX_NUM) ? nvwrSecCtrlNV.blockListNum :
                                                                                       TAF_NV_BLOCK_LIST_MAX_NUM;
            for (loop = 0; loop < blockListNum; loop++) {
                if (nvId == nvwrSecCtrlNV.blockList[loop]) {
                    return VOS_FALSE;
                }
            }
            return VOS_TRUE;

        default:
            AT_ERR_LOG1("AT_IsNVWRAllowedNvId: Error SecType:", nvwrSecCtrlNV.secType);
            break;
    }

    return VOS_FALSE;
}

TAF_UINT32 At_SsPrint2Class(TAF_SS_BasicService *bsService, TAF_UINT8 classValue)
{
    VOS_UINT32 loop;
    VOS_UINT32 itemsNum;

    itemsNum = sizeof(g_clckClassServiceTbl) / sizeof(AT_ClckClassServiceTbl);

    /* �����д��ӦClass�ķ������ͼ������� */
    for (loop = 0; loop < itemsNum; loop++) {
        if (g_clckClassServiceTbl[loop].classType == classValue) {
            bsService->bsType        = g_clckClassServiceTbl[loop].serviceType;
            bsService->bsServiceCode = g_clckClassServiceTbl[loop].serviceCode;
            return AT_SUCCESS;
        }
    }

    return AT_FAILURE;
}

VOS_UINT32 AT_SetHighFourBitHexString(VOS_UINT8 *outPut, VOS_UINT8 *src, VOS_UINT16 indexNum)
{
    if ((src[indexNum] >= '0') && (src[indexNum] <= '9')) { /* the number is 0-9 */
        *outPut = src[indexNum] - '0';
    } else if ((src[indexNum] >= 'a') && (src[indexNum] <= 'f')) { /* the number is 0-9 */
        *outPut = (src[indexNum] - 'a') + 0x0a;
    } else if ((src[indexNum] >= 'A') && (src[indexNum] <= 'F')) { /* the number is 0-9 */
        *outPut = (src[indexNum] - 'A') + 0x0a;
    } else {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_SetLowFourBitHexString(VOS_UINT8 *outPut, VOS_UINT8 *src, VOS_UINT16 indexNum)
{
    if ((src[indexNum] >= '0') && (src[indexNum] <= '9')) { /* the number is 0-9 */
        *outPut |= src[indexNum] - '0';
    } else if ((src[indexNum] >= 'a') && (src[indexNum] <= 'f')) { /* the number is 0-9 */
        *outPut |= (src[indexNum] - 'a') + 0x0a;
    } else if ((src[indexNum] >= 'A') && (src[indexNum] <= 'F')) { /* the number is 0-9 */
        *outPut |= (src[indexNum] - 'A') + 0x0a;
    } else {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

TAF_UINT32 At_AsciiNum2HexString(TAF_UINT8 *src, TAF_UINT16 *srcLen)
{
    TAF_UINT16 chkLen     = 0;
    TAF_UINT16 tmp        = 0;
    TAF_UINT8  left       = 0;
    TAF_UINT16 srcLenTemp = *srcLen;
    TAF_UINT8 *dst        = src;

    /* ��������������ֽ��򷵻ش��� */
    if ((srcLenTemp % 2) != 0) {
        return AT_FAILURE;
    }

    while (chkLen < srcLenTemp) {
        if (AT_SetHighFourBitHexString(&left, src, chkLen) == AT_FAILURE) {
            return AT_FAILURE;
        }

        dst[tmp] = 0xf0 & (left << 4);
        chkLen += 1;

        if (AT_SetLowFourBitHexString(&dst[tmp], src, chkLen) == AT_FAILURE) {
            return AT_FAILURE;
        }

        chkLen += 1;
        tmp += 1;
    }

    *srcLen = srcLenTemp / 2; /* �����ֽ��� */
    return AT_SUCCESS;
}

TAF_UINT32 At_AsciiString2HexSimple(TAF_UINT8 *textStr, TAF_UINT8 *src, TAF_UINT16 srcLen)
{
    TAF_UINT16 chkLen = 0;
    TAF_UINT16 tmp    = 0;
    TAF_UINT8  left   = 0;
    TAF_UINT8 *dst    = textStr;

    if (srcLen == 0) {
        return AT_FAILURE;
    }

    while (chkLen < srcLen) {
        if (AT_SetHighFourBitHexString(&left, src, chkLen) == AT_FAILURE) {
            return AT_FAILURE;
        }

        dst[tmp] = 0xf0 & (left << 4);

        chkLen += 1;

        if (AT_SetLowFourBitHexString(&dst[tmp], src, chkLen) == AT_FAILURE) {
            return AT_FAILURE;
        }

        chkLen += 1;

        tmp += 1;
    }

    return AT_SUCCESS;
}

/* Del At_AbortCmdProc */

TAF_UINT32 AT_HexToAsciiString(TAF_UINT8 *dstStr, TAF_UINT32 dstLen, TAF_UINT8 *srcStr, TAF_UINT32 srcLen)
{
    TAF_UINT16 chkLen = 0;
    TAF_UINT8  high   = 0;
    TAF_UINT8  low    = 0;
    TAF_UINT8 *read   = srcStr;
    TAF_UINT8 *write  = dstStr;

    if ((srcStr == VOS_NULL_PTR) || (dstStr == VOS_NULL_PTR) || (srcLen == 0)) {
        return AT_ERROR;
    }

    /* ɨ�������ִ� */
    while (chkLen++ < srcLen) {
        high = 0x0F & (*read >> 4);
        low  = 0x0F & *read;

        if (high <= 0x09) {
            /* 0-9 */
            *write++ = high + 0x30;
        } else {
            /* A-F */
            *write++ = high + 0x37;
        }

        if (low <= 0x09) {
            *write++ = low + 0x30;
        } else {
            *write++ = low + 0x37;
        }

        /* ��һ���ַ� */
        read++;
    }
    *write = '\0';
    return AT_OK;
}

TAF_UINT32 At_AsciiNum2Num(TAF_UINT8 *dst, TAF_UINT8 *src, TAF_UINT16 srcLen)
{
    TAF_UINT16 chkLen = 0;

    for (chkLen = 0; chkLen < srcLen; chkLen++) {
        if ((src[chkLen] >= 0x30) && (src[chkLen] <= 0x39)) { /* the number is 0-9 */
            dst[chkLen] = src[chkLen] - 0x30;                 /* ��λ */
        } else {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}

TAF_UINT32 At_CheckDialString(TAF_UINT8 *data, TAF_UINT16 len)
{
    TAF_UINT16 count = 0;
    TAF_UINT8 *tmp   = data;

    if (*tmp == '+') {
        tmp++;
        count++;
    }

    while (count++ < len) {
        if ((*tmp >= '0') && (*tmp <= '9')) {
        } else if ((*tmp >= 'a') && (*tmp <= 'c')) {
        } else if ((*tmp >= 'A') && (*tmp <= 'C')) {
        } else if ((*tmp == '*') || (*tmp == '#')) {
        } else {
            return AT_FAILURE;
        }
        tmp++;
    }
    return AT_SUCCESS;
}

TAF_UINT32 At_UnicodePrint2Unicode(TAF_UINT8 *data, TAF_UINT16 *len)
{
    TAF_UINT8 *check   = data;
    TAF_UINT8 *write   = data;
    TAF_UINT8 *read    = data;
    TAF_UINT16 lenTemp = 0;
    TAF_UINT16 chkLen  = 0;
    TAF_UINT8  high    = 0;
    TAF_UINT8  low     = 0;

    /* �ַ���Ϊ'0'-'9','a'-'f','A'-'F' */
    while (chkLen++ < *len) {
        if ((*check >= '0') && (*check <= '9')) {
            *check = *check - '0';
        } else if ((*check >= 'a') && (*check <= 'f')) {
            *check = (*check - 'a') + 10; /* ʮ�����Ƹ�ʽת�� */
        } else if ((*check >= 'A') && (*check <= 'F')) {
            *check = (*check - 'A') + 10; /* ʮ�����Ƹ�ʽת�� */
        } else {
            return AT_FAILURE;
        }
        check++;
    }

    while (lenTemp < *len) {
        /* �жϽ�β */
        high = *read++; /* ��λ */
        lenTemp++;
        low = *read++; /* ��λ */
        lenTemp++;

        *write++ = (TAF_UINT8)(high * 16) + low; /* д��UNICODE��һ���ֽ� */
    }

    *len = lenTemp >> 1;
    return AT_SUCCESS;
}

VOS_UINT32 AT_CheckApnCustomChar(VOS_UINT8 apnCharacter, VOS_UINT16 clientId)
{
    AT_CommPsCtx *commPsCtx = VOS_NULL_PTR;
    VOS_UINT32    i;
    ModemIdUint16 modemId = MODEM_ID_0;

    commPsCtx = AT_GetCommPsCtxAddr();

    /* ��ȡmodem id */
    if (AT_GetModemIdFromClient(clientId, &modemId) != VOS_OK) {
        AT_WARN_LOG("AT_CheckApnCustomChar: Get ModemId Failed.");
        return VOS_ERR;
    }

    /* ƥ�䶨���ַ� */
    for (i = 0; i < commPsCtx->apnCustomFormatCfg[modemId].customCharNum; i++) {
        if (apnCharacter == commPsCtx->apnCustomFormatCfg[modemId].customChar[i]) {
            return VOS_OK;
        }
    }

    return VOS_ERR;
}

VOS_UINT32 AT_CheckApnFirstAndLastChar(VOS_UINT8 apnCharacter, VOS_UINT16 clientId)
{
    if (!(((apnCharacter >= 'A') && (apnCharacter <= 'Z')) || ((apnCharacter >= '0') && (apnCharacter <= '9')) ||
          (AT_CheckApnCustomChar(apnCharacter, clientId) == VOS_OK))) {
        AT_WARN_LOG("AT_CheckApnFirstAndLastChar: WARNING: Invalid begin/end character in APN.");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_CheckApnFirstAndLastString(VOS_UINT8 *apn, VOS_UINT16 apnLen, VOS_UINT16 clientId)
{
    AT_CommPsCtx *commPsCtx = VOS_NULL_PTR;
    VOS_UINT8     apnInfo[TAF_MAX_APN_LEN + 1];
    errno_t       memResult;
    ModemIdUint16 modemId = MODEM_ID_0;

    commPsCtx = AT_GetCommPsCtxAddr();
    (VOS_VOID)memset_s(apnInfo, sizeof(apnInfo), 0x00, sizeof(apnInfo));

    if (apnLen > sizeof(apnInfo)) {
        return VOS_ERR;
    }

    /* ��ȡmodem id */
    if (AT_GetModemIdFromClient(clientId, &modemId) != VOS_OK) {
        AT_WARN_LOG("AT_CheckApnFirstAndLastString: Get ModemId Failed.");
        return VOS_OK;
    }

    /* ����Ҫ���ֱ�ӷ���OK */
    if (commPsCtx->apnCustomFormatCfg[modemId].protocolStringCheckFlag == VOS_FALSE) {
        return VOS_OK;
    }

    /*
     * TS 23.003 9.1.1
     * The APN Network Identifier shall contain at least one label and shall have,
     * after encoding as defined in subclause 9.1 above, a maximum length of 63 octets.
     * An APN Network Identifier shall not start with any of the strings "rac", "lac",
     * "sgsn" or "rnc", and it shall not end in ".gprs", i.e. the last label of the
     * APN Network Identifier shall not be "gprs". Further, it shall not take the value "*".
     */

    if (apnLen > 0) {
        memResult = memcpy_s(apnInfo, sizeof(apnInfo), apn, apnLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(apnInfo), apnLen);
    }

    (VOS_VOID)At_UpString(apnInfo, apnLen);

    /* �ж�ͷ���ַ��� */
    if (!(TAF_MEM_CMP(apnInfo, "RAC", AT_PS_RAC_STRING_LENGTH) &&
          TAF_MEM_CMP(apnInfo, "LAC", AT_PS_LAC_STRING_LENGTH) &&
          TAF_MEM_CMP(apnInfo, "SGSN", AT_PS_SGSN_STRING_LENGTH) &&
          TAF_MEM_CMP(apnInfo, "RNC", AT_PS_RNC_STRING_LENGTH))) {
        return VOS_ERR;
    }

    /* �ж�β���ַ��� */
    if (apnLen > AT_PS_GRPS_STRING_LENGTH) {
        if (!TAF_MEM_CMP(apnInfo + apnLen - AT_PS_GRPS_STRING_LENGTH, "GPRS", AT_PS_GRPS_STRING_LENGTH)) {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

VOS_UINT32 AT_CheckApnFormat(VOS_UINT8 *apn, VOS_UINT16 apnLen, VOS_UINT16 clientId)
{
    VOS_UINT32 i;
    VOS_UINT8  apnCharacter;

    for (i = 0; i < apnLen; i++) {
        /* ת��APN�ַ�Ϊ��д */
        apnCharacter = AT_UPCASE(apn[i]);

        /* ���APN�ַ���Ч�� */
        if (!(((apnCharacter >= 'A') && (apnCharacter <= 'Z')) || ((apnCharacter >= '0') && (apnCharacter <= '9')) ||
              (apnCharacter == '-') || (apnCharacter == '.') ||
              (AT_CheckApnCustomChar(apnCharacter, clientId) == VOS_OK))) {
            AT_WARN_LOG("AT_CheckApnFormat: Invalid character in APN.");
            return VOS_ERR;
        }
    }

    /* ���APN��ʼ�ַ���Ч�� */
    apnCharacter = AT_UPCASE(apn[0]);
    if (AT_CheckApnFirstAndLastChar(apnCharacter, clientId) != VOS_OK) {
        AT_WARN_LOG("AT_CheckApnFormat: Invalid begin/end character in APN.");
        return VOS_ERR;
    }

    /* ���APNĩβ�ַ���Ч�� */
    apnCharacter = AT_UPCASE(apn[apnLen - 1]);
    if (AT_CheckApnFirstAndLastChar(apnCharacter, clientId) != VOS_OK) {
        AT_WARN_LOG("AT_CheckApnFormat: Invalid begin/end character in APN.");
        return VOS_ERR;
    }

    /* ���APN��ͷĩβ�Ƿ�������������ַ��� */
    if (AT_CheckApnFirstAndLastString(apn, apnLen, clientId) != VOS_OK) {
        AT_WARN_LOG("AT_CheckApnFormat: UnexpectString in APN.");
        return VOS_ERR;
    }

    return VOS_OK;
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)

LOCAL VOS_UINT32 AT_CheckSNssaiFormat(VOS_UINT8 *sNssai, VOS_UINT16 sNssaiLen)
{
    VOS_UINT32 i;
    VOS_UINT8  character;

    for (i = 0; i < sNssaiLen; i++) {
        character = AT_UPCASE(sNssai[i]);

        /* ���S-NSSAI�ַ���Ч�� */
        if (!(((character >= '0') && (character <= '9')) || ((character >= 'A') && (character <= 'F')) ||
              (character == ';') || (character == '.'))) {
            AT_WARN_LOG("AT_CheckSNssaiFormat: WARNING: Invalid character in S-NSSAI.");
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

LOCAL VOS_UINT32 AT_DecodeSNssaiPara(VOS_UINT8 *sNssai, VOS_UINT16 sNssaiLen, AT_SNssaiParse *sNssaiParse)
{
    VOS_UINT32 i;
    VOS_UINT32 digitCount = 0;
    VOS_UINT8  character;

    for (i = 0; i < sNssaiLen; i++) {
        character = AT_UPCASE(sNssai[i]);

        if ((character >= '0') && (character <= '9')) {
            /* ��λ��ֵ����4bit */
            sNssaiParse->digite[sNssaiParse->digitParaNum] =
                (VOS_UINT32)(sNssaiParse->digite[sNssaiParse->digitParaNum] << AT_OCTET_MOVE_FOUR_BITS);
            sNssaiParse->digite[sNssaiParse->digitParaNum] += (VOS_UINT32)(sNssai[i] - '0');

            digitCount++;
            if (digitCount > AT_S_NSSAI_SD_MAX_DIGIT_NUM) {
                /* ÿ��������λ����಻����6 */
                return VOS_ERR;
            }
        } else if ((character >= 'A') && (character <= 'F')) {
            /* ��λ��ֵ����4bit */
            sNssaiParse->digite[sNssaiParse->digitParaNum] =
                (VOS_UINT32)(sNssaiParse->digite[sNssaiParse->digitParaNum] << AT_OCTET_MOVE_FOUR_BITS);
            sNssaiParse->digite[sNssaiParse->digitParaNum] += (VOS_UINT32)((character - 'A') + 0x0A);

            digitCount++;
            if (digitCount > AT_S_NSSAI_SD_MAX_DIGIT_NUM) {
                /* ÿ��������λ����಻����6 */
                return VOS_ERR;
            }
        } else {
            if (digitCount == 0) {
                /* �ַ�������Է��ſ�ͷ�������ַ����г��������ķ��ţ���������� */
                return VOS_ERR;
            }

            if ((i + 1) == sNssaiLen) {
                /* ������1���ַ��Ƿ��ţ�˵����1�����֣���Ҫ���ش��� */
                return VOS_ERR;
            }

            /* ��ֵ�ַ�������0 */
            digitCount = 0;

            /* ��¼�ַ� */
            sNssaiParse->punctuation[sNssaiParse->punctuationNum] = sNssai[i];

            /* ׼����¼��һ�����ֻ����ַ� */
            sNssaiParse->digitParaNum++;
            sNssaiParse->punctuationNum++;

            if ((sNssaiParse->digitParaNum > AT_S_NSSAI_MAX_DIGIT_NUM) ||
                (sNssaiParse->punctuationNum > AT_S_NSSAI_MAX_PUNC_NUM)) {
                return VOS_ERR;
            }
        }
    }

    /* ��¼���һ�����ֺ󣬼�����1 */
    sNssaiParse->digitParaNum++;

    return VOS_OK;
}

LOCAL VOS_UINT32 AT_CheckSNssaiPunctuationPara(AT_SNssaiParse *sNssaiParse)
{
    /*
     * sst                                     only slice/service type (SST) is present
     * sst;mapped_sst                          SST and mapped configured SST are present
     * sst.sd                                  SST and slice differentiator (SD) are present
     * sst.sd;mapped_sst                       SST, SD and mapped configured SST are present
     * sst.sd;mapped_sst.mapped_sd             SST, SD, mapped configured SST and mapped configured SD are present
     */

    VOS_UINT8 puncMould[AT_S_NSSAI_MAX_PUNC_NUM + 1] = { '.', ';', '.', 0 };
    VOS_UINT8 num;

    if (sNssaiParse->punctuationNum == 0) {
        return VOS_OK;
    }

    num = AT_MIN(sNssaiParse->punctuationNum, AT_S_NSSAI_MAX_PUNC_NUM);

    if ((num == 0x01) && (sNssaiParse->punctuation[0] == ';')) {
        /* ������ǺϷ��� */
        return VOS_OK;
    }

    if (PS_MEM_CMP(puncMould, sNssaiParse->punctuation, num) != 0) {
        return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL VOS_UINT32 AT_CheckSNssaiDigitPara(AT_SNssaiParse *sNssaiParse)
{
    /* ��������Ҫ���SST��MAP SST��ȡֵ��Χ��SD�ڽ���ʱ���˼�� */

    /* ����һ����ֵ��Ӧ��ΪSST */
    if (sNssaiParse->digite[0] > 0xFF) {
        return VOS_ERR;
    }

    if (sNssaiParse->punctuationNum == 1) {
        /* ���ΪSST;MAP SST�ĸ�ʽ�����MAP SST�Ĳ���ֵ */
        if (sNssaiParse->punctuation[0] == ';') {
            if (sNssaiParse->digite[1] > 0xFF) {
                return VOS_ERR;
            }
        }
    }

    if ((sNssaiParse->punctuationNum == AT_S_NSSAI_PUNC_NUM) ||
        (sNssaiParse->punctuationNum == AT_S_NSSAI_MAX_PUNC_NUM)) {
        /* ���ΪSST.SD;map_SST����SST.map_SD;map_SST.map_SD�ĸ�ʽ�����MAP SST�Ĳ���ֵ */
        if (sNssaiParse->digite[AT_S_NSSAI_DIGIT_INDEX_2] > 0xFF) {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

VOS_UINT32 AT_DecodeSNssai(VOS_UINT8 *sNssai, VOS_UINT16 sNssaiLen, PS_S_NSSAI_STRU *sNssaiOut)
{
    AT_SNssaiParse sNssaiInfo;

    /*
     * 27007 rel15, 10.1.1�½�
     * sst                                     only slice/service type (SST) is present
     * sst;mapped_sst                          SST and mapped configured SST are present
     * sst.sd                                  SST and slice differentiator (SD) are present
     * sst.sd;mapped_sst                       SST, SD and mapped configured SST are present
     * sst.sd;mapped_sst.mapped_sd             SST, SD, mapped configured SST and mapped configured SD are present
     */

    (VOS_VOID)memset_s(sNssaiOut, sizeof(PS_S_NSSAI_STRU), 0, sizeof(PS_S_NSSAI_STRU));
    (VOS_VOID)memset_s(&sNssaiInfo, sizeof(sNssaiInfo), 0, sizeof(AT_SNssaiParse));

    /* ���S-NSSAI�ַ����е��ַ���ʽ�Ƿ���ȷ  */
    if (AT_CheckSNssaiFormat(sNssai, sNssaiLen) == VOS_ERR) {
        return VOS_ERR;
    }

    /* ����S-NSSAI�ַ��� */
    if (AT_DecodeSNssaiPara(sNssai, sNssaiLen, &sNssaiInfo) == VOS_ERR) {
        return VOS_ERR;
    }

    /* �ȼ��ָ���Ƿ���ȷ */
    if (AT_CheckSNssaiPunctuationPara(&sNssaiInfo) == VOS_ERR) {
        return VOS_ERR;
    }

    /* �ټ����ֵ�Ƿ���ȷ */
    if (AT_CheckSNssaiDigitPara(&sNssaiInfo) == VOS_ERR) {
        return VOS_ERR;
    }

    /* ��ֵ */
    if (sNssaiInfo.digitParaNum == 1) {
        /* SST */
        sNssaiOut->ucSst = (VOS_UINT8)sNssaiInfo.digite[AT_S_NSSAI_DIGITE_INDEX_0];
    } else if (sNssaiInfo.digitParaNum == 2) { /* 2��ʾS-NSSAI�ַ����������ŷָ�Ϊ������ */
        /* SST.SD����SST;map_SST */
        sNssaiOut->ucSst = (VOS_UINT8)sNssaiInfo.digite[AT_S_NSSAI_DIGITE_INDEX_0];

        /* ��������ɹ����жϺ����ַ���"."����";" */
        if (sNssaiInfo.punctuation[0] == '.') {
            sNssaiOut->bitOpSd = VOS_TRUE;
            sNssaiOut->ulSd    = sNssaiInfo.digite[AT_S_NSSAI_DIGITE_INDEX_1];
        } else {
            sNssaiOut->bitOpMappedSst = VOS_TRUE;
            sNssaiOut->ucMappedSst    = (VOS_UINT8)sNssaiInfo.digite[AT_S_NSSAI_DIGITE_INDEX_1];
        }
    } else if (sNssaiInfo.digitParaNum == 3) { /* 3��ʾS-NSSAI�ַ����������ŷָ�Ϊ3���� */
        /* SST.SD;map_SST */
        sNssaiOut->ucSst          = (VOS_UINT8)sNssaiInfo.digite[AT_S_NSSAI_DIGITE_INDEX_0];
        sNssaiOut->bitOpSd        = VOS_TRUE;
        sNssaiOut->ulSd           = sNssaiInfo.digite[AT_S_NSSAI_DIGITE_INDEX_1];
        sNssaiOut->bitOpMappedSst = VOS_TRUE;
        sNssaiOut->ucMappedSst    = (VOS_UINT8)sNssaiInfo.digite[AT_S_NSSAI_DIGIT_INDEX_2];
    } else {
        /* SST.SD;map_SST.map_SD */
        sNssaiOut->ucSst          = (VOS_UINT8)sNssaiInfo.digite[AT_S_NSSAI_DIGITE_INDEX_0];
        sNssaiOut->bitOpSd        = VOS_TRUE;
        sNssaiOut->ulSd           = sNssaiInfo.digite[AT_S_NSSAI_DIGITE_INDEX_1];
        sNssaiOut->bitOpMappedSst = VOS_TRUE;
        sNssaiOut->ucMappedSst    = (VOS_UINT8)sNssaiInfo.digite[AT_S_NSSAI_DIGIT_INDEX_2];
        sNssaiOut->bitOpMappedSd  = VOS_TRUE;
        sNssaiOut->ulMappedSd     = sNssaiInfo.digite[AT_S_NSSAI_DIGIT_INDEX_3];
    }

    /* SDֵ�����ȫF���ǷǷ��ģ���Ҫ��OP������ΪFalse */
    if (sNssaiOut->ulSd == 0xffffff) {
        sNssaiOut->bitOpSd = VOS_FALSE;
        sNssaiOut->ulSd    = 0;
    }

    /* SDֵ�����ȫF���ǷǷ��ģ���Ҫ��OP������ΪFalse */
    if (sNssaiOut->ulMappedSd == 0xffffff) {
        sNssaiOut->bitOpMappedSd = VOS_FALSE;
        sNssaiOut->ulMappedSd    = 0;
    }

    return VOS_OK;
}
#endif

VOS_VOID At_SetCopsActPara(TAF_MMA_RatTypeUint8 *phRat, VOS_UINT16 paraLen, VOS_UINT32 paraValue)
{
    /* �����������Ϊ0��ʹ����Чֵ */
    if (paraLen == 0) {
        *phRat = TAF_MMA_RAT_BUTT;

        return;
    }

    /* �����û�����Ĳ���ֵ,ת��ΪAT��MMA�Ľӿڶ���Ľ��뼼������ */
    if (paraValue == AT_COPS_RAT_GSM) {
        *phRat = TAF_MMA_RAT_GSM;
    } else if (paraValue == AT_COPS_RAT_WCDMA) {
        *phRat = TAF_MMA_RAT_WCDMA;
    }
#if (FEATURE_LTE == FEATURE_ON)
    else if (paraValue == AT_COPS_RAT_LTE) {
        *phRat = TAF_MMA_RAT_LTE;
    }
#endif
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    else if (paraValue == AT_COPS_RAT_NR) {
        *phRat = TAF_MMA_RAT_NR;
    }
#endif
    else {
        *phRat = TAF_MMA_RAT_BUTT;
    }
}

VOS_UINT32 AT_DigitString2Hex(VOS_UINT8 *digitStr, VOS_UINT16 len, VOS_UINT32 *destHex)
{
    VOS_UINT32 currValue = 0;
    VOS_UINT32 totalVaue = 0;
    VOS_UINT8  length = 0;

    currValue = (VOS_UINT32)*digitStr++;

    while (length++ < len) {
        if ((currValue >= '0') && (currValue <= '9')) {
            currValue = currValue - '0';
        } else {
            return VOS_FALSE;
        }

        if (totalVaue > 0x0FFFFFFF) { /* ������ת */
            return VOS_FALSE;
        } else {
            totalVaue = (totalVaue << AT_OCTET_MOVE_FOUR_BITS) + currValue; /* accumulate digit */
            currValue = (VOS_UINT32)(VOS_UINT8)*digitStr++;                 /* get next Char */
        }
    }

    *destHex = totalVaue;
    return VOS_TRUE;
}

VOS_UINT32 AT_FillSsBsService(TAF_SS_ErasessReq *sSPara, AT_ParseParaType *atPara)
{
    if ((atPara->paraLen == 0) || (atPara->paraValue == AT_CLCK_PARA_CLASS_ALL)) {
        /* Ĭ��ֵ */
        sSPara->opBsService = 0;
    } else {
        if (At_SsPrint2Class(&(sSPara->bsService), (VOS_UINT8)atPara->paraValue) == AT_FAILURE) {
            return VOS_ERR;
        }
        sSPara->opBsService = 1;
    }

    return VOS_OK;
}
AT_MTA_PersCategoryUint8 AT_GetSimlockUnlockCategoryFromClck(VOS_VOID)
{
    switch (g_atParaList[0].paraValue) {
        case AT_CLCK_PN_TYPE:
            return AT_MTA_PERS_CATEGORY_NETWORK;
        case AT_CLCK_PU_TYPE:
            return AT_MTA_PERS_CATEGORY_NETWORK_SUBSET;
        case AT_CLCK_PP_TYPE:
            return AT_MTA_PERS_CATEGORY_SERVICE_PROVIDER;
        default:
            return AT_MTA_PERS_CATEGORY_BUTT;
    }
}

VOS_UINT32 AT_FillCalledNumPara(VOS_UINT8 *atPara, VOS_UINT16 len, MN_CALL_CalledNum *calledNum)
{
    VOS_UINT32 relt;

    /* �������ͼ�� */
    if (*atPara == '+') {
        /* ���ʺ���ĵ�һ���ַ�����Ҫ����,������Ϊ81���ַ� */
        if (len > (MN_CALL_MAX_CALLED_ASCII_NUM_LEN + 1)) {
            return VOS_ERR;
        }

        *(atPara + len) = '\0';
        atPara += 1;

        calledNum->numType = (MN_CALL_IS_EXIT | (MN_CALL_TON_INTERNATIONAL << 4) | MN_CALL_NPI_ISDN);
    } else {
        /* ���ں����δ֪����������Ϊ80���ַ� */
        if (len > MN_CALL_MAX_CALLED_ASCII_NUM_LEN) {
            return VOS_ERR;
        }

        *(atPara + len) = '\0';

        calledNum->numType = (MN_CALL_IS_EXIT | (MN_CALL_TON_UNKNOWN << 4) | MN_CALL_NPI_ISDN);
    }

    relt = AT_AsciiNumberToBcd((VOS_CHAR *)atPara, calledNum->bcdNum, &calledNum->numLen);

    if (relt != MN_ERR_NO_ERROR) {
        return VOS_ERR;
    }

    AT_JudgeIsPlusSignInDialString((VOS_CHAR *)atPara, len + 1, &calledNum->isExistPlusSign, &calledNum->plusSignLocation);

    return VOS_OK;
}

VOS_VOID AT_SetDefaultRatPrioList(ModemIdUint16 modemId, TAF_MMA_MultimodeRatCfg *sysCfgRatOrder,
                                  VOS_UINT8 userSptLteFlag, VOS_UINT8 userSptNrFlag)
{
    VOS_UINT8 userRatNum = 0;

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /* ��ǰ�����Ƿ�֧��NR */
    if (userSptNrFlag == VOS_TRUE) {
        /* ƽ̨֧��NR */
        if (AT_IsModemSupportRat(modemId, TAF_MMA_RAT_NR) == VOS_TRUE) {
            sysCfgRatOrder->ratOrder[userRatNum] = TAF_MMA_RAT_NR;

            userRatNum++;
        }
    }
#endif

#if (FEATURE_LTE == FEATURE_ON)
    /* ��ǰ�����Ƿ�֧��LTE */
    if (userSptLteFlag == VOS_TRUE) {
        /* ƽ̨֧��LTE */
        if (AT_IsModemSupportRat(modemId, TAF_MMA_RAT_LTE) == VOS_TRUE) {
            sysCfgRatOrder->ratOrder[userRatNum] = TAF_MMA_RAT_LTE;

            userRatNum++;
        }
    }
#endif

    /* ƽ̨֧��WCDMA */
    if (AT_IsModemSupportRat(modemId, TAF_MMA_RAT_WCDMA) == VOS_TRUE) {
        sysCfgRatOrder->ratOrder[userRatNum] = TAF_MMA_RAT_WCDMA;

        userRatNum++;
    }

    /* ƽ̨֧��GSM */
    if (AT_IsModemSupportRat(modemId, TAF_MMA_RAT_GSM) == VOS_TRUE) {
        sysCfgRatOrder->ratOrder[userRatNum] = TAF_MMA_RAT_GSM;

        userRatNum++;
    }

    sysCfgRatOrder->ratNum = userRatNum;
}

TAF_UINT32 At_SetParaCmd(TAF_UINT8 indexNum)
{
    AT_RreturnCodeUint32 result = AT_FAILURE;

    /*
     * vts������������Ҫע�⣬���ڲ������еĲ�������ΪTAF_NULL_PTR����ʵ�����ò���ʱ��ȡ�����õĲ���
     * �����������޲��������Ҫ�жϣ������Ч����������Ϊ0���򷵻ش���
     */

    if (g_parseContext[indexNum].cmdElement->setProc != TAF_NULL_PTR) {
        result = (AT_RreturnCodeUint32)g_parseContext[indexNum].cmdElement->setProc(indexNum);

        if (result == AT_WAIT_ASYNC_RETURN) {
            g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_PEND;

            /* ����ʱ�� */
            if (At_StartTimer(g_parseContext[indexNum].cmdElement->setTimeOut, indexNum) != AT_SUCCESS) {
                AT_ERR_LOG("At_SetParaCmd:ERROR:Start Timer");
                return AT_ERROR;
            }
        }
        return result;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_PhyNumIsNull(ModemIdUint16 modemId, AT_PhynumTypeUint32 setType, VOS_BOOL *pbPhyNumIsNull)
{
    VOS_UINT32   ret;
    VOS_UINT32   loop;
    VOS_UINT32   totalNum;
    IMEI_STRU    imei;
    TAF_SVN_Data svnData;
    VOS_UINT8   *checkNum = VOS_NULL_PTR;
    VOS_UINT8    mac[AT_PHYNUM_MAC_LEN + 1]; /* MAC��ַ */

    (VOS_VOID)memset_s(&svnData, sizeof(svnData), 0x00, sizeof(svnData));

    /* �ӻ�ȡNV��IMEI��SVN����ţ���ȡ����ʧ�ܣ��򷵻�AT_ERROR */
    if (setType == AT_PHYNUM_TYPE_IMEI) {
        ret = TAF_ACORE_NV_READ(modemId, en_NV_Item_IMEI, &imei, NV_ITEM_IMEI_SIZE);
        if (ret != NV_OK) {
            AT_WARN_LOG("AT_PhyNumIsNull: Fail to read en_NV_Item_IMEI");
            return AT_ERROR;
        }

        /* NV��en_NV_Item_IMEI�н�15λ��UE��IMEI�ţ����һλ����Чλ�����Ƚ� */
        totalNum = TAF_IMEI_DATA_LENGTH;
        checkNum = imei.aucImei;
    }
    else if (setType == AT_PHYNUM_TYPE_SVN) {
        ret = TAF_ACORE_NV_READ(modemId, NV_ITEM_IMEI_SVN, &svnData, sizeof(svnData));
        if (ret != NV_OK) {
            AT_WARN_LOG("AT_PhyNumIsNull: Fail to read en_Nv_Item_Imei_Svn");
            return AT_ERROR;
        }

        /* SVNδ��������Ϊ�մ��� */
        if (svnData.activeFlag != NV_ITEM_ACTIVE) {
            *pbPhyNumIsNull = VOS_TRUE;
            return AT_OK;
        }

        totalNum = TAF_SVN_DATA_LENGTH;
        checkNum = svnData.svn;
    } else if (setType == AT_PHYNUM_TYPE_MAC) {
        /* �����ǰMAC��ַ�Ƿ�Ϊ��״̬ */
        AT_GetPhynumMac(mac, AT_PHYNUM_MAC_LEN + 1);
        totalNum = AT_PHYNUM_MAC_LEN;
        checkNum = mac;
    } else {
        AT_WARN_LOG("AT_PhyNumIsNull: the type of physical is error.");
        return AT_ERROR;
    }

    /* NV��IMEI��SVN�����Ϊȫ0��Ϊ�����Ϊ�գ����򣬷ǿ� */
    for (loop = 0; loop < totalNum; loop++) {
        if (*checkNum != 0) {
            *pbPhyNumIsNull = VOS_FALSE;
            return AT_OK;
        }

        checkNum++;
    }

    *pbPhyNumIsNull = VOS_TRUE;

    return AT_OK;
}

VOS_UINT32 AT_OpenSpecificPort(VOS_UINT8 port)
{
    AT_DynamicPidType dynamicPidType;
    VOS_UINT32        portPos;
    VOS_UINT32        portNum;

    (VOS_VOID)memset_s(&dynamicPidType, sizeof(dynamicPidType), 0x00, sizeof(dynamicPidType));

    portPos = AT_DEV_NONE;

    /* ��NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE��ȡ��ǰ�Ķ˿�״̬ */
    if (NV_OK !=
        TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, &dynamicPidType, sizeof(AT_DynamicPidType))) {
        AT_WARN_LOG("AT_OpenSpecificPort: Read NV fail!");
        return AT_ERROR;
    }

    /* �ж϶˿��Ƿ��Ѿ���: �Ѿ�����ֱ�ӷ���AT_OK */
    if (dynamicPidType.nvStatus == VOS_TRUE) {
        /* ��ѯNV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE���Ƿ��Ѿ����ڸö˿� */
        AT_GetSpecificPort(port, dynamicPidType.rewindPortStyle, &portPos, &portNum);

        if (portPos != AT_DEV_NONE) {
            return AT_OK;
        }

    } else {
        AT_WARN_LOG("AT_OpenSpecificPort: NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE is inactive!");
        return AT_ERROR;
    }

    /* ��ǰ���������Ӷ˿� */
    if (portNum >= AT_SETPORT_PARA_MAX_LEN) {
        return AT_ERROR;
    }

    dynamicPidType.rewindPortStyle[portNum] = port;

    /* ���¶˿ڼ������ݵ�NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, (VOS_UINT8 *)&dynamicPidType,
                           sizeof(AT_DynamicPidType)) != NV_OK) {
        AT_ERR_LOG("AT_OpenSpecificPort: Write NV fail");
        return AT_ERROR;
    } else {
        return AT_OK;
    }
}

VOS_UINT32 AT_CloseSpecificPort(VOS_UINT8 port)
{
    AT_DynamicPidType dynamicPidType;
    VOS_UINT32        portPos;
    VOS_UINT32        portNum;
    VOS_UINT32        loop;

    (VOS_VOID)memset_s(&dynamicPidType, sizeof(dynamicPidType), 0x00, sizeof(dynamicPidType));

    portPos = AT_DEV_NONE;

    /* ��NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE��ȡ��ǰ�Ķ˿�״̬ */
    if (NV_OK !=
        TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, &dynamicPidType, sizeof(AT_DynamicPidType))) {
        AT_ERR_LOG("AT_CloseSpecificPort: Read NV fail!");
        return AT_ERROR;
    }

    /* �жϸö˿��Ƿ��Ѿ��ر�: �Ѿ��ر���ֱ�ӷ���AT_OK */
    if (dynamicPidType.nvStatus == VOS_TRUE) {
        /* ��ѯNV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE���Ƿ��Ѿ����ڸö˿� */
        AT_GetSpecificPort(port, dynamicPidType.rewindPortStyle, &portPos, &portNum);

        if (portPos == AT_DEV_NONE) {
            return AT_OK;
        }
    } else {
        return AT_ERROR;
    }

    /* ɾ��NV���е�ָ���˿� */
    dynamicPidType.rewindPortStyle[portPos] = 0;
    portNum--;

    for (loop = portPos; loop < portNum; loop++) {
        dynamicPidType.rewindPortStyle[loop] = dynamicPidType.rewindPortStyle[loop + 1UL];
    }

    dynamicPidType.rewindPortStyle[portNum] = 0;

    /* �˿��쳣���ݱ���: �л�����豸��̬�У���һ���豸����ΪMASS�豸(0xa1,0xa2) */
    if (portNum != 0) {
        if ((dynamicPidType.rewindPortStyle[0] == AT_DEV_CDROM) || (dynamicPidType.rewindPortStyle[0] == AT_DEV_SD)) {
            return AT_ERROR;
        }
    }

    /* ���¶˿ڼ������ݵ�NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, (VOS_UINT8 *)&dynamicPidType,
                           sizeof(AT_DynamicPidType)) != NV_OK) {
        AT_ERR_LOG("AT_CloseSpecificPort: Write NV fail");
        return AT_ERROR;
    } else {
        return AT_OK;
    }
}

VOS_UINT32 At_GetParaCnt(VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT32 i;
    VOS_UINT32 cnt = 0;

    for (i = 0; i < len; i++) {
        if (data[i] == ',') {
            ++cnt;
        }
    }

    /* �������� = ','����+1 */
    return (cnt + 1);
}

VOS_UINT32 At_AsciiNum2SimLockImsiStr(VOS_UINT8 *dst, VOS_UINT8 *src, VOS_UINT16 srcLen)
{
    VOS_UINT16 chkLen;
    VOS_UINT8  tmp;
    VOS_UINT8  bcdCode;

    /* �ַ����������Ϊ16 */
    if (srcLen > (TAF_PH_SIMLOCK_PLMN_STR_LEN * 2)) {
        return AT_FAILURE;
    }

    for (chkLen = 0; chkLen < srcLen; chkLen++) {
        /* the number is 0-9 */
        if ((src[chkLen] >= 0x30) && (src[chkLen] <= 0x39)) {
            bcdCode = src[chkLen] - 0x30;
        } else {
            return AT_FAILURE;
        }

        tmp = chkLen % 2; /* �жϸߵ�λ */
        if (tmp == 0) {
            dst[chkLen / 2] = (VOS_UINT8)((bcdCode << 4) & 0xF0); /* ��λ */
        } else {
            dst[chkLen / 2] |= (VOS_UINT8)(bcdCode); /* ��λ */
        }
    }
    /* �ж�srcLen�����Ƿ���2n+1 */
    if ((srcLen % 2) == 1) {
        dst[srcLen / 2] |= 0x0F; /* ��λ */
    }

    return AT_SUCCESS;
}

VOS_UINT32 At_CovertAtParaToSimlockPlmnInfo(VOS_UINT32 paraCnt, AT_ParseParaType *paralist,
                                            TAF_CUSTOM_SimLockPlmnInfo *simLockPlmnInfo)
{
    VOS_UINT8  mncNum = AT_MNC_MIN_LEN;
    VOS_UINT8  imsiStr[TAF_PH_SIMLOCK_PLMN_STR_LEN] = { 0 };
    VOS_UINT8 *plmnRange = VOS_NULL_PTR;
    VOS_UINT32 i;
    VOS_UINT32 plmnParaIdx = 0;
    VOS_UINT32 imsiStrLen;
    errno_t    memResult;
    VOS_UINT32 mncParaInvalidFlg;

    /* Plmn�Ŷ���Ϣ���ӵ�2��������ʼ��ÿ3��Ϊһ�飬��Ӧ(MNClen,PlmnRangeBegin,PlmnRangeEnd) */
    for (i = 1; i < paraCnt; i++) {
        /* ��Plmninfo����index��3n+1ʱ,��ӦMNC�ĳ��� */
        if ((i % 3) == 1) {
            mncParaInvalidFlg = (paralist[i].paraLen != 1) ||
                                ((paralist[i].para[0] != '2') && (paralist[i].para[0] != '3'));

            if (mncParaInvalidFlg == VOS_TRUE) {
                return AT_SIMLOCK_PLMN_MNC_LEN_ERR;
            } else {
                mncNum                                                = paralist[i].para[0] - '0';
                simLockPlmnInfo->simLockPlmnRange[plmnParaIdx].mncNum = mncNum;
            }

        } else if ((i % 3) == 2) {
            /* ��Plmninfo����index��3n+2 ʱ,��ӦPlmnRangeBegin */
            if (At_AsciiNum2SimLockImsiStr(imsiStr, paralist[i].para, paralist[i].paraLen) == AT_FAILURE) {
                return AT_ERROR;
            } else {
                imsiStrLen = (((VOS_UINT32)paralist[i].paraLen + 1) / 2);
                plmnRange  = simLockPlmnInfo->simLockPlmnRange[plmnParaIdx].rangeBegin;
                if (imsiStrLen > 0) {
                    memResult = memcpy_s(plmnRange, TAF_PH_SIMLOCK_PLMN_STR_LEN, imsiStr, (VOS_UINT16)imsiStrLen);
                    TAF_MEM_CHK_RTN_VAL(memResult, TAF_PH_SIMLOCK_PLMN_STR_LEN, (VOS_UINT16)imsiStrLen);
                }
                (VOS_VOID)memset_s(plmnRange + imsiStrLen, TAF_PH_SIMLOCK_PLMN_STR_LEN - imsiStrLen, 0xFF,
                         (TAF_PH_SIMLOCK_PLMN_STR_LEN - imsiStrLen));
            }
        } else {
            /* ��Plmninfo����index��3n+2 ʱ,��ӦPlmnRangeEnd */
            if (At_AsciiNum2SimLockImsiStr(imsiStr, paralist[i].para, paralist[i].paraLen) == AT_FAILURE) {
                return AT_ERROR;
            }
            /* ��ʼ�ͽ����Ŷγ��Ȳ�һ�� ����ʼ�Ŷδ��ڽ����Ŷ� ֱ�ӷ���ʧ�� */
            else if ((paralist[i - 1].paraLen != paralist[i].paraLen) ||
                     (AT_AtoI((VOS_CHAR *)paralist[i - 1].para, (VOS_UINT32)paralist[i - 1].paraLen) >
                        AT_AtoI((VOS_CHAR *)paralist[i].para, (VOS_UINT32)paralist[i].paraLen))) {
                AT_ERR_LOG("At_CovertAtParaToSimlockPlmnInfo: AT_CME_INCORRECT_PARAMETERS!");
                return AT_CME_INCORRECT_PARAMETERS;
            } else {
                imsiStrLen = (((VOS_UINT32)paralist[i].paraLen + 1) / 2);
                plmnRange  = simLockPlmnInfo->simLockPlmnRange[plmnParaIdx].rangeEnd;
                if (imsiStrLen > 0) {
                    memResult = memcpy_s(plmnRange, TAF_PH_SIMLOCK_PLMN_STR_LEN, imsiStr, (VOS_UINT16)imsiStrLen);
                    TAF_MEM_CHK_RTN_VAL(memResult, TAF_PH_SIMLOCK_PLMN_STR_LEN, (VOS_UINT16)imsiStrLen);
                }
                (VOS_VOID)memset_s(plmnRange + imsiStrLen, TAF_PH_SIMLOCK_PLMN_STR_LEN - imsiStrLen, 0xFF,
                         (TAF_PH_SIMLOCK_PLMN_STR_LEN - imsiStrLen));
            }
            ++plmnParaIdx;
        }
    }

    return AT_OK;
}

VOS_UINT32 At_SetSimLockPlmnInfo(VOS_UINT8 indexNum, VOS_UINT32 paraCnt, AT_ParseParaType *paralist)
{
    VOS_UINT32                 rslt;
    TAF_CUSTOM_SimLockPlmnInfo simLockPlmnInfo;
    TAF_CUSTOM_CardlockStatus  cardLockStatus = {0};
    VOS_UINT32                 i;
    VOS_UINT32                 plmnInfoNum;
    /* TAF_CUSTOM_SimLockPlmnRange stDftPlmnInfoVal; */

    if (g_atDataLocked == VOS_TRUE) {
        rslt = AT_DATA_UNLOCK_ERROR;
    } else if (paraCnt > ((TAF_MAX_SIM_LOCK_RANGE_NUM * 3) + 1)) {
        rslt = AT_SIMLOCK_PLMN_NUM_ERR;
        /* �ж�ulParaCnt�����Ƿ���3n+1 */
    } else if (((paraCnt - 1) % 3) != 0) {
        rslt = AT_ERROR;
    } else {
        rslt = At_CovertAtParaToSimlockPlmnInfo(paraCnt, paralist, &simLockPlmnInfo);
    }

    if (rslt != AT_OK) {
        At_FormatResultData(indexNum, rslt);
        return AT_ERROR;
    }

    /* ���PLMN �Ŷ�(MNClen,PlmnRangeBegin,PlmnRangeEnd)�ĸ��� ����2��������ʼ��Plmn ��Ϣ */
    plmnInfoNum = (paraCnt - 1) / 3;

    /* ��ʣ���PLMN�Ŷ�����ΪĬ��ֵ */
    for (i = plmnInfoNum; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++) {
        (VOS_VOID)memset_s(simLockPlmnInfo.simLockPlmnRange[i].rangeBegin, TAF_PH_SIMLOCK_PLMN_STR_LEN,
                 AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL, TAF_PH_SIMLOCK_PLMN_STR_LEN);
        (VOS_VOID)memset_s(simLockPlmnInfo.simLockPlmnRange[i].rangeEnd, TAF_PH_SIMLOCK_PLMN_STR_LEN,
                 AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL, TAF_PH_SIMLOCK_PLMN_STR_LEN);
        simLockPlmnInfo.simLockPlmnRange[i].mncNum = AT_SIM_LOCK_MNC_NUM_DEFAULT_VAL;
    }

    simLockPlmnInfo.status = NV_ITEM_ACTIVE;

    /* ��ȡ��ǰ����״̬ */
    rslt = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CARDLOCK_STATUS, &cardLockStatus, sizeof(cardLockStatus));

    if (rslt != NV_OK) {
        AT_WARN_LOG("At_SetSimLockPlmnInfo:read NV_ITEM_CARDLOCK_STATUS Fail.");
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_ERROR;
    }

    /* �����ǰ�Ѿ���������״̬��ֱ�ӷ���ERROR */
    if ((cardLockStatus.status == NV_ITEM_ACTIVE) &&
        (cardLockStatus.remainUnlockTimes == TAF_OPERATOR_UNLOCK_TIMES_MIN) &&
        (cardLockStatus.cardlockStatus == TAF_OPERATOR_LOCK_LOCKED)) {
        AT_WARN_LOG("At_SetSimLockPlmnInfo: is locked, operation is not allowed.");
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_ERROR;
    }

    /* ��NV_ITEM_CARDLOCK_STATUS��״̬��Ϊ���CardStatus ����������Ϊ1,Remain Times���ֲ��� */
    cardLockStatus.status         = NV_ITEM_ACTIVE;
    cardLockStatus.cardlockStatus = TAF_OPERATOR_LOCK_NEED_UNLOCK_CODE;

    rslt = TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_CARDLOCK_STATUS, (VOS_UINT8 *)&cardLockStatus,
                              sizeof(cardLockStatus));
    if (rslt != NV_OK) {
        AT_WARN_LOG("At_SetSimLockPlmnInfo:write NV_ITEM_CARDLOCK_STATUS Fail.");
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_ERROR;
    }

    /* ��C�˷�����Ϣ����simlockNV */
    if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, DRV_AGENT_SIMLOCK_NV_SET_REQ,
                               (VOS_UINT8 *)&cardLockStatus, sizeof(cardLockStatus),
                               I0_WUEPS_PID_DRV_AGENT) != TAF_SUCCESS) {
        AT_WARN_LOG("At_SetSimLockPlmnInfo():DRV_AGENT_SIMLOCK_NV_SET_REQ NV Write Fail!");
        At_FormatResultData(indexNum, AT_ERROR);
        return VOS_ERR;
    }

    /* ��PLMN��Ϣд�뵽NV��NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO */
    rslt = TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO, (VOS_UINT8 *)&simLockPlmnInfo,
                              sizeof(simLockPlmnInfo));
    if (rslt != NV_OK) {
        AT_WARN_LOG("At_SetSimLockPlmnInfo:write NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO Fail.");
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_ERROR;
    }

    At_FormatResultData(indexNum, AT_OK);
    return AT_OK;
}

VOS_UINT32 AT_ProcAuthPubkeyExData(VOS_UINT32 paraLen, VOS_UINT8 *pubKeyData)
{
    AT_AUTH_PUBKEYEX_CMD_PROC_CTX *authPubKeyCtx = VOS_NULL_PTR;
    VOS_UINT8                     *tempData      = VOS_NULL_PTR;
    errno_t                        memResult;
    VOS_UINT16                     totalLen;

    if ((pubKeyData == VOS_NULL_PTR) || (paraLen == 0)) {
        AT_ERR_LOG("AT_ProcAuthPubkeyExData: NULL Pointer");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    authPubKeyCtx = AT_GetAuthPubkeyExCmdCtxAddr();

    /* ��ǰ��һ���µ����ù��̣��յ����ǵ�һ��AT���� */
    if (authPubKeyCtx->data == VOS_NULL_PTR) {
        authPubKeyCtx->data = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, paraLen);

        /* �����ڴ�ʧ�ܣ�ֱ�ӷ��� */
        if (authPubKeyCtx->data == VOS_NULL_PTR) {
            AT_ERR_LOG("AT_ProcAuthPubkeyExData: first data, Alloc mem fail");

            return AT_ERROR;
        }

        (VOS_VOID)memset_s(authPubKeyCtx->data, paraLen, 0x00, paraLen);

        memResult = memcpy_s(authPubKeyCtx->data, paraLen, pubKeyData, paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, paraLen, paraLen);

        authPubKeyCtx->paraLen = (VOS_UINT16)paraLen;
    } else {
        /* ��ǰ�����յ���һ��AT�����Ҫƴ������ */
        totalLen = (VOS_UINT16)paraLen + authPubKeyCtx->paraLen;
        if (totalLen < paraLen) {
            return AT_ERROR;
        }
        /*lint -save -e516 */
        tempData = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, totalLen);
        /*lint -restore */
        /* �����ڴ�ʧ�ܣ�ֱ�ӷ��� */
        if (tempData == VOS_NULL_PTR) {
            AT_ERR_LOG("AT_ProcAuthPubkeyExData: Non-first data, Alloc mem fail");

            return AT_ERROR;
        }

        (VOS_VOID)memset_s(tempData, totalLen, 0x00, totalLen);

        if (authPubKeyCtx->paraLen > 0) {
            memResult = memcpy_s(tempData, totalLen, authPubKeyCtx->data, authPubKeyCtx->paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, totalLen, authPubKeyCtx->paraLen);
        }
        memResult = memcpy_s((tempData + authPubKeyCtx->paraLen), paraLen, pubKeyData, paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, paraLen, paraLen);
        PS_MEM_FREE(WUEPS_PID_AT, authPubKeyCtx->data);
        authPubKeyCtx->data = VOS_NULL_PTR;

        authPubKeyCtx->paraLen = totalLen;
        authPubKeyCtx->data    = tempData;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_FirstSetAuthPubKeyCtx(VOS_UINT8 indexNum, VOS_UINT32 currIndex, VOS_UINT32 total, VOS_UINT32 paraLen,
                                     VOS_UINT8 *pubKeyData)
{
    AT_AUTH_PUBKEYEX_CMD_PROC_CTX *authPubKeyCtx       = VOS_NULL_PTR;
    VOS_UINT32                     result;

    authPubKeyCtx = AT_GetAuthPubkeyExCmdCtxAddr();

    if (currIndex != 1) {
        AT_WARN_LOG1("AT_FirstSetAuthPubKeyCtx: Invalid ulCurrIndex", currIndex);

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���ַ�������ת��Ϊ���� */
    result = AT_ProcAuthPubkeyExData(paraLen, pubKeyData);
    if (result != AT_SUCCESS) {
        AT_WARN_LOG1("AT_FirstSetAuthPubKeyCtx: AT_ProcAuthPubkeyExData fail %d", result);

        return result;
    }

    authPubKeyCtx->clientId    = indexNum;
    authPubKeyCtx->totalNum    = (VOS_UINT8)total;
    authPubKeyCtx->curIdx      = (VOS_UINT8)currIndex;
    authPubKeyCtx->settingFlag = VOS_TRUE;

    return AT_SUCCESS;
}

VOS_UINT32 AT_CheckAuthPubKeyCtxInSettingProcess(VOS_UINT8 indexNum, VOS_UINT32 currIndex, VOS_UINT32 total,
    VOS_UINT32 timerName, VOS_UINT8 *pubKeyData)
{
    AT_AUTH_PUBKEYEX_CMD_PROC_CTX *authPubKeyCtx       = VOS_NULL_PTR;

    authPubKeyCtx = AT_GetAuthPubkeyExCmdCtxAddr();

    /* ������ͬһ��ͨ���·����� */
    if (indexNum != authPubKeyCtx->clientId) {
        AT_WARN_LOG2("AT_SetFacAuthPubkeyExPara: port error, ucIndex %d ucClientId, %d", indexNum,
                     authPubKeyCtx->clientId);

        AT_ClearAuthPubkeyCtx();
        (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ǰ�Ѿ��������У���ǰ�·���total��֮ǰ֮ǰ�·���total��ͬ */
    if ((VOS_UINT8)total != authPubKeyCtx->totalNum) {
        AT_WARN_LOG2("AT_SetFacAuthPubkeyExPara: total %d wrong, %d", total, authPubKeyCtx->totalNum);

        AT_ClearAuthPubkeyCtx();
        (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ǰ�·���Index����֮ǰ�·�Index+1 */
    if ((VOS_UINT8)currIndex != (authPubKeyCtx->curIdx + 1)) {
        AT_WARN_LOG2("AT_SetFacAuthPubkeyExPara: CurrIndex %d wrong, %d", currIndex, authPubKeyCtx->curIdx);

        AT_ClearAuthPubkeyCtx();
        (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_SetAuthPubKeyCtxValue(VOS_UINT8 indexNum, VOS_UINT32 currIndex, VOS_UINT32 total, VOS_UINT32 paraLen,
                                     VOS_UINT8 *pubKeyData)
{
    AT_AUTH_PUBKEYEX_CMD_PROC_CTX *authPubKeyCtx       = VOS_NULL_PTR;
    VOS_UINT32                     result;
    VOS_UINT32                     tempIndex;
    VOS_UINT32                     timerName;

    tempIndex = (VOS_UINT32)indexNum;
    timerName = AT_AUTH_PUBKEY_TIMER;
    timerName |= AT_INTERNAL_PROCESS_TYPE;
    timerName |= (tempIndex << 12);

    authPubKeyCtx = AT_GetAuthPubkeyExCmdCtxAddr();

    /* ��ǰ�������ù����У���һ���յ������� */
    if (authPubKeyCtx->settingFlag == VOS_FALSE) {
        result = AT_FirstSetAuthPubKeyCtx(indexNum, currIndex, total, paraLen, pubKeyData);
        if (result != AT_SUCCESS) {
            return result;
        }
    } else {
        result = AT_CheckAuthPubKeyCtxInSettingProcess(indexNum, currIndex, total, timerName, pubKeyData);
        if (result != AT_SUCCESS) {
            return result;
        }

        /* ���ַ�������ת��Ϊ���� */
        result = AT_ProcAuthPubkeyExData(paraLen, pubKeyData);
        if (result != AT_SUCCESS) {
            AT_WARN_LOG1("AT_SetAuthPubKeyCtxValue: AT_ProcAuthPubkeyExData fail %d", result);

            AT_ClearAuthPubkeyCtx();
            (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
            return result;
        }

        /* ����CurrIndex */
        authPubKeyCtx->curIdx = (VOS_UINT8)currIndex;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_SendDrvAgentFacAuthPubKeyReq(VOS_UINT8 indexNum, VOS_UINT32 timerName)
{
    DRV_AGENT_FacauthpubkeySetReq *facAuthPubkeySetReq = VOS_NULL_PTR;
    AT_AUTH_PUBKEYEX_CMD_PROC_CTX *authPubKeyCtx       = VOS_NULL_PTR;
    errno_t                        memResult;
    VOS_UINT32                     result;

    authPubKeyCtx = AT_GetAuthPubkeyExCmdCtxAddr();

    /*lint -save -e516 */
    facAuthPubkeySetReq = (DRV_AGENT_FacauthpubkeySetReq *)PS_MEM_ALLOC(WUEPS_PID_AT,
                                                                        sizeof(DRV_AGENT_FacauthpubkeySetReq));
    /*lint -restore */
    if (facAuthPubkeySetReq == VOS_NULL_PTR) {
        AT_WARN_LOG("AT_SetFacAuthPubkeyExPara: alloc mem fail.");

        AT_ClearAuthPubkeyCtx();
        (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
        return AT_ERROR;
    }

    memResult = memcpy_s(facAuthPubkeySetReq, sizeof(DRV_AGENT_FacauthpubkeySetReq), authPubKeyCtx->data,
                         authPubKeyCtx->paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(DRV_AGENT_FacauthpubkeySetReq), authPubKeyCtx->paraLen);

    AT_ClearAuthPubkeyCtx();
    (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
    /* ת���ɹ�, ���Ϳ����Ϣ��C��, ���ò��߹�Կ */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    DRV_AGENT_FACAUTHPUBKEY_SET_REQ, (VOS_UINT8 *)facAuthPubkeySetReq,
                                    sizeof(DRV_AGENT_FacauthpubkeySetReq), I0_WUEPS_PID_DRV_AGENT);
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, facAuthPubkeySetReq);
    /*lint -restore */
    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetFacAuthPubkeyExPara: AT_FillAndSndAppReqMsg fail.");

        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetFacAuthPubkeyExPara(VOS_UINT8 indexNum, VOS_UINT32 currIndex, VOS_UINT32 total, VOS_UINT32 paraLen,
                                     VOS_UINT8 *pubKeyData)
{
    AT_AUTH_PUBKEYEX_CMD_PROC_CTX *authPubKeyCtx       = VOS_NULL_PTR;
    VOS_UINT32                     result;
    VOS_UINT32                     tempIndex;
    VOS_UINT32                     timerName;

    tempIndex = (VOS_UINT32)indexNum;
    timerName = AT_AUTH_PUBKEY_TIMER;
    timerName |= AT_INTERNAL_PROCESS_TYPE;
    timerName |= (tempIndex << 12);

    authPubKeyCtx = AT_GetAuthPubkeyExCmdCtxAddr();

    /* �����������ͣ��������ͺͲ������� */
    g_atParseCmd.cmdOptType = AT_CMD_OPT_SET_PARA_CMD;
    g_atCmdFmtType          = AT_EXTEND_CMD_TYPE;

    /* IndexҪС��total */
    if (currIndex > total) {
        AT_WARN_LOG2("AT_SetFacAuthPubkeyExPara: Index bigger then total", currIndex, total);

        AT_ClearAuthPubkeyCtx();
        (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
        return AT_CME_INCORRECT_PARAMETERS;
    }

    result = AT_SetAuthPubKeyCtxValue(indexNum, currIndex, total, paraLen, pubKeyData);
    if (result != AT_SUCCESS) {
        AT_WARN_LOG("AT_SetFacAuthPubkeyExPara: SetAuthPubKeyCtxValue fail.");
        return result;
    }

    /* �����δ�������ݣ���������ʱ�����ظ�OK */
    if (authPubKeyCtx->curIdx < authPubKeyCtx->totalNum) {
        (VOS_VOID)AT_StartRelTimer(&(authPubKeyCtx->hAuthPubkeyProtectTimer), AT_AUTH_PUBKEY_PROTECT_TIMER_LEN,
                                   timerName, 0, VOS_RELTIMER_NOLOOP);
        AT_WARN_LOG("AT_SetFacAuthPubkeyExPara: AT_StartRelTimer OK.");
        return AT_OK;
    }

    /* �Ѿ����������ݣ�ת�������󷢸�C�� */
    result = At_AsciiNum2HexString(authPubKeyCtx->data, &authPubKeyCtx->paraLen);
    if ((result != AT_SUCCESS) || (authPubKeyCtx->paraLen != (DRV_AGENT_PUBKEY_LEN + DRV_AGENT_PUBKEY_SIGNATURE_LEN))) {
        AT_WARN_LOG2("AT_SetFacAuthPubkeyExPara: At_AsciiNum2HexString fail ulResult: %d ulParaLen: %d", result,
                     authPubKeyCtx->paraLen);

        AT_ClearAuthPubkeyCtx();
        (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_SendDrvAgentFacAuthPubKeyReq(indexNum, timerName) == AT_ERROR) {
        AT_WARN_LOG("AT_SetFacAuthPubkeyExPara: FacAuthPubKeyReq fail.");
        return AT_ERROR;
    }

    /* ����FACAUTHPUBKEYEX���⴦����Ҫ�ֶ�������ʱ�� */
    if (At_StartTimer(AT_SET_PARA_TIME, indexNum) != AT_SUCCESS) {
        AT_WARN_LOG("AT_SetFacAuthPubkeyExPara: At_StartTimer fail.");

        return AT_ERROR;
    }

    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_PEND;

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FACAUTHPUBKEYEX_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetSimlockDataWritePara(VOS_UINT8 indexNum, AT_SIMLOCKDATAWRITE_SetReq *simlockDataWrite)
{
    return AT_OK;
}

/*
 * ��������: ��ԭMCCdת��ΪNAS�ڲ�ʹ�ø�ʽ��¼
 *           ����:MCCΪ460
 *           pstPlmnId->Mcc = 0x000604      pstPlmnId->Mcc = 0x00000460
 *
 */
VOS_VOID AT_ConvertMccToNasType(VOS_UINT32 mcc, VOS_UINT32 *nasMcc)
{
    VOS_UINT32 tmp[AT_MCC_LENGTH];
    VOS_UINT32 i;

    *nasMcc = 0;

    for (i = 0; i < AT_MCC_LENGTH; i++) {
        tmp[i] = mcc & 0x0f;
        mcc >>= 4;
    }

    /* tmp[0],tmp[1],tmp[2]�е�0 1 2�ֱ��ʾtmp[AT_MCC_LENGTH]�еĵ�0��Ԫ�أ���1��Ԫ�أ��͵�2��Ԫ�� */
    *nasMcc = tmp[2] | ((VOS_UINT32)tmp[1] << 8) | ((VOS_UINT32)tmp[0] << 16);
}

VOS_UINT32 AT_SetCISAPara(VOS_UINT8 indexNum, VOS_UINT32 lengthValue, VOS_UINT8 *command, VOS_UINT16 commandLength)
{
    SI_PIH_IsdbAccessCommand commandInfo;
    errno_t                  memResult;
    VOS_UINT16               commandHexLen;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&commandInfo, sizeof(commandInfo), 0x00, sizeof(commandInfo));

    /* ������� */
    if ((lengthValue > (SI_APDU_MAX_LEN * 2)) || (lengthValue != commandLength)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <command>��ASCII�룬�����������HEX��ת�� */
    commandHexLen = commandLength;
    if (At_AsciiNum2HexString(command, &commandHexLen) == AT_FAILURE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<length> */
    commandInfo.len = commandHexLen;

    /* ����<command> */
    if ((VOS_UINT16)commandInfo.len > 0) {
        memResult = memcpy_s((TAF_VOID *)commandInfo.command, sizeof(commandInfo.command), (TAF_VOID *)command,
                             (VOS_UINT16)commandInfo.len);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(commandInfo.command), (VOS_UINT16)commandInfo.len);
    }

    /* ִ��������� */
    if (SI_PIH_IsdbAccessReq(g_atClientTab[indexNum].clientId, 0, &commandInfo) == AT_SUCCESS) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CISA_SET;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("AT_SetCISAPara: SI_PIH_IsdbAccessReq fail.");

        return AT_ERROR;
    }
}

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
VOS_UINT32 AT_SetCcmgsPara(VOS_UINT8 indexNum, VOS_UINT32 lengthValue, VOS_UINT8 *command, VOS_UINT16 commandLength)
{
    TAF_XSMS_Message smsPdu;
    errno_t          memResult;
    VOS_UINT16       commandHexLen;
    TAF_UINT8        smSFormat;

    smSFormat = AT_GetModemSmsCtxAddrFromClientId(indexNum)->cmgfMsgFormat;

    if (smSFormat != AT_CMGF_MSG_FORMAT_PDU) {
        AT_WARN_LOG("AT_SetCcmgsPara: current format is not PDU!");
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&smsPdu, sizeof(smsPdu), 0x00, sizeof(smsPdu));

    /* ������� */
    if ((lengthValue > (AT_DOUBLE_LENGTH * sizeof(TAF_XSMS_Message))) || (lengthValue != commandLength) ||
        ((lengthValue % AT_DOUBLE_LENGTH) != VOS_NULL)) {
        AT_WARN_LOG("AT_SetCcmgsPara: incorrect parameters!");
        AT_WARN_LOG1("AT_SetCcmgsPara: ulLengthValue:", lengthValue);
        AT_WARN_LOG1("AT_SetCcmgsPara: usCommandLength:", commandLength);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <PDU>��ASCII�룬�����������HEX��ת�� */
    commandHexLen = commandLength;
    if (At_AsciiNum2HexString(command, &commandHexLen) == AT_FAILURE) {
        AT_WARN_LOG("AT_SetCcmgsPara: ascii2hex fail!");
        AT_WARN_LOG1("AT_SetCcmgsPara: usCommandLength:", commandLength);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��PDU���������������ݽṹ */
    if (commandHexLen > 0) {
        memResult = memcpy_s(&smsPdu, sizeof(smsPdu), command, commandHexLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(smsPdu), commandHexLen);
    }

    /* ִ��������� */
    if (TAF_XSMS_SendSmsReq(g_atClientTab[indexNum].clientId, 0, TAF_XSMS_SEND_OPTION_SINGLE, (VOS_UINT8 *)&smsPdu) ==
        AT_SUCCESS) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CCMGS_SET;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("AT_SetCcmgsPara: CNAS_XSMS_SendSmsReq fail.");

        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetCcmgwPara(VOS_UINT8 indexNum, VOS_UINT32 lengthValue, VOS_UINT32 statValue, VOS_UINT8 *command,
                           VOS_UINT16 commandLength)
{
    TAF_XSMS_Message smsPdu;
    errno_t          memResult;
    VOS_UINT16       commandHexLen;
    TAF_UINT8        smSFormat;

    smSFormat = AT_GetModemSmsCtxAddrFromClientId(indexNum)->cmgfMsgFormat;

    if (smSFormat != AT_CMGF_MSG_FORMAT_PDU) {
        return AT_CMS_OPERATION_NOT_ALLOWED;
    }

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&smsPdu, sizeof(smsPdu), 0x00, sizeof(smsPdu));

    /* ������� */
    if ((lengthValue > (AT_DOUBLE_LENGTH * sizeof(TAF_XSMS_Message))) || (lengthValue != commandLength) ||
        ((lengthValue % AT_DOUBLE_LENGTH) != VOS_NULL)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����2�Ϸ��Լ�� */
    if (statValue > TAF_XSMS_STATUS_STO_SEND) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <PDU>��ASCII�룬�����������HEX��ת�� */
    commandHexLen = commandLength;
    if (At_AsciiNum2HexString(command, &commandHexLen) == AT_FAILURE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��PDU���������������ݽṹ */
    if (commandHexLen > 0) {
        memResult = memcpy_s(&smsPdu, sizeof(smsPdu), command, commandHexLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(smsPdu), commandHexLen);
    }

    /* ִ��������� */
    if (TAF_XSMS_WriteSmsReq(g_atClientTab[indexNum].clientId, 0, (VOS_UINT8)statValue, (VOS_UINT8 *)&smsPdu) ==
        AT_SUCCESS) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CCMGW_SET;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("AT_SetCcmgwPara: CNAS_XSMS_WriteSmsReq fail.");

        return AT_ERROR;
    }
}
#endif

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
VOS_UINT32 AT_IsPlatformSupportHrpdMode(PLATAFORM_RatCapability *platform)
{
    VOS_UINT16 i;

    for (i = 0; i < AT_MIN(platform->ratNum, PLATFORM_MAX_RAT_NUM); i++) {
        if (platform->ratList[i] == PLATFORM_RAT_HRPD) {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

VOS_UINT32 AT_IsPlatformSupport1XMode(PLATAFORM_RatCapability *platform)
{
    VOS_UINT16 i;

    for (i = 0; i < AT_MIN(platform->ratNum, PLATFORM_MAX_RAT_NUM); i++) {
        if (platform->ratList[i] == PLATFORM_RAT_1X) {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}
#endif

VOS_UINT32 AT_IsSpecRatSupported(TAF_MMA_RatTypeUint8 ratMode, TAF_MMA_MultimodeRatCfg *ratOrder)
{
    VOS_UINT32 i;

    ratOrder->ratNum = AT_MIN(ratOrder->ratNum, TAF_MMA_RAT_BUTT);
    for (i = 0; i < ratOrder->ratNum; i++) {
        if (ratMode == ratOrder->ratOrder[i]) {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

TAF_MMA_UserSetPrioRatUint8 AT_GetSysCfgPrioRat(TAF_MMA_SysCfgPara *sysCfgExSetPara)
{
    TAF_MMA_UserSetPrioRatUint8 userPrio = TAF_MMA_USER_SET_PRIO_NOCHANGE;
    VOS_UINT8                   i;

    if (AT_IsSpecRatSupported(TAF_MMA_RAT_LTE, &sysCfgExSetPara->multiModeRatCfg) == VOS_TRUE) {
        return userPrio;
    }

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    if (AT_IsSpecRatSupported(TAF_MMA_RAT_NR, &sysCfgExSetPara->multiModeRatCfg) == VOS_TRUE) {
        return userPrio;
    }
#endif
    for (i = 0; i < AT_MIN(TAF_MMA_RAT_BUTT, sysCfgExSetPara->multiModeRatCfg.ratNum); i++) {
        if (sysCfgExSetPara->multiModeRatCfg.ratOrder[i] == TAF_MMA_RAT_GSM) {
            userPrio = TAF_MMA_USER_SET_PRIO_GSM_PREFER;
            break;
        }

        if (sysCfgExSetPara->multiModeRatCfg.ratOrder[i] == TAF_MMA_RAT_WCDMA) {
            userPrio = TAF_MMA_USER_SET_PRIO_WCDMA_PREFER;
            break;
        }
    }

    return userPrio;
}

TAF_WRITE_AcoreNv* AT_FillACoreNvWriteStru(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT8 *data,
                                           VOS_UINT32 nvLength)
{
    g_writeAcoreNv.modemId     = modemId;
    g_writeAcoreNv.nvItemId    = nvItemId;
    g_writeAcoreNv.nvLength    = nvLength;
    g_writeAcoreNv.isNeedCheck = VOS_TRUE;
    g_writeAcoreNv.offset      = 0;
    g_writeAcoreNv.data        = data;

    return &g_writeAcoreNv;
}

TAF_WRITE_AcoreNv* AT_FillACoreNvWriteNoCheckStru(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT8 *data,
                                                  VOS_UINT32 nvLength)
{
    g_writeAcoreNv.modemId     = modemId;
    g_writeAcoreNv.nvItemId    = nvItemId;
    g_writeAcoreNv.nvLength    = nvLength;
    g_writeAcoreNv.isNeedCheck = VOS_FALSE;
    g_writeAcoreNv.offset      = 0;
    g_writeAcoreNv.data        = data;

    return &g_writeAcoreNv;
}

TAF_WRITE_AcoreNv* AT_FillACoreNvWritePartStru(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT32 offset,
                                               VOS_UINT8 *data, VOS_UINT32 nvLength)
{
    g_writeAcoreNv.modemId     = modemId;
    g_writeAcoreNv.nvItemId    = nvItemId;
    g_writeAcoreNv.nvLength    = nvLength;
    g_writeAcoreNv.isNeedCheck = VOS_TRUE;
    g_writeAcoreNv.offset      = offset;
    g_writeAcoreNv.data        = data;

    return &g_writeAcoreNv;
}

TAF_WRITE_AcoreNv* AT_FillACoreNvWritePartNoCheckStru(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT32 offset,
                                                      VOS_UINT8 *data, VOS_UINT32 nvLength)
{
    g_writeAcoreNv.modemId     = modemId;
    g_writeAcoreNv.nvItemId    = nvItemId;
    g_writeAcoreNv.nvLength    = nvLength;
    g_writeAcoreNv.isNeedCheck = VOS_FALSE;
    g_writeAcoreNv.offset      = offset;
    g_writeAcoreNv.data        = data;

    return &g_writeAcoreNv;
}

