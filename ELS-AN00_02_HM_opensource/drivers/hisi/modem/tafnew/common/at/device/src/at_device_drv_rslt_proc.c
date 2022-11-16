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
#include "at_device_drv_rslt_proc.h"
#include "securec.h"

#include "at_cmd_proc.h"
#include "at_cmd_msg_proc.h"

#include "at_mta_interface.h"
#include "at_device_comm.h"
#include "osm.h"
#include "at_lte_common.h"


#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_DRV_RSLT_PROC_C

VOS_UINT32 AT_RcvDrvAgentQryTbatvoltRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg            *rcvMsg         = VOS_NULL_PTR;
    DRV_AGENT_TbatvoltQryCnf *tbatvoltQryCnf = VOS_NULL_PTR;
    VOS_UINT8                 indexNum       = 0;
    VOS_UINT32                result;

    /* ��ʼ�� */
    rcvMsg         = (DRV_AGENT_Msg *)msg;
    tbatvoltQryCnf = (DRV_AGENT_TbatvoltQryCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (AT_GetIndexNumByClientId(tbatvoltQryCnf->atAppCtrl.clientId, AT_CMD_TBATVOLT_QRY, &indexNum) !=
        VOS_OK) {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* ��ص�ѹ��ѯ����ж� */
    if (tbatvoltQryCnf->fail != VOS_TRUE) {
        g_atSendDataBuff.bufLen =
            (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
                tbatvoltQryCnf->batVol);
        result = AT_OK;
    } else {
        g_atSendDataBuff.bufLen = 0;
        result                  = AT_ERROR;
    }

    /* ����AT_FormatResultData���������� */
    At_FormatResultData(indexNum, result);
    return VOS_OK;
}

VOS_VOID At_ZeroReplaceBlankInString(VOS_UINT8 *data, VOS_UINT32 len)
{
    TAF_UINT32 chkLen = 0;
    TAF_UINT8 *write  = data;
    TAF_UINT8 *read   = data;

    /* ���������� */
    while (chkLen < len) {
        /*
         * ʱ���ʽ May  5 2011 17:08:00
         * ת����   May 05 2011 17:08:00
         */
        if (*read++ == ' ') {
            if (*read == ' ') {
                write++;
                *write = '0';
                read++;
                chkLen++;
            }
        }
        write++;
        chkLen++;
    }
}

VOS_UINT32 AT_RcvDrvAgentQryVersionRspParamCheck(VOS_UINT16 clientId, VOS_UINT8 *indexNum, ModemIdUint16 *modemId)
{
    VOS_UINT32     ret;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(clientId, indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentQryVersionRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(*indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentQryVersionRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_VERSION_QRY/AT_CMD_RSFR_VERSION_QRY, ^RSFR����Ҳ���ô˽ӿ� */
    if ((g_atClientTab[*indexNum].cmdCurrentOpt != AT_CMD_VERSION_QRY) &&
        (g_atClientTab[*indexNum].cmdCurrentOpt != AT_CMD_RSFR_VERSION_QRY)) {
        return VOS_ERR;
    }

    ret = AT_GetModemIdFromClient(*indexNum, modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("AT_RcvDrvAgentQryVersionRsp: Get modem id fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_VOID AT_PrintfVersionInfoWithNullInnerVersion(DRV_AGENT_VersionQryCnf *versionQryCnfInfo, VOS_CHAR *acTmpTime,
    VOS_UINT32 len, TAF_NVIM_CsVer csver, VOS_UINT16 *length)
{
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:BDT:%s%s", "^VERSION", acTmpTime, g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:EXTS:%s%s", "^VERSION",
        versionQryCnfInfo->softVersion.revisionId, g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:INTS:%s%s", "^VERSION",
        versionQryCnfInfo->softVersion.revisionId, g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:EXTD:%s%s", "^VERSION", versionQryCnfInfo->isoVer.isoInfo,
        g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:INTD:%s%s", "^VERSION", versionQryCnfInfo->isoVer.isoInfo,
        g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:EXTH:%s%s", "^VERSION", versionQryCnfInfo->interHwVer.hwVer,
        g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:INTH:%s%s", "^VERSION", versionQryCnfInfo->fullHwVer.hwVer,
        g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:EXTU:%s%s", "^VERSION", versionQryCnfInfo->modelId.modelId,
        g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:INTU:%s%s", "^VERSION",
        versionQryCnfInfo->interModelId.modelId, g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:CFG:%d%s", "^VERSION", csver.csver, g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:PRL:", "^VERSION");

    g_atSendDataBuff.bufLen = *length;
}

VOS_VOID AT_PrintfVersionInfoWithValidInnerVersion(DRV_AGENT_VersionQryCnf *versionQryCnfInfo, VOS_CHAR *acTmpTime,
    VOS_UINT32 len, TAF_NVIM_CsVer csver, VOS_UINT16 *length)
{
    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:BDT:%s%s", "^VERSION", acTmpTime, g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:EXTS:%s%s", "^VERSION",
        versionQryCnfInfo->softVersion.revisionId, g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:INTS:%s", "^VERSION", g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:EXTD:%s%s", "^VERSION", versionQryCnfInfo->isoVer.isoInfo,
        g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:INTD:%s", "^VERSION", g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:EXTH:%s%s", "^VERSION", versionQryCnfInfo->interHwVer.hwVer,
        g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:INTH:%s", "^VERSION", g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:EXTU:%s%s", "^VERSION", versionQryCnfInfo->modelId.modelId,
        g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:INTU:%s", "^VERSION", g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:CFG:%d%s", "^VERSION", csver.csver, g_atCrLf);

    *length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)(g_atSndCodeAddress + *length), "%s:PRL:", "^VERSION");

    g_atSendDataBuff.bufLen = *length;
}

VOS_VOID AT_SetRsfrVersionCnf(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT32 len)
{
    VOS_UINT16 length;
    VOS_CHAR  *pcRsfrItem = NULL;

    pcRsfrItem = (VOS_CHAR *)PS_MEM_ALLOC(WUEPS_PID_AT, AT_RSFR_RSFW_MAX_LEN);
    if (pcRsfrItem == VOS_NULL_PTR) {
        return;
    }

    (VOS_VOID)memset_s(pcRsfrItem, AT_RSFR_RSFW_MAX_LEN, 0x00, AT_RSFR_RSFW_MAX_LEN);

    /* �Ѵ�SIMLOCK�����ж������Ľ���ַ�����Ϊ����Դ */
    AtBase64Encode(data, len, (VOS_UINT8 *)pcRsfrItem);

    length = 0;
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "^RSFR:\"VERSION\",\"111\",125,1,1,%s", g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "\"%s\"", pcRsfrItem);

    g_atSendDataBuff.bufLen = length;

    At_FormatResultData(indexNum, AT_OK);
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, pcRsfrItem);
    /*lint -restore */
}

VOS_UINT32 At_DelCtlAndBlankCharWithEndPadding(VOS_UINT8 *data, VOS_UINT16 *cmdLen)
{
    VOS_UINT16 origLen;

    origLen = *cmdLen;

    /* ɨ����Ʒ� */
    if (At_ScanCtlChar(data, cmdLen) == AT_FAILURE) {
        return AT_FAILURE;
    }

    /* ɨ����������Ŀո�� */
    if (At_ScanBlankChar(data, cmdLen) == AT_FAILURE) {
        return AT_FAILURE;
    }

    (VOS_VOID)memset_s(data + (*cmdLen), origLen - (*cmdLen), 0x00, origLen - (*cmdLen));

    return AT_SUCCESS;
}

VOS_UINT32 AT_RcvDrvAgentQryVersionRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg           *rcvMsg            = (DRV_AGENT_Msg *)msg;
    DRV_AGENT_VersionQryCnf *versionQryCnfInfo = VOS_NULL_PTR;
    VOS_UINT8                indexNum          = 0;
    VOS_UINT32               len = 0;
    VOS_CHAR                 acTmpTime[AT_AGENT_DRV_VERSION_TIME_LEN] = {0};
    VOS_UINT16               length;
    VOS_BOOL                 bPhyNumIsNull;

    VOS_UINT32     opt;
    VOS_UINT32     ret;
    errno_t        memResult;
    TAF_NVIM_CsVer csver = {0};
    ModemIdUint16  modemId = MODEM_ID_0;

    AT_PR_LOGI("Rcv Msg");

    csver.csver = 0;

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CSVER, &(csver.csver), sizeof(csver.csver));

    AT_PR_LOGI("Call interface success!");
    if (ret != NV_OK) {
        return AT_ERROR;
    }

    /* ��ʼ����Ϣ����ȡucContent */
    versionQryCnfInfo = (DRV_AGENT_VersionQryCnf *)rcvMsg->content;

    if (AT_RcvDrvAgentQryVersionRspParamCheck(versionQryCnfInfo->atAppCtrl.clientId, &indexNum, &modemId) != VOS_OK) {
        return VOS_ERR;
    }

    opt = g_atClientTab[indexNum].cmdCurrentOpt;

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* ��ѯ���� */
    if (versionQryCnfInfo->result != DRV_AGENT_VERSION_QRY_NO_ERROR) {
        g_atSendDataBuff.bufLen = 0;
        At_FormatResultData(indexNum, AT_ERROR);
        return VOS_OK;
    }

    /* �汾����ʱ���ʽת������ʱ��������������ո�ĺ�һ���ո���0�滻 */
    len       = VOS_StrLen(versionQryCnfInfo->verTime);
    memResult = memcpy_s(acTmpTime, sizeof(acTmpTime), versionQryCnfInfo->verTime, len + 1);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(acTmpTime), len + 1);
    At_ZeroReplaceBlankInString((VOS_UINT8 *)acTmpTime, len);

    /* �ַ���Ԥ���� */
    length = TAF_CDROM_VERSION_LEN;
    At_DelCtlAndBlankCharWithEndPadding(versionQryCnfInfo->isoVer.isoInfo, &length);

    /* ��ȡ������Ƿ�Ϊ�յı�ʶ */
    if (AT_PhyNumIsNull(modemId, AT_PHYNUM_TYPE_IMEI, &bPhyNumIsNull) != AT_OK) {
        AT_WARN_LOG("AT_RcvDrvAgentQryVersionRsp(): AT_PhyNumIsNull Error!");
        g_atSendDataBuff.bufLen = 0;
        At_FormatResultData(indexNum, AT_ERROR);
        return VOS_OK;
    }

    length = 0;

    /*
     * �ڲ��汾���ڵ���δд�������ǰ����ѯ����ʵ��ʾ��д������ź����ѯ�汾��ǰδ
     * �������ָ��ڲ��汾����ʾΪ�գ�����Ѿ��������ָ��ڲ��汾����ʵ��ʾ
     */
    if (g_atDataLocked == VOS_FALSE || (bPhyNumIsNull == VOS_TRUE)) {
        AT_PrintfVersionInfoWithNullInnerVersion(versionQryCnfInfo, acTmpTime, VOS_StrLen(acTmpTime), csver, &length);
    } else {
        AT_PrintfVersionInfoWithValidInnerVersion(versionQryCnfInfo, acTmpTime, VOS_StrLen(acTmpTime), csver, &length);
    }

    /* �����^RSFR�����Ĳ�ѯVERSION��Ϣ������,����^RSFR����ȥ���� */
    if (opt == AT_CMD_RSFR_VERSION_QRY) {
        AT_SetRsfrVersionCnf(indexNum, g_atSndCodeAddress, g_atSendDataBuff.bufLen);

        /* AT���ص��ַ�����AT_SetRsfrVersionCnf�д��� */
        return VOS_OK;
    }

    At_FormatResultData(indexNum, AT_OK);
    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentSetGpioplRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg          *rcvMsg             = VOS_NULL_PTR;
    DRV_AGENT_GpioplSetCnf *drvAgentGpioSetCnf = VOS_NULL_PTR;
    VOS_UINT8               indexNum           = 0;
    VOS_UINT32              result;

    rcvMsg             = (DRV_AGENT_Msg *)msg;
    drvAgentGpioSetCnf = (DRV_AGENT_GpioplSetCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (AT_GetIndexNumByClientId(drvAgentGpioSetCnf->atAppCtrl.clientId, AT_CMD_GPIOPL_SET, &indexNum) !=
        VOS_OK) {
        return VOS_ERR;
    }

    AT_StopTimerCmdReady(indexNum);

    if (drvAgentGpioSetCnf->fail == VOS_TRUE) {
        result = AT_ERROR;
    } else {
        result = AT_OK;
    }

    /* ����AT_FormATResultDATa���������� */
    g_atSendDataBuff.bufLen = 0;
    At_FormatResultData(indexNum, result);
    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentQryGpioplRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg          *rcvMsg     = VOS_NULL_PTR;
    DRV_AGENT_GpioplQryCnf *gpioQryCnf = VOS_NULL_PTR;
    VOS_UINT16              length;
    VOS_UINT32              i;
    VOS_UINT8               indexNum = 0;
    VOS_UINT32              result;

    /* ��ʼ����Ϣ����ȡucContent */
    rcvMsg     = (DRV_AGENT_Msg *)msg;
    gpioQryCnf = (DRV_AGENT_GpioplQryCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (AT_GetIndexNumByClientId(gpioQryCnf->atAppCtrl.clientId, AT_CMD_GPIOPL_QRY, &indexNum) != VOS_OK) {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);

    if (gpioQryCnf->fail != VOS_TRUE) {
        /* ��ӡ��� */
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);

        for (i = 0; i < DRVAGENT_GPIOPL_MAX_LEN; i++) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)(g_atSndCodeAddress + length), "%02X", gpioQryCnf->gpiopl[i]);
        }

        g_atSendDataBuff.bufLen = length;
        result                  = AT_OK;
    } else {
        /* ����AT_FormATResultDATa���������� */
        g_atSendDataBuff.bufLen = 0;
        result                  = AT_ERROR;
    }
    At_FormatResultData(indexNum, result);
    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentHkAdcGetRsp(struct MsgCB *msg)
{
    VOS_UINT32             ret;
    VOS_UINT8              indexNum = 0;
    DRV_AGENT_HkadcGetCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg         *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_HkadcGetCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentTseLrfSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentTseLrfSetRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�HKADC��ѹ��ѯ����Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_TBAT_SET) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �����ѯ��� */
    g_atSendDataBuff.bufLen = 0;
    if (event->result == DRV_AGENT_HKADC_GET_NO_ERROR) {
        /* ���ô�����ΪAT_OK */
        g_atSendDataBuff.bufLen =
            (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress, "%s:1,%d", g_parseContext[indexNum].cmdElement->cmdName,
                event->tbatHkadc);

        ret = AT_OK;

    } else {
        /* ��ѯʧ�ܷ���ERROR�ַ��� */
        ret = AT_ERROR;
    }

    At_FormatResultData(indexNum, ret);
    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentQryTbatRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg        *rcvMsg      = VOS_NULL_PTR;
    DRV_AGENT_TbatQryCnf *tbatTypeCnf = VOS_NULL_PTR;
    VOS_UINT8             indexNum    = 0;

    /* ��ʼ����Ϣ */
    rcvMsg      = (DRV_AGENT_Msg *)msg;
    tbatTypeCnf = (DRV_AGENT_TbatQryCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (AT_GetIndexNumByClientId(tbatTypeCnf->atAppCtrl.clientId, AT_CMD_TBAT_QRY, &indexNum) != VOS_OK) {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);
    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
            tbatTypeCnf->tbatType);

    /* ����At_FormatResultData���������� */
    At_FormatResultData(indexNum, AT_OK);
    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentChipTempQryRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg            *rcvMsg   = VOS_NULL_PTR;
    DRV_AGENT_ChiptempQryCnf *event    = VOS_NULL_PTR;
    VOS_UINT8                 indexNum = 0;
    VOS_UINT32                result;
    VOS_UINT16                length;

    /* ��ʼ����Ϣ���� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_ChiptempQryCnf *)rcvMsg->content;
    length = 0;

    /* ͨ��clientid��ȡindex */
    if (AT_GetIndexNumByClientId(event->atAppCtrl.clientId, AT_CMD_CHIPTEMP_READ, &indexNum) != VOS_OK) {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �жϲ�ѯ�����Ƿ�ɹ� */
    if (event->result == DRV_AGENT_NO_ERROR) {
        /* �����ѯ��� */
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d,%d,%d,%d,%d",
            g_parseContext[indexNum].cmdElement->cmdName, event->gpaTemp, event->wpaTemp, event->lpaTemp,
            event->simTemp, event->batTemp);

        result = AT_OK;
    } else {
        result = AT_ERROR;
    }

    g_atSendDataBuff.bufLen = length;
    /* ����AT_FormATResultDATa���������� */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentNvBackupStatQryRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg                *rcvMsg   = VOS_NULL_PTR;
    DRV_AGENT_NvbackupstatQryCnf *event    = VOS_NULL_PTR;
    VOS_UINT8                     indexNum = 0;
    VOS_UINT32                    result;
    VOS_UINT16                    length;

    /* ��ʼ����Ϣ���� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_NvbackupstatQryCnf *)rcvMsg->content;
    length = 0;

    /* ͨ��clientid��ȡindex */
    if (AT_GetIndexNumByClientId(event->atAppCtrl.clientId, AT_CMD_NVBACKUPSTAT_READ, &indexNum) != VOS_OK) {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �жϲ�ѯ�����Ƿ�ɹ� */
    if (event->result == DRV_AGENT_NO_ERROR) {
        if (event->nvBackupStat == NV_OK) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
                VOS_TRUE);
        } else {
            length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
                VOS_FALSE);
        }

        result = AT_OK;
    } else {
        result = AT_ERROR;
    }

    g_atSendDataBuff.bufLen = length;
    /* ����AT_FormATResultDATa���������� */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentNandBadBlockQryRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg           *rcvMsg   = VOS_NULL_PTR;
    DRV_AGENT_NandbbcQryCnf *event    = VOS_NULL_PTR;
    VOS_UINT8                indexNum = 0;
    VOS_UINT32               result;
    VOS_UINT16               length;
    VOS_UINT32               uli;
    VOS_UINT32               maxBBNum;

    /* ��ʼ����Ϣ���� */
    rcvMsg   = (DRV_AGENT_Msg *)msg;
    event    = (DRV_AGENT_NandbbcQryCnf *)rcvMsg->content;
    length   = 0;
    uli      = 0;
    maxBBNum = 0;

    /* ͨ��clientid��ȡindex */
    if (AT_GetIndexNumByClientId(event->atAppCtrl.clientId, AT_CMD_NANDBBC_READ, &indexNum) != VOS_OK) {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �жϲ�ѯ�����Ƿ�ɹ� */
    if (event->result == DRV_AGENT_NO_ERROR) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            event->badBlockNum);

        maxBBNum = (event->badBlockNum > DRV_AGENT_NAND_BADBLOCK_MAX_NUM) ? DRV_AGENT_NAND_BADBLOCK_MAX_NUM :
                                                                            event->badBlockNum;
        for (uli = 0; uli < maxBBNum; uli++) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                (TAF_CHAR *)g_atSndCodeAddress + length, "%s%s: %d", g_atCrLf,
                g_parseContext[indexNum].cmdElement->cmdName, event->badBlockIndex[uli]);
        }

        result = AT_OK;
    } else {
        result = AT_ERROR;
    }

    g_atSendDataBuff.bufLen = length;
    /* ����AT_FormATResultDATa���������� */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentNandDevInfoQryRsp(struct MsgCB *msg)
{
    DRV_AGENT_Msg           *rcvMsg   = VOS_NULL_PTR;
    DRV_AGENT_NandverQryCnf *event    = VOS_NULL_PTR;
    VOS_UINT8                indexNum = 0;
    VOS_UINT32               result;
    VOS_UINT16               length;

    /* ��ʼ����Ϣ���� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_NandverQryCnf *)rcvMsg->content;
    length = 0;

    /* ͨ��clientid��ȡindex */
    if (AT_GetIndexNumByClientId(event->atAppCtrl.clientId, AT_CMD_NANDVER_READ, &indexNum) != VOS_OK) {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �жϲ�ѯ�����Ƿ�ɹ� */
    if (event->result == DRV_AGENT_NO_ERROR) {
        /* �����ѯ��� */

        /* buf���ⲿ�ӿڷ��صģ����ӽ���������ֹ��������� */
        event->nandDevInfo.mufName[DRV_AGENT_NAND_MFU_NAME_MAX_LEN - 1] = '\0';
        event->nandDevInfo.devSpec[DRV_AGENT_NAND_DEV_SPEC_MAX_LEN - 1] = '\0';
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d,\"%s\",%d,\"%s\"",
            g_parseContext[indexNum].cmdElement->cmdName, event->nandDevInfo.mufId, event->nandDevInfo.mufName,
            event->nandDevInfo.devId, event->nandDevInfo.devSpec);

        result = AT_OK;
    } else {
        result = AT_ERROR;
    }

    g_atSendDataBuff.bufLen = length;
    /* ����AT_FormATResultDATa���������� */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}

#if (FEATURE_LTE == FEATURE_ON)
VOS_UINT32 AT_RcvDrvAgentInfoRrsSetRsp(struct MsgCB *msg)
{
    VOS_UINT8                indexNum = 0;
    DRV_AGENT_InforrsSetCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg           *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_InforrsSetCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�INFORRU��������Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_INFORRS_SET) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* ������ò������ :  */
    if (event->rslt == NV_OK) {
        At_FormatResultData(indexNum, AT_OK);
    } else {
        At_FormatResultData(indexNum, AT_ERROR);
    }

    return VOS_OK;
}
#endif

VOS_UINT32 AT_RcvMtaPmuDieSNQryCnf(VOS_VOID *msg, VOS_UINT8 indexNum)
{
    AT_MTA_Msg            *rcvMsg         = VOS_NULL_PTR;
    MTA_AT_PmuDieSnReqCnf *pmuDieSNReqCnf = VOS_NULL_PTR;
    VOS_INT32              i; /* ѭ���� */
    VOS_UINT16             length;

    /* ��ʼ���ֲ����� */
    rcvMsg         = (AT_MTA_Msg *)msg;
    pmuDieSNReqCnf = (MTA_AT_PmuDieSnReqCnf *)rcvMsg->content;
    length         = 0;

    /* ��ʽ���ϱ����� */
    if (pmuDieSNReqCnf->result != MTA_AT_RESULT_NO_ERROR) {
        /* ������ *AT_ERROR */
        g_atSendDataBuff.bufLen = 0;
        return AT_ERROR;
    } else {
        /* ������ *AT_OK */

        /* ���λ,��4 BIT��0 */
        pmuDieSNReqCnf->dieSn[MTA_PMU_MAX_DIE_SN_LEN - 1] = (pmuDieSNReqCnf->dieSn[MTA_PMU_MAX_DIE_SN_LEN - 1] & 0x0F);

        /* ��ʽ�������ѯ��� */
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: 0x", g_parseContext[indexNum].cmdElement->cmdName);

        for (i = (MTA_PMU_MAX_DIE_SN_LEN - 1); i >= 0; i--) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%02x", pmuDieSNReqCnf->dieSn[i]);
        }

        g_atSendDataBuff.bufLen = length;
        return AT_OK;
    }
}

VOS_UINT32 AT_RcvDrvAgentDloadInfoQryRsp(struct MsgCB *msg)
{
    VOS_UINT32                 ret;
    VOS_UINT8                  indexNum = 0;
    DRV_AGENT_DloadinfoQryCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg             *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_DloadinfoQryCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentDloadInfoQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentDloadInfoQryRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�DLOADINFO��ѯ����Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_DLOADINFO_READ) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �����ѯ��� */
    if (event->result == DRV_AGENT_DLOADINFO_QRY_NO_ERROR) {
        /*
         * ���ô�����ΪAT_OK
         *   ����ṹΪ^DLOADINFO:<CR><LF>
         * <CR><LF>swver:<software version><CR><LF>
         * <CR><LF>isover:<iso version><CR><LF>
         * <CR><LF>product name:<product name><CR><LF>
         * <CR><LF>product name:<WebUiVer><CR><LF>
         * <CR><LF>dload type: <dload type><CR><LF>
         * <CR><LF>OK<CR><LF>��ʽ
         */
        ret                     = AT_OK;
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s", event->dlodInfo);

    } else {
        /* ��ѯʧ�ܷ���ERROR�ַ��� */
        ret                     = AT_ERROR;
        g_atSendDataBuff.bufLen = 0;
    }

    /* ����At_FormatResultData������ */
    At_FormatResultData(indexNum, ret);
    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentHwnatQryRsp(struct MsgCB *msg)
{
    VOS_UINT32                ret;
    VOS_UINT8                 indexNum = 0;
    DRV_AGENT_HwnatqryQryCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg            *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_HwnatqryQryCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentHwnatQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentHwnatQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�HWNAT��ѯ����Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_HWNATQRY_READ) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �����ѯ��� */
    if (event->result == DRV_AGENT_HWNATQRY_QRY_NO_ERROR) {
        /*
         * ���ô�����ΪAT_OK
         * ����ṹΪ^HWNATQRY: <cur_mode> ��ʽ
         */
        ret                     = AT_OK;
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, event->netMode);
    } else {
        /* ��ѯʧ�ܷ���ERROR�ַ��� */
        ret                     = AT_ERROR;
        g_atSendDataBuff.bufLen = 0;
    }

    /* ����At_FormatResultData������ */
    At_FormatResultData(indexNum, ret);
    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentAuthorityIdQryRsp(struct MsgCB *msg)
{
    VOS_UINT32                   ret;
    VOS_UINT8                    indexNum = 0;
    DRV_AGENT_AuthorityidQryCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg               *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_AuthorityidQryCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentAuthorityIdQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentAuthorityIdQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�AUTHORITYID��ѯ����Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_AUTHORITYID_READ) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �����ѯ��� :  */
    if (event->result == DRV_AGENT_AUTHORITYID_QRY_NO_ERROR) {
        /*
         * ���ô�����ΪAT_OK ��ʽΪ<CR><LF><Authority ID>, <Authority Type><CR><LF>
         * <CR><LF>OK<CR><LF>
         */
        /* buf���ⲿ�ӿڷ��صģ����ӽ���������ֹ��������� */
        event->authorityId[TAF_AUTH_ID_LEN] = '\0';
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s", event->authorityId);

        ret = AT_OK;
    } else {
        /* ��ѯʧ�ܷ���ERROR�ַ��� */
        ret                     = AT_ERROR;
        g_atSendDataBuff.bufLen = 0;
    }

    /* ����At_FormatResultData������ */
    At_FormatResultData(indexNum, ret);
    return VOS_OK;
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
VOS_UINT32 AT_RcvDrvAgentGodloadSetRsp(struct MsgCB *msg)
{
    VOS_UINT32               ret;
    VOS_UINT8                indexNum = 0;
    DRV_AGENT_GodloadSetCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg           *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_GodloadSetCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentGodloadSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentGodloadSetRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�GODLOAD��ѯ����Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_GODLOAD_SET) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* ������ò������ :  */
    g_atSendDataBuff.bufLen = 0;
    if (event->result == DRV_AGENT_GODLOAD_SET_NO_ERROR) {
        /* ���ô�����ΪAT_OK */
        ret = AT_OK;
    } else {
        /* ����ʧ�ܷ���ERROR�ַ��� */
        ret = AT_ERROR;
    }

    /* ����At_FormatResultData������ */
    At_FormatResultData(indexNum, ret);
    return VOS_OK;
}
#endif

VOS_UINT32 AT_RcvDrvAgentInfoRbuSetRsp(struct MsgCB *msg)
{
    VOS_UINT8                indexNum = 0;
    DRV_AGENT_InforbuSetCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg           *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_InforbuSetCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentInfoRbuSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentInfoRbuSetRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�INFORBU��������Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_INFORBU_SET) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* ������ò������ :  */
    if (event->rslt == NV_OK) {
        At_FormatResultData(indexNum, AT_OK);
    } else {
        At_FormatResultData(indexNum, AT_ERROR);
    }

    return VOS_OK;
}

VOS_UINT32 AT_RcvDrvAgentNvBackupSetRsp(struct MsgCB *msg)
{
    VOS_UINT8                 indexNum = 0;
    DRV_AGENT_NvbackupSetCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg            *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_NvbackupSetCnf *)rcvMsg->content;

    AT_PR_LOGI("Rcv Msg");

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentNvBackupSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentNvBackupSetRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�NVBACKUP��������Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_NVBACKUP_SET) {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    if (event->rslt == NV_OK) {
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%d", event->rslt);
        At_FormatResultData(indexNum, AT_OK);
    } else {
        At_FormatResultData(indexNum, AT_ERROR);
    }

    return VOS_OK;
}

#if ((FEATURE_VSIM == FEATURE_ON) && (FEATURE_VSIM_ICC_SEC_CHANNEL == FEATURE_OFF))
VOS_UINT32 AT_RcvDrvAgentHvpdhSetCnf(struct MsgCB *msg)
{
    VOS_UINT32          ret;
    VOS_UINT8           indexNum;
    DRV_AGENT_HvpdhCnf *event  = VOS_NULL_PTR;
    DRV_AGENT_Msg      *rcvMsg = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_HvpdhCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvDrvAgentHvpdhSetCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvDrvAgentHvpdhSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�HVPDH��������Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_HVPDH_SET) {
        AT_WARN_LOG("AT_RcvDrvAgentHvpdhSetCnf: WARNING:Not AT_CMD_HVPDH_SET!");
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    /* �����ѯ��� */
    if (event->result == DRV_AGENT_HVPDH_NO_ERROR) {
        ret = AT_OK;
    } else {
        /* ��ѯʧ�ܷ���ERROR�ַ��� */
        ret = AT_ERROR;
    }

    g_atSendDataBuff.usBufLen = 0;

    /* ����At_FormatResultData������ */
    At_FormatResultData(indexNum, ret);
    return VOS_OK;
}
#endif

