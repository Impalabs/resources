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

#include "ndserver_timer_proc.h"
#include "securec.h"
#include "ip_comm.h"
#include "ip_nd_server.h"
#include "ndserver_entity.h"
#include "ndserver_ns_proc.h"
#include "ndserver_debug.h"
#include "ndserver_ns_proc.h"
#include "ndserver_ra_proc.h"
#include "ndserver_om.h"


#define THIS_MODU ps_nd
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_NDSERVERTIMERPROC_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */


/*
 * Description: �ж϶�ʱ�����Ƿ�Ϸ�
 * Input: ulIndex     --- ND SERVER������������
 *                   enTimerType --- ��ʱ������
 * History: VOS_UINT32
 *    1.      2011-04-02  Draft Enact
 */
STATIC VOS_UINT32 IP_NDSERVER_IsTimerNameValid(VOS_UINT32 addrIndex, NDSER_TimerUint32 timerType)
{
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
        case IP_ND_SERVER_TIMER_FIRST_NS:
        case IP_ND_SERVER_TIMER_RA: {
            if (addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM) {
                return IP_TRUE;
            }

            break;
        }
        default:
            break;
    }

    return IP_FALSE;
}

/*
 * Description: ��ȡ��ʱ��
 * Input: enTimerType------------------��ʱ������
 *                   ulIndex----------------------��ʱ��������
 * History:
 *    1.      2011-04-02  Draft Enact
 */
IP_TimerInfo *IP_NDSERVER_GetTimer(VOS_UINT32 timerIndex, NDSER_TimerUint32 timerType)
{
    IP_TimerInfo *timerInfo = VOS_NULL_PTR;

    /* ���ݶ�ʱ����ͬ���ͣ���ȡ��ʱ�� */
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
        case IP_ND_SERVER_TIMER_FIRST_NS:
        case IP_ND_SERVER_TIMER_RA:
            timerInfo = IP_NDSERVER_ADDRINFO_GET_TIMER(timerIndex);
            break;
        default:
            break;
    }

    return timerInfo;
}

/*
 * Description: ��ȡ��ʱ��ʱ��
 * Input: enTimerType------------------��ʱ������
 * History:
 *    1.      2011-04-02  Draft Enact
 */
STATIC VOS_UINT32 IP_NDSERVER_GetTimerLen(NDSER_TimerUint32 timerType)
{
    VOS_UINT32 timerLen;

    /* ���ݶ�ʱ����ͬ���ͣ���ʱ��ʱ����ͬ */
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
            timerLen = g_nsTimerLen;
            break;
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
            timerLen = g_periodicNsTimerLen;
            break;
        case IP_ND_SERVER_TIMER_FIRST_NS:
            timerLen = g_firstNsTimerLen;
            break;
        case IP_ND_SERVER_TIMER_RA:
            timerLen = g_raTimerLen;
            break;
        default:
            timerLen = IP_NULL;
            break;
    }

    return timerLen;
}
/*lint +e550*/
/*
 * Discription: ����ĳһ���ص�ĳ�����͵Ķ�ʱ��
 * History:
 *      1.      2011-04-02  Draft Enact
 */
VOS_VOID IP_NDSERVER_TimerStart(VOS_UINT32 timerIndex, NDSER_TimerUint32 timerType)
{
    VOS_UINT32     timerLen;
    IP_TimerInfo  *timerInfo = VOS_NULL_PTR;

    /* ��ulIndex�Ϸ����ж� */
    if (IP_NDSERVER_IsTimerNameValid(timerIndex, timerType) == IP_FALSE) {
        /* ��ӡ�쳣��Ϣ */
        IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart: WARN: Input Para(ulIndex) err !", timerIndex,
                          timerType);
        return;
    }

    /* ������Ϣ��Ӧ�������źͶ�ʱ������,��ȡ������Ķ�ʱ�� */
    timerInfo = IP_NDSERVER_GetTimer(timerIndex, timerType);
    if (timerInfo == VOS_NULL_PTR) {
        /* ��ӡ�쳣��Ϣ */
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:ERROR: Get Timer failed.");
        return;
    }

    /* �ж϶�ʱ���Ƿ�򿪣��Ѵ���ر� */
    if (timerInfo->hTm != VOS_NULL_PTR) {
        /* �ر�ʧ�ܣ��򱨾����� */
        if (PS_STOP_REL_TIMER(&(timerInfo->hTm)) != VOS_OK) {
            /* ��ӡ�쳣��Ϣ */
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:WARN: stop reltimer error!");
            return;
        }

        /* ��ӡ�쳣��Ϣ */
        IPND_INFO_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:(TimerType) Timer not close!", timerInfo->timerName,
                       timerIndex);
    }

    /* ���ݶ�ʱ����ͬ���ͣ���ʱ����Ϣ��ͬ */
    timerLen = IP_NDSERVER_GetTimerLen(timerType);
    if (timerLen == IP_NULL) {
        /* ��ӡ�쳣��Ϣ */
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:ERROR: start unreasonable reltimer.");
        return;
    }

    /* �趨��ʱ��NAME,enTimerType�趨��ʱ��Para����ʧ���򱨾����� */
    if (PS_START_REL_TIMER(&(timerInfo->hTm), NDIS_NDSERVER_PID, timerLen, (VOS_UINT32)timerType,
                           timerIndex, VOS_RELTIMER_NOLOOP) != VOS_OK) {
        /* ��ӡ�쳣��Ϣ */
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:WARN: start reltimer error!");
        return;
    }

    /* ���¶�ʱ����� */
    timerInfo->timerName = timerType;

    /* ��ӡ��ʱ��������Ϣ */
    IP_NDSERVER_PrintTimeStartInfo(timerIndex, timerType);

    return;
}

/*
 * Discription: ֹͣĳһ����ĳ�����Ͷ�ʱ��
 * Return: None
 *      1.      2011-04-02  Draft Enact
 */
