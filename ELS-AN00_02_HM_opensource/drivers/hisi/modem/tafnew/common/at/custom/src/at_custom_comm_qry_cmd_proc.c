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
#include "at_custom_comm_qry_cmd_proc.h"
#include "securec.h"
#include "at_snd_msg.h"
#include "at_cmd_proc.h"
#include "at_check_func.h"

#include "nv_stru_msp_interface.h"
#include "nv_stru_msp.h"

#include "at_taf_agent_interface.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_COMM_QRY_CMD_PROC_C

VOS_UINT32 AT_QrySARReduction(VOS_UINT8 indexNum)
{
    VOS_UINT32         ret;
    VOS_UINT16         uETestMode = VOS_FALSE;
    AT_ModemMtInfoCtx *mtInfoCtx = VOS_NULL_PTR;
    ModemIdUint16      modemId;
    VOS_UINT16         antState;

    /* ��ȡNV���ж��Ƿ�Ϊ����ģʽ */
    ret = TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Item_RF_SAR_BACKOFF_TESTMODE, &uETestMode, sizeof(uETestMode));

    if (ret != NV_OK) {
        AT_ERR_LOG("AT_QrySARReduction:Read NV fail");
        return AT_ERROR;
    }

    modemId = MODEM_ID_0;

    ret = AT_GetModemIdFromClient(indexNum, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("AT_QrySARReduction:Get modem id fail");
        return AT_ERROR;
    }

    mtInfoCtx = AT_GetModemMtInfoCtxAddrFromModemId(modemId);

    antState = 0;
    ret      = TAF_AGENT_GetAntState(indexNum, &antState);
    if (ret != VOS_OK) {
        AT_ERR_LOG("AT_QrySARReduction:Get modem id fail");
        return AT_ERROR;
    }

#ifndef FEATURE_ANTEN_DETECT
    if ((antState == AT_ANT_CONDUCTION_MODE) && (uETestMode != VOS_TRUE))
#else
    if ((antState != AT_ANT_AIRWIRE_MODE) && (uETestMode != VOS_TRUE))
#endif
    {
        AT_ERR_LOG("AT_QrySARReduction: ant state wrong");
        return AT_ERROR;
    }

    /* AT��� */
    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "%s:%u", g_parseContext[indexNum].cmdElement->cmdName,
            mtInfoCtx->sarReduction);

    return AT_OK;
}

#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
 * ��������: ��ѯװ��д���APN KEY
 */
VOS_UINT32 AT_QryVoipApnKeyPara(VOS_UINT8 indexNum)
{
    TAF_NVIM_ApnKeyList encrptPwd;

    /* �����Ƿ�� */
    if (AT_GetModemMbbCustCtxAddrFromClientId(indexNum)->voipApnKey == (VOS_FALSE)) {
        return AT_ERROR;
    }

    /* ��������NV�����ĵ� */
    (VOS_VOID)memset_s(&encrptPwd, sizeof(TAF_NVIM_ApnKeyList), 0x00, sizeof(TAF_NVIM_ApnKeyList));
    /* ��ȡvoip apn��Ӧ��NV�� */
    if (TAF_ACORE_NV_READ_IN_CCORE(MODEM_ID_0, NV_ITEM_VOIP_APN_KEY, (VOS_UINT8 *)&encrptPwd, sizeof(TAF_NVIM_ApnKeyList)) != VOS_OK) {
        AT_WARN_LOG("AT_QryVoipApnKeyPara:READ NV FAIL");
        (VOS_VOID)memset_s(&encrptPwd, sizeof(TAF_NVIM_ApnKeyList), 0x00, sizeof(TAF_NVIM_ApnKeyList));
        return AT_ERROR;
    }
    /* ���ܺ�������Ƿ���Ч��useFlagΪ1��ʾ��Ч */
    if (encrptPwd.useFlag == 1) {
        /* ������Ч���ϱ�AT�������̶�Ϊ1����ǰֻ֧��1�����룬������120���ֽ� */
        g_atSendDataBuff.bufLen= (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "%s:%d%s%s:%d,\"%s\"", g_parseContext[indexNum].cmdElement->cmdName,
            1, g_atCrLf, g_parseContext[indexNum].cmdElement->cmdName, 0, encrptPwd.apnKey);
    } else {
        /* ������Ч������0 */
        g_atSendDataBuff.bufLen= (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, 0);
    }

    (VOS_VOID)memset_s(&encrptPwd, sizeof(TAF_NVIM_ApnKeyList), 0x00, sizeof(TAF_NVIM_ApnKeyList));
    return AT_OK;
}

VOS_UINT32 AT_TestVoipApnKeyPara(VOS_UINT8 indexNum)
{
    /* �����Ƿ�� */
    if (AT_GetModemMbbCustCtxAddrFromClientId(indexNum)->voipApnKey == (VOS_FALSE)) {
        return AT_ERROR;
    }

    /* ��ǰֻ֧��һ������ */
    g_atSendDataBuff.bufLen= (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
        AT_MAX_APN_KEY_GROUP);
    return AT_OK;
}
#endif

VOS_UINT32 AT_QryWifiGlobalMacPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;
    VOS_UINT32 result;
    VOS_UINT8  e5GwMacAddr[AT_MAC_ADDR_LEN + 1]; /* MAC��ַ */

    /* ��ȡ����MAC��ַ */
    result                       = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_WIFI_MAC_ADDR, e5GwMacAddr, AT_MAC_ADDR_LEN);
    e5GwMacAddr[AT_MAC_ADDR_LEN] = '\0';

    if (result != NV_OK) {
        AT_WARN_LOG("AT_QryApMacPara:read en_NV_Item_GWMAC_ADDR failed or no need to updatee");
        return AT_ERROR;
    }

    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%s", g_parseContext[indexNum].cmdElement->cmdName, e5GwMacAddr);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

