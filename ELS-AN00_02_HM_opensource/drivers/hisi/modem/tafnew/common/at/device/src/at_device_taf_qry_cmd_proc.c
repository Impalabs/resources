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
#include "at_device_taf_qry_cmd_proc.h"
#include "securec.h"

#include "at_snd_msg.h"
#include "at_mta_interface.h"
#include "at_cmd_proc.h"
#include "at_device_comm.h"
#include "at_device_cmd.h"
#include "at_mdrv_interface.h"
#include "at_lte_common.h"


#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_TAF_QRY_CMD_PROC_C

#define AT_BSN_SERIAL_NUM_LEN 17
#define AT_MNC_LEN_MAX_VALID_VALUE 3
#define AT_MNC_LEN_MIN_VALID_VALUE 2
#define AT_WIWEP_WIFI_KEY_INDEX_2 2
#define AT_WIWEP_WIFI_KEY_INDEX_3 3

VOS_UINT32 At_QryBsn(VOS_UINT8 indexNum)
{
    /* ���кų���Ϊ16,�ټ����һλ�洢������ */
    VOS_UINT8     bsnSerialNum[AT_BSN_SERIAL_NUM_LEN];
    VOS_UINT16    length;
    ModemIdUint16 modemId = MODEM_ID_0;
    VOS_UINT32    ret;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    length = sizeof(bsnSerialNum);

    ret = AT_GetModemIdFromClient(indexNum, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("At_SetSDomainPara: Get modem id fail.");
        return AT_ERROR;
    }

    if (TAF_ACORE_NV_READ(modemId, NV_ITEM_MANUFACTURE_INFO, bsnSerialNum, length) != NV_OK) {
        AT_WARN_LOG("At_QryBsn:WARNING:NVIM Read NV_ITEM_SERIAL_NUM falied!");
        return AT_ERROR;
    } else {
        bsnSerialNum[AT_BSN_SERIAL_NUM_LEN - 1] = 0; /* �����һλ���Խ�������'\0' */
        length           = 0;
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s:", g_parseContext[indexNum].cmdElement->cmdName);
        length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s", bsnSerialNum);
    }
    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}

TAF_UINT32 At_QryPlatForm(TAF_UINT8 indexNum)
{
    VOS_UINT32        ret;
    NAS_NVIM_Platform platform;

    platform.platform = 0;

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_SUB_PLATFORM_INFO, &platform.platform, sizeof(platform.platform));
    if (ret != NV_OK) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen =
        (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "%s: %d,%d", g_parseContext[indexNum].cmdElement->cmdName, 1,
            platform.platform);

    return AT_OK;
}

VOS_UINT32 At_TestTmodePara(VOS_UINT8 indexNum)
{
#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
#else
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
#endif
    VOS_UINT16 length;

    length = 0;

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    atDevCmdCtrl = AT_GetDevCmdCtrl();
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        atDevCmdCtrl->currentTMode);
#else
    atMtInfoCtx             = AT_GetMtInfoCtx();
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        atMtInfoCtx->currentTMode);
#endif

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryTmmiPara(VOS_UINT8 indexNum)
{
    VOS_UINT8  facInfo[AT_FACTORY_INFO_LEN];
    VOS_UINT8  mmiFlag;
    VOS_UINT32 result;

    result = TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Item_Factory_Info, facInfo, AT_FACTORY_INFO_LEN);

    if (result != NV_OK) {
        return AT_ERROR;
    }

    if (VOS_MemCmp(&facInfo[AT_MMI_TEST_FLAG_OFFSET], AT_MMI_TEST_FLAG_SUCC, AT_MMI_TEST_FLAG_LEN) == VOS_OK) {
        mmiFlag = VOS_TRUE;
    } else {
        mmiFlag = VOS_FALSE;
    }

    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, mmiFlag);

    return AT_OK;
}

VOS_UINT32 AT_QryProdNamePara(VOS_UINT8 indexNum)
{
    TAF_AT_ProductId productId;
    VOS_UINT32       ret;

    (VOS_VOID)memset_s(&productId, sizeof(productId), 0x00, sizeof(TAF_AT_ProductId));

    /* ��NV50048�ж�ȡ��Ʒ���� */
    /* ��ȡNV��NV_ITEM_PRODUCT_ID��ȡ��Ʒ���� */
    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_PRODUCT_ID, &productId,
                            sizeof(productId.nvStatus) + sizeof(productId.productId));

    if (ret != NV_OK) {
        AT_WARN_LOG("AT_QryProdNamePara: Fail to read NV_ITEM_PRODUCT_ID");
        return AT_ERROR;
    }

    /* ��NV�Ƿ�ʹ�� */
    if (productId.nvStatus != VOS_TRUE) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:%s", g_parseContext[indexNum].cmdElement->cmdName, productId.productId);

    return AT_OK;
}

