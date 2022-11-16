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
#include "at_custom_mm_set_cmd_proc.h"
#include "securec.h"

#include "AtParse.h"
#include "at_cmd_proc.h"

#include "taf_app_mma.h"
#include "at_set_para_cmd.h"
#include "css_at_interface.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_MM_SET_CMD_PROC_C

#define AT_DPLMNLIST_MIN_PARA_NUM 6
#define AT_DPLMNLIST_VER 1

#define AT_BORDERINFO_OPERTION_TYPE 0
#define AT_BORDERINFO_MIN_PARA_NUM 7
#define AT_BORDERINFO_MAX_PARA_NUM 9
#define TAF_MMA_BODER_INFO_BUFF_LEN 4
#define AT_PLMNSRCH_RAT 2
#define AT_PLMNSRCH_SRCHTYPE 3
#define AT_PLMNSRCH_RSRP 4
#define AT_PLMNSRCH_LTE_RSRP_THRESHOLD_MIN 44
#define AT_PLMNSRCH_LTE_RSRP_THRESHOLD_MAX 141
#define AT_PLMNSRCH_NR_RSRP_THRESHOLD_MIN 31
#define AT_PLMNSRCH_NR_RSRP_THRESHOLD_MAX 156
#define AT_PLMNSRCH_PARA_NUM 4
#define AT_PLMNSRCH_MAX_PARA_NUM 5
#define AT_PLMNSRCH_SRCH_MODE_FLAG 0
#define AT_PLMNSRCH_PLMN_LEN 1
#define AT_DETECTPLMN_RAT 1
#define AT_DETECTPLMN_PARA_NUM 2
#define AT_DETECTPLMN_PLMN 0
#define AT_UCNUMDPLMNSUB_NUM1 1
#define AT_UCNUMDPLMNSUB_NUM2 2
#define AT_UCNUMDPLMNSUB_NUM3 3
#define AT_DPLMNLIST_EHPLMNNUM 2
#define AT_DPLMNLIST_EHPLMN 3
#define AT_DPLMNLIST_BSCOUNT 4
#define AT_DPLMNLIST_BS1 5
#define AT_DPLMNLIST_BS2 6
#define AT_DPLMNLIST_BS3 7
#define AT_BORDERINFO_VER 2
#define AT_BORDERINFO_EHPLMNNUM 3
#define AT_BORDERINFO_EHPLMNLIST 4
#define AT_BORDERINFO_COUNT 5
#define AT_BORDERINFO_BS1 6
#define AT_BORDERINFO_BS2 7
#define AT_BORDERINFO_BS3 8
#define AT_PSSCENE_MAX_PARA_NUM 2
#define AT_CACDC_ENAPPINDICATION 2
#define AT_IMSSWITCH_PARA_MAX_NUM 3
#define AT_IMSSWITCH_LTE_ENABLE 0
#define AT_IMSSWITCH_UTRAN_ENABLE 1
#define AT_IMSSWITCH_GSM_ENABLE 2
#define AT_OOS_RECORD_LEN (15)
#define AT_CSGIDSRCH_PARA_NUM 3
#define AT_CSGIDSRCH_PLMN_ID_VAILD_LEN1 5
#define AT_CSGIDSRCH_PLMN_ID_VAILD_LEN2 6
#define AT_CSGIDSRCH_CSG_ID_LEN_MAX 8
#define AT_CSGIDSRCH_PLMN_ID 0
#define AT_CSGIDSRCH_CSG_ID 1
#define AT_CSGIDSRCH_RAT 2
#define AT_CMM_PARA_MAX_NUM 2
#define AT_USIMSTUB_CARD_ID 0
#define AT_USIMSTUB_CARD_TYPE 1
#define AT_USIMSTUB_TOTALNUM 2
#define AT_USIMSTUB_EFLEN 3
#define AT_USIMSTUB_FILECONTENT 4
#define AT_REFRESHSTUB_TOTALNUM 2
#define AT_HEX_ONEBYTE_VALID_LENTH 2  // HEX_ONEBYTE_VALID_LENTH:һ���ֽڵ�16����0x������λ�ĳ���
#define AT_REFRESHSTUB_RECEIVED_PID 0
#define AT_REFRESHSTUB_FILE_TYPE 1
#define AT_REFRESHSTUB_FILE_ID 3
#define AT_NRIMSSWITCH_PARA_NUM 1
#define AT_NRIMSSWITCH_ENABLE_INDEX_0 0
#define AT_SUB_STR_INDEX_3 3
#define AT_EHPLMN_LEN_FIVE 5
#define AT_EHPLMN_LEN_SIX 6
#define AT_DPLMN_LEN_FIVE 5
#define AT_DPLMN_LEN_SIX 6

