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
#include "at_device_drv_set_cmd_proc.h"
#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "taf_type_def.h"
#include "taf_drv_agent.h"
#include "at_mta_interface.h"

#include "at_lte_common.h"
#include "at_mdrv_interface.h"

#include "at_device_comm.h"

#include "osm.h"


#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_DRV_SET_CMD_PROC_C

#define AT_MIPIWREX_PARA_NUM 6
#define AT_MIPIWREX_SECONDARY_ID 2
#define AT_MIPIWREX_REG_ADDR 3
#define AT_MIPIWREX_BYTE_CNT 4
#define AT_MIPIWREX_VALUE 5

#define AT_MIPIRDEX_SECONDARY_ID 2
#define AT_MIPIRDEX_REG_ADDR 3
#define AT_MIPIRDEX_BYTE_CNT 4
#define AT_MIPIRDEX_SPEED_TYPE 5

#define AT_ETDFLG_IS_0_REG_ADDR_MAX 31
#define AT_ETDFLG_IS_1_REG_ADDR_MAX 255
#define AT_ETDFLG_IS_0_BYTE_CNT_VALID 1
#define AT_ETDFLG_IS_1_BYTE_CNT_VALID1 1
#define AT_ETDFLG_IS_1_BYTE_CNT_VALID2 2

#define AT_GPIOPL_PL 0
#define AT_DOUBLE_BCD_NUM 2 // ����BCD��ռһ���ֽ�

#define AT_TBAT_VALUE 2
#define AT_TBAT_VALUE_MIN 2
#define AT_TBAT_VALUE_MAX 3
#define AT_TBAT_MAX_PARA_NUM 4
#define AT_TBAT_PARA_TYPE 0
#define AT_TBAT_PARA_OPR 1
#define AT_TBAT_READ_OPR_PARA_NUM 2
#define AT_TBAT_SET_OPR_PARA_NUM 4
#define AT_TBAT_MIN_HKADC 2
#define AT_TBAT_MAX_HKADC 3
#define AT_RSFR_PARA_VALID_NUM 2

VOS_UINT32 g_atNvBackupFlag = 0;

VOS_UINT32 atSetNVFactoryBack(VOS_UINT8 clientId)
{
    VOS_UINT32 rst;

    rst = TAF_ACORE_NV_BACKUP_FNV();
    if (rst != VOS_OK) {
        AT_PR_LOGE("nv Backup faile! %d", rst);

        CmdErrProc(clientId, rst, 0, NULL);

        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "\r%d\r", rst);
    return AT_OK;
}

VOS_UINT32 At_SetGPIOPL(VOS_UINT8 indexNum)
{
    DRV_AGENT_GpioplSetReq gpioplSet;
    errno_t                memResult;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_ERROR;
    }

    /* �����������GPIOPL����20���ֽ�,��:BCD�� > 20*2����ֱ�ӷ��ش��� */
    if (g_atParaList[AT_GPIOPL_PL].paraLen > (AT_GPIOPL_MAX_LEN * AT_DOUBLE_BCD_NUM)) {
        return AT_ERROR;
    }

    /* ���GPIOֵ�Ƿ�Ϊ16�����ַ���,������ֱ�ӷ��ش���;����ת��Ϊ16�������� */
    if (At_AsciiNum2HexString(g_atParaList[AT_GPIOPL_PL].para, &g_atParaList[AT_GPIOPL_PL].paraLen) == AT_FAILURE) {
        return AT_ERROR;
    }

    (VOS_VOID)memset_s(gpioplSet.gpiopl, sizeof(gpioplSet.gpiopl), 0x00, DRVAGENT_GPIOPL_MAX_LEN);

    if (g_atParaList[AT_GPIOPL_PL].paraLen > 0) {
        memResult = memcpy_s(gpioplSet.gpiopl, sizeof(gpioplSet.gpiopl), g_atParaList[AT_GPIOPL_PL].para,
                             g_atParaList[AT_GPIOPL_PL].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(gpioplSet.gpiopl), g_atParaList[AT_GPIOPL_PL].paraLen);
    }

    /* ����AT_FillAndSndAppReqMsg����Ϣ��C��I0_WUEPS_PID_DRV_AGENT */
    if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, DRV_AGENT_GPIOPL_SET_REQ,
                               (VOS_UINT8 *)&gpioplSet, sizeof(gpioplSet), I0_WUEPS_PID_DRV_AGENT) == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_GPIOPL_SET; /* ���õ�ǰ����ģʽ */
        return AT_WAIT_ASYNC_RETURN;                               /* �ȴ��첽�¼����� */
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_WriteWifiNVToDefault(AT_CustomizeItemDfltUint8 customizeItem)
{
    TAF_AT_MultiWifiSec  wifiKey;
    TAF_AT_MultiWifiSsid wifiSsid;

    if (AT_IsMiscSupport(BSP_MODULE_TYPE_WIFI) == BSP_MODULE_UNSUPPORT) {
        return VOS_OK;
    }

    (VOS_VOID)memset_s(&wifiKey, sizeof(wifiKey), 0x00, sizeof(wifiKey));
    (VOS_VOID)memset_s(&wifiSsid, sizeof(wifiSsid), 0x00, sizeof(wifiSsid));

    /* ��ȡWIFI KEY��Ӧ��NV�� */
    if (TAF_ACORE_NV_READ_IN_CCORE(MODEM_ID_0, NV_ITEM_MULTI_WIFI_KEY, (VOS_UINT8 *)&wifiKey,
                                   sizeof(TAF_AT_MultiWifiSec)) != NV_OK) {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault:READ NV ERROR");
        return VOS_ERR;
    }

    /* ����WIKEYΪĬ��ֵ */
    (VOS_VOID)memset_s(wifiKey.wifiWpapsk, sizeof(wifiKey.wifiWpapsk), 0x00, sizeof(wifiKey.wifiWpapsk));

    /* ����WIWEPΪĬ��ֵ */
    (VOS_VOID)memset_s(wifiKey.wifiWepKey1, sizeof(wifiKey.wifiWepKey1), 0x00, sizeof(wifiKey.wifiWepKey1));
    (VOS_VOID)memset_s(wifiKey.wifiWepKey2, sizeof(wifiKey.wifiWepKey2), 0x00, sizeof(wifiKey.wifiWepKey2));
    (VOS_VOID)memset_s(wifiKey.wifiWepKey3, sizeof(wifiKey.wifiWepKey3), 0x00, sizeof(wifiKey.wifiWepKey3));
    (VOS_VOID)memset_s(wifiKey.wifiWepKey4, sizeof(wifiKey.wifiWepKey4), 0x00, sizeof(wifiKey.wifiWepKey4));

    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_MULTI_WIFI_KEY, (VOS_UINT8 *)&wifiKey, sizeof(TAF_AT_MultiWifiSec)) !=
        NV_OK) {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault:WRITE NV ERROR");
        return VOS_ERR;
    }

    /* ��ȡSSID��Ӧ��NV�� */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_MULTI_WIFI_STATUS_SSID, &wifiSsid, sizeof(TAF_AT_MultiWifiSsid)) !=
        VOS_OK) {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault:READ NV ERROR");
        return VOS_ERR;
    }

    /* ����SSIDΪĬ��ֵ */
    (VOS_VOID)memset_s(wifiSsid.wifiSsid, sizeof(wifiSsid.wifiSsid), 0x00, sizeof(wifiSsid.wifiSsid));

    /* д��WIFI SSID��Ӧ��NV�� */
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_MULTI_WIFI_STATUS_SSID, (VOS_UINT8 *)&wifiSsid,
                           sizeof(TAF_AT_MultiWifiSsid)) != VOS_OK) {
        AT_WARN_LOG("At_WriteCustomizeSrvNVToDefault:WRITE NV ERROR");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 At_WriteSimLockNVToDefault(VOS_UINT8 indexNum, AT_CustomizeItemDfltUint8 customizeItem)
{
    TAF_CUSTOM_SimLockPlmnInfo simLockPlmnInfo;
    TAF_CUSTOM_CardlockStatus  cardLockStatus;
    TAF_CUSTOM_SimLockMaxTimes simLockMaxTimes;
    VOS_UINT32                 i;
    errno_t                    memResult;

    VOS_UINT8 range[TAF_PH_SIMLOCK_PLMN_STR_LEN] = {
        AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL, AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL, AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
        AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL, AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL, AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL,
        AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL, AT_SIM_LOCK_PLMN_RANGE_DEFAULT_VAL
    };

    /*
     * �ö������ڿ����������ĵ���Ĭ��ֵ�͵����Զ����Ĭ��ֵ��ͬ
     * NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO��ulStatus:Ĭ��δ����;
     * mnc_num:Ĭ��Ϊ2; range_begin��rang_endĬ��ȫΪ0xAA
     */
    simLockPlmnInfo.status = NV_ITEM_DEACTIVE;
    for (i = 0; i < TAF_MAX_SIM_LOCK_RANGE_NUM; i++) {
        simLockPlmnInfo.simLockPlmnRange[i].mncNum = AT_SIM_LOCK_MNC_NUM_DEFAULT_VAL;
        memResult = memcpy_s(simLockPlmnInfo.simLockPlmnRange[i].rangeBegin,
                             sizeof(simLockPlmnInfo.simLockPlmnRange[i].rangeBegin), range, sizeof(range));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(simLockPlmnInfo.simLockPlmnRange[i].rangeBegin), sizeof(range));
        memResult = memcpy_s(simLockPlmnInfo.simLockPlmnRange[i].rangeEnd,
                             sizeof(simLockPlmnInfo.simLockPlmnRange[i].rangeEnd), range, sizeof(range));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(simLockPlmnInfo.simLockPlmnRange[i].rangeEnd), sizeof(range));
    }

    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO, (VOS_UINT8 *)&simLockPlmnInfo,
                           sizeof(simLockPlmnInfo)) != NV_OK) {
        AT_WARN_LOG("At_WriteSimLockNVToDefault():NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO NV Write Fail!");
        return VOS_ERR;
    }

    /*
     * �ö������ڿ����������ĵ���Ĭ��ֵ�͵����Զ����Ĭ��ֵ��ͬ,
     * NV_ITEM_CARDLOCK_STATUS��ulStatus:Ĭ��Ϊδ����; CardlockStatus:Ĭ��ֵΪ2;
     * RemainUnlockTimes:Ĭ��ֵΪ0
     */
    cardLockStatus.status            = NV_ITEM_DEACTIVE;
    cardLockStatus.cardlockStatus    = TAF_OPERATOR_LOCK_NONEED_UNLOCK_CODE;
    cardLockStatus.remainUnlockTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_CARDLOCK_STATUS, (VOS_UINT8 *)&cardLockStatus, sizeof(cardLockStatus)) !=
        NV_OK) {
        AT_WARN_LOG("At_WriteSimLockNVToDefault():NV_ITEM_CARDLOCK_STATUS NV Write Fail!");
        return VOS_ERR;
    }

    /* ��C�˷�����Ϣ����simlockNV */
    if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, DRV_AGENT_SIMLOCK_NV_SET_REQ,
                               (VOS_UINT8 *)&cardLockStatus, sizeof(cardLockStatus),
                               I0_WUEPS_PID_DRV_AGENT) != TAF_SUCCESS) {
        AT_WARN_LOG("At_WriteSimLockNVToDefault():DRV_AGENT_SIMLOCK_NV_SET_REQ NV Write Fail!");
        return VOS_ERR;
    }

    /*
     * �ö������ڿ����������ĵ���Ĭ��ֵ�͵����Զ����Ĭ��ֵ��ͬ,
     * NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES��ulStatus:Ĭ��Ϊδ���
     * LockMaxTimes:Ĭ������������Ϊ10��
     */
    simLockMaxTimes.status       = NV_ITEM_DEACTIVE;
    simLockMaxTimes.lockMaxTimes = TAF_PH_CARDLOCK_DEFAULT_MAXTIME;
    if (TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES, (VOS_UINT8 *)&simLockMaxTimes,
                           sizeof(simLockMaxTimes)) != NV_OK) {
        AT_WARN_LOG("At_WriteSimLockNVToDefault():NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES NV Write Fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}

static VOS_UINT32 At_WriteVoipApnKeyNvToDefault(AT_CustomizeItemDfltUint8 customizeItem)
{
    TAF_NVIM_ApnKeyList voipApnKey;
    VOS_UINT32 ret;

    (VOS_VOID)memset_s(&voipApnKey, sizeof(voipApnKey), 0x00, sizeof(voipApnKey));
    ret = TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_VOIP_APN_KEY, (VOS_UINT8 *)&voipApnKey, sizeof(voipApnKey));
    if (ret != NV_OK) {
        AT_WARN_LOG("At_WriteVoipApnKeyNvToDefault : WRITE VOIP APN key NV ERROR.");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_UINT32 At_SetCustomizeItemToDefaultVal(VOS_UINT8 indexNum, AT_CustomizeItemDfltUint8 customizeItem)
{
    /*
     * ����WIFI�������Ӧ��en_NV_Item_WIFI_KEY��
     * en_NV_Item_WIFI_STATUS_SSID 2��NV��
     */
    if (At_WriteWifiNVToDefault(customizeItem) != VOS_OK) {
        return VOS_ERR;
    }

    /*
     * ����SimLock�������Ӧ��NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO��
     * NV_ITEM_CARDLOCK_STATUS��NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES 3��NV��
     */
    if (At_WriteSimLockNVToDefault(indexNum, customizeItem) != VOS_OK) {
        return VOS_ERR;
    }

    if (At_WriteVoipApnKeyNvToDefault(customizeItem) != VOS_OK) {
        AT_WARN_LOG("At_WriteVoipApnKeyNvToDefault : WRITE VOIP APN key to default error.");
        return VOS_ERR;
    }

    return VOS_OK;
}

VOS_VOID AT_PhSendRestoreFactParmNoReset(VOS_VOID)
{
    VOS_UINT16 length;
    VOS_UINT32 i;

    for (i = 0; i < AT_MAX_CLIENT_NUM; i++) {
        if (g_atClientTab[i].userType == AT_APP_USER) {
            break;
        }
    }

    /* δ�ҵ�E5 User,�����ϱ� */
    if (i >= AT_MAX_CLIENT_NUM) {
        return;
    }

    length = 0;
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "^NORSTFACT");
    length += (TAF_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

    At_SendResultData((VOS_UINT8)i, g_atSndCodeAddress, length);
}

VOS_UINT32 At_SetCsdfltPara(VOS_UINT8 indexNum)
{
    /* ����״̬��� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_ERROR;
    }

    /* ������������ */
    if (g_atParaIndex != 0) {
        return AT_ERROR;
    }

    /* ����Ƿ������ݱ���,δ���ʱ���س�����Ϣ */
    if (g_atDataLocked == VOS_TRUE) {
        return AT_ERROR;
    }

    /* Ĭ��ֵΪ0 */
    if (At_SetCustomizeItemToDefaultVal(indexNum, AT_CUSTOMIZE_ITEM_DEFAULT_VALUE_FROM_CFGFILE) != VOS_OK) {
        return AT_ERROR;
    }

    /*  �ָ��������ú�֪ͨӦ�ñ���wifi������Ϣ */
    if (AT_IsMiscSupport(BSP_MODULE_TYPE_WIFI) == BSP_MODULE_SUPPORT) {
        AT_PhSendRestoreFactParmNoReset();
    }

    return AT_OK;
}

VOS_UINT32 atSetTBATPara(VOS_UINT8 clientId)
{
    FTM_SetTbatReq tbatSet = {0};
    FTM_RdTbatReq  tbatRd  = {0};

    VOS_UINT8  type;
    VOS_UINT8  opr;
    VOS_UINT16 value = 0;
    VOS_UINT32 rst;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex > AT_TBAT_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    type = (VOS_UINT8)(g_atParaList[0].paraValue);
    opr  = (VOS_UINT8)(g_atParaList[1].paraValue);

    if (opr == 1) {
        /* ���� */
        if (g_atParaList[AT_TBAT_VALUE_MIN].paraLen != 0) {
            tbatSet.opr      = opr;
            tbatSet.type     = type;
            tbatSet.valueMin = (VOS_UINT16)g_atParaList[AT_TBAT_VALUE_MIN].paraValue;
            tbatSet.valueMax = (VOS_UINT16)g_atParaList[AT_TBAT_VALUE_MAX].paraValue;

            HAL_SDMLOG("\n stTbatSet.usValueMin=%d,stTbatSet.usValueMax=%d\n", tbatSet.valueMin, tbatSet.valueMax);

            rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_SET_TBAT_REQ, clientId, (VOS_UINT8 *)(&tbatSet),
                                   sizeof(tbatSet));
            if (rst == AT_SUCCESS) {
                g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_TBAT_SET;
                return AT_WAIT_ASYNC_RETURN;
            }
            return AT_ERROR;
        } else {
            return AT_ERROR;
        }
    } else if (opr == 0) {
        /* ��ѯ */
        value = (VOS_UINT16)g_atParaList[AT_TBAT_VALUE].paraLen;

        tbatRd.opr   = opr;
        tbatRd.type  = type;
        tbatRd.value = value;

        rst = atSendFtmDataMsg(I0_MSP_SYS_FTM_PID, ID_MSG_FTM_RD_TBAT_REQ, clientId, (VOS_UINT8 *)(&tbatRd),
                               sizeof(tbatRd));
        if (rst == AT_SUCCESS) {
            g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_TBAT_READ;
            return AT_WAIT_ASYNC_RETURN;
        }
        return AT_ERROR;

    } else {
        return AT_CME_INCORRECT_PARAMETERS;
    }
}

VOS_UINT32 AT_SetTbatPara(VOS_UINT8 indexNum)
{
#if (FEATURE_LTE == FEATURE_ON)
    return atSetTBATPara(indexNum);

#else
    AT_TbatOperationDirectionUint32 tbatOperdirection;
    VBAT_CALIBART_Type              adcInfo;
    VOS_UINT32                      ret;

    /* ������Ч�Լ�� */
    /* ^TBAT���������������ٴ�2������: �������ͺͷ��� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[AT_TBAT_PARA_TYPE].paraLen == 0) || (g_atParaList[AT_TBAT_PARA_OPR].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /*
     * ^TBAT������������������:
     * ���ֵ�ص�ѹ���ò���������3.7V��Ӧ��HKADCֵ��4.2V��Ӧ��HKADCֵ
     * ���ֵ�ص�ѹ��ѯ������<value1>��<value2>����
     */

    /* V3R2-��֧�ֵ��ģ���ѹ */
    if (g_atParaList[AT_TBAT_PARA_TYPE].paraValue == AT_TBAT_BATTERY_ANALOG_VOLTAGE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    tbatOperdirection = g_atParaList[AT_TBAT_PARA_OPR].paraValue;
    if (tbatOperdirection == AT_TBAT_READ_FROM_UUT) {
        if (g_atParaIndex != AT_TBAT_READ_OPR_PARA_NUM) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    } else {
        if ((g_atParaIndex != AT_TBAT_SET_OPR_PARA_NUM) || (g_atParaList[AT_TBAT_MIN_HKADC].paraLen == 0) ||
            (g_atParaList[AT_TBAT_MAX_HKADC].paraLen == 0)) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    if (tbatOperdirection == AT_TBAT_READ_FROM_UUT) {
        /* ���õ���ӿڻ�ȡ������ֵ�ѹ */
        if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                   DRV_AGENT_HKADC_GET_REQ, VOS_NULL_PTR, 0, I0_WUEPS_PID_DRV_AGENT) == TAF_SUCCESS) {
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_TBAT_SET; /* ���õ�ǰ����ģʽ */
            return AT_WAIT_ASYNC_RETURN;                             /* �ȴ��첽�¼����� */
        } else {
            return AT_ERROR;
        }
    } else {
        /* д���������У׼�õ�NV��90 NV_ITEM_BATTERY_ADC */
        adcInfo.minValue = (VOS_UINT16)g_atParaList[AT_TBAT_MIN_HKADC].paraValue;
        adcInfo.maxValue = (VOS_UINT16)g_atParaList[AT_TBAT_MAX_HKADC].paraValue;
        ret              = TAF_ACORE_NV_WRITE(MODEM_ID_0, NV_ITEM_BATTERY_ADC, &adcInfo, sizeof(adcInfo));
        if (ret != NV_OK) {
            AT_WARN_LOG("AT_SetTbatPara: Fail to write NV NV_ITEM_BATTERY_ADC.");
            return AT_ERROR;
        }

        return AT_OK;
    }
#endif
    /*
     * ���ݲ�����ͬ������������:
     * 1.  ֧���û�����AT^TBAT=1,0��ȡ��ص�ѹ����ֵ��
     * ���õ���/OM�ӿڻ�ȡ�������ֵ
     * 2.  ֧���û�����AT^TBAT=1,1,<value1>,<value2>���õ�ص�ѹ����ֵ�����ڵ��У׼��
     * дУ׼��ѹ��NVID 90(NV_ITEM_BATTERY_ADC)���˴������ʴ�ȷ�ϣ�
     * NV_ITEM_BATTERY_ADC�м�¼������������ֵ����AT�����һ�����������ӳ��
     * 3.4V ��ѹ��Ӧ��ADCֵ
     * 4.2V ��ѹ��Ӧ��ADCֵ
     */
}

#if (FEATURE_LTE == FEATURE_ON)
/* ����NV�ָ� */
VOS_UINT32 At_SetInfoRRS(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_ERROR;
    }

    /* ������������ */
    if (g_atParaIndex != 0) {
        return AT_ERROR;
    }

    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, DRV_AGENT_INFORRS_SET_REQ, VOS_NULL_PTR, 0,
                                 I0_WUEPS_PID_DRV_AGENT);
    if (rst == TAF_SUCCESS) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_INFORRS_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_UINT32 AT_RsfrSimLockProc(VOS_UINT8 *pgcSrcData, VOS_UINT16 *len)
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

    if (NV_OK !=
        TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO, &simLockPlmnInfo, sizeof(simLockPlmnInfo))) {
        return AT_ERROR;
    }

    totalPlmnNum = 0;

    if (simLockPlmnInfo.status == NV_ITEM_DEACTIVE) {
        /* ״̬�Ǽ���ʱ����ʾPlmn����Ϊ0 */
        length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)pgcSrcData, (VOS_CHAR *)pgcSrcData,
            "%s%s:%d%s", g_atCrLf, "^SIMLOCK", totalPlmnNum, g_atCrLf);

        *len = length;
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
            ((mncLen <= AT_MNC_MAX_LEN) && (mncLen >= AT_MNC_MIN_LEN))) {
            abPlmnValidFlg[i] = VOS_TRUE;
            ++totalPlmnNum;
        }
    }

    length = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)pgcSrcData, (VOS_CHAR *)pgcSrcData,
        "%s%s:%d%s", g_atCrLf, "^SIMLOCK", totalPlmnNum, g_atCrLf);

    if (totalPlmnNum == 0) {
        *len = length;
        return AT_OK;
    }

    /* CustomInfoSetting �������Ҫ��ĸ�ʽ */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)pgcSrcData,
        (VOS_CHAR *)pgcSrcData + length, "^SIMLOCK:");

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

        length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)pgcSrcData,
            (VOS_CHAR *)pgcSrcData + length, "%d,%d,%s,%s%s", plmnIdx, simLockPlmnInfo.simLockPlmnRange[i].mncNum,
            asciiNumBegin, asciiNumEnd, g_atCrLf);
    }

    *len = length;

    return AT_OK;
}

VOS_UINT32 AT_SetRsfrPara(VOS_UINT8 indexNum)
{
    errno_t    memResult;
    VOS_UINT32 ret;
    VOS_UINT16 length                                = 0;
    VOS_CHAR   name[AT_RSFR_RSFW_NAME_LEN + 1]       = {0};
    VOS_CHAR   subName[AT_RSFR_RSFW_SUBNAME_LEN + 1] = {0};
    VOS_UINT8 *pcRsfrSrcItem                         = VOS_NULL_PTR;
    VOS_CHAR  *pcRsfrDstItem                         = VOS_NULL_PTR;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* ����������Ϊ���� */
    if (g_atParaIndex != AT_RSFR_PARA_VALID_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��һ������Ϊ�� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[0].paraLen > AT_RSFR_RSFW_NAME_LEN)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    memResult = memcpy_s(name, sizeof(name), g_atParaList[0].para, g_atParaList[0].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(name), g_atParaList[0].paraLen);

    At_UpString((VOS_UINT8 *)name, g_atParaList[0].paraLen);

    if (g_atParaList[1].paraLen > AT_RSFR_RSFW_SUBNAME_LEN) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[1].paraLen > 0) {
        memResult = memcpy_s(subName, sizeof(subName), g_atParaList[1].para, g_atParaList[1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(subName), g_atParaList[1].paraLen);
    }

    if (AT_STRCMP(name, "VERSION") == ERR_MSP_SUCCESS) {
        if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                   DRV_AGENT_VERSION_QRY_REQ, VOS_NULL_PTR, 0, I0_WUEPS_PID_DRV_AGENT) == TAF_SUCCESS) {
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_RSFR_VERSION_QRY;
            return AT_WAIT_ASYNC_RETURN;
        } else {
            return AT_ERROR;
        }
    } else if (AT_STRCMP(name, "SIMLOCK") == ERR_MSP_SUCCESS) {
        pcRsfrSrcItem = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, AT_RSFR_RSFW_MAX_LEN);
        if (pcRsfrSrcItem == VOS_NULL_PTR) {
            return AT_ERROR;
        }

        (VOS_VOID)memset_s(pcRsfrSrcItem, AT_RSFR_RSFW_MAX_LEN, 0x00, AT_RSFR_RSFW_MAX_LEN);

        ret = AT_RsfrSimLockProc(pcRsfrSrcItem, &length);
        if (ret != AT_OK) {
            PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem);
            return ret;
        }

        pcRsfrDstItem = (VOS_CHAR *)PS_MEM_ALLOC(WUEPS_PID_AT, AT_RSFR_RSFW_MAX_LEN);
        if (pcRsfrDstItem == VOS_NULL_PTR) {
            PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem);
            return AT_ERROR;
        }

        (VOS_VOID)memset_s(pcRsfrDstItem, AT_RSFR_RSFW_MAX_LEN, 0x00, AT_RSFR_RSFW_MAX_LEN);

        /* �Ѵ�SIMLOCK�����ж������Ľ���ַ�����Ϊ����Դ */
        AtBase64Encode(pcRsfrSrcItem, length, (VOS_UINT8 *)pcRsfrDstItem);

        g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress, "^RSFR:\"%s\",\"%s\",123,1,1,%s", name, subName, g_atCrLf);

        g_atSendDataBuff.bufLen += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
            (VOS_CHAR *)g_atSndCodeAddress + g_atSendDataBuff.bufLen, "\"%s\"", pcRsfrDstItem);
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem);
        PS_MEM_FREE(WUEPS_PID_AT, pcRsfrDstItem);
        /*lint -restore */

        return AT_OK;
    } else {
        return AT_ERROR;
    }
}