VOS_UINT32 AT_QryEqverPara(VOS_UINT8 indexNum)
{
    /*
     * <version>װ����һ�� AT����汾�ţ��汾������λ��ʾ��ÿλΪ0~9�������ַ���
     * �ð汾��ȡ���ڵ���ʵ�ֵ�AT����ʱ�ο��ı��ĵ��İ汾�š�
     * 114 ֧�ֹ�һ��AT����汾��ѯ�ĵ�һ���汾
     * balong�̶�����115
     */
    VOS_UINT32   ret;
    TAF_AT_EqVer eqver;

    eqver.eqver = 0;

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_EQVER, &eqver.eqver, sizeof(eqver.eqver));
    if (ret != NV_OK) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, eqver.eqver);

    return AT_OK;
}

VOS_UINT32 At_QryDataLock(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, g_atDataLocked);

    return AT_OK;
}

VOS_UINT32 At_QrySimLockPlmnInfo(VOS_UINT8 indexNum)
{
    TAF_CUSTOM_SimLockPlmnInfo simLockPlmnInfo;
    VOS_UINT32                 i;
    VOS_UINT16                 length;
    VOS_UINT8                  mncLen;
    VOS_UINT32                 plmnIdx;
    VOS_UINT32                 rsltChkPlmnBegin;
    VOS_UINT32                 rsltChkPlmnEnd;
    VOS_UINT32                 totalPlmnNum;
    VOS_BOOL                   abPlmnValidFlg[TAF_MAX_SIM_LOCK_RANGE_NUM] = {VOS_FALSE};
    VOS_UINT8                  asciiNumBegin[(TAF_PH_SIMLOCK_PLMN_STR_LEN * 2) + 1];
    VOS_UINT8                  asciiNumEnd[(TAF_PH_SIMLOCK_PLMN_STR_LEN * 2) + 1];

    (VOS_VOID)memset_s(&simLockPlmnInfo, sizeof(simLockPlmnInfo), 0x00, sizeof(simLockPlmnInfo));

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO, &simLockPlmnInfo, sizeof(simLockPlmnInfo)) !=
        NV_OK) {
        return AT_ERROR;
    }

    totalPlmnNum = 0;

    if (simLockPlmnInfo.status == NV_ITEM_DEACTIVE) {
        /* ״̬�Ǽ���ʱ����ʾPlmn����Ϊ0 */
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, totalPlmnNum);

        g_atSendDataBuff.bufLen = length;
        return AT_OK;
    }

    /*  �ж�Plmn�Ŷ��Ƿ���Ч */
    for (i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++) {
        mncLen = simLockPlmnInfo.simLockPlmnRange[i].mncNum;

        rsltChkPlmnBegin = At_SimlockPlmnNumToAscii(simLockPlmnInfo.simLockPlmnRange[i].rangeBegin,
                                                    TAF_PH_SIMLOCK_PLMN_STR_LEN, asciiNumBegin);

        rsltChkPlmnEnd = At_SimlockPlmnNumToAscii(simLockPlmnInfo.simLockPlmnRange[i].rangeBegin,
                                                  TAF_PH_SIMLOCK_PLMN_STR_LEN, asciiNumEnd);

        if ((rsltChkPlmnBegin == AT_SUCCESS) && (rsltChkPlmnEnd == AT_SUCCESS) &&
            ((mncLen <= AT_MNC_LEN_MAX_VALID_VALUE) && (mncLen >= AT_MNC_LEN_MIN_VALID_VALUE))) {
            abPlmnValidFlg[i] = VOS_TRUE;
            ++totalPlmnNum;
        }
    }

    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, totalPlmnNum);

    if (totalPlmnNum == 0) {
        g_atSendDataBuff.bufLen = length;
        return AT_OK;
    } else {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);
    }

    plmnIdx = 0;
    for (i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++) {
        if (abPlmnValidFlg[i] == VOS_FALSE) {
            continue;
        }

        ++plmnIdx;

        At_SimlockPlmnNumToAscii(simLockPlmnInfo.simLockPlmnRange[i].rangeBegin, TAF_PH_SIMLOCK_PLMN_STR_LEN,
                                 asciiNumBegin);

        At_SimlockPlmnNumToAscii(simLockPlmnInfo.simLockPlmnRange[i].rangeEnd, TAF_PH_SIMLOCK_PLMN_STR_LEN,
                                 asciiNumEnd);

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s:%d,%d,%s,%s", g_parseContext[indexNum].cmdElement->cmdName,
            plmnIdx, simLockPlmnInfo.simLockPlmnRange[i].mncNum, asciiNumBegin, asciiNumEnd);

        if (plmnIdx < totalPlmnNum) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);
        }
    }

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_QryMaxLockTimes(VOS_UINT8 indexNum)
{
    TAF_CUSTOM_SimLockMaxTimes simLockMaxTimes;
    VOS_UINT32                 result;
    VOS_UINT16                 length;

    simLockMaxTimes.status       = NV_ITEM_DEACTIVE;
    simLockMaxTimes.lockMaxTimes = 0;

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES, &simLockMaxTimes,
                               sizeof(simLockMaxTimes));
    if (result != NV_OK) {
        AT_WARN_LOG("At_QryMaxLockTimes():NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES TAF_ACORE_NV_READ Fail!");
        return AT_DEVICE_OTHER_ERROR;
    }

    /* ���NV��δ������ص���Ĭ��ֵ 10 */
    if (simLockMaxTimes.status == NV_ITEM_DEACTIVE) {
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            TAF_PH_CARDLOCK_DEFAULT_MAXTIME);
    } else {
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            simLockMaxTimes.lockMaxTimes);
    }

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_QryCallSrvPara(VOS_UINT8 indexNum)
{
    NAS_NVIM_CustomizeService custSrv;
    VOS_UINT16                length;

    custSrv.status           = NV_ITEM_DEACTIVE;
    custSrv.customizeService = 0;

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CUSTOMIZE_SERVICE, &custSrv, sizeof(custSrv)) != NV_OK) {
        AT_WARN_LOG("At_QryCallSrvPara():NV_ITEM_CUSTOMIZE_SERVICE NV Read Fail!");
        return AT_DEVICE_OTHER_ERROR;
    }

    /* ���NV��δ������ص���Ĭ��ֵFALSE */
    if (custSrv.status == NV_ITEM_DEACTIVE) {
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, VOS_FALSE);
    } else {
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            custSrv.customizeService);
    }

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryMDatePara(VOS_UINT8 indexNum)
{
    VOS_UINT32                 ret;
    VOS_UINT16                 length;
    TAF_AT_NvimManufactureDate manufactrueDate;

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_AT_MANUFACTURE_DATE, &manufactrueDate, AT_MDATE_STRING_LENGTH);
    if (ret != NV_OK) {
        AT_WARN_LOG("AT_QryMDatePara: fail to read NVIM . ");
        return AT_ERROR;
    }

    /* ƴ����Ӧ�ַ���: �����֣�����������Ϣ */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);

    manufactrueDate.mDate[AT_MDATE_STRING_LENGTH - 1] = '\0';
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s", manufactrueDate.mDate);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryFacInfoPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 ret;
    VOS_UINT8 *facInfo = TAF_NULL_PTR;
    VOS_UINT16 length;

    /* ��ȡNV���Ѿ��洢��������Ϣ */
    facInfo = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, AT_FACINFO_STRING_LENGTH);
    if (facInfo == VOS_NULL_PTR) {
        AT_WARN_LOG("AT_QryFacInfoPara: fail to alloc memory.");
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(facInfo, AT_FACINFO_STRING_LENGTH, 0x00, AT_FACINFO_STRING_LENGTH);

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_AT_FACTORY_INFO, facInfo, AT_FACINFO_STRING_LENGTH);
    if (ret != NV_OK) {
        AT_WARN_LOG("AT_QryFacInfoPara: fail to read NV_ITEM_AT_FACTORY_INFO . ");
        PS_MEM_FREE(WUEPS_PID_AT, facInfo);
        return AT_ERROR;
    }

    /* �ݴ���ǿ�Ƹ�NV��INFO�ֶμӽ����� */
    *(facInfo + (AT_FACINFO_STRING_LENGTH - 1)) = '\0';
    *(facInfo + AT_FACINFO_INFO1_LENGTH)        = '\0';

    /* ƴ����Ӧ�ַ���: �����֣���һ��������Ϣ */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:0,%s", g_parseContext[indexNum].cmdElement->cmdName, facInfo);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

    /* ƴ����Ӧ�ַ���: �����֣��ڶ���������Ϣ */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s:1,%s", g_parseContext[indexNum].cmdElement->cmdName,
        ((facInfo + AT_FACINFO_INFO1_LENGTH) + 1));

    g_atSendDataBuff.bufLen = length;

    PS_MEM_FREE(WUEPS_PID_AT, facInfo);
    return AT_OK;
}

