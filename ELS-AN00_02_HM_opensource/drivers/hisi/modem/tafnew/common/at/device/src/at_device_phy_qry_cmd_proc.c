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
#include "at_device_phy_qry_cmd_proc.h"
#include "securec.h"

#include "at_snd_msg.h"
#include "at_mta_interface.h"
#include "at_cmd_proc.h"
#include "at_lte_common.h"
#include "at_device_comm.h"
#include "at_mt_msg_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_PHY_QRY_CMD_PROC_C

#define AT_FTEMPRPT_PARA_NUM 2

VOS_UINT32 AT_QryProdTypePara(VOS_UINT8 indexNum)
{
    if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                               DRV_AGENT_PRODTYPE_QRY_REQ, VOS_NULL_PTR, 0, I0_WUEPS_PID_DRV_AGENT) == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PRODTYPE_QRY; /* ���õ�ǰ����ģʽ */
        return AT_WAIT_ASYNC_RETURN;                                 /* �ȴ��첽�¼����� */
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_QryRficDieIDPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* ���Ϳ����Ϣ��C��, ��ѯRFIC IDE ID */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_RFIC_DIE_ID_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryRficDieIDPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_RFIC_DIE_ID_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryRffeDieIDPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* ���Ϳ����Ϣ��C��, ��ѯRFFE IDE ID */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_RFFE_DIE_ID_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryRffeDieIDPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_RFFE_DIE_ID_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

/*
 * ��������: ����ͨ��AT�����ѯ��TL C��
 */

