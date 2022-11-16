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
#include "at_custom_phy_set_cmd_proc.h"
#include "securec.h"

#include "AtParse.h"
#include "at_cmd_proc.h"

#include "at_custom_comm.h"
#include "nv_stru_msp_interface.h"
#include "at_input_proc.h"
#include "at_data_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_PHY_SET_CMD_PROC_C

#define AT_PHYCOMCFG_MAX_PARA_NUM 5
#define AT_PHYCOMCFG_MIN_PARA_NUM 3
#define AT_PHYCOMCFG_PARA1 2
#define AT_PHYCOMCFG_PARA2 3
#define AT_PHYCOMCFG_PARA3 4
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
#define AT_NRPHY_COMCFG_MAX_PARA_NUM 16
#define AT_NRPHY_COMCFG_MIN_PARA_NUM 2
#endif
#define AT_BODYSARGSM_MIN_PARA_NUM 1
#define AT_BODYSARGSM_BAND 1
#define AT_BODYSARWCDMA_MIN_PARA_NUM 1
#define AT_BODYSARWCDMA_BAND 1
#define AT_LTEPWRCTRL_PARA 1
#define AT_LTEPWRCTRL_PARA_NUM 2
#define AT_LTEPWRCTRL_PARA_MAX_LEN 2

#define AT_LTEPWRCTRL_MAX_DELETE_CA_NUM (-8)
#define AT_LTEPWRCTRL_MAX_ADD_CA_NUM 8

#define AT_LTEPWRCTRL_DISABLE_REDUCE_RI 0
#define AT_LTEPWRCTRL_ENABLE_REDUCE_RI 1

#define AT_LTEPWRCTRL_MIN_SUPPORT_BSR_NUM 0
#define AT_LTEPWRCTRL_MAX_SUPPORT_BSR_NUM 63

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
#define AT_NRPWRCTRL_MAX_DELETE_CA_NUM (-8)

#define AT_NRPWRCTRL_DISABLE_REDUCE_RI 0
#define AT_NRPWRCTRL_ENABLE_REDUCE_RI 1

#endif
#define AT_NRPWRCTRL_PARA_NUM 2
#define AT_TFDPDT_QRY_RAT_MODE 0
#define AT_PDMCTRL_PARA_NUM 4

#define AT_PDM_CTRL_REG_VALUE 0
#define AT_PDM_CTRL_BIAS_1 1
#define AT_PDM_CTRL_BIAS_2 2
#define AT_PDM_CTRL_BIAS_3 3