VOS_UINT32 AT_DeciDigit2Ascii(VOS_UINT8 aucDeciDigit[], VOS_UINT32 length, VOS_UINT8 aucAscii[])
{
    VOS_UINT32 loop;

    for (loop = 0; loop < length; loop++) {
        if (aucDeciDigit[loop] > AT_DEC_MAX_NUM) {
            return VOS_ERR;
        }

        aucAscii[loop] = aucDeciDigit[loop] + '0';
    }

    return VOS_OK;
}

VOS_UINT32 AT_GetImeiValue(ModemIdUint16 modemId, VOS_UINT8 imei[TAF_PH_IMEI_LEN + 1], VOS_UINT8 imeiMaxLength)
{
    VOS_UINT8      buf[TAF_PH_IMEI_LEN + 1];
    VOS_UINT8      checkData;
    VOS_UINT32     dataLen;
    VOS_UINT32     i;

    if (imeiMaxLength < (TAF_PH_IMEI_LEN + 1)) {
        return VOS_ERR;
    }

    checkData = 0;
    dataLen   = TAF_PH_IMEI_LEN;

    (VOS_VOID)memset_s(buf, sizeof(buf), 0x00, sizeof(buf));

    if (TAF_ACORE_NV_READ(modemId, en_NV_Item_IMEI, buf, dataLen) != NV_OK) {
        AT_WARN_LOG("TAF_GetImeiValue:Read IMEI Failed!");
        return VOS_ERR;
    }

    checkData = 0;
    /* checkdata�ͽ�β��������ֵ�����Լ�2��һ��ѭ����i��i+1��ֵ������ѭ����2 */
    for (i = 0; i < (TAF_PH_IMEI_LEN - 2); i += 2) {
        checkData += buf[i] + ((buf[i + 1UL] + buf[i + 1UL]) / AT_DECIMAL_BASE_NUM) +
                     ((buf[i + 1UL] + buf[i + 1UL]) % AT_DECIMAL_BASE_NUM);
    }
    checkData = (AT_DECIMAL_BASE_NUM - (checkData % AT_DECIMAL_BASE_NUM)) % AT_DECIMAL_BASE_NUM;
    AT_DeciDigit2Ascii(buf, (VOS_UINT8)dataLen, imei);
    imei[TAF_PH_IMEI_LEN - 2] = checkData + AT_HEX_CONVERT_ASCII_NUM_BASE_VALUE;
    imei[TAF_PH_IMEI_LEN - 1] = 0;

    return VOS_OK;
}

VOS_UINT32 AT_QryPhyNumPara(VOS_UINT8 indexNum)
{
    VOS_UINT32    ret;
    VOS_UINT16    length;
    VOS_UINT8     asciiImei[TAF_PH_IMEI_LEN + 1];
    TAF_SVN_Data  svn;
    VOS_UINT8     asciiSvn[TAF_SVN_DATA_LENGTH + 1];
    VOS_UINT8     mac[AT_PHYNUM_MAC_LEN + 1]; /* MAC��ַ */
    ModemIdUint16 modemId = MODEM_ID_0;

    (VOS_VOID)memset_s(&svn, sizeof(svn), 0x00, sizeof(svn));
    (VOS_VOID)memset_s(asciiImei, sizeof(asciiImei), 0x00, sizeof(asciiImei));

    ret = AT_GetModemIdFromClient(indexNum, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("AT_QryPhyNumPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* ��ȡNV���ȡIMEI */
    ret = AT_GetImeiValue(modemId, asciiImei, sizeof(asciiImei));

    if (ret != VOS_OK) {
        AT_WARN_LOG("AT_QryPhyNumPara: Fail to read en_NV_Item_IMEI.");
        return AT_ERROR;
    }

    /* ��ȡNV���ȡSVN */
    ret = TAF_ACORE_NV_READ(modemId, NV_ITEM_IMEI_SVN, &svn, sizeof(svn));
    if (ret != NV_OK) {
        AT_WARN_LOG("AT_QryPhyNumPara: Fail to read en_Nv_Item_Imei_Svn.");
        return AT_ERROR;
    } else {
        if (svn.activeFlag != NV_ITEM_ACTIVE) {
            /* nvû�䣬��ʼ�����ַ���'0' */
            (VOS_VOID)memset_s(asciiSvn, sizeof(asciiSvn), '0', TAF_SVN_DATA_LENGTH);
        } else {
            AT_DeciDigit2Ascii(svn.svn, TAF_SVN_DATA_LENGTH, asciiSvn);
        }

        asciiSvn[TAF_SVN_DATA_LENGTH] = '\0';
    }

    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:IMEI,%s%s", g_parseContext[indexNum].cmdElement->cmdName, asciiImei,
        g_atCrLf);

    if (AT_IsMiscSupport(BSP_MODULE_TYPE_WIFI) == BSP_MODULE_SUPPORT) {
        ret = AT_GetPhynumMac(mac, AT_PHYNUM_MAC_LEN + 1);
        if (ret != AT_OK) {
            AT_WARN_LOG("AT_QryPhyNumPara: Fail to read en_NV_Item_IMEI.");
            return AT_ERROR;
        }

        /* MAC��ַ��� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s:MACWLAN,%s%s", g_parseContext[indexNum].cmdElement->cmdName,
            mac, g_atCrLf);
    }

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s:SVN,%s", g_parseContext[indexNum].cmdElement->cmdName, asciiSvn);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_QryCsVer(VOS_UINT8 indexNum)
{
    VOS_UINT32     ret;
    TAF_NVIM_CsVer csver;

    csver.csver = 0;

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CSVER, &csver, sizeof(VOS_UINT16));
    if (ret != NV_OK) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, csver.csver);

    return AT_OK;
}

VOS_UINT32 At_QryQosPara(VOS_UINT8 indexNum)
{
    AT_TrafficClassCustomize aTTrafficClass;
    VOS_UINT16               readLength;
    VOS_UINT16               length;
    ModemIdUint16            modemId = MODEM_ID_0;
    VOS_UINT32               ret;

    aTTrafficClass.status       = NV_ITEM_DEACTIVE;
    aTTrafficClass.trafficClass = AT_QOS_TRAFFIC_CLASS_SUBSCRIBE;

    /* ����״̬���ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* ��ȡNV�е�ֵ��stATTrafficClass�ṹ����� */
    readLength = sizeof(aTTrafficClass);

    ret = AT_GetModemIdFromClient(indexNum, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("At_QryQosPara: Get modem id fail.");
        return AT_ERROR;
    }

    if (TAF_ACORE_NV_READ(modemId, NV_ITEM_TRAFFIC_CLASS_TYPE, &aTTrafficClass, readLength) != NV_OK) {
        AT_WARN_LOG("At_QryQosPara:WARNING:NVIM Read NV_ITEM_TRAFFIC_CLASS_TYPE failed!");
        return AT_DEVICE_OTHER_ERROR;
    }

    /* NV���ڷǼ���̬,ֱ���ϱ�Ĭ��ֵ:AT_QOS_TRAFFIC_CLASS_INTERACTIVE */
    if (aTTrafficClass.status == NV_ITEM_DEACTIVE) {
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            AT_QOS_TRAFFIC_CLASS_SUBSCRIBE);

    } else {
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            aTTrafficClass.trafficClass);
    }
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryRsimPara(VOS_UINT8 indexNum)
{
    VOS_UINT32       cardStatus;
    VOS_UINT32       cardType;
    ModemIdUint16    modemId = MODEM_ID_0;
    VOS_UINT32       getModemIdRslt;
    AT_USIM_InfoCtx *usimInfoCtx = VOS_NULL_PTR;

    getModemIdRslt = AT_GetModemIdFromClient(indexNum, &modemId);

    if (getModemIdRslt != VOS_OK) {
        AT_ERR_LOG("At_SetCardModePara:Get Modem Id fail!");
        return AT_ERROR;
    }

    usimInfoCtx = AT_GetUsimInfoCtxFromModemId(modemId);

    /* ��ȡ SIM �����ͺ�״̬ */
    cardType   = usimInfoCtx->cardType;
    cardStatus = usimInfoCtx->cardStatus;

    AT_NORM_LOG1("AT_QryRsimPara: ucCardStatus is ", (VOS_INT32)cardStatus);
    AT_NORM_LOG1("AT_QryRsimPara: ucCardType is ", (VOS_INT32)cardType);

    if (cardStatus == USIMM_CARDAPP_SERVIC_BUTT) {
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            AT_RSIM_STATUS_USIMM_CARD_BUSY);
        return AT_OK;
    }

    /* ���������ΪUSIMM_CARD_ROM_SIMָʾ��ǰΪ���ٿ�����Ч���޿� */
    if ((usimInfoCtx->cardMediumType == USIMM_CARD_MEDIUM_ROM) || (cardStatus == USIMM_CARDAPP_SERVIC_ABSENT)) {
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            AT_RSIM_STATUS_USIMM_CARD_ABSENT);
    } else {
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            AT_RSIM_STATUS_USIMM_CARD_AVAILABLE);
    }

    return AT_OK;
}

LOCAL VOS_VOID AT_PrintWifiWepKey1Para(VOS_UINT8 *wepKeyLen1, VOS_UINT32 loop, VOS_UINT16 *length, VOS_UINT8 indexNum,
    TAF_AT_MultiWifiSec *wifiSecInfo)
{
    VOS_UINT8            wifiWepKey[AT_NV_WLKEY_LEN + 1];
    errno_t              memResult;

    if (wepKeyLen1[loop] != 0) {
        /* wifikey1 */
        (VOS_VOID)memset_s(wifiWepKey, sizeof(wifiWepKey), 0x00, sizeof(wifiWepKey));

        memResult = memcpy_s(wifiWepKey, sizeof(wifiWepKey), wifiSecInfo->wifiWepKey1[loop], AT_WIFI_KEY_LEN_MAX);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(wifiWepKey), AT_WIFI_KEY_LEN_MAX);

        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + *length, "%s:%d,%s,%d%s", g_parseContext[indexNum].cmdElement->cmdName,
            0, wifiWepKey, loop, g_atCrLf);
    }
}

LOCAL VOS_VOID AT_PrintWifiWepKey2Para(VOS_UINT8 *wepKeyLen2, VOS_UINT32 loop, VOS_UINT16 *length, VOS_UINT8 indexNum,
    TAF_AT_MultiWifiSec *wifiSecInfo)
{
    VOS_UINT8            wifiWepKey[AT_NV_WLKEY_LEN + 1];
    errno_t              memResult;

    if (wepKeyLen2[loop] != 0) {
        /* wifikey2 */
        (VOS_VOID)memset_s(wifiWepKey, sizeof(wifiWepKey), 0x00, sizeof(wifiWepKey));

        memResult = memcpy_s(wifiWepKey, sizeof(wifiWepKey), wifiSecInfo->wifiWepKey2[loop], AT_WIFI_KEY_LEN_MAX);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(wifiWepKey), AT_WIFI_KEY_LEN_MAX);

        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + *length, "%s:%d,%s,%d%s", g_parseContext[indexNum].cmdElement->cmdName,
            1, wifiWepKey, loop, g_atCrLf);
    }
}

