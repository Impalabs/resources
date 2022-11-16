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
#include "at_custom_taf_set_cmd_proc.h"
#include "securec.h"

#include "AtParse.h"
#include "css_at_interface.h"
#include "at_input_proc.h"
#include "at_init.h"
#include "at_mdrv_interface.h"
#include "taf_ps_api.h"
#include "at_data_proc.h"
#include "at_lte_common.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_TAF_SET_CMD_PROC_C

#define AT_APRPTPORTSEL_PARA_MAX_NUM 3
#define AT_APRPTPORTSEL_PORTSELTHREE 2
#define AT_APRPTPORTSEL_PORTSEL1 0
#define AT_APRPTPORTSEL_PORTSEL2 1
#define AT_BESTFREQ_PARA_NUM 2
#define AT_EXCHANGEMODEMINFO_PARA_NUM 2
#define AT_EXCHANGEMODEMINFO_MODEMID1 0
#define AT_EXCHANGEMODEMINFO_MODEMID2 1
#define AT_BESTFREQ_DEVICEID 0
#define AT_BESTFREQ_MODE 1
#define AT_LOGENABLE_PARA_MAX_NUM 1
#define AT_LOGENABLE_PARA_ENABLE 0  /* LOGENABLE�ĵ�һ������ENABLE */
#define AT_PDPSTU_CLIENT_ID 0
#define AT_PCUIPSCALL_FLAG_INDEX 0
#define AT_CTRLPSCALL_FLAG_INDEX 1
#define AT_PCUI2PSCALL_FLAG_INDEX 2
#define AT_ACTPDPSTUB_PARA_NUM 2
#define AT_TIMESET_PARA_NUM 3
#define AT_TIMESET_DATE 0
#define AT_TIMESET_TIME 1
#define AT_TIMESET_ZONE 2
#define AT_EPDU_PARA_TRANSACTIONID 0 /* EPDU�ĵ�һ������TRANSACTIONID */
#define AT_EPDU_PARA_MSG_TYPE 1      /* EPDU�ĵڶ�������MSG_TYPE */
#define AT_EPDU_PARA_CIEVFLG 2       /* EPDU�ĵ���������CIEVFLG */
#define AT_EPDU_PARA_ENDFLAG 3       /* EPDU�ĵ��ĸ�����ENDFLAG */
#define AT_EPDU_PARA_LOCSOURCE 4     /* EPDU�ĵ��������LOCSOURCE */
#define AT_EPDU_PARA_LOCCALCERR 5    /* EPDU�ĵ���������LOCCALCERR */
#define AT_EPDU_PARA_ID 6            /* EPDU�ĵ��߸�����ID */
#define AT_EPDU_PARA_NAME 7          /* EPDU�ĵڰ˸�����NAME */
#define AT_EPDU_PARA_TOTAL 8         /* EPDU�ĵھŸ�����TOTAL */
#define AT_EPDU_PARA_INDEX 9         /* EPDU�ĵ�ʮ������INDEX */
#define AT_EPDU_PARA_DATA 10         /* EPDU�ĵ�ʮһ������DATA */
#define AT_VTFLOWRPT_PARA_NUM 1
#define AT_VTFLOWRPT_RPT_OPER 0
#define AT_UE_MODE_STATUS_PARA_NUM 2
#define AT_IMSPDPCFG_PARA_NUM 2
#define AT_SET_TRUST_NUM_CTRL_MAX_PARA_NUM 3
#define AT_CCC_PARA_MAX_NUM 2
#define AT_IMEI_CHECK_NUM_LEN 1
#define AT_RSRPCFG_LEVEL 0
#define AT_RSRPCFG_VALUE 1
#define AT_RSCPCFG_LEVEL 0
#define AT_RSCPCFG_VALUE 1
#define AT_ECIOCFG_LEVEL 0
#define AT_ECIOCFG_VALUE 1
#define AT_ECIOCFG_PARA_VALID_NUM 2
#define AT_RSRPCFG_PARA_VALID_NUM 2
#define AT_RSCPCFG_PARA_VALID_NUM 2
#define AT_MODEM_ZONE_MAX 12    /* ������ʱ�����ֵ */
#define AT_MODEM_ZONE_MIN (-12) /* ������ʱ����Сֵ */
#define AT_MBMS_PRIORITY_UNICAST 0
#define AT_MBMS_PRIORITY_MBMS 1
#define AT_MBMSINTERESTLIST_PARA_NUM 6
#define AT_MBMSINTERESTLIST_MBMS_PRIORITY 5
#define AT_MBMSCMD_MAX_PARA_NUM 3
#define AT_MBMSCMD_PARAM1 1
#define AT_MBMSCMD_PARAM2 2
#define AT_C5GRSDQRY_APPID 0
#define AT_C5GRSDQRY_OSID_APPID 1
#define AT_C5GRSDQRY_DNN 2
#define AT_C5GRSDQRY_FQDN 3
#define AT_C5GRSDQRY_CONN_CAP 4
#define AT_C5GRSDQRY_REMOTE_IPV4_ADDR 5
#define AT_C5GRSDQRY_REMOTE_IPV6_ADDR 6
#define AT_C5GRSDQRY_PROTOCOL_NUM 7
#define AT_C5GRSDQRY_SINGLE_REMOTE_PORT 8
#define AT_C5GRSDQRY_REMOTE_PORT_RANGE 9
#define AT_C5GRSDQRY_SEC_PARA_IDX 10
#define AT_C5GRSDQRY_TYPE_OF_SRV 11
#define AT_C5GRSDQRY_FLOW_LABEL 12
#define AT_C5GRSDQRY_ETHERNET_TYPE 13
#define AT_C5GRSDQRY_DES_MAC_ADDR 14
#define AT_C5GRSDQRY_CTAGVID 15
#define AT_C5GRSDQRY_STAGVID 16
#define AT_C5GRSDQRY_CTAGPCPDEI 17
#define AT_C5GRSDQRY_STAGPCPDEI 18
#define AT_QRY_URSP_RSD_PARA_MAX_NUMBER 19

