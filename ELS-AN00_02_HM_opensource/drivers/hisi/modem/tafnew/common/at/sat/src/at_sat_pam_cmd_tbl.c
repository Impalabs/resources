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
#include "at_sat_pam_cmd_tbl.h"
#include "AtParse.h"
#include "at_parse_cmd.h"
#include "at_sat_pam_set_cmd_proc.h"
#include "at_sat_pam_qry_cmd_proc.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_SAT_PAM_CMD_TBL_C
/*
 * [���]: Э��AT-(U)SATҵ��
 * [����]: (U)SAT�����������˵��
 * [��]: (U)SAT�����������˵��
 *        ����,                    ˵��,
 *        <CmdType>,               0��SIM���ѷ�����һ����Setup Menu���������
 *                                 1��SIM���ѷ�����һ����Display Text���������
 *                                 2��SIM���ѷ�����һ����Get Inkey���������
 *                                 3��SIM���ѷ�����һ����Get Input���������
 *                                 4��SIM���ѷ�����һ����Setup Call���������
 *                                 5��SIM���ѷ�����һ����Play Tone���������
 *                                 6��SIM���ѷ�����һ����Sel Item���������
 *                                 7��SIM���ѷ�����һ����Refresh���������
 *                                 8��SIM���ѷ�����һ����Send SS���������
 *                                 9��SIM���ѷ�����һ����Send SMS���������
 *                                 10��SIM���ѷ�����һ����Send USSD���������
 *                                 11��SIM���ѷ�����һ����LAUNCH BROWSER���������
 *                                 12��SIM���Ѿ�������һ����SET UP IDLE MODE TEXT���������
 *                                 99��SIM���ѷ�����һ����End Session���������Ự����,
 *        <CmdIndex>,              ȡֵ��ΧΪ0��8����ʾ���������ڵ����е�λ�á�,
 *        <isTimeOut>,             ȡֵ��ΧΪ0��1����ʾ�����������Ƿ�ʱ��
 *                                 0������ʱ��
 *                                 1����ʱ��,
 *        <Alpha Identifier menu>, ���˵���Alpha Identifier�������⡣
 *                                 ���뷽ʽΪ���жϵ�һ���ֽ��Ƿ���80��81��82������ǣ��Ͱ���UCS2��ʽ���룻�����շ�ѹ����GSM7λ���뷽ʽ���롣,
 *        <Id>,                    (0)����ʾ���˵���,
 *        <Idx>,                   xȡֵ��ΧΪ0��255���˵����Identifier��,
 *        <NbItems>,               ȡֵ��ΧΪ0��255�����˵��е�ѡ�������,
 *        <Alpha Idx Label>,       ѡ���Alpha Identifier label���˵�ѡ�����ơ�
 *                                 ���뷽ʽΪ���жϵ�һ���ֽ��Ƿ���80��81��82������ǣ��Ͱ���UCS2��ʽ���룻�����շ�ѹ����GSM7λ���뷽ʽ���롣
 *                                 <HelpInfo>, ȡֵ��ΧΪ0��1��
 *                                 0��������Ϣ�����ã�
 *                                 1��������Ϣ���á�,
 *        <NextActionId>,          ����һ����������Identifier��TA�ܿ������յ�^STIN:0��֪֮ͨ���ȡsetup menu���й���Ϣ��,
 *        <TextInfo>,              �ַ������ͣ�Ҫ��ʾ���ı���Ϣ��,
 *        <TextFormat>,            Ҫ��ʾ���ı��ı��뷽ʽ��
 *                                 0��GSM7λѹ�����룻
 *                                 4��8bit���룻
 *                                 8��UCS2���롣,
 *        <ClearMode>,             �û��ر���ʾģʽ��
 *                                 0����һ���ӳ�֮������ʾ���ı���Ϣ��
 *                                 1���ȴ��û������ʾ���ı���Ϣ��,
 *        <DurationTime>,          ��ʾ�ı�������ʾ�ĳ���ʱ�䣬��λΪ�롣,
 *        <rspFormat>,             �û�����ķ�ʽ���������ݵ��ַ����͡�
 *                                 0��GSM7���룻
 *                                 1��YES NOģʽ��
 *                                 2�����֣�0��9��*��#��and+����
 *                                 3��UCS2��,
 *        <Timeout>,               ��ʱʱ�䣬��λΪ�롣,
 *        <PackMode>,              ȡֵ��Χ0��1����ʾ��ǰ�Ƿ�Ϊѹ��ģʽ��
 *                                 0��δѹ��ģʽ��
 *                                 1��ѹ��ģʽ��,
 *        <EchoMode>,              ȡֵ��Χ0��1����ʾ��ǰ�Ƿ�Ϊ����ģʽ��
 *                                 0���رջ��ԣ�
 *                                 1���򿪻��ԡ�,
 *        <SizeMin>,               ȡֵ��ΧΪ1��255����С���볤�ȡ�,
 *        <SizeMax>,               ȡֵ��ΧΪ1��255��������볤�ȡ�,
 *        <DefaultTextInfo>,       �ı���Ϣ��Ĭ���û������ַ�����,
 *        <ToneType>,              Tone�����͡�
 *                                 0��Tone Dial��
 *                                 1��Tone Busy��
 *                                 2��Tone Conestion��
 *                                 3��Tone Radio ack��
 *                                 4��Tone Dropped��
 *                                 5��Tone Error��
 *                                 6��Tone Call waitting��
 *                                 7��Tone Ringing��
 *                                 8��Tone General beep��
 *                                 9��Tone Positive beep��
 *                                 10��Tone Negative beep��
 *                                 ��û��ָ��toneʱ��ME��ʹ��Ĭ�ϵ�general beep��,
 *        <textCode>,              ͬ<TextFormat>��,
 *        <icon>,                  ͼ����Ϣ��,
 *        <DefaultItem>,           ȡֵ��ΧΪ1��255��Ĭ��Item Identifier��,
 *        <Refresh Type>,          ˢ�����͡�
 *                                 0��NAA��ʼ����
 *                                 1��NAA�ļ��仯֪ͨ��FCN����
 *                                 2��NAA��ʼ�����ļ��仯֪ͨ��FCN����
 *                                 3��NAA��ʼ����ȫ���ļ��仯֪ͨ��FCN����
 *                                 4��UICC������
 *                                 5��NAAӦ��������
 *                                 6��NAA�Ự������,
 *        <File List>,             ��Ҫ���µ��ļ��б�,
 *        <URL>,                   Uniform Resource Location��,
 *        <LaunchMode>,            ����ģʽ��
 *                                 0�����û������������������������
 *                                 2��ʹ���Ѿ����ڵ����������ʹ���Ѿ�����Ự�����������
 *                                 3���ر��Ѿ����ڵ�������������������,
 */