VOS_UINT32 AT_RcvNvManufactureExtSetCnf(struct MsgCB *msg)
{
    VOS_UINT8                         indexNum = 0;
    DRV_AGENT_NvmanufactureextSetCnf *event    = VOS_NULL_PTR;
    DRV_AGENT_Msg                    *rcvMsg   = VOS_NULL_PTR;

    /* ��ʼ�� */
    rcvMsg = (DRV_AGENT_Msg *)msg;
    event  = (DRV_AGENT_NvmanufactureextSetCnf *)rcvMsg->content;

    /* ͨ��clientid��ȡindex */
    if (At_ClientIdToUserId(event->atAppCtrl.clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvNvManufactureExtSetCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IsBroadcastClientIndex(indexNum)) {
        AT_WARN_LOG("AT_RcvNvManufactureExtSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�NvManufactureExt��������Ľ���¼��ϱ� */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_NVMANUFACTUREEXT_SET) {
        AT_WARN_LOG("AT_RcvNvManufactureExtSetCnf: WARNING:Not AT_CMD_NVMANUFACTUREEXT_SET!");
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_StopTimerCmdReady(indexNum);

    if (event->rslt == NV_OK) {
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%d", event->rslt);
        At_FormatResultData(indexNum, AT_OK);
    } else {
        At_FormatResultData(indexNum, AT_ERROR);
    }

    return VOS_OK;
}

VOS_UINT32 atSetTbatCnf(VOS_UINT8 clientId, struct MsgCB *msgBlock)
{
    OS_MSG         *event = NULL;
    FTM_SetTbatCnf *cnf   = NULL;

    event = (OS_MSG *)(((MsgBlock *)msgBlock)->value);
    cnf   = (FTM_SetTbatCnf *)event->param1;
    HAL_SDMLOG("\n enter into atSetTbatCnf\n");
    CmdErrProc(clientId, cnf->errCode, 0, NULL);
    return AT_OK;
}

VOS_UINT32 atRdTbatCnf(VOS_UINT8 clientId, struct MsgCB *msgBlock)
{
    OS_MSG        *event = NULL;
    FTM_RdTbatCnf *cnf   = NULL;

    event = (OS_MSG *)(((MsgBlock *)msgBlock)->value);
    cnf   = (FTM_RdTbatCnf *)event->param1;
    HAL_SDMLOG("\n enter into atRdTbatCnf\n");
    HAL_SDMLOG("\n pstCnf->ucType=%d,pstCnf->usValue=%d \n", (VOS_INT)cnf->type, (VOS_INT)cnf->value);

    g_atSendData.bufLen = 0;
    g_atSendData.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddr,
        (VOS_CHAR *)g_atSndCodeAddr, "^TBAT:%d,%d", cnf->type, cnf->value);

    CmdErrProc(clientId, cnf->errCode, g_atSendData.bufLen, g_atSndCodeAddr);

    return AT_OK;
}