/* NV�������� */
VOS_UINT32 atSetNVBackup(VOS_UINT8 clientId)
{
    VOS_UINT32 rst;
    g_atSendDataBuff.bufLen = 0;

    AT_PR_LOGI("Rcv Msg");

#if (VOS_OS_VER != VOS_WIN32)
    rst = TAF_ACORE_NV_UPGRADE_BACKUP(EN_NVM_BACKUP_FILE);
    AT_PR_LOGI("Call interface success!");
#else
    rst = VOS_OK;
#endif
    if (rst != VOS_OK) {
        CmdErrProc(clientId, rst, 0, NULL);
        return AT_ERROR;
    }
    g_atNvBackupFlag++;
    g_atSendDataBuff.bufLen = (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
        (VOS_CHAR *)g_atSndCodeAddress, "\r%d\r", rst);

    return AT_OK;
}

VOS_UINT32 AT_SetPmuDieSNPara(VOS_UINT8 indexNum)
{
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    AT_MTA_MultiPmuDieIdSet pmicId;
    VOS_UINT32              result;

    memset_s(&pmicId, (VOS_SIZE_T)sizeof(pmicId), 0x00, (VOS_SIZE_T)sizeof(pmicId));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���������� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��д��Ϣ���� */
    pmicId.pmicId = (VOS_UINT8)g_atParaList[0].paraValue;

    /* ������Ϣ��MTA */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                    ID_AT_MTA_MULTI_PMU_DIE_ID_SET_REQ, (VOS_UINT8 *)&pmicId,
                                    (VOS_SIZE_T)sizeof(pmicId), I0_UEPS_PID_MTA);
    /* ����ʧ�� */
    if (result != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetPmuDieSNPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ���ͳɹ������õ�ǰ����ģʽ */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MULTI_PMU_DIE_ID_QRY;

    /* �ȴ��첽����ʱ�䷵�� */
    return AT_WAIT_ASYNC_RETURN;
#else
    return AT_CME_OPERATION_NOT_ALLOWED;
#endif
}