VOS_UINT32 atQryLTCommCmdPara(VOS_UINT8 clientId)
{
    FTM_RdNopraraReq qryReq = {0};
    VOS_UINT32       rst;

    rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_RD_LTCOMMCMD_REQ, clientId, (VOS_UINT8 *)(&qryReq),
                           sizeof(qryReq));

    if (rst == AT_SUCCESS) {
        g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_LTCOMMCMD_READ;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

/* ***************************************************************************** */

/* ��������: ��ѯ��������ŵ� */

/* ����˵��: */

/*   ulIndex [in] ... */

/* �� �� ֵ: */

/*    TODO: ... */
/* ***************************************************************************** */
VOS_UINT32 atQryFCHANPara(VOS_UINT8 clientId)
{
    FTM_RdFchanReq fCHANQryReq = {0};
    VOS_UINT32     rst;

    rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_RD_FCHAN_REQ, clientId, (VOS_UINT8 *)(&fCHANQryReq),
                           sizeof(fCHANQryReq));

    if (rst == AT_SUCCESS) {
        g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_FCHAN_READ;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

/* ***************************************************************************** */

/* ��������: ��ѯ�����ŵ���״̬ */

/*  */

/* ����˵��: */

/*   ulIndex [in] ... */

/*                ... */

/*  */

/* �� �� ֵ: */

/*    TODO: ... */

/* ***************************************************************************** */
VOS_UINT32 atQryFTXONPara(VOS_UINT8 clientId)
{
    FTM_RdTxonReq fTXONQryReq = {0};
    VOS_UINT32    rst;


    rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_RD_TXON_REQ, clientId, (VOS_UINT8 *)(&fTXONQryReq),
                           sizeof(fTXONQryReq));

    if (rst == AT_SUCCESS) {
        g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_FTXON_READ;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

#if (FEATURE_LTE == FEATURE_ON)
VOS_UINT32 AT_QryLCaCellExPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* ���Ϳ����Ϣ��C��, ��ѯLCACELLEX�б����� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_LTE_CA_CELLEX_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryLCaCellExPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LTE_CA_CELLEX_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryLcaCellRptCfgPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* ���Ϳ����Ϣ��C��, ��ѯLCACELLRPTCFG�б����� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_LTE_CA_CELL_RPT_CFG_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryLcaCellRptCfgPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LCACELLRPTCFG_QRY;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

/* ***************************************************************************** */

/* ��������: ��ѯRSSI */

/* ����˵��: */

/*   ulIndex [in] ... */

/* �� �� ֵ: */

/*    TODO: ... */

/* ***************************************************************************** */
VOS_UINT32 atQryFRSSIPara(VOS_UINT8 clientId)
{
    FTM_FRSSI_Req fRssiQryReq = {0};
    VOS_UINT32    rst;

    rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_FRSSI_REQ, clientId, (VOS_UINT8 *)(&fRssiQryReq),
                           sizeof(fRssiQryReq));

    if (rst == AT_SUCCESS) {
        g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_FRSSI_READ;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 atQryFPllStatusPara(VOS_UINT8 clientId)
{
    FTM_RdFpllstatusReq fPLLSTATUSQryReq = {0};
    VOS_UINT32          rst;

    rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_RD_FPLLSTATUS_REQ, clientId, (VOS_UINT8 *)(&fPLLSTATUSQryReq),
                           sizeof(fPLLSTATUSQryReq));
    if (rst == AT_SUCCESS) {
        g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_FPLLSTATUS_READ;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 atQryFRXONPara(VOS_UINT8 clientId)
{
    FTM_RdRxonReq fRXONQryReq = {0};
    VOS_UINT32    rst;

    rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_RD_RXON_REQ, clientId, (VOS_UINT8 *)(&fRXONQryReq),
                           sizeof(fRXONQryReq));

    if (rst == AT_SUCCESS) {
        g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_FRXON_READ;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)

VOS_UINT32 At_QryFChanPara(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
    TAF_UINT16         length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
#if (FEATURE_LTE == FEATURE_ON)
    if ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_FDD_LTE) ||
#if (FEATURE_LTEV == FEATURE_ON)
        (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_LTEV) ||
#endif
        (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_TDD_LTE)) {
        return atQryFCHANPara(indexNum);
    }
#endif

    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ѯ��ǰFCHAN������ */
    length = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%d,%d,%d,%d", atDevCmdCtrl->deviceRatMode,
        atDevCmdCtrl->deviceAtBand, atDevCmdCtrl->dspBandArfcn.ulArfcn, atDevCmdCtrl->dspBandArfcn.dlArfcn);
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

/* ***************************************************************************** */

/* ��������: AT^TSELRF  ѡ����Ƶͨ·ָ�� */

/*  */

/* ����˵��: */

/*   ulIndex [in] ... */

/*                ... */

/*  */

/* �� �� ֵ: */

/*    TODO: ... */

/* ***************************************************************************** */

VOS_UINT32 atQryTselrfPara(VOS_UINT8 clientId)
{
    /*
     * ƽ̨���ṩ�ýӿڣ��ɲ�Ʒ��ʵ�֡�����ʹ����������NV���ȡ:
     * 10000 NV_WG_RF_MAIN_BAND
     * 0xD22C NV_ID_UE_CAPABILITY
     * 0xD304 EN_NV_ID_TDS_SUPPORT_FREQ_BAND
     */
    return AT_OK;
}

VOS_UINT32 AT_QryTSelRfPara(VOS_UINT8 indexNum)
{
#if (FEATURE_LTE == FEATURE_ON)
    return atQryTselrfPara(indexNum);
#else
    VOS_UINT16 length;

    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%d%s", g_parseContext[indexNum].cmdElement->cmdName, AT_TSELRF_PATH_TOTAL,
        g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s:%d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_GSM, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s:%d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_WCDMA_PRI, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s:%d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_WCDMA_DIV, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_WIFI);

    g_atSendDataBuff.usBufLen = length;

    return AT_OK;
#endif
}

VOS_UINT32 At_QryCltInfo(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
    VOS_UINT16         length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    length       = 0;

    /* �����¼��Ч�����ϱ���ѯ��� */
    if (atDevCmdCtrl->cltInfo.infoAvailableFlg == VOS_TRUE) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length,
            "%s%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
            "^CLTINFO: ", atDevCmdCtrl->cltInfo.gammaReal, /* ����ϵ��ʵ�� */
            atDevCmdCtrl->cltInfo.gammaImag,               /* ����ϵ���鲿 */
            atDevCmdCtrl->cltInfo.gammaAmpUc0,             /* פ����ⳡ��0����ϵ������ */
            atDevCmdCtrl->cltInfo.gammaAmpUc1,             /* פ����ⳡ��1����ϵ������ */
            atDevCmdCtrl->cltInfo.gammaAmpUc2,             /* פ����ⳡ��2����ϵ������ */
            atDevCmdCtrl->cltInfo.gammaAntCoarseTune,      /* �ֵ����λ�� */
            atDevCmdCtrl->cltInfo.ulwFomcoarseTune,        /* �ֵ�FOMֵ */
            atDevCmdCtrl->cltInfo.cltAlgState,             /* �ջ��㷨����״̬ */
            atDevCmdCtrl->cltInfo.cltDetectCount,          /* �ջ������ܲ��� */
            atDevCmdCtrl->cltInfo.dac0,                    /* DAC0 */
            atDevCmdCtrl->cltInfo.dac1,                    /* DAC1 */
            atDevCmdCtrl->cltInfo.dac2,                    /* DAC2 */
            atDevCmdCtrl->cltInfo.dac3);                   /* DAC3 */

        /* �ϱ��󱾵ؼ�¼����Ч */
        atDevCmdCtrl->cltInfo.infoAvailableFlg = VOS_FALSE;
    }

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_QryFTxonPara(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
    TAF_UINT16         length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    /* ��� LTE ģ�Ľӿڷ�֧ */
#if (FEATURE_LTE == FEATURE_ON)
    if ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_FDD_LTE) || (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_TDD_LTE)) {
        return atQryFTXONPara(indexNum);
    }