VOS_UINT32 AT_SetPhyComCfgPara(VOS_UINT8 indexNum)
{
    AT_MTA_PhyComCfgSetReq phyComCfg;
    VOS_UINT32             result;

    /* �ֲ�������ʼ�� */
    (VOS_VOID)memset_s(&phyComCfg, (VOS_SIZE_T)sizeof(phyComCfg), 0x00, (VOS_SIZE_T)sizeof(phyComCfg));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_SetPhyComCfg : Current Option is not AT_CMD_OPT_SET_PARA_CMD.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �жϲ������� */
    if ((g_atParaIndex < AT_PHYCOMCFG_MIN_PARA_NUM) || (g_atParaIndex > AT_PHYCOMCFG_MAX_PARA_NUM)) {
        /* ������������ */
        AT_WARN_LOG("AT_SetPhyComCfg : Current Number wrong.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��Ϣ��ֵ */
    phyComCfg.cmdType   = (VOS_UINT16)g_atParaList[0].paraValue;
    phyComCfg.ratBitmap = (VOS_UINT16)g_atParaList[1].paraValue;
    phyComCfg.para1     = g_atParaList[AT_PHYCOMCFG_PARA1].paraValue;

    if (g_atParaIndex == AT_CONST_NUM_4) {
        phyComCfg.para2 = g_atParaList[AT_PHYCOMCFG_PARA2].paraValue;
    } else {
        phyComCfg.para2 = g_atParaList[AT_PHYCOMCFG_PARA2].paraValue;
        phyComCfg.para3 = g_atParaList[AT_PHYCOMCFG_PARA3].paraValue;
    }

    /* ������Ϣ��MTA */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    ID_AT_MTA_PHY_COM_CFG_SET_REQ, (VOS_UINT8 *)&phyComCfg,
                                    (VOS_SIZE_T)sizeof(phyComCfg), I0_UEPS_PID_MTA);

    /* ����ʧ�� */
    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetPhyComCfg: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ���ͳɹ������õ�ǰ����ģʽ */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PHY_COM_CFG_SET;

    /* �ȴ��첽����ʱ�䷵�� */
    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
VOS_UINT32 AT_SetNrphyComCfgPara(VOS_UINT8 indexNum)
{
    AT_MTA_NrphyComCfgSetReq nrPhyComCfg;
    VOS_UINT32               result;
    VOS_UINT32               i;

    /* �ֲ�������ʼ�� */
    (VOS_VOID)memset_s(&nrPhyComCfg, sizeof(nrPhyComCfg), 0x00, sizeof(nrPhyComCfg));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_SetNrphyComCfgPara : Current Option is not AT_CMD_OPT_SET_PARA_CMD.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �жϲ������� */
    if ((g_atParaIndex > AT_NRPHY_COMCFG_MAX_PARA_NUM) || (g_atParaIndex < AT_NRPHY_COMCFG_MIN_PARA_NUM)) {
        /* ������������ */
        AT_WARN_LOG("AT_SetNrphyComCfgPara : Current Number wrong.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��Ϣ��ֵ */
    nrPhyComCfg.cmdType = (VOS_UINT16)g_atParaList[0].paraValue;
    nrPhyComCfg.paraNum = (VOS_UINT16)(g_atParaIndex - 1);
    for(i = 0; i < nrPhyComCfg.paraNum; i++){
        nrPhyComCfg.paraList[i] = g_atParaList[i + 1].paraValue;
    }

    /* ������Ϣ��MTA */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
        ID_AT_MTA_NRPHY_COM_CFG_SET_REQ, (VOS_UINT8 *)&nrPhyComCfg,(VOS_SIZE_T)sizeof(nrPhyComCfg), I0_UEPS_PID_MTA);

    /* ����ʧ�� */
    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetNrphyComCfgPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ���ͳɹ������õ�ǰ����ģʽ */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NRPHY_COM_CFG_SET;

    /* �ȴ��첽����ʱ�䷵�� */
    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_SndMtaBodySarPara(VOS_UINT8 indexNum, AT_MTA_BodySarStateUint16 bodySarState,
                                MTA_BodySarPara *bodySarPara)
{
    errno_t              memResult;
    VOS_UINT32           result;
    AT_MTA_BodySarSetReq bodySarSetReq;

    (VOS_VOID)memset_s(&bodySarSetReq, sizeof(bodySarSetReq), 0x00, sizeof(bodySarSetReq));

    /* �����Ϣ�ṹ�� */
    bodySarSetReq.state = bodySarState;
    memResult = memcpy_s(&bodySarSetReq.bodySarPara, sizeof(bodySarSetReq.bodySarPara), bodySarPara,
                         sizeof(MTA_BodySarPara));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(bodySarSetReq.bodySarPara), sizeof(MTA_BodySarPara));

    /* ������ϢAT_MTA_BodySarSetReq */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    ID_AT_MTA_BODY_SAR_SET_REQ, (VOS_UINT8 *)&bodySarSetReq,
                                    sizeof(AT_MTA_BodySarSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SndMtaBodySarPara: AT_FillAndSndAppReqMsg fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 AT_SetBodySarOnPara(VOS_UINT8 indexNum)
{
    VOS_UINT32      result;
    MTA_BodySarPara bodySarPara;

    (VOS_VOID)memset_s(&bodySarPara, sizeof(bodySarPara), 0x00, sizeof(bodySarPara));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �ж�BODYSAR��ǰ״̬ */
    if (g_atBodySarState == g_atParaList[0].paraValue) {
        return AT_OK;
    }

    /* ��ȡBODYSAR NV�� */
    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_BODY_SAR_PARA, &bodySarPara, sizeof(MTA_BodySarPara));

    if (result != NV_OK) {
        AT_ERR_LOG("AT_SetBodySarOnPara: Read NV fail");
        return AT_ERROR;
    }

    /* ������Ϣ��MTA */
    result = AT_SndMtaBodySarPara(indexNum, (AT_MTA_BodySarStateUint16)g_atParaList[0].paraValue, &bodySarPara);
    if (result != VOS_OK) {
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_BODYSARON_SET;
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_CheckBodySarWcdmaPara(AT_BodysarwcdmaSetPara *bodySarWcdmaPara)
{
    AT_NvWgRfMainBand wGBand;
    VOS_UINT8         loop;
    VOS_UINT32        len;
    errno_t           memResult;

    (VOS_VOID)memset_s(&wGBand, sizeof(wGBand), 0x00, sizeof(wGBand));

    /* ����������� */
    if ((g_atParaIndex < AT_BODYSARWCDMA_MIN_PARA_NUM) || (g_atParaIndex > AT_BODYSARWCDMA_MAX_PARA_NUM)) {
        AT_WARN_LOG1("AT_CheckBodySarWcdmaPara: Para Num Incorrect!", g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������Ϊ1ʱ������Ĭ��ֵ */
    if (g_atParaIndex == AT_BODYSARWCDMA_MIN_PARA_NUM) {
        len = VOS_StrLen(AT_RF_BAND_ANY_STR);
        if (len > 0) {
            memResult = memcpy_s(g_atParaList[AT_BODYSARWCDMA_BAND].para, sizeof(g_atParaList[AT_BODYSARWCDMA_BAND].para),
                                 AT_RF_BAND_ANY_STR, len);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(g_atParaList[AT_BODYSARWCDMA_BAND].para), len);
        }
        g_atParaList[AT_BODYSARWCDMA_BAND].para[len] = '\0';
        g_atParaList[AT_BODYSARWCDMA_BAND].paraLen   = (VOS_UINT16)len;
        g_atParaIndex                                = 2; /* ��ʾBODYSARWCDMAָ����2������ */
    }

    /* ������������Ϊż�� */
    if ((g_atParaIndex % 2) != 0) {
        AT_WARN_LOG1("AT_CheckBodySarWcdmaPara: Para Num is not Even!", g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ȡWCDMA Band����ֵ */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Item_WG_RF_MAIN_BAND, &wGBand, sizeof(wGBand)) != NV_OK) {
        AT_ERR_LOG("AT_CheckBodySarWcdmaPara: Read NV fail!");
        return AT_ERROR;
    }

    /* ѭ�����BandƵ���Ƿ�֧�֣����������ṹ�� */
    bodySarWcdmaPara->paraNum = 0;
    for (loop = 0; loop < (g_atParaIndex / 2); loop++) { /* ��BODYSARWCDMAָ�������Ϊ<power><band>Ϊ��Ӧһ�� */
        /* ���û������Ƶ�β�����16�����ַ���ת��Ϊ���� */
        if (At_String2Hex(g_atParaList[loop * 2 + 1].para, g_atParaList[loop * 2 + 1].paraLen,
                          &g_atParaList[loop * 2 + 1].paraValue) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����û������������BandƵ�Σ����滻Ϊ��ǰ֧�ֵ�BandƵ��ֵ */
        if (g_atParaList[loop * 2 + 1].paraValue == AT_RF_BAND_ANY) {
            g_atParaList[loop * 2 + 1].paraValue = wGBand.unWcdmaBand.band;
        }

        /* ����û������Ƶ�ε�ǰ�Ƿ�֧�֣������֧�ַ��ز������� */
        if ((g_atParaList[loop * 2 + 1].paraValue & (~wGBand.unWcdmaBand.band)) != 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        bodySarWcdmaPara->asPower[bodySarWcdmaPara->paraNum] = (VOS_INT16)g_atParaList[loop * 2].paraValue;
        bodySarWcdmaPara->band[bodySarWcdmaPara->paraNum]    = g_atParaList[loop * 2 + 1].paraValue;
        bodySarWcdmaPara->paraNum++;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_FillBodySarWcdmaPara(AT_BodysarwcdmaSetPara *bodySarWcdmaPara, MTA_BodySarPara *bodySarPara)
{
    VOS_UINT8           loop1;
    MTA_WcdmaBandUint16 loop2;
    VOS_UINT8           loop3;

    bodySarWcdmaPara->paraNum = (VOS_UINT8)TAF_MIN(bodySarWcdmaPara->paraNum, AT_BODYSARWCDMA_MAX_PARA_GROUP_NUM);

    bodySarPara->wBandNum = (VOS_UINT16)TAF_MIN(bodySarPara->wBandNum, MTA_BODY_SAR_WBAND_MAX_NUM);

    /* �����������еĲ��� */
    for (loop1 = 0; loop1 < bodySarWcdmaPara->paraNum; loop1++) {
        /* ����WCDMA Band�������� */
        for (loop2 = MTA_WCDMA_I_2100; loop2 < MTA_WCDMA_BAND_BUTT; loop2++) {
            if ((bodySarWcdmaPara->band[loop1] & (0x00000001UL << (loop2 - 1))) == 0) {
                continue;
            }

            for (loop3 = 0; loop3 < bodySarPara->wBandNum; loop3++) {
                if (loop2 == bodySarPara->wBandPara[loop3].band) {
                    break;
                }
            }

            if (loop3 == bodySarPara->wBandNum) {
                /* ������W Band���֧�ָ�������ظ�ʧ�� */
                if (bodySarPara->wBandNum >= MTA_BODY_SAR_WBAND_MAX_NUM) {
                    AT_ERR_LOG("AT_FillBodySarWcdmaPara: Too Many W Band!");
                    return VOS_ERR;
                }

                bodySarPara->wBandNum++;
            }
            if (loop3 < MTA_BODY_SAR_WBAND_MAX_NUM) {
                bodySarPara->wBandPara[loop3].band  = loop2;
                bodySarPara->wBandPara[loop3].power = bodySarWcdmaPara->asPower[loop1];
            }
        }
    }

    return VOS_OK;
}

VOS_UINT32 AT_SetBodySarWcdmaPara(VOS_UINT8 indexNum)
{
    MTA_BodySarPara        bodySarPara;
    AT_BodysarwcdmaSetPara bodySarWcdmaPara;
    VOS_UINT32             result;

    (VOS_VOID)memset_s(&bodySarPara, sizeof(bodySarPara), 0x00, sizeof(bodySarPara));
    (VOS_VOID)memset_s(&bodySarWcdmaPara, sizeof(bodySarWcdmaPara), 0x00, sizeof(bodySarWcdmaPara));

    /* ��������Ч�� */
    result = AT_CheckBodySarWcdmaPara(&bodySarWcdmaPara);
    if (result != AT_SUCCESS) {
        return result;
    }

    /* ��NV���ж�ȡBody SAR��������ֵ */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_BODY_SAR_PARA, &bodySarPara, sizeof(bodySarPara)) != NV_OK) {
        AT_ERR_LOG("AT_SetBodysarWcdmaPara: Read NV fail!");
        return AT_ERROR;
    }

    /* ��WCDMA��Ƶ�β�����䵽Body SAR�����ṹ���� */
    if (AT_FillBodySarWcdmaPara(&bodySarWcdmaPara, &bodySarPara) != VOS_OK) {
        return AT_ERROR;
    }

    /* ��Body SAR��������ֵ���浽NV */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_BODY_SAR_PARA, (VOS_UINT8 *)&(bodySarPara), sizeof(bodySarPara)) !=
        NV_OK) {
        AT_ERR_LOG("AT_SetBodysarWcdmaPara: Write NV fail!");
        return AT_ERROR;
    }

    /* �����ǰ������Body SAR���ܣ���֪ͨ������µ�Body SAR���� */
    if (g_atBodySarState == AT_MTA_BODY_SAR_ON) {
        if (AT_SndMtaBodySarPara(indexNum, g_atBodySarState, &bodySarPara) != VOS_OK) {
            AT_WARN_LOG("AT_SetBodySarWcdmaPara: AT_SndMtaBodySarPara fail!");
        }
    }

    return AT_OK;
}

VOS_UINT32 AT_CheckBodySarGsmPara(AT_BodysargsmSetPara *bodySarGsmPara)
{
    VOS_UINT32 gBand = 0;
    VOS_UINT8  loop;
    VOS_UINT32 len;
    errno_t    memResult;

    /* ����������� */
    if ((g_atParaIndex < AT_BODYSARGSM_MIN_PARA_NUM) || (g_atParaIndex > AT_BODYSARGSM_MAX_PARA_NUM)) {
        AT_WARN_LOG1("AT_CheckBodySarGsmPara: Para Num Incorrect!", g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������Ϊ1ʱ������Ĭ��ֵ */
    if (g_atParaIndex == AT_BODYSARGSM_MIN_PARA_NUM) {
        len = VOS_StrLen(AT_RF_BAND_ANY_STR);
        if (len > 0) {
            memResult = memcpy_s(g_atParaList[AT_BODYSARGSM_BAND].para, sizeof(g_atParaList[AT_BODYSARGSM_BAND].para),
                             AT_RF_BAND_ANY_STR, len);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(g_atParaList[AT_BODYSARGSM_BAND].para), len);
        }
        g_atParaList[AT_BODYSARGSM_BAND].para[len] = '\0';
        g_atParaList[AT_BODYSARGSM_BAND].paraLen   = (VOS_UINT16)len;
        g_atParaIndex                              = 2; /* ��ʾBODYSARGSMָ����2������ */
    }

    /* ������������Ϊż�� */
    if ((g_atParaIndex % 2) != 0) {
        AT_WARN_LOG1("AT_CheckBodySarGsmPara: Para Num is not Even!", g_atParaIndex);
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ȡGSM Band����ֵ */
    if (AT_GetGsmBandCapa(&gBand) != VOS_OK) {
        return AT_ERROR;
    }

    /* ѭ�����BandƵ���Ƿ�֧�֣����������ṹ�� */
    bodySarGsmPara->paraNum = 0;
    for (loop = 0; loop < (g_atParaIndex / 2); loop++) { /* ��BODYSARGSMָ�������Ϊ<power><band>Ϊ��Ӧһ�� */
        /* ���û������Ƶ�β�����16�����ַ���ת��Ϊ���� */
        if (At_String2Hex(g_atParaList[loop * 2 + 1].para, g_atParaList[loop * 2 + 1].paraLen,
                          &g_atParaList[loop * 2 + 1].paraValue) != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ����û������������BandƵ�Σ����滻Ϊ��ǰ֧�ֵ�BandƵ��ֵ */
        if (g_atParaList[loop * 2 + 1].paraValue == AT_RF_BAND_ANY) {
            g_atParaList[loop * 2 + 1].paraValue = gBand;
        }

        /* ����û������Ƶ�ε�ǰ�Ƿ�֧�֣������֧�ַ��ز������� */
        if ((g_atParaList[loop * 2 + 1].paraValue & (~gBand)) != 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        bodySarGsmPara->asPower[bodySarGsmPara->paraNum] = (VOS_INT16)g_atParaList[loop * 2].paraValue;
        bodySarGsmPara->band[bodySarGsmPara->paraNum]    = g_atParaList[loop * 2 + 1].paraValue;
        bodySarGsmPara->paraNum++;
    }

    return AT_SUCCESS;
}

VOS_VOID AT_FillBodySarGsmPara(AT_BodysargsmSetPara *bodySarGsmPara, MTA_BodySarPara *bodySarPara)
{
    VOS_UINT8 loop;

    /* �����������еĲ��� */
    bodySarGsmPara->paraNum = AT_MIN(bodySarGsmPara->paraNum, AT_NVIM_BODYSARGSM_MAX_PARA_GROUP_NUM);
    for (loop = 0; loop < bodySarGsmPara->paraNum; loop++) {
        if ((AT_BODY_SAR_GBAND_GPRS_850_MASK & bodySarGsmPara->band[loop]) != 0) {
            bodySarPara->gBandMask |= AT_BODY_SAR_GBAND_GPRS_850_MASK;
            bodySarPara->gBandPara[AT_GSM_850].gprsPower = bodySarGsmPara->asPower[loop];
        }

        if ((AT_BODY_SAR_GBAND_EDGE_850_MASK & bodySarGsmPara->band[loop]) != 0) {
            bodySarPara->gBandMask |= AT_BODY_SAR_GBAND_EDGE_850_MASK;
            bodySarPara->gBandPara[AT_GSM_850].edgePower = bodySarGsmPara->asPower[loop];
        }

        if ((AT_BODY_SAR_GBAND_GPRS_900_MASK & bodySarGsmPara->band[loop]) != 0) {
            bodySarPara->gBandMask |= AT_BODY_SAR_GBAND_GPRS_900_MASK;
            bodySarPara->gBandPara[AT_GSM_900].gprsPower = bodySarGsmPara->asPower[loop];
        }

        if ((AT_BODY_SAR_GBAND_EDGE_900_MASK & bodySarGsmPara->band[loop]) != 0) {
            bodySarPara->gBandMask |= AT_BODY_SAR_GBAND_EDGE_900_MASK;
            bodySarPara->gBandPara[AT_GSM_900].edgePower = bodySarGsmPara->asPower[loop];
        }

        if ((AT_BODY_SAR_GBAND_GPRS_1800_MASK & bodySarGsmPara->band[loop]) != 0) {
            bodySarPara->gBandMask |= AT_BODY_SAR_GBAND_GPRS_1800_MASK;
            bodySarPara->gBandPara[AT_GSM_1800].gprsPower = bodySarGsmPara->asPower[loop];
        }

        if ((AT_BODY_SAR_GBAND_EDGE_1800_MASK & bodySarGsmPara->band[loop]) != 0) {
            bodySarPara->gBandMask |= AT_BODY_SAR_GBAND_EDGE_1800_MASK;
            bodySarPara->gBandPara[AT_GSM_1800].edgePower = bodySarGsmPara->asPower[loop];
        }

        if ((AT_BODY_SAR_GBAND_GPRS_1900_MASK & bodySarGsmPara->band[loop]) != 0) {
            bodySarPara->gBandMask |= AT_BODY_SAR_GBAND_GPRS_1900_MASK;
            bodySarPara->gBandPara[AT_GSM_1900].gprsPower = bodySarGsmPara->asPower[loop];
        }

        if ((AT_BODY_SAR_GBAND_EDGE_1900_MASK & bodySarGsmPara->band[loop]) != 0) {
            bodySarPara->gBandMask |= AT_BODY_SAR_GBAND_EDGE_1900_MASK;
            bodySarPara->gBandPara[AT_GSM_1900].edgePower = bodySarGsmPara->asPower[loop];
        }
    }
}

VOS_UINT32 AT_SetBodySarGsmPara(VOS_UINT8 indexNum)
{
    MTA_BodySarPara      bodySarPara;
    AT_BodysargsmSetPara bodySarGsmPara;
    VOS_UINT32           result;

    (VOS_VOID)memset_s(&bodySarPara, sizeof(bodySarPara), 0x00, sizeof(bodySarPara));
    (VOS_VOID)memset_s(&bodySarGsmPara, sizeof(bodySarGsmPara), 0x00, sizeof(bodySarGsmPara));

    /* ��������Ч�� */
    result = AT_CheckBodySarGsmPara(&bodySarGsmPara);
    if (result != AT_SUCCESS) {
        return result;
    }

    /* ��NV���ж�ȡBody SAR��������ֵ */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_BODY_SAR_PARA, &bodySarPara, sizeof(bodySarPara)) != NV_OK) {
        AT_ERR_LOG("AT_SetBodySarGsmPara: Read NV fail!");
        return AT_ERROR;
    }

    /* ��GSM��Ƶ�β�����䵽Body SAR�����ṹ���� */
    AT_FillBodySarGsmPara(&bodySarGsmPara, &bodySarPara);

    /* ��Body SAR��������ֵ���浽NV */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_BODY_SAR_PARA, (VOS_UINT8 *)&(bodySarPara), sizeof(bodySarPara)) !=
        NV_OK) {
        AT_ERR_LOG("AT_SetBodySarGsmPara: Write NV fail!");
        return AT_ERROR;
    }

    /* �����ǰ������Body SAR���ܣ���֪ͨ������µ�Body SAR���� */
    if (g_atBodySarState == AT_MTA_BODY_SAR_ON) {
        if (AT_SndMtaBodySarPara(indexNum, g_atBodySarState, &bodySarPara) != VOS_OK) {
            AT_WARN_LOG("AT_SetBodySarGsmPara: AT_SndMtaBodySarPara fail!");
        }
    }

    return AT_OK;
}

VOS_UINT32 AT_SetRefclkfreqPara(VOS_UINT8 indexNum)
{
    VOS_UINT32              result;
    AT_MTA_RefclkfreqSetReq refClockReq;

    /* ͨ����� */
    if (AT_IsApPort(indexNum) == VOS_FALSE) {
        return AT_ERROR;
    }

    /* ������� */
    if (g_atParaIndex != 1) {
        return AT_ERROR;
    }

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&refClockReq, sizeof(refClockReq), 0x00, sizeof(refClockReq));

    refClockReq.rptFlg = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ���Ϳ����Ϣ��C��, ����GPS�ο�ʱ��״̬�Ƿ������ϱ� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    ID_AT_MTA_REFCLKFREQ_SET_REQ, (VOS_UINT8 *)&refClockReq, sizeof(refClockReq),
                                    I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetRefclkfreqPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_REFCLKFREQ_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 At_SetHandleDect(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;
    VOS_UINT8  handleType;
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    /* ��ȡNV���е�ǰ��Ʒ��̬ */
    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* ��������������phone��pad��̬cp���ֳ�λ�ã���ANDROIDϵͳ��֧�� */
    if (*systemAppConfig != SYSTEM_APP_ANDROID) {
        return AT_CMD_NOT_SUPPORT;
    }

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_TOO_MANY_PARA;
    }

    /* ����Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_ERROR;
    }

    /* handletype ȡֵ���� */
    if (g_atParaList[0].paraValue > AT_MTA_HANDLEDECT_MAX_TYPE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    handleType = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ������Ϣ ID_AT_MTA_HANDLEDECT_SET_REQ ��MTA��������Ϣ������(VOS_UINT8)g_atParaList[0].ulParaValue */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, At_GetOpId(), ID_AT_MTA_HANDLEDECT_SET_REQ,
                                 &handleType, sizeof(handleType), I0_UEPS_PID_MTA);

    if (rst == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_HANDLEDECT_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_SetRatRfSwitch(VOS_UINT8 indexNum)
{
    NAS_NvTriModeEnable       triModeEnableStru;
    NAS_NvTriModeFemProfileId triModeFemProfileIdStru;

    (VOS_VOID)memset_s(&triModeEnableStru, sizeof(triModeEnableStru), 0x00, sizeof(triModeEnableStru));
    (VOS_VOID)memset_s(&triModeFemProfileIdStru, sizeof(triModeFemProfileIdStru), 0x00, sizeof(triModeFemProfileIdStru));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[0].paraLen == 0) || (g_atParaIndex != 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��ѯ�Ƿ�֧��ȫ��ͨ���� */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Item_TRI_MODE_ENABLE, &triModeEnableStru, sizeof(triModeEnableStru)) !=
        NV_OK) {
        AT_WARN_LOG("At_SetRatRfSwitch:read en_NV_Item_TRI_MODE_ENABLE failed");
        return AT_ERROR;
    }

    /* ��֧�֣����� */
    if (triModeEnableStru.enable == 0) {
        AT_WARN_LOG("At_SetRatRfSwitch:uhwEnable is not enabled");
        return AT_CME_OPERATION_NOT_ALLOWED;
    }

    triModeFemProfileIdStru.profileId = g_atParaList[0].paraValue;

    /* ����Profile Id */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, en_NV_Item_TRI_MODE_FEM_PROFILE_ID, (VOS_UINT8 *)&triModeFemProfileIdStru,
                           sizeof(triModeFemProfileIdStru)) != NV_OK) {
        AT_WARN_LOG("At_SetRatRfSwitch: MODEM_ID_0 write en_NV_Item_TRI_MODE_FEM_PROFILE_ID failed");
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 AT_SetMcsPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;
    VOS_UINT32      result;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

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

    netCtx->mcsDirection = (AT_McsDirectionUint8)g_atParaList[0].paraValue;

    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    ID_AT_MTA_MCS_SET_REQ, VOS_NULL_PTR, 0, I0_UEPS_PID_MTA);

    if (result == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MCS_SET;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

LOCAL VOS_UINT32 AT_LtePwrDissParaCheck(AT_MTA_LtepwrdissSetReq *atCmd)
{
    VOS_INT32  val = 0;
    VOS_UINT32 rst;

    if (AT_AtoInt((VOS_CHAR *)g_atParaList[1].para, g_atParaList[1].paraLen, &val) == VOS_ERR) {
        return VOS_ERR;
    }

    rst = VOS_ERR;
    switch ((AT_LTEPWRCTRL_MODE_TYPE_ENUM_UINT32)g_atParaList[0].paraValue) {
        case AT_LTEPWRCTRL_MODE_CC_NUM_CTR: {
            if ((val >= AT_LTEPWRCTRL_MAX_DELETE_CA_NUM) && (val <= AT_LTEPWRCTRL_MAX_ADD_CA_NUM)) {
                rst = VOS_OK;
            }
            break;
        }
        case AT_LTEPWRCTRL_MODE_RI_NUM_CTR: {
            if ((val == AT_LTEPWRCTRL_DISABLE_REDUCE_RI) || (val == AT_LTEPWRCTRL_ENABLE_REDUCE_RI)) {
                rst = VOS_OK;
            }
            break;
        }
        case AT_LTEPWRCTRL_MODE_BSR_NUM_CTR: {
            if ((val >= AT_LTEPWRCTRL_MIN_SUPPORT_BSR_NUM) && (val <= AT_LTEPWRCTRL_MAX_SUPPORT_BSR_NUM)) {
                rst = VOS_OK;
            }
            break;
        }
        default: {
            break;
        }
    }

    if (rst == VOS_ERR) {
        return VOS_ERR;
    }

    atCmd->mode = (VOS_UINT8)g_atParaList[0].paraValue;
    atCmd->para = (VOS_INT16)val;

    return VOS_OK;
}

VOS_UINT32 AT_SetLtePwrDissPara(VOS_UINT8 indexNum)
{
    AT_MTA_LtepwrdissSetReq atCmd;
    VOS_UINT32              rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != AT_LTEPWRCTRL_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����1�Ͳ���2�ĳ��Ȳ���Ϊ0 */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����2���������ΧΪ(-8~63),���Ȳ��ܴ���2 */
    if (g_atParaList[AT_LTEPWRCTRL_PARA].paraLen > AT_LTEPWRCTRL_PARA_MAX_LEN) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(AT_MTA_LtepwrdissSetReq), 0, sizeof(AT_MTA_LtepwrdissSetReq));

    rst = AT_LtePwrDissParaCheck(&atCmd);

    if (rst == VOS_ERR) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �·���������������Ϣ��C�˴��� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_SET_LTEPWRDISS_REQ, (VOS_UINT8 *)&atCmd,
                                 sizeof(AT_MTA_LtepwrdissSetReq), I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LTEPWRDISS_SET;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
LOCAL VOS_UINT32 AT_NrPwrCtrlParaCheck(AT_MTA_NrpwrctrlSetReq *atCmd)
{
    VOS_INT32  val = 0;
    VOS_UINT32 rst;

    if (AT_AtoInt((VOS_CHAR *)g_atParaList[1].para, g_atParaList[1].paraLen, &val) == VOS_ERR) {
        AT_WARN_LOG("AT_NrPwrCtrlParaCheck: string -> num error.");
        return VOS_ERR;
    }

    rst = VOS_ERR;
    switch ((AT_NrpwrctrlModeTypeUint32)g_atParaList[0].paraValue) {
        /* 0��2��case��Χ�ȱ����ƣ�����㿪����ʱ�����޸� */
        case AT_NRPWRCTRL_MODE_RI_NUM_CTR:
            if ((val == AT_NRPWRCTRL_DISABLE_REDUCE_RI) || (val == AT_NRPWRCTRL_ENABLE_REDUCE_RI)) {
                rst = VOS_OK;
            }
            break;

        default:
            AT_WARN_LOG("AT_NrPwrCtrlParaCheck: unexpected mode type.");
            break;
    }

    if (rst == VOS_ERR) {
        return VOS_ERR;
    }

    atCmd->mode = (VOS_UINT8)g_atParaList[0].paraValue;
    atCmd->para = (VOS_INT16)val;

    return VOS_OK;
}

VOS_UINT32 AT_SetNrPwrCtrlPara(VOS_UINT8 indexNum)
{
    AT_MTA_NrpwrctrlSetReq atCmd;
    VOS_UINT32             rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("AT_SetNrPwrCtrlPara: ucCmdOptType is not SET_PARA_CMD.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex != AT_NRPWRCTRL_PARA_NUM) {
        AT_WARN_LOG("AT_SetNrPwrCtrlPara: Para num is not correct.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����1�Ͳ���2�ĳ��Ȳ���Ϊ0 */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        AT_WARN_LOG("AT_SetNrPwrCtrlPara: Length of para is 0.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����2���������ΧΪ(-8~63),���Ȳ��ܴ���2 */
    if (g_atParaList[1].paraLen > 2) {
        AT_WARN_LOG("AT_SetNrPwrCtrlPara: Length of para 2 is greater than 2.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(AT_MTA_NrpwrctrlSetReq), 0, sizeof(AT_MTA_NrpwrctrlSetReq));

    rst = AT_NrPwrCtrlParaCheck(&atCmd);

    if (rst == VOS_ERR) {
        AT_WARN_LOG("AT_SetNrPwrCtrlPara: Check para fail.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �·���������������Ϣ��C�˴��� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_NRPWRCTRL_SET_REQ, (VOS_UINT8 *)&atCmd,
                                 sizeof(AT_MTA_NrpwrctrlSetReq), I0_UEPS_PID_MTA);

    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NRPWRCTRL_SET;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
VOS_UINT32 AT_SetPdmCtrlPara(VOS_UINT8 indexNum)
{
    AT_HPA_PdmCtrlReq *msg          = VOS_NULL_PTR;
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;

    atDevCmdCtrl = AT_GetDevCmdCtrl();

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != AT_PDMCTRL_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[AT_PDM_CTRL_REG_VALUE].paraLen == 0) || (g_atParaList[AT_PDM_CTRL_BIAS_1].paraLen == 0) ||
        (g_atParaList[AT_PDM_CTRL_BIAS_2].paraLen == 0) || (g_atParaList[AT_PDM_CTRL_BIAS_3].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (atDevCmdCtrl->currentTMode != AT_TMODE_FTM) {
        return AT_DEVICE_MODE_ERROR;
    }
    /*lint -save -e516 */
    msg = (AT_HPA_PdmCtrlReq *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_HPA_PdmCtrlReq));
    /*lint -restore */
    if (msg == VOS_NULL_PTR) {
        AT_WARN_LOG("AT_SetPdmCtrlPara: alloc msg fail!");
        return AT_ERROR;
    }

    /* ��ʼ����Ϣ */
    (VOS_VOID)memset_s((VOS_CHAR *)msg + VOS_MSG_HEAD_LENGTH,
                       sizeof(AT_HPA_PdmCtrlReq) - VOS_MSG_HEAD_LENGTH, 0x00,
                       sizeof(AT_HPA_PdmCtrlReq) - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, DSP_PID_WPHY, ID_AT_HPA_PDM_CTRL_REQ);

    msg->msgId       = ID_AT_HPA_PDM_CTRL_REQ;
    msg->rsv         = 0;
    msg->pdmRegValue = (VOS_UINT16)g_atParaList[AT_PDM_CTRL_REG_VALUE].paraValue;
    msg->paVbias     = (VOS_UINT16)g_atParaList[AT_PDM_CTRL_BIAS_1].paraValue;
    msg->paVbias2    = (VOS_UINT16)g_atParaList[AT_PDM_CTRL_BIAS_2].paraValue;
    msg->paVbias3    = (VOS_UINT16)g_atParaList[AT_PDM_CTRL_BIAS_3].paraValue;

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("AT_SetPdmCtrlPara: Send msg fail!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PDM_CTRL; /* ���õ�ǰ����ģʽ */
    atDevCmdCtrl->index                   = indexNum;

    return AT_WAIT_ASYNC_RETURN; /* �ȴ��첽�¼����� */
}
#endif

VOS_UINT32 At_SetQryTfDpdtPara(VOS_UINT8 indexNum)
{
    AT_MTA_QryDpdtValueReq atCmd;
    VOS_UINT32             rst;

    /* ������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT������MTA����Ϣ�ṹ��ֵ */
    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(AT_MTA_QryDpdtValueReq));
    atCmd.ratMode = (AT_MTA_CmdRatmodeUint8)g_atParaList[AT_TFDPDT_QRY_RAT_MODE].paraValue;

    /* ������Ϣ��C�˴��� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_QRY_DPDT_VALUE_REQ, (VOS_UINT8 *)&atCmd,
                                 sizeof(AT_MTA_QryDpdtValueReq), I0_UEPS_PID_MTA);

    if (rst == AT_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_TFDPDTQRY_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
VOS_UINT32 At_SetLowPowerModePara(VOS_UINT8 indexNum)
{
    AT_MTA_LowPwrModeReq lowPwrModeReq;

    /* ��ʼ�� */
    (VOS_VOID)memset_s(&lowPwrModeReq, sizeof(lowPwrModeReq), 0x0, sizeof(lowPwrModeReq));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("At_SetLowPowerModePara:Current Option is not AT_CMD_OPT_SET_PARA_CMD.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �жϲ������� */
    if ((g_atParaIndex != 1) || (g_atParaList[0].paraLen == 0)) {
        /* ������������ */
        AT_WARN_LOG("At_SetLowPowerModePara : Para wrong.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    lowPwrModeReq.rat = (AT_MTA_LowPwrModeRatUint8)g_atParaList[0].paraValue;

    /* ��MTA����Ϣ */
    if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                               ID_AT_MTA_LOW_PWR_MODE_SET_REQ, (VOS_UINT8 *)&lowPwrModeReq, sizeof(lowPwrModeReq),
                               I0_UEPS_PID_MTA) == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LOWPWRMODE_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif


