/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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
/*lint -save -e537 -e718 -e746 -e734 -e958 -e713 -e740*/
#include "at_lte_eventreport.h"
#include "securec.h"
#include "osm.h"
#include "gen_msg.h"
#include "msp_at.h"
#include "at_lte_common.h"
#include "at_cmd_proc.h"
#include "AtParse.h" /* Ϊ����mn_client.h */

#include "gen_common.h"
#include "at_common.h"
#include "at_device_phy_rslt_proc.h"
#include "at_custom_as_rslt_proc.h"
#include "at_phone_as_rslt_proc.h"
#include "at_custom_phy_rslt_proc.h"
#include "at_custom_event_report.h"
#include "at_phone_mm_rslt_proc.h"
#include "at_custom_taf_rslt_proc.h"
#include "at_general_mm_rslt_proc.h"
#include "at_phone_event_report.h"
#include "at_general_event_report.h"
#include "at_device_taf_rslt_proc.h"
#include "at_device_drv_rslt_proc.h"
#include "at_device_comm.h"


/*lint -e767 ԭ��:Log��ӡ*/
#define THIS_FILE_ID MSP_FILE_ID_AT_LTE_EVENTREPORT_C
/*lint +e767 */
#define AT_MSG_BLOCK_VALUE_LEN 2
#define AT_NVIM_RSRP_VALUE_INDEX_1 1
#define AT_NVIM_RSRP_VALUE_INDEX_2 2
#define AT_NVIM_RSRP_VALUE_INDEX_3 3

#define AT_ANTENNA_LEVEL_NUM 3


VOS_UINT8 g_atAntennaLevel[AT_ANTENNA_LEVEL_NUM];
VOS_UINT8 g_antennaLevel = 0;

/* ***************************************************************************** */

/* ��������: atCsqInfoIndProc */

/* ��������: ����CSQ�����ϱ���Ϣ */

/*  */

/* ����˵��: pMsgBlock��MSP MSG��Ϣ�ṹ */

/*  */

/* �� �� ֵ: */

/*  */

/* ����Ҫ��: TODO: ... */

/* ���þ���: TODO: ... */


/* ***************************************************************************** */

/* ����V1�е�#define MSP_HAVE_AT_RSSI_REPORT�޸ĵ� */

VOS_UINT8 AT_CalculateLTEAntennaLevel(VOS_INT16 rsrp)
{
    VOS_INT16 level = 0;
    /* ���߸�ʽ��ʾ���� */

    if (rsrp <= g_rsrpCfg.value[AT_NVIM_RSRP_VALUE_INDEX_3]) {
        level = AT_CMD_ANTENNA_LEVEL_4;
    } else if ((g_rsrpCfg.value[AT_NVIM_RSRP_VALUE_INDEX_3] < rsrp) &&
               (rsrp <= g_rsrpCfg.value[AT_NVIM_RSRP_VALUE_INDEX_2])) {
        level = AT_CMD_ANTENNA_LEVEL_3;
    } else if ((g_rsrpCfg.value[AT_NVIM_RSRP_VALUE_INDEX_2] < rsrp) &&
               (rsrp <= g_rsrpCfg.value[AT_NVIM_RSRP_VALUE_INDEX_1])) {
        level = AT_CMD_ANTENNA_LEVEL_2;
    } else {
        level = AT_CMD_ANTENNA_LEVEL_1;
    }
    return (VOS_UINT8)level;
}

VOS_UINT8 AT_GetSmoothLTEAntennaLevel(AT_CmdAntennaLevelUint8 level)
{
    VOS_UINT32 i;
    /* ����ʱ ��������  */
    if (level == AT_CMD_ANTENNA_LEVEL_0) {
        g_antennaLevel = level;

        (VOS_VOID)memset_s(g_atAntennaLevel, sizeof(g_atAntennaLevel), 0x00, sizeof(g_atAntennaLevel));
        return g_antennaLevel;
    }

    /* �������з���״̬  ��������  */
    if (g_antennaLevel == AT_CMD_ANTENNA_LEVEL_0) {
        g_antennaLevel = level;

        memset_s(g_atAntennaLevel, sizeof(g_atAntennaLevel), 0x00, sizeof(g_atAntennaLevel));
        g_atAntennaLevel[AT_ANTENNA_LEVEL_NUM - 1] = level;
        return g_antennaLevel;
    }

    /* ���ϴε��źŸ����Ƚ�, �仯�Ƚϴ�(����1��)���������� */
    if (level > (g_antennaLevel + 1)) {
        g_antennaLevel = level;

        memset_s(g_atAntennaLevel, sizeof(g_atAntennaLevel), 0x00, sizeof(g_atAntennaLevel));
        g_atAntennaLevel[AT_ANTENNA_LEVEL_NUM - 1] = level;
        return g_antennaLevel;
    } else if ((level + 1) < g_antennaLevel) {
        g_antennaLevel = level;

        memset_s(g_atAntennaLevel, sizeof(g_atAntennaLevel), 0x00, sizeof(g_atAntennaLevel));
        g_atAntennaLevel[AT_ANTENNA_LEVEL_NUM - 1] = level;
        return g_antennaLevel;
    } else {
        /* Do nothing... */
    }

    /* �Ƚ��ȳ������3�εĲ�ѯ��� */
    for (i = 0; i < AT_ANTENNA_LEVEL_NUM - 1; i++) {
        g_atAntennaLevel[i] = g_atAntennaLevel[i + 1];
    }
    g_atAntennaLevel[i] = level;

    /* ���������򲻸��£��Դﵽƽ����Ч�� */
    for (i = 0; i < AT_ANTENNA_LEVEL_NUM - 1; i++) {
        if (g_atAntennaLevel[i] != g_atAntennaLevel[i + 1]) {
            return g_antennaLevel;
        }
    }

    /* �źŸ����ȶ��� AT_ANTENNA_LEVEL_MAX_NUM ��ʱ�������� */
    g_antennaLevel = level;
    return g_antennaLevel;
}

VOS_UINT32 atLcacellInd(struct MsgCB *msgBlock)
{
    L4A_READ_LcacellInd *lcacell = NULL;
    VOS_UINT16           length  = 0;
    VOS_UINT32           statCnt = 0;

    lcacell = (L4A_READ_LcacellInd *)msgBlock;

    /* ������ͷβ���\r\n */
    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddr,
        (VOS_CHAR *)g_atSndCodeAddr + length, "%s^LCACELLURC: ", g_atCrLf);

    for (statCnt = 0; statCnt < CA_MAX_CELL_NUM; statCnt++) {
        if (statCnt != 0) {
            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddr,
                (VOS_CHAR *)g_atSndCodeAddr + length, ",");
        }

        length +=
            (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddr,
                (VOS_CHAR *)g_atSndCodeAddr + length, "\"%d %d %d %d\"", statCnt,
                lcacell->lcacellInfo[statCnt].ulConfigured, lcacell->lcacellInfo[statCnt].dlConfigured,
                lcacell->lcacellInfo[statCnt].actived);
    }

    length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddr,
        (VOS_CHAR *)g_atSndCodeAddr + length, "%s", g_atCrLf);

    At_SendResultData(AT_BROADCAST_CLIENT_INDEX_MODEM_0, g_atSndCodeAddr, length);

    return AT_FW_CLIENT_STATUS_READY;
}

