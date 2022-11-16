/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Create: 2012/10/30
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 and
 *  only version 2 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3) Neither the name of Huawei nor the names of its contributors may
 *     be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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
/*
 * 1 Include HeadFile
 */

#include "ndis_arp_proc.h"
#include "vos.h"
#include "securec.h"
#include "ndis_om.h"
#include "ndis_drv.h"
#include "ndis_interface.h"
#include "ndis_entity.h"
#include "ndis_dl_proc.h"


#define THIS_MODU ps_ndis
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_NDISARPPROC_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */

/* arp�����м䲿�̶ֹ���ֵ */
const VOS_UINT8 g_arpReqFixVal[ETH_ARP_FIXED_MSG_LEN] = { 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01 };
/* arp��Ӧ�м䲿�̶ֹ���ֵ */
const VOS_UINT8 g_arpRspFixVal[ETH_ARP_FIXED_MSG_LEN] = { 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02 };
/* �㲥��ַ��ȫ1 */
const VOS_UINT8 g_broadCastAddr[ETH_MAC_ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

STATIC VOS_UINT32 Ndis_FillArpReplyInfo(ETH_ArpFrame *reqArp, NDIS_Ipv4Info *arpV4Info)
{
    errno_t              rlt;
    VOS_UINT32           tgtIpAddr   = reqArp->targetIP.ipAddr32bit;
    rlt = memcpy_s(reqArp->dstAddr, ETH_MAC_ADDR_LEN, reqArp->srcAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL((rlt != EOK), PS_FAIL);

    rlt = memcpy_s(reqArp->targetAddr, ETH_MAC_ADDR_LEN, reqArp->srcAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL((rlt != EOK), PS_FAIL);

    reqArp->targetIP.ipAddr32bit = reqArp->senderIP.ipAddr32bit;

    rlt = memcpy_s(reqArp->srcAddr, ETH_MAC_ADDR_LEN, (arpV4Info->macFrmHdr + ETH_MAC_ADDR_LEN),
                   ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL((rlt != EOK), PS_FAIL);

    rlt = memcpy_s(reqArp->senderAddr, ETH_MAC_ADDR_LEN, (arpV4Info->macFrmHdr + ETH_MAC_ADDR_LEN),
                   ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL((rlt != EOK), PS_FAIL);

    reqArp->senderIP.ipAddr32bit = tgtIpAddr;

    /* opcode */
    reqArp->opCode = ETH_ARP_RSP_TYPE;
    return PS_SUCC;
}

/*
 * ��������: ���������������ARP֡
 * �޸���ʷ:
 *  1.��    ��: 2009��12��31��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_SendMacFrm(const VOS_UINT8 *buf, VOS_UINT32 len, VOS_UINT8 exRabId)
{
    IMM_Zc           *immZc = VOS_NULL_PTR;
    VOS_INT32         lRtn;
    VOS_INT32         handle;
    VOS_UINT8        *pdata       = VOS_NULL_PTR;
    NDIS_Entity      *ndisEntity = VOS_NULL_PTR;
    errno_t           rlt;
    if (buf == VOS_NULL_PTR) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, pucBuf is NULL!");
        return PS_FAIL;
    }

    immZc = IMM_ZcStaticAlloc(len);
    if (immZc == VOS_NULL_PTR) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, IMM_ZcStaticAlloc fail!");
        return PS_FAIL;
    }

    pdata = (VOS_UINT8 *)IMM_ZcPut(immZc, len);
    if (pdata == VOS_NULL_PTR) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, IMM_ZcPut fail!");
        return PS_FAIL;
    }

    rlt = memcpy_s(pdata, len, buf, len);
    if (rlt != EOK) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        return PS_FAIL;
    }

    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (ndisEntity == VOS_NULL_PTR) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, NDIS_GetEntityByRabId fail!");
        return PS_FAIL;
    }

    handle = ndisEntity->handle;

    /* ���ݷ��� */
    lRtn = NDIS_UDI_WRITE(handle, immZc, len);
    if (lRtn != 0) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, udi_write fail!");
        NDIS_STAT_DL_SEND_ARPDHCPPKT_FAIL(1);
        return PS_FAIL;
    }
    return PS_SUCC;
}

/*
 * ��������: ����ARP Request ֡
 */
STATIC VOS_UINT32 Ndis_ProcReqArp(ETH_ArpFrame *reqArp, VOS_UINT8 rabId)
{
    NDIS_Entity         *ndisEntity = VOS_NULL_PTR;
    NDIS_Ipv4Info       *arpV4Info  = VOS_NULL_PTR;
    VOS_UINT32           addrIndex;
    ndisEntity = NDIS_GetEntityByRabId(rabId);
    if (ndisEntity == VOS_NULL_PTR) {
        return PS_FAIL;
    }

    arpV4Info = &ndisEntity->ipV4Info;

    if ((reqArp->senderIP.ipAddr32bit != 0) /* ����MAC OS ���ARP����,��Sender IPΪ0 */
        && (arpV4Info->ueIpInfo.ipAddr32bit != reqArp->senderIP.ipAddr32bit)) {
        /* ԴUE IP���������ò������������������ */
        NDIS_STAT_PROC_NOTUE_ARP(1);
        return PS_SUCC;
    }

    /* ����PC MAC��ַ */
    if (Ndis_UpdateMacAddr(reqArp->senderAddr, ETH_MAC_ADDR_LEN, arpV4Info) == PS_FAIL) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_ProcReplyArp,  Ndis_UpdateMacAddr Error!");
        return PS_FAIL;
    }

    addrIndex = NDIS_GetEntityIndex(rabId);
    /* ��������IP��������е����ݰ� */
    NDIS_SendDlPkt(addrIndex);


    /* ���ARP���ظ���Ӧ */
    if ((reqArp->targetIP.ipAddr32bit == reqArp->senderIP.ipAddr32bit) || (reqArp->senderIP.ipAddr32bit == 0)) {
        NDIS_STAT_PROC_FREE_ARP(1);
        return PS_SUCC;
    }
    /* ������Ӧ */
    if (Ndis_FillArpReplyInfo(reqArp, arpV4Info) == PS_FAIL) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_ProcReqArp, Fill Arp Reply fail!");
        return PS_FAIL;
    }
    /* ������Ӧ */
    if (Ndis_SendMacFrm((VOS_UINT8 *)reqArp, sizeof(ETH_ArpFrame), rabId) == PS_FAIL) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_ProcReqArp, Send Mac Frm fail!");
        return PS_FAIL;
    }
    NDIS_STAT_DL_SEND_ARP_REPLY(1);

    return PS_SUCC;
}
/*
 * ��������: ����ARP Reply ֡,����PC��MAC��ַ
 * �޸���ʷ:
 *  1.��    ��: 2009��12��31��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��22��
 *    �޸�����: DSDA
 */
STATIC VOS_UINT32 Ndis_ProcReplyArp(ETH_ArpFrame *rspArp, VOS_UINT8 rabId)
{
    NDIS_Ipv4Info *arpV4Info  = VOS_NULL_PTR;
    VOS_UINT32     targetIP   = rspArp->targetIP.ipAddr32bit;
    NDIS_Entity   *ndisEntity;
    VOS_UINT32     addrIndex;

    ndisEntity = NDIS_GetEntityByRabId(rabId);
    if (ndisEntity == VOS_NULL_PTR) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_ProcReplyArp,  NDIS_GetEntityByRabId Error!", rabId);
        return PS_FAIL;
    }

    arpV4Info = &ndisEntity->ipV4Info;

    if (targetIP == ndisEntity->ipV4Info.gwIpInfo.ipAddr32bit) {
        /* ����PC MAC��ַ */
        if (Ndis_UpdateMacAddr(rspArp->senderAddr, ETH_MAC_ADDR_LEN, arpV4Info) == PS_FAIL) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_ProcReplyArp,  Ndis_UpdateMacAddr Error!");
            return PS_FAIL;
        }
        addrIndex = NDIS_GetEntityIndex(rabId);
        /* ��������IP��������е����ݰ� */
        NDIS_SendDlPkt(addrIndex);

        arpV4Info->arpRepFlg = PS_TRUE;
        return PS_SUCC;
    }

    return PS_FAIL;
}

/*
 * ��������: ���������������ARP֡
 * �޸���ʷ:
 *  1.��    ��: 2009��12��31��
 *    �޸�����: �����ɺ���
 */
VOS_VOID Ndis_ProcArpMsg(ETH_ArpFrame *arpMsg, VOS_UINT8 rabId)
{
    /* Arp Request */
    if (VOS_MemCmp((VOS_UINT8 *)arpMsg + ETH_MAC_HEADER_LEN, g_arpReqFixVal, ETH_ARP_FIXED_MSG_LEN) == 0) {
        NDIS_STAT_UL_RECV_ARP_REQUEST(1);
        if (Ndis_ProcReqArp(arpMsg, rabId) == PS_FAIL) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_ProcArpMsg,  procreqarp fail!");
        }
    } else if (VOS_MemCmp((VOS_UINT8 *)arpMsg + ETH_MAC_HEADER_LEN, g_arpRspFixVal, ETH_ARP_FIXED_MSG_LEN) == 0) {
        NDIS_STAT_DL_RECV_ARP_REPLY(1);
        if (Ndis_ProcReplyArp(arpMsg, rabId) == PS_FAIL) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_ProcArpMsg,  ProcReplyArp fail!");
        }
    } else {
        NDIS_STAT_PROC_ARP_FAIL(1);
    }

    return;
}

/* NDIS ARP PROC Begin */
/*
 * ��������: ����ARP Request ֡��Ethenet��
 * �޸���ʷ:
 *  1.��    ��: 2009��12��31��
 *    �޸�����: �����ɺ���
 */
