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

#include "ndserver_entity.h"
#include "securec.h"
#include "ps_tag.h"
#include "ps_type_def.h"
#include "ip_ipm_global.h"
#include "ndis_entity.h"
#include "ndserver_debug.h"
#include "ip_nd_server.h"


#define THIS_MODU ps_nd
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_NDSERVERENTITY_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */

/* ND SERVER���ر�������ò��� */
VOS_UINT8  g_ndServerMFlag;          /* M��ʶ */
VOS_UINT8  g_ndServerOFlag;          /* O��ʶ */
VOS_UINT16 g_routerLifetime; /* Router Lifetime */
VOS_UINT32 g_reachableTime;  /* Reachable Time */
VOS_UINT32 g_retransTimer;   /* Retrans Timer */

VOS_UINT32 g_nsTimerLen;         /* ��������NS��ʱ��ʱ�� */
VOS_UINT32 g_nsTimerMaxExpNum;   /* ��������NS���ʱ���� */
VOS_UINT32 g_periodicNsTimerLen; /* ������NS��ʱ��ʱ�� */
VOS_UINT32 g_periodicRaTimerLen; /* ������RA��ʱ��ʱ������ֹRouter���� */
VOS_UINT32 g_firstNsTimerLen;    /* �յ��ظ���ַ����ȴ��Ķ�ʱ��ʱ�� */
VOS_UINT32 g_raTimerLen;         /* �յ��ظ���ַ���ǰ���ڷ���RA��ʱ��ʱ�� */


/* �����Է���RAʱ������� */
VOS_UINT32 g_periodicRaTimeCnt[IP_NDSERVER_ADDRINFO_MAX_NUM];

/*
 * Description: ���ñ����������
 * History:
 *    1.      2011-04-01  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_SetLocalParam(VOS_VOID)
{
    /* M��ʶ��0����������ͨ��DHCPv6��ȡIPv6��ַ */
    g_ndServerMFlag = 0;

    /* O��ʶ��1����������ͨ����״̬DHCPv6��ȡ��������������DNS��������SIP�������� */
    g_ndServerOFlag = 1;

    /* Router Lifetime(��) */
    g_routerLifetime = 9000; /* ·��������9000s */

    /* 3600000:Reachable Time(����) */
    g_reachableTime = 3600000;

    /* Retrans Timer(����)��0����ʾδ֪ */
    g_retransTimer = 0;

    /* 4000:��������NS��ʱ��ʱ��(����) */
    g_nsTimerLen = 4000;

    /* 3:��������NS���ʱ���� */
    g_nsTimerMaxExpNum = 3;

    /* 60000:������NS��ʱ��ʱ��(����) */
    g_periodicNsTimerLen = 60000;

    /* 3600000:������RA����ֹRouter����(����) */
    g_periodicRaTimerLen = 3600000;

    /* 2000:�յ��ظ���ַ����ȴ��Ķ�ʱ��ʱ��(����) */
    g_firstNsTimerLen = 2000;

    /* 15000:�յ��ظ���ַ���ǰ���ڷ���RA�Ķ�ʱ��ʱ��(����) */
    g_raTimerLen = 15000;

    return;
}