VOS_UINT32 at_CsqInfoProc(struct MsgCB *msgBlock, AT_AnlevelInfoCnf *anlevelAnqueryInfo)
{
    VOS_UINT32             result  = ERR_MSP_SUCCESS;
    L4A_CSQ_INFO_IND_STRU *csqInfo = NULL;
    VOS_UINT16             rsrp;
    VOS_UINT8              curAntennaLevel;

    if ((msgBlock == NULL) || (anlevelAnqueryInfo == NULL)) {
        HAL_SDMLOG("pMsgBlock is NULL!\n");
        return AT_ERROR;
    }
    csqInfo                       = (L4A_CSQ_INFO_IND_STRU *)msgBlock;
    anlevelAnqueryInfo->clientId  = csqInfo->clientId;
    anlevelAnqueryInfo->errorCode = csqInfo->errorCode;

    /* RSSI ��ʽ��ʾ���� */

    if (csqInfo->rssi == AT_RSSI_UNKNOWN) {
        anlevelAnqueryInfo->rssi = AT_RSSI_UNKNOWN;
    } else if (csqInfo->rssi >= AT_RSSI_HIGH) {
        anlevelAnqueryInfo->rssi = AT_CSQ_RSSI_HIGH;
    } else if (csqInfo->rssi <= AT_RSSI_LOW) {
        anlevelAnqueryInfo->rssi = AT_CSQ_RSSI_LOW;
    } else {
        anlevelAnqueryInfo->rssi = (VOS_UINT16)((csqInfo->rssi - AT_RSSI_LOW) / 2);
    }

    /*
     * �ϱ�����ת��:�� Rscp��Ecio��ʾΪ�Ǹ�ֵ����Rscp��EcioΪ-145��-32������rssiΪ99��
     * ��ת��Ϊ0
     */
    if ((csqInfo->rsrp == AT_RSSI_UNKNOWN) || (csqInfo->rssi == AT_RSSI_UNKNOWN)) {
        /* ��������0, ��ӦӦ�õ�Ȧ�� */
        curAntennaLevel = AT_CMD_ANTENNA_LEVEL_0;
    } else {
        /* ȡ����ֵ */
        rsrp = (VOS_UINT16)(-(csqInfo->rsrp));
        /* ���ú���AT_CalculateAntennaLevel������D25�㷨������źŸ��� */
        curAntennaLevel = AT_CalculateLTEAntennaLevel((VOS_INT16)rsrp);
    }

    /* �źŴ��ʹ��� */
    anlevelAnqueryInfo->level = AT_GetSmoothLTEAntennaLevel(curAntennaLevel);

    anlevelAnqueryInfo->rsrp = csqInfo->rsrp;
    anlevelAnqueryInfo->rsrq = csqInfo->rsrq;

    return result;
}

/* ***************************************************************************** */

/* ��������: atSetCsqCnfProc */

/* ��������: ����SET CSQ����ظ���Ϣ */

/*  */

/* ����˵��: pMsgBlock��MSP MSG��Ϣ�ṹ */

/*  */

/* �� �� ֵ: */

/*  */

/* ����Ҫ��: TODO: ... */

/* ���þ���: TODO: ... */


/* ***************************************************************************** */

/* ���������AT������еĺ���������ͬ���ǲ���Ҫ���¸����� */


typedef VOS_UINT32 (*AT_L4A_MSG_FUN)(struct MsgCB *pMsgBlock);

/*lint -e958 ԭ��:ֻ��A��ʹ�� */
typedef struct {
    VOS_UINT32     msgId;
    AT_L4A_MSG_FUN atL4aMsgProc;
} AT_L4A_MSG_FUN_TABLE_STRU;
/*lint +e958 */
static const AT_L4A_MSG_FUN_TABLE_STRU g_atL4aCnfMsgFunTable[] = {
    { ID_MSG_L4A_CSQ_INFO_CNF, atSetCsqCnfSameProc },
    { ID_MSG_L4A_ANQUERY_INFO_CNF, atSetAnlevelCnfSameProc },
    { ID_MSG_L4A_CELL_ID_CNF, atQryCellIdCnfProc },
    { ID_MSG_L4A_LWCLASHQRY_CNF, atLwclashCnfProc },
    { ID_MSG_L4A_RADVER_SET_CNF, atSetRadverCnfProc },
#if (FEATURE_MBB_CUST == FEATURE_OFF)
    { ID_MSG_L4A_CELL_INFO_CNF, atQryCellInfoCnfProc },
#else
    { ID_MSG_L4A_CELL_INFO_CNF, AT_MbbQryCellInfoCnfProc },
#endif
    { ID_MSG_L4A_CERSSI_INQ_CNF, atCerssiInfoCnfProc },
    { ID_MSG_L4A_LCACELLQRY_CNF, atLcacellCnfProc },
    { ID_MSG_L4A_LCASCELLINFO_QRY_CNF, atScellInfoQryCnfProc },
};

static const AT_L4A_MSG_FUN_TABLE_STRU g_atL4aIndMsgFunTable[] = {
    { ID_MSG_L4A_RSSI_IND, atCsqInfoIndProc },      { ID_MSG_L4A_ANLEVEL_IND, atAnlevelInfoIndProc },
    { ID_MSG_L4A_CERSSI_IND, atCerssiInfoIndProc },
    { ID_MSG_L4A_LWCLASH_IND, atLwclashInd },       { ID_MSG_L4A_LCACELL_IND, atLcacellInd },
};

/* ***************************************************************************** */

/* ��������: atL4aGetMsgFun */

/* ��������: ��ȡ����L4A�ظ���Ϣ�ĺ��� */

/*  */

/* ����˵��: ulMsgId����ϢID */

/*  */

/* �� �� ֵ: */

/*  */

/* ����Ҫ��: TODO: ... */

/* ���þ���: TODO: ... */


/* ***************************************************************************** */

AT_L4A_MSG_FUN_TABLE_STRU* atL4aGetCnfMsgFun(VOS_UINT32 msgId)
{
    VOS_UINT32                       i = 0;
    VOS_UINT32                       tableLen;
    const AT_L4A_MSG_FUN_TABLE_STRU *table = g_atL4aCnfMsgFunTable;

    tableLen = sizeof(g_atL4aCnfMsgFunTable);

    for (i = 0; i < (tableLen / sizeof(AT_L4A_MSG_FUN_TABLE_STRU)); i++) {
        if (msgId == (table + i)->msgId) {
            return (AT_L4A_MSG_FUN_TABLE_STRU *)(table + i);
        }
    }

    /* ����Ҳ��� */

    return NULL;
}

/* ***************************************************************************** */

/* ��������: atL4aGetIndMsgFun */

/* ��������: ��ȡ����L4A�ظ���Ϣ�ĺ��� */

/*  */

/* ����˵��: ulMsgId����ϢID */

/*  */

/* �� �� ֵ: */

/*  */

/* ����Ҫ��: TODO: ... */

/* ���þ���: TODO: ... */


/* ***************************************************************************** */

AT_L4A_MSG_FUN_TABLE_STRU* atL4aGetIndMsgFun(VOS_UINT32 msgId)
{
    VOS_UINT32                       i = 0;
    VOS_UINT32                       tableLen;
    const AT_L4A_MSG_FUN_TABLE_STRU *table = g_atL4aIndMsgFunTable;

    tableLen = sizeof(g_atL4aIndMsgFunTable);

    for (i = 0; i < (tableLen / sizeof(AT_L4A_MSG_FUN_TABLE_STRU)); i++) {
        if (msgId == (table + i)->msgId) {
            return (AT_L4A_MSG_FUN_TABLE_STRU *)(table + i);
        }
    }

    /* ����Ҳ��� */

    return NULL;
}

/* ***************************************************************************** */

/* ��������: at_L4aCnfProc */

/* ��������: ����L4A��CNF��IND��Ϣ�������Ҷ�Ӧ�������д��� */

/*  */

/* ����˵��: pMsgBlockTmp��VOS��Ϣ */

/*  */

/* �� �� ֵ: */

/*  */

/* ����Ҫ��: TODO: ... */

/* ���þ���: TODO: ... */


/* ***************************************************************************** */

VOS_VOID at_L4aCnfProc(struct MsgCB *msgBlockTmp)
{
    VOS_UINT32                 msgId;
    VOS_UINT16                 indexNum  = 0;
    AT_L4A_MSG_FUN_TABLE_STRU *table     = NULL;
    AT_L4A_MSG_FUN_TABLE_STRU *table2    = NULL;
    AT_L4A_MSG_FUN             msgProc   = NULL;
    L4A_AT_CnfHeader          *msgHeader = VOS_NULL_PTR;

    msgHeader = (L4A_AT_CnfHeader *)msgBlockTmp;
    msgId     = msgHeader->msgId;

    if (At_ClientIdToUserId((VOS_UINT16)msgHeader->clientId, (TAF_UINT8 *)&indexNum) == AT_FAILURE) {
        return;
    }

    msgHeader->clientId = indexNum;
    table               = atL4aGetCnfMsgFun(msgId);
    table2              = atL4aGetIndMsgFun(msgId);
    if ((table != NULL) && (table->atL4aMsgProc)) {
        AT_StopTimerCmdReady((VOS_UINT8)indexNum);
        msgProc = table->atL4aMsgProc;
    } else if ((table2 != NULL) && (table2->atL4aMsgProc)) {
        msgProc = table2->atL4aMsgProc;
    } else {

    }

    if (msgProc != NULL) {
        msgProc(msgBlockTmp);
    }
}

