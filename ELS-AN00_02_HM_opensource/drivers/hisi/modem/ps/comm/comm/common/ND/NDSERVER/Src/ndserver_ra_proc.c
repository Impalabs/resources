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

#include "ndserver_ra_proc.h"
#include "securec.h"
#include "ip_ipm_global.h"
#include "ip_comm.h"
#include "ndis_entity.h"
#include "ip_nd_server.h"
#include "ndserver_om.h"
#include "ndserver_entity.h"
#include "ndserver_pkt_comm.h"


#define THIS_MODU ps_nd
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_NDSERVERRAPROC_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */

/*
 * Description: RA����ͷ�̶���������
 * Input: ulIndex --- ND SERVERʵ������
 * Output: pucData --- ���Ļ���ָ��
 * History:
 *    1.      2011-04-07  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_FormRaHeaderMsg(VOS_UINT32 addrIndex, VOS_UINT8 *data)
{
    ESM_IP_Ipv6NWPara *nwPara = IP_NULL_PTR;

    IP_ASSERT(addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM);
    nwPara = IP_NDSERVER_ADDRINFO_GET_NWPARA(addrIndex);

    /* ���� */
    *data = IP_ICMPV6_TYPE_RA;
    data++;

    /* ���� */
    *data = IP_IPV6_ND_VALID_CODE;
    data++;

    /* 2:����У��� */
    data += 2;

    /* ��ǰ������ */
    *data = nwPara->curHopLimit;
    data++;

    /* 7 6:�����ַ���ñ�־��������״̬���ñ�־ */
    *data = (VOS_UINT8)(((g_ndServerMFlag & 0x1) << 7) | ((g_ndServerOFlag & 0x1) << 6));
    data++;

    /* ·�������� */
    IP_SetUint16Data(data, g_routerLifetime);
    data += 2; /* ·������������Ҫռ2���ֽ� */

    /* �ɴ�ʱ�� */
    IP_SetUint32Data(data, g_reachableTime);
    data += 4; /* �ɴ�ʱ����Ҫռ4���ֽ� */

    /* �ط���ʱ�� */
    IP_SetUint32Data(data, g_retransTimer);

    return;
}