VOS_VOID IP_NDSERVER_TimerStop(VOS_UINT32 timerIndex, NDSER_TimerUint32 timerType)
{
    IP_TimerInfo *timerInfo = VOS_NULL_PTR;

    /* ��ulIndex�Ϸ����ж� */
    if (IP_NDSERVER_IsTimerNameValid(timerIndex, timerType) == IP_FALSE) {
        /* ��ӡ�쳣��Ϣ */
        IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStop: WARN: Input Para(ulIndex) err !", timerIndex,
                          timerType);
        return;
    }

    /* ������Ϣ��Ӧ�������źͶ�ʱ������,��ȡ������Ķ�ʱ�� */
    timerInfo = IP_NDSERVER_GetTimer(timerIndex, timerType);
    if (timerInfo == VOS_NULL_PTR) {
        /* ��ӡ�쳣��Ϣ */
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStop:ERROR:Get Timer failed.");
        return;
    }

    /* ��ʱ�����ڴ�״̬����رգ����򣬺��� */
    if (timerInfo->hTm != VOS_NULL_PTR) {
        if (timerType != timerInfo->timerName) {
            /* ��ӡ�쳣��Ϣ */
            IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStop: TimerType not match:", timerType,
                              timerInfo->timerName);
        }
        /* �ر�ʧ�ܣ��򱨾����� */
        if (PS_STOP_REL_TIMER(&(timerInfo->hTm)) != VOS_OK) {
            /* ��ӡ�쳣��Ϣ */
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStop:WARN: stop reltimer error!");
            return;
        }

        /* �����ʱ���� */
        timerInfo->loopTimes = 0;

        /* ��ӡ��ʱ���ر���Ϣ */
        IP_NDSERVER_PrintTimeStopInfo(timerIndex, timerType);
    }

    return;
}
/*
 * Function Name: IP_NDSERVER_ProcTimerMsgNsExp
 * Description: �����ھ�����󣬵ȴ��ھӹ��泬ʱ
 * Input: pMsg -------- ��Ϣָ��
 * History:
 *      1.      2011-04-09  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_ProcTimerMsgNsExp(VOS_UINT32 addrIndex)
{
    IP_NdserverAddrInfo    *infoAddr = IP_NULL_PTR;
    IP_NdserverTeAddrInfo  *teInfo   = IP_NULL_PTR;

    /* ��ӡ����ú��� */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgNsExp is entered.");

    /* ����Ϣ��ȡ��ND SERVER���� */
    IP_ASSERT(addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);
    teInfo   = IP_NDSERVER_ADDRINFO_GET_TEINFO(addrIndex);

    if ((infoAddr->validFlag != IP_TRUE) ||
        (infoAddr->teAddrInfo.teAddrState == IP_NDSERVER_TE_ADDR_INEXISTENT)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgNsExp: ND Server state err!");
        return;
    }

    /* �ж���ʱ�����Ƿ�С�ڹ涨��ʱ���� */
    if (infoAddr->timerInfo.loopTimes < g_nsTimerMaxExpNum) {
        /* ����NS��Ϣ��PC */
        if (PS_SUCC != IP_NDSERVER_SendNsMsg(addrIndex, teInfo->teGlobalAddr)) {
            IP_NDSERVER_AddTransPktFailNum(addrIndex);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgNsExp:Send NS Msg failed.");
        }

        /* ����ʱ����ʱ������1 */
        infoAddr->timerInfo.loopTimes++;

        /* �����ھ�����ʱ�� */
        IP_NDSERVER_TimerStart(addrIndex, IP_ND_SERVER_TIMER_NS);
    } else {
        /* �����ʱ����Ϣ */
        infoAddr->timerInfo.loopTimes = 0;

        /* ���δ���״̬��TE��ַ����Ӧ��ַ�������������TE��ַ���ط�RA */
        if (infoAddr->teAddrInfo.teAddrState == IP_NDSERVER_TE_ADDR_INCOMPLETE) {
            infoAddr->teAddrInfo.teAddrState = IP_NDSERVER_TE_ADDR_INEXISTENT;

            /* ����RA��Ϣ��PC */
            if (IP_NDSERVER_SendRaMsg(addrIndex, VOS_NULL_PTR) != PS_SUCC) {
                IP_NDSERVER_AddTransPktFailNum(addrIndex);
                IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgNsExp:Send RA Msg failed.");
            }

            /* ����·�ɹ��涨ʱ�� */
            IP_NDSERVER_TimerStart(addrIndex, IP_ND_SERVER_TIMER_RA);
            IP_NDSERVER_ClearDlPktQue(addrIndex); /* ������л�������е�PKT */
        } else {
            /* �����������ھ�����ʱ�� */
            IP_NDSERVER_TimerStart(addrIndex, IP_ND_SERVER_TIMER_PERIODIC_NS);
        }
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_ProcTimerMsgPeriodicNsExp
 * Description: �������ھ�����ʱ
 * Input: pMsg -------- ��Ϣָ��
 * History:
 *      1.      2011-04-09  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_ProcTimerMsgPeriodicNsExp(VOS_UINT32 addrIndex)
{
    IP_NdserverAddrInfo    *infoAddr = IP_NULL_PTR;
    IP_NdserverTeAddrInfo  *teInfo   = IP_NULL_PTR;

    /* ��ӡ����ú��� */
    IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgPeriodicNsExp is entered.");

    /* ����Ϣ��ȡ��ND SERVER���� */
    IP_ASSERT(addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);
    teInfo   = IP_NDSERVER_ADDRINFO_GET_TEINFO(addrIndex);

    if ((infoAddr->validFlag != IP_TRUE) ||
        (infoAddr->teAddrInfo.teAddrState == IP_NDSERVER_TE_ADDR_INEXISTENT)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgPeriodicNsExp: ND Server state err!");
        return;
    }

    /* ���������Է���RA��ʱ����ֹRouter���� */
    if (g_periodicRaTimeCnt[addrIndex]-- == 0) {
        g_periodicRaTimeCnt[addrIndex] = g_periodicRaTimerLen / g_periodicNsTimerLen;
        ;

        /* ����RA��Ϣ��PC */
        if (IP_NDSERVER_SendRaMsg(addrIndex, VOS_NULL_PTR) != PS_SUCC) {
            IP_NDSERVER_AddTransPktFailNum(addrIndex);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgPeriodicNsExp:Send RA Msg failed.");
        }
    }

    /* ����NS��Ϣ��PC */
    if (IP_NDSERVER_SendNsMsg(addrIndex, teInfo->teGlobalAddr) != PS_SUCC) {
        IP_NDSERVER_AddTransPktFailNum(addrIndex);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgPeriodicNsExp:Send NS Msg failed.");
    }

    /* �����ʱ����Ϣ */
    infoAddr->timerInfo.loopTimes = 0;
    /* �����ھ�����ʱ�� */
    IP_NDSERVER_TimerStart(addrIndex, IP_ND_SERVER_TIMER_NS);

    return;
}

/*
 * Function Name: IP_NDSERVER_ProcTimerMsgPeriodicNsExp
 * Description: �յ��ظ���ַ����ȴ���ʱ����ʱ
 * Input: pMsg -------- ��Ϣָ��
 * History:
 *      1.      2011-04-09  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_ProcTimerMsgFirstNsExp(VOS_UINT32 addrIndex)
{
    IP_NdserverAddrInfo    *infoAddr = IP_NULL_PTR;
    IP_NdserverTeAddrInfo  *teInfo   = IP_NULL_PTR;

    /* ��ӡ����ú��� */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgFirstNsExp is entered.");

    /* ����Ϣ��ȡ��ND SERVER���� */
    IP_ASSERT(addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);
    teInfo   = IP_NDSERVER_ADDRINFO_GET_TEINFO(addrIndex);

    if ((infoAddr->validFlag != IP_TRUE) ||
        (infoAddr->teAddrInfo.teAddrState == IP_NDSERVER_TE_ADDR_INEXISTENT)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgFirstNsExp: ND Server state err!");
        return;
    }

    /* ����NS��Ϣ��PC */
    if (IP_NDSERVER_SendNsMsg(addrIndex, teInfo->teGlobalAddr) != PS_SUCC) {
        IP_NDSERVER_AddTransPktFailNum(addrIndex);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgFirstNsExp:Send NS Msg failed.");
    }

    /* �����ʱ����Ϣ */
    infoAddr->timerInfo.loopTimes = 0;
    /* �����ھ�����ʱ�� */
    IP_NDSERVER_TimerStart(addrIndex, IP_ND_SERVER_TIMER_NS);

    return;
}