#endif

    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }
    /* ��ѯ��ǰDAC������ */
    length = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress + length, "%d", atDevCmdCtrl->txOnOff);
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

/*
 * ��������: ^DCXOTEMPCOMP�Ĳ�ѯ�������
 */
VOS_UINT32 AT_QryDcxotempcompPara(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }
    /*  ���Ƿ�����ģʽ�·����ش��� */
    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }
    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            atDevCmdCtrl->dcxoTempCompEnableFlg);

    return AT_OK;
}

VOS_UINT32 At_QryFRxonPara(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;
    VOS_UINT16         length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    /* ���LTE ģ�Ľӿڷ�֧ */
#if (FEATURE_LTE == FEATURE_ON)
    if ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_FDD_LTE) || (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_TDD_LTE)) {
        return atQryFRXONPara(indexNum);
    }
#endif

    /* ��ǰ��Ϊ������ģʽ */
    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ѯ��ǰ���ջ�����״̬ */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, atDevCmdCtrl->rxOnOff);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryFpowdetTPara(VOS_UINT8 indexNum)
{
    AT_PHY_PowerDetReq *msg          = VOS_NULL_PTR;
    AT_DEVICE_CmdCtrl  *atDevCmdCtrl = VOS_NULL_PTR;
    VOS_UINT32          length;
    VOS_UINT32          result;
    VOS_UINT8           isLteFlg;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    isLteFlg     = VOS_FALSE;

    /* �жϵ�ǰ����ģʽ��ֻ֧��W */
    if ((atDevCmdCtrl->deviceRatMode != AT_RAT_MODE_WCDMA)
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
        && (atDevCmdCtrl->deviceRatMode != AT_RAT_MODE_CDMA)
#endif
        && (atDevCmdCtrl->deviceRatMode != AT_RAT_MODE_GSM)
        && (atDevCmdCtrl->deviceRatMode != AT_RAT_MODE_FDD_LTE) &&
        (atDevCmdCtrl->deviceRatMode != AT_RAT_MODE_TDD_LTE)) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ����AT_PHY_PowerDetReq��Ϣ */
    length = sizeof(AT_PHY_PowerDetReq) - VOS_MSG_HEAD_LENGTH;
    /*lint -save -e516 */
    msg = (AT_PHY_PowerDetReq *)PS_ALLOC_MSG(WUEPS_PID_AT, length);
    /*lint -restore */
    if (msg == VOS_NULL_PTR) {
        AT_WARN_LOG("AT_QryFpowdetTPara: Alloc msg fail!");
        return AT_ERROR;
    }

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    /* CDMA�Ļ������͸�UPHY_PID_CSDR_1X_CM */
    if (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_CDMA) {
        TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, UPHY_PID_CSDR_1X_CM, length);
    }
    else
#endif
    if (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_GSM) {
        TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, AT_GetDestPid(indexNum, I0_DSP_PID_GPHY), length);
    }
    else if (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_WCDMA) {
        TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, AT_GetDestPid(indexNum, I0_DSP_PID_WPHY), length);
    } else {
        isLteFlg = VOS_TRUE;
    }

    if (isLteFlg == VOS_FALSE) {
        msg->msgId = ID_AT_PHY_POWER_DET_REQ;
        msg->rsv   = 0;

        /* ���ӦPHY������Ϣ */
        result = TAF_TraceAndSendMsg(WUEPS_PID_AT, msg);
    } else {
        /*lint --e{516,830} */
        PS_FREE_MSG(WUEPS_PID_AT, msg);
        msg = VOS_NULL_PTR;
        result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                        ID_AT_MTA_POWER_DET_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);
    }

    if (result != VOS_OK) {
        AT_WARN_LOG("AT_QryFpowdetTPara: Send msg fail!");
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FPOWDET_QRY;
    atDevCmdCtrl->index                   = indexNum;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryFPllStatusPara(VOS_UINT8 indexNum)
{
    AT_DEVICE_CmdCtrl     *atDevCmdCtrl = VOS_NULL_PTR;
    AT_PHY_RfPllStatusReq *msg          = VOS_NULL_PTR;
    VOS_UINT32             receiverPid;
    VOS_UINT32             length;
    VOS_UINT16             msgId;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
#if (FEATURE_LTE == FEATURE_ON)
    if ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_FDD_LTE) || (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_TDD_LTE)) {
        return atQryFPllStatusPara(indexNum);
    }
#endif
    /* �жϵ�ǰ����ģʽ��ֻ֧��G/W */
    if (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_WCDMA) {
        receiverPid = AT_GetDestPid(indexNum, I0_DSP_PID_WPHY);
        msgId       = ID_AT_WPHY_RF_PLL_STATUS_REQ;
    } else if ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_GSM) || (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_EDGE)) {
        receiverPid = AT_GetDestPid(indexNum, I0_DSP_PID_GPHY);
        msgId       = ID_AT_GPHY_RF_PLL_STATUS_REQ;
    } else {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ����AT_PHY_RfPllStatusReq��Ϣ */
    length = sizeof(AT_PHY_RfPllStatusReq) - VOS_MSG_HEAD_LENGTH;
    /*lint -save -e516 */
    msg = (AT_PHY_RfPllStatusReq *)TAF_AllocMsgWithoutHeaderLen(WUEPS_PID_AT, length);
    /*lint -restore */
    if (msg == VOS_NULL_PTR) {
        AT_WARN_LOG("AT_QryFPllStatusPara: Alloc msg fail!");
        return AT_ERROR;
    }

    /* �����Ϣ */
    TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, receiverPid, length);
    msg->msgId   = msgId;
    msg->rsv1    = 0;
    msg->dspBand = atDevCmdCtrl->dspBandArfcn.dspBand;
    msg->rsv2    = 0;

    /* ���ӦPHY������Ϣ */
    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("AT_QryFPllStatusPara: Send msg fail!");
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FPLLSTATUS_QRY;
    atDevCmdCtrl->index                   = indexNum;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 At_QryFrssiPara(VOS_UINT8 indexNum)
{
    AT_HPA_RfRxRssiReq *msg          = VOS_NULL_PTR;
    AT_DEVICE_CmdCtrl  *atDevCmdCtrl = VOS_NULL_PTR;
    VOS_UINT32          length;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
#if (FEATURE_LTE == FEATURE_ON)
    if (At_IsLteRatMode(atDevCmdCtrl->deviceRatMode) == VOS_TRUE) {
        return atQryFRSSIPara(indexNum);
    }
#endif

#if (FEATURE_UE_MODE_TDS == FEATURE_ON)
    if (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_TDSCDMA) {
        return atQryFRSSIPara(indexNum);
    }
#endif

    /* ���������ڷ�����ģʽ��ʹ�� */
    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* �������������÷������ŵ���ʹ�� */
    if (atDevCmdCtrl->dspLoadFlag == VOS_FALSE) {
        return AT_CHANNEL_NOT_SET;
    }

    /* ��������Ҫ�ڴ򿪽��ջ���ʹ�� */
    if (atDevCmdCtrl->rxOnOff == AT_DSP_RF_SWITCH_OFF) {
        return AT_FRSSI_RX_NOT_OPEN;
    }

    /*
     * GDSP LOAD������²�֧�ֽ��ջ��ͷ����ͬʱ�򿪣���Ҫ�ж����һ��ִ�е��Ǵ򿪽��ջ�����
     * ���Ǵ򿪷��������������Ǵ򿪷������������ֱ�ӷ��س��������GDSP ����
     */
    if ((atDevCmdCtrl->rxonOrTxon == AT_TXON_OPEN) &&
        ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_GSM) || (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_EDGE))) {
        return AT_FRSSI_OTHER_ERR;
    }

    /* ����AT_HPA_RfRxRssiReq��Ϣ */
    length = sizeof(AT_HPA_RfRxRssiReq) - VOS_MSG_HEAD_LENGTH;
    /*lint -save -e830 */
    msg = (AT_HPA_RfRxRssiReq *)TAF_AllocMsgWithoutHeaderLen(WUEPS_PID_AT, length);
    /*lint -restore */
    if (msg == VOS_NULL_PTR) {
        AT_WARN_LOG("At_QryFrssiPara: alloc msg fail!");
        return AT_FRSSI_OTHER_ERR;
    }

    if ((atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_GSM) || (atDevCmdCtrl->deviceRatMode == AT_RAT_MODE_EDGE)) {
        TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, AT_GetDestPid(indexNum, I0_DSP_PID_GPHY), length);
        msg->msgId = ID_AT_GHPA_RF_RX_RSSI_REQ;
    } else {
        TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, AT_GetDestPid(indexNum, I0_DSP_PID_WPHY), length);
        msg->msgId = ID_AT_HPA_RF_RX_RSSI_REQ;
    }

    msg->measNum  = AT_DSP_RSSI_MEASURE_NUM;
    msg->interval = AT_DSP_RSSI_MEASURE_INTERVAL;
    msg->rsv      = 0;

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("At_QryFrssiPara: Send msg fail!");
        return AT_FRSSI_OTHER_ERR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_QUERY_RSSI;
    atDevCmdCtrl->index                   = indexNum;

    return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
}
#else