#if (FEATURE_MULTI_MODEM == FEATURE_ON)
VOS_UINT32 AT_ExchangeModemInfo(VOS_UINT8 indexNum)
{
    VOS_UINT32    rst;
    ModemIdUint16 firstModemId;
    ModemIdUint16 secondModemId;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_ExchangeModemInfo: NOT AT_CMD_OPT_SET_PARA_CMD!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��������Ϊ0�����ش��� */
    if ((g_atParaList[AT_EXCHANGEMODEMINFO_MODEMID1].paraLen == 0) ||
        (g_atParaList[AT_EXCHANGEMODEMINFO_MODEMID2].paraLen == 0)) {
        AT_WARN_LOG("AT_ExchangeModemInfo: para len is 0!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ȷ */
    if (g_atParaIndex != AT_EXCHANGEMODEMINFO_PARA_NUM) {
        AT_WARN_LOG("AT_ExchangeModemInfo: para num is wrong!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����modemID��ͬ�����ش��� */
    if (g_atParaList[AT_EXCHANGEMODEMINFO_MODEMID1].paraValue ==
        g_atParaList[AT_EXCHANGEMODEMINFO_MODEMID2].paraValue) {
        AT_WARN_LOG("AT_ExchangeModemInfo: two modem ID is the same!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    firstModemId  = (ModemIdUint16)g_atParaList[AT_EXCHANGEMODEMINFO_MODEMID1].paraValue;
    secondModemId = (ModemIdUint16)g_atParaList[AT_EXCHANGEMODEMINFO_MODEMID2].paraValue;

    /* modemIDֵ��Ч�����ش��� */
    if ((firstModemId >= MODEM_ID_BUTT) || (secondModemId >= MODEM_ID_BUTT)) {
        AT_WARN_LOG("AT_ExchangeModemInfo: modem ID is butt!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    rst = TAF_MMA_ExchangeModemInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, firstModemId, secondModemId);

    if (rst != VOS_TRUE) {
        AT_ERR_LOG("AT_ExchangeModemInfo: snd msg failed!");
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EXCHANGE_MODEM_INFO_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_SetApRptPortSelectTwoPara(void)
{
    VOS_UINT32                    result0;
    VOS_UINT32                    result1;
    AT_PortRptCfg                 rptCfg;
    VOS_UINT32                    clientCfgTabLen;
    const AT_ClientCfgMapTab     *cfgMapTbl = VOS_NULL_PTR;
    AT_ClientConfiguration       *clientCfg = VOS_NULL_PTR;
    VOS_UINT8                     i;

    (VOS_VOID)memset_s(&rptCfg, sizeof(rptCfg), 0x00, sizeof(rptCfg));
    /* AT�Ķ˿ڸ�������32��������������32bit�Ĳ�����ʾ��Ӧ�Ķ˿��Ƿ����������ϱ� */
    result0 = AT_String2Hex(g_atParaList[0].para, g_atParaList[0].paraLen, &rptCfg.rptCfgBit32[0]);
    result1 = AT_String2Hex(g_atParaList[1].para, g_atParaList[1].paraLen, &rptCfg.rptCfgBit32[1]);

    if ((result0 == VOS_ERR) || (result1 == VOS_ERR)) {
        AT_ERR_LOG1("AT_SetApRptPortSelectTwoPara: ulResult0 is d%", result0);
        AT_ERR_LOG1("AT_SetApRptPortSelectTwoPara: ulResult1 is d%", result1);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    clientCfgTabLen = AT_GET_CLIENT_CFG_TAB_LEN();
    /*  FEATURE_VCOM_EXT��رյ�ʱ��ulClientCfgTabLen �ܳ���Ϊ2������Ҫ�� */
#if (FEATURE_VCOM_EXT == FEATURE_ON)
    /* ����������ʱ����Ҫ�ܳ�����Ҫ��ȥһ������������ĳ���32���������Ԥ����6���˿ڣ�������26 */
    clientCfgTabLen = clientCfgTabLen - 26;
#endif

    for (i = 0; i < clientCfgTabLen; i++) {
        cfgMapTbl = AT_GetClientCfgMapTbl(i);
        clientCfg = AT_GetClientConfig(cfgMapTbl->clientId);

        clientCfg->reportFlg = ((rptCfg.rptCfgBit32[0] & cfgMapTbl->rptCfgBit32[0]) ||
                                (rptCfg.rptCfgBit32[1] & cfgMapTbl->rptCfgBit32[1])) ?
                                   VOS_TRUE :
                                   VOS_FALSE;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetApRptPortSelectThreePara(void)
{
    VOS_UINT32                    result0;
    VOS_UINT32                    result1;
    VOS_UINT32                    result2;
    AT_PortRptCfg                 rptCfg;
    const AT_ClientCfgMapTab     *cfgMapTbl = VOS_NULL_PTR;
    AT_ClientConfiguration       *clientCfg = VOS_NULL_PTR;
    VOS_UINT8                     i;

    (VOS_VOID)memset_s(&rptCfg, sizeof(rptCfg), 0x00, sizeof(rptCfg));
    /* ��ȡ�û����õĲ��� */
    /* AT�Ķ˿ڸ�������64��������������32bit�Ĳ�����ʾ��Ӧ�Ķ˿��Ƿ����������ϱ� */
    result0 = AT_String2Hex(g_atParaList[AT_APRPTPORTSEL_PORTSEL1].para, g_atParaList[AT_APRPTPORTSEL_PORTSEL1].paraLen,
                            &rptCfg.rptCfgBit32[AT_RPT_CFG_BIT32_INDEX_0]);
    result1 = AT_String2Hex(g_atParaList[AT_APRPTPORTSEL_PORTSEL2].para, g_atParaList[AT_APRPTPORTSEL_PORTSEL2].paraLen,
                            &rptCfg.rptCfgBit32[AT_RPT_CFG_BIT32_INDEX_1]);
    result2 = AT_String2Hex(g_atParaList[AT_APRPTPORTSEL_PORTSELTHREE].para,
                            g_atParaList[AT_APRPTPORTSEL_PORTSELTHREE].paraLen,
                            &rptCfg.rptCfgBit32[AT_RPT_CFG_BIT32_INDEX_2]);

    if ((result0 == VOS_ERR) || (result1 == VOS_ERR) || (result2 == VOS_ERR)) {
        AT_ERR_LOG1("AT_SetApRptPortSelectThreePara: ulResult0 is d%", result0);
        AT_ERR_LOG1("AT_SetApRptPortSelectThreePara: ulResult1 is d%", result1);
        AT_ERR_LOG1("AT_SetApRptPortSelectThreePara: ulResult2 is d%", result2);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    for (i = 0; i < AT_GET_CLIENT_CFG_TAB_LEN(); i++) {
        cfgMapTbl = AT_GetClientCfgMapTbl(i);
        clientCfg = AT_GetClientConfig(cfgMapTbl->clientId);

        clientCfg->reportFlg = ((rptCfg.rptCfgBit32[0] & cfgMapTbl->rptCfgBit32[0]) ||
                                (rptCfg.rptCfgBit32[1] & cfgMapTbl->rptCfgBit32[1]) ||
                                (rptCfg.rptCfgBit32[AT_APRPTPORTSEL_PORTSELTHREE] &
                                 cfgMapTbl->rptCfgBit32[AT_APRPTPORTSEL_PORTSELTHREE])) ?
                                    VOS_TRUE :
                                    VOS_FALSE;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetApRptPortSelectPara(VOS_UINT8 indexNum)
{
    /* ͨ����� */
    if (AT_IsApPort(indexNum) == VOS_FALSE) {
        return AT_ERROR;
    }

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_APRPTPORTSEL_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* Ϊ�˼���������ǰ������������AT���� �ܳ���Ҫ��ȥ32 */
    if (g_atParaList[AT_APRPTPORTSEL_PORTSELTHREE].paraLen == 0) {
        return AT_SetApRptPortSelectTwoPara();
    }

    return AT_SetApRptPortSelectThreePara();
}

VOS_UINT32 AT_SetCposPara(VOS_UINT8 indexNum)
{
    AT_ModemAgpsCtx *agpsCtx = VOS_NULL_PTR;

    agpsCtx = AT_GetModemAgpsCtxAddrFromClientId(indexNum);

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* Ϊ׼�������XML���������ڴ� */
    agpsCtx->xml.xmlTextHead = (VOS_CHAR *)PS_MEM_ALLOC(WUEPS_PID_AT, AT_XML_MAX_LEN);
    if (agpsCtx->xml.xmlTextHead == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetCposPara: Memory malloc failed!");
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(agpsCtx->xml.xmlTextHead, AT_XML_MAX_LEN, 0x00, AT_XML_MAX_LEN);

    agpsCtx->xml.xmlTextCur = agpsCtx->xml.xmlTextHead;

    /* �л�ΪAGPS XML��������ģʽ */
    At_SetCmdSubMode(indexNum, AT_XML_MODE);

    /* ������ʾXML��������״̬ */
    return AT_WAIT_XML_INPUT;
}

VOS_UINT32 AT_SetCposrPara(VOS_UINT8 indexNum)
{
    AT_ModemAgpsCtx *agpsCtx = VOS_NULL_PTR;

    TAF_NVIM_XcposrrptCfg xcposrRptCfg;
    (VOS_VOID)memset_s(&xcposrRptCfg, sizeof(xcposrRptCfg), 0x00, sizeof(xcposrRptCfg));

    agpsCtx = AT_GetModemAgpsCtxAddrFromClientId(indexNum);

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    agpsCtx->cposrReport = (AT_CposrFlgUnit8)g_atParaList[0].paraValue;

    /* ��ȡNV�� */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_XCPOSRRPT_CFG, &xcposrRptCfg, (VOS_SIZE_T)sizeof(xcposrRptCfg)) ==
        NV_OK) {
        xcposrRptCfg.cposrDefault = agpsCtx->cposrReport;
        if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_XCPOSRRPT_CFG, (VOS_UINT8 *)&xcposrRptCfg,
                               (VOS_SIZE_T)sizeof(xcposrRptCfg)) != NV_OK) {
            AT_ERR_LOG("AT_SetCposrPara: write NV_ITEM_XCPOSRRPT_CFG Error");
        }
    } else {
        AT_ERR_LOG("AT_SetCposrPara: read NV_ITEM_XCPOSRRPT_CFG Error");
    }

    return AT_OK;
}

VOS_UINT32 AT_SetXcposrPara(VOS_UINT8 indexNum)
{
    VOS_UINT32          result;
    AT_MTA_SetXcposrReq setXcposr;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&setXcposr, sizeof(setXcposr), 0x00, sizeof(setXcposr));

    setXcposr.xcposrEnableCfg = (AT_MTA_XcposrCfgUnit8)g_atParaList[0].paraValue;

    /* ���Ϳ����Ϣ��C��, ���ñ���ģʽ */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    ID_AT_MTA_XCPOSR_SET_REQ, (VOS_UINT8 *)&setXcposr, sizeof(setXcposr),
                                    I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetXcposrPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_XCPOSR_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetXcposrRptPara(VOS_UINT8 indexNum)
{
    VOS_UINT32             result;
    AT_MTA_SetXcposrrptReq setXcposrRpt;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&setXcposrRpt, sizeof(setXcposrRpt), 0x00, sizeof(setXcposrRpt));

    setXcposrRpt.xcposrRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ���Ϳ����Ϣ��C��, ���ñ���ģʽ */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    ID_AT_MTA_XCPOSRRPT_SET_REQ, (VOS_UINT8 *)&setXcposrRpt, sizeof(setXcposrRpt),
                                    I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetXcposrRptPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_XCPOSRRPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetGpsInfoPara(VOS_UINT8 indexNum)
{
    return AT_OK;
}

VOS_UINT32 AT_SetLogSavePara(VOS_UINT8 indexNum)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetBestFreqPara(VOS_UINT8 indexNum)
{
    AT_MTA_BestfreqSetReq bestFreqSetReq;
    VOS_UINT32            rslt;
    ModemIdUint16         modemId;

    /* ָ�����ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_SetBestFreqPara : Current Option is not AT_CMD_OPT_SET_PARA_CMD.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���������򳤶Ȳ���ȷ */
    if ((g_atParaIndex != AT_BESTFREQ_PARA_NUM) || (g_atParaList[AT_BESTFREQ_DEVICEID].paraLen == 0) ||
        (g_atParaList[AT_BESTFREQ_MODE].paraLen == 0)) {
        AT_WARN_LOG("AT_SetBestFreqPara : The number of input parameters is error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&bestFreqSetReq, sizeof(bestFreqSetReq), 0x00, sizeof(AT_MTA_BestfreqSetReq));
    modemId = MODEM_ID_BUTT;

    /* ͨ��clientID���ModemID */
    rslt = AT_GetModemIdFromClient(g_atClientTab[indexNum].clientId, &modemId);

    /* ���ModemID��ȡʧ�ܻ���Modem0������ʧ�� */
    if ((rslt != VOS_OK) || (modemId != MODEM_ID_0)) {
        AT_WARN_LOG("AT_SetBestFreqPara: AT_GetModemIdFromClient failed or not modem0!");
        return AT_ERROR;
    }

    /* ��д��Ϣ */
    bestFreqSetReq.deviceId = (VOS_UINT8)g_atParaList[AT_BESTFREQ_DEVICEID].paraValue;
    bestFreqSetReq.mode     = (VOS_UINT8)g_atParaList[AT_BESTFREQ_MODE].paraValue;

    /* ���Ϳ����Ϣ��C��, ������������ģʽ */
    rslt = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                  ID_AT_MTA_BESTFREQ_SET_REQ, (VOS_UINT8 *)&bestFreqSetReq,
                                  (VOS_SIZE_T)sizeof(bestFreqSetReq), I0_UEPS_PID_MTA);

    if (rslt != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetBestFreqPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_BESTFREQ_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetMtReattachPara(VOS_UINT8 indexNum)
{
    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_TOO_MANY_PARA;
    }

    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[0].paraValue > 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));
    atCmd.reqType            = AT_MTA_SET_MTREATTACH_RPT_TYPE;
    atCmd.u.mtReattachRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ��MTA����^MTREATTACH�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 At_SetSensorPara(VOS_UINT8 indexNum)
{
    VOS_UINT32          rst;
    AT_MTA_SetSensorReq sensorStat;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("At_SetSensorPara: Incorrect Cmd Type.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaIndex != 1)) {
        AT_WARN_LOG("At_SetSensorPara: Incorrect Param Length.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&sensorStat, sizeof(sensorStat), 0x00, sizeof(sensorStat));

    sensorStat.sensorStat = g_atParaList[0].paraValue;

    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_SET_SENSOR_REQ,
                                 (VOS_UINT8 *)&sensorStat, sizeof(sensorStat), I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SENSOR_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("At_SetSensorPara: Send msg fail!");
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetNvRefreshPara(VOS_UINT8 indexNum)
{
    AT_MTA_NvRefreshSetReq atCmd;
    VOS_UINT32             result;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Phone�濨NV */
    AT_ReadIpv6AddrTestModeCfgNV();
    AT_ReadPrivacyFilterCfgNv();
    AT_InitStk();
    AT_ReadWasCapabilityNV();
    AT_ReadAgpsNv();

    /* ����MBB�濨NV */
    AT_ReadSmsNV();
    AT_ReadRoamCapaNV();
    AT_ReadIpv6CapabilityNV();
    AT_ReadGasCapabilityNV();
    AT_ReadApnCustomFormatCfgNV();

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    atCmd.reason = AT_MTA_NV_REFRESH_USIM_DEPENDENT;

    /* ��MTA����NV_REFRESH�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_NV_REFRESH_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_NvRefreshSetReq), I0_UEPS_PID_MTA);

    if (result == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NVREFRESH_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_ERR_LOG("AT_SetNvRefreshPara:  AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }
}

LOCAL VOS_VOID AT_SendCssScreenStatusInd(VOS_UINT8 status)
{
    AT_CSS_ScreenStatusInd *msg = VOS_NULL_PTR;
    VOS_UINT32              bufLen;

    bufLen = sizeof(AT_CSS_ScreenStatusInd);

    msg = (AT_CSS_ScreenStatusInd *)AT_ALLOC_MSG_WITH_HDR(bufLen);

    /* �ڴ�����ʧ�ܣ����� */
    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("msg alloc fail.");
        return;
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)msg + VOS_MSG_HEAD_LENGTH, bufLen - VOS_MSG_HEAD_LENGTH, 0x00, bufLen -
        VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, PS_PID_CSS, ID_AT_CSS_SCREEN_STATUS_IND);
    /* ��д��Ϣ���� */
    msg->status = status;

    /* ������Ϣ */
    AT_SEND_MSG(msg);
}

VOS_UINT32 At_SetScreenPara(VOS_UINT8 indexNum)
{
    VOS_UINT32          rst;
    AT_MTA_SetScreenReq screenStat;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("At_SetScreenPara: Incorrect Cmd Type.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if ((g_atParaList[0].paraLen != 1) || (g_atParaIndex != 1)) {
        AT_WARN_LOG("At_SetScreenPara: Incorrect Param Length.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&screenStat, sizeof(screenStat), 0, sizeof(screenStat));

    screenStat.screenStat = (VOS_UINT8)g_atParaList[0].paraValue;

    AT_SendCssScreenStatusInd(screenStat.screenStat);

    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_SET_SCREEN_REQ,
                                 (VOS_UINT8 *)&screenStat, sizeof(screenStat), I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SCREEN_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("At_SetScreenPara: Send msg fail!");
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetLogEnablePara(VOS_UINT8 indexNum)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != AT_LOGENABLE_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[AT_LOGENABLE_PARA_ENABLE].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetActPdpStubPara(VOS_UINT8 indexNum)
{
    VOS_UINT8 flag;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != AT_ACTPDPSTUB_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ȡ���õı�־ */
    flag = (VOS_UINT8)g_atParaList[1].paraValue;

    /* ����MODEM ID���ò�ͬ��׮���� */
    if (g_atParaList[AT_PDPSTU_CLIENT_ID].paraValue == AT_PCUIPSCALL_FLAG_INDEX) {
        AT_SetPcuiPsCallFlag(flag, AT_CLIENT_ID_APP);
    } else if (g_atParaList[AT_PCUIPSCALL_FLAG_INDEX].paraValue == AT_CTRLPSCALL_FLAG_INDEX) {
        AT_SetCtrlPsCallFlag(flag, AT_CLIENT_ID_APP);
    } else if (g_atParaList[AT_PCUIPSCALL_FLAG_INDEX].paraValue == AT_PCUI2PSCALL_FLAG_INDEX) {
        AT_SetPcui2PsCallFlag(flag, AT_CLIENT_ID_APP);
    } else {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetCtzuPara(VOS_UINT8 indexNum)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen == 0) {
        g_ctzuFlag = 0;
    } else {
        g_ctzuFlag = g_atParaList[0].paraValue;
    }

    return AT_OK;
}

VOS_UINT32 AT_TimeParaYTDCheck(AT_MTA_ModemTime *atMtaModemTime)
{
    VOS_UINT8 buffer[AT_GET_MODEM_TIME_BUFF_LEN];
    errno_t   memResult;

    /* ������������VER���Ȳ���ȷ */
    if (g_atParaList[AT_TIMESET_DATE].paraLen != AT_MODEM_YTD_LEN) {
        AT_ERR_LOG("AT_TimeParaYTDCheck: length of YTD parameter is error.");
        return VOS_ERR;
    }

    /* ���ո�ʽ YYYY/MM/DD ���������գ����жϸ�ʽ����Χ */
    if ((g_atParaList[AT_TIMESET_DATE].para[AT_MODEM_YEAR_LEN] != '/') ||
         /* 1Ϊ���ڵķָ������ */
        (g_atParaList[AT_TIMESET_DATE].para[AT_MODEM_YEAR_LEN+1+AT_MODEM_MONTH_LEN] != '/')) {
        AT_ERR_LOG("AT_TimeParaYTDCheck: The date formats parameter is error.");
        return VOS_ERR;
    }

    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), g_atParaList[AT_TIMESET_DATE].para, AT_MODEM_YEAR_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_YEAR_LEN);
    atMtaModemTime->year = (VOS_INT32)AT_AtoI((VOS_CHAR *)buffer, AT_MODEM_YEAR_LEN);

    if ((atMtaModemTime->year > AT_MODEM_YEAR_MAX) || (atMtaModemTime->year < AT_MODEM_YEAR_MIN)) {
        AT_ERR_LOG("AT_TimeParaYTDCheck: The parameter of year is out of range");
        return VOS_ERR;
    }

    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), &g_atParaList[AT_TIMESET_DATE].para[AT_MODEM_YEAR_LEN+1],
                         AT_MODEM_MONTH_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_MONTH_LEN);
    atMtaModemTime->month = (VOS_INT32)AT_AtoI((VOS_CHAR *)buffer, AT_MODEM_MONTH_LEN);

    if ((atMtaModemTime->month > AT_MODEM_MONTH_MAX) || (atMtaModemTime->month < AT_MODEM_MONTH_MIN)) {
        AT_ERR_LOG("AT_TimeParaYTDCheck: The parameter of month is out of range");
        return VOS_ERR;
    }

    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer),&g_atParaList[AT_TIMESET_DATE].para[AT_MODEM_YEAR_LEN + 1+
                         AT_MODEM_MONTH_LEN +1], AT_MODEM_DATE_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_DATE_LEN);
    atMtaModemTime->day = (VOS_INT32)AT_AtoI((VOS_CHAR *)buffer, AT_MODEM_DATE_LEN);

    if ((atMtaModemTime->day > AT_MODEM_DAY_MAX) || (atMtaModemTime->day < AT_MODEM_DAY_MIN)) {
        AT_ERR_LOG("AT_TimeParaYTDCheck: The parameter of day is out of range");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_TimeParaTimeCheck(AT_MTA_ModemTime *atMtaModemTime)
{
    VOS_UINT8 buffer[AT_GET_MODEM_TIME_BUFF_LEN];
    errno_t   memResult;

    if (g_atParaList[AT_TIMESET_TIME].paraLen != AT_MODEM_TIME_LEN) {
        AT_ERR_LOG("AT_TimeParaTimeCheck: length of time parameter is error.");
        return VOS_ERR;
    }

    /* ���ո�ʽ HH:MM:SS ����ʱ�䣬���жϸ�ʽ����Χ */
    /* 1Ϊ���ڵķָ������ */
    if ((g_atParaList[AT_TIMESET_TIME].para[AT_MODEM_HOUR_LEN] != ':') ||
        (g_atParaList[AT_TIMESET_TIME].para[AT_MODEM_HOUR_LEN + 1 + AT_MODEM_MIN_LEN] != ':')) {
        AT_ERR_LOG("AT_TimeParaTimeCheck: The time formats parameter is error.");
        return VOS_ERR;
    }

    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), g_atParaList[AT_TIMESET_TIME].para, AT_MODEM_HOUR_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_HOUR_LEN);
    atMtaModemTime->hour = (VOS_INT32)AT_AtoI((VOS_CHAR *)buffer, AT_MODEM_HOUR_LEN);

    if ((atMtaModemTime->hour > AT_MODEM_HOUR_MAX) || (atMtaModemTime->hour < AT_MODEM_HOUR_MIN)) {
        AT_ERR_LOG("AT_TimeParaTimeCheck: The parameter of hour is out of range");
        return VOS_ERR;
    }

    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer),
                         &g_atParaList[AT_TIMESET_TIME].para[AT_MODEM_HOUR_LEN + 1], AT_MODEM_MIN_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_MIN_LEN);
    atMtaModemTime->min = (VOS_INT32)AT_AtoI((VOS_CHAR *)buffer, AT_MODEM_MIN_LEN);

    if ((atMtaModemTime->min > AT_MODEM_MIN_MAX) || (atMtaModemTime->min < AT_MODEM_MIN_MIN)) {
        AT_ERR_LOG("AT_TimeParaTimeCheck: The parameter of min is out of range");
        return VOS_ERR;
    }

    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    /* 1Ϊ���ڵķָ������ */
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer),&g_atParaList[AT_TIMESET_TIME].para[AT_MODEM_HOUR_LEN + 1+
                         AT_MODEM_MIN_LEN + 1], AT_MODEM_SEC_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), AT_MODEM_SEC_LEN);
    atMtaModemTime->sec = (VOS_INT32)AT_AtoI((VOS_CHAR *)buffer, AT_MODEM_SEC_LEN);

    if ((atMtaModemTime->sec > AT_MODEM_SEC_MAX) || (atMtaModemTime->sec < AT_MODEM_SEC_MIN)) {
        AT_ERR_LOG("AT_TimeParaTimeCheck: The parameter of second is out of range");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_TimeParaZoneCheck(AT_MTA_ModemTime *atMtaModemTime)
{
    VOS_UINT8 buffer[AT_GET_MODEM_TIME_BUFF_LEN];
    errno_t   memResult;

    if ((g_atParaList[AT_TIMESET_ZONE].paraLen == 0) ||
        (g_atParaList[AT_TIMESET_ZONE].paraLen >= AT_GET_MODEM_TIME_BUFF_LEN)) {
        AT_ERR_LOG1("AT_TimeParaZoneCheck: length of zone parameter is wrong.", g_atParaList[AT_TIMESET_ZONE].paraLen);
        return VOS_ERR;
    }

    (VOS_VOID)memset_s(buffer, (VOS_SIZE_T)sizeof(buffer), 0x00, (VOS_SIZE_T)sizeof(buffer));
    memResult = memcpy_s(buffer, (VOS_SIZE_T)sizeof(buffer), g_atParaList[AT_TIMESET_ZONE].para,
                         g_atParaList[AT_TIMESET_ZONE].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)sizeof(buffer), g_atParaList[AT_TIMESET_ZONE].paraLen);

    /* ʱ����Χ��[-12, 12] */
    if (AT_AtoInt((VOS_CHAR *)buffer, AT_GET_MODEM_TIME_BUFF_LEN, &atMtaModemTime->zone) == VOS_ERR) {
        return VOS_ERR;
    }

    if ((atMtaModemTime->zone > AT_MODEM_ZONE_MAX) || (atMtaModemTime->zone < AT_MODEM_ZONE_MIN)) {
        AT_ERR_LOG("AT_TimeParaZoneCheck: The parameter of zone is out of range.");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_SetModemTimePara(VOS_UINT8 indexNum)
{
    AT_MTA_ModemTime atMtaModemTime;
    VOS_UINT32       rst;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ŀ��� */
    if (g_atParaIndex != AT_TIMESET_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������ʽ��� */
    (VOS_VOID)memset_s(&atMtaModemTime, (VOS_SIZE_T)sizeof(atMtaModemTime), 0x00, (VOS_SIZE_T)sizeof(atMtaModemTime));

    /* ��������� */
    if (AT_TimeParaYTDCheck(&atMtaModemTime) == VOS_ERR) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���ʱ�� */
    if (AT_TimeParaTimeCheck(&atMtaModemTime) == VOS_ERR) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���ʱ�� */
    if (AT_TimeParaZoneCheck(&atMtaModemTime) == VOS_ERR) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϣ ID_AT_MTA_MODEM_TIME_SET_REQ ��MTA��������Ϣ������ stAtMtaModemTime */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_MODEM_TIME_SET_REQ,
                                 (VOS_UINT8 *)&atMtaModemTime, (VOS_SIZE_T)sizeof(atMtaModemTime), I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MODEM_TIME_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("AT_SetModemTimePara: send ReqMsg fail");
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetCrrconnPara(VOS_UINT8 indexNum)
{
    AT_MTA_SetCrrconnReq setCrrconn;
    VOS_UINT32           rst;

    (VOS_VOID)memset_s(&setCrrconn, sizeof(setCrrconn), 0x00, sizeof(setCrrconn));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ȷ */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������ֵ */
    setCrrconn.enable = (AT_MTA_CfgUint8)g_atParaList[0].paraValue;

    /* ���Ϳ����Ϣ��C�ˣ�����CRRCONN�����ϱ����� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_CRRCONN_SET_REQ, (VOS_UINT8 *)&setCrrconn, sizeof(setCrrconn),
                                 I0_UEPS_PID_MTA);
    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetCrrconnPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CRRCONN_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_CheckEpduParaValid(VOS_VOID)
{
    /* ������� */
    if (g_atParaIndex != AT_CMD_EPDU_PARA_NUM) {
        return VOS_FALSE;
    }

    if (g_atParaList[AT_EPDU_PARA_NAME].paraLen > AT_MTA_EPDU_NAME_LENGTH_MAX) {
        return VOS_FALSE;
    }

    if (g_atParaList[AT_EPDU_PARA_DATA].paraLen > AT_MTA_EPDU_CONTENT_STRING_LEN_MAX) {
        return VOS_FALSE;
    }

    /*
     * g_atParaList[8].ulParaValue��ʾtotalֵ�� g_atParaList[9].ulParaValue��ʾindexֵ
     * ���totalֵС��indexֱֵ�ӷ���
     */
    if (g_atParaList[AT_EPDU_PARA_TOTAL].paraValue < g_atParaList[AT_EPDU_PARA_INDEX].paraValue) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

VOS_UINT32 At_SetEpduPara(VOS_UINT8 indexNum)
{
    errno_t           memResult;
    AT_MTA_EpduSetReq atCmd;
    VOS_UINT32        rst;
    VOS_UINT16        epduLen;

    if (AT_CheckEpduParaValid() != VOS_TRUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    epduLen = 0;

    /* AT������MTA����Ϣ�ṹ��ֵ */
    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(AT_MTA_EpduSetReq));
    atCmd.transactionId    = (VOS_UINT16)g_atParaList[AT_EPDU_PARA_TRANSACTIONID].paraValue;
    atCmd.msgBodyType      = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_MSG_TYPE].paraValue;
    atCmd.commonIeValidFlg = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_CIEVFLG].paraValue;
    atCmd.endFlag          = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_ENDFLAG].paraValue;
    atCmd.locSource        = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_LOCSOURCE].paraValue;
    atCmd.locCalcErr       = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_LOCCALCERR].paraValue;
    atCmd.id               = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_ID].paraValue;
    atCmd.totalNum         = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_TOTAL].paraValue;
    atCmd.index            = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_INDEX].paraValue;

    if (g_atParaList[AT_EPDU_PARA_NAME].paraLen != 0) {
        atCmd.nameLength = (VOS_UINT8)g_atParaList[AT_EPDU_PARA_NAME].paraLen;
        memResult = memcpy_s(atCmd.name, AT_MTA_EPDU_NAME_LENGTH_MAX, g_atParaList[AT_EPDU_PARA_NAME].para,
                             g_atParaList[AT_EPDU_PARA_NAME].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_MTA_EPDU_NAME_LENGTH_MAX, g_atParaList[AT_EPDU_PARA_NAME].paraLen);
    }

    if (g_atParaList[AT_EPDU_PARA_DATA].paraLen != 0) {
        epduLen = g_atParaList[AT_EPDU_PARA_DATA].paraLen;

        if (At_AsciiNum2HexString(g_atParaList[AT_EPDU_PARA_DATA].para, &epduLen) == AT_FAILURE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        atCmd.epduLength = epduLen;

        memResult = memcpy_s(atCmd.epduContent, AT_MTA_EPDU_CONTENT_LENGTH_MAX,
                             g_atParaList[AT_EPDU_PARA_DATA].para, epduLen);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_MTA_EPDU_CONTENT_LENGTH_MAX, epduLen);
    }

    /* ������Ϣ��C�˴��� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, ID_AT_MTA_EPDU_SET_REQ,
                                 (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_EpduSetReq), I0_UEPS_PID_MTA);

    if (rst == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EPDU_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

TAF_UINT32 AT_SetVTFlowRptPara(TAF_UINT8 indexNum)
{
    TAF_VTFLOW_ReportConfig reportConfigInfo;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&reportConfigInfo, sizeof(reportConfigInfo), 0x00, sizeof(TAF_VTFLOW_ReportConfig));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_VTFLOWRPT_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[AT_VTFLOWRPT_RPT_OPER].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���ò��� */
    reportConfigInfo.rptEnabled = (VOS_UINT8)g_atParaList[AT_VTFLOWRPT_RPT_OPER].paraValue;

    /* ִ��������� */
    if (TAF_PS_ConfigVTFlowRpt(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                               &reportConfigInfo) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_VTFLOWRPT_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetDataSwitchStatus(VOS_UINT8 indexNum)
{
    VOS_UINT8 dataSwitchAT;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_SetDataSwitchStatus: NOT AT_CMD_OPT_SET_PARA_CMD!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ȷ */
    if (g_atParaIndex != 1) {
        AT_WARN_LOG("AT_SetDataSwitchStatus: para num is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen != 1) {
        AT_WARN_LOG("AT_SetDataSwitchStatus: usParaLen  is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    dataSwitchAT = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ��TAF APS����֪ͨ��Ϣ */
    if (TAF_PS_Set_DataSwitchStatus(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                    dataSwitchAT) != VOS_OK) {
        AT_WARN_LOG("AT_SetDataSwitchStatus():TAF_PS_Set_DataSwitchStatus fail");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DATASWITCH_SET;
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetDataRoamSwitchStatus(VOS_UINT8 indexNum)
{
    VOS_UINT8 dataRoamSwitchAT;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_SetDataRoamSwitchStatus: NOT AT_CMD_OPT_SET_PARA_CMD!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ȷ */
    if (g_atParaIndex != 1) {
        AT_WARN_LOG("AT_SetDataRoamSwitchStatus: para num is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen != 1) {
        AT_WARN_LOG("AT_SetDataRoamSwitchStatus: usParaLen  is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    dataRoamSwitchAT = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ��TAF APS����֪ͨ��Ϣ */
    if (TAF_PS_Set_DataRoamSwitchStatus(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                        dataRoamSwitchAT) != VOS_OK) {
        AT_WARN_LOG("AT_SetDataRoamSwitchStatus():TAF_PS_Set_DataRoamSwitchStatus fail");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DATAROAMSWITCH_SET;

    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
VOS_UINT32 AT_SetLendcPara(VOS_UINT8 indexNum)
{
    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

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

    atCmd.u.lendcRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;
    atCmd.reqType       = AT_MTA_SET_LENDC_RPT_TYPE;

    /* ��MTA����^LENDC�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 AT_SetCpolicyRptPara(VOS_UINT8 indexNum)
{
    /* ָ�����ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_PS_SetUePolicyRpt(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                              (VOS_UINT8)g_atParaList[0].paraValue) != VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPOLICYRPT_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_GetCpolicyCodePara(VOS_UINT8 indexNum)
{
    /* ָ�����ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_PS_GetUePolicyInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                               (VOS_UINT8)g_atParaList[0].paraValue) != VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPOLICYCODE_QRY;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetLadnRptPara(VOS_UINT8 idx)
{
    AT_MTA_SetLadnRptReq ladnRptReq;
    VOS_UINT32           result;

    (VOS_VOID)memset_s(&ladnRptReq, sizeof(ladnRptReq), 0x00, sizeof(ladnRptReq));

    if (g_atParaIndex == 1) {
        ladnRptReq.unsolicitedRptFlg = g_atParaList[0].paraValue;
    } else {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    result = AT_FillAndSndAppReqMsg(g_atClientTab[idx].clientId, g_atClientTab[idx].opId, ID_AT_MTA_SET_LADN_RPT_REQ,
                                    (VOS_UINT8 *)&ladnRptReq, (VOS_SIZE_T)sizeof(ladnRptReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetLadnRptPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }
    g_atClientTab[idx].cmdCurrentOpt = AT_CMD_CLADN_SET;
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_ParseC5gRsdAppId(PS_URSP_Td *tdPara)
{
    if (g_atParaList[AT_C5GRSDQRY_APPID].paraLen != 0) {
        if (g_atParaList[AT_C5GRSDQRY_APPID].paraLen > PS_URSP_OS_APP_ID_MAX_LEN * 2) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (At_AsciiString2HexSimple(tdPara->osAppId.value, g_atParaList[AT_C5GRSDQRY_APPID].para,
            g_atParaList[AT_C5GRSDQRY_APPID].paraLen) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        tdPara->osAppId.len = g_atParaList[AT_C5GRSDQRY_APPID].paraLen / 2;
        tdPara->type |= PS_URSP_TD_OS_APP_ID_TYPE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_ParseC5gRsdOsIdAndAppId(PS_URSP_Td *tdPara)
{
    VOS_UINT32 appIdLen;

    if (g_atParaList[AT_C5GRSDQRY_OSID_APPID].paraLen != 0) {
        /* os id ���ȹ̶�16���ֽڣ�����ܳ���С�ڵ���16��������appid��Ϣ */
        if (g_atParaList[AT_C5GRSDQRY_OSID_APPID].paraLen <= (PS_URSP_OS_ID_LEN * 2)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ���ȹ��� */
        if (g_atParaList[AT_C5GRSDQRY_OSID_APPID].paraLen > ((PS_URSP_OS_APP_ID_MAX_LEN + PS_URSP_OS_ID_LEN) * 2)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* os id�̶�16���ֽڣ�at�����·�ʱ��Ӧ���ַ�������Ϊ32 */
        if (At_AsciiString2HexSimple(tdPara->osIdAndOsAppId.osId.value, g_atParaList[AT_C5GRSDQRY_OSID_APPID].para,
                                     (VOS_UINT16)(PS_URSP_OS_ID_LEN * 2)) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        appIdLen = g_atParaList[AT_C5GRSDQRY_OSID_APPID].paraLen - PS_URSP_OS_ID_LEN * 2;

        if (At_AsciiString2HexSimple(tdPara->osIdAndOsAppId.osAppId.value,
            &g_atParaList[AT_C5GRSDQRY_OSID_APPID].para[PS_URSP_OS_ID_LEN * 2], (VOS_UINT16)appIdLen) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        tdPara->osIdAndOsAppId.osAppId.len = appIdLen / 2;

        tdPara->type |= PS_URSP_TD_OS_ID_AND_OS_APP_ID_TYPE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_ParseC5gRsdDnn(PS_URSP_Td *tdPara)
{
    VOS_UINT32 i;

    if (g_atParaList[AT_C5GRSDQRY_DNN].paraLen != 0) {
        if (g_atParaList[AT_C5GRSDQRY_DNN].paraLen > (PS_MAX_APN_LEN - 1)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        tdPara->type |= PS_URSP_TD_DNN_TYPE;
        tdPara->dnn.length = (VOS_UINT8)g_atParaList[AT_C5GRSDQRY_DNN].paraLen;

        for (i = 0; i < tdPara->dnn.length; i++) {
            tdPara->dnn.value[i] = AT_UPCASE(g_atParaList[AT_C5GRSDQRY_DNN].para[i]);
        }
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_ParseC5gRsdFqdn(PS_URSP_Td *tdPara)
{
    VOS_UINT32 i;

    if (g_atParaList[AT_C5GRSDQRY_FQDN].paraLen != 0) {
        if (g_atParaList[AT_C5GRSDQRY_FQDN].paraLen > PS_URSP_DEST_FQDN_MAX_LEN) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        tdPara->type |= PS_URSP_TD_FQDN_TYPE;

        for (i = 0; i < g_atParaList[AT_C5GRSDQRY_FQDN].paraLen; i++) {
            tdPara->fqdn.value[i] = AT_UPCASE(g_atParaList[AT_C5GRSDQRY_FQDN].para[i]);
        }
    }

    return AT_SUCCESS;
}

VOS_VOID AT_ParseC5gRsdConnectCapabilities(PS_URSP_Td *tdPara)
{
    if (g_atParaList[AT_C5GRSDQRY_CONN_CAP].paraLen != 0) {
        tdPara->connCap = (VOS_UINT8)g_atParaList[AT_C5GRSDQRY_CONN_CAP].paraValue;

        tdPara->type |= PS_URSP_TD_CONN_CAP_TYPE;
    }
}

VOS_UINT32 AT_ParseC5gRsdIpAddr(PS_URSP_Td *tdPara)
{
    /* ����<remote ipv4 address and mask> */
    if (g_atParaList[AT_C5GRSDQRY_REMOTE_IPV4_ADDR].paraLen != 0) {
        if (AT_Ipv4AddrAndMaskAtoi(g_atParaList[AT_C5GRSDQRY_REMOTE_IPV4_ADDR].para,
            g_atParaList[AT_C5GRSDQRY_REMOTE_IPV4_ADDR].paraLen, tdPara->ipTuple.rmtIpv4Addr.addr.data,
            tdPara->ipTuple.rmtIpv4Addr.mask.data) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        tdPara->ipTuple.opRmtIpv4Addr = VOS_TRUE;
    }

    /* ����<remote ipv6 address and prefix len> */
    if (g_atParaList[AT_C5GRSDQRY_REMOTE_IPV6_ADDR].paraLen != 0) {
        if (AT_Ipv6AddrAndPrefixLenAtoi(g_atParaList[AT_C5GRSDQRY_REMOTE_IPV6_ADDR].para,
            g_atParaList[AT_C5GRSDQRY_REMOTE_IPV6_ADDR].paraLen, tdPara->ipTuple.rmtIpv6Addr.addr.data,
            &tdPara->ipTuple.rmtIpv6Addr.prefixLen) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        tdPara->ipTuple.opRmtIpv6Addr = VOS_TRUE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_ParseC5gRsdRmtPortInfo(PS_URSP_Td *tdPara)
{
    /* ����<single remote port> */
    if (g_atParaList[AT_C5GRSDQRY_SINGLE_REMOTE_PORT].paraLen != 0) {
        tdPara->ipTuple.opSingleRmtPort = VOS_TRUE;
        tdPara->ipTuple.singleRmtPort = (VOS_UINT16)g_atParaList[AT_C5GRSDQRY_SINGLE_REMOTE_PORT].paraValue;
    }

    /* ����<remote port range> */
    if (g_atParaList[AT_C5GRSDQRY_REMOTE_PORT_RANGE].paraLen != 0) {
        if (AT_PortRangeAtoi(g_atParaList[AT_C5GRSDQRY_REMOTE_PORT_RANGE].para,
            g_atParaList[AT_C5GRSDQRY_REMOTE_PORT_RANGE].paraLen, &tdPara->ipTuple.rmtPortRange.lowLimit,
            &tdPara->ipTuple.rmtPortRange.highLimit) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        tdPara->ipTuple.opRmtPortRange = VOS_TRUE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_ParseC5gRsdIpTuple(PS_URSP_Td *tdPara)
{
    /* ����ip address */
    if (AT_ParseC5gRsdIpAddr(tdPara) != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<protocol id> */
    if (g_atParaList[AT_C5GRSDQRY_PROTOCOL_NUM].paraLen != 0) {
        tdPara->ipTuple.opProtocolId = VOS_TRUE;
        tdPara->ipTuple.protocolId = (VOS_UINT8)g_atParaList[AT_C5GRSDQRY_PROTOCOL_NUM].paraValue;
    }

    /* ����remote port info */
    if (AT_ParseC5gRsdRmtPortInfo(tdPara) != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<SPI> */
    if (g_atParaList[AT_C5GRSDQRY_SEC_PARA_IDX].paraLen != 0) {
        tdPara->ipTuple.opSecuParaIndex = VOS_TRUE;
        tdPara->ipTuple.secuParaIndex = g_atParaList[AT_C5GRSDQRY_SEC_PARA_IDX].paraValue;
    }

    /* ����<tos> */
    if (g_atParaList[AT_C5GRSDQRY_TYPE_OF_SRV].paraLen != 0) {
        if (AT_TosAndMaskAtoi(g_atParaList[AT_C5GRSDQRY_TYPE_OF_SRV].para,
            g_atParaList[AT_C5GRSDQRY_TYPE_OF_SRV].paraLen, &tdPara->ipTuple.tos.tos,
            &tdPara->ipTuple.tos.tosMask) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
        tdPara->ipTuple.opTos = VOS_TRUE;
    }

    /* ����<flow label> */
    if (g_atParaList[AT_C5GRSDQRY_FLOW_LABEL].paraLen != 0) {
        tdPara->ipTuple.opFlowLabelType = VOS_TRUE;
        tdPara->ipTuple.flowLabelType = g_atParaList[AT_C5GRSDQRY_FLOW_LABEL].paraValue;
    }

    /* ֻҪ����1��op���Ҫ��type������ */
    tdPara->type |= (PS_URSP_TD_IP_TUPLE_TYPE * (tdPara->ipTuple.opProtocolId | tdPara->ipTuple.opSingleRmtPort |
        tdPara->ipTuple.opSecuParaIndex | tdPara->ipTuple.opFlowLabelType | tdPara->ipTuple.opTos |
        tdPara->ipTuple.opRmtPortRange | tdPara->ipTuple.opRmtIpv4Addr | tdPara->ipTuple.opRmtIpv6Addr));

    return AT_SUCCESS;
}

VOS_UINT32 AT_ParseC5gRsdNonIpTuple(PS_URSP_Td *tdPara)
{
    /* ����<ethertype> */
    if (g_atParaList[AT_C5GRSDQRY_ETHERNET_TYPE].paraLen != 0) {
        tdPara->nonIpTuple.opEtherType = VOS_TRUE;
        tdPara->nonIpTuple.etherType = (VOS_UINT16)g_atParaList[AT_C5GRSDQRY_ETHERNET_TYPE].paraValue;
    }

    /* ����<desMac> */
    if (g_atParaList[AT_C5GRSDQRY_DES_MAC_ADDR].paraLen != 0) {
        if (AT_MacAddrAtoi((VOS_CHAR *)g_atParaList[AT_C5GRSDQRY_DES_MAC_ADDR].para,
            tdPara->nonIpTuple.dstMacAddr.data, PS_MAC_ADDR_LEN) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        tdPara->nonIpTuple.opDstMacAddr = VOS_TRUE;
    }

    /* ����<cTagVid> */
    if (g_atParaList[AT_C5GRSDQRY_CTAGVID].paraLen != 0) {
        tdPara->nonIpTuple.opCTagVid = VOS_TRUE;
        tdPara->nonIpTuple.cTagVid = (VOS_UINT16)g_atParaList[AT_C5GRSDQRY_CTAGVID].paraValue;
    }

    /* ����<sTagVid> */
    if (g_atParaList[AT_C5GRSDQRY_STAGVID].paraLen != 0) {
        tdPara->nonIpTuple.opSTagVid = VOS_TRUE;
        tdPara->nonIpTuple.sTagVid = (VOS_UINT16)g_atParaList[AT_C5GRSDQRY_STAGVID].paraValue;
    }

    /* ����<cTagPcpDei> */
    if (g_atParaList[AT_C5GRSDQRY_CTAGPCPDEI].paraLen != 0) {
        tdPara->nonIpTuple.opCTagPcpDei = VOS_TRUE;
        tdPara->nonIpTuple.cTagPcpDei = (VOS_UINT8)g_atParaList[AT_C5GRSDQRY_CTAGPCPDEI].paraValue;
    }

    /* ����<sTagPcpDei> */
    if (g_atParaList[AT_C5GRSDQRY_STAGPCPDEI].paraLen != 0) {
        tdPara->nonIpTuple.opSTagPcpDei = VOS_TRUE;
        tdPara->nonIpTuple.sTagPcpDei = (VOS_UINT8)g_atParaList[AT_C5GRSDQRY_STAGPCPDEI].paraValue;
    }

    tdPara->type |= (PS_URSP_TD_NON_IP_TUPLE_TYPE * (tdPara->nonIpTuple.opDstMacAddr | tdPara->nonIpTuple.opEtherType |
        tdPara->nonIpTuple.opCTagVid | tdPara->nonIpTuple.opSTagVid | tdPara->nonIpTuple.opCTagPcpDei |
        tdPara->nonIpTuple.opSTagPcpDei));

    return AT_SUCCESS;
}

VOS_UINT32 AT_SetC5gRsdPara(VOS_UINT8 indexNum)
{
    PS_URSP_Td tdPara;
    VOS_UINT16 exClientId;

    (VOS_VOID)memset_s(&tdPara, sizeof(tdPara), 0x00, sizeof(PS_URSP_Td));

    /* �������� */
    if (g_atParaIndex > AT_QRY_URSP_RSD_PARA_MAX_NUMBER) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �����Я������, Ĭ�ϲ�ѯdefault ursp rsd */
    if (g_atParaIndex == 0) {
        tdPara.matchAllFlg = PS_URSP_MATCH_ALL;
    } else {
        /* ����appid */
        if (AT_ParseC5gRsdAppId(&tdPara) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����os id && appid */
        if (AT_ParseC5gRsdOsIdAndAppId(&tdPara) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����dnn */
        if (AT_ParseC5gRsdDnn(&tdPara) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����fqdn */
        if (AT_ParseC5gRsdFqdn(&tdPara) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����connect capabilities */
        AT_ParseC5gRsdConnectCapabilities(&tdPara);

        /* ����IP��Ԫ�� */
        if (AT_ParseC5gRsdIpTuple(&tdPara) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����NON IPԪ�飬��Ҫ����̫������ */
        if (AT_ParseC5gRsdNonIpTuple(&tdPara) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    exClientId = AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId);
    /* ִ��������� */
    if (TAF_PS_Get5gUrspRsd(WUEPS_PID_AT, exClientId, 0, &tdPara) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_C5GRSDQRY_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

#endif

VOS_UINT32 AT_SetCsqlvlPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 result;

    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_CSQLVL_QRY_REQ, VOS_NULL_PTR, 0,
                                    I0_UEPS_PID_MTA);
    if (result == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSQLVL_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("AT_SetCsqlvlPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetCsqlvlExtPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 result;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_CSQLVL_QRY_REQ, VOS_NULL_PTR, 0,
                                    I0_UEPS_PID_MTA);

    if (result == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSQLVLEXT_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("AT_SetCsqlvlExtPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }
}

VOS_UINT32 At_GetNvRevertState(VOS_VOID)
{
    VOS_UINT32 ret;
    VOS_UINT16 resumeFlag = 0;

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ID_DRV_RESUME_FLAG, &resumeFlag, sizeof(VOS_UINT16)); /* to do */
    if (ret) {
        return ret;
    }
    if (resumeFlag != 0) {
        return ((VOS_UINT32)-1);
    }
    return 0;
}

// �Զ��ָ�״̬��ѯ
VOS_UINT32 atSetNVRstSTTS(VOS_UINT8 clientId)
{
    VOS_UINT32 rst;
    g_atSendDataBuff.bufLen = 0;

    rst = At_GetNvRevertState();
    if (rst != ERR_MSP_SUCCESS) {
        CmdErrProc(clientId, rst, 0, NULL);
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "\r%d\r", rst);

    return AT_OK;
}

VOS_UINT32 AT_SetApRptSrvUrlPara(VOS_UINT8 indexNum)
{
    errno_t   memResult;
    VOS_UINT8 apRptSrvUrl[AT_AP_XML_RPT_SRV_URL_LEN + 1];
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_ERROR;
    }

    /* URLΪ�ջ��߳��ȳ���127 */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[0].paraLen > AT_AP_XML_RPT_SRV_URL_LEN)) {
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(apRptSrvUrl, sizeof(apRptSrvUrl), 0x00, sizeof(apRptSrvUrl));
    memResult = memcpy_s(apRptSrvUrl, sizeof(apRptSrvUrl), g_atParaList[0].para, g_atParaList[0].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(apRptSrvUrl), g_atParaList[0].paraLen);

    /* дNV:NV_ITEM_AP_RPT_SRV_URL */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_AP_RPT_SRV_URL, apRptSrvUrl, AT_AP_XML_RPT_SRV_URL_LEN + 1) != NV_OK) {
        AT_ERR_LOG("AT_SetApRptSrvUrlPara:Write NV fail");
        return AT_ERROR;
    } else {
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "^APRPTSRVURLNTY");

        return AT_OK;
    }
}

VOS_UINT32 AT_SetApXmlInfoTypePara(VOS_UINT8 indexNum)
{
    VOS_UINT8 apXmlInfoType[AT_AP_XML_RPT_INFO_TYPE_LEN + 1];
    errno_t   memResult;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_ERROR;
    }

    /* INFO TYPEΪ�ջ��߳��ȳ���127 */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[0].paraLen > AT_AP_XML_RPT_INFO_TYPE_LEN)) {
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(apXmlInfoType, sizeof(apXmlInfoType), 0x00, sizeof(apXmlInfoType));
    memResult = memcpy_s(apXmlInfoType, sizeof(apXmlInfoType), g_atParaList[0].para, g_atParaList[0].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(apXmlInfoType), g_atParaList[0].paraLen);

    /* дNV:NV_ITEM_AP_XML_INFO_TYPE */
    if (NV_OK !=
        TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_AP_XML_INFO_TYPE, apXmlInfoType, AT_AP_XML_RPT_INFO_TYPE_LEN + 1)) {
        AT_ERR_LOG("AT_SetApXmlInfoTypePara:Write NV fail");
        return AT_ERROR;
    } else {
        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
            (TAF_CHAR *)g_atSndCodeAddress, "^APXMLINFOTYPENTY");

        return AT_OK;
    }
}

VOS_UINT32 AT_SetApXmlRptFlagPara(VOS_UINT8 indexNum)
{
    TAF_AT_NvimApXmlRptFlg apXmlRptFlg;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_ERROR;
    }

    apXmlRptFlg.apXmlRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;

    /* дNV:NV_ITEM_AP_XML_RPT_FLAG */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_AP_XML_RPT_FLAG, (VOS_UINT8 *)&apXmlRptFlg,
                           sizeof(TAF_AT_NvimApXmlRptFlg)) != NV_OK) {
        AT_ERR_LOG("AT_SetApXmlRptFlagPara:Write NV fail");
        return AT_ERROR;
    } else {
        return AT_OK;
    }
}

VOS_UINT32 AT_SetVertime(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_ERROR;
    }

    /* ������ϢDRV_AGENT_VERTIME_QRY_REQ��AT������ */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, DRV_AGENT_VERTIME_QRY_REQ, VOS_NULL_PTR, 0,
                                 I0_WUEPS_PID_DRV_AGENT);

    if (rst == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_VERSIONTIME_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetRrcStatPara(VOS_UINT8 indexNum)
{
    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    atCmd.u.rrcStatRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;
    atCmd.reqType         = AT_MTA_SET_RRCSTAT_RPT_TYPE;

    /* ��MTA����^RRCSTAT�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 AT_SetHfreqInfoPara(VOS_UINT8 indexNum)
{
    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    atCmd.u.hfreqInfoRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;
    atCmd.reqType           = AT_MTA_SET_HFREQINFO_RPT_TYPE;

    /* ��MTA����^HFREQINFO�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 At_SetHifiReset(VOS_UINT8 indexNum)
{
    APP_VC_MsgUint16          msgName;
    APP_VC_HifiResetTypeUint8 hifiResetType = APP_VC_HIFI_RESET_FROM_AP;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraValue == 0) {
        msgName = APP_VC_MSG_HIFI_RESET_BEGIN_NTF;
    } else {
        msgName = APP_VC_MSG_HIFI_RESET_END_NTF;
    }

    APP_VC_SendHifiResetNtf(I0_WUEPS_PID_VC, hifiResetType, msgName);

#if (MULTI_MODEM_NUMBER == 2)
    APP_VC_SendHifiResetNtf(I1_WUEPS_PID_VC, hifiResetType, msgName);
#endif

#if (MULTI_MODEM_NUMBER == 3)
    APP_VC_SendHifiResetNtf(I1_WUEPS_PID_VC, hifiResetType, msgName);
    APP_VC_SendHifiResetNtf(I2_WUEPS_PID_VC, hifiResetType, msgName);
#endif

    return AT_OK;
}

VOS_UINT32 AT_SetUeModeStatus(VOS_UINT8 indexNum)
{
    AT_MTA_UeModeStatusNtf atMtaUeModeStatusNtf;
    VOS_UINT32             result;

    (VOS_VOID)memset_s(&atMtaUeModeStatusNtf, sizeof(AT_MTA_UeModeStatusNtf), 0x00, sizeof(AT_MTA_UeModeStatusNtf));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex != AT_UE_MODE_STATUS_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    atMtaUeModeStatusNtf.modeStatus = (AT_MTA_UeModeStatusUint8)g_atParaList[0].paraValue;
    atMtaUeModeStatusNtf.modeFlag   = (VOS_UINT8)g_atParaList[1].paraValue;

    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UE_MODE_STATUS_NTF,
        (VOS_UINT8 *)&atMtaUeModeStatusNtf, sizeof(AT_MTA_UeModeStatusNtf), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetUeModeStatus: snd Fail");
        return AT_ERROR;
    }

    /* ��ap�ӿ�Լ��������Ҫ�ȴ����Ӧ�� */
    return AT_OK;
}

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_SetImsPdpCfg(VOS_UINT8 indexNum)
{
    TAF_IMS_PdpCfg imsPdpCfg;

    /* �������� */
    if (g_atParaIndex != AT_IMSPDPCFG_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&imsPdpCfg, sizeof(imsPdpCfg), 0x00, sizeof(imsPdpCfg));

    /* ������ֵ */
    imsPdpCfg.cid     = (VOS_UINT8)g_atParaList[0].paraValue;
    imsPdpCfg.imsFlag = (VOS_UINT8)g_atParaList[1].paraValue;

    /* ���Ϳ����Ϣ */
    if (TAF_PS_SetImsPdpCfg(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &imsPdpCfg) !=
        VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_IMSPDPCFG_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}
#endif

TAF_UINT32 At_SetpidPara(TAF_UINT8 indexNum)
{
    TAF_UINT32 rslt;
    TAF_UINT8  diagValue;

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[0].paraValue != 1) && (g_atParaList[0].paraValue != 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    diagValue = (VOS_UINT8)g_atParaList[0].paraValue;

    rslt = (VOS_UINT32)(DRV_SET_PID(diagValue));
    if (rslt != VOS_OK) {
        AT_WARN_LOG("At_SetpidPara:Setpid fail.");
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetRATCombinePara(VOS_UINT8 indexNum)
{
    return AT_CME_OPERATION_NOT_ALLOWED;
}

/* AT^TNUMCTL����������� */
VOS_UINT32 AT_SetTrustNumCtrlPara(VOS_UINT8 indexNum)
{
    AT_MtTrustListCtrl        *mtTrustCtrl = VOS_NULL_PTR;
    TAF_NVIM_MtListAndEidCtrl  nvimMtCtrl;
    VOS_UINT32                 result;

    /* �������� */
    if (g_atParaIndex > AT_SET_TRUST_NUM_CTRL_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0) ||
        (g_atParaList[2].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ȡȫ�ֱ�����Trust������������ */
    mtTrustCtrl = AT_GetCommMtTrustCtrlAddr();

    /* ��ȡNV�л�ȡMT��������Trust�������ƿ��� */
    (VOS_VOID)memset_s(&nvimMtCtrl, sizeof(TAF_NVIM_MtListAndEidCtrl), 0, sizeof(TAF_NVIM_MtListAndEidCtrl));

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_MT_LIST_AND_EID_CTRL, &nvimMtCtrl,
        sizeof(TAF_NVIM_MtListAndEidCtrl));

    if (result != NV_OK) {
        AT_ERR_LOG("AT_SetSmsTrustNumPara: Read NV fail.");
        return AT_ERROR;
    }

    /* ����Trust�����������õ�NV */
    nvimMtCtrl.mtSmsTrustListEnable  = (VOS_UINT8)g_atParaList[0].paraValue;
    nvimMtCtrl.mtCallTrustListEnable = (VOS_UINT8)g_atParaList[1].paraValue;

    result = TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_MT_LIST_AND_EID_CTRL, (VOS_UINT8 *)(&nvimMtCtrl),
        sizeof(TAF_NVIM_MtListAndEidCtrl));

    if (result != NV_OK) {
        /* дNVʧ�ܺ󣬲�����ȫ�ֱ���ֵ */
        AT_ERR_LOG("AT_SetTrustNumCtrlPara: Write NV fail.");
        return AT_ERROR;
    } else {
        /* дNV�ɹ��󣬸���ȫ�ֱ���ֵ */
        mtTrustCtrl->mtSmsTrustListEnable  = nvimMtCtrl.mtSmsTrustListEnable;
        mtTrustCtrl->mtCallTrustListEnable = nvimMtCtrl.mtCallTrustListEnable;
    }

    return AT_OK;
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
VOS_UINT32 AT_SetCccPara(TAF_UINT8 indexNum)
{
    VOS_UINT8  callId;
    VOS_UINT32 rst;

    if ((g_atParaIndex > AT_CCC_PARA_MAX_NUM) || (g_atParaIndex == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraValue == 0) {
        callId = 0; /* 0��ʾ��ѯ����call Id */
    } else {
        /* g_atParaList[1].ulParaValueȡֵ������ȡĬ��ֵ,��ȡCallId = 1�ĺ���״̬ */
        if (g_atParaList[1].paraLen == 0) {
            callId = 1;
        } else {
            callId = (VOS_UINT8)g_atParaList[1].paraValue;
        }
    }

    /* ������ϢAT_CC_MSG_STATE_QRY_REQ��CC��ȡCCЭ��״̬ */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, AT_CC_MSG_STATE_QRY_REQ, &callId,
                                 sizeof(VOS_UINT8), I0_WUEPS_PID_CC);

    if (rst == VOS_OK) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CC_STATE_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_UpdateImei(ModemIdUint16 modemId, VOS_UINT8 aucImei[], VOS_UINT16 imeiLength)
{
    VOS_UINT8  bcdNum[TAF_PH_IMEI_LEN];
    VOS_UINT8  checkData;
    VOS_UINT32 i;

    /* IMEI ���ȼ��: �û������IMEI����ӦΪ14λ��IMEI��1λ��CHECK NUM����15λ */
    if (imeiLength != (TAF_PH_IMEI_LEN - AT_IMEI_CHECK_NUM_LEN)) {
        AT_NORM_LOG1("AT_UpdateImei: the length of imei is error ", (VOS_INT32)imeiLength);
        return AT_PHYNUM_LENGTH_ERR;
    }

    /* IMEI ������: ȷ��<imei>Ϊ�����ַ���, ����ASCII��ת����BCD�� */
    if (At_AsciiNum2Num(bcdNum, aucImei, imeiLength) != AT_SUCCESS) {
        AT_NORM_LOG("AT_UpdateImei: the number of imei is error.");
        return AT_PHYNUM_NUMBER_ERR;
    }
    bcdNum[TAF_PH_IMEI_LEN - AT_IMEI_CHECK_NUM_LEN] = 0;

    /*
     * IMEI У����: IMEI��Ϣ��IMEI(TAC8λ,SNR6λ)��У��λ���������
     * �ο�Э��: 3GPP 23003 B.2 Computation of CD for an IMEI��B.3 Example of computation
     */
    checkData = 0;
    /* һ��ѭ��i��i+1������ѭ����2 */
    for (i = 0; i < (TAF_IMEI_DATA_LENGTH - AT_IMEI_CHECK_NUM_LEN); i += 2) {
        checkData += bcdNum[i] + ((bcdNum[i + 1UL] + bcdNum[i + 1UL]) / AT_DECIMAL_BASE_NUM) +
                     ((bcdNum[i + 1UL] + bcdNum[i + 1UL]) % AT_DECIMAL_BASE_NUM);
    }
    checkData = (AT_DECIMAL_BASE_NUM - (checkData % AT_DECIMAL_BASE_NUM)) % AT_DECIMAL_BASE_NUM;

    if (checkData != bcdNum[TAF_PH_IMEI_LEN - 2]) {
        AT_NORM_LOG("AT_UpdateImei: the checkdata of imei is error.");
        return AT_PHYNUM_NUMBER_ERR;
    }

    if (TAF_ACORE_NV_WRITE(modemId, en_NV_Item_IMEI, bcdNum, TAF_PH_IMEI_LEN) != NV_OK) {
        AT_WARN_LOG("AT_UpdateImei: Fail to write NV.");
        return AT_ERROR;
    }

    return AT_OK;
}

TAF_UINT32 At_SetImeiPara(TAF_UINT8 indexNum)
{
    VOS_BOOL      bPhyNumIsNull;
    VOS_UINT32    ret;
    ModemIdUint16 modemId = MODEM_ID_0;

    /* �������� */
    if (g_atParaIndex > 1) {
        AT_NORM_LOG1("At_SetImeiPara: the number of parameter is error ", (VOS_INT32)g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ret = AT_GetModemIdFromClient(indexNum, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("At_SetImeiPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* ����Ų�Ϊ�գ����ݱ���δ������ֱ�ӷ��ش��󣬴�����Ϊ1����Ų��Ϸ��� */
    if (AT_PhyNumIsNull(modemId, AT_PHYNUM_TYPE_IMEI, &bPhyNumIsNull) != AT_OK) {
        return AT_ERROR;
    }

    if ((bPhyNumIsNull == VOS_FALSE) && (g_atDataLocked == VOS_TRUE)) {
        AT_NORM_LOG("At_SetImeiPara: physical number is not null and data locked.");
        return AT_PHYNUM_TYPE_ERR;
    }

    /* �������ݵ�NV��en_NV_Item_IMEI�����ظ��½�� */
    ret = AT_UpdateImei(modemId, g_atParaList[0].para, g_atParaList[0].paraLen);

    return ret;
}
#endif

VOS_UINT32 AT_SetLogNvePara(VOS_UINT8 indexNum)
{
    return AT_OK;
}

VOS_UINT32 AT_SetHistoryFreqPara(VOS_UINT8 indexNum)
{
    AT_MTA_ClearHistoryFreqReq atCmd;
    VOS_UINT32                 result;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex > 1) {
        return AT_TOO_MANY_PARA;
    }

    if (g_atParaList[0].paraLen == 0) {
        AT_WARN_LOG("AT_SetHistoryFreqPara: WARNING:Input Len invalid!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));
    atCmd.mode = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ��MTA����^CHISFREQ�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_CLEAR_HISTORY_FREQ_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_ClearHistoryFreqReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CLEAR_HISTORY_FREQ;

    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_LTE == FEATURE_ON)
#if (FEATURE_LTE_MBMS == FEATURE_ON)

VOS_UINT32 AT_SetMBMSServiceOptPara(VOS_UINT8 indexNum)
{
    AT_MTA_MbmsServiceOptionSetReq mBMSServiceOption;
    VOS_UINT32                     rst;

    (VOS_VOID)memset_s(&mBMSServiceOption, sizeof(mBMSServiceOption), 0x00, sizeof(mBMSServiceOption));

    /* ����Ϊ�� */
    if (g_atParaList[1].paraLen != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    rst = atAuc2ul(g_atParaList[1].para, (VOS_UINT16)g_atParaList[1].paraLen, &g_atParaList[1].paraValue);

    if (rst != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switch (g_atParaList[1].paraValue) {
        case 0:
            mBMSServiceOption.cfg = AT_MTA_CFG_DISABLE;
            break;

        case 1:
            mBMSServiceOption.cfg = AT_MTA_CFG_ENABLE;
            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���Ϳ����Ϣ��C��, ����ʹ�ܻ���ȥʹ��MBMS�������� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_MBMS_SERVICE_OPTION_SET_REQ, &mBMSServiceOption, sizeof(mBMSServiceOption),
                                 I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetMBMSServicePara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MBMS_SERVICE_OPTION_SET;

    return AT_WAIT_ASYNC_RETURN;
}

/*
 * ��������: ��MBMSΪACT��DEACT����״̬�Ĵ���
 */
LOCAL VOS_UINT32 AT_ProcUnDeacAllMbmsServiceState(AT_MTA_MbmsServiceStateSetReq *mBMSServiceState)
{
    VOS_UINT32 rst = 0;

    /* ����Ϊ�� */
    if (g_atParaList[AT_MBMSCMD_PARAM1].paraLen == 0 || g_atParaList[AT_MBMSCMD_PARAM2].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    rst = atAuc2ul(g_atParaList[AT_MBMSCMD_PARAM1].para, (VOS_UINT16)g_atParaList[AT_MBMSCMD_PARAM1].paraLen,
                   &g_atParaList[AT_MBMSCMD_PARAM1].paraValue);

    if (rst != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������Ȳ���ȷ */
    if (((AT_MBMS_TMGI_MAX_LENGTH - 1) != g_atParaList[AT_MBMSCMD_PARAM2].paraLen) &&
        (g_atParaList[AT_MBMSCMD_PARAM2].paraLen != AT_MBMS_TMGI_MAX_LENGTH)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <AreaID> */
    mBMSServiceState->areaId = g_atParaList[AT_MBMSCMD_PARAM1].paraValue;

    /* <TMGI>:MBMS Service ID */
    if (At_Auc2ul(g_atParaList[AT_MBMSCMD_PARAM2].para, AT_MBMS_SERVICE_ID_LENGTH,
                  &mBMSServiceState->tmgi.mbmsSerId) == AT_FAILURE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <TMGI>:Mcc */
    if (At_String2Hex(&g_atParaList[AT_MBMSCMD_PARAM2].para[AT_MBMS_SERVICE_ID_LENGTH], AT_MBMS_MCC_LENGTH,
                      &mBMSServiceState->tmgi.plmnId.mcc) == AT_FAILURE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <TMGI>:Mnc */
    if (At_String2Hex(&g_atParaList[AT_MBMSCMD_PARAM2].para[AT_MBMS_SERVICE_ID_AND_MCC_LENGTH],
                      g_atParaList[AT_MBMSCMD_PARAM2].paraLen - AT_MBMS_SERVICE_ID_AND_MCC_LENGTH,
                      &mBMSServiceState->tmgi.plmnId.mnc) == AT_FAILURE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((AT_MBMS_TMGI_MAX_LENGTH - 1) == g_atParaList[AT_MBMSCMD_PARAM2].paraLen) {
        mBMSServiceState->tmgi.plmnId.mnc |= 0x0F00;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_SetMBMSServiceStatePara(VOS_UINT8 indexNum)
{
    AT_MTA_MbmsServiceStateSetReq mBMSServiceState;
    VOS_UINT32                    rst;

    (VOS_VOID)memset_s(&mBMSServiceState, sizeof(mBMSServiceState), 0x00, sizeof(mBMSServiceState));

    switch (g_atParaList[0].paraValue) {
        case AT_MBMS_ACTIVATE_TYPE:
            mBMSServiceState.stateSet = AT_MTA_MBMS_SERVICE_STATE_SET_ACTIVE;
            break;

        case AT_MBMS_DEACTIVATE_TYPE:
            mBMSServiceState.stateSet = AT_MTA_MBMS_SERVICE_STATE_SET_DEACTIVE;
            break;

        case AT_MBMS_DEACTIVATE_ALL_TYPE:
            mBMSServiceState.stateSet = AT_MTA_MBMS_SERVICE_STATE_SET_DEACTIVE_ALL;
            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    if (mBMSServiceState.stateSet != AT_MTA_MBMS_SERVICE_STATE_SET_DEACTIVE_ALL) {
        if (AT_ProcUnDeacAllMbmsServiceState(&mBMSServiceState) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    /* ���Ϳ����Ϣ��C��, ����ʹ�ܻ���ȥʹ��MBMS�������� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_MBMS_SERVICE_STATE_SET_REQ, &mBMSServiceState, sizeof(mBMSServiceState),
                                 I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetMBMSServiceStatePara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MBMS_SERVICE_STATE_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetMBMSPreferencePara(VOS_UINT8 indexNum)
{
    AT_MTA_MbmsPreferenceSetReq mBMSCastMode;
    VOS_UINT32                  rst;

    (VOS_VOID)memset_s(&mBMSCastMode, sizeof(mBMSCastMode), 0x00, sizeof(mBMSCastMode));

    /* ����Ϊ�� */
    if (g_atParaList[1].paraLen != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    rst = atAuc2ul(g_atParaList[1].para, (VOS_UINT16)g_atParaList[1].paraLen, &g_atParaList[1].paraValue);

    if (rst != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switch (g_atParaList[1].paraValue) {
        case 0:
            mBMSCastMode.castMode = AT_MTA_MBMS_CAST_MODE_UNICAST;
            break;

        case 1:
            mBMSCastMode.castMode = AT_MTA_MBMS_CAST_MODE_MULTICAST;
            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���Ϳ����Ϣ��C��, ����MBMS�㲥ģʽ */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_MBMS_PREFERENCE_SET_REQ, &mBMSCastMode, sizeof(mBMSCastMode),
                                 I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetMBMSPreferencePara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MBMS_PREFERENCE_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryMBMSSib16NetworkTimePara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ���Ϳ����Ϣ��C��, ��ѯSIB16����ʱ������ */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_SIB16_NETWORK_TIME_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryMBMSSib16NetworkTimePara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MBMS_SIB16_NETWORK_TIME_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryMBMSBssiSignalLevelPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ���Ϳ����Ϣ��C��, ��ѯBSSI�ź�ǿ������ */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_BSSI_SIGNAL_LEVEL_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryMBMSBssiSignalLevelPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MBMS_BSSI_SIGNAL_LEVEL_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryMBMSNetworkInfoPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ���Ϳ����Ϣ��C��, ��ѯ������Ϣ���� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_NETWORK_INFO_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryMBMSNetworkInfoPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MBMS_NETWORK_INFO_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_QryMBMSModemStatusPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ���Ϳ����Ϣ��C��, ��ѯeMBMS����״̬���� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_EMBMS_STATUS_QRY_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_QryMBMSModemStatusPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EMBMS_STATUS_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetMBMSCMDPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_MBMSCMD_MAX_PARA_NUM) {
        return AT_TOO_MANY_PARA;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<cmd> */
    switch (g_atParaList[0].paraValue) {
        /* <cmd> equal "MBMS_SERVICE_ENABLER" */
        case AT_MBMS_SERVICE_ENABLER_TYPE:
            rst = AT_SetMBMSServiceOptPara(indexNum);
            break;
        /* <cmd> equal "ACTIVATE", "DEACTIVATE" or "DEACTIVATE_ALL" */
        case AT_MBMS_ACTIVATE_TYPE:
        case AT_MBMS_DEACTIVATE_TYPE:
        case AT_MBMS_DEACTIVATE_ALL_TYPE:
            rst = AT_SetMBMSServiceStatePara(indexNum);
            break;
        /* <cmd> equal "MBMS_PREFERENCE" */
        case AT_MBMS_PREFERENCE_TYPE:
            rst = AT_SetMBMSPreferencePara(indexNum);
            break;
        /* <cmd> equal "SIB16_GET_NETWORK_TIME" */
        case AT_MBMS_SIB16_GET_NETWORK_TIME_TYPE:
            rst = AT_QryMBMSSib16NetworkTimePara(indexNum);
            break;
        /* <cmd> equal "BSSI_SIGNAL_LEVEL" */
        case AT_MBMS_BSSI_SIGNAL_LEVEL_TYPE:
            rst = AT_QryMBMSBssiSignalLevelPara(indexNum);
            break;
        /* <cmd> equal "NETWORK_INFORMATION" */
        case AT_MBMS_NETWORK_INFORMATION_TYPE:
            rst = AT_QryMBMSNetworkInfoPara(indexNum);
            break;
        /* <cmd> equal "MODEM_STATUS" */
        case AT_MBMS_MODEM_STATUS_TYPE:
            rst = AT_QryMBMSModemStatusPara(indexNum);
            break;

        default:
            rst = AT_CME_INCORRECT_PARAMETERS;
            break;
    }

    return rst;
}

VOS_UINT32 AT_SetMBMSEVPara(VOS_UINT8 indexNum)
{
    AT_MTA_MbmsUnsolicitedCfgSetReq mBMSUnsolicitedCfg;
    VOS_UINT32                      rst;

    (VOS_VOID)memset_s(&mBMSUnsolicitedCfg, sizeof(mBMSUnsolicitedCfg), 0x00, sizeof(mBMSUnsolicitedCfg));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ȷ */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switch (g_atParaList[0].paraValue) {
        case 0:
            mBMSUnsolicitedCfg.cfg = AT_MTA_CFG_DISABLE;
            break;

        case 1:
            mBMSUnsolicitedCfg.cfg = AT_MTA_CFG_ENABLE;
            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���Ϳ����Ϣ��C��, ����MBMS�����ϱ����� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_MBMS_UNSOLICITED_CFG_SET_REQ, &mBMSUnsolicitedCfg,
                                 sizeof(mBMSUnsolicitedCfg), I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetMBMSEVPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MBMS_UNSOLICITED_CFG_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetMBMSInterestListPara(VOS_UINT8 indexNum)
{
    AT_MTA_MbmsInterestlistSetReq mBMSInterestList;
    VOS_UINT32                    rst;
    VOS_UINT8                     interestNum;

    (VOS_VOID)memset_s(&mBMSInterestList, sizeof(mBMSInterestList), 0x00, sizeof(mBMSInterestList));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ȷ */
    if (g_atParaIndex != AT_MBMSINTERESTLIST_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[AT_MBMSINTERESTLIST_MBMS_PRIORITY].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switch (g_atParaList[AT_MBMSINTERESTLIST_MBMS_PRIORITY].paraValue) {
        case AT_MBMS_PRIORITY_UNICAST:
            mBMSInterestList.mbmsPriority = AT_MTA_MBMS_PRIORITY_UNICAST;
            break;

        case AT_MBMS_PRIORITY_MBMS:
            mBMSInterestList.mbmsPriority = AT_MTA_MBMS_PRIORITY_MBMS;
            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    /* Ƶ���б�ֵ */
    for (interestNum = 0; interestNum < AT_MTA_INTEREST_FREQ_MAX_NUM; interestNum++) {
        if (g_atParaList[interestNum].paraLen != 0) {
            rst = atAuc2ul(g_atParaList[interestNum].para, (VOS_UINT16)g_atParaList[interestNum].paraLen,
                           &g_atParaList[interestNum].paraValue);

            if (rst != AT_SUCCESS) {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            mBMSInterestList.freqList[interestNum] = g_atParaList[interestNum].paraValue;
        } else {
            /* Ĭ��ֵΪ��Чֵ0xFFFFFFFF */
            mBMSInterestList.freqList[interestNum] = 0xFFFFFFFF;
        }
    }

    /* ���Ϳ����Ϣ��C��, ����MBMS�����ϱ����� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_INTEREST_LIST_SET_REQ, &mBMSInterestList, sizeof(mBMSInterestList),
                                 I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetMBMSInterestListPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MBMS_INTERESTLIST_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_SetLteLowPowerPara(VOS_UINT8 indexNum)
{
    AT_MTA_LowPowerConsumptionSetReq powerConsumption;
    VOS_UINT32                       rst;

    (VOS_VOID)memset_s(&powerConsumption, sizeof(powerConsumption), 0x00, sizeof(powerConsumption));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ȷ */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switch (g_atParaList[0].paraValue) {
        case 0:
            powerConsumption.lteLowPowerFlg = AT_MTA_LTE_LOW_POWER_NORMAL;
            break;

        case 1:
            powerConsumption.lteLowPowerFlg = AT_MTA_LTE_LOW_POWER_LOW;
            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���Ϳ����Ϣ��C��, ���õ͹��� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_LTE_LOW_POWER_SET_REQ, (VOS_UINT8 *)&powerConsumption,
                                 sizeof(powerConsumption), I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetLteLowPowerPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LTE_LOW_POWER_CONSUMPTION_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

#if (FEATURE_LTE == FEATURE_ON)
VOS_UINT32 AT_SetRsrpCfgPara(VOS_UINT8 indexNum)
{
    TAF_NVIM_LteRsrpCfg rsrpCfg;
    VOS_UINT32   rsrpLevel;

    (VOS_VOID)memset_s(&rsrpCfg, sizeof(rsrpCfg), 0x00, sizeof(rsrpCfg));

    /* ������� */
    if ((g_atParaList[AT_RSRPCFG_LEVEL].paraLen == 0) || (g_atParaList[AT_RSRPCFG_VALUE].paraLen == 0) ||
        (g_atParaIndex != AT_RSRPCFG_PARA_VALID_NUM)) {
        return AT_ERROR;
    }

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_TAF_LTE_RSRP_CFG, &rsrpCfg, sizeof(rsrpCfg)) != NV_OK) {
        PS_PRINTF_WARNING("<AT_SetRsrpCfgPara> WARNING:TAF_ACORE_NV_READ RSRP faild!\n");
        return AT_ERROR;
    }

    /* ��������ֵ����NV�� */
    rsrpLevel = g_atParaList[0].paraValue;
    rsrpCfg.level[rsrpLevel] = (VOS_INT16)(g_atParaList[AT_RSRPCFG_LEVEL].paraValue);
    rsrpCfg.value[rsrpLevel] = (VOS_INT16)(g_atParaList[AT_RSRPCFG_VALUE].paraValue);
    if (g_ate5RightFlag == AT_E5_RIGHT_FLAG_YES) {
        if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_TAF_LTE_RSRP_CFG, (VOS_UINT8 *)&rsrpCfg, sizeof(rsrpCfg)) != NV_OK) {
            PS_PRINTF_WARNING("<AT_SetRsrpCfgPara> WARNING:TAF_ACORE_NV_WRITE RSRP faild!\n");
            return AT_ERROR;
        }
    } else {
        return AT_ERROR; /* ���Ȩ��δ�򿪣�����ERROR */
    }

    return AT_OK;
}
#endif

#if (FEATURE_LTE == FEATURE_ON)
VOS_UINT32 AT_SetRscpCfgPara(VOS_UINT8 indexNum)
{
    TAF_NVIM_LteRscpCfg rscpCfg;
    VOS_UINT32   rscpLevel;

    (VOS_VOID)memset_s(&rscpCfg, sizeof(rscpCfg), 0x00, sizeof(rscpCfg));

    /* ������� */
    if ((g_atParaList[AT_RSCPCFG_LEVEL].paraLen == 0) || (g_atParaList[AT_RSCPCFG_VALUE].paraLen == 0) ||
        (g_atParaIndex != AT_RSCPCFG_PARA_VALID_NUM)) {
        PS_PRINTF_WARNING("RSCP para error\n");
        return AT_ERROR;
    }

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_TAF_LTE_RSCP_CFG, &rscpCfg, sizeof(rscpCfg)) != NV_OK) {
        PS_PRINTF_WARNING("RSCP read error!\n");
        return AT_ERROR;
    }

    /* ��������ֵ����NV�� */
    rscpLevel = g_atParaList[AT_RSCPCFG_LEVEL].paraValue;
    rscpCfg.level[rscpLevel] = (VOS_INT16)(g_atParaList[AT_RSCPCFG_LEVEL].paraValue);
    rscpCfg.value[rscpLevel] = (VOS_INT16)(g_atParaList[AT_RSCPCFG_VALUE].paraValue);
    if (g_ate5RightFlag == AT_E5_RIGHT_FLAG_YES) {
        if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_TAF_LTE_RSCP_CFG, (VOS_UINT8 *)&rscpCfg, sizeof(rscpCfg)) != NV_OK) {
            PS_PRINTF_WARNING("RSCP NV write error\n");
            return AT_ERROR;
        }
    } else {
        PS_PRINTF_WARNING("RSCP write limit\n");
        return AT_ERROR; /* ���Ȩ��δ�򿪣�����ERROR */
    }

    return AT_OK;
}
#endif

#if (FEATURE_LTE == FEATURE_ON)
VOS_UINT32 AT_SetEcioCfgPara(VOS_UINT8 indexNum)
{
    TAF_NVIM_LteEcioCfg ecioCfg;
    VOS_UINT32   ecioLevel;

    (VOS_VOID)memset_s(&ecioCfg, sizeof(ecioCfg), 0x00, sizeof(ecioCfg));

    /* ������� */
    if ((g_atParaList[AT_ECIOCFG_LEVEL].paraLen == 0) || (g_atParaList[AT_ECIOCFG_VALUE].paraLen == 0) ||
        (g_atParaIndex != AT_ECIOCFG_PARA_VALID_NUM)) {
        PS_PRINTF_WARNING("ECIO para error\n");
        return AT_ERROR;
    }

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_TAF_LTE_ECIO_CFG, &ecioCfg, sizeof(ecioCfg)) != NV_OK) {
        PS_PRINTF_WARNING("ECIO read error\n");
        return AT_ERROR;
    }

    /* ��������ֵ����NV�� */
    ecioLevel = g_atParaList[AT_ECIOCFG_LEVEL].paraValue;
    ecioCfg.level[ecioLevel] = (VOS_INT16)(g_atParaList[AT_ECIOCFG_LEVEL].paraValue);
    ecioCfg.value[ecioLevel] = (VOS_INT16)(g_atParaList[AT_ECIOCFG_VALUE].paraValue);
    if (g_ate5RightFlag == AT_E5_RIGHT_FLAG_YES) {
        if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_TAF_LTE_ECIO_CFG, (VOS_UINT8 *)&ecioCfg, sizeof(ecioCfg)) != NV_OK) {
            PS_PRINTF_WARNING("ECIO write error\n");
            return AT_ERROR;
        }
    } else {
        PS_PRINTF_WARNING("ECIO write limit\n");
        return AT_ERROR; /* ���Ȩ��δ�򿪣�����ERROR */
    }

    return AT_OK;
}

VOS_UINT32 AT_SetFrStatus(VOS_UINT8 indexNum)
{
    VOS_UINT32         rst = TAF_FAILURE;
    AT_MTA_SetFrReq    atMtaSetFrReq;
    PS_BOOL_ENUM_UINT8 actFrFlag = PS_BOOL_BUTT;

    /* �ֲ�������ʼ�� */
    (VOS_VOID)memset_s(&atMtaSetFrReq, sizeof(atMtaSetFrReq), 0x00, sizeof(AT_MTA_SetFrReq));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* ����������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* enActFrFlagȡֵ��ΧΪ'0'~'1' */
    if (((g_atParaList[0].paraValue) != 0) && ((g_atParaList[0].paraValue) != 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    actFrFlag = (PS_BOOL_ENUM_UINT8)(g_atParaList[0].paraValue);

    /* ���ṹ�� */
    atMtaSetFrReq.actFrFlag = actFrFlag;
    atMtaSetFrReq.rsv[0]    = 0;
    atMtaSetFrReq.rsv[1]    = 0;
    atMtaSetFrReq.rsv[2]    = 0;

    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, ID_AT_MTA_SET_FR_REQ,
                                 (VOS_UINT8 *)&atMtaSetFrReq, sizeof(AT_MTA_SetFrReq), I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FRSTATUS_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif





