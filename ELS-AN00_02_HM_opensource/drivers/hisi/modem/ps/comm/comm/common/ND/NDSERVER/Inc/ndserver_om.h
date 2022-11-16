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

#ifndef __NDSERVEROM_H__
#define __NDSERVEROM_H__
/*
 * 1 ����ͷ�ļ�����
 */
#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

#define IP_NDSERVER_GET_STATINFO_ADDR(index) (&g_ndServerPktStatInfo[index])
#define IP_NDSERVER_AddRcvPktTotalNum(index) (g_ndServerPktStatInfo[index].rcvPktTotalNum++)
#define IP_NDSERVER_AddDiscPktNum(index) (g_ndServerPktStatInfo[index].discPktNum++)
#define IP_NDSERVER_AddRcvNsPktNum(index) (g_ndServerPktStatInfo[index].rcvNsPktNum++)
#define IP_NDSERVER_AddRcvNaPktNum(index) (g_ndServerPktStatInfo[index].rcvNaPktNum++)
#define IP_NDSERVER_AddRcvRsPktNum(index) (g_ndServerPktStatInfo[index].rcvRsPktNum++)
#define IP_NDSERVER_AddRcvEchoPktNum(index) (g_ndServerPktStatInfo[index].rcvEchoPktNum++)
#define IP_NDSERVER_AddRcvBooBigPktNum(index) (g_ndServerPktStatInfo[index].rcvTooBigPktNum++)
#define IP_NDSERVER_AddRcvDHCPV6PktNum(index) (g_ndServerPktStatInfo[index].rcvDhcpv6PktNum++)
#define IP_NDSERVER_AddErrNsPktNum(index) (g_ndServerPktStatInfo[index].errNsPktNum++)
#define IP_NDSERVER_AddErrNaPktNum(index) (g_ndServerPktStatInfo[index].errNaPktNum++)
#define IP_NDSERVER_AddErrRsPktNum(index) (g_ndServerPktStatInfo[index].errRsPktNum++)
#define IP_NDSERVER_AddErrEchoPktNum(index) (g_ndServerPktStatInfo[index].errEchoPktNum++)
#define IP_NDSERVER_AddErrTooBigPktNum(index) (g_ndServerPktStatInfo[index].errTooBigPktNum++)
#define IP_NDSERVER_AddErrDhcpv6PktNum(index) (g_ndServerPktStatInfo[index].errDhcpv6PktNum++)
#define IP_NDSERVER_AddTransPktTotalNum(index) (g_ndServerPktStatInfo[index].transPktTotalNum++)
#define IP_NDSERVER_AddTransPktFailNum(index) (g_ndServerPktStatInfo[index].transPktFailNum++)
#define IP_NDSERVER_AddTransNsPktNum(index) (g_ndServerPktStatInfo[index].transNsPktNum++)
#define IP_NDSERVER_AddTransNaPktNum(index) (g_ndServerPktStatInfo[index].transNaPktNum++)
#define IP_NDSERVER_AddTransRaPktNum(index) (g_ndServerPktStatInfo[index].transRaPktNum++)
#define IP_NDSERVER_AddTransEchoPktNum(index) (g_ndServerPktStatInfo[index].transEchoPktNum++)
#define IP_NDSERVER_AddTransTooBigPktNum(index) (g_ndServerPktStatInfo[index].transTooBigPktNum++)
#define IP_NDSERVER_AddTransDhcpv6PktNum(index) (g_ndServerPktStatInfo[index].transDhcpv6PktNum++)
#define IP_NDSERVER_AddMacInvalidPktNum(index) (g_ndServerPktStatInfo[index].macInvalidPktNum++)
#define IP_NDSERVER_AddEnquePktNum(index) (g_ndServerPktStatInfo[index].enquePktNum++)
#define IP_NDSERVER_AddSendQuePktNum(index) (g_ndServerPktStatInfo[index].sendQuePktNum++)

/*
 * �ṹ��: IP_NdserverPacketStatisticsInfo
 * Э����:
 * ASN.1����:
 * �ṹ˵��: ND SERVER����ͳ�����ݽṹ��
 */
typedef struct {
    VOS_UINT32 rcvPktTotalNum;    /* �յ����ݰ����� */
    VOS_UINT32 discPktNum;        /* �������ݰ����� */
    VOS_UINT32 rcvNsPktNum;       /* �յ�NS���ݰ����� */
    VOS_UINT32 rcvNaPktNum;       /* �յ�NA���ݰ����� */
    VOS_UINT32 rcvRsPktNum;       /* �յ�RS���ݰ����� */
    VOS_UINT32 rcvEchoPktNum;     /* �յ�PING������ */
    VOS_UINT32 rcvTooBigPktNum;   /* �յ�IPV6���������� */
    VOS_UINT32 rcvDhcpv6PktNum;   /* �յ�DHCPV6������ */
    VOS_UINT32 errNsPktNum;       /* ����NS���ݰ����� */
    VOS_UINT32 errNaPktNum;       /* ����NA���ݰ����� */
    VOS_UINT32 errRsPktNum;       /* ����RS���ݰ����� */
    VOS_UINT32 errEchoPktNum;     /* ����ECHO REQ���ݰ����� */
    VOS_UINT32 errTooBigPktNum;   /* ���󳬳������� */
    VOS_UINT32 errDhcpv6PktNum;   /* ����DHCPV6���ݰ����� */
    VOS_UINT32 transPktTotalNum;  /* �������ݰ����� */
    VOS_UINT32 transPktFailNum;   /* �������ݰ�ʧ������ */
    VOS_UINT32 transNsPktNum;     /* ����NS���ݰ����� */
    VOS_UINT32 transNaPktNum;     /* ����NA���ݰ����� */
    VOS_UINT32 transRaPktNum;     /* ����RA���ݰ����� */
    VOS_UINT32 transEchoPktNum;   /* ����ECHO REPLY���ݰ����� */
    VOS_UINT32 transTooBigPktNum; /* ����IPV6��������Ӧ���� */
    VOS_UINT32 transDhcpv6PktNum; /* ����DHCPV6 REPLY���ݰ����� */
    VOS_UINT32 macInvalidPktNum;  /* ����IP������ʱPC MAC��ַ��Ч��ͳ���� */
    VOS_UINT32 enquePktNum;       /* ���гɹ������IP������ */
    VOS_UINT32 sendQuePktNum;     /* ���гɹ����ͻ����IP������ */
} IP_NdserverPacketStatisticsInfo;

extern IP_NdserverPacketStatisticsInfo g_ndServerPktStatInfo[];

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

#endif





