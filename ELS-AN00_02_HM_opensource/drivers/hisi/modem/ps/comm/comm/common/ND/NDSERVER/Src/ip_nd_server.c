/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Create: 2012/10/20
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
#include "ip_nd_server.h"
#include "vos.h"
#include "mdrv.h"
#include "ps_tag.h"
#include "securec.h"
#include "ads_dev_i.h"
#include "ads_device_interface.h"
#include "ppp_interface.h"
#include "ndis_interface.h"
#include "ndis_om.h"
#include "ndis_arp_proc.h"
#include "ndis_entity.h"
#include "ndis_dl_proc.h"
#include "ndserver_entity.h"
#include "ndserver_timer_proc.h"
#include "ndserver_ra_proc.h"
#include "ndserver_pkt_comm.h"
#include "ndserver_pkt_comm.h"
#include "ndserver_at_proc.h"
#include "ndserver_debug.h"
#include "ndserver_om.h"


#define THIS_MODU ps_nd
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_IPNDSERVER_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */
/* ND SERVER������Ϣ */
IP_NdserverAddrInfo     g_ndServerAddrInfo[IP_NDSERVER_ADDRINFO_MAX_NUM];
IP_NdserverTeDetectBuf g_ndServerTeDetectBuf;


/* ���ڴ洢�������ND�������� */
IP_ND_Msg g_ndMsgData;

/* ND SERVERʹ�õķ��������� */
VOS_UINT8 g_ndserverSendMsgBuffer[IP_IPM_MTU];


/*
 * ��������: ��ʱ����Te IP��ַ����TE�����ظ���ַ���������������ȡTE MAC��
 *             ַ
 * �޸���ʷ:
 *  1.��    ��: 2011��5��17��
 *    �޸�����: �����ɺ���
 */
