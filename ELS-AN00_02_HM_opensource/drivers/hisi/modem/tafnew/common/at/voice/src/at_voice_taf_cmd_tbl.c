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
#include "at_voice_taf_cmd_tbl.h"
#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_voice_taf_set_cmd_proc.h"
#include "at_voice_taf_qry_cmd_proc.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_VOICE_TAF_CMD_TBL_C

static const AT_ParCmdElement g_atVoiceTafCmdTbl[] = {
#if (FEATURE_MBB_CUST == FEATURE_OFF)
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ͨ���з���DTMF��
     * [˵��]: ����������ͨ�������������෢��DTMF������FEATURE_MBB_CUST���ʱ�ɴ�������෢��DTMF����
     * [�﷨]:
     *     [����]: ^DTMF=[<call_id>],[<Dtmf_Key>],<On_Length>[,<off_length>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^DTMF=?
     *     [���]: <CR><LF> ERROR<CR><LF>
     * [����]:
     *     <call_id>: ����ֵ������ID��ȡֵ��Χ1~7��
     *             �ò�����Я����Ҳ�ɲ�Я�������Я���ò����������ǵ�ǰ���еĺ���ID��
     *     <Dtmf_Key>: ��ʼ�����෢��DTMFʱ����On_Lengthȡֵ��0������Ҫ������һ��ASCII�ַ���ȡֵ��ΧΪ0��9��#��*��A��D��AT�����б���Я���ò�����
     *             ֹͣ�����෢��DTMFʱ����On_LengthȡֵΪ0��������ע�˲�����AT�����п��Բ�Я���ò�����Ҳ����Я���ò�����
     *     <On_Length>: DTMF�����ŵ�ʱ����
     *             0��ֹͣ�����෢�ͣ�
     *             1����ʼ�����෢��DTMF��Ĭ��ʱ��60s��
     *             ������Ϊ1����Ҫ����0����ֹͣ���š�
     *             ��Ĭ��ʱ��ȡֵΪ��95��150��200��250��300��350����λ�Ǻ��롣ʱ�䵽�ں��Զ�ֹͣ���š�
     *     <off_length>: DTMF���͵ļ������STOP DTMF ACK ~ START DTMF REQ��ʱ��������λΪ���롣
     *             ȡֵ��Χ0ms����10ms-60000ms��
     *             ��ѡ�����������ò���ʱĬ��0ms��
     * [ʾ��]:
     *     �� ��ʼ����DTMF
     *       AT^DTMF=1,A,1,0
     *       OK
     *     �� ֹͣ����DTMF
     *       AT^DTMF=1,A,0,0
     *       OK
     *     �� ָ��ʱ��150ms����DTMF
     *       AT^DTMF=1,A,150,0
     *       OK
     *     �� ָ��ǰһ��DTMF��STOP DTMF ACK ����һ��START DTMF REQDTMF���͵�ʱ����Ϊ70ms
     *       AT^DTMF=1,A,1,70
     *       OK
     *       AT^DTMF=1,A,0,70
     *       OK
     *     �� ����^DTMF
     *       AT^DTMF=?
     *       ERROR
     */
    { AT_CMD_DTMF,
      At_SetDtmfPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DTMF", (VOS_UINT8 *)"(0-7),(@key),(0,1,95,120,150,200,250,300,350),(0-60000)" },
#else
    { AT_CMD_DTMF,
      At_SetDtmfPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      AT_AbortDtmfPara, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DTMF", (VOS_UINT8 *)"(0-7),(@key),(0,1,95,120,150,200,250,300,350),(0-60000)" },
#endif

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ѯͨ��ʱ��
     * [˵��]: ��ѯ���ڽ���ͨ����ͨ��ʱ����
     *         ע�⣺Xģ�²�֧�ֲ�ѯ��
     * [�﷨]:
     *     [����]: ^CDUR=<idx>
     *     [���]: <CR><LF>^CDUR: <idx>,<duration><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CDUR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <idx>: ����ID��
     *     <duration>: ͨ��ʱ������λ���롣
     * [ʾ��]:
     *     �� ��ѯ��ǰͨ��ʱ��
     *       AT^CDUR=1
     *       ^CDUR: 1,136
     *       OK
     */
    { AT_CMD_CDUR,
      AT_SetCdurPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CDUR", (VOS_UINT8 *)"(0-7)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ���ӵ�ַ�ĺ���
     * [˵��]: ����һ�����ӵ�ַ�ĺ��У��ù��̿��Ա���ϣ���
     * [�﷨]:
     *     [����]: ^APDS=<dial_string>[,<sub_string>[,<I>[,<G>[,<call_type>[,<callDomain>[,<srv_type>[,<rtt>[,<CallPull_DialogId>[,<isEncrypt>]]]]]]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>NO CARRIER<CR><LF>
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <dial_string>: �ַ������ͣ����к��루���40��BCD�����ַ�����
     *     <sub_string>: �ַ������ͣ��ӵ�ַ�����20���ַ�����
     *     <I>: ����ֵ���Ƿ���ʾ���к��롣
     *             0������ʾ��
     *             1����ʾ��
     *     <G>: ����ֵ���Ƿ�ʹ��CUG����ҵ��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     *     <call_type>: ����ֵ���������͡�
     *             0���������У�
     *             1����Ƶͨ������������Ƶ��˫��������
     *             2����Ƶͨ�������������Ƶ��˫��������
     *             3����Ƶͨ����˫����Ƶ��˫��������
     *     <callDomain>: ����ֵ��������
     *             0��CS��
     *             1��PS��
     *     <srv_type>: ����ֵ��ҵ�����͡�
     *             0����ҵ������
     *             1��WPS����
     *     <rtt>: ����ֵ��ָʾ�Ƿ���RTTͨ����
     *             0����RTTͨ����
     *             1��RTTͨ����
     *     <CallPull_DialogId>: ����ֵ�����ڶ�·����ȡ����ʱ��Ӧ�÷���Call Pull����ʱ��DialogId��ȡֵ��Χ��0~255��
     *             255����Call Pull������
     *             0~254��Call Pull����ʱ��DialogID��
     *     <isEncrypt>: ����ֵ���Ƿ��������ܣ�
     *             0������������
     *             1����������
     * [ʾ��]:
     *     �� �����ӵ�ַ��IMS����
     *       AT^APDS="12345678900","1234",1,0,0,1,0,0,0
     *       OK
     *     �� �����ӵ�ַ��IMS��Ƶ�绰
     *       AT^APDS="12345678900",,,,1,1,0,0,0
     *       OK
     */
    { AT_CMD_APDS,
      AT_SetApdsPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^APDS", (VOS_UINT8 *)"(@dial_string),(@subaddr),(0,1),(0,1),(0-3),(0,1),(0,1),(0,1),(0-255),(0,1)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ѯ����Դ����
     * [˵��]: ��ѯGULģ����Դ����ʹ���ָʾ��
     * [�﷨]:
     *     [����]: ^CLPR=<call_id>
     *     [���]: <CR><LF>^CLPR: <calling_num_pi>,<no_CLI_cause>,<redirect_num>, <num_type>,<redirect_subaddr>,<subaddr_type>,<redirect_num_pi><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CLPR=?
     *     [���]: <CR><LF>^CLPR: (list of supported <call_id>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <call_id>: ����ֵ������ID��ȡֵ��Χ1~7��
     *     <calling_num_pi>: ����ֵ�����к������ָʾ��
     *             0��������ʾ
     *             1��������ʾ
     *             2�����벻����
     *     <no_CLI_cause>: ����ֵ�����벻���ڵ�ԭ��ֵ��
     *             0�����벻����
     *             1�����û��ܾ�
     *             2������������Ľ���
     *             3�����ѵ绰
     *     <redirect_num>: �ַ������ͣ�����ת�ƺ��롣
     *     <num_type>: ����ֵ�������ַ���͡�
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ֵ��μ�2.4.11 �����ϱ�������룺+CLIP�Ĳ���<type>��
     *     <redirect_subaddr>: �ַ������ͣ��ӵ�ַ��
     *     <subaddr_type>: ����ֵ���ӵ�ַ���͡�
     *     <redirect_num_pi>: ����ֵ������ת�ƺ������ָʾ��
     *             0������ͨ��
     *             1��ͨ������
     *             2���޷�����
     *             3��ͨ������
     * [ʾ��]:
     *     �� ��ѯ����Դ����ɹ�
     *       AT^CLPR=1
     *       ^CLPR: 0,,"12345678900",129,"1234",0,0
     *       OK
     *     �� ����^CLPR����
     *       AT^CLPR=?
     *       ^CLPR: (1-7)
     *       OK
     */
    { AT_CMD_CLPR,
      AT_SetClprPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestClprPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CLPR", (VOS_UINT8 *)"(1-7)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ѯ���һ��CS����д�����
     * [˵��]: ��ѯ���һ��CS���ϱ��Ĵ���ԭ��ֵ��
     * [�﷨]:
     *     [����]: ^CCSERR?
     *     [���]: <CR><LF>^CCSERR: <cause><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CCSERR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cause>: ����ֵ�������4.5 CS������롱��
     * [ʾ��]:
     *     �� CS�����ʧ�ܣ���ѯԭ��ֵ
     *       AT^CCSERR?
     *       ^CCSERR: 1
     *       OK
     *     �� ��������
     *       AT^CCSERR=?
     *       OK
     */
    { AT_CMD_CCSERR,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryCCsErrPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CCSERR", VOS_NULL_PTR },

#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ѯLTE������CSҵ��ʵ�ַ�ʽ
     * [˵��]: ����������UE��ѯ��ע���LTE�����Ƿ�֧��ָ����CSҵ��ʵ�ַ�ʽ��
     * [�﷨]:
     *     [����]: ^LTECS?
     *     [���]: <CR><LF>^LTECS: <SMS OVER SGs>,<SMS OVER IMS>,<CSFB>,<SR-VCC>,<VoLGA><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <SMS OVER SGs>: �Ƿ�֧��SMS OVER SGs��
     *             0����֧�֣�
     *             1��֧�֡�
     *     <SMS OVER IMS>: �Ƿ�֧��SMS OVER IMS��
     *             0����֧�֣�
     *             1��֧�֡�
     *     <CSFB>: �Ƿ�֧��CSFB��
     *             0����֧�֣�
     *             1��֧�֡�
     *     <SR-VCC>: �Ƿ�֧��SR-VCC��
     *             0����֧�֣�
     *             1��֧�֡�
     *     <VoLGA>: �Ƿ�֧��VoLGA��
     *             0����֧�֣�
     *             1��֧�֡�
     * [ʾ��]:
     *     �� �����ǰע���LTE����֧��SMS OVER SGs��CSFB���·��������ѯ
     *       AT^LTECS?
     *       ^LTECS: 1,0,1,0,0
     *       OK
     */
    { AT_CMD_LTECS,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryLtecsInfo, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LTECS", TAF_NULL_PTR },
#endif

#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: IMS����绰���������
     * [˵��]: ��IMS�绰��������У���������ʱʹ�á�
     * [�﷨]:
     *     [����]: ^CACMIMS=<total_callnum>,<isEcon>,<call_address>[,call_address[��.]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CACMIMS=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <total_callnum>: ��Ҫ����෽ͨ���е���������ĸ���
     *     <isEcon>: ����ֵ���Ƿ�����ǿ�Ͷ෽ͨ��
     *             0��������ǿ�Ͷ෽ͨ��
     *             1������ǿ�Ͷ෽ͨ��
     *     <call_address>: �ַ������ͣ��������绰���룬��ǿ�Ͷ෽ͨ�������п���������û�������ǿ�Ͷ෽ͨ��������һ���û���
     * [ʾ��]:
     *     �� IMS����绰��������
     *       AT^CACMIMS=1,0,"+8613923002240"
     *       OK
     *     �� IMS��ǿ�ͻ���绰�����������
     *       AT^CACMIMS=2,1,"+8613923002240","+8613816980803"
     *       OK
     *     �� ����CACMIMS
     *       AT^CACMIMS=?
     *       OK
     */
    { AT_CMD_CACMIMS,
      AT_SetCacmimsPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CACMIMS", (VOS_UINT8 *)"(1-5),(0-1),(@number),(@number),(@number),(@number),(@number)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ������ǿ�Ͷ෽ͨ��
     * [˵��]: ���ڷ�����ǿ�Ͷ෽ͨ����
     * [�﷨]:
     *     [����]: ^ECONFDIAL=<dial_count>,<conf_center_string>,<I>,<call_type>,<callDomain>,<isEcon>,<dial_string>[,<dial_string>��]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>NO CARRIER<CR><LF>
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^ECONFDIAL?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^ECONFDIAL=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <dial_count>: ����ֵ����������������������Ϊ5��
     *     <conf_center_string>: �ַ������ͣ��������ĺ��루���40��BCD�����ַ���
     *             Ŀǰ�������ĺ��뱣����NV�У��ò���Ԥ���Ժ�ʹ��
     *     <I>: ����ֵ���Ƿ���ʾ���к��롣
     *             0������ʾ��
     *             1����ʾ��
     *     <call_type>: <call_type> ����ֵ���������͡�
     *             0�� ��������
     *             1�� ��Ƶͨ������������Ƶ��˫������
     *             2�� ��Ƶͨ�������������Ƶ��˫������
     *             3�� ��Ƶͨ����˫����Ƶ��˫������
     *     <callDomain>: ����ֵ��������
     *             0�� CS ��
     *             1�� PS ��
     *     <isEcon>: ����ֵ���Ƿ�����ǿ�Ͷ෽ͨ��
     *             0��������ǿ�Ͷ෽ͨ��
     *             1������ǿ�Ͷ෽ͨ��
     *     <dial_string>: �ַ������ͣ����к��루���40��BCD�����ַ���
     * [ʾ��]:
     *     �� ������ǿ�Ͷ෽ͨ����Я�������û�
     *       AT^ECONFDIAL=2,"",1,0,1,1,"13812344321","13943211234"
     *       OK
     *     �� ��ѯ����
     *       AT^ECONFDIAL?
     *       OK
     *     �� ����ECONFDIAL
     *       AT^ECONFDIAL=?
     *       OK
     */
    { AT_CMD_ECONFDIAL,
      AT_SetEconfDialPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECONFDIAL", (VOS_UINT8 *)"(1-5),(@econf_center),(0,1),(0-3),(0,1),(0,1),(@number),(@number),(@number),(@number),(@number)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ȡ�������Ϣ
     * [˵��]: ���ڻ�ȡ��ǿ�Ͷ෽ͨ��������������Ϣ�����յ�^CLCCESTATE�����ϱ������״̬�����ı���ϲ��·�^CLCCECONF����ѯ�������Ϣ��
     * [�﷨]:
     *     [����]: ^CLCCECONF?
     *     [���]: <CR><LF>  [^CLCCECONF: Maximum-user-count, n_address, entity, Display-text,Status[,entity,  Display-text,Status[..]]]
     *             <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CLCCECONF=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <Maximum-user-count>: ����ֵ����ǿ�Ͷ෽ͨ���������߸�����(�����ǿ�ͻ���û�д�������ֵΪ0)
     *     <n_address>: ����ֵ���෽ͨ������߸���
     *     <entity>: ���к���
     *     <Display-text>: ���к�������
     *     <Status>: ����ֵ�������״̬��
     *             0��disconnected��
     *             1��connected��
     *             2��on-hold��
     *             3��pre-dialing;
     *             4��dialing
     * [ʾ��]:
     *     �� ��ѯ��ǿ�Ͷ෽ͨ������ߵ�״̬
     *       AT^CLCCECONF?
     *       ^CLCCECONF: 5,1,"13800000000","xxx",1
     *       OK
     */
    { AT_CMD_CLCCECONF,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryClccEconfInfo, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CLCCECONF", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ȡ��ǿ�Ͷ෽ͨ�������ʧ��ԭ��ֵ
     * [˵��]: ���ڻ�ȡ��ǿ�Ͷ෽ͨ�������ʧ��ԭ��ֵ
     * [�﷨]:
     *     [����]: ^ECONFERR?
     *     [���]: <CR><LF>  [^ECONFERR: <number>,<type>,<cause>]
     *             [<CR><LF>  ^ECONFERR: <number>,<type>,<cause>]
     *             [...]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^ECONFERR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ������ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <cause>: ԭ��ֵ������ֵ��
     * [ʾ��]:
     *     �� ��ѯ��ǿ�Ͷ෽ͨ��ʧ��ԭ��ֵ
     *       AT^ECONFERR?
     *       ^ECONFERR: "13800000001",129,130
     *       ^ECONFERR: "13800000002",129,130
     *       OK
     */
    { AT_CMD_ECONFERR,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryEconfErrPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECONFERR", VOS_NULL_PTR },

    /* �����class��֧��VIDEO */
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: IMS���еȴ�
     * [˵��]: ���������ڼ������ȥ����IMS���еȴ���
     * [�﷨]:
     *     [����]: ^CCWAI=<mode>,<service_class>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CCWAI?
     *     [���]: <CR><LF>^CCWAI: <mode>,<ccwa_ctrl_mode><CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CCWAI=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������ģʽ��ȡֵ0~1��
     *             0��ȥ���
     *             1�����
     *     <service_class>: ����ֵ���������ͣ�ȡֵ1~2��
     *             1��������
     *             2����Ƶ��
     *             3��֪ͨmodem CS����еȴ���������ƣ����ز�������
     *     <ccwa_ctrl_mode>: ����ֵ�����еȴ��Ŀ���ģʽ��ȡֵ0~1��
     *             0�����еȴ���3gpp������ƣ�
     *             1�����еȴ���UE���ƣ����ն˲෽��
     * [ʾ��]:
     *     �� ����IMS���еȴ�
     *       AT^CCWAI=1,1
     *       OK
     *     �� ȥ����IMS���еȴ�
     *       AT^CCWAI=0,1
     *       OK
     */
    { AT_CMD_CCWAI,
      AT_SetCcwaiPara, AT_SET_PARA_TIME, AT_QryCcwaiPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CCWAI", (VOS_UINT8 *)"(0,1),(1,2,3)" },
#endif

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �г���ǰ���еĺ�����Ϣ
     * [˵��]: �г���ǰCS/IMS�ĺ�����Ϣ��
     * [�﷨]:
     *     [����]: ^CLCC?
     *     [���]: [<CR><LF>^CLCC: <id1>,<dir>,<stat>,<mode>,<mpty>,<voice_domain>,<call_type>,<isEconference>[,<number>,<type>[,<display_name> [,<terminal_video_support>[,<imsDomain> [,<rtt>,<rtt_chan_id>,<cps>,<isEncrypt>]]]]]
     *             [<CR><LF>^CLCC: <id2>,<dir>,<stat>,<mode>,<mpty><voice_domain>,<call_type>,<isEconference>[,<number>,<type>[,<display_name> [,<terminal_video_support>[,<imsDomain>[,<rtt>,<rtt_chan_id>[,<cps>,<isEncrypt>]]]]]][...]]]<CR><LF>]
     *             <CR><LF>OK<CR><LF>
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CLCC=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <idx>: ����ֵ�����б�ʶ��ȡֵ��Χ1~7��
     *     <dir>: ����ֵ�����з���
     *             0�����У�
     *             1�����С�
     *     <state>: ����ֵ������״̬��
     *             0������״̬��
     *             1�����б���״̬��
     *             2�����У�����״̬��
     *             3�����У�������״̬��
     *             4�����У�����״̬��
     *             5�����У����еȴ�״̬��
     *     <mode>: ����ֵ������ģʽ��
     *             0���������У�
     *             1�����ݺ��У�
     *             2�����档
     *     <mpty>: ����ֵ���෽ͨ��״̬��Xģmodem������ʶ������ͨ����Xģ��CLCC��ѯ��Ϣ�й̶���д��3��ͨ����
     *             0�����ڶ෽ͨ���У�
     *             1���ڶ෽ͨ���С�
     *     <voice_domain>: ����ֵ����ʶ������
     *             0��CS��绰
     *             1��IMS��绰
     *     <call_type>: ����ֵ���������͡�
     *             0����������
     *             1����Ƶͨ������������Ƶ��˫������
     *             2����Ƶͨ�������������Ƶ��˫������
     *             3����Ƶͨ����˫����Ƶ��˫������
     *             9���������С�
     *     <isEconference>: ����ֵ�� �������͡�
     *             0��������ǿ�Ͷ෽ͨ��
     *             1����IMS������ǿ�Ͷ෽ͨ��
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ����ṹ���2-6��ʾ�����У�
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ������
     *             Type-of-number��bit[6:4]����ȡֵ���£�
     *             000���û����˽�Ŀ�ĵ�ַ����ʱ��ѡ�ô�ֵ����ʱĿ�ĵ�ַ������������д��
     *             001�����û���ʶ���ǹ��ʺ���ʱ��ѡ�ô�ֵ��������Ϊ�ǹ��ڷ�ΧҲ������д��
     *             010�����ں��룬�������ǰ׺���ߺ�׺�����û����͹��ڵ绰ʱ��ѡ�ô�ֵ��
     *             011���������ڵ��ض����룬���ڹ�����߷����û�����ѡ�ô�ֵ��
     *             101����������ΪGSM��ȱʡ7bit���뷽ʽ��
     *             110���̺��롣
     *             111����չ������
     *             Numbering-plan-identification��bit[3:0]����ȡֵ���£�
     *             0000������������ĺ��뷽��ȷ����
     *             0001��ISDN/telephone numbering plan��
     *             0011��Data numbering plan��
     *             0100��Telex numbering plan��
     *             1000��National numbering plan��
     *             1001��Private numbering plan��
     *             1010��ERMES numbering plan��
     *             ˵������bit[6:4]ȡֵΪ000��001��010ʱbit[3:0]����Ч��
     *     <display_name>: ��Ҫ��ʾ�����֡�
     *     <terminal_video_support>: ����ֵ���Զ��Ƿ�֧�����������
     *             0����֧�֣�
     *             1��֧�֡�
     *     <imsDomain>: ����ֵ��IMS�绰����VOLTE��VONR�ϻ�����VOWIFI�ϣ�
     *             0��IMS�绰��VOLTE��
     *             1��IMS�绰��VOWIFI��
     *             2��IMS�绰��VONR��
     *     <rtt>: ����ֵ��ָʾ��ǰ��·ͨ���Ƿ�ΪRTTͨ����
     *             0����RTTͨ����
     *             1��RTTͨ����
     *             2��Զ��ΪTTYͨ����
     *             3��Զ�˲�֧��RTT��
     *     <rtt_chan_id>: ����ֵ��ָʾ��ǰ��·ͨ���û���ʹ�õ�ͨ���ţ�<rtt>����Ϊ1ʱֵ�����壬-1Ϊ��Чֵ��
     *     <cps>: ����ֵ��ÿ���������͵��ַ�����0��ʾ��Ч��
     *     <isEncrypt>: ����ֵ����������״̬��
     *             0������������
     *             1����������
     * [��]: �����ַ���͵Ľṹ
     *       bit:   7,    6,              5,              4,              3,                             2,                             1,                             0,
     *              1,    Type-of-number, Type-of-number, Type-of-number, Numbering-plan-identification, Numbering-plan-identification, Numbering-plan-identification, Numbering-plan-identification,
     * [ʾ��]:
     *     �� ��ʾ��ǰ����IMS�������Ϣ
     *       AT^CLCC?
     *       ^CLCC: 1,0,0,0,0,1,0,0,"13810000000",129,,,,0,0,0
     *       ^CLCC: 2,0,1,0,0,1,0,0,"13800000000",129,,,,0,0,0
     *       OK
     *     �� ����CLCC
     *       AT^CLCC=?
     *       OK
     */
    { AT_CMD_CLCC_IMS,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryClccPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CLCC", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �Ҷ������ȴ��еĵ绰
     * [˵��]: ���������ڹҶ������ȴ��еĵ绰��ָ���Ҷϵ�ԭ��ֵ��
     *         �������������������
     * [�﷨]:
     *     [����]: ^REJCALL=<id>,<cause>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^REJCALL=?
     *     [���]: <CR><LF>^REJCALL:  (list of supported <cause>s) <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <id>: ����ֵ�����б�ʶ��ȡֵ��Χ1~7��
     *             ˵��
     *             ��+CLCC��^CLCC�����ѯ����е�<id>һ�¡�
     *     <cause>: ����ֵ���Ҷ�ԭ��ֵ��
     *             0���û�æ��CS���¶�Ӧ#17 User busy��IMS���¶�Ӧ486 Busy Here��
     *             1�����оܾ���CS���¶�Ӧ#21 Call rejected��IMS���¶�Ӧ603 Decline��
     * [ʾ��]:
     *     �� �ܾ����磬ԭ��ֵ�û�æ
     *       AT^REJCALL=1,0
     *       OK
     *     �� ִ�в�������
     *       AT^REJCALL=?
     *       ^REJCALL: (0,1)
     *       OK
     */
    { AT_CMD_REJCALL,
      AT_SetRejCallPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestRejCallPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^REJCALL", (VOS_UINT8 *)"(1-7),(0,1)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ѯͨ������������������Ϣ
     * [˵��]: ����������ͨ�������в�ѯ�����ŵ��Ĵ�����Ϣ��
     * [�﷨]:
     *     [����]: ^CSCHANNELINFO?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^ CSCHANNELINFO: <status>,<voice_domain><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF> ERROR <CR><LF>
     *     [����]: ^CSCHANNELINFO=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <status>: ����ֵ������ͨ�����ͣ�
     *             0���޴�������Ϣ��
     *             1�����������ã�խ��������
     *             2�����������ã����������
     *             3�����������ã�EVS NB������
     *             4�����������ã�EVS WB������
     *             5�����������ã�EVS SWB������
     *     <voice_domain>: ����ֵ�������绰�ĺ�����
     *             0��3GPP CS�������绰��
     *             1��IMS�������绰��
     *             2��3GPP2 CS�������绰��
     * [ʾ��]:
     *     �� ִ�в�ѯ���CS��GSM��������ͨ����խ������
     *       AT^CSCHANNELINFO?
     *       ^CSCHANNELINFO: 1,0
     *       OK
     *     �� ִ�в�ѯ�����ǰ����������ͨ��
     *       AT^CSCHANNELINFO?
     *       ERROR
     *     �� ִ�в�������
     *       AT^CSCHANNELINFO=?
     *       OK
     */
    { AT_CMD_CSCHANNELINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryCsChannelInfoPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CSCHANNELINFO", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �������غ�ģʽ״̬��ѯ
     * [˵��]: EMCCBM�����������ڲ�ѯ��ǰ�Ƿ��ڽ������ĺ��лغ�ģʽ��
     * [�﷨]:
     *     [����]: ^EMCCBM?
     *     [���]: <CR><LF>^ EMCCBM: n<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <n>: �������лغ�ģʽ״̬��
     *             1����ǰ���ڻغ�ģʽ�У�
     *             0����ǰ�����ڻغ�ģʽ�У�
     * [ʾ��]:
     *     �� CALLBACK״̬��ѯ
     *       AT^EMCCBM?
     *       ^ EMCCBM: 1
     *       OK
     */
    { AT_CMD_EMCCBM,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryEmcCallBack, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^EMCCBM", VOS_NULL_PTR },

#if (FEATURE_ECALL == FEATURE_ON)
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �ỰECALL����
     * [˵��]: �����������ڷ���ECALL�Ự��
     *         �����������ڻ�ȡ��ǰ֧�ֵ�ECALL������
     *         ˵���������������Balong V722C60/C70, V711C60, V765C60��
     * [�﷨]:
     *     [����]: ^ECLSTART=<activation_type>,
     *             <type_of_call>[,<dial_num>[,<oprt_mode>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^ECLSTART=?
     *     [���]: <CR><LF>^ECLSTART: (list of supported <activation_type>s), (list of supported <type_of_call>s) ,(list of supported <oprt_mode>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <activation_type>: ����ֵ����ʾ����ECALL�Ự�Ĵ�������
     *             0���û�����ECALL�Ự
     *             1���Զ�����ECALL�Ự
     *             ֻ�н������й�ע��ֵ�����ԻỰ����ע��ֵ��
     *     <type_of_call>: ����ֵ����ʾ��ǰ�����ECALL�Ự����
     *             0��������ԻỰ
     *             1�������������
     *             2������Reconfiguration����
     *     <dial_num>: ���е绰���룬ASCII�ַ�����
     *             �Ϸ����ַ�����������0�� �C ��9��, ��*��, ��#��, ��+������+��ֻ�ܳ����ں������ǰ�棬�������󳤶Ȱ���3GPP TS 24.008 10.5.4.7���塣
     *             ������Ժ���ʱ��Ӧ��Ҫ���绰���룬���û�����룬���SIM����EEfdn�л�ȡ�������������ʱ���������뷢�͵�����࣬�������û��ʵ�����壬�����ṩ��Ҳ���Բ��ṩ������Reconfiguration����ʱ��Ӧ��Ҫ���绰���룬���û�����룬���SIM����EFsdn�л�ȡ��
     *     <oprt_mode>: ����ֵ����ʾ����eCall�����С���ݼ���MSD������ģʽ��ȱʡģʽΪ0��
     *             0��PULL Mode
     *             1��PUSH Mode
     * [ʾ��]:
     *     �� �ֶ�����������У�ģʽΪPUSHģʽ
     *       AT^ECLSTART=0,1,,1
     *       OK
     *     �� ������Ժ��У�ģʽΪPULLģʽ
     *       AT^ECLSTART=0,0,"150xxxx2856",0
     *       OK
     *     �� ����Reconfiguration���У�ģʽΪPUSHģʽ
     *       AT^ECLSTART=0,2,"150xxxx2856",1
     *       OK
     *     �� ��������
     *       AT^ECLSTART=?
     *       ^ECLSTART: (0,1),(0,1,2),(0,1)
     *       OK
     */
    { AT_CMD_ECLSTART,
      AT_SetEclstartPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestEclstartPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLSTART", (VOS_UINT8 *)"(0,1),(0-2),(PNUMBER),(0,1)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ECALL�Ự�Ͽ�
     * [˵��]: ���������ڶϿ���ǰECALL�Ự
     *         ����ǰû��eCall���·��������OK��
     *         ˵���������������Balong V722C60/C70, V711C60, V765C60��
     * [�﷨]:
     *     [����]: ^ECLSTOP
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [ʾ��]:
     *     �� �Ͽ���ǰECALL�Ự
     *       AT^ECLSTOP
     *       OK
     */
    { AT_CMD_ECLSTOP,
      AT_SetEclstopPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLSTOP", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ECALL���ܲ�������
     * [˵��]: ���������������õ����ȡMSD���ݵ�ģʽ���Ƿ��ֹ�������ܣ��Ƿ����Զ��ز����ܡ�
     *         ��ѯ�������ڻ�ȡ��ǰ��������Ϣ��
     *         �����������ڻ�ȡ��������֧�ֵ�ȡֵ��Χ��
     *         ˵���������������Balong V722C60/C70, V711C60, V765C60��
     * [�﷨]:
     *     [����]: ^ECLCFG=<mode>[,<voc_config>][,<redial_config>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^ECLCFG?
     *     [���]: <CR><LF>^ECLCFG: <mode>,<voc_config><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^ECLCFG=?
     *     [���]: <CR><LF>^ECLCFG: (list of supported <mode>s),(list of supported <voc_config>s) ,(list of supported <redial_config>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ����ʾ�����ȡMSD���ݵ�ģʽ��Ŀǰֻ֧��͸��ģʽ��
     *             0��͸��ģʽ
     *     <voc_config>: ����ֵ����ʾ��MSD����������Ƿ��ֹ�������ݵĴ��䣬����ȱʡ���Ĭ����һ�ε����á�
     *             0����MSD���������ͬ�����������źţ�Ĭ��ֵ��
     *             1����MSD��������н�ֹ�����źŵĴ��䣬���������źŵĴ������MSD���ݵĴ��䡣
     *     <redial_config>: ����ֵ��ָʾ���û��߽����Զ��ز����ܣ�Ŀǰ��֧�������Զ��ز����ܡ�
     *             0�������Զ��ز����ܡ�
     *             1�������Զ��ز����ܡ�����ǰ��֧�֣�
     * [ʾ��]:
     *     �� ���õ����ȡMSD���ݵ�ģʽΪ͸��ģʽ�����ҽ�ֹ��������
     *       AT^ECLCFG=0,1,0
     *       OK
     *     �� ��ѯ����
     *       AT^ECLCFG?
     *       ^ECLCFG: 0,0
     *       OK
     *     �� ��������
     *       AT^ECLCFG=?
     *       ^ECLCFG: (0),(0-1),(0-1)
     *       OK
     */
    { AT_CMD_ECLCFG,
      AT_SetEclcfgPara, AT_SET_PARA_TIME, AT_QryEclcfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLCFG", (VOS_UINT8 *)"(0),(0-1),(0-1)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: MSD����͸������
     * [˵��]: �������������·���ǰECALL�Ự��Ҫ�����MSD��Ϣ��
     *         ��ѯ�������ڻ�ȡMT��ǰ��������µ�MSD��Ϣ��
     *         ˵���������������Balong V722C60/C70, V711C60, V765C60��
     * [�﷨]:
     *     [����]: ^ECLMSD=<ecall_msd>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^ECLMSD?
     *     [���]: <CR><LF>^ECLMSD: <ecall_msd><CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <ecall_msd>: ʮ�������ı��ַ���������ʮ���������ݱ��뷽ʽ���ַ�������Ϊ280���ֽڣ���ʾ����MSDЭ��Ҫ���140���ֽ�ԭʼ���ݡ�����MSD���ݱ����ʽ��ο�MSDЭ�顣
     * [ʾ��]:
     *     �� �·���ǰECALL�Ự��Ҫ�����MSD��Ϣ
     *       AT^ECLMSD="
     *       0101084141414141414141414141313131313131880000000100000001000000010100010001000100010600010db885a308d313198a2e03707334000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
     *       OK
     *     �� ��ѯ����
     *       AT^ECLMSD?
     *       ^ECLMSD:
     *       "0101084141414141414141414141313131313131880000000100000001000000010100010001000100010600010db885a308d313198a2e03707334000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
     *       OK
     */
    { AT_CMD_ECLMSD,
      AT_SetEclmsdPara, AT_SET_PARA_TIME, AT_QryEclmsdPara, AT_QRY_PARA_TIME, AT_TestEclmsdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLMSD", (VOS_UINT8 *)"(MSD)" },

    { AT_CMD_ECLABORT,
      AT_SetEclAbortPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLABORT", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: eCall AL_ACK��Ϣ�б��ѯ����
     * [˵��]: ���������ڲ�ѯPSAP���͵����20��AL_ACK��Ϣ�ͽ��յ�ʱ�����¼�б�����б�Ϊ�գ��򷵻�OK������б��б���ļ�¼����20�������¼�¼���滻����ļ�¼��
     * [�﷨]:
     *     [����]: ^ECLLIST?
     *     [���]: <CR><LF>^ECLLIST: <timestamp>,<AL_ACK>[
     *             <CR><LF>^ECLLIST: <timestamp>,<AL_ACK>[...]]
     *             <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <timestamp>: �ַ������ͣ�ָʾIVS����PSAP���͵�AL_ACK��ʱ�䡣
     *             ��ʽΪ��
     *             ��YYYY/MM/DD,HH:MM:SS+ZZ��
     *             YYYY��ȡֵ��Χ��2000��2100��
     *     <AL_ACK>: ����ֵ����PSAP���ղ��ɹ���֤MSD����ʱ��PSAP��AL_ACK���͵�IVS������Clear Down��־��AL_ACKҪ��IVS�Ҷ�eCall�����ο�Э��16062  7.5.5��
     *             AL_ACK��4 Bit��ɣ�����Bit3��Bit4Ϊ����λ
     *             Bit1ΪFormat version��ȡֵ���£�
     *             0��Format version0��
     *             1��Format version1��
     *             Bit2ΪStatus��ȡֵ���£�
     *             0��MSD���ݱ���֤��
     *             1��MSD���ݱ���֤������Ҫ��IVS�Ҷ�eCall��
     * [ʾ��]:
     *     �� AL_ACK��Ϣ�б��ѯ
     *       AT^ECLLIST?
     *       ^ECLLIST: "2018/11/28,03:18:16+32",0
     *       OK
     */
    { AT_CMD_ECLLIST,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryEclListPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLLIST", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: eCall ����ģʽ��������
     * [˵��]: ��������������������eCall����ģʽ������eCall onlyģʽ�²�����ע�����磬����eCall inactive״̬������eCall�ᴥ��ע����Test eCall��Reconfiguration eCall�ĺ����EFFDN��ȡ��eCall and normalģʽ����ע�����磬Test eCall��Reconfiguration eCall�ĺ����EFSDN��ȡ��ע����Ҫ������SIM������Ч��
     *         ��ѯ�������ڲ�ѯ��ǰeCall����ģʽ��
     *         �����������ڻ�ȡ��������֧�ֵ�ȡֵ��Χ��
     * [�﷨]:
     *     [����]: ^ECLMODE=<mode>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^ECLMODE?
     *     [���]: <CR><LF>^ECLMODE: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^ECLMODE=?
     *     [���]: <CR><LF>^ECLMODE: (list of supported <mode>s)
     *             <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ����ʾ����eCall����ģʽ��
     *             0��Autoģʽ��UE����SIM�ļ�����������ģʽ������3GPPЭ��TS31.102�������EFUST��eCall data (Service n��89) ��FDN (Service n��2) ����������EFEST��FDN��Service n��1������������eCall onlyģʽ���У������EFUST��eCall data (Service n��89) ��SDN (Service n��4) ����������eCall and normalģʽ���У������������normalģʽ���У�
     *             1��Force eCall Onlyģʽ�������ǲ����������SIM����UE����eCall onlyģʽ�����У�
     *             2��Force eCall and normalģʽ�������ǲ����������SIM����UE����eCall and normalģʽ�����У�
     * [ʾ��]:
     *     �� ����eCall����ģʽΪFore eCall only
     *       AT^ECLMODE=1
     *       OK
     */
    { AT_CMD_ECLMODE,
      AT_SetEclModePara, AT_SET_PARA_TIME, AT_QryEclModePara, AT_QRY_PARA_TIME, AT_TestEclModePara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLMODE", (VOS_UINT8 *)"(0-2)" },

#endif
    /*
     * [���]: Э��AT-�绰����
     * [����]: MT���п���������������
     * [˵��]: ��������/��ѯ���еĺ��п���������
     *         �������������ͺ��к����ֶζ���Ϊ��ʱ�����к����ֶ���Ҫ�����ţ������������ڽ�����ָ���ĺ���д����п������������ֻ��һ���������������ֶβ�Ϊ�գ���������ȡ���п�������������ָ�������к��롣
     *         ������MT���п�������ʱ��UE��Ҫ���˺��п���������ֻ�д����ں��п��������еĺ�����ܱ��洢���ϱ���
     *         ���MT���п��������ѿ����������п�������Ϊ�գ������к��ж��޷����롣
     *         �ù���FEATURE_MBB_CUST�꿪��ʱ��Ч��
     * [�﷨]:
     *     [����]: ^TRUSTCNUM=<index>[,<number>]
     *     [���]: �����õ�����£�
     *             <CR><LF>OK<CR><LF>
     *             �ڲ�ѯ������£�
     *             [<CR><LF>^TRUSTCNUM: <index>,<number><CR><LF>]<CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TRUSTCNUM?
     *     [���]: [<CR><LF>^TRUSTCNUM: <index>,<number><CR><LF>^TRUSTCNUM: <index>,<number>[...]<CR><LF>]<CR><LF>OK<CR><LF>
     *     [����]: ^TRUSTCNUM=?
     *     [���]: <CR><LF>^TRUSTCNUM: (list of supported <index>s)<CR><LF><CR><LF>OK<CR><LR>
     * [����]:
     *     <index>: ����ֵ��ȡֵ��Χ��0~19��
     * [ʾ��]:
     *     �� ������Ϊ0�ĺ��롰1111111111����ӵ����п��������У�
     *       AT^TRUSTCNUM=0,"1111111111"
     *       OK
     *     �� ��ȡ���к��п����������룺
     *       AT^TRUSTCNUM?
     *       ^TRUSTCNUM: 1,"1111111111"
     *       ^TRUSTCNUM: 2,"2222222222"
     *       ^TRUSTCNUM: 3,"3333333333"
     *       OK
     *     �� ��ѯ֧�ֵ������б�
     *       AT^TRUSTCNUM=?
     *       ^TRUSTCNUM: (0-19)
     *       OK
     */
    { AT_CMD_TRUSTCNUM,
      AT_SetTrustCallNumPara, AT_NOT_SET_TIME, AT_QryTrustCallNumPara, AT_NOT_SET_TIME, AT_TestCallTrustNumPara,
      AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TRUSTCNUM", (VOS_UINT8 *)"(0-19),(number)" },
};

/* ע��taf����AT����� */
VOS_UINT32 AT_RegisterVoiceTafCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atVoiceTafCmdTbl, sizeof(g_atVoiceTafCmdTbl) / sizeof(g_atVoiceTafCmdTbl[0]));
}