/*
 * Description: ND SERVERģ��ĳ�ʼ��
 * History: VOS_VOID
 *    1.        2011-04-01  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_Init(VOS_VOID)
{
    VOS_UINT32 addrIndex;
    VOS_UINT16 payLoad = IP_IPV6_PAYLOAD;
    errno_t    rlt;

    IP_NDSERVER_SetLocalParam();
    IP_NDSERVER_DebugInit();

    for (addrIndex = 0; addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM; addrIndex++) {
        (VOS_VOID)memset_s(IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex), sizeof(IP_NdserverAddrInfo), IP_NULL,
                           sizeof(IP_NdserverAddrInfo));

        rlt = memcpy_s(IP_NDSERVER_ADDRINFO_GET_MACADDR(addrIndex), IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(),
                       IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        rlt = memcpy_s((IP_NDSERVER_ADDRINFO_GET_MACFRAME(addrIndex) + IP_MAC_ADDR_LEN),
                       (IP_ETH_MAC_HEADER_LEN - IP_MAC_ADDR_LEN), (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        /* payload����Ϊ2���ֽ� */
        rlt = memcpy_s((IP_NDSERVER_ADDRINFO_GET_MACFRAME(addrIndex) + 2 * IP_MAC_ADDR_LEN),
                       (IP_ETH_MAC_HEADER_LEN - 2 * IP_MAC_ADDR_LEN), (VOS_UINT8 *)(&payLoad), 2); /* 2:byte */
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        g_periodicRaTimeCnt[addrIndex] = g_periodicRaTimerLen / g_periodicNsTimerLen;
        /* ��������IP��������� */
        rlt = LUP_CreateQue(NDIS_NDSERVER_PID, &(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(addrIndex)),
                            ND_IPV6_WAIT_ADDR_RSLT_Q_LEN);
        if (rlt != PS_SUCC) {
            PS_PRINTF_ERR("IP_NDSERVER_Init, LUP_CreateQue DlPktQue fail.\n");
            return;
        }
    }

    (VOS_VOID)memset_s(&g_ndServerTeDetectBuf, sizeof(IP_NdserverTeDetectBuf), IP_NULL,
                       sizeof(IP_NdserverTeDetectBuf));

    g_ndServerTeDetectBuf.head   = 0;
    g_ndServerTeDetectBuf.tail   = 0;
    g_ndServerTeDetectBuf.maxNum = 2 * IP_NDSERVER_ADDRINFO_MAX_NUM; /* 2:buf */

    return;
}
/*
 * ��������: ND SERVER PID��ʼ������
 * �޸���ʷ:
 *  1.��    ��: 2011��12��7��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 APP_NdServer_Pid_InitFunc(enum VOS_InitPhaseDefine phase)
{
    switch (phase) {
        case VOS_IP_LOAD_CONFIG:
            IP_NDSERVER_Init();
            break;
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        case VOS_IP_BUTT:
            break;
        default:
            break;
    }
    return 0;
}


/*
 * ��������: ����ExRabId����NDSERVERʵ�������
 * �޸���ʷ:
 *  1.��    ��: 2013��1��15��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 NdSer_GetAddrInfoIdx(VOS_UINT8 exRabId)
{
    VOS_UINT32 i = 0;

    /* ��ѯ�Ƿ��Ѵ�����ӦEntity */
    do {
        if ((PS_TRUE == g_ndServerAddrInfo[i].validFlag) && (exRabId == g_ndServerAddrInfo[i].epsbId)) {
            /* �ҵ���Ӧʵ�� */
            return i;
        }
    } while ((++i) < IP_NDSERVER_ADDRINFO_MAX_NUM);

    return IP_NDSERVER_ADDRINFO_MAX_NUM;
}

/*
 * ��������: ����һ�����е�NDSERVERʵ��
 * �޸���ʷ:
 *  1.��    ��: 2013��1��15��
 *    �޸�����: �����ɺ���
 */
IP_NdserverAddrInfo *NdSer_AllocAddrInfo(VOS_UINT32 *pUlIndex)
{
    VOS_UINT32 i = 0;

    if (pUlIndex == IP_NULL_PTR) {
        return IP_NULL_PTR;
    }

    *pUlIndex = IP_NDSERVER_ADDRINFO_MAX_NUM;

    do {
        if (PS_FALSE == g_ndServerAddrInfo[i].validFlag) {
            /* �ҵ�����ʵ�� */
            *pUlIndex = i;
            return &g_ndServerAddrInfo[i];
        }
    } while ((++i) < IP_NDSERVER_ADDRINFO_MAX_NUM);

    return IP_NULL_PTR;
}



