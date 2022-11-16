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
#include "at_test_phy_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_test_phy_set_cmd_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_TEST_PHY_CMD_TBL_C

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
static const AT_ParCmdElement g_atTestPhyCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����TxTas�����㷨����
     * [˵��]: �������������������㷨�Ĳ�����
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^TXTASTEST=<RatMode>,<Para0>,<Para1>,<Para2>,<Para3>
     *     [���]: ��ȷ����ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TXTASTEST=?
     *     [���]: <CR><LF>^TXTASTEST: (0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <RatMode>: ���뼼����UINT32���ͣ�0-0xFFFFFFFF����
     *             0����GSM
     *             1����WCDMA
     *             2.����LTE
     *             3����TD_SCDMA
     *             4���� 1x
     *             5����Hrpd
     *     <Para0>: TxSarǿ�Ƶ�λ����UINT32���ͣ�0-0xFFFFFFFF����
     *             0ȡ��TxSarǿ�Ƶ�λ���㷨
     *             1ִ��TxSarǿ�Ƶ�λ���㷨
     *     <para1>: ������������UINT32���ͣ�0-0xFFFFFFFF��
     *     <para2>: TxSar ��λUINT32���ͣ�0-0xFFFFFFFF��
     *     <para3>: Ԥ�� UINT32���ͣ�0-0xFFFFFFFF��
     * [ʾ��]:
     *     �� ����TxTas�����㷨����
     *       AT^TXTASTEST=1,5,10,15,0
     *       OK
     *     �� ��������
     *       AT^TXTASTEST=?
     *       ^TXTASTEST: (0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)
     *       OK
     */
    { AT_CMD_TAS_TEST,
      AT_SetTasTestCfg, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TXTASTEST", (VOS_UINT8 *)"(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯTxTas ��ǰ��λ�͵�λ��
     * [˵��]: ��������Ĳ������õ���Ӧ��tas��λ�������Լ��ܵĵ�λ��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^TXTASTESTQRY=
     *             <RatMode>
     *     [���]: <CR><LF>level index: <ratmode>(para0,para1,parm2,param3)
     *             Level table: (para0,para1,parm2,param3)
     *             (para0,para1,parm2,param3)����
     *             <CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TXTASTESTQRY=?
     *     [���]: <CR><LF>^TXTASTESTQRY: (0-4294967295)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <RAT>: ���뼼����UINT32���ͣ�0-0xFFFFFFFF����
     *             0����GSM
     *             1����WCDMA
     *             2����LTE
     *             3����TD_SCDMA
     *             4���� 1x
     *             5 ����Hrpd
     * [ʾ��]:
     *     �� ��ѯTxTas ��ǰ��λ����ǰ��λ�����͵�λ��
     *       AT^TXTASTESTQRY=1
     *       level index: 1(3,128,9,5)
     *       level table:
     *       (2,19,40,8)
     *       (3,128,9,5)
     *     �� ��������
     *       AT^TXTASTESTQRY=?
     *       ^TXTASTESTQRY: (0-4294967295)
     *       OK
     */
    { AT_CMD_TAS_TEST_QUERY,
      AT_QryTasTestCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TXTASTESTQRY", (VOS_UINT8 *)"(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����SAR������Ч����
     * [˵��]: AT�����׮LTE PHY����DSDS3.5/DSDA����SAR��Ч��
     *         �������ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^LTESARSTUB=<mode>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ������ʧ��ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <mode>: ȡֵ0��1
     *             0������δ��Ч
     *             1��������Ч
     * [ʾ��]:
     *     �� set����
     *       AT^LTESARSTUB=0
     *       ERROR
     *       AT^LTESARSTUB=1
     *       OK
     */
    { AT_CMD_LTESARSTUB,
      AT_SetLteSarStubPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LTESARSTUB", (VOS_UINT8 *)"(0-1)" },
};

/* ע��PHY����������AT����� */
VOS_UINT32 AT_RegisterTestPhyCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atTestPhyCmdTbl, sizeof(g_atTestPhyCmdTbl) / sizeof(g_atTestPhyCmdTbl[0]));
}
#endif

