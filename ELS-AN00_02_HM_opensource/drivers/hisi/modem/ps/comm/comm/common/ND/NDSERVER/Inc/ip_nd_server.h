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

#ifndef __IPNDSERVER_H__
#define __IPNDSERVER_H__

/*
 * 1 Include Headfile
 */
#include "vos.h"
#include "PsTypeDef.h"
#include "LUPQueue.h"
#include "at_ndis_interface.h"
#include "ip_comm.h"
#include "ip_ipm_global.h"
#include "ps_iface_global_def.h"
#include "imm_interface.h"
#include "ads_ndis_interface.h"

/*
 * 1.1 Cplusplus Announce
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*
 * #pragma pack(*)    �����ֽڶ��뷽ʽ
 */
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*
 * 2 Macro
 */
#ifdef _lint
#define NDIS_NDSERVER_PID 420
#else
#define NDIS_NDSERVER_PID UEPS_PID_NDSERVER
#endif

/* ����IP��������г��� */
#define ND_IPV6_WAIT_ADDR_RSLT_Q_LEN 10

/* ND SERVER����ַ��Ϣ�� */
#define IP_NDSERVER_ADDRINFO_MAX_NUM 11


#define IP_NDSERVER_GET_EPSBID(index) (g_ndServerAddrInfo[index].epsbId)

#define IP_NDSERVER_ADDRINFO_GET_ADDR(index) (&g_ndServerAddrInfo[index])

#define IP_NDSERVER_ADDRINFO_GET_EPSID(index) (g_ndServerAddrInfo[index].epsbId)

#define IP_NDSERVER_ADDRINFO_GET_TIMER(index) (&(g_ndServerAddrInfo[index].timerInfo))

#define IP_NDSERVER_ADDRINFO_GET_NWPARA(index) (&(g_ndServerAddrInfo[index].ipv6NwPara))

#define IP_NDSERVER_ADDRINFO_GET_TEINFO(index) (&(g_ndServerAddrInfo[index].teAddrInfo))

#define IP_NDSERVER_ADDRINFO_GET_MACADDR(index) (g_ndServerAddrInfo[index].ueMacAddr)

#define IP_NDSERVER_ADDRINFO_GET_MACFRAME(index) (g_ndServerAddrInfo[index].macFrmHdr)

#define IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index) (g_ndServerAddrInfo[index].dlPktQue)

#define IP_NDSERVER_GET_NDMSGDATA_ADDR() (&g_ndMsgData)

#define IP_NDSERVER_GET_SENDMSG_BUFFER() (g_ndserverSendMsgBuffer)

/*
 * 4 Enum
 */

/*
 * Э����:
 * ASN.1����:
 * ö��˵��: TE��ַ״̬����
 */
enum IP_NdserverTeAddrState {
    IP_NDSERVER_TE_ADDR_INEXISTENT = 0, /* ������ */
    IP_NDSERVER_TE_ADDR_INCOMPLETE = 1, /* δ��� */
    IP_NDSERVER_TE_ADDR_REACHABLE  = 2, /* �ɴ� */
    IP_NDSERVER_TE_ADDR_BUTT
};
typedef VOS_UINT8 IP_NdserverTeAddrStateUint8;

/*
 * 5 STRUCT
 */


/*
 * �ṹ��: IP_NdserverTeAddrInfo
 * Э����:
 * ASN.1����:
 * �ṹ˵��: ND SERVER�����TE��ַ�ṹ��
 */
typedef struct {
    VOS_UINT8                            teLinkLocalAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8                            teGlobalAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8                            teLinkLayerAddr[IP_MAC_ADDR_LEN];
    IP_NdserverTeAddrStateUint8 teAddrState;
    VOS_UINT8                            reserved[1];
} IP_NdserverTeAddrInfo;

/*
 * �ṹ��: IP_Ipv6Addr
 * Э����:
 * ASN.1����:
 * �ṹ˵��: IPV6��ַ�ṹ��
 */
typedef struct {
    VOS_UINT32 valid;
    VOS_UINT8  teGlobalAddr[IP_IPV6_ADDR_LEN];
} IP_Ipv6Addr;

/*
 * �ṹ��: IP_NdserverTeDetectBuf
 * Э����:
 * ASN.1����:
 * �ṹ˵��: ND SERVER�����TE��ַ�ṹ�壬����MAC��ַ��ȡ
 */
typedef struct {
    VOS_UINT32        head;
    VOS_UINT32        tail;
    VOS_UINT32        maxNum;
    IP_Ipv6Addr teIpBuf[2 * (IP_NDSERVER_ADDRINFO_MAX_NUM)]; /* 2:buf */
} IP_NdserverTeDetectBuf;

/*
 * �ṹ��: IP_NdserverAddrInfo
 * Э����:
 * ASN.1����:
 * �ṹ˵��: ND SERVER���ݽṹ��
 */
typedef struct {
    VOS_UINT8 epsbId;
    VOS_UINT8 validFlag;
    VOS_UINT8 reserved[14]; /* 14:rsv */
    /* Ϊ��ͬ64λ����ϵͳ���ݣ�����8�ֽڶ���λ�� */
    LUP_QUEUE_STRU *dlPktQue; /* ���а�������� */
    IP_SndMsg ipSndNwMsg;

    ESM_IP_Ipv6NWPara ipv6NwPara;

    IP_NdserverTeAddrInfo teAddrInfo;

    VOS_UINT8 ueLinkLocalAddr[IP_IPV6_ADDR_LEN];

    IP_TimerInfo timerInfo;

    VOS_UINT8 ueMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8 macFrmHdr[IP_ETH_MAC_HEADER_LEN];
} IP_NdserverAddrInfo;


/*
 * 7 Extern Global Variable
 */
extern IP_NdserverAddrInfo g_ndServerAddrInfo[];
extern VOS_UINT8 g_ndserverSendMsgBuffer[];
extern IP_NdserverTeDetectBuf g_ndServerTeDetectBuf;
extern IP_ND_Msg g_ndMsgData;

/*
 * 8 Fuction Extern
 */
VOS_VOID   NdSer_Ipv6PdnInfoCfg(VOS_UINT8 rabId, const AT_NDIS_Ipv6PdnInfo *ipv6PdnInfo);
VOS_UINT8 *NdSer_GetMacFrm(VOS_UINT8 index, VOS_UINT8 *teAddrState);
VOS_VOID   NdSer_Ipv6PdnRel(VOS_UINT8 rabId);
VOS_VOID   IP_NDSERVER_ProcTooBigPkt(VOS_UINT8 rabId, const VOS_UINT8 *srcData, VOS_UINT32 dataLen);
VOS_VOID   APP_NdServer_PidMsgProc(MsgBlock *rcvMsg);
VOS_VOID   NdSer_MacAddrInvalidProc(IMM_Zc *immZc, VOS_UINT8 index);
VOS_VOID IP_NDSERVER_ClearDlPktQue(VOS_UINT32 addrIndex);
VOS_VOID IP_NDSERVER_SaveTeDetectIp(const VOS_UINT8 *teGlobalAddr);

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of ip_nd_server.h */