VOS_UINT32 At_SetGnssNtyPara(VOS_UINT8 indexNum)
{
    AT_MTA_Gnssnty atMtaGnssnty;
    VOS_UINT32     rst;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������ȼ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    /* Hi110XоƬ��GNSS�����¸�MODEM��AT^GNSSNTY=1,�˳�GNSSNTY������AT^GNSSNTY=0 */
    atMtaGnssnty.value = g_atParaList[0].paraValue;

    /* ������Ϣ ID_AT_MTA_GNSS_NTY ��MTA��������Ϣ������ stAtMtaGnssnty */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_GNSS_NTY, (VOS_UINT8 *)&atMtaGnssnty,
                                 (VOS_SIZE_T)sizeof(atMtaGnssnty), I0_UEPS_PID_MTA);

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("At_SetGnssNtyPara: send ReqMsg fail");
        return AT_ERROR;
    }

    return AT_OK;
}

VOS_UINT32 At_SetAppDialModePara(VOS_UINT8 indexNum)
{
    VOS_UINT32 status;
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (*systemAppConfig != SYSTEM_APP_WEBUI) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϊ�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    } else {
        if (g_atParaList[0].paraValue == 1) {
            /* ���õ���ӿ�֪ͨӦ�ý��в��� */
            status = AT_USB_ENABLE_EVENT;
        } else if (g_atParaList[0].paraValue == 0) {
            /* ���õ���ӿ�֪ͨӦ�ý��жϿ����� */
            status = AT_USB_SUSPEND_EVENT;
        } else {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    DRV_AT_SETAPPDAILMODE(status);

    return AT_OK;
}