#define TAF_AT_EONS_MAX_PARA_NUM 3
#define TAF_AT_EONS_LEN_PARA_INDEX 2

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_SetRcsSwitch(VOS_UINT8 indexNum)
{
    VOS_UINT32            rst;
    TAF_MMA_ImsSwitchInfo imsSwitchInfo;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != 1) {
        return AT_TOO_MANY_PARA;
    }

    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&imsSwitchInfo, sizeof(imsSwitchInfo), 0, sizeof(imsSwitchInfo));

    imsSwitchInfo.bitOpRcsSwitch = VOS_TRUE;
    imsSwitchInfo.rcsSwitch      = g_atParaList[0].paraValue;

    rst = TAF_MMA_SetImsSwitchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                  &imsSwitchInfo);
    if (rst != VOS_TRUE) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_RCSSWITCH_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_DplmnListParaCheck(VOS_VOID)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��������ȷ */
    if (g_atParaIndex < AT_DPLMNLIST_MIN_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �汾�ų��Ȳ��� */
    if (g_atParaList[AT_DPLMNLIST_VER].paraLen != TAF_MMA_VERSION_INFO_LEN) {
        AT_ERR_LOG("AT_SetDplmnListPara: incorrect version length ");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}

VOS_VOID AT_ProcNumDplmnSubStrNum1(VOS_UINT8 *dplmnString)
{
    errno_t memResult;

    if ((g_atParaList[AT_DPLMNLIST_BS1].paraLen > 0) &&
        (g_atParaList[AT_DPLMNLIST_BS1].paraLen <= TAF_MMA_MAX_STR_LEN)) {
        memResult = memcpy_s(dplmnString, TAF_MMA_MAX_STR_LEN, (VOS_UINT8 *)g_atParaList[AT_DPLMNLIST_BS1].para,
                             g_atParaList[AT_DPLMNLIST_BS1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_MAX_STR_LEN, g_atParaList[AT_DPLMNLIST_BS1].paraLen);
    }
}

VOS_VOID AT_ProcNumDplmnSubStrNum2(VOS_UINT8 *dplmnString)
{
    errno_t memResult;

    if ((g_atParaList[AT_DPLMNLIST_BS1].paraLen > 0) &&
        (g_atParaList[AT_DPLMNLIST_BS1].paraLen <= TAF_MMA_MAX_STR_LEN)) {
        memResult = memcpy_s(dplmnString, TAF_MMA_MAX_STR_LEN, (VOS_UINT8 *)g_atParaList[AT_DPLMNLIST_BS1].para,
                             g_atParaList[AT_DPLMNLIST_BS1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_MAX_STR_LEN, g_atParaList[AT_DPLMNLIST_BS1].paraLen);
    }
    if ((g_atParaList[AT_DPLMNLIST_BS2].paraLen > 0) &&
        ((g_atParaList[AT_DPLMNLIST_BS2].paraLen + g_atParaList[AT_DPLMNLIST_BS1].paraLen ) <=
          TAF_MMA_MAX_STR_LEN)) {
        memResult = memcpy_s(dplmnString + g_atParaList[AT_DPLMNLIST_BS1].paraLen,
                             TAF_MMA_MAX_STR_LEN - g_atParaList[AT_DPLMNLIST_BS1].paraLen,
                             (VOS_UINT8 *)g_atParaList[AT_DPLMNLIST_BS2].para,
                             g_atParaList[AT_DPLMNLIST_BS2].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_MAX_STR_LEN - g_atParaList[AT_DPLMNLIST_BS1].paraLen,
                            g_atParaList[AT_DPLMNLIST_BS2].paraLen);
    }
}

VOS_VOID AT_ProcNumDplmnSubStrNum3(VOS_UINT8 *dplmnString)
{
    errno_t memResult;

    if ((g_atParaList[AT_DPLMNLIST_BS1].paraLen > 0) &&
        (g_atParaList[AT_DPLMNLIST_BS1].paraLen <= TAF_MMA_MAX_STR_LEN)) {
        memResult = memcpy_s(dplmnString, TAF_MMA_MAX_STR_LEN,
                             (VOS_UINT8 *)g_atParaList[AT_DPLMNLIST_BS1].para,
                             g_atParaList[AT_DPLMNLIST_BS1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_MAX_STR_LEN, g_atParaList[AT_DPLMNLIST_BS1].paraLen);
    }
    if ((g_atParaList[AT_DPLMNLIST_BS2].paraLen > 0) &&
        ((g_atParaList[AT_DPLMNLIST_BS2].paraLen + g_atParaList[AT_DPLMNLIST_BS1].paraLen) <=
         TAF_MMA_MAX_STR_LEN)) {
        memResult = memcpy_s(dplmnString + g_atParaList[AT_DPLMNLIST_BS1].paraLen,
                             TAF_MMA_MAX_STR_LEN - g_atParaList[AT_DPLMNLIST_BS1].paraLen,
                             (VOS_UINT8 *)g_atParaList[AT_DPLMNLIST_BS2].para,
                             g_atParaList[AT_DPLMNLIST_BS2].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_MAX_STR_LEN - g_atParaList[AT_DPLMNLIST_BS1].paraLen,
                            g_atParaList[AT_DPLMNLIST_BS2].paraLen);
    }
    if ((g_atParaList[AT_DPLMNLIST_BS3].paraLen > 0) &&
        ((g_atParaList[AT_DPLMNLIST_BS1].paraLen + g_atParaList[AT_DPLMNLIST_BS2].paraLen +
         g_atParaList[AT_DPLMNLIST_BS3].paraLen) <= TAF_MMA_MAX_STR_LEN)) {
        memResult = memcpy_s(dplmnString + g_atParaList[AT_DPLMNLIST_BS1].paraLen +
                                g_atParaList[AT_DPLMNLIST_BS2].paraLen,
                             TAF_MMA_MAX_STR_LEN - g_atParaList[AT_DPLMNLIST_BS1].paraLen -
                                g_atParaList[AT_DPLMNLIST_BS2].paraLen,
                             (VOS_UINT8 *)g_atParaList[AT_DPLMNLIST_BS3].para,
                             g_atParaList[AT_DPLMNLIST_BS3].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult,
                            TAF_MMA_MAX_STR_LEN - g_atParaList[AT_DPLMNLIST_BS1].paraLen -
                            g_atParaList[AT_DPLMNLIST_BS2].paraLen,
                            g_atParaList[AT_DPLMNLIST_BS3].paraLen);
    }
}

VOS_UINT32 AT_DplmnFillDplmnString(VOS_UINT8 numDplmnSubStr, VOS_UINT8 *dplmnString, TAF_MMA_DplmnInfoSet *dplmnInfo)
{
    switch (numDplmnSubStr) {
        case AT_UCNUMDPLMNSUB_NUM1:
            AT_ProcNumDplmnSubStrNum1(dplmnString);
            break;

        case AT_UCNUMDPLMNSUB_NUM2:
            AT_ProcNumDplmnSubStrNum2(dplmnString);
            break;

        case AT_UCNUMDPLMNSUB_NUM3:
            AT_ProcNumDplmnSubStrNum3(dplmnString);
            break;

        default:
            AT_ERR_LOG("AT_SetDplmnListPara: ucNumDplmnSubStr error!");
            return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_ParseDplmnLengthIllegal(VOS_UINT16 subStrLen)
{
    /* Dplmn���Ȳ��Ϸ� */
    if ((subStrLen != AT_DPLMN_LEN_FIVE) && (subStrLen != AT_DPLMN_LEN_SIX)) {
        AT_ERR_LOG("AT_ParseDplmnLengthIllegal: dplmn len error");
        return AT_ERROR;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_CheckDplmnPara(TAF_PLMN_Id *plmnId, VOS_UINT8 *subStr, VOS_UINT16 subStrLen)
{
    /* Dplmn���Ȳ��Ϸ� */
    if (AT_ParseDplmnLengthIllegal(subStrLen) == AT_ERROR) {
        AT_ERR_LOG("AT_CheckDplmnPara: dplmn len error");
        return AT_ERROR;
    }

    /* Dplmn Mcc */
    if (AT_String2Hex(subStr, AT_MCC_LENGTH, &plmnId->mcc) == VOS_ERR) {
        AT_ERR_LOG("AT_CheckDplmnPara: dplmn mcc error");
        return AT_ERROR;
    }

    /* Dplmn Mnc */
    if (AT_String2Hex(&subStr[AT_SUB_STR_INDEX_3], subStrLen - AT_MCC_LENGTH, &plmnId->mnc) == VOS_ERR) {
        AT_ERR_LOG("AT_CheckDplmnPara: dplmn mnc error");
        return AT_ERROR;
    }

    plmnId->mcc &= 0x0FFF;

    if (subStrLen == AT_DPLMN_LEN_FIVE) {
        plmnId->mnc |= 0x0F00;
    }

    plmnId->mnc &= 0x0FFF;

    /* ����������Dplmn����Ч��Plmn */
    if (AT_PH_IsPlmnValid(plmnId) == VOS_FALSE) {
        AT_ERR_LOG("AT_CheckDplmnPara: dplmn is invalid");
        return AT_ERROR;
    }
    return AT_OK;
}

VOS_UINT32 AT_ParseDplmnStringList(VOS_UINT8 indexNum, VOS_UINT8 *dplmnString, TAF_MMA_DplmnInfoSet *dplmnInfoSet)
{
    VOS_UINT8  *subStr     = VOS_NULL_PTR;
    VOS_CHAR   *nextSubStr = VOS_NULL_PTR;
    VOS_UINT8   dilms[]    = ",";
    VOS_UINT16  subStrLen;
    VOS_UINT16  indexValue = 0;
    VOS_UINT32  dplmnNum   = 0;
    VOS_UINT32  simRat     = 0;
    errno_t     memResult;
    TAF_PLMN_Id plmnId = {0};

    subStr     = (VOS_UINT8 *)strtok_s((VOS_CHAR *)dplmnString, (const VOS_CHAR *)dilms, &nextSubStr);

    /* ����Ԥ��Dplmn & SimRat��ϵĸ��� */
    if (subStr != VOS_NULL_PTR) {
        subStrLen = (VOS_UINT16)AT_STRLEN((VOS_CHAR *)subStr);
        if (AT_String2Hex((TAF_UINT8 *)subStr, subStrLen, &dplmnNum) == VOS_ERR) {
            AT_ERR_LOG("AT_ParseDplmnStringList: dplmn mnc error");
            return AT_ERROR;
        }
        subStr = (VOS_UINT8 *)strtok_s(VOS_NULL_PTR, (const VOS_CHAR *)dilms, &nextSubStr);
    }

    dplmnInfoSet->dplmnNum = (VOS_UINT16)dplmnNum;

    if (subStr == VOS_NULL_PTR) {
        dplmnInfoSet->dplmnNum = 0;
    }

    while (subStr != VOS_NULL_PTR) {
        subStrLen = (VOS_UINT16)AT_STRLEN((VOS_CHAR *)subStr);

        if (AT_CheckDplmnPara(&plmnId, subStr, subStrLen) == AT_ERROR) {
            AT_ERR_LOG("AT_ParseDplmnStringList: dplmn error");
            return AT_ERROR;
        }

        /* Dplmn Sim Rat */
        subStr = (VOS_UINT8 *)strtok_s(VOS_NULL_PTR, (const VOS_CHAR *)dilms, &nextSubStr);

        if (subStr == VOS_NULL_PTR) {
            AT_ERR_LOG("AT_ParseDplmnStringList: no sim rat");
            return AT_ERROR;
        }

        subStrLen = (VOS_UINT16)AT_STRLEN((VOS_CHAR *)subStr);

        if (AT_String2Hex(subStr, subStrLen, &simRat) == VOS_ERR) {
            AT_ERR_LOG("AT_ParseDplmnStringList: parse sim rat error");
            return AT_ERROR;
        }

        /* APԤ�õ�Dplmn��SimratΪ��Чֵ */
        if ((simRat == AT_AP_PRESET_DPLMN_INVALID_RAT) || (simRat > AT_AP_PRESET_DPLMN_ALL_RAT)) {
            AT_ERR_LOG("AT_ParseDplmnStringList: sim rat invalid");
            return AT_ERROR;
        }

        memResult = memcpy_s(&(dplmnInfoSet->dplmnList[indexValue].plmnId),
                             sizeof(dplmnInfoSet->dplmnList[indexValue].plmnId), &plmnId, sizeof(TAF_PLMN_Id));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(dplmnInfoSet->dplmnList[indexValue].plmnId), sizeof(TAF_PLMN_Id));
        dplmnInfoSet->dplmnList[indexValue].simRat = (VOS_UINT16)simRat;
        indexValue++;

        if (dplmnInfoSet->dplmnNum <= indexValue) {
            break;
        }

        subStr = (VOS_UINT8 *)strtok_s(VOS_NULL_PTR, (const VOS_CHAR *)dilms, &nextSubStr);
    }

    dplmnInfoSet->dplmnNum = AT_MIN(dplmnInfoSet->dplmnNum, indexValue);

    return AT_SUCCESS;
}

VOS_UINT32 AT_ParseEhplmnStringList(VOS_UINT32 paraLen, VOS_UINT8 *para, VOS_UINT8 *ehPlmnNum, TAF_PLMN_Id *ehplmnList)
{
    TAF_PLMN_Id plmnId;
    VOS_UINT16  indexNum;
    VOS_UINT16  subStrLen;
    VOS_UINT8 ehplmnStr[TAF_MMA_MAX_EHPLMN_STR + 1] = {0};
    VOS_UINT8 *subStr     = VOS_NULL_PTR;
    VOS_CHAR  *nextSubStr = VOS_NULL_PTR;
    VOS_UINT8  dilms[]    = ",";
    VOS_UINT32 len = paraLen;
    errno_t    memResult;

    if (len > TAF_MMA_MAX_EHPLMN_STR) {
        len = TAF_MMA_MAX_EHPLMN_STR;
    }

    if (len > 0) {
        memResult = memcpy_s(ehplmnStr, sizeof(ehplmnStr), para, len);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(ehplmnStr), len);
    }

    subStr   = (VOS_UINT8 *)strtok_s((VOS_CHAR *)ehplmnStr, (const VOS_CHAR *)dilms, &nextSubStr);
    indexNum = 0;

    while (subStr != VOS_NULL_PTR) {
        subStrLen = (VOS_UINT16)AT_STRLEN((VOS_CHAR *)subStr);

        if ((subStrLen != AT_EHPLMN_LEN_FIVE) && (subStrLen != AT_EHPLMN_LEN_SIX)) {
            AT_ERR_LOG("AT_ParseEhplmnStringList: ehplmn len error");
            return AT_ERROR;
        }

        /* Ehplmn Mcc */
        if (AT_String2Hex(subStr, AT_MCC_LENGTH, &plmnId.mcc) == VOS_ERR) {
            AT_ERR_LOG("AT_ParseEhplmnStringList: ehplmn mcc error");
            return AT_ERROR;
        }

        /* Ehplmn Mnc */
        if (AT_String2Hex(&subStr[AT_SUB_STR_INDEX_3], subStrLen - AT_MCC_LENGTH, &plmnId.mnc) == VOS_ERR) {
            AT_ERR_LOG("AT_ParseEhplmnStringList: ehplmn mnc error");
            return AT_ERROR;
        }

        plmnId.mcc &= 0x0FFF;

        if (subStrLen == AT_EHPLMN_LEN_FIVE) {
            plmnId.mnc |= 0x0F00;
        }

        plmnId.mnc &= 0x0FFF;

        /* ����������Ehplmn����Ч��Plmn */
        if (AT_PH_IsPlmnValid(&plmnId) == VOS_FALSE) {
            AT_ERR_LOG("AT_ParseEhplmnStringList: plmn is invalid");
            return AT_ERROR;
        }

        memResult = memcpy_s(&(ehplmnList[indexNum]), sizeof(TAF_PLMN_Id), &plmnId, sizeof(TAF_PLMN_Id));
        indexNum++;
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(TAF_PLMN_Id), sizeof(TAF_PLMN_Id));

        if (*ehPlmnNum <= indexNum) {
            break;
        }

        subStr = (VOS_UINT8 *)strtok_s(VOS_NULL_PTR, (const VOS_CHAR *)dilms, &nextSubStr);
    }

    if (*ehPlmnNum > indexNum) {
        *ehPlmnNum = (VOS_UINT8)indexNum;
    }

    return AT_SUCCESS;
}

/*
 * ��������: ����PLMN:����Խ���EHPLMN��DPLMN
 */
LOCAL VOS_UINT32 AT_ProcPlmnStringList(VOS_UINT8 indexNum, VOS_UINT8 numDplmnSubStr,
    VOS_UINT8 *dplmnString, TAF_MMA_DplmnInfoSet *dplmnInfo)
{
    /* ���dplmn�ַ� */
    if (AT_DplmnFillDplmnString(numDplmnSubStr, dplmnString, dplmnInfo) == AT_ERROR) {
        return AT_ERROR;
    }

    /* Ehplmn���� */
    dplmnInfo->ehPlmnNum = (VOS_UINT8)g_atParaList[AT_DPLMNLIST_EHPLMNNUM].paraValue;
    if (AT_ParseEhplmnStringList(g_atParaList[AT_DPLMNLIST_EHPLMN].paraLen,
                                 g_atParaList[AT_DPLMNLIST_EHPLMN].para, &(dplmnInfo->ehPlmnNum),
                                 dplmnInfo->ehPlmnInfo) == AT_ERROR) {
        return AT_ERROR;
    }

    /* Dplmn���� */
    if (AT_ParseDplmnStringList(indexNum, dplmnString, dplmnInfo) == AT_ERROR) {
        return AT_ERROR;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_SetDplmnListPara(VOS_UINT8 indexNum)
{
    errno_t               memResult;
    VOS_UINT8             seq;
    VOS_UINT32            rst;
    VOS_UINT8             versionId[TAF_MMA_VERSION_INFO_LEN];
    TAF_MMA_DplmnInfoSet *dplmnInfo   = VOS_NULL_PTR;
    VOS_UINT8            *dplmnString = VOS_NULL_PTR;
    VOS_UINT8             numDplmnSubStr;

    rst = AT_DplmnListParaCheck();
    if (rst != AT_OK) {
        return rst;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DPLMNLIST_SET;

    /* �����ˮ�� */
    seq = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ���汾�� */
    memResult = memcpy_s(versionId, sizeof(versionId), g_atParaList[AT_DPLMNLIST_VER].para,
        g_atParaList[AT_DPLMNLIST_VER].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(versionId), g_atParaList[AT_DPLMNLIST_VER].paraLen);

    /* ����DPLMN INFO SET�ṹ�� */
    /*lint -save -e516 */
    dplmnInfo = (TAF_MMA_DplmnInfoSet *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(TAF_MMA_DplmnInfoSet));
    /*lint -restore */

    if (dplmnInfo == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetDplmnListPara: Memory malloc failed!");
        return AT_ERROR;
    }

    /* ����ռ佫��Ƭ��Dplmn String��ϳ�һ���������ַ��� */
    /*lint -save -e516 */
    dplmnString = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, TAF_MMA_MAX_STR_LEN * sizeof(VOS_UINT8));
    /*lint -restore */
    if (dplmnString == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetDplmnListPara: Memory malloc failed!");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dplmnInfo);
        /*lint -restore */
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(dplmnInfo, sizeof(TAF_MMA_DplmnInfoSet), 0x00, sizeof(TAF_MMA_DplmnInfoSet));

    (VOS_VOID)memset_s(dplmnString, TAF_MMA_MAX_STR_LEN * sizeof(VOS_UINT8), 0x00, TAF_MMA_MAX_STR_LEN * sizeof(VOS_UINT8));
    numDplmnSubStr = (VOS_UINT8)g_atParaList[AT_DPLMNLIST_BSCOUNT].paraValue;

    if ((g_atParaList[AT_DPLMNLIST_BS1].paraLen + g_atParaList[AT_DPLMNLIST_BS2].paraLen +
        g_atParaList[AT_DPLMNLIST_BS3].paraLen) > TAF_MMA_MAX_STR_LEN) {
        AT_ERR_LOG("AT_SetDplmnListPara: para len error!");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dplmnInfo);
        PS_MEM_FREE(WUEPS_PID_AT, dplmnString);
        /*lint -restore */
        return AT_ERROR;
    }

    /* ����PLMN�ַ�����Ϣ */
    if (AT_ProcPlmnStringList(indexNum, numDplmnSubStr, dplmnString, dplmnInfo) == AT_ERROR) {
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dplmnInfo);
        PS_MEM_FREE(WUEPS_PID_AT, dplmnString);
        /*lint -restore */
        return AT_ERROR;
    }

    rst = TAF_MMA_SetDplmnListReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, seq, versionId, dplmnInfo);

    if (rst != VOS_TRUE) {
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dplmnInfo);
        PS_MEM_FREE(WUEPS_PID_AT, dplmnString);
        /*lint -restore */
        return AT_ERROR;
    }
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, dplmnInfo);
    PS_MEM_FREE(WUEPS_PID_AT, dplmnString);
    /*lint -restore */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetFratIgnitionPara(VOS_UINT8 indexNum)
{
    VOS_UINT32               rst;
    AT_MTA_FratIgnitionUnit8 fratIgnitionSta;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ŀ��� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ucFratIgnitionSta ȡֵ���� */
    if ((g_atParaList[0].paraValue >= AT_MTA_FRAT_IGNITION_STATT_BUTT) || (g_atParaList[0].paraLen == 0)) {
        AT_WARN_LOG1("AT_SetFratIgnitionPara: para err", g_atParaList[0].paraValue);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    fratIgnitionSta = (AT_MTA_FratIgnitionUnit8)g_atParaList[0].paraValue;

    /* ������Ϣ ID_AT_MTA_FRAT_IGNITION_SET_REQ ��MTA��������Ϣ������(VOS_UINT8)g_atParaList[0].ulParaValue */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_FRAT_IGNITION_SET_REQ,
                                 (VOS_UINT8 *)&fratIgnitionSta, sizeof(fratIgnitionSta), I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FRATIGNITION_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("AT_SetFratIgnitionPara: send ReqMsg fail");
        return AT_ERROR;
    }
}

VOS_UINT32 TAF_MMA_BuildBorderInfo(VOS_UINT32 bsTotalLen, TAF_MMA_BorderInfo *borderInfo)
{
    errno_t    memResult;
    VOS_UINT32 offset;

    /* ��ˮ�� */
    borderInfo->seq = (VOS_UINT8)g_atParaList[1].paraValue;

    /* �汾�� */
    if (g_atParaList[AT_BORDERINFO_VER].paraLen > 0) {
        memResult = memcpy_s(borderInfo->versionId, TAF_MMA_VERSION_INFO_LEN, g_atParaList[AT_BORDERINFO_VER].para,
                             g_atParaList[AT_BORDERINFO_VER].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_VERSION_INFO_LEN, g_atParaList[AT_BORDERINFO_VER].paraLen);
    }

    /* EHPLMN��Ϣ */
    borderInfo->ehPlmnNum = (VOS_UINT8)g_atParaList[AT_BORDERINFO_EHPLMNNUM].paraValue;
    if (AT_ParseEhplmnStringList(g_atParaList[AT_BORDERINFO_EHPLMNLIST].paraLen,
                                 g_atParaList[AT_BORDERINFO_EHPLMNLIST].para, &(borderInfo->ehPlmnNum),
                                 borderInfo->ehPlmnList) == AT_ERROR) {
        AT_ERR_LOG("TAF_MMA_BuildBorderInfo: parse EHPLMN list failed!");
        return VOS_FALSE;
    }

    /* �߾���Ϣ */
    borderInfo->borderInfoLen = bsTotalLen;

    offset = 0;

    /* BS1 */
    if ((borderInfo->borderInfoLen > 0) && (g_atParaList[AT_BORDERINFO_BS1].paraLen > 0)) {
        memResult = memcpy_s(borderInfo->borderInfoBuff, borderInfo->borderInfoLen,
                             g_atParaList[AT_BORDERINFO_BS1].para, g_atParaList[AT_BORDERINFO_BS1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, borderInfo->borderInfoLen, g_atParaList[AT_BORDERINFO_BS1].paraLen);
    }

    offset += g_atParaList[AT_BORDERINFO_BS1].paraLen;

    /* BS1 δЯ�������еı߾���Ϣ��BS2 Я���˲�����Ϣ */
    if (bsTotalLen > offset) {

        if (g_atParaList[AT_BORDERINFO_BS2].paraLen > 0) {
            memResult = memcpy_s(borderInfo->borderInfoBuff + offset, g_atParaList[AT_BORDERINFO_BS2].paraLen,
                                 g_atParaList[AT_BORDERINFO_BS2].para, g_atParaList[AT_BORDERINFO_BS2].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, g_atParaList[AT_BORDERINFO_BS2].paraLen,
                                g_atParaList[AT_BORDERINFO_BS2].paraLen);
        }

        offset += g_atParaList[AT_BORDERINFO_BS2].paraLen;

        /* BS1 & BS2 δЯ�������еı߾���Ϣ��BS3 Я���˲�����Ϣ */
        if (bsTotalLen > offset) {

            if (g_atParaList[AT_BORDERINFO_BS3].paraLen > 0) {
                memResult = memcpy_s(borderInfo->borderInfoBuff + offset, g_atParaList[AT_BORDERINFO_BS3].paraLen,
                                     g_atParaList[AT_BORDERINFO_BS3].para, g_atParaList[AT_BORDERINFO_BS3].paraLen);
                TAF_MEM_CHK_RTN_VAL(memResult, g_atParaList[AT_BORDERINFO_BS3].paraLen,
                                    g_atParaList[AT_BORDERINFO_BS3].paraLen);
            }
        }
    }

    return VOS_TRUE;
}

VOS_UINT32 AT_SetBorderInfoPara(VOS_UINT8 indexNum)
{
    TAF_MMA_BorderInfo                   *borderInfo = VOS_NULL_PTR;
    VOS_UINT32                            bsTotalLen = 0;
    VOS_UINT32                            rslt;
    ModemIdUint16                         modemId = MODEM_ID_0;
    VOS_UINT8                             i;
    TAF_MMA_SetBorderInfoOperateTypeUint8 operateType;
    VOS_UINT8                             bsCount;

    rslt = AT_GetModemIdFromClient(indexNum, &modemId);

    if (rslt != VOS_OK) {
        AT_ERR_LOG("AT_SetBorderInfoPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� 0:���� 1:ɾ������ */
    operateType = (TAF_MMA_SetBorderInfoOperateTypeUint8)g_atParaList[AT_BORDERINFO_OPERTION_TYPE].paraValue;

    /* ������������VER���Ȳ���ȷ */
    if (operateType == TAF_MMA_SET_BORDER_INFO_OPERATE_TYPE_ADD) {
        if ((g_atParaIndex > AT_BORDERINFO_MAX_PARA_NUM) || (g_atParaIndex < AT_BORDERINFO_MIN_PARA_NUM) ||
            (g_atParaList[AT_BORDERINFO_VER].paraLen != MCC_INFO_VERSION_LEN)) {
            AT_ERR_LOG("AT_SetBorderInfoPara:number of parameter error.");
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ucBsCount����Ч��AT����Ѿ��кϷ��Լ�飬�˴����ټ�� */
        bsCount = (VOS_UINT8)g_atParaList[AT_BORDERINFO_COUNT].paraValue;

        for (i = 0; i < bsCount; i++) {
            bsTotalLen += (VOS_UINT32)g_atParaList[AT_BORDERINFO_BS1 + i].paraLen;
        }

    } else {
        bsTotalLen = TAF_MMA_BODER_INFO_BUFF_LEN;
    }

    /* ����TAF_MMA_BorderInfo�ṹ�� */
    /*lint -save -e516 */
    borderInfo = (TAF_MMA_BorderInfo *)PS_MEM_ALLOC(WUEPS_PID_AT,
        sizeof(TAF_MMA_BorderInfo) + bsTotalLen - TAF_MMA_BODER_INFO_BUFF_LEN);
    /*lint -restore */
    if (borderInfo == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetBorderInfoPara: Memory malloc failed!");
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(borderInfo, sizeof(TAF_MMA_BorderInfo) + bsTotalLen - TAF_MMA_BODER_INFO_BUFF_LEN, 0x00,
        sizeof(TAF_MMA_BorderInfo) + bsTotalLen - TAF_MMA_BODER_INFO_BUFF_LEN);

    /* ��д�߾���Ϣ�ṹ��:ɾ��ʱ����Ҫ��Щ��Ϣ */
    if (operateType == TAF_MMA_SET_BORDER_INFO_OPERATE_TYPE_ADD) {
        if (TAF_MMA_BuildBorderInfo(bsTotalLen, borderInfo) == VOS_FALSE) {
            /*lint -save -e516 */
            PS_MEM_FREE(WUEPS_PID_AT, borderInfo);
            /*lint -restore */
            return AT_ERROR;
        }
    }

    rslt = TAF_MMA_SetBorderInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, operateType, borderInfo);

    if (rslt != VOS_TRUE) {
        AT_ERR_LOG("AT_SetBorderInfoPara: snd msg failed!");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, borderInfo);
        /*lint -restore */
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_BORDERINFO_SET;
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, borderInfo);
    /*lint -restore */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetEmRssiRptPara(VOS_UINT8 indexNum)
{
    VOS_UINT8 emRssiRptSwitch;

    /* ����Ƿ��������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("At_SetEmRssiRptPara: Not Set Command!");
        return AT_ERROR;
    }

    /* ���������� */
    if (g_atParaIndex != 1) {
        AT_WARN_LOG("At_SetEmRssiRptPara: Input parameters go wrong!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    emRssiRptSwitch = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ������Ϣ��C�˴��� */
    if (TAF_MMA_SetEmRssiRptReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                &emRssiRptSwitch) == VOS_TRUE) {
        /* ������������״̬ */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EMRSSIRPT_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

LOCAL VOS_UINT32 AT_GetPlmnSrchRsrpPara(TAF_MMA_RatTypeUint8 rat, TAF_PLMN_UserSel *extUserPlmnSrch)
{
#if (FEATURE_LTE == FEATURE_ON)
    VOS_UINT32 rsrpTmp;

    if (g_atParaIndex <= AT_PLMNSRCH_RSRP) {
        return VOS_OK;
    }

    rsrpTmp = g_atParaList[AT_PLMNSRCH_RSRP].paraValue;
    switch (rat) {
        case TAF_MMA_RAT_LTE:
            if ((rsrpTmp < AT_PLMNSRCH_LTE_RSRP_THRESHOLD_MIN) ||
                (rsrpTmp > AT_PLMNSRCH_LTE_RSRP_THRESHOLD_MAX)) {
                AT_NORM_LOG1("AT_GetPlmnSrchRsrpPara: LTE rsrpthreshold incorrect!", rsrpTmp);
                return VOS_ERR;
            }
            extUserPlmnSrch->rsrpThresholdFlg = VOS_TRUE;
            extUserPlmnSrch->rsrp             = 0 - (VOS_INT16)rsrpTmp;
            break;
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        case TAF_MMA_RAT_NR:
            if ((rsrpTmp < AT_PLMNSRCH_NR_RSRP_THRESHOLD_MIN) ||
                (rsrpTmp > AT_PLMNSRCH_NR_RSRP_THRESHOLD_MAX)) {
                AT_NORM_LOG1("AT_GetPlmnSrchRsrpPara: NR rsrpthreshold incorrect!", rsrpTmp);
                return VOS_ERR;
            }
            extUserPlmnSrch->rsrpThresholdFlg = VOS_TRUE;
            extUserPlmnSrch->rsrp             = 0 - (VOS_INT16)rsrpTmp;
            break;
#endif
        default:
            AT_NORM_LOG1("AT_GetPlmnSrchRsrpPara: rat not support rsrpthreshold!", rat);
            break;
    }
#endif

    return VOS_OK;
}

VOS_UINT32 AT_SetPlmnSrchPara(VOS_UINT8 indexNum)
{
    TAF_MMA_RatTypeUint8  phRat;
    TAF_MMA_Ctrl           ctrl;
    TAF_PLMN_UserSel      extUserPlmnSrch;
    errno_t                     memResult;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(TAF_MMA_Ctrl));
    (VOS_VOID)memset_s(&extUserPlmnSrch, sizeof(extUserPlmnSrch), 0x00, sizeof(TAF_PLMN_UserSel));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_NORM_LOG("AT_SetPlmnSrchPara:  not AT_CMD_OPT_SET_PARA_CMD");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if ((g_atParaIndex != AT_PLMNSRCH_PARA_NUM) && (g_atParaIndex != AT_PLMNSRCH_MAX_PARA_NUM)) {
        AT_NORM_LOG1("AT_SetPlmnSrchPara: Param Number not correct:", g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��д�˿������Ϣ */
    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = g_atClientTab[indexNum].clientId;
    ctrl.opId     = 0;

    /* ����RAT */
    At_SetCopsActPara(&phRat, g_atParaList[AT_PLMNSRCH_RAT].paraLen, g_atParaList[AT_PLMNSRCH_RAT].paraValue);

    extUserPlmnSrch.formatType       = AT_COPS_NUMERIC_TYPE;
    extUserPlmnSrch.bgSrchFlag       = (VOS_UINT8)g_atParaList[AT_PLMNSRCH_SRCH_MODE_FLAG].paraValue;
    extUserPlmnSrch.operNameLen      = g_atParaList[AT_PLMNSRCH_PLMN_LEN].paraLen;
    extUserPlmnSrch.accessMode       = phRat;
    extUserPlmnSrch.srchType         = (VOS_UINT8)g_atParaList[AT_PLMNSRCH_SRCHTYPE].paraValue;
    extUserPlmnSrch.rsrpThresholdFlg = VOS_FALSE;

    if ((g_atParaList[AT_PLMNSRCH_PLMN_LEN].paraLen != AT_PLMN_LEN_FIVE) &&
        (g_atParaList[AT_PLMNSRCH_PLMN_LEN].paraLen != AT_PLMN_LEN_SIX)) {
        AT_NORM_LOG1("AT_SetPlmnSrchPara: PLMN Length Not correct:", g_atParaList[AT_PLMNSRCH_PLMN_LEN].paraLen);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    memResult = memcpy_s(extUserPlmnSrch.operName, TAF_MMA_PLMN_ID_LEN,
                         (VOS_CHAR *)g_atParaList[AT_PLMNSRCH_PLMN_LEN].para,
                         g_atParaList[AT_PLMNSRCH_PLMN_LEN].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_PLMN_ID_LEN, g_atParaList[AT_PLMNSRCH_PLMN_LEN].paraLen);

    if (AT_GetPlmnSrchRsrpPara(phRat, &extUserPlmnSrch) != VOS_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_MMA_PlmnSearchReq(&ctrl, TAF_MMA_SEARCH_TYPE_MANUAL, &extUserPlmnSrch) == VOS_TRUE) {
        AT_NORM_LOG("AT_SetPlmnSrchPara:  AT_CMD_PLMNSRCH_SET: ASYNC");
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PLMNSRCH_SET;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 At_SetDetectPlmnRsrp(TAF_UINT8 indexNum)
{
    TAF_MMA_RatTypeUint8 phRat;
    TAF_MMA_Ctrl         ctrl;
    TAF_DETECT_Plmn      plmnDetect;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(TAF_MMA_Ctrl));
    (VOS_VOID)memset_s(&plmnDetect, sizeof(plmnDetect), 0x00, sizeof(TAF_DETECT_Plmn));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_NORM_LOG("At_SetDtectPlmnRsrp:  not AT_CMD_OPT_SET_PARA_CMD");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != AT_DETECTPLMN_PARA_NUM) {
        AT_NORM_LOG1("At_SetDtectPlmnRsrp: Param Number not correct:", g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��д�˿������Ϣ */
    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = g_atClientTab[indexNum].clientId;
    ctrl.opId     = 0;

    /* ����RAT */
    At_SetCopsActPara(&phRat, g_atParaList[AT_DETECTPLMN_RAT].paraLen, g_atParaList[AT_DETECTPLMN_RAT].paraValue);

    plmnDetect.accessMode = phRat;

    if (phRat != TAF_MMA_RAT_LTE) {
        AT_NORM_LOG("At_SetDtectPlmnRsrp: Only support LTE");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[AT_DETECTPLMN_PLMN].paraLen != AT_PLMN_LEN_FIVE) &&
        (g_atParaList[AT_DETECTPLMN_PLMN].paraLen != AT_PLMN_LEN_SIX)) {
        AT_NORM_LOG1("At_SetDtectPlmnRsrp: PLMN Length Not correct:", g_atParaList[AT_DETECTPLMN_PLMN].paraLen);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* mcc mnc ���� */
    /* Mcc */
    if (AT_DigitString2Hex((TAF_UINT8 *)(g_atParaList[AT_DETECTPLMN_PLMN].para),
                           AT_MCC_LENGTH, &(plmnDetect.plmnId.mcc)) == VOS_FALSE) {
        AT_NORM_LOG("At_SetDtectPlmnRsrp: PLMN Illegal");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* Mnc */
    if (AT_DigitString2Hex((TAF_UINT8 *)&(g_atParaList[AT_DETECTPLMN_PLMN].para[3]),
                           g_atParaList[AT_DETECTPLMN_PLMN].paraLen - AT_MNC_LENGTH,
                           &(plmnDetect.plmnId.mnc)) == VOS_FALSE) {
        AT_NORM_LOG("At_SetDtectPlmnRsrp: PLMN Illegal");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    plmnDetect.plmnId.mcc |= 0xFFFFF000;
    /* mncһ������ֻռ����ֽڣ�Ҳ����4bit */
    plmnDetect.plmnId.mnc |= (0xFFFFFFFF << ((g_atParaList[AT_DETECTPLMN_PLMN].paraLen - AT_MNC_LENGTH) * 4));

    if (TAF_MMA_PlmnDetectReq(&ctrl, &plmnDetect) == VOS_TRUE) {
        AT_NORM_LOG("At_SetDtectPlmnRsrp:  AT_CMD_PLMNSRCH_SET: ASYNC");

        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PLMNSRCH_SET;

        return AT_OK;
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_DSDS == FEATURE_ON)
VOS_UINT32 AT_SetDsdsStatePara(VOS_UINT8 indexNum)
{
    ModemIdUint16 modemId = MODEM_ID_0;

    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        AT_ERR_LOG("AT_SetDsdsStatePara: Get modem id fail.");
        return AT_ERROR;
    }

    if (modemId != MODEM_ID_0) {
        return AT_ERROR;
    }

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_MMA_SetDsdsStateReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atParaList[0].paraValue) ==
        VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DSDS_STATE_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_UINT32 At_SetPsScenePara(TAF_UINT8 indexNum)
{
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    TAF_MMA_PsScenePara psSceneStru;
    VOS_UINT32          rslt;
    ModemIdUint16       modemId;

    modemId = MODEM_ID_BUTT;

    /* ͨ��clientID���ModemID */
    rslt = AT_GetModemIdFromClient(g_atClientTab[indexNum].clientId, &modemId);

    /* ���ModemID��ȡʧ�ܻ���Modem1������ʧ�� */
    if ((rslt != VOS_OK) || (modemId != MODEM_ID_1)) {
        AT_WARN_LOG1("At_SetPsScenePara: AT_GetModemIdFromClient failed or not modem1!modem id is,", modemId);
        return AT_ERROR;
    }

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (At_CheckCurrRatModeIsCL((VOS_UINT8)(g_atClientTab[indexNum].clientId)) == VOS_TRUE) {
        AT_WARN_LOG("At_SetPsScenePara: operation not allowed in CL mode!");
        return AT_CME_OPERATION_NOT_ALLOWED_IN_CL_MODE;
    }
#endif

    /* ������� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_PSSCENE_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* PSҵ��ʼʱ��ps_detach��Ǳ���Ϊfalse */
    if ((g_atParaList[0].paraValue == 1) && (g_atParaList[1].paraValue != 0)) {
        AT_WARN_LOG2("At_SetPsScenePara: paravalue is wrong,para0,para1,", g_atParaList[0].paraValue,
                     g_atParaList[1].paraValue);
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(&psSceneStru, sizeof(psSceneStru), 0x00, sizeof(psSceneStru));
    psSceneStru.psServiceState = (VOS_UINT8)g_atParaList[0].paraValue;
    psSceneStru.psDetachFlag   = (VOS_UINT8)g_atParaList[1].paraValue;

    if (TAF_MMA_SetPsSceneReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &psSceneStru) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PSSCENE_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
#else
    return AT_ERROR;
#endif
}

VOS_UINT32 AT_SetSmsAntiAttackState(VOS_UINT8 indexNum)
{
    VOS_UINT32                      result;
    TAF_MMA_SmsAntiAttackStateUint8 state;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (At_CheckCurrRatModeIsCL((VOS_UINT8)(g_atClientTab[indexNum].clientId)) == VOS_TRUE) {
        AT_WARN_LOG("AT_SetSmsAntiAttackState: operation not allowed in CL mode!");
        return AT_CME_OPERATION_NOT_ALLOWED_IN_CL_MODE;
    }
#endif

    /* ����������Ϊ1�����ַ������Ȳ�Ϊ1 */
    if ((g_atParaIndex != 1) || (g_atParaList[0].paraLen != 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ0����1����ǰֻ֧��������ֵ */
    if (g_atParaList[0].paraValue > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    state = (TAF_MMA_SmsAntiAttackStateUint8)g_atParaList[0].paraValue;

    /* ���Ϳ����Ϣ��C��, ���÷�����״̬ */

    result = TAF_MMA_SetSmsAntiAttackState(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                           state);

    if (result != VOS_TRUE) {
        AT_WARN_LOG("AT_SetSmsAntiAttackState: AT_SetSmsAntiAttackState fail.");
        return AT_ERROR;
    }

    /* ���ͳɹ������õ�ǰ����ģʽ */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SMSANTIATTACK_SET;

    /* �ȴ��첽����ʱ�䷵�� */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetSmsAntiAttackCapQrypara(TAF_UINT8 indexNum)
{
    VOS_UINT8 antiSmsAttacktype;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* ����������Ϊ1�����ַ������Ȳ�Ϊ1 */
    if ((g_atParaIndex != 1) || (g_atParaList[0].paraLen != 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraValue > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (At_CheckCurrRatModeIsCL((VOS_UINT8)(g_atClientTab[indexNum].clientId)) == VOS_TRUE) {
        AT_WARN_LOG("AT_SetSmsAntiAttackCapQrypara: operation not allowed in CL mode.");
        return AT_CME_OPERATION_NOT_ALLOWED_IN_CL_MODE;
    }
#endif

    antiSmsAttacktype = (VOS_UINT8)g_atParaList[0].paraValue;

    if (TAF_MMA_QrySmsAntiAttackCapReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                       antiSmsAttacktype) != VOS_TRUE) {
        AT_WARN_LOG("AT_SetSmsAntiAttackCapQrypara: send msg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SMSANTIATTACKCAP_QRY;
    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 At_SetScidPara(TAF_UINT8 indexNum)
{
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_ERROR;
    }

    /* ִ��������� */
    if (Taf_ParaQuery(g_atClientTab[indexNum].clientId, 0, TAF_PH_ICC_ID, TAF_NULL_PTR) == AT_SUCCESS) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_ICCID_READ;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_LTE == FEATURE_ON)
LOCAL VOS_UINT32 AT_CheckCacdcPara(VOS_VOID)
{
    /* ������Ч�Լ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_CheckCacdcPara: At Cmd Opt Set Para Error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������ΪTAF_AT_EOPLMN_PARA_NUM������AT_CME_INCORRECT_PARAMETERS */
    if (g_atParaIndex != TAF_MMA_ACDC_PARA_NUM) {
        AT_WARN_LOG("AT_CheckCacdcPara: At Para Num Error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��1��������飬���Ȳ�����TAF_MMA_OSID_ORIGINAL_LEN�����򷵻�AT_CME_INCORRECT_PARAMETERS */
    if (g_atParaList[0].paraLen != TAF_MMA_OSID_ORIGINAL_LEN) {
        AT_WARN_LOG1("AT_CheckCacdcPara: OsId Error.", g_atParaList[0].paraLen);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��2��������飬���ȴ���0С�ڵ���TAF_MMA_MAX_APPID_LEN������AT_CME_INCORRECT_PARAMETERS */
    if ((g_atParaList[1].paraLen == 0) || (g_atParaList[1].paraLen > TAF_MMA_MAX_APPID_LEN)) {
        AT_WARN_LOG1("AT_CheckCacdcPara: AppId Error.", g_atParaList[1].paraLen);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_SetCacdcPara(VOS_UINT8 indexNum)
{
    TAF_MMA_AcdcAppInfo acdcAppInfo;
    VOS_UINT8           osId[TAF_MMA_OSID_ORIGINAL_LEN];
    VOS_UINT16          len = TAF_MMA_OSID_ORIGINAL_LEN;
    VOS_UINT32          result;
    errno_t             memResult;

    (VOS_VOID)memset_s(&acdcAppInfo, sizeof(acdcAppInfo), 0x00, sizeof(acdcAppInfo));
    (VOS_VOID)memset_s(osId, sizeof(osId), 0x00, sizeof(osId));

    /* ���������ͺϷ��Լ��,���Ϸ�ֱ�ӷ���ʧ�� */
    result = AT_CheckCacdcPara();
    if (result != AT_SUCCESS) {
        return result;
    }

    memResult = memcpy_s(osId, TAF_MMA_OSID_ORIGINAL_LEN, g_atParaList[0].para, TAF_MMA_OSID_ORIGINAL_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_OSID_ORIGINAL_LEN, TAF_MMA_OSID_ORIGINAL_LEN);

    /* ��д��Ϣ�ṹ */
    /* ��дOSID */
    result = At_AsciiNum2HexString(osId, &len);
    if ((result != AT_SUCCESS) || (len != TAF_MMA_OSID_LEN)) {
        AT_ERR_LOG1("AT_SetCacdcPara: Ascii to Hex Error", len);
        return AT_ERROR;
    }

    memResult = memcpy_s(acdcAppInfo.osId, TAF_MMA_OSID_LEN, osId, TAF_MMA_OSID_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_OSID_LEN, TAF_MMA_OSID_LEN);

    /* ��дAPPID */
    memResult = strncpy_s((VOS_CHAR *)acdcAppInfo.appId, TAF_MMA_MAX_APPID_LEN + 1, (VOS_CHAR *)g_atParaList[1].para,
                          g_atParaList[1].paraLen);
    TAF_STRCPY_CHK_RTN_VAL_CONTINUE(memResult, TAF_MMA_MAX_APPID_LEN + 1, g_atParaList[1].paraLen);

    /* ��дenAppIndication */
    acdcAppInfo.appIndication = g_atParaList[AT_CACDC_ENAPPINDICATION].paraValue;

    /* ͨ��ID_TAF_MMA_ACDC_APP_NOTIFY��Ϣ������ACDC */
    result = TAF_MMA_AcdcAppNotify(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, At_GetOpId(), &acdcAppInfo);

    if (result == VOS_TRUE) {
        return AT_OK;
    }

    return AT_ERROR;
}
#endif

VOS_UINT32 AT_CheckOosSrchStgyPara(VOS_VOID)
{
    /*
     * ����У��,�����������У��ʧ��:
     * 1���� set ���
     * 2��������������1��2
     * 3������1ֵ���� 0��1
     * 4������1ֵΪ0ʱ������������Ϊ1
     */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_NORM_LOG("AT_CheckOosSrchStgyPara:  not AT_CMD_OPT_SET_PARA_CMD");
        return AT_ERROR;
    }

    if ((g_atParaIndex != 1) && (g_atParaIndex != 2)) {
        AT_NORM_LOG1("AT_CheckOosSrchStgyPara: Param Number not 1 or 2:", g_atParaIndex);
        return AT_ERROR;
    }

    if ((g_atParaList[0].paraValue != 0) && (g_atParaList[0].paraValue != 1)) {
        AT_NORM_LOG1("AT_CheckOosSrchStgyPara: Para1 value not correct:", g_atParaList[0].paraValue);
        return AT_ERROR;
    }

    if ((g_atParaList[0].paraValue == 0) && (g_atParaIndex != 1)) {
        AT_NORM_LOG1("AT_CheckOosSrchStgyPara: Para1 is 0, but para num not correct:", g_atParaIndex);
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_DecodeOosSrchStgySceneInfo(TAF_MMA_SetOosSrchStrategyPara *srchStrategy, VOS_UINT32 paraIndex)
{
    VOS_UINT32 indexNum = paraIndex;
    VOS_UINT32 i;

    for (i = 0; i < srchStrategy->strategyNum; i++) {
        /* ��ȡ����id */
        if (g_atParaList[1].para[indexNum] >= TAF_MMA_STRATEGY_SCENE_BUTT) {
            AT_NORM_LOG("AT_DecodeOosSrchStgySceneInfo: Scene not support");
            return AT_ERROR;
        }

        srchStrategy->strategyInfo[i].stragetyScene = g_atParaList[1].para[indexNum];

        /* ��ȡ������OOS�Ĳ������� */
        indexNum++;

        /* һ�׶β���ֵ */
        srchStrategy->strategyInfo[i].strategyCfg[0].totalTimerLen =
            (g_atParaList[1].para[indexNum] << AT_BITS_OF_ONE_BYTE) + g_atParaList[1].para[indexNum + 1];
        indexNum += AT_OOS_SRCH_STGY_TIMER_LEN_PARA_LEN;
        srchStrategy->strategyInfo[i].strategyCfg[0].sleepTimerLen =
            (g_atParaList[1].para[indexNum] << AT_BITS_OF_ONE_BYTE) + g_atParaList[1].para[indexNum + 1];
        indexNum += AT_OOS_SRCH_STGY_TIMER_LEN_PARA_LEN;
        srchStrategy->strategyInfo[i].strategyCfg[0].historyNum = g_atParaList[1].para[indexNum++];
        srchStrategy->strategyInfo[i].strategyCfg[0].prefBandNum = g_atParaList[1].para[indexNum++];
        srchStrategy->strategyInfo[i].strategyCfg[0].fullBandNum = g_atParaList[1].para[indexNum++];

        /* ���׶β���ֵ */
        srchStrategy->strategyInfo[i].strategyCfg[1].totalTimerLen =
            (g_atParaList[1].para[indexNum] << AT_BITS_OF_ONE_BYTE) + g_atParaList[1].para[indexNum + 1];
        indexNum += AT_OOS_SRCH_STGY_TIMER_LEN_PARA_LEN;
        srchStrategy->strategyInfo[i].strategyCfg[1].sleepTimerLen =
            (g_atParaList[1].para[indexNum] << AT_BITS_OF_ONE_BYTE) + g_atParaList[1].para[indexNum + 1];
        indexNum += AT_OOS_SRCH_STGY_TIMER_LEN_PARA_LEN;
        srchStrategy->strategyInfo[i].strategyCfg[1].historyNum = g_atParaList[1].para[indexNum++];
        srchStrategy->strategyInfo[i].strategyCfg[1].prefBandNum = g_atParaList[1].para[indexNum++];
        srchStrategy->strategyInfo[i].strategyCfg[1].fullBandNum = g_atParaList[1].para[indexNum++];
    }

    return AT_OK;
}

VOS_UINT32 AT_DecodeOosSrchStgyPara(TAF_MMA_SetOosSrchStrategyPara *srchStrategy)
{
    VOS_UINT32 paraIndex = 0;
    VOS_UINT32 paraLength;

    if (g_atParaList[0].paraValue == 0) {
        /* ��������������� */
        srchStrategy->strategyMode = TAF_MMA_STRATEGY_MODE_CLEAR;
        return AT_OK;
    } else if (g_atParaList[0].paraValue == 1) {
        /* ������������ */
        srchStrategy->strategyMode = TAF_MMA_STRATEGY_MODE_SET;
    }

    /* ���ַ���ת��Ϊ16�������� */
    if (At_AsciiNum2HexString(g_atParaList[1].para, &g_atParaList[1].paraLen) == AT_FAILURE) {
        AT_ERR_LOG("AT_SetOosSrchStgyPara: At_AsciiNum2HexString fail.");
        return AT_ERROR;
    }

    /*
     * �������󣬷���ʧ��
     * 1.ÿ����¼������Ϊ15�ֽڣ�numռ1���ֽڣ����Բ������Ȳ�Ϊ (15 * Num+1),
     * 2.��¼������ 1-10 ��Χ�ڣ�
     * 3.������ʶ��
     */
    srchStrategy->strategyNum = g_atParaList[1].para[paraIndex++];
    if ((srchStrategy->strategyNum > TAF_MMA_SET_OOS_SCENE_MAX_NUM) ||
        (srchStrategy->strategyNum > TAF_MMA_STRATEGY_SCENE_BUTT) ||
        (srchStrategy->strategyNum < 1)) {
        AT_ERR_LOG1("AT_SetOosSrchStgyPara: RecordNum out of range, ", srchStrategy->strategyNum);
        return AT_ERROR;
    }
    paraLength = AT_OOS_RECORD_LEN * srchStrategy->strategyNum + 1;
    if (paraLength != g_atParaList[1].paraLen) {
        AT_ERR_LOG1("AT_SetOosSrchStgyPara: paraLength is not correct, ", g_atParaList[1].paraLen);
        return AT_ERROR;
    }

    /* data */
    return AT_DecodeOosSrchStgySceneInfo(srchStrategy, paraIndex);
}

VOS_UINT32 AT_SetOosSrchStgyPara(VOS_UINT8 indexNum)
{
    TAF_MMA_SetOosSrchStrategyPara      srchStrategy;
    TAF_MMA_Ctrl                        ctrl;

    /* ������� */
    if (AT_CheckOosSrchStgyPara() == AT_ERROR) {
        AT_ERR_LOG("AT_SetOosSrchStgyPara: check para fail.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&ctrl, sizeof(TAF_MMA_Ctrl), 0x00, sizeof(TAF_MMA_Ctrl));
    (VOS_VOID)memset_s(&srchStrategy, sizeof(srchStrategy), 0x00, sizeof(TAF_MMA_SetOosSrchStrategyPara));

    /* ��д�˿������Ϣ */
    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = g_atClientTab[indexNum].clientId;
    ctrl.opId     = 0;

    /* �������� */
    if (AT_DecodeOosSrchStgyPara(&srchStrategy) == AT_ERROR) {
        AT_ERR_LOG("AT_SetOosSrchStgyPara: decode fail.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϣ */
    if (TAF_MMA_SndOosSrchStrategySetInd(&ctrl, &srchStrategy) == VOS_TRUE) {
        AT_NORM_LOG("AT_SetOosSrchStgyPara:  AT_CMD_OOSSRCHSTGY: ASYNC");
        return AT_OK;
    }

    return AT_ERROR;
}

VOS_UINT32 AT_SetCarOosGearsPara(VOS_UINT8 indexNum)
{
    TAF_MMA_SetCarOosGearsNtf           carOosGearsPara;
    TAF_MMA_Ctrl                        ctrl;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_NORM_LOG("AT_SetCarOosGearsPara:  not AT_CMD_OPT_SET_PARA_CMD");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex != 1) {
        AT_NORM_LOG1("AT_SetCarOosGearsPara: Param Number not 1 ", g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&ctrl, sizeof(TAF_MMA_Ctrl), 0x00, sizeof(TAF_MMA_Ctrl));
    (VOS_VOID)memset_s(&carOosGearsPara, sizeof(carOosGearsPara), 0x00, sizeof(TAF_MMA_SetCarOosGearsNtf));

    /* ��д�˿������Ϣ */
    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = g_atClientTab[indexNum].clientId;
    ctrl.opId     = 0;

    /* �������� */
    carOosGearsPara.carGears = g_atParaList[0].paraValue;

    /* ������Ϣ */
    if (TAF_MMA_SndCarOosGearsSetNtf(&ctrl, &carOosGearsPara) == VOS_TRUE) {
        AT_NORM_LOG("AT_SetCarOosGearsPara:  AT_CMD_CAROOSGEARS: ASYNC");
        return AT_OK;
    }

    return AT_ERROR;
}

TAF_UINT32 At_SetAutoAttach(TAF_UINT8 indexNum)
{
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex > 1) { /* �������� */
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraValue > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_MMA_SetAutoAttachReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, g_atParaList[0].paraValue) ==
        VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CAATT_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_IMS == FEATURE_ON)
LOCAL VOS_VOID AT_FillGulImsSwitchPara(TAF_NV_ImsSwitchRatRelationCfg *imsSwitchRatRelationCfg,
    TAF_MMA_ImsSwitchInfo *imsSwitchInfo)
{
    imsSwitchInfo->imsSwitchRat.lteEnable = (g_atParaList[AT_IMSSWITCH_LTE_ENABLE].paraValue ==
                                            TAF_MMA_IMS_SWITCH_SET_ON) ?
                                            TAF_MMA_IMS_SWITCH_SET_ON :
                                            TAF_MMA_IMS_SWITCH_SET_OFF;

    if (g_atParaIndex > AT_IMSSWITCH_UTRAN_ENABLE) {
        imsSwitchInfo->imsSwitchRat.utranEnable = ((g_atParaList[AT_IMSSWITCH_UTRAN_ENABLE].paraLen > 0) &&
                                                  (g_atParaList[AT_IMSSWITCH_UTRAN_ENABLE].paraValue) ==
                                                  TAF_MMA_IMS_SWITCH_SET_ON) ?
                                                  TAF_MMA_IMS_SWITCH_SET_ON :
                                                  TAF_MMA_IMS_SWITCH_SET_OFF;
    } else {
        if (imsSwitchRatRelationCfg->utranRelationLteFlg == VOS_TRUE) {
            imsSwitchInfo->imsSwitchRat.utranEnable = imsSwitchInfo->imsSwitchRat.lteEnable;
        }
    }

    if (g_atParaIndex > AT_IMSSWITCH_GSM_ENABLE) {
        imsSwitchInfo->imsSwitchRat.gsmEnable = ((g_atParaList[AT_IMSSWITCH_GSM_ENABLE].paraLen > 0) &&
                                                (g_atParaList[AT_IMSSWITCH_GSM_ENABLE].paraValue) ==
                                                TAF_MMA_IMS_SWITCH_SET_ON) ? TAF_MMA_IMS_SWITCH_SET_ON :
                                                TAF_MMA_IMS_SWITCH_SET_OFF;
    } else {
        if (imsSwitchRatRelationCfg->gsmRelationLteFlg == VOS_TRUE) {
            imsSwitchInfo->imsSwitchRat.gsmEnable = imsSwitchInfo->imsSwitchRat.lteEnable;
        }
    }
}

VOS_UINT32 AT_SetImsSwitchPara(VOS_UINT8 indexNum)
{
    VOS_UINT32                     rst;
    TAF_MMA_ImsSwitchInfo          imsSwitchInfo;
    TAF_NV_ImsSwitchRatRelationCfg imsSwitchRatRelationCfg;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex > AT_IMSSWITCH_PARA_MAX_NUM) {
        return AT_TOO_MANY_PARA;
    }

    /* ����Ϊ�� */
    if (g_atParaList[AT_IMSSWITCH_LTE_ENABLE].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&imsSwitchInfo, sizeof(imsSwitchInfo), 0, sizeof(TAF_MMA_ImsSwitchInfo));
    (VOS_VOID)memset_s(&imsSwitchRatRelationCfg, sizeof(imsSwitchRatRelationCfg), 0, sizeof(TAF_NV_ImsSwitchRatRelationCfg));

    imsSwitchInfo.bitOpImsSwitch = VOS_TRUE;
    imsSwitchInfo.imsSwitchRat.imsSwitchType = TAF_MMA_IMS_SWITCH_TYPE_GUL;

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_IMS_SWITCH_RAT_RELATION_CFG, &imsSwitchRatRelationCfg,
                          sizeof(imsSwitchRatRelationCfg)) != NV_OK) {
        AT_WARN_LOG("AT_SetImsSwitchPara: Read NV fail!");
        (VOS_VOID)memset_s(&imsSwitchRatRelationCfg, sizeof(imsSwitchRatRelationCfg), 0, sizeof(imsSwitchRatRelationCfg));
    }

    AT_FillGulImsSwitchPara(&imsSwitchRatRelationCfg, &imsSwitchInfo);

    rst = TAF_MMA_SetImsSwitchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                  &imsSwitchInfo);
    if (rst != VOS_TRUE) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_GUL_IMS_SWITCH_SET;

    return AT_WAIT_ASYNC_RETURN;
}

#endif

VOS_UINT32 AT_SetQuickCardSwitchPara(VOS_UINT8 indexNum)
{
    AT_MTA_SetQuickCardSwitchReq quickCardSwitchInfo;
    TAF_MMA_IsSamePlmnFlagUint8  isSamePlmnInfo;
    VOS_UINT32                   rst;

    (VOS_VOID)memset_s(&quickCardSwitchInfo, sizeof(AT_MTA_SetQuickCardSwitchReq), 0x00, sizeof(AT_MTA_SetQuickCardSwitchReq));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    quickCardSwitchInfo.isSameFlag = (VOS_UINT8)g_atParaList[0].paraValue;
    isSamePlmnInfo = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ֪ͨMMA˫���л�������Ϣ */
    TAF_MMA_CardQuickSwitchNotify(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, isSamePlmnInfo);

    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_QUICK_CARD_SWITCH_REQ,
                                 (VOS_UINT8 *)&quickCardSwitchInfo, sizeof(AT_MTA_SetQuickCardSwitchReq), I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_QUICK_CARD_SWITCH_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_FormatNumericPlmnStr2PlmnId(TAF_PLMN_Id *plmn, VOS_UINT16 oprNameLen, VOS_CHAR *oprName)
{
    if (AT_DigitString2Hex((VOS_UINT8 *)oprName, AT_MBMS_MCC_LENGTH, &plmn->mcc) == VOS_FALSE) {
        return VOS_FALSE;
    }

    if (AT_DigitString2Hex((VOS_UINT8 *)&oprName[AT_MBMS_MCC_LENGTH], oprNameLen - AT_MBMS_MNC_MAX_LENGTH,
                           &plmn->mnc) == VOS_FALSE) {
        return VOS_FALSE;
    }

    plmn->mcc |= 0xFFFFF000;
    /* mncһ������ֻռ����ֽڣ�Ҳ����4bit */
    plmn->mnc |= (0xFFFFFFFF << ((oprNameLen - AT_MBMS_MNC_MAX_LENGTH) * 4));

    return VOS_TRUE;
}

#if (FEATURE_CSG == FEATURE_ON)
VOS_UINT32 AT_SetCsgIdSearchPara(VOS_UINT8 indexNum)
{
    /* �������at��������Ƿ�Ϸ����Ϸ�֪ͨmma */
    TAF_MMA_CsgSpecSearchInfo csgIdUserSel;
    VOS_UINT32                ratType;

    csgIdUserSel.plmnId.mcc = TAF_MMA_INVALID_MCC;
    csgIdUserSel.plmnId.mnc = TAF_MMA_INVALID_MNC;
    csgIdUserSel.csgId      = TAF_MMA_INVALID_CSG_ID_VALUE;
    csgIdUserSel.ratType    = TAF_MMA_RAT_BUTT;
    ratType                 = TAF_MMA_RAT_BUTT;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��������3�� */
    if (g_atParaIndex != AT_CSGIDSRCH_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* @oper���ȷǷ� */
    if ((g_atParaList[AT_CSGIDSRCH_PLMN_ID].paraLen != AT_CSGIDSRCH_PLMN_ID_VAILD_LEN1) &&
        (g_atParaList[AT_CSGIDSRCH_PLMN_ID].paraLen != AT_CSGIDSRCH_PLMN_ID_VAILD_LEN2)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* CSG ID���ȷǷ� */
    if ((g_atParaList[AT_CSGIDSRCH_CSG_ID].paraLen == 0) ||
        (g_atParaList[AT_CSGIDSRCH_CSG_ID].paraLen > AT_CSGIDSRCH_CSG_ID_LEN_MAX)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (AT_String2Hex(g_atParaList[AT_CSGIDSRCH_RAT].para,
                      g_atParaList[AT_CSGIDSRCH_RAT].paraLen, &ratType) != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ָ���Ľ��뼼����ΪLTE��Ŀǰ�Ľ׶�ֻ֧��ָ����LTE��CSG ID���������֧���������뼼����ɾ�����жϼ��� */
    if ((ratType != AT_COPS_RAT_LTE)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<AcT> */
    At_SetCopsActPara(&csgIdUserSel.ratType, g_atParaList[AT_CSGIDSRCH_RAT].paraLen, ratType);

    /* ����PLMN ID */
    if (AT_FormatNumericPlmnStr2PlmnId(&csgIdUserSel.plmnId, g_atParaList[AT_CSGIDSRCH_PLMN_ID].paraLen,
                                       (VOS_CHAR *)g_atParaList[AT_CSGIDSRCH_PLMN_ID].para) != VOS_TRUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����CSG ID */
    if (AT_String2Hex(g_atParaList[AT_CSGIDSRCH_CSG_ID].para, g_atParaList[AT_CSGIDSRCH_CSG_ID].paraLen,
                      &csgIdUserSel.csgId) != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_MMA_SetCsgIdSearch(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                               &csgIdUserSel) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSG_SPEC_SEARCH;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    }

    return AT_ERROR;
}
#endif


#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
VOS_UINT32 At_SetUsimStub(VOS_UINT8 indexNum)
{
    TAF_MMA_UsimStubSetReq usimStub;
    VOS_UINT32             ef;
    VOS_UINT32             totalNum;
    VOS_UINT32             efId;
    VOS_UINT32             efLen;
    VOS_UINT32             apptype;
    VOS_UINT8              i;
    VOS_UINT8              num;
#if (VOS_WIN32 != VOS_OS_VER)
    ModemIdUint16                       modemID;
    TAF_NVIM_EnhancedOperNameServiceCfg operNameServiceCfg;
    VOS_UINT32                          ret;
#endif
    errno_t memResult;

    num      = 0;
    ef       = 0;
    totalNum = 0;
    apptype  = 0;
    efId     = 0;
    efLen    = 0;
    (VOS_VOID)memset_s(&usimStub, sizeof(usimStub), 0x00, sizeof(usimStub));

    /* ��ȡnv�nv��رգ�����ʧ�� */

#if (VOS_WIN32 != VOS_OS_VER)
    (VOS_VOID)memset_s(&operNameServiceCfg, sizeof(operNameServiceCfg), 0x00, sizeof(operNameServiceCfg));
    modemID = MODEM_ID_0;

    ret = AT_GetModemIdFromClient(indexNum, &modemID);

    if (ret != VOS_OK) {
        return AT_ERROR;
    }

    if (NV_OK != TAF_ACORE_NV_READ(modemID, NV_ITEM_ENHANCED_OPERATOR_NAME_SRV_CFG, &operNameServiceCfg,
                                   sizeof(TAF_NVIM_EnhancedOperNameServiceCfg))) {
        return AT_ERROR;
    }

    if (operNameServiceCfg.reserved5 == VOS_FALSE) {
        return AT_ERROR;
    }
#endif

    (VOS_VOID)At_String2Hex(g_atParaList[AT_USIMSTUB_CARD_ID].para, g_atParaList[AT_USIMSTUB_CARD_ID].paraLen, &efId);
    (VOS_VOID)At_String2Hex(g_atParaList[AT_USIMSTUB_CARD_TYPE].para, g_atParaList[AT_USIMSTUB_CARD_TYPE].paraLen, &apptype);
    (VOS_VOID)At_String2Hex(g_atParaList[AT_USIMSTUB_TOTALNUM].para,
                            g_atParaList[AT_USIMSTUB_TOTALNUM].paraLen, &totalNum);
    (VOS_VOID)At_String2Hex(g_atParaList[AT_USIMSTUB_EFLEN].para,
                            g_atParaList[AT_USIMSTUB_EFLEN].paraLen, &efLen);

    usimStub.totalNum = (VOS_UINT8)totalNum;
    usimStub.efId     = (VOS_UINT16)efId;
    usimStub.efLen    = (VOS_UINT16)efLen;
    usimStub.apptype  = (VOS_UINT8)apptype;

    for (i = 0; i < (g_atParaList[AT_USIMSTUB_FILECONTENT].paraLen / AT_HEX_ONEBYTE_VALID_LENTH); i++) {
        (VOS_VOID)At_String2Hex(g_atParaList[AT_USIMSTUB_FILECONTENT].para + AT_HEX_ONEBYTE_VALID_LENTH * i,
                                AT_HEX_ONEBYTE_VALID_LENTH, &ef);

        if (num >= TAF_MMA_MAX_EF_LEN) {
            return AT_ERROR;
        }

        memResult = memcpy_s(&usimStub.ef[num], TAF_MMA_MAX_EF_LEN - num, &ef, AT_CONST_NUM_1);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_MMA_MAX_EF_LEN - num, AT_CONST_NUM_1);
        num++;
    }

    TAF_SetUsimStub(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, &usimStub);

    return AT_OK;
}

VOS_UINT32 At_SetRefreshStub(VOS_UINT8 indexNum)
{
    TAF_MMA_RefreshStubSetReq refreshStub;
    VOS_UINT8                 i;
    VOS_UINT32                totalNum;
    VOS_UINT32                refreshFileType;
    VOS_UINT32                receivePid;
    VOS_UINT32                fileId;
#if (VOS_WIN32 != VOS_OS_VER)
    TAF_NVIM_EnhancedOperNameServiceCfg operNameServiceCfg;
    ModemIdUint16                       modemID;
    VOS_UINT32                          ret;
#endif

    totalNum        = 0;
    refreshFileType = 0;
    receivePid      = 0;
    fileId          = 0;
    (VOS_VOID)memset_s(&refreshStub, sizeof(refreshStub), 0x00, sizeof(refreshStub));

    /* ��ȡnv�nv��رգ�����ʧ�� */
#if (VOS_WIN32 != VOS_OS_VER)
    (VOS_VOID)memset_s(&operNameServiceCfg, sizeof(operNameServiceCfg), 0x00, sizeof(operNameServiceCfg));
    modemID = MODEM_ID_0;

    ret = AT_GetModemIdFromClient(indexNum, &modemID);

    if (ret != VOS_OK) {
        return AT_ERROR;
    }

    if (NV_OK != TAF_ACORE_NV_READ(modemID, NV_ITEM_ENHANCED_OPERATOR_NAME_SRV_CFG, &operNameServiceCfg,
                                   sizeof(TAF_NVIM_EnhancedOperNameServiceCfg))) {
        return AT_ERROR;
    }

    if (operNameServiceCfg.reserved5 == VOS_FALSE) {
        return AT_ERROR;
    }
#endif

    (VOS_VOID)At_String2Hex(g_atParaList[AT_REFRESHSTUB_RECEIVED_PID].para,
                            g_atParaList[AT_REFRESHSTUB_RECEIVED_PID].paraLen, &receivePid);
    (VOS_VOID)At_String2Hex(g_atParaList[AT_REFRESHSTUB_FILE_TYPE].para, g_atParaList[AT_REFRESHSTUB_FILE_TYPE].paraLen,
                            &refreshFileType);
    (VOS_VOID)At_String2Hex(g_atParaList[AT_REFRESHSTUB_TOTALNUM].para,
                            g_atParaList[AT_REFRESHSTUB_TOTALNUM].paraLen, &totalNum);

    refreshStub.receivePid      = receivePid;
    refreshStub.totalNum        = (VOS_UINT8)totalNum;
    refreshStub.refreshFileType = (VOS_UINT8)refreshFileType;

    if (refreshStub.totalNum > TAF_MMA_MAX_FILE_ID_NUM) {
        refreshStub.totalNum = TAF_MMA_MAX_FILE_ID_NUM;
    }

    for (i = 0; i < refreshStub.totalNum; i++) {
        (VOS_VOID)At_String2Hex(g_atParaList[i + AT_REFRESHSTUB_FILE_ID].para,
                                g_atParaList[i + AT_REFRESHSTUB_FILE_ID].paraLen, &fileId);
        refreshStub.efId[i] = (VOS_UINT16)fileId;
    }

    TAF_SetRefreshStub(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, &refreshStub);

    return AT_OK;
}

VOS_UINT32 At_SetAutoReselStub(VOS_UINT8 indexNum)
{
    TAF_MMA_AutoReselStubSetReq autoReselStub;

    (VOS_VOID)memset_s(&autoReselStub, sizeof(autoReselStub), 0x00, sizeof(autoReselStub));

    autoReselStub.activeFlg = (VOS_UINT8)g_atParaList[0].paraValue;

    TAF_SetAutoReselStub(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, &autoReselStub);

    return AT_OK;
}

TAF_UINT32 At_SetCmmPara(TAF_UINT8 indexNum)
{
    MM_TEST_AtCmdPara atCmd;

    /* �������� */
    if (g_atParaIndex > AT_CMM_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(MM_TEST_AtCmdPara));

    atCmd.cmd = (TAF_UINT8)g_atParaList[0].paraValue;
    for (atCmd.paraNum = 0; atCmd.paraNum < (g_atParaIndex - 1); atCmd.paraNum++) {
        atCmd.para[atCmd.paraNum] = g_atParaList[atCmd.paraNum + 1].paraValue;
    }

    if (TAF_MMA_SetCmmReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &atCmd) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MM_TEST_CMD_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetCLteRoamAllowPara(VOS_UINT8 indexNum)
{
    NAS_MMC_NvimLteInternationalRoamCfg nvimLteRoamAllowedFlg;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_ERROR;
    }

    nvimLteRoamAllowedFlg.lteRoamAllowedFlg = (VOS_UINT8)g_atParaList[0].paraValue;
    nvimLteRoamAllowedFlg.reserve[0]        = 0x00;

    /*
     * дNV:NV_ITEM_LTE_INTERNATION_ROAM_CONFIG,
     * ��NV�ĽṹΪ NAS_MMC_NvimLteInternationalRoamCfg, ֻд��1���ֽ�
     */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_LTE_INTERNATION_ROAM_CONFIG, (VOS_UINT8 *)&(nvimLteRoamAllowedFlg),
                           sizeof(VOS_UINT16)) != NV_OK) {
        AT_ERR_LOG("AT_SetCLteRoamAllowPara:Write NV fail");
        return AT_ERROR;
    } else {
        return AT_OK;
    }
}

#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
VOS_UINT32 AT_SetNrImsSwitchPara(VOS_UINT8 indexNum)
{
    VOS_UINT32                     rst;
    TAF_MMA_ImsSwitchInfo          imsSwitchInfo;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != AT_NRIMSSWITCH_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[AT_NRIMSSWITCH_ENABLE_INDEX_0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&imsSwitchInfo, sizeof(imsSwitchInfo), 0, sizeof(imsSwitchInfo));

    imsSwitchInfo.bitOpImsSwitch             = VOS_TRUE;
    imsSwitchInfo.imsSwitchRat.imsSwitchType = TAF_MMA_IMS_SWITCH_TYPE_NR;
    imsSwitchInfo.imsSwitchRat.nrEnable      =
        (TAF_MMA_ImsSwitchSetUint8)g_atParaList[AT_NRIMSSWITCH_ENABLE_INDEX_0].paraValue;

    rst = TAF_MMA_SetImsSwitchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                  &imsSwitchInfo);
    if (rst != VOS_TRUE) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NR_IMS_SWITCH_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_SetUlFreqRptPara(VOS_UINT8 indexNum)
{
    VOS_UINT8 mode;

    /* ָ�����ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_SetUlFreqPara : Current Option is not AT_CMD_OPT_SET_PARA_CMD.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���������򳤶Ȳ���ȷ */
    if (g_atParaIndex != 1) {
        AT_WARN_LOG("AT_SetUlFreqPara : The number of input parameters is error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��д��Ϣ */
    mode = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ������Ϣ��C�˴��� */
    if (TAF_MMA_SetUlFreqRptReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, mode) ==
        VOS_TRUE) {
        /* ������������״̬ */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_ULFREQRPT_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
 * ����˵��: ��������PLMN����������Ч
 * �������: eonsRequest: EONS��ѯ����
 * ���ؽ��: VOS_FALSE: ִ��ʧ��
 *           VOS_TRUE: ִ�гɹ�
 */
LOCAL VOS_UINT32 AT_CheckEonsPlmnParaValid(TafMmaEonsPlmnInfo *eonsPlmnInfo)
{
    VOS_UINT32 i;
    /* ���PLMN */
    if ((eonsPlmnInfo->plmnLen < TAF_PLMN_ID_LEN_5) || (eonsPlmnInfo->plmnLen > TAF_PLMN_ID_LEN_6)) {
        return VOS_FALSE;
    }
    /* ����ֵ���ַ�'0'~'9'֮�����Ϊ�ǺϷ��� */
    for (i = 0; i < eonsPlmnInfo->plmnLen; i++) {
        if ((g_atParaList[1].para[i] >= '0') && (g_atParaList[1].para[i] <= '9')) {
            eonsPlmnInfo->plmnId[i] = (VOS_CHAR)g_atParaList[1].para[i];
        } else {
            return VOS_FALSE;
        }
    }
    return VOS_TRUE;
}

/*
 * ����˵��: ���EONS����
 * �������: eonsRequest: EONS��ѯ����
 * ���ؽ��: VOS_FALSE: ִ��ʧ��
 *           VOS_TRUE: ִ�гɹ�
 */
LOCAL VOS_UINT32 AT_CheckEonsPara(VOS_VOID)
{
    /* �������࣬���ز������� */
    if ((g_atParaIndex > TAF_AT_EONS_MAX_PARA_NUM) || (g_atParaIndex == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��һ����������Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* EONS TypeΪ5ʱ�ڶ�������Plmn����Ϊ�� */
    if (g_atParaList[0].paraValue == TAF_EONS_TYPE_SPDI) {
        if (g_atParaList[1].paraLen != 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    } else {
        /* EONS TypeΪ5֮��Ĳ���ʱ����������������Ϊ�� */
        if (g_atParaList[TAF_AT_EONS_LEN_PARA_INDEX].paraLen != 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    return AT_SUCCESS;
}

/*
 * ����˵��: ���EONS���ṹ����
 * �������: eonsRequest: EONS��ѯ����
 * ���ؽ��: VOS_FALSE: ִ��ʧ��
 *           VOS_TRUE: ִ�гɹ�
 */
LOCAL VOS_UINT32 AT_FillEonsPara(TafMmaEonsRequestPara *eonsRequest)
{
    VOS_UINT32 plmnNameLen = 0;

    /* EONS TypeΪ5ʱ�ڶ�������Plmn����Ϊ�� */
    if (g_atParaList[0].paraValue == TAF_EONS_TYPE_SPDI) {
        /* ������δָ�����ֳ���ʱ��Ĭ�ϰ���20�ֽڴ��� */
        if (g_atParaList[TAF_AT_EONS_LEN_PARA_INDEX].paraLen == 0) {
            eonsRequest->maxNwNameLen = TAF_EONS_PLMN_NAME_DEFAULT_LEN;
        } else {
            if (At_Auc2ul(g_atParaList[TAF_AT_EONS_LEN_PARA_INDEX].para,
                    g_atParaList[TAF_AT_EONS_LEN_PARA_INDEX].paraLen, &plmnNameLen) == AT_FAILURE) {
                return AT_ERROR;
            }
            eonsRequest->maxNwNameLen = (VOS_UINT8)plmnNameLen;
        }
    }

    /* ��ֵEons�������� */
    eonsRequest->eonsType = (TafEonsSetType)(g_atParaList[0].paraValue);

    /* �ڶ�������Ϊ�� */
    if (g_atParaList[1].paraLen == 0) {
        eonsRequest->eonsPlmnInfo.plmnLen = 0;
    } else {
        eonsRequest->eonsPlmnInfo.plmnLen = (VOS_UINT8)g_atParaList[1].paraLen;
        /* ��Ҫ�ж����������Ƿ�����ID�򷵻ز������� */
        if (AT_CheckEonsPlmnParaValid(&(eonsRequest->eonsPlmnInfo)) == VOS_FALSE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_SUCCESS;
}

/*
 * ����˵��: ����ָ��ģʽ����EONS����MMA
 * �������: moduleId: ATģ��ID
 *           clientId: AT Client ID
 *           opId: ����ָʾ
 * ���ؽ��: AT_FAILURE: ִ��ʧ��
 *           AT_SUCCESS: ִ�гɹ�
 */
VOS_UINT32 AT_SndEonsNameQryReq(VOS_UINT32 moduleId, VOS_UINT8 clientIdx, TafMmaEonsRequestPara *eonsPara)
{
    TafMmaEonsRequest *msg = VOS_NULL_PTR;
    errno_t            memResult;
    VOS_UINT32         receiverPid;
    VOS_UINT32         senderPid;

    receiverPid = TAF_GetDestPid(g_atClientTab[clientIdx].clientId, WUEPS_PID_MMA);
    senderPid   = TAF_GetSendPid(g_atClientTab[clientIdx].clientId, WUEPS_PID_TAF);

    /* ������Ϣ��TafMmaEonsRequest */
    msg = (TafMmaEonsRequest *)TAF_AllocMsgWithHeaderLen(senderPid, sizeof(TafMmaEonsRequest));

    /* �ڴ�����ʧ�ܣ����� */
    if (msg == VOS_NULL_PTR) {
        return VOS_FALSE;
    }

    /* ��������������TafMmaEonsRequest */
    TAF_CfgMsgHdr((MsgBlock *)msg, senderPid, receiverPid, sizeof(TafMmaEonsRequest) - VOS_MSG_HEAD_LENGTH);
    msg->msgName       = ID_TAF_MMA_EONS_QRY_REQ;
    msg->ctrlInfo.moduleId = moduleId;
    msg->ctrlInfo.clientId = g_atClientTab[clientIdx].clientId;
    msg->ctrlInfo.opId     = g_atClientTab[clientIdx].opId;

    memResult = memcpy_s(&(msg->eonsRequestPara), sizeof(msg->eonsRequestPara),
        eonsPara,sizeof(TafMmaEonsRequestPara));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(msg->eonsRequestPara), sizeof(TafMmaEonsRequestPara));

    /* ������Ϣ */
    if (TAF_TraceAndSendMsg(senderPid, msg) != VOS_OK) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*
 * ����˵��: AT^EONS�����������
 * �������: index: AT�·�ͨ��
 * ���ؽ��: AT_WAIT_ASYNC_RETURN: �첽����״̬
 *           AT_ERROR: ִ��ʧ��
 */
VOS_UINT32 AT_SetEonsPara(VOS_UINT8 clientIdx)
{
    VOS_UINT32 result;
    TafMmaEonsRequestPara eonsReqPara;

    result = AT_CheckEonsPara();
    if (result != AT_SUCCESS) {
        AT_WARN_LOG("AT_SetEonsPara :para check fail.");
        return result;
    }
    (VOS_VOID)memset_s(&eonsReqPara, sizeof(eonsReqPara), 0x00, sizeof(eonsReqPara));
    result = AT_FillEonsPara(&eonsReqPara);
    if (result != AT_SUCCESS) {
        return result;
    }
    /* ���͵�C�˻�ȡ��Ӫ������ */
    result = AT_SndEonsNameQryReq(WUEPS_PID_AT, clientIdx, &eonsReqPara);
    if (result == VOS_TRUE) {
        g_atClientTab[clientIdx].cmdCurrentOpt = AT_CMD_EONS_SET;
        AT_WARN_LOG2("AT_SetEonsPara :curropt clientidx.", g_atClientTab[clientIdx].cmdCurrentOpt, clientIdx);
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

/*
 * ����˵��: У�鲢���^NWSCAN�����������
 * �������: nwScanPara: NWSCAN�����ṹָ��
 * ���ؽ��: AT_OK: ִ�гɹ�
 *           AT_ERROR: ִ��ʧ��
 */
LOCAL VOS_UINT32 AT_CheckAndFillNwScanPara(TAF_MMA_SysCfgPara *nwScanPara)
{
    /* ���������֧��3����������ǰʵ��ֻʵ�ֵ�һ������ */
    if (g_atParaIndex > 3) {
        return AT_TOO_MANY_PARA;
    }

    /* ��ȡ���뼼�� */
    switch (g_atParaList[0].paraValue) {
        case AT_NWSCAN_GSM:
        case AT_NWSCAN_EDGE:
        case AT_NWSCAN_WCDMA: {
            nwScanPara->utranMode = 0; /* WCDMA */
            nwScanPara->multiModeRatCfg.ratNum = 2; /* ֧��GSMʱ��Rat��ĿΪ2 */
            nwScanPara->multiModeRatCfg.ratOrder[0] = TAF_MMA_RAT_WCDMA;
            nwScanPara->multiModeRatCfg.ratOrder[1] = TAF_MMA_RAT_GSM;
            break;
        }
#if (FEATURE_LTE == FEATURE_ON)
        case AT_NWSCAN_LTE_FDD:
        case AT_NWSCAN_LTE_TDD: {
            nwScanPara->multiModeRatCfg.ratNum = 1; /* BTL��λ��ʽ�����̶�Ϊ1 */
            nwScanPara->multiModeRatCfg.ratOrder[0] = TAF_MMA_RAT_LTE;
            break;
        }
#endif
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        case AT_NWSCAN_NR: {
            nwScanPara->multiModeRatCfg.ratNum = 1; /* BTL��λ��ʽ�����̶�Ϊ1 */
            nwScanPara->multiModeRatCfg.ratOrder[0] = TAF_MMA_RAT_NR;
            break;
        }
#endif
        default: {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    /* ������ȷʱ��NWSCAN��־λ����ΪTRUE */
    nwScanPara->nwScanFlag = VOS_TRUE;
    return AT_OK;
}

/*
 * ����˵��: AT^NWSCAN�������
 * �������: index: AT�·�ͨ��
 * ���ؽ��: AT_WAIT_ASYNC_RETURN: �첽�ȴ�״̬
 *           AT_ERROR: ִ��ʧ��
 */
VOS_UINT32 AT_SetNwScanPara(VOS_UINT8 indexNum)
{
    TAF_MMA_SysCfgPara nwScanSetPara;
    VOS_UINT32 result;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&nwScanSetPara, sizeof(nwScanSetPara), 0x00, sizeof(nwScanSetPara));

    /* �������ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������� */
    result = AT_CheckAndFillNwScanPara(&nwScanSetPara);
    if (result != AT_OK) {
        return result;
    }

    /* ������������ */
    result = TAF_MMA_SetSysCfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &nwScanSetPara);
    if (result == VOS_TRUE) {
        /* ָʾ��ǰ�û��������������ΪSYSCFG�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSCFG_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

/*
 * ����˵��: ��MMA����ID_TAF_MMA_NETSELOPT_SET_REQ��Ϣ
 * �������: moduleId: ATģ��ID
 *           clientId: ATͨ��ID
 *           opId: ����ID
 * ���ؽ��: AT_SUCCESS: ִ�гɹ�
 *           AT_FAILURE: ִ��ʧ��
 */
LOCAL VOS_UINT32 TAF_MMA_SetNetSelOptReq(VOS_UINT32 moduleId, VOS_UINT16 clientId, VOS_UINT8 opId)
{
    TafMmaNetSelOptSetReq *msg = VOS_NULL_PTR;
    VOS_UINT32             receiverPid;
    VOS_UINT32             senderPid;

    receiverPid = TAF_GetDestPid(clientId, WUEPS_PID_MMA);
    senderPid   = TAF_GetSendPid(clientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MMA_NETSELOPT_SET_REQ */
    msg = (TafMmaNetSelOptSetReq *)TAF_AllocMsgWithHeaderLen(senderPid, sizeof(TafMmaNetSelOptSetReq));

    /* �ڴ�����ʧ�ܣ����� */
    if (msg == VOS_NULL_PTR) {
        return VOS_FALSE;
    }

    /* ��������������TafMmaNetSelOptSetReq */
    TAF_CfgMsgHdr((MsgBlock *)msg, senderPid, receiverPid, sizeof(TafMmaNetSelOptSetReq) - VOS_MSG_HEAD_LENGTH);
    msg->msgName       = ID_TAF_MMA_NETSELOPT_SET_REQ;
    msg->ctrl.moduleId = moduleId;
    msg->ctrl.clientId = clientId;
    msg->ctrl.opId     = opId;
    msg->scene         = g_atParaList[AT_NETSEL_PARA_SCENE].paraValue;
    msg->operate       = g_atParaList[AT_NETSEL_PARA_OPERATE].paraValue;

    /* ������Ϣ */
    if (TAF_TraceAndSendMsg(senderPid, msg) != VOS_OK) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
/*
 * ����˵��: AT^NETSELOPT�����������
 * �������: index: AT����ִ��ͨ��index
 * ���ؽ��: AT_WAIT_ASYNC_RETURN: �첽�ȴ�
 *           ����: ִ��ʧ��
 */
VOS_UINT32 AT_SetNetSelOptPara(VOS_UINT8 indexNum)
{
    /* ����������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������Ϊ2����Ϊ�Ƿ� */
    if (g_atParaIndex != AT_NETSEL_PARA_NUM) {
        AT_WARN_LOG("AT_SetNetSelOptPara: para num is invalid!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϣ������MMA */
    if (TAF_MMA_SetNetSelOptReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId,
        g_atClientTab[indexNum].opId) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NETSELOPT_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#endif