/*
 * Function Name: IP_NDSERVER_ProcTimerMsgRaExp
 * Description: �յ��ظ���ַ���ǰ���ڷ���·�ɹ��泬ʱ
 * Input: pMsg -------- ��Ϣָ��
 * History:
 *      1.      2011-04-09  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_ProcTimerMsgRaExp(VOS_UINT32 addrIndex)
{
    IP_NdserverAddrInfo        *infoAddr                    = IP_NULL_PTR;
#if (VOS_OS_VER != VOS_WIN32)
    VOS_UINT8                  *data                        = VOS_NULL_PTR;
    VOS_INT32                   lockKey;
#endif
    VOS_INT32                   rlt;
    IMM_Zc                     *immZc                       = VOS_NULL_PTR;
    VOS_UINT8                   dstIPAddr[IP_IPV6_ADDR_LEN];

    /* ��ӡ����ú��� */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp is entered.");

    /* ����Ϣ��ȡ��ND SERVER���� */
    IP_ASSERT(addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);
    if (infoAddr->validFlag != IP_TRUE) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp: ND Server state err!");
        return;
    }

    /* ����RA��Ϣ��PC */
    if (IP_NDSERVER_SendRaMsg(addrIndex, VOS_NULL_PTR) != PS_SUCC) {
        IP_NDSERVER_AddTransPktFailNum(addrIndex);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp:Send RA Msg failed.");
    }

    /* �����ʱ����Ϣ */
    infoAddr->timerInfo.loopTimes = 0;
    /* ����·�ɹ��涨ʱ�� */
    IP_NDSERVER_TimerStart(addrIndex, IP_ND_SERVER_TIMER_RA);

    /* NDIS����C�ˣ��޻������ */
