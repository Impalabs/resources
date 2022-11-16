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
#include "at_custom_rcm_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_rcm_set_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_RCM_CMD_TBL_C

#if (FEATURE_DCXO_HI1102_SAMPLE_SHARE == FEATURE_ON)
static const AT_ParCmdElement g_atCustomRcmCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: DCXO������ȡ
     * [˵��]: ���������ڻ�ȡDCXO������Ϣ��
     *         ע�⣺�ܺ���ƣ�������Ŀǰ����Orlandoƽ̨ʹ�á�
     *         �������ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^DCXOQRY=<SampleType>
     *     [���]: <CR><LF>^DCXOQRY: <SampleType ><Result > [[,<aTime>,<aMode>,<aTemp>,<aPpm>],[,<aTime>,<aMode>,<aTemp>,<aPpm>],>],[,<aTime>,<aMode>,<aTemp>,<aPpm>],����]
     *             [,<Frequency >,<Teperature]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <SampleType>: ����ֵ��DCXO�������͡�
     *             0�����¶��������ݣ�
     *             1�����¶��������ݣ�
     *             2�����¶��������ݣ�
     *             3������У׼�����
     *     <Result>: ����ֵ��DCXO������ѯ�����
     *             0����ѯ�����ȷ��
     *             1����ѯʧ�ܣ�
     *             2����ѯ�����������
     *             3����ʱ����ʱ��
     *             4��δ֪����
     *     <aTime>: ����ֵ��DCXO����ʱ�䡣���£�����2016.4��ʾΪ1604��С��1604��ʾ��������Ч
     *     <aMode>: ����ֵ��DCXO����ģʽ��
     *     <aTemp>: ����ֵ��DCXO�����¶ȡ������¶ȵ�λΪ0.001�ȡ�
     *     <aPpm>: ����ֵ��DCXO�������Ƶƫ�����㻯2^31/10^6
     *     <Frequency>: ��ͨ����ĳ�ʼƵƫ
     *     <Teperature>: ��ͨ����ĳ�ʼ�¶�
     * [ʾ��]:
     *     �� ��ѯ���¶�DCXO����
     *       AT^DCXOQRY=0
     *       ^DCXOQRY: 0,0,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139
     *       OK
     *     �� ��ѯ���¶�DCXO����
     *       AT^DCXOQRY=1
     *       ^DCXOQRY: 1,0,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139
     *       OK
     *     �� ��ѯ���¶�DCXO����
     *       AT^DCXOQRY=2
     *       ^DCXOQRY: 2,0,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139
     *       OK
     *     �� ��ѯ����У׼���
     *       AT^DCXOQRY=3
     *       ^DCXOQRY: 3,0,100,101
     *       OK
     */
    { AT_CMD_DCXOQRY,
      AT_SetDcxoQryPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DCXOQRY", (TAF_UINT8 *)"(0-3)" },
};

/* ע��RCM����AT����� */
VOS_UINT32 AT_RegisterCustomRcmCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomRcmCmdTbl, sizeof(g_atCustomRcmCmdTbl) / sizeof(g_atCustomRcmCmdTbl[0]));
}
#endif

