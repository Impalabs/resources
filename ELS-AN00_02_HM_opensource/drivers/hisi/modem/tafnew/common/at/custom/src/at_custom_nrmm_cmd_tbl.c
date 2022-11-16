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
#include "at_custom_nrmm_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_nrmm_set_cmd_proc.h"
#include "at_custom_nrmm_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_NRMM_CMD_TBL_C

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
static const AT_ParCmdElement g_atCustomNrmmCmdTbl[] = {
    /*
     * [���]: Э��AT-�绰����
     * [����]: ���úͲ�ѯ5G����ģʽѡ��
     * [˵��]: �������������úͲ�ѯUE��5G����ģʽѡ�
     *         ��������ڵ���֧��NRʱ����ʹ�ã�����ǰ����֧��NR��������
     *         Ŀǰ��֧��Option3 Only��Option2 Only��Option2+3�������ã�
     *         ʹ�����AT�����Ժ�������Ч��
     * [�﷨]:
     *     [����]: ^C5GOPTION=<nr_sa_support_flag>,<nr_dc_mode>,<5gc_access_mode>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^C5GOPTION?
     *     [���]: ִ�в�ѯ�ɹ�ʱ��
     *             <CR><LF>^C5GOPTION: <nr_sa_support_flag>,<nr_dc_mode>,<5gc_access_mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +C5GOPTION=?
     *     [���]: <CR><LF>+C5GOPTION:  (list of supported <nr_sa_support_flag>s), (list of supported <nr_dc_mode>s), (list of supported <5gc_access_mode>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <nr_sa_support_flag>: �Ƿ�֧��NR��������
     *             0����֧��NR��������SA������
     *             1��֧��NR��������SA������
     *     <nr_dc_mode>: NR��DC֧��ģʽ
     *             0����֧�ָ����ӣ�
     *             1������֧��ENDC��
     *             2������֧��NEDC��
     *             3��ENDC��NEDC��֧�֡�
     *     <5gc_access_mode>: �������5gc����ʽ
     *             0�����������5gc��
     *             1����������NR����5gc��
     *             2����������LTE����5gc��
     *             3������LTE��NR����5gc��
     * [ʾ��]:
     *     �� ����5G����ģʽѡ�OPTION 2��
     *       AT^C5GOPTION=1,0,1
     *       OK
     *     �� ����5G����ģʽѡ�OPTION 3��
     *       AT^C5GOPTION=0,1,0
     *       OK
     *     �� ����5G����ģʽѡ�OPTION 2+3��
     *       AT^C5GOPTION=1,1,1
     *       OK
     *     �� ��ѯ5G����ģʽѡ��
     *       AT^C5GOPTION?
     *       ^C5GOPTION: 1,1,1
     *       OK
     *     �� ����5G����ģʽѡ��
     *       AT^C5GOPTION=?
     *       ^C5GOPTION: (0,1),(0-3),(0-3)
     *       OK
     */
    { AT_CMD_C5GOPTION,
      At_Set5gOptionPara, AT_SET_PARA_TIME, At_Qry5gOptionPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^C5GOPTION", (VOS_UINT8 *)"(0,1),(0-3),(0-3)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ�����á�ȡ��NR��Band���������͸�Ƶ����
     * [˵��]: ���������ڲ�ѯ�����á�ȡ��NR��Band ���������͸�Ƶ���ع��ܡ�
     *         �������β���Ч���ɵ��ӣ�
     *         �����������֧����ĿΪ10��������������Ŀ���������������Ĳ���������ʧ�ܣ�
     *         �������������������߹رո�Ƶ�Ĳ����������UE֧�ֵ�NR Bandȫ�������ã����β�����ʧ�ܡ���������ͨ�������NR������Band����ֹ����
     *         AT����سɹ�����ʾ����������ɹ���band������Ч���첽���̣���������Ч����
     *         ��������ڵ���֧��NRʱ����ʹ�ã�����ǰ����֧��NR��������
     *         ������ֻ��ͨ��һ��ͨ���·�����֧�ֶ�ͨ���·����
     * [�﷨]:
     *     [����]: ^NRBANDBLOCKLIST=<opmode>[,<bandnum>,[<bandlist>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NRBANDBLOCKLIST?
     *     [���]: <CR><LF>^NRBANDBLOCKLIST: <HibandSwitch>,< bandnum >[<bandlist>]
     *             <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <opmode>: ����ֵ����ʶ���β������͡�
     *             0���ſ����и�ƵƵ�Σ�
     *             1����ֹ���и�ƵƵ�Σ�
     *             2����ָ��Ƶ�δ���������ȥ����
     *             3����ָ��Ƶ�μ�������������
     *             ע�⣺������Ч���ɵ��ӣ���Ƶ���غ����������Ĳ������Զ�����
     *     <bandnum>: ����ֵ��Bandlist ����Ŀ�����Ϊ10��
     *     <bandlist>: �ַ������ͣ�Band�б����洢��10��band�ţ�ʹ�ö��Ÿ�����
     *     <HibandSwitch>: ����ֵ����ѯ�ӿڷ��صĸ�Ƶ����״̬��
     *             0����ƵƵ��ȫ���ſ���
     *             1����ƵƵ��ȫ����ֹ��
     * [ʾ��]:
     *     �� ���ø�Ƶ����
     *       AT^NRBANDBLOCKLIST=1
     *       OK
     *     �� ����band�����������·���LIST����������ظ��򷵻�ʧ�ܣ�
     *       AT^NRBANDBLOCKLIST=3,2,"77,78"
     *       OK
     *     �� ��ѯband��������
     *       AT^NRBANDBLOCKLIST?
     *       ^NRBANDBLOCKLIST: 0,2,77,78
     *       OK
     *     �� ����band��������
     *       AT^NRBANDBLOCKLIST=?
     *       ^NRBANDBLOCKLIST: (0-3),(1-10),(bandlist)
     *       OK
     */
    { AT_CMD_NRBANDBLOCKLIST,
      AT_SetNrBandBlockListPara, AT_SET_PARA_TIME, At_QryNrBandBlockListPara, AT_QRY_PARA_TIME, VOS_NULL_PTR,
      AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NRBANDBLOCKLIST", (VOS_UINT8 *)"(0-3),(1-10),(bandlist)" },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
#if (FEATURE_NSSAI_AUTH == FEATURE_ON)
    { AT_CMD_C5GNSSAA,
      AT_SetC5gNssaaPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^C5GNSSAA", (VOS_UINT8 *)"(sNssaiStr),(eapMsgStr)" },
#endif
#endif
};

/* ע��NRMM����AT����� */
VOS_UINT32 AT_RegisterCustomNrmmCmdTable(VOS_VOID)
{
   return AT_RegisterCmdTable(g_atCustomNrmmCmdTbl, sizeof(g_atCustomNrmmCmdTbl) / sizeof(g_atCustomNrmmCmdTbl[0]));
}
#endif