static const AT_ParCmdElement g_atSatPamCmdTbl[] = {
    /* USAT */
    /*
     * [���]: Э��AT-(U)SATҵ��
     * [����]: ����STK
     * [˵��]: ����������������SIM��Э��֧�ֵ�STK���������Ϣд�����NV�����һ�ο�����ʱ����Ч��
     * [�﷨]:
     *     [����]: ^STSF=<Mode>[,<Config>][,<Timeout>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^STSF?
     *     [���]: <CR><LF>^STSF: <Mode>, <Config>, <Timeout><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^STSF=?
     *     [���]: <CR><LF>^STSF: (0-2), (160060c0-5ffffff7), (1-255) <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����STK���ܡ�
     *             0��ʹSTK������Ч��
     *             1������STK���ܣ�
     *             2������STK���ܣ��ݲ�֧�֣���
     *     <Config>: 16�����ַ������ͣ��ֻ�������Ϣ����μ���ETSI TS 102 223���ġ�5 Profile download����
     *     <Timeout>: �������������û���Ӧ���������ʱ�䣨�ݲ�֧�֣���
     * [ʾ��]:
     *     �� ����ʹSTK������Ч
     *       AT^STSF=0
     *       OK
     *     �� �﷨����
     *       AT^STSF=3
     *       ��CME ERROR: 50
     *     �� ����STK����
     *       AT^STSF=1
     *       OK
     */
    { AT_CMD_STSF,
      At_SetStsfPara, AT_SET_PARA_TIME, At_QryStsfPara, AT_QRY_PARA_TIME, AT_TestStsfPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^STSF", (TAF_UINT8 *)"(0-2),(cmd),(1-255)" },

#if ((FEATURE_PHONE_USIM == FEATURE_OFF) || (defined(DMT)) || (defined(__PC_UT__)))
    /*
     * [���]: Э��AT-(U)SATҵ��
     * [����]: ��ȡ��Ϣ��������
     * [˵��]: ������������ȡ�����������ݣ�TE�յ���������֪ͨ��^STIN������ִ�������ȡSIM���͵������������Ϣ����ʾ�ı����˵���Ϣ�����Եȣ���
     *         ��ѯ����ص�ǰSIM���ϱ��������������ͺ�������
     *         ���������֧�ֵ�STK���
     *         ���ڻ�ȡ���˵���Ϣ��TE�������κ�ʱ����AT^STGI=0,0��ȡ���˵���������������Ϣ���������ϱ�������²��ܻ�ȡ��
     *         �������֧����MBBƽ̨ʹ�á�
     * [�﷨]:
     *     [����]: ^STGI=<CmdType>,<CmdIndex>
     *     [���]: ��STGI�������и���������ϸ������
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^STGI=?
     *     [���]: [<CR><LF>^STGI: (list of supported <CmdType>)<CR><LF>]
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *       ��������, �����ʽ,
     *       0,        ^STGI: <Id>, <NbItems>, <Alpha Identifier menu><CR><LF>^STGI: <Id1>, <NbItems>, <Alpha Id1 Label>, <Help Info> [,<NextActionId>] <CR><LF>^STGI: <Id2>, <Nbltems>, <Alpha Id2 Label>, <Help Info> [,<NextActionId>]<CR><LF>[��],
     *       1,        ^STGI:<TextInfo>, <TextFormat>, <ClearMode>[,<DurationTime>],
     *       2,        ^STGI: <TextInfo>, <textCode>, <rspFormat>, <HelpInfo> [,<Timeout> [,Icon]],
     *       3,        ^STGI: <TextInfo>,<textCode>,<rspFormat>, <PackMode>, <EchoMode>, <SizeMin>, <SizeMax>, <HelpInfo> [,<DefaultTextInfo>,<textCode>],
     *       5,        ^STGI:<ToneType>[,<Duration>,<TextInfo>,<textCode>,<icon>],
     *       6,        ^STGI: <DefaultItem>, <NbItems>, <Alpha Identifier menu><CR><LF>
^STGI: <Id1>, <NbItems>, <Alpha Id1 Label>, <Help Info> [,<NextActionId>]<CR><LF>
^STGI: <Id2>, <NbItems>, <Alpha Id2 Label>, <Help Info> [,<NextActionId>]<CR><LF>
[��],
     *       7,        ^STGI:<Refresh Type>[,<File List>],
     *       11,       ^STGI:<URL>,<LaunchMode>,
     *       12,       ^STGI:<TextInfo>,<textCode>[,<icon>],
     * [ʾ��]:
     *       �����й��ܶ������һ��ʼ����Ҫ��PINУ�飬����STK���ܱ����
     *       ^STIN:0,2                              SIM�Ѿ��������˵�
     *     �� ��ù������˵�����Ϣ
     *       AT^STGI=0,2
     *       ^STGI: "SIM TOOKIT MAIN MENU"          ���˵���������
     *       ^STGI: 1,3,"BANK", 0
     *       ^STGI: 2,3,"QUIZ", 0
     *       ^STGI: 3,3,"WEATHER",0
     *       OK
     */
    { AT_CMD_STGI,
      At_SetStgiPara, AT_SET_PARA_TIME, At_QryStgiPara, AT_QRY_PARA_TIME, AT_TestStgiPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^STGI", (TAF_UINT8 *)"(0-12)" },

    /*
     * [���]: Э��AT-(U)SATҵ��
     * [����]: STK������Ӧ
     * [˵��]: ������������TEִ����������Ľ������SIM����
     *         ��ѯ����ص�ǰ���ڵȴ���̨��Ӧ�������������ͺ�������
     *         ��������ص�ǰ֧�ֵ������������ͺ�������
     *         �������֧����MBBƽ̨ʹ�á�
     * [�﷨]:
     *     [����]: ^STGR=<CmdType>,<CmdIndex>[,<Result>[,<Data>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^STGR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <CmdType>: ��μ�2.9.1 (U)SAT�����������˵����
     *     <CmdIndex>: ��μ�2.9.1 (U)SAT�����������˵����
     *     <Result>: 0���û���ֹ�Ự��
     *             1��ִ���û����ܶ�����
     *             2���û�Ҫ��İ�����Ϣ��
     *             3��������һ��˵���
     *             4����ʾME��֧�֣�
     *             5����ʾME�ϵ������æ�򲻿ɼ���
     *             6����ʾME��æ��SS���䡣
     *     <Data>: ���<Result>�ǡ�1��ִ���û����ܶ���������ڣ�������μ���2-17��
     * [��]: Data�ֶ�����
     *       ��������, Data����,
     *       0,        ѡ�в˵���<Id>�š�,
     *       1,        �ޡ�,
     *       2,        <textCode>���û��������ݡ�,
     *       3,        <textCode>���û��������ݡ�,
     *       4,        �ޡ�,
     *       5,        �ޡ�,
     *       6,        ѡ�в˵���<Id>�š�,
     *       7,        �ޡ�,
     *       8,        �ޡ�,
     *       9,        �ޡ�,
     *       10,       �ޡ�,
     *       11,       �ޡ�,
     *       12,       �ޡ�,
     * [ʾ��]:
     *       �����й��ܶ������һ��ʼ����Ҫ��PINУ�飬����STK���ܱ����
     *       ^STIN: 0,3                             SIM�Ѿ��������˵�
     *     �� ��ù������˵�����Ϣ
     *       AT^STGI=0,4
     *       ^STGI: 1,3,"BANK", 0                  ���˵���������
     *       ^STGI: 2,3,"QUIZ", 0
     *       ^STGI: 3,3,"WEATHER",0
     *       OK
     *     �� ���˵��е�item2��ѡ��
     *       AT^STGR=0,4,1,2
     *       OK
     *       ^STIN: 6,1                             SIM�ѷ���Sel item�˵�
     *     �� ��ȡBANK�˵�����Ϣ
     *       AT^STGI=6,1
     *       ^STGI:0, "BANK"                        BANK�˵�����2��ѡ��
     *       ^STGI: 1,2, "PERSONAL ACCOUNT ENQUIRY", 1
     *       ^STGI: 2,2, "NEWS", 0
     *       OK
     *     �� ѡ��item2
     *       AT^STGR=6,1,1,2
     *       OK
     */
    { AT_CMD_STGR,
      At_SetStgrPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, AT_TestStgrPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^STGR", (TAF_UINT8 *)"(0-12),(0-8),(0-6)" },
#endif

    /*
     * [���]: Э��AT-(U)SATҵ��
     * [����]: �л�SIM����������
     * [˵��]: ������ֻ�ڶ�Modem��̬����Ч����������Modem��SIM�����۵Ķ�Ӧ��ϵ���������·�����Ҫ����Modem����Ч��
     *         ��������Modem����ͬʱ����Ϊͬһ���ۡ�
     * [�﷨]:
     *     [����]: ^SIMSLOT=<Modem0>,<Modem1>[,<Modem2>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SIMSLOT?
     *     [���]: <CR><LF>^SIMSLOT: < Modem0>,<Modem1> [,<Modem2>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^SIMSLOT=?
     *     [���]: <CR><LF>^SIMSLOT: (list of supported <Modem0>s),(list of supported <Modem1>s) [,(list of supported <Modem2>s)]<CR><LF>
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
     *       AT^SIMSLOT=1,0
     *       OK
     *     �� ��Modem��̬������Modem0ʹ��SIM����1��Modem1ʹ��SIM����0��Modem2ʹ��SIM����2
     *       AT^SIMSLOT=1,0,2
     *       OK
     *     �� ˫Modem��̬����ѯSIM�������ã�Modem0ʹ��SIM����0��Modem1ʹ��SIM����1
     *       AT^SIMSLOT?
     *       ^SIMSLOT: 0,1
     *       OK
     *     �� ��Modem��̬����ѯSIM�������ã�Modem0ʹ��SIM����2��Modem1ʹ��SIM����0��Modem2ʹ��SIM����1
     *       AT^SIMSLOT?
     *       ^SIMSLOT: 2,0,1
     *       OK
     *     �� ˫Modem��̬����������
     *       AT^SIMSLOT=?
     *       ^SIMSLOT: (0-2),(0-2)
     *       OK
     *     �� ��Modem��̬����������
     *       AT^SIMSLOT=?
     *       ^SIMSLOT: (0-2),(0-2),(0-2)
     *       OK
     */
    { AT_CMD_SIMSLOT,
      At_SetSIMSlotPara, AT_NOT_SET_TIME, At_QrySIMSlotPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMSLOT", (VOS_UINT8 *)SCICHG_CMD_PARA_STRING },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: Э��AT-(U)SATҵ��
     * [����]: ָʾSIM���Ƴ������
     * [˵��]: ����������APָʾSIM���Ƴ�����룬AP��⵽SIM���Ƴ������ʱ����ͨ��������֪ͨmodem��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^SIMINSERT=<n>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^SIMINSERT=?
     *     [���]: <CR><LF>^SIMINSERT: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: SIM��״̬������ֵ��ȡֵ��ΧΪ0~1��
     *             0��SIM���Ƴ�
     *             1��SIM������
     * [ʾ��]:
     *     �� ָ��SIM���Ƴ�
     *       AT^SIMINSERT=0
     *       OK
     *     �� ָ��SIM������
     *       AT^SIMINSERT=1
     *       OK
     *     �� ��������
     *       AT^SIMINSERT=?
     *       ^SIMINSERT: (0,1)
     *       OK
     */
    { AT_CMD_SIMINSERT,
      At_SetSimInsertPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMINSERT", (VOS_UINT8 *)"(0,1)" },
#endif
};

/* ע��sat pam AT����� */
VOS_UINT32 AT_RegisterSatPamCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atSatPamCmdTbl, sizeof(g_atSatPamCmdTbl) / sizeof(g_atSatPamCmdTbl[0]));
}