LOCAL VOS_VOID AT_PrintWifiWepKey3Para(VOS_UINT8 *wepKeyLen3, VOS_UINT32 loop, VOS_UINT16 *length, VOS_UINT8 indexNum,
    TAF_AT_MultiWifiSec *wifiSecInfo)
{
    VOS_UINT8            wifiWepKey[AT_NV_WLKEY_LEN + 1];
    errno_t              memResult;

    if (wepKeyLen3[loop] != 0) {
        /* wifikey1 */
        (VOS_VOID)memset_s(wifiWepKey, sizeof(wifiWepKey), 0x00, sizeof(wifiWepKey));

        memResult = memcpy_s(wifiWepKey, sizeof(wifiWepKey), wifiSecInfo->wifiWepKey3[loop], AT_WIFI_KEY_LEN_MAX);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(wifiWepKey), AT_WIFI_KEY_LEN_MAX);

        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + *length, "%s:%d,%s,%d%s", g_parseContext[indexNum].cmdElement->cmdName,
            AT_WIWEP_WIFI_KEY_INDEX_2, wifiWepKey, loop, g_atCrLf);
    }
}

LOCAL VOS_VOID AT_PrintWifiWepKey4Para(VOS_UINT8 *wepKeyLen4, VOS_UINT32 loop, VOS_UINT16 *length, VOS_UINT8 indexNum,
    TAF_AT_MultiWifiSec *wifiSecInfo)
{
    VOS_UINT8            wifiWepKey[AT_NV_WLKEY_LEN + 1];
    errno_t              memResult;

    if (wepKeyLen4[loop] != 0) {
        /* wifikey1 */
        (VOS_VOID)memset_s(wifiWepKey, sizeof(wifiWepKey), 0x00, sizeof(wifiWepKey));

        memResult = memcpy_s(wifiWepKey, sizeof(wifiWepKey), wifiSecInfo->wifiWepKey4[loop], AT_WIFI_KEY_LEN_MAX);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(wifiWepKey), AT_WIFI_KEY_LEN_MAX);

        *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + *length, "%s:%d,%s,%d%s", g_parseContext[indexNum].cmdElement->cmdName,
            AT_WIWEP_WIFI_KEY_INDEX_3, wifiWepKey, loop, g_atCrLf);
    }
}

