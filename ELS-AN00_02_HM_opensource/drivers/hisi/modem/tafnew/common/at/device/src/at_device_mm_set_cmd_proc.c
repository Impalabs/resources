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
#include "at_device_mm_set_cmd_proc.h"
#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "taf_app_mma.h"
#include "at_lte_common.h"
#include "nv_stru_gucnas.h"
#include "at_mdrv_interface.h"


#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_MM_SET_CMD_PROC_C

#define AT_PSTANDBY_PARA_NUM 2
#define AT_PSTANDBY_STANDBY_TIME 0
#define AT_PSTANDBY_SWITCH_TIME 1
#define TASK_DELAY_MILL_SECONDS 10

VOS_UINT32 At_SetSDomainPara(VOS_UINT8 indexNum)
{
    TAF_MMA_ServiceDomainUint8 srvDomain;
    NAS_NVIM_MsClass msClass;
    ModemIdUint16    modemId = MODEM_ID_0;
    VOS_UINT32       ret;

    (VOS_VOID)memset_s(&msClass, sizeof(msClass), 0x00, sizeof(NAS_NVIM_MsClass));

    /* ����״̬���ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* ���ݱ���δ��� */
    if (g_atDataLocked == VOS_TRUE) {
        return AT_DATA_UNLOCK_ERROR;
    }

    /* ����������Ϊ1 */
    if (g_atParaIndex != 1) {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* �������Ȳ�Ϊ1 */
    if (g_atParaList[0].paraLen != 1) {
        return AT_DEVICE_OTHER_ERROR;
    }

    srvDomain = g_atParaList[0].para[0];

    /* ucSrvDomainȡֵ��ΧΪ'0'~'3' */
    if ((srvDomain >= '0') && (srvDomain <= '3')) {
        /* ���ַ�����ת��Ϊ����,��ΧΪ0~3 */
        srvDomain = g_atParaList[0].para[0] - '0';
    } else {
        return AT_DEVICE_OTHER_ERROR;
    }

    /* �����ֲ���ת��ΪMsClass���ͣ�����3ת��Ϊ0 */
    if (srvDomain == TAF_MMA_SERVICE_DOMAIN_ANY) {
        srvDomain = TAF_MMA_SERVICE_DOMAIN_CS;
    }

    msClass.msClass = srvDomain;
    /* ��ת����Ĳ���ֵд��NV��NV_ITEM_MMA_MS_CLASS��MsClass��Ԫ */
    ret = AT_GetModemIdFromClient(indexNum, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("At_SetSDomainPara: Get modem id fail.");
        return AT_ERROR;
    }
    if (TAF_ACORE_NV_WRITE(modemId, NV_ITEM_MMA_MS_CLASS, (VOS_UINT8 *)&msClass, sizeof(NAS_NVIM_MsClass)) != NV_OK) {
        AT_WARN_LOG("At_SetSDomainPara:WARNING:NVIM Write NV_ITEM_MMA_MS_CLASS failed!");
        return AT_DEVICE_OTHER_ERROR;
    }
    return AT_OK;
}

VOS_UINT32 AT_SetPstandbyPara(VOS_UINT8 indexNum)
{
    DRV_AGENT_PstandbyReq pstandbyInfo;

    TAF_MMA_PhoneModePara phoneModePara;

    /* ^PSTANDBY�����������ҽ���2������: �������״̬��ʱ�䳤�Ⱥ͵���������״̬���л�ʱ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������������ */
    if ((g_atParaList[AT_PSTANDBY_STANDBY_TIME].paraLen == 0) ||
        (g_atParaList[AT_PSTANDBY_SWITCH_TIME].paraLen == 0) || (g_atParaIndex != AT_PSTANDBY_PARA_NUM)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    At_FormatResultData(indexNum, AT_OK); /* ��Ҫ�Ȼظ�OK */

    /* ��֤�������ݷ������ */
    VOS_TaskDelay(TASK_DELAY_MILL_SECONDS);

    /*
     * ���õ����OM�ӿ�ʹ����������״̬:
     * ACPU����ɵĲ�����
     * 1���µ�WIFI
     * 2��LED�µ�
     * 3��USB PowerOff
     * ����Ϣ��C�ˣ�ָʾCCPU����ɵĲ�����
     * 1��ͨ��ģ���µ�
     * 2���ض�ʱ��
     * 3�����ж�
     * 4�����õ���ӿڽ�����˯
     */
    pstandbyInfo.standbyTime = g_atParaList[AT_PSTANDBY_STANDBY_TIME].paraValue;
    pstandbyInfo.switchTime  = g_atParaList[AT_PSTANDBY_SWITCH_TIME].paraValue;

    DRV_PWRCTRL_STANDBYSTATEACPU(pstandbyInfo.standbyTime, pstandbyInfo.switchTime);

    /* ������Ϣ��c�� */
    if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                               DRV_AGENT_PSTANDBY_SET_REQ, (VOS_UINT8 *)&pstandbyInfo, sizeof(pstandbyInfo),
                               I0_WUEPS_PID_DRV_AGENT) != TAF_SUCCESS) {
        AT_ERR_LOG("AT_SetPstandbyPara: AT_FillAndSndAppReqMsg fail.");
    }

    /* V7R2���ùػ�����͹����������� */

    phoneModePara.phMode = TAF_PH_MODE_MINI;

    if (TAF_MMA_PhoneModeSetReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &phoneModePara) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PSTANDBY_SET;

        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    }

    return AT_SUCCESS;
}