VOS_UINT32 At_QryFChanPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
    VOS_UINT16  length;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("At_QryFChanPara Enter");

    if (atMtInfoCtx->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ѯ��ǰFCHAN������ */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:", g_parseContext[indexNum].cmdElement->cmdName);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%d,%d,%d,%d,%d", atMtInfoCtx->atInfo.ratMode,
        atMtInfoCtx->atInfo.bandArfcn.dspBand, atMtInfoCtx->atInfo.bandArfcn.ulChanNo,
        atMtInfoCtx->atInfo.bandArfcn.dlChanNo, atMtInfoCtx->atInfo.bandWidth);

    if (atMtInfoCtx->atInfo.ratMode == AT_RAT_MODE_NR) {
        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + length, ",%d", atMtInfoCtx->atInfo.bbicScs);
    }

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryTSelRfPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length = 0;

    AT_PR_LOGH("AT_QryTSelRfPara Enter");

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_TOTAL_MT, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_GSM, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_WCDMA_PRI, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_WCDMA_DIV, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_CDMA_PRI, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_CDMA_DIV, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_TD, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_WIFI, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_FDD_LTE_PRI, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_FDD_LTE_DIV, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_FDD_LTE_MIMO, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_TDD_LTE_PRI, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_TDD_LTE_DIV, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_TDD_LTE_MIMO, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_NR_PRI, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d%s", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_NR_DIV, g_atCrLf);

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        AT_TSELRF_PATH_NR_MIMO);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_QryFTxonPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
    VOS_UINT16  length;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("At_QryFTxonPara Enter");

    if (atMtInfoCtx->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        atMtInfoCtx->atInfo.txOnOff);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_QryRfIcMemTest(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("At_QryRfIcMemTest Enter");

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            atMtInfoCtx->atInfo.rficTestResult);
    /* ���ײ��Ѿ����أ����ѯ����Ҫ������ */
    if (atMtInfoCtx->atInfo.rficTestResult != AT_RFIC_MEM_TEST_RUNNING) {
        atMtInfoCtx->atInfo.rficTestResult = AT_RFIC_MEM_TEST_NOT_START;
    }

    return AT_OK;
}

VOS_UINT32 At_QryFSerdesRt(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("At_QryFSerdesRt Enter");

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            atMtInfoCtx->rserTestResult);

    return AT_OK;
}

VOS_UINT32 AT_QryDcxotempcompPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("AT_QryDcxotempcompPara Enter");

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
            atMtInfoCtx->atInfo.dcxoTempCompEnableFlg);

    return AT_OK;
}