VOS_VOID IP_NDSERVER_SaveTeDetectIp(const VOS_UINT8 *teGlobalAddr)
{
    errno_t    rlt;
    VOS_UINT32 bufIndex = g_ndServerTeDetectBuf.head;

    if ((IP_IPV6_IS_LINKLOCAL_ADDR(teGlobalAddr)) || (IP_IPV6_64BITPREFIX_EQUAL_ZERO(teGlobalAddr)) ||
        (IP_IPV6_IS_MULTICAST_ADDR(teGlobalAddr))) {
        /* ����Global IP */
        return;
    }

    /* ��ͬPrefix�ĵ�ַ�Ƿ��Ѿ����� */
    while (bufIndex != g_ndServerTeDetectBuf.tail) {
        /*lint -e960*/
        if ((g_ndServerTeDetectBuf.teIpBuf[bufIndex].valid == IP_TRUE) &&
            (IP_MEM_CMP(teGlobalAddr, g_ndServerTeDetectBuf.teIpBuf[bufIndex].teGlobalAddr,
                        ND_IP_IPV6_PREFIX_LENGTH) == 0)) {
            /*lint +e960*/
            /* ��ͬPrefixֻ����һ����ַ */
            rlt = memcpy_s(g_ndServerTeDetectBuf.teIpBuf[bufIndex].teGlobalAddr, IP_IPV6_ADDR_LEN,
                           teGlobalAddr, IP_IPV6_ADDR_LEN);
            IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
            return;
        }
        bufIndex = TTF_MOD_ADD(bufIndex, 1, g_ndServerTeDetectBuf.maxNum);
    }

    if (g_ndServerTeDetectBuf.head ==
        TTF_MOD_ADD(g_ndServerTeDetectBuf.tail, 1, g_ndServerTeDetectBuf.maxNum)) {
        /* BUF��ʱ�������ϵĵ�ַ */
        g_ndServerTeDetectBuf.head = TTF_MOD_ADD(g_ndServerTeDetectBuf.head, 1,
                                                 g_ndServerTeDetectBuf.maxNum);
    }

    /* ����IP��ַ */
    rlt = memcpy_s(g_ndServerTeDetectBuf.teIpBuf[g_ndServerTeDetectBuf.tail].teGlobalAddr,
                   IP_IPV6_ADDR_LEN, teGlobalAddr, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    g_ndServerTeDetectBuf.teIpBuf[g_ndServerTeDetectBuf.tail].valid = IP_TRUE;

    g_ndServerTeDetectBuf.tail = TTF_MOD_ADD(g_ndServerTeDetectBuf.tail, 1,
                                             g_ndServerTeDetectBuf.maxNum);
}

/*
 * ��������: ��ȡ��ʱ�����TE IP��ַ
 * �޸���ʷ:
 *  1.��    ��: 2011��5��17��
 *    �޸�����: �����ɺ���
 */
STATIC VOS_UINT8 *IP_NDSERVER_GetTeDetectIp(const VOS_UINT8 *prefixAddr)
{
    VOS_UINT32 bufIndex = g_ndServerTeDetectBuf.head;
    while (bufIndex != g_ndServerTeDetectBuf.tail) {
        /*lint -e960*/
        if ((g_ndServerTeDetectBuf.teIpBuf[bufIndex].valid == IP_TRUE) &&
            (IP_MEM_CMP(prefixAddr, g_ndServerTeDetectBuf.teIpBuf[bufIndex].teGlobalAddr,
                        ND_IP_IPV6_PREFIX_LENGTH) == 0)) {
            /*lint +e960*/
            g_ndServerTeDetectBuf.teIpBuf[bufIndex].valid = IP_FALSE;

            /* Bufͷ����϶���� */
            while ((g_ndServerTeDetectBuf.head != g_ndServerTeDetectBuf.tail) &&
                   (g_ndServerTeDetectBuf.teIpBuf[g_ndServerTeDetectBuf.head].valid == IP_FALSE)) {
                g_ndServerTeDetectBuf.head = TTF_MOD_ADD(g_ndServerTeDetectBuf.head, 1,
                                                         g_ndServerTeDetectBuf.maxNum);
            }

            return g_ndServerTeDetectBuf.teIpBuf[bufIndex].teGlobalAddr;
        }
        bufIndex = TTF_MOD_ADD(bufIndex, 1, g_ndServerTeDetectBuf.maxNum);
    }

    return IP_NULL_PTR;
}

/*
 * Description: ���ND SERVER�����Ϣ
 * Input: ulIndex --- ND SERVER�ṹ������
 * History:
 *    1.     2011-04-01  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_Stop(VOS_UINT32 addrIndex)
{
    IP_NdserverAddrInfo *infoAddr = IP_NULL_PTR;
    IP_ASSERT(addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);

    infoAddr->validFlag = IP_FALSE;

    infoAddr->epsbId = 0;

    (VOS_VOID)memset_s(&infoAddr->ipv6NwPara, sizeof(ESM_IP_Ipv6NWPara), IP_NULL,
                       sizeof(ESM_IP_Ipv6NWPara));

    if (infoAddr->teAddrInfo.teAddrState != IP_NDSERVER_TE_ADDR_INEXISTENT) {
        IP_NDSERVER_SaveTeDetectIp(infoAddr->teAddrInfo.teGlobalAddr);
    }

    /* ���TE��ַ��¼ */
    (VOS_VOID)memset_s(&infoAddr->teAddrInfo, sizeof(IP_NdserverTeAddrInfo), IP_NULL,
                       sizeof(IP_NdserverTeAddrInfo));

    IP_NDSERVER_TimerStop(addrIndex, infoAddr->timerInfo.timerName);

    (VOS_VOID)memset_s(&infoAddr->ipSndNwMsg, sizeof(IP_SndMsg), IP_NULL, sizeof(IP_SndMsg));

    g_periodicRaTimeCnt[addrIndex] = g_periodicRaTimerLen / g_periodicNsTimerLen;

    return;
}

/*
 * Description: �ͷ����л�������е�PKT
 * History: VOS_VOID
 *    1.       2011-12-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_ClearDlPktQue(VOS_UINT32 addrIndex)
{
    IMM_Zc      *immZc = VOS_NULL_PTR;
    VOS_INT32    lockKey;

    while (LUP_IsQueEmpty(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(addrIndex)) != PS_TRUE) {
        lockKey = VOS_SplIMP();
        if (LUP_DeQue(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(addrIndex), (VOS_VOID **)(&immZc)) != PS_SUCC) {
            VOS_Splx(lockKey);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ClearDlPktQue, LUP_DeQue return fail!");
            return;
        }
        VOS_Splx(lockKey);
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
    }

    return;
}

/*
 * Description: ����NDIS������Ipv6PdnRel
 * History: VOS_VOID
 *    1.       2011-12-09  Draft Enact
 *    2.       2013-1-16  DSDA
 */
