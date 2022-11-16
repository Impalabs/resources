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
#include "at_voice_ims_cmd_tbl.h"
#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_voice_ims_set_cmd_proc.h"
#include "at_voice_ims_qry_cmd_proc.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_VOICE_IMS_CMD_TBL_C

static const AT_ParCmdElement g_atVoiceImsCmdTbl[] = {
#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ����IMSע��������ȼ�
     * [˵��]: ����IMSע��������ȼ���IMS���ն�Ӧ�����ȼ�˳����ע�����̡�
     * [�﷨]:
     *     [����]: ^IMSDOMAINCFG=<domain_cfg>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IMSDOMAINCFG?
     *     [���]: <CR><LF>^IMSDOMAINCFG: <domain_cfg><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^IMSDOMAINCFG=?
     *     [���]: <CR><LF>^IMSDOMAINCFG: (list of supported <domain_cfg>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <domain_cfg>: ����ֵ��IMSע��������ȼ���
     *             0��WIFI���ȣ�ע��˳��ΪWIFI->LTE/NR->GU��
     *             1��LTE/NR���ȣ�ע��˳��ΪLTE/NR->WIFI->GU��
     *             2����WIFI������WIFI����ע�����̣�
     *             3��WIFI�����ȼ���Modem�޷���ʱѡ��WIFI����ע�����̡�
     *             4����Чֵ������APû�е���^IMSDOMAINCFG��������IMSע��������ʱ����ѯ^IMSDOMAINCFG����ʱ�᷵�ظ�ֵ������������ֵ��Ч����IMSע��������������Ϊ4������APû�е���^IMSDOMAINCFG��������IMSע��������ʱ��IMS����Cellular�ϳ���ע�᣻
     * [ʾ��]:
     *     �� ����IMSע��������ȼ�ΪWIFI ����
     *       AT^IMSDOMAINCFG=0
     *       OK
     *     �� ��ѯIMSע��������ȼ�
     *       AT^IMSDOMAINCFG?
     *       ^IMSDOMAINCFG: 0
     *       OK
     *     �� ����IMSע��������ȼ�
     *       AT^IMSDOMAINCFG=?
     *       ^IMSDOMAINCFG: (0-3)
     *       OK
     */
    { AT_CMD_IMSDOMAINCFG,
      AT_SetImsDomainCfgPara, AT_SET_PARA_TIME, AT_QryImsDomainCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSDOMAINCFG", (VOS_UINT8 *)"(0-3)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ѯ��ǰIMSע����
     * [˵��]: ��ѯ��ǰIMSע����
     * [�﷨]:
     *     [����]: ^IMSREGDOMAIN?
     *     [���]: <CR><LF>^IMSREGDOMAIN: <domain><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <domain>: ����ֵ��IMSע����
     *             0�� LTE
     *             1�� WIFI
     *             2�� Utran
     *             3�� Gsm
     *             4�� NR
     *             255�� UNKNOWN
     * [ʾ��]:
     *     �� ��ѯ��ǰIMSע����
     *       AT^IMSREGDOMAIN?
     *       ^IMSREGDOMAIN: 0
     *       OK
     */
    { AT_CMD_IMSREGDOMAIN,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryImsRegDomainPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSREGDOMAIN", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ת��MAPCON��IMSA����Ϣ
     * [˵��]: ת��MAPCON��IMSA����Ϣ��
     * [�﷨]:
     *     [����]: ^IMSCTRLMSG=<msg_id>,<msg_len>,<msg_context>[,<msg_context>,��]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^IMSCTRLMSG=?
     *     [���]: <CR><LF>^IMSCTRLMSG: (list of supported <msg_id>s) (list of supported < msg_len >s)<msg><msg><msg><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <msg_id>: ��Ӧ����ģ��
     *             0��IMSA
     *             1��MAPCONN
     *     <msg_len>: ��Ϣ����<msg_context>�ĳ���
     *     <msg_context>: ģ���ӿ���Ϣ����
     * [ʾ��]:
     *     �� ת��MAPCON��IMSA����Ϣ
     *       AT^IMSCTRLMSG=0,4,"12345678"
     *       OK
     *     �� ���Ա�����
     *       AT^IMSCTRLMSG=?
     *       ^IMSCTRLMSG: (0,1),(1-600),(msg),(msg),(msg)
     *       OK
     */
    { AT_CMD_IMSCTRLMSG,
      AT_SetImsctrlmsgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSCTRLMSG", (VOS_UINT8 *)"(0,1),(1-600),(msg),(msg),(msg)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ѯIMS����������
     * [˵��]: ���������ڲ�ѯIMS���IMPU��Ϣ�Լ�������롣
     * [�﷨]:
     *     [����]: ^VOLTEIMPU
     *     [���]: <CR><LF>^VOLTEIMPU: < IMPU>[,<Virtual_Number >]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^VOLTEIMPU?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^VOLTEIMPU=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <IMPU>: �ַ������ͣ�IMPU��Ϣ��ȡֵ��Χ0-128bytes
     *     <Virtual_Number>: �ַ������ͣ�������������绰���룬��󳤶�Ϊ127 bytes
     * [ʾ��]:
     *     �� ����û�з�������绰����
     *       AT^VOLTEIMPU
     *       ^VOLTEIMPU: sip:01022331621@lte-lguplus.co.kr
     *       OK
     *     �� �����������绰����01023151463
     *       AT^VOLTEIMPU
     *       ^VOLTEIMPU: sip:01022331621@lte-lguplus.co.kr,01023151463
     *       OK
     */
    { AT_CMD_VOLTEIMPU,
      AT_SetVolteimpuPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^VOLTEIMPU", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ���úͲ�ѯIMS���ǳ���Ϣ
     * [˵��]: �������������úͲ�ѯIMS����ǳ���Ϣ��
     * [�﷨]:
     *     [����]: ^NICKNAME=<Nick_Name>
     *     [���]: ������ȷ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NICKNAME?
     *     [���]: ��ѯ�ɹ���
     *             <CR><LF>^NICKNAME: <Nick_Name><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NICKNAME=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <Nick_Name>: UTF8�����ʽ����󳤶�Ϊ256���ַ�
     * [ʾ��]:
     *       �����ǳƣ��й�
     *       ���С���Ӧ��UTF8����ΪE4 B8 AD
     *     �� ��������Ӧ��UTF8����ΪE5 9B BD
     *       AT^NICKNAME=E4B8ADE59BBD
     *       OK
     *     �� ��ѯ�ǳƽ��Ϊ���й�
     *       AT^NICKNAME?
     *       ^NICKNAME: E4B8ADE59BBD
     *       OK
     *     �� ��������
     *       AT^NICKNAME=?
     *       OK
     */
    { AT_CMD_NICKNAME,
      AT_SetNickNamePara, AT_SET_PARA_TIME, AT_QryNickNamePara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NICKNAME", (VOS_UINT8 *)"(@nickname)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ����RTT���ܿ���
     * [˵��]: ���������ڽ��û����õ�RTT���ܿ������ø�IMS������RTT���ܿ���ʱ�Զ��ܽ���Ƶ���硣
     * [�﷨]:
     *     [����]: ^RTTCFG=<enable>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^RTTCFG=?
     *     [���]: <CR><LF>^RTTCFG: (list of supported <enable>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ���û�����RTT���ܿ��أ�
     *             0��RTT���ܹرգ�
     *             1��RTT���ܿ�����
     * [ʾ��]:
     *     �� ���ô�RTT���ܿ���
     *       AT^RTTCFG=1
     *       OK
     *     �� ��������
     *       AT^RTTCFG=?
     *       ^RTTCFG: (0,1)
     *       OK
     */
    { AT_CMD_RTTCFG,
      AT_SetRttCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^RTTCFG", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �л�RTTͨ��
     * [˵��]: ���������ڱ�����������RTTͨ���л������Ӧ��/�ܾ�Զ�˷����RTTͨ���л�����
     * [�﷨]:
     *     [����]: ^RTTMODIFY=<call_id>,<operation>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^RTTMODIFY=?
     *     [���]: <CR><LF>^RTTMODIFY: (list of supported <call_id>s),( list of supported <operation>s)<CR><LF>  <CR><LF>OK<CR><LF>
     * [����]:
     *     <call_id>: ����ֵ�����б�ʶ��ȡֵ��Χ1~7��
     *     <operation>: ����ֵ��RTTͨ���л��������ͣ�
     *             0��Add Text����
     *             1��Close Text����
     *             2��ͬ��Add Text����
     *             3���ܾ�Add Text�����ݲ�֧�֣���
     * [ʾ��]:
     *     �� ���˷�������RTTͨ������
     *       AT^RTTMODIFY=1,0
     *       OK
     *     �� ��������
     *       AT^RTTMODIFY=?
     *       ^RTTMODIFY: (1-7),(0-3)
     *       OK
     */
    { AT_CMD_RTTMODIFY,
      AT_SetRttModifyPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^RTTMODIFY", (VOS_UINT8 *)"(1-7),(0-3)" },
#endif

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ����IMSЭ��ջ����
     * [˵��]: ��������������IMSЭ��ջIP���������ء�
     *         Ŀǰ����֧��IPSEC��KeepAlive�������á�
     * [�﷨]:
     *     [����]: ^IMSIPCAPCFG=[<IPSec>],[<KeepAlive>]
     *     [���]: ��ȷ����ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IMSIPCAPCFG?
     *     [���]: <CR><LF>^IMSIPCAPCFG:  <IPSec>,<KeepAlive>  <CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^IMSIPCAPCFG=?
     *     [���]: <CR><LF>^IMSIPCAPCFG:  (list of supported <IPSec>s),(list of supported <KeepAlive>s)<CR><LF>  <CR><LF>OK<CR><LF>
     * [����]:
     *     <IPSec>: ����ֵ��IPSEC�������ã�
     *             0������IPSEC������
     *             1���ر�IPSEC������
     *             ���������ȱ��������ԭ�����������á�
     *     <KeepAlive>: ����ֵ��KeepAlive�������ã�
     *             0���ر�KeepAlive������
     *             1������KeepAlive������
     *             ���������ȱ��������ԭ�����������á�
     * [ʾ��]:
     *     �� ����IMSЭ��ջ����������IPSEC�������ر�KeepAlive����
     *       AT^IMSIPCAPCFG=0,0
     *       OK
     *     �� ��ѯIMSЭ��ջ��������
     *       AT^IMSIPCAPCFG?
     *       ^IMSIPCAPCFG: 0,0
     *       OK
     *     �� ��������
     *       AT^IMSIPCAPCFG=?
     *       ^IMSIPCAPCFG: (0,1),(0,1)
     *       OK
     */
    { AT_CMD_IMSIPCAPCFG,
      AT_SetImsIpCapPara, AT_SET_PARA_TIME, AT_QryImsIpCapPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IMSIPCAPCFG", (VOS_UINT8 *)"(0,1),(0,1)" },
};

/* ע��taf����AT����� */
VOS_UINT32 AT_RegisterVoiceImsCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atVoiceImsCmdTbl, sizeof(g_atVoiceImsCmdTbl) / sizeof(g_atVoiceImsCmdTbl[0]));
}

