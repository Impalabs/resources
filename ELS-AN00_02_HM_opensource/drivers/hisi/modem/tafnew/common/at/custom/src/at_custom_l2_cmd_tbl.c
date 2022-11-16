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
#include "at_custom_l2_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_l2_set_cmd_proc.h"



#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_l2_CMD_TBL_C

static const AT_ParCmdElement g_atCustomL2CmdTbl[] = {
#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: LTE L2��������
     * [˵��]: ͨ��AT����LTE L2���������
     *         ˵����
     *         1��L2ͨ������BSRʵ���������١�BSR�����������Ǵ����ȵ�ӳ���ϵ����һ��������Χ�����Ǿ�ȷ����ָ�����������������ʡ�
     *         2����֧������Ϊ�㣬���޶�����С���ʺͻ�վʵ����أ����ܴ������������
     *         3��Ϊ������÷����ȶ��Եĸ���ԭ���ڸ�Modem�·���������������󣬲��ܰ�ʱ��Modem�·��ָ���������������Ҫ�����÷���ģ���ʼ��ʱ���·��ָ����������
     * [�﷨]:
     *     [����]: ^LL2COMCFG=<cmd_type>,<para1>[,<para2>[,<para3>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LL2COMCFG?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cmd_type>: ����ֵ���������ͣ�ȡֵ0~255��
     *             0�������������ʡ�
     *     <para1>: ����ֵ����Ӧ�������͵ĵ�1�������ȡֵ0~4294967295��
     *             cmd_type 0��0��ʾȡ�����ƣ��ָ�Ĭ�����ʣ�����ֵ��ʾ������������ֵ����λ0.1Mbps��
     *     <para2>: ����ֵ����Ӧ�������͵ĵ�2�������ȡֵ0~4294967295��
     *             ˵�����ò����ǿ�ѡ�������������Ͳ���2�����������øò�����
     *     <para3>: ����ֵ����Ӧ�������͵ĵ�3�������ȡֵ0~4294967295��
     *             ˵�����ò����ǿ�ѡ�������������Ͳ���3�����������øò�����
     * [ʾ��]:
     *     �� ����LTE�������٣�����ֵΪ1Mbps
     *       AT^LL2COMCFG=0,10
     *       OK
     *     �� �ر�LTE��������
     *       AT^LL2COMCFG=0,0
     *       OK
     */
    { AT_CMD_LL2COMCFG,
      AT_SetLL2ComCfgPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^LL2COMCFG", (VOS_UINT8 *)"(0-255),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: LTE L2�����ѯ
     * [˵��]: ͨ��AT��ѯL2���������������
     * [�﷨]:
     *     [����]: ^LL2COMQRY=<cmd_type>
     *     [���]: <CR><LF>^LL2COMQRY: <cmd_type>[,<para1>,<para2>,<para3>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LL2COMQRY?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cmd_type>: ����ֵ���������ͣ�ȡֵ0~255��
     *             0�������������ʡ�
     *     <para1>: ����ֵ����Ӧ�������͵ĵ�1�������ȡֵ0~4294967295��
     *             cmd_type 0��0��ʾȡ�����ƣ��ָ�Ĭ�����ʣ�����ֵ��ʾ������������ֵ����λ0.1Mbps��
     *     <para2>: ����ֵ����Ӧ�������͵ĵ�2�������ȡֵ0~4294967295��
     *             ˵�����ò����ǿ�ѡ�������������Ͳ���2�������򲻷��ظò�����
     *     <para3>: ����ֵ����Ӧ�������͵ĵ�3�������ȡֵ0~4294967295��
     *             ˵�����ò����ǿ�ѡ�������������Ͳ���3�������򲻷��ظò�����
     * [ʾ��]:
     *     �� ��ѯLTE��������ֵ
     *       AT^LL2COMQRY=0
     *       ^LL2COMQRY: 0,10,0,0
     *       OK
     */
    { AT_CMD_LL2COMQRY,
      AT_SetLL2ComQryPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^LL2COMQRY", (VOS_UINT8 *)"(0-255)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR L2��������
     * [˵��]: ͨ��AT����NR L2���������
     *         ˵����
     *         1��L2ͨ������BSRʵ���������١�BSR�����������Ǵ����ȵ�ӳ���ϵ����һ��������Χ�����Ǿ�ȷ����ָ�����������������ʡ�
     *         2����֧������Ϊ�㣬���޶�����С���ʺͻ�վʵ����أ����ܴ������������
     *         3��Ϊ������÷����ȶ��Եĸ���ԭ���ڸ�Modem�·���������������󣬲��ܰ�ʱ��Modem�·��ָ���������������Ҫ�����÷���ģ���ʼ��ʱ���·��ָ����������
     * [�﷨]:
     *     [����]: ^NL2COMCFG=<cmd_type>,<para1>[,<para2>[,<para3>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NL2COMCFG?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cmd_type>: ����ֵ���������ͣ�ȡֵ0~255��
     *             0�������������ʡ�
     *     <para1>: ����ֵ����Ӧ�������͵ĵ�1�������ȡֵ0~4294967295��
     *             cmd_type 0��0��ʾȡ�����ƣ��ָ�Ĭ�����ʣ�����ֵ��ʾ������������ֵ����λ0.1Mbps��
     *     <para2>: ����ֵ����Ӧ�������͵ĵ�2�������ȡֵ0~4294967295��
     *             ˵�����ò����ǿ�ѡ�������������Ͳ���2�����������øò�����
     *             ��ǰ�����£��˲���δͶ��ʹ�ã���ʵ�����塣
     *     <para3>: ����ֵ����Ӧ�������͵ĵ�3�������ȡֵ0~4294967295��
     *             ˵�����ò����ǿ�ѡ�������������Ͳ���3�����������øò�����
     *             ��ǰ�����£��˲���δͶ��ʹ�ã���ʵ�����塣
     * [ʾ��]:
     *     �� ����NR�������٣�����ֵΪ1Mbps
     *       AT^NL2COMCFG=0,10
     *       OK
     *     �� �ر�NR��������
     *       AT^NL2COMCFG=0,0
     *       OK
     */
    { AT_CMD_NL2COMCFG,
      AT_SetNL2ComCfgPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^NL2COMCFG", (VOS_UINT8 *)"(0-255),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR L2�����ѯ
     * [˵��]: ͨ��AT��ѯNR L2�������������
     * [�﷨]:
     *     [����]: ^NL2COMQRY=<cmd_type>
     *     [���]: <CR><LF>^NL2COMQRY: <cmd_type>[,<para1>,<para2>,<para3>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NL2COMQRY?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cmd_type>: ����ֵ���������ͣ�ȡֵ0~255��
     *             0�������������ʡ�
     *     <para1>: ����ֵ����Ӧ�������͵ĵ�1�������ȡֵ0~4294967295��
     *             cmd_type 0��0��ʾȡ�����ƣ��ָ�Ĭ�����ʣ�����ֵ��ʾ������������ֵ����λ0.1Mbps��
     *     <para2>: ����ֵ����Ӧ�������͵ĵ�2�������ȡֵ0~4294967295��
     *             ˵�����ò����ǿ�ѡ�������������Ͳ���2�������򲻷��ظò�����
     *             ��ǰ�����£��˲���δͶ��ʹ�ã���ʵ�����塣
     *     <para3>: ����ֵ����Ӧ�������͵ĵ�3�������ȡֵ0~4294967295��
     *             ˵�����ò����ǿ�ѡ�������������Ͳ���3�������򲻷��ظò�����
     *             ��ǰ�����£��˲���δͶ��ʹ�ã���ʵ�����塣
     * [ʾ��]:
     *     �� ��ѯNR��������ֵ
     *       AT^NL2COMQRY=0
     *       ^NL2COMQRY: 0,10,0,0
     *       OK
     */
    { AT_CMD_NL2COMQRY,
      AT_SetNL2ComQryPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^NL2COMQRY", (VOS_UINT8 *)"(0-255)" },

    { AT_CMD_NPDCP_SLEEPTHRES,
      AT_SetNPdcpSleepThresCfgPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^NPDCPALIVECFG", (VOS_UINT8 *)"(0-1),(0-4294967295)" },
#endif
#endif
};

/* ע��L2����AT����� */
VOS_UINT32 AT_RegisterCustomL2CmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomL2CmdTbl, sizeof(g_atCustomL2CmdTbl) / sizeof(g_atCustomL2CmdTbl[0]));
}