VOS_VOID NdSer_Ipv6PdnRel(VOS_UINT8 exRabId)
{
    VOS_UINT32 addrIndex;

    IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnRel is entered.");

    addrIndex = IP_NDSERVER_GET_ADDR_INFO_INDEX(exRabId);
    if (addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM) {
        IP_NDSERVER_Stop(addrIndex);
        IP_NDSERVER_ClearDlPktQue(addrIndex); /* �ͷ�����PKT������� */
    } else {
        IPND_ERROR_LOG2(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnRel: Invalid Input:", addrIndex, exRabId);
    }

    return;
}

/*
 * Description: ��NDIS���ã���ѯ��Ӧ���ص�ND SERVERʵ���Ƿ���Ч
 * History: VOS_UINT32
 *    1.       2011-12-06  Draft Enact
 */
VOS_UINT32 NdSer_Ipv6PdnValid(VOS_UINT8 rabId)
{
    VOS_UINT32 addrIndex;

    addrIndex = IP_NDSERVER_GET_ADDR_INFO_INDEX(rabId);
    if (addrIndex >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
        return PS_FAIL;
    }

    return PS_SUCC;
}

/*
 * Description: ��NDIS���ã���ȡND SERVER��Ӧʵ�������MAC֡ͷ
 * History: VOS_UINT8
 *    1.       2011-12-06  Draft Enact
 */
VOS_UINT8 *NdSer_GetMacFrm(VOS_UINT8 addrIndex, VOS_UINT8 *teAddrState)
{
    IP_NdserverAddrInfo *infoAddr = IP_NULL_PTR;

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);
    if (infoAddr->validFlag != IP_TRUE) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "pstInfoAddr ucValidFlag is not TRUE!", addrIndex);
        return VOS_NULL_PTR;
    }

    *teAddrState = infoAddr->teAddrInfo.teAddrState;

    return IP_NDSERVER_ADDRINFO_GET_MACFRAME(addrIndex);
}

/*
 * Description: ��NDIS���ã���δ���PC MAC��ַʱ����������IP��
 * History: VOS_UINT8
 *    1.      2011-12-06  Draft Enact
 */
VOS_VOID NdSer_MacAddrInvalidProc(IMM_Zc *immZc, VOS_UINT8 addrIndex)
{
    IMM_Zc      *queHead = VOS_NULL_PTR;
    VOS_INT32    lockKey;

    IP_NDSERVER_AddMacInvalidPktNum(addrIndex);

    /* **********************����IPV6���ݰ�����******************************** */
    if (LUP_IsQueFull(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(addrIndex)) == PS_TRUE) {
        lockKey = VOS_SplIMP();
        if (LUP_DeQue(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(addrIndex), (VOS_VOID **)(&queHead)) != PS_SUCC) {
            VOS_Splx(lockKey);
            /*lint -e522*/
            IMM_ZcFree(immZc);
            /*lint +e522*/
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_MacAddrInvalidProc, LUP_DeQue return NULL");
            return;
        }
        VOS_Splx(lockKey);
        /*lint -e522*/
        IMM_ZcFree(queHead); /* �ͷ������IP�� */
        /*lint +e522*/
    }

    lockKey = VOS_SplIMP();
    if (LUP_EnQue(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(addrIndex), immZc) != PS_SUCC) { /* �������µ�IP�� */
        VOS_Splx(lockKey);
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_MacAddrInvalidProc, LUP_EnQue return NULL");
        return;
    }
    VOS_Splx(lockKey);

    IP_NDSERVER_AddEnquePktNum(addrIndex);

    return;
}


/*
 * Description: ���ҵ�һ�����õ�IPv6ǰ׺
 * Input: pstConfigParaInd ---- ESM_IP_NW_PARA_IND_STRU��Ϣָ��
 * Output: pulPrefixIndex   ---- ǰ׺����ָ��
 * History:
 *      1.     2011-04-05  Draft Enact
 */