/*
 * Function Name: IP_NDSERVER_FormRaOptMsg
 * Description: RA����ѡ�������
 * Input: ulIndex --- ND SERVERʵ������
 *                   pucMacAddr- Դ��·���ַ
 * Output: pucData --- ���Ļ���ָ��
 *                   pulLen ---- ���ĳ���ָ��
 * History:
 *    1.      2011-04-07  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_FormRaOptMsg(VOS_UINT32 addrIndex, const ND_MacAddrInfo macAddr, VOS_UINT8 *data,
                                         VOS_UINT32 *len)
{
    ESM_IP_Ipv6NWPara        *nwPara = IP_NULL_PTR;
    VOS_UINT32                count;
    VOS_UINT32                tmpMtu;
    VOS_INT32                 rlt;
    IP_ASSERT(addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM);
    nwPara = IP_NDSERVER_ADDRINFO_GET_NWPARA(addrIndex);

    /* ���� */
    *data = IP_ICMPV6_OPT_SRC_LINK_LAYER_ADDR;
    data++;

    /* ���� */
    *data = 1;
    data++;

    /* ��·���ַ */
    rlt = memcpy_s(data, IP_MAC_ADDR_LEN, macAddr.macAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    data += IP_MAC_ADDR_LEN;

    (*len) += IP_ND_OPT_UNIT_LEN;

    if (nwPara->opMtu == IP_IPV6_OP_TRUE) {
        /* ���� */
        *data = IP_ICMPV6_OPT_MTU;
        data++;

        /* ���� */
        *data = 1;
        data++;

        /* 2:���� */
        data += 2;

        /* MTU: ȡNV�е�MTU��RA�е�MTU����Сֵ��Ϊ����PC��MTU */
        tmpMtu = PS_MIN(g_nvMtu, nwPara->mtuValue);
        IP_SetUint32Data(data, tmpMtu);
        data += 4; /* MTU����ռ4���ֽ� */

        (*len) += IP_ND_OPT_UNIT_LEN;
    }

    for (count = 0; count < nwPara->prefixNum; count++) {
        /* ���� */
        *data = IP_ICMPV6_OPT_PREFIX_INFO;
        data++;

        /* 4:���� */
        *data = 4;
        data++;

        /* ǰ׺���� */
        *data = (VOS_UINT8)(nwPara->prefixList[count].prefixLen);
        data++;

        /* ��·�ϱ�־�����α�־ */
        *data = (VOS_UINT8)(((nwPara->prefixList[count].prefixLValue & 0x1) << 7) | /* 7:bit */
                            ((nwPara->prefixList[count].prefixAValue & 0x1) << 6)); /* 6:bit */
        data++;

        /* ��Ч������ */
        IP_SetUint32Data(data, nwPara->prefixList[count].validLifeTime);
        data += 4; /* ��Ч������ռ4���ֽ� */

        /* ѡ�������� */
        IP_SetUint32Data(data, nwPara->prefixList[count].preferredLifeTime);
        data += 4; /* ѡ��������ռ4���ֽ� */

        /* 4:�����ֶ� */
        data += 4;

        /* ǰ׺ */
        rlt = memcpy_s(data, IP_IPV6_ADDR_LEN, nwPara->prefixList[count].prefix, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
        data += IP_IPV6_ADDR_LEN;

        (*len) += (4 * IP_ND_OPT_UNIT_LEN); /* 4:len */
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_FormRaMsg
 * Description: ����·�ɹ�����Ϣ
 * Input: ulIndex ------- ����RA����ʵ������
 *                   pstNdMsgData -- Ŀ����Ϣ����
 * Output: pucSendBuff --- ����RA���Ļ���
 *                   pulSendLen ---- ���ͱ��ĳ���
 * History:
 *    1.      2011-04-07  Draft Enact
 */
STATIC VOS_VOID IP_NDSERVER_FormRaMsg(VOS_UINT32 addrIndex, const IP_ND_Msg *ndMsgData, VOS_UINT8 *sendBuff,
                                      VOS_UINT32 *sendLen)
{
    VOS_UINT32                  packetLen                    = IP_NULL;
    VOS_UINT8                  *data                        = sendBuff;
    ND_Ipv6AddrInfo             srcIPAddr;
    ND_MacAddrInfo              srcMacAddr;
    ND_Ipv6AddrInfo             dstIPAddr;
    ND_MacAddrInfo              dstMacAddr;
    IP_NdserverAddrInfo *infoAddr                    = IP_NULL_PTR;
    VOS_INT32                   rlt;
    /* ��ӡ����ú��� */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormRaMsg is entered.");
    IP_NDSERVER_InitAddrInfo(&srcIPAddr, &srcMacAddr, &dstIPAddr, &dstMacAddr);
    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(addrIndex);

    data += IP_ETHERNET_HEAD_LEN + IP_IPV6_HEAD_LEN;

    /* ����RA��ͷ */
    IP_NDSERVER_FormRaHeaderMsg(addrIndex, data);

    packetLen += IP_ICMPV6_RA_HEADER_LEN;

    /* ��������MAC��ַ����link-local��ַ */
    rlt = memcpy_s(srcMacAddr.macAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    IP_ProduceIfaceIdFromMacAddr(srcIPAddr.ipv6Addr, srcMacAddr.macAddr);
    IP_SetUint16Data(srcIPAddr.ipv6Addr, IP_IPV6_LINK_LOCAL_PREFIX);
    rlt = memcpy_s(infoAddr->ueLinkLocalAddr, IP_IPV6_ADDR_LEN, srcIPAddr.ipv6Addr, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* ����RA����ѡ�� */
    IP_NDSERVER_FormRaOptMsg(addrIndex, srcMacAddr, (VOS_VOID *)(data + packetLen), &packetLen);

    /* ȷ���������鲥��ʽ */
    if ((ndMsgData != VOS_NULL_PTR) && (IP_IPV6_IS_LINKLOCAL_ADDR(ndMsgData->srcIp)) &&
        (ndMsgData->ndMsgStru.rsInfo.opSrcLinkLayerAddr == 1)) {
        rlt = memcpy_s(dstIPAddr.ipv6Addr, IP_IPV6_ADDR_LEN, ndMsgData->srcIp, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        rlt = memcpy_s(dstMacAddr.macAddr, IP_MAC_ADDR_LEN, ndMsgData->ndMsgStru.rsInfo.srcLinkLayerAddr,
                       IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    } else {
        rlt = memcpy_s(dstIPAddr.ipv6Addr, IP_IPV6_ADDR_LEN, g_ndAllNodesMulticaseAddr, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        rlt = memcpy_s(dstMacAddr.macAddr, IP_MAC_ADDR_LEN, g_ndAllNodesMacAddr, IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    }

    data -= IP_IPV6_HEAD_LEN;

    /* ����IPv6��ͷ */
    IP_ND_FormIPv6HeaderMsg(srcIPAddr, dstIPAddr, packetLen, data, IP_HEAD_PROTOCOL_ICMPV6);

    /* ����ICMPv6��ͷУ��� */
    if (IP_BuildIcmpv6Checksum(data, IP_IPV6_HEAD_LEN) != IP_SUCC) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormRaMsg: Build ICMPv6 Checksum failed.");
    }

    data -= IP_ETHERNET_HEAD_LEN;

    /* ������̫��ͷ */
    IP_NDSERVER_FormEtherHeaderMsg(srcMacAddr, dstMacAddr, data);

    /* ���ر��ĳ��� */
    *sendLen = packetLen + IP_IPV6_HEAD_LEN + IP_ETHERNET_HEAD_LEN;

    return;
}

/*
 * Function Name: IP_NDSERVER_SendRaMsg
 * Description: ����·�ɹ�����Ϣ
 * Input: ulIndex ------- ����RA����ʵ������
 *                   pstNdMsgData -- Ŀ����Ϣ����
 * History: VOS_UINT32
 *    1.      2011-04-07  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_SendRaMsg(VOS_UINT32 addrIndex, const IP_ND_Msg *ndMsgData)
{
    VOS_UINT8 *sendBuff = VOS_NULL_PTR;
    VOS_UINT32 sendLen   = IP_NULL;
    VOS_UINT32 epsbId;

    IP_ASSERT_RTN(addrIndex < IP_NDSERVER_ADDRINFO_MAX_NUM, PS_FAIL);
    epsbId = IP_NDSERVER_GET_EPSBID(addrIndex);

    sendBuff = IP_NDSERVER_GET_SENDMSG_BUFFER();
    (VOS_VOID)memset_s(sendBuff, IP_IPM_MTU, IP_NULL, IP_IPM_MTU);

    /* �����γ�RA��Ϣ���� */
    IP_NDSERVER_FormRaMsg(addrIndex, ndMsgData, sendBuff, &sendLen);

    IP_NDSERVER_AddTransRaPktNum(addrIndex);
    IP_NDSERVER_AddTransPktTotalNum(addrIndex);

    /* ��RA��Ϣ���͵�PC */
    return NdSer_SendPkt(sendBuff, sendLen, (VOS_UINT8)epsbId);
}



