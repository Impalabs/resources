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
#include "at_sms_taf_cmd_tbl.h"
#include "AtParse.h"
#include "at_parse_cmd.h"
#include "at_sms_taf_set_cmd_proc.h"
#include "at_sms_taf_qry_cmd_proc.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"



/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_SMS_TAF_CMD_TBL_C

/* SMS����� */

/* �����ϱ�����������úͲ�ѯʱ��Ҫ�ȴ��ظ�����ӵȴ����ûظ�ʱ��͵ȴ���ѯ�ظ�ʱ�� */
/*
 * [���]: Э��AT-����ҵ��
 * [����]: ���Ų����������˵��
 * [˵��]: ������ز�����ο�3GPP TS 23.038��3GPP TS 23.040��3GPP TS 24.011��Э�顣
 * [��]�����Ŵ洢��ز���˵��
 *       ����,     ˵��
 *       <index>,  ����ֵ�������ڴ洢���е�����ֵ��
 *                 �洢��ΪFlashʱ��ȡֵ��Χȡ����ME��������ME������NV��ƣ����֧��500��������0��ʼȡֵ��
 *                 �洢��Ϊ(U)SIMʱ��ȡֵ��Χȡ����(U)SIM����������0��ʼȡֵ��,
 *       <mem>,    �ַ������ͣ����Ų����洢����
 *                 SM��SIM���洢��
 *                 ME��Flash�洢��,
 *       <mem1>,   �ַ������ͣ����Ŷ�ȡ��ɾ���洢����ȡֵ��Χͬ<mem>��,
 *       <mem2>,   �ַ������ͣ����ŷ��ͺ�д�洢����ȡֵ��Χͬ<mem>��,
 *       <mem3>,   �ַ������ͣ����Ž��մ洢����ȡֵ��Χͬ<mem>��,
 *       <total>,  ����ֵ����ǰ�洢���еĶ���������,
 *       <used>,   ����ֵ����ǰ�洢���еĶ�����Ŀ��,
 *       <unread>, ����ֵ����ǰ�洢���е�δ��������Ŀ��,
 *       <stat>,   PDUģʽ��������ֵ���ı�ģʽ�����ַ������͡�
 *                 0����REC UNREAD�������յ���δ�����ţ�
 *                 1����REC READ�������յ����Ѷ����ţ�
 *                 2����STO UNSENT�����洢��δ�����ţ�
 *                 3����STO SENT�����洢���ѷ����ţ�
 *                 4����ALL�������ж��š�,
 * [��]: ����������ز���˵��
 *       ����,     ˵��,
 *       <ackpdu>, 16�������ַ�����ȷ��PDU��,
 *       <alpha>,  �ַ����������ڵ绰���ж�Ӧ��������Ŀǰ������˲�����,
 *       <cdata>,  16�������ַ����������������ݡ�,
 *       <ct>,     ����ֵ��ȡֵ��ΧΪ0��255�������������͡�,
 *       <da>,     �ַ������ͣ����Ͷ��ŵ�Ŀ�ĵ�ַ��,
 *       <data>,   16�������ַ������������ݡ�,
 *       <dcs>,    ����ֵ��ȡֵ��ΧΪ0��255���������ݵı��뷽����
 *                 0��GSM 7bit��
 *                 4��8bit��
 *                 8��UNICODE��
 *                 �ò���������������Ϣ�ȼ��Ȳ���������������μ���3GPP TS 23.038���ġ�4 SMS Data Coding Scheme����,
 *       <dt>,     ʱ���ַ������ͣ��������ķ������ŵ�ʱ�䡣
 *       <fo>,     ����ֵ��ȡֵ��ΧΪ0��255��TPDU�ĵ�1���ֽڡ�����������μ���3GPP TS 23.040���ġ�9.2 Service provided by the SM?TL����,
 *       <length>, ����ֵ������ָʾ��,
 *       <mn>,     ����ֵ��ȡֵ��ΧΪ0��255��������������Ҫ�����Ķ��ŵı�š�,
 *       <mr>,     ����ֵ��ȡֵ��ΧΪ0��255�����ű�š�,
 *       <oa>,     �ַ������ͣ�Դ��ַ��,
 *       <pdu>,    16�������ַ�����PDU���ݡ�,
 *       <pid>,    ����ֵ��ȡֵ��ΧΪ0��255��Э���ʶ��,
 *       <ra>,     �ַ������ͣ�Ŀ�ĵ�ַ��,
 *       <sca>,    �ַ������ͣ��������ĵ�ַ�������ɡ�*������#������+������a������b������c������0������9����ɣ����볤�Ȳ�����20����������+���ţ����ַ���,
 *       <scts>,   ʱ���ַ������ͣ�ʱ�������yy/mm/dd,hh:mm:ss��zz����
 *                 �硰94/05/06,22:10:00+08����1994��5��6�գ���8����22ʱ10��0�롣,
 *       <st>,     ����ֵ��ȡֵ��ΧΪ0��255���������İѶ��ŷ���MT��ִ�н����,
 *       <toda>,   ����ֵ��ȡֵ��ΧΪ0��255��Ŀ�ĵ�ַ���͡�,
 *       <tooa>,   ����ֵ��ȡֵ��ΧΪ0��255��Դ��ַ���͡�,
 *       <tora>,   ����ֵ��ȡֵ��ΧΪ0��255��ǰһ�ζ��ŵ�Ŀ�ĵ�ַ���͡�,
 *       <tosca>,  ����ֵ��ȡֵ��ΧΪ0��255���������ĵ�ַ���͡�,
 *       <vp>,     ������Ч�ڣ���ʽ��<fo>����Ч�ڸ�ʽ������
 *                 ��������ʱ���ʽ����������ֵ��ȡֵ��ΧΪ0��255��
 *                 0��143����VP+1��*5���ӣ�
 *                 144��167��12Сʱ+����VP�C143��*30���ӣ���
 *                 168��196����VP�C166��*1�գ�
 *                 197��255����VP�C192��*1�ܡ�
 *                 ����Ǿ���ʱ���ʽ������ʱ���ַ������͡�����������μ���3GPP TS 23.040���ġ�9.2.3.11 TP-Service-Centre-Time-Stamp (TP-SCTS)����,
 */