static const AT_FTM_CnfMsgProc g_lteAtFtmCnfMsgTbl[] = {
    { ID_MSG_SYM_SET_TMODE_CNF, atSetTmodeParaCnfProc },
    { ID_MSG_FTM_SET_FCHAN_CNF, atSetFCHANParaCnfProc },
    { ID_MSG_FTM_RD_FCHAN_CNF, atQryFCHANParaCnfProc },
    { ID_MSG_FTM_SET_TXON_CNF, atSetFTXONParaCnfProc },
    { ID_MSG_FTM_RD_TXON_CNF, atQryFTXONParaCnfProc },
    { ID_MSG_FTM_SET_RXON_CNF, atSetFRXONParaCnfProc },
    { ID_MSG_FTM_RD_RXON_CNF, atQryFRXONParaCnfProc },
    { ID_MSG_FTM_SET_FWAVE_CNF, atSetFWAVEParaCnfProc },
    { ID_MSG_FTM_SET_TSELRF_CNF, atSetTselrfParaCnfProc }, /* TSELRF ��ʵ��δ���� */
    { ID_MSG_FTM_SET_AAGC_CNF, atSetFLNAParaCnfProc },
    { ID_MSG_FTM_RD_AAGC_CNF, atQryFLNAParaCnfProc },
    { ID_MSG_FTM_FRSSI_CNF, atQryFRSSIParaCnfProc },
    { ID_MSG_FTM_SET_TBAT_CNF, atSetTbatCnf },
    { ID_MSG_FTM_RD_TBAT_CNF, atRdTbatCnf },
    { ID_MSG_FTM_SET_LTCOMMCMD_CNF, atSetLTCommCmdParaCnfProc },
    { ID_MSG_FTM_RD_LTCOMMCMD_CNF, atQryLTCommCmdParaCnfProc },
    { ID_MSG_FTM_RD_FPLLSTATUS_CNF, atQryFPllStatusParaCnfProc },
};

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
static const AT_FTM_IndMsgProc g_lteAtFtmIndMsgTbl[] = {
    { ID_MSG_FTM_TX_CLT_INFO_IND, At_ProcLteTxCltInfoReport },
};
#endif

/* ***************************************************************************** */

/* ��������: atGetFtmCnfMsgProc */

/* ��������: ���ݻظ�����Ϣ�ҵ�ƥ��Ĵ����� */

/*  */

/* ����˵��: */

/*   MsgId  [in] ��ϢID */

/* �� �� ֵ: */

/*    ��NULL g_lteAtFtmCnfMsgTbl Ԫ�ص�ַ�������˴����� */

/*    NULL ƥ��ʧ�� */


/* ***************************************************************************** */
const AT_FTM_CnfMsgProc* atGetFtmCnfMsgProc(VOS_UINT32 msgId)
{
    VOS_UINT32 i        = 0;
    VOS_UINT32 tableLen = sizeof(g_lteAtFtmCnfMsgTbl) / sizeof(g_lteAtFtmCnfMsgTbl[0]);

    for (i = 0; i < tableLen; i++) {
        if (g_lteAtFtmCnfMsgTbl[i].msgId == msgId) {
            return &(g_lteAtFtmCnfMsgTbl[i]);
        }
    }

    return NULL;
}

const AT_FTM_IndMsgProc* At_GetFtmIndMsgProc(VOS_UINT32 msgId)
{
#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    VOS_UINT32 i;
    VOS_UINT32 tableLen;

    tableLen = sizeof(g_lteAtFtmIndMsgTbl) / sizeof(g_lteAtFtmIndMsgTbl[0]);

    for (i = 0; i < tableLen; i++) {
        if (g_lteAtFtmIndMsgTbl[i].msgId == msgId) {
            return &(g_lteAtFtmIndMsgTbl[i]);
        }
    }
#endif
    return VOS_NULL_PTR;
}

