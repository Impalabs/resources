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
#include "at_phone_mm_qry_cmd_proc.h"
#include "securec.h"
#include "at_ctx.h"
#include "at_cmd_proc.h"
#include "taf_ccm_api.h"
#include "at_data_proc.h"
#include "at_msg_print.h"
#include "at_phone_comm.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_MM_QRY_CMD_PROC_C

TAF_UINT32 At_QryCfunPara(TAF_UINT8 indexNum)
{
    if (TAF_MMA_QryPhoneModeReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CFUN_READ;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

TAF_UINT32 At_QryCopsPara(TAF_UINT8 indexNum)
{
    /* �ſ�CLģʽ��AT+COPS��ѯ���� */

    if (TAF_MMA_QryCopsInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_AbortCopsPara(VOS_UINT8 indexNum)
{
    /* ��ǰֻ�ܴ����б��ѵĴ��, ���������򲻽��д�ϲ��� */
    if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_COPS_TEST) {
        /* AT��MMA���ʹ���б��ѵ����� */
        if (TAF_MMA_AbortPlmnListReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
            /* ֹͣ�б���AT�ı�����ʱ�� */
            AT_StopRelTimer(indexNum, &g_atClientTab[indexNum].hTimer);

            /* ���µ�ǰ�������� */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_ABORT_PLMN_LIST;

            return AT_WAIT_ASYNC_RETURN;
        } else {
            return AT_FAILURE;
        }
    } else {
        return AT_FAILURE;
    }
}

VOS_UINT32 At_TestCopsPara(TAF_UINT8 indexNum)
{
    TAF_MMA_PlmnListPara plmnListPara;

    plmnListPara.qryNum    = TAF_MMA_MAX_PLMN_NAME_LIST_NUM;
    plmnListPara.currIndex = 0;

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (At_CheckCurrRatModeIsCL((VOS_UINT8)(g_atClientTab[indexNum].clientId)) == VOS_TRUE) {
        return AT_CME_OPERATION_NOT_ALLOWED_IN_CL_MODE;
    }
#endif

    if (Taf_PhonePlmnList(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &plmnListPara) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_TEST;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_QryCgattPara(VOS_UINT8 indexNum)
{
    /* AT��MMAģ�鷢��Ϣ��Ҫ��MMA����CS��PS��ע��״̬ */
    if (TAF_MMA_AttachStatusQryReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                   TAF_MMA_SERVICE_DOMAIN_PS) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MMA_GET_CURRENT_ATTACH_STATUS;
        return AT_WAIT_ASYNC_RETURN;
    }
    return AT_ERROR;
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
TAF_UINT32 At_QrySysCfgPara(TAF_UINT8 indexNum)
{
    if (TAF_MMA_QrySyscfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSCFG_READ; /* ���õ�ǰ����ģʽ */
        return AT_WAIT_ASYNC_RETURN;                                /* �ȴ��첽�¼����� */
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_UINT32 AT_QrySysCfgExPara(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QrySyscfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSCFGEX_READ; /* ���õ�ǰ����ģʽ */
        return AT_WAIT_ASYNC_RETURN;                                  /* �ȴ��첽�¼����� */
    } else {
        return AT_ERROR;
    }
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))

VOS_UINT32 AT_TestSyscfg(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    rst = TAF_MMA_TestSysCfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId);

    if (rst == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSCFG_TEST;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_OK;
}
#endif

VOS_UINT32 AT_TestSyscfgEx(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    rst = TAF_MMA_TestSysCfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId);

    if (rst == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSCFGEX_TEST;

        return AT_WAIT_ASYNC_RETURN;
    }
    return AT_OK;
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))

TAF_UINT32 At_QryCpamPara(TAF_UINT8 indexNum)
{
    if (TAF_MMA_QryAccessModeReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPAM_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_UINT32 At_QryCgcattPara(VOS_UINT8 indexNum)
{
    /* AT��MMAģ�鷢��Ϣ��Ҫ��MMA����CS��PS��ע��״̬ */
    if (TAF_MMA_AttachStatusQryReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                   TAF_MMA_SERVICE_DOMAIN_CS_PS) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MMA_GET_CURRENT_ATTACH_STATUS;
        return AT_WAIT_ASYNC_RETURN;
    }
    return AT_ERROR;
}

TAF_UINT32 At_QryFPlmnPara(TAF_UINT8 indexNum)
{
    if (TAF_MMA_QryFPlmnInfo(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CFPLMN_READ; /* ���õ�ǰ����ģʽ */
        return AT_WAIT_ASYNC_RETURN;                                /* �ȴ��첽�¼����� */
    } else {
        return AT_ERROR; /* ���󷵻� */
    }
}

VOS_UINT32 At_QryCplsPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            netCtx->prefPlmnType);

    return AT_OK;
}

TAF_UINT32 At_QryCpolPara(TAF_UINT8 indexNum)
{
    VOS_UINT32      ret;
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;
    TAF_MMA_CpolInfoQueryReq cpolInfo;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /*
     * AT��MMA������Ӫ����Ϣ:
     * ��Ϊ�˼���Ϣ���ƣ�����һ�λ�ȡ������Ӫ����Ϣ�����ﶨ��Ϊһ�λ�ȡ37����Ӫ����Ϣ
     * ��һ��������Ϣ��������0��ʼҪ��������37����Ӫ����Ϣ
     */
    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    (VOS_VOID)memset_s(&cpolInfo, sizeof(cpolInfo), 0x00, sizeof(cpolInfo));

    cpolInfo.prefPLMNType = netCtx->prefPlmnType;
    cpolInfo.fromIndex    = 0;

    cpolInfo.plmnNum = TAF_MMA_MAX_PLMN_NAME_LIST_NUM;

    /* ��MMA����Ϣ������Ӫ����Ϣ */
    ret = TAF_MMA_QueryCpolReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &cpolInfo);
    if (ret != VOS_TRUE) {
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPOL_READ;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 At_TestCpolPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_TEST_CMD) {
        return AT_ERROR;
    }

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    rst = TAF_MMA_TestCpolReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, netCtx->prefPlmnType);

    if (rst == VOS_TRUE) {
        /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPOL_TEST;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 At_QryQuickStart(TAF_UINT8 indexNum)
{
    AT_PR_LOGI("Rcv Msg");

    if (TAF_MMA_QryQuickStartReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ����ģʽ */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CQST_READ;
        /* �ȴ��첽�¼����� */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        /* ���󷵻� */
        return AT_ERROR;
    }
}

VOS_UINT32 At_QryPlmnPara(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryPlmnReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PLMN_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_QryAcInfoPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 result;

    /* ����Ϣ��MMA��ȡ��ǰUE��λ����Ϣ */
    result = TAF_MMA_QryAcInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0);
    /* ִ��������� */
    if (result == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_ACINFO_READ;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_QryMmPlmnInfoPara(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryMMPlmnInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MMPLMNINFO_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_QryRejInfoPara(VOS_UINT8 indexNum)
{
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    if (TAF_MMA_QryRejinfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_REJINFO_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_TestNetScanPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress,

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        "%s: (1-20),(-125--47),(0-4)",
#else
        "%s: (1-20),(-125--47),(0-2)",
#endif
        g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}

VOS_UINT32 AT_AbortNetScanPara(VOS_UINT8 indexNum)
{
    /* ��ǰֻ�ܴ���NETSCAN���õĴ��, ���������򲻽��д�ϲ��� */
    if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_NETSCAN_SET) {
        /* AT��MMA���ʹ��NETSCAN������ */
        if (TAF_MMA_NetScanAbortReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
            /* ֹͣNETSCANAT�ı�����ʱ�� */
            AT_StopRelTimer(indexNum, &g_atClientTab[indexNum].hTimer);

            /* ���µ�ǰ�������� */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NETSCAN_ABORT;

            return AT_WAIT_ASYNC_RETURN;
        }

        /* AT��MMA���ʹ��NETSCAN������ʧ�ܣ�����ʧ�� */
        return AT_FAILURE;
    }

    /* ��ǰ�������������ʧ�� */
    return AT_FAILURE;
}

VOS_UINT32 AT_QryEmRssiCfgPara(VOS_UINT8 indexNum)
{
    /* ִ��������� */
    if (TAF_MMA_QryEmRssiCfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId) ==
        VOS_TRUE) {
        /* ������������״̬ */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EMRSSICFG_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_TestCesqPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length = 0;

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0-63,99),(0-7,99),(0-96,255),(0-49,255),(0-34,255),(0-97,255)",
        g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 At_QryEonsUcs2Para(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryEonsUcs2Req(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EONSUCS2_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_QryCevdpPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* AT ��MMA ���Ͳ�ѯ������Ϣ */
    rst = TAF_MMA_QryVoiceDomainReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0);
    if (rst == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_VOICE_DOMAIN_QRY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_UINT32 AT_QryCindPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_READ_CMD) {
        return AT_ERROR;
    }

    /* +CIND: <ind> Ŀǰֻ֧��signal�Ĳ�ѯ */
    g_atSendDataBuff.bufLen =
        (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "%s: %d", g_parseContext[indexNum].cmdElement->cmdName,
            netCtx->cerssiReportType);

    return AT_OK;
}

VOS_UINT32 AT_TestCindPara(VOS_UINT8 indexNum)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_TEST_CMD) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: \"signal\",(0-5)", g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}

/*
 * Description: ^CRPN?
 * History:
 *  1.Date: 2009-09-01
 *    Modification: Created function
 */
TAF_UINT32 At_QryCrpnPara(TAF_UINT8 indexNum)
{
    return AT_OK;
}

VOS_UINT32 AT_QryUserSrvStatePara(VOS_UINT8 indexNum)
{
    if (TAF_MMA_QryUserSrvStateReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_USER_SRV_STATE_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

