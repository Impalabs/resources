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
#include "at_phone_taf_set_cmd_proc.h"
#include "securec.h"
#include "taf_app_mma.h"
#include "at_ctx.h"
#include "at_cmd_proc.h"
#include "taf_ccm_api.h"
#include "at_data_proc.h"
#include "at_msg_print.h"
#include "at_phone_comm.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_TAF_SET_CMD_PROC_C

#define AT_PH_MS_A_MODE 0
#define AT_PH_MS_B_MODE 1
#define AT_PH_MS_CG_MODE 2
#define AT_TIME_PARA_NUM 1
#define AT_TIME_RPT_FLG 0
#define AT_TIME_RPT_FLG_MAX_VALID_VALUE 2
#define AT_CTZR_VALUE_MAX_VALUE 2
#define AT_NVM_MIN_PARA_NUM 6
#define AT_NVM_MAX_PARA_NUM 7
#define AT_NVM_INDEX 0
#define AT_NVM_TOTAL 1
#define AT_NVM_ECC_NUM 2
#define AT_NVM_CATEGORY 3
#define AT_NVM_SIMPRESENT 4
#define AT_NVM_MCC 5
#define AT_NVM_ABNORMAL_SERVICE 6
#define AT_CSDF_MAX_PARA_NUM 2
#define AT_MODEM_DEFALUT_AUX_MODE_YEAR_LEN 2      /* "yy"�ܳ��� */
#define AT_MODEM_OTHER_AUX_MODE_YEAR_LEN 4        /* "yyyy"�ܳ��� */
#define AT_MODEM_AUX_MODE_EXCEPT_YEAR_TIME_LEN 18 /* "/mm/dd,hh:mm:ss+/-zz"�ܳ��� */
#define AT_MODEM_DEFALUT_AUX_MODE_YEAR_MAX 99 /* "yy"�����������ֵ */
#define AT_MODEM_DEFALUT_AUX_MODE_YEAR_MIN 0  /* "yy"����������Сֵ */
#define AT_MODEM_TIME_ZONE_MAX 48    /* ������ʱ�����ֵ */
#define AT_MODEM_TIME_ZONE_MIN (-48) /* ������ʱ�����ֵ */
#define AT_CCLK_TIME 0
#define AT_JDCFG_GSM_RSSI_THRESH 1
#define AT_JDCFG_GSM_RSSI_NUM 2
#define AT_JDCFG_WCDMA_RSSI_THRESH 1
#define AT_JDCFG_WCDMA_RSSI_PERCENT 2
#define AT_JDCFG_WCDMA_PSCH_THRESH 3
#define AT_JDCFG_WCDMA_PSCH_PERCENT 4
#define AT_JDCFG_LTE_RSSI_THRESH 1
#define AT_JDCFG_LTE_RSSI_PERCENT 2
#define AT_JDCFG_LTE_PSSRATIO_THRESH 3
#define AT_JDCFG_LTE_PSSRATIO_PERCENT 4
#define AT_JDCFG_RSSISRHFREQ_PERCENT 2
#define AT_JDCFG_PSCHSRHTHRE_SHOID 3
#define AT_JDCFG_PSCHSRHFREQ_PERCENT 4
#define AT_JAM_DETECT_GSM_THRESHOLD_MAX 70
#define AT_JAM_DETECT_GSM_FREQNUM_MAX 255
#define AT_JAM_DETECT_WCDMA_RSSI_THRESHOLD_MAX 70
#define AT_JAM_DETECT_WCDMA_RSSI_PERCENT_MAX 100
#define AT_JAM_DETECT_WCDMA_PSCH_THRESHOLD_MAX 65535
#define AT_JAM_DETECT_WCDMA_PSCH_PERCENT_MAX 100
#define AT_JAM_DETECT_LTE_RSSI_PERCENT_MAX 100
#define AT_JAM_DETECT_LTE_PSSRATIO_THRESHOLD_MAX 1000
#define AT_JAM_DETECT_LTE_PSSRATIO_PERCENT_MAX 100
#define AT_JAM_DETECT_GSM_PARA_NUM 3
#define AT_JAM_DETECT_WL_PARA_NUM 5

TAF_UINT32 At_SetCgclassPara(TAF_UINT8 indexNum)
{
    TAF_PH_MS_CLASS_TYPE msClass;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<class> */
    switch (g_atParaList[0].paraValue) {
        case AT_PH_MS_A_MODE:
            msClass = TAF_PH_MS_CLASS_A;
            break;

        case AT_PH_MS_B_MODE:
            msClass = TAF_PH_MS_CLASS_B;
            break;

        case AT_PH_MS_CG_MODE:
            msClass = TAF_PH_MS_CLASS_CG;
            break;

        default:
            msClass = TAF_PH_MS_CLASS_CC;
            break;
    }

    if (msClass == TAF_PH_MS_CLASS_A) {
        return AT_OK;
    } else {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetCregPara(TAF_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϊ�� */
    if (g_atParaList[0].paraLen != 0) {
        netCtx->cregType   = (AT_CREG_TYPE)g_atParaList[0].paraValue;
        atCmd.u.cregRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;

        /*
         * ����Ĭ��TAF_EVENT_INFO_CELL_SIGN+TAF_EVENT_INFO_CELL_ID + TAF_EVENT_INFO_CELL_BLER
         * ���������ϱ����˴�������֪ͨ����㿪�������ϱ���Ŀǰû��ֹͣ��ֹͣ�˻�Ӱ��CSQ��ѯ
         */
    } else {
        netCtx->cregType   = AT_CREG_RESULT_CODE_NOT_REPORT_TYPE;
        atCmd.u.cregRptFlg = AT_CREG_RESULT_CODE_NOT_REPORT_TYPE;
    }

    atCmd.reqType = AT_MTA_SET_CREG_RPT_TYPE;

    /* ��MTA����+creg�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 At_SetCgregPara(TAF_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϊ�� */
    if (g_atParaList[0].paraLen != 0) {
        netCtx->cgregType   = (AT_CGREG_TYPE)g_atParaList[0].paraValue;
        atCmd.u.cgregRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;

        /*
         * ����Ĭ��TAF_EVENT_INFO_CELL_SIGN+TAF_EVENT_INFO_CELL_ID + TAF_EVENT_INFO_CELL_BLER
         * ���������ϱ����˴�������֪ͨ����㿪�������ϱ���Ŀǰû��ֹͣ��ֹͣ�˻�Ӱ��CSQ��ѯ
         */
    } else {
        netCtx->cgregType   = AT_CGREG_RESULT_CODE_NOT_REPORT_TYPE;
        atCmd.u.cgregRptFlg = AT_CGREG_RESULT_CODE_NOT_REPORT_TYPE;
    }

    atCmd.reqType = AT_MTA_SET_CGREG_RPT_TYPE;

    /* ��MTA����+cgreg�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 At_SetRssiPara(TAF_UINT8 indexNum)
{
    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));
    atCmd.reqType      = AT_MTA_SET_RSSI_RPT_TYPE;
    atCmd.u.rssiRptFlg = AT_RSSI_RESULT_CODE_NOT_REPORT_TYPE;

    if (g_atParaList[0].paraLen != 0) {
        atCmd.u.rssiRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;
    }

    /* ��MTA����^rssi�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    /* AT����Ҫ����L4A����MMC����LMM,����LMM֪ͨLRRC */

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 At_SetTimePara(TAF_UINT8 indexNum)
{
    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex > AT_TIME_PARA_NUM) {
        return AT_TOO_MANY_PARA;
    }

    if ((g_atParaList[AT_TIME_RPT_FLG].paraLen == 0) ||
        (g_atParaList[AT_TIME_RPT_FLG].paraValue > AT_TIME_RPT_FLG_MAX_VALID_VALUE)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));
    atCmd.reqType      = AT_MTA_SET_TIME_RPT_TYPE;
    atCmd.u.timeRptFlg = (VOS_UINT8)g_atParaList[AT_TIME_RPT_FLG].paraValue;

    /* ��MTA����^time�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 At_SetCtzrPara(TAF_UINT8 indexNum)
{
    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex > 1) {
        return AT_TOO_MANY_PARA;
    }

    /* ������Χ��չ */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[0].paraValue > AT_CTZR_VALUE_MAX_VALUE)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));
    atCmd.reqType      = AT_MTA_SET_CTZR_RPT_TYPE;
    atCmd.u.ctzrRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ��MTA����^ctzr�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetNvmEccNumPara(VOS_UINT8 indexNum)
{
    VOS_UINT32                 rst;
    VOS_UINT32                 temp;
    VOS_UINT32                 mcc;
    TAF_Ctrl                   ctrl;
    MN_CALL_AppCustomEccNumReq eccNumReq;
    ModemIdUint16              modemId;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));
    (VOS_VOID)memset_s(&eccNumReq, sizeof(eccNumReq), 0x00, sizeof(eccNumReq));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_ERR_LOG("AT_SetNvmEccNumSetPara: Cmd Opt Type is wrong.");
        return AT_ERROR;
    }

    /* �������� */
    if ((g_atParaIndex < AT_NVM_MIN_PARA_NUM) || (g_atParaIndex > AT_NVM_MAX_PARA_NUM)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �����Ϸ��Լ��, index > total��Ϊ������Ч */
    if (g_atParaList[AT_NVM_INDEX].paraValue > g_atParaList[AT_NVM_TOTAL].paraValue) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���<ecc_num>������ֱ�ӷ��ش��� */
    if (g_atParaList[AT_NVM_ECC_NUM].paraLen > (MN_CALL_MAX_BCD_NUM_LEN * 2)) {
        AT_ERR_LOG("AT_SetNvmEccNumSetPara: ecc_num too long.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ECC NUMת��ΪBCD�� */
    if (AT_AsciiNumberToBcd((TAF_CHAR *)&g_atParaList[AT_NVM_ECC_NUM].para[0], eccNumReq.eccNum.bcdNum,
                            &eccNumReq.eccNum.numLen) != MN_ERR_NO_ERROR) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    temp = 0;
    /* ������������λ�� */
    temp |= (g_atParaList[AT_NVM_MCC].paraValue % 10);
    /* ������������ʮλ��������4λ */
    temp |= (((g_atParaList[AT_NVM_MCC].paraValue / 10) % 10) << 4);
    /* �����������İ�λ��������8λ */
    temp |= (((g_atParaList[AT_NVM_MCC].paraValue / 100) % 10) << 8);

    /* ��MCCת��ΪNAS�ṹ */
    AT_ConvertMccToNasType(temp, &mcc);

    eccNumReq.index      = (VOS_UINT8)g_atParaList[AT_NVM_INDEX].paraValue;
    eccNumReq.total      = (VOS_UINT8)g_atParaList[AT_NVM_TOTAL].paraValue;
    eccNumReq.category   = (VOS_UINT8)g_atParaList[AT_NVM_CATEGORY].paraValue;
    eccNumReq.simPresent = (VOS_UINT8)g_atParaList[AT_NVM_SIMPRESENT].paraValue;
    eccNumReq.mcc        = mcc;

    if (g_atParaIndex == AT_NVM_MAX_PARA_NUM) {
        eccNumReq.abnormalServiceFlg = (VOS_UINT8)g_atParaList[AT_NVM_ABNORMAL_SERVICE].paraValue;
    } else {
        /* Ĭ���п�����������״̬ʱ�����������ʾĬ��ֵΪFALSE */
        eccNumReq.abnormalServiceFlg = VOS_FALSE;
    }

    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = g_atClientTab[indexNum].clientId;
    ctrl.opId     = g_atClientTab[indexNum].opId;

    if (AT_GetModemIdFromClient(g_atClientTab[indexNum].clientId, &modemId) != VOS_OK) {
        return AT_ERROR;
    }

    /* ������Ϣ */
    rst = TAF_CCM_CallCommonReq(&ctrl, &eccNumReq, ID_TAF_CCM_CUSTOM_ECC_NUM_REQ, sizeof(eccNumReq), modemId);

    if (rst != MN_ERR_NO_ERROR) {
        AT_WARN_LOG("AT_SetNvmEccNumPara: TAF_CCM_CallCommonReq fail.");
        return AT_ERROR;
    }

    /* ��ֹ�����·� �ȴ�CCM�ظ��󷵻�OK */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NVM_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_CheckJDCfgGsmPara(VOS_VOID)
{
    ModemIdUint16 modemId = MODEM_ID_0;

    /* �ж�ƽ̨�Ƿ�֧��GSM */
    if (AT_IsModemSupportRat(modemId, TAF_MMA_RAT_GSM) != VOS_TRUE) {
        AT_ERR_LOG("AT_CheckJDCfgGsmPara: Not Support GSM.");
        return VOS_ERR;
    }

    /* ���������ʽ:AT^JDCFG=0,<rssi_thresh>,<rssi_num> */
    if (g_atParaIndex != AT_JAM_DETECT_GSM_PARA_NUM) {
        return VOS_ERR;
    }

    /* ���ò�����Χ��� */
    if ((g_atParaList[AT_JDCFG_GSM_RSSI_THRESH].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_GSM_RSSI_THRESH].paraValue > AT_JAM_DETECT_GSM_THRESHOLD_MAX)) {
        return VOS_ERR;
    }

    if ((g_atParaList[AT_JDCFG_GSM_RSSI_NUM].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_GSM_RSSI_NUM].paraValue > AT_JAM_DETECT_GSM_FREQNUM_MAX)) {
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_CheckJDCfgWcdmaPara(VOS_VOID)
{
    ModemIdUint16 modemId = MODEM_ID_0;

    /* �ж�ƽ̨�Ƿ�֧��WCDMA */
    if (AT_IsModemSupportRat(modemId, TAF_MMA_RAT_WCDMA) != VOS_TRUE) {
        AT_ERR_LOG("AT_CheckJDCfgWcdmaPara: Not Support WCDMA.");
        return VOS_ERR;
    }

    /* ���������ʽ:AT^JDCFG=1,<rssi_thresh>,<rssi_percent>,<psch_thresh>,<psch_percent> */
    if (g_atParaIndex != AT_JAM_DETECT_WL_PARA_NUM) {
        return VOS_ERR;
    }

    /* ���ò�����Χ��� */
    /* �ڶ���������Ϊ�գ����򷵻�ERR */
    if ((g_atParaList[AT_JDCFG_WCDMA_RSSI_THRESH].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_WCDMA_RSSI_THRESH].paraValue > AT_JAM_DETECT_WCDMA_RSSI_THRESHOLD_MAX)) {
        return VOS_ERR;
    }
     /* ������������Ϊ�գ����򷵻�ERR */
    if ((g_atParaList[AT_JDCFG_WCDMA_RSSI_PERCENT].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_WCDMA_RSSI_PERCENT].paraValue > AT_JAM_DETECT_WCDMA_RSSI_PERCENT_MAX)) {
        return VOS_ERR;
    }
    /* ���ĸ�������Ϊ�գ����򷵻�ERR */
    if ((g_atParaList[AT_JDCFG_WCDMA_PSCH_THRESH].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_WCDMA_PSCH_THRESH].paraValue > AT_JAM_DETECT_WCDMA_PSCH_THRESHOLD_MAX)) {
        return VOS_ERR;
    }
     /* �����������Ϊ�գ����򷵻�ERR */
    if ((g_atParaList[AT_JDCFG_WCDMA_PSCH_PERCENT].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_WCDMA_PSCH_PERCENT].paraValue > AT_JAM_DETECT_WCDMA_PSCH_PERCENT_MAX)) {
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_CheckJDCfgLtePara(VOS_VOID)
{
    ModemIdUint16 modemId = MODEM_ID_0;

    /* �ж�ƽ̨�Ƿ�֧��LTE */
    if (AT_IsModemSupportRat(modemId, TAF_MMA_RAT_LTE) != VOS_TRUE) {
        AT_ERR_LOG("AT_CheckJDCfgLtePara: Not Support LTE.");
        return VOS_ERR;
    }

    /* ���������ʽ:AT^JDCFG=1,<rssi_thresh>,<rssi_percent>,<pssratio_thresh>,<pssratio_percent> */
    if (g_atParaIndex != AT_JAM_DETECT_WL_PARA_NUM) {
        return VOS_ERR;
    }

    /* ���ò�����Χ��� */
     /* �ڶ���������Ϊ�գ����򷵻�ERR */
    if ((g_atParaList[AT_JDCFG_LTE_RSSI_THRESH].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_LTE_RSSI_THRESH].paraValue > AT_JAM_DETECT_LTE_RSSI_THRESHOLD_MAX)) {
        return VOS_ERR;
    }
     /* ������������Ϊ�գ����򷵻�ERR */
    if ((g_atParaList[AT_JDCFG_LTE_RSSI_PERCENT].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_LTE_RSSI_PERCENT].paraValue > AT_JAM_DETECT_LTE_RSSI_PERCENT_MAX)) {
        return VOS_ERR;
    }

    if ((g_atParaList[AT_JDCFG_LTE_PSSRATIO_THRESH].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_LTE_PSSRATIO_THRESH].paraValue > AT_JAM_DETECT_LTE_PSSRATIO_THRESHOLD_MAX)) {
        return VOS_ERR;
    }

    if ((g_atParaList[AT_JDCFG_LTE_PSSRATIO_PERCENT].paraLen == 0) ||
        (g_atParaList[AT_JDCFG_LTE_PSSRATIO_PERCENT].paraValue > AT_JAM_DETECT_LTE_PSSRATIO_PERCENT_MAX)) {
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_CheckJDCfgPara(VOS_VOID)
{
    VOS_UINT32 rst = VOS_ERR;

    /* ����������� */
    if ((g_atParaIndex != AT_JAM_DETECT_GSM_PARA_NUM) && (g_atParaIndex != AT_JAM_DETECT_WL_PARA_NUM)) {
        return VOS_ERR;
    }

    /* ���GUL��ʽ�£�JD���ò�����Ч�� */
    switch ((AT_MTA_CmdRatmodeUint8)g_atParaList[0].paraValue) {
        case AT_MTA_CMD_RATMODE_GSM:
            rst = AT_CheckJDCfgGsmPara();
            break;

        case AT_MTA_CMD_RATMODE_WCDMA:
            rst = AT_CheckJDCfgWcdmaPara();
            break;

        case AT_MTA_CMD_RATMODE_LTE:
            rst = AT_CheckJDCfgLtePara();
            break;

        default:
            break;
    }

    return rst;
}

VOS_UINT32 AT_ProcJDCfgPara(AT_MTA_SetJamDetectReq *atCmd)
{
    /* ��ȡGUL��ʽ�£����ż�����ò��� */
    atCmd->mode = AT_MTA_JAM_DETECT_MODE_UPDATE;
    atCmd->rat  = (AT_MTA_CmdRatmodeUint8)g_atParaList[0].paraValue;

    switch ((AT_MTA_CmdRatmodeUint8)g_atParaList[0].paraValue) {
        case AT_MTA_CMD_RATMODE_GSM:
            atCmd->unJamPara.gsmPara.threshold = (VOS_UINT8)g_atParaList[1].paraValue;
            atCmd->unJamPara.gsmPara.freqNum   = (VOS_UINT8)g_atParaList[AT_JDCFG_RSSISRHFREQ_PERCENT].paraValue;
            break;

        case AT_MTA_CMD_RATMODE_WCDMA:
            atCmd->unJamPara.wcdmaPara.rssiSrhThreshold   = (VOS_UINT8)g_atParaList[1].paraValue;
            atCmd->unJamPara.wcdmaPara.rssiSrhFreqPercent =
                (VOS_UINT8)g_atParaList[AT_JDCFG_RSSISRHFREQ_PERCENT].paraValue;
            atCmd->unJamPara.wcdmaPara.pschSrhThreshold   =
                (VOS_UINT16)g_atParaList[AT_JDCFG_PSCHSRHTHRE_SHOID].paraValue;
            atCmd->unJamPara.wcdmaPara.pschSrhFreqPercent =
                (VOS_UINT8)g_atParaList[AT_JDCFG_PSCHSRHFREQ_PERCENT].paraValue;
            break;

        case AT_MTA_CMD_RATMODE_LTE:
            atCmd->unJamPara.ltePara.rssiThresh = (VOS_INT16)g_atParaList[1].paraValue -
                                                  AT_JAM_DETECT_LTE_RSSI_THRESHOLD_MAX;
            atCmd->unJamPara.ltePara.rssiPercent     = (VOS_UINT8)g_atParaList[AT_JDCFG_RSSISRHFREQ_PERCENT].paraValue;
            atCmd->unJamPara.ltePara.pssratioThresh  = (VOS_UINT16)g_atParaList[AT_JDCFG_PSCHSRHTHRE_SHOID].paraValue;
            atCmd->unJamPara.ltePara.pssratioPercent = (VOS_UINT8)g_atParaList[AT_JDCFG_PSCHSRHFREQ_PERCENT].paraValue;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_SetJDCfgPara(VOS_UINT8 indexNum)
{
    AT_MTA_SetJamDetectReq atCmd;
    VOS_UINT32             rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ч�Լ�� */
    rst = AT_CheckJDCfgPara();

    if (rst != VOS_OK) {
        AT_ERR_LOG("AT_SetJDCfgPara: AT_CheckJDCfgPara error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    /* ��ȡ���ż�����ò��� */
    rst = AT_ProcJDCfgPara(&atCmd);

    if (rst != VOS_OK) {
        return AT_ERROR;
    }

    /* �·����ż���������������Ϣ��C�˴��� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_SET_JAM_DETECT_REQ, (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_SetJamDetectReq),
                                 I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_JDCFG_SET;

        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetJDSwitchPara(VOS_UINT8 indexNum)
{
    AT_MTA_SetJamDetectReq atCmd = {0};
    VOS_UINT32             rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������� */
    if (g_atParaIndex != 1) {
        return AT_ERROR;
    }

    /* �·����ż�⿪������������Ϣ��C�˴��� */
    atCmd.mode = (VOS_UINT8)g_atParaList[0].paraValue;
    atCmd.rat  = AT_MTA_CMD_RATMODE_BUTT;

    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_SET_JAM_DETECT_REQ, (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_SetJamDetectReq),
                                 I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_JDSWITCH_SET;

        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_LTE == FEATURE_ON)
VOS_UINT32 AT_SetCeregPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϊ�� */
    if (g_atParaList[0].paraLen != 0) {
        netCtx->ceregType   = (AT_CeregTypeUint8)g_atParaList[0].paraValue;
        atCmd.u.ceregRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;

        /*
         * ����Ĭ��TAF_EVENT_INFO_CELL_SIGN+TAF_EVENT_INFO_CELL_ID + TAF_EVENT_INFO_CELL_BLER
         * ���������ϱ����˴�������֪ͨ����㿪�������ϱ���Ŀǰû��ֹͣ��ֹͣ�˻�Ӱ��CSQ��ѯ
         */
    } else {
        netCtx->ceregType   = AT_CEREG_RESULT_CODE_NOT_REPORT_TYPE;
        atCmd.u.ceregRptFlg = AT_CEREG_RESULT_CODE_NOT_REPORT_TYPE;
    }

    atCmd.reqType = AT_MTA_SET_CEREG_RPT_TYPE;

    /* ��MTA����+cgreg�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
VOS_UINT32 AT_SetC5gregPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϊ�� */
    if (g_atParaList[0].paraLen != 0) {
        netCtx->c5gregType   = (AT_C5GREG_TypeUint8)g_atParaList[0].paraValue;
        atCmd.u.c5gregRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;
    } else {
        netCtx->c5gregType   = AT_C5GREG_RESULT_CODE_NOT_REPORT_TYPE;
        atCmd.u.c5gregRptFlg = AT_C5GREG_RESULT_CODE_NOT_REPORT_TYPE;
    }

    atCmd.reqType = AT_MTA_SET_C5GREG_RPT_TYPE;

    /* ��MTA����+c5greg�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_SetTTYModePara(VOS_UINT8 indexNum)
{
    VOS_UINT32           result;
    APP_VC_SetTtymodeReq setTTYMode;

    (VOS_VOID)memset_s(&setTTYMode, sizeof(setTTYMode), 0x00, sizeof(APP_VC_SetTtymodeReq));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if ((g_atParaList[0].paraLen != 1) || (g_atParaIndex != 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    setTTYMode.ttyMode = (TAF_VC_TtymodeUint8)g_atParaList[0].paraValue;

    /* ��ȡ�·�VC����Ϣͷ���� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    APP_VC_MSG_SET_TTYMODE_REQ, (VOS_UINT8 *)&setTTYMode, sizeof(APP_VC_SetTtymodeReq),
                                    I0_WUEPS_PID_VC);

    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetTTYModePara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_TTYMODE_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetCsdfPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CSDF_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    netCtx->csdfCfg.mode    = 1;
    netCtx->csdfCfg.auxMode = 1;

    /* ������Ϊ�� */
    if (g_atParaList[0].paraLen != 0) {
        netCtx->csdfCfg.mode = (VOS_UINT8)g_atParaList[0].paraValue;
    }

    if (g_atParaList[1].paraLen != 0) {
        netCtx->csdfCfg.auxMode = (VOS_UINT8)g_atParaList[1].paraValue;
    }

    return AT_OK;
}

/*
 * ��������: ������ֶκϷ���
 */
LOCAL VOS_UINT32 AT_CheckCclkYearTimePara(VOS_UINT8 yearLen, VOS_INT32 plYear)
{
    if (yearLen == AT_MODEM_DEFALUT_AUX_MODE_YEAR_LEN) {
        if ((plYear > AT_MODEM_DEFALUT_AUX_MODE_YEAR_MAX) || (plYear < AT_MODEM_DEFALUT_AUX_MODE_YEAR_MIN)) {
            AT_ERR_LOG("AT_CheckCclkYearTimePara: The parameter of year is out of range");
            return VOS_ERR;
        }
    } else {
        if ((plYear > AT_MODEM_YEAR_MAX) || (plYear < AT_MODEM_YEAR_MIN)) {
            AT_ERR_LOG("AT_CheckCclkYearTimePara: The parameter of year is out of range");
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

VOS_UINT32 AT_ProcCclkTimeParaYMDAuxMode(VOS_UINT8 yearLen, VOS_INT32 *plYear, VOS_INT32 *plMonth, VOS_INT32 *plDay)
{
    VOS_UINT8 buffer[AT_GET_MODEM_TIME_BUFF_LEN];
    errno_t   memResult;

    /* ���ո�ʽ "(yy/yyyy)/mm/dd,hh:mm:ss(+/-)zz"�����жϷ�Χ */

    /* ���(yy/yyyy) */
    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    if ((yearLen > 0) && (yearLen <= AT_GET_MODEM_TIME_BUFF_LEN)) {
        memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), g_atParaList[AT_CCLK_TIME].para, yearLen);
        TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), yearLen);
    }

    if (AT_AtoInt((VOS_CHAR *)buffer, AT_GET_MODEM_TIME_BUFF_LEN, plYear) == VOS_ERR) {
        AT_ERR_LOG("AT_ProcCclkTimeParaYMDAuxMode: The parameter of year is err");
        return VOS_ERR;
    }

    if (AT_CheckCclkYearTimePara(yearLen, *plYear) == VOS_ERR) {
        AT_ERR_LOG("AT_ProcCclkTimeParaYMDAuxMode: The check parameter of year is err");
        return VOS_ERR;
    }

    /* ���mm */
    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer),
                         &g_atParaList[AT_CCLK_TIME].para[yearLen + 1], AT_MODEM_MONTH_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_MONTH_LEN);

    if (AT_AtoInt((VOS_CHAR *)buffer, AT_GET_MODEM_TIME_BUFF_LEN, plMonth) == VOS_ERR) {
        AT_ERR_LOG("AT_ProcCclkTimeParaYMDAuxMode: The parameter of month is err");
        return VOS_ERR;
    }

    if ((*plMonth > AT_MODEM_MONTH_MAX) || (*plMonth < AT_MODEM_MONTH_MIN)) {
        AT_ERR_LOG("AT_ProcCclkTimeParaYMDAuxMode: The parameter of month is out of range");
        return VOS_ERR;
    }

    /* ���dd */
    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer),
                         &g_atParaList[AT_CCLK_TIME].para[yearLen + 1 + AT_MODEM_MONTH_LEN + 1],AT_MODEM_DATE_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_DATE_LEN);

    if (AT_AtoInt((VOS_CHAR *)buffer, AT_GET_MODEM_TIME_BUFF_LEN, plDay) == VOS_ERR) {
        AT_ERR_LOG("AT_ProcCclkTimeParaYMDAuxMode: The parameter of day is err");
        return VOS_ERR;
    }

    if ((*plDay > AT_MODEM_DAY_MAX) || (*plDay < AT_MODEM_DAY_MIN)) {
        AT_ERR_LOG("AT_ProcCclkTimeParaYMDAuxMode: The parameter of day is out of range");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_ProcCclkTimeParaHMSAuxMode(VOS_UINT8 yearLen, VOS_INT32 *plHour, VOS_INT32 *plMin, VOS_INT32 *plSec)
{
    VOS_UINT8 buffer[AT_GET_MODEM_TIME_BUFF_LEN];
    errno_t   memResult;

    /* ���ո�ʽ "(yy/yyyy)/mm/dd,hh:mm:ss(+/-)zz"�����жϷ�Χ */

    /* ���hh */
    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), &g_atParaList[AT_CCLK_TIME].para[yearLen + 1 +
                         AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1],AT_MODEM_HOUR_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_HOUR_LEN);

    if (AT_AtoInt((VOS_CHAR *)buffer, AT_GET_MODEM_TIME_BUFF_LEN, plHour) == VOS_ERR) {
        AT_ERR_LOG("AT_ProcCclkTimeParaHMSAuxMode: The parameter of hour is err");
        return VOS_ERR;
    }

    if ((*plHour > AT_MODEM_HOUR_MAX) || (*plHour < AT_MODEM_HOUR_MIN)) {
        AT_ERR_LOG("AT_ProcCclkTimeParaHMSAuxMode: The parameter of hour is out of range");
        return VOS_ERR;
    }

    /* ���mm */
    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), &g_atParaList[AT_CCLK_TIME].para[yearLen + 1 +
                         AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 + AT_MODEM_HOUR_LEN + 1], AT_MODEM_MIN_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_MIN_LEN);

    if (AT_AtoInt((VOS_CHAR *)buffer, AT_GET_MODEM_TIME_BUFF_LEN, plMin) == VOS_ERR) {
        AT_ERR_LOG("AT_ProcCclkTimeParaHMSAuxMode: The parameter of min is err");
        return VOS_ERR;
    }

    if ((*plMin > AT_MODEM_MIN_MAX) || (*plMin < AT_MODEM_MIN_MIN)) {
        AT_ERR_LOG("AT_ProcCclkTimeParaHMSAuxMode: The parameter of min is out of range");
        return VOS_ERR;
    }

    /* ���ss */
    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), &g_atParaList[AT_CCLK_TIME].para[yearLen+ 1 +
                         AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 + AT_MODEM_HOUR_LEN + 1 + AT_MODEM_MIN_LEN +1],
                         AT_MODEM_SEC_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_SEC_LEN);

    if (AT_AtoInt((VOS_CHAR *)buffer, AT_GET_MODEM_TIME_BUFF_LEN, plSec) == VOS_ERR) {
        AT_ERR_LOG("AT_ProcCclkTimeParaHMSAuxMode: The parameter of sec is err");
        return VOS_ERR;
    }

    if ((*plSec > AT_MODEM_SEC_MAX) || (*plSec < AT_MODEM_SEC_MIN)) {
        AT_ERR_LOG("AT_ProcCclkTimeParaHMSAuxMode: The parameter of sec is out of range");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_ProcCclkTimeParaZoneAuxMode(VOS_UINT8 yearLen, VOS_INT32 *plZone)
{
    VOS_UINT8 buffer[AT_GET_MODEM_TIME_BUFF_LEN];
    errno_t   memResult;

    /* ���ո�ʽ "(yy/yyyy)/mm/dd,hh:mm:ss(+/-)zz"�����жϷ�Χ */

    /* ���(+/-)zz */
    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));

    /* 1Ϊ���ڵķָ������ */
    if (g_atParaList[AT_CCLK_TIME].para[yearLen+ 1 +AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 +
        AT_MODEM_TIME_LEN] == '-') {
        memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), &g_atParaList[AT_CCLK_TIME].para[yearLen + 1 +
                             AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 + AT_MODEM_TIME_LEN],AT_MODEM_ZONE_LEN);
        TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_ZONE_LEN);
    } else {
        memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), &g_atParaList[AT_CCLK_TIME].para[yearLen + 1 +
                             AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 + AT_MODEM_TIME_LEN + 1],
                             AT_MODEM_ZONE_LEN - 1);
        TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_ZONE_LEN - 1);
    }

    if (AT_AtoInt((VOS_CHAR *)buffer, AT_GET_MODEM_TIME_BUFF_LEN, plZone) == VOS_ERR) {
        AT_ERR_LOG("AT_ProcCclkTimeParaZoneAuxMode: The parameter of zone is err");
        return VOS_ERR;
    }

    if ((*plZone > AT_MODEM_TIME_ZONE_MAX) || (*plZone < AT_MODEM_TIME_ZONE_MIN)) {
        AT_ERR_LOG("AT_ProcCclkTimeParaZoneAuxMode: The parameter of zone is out of range");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_ProcCclkTimeParaAuxMode(VOS_UINT8 indexNum, VOS_UINT8 yearLen)
{
    VOS_INT32       sec = 0;
    VOS_INT32       min = 0;
    VOS_INT32       hour = 0;
    VOS_INT32       day = 0;
    VOS_INT32       month = 0;
    VOS_INT32       year = 0;
    VOS_INT32       zone = 0;
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    /* ���ո�ʽ "(yy/yyyy)/mm/dd,hh:mm:ss(+/-)zz"�����жϷ�Χ */

    /* ���(yy/yyyy)/mm/dd */
    if (AT_ProcCclkTimeParaYMDAuxMode(yearLen, &year, &month, &day) == VOS_ERR) {
        return VOS_ERR;
    }

    /* ���hh:mm:ss */
    if (AT_ProcCclkTimeParaHMSAuxMode(yearLen, &hour, &min, &sec) == VOS_ERR) {
        return VOS_ERR;
    }

    /* ���(+/-)zz */
    if (AT_ProcCclkTimeParaZoneAuxMode(yearLen, &zone) == VOS_ERR) {
        return VOS_ERR;
    }

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    if (yearLen == AT_MODEM_DEFALUT_AUX_MODE_YEAR_LEN) {
        netCtx->timeInfo.universalTimeandLocalTimeZone.year = (VOS_UINT8)year;
    } else {
        /* '%100'��Ϊ�˽�yyyy����ݱ�ʾ��ʽתΪyy�ķ�ʽ */
        netCtx->timeInfo.universalTimeandLocalTimeZone.year = (VOS_UINT8)(year % 100);
    }

    netCtx->timeInfo.universalTimeandLocalTimeZone.month    = (VOS_UINT8)month;
    netCtx->timeInfo.universalTimeandLocalTimeZone.day      = (VOS_UINT8)day;
    netCtx->timeInfo.universalTimeandLocalTimeZone.hour     = (VOS_UINT8)hour;
    netCtx->timeInfo.universalTimeandLocalTimeZone.minute   = (VOS_UINT8)min;
    netCtx->timeInfo.universalTimeandLocalTimeZone.second   = (VOS_UINT8)sec;
    netCtx->timeInfo.universalTimeandLocalTimeZone.timeZone = (VOS_INT8)zone;
    netCtx->timeInfo.localTimeZone                          = (VOS_INT8)zone;

    netCtx->timeInfo.ieFlg = netCtx->timeInfo.ieFlg | NAS_MM_INFO_IE_UTLTZ;

    return VOS_OK;
}

#if (FEATURE_MBB_CUST == FEATURE_ON)
VOS_UINT32 AtMbbProcCclkTimeParaAuxMode(VOS_UINT8 index, VOS_UINT8 yearLen)
{
    VOS_INT32 sec = 0;
    VOS_INT32 min = 0 ;
    VOS_INT32 hour = 0;
    VOS_INT32 day = 0;
    VOS_INT32 month = 0;
    VOS_INT32 year = 0;
    VOS_INT32 zone = 0;
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    /* ���ո�ʽ "(yy/yyyy)/mm/dd,hh:mm:ss(+/-)zz"�����жϷ�Χ */
    /* ���(yy/yyyy)/mm/dd */
    if (AT_ProcCclkTimeParaYMDAuxMode(yearLen, &year, &month, &day) == VOS_ERR) {
        return VOS_ERR;
    }

    /* ���hh:mm:ss */
    if (AT_ProcCclkTimeParaHMSAuxMode(yearLen, &hour, &min, &sec) == VOS_ERR) {
        return VOS_ERR;
    }

    /* ���(+/-)zz */
    if (AT_ProcCclkTimeParaZoneAuxMode(yearLen, &zone) == VOS_ERR) {
        return VOS_ERR;
    }

    netCtx = AT_GetModemNetCtxAddrFromClientId(index);

    if (yearLen == AT_MODEM_DEFALUT_AUX_MODE_YEAR_LEN) {
        netCtx->cclkTimeInfo.universalTimeandLocalTimeZone.year = (VOS_UINT8)year;
    } else {
        /* 100 ��׼��ݵ�ƫ���� */
        netCtx->cclkTimeInfo.universalTimeandLocalTimeZone.year = (VOS_UINT8)(year % 100);
    }

    netCtx->cclkTimeInfo.universalTimeandLocalTimeZone.month = (VOS_UINT8)month;
    netCtx->cclkTimeInfo.universalTimeandLocalTimeZone.day = (VOS_UINT8)day;
    netCtx->cclkTimeInfo.universalTimeandLocalTimeZone.hour = (VOS_UINT8)hour;
    netCtx->cclkTimeInfo.universalTimeandLocalTimeZone.minute = (VOS_UINT8)min;
    netCtx->cclkTimeInfo.universalTimeandLocalTimeZone.second = (VOS_UINT8)sec;
    netCtx->cclkTimeInfo.universalTimeandLocalTimeZone.timeZone = (VOS_INT8)zone;
    netCtx->cclkTimeInfo.localTimeZone = (VOS_INT8)zone;
    netCtx->cclkTimeInfo.ieFlg = netCtx->cclkTimeInfo.ieFlg | NAS_MM_INFO_IE_UTLTZ;
    netCtx->cclkSecond = AT_GetSeconds();

    return VOS_OK;
}
#endif

VOS_UINT32 AT_SetCclkPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;
    VOS_UINT8       yearLen;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ŀ��� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    if (netCtx->csdfCfg.auxMode == 1) {
        /* "yy/mm/dd,hh:mm:ss(+/-)zz" */
        yearLen = AT_MODEM_DEFALUT_AUX_MODE_YEAR_LEN;
    } else {
        /* "yyyy/mm/dd,hh:mm:ss(+/-)zz" */
        yearLen = AT_MODEM_OTHER_AUX_MODE_YEAR_LEN;
    }

    /* �������Ȳ���ȷ */
    if ((AT_MODEM_AUX_MODE_EXCEPT_YEAR_TIME_LEN + yearLen) != g_atParaList[AT_CCLK_TIME].paraLen) {
        AT_ERR_LOG("AT_SetCclkPara: length of parameter is error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���ո�ʽ "(yy/yyyy)/mm/dd,hh:mm:ss(+/-)zz"�����жϸ�ʽ */
    /* 1Ϊ���ڵķָ������ */
    if ((g_atParaList[AT_CCLK_TIME].para[yearLen] != '/') ||
        (g_atParaList[AT_CCLK_TIME].para[yearLen + 1 + AT_MODEM_MONTH_LEN] != '/') ||
        (g_atParaList[AT_CCLK_TIME].para[yearLen + 1 + AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN] != ',') ||
        (g_atParaList[AT_CCLK_TIME].para[yearLen + 1 + AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 +
        AT_MODEM_HOUR_LEN] != ':') ||
        (g_atParaList[AT_CCLK_TIME].para[yearLen + 1 + AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 +
        AT_MODEM_HOUR_LEN + 1 +AT_MODEM_MIN_LEN] != ':') ||
        ((g_atParaList[AT_CCLK_TIME].para[yearLen + 1 + AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 +
        AT_MODEM_TIME_LEN] != '+') &&
        (g_atParaList[AT_CCLK_TIME].para[yearLen + 1 + AT_MODEM_MONTH_LEN + 1 + AT_MODEM_DATE_LEN + 1 +
        AT_MODEM_TIME_LEN] != '-'))) {
        AT_ERR_LOG("AT_SetCclkPara: The date formats parameter is error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }
#if (FEATURE_MBB_CUST == FEATURE_ON)
    if (AtMbbProcCclkTimeParaAuxMode(indexNum, yearLen) == VOS_ERR) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#else
    if (AT_ProcCclkTimeParaAuxMode(indexNum, yearLen) == VOS_ERR) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#endif

    return AT_OK;
}

#if (FEATURE_IOT_CMUX == FEATURE_ON)

VOS_UINT32 AT_CheckCmuxSpeedAndBaudrate(cmux_info_type *atCmuxPara)
{
    TAF_NV_UartCfg uartNVCfg = {0};
    VOS_UINT32     ret = 0;
    VOS_UINT32     result;

    /* ��ȡNV���в����ʣ������� */
    result = TAF_ACORE_NV_READ(MODEM_ID_0,
                 NV_ITEM_UART_CFG,
                  &uartNVCfg,
                  sizeof(TAF_NV_UartCfg));
    if (result == VOS_OK) {
        ret = cmux_baud_to_num(uartNVCfg.baudRate);
    } else {
        /*NV��ȡʧ�ܣ�ʹ��Ĭ��ֵ���бȽ�*/
        AT_NORM_LOG("At_SetCmuxPara: read baud NV failed!");
        return AT_ERROR;
    }

    /*  <port_speed> 1 - 9600 bit/s;2 - 19200 bit/s;3 - 38400 bit/s;4 - 57600 bit/s;5 - 115200 bit/s;6 - 230400 bits/s;7 - 460800 bits/s;8 - 921600 bits/s */
    if (g_atParaList[2].paraLen != 0) {
        if ((g_atParaList[2].paraValue < CMUX_PHY_PORT_SPEED_1) || (g_atParaList[2].paraValue > CMUX_PHY_PORT_SPEED_6)) {
            AT_NORM_LOG("At_SetCmuxPara: Invalid Port speed!");
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /*CMUX���ò����ʲ�����NV�в����ʲ����*/
        if(ret != g_atParaList[2].paraValue) {
            AT_NORM_LOG("At_SetCmuxPara:port speed not equal to NV!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        atCmuxPara->port_speed = ((TAF_UINT16)g_atParaList[2].paraValue);
    } else {
        /*�ж�NV�������Ƿ���CMUX��֧�ֵķ�Χ��*/
        if (ret != CMUX_PHY_PORT_SPEED_INVALID) {
            atCmuxPara->port_speed = ret;
        } else {
            AT_NORM_LOG("NV value is not supported by CMUX!");
            return AT_ERROR;
        }
    }

    return AT_SUCCESS;
}

VOS_UINT32 At_CheckCmuxParaN2T2N3(cmux_info_type *atCmuxPara)
{
    /* <N2>: 0-10; where 3 is default */
    if (g_atParaList[5].paraLen != 0) {
        if (g_atParaList[5].paraValue > CMUX_MAX_FRAME_N2) {
            AT_NORM_LOG("At_SetCmuxPara: Invalid N2!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        atCmuxPara->max_cmd_num_tx_times_N2 = (TAF_UINT8)g_atParaList[5].paraValue;
    }

    /* <T2>: 10-250; where 90 is default (900 ms); T2 must be longer than T1. */
    if (g_atParaList[6].paraLen != 0) {
        if ((g_atParaList[6].paraValue < 10 ) || (g_atParaList[6].paraValue > 250)) {
            AT_NORM_LOG("At_SetCmuxPara: Invalid T2!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        atCmuxPara->response_timer_T2 = T1_T2_FACTOR * g_atParaList[6].paraValue;

        if (atCmuxPara->response_timer_T2 <= atCmuxPara->response_timer_T1) {
            AT_NORM_LOG("At_SetCmuxPara: T2 must be longer than T1!");
            return AT_ERROR;
        }
    }

    /* <T3>: 1-255; where 10 is default  */
    if (g_atParaList[7].paraLen != 0) {
        if ((g_atParaList[7].paraValue < 1 ) || (g_atParaList[7].paraValue > 255)) {
            AT_NORM_LOG("At_SetCmuxPara: Invalid T3!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        atCmuxPara->response_timer_T3 = g_atParaList[7].paraValue;
    }

    return AT_SUCCESS;
}

VOS_UINT32 At_CheckCmuxParaN1T1N2T2N3(cmux_info_type *atCmuxPara)
{
    VOS_UINT32 result;

    /* <N1>: 31- 1540 ;31 is default for Basic option and 64 is default for Advanced option   */
    if (g_atParaList[3].paraLen != 0) {
        if ((g_atParaList[3].paraValue < CMUX_MIN_FRAME_N1 ) || (g_atParaList[3].paraValue > CMUX_MAX_FRAME_N1)) {
            AT_NORM_LOG("At_SetCmuxPara: Invalid N1!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        atCmuxPara->max_frame_size_N1 = (TAF_UINT16)g_atParaList[3].paraValue;
    } else if (atCmuxPara->operating_mode == CMUX_MODE_ADVANCED) {
        atCmuxPara->max_frame_size_N1 = CMUX_ADVANCED_MODE_DEFAULT_FRAME_N1;
    }

    /* <T1>: 10-255; where 10 is default (100 ms)  */
    if (g_atParaList[4].paraLen != 0) {
        if ((g_atParaList[4].paraValue < 10 ) || (g_atParaList[4].paraValue > 250)) {
            AT_NORM_LOG("At_SetCmuxPara: Invalid T1!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        atCmuxPara->response_timer_T1 = T1_T2_FACTOR * g_atParaList[4].paraValue;
    }

    result = At_CheckCmuxParaN2T2N3(atCmuxPara);

    return result;
}

VOS_UINT32 At_CheckCmuxPara(cmux_info_type *atCmuxPara)
{
    VOS_UINT32 result;

    /* ������� */
    if ((g_atParaIndex < CMUX_NUM_MIN) || (g_atParaIndex > CMUX_NUM_MAX)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <operation>: 0 Basic option ; 1 Advanced option */
    if ((g_atParaList[0].paraLen == 0) || ((g_atParaList[0].paraValue != CMUX_MODE_BASIC) &&
        (g_atParaList[0].paraValue != CMUX_MODE_ADVANCED))) {
        return AT_CME_INCORRECT_PARAMETERS;
    } else {
        atCmuxPara->operating_mode = (TAF_UINT8)g_atParaList[0].paraValue;
    }

    /* <subset>: 0 UIH frames used only ;1 UI frames used only;2 I frames used only,UIH supported only */
    if (g_atParaList[1].paraLen != 0) {
        if (g_atParaList[1].paraValue != CMUX_SUBSET_UIH) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        atCmuxPara->subset = (TAF_UINT8)g_atParaList[1].paraValue;
    }

    result = AT_CheckCmuxSpeedAndBaudrate(atCmuxPara);
    if (result != AT_SUCCESS) {
        return result;
    }

    result = At_CheckCmuxParaN1T1N2T2N3(atCmuxPara);

    return result;
}

/*************************************************************
  ��������  : At_SetCmuxPara
  ��������  : ����CMUX���ܣ����봮�ڶ�·����ģʽ
              ����ͨ������
  �������  : ��
  ��������  : NA
  �������  : ��
  �� �� ֵ      : 0-ִ�гɹ�
                  ��0-ִ��ʧ�ܣ���ο�����log
*************************************************************/
VOS_UINT32 At_SetCmuxPara(VOS_UINT8 indexNum)
{
    cmux_info_type          atCmuxPara;
    VOS_UINT32              result;
    DMS_PortIdUint16        portId;
    cmux_info_type         *cmux_info = VOS_NULL_PTR;

    portId = AT_GetDmsPortIdByClientId((AT_ClientIdUint16)indexNum);

    /* ���ͨ����ֻ֧��UART��HSUART�˿��·� */
    if ((AT_CheckUartUser(indexNum) != VOS_TRUE) && (AT_CheckHsUartUser(indexNum) != VOS_TRUE)) {
        return AT_ERROR;
    }

    if (DMS_PORT_GetCmuxOpenFlg() == VOS_TRUE) {
        /* CMUX�����Ѿ����������·������Ч */
        AT_NORM_LOG("At_SetCmuxPara: CMUX is already OPEN!");
        return AT_ERROR;
    }

    /* ������ʼ�� */
    atCmuxPara.operating_mode          = CMUX_MODE_BASIC;
    atCmuxPara.subset                  = CMUX_SUBSET_UIH;
    atCmuxPara.port_speed              = CMUX_PHY_PORT_SPEED_5;
    atCmuxPara.max_frame_size_N1       = CMUX_BASIC_MODE_DEFAULT_FRAME_N1;
    atCmuxPara.response_timer_T1       = CMUX_DEFAULT_CMD_TIMER_T1;
    atCmuxPara.max_cmd_num_tx_times_N2 = CMUX_DEFAULT_MAX_TX_N2;
    atCmuxPara.response_timer_T2       = CMUX_DEFAULT_DLCI0_TIMER_T2;
    atCmuxPara.response_timer_T3       = CMUX_DEFAULT_TIMER_T3;
    atCmuxPara.window_size_k           = CMUX_DEFAULT_WINDOW_SIZE_K;

    result = At_CheckCmuxPara(&atCmuxPara);
    if (result != AT_SUCCESS) {
        return result;
    }

    /* <k>: 1-7; where 2 is default */
    if (g_atParaList[8].paraLen != 0) {
        if ((g_atParaList[8].paraValue < CMUX_WINDOW_SIZE_1 ) || (g_atParaList[8].paraValue > CMUX_WINDOW_SIZE_7)) {
            AT_NORM_LOG("At_SetCmuxPara: Invalid window size k!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        atCmuxPara.window_size_k = (TAF_UINT8)g_atParaList[8].paraValue;
    }
    cmux_info = DMS_PORT_GetCmuxInfoType();
    /* Assign the default values */
    cmux_info->operating_mode          = atCmuxPara.operating_mode;
    cmux_info->subset                  = atCmuxPara.subset;
    cmux_info->port_speed              = atCmuxPara.port_speed;
    cmux_info->max_frame_size_N1       = atCmuxPara.max_frame_size_N1;
    cmux_info->response_timer_T1       = atCmuxPara.response_timer_T1;
    cmux_info->max_cmd_num_tx_times_N2 = atCmuxPara.max_cmd_num_tx_times_N2;
    cmux_info->response_timer_T2       = atCmuxPara.response_timer_T2;
    cmux_info->response_timer_T3       = atCmuxPara.response_timer_T3;
    cmux_info->window_size_k           = atCmuxPara.window_size_k;

    /* ��ʼ��CMUX���ܽӿ� */
    if (DMS_PORT_SwitchUart2Cmux(portId, cmux_info) == VOS_ERR) {
        return AT_ERROR;
    }

    DMS_PORT_SetCmuxOpenFlg(VOS_TRUE);

    return AT_OK;
}
#endif