/* ***************************************************************************** */

/* ��������: atGetFtmCnfMsgProc */

/* ��������: AT����LTEװ��FTM�ظ���Ϣ��� */

/*  */

/* ����˵��: */

/*   pMsg  [in] �˼���Ϣ�ṹ */

/* �� �� ֵ: */

/*    ERR_MSP_SUCCESS �ɹ� */

/*    ERR_MSP_FAILURE ʧ�� */

/* ***************************************************************************** */
VOS_VOID At_FtmEventMsgProc(struct MsgCB *msg)
{
    OS_MSG                  *osMsg       = NULL;
    VOS_UINT8               *msgBlock    = NULL;
    const AT_FTM_CnfMsgProc *msgProcItem = NULL;
    AT_FW_DataMsg           *dataMsg     = (AT_FW_DataMsg *)msg;
    VOS_VOID                *tmp         = VOS_NULL_PTR;
    errno_t                  memResult;

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    const AT_FTM_IndMsgProc *ftmIndMsgItem = VOS_NULL_PTR;
#endif

    msgBlock = VOS_MemAlloc(WUEPS_PID_AT, (DYNAMIC_MEM_PT),
                            (sizeof(MsgBlock) + sizeof(OS_MSG) - AT_MSG_BLOCK_VALUE_LEN));
    if (msgBlock == NULL) {
        return;
    }
    (VOS_VOID)memset_s(msgBlock,
        (VOS_SIZE_T)(sizeof(MsgBlock) + sizeof(OS_MSG) - AT_MSG_BLOCK_VALUE_LEN), 0x00,
        (VOS_SIZE_T)(sizeof(MsgBlock) + sizeof(OS_MSG) - AT_MSG_BLOCK_VALUE_LEN));

    /* ��Ϣ�ṹת�� */
    VOS_SET_SENDER_ID(msgBlock, VOS_GET_SENDER_ID(dataMsg));
    osMsg         = (OS_MSG *)(((MsgBlock *)(msgBlock))->value);
    osMsg->msgId  = dataMsg->msgId;
    osMsg->param2 = dataMsg->len;
    osMsg->param3 = VOS_GET_RECEIVER_ID(dataMsg);

    if (dataMsg->len == 0) {
        VOS_MemFree(WUEPS_PID_AT, msgBlock);
        AT_WARN_LOG("tmp cannot malloc space !");
        return;
    }
    tmp = VOS_MemAlloc(WUEPS_PID_AT, (DYNAMIC_MEM_PT), dataMsg->len);
    if (tmp == NULL) {
        VOS_MemFree(WUEPS_PID_AT, msgBlock);
        return;
    }
    (VOS_VOID)memset_s(tmp, dataMsg->len, 0x00, dataMsg->len);

    osMsg->param1 = tmp;

    memResult = memcpy_s((VOS_UINT8 *)(osMsg->param1), dataMsg->len, dataMsg->context, dataMsg->len);
    TAF_MEM_CHK_RTN_VAL(memResult, dataMsg->len, dataMsg->len);

    /* ��Ϣ���� */
    msgProcItem = atGetFtmCnfMsgProc(dataMsg->msgId);
#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    ftmIndMsgItem = At_GetFtmIndMsgProc(dataMsg->msgId);
#endif

    if (msgProcItem != NULL) {
        AT_StopTimerCmdReady(dataMsg->clientId);
        msgProcItem->cnfMsgProc((VOS_UINT8)(dataMsg->clientId), (struct MsgCB *)msgBlock);
    }
#if (FEATURE_UE_MODE_NR == FEATURE_OFF) /* only CLT, not support NR MODE */
    else if (ftmIndMsgItem != VOS_NULL_PTR) {
        ftmIndMsgItem->indMsgProc((struct MsgCB *)msgBlock);
    }
#endif
    else {

    }

    VOS_MemFree(WUEPS_PID_AT, tmp);
    VOS_MemFree(WUEPS_PID_AT, msgBlock);
}

/*lint -restore*/

