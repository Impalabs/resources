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
#include "at_custom_mm_qry_cmd_proc.h"
#include "securec.h"

#include "at_snd_msg.h"
#include "at_cmd_proc.h"

#include "taf_app_mma.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_MM_QRY_CMD_PROC_C

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_QryRcsSwitch(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* AT ��MMA ���Ͳ�ѯ������Ϣ */
    rst = TAF_MMA_QryRcsSwitchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0);
    if (rst == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_RCSSWITCH_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#endif

VOS_UINT32 At_QryHplmnPara(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryApHplmnInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EHPLMN_LIST_QRY;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 AT_QryDplmnListPara(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryDplmnListReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DPLMNLIST_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_QryBorderInfoPara(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryBorderInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_BORDERINFO_QRY;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 AT_QryFratIgnitionInfo(VOS_UINT8 indexNum)
{
    VOS_UINT32 ret;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    ret = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_FRAT_IGNITION_QRY_REQ, VOS_NULL_PTR, 0,
                                 I0_UEPS_PID_MTA);
    if (ret != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryFratIgnitionInfo: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FRATIGNITION_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryEmRssiRptPara(VOS_UINT8 indexNum)
{
    /* ִ��������� */
    if (TAF_MMA_QryEmRssiRptReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId) ==
        VOS_TRUE) {
        /* ������������״̬ */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EMRSSIRPT_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_QryPsScenePara(TAF_UINT8 indexNum)
{
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    VOS_UINT32    rslt;
    ModemIdUint16 modemId;

    modemId = MODEM_ID_BUTT;

    /* ͨ��clientID���ModemID */
    rslt = AT_GetModemIdFromClient(g_atClientTab[indexNum].clientId, &modemId);

    /* ���ModemID��ȡʧ�ܻ���Modem1������ʧ�� */
    if ((rslt != VOS_OK) || (modemId != MODEM_ID_1)) {
        AT_WARN_LOG("AT_QryPsScenePara: AT_GetModemIdFromClient failed or not modem1!");
        return AT_ERROR;
    }

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (At_CheckCurrRatModeIsCL((VOS_UINT8)(g_atClientTab[indexNum].clientId)) == VOS_TRUE) {
        AT_WARN_LOG("AT_QryPsScenePara: operation not allowed in CL mode!");
        return AT_CME_OPERATION_NOT_ALLOWED_IN_CL_MODE;
    }
#endif

    if (TAF_MMA_QryPsSceneReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PSSCENE_QRY;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
#else
    return AT_ERROR;
#endif
}

VOS_UINT32 AT_QryCarOosGearsPara(TAF_UINT8 indexNum)
{
    if (TAF_MMA_QryOosGearsReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CAROOSGEARS_QRY;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_QryAutoAttach(TAF_UINT8 indexNum)
{
    if (TAF_MMA_QryAutoAttachInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CAATT_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_QryImsSwitchPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* AT ��MMA ���Ͳ�ѯ������Ϣ */
    rst = TAF_MMA_QryImsSwitchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, TAF_MMA_IMS_SWITCH_TYPE_GUL);
    if (rst == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_GUL_IMS_SWITCH_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_UINT32 At_QryAppWronReg(VOS_UINT8 indexNum)
{
    /* ������Ϣ��MMA����ѯע��ʱ�� */
    if (TAF_MMA_QryApPwrOnAndRegTimeReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_REGISTER_TIME_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_QryApHplmn(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryApHplmnInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_HOMEPLMN_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_CSG == FEATURE_ON)
VOS_UINT32 At_QryCampCsgIdInfo(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryCampCsgIdInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSG_ID_INFO_QRY;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 AT_TestCsgIdSearchPara(VOS_UINT8 indexNum)
{
    /* �˼�ͨ����󻺴�4K,��Ҫ�ֶβ�ѯ�ֶ��ϱ���� */
    TAF_MMA_PlmnListPara plmnListPara;

    plmnListPara.qryNum    = TAF_MMA_MAX_CSG_ID_LIST_NUM;
    plmnListPara.currIndex = 0;

    if (TAF_MMA_CsgListSearchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &plmnListPara) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSG_LIST_SEARCH;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 AT_AbortCsgIdSearchPara(VOS_UINT8 indexNum)
{
    /* ��ǰֻ�ܴ����б��ѵĴ��, ���������򲻽��д�ϲ��� */
    if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CSG_LIST_SEARCH) {
        /* AT��MMA���ʹ���б��ѵ����� */
        if (TAF_MMA_AbortCsgListSearchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
            /* ֹͣcsg�б���AT�ı�����ʱ�� */
            AT_StopRelTimer(indexNum, &g_atClientTab[indexNum].hTimer);
            /* ���µ�ǰ�������� */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_ABORT_CSG_LIST_SEARCH;
            return AT_WAIT_ASYNC_RETURN;
        }
    }
    return AT_FAILURE;
}
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
VOS_UINT32 AT_QryCLteRoamAllowPara(VOS_UINT8 indexNum)
{
    NAS_MMC_NvimLteInternationalRoamCfg nvimLteRoamAllowedFlg;
    VOS_UINT8                           lteRoamAllow = VOS_FALSE;

    /* �ֲ�������ʼ�� */
    nvimLteRoamAllowedFlg.lteRoamAllowedFlg = VOS_FALSE;

    /* ��ȡNV, ��NV�ĽṹΪ NAS_MMC_NvimLteInternationalRoamCfg, ֻ��ȡ��1���ֽ� */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_LTE_INTERNATION_ROAM_CONFIG, &nvimLteRoamAllowedFlg,
                          sizeof(NAS_MMC_NvimLteInternationalRoamCfg)) != NV_OK) {
        AT_WARN_LOG("AT_QryCLteRoamAllowPara(): NV_ITEM_LTE_INTERNATION_ROAM_CONFIG TAF_ACORE_NV_READ Error");
        return AT_ERROR;
    }

    /* �ݴ���, NV��ֵΪVOS_FALSEʱ������������, Ϊ����ֵʱ��Ϊ�������� */
    if (nvimLteRoamAllowedFlg.lteRoamAllowedFlg == VOS_FALSE) {
        lteRoamAllow = VOS_FALSE;
    } else {
        lteRoamAllow = VOS_TRUE;
    }

    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
        (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", g_parseContext[indexNum].cmdElement->cmdName, lteRoamAllow);
    return AT_OK;
}
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
VOS_UINT32 AT_QryNrImsSwitchPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* AT ��MMA ���Ͳ�ѯ������Ϣ */
    rst = TAF_MMA_QryImsSwitchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, TAF_MMA_IMS_SWITCH_TYPE_NR);
    if (rst == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NR_IMS_SWITCH_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_UINT32 AT_QryUlFreqPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rslt;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* ��MMA������Ϣ��ѯ����С������Ƶ�� */
    rslt = TAF_MMA_QryUlFreqRptReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId);

    if (rslt == VOS_TRUE) {
        /* ������������״̬ */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_ULFREQRPT_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
 * ����˵��: AT^EONS�����������
 * �������: index: AT�·�ͨ��
 * ���ؽ��: AT_OK: ִ�гɹ�
 */
VOS_UINT32 AT_TestEonsPara(VOS_UINT8 indexId)
{
    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR*)g_atSndCodeAddress,
        (TAF_CHAR*)g_atSndCodeAddress, "%s: (1-5)", g_parseContext[indexId].cmdElement->cmdName);
    return AT_OK;
}

VOS_UINT32 AT_QryCellRoamPara(VOS_UINT8 indexId)
{
    VOS_UINT32 result;

    /* ����CELLROAM��ѯ��Ϣ��MMA */
    result = Taf_ParaQuery(g_atClientTab[indexId].clientId, 0, TAF_PH_CELLROAM_PARA, TAF_NULL_PTR);
    if (result == AT_SUCCESS) {
        g_atClientTab[indexId].cmdCurrentOpt = AT_CMD_CELLROAM_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

/*
 * ����˵��: AT����HCSQ��ѯ����C��
 * �������: moduleId: AT Module ID
 *           clientId: AT Client ID
 *           opId: ����ID
 * ���ؽ��: VOS_TRUE: ���ͳɹ�
 *           VOS_FALSE: ����ʧ��
 */
VOS_UINT32 AT_SendHcsqQryReq(VOS_UINT32 moduleId, VOS_UINT16 clientId, VOS_UINT8 opId)
{
    TafMmaHcsqQryRequest *reqMsg = (TafMmaHcsqQryRequest *)TAF_AllocMsgWithHeaderLen(WUEPS_PID_TAF,
        sizeof(TafMmaHcsqQryRequest));

    if (reqMsg == VOS_NULL_PTR) {
        return VOS_FALSE;
    }

    /* ��������������TafMmaNetSelOptSetReq */
    TAF_CfgMsgHdr((MsgBlock *)reqMsg, WUEPS_PID_TAF, WUEPS_PID_MMA, sizeof(TafMmaHcsqQryRequest) - VOS_MSG_HEAD_LENGTH);
    reqMsg->msgName = ID_TAF_MMA_HCSQ_QRY_REQ;
    reqMsg->ctrlInfo.moduleId = moduleId;
    reqMsg->ctrlInfo.clientId = clientId;
    reqMsg->ctrlInfo.opId = opId;

    if (TAF_TraceAndSendMsg(WUEPS_PID_TAF, reqMsg) != VOS_OK) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*
 * ����˵��: AT^HCSQ�����������
 * �������: indexNum: AT�·�ͨ��
 * ���ؽ��: AT_WAIT_ASYNC_RETURN: �첽�ȴ����
 *           AT_ERROR: ����ʧ��
 */
VOS_UINT32 AT_QryHcsqPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 result;

    /* ����HCSQ��ѯ����C�� */
    result = AT_SendHcsqQryReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId);
    if (result == VOS_TRUE)  {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_HCSQ_QRY;
        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

/*
 * ����˵��: AT^HCSQ�����������
 * �������: index: AT�·�ͨ��
 * ���ؽ��: AT_OK: ִ�гɹ�
 */
VOS_UINT32 AT_TestHcsqPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN,(TAF_CHAR*)g_atSndCodeAddress,
        (TAF_CHAR*)g_atSndCodeAddress, "%s: \"NOSERVICE\",\"GSM\",\"WCDMA\",\"LTE\"",
        g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}
#endif