STATIC VOS_UINT32 Ndis_SendRequestArp(NDIS_Ipv4Info *arpInfoItem, VOS_UINT8 exRabId)
{
    ETH_ArpFrame arpReq;
    errno_t      rlt;
    /* ֮ǰһ�η��͵�Req��δ�ܵ�Reply���� */
    if (arpInfoItem->arpRepFlg == PS_FALSE) {
        NDIS_STAT_ARPREPLY_NOTRECV(1);
        /* ��һ�θ澯��־ */
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)&arpReq, sizeof(ETH_ArpFrame), 0, sizeof(ETH_ArpFrame));

    /* ��ARP Request */
    rlt = memcpy_s(arpReq.dstAddr, ETH_MAC_ADDR_LEN, g_broadCastAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL((rlt != EOK), PS_FAIL);

    rlt = memcpy_s(arpReq.srcAddr, ETH_MAC_ADDR_LEN, arpInfoItem->macFrmHdr + ETH_MAC_ADDR_LEN,
                   ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL((rlt != EOK), PS_FAIL);
    arpReq.frameType = ARP_PAYLOAD;

    /* ����Ĺ̶����� */
    rlt = memcpy_s(((VOS_UINT8 *)&arpReq + ETH_MAC_HEADER_LEN), ETH_ARP_FIXED_MSG_LEN, g_arpReqFixVal,
                   ETH_ARP_FIXED_MSG_LEN);
    NDIS_CHK_SEC_RETURN_VAL((rlt != EOK), PS_FAIL);

    /* Payload���ֵ�MAC��ַ���� */
    (VOS_VOID)memset_s(arpReq.targetAddr, ETH_MAC_ADDR_LEN, 0, ETH_MAC_ADDR_LEN);
    rlt = memcpy_s(arpReq.senderAddr, ETH_MAC_ADDR_LEN, arpInfoItem->macFrmHdr + ETH_MAC_ADDR_LEN,
                   ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL((rlt != EOK), PS_FAIL);

    /* ����IP */
    arpReq.senderIP.ipAddr32bit = arpInfoItem->gwIpInfo.ipAddr32bit;
    arpReq.targetIP.ipAddr32bit = arpInfoItem->ueIpInfo.ipAddr32bit;

    if (Ndis_SendMacFrm((VOS_UINT8 *)&arpReq, sizeof(ETH_ArpFrame), exRabId) != PS_SUCC) {
        arpInfoItem->arpRepFlg = PS_TRUE;
        NDIS_STAT_DL_SEND_ARP_REQUEST_FAIL(1);
        return PS_FAIL;
    }

    NDIS_STAT_DL_SEND_ARP_REQUEST_SUCC(1);

    arpInfoItem->arpRepFlg = PS_FALSE;

    return PS_SUCC;
}
/*
 * ��������: NDISֹͣ������ARP��ʱ��
 * �޸���ʷ:
 *  1.��    ��: 2012��4��19��
 *    �޸�����: �����ɺ���
 */
VOS_VOID Ndis_StopARPTimer(NDIS_ArpPeriodTimer *arpPeriodTimer)
{
    if (arpPeriodTimer->tm != VOS_NULL_PTR) {
        if (VOS_StopRelTimer(&(arpPeriodTimer->tm)) != VOS_OK) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_StopARPTimer, VOS_StopRelTimer fail!");
        }
        arpPeriodTimer->tm = VOS_NULL_PTR;
    }

    return;
}

/*
 * ��������: NDIS����������ARP��ʱ��
 * �޸���ʷ:
 *  1.��    ��: 2012��4��19��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_StartARPTimer(NDIS_Entity *ndisEntity)
{
#if (VOS_OS_VER != VOS_WIN32)
    VOS_UINT32                  rtn;
    NDIS_ArpPeriodTimer        *arpPeriodTimer = VOS_NULL_PTR;

    /* ���ָ���ж� */
    if (ndisEntity == VOS_NULL_PTR) {
        return PS_FAIL;
    }

    /* ����Ƿ���Ҫ����ARP��ʱ�� */
    if (((ndisEntity->rabType & NDIS_ENTITY_IPV4) != NDIS_ENTITY_IPV4) ||
        (ndisEntity->ipV4Info.arpInitFlg == PS_TRUE)) {
        return PS_SUCC;
    }

    arpPeriodTimer = &(ndisEntity->ipV4Info.arpPeriodTimer);

    /* ����������У���ͣ�� */
    if (arpPeriodTimer->tm != VOS_NULL_PTR) {
        Ndis_StopARPTimer(arpPeriodTimer);
    }

    rtn = VOS_StartRelTimer(&(arpPeriodTimer->tm), PS_PID_APP_NDIS, arpPeriodTimer->timerValue,
                            arpPeriodTimer->name, 0, VOS_RELTIMER_LOOP, VOS_TIMER_PRECISION_0);
    if (rtn != VOS_OK) {
        arpPeriodTimer->tm = VOS_NULL_PTR;
        return PS_FAIL;
    }
#endif

    return PS_SUCC;
}

/*
 * ��������: ����TmerMsg
 * 1.��    ��: 2012��4��28��
 *   �޸�����: �����ɺ���
 */
VOS_VOID Ndis_ProcARPTimerExp(VOS_VOID)
{
    VOS_UINT32                  loop;
    NDIS_Entity           *ndisEntity     = VOS_NULL_PTR;
    NDIS_Ipv4Info        *ipV4Info       = VOS_NULL_PTR;
    NDIS_ArpPeriodTimer *arpPeriodTimer = VOS_NULL_PTR;

    for (loop = 0; loop < NAS_NDIS_MAX_ITEM; loop++) {
        ndisEntity = &g_ndisEntity[loop];
        if ((ndisEntity->rabType & NDIS_ENTITY_IPV4) == NDIS_ENTITY_IPV4) {
            ipV4Info       = &(ndisEntity->ipV4Info);
            arpPeriodTimer = &(ipV4Info->arpPeriodTimer);

            if (ipV4Info->arpInitFlg == PS_TRUE) {
#if (VOS_OS_VER != VOS_WIN32)
                Ndis_StopARPTimer(arpPeriodTimer);
#endif
            } else {
                (VOS_VOID)Ndis_SendRequestArp(&(ndisEntity->ipV4Info), ndisEntity->rabId);
            }
        }
    }

    return;
}