STATIC VOS_UINT32 IP_NDSERVER_FindIpv6EffectivePrefix(const AT_NDIS_Ipv6PdnInfo *configParaInd,
                                                      VOS_UINT32 *prefixIndex)
{
    VOS_UINT32              loopIndex;
    ND_IP_Ipv6Prefix       *ipv6Prefix = IP_NULL_PTR;

    /* ����ǰ׺�б�����A��ʶΪ1��ǰ׺����Ϊ64��ǰ׺,prefixLen��λ��bit */
    for (loopIndex = IP_NULL; loopIndex < configParaInd->prefixNum; loopIndex++) {
        ipv6Prefix = (ND_IP_Ipv6Prefix *)&configParaInd->prefixList[loopIndex];
        if ((ipv6Prefix->prefixAValue == IP_IPV6_OP_TRUE) &&
            (ipv6Prefix->prefixLen == (ND_IP_IPV6_ADDR_LENGTH - ND_IP_IPV6_IFID_LENGTH) * 8) && /* 8:bit */
            (!IP_IPV6_64BITPREFIX_EQUAL_ZERO(ipv6Prefix->prefix)) &&
            (!IP_IPV6_IS_LINKLOCAL_ADDR(ipv6Prefix->prefix)) &&
            (!IP_IPV6_IS_MULTICAST_ADDR(ipv6Prefix->prefix))) {
            *prefixIndex = loopIndex;
            return IP_SUCC;
        }
    }

    return IP_FAIL;
}

/*
 * Function Name: IP_NDSERVER_GetNwPara
 * Description: ��ID_ESM_IP_NW_PARA_IND�л�ȡ�������
 * Input: pstNwParaInd--------------��Ϣָ��
 * Output: pstNwParamTmp-------------�������ָ��
 * History: VOS_UINT32
 *    1.      2011-04-05  Draft Enact
 */
STATIC VOS_UINT32 IP_NDSERVER_GetNwPara(ESM_IP_Ipv6NWPara *nwParamTmp,
                                        const AT_NDIS_Ipv6PdnInfo *nwParaInd)
{
    VOS_UINT32 prefixIndex;
    VOS_UINT32 rslt;
    errno_t    rlt;
    (VOS_VOID)memset_s(nwParamTmp, sizeof(ESM_IP_Ipv6NWPara), IP_NULL, sizeof(ESM_IP_Ipv6NWPara));

    /* ��ȡHOP LIMIT */
    nwParamTmp->curHopLimit = nwParaInd->curHopLimit;

    /* ��ȡMTU */
    if (nwParaInd->OpMtu == IP_IPV6_OP_TRUE) {
        nwParamTmp->opMtu = nwParaInd->OpMtu;
        nwParamTmp->mtuValue      = nwParaInd->Mtu;
    }

    /* ��ȡ�ӿڱ�ʶ�� */
    rlt = memcpy_s(nwParamTmp->interfaceId, ND_IP_IPV6_IFID_LENGTH, nwParaInd->interfaceId,
                   ND_IP_IPV6_IFID_LENGTH);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* �����ַǰ׺�б�Ŀǰֻ�����ʹ�õ�һ�����õ�IPv6ǰ׺ */
    rslt = IP_NDSERVER_FindIpv6EffectivePrefix(nwParaInd, &prefixIndex);
    if (rslt != IP_SUCC) {
        return IP_FAIL;
    }

    rlt = memcpy_s(&nwParamTmp->prefixList[IP_NULL], sizeof(ND_IP_Ipv6Prefix),
                   &nwParaInd->prefixList[prefixIndex], sizeof(ND_IP_Ipv6Prefix));
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    nwParamTmp->prefixNum = 1;

    /* ��ȡDNS�������б� */
    rlt = memcpy_s(nwParamTmp->dnsSer.priDnsServer, IP_IPV6_ADDR_LEN, nwParaInd->dnsSer.priServer,
                   IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    rlt = memcpy_s(nwParamTmp->dnsSer.secDnsServer, IP_IPV6_ADDR_LEN, nwParaInd->dnsSer.secServer,
                   IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    nwParamTmp->dnsSer.dnsSerNum = nwParaInd->dnsSer.serNum;

    /* ��ȡSIP�������б� */
    rlt = memcpy_s(nwParamTmp->sipSer.priSipServer, IP_IPV6_ADDR_LEN, nwParaInd->pcscfSer.priServer,
                   IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    rlt = memcpy_s(nwParamTmp->sipSer.secSipServer, IP_IPV6_ADDR_LEN, nwParaInd->pcscfSer.secServer,
                   IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    nwParamTmp->sipSer.sipSerNum = nwParaInd->pcscfSer.serNum;

    return IP_SUCC;
}
STATIC VOS_UINT32 NdSer_RoutePrefixConfirm(ESM_IP_Ipv6NWPara *nwParamTmp, IP_NdserverAddrInfo *infoAddr,
                                           VOS_UINT32 timerIndex)
{
    VOS_UINT8 *ipV6;
    VOS_INT32  rlt;
    ipV6 = IP_NDSERVER_GetTeDetectIp(nwParamTmp->prefixList[0].prefix);
    /* MT�ػ��ٿ�����ND SERVER�յ�����RAǰ���������յ�TE���ظ���ַ���NS */
    if (ipV6 != IP_NULL_PTR) {
        IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: Using saved TeAddr to get MAC.");
        rlt = memcpy_s(infoAddr->teAddrInfo.teGlobalAddr, IP_IPV6_ADDR_LEN, ipV6,
                       IP_IPV6_ADDR_LEN);
        if (rlt != EOK) {
            return IP_FAIL;
        }
        infoAddr->teAddrInfo.teAddrState = IP_NDSERVER_TE_ADDR_INCOMPLETE;
        /* �����յ��ظ���ַ����ĵȴ���ʱ�� */
        IP_NDSERVER_TimerStart(timerIndex, IP_ND_SERVER_TIMER_FIRST_NS);
    } else {
        IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: None saved TeAddr.");

        /* ����·�ɹ��涨ʱ�� */
        IP_NDSERVER_TimerStart(timerIndex, IP_ND_SERVER_TIMER_RA);
    }
    return IP_SUCC;
}
/*
 * Description: ����NDIS������Ipv6PdnInfoCfg
 */
VOS_VOID NdSer_Ipv6PdnInfoCfg(VOS_UINT8 exRabId, const AT_NDIS_Ipv6PdnInfo *ipv6PdnInfo)
{
    VOS_UINT32                  addrIndex;
    IP_NdserverAddrInfo        *infoAddr  = IP_NULL_PTR;
    ESM_IP_Ipv6NWPara           nwParamTmp;
    VOS_INT32                   rlt;
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcEsmIpNwParaInd is entered.");

    /* ͨ����չRabId����NDSERVERʵ�壬������Ҳ����������һ�����е� */
    addrIndex = IP_NDSERVER_GET_ADDR_INFO_INDEX(exRabId);
    if (addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM) {
        infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);
    } else {
        /* ����һ��AddrInfo�ṹ�壬���һ��Index */
        infoAddr = NdSer_AllocAddrInfo(&addrIndex);
    }

    if (infoAddr == IP_NULL_PTR) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: Get ND Server Entity failed.", exRabId);
        return;
    }

    /* ��ȡ������� */
    if (IP_NDSERVER_GetNwPara(&nwParamTmp, ipv6PdnInfo) != IP_SUCC) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: Get Nw Param fail.");
        return;
    }

    /* �ж��Ƿ�δ�����·��ǰ׺ */
    if (infoAddr->validFlag != IP_TRUE) {
        if (NdSer_RoutePrefixConfirm(&nwParamTmp, infoAddr, addrIndex) == IP_FAIL) {
            return;
        }
    } else if (IP_MEM_CMP(infoAddr->ipv6NwPara.prefixList[0].prefix,
                          nwParamTmp.prefixList[0].prefix,
                          (ND_IP_IPV6_ADDR_LENGTH - ND_IP_IPV6_IFID_LENGTH)) != 0) {
        IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: Valid ND Server get new Nw Param.");

        /* ���TE��ַ��¼ */
        (VOS_VOID)memset_s(&infoAddr->teAddrInfo, sizeof(IP_NdserverTeAddrInfo), IP_NULL,
                           sizeof(IP_NdserverTeAddrInfo));

        /* ����·�ɹ��涨ʱ�� */
        IP_NDSERVER_TimerStart(addrIndex, IP_ND_SERVER_TIMER_RA);
    }

    /* ��stNwParamTmp�е����ݿ�����pstInfoAddr->stIpv6NwPara�� */
    rlt = memcpy_s(&infoAddr->ipv6NwPara, sizeof(ESM_IP_Ipv6NWPara), &nwParamTmp,
                   sizeof(ESM_IP_Ipv6NWPara));
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* ���ó��غ� */
    infoAddr->epsbId = exRabId;

    /* ��λND SERVER�ṹ����Ч��־ */
    infoAddr->validFlag = IP_TRUE;

    /* ����RA��Ϣ��PC */
    if (IP_NDSERVER_SendRaMsg(addrIndex, VOS_NULL_PTR) != PS_SUCC) {
        IP_NDSERVER_AddTransPktFailNum(addrIndex);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg:Send RA Msg failed.");
    }

    return;
}


/*
 * Function Name: IP_NDSERVER_BuildTooBigICMPPkt
 * Description: �γ���PC�ظ��ĳ�������Ӧ
 * Input: pucSrcData ----------- Դ����ָ��
 *                   pstDestData ---------- Ŀ��ת���ṹָ��
 *                   ulIcmpv6HeadOffset --- ICMPv6��ͷƫ����
 * History:
 *    1.      2011-12-09  Draft Enact
 */
STATIC VOS_UINT32 IP_NDSERVER_BuildTooBigICMPPkt(VOS_UINT32 addrIndex, const VOS_UINT8 *srcData, VOS_UINT8 *destData,
                                                 VOS_UINT32 dataLen)
{
    const VOS_UINT8                     *ipMsg                       = srcData;
    VOS_UINT8                     *data                        = VOS_NULL_PTR;
    ND_Ipv6AddrInfo                srcIPAddr;
    ND_MacAddrInfo                 srcMacAddr;
    ND_Ipv6AddrInfo                dstIPAddr;
    ND_MacAddrInfo                 dstMacAddr;
    IP_NdserverTeAddrInfo         *teInfo                      = IP_NULL_PTR;
    VOS_UINT32                     datatemp; /* linux 4�ֽڶ��� */
    VOS_INT32                      rlt;

    if (dataLen <= (IP_IPV6_HEAD_LEN + IP_ICMPV6_HEAD_LEN)) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_BuildTooBigICMPPkt: ulDataLen is too short.", dataLen);
        return IP_FAIL;
    }
    /* ��������MAC��ַ����link-local��ַ */
    IP_NDSERVER_InitAddrInfo(&srcIPAddr, &srcMacAddr, &dstIPAddr, &dstMacAddr);
    rlt = memcpy_s(srcMacAddr.macAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    IP_ProduceIfaceIdFromMacAddr(srcIPAddr.ipv6Addr, srcMacAddr.macAddr);
    IP_SetUint16Data(srcIPAddr.ipv6Addr, IP_IPV6_LINK_LOCAL_PREFIX);

    /* �õ�Ŀ��IPV6��ַ */
    rlt = memcpy_s(dstIPAddr.ipv6Addr, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_SRC_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* �õ�Ŀ��MAC��ַ */
    teInfo = IP_NDSERVER_ADDRINFO_GET_TEINFO(addrIndex);
    rlt     = memcpy_s(dstMacAddr.macAddr, IP_MAC_ADDR_LEN, teInfo->teLinkLayerAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* ָ��ICMP�ײ� */
    data = destData + IP_ETHERNET_HEAD_LEN + IP_IPV6_HEAD_LEN;

    /* ICMP type�� */
    *data = IP_ICMPV6_PACKET_TOO_BIG;
    data++;

    /* ICMP code�� */
    *data = IP_IPV6_ND_VALID_CODE;
    data++;

    /* ICMP У����� */
    *(VOS_UINT16 *)(VOS_VOID *)data = 0;
    data += 2; /* 2:len */

    /* MTU �� */
    /*lint -e960*/
    datatemp = VOS_HTONL(dataLen); /* linux 4�ֽڶ��� */
    rlt      = memcpy_s(data, sizeof(VOS_UINT32), (VOS_UINT8 *)(&datatemp), sizeof(VOS_UINT32)); /* linux 4�ֽڶ��� */
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    /*lint +e960*/
    data += 4; /* 4:len */

    /* ��дICMP PayLoad���� */
    rlt = memcpy_s(data, (IP_IPM_MTU - IP_ETHERNET_HEAD_LEN - IP_IPV6_HEAD_LEN - IP_ICMPV6_HEAD_LEN), ipMsg,
                   ((dataLen - IP_IPV6_HEAD_LEN) - IP_ICMPV6_HEAD_LEN));
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* ָ��IPV6�ײ� */
    destData += IP_ETHERNET_HEAD_LEN;
    /* ��дIPV6ͷ�� */
    IP_ND_FormIPv6HeaderMsg(srcIPAddr, dstIPAddr, (dataLen - IP_IPV6_HEAD_LEN), destData, IP_HEAD_PROTOCOL_ICMPV6);

    /* ����ICMPv6��ͷУ��� */
    if (IP_BuildIcmpv6Checksum(destData, IP_IPV6_HEAD_LEN) != IP_SUCC) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormRaMsg: Build ICMPv6 Checksum failed.");
        return IP_FAIL;
    }

    /* ������̫��ͷ */
    destData -= IP_ETHERNET_HEAD_LEN;
    IP_NDSERVER_FormEtherHeaderMsg(srcMacAddr, dstMacAddr, destData);

    return IP_SUCC;
}

/*
 * ��������: ����PC���͵ĳ�����
 * �޸���ʷ:
 *  1.��    ��: 2011��12��9��
 *    �޸�����: �����ɺ���
 */
VOS_VOID IP_NDSERVER_ProcTooBigPkt(VOS_UINT8 rabId, const VOS_UINT8 *srcData, VOS_UINT32 dataLen)
{
    VOS_UINT32                  addrIndex;
    const VOS_UINT8                  *ipMsg    = srcData;
    VOS_UINT8                  *sendBuff = VOS_NULL_PTR;
    IP_NdserverAddrInfo *infoAddr = IP_NULL_PTR;

    addrIndex = IP_NDSERVER_GET_ADDR_INFO_INDEX(rabId);
    if (addrIndex >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTooBigPkt: ND Server info flag is invalid!");
        return;
    }

    IP_NDSERVER_AddRcvBooBigPktNum(addrIndex);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);

    sendBuff = IP_NDSERVER_GET_SENDMSG_BUFFER();
    (VOS_VOID)memset_s(sendBuff, IP_IPM_MTU, IP_NULL, IP_IPM_MTU);
    if (IP_NDSERVER_BuildTooBigICMPPkt(addrIndex, ipMsg, sendBuff, dataLen) != IP_SUCC) {
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTooBigPkt: IP_NDSERVER_BuildTooBigICMPPkt FAIL!");
        return;
    }

    /* ����������Ӧ���͵�PC */
    if (NdSer_SendPkt(sendBuff, dataLen + IP_ETHERNET_HEAD_LEN, infoAddr->epsbId) == PS_FAIL) {
        IP_NDSERVER_AddTransPktFailNum(addrIndex);
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTooBigPkt:Send TooBigPkt failed.");
        return;
    }

    IP_NDSERVER_AddTransTooBigPktNum(addrIndex);
    IP_NDSERVER_AddTransPktTotalNum(addrIndex);

    return;
}

/*
 * ��������: PP NDIS����PID��Ϣ������
 * �޸���ʷ:
 *  1.��    ��: 2012��12��9��
 *    �޸�����: �����ɺ���
 */
VOS_VOID APP_NdServer_PidMsgProc(MsgBlock *rcvMsg)
{
    if (rcvMsg == VOS_NULL_PTR) {
        PS_PRINTF_INFO("Error:APP_NdServer_PidMsgProc Parameter pRcvMsg is NULL!");
        return;
    }
    switch (rcvMsg->ulSenderPid) {
        case DOPRA_PID_TIMER:
            (VOS_VOID)IP_NDSERVER_TimerMsgDistr(rcvMsg);
            break;

        case APP_AT_PID:
            NDSERVER_AtMsgProc(rcvMsg);
            break;
        default:
            break;
    }
    return;
}

