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
#include "at_phone_mm_set_cmd_proc.h"
#include "securec.h"
#include "taf_app_mma.h"
#include "at_ctx.h"
#include "at_cmd_proc.h"
#include "taf_ccm_api.h"
#include "at_data_proc.h"
#include "at_msg_print.h"
#include "at_phone_comm.h"
#include "at_mdrv_interface.h"
#include "gunas_errno.h"
#include "at_check_func.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_MM_SET_CMD_PROC_C

#define AT_COPS_PARA_MAX_NUM 4
#define AT_CFUN_PARA_MAX_NUM 2
#define AT_CFUN_PARA_NUM 2
#define AT_CFUN_FUN 0
#define AT_CFUN_RST 1
#define AT_CFPLMN_OPER 1
#define AT_SYSCFGEX_ACQORDER 0
#define AT_SYSCFGEX_BAND 1
#define AT_SYSCFGEX_ROAM 2
#define AT_SYSCFGEX_SRVDOMAIN 3
#define AT_SYSCFGEX_LTEBAND 4
#define AT_NETSCAN_POWER 1
#define AT_NETSCAN_MODE 2
#define AT_NETSCAN_BAND 3
#define AT_SYSCFG_BAND 2
#define AT_SYSCFG_ROAM 3
#define AT_SYSCFG_SRVDOMAIN 4
#define AT_COPS_MODE 0
#define AT_COPS_OPER 2
#define AT_COPS_RAT 3
#define AT_CPOL_OPER 2
#define AT_CPOL_UTRAN 5
#define AT_CPOL_E_UTRAN 6
#define AT_CPOL_RAT 7
#define AT_CRPN_NAME_INDEX 0
#define AT_CRPN_NAME 1
#define TAF_MMA_RAT_ORDER_INDEX_0 0
#define TAF_MMA_RAT_ORDER_INDEX_1 1
#define TAF_MMA_RAT_ORDER_INDEX_2 2
#define AT_CFPLMN_OPER_MAX_VALID_LEN 6
#define AT_CFPLMN_OPER_MIN_VALID_LEN 5
#define AT_SYSCFG_VALID_NUM 5
#define AT_CRPN_NAME_MIN_LEN 5
#define AT_CRPN_NAME_MAX_LEN 6
#define AT_COPS_SHORT_OPER_LEN 5
#define AT_COPS_FULL_OPER_LEN 6
#define AT_SYSCFG_ROAM_INACTIVE_STATUS_MAX_VALUE 2
#define AT_SYSCFG_ROAM_ACTIVATE_STATUS_MAX_VALUE 3
#define AT_CPAM_PARA_MAX_NUM 2
#define AT_CGCATT_MODE_EQUALED_3_MAX_PARA_NUM 3
#define AT_CGCATT_MODE 1
#define AT_CGCATT_PS_STATE 2
#define AT_CGCATT_PARA_MAX_NUM 2
#define AT_CFPLMN_PARA_MAX_NUM 2
#define AT_UE_MODE_NR_AND_LTE_AT_CPOL_PARA_MAX_NUM 8
#define AT_UE_MODE_NR_OR_LTE_AT_CPOL_PARA_MAX_NUM 7
#define AT_CPOL_PARA_NUM 2
#define AT_CPOL_INDEX 0
#define AT_CPOL_FORMAT 1
#define AT_CPOL_GSM_ACTN 3
#define AT_CPOL_GSM_COMPACT_ACTN 4
#define AT_EMRSSICFG_PARA_NUM 2
#define AT_CRPN_PARA_VALID_NUM 2
#define AT_OTHER_MODE_AT_CPOL_PARA_MAX_NUM 6

#if (VOS_WIN32 == VOS_OS_VER)
VOS_VOID NAS_MSG_SndOutsideContextData_Part1(VOS_VOID);
VOS_VOID NAS_MSG_SndOutsideContextData_Part2(VOS_VOID);
VOS_VOID NAS_MSG_SndOutsideContextData_Part3(VOS_VOID);
#endif