VOS_UINT32 AT_QryWiwepPara(VOS_UINT8 indexNum)
{
    VOS_UINT32           ret;
    VOS_UINT32           loop;
    VOS_UINT16           length;
    TAF_AT_MultiWifiSec *wifiSecInfo = VOS_NULL_PTR;
    VOS_UINT8            wepKeyLen1[AT_WIFI_MAX_SSID_NUM] = {0};
    VOS_UINT8            wepKeyLen2[AT_WIFI_MAX_SSID_NUM] = {0};
    VOS_UINT8            wepKeyLen3[AT_WIFI_MAX_SSID_NUM] = {0};
    VOS_UINT8            wepKeyLen4[AT_WIFI_MAX_SSID_NUM] = {0};
    VOS_UINT8            wepKeyNum = 0;

    if (AT_IsMiscSupport(BSP_MODULE_TYPE_WIFI) == BSP_MODULE_UNSUPPORT) {
        return AT_ERROR;
    }

    /* Ϊ��ȡWIFI KEY�����ڴ棬��NV��en_NV_Item_WIFI_KEY��ȡWIFI KEY��Ϣ */
    wifiSecInfo = (TAF_AT_MultiWifiSec *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(TAF_AT_MultiWifiSec));
    if (wifiSecInfo == VOS_NULL_PTR) {
        return AT_ERROR;
    }
    (VOS_VOID)memset_s(wifiSecInfo, sizeof(TAF_AT_MultiWifiSec), 0x00, sizeof(TAF_AT_MultiWifiSec));

    ret = TAF_ACORE_NV_READ_IN_CCORE(MODEM_ID_0, NV_ITEM_MULTI_WIFI_KEY, (VOS_UINT8 *)wifiSecInfo,
                                     sizeof(TAF_AT_MultiWifiSec));
    if (ret != NV_OK) {
        AT_WARN_LOG("AT_QryWiwepPara: Fail to read en_NV_Item_WIFI_KEY.");
        PS_MEM_FREE(WUEPS_PID_AT, wifiSecInfo);
        return AT_ERROR;
    }

    for (loop = 0; loop < AT_WIFI_MAX_SSID_NUM; loop++) {
        /* KEY1��Ӧ��NV���ձ�ʾKEY1��Ч */
        wepKeyLen1[loop] = (VOS_UINT8)VOS_StrLen((VOS_CHAR *)wifiSecInfo->wifiWepKey1[loop]);
        if (wepKeyLen1[loop] != 0) {
            wepKeyNum++;
        }

        wepKeyLen2[loop] = (VOS_UINT8)VOS_StrLen((VOS_CHAR *)wifiSecInfo->wifiWepKey2[loop]);
        if (wepKeyLen2[loop] != 0) {
            wepKeyNum++;
        }

        wepKeyLen3[loop] = (VOS_UINT8)VOS_StrLen((VOS_CHAR *)wifiSecInfo->wifiWepKey3[loop]);
        if (wepKeyLen3[loop] != 0) {
            wepKeyNum++;
        }

        wepKeyLen4[loop] = (VOS_UINT8)VOS_StrLen((VOS_CHAR *)wifiSecInfo->wifiWepKey4[loop]);
        if (wepKeyLen4[loop] != 0) {
            wepKeyNum++;
        }
    }

    /* δ����ʱ����Ҫ�����Ѷ���0�� */
    if (g_atDataLocked == VOS_TRUE) {
        wepKeyNum = 0;
        (VOS_VOID)memset_s(wepKeyLen1, sizeof(wepKeyLen1), 0x00, sizeof(wepKeyLen1));
        (VOS_VOID)memset_s(wepKeyLen2, sizeof(wepKeyLen2), 0x00, sizeof(wepKeyLen2));
        (VOS_VOID)memset_s(wepKeyLen3, sizeof(wepKeyLen3), 0x00, sizeof(wepKeyLen3));
        (VOS_VOID)memset_s(wepKeyLen4, sizeof(wepKeyLen4), 0x00, sizeof(wepKeyLen4));
    }

    /*
     * ���㹤λ�Խ�Ҫ����Ҫ�����λ֧�ֵ�ȫ����20��WiFi WEP��WIFI��KEY��
     * ��ӡ���鵥��֧�ֵ�WIFI KEY��Ϣ
     * ���16��^WIWEP: <index>,�빤λҪ���20����WiFi WEP��WIFI��KEY��һ�£�
     */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%d%s", g_parseContext[indexNum].cmdElement->cmdName, wepKeyNum, g_atCrLf);

    for (loop = 0; loop < AT_WIFI_MAX_SSID_NUM; loop++) {
        AT_PrintWifiWepKey1Para(&wepKeyLen1[0], loop, &length, indexNum, wifiSecInfo);

        AT_PrintWifiWepKey2Para(&wepKeyLen2[0], loop, &length, indexNum, wifiSecInfo);

        AT_PrintWifiWepKey3Para(&wepKeyLen3[0], loop, &length, indexNum, wifiSecInfo);

        AT_PrintWifiWepKey4Para(&wepKeyLen4[0], loop, &length, indexNum, wifiSecInfo);
    }

    g_atSendDataBuff.bufLen = length - (VOS_UINT16)VOS_StrLen((VOS_CHAR *)g_atCrLf);

    PS_MEM_FREE(WUEPS_PID_AT, wifiSecInfo);

    return AT_OK;
}

VOS_UINT32 AT_QryCmdlenPara(VOS_UINT8 indexNum)
{
    /*
     * �������������ֱ�ӽ���AT�����ַ�����(AT_CMD_MAX_LEN - sizeof("AT"))��
     * ���ַ�������������AT�������ַ����Լ�����ͨ��ATһ����������Ӧ���ַ�������
     * BALONG��Ʒ��Ӧ�ַ������ȿ��Է�
     * ����ϱ���û�����ֵ�����λ�Ը�ֵ�޴����˴����һ���ϱ������ֵAT_CMD_MAX_LEN��
     */
    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress,
        "%s:%d,%d", g_parseContext[indexNum].cmdElement->cmdName, (AT_COM_BUFF_LEN - VOS_StrLen("AT")),
        AT_CMD_MAX_LEN);

    return AT_OK;
}

VOS_UINT32 AT_QryNvResumePara(VOS_UINT8 indexNum)
{
    VOS_UINT16 nvResumeFlag = AT_NV_RESUME_SUCC;

    /* ��ȡen_NV_Resume_Flag */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Resume_Flag, &nvResumeFlag, sizeof(VOS_UINT16)) != NV_OK) {
        AT_WARN_LOG("AT_QryNvResumePara:READ NV FAIL");
        return AT_ERROR;
    }

    /* ������ֵֻ��Ϊ0����1 */
    if ((nvResumeFlag != AT_NV_RESUME_SUCC) && (nvResumeFlag != AT_NV_RESUME_FAIL)) {
        AT_WARN_LOG("AT_QryNvResumePara:PARA IS INVALID");
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, nvResumeFlag);
    return AT_OK;
}

VOS_UINT32 AT_TestWiwepPara(VOS_UINT8 indexNum)
{
    if (AT_IsMiscSupport(BSP_MODULE_TYPE_WIFI) == BSP_MODULE_UNSUPPORT) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            AT_WIWEP_TOOLS_WIFI_KEY_TOTAL);

    return AT_OK;
}

