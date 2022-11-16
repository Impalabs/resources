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
#include "at_custom_lrrc_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_lrrc_set_cmd_proc.h"
#include "at_custom_lrrc_qry_cmd_proc.h"
#include "at_lte_common.h"

#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_LRRC_CMD_TBL_C

static const AT_ParCmdElement g_atCustomLrrcCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����LTE�տ�������Ϣ�ϱ�
     * [˵��]: ��������������VoLTE��Ƶͨ��������LTE�տ�������Ϣ�ϱ����á��򿪺͹ر�^LPDCPINFORPT��^LTERLQUALINFO��������������ϱ����Ա�Ӧ�ø��ݵ�ǰLTE�Ŀտ�������Ϣ������Ƶͨ�������ʣ������û����֡�
     * [�﷨]:
     *     [����]: ^VTRLQUALRPT=<enable>,<threshold>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^VTRLQUALRPT=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ�������ϱ����ƿ��أ�
     *             0���ر������ϱ���
     *             1�����������ϱ���
     *     <threshold>: ����ֵ��LPDCP��Ϣ�ϱ���ֵ����λ��LPDCP������ʱ���ٷֱȣ�ȡֵ��Χ1~100��
     * [ʾ��]:
     *     �� ����LTE�տ�������Ϣ�ϱ�
     *       AT^VTRLQUALRPT=1,35
     *       OK
     */
    { AT_CMD_VTRLQUALRPT,
      AT_SetVtrlqualrptPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VTRLQUALRPT", (VOS_UINT8 *)"(0,1),(1-100)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����LTE SCell��CA������Ϣ
     * [˵��]: ���������ڿ���LTE�µ�SCell��CA���ܱ�����ʱʹ�ܡ�ȥʹ�ܣ������޸�ƽ̨CA��֧��������������ʱȥʹ��CAʱ��Ҫ����CA���A2��A4��CQI=0���ܿ��ؼ�Band��Ϣ����BandƵ�ʷ�Χ�ȡ�
     * [�﷨]:
     *     [����]: ^LCACFG=<enable>[,<cfg_para>,<band_num>[,<band>[,<band>[,<band>[,<band>[,<band>[,<band>[,<band>[,<band>]]]]]]]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^LCACFG=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ��CA���ܱ�����ʱ���أ�
     *             0��ȥʹ��CA���ܣ�
     *             1��ʹ��CA���ܡ�
     *     <cfg_para>: ����ֵ��ͨ��bitλ������CA��ع��ܵĿ���״̬��<enable>Ϊ0ʱ�·���bitλֵΪ0����رն�Ӧ���ܣ�Ϊ1��򿪶�Ӧ���ܣ�bitλ�빦�ܶ�Ӧ��ϵ���£�
     *             bit 0��A2���ܣ�
     *             bit 1��A4���ܣ�
     *             bit 2��CQI=0���ܡ�
     *     <band_num>: ����ֵ��������ʱȥʹ��CAʱ�����õ�Band������ȡֵ��Χ0~8��0��ʾȥʹ�����е�CA SCell��
     *     <band>: 16�����ַ�������������Ϊ72���ַ�������Band��Ϣ����BandƵ�ʷ�Χ���ṹ����������С��ת������
     * [ʾ��]:
     *     �� ������ʱȥʹ��CA���ܣ���A2��CQI=0����
     *       AT^LCACFG=0,5,2,"2E0000017478360009F15600000000000000000000000000000000000000000000000000","300000000000000000000000000000000000000000000000000000000000000000000000"
     *       OK
     *     �� ʹ��CA����
     *       AT^LCACFG=1
     *       OK
     *     �� ִ�в�������
     *       AT^LCACFG=?
     *       OK
     */
    { AT_CMD_LCACFG,
      AT_SetLCaCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^LCACFG", (VOS_UINT8 *)"(0,1),(0-7),(0-8),(str),(str),(str),(str),(str),(str),(str),(str)" },
#if (FEATURE_MBB_CUST == FEATURE_ON)
    { AT_CMD_LTETDDSUBFRAME,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryTddSubframePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^LTETDDSUBFRAME", VOS_NULL_PTR },
    { AT_CMD_LCELLINFO,
      AT_SetLCellInfoPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LCELLINFO", (VOS_UINT8 *)"(0,1)" },
#endif
};

/* ע��LRRC����AT����� */
VOS_UINT32 AT_RegisterCustomLrrcCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomLrrcCmdTbl, sizeof(g_atCustomLrrcCmdTbl) / sizeof(g_atCustomLrrcCmdTbl[0]));
}

