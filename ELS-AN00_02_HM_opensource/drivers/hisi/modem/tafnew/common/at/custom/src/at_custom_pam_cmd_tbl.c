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
#include "at_custom_pam_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_pam_set_cmd_proc.h"
#include "at_custom_pam_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_PAM_CMD_TBL_C

static const AT_ParCmdElement g_atCustomPamCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �·�STK��������ִ�н��
     * [˵��]: APִ�������ϱ���Ϣ������󣬵���^CSTR���������ִ�н�����������е����������ҪAPͨ��^CSTR�����ִ�н������
     * [�﷨]:
     *     [����]: ^CSTR=<Len>,<CmdData>
     *     [���]: <CR><LF>^CSTR: <SW1>,<SW2>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^CSTR=?
     *     [���]: <CR><LF>^CSTR: (list of supported <Len>s),(cmd) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Len>: ����ֵ��ָʾ<CmdData>�ĳ��ȣ�ȡֵ��Χ2-512��
     *     <CmdDate>: ʮ�����Ƹ�ʽ����Ϣ�ֶΣ�AP�ظ���Terminal Response�����
     *     <SW1>: �·�Terminal Response���ص�״̬�ֽ�1�������庬��SIM���μ���3GPP 51.011���ġ�9.4 Status conditions returned by the card����USIM���μ���ETSI TS 102.221���ġ�10.2.1Status conditions returned by the UICC����
     *     <SW2>: �·�Terminal Response���ص�״̬�ֽ�2�������庬��SIM���μ���3GPP 51.011���ġ�9.4 Status conditions returned by the card����USIM���μ���ETSI TS 102.221���ġ�10.2.1 Status conditions returned by the UICC����
     * [ʾ��]:
     *     �� �·�����ִ�н�����к���STK�����ϱ�
     *       AT^CSTR=30,"810301240082028281830100100102"
     *       ^CSTR: 145,20
     *       OK
     *     �� �·�����ִ�н������STK�����ϱ�
     *       AT^CSTR=30,"810301240082028281830100100102"
     *       ^CSTR: 144,0
     *       OK
     */
    { AT_CMD_CSTR,
      At_SetCstrPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CSTR", (TAF_UINT8 *)"(2-512),(cmd)" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �ϱ��Ͳ�ѯSTK��������
     * [˵��]: ����������֪ͨAP�����ϱ���һ�����������UE��AP�յ����������Ҹ��ݵ�ǰUE״̬�ж��Ƿ�ִ�и����ִ����Ϻ����^CSTR���������ִ�н�����������е����������ҪAPͨ��^CSTR�����ִ�н��������������AP��ModemЭ��ȷ������ͬʱ��AP����ͨ��^CSTR�����ȡ��SIM���ϱ������һ������������SIM��û���ϱ������������ѯʱ�������������Ϊ0��
     * [�﷨]:
     *     [����]: ^CSIN
     *     [���]: <CR><LF>^CSIN=<Len>[,<CmdType>,<CmdData>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Len>: ����ֵ������ָʾ<CmdData>�ĳ��ȡ�
     *     <CmdType>: ����ֵ(ʮ�������)��ָʾ��ǰSTK�������ͣ���μ���2-21��
     *     <CmdData>: ʮ�����Ƹ�ʽ����Ϣ�ֶΣ���STK���������������ݡ�
     * [��]: STK��������
     *       CmdTypeֵ(ʮ������), STK��������,
     *       '00',                END SESSION,
     *       '01',                REFRESH,
     *       '02',                MORE TIME,
     *       '03',                POLL INTERVAL,
     *       '04',                POLLING OFF,
     *       '05',                SET UP EVENT LIST,
     *       '10',                SET UP CALL,
     *       '11',                SEND SS,
     *       '12',                SEND USSD,
     *       '13',                SEND SHORT MESSAGE,
     *       '14',                SEND DTMF,
     *       '15',                LAUNCH BROWSER,
     *       '20',                PLAY TONE,
     *       '21',                DISPLAY TEXT,
     *       '22',                GET INKEY,
     *       '23',                GET INPUT,
     *       '24',                SELECT ITEM,
     *       '25',                SET UP MENU,
     *       '26',                PROVIDE LOCAL INFORMATION,
     *       '27',                TIMER MANAGEMENT,
     *       '28',                SET UP IDLE MODEL TEXT,
     *       '30',                PERFORM CARD APDU,
     *       '31',                POWER ON CARD,
     *       '32',                POWER OFF CARD,
     *       '33',                GET READER STATUS,
     *       '34',                RUN AT COMMAND,
     *       '35',                LANGUAGE NOTIFICATION,
     *       '40',                OPEN CHANNEL,
     *       '41',                CLOSE CHANNEL,
     *       '42',                RECEIVE DATA,
     *       '43',                SEND DATA,
     *       '44',                GET CHANNEL STATUS,
     *       '45',                SERVICE SEARCH,
     *       '46',                GET SERVICE INFORMATION,
     *       '47',                DECLARE SERVICE,
     *       '50',                SET FRAMES,
     *       '51',                GET FRAMES STATUS,
     *       '60',                RETRIEVE MULTIMEDIA MESSAGE,
     *       '61',                SUBMIT MULTIMEDIA MESSAGE,
     *       '62',                DISPLAY MULTIMEDIA MESSAGE,
     * [ʾ��]:
     *       SIM���ϱ�һ��Select Item����������ݳ���118���ַ�
     *       ^CSIN: 118,36,"D0398103012400820281828F0A01805F53524D53F778018F0A02805207636253F778018F0A03807F168F9153F778018F0A0480547C53EB8F6C79FB"
     *       �˴λỰ�������������������û���µ����������ϱ�
     *       ^CSIN: 0,0
     *     �� ��ѯSIM�����һ���ϱ�������������巵��Refresh����
     *       AT^CSIN
     *       ^CSIN: 22,1,"D009810301010382028182"
     *       OK
     *     �� ��ѯSIM�����һ���ϱ�������������巵������������
     *       AT^CSIN
     *       ^CSIN: 0
     *       OK
     */
    { AT_CMD_CSIN,
      At_SetCsinPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CSIN", VOS_NULL_PTR },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �·�STK Envelope
     * [˵��]: ��������AP��Envelope���������·���MODEM��MODEM�յ���AT�������Envelope��������ݷ��͸��������ҷ����������ݷ��ͽ����
     * [�﷨]:
     *     [����]: ^CSEN=<Len>,<CmdData>
     *     [���]: <CR><LF>^CSTR: <SW1>,<SW2><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CSEN=?
     *     [���]: <CR><LF>^CSEN: (list of supported <Len>s),(cmd) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Len>: ����ֵ��ָʾ<CmdData>�ĳ��ȣ�ȡֵ��Χ2-512��
     *     <CmdDate>: ʮ�����Ƹ�ʽ����Ϣ�ֶΣ�AP�ظ���Envelope�������ݡ�
     *     <SW1>: �·�Terminal Response���ص�״̬�ֽ�1�������庬��SIM���μ���3GPP 51.011���ġ�9.4 Status conditions returned by the card����USIM���μ���ETSI TS 102.221���ġ�10.2.1Status conditions returned by the UICC����
     *     <SW2>: �·�Terminal Response���ص�״̬�ֽ�2�������庬��SIM���μ���3GPP 51.011���ġ�9.4 Status conditions returned by the card����USIM���μ���ETSI TS 102.221���ġ�10.2.1 Status conditions returned by the UICC����
     * [ʾ��]:
     *     �� �·�����ִ�н�����к���STK�����ϱ�
     *       AT^CSEN=18,"D30782020181100101"
     *       ^CSEN: 145,20
     *       OK
     *     �� �·�����ִ�н������STK�����ϱ�
     *       AT^CSEN=18,"D30782020181100101"
     *       ^CSEN: 144,0
     *       OK
     */
    { AT_CMD_CSEN,
      At_SetCsenPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CSEN", (TAF_UINT8 *)"(2-512),(cmd)" },
    { AT_CMD_CSMN,
      At_SetCsmnPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^CSMN", TAF_NULL_PTR },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: Setup Call�������ȷ������
     * [˵��]: ��AP-Modem��̬�£�Modem�ϱ�Setup Call����������û�ͨ��������ȷ���Ƿ�ͬ�ⷢ����С�
     * [�﷨]:
     *     [����]: ^CSTC=<Cmd>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CSTC=?
     *     [���]: <CR><LF>^CSTC: (list of supported <Cmd>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Cmd>: ����ֵ��
     *             0���ܾ�����Setup Call��
     *             1��������Setup Call��
     * [ʾ��]:
     *     �� APͬ�ⷢ�����
     *       AT^CSTC=1
     *       OK
     *     �� ��������
     *       AT^CSTC=?
     *       ^CSTC: (0-1)
     *       OK
     */
    { AT_CMD_CSTC,
      At_SetCstcPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^CSTC", (TAF_UINT8 *)"(0-1)" },

    { AT_CMD_CCIN,
      At_SetCcinPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CCIN", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_CARDATR,
      At_SetCardATRPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^CARDATR", VOS_NULL_PTR },

#if (FEATURE_SECURITY_SHELL == FEATURE_ON)
    { AT_CMD_SPWORD,
      AT_SetSpwordPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SPWORD", VOS_NULL_PTR },
#endif

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: SIM����eSIM�� PASSTHROUGH���µ�����
     * [˵��]: ���������ڸ�SIM����eSIM������PASSTHROUGHģʽ���µ硣ִ��PASSTHROUGH���õ�ǰ���ǿ�Ҫ���µ硣
     *         PASSTHROUGH�ϵ翨��ָModem��������SCI���������ϵ磬�������PPSЭ�̺��ֹͣ��������������USIMMҲ���Կ�����ʼ���������µ翨Ҳ�ǽ�������SCI�����Կ����µ硣
     *         �����ϵ��µ磬ModemЭ�鶼ʼ�մ����޿�״̬���Ҳ�������ģ��㲥��״̬��
     *         ����Modem��AT^HVSST=1,1ָ����˳�PASSTHROUGHģʽ��
     * [�﷨]:
     *     [����]: ^PASSTHROUGH=<state>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: 13<CR><LF>
     *             ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^PASSTHROUGH?
     *     [���]: <CR><LF>^PASSTHROUGH: <state><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^PASSTHROUGH=?
     *     [���]: <CR><LF>^PASSTHROUGH: (list of supported <state>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <state>: ����ֵ��ȡֵ1����2
     *             1���ϵ�
     *             2���µ�
     *             ��ѯʱ��ȡֵ��Χ0~2
     *             0����passthrough״̬��normal̬��
     *             1���ϵ�״̬
     *             2���µ�״̬
     * [ʾ��]:
     *     �� PASSTHROUGH�ϵ�
     *       AT^PASSTHROUGH=1
     *       OK
     *     �� ��ѯPASSTHROUGH״̬
     *       AT^PASSTHROUGH?
     *       ^PASSTHROUGH: 1
     *       OK
     *     �� PASSTHROUGH�µ�
     *       AT^PASSTHROUGH=2
     *       OK
     *     �� ��ѯPASSTHROUGH״̬
     *       AT^PASSTHROUGH?
     *       ^PASSTHROUGH: 2
     *       OK
     */
    { AT_CMD_PASSTHROUGH,
      At_SetPassThrough, AT_SET_PARA_TIME, At_QryPassThrough, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PASSTHROUGH", (VOS_UINT8 *)"(1,2)" },

#if ((FEATURE_SINGLEMODEM_DUALSLOT == FEATURE_ON) && (MULTI_MODEM_NUMBER == 1))
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��modem֧��˫���ۣ�USIM�����뿨�����ù�ϵ����
     * [˵��]: ���������ڵ�modem֧��˫���۳���������USIM�����뿨�����ù�ϵ��
     *         1������USIM�뿨�۶�Ӧ��ϵAT�����谴������˳��ʹ�ã�
     *         ���µ�(HVSST=1,0)->�·�AT����(���ÿ�����USIM�����Ӧ��ϵ)->���ϵ�(HVSST=1,1)��
     *         2��SIM���Ȳ�κ�����2��������ͬʱ���У�
     *         3�������л��������С��10s��
     *         4����ȷ���������λ���������ò�λһ�£�
     * [�﷨]:
     *     [����]: ^SINGLEMODEMDUALSLOT=<slot>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: 13<CR><LF>
     *             ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^SINGLEMODEMDUALSLOT?
     *     [���]: <CR><LF>^SINGLEMODEMDUALSLOT: <slot><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^SINGLEMODEMDUALSLOT=?
     *     [���]: <CR><LF>^SINGLEMODEMDUALSLOT: (0,1)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <slot>: ����ֵ��ȡֵ0����1
     *             0��USIM�����Ӧ����0
     *             1��USIM�����Ӧ����1
     *             ��ѯʱ��ȡֵ��Χ0��1
     *             USIM�뿨�����ù�ϵ
     * [ʾ��]:
     *     �� ����USIM����ƥ�俨��1
     *       AT^SINGLEMODEMDUALSLOT=1
     *       OK
     *     �� ��ѯUSIM����ƥ��Ŀ���
     *       AT^SINGLEMODEMDUALSLOT?
     *       ^SINGLEMODEMDUALSLOT: 1
     *       OK
     */
    { AT_CMD_SINGLEMODEMDUALSLOT,
      At_SetSingleModemDualSlot, AT_SET_PARA_TIME, At_QrySingleModemDualSlot, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SINGLEMODEMDUALSLOT", (VOS_UINT8 *)"(0,1)" },
#endif

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: �л�SIM����������
     * [˵��]: ������ֻ�ڶ�Modem��̬����Ч����������Modem��SIM�����۵Ķ�Ӧ��ϵ��
     *         �������·�ǰ����Ҫ�ѻᷢ�����øı�Ŀ��µ磬�޸���ɺ����ϵ���Ч�µ����á�
     *         ��������Modem����ͬʱ����Ϊͬһ���ۡ�
     * [�﷨]:
     *     [����]: ^SCICHG=<Modem0>,<Modem1>[,<Modem2>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SCICHG?
     *     [���]: <CR><LF>^SCICHG: < Modem0>,<Modem1> [,<Modem2>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^SCICHG=?
     *     [���]: <CR><LF>^SCICHG: (list of supported <Modem0>s),(list of supported <Modem1>s) [,(list of supported <Modem2>s)]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Modem0>: Modem0��ӦSIM���ۣ�����ֵ��ȡֵ��ΧΪ0~2��
     *             0��Modem0��ӦSIM�����л�ΪSIM����0��
     *             1��Modem0��ӦSIM�����л�ΪSIM����1��
     *             2��Modem0��ӦSIM�����л�ΪSIM����2��
     *     <Modem1>: Modem1��ӦSIM���ۣ�����ֵ��ȡֵ��ΧΪ0~2��
     *             0��Modem1��ӦSIM�����л�ΪSIM����0��
     *             1��Modem1��ӦSIM�����л�ΪSIM����1��
     *             2��Modem1��ӦSIM�����л�ΪSIM����2��
     *     <Modem2>: Modem2��ӦSIM���ۣ�����ֵ��ȡֵ��ΧΪ0~2��˫Modem��̬ʱ���˲���ʡ�ԡ�
     *             0��Modem2��ӦSIM�����л�ΪSIM����0��
     *             1��Modem2��ӦSIM�����л�ΪSIM����1��
     *             2��Modem2��ӦSIM�����л�ΪSIM����2��
     * [ʾ��]:
     *     �� ˫Modem��̬������Modem0ʹ��SIM����1��Modem1ʹ��SIM����0
     *       AT^SCICHG=1,0
     *       OK
     *     �� ��Modem��̬������Modem0ʹ��SIM����1��Modem1ʹ��SIM����0��Modem2ʹ��SIM����2
     *       AT^SCICHG=1,0,2
     *       OK
     *     �� ˫Modem��̬����ѯSIM�������ã�Modem0ʹ��SIM����0��Modem1ʹ��SIM����1
     *       AT^SCICHG?
     *       ^SCICHG: 0,1
     *       OK
     *     �� ��Modem��̬����ѯSIM�������ã�Modem0ʹ��SIM����2��Modem1ʹ��SIM����0��Modem2ʹ��SIM����1
     *       AT^SCICHG?
     *       ^SCICHG: 2,0,1
     *       OK
     *     �� ˫Modem��̬����������
     *       AT^SCICHG=?
     *       ^SCICHG: (0-2),(0-2)
     *       OK
     *     �� ��Modem��̬����������
     *       AT^SCICHG=?
     *       ^SCICHG: (0-2),(0-2),(0-2)
     *       OK
     */
    { AT_CMD_SCICHG,
      At_SetSciChgPara, AT_SET_PARA_TIME, At_QrySciChgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SCICHG", (VOS_UINT8 *)SCICHG_CMD_PARA_STRING },

#if (FEATURE_VSIM == FEATURE_ON)
#if (FEATURE_VSIM_ICC_SEC_CHANNEL == FEATURE_ON)
    { AT_CMD_ICCVSIMVER,
      VOS_NULL_PTR, AT_SET_PARA_TIME, At_QryIccVsimVer, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ICCVSIMVER", VOS_NULL_PTR },

    { AT_CMD_HVCHECKCARD,
      VOS_NULL_PTR, AT_SET_PARA_TIME, At_QryHvCheckCardPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HVCHECKCARD", VOS_NULL_PTR },

#endif
#endif /* end (FEATURE_VSIM == FEATURE_ON) */

#if ((FEATURE_PHONE_SC == FEATURE_ON) && (FEATURE_PHONE_USIM == FEATURE_ON))
    { AT_CMD_SILENTPIN,
      At_SetSilentPin, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SILENTPIN", (VOS_UINT8 *)"(@encryptpin),(@iv),(@hmac)" },

    { AT_CMD_SILENTPININFO,
      At_SetSilentPinInfo, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SILENTPININFO", (VOS_UINT8 *)"(@pin)" },
#endif

    { AT_CMD_PRIVATECGLA,
      At_SetPrivateCglaPara, AT_SET_CGLA_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^CGLA", (TAF_UINT8 *)"(0-4294967295),(8-522),(command)" },

    { AT_CMD_CARDVOLTAGE,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryCardVoltagePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CARDVOLTAGE", VOS_NULL_PTR },

    { AT_CMD_IMSICHG,
      At_SetImsichgPara, AT_SET_PARA_TIME, At_QryImsichgPara, AT_QRY_PARA_TIME, AT_TestImsichgPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (TAF_UINT8 *)"^IMSICHG", (TAF_UINT8 *)"(0-1),(0-1)" },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    { AT_CMD_MEMQUERY,
      AT_SetMemInfoPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
      (TAF_UINT8 *)"^MEMQUERY", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_APSIMST,
      AT_SetApSimStPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestApSimStPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^APSIMST", (VOS_UINT8 *)"(0,1)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: Modem������λ״̬֪ͨ
     * [˵��]: ��AT��������AP��Balong Modem֪ͨ����ƴƬModem�����˸�λ��
     * [�﷨]:
     *     [����]: ^MODEMSTATUS=<modem>,<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^MODEMSTATUS?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^MODEMSTATUS=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <modem>: �����ͣ���ǰ��λ��Modem��š�
     *             0��Balong Modem
     *             1��VIA Modem
     *             ����������չ
     *     <mode>: �����ͣ���ǰ�ĸ�λ״̬��
     *             0����ʼ��λ
     *             1����λ�ɹ�����
     *             2����λʧ��
     * [ʾ��]:
     *     �� VIA��ʼ��λ֪ͨ
     *       AT^MODEMSTATUS=1,0
     *       OK
     */
    { AT_CMD_MODEMSTATUS,
      AT_SetModemStatusPara, AT_NOT_SET_TIME, AT_QryModemStatusPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^MODEMSTATUS", (VOS_UINT8 *)"(0,1),(0-2)" },

#if ((FEATURE_PHONE_USIM == FEATURE_OFF) && (MULTI_MODEM_NUMBER == 1))
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ����Ӧ�õ�AID
     * [˵��]: ��������������UICC����USIM/ISIM/CSIMӦ�õ�AID�����޸�NV4085��
     *         ��ע�����������������Բ�Ʒ�����������USIMӦ��AID���������ṩ����Ʒ���Ա��޸�USIM/ISIM/CSIM��
     * [�﷨]:
     *     [����]: ^CARDAPPAIDSET=<enable>,<appType>,<dfNameLength>,<dfName>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <enable>: NV4085�Ƿ�ʹ�ܡ�0��Ĭ��ֵ����ʹ�ܡ�1��ʹ�ܡ�
     *     <appType>: ��Ҫ���õ�Ӧ�õ����͡�ȡֵ��Χ��0-2.
     *             0��USIM
     *             1��ISIM
     *             2��CSIM
     *     <dfNameLength>: ����ֵ������Ӧ�����Ƶĳ��ȡ����ܳ���16��
     *     <dfName>: ����ֵ����Ҫ���õ�Ӧ��AIDֵ������1~16�ֽڡ�
     * [ʾ��]:
     *       AT^CARDAPPAIDSET=1,0,16,"A0000000871002FF86FF0389FFFFFFFF"
     *       OK
     */
    { AT_CMD_CARDAPPAIDSET,
      AT_SetCardAppAidPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CARDAPPAIDSET", (VOS_UINT8 *)"(0,1),(0-2),(1-16),(dfname)" },
#endif
};

/* ע��PAM����AT����� */
VOS_UINT32 AT_RegisterCustomPamCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomPamCmdTbl, sizeof(g_atCustomPamCmdTbl) / sizeof(g_atCustomPamCmdTbl[0]));
}