VOS_UINT32 AT_TestRsfwPara(VOS_UINT8 indexNum)
{
#if ((FEATURE_VSIM == FEATURE_ON) && (FEATURE_VSIM_ICC_SEC_CHANNEL == FEATURE_OFF))
    g_atSendDataBuff.usBufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "^RSFW:(SIMLOCK,VSIM)");
#else
    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "^RSFW:(SIMLOCK)");
#endif

    return AT_OK;
}

/*
 * Description: ^ICCID��ѯ
 * History:
 *  1.Date: 2009-08-27
 *    Modification: Created function
 */
TAF_UINT32 At_QryIccidPara(TAF_UINT8 indexNum)
{
    /* ִ��������� */
    if (Taf_ParaQuery(g_atClientTab[indexNum].clientId, 0, TAF_PH_ICC_ID, TAF_NULL_PTR) == AT_SUCCESS) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_ICCID_READ;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 atQryFLNAPara(VOS_UINT8 clientId)
{
    FTM_RdAagcReq fLNAQryReq = {0};
    VOS_UINT32    rst;

    rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_RD_AAGC_REQ, clientId, (VOS_UINT8 *)(&fLNAQryReq),
                           sizeof(fLNAQryReq));

    if (rst == AT_SUCCESS) {
        g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_FLNA_READ;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

#if ((FEATURE_UE_MODE_NR == FEATURE_ON) && defined(FEATURE_PHONE_ENG_AT_CMD) && \
     (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
VOS_UINT32 AT_QryUartTest(VOS_UINT8 indexId)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", "^UARTTEST", AT_GetUartTestState());

    /* �������ڲ��ԣ���ѯ��ͳһ���óɳ�ʼ״̬ */
    if (AT_GetUartTestState() != AT_UART_TEST_RUNNING) {
        AT_SetUartTestState(AT_UART_TEST_NOT_START);
    }
    return AT_OK;
}

VOS_UINT32 AT_QryI2sTest(VOS_UINT8 indexId)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", "^I2STEST", AT_GetI2sTestState());

    /* �������ڲ��ԣ���ѯ��ͳһ���óɳ�ʼ״̬ */
    if (AT_GetI2sTestState() != AT_I2S_TEST_RUNNING) {
        AT_SetI2sTestState(AT_I2S_TEST_NOT_START);
    }
    return AT_OK;
}
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
VOS_UINT32 AT_QryFDac(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
    TAF_UINT16         length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();

    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ѯ��ǰDAC������ */
    length = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%d", atDevCmdCtrl->fdac);
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_TestFdacPara(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
    VOS_UINT16         length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    length       = 0;

    if ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_WCDMA) || (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_AWS)) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0-2047)", g_parseContext[indexNum].cmdElement->cmdName);
    } else {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0-1023)", g_parseContext[indexNum].cmdElement->cmdName);
    }
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_QryFpaPara(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
    VOS_UINT16         length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    /* ��ǰ��Ϊ������ģʽ */
    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ѯ��ǰ�����PA�ȼ������� */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, atDevCmdCtrl->paLevel);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryFAgcgainPara(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
    VOS_UINT16         length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    /* ��� LTE ģ�Ľӿڷ�֧ */
#if (FEATURE_LTE == FEATURE_ON)
    if ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_FDD_LTE) || (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_TDD_LTE)) {
        return atQryFLNAPara(indexNum);
    }
#endif

    /* ��ǰ��Ϊ������ģʽ */
    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ѯ��ǰ�����PA�ȼ������� */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, atDevCmdCtrl->lnaLevel);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}
#else
VOS_UINT32 At_QryFpaPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
    VOS_UINT16  length;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("At_QryFpaPara Enter");

    /* ��ǰ��Ϊ������ģʽ */
    if (atMtInfoCtx->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ѯ��ǰ�����PA�ȼ������� */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        atMtInfoCtx->atInfo.paLevel);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryFAgcgainPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("AT_QryFAgcgainPara Enter");

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*  ���Ƿ�����ģʽ�·����ش��� */
    if (atMtInfoCtx->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            atMtInfoCtx->atInfo.agcGainLevel);

    return AT_OK;
}
#endif

#if (FEATURE_LTE == FEATURE_ON)
#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
VOS_UINT32 At_QrySfm(VOS_UINT8 indexNum)
{
    OM_SW_VerFlag customVersion;

    customVersion.nvSwVerFlag = 0;

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_SW_VERSION_FLAG, &customVersion.nvSwVerFlag,
                          sizeof(customVersion.nvSwVerFlag)) != VOS_OK) {
        return AT_ERROR;
    } else {
        /* AT������NV�д洢����Ϣ�෴ */
        if (customVersion.nvSwVerFlag == 0) {
            customVersion.nvSwVerFlag = 1;

        } else {
            customVersion.nvSwVerFlag = 0;
        }

        g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "^SFM: %d", customVersion.nvSwVerFlag);

        return AT_OK;
    }
}
#endif
#endif