VOS_UINT32 AT_SndBbicCalQryFtemprptReq(INT16 channelNum)
{
    BBIC_CAL_TEMP_QRY_REQ_STRU *msg         = VOS_NULL_PTR;
    AT_MT_Info                 *atMtInfoCtx = VOS_NULL_PTR;
    VOS_UINT32                  length;

    atMtInfoCtx = AT_GetMtInfoCtx();

    /* ����BBIC_CAL_TEMP_QRY_REQ_STRU��Ϣ */
    length = sizeof(BBIC_CAL_TEMP_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    msg    = (BBIC_CAL_TEMP_QRY_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, length);

    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SndBbicCalQryFtemprptReq: alloc msg fail!");
        return AT_FAILURE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(msg);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(msg, DSP_PID_BBA_CAL, ID_AT_BBIC_TEMP_QRY_REQ);

    msg->stPara.enChannelType = atMtInfoCtx->bbicInfo.currentChannelType;
    msg->stPara.hwChannelNum  = channelNum;

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_ERR_LOG("AT_SndBbicCalQryFtemprptReq: Send msg fail!");
        return AT_FAILURE;
    }

    AT_PR_LOGH("AT_SndBbicCalQryFtemprptReq Exit");

    return AT_SUCCESS;
}

VOS_UINT32 At_CovertChannelTypeToBbicCal(AT_TEMP_ChannelTypeUint16           channelType,
                                         BBIC_TEMP_CHANNEL_TYPE_ENUM_UINT16 *bbicChannelType)
{
    VOS_UINT32 result = VOS_TRUE;

    switch (channelType) {
        case AT_TEMP_CHANNEL_TYPE_LOGIC:
            *bbicChannelType = BBIC_TEMP_CHANNEL_TYPE_LOGIC;
            break;

        case AT_TEMP_CHANNEL_TYPE_PHY:
            *bbicChannelType = BBIC_TEMP_CHANNEL_TYPE_PHY;
            break;

        default:
            *bbicChannelType = BBIC_TEMP_CHANNEL_TYPE_BUTT;
            result           = VOS_FALSE;
            break;
    }

    return result;
}

