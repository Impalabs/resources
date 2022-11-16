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
#include "at_ss_taf_cmd_tbl.h"
#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_ss_taf_set_cmd_proc.h"
#include "at_ss_taf_qry_cmd_proc.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_SS_TAF_CMD_TBL_C

static const AT_ParCmdElement g_atSsTafCmdTbl[] = {
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ������ز���ҵ��
     * [˵��]: ͨ�����������Կ������в���ҵ��
     *         ���б��ֺͻظ�
     *         �෽ͨ��
     *         �����͹ҶϺ���
     * [�﷨]:
     *     [����]: ^CHLD=<n>,<call_type>
     *             [,<num>[,<rtt>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <n>: ����ֵ�������롣
     *             0���Ҷ����б����ֵĻ��ߵȴ��ĺ��У���ͬʱ���ڱ����ֵĺ��к͵ȴ��ĺ��У���ֻ�Ҷ����ڵȴ��ĺ��У�
     *             1�� �Ҷ�����ͨ���ĺ��У��������ȴ��ĺ��л��߻ָ������ֵĺ��У�
     *             2�� ��������ͨ���ĺ��У��������ȴ��ĺ��л��߻ָ������ֵĺ��У�
     *             3�� �����෽���У�
     *             1x�� �Ҷϵ�x�����У�
     *             2x�� ���ֳ���x����������������������У�
     *             4�� �����ֵĺ��к͵�ǰ�����罨��ͨ�������з��������У�������ͨ���ĺ��к͵ȴ��ĺ��н���ͨ�������з��������У�
     *             5�� �����������緢��ĺ��У�CCBS����
     *             6����ǿ�Ͷ෽ͨ���йҶϵ绰����Ϊnum���û�
     *             7�� �ϲ���ǿ�Ͷ෽ͨ������ͨ�绰
     *             8�� �����������һ·ͨ�������ֵ�״̬��������һ·�ȴ��ĺ���
     *     <call_type>: ����ֵ���������͡�
     *             0�� ��������
     *             1�� ��Ƶͨ������������Ƶ��˫������
     *             2�� ��Ƶͨ�������������Ƶ��˫������
     *             3�� ��Ƶͨ����˫����Ƶ��˫������
     *     <num>: �ַ������ͣ���Ҫ�Ҷϵĵ绰����
     *             ˵�����������ֻ�е�<n>=6ʱ�������·�<num>,������Ϊ��������
     *     <rtt>: ����ֵ��ָʾ�Ƿ�RTTģʽ����ͨ��������<n>Ϊ2��8ʱ��Ч��
     *             0����RTTͨ����
     *             1��RTTͨ����
     * [ʾ��]:
     *     �� ���������绰
     *       AT^CHLD=8,0
     *       OK
     *     �� ����ǿ�Ͷ෽ͨ���йҶ�ĳ���û�
     *       AT^CHLD=6,0,"135xxxxxxxx"
     *       OK
     *     �� �ϲ���ǿ�Ͷ෽ͨ������ͨͨ��
     *       AT^CHLD=7,0
     *       OK
     *     �� ��RTTģʽ��������
     *       AT^CHLD=8,0,,1
     *       OK
     */
    { AT_CMD_CHLD_EX,
      At_SetChldExPara, AT_SET_CALL_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CHLD", (VOS_UINT8 *)"(0-8,11-19,21-29),(0-3),(@number),(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ��ѯ��������
     * [˵��]: ����ʱ��ѯUnicode�����ʽ���������ơ�
     * [�﷨]:
     *     [����]: ^CNAP?
     *     [���]: <CR><LF>^CNAP: <name>,<CNI_validity><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: ^CNAP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <name>: �ַ������ͣ�Unicode�����ʽ��HEX��������󳤶�160�ֽڣ�320��ASCII�ַ�����
     *     <CNI_validity>: ����ֵ������������ʾ��Ч�ԣ�
     *             0��CNI��Ч��
     *             1��CNI�����з��ܾ���ʾ��
     *             2�����ڷ�����������ƻ������������CNI�����á�
     * [ʾ��]:
     *     �� ����ʱ��ѯUSC-2����������������Ϊ������abc123��
     *       AT^CNAP?
     *       ^CNAP: "674E96F7006100620063003100320033",0
     *       OK
     *     �� Xģ����ʱ��ѯ
     *       AT^CNAP?
     *       OK
     */
    { AT_CMD_CNAPEX,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryCnapExPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CNAP", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����·ѡ��
     * [˵��]: �ù��ܿ��õ�ǰ���Ǵ����д򿪶���·ѡ��ĺ꿪�ء�
     *         ͬһ��SIM��֧�������绰����ʱ�����ô��������ò������еĺ��롣
     * [�﷨]:
     *     [����]: ^ALS=<view>[,<line>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^ALS?
     *     [���]: <CR><LF>^ALS: <view>,<line><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^ALS=?
     *     [���]: <CR><LF>^ALS: (list of supported <view>s),(list of supported <line>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <view>: ����ֵ����ֹ��ʹ�ܵ�ǰ������·֪ͨ��Ĭ��ֵΪ0��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     *     <line>: ����ֵ����ǰ����ʱ����·�ţ�Ĭ��ֵΪ1��
     *             1������绰ʱʹ����·1�ĺ�����к��У�
     *             2������绰ʱʹ����·2�ĺ�����к��С�
     * [ʾ��]:
     *     �� ʹ�ܵ�ǰ������·֪ͨ����ʹ����·1���к���
     *       AT^ALS=1,1
     *       OK
     *     �� ��ѯ����·ѡ��ҵ��
     *       AT^ALS?
     *       ^ALS: 1,1
     *       OK
     *     �� ����ALS
     *       AT^ALS=?
     *       ^ALS: (0,1),(1,2)
     *       OK
     */
    { AT_CMD_ALS,
      AT_SetAlsPara, AT_SET_PARA_TIME, AT_QryAlsPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^ALS", (VOS_UINT8 *)"(0,1),(1,2)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����USSD����ģʽ
     * [˵��]: ����USSD����Ϊ͸�����߷�͸�����������ý�����豣�浽NV�С������ʼ��ʱ�ȳ��Զ�ȡNV�����NVδ���������ussd_default_table�����ò�ƷĬ�ϵ�USSD������
     *         ^USSDMODE?�����ѯ���嵱ǰ֧�ֵ�USSD����������ȫ�ֱ���ussd_mode��ǰֵ��
     * [�﷨]:
     *     [����]: ^USSDMODE=<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^USSDMODE?
     *     [���]: <CR><LF>^USSDMODE: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^USSDMODE=?
     *     [���]: <CR><LF>^USSDMODE: (list of supported <mode>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ��
     *             0������ʹ��USSD��͸��������
     *             1������ʹ��USSD͸��������
     * [ʾ��]:
     *     �� ���õ�ǰģʽΪ͸��ģʽ
     *       AT^USSDMODE=1
     *       OK
     */
    { AT_CMD_USSDMODE,
      At_SetUssdModePara, AT_NOT_SET_TIME, At_QryUssdModePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^USSDMODE", (VOS_UINT8 *)"(0-1)" },

#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �������Ƶͨ������
     * [˵��]: �������Ƶͨ�����͡�
     * [�﷨]:
     *     [����]: ^CALLMODIFYINIT=<idx>,<curr_call_type>,<dest_call_type>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CALLMODIFYINIT=?
     *     [���]: <CR><LF>^CALLMODIFYINIT: (list of supported <idx>s),(list of supported <curr_call_type>s),(list of supported <dest_call_type>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <idx>: ����ֵ������ID��ȡֵ��Χ1~7
     *     <curr_call_type>: ����ֵ����ǰ�������͡�
     *             0�� ��������
     *             1�� ��Ƶͨ������������Ƶ��˫������
     *             2�� ��Ƶͨ�������������Ƶ��˫������
     *             3�� ��Ƶͨ����˫����Ƶ��˫������
     *     <dest_call_type>: ����ֵ��Ŀ��������͡�
     *             0�� ��������
     *             1�� ��Ƶͨ������������Ƶ��˫������
     *             2�� ��Ƶͨ�������������Ƶ��˫������
     *             3�� ��Ƶͨ����˫����Ƶ��˫������
     * [ʾ��]:
     *     �� ��Ƶͨ���л�ת����Ƶͨ��
     *       AT^CALLMODIFYINIT=1,0,3
     *       OK
     *     �� ��������
     *       AT^CALLMODIFYINIT=?
     *       ^CALLMODIFYINIT: (1-7),(0-3),(0-3)
     *       OK
     */
    { AT_CMD_CALL_MODIFY_INIT,
      AT_SetCallModifyInitPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CALLMODIFYINIT", (VOS_UINT8 *)"(1-7),(0-3),(0-3)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ܶԷ�����Ƶͨ�����ͱ������
     * [˵��]: ���ܶԷ�����Ƶͨ�����ͱ������
     * [�﷨]:
     *     [����]: ^CALLMODIFYANS=<idx>,<curr_call_type>,<dest_call_type>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CALLMODIFYANS=?
     *     [���]: <CR><LF>^CALLMODIFYANS: (list of supported <idx>s),(list of supported <curr_call_type>s),(list of supported <dest_call_type>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <idx>: ����ֵ������ID��ȡֵ��Χ1~7
     *     <curr_call_type>: ����ֵ����ǰ�������͡�
     *             0�� ��������
     *             1�� ��Ƶͨ������������Ƶ��˫������
     *             2�� ��Ƶͨ�������������Ƶ��˫������
     *             3�� ��Ƶͨ����˫����Ƶ��˫������
     *     <dest_call_type>: ����ֵ��Ŀ��������͡�
     *             0�� ��������
     *             1�� ��Ƶͨ������������Ƶ��˫������
     *             2�� ��Ƶͨ�������������Ƶ��˫������
     *             3�� ��Ƶͨ����˫����Ƶ��˫������
     * [ʾ��]:
     *     �� ���ܶԷ�����Ƶͨ�����ͱ������
     *       AT^CALLMODIFYANS=1,0,3
     *       OK
     *     �� ��������
     *       AT^CALLMODIFYANS=?
     *       ^CALLMODIFYANS: (1-7),(0-3),(0-3)
     *       OK
     */
    { AT_CMD_CALL_MODIFY_ANS,
      AT_SetCallModifyAnsPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CALLMODIFYANS", (VOS_UINT8 *)"(1-7),(0-3),(0-3)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: IMS��Ƶ�绰��������
     * [˵��]: ��������������IMS��Ƶ�绰���������
     * [�﷨]:
     *     [����]: ^IMSVTCAPCFG=<cap_type>,<value>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IMSVTCAPCFG?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^IMSVTCAPCFG=?
     *     [���]: <CR><LF>^IMSVTCAPCFG: (list of supported <cap_type>s), (list of supported<value>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <cap_type>: ����ֵ����Ƶ�绰�������͡�
     *             0����Ƶ�绰��̬����(ChicagoC20��֧��)��ͬʱ����LTE��NR�Ŀ��أ�
     *             1����Ƶ�绰���еȴ�����
     *     <value>: ����ֵ���������ء�
     *             0����
     *             1����
     * [ʾ��]:
     *     �� ����IMS��Ƶ�绰֧�ֺ��еȴ�
     *       AT^IMSVTCAPCFG=1,1
     *       OK
     *     �� ����IMS��Ƶ�绰��֧�ֺ��еȴ�
     *       AT^IMSVTCAPCFG=1,0
     *       OK
     *     �� ���ô�IMS��Ƶ�绰����(Kirin960C20��֧��)
     *       AT^IMSVTCAPCFG=0,1
     *       OK
     */
    { AT_CMD_IMSVTCAPCFG,
      AT_SetImsVtCapCfgPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSVTCAPCFG", (VOS_UINT8 *)"(0,1),(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: VOLTE��������Ƶ������ȡ����Ƶ
     * [˵��]: ����������VOLTE��������Ƶ������ȡ����Ƶ��
     * [�﷨]:
     *     [����]: ^IMSVIDEOCALLCANCEL=<idx>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IMSVIDEOCALLCANCEL=?
     *     [���]: <CR><LF>^IMSVIDEOCALLCANCEL: (1-7)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <idx>: ���б�ʶ����^clcc������idx����һ��  ȡֵ��Χ��1-7��
     * [ʾ��]:
     *     �� ��������
     *       AT^IMSVIDEOCALLCANCEL=?
     *       ^IMSVIDEOCALLCANCEL: (1-7)
     *       OK
     *     �� VOLTE��������Ƶ������ȡ����Ƶ
     *       AT^IMSVIDEOCALLCANCEL=1
     *       OK
     */
    { AT_CMD_IMSVIDEOCALLCANCEL,
      AT_SetImsVideoCallCancelPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IMSVIDEOCALLCANCEL", (VOS_UINT8 *)"(1-7)" },
#endif

    { AT_CMD_CMMI,
      At_SetCmmiPara, AT_SET_SS_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CMMI", (VOS_UINT8 *)"(num)" },
};

/* ע��taf����ҵ��AT����� */
VOS_UINT32 AT_RegisterSsTafCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atSsTafCmdTbl, sizeof(g_atSsTafCmdTbl) / sizeof(g_atSsTafCmdTbl[0]));
}

