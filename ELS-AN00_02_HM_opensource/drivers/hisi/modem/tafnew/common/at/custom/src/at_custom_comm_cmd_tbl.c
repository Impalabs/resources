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
#include "at_custom_comm_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_comm_set_cmd_proc.h"
#include "at_custom_comm_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_COMM_CMD_TBL_C

static const AT_ParCmdElement g_atCustomCommCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���͸��ײ����ͨ�õ�BOOSTER����
     * [˵��]: ���͸��ײ����ͨ�õ�BOOSTER���^COMMBOOSTER��
     *         �������ݲ�֧�֡�
     *         ���õ�MODULEIDΪRRC(4)ʱ����ָ������ָ���ĸ�RRC���ã���modem���ݵ�ǰפ����ģʽ������Ӧ��RRC��Ŀǰֻ֧��LTE��NR�������ǰû��פ����LTE����NR����ظ�CME ERROR��
     * [�﷨]:
     *     [����]: ^COMMBOOSTER=<TYPE>,<MODULEID>,<LEN>,<PARAM>
     *     [���]: <type>����Ϊ����ʱ��
     *             ���͸�������������OKʱ��
     *             <CR><LF>^COMMBOOSTER: 0<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���÷��͸������������ش���ʱ��
     *             1��������ش���
     *             <CR><LF>^COMMBOOSTER: <err><CR><LF>
     *             <CR><LF>ERROR<CR><LF>
     *             2��NASֱ�ӷ��ش���
     *             <CR><LF>CME ERROR<CR><LF>
     *             <type>����Ϊ��ѯʱ��
     *             ��ѯ�ɹ���
     *             <CR><LF>^COMMBOOSTER: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��ѯʧ��ʱ��
     *             <CR><LF>CME ERROR<CR><LF>
     *     [����]: ^COMMBOOSTER=?
     *     [���]: <CR><LF>^ COMMBOOSTER: (list of supported <TYPE>s), (list of supported < MODULEID > s) , (list of supported < LEN >s),(str)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <TYPE>: �������ͣ����Σ�ȡֵ��Χ(1,2)��
     *             1�����ã�
     *             2����ѯ��
     *     <MODULEID>: ��Ϣ������������Σ�ȡֵ��Χ(0~255)��
     *             0��ERRC��
     *             1��RTTAGENT��
     *             2��MMC��
     *             3��BASTET����BASTET�꿪������Ч����
     *             4��RRC��
     *             ע�⣺���������У���FEATURE_MBB_CUST�꿪��,�ò�������Ϊ0������LTEģʽ������Ϊ4�����֧�֡�
     *     <LEN>: <PARAM>�ĳ��ȣ����Σ�ȡֵ��Χ(1~250)
     *     <PARAM>: �ַ�����TLV��ʽ�Ĳ�����T��L��2���ֽڣ����嶨���ɸ����ά��������ʧ�ܵĴ���ԭ��ֵҲ�ɸ����������ά��
     * [ʾ��]:
     *     �� ���͸�LRRC��BOOSTER��������
     *       AT^COMMBOOSTER=1,0,4,"01021236"
     *       ^COMMBOOSTER: 0
     *       OK
     *     �� ��������
     *       AT^COMMBOOSTER=?
     *       ^COMMBOOSTER: (1,2),(0-255),(1-250),(str)
     *       OK
     */
    { AT_CMD_COMMBOOSTER,
      AT_SetCommBoosterPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^COMMBOOSTER", (VOS_UINT8 *)"(1,2),(0-255),(1-250),(str)" },

    { AT_CMD_SARREDUCTION,
      AT_SetSARReduction, AT_SET_PARA_TIME, AT_QrySARReduction, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SARREDUCTION", (VOS_UINT8 *)"(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ����汾�ż��汾����ʱ��
     * [˵��]: ���ⲿ����汾�ż��汾����ʱ��Ĳ�ѯ������ظ�AP��
     * [�﷨]:
     *     [����]: ^SWVER
     *     [���]: <CR><LF>^SWVER: <external_sw_version>_(<build_time>)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^SWVER=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <external_sw_version>: �ַ������ͣ��ⲿ����汾�š�
     *     <build_time>: �ַ������ͣ��汾����ʱ�䣬��ʽ��Oct 15 2009, 16:27:00��
     * [ʾ��]:
     *     �� ��ѯ����汾�ż��汾����ʱ��
     *       AT^SWVER
     *       ^SWVER: 21C10B570S007C000_(May 19 2018, 16:30:49)
     *       OK
     */
    { AT_CMD_SWVER,
      At_SetSwverPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SWVER", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���ù��ϳ����ϱ�CHR������Ϣ
     * [˵��]: �������������ù��ϳ����ϱ�CHR������Ϣ��
     * [�﷨]:
     *     [����]: ^CHRALARMRLATCFG=<alarmid>[,<alarmiddetail>]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�в�ѯ�ɹ�ʱ��
     *             <CR><LF>^CHRALARMRLATCFG: <alarmid detail><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR:  <err_code><CR><LF>
     *             ע�⣺���ֻ��<alarmid>һ������ʱ��ִ�е��ǲ�ѯ������Ϣ��
     *     [����]: ^CHRALARMRLATCFG=?
     *     [���]: <CR><LF>^CHRALARMRLATCFG: (list of supported <alarmid>s), (list of supported <alarmid detail>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <alarmid>: ���ϳ�����ȡֵ��Χ0~39�����40�顣
     *     <alarmid detail>: ���ϸ澯����ԣ�32bit��ÿ��bit����һ������ԣ�ȡֵ��Χ0~ 4294967295��0����͸�������޹ء�
     *             ÿ��bit��Ӧ�Ĺ�ϵ��鿴NV135����ϸ˵����
     * [ʾ��]:
     *     �� ���ù��ϳ����ϱ�CHR������Ϣ
     *       AT^CHRALARMRLATCFG=0,4097
     *       OK
     *     �� ��ѯ���ϳ����ϱ�CHR������Ϣ
     *       AT^CHRALARMRLATCFG=0
     *       ^CHRALARMRLATCFG: 4097
     *       OK
     */
    { AT_CMD_CHRALARMRLATCFG,
      AT_SetChrAlarmRlatCnfInfoPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^CHRALARMRLATCFG", (VOS_UINT8 *)"(0-39),(0-4294967295)" },

#if (FEATURE_MBB_CUST == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: VOIP APN�����ò�ѯAT����
     * [˵��]: ���á���ȡ����ѯVOIP APN KEY���ݡ�����ʱ�����з������ж������ݡ���������DATALOCK������������δ�������룬��ѯ���ݷ��ؿգ��Ѷ�������Ϊ0����
     * [�﷨]:
     *     [����]: ^VOIPAPNKEY=<group>,<content>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ����ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^VOIPAPNKEY?
     *     [���]: <CR><LF>^VOIPAPNKEY: <number><CR><LF><CR><LF>^VOIPAPNKEY: <group>,<content><CR><LF>[��]<CR><LF>OK<CR><LF>
     *             ����ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^VOIPAPNKEY=?
     *     [���]: ^VOIPAPNKEY=?<CR><LF>^VOIPAPNKEY: <total><CR><LF><CR><LF>OK<CR><LF>
     *             ����ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <group>: �����indexֵ��ȡֵ��Χ0~256��Ŀǰȡֵֻ��Ϊ0��
     *     <content>: ��Ӧ��VOIP APN KEY�����ݣ���˫���ţ���󳤶�Ϊ95��
     * [ʾ��]:
     *     �� ����VOIP APN KEY����
     *       AT^VOIPAPNKEY=0,"xxxx"
     *       OK              //��ȷ����OK������ʱ����ERROR
     */
    { AT_CMD_VOIPAPNKEY,
      AT_SetVoipApnKeyPara, AT_NOT_SET_TIME, AT_QryVoipApnKeyPara, AT_NOT_SET_TIME, AT_TestVoipApnKeyPara,
      AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VOIPAPNKEY", (VOS_UINT8 *)"(0-1),(@password)" },
#endif

    { AT_CMD_WIFIGLOBALMAC,
      AT_SetWifiGlobalMacPara, AT_NOT_SET_TIME, AT_QryWifiGlobalMacPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^WIFIGLOBALMAC", VOS_NULL_PTR },
};

/* ע��COMM����AT����� */
VOS_UINT32 AT_RegisterCustomCommCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomCommCmdTbl, sizeof(g_atCustomCommCmdTbl) / sizeof(g_atCustomCommCmdTbl[0]));
}