VOS_UINT32 AT_CheckUserRebootSupportFlg(VOS_VOID)
{
    TAF_NVIM_UserRebootSupport userRebootSupport;
    VOS_UINT32                 result;
    VOS_UINT32                 len;

    len = sizeof(TAF_NVIM_UserRebootSupport);

    (VOS_VOID)memset_s(&userRebootSupport, len, 0x00, len);

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_USER_REBOOT_SUPPORT_FLG, &userRebootSupport, len);

    if (result != NV_OK) {
        AT_WARN_LOG("TAF_MMA_IsUserRebootSupport:Read Nvim Failed");
        return VOS_FALSE;
    }

    if (userRebootSupport.userRebootConfig == 0xEF) {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
TAF_UINT32 At_SetCfunPara(TAF_UINT8 indexNum)
{
    TAF_MMA_PhoneModePara phoneModePara;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CFUN_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (((g_atParaList[AT_CFUN_FUN].paraLen == 0) || (g_atParaList[AT_CFUN_RST].paraLen == 0)) &&
        (g_atParaIndex == AT_CFUN_PARA_NUM)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����<fun> */
    if (g_atParaList[AT_CFUN_FUN].paraValue >= TAF_PH_MODE_NUM_MAX) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

/* ���TAF�Ļط����� */
#if (VOS_WIN32 == VOS_OS_VER)
    NAS_MSG_SndOutsideContextData_Part1();
    NAS_MSG_SndOutsideContextData_Part2();
    NAS_MSG_SndOutsideContextData_Part3();
#endif
    /* ���TAF�Ļط����� */

    (VOS_VOID)memset_s(&phoneModePara, sizeof(phoneModePara), 0x00, sizeof(TAF_MMA_PhoneModePara));
    phoneModePara.phMode = (VOS_UINT8)g_atParaList[AT_CFUN_FUN].paraValue;

    if (g_atParaIndex == AT_CFUN_PARA_NUM) {
        phoneModePara.phReset = (VOS_UINT8)g_atParaList[AT_CFUN_RST].paraValue;

        /* NV���ʱAT+CFUN=1,1ֱ����A�˴���������λ */
        if ((AT_CheckUserRebootSupportFlg() == VOS_TRUE) && (phoneModePara.phMode == TAF_PH_MODE_FULL) &&
            (phoneModePara.phReset == TAF_PH_OP_MODE_RESET)) {
            g_atSendDataBuff.bufLen = 0;
            At_FormatResultData(indexNum, AT_OK);
            VOS_TaskDelay(300); /* wait 300 tick */
            (VOS_VOID)TAF_ACORE_NV_FLUSH();
            AT_OmSystemError(NAS_REBOOT_MOD_ID_USER, 0, 0, VOS_NULL_PTR, 0);
            return AT_SUCCESS;
        }
    }

    if (TAF_MMA_PhoneModeSetReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &phoneModePara) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CFUN_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetCsqPara(TAF_UINT8 indexNum)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ִ��������� */
    if (TAF_MMA_QryCsqReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSQ_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_GetOperatorNameFromParam(VOS_UINT16 *operNameLen, VOS_CHAR *operName, VOS_UINT16 bufSize,
                                       VOS_UINT32 formatType)
{
    errno_t memResult;
    if ((formatType == AT_COPS_LONG_ALPH_TYPE) && (bufSize > g_atParaList[AT_COPS_OPER].paraLen) &&
        (g_atParaList[AT_COPS_OPER].paraLen != 0) && (g_atParaList[AT_COPS_OPER].paraLen <= TAF_PH_OPER_NAME_LONG)) {
        memResult = memcpy_s((VOS_CHAR *)operName, TAF_PH_OPER_NAME_LONG, (VOS_CHAR *)g_atParaList[AT_COPS_OPER].para,
                             g_atParaList[AT_COPS_OPER].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_PH_OPER_NAME_LONG, g_atParaList[AT_COPS_OPER].paraLen);
        operName[g_atParaList[AT_COPS_OPER].paraLen] = 0;
        *operNameLen                                 = g_atParaList[AT_COPS_OPER].paraLen;

        return AT_OK;
    } else if ((formatType == AT_COPS_SHORT_ALPH_TYPE) && (bufSize > g_atParaList[AT_COPS_OPER].paraLen) &&
               (g_atParaList[AT_COPS_OPER].paraLen != 0) &&
               (g_atParaList[AT_COPS_OPER].paraLen <= TAF_PH_OPER_NAME_LONG)) {
        memResult = memcpy_s((VOS_CHAR *)operName, TAF_PH_OPER_NAME_LONG, (VOS_CHAR *)g_atParaList[AT_COPS_OPER].para,
                             g_atParaList[AT_COPS_OPER].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_PH_OPER_NAME_LONG, g_atParaList[AT_COPS_OPER].paraLen);
        operName[g_atParaList[AT_COPS_OPER].paraLen] = 0;
        *operNameLen                                 = g_atParaList[AT_COPS_OPER].paraLen;

        return AT_OK;
    } else if ((bufSize > g_atParaList[AT_COPS_OPER].paraLen) &&
               ((g_atParaList[AT_COPS_OPER].paraLen == AT_COPS_SHORT_OPER_LEN) ||
                (g_atParaList[AT_COPS_OPER].paraLen == AT_COPS_FULL_OPER_LEN)) &&
                (g_atParaList[AT_COPS_OPER].paraLen <= TAF_PH_OPER_NAME_LONG)) {
        memResult = memcpy_s((VOS_CHAR *)operName, TAF_PH_OPER_NAME_LONG, (VOS_CHAR *)g_atParaList[AT_COPS_OPER].para,
                             g_atParaList[AT_COPS_OPER].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, TAF_PH_OPER_NAME_LONG, g_atParaList[AT_COPS_OPER].paraLen);
        operName[g_atParaList[AT_COPS_OPER].paraLen] = 0;
        *operNameLen                                 = g_atParaList[AT_COPS_OPER].paraLen;

        return AT_OK;
    } else {
        return AT_CME_INCORRECT_PARAMETERS;
    }
}

LOCAL VOS_UINT32 AT_ProcCopsSetMode0(VOS_UINT8 indexNum)
{
    TAF_MMA_Ctrl     ctrl;
    TAF_PLMN_UserSel plmnUserSel;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(TAF_MMA_Ctrl));
    (VOS_VOID)memset_s(&plmnUserSel, sizeof(plmnUserSel), 0x00, sizeof(TAF_PLMN_UserSel));

    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = g_atClientTab[indexNum].clientId;
    ctrl.opId     = 0;

    plmnUserSel.bgSrchFlag = VOS_FALSE;
    plmnUserSel.srchType   = TAF_MMA_EXTEND_SEARCH_TYPE_FULLBAND;

    if (TAF_MMA_PlmnSearchReq(&ctrl, TAF_MMA_SEARCH_TYPE_AUTO, &plmnUserSel) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_SET_AUTOMATIC;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

LOCAL VOS_UINT32 AT_ProcCopsSetMode1(VOS_UINT8 indexNum)
{
    TAF_MMA_Ctrl         ctrl;
    TAF_PLMN_UserSel     plmnUserSel;
    VOS_UINT32           getOperNameRst;
    TAF_MMA_RatTypeUint8 phRat;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(TAF_MMA_Ctrl));
    (VOS_VOID)memset_s(&plmnUserSel, sizeof(plmnUserSel), 0x00, sizeof(TAF_PLMN_UserSel));

    /* ����Ϊ�� */
    if (g_atParaList[AT_COPS_OPER].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��д�˿���Ϣ */
    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = g_atClientTab[indexNum].clientId;
    ctrl.opId     = 0;

    /* ����<AcT> */
    At_SetCopsActPara(&phRat, g_atParaList[AT_COPS_RAT].paraLen, g_atParaList[AT_COPS_RAT].paraValue);

    plmnUserSel.formatType = (VOS_UINT8)g_atParaList[1].paraValue;
    plmnUserSel.accessMode = phRat;

    plmnUserSel.bgSrchFlag = VOS_FALSE;
    plmnUserSel.srchType   = TAF_MMA_EXTEND_SEARCH_TYPE_FULLBAND;

    /* ��������PLMN�����������浽TAF_PLMN_UserSel�ṹ�У����͵�MMAģ�飬��MMA����ת��������ָ���ѡ� */
    getOperNameRst = AT_GetOperatorNameFromParam(&plmnUserSel.operNameLen, plmnUserSel.operName,
                                                 sizeof(plmnUserSel.operName), plmnUserSel.formatType);

    if (getOperNameRst != AT_OK) {
        return getOperNameRst;
    }

    if (TAF_MMA_PlmnSearchReq(&ctrl, TAF_MMA_SEARCH_TYPE_MANUAL, &plmnUserSel) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_SET_MANUAL;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

LOCAL VOS_UINT32 AT_ProcCopsSetMode2(VOS_UINT8 indexNum)
{
    TAF_MMA_DetachPara detachPara;

    (VOS_VOID)memset_s(&detachPara, sizeof(detachPara), 0x00, sizeof(TAF_MMA_DetachPara));

    detachPara.detachCause = TAF_MMA_DETACH_CAUSE_COPS;

    /* ת���û���DETACH���� */
    detachPara.detachDomain = TAF_MMA_SERVICE_DOMAIN_CS_PS;

    if (TAF_MMA_DetachReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &detachPara) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_SET_DEREGISTER;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

LOCAL VOS_UINT32 AT_ProcCopsSetMode3(VOS_UINT8 indexNum)
{
    TAF_MMA_CopsFormatTypeSetReq copsFormatSetReq;

    (VOS_VOID)memset_s(&copsFormatSetReq, sizeof(copsFormatSetReq), 0x00, sizeof(copsFormatSetReq));

    copsFormatSetReq.copsFormatType = (VOS_UINT8)g_atParaList[1].paraValue;

    Taf_SetCopsFormatTypeReq(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId, &copsFormatSetReq);

    return AT_OK;
}

LOCAL VOS_UINT32 AT_ProcCopsSetMode4(VOS_UINT8 indexNum)
{
    TAF_MMA_Ctrl         ctrl;
    TAF_PLMN_UserSel     plmnUserSel;
    VOS_UINT32           getOperNameRst;
    TAF_MMA_RatTypeUint8 phRat;

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(TAF_MMA_Ctrl));
    (VOS_VOID)memset_s(&plmnUserSel, sizeof(plmnUserSel), 0x00, sizeof(TAF_PLMN_UserSel));

    /* ����Ϊ�� */
    if (g_atParaList[AT_COPS_OPER].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��д�˿���Ϣ */
    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = g_atClientTab[indexNum].clientId;
    ctrl.opId     = 0;

    /* ����<AcT> */
    At_SetCopsActPara(&phRat, g_atParaList[AT_COPS_RAT].paraLen, g_atParaList[AT_COPS_RAT].paraValue);

    plmnUserSel.formatType = (VOS_UINT8)g_atParaList[1].paraValue;
    plmnUserSel.accessMode = phRat;
    plmnUserSel.bgSrchFlag = VOS_FALSE;
    plmnUserSel.srchType   = TAF_MMA_EXTEND_SEARCH_TYPE_FULLBAND;

    /* ��������PLMN�����������浽TAF_PLMN_UserSel�ṹ�У����͵�MMAģ�飬��MMA����ת��������ָ���ѡ� */
    getOperNameRst = AT_GetOperatorNameFromParam(&plmnUserSel.operNameLen, plmnUserSel.operName,
                                                 sizeof(plmnUserSel.operName), plmnUserSel.formatType);

    if (getOperNameRst != AT_OK) {
        return getOperNameRst;
    }

    if (TAF_MMA_PlmnSearchReq(&ctrl, TAF_MMA_SEARCH_TYPE_MANUAL_AUTO, &plmnUserSel) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_SET_MANUAL_AUTOMATIC_MANUAL;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 At_SetCopsPara(TAF_UINT8 indexNum)
{
    VOS_UINT32                  i;
    const AT_UserReselProcTbl   userReselProcTbl[] = {
        /*   user option     reserved     proc func            */
        { 0, 0, AT_ProcCopsSetMode0 }, { 1, 0, AT_ProcCopsSetMode1 }, { 2, 0, AT_ProcCopsSetMode2 },
        { 3, 0, AT_ProcCopsSetMode3 }, { 4, 0, AT_ProcCopsSetMode4 },
    };

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_COPS_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�� */
    if (g_atParaList[AT_COPS_MODE].paraLen == 0) {
        g_atParaList[AT_COPS_MODE].paraValue = 0;
    }

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (At_CheckCurrRatModeIsCL((VOS_UINT8)(g_atClientTab[indexNum].clientId)) == VOS_TRUE) {
        if ((g_atParaList[AT_COPS_MODE].paraValue != 3) && (g_atParaList[AT_COPS_MODE].paraValue != 0)) {
            return AT_CME_OPERATION_NOT_ALLOWED_IN_CL_MODE;
        }
    }
#endif


    for (i = 0; i < sizeof(userReselProcTbl) / sizeof(userReselProcTbl[0]); i++) {
        if (g_atParaList[AT_COPS_MODE].paraValue == userReselProcTbl[i].userResel) {
            return userReselProcTbl[i].userReselProc(indexNum);
        }
    }

    return AT_ERROR;
}

TAF_UINT32 At_SetCgattPara(TAF_UINT8 indexNum)
{
    TAF_MMA_DetachPara detachPara;

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

    /* ִ��������� */
    if (g_atParaList[0].paraValue == 0) {
        /* ִ��Detach���� */
        (VOS_VOID)memset_s(&detachPara, sizeof(detachPara), 0x00, sizeof(TAF_MMA_DetachPara));
        detachPara.detachCause  = TAF_MMA_DETACH_CAUSE_USER_DETACH;
        detachPara.detachDomain = TAF_MMA_SERVICE_DOMAIN_PS;

        if (TAF_MMA_DetachReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &detachPara) == VOS_TRUE) {
            /* ���õ�ǰ�������� */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGATT_DETAACH_SET;
            return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
        } else {
            return AT_ERROR;
        }
    } else {
        /* ִ��Attach���� */
        if (TAF_MMA_AttachReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, TAF_MMA_ATTACH_TYPE_GPRS) ==
            VOS_TRUE) {
            /* ���õ�ǰ�������� */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGATT_ATTACH_SET;
            return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
        } else {
            return AT_ERROR;
        }
    }
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))

VOS_UINT32 At_SetSystemInfo(TAF_UINT8 indexNum)
{
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_MMA_QrySystemInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, VOS_FALSE) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSINFO_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif

VOS_VOID AT_ReadRoamCfgInfoNvim(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx      *netCtx = VOS_NULL_PTR;
    NAS_NVIM_RoamCfgInfo roamFeature;
    VOS_UINT32           rst;
    ModemIdUint16        modemId = MODEM_ID_0;

    /* ��ȡModemId */
    rst = AT_GetModemIdFromClient(indexNum, &modemId);
    if (rst != VOS_OK) {
        AT_ERR_LOG1("AT_ReadRoamCfgInfoNvim:Get ModemID From ClientID fail,ucIndex=%d", indexNum);
        return;
    }

    roamFeature.roamFeatureFlg = NV_ITEM_ACTIVE;
    roamFeature.roamCapability = 0;
#if (FEATURE_MBB_CUST == FEATURE_ON)
    roamFeature.roamRule = NVIM_ROAM_RULE_CMP_PLMN;
#endif
    if (TAF_ACORE_NV_READ(modemId, NV_ITEM_ROAM_CAPA, &roamFeature, sizeof(NAS_NVIM_RoamCfgInfo)) != NV_OK) {
        AT_WARN_LOG("AT_ReadRoamCfgInfoNvim():WARNING: NV_ITEM_ROAM_CAPA Error");

        return;
    }

    if ((roamFeature.roamFeatureFlg != NV_ITEM_ACTIVE) && (roamFeature.roamFeatureFlg != NV_ITEM_DEACTIVE)) {
        roamFeature.roamFeatureFlg = NV_ITEM_DEACTIVE; /* �ݴ�nv������Чֵ���������Թرմ��� */
    }

    if (roamFeature.roamCapability >= TAF_MMA_ROAM_BUTT) {
        roamFeature.roamCapability = TAF_MMA_ROAM_NATIONAL_OFF_INTERNATIONAL_OFF;
    }

    netCtx              = AT_GetModemNetCtxAddrFromModemId(modemId);
    netCtx->roamFeature = roamFeature.roamFeatureFlg;
    netCtx->roamCapa    = roamFeature.roamCapability;
 #if (FEATURE_MBB_CUST == FEATURE_ON)
    if (roamFeature.roamRule >= NVIM_ROAM_RULE_BUTT) {
        roamFeature.roamRule = NVIM_ROAM_RULE_CMP_PLMN;
    }
    netCtx->roamRule = roamFeature.roamRule;
 #endif
}

AT_SYSCFG_RatPrioUint8 AT_GetGuAccessPrioNvim(ModemIdUint16 modemId)
{
    NAS_MMA_NvimAccessMode accessMode;
    AT_SYSCFG_RatPrioUint8 acqorder;

    (VOS_VOID)memset_s(&accessMode, sizeof(accessMode), 0x00, sizeof(NAS_MMA_NvimAccessMode));

    if (TAF_ACORE_NV_READ(modemId, NV_ITEM_MMA_ACCESS_MODE, &accessMode, sizeof(NAS_MMA_NvimAccessMode)) != NV_OK) {
        return AT_SYSCFG_RAT_PRIO_AUTO;
    }

    acqorder = accessMode.accessMode[1];
    return acqorder;
}

VOS_VOID AT_ConvertSysCfgRatAutoOrder(AT_SYSCFG_RatPrioUint8 *acqorder, TAF_MMA_MultimodeRatCfg *sysCfgRatOrder,
    ModemIdUint16 modemId)
{
    /* ����������ȼ����ı���Ĭ�ϴ�NVIM�л�ȡ */
    if (*acqorder == AT_SYSCFG_RAT_PRIO_NO_CHANGE) {
        *acqorder = AT_GetGuAccessPrioNvim(modemId);
    }

    /* ucAcqorderΪ0�Զ�����WCDMA��GSM */

    if (*acqorder == AT_SYSCFG_RAT_PRIO_AUTO) {
        AT_SetDefaultRatPrioList(modemId, sysCfgRatOrder, VOS_FALSE, VOS_FALSE);
    } else if (*acqorder == AT_SYSCFG_RAT_PRIO_WCDMA) {
        sysCfgRatOrder->ratNum      = TAF_PH_MAX_GU_RAT_NUM;
        sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_WCDMA;
        sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_GSM;
        sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;

    } else if (*acqorder == AT_SYSCFG_RAT_PRIO_GSM) {
        sysCfgRatOrder->ratNum      = TAF_PH_MAX_GU_RAT_NUM;
        sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_GSM;
        sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_WCDMA;
        sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;

    } else {
    }
}

VOS_UINT32 AT_ConvertSysCfgRatOrderPara(VOS_UINT8 clientId, AT_SYSCFG_RatTypeUint8 ratMode,
                                        AT_SYSCFG_RatPrioUint8 acqorder, TAF_MMA_SysCfgPara *sysCfgPara)
{
    ModemIdUint16            modemId = MODEM_ID_0;
    VOS_UINT32               rst;
    TAF_MMA_MultimodeRatCfg *sysCfgRatOrder = VOS_NULL_PTR;

    sysCfgRatOrder = &(sysCfgPara->multiModeRatCfg);
    /* ��ȡModemId */
    rst = AT_GetModemIdFromClient(clientId, &modemId);

    if (rst != VOS_OK) {
        AT_ERR_LOG1("AT_ConvertSysCfgRatOrderPara:Get ModemID From ClientID fail,ClientID:", clientId);
        return AT_ERROR;
    }
    switch (ratMode) {
        case AT_SYSCFG_RAT_AUTO:
            AT_ConvertSysCfgRatAutoOrder(&acqorder, sysCfgRatOrder, modemId);
            break;

        case AT_SYSCFG_RAT_GSM:
            /* G��ģ */
            sysCfgRatOrder->ratNum      = 1;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_GSM;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_BUTT;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;

            break;

        case AT_SYSCFG_RAT_WCDMA:
            /* W��ģ */
            sysCfgRatOrder->ratNum      = 1;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_WCDMA;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_BUTT;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;

            break;

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
        case AT_SYSCFG_RAT_1X:
            /* 1X��ģ */
            sysCfgRatOrder->ratNum      = 1;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_1X;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_BUTT;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;

            break;

        case AT_SYSCFG_RAT_HRPD:
            /* HRPD��ģ */
            sysCfgRatOrder->ratNum      = 1;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_HRPD;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_BUTT;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;

            break;

        case AT_SYSCFG_RAT_1X_AND_HRPD:
            /* 1X&HRPD���ģʽ */
            sysCfgRatOrder->ratNum      = 2;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_1X;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_HRPD;
            sysCfgRatOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;

            break;
#endif

        case AT_SYSCFG_RAT_NO_CHANGE:
            sysCfgRatOrder->ratNum         = 0;
            sysCfgPara->ratCfgNoChangeType = TAF_MMA_RAT_NO_CHANGE_CMD_SYSCFG;
            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���سɹ� */
    return AT_OK;
}

#if (FEATURE_MBB_CUST == FEATURE_ON)
LOCAL VOS_VOID AT_SysCfgexSetRoamParaConvert(VOS_UINT8 clientId, VOS_UINT8 *outRoam)
{
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    netCtx = AT_GetModemNetCtxAddrFromClientId(clientId);

    if (netCtx->roamFeature == VOS_FALSE) {
        /* syscfg�������������������ı䣬ʹ��ԭ�������� */
        if (*outRoam == AT_ROAM_FEATURE_OFF_NOCHANGE) {
            *outRoam = netCtx->roamCapa;
        } else {
            *outRoam = (*outRoam == VOS_FALSE) ? TAF_MMA_ROAM_NATIONAL_OFF_INTERNATIONAL_OFF :
                TAF_MMA_ROAM_NATIONAL_ON_INTERNATIONAL_ON;
            AT_WARN_LOG1("AT_SysCfgexSetRoamParaConvert():Convert RoamCapa, outRoam", *outRoam);
        }
    }
}

/*
 * ����˵��: ���SYSCFGEX��acqorder�Ƿ���NV���������б���
 * �������: ratOrder: �������������RAT�б�
 * ���ؽ��: VOS_TRUE: RAT�б���NV���������б���
 *           VOS_FALSE: RAT�б���NV���������б���
 */
LOCAL VOS_UINT32 AT_CheckRatOrder(const AT_SyscfgexRatOrder *ratOrder)
{
    VOS_UINT8 i;
    VOS_UINT8 j;
    VOS_UINT8 *tmpRatOrder = VOS_NULL_PTR;
    AT_ModemMbbSysCfgExRatGroupCtrl *mbbSysCfgExCtrl = AT_GetModemMbbSysCfgExRatGroupCtrl(MODEM_ID_0);

    for (i = 0; i < mbbSysCfgExCtrl->itemNum; i++) {
        /* ������뼼������ƥ�� */
        tmpRatOrder = mbbSysCfgExCtrl->ratGroupList[i];

        for (j = 0; j < ratOrder->ratOrderNum; j++) {
            if ((ratOrder->ratOrder[j] == tmpRatOrder[j]) ||
                ((ratOrder->ratOrder[j] == AT_SYSCFGEX_RAT_NO_CHANGE) &&
                (tmpRatOrder[j] == AT_SYSCFGEX_ACT_NOT_CHANGE))) {
                continue;
            }
            break;
        }
        /* ƥ��ɹ� */
        if ((j == ratOrder->ratOrderNum) && (tmpRatOrder[j] == AT_SYSCFGEX_ACT_INVALID)) {
            return VOS_TRUE;
        }
    }
    return VOS_FALSE;
}

/*
 * ����˵��: ������õ���ʽ�Ƿ���NV50198�б���
 * �������: ratOrder: �������������RAT�б�
 * ���ؽ��: VOS_TRUE: ����Ϸ�
 *           VOS_FALSE: ����Ƿ�
 */
VOS_UINT32 AT_CheckSysCfgExRatRestrict(const AT_SyscfgexRatOrder *ratOrder)
{
    AT_ModemMbbSysCfgExRatGroupCtrl *mbbSysCfgExCtrl = AT_GetModemMbbSysCfgExRatGroupCtrl(MODEM_ID_0);
    /* ��Ҫ�������õĽ��뼼�� */
    if (mbbSysCfgExCtrl->restrictFlg == VOS_TRUE) {
        /* ���б������õ���Ͽ������� */
        return AT_CheckRatOrder(ratOrder);
    }
    return VOS_TRUE;
}

/*
 * ����˵��: ��SYSCFGEX��00����ת��Ϊ��Ӧ����ʽ���
 * �������: ת�������ʽ��ϴ洢�ṹָ��
 */
VOS_UINT32 AT_ConvertAutoModeToRatList(TAF_MMA_MultimodeRatCfg *ratOrder)
{
    VOS_UINT8 ratIdx;
    VOS_UINT8 ratNum = 0;

    const AT_ModemMbbAutoModeRatInfo *autoModeRatInfo = AT_GetModemMbbModeRatInfo(MODEM_ID_0);
    if (autoModeRatInfo->ratNum == 0) {
        return VOS_FALSE;
    }

    for (ratIdx = 0; ratIdx < autoModeRatInfo->ratNum && ratNum < TAF_MMA_RAT_BUTT; ratIdx++) {
        switch (autoModeRatInfo->ratList[ratIdx]) {
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
            case TAF_MMA_RAT_NR: {
                ratOrder->ratOrder[ratNum++] = TAF_MMA_RAT_NR;
                break;
            }
#endif
#if (FEATURE_LTE == FEATURE_ON)
            case TAF_MMA_RAT_LTE: {
                ratOrder->ratOrder[ratNum++] = TAF_MMA_RAT_LTE;
                break;
            }
#endif
            case TAF_MMA_RAT_WCDMA: {
                ratOrder->ratOrder[ratNum++] = TAF_MMA_RAT_WCDMA;
                break;
            }
            case TAF_MMA_RAT_GSM: {
                ratOrder->ratOrder[ratNum++] = TAF_MMA_RAT_GSM;
                break;
            }
            default: {
                AT_WARN_LOG1("AT_ConvertAutoModeToRatList():Convert rat invalid, rat", autoModeRatInfo->ratList[ratIdx]);
                break;
            }
        }
    }
    ratOrder->ratNum = ratNum;
    return VOS_TRUE;
}
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
VOS_UINT32 At_SetSysCfgPara(VOS_UINT8 indexNum)
{
    TAF_MMA_SysCfgPara sysCfgPara;

    VOS_UINT32      rst;
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    AT_PR_LOGI("Rcv Msg");

    (VOS_VOID)memset_s(&sysCfgPara, sizeof(sysCfgPara), 0x00, sizeof(TAF_MMA_SysCfgPara));

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    if (g_atParaIndex != AT_SYSCFG_VALID_NUM) { /* �������� */
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0) ||
        (g_atParaList[AT_SYSCFG_BAND].paraLen == 0) || (g_atParaList[AT_SYSCFG_ROAM].paraLen == 0) ||
        (g_atParaList[AT_SYSCFG_SRVDOMAIN].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    rst = AT_ConvertSysCfgRatOrderPara(indexNum, (VOS_UINT8)g_atParaList[0].paraValue,
                                       (VOS_UINT8)g_atParaList[1].paraValue, &sysCfgPara);

    if (rst != AT_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    rst = AT_ConvertSysCfgExGuBandPara(g_atParaList[AT_SYSCFG_BAND].para, g_atParaList[AT_SYSCFG_BAND].paraLen,
                                       &sysCfgPara.guBand);
    if (rst != AT_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��Ҫ��L��Ƶ������Ϊ���ı� */
    sysCfgPara.userPrio = (VOS_UINT8)g_atParaList[1].paraValue;
    (VOS_VOID)memset_s(&(sysCfgPara.lBand), sizeof(sysCfgPara.lBand), 0x00, sizeof(TAF_USER_SetLtePrefBandInfo));
    sysCfgPara.lBand.bandInfo[0] = TAF_PH_BAND_NO_CHANGE;

    AT_ReadRoamCfgInfoNvim(indexNum);

    if (((netCtx->roamFeature == AT_ROAM_FEATURE_OFF) &&
         (g_atParaList[AT_SYSCFG_ROAM].paraValue > AT_SYSCFG_ROAM_INACTIVE_STATUS_MAX_VALUE)) ||
        ((netCtx->roamFeature == AT_ROAM_FEATURE_ON) &&
         (g_atParaList[AT_SYSCFG_ROAM].paraValue > AT_SYSCFG_ROAM_ACTIVATE_STATUS_MAX_VALUE))) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    sysCfgPara.roam      = (VOS_UINT8)g_atParaList[AT_SYSCFG_ROAM].paraValue;
    sysCfgPara.srvDomain = (VOS_UINT8)g_atParaList[AT_SYSCFG_SRVDOMAIN].paraValue;

#if (FEATURE_MBB_CUST == FEATURE_ON)
    AT_SysCfgexSetRoamParaConvert(indexNum, &(sysCfgPara.roam));
#endif

    /* ִ��������� */
    if (TAF_MMA_SetSysCfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &sysCfgPara) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSCFG_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}
#endif

LOCAL VOS_VOID AT_ConvertSysRatToTafRat(AT_SyscfgexRatOrder *sysCfgExRatOrder, TAF_MMA_MultimodeRatCfg *ratOrder)
{
    VOS_UINT32  i;
    /* �����ԭ��ֵ�Ľ��뼼��ת�� */
    ratOrder->ratNum = sysCfgExRatOrder->ratOrderNum;

    for (i = 0; i < sysCfgExRatOrder->ratOrderNum; i++) {
        switch (sysCfgExRatOrder->ratOrder[i]) {
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
            case AT_SYSCFGEX_RAT_NR:
                ratOrder->ratOrder[i] = TAF_MMA_RAT_NR;
                break;
#endif

#if (FEATURE_LTE == FEATURE_ON)
            case AT_SYSCFGEX_RAT_LTE:
                ratOrder->ratOrder[i] = TAF_MMA_RAT_LTE;
                break;
#endif
            case AT_SYSCFGEX_RAT_WCDMA:
                ratOrder->ratOrder[i] = TAF_MMA_RAT_WCDMA;
                break;

            case AT_SYSCFGEX_RAT_GSM:
                ratOrder->ratOrder[i] = TAF_MMA_RAT_GSM;
                break;
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
            case AT_SYSCFGEX_RAT_1X:
                ratOrder->ratOrder[i] = TAF_MMA_RAT_1X;
                break;

            case AT_SYSCFGEX_RAT_DO:
                ratOrder->ratOrder[i] = TAF_MMA_RAT_HRPD;
                break;
#endif
            default:
                AT_ERR_LOG1("AT_ConvertSysRatToTafRat, invalid rat ", sysCfgExRatOrder->ratOrder[i]);
                break;
        }
    }
}

VOS_VOID At_ConvertSysCfgExRat(VOS_UINT8 clientId, AT_SyscfgexRatOrder *sysCfgExRatOrder,
                               TAF_MMA_SysCfgPara *sysCfgExSetPara)
{
    ModemIdUint16            modemId = MODEM_ID_0;
    VOS_UINT32               rst;
    TAF_MMA_MultimodeRatCfg *ratOrder = VOS_NULL_PTR;

    ratOrder = &(sysCfgExSetPara->multiModeRatCfg);

    /* ��ȡModemID */
    rst = AT_GetModemIdFromClient(clientId, &modemId);
    if (rst != VOS_OK) {
        AT_ERR_LOG1("At_ConvertSysCfgExRat:Get ModemID From ClientID fail,ClientID=%d", clientId);
        return;
    }

    /* ���ԭ��ֵ00�Ľ��뼼��ת�� */
    if ((sysCfgExRatOrder->ratOrderNum == 1) && (sysCfgExRatOrder->ratOrder[0] == AT_SYSCFGEX_RAT_AUTO)) {
#if (FEATURE_MBB_CUST == FEATURE_ON)
        if (AT_ConvertAutoModeToRatList(ratOrder) == VOS_FALSE) {
            AT_SetDefaultRatPrioList(modemId, ratOrder, VOS_TRUE, VOS_TRUE);
        }
#else
        AT_SetDefaultRatPrioList(modemId, ratOrder, VOS_TRUE, VOS_TRUE);
#endif
        return;
    }

    /* ���ԭ��ֵ99�Ľ��뼼��ת�� */
    if ((sysCfgExRatOrder->ratOrderNum == 1) && (sysCfgExRatOrder->ratOrder[0] == AT_SYSCFGEX_RAT_NO_CHANGE)) {
        ratOrder->ratNum                    = 0;
        sysCfgExSetPara->ratCfgNoChangeType = TAF_MMA_RAT_NO_CHANGE_CMD_SYSCFGEX;
        return;
    }
    AT_ConvertSysRatToTafRat(sysCfgExRatOrder, ratOrder);
}

LOCAL VOS_UINT32 AT_CheckSyscfgExRatPara(VOS_UINT8 clientId, AT_SyscfgexRatOrder *syscfgExRatOrder,
    TAF_MMA_SysCfgPara *sysCfgExSetPara)
{

    syscfgExRatOrder->ratOrderNum = AT_MIN(syscfgExRatOrder->ratOrderNum, TAF_PH_MAX_SUPPORT_RAT_NUM);
    /* acqorder�ַ���ģʽ�������ظ� */
    if (At_CheckRepeatedRatOrder(syscfgExRatOrder) == VOS_TRUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#if (FEATURE_MBB_CUST == FEATURE_ON)
    /* ���NV50198Ҫ������������뼼����� */
    if (AT_CheckSysCfgExRatRestrict(syscfgExRatOrder) == VOS_FALSE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#endif
    /* ��SYSCFGEX�Ľ��뼼����ʽת��ΪTAF_MMA_RatOrder�ṹ��ʽ */
    At_ConvertSysCfgExRat(clientId, syscfgExRatOrder, sysCfgExSetPara);

    return AT_OK;
}

VOS_UINT32 AT_ConvertSysCfgExRatOrderPara(VOS_UINT8 clientId, TAF_MMA_SysCfgPara *sysCfgExSetPara)
{
    VOS_UINT32          rst;
    VOS_UINT32          i;
    VOS_UINT32          ratOrder;
    VOS_UINT8           indexNum = 0;
    VOS_UINT32          ratAutoNoChangeExistFlag = VOS_FALSE;
    AT_SyscfgexRatOrder syscfgExRatOrder;

    /* acqorder�ַ�������Ϊ���� */
    if ((g_atParaList[AT_SYSCFGEX_ACQORDER].paraLen % 2) != 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* acqorder�ַ����������� */
    rst = At_CheckNumString(g_atParaList[AT_SYSCFGEX_ACQORDER].para, g_atParaList[AT_SYSCFGEX_ACQORDER].paraLen);
    if (rst != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* acqorder�ַ���������ż��������12Ҳ�Ƿ� */
    if ((g_atParaList[AT_SYSCFGEX_ACQORDER].paraLen / 2) > TAF_PH_MAX_SUPPORT_RAT_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    for (i = 0; i < g_atParaList[AT_SYSCFGEX_ACQORDER].paraLen; i += AT_SYSCFGEX_RAT_MODE_STR_LEN) {
        rst = At_String2Hex(&g_atParaList[AT_SYSCFGEX_ACQORDER].para[i], AT_SYSCFGEX_RAT_MODE_STR_LEN, (VOS_UINT32 *)&ratOrder);
        if (rst != AT_SUCCESS) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* ���ulRatOrder��0x99ʮ������������Ҫת��Ϊʮ���Ƶ�99 */
        if (ratOrder == AT_SYSCFGEX_RAT_NO_CHANGE_HEX_VALUE) {
            ratOrder = AT_SYSCFGEX_RAT_NO_CHANGE;
        }

        /* acqorder�ַ�������00/01/02/03/04/07/08/99����� */
        if (At_CheckSysCfgExRatTypeValid((AT_SyscfgexRatTypeUint8)ratOrder) == VOS_FALSE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (((AT_SyscfgexRatTypeUint8)ratOrder == AT_SYSCFGEX_RAT_AUTO) ||
            ((AT_SyscfgexRatTypeUint8)ratOrder == AT_SYSCFGEX_RAT_NO_CHANGE)) {
            ratAutoNoChangeExistFlag = VOS_TRUE;
        }

        syscfgExRatOrder.ratOrder[indexNum] = (AT_SyscfgexRatTypeUint8)ratOrder;
        indexNum++;
    }

    syscfgExRatOrder.ratOrderNum = indexNum;

    /* acqorder�ַ�����00��99������ֵ����� */
    if ((ratAutoNoChangeExistFlag == VOS_TRUE) && (syscfgExRatOrder.ratOrderNum > 1)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    return AT_CheckSyscfgExRatPara(clientId, &syscfgExRatOrder, sysCfgExSetPara);
}

VOS_UINT32 At_CheckRepeatedRatOrder(AT_SyscfgexRatOrder *syscfgExRatOrder)
{
    VOS_UINT32 i;
    VOS_UINT32 j;

    for (i = 0; i < syscfgExRatOrder->ratOrderNum; i++) {
        for (j = i + 1; j < syscfgExRatOrder->ratOrderNum; j++) {
            if (syscfgExRatOrder->ratOrder[i] == syscfgExRatOrder->ratOrder[j]) {
                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}

VOS_UINT32 At_CheckSysCfgExRatTypeValid(VOS_UINT8 ratOrder)
{
    switch (ratOrder) {
        case AT_SYSCFGEX_RAT_AUTO:
        case AT_SYSCFGEX_RAT_NO_CHANGE:
        case AT_SYSCFGEX_RAT_LTE:
        case AT_SYSCFGEX_RAT_WCDMA:
        case AT_SYSCFGEX_RAT_GSM:
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
        case AT_SYSCFGEX_RAT_1X:
        case AT_SYSCFGEX_RAT_DO:
#endif
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        case AT_SYSCFGEX_RAT_NR:
#endif
            return VOS_TRUE;

        default:
            return VOS_FALSE;
    }
}

VOS_UINT32 AT_ConvertSysCfgExGuBandPara(VOS_UINT8 *para, VOS_UINT16 len, TAF_UserSetPrefBand64 *prefBandPara)
{
    VOS_UINT32 rslt;

    prefBandPara->bandHigh = 0;
    prefBandPara->bandLow  = 0;

    /* ���Ƶ��Ϊ�գ�����ʧ�� */
    if (len == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* Ƶ��ֵС��32λʱ */
    if (len <= AT_HEX_LEN_OF_ULONG_TYPE) {
        rslt = At_String2Hex(para, len, &(prefBandPara->bandLow));
    } else {
        /* 64λƵ�εĸ�32λ�ַ���ת��Ϊ16��������ת��ʧ�ܷ��� */
        rslt = At_String2Hex(para, (len - AT_HEX_LEN_OF_ULONG_TYPE), &(prefBandPara->bandHigh));
        if (rslt == AT_FAILURE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* 64λƵ�εĵ�32λ�ַ���ת��Ϊ16��������ת��ʧ�ܷ���  */
        rslt = At_String2Hex(para + (len - AT_HEX_LEN_OF_ULONG_TYPE), AT_HEX_LEN_OF_ULONG_TYPE,
                             &(prefBandPara->bandLow));
    }

    if (rslt == AT_FAILURE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}

VOS_UINT32 AT_ConvertSysCfgExLteBandPara(VOS_UINT8 *para, VOS_UINT16 len, TAF_USER_SetLtePrefBandInfo *prefBandPara)
{
    TAF_USER_SetLtePrefBandInfo allBand;
    VOS_UINT32                  rslt = AT_SUCCESS;
    VOS_UINT32                  loop;
    VOS_UINT16                  convertCnt;

    convertCnt = len / AT_HEX_LEN_OF_ULONG_TYPE;
    (VOS_VOID)memset_s(&allBand, sizeof(allBand), 0xFF, sizeof(TAF_USER_SetLtePrefBandInfo));
    (VOS_VOID)memset_s(prefBandPara, sizeof(TAF_USER_SetLtePrefBandInfo), 0x00, sizeof(TAF_USER_SetLtePrefBandInfo));

    /* �������������ش��� */
    if ((len > TAF_MMA_LTE_BAND_MAX_LENGTH * AT_HEX_LEN_OF_ULONG_TYPE)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AP�����������Ƶ��Ϊ�գ���ΪƵ�β��� */
    if (len == 0) {
        prefBandPara->bandInfo[0] = TAF_PH_BAND_NO_CHANGE;

        return AT_OK;
    }

    for (loop = 0; loop < convertCnt; loop++) {
        rslt |= At_String2Hex(para + (VOS_UINT16)((len - (loop + 1) * AT_HEX_LEN_OF_ULONG_TYPE)),
                              AT_HEX_LEN_OF_ULONG_TYPE, &(prefBandPara->bandInfo[loop]));
    }

    if ((len % AT_HEX_LEN_OF_ULONG_TYPE) != 0) {
        rslt |= At_String2Hex(para, len - (VOS_UINT16)(convertCnt * AT_HEX_LEN_OF_ULONG_TYPE),
                              &(prefBandPara->bandInfo[convertCnt]));
    }

    if (rslt != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AP������ȫ������ΪFʱ����Ϊ��ȫƵ�� */
    if (VOS_MemCmp(prefBandPara, &allBand, sizeof(TAF_USER_SetLtePrefBandInfo)) == 0) {
        (VOS_VOID)memset_s(prefBandPara, sizeof(TAF_USER_SetLtePrefBandInfo), 0x00, sizeof(TAF_USER_SetLtePrefBandInfo));
        prefBandPara->bandInfo[0] = MN_MMA_LTE_LOW_BAND_ANY;
        prefBandPara->bandInfo[1] = MN_MMA_LTE_HIGH_BAND_ANY;
    }

    return AT_OK;
}

LOCAL VOS_UINT32 AT_IsModeConflictingDomain(TAF_MMA_SysCfgPara *sysCfgExSetPara)
{
    /* ������õ�ģʽ����L ���� NR,�ҷ�������cs only������VOS_TRUE */
    if ((g_atParaList[AT_SYSCFGEX_SRVDOMAIN].paraValue == TAF_MMA_SERVICE_DOMAIN_CS) ||
        (g_atParaList[AT_SYSCFGEX_SRVDOMAIN].paraValue == TAF_MMA_SERVICE_DOMAIN_ANY)) {
        if (AT_IsSpecRatSupported(TAF_MMA_RAT_LTE, &sysCfgExSetPara->multiModeRatCfg) == VOS_TRUE) {
            return VOS_TRUE;
        }
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        if (AT_IsSpecRatSupported(TAF_MMA_RAT_NR, &sysCfgExSetPara->multiModeRatCfg) == VOS_TRUE) {
            return VOS_TRUE;
        }
#endif
    }
    return VOS_FALSE;
}

VOS_UINT32 AT_CheckSysCfgExPara(VOS_UINT8 clientId, TAF_MMA_SysCfgPara *sysCfgExSetPara)
{
    VOS_UINT32      rst;
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    netCtx = AT_GetModemNetCtxAddrFromClientId(clientId);

    /* ����������,�����Ϊ7��������AT_CME_INCORRECT_PARAMETERS */
    if (g_atParaIndex != AT_SYSCFGEX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���acqorder�����Ϸ���,����Ϸ�,����ת������¼,������Ϸ�ֱ�ӷ���ʧ�� */
    rst = AT_ConvertSysCfgExRatOrderPara(clientId, sysCfgExSetPara);

    if (rst != AT_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (sysCfgExSetPara->ratCfgNoChangeType != TAF_MMA_RAT_NO_CHANGE_CMD_SYSCFGEX) {
        sysCfgExSetPara->userPrio = AT_GetSysCfgPrioRat(sysCfgExSetPara);
    }

    /* ������õ�ģʽ����L,�ҷ�������cs only������ʧ�� */

    if (AT_IsModeConflictingDomain(sysCfgExSetPara) == VOS_TRUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���band�����Ϸ���,����Ϸ�,����ת������¼,������Ϸ�ֱ�ӷ���ʧ�� */
    rst = AT_ConvertSysCfgExGuBandPara(g_atParaList[AT_SYSCFGEX_BAND].para, g_atParaList[AT_SYSCFGEX_BAND].paraLen,
                                       &sysCfgExSetPara->guBand);

    if (rst != AT_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��������δ����,��������ȡֵ��Χ0-2; �������Լ���,��������ȡֵ��Χ0-3 */
    AT_ReadRoamCfgInfoNvim(clientId);
    if (((netCtx->roamFeature == AT_ROAM_FEATURE_OFF) &&
         (g_atParaList[AT_SYSCFGEX_ROAM].paraValue > AT_ROAM_FEATURE_OFF_NOCHANGE)) ||
        ((netCtx->roamFeature == AT_ROAM_FEATURE_ON) &&
         (g_atParaList[AT_SYSCFGEX_ROAM].paraValue > TAF_MMA_ROAM_NATIONAL_OFF_INTERNATIONAL_OFF))) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    sysCfgExSetPara->roam = (VOS_UINT8)g_atParaList[AT_SYSCFGEX_ROAM].paraValue;

    /* ���lteband�����Ϸ���,����Ϸ�,����ת������¼,������Ϸ�ֱ�ӷ���ʧ�� */
    rst = AT_ConvertSysCfgExLteBandPara(g_atParaList[AT_SYSCFGEX_LTEBAND].para,
                                        g_atParaList[AT_SYSCFGEX_LTEBAND].paraLen, &sysCfgExSetPara->lBand);
    if (rst != AT_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    sysCfgExSetPara->srvDomain = (VOS_UINT8)g_atParaList[AT_SYSCFGEX_SRVDOMAIN].paraValue;

    return rst;
}

VOS_UINT32 AT_SetSysCfgExPara(VOS_UINT8 indexNum)
{
    TAF_MMA_SysCfgPara sysCfgExSetPara;

    VOS_UINT32 rst;

    AT_PR_LOGI("Rcv Msg");

    (VOS_VOID)memset_s(&sysCfgExSetPara, sizeof(sysCfgExSetPara), 0x00, sizeof(sysCfgExSetPara));

    /* ���������ͺϷ��Լ��,���Ϸ�ֱ�ӷ���ʧ�� */
    rst = AT_CheckSysCfgExPara(indexNum, &sysCfgExSetPara);

    if (rst != AT_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#if (FEATURE_MBB_CUST == FEATURE_ON)
    AT_SysCfgexSetRoamParaConvert(indexNum, &(sysCfgExSetPara.roam));
#endif
    /* ��MMA���н������ɹ�����ʧ�ܣ������ɹ�������������״̬ */
    rst = TAF_MMA_SetSysCfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &sysCfgExSetPara);
    AT_PR_LOGI("Call interface success!");

    if (rst == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSCFG_SET;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

/* begin V7R1 PhaseI Modify */

VOS_UINT32 At_SetSystemInfoEx(TAF_UINT8 indexNum)
{
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (TAF_MMA_QrySystemInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, VOS_TRUE) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_SYSINFOEX_READ;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
/* end V7R1 PhaseI Modify */

VOS_UINT32 AT_ConvertCpamRatOrderPara(VOS_UINT8 clientId, MN_MMA_CpamRatTypeUint8 mode,
                                      MN_MMA_CpamRatPrioUint8 acqorder, TAF_MMA_SysCfgPara *sysCfgSetPara,
                                      AT_SYSCFG_RatPrioUint8 *syscfgRatPrio)
{
    TAF_MMA_MultimodeRatCfg *ratOrder = VOS_NULL_PTR;
    AT_SYSCFG_RatPrioUint8   nvimRatPrio;
    ModemIdUint16            modemId = MODEM_ID_0;
    VOS_UINT32               rst;

    ratOrder = &(sysCfgSetPara->multiModeRatCfg);

    rst = AT_GetModemIdFromClient(clientId, &modemId);
    if (rst != VOS_OK) {
        AT_ERR_LOG1("AT_ConvertCpamRatOrderPara:Get ModemID From ClientID fail,ClientID=%d", clientId);
        return AT_ERROR;
    }

    /* ����������ȼ����ı���Ĭ�ϴ�NVIM�л�ȡ */
    /* ����NVIM�еĽ������ȼ���CPAM�����õĲ�һ����Ҫת��һ�� */
    if (acqorder == MN_MMA_CPAM_RAT_PRIO_NO_CHANGE) {
        nvimRatPrio = AT_GetGuAccessPrioNvim(modemId);
        *syscfgRatPrio = nvimRatPrio;
    } else if (acqorder == MN_MMA_CPAM_RAT_PRIO_GSM) {
        *syscfgRatPrio = AT_SYSCFG_RAT_PRIO_GSM;
    } else {
        *syscfgRatPrio = AT_SYSCFG_RAT_PRIO_WCDMA;
    }

    switch (mode) {
        case MN_MMA_CPAM_RAT_TYPE_WCDMA_GSM:
            if (*syscfgRatPrio == AT_SYSCFG_RAT_PRIO_GSM) {
                /* GSM���� */
                ratOrder->ratNum      = TAF_PH_MAX_GU_RAT_NUM;
                ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_GSM;
                ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_WCDMA;
                ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;
            } else {
                /* WCDMA���� */
                ratOrder->ratNum      = TAF_PH_MAX_GU_RAT_NUM;
                ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_WCDMA;
                ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_GSM;
                ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;
            }

            break;

        case MN_MMA_CPAM_RAT_TYPE_GSM:
            /* G��ģ */
            ratOrder->ratNum      = 1;
            ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_GSM;
            ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_BUTT;
            ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;
            break;

        case MN_MMA_CPAM_RAT_TYPE_WCDMA:
            /* W��ģ */
            ratOrder->ratNum      = 1;
            ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_0] = TAF_MMA_RAT_WCDMA;
            ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_1] = TAF_MMA_RAT_BUTT;
            ratOrder->ratOrder[TAF_MMA_RAT_ORDER_INDEX_2] = TAF_MMA_RAT_BUTT;
            break;

        case MN_MMA_CPAM_RAT_TYPE_NOCHANGE:
            ratOrder->ratNum                  = 0;
            sysCfgSetPara->ratCfgNoChangeType = TAF_MMA_RAT_NO_CHANGE_CMD_CPAM;

            break;

        default:
            return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}
#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))

VOS_UINT32 At_SetCpamPara(VOS_UINT8 indexNum)
{
    TAF_MMA_SysCfgPara sysCfgSetPara;

    VOS_UINT32      rst;
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    (VOS_VOID)memset_s(&sysCfgSetPara, sizeof(sysCfgSetPara), 0x00, sizeof(TAF_MMA_SysCfgPara));

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    rst = AT_OK;

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_CPAM_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen == 0) {
        g_atParaList[0].paraValue = MN_MMA_CPAM_RAT_TYPE_NOCHANGE;
    }

    if (g_atParaList[1].paraLen == 0) {
        g_atParaList[1].paraValue = MN_MMA_CPAM_RAT_PRIO_NO_CHANGE;
    }

    rst = AT_ConvertCpamRatOrderPara(indexNum, (VOS_UINT8)g_atParaList[0].paraValue,
                                     (VOS_UINT8)g_atParaList[1].paraValue, &sysCfgSetPara, &sysCfgSetPara.userPrio);

    if (rst != AT_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* stSysCfgSetPara����������ֵ */
    if (netCtx->roamFeature == AT_ROAM_FEATURE_ON) {
        sysCfgSetPara.roam = TAF_MMA_ROAM_UNCHANGE;
    } else {
        sysCfgSetPara.roam = AT_ROAM_FEATURE_OFF_NOCHANGE;
    }
    sysCfgSetPara.srvDomain       = TAF_MMA_SERVICE_DOMAIN_NOCHANGE;
    sysCfgSetPara.guBand.bandLow  = TAF_PH_BAND_NO_CHANGE;
    sysCfgSetPara.guBand.bandHigh = 0;
    (VOS_VOID)memset_s(&(sysCfgSetPara.lBand), sizeof(sysCfgSetPara.lBand), 0x00, sizeof(TAF_USER_SetLtePrefBandInfo));
    sysCfgSetPara.lBand.bandInfo[0] = TAF_PH_BAND_NO_CHANGE;

    /* ִ��������� */
    if (TAF_MMA_SetSysCfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &sysCfgSetPara) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPAM_SET;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}
#endif

TAF_MMA_ServiceDomainUint8 At_ConvertDetachTypeToServiceDomain(VOS_UINT32 cgcattMode)
{
    switch (cgcattMode) {
        case AT_CGCATT_MODE_PS:
            return TAF_MMA_SERVICE_DOMAIN_PS;

        case AT_CGCATT_MODE_CS:
            return TAF_MMA_SERVICE_DOMAIN_CS;

        case AT_CGCATT_MODE_CS_PS:
            return TAF_MMA_SERVICE_DOMAIN_CS_PS;

        default:
            AT_ERR_LOG("At_ConvertDetachTypeToServiceDomain Detach Type ERROR");
            return TAF_MMA_SERVICE_DOMAIN_CS_PS;
    }
}

TAF_UINT32 At_SetCgcattParaDetach(TAF_UINT8 ucIndex)
{
    TAF_MMA_DetachPara stDetachPara;

    /* ִ��Detach���� */
    (VOS_VOID)memset_s(&stDetachPara, sizeof(stDetachPara), 0x00, sizeof(TAF_MMA_DetachPara));
    stDetachPara.detachCause = TAF_MMA_DETACH_CAUSE_USER_DETACH;

    /* ֻ��detach mode=3ʱ ��֧������������������������������� */
    if (g_atParaList[AT_CGCATT_MODE].paraValue == TAF_MMA_ATTACH_TYPE_GPRS_IMSI) {
        if (g_atParaIndex > AT_CGCATT_MODE_EQUALED_3_MAX_PARA_NUM) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (g_atParaList[AT_CGCATT_PS_STATE].paraValue > 1) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (g_atParaList[AT_CGCATT_PS_STATE].paraValue == 1) {
            stDetachPara.detachCause = TAF_MMA_DETACH_CAUSE_SMS_ANTI_ATTACK;
        }
    } else {
        if (g_atParaIndex > AT_CGCATT_PARA_MAX_NUM) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    /* ת���û���DETACH���� */
    stDetachPara.detachDomain = At_ConvertDetachTypeToServiceDomain(g_atParaList[1].paraValue);

    if (TAF_MMA_DetachReq(WUEPS_PID_AT, g_atClientTab[ucIndex].clientId, 0, &stDetachPara) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        switch (g_atParaList[1].paraValue) {
            case AT_CGCATT_MODE_PS:
                g_atClientTab[ucIndex].cmdCurrentOpt = AT_CMD_CGCATT_PS_DETAACH_SET;
                break;

            case AT_CGCATT_MODE_CS:
                g_atClientTab[ucIndex].cmdCurrentOpt = AT_CMD_CGCATT_CS_DETAACH_SET;
                break;

            default:
                g_atClientTab[ucIndex].cmdCurrentOpt = AT_CMD_CGCATT_PS_CS_DETAACH_SET;
                break;
        }
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

TAF_MMA_AttachTypeUint8 At_ConvertCgcattModeToAttachType(VOS_UINT32 cgcattMode)
{
    switch (cgcattMode) {
        case AT_CGCATT_MODE_PS:
            return TAF_MMA_ATTACH_TYPE_GPRS;

        case AT_CGCATT_MODE_CS:
            return TAF_MMA_ATTACH_TYPE_IMSI;

        case AT_CGCATT_MODE_CS_PS:
            return TAF_MMA_ATTACH_TYPE_GPRS_IMSI;

        default:
            AT_ERR_LOG("At_ConvertCgcattModeToAttachType Attach Oper ERROR");
            return TAF_MMA_ATTACH_TYPE_GPRS_IMSI;
    }
}

TAF_UINT32 At_SetCgcattPara(TAF_UINT8 indexNum)
{
    TAF_MMA_AttachTypeUint8 attachType = TAF_MMA_ATTACH_TYPE_BUTT;

    /* ������� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* detach �����װ���Ӻ��� */
    if (g_atParaList[0].paraValue == 0) {
        return At_SetCgcattParaDetach(indexNum);
    }

    /* �������� */
    if (g_atParaIndex > AT_CGCATT_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ת���û���ATTACH���� */
    attachType = At_ConvertCgcattModeToAttachType(g_atParaList[1].paraValue);

    /* ִ��Attach���� */
    if (TAF_MMA_AttachReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, attachType) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        switch (g_atParaList[1].paraValue) {
            case AT_CGCATT_MODE_PS:
                g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGCATT_PS_ATTACH_SET;
                break;

            case AT_CGCATT_MODE_CS:
                g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGCATT_CS_ATTACH_SET;
                break;

            default:
                g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGCATT_PS_CS_ATTACH_SET;
                break;
        }
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetSrvstPara(TAF_UINT8 indexNum)
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
    atCmd.reqType       = AT_MTA_SET_SRVST_RPT_TYPE;
    atCmd.u.srvstRptFlg = 0;

    if (g_atParaList[0].paraLen != 0) {
        atCmd.u.srvstRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;
    }

    /* ��MTA����^srvst�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

LOCAL VOS_UINT32 AT_CheckSetFplmnPara(VOS_VOID)
{
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return VOS_FALSE;
    }

    /* �������� */
    if (g_atParaIndex > AT_CFPLMN_PARA_MAX_NUM) {
        return VOS_FALSE;
    }

    if (g_atParaList[0].paraLen != 1) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

LOCAL VOS_UINT32 AT_CheckSetFplmnParaOper(TAF_PH_FplmnOperate *fplmnOperate)
{
    if ((g_atParaList[AT_CFPLMN_OPER].paraLen > AT_CFPLMN_OPER_MAX_VALID_LEN) ||
        (g_atParaList[AT_CFPLMN_OPER].paraLen < AT_CFPLMN_OPER_MIN_VALID_LEN)) {
        return VOS_FALSE;
    }
    /* Mcc */
    if (At_String2Hex(g_atParaList[AT_CFPLMN_OPER].para, AT_MCC_LENGTH, &(fplmnOperate->plmn.mcc)) ==AT_FAILURE) {
        return VOS_FALSE;
    }
    /* Mnc */
    if (At_String2Hex(&g_atParaList[AT_CFPLMN_OPER].para[3], g_atParaList[AT_CFPLMN_OPER].paraLen - AT_MNC_LENGTH,
                      &(fplmnOperate->plmn.mnc)) == AT_FAILURE) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

TAF_UINT32 At_SetFplmnPara(TAF_UINT8 indexNum)
{
    TAF_PH_FplmnOperate fplmnOperate;

    if (AT_CheckSetFplmnPara() != VOS_TRUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex == 1) {
        if (g_atParaList[0].paraValue != 0) {
            return AT_CME_INCORRECT_PARAMETERS;
        } else {
            fplmnOperate.plmn.mcc = 0;
            fplmnOperate.plmn.mnc = 0;
            fplmnOperate.cmdType  = TAF_PH_FPLMN_DEL_ALL;
        }
    } else if (g_atParaIndex == AT_CFPLMN_PARA_MAX_NUM) {
        if (AT_CheckSetFplmnParaOper(&fplmnOperate) != VOS_TRUE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        fplmnOperate.plmn.mcc &= 0x0FFF;

        if (g_atParaList[AT_CFPLMN_OPER].paraLen == AT_CFPLMN_OPER_MIN_VALID_LEN) {
            fplmnOperate.plmn.mnc |= 0x0F00;
        }

        fplmnOperate.plmn.mnc &= 0x0FFF;

        if (g_atParaList[0].paraValue == 0) {
            fplmnOperate.cmdType = TAF_PH_FPLMN_DEL_ONE;
        } else if (g_atParaList[0].paraValue == 1) {
            fplmnOperate.cmdType = TAF_PH_FPLMN_ADD_ONE;
        } else {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    } else {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ִ��������� */
    if (TAF_MMA_SetFPlmnInfo(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &fplmnOperate) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CFPLMN_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetModePara(TAF_UINT8 indexNum)
{
    AT_MTA_UnsolicitedRptSetReq atCmd;
    VOS_UINT32                  result;
#if (FEATURE_LTE == FEATURE_ON)
    L4A_IND_Cfg   l4AIndCfgReq;
    ModemIdUint16 modemId;

    modemId = MODEM_ID_0;
#endif

    /* AT��������Ϸ��Լ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));
    atCmd.reqType      = AT_MTA_SET_MODE_RPT_TYPE;
    atCmd.u.modeRptFlg = AT_MODE_RESULT_CODE_NOT_REPORT_TYPE;

    if (g_atParaList[0].paraLen != 0) {
        atCmd.u.modeRptFlg = (VOS_UINT8)g_atParaList[0].paraValue;
    }

    /* ��MTA����^mode�������� */
    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_UNSOLICITED_RPT_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_UnsolicitedRptSetReq), I0_UEPS_PID_MTA);

    if (result != TAF_SUCCESS) {
        return AT_ERROR;
    }

#if (FEATURE_LTE == FEATURE_ON)
    result = AT_GetModemIdFromClient(indexNum, &modemId);

    if (result != VOS_OK) {
        AT_ERR_LOG1("At_SetModePara:Get ModemID From ClientID fail,ClientID=%d", indexNum);
        return AT_ERROR;
    }

    if (AT_IsModemSupportRat(modemId, TAF_MMA_RAT_LTE) == VOS_TRUE) {
        (VOS_VOID)memset_s(&l4AIndCfgReq, sizeof(l4AIndCfgReq), 0x00, sizeof(l4AIndCfgReq));
        l4AIndCfgReq.modeBitValid = 1;
        l4AIndCfgReq.modeBit      = g_atParaList[0].paraValue;

        /* ֪ͨL�˴�mode������ */
        AT_SetLIndCfgReq(indexNum, &l4AIndCfgReq);
    }
#endif

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_UNSOLICITED_RPT_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 At_SetCplsPara(VOS_UINT8 indexNum)
{
    VOS_UINT32              rst;
    MN_PH_PrefPlmnTypeUint8 prefPlmnType;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������Ϊ�� */
    if (g_atParaList[0].paraLen != 0) {
        prefPlmnType = (MN_PH_PrefPlmnTypeUint8)g_atParaList[0].paraValue;
    } else {
        prefPlmnType = MN_PH_PREF_PLMN_UPLMN;
    }

    if (prefPlmnType > MN_PH_PREF_PLMN_HPLMN) {
        return AT_CME_OPERATION_NOT_SUPPORTED;
    }

    /* ������Ϣ ID_TAF_MMA_PREF_PLMN_TYPE_SET_REQ �� MMA ���� */
    rst = TAF_MMA_SetPrefPlmnTypeReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                     &prefPlmnType); /* Ҫ�����õ������������� */

    if (rst == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MMA_SET_PREF_PLMN_TYPE;
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

VOS_UINT32 At_SetCpolPara(VOS_UINT8 indexNum)
{
    TAF_PH_SetPrefplmn prefPlmn;
    VOS_UINT32         rst;
    AT_ModemNetCtx    *netCtx = VOS_NULL_PTR;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    (VOS_VOID)memset_s(&prefPlmn, sizeof(prefPlmn), 0x00, sizeof(prefPlmn));

    /* �������� */
#if (FEATURE_UE_MODE_NR == FEATURE_ON) && (FEATURE_LTE == FEATURE_ON)
    if (g_atParaIndex > AT_UE_MODE_NR_AND_LTE_AT_CPOL_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#elif (FEATURE_LTE == FEATURE_ON) || (FEATURE_UE_MODE_NR == FEATURE_ON)
    if (g_atParaIndex > AT_UE_MODE_NR_OR_LTE_AT_CPOL_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#else
    if (g_atParaIndex > AT_OTHER_MODE_AT_CPOL_PARA_MAX_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }
#endif

    if ((g_atParaIndex == AT_CPOL_PARA_NUM) && (g_atParaList[AT_CPOL_INDEX].paraLen == 0) &&
        (g_atParaList[AT_CPOL_FORMAT].paraLen != 0)) {
        /*
         * ��ǰֻ����<format>�������ǰ��Ӧ��<oper>�ܹ���ȡ�����ȡ������<oper>
         * ��format��Ҫ�ı�
         */
        netCtx->cpolFormatType = (AT_COPS_FORMAT_TYPE)g_atParaList[AT_CPOL_FORMAT].paraValue;
        return AT_OK;
    }

    if (netCtx->prefPlmnType > MN_PH_PREF_PLMN_HPLMN) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������ṹ�У����͸�MMA���д��� */
    prefPlmn.prefPlmnType     = netCtx->prefPlmnType;
    prefPlmn.atParaIndex      = g_atParaIndex;
    prefPlmn.indexLen         = g_atParaList[AT_CPOL_INDEX].paraLen;
    prefPlmn.index            = g_atParaList[AT_CPOL_INDEX].paraValue;
    prefPlmn.formatLen        = g_atParaList[AT_CPOL_FORMAT].paraLen;
    prefPlmn.format           = g_atParaList[AT_CPOL_FORMAT].paraValue;
    prefPlmn.gsmRatLen        = g_atParaList[AT_CPOL_GSM_ACTN].paraLen;
    prefPlmn.gsmRat           = g_atParaList[AT_CPOL_GSM_ACTN].paraValue;
    prefPlmn.gsmCompactRatLen = g_atParaList[AT_CPOL_GSM_COMPACT_ACTN].paraLen;
    prefPlmn.gsmCompactRat    = g_atParaList[AT_CPOL_GSM_COMPACT_ACTN].paraValue;
    prefPlmn.utrnLen          = g_atParaList[AT_CPOL_UTRAN].paraLen;
    prefPlmn.utrn             = g_atParaList[AT_CPOL_UTRAN].paraValue;

#if (FEATURE_UE_MODE_NR == FEATURE_ON) && (FEATURE_LTE == FEATURE_ON)
    prefPlmn.eutrnLen = g_atParaList[AT_CPOL_E_UTRAN].paraLen;
    prefPlmn.eutrn    = g_atParaList[AT_CPOL_E_UTRAN].paraValue;
    prefPlmn.nrRatLen = g_atParaList[AT_CPOL_RAT].paraLen;
    prefPlmn.nrRat    = g_atParaList[AT_CPOL_RAT].paraValue;
#elif (FEATURE_LTE == FEATURE_ON)
    /* LTE�Ľ��뼼���Լ����� */
    prefPlmn.eutrnLen = g_atParaList[AT_CPOL_E_UTRAN].paraLen;
    prefPlmn.eutrn    = g_atParaList[AT_CPOL_E_UTRAN].paraValue;
#elif (FEATURE_UE_MODE_NR == FEATURE_ON)
    prefPlmn.usNrRatLen = g_atParaList[AT_CPOL_E_UTRAN].paraLen;
    prefPlmn.ulNrRat    = g_atParaList[AT_CPOL_E_UTRAN].paraValue;
#else
#endif

    if (prefPlmn.formatLen == 0) {
        prefPlmn.formatLen = 1;
        prefPlmn.format    = AT_COPS_NUMERIC_TYPE;
    }

    netCtx->cpolFormatType = (AT_COPS_FORMAT_TYPE)prefPlmn.format;

    if (g_atParaList[AT_CPOL_OPER].paraLen != 0) {
        rst = AT_GetOperatorNameFromParam(&prefPlmn.operNameLen, prefPlmn.operName, sizeof(prefPlmn.operName),
                                          prefPlmn.format);
        if (rst != AT_OK) {
            return rst;
        }
    }

    if (TAF_MMA_SetCpolReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &prefPlmn) == VOS_TRUE) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPOL_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

TAF_UINT32 At_SetQuickStart(TAF_UINT8 indexNum)
{
    AT_PR_LOGI("Rcv Msg");
    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_ERROR;
    }

    /* �������� */
    if (g_atParaIndex != 1) {
        return AT_ERROR;
    }

    if (g_atParaList[0].paraValue > 1) {
        return AT_ERROR;
    }

    /* ִ��������� */
    if (TAF_MMA_SetQuickStartReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, g_atParaList[0].paraValue) ==
        VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CQST_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetCopnPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 ret;
    VOS_UINT16 fromIndex = 0;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_ERROR;
    }
    ret = TAF_MMA_QryCopnInfoReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, fromIndex, 0);
    if (ret != VOS_TRUE) {
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPN_QRY;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_GetValidNetScanPowerParam(VOS_INT16 *psCellPower)
{
    VOS_UINT32 cellPower = 0;

    /* power���ȴ���4��С��3��ʾ��Ч���� */
    if ((g_atParaList[AT_NETSCAN_POWER].paraLen > AT_NETSCAN_POWER_PARA_MAX_LEN) ||
        (g_atParaList[AT_NETSCAN_POWER].paraLen < AT_NETSCAN_POWER_PARA_MIN_LEN)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �ַ�����һ��������Ϊ'-'����Ϊ��Ч���� */
    if (g_atParaList[AT_NETSCAN_POWER].para[0] != '-') {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* POWER���ַ�Ϊ'-',��ȡ�����ֵ */
    if (atAuc2ul(&g_atParaList[AT_NETSCAN_POWER].para[1],
                 g_atParaList[AT_NETSCAN_POWER].paraLen - 1, &cellPower) != AT_SUCCESS) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����POWERֵΪ��������ת���� */
    *psCellPower = -(VOS_INT16)cellPower;

    /* GU Powerȡֵ�ж�[-110,-47]��LTE/NR Powerȡֵ�ж�[-125,-47] */
    if (*psCellPower > AT_NETSCAN_POWER_MAX_VALUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[AT_NETSCAN_MODE].paraValue == TAF_MMA_NET_SCAN_RAT_MODE_LTE
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        || g_atParaList[AT_NETSCAN_MODE].paraValue == TAF_MMA_NET_SCAN_RAT_MODE_LTE_NRDC ||
        g_atParaList[AT_NETSCAN_MODE].paraValue == TAF_MMA_NET_SCAN_RAT_MODE_NR
#endif
    ) {
        if (*psCellPower < AT_NETSCAN_LTE_POWER_MIN_VALUE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    } else {
        if (*psCellPower < AT_NETSCAN_POWER_MIN_VALUE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    return AT_OK;
}

VOS_UINT32 AT_GetValidNetScanBandPara(TAF_MMA_BandSet *bandSet)
{
    VOS_UINT8 *para = VOS_NULL_PTR;
    VOS_UINT16 len;
    VOS_UINT16 tmpLen;
    VOS_UINT8  indexNum;
    VOS_UINT8  isNullBand;

    para       = g_atParaList[AT_NETSCAN_BAND].para;
    len        = g_atParaList[AT_NETSCAN_BAND].paraLen;
    isNullBand = VOS_TRUE;
    (VOS_VOID)memset_s(bandSet, sizeof(TAF_MMA_BandSet), 0, sizeof(TAF_MMA_BandSet));

    /* ���Ƶ��Ϊ�գ�����Ϊ��ȫƵ������ */
    if (len == 0) {
        if ((g_atParaList[AT_NETSCAN_MODE].paraValue == TAF_MMA_NET_SCAN_RAT_MODE_GSM) ||
            (g_atParaList[AT_NETSCAN_MODE].paraValue == TAF_MMA_NET_SCAN_RAT_MODE_UMTS)) {
            bandSet->band[0] = AT_ALL_GU_BAND;
        } else if (g_atParaList[AT_NETSCAN_MODE].paraValue == TAF_MMA_NET_SCAN_RAT_MODE_LTE) {
            bandSet->band[0] = MN_MMA_LTE_LOW_BAND_ANY;
            bandSet->band[1] = MN_MMA_LTE_HIGH_BAND_ANY;
        } else {
            AT_NORM_LOG1("AT_GetValidNetScanBandPara: <mode>", g_atParaList[AT_NETSCAN_MODE].paraValue);
        }
        return AT_OK;
    }

    /* ת��Ƶ�� */
    for (indexNum = 0; indexNum < TAF_MMA_BAND_MAX_LEN; indexNum++) {
        tmpLen = (len < AT_HEX_LEN_OF_ULONG_TYPE) ? len : AT_HEX_LEN_OF_ULONG_TYPE;

        if (At_String2Hex(para + g_atParaList[AT_NETSCAN_BAND].paraLen - (indexNum * AT_HEX_LEN_OF_ULONG_TYPE) - tmpLen,
                          tmpLen, &(bandSet->band[indexNum])) == AT_FAILURE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        if (bandSet->band[indexNum] != 0) {
            isNullBand = VOS_FALSE;
        }

        if (len <= AT_HEX_LEN_OF_ULONG_TYPE) {
            break;
        }

        len -= AT_HEX_LEN_OF_ULONG_TYPE;
    }

    /* ������ɨ���BAND */
    if (isNullBand == VOS_TRUE) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_OK;
}

VOS_UINT32 AT_GetValidNetScanPara(TAF_MMA_NetScanSetPara *netScanSetPara)
{
    /* ����������,�������4��������AT_CME_INCORRECT_PARAMETERS */
    if ((g_atParaIndex > AT_NETSCAN_PARA_MAX_NUM) || (g_atParaIndex < AT_NETSCAN_PARA_MIN_NUM)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��һ������У�� */
    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����CellNum */
    netScanSetPara->cellNum = (VOS_UINT16)g_atParaList[0].paraValue;

    /* ����CellPow */
    if (AT_GetValidNetScanPowerParam(&(netScanSetPara->cellPow)) != AT_OK) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ȱʡ<mode>����������ȱʡ<band>����������Ϊ�������� */
    if ((g_atParaIndex == AT_NETSCAN_PARA_MAX_NUM) && (g_atParaList[AT_NETSCAN_MODE].paraLen == 0) &&
        (g_atParaList[AT_NETSCAN_BAND].paraLen != 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����û�û������뼼������g_atParaList[2].ulParaValue����0 */
    if (g_atParaList[AT_NETSCAN_MODE].paraLen == 0) {
        netScanSetPara->rat = TAF_MMA_NET_SCAN_RAT_MODE_GSM;
    } else {
        netScanSetPara->rat = (TAF_MMA_NetScanRatModeUint8)g_atParaList[AT_NETSCAN_MODE].paraValue;
    }

    /* ���band�����Ϸ���,����Ϸ�,����ת������¼,������Ϸ�ֱ�ӷ���ʧ�� */
    return AT_GetValidNetScanBandPara(&netScanSetPara->band);
}

VOS_UINT32 AT_SetNetScanPara(VOS_UINT8 indexNum)
{
    VOS_UINT32             rst;
    TAF_MMA_NetScanSetPara netScanSetPara;

    (VOS_VOID)memset_s(&netScanSetPara, sizeof(netScanSetPara), 0x00, sizeof(netScanSetPara));

    rst = AT_GetValidNetScanPara(&netScanSetPara);
    if (rst != AT_OK) {
        TAF_LOG(WUEPS_PID_AT, VOS_NULL, PS_LOG_LEVEL_ERROR,
                "AT_SetNetScanPara:ERROR: AT_GetValidNetScanPara returns FAILURE");
        return rst;
    }

    rst = TAF_MMA_NetScanReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &netScanSetPara);
    if (rst == VOS_TRUE) {
        /* ָʾ��ǰ�û��������������Ϊ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_NETSCAN_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_SetEmRssiCfgPara(VOS_UINT8 indexNum)
{
    TAF_MMA_EmrssicfgReq emRssiCfgPara;

    (VOS_VOID)memset_s(&emRssiCfgPara, sizeof(emRssiCfgPara), 0x00, sizeof(emRssiCfgPara));

    /* ����Ƿ��������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_WARN_LOG("At_SetEmRssiCfgPara: Not Set Command!");
        return AT_ERROR;
    }

    /* ���������� */
    if (g_atParaIndex != AT_EMRSSICFG_PARA_NUM) {
        AT_WARN_LOG("At_SetEmRssiCfgPara: Input parameters go wrong!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����η�װ���ṹ�����ٷ������� */
    emRssiCfgPara.emRssiCfgRat       = (VOS_UINT8)g_atParaList[0].paraValue;
    emRssiCfgPara.emRssiCfgThreshold = (VOS_UINT8)g_atParaList[1].paraValue;

    /* ������Ϣ��C�˴��� */
    if (TAF_MMA_SetEmRssiCfgReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                &emRssiCfgPara) == VOS_TRUE) {
        /* ������������״̬ */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_EMRSSICFG_SET;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        AT_WARN_LOG("At_SetEmRssiCfgPara: AT send ERROR!");
        return AT_ERROR;
    }
}

VOS_UINT32 At_SetCesqPara(TAF_UINT8 indexNum)
{
    VOS_UINT32 result;

    result = TAF_MMA_QryCerssiReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId);

    /* ִ��������� */
    if (result == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CESQ_SET;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_IMS == FEATURE_ON)
VOS_UINT32 AT_InputValueTransToVoiceDomain(VOS_UINT32 value, TAF_MMA_VoiceDomainUint32 *voiceDomain)
{
    VOS_UINT32 rst = VOS_TRUE;

    switch (value) {
        case AT_VOICE_DOMAIN_TYPE_CS_ONLY:
            *voiceDomain = TAF_MMA_VOICE_DOMAIN_CS_ONLY;
            break;

        case AT_VOICE_DOMAIN_TYPE_CS_PREFERRED:
            *voiceDomain = TAF_MMA_VOICE_DOMAIN_CS_PREFERRED;
            break;

        case AT_VOICE_DOMAIN_TYPE_IMS_PS_PREFERRED:
            *voiceDomain = TAF_MMA_VOICE_DOMAIN_IMS_PS_PREFERRED;
            break;

        case AT_VOICE_DOMAIN_TYPE_IMS_PS_ONLY:
            *voiceDomain = TAF_MMA_VOICE_DOMAIN_IMS_PS_ONLY;
            break;

        default:
            *voiceDomain = TAF_MMA_VOICE_DOMAIN_BUTT;
            rst          = VOS_FALSE;
            break;
    }

    return rst;
}

VOS_UINT32 AT_SetCevdpPara(VOS_UINT8 indexNum)
{
    TAF_MMA_VoiceDomainUint32 voiceDomain = TAF_MMA_VOICE_DOMAIN_BUTT;
    VOS_UINT32                rst = VOS_FALSE;

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����Ϊ�գ�Э��û����ȷ�涨Ĭ��ֵ���˴�Ĭ������ΪCS ONLY */
    if (g_atParaList[0].paraLen == 0) {
        voiceDomain = TAF_MMA_VOICE_DOMAIN_CS_ONLY;
    } else {
        if (AT_InputValueTransToVoiceDomain(g_atParaList[0].paraValue, &voiceDomain) != VOS_TRUE) {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    rst = TAF_MMA_SetVoiceDomainReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, voiceDomain);
    if (rst != VOS_TRUE) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_VOICE_DOMAIN_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

VOS_UINT32 AT_SetCindPara(VOS_UINT8 indexNum)
{
    AT_ModemNetCtx   *netCtx = VOS_NULL_PTR;
    TAF_START_InfoInd startInfoInd;

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);
    (VOS_VOID)memset_s(&startInfoInd, sizeof(startInfoInd), 0x00, sizeof(startInfoInd));

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex == 0) {
        return AT_OK;
    }

    /* +CIND=[<ind>] Ŀǰֻ֧��signal������ */
    if (g_atParaList[0].paraLen != 0) {
        netCtx->cerssiReportType = (AT_CerssiReportTypeUint8)g_atParaList[0].paraValue;

        startInfoInd.actionType          = TAF_START_EVENT_INFO_FOREVER;
        startInfoInd.signThreshold       = (VOS_UINT8)g_atParaList[0].paraValue;
        startInfoInd.rrcMsgType          = TAF_EVENT_INFO_CELL_SIGN;
        startInfoInd.minRptTimerInterval = netCtx->cerssiMinTimerInterval;

        if (TAF_MMA_SetCindReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &startInfoInd) == VOS_TRUE) {
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CIND_SET;
            return AT_WAIT_ASYNC_RETURN;
        } else {
            return AT_ERROR;
        }
    }

    return AT_OK;
}

TAF_UINT32 At_SetCrpnPara(TAF_UINT8 indexNum)
{
    TAF_MMA_CrpnQryPara mnMmaCrpnQry;
    errno_t             memResult;

    /* ������Ч�Լ�� */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_CMD_NO_PARA) {
        return AT_OK;
    }

    if (g_atParaIndex != AT_CRPN_PARA_VALID_NUM) {
        AT_WARN_LOG("At_SetCrpnPara: g_atParaIndex != 2.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��<name_index>��<name>����Ϊ�գ��򷵻�ʧ�� */
    if ((g_atParaList[AT_CRPN_NAME_INDEX].paraLen == 0) || (g_atParaList[AT_CRPN_NAME].paraLen == 0)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ���<name>����������Ч�� */
    if (g_atParaList[AT_CRPN_NAME_INDEX].paraValue == 0) {
        if (g_atParaList[AT_CRPN_NAME].paraLen >= TAF_PH_OPER_NAME_LONG) {
            return AT_CME_TEXT_STRING_TOO_LONG;
        }
    } else if (g_atParaList[AT_CRPN_NAME_INDEX].paraValue == 1) {
        if (g_atParaList[AT_CRPN_NAME].paraLen >= TAF_PH_OPER_NAME_SHORT) {
            return AT_CME_TEXT_STRING_TOO_LONG;
        }
    } else {
        if ((g_atParaList[AT_CRPN_NAME].paraLen < AT_CRPN_NAME_MIN_LEN) ||
            (g_atParaList[AT_CRPN_NAME].paraLen > AT_CRPN_NAME_MAX_LEN)) {
            return AT_CME_TEXT_STRING_TOO_LONG;
        }
    }

    (VOS_VOID)memset_s(&mnMmaCrpnQry, sizeof(mnMmaCrpnQry), 0x00, sizeof(TAF_MMA_CrpnQryPara));

    /* ������Ϣ�ṹ��ֵ */
    mnMmaCrpnQry.currIndex   = 0;
    mnMmaCrpnQry.qryNum      = TAF_PH_CRPN_PLMN_MAX_NUM;
    mnMmaCrpnQry.plmnType    = (VOS_UINT8)g_atParaList[AT_CRPN_NAME_INDEX].paraValue;
    mnMmaCrpnQry.plmnNameLen = g_atParaList[AT_CRPN_NAME].paraLen;
    memResult = memcpy_s(mnMmaCrpnQry.plmnName, sizeof(mnMmaCrpnQry.plmnName), g_atParaList[AT_CRPN_NAME].para,
                         g_atParaList[AT_CRPN_NAME].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(mnMmaCrpnQry.plmnName), g_atParaList[AT_CRPN_NAME].paraLen);

    if (TAF_MMA_QryCrpnReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &mnMmaCrpnQry) == VOS_TRUE) {
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CRPN_QUERY;
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