const AT_SMS_CmdTab g_atSmsCmdTab[] = {
    /* SMS */
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���Ͷ���
     * [˵��]: ����һ�����š�
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: iftextmode(+CMGF=1):
     *             +CMGS=<da>[,<toda>]<CR>
     *             textisentered<ctrl-Z/ESC>
     *     [���]: if text mode (+CMGF=1) and sending successful:
     *             <CR><LF>+CMGS: <mr>[,<scts>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if sending fails:
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: ifPDUmode(+CMGF=0):
     *             +CMGS=<length><CR>
     *             PDUisgiven<ctrl-Z/ESC>
     *     [���]: if PDU mode (+CMGF=0) and sending successful:
     *             <CR><LF>+CMGS: <mr>[,<ackpdu>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if sending fails:
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMGS=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <CR>: MT�������н�����<CR>����4���ַ�<CR><LF><greater_than><space>��ASCII��ֵ13��10��62��32����
     *     <ctrl-Z>: �ַ���0x1A������ʾ����������������Ͷ��š�
     *     <ESC>: �ַ���0x1B������ʾȡ�����η��Ͷ��Ų�����
     *     <����>: ��μ�2.5.1 ���Ų����������˵����
     * [ʾ��]:
     *     �� �ı�ģʽ������
     *       AT+CMGS="13902100077"
     *       >This the first line<CR>
     *       >This is the last line<ctrl-Z>
     *       +CMGS: 252
     *       OK
     *     �� +CSCS����TE��MT֮����ַ���ΪGSM7BIT���룬��������ͨ��������������������磺�ı�@123
     *       AT+CMGS="13902100096"
     *       >00313233<ctrl-Z>
     *       +CMGS: 0
     *       OK
     *     �� PDUģʽ������
     *       AT+CMGS=18
     *       >0015660B813109120090F60008FF044F60597D<ctrl-Z>
     *       +CMGS: 229
     *       OK
     *     �� ����CMGS
     *       AT+CMGS=?
     *       OK
     */
    { AT_CMD_CMGS,
      At_SetCmgsPara, AT_SMS_CMGS_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
#if (FEATURE_MBB_CUST == FEATURE_ON)
      AT_AbortCmgsPara, AT_NOT_SET_TIME,
#else
      VOS_NULL_PTR, AT_NOT_SET_TIME,
#endif
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMGS", (VOS_UINT8 *)"(da),(0-255)", (TAF_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �洢����
     * [˵��]: дһ�����Ų��洢��mem2�����������<stat>�����������Ϊָ����״̬��Ĭ������£�����״̬����Ϊ���洢δ���͡�״̬��
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: iftextmode(+CMGF=1):
     *             +CMGW[=<oa/da>[,<tooa/toda>[,<stat>]]]<CR>
     *             textisentered<ctrl-Z/ESC>
     *             ifPDUmode(+CMGF=0):
     *             +CMGW=<length>[,<stat>]<CR>
     *             PDUisgiven<ctrl-Z/ESC>
     *     [���]: <CR><LF>+CMGW: <index><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMGW=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <CR>: MT�������н�����<CR>����4���ַ�<CR><LF><greater_than><space>��ASCII��ֵ13��10��62��32����
     *     <ctrl-Z>: �ַ���0x1A������ʾ�������������������š�
     *     <ESC>: �ַ���0x1B������ʾȡ������д���Ų�����
     *     <����>: ��μ�2.5.1 ���Ų����������˵����
     * [ʾ��]:
     *     �� �ı�ģʽд����
     *       AT+CMGW="13902100096"
     *       >1651<ctrl-Z>
     *       +CMGW: 0
     *       OK
     *     �� +CSCS����TE��MT֮����ַ���ΪGSM7BIT���룬��������ͨ��������������������磺�ı�@123
     *       AT+CMGW="13902100096"
     *       >00313233<ctrl-Z>
     *       +CMGW: 0
     *       OK
     *     �� PDUģʽд����
     *       AT+CMGW=18
     *       >0015660B813109120090F60004FF0461626364<ctrl-Z>
     *       +CMGW: 3
     *       OK
     *     �� ����CMGW
     *       AT+CMGW=?
     *       OK
     */
    { AT_CMD_CMGW,
      At_SetCmgwPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMGW", (VOS_UINT8 *)"(oa),(0-255),(\"REC UNREAD\",\"REC READ\",\"STO UNSENT\",\"STO SENT\",\"ALL\")", (TAF_UINT8 *)"(0-255),(0-4)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����������ȡ����
     * [˵��]: ���������Ӵ洢����mem1���ж�һ�����ţ������Ϣ״̬Ϊ������δ���������Ϊ�������Ѷ���������״̬���䡣
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CMGR=<index>
     *     [���]: if text mode (+CMGF=1), command successful and SMS-DELIVER:
     *             <CR><LF>+CMGR: <stat>,<oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>,<sca>,<tosca>,<length>]<CR><LF><data><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if text mode (+CMGF=1), command successful and SMS-SUBMIT:
     *             <CR><LF>+CMGR: <stat>,<da>,[<alpha>][,<toda>,<fo>,<pid>,<dcs>,[<vp>],<sca>,<tosca>,<length>]<CR><LF><data><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if text mode (+CMGF=1), command successful and SMS-STATUS-REPORT:
     *             <CR><LF>+CMGR: <stat>,<fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if text mode (+CMGF=1), command successful and SMS-COMMAND:
     *             <CR><LF>+CMGR: <stat>,<fo>,<ct>[,<pid>,[<mn>],[<da>],[<toda>],<length><CR><LF><cdata>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if PDU mode (+CMGF=0) and command successful:
     *             <CR><LF>+CMGR: <stat>,[<alpha>],<length><CR><LF><pdu><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             otherwise:
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMGR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� �ı�ģʽ������������һ������
     *       AT+CMGR=0
     *       +CMGR: "STO SENT","13902100096",,129,21,0,8,255,"+8613800773501",145,4
     *       4F60597D "���"��UNICODE��
     *       OK
     *     �� +CSCS����TE��MT֮����ַ���ΪGSM7BIT���룬��������ͨ��������������������磺�ı�@123
     *       AT+CMGR=0
     *       +CMGR: "STO SENT","13902100096",,129,21,0,8,255,"+8613800773501",145,4
     *       00313233
     *       OK
     *     �� PDUģʽ������������һ������
     *       AT+CMGR=0
     *       +CMGR: 1,,24
     *       0891683108703705F1040D91683109120090F70008603070
     *       OK
     *     �� ����CMGR
     *       AT+CMGR=?
     *       OK
     */
    { AT_CMD_CMGR,
      At_SetCmgrPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMGR", (VOS_UINT8 *)"(0-65535)", (TAF_UINT8 *)"(0-65535)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����״̬��ȡ����
     * [˵��]: ����״̬�Ӵ洢����mem1���ж����ж��ţ������Ϣ״̬Ϊ������δ������Ĭ�ϻ��޸�Ϊ�������Ѷ�������˽������^CMSR���ơ�
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CMGL[=<stat>]
     *     [���]: if text mode (+CMGF=1), command successful and SMS-SUBMITs and/or SMS-DELIVERs:
     *             <CR><LF>+CMGL: <index>,<stat>,<oa/da>,[<alpha>],[<scts>][,<tooa/toda>,<length>]<CR><LF><data>
     *             [<CR><LF>+CMGL: <index>,<stat>,<da/oa>,[<alpha>],[<scts>][,<tooa/toda>,<length>]<CR><LF><data>
     *             [...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if text mode (+CMGF=1), command successful and SMS-STATUS-REPORTs:
     *             <CR><LF>+CMGL: <index>,<stat>,<fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st>
     *             [<CR><LF>
     *             +CMGL: <index>,<stat>,<fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st>
     *             [...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if text mode (+CMGF=1), command successful and SMS-COMMANDs:
     *             <CR><LF>+CMGL: <index>,<stat>,<fo>,<ct>
     *             [<CR><LF>+CMGL: <index>,<stat>,<fo>,<ct>[...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if PDU mode (+CMGF=0) and command successful:
     *             <CR><LF>+CMGL: <index>,<stat>,[<alpha>],<length><CR><LF><pdu>
     *             [<CR><LF>+CMGL: <index>,<stat>,[<alpha>],<length><CR><LF><pdu>[...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             otherwise:
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMGL=?
     *     [���]: <CR><LF>+CMGL: (list of supported <stat>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� �ı�ģʽ�°���״̬�����ж���
     *       AT+CMGL="ALL"
     *       +CMGL: 0,"REC READ","+8613902100097",,"06/03/06,16:34:24+00",
     *       4F60597D "���"��UNICODE��
     *       +CMGL: 1,"REC READ��,"+8613902100064",,"06/03/06,19:35:29+00",
     *       Happy New Year
     *       OK
     *     �� +CSCS����TE��MT֮����ַ���ΪGSM7BIT���룬��������ͨ��������������������磺�ı�@123
     *       AT+CMGL="ALL"
     *       +CMGL: 0,"REC READ","+8613902100097",,"06/03/06,16:34:24+00",
     *       00313233
     *       OK
     *     �� PDUģʽ�б����ж���
     *       AT+CMGL=4
     *       +CMGL: 0,1,,18
     *       0891683108703705F115660B813109120090
     *       +CMGL: 1,2,,24
     *       0891683108703705F1040D91683109120090F60008603070
     *       OK
     *     �� ����CMGL���ı�ģʽ�£�
     *       AT+CMGL=?
     *       +CMGL:("REC UNREAD","REC READ","STO UNSENT","STO SENT","ALL")
     *       OK
     */
    { AT_CMD_CMGL,
      At_SetCmglPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMGL", (VOS_UINT8 *)"(\"REC UNREAD\",\"REC READ\",\"STO UNSENT\",\"STO SENT\",\"ALL\")", (TAF_UINT8 *)"(0-4)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���Ͷ�������
     * [˵��]: ����һ���������
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: iftextmode(+CMGF=1):
     *             +CMGC=<fo>,<ct>[,<pid>[,<mn>[,<da>[,<toda>]]]]<CR>
     *             textisentered<ctrl-Z/ESC>
     *     [���]: if text mode (+CMGF=1) and sending successful:
     *             <CR><LF>+CMGC: <mr>[,<scts>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if sending fails:<CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: ifPDUmode(+CMGF=0):
     *             +CMGC=<length><CR>
     *             PDUisgiven<ctrl-Z/ESC>
     *     [���]: if PDU mode (+CMGF=0) and sending successful:
     *             <CR><LF>+CMGC: <mr>[,<ackpdu>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if sending fails:
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMGC=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <CR>: MT�������н�����<CR>����4���ַ�<CR><LF><greater_than><space>��ASCII��ֵ13��10��62��32����
     *     <ctrl-Z>: �ַ���0x1A������ʾ����������������Ͷ������
     *     <ESC>: �ַ���0x1B������ʾȡ�����η��Ͷ������������
     *     <����>: ��μ�2.5.1 ���Ų����������˵����
     * [ʾ��]:
     *     �� �ı�ģʽ�·��Ͷ�������
     *       AT+CMGC=0,0,0,2,"13902100096"
     *       ><ctrl-Z>
     *       +CMGC: 0
     *       OK
     *     �� ����PDU��������
     *       AT+CMGC=14
     *       >0002A90001A70B813109210090F600<ctrl-Z>
     *     �� ����CMGC
     *       AT+CMGC=?
     *       OK
     */
    { AT_CMD_CMGC,
      At_SetCmgcPara, AT_SMS_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMGC", (VOS_UINT8 *)"(0-255),(0-255),(0-255),(0-255),(da),(0-255)", (TAF_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �Ӵ洢���з��Ͷ���
     * [˵��]: �Ӵ洢����mem2���з���һ�����ţ��������Ŀ�ĵ�ַ<da>�����ڶ��ŷ���ʱ�õ�ַ���滻�洢���еĶ�������������Ŀ�ĵ�ַ��
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CMSS=<index>[,<da>[,<toda>]]
     *     [���]: if text mode (+CMGF=1) and sending successful:
     *             <CR><LF>+CMSS: <mr>[,<scts>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if PDU mode (+CMGF=0) and sending successful:
     *             <CR><LF>+CMSS: <mr>[,<ackpdu>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if sending fails:
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMSS=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� �ı�ģʽ�Ӵ洢���з�����
     *       AT+CMSS=0
     *       +CMSS: 239
     *       OK
     *     �� PDUģʽ�Ӵ洢���з�����
     *       AT+CMSS=3
     *       +CMSS: 240
     *       OK
     *     �� ����CMSS
     *       AT+CMSS=?
     *       OK
     */
    { AT_CMD_CMSS,
      At_SetCmssPara, AT_SMS_CMSS_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMSS", (VOS_UINT8 *)"(0-255),(da),(0-255)", (TAF_UINT8 *)"(0-255),(da),(0-255)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �¶���ȷ��
     * [˵��]: �¶��ţ��������յĶ��źͶ���״̬���棩ȷ�����ͨ��ִ�и�������ȷ���Ƿ��յ�һ��ֱ�ӷ��͸�TE���¶��š�����һ�����ŵõ�ȷ��ǰ��MT������TE�ϱ�����һ�����š�����涨��ʱ����û�еõ�ȷ�ϣ����糬ʱ������MT��ظ���Ӧ����ԭ������硣
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: iftextmode(+CMGF=1):
     *             +CNMA
     *             ifPDUmode(+CMGF=0):
     *             +CNMA[=<n>[,<length>[<CR>
     *             PDUisgiven<ctrl-Z/ESC>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CNMA=?
     *     [���]: if PDU mode (+CMGF=0):
     *             <CR><LF>+CNMA: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             if text mode (+CMGF=1):
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��PDUģʽ����Ч��
     *             0: �൱���ı�ģʽ�·���AT+CNMA����ȷ������
     *             1: ����RP-ACK
     *             2: ����RP-ERROR
     *     <length>: ����ֵ��
     *             TPDU�ֽ�����
     * [ʾ��]:
     *     �� �����ı�����ȷ������
     *       AT+CNMA
     *       OK
     *     �� ����PDU����ȷ������
     *       AT+CNMA=2,7
     *       >0116410300D000<ctrl-Z>
     *     �� �ı�ģʽ����CNMA����
     *       AT+CNMA=?
     *       OK
     *     �� PDUģʽ����CNMA����
     *       AT+CNMA=?
     *       +CNMA: (0-2)
     *       OK
     */
    { AT_CMD_CNMA,
      At_SetCnmaPara, AT_SMS_SET_PARA_TIME, AT_TestCnmaPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CNMA", TAF_NULL_PTR, (TAF_UINT8 *)"(0-2),(0-65535)" },
};

const TAF_UINT16 g_atSmsCmdNum = sizeof(g_atSmsCmdTab) / sizeof(AT_SMS_CmdTab);

static const AT_ParCmdElement g_atSmsTafCmdTbl[] = {
    /* SMS */
    { AT_CMD_CMST,
      At_SetCmstPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
      (TAF_UINT8 *)"^CMST", (TAF_UINT8 *)"(0-255),(da),(0-255),(1-255)" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ö�ȡ���ű��
     * [˵��]: SET�����ڶ�ȡδ�����ţ���ͨ���ţ����������ű��棩���Ƿ���Ҫ�ı佫δ����Ǹĳ��Ѷ������á�
     *         �������^CMSR=1��ʹ�ã�+CMGR,+CMGL����ȡδ�����ţ���ȡ��ö���δ����ǲ��ı䡣
     *         �������^CMSR=0��Ĭ�ϣ���ʹ�ã�+CMGR,+CMGL����ȡδ�����ţ���ȡ��δ����Ǹĳ��Ѷ���ǡ�
     *         READ����ص�ǰ�����������ֵ��
     *         TEST����ظ��������<ReadTag>��ȡֵ��Χ��
     * [�﷨]:
     *     [����]: ^CMSR=<ReadTag>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: ^CMSR?
     *     [���]: <CR><LF>^CMSR: <ReadTag><CR><LF>
     *     [����]: ^CMSR=?
     *     [���]: <CR><LF>^CMSR: (list of support <readTag>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <readTag>: ����ֵ��
     *             0����ȡδ�����ź�ı�δ����ǵ����ã�Ĭ�ϣ���
     *             1����ȡδ�����ź󲻸ı�δ����ǵ����á�
     * [ʾ��]:
     *     �� ��ȡδ�����ź�ı�δ�����
     *       AT^CMSR=0
     *       OK
     *     �� ��ȡδ�����ź󲻸ı�δ�����
     *       AT^CMSR=1
     *       OK
     */
    { AT_CMD_CMSR,
      At_SetCmsrPara, AT_NOT_SET_TIME, At_QryCmsrPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CMSR", (VOS_UINT8 *)"(0,1)" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ��ȡ�������͵�����
     * [˵��]: ��<meml>�з���״ֵ̬��<Stat>����ָ�������ж��ŵ�����ֵ��
     * [�﷨]:
     *     [����]: ^CMGI=<stat>
     *     [���]: <CR><LF>^CMGI: index1,index2,��indexN<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: ^CMGI=?
     *     [���]: <CR><LF>^CMGI: (list of supported <stat>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <stat>: ����ֵ������״̬��
     *             0�����յ���δ�����ţ�
     *             1�����յ����Ѷ����ţ�
     *             2���洢��δ�����ţ�
     *             3���洢���ѷ����ţ�
     *             4�����ж��š�
     * [ʾ��]:
     *     �� ��ȡ���յ���δ������
     *       AT^CMGI=0
     *       OK
     */
    { AT_CMD_CMGI,
      At_SetCmgiPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CMGI", (TAF_UINT8 *)"(0-4)" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �޸Ķ��ű��
     * [˵��]: ��<meml>�У�����λ�õĶ��ű���޸�Ϊ<stat>���������������ڽ����յĶ��ţ��޸ĳ��Ѷ���δ�����š�
     * [�﷨]:
     *     [����]: ^CMMT=<index>,<stat>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: ^CMMT=?
     *     [���]: <CR><LF>^CMMT: (list of supported <index>s),(list of supported <stat>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Index>: ����ֵ���ڴ洢���е�λ�á�
     *     <Stat>: 0�����յ���δ�����ţ�
     *             1�����յ����Ѷ����š�
     * [ʾ��]:
     *     �� �ѵ�һ�����ŵ�״̬�޸�Ϊδ��
     *       AT^CMMT=0,0
     *       OK
     */
    { AT_CMD_CMMT,
      At_SetCmmtPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CMMT", (TAF_UINT8 *)"(0-254),(0,1)" },

    { AT_CMD_CMSTUB,
      At_SetCmStubPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CMSTUB", (VOS_UINT8 *)"(0-11),(0-6)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: APָʾMODEM�Ƿ��пռ�洢����
     * [˵��]: AP-MODEM��̬�£����Ŵ洢��AP�������AP֪ͨMODEM�Ƿ��пռ�洢���š�
     * [�﷨]:
     *     [����]: ^CSASM=<memstat>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CSASM=?
     *     [���]: <CR><LF>^CSASM: (list of supported <memstat>s) <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <memstat>: �����ͣ�MODEM��ǰ���Ŵ洢�ռ�״̬��
     *             0����ǰ���Ŵ洢�ռ�������
     *             1����ǰ���Ŵ洢�ռ�δ����AP���Խ����¶��š�
     * [ʾ��]:
     *     �� AP֪ͨMODEM���Ŵ洢�ռ�����
     *       AT^CSASM=0
     *       OK
     */
    { AT_CMD_CSASM,
      AT_SetMemStatusPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CSASM", (TAF_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ö�����ѡ��
     * [˵��]: ��������Ҫ�������ö�����ѡ��
     * [�﷨]:
     *     [����]: ^SMSDOMAIN=<domain>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: ^SMSDOMAIN?
     *     [���]: <CR><LF>^SMSDOMAIN: <domain><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: ^SMSDOMAIN=?
     *     [���]: <CR><LF>^SMSDOMAIN: (list of supported <domain>s) <CR><LF>
     * [����]:
     *     <domain>: ����ֵ��������ѡ��ȡֵ��Χ(0,1)��
     *             0����ʹ��IMS���Ͷ��ţ�
     *             1������ʹ��IMS���Ͷ��š�
     * [ʾ��]:
     *     �� ���ö�����ѡ��
     *       AT^SMSDOMAIN=1
     *       OK
     *     �� ��ѯ������ѡ��
     *       AT^SMSDOMAIN?
     *       ^SMSDOMAIN: 1
     *       OK
     *     �� ��������
     *       AT^SMSDOMAIN=?
     *       ^SMSDOMAIN: (0,1)
     *       OK
     */
    { AT_CMD_SMSDOMAIN,
      AT_SetSmsDomainPara, AT_SET_PARA_TIME, AT_QrySmsDomainPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^SMSDOMAIN", (VOS_UINT8 *)SMSDOMAIN_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ��ѯע���������2/3/4G CS/PS������������
     * [˵��]: ���������ڲ�ѯmodem��ǰ�Ƿ�������ע��֧��2/3/4G��CS/PS���š�
     * [�﷨]:
     *     [����]: ^SMSNASCAP?
     *     [���]: ��ѯ�ɹ���
     *             <CR><LF>^SMSNASCAP: <state>
     *             <CR><LF>OK<CR><LF>
     *             ��ѯʧ�ܣ�
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <state>: ����ֵ��ע���������2/3/4G CS/PS����������
     *             0����֧�֣�
     *             1��֧�֣�
     * [ʾ��]:
     *       AT^SMSNASCAP?
     *       ^SMSNASCAP: 1
     *       OK
     */
    { AT_CMD_SMSNASCAP,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QrySmsNasCapPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^SMSNASCAP", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: MT���ſ���������������
     * [˵��]: ^TRUSTNUM��������/��ѯMT���ſ���������
     *         ����MT���ſ�������ʱ�������������Ͷ��ź����ֶΣ����ź����ֶ���Ҫ�����ţ�����Ϊ��ʱ�����������ڽ�����ָ���ĺ���д����п������������ֻ��һ���������������ֶβ�Ϊ�գ����ȡ���ſ�������������ָ���Ķ��ź��롣
     *         ������MT���ſ�������ʱ��UE��Ҫ���˶��ſ���������ֻ�д����ڶ��ſ��������еĺ�����ܱ��洢���ϱ���
     *         ���MT���ſ��������ѿ����������ſ�������Ϊ�գ������ж��Ŷ��޷����ա�
     *         �ù���FEATURE_MBB_CUST�꿪��ʱ��Ч��
     * [�﷨]:
     *     [����]: ^TRUSTNUM=<index>[,<number>]
     *     [���]: �����õ�����£�
     *             <CR><LF>OK<CR><LF>
     *             �ڲ�ѯ������£�
     *             [<CR><LF>^TRUSTNUM: <index>,<number><CR><LF>]<CR><LF>OK<CR><LF>
     *             ����ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TRUSTNUM?
     *     [���]: [<CR><LF>^TRUSTNUM: <index>,<number><CR><LF>^TRUSTNUM: <index>,<number>[...]<CR><LF>]<CR><LF>OK<CR><LF>
     *     [����]: ^TRUSTNUM=?
     *     [���]: <CR><LF>^TRUSTNUM: (list of supported <index>s)<CR><LF><CR><LF>OK<CR><LR>
     * [����]:
     *     <index>: ����ֵ��ȡֵ��Χ��0~19��
     * [ʾ��]:
     *     �� ������Ϊ0�ĺ��롰1111111111����ӵ����ſ��������С�
     *       AT^TRUSTNUM=0,"1111111111"
     *       OK
     *     �� ��ȡ���ſ��������е����к��룺
     *       AT^TRUSTNUM?
     *       ^TRUSTNUM: 1,"1111111111"
     *       ^TRUSTNUM: 2,"2222222222"
     *       ^TRUSTNUM: 3,"3333333333"
     *       OK
     *     �� ��ѯ֧�ֵ������б�
     *       AT^TRUSTNUM=?
     *       ^TRUSTNUM: (0-19)
     *       OK
     */
    { AT_CMD_TRUSTNUM,
    AT_SetSmsTrustNumPara, AT_NOT_SET_TIME, AT_QrySmsTrustNumPara, AT_NOT_SET_TIME, AT_TestSmsTrustNumPara,
    AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8 *)"^TRUSTNUM", (VOS_UINT8 *)"(0-19),(number)" },

#if (FEATURE_MBB_CUST == FEATURE_ON)
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: HSMF״̬��������
     */
    { AT_CMD_HSMF,
    AT_SetHsmfPara, AT_SET_PARA_TIME, AT_QryHsmfPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
    VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8 *)"^HSMF", (VOS_UINT8 *)"(0,1)" },
#endif
};

/* ע��taf����ҵ��AT����� */
VOS_UINT32 AT_RegisterSmsTafCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atSmsTafCmdTbl, sizeof(g_atSmsTafCmdTbl) / sizeof(g_atSmsTafCmdTbl[0]));
}

