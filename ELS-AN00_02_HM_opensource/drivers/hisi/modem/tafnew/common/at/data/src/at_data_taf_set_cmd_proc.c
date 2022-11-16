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

#include "at_data_taf_set_cmd_proc.h"
#include "securec.h"
#include "at_data_comm.h"
#include "at_data_proc.h"

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
#include "nv_stru_lps.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_DATA_TAF_SET_CMD_PROC_C

#define AT_C5GQOS_MAX_PARA_NUM 6
#define AT_C5GQOS_CID 0
#define AT_C5GQOS_5QI 1
#define AT_C5GQOS_DL_GFBR 2
#define AT_C5GQOS_UL_GFBR 3
#define AT_C5GQOS_DL_MFBR 4
#define AT_C5GQOS_UL_MFBR 5
#define AT_CSUEPOLICY_PTI 0
#define AT_CSUEPOLICY_MSG_TYPE 1
#define AT_CSUEPOLICY_INFO_LEN 2
#define AT_CSUEPOLICY_CLASS_MARK 3
#define AT_CSUEPOLICY_OS_ID_INFORMATION 4
#define AT_CSUEPOLICY_PROTOCAL_VERSION 5
#define AT_CSUEPOLICY_MAX_PARA_NUM 6
#define AT_CSUEPOLICY_MIN_PARA_NUM 2
#define AT_CGEREP_PARA_MAX_NUM 2
#define AT_CSND_PARA_MAX_NUM 2
#define AT_CSND_PLAYLOAD_MAX_LEN 1400
#define AT_APDSFLOWRPTCFG_PARA_MAX_NUM 4
#define AT_APDSFLOWRPTCFG_PARA_MIN_NUM 1
#define AT_DSFLOWNVWRCFG_PARA_MAX_NUM 2
#define AT_UDP_SRC_PORT 9700
#define AT_CGPIAF_MAX_PARA_NUM 4
#define AT_CGPIAF_IPV6_ADDRFORMAT 0
#define AT_CGPIAF_IPV6_SUBNETNOTATION 1
#define AT_CGPIAF_IPV6_LEADINGZEROS 2
#define AT_CGPIAF_IPV6_COMPRESSZEROS 3
#define AT_AUTHDATA_PARA_MAX_NUM 5
#define AT_AUTHDATA_PLMN 2
#define AT_AUTHDATA_PASSWD_INDEX 3
#define AT_AUTHDATA_USERNAME_INDEX 4
#define AT_CHDATA_PARA_MAX_NUM 2
#define AT_TRIG_NSAPI 1
#define AT_TRIG_LENGTH 2
#define AT_TRIG_TIMES 3
#define AT_TRIG_MILLISECOND 4
#define AT_TRIG_PARA_VALID_NUM 5
#define AT_TRIG_NSAPI_MIN_VALUE 5
#define AT_TRIG_NSAPI_MAX_VALUE 15
#define AT_FASTDORM_PARA_MAX_NUM 2
#define AT_CGDATA_PARA_MAX_NUM 2
#define AT_CGDSCONT_MAX_PARA_NUM 5
#define AT_CGACT_MAX_PARA_NUM 2
#define AT_CGANS_MAX_PARA_NUM 3
#define AT_CGDNS_MAX_PARA_NUM 3
#define AT_PDP_TYPE_IPV4 0
#define AT_PDP_TYPE_IPV6 1
#define AT_PDP_TYPE_IPV4V6 2
#define AT_PDP_TYPE_PPP 3
#define AT_PDP_TYPE_ETHERNET 4
#define AT_CGTFT_PACKET_FILTER_ID 1
#define AT_CGTFT_PRECEDENCE 2
#define AT_CGTFT_PROTOCOL_ID 4
#define AT_CGTFT_DIRECTION 10
#define AT_CGTFT_QRI 12
#define AT_CGTFT_FLOW_LABLE 9
#define AT_CGTFT_SECU_PARA_INDEX 7
#define AT_CGTFT_TYPE_OF_SERVICE 8
#define AT_CGTFT_DESTINATION_PORT_RANGE 5
#define AT_CGTFT_SOURCE_PORT_RANGE 6
#define AT_CGEQREQ_MAX_PARA_NUM 15
#define AT_CGDCONT_D_COMP 4
#define AT_CGDCONT_H_COMP 5
#define AT_CGDCONT_IPV4_ADDR_ALLOC 6
#define AT_CGDCONT_EMERGENCY_INDICATION 7
#define AT_CGDCONT_P_CSCF_DISCOVERY 8
#define AT_CGDCONT_IM_CN_SIGNALLING_FLAG_IND 9
#define AT_CGDCONT_NAS_SIGNALLING_PRIO_IND 10
#define AT_CGDCONT_SSC_MODE 16
#define AT_CGDCONT_PREF_ACCESS_TYPE 18
#define AT_CGDCONT_RQOS_IND 19
#define AT_CGDCONT_ALWAYS_ON_IND 21
#define AT_CGDCONT_APN 2
#define AT_CGDCONT_PDP_ADDRESS 3
#define AT_CGDCONT_S_NSSAI 17
#define AT_CGDCONT_MH6_PDU 20
#define AT_CGDCONT_PDP_D_COMP 2
#define AT_CGDCONT_PDP_H_COMP 3
#define AT_CGDCONT_IM_CN_SIGNAL_FLAG 4
#define AT_CGEQREQ_MAXIMUM_BITRATE_UL 2
#define AT_CGEQREQ_MAXIMUM_BITRATE_DL 3
#define AT_CGEQREQ_GUARANTEED_BITRATE_UL 4
#define AT_CGEQREQ_GUARANTEED_BITRATE_DL 5
#define AT_CGEQREQ_DELIVERY_ORDER 6
#define AT_CGEQREQ_MAXIMUM_SDU_SIZE 7
#define AT_CGEQREQ_SDU_ERROR_RATIO 8
#define AT_CGEQREQ_RESIDUAL_BIT_ERROR_RATIO 9
#define AT_CGEQREQ_DELIVERY_OF_ERR_SDUS 10
#define AT_CGEQREQ_TRANSFER_DELAY 11
#define AT_CGEQREQ_TRAFF_HANDLE_PRIOR 12
#define AT_CGEQREQ_SRC_STATISTICS_DESCRIPTOR 13
#define AT_CGEQREQ_SIGNALLING_INDICATION 14
#define AT_CGANS_CID 2
#define AT_CGTFT_SRC_ADDR_AND_SUBNET_MASK 3
#define AT_CGTFT_LOCAL_ADDR_AND_SUBNET_MASK 11
#define AT_CGDNS_SECDNS 2
#define AT_PDP_AUTH_TYPE_NONE 0
#define AT_PDP_AUTH_TYPE_PAP 1
#define AT_PDP_AUTH_TYPE_CHAP 2
#define AT_PDP_AUTH_TYPE_PAP_OR_CHAP 3
#define AT_ROAMPDPTYPEPARA_PARA_MAX_NUM 2
#define AT_USBTETHERINFO_PARA_NUM 2
#define AT_RMNETCFG_PARA_NUM 3
#define AT_EMC_PDN_STAT_PAPA_VALUE 1
#define AT_PS_CALL_RPT_CMD_PARA_NUM 2
#define AT_PS_CALL_RPT_CMD_MODE_PARA 0
#define AT_PS_CALL_RPT_CMD_TYPE_PARA 1
#define IPV6_ADDR_MAX_VALUE 255
#define AT_PHONE_DIAL_MAX_PARA_NUM 8
#define AT_WEBUI_DIAL_MAX_PARA_NUM 7
#define AT_NDIS_DIAL_MAX_PARA_NUM 7
#define AT_PS_DIAL_PARAM_APN 2
#define AT_PS_DIAL_PARAM_IP_ADDR 6
#define AT_PS_DIAL_PARAM_USER_NAME 3
#define AT_PS_DIAL_PARAM_PASS_WORD 4
#define AT_PS_DIAL_PARAM_RAT_TYPE 7
#define AT_PS_USR_DIAL_AUTH_TYPE 5
#define AT_IP_HEAD_LEN 5

VOS_BOOL AT_CheckCgeqreqPara(VOS_VOID)
{
    if (g_atParaList[0].paraLen == 0) {
        return VOS_FALSE;
    }

    /* �������� */
    if (g_atParaIndex > AT_CGEQREQ_MAX_PARA_NUM) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

VOS_VOID AT_SetCgeqreqParaTrafficClass(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[1].paraLen != 0) {
        umtsQosInfo->opTrafficClass = VOS_TRUE;
        umtsQosInfo->trafficClass   = (VOS_UINT8)g_atParaList[1].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaMaxBitUl(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_MAXIMUM_BITRATE_UL].paraLen != 0) {
        umtsQosInfo->opMaxBitUl = VOS_TRUE;
        umtsQosInfo->maxBitUl   = g_atParaList[AT_CGEQREQ_MAXIMUM_BITRATE_UL].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaMaxBitDl(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_MAXIMUM_BITRATE_DL].paraLen != 0) {
        umtsQosInfo->opMaxBitDl = VOS_TRUE;
        umtsQosInfo->maxBitDl   = g_atParaList[AT_CGEQREQ_MAXIMUM_BITRATE_DL].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaGtdBitUl(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_GUARANTEED_BITRATE_UL].paraLen != 0) {
        umtsQosInfo->opGtdBitUl = VOS_TRUE;
        umtsQosInfo->gtdBitUl   = g_atParaList[AT_CGEQREQ_GUARANTEED_BITRATE_UL].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaGtdBitDl(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_GUARANTEED_BITRATE_DL].paraLen != 0) {
        umtsQosInfo->opGtdBitDl = VOS_TRUE;
        umtsQosInfo->gtdBitDl   = g_atParaList[AT_CGEQREQ_GUARANTEED_BITRATE_DL].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaDeliverOrder(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_DELIVERY_ORDER].paraLen != 0) {
        umtsQosInfo->opDeliverOrder = VOS_TRUE;
        umtsQosInfo->deliverOrder   = (VOS_UINT8)g_atParaList[AT_CGEQREQ_DELIVERY_ORDER].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaMaxSduSize(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_MAXIMUM_SDU_SIZE].paraLen != 0) {
        umtsQosInfo->opMaxSduSize = VOS_TRUE;
        umtsQosInfo->maxSduSize   = (VOS_UINT16)g_atParaList[AT_CGEQREQ_MAXIMUM_SDU_SIZE].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaSduErrRatio(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_SDU_ERROR_RATIO].paraLen != 0) {
        umtsQosInfo->opSduErrRatio = VOS_TRUE;
        umtsQosInfo->sduErrRatio   = (VOS_UINT8)g_atParaList[AT_CGEQREQ_SDU_ERROR_RATIO].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaResidualBer(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_RESIDUAL_BIT_ERROR_RATIO].paraLen != 0) {
        umtsQosInfo->opResidualBer = VOS_TRUE;
        umtsQosInfo->residualBer   = (VOS_UINT8)g_atParaList[AT_CGEQREQ_RESIDUAL_BIT_ERROR_RATIO].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaDeliverErrSdu(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_DELIVERY_OF_ERR_SDUS].paraLen != 0) {
        umtsQosInfo->opDeliverErrSdu = VOS_TRUE;
        umtsQosInfo->deliverErrSdu   = (VOS_UINT8)g_atParaList[AT_CGEQREQ_DELIVERY_OF_ERR_SDUS].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaTransDelay(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_TRANSFER_DELAY].paraLen != 0) {
        umtsQosInfo->opTransDelay = VOS_TRUE;
        umtsQosInfo->transDelay   = (VOS_UINT16)g_atParaList[AT_CGEQREQ_TRANSFER_DELAY].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaTraffHandlePrior(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_TRAFF_HANDLE_PRIOR].paraLen != 0) {
        umtsQosInfo->opTraffHandlePrior = VOS_TRUE;
        umtsQosInfo->traffHandlePrior   = (VOS_UINT8)g_atParaList[AT_CGEQREQ_TRAFF_HANDLE_PRIOR].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaSrcStatisticsDes(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_SRC_STATISTICS_DESCRIPTOR].paraLen != 0) {
        umtsQosInfo->opSrcStatisticsDes = VOS_TRUE;
        umtsQosInfo->srcStatisticsDes   = (VOS_UINT8)g_atParaList[AT_CGEQREQ_SRC_STATISTICS_DESCRIPTOR].paraValue;
    }
}

VOS_VOID AT_SetCgeqreqParaSignalIndication(TAF_UMTS_QosExt *umtsQosInfo)
{
    if (g_atParaList[AT_CGEQREQ_SIGNALLING_INDICATION].paraLen != 0) {
        umtsQosInfo->opSignalIndication = VOS_TRUE;
        umtsQosInfo->signalIndication   = (VOS_UINT8)g_atParaList[AT_CGEQREQ_SIGNALLING_INDICATION].paraValue;
    }
}

TAF_UINT32 AT_SetCgeqreqPara(TAF_UINT8 indexNum)
{
    TAF_UMTS_QosExt umtsQosInfo;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&umtsQosInfo, sizeof(umtsQosInfo), 0x00, sizeof(TAF_UMTS_QosExt));

    /* ������� */
    if (AT_CheckCgeqreqPara() != VOS_TRUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<cid> */
    umtsQosInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* �������ֻ��<CID>, �������������� */
    if (g_atParaIndex == 1) {
        umtsQosInfo.defined = VOS_FALSE;
    } else {
        umtsQosInfo.defined = VOS_TRUE;

        /* ����<Traffic class> */
        AT_SetCgeqreqParaTrafficClass(&umtsQosInfo);

        /* ����<Maximum bitrate UL> */
        AT_SetCgeqreqParaMaxBitUl(&umtsQosInfo);

        /* ����<Maximum bitrate DL> */
        AT_SetCgeqreqParaMaxBitDl(&umtsQosInfo);

        /* ����<Guaranteed bitrate UL> */
        AT_SetCgeqreqParaGtdBitUl(&umtsQosInfo);

        /* ����<Guaranteed bitrate DL> */
        AT_SetCgeqreqParaGtdBitDl(&umtsQosInfo);

        /* ����<Delivery order> */
        AT_SetCgeqreqParaDeliverOrder(&umtsQosInfo);

        /* ����<Maximum SDU size> */
        AT_SetCgeqreqParaMaxSduSize(&umtsQosInfo);

        /* ����<SDU error ratio> */
        AT_SetCgeqreqParaSduErrRatio(&umtsQosInfo);

        /* ����<Residual bit error ratio> */
        AT_SetCgeqreqParaResidualBer(&umtsQosInfo);

        /* ����<Delivery of erroneous SDUs> */
        AT_SetCgeqreqParaDeliverErrSdu(&umtsQosInfo);

        /* ����<Transfer delay> */
        AT_SetCgeqreqParaTransDelay(&umtsQosInfo);

        /* ����<Traffic handling priority> */
        AT_SetCgeqreqParaTraffHandlePrior(&umtsQosInfo);

        /* ����<Source Statistics Descriptor> */
        AT_SetCgeqreqParaSrcStatisticsDes(&umtsQosInfo);

        /* ����<Signalling Indication> */
        AT_SetCgeqreqParaSignalIndication(&umtsQosInfo);
    }

    /* ִ��������� */
    if (g_parseContext[indexNum].cmdElement->cmdIndex == AT_CMD_CGEQREQ) {
        if (TAF_PS_SetUmtsQosInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                  &umtsQosInfo) != VOS_OK) {
            return AT_ERROR;
        }

        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGEQREQ_SET;
    } else {
        if (TAF_PS_SetUmtsQosMinInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                     &umtsQosInfo) != VOS_OK) {
            return AT_ERROR;
        }

        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGEQMIN_SET;
    }

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 AT_SetCgeqminPara(TAF_UINT8 indexNum)
{
    return AT_SetCgeqreqPara(indexNum);
}

VOS_UINT32 AT_CountSpecCharInString(VOS_UINT8 *data, VOS_UINT16 len, VOS_UINT8 charValue)
{
    VOS_UINT8 *read = VOS_NULL_PTR;
    VOS_UINT32 chkLen;
    VOS_UINT32 count;

    if (data == VOS_NULL_PTR) {
        return 0;
    }

    read   = data;
    chkLen = 0;
    count  = 0;

    /* ��� */
    while (chkLen++ < len) {
        if (charValue == *read) {
            count++;
        }
        read++;
    }

    return count;
}

VOS_UINT32 AT_FillTftIpv4Addr(VOS_UINT8 *data, VOS_UINT16 len, TAF_TFT_Ext *tftInfo)
{
    VOS_UINT32 count = 0;
    VOS_UINT32 ret;
    VOS_CHAR  *ptrTmp = VOS_NULL_PTR;

    ptrTmp = VOS_NULL_PTR;

    count = AT_CountDigit(data, len, '.', 7); /* ������ȡIPV4��ַ��'.'��7�γ��ֵĵ�ַ */
    if ((count == 0) || (data[count] == 0)) {
        return AT_FAILURE;
    }

    count = AT_CountDigit(data, len, '.', 4); /* ������ȡIPV4��ַ��'.'��4�γ��ֵĵ�ַ */

    tftInfo->opSrcIp              = VOS_TRUE;
    tftInfo->sourceIpaddr.pdpType = TAF_PDP_IPV4;
    tftInfo->sourceIpMask.pdpType = TAF_PDP_IPV4;

    if ((len - count) > TAF_MAX_IPV4_ADDR_STR_LEN) {
        return AT_FAILURE;
    }

    ptrTmp = (VOS_CHAR *)&(data[count]);

    ret = AT_Ipv4AddrAtoi(ptrTmp, tftInfo->sourceIpMask.ipv4Addr, TAF_IPV4_ADDR_LEN);
    if (ret != VOS_OK) {
        return AT_FAILURE;
    }

    if ((count - 1) > TAF_MAX_IPV4_ADDR_STR_LEN) {
        return AT_FAILURE;
    }

    ptrTmp            = (VOS_CHAR *)(data);
    ptrTmp[count - 1] = '\0';

    ret = AT_Ipv4AddrAtoi(ptrTmp, tftInfo->sourceIpaddr.ipv4Addr, TAF_IPV4_ADDR_LEN);
    if (ret != VOS_OK) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_FillTftLocalIpv4Addr(VOS_UINT8 *data, VOS_UINT16 len, TAF_TFT_Ext *tftInfo)
{
    VOS_UINT32 count = 0;
    VOS_UINT32 ret;
    VOS_CHAR  *ptrTmp = VOS_NULL_PTR;

    ptrTmp = VOS_NULL_PTR;

    count = AT_CountDigit(data, len, '.', 7); /* ������ȡIPV4��ַ��'.'��7�γ��ֵĵ�ַ */
    if ((count == 0) || (data[count] == 0)) {
        return AT_FAILURE;
    }

    count = AT_CountDigit(data, len, '.', 4); /* ������ȡIPV4��ַ��'.'��4�γ��ֵĵ�ַ */

    tftInfo->opLocalIpv4AddrAndMask = VOS_TRUE;

    if ((len - count) > TAF_MAX_IPV4_ADDR_STR_LEN) {
        return AT_FAILURE;
    }

    ptrTmp = (VOS_CHAR *)&(data[count]);

    ret = AT_Ipv4AddrAtoi(ptrTmp, tftInfo->localIpv4Mask, TAF_IPV4_ADDR_LEN);
    if (ret != VOS_OK) {
        return AT_FAILURE;
    }

    if ((count - 1) > TAF_MAX_IPV4_ADDR_STR_LEN) {
        return AT_FAILURE;
    }

    ptrTmp            = (VOS_CHAR *)(data);
    ptrTmp[count - 1] = '\0';

    ret = AT_Ipv4AddrAtoi(ptrTmp, tftInfo->localIpv4Addr, TAF_IPV4_ADDR_LEN);
    if (ret != VOS_OK) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

/*
 * ��������: ���ַ�����ַ�л�ȡIPV6�������͵ĵ�ַ��Ϣ
 */
LOCAL VOS_UINT32 AT_GetIntegerIpv6AddrInfo(const VOS_CHAR *pcString, VOS_UINT32 *valTmp, VOS_UINT32 *dotNum, VOS_UINT8 *addr)
{
    VOS_UINT32 strLen;
    VOS_UINT32 i = 0;
    VOS_UINT32 numLen = 0;

    strLen = VOS_StrLen(pcString);

    if (strLen > VOS_StrLen("255.255.255.255.255.255.255.255.255.255.255.255.255.255.255.255")) {
        return VOS_ERR;
    }

    for (i = 0; i < strLen; i++) {
        if ((pcString[i] >= '0') && (pcString[i] <= '9')) {
            (*valTmp) = ((*valTmp) * 10) + (pcString[i] - '0'); /* ��10��Ϊ�˽��ַ���ת����ʮ������ */

            numLen++;
            continue;
        } else if (pcString[i] == '.') {
            if ((numLen == 0) || (numLen > 3)) { /* '.'�ָ��IPV6��ַ���ָ�ʽ���Ϊ255��������3����� */
                return VOS_ERR;
            }

            if ((*valTmp) > IPV6_ADDR_MAX_VALUE) {
                return VOS_ERR;
            }

            addr[(*dotNum)] = (VOS_UINT8)(*valTmp);

            (*valTmp) = 0;
            numLen = 0;

            /* ͳ��'.'�ĸ��� */
            (*dotNum)++;
            if ((*dotNum) >= TAF_IPV6_ADDR_LEN) {
                return VOS_ERR;
            }

            continue;
        } else {
            /* ����ֱֵ�ӷ���ʧ�� */
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

VOS_UINT32 AT_Ipv6AddrAtoi(VOS_CHAR *pcString, VOS_UINT8 *number, VOS_UINT32 ipv6BufLen)
{
    errno_t    memResult;
    VOS_UINT32 dotNum                  = 0;
    VOS_UINT32 valTmp                  = 0;
    VOS_UINT8  addr[TAF_IPV6_ADDR_LEN] = {0};

    if ((pcString == VOS_NULL_PTR) || (number == VOS_NULL_PTR)) {
        return VOS_ERR;
    }

    if (ipv6BufLen < TAF_IPV6_ADDR_LEN) {
        return VOS_ERR;
    }

    /* ���ַ�����ַ�л�ȡ�������͵ĵ�ַ��Ϣ */
    if (AT_GetIntegerIpv6AddrInfo(pcString, &valTmp, &dotNum, addr) == VOS_ERR) {
        return VOS_ERR;
    }

    /* �������3��'.'�򷵻�ʧ�� */
    if ((TAF_IPV6_ADDR_LEN - 1) != dotNum) {
        return VOS_ERR;
    }

    /* �������ַ���ȡֵ */
    if (valTmp > IPV6_ADDR_MAX_VALUE) {
        return VOS_ERR;
    }

    addr[dotNum] = (VOS_UINT8)valTmp;

    memResult = memcpy_s(number, ipv6BufLen, addr, TAF_IPV6_ADDR_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, ipv6BufLen, TAF_IPV6_ADDR_LEN);

    return VOS_OK;
}

VOS_UINT32 AT_FillTftIpv6Addr(VOS_UINT8 *data, VOS_UINT16 len, TAF_TFT_Ext *tftInfo)
{
    VOS_UINT32 count = 0;
    VOS_UINT32 ret;
    VOS_CHAR  *ptrTmp = VOS_NULL_PTR;

    ptrTmp = VOS_NULL_PTR;

    count = AT_CountDigit(data, len, '.', 31); /* ������ȡIPV6��ַ��'.'��31�γ��ֵĵ�ַ */
    if ((count == 0) || (data[count] == 0)) {
        return AT_FAILURE;
    }

    count = AT_CountDigit(data, len, '.', 16); /* ������ȡIPV6��ַ��'.'��16�γ��ֵĵ�ַ */

    tftInfo->opSrcIp              = VOS_TRUE;
    tftInfo->sourceIpaddr.pdpType = TAF_PDP_IPV6;
    tftInfo->sourceIpMask.pdpType = TAF_PDP_IPV6;

    if ((len - count) > TAF_MAX_IPV6_ADDR_DOT_STR_LEN) {
        return AT_FAILURE;
    }

    ptrTmp = (VOS_CHAR *)&(data[count]);

    ret = AT_Ipv6AddrAtoi(ptrTmp, tftInfo->sourceIpMask.ipv6Addr, TAF_IPV6_ADDR_LEN);
    if (ret != VOS_OK) {
        return AT_FAILURE;
    }

    if ((count - 1) > TAF_MAX_IPV6_ADDR_DOT_STR_LEN) {
        return AT_FAILURE;
    }

    ptrTmp            = (VOS_CHAR *)(data);
    ptrTmp[count - 1] = '\0';

    ret = AT_Ipv6AddrAtoi(ptrTmp, tftInfo->sourceIpaddr.ipv6Addr, TAF_IPV6_ADDR_LEN);
    if (ret != VOS_OK) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_FillTftLocalIpv6Addr(VOS_UINT8 *data, VOS_UINT16 len, TAF_TFT_Ext *tftInfo)
{
    VOS_UINT32 count = 0;
    VOS_UINT32 ret;
    VOS_CHAR  *ptrTmp = VOS_NULL_PTR;
    VOS_UINT8  localIpv6Mask[TAF_IPV6_ADDR_LEN];

    ptrTmp = VOS_NULL_PTR;
    (VOS_VOID)memset_s(localIpv6Mask, sizeof(localIpv6Mask), 0x00, sizeof(localIpv6Mask));

    count = AT_CountDigit(data, len, '.', 31); /* ������ȡIPV6��ַ��'.'��31�γ��ֵĵ�ַ */
    if ((count == 0) || (data[count] == 0)) {
        return AT_FAILURE;
    }

    count = AT_CountDigit(data, len, '.', 16); /* ������ȡIPV6��ַ��'.'��16�γ��ֵĵ�ַ */

    tftInfo->opLocalIpv6AddrAndMask = VOS_TRUE;

    if ((len - count) > TAF_MAX_IPV6_ADDR_DOT_STR_LEN) {
        return AT_FAILURE;
    }

    ptrTmp = (VOS_CHAR *)&(data[count]);

    ret = AT_Ipv6AddrAtoi(ptrTmp, localIpv6Mask, sizeof(localIpv6Mask));

    if (ret != VOS_OK) {
        return AT_FAILURE;
    }

    tftInfo->localIpv6Prefix = AT_CalcIpv6PrefixLength(localIpv6Mask, sizeof(localIpv6Mask));

    if ((count - 1) > TAF_MAX_IPV6_ADDR_DOT_STR_LEN) {
        return AT_FAILURE;
    }

    ptrTmp            = (VOS_CHAR *)(data);
    ptrTmp[count - 1] = '\0';

    ret = AT_Ipv6AddrAtoi(ptrTmp, tftInfo->localIpv6Addr, TAF_IPV6_ADDR_LEN);
    if (ret != VOS_OK) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_FillTftIpAddr(VOS_UINT8 *data, VOS_UINT16 len, TAF_TFT_Ext *tftInfo, VOS_UINT8 ipAddrType)
{
    VOS_UINT32 count;

    /* ��ȡ�����IP��ַ�а�����'.'�ĸ��� */
    count = AT_CountSpecCharInString(data, len, '.');

    /* �������ΪIPV4��ַ����ַ���������7��'.' */
    /* ����"10.140.23.144.255.255.255.0"��ǰ4����IPV4��ַ����4��Ϊ���� */
    if (count == (TAF_IPV4_ADDR_LEN * 2 - 1)) {
        if (ipAddrType == AT_IP_ADDR_TYPE_SOURCE) {
            return (AT_FillTftIpv4Addr(data, len, tftInfo));
        }

        if (ipAddrType == AT_IP_ADDR_TYPE_LOCAL) {
            return (AT_FillTftLocalIpv4Addr(data, len, tftInfo));
        }
    }
    /* �������ΪIPV6��ַ����ַ���������31��'.' */
    /* ����"32.8.0.2.0.2.0.1.0.2.0.1.0.3.21.111.255.255.255.255.255.255.255.255.255.255.255.255.255.255.255.255" */
    /* ǰ16��ΪIPV6��ַ����16��Ϊ���� */
    else if ((AT_GetIpv6Capability() != AT_IPV6_CAPABILITY_IPV4_ONLY) && ((TAF_IPV6_ADDR_LEN * 2 - 1) == count)) {
        if (ipAddrType == AT_IP_ADDR_TYPE_SOURCE) {
            return (AT_FillTftIpv6Addr(data, len, tftInfo));
        }

        if (ipAddrType == AT_IP_ADDR_TYPE_LOCAL) {
            return (AT_FillTftLocalIpv6Addr(data, len, tftInfo));
        }
    } else {
        return AT_FAILURE;
    }

    return AT_FAILURE;
}

TAF_UINT32 AT_CheckCgtftParaNum(VOS_UINT8 paraIndex)
{
    if (AT_IsSupportReleaseRst(AT_ACCESS_STRATUM_REL11)) {
        if (paraIndex > AT_SET_CGTFT_PARA_MAX_NUMBER) {
            return AT_FAILURE;
        }
    } else {
        if (paraIndex > AT_SET_CGTFT_PARA_MAX_NUMBER - 1) {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}

VOS_VOID AT_SetTftPfIdPara(TAF_TFT_Ext *tftInfo)
{
    if (g_atParaList[AT_CGTFT_PACKET_FILTER_ID].paraLen != 0) {
        tftInfo->opPktFilterId  = 1;
        tftInfo->packetFilterId = (VOS_UINT8)g_atParaList[AT_CGTFT_PACKET_FILTER_ID].paraValue;
    }
}

VOS_VOID AT_SetTftPrecedencePara(TAF_TFT_Ext *tftInfo)
{
    if (g_atParaList[AT_CGTFT_PRECEDENCE].paraLen != 0) {
        tftInfo->opPrecedence = 1;
        tftInfo->precedence   = (VOS_UINT8)g_atParaList[AT_CGTFT_PRECEDENCE].paraValue;
    }
}

VOS_VOID AT_SetTftProtocolIdPara(TAF_TFT_Ext *tftInfo)
{
    if (g_atParaList[AT_CGTFT_PROTOCOL_ID].paraLen != 0) {
        tftInfo->opProtocolId = 1;
        tftInfo->protocolId   = (VOS_UINT8)g_atParaList[AT_CGTFT_PROTOCOL_ID].paraValue;
    }
}

TAF_UINT32 AT_SetDestPortRangePara(TAF_TFT_Ext *tftInfo)
{
    VOS_UINT32 low   = 0;
    VOS_UINT32 high  = 0;
    VOS_UINT32 count = 0;

    if (g_atParaList[AT_CGTFT_DESTINATION_PORT_RANGE].paraLen != 0) {
        count = AT_CountDigit(g_atParaList[AT_CGTFT_DESTINATION_PORT_RANGE].para,
                              g_atParaList[AT_CGTFT_DESTINATION_PORT_RANGE].paraLen, '.', 1);
        if ((count == 0) || (g_atParaList[AT_CGTFT_DESTINATION_PORT_RANGE].para[count] == 0)) {
            return AT_FAILURE;
        }

        tftInfo->opDestPortRange = 1;

        if (atAuc2ul(g_atParaList[AT_CGTFT_DESTINATION_PORT_RANGE].para, (VOS_UINT16)(count - 1), &low) == AT_FAILURE) {
            return AT_FAILURE;
        }
        if (atAuc2ul(&g_atParaList[AT_CGTFT_DESTINATION_PORT_RANGE].para[count],
                     (VOS_UINT16)(g_atParaList[AT_CGTFT_DESTINATION_PORT_RANGE].paraLen - count),
                     &high) == AT_FAILURE) {
            return AT_FAILURE;
        }

        if ((low > AT_LOW_DEST_PORT_MAX_VALUE_RANGE) || (high > AT_HIGH_DEST_PORT_MAX_VALUE_RANGE) || (low > high)) {
            return AT_FAILURE;
        } else {
            tftInfo->highDestPort = (VOS_UINT16)high;
            tftInfo->lowDestPort  = (VOS_UINT16)low;
        }
    }

    return AT_SUCCESS;
}

TAF_UINT32 AT_SetSrcPortRangePara(TAF_TFT_Ext *tftInfo)
{
    VOS_UINT32 low   = 0;
    VOS_UINT32 high  = 0;
    VOS_UINT32 count = 0;

    if (g_atParaList[AT_CGTFT_SOURCE_PORT_RANGE].paraLen != 0) {
        count = AT_CountDigit(g_atParaList[AT_CGTFT_SOURCE_PORT_RANGE].para,
                              g_atParaList[AT_CGTFT_SOURCE_PORT_RANGE].paraLen, '.', 1);
        if ((count == 0) || (g_atParaList[AT_CGTFT_SOURCE_PORT_RANGE].para[count] == 0)) {
            return AT_FAILURE;
        }

        tftInfo->opSrcPortRange = 1;

        if (atAuc2ul(g_atParaList[AT_CGTFT_SOURCE_PORT_RANGE].para, (VOS_UINT16)(count - 1), &low) == AT_FAILURE) {
            return AT_FAILURE;
        }
        if (atAuc2ul(&g_atParaList[AT_CGTFT_SOURCE_PORT_RANGE].para[count],
                     (VOS_UINT16)(g_atParaList[AT_CGTFT_SOURCE_PORT_RANGE].paraLen - count), &high) == AT_FAILURE) {
            return AT_FAILURE;
        }
        if ((low > AT_LOW_SOURCE_PORT_MAX_VALUE_RANGE) || (high > AT_HIGH_SOURCE_PORT_MAX_VALUE_RANGE) || (low > high)) {
            return AT_FAILURE;
        } else {
            tftInfo->highSourcePort = (VOS_UINT16)high;
            tftInfo->lowSourcePort  = (VOS_UINT16)low;
        }
    }

    return AT_SUCCESS;
}

TAF_UINT32 AT_SetTftTosPara(TAF_TFT_Ext *tftInfo)
{
    VOS_UINT32 low   = 0;
    VOS_UINT32 high  = 0;
    VOS_UINT32 count = 0;

    if (g_atParaList[AT_CGTFT_TYPE_OF_SERVICE].paraLen != 0) {
        count = AT_CountDigit(g_atParaList[AT_CGTFT_TYPE_OF_SERVICE].para,
                              g_atParaList[AT_CGTFT_TYPE_OF_SERVICE].paraLen, '.', 1);
        if ((count == 0) || (g_atParaList[AT_CGTFT_TYPE_OF_SERVICE].para[count] == 0)) {
            return AT_FAILURE;
        }

        tftInfo->opTosMask = 1;

        if (atAuc2ul(g_atParaList[AT_CGTFT_TYPE_OF_SERVICE].para, (VOS_UINT16)(count - 1), &low) == AT_FAILURE) {
            return AT_FAILURE;
        }
        if (atAuc2ul(&g_atParaList[AT_CGTFT_TYPE_OF_SERVICE].para[count],
                     (VOS_UINT16)(g_atParaList[AT_CGTFT_TYPE_OF_SERVICE].paraLen - count), &high) == AT_FAILURE) {
            return AT_FAILURE;
        }

        if ((low > AT_TYPE_OF_SERVICE_MAX_VALUE_RANGE) || (high > AT_TYPE_OF_SERVICE_MASK_MAX_VALUE_RANGE)) {
            return AT_FAILURE;
        } else {
            tftInfo->typeOfServiceMask = (VOS_UINT8)high;
            tftInfo->typeOfService     = (VOS_UINT8)low;
        }
    }

    return AT_SUCCESS;
}

TAF_UINT32 AT_SetTftSecuPara(TAF_TFT_Ext *tftInfo)
{
    if (g_atParaList[AT_CGTFT_SECU_PARA_INDEX].paraLen != 0) {
        tftInfo->opSpi = 1;
        /* ��Ҫ�����жϴ�С */
        if (g_atParaList[AT_CGTFT_SECU_PARA_INDEX].paraLen > 8) { /* ����FFFFFFFF */
            return AT_FAILURE;
        }

        if (At_String2Hex(g_atParaList[AT_CGTFT_SECU_PARA_INDEX].para, g_atParaList[AT_CGTFT_SECU_PARA_INDEX].paraLen,
                          &g_atParaList[AT_CGTFT_SECU_PARA_INDEX].paraValue) == AT_FAILURE) {
            return AT_FAILURE;
        }
        tftInfo->secuParaIndex = g_atParaList[AT_CGTFT_SECU_PARA_INDEX].paraValue;
    }

    return AT_SUCCESS;
}

VOS_VOID AT_SetTftFlowLablePara(TAF_TFT_Ext *tftInfo)
{
    if (g_atParaList[AT_CGTFT_FLOW_LABLE].paraLen != 0) {
        tftInfo->opFlowLable = 1;
        tftInfo->flowLable   = g_atParaList[AT_CGTFT_FLOW_LABLE].paraValue;
    }
}

VOS_VOID AT_SetTftDirectionPara(TAF_TFT_Ext *tftInfo)
{
    if (g_atParaList[AT_CGTFT_DIRECTION].paraLen != 0) {
        tftInfo->opDirection = 1;
        tftInfo->direction   = (VOS_UINT8)g_atParaList[AT_CGTFT_DIRECTION].paraValue;
    }
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
VOS_VOID AT_SetTftQriPara(TAF_TFT_Ext *tftInfo)
{
    if (g_atParaList[AT_CGTFT_QRI].paraLen != 0) {
        tftInfo->opQri = 1;
        tftInfo->qri   = (VOS_UINT8)g_atParaList[AT_CGTFT_QRI].paraValue;
    }
}
#endif

TAF_UINT32 AT_SetCgtftPara(TAF_UINT8 indexNum)
{
    TAF_TFT_Ext tftInfo;
    VOS_UINT32  ret;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&tftInfo, sizeof(tftInfo), 0x00, sizeof(TAF_TFT_Ext));

    /* ���û��cid���򷵻�ʧ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��������,<flow label (ipv6)>��֧�֣�<direction>Ҳ���ټ�� */
    ret = AT_CheckCgtftParaNum(g_atParaIndex);
    if (ret == AT_FAILURE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<cid> */
    tftInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* �������ֻ��<CID>, �������������� */
    if (g_atParaIndex == 1) {
        tftInfo.defined = VOS_FALSE;
    } else {
        tftInfo.defined = VOS_TRUE;

        /* ����<packet filter identifier> */
        AT_SetTftPfIdPara(&tftInfo);

        /* ����<evaluation precedence index> */
        AT_SetTftPrecedencePara(&tftInfo);

        /* ����<source address and subnet mask> */
        if (g_atParaList[AT_CGTFT_SRC_ADDR_AND_SUBNET_MASK].paraLen != 0) {
            ret = AT_FillTftIpAddr(g_atParaList[AT_CGTFT_SRC_ADDR_AND_SUBNET_MASK].para,
                                   g_atParaList[AT_CGTFT_SRC_ADDR_AND_SUBNET_MASK].paraLen, &tftInfo,
                                   AT_IP_ADDR_TYPE_SOURCE);
            if (ret == AT_FAILURE) {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }

        /* ����<protocol number (ipv4) / next header (ipv6)> */
        AT_SetTftProtocolIdPara(&tftInfo);

        /* ����<destination port range> */
        ret = AT_SetDestPortRangePara(&tftInfo);
        if (ret == AT_FAILURE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����<source port range> */
        ret = AT_SetSrcPortRangePara(&tftInfo);
        if (ret == AT_FAILURE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����<ipsec security parameter index (spi)> */
        ret = AT_SetTftSecuPara(&tftInfo);
        if (ret == AT_FAILURE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����<type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask> */
        ret = AT_SetTftTosPara(&tftInfo);
        if (ret == AT_FAILURE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* <flow label (ipv6)> */
        AT_SetTftFlowLablePara(&tftInfo);

        /* <direction> */
        AT_SetTftDirectionPara(&tftInfo);

        if (AT_IsSupportReleaseRst(AT_ACCESS_STRATUM_REL11)) {
            /* ����<local address and subnet mask> */
            if (g_atParaList[AT_CGTFT_LOCAL_ADDR_AND_SUBNET_MASK].paraLen != 0) {
                ret = AT_FillTftIpAddr(g_atParaList[AT_CGTFT_LOCAL_ADDR_AND_SUBNET_MASK].para,
                                       g_atParaList[AT_CGTFT_LOCAL_ADDR_AND_SUBNET_MASK].paraLen, &tftInfo,
                                       AT_IP_ADDR_TYPE_LOCAL);
                if (ret == AT_FAILURE) {
                    return AT_CME_INCORRECT_PARAMETERS;
                }
            }

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
            /* <qri> */
            AT_SetTftQriPara(&tftInfo);
#endif
        }
    }

    if (TAF_PS_SetTftInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &tftInfo) !=
        VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGTFT_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 AT_SetCgautoPara(TAF_UINT8 indexNum)
{
    VOS_UINT32 ansMode;

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

    /* ����<n> */
    ansMode = g_atParaList[0].paraValue;

    /* ִ��������� */
    if (TAF_PS_SetAnsModeInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, ansMode) !=
        VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGAUTO_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 AT_SetCgpaddrPara(TAF_UINT8 indexNum)
{
    VOS_UINT32   i;
    VOS_UINT32   cidIndex = 0;
    TAF_CID_List cidListInfo;
    VOS_UINT32   loop;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&cidListInfo, sizeof(cidListInfo), 0x00, sizeof(TAF_CID_List));

    /* �������� */
    if (g_atParaIndex > TAF_MAX_CID) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

#if (FEATURE_LTE == FEATURE_ON)
    loop = AT_MAX_PARA_NUMBER;
#else
    loop = TAF_MAX_CID;
#endif

    /* �������� */
    if (g_atParaIndex != 0) {
        for (i = 0; i < loop; i++) {
            if (g_atParaList[i].paraLen == 0) {
                break;
            } else {
                cidIndex = g_atParaList[i].paraValue;
            }

            /* ��CID��Ϊ�±�, ��Ƕ�Ӧ��CID */
            cidListInfo.cid[cidIndex] = 1;
        }
    } else {
        for (i = 1; i <= TAF_MAX_CID; i++) {
            cidListInfo.cid[i] = 1;
        }
    }

    /* ִ��������� */
    if (TAF_PS_GetPdpIpAddrInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                &cidListInfo) != VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGPADDR_SET;

    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 AT_SetCgeqnegPara(TAF_UINT8 indexNum)
{
    VOS_UINT32   i;
    VOS_UINT32   cidIndex = 0;
    TAF_CID_List cidListInfo;
    VOS_UINT32   loop;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&cidListInfo, sizeof(cidListInfo), 0x00, sizeof(TAF_CID_List));

    /* �������� */
    if (g_atParaIndex > TAF_MAX_CID) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

#if (FEATURE_LTE == FEATURE_ON)
    loop = AT_MAX_PARA_NUMBER;
#else
    loop = TAF_MAX_CID;
#endif

    /* �������� */
    /* +CGEQNEG[=<cid>[,<cid>[,...]]] */
    if (g_atParaIndex != 0) {
        for (i = 0; i < loop; i++) {
            if (g_atParaList[i].paraLen == 0) {
                break;
            } else {
                cidIndex = g_atParaList[i].paraValue;
            }

            /* ��CID��Ϊ�±�, ��Ƕ�Ӧ��CID */
            cidListInfo.cid[cidIndex] = 1;
        }
    } else {
        for (i = 1; i <= TAF_MAX_CID; i++) {
            cidListInfo.cid[i] = 1;
        }
    }

    /* ִ��������� */
    if (TAF_PS_GetDynamicUmtsQosInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                     &cidListInfo) != VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGEQNEG_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_VOID AT_ParseCgdcontPara(TAF_PDP_PrimContextExt *pdpCxtInfo)
{
    /* ����<d_comp> */
    if (g_atParaList[AT_CGDCONT_D_COMP].paraLen != 0) {
        pdpCxtInfo->opPdpDcomp = VOS_TRUE;
        pdpCxtInfo->pdpDcomp   = (VOS_UINT8)g_atParaList[AT_CGDCONT_D_COMP].paraValue;
    }

    /* ����<h_comp> */
    if (g_atParaList[AT_CGDCONT_H_COMP].paraLen != 0) {
        pdpCxtInfo->opPdpHcomp = VOS_TRUE;
        pdpCxtInfo->pdpHcomp   = (VOS_UINT8)g_atParaList[AT_CGDCONT_H_COMP].paraValue;
    }

    /* ����<IPv4AddrAlloc> */
    if (g_atParaList[AT_CGDCONT_IPV4_ADDR_ALLOC].paraLen != 0) {
        pdpCxtInfo->opIpv4AddrAlloc = VOS_TRUE;
        pdpCxtInfo->ipv4AddrAlloc   = (VOS_UINT8)g_atParaList[AT_CGDCONT_IPV4_ADDR_ALLOC].paraValue;
    }

    /* ����<Emergency Indication> */
    if (g_atParaList[AT_CGDCONT_EMERGENCY_INDICATION].paraLen != 0) {
        pdpCxtInfo->opEmergencyInd = VOS_TRUE;
        pdpCxtInfo->emergencyFlg   = (VOS_UINT8)g_atParaList[AT_CGDCONT_EMERGENCY_INDICATION].paraValue;
    }

    /* ����<P-CSCF_discovery> */
    if (g_atParaList[AT_CGDCONT_P_CSCF_DISCOVERY].paraLen != 0) {
        pdpCxtInfo->opPcscfDiscovery = VOS_TRUE;
        pdpCxtInfo->pcscfDiscovery   = (VOS_UINT8)g_atParaList[AT_CGDCONT_P_CSCF_DISCOVERY].paraValue;
    }
    /* ����<IM_CN_Signalling_Flag_Ind> */
    if (g_atParaList[AT_CGDCONT_IM_CN_SIGNALLING_FLAG_IND].paraLen != 0) {
        pdpCxtInfo->opImCnSignalFlg = VOS_TRUE;
        pdpCxtInfo->imCnSignalFlg   = (VOS_UINT8)g_atParaList[AT_CGDCONT_IM_CN_SIGNALLING_FLAG_IND].paraValue;
    }
    /* ����<NAS_Signalling_Prio_Ind> */
    if (g_atParaList[AT_CGDCONT_NAS_SIGNALLING_PRIO_IND].paraLen != 0) {
        pdpCxtInfo->opNasSigPrioInd = VOS_TRUE;
        pdpCxtInfo->nasSigPrioInd   = (VOS_UINT8)g_atParaList[AT_CGDCONT_NAS_SIGNALLING_PRIO_IND].paraValue;
    }
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /* ����<SSC_mode> */
    if (g_atParaList[AT_CGDCONT_SSC_MODE].paraLen != 0) {
        pdpCxtInfo->opSscMode = VOS_TRUE;
        pdpCxtInfo->sscMode   = (VOS_UINT8)g_atParaList[AT_CGDCONT_SSC_MODE].paraValue;
    }

    /* ����<Pref_access_type> */
    if (g_atParaList[AT_CGDCONT_PREF_ACCESS_TYPE].paraLen != 0) {
        pdpCxtInfo->opPrefAccessType = VOS_TRUE;
        pdpCxtInfo->prefAccessType   = (VOS_UINT8)g_atParaList[AT_CGDCONT_PREF_ACCESS_TYPE].paraValue;
    }

    /* ����<RQos_ind> */
    if (g_atParaList[AT_CGDCONT_RQOS_IND].paraLen != 0) {
        pdpCxtInfo->opRQosInd = VOS_TRUE;
        pdpCxtInfo->rQosInd   = (VOS_UINT8)g_atParaList[AT_CGDCONT_RQOS_IND].paraValue;
    }

    /* ����<always on ind> */
    if (g_atParaList[AT_CGDCONT_ALWAYS_ON_IND].paraLen != 0) {
        pdpCxtInfo->opAlwaysOnInd = VOS_TRUE;
        pdpCxtInfo->alwaysOnInd   = (VOS_UINT8)g_atParaList[AT_CGDCONT_ALWAYS_ON_IND].paraValue;
    }
#endif
}

LOCAL AT_RreturnCodeUint32 AT_ParseCgdcontIpType(TAF_PDP_PrimContextExt *pdpCxtInfo)
{
    /*
     * �Ա��ն˵Ľ������<PDP_type>����Ϊ""(��������)ʱ,����ԭ�ȵ�ֵ��
     * ��g_TafCidTab[ucCid].ucUsedָʾ�����ı�ɾ������ʹ��֮ǰ����������ֵ
     */
    if (g_atParaList[1].paraLen != 0) {
        /* IP:1, IPV6:2, IPV4V6:3, PPP:4, Ethernet:8 */
        switch (g_atParaList[1].paraValue) {
            case AT_PDP_TYPE_IPV4:
                pdpCxtInfo->pdpType = TAF_PDP_IPV4;
                break;
            case AT_PDP_TYPE_IPV6:
                pdpCxtInfo->pdpType = TAF_PDP_IPV6;
                break;
            case AT_PDP_TYPE_IPV4V6:
                pdpCxtInfo->pdpType = TAF_PDP_IPV4V6;
                break;
            case AT_PDP_TYPE_PPP:
                pdpCxtInfo->pdpType = TAF_PDP_PPP;
                break;
            case AT_PDP_TYPE_ETHERNET:
                if (AT_GetEthernetCap() == VOS_TRUE) {
                    pdpCxtInfo->pdpType = TAF_PDP_ETHERNET;
                    break;
                } else {
                    return AT_CME_INCORRECT_PARAMETERS;
                }
            default:
                return AT_CME_INCORRECT_PARAMETERS;
        }

        if ((pdpCxtInfo->pdpType == TAF_PDP_IPV6) || (pdpCxtInfo->pdpType == TAF_PDP_IPV4V6)) {
            if (AT_GetIpv6Capability() == AT_IPV6_CAPABILITY_IPV4_ONLY) {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        }
        pdpCxtInfo->opPdpType = VOS_TRUE;
    }
    return AT_SUCCESS;
}

LOCAL AT_RreturnCodeUint32 AT_ParseCgdcontApn(TAF_PDP_PrimContextExt *pdpCxtInfo, VOS_UINT16 clientId)
{
    errno_t memResult;
    if (g_atParaList[AT_CGDCONT_APN].paraLen > TAF_MAX_APN_LEN) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[AT_CGDCONT_APN].paraLen != 0) {
        /* ���APN */
        if (AT_CheckApnFormat(g_atParaList[AT_CGDCONT_APN].para,
                              g_atParaList[AT_CGDCONT_APN].paraLen, clientId) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        pdpCxtInfo->opApn = VOS_TRUE;
        memResult = memcpy_s((VOS_CHAR *)pdpCxtInfo->apn, sizeof(pdpCxtInfo->apn),
                             (VOS_CHAR *)g_atParaList[AT_CGDCONT_APN].para,
                             g_atParaList[AT_CGDCONT_APN].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(pdpCxtInfo->apn), g_atParaList[AT_CGDCONT_APN].paraLen);
        pdpCxtInfo->apn[g_atParaList[AT_CGDCONT_APN].paraLen] = 0;
    }

    return AT_SUCCESS;
}

LOCAL AT_RreturnCodeUint32 AT_ParseCgdcontIpAddr(TAF_PDP_PrimContextExt *pdpCxtInfo)
{
    if (g_atParaList[AT_CGDCONT_PDP_ADDRESS].paraLen != 0) {
        if (((g_atParaList[1].paraValue + 1) != TAF_PDP_IPV4) ||
            (g_atParaList[AT_CGDCONT_PDP_ADDRESS].paraLen > (TAF_MAX_IPV4_ADDR_STR_LEN - 1))) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        pdpCxtInfo->opPdpAddr       = VOS_TRUE;
        pdpCxtInfo->pdpAddr.pdpType = TAF_PDP_IPV4;

        /* ��IP��ַ�ַ���ת�������� */
        if (AT_Ipv4AddrAtoi((VOS_CHAR *)g_atParaList[AT_CGDCONT_PDP_ADDRESS].para,
                            (VOS_UINT8 *)pdpCxtInfo->pdpAddr.ipv4Addr,
                            TAF_IPV4_ADDR_LEN) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_SUCCESS;
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)

LOCAL AT_RreturnCodeUint32 AT_ParseCgdcontSNssai(TAF_PDP_PrimContextExt *pdpCxtInfo)
{
    /* ����S-NSSAI���� */
    if (g_atParaList[AT_CGDCONT_S_NSSAI].paraLen != 0) {
        if (AT_DecodeSNssai(g_atParaList[AT_CGDCONT_S_NSSAI].para,
            g_atParaList[AT_CGDCONT_S_NSSAI].paraLen, &pdpCxtInfo->sNssai) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        pdpCxtInfo->opSNssai = VOS_TRUE;
    }

    return AT_SUCCESS;
}

LOCAL AT_RreturnCodeUint32 AT_ParseCgdcontMhpv6(TAF_PDP_PrimContextExt *pdpCxtInfo)
{
    /* ����<MH6-PDU> */
    if (g_atParaList[AT_CGDCONT_MH6_PDU].paraLen != 0) {
        pdpCxtInfo->opMh6Pdu = VOS_TRUE;
        pdpCxtInfo->mh6Pdu   = (VOS_UINT8)g_atParaList[AT_CGDCONT_MH6_PDU].paraValue;
    }

    /* �ο�24501 6.2.11�½ڣ�ֻ����IPV6����IPV4V6����ʱ������Ҫ֧��MHPV6 */
    if (pdpCxtInfo->mh6Pdu == TAF_PS_IPV6_MULTI_HOMING_IND_SUPPORT) {
        if ((pdpCxtInfo->pdpAddr.pdpType != TAF_PDP_IPV6) && (pdpCxtInfo->pdpAddr.pdpType != TAF_PDP_IPV4V6)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_SUCCESS;
}

#endif

VOS_UINT32 AT_SetCgdcontPara(VOS_UINT8 indexNum)
{
    TAF_PDP_PrimContextExt pdpCxtInfo;

    AT_RreturnCodeUint32 errCode = AT_SUCCESS;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&pdpCxtInfo, sizeof(pdpCxtInfo), 0x00, sizeof(TAF_PDP_PrimContextExt));

    /* ������� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_SET_CGDCONT_PARA_MAX_NUMBER) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<CID> */
    pdpCxtInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* �������ֻ��<CID>, �������������� */
    if (g_atParaIndex == 1) {
        pdpCxtInfo.defined = VOS_FALSE;
    } else {
        pdpCxtInfo.defined = VOS_TRUE;

        /* ����<PDP_type> */
        errCode |= AT_ParseCgdcontIpType(&pdpCxtInfo);

        /* ����<APN> */
        errCode |= AT_ParseCgdcontApn(&pdpCxtInfo, g_atClientTab[indexNum].clientId);

        /* ����<PDP_addr> */
        /* ֻ����IPV4��IP ADDR��IPV6��ʹ�ô�AT�������õ�IP ADDR */
        errCode |= AT_ParseCgdcontIpAddr(&pdpCxtInfo);

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        /* ����<S-NSSAI> */
        errCode |= AT_ParseCgdcontSNssai(&pdpCxtInfo);

        errCode |= AT_ParseCgdcontMhpv6(&pdpCxtInfo);

#endif
        /* ���<IP TYEP><APN><PDP_addr><S-NSSAI>��������ȷ�� */
        if (errCode != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        AT_ParseCgdcontPara(&pdpCxtInfo);
    }

    /* ִ��������� */
    if (TAF_PS_SetPrimPdpContextInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                     &pdpCxtInfo) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGDCONT_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetCgdscontPara(VOS_UINT8 indexNum)
{
    TAF_PDP_SecContextExt pdpCxtInfo;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&pdpCxtInfo, sizeof(pdpCxtInfo), 0x00, sizeof(TAF_PDP_SecContextExt));

    /* ������� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CGDSCONT_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<s_cid> */
    pdpCxtInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* �������ֻ��<CID>, �������������� */
    if (g_atParaIndex == 1) {
        pdpCxtInfo.defined = VOS_FALSE;
    } else {
        pdpCxtInfo.defined = VOS_TRUE;

        /* ����<p_cid> */
        if (g_atParaList[1].paraLen != 0) {
            pdpCxtInfo.opLinkdCid = VOS_TRUE;
            pdpCxtInfo.linkdCid   = (VOS_UINT8)g_atParaList[1].paraValue;

            if (pdpCxtInfo.cid == pdpCxtInfo.linkdCid) {
                return AT_CME_INCORRECT_PARAMETERS;
            }
        } else {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����<d_comp> */
        if (g_atParaList[AT_CGDCONT_PDP_D_COMP].paraLen != 0) {
            pdpCxtInfo.opPdpDcomp = VOS_TRUE;
            pdpCxtInfo.pdpDcomp   = (VOS_UINT8)g_atParaList[AT_CGDCONT_PDP_D_COMP].paraValue;
        }

        /* ����<h_comp> */
        if (g_atParaList[AT_CGDCONT_PDP_H_COMP].paraLen != 0) {
            pdpCxtInfo.opPdpHcomp = VOS_TRUE;
            pdpCxtInfo.pdpHcomp   = (VOS_UINT8)g_atParaList[AT_CGDCONT_PDP_H_COMP].paraValue;
        }
        /* ����<IM_CN_Signalling_Flag_Ind> */
        if (g_atParaList[AT_CGDCONT_IM_CN_SIGNAL_FLAG].paraLen != 0) {
            pdpCxtInfo.opImCnSignalFlg = VOS_TRUE;
            pdpCxtInfo.imCnSignalFlg   = (VOS_UINT8)g_atParaList[AT_CGDCONT_IM_CN_SIGNAL_FLAG].paraValue;
        }
    }

    /* ִ��������� */
    if (TAF_PS_SetSecPdpContextInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                    &pdpCxtInfo) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGDSCONT_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 AT_SetCgactPara(TAF_UINT8 indexNum)
{
    VOS_UINT32        i;
    VOS_UINT32        cidIndex = 0;
    TAF_CID_ListState cidListStateInfo;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&cidListStateInfo, sizeof(cidListStateInfo), 0x00, sizeof(TAF_CID_ListState));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != AT_CGACT_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    cidListStateInfo.state = (VOS_UINT8)g_atParaList[0].paraValue;

    for (i = 0; i < (AT_MAX_PARA_NUMBER - 1); i++) {
        if (g_atParaList[i + 1UL].paraLen == 0) {
            break;
        } else {
            cidIndex = g_atParaList[i + 1UL].paraValue;
        }

        /* ��CID��Ϊ�±�, ��Ƕ�Ӧ��CID */
        cidListStateInfo.cid[cidIndex] = 1;
    }

    /* ִ��������� */
    if (TAF_PS_SetPdpContextState(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                  &cidListStateInfo) != VOS_OK) {
        return AT_ERROR;
    }

    if (cidListStateInfo.state == 1) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGACT_ORG_SET;
    } else {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGACT_END_SET;
    }

    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 AT_SetCgcmodPara(TAF_UINT8 indexNum)
{
    VOS_UINT32   i;
    VOS_UINT32   cidIndex = 0;
    TAF_CID_List cidListInfo;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&cidListInfo, sizeof(cidListInfo), 0x00, sizeof(TAF_CID_List));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != 0) {
        for (i = 0; i < AT_MAX_PARA_NUMBER; i++) {
            if (g_atParaList[i].paraLen == 0) {
                break;
            } else {
                cidIndex = g_atParaList[i].paraValue;
            }

            /* ��CID��Ϊ�±�, ��Ƕ�Ӧ��CID */
            cidListInfo.cid[cidIndex] = VOS_TRUE;
        }
    }

    /* ִ��������� */
    if (TAF_PS_CallModify(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &cidListInfo) !=
        VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGCMOD_SET;

    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 AT_SetCgansPara(TAF_UINT8 indexNum)
{
    VOS_UINT8     rspType;
    TAF_PS_Answer ansInfo;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&ansInfo, sizeof(ansInfo), 0x00, sizeof(TAF_PS_Answer));

    /* ������� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CGANS_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    rspType = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ����<CID> */
    if (g_atParaList[AT_CGANS_CID].paraLen != 0) {
        ansInfo.cid = (VOS_UINT8)g_atParaList[AT_CGANS_CID].paraValue;
    }

    /* ���ans��չ��־ */
    if (g_parseContext[indexNum].cmdElement->cmdIndex == AT_CMD_CGANS_EXT) {
        ansInfo.ansExtFlg = VOS_TRUE;
    } else {
        ansInfo.ansExtFlg = VOS_FALSE;
    }

    /* ִ��������� */
    if (rspType == 1) {
        if (TAF_PS_CallAnswer(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &ansInfo) ==
            VOS_OK) {
            /* ���õ�ǰ�������� */
            if (g_parseContext[indexNum].cmdElement->cmdIndex == AT_CMD_CGANS_EXT) {
                g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGANS_ANS_EXT_SET;
            } else {
                g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGANS_ANS_SET;
            }

            /* ������������״̬ */
            return AT_WAIT_ASYNC_RETURN;
        } else {
            return AT_ERROR;
        }
    } else {
        /* 0 --  �ܾ����緢��ļ���ָʾ */
        if (TAF_PS_CallHangup(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0) == VOS_OK) {
            /* ���õ�ǰ�������� */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGANS_ANS_SET;

            /* ������������״̬ */
            return AT_WAIT_ASYNC_RETURN;
        } else {
            return AT_ERROR;
        }
    }
}

TAF_UINT32 At_SetCgdataPara(TAF_UINT8 indexNum)
{
    VOS_UINT32        cidIndex = 0;
    TAF_CID_ListState cidListStateInfo;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&cidListStateInfo, sizeof(cidListStateInfo), 0x00, sizeof(TAF_CID_ListState));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��������, Ŀǰ���֧��һ��CID */
    if (g_atParaIndex > AT_CGDATA_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* <L2P>������ */

    /* �������� */
    cidListStateInfo.state = 1;

    if (g_atParaList[1].paraLen == 0) {
        cidIndex = 1;
    } else {
        cidIndex = g_atParaList[1].paraValue;
    }

    /* ��CID��Ϊ�±�, ��Ƕ�Ӧ��CID */
    cidListStateInfo.cid[cidIndex] = 1;

    /* ִ��������� */
    if (TAF_PS_SetPdpContextState(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                  &cidListStateInfo) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGDATA_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

TAF_UINT32 AT_SetCgdnsPara(TAF_UINT8 indexNum)
{
    TAF_PDP_DnsExt pdpDnsInfo;

    VOS_UINT8  tempIpv4Addr[TAF_IPV4_ADDR_LEN];
    VOS_UINT32 rslt;
    errno_t    memResult;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&pdpDnsInfo, sizeof(pdpDnsInfo), 0x00, sizeof(TAF_PDP_DnsExt));

    /* ������� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CGDNS_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<cid> */
    pdpDnsInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* �������ֻ��<CID>, �������������� */
    if (g_atParaIndex == 1) {
        pdpDnsInfo.defined = VOS_FALSE;
    } else {
        pdpDnsInfo.defined = VOS_TRUE;

        /* ����<PriDns> */
        if (g_atParaList[1].paraLen != 0) {
            /* ���<PriDns>������ֱ�ӷ��ش��� */
            if (g_atParaList[1].paraLen > (TAF_MAX_IPV4_ADDR_STR_LEN - 1)) {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            rslt = AT_Ipv4AddrAtoi((VOS_CHAR *)g_atParaList[1].para, &tempIpv4Addr[0], TAF_IPV4_ADDR_LEN);

            if (rslt != VOS_OK) {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            pdpDnsInfo.opPrimDnsAddr = VOS_TRUE;
            memResult                = memcpy_s(pdpDnsInfo.primDnsAddr, sizeof(pdpDnsInfo.primDnsAddr), tempIpv4Addr,
                                                TAF_IPV4_ADDR_LEN);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(pdpDnsInfo.primDnsAddr), TAF_IPV4_ADDR_LEN);
        }

        /* ����<SecDns> */
        if (g_atParaList[AT_CGDNS_SECDNS].paraLen != 0) {
            /* ���<SecDns>������ֱ�ӷ��ش��� */
            if (g_atParaList[AT_CGDNS_SECDNS].paraLen > (TAF_MAX_IPV4_ADDR_STR_LEN - 1)) {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            /* �˴��洢�����ַ�����ʽ��DNS����ת�����������ʹ�� */
            rslt = AT_Ipv4AddrAtoi((VOS_CHAR *)g_atParaList[AT_CGDNS_SECDNS].para,
                                   &tempIpv4Addr[0], TAF_IPV4_ADDR_LEN);

            if (rslt != VOS_OK) {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            pdpDnsInfo.opSecDnsAddr = VOS_TRUE;
            memResult = memcpy_s(pdpDnsInfo.secDnsAddr, sizeof(pdpDnsInfo.secDnsAddr), tempIpv4Addr, TAF_IPV4_ADDR_LEN);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(pdpDnsInfo.secDnsAddr), TAF_IPV4_ADDR_LEN);
        }
    }

    /* ִ��������� */
    if (TAF_PS_SetPdpDnsInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &pdpDnsInfo) !=
        VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGDNS_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 At_SetGlastErrPara(VOS_UINT8 indexNum)
{
    VOS_UINT16         length = 0;
    TAF_PS_CauseUint32 psCause;
    TAF_UINT32         atErrType;

    /* ��鲦�Ŵ������Ƿ�ʹ�� */
    if (g_pppDialErrCodeRpt == PPP_DIAL_ERR_CODE_DISABLE) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* û�в��� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����ֵ���� */
    if (g_atParaList[0].paraValue != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    AT_SetErrType(g_atParaList[0].paraValue);

    /* ��ȡ������ */
    psCause = AT_PS_GetPsCallErrCause(indexNum);
    if (psCause == TAF_PS_CAUSE_SUCCESS) {
        return AT_ERROR;
    }

    atErrType = AT_GetErrType();
    /* ���������ϱ�����̨ */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %d,%d", g_parseContext[indexNum].cmdElement->cmdName, atErrType,
        AT_Get3gppSmCauseByPsCause(psCause));

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

VOS_UINT32 AT_SetDnsQueryPara(VOS_UINT8 indexNum)
{
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

    /* ��APSģ�鷢��ִ�����������Ϣ */
    if (TAF_PS_GetDynamicDnsInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId),
                                 g_atClientTab[indexNum].opId, (VOS_UINT8)g_atParaList[0].paraValue) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DNSQUERY_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetLFastDormPara(VOS_UINT8 indexNum)
{
    VOS_UINT32           ret;
    L4A_SET_FastDormReq  req            = {0};
    LPS_SWITCH_PARA_STRU drxControlFlag = {0};

    /* ���������AT������� */

    req.ctrl.clientId = g_atClientTab[indexNum].clientId;
    req.ctrl.opId     = 0;
    req.ctrl.pid      = WUEPS_PID_AT;
    req.flag          = (VOS_INT32)g_atParaList[0].paraValue;
    req.timerLen      = (VOS_INT32)g_atParaList[1].paraValue;

    if (TAF_ACORE_NV_READ(MODEM_ID_0, EN_NV_ID_SWITCH_PARA, &drxControlFlag, sizeof(LPS_SWITCH_PARA_STRU)) != NV_OK) {
        return AT_ERROR;
    }

    if (((*((VOS_UINT32 *)(&(drxControlFlag.stPsFunFlag01)))) & LPS_NV_JP_DCOM_FAST_DORM_BIT) == 0) {
        return AT_OK;
    }

    /* Lģ�������½ӿ�ת�� */
    ret = atSendL4aDataMsg(g_atClientTab[indexNum].clientId, I0_MSP_L4_L4A_PID, ID_MSG_L4A_FAST_DORM_REQ,
                           (VOS_UINT8 *)(&req), sizeof(L4A_SET_FastDormReq));

    if (ret == ERR_MSP_SUCCESS) {
        return AT_OK;
    }
    return AT_ERROR;
}

VOS_UINT32 AT_SndSetFastDorm(VOS_UINT16 clientId, VOS_UINT8 opId, AT_RABM_FastdormPara *fastDormPara)
{
    AT_RABM_SetFastdormParaReq *sndMsg = VOS_NULL_PTR;
    VOS_UINT32                  rslt;

    /* �����ڴ�  */
    /*lint -save -e516 */
    sndMsg = (AT_RABM_SetFastdormParaReq *)TAF_AllocMsgWithoutHeaderLen(WUEPS_PID_AT,
                                                        sizeof(AT_RABM_SetFastdormParaReq) - VOS_MSG_HEAD_LENGTH);
    /*lint -restore */
    if (sndMsg == VOS_NULL_PTR) {
        /* �ڴ�����ʧ�� */
        AT_ERR_LOG("AT_SndSetFastDorm:ERROR: Memory Alloc Error for pstMsg");
        return VOS_ERR;
    }

    /* ��д��ز��� */
    TAF_CfgMsgHdr((MsgBlock *)sndMsg, WUEPS_PID_AT, AT_GetDestPid(clientId, I0_WUEPS_PID_RABM),
                  sizeof(AT_RABM_SetFastdormParaReq) - VOS_MSG_HEAD_LENGTH);

    sndMsg->msgHeader.msgName = ID_AT_RABM_SET_FASTDORM_PARA_REQ;
    sndMsg->fastDormPara      = *fastDormPara;
    sndMsg->clientId          = clientId;
    sndMsg->opId              = opId;

    /* ����VOS����ԭ�� */
    rslt = TAF_TraceAndSendMsg(WUEPS_PID_AT, sndMsg);
    if (rslt != VOS_OK) {
        AT_ERR_LOG("AT_SndSetFastDorm:ERROR:TAF_TraceAndSendMsg ");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_SetFastDormPara(VOS_UINT8 indexNum)
{
    AT_RABM_FastdormPara fastDormPara;
    VOS_UINT32           rslt;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex > AT_FASTDORM_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��һ������Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �ڶ�������Ϊ�գ�����дĬ��ֵ */
    if (g_atParaList[1].paraLen == 0) {
        /* ��дĬ��ֵ5S */
        g_atParaList[1].paraValue = AT_FASTDORM_DEFAULT_TIME_LEN;
    }

    fastDormPara.fastDormOperationType = g_atParaList[0].paraValue;
    fastDormPara.timeLen               = g_atParaList[1].paraValue;

#if (FEATURE_LTE == FEATURE_ON)
    AT_SetLFastDormPara(indexNum);
#endif

    /* ��AT�ںϰ汾�ϣ������Ľӿ�Ϊֱ�ӷ���Ϣ���������ֱ�ӷ�����Ϣ��RABM */
    rslt = AT_SndSetFastDorm(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, &fastDormPara);
    if (rslt == VOS_OK) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_FASTDORM_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 atSetCgcontrdpPara(VOS_UINT8 indexNum)
{
    VOS_UINT8 cid = 0;

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen != 0) {
        cid = (VOS_UINT8)g_atParaList[0].paraValue;
    } else {
        cid = 0xff;
    }

    if (TAF_PS_GetDynamicPrimPdpContextInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                            cid) != VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGCONTRDP_SET;

    return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
}

VOS_UINT32 atSetCgscontrdpPara(VOS_UINT8 indexNum)
{
    VOS_UINT8 cid = 0;

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen != 0) {
        cid = (VOS_UINT8)g_atParaList[0].paraValue;
    } else {
        cid = 0xff;
    }

    if (TAF_PS_GetDynamicSecPdpContextInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                           cid) != VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGSCONTRDP_SET;

    return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
}

VOS_UINT32 atSetCgtftrdpPara(VOS_UINT8 indexNum)
{
    VOS_UINT8 cid = 0;

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen != 0) {
        cid = (VOS_UINT8)(g_atParaList[0].paraValue);
    } else {
        cid = 0xff;
    }

    if (TAF_PS_GetDynamicTftInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, cid) !=
        VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGTFTRDP_SET;

    return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
}

VOS_UINT32 AT_PS_GenIfaceId(const VOS_UINT8 indexNum, const PS_IFACE_IdUint8 beginIfaceId,
                            const PS_IFACE_IdUint8 endIfaceId, VOS_UINT8 cid)
{
    AT_CommPsCtx           *psCtx = VOS_NULL_PTR;
    AT_CH_DataChannelUint32 dataChannelId;
    VOS_UINT32              i;
    VOS_UINT32              rslt;
    ModemIdUint16           modemId = MODEM_ID_0;

    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        AT_WARN_LOG("AT_PS_GenIfaceId: get modem id error!");
        return AT_ERROR;
    }

    /* E5��̬APP���Ͳ��ſ��Բ���Ҫ^CHDATA���� */
    for (i = beginIfaceId; i <= endIfaceId; i++) {
        dataChannelId = AT_CH_DATA_CHANNEL_BUTT;

        rslt = AT_PS_GetChDataValueFromIfaceId((VOS_UINT8)i, &dataChannelId);

        if (rslt == VOS_ERR) {
            continue;
        }

        if (dataChannelId >= AT_CH_DATA_CHANNEL_BUTT) {
            continue;
        }

        psCtx = AT_GetCommPsCtxAddr();

        if (psCtx->channelCfg[dataChannelId].used == VOS_TRUE) {
            continue;
        }

        psCtx->channelCfg[dataChannelId].used    = VOS_TRUE;
        psCtx->channelCfg[dataChannelId].rmNetId = AT_PS_GetRmnetIdFromIfaceId((VOS_UINT8)i);
        psCtx->channelCfg[dataChannelId].ifaceId = i;
        psCtx->channelCfg[dataChannelId].cid     = cid;
        psCtx->channelCfg[dataChannelId].modemId = modemId;

        return AT_SUCCESS;
    }

    AT_NORM_LOG("AT_PS_GenIfaceId: IFACEID is all used!");
    return AT_ERROR;
}

VOS_UINT32 AT_PS_CheckDialParamDataChanl(const VOS_UINT8 indexNum, const TAF_IFACE_UserTypeUint8 userType)
{
    AT_PS_DataChanlCfg *chanCfg         = VOS_NULL_PTR;
    VOS_UINT8          *systemAppConfig = VOS_NULL_PTR;
    PS_IFACE_IdUint8    beginIfaceId;
    PS_IFACE_IdUint8    endIfaceId;

    systemAppConfig = AT_GetSystemAppConfigAddr();
    chanCfg         = AT_PS_GetDataChanlCfg(indexNum, (VOS_UINT8)g_atParaList[0].paraValue);

    /* �ֻ���̬������ʹ��^CHDATA���� */
    if ((userType == TAF_IFACE_USER_TYPE_APP) && (*systemAppConfig != SYSTEM_APP_WEBUI)) {
        /* ���ͨ��ӳ�� */
        if ((chanCfg->used == VOS_FALSE) || (chanCfg->rmNetId == AT_PS_INVALID_RMNET_ID)) {
            AT_NORM_LOG2("AT_PS_CheckDialParamDataChanl: Used is .RmNetId is .\n", chanCfg->used, chanCfg->rmNetId);
            return AT_CME_INCORRECT_PARAMETERS;
        }

        return AT_SUCCESS;
    }

    /* ����ʱ�����ֻ���̬�Ĳ������ʹ����^CHDATA���ã���ֱ��ʹ��������Ϣ��������Ҫ�������IFACEID */
    if (g_atParaList[1].paraValue == TAF_PS_CALL_TYPE_UP) {
        if (chanCfg->used == VOS_FALSE) {
            if (userType == TAF_IFACE_USER_TYPE_NDIS) {
                /* NDIS���͵Ĳ���Ĭ��ֱ��ʹ��PS_IFACE_ID_NDIS0 */
                beginIfaceId = PS_IFACE_ID_NDIS0;
#if (FEATURE_MULTI_NCM == FEATURE_ON)
                endIfaceId   = PS_IFACE_ID_NDIS3;
#else
                endIfaceId   = PS_IFACE_ID_NDIS0;
#endif
            } else {
                /* E5��̬APP���Ͳ��ŷ�ΧΪPS_IFACE_ID_RMNET0~PS_IFACE_ID_RMNET7 */
                beginIfaceId = PS_IFACE_ID_RMNET0;
                endIfaceId   = PS_IFACE_ID_RMNET7;
            }

            return AT_PS_GenIfaceId(indexNum, beginIfaceId, endIfaceId, (VOS_UINT8)g_atParaList[0].paraValue);
        }
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_PS_CheckDialParamCnt(TAF_IFACE_UserTypeUint8 userType)
{
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* ���������� */
    if (userType == TAF_IFACE_USER_TYPE_APP) {
        if (*systemAppConfig != SYSTEM_APP_WEBUI) {
            if (g_atParaIndex > AT_PHONE_DIAL_MAX_PARA_NUM) {
                AT_NORM_LOG1("AT_PS_CheckDialParamCnt: Phone Dial Parameter number is .\n", g_atParaIndex);
                return AT_TOO_MANY_PARA;
            }
        } else {
            if (g_atParaIndex > AT_WEBUI_DIAL_MAX_PARA_NUM) {
                AT_NORM_LOG1("AT_PS_CheckDialParamCnt: WEBUI APP Dial Parameter number is .\n", g_atParaIndex);
                return AT_TOO_MANY_PARA;
            }
        }

        return AT_SUCCESS;
    }

    if (userType == TAF_IFACE_USER_TYPE_NDIS) {
        /* ����MBB��Ʒ��Ҫ����չ��7�� */
        if (g_atParaIndex > AT_NDIS_DIAL_MAX_PARA_NUM) {
            AT_NORM_LOG1("AT_PS_CheckDialParamCnt: NDIS Dial Parameter number is .\n", g_atParaIndex);
            return AT_TOO_MANY_PARA;
        } else {
            return AT_SUCCESS;
        }
    }

    AT_WARN_LOG("AT_PS_ValidateDialParam: User Type is invalid.\n");

    return AT_ERROR;
}

VOS_UINT32 AT_PS_CheckDialParamApn(VOS_UINT16 clientId)
{
    /* ��� APN */
    if (g_atParaList[AT_PS_DIAL_PARAM_APN].paraLen != 0) {
        /* APN���ȼ�� */
        if (g_atParaList[AT_PS_DIAL_PARAM_APN].paraLen > TAF_MAX_APN_LEN) {
            AT_NORM_LOG("AT_PS_CheckDialParamApn: APN is too long.");
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* APN��ʽ��� */
        if (AT_CheckApnFormat(g_atParaList[AT_PS_DIAL_PARAM_APN].para,
                              g_atParaList[AT_PS_DIAL_PARAM_APN].paraLen, clientId) != VOS_OK) {
            AT_NORM_LOG("AT_PS_CheckDialParamApn: Format of APN is wrong.");
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_PS_CheckDialParamIpAddr(VOS_VOID)
{
    VOS_UINT8 ipv4Addr[TAF_IPV4_ADDR_LEN];

    (VOS_VOID)memset_s(ipv4Addr, sizeof(ipv4Addr), 0x00, sizeof(ipv4Addr));

    /* ip addr��� */
    if (g_atParaList[AT_PS_DIAL_PARAM_IP_ADDR].paraLen > (TAF_MAX_IPV4_ADDR_STR_LEN - 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[AT_PS_DIAL_PARAM_IP_ADDR].paraLen > 0) {
        if (AT_Ipv4AddrAtoi((VOS_CHAR *)g_atParaList[AT_PS_DIAL_PARAM_IP_ADDR].para, ipv4Addr,
                            sizeof(ipv4Addr)) != VOS_OK) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_SUCCESS;
}

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
VOS_UINT32 AT_PS_CheckDialRatType(VOS_UINT8 indexNum, VOS_UINT8 bitRatType)
{
    if (At_CheckCurrRatModeIsCL(indexNum) == VOS_TRUE) {
        switch (bitRatType) {
            case AT_PS_DIAL_RAT_TYPE_NO_ASTRICT:
            case AT_PS_DIAL_RAT_TYPE_1X_OR_HRPD:
            case AT_PS_DIAL_RAT_TYPE_LTE_OR_EHRPD:
                return VOS_TRUE;

            default:
                AT_NORM_LOG1("AT_PS_CheckDialRatType: Rat Type Error.\n", bitRatType);
                return VOS_FALSE;
        }
    } else {
        AT_NORM_LOG("AT_PS_CheckDialRatType: Not CL mode.\n");
        return VOS_FALSE;
    }
}
#endif

VOS_UINT32 AT_PS_ValidateDialParam(const VOS_UINT8 indexNum, const TAF_IFACE_UserTypeUint8 userType)
{
    VOS_UINT32 rst;

    /* ����������� */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_CMD_NO_PARA) {
        AT_NORM_LOG("AT_PS_ValidateDialParam: No parameter input.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���������� */
    rst = AT_PS_CheckDialParamCnt(userType);
    if (rst != AT_SUCCESS) {
        return rst;
    }

    /* ��� CID */
    if (g_atParaList[0].paraLen == 0) {
        AT_NORM_LOG("AT_PS_ValidateDialParam: Missing CID.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���� CONN: �ò�������ʡ��, 1��ʾ��������, 0��ʾ�Ͽ��Ͽ����� */
    if (g_atParaList[1].paraLen == 0) {
        AT_NORM_LOG("AT_PS_ValidateDialParam: Missing connect state.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��� APN */
    rst = AT_PS_CheckDialParamApn(g_atClientTab[indexNum].clientId);
    if (rst != AT_SUCCESS) {
        return rst;
    }

    /* ��� Username */
    if (g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].paraLen > TAF_MAX_AUTHDATA_USERNAME_LEN) {
        AT_NORM_LOG1("AT_PS_ValidateDialParam: Username length is.\n",
                     g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].paraLen);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��� Password */
    if (g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].paraLen > TAF_MAX_AUTHDATA_PASSWORD_LEN) {
        AT_NORM_LOG1("AT_PS_ValidateDialParam: Password length is.\n",
                     g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].paraLen);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ip addr��� */
    rst = AT_PS_CheckDialParamIpAddr();
    if (rst != AT_SUCCESS) {
        AT_NORM_LOG("AT_PS_ValidateDialParam: ip addr is invalid.");
        return rst;
    }

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (g_atParaList[AT_PS_DIAL_PARAM_RAT_TYPE].paraLen > 0) {
        if (AT_PS_CheckDialRatType(indexNum,
                                   (VOS_UINT8)g_atParaList[AT_PS_DIAL_PARAM_RAT_TYPE].paraValue) != VOS_TRUE) {
            AT_NORM_LOG1("AT_PS_ValidateDialParam: DialRatType is.\n",
                         g_atParaList[AT_PS_DIAL_PARAM_RAT_TYPE].paraValue);
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
#endif

    /* ���ͨ��ӳ�� */
    rst = AT_PS_CheckDialParamDataChanl(indexNum, userType);
    if (rst != AT_SUCCESS) {
        return rst;
    }

    return AT_SUCCESS;
}

TAF_PS_CallEndCauseUint8 AT_PS_TransCallEndCause(VOS_UINT8 connectType)
{
    TAF_PS_CallEndCauseUint8 cause = TAF_PS_CALL_END_CAUSE_NORMAL;

    if (connectType == TAF_PS_CALL_TYPE_DOWN_FORCE) {
        cause = TAF_PS_CALL_END_CAUSE_FORCE;
    }

    return cause;
}

VOS_UINT32 AT_PS_ProcIfaceDown(const VOS_UINT8 indexNum, const TAF_IFACE_UserTypeUint8 userType)
{
    AT_PS_DataChanlCfg *chanCfg = VOS_NULL_PTR;
    TAF_Ctrl            ctrl;
    TAF_IFACE_Deactive  ifaceDown;

    chanCfg = AT_PS_GetDataChanlCfg(indexNum, (VOS_UINT8)g_atParaList[0].paraValue);

    if ((chanCfg->used == VOS_FALSE) || (chanCfg->ifaceId == AT_PS_INVALID_IFACE_ID)) {
        AT_NORM_LOG("AT_PS_ProcIfaceDown: Iface is not act.");
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    /* ������ƽṹ�� */
    if (AT_PS_BuildIfaceCtrl(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &ctrl) == VOS_ERR) {
        return AT_ERROR;
    }

    /* ����ȥ������Ϣ��ֵ */
    (VOS_VOID)memset_s(&ifaceDown, sizeof(ifaceDown), 0x00, sizeof(ifaceDown));
    ifaceDown.ifaceId  = (VOS_UINT8)chanCfg->ifaceId;
    ifaceDown.cause    = AT_PS_TransCallEndCause((VOS_UINT8)g_atParaList[1].paraValue);
    ifaceDown.userType = userType;

    /* ��������ȥ������� */
    if (TAF_IFACE_Down(&ctrl, &ifaceDown) != VOS_OK) {
        AT_ERR_LOG("AT_PS_ProcIfaceDown: TAF_IFACE_Down is error.");
        return AT_ERROR;
    }

    return AT_SUCCESS;
}

/*
 * Function:      AT_CtrlGetPDPAuthType
 * Description:    ��ȡPC���õ�PDP�������ж�Ӧ���͵�����
 * Return:        0   no auth
 *                1   pap
 *                2   chap
 *   1.Date: 2009-08-03
 *    Modification: Created function
 */
PPP_AuthTypeUint8 AT_CtrlGetPDPAuthType(VOS_UINT32 value, VOS_UINT16 totalLen)
{
    /* ��ȡ��֤���� */
    if (totalLen == 0) {
        return TAF_PDP_AUTH_TYPE_NONE;
    } else {
        if (value == 0) {
            return TAF_PDP_AUTH_TYPE_NONE;
        } else if (value == 1) {
            return TAF_PDP_AUTH_TYPE_PAP;
        } else {
            return TAF_PDP_AUTH_TYPE_CHAP;
        }
    }
}

TAF_PDP_AuthTypeUint8 AT_ClGetPdpAuthType(VOS_UINT32 value, VOS_UINT16 totalLen)
{
    /* ��ȡ��֤���� */
    if (totalLen == 0) {
        return TAF_PDP_AUTH_TYPE_NONE;
    } else {
        switch (value) {
            case AT_PDP_AUTH_TYPE_NONE:
                return TAF_PDP_AUTH_TYPE_NONE;
            case AT_PDP_AUTH_TYPE_PAP:
                return TAF_PDP_AUTH_TYPE_PAP;
            case AT_PDP_AUTH_TYPE_CHAP:
                return TAF_PDP_AUTH_TYPE_CHAP;
            case AT_PDP_AUTH_TYPE_PAP_OR_CHAP:
                return TAF_PDP_AUTH_TYPE_PAP_OR_CHAP;
            default:
                return TAF_PDP_AUTH_TYPE_BUTT;
        }
    }
}

VOS_VOID AT_PS_GetUsrDialAuthType(VOS_UINT8 indexNum, TAF_IFACE_UserTypeUint8 userType,
                                  TAF_IFACE_DialParam *usrDialParam)
{
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* AUTH TYPE */
    if ((At_CheckCurrRatModeIsCL(indexNum) == VOS_TRUE) && (*systemAppConfig != SYSTEM_APP_WEBUI) &&
        (userType == TAF_IFACE_USER_TYPE_APP)) {
        usrDialParam->authType = AT_ClGetPdpAuthType(g_atParaList[AT_PS_USR_DIAL_AUTH_TYPE].paraValue,
                                                     g_atParaList[AT_PS_USR_DIAL_AUTH_TYPE].paraLen);
    } else
#endif
    {
        if (g_atParaList[AT_PS_USR_DIAL_AUTH_TYPE].paraLen > 0) {
            usrDialParam->authType = AT_CtrlGetPDPAuthType(g_atParaList[AT_PS_USR_DIAL_AUTH_TYPE].paraValue,
                                                           g_atParaList[AT_PS_USR_DIAL_AUTH_TYPE].paraLen);
        } else {
            usrDialParam->authType = TAF_PDP_AUTH_TYPE_NONE;

            /* ����û��������볤�Ⱦ���Ϊ0, �Ҽ�Ȩ����δ����, ��Ĭ��ʹ��CHAP���� */
            if ((g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].paraLen != 0) &&
                (g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].paraLen != 0)) {
                usrDialParam->authType = TAF_PDP_AUTH_TYPE_CHAP;
            }
        }
    }
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
VOS_VOID AT_PS_GetUsrDialNrPara(TAF_PDP_PrimContext *pdpCtxInfo, TAF_IFACE_DialParam *usrDialParam)
{
    usrDialParam->sscMode       = pdpCtxInfo->sscMode;
    usrDialParam->alwaysonType  = pdpCtxInfo->alwaysOnInd;
    usrDialParam->multiHomeIPv6 = pdpCtxInfo->mh6Pdu;
    usrDialParam->rQosFlag      = pdpCtxInfo->rQosInd;
    usrDialParam->multiHomeIPv6 = pdpCtxInfo->mh6Pdu;
    usrDialParam->accessDomain  = pdpCtxInfo->prefAccessType;

    /* �����Ƭ��Ϣ */
    if (pdpCtxInfo->sNssai.ucSst != TAF_PS_SNSSAI_SST_INVALID) {
        usrDialParam->bitOpSNssai = VOS_TRUE;
        usrDialParam->sNssai      = pdpCtxInfo->sNssai;
    }
}
#endif

VOS_UINT32 AT_CheckIpv6Capability(VOS_UINT8 pdpType)
{
    switch (AT_GetIpv6Capability()) {
        case AT_IPV6_CAPABILITY_IPV4_ONLY:
            /* IPv4 onlyֻ�ܷ���IPv4��PDP���� */
            if (pdpType != TAF_PDP_IPV4) {
                AT_ERR_LOG("AT_CheckIpv6Capablity: Only IPv4 is supported!");
                return VOS_ERR;
            }
            break;

        case AT_IPV6_CAPABILITY_IPV6_ONLY:
            /* IPv6 onlyֻ�ܷ���IPv6��PDP���� */
            if (pdpType != TAF_PDP_IPV6) {
                AT_ERR_LOG("AT_CheckIpv6Capablity: Only IPv6 is supported!");
                return VOS_ERR;
            }
            break;

        default:
            break;
    }

    return VOS_OK;
}

LOCAL VOS_UINT32 AT_CheckPdpTypeAndUserType(TAF_PDP_TypeUint8 pdpType, TAF_IFACE_UserTypeUint8 userType)
{
    switch (pdpType) {
        case TAF_PDP_IPV4:
        case TAF_PDP_IPV6:
        case TAF_PDP_IPV4V6:
            return AT_CheckIpv6Capability(pdpType);

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        case TAF_PDP_ETHERNET:
            /* APP��NDISͨ��֧��Ethernet���Ͳ��� */
            if ((AT_GetEthernetCap() == VOS_TRUE) &&
                ((userType == TAF_IFACE_USER_TYPE_APP) || (userType == TAF_IFACE_USER_TYPE_NDIS))) {
                return VOS_OK;
            }
            break;
#endif
        default :
            break;
    }

    return VOS_ERR;
}

/*
 * ��������: ��ȡ�û����ŵ�ipv4��ַ��Ϣ
 */
LOCAL VOS_VOID AT_PS_GetUsrDialIpv4AddrParam(TAF_IFACE_UserTypeUint8 userType, TAF_IFACE_DialParam *usrDialParam)
{
    VOS_UINT8          *systemAppConfig = VOS_NULL_PTR;
    errno_t             memResult;

    /* ֻ��E5���ź�NDIS���Ų���Ҫ��� */
    systemAppConfig = AT_GetSystemAppConfigAddr();
    if ((((userType == TAF_IFACE_USER_TYPE_APP) && (*systemAppConfig == SYSTEM_APP_WEBUI)) ||
         (userType == TAF_IFACE_USER_TYPE_NDIS)) &&
        ((g_atParaList[AT_PS_DIAL_PARAM_IP_ADDR].paraLen > 0) &&
         (g_atParaList[AT_PS_DIAL_PARAM_IP_ADDR].paraLen <= TAF_MAX_IPV4_ADDR_STR_LEN))) {
        usrDialParam->opIPv4ValidFlag = VOS_TRUE;
        memResult = memcpy_s((VOS_CHAR *)usrDialParam->ipv4Addr, sizeof(usrDialParam->ipv4Addr),
                             (VOS_CHAR *)g_atParaList[AT_PS_DIAL_PARAM_IP_ADDR].para,
                             g_atParaList[AT_PS_DIAL_PARAM_IP_ADDR].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(usrDialParam->ipv4Addr), g_atParaList[AT_PS_DIAL_PARAM_IP_ADDR].paraLen);
    }
}


VOS_VOID AT_GetMbbUsrDialPdpType(TAF_IFACE_DialParam *usrDialParam)
{
    VOS_UINT8           ipv6Capability;

    ipv6Capability = AT_GetIpv6Capability();
    if ((ipv6Capability == AT_IPV6_CAPABILITY_IPV4V6_OVER_ONE_PDP) ||
        (ipv6Capability == AT_IPV6_CAPABILITY_IPV4V6_OVER_TWO_PDP)) {
        usrDialParam->pdpType = TAF_PDP_IPV4V6;
    } else if (ipv6Capability == AT_IPV6_CAPABILITY_IPV6_ONLY) {
        usrDialParam->pdpType = TAF_PDP_IPV6;
    } else {
        usrDialParam->pdpType = TAF_PDP_IPV4;
    }
}

/*
 * ����˵��: �����û�����APN�����NDISDUPû��Я���Ļ����NV��ȡ
 * �������: index: �����·�ͨ��
 *           usrDialParam: ���Ų�������ṹָ��
 */
LOCAL VOS_VOID AT_PS_GetUserDialApn(VOS_UINT8 indexNum, TAF_IFACE_DialParam *usrDialParam)
{
    errno_t memResult;
    VOS_BOOL getCidTabFlag = VOS_FALSE;
#if (FEATURE_MBB_CUST == FEATURE_ON)
    VOS_UINT32 result = 0;
    TAF_PDP_PrimContext pdpCtxInfo;
#endif

    /* ���ָ���� */
    if (usrDialParam == VOS_NULL_PTR) {
        return;
    }

#if (FEATURE_MBB_CUST == FEATURE_ON)
    getCidTabFlag = (g_atParaIndex > AT_PS_DIAL_PARAM_APN) ? VOS_FALSE : VOS_TRUE;
#endif

    if (getCidTabFlag == VOS_FALSE) {
        usrDialParam->apnLen = (VOS_UINT8)g_atParaList[AT_PS_DIAL_PARAM_APN].paraLen;
        if ((g_atParaList[AT_PS_DIAL_PARAM_APN].paraLen > 0) &&
            (g_atParaList[AT_PS_DIAL_PARAM_APN].paraLen <= sizeof(usrDialParam->apn))) {
            memResult = memcpy_s(usrDialParam->apn, sizeof(usrDialParam->apn), g_atParaList[AT_PS_DIAL_PARAM_APN].para,
                                 g_atParaList[AT_PS_DIAL_PARAM_APN].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(usrDialParam->apn), g_atParaList[AT_PS_DIAL_PARAM_APN].paraLen);
        }
    }
#if (FEATURE_MBB_CUST == FEATURE_ON)
    else {
        /* ���NDISDUP������û��Я��APN��������C��NV�ж�ȡAPN */
        (VOS_VOID)memset_s(&pdpCtxInfo, sizeof(pdpCtxInfo), 0x00, sizeof(pdpCtxInfo));
        result = TAF_AGENT_GetPdpCidPara(&pdpCtxInfo, indexNum, usrDialParam->cid);
        if ((result == VOS_OK) && (pdpCtxInfo.apn.length > 0)) {
            pdpCtxInfo.apn.length = AT_MIN(pdpCtxInfo.apn.length, sizeof(pdpCtxInfo.apn.value));
            usrDialParam->apnLen = pdpCtxInfo.apn.length;

            memResult = memcpy_s(usrDialParam->apn, sizeof(usrDialParam->apn), pdpCtxInfo.apn.value,
                pdpCtxInfo.apn.length);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(usrDialParam->apn), pdpCtxInfo.apn.length);
        }
    }
#endif
}

VOS_UINT32 AT_PS_GetUsrDialParam(const VOS_UINT8 indexNum, const TAF_IFACE_UserTypeUint8 userType,
                                 TAF_IFACE_DialParam *usrDialParam)
{
    /* �ɵ����߱�֤��κͳ�����Ч�� */
    TAF_PDP_PrimContext pdpCtxInfo;
    VOS_UINT32          rslt;
    errno_t             memResult;

    /* User Type */
    usrDialParam->userType = userType;

    /* CID */
    usrDialParam->cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* APN MBB��Ʒ���NDISDUP���������û��APN��Ϣ����ȡCGDCONT���õ�APN */
    AT_PS_GetUserDialApn(indexNum, usrDialParam);

    /* Username */
    usrDialParam->usernameLen = (VOS_UINT8)g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].paraLen;
    if ((g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].paraLen > 0) &&
        (g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].paraLen <= sizeof(usrDialParam->username))) {
        memResult = memcpy_s(usrDialParam->username, sizeof(usrDialParam->username),
                             g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].para,
                             g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(usrDialParam->username),
                            g_atParaList[AT_PS_DIAL_PARAM_USER_NAME].paraLen);
    }

    /* Password */
    usrDialParam->passwordLen = (VOS_UINT8)g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].paraLen;
    if ((g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].paraLen > 0) &&
        (g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].paraLen <= sizeof(usrDialParam->password))) {
        memResult = memcpy_s(usrDialParam->password, sizeof(usrDialParam->password),
                             g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].para,
                             g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(usrDialParam->password),
                            g_atParaList[AT_PS_DIAL_PARAM_PASS_WORD].paraLen);
    }

    /* AUTH TYPE */
    AT_PS_GetUsrDialAuthType(indexNum, userType, usrDialParam);

    /* ADDR */
    AT_PS_GetUsrDialIpv4AddrParam(userType, usrDialParam);

    /* PDN TYPE */
    (VOS_VOID)memset_s(&pdpCtxInfo, sizeof(pdpCtxInfo), 0x00, sizeof(pdpCtxInfo));
    rslt = TAF_AGENT_GetPdpCidPara(&pdpCtxInfo, indexNum, usrDialParam->cid);

    if ((rslt == VOS_OK) && (AT_CheckPdpTypeAndUserType(pdpCtxInfo.pdpAddr.pdpType, userType) == VOS_OK)) {
        usrDialParam->pdpType = pdpCtxInfo.pdpAddr.pdpType;
    } else {
        if (AT_GetModemMbbCustCtxAddrFromClientId(indexNum)->dialStickFlg == VOS_TRUE) {
            AT_GetMbbUsrDialPdpType(usrDialParam);
        } else {
            AT_LOG1("AT_PS_GetUsrDialParam: PDP type is not supported.", pdpCtxInfo.pdpAddr.pdpType);
            return VOS_ERR;
        }
    }

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (g_atParaList[AT_PS_DIAL_PARAM_RAT_TYPE].paraLen > 0) {
        usrDialParam->ratType = (VOS_UINT8)g_atParaList[AT_PS_DIAL_PARAM_RAT_TYPE].paraValue;
    }

#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    AT_PS_GetUsrDialNrPara(&pdpCtxInfo, usrDialParam);
#endif

    return VOS_OK;
}

VOS_UINT32 AT_PS_ProcIfaceUp(const VOS_UINT8 indexNum, const TAF_IFACE_UserTypeUint8 userType)
{
    AT_PS_DataChanlCfg *chanCfg = VOS_NULL_PTR;
    TAF_Ctrl            ctrl;
    TAF_IFACE_Active    ifaceUp;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    /* ������ƽṹ�� */
    if (AT_PS_BuildIfaceCtrl(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &ctrl) == VOS_ERR) {
        AT_PS_SetPsCallErrCause(indexNum, TAF_PS_CAUSE_UNKNOWN);
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(&ifaceUp, sizeof(ifaceUp), 0x00, sizeof(ifaceUp));

    /* ����������Ϣ��ֵ */
    if (AT_PS_GetUsrDialParam(indexNum, userType, &(ifaceUp.usrDialParam)) == VOS_ERR) {
        AT_ERR_LOG("AT_PS_ProcIfaceUp: AT_PS_GetUsrDialParam is error.");
        AT_PS_SetPsCallErrCause(indexNum, TAF_PS_CAUSE_UNKNOWN);
        (VOS_VOID)memset_s(&ifaceUp, sizeof(ifaceUp), 0x00, sizeof(ifaceUp));
        return AT_ERROR;
    }

    chanCfg         = AT_PS_GetDataChanlCfg(indexNum, (VOS_UINT8)g_atParaList[0].paraValue);
    ifaceUp.ifaceId = (VOS_UINT8)chanCfg->ifaceId;

    /* ��������������� */
    if (TAF_IFACE_Up(&ctrl, &ifaceUp) != VOS_OK) {
        AT_ERR_LOG("AT_PS_ProcIfaceUp: TAF_IFACE_Up is error.");
        AT_PS_SetPsCallErrCause(indexNum, TAF_PS_CAUSE_UNKNOWN);
        (VOS_VOID)memset_s(&ifaceUp, sizeof(ifaceUp), 0x00, sizeof(ifaceUp));
        return AT_ERROR;
    }

    /* ���淢��IPFACE UP��PortIndex */
    chanCfg->portIndex = indexNum;
    (VOS_VOID)memset_s(&ifaceUp, sizeof(ifaceUp), 0x00, sizeof(ifaceUp));
    return AT_SUCCESS;
}

VOS_UINT32 AT_PS_ProcIfaceCmd(const VOS_UINT8 indexNum, const TAF_IFACE_UserTypeUint8 userType)
{
    VOS_UINT8          *systemAppConfig = VOS_NULL_PTR;
    AT_PS_DataChanlCfg *chanCfg         = VOS_NULL_PTR;
    VOS_UINT32          rst;

    if (g_atParaList[1].paraValue == TAF_PS_CALL_TYPE_UP) {
        systemAppConfig = AT_GetSystemAppConfigAddr();

        if ((userType == TAF_IFACE_USER_TYPE_APP) && (*systemAppConfig == SYSTEM_APP_WEBUI) &&
            (g_hiLinkMode == AT_HILINK_GATEWAY_MODE)) {
            /* ��¼PS����д����� */
            AT_PS_SetPsCallErrCause(indexNum, TAF_PS_CAUSE_UNKNOWN);

            /* ���ͨ��ӳ�� */
            chanCfg = AT_PS_GetDataChanlCfg(indexNum, (VOS_UINT8)g_atParaList[0].paraValue);
            AT_CleanDataChannelCfg(chanCfg);

            return AT_ERROR;
        }

        rst = AT_PS_ProcIfaceUp(indexNum, userType);

        /* MBB��̬����Ҫ���ͨ��ӳ�� */
        if ((rst != AT_SUCCESS) && (*systemAppConfig != SYSTEM_APP_ANDROID)) {
            /* ���ͨ��ӳ�� */
            chanCfg = AT_PS_GetDataChanlCfg(indexNum, (VOS_UINT8)g_atParaList[0].paraValue);
            AT_CleanDataChannelCfg(chanCfg);
        }
    } else {
        rst = AT_PS_ProcIfaceDown(indexNum, userType);
    }

    if (rst == AT_SUCCESS) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NDISDUP_SET;

        /* ������������״̬ */
        rst = AT_WAIT_ASYNC_RETURN;
    }

    return rst;
}

VOS_UINT32 AT_SetNdisdupPara(VOS_UINT8 indexNum)
{
    VOS_UINT32              rslt;
    TAF_IFACE_UserTypeUint8 userType;

    userType = AT_PS_GetUserType(indexNum);

    if (userType == TAF_IFACE_USER_TYPE_BUTT) {
        /* ��V2�汾�У������˿�����������ֱ�ӷ���OK */
        AT_ERR_LOG("AT_SetNdisdupPara: User Type is Butt!");
        return AT_OK;
    }

    /* ��������Ч�� */
    rslt = AT_PS_ValidateDialParam(indexNum, userType);

    if (rslt != AT_SUCCESS) {
        /* ��¼PS����д����� */
        AT_PS_SetPsCallErrCause(indexNum, TAF_PS_CAUSE_INVALID_PARAMETER);

        return rslt;
    }

    return AT_PS_ProcIfaceCmd(indexNum, userType);
}

TAF_UINT32 AT_SetDsFlowClrPara(TAF_UINT8 indexNum)
{
    TAF_DSFLOW_ClearConfig clearConfigInfo;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&clearConfigInfo, sizeof(clearConfigInfo), 0x00, sizeof(TAF_DSFLOW_ClearConfig));

    /* ���ò��� */
    clearConfigInfo.clearMode = TAF_DSFLOW_CLEAR_TOTAL;

    /* ִ��������� */
    if (TAF_PS_ClearDsFlowInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                               &clearConfigInfo) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DSFLOWCLR_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
 * ����˵��: ������ѯ������չ�ӿ�
 * �������: index: AT��������˿�
 * ���ؽ��: AT_ERROR: ʧ��
 *           AT_WAIT_ASYNC_RETURN: �첽����״̬
 */
VOS_UINT32 AT_SetDsFlowQryParaExt(VOS_UINT8 indexNum)
{
    VOS_UINT8 usrCid;

    /* ����Ǵ�CID�����ģ���ָ��CID��ѯ���� */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_PARA_CMD) {
        if (g_atParaList[0].paraLen == 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        usrCid = g_atParaList[0].para[0] - '0';
    } else if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_CMD_NO_PARA) {
        /* ����������ʾ��ѯ���е�CID���� */
        usrCid = TAF_MAX_CID;
    } else {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ִ��������� */
    if (TAF_PS_GetDsFlowInfo(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, usrCid, 0) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DSFLOWQRY_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}
#endif

TAF_UINT32 AT_SetDsFlowQryPara(TAF_UINT8 indexNum)
{
#if (FEATURE_MBB_CUST == FEATURE_ON)
    return AT_SetDsFlowQryParaExt(indexNum);
#else
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ִ��������� */
    if (TAF_PS_GetDsFlowInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DSFLOWQRY_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
#endif
}

TAF_UINT32 AT_SetDsFlowRptPara(TAF_UINT8 indexNum)
{
    TAF_DSFLOW_ReportConfig reportConfigInfo;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&reportConfigInfo, sizeof(reportConfigInfo), 0x00, sizeof(TAF_DSFLOW_ReportConfig));

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

    /* ���ò��� */
    reportConfigInfo.rptEnabled = g_atParaList[0].paraValue;


    if (reportConfigInfo.rptEnabled == VOS_TRUE) {
        /* ���������ϱ�����ʱ�� */
        reportConfigInfo.timerLength = 2000;
    }

    /* ִ��������� */
    if (TAF_PS_ConfigDsFlowRpt(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                               &reportConfigInfo) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DSFLOWRPT_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
#if (FEATURE_UE_MODE_G == FEATURE_ON)

TAF_UINT32 At_SetTrigPara(TAF_UINT8 indexNum)
{
    TAF_UINT8     i;
    TAF_GPRS_Data gprsDataInfo;

    (VOS_VOID)memset_s(&gprsDataInfo, sizeof(gprsDataInfo), 0x00, sizeof(gprsDataInfo));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != AT_TRIG_PARA_VALID_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    for (i = 0; i < AT_TRIG_PARA_VALID_NUM; i++) {
        if (g_atParaList[i].paraLen == 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    gprsDataInfo.mode = (TAF_UINT8)g_atParaList[0].paraValue;
    if ((gprsDataInfo.mode != 0) && (gprsDataInfo.mode != 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    if ((g_atParaList[AT_TRIG_NSAPI].paraValue >= AT_TRIG_NSAPI_MIN_VALUE) &&
        (g_atParaList[AT_TRIG_NSAPI].paraValue <= AT_TRIG_NSAPI_MAX_VALUE)) {
        gprsDataInfo.nsapi = (TAF_UINT8)g_atParaList[AT_TRIG_NSAPI].paraValue;
    } else {
        return AT_ERROR;
    }

    gprsDataInfo.length      = (TAF_UINT32)g_atParaList[AT_TRIG_LENGTH].paraValue;
    gprsDataInfo.times       = (TAF_UINT8)g_atParaList[AT_TRIG_TIMES].paraValue;
    gprsDataInfo.millisecond = (TAF_UINT32)g_atParaList[AT_TRIG_MILLISECOND].paraValue;

    /* ִ��������� */
    if (TAF_PS_TrigGprsData(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId),
                            g_atClientTab[indexNum].opId, &gprsDataInfo)) {
        return AT_ERROR;
    }

    return AT_OK;
}
#endif
#endif

VOS_UINT32 AT_SetChdataPara_AppUser(VOS_UINT8 indexNum)
{
    AT_CommPsCtx       *psCtx             = VOS_NULL_PTR;
    AT_PS_DataChanlCfg *dataChanCfg       = VOS_NULL_PTR;
    CONST AT_ChdataRnicRmnetId *chDataCfg = VOS_NULL_PTR;
    AT_CH_DataChannelUint32     dataChannelId;
    ModemIdUint16               modemId = MODEM_ID_0;
    VOS_UINT8                   cid;

    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        AT_WARN_LOG("AT_SetChdataPara_AppUser: get modem id error!");
        return AT_ERROR;
    }

    cid         = (VOS_UINT8)g_atParaList[0].paraValue;
    dataChanCfg = AT_PS_GetDataChanlCfg(indexNum, cid);

    /*
     * ָ��CID��PDP���Ѿ���������ڼ�������У�������ɾ�����޸ĸ�CID
     * ��ͨ��ӳ���ϵ��ֱ�ӷ���ERROR
     */
    if ((dataChanCfg->used == VOS_TRUE) &&
        ((dataChanCfg->ifaceActFlg == VOS_TRUE) || (dataChanCfg->portIndex < AT_CLIENT_ID_BUTT))) {
        AT_WARN_LOG("AT_SetChdataPara_AppUser: already act or acting!");
        return AT_ERROR;
    }

    /* �ڶ�������Ϊ�գ����ʾɾ�����ù�ϵ */
    if (g_atParaList[1].paraLen == 0) {
        AT_CleanDataChannelCfg(dataChanCfg);
        return AT_OK;
    }

    dataChannelId = g_atParaList[1].paraValue;

    /* ��ȡ����ID */
    chDataCfg = AT_PS_GetChDataCfgByChannelId(dataChannelId);
    if (chDataCfg == VOS_NULL_PTR) {
        AT_WARN_LOG("AT_SetChdataPara_AppUser: get chdata cfg fail!");
        return AT_ERROR;
    }

    psCtx = AT_GetCommPsCtxAddr();

    /* ���ͨ���Ƿ�ʹ�ù� */
    if (psCtx->channelCfg[dataChannelId].used == VOS_TRUE) {
        if (psCtx->channelCfg[dataChannelId].cid != cid) {
            AT_WARN_LOG("AT_SetChdataPara_AppUser: already other cid used!");
            return AT_ERROR;
        }

        if (psCtx->channelCfg[dataChannelId].modemId != modemId) {
            AT_WARN_LOG("AT_SetChdataPara_AppUser: already other modem used!");
            return AT_ERROR;
        }

        return AT_OK;
    }

    /* ɾ��ԭ�������� */
    AT_CleanDataChannelCfg(dataChanCfg);

    /* �����µ�����ͨ��ӳ��� */
    psCtx->channelCfg[dataChannelId].used    = VOS_TRUE;
    psCtx->channelCfg[dataChannelId].rmNetId = chDataCfg->rnicRmNetId;
    psCtx->channelCfg[dataChannelId].ifaceId = chDataCfg->ifaceId;
    psCtx->channelCfg[dataChannelId].cid     = cid;
    psCtx->channelCfg[dataChannelId].modemId = modemId;

    return AT_OK;
}

VOS_UINT32 AT_SetChdataPara(VOS_UINT8 indexNum)
{
    VOS_UINT8 userIndex;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CHDATA_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��һ������Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    userIndex = AT_GetUserIndexByClientIndex(indexNum);

    /* APPͨ���Ĵ��� */
    if (AT_CheckAppUser(userIndex) == VOS_TRUE) {
        return AT_SetChdataPara_AppUser(userIndex);
    }

    return AT_ERROR;
}

VOS_UINT32 At_SetIfaceDynamicParaComCheck(const VOS_UINT8 indexNum, TAF_Ctrl *ctrl)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������ƽṹ�� */
    if (AT_PS_BuildIfaceCtrl(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, ctrl) == VOS_ERR) {
        return AT_ERROR;
    }

    return VOS_OK;
}

VOS_UINT32 At_SetApConnStPara(VOS_UINT8 indexNum)
{
    TAF_Ctrl   ctrl;
    VOS_UINT32 bitCid;
    VOS_UINT32 rst;

    /* ������ƽṹ�� */
    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    rst = At_SetIfaceDynamicParaComCheck(indexNum, &ctrl);

    if (rst != VOS_OK) {
        return rst;
    }

    /* BitCid ���� */
    bitCid = (VOS_UINT32)(0x01UL << g_atParaList[0].paraValue);

    /* ���Ͳ�ѯ��Ϣ */
    if (TAF_IFACE_GetDynamicPara(&ctrl, bitCid) != VOS_OK) {
        AT_ERR_LOG("At_SetApConnStPara: TAF_IFACE_GetDynamicPara return ERROR!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_APCONNST_SET;

    return AT_WAIT_ASYNC_RETURN;
}

/*
 * Description: ^AUTHDATA=<cid>[,<Auth_type>[<PLMN>[,<passwd>[,<username>]]]
 * History:
 *  1.Date: 2009-08-27
 *    Modification: Created function
 */
TAF_UINT32 At_SetAuthdataPara(TAF_UINT8 indexNum)
{
    TAF_AUTHDATA_Ext authDataInfo;
    errno_t          memResult;

    (VOS_VOID)memset_s(&authDataInfo, sizeof(authDataInfo), 0x00, sizeof(TAF_AUTHDATA_Ext));

    /* ���޲�������ֱ�ӷ���OK */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_OK;
    }

    /* ������� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_AUTHDATA_PARA_MAX_NUM) {
        return AT_TOO_MANY_PARA;
    }

    authDataInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* �������ֻ��<CID>, �������������� */
    if (g_atParaIndex == 1) {
        authDataInfo.defined = VOS_FALSE;
    } else {
        authDataInfo.defined = VOS_TRUE;

        /* ����<Auth_type> */
        if (g_atParaList[1].paraLen != 0) {
            authDataInfo.opAuthType = VOS_TRUE;
            authDataInfo.authType   = (VOS_UINT8)g_atParaList[1].paraValue;
        }

        /* ����<PLMN> */
        if (g_atParaList[AT_AUTHDATA_PLMN].paraLen > TAF_MAX_AUTHDATA_PLMN_LEN) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (g_atParaList[AT_AUTHDATA_PLMN].paraLen != 0) {
            authDataInfo.opPlmn = VOS_TRUE;

            memResult = memcpy_s((VOS_CHAR *)authDataInfo.plmn, sizeof(authDataInfo.plmn),
                                 (VOS_CHAR *)g_atParaList[AT_AUTHDATA_PLMN].para,
                                 g_atParaList[AT_AUTHDATA_PLMN].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(authDataInfo.plmn), g_atParaList[AT_AUTHDATA_PLMN].paraLen);
            authDataInfo.plmn[g_atParaList[AT_AUTHDATA_PLMN].paraLen] = 0;
        }

        /* ����<username> */
        if (g_atParaList[AT_AUTHDATA_USERNAME_INDEX].paraLen > TAF_MAX_AUTHDATA_USERNAME_LEN) {
            (VOS_VOID)memset_s(&authDataInfo, sizeof(authDataInfo), 0x00, sizeof(TAF_AUTHDATA_Ext));
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (g_atParaList[AT_AUTHDATA_USERNAME_INDEX].paraLen != 0) {
            authDataInfo.opUserName = VOS_TRUE;

            memResult = memcpy_s((VOS_CHAR *)authDataInfo.userName, sizeof(authDataInfo.userName),
                                 (VOS_CHAR *)g_atParaList[AT_AUTHDATA_USERNAME_INDEX].para,
                                 g_atParaList[AT_AUTHDATA_USERNAME_INDEX].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(authDataInfo.userName),
                                g_atParaList[AT_AUTHDATA_USERNAME_INDEX].paraLen);
            authDataInfo.userName[g_atParaList[AT_AUTHDATA_USERNAME_INDEX].paraLen] = 0;
        }

        /* ����<passwd> */
        if (g_atParaList[AT_AUTHDATA_PASSWD_INDEX].paraLen > TAF_MAX_AUTHDATA_PASSWORD_LEN) {
            (VOS_VOID)memset_s(&authDataInfo, sizeof(authDataInfo), 0x00, sizeof(TAF_AUTHDATA_Ext));
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (g_atParaList[AT_AUTHDATA_PASSWD_INDEX].paraLen != 0) {
            authDataInfo.opPassWord = VOS_TRUE;

            memResult = memcpy_s((VOS_CHAR *)authDataInfo.passWord, sizeof(authDataInfo.passWord),
                                 (VOS_CHAR *)g_atParaList[AT_AUTHDATA_PASSWD_INDEX].para,
                                 g_atParaList[AT_AUTHDATA_PASSWD_INDEX].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(authDataInfo.passWord),
                                g_atParaList[AT_AUTHDATA_PASSWD_INDEX].paraLen);
            authDataInfo.passWord[g_atParaList[AT_AUTHDATA_PASSWD_INDEX].paraLen] = 0;
        }
    }

    /* ִ��������� */
    if (TAF_PS_SetAuthDataInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                               &authDataInfo) != VOS_OK) {
        (VOS_VOID)memset_s(&authDataInfo, sizeof(authDataInfo), 0x00, sizeof(TAF_AUTHDATA_Ext));
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_AUTHDATA_SET;
    (VOS_VOID)memset_s(&authDataInfo, sizeof(authDataInfo), 0x00, sizeof(TAF_AUTHDATA_Ext));
    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetCgpiafPara(VOS_UINT8 indexNum)
{
    AT_CommPsCtx *commPsCtx = VOS_NULL_PTR;

    commPsCtx = AT_GetCommPsCtxAddr();

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CGPIAF_MAX_PARA_NUM) {
        return AT_TOO_MANY_PARA;
    }

    /* ����<IPv6_AddressFormat> */
    if (g_atParaList[AT_CGPIAF_IPV6_ADDRFORMAT].paraLen != 0) {
        commPsCtx->opIpv6AddrFormat = (VOS_UINT8)g_atParaList[AT_CGPIAF_IPV6_ADDRFORMAT].paraValue;
    }

    /* ����<IPv6_SubnetNotation> */
    if (g_atParaList[AT_CGPIAF_IPV6_SUBNETNOTATION].paraLen != 0) {
        commPsCtx->opIpv6SubnetNotation = (VOS_UINT8)g_atParaList[AT_CGPIAF_IPV6_SUBNETNOTATION].paraValue;
    }

    /* ����<IPv6_LeadingZeros> */
    if (g_atParaList[AT_CGPIAF_IPV6_LEADINGZEROS].paraLen != 0) {
        commPsCtx->opIpv6LeadingZeros = (VOS_UINT8)g_atParaList[AT_CGPIAF_IPV6_LEADINGZEROS].paraValue;
    }

    /* ����<IPv6_CompressZeros> */
    if (g_atParaList[AT_CGPIAF_IPV6_LEADINGZEROS].paraLen != 0) {
        commPsCtx->opIpv6CompressZeros = (VOS_UINT8)g_atParaList[AT_CGPIAF_IPV6_COMPRESSZEROS].paraValue;
    }

    return AT_OK;
}

TAF_UINT32 At_SetCeerPara(TAF_UINT8 indexNum)
{
    VOS_UINT16 length;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ִ��������� */
    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "%s: %d,%d", g_parseContext[indexNum].cmdElement->cmdName,
        AT_GetCsCallErrCause(indexNum), AT_PS_GetPsCallErrCause(indexNum));

    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}

VOS_UINT32 AT_SetCgerepPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CGEREP_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* +CGEREP=[<mode>[,<bfr>]] */
    if (g_atParaIndex == 0) {
        netCtx->cgerepCfg.mode = 0;
        netCtx->cgerepCfg.bfr  = 0;
    } else if (g_atParaIndex == 1) {
        if (g_atParaList[0].paraLen == 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        netCtx->cgerepCfg.mode = (VOS_UINT8)g_atParaList[0].paraValue;
        netCtx->cgerepCfg.bfr  = 0;
    } else {
        if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        netCtx->cgerepCfg.mode = (VOS_UINT8)g_atParaList[0].paraValue;
        netCtx->cgerepCfg.bfr  = (VOS_UINT8)g_atParaList[1].paraValue;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetCgmtuPara(VOS_UINT8 indexNum)
{
    TAF_Ctrl   ctrl;
    VOS_UINT32 bitCid;
    VOS_UINT32 rst;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    rst = At_SetIfaceDynamicParaComCheck(indexNum, &ctrl);

    if (rst != VOS_OK) {
        return rst;
    }

    bitCid = (VOS_UINT32)(0x01UL << g_atParaList[0].paraValue);

    /* ���Ͳ�ѯ��Ϣ */
    if (TAF_IFACE_GetDynamicPara(&ctrl, bitCid) != VOS_OK) {
        AT_ERR_LOG("AT_SetCgmtuPara: TAF_IFACE_GetDynamicPara return ERROR!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGMTU_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetRoamPdpTypePara(VOS_UINT8 indexNum)
{
    TAF_PS_RoamingPdpTypeInfo pdpCxtInfo;
    VOS_UINT32                rslt;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&pdpCxtInfo, sizeof(pdpCxtInfo), 0, sizeof(pdpCxtInfo));

    /* ָ�����ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_ROAMPDPTYPEPARA_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<CID> */
    pdpCxtInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* �������ֻ��<CID>, �������������� */
    if (g_atParaIndex == 1) {
        pdpCxtInfo.defined = VOS_FALSE;
    } else {
        /* ������ip���� �����ʹ���ֱ�ӷ��� */
        if ((g_atParaList[1].paraLen == 0) || (g_atParaList[1].paraValue >= TAF_PDP_IPV4V6)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        pdpCxtInfo.defined = VOS_TRUE;

        /* ����<PDP_type> IP:1, IPV6:2, IPV4V6:3 */
        pdpCxtInfo.roamingPdpType = (VOS_UINT8)(g_atParaList[1].paraValue + 1);
    }

    rslt = TAF_PS_SetRoamPdpType(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &pdpCxtInfo);
    /* ִ��������� */
    if (rslt != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_ROAMPDPTYPE_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetSinglePdnSwitch(VOS_UINT8 indexNum)
{
    /* ָ�����ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_PS_SetSinglePdnSwitch(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                  (VOS_UINT8)g_atParaList[0].paraValue) != VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SINGLEPDNSWITCH_SET;
    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
 * ����˵��: AT^DSAMBR��ѯ�������
 * �������: index: AT�·�ͨ��
 * ���ؽ��: AT_WAIT_ASYNC_RETURN: �첽�ȴ����
 *           AT_ERROR: ����ʧ��
 */
VOS_UINT32 AT_SetDsambrPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 result;
    VOS_UINT8 cid = 0;

    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[0].paraLen != 0) && (g_atParaList[0].paraValue == 1)) {
        cid = (VOS_UINT8)g_atParaList[0].paraValue;
    } else {
        /* ��cid���� */
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����DSAMBR��ѯ����DSM */
    result = TAF_PS_SendDsambrQryReq(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId),
        g_atClientTab[indexNum].opId, cid);
    if (result != VOS_OK) {
        return AT_ERROR;
    }
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DSAMBR_QRY;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 At_SetDhcpPara(VOS_UINT8 indexNum)
{
    TAF_Ctrl   ctrl;
    VOS_UINT32 bitCid;
    VOS_UINT32 rst;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    rst = At_SetIfaceDynamicParaComCheck(indexNum, &ctrl);

    if (rst != VOS_OK) {
        return rst;
    }

    /* BitCid ���� */
    bitCid = (VOS_UINT32)(0x01UL << g_atParaList[0].paraValue);

    /* ���Ͳ�ѯ��Ϣ */
    if (TAF_IFACE_GetDynamicPara(&ctrl, bitCid) != VOS_OK) {
        AT_ERR_LOG("At_SetDhcpPara: TAF_IFACE_GetDynamicPara return ERROR!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DHCP_SET;

    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
 * ����˵��: AT^NDISSTATQRY���������ӿ�
 * �������: index: AT��������˿�
 * ���ؽ��: AT_ERROR: ʧ��
 *           AT_OK: �ɹ�
 */
VOS_UINT32 AT_SetNdisStatQryPara(VOS_UINT8 indexNum)
{
    TAF_Ctrl ctrl;
    VOS_UINT32 bitCid;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    /* ������ƽṹ�� */
    if (AT_PS_BuildIfaceCtrl(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &ctrl) == VOS_ERR) {
        return AT_ERROR;
    }

    /* ���������CID����BitMask */
    bitCid = (VOS_UINT32)(0x01UL << g_atParaList[0].paraValue);

    /* ���ò�ѯ���̣����Ͳ�ѯ��Ϣ */
    if (TAF_IFACE_GetDynamicPara(&ctrl, bitCid) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���������ò�ѯ����Ĺ��� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NDISSTATQRY_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_SetDhcpv6Para(VOS_UINT8 indexNum)
{
    TAF_Ctrl   ctrl;
    VOS_UINT32 bitCid;
    VOS_UINT32 rst;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    rst = At_SetIfaceDynamicParaComCheck(indexNum, &ctrl);

    if (rst != VOS_OK) {
        return rst;
    }

    /* BitCid ���� */
    bitCid = (VOS_UINT32)(0x01UL << g_atParaList[0].paraValue);

    /* ���Ͳ�ѯ��Ϣ */
    if (TAF_IFACE_GetDynamicPara(&ctrl, bitCid) != VOS_OK) {
        AT_ERR_LOG("AT_SetDhcpv6Para: TAF_IFACE_GetDynamicPara return ERROR!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DHCPV6_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 At_SndReleaseRrcReq(VOS_UINT16 clientId, VOS_UINT8 opId)
{
    AT_RABM_ReleaseRrcReq *sndMsg = VOS_NULL_PTR;
    VOS_UINT32             rslt;

    /* �����ڴ�  */
    /*lint -save -e516 */
    sndMsg = (AT_RABM_ReleaseRrcReq *)TAF_AllocMsgWithoutHeaderLen(WUEPS_PID_AT, sizeof(AT_RABM_ReleaseRrcReq) - VOS_MSG_HEAD_LENGTH);
    /*lint -restore */
    if (sndMsg == VOS_NULL_PTR) {
        /* �ڴ�����ʧ�� */
        AT_ERR_LOG("At_SndReleaseRrcReq:ERROR: Memory Alloc Error for pstMsg");
        return VOS_ERR;
    }

    /* ��д��ز��� */
    TAF_CfgMsgHdr((MsgBlock *)sndMsg, WUEPS_PID_AT, AT_GetDestPid(clientId, I0_WUEPS_PID_RABM),
                  sizeof(AT_RABM_ReleaseRrcReq) - VOS_MSG_HEAD_LENGTH);

    sndMsg->msgHeader.msgName = ID_AT_RABM_SET_RELEASE_RRC_REQ;
    sndMsg->clientId          = clientId;
    sndMsg->opId              = opId;

    /* ����VOS����ԭ�� */
    rslt = TAF_TraceAndSendMsg(WUEPS_PID_AT, sndMsg);
    if (rslt != VOS_OK) {
        AT_ERR_LOG("At_SndReleaseRrcReq:ERROR:TAF_TraceAndSendMsg ");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 At_SetReleaseRrc(VOS_UINT8 indexNum)
{
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    /* ��ȡNV���е�ǰ��Ʒ��̬ */
    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* ��ANDROIDϵͳ��֧�� */
    if (*systemAppConfig != SYSTEM_APP_ANDROID) {
        return AT_CMD_NOT_SUPPORT;
    }

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �·����� */
    if (At_SndReleaseRrcReq(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId) == VOS_OK) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_RELEASE_RRC_SET;

        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetApRaInfoPara(VOS_UINT8 indexNum)
{
    TAF_Ctrl   ctrl;
    VOS_UINT32 bitCid;
    VOS_UINT32 rst;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    rst = At_SetIfaceDynamicParaComCheck(indexNum, &ctrl);

    if (rst != VOS_OK) {
        return rst;
    }

    /* BitCid ���� */
    bitCid = (VOS_UINT32)(0x01UL << g_atParaList[0].paraValue);

    /* ���Ͳ�ѯ��Ϣ */
    if (TAF_IFACE_GetDynamicPara(&ctrl, bitCid) != VOS_OK) {
        AT_ERR_LOG("AT_SetApRaInfoPara: TAF_IFACE_GetDynamicPara return ERROR!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_APRAINFO_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 At_SetUsbTetherInfo(VOS_UINT8 indexNum)
{
    AT_RNIC_UsbTetherInfoInd *msg = VOS_NULL_PTR;
    errno_t                   memResult;

    /* ָ�����ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ŀ���� */
    if (g_atParaIndex != AT_USBTETHERINFO_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0) ||
        (g_atParaList[1].paraLen > RNIC_RMNET_NAME_MAX_LEN)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    AT_WARN_LOG1("At_SetUsbTetherInfo: para2 len is ", g_atParaList[1].paraLen);

    msg = (AT_RNIC_UsbTetherInfoInd *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_RNIC_UsbTetherInfoInd));

    if (msg == VOS_NULL_PTR) {
        AT_WARN_LOG("At_SetUsbTetherInfo: alloc msg fail!");
        return AT_ERROR;
    }

    /* ��ʼ����Ϣ�� */
    (VOS_VOID)memset_s((VOS_INT8 *)msg + VOS_MSG_HEAD_LENGTH,
                       (VOS_SIZE_T)sizeof(AT_RNIC_UsbTetherInfoInd) - VOS_MSG_HEAD_LENGTH, 0x00,
                       (VOS_SIZE_T)sizeof(AT_RNIC_UsbTetherInfoInd) - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ����ϢID */
    AT_CFG_MSG_HDR(msg, ACPU_PID_RNIC, ID_AT_RNIC_USB_TETHER_INFO_IND);

    /* ��д��Ϣ�� */
    msg->tetherConnStat = (VOS_UINT8)g_atParaList[0].paraValue;
    memResult = memcpy_s(msg->rmnetName, RNIC_RMNET_NAME_MAX_LEN, g_atParaList[1].para, g_atParaList[1].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, RNIC_RMNET_NAME_MAX_LEN, g_atParaList[1].paraLen);

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("At_SetUsbTetherInfo: Send msg fail!");
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetRmnetCfg(VOS_UINT8 indexNum)
{
    AT_RNIC_RmnetCfgReq *msg = VOS_NULL_PTR;
    errno_t memResult;

    /* ָ�����ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_ERR_LOG("AT_SetRmnetCfg: cmdOptType is not AT_CMD_OPT_SET_PARA_CMD!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ŀ���� */
    if (g_atParaIndex != AT_RMNETCFG_PARA_NUM) {
        AT_ERR_LOG1("AT_SetRmnetCfg: para number error, numer is !", g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���������쳣 */
    if (g_atParaList[1].paraLen > RNIC_RMNET_NAME_MAX_LEN) {
        AT_ERR_LOG1("AT_SetRmnetCfg: para len error, len of para 1 is !", g_atParaList[1].paraLen);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    msg = (AT_RNIC_RmnetCfgReq *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_RNIC_RmnetCfgReq));

    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetRmnetCfg: alloc msg fail!");
        return AT_ERROR;
    }

    /* ��ʼ����Ϣ�� */
    (VOS_VOID)memset_s(AT_GET_MSG_ENTITY(msg), AT_GET_MSG_LENGTH(msg), 0x00,
                       AT_GET_MSG_LENGTH(msg));

    /* ��д��Ϣͷ����ϢID */
    AT_CFG_RNIC_MSG_HDR(msg, ID_AT_RNIC_RMNET_CFG_REQ);

    /* ��д��Ϣ�� */
    msg->clientId = g_atClientTab[indexNum].clientId;
    msg->featureMode = (VOS_UINT8)g_atParaList[0].paraValue;
    if (g_atParaList[1].paraLen > 0) {
        memResult = memcpy_s(msg->rmnetName, RNIC_RMNET_NAME_MAX_LEN, g_atParaList[1].para,
                             g_atParaList[1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, RNIC_RMNET_NAME_MAX_LEN, g_atParaList[1].paraLen);
    }
    msg->weight = (VOS_UINT8)g_atParaList[2].paraValue;

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_ERR_LOG("AT_SetRmnetCfg: Send msg fail!");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_RMNETCFG_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetIPv6TempAddrPara(VOS_UINT8 indexNum)
{
    TAF_Ctrl   ctrl;
    VOS_UINT32 bitCid;
    VOS_UINT32 rst;

    /* ������ƽṹ�� */
    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    rst = At_SetIfaceDynamicParaComCheck(indexNum, &ctrl);

    if (rst != VOS_OK) {
        return rst;
    }

    if (AT_GetPsIPv6IIDTestModeConfig() != IPV6_ADDRESS_TEST_MODE_ENABLE) {
        AT_ERR_LOG("AT_PS_ReportIPv6TempAddr: Test mode is disabled!");
        return AT_CME_OPERATION_NOT_ALLOWED;
    }

    /* BitCid ���� */
    bitCid = (VOS_UINT32)(0x01UL << g_atParaList[0].paraValue);

    /* ���Ͳ�ѯ��Ϣ */
    if (TAF_IFACE_GetDynamicPara(&ctrl, bitCid) != VOS_OK) {
        AT_ERR_LOG("AT_SetIPv6TempAddrPara: TAF_IFACE_GetDynamicPara return ERROR!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_IPV6TEMPADDR_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetOpwordParaForApModem(VOS_UINT8 indexNum)
{
    return AT_OK;
}

VOS_UINT32 AT_SetOpwordPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 checkRlst       = VOS_FALSE;
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;
    VOS_UINT32 result;

    /* ��ȡNV���е�ǰ��Ʒ��̬ */
    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* ������� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_ERROR;
    }

    /* �Ѿ�����Ȩ��  ֱ�ӷ���OK */
    if (g_ate5RightFlag == AT_E5_RIGHT_FLAG_YES) {
        return AT_OK;
    }

    /* ����ǰ�豸����̬ΪAP-MODEM */
    if (*systemAppConfig == SYSTEM_APP_ANDROID) {
        result = AT_SetOpwordParaForApModem(indexNum);
        return result;
    }

    /* �������볬�� */
    if (g_atParaList[0].paraLen > AT_DISLOG_PWD_LEN) {
        return AT_ERROR;
    }

    /* ����Ƚ� */
    if (VOS_StrLen((VOS_CHAR *)g_atOpwordPwd) == 0) {
        /* NV������Ϊ���������κ����붼�ܻ�ȡȨ�� */
        checkRlst = VOS_TRUE;
    } else {
        if (VOS_StrCmp((VOS_CHAR *)g_atOpwordPwd, (VOS_CHAR *)g_atParaList[0].para) == 0) {
            checkRlst = VOS_TRUE;
        }
    }

    if (checkRlst == VOS_FALSE) {
        return AT_ERROR;
    }

    /* ����ȶԳɹ�  ��ȡȨ�� */
    g_ate5RightFlag = AT_E5_RIGHT_FLAG_YES;

    /* ��D25������  ������Ȩ�ޱ�־ */

    return AT_OK;
}

VOS_UINT32 AT_SetCpwordPara(VOS_UINT8 indexNum)
{
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �Ѿ�������Ȩ�� */
    if (g_ate5RightFlag == AT_E5_RIGHT_FLAG_NO) {
        return AT_OK;
    }

    /* �ͷ�Ȩ�� */
    g_ate5RightFlag = AT_E5_RIGHT_FLAG_NO;

    /* ��D25������  ������Ȩ�ޱ�־ */

    return AT_OK;
}

VOS_UINT32 AT_OpenDiagPort(VOS_VOID)
{
    AT_DynamicPidType dynamicPidType;
    VOS_UINT32        portPos;
    VOS_UINT32        portNum;

    memset_s(&dynamicPidType, sizeof(dynamicPidType), 0x00, sizeof(dynamicPidType));

    portPos = 0;
    portNum = 0;

    /* ��NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE��ȡ��ǰ�Ķ˿�״̬ */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, &dynamicPidType, sizeof(AT_DynamicPidType)) !=
        NV_OK) {
        AT_WARN_LOG("AT_OpenDiagPort: Read NV fail!");
        return AT_ERROR;
    }

    /* �ж�DIAG�˿��Ƿ��Ѿ���: �Ѿ�����ֱ�ӷ���AT_OK */
    if (dynamicPidType.nvStatus == VOS_TRUE) {
        /* ��ѯNV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE���Ƿ��Ѿ�����DIAG�� */
        AT_GetSpecificPort(AT_DEV_DIAG, dynamicPidType.rewindPortStyle, &portPos, &portNum);

        if (portPos != AT_DEV_NONE) {
            return AT_OK;
        }

    } else {
        AT_WARN_LOG("AT_OpenDiagPort: NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE is inactive!");
        return AT_OK;
    }

    /* DIAG�˿ڲ���Ȩ��δ��ȡ: ֱ�ӷ���AT_OK */
    if (g_ate5RightFlag == AT_E5_RIGHT_FLAG_NO) {
        return AT_OK;
    }

    /* ׷��DIAG�˿ڵ��л���˿ڼ� */
    if (portNum == AT_SETPORT_PARA_MAX_LEN) {
        return AT_OK;
    }

    dynamicPidType.rewindPortStyle[portNum] = AT_DEV_DIAG;

    /* ���¶˿ڼ������ݵ�NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, (VOS_UINT8 *)&dynamicPidType,
                           sizeof(AT_DynamicPidType)) != NV_OK) {
        AT_ERR_LOG("AT_OpenDiagPort: Write NV fail");
        return AT_ERROR;
    } else {
        return AT_OK;
    }
}

VOS_UINT32 AT_CloseDiagPort(VOS_VOID)
{
    AT_DynamicPidType dynamicPidType;
    VOS_UINT32        portPos;
    VOS_UINT32        portNum;
    VOS_UINT32        loop;

    memset_s(&dynamicPidType, sizeof(dynamicPidType), 0x00, sizeof(dynamicPidType));

    portPos = 0;
    portNum = 0;

    /* ��NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE��ȡ��ǰ�Ķ˿�״̬ */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, &dynamicPidType, sizeof(AT_DynamicPidType)) !=
        NV_OK) {
        AT_ERR_LOG("AT_CloseDiagPort: Read NV fail!");
        return AT_ERROR;
    }

    /* �ж�DIAG�˿��Ƿ��Ѿ���: �Ѿ�����ֱ�ӷ���AT_OK */
    if (dynamicPidType.nvStatus == VOS_TRUE) {
        /* ��ѯNV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE���Ƿ��Ѿ�����DIAG�� */
        AT_GetSpecificPort(AT_DEV_DIAG, dynamicPidType.rewindPortStyle, &portPos, &portNum);

        if (portPos == AT_DEV_NONE) {
            return AT_OK;
        }
    } else {
        return AT_OK;
    }

    /* DIAG�˿ڲ���Ȩ��δ��ȡ: ֱ�ӷ���AT_OK */
    if (g_ate5RightFlag == AT_E5_RIGHT_FLAG_NO) {
        return AT_OK;
    }

    /* ɾ��NV���е�DIAG�˿� */
    dynamicPidType.rewindPortStyle[portPos] = 0;
    portNum--;
    for (loop = portPos; loop < portNum; loop++) {
        dynamicPidType.rewindPortStyle[loop] = dynamicPidType.rewindPortStyle[loop + 1UL];
    }
    dynamicPidType.rewindPortStyle[portNum] = 0;

    /* �˿��쳣���ݱ���: �л�����豸��̬�У���һ���豸����ΪMASS�豸(0xa1,0xa2) */
    if (portNum != 0) {
        if ((dynamicPidType.rewindPortStyle[0] == AT_DEV_CDROM) || (dynamicPidType.rewindPortStyle[0] == AT_DEV_SD)) {
            return AT_OK;
        }
    }

    /* ���¶˿ڼ������ݵ�NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, (VOS_UINT8 *)&dynamicPidType,
                           sizeof(AT_DynamicPidType)) != NV_OK) {
        AT_ERR_LOG("AT_CloseDiagPort: Write NV fail");
        return AT_ERROR;
    } else {
        return AT_OK;
    }
}

VOS_UINT32 AT_SetDislogPara(VOS_UINT8 indexNum)
{
    VOS_UINT32        ret;
    NV_PID_EnableType pidEnableType;

#if (FEATURE_LTE == FEATURE_ON)
    VOS_UINT32 retDiag;
    VOS_UINT32 ret3GDiag;
    VOS_UINT32 retGps;
#endif

    pidEnableType.pidEnabled = VOS_FALSE;

    /* ������� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaIndex > 1)) {
        return AT_ERROR;
    }

    /* ��ȡPIDʹ��NV�� */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_PID_ENABLE_TYPE, &pidEnableType, sizeof(NV_PID_EnableType)) != NV_OK) {
        AT_ERR_LOG("AT_SetDislogPara:Read NV failed!");
        return AT_ERROR;
    }

    /* NV��ȡ�ɹ������PID�Ƿ�ʹ�ܣ�����ʹ�ܣ�����AT_OK */
    if (pidEnableType.pidEnabled != VOS_TRUE) {
        AT_WARN_LOG("AT_SetDislogPara:PID is not enabled");
        return AT_OK;
    }

    /*  ��DIAG�� */
    if (g_atParaList[0].paraValue == AT_DISLOG_DIAG_OPEN) {
#if (FEATURE_LTE == FEATURE_ON)
        ret3GDiag = AT_OpenSpecificPort(AT_DEV_DIAG);
        retDiag   = AT_OpenSpecificPort(AT_DEV_4G_DIAG);
        retGps    = AT_OpenSpecificPort(AT_DEV_4G_GPS);

        if ((retDiag == AT_OK) && (retGps == AT_OK) && (ret3GDiag == AT_OK)) {
            ret = AT_OK;
        } else {
            ret = AT_ERROR;
        }
#else
        ret = AT_OpenDiagPort();
#endif
        return ret;
    }

    /* �ر�DIAG�� */
    if (g_atParaList[0].paraValue == AT_DISLOG_DIAG_CLOSE) {
#if (FEATURE_LTE == FEATURE_ON)
        ret3GDiag = AT_CloseSpecificPort(AT_DEV_DIAG);
        retDiag   = AT_CloseSpecificPort(AT_DEV_4G_DIAG);
        retGps    = AT_CloseSpecificPort(AT_DEV_4G_GPS);

        if ((retDiag == AT_OK) && (retGps == AT_OK) && (ret3GDiag == AT_OK)) {
            ret = AT_OK;
        } else {
            ret = AT_ERROR;
        }

#else
        ret = AT_CloseDiagPort();
#endif
        return ret;
    }

    return AT_ERROR;
}

#if (FEATURE_SECURITY_SHELL == FEATURE_ON)
VOS_UINT32 AT_SetShellPara(VOS_UINT8 indexNum)
{
    AT_ShellOpenFlagUint32 tmpOpenFlag;

    /* ������� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_ERROR;
    }

    /*  WIFIʹ��SHELL�� */
    if (g_atParaList[0].paraValue == AT_SHELL_WIFI_USE) {
        /* ����WIFIʹ��SHELL�����NV�� */
        tmpOpenFlag = AT_SHELL_OPEN_FLAG_WIFI;
        if (TAF_ACORE_NV_WRITE(MODEM_ID_0, en_NV_Item_AT_SHELL_OPEN_FLAG, (VOS_UINT8 *)&tmpOpenFlag,
                               sizeof(tmpOpenFlag)) != NV_OK) {
            AT_WARN_LOG("AT_SetShellPara:WARNING:TAF_ACORE_NV_WRITE SHELL_OPEN_FLAG faild!");
            return AT_ERROR;
        }

        return AT_OK;
    }

    /*  ��SHELL�� */
    if (g_atParaList[0].paraValue == AT_SHELL_OPEN) {
        /* �ж�CK�Ƿ�У��ͨ�� */
        if (g_spWordCtx.shellPwdCheckFlag != VOS_TRUE) {
            AT_WARN_LOG("AT_SetShellPara:WARNING:TAF_ACORE_NV_WRITE SHELL_OPEN_FLAG faild!");

            return AT_ERROR;
        }

        /* ����򿪽����NV�� */
        tmpOpenFlag = AT_SHELL_OPEN_FLAG_OPEN;
        if (TAF_ACORE_NV_WRITE(MODEM_ID_0, en_NV_Item_AT_SHELL_OPEN_FLAG, (VOS_UINT8 *)&tmpOpenFlag,
                               sizeof(tmpOpenFlag)) != NV_OK) {
            AT_WARN_LOG("AT_SetShellPara:WARNING:TAF_ACORE_NV_WRITE SHELL_OPEN_FLAG faild!");

            return AT_ERROR;
        }

        return AT_OK;
    }

    /* �ر�SHELL�� */
    if (g_atParaList[0].paraValue == AT_SHELL_CLOSE) {
        /* ���浱ǰSHELL�ڵĹر�״̬��NV�� */
        tmpOpenFlag = AT_SHELL_OPEN_FLAG_CLOSE;
        if (TAF_ACORE_NV_WRITE(MODEM_ID_0, en_NV_Item_AT_SHELL_OPEN_FLAG, (VOS_UINT8 *)&tmpOpenFlag,
                               sizeof(tmpOpenFlag)) != NV_OK) {
            AT_WARN_LOG("AT_SetShellPara:WARNING:TAF_ACORE_NV_WRITE SHELL_OPEN_FLAG faild!");
            return AT_ERROR;
        }

        return AT_OK;
    }

    return AT_ERROR;
}
#endif

VOS_UINT16 AT_CalcIpHdrCRC16(VOS_UINT8 *data, VOS_UINT16 size)
{
    VOS_UINT8  *buffer = data;
    VOS_UINT32  checkSum = 0;

    while (size > 1) {
        checkSum += TAF_GET_HOST_UINT16(buffer);
        buffer += sizeof(VOS_UINT16);
        size -= sizeof(VOS_UINT16);
    }

    if (size) {
#if (VOS_LITTLE_ENDIAN == VOS_BYTE_ORDER)
        checkSum += *buffer;
#else
        checkSum += 0 | ((*buffer) << 8);
#endif
    }

    checkSum = (checkSum >> 16) + (checkSum & 0xffff);
    checkSum += (checkSum >> 16);

    return (VOS_UINT16)(~checkSum);
}

ATTRIBUTE_NO_SANITIZE_RUNTIME VOS_UINT32 AT_BuildUdpHdr(AT_UdpPacketFormat *udpPkt, VOS_UINT16 len, VOS_UINT32 srcAddr,
                                                        VOS_UINT32 dstAddr, VOS_UINT16 srcPort, VOS_UINT16 dstPort)
{
    static VOS_UINT16 identification = 0;

    /* ���ָ��Ϸ��� */
    if (udpPkt == VOS_NULL_PTR) {
        return VOS_ERR;
    }

    /* ��дIPͷ */
    udpPkt->ipHdr.ucIpVer        = AT_IP_VERSION;
    udpPkt->ipHdr.ucIpHdrLen     = AT_IP_HEAD_LEN;
    udpPkt->ipHdr.serviceType    = 0x00;
    udpPkt->ipHdr.totalLen       = VOS_HTONS(len + AT_IP_HDR_LEN + AT_UDP_HDR_LEN);
    udpPkt->ipHdr.identification = VOS_HTONS(identification);
    udpPkt->ipHdr.offset         = 0;
    udpPkt->ipHdr.tTL            = AT_IP_DEF_TTL;
    udpPkt->ipHdr.protocol       = AT_IPPROTO_UDP;
    udpPkt->ipHdr.srcAddr        = VOS_HTONL(srcAddr);
    udpPkt->ipHdr.dstAddr        = VOS_HTONL(dstAddr);
    udpPkt->ipHdr.checkSum       = AT_CalcIpHdrCRC16((VOS_UINT8 *)&udpPkt->ipHdr, AT_IP_HDR_LEN);

    /* ��дUDPͷ */
    udpPkt->udpHdr.srcPort  = VOS_HTONS(srcPort);
    udpPkt->udpHdr.dstPort  = VOS_HTONS(dstPort);
    udpPkt->udpHdr.len      = VOS_HTONS(len + AT_UDP_HDR_LEN);
    udpPkt->udpHdr.checksum = 0;

    identification++;

    return VOS_OK;
}

IMM_Zc* At_CsndBuildUdpPacket(const VOS_UINT32 ipAddr, const VOS_UINT16 length)
{
    IMM_Zc    *immZc = VOS_NULL_PTR;
    VOS_UINT8 *data  = VOS_NULL_PTR;
    VOS_UINT32 srcAddr;
    VOS_UINT32 destAddr;
    VOS_UINT16 destPort;
    VOS_UINT16 srcPort;
    VOS_UINT16 totalLen;
    VOS_UINT16 macHeaderLen;

    totalLen = length + AT_IP_HDR_LEN + AT_UDP_HDR_LEN;
#if (FEATURE_ACORE_MODULE_TO_CCORE == FEATURE_OFF)
    macHeaderLen = IMM_MAC_HEADER_RES_LEN;
#else
    macHeaderLen = 0;
#endif

    /* �������㿽���ڴ� */
    immZc = IMM_ZcStaticAlloc(totalLen + macHeaderLen);
    if (immZc == VOS_NULL_PTR) {
        AT_ERR_LOG("At_CsndBuildUdpPacket: IMM_ZcStaticAlloc fail!");
        return VOS_NULL_PTR;
    }
    IMM_ZcReserve(immZc, macHeaderLen);

    srcAddr  = ipAddr;
    destAddr = ipAddr + 1;
    destPort = AT_UART_BAUDRATE_9600;
    srcPort  = AT_UDP_SRC_PORT;

    data = (VOS_UINT8 *)IMM_ZcPut(immZc, totalLen);
    if (data == VOS_NULL_PTR) {
        IMM_ZcFree(immZc);

        AT_ERR_LOG("At_CsndBuildUdpPacket: IMM_ZcPut fail!");

        return VOS_NULL_PTR;
    }

#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON && FEATURE_ACORE_MODULE_TO_CCORE == FEATURE_OFF)
    immZc->protocol = htons(ETH_P_IP); /*lint !e778*/
#endif

    /* ����UDPͷ */
    if (AT_BuildUdpHdr((AT_UdpPacketFormat *)data, length, srcAddr, destAddr, srcPort, destPort) != VOS_OK) {
        IMM_ZcFree(immZc);

        AT_ERR_LOG("At_CsndBuildUdpPacket: IMM_ZcPut fail!");

        return VOS_NULL_PTR;
    }

    return immZc;
}

#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)

VOS_UINT32 AT_PS_GetIpAddrByIfaceId(const VOS_UINT16 clientId, const VOS_UINT8 ifaceId)
{
    AT_ModemPsCtx *psModemCtx = VOS_NULL_PTR;

    psModemCtx = AT_GetModemPsCtxAddrFromClientId(clientId);

    return psModemCtx->ipAddrIfaceIdMap[ifaceId];
}
#else

VOS_UINT32 AT_PS_GetIpAddrByRabId(VOS_UINT16 clientId, VOS_UINT8 rabId)
{
    AT_ModemPsCtx *psModemCtx = VOS_NULL_PTR;
    VOS_UINT32 ipAddr;

    psModemCtx = AT_GetModemPsCtxAddrFromClientId(clientId);
    ipAddr = 0;

    if (AT_PS_IS_RABID_VALID(rabId)) {
        ipAddr = psModemCtx->ipAddrRabIdMap[rabId - AT_PS_RABID_OFFSET];
    }

    return ipAddr;
}
#endif


#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
#if (FEATURE_ACORE_MODULE_TO_CCORE == FEATURE_OFF)
VOS_UINT32 AT_IfaceTx(VOS_UINT8 ifaceId, IMM_Zc *buff)
{
    return (VOS_UINT32)ads_iface_tx(ifaceId, buff);
}
#else
VOS_UINT32 AT_IfaceTx(VOS_UINT8 ifaceId, IMM_Zc *buff)
{
    return CDS_IfaceTx(ifaceId, (TTF_Mem *)buff);
}
#endif

VOS_UINT32 At_SetCsndPara(VOS_UINT8 indexNum)
{
    IMM_Zc          *immZc = VOS_NULL_PTR;
    VOS_UINT32       ipAddr;
    VOS_UINT16       length;
    PS_IFACE_IdUint8 ifaceId;

    /* ������� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CSND_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ifaceId = (PS_IFACE_IdUint8)g_atParaList[0].paraValue;
    if (ifaceId >= PS_IFACE_ID_BUTT) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����UDP���ĳ��� */
    length = (VOS_UINT16)g_atParaList[1].paraValue;
    length = (length > AT_CSND_PLAYLOAD_MAX_LEN) ? (AT_CSND_PLAYLOAD_MAX_LEN) : length;

    ipAddr = AT_PS_GetIpAddrByIfaceId(indexNum, ifaceId);

    immZc = At_CsndBuildUdpPacket(ipAddr, length);

    if (immZc == VOS_NULL_PTR) {
        AT_ERR_LOG("At_SetCsndPara: At_CsndBuildUdpPacket fail!");
        return AT_ERROR;
    }

    /* �����������ݰ� */
    if (AT_IfaceTx(ifaceId, immZc) != VOS_OK) {
        AT_ERR_LOG("At_SetCsndPara: ads_iface_tx fail!");
        return AT_ERROR;
    }

    return AT_OK;
}
#else

VOS_UINT32 At_SetCsndPara(VOS_UINT8 indexNum)
{
    IMM_Zc *immZc = VOS_NULL_PTR;
    VOS_UINT8 rabId;
    VOS_UINT16 length;
    ModemIdUint16 modemId;
    VOS_UINT32 ipAddr;

    /* ������� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CSND_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* RABID��Ч�Լ��[5,15] */
    rabId = (VOS_UINT8)g_atParaList[0].paraValue;

    if (!AT_PS_IS_RABID_VALID(rabId)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����UDP���ĳ��� */
    length = (VOS_UINT16)g_atParaList[1].paraValue;
    length = (length > AT_CSND_PLAYLOAD_MAX_LEN) ? (AT_CSND_PLAYLOAD_MAX_LEN) : length;

    ipAddr = AT_PS_GetIpAddrByRabId(indexNum, rabId);

    immZc = At_CsndBuildUdpPacket(ipAddr, length);

    if (immZc == VOS_NULL_PTR) {
        AT_ERR_LOG("At_SetCsndPara: At_CsndBuildUdpPacket fail!");
        return AT_ERROR;
    }

    modemId = MODEM_ID_0;
    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        IMM_ZcFree(immZc);
        AT_ERR_LOG("At_SetCsndPara: Get modem id fail.");
        return AT_ERROR;
    }

    if (modemId != MODEM_ID_0) {
        rabId |= AT_PS_RABID_MODEM_1_MASK;
    }

    /* �����������ݰ� */
    if (ADS_UL_SendPacket(immZc, rabId) != VOS_OK) {
        AT_ERR_LOG("At_SetCsndPara: ADS_UL_SendPacket fail!");
        return AT_ERROR;
    }

    return AT_OK;
}
#endif

TAF_UINT32 AT_SetDwinsPara(TAF_UINT8 indexNum)
{
    VOS_UINT32  enabled;
    WINS_Config wins;

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

    /* ���ò��� */
    enabled = g_atParaList[0].paraValue;
    /* ���stWins�ṹ�� */
    (VOS_VOID)memset_s(&wins, sizeof(wins), 0x00, sizeof(WINS_Config));

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_WINS_CONFIG, &wins, sizeof(WINS_Config)) == NV_OK) {
        if ((wins.ucStatus == 1) && (enabled == wins.ucWins)) {
            AT_INFO_LOG("AT_SetDwinsPara():The content to write is same as NV's");

            /* ����PPP��WINS���� */
            PPP_UpdateWinsConfig((VOS_UINT8)g_atParaList[0].paraValue);

            /* ����APS��WINS���� */
            TAF_PS_ConfigNbnsFunction(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                      enabled);

            return AT_OK;
        }
    }

    wins.ucStatus = 1;
    wins.ucWins   = (VOS_UINT8)g_atParaList[0].paraValue;

    /* д��NVIM */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_WINS_CONFIG, (VOS_UINT8 *)&wins, sizeof(WINS_Config)) != NV_OK) {
        return AT_ERROR;
    } else {
        /* ����PPP��WINS���� */
        PPP_UpdateWinsConfig((VOS_UINT8)g_atParaList[0].paraValue);

        /* ����APS��WINS���� */
        TAF_PS_ConfigNbnsFunction(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, enabled);

        return AT_OK;
    }
}

VOS_UINT32 AT_SetApDsFlowRptCfgPara(VOS_UINT8 indexNum)
{
    TAF_APDSFLOW_RptCfg rptCfg;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������������� */
    if ((g_atParaIndex < AT_APDSFLOWRPTCFG_PARA_MIN_NUM) || (g_atParaIndex > AT_APDSFLOWRPTCFG_PARA_MAX_NUM)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT^APDSFLOWRPTCFG=, */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<enable> */
    rptCfg.rptEnabled = g_atParaList[0].paraValue;

    if (rptCfg.rptEnabled == VOS_TRUE) {
        /* AT^APDSFLOWRPTCFG����1 */
        if (g_atParaIndex == 1) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* AT^APDSFLOWRPTCFG=1, */
        if (g_atParaList[1].paraLen == 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* AT^APDSFLOWRPTCFG=1,<threshold> */
        rptCfg.fluxThreshold = g_atParaList[1].paraValue;

        /* AT^APDSFLOWRPTCFG����1,0 */
        if (rptCfg.fluxThreshold == 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    } else {
        /* AT^APDSFLOWRPTCFG����0 */
        rptCfg.fluxThreshold = 0;
    }

    /* ִ��������� */
    if (TAF_PS_SetApDsFlowRptCfg(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &rptCfg) !=
        VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_APDSFLOWRPTCFG_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetDsFlowNvWriteCfgPara(VOS_UINT8 indexNum)
{
    TAF_DSFLOW_NvWriteCfg writeNvCfg;

    (VOS_VOID)memset_s(&writeNvCfg, sizeof(writeNvCfg), 0x00, sizeof(writeNvCfg));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������������� */
    if (g_atParaIndex > AT_DSFLOWNVWRCFG_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT^DSFLOWNVWRCFG= */
    if (g_atParaIndex == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT^DSFLOWNVWRCFG=,<interval> */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ȡ<enable> */
    writeNvCfg.enabled = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ��ȡ<interval> */
    if (g_atParaIndex > 1) {
        if (g_atParaList[1].paraLen != 0) {
            /* AT^DSFLOWNVWRCFG=<enable>,<interval> */
            writeNvCfg.interval = (VOS_UINT8)g_atParaList[1].paraValue;
        } else {
            /* AT^DSFLOWNVWRCFG=<enable>, */
            return AT_CME_INCORRECT_PARAMETERS;
        }
    } else {
        /* AT^DSFLOWNVWRCFG=<enable> */
        writeNvCfg.interval = TAF_DEFAULT_DSFLOW_NV_WR_INTERVAL;
    }

    /* AT^DSFLOWNVWRCFG����1,0 */
    if ((writeNvCfg.enabled == VOS_TRUE) && (writeNvCfg.interval == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ִ��������� */
    if (TAF_PS_SetDsFlowNvWriteCfg(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                                   &writeNvCfg) != VOS_OK) {
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DSFLOWNVWRCFG_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_HUAWEI_VP == FEATURE_ON)
VOS_UINT32 AT_SndSetVoicePrefer(VOS_UINT16 clientId, VOS_UINT8 opId, VOS_UINT32 voicePreferApStatus)
{
    AT_RABM_SetVoicepreferParaReq *sndMsg = VOS_NULL_PTR;
    VOS_UINT32                     rslt;

    /* �����ڴ�  */
    /*lint -save -e516 */
    sndMsg = (AT_RABM_SetVoicepreferParaReq *)TAF_AllocMsgWithoutHeaderLen(WUEPS_PID_AT,
                                                           sizeof(AT_RABM_SetVoicepreferParaReq) - VOS_MSG_HEAD_LENGTH);
    /*lint -restore */
    if (sndMsg == VOS_NULL_PTR) {
        /* �ڴ�����ʧ�� */
        AT_ERR_LOG("AT_SndSetVoicePrefer:ERROR: Memory Alloc Error for pstMsg");
        return VOS_ERR;
    }

    /* ��д��ز��� */
    TAF_CfgMsgHdr((MsgBlock *)sndMsg, WUEPS_PID_AT, AT_GetDestPid(clientId, I0_WUEPS_PID_RABM),
                  sizeof(AT_RABM_SetVoicepreferParaReq) - VOS_MSG_HEAD_LENGTH);

    sndMsg->msgHeader.msgName = ID_AT_RABM_SET_VOICEPREFER_PARA_REQ;
    sndMsg->clientId          = clientId;
    sndMsg->opId              = opId;

    sndMsg->voicePreferApStatus = voicePreferApStatus;

    /* ����VOS����ԭ�� */
    rslt = TAF_TraceAndSendMsg(WUEPS_PID_AT, sndMsg);
    if (rslt != VOS_OK) {
        AT_ERR_LOG("AT_SndSetVoicePrefer:ERROR:TAF_TraceAndSendMsg ");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_SetVoicePreferPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 voicePreferAct;
    VOS_UINT32 rslt;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��һ������Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    voicePreferAct = g_atParaList[0].paraValue;

    rslt = AT_SndSetVoicePrefer(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, voicePreferAct);
    if (rslt == VOS_OK) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_VOICEPREFER_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
#endif

#if (FEATURE_MBB_CUST == FEATURE_ON)
/*
* ����˵��: ^LTEPROFILE�����PdpType�������
* �������: output: ���ڻ�ȡpdptype����
* ���ؽ��: AT_SUCCESS: ִ�гɹ�
*           �������:   ִ��ʧ��
*/
LOCAL VOS_UINT32 AT_LteProfileParaCheckPdpType(VOS_UINT8 *output)
{
    VOS_UINT8 pdpType;

    /* �ο�cgdcont��PDPTYPEУ���ʵ�� */
    pdpType = (VOS_UINT8)(g_atParaList[LTE_PROFILE_PDP_TYPE].paraValue + 1);

    /* ���õ�������ʵ��֧�ֵ����ͽ��бȽ� */
    if ((pdpType == TAF_PDP_IPV6) || (pdpType == TAF_PDP_IPV4V6)) {
#if (FEATURE_IPV6 == FEATURE_ON)
        if (AT_GetIpv6Capability() == AT_IPV6_CAPABILITY_IPV4_ONLY) {
            AT_ERR_LOG("AT_LteProfileParaCheckPdpType UE Capability is not support ipv6!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
#else
        AT_ERR_LOG("AT_LteProfileParaCheckPdpType UE is not support ipv6!");
        return AT_CME_INCORRECT_PARAMETERS;
#endif
    }

    *output = pdpType;
    return AT_SUCCESS;
}

/*
* ����˵��: ^LTEPROFILE�����PdpType�������
* �������: nvAttachProfileInfo: ���ɹ�����Ҫ����ImsiPreFix�ĳ�Ա����
* ���ؽ��: AT_SUCCESS: ִ�гɹ�
*           �������:   ִ��ʧ��
*/
LOCAL VOS_UINT32 AT_LteProfileParaCheckImsiPreFix(TAF_NVIM_LteAttachProfileInfo *nvAttachProfileInfo)
{
    /* �������ȼ��,imsiprefix length must in [5-10] */
    if ((g_atParaList[LTE_PROFILE_IMSIPREFIX].paraLen > TAF_MAX_LTE_APN_IMSI_PREFIX_SUPPORT) ||
        (g_atParaList[LTE_PROFILE_IMSIPREFIX].paraLen < TAF_MIN_LTE_APN_IMSI_PREFIX_SUPPORT)) {
        AT_ERR_LOG("AT_LteProfileParaCheckImsiPreFix IMSI_PREFIX is len is error!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �ַ������ּ�� */
    if (At_CheckNumString(g_atParaList[LTE_PROFILE_IMSIPREFIX].para, g_atParaList[LTE_PROFILE_IMSIPREFIX].paraLen) != AT_SUCCESS) {
        AT_ERR_LOG("AT_LteProfileParaCheckImsiPreFix At_CheckNumString return error!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���� imsiPrefix */
    nvAttachProfileInfo->imsiPrefixLen = (VOS_UINT8)g_atParaList[LTE_PROFILE_IMSIPREFIX].paraLen;
    (VOS_VOID)At_AsciiString2HexSimple(nvAttachProfileInfo->imsiPrefixBcd, g_atParaList[LTE_PROFILE_IMSIPREFIX].para,
        nvAttachProfileInfo->imsiPrefixLen);

    return AT_SUCCESS;
}

/*
* ����˵��: ^LTEPROFILE�����PdpType�������
* �������: index: AT�·�ͨ��
* �������: nvAttachProfileInfo: ���ɹ�����Ҫ����APN�ĳ�Ա����
* ���ؽ��: AT_SUCCESS: ִ�гɹ�
*           �������:   ִ��ʧ��
*/
LOCAL VOS_UINT32 AT_LteProfileParaCheckApn(TAF_NVIM_LteAttachProfileInfo *nvAttachProfileInfo, VOS_UINT8 indexNum)
{
    errno_t memResult;

    /* ���apn���ȣ�32λ���ڻ��߷�0 */
    if ((g_atParaList[4].paraLen > TAF_NVIM_MAX_APNRETRY_APN_LEN) || (g_atParaList[4].paraLen == 0)) {
        AT_ERR_LOG("AT_LteProfileParaCheckApn apn len is error!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���apn��ʽ */
    if (AT_CheckApnFormat(g_atParaList[4].para, g_atParaList[4].paraLen,
        g_atClientTab[indexNum].clientId) != VOS_OK) {
        AT_ERR_LOG("AT_LteProfileParaCheckApn AT_CheckApnFormat return error!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����APN */
    nvAttachProfileInfo->apnLen = (VOS_UINT8)g_atParaList[4].paraLen;
    memResult = memcpy_s(nvAttachProfileInfo->apn, sizeof(nvAttachProfileInfo->apn), g_atParaList[4].para,
        nvAttachProfileInfo->apnLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(nvAttachProfileInfo->apn), nvAttachProfileInfo->apnLen);
    return AT_SUCCESS;
}

/*
* ����˵��: ^LTEPROFILE�����PdpType�������
* �������: nvAttachProfileInfo: ���ɹ�����Ҫ�����û���������ͼ�Ȩ����
* ���ؽ��: AT_SUCCESS: ִ�гɹ�
*           �������:   ִ��ʧ��
*/
LOCAL VOS_UINT32 AT_LteProfileParaCheckAuthInfo(TAF_NVIM_LteAttachProfileInfo *nvAttachProfileInfo)
{
    errno_t memResult;

    /* ���username��userpwd�ĳ��� */
    if ((g_atParaList[5].paraLen > TAF_NVIM_MAX_APNRETRY_USERNAME_LEN) ||
        (g_atParaList[6].paraLen > TAF_NVIM_MAX_APNRETRY_PASSWORD_LEN)) {
        AT_ERR_LOG("AT_LteProfileParaCheckAuthInfo USERNAME_LEN or USERPWD_LEN is error!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����USERNAME */
    nvAttachProfileInfo->userNameLen = (VOS_UINT8)g_atParaList[5].paraLen; /* param5 user len */
    if (nvAttachProfileInfo->userNameLen > 0) {
        memResult = memcpy_s(nvAttachProfileInfo->userName, sizeof(nvAttachProfileInfo->userName),
            g_atParaList[5].para, nvAttachProfileInfo->userNameLen); /* param5 user save */
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(nvAttachProfileInfo->userName), nvAttachProfileInfo->userNameLen);
    }

    /* ����PWD */
    nvAttachProfileInfo->pwdLen = (VOS_UINT8)g_atParaList[6].paraLen; /* param6 pwd len */
    if (nvAttachProfileInfo->pwdLen > 0) {
        memResult = memcpy_s(nvAttachProfileInfo->pwd, sizeof(nvAttachProfileInfo->pwd),
            g_atParaList[6].para, nvAttachProfileInfo->pwdLen); /* param6 pwd save */
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(nvAttachProfileInfo->pwd), nvAttachProfileInfo->pwdLen);
    }

    /* ���authType��Ϊ��,ֵ��TAF_PDP_AUTH_TYPE_BUTT֮�� */
    if ((g_atParaList[7].paraLen == 0) || (g_atParaList[7].paraValue >= TAF_PDP_AUTH_TYPE_BUTT)) {
        AT_ERR_LOG("AT_LteProfileParaCheckAuthInfo AUTH_TYPE is error!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���� AuthType */
    nvAttachProfileInfo->authType = (VOS_UINT8)g_atParaList[7].paraValue; /* param7 authType */
                                                                          /* �����Ȩ���Ͳ�Ϊ�գ������û�������������һΪ���򷵻ز������� */
    if (nvAttachProfileInfo->authType != TAF_PDP_AUTH_TYPE_NONE) {
        if ((nvAttachProfileInfo->userNameLen == 0) || (nvAttachProfileInfo->pwdLen == 0)) {
            AT_ERR_LOG("AT_LteProfileParaCheckAuthInfo AUTH_TYPE is not null but username or pwd len is 0!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    return AT_SUCCESS;
}

/*
* ����˵��: ^LTEPROFILE�����PdpType�������
* �������: nvAttachProfileInfo: ���ɹ�����Ҫ�����û���������ͼ�Ȩ����
* ���ؽ��: AT_SUCCESS: ִ�гɹ�
*           �������:   ִ��ʧ��
*/
LOCAL VOS_UINT32 AT_LteProfileParaCheckProfileName(TAF_NVIM_LteAttachProfileInfo *nvAttachProfileInfo)
{
    errno_t memResult;

    /* ���profileName��Ϊ�� */
    if (g_atParaList[8].paraLen == 0) {
        AT_ERR_LOG("AT_LteProfileParaCheckProfileName profileName len is 0!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ɶ�Ӧ�ַ���ת�� */
    if (At_AsciiNum2HexString(g_atParaList[8].para, &g_atParaList[8].paraLen) != AT_SUCCESS) {
        AT_ERR_LOG("AT_LteProfileParaCheckProfileName At_AsciiNum2HexString return error!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �ַ�������18�ֽ���ض� */
    if (g_atParaList[8].paraLen >= TAF_NVIM_MAX_APNRETRY_PRONAME_LEN) {
        g_atParaList[8].paraLen = TAF_NVIM_MAX_APNRETRY_PRONAME_LEN - 1;
    }

    /* ����ProfileName */
    nvAttachProfileInfo->profileNameLen = (VOS_UINT8)g_atParaList[8].paraLen;
    if (nvAttachProfileInfo->profileNameLen > 0) {
        memResult = memcpy_s(nvAttachProfileInfo->profileName, sizeof(nvAttachProfileInfo->profileName),
            g_atParaList[8].para, nvAttachProfileInfo->profileNameLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(nvAttachProfileInfo->profileName), nvAttachProfileInfo->profileNameLen);
    }
    return AT_SUCCESS;
}

/*
 * ����˵��: APN info�ֶ����
 * �������: index: AT�·�ͨ��
 * �������: apnInfo: ���ڻ�ȡapn��Ϣ
 * ���ؽ��: AT_SUCCESS: ִ�гɹ�
 *           �������:   ִ��ʧ��
 */
LOCAL VOS_UINT32 AT_LteProfileParaSet(VOS_UINT8 indexNum, TAF_PS_CustomerAttachApnInfo *proInfo)
{
    /* У����������3��IpType */
    /* IP:1, IPV6:2, IPV4V6:3, ��������ʵ����Ҫȷ�� */
    if (AT_LteProfileParaCheckPdpType(&proInfo->nvAttachProfileInfo.pdpType) != AT_SUCCESS) {
        AT_ERR_LOG("At_SetLteProfileParaCheckParameter AT_LteProfileParaCheckPdpType return ERROR!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* У����������4��ImsiPrefix */
    if (AT_LteProfileParaCheckImsiPreFix(&proInfo->nvAttachProfileInfo) != AT_SUCCESS) {
        AT_ERR_LOG("At_SetLteProfileParaCheckParameter AT_LteProfileParaCheckImsiPreFix return ERROR!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* У����������5��Apn ��Ϣ */
    if (AT_LteProfileParaCheckApn(&proInfo->nvAttachProfileInfo, indexNum) != AT_SUCCESS) {
        AT_ERR_LOG("At_SetLteProfileParaCheckParameter AT_LteProfileParaCheckApn return ERROR!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* У����������6��7��8��auth��Ȩ�����û��������롢��Ȩ���� */
    if (AT_LteProfileParaCheckAuthInfo(&proInfo->nvAttachProfileInfo) != AT_SUCCESS) {
        AT_ERR_LOG("At_SetLteProfileParaCheckParameter AT_LteProfileParaCheckAuthInfo return ERROR!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* У����������9��ProfileName */
    if (AT_LteProfileParaCheckProfileName(&proInfo->nvAttachProfileInfo) != AT_SUCCESS) {
        AT_ERR_LOG("At_SetLteProfileParaCheckParameter AT_LteProfileParaCheckProfileName return ERROR!");
        return AT_CME_INCORRECT_PARAMETERS;
    }
    return AT_SUCCESS;
}

/*
 * ����˵��: ^LTEPROFILE����������
 * �������: index: ATͨ����
 *           cmdReadFlg: ���ڻ�ȡ��ȡ��ǩ
 *           apnInfo: ���ڻ�ȡapn��Ϣ
 * ���ؽ��: AT_SUCCESS: ִ�гɹ�
 *           �������: ִ��ʧ��
 */
LOCAL VOS_UINT32 AT_SetLteProfileParaCheckParameter(VOS_UINT8 indexNum, VOS_UINT8 *cmdReadFlg,
    TAF_PS_CustomerAttachApnInfo *proInfo)
{
    /* �˿���̬��飬APP�˿ڿ���������д */
    if ((g_atDataLocked == VOS_TRUE) && (indexNum != AT_CLIENT_ID_APP) && (indexNum != AT_CLIENT_ID_APP1)) {
        AT_ERR_LOG("At_SetLteProfileParaCheckParameter the port is not allow!");
        return AT_ERROR;
    }

    /* ���ͺ�index ����Я��У�� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        AT_ERR_LOG("At_SetLteProfileParaCheckParameter the indexNum is not support!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    proInfo->exInfo.index = (VOS_UINT16)g_atParaList[1].paraValue;

    /* ��ȡ���������͵��ж������ */
    if (g_atParaList[0].paraValue == 0) {
        *cmdReadFlg = VOS_TRUE;
        /* ��ȡ����ʱ����2�������ͷ��ز������� */
        if (g_atParaIndex > 2) {
            AT_ERR_LOG("At_SetLteProfileParaCheckParameter read cmd para is too much!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
        return AT_SUCCESS;
    } else {
        /* Ĭ�Ͻ������������Ϊ0�Ķ���APN */
        if (proInfo->exInfo.index != 0) {
            AT_ERR_LOG("At_SetLteProfileParaCheckParameter usIndex is not 0, this indexNum is not support!");
            return AT_CME_INCORRECT_PARAMETERS;
        }

        *cmdReadFlg = VOS_FALSE;
        proInfo->nvAttachProfileInfo.activeFlag = VOS_TRUE;

        /* ֻ��2����������������������ɾ�������� */
        if (g_atParaIndex == 2) {
            /* ����״̬����ΪFALSE */
            proInfo->nvAttachProfileInfo.activeFlag = VOS_FALSE;
            return AT_SUCCESS;
        }

        /* У����������3~9 */
        if (AT_LteProfileParaSet(indexNum, proInfo) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_SUCCESS;
}

/*
 * ����˵��: ^LTEPROFILE���������
 * �������: index: ATͨ����
 * ���ؽ��: AT_WAIT_ASYNC_RETURN: �첽�ȴ�״̬
 *           AT_ERROR: ִ��ʧ��
 */
VOS_UINT32 AT_SetCustProfilePara(VOS_UINT8 indexNum)
{
    TAF_PS_CustomerAttachApnInfo        setCustomAttachApn;
    VOS_UINT32                          result;
    VOS_UINT8                           cmdReadFlg = VOS_FALSE;

    (VOS_VOID)memset_s(&setCustomAttachApn, sizeof(setCustomAttachApn), 0x00, sizeof(setCustomAttachApn));

    /* ������� */
    result = AT_SetLteProfileParaCheckParameter(indexNum, &cmdReadFlg, &setCustomAttachApn);
    if (result != AT_SUCCESS) {
        return result;
    }

    /* ��ȡ���������� */
    if (cmdReadFlg == VOS_TRUE) {
        result = TAF_PS_GetCustomAttachProfile(WUEPS_PID_AT,
            AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), g_atClientTab[indexNum].opId,
            &(setCustomAttachApn.exInfo));
    } else {
        /* �������������� */
        result = TAF_PS_SetCustomAttachProfile(WUEPS_PID_AT,
            AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), g_atClientTab[indexNum].opId,
            &setCustomAttachApn);
    }

    if (result == VOS_OK) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LTEPROFILE_SET;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

/*
 * ����˵��: ^LTEAPNATTACH���������
 * �������: index: ATͨ����
 * ���ؽ��: AT_WAIT_ASYNC_RETURN: �첽�ȴ�
 *           AT_ERROR: ִ��ʧ��
 */
VOS_UINT32 AT_SetCustProfileAttachSwitch(VOS_UINT8 indexNum)
{
    VOS_UINT32                          result;
    VOS_UINT32                          switchFlag;

    /* ����ֻ��Ϊ0��1 */
    if ((g_atParaIndex > 1) || (g_atParaList[0].paraLen != 1) || (g_atParaList[0].paraValue > 1)) {
        AT_ERR_LOG("AT_SetCustProfileAttachSwitch Para check ERROR !");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    switchFlag = g_atParaList[0].paraValue;

    result = TAF_PS_SetAttachProfileSwitchStatus(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId),
        g_atClientTab[indexNum].opId, switchFlag);
    if (result == VOS_OK) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LTEAPNATTACH_SWITCH;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
VOS_BOOL AT_CheckCsUePolicyPara(VOS_VOID)
{
    /* �����������Ϸ� */
    if ((g_atParaIndex > AT_CSUEPOLICY_MAX_PARA_NUM) || (g_atParaIndex < AT_CSUEPOLICY_MIN_PARA_NUM)) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

VOS_UINT32 AT_ProcUePolicyClassMark(AT_ModemUePolicyCtx *uePolicyCtx)
{
    if (g_atParaList[AT_CSUEPOLICY_CLASS_MARK].paraLen != 0) {
        /* ���������������ȼ�� */
        if (g_atParaList[AT_CSUEPOLICY_CLASS_MARK].paraLen > (TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN * 2)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (At_AsciiString2HexSimple(uePolicyCtx->classMark, g_atParaList[AT_CSUEPOLICY_CLASS_MARK].para,
                                     (VOS_UINT16)g_atParaList[AT_CSUEPOLICY_CLASS_MARK].paraLen) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        uePolicyCtx->classMarkLen = (VOS_UINT8)(g_atParaList[AT_CSUEPOLICY_CLASS_MARK].paraLen / 2);
    }

    return AT_OK;
}

VOS_UINT32 AT_ProcUePolicyOsIdInformation(AT_ModemUePolicyCtx *uePolicyCtx)
{
    if (g_atParaList[AT_CSUEPOLICY_OS_ID_INFORMATION].paraLen != 0) {
        /* ���������������ȼ�� */
        if (g_atParaList[AT_CSUEPOLICY_OS_ID_INFORMATION].paraLen > (TAF_PS_UE_OS_ID_INFO_MAX_LEN * 2)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (At_AsciiString2HexSimple(uePolicyCtx->ueOsIdInfo, g_atParaList[AT_CSUEPOLICY_OS_ID_INFORMATION].para,
                                     (VOS_UINT16)g_atParaList[AT_CSUEPOLICY_OS_ID_INFORMATION].paraLen) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        uePolicyCtx->ueOsIdLen = (VOS_UINT8)(g_atParaList[AT_CSUEPOLICY_OS_ID_INFORMATION].paraLen / 2);
    }

    return AT_OK;
}

VOS_UINT32 AT_TransUePolicyProtocalVer(VOS_UINT32 protocalVer)
{
    VOS_UINT32 loop = protocalVer;
    VOS_UINT32 rslt = 0;
    VOS_UINT8 singleDigit = 0;
    VOS_UINT8 moveBit = 0;

    while (loop != 0) {
        singleDigit = loop % 10;
        loop = loop / 10;

        rslt += (singleDigit << moveBit);
        moveBit += 4;
    }

    return rslt;
}

VOS_UINT32 AT_SetCsUePolicy(VOS_UINT8 clientId)
{
    errno_t                 memResult;
    TAF_PS_5GUePolicyInfo   uePolicyInfo;
    AT_ModemUePolicyCtx    *uePolicyCtx = VOS_NULL_PTR;
    VOS_UINT32              rslt;

    uePolicyCtx = AT_GetModemUePolicyCtxAddrFromClientId(clientId);

    /*
     * +CSUEPOLICY=<PTI>,<message_type>[,<UE_policy_information_length>[,<UE_policy_classmark>
     * [,<OS_Id_infomation>]]]
     */
    /* �����������Ϸ� */
    if (AT_CheckCsUePolicyPara() != VOS_TRUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(uePolicyCtx, sizeof(AT_ModemUePolicyCtx), 0x00, sizeof(AT_ModemUePolicyCtx));

    /* ��¼���� */
    if (g_atParaList[AT_CSUEPOLICY_PTI].paraLen != 0) {
        uePolicyCtx->pti = (VOS_UINT8)g_atParaList[AT_CSUEPOLICY_PTI].paraValue;
    }

    if (g_atParaList[AT_CSUEPOLICY_MSG_TYPE].paraLen != 0) {
        uePolicyCtx->uePolicyMsgType = (TAF_PS_PolicyMsgTypeUint8)g_atParaList[AT_CSUEPOLICY_MSG_TYPE].paraValue;
    }

    /* ����<UE_policy_classmark> */
    rslt = AT_ProcUePolicyClassMark(uePolicyCtx);
    if (rslt != AT_OK) {
        return rslt;
    }
    /* ����<OS_Id_infomation> */
    rslt = AT_ProcUePolicyOsIdInformation(uePolicyCtx);
    if (rslt != AT_OK) {
        return rslt;
    }

    /* ����<protocal version> */
    if (g_atParaList[AT_CSUEPOLICY_PROTOCAL_VERSION].paraLen != 0) {
        uePolicyCtx->protocalVer = AT_TransUePolicyProtocalVer(g_atParaList[AT_CSUEPOLICY_PROTOCAL_VERSION].paraValue);
    }

    if ((g_atParaList[AT_CSUEPOLICY_INFO_LEN].paraLen != 0) && (g_atParaList[AT_CSUEPOLICY_INFO_LEN].paraValue != 0)) {
        if (g_atParaList[AT_CSUEPOLICY_INFO_LEN].paraValue % 2 != 0) {
            AT_ERR_LOG("AT_SetCsUePolicy: policy length is invalid!");
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* Ϊ׼�������UE POLICY���������ڴ� */
        uePolicyCtx->uePolicyHead = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT,
                                                              g_atParaList[AT_CSUEPOLICY_INFO_LEN].paraValue);
        if (uePolicyCtx->uePolicyHead == VOS_NULL_PTR) {
            AT_ERR_LOG("AT_SetCsUePolicy: Memory malloc failed!");
            return AT_ERROR;
        }

        uePolicyCtx->uePolicyCur = uePolicyCtx->uePolicyHead;
        uePolicyCtx->uePolicyLen = g_atParaList[AT_CSUEPOLICY_INFO_LEN].paraValue;
        (VOS_VOID)memset_s(uePolicyCtx->uePolicyHead, g_atParaList[AT_CSUEPOLICY_INFO_LEN].paraValue, 0x00,
                           g_atParaList[AT_CSUEPOLICY_INFO_LEN].paraValue);

        /* �л�ΪUE POLICY��������ģʽ */
        At_SetCmdSubMode(clientId, AT_UE_POLICY_MODE);

        return AT_WAIT_UE_POLICY_INPUT;
    }

    (VOS_VOID)memset_s(&uePolicyInfo, sizeof(uePolicyInfo), 0x00, sizeof(uePolicyInfo));

    uePolicyInfo.protocalVer     = uePolicyCtx->protocalVer;
    uePolicyInfo.pti             = uePolicyCtx->pti;
    uePolicyInfo.uePolicyMsgType = uePolicyCtx->uePolicyMsgType;
    uePolicyInfo.classMarkLen    = uePolicyCtx->classMarkLen;
    memResult = memcpy_s(uePolicyInfo.classMark, TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN, uePolicyCtx->classMark,
                         TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN);

    TAF_MEM_CHK_RTN_VAL(memResult, TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN, TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN);

    uePolicyInfo.ueOsIdLen = uePolicyCtx->ueOsIdLen;
    memResult              = memcpy_s(uePolicyInfo.ueOsIdInfo, TAF_PS_UE_OS_ID_INFO_MAX_LEN, uePolicyCtx->ueOsIdInfo,
                                      TAF_PS_UE_OS_ID_INFO_MAX_LEN);

    TAF_MEM_CHK_RTN_VAL(memResult, TAF_PS_UE_OS_ID_INFO_MAX_LEN, TAF_PS_UE_OS_ID_INFO_MAX_LEN);

    /* ����DSM�ṩ�����ýӿ� */
    if (TAF_PS_SetCsUePolicy(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[clientId].clientId), 0, &uePolicyInfo) !=
        VOS_OK) {
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetC5gqosPara(VOS_UINT8 indexNum)
{
    TAF_5G_QosExt st5QosInfo;

    (VOS_VOID)memset_s(&st5QosInfo, sizeof(st5QosInfo), 0x00, sizeof(TAF_5G_QosExt));

    if (g_atParaList[AT_C5GQOS_CID].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex > AT_C5GQOS_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    st5QosInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;

    /* �������ֻ��<CID>, �������������� */
    if (g_atParaIndex == 1) {
        st5QosInfo.defined = VOS_FALSE;
    } else {
        st5QosInfo.defined = VOS_TRUE;
        if (g_atParaList[AT_C5GQOS_5QI].paraLen != 0) {
            st5QosInfo.op5Qi = 1;
            st5QosInfo.uc5Qi = (VOS_UINT8)g_atParaList[AT_C5GQOS_5QI].paraValue;
        }

        if (g_atParaList[AT_C5GQOS_DL_GFBR].paraLen != 0) {
            st5QosInfo.opDlgfbr = 1;
            st5QosInfo.dlgfbr   = g_atParaList[AT_C5GQOS_DL_GFBR].paraValue;
        }

        if (g_atParaList[AT_C5GQOS_UL_GFBR].paraLen != 0) {
            st5QosInfo.opUlgfbr = 1;
            st5QosInfo.ulgfbr   = g_atParaList[AT_C5GQOS_UL_GFBR].paraValue;
        }

        /* ULGFBR��DLGFBR��Ҫͬʱ���ò���Ч;��ͬʱ���ã���Ϊ��Ч���ο�3gpp 27.007 */
        if (st5QosInfo.opDlgfbr != st5QosInfo.opUlgfbr) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (g_atParaList[AT_C5GQOS_DL_MFBR].paraLen != 0) {
            st5QosInfo.opDlmfbr = 1;
            st5QosInfo.dlmfbr   = g_atParaList[AT_C5GQOS_DL_MFBR].paraValue;
        }

        if (g_atParaList[AT_C5GQOS_UL_MFBR].paraLen != 0) {
            st5QosInfo.opUlmfbr = 1;
            st5QosInfo.ulmfbr   = g_atParaList[AT_C5GQOS_UL_MFBR].paraValue;
        }

        /* ULMFBR��DLMFBR��Ҫͬʱ���ò���Ч;��ͬʱ���ã���Ϊ��Ч���ο�3gpp 27.007 */
        if (st5QosInfo.opDlmfbr != st5QosInfo.opUlmfbr) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    if (TAF_PS_Set5QosInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &st5QosInfo) !=
        VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_C5GQOS_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetC5gqosRdpPara(VOS_UINT8 indexNum)
{
    VOS_UINT8 cid = 0;

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen != 0) {
        cid = (VOS_UINT8)g_atParaList[0].paraValue;
    } else {
        cid = 0xff;
    }

    if (TAF_PS_GetDynamic5gQosInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, cid) !=
        VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_C5GQOSRDP_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetCgcontexPara(VOS_UINT8 indexNum)
{
    TAf_PS_CgContExInfo cgContInfo = {0};

    /* ������������ */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex == 1) {
        cgContInfo.defined = VOS_FALSE;
    } else {
        if (g_atParaIndex != 3) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        cgContInfo.defined = VOS_TRUE;
        cgContInfo.process = (VOS_UINT8)g_atParaList[1].paraValue;
        cgContInfo.times   = (VOS_UINT8)g_atParaList[2].paraValue;
    }

    cgContInfo.cid = (VOS_UINT8)g_atParaList[0].paraValue;
    if (TAF_PS_SetCgContEx(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &cgContInfo) !=
        VOS_OK) {
        return AT_ERROR;
    }

    return AT_WAIT_ASYNC_RETURN;
}
#endif


#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_SetEmcStatus(VOS_UINT8 indexNum)
{
    VOS_UINT32      ret;
    VOS_UINT16      msgType;
    VOS_UINT8       emcPdnAt;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_SetEmcPdn: NOT AT_CMD_OPT_SET_PARA_CMD!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ȷ */
    if (g_atParaIndex != AT_CMD_PARA_INDEX_ONE) {
        AT_WARN_LOG("AT_SetEmcPdn: para num is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������ȼ�� */
    if (g_atParaList[0].paraLen != AT_CMD_PARA_LEN_ONE) {
        AT_WARN_LOG("AT_SetEmcPdn: paraLen  is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    emcPdnAt = (VOS_UINT8)g_atParaList[0].paraValue;

    msgType = (emcPdnAt == AT_EMC_PDN_STAT_PAPA_VALUE) ? ID_AT_IMSA_EMC_PDN_ACTIVATE_REQ :
        ID_AT_IMSA_EMC_PDN_DEACTIVATE_REQ;

    ret = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, msgType,
        VOS_NULL_PTR, 0, PS_PID_IMSA);

    if (ret != VOS_OK)
    {
        AT_WARN_LOG("AT_SetEmcPdn:AT_FillAndSndAppReqMsg fail");
        return AT_ERROR;
    }

    return AT_OK;
}
#endif

VOS_UINT32 AT_SetPsCallRptCmdPara(VOS_UINT8 indexNum)
{
    TAF_PS_CallRptCmdCfgInfo cmdCfg;
    VOS_UINT32 rslt;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&cmdCfg, sizeof(cmdCfg), 0, sizeof(cmdCfg));

    /* ������������ */
    if (g_atParaIndex != AT_PS_CALL_RPT_CMD_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������ͻ��ϱ�����δЯ�� ���ز������� */
    if ((g_atParaList[AT_PS_CALL_RPT_CMD_MODE_PARA].paraLen == 0) ||
        (g_atParaList[AT_PS_CALL_RPT_CMD_TYPE_PARA].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* Ŀǰֻ֧������stick���� */
    cmdCfg.opStickCmd = VOS_TRUE;
    cmdCfg.rptCmd = (TAF_PS_CallRptCmdUint8)g_atParaList[AT_PS_CALL_RPT_CMD_TYPE_PARA].paraValue;

    rslt = TAF_PS_SetPsCallRptCmd(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0, &cmdCfg);
    /* ִ��������� */
    if (rslt != VOS_OK) {
        return AT_ERROR;
    }

    /* ����AT���� */
    AT_GetCommPsCtxAddr()->stickCmd = (TAF_PS_CallRptCmdUint8)g_atParaList[AT_PS_CALL_RPT_CMD_TYPE_PARA].paraValue;

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PSCALLRPTCMD_SET;

    /* ������������״̬ */
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetApLanAddrPara(VOS_UINT8 indexNum)
{
    TAF_Ctrl   ctrl;
    VOS_UINT32 bitCid;
    VOS_UINT32 rst;

    /* ������ƽṹ�� */
    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    rst = At_SetIfaceDynamicParaComCheck(indexNum, &ctrl);

    if (rst != VOS_OK) {
        return rst;
    }

    /* BitCid ���� */
    bitCid = (VOS_UINT32)(0x01UL << g_atParaList[0].paraValue);

    /* ���Ͳ�ѯ��Ϣ */
    if (TAF_IFACE_GetDynamicPara(&ctrl, bitCid) != VOS_OK) {
        AT_ERR_LOG("AT_SetApLanAddrPara: TAF_IFACE_GetDynamicPara return ERROR!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_APLANADDR_SET;

    return AT_WAIT_ASYNC_RETURN;
}