#if (VOS_OS_VER != VOS_WIN32)
    /* *********** ����IP����Ŀ�ĵ�ַ��ΪNS����Ŀ���ַ�����е�ַ���� ********* */
    lockKey = VOS_SplIMP();
    if (LUP_PeekQueHead(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(addrIndex), (VOS_VOID **)(&immZc)) != PS_SUCC) {
        VOS_Splx(lockKey);
        return;
    }
    VOS_Splx(lockKey);

    data = IMM_ZcGetDataPtr(immZc);
    if (data == VOS_NULL_PTR) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp, IMM_ZcGetDataPtr return NULL");
        return;
    }

    /* �õ�Ŀ��IPV6��ַ������NS���� */
    rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, data + IP_IPV6_DST_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    if (IP_NDSERVER_SendNsMsg(addrIndex, dstIPAddr) != PS_SUCC) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp, IP_NDSERVER_SendNsMsg return NULL");
        return;
    }
#endif
    return;
}

/*
 * Function Name: IP_NDSERVER_TimerMsgDistr
 * Description: ND SERVER TIMER��Ϣ�ַ�����
 * History:
 *      1.     2011-04-02  Draft Enact
 */
NDSER_TimerUint32 IP_NDSERVER_TimerMsgDistr(const MsgBlock *rcvMsg)
{
    VOS_UINT32              timerIndex;
    NDSER_TimerUint32       timerType;
    IP_TimerInfo           *timerInfo = VOS_NULL_PTR;

    /* ����Ϣ��ȡ��ulIndex��enTimerType */
    timerIndex     = IP_GetTimerPara(rcvMsg);
    timerType = (NDSER_TimerUint32)IP_GetTimerName(rcvMsg);
    /* �жϺϷ��� */
    if (IP_NDSERVER_IsTimerNameValid(timerIndex, timerType) == IP_FALSE) {
        /* ��ӡ�쳣��Ϣ */
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerMsgDistr: Invalid Timer Name or Para !");
        return IP_MSG_DISCARD;
    }

    /* ������Ϣ��Ӧ�������źͶ�ʱ������,��ȡ������Ķ�ʱ�� */
    timerInfo = IP_NDSERVER_GetTimer(timerIndex, timerType);
    if (timerInfo == VOS_NULL_PTR) {
        /* ��ӡ�쳣��Ϣ */
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerMsgDistr: Get Timer failed.");
        return IP_MSG_DISCARD;
    }

    /* �ж���Ϣ�����붨ʱ�������Ƿ�һ�� */
    if (timerType != timerInfo->timerName) {
        /* ��ӡ�쳣��Ϣ */
        IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerMsgDistr: TimerType not match:", timerType,
                          timerInfo->timerName);
        return IP_MSG_DISCARD;
    }

    /* ��ʱ����ʱ���� */
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
            IP_NDSERVER_ProcTimerMsgNsExp(timerIndex);
            break;
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
            IP_NDSERVER_ProcTimerMsgPeriodicNsExp(timerIndex);
            break;
        case IP_ND_SERVER_TIMER_FIRST_NS:
            IP_NDSERVER_ProcTimerMsgFirstNsExp(timerIndex);
            break;
        case IP_ND_SERVER_TIMER_RA:
            IP_NDSERVER_ProcTimerMsgRaExp(timerIndex);
            break;
        default:
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerMsgDistr: Illegal Timer Type!");
            break;
    }

    return IP_MSG_HANDLED;
}