VOS_UINT32 AT_QryFtemprptPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
    VOS_UINT32  rst;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("AT_QryFtemprptPara Enter");

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != AT_FTEMPRPT_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��¼ͨ��������Ϣ */
    if (At_CovertChannelTypeToBbicCal((AT_TEMP_ChannelTypeUint16)g_atParaList[0].paraValue,
                                      &atMtInfoCtx->bbicInfo.currentChannelType) == VOS_FALSE) {
        return AT_ERROR;
    }

    /* AT����Ftemprpt��ѯ��Ϣ��BBIC���� */
    rst = AT_SndBbicCalQryFtemprptReq((INT16)g_atParaList[1].paraValue);

    if (rst == AT_SUCCESS) {
        atMtInfoCtx->atInfo.indexNum          = indexNum;
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FTEMPRPT_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_QryFRxonPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
    VOS_UINT16  length;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("At_QryFRxonPara Enter");

    /* ��ǰ��Ϊ������ģʽ */
    if (atMtInfoCtx->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ѯ��ǰ���ջ�����״̬ */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
        atMtInfoCtx->atInfo.rxOnOff);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_QryFpowdetTPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;

    atMtInfoCtx = AT_GetMtInfoCtx();

    AT_PR_LOGH("AT_QryFpowdetTPara Enter");

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ǰ��Ϊ������ģʽ */
    if (atMtInfoCtx->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* ��ǰ�Ƿ������������ý��뼼�� */
    if (atMtInfoCtx->bbicInfo.currtRatMode >= RAT_MODE_BUTT) {
        return AT_ERROR;
    }

    /* ��BBIC����Ϣ */
    if ((atMtInfoCtx->bbicInfo.currtRatMode == RAT_MODE_GSM) &&
        (atMtInfoCtx->atInfo.faveType != AT_FWAVE_TYPE_CONTINUE)) {
        if (At_SndGsmTxOnOffReq_ModulatedWave(VOS_TRUE) != VOS_TRUE) {
            return AT_SND_MSG_FAIL;
        }
    } else {
        if (At_SndTxOnOffReq(VOS_TRUE) != VOS_TRUE) {
            return AT_SND_MSG_FAIL;
        }
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FPOWDET_QRY;
    atMtInfoCtx->atInfo.indexNum          = indexNum;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryFPllStatusPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
    VOS_UINT32  result;

    atMtInfoCtx = AT_GetMtInfoCtx();
    result      = AT_FAILURE;

    AT_PR_LOGH("AT_QryFPllStatusPara Enter");

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (atMtInfoCtx->atInfo.dspLoadFlag != VOS_TRUE) {
        return AT_ERROR;
    }

    result = AT_SndBbicPllStatusReq();

    /* ����ʧ�� */
    if (result != AT_SUCCESS) {
        AT_ERR_LOG("AT_QryFPllStatusPara: AT Snd ReqMsg fail.");
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FPLLSTATUS_QRY;
    atMtInfoCtx->atInfo.indexNum          = indexNum;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SndBbicRssiReq(VOS_VOID)
{
    AT_BBIC_CAL_MT_RX_RSSI_REQ *msg = VOS_NULL_PTR;
    VOS_UINT32                  length;
#if (FEATURE_LTEV == FEATURE_ON)
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
#endif

    /* ����BBIC_CAL_RF_DEBUG_RSSI_REQ_STRU��Ϣ */
    length = sizeof(AT_BBIC_CAL_MT_RX_RSSI_REQ) - VOS_MSG_HEAD_LENGTH;
    msg    = (AT_BBIC_CAL_MT_RX_RSSI_REQ *)PS_ALLOC_MSG(WUEPS_PID_AT, length);

    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SndBbicRssiReq: alloc msg fail!");
        return AT_FRSSI_OTHER_ERR;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(msg);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(msg, DSP_PID_BBA_CAL, ID_AT_BBIC_CAL_MT_RX_RSSI_REQ);
#if (FEATURE_LTEV == FEATURE_ON)
    atMtInfoCtx = AT_GetMtInfoCtx();
    if (atMtInfoCtx->atInfo.ratMode == AT_RAT_MODE_LTEV) {
        msg->stComponentID.uwComponentType = AT_COMPONENTTYPE_RAT_LTEV;
    }
#endif
    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_ERR_LOG("AT_SndBbicRssiReq: Send msg fail!");
        return AT_FRSSI_OTHER_ERR;
    }

    AT_PR_LOGH("AT_SndBbicRssiReq Exit");

    return AT_SUCCESS;
}

VOS_UINT32 At_QryFrssiPara(VOS_UINT8 indexNum)
{
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;
    VOS_UINT32  result;

    atMtInfoCtx = AT_GetMtInfoCtx();
    result      = AT_FAILURE;

    AT_PR_LOGH("At_QryFrssiPara Enter");

    /* ���������ڷ�����ģʽ��ʹ�� */
    if (atMtInfoCtx->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }

    /* �������������÷������ŵ���ʹ�� */
    if (atMtInfoCtx->atInfo.dspLoadFlag == VOS_FALSE) {
        return AT_CHANNEL_NOT_SET;
    }

    /* ��������Ҫ�ڴ򿪽��ջ���ʹ�� */
    if (atMtInfoCtx->atInfo.rxOnOff != AT_DSP_RF_SWITCH_ON) {
        return AT_FRSSI_RX_NOT_OPEN;
    }

    result = AT_SndBbicRssiReq();

    /* ����ʧ�� */
    if (result != AT_SUCCESS) {
        AT_ERR_LOG("At_QryFrssiPara: AT Snd ReqMsg fail.");
        return AT_FRSSI_OTHER_ERR;
    }

    atMtInfoCtx->atInfo.indexNum          = indexNum;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_QUERY_RSSI;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_QryLcacellPara(VOS_UINT8 indexNum)
{
    VOS_UINT32                ret;
    L4A_READ_LCACELL_REQ_STRU lcacell = {0};

    lcacell.ctrl.clientId = g_atClientTab[indexNum].clientId;

    lcacell.ctrl.opId = 0;
    lcacell.ctrl.pid  = WUEPS_PID_AT;

    ret = atSendL4aDataMsg(g_atClientTab[indexNum].clientId, I0_MSP_L4_L4A_PID, ID_MSG_L4A_LCACELLQRY_REQ,
                           (VOS_UINT8 *)(&lcacell), sizeof(L4A_READ_LCACELL_REQ_STRU));

    if (ret == ERR_MSP_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LCACELL_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    }
    return AT_ERROR;
}

VOS_UINT32 AT_TestFPllStatusPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: (0,1),(0,1)", g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}

