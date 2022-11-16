/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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

#include "at_extend_cmd.h"
#include "at_cmd_proc.h"
#include "taf_tafm_remote.h"
#include "at_device_cmd.h"

#include "at_test_para_cmd.h"
#include "at_sim_pam_set_cmd_proc.h"
#include "at_sim_pam_qry_cmd_proc.h"
#include "at_custom_pam_set_cmd_proc.h"
#include "at_custom_pam_qry_cmd_proc.h"
#include "at_custom_mm_set_cmd_proc.h"
#include "at_custom_mm_qry_cmd_proc.h"
#include "at_custom_taf_set_cmd_proc.h"
#include "at_custom_taf_qry_cmd_proc.h"
#include "at_general_pam_set_cmd_proc.h"
#include "at_general_pam_qry_cmd_proc.h"
#include "at_general_lmm_set_cmd_proc.h"
#include "at_general_lmm_qry_cmd_proc.h"
#include "at_general_mm_set_cmd_proc.h"
#include "at_general_mm_qry_cmd_proc.h"
#include "at_safety_mm_qry_cmd_proc.h"
#include "at_safety_mm_set_cmd_proc.h"
#include "at_general_taf_set_cmd_proc.h"
#include "at_general_taf_qry_cmd_proc.h"
#include "at_general_drv_set_cmd_proc.h"
#include "at_data_taf_set_cmd_proc.h"
#include "at_data_taf_qry_cmd_proc.h"
#include "at_data_lnas_set_cmd_proc.h"
#include "at_data_lnas_qry_cmd_proc.h"
#include "at_data_basic_set_cmd_proc.h"
#include "at_data_basic_qry_cmd_proc.h"
#include "at_voice_hifi_set_cmd_proc.h"
#include "at_voice_hifi_qry_cmd_proc.h"
#include "at_voice_taf_set_cmd_proc.h"
#include "at_voice_taf_qry_cmd_proc.h"
#include "at_ss_taf_set_cmd_proc.h"
#include "at_ss_taf_qry_cmd_proc.h"
#include "at_sms_taf_set_cmd_proc.h"
#include "at_sms_taf_qry_cmd_proc.h"
#include "at_phone_mm_set_cmd_proc.h"
#include "at_phone_mm_qry_cmd_proc.h"
#include "at_phone_ims_set_cmd_proc.h"
#include "at_phone_ims_qry_cmd_proc.h"
#include "at_phone_taf_set_cmd_proc.h"
#include "at_phone_taf_qry_cmd_proc.h"
#include "at_cdma_qry_cmd_proc.h"
#include "at_cdma_set_cmd_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_EXTENDCMD_C

/*
 * ʾ��: ^CMDX �����ǲ���E5���뱣���������+CLAC�о���������ʱ����ʾ����һ�������ǲ���˫���ŵ��ַ���,
 *      �ڶ��������Ǵ�˫���ŵ��ַ����������������������Ͳ���
 * !!!!!!!!!!!ע��: param1��param2��ʾ����ʵ�ʶ�������ʱӦ��������ļ��(����߽���Ч��)!!!!!!!!!!!!!
 *  {AT_CMD_CMDX,
 *  At_SetCmdxPara, AT_SET_PARA_TIME, At_QryCmdxPara, AT_QRY_PARA_TIME, At_TestCmdxPara, AT_NOT_SET_TIME,
 *  AT_ERROR, CMD_TBL_E5_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
 *  (VOS_UINT8 *)"^CMDX", (VOS_UINT8 *)"(@param1),(param2),(0-255)"},
 */
static const AT_ParCmdElement g_atExtendCmdTbl[] = {
    /* PHONEBOOK */
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ���õ绰���洢��
     * [˵��]: ���õ绰���洢����
     *         ���������3GPP TS 27.007Э�顣
     *         ��������MBBģʽ����Ч��PHONEģʽ�����á�
     * [�﷨]:
     *     [����]: +CPBS=<storage>[,<password>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPBS?
     *     [���]: <CR><LF>+CPBS: <storage>[,<used>,<total>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CPBS=?
     *     [���]: <CR><LF>+CPBS: (list of supported <storage>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <storage>: �ַ������ͣ�Ĭ��ֵ��SM����
     *             ��SM����(U)SIM�绰����
     *             ��ON������������MSISDN��
     *             ��EN�����������к��룻
     *             ��FD����(U)SIM���ϵ�FDN�ļ���
     *     <used>: ����ֵ����ǰ�洢�����Ѿ�����ļ�¼����
     *     <total>: ����ֵ����ǰ�洢��֧�ֵ�����¼����
     *     <password>: 10���������ַ������ͣ��绰�����룬����4��8��Ŀǰ������˲�����
     * [ʾ��]:
     *     �� ���õ�ǰ�洢������
     *       AT+CPBS="SM"
     *       OK
     *     �� ��ѯ��ǰ�洢�����͡����ü�¼��������¼��
     *       AT+CPBS?
     *       +CPBS: "SM",50,100
     *       OK
     *     �� �г����еĿ��ô洢������
     *       AT+CPBS=?
     *       +CPBS: ("SM","ON","EN","FD")
     *       OK
     */
    { AT_CMD_CPBS,
      At_SetCpbsPara, AT_SET_PARA_TIME, At_QryCpbsPara, AT_QRY_PARA_TIME, AT_TestCpbsPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"+CPBS", (TAF_UINT8 *)"(\"SM\",\"ON\",\"EN\",\"FD\")" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ȡ�绰��
     * [˵��]: ��ȡ��ǰ�洢����λ����<index1>��<index2>֮�䣨�����䣩�ĵ绰����¼��
     *         ���ֻ����<index1>����ֻ������<index1>λ�õĵ绰����¼��
     *         ��������ص�ǰ�绰���洢��֧�ֵ�<number>��<text>����󳤶ȡ�
     *         ���������3GPP TS 27.007Э�顣
     *         ע�⣺����index1-index2��ֵ������200���������ʳ��ֲ�ѯ������ز�ȫ���⣬ԭ����USB����Ϊ256�������һ�β�ѯ���࣬PC��������������USB��������л��棬�������������᷵�ط���ʧ�ܣ������ϱ�ͨ��û�з�ѹ���ƣ�ֻ�ܶ�����Ϣ�������ϱ������ȫ��
     * [�﷨]:
     *     [����]: +CPBR=<index1>[,<index2>]
     *     [���]: <CR><LF> [+CPBR: <index1>,<number>,<type>,<text>[<CR><LF>+CPBR: <index2>,<number>,<type>,<text>]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPBR=?
     *     [���]: <CR><LF>+CPBR: (list of supported <index>s),[<nlength>],[<tlength>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <index1>��\n<index2>: ����ֵ���绰�����ڴ洢���е�λ�á�
     *             <index1>��<index2>ȡֵ�����㲢��С�ڵ���+CPBS?����ص�<total>��
     *             <index1>С�ڵ���<index2>��
     *             ˵��
     *             ���绰����ʵ�ʴ洢�ĵ绰������С����<index1>��<index2>ȷ��������ʱ������ʵ�ʴ洢�ĵ绰����¼����ʾ��
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <text>: �ַ������ͣ�������
     *     <nlength>: ����ֵ���绰�������󳤶ȡ�
     *     <tlength>: ����ֵ����������󳤶ȡ�
     * [ʾ��]:
     *     �� ��ȡ��¼��AT��CPBS����Ϊ��SM����
     *       AT+CPBR=1,3
     *       +CPBR: 1,"12345678",129,"reinhardt",
     *       +CPBR: 2,"7654321",129,"sophia",
     *       +CPBR: 3,"111111111",129,"David"
     *       OK
     *     �� ��ȡ��¼��AT+CPBS����Ϊ��ON����
     *       AT+CPBR=1
     *       +CPBR: 1,"+8612345678",145,"reinhardt",
     *       OK
     *     �� ��ѯλ�÷�Χ���������������󳤶�
     *       AT+CPBR=?
     *       +CPBR: (1-65535),20,100
     *       OK
     */
    { AT_CMD_CPBR2,
      At_SetCpbr2Para, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, AT_TestCpbrPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"+CPBR", (TAF_UINT8 *)"(1-65535),(1-65535)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: д��绰��
     * [˵��]: �ڵ�ǰ�洢������ӡ��޸ġ�ɾ��һ���绰����¼��
     *         ������������ֻ����index����index��Ӧλ�õĵ绰����Ŀ����ɾ����
     *         ���index�ֶα�ʡ�ԣ��������к���number�ֶΣ��������绰����Ŀ����д����һ����λ�á������ʱû�п�λ�ã����ϱ���+CME ERROR: memory full��������ѡ�绰������Ϊ"SM"��"ON"�е�һ��ʱ�ɽ���д������������������ִ��д��������AT�����з��ز���������Ĵ�����ʾ��
     *         ��������ص�ǰ�洢����<number>��<text>����󳤶ȡ�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CPBW=[<index>][,<number>[,<type>[,<text>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPBW=?
     *     [���]: <CR><LF>+CPBW: (list of supported <index>s),[<nlength>],(list of supported <type>s),[<tlength>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <text>: �ַ������ͣ�������
     *     <index>: ����ֵ���绰�����ڴ洢���е�λ�á�
     *     <number>: �绰���룬�ַ������ͣ��Ϸ��ַ�Ϊ����0������9������#������*������+������(������)������-��������(������)������-�������ַ����۳������κ�λ�ã�������򶼺������ǣ�������Ϊ�Ƿ��ַ�������+��ֻ�ܳ����ں������ʼλ�á�
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <nlength>: ����ֵ���绰�������󳤶ȡ�
     *     <tlength>: ����ֵ����������󳤶ȡ�
     * [ʾ��]:
     *     �� �޸�һ����¼
     *       AT+CPBW=3,"12345678",129,"reinhardt"
     *       OK
     *     �� ���һ����¼
     *       AT+CPBW=,"12345678",129,"reinhardt"
     *       OK
     *     �� ɾ��һ����¼
     *       AT+CPBW=7
     *       OK
     *     �� ��ѯ�洢��λ�÷�Χ��������󳤶ȣ��������ͺ�������󳤶�
     *       AT+CPBW=?
     *       +CPBW: (1-65535),20,(0-255),14
     *       OK
     */
    { AT_CMD_CPBW2,
      At_SetCpbwPara2, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, AT_TestCpbrPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"+CPBW", (TAF_UINT8 *)"(1-65535),(number),(0-255),(text)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ѯ��������
     * [˵��]: ��ȡ��ǰ(U)SIM�������б��������¼������ʾ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CNUM
     *     [���]: <CR><LF>+CNUM: [<text1],<number1>,<type1>
     *             [<CR><LF>+CNUM: [<text2>],<number2>,<type2>]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CNUM=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <text>: �ַ������ͣ�������
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     * [ʾ��]:
     *     �� ��ȡ��¼
     *       AT+CNUM
     *       +CNUM: "reinhardt","12345678",129,
     *       OK
     *     �� ����CNUM
     *       AT+CNUM=?
     *       OK
     */
    { AT_CMD_CNUM,
      At_SetCnumPara, AT_QRY_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"+CNUM", TAF_NULL_PTR },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: �����绰��
     * [˵��]: ��ѯ��ǰ�洢����������<findtext>ƥ��ļ�¼��
     *         ��������ص�ǰ�洢��֧�ֵ�<number>��<text>����󳤶ȡ�
     *         ���������3GPP TS 27.007Э�顣
     *         ��ע��ʹ��AT+CPBW������ϵ�ˣ������ַ�ΪUCS2-81��ʽ��AT+CPBF�޷�֧�ִ˸�ʽ������������
     * [�﷨]:
     *     [����]: +CPBF=<findtext>
     *     [���]: <CR><LF> [+CPBF: <index1>,<number>,<type>,<text>]
     *             <CR><LF>+CPBF: <index2>,<number>,<type>,<text>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPBF=?
     *     [���]: <CR><LF>+CPBF: [<nlength>],[<tlength>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <findtext>,<text>: �ַ������ͣ��ؼ����ַ�����
     *     <index1>,<index2>: ����ֵ���绰�����ڴ洢���е�λ�á�
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <nlength>: ����ֵ���绰�������󳤶ȡ�
     *     <tlength>: ����ֵ����������󳤶ȡ�
     * [ʾ��]:
     *     �� ����ƥ���¼
     *       AT+CPBF="reinhardt"
     *       +CPBF: 3,"12345678",129,"reinhardt",
     *       +CPBF: 7,"7654321",129,"reinhardt",
     *       +CPBF: 11,"111111111",129,"reinhardt"
     *       OK
     *     �� ��ѯ��¼�ĺ������������󳤶�
     *       AT+CPBF=?
     *       +CPBF: 20,100
     *       OK
     */
    { AT_CMD_CPBF,
      At_SetCpbfPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, AT_TestCpbrPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"+CPBF", (TAF_UINT8 *)"(number),(text)" },

    /* USIM */
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ͨ��UICC�����ŵ�����
     * [˵��]: ����������Զ��Ӧ�ó�����ֱ�ӿ��Ƶ�ǰѡ��Ŀ����ϰ�װ��SIM����TE������GSM/UMTSָ���Ŀ���ڴ���SIM��Ϣ��
     *         ������SIM����������CRSM��ȣ�CSIM�Ķ�������TE��SIM-MT�ӿڽ��и���Ŀ��ƣ��ӿڵ������ͽ�������ͨ�������<command>ֵ��ͨ������<command>�������Զ���ɡ����TEӦ��û��ʹ�ý���������߲�����< command >�����Զ�����������MT���ܻ��ͷ�������
     * [�﷨]:
     *     [����]: +CSIM=<length>,<command>
     *     [���]: ������ȷ��
     *             +CSIM: <length>,<response>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF> +CME ERROR: <err><CR><LF>
     * [����]:
     *     <length>: ����ֵ��<command>��<response>�з��͸�TE���ַ����ȣ�ͨ����<command>��<response>���ȵ�������ȡֵ��Χ4-520��
     *     <command>: ����ֵ��SIM�������
     *             MTͨ��3GPP TS 51.011 [28]�еĸ�ʽ��SIM�����ݵ������16�����ַ���ʽ���ο�+CSCS���
     *     <response>: Я������ִ�гɹ����ϱ���״̬������ݡ�
     *             SIM��ͨ��3GPP TS 51.011 [28]�еĸ�ʽ��MT���ص������ʽ��16�����ַ���ʽ���ο�+CSCS���
     * [ʾ��]:
     *     �� ѡ��OPL�ļ�
     *       AT+CSIM=14,"00A40004026F5A"
     *       +CSIM: 4,"6134"
     *       OK
     */
    { AT_CMD_CSIM,
      At_SetCsimPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE_E5 | CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"+CSIM", (TAF_UINT8 *)"(4-520),(cmd)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ���߼�ͨ��
     * [�﷨]:
     *     [����]: +CCHO=<dfname>
     *     [���]: <CR><LF><sessionid><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CCHO=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <dfname>: ����ֵ��Ӧ�����ƣ�����1~16�ֽڡ�
     *             UICC�������п��Ա�ѡ���Ӧ�ö���һ����Ӧ��DF���ơ�
     *     <sessionid>: ����ֵ���߼�ͨ���š�
     * [ʾ��]:
     *     �� ���߼�ͨ��
     *       AT+CCHO="A0000000871002FF86FFFF89FFFFFFFF"
     *       +CCHO: 1465853795
     *       OK
     *     �� �ر��߼�ͨ��
     *       AT+CCHC=1465853795
     *       OK
     *     �� ����CCHO
     *       AT+CCHO=?
     *       OK
     */
    { AT_CMD_CCHO,
      At_SetCchoPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"+CCHO", (TAF_UINT8 *)"(dfname)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ���߼�ͨ��
     * [˵��]: ���߼�ͨ����������APDU�����P2������
     *         ��������ʱû����ص�Э��ο�����Ϊ��֧��OMAPI3.0��׼��
     * [�﷨]:
     *     [����]: +CCHP=<dfname><P2>
     *     [���]: <CR><LF><sessionid><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CCHP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <dfname>: ����ֵ��Ӧ�����ƣ�����1~16�ֽڡ�
     *             UICC�������п��Ա�ѡ���Ӧ�ö���һ����Ӧ��DF���ơ�
     *     <P2>: ����ֵ��APDU�����P2������ȡֵ0~255��Ŀǰ֧�֣�
     *             0x00:����Ӧ�á�
     *             0x40:��ֹӦ�á�
     *             0x04:���߼�ͨ������Ҫ����FCP��
     *             0x0C:���߼�ͨ����û�����ݴ��ء�
     *     <sessionid>: ����ֵ���߼�ͨ���š�
     * [ʾ��]:
     *     �� ���߼�ͨ��
     *       AT+CCHP="A0000000871002FF86FFFF89FFFFFFFF",4
     *       +CCHP: 2056116483
     *       OK
     *     �� �ر��߼�ͨ��
     *       AT+CCHC=2056116483
     *       OK
     *     �� ����CCHP
     *       AT+CCHP=?
     *       OK
     */
    { AT_CMD_CCHP,
      At_SetCchpPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"+CCHP", (TAF_UINT8 *)"(dfname),(0-255)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: �ر��߼�ͨ��
     * [˵��]: �ر��߼�ͨ����
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CCHC=<sessionid>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CCHC=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <sessionid>: ����ֵ���߼�ͨ���š�
     * [ʾ��]:
     *     �� ���߼�ͨ��
     *       AT+CCHO="A0000000871002FF86FFFF89FFFFFFFF"
     *       +CCHO: 1465853795
     *       OK
     *     �� �ر��߼�ͨ��
     *       AT+CCHC=1465853795
     *       OK
     *     �� ����CCHC
     *       AT+CCHC=?
     *       OK
     */
    { AT_CMD_CCHC,
      At_SetCchcPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"+CCHC", (TAF_UINT8 *)"(0-4294967295)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ͨ��UICC�߼��ŵ�����
     * [˵��]: ����<command>�е����ָ����UICCͨ����
     *         ����������ն�APK����ʹ�á�
     * [�﷨]:
     *     [����]: +CGLA=<sessionid>,<length>,<command>
     *     [���]: ��ȷ����ʱ��
     *             +CGLA: <length>,<response>
     *             ����ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <sessionid>: UINT32���ͣ�0-0xFFFFFFFF��:����AT�����еı�ʾ������������APDU���UICCͨ��
     *     <length>: UINT32���ͣ�8-522�����������<command>�ĳ���
     *     <command>: ���͸�UICC�������ο�3GPP TS 31.010��
     *     <response>: ����ֵ
     * [ʾ��]:
     *     �� ִ����������
     *       AT+CGLA=1,8,"INCREASE"
     *       +CGLA: 8,"INCREASE"
     */
    { AT_CMD_CGLA,
      At_SetCglaPara, AT_SET_CGLA_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"+CGLA", (TAF_UINT8 *)"(0-4294967295),(8-522),(command)" },

    /*
     * [���]: Э��AT-��ȫ����
     * [����]: PIN����
     * [˵��]: PIN����֤�ͽ�����
     *         ��֤ʱ<pin>�������PIN�룬<newpin>�������룻
     *         ����ʱ<pin>�������PUK�룬<newpin>���滻��PIN�����PIN�루������������ʱ����˲�������
     *         ��ѯ�����PIN��ĵ�ǰ״̬��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CPIN=<pin>[,<newpin>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPIN?
     *     [���]: <CR><LF>+CPIN: <code><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CPIN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <pin>,<newpin>: �ַ������ͣ�����4��8��
     *     <code>: �ַ�������������˫���ţ���
     *             READY��MT��������������
     *             SIM PIN��(U)SIM PIN��������
     *             SIM PUK��(U)SIM PUK��������
     *             SIM PIN2��(U)SIM PIN2��������
     *             SIM PUK2��(U)SIM PUK2��������
     *             PH-NET PIN��������PIN��������
     *             PH-NET PUK��������PUK��������
     *             PH-NETSUB PIN��������PIN��������
     *             PH-NETSUB PUK��������PUK��������
     *             PH-SP PIN���������ṩ��PIN��������
     *             PH-SP PUK���������ṩ��PUK��������
     * [ʾ��]:
     *     �� PIN��֤����
     *       AT+CPIN="1234"
     *       OK
     *     �� PIN������������һ��������PUK�룬�ڶ����������µ�PIN��
     *       AT+CPIN="11111111","1234"
     *       OK
     *     �� ��ѯ��ǰPIN��״̬
     *       AT+CPIN?
     *       +CPIN: SIM PIN
     *       OK
     *     �� ����CPIN
     *       AT+CPIN=?
     *       OK
     */
    { AT_CMD_CPIN,
      At_SetCpinPara, AT_SET_PARA_TIME, At_QryCpinPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"+CPIN", (TAF_UINT8 *)"(@pin),(@newpin)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ����(U)SIM����
     * [˵��]: ���������ڷ���(U)SIM����
     *         SIM������μ���3GPP 51.011���ġ�9.2 Coding of the commands����������ı��롣
     *         USIM������μ���ETSI TS 102.221���ġ�11 Commands����������ı��롣
     *         ���������3GPP TS 27.007Э��
     *         ����FEATURE_UE_UICC_MULTI_APP_SUPPORT��򿪵İ汾��dallas ��֧�Ǵ򿪵ģ�austin �ǹرյģ���<fileid>���ڵ�ʱ��<pathid>һ��Ҫ���ڡ�
     *         ��FEATURE_UE_UICC_MULTI_APP_SUPPORT��İ汾���棬���ڿ�ģ��֧���˶�Ӧ��ͨ������˸�����Ĭ�Ϸ��ʵ��ǻ���ͨ���Ŀ��ļ���������ͨ���Ŀ�Ӧ�����ܿ���NV 4001�����NVû�����ã���ô������EFDIR��AID˳����г�ʼ����
     * [�﷨]:
     *     [����]: +CRSM=<command>[,<fileid>[,<P1>,<P2>,<P3>[,<data>[,<pathid>]]]]
     *     [���]: <CR><LF>+CRSM: <sw1>,<sw2>[,<response>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CRSM=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <command>: ����ֵ��SIM�������
     *             162����ѯ��¼�ļ���Ч��¼��Ϣ��
     *             176����ȡ�������ļ���
     *             178����ȡ��¼���ݣ�
     *             192����ȡ��Ӧ��
     *             214�����¶������ļ���
     *             220�����¼�¼���ݣ�
     *             242��״̬��ѯ��
     *     <fileid>: ����ֵ��EF�ļ�ID����״̬��ѯ���������������·��ò�����
     *     <P1>,<P2>,<P3>: ����ֵ������״̬��ѯ�����Щ��������ָ�������255���μ�3GPP TS 51.011  9.2 Coding of the commands��
     *     <data>: ��ʮ�����Ƹ�ʽ����Ϣ�ֶΡ��514������Ϊ2����������
     *     <pathid>: �ַ������ͣ�����SIM/USIM��EF�ļ���·������ο�ETSI TS 102 221������״̬��ѯ���������������·��ò�����
     *     <sw1>,<sw2>: ����ֵ������ִ�к�SIM�����ص���Ӧ��
     *     <response>: Я������ִ�гɹ����ϱ������ݣ����ڶ����Ƹ��ºͼ�¼�������<response>�����ء�
     * [ʾ��]:
     *     �� ��״̬��ѯ
     *       AT+CRSM=242
     *       +CRSM: 90,0,"622D8202782184 0CA0000000871002FF49FF0589A503DA01038A01058B03 2F0603C60C90016083010183010C830181"
     *       OK
     *     �� ��ȡIMSI
     *       AT+CRSM=176,28423,0,0,9,,"3F007FFF"
     *       +CRSM: 90,0,"084906202811908074"
     *       OK
     *     �� ��ȡ��Ӧ
     *       AT+CRSM=192,20272,0,0,15,,"3F007F105F3A"
     *       +CRSM: 144,0,"621E82054221004C0283024F30A5038001718A01058B036F0605800200988800"
     *       OK
     *     �� ��ѯ��¼�ļ���Ч��¼��Ϣ
     *       AT+CRSM=162,0x6F3B,1,4,0x1C,"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF","3F007FFF"
     *       +CRSM: 144,0,"0102030405060708090A"
     *       OK
     */
    { AT_CMD_CRSM,
      At_SetCrsmPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"+CRSM", (TAF_UINT8 *)"(0-255),(1-65535),(0-255),(0-255),(0-255),(cmd),(pathid)" },

    /* SMS CONFIG */
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ����TE�ַ���
     * [˵��]: ����TE��MT֮����ַ���������ָ�������ı������ͣ�IRA��GSM��UCS2�������ͣ�������AT��������б���ֱ�ʾ�Ĳ��������������Ϊ+CSCS���õı������ͣ�Ĭ�ϱ�������Ϊ��IRA����
     *         D><str>[I][G][;]
     *         +CUSD=[<n>[,<str>[,<dcs>]]]
     *         +CPBR:<index1>,<number>,<type>,<text>
     *         +CPBF=<findtext>
     *         +CPBF:<index1>,<number>,<type>,<text>
     *         +CPBW=<index>][,<number>[,<type>[,<text>]]]
     *         +CMGS=<da>[,<toda>]<CR><text><ctrl-Z/ESC>
     *         +CSCA=<sca>[,<tosca>]
     *         +CMSS=<index>[,<da>[,<toda>]]
     *         +CMGW[=<oa/da>[,<tooa/toda>[,<stat>]]]<CR><text><ctrl-Z/ESC> +CMGC=<fo>,<ct>[,<pid>[,<mn>[,<da>[,<toda>]]]]<CR><text><ctrl-Z/ESC>
     *         +CMT:<oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>,<sca>,<tosca>,<length>]<CR><LF><data>
     *         +CDS:<fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st>
     *         +CMGL:<index>,<stat>,<oa/da>,[<alpha>],[<scts>][,<tooa/toda>,
     *         <length>]<CR><LF><data>
     *         +CMGL:<index>,<stat>,<fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st>
     *         +CMGR:<stat>,<oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>,<sca>,<tosca>,<length>]<CR><LF><data>
     *         +CMGR:<stat>,<da>,[<alpha>][,<toda>,<fo>,<pid>,<dcs>,[<vp>],<sca>,<tosca>,<length>]<CR><LF><data>
     *         +CMGR:<stat>,<fo>,<ct>[,<pid>,[<mn>],[<da>],[<toda>],<length>
     *         <CR><LF><cdata>]
     *         ^CMGL:<index>,<stat>,<oa/da>,[<alpha>],[<scts>][,<tooa/toda>,
     *         <length>]<CR><LF><data>
     *         ^CMGL:<index>,<stat>,<fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st>
     *         ^CMGR:<stat>,<oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>,<sca>,<tosca>,<length>]<CR><LF><data>
     *         ^CMGR:<stat>,<da>,[<alpha>][,<toda>,<fo>,<pid>,<dcs>,[<vp>],<sca>,<tosca>,<length>]<CR><LF><data>
     *         ^CMGR:<stat>,<fo>,<ct>[,<pid>,[<mn>],[<da>],[<toda>],<length>
     *         <CR><LF><cdata>]
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CSCS=[<chset>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CSCS?
     *     [���]: <CR><LF>+CSCS: <chset><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CSCS=?
     *     [���]: <CR><LF>+CSCS: (list of supported <chset>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <chset>: �ַ������ͣ��ַ�����Ĭ��ֵΪ��IRA����
     *             ��IRA����ASCII�룻
     *             ��UCS2����UNICODE�룻
     *             ��GSM����GSM 7 bit default alphabet�롣
     * [ʾ��]:
     *     �� �����ַ���ΪUNICODE
     *       AT+CSCS="UCS2"
     *       OK
     *     �� ���ö������ĺ���1234��"0031"תΪ0x31����ASCII���ַ�"1"
     *       AT+CSCA="0031003200330034"
     *       OK
     *     �� ��������UNICODE���ţ�������123456��������"ABC"
     *       AT+CMGS="003100320033003400350036"
     *       >004100420043<ctr-Z>
     *     �� �����ַ���ΪIRA
     *       AT+CSCS="IRA"
     *       OK
     *     �� ���ö������ĺ���1234
     *       AT+CSCA="1234"
     *       OK
     *     �� ����Ӣ�Ķ��ţ�������123456��������"ABC"
     *       AT+CMGS="123456"
     *       >ABC<ctr-Z>
     *     �� ��ѯCSCS
     *       AT+CSCS?
     *       +CSCS: "IRA"
     *       OK
     *     �� ����CSCS
     *       AT+CSCS=?
     *       +CSCS: ("IRA","UCS2","GSM")
     *       OK
     */
    { AT_CMD_CSCS,
      At_SetCscsPara, AT_NOT_SET_TIME, At_QryCscsPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"+CSCS", (TAF_UINT8 *)"(\"IRA\",\"UCS2\",\"GSM\")" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �����ϱ�������
     * [˵��]: �����Ƿ����ô����롣
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CMEE=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CMEE?
     *     [���]: <CR><LF>+CMEE: <n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CMEE=?
     *     [���]: <CR><LF>+CMEE: (list of supported <n>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ���������ϱ���ʽ��Ĭ��ֵΪ0��
     *             0��������+CME ERROR: <err>��ֻ�ϱ�ERROR��
     *             1������+CME ERROR: <err>��<err>ȡֵΪ�����ŷ�ʽ��
     *             2������+CME ERROR: <err>��<err>ȡֵΪ�ı�������ʽ��
     * [ʾ��]:
     *     �� <n>Ϊ0ʱ
     *       AT+CMEE=0
     *       OK
     *       AT+CLIP=2
     *       ERROR�������󣬽��ϱ�ERROR
     *     �� <n>Ϊ1ʱ
     *       AT+CMEE=1
     *       OK
     *       AT+CLIP=2
     *       +CME ERROR: 50���������ϱ�������
     *     �� <n>Ϊ2ʱ
     *       AT+CMEE=2
     *       OK
     *       AT+CLIP=2
     *       +CME ERROR: Incorrect parameters���������ϱ���������
     *     �� ��ѯCMEE
     *       AT+CMEE?
     *       +CMEE: 2
     *       OK
     *     �� ����CMEE
     *       AT+CMEE=?
     *       +CMEE: (0-2)
     *       OK
     */
    { AT_CMD_CMEE,
      At_SetCmeePara, AT_NOT_SET_TIME, At_QryCmeePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"+CMEE", (TAF_UINT8 *)"(0-2)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯCS��PS�������
     * [˵��]: ���ڲ�ѯUE(GUL) CS���PS������롣
     *         ���������3GPP TS 27.007Э�顣
     *         ������������������
     * [�﷨]:
     *     [����]: +CEER
     *     [���]: <CR><LF>+CEER: <cs cause>,<ps cause><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CEER=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cs_cause>: ���һ��CS����д����롣����ֵ�������4.5 CS������롱
     *     <ps_cause>: ���һ��PS����д����롣����ֵ��������
     *             0~65535�������������
     * [ʾ��]:
     *     �� ��ѯCS��PS�������
     *       AT+CEER
     *       +CEER: 99,99
     *       OK
     *     �� ��������
     *       AT+CEER=?
     *       OK
     */
    { AT_CMD_CEER,
      At_SetCeerPara, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
      (TAF_UINT8 *)"+CEER", TAF_NULL_PTR },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ı�ģʽ�²�����ʾ
     * [˵��]: �����ı�ģʽ����Ϣ�ϱ���ʽ��
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CSDH=[<show>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CSDH?
     *     [���]: <CR><LF>+CSDH: <show><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CSDH=?
     *     [���]: <CR><LF>+CSDH: (list of supported <show>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <show>: ����ֵ��Ĭ��ֵΪ0��
     *             0���ڶ�������+CMT��+CMGL��+CMGR������в���ʾ����ͷ��Ϣ��
     *             ����SMS-DELIVERs��SMS-SUBMITs���Ͷ��ţ�+CMT��+CMGL��+CMGR���������в���ʾ<length>��<toda>��<tooa>��<sca>��<tosca>��<fo>��<vp>��<pid>��<dcs>��
     *             ����SMS-COMMANDs���Ͷ��ţ�+CMGR���������в���ʾ<pid>��<mn>��<da>��<toda>��<length>��<cdata>��
     *             1���ڶ�������+CMT��+CMGL��+CMGR���������ʾͷ��Ϣ��
     * [ʾ��]:
     *     �� �����ı������ϱ���ʽΪ��ʾ
     *       AT+CSDH=1
     *       OK
     *     �� ��ѯ�ı������ϱ���ʽ
     *       AT+CSDH?
     *       +CSDH: 1
     *       OK
     *     �� ����CSDH
     *       AT+CSDH=?
     *       +CSDH: (0,1)
     *       OK
     */
    { AT_CMD_CSDH,
      At_SetCsdhPara, AT_NOT_SET_TIME, At_QryCsdhPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"+CSDH", (TAF_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �¶���֪ͨ
     * [˵��]: �����������ֵ��MT���������󽫱���0����ʱ�����ϱ��κ��¶��š����鲻ʹ��AT+CNMI=0,0,0,0,0�����÷�ʽ��
     *         ����֪ͨ��������ʧ�Դ洢���У����MT�ڷ���֮ǰ�رյ�Դ���������п��ܶ�ʧ�����ԣ���<mode>=0��2ʱ������ʹ�ö���ֱ��ת����<mt>=2��3����Ҳ����ʹ�ö���״̬����ֱ��ת����<ds>=1����<bm>��ΪBALONGֻ֧��CBM�ϱ�������û�н�Լ��Ӧ�õ�<BM>������
     *         �����¶����ϱ���ʽ��
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CNMI[=<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CNMI?
     *     [���]: <CR><LF>+CNMI: <mode>,<mt>,<bm>,<ds>,<bfr><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CNMI=?
     *     [���]: <CR><LF>+CNMI: (list of supported <mode>s),(list of supported <mt>s),(list of supported <bm>s),(list of supported <ds>s),(list of supported <bfr>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������֪ͨ��ʽ��ȡֵ��ΧΪ0��3��Ŀǰ��֧��0��2��
     *             0��������֪ͨ������ME�У����ME�Ļ���������������֪ͨ�������ϵ�֪ͨ��
     *             1��������ֱ֪ͨ�ӷ��͸�TE�����޷�����ʱ�����磬����online dataģʽ��������֪ͨ��
     *             2��������֪ͨ�Ͷ���״̬����ֱ�ӷ��͸�TE�����޷�����ʱ�����磬����online dataģʽ����������֪ͨ������ME�У������Է���ʱһ���Է��͸�TE��
     *             ע�⣺
     *             ����֪ͨ��������ʧ�Դ洢���У����MT�ڷ���֮ǰ�رյ�Դ���������п��ܶ�ʧ�����ԣ���<mode>=0��2ʱ������ʹ�ö���ֱ��ת����<mt>=2��3����Ҳ����ʹ�ö���״̬����ֱ��ת����<ds>=1����<bm>��ΪBALONGֻ֧��CBM�ϱ�������û�н�Լ��Ӧ�õ�<BM>������
     *     <mt>: ����ֵ���¶����ϱ���ʽ��ȡֵ��ΧΪ0��3��
     *             0�������¶��ŷ���TE��
     *             1���¶���ͨ��+CMTI: <mem>,<index>��ʽ����TE��
     *             2���¶���ͨ��+CMT: [<alpha>],<length><CR><LF><pdu>��PDUģʽ������+CMT: <oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>,<sca>,<tosca>,<length>]<CR><LF><data>���ı�ģʽ����ʽ����TE��
     *             3�����յ�CLASS3����ʱ���¶���ͨ��+CMT: [<alpha>],<length><CR><LF><pdu>��PDU ģʽ������+CMT:<oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>,<sca>,<tosca>,<length>]<CR><LF><data>���ı�ģʽ����ʽ����TE��
     *             ��CLASS3���ţ��¶���ͨ��+CMTI: <mem>,<index>��ʽ����TE��
     *     <bm>: ����ֵ���¹㲥��Ϣ���ϱ���ʽ��ȡֵ��ΧΪ0��2��
     *             0������С���㲥��Ϣ����TE��
     *             2���¹㲥����ͨ��+CBM��
     *     <ds>: ����ֵ������״̬������ϱ���ʽ��ȡֵ��ΧΪ0��2��
     *             0������״̬���淢��TE��
     *             1��״̬����ͨ��+CDS: <length><CR><LF><pdu>��PDUģʽ������+CDS: <fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st>���ı�ģʽ����ʽ����TE��
     *             2��״̬����ͨ��+CDSI: <mem>,<index>��ʽ����TE��
     *     <bfr>: ����ֵ���������ô�<mode>=0ģʽ����<mode>=1-2ģʽ�󻺴�Ĵ���ȡֵ��ΧΪ0��1��
     *             0������<mode>1-2ģʽ�󣬽������unsolicited result codeһ���Է��͸�TE��
     *             1������<mode>1-2ģʽ�󣬽������unsolicited result code��ա�
     * [ʾ��]:
     *     �� �����¶����ϱ���ʽΪ+CMTI: <mem>,<index>��״̬����Ϊ+CDSI: <mem>,<index>
     *       AT+CNMI=2,1,0,2,0
     *       OK
     *     �� ��ѯ�¶����ϱ���ʽ
     *       AT+CNMI?
     *       +CNMI: 2,1,0,2,0
     *       OK
     *     �� ���� CNMI
     *       AT+CNMI=?
     *       +CNMI: (0-2),(0-3),(0,2),(0-2),(0,1)
     *       OK
     */
    { AT_CMD_CNMI,
      At_SetCnmiPara, AT_SET_PARA_TIME, At_QryCnmiPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"+CNMI", (TAF_UINT8 *)"(0-2),(0-3),(0,2),(0-2),(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ƶ�����·
     * [˵��]: ���ÿ��ƶ����м�Э�飨RP��������ӵ������ԡ���ʹ���˸����ԣ���������Ҳ֧��ʱ������������������ʱRP������ӽ�һֱ���ڣ���������ÿ��������֮���ٽ��в����ͽ����Ĺ��̣��Ӷ����͵��ٶȽ���ܶࡣ
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CMMS=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMMS?
     *     [���]: <CR><LF>+CMMS: <n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CMMS=?
     *     [���]: <CR><LF>+CMMS: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ����ֹ��ʹ�ܿ��ƶ����м�Э�飨RP��������ӵ������Թ��ܣ�Ĭ��ֵΪ2��
     *             0����ֹ��
     *             1��ʹ��һ�Ρ���ǰһ�����Ͷ��ŵķ��غͺ�һ�����Ͷ��ŵļ��С��1s��5s�������ȡֵȡ����MT��ʵ�֣���RP���ӱ��ֲ��ϡ���������1s��5sʱ��RP�����жϣ�����<n>ֵ���Զ����û�0�������ټ���ʹ�ܡ�
     *             2��һֱʹ�ܡ���ǰһ�����Ͷ��ŵķ��غͺ�һ�����Ͷ��ŵļ��С��1s��5s�������ȡֵȡ����MT��ʵ�֣���RP���ӱ��ֲ��ϡ���������1s��5sʱ��RP�����жϣ�����<n>ֵ�Ա���Ϊ2��������ʹ�ܡ�
     * [ʾ��]:
     *     �� ���ÿ��ƶ����м�Э�飨RP��������ӵ������Թ���һֱʹ��
     *       AT+CMMS=2
     *       OK
     *     �� ��ѯ��ǰ��ʹ��״̬
     *       AT+CMMS?
     *       +CMMS: 2
     *       OK
     *     �� ����CMMS
     *       AT+CMMS=?
     *       +CMMS: (0-2)
     *       OK
     */
    { AT_CMD_CMMS,
      At_SetCmmsPara, AT_SET_PARA_TIME, At_QryCmmsPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMMS", (VOS_UINT8 *)"(0-2)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ö��Ÿ�ʽ
     * [˵��]: ���ö��Ų��õĸ�ʽ����ʽ������ģʽ����<mode>�����������ֱ���PDUģʽ��textģʽ��
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CMGF=[<mode>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMGF?
     *     [���]: <CR><LF>+CMGF: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CMGF=?
     *     [���]: <CR><LF>+CMGF: (list of supported <mode>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ�����Ÿ�ʽ��Ĭ��ֵΪ0��
     *             0��PDUģʽ��
     *             1���ı�ģʽ��
     * [ʾ��]:
     *     �� ���ö��Ÿ�ʽΪ�ı�ģʽ
     *       AT+CMGF=1
     *       OK
     *     �� ��ѯ���Ÿ�ʽ
     *       AT+CMGF?
     *       +CMGF: 1
     *       OK
     *     �� ����CMGF
     *       AT+CMGF=?
     *       +CMGF: (0,1)
     *       OK
     */
    { AT_CMD_CMGF,
      At_SetCmgfPara, AT_NOT_SET_TIME, At_QryCmgfPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CMGF", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ö��ŷ�����
     * [˵��]: ���÷��Ͷ���ʱ�ķ��������ѡ������
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CGSMS=[<service>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGSMS?
     *     [���]: <CR><LF>+CGSMS: <service><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGSMS=?
     *     [���]: <CR><LF>+CGSMS: (list of currently available <service>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <service>: ����ֵ��ѡ�������Ĭ��ֵΪ1��
     *             0��ֻѡ��PS��
     *             1��ֻѡ��CS��
     *             2������ѡ��PS��
     *             3������ѡ��CS��
     *             ע�⣺
     *             Ϊ��߶��ŷ��ͳɹ��ʣ�ʵ���������£�
     *             0��2������ѡ��PS��
     *             1��3������ѡ��CS��
     * [ʾ��]:
     *     �� ��������ѡ��CS���Ͷ���
     *       AT+CGSMS=3
     *       OK
     *     �� ��ѯ���ŷ�����
     *       AT+CGSMS?
     *       +CGSMS: 3
     *       OK
     *     �� ����CGSMS
     *       AT+CGSMS=?
     *       +CGSMS: (0-3)
     *       OK
     */
    { AT_CMD_CGSMS,
      At_SetCgsmsPara, AT_SET_PARA_TIME, At_QryCgsmsPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGSMS", (VOS_UINT8 *)"(0-3)" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ö������ĵ�ַ
     * [˵��]: ���ö������ĵ�ַ��
     *         ���������3GPP TS 27.005Э�顣
     *         �ڲ�ͬģʽ�´�����Ĺ������£�
     *         �ı�ģʽ�£�д���źͷ���������ʹ�ø��������õĶ������ĵ�ַ��
     *         PDUģʽ�£�����<pdu>�����ж������ĵ�ַ����Ϊ0ʱ��ʹ�ø��������ö������ĵ�ַ��
     * [�﷨]:
     *     [����]: +CSCA=<sca>[,<tosca>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CSCA?
     *     [���]: <CR><LF>+CSCA: <sca>,<tosca><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CSCA=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� ���ö������ĵ�ַ
     *       AT+CSCA="13800000000"
     *       OK
     *     �� ��ѯ�������ĵ�ַ
     *       AT+CSCA?
     *       +CSCA: "13800000000",129
     *       OK
     *     �� ����CSCA
     *       AT+CSCA=?
     *       OK
     */
    { AT_CMD_CSCA,
      At_SetCscaPara, AT_SET_PARA_TIME, At_QryCscaPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CSCA", (VOS_UINT8 *)"(sca),(0-255)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ö��ŷ�������
     * [˵��]: ���ö��ŷ������͡�
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CSMS=<service>
     *     [���]: <CR><LF>+CSMS: <mt>,<mo>,<bm><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CSMS?
     *     [���]: <CR><LF>+CSMS: <service>,<mt>,<mo>,<bm><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CSMS=?
     *     [���]: <CR><LF>+CSMS: (list of supported <service>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <service>: ����ֵ����Ϣ�������ͣ�Ĭ��ֵΪ1��
     *             0��3G TS 23.040��3G TS 23.041 Phase 2�汾��
     *             1��3G TS 23.040��3G TS 23.041 Phase 2+�汾��
     *     <mt>: ����ֵ��ָʾMT�Ƿ�֧�ֽ��ն��š�
     *             0����֧�֣�
     *             1��֧�֡�
     *     <mo>: ����ֵ��ָʾMT�Ƿ�֧�ַ��Ͷ���
     *             0����֧�֣�
     *             1��֧�֡�
     *     <bm>: ����ֵ��ָʾMT�Ƿ�֧�ֹ㲥��Ϣ����
     *             0����֧�֣�
     *             1��֧�֡�
     * [ʾ��]:
     *     �� ���ö��ŷ�������ΪPhase 2+�汾
     *       AT+CSMS=1
     *       +CSMS: 1,1,0
     *       OK
     *     �� ��ѯ���ŷ�������
     *       AT+CSMS?
     *       +CSMS: 1,1,1,1
     *       OK
     *     �� ����CSMS
     *       AT+CSMS=?
     *       +CSMS: (0,1)
     *       OK
     */
    { AT_CMD_CSMS,
      At_SetCsmsPara, AT_SET_PARA_TIME, At_QryCsmsPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CSMS", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ı�ģʽ����
     * [˵��]: �ı�ģʽ�����÷��ͺʹ洢���ŵ���ز�������Ч�����������ͣ�
     *         ���ʱ�䣨<vp>��ȡֵ��ΧΪ0��255��
     *         ����ʱ�䣨<vp>Ϊ�ַ������ͣ�
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CSMP?
     *     [���]: <CR><LF>+CSMP: <fo>,<vp>,<pid>,<dcs><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CSMP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <fo>: TPDU�ĵ�һ���ֽڡ�
     *             ���ڲ�ͬ����Ϣ���ͣ�<fo>�ĺ������£�
     *             ������Ϣ����ΪSMS-SUBMIT���ͣ�<fo>������μ���2-12��
     *             ������Ϣ����ΪSMS-COMMAND���ͣ�<fo>������μ���2-14��
     *     <����>: ��μ�2.5.1 ���Ų����������˵����
     * [��]: ��Ϣ����ΪSMS-submitʱ<fo>����
     *       Abbr.,   Reference,                     P��note 1��, P��note 2��, Description,
     *       TP-MTI,  TP-Message-Type-Indicator,     M,           2b,          TP����Ϣ���͡�,
     *       TP-RD,   TP-Reject-Duplicates,          M,           b,           ָʾSC�Ƿ���Ҫ����һ���ڶ��������б���Ķ��ţ����������뱣���ڶ������ĵĶ�������ͬ��TP-OA��TP-MR��TP-DA��,
     *       TP-VPF,  TP-Validity-Period-Format,     M,           2b,          ָʾTP-VP�Ƿ���Ч��,
     *       TP-RP,   TP-Reply-Path,                 M,           b,           ָʾ�Ƿ�����ظ�·����,
     *       TP-UDHI, TP-User-Data-Header-Indicator, O,           b,           ָʾTP-UD��һ��ͷ��,
     *       TP-SRR,  TP-Status-Report-Request,      O,           b,           ָʾ�Ƿ��������״̬���档,
     * [��]: bit[7:0]��������
     *       b7,    b6,      b5,     b4,     b3,     b2,    b1,     b0,
     *       TP-RP, TP-UDHI, TP-SRR, TP-VPF, TP-VPF, TP-RD, TP-MTI, TP-MTI,
     * [��]: ��Ϣ����ΪSMS-COMMANDʱ<fo>����
     *       Abbr.,   Reference,                      P��note 1��, R��note 2��, Description,
     *       TP-MTI,  TP-Message-Type-Indicator,      M,           2b,          TP-��Ϣ���͡�,
     *       TP-UDHI, TP-User-Data-Header-Indication, O,           b,           ָʾTP-UD��һ��ͷ��,
     *       TP-SRR,  TP-Status-Report-Request,       O,           b,           ָʾ�Ƿ��������״̬���档,
     * [��]: bit[7:0]��������
     *       b7, b6,      b5,     b4, b3, b2, b1,     b0,
     *       0,  TP-UDHI, TP-SRR, 0,  0,  0,  TP-MTI, TP-MTI,
     * [ʾ��]:
     *     �� ������Ч��Ϊ���ʱ���ʽ�����ұ�������ΪGSM 7bit
     *       AT+CSMP=17,255,0,0
     *       OK
     *     �� ������Ч��Ϊ����ʱ���ʽ�����ұ�������ΪUNICODE
     *       AT+CSMP=25,"94/05/06,22:10:00+08",0,8
     *       OK
     *     �� ��ѯ�ı�ģʽ����
     *       AT+CSMP?
     *       +CSMP: 17,255,0,0
     *       OK
     *     �� ����CSMP
     *       AT+CSMP=?
     *       OK
     */
    { AT_CMD_CSMP,
      At_SetCsmpPara, AT_SET_PARA_TIME, At_QryCsmpPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CSMP", TAF_NULL_PTR },
#if (NAS_FEATURE_SMS_FLASH_SMSEXIST == FEATURE_ON)
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���ö��Ŵ洢��
     * [˵��]: ���ö��Ŷ���ɾ��д�����ͽ��ղ����ֱ��Ӧ�Ĵ洢����
     *         CLģʽ�¸����֧�֡�
     *         �ֻ���̬��֧�ֽ��洢��������ΪME��
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CPMS=<mem1>[,<mem2>[,<mem3>]]
     *     [���]: <CR><LF>+CPMS: <used1>,<total1>,<used2>,<total2>,<used3>,<total3><CR><LF> <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CPMS?
     *     [���]: <CR><LF>+CPMS: <mem1>,<used1>,<total1>,<mem2>,<used2>,<total2>,<mem3>, <used3>,<total3><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: +CPMS=?
     *     [���]: <CR><LF>+CPMS: (list of supported <mem1>s),(list of supported <mem2>s), (list of supported <mem3>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mem1>: �ַ���ֵ����ʾ���Ŷ�ȡ��ɾ���������õĽ��ʡ���ѡֵΪ��
     *             ��SM������ʾ(U)SIM������ME������ʾFLASH�洢��
     *     <mem2>: �ַ���ֵ����ʾ����д��ͷ��Ͳ������õĽ��ʡ���ѡֵͬ<mem1>��
     *     <mem3>: �ַ���ֵ����ʾ���ղ������õĽ��ʡ���ѡֵͬ<mem1>��
     *     <total1>: ����ֵ����ʾ<mem1>�洢���ŵ�������
     *     <total2>: ����ֵ����ʾ<mem2>�洢���ŵ�������
     *     <total3>: ����ֵ����ʾ<mem3>�洢���ŵ�������
     *     <used1>: ����ֵ����ʾ<mem1>�����еĶ�����Ŀ��
     *     <used2>: ����ֵ����ʾ<mem2>�����еĶ�����Ŀ��
     *     <used3>: ����ֵ����ʾ<mem3>�����еĶ�����Ŀ��
     * [ʾ��]:
     *     �� ���ö��Ų����洢��Ϊ(U)SIM
     *       AT+CPMS="SM","SM","SM"
     *       +CPMS: 0,16,0,16,0,16
     *       OK
     *     �� ��ѯ���Ų����洢��
     *       AT+CPMS?
     *       +CPMS: "SM",0,16,"SM",0,16,"SM",0,16
     *       OK
     *       ����CPMS
     *       ����NV���ö���ͬ��
     *     �� ֧��ME�洢
     *       AT+CPMS=?
     *       +CPMS: ("SM","ME"), ("SM","ME"), ("SM","ME")
     *       OK
     *     �� ��֧��ME�洢
     *       AT+CPMS=?
     *       +CPMS: ("SM"), ("SM"), ("SM")
     *       OK
     */
    { AT_CMD_CPMS,
      At_SetCpmsPara, AT_SET_PARA_TIME, At_QryCpmsPara, AT_QRY_PARA_TIME, AT_TestCpmsPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CPMS", (VOS_UINT8 *)"(\"SM\",\"ME\"),(\"SM\",\"ME\"),(\"SM\",\"ME\")" },
#else

    { AT_CMD_CPMS,
      At_SetCpmsPara, AT_SET_PARA_TIME, At_QryCpmsPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CPMS", (VOS_UINT8 *)"(\"SM\"),(\"SM\"),(\"SM\")" },
#endif

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ɾ������
     * [˵��]: �Ӵ洢����mem1����ɾ�����ţ����<delflag>�����Ҳ�Ϊ0�������<index>��
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: +CMGD=<index>[,<delflag>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CMGD=?
     *     [���]: <CR><LF>+CMGD: ([list of supported <index>s]),(list of supported <delflag>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <delflag>: ����ֵ��ɾ����ʽ��Ĭ��Ϊ0��
     *             0��ɾ��<index>ָ���Ķ��ţ�
     *             1��ɾ����ǰ�洢�������е��Ѷ����ţ�
     *             2��ɾ����ǰ�洢�������е��Ѷ����ѷ����ţ�
     *             3��ɾ����ǰ�洢�������е��Ѷ����ѷ���δ�����ţ�
     *             4��ɾ����ǰ�洢�������ж��š�
     *     <����>: ��μ�2.5.1 ���Ų����������˵����
     * [ʾ��]:
     *     �� ɾ����ǰ�洢����һ������
     *       AT+CMGD=0
     *       OK
     *     �� ����CMGD�����赱ǰ�洢��������Ϊ0��1��2��λ�ô��ж��ţ�
     *       AT+CMGD=?
     *       +CMGD: (0,1,2),(0-4)
     *       OK
     */
    { AT_CMD_CMGD,
      At_SetCmgdPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_QryCmgdPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMGD", (VOS_UINT8 *)"(0-254),(0-4)" },
#if ((FEATURE_GCBS == FEATURE_ON) || (FEATURE_WCBS == FEATURE_ON))

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ѡ��С����Ϣ����
     * [˵��]: PC�ͻ����ô���������С���㲥���ķ�ʽ��
     *         CLģʽ�¸����֧�֡�
     *         ���������3GPP TS 27.005Э�顣
     *         ʹ��+CNMI�����<bm>��������PC�ͻ��˽�����Ϣ�ķ���������ΪMT�����+CBM�����PDUģʽ�������ϱ���Ϣ��
     * [�﷨]:
     *     [����]: +CSCB=[<mode>
     *             [,<mids>[,<dcss>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CMS ERROR: <err><CR><LF>
     *     [����]: +CSCB?
     *     [���]: <CR><LF>+CSCB: <mode>,<mids>,<dcss><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CSCB=?
     *     [���]: <CR><LF>+CSCB: (list of supported <mode>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ����Ϣ���ͽ��շ�ʽ��Ĭ��Ϊ0��
     *             0������<mids>��<dcss>��ָ������Ϣ���ͣ�
     *             1��������<mids>��<dcss>��ָ������Ϣ���͡�
     *     <mids>: �ַ������ͣ����յ�������ϢID�б��磺"0,1,5,320-478,922"
     *     <dcss>: �ַ������ͣ����յ����б���DCS�б��磺"0-3,5"
     * [ʾ��]:
     *     �� �����û�ѡ��С����Ϣ���͵Ľ��շ�ʽΪ����<mids>��<dcss>��ָ������Ϣ����
     *       AT+CSCB=0
     *       OK
     *     �� ��ѯ��ǰ�û�ѡ��С����Ϣ���ͣ�Ŀǰ��ѯ���ص�ֻ���ǽ����б�
     *       AT+CSCB?
     *       +CSCB: 0,"0-65535",""
     *       OK
     *     �� ����CSCB����
     *       AT+CSCB=?
     *       +CSCB: (0,1)
     *       OK
     */
    { AT_CMD_CSCB,
      At_SetCscbPara, AT_SET_PARA_TIME, At_QryCscbPara, AT_QRY_PARA_TIME, AT_TestCscbPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CMS_OPERATION_NOT_ALLOWED, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"+CSCB", (VOS_UINT8 *)"(0,1),(MIDS),(DCSS)" },
#endif

    /* ��������AT���� */
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ������Ͳ����
     * [˵��]: ����ͨ��ʱ��Ͳ����������������
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CLVL=<level>
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CLVL?
     *     [���]: <CR><LF>+CLVL: <level><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CLVL=?
     *     [���]: <CR><LF>+CLVL: (list of supported <level>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <level>: ����ֵ�������ļ���
     *             0��5��0Ϊ��С������5Ϊ���������
     * [ʾ��]:
     *     �� ������������Ϊ3
     *       AT+CLVL=3
     *       OK
     *     �� ��ѯ��������
     *       AT+CLVL?
     *       +CLVL: 3
     *       OK
     *     �� ����CLVL
     *       AT+CLVL=?
     *       +CLVL: (0-5)
     *       OK
     */
    { AT_CMD_CLVL,
      At_SetClvlPara, AT_SET_VC_PARA_TIME, At_QryClvlPara, AT_QRY_VC_PARA_TIME, AT_TestClvlPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CLVL", (VOS_UINT8 *)"(0-12)" },

    /* CS */
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �Ҷϵ�ǰ���к���
     * [˵��]: �Ҷϵ�ǰ���еĺ��С�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CHUP
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: +CHUP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� �Ҷϵ�ǰ���еĺ���
     *       AT+CHUP
     *       OK
     *     �� ����CHUP
     *       AT+CHUP=?
     *       OK
     */
    { AT_CMD_CHUP,
      At_SetChupPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CHUP", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �г���ǰ������Ϣ
     * [˵��]: �г���ǰ���еĺ��С�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CLCC
     *     [���]: <CR><LF> [+CLCC: <id1>,<dir>,<state>,<mode>,<mpty>[,<number>
     *             ,<type>[,<alpha>[,<priority>[,<CLI validity>]]]]
     *             [<CR><LF>+CLCC: < id2>,<dir>,<stat>,<mode>,<mpty>[,
     *             <number>,<type>[,<alpha>[,<priority>[,<CLI validity>]]]] <CR><LF>[...]]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CLCC=?
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
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <alpha>: �����ڵ绰���ж�Ӧ����������֧�֡�
     *     <priority>: ����ֵ����ʾ���е�eMLPP���ȼ����ݲ�֧��
     *     <CLI validity>: CLI��Ч�ԣ��ݲ�֧�֣�
     *             1��CLI��Ч��
     *             2���������ѱ���CLI��
     *             3���������з�Ϊ���ѵ绰���ͣ�����޷�ʹ��CLI��
     *             4������ԭ��CLI�����á�
     * [ʾ��]:
     *     �� ��ʾ��ǰ������Ϣ
     *       AT+CLCC?
     *       +CLCC: 1,0,0,0,0,0,0,0,"18888888888",129
     *       OK
     *     �� ����CLCC
     *       AT+CLCC=?
     *       OK
     */
    { AT_CMD_CLCC,
      At_SetClccPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CLCC", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯMT��ǰ״̬
     * [˵��]: ��ѯMT��ǰ״̬��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CPAS
     *     [���]: <CR><LF>+CPAS: <pas><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPAS=?
     *     [���]: <CR><LF>+CPAS: (list of supported <pas>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <pas>: ����ֵ��MT�Ļ״̬��
     *             0��������TE������MT���������
     *             1�������ã�MT�ܾ�TE�������
     *             2��δ֪��MT����֤������������Ӧ����
     *             3�����壨�������壬����MT����ִ��TE�������
     *             4�����н����У��к������ڽ��У�����MT����ִ��TE�������
     *             5�����ߣ�����״̬��MT���ܴ���TE�������
     * [ʾ��]:
     *     �� ��ȡMT��ǰ״̬
     *       AT+CPAS
     *       +CPAS: 0
     *       OK
     *     �� ����CPAS
     *       AT+CPAS=?
     *       +CPAS: (0-5)
     *       OK
     */
    { AT_CMD_CPAS,
      At_SetCpasPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestCpasPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CPAS", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ���ó�������
     * [˵��]: ���ó������͡�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CBST=[<speed>[,<name>[,<ce>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: +CBST?
     *     [���]: <CR><LF>+CBST: <speed>,<name>,<ce><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CBST=?
     *     [���]: <CR><LF>+CBST: (list of supported <speed>s),(list of supported <name>s),(list of supported <ce>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <speed>: ����ֵ�����ʡ�
     *             134��64000 bps (multimedia)��
     *     <name>: ����ֵ��ҵ�����͡�
     *             1��data circuit synchronous (UDI or 3.1 kHz modem)��
     *     <ce>: ����ֵ���������͡�
     *             0��transparent��
     * [ʾ��]:
     *     �� ���óɿ��ӵ绰����
     *       AT+CBST=134,1,0
     *       OK
     *     �� ��ѯ��������
     *       AT+CBST?
     *       +CBST: 134,1,0
     *       OK
     *     �� ����CBST
     *       AT+CBST=?
     *       +CBST: (134),(1),(0)
     *       OK
     */
    { AT_CMD_CBST,
      At_SetCbstPara, AT_NOT_SET_TIME, At_QryCbstPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CBST", (VOS_UINT8 *)"(134),(1),(0)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ���ú���ģʽ
     * [˵��]: ���ú���ģʽ��
     *         ���������3GPP TS 27.007Э�顣
     *         ����ģʽ��Ϊ��һ�ͽ�������ģʽ��
     *         ��һģʽ��ָ��һ��������ֻ��֧��һ�ֻ���ҵ����������ҵ���һ�֣���
     *         ����ģʽ��ָ��һ�������п���֧�����ֻ���ҵ�������ʹ���ҵ������У���
     *         Ŀǰ��֧�ֵ�һģʽ���С�
     * [�﷨]:
     *     [����]: +CMOD=[<mode>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CMOD?
     *     [���]: <CR><LF>+CMOD: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CMOD=?
     *     [���]: <CR><LF>+CMOD: (list of supported <mode>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������ģʽ��Ĭ��ֵΪ0��
     *             0��single mode��
     * [ʾ��]:
     *     �� ���ú���ģʽΪ��һģʽ
     *       AT+CMOD=0
     *       OK
     *     �� ��ѯ����ģʽ
     *       AT+CMOD?
     *       +CMOD: 0
     *       OK
     *     �� ����CMOD
     *       AT+CMOD=?
     *       +CMOD: (0)
     *       OK
     */
    { AT_CMD_CMOD,
      At_SetCmodPara, AT_NOT_SET_TIME, At_QryCmodPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CMOD", (VOS_UINT8 *)"(0)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ѡ���ַ����
     * [˵��]: ѡ�������ʱ�ı��к���ĺ������͡�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CSTA=<type>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: +CSTA?
     *     [���]: <CR><LF>+CSTA: <type><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CSTA=?
     *     [���]: <CR><LF>+CSTA: (list of supported <type>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     * [ʾ��]:
     *     �� ���õ�ַ����
     *       AT+CSTA=129
     *       OK
     *     �� ��ѯ��ַ����
     *       AT+CSTA?
     *       +CSTA: 129
     *       OK
     *     �� ���Ե�ַ����
     *       AT+CSTA=?
     *       +CSTA: (0-255)
     *       OK
     *     �� ��������
     *       AT+CSTA=
     *       +CME ERROR: Incorrect parameters
     */
    { AT_CMD_CSTA,
      At_SetCstaPara, AT_NOT_SET_TIME, At_QryCstaPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CSTA", (VOS_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ͨ���з���DTMF��
     * [˵��]: ����������ͨ���������෢��DTMF��Dual Tone Multi-Frequency��������FEATURE_MBB_CUST���ʱ�ɴ�������෢��DTMF����
     *         ���������3GPP TS 27.007Э�顣
     *         �������ݲ��ṩ��Ӧ��ʹ�á�
     * [�﷨]:
     *     [����]: +VTS=<DTMF>[,<duration>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +VTS=?
     *     [���]: <CR><LF>+VTS: (0-9,A-D,*,#),(1-255)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <DTMF>: ��Ҫ������һ��ASCII�ַ���ȡֵ��ΧΪ0��9��#��*��A��D��
     *     <duration>: ����DTMF����ʱ����ȡֵ��ΧΪ1-255��
     *             ����<duration>����Ĭ��ʱ��100ms��
     *             ����<duration>����ʱ��Ϊduration * 100ms��
     * [ʾ��]:
     *     �� ����DTMF
     *       AT+VTS=A
     *       OK
     *     �� ����VTS
     *       AT+VTS=?
     *       +VTS: (0-9,A-D,*,#),(1-255)
     *       OK
     */
    { AT_CMD_VTS,
      At_SetVtsPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestVtsPara, AT_NOT_SET_TIME,
      AT_AbortVtsPara, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+VTS", (VOS_UINT8 *)"(@key),(1-255)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ʹ��/��ֹ����ָʾ��չ�ϱ���ʽ
     * [˵��]: ѡ�����ָʾ�����緢���PDP��Packet Data Protocol����������ʱ���Ƿ�ʹ����չ�ϱ���ʽ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CRC=[<mode>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CRC?
     *     [���]: <CR><LF>+CRC: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CRC=?
     *     [���]: <CR><LF>+CRC: (list of supported <mode>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������򼤻�ָʾ�ϱ���ʽ��Ĭ��ֵΪ0��
     *             0����ֹ��չ��ʽ��ʹ��RING�ϱ���
     *             1��ʹ����չ��ʽ��ʹ��+CRING: <type>�ϱ���
     * [ʾ��]:
     *     �� ʹ���������չ��ʽ
     *       AT+CRC=1
     *       OK
     *     �� ��ѯCRC
     *       AT+CRC?
     *       +CRC: 0
     *       OK
     *     �� ����CRC
     *       AT+CRC=?
     *       +CRC: (0,1)
     *       OK
     */
    { AT_CMD_CRC,
      At_SetCrcPara, AT_NOT_SET_TIME, At_QryCrcPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CRC", (VOS_UINT8 *)"(0,1)" },
#if (FEATURE_MBB_CUST== FEATURE_ON)
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���к�����ʾ
     * [˵��]: ʹ�ܻ��ֹ���к������ʾ�����ʹ�����к�����ʾ������������������ʱ���ϱ����к��롣
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CLIP=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CLIP?
     *     [���]: <CR><LF>+CLIP: <n>,<m><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CLIP=?
     *     [���]: <CR><LF>+CLIP: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ����ֹ��ʹ��+CLIP�������ϱ���Ĭ��ֵΪ0��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     *     <m>: ����ֵ��CLIPҵ���������ǩԼ״̬��
     *             0��CLIPҵ��δ�ṩ��
     *             1��CLIPҵ�����ṩ��
     *             2��δ֪������ԭ�򣩡�
     * [ʾ��]:
     *     �� ʹ�����к�����ʾ
     *       AT+CLIP=1
     *       OK
     *     �� ��ѯ���к�����ʾ
     *       AT+CLIP?
     *       +CLIP: 1,2
     *       OK
     *     �� ����CLIP
     *       AT+CLIP=?
     *       +CLIP: (0,1)
     *       OK
     */
    { AT_CMD_CLIP,
      At_SetClipPara, AT_NOT_SET_TIME, At_QryClipPara, AT_QRY_SS_PARA_TIME, VOS_NULL_PTR,
      AT_NOT_SET_TIME, AT_AbortClipPara, AT_NOT_SET_TIME, AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CLIP", (VOS_UINT8 *)"(0,1)"
    },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���к�������
     * [˵��]: ʹ�ܻ��ֹ���к������ơ����ʹ�����к������Ʋ������������������ʱ��ֹ�򱻽з���ʾ���к��롣
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CLIR=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CLIR?
     *     [���]: <CR><LF>+CLIR: <n>,<m><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CLIR=?
     *     [���]: <CR><LF>+CLIR: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ����ֹ��ʹ��+CLIR���ܣ�Ĭ��ֵΪ0��
     *             0������ǩԼCLIR�ķ�������ʾ��
     *             1��ʹ�ܣ�
     *             2����ֹ��
     *     <m>: ����ֵ��CLIRҵ���������ǩԼ״̬��
     *             0��CLIRҵ��δ�ṩ��
     *             1��CLIRҵ��������ģʽ�ṩ��
     *             2��δ֪������ԭ�򣩣�
     *             3��CLIRҵ����ʱ���ƣ�
     *             4��CLIRҵ����ʱ����
     * [ʾ��]:
     *     �� ʹ�����к�������
     *       AT+CLIR=1
     *       OK
     *     �� ��ѯ���к�������
     *       AT+CLIR?
     *       +CLIR: 1,1
     *       OK
     *     �� ����CLIR
     *       AT+CLIR=?
     *       +CLIR: (0,1,2)
     *       OK
     */
    { AT_CMD_CLIR,
      At_SetClirPara, AT_NOT_SET_TIME, At_QryClirPara, AT_QRY_SS_PARA_TIME, VOS_NULL_PTR,
      AT_NOT_SET_TIME, AT_AbortClirPara, AT_NOT_SET_TIME, AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CLIR", (VOS_UINT8 *)"(0,1,2)"
    },
#else
    { AT_CMD_CLIP,
      At_SetClipPara, AT_NOT_SET_TIME, At_QryClipPara, AT_QRY_SS_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CLIP", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_CLIR,
      At_SetClirPara, AT_NOT_SET_TIME, At_QryClirPara, AT_QRY_SS_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CLIR", (VOS_UINT8 *)"(0,1,2)" },
#endif
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���Ӻ�����ʾ
     * [˵��]: ʹ�ܻ��ֹ���Ӻ������ʾ�����ʹ�����Ӻ�����ʾ�������������������к������ӽ������ʱ��ʾʵ�����ӵĶԷ����롣
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +COLP=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +COLP?
     *     [���]: <CR><LF>+COLP: <n>,<m><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +COLP=?
     *     [���]: <CR><LF>+COLP: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ����ֹ��ʹ��+COLP�������ϱ���Ĭ��ֵΪ0��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     *     <m>: ����ֵ����ʾCOLPҵ���������ǩԼ״̬��
     *             0��COLPҵ��δ�ṩ��
     *             1��COLPҵ�����ṩ��
     *             2��δ֪������ԭ�򣩡�
     * [ʾ��]:
     *     �� ʹ�����Ӻ�����ʾ
     *       AT+COLP=1
     *       OK
     *     �� ��ѯ���Ӻ�����ʾ
     *       AT+COLP?
     *       +COLP: 1,1
     *       OK
     *     �� ����COLP
     *       AT+COLP=?
     *       +COLP: (0,1)
     *       OK
     */
    { AT_CMD_COLP,
      At_SetColpPara, AT_NOT_SET_TIME, At_QryColpPara, AT_QRY_SS_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+COLP", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �պ��û�Ⱥ
     * [˵��]: ����պ��û�Ⱥ��CUG������ҵ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CCUG=[<n>[,<index>[,<info>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CCUG?
     *     [���]: <CR><LF>+CCUG: <n>,<index>,<info><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CCUG=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ����ֹ��ʹ��CUGģʽ��Ĭ��ֵΪ0��
     *             0����ֹCUGģʽ��
     *             1��ʹ��CUGģʽ��
     *     <index>: ����ֵ���պ��û�Ⱥ������ֵ��Ĭ��ֵΪ0��
     *             0��9��Ⱥ�������ֵ��
     *             10�����������û�������ǩԼ����ѡ��Ⱥ�飩��
     *     <info>: ����ֵ��Ĭ��ֵΪ0��
     *             0������Ϣ��
     *             1����ֹOA��Outgoing Access����
     *             2����ֹ��ѡȺ�飻
     *             3����ֹOA����ѡȺ�顣
     * [ʾ��]:
     *     �� ���ñպ��û�Ⱥҵ��
     *       AT+CCUG=1,2,2
     *       OK
     *     �� ��ѯ�պ��û�Ⱥҵ��
     *       AT+CCUG?
     *       +CCUG: 1,2,2
     *       OK
     *     �� ����CCUG
     *       AT+CCUG=?
     *       OK
     */
    { AT_CMD_CCUG,
      At_SetCcugPara, AT_NOT_SET_TIME, At_QryCcugPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CCUG", (VOS_UINT8 *)"(0,1),(0-10),(0-3)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����ҵ��֪ͨ
     * [˵��]: ʹ�ܻ��ֹ����ҵ���֪ͨ������ҵ��֪ͨ����+CSSI��+CSSU���֡�
     *         +CSSI��ָ�ڷ������еĹ������յ���֪ͨ��
     *         +CSSU��ָ�ڱ���ʱ��ͨ���������յ���֪ͨ��
     *         CLģʽ�¸����֧�֡�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CSSN=[<n>[,<m>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CSSN?
     *     [���]: <CR><LF>+CSSN: <n>,<m><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CSSN=?
     *     [���]: <CR><LF>+CSSN: (list of supported <n>s),( list of supported <m>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ����ֹ��ʹ��+CSSI�Ĳ���ҵ��֪ͨ��Ĭ��ֵΪ0��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     *     <m>: ����ֵ����ֹ��ʹ��+CSSU�Ĳ���ҵ��֪ͨ��Ĭ��ֵΪ0��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     * [ʾ��]:
     *     �� ʹ��CSSI��CSSU�ϱ�
     *       AT+CSSN=1,1
     *       OK
     *     �� ��ѯCSSI��CSSU�ϱ�
     *       AT+CSSN?
     *       +CSSN: 1,1
     *       OK
     *     �� ����CSSN
     *       AT+CSSN=?
     *       +CSSN: (0,1),(0,1)
     *       OK
     */
    { AT_CMD_CSSN,
      At_SetCssnPara, AT_SET_PARA_TIME, At_QryCssnPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CSSN", (VOS_UINT8 *)"(0,1),(0,1)" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ������ز���ҵ��
     * [˵��]: ͨ�����������Կ������в���ҵ��
     *         ���б��ֺͻָ�
     *         �෽ͨ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CHLD=<n>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CHLD=?
     *     [���]: <CR><LF>+CHLD: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ�������롣
     *             0���Ҷ����б����ֵĻ��ߵȴ��ĺ��У�
     *             1���Ҷ�����ͨ���ĺ��У��������ȴ��ĺ��л��߻ָ������ֵĺ��У�
     *             2����������ͨ���ĺ��У��������ȴ��ĺ��л��߻ָ������ֵĺ��У�
     *             3�������෽ͨ����
     *             1x���Ҷϵ�x�����У�
     *             2x�����ֳ���x����������������������У�
     *             4�������ֵĺ��к͵�ǰ�����罨��ͨ�������з��������У�������ͨ���ĺ��к͵ȴ��ĺ��н���ͨ�������з��������У�
     *             5�������������緢��ĺ��У�CCBS����
     *             ˵��
     *             ������һֱ����ֱ�������ͷţ��µĺ����ֻ�ʹ�õ�ǰcallid+1����š�
     *             �ڱ��ֵĺ��к͵ȴ��ĺ���ͬʱ���ڵ�����£���������ͻ��ʱ����������ֻ�Եȴ��ĺ��������á�
     * [ʾ��]:
     *     �� ��ѯ����֧�ֵĲ���<n>
     *       AT+CHLD=?
     *       +CHLD: (0,1,1x,2,2x,3,4,5)
     *       OK
     *     �� ���ֳ���3�����������������������
     *       AT+CHLD=23
     *       OK
     */
    { AT_CMD_CHLD,
      At_SetChldPara, AT_SET_CALL_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestChldPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CHLD", (VOS_UINT8 *)"(0-5,11-19,21-29)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �û��������1
     * [˵��]: ���������ڿ����û��������1��UUS1��������ɲο�Э��3G 22.087��
     *         ��Ϣ���ͺ�UUIEͬʱ���ڣ�������Ҫ�������Ϣ���͵��û��������1��
     *         ��������Ϣ���ͣ�UUIE��Ϣ�����ڣ�������Ҫȥ�������Ϣ���͵��û��������1��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CUUS1=[<n>[,<m>[,<message>[,<UUIE>
     *             [,<message>[,<UUIE>[,...]]]]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CUUS1?
     *     [���]: <CR><LF>+ CUUS1: <n>,<m>[,<message>,<UUIE>
     *             [,<message>,<UUIE>[,...]]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CUUS1=?
     *     [���]: <CR><LF>+ CUUS1: (list of supported <n>s), (list of supported <m>s), (list of supported <message>s), (list of supported <messageI>s),
     *             (list of supported <messageU>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����+CUUS1I�Ľ�����Ƿ��ϱ���Ĭ��ֵΪ0��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     *     <m>: ����+CUUS1U�Ľ�����Ƿ��ϱ���Ĭ��ֵΪ0��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     *     <message>: ������Ϣ�а���User-to-User Information Element����Ϣ���͡�
     *             0��ANY����ͬ��SETUP��
     *             1��SETUP��
     *             2��ALERT��
     *             3��CONNECT��
     *             4��DISCONNECT��
     *             5��RELEASE��
     *             6��RELEASE_COMPLETE��
     *     <UUIE>: 16�����ַ�����ͬ��Э��24008�涨��User-to-User Information ElementЭ���ʽ��
     *     <messageI>: +CUUS1I�Ľ�����ϱ�����Ϣ���͡�
     *             0��ANY��
     *             1��ALERT��
     *             2��PROGRESS��
     *             3��CONNECT (sent after +COLP if enabled)��
     *             4��RELEASE��
     *     <messageU>: +CUUS1U�Ľ�����ϱ�����Ϣ���͡�
     *             0��ANY��
     *             1��SETUP (returned after +CLIP if presented, otherwise after every RING or +CRING)��
     *             2��DISCONNECT��
     *             3��RELEASE_COMPLETE��
     * [ʾ��]:
     *     �� ����SETUP��Ϣ���û��������1��ͬʱʹ���ϱ�+CUUS1I��+CUUS1U�Ľ����
     *       AT+CUUS1=1,1,1,7e0122
     *       OK
     *       ������������ɺ�ÿ�η��������SETUP��Ϣ�б�Ȼ��Я����UUIE���ݡ�
     *     �� ȥ����CONNECT��Ϣ���û��������1��ͬʱ��ֹ�ϱ�+CUUS1I��+CUUS1U�Ľ����
     *       AT+CUUS1=0,0,3
     *       OK
     *       ������������ɺ�ÿ�η��������CONNECT��Ϣ�в���Я��UUIE���ݡ�
     *     �� ��ѯ�Ѽ���CUUS1Iҵ�����Ϣ�����Լ�+CUUS1I��+CUUS1U���ϱ�ʹ��
     *       AT+CUUS1?
     *       +CUUS1: 1,1,1,7e0110,4,7e022020
     *       OK
     *       ������ǰSETUP��DISCONNECT��Ϣ�м�����UUS1��Ϣ��
     *     �� ����CUUS1
     *       AT+CUUS1=?
     *       +CUUS1: (0,1),(0,1),(0-6),(0-4),(0-3)
     *       OK
     */
    { AT_CMD_CUUS1,
      At_SetCuus1Para, AT_SET_PARA_TIME, At_QryCuus1Para, AT_QRY_PARA_TIME, At_TestCuus1Para, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CUUS1", (VOS_UINT8 *)"(0,1),(0,1),(0-6)" },

    /* SSA */
#if (FEATURE_MBB_CUST == FEATURE_ON)
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ���еȴ�
     * [˵��]: ����ҵ����еȴ��Ĳ������������ȥ�����״̬��ѯ�����еȴ�ҵ�񱻼������ǰͨ��������������ʱ���ϱ����еȴ���Ϣ��
     *         CLģʽ�¸����֧�֡�
     *         ���������3GPP TS 27.007Э�顣
     *         ״̬��ѯ���̿��Ա���ϡ�
     * [�﷨]:
     *     [����]: +CCWA=[<n>[,<mode>][,<class>]]
     *     [���]: when <mode>=2 and command successful
     *             <CR><LF>+CCWA: <status>,<class1>[,<ss-status>]
     *             [<CR><LF>+CCWA: <status>,<class2>[,<ss-status>]
     *             [...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CCWA?
     *     [���]: <CR><LF>+CCWA: <n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CCWA=?
     *     [���]: <CR><LF>+CCWA: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ����ֹ��ʹ�ܽ����������ϱ���Ĭ��ֵΪ0��
     *             0����ֹ��
     *             1��ʹ�ܡ�
     *     <mode>: ����ֵ���������͡�
     *             0��ȥ���
     *             1�����
     *             2��״̬��ѯ��
     *     <class>: ����ֵ��ҵ�����ȡֵΪ����ҵ��������ϣ�Ĭ��ֵΪ255��
     *             1��voice (telephony)��
     *             2��data��
     *             4��fax (facsimile services)��
     *             8��short message service��
     *             16��data circuit sync��
     *             32��data circuit async��
     *             64��dedicated packet access��
     *             128��dedicated PAD access��
     *     <status>: ����ֵ����ǰ���еȴ���ҵ��״̬��
     *             0��δ���
     *             1�����
     *     <ss-status>: ����ֵ��˽���ֶνӿڣ����������ҵ��״̬��������տڿ��ƣ��������¡�
     *             BIT3��ҵ��ɲ���״̬��ҵ�񼤻�״̬��Ч��
     *             1��ҵ���û����ɲ�����
     *             0��ҵ���û��ɲ�����
     *             BIT 2��ǩԼ״̬��
     *             1��ҵ����ǩԼ��
     *             0��ҵ��δǩԼ��
     *             BIT1��ҵ��ע��״̬��
     *             1��ҵ����ע�᣻
     *             0��ҵ��δע�᣻
     *             BIT0��ҵ�񼤻�״̬��
     *             1��ҵ���Ѽ��
     *             0��ҵ��δ���
     * [ʾ��]:
     *     �� �ɹ�������еȴ�
     *       AT+CCWA=1,1,1
     *       OK
     *     �� ��ѯ���еȴ�״̬
     *       AT+CCWA=1,2,1
     *       +CCWA: 1,1
     *       OK
     *     �� ��ѯ���еȴ��Ƿ�ʹ��
     *       AT+CCWA?
     *       +CCWA: 1
     *       OK
     *     �� ����CCWA
     *       AT+CCWA=?
     *       +CCWA: (0,1)
     *       OK
     */
    { AT_CMD_CCWA,
      At_SetCcwaPara, AT_SET_SS_PARA_TIME, At_QryCcwaPara, AT_NOT_SET_TIME, At_TestCcwaPara,
      AT_NOT_SET_TIME, AT_AbortCcwaPara, AT_NOT_SET_TIME, AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CCWA", (VOS_UINT8 *)"(0,1),(0-2),(1-255)"
    },
#else
    { AT_CMD_CCWA,
      At_SetCcwaPara, AT_SET_SS_PARA_TIME, At_QryCcwaPara, AT_NOT_SET_TIME, At_TestCcwaPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CCWA", (VOS_UINT8 *)"(0,1),(0-2),(1-255)" },
#endif
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����ת��
     * [˵��]: ����ҵ�����ת�ƵĲ���������ע�ᡢɾ�������ȥ�����״̬��ѯ��
     *         ���У���ѯʱ<class>����Ϊ��һ�ľ���ҵ�����͡�
     *         CLģʽ�¸����֧�֡�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CCFC=<reason>,<mode>[,<number>[,<type>[,<class>[,<subaddr>[,<satype>[,<time>]]]]]]
     *     [���]: when <mode>=2 and command successful:
     *             <CR><LF>+CCFC: <status>,<class1>[,<number>,<type>[,<subaddr>,<satype>[,<time>]]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CCFC=?
     *     [���]: <CR><LF>+CCFC: (list of supported <reason>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <reason>: ����ֵ������ת�Ƶ����͡�
     *             0��������ת�ƣ�
     *             1����æת�ƣ�
     *             2����Ӧ��ת�ƣ�
     *             3�����ɴ�ת�ƣ������������߹ػ�ʱ����
     *             4�����к���ת�ƣ�
     *             5����������ת�ơ�
     *     <mode>: ����ֵ������ת�ƵĲ���ģʽ��
     *             0��ȥ���
     *             1�����
     *             2��״̬��ѯ����ѯʱ��reason����Ϊ4��5����
     *             3��ע�᣻
     *             4��ɾ����
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <subaddr>: �ַ������ͣ��ӵ�ַ��������˲�����
     *     <satype>: ����ֵ���ӵ�ַ���ͣ�������˲�����
     *     <class>: ����ֵ��ҵ�����ȡֵΪ����ҵ��������ϣ�Ĭ��ֵΪ255��
     *             1��voice (telephony)��
     *             2��data��
     *             4��fax (facsimile services)��
     *             8��short message service��
     *             16��data circuit sync��
     *             32��data circuit async��
     *             64��dedicated packet access��
     *             128��dedicated PAD access��
     *     <time>: ����ֵ�������û��ѯ����Ӧ��ת�ơ�ʱ���˲������Ա�ʾ��ת�ƺ���ǰ�ȴ��೤ʱ�䡣
     *     <status>: ����ֵ��״̬��
     *             0��δ���
     *             1�����
     * [ʾ��]:
     *     �� ע��һ����æת��ҵ��
     *       AT+CCFC=1,3,"12345678",129,1
     *       OK
     *     �� ע��һ����Ӧ��ת��ҵ����������Ӧҵ��
     *       AT+CCFC=2,3,"12345678",129,1
     *       +CME ERROR: 30
     *     �� ��ѯ��æת��ҵ��
     *       AT+CCFC=1,2
     *       +CCFC: 1,1,"+8612345678",145
     *       OK
     *     �� ɾ��һ����æת��ҵ��
     *       AT+CCFC=1,4
     *       OK
     *     �� ����CCFC
     *       AT+CCFC=?
     *       +CCFC: (0-5)
     *       OK
     */
    { AT_CMD_CCFC,
      At_SetCcfcPara, AT_SET_SS_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_TestCcfcPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CCFC", (VOS_UINT8 *)"(0-5),(0-4),(number),(0-255),(1-255),(subaddr),(0-255),(1-30)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: USSDҵ��
     * [˵��]: �ṩ����ҵ��USSD��Unstructured Supplementary Service Data���Ŀ��ƣ�֧�������TE����Ĳ�����
     *         CLģʽ�¸����֧�֡�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CUSD=[<n>[,<str>[,<dcs>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CUSD?
     *     [���]: <CR><LF>+CUSD: <n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CUSD=?
     *     [���]: <CR><LF>+CUSD: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ�����ƽ�����ϱ���Ĭ��ֵΪ0��
     *             0����ֹ������ϱ���
     *             1��ʹ�ܽ�����ϱ���
     *             2���˳��Ự��
     *     <str>: USSD�ַ�������󳤶�Ϊ160���ַ���
     *     <dcs>: ����ֵ�����뷽ʽ��
     *             15��7bit���루Ĭ��7bit���룩��
     *             68��8bit���루������ʹ�ã���
     *             72��UCS2���루������ʹ�ã���
     *             ����͸��ģʽ������Ӧ���·�����23038 CBS DCSЭ���DCS����������0��1��2���ȵȣ�
     * [ʾ��]:
     *     �� ����һ��USSD������7bit���룬ʹ���ϱ�
     *       AT+CUSD=1,"AAD86C3602",15
     *       OK
     *     �� ����һ��USSD�����������κβ�����Ĭ�ϰ���AT+CUSD=0����
     *       AT+CUSD=
     *       OK
     *     �� ��ѯUSSDҵ��
     *       AT+CUSD?
     *       +CUSD: 1
     *       OK
     *     �� �˳��Ự
     *       AT+CUSD=2
     *       OK
     *     �� ����USSD
     *       AT+CUSD=?
     *       +CUSD: (0-2)
     *       OK
     */
    { AT_CMD_CUSD,
      At_SetCusdPara, AT_SS_CUSD_SET_PARA_TIME, At_QryCusdPara, AT_QRY_PARA_TIME, At_TestCusdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CUSD", (VOS_UINT8 *)"(0-2),(str),(0-255)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯ��Ʒ����汾��Ϣ
     * [˵��]: ��2��������ڲ�ѯ��Ʒ����汾��Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +GMR
     *     [���]: <CR><LF><revision><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +GMR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <revision>: �ַ���������汾��Ϣ��
     * [ʾ��]:
     *     �� ��ѯ����汾��Ϣ
     *       AT+CGMR
     *       V100
     *       OK
     *       AT+GMR
     *       V100
     *       OK
     *     �� ����CGMR
     *       AT+CGMR=?
     *       OK
     *       AT+GMR=?
     *       OK
     */
    { AT_CMD_GMR,
      At_SetCgmrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+GMR", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯ��������Ϣ
     * [˵��]: ��2��������ڲ�ѯ��������Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     *         ��2�������ѯ����ܲ�Ʒ�߶���NV8203�أ�Ĭ��Ϊ��Huawei��
     * [�﷨]:
     *     [����]: +GMI
     *     [���]: <CR><LF><manufacturer><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +GMI=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <manufacturer>: �ַ�������������Ϣ��
     * [ʾ��]:
     *     �� ��ѯ��������Ϣ
     *       AT+CGMI
     *       Huawei
     *       OK
     *       AT+GMI
     *       Huawei
     *       OK
     *     �� ����CGMI
     *       AT+CGMI=?
     *       OK
     *       AT+GMI=?
     *       OK
     */
    { AT_CMD_GMI,
      At_SetCgmiPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+GMI", VOS_NULL_PTR },

    /* OTHER */
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����ƫת
     * [˵��]: ����ǰ���������еȴ��ĵ绰ƫת������һ�����롣
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CTFR=<number>[,<type>[,<subaddr>[,<satype>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CTFR=?
     *     [���]: <CR><LF>+CTFR: <number>, (list of supported <type>s),<subaddr>,(list of supported <satype>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: ����ֵ�������ַ���ͣ���ṹ���2-7��ʾ��
     *             Type-of-number��bit[6:4]����ȡֵ���£�
     *             000���û����˽�Ŀ�ĵ�ַ����ʱ��ѡ�ô�ֵ����ʱĿ�ĵ�ַ������������д��
     *             001�����ʺ��롣
     *             010�����ں��롣
     *             011���ض����룬���á�
     *             101��������
     *             110��������
     *             111��������
     *             Numbering-plan-identification��bit[3:0]����ȡֵ���£�
     *             0000������������ĺ��뷽��ȷ����
     *             0001��ISDN/telephone numbering plan��
     *             0011��Data numbering plan��
     *             0100��Telex numbering plan��
     *             1000��National numbering plan��
     *             1001��Private numbering plan��
     *             1010��ERMES numbering plan��
     *             ˵������bit[6:4]ȡֵΪ000��001��010ʱbit[3:0]����Ч��
     *     <subaddr>: �ַ������ͣ��ӵ�ַ�����������
     *     <satype>: ����ֵ���ӵ�ַ���ͣ����������
     * [��]: �����ַ���͵Ľṹ
     *       bit:   7,    6,              5,              4,              3,                             2,                             1,                             0,
     *              1,    Type-of-number, Type-of-number, Type-of-number, Numbering-plan-identification, Numbering-plan-identification, Numbering-plan-identification, Numbering-plan-identification,
     * [ʾ��]:
     *     �� ������ƫת������1234
     *       AT+CTFR="1234",129
     *       OK
     *     �� ����CTFR
     *       AT+CTFR=?
     *       +CTFR: (number),(0-255),(subaddr),(0-255)
     *       OK
     */
    { AT_CMD_CTFR,
      At_SetCtfrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"+CTFR", (VOS_UINT8 *)"(number),(0-255),(subaddr),(0-255)" },

    /*
     * [���]: Э��AT-��ȫ����
     * [����]: �޸�����
     * [˵��]: �����޸�+CLCK������豸�������롣
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CPWD=<fac>,<oldpwd>,<newpwd>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPWD=?
     *     [���]: <CR><LF>+CPWD: list of supported (<fac>,<pwdlength>)s<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <fac>: �ַ������ͣ�ָ������������Ķ���
     *             ��SC����SIM PIN1��
     *             ��P2����SIM PIN2��
     *             ��AO������ֹ���к�����
     *             ��OI������ֹ���й��ʺ�����
     *             ��OX������ֹ���й��ʺ��������������⣻
     *             ��AI������ֹ���к��룻
     *             ��IR������������������ʱ����ֹ���к��룻
     *             ��AB������ֹ����ҵ�񣨽���<mode>=0ʱ���ã���
     *             ��AG������ֹ����ҵ�񣨽���<mode>=0ʱ���ã���
     *             ��AC������ֹ����ҵ�񣨽���<mode>=0ʱ���ã���
     *     <oldpwd>��<newpwd>: �ַ������ͣ�������������롣
     *     <pwdlength>: ����ֵ��<oldpwd>��<newpwd>����󳤶ȡ�
     * [ʾ��]:
     *     �� �ı�����ɹ�
     *       AT+CPWD="AO","1234","5678"
     *       OK
     *     �� �ı�����ʧ��
     *       AT+CPWD="AO","5678","1234"
     *       +CME ERROR: 16
     *     �� ����CPWD
     *       AT+CPWD=?
     *       +CPWD: ("P2",8),("SC",8),("AO",4)("OI",4),("OX",4),("AI",4),("IR",4), ("AB",4),("AG",4),("AC",4)
     *       OK
     */
    { AT_CMD_CPWD,
      At_SetCpwdPara, AT_SET_SS_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCpwdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CPWD", (VOS_UINT8 *)"(\"P2\",\"SC\",\"AO\",\"OI\",\"OX\",\"AI\",\"IR\",\"AB\",\"AG\",\"AC\",\"PS\"),(@oldpwd),(@newpwd)" },

    /*
     * [���]: Э��AT-��ȫ����
     * [����]: �豸����
     * [˵��]: ���������ڼ���������MT�������繦�ܣ��Լ���ѯ����״̬��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CLCK=<fac>,<mode>[,<passwd>[,<class>]]
     *     [���]: when <mode>=2 and command successful:
     *             <CR><LF>+CLCK: <status>[,<class1>[,<ss-status>]
     *             [<CR><LF>+CLCK: <status>,<class2>[,<ss-status>]
     *             [...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CLCK=?
     *     [���]: <CR><LF>+CLCK: (list of supported <fac>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <fac>: �ַ������ͣ�ָ������������Ķ���
     *             ��SC����SIM PIN1��
     *             ��AO������ֹ���к�����
     *             ��OI������ֹ���й��ʺ�����
     *             ��OX������ֹ���й��ʺ��������������⣻
     *             ��AI������ֹ���к��룻
     *             ��IR������������������ʱ����ֹ���к��룻
     *             ��AB������ֹ����ҵ�񣨽���<mode>=0ʱ���ã���
     *             ��AG������ֹ����ҵ�񣨽���<mode>=0ʱ���ã���
     *             ��AC������ֹ����ҵ�񣨽���<mode>=0ʱ���ã���
     *             ��FD����FDN��
     *             ��PN���������磻
     *             ��PU������������
     *             ��PP������SP��
     *     <mode>: ����ֵ������ģʽ��
     *             0��ȥ�����<fac>����Ϊ"PN","PU","PP"ʱ��Ϊ��������)��
     *             1�������<fac>����Ϊ"PN","PU","PP"ʱ����֧�ּ����
     *             2����ѯ��
     *     <status>: ����ֵ����ǰ״̬,��<fac>����Ϊ"PN","PU","PP"ʱ����ʾ������������ǰ�������ļ���״̬��
     *             0��δ���
     *             1�����
     *     <passwd>: �ַ������ͣ����޸���������+CPWD���趨��������ͬ��
     *     <class>: ����ֵ��ҵ�����ȡֵΪ����ҵ��������ϣ�Ĭ��ֵΪ255��
     *             1��voice (telephony)��
     *             2��data��
     *             4��fax (facsimile services)��
     *             8��short message service��
     *             16��data circuit sync��
     *             32��data circuit async��
     *             64��dedicated packet access��
     *             128��dedicated PAD access��
     *     <ss-status>: ����ֵ��˽���ֶνӿڣ����������ҵ��״̬��������տڿ��ƣ��������¡�
     *             BIT3��ҵ��ɲ���״̬��ҵ�񼤻�״̬��Ч��
     *             1��ҵ���û����ɲ�����
     *             0��ҵ���û��ɲ�����
     *             BIT 2��ǩԼ״̬��
     *             1��ҵ����ǩԼ��
     *             0��ҵ��δǩԼ��
     *             BIT1��ҵ��ע��״̬��
     *             1��ҵ����ע�᣻
     *             0��ҵ��δע�᣻
     *             BIT0��ҵ�񼤻�״̬��
     *             1��ҵ���Ѽ��
     *             0��ҵ��δ���
     * [ʾ��]:
     *     �� ����PIN�룬�򿪻���Ҫ����PIN��
     *       AT+CLCK="SC",1,"1234"
     *       OK
     *     �� �����ֹ���к���
     *       AT+CLCK="AO",1,"1234"
     *       OK
     *     �� ��ѯ��ֹ���к���״̬
     *       AT+CLCK="AO",2
     *       +CLCK: 1,1
     *       OK
     *     �� ����CLCK
     *       AT+CLCK=?
     *       +CLCK: ("SC","AO","OI","OX","AI","IR","AB","AG","AC","FD","PN","PU","PP")
     *       OK
     */
    { AT_CMD_CLCK,
      At_SetClckPara, AT_SET_SS_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestClckPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CLCK", (VOS_UINT8 *)"(\"P2\",\"SC\",\"AO\",\"OI\",\"OX\",\"AI\",\"IR\",\"AB\",\"AG\",\"AC\",\"PS\",\"FD\",\"PN\",\"PU\",\"PP\"),(0-2),(psw),(1-255)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: PS����
     * [˵��]: PS������ز�����
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGATT=<state>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGATT?
     *     [���]: <CR><LF>+CGATT: <state><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGATT=?
     *     [���]: <CR><LF>+CGATT: (list of supported <state>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <state>: ����ֵ��PS�����״̬��
     *             0������״̬��
     *             1������״̬��
     * [ʾ��]:
     *     �� ����PS����
     *       AT+CGATT=1
     *       OK
     *     �� ��ѯPS����״̬
     *       AT+CGATT?
     *       +CGATT: 1
     *       OK
     *     �� ����CGATT
     *       AT+CGATT=?
     *       +CGATT: (0,1)
     *       OK
     */
    { AT_CMD_CGATT,
      At_SetCgattPara, AT_CGATT_SET_PARA_TIME, At_QryCgattPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGATT", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ѡ��PLMN
     * [˵��]: �ֶ����Զ���ʽ��PLMN��Public Land Mobile Network��ѡ��
     *         ��ѯ��ǰע���PLMN��
     *         ��ȡPLMN�б�
     *         ��ȡPLMN�б�����У���ͨ���������ַ��������ַ�����ͨ��NV en_NV_Item_AT_ABORT_CMD_PARA���ã������ô���ַ���ΪANY��ASCII�룬���ʾ�����ַ���ϣ��������ַ�����NV�����õ��ַ�������ϸù��̣������ɺ������OK��������ַ�����ͨ����ͬNV�����ã���
     *         ��CSIM����λ��CLģʽ�£�֧��AT+COPS=0��AT+COPS=3,<format>�� AT+COPS?��ѯ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +COPS=[<mode>[,<format>
     *             [,<oper>[,<rat>]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +COPS?
     *     [���]: <CR><LF>+COPS: <mode>[,<format>,<oper>[,<rat>]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +COPS=?
     *     [���]: <CR><LF>+COPS: [list of supported (<stat>,long alphanumeric <oper>,short alphanumeric <oper>,numeric <oper>[,<rat>])s][,,(list of supported <mode>s),(list of supported <format>s)] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������ѡ��ģʽ��Ĭ��ֵΪ0��
     *             0���Զ�������<mode>Ϊ0ʱ������<format>��<oper>��Ч��
     *             1���ֶ�������
     *             2��ȥע�����磻
     *             3����Ϊ+COPS?��ѯ�������÷��ظ�ʽ<format>��
     *             4���ֶ����Զ������ϣ�����ֶ�����ʧ�ܣ����Զ�ת���Զ�����ģʽ��
     *     <format>: ����ֵ����Ӫ����Ϣ<oper>�ĸ�ʽ��Ĭ��ֵΪ0��
     *             0�����ַ�����ʽ����Ӫ����Ϣ<oper>��
     *             1�����ַ�����ʽ����Ӫ����Ϣ<oper>��
     *             2�����ָ�ʽ����Ӫ����Ϣ<oper>��
     *     <oper>: �ַ������ͣ���Ӫ����Ϣ��
     *     <stat>: ����ֵ�������״̬��ʶ��
     *             0��δ֪��
     *             1�����ã�
     *             2����ע�᣻
     *             3����ֹ��
     *     <rat>: ����ֵ�����߽��뼼����Ĭ��ֵ��SYSCFG�����õ����ȼ���ͬ��
     *             0��GSM/GPRS��
     *             2��WCDMA��
     *             7��LTE��
     *             ע�⣺��ֵ���ڵ���֧��GUL��CL��ģʱ��Ч����֧��LTEʱ���ò�������ѡ��
     *             12: NR��
     *             ע�⣺��ֵ���ڵ���֧��GULNRģʱ��Ч����֧��NRʱ���ò�������ѡ��
     *             13��endc;
     *             ע�⣺��ֵ����ͬʱ֧��LTE��NRʱ��Ч����֧��endcʱ���ò�������ѡ��
     * [ʾ��]:
     *     �� ��ȡPLMN�б�
     *       AT+COPS=?
     *       +COPS: (2,"CHINA MOBILE","CMCC","46000",0),(0,"CHN-UNICOM","UNICOM","46001",0)
     *       OK
     *     �� �Զ�����
     *       AT+COPS=0
     *       OK
     *     �� �ֶ�����
     *       AT+COPS=1,2,"46000"
     *       OK
     *     �� +COPS?��ѯ�������÷��ظ�ʽ
     *       AT+COPS=3,2
     *       OK
     *     �� ��ѯ��ǰע���PLMN
     *       AT+COPS?
     *       +COPS: 1,2,"46000",0
     *       OK
     *       ��ȡPLMN�б���̱����
     *     �� ִ�л�ȡPLMN�б�����
     *       AT+COPS=?
     *       AT+CSQ
     *       AT
     *       OK
     *     �� �ٴβ�ѯ�ź�ǿ�ȣ������ź�ǿ��
     *       AT+CSQ
     *       +CSQ: 5,99
     *       OK
     */
    { AT_CMD_COPS,
      At_SetCopsPara, AT_COPS_SPEC_SRCH_TIME, At_QryCopsPara, AT_QRY_PARA_TIME, At_TestCopsPara, AT_COPS_LIST_SRCH_TIME,
      At_AbortCopsPara, AT_COPS_LIST_ABORT_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+COPS", (VOS_UINT8 *)COPS_CMD_PARA_STRING },

#if (FEATURE_CSG == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: CSG IDS����
     * [˵��]: �����ڽ���ָ����CSG ID��������ָ��CSG����ʱֻ����Я��һ��CSG ID��
     *         �����ڵ�ǰפ��CSG ID�Ĳ�ѯ��
     *         �����ڽ���CSG ID�б�������
     *         Ŀǰֻ֧��LTE��CSG ID��������
     *         ��AT����Ϊ˽��AT���
     * [�﷨]:
     *     [����]: ^CSGIDSRCH=<PlmnId>,<CSGID>,<rat>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CSGIDSRCH?
     *     [���]: <CR><LF>^CSGIDSRCH: <PlmnId>,<CSG ID > ,<rat><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CSGIDSRCH=?
     *     [���]: <CR><LF>^CSGIDSRCH: <OperNameLong >,< OperNameShort >,<PlmnId>,<CSG ID > , <CSG ID type>,<home NodeB Name>,< CSG type >,<rat>,< sSignalValue1>,< sSignalValue2>,<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <OperNameLong>: �ַ������͵���Ӫ�̳���
     *     <OperNameShort>: �ַ������͵���Ӫ�̶���
     *     <PlmnId>: ���ָ�ʽ����Ӫ����Ϣ����PLMN��Ϣ
     *     <CSG ID>: Closed Subscriber Group Identification��CSGС���ı�ʶ
     *     <CSG ID type>: CSG ID����
     *             1��CSG ID��Allowed CSG List��
     *             2��CSG ID��Operator CSG List���Ҳ���Forbidden CSG List��
     *             3��CSG ID��Operator CSG List������Forbidden CSG List��
     *             4��CSG ID��Allowed CSG List��Operator CSG List
     *     <home NodeB Name>: home NodeB Name
     *     <CSG type>: CSG����
     *     <rat>: ����ֵ�����߽��뼼����Ĭ��ֵ��SYSCFG �����õ����ȼ���ͬ��
     *             0��GSM/GPRS��
     *             2��WCDMA��
     *             7��LTE��Ŀǰֻ֧��LTE��CSG���ܣ�
     *     <sSignalValue1>: ratΪLTEʱ��ʾRSRP��ratΪWCDMAʱ��ʾRSCP��ratΪGSMʱ��ʾRSSI
     *     <sSignalValue2>: ratΪLTEʱ��ʾRSRQ��ratΪWCDMAʱ��ʾEC/IO��ratΪGSMʱĬ��Ϊ0
     * [ʾ��]:
     *     �� CSGָ����310410 CSG IDΪ1234������
     *       AT^CSGIDSRCH="310410","1234",7
     *       OK
     *     �� ��ѯ��ǰפ��CSG ID��Ϣ
     *       AT^CSGIDSRCH?
     *       ^CSGIDSRCH: "310410","1234",7
     *       OK
     *     �� CSG ID�б�����
     *       AT^CSGIDSRCH=?
     *       ^CSGIDSRCH: "Vodafone CZ","Vodafone","23003","BB8",4,0203,0202020202020202,7,-60,-82
     *       OK
     */
    { AT_CMD_CSGIDSEARCH,
      AT_SetCsgIdSearchPara, AT_CSG_SPEC_SRCH_TIME, At_QryCampCsgIdInfo, AT_QRY_PARA_TIME, AT_TestCsgIdSearchPara, AT_CSG_LIST_SRCH_TIME,
      AT_AbortCsgIdSearchPara, AT_CSG_LIST_ABORT_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CSGIDSRCH", (VOS_UINT8 *)"(@oper),(@csg),(0,2,7)" },
#endif

    /*
     * [���]: Э��AT-�绰����
     * [����]: ����MTģʽ
     * [˵��]: ����MTģʽ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGCLASS=[<class>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGCLASS?
     *     [���]: <CR><LF>+CGCLASS: <class><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGCLASS=?
     *     [���]: <CR><LF>+CGCLASS: (list of supported <class>s)
     *             <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <class>: �ַ������ͣ�ָʾMTģʽ��
     *             A��ͬʱ֧��CS���PS��WCDMA����ģʽ����Ч����GSM����ģʽ��MTģʽA���Զ�ת��ΪMTģʽB�����ݲ�֧�֣���
     *             B��ͬһʱ��ֻ��֧��CS���PS���е�һ����GSM����ģʽ����Ч����WCDMA����ģʽ��MTģʽB���Զ�ת��ΪMTģʽA�����ݲ�֧�֣���
     *             CG��ֻ֧��PS���ݲ�֧�֣���
     *             CC��ֻ֧��CS���ݲ�֧�֣���
     * [ʾ��]:
     *     �� ����ΪMTģʽA
     *       AT+CGCLASS="A"
     *       OK
     *     �� ��ѯCGCLASS
     *       AT+CGCLASS?
     *       +CGCLASS: "A"
     *       OK
     *     �� ����CGCLASS
     *       AT+CGCLASS=?
     *       +CGCLASS: ("A")
     *       OK
     *     �� ��������
     *       AT+CGCLASS=
     *       +CME ERROR: Incorrect parameters
     */
    { AT_CMD_CGCLASS,
      At_SetCgclassPara, AT_SET_PARA_TIME, At_QryCgclassPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGCLASS", (VOS_UINT8 *)"(\"A\")" },

#if (FEATURE_MBB_CUST == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: ����MT����ģʽ
     * [˵��]: ����MT����ģʽ��
     *         ���������3GPP TS 27.007Э�顣
     *         AP-MODEM��E5��̬AT+CFUN=8�µ繦����Ҫ��Ӧ�÷������������Աͨ��AT�����·�CFUN������ᴥ���µ硣
     * [�﷨]:
     *     [����]: +CFUN=[<fun>[,<rst>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CFUN?
     *     [���]: <CR><LF>+CFUN: <fun><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CFUN=?
     *     [���]: <CR><LF>+CFUN: (list of supported <fun>s), (list of supported <rst>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <fun>: ����ֵ������ģʽ��Ĭ��ֵΪ0��
     *             0������Ϊ��Сģʽ������NV2594�����Ƿ������offlineģʽ�л�����Сģʽ��Ĭ�ϲ�������
     *             1������Ϊonlineģʽ���������Ĭ��ֵ��������NV2594�����Ƿ������offlineģʽ�л���onlineģʽ��Ĭ�ϲ�������
     *             4������Ϊofflineģʽ��MT֮ǰ�����ñ���Ϊ��FTMģʽ����
     *             5������ΪFTMģʽ��MT֮ǰ�����ñ���Ϊonlineģʽ����Ϊ��У׼ʹ�ã����Դ���Сģʽ�л�������ģʽ����
     *             6������MT��MT֮ǰ�����ñ���Ϊofflineģʽ����
     *             7������Ϊ��Сģʽ��MT֮ǰ�����ñ���Ϊ��offlineģʽ����ΪVODAFONE��Ӫ�̶��ƣ�
     *             8���ػ��µ磨��������USB��λ�������ػ����ģʽ����������E5��̬��AP-Modem��̬��
     *             9��SIM���±��ػ�ģʽ�����ڲ�ģ��ʹ�ã��û��������ã���
     *             10�����ٹػ����ػ���Ч����ͬ��ģʽ0��
     *             11�����ٿ�����(MT֮ǰ�����ñ���Ϊofflineģʽ)��
     *             ע������ֵ10��11����FEATURE_MBB_CUST�꿪��ʱ��Ч��
     *     <rst>: ����ֵ���Ƿ�λ��
     *             0����������λ��
     *             1��������λ����ǰֻ֧��onlineģʽ��AT+CFUN=1,1������λ��
     *             ������λ��modem������λ����������λ��NV2391���ơ�
     * [ʾ��]:
     *     �� onlineģʽ�·���λ
     *       AT+CFUN=1,1
     *       OK
     *     �� ����Ϊonlineģʽ
     *       AT+CFUN=1
     *       OK
     *     �� ��ѯCFUN
     *       AT+CFUN?
     *       +CFUN: 1
     *       OK
     *     �� ����CFUN
     *       AT+CFUN=?
     *       +CFUN: (0,1,4,5,6,7,8,10,11),(0,1)
     *       OK
     */
    { AT_CMD_CFUN,
      At_SetCfunPara, AT_SET_CFUN_TIME, At_QryCfunPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_E5_IS_LOCKED | CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CFUN", (VOS_UINT8 *)"(0,1,4,5,6,7,8,10,11),(0,1)" },
#else
    /*
     * [���]: Э��AT-�绰����
     * [����]: ����MT����ģʽ
     * [˵��]: ����MT����ģʽ��
     *         ���������3GPP TS 27.007Э�顣
     *         AP-MODEM��E5��̬AT+CFUN=8�µ繦����Ҫ��Ӧ�÷������������Աͨ��AT�����·�CFUN������ᴥ���µ硣
     * [�﷨]:
     *     [����]: +CFUN=[<fun>[,<rst>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CFUN?
     *     [���]: <CR><LF>+CFUN: <fun><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CFUN=?
     *     [���]: <CR><LF>+CFUN: (list of supported <fun>s), (list of supported <rst>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <fun>: ����ֵ������ģʽ��Ĭ��ֵΪ0��
     *             0������Ϊ��Сģʽ������NV2594�����Ƿ������offlineģʽ�л�����Сģʽ��Ĭ�ϲ�������
     *             1������Ϊonlineģʽ���������Ĭ��ֵ��������NV2594�����Ƿ������offlineģʽ�л���onlineģʽ��Ĭ�ϲ�������
     *             4������Ϊofflineģʽ��MT֮ǰ�����ñ���Ϊ��FTMģʽ����
     *             5������ΪFTMģʽ��MT֮ǰ�����ñ���Ϊonlineģʽ����Ϊ��У׼ʹ�ã����Դ���Сģʽ�л�������ģʽ����
     *             6������MT��MT֮ǰ�����ñ���Ϊofflineģʽ����
     *             7������Ϊ��Сģʽ��MT֮ǰ�����ñ���Ϊ��offlineģʽ����ΪVODAFONE��Ӫ�̶��ƣ�
     *             8���ػ��µ磨��������USB��λ�������ػ����ģʽ����������E5��̬��AP-Modem��̬��
     *             9��SIM���±��ػ�ģʽ�����ڲ�ģ��ʹ�ã��û��������ã���
     *             10�����ٹػ����ػ���Ч����ͬ��ģʽ0��
     *             11�����ٿ�����(MT֮ǰ�����ñ���Ϊofflineģʽ)��
     *             ע������ֵ10��11����FEATURE_MBB_CUST�꿪��ʱ��Ч��
     *     <rst>: ����ֵ���Ƿ�λ��
     *             0����������λ��
     *             1��������λ����ǰֻ֧��onlineģʽ��AT+CFUN=1,1������λ��
     *             ������λ��modem������λ����������λ��NV2391���ơ�
     * [ʾ��]:
     *     �� onlineģʽ�·���λ
     *       AT+CFUN=1,1
     *       OK
     *     �� ����Ϊonlineģʽ
     *       AT+CFUN=1
     *       OK
     *     �� ��ѯCFUN
     *       AT+CFUN?
     *       +CFUN: 1
     *       OK
     *     �� ����CFUN
     *       AT+CFUN=?
     *       +CFUN: (0,1,4,5,6,7,8,10,11),(0,1)
     *       OK
     */
    { AT_CMD_CFUN,
      At_SetCfunPara, AT_SET_CFUN_TIME, At_QryCfunPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_E5_IS_LOCKED | CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CFUN", (VOS_UINT8 *)"(0,1,4,5,6,7,8),(0,1)" },
#endif


    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯ��������Ϣ
     * [˵��]: ��2��������ڲ�ѯ��������Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     *         ��2�������ѯ����ܲ�Ʒ�߶���NV8203�أ�Ĭ��Ϊ��Huawei��
     * [�﷨]:
     *     [����]: +CGMI
     *     [���]: <CR><LF><manufacturer><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGMI=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <manufacturer>: �ַ�������������Ϣ��
     * [ʾ��]:
     *     �� ��ѯ��������Ϣ
     *       AT+CGMI
     *       Huawei
     *       OK
     *       AT+GMI
     *       Huawei
     *       OK
     *     �� ����CGMI
     *       AT+CGMI=?
     *       OK
     *       AT+GMI=?
     *       OK
     */
    { AT_CMD_CGMI,
      At_SetCgmiPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGMI", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯMT�ͺ�
     * [˵��]: ��ѯMT�ͺš�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGMM
     *     [���]: <CR><LF><model><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGMM=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <model>: �ַ�����MT�ͺ���Ϣ��
     * [ʾ��]:
     *     �� ��ѯMT�ͺ�
     *       AT+CGMM
     *       H8180
     *       OK
     *     �� ����CGMM
     *       AT+CGMM=?
     *       OK
     */
    { AT_CMD_CGMM,
      At_SetCgmmPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGMM", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯMT�ͺ�
     * [˵��]: ��ѯMT�ͺš�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +GMM
     *     [���]: <CR><LF><model><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +GMM=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <model>: �ַ�����MT�ͺ���Ϣ��
     * [ʾ��]:
     *     �� ��ѯMT�ͺ�
     *       AT+CGMM
     *       H8180
     *       OK
     *     �� ����CGMM
     *       AT+CGMM=?
     *       OK
     */
    { AT_CMD_GMM,
      At_SetCgmmPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+GMM", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯ��Ʒ����汾��Ϣ
     * [˵��]: ��2��������ڲ�ѯ��Ʒ����汾��Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGMR
     *     [���]: <CR><LF><revision><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGMR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <revision>: �ַ���������汾��Ϣ��
     * [ʾ��]:
     *     �� ��ѯ����汾��Ϣ
     *       AT+CGMR
     *       V100
     *       OK
     *       AT+GMR
     *       V100
     *       OK
     *     �� ����CGMR
     *       AT+CGMR=?
     *       OK
     *       AT+GMR=?
     *       OK
     */
    { AT_CMD_CGMR,
      At_SetCgmrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGMR", VOS_NULL_PTR },
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯ��Ʒ���к�
     * [˵��]: ��ѯ��Ʒ���кţ��������ƶ��豸ID��International Mobile Equipment Identity����+GSN��+CGSN������ȫһ��������ʹ�ø�ʽҲһ����
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGSN
     *     [���]: <CR><LF><sn><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGSN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <sn>: �ַ�������Ʒ���кţ�������μ���2-3��
     * [��]: <sn>��������
     *       8byte,                   6byte,             1byte,
     *       TAC���豸����������룩, SNR���豸���кţ�, Spare�����ã�,
     *       ���磺���TACΪ��35154800����SNRΪ��122544����SpareΪ��0������IMEIΪ351548001225440��,
     * [ʾ��]:
     *     �� ��ѯ��Ʒ���к�
     *       AT+CGSN
     *       351548001225440
     *       OK
     *     �� ����CGSN
     *       AT+CGSN=?
     *       OK
     */
    { AT_CMD_CGSN,
      At_SetCgsnPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGSN", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯ��Ʒ���к�
     * [˵��]: ��ѯ��Ʒ���кţ��������ƶ��豸ID��International Mobile Equipment Identity����+GSN��+CGSN������ȫһ��������ʹ�ø�ʽҲһ����
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +GSN
     *     [���]: <CR><LF><sn><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +GSN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <sn>: �ַ�������Ʒ���кţ�������μ���2-3��
     * [��]: <sn>��������
     *       8byte,                   6byte,             1byte,
     *       TAC���豸����������룩, SNR���豸���кţ�, Spare�����ã�,
     *       ���磺���TACΪ��35154800����SNRΪ��122544����SpareΪ��0������IMEIΪ351548001225440��,
     * [ʾ��]:
     *     �� ��ѯ��Ʒ���к�
     *       AT+CGSN
     *       351548001225440
     *       OK
     *     �� ����CGSN
     *       AT+CGSN=?
     *       OK
     */
    { AT_CMD_GSN,
      At_SetCgsnPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+GSN", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯIMSI
     * [˵��]: ��ѯ(U)SIM����IMSI��International Mobile Subscriber Identity��ֵ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CIMI
     *     [���]: <CR><LF><IMSI><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CIMI=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <IMSI>: �����ƶ��û�ʶ���롣
     * [��]: IMSI��������
     *       ���15���ַ�,
     *       3���ַ�,         2����3���ַ�,    ����,
     *       MCC�����Ҵ��ţ�, MNC��������ţ�, MSIN����ʶ�ƶ��û���ݣ�,
     *       ���磺���MCCΪ��230����MNCΪ��02����MSINΪ��0216666831������IMSIΪ��230020216666831��,
     * [ʾ��]:
     *     �� ��ѯIMSIֵ
     *       AT+CIMI
     *       230020216666831
     *       OK
     *     �� ����CIMI
     *       AT+CIMI=?
     *       OK
     */
    { AT_CMD_CIMI,
      At_SetCimiPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"+CIMI", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯ�ź�ǿ��
     * [˵��]: ��ѯ��ǰ����С���ź�ǿ�����ŵ������ʡ�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CSQ
     *     [���]: <CR><LF>+CSQ: <rssi>,<ber><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CSQ: <99>,<99><CR><LF>
     *     [����]: +CSQ=?
     *     [���]: <CR><LF>+CSQ: (list of supported <rssi>s),(list of supported <ber>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <rssi>: ����ֵ�������ź�ǿ��ָʾ��
     *             0�����ڻ�С�ڨC113dBm��
     *             1���C111dBm��
     *             2��30���C109dBm���C53dBm������Ϊ2dBm��2���C109dBm��3���C107dBm��
     *             ����30���C53dBm����
     *             31�����ڻ���ڨC51dBm��
     *             99��δ֪�򲻿ɲ⡣
     *     <ber>: 0~7������ֵ�����������ʰٷֱȡ�
     *             99��δ֪�򲻿ɲ⡣
     * [ʾ��]:
     *     �� ��ѯ�ź�ǿ�Ⱥ�������
     *       AT+CSQ
     *       +CSQ: 5,99
     *       OK
     *     �� ����CSQ
     *       AT+CSQ=?
     *       +CSQ: (0-31,99),(99)
     *       OK
     */
    { AT_CMD_CSQ,
      At_SetCsqPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CSQ", (VOS_UINT8 *)"(0-31,99),(99)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �г�������Ч��AT����
     * [˵��]: �������ѯ֧�ֵ����д����й涨�Ŀ���ʾ��AT���
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CLAC
     *     [���]: <CR><LF><AT Command1> [<CR><LF><AT Command2>[��]] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     */
    { AT_CMD_CLAC,
      At_SetCLACPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CLAC", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯMS��ǰ��֧�ֵĴ���������
     * [˵��]: ��ѯMS��ǰ��֧�ֵĴ���������
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: +GCAP
     *     [���]: <CR><LF>+GCAP: (list of supported<name>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +GCAP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <name>: MS֧�ֵĴ����������������б�
     *             CGSM��GSM������
     *             DS������ѹ��������
     *             ES���������������
     * [ʾ��]:
     *     �� ��ѯMS��ǰ��֧�ֵĴ���������
     *       AT+GCAP
     *       +GCAP: +CGSM,+DS,+ES
     *       OK
     *     �� GCAP��������
     *       AT+GCAP=?
     *       OK
     */
    { AT_CMD_GCAP,
      At_SetGcapPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+GCAP", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ѡ�����������б�
     * [˵��]: ѡ��ǰ��������������б�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CPLS=<list>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPLS?
     *     [���]: <CR><LF>+CPLS: <list><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CPLS=?
     *     [���]: <CR><LF>+CPLS: (list of supported <list>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <list>: ����ֵ��ѡ�����������б�ȡֵ0��2��
     *             0���û���ѡPLMN�б���ӦUSIM���е�EFPLMNwAcT�ļ���SIM���е�EFPLMNsel�ļ���
     *             1����Ӫ����ѡPLMN�б���ӦUSIM/SIM���е�EFOPLMNwAcT�ļ���
     *             2����������ѡPLMN�б���ӦUSIM/SIM���е�EFHPLMNwAcT�ļ���
     * [ʾ��]:
     *     �� ����ѡ����ѡ�����б�Ϊ�û���ѡPLMN�б�
     *       AT+CPLS=0
     *       OK
     *     �� ��ѯ��ǰѡ����ѡ�����б�
     *       AT+CPLS?
     *       +CPLS: 0
     *       OK
     *     �� ѡ����ѡ�����б�Ĳ�������
     *       AT+CPLS=?
     *       +CPLS: (0,1,2)
     *       OK
     */
    { AT_CMD_CPLS,
      At_SetCplsPara, AT_SET_PARA_TIME, At_QryCplsPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CPLS", (VOS_UINT8 *)"(0,1,2)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: �������������б�
     * [˵��]: ��+CPLSѡ�������������б���в�ѯ���޸�PLMN ID�ͽ��뼼����ɾ�������Ӳ���,�ܿ��ļ�ADMȨ�����ƣ�����ͨ����AT�����޸�HPLMN��OPLMN�б�
     *         ���������3GPP TS 27.007Э�顣
     *         1��ͬһ��PLMN ID��֧�ֵĽ��뼼����ͬ��������Ӻ��޸Ķ�Ρ�������뼼�����ظ���������ӣ������޸ġ�
     *         2����NR���ʱ��ʹ�ø�AT�����������ʱ����NV6006��������أ������UPLMN/OPLMN�Ƿ���Ҫ����NR��ʽ����6006����ʹ��UPLMN/OPLMN֧��NRʱ��ֻҪAT�������µ�UPLMN/OPLMN��������<NG-RAN_AcTn>�Ƿ�֧�֣�������UPLMN/OPLMN������Ϊ֧����NR��
     * [�﷨]:
     *     [����]: +CPOL=[<index>][,<format>[,<oper>[,<GSM_AcT>,<GSM_Compact_AcT>,<UTRAN_AcT>,[<E-UTRAN_AcT>],[NG-RAN_AcT]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPOL?
     *     [���]: <CR><LF>+CPOL: <index1>,<format>,<oper1>[,<GSM_AcT1>, <GSM_Compact_AcT1>,<UTRAN_AcT1>,<E-UTRAN_AcT>,<NG-RAN_AcT1>][<CR><LF>
     *             +CPOL: <index2>,<format>,<oper2>[,<GSM_AcT2>,<GSM_Compact_AcT2>,<UTRAN_AcT2>,<E-UTRAN_AcT>,<NG-RAN_AcT2>][...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CPOL=?
     *     [���]: <CR><LF>+CPOL: (list of supported <index>s),(list of supported <format>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <indexn>: ����ֵ��USIM/SIM����PLMN��˳��
     *     <format>: ����ֵ����Ӫ����Ϣ<oper>�ĸ�ʽ��Ĭ��ֵΪ2��
     *             0�����ַ�����ʽ����Ӫ����Ϣ<oper>��
     *             1�����ַ�����ʽ����Ӫ����Ϣ<oper>��
     *             2�����ָ�ʽ����Ӫ����Ϣ<oper>��
     *     <opern>: �ַ������ͣ���Ӫ����Ϣ��
     *     <GSM_AcTn>: GSM�Ľ���������
     *             0����֧�֣�
     *             1��֧�֡�
     *     <GSM_Compact_AcTn>: GSM_Compact�Ľ���������
     *             0����֧�֣�
     *             1��֧�֡�
     *     <UTRA_AcTn>: UTRAN�Ľ���������
     *             0����֧�֣�
     *             1��֧�֡�
     *     <E-UTRAN_AcT>: LTE�Ľ���������
     *             0����֧�֣�
     *             1��֧�֡�
     *             ע�⣺�ò������ڵ���֧��GUL��ģʱ��Ч����֧��LTEʱ���ò�������ѡ��
     *     <NG-RAN_AcTn>: NR�Ľ���������
     *             0����֧�֣�
     *             1��֧�֡�
     *             ע�⣺�ò������ڵ���֧��NRʱ��Ч����֧��NRʱ���ò�������ѡ��
     * [ʾ��]:
     *     �� ɾ����ǰ���������б�������Ϊ5��PLMN
     *       AT+CPOL=5
     *       OK
     *     �� ��ѯ��ǰ��ѡ�����б�
     *       AT+CPOL?
     *       +CPOL: 2,2,"222222",1,0,1,0
     *       +CPOL: 3,1,"CMCC",1,0,1,0
     *       +CPOL: 4,2,"444444",1,0,1,1
     *       +CPOL: 8,2,"123456",1,0,0,1
     *       OK
     *     �� ��ǰ��ѡ�����б�ĸ���Ϊ8������+CPOL
     *       AT+CPOL=?
     *       +CPOL: (1-8),(0-2)
     *       OK
     *     �� ƽ̨����֧��NR����ѯ��ǰ��ѡ�����б�
     *       AT+CPOL?
     *       +CPOL: 2,2,"222222",1,0,1,1,1
     *       OK
     */
    { AT_CMD_CPOL,
      At_SetCpolPara, AT_SET_PARA_TIME, At_QryCpolPara, AT_QRY_PARA_TIME, At_TestCpolPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CPOL", (VOS_UINT8 *)CPOL_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯCS��ע��״̬
     * [˵��]: ��ѯ��ǰCS��Circuit Switched domain��������ע��״̬���Լ�����״̬�ϱ���ʽ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CREG=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CREG?
     *     [���]: <CR><LF>+CREG: <n>,<stat>[,<lac>,<ci>[,<AcT>]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CREG=?
     *     [���]: <CR><LF>+CREG: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ�������ϱ���ʽ��Ĭ��ֵΪ0��
     *             0����ֹ+CREG�������ϱ���
     *             1��ʹ��+CREG: <stat>�������ϱ���
     *             2��ʹ��+CREG: <stat>[,<lac>,<ci>[,<AcT>]]�������ϱ���
     *     <stat>: ����ֵ��ע��״̬��
     *             0��û��ע�ᣬMTû�д���������ע���µ���Ӫ��״̬��
     *             1��ע���˹������磻
     *             2��û��ע�ᣬMT����������ע���µ���Ӫ�̣�
     *             3��ע�ᱻ�ܾ���
     *             4��δ֪״̬��
     *             5��ע�����������硣
     *     <lac>: 2byte��16���������ַ������ͣ�λ������Ϣ�����磺��00C3����ʾ10���Ƶ�195��
     *     <ci>: 4byte��16���������ַ������ͣ�С����Ϣ��
     *     <AcT>: ����ֵ����ǰ����Ľ��뼼����
     *             0��GSM��
     *             1��GSM Compact��
     *             2��UTRAN��
     *             3��GSM EGPRS��
     *             4��UTRAN HSDPA��
     *             5��UTRAN HSUPA��
     *             6��UTRAN HSDPA��HSUPA��
     *             7��E-UTRAN��
     * [ʾ��]:
     *     �� ����CS��ע��״̬�ϱ�
     *       AT+CREG=1
     *       OK
     *     �� <n>=1ʱ��ѯע��״̬
     *       AT+CREG?
     *       +CREG: 1,1
     *       OK
     *     �� <n>=2ʱ��ѯע��״̬
     *       AT+CREG?
     *       +CREG: 2,1,"00C3","0000001A",4
     *       OK
     *     �� ����CREG
     *       AT+CREG=?
     *       +CREG: (0-2)
     *       OK
     */
    { AT_CMD_CREG,
      At_SetCregPara, AT_SET_PARA_TIME, At_QryCregPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CREG", (VOS_UINT8 *)"(0-2)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯPS��ע��״̬
     * [˵��]: ��ѯ��ǰPS������ע��״̬��������״̬�ϱ���ʽ��
     * [�﷨]:
     *     [����]: +CGREG=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGREG?
     *     [���]: <CR><LF>+CGREG: <n>,<stat>[,<lac>,<ci>[,<AcT>]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGREG=?
     *     [���]: <CR><LF>+CGREG: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ�������ϱ���ʽ��Ĭ��ֵΪ0��
     *             0����ֹ+CGREG�������ϱ���
     *             1��ʹ��+CGREG: <stat>�������ϱ���ʽ��
     *             2��ʹ��+CGREG: <stat>[,<lac>,<ci>[,<AcT>]]�������ϱ���ʽ��
     *     <stat>: ����ֵ��ע��״̬��
     *             0��û��ע�ᣬMT����û��������ע���µ���Ӫ�̣�
     *             1��ע���˹������磻
     *             2��û��ע�ᣬMT����������ע���µ���Ӫ�̣�
     *             3��ע�ᱻ�ܾ���
     *             4��δ֪״̬��
     *             5��ע�����������硣
     *     <lac>: 2byte��16���������ַ������ͣ�λ������Ϣ�����磺��00C3����ʾ10���Ƶ�195��
     *     <ci>: 4byte��16���������ַ������ͣ�С����Ϣ��
     *     <AcT>: ����ֵ����ǰ����Ľ��뼼����
     *             0��GSM��
     *             1��GSM Compact��
     *             2��UTRAN��
     *             3��GSM EGPRS��
     *             4��UTRAN HSDPA��
     *             5��UTRAN HSUPA��
     *             6��UTRAN HSDPA��HSUPA��
     *             7��E-UTRAN��
     *             10��EUTRAN-5GC��
     *             11��NR-5GC��
     * [ʾ��]:
     *     �� ����PS��ע��״̬�ϱ�
     *       AT+CGREG=1
     *       OK
     *     �� <n>=1ʱ��ѯע��״̬
     *       AT+CGREG?
     *       +CGREG: 1,1
     *       OK
     *     �� <n>=2ʱ��ѯע��״̬
     *       AT+CGREG?
     *       +CGREG: 2,1,"00C3","000001A",4
     *       OK
     *     �� ����CGREG
     *       AT+CGREG=?
     *       +CGREG: (0-2)
     *       OK
     */
    { AT_CMD_CGREG,
      At_SetCgregPara, AT_SET_PARA_TIME, At_QryCgregPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CGREG", (VOS_UINT8 *)"(0-2)" },

#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: EPS��ע��״̬
     * [˵��]: SET�������+CEREG�����ϱ��¼��ķ�ʽ��
     *         ��<n>=1������ע���״̬�����ı��ʱ���ϱ�+CEREG:<stat>
     *         ��<n>=2��С����Ϣ�����ı�ʱ���ϱ�+CEREG: <stat>[,<lac>,<ci>[,<AcT>]]
     *         ������ص�ǰ��ע��״̬<stat>��λ����Ϣ<lac>,<ci>����<n>=2ʱ�ϱ���
     *         ���������3GPP TS 27.007Э�顣
     *         ��������ڵ���֧��LTEʱ����ʹ�ã�����ǰ����֧��LTE��������
     * [�﷨]:
     *     [����]: +CEREG=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: +CEREG?
     *     [���]: <CR><LF>+CEREG: <n>,<stat>[,<lac>,<ci>[,<AcT>]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��ע��n=2ʱ���ݵ�ǰע��Ľ��뼼����ͬ����ѯ�����ͬ
     *             UEע����G/U��ʱ��ѯ������ֻ�ϱ�<stat>��������
     *             <CR><LF>+CEREG: <n>,<stat><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             UEע����L��ʱ�������������ϱ���
     *     [����]: +CEREG=?
     *     [���]: <CR><LF>+CEREG: (list of supported <n>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��Ĭ��ֵΪ0��
     *             0����ֹ+CEREG�������ϱ���
     *             1��ʹ��+CEREG: <stat>�������ϱ���
     *             2��ʹ��+CEREG: <stat>[,<lac>,<ci>[,<AcT>]]�������ϱ���
     *     <stat>: 0��û��ע�ᣬMT���ڲ�û������ѰҪע����µ���Ӫ�̣�
     *             1��ע���˱������磻
     *             2��û��ע�ᣬ��MT������ѰҪע����µ���Ӫ�̣�
     *             3��ע�ᱻ�ܾ���
     *             4��δ֪ԭ��
     *             5��ע�����������硣
     *     <lac>: λ������Ϣ���ĸ��ַ���16���Ʊ�ʾ����������00C3����10���Ƶ�195��
     *     <ci>: С����Ϣ���˸��ַ���16���Ʊ�ʾ��
     *     <AcT>: ����ֵ����ǰ����Ľ��뼼����
     *             0��GSM��
     *             1��GSM Compact��
     *             2��UTRAN��
     *             3��GSM EGPRS��
     *             4��UTRAN HSDPA��
     *             5��UTRAN HSUPA��
     *             6��UTRAN HSDPA ��HSUPA��
     *             7��E-UTRAN��
     *             10��EUTRAN-5GC��
     *             11��NR-5GC��
     * [ʾ��]:
     *     �� ����EPS��ע��״̬�ϱ�
     *       AT+CEREG=1
     *       OK
     *     �� <n>=1ʱ��ѯע��״̬
     *       AT+CEREG?
     *       +CEREG: 1,1
     *       OK
     *     �� <n>=2ʱע����L��ʱ��ѯע��״̬
     *       AT+CEREG?
     *       +CEREG: 2,1,"00C3","0000001A",7
     *       OK
     *     �� <n>=2ע����U(��G��NR)��ʱ��ѯע��״̬
     *       AT+CEREG?
     *       +CEREG: 2,1
     *       OK
     *     �� ����CEREG
     *       AT+CEREG=?
     *       +CEREG: (0-2)
     *       OK
     */
    { AT_CMD_CEREG,
      AT_SetCeregPara, AT_SET_PARA_TIME, At_QryCeregPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CEREG", (VOS_UINT8 *)"(0-2)" },
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: 5GCע��״̬
     * [˵��]: ��ѯ��ǰ5G����ע��״̬��������״̬�ϱ���ʽ��
     *         ��������ڵ���֧��NRʱ����ʹ�ã�����ǰ����֧��NR��������
     * [�﷨]:
     *     [����]: +C5GREG=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +C5GREG?
     *     [���]: <CR><LF>+C5GREG: <n>,<stat>[,[<tac>],[<ci>],[<AcT>],[<Allowed_NSSAI_length>],[<Allowed_NSSAI>] ]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��ע��
     *             n=0ʱ��ѯ�������ϱ�<n>,<stat>������
     *             n=1ʱ��ѯ�������ϱ�<n>,<stat>������
     *             n=2ʱ���ݵ�ǰע��Ľ��뼼����ͬ����ѯ�����ͬ
     *             UEע����G/U/L��ʱ��ѯ�������ϱ�<n>,<stat>����
     *             <CR><LF>+C5GREG: <n>,<stat><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             UEע����NR��ʱ�������������ϱ���
     *     [����]: +C5GREG=?
     *     [���]: <CR><LF>+C5GREG: (list of supported <n>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��Ĭ��ֵΪ0��
     *             0����ֹ+C5GREG�������ϱ���
     *             1��ʹ��+C5GREG: <stat>�������ϱ���
     *             2��ʹ��+C5GREG:
     *             <stat>[,[<tac>],[<ci>],[<AcT>],[<Allowed_NSSAI_length>],
     *             [<Allowed_NSSAI>] ]�������ϱ���
     *     <stat>: 0��û��ע�ᣬMT���ڲ�û������ѰҪע����µ���Ӫ�̣�
     *             1��ע���˱������磻
     *             2��û��ע�ᣬ��MT������ѰҪע����µ���Ӫ�̣�
     *             3��ע�ᱻ�ܾ���
     *             4��δ֪ԭ��
     *             5��ע�����������硣
     *             8��registered for emergency services only��
     *     <tac>: λ������Ϣ�������ַ���16���Ʊ�ʾ����������0000C3����10���Ƶ�195��
     *     <ci>: С����Ϣ��8���ֽڣ�16���ַ���16���Ʊ�ʾ��ʹ��Half-Byte�루���ֽڣ����룬����000000010000001A����ʾ��4�ֽ���0x00000001����4�ֽ���0x0000001A��
     *     <AcT>: ����ֵ����ǰ����Ľ��뼼����
     *             10��EUTRAN-5GC
     *             11��NR-5GC
     *     <Allowed_NSSAI_length>: ����ֵ����ʾ<Allowed_NSSAI>��Ϣ����Ƭ�ֽڸ�����
     *             ��<Allowed_NSSAI>��ϢʱֵΪ0��
     *     <Allowed_NSSAI>: �ַ������͡�ÿ��S-NSSAIʹ�á�:�����֡�����S-NSSAI�ĸ�ʽ
     *             "sst" if only slice/service type (SST) is present
     *             "sst;mapped_sst" if SST and mapped configured SST are present
     *             "sst.sd" if SST and slice differentiator (SD) are present
     *             "sst.sd;mapped_sst" if SST, SD and mapped configured SST are present
     *             "sst.sd;mapped_sst.mapped_sd" if SST, SD, mapped configured SST and mapped configured SD are present
     *             ע�⣺�ַ����е�����Ϊ16���ƣ��ַ�ȡֵ��Χ��0-9��A-F��a-f��
     *             ��Allowed_NSSAI�б���ϢʱֵΪ�ա�
     * [ʾ��]:
     *     �� ����NRPS��ע��״̬�ϱ�
     *       AT+C5GREG=1
     *       OK
     *     �� <n>=1ʱ��ѯע��״̬
     *       AT+C5GREG?
     *       +C5GREG: 1,1
     *       OK
     *     �� <n>=2ʱע����NR��ʱ��ѯע��״̬
     *       AT+C5GREG?
     *       +C5GREG: 2,1,"0000C3","000000010000001A",11,9,"01:01.123456;2F.654321"
     *       OK
     *     �� <n>=2ע����U(��G)��ʱ��ѯע��״̬
     *       AT+C5GREG?
     *       +C5GREG: 2,1
     *       OK
     *     �� ����C5GREG
     *       AT+C5GREG=?
     *       +C5GREG: (0-2)
     *       OK
     */
    { AT_CMD_C5GREG,
      AT_SetC5gregPara, AT_SET_PARA_TIME, At_QryC5gregPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+C5GREG", (VOS_UINT8 *)"(0-2)" },
#endif

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ������PDP������
     * [˵��]: ���û�ɾ����PDP�����ġ�
     *         ���������3GPP TS 27.007Э�顣
     *         ���������ֻ��<cid>һ����������AT+CGDCONT=<cid>����ʾɾ����<cid>��Ӧ����PDP�����ġ�
     *         V9R1 <CID 0>��֧��ɾ��������
     * [�﷨]:
     *     [����]: +CGDCONT=[<cid>[,<PDP_type>[,<APN>[,<PDP_addr>[,<d_comp>[,<h_comp>[,<IPv4AddrAlloc>[,<emergencyindication>[,<P-CSCF_discovery>[,<IM_CN_Signalling_Flag_Ind>[,<NSLPI>[,<securePCO>[,<IPv4_MTU_discovery>[,<Local_Addr_Ind>[,<Non-IP_MTU_discovery>[,<Reliable_Data_Service>[,<SSC_mode>[,<S-NSSAI>[,<Pref_access_type>[,<RQoS_ind>[,<MH6-PDU>[,<Always-onind>]]]]]]]]]]]]]]]]]]]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGDCONT?
     *     [���]: <CR><LF>[+CGDCONT: <cid>,<PDP_type>,<APN>,<PDP_addr>,<d_comp>,<h_comp>[,<IPv4AddrAlloc>[,<emergencyindication>[,<P-CSCF_discovery>[,<IM_CN_Signalling_Flag_Ind>[,<NSLPI>[,<securePCO>[,<IPv4_MTU_discovery>[,<Local_Addr_Ind>[,<Non-IP_MTU_discovery>[,<Reliable_Data_Service>[,<SSC_mode>[,<S-NSSAI>[,<Pref_access_type>[,<RQoS_ind>[,<MH6-PDU>[,<Always-on ind>]]]]]]]]]]]]]]]]
     *             [<CR><LF>+CGDCONT: <cid>,<PDP_type>,<APN>,<PDP_addr>,<d_comp>,<h_comp>[,<IPv4AddrAlloc>[,<emergencyindication>[,<P-CSCF_discovery>[,<IM_CN_Signalling_Flag_Ind>[,<NSLPI>[,<securePCO>[,<IPv4_MTU_discovery>[,<Local_Addr_Ind>[,<Non-IP_MTU_discovery>[,<Reliable_Data_Service>[,<SSC_mode>[,<S-NSSAI>[,<Pref_access_type>[,<RQoS_ind>[,<MH6-PDU>[,<Always-on ind>]]]]]]]]]]]]]]]]
     *             [...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGDCONT=?
     *     [���]: <CR><LF>+CGDCONT: ( range of supported <cid>s),<PDP_type>,,,(list of supported <d_comp>s),(list of supported <h_comp>s), (list of supported <IPv4AddrAlloc>s),(list of supported <emergency indication>s)[,(list of supported <P-CSCF_discovery>s),(list of supported <IM_CN_Signalling_Flag_Ind>s) ,(list of supported < NSLPI >s)[,,,,,,(list of supported <SSC_mode>s),,(list of supported <Pref_access_type>s),(list of supported <RQoS_ind>s),(list of supported <MH6-PDU>s) ,(list of supported<Always-on ind>s)]]
     *             [<CR><LF>+CGDCONT: (range of supported <cid>s), <PDP_type>,,,(list of supported <d_comp>s),(list of supported <h_comp>s) ,(list of supported <IPv4AddrAlloc>s),(list of supported <emergencyindication>s)[,(list of supported <P-CSCF_discovery>s),(list of supported <IM_CN_Signalling_Flag_Ind>s) ,(list of supported < NSLPI >s) [,,,,,,(list of supported <SSC_mode>s),,(list of supported <Pref_access_type>s),(list of supported <RQoS_ind>s),(list of supported <MH6-PDU>s) ,(list of supported<Always-on ind>s)]]
     *             [...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             ֧�ְ汾��ȡֵ��Χ0~31��
     *     <PDP_type>: �ַ������ͣ���������Э�����͡�
     *             IP������Э�飨IPV4����
     *             IPV6��IPV6Э�顣
     *             IPV4V6��IPV4��IPV6��
     *             PPP��PPPЭ�顣
     *             Ethernet����̫��Э�顣�������͵�������NV6066���ƣ�
     *     <APN>: �ַ������ͣ���������ƣ�����ѡ��GGSN���ⲿ�����������磻����ֵΪ�գ���ʹ��ǩԼֵ��
     *     <PDP_address>: �ַ������ͣ�IPV4���͵�PDP��ַ��
     *     <d_comp>: ����ֵ��PDP����ѹ����ʽ������SNDCP��Ч����
     *             0���أ�ȱʡֵ����
     *             1������������ָ������
     *             2��V.42��
     *             3��V.44���ݲ�֧�֣���
     *     <h_comp>: ����ֵ��PDPͷѹ����ʽ��
     *             0���أ�ȱʡֵ����
     *             1������������ָ������
     *             2��RFC1144������SNDCP��Ч����
     *             3��RFC2507��
     *             4��RFC3095������PDCP��Ч���ݲ�֧�֣���
     *     <IPv4AddrAlloc>: ���ƻ�ȡIPV4��ַ�ķ�ʽ���ݲ�֧�֣���
     *             0��ͨ��NAS�������IPV4��ַ��
     *             1��ͨ��DHCP����IPV4��ַ��
     *     <emergency indication>: PDP�Ƿ����ڽ������ط����ݲ�֧�֣���
     *             0��PDP�����ڽ������ط���
     *             1��PDP���ڽ������ط���
     *     <P-CSCF_discovery>: ����ֵ������MT/TA��ȡP-CSCF ��ַ�ķ�ʽ��
     *             0������+CGDCONT����
     *             1������ͨ��NAS�����ȡ
     *             2������ͨ��DHCP��ȡ
     *     <IM_CN_Signalling_Flag_Ind>: ����ֵ��ָʾ�����PDP�Ƿ������ IM CN
     *             0����PDP�Ƿ񲻽����� IM CN
     *             1����PDP�Ƿ������ IM CN
     *     <NSLPI>: ����ֵ��ָʾPDP��NAS ��������ȼ�:
     *             0: PDP�������ڼ���ʱ��������ȼ�������MT����.
     *             1: PDP�������ڼ���ʱ��������ȼ�����������Ϊ "MS is not configured for NAS ignalling low priority".��Ĭ��ֵ��
     *     <securePCO>: ����ֵ��Specifies if security protected transmission of PCO is requested or not (applicable for EPS only)
     *             0��Security protected transmission of PCO is not requested
     *             1��Security protected transmission of PCO is requested
     *             ע���ò����ݲ�֧��
     *     <IPv4_MTU_discovery>: ����ֵ��influences how the MT/TA requests to get the IPv4 MTU size
     *             0��Preference of IPv4 MTU size discovery not influenced by +CGDCONT
     *             1��Preference of IPv4 MTU size discovery through NAS signalling
     *             ע���ò����ݲ�֧��
     *     <Local_Addr_Ind>: ����ֵ��indicates to the network whether or not the MS supports local IP address in TFTs
     *             0��indicates that the MS does not support local IP address in TFTs
     *             1��indicates that the MS supports local IP address in TFTs
     *             ע���ò����ݲ�֧��
     *     <Non-IP_MTU_discovery>: ����ֵ��influences how the MT/TA requests to get the Non-IP MTU size
     *             0��Preference of Non-IP MTU size discovery not influenced by +CGDCONT
     *             1��Preference of Non-IP MTU size discovery through NAS signalling
     *             ע���ò����ݲ�֧��
     *     <Reliable_Data_Service>: ����ֵ��indicates whether the UE is using Reliable Data Service for a PDN connection or not
     *             0��Reliable Data Service is not being used for the PDN connection
     *             1��Reliable Data Service is being used for the PDN connection
     *             ע���ò����ݲ�֧��
     *     <SSC_mode>: ����ֵ������ҵ��ĻỰ�ͷ���������ģʽ
     *             0��SSC mode 1
     *             1��SSC mode 2
     *             2��SSC mode 3
     *     <S-NSSAI>: �ַ������ͣ�5GS�ķ�Ƭ��Ϣ��
     *             "sst" if only slice/service type (SST) is present
     *             "sst;mapped_sst" if SST and mapped configured SST are present
     *             "sst.sd" if SST and slice differentiator (SD) are present
     *             "sst.sd;mapped_sst" if SST, SD and mapped configured SST are present
     *             "sst.sd;mapped_sst.mapped_sd" if SST, SD, mapped configured SST and mapped configured SD are present
     *             ע�⣺�ַ����е�����Ϊ16���ƣ��ַ�ȡֵ��Χ��0-9��A-F,a-f��
     *     <Pref_access_type>: ����ֵ��5GSϵͳ�����Ƚ�������
     *             0��3GPP access
     *             1��non-3GPP access
     *             ע���ò���ֻ֧��0
     *     <RQoS_ind>: ����ֵ��PDU session�Ƿ�֧�ַ���QoS��Ĭ��ֵ0
     *             0��indicates that reflective QoS is not supported for the PDU session
     *             1��indicates that reflective QoS is supported for the PDU session
     *     <MH6-PDU>: ����ֵ��UE����PDU sessionʱ�Ƿ�֧��IPv6 multi-homing
     *             0����֧��
     *             1��֧��
     *             ע���ò���ֻ֧��0
     *     <Always-on ind>: ����ֵ���Ƿ�Ϊ  always-on PDU session
     *             0����
     *             1����
     * [ʾ��]:
     *       ������PDP������
     *     �� ��֧��5GSʱ
     *       AT+CGDCONT=1,"IP","ABCDEF","200.1.1.80",1,2,0,0,0,0,0
     *       OK
     *     �� ֧��5GSʱ
     *       AT+CGDCONT=1,"IP","ABCDEF","200.1.1.80",1,2,0,0,0,0,0,,,,,,1,"1.123456;2F.654321",0,1,0,0
     *       OK
     *       ��ѯ����
     *     �� ��֧��5GSʱ
     *       AT+CGDCONT?
     *       +CGDCONT: 1,"IP","ABCDEF","200.1.1.80",1,2,0,0,0,0,0
     *       +CGDCONT: 2,"IP6","abc.com",,0,0,0,0,0,0
     *       +CGDCONT: 3,"IPV4V6","huawei.com",,0,0,0,0,1,1
     *       OK
     *     �� ֧��5GSʱ
     *       AT+CGDCONT?
     *       +CGDCONT: 1,"IP","ABCDEF","200.1.1.80",1,2,0,0,0,0,0,,,,,,
     *       1,"1.1234",0,1,0,0
     *       +CGDCONT: 2,"IP6","abc.com",,0,0,0,0,0,0,,,,,,1,"1.1234",
     *       0,0,0,0
     *       +CGDCONT: 3,"IPV4V6","huawei.com",,0,0,0,0,1,1,,,,,,
     *       0,"1.1234",0,1,0,0
     *       +CGDCONT: 4,"Ethernet","huawei.com",,0,0,0,0,0,1
     *       OK
     *       �������֧��IPV6����ʱ��
     *     �� ��֧��5GSʱ
     *       AT+CGDCONT=?
     *       +CGDCONT: (0-31),"IP",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)
     *       +CGDCONT: (0-31),"IPV6",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)
     *       +CGDCONT: (0-31),"IPV4V6",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)
     *       +CGDCONT: (0-31),"PPP",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)
     *       OK
     *     �� ֧��5GS��NV6066ʹ��ʱ
     *       AT+CGDCONT=?
     *       +CGDCONT: (0-31),"IP",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0-1),(0-1),(0-1),(0-1)
     *       +CGDCONT: (0-31),"IPV6",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0-1),(0-1),(0-1),(0-1)
     *       +CGDCONT: (0-31),"IPV4V6",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0-1),(0-1),(0-1),(0-1)
     *       +CGDCONT: (0-31),"PPP",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0-1),(0-1),(0-1),(0-1)
     *       (0-31),"Ethernet",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0-1),(0-1),(0-1),(0-1)
     *       OK
     */
    { AT_CMD_CGDCONT,
      AT_SetCgdcontPara, AT_SET_PARA_TIME, AT_QryCgdcontPara, AT_QRY_PARA_TIME, At_TestCgdcont, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGDCONT", (VOS_UINT8 *)CGDCONT_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: �������PDP������
     * [˵��]: ���û�ɾ������PDP�����ġ�
     *         ���������ֻ��<cid>һ����������AT+CGDSCONT=<cid>����ʾɾ����<cid>��Ӧ�Ķ���PDP�����ġ�
     *         һ��PDP�����Ĳ��ܼ�����PDP�����ģ����Ƕ���PDP�����ġ�����ĳCID�Ⱥ�ִ����+CGDCONT�����+CGDSCONT��������һ��ִ�е�AT������Ч��
     *         ����PDP�����ĺ����Ӧ����PDP�����Ķ�ֻ����IP���͵�PDP�����ġ�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGDSCONT=[<cid>,<p_cid>[,<d_comp>[,<h_comp>[,<IM_CN_Signalling_Flag_Ind>]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGDSCONT?
     *     [���]: <CR><LF>+CGDSCONT: <cid>,<p_cid>,<d_comp>,<h_comp>
     *             [<CR><LF>+CGDSCONT: <cid>,<p_cid>,<d_comp>,<h_comp>,<IM_CN_Signalling_Flag_Ind> [...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGDSCONT=?
     *     [���]: <CR><LF>+CGDSCONT: (list of supported <cid>s),(list of <p_cid>s for active primary contexts),(list of supported <d_comp>s),(list of supported <h_comp>s) ,(list of
     *             supported <IM_CN_Signalling_Flag_Ind>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             V9R1�汾��ȡֵ��Χ1~31��
     *     <p_cid>: ����ֵ����PDP�����ı�ʶ����
     *             V9R1�汾��ȡֵ��Χ0~31��
     *     <d_comp>: ����ֵ��PDP����ѹ����ʽ������SNDCP��Ч����
     *             0���أ�ȱʡֵ����
     *             1������������ָ������
     *             2��V.42��
     *             3��V.44���ݲ�֧�֣���
     *     <h_comp>: ����ֵ��PDPͷѹ����ʽ��
     *             0���أ�ȱʡֵ����
     *             1������������ָ������
     *             2��RFC1144������SNDCP��Ч����
     *             3��RFC2507��
     *             4��RFC3095������PDCP��Ч���ݲ�֧�֣���
     *     <IM_CN_Signalling_Flag_Ind>: IMS���ر�־��
     *             0��������IM CN��ϵͳ��ص��ź�
     *             1������IM CN��ϵͳ��ص��ź�
     * [ʾ��]:
     *     �� �������PDP������
     *       AT+CGDSCONT=2,1,1,2,0
     *       OK
     *     �� ��ѯ����
     *       AT+CGDSCONT?
     *       +CGDSCONT: 2,1,1,2,0
     *       +CGDSCONT: 3,1,1,0
     *       OK
     *     �� ��������
     *       AT+CGDSCONT=?
     *       +CGDSCONT: (1-31),(0-31),(0-2),(0-3),(0-1)
     *       OK
     */
    { AT_CMD_CGDSCONT,
      AT_SetCgdscontPara, AT_SET_PARA_TIME, AT_QryCgdscontPara, AT_QRY_PARA_TIME, At_TestCgdscont, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGDSCONT", (VOS_UINT8 *)CGDSCONT_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����TFT
     * [˵��]: ���û�ɾ��PDP�����ĵı��Ĺ�������ÿ��PDP������ֻ֧��һ�����Ĺ���������
     *         ���������ֻ��<cid>һ����������AT+CGTFT=<cid>����ʾɾ����<cid>��Ӧ��PDP�����ĵı��Ĺ�������
     *         ���������3GPP TS 27.007Э�顣
     *         �����������õĲ��������ѭ��2-16�е����Ҫ��
     *         ע������еġ�X�����ű�ʾ������Ч�����ѡ���ò�����Ϊ�����ʾ������Ч����в���ѡ���ò�����
     * [�﷨]:
     *     [����]: +CGTFT=[<cid>,[<packetfilteridentifier>,<evaluationprecedenceindex>[,<remoteaddressandsubnetmask>[,<protocolnumber(ipv4)/nextheader(ipv6)>[,<sourceportrange>[,<destinationportrange>[,<ipsecsecurityparameterindex(spi)>[,<typeofservice(tos)(ipv4)andmask/trafficclass(ipv6)andmask>[,<flowlabel(ipv6)>[,<direction>[,<localaddressandsubnetmask>[,<QRI>]]]]]]]]]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGTFT?
     *     [���]: <CR><LF> [+CGTFT: <cid>, <packet filter identifier>, <evaluation precedence index>, < remote address and subnet mask>, <protocol number (ipv4) / next header (ipv6)>, <source port range>,<destination port range>,<ipsec security parameter index (spi)>,<type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>,<flow label (ipv6)>,<direction>,<local address and subnet mask>,<QRI>]
     *             [<CR><LF>+CGTFT: <cid>, <packet filter identifier>, <evaluation precedence index>, < remote address and subnet mask>, <protocol number (ipv4) / next header (ipv6)>,<source port range>,<destination port range>,<ipsec security parameter index (spi)>,<type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>,<flow label (ipv6)>,<direction>,<local address and subnet mask>,<QRI>]
     *             [...]] <CR><LF>
     *     [����]: +CGTFT=?
     *     [���]: <CR><LF>+CGTFT: <PDP_type>, (list of supported <packet filter identifier>s), (list of supported <evaluation precedence index>s), (list of supported < remote address and subnet mask>s), (list of supported <protocol number (ipv4) / next header (ipv6)>s), (list of supported<source port range>s),(list of supported<destination port range>s) ,(list of supported <ipsec security parameter index (spi)>s),(list of supported <type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>s),(list of supported <flow label (ipv6)>s),(list of supported <direction>s),(list of supported <local address and subnet mask>s),(range of supported <QRI>s)
     *             [<CR><LF>+CGTFT: <PDP_type>, (list of supported <packet filter identifier>s), (list of supported <evaluation precedence index>s), (list of supported < remote address and subnet mask>s), ((list of supported <protocol number (ipv4) / next header (ipv6)>s), (list of supported<source port range>s),(list of supported<destination port range>s),(list of supported <ipsec security parameter index (spi)>s),(list of supported <type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>s),(list of supported <flow label (ipv6)>s),(list of supported <direction>s),(list of supported <local address and subnet mask>s),(range of supported <QRI>s)
     *             [...]] <CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             V9R1�汾��ȡֵ��Χ1~31��
     *     <packet filter identifier>: ����ֵ�����Ĺ�������ʶ��ȡֵ��ΧΪ1��16��Ŀǰ��֧��1��
     *     <remote address and subnet mask>: IP��ַ�ַ������ͣ�Զ��IP��ַ���������룬Ŀǰ��֧��IPV4��ַ���ͣ���ʽΪ��a1.a2.a3.a4.m1.m2.m3.m4����
     *     <protocol number (ipv4) / next header (ipv6)>: ����ֵ��IPV4��Э��ţ�IPV6 next header��ȡֵ��ΧΪ0��255��
     *     <source port range>: �ַ������ͣ�Դ�˿ڷ�Χ��ȡֵ��ΧΪ0��65535����ʽΪ��from.to����
     *     <destination port range>: �ַ������ͣ�Ŀ��˿ڷ�Χ��ȡֵ��ΧΪ0��65535����ʽΪ��from.to����
     *     <ipsec security parameter index (spi)>: �ַ������͵�16��������ipsec��ȫ����������ȡֵ��ΧΪ0x0000_0000��0xFFFF_FFFF��
     *     <type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>: �ַ������ͣ���IPV4���������ͣ���IPV6��ҵ�����������룬ȡֵ��ΧΪ0��255����ʽΪ��tos.mask����
     *     <evaluation precedence index>: ����ֵ�����ȼ���ȡֵ��ΧΪ0��255��
     *     <flow label (ipv6)>: ����ǩ��ȡֵ��Χ0~1048575��
     *     <direction>: ���˵ķ���
     *             0��R7֮ǰЭ��ʹ�ã�
     *             1�����У�
     *             2�����У�
     *             3��˫��
     *     <local address and subnet mask>: IP��ַ�ַ������ͣ�����IP��ַ���������룬Ŀǰ��֧��IPV4��ַ���ͣ���ʽΪ��a1.a2.a3.a4.m1.m2.m3.m4����
     *             ע�⣺ֻ��R11���ϰ汾֧�ִ˲���
     *     <QRI>: ����ֵ��Qos rule��identity��Э��֧��0-255.
     *             ע�⣺ֻ��R15���ϰ汾֧�ִ˲���
     * [ʾ��]:
     *     �� ��������
     *       AT+CGTFT=1,1,1,"10.10.10.10.255.255.0.0",,,,,"255.255"
     *       OK
     *     �� ��ѯ����
     *       AT+CGTFT?
     *       +CGTFT: 1,1,1,"10.10.10.10.255.255.0.0",,,,,"255.255",,0,0
     *       OK
     *     �� �������R11���°汾��
     *       AT+CGTFT=?
     *       +CGTFT: "IP",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)
     *       +CGTFT: "IPV6",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)
     *       +CGTFT: "IPV4V6",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)
     *       OK
     */
    { AT_CMD_CGTFT,
      AT_SetCgtftPara, AT_SET_PARA_TIME, AT_QryCgtftPara, AT_QRY_PARA_TIME, At_TestCgtft, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGTFT", (VOS_UINT8 *)CGTFT_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����PDP
     * [˵��]: �����ȥ����<cid>��ʶ��PDP�����ġ�
     *         ĿǰBalong��֧��һ�μ�����PDP��Ŀǰһ��ֻ֧�ּ���һ��PDP��
     *         ��֮ǰͨ��PPP���ŵķ�ʽ������PDP������ʹ�ø�AT���������ӦPDP��ȥ���������
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGACT=[<state>,<cid>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGACT?
     *     [���]: <CR><LF>+CGACT: <cid>,<state>
     *             [<CR><LF>+CGACT: <cid>,<state>[...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGACT=?
     *     [���]: <CR><LF>+CGACT: (list of supported <state>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <state>: ����ֵ��ָʾPDP�����ļ����״̬��
     *             0��δ���
     *             1���Ѽ��
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             V9R1�汾��ȡֵ��Χ1~31��
     * [ʾ��]:
     *     �� ���ȥ����PDP������
     *       AT+CGACT=1,1
     *       OK
     *       AT+CGACT=0,1
     *       OK
     *     �� ��������
     *       AT+CGACT=?
     *       +CGACT: (0,1)
     *       OK
     *       ��ѯ����
     *       ע:����ʾ�������ο���ʵ�ʷ��ؽ�����û�ʵ��ʹ��cid����й� ��
     *     �� cid21��31Ĭ�Ϸ���
     *       AT+CGACT?
     *       +CGACT: 1,1
     *       +CGACT: 21,0
     *       +CGACT: 22,0
     *       +CGACT: 23,0
     *       +CGACT: 24,0
     *       +CGACT: 25,0
     *       +CGACT: 26,0
     *       +CGACT: 27,0
     *       +CGACT: 28,0
     *       +CGACT: 29,0
     *       +CGACT: 30,0
     *       +CGACT: 31,0
     *       OK
     */
    { AT_CMD_CGACT,
      AT_SetCgactPara, AT_ACT_PDP_TIME, AT_QryCgactPara, AT_QRY_PARA_TIME, At_TestCgact, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGACT", (VOS_UINT8 *)CGACT_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: �޸�PDP
     * [˵��]: ��PDP���ڼ���״̬��ʹ��+CGEQREQ��+CGTFT��������PDP�����ĵ�QoS��TFT��Traffic Flow Template����������ֵ����������Ч��ʹ��+CGCMOD���޸�<cid>����ʶ��PDP�����ģ�ʹQoS��TFT����ֵ��Ч��
     *         ĿǰBalong��֧��һ���޸Ķ��PDP��Ŀǰһ��ֻ֧���޸�һ��PDP��
     *         NRģʽ�£�ʹ�ô���������һ��QOS RULE������һ������PF��������4�������ݲ�֧��ʹ�ô������޸������·���TFT��������֧�ֶ���ͬһ��CID�����MODIFY������
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGCMOD=<cid>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGCMOD=?
     *     [���]: <CR><LF>+CGCMOD: (list of <cid>s associated with active contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             ֧��LTE��ƽ̨ȡֵ��ΧΪ1~31��
     *             ��֧��LTE��ƽ̨ȡֵ1~11��
     * [ʾ��]:
     *     �� ����PDP�������޸�
     *       AT+CGCMOD=1
     *       OK
     *     �� ��������
     *       AT+CGCMOD=?
     *       +CGCMOD: (1)
     *       OK
     */
    { AT_CMD_CGCMOD,
      AT_SetCgcmodPara, AT_MOD_PDP_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgcmod, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGCMOD", (VOS_UINT8 *)CGCMOD_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯPDP��ַ
     * [˵��]: ��ѯ<cid>����ʶ��PDP������ʵ��ʹ�õ�IP��ַ�����������<cid>�򷵻����м����PDP������ʵ��ʹ�õ�IP��ַ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGPADDR[=<cid>[,<cid>[,...]]]
     *     [���]: [<CR><LF>+CGPADDR: <cid>,<PDP_addr>
     *             [<CR><LF>+CGPADDR: <cid>,<PDP_addr>[��]] <CR><LF>]
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGPADDR
     *     [���]: [<CR><LF>+CGPADDR: <cid>,<PDP_addr>
     *             [<CR><LF>+CGPADDR: <cid>,<PDP_addr>[��]] <CR><LF>]
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGPADDR=?
     *     [���]: <CR><LF>+CGPADDR: (list of defined <cid>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *     <PDP_addr>: �ַ������ͣ�PDP��ַ��
     * [ʾ��]:
     *     �� ��ȡĳ��IPV4�Ѽ����PDP������ʹ�õ�PDP��ַ
     *       AT+CGPADDR=1
     *       +CGPADDR: 1,"10.101.2.15"
     *       OK
     *     �� ��ȡĳ��IPV6�Ѽ����PDP������ʹ�õ�PDP��ַ
     *       AT+CGPADDR=1
     *       +CGPADDR: 1,"32.8.0.2.0.2.0.1.255.255.255.255.255.255.255.255"
     *       OK
     *     �� ��������г������Ѿ������CID
     *       AT+CGDCONT=1,"ip","huawei.com"
     *       OK
     *       AT+CGDCONT=2,"ip","huawei1.com"
     *       OK
     *       AT+CGPADDR=?
     *       +CGPADDR: (1,2)
     *       OK
     */
    { AT_CMD_CGPADDR,
      AT_SetCgpaddrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgpaddr, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGPADDR", (VOS_UINT8 *)CGPADDR_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯЭ��QoS
     * [˵��]: ��ѯ<cid>����ʶ��PDP�����ļ���󾭹�������Э�̺��ʵ��ʹ�õ�QoS��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGEQNEG[=<cid>[,<cid>[,...]]]
     *     [���]: <CR><LF> [+CGEQNEG: <cid>, <Traffic class> ,<Maximum bitrate UL>,<Maximum bitrate DL> ,<Guaranteed bitrate UL>, <Guaranteedbitrate DL> ,<Delivery order> ,<Maximum SDU size> ,<SDU error ratio> ,<Residual bit error ratio> ,<Delivery of erroneous SDUs> ,<Transfer delay> ,<Traffic handling priority>[��]] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGEQNEG=?
     *     [���]: <CR><LF>+CGEQNEG: (list of <cid>s associated with active contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             V9R1�汾��ȡֵ��Χ1~31��
     *     <Traffic class>: ����ֵ��ҵ�����͡�
     *             0��Conversational���Ự���ͣ���
     *             1��Streaming�������ͣ���
     *             2��Interactive���������ͣ���
     *             3��Background���������ͣ���
     *             4��Subscribed value��ǩԼֵ����
     *     <Maximum bitrate UL>: ����ֵ�������������ʣ���λΪkbit/s����ȡֵ��ΧΪ0��10000000��
     *             0��Э��ֵ��
     *             1��10000000��1kbit/s��10000000kbit/s��
     *     <Maximum bitrate DL>: ����ֵ�������������ʣ���λΪkbit/s����ȡֵ��ΧΪ0��10000000��
     *             0��Э��ֵ��
     *             1��10000000��1kbit/s��10000000kbit/s��
     *     <Guaranteed bitrate UL>: ����ֵ�����пɱ�֤�����ʣ���λΪkbit/s����ȡֵ��ΧΪ0��10000000��
     *             0��Э��ֵ��
     *             1��10000000��1kbit/s��10000000  kbit/s��
     *     <Guaranteed bitrate DL>: ����ֵ�����пɱ�֤�����ʣ���λΪkbit/s����ȡֵ��ΧΪ0��10000000��
     *             0��Э��ֵ��
     *             1��10000000��1kbit/s��10000000kbit/s��
     *     <Delivery order>: ����ֵ���Ƿ�˳����SDU��
     *             0����˳���ԣ�
     *             1����˳���ԣ�
     *             2��Э��ֵ��
     *     <Maximum SDU size>: ����ֵ�����SDU���ȣ���λΪbyte����ȡֵ��ΧΪ0��1520��
     *             0��Э��ֵ��
     *             1��1520��1byte��1520byte��
     *     <SDU error ratio>: �ַ������ͣ�SDU�����ʡ�
     *             0E0��Э��ֵ��
     *             1E2��1*10-2��
     *             7E3��7*10-3��
     *             1E3��1*10-3��
     *             1E4��1*10-4��
     *             1E5��1*10-5��
     *             1E6��1*10-6��
     *             1E1��1*10-1��
     *     <Residual bit error ratio>: �ַ������ͣ�SDU�������������ʡ�
     *             0E0��Э��ֵ��
     *             5E2��5*10-2��
     *             1E2��1*10-2��
     *             5E3��5*10-3��
     *             4E3��4*10-3��
     *             1E3��1*10-3��
     *             1E4��1*10-4��
     *             1E5��1*10-5��
     *             1E6��1*10-6��
     *             6E8��6*10-8��
     *     <Delivery of erroneous SDUs>: ����ֵ�������SDU�Ƿ񱻷��͡�
     *             0�������ͣ�
     *             1�����ͣ�
     *             2������⣻
     *             3��Э��ֵ��
     *     <Transfer delay>: ����ֵ������ʱ�ӣ���λΪms����ȡֵ��ΧΪ0��4100��
     *             0��Э��ֵ��
     *             1��4100��1ms��4100ms��
     *     <Traffic handling priority>: ����ֵ�����ȼ���
     *             0��Э��ֵ��
     *             1�����ȼ�1��
     *             2�����ȼ�2��
     *             3�����ȼ�3��
     *     <PDP_type>: �ַ������ͣ���������Э�����͡�
     *             IP������Э�飨IPV4����
     *             IPV6��IPV6Э�顣
     *             IPV4V6��IPV4��IPV6��
     * [ʾ��]:
     *     �� ��ѯPDP�����ļ����ʵ��ʹ�õ�QoS
     *       AT+CGEQNEG=1
     *       +CGEQNEG: 1,2,64,64,0,0,1,320,"1E4","1E5",1,0,3
     *       OK
     *     �� ��������
     *       AT+CGEQNEG=?
     *       +CGEQNEG: (1)
     *       OK
     */
    { AT_CMD_CGEQNEG,
      AT_SetCgeqnegPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgeqnegPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGEQNEG", (VOS_UINT8 *)CGEQNEG_CMD_PARA_STRING },

    { AT_CMD_CGANS,
      AT_SetCgansPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgansPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGANS", (VOS_UINT8 *)CGANS_CMD_PARA_STRING },

    { AT_CMD_CGANS_EXT,
      AT_SetCgansPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgansExtPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CGANS", (VOS_UINT8 *)CGANS_EXT_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��������״̬
     * [˵��]: ��������״̬�����<cid>����ʶ��PDP������û�м�����ȼ����ٽ�������״̬��
     *         ���������3GPP TS 27.007Э�顣
     *         ע�⣺�������Ǹ��˿ڷ��͸�AT�������ʹ�ö˿ڴ�����̬�л�������̬���˺�ö˿ڽ����ܴ���AT���
     * [�﷨]:
     *     [����]: +CGDATA=[<L2P>,[<cid>]]
     *     [���]: <CR><LF>CONNECT<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGDATA=?
     *     [���]: <CR><LF>+CGDATA: (list of supported <L2P>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <L2P>: �ַ������ͣ���ʶTE��MT֮��ʹ�õĲ����Э�飬�ò���Ŀǰ��֧��NULL���޲��Э�飩��
     *             NULL��none, for PDP type OSP:IHOSS (Obsolete)
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             V9R1�汾��ȡֵ��Χ1~31��
     * [ʾ��]:
     *     �� ��������״̬
     *       AT+CGDATA="NULL",1
     *       CONNECT
     *     �� ��������
     *       AT+CGDATA=?
     *       +CGDATA: ("NULL")
     *       OK
     */
    { AT_CMD_CGDATA,
      At_SetCgdataPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgdataPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGDATA", (VOS_UINT8 *)CGDATA_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����QoS����
     * [˵��]: ���û�ɾ��PDP�����������QoS��Quality of Service��������
     *         ���������ֻ��<cid>һ����������AT+CGEQREQ=<cid>����ʾɾ����<cid>��Ӧ��PDP�����ĵ�QoS��
     *         ��δʹ�ø���������QoS��������PDP�������ʹ�õ�QoS����ֵȡ����HLR��ǩԼ��Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: AT+CGEQREQ=[<cid>[,<Trafficclass>[,<MaximumbitrateUL>[,<MaximumbitrateDL>[,<GuaranteedbitrateUL>[,<GuaranteedbitrateDL>[,<Deliveryorder>[,<MaximumSDUsize>[,<SDUerrorratio>[,<Residualbiterrorratio>[,<DeliveryoferroneousSDUs>[,<Transferdelay>[,<Traffichandlingpriority>[,<Sourcestatisticsdescriptor>[,<Signallingindication>]]]]]]]]]]]]]]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: +CGEQREQ?
     *     [���]: <CR><LF>+CGEQREQ: <cid>,<Traffic
     *             class>,<Maximum bitrate UL> ,<Maximum bitrate
     *             DL> ,<Guaranteed bitrate UL> ,<Guaranteed bitrate
     *             DL> ,<Delivery order> ,<Maximum SDU
     *             size> ,<SDU error ratio> ,<Residual bit error
     *             ratio> ,<Delivery of erroneous SDUs> ,<Transfer
     *             delay> ,<Traffic handling priority>,<Source Statistics Descriptor>,<Signalling Indication>
     *             [<CR><LF>+CGEQREQ: <cid>, <Traffic
     *             class> ,<Maximum bitrate UL> ,<Maximum bitrate
     *             DL> ,<Guaranteed bitrate UL> ,<Guaranteed bitrate
     *             DL> ,<Delivery order> ,<Maximum SDU
     *             size> ,<SDU error ratio> ,<Residual bit error
     *             ratio> ,<Delivery of erroneous SDUs> ,<Transfer
     *             delay> ,<Traffic handling priority>,<Source Statistics Descriptor>,<Signalling Indication>
     *             [...]] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGEQREQ=?
     *     [���]: +CGEQREQ: <PDP_type>, (list of supported <Traffic class>s) ,(list of supported <Maximum bitrate UL>s), (list of supported <Maximum bitrate DL>s), (list of supported <Guaranteed bitrate UL>s), (list of supported <Guaranteed bitrate DL>s),(list of supported <Delivery order>s) ,(list of supported <Maximum SDU size>s) ,(list of supported <SDU error ratio>s) ,(list of supported <Residual bit error ratio>s) ,(list of supported <Delivery of erroneous SDUs>s) ,(list of supported <Transfer delay>s) ,(list of supported <Traffic handling priority>s) ,(list of supported <Source statistics descriptor>s),(list of supported <Signalling indication>s)
     *             [<CR><LF>+CGEQREQ: <PDP_type>, (list of supported <Traffic class>s) ,(list of supported <Maximum bitrate UL>s), (list of supported <Maximum bitrate DL>s), (list of supported <Guaranteed bitrate UL>s), (list of supported <Guaranteed bitrate DL>s),(list of supported <Delivery order>s) ,(list of supported <Maximum SDU size>s) ,(list of supported <SDU error ratio>s) ,(list of supported <Residual bit error ratio>s) ,(list of supported <Delivery of erroneous SDUs>s) ,(list of supported <Transfer delay>s) ,(list of supported <Traffic handling priority>s) ,(list of supported <Source statistics descriptor>s),(list of supported <Signalling indication>s)
     *             [...]]
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����ȡֵ��Χ1~31��
     *     <Traffic class>: ����ֵ��ҵ�����͡�
     *             0��Conversational���Ự���ͣ���
     *             1��Streaming�������ͣ���
     *             2��Interactive���������ͣ���
     *             3��Background���������ͣ���
     *             4��Subscribed value��ǩԼֵ����
     *     <Maximum bitrate UL>: ����ֵ�������������ʣ���λΪkbit/s����ȡֵ��ΧΪ0��11480��
     *             0��Э��ֵ��
     *             1��11480��1kbit/s��11480kbit/s��
     *     <Maximum bitrate DL>: ����ֵ�������������ʣ���λΪkbit/s����ȡֵ��ΧΪ0��42000��
     *             0��Э��ֵ��
     *             1��42000��1kbit/s��42000kbit/s��
     *     <Guaranteed bitrate UL>: ����ֵ�����пɱ�֤�����ʣ���λΪkbit/s����ȡֵ��ΧΪ0��11480��
     *             0��Э��ֵ��
     *             1��11480��1kbit/s��11480kbit/s��
     *     <Guaranteed bitrate DL>: ����ֵ�����пɱ�֤�����ʣ���λΪkbit/s����ȡֵ��ΧΪ0��42000��
     *             0��Э��ֵ��
     *             1��42000��1kbit/s��42000kbit/s��
     *     <Delivery order>: ����ֵ���Ƿ�˳����SDU��
     *             0����˳���ԣ�
     *             1����˳���ԣ�
     *             2��Э��ֵ��
     *     <Maximum SDU size>: ����ֵ�����SDU���ȣ���λΪbyte����ȡֵ��ΧΪ0��1520��
     *             0��Э��ֵ��
     *             1��1520��1byte��1520byte��
     *     <SDU error ratio>: �ַ������ͣ�SDU�����ʡ�
     *             0E0��Э��ֵ��
     *             1E2��1*10-2��
     *             7E3��7*10-3��
     *             1E3��1*10-3��
     *             1E4��1*10-4��
     *             1E5��1*10-5��
     *             1E6��1*10-6��
     *             1E1��1*10-1��
     *     <Residual bit error ratio>: �ַ������ͣ�SDU�������������ʡ�
     *             0E0��Э��ֵ��
     *             5E2��5*10-2��
     *             1E2��1*10-2��
     *             5E3��5*10-3��
     *             4E3��4*10-3��
     *             1E3��1*10-3��
     *             1E4��1*10-4��
     *             1E5��1*10-5��
     *             1E6��1*10-6��
     *             6E8��6*10-8��
     *     <Delivery of erroneous SDUs>: ����ֵ�������SDU�Ƿ񱻷��͡�
     *             0�������ͣ�
     *             1�����ͣ�
     *             2������⣻
     *             3��Э��ֵ��
     *     <Transfer delay>: ����ֵ������ʱ�ӣ���λΪms����ȡֵ��ΧΪ0��4100��
     *             0��Э��ֵ��
     *             1��4100��1ms��4100ms��
     *     <Traffic handling priority>: ����ֵ�����ȼ���
     *             0��Э��ֵ��
     *             1�����ȼ�1��
     *             2�����ȼ�2��
     *             3�����ȼ�3��
     *     <PDP_type>: �ַ������ͣ���������Э�����͡�
     *             IP������Э�飨IPV4����
     *             IPV6��IPV6Э�顣
     *             IPV4V6��IPV4��IPV6��
     *     <Source Statistics Descriptor>: ����ֵ��ͳ��Դ��������
     *             0��δ֪��SDU����
     *             1������Դ���SDU����
     *     <Signalling Indication>: ����ֵ������������
     *             0��δ�Ż�����
     *             1�����Ż�����
     * [ʾ��]:
     *     �� ����PDP�����ĵ�QoS
     *       AT+CGEQREQ=2,2,64,64,,,1,320,"1E4","1E5",1,,3,0,0
     *       OK
     *     �� ��ѯ����
     *       AT+CGEQREQ?
     *       +CGEQREQ: 2,2,64,64,,,1,320,"1E4","1E5",1,,3,0,0
     *       OK
     *       ��������
     *     �� ���������������ݰ汾֧�ֵ�Э��汾����ͬ�����磬֧��R9��Э��汾�������������Ĳ����б����£�
     *       AT+CGEQREQ=?
     *       +CGEQREQ: "IP",(0-4),(0-11480),(0-42000),(0-11480),(0-42000),(0-2),(0-1520),("0E0","1E2","7E3","1E3","1E4","1E5","1E6","1E1"),("0E0","5E2","1E2","5E3","4E3","1E3","1E4","1E5","1E6","6E8"),(0-3),(0-4100),(0-3),(0-1),(0-1)
     *       +CGEQREQ: "IPV6",(0-4),(0-11480),(0-42000),(0-11480),(0-42000),(0-2),(0-1520),("0E0","1E2","7E3","1E3","1E4","1E5","1E6","1E1"),("0E0","5E2","1E2","5E3","4E3","1E3","1E4","1E5","1E6","6E8"),(0-3),(0-4100),(0-3),(0-1),(0-1)
     *       +CGEQREQ: "IPV4V6",(0-4),(0-11480),(0-42000),(0-11480),(0-42000),(0-2),(0-1520), ("0E0","1E2","7E3","1E3","1E4","1E5","1E6","1E1"), ("0E0","5E2","1E2","5E3","4E3","1E3","1E4","1E5","1E6","6E8"),(0-3),(0-4100),(0-3),(0-1),(0-1)
     *       OK
     */
    { AT_CMD_CGEQREQ,
      AT_SetCgeqreqPara, AT_SET_PARA_TIME, AT_QryCgeqreqPara, AT_QRY_PARA_TIME, At_TestCgeqreqPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGEQREQ", (VOS_UINT8 *)CGEQREQ_CMD_PARA_STRING },

    /* +CGEQMIN����Ĳ�����ʾ�����+CGEQREQ��һ�µģ���ʹ��ͬһ�����Ժ��� */
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ������СQoS
     * [˵��]: ���û�ɾ��PDP�����ĵ���С�ɽ��ܷ���������QoS����
     *         ���������ֻ��<cid>һ����������AT+CGEQMIN=<cid>����ʾɾ����<cid>��Ӧ��PDP�����ĵ���С�ɽ��ܷ���������
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGEQMIN=[<cid>[,<Trafficclass>[,<MaximumbitrateUL>[,<MaximumbitrateDL>[,<GuaranteedbitrateUL>[,<GuaranteedbitrateDL>[,<Deliveryorder>[,<MaximumSDUsize>[,<SDUerrorratio>[,<Residualbiterrorratio>[,<DeliveryoferroneousSDUs>[,<Transferdelay>[,<Traffichandlingpriority>[,<Sourcestatisticsdescriptor>[,<Signallingindication>]]]]]]]]]]]]]]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: +CGEQMIN?
     *     [���]: +CGEQMIN: <cid>, <Traffic class> ,<Maximum bitrate UL>, <Maximum bitrate DL> ,<Guaranteed bitrate UL> ,<Guaranteed bitrate DL>, <Delivery order> ,<Maximum SDU size> ,<SDU error ratio> ,<Residual bit error ratio> ,<Delivery of erroneous SDUs> ,<Transfer delay> ,<Traffic handling priority>,<Source Statistics Descriptor>,<Signalling Indication>
     *             [<CR><LF>+CGEQMIN: <cid>, <Traffic class> ,<Maximum bitrate UL> ,<Maximum bitrate DL> ,<Guaranteed bitrate UL> ,<Guaranteed bitrate DL>, <Delivery order> ,<Maximum SDU size> ,<SDU error ratio> ,<Residual bit error ratio> ,<Delivery of erroneous SDUs> ,<Transfer delay> ,<Traffic handling priority>,<Source Statistics Descriptor>,<Signalling Indication>
     *             [...]]
     *     [����]: +CGEQMIN=?
     *     [���]: +CGEQMIN: <PDP_type>, (list of supported <Traffic class>s) ,(list of supported <Maximum bitrate UL>s) ,(list of supported <Maximum bitrate DL>s), (list of supported <Guaranteed bitrate UL>s), (list of supported <Guaranteed bitrate DL>s) ,(list of supported <Delivery order>s) ,(list of supported <Maximum SDU size>s) ,(list of supported <SDU error ratio>s) ,(list of supported <Residual bit error ratio>s) ,(list of supported <Delivery of erroneous SDUs>s) ,(list of supported <Transfer delay>s) ,(list of supported <Traffic handling priority>s) ,(list of supported <Source statistics descriptor>s),(list of supported <Signalling indication>s)
     *             [<CR><LF>+CGEQMIN: <PDP_type>, (list of supported <Traffic class>s) ,(list of supported <Maximum bitrate UL>s), (list of supported <Maximum bitrate DL>s) ,(list of supported <Guaranteed bitrate UL >s), (list of supported <Guaranteed bitrate DL >s) ,(list of supported <Delivery order>s) ,(list of supported <Maximum SDU size>s) ,(list of supported <SDU error ratio>s) ,(list of supported <Residual bit error ratio>s) ,(list of supported <Delivery of erroneous SDUs>s) ,(list of supported <Transfer delay>s) ,(list of supported <Traffic handling priority>s) ,(list of supported <Source statistics descriptor>s),(list of supported <Signalling indication>s)
     *             [...]]
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����ȡֵ��Χ1~31��
     *     <Traffic class>: ����ֵ��ҵ�����͡�
     *             0��Conversational���Ự���ͣ���
     *             1��Streaming�������ͣ���
     *             2��Interactive���������ͣ���
     *             3��Background���������ͣ���
     *             4��Subscribed value��ǩԼֵ����
     *     <Maximum bitrate UL>: ����ֵ�������������ʣ���λΪkbit/s����ȡֵ��ΧΪ0��11480��
     *             0��Э��ֵ��
     *             1��11480��1kbit/s��11480kbit/s��
     *     <Maximum bitrate DL>: ����ֵ�������������ʣ���λΪkbit/s����ȡֵ��ΧΪ0��42000��
     *             0��Э��ֵ��
     *             1��42000��1kbit/s��42000kbit/s��
     *     <Guaranteed bitrate UL>: ����ֵ�����пɱ�֤�����ʣ���λΪkbit/s����ȡֵ��ΧΪ0��11480��
     *             0��Э��ֵ��
     *             1��11480��1kbit/s��11480kbit/s��
     *     <Guaranteed bitrate DL>: ����ֵ�����пɱ�֤�����ʣ���λΪkbit/s����ȡֵ��ΧΪ0��42000��
     *             0��Э��ֵ��
     *             1��42000��1kbit/s��42000kbit/s��
     *     <Delivery order>: ����ֵ���Ƿ�˳����SDU��
     *             0����˳���ԣ�
     *             1����˳���ԣ�
     *             2��Э��ֵ��
     *     <Maximum SDU size>: ����ֵ�����SDU���ȣ���λΪbyte����ȡֵ��ΧΪ0��1520��
     *             0��Э��ֵ��
     *             1��1520��1byte��1520byte��
     *     <SDU error ratio>: �ַ������ͣ�SDU�����ʡ�
     *             0E0��Э��ֵ��
     *             1E2��1*10-2��
     *             7E3��7*10-3��
     *             1E3��1*10-3��
     *             1E4��1*10-4��
     *             1E5��1*10-5��
     *             1E6��1*10-6��
     *             1E1��1*10-1��
     *     <Residual bit error ratio>: �ַ������ͣ�SDU�������������ʡ�
     *             0E0��Э��ֵ��
     *             5E2��5*10-2��
     *             1E2��1*10-2��
     *             5E3��5*10-3��
     *             4E3��4*10-3��
     *             1E3��1*10-3��
     *             1E4��1*10-4��
     *             1E5��1*10-5��
     *             1E6��1*10-6��
     *             6E8��6*10-8��
     *     <Delivery of erroneous SDUs>: ����ֵ�������SDU�Ƿ񱻷��͡�
     *             0�������ͣ�
     *             1�����ͣ�
     *             2������⣻
     *             3��Э��ֵ��
     *     <Transfer delay>: ����ֵ������ʱ�ӣ���λΪms����ȡֵ��ΧΪ0��4100��
     *             0��Э��ֵ��
     *             1��4100��1ms��4100ms��
     *     <Traffic handling priority>: ����ֵ�����ȼ���
     *             0��Э��ֵ��
     *             1�����ȼ�1��
     *             2�����ȼ�2��
     *             3�����ȼ�3��
     *     <PDP_type>: �ַ������ͣ���������Э�����͡�
     *             IP������Э�飨IPV4����
     *             IPV6��IPV6Э�顣
     *             IPV4V6��IPV4��IPV6��
     *     <Source Statistics Descriptor>: ����ֵ
     *             0��SDU������δ֪��
     *             1��SDU�����Ժ�����Դ��ء�
     *     <Signalling Indication>: ����ֵ
     *             0������Ż�PDP�����ġ�
     *             1�������Ż�PDP�����ġ�
     * [ʾ��]:
     *     �� ����PDP�����ĵ���С�ɽ���QoS
     *       AT+CGEQMIN=2,2,64,64,,,1,320,"1E4","1E5",1,,3,0,0
     *       OK
     *     �� ��ѯ����
     *       AT+CGEQMIN?
     *       +CGEQMIN: 2,2,64,64,,,1,320,"1E4","1E5",1,,3,0,0
     *       OK
     *     �� ��������
     *       AT+CGEQMIN=?
     *       +CGEQMIN: "IP",(0-4),(0-5760),(0-21600),(0-5760),(0-21600),(0-2),(0-1520),("0E0","1E2","7E3","1E3","1E4","1E5","1E6","1E1"),("0E0","5E2","1E2","5E3","4E3","1E3","1E4","1E5","1E6","6E8"),(0-3),(0-4100),(0-3),(0-1),(0-1)
     *       +CGEQMIN: "IPV6",(0-4),(0-11480),(0-42000),(0-11480),(0-42000),(0-2),(0-1520),("0E0","1E2","7E3","1E3","1E4","1E5","1E6","1E1"),("0E0","5E2","1E2","5E3","4E3","1E3","1E4","1E5","1E6","6E8"),(0-3),(0-4100),(0-3),(0-1),(0-1)
     *       +CGEQMIN: "IPV4V6",(0-4),(0-11480),(0-42000),(0-11480),(0-42000),(0-2),(0-1520), ("0E0","1E2","7E3","1E3","1E4","1E5","1E6","1E1"), ("0E0","5E2","1E2","5E3","4E3","1E3","1E4","1E5","1E6","6E8"),(0-3),(0-4100),(0-3),(0-1),(0-1)
     *       OK
     */
    { AT_CMD_CGEQMIN,
      AT_SetCgeqminPara, AT_SET_PARA_TIME, At_QryCgeqminPara, AT_QRY_PARA_TIME, At_TestCgeqreqPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGEQMIN", (VOS_UINT8 *)CGEQMIN_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����PDP�����Զ�Ӧ��
     * [˵��]: �򿪻�ر�PS����Զ�Ӧ�𣬸�Ӧ�����ڽ������緢���PDP��������
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGAUTO=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CGAUTO?
     *     [���]: <CR><LF>+CGAUTO: <n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGAUTO=?
     *     [���]: <CR><LF>+CGAUTO: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ���򿪻��߹ر�PS����Զ�Ӧ��
     *             0���رգ�
     *             1���򿪡�
     * [ʾ��]:
     *     �� ��������
     *       AT+CGAUTO=1
     *       OK
     *     �� ��������
     *       AT+CGAUTO=?
     *       +CGAUTO: (0,1)
     *       OK
     *     �� ��ѯ����
     *       AT+CGAUTO?
     *       +CGAUTO: 1
     *       OK
     */
    { AT_CMD_CGAUTO,
      AT_SetCgautoPara, AT_SET_PARA_TIME, AT_QryCgautoPara, AT_QRY_PARA_TIME, At_TestCgautoPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CGAUTO", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: �����Ƿ�����ʱ���ϱ�
     * [˵��]: �����ͨ��MM INFORMATION��GMM INFORMATION����ǰʱ����Ϣ֪ͨ�նˡ�
     *         ���������������ն��Ƿ��ϱ���Ӧ�������·���ʱ����Ϣ���ϱ��ĸ�ʽ��
     * [�﷨]:
     *     [����]: +CTZR=<value>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CTZR?
     *     [���]: <CR><LF>+CTZR: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CTZR=?
     *     [���]: <CR><LF>+CTZR: (list of supported <value>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: ����ֵ
     *             0�� ���ϱ�
     *             1�� ʹ��+ctzv�ϱ�
     *             2�� ʹ��+ctze�ϱ�
     * [ʾ��]:
     *     �� ���ô�����ʱ���ϱ����ϱ���ʽʹ��+ctzv
     *       AT+CTZR=1
     *       OK
     *     �� ��ѯ
     *       AT+CTZR?
     *       +CTZR: 1
     *       OK
     */
    { AT_CMD_CTZR,
      At_SetCtzrPara, AT_SET_PARA_TIME, At_QryCtzrPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CTZR", (VOS_UINT8 *)"(0-2)" },

#if (FEATURE_LTE == FEATURE_ON)

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ȡȱʡPDP�����Ķ�̬����
     * [˵��]: Execution�����ȡȱʡPDP����ض�̬�����Ĳ�����
     *         Read����ش��ڼ���״̬��ȱʡPDP�����Ĺ�����<p_cid>�б�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGCONTRDP=[<p_cid>]
     *     [���]: <CR><LF>+CGCONTRDP: <p_cid>,<bearer_id>,<apn>[,<local_addr and subnet_mask>[,<gw_addr>[,<DNS_prim_addr>[,<DNS_sec_addr>[,<P-CSCF_prim_addr>[,<P-CSCF_sec_addr>[,<IM_CN_Signalling_Flag>[,[,<IPv4_MTU>[,[,[,[,[,[,[,<PDU_session_id>,<QFI>[,<SSC_mode>[,<S-NSSAI>[,<Access_type>[,<RQ_timer>[,<Always-on_ind>]]]]]]]]]]]]]]]]]]]]]
     *             [<CR><LF>+CGCONTRDP: <p_cid>,<bearer_id>,<apn>[,<local_addr and subnet_mask>[,<gw_addr>[,<DNS_prim_addr>[,<DNS_sec_addr>[,<P-CSCF_prim_addr>[,<P-CSCF_sec_addr>[,<IM_CN_Signalling_Flag>[,[,<IPv4_MTU>[,[,[,[,[,[,[,<PDU_session_id>,<QFI>[,<SSC_mode>[,<S-NSSAI>[,<Access_type>[,<RQ_timer>[,<Always-on_ind>]]]]]]]]]]]]]]]]]]]]]
     *             [��]] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: +CGCONTRDP=?
     *     [���]: <CR><LF>+CGCONTRDP: (list of <p_cid>s associated with active contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <p_cid>: ���ֲ�������ʶDefault PDP��
     *     <bearer_id>: ���ֲ�����EPS����ҵ��ı�ʶ����UMTS/GPRS��NSAPI��ʶ��ȡֵ��Χ5~15��
     *     <APN>: �ַ���������ָ��GGSN���ⲿ�����������硣
     *     <local_addr_and_subnet_mask>: �ַ����������ն�IP��ַ���������룬��ָ�ʽ����ʽ���£�
     *             IPv4��"a1.a2.a3.a4.m1.m2.m3.m4"
     *             Ipv6��"a1.a2.a3.a4.a5.a6.a7.a8.a9.a10.a11.a12.a13.a14.a15.a16.m1.m2.m3.m4.m5.m6.m7.m8.m9.m10.m11.m12.m13. m14.m15.m16"
     *     <gw_addr>: �ַ����������ն˵����ص�ַ��
     *     <DNS_prim_addr>: �ַ�����������DNS������IP��ַ��
     *     <DNS_sec_addr>: �ַ�����������DNS������IP��ַ��
     *     <P_CSCF_prim_addr>: �ַ�����������CSCF��������ַ��
     *     <P_CSCF_sec_addr>: �ַ�����������CSCF��������ַ��
     *     <IM_CN_Signalling_Flag>: ����ֵ��ָʾ��PDP�Ƿ������ IM CN
     *     <IPv4_MTU>: ����ֵ��IPv4ջ��MTUֵ
     *     <PDU_session_id>: ����ֵ��PDU session ID
     *     <QFI>: ����ֵ��QoS flow ID
     *     <SSC_mode>: ����ֵ��SSC mode
     *     <S-NSSAI>: �ַ������ͣ���Ƭ��Ϣ
     *             "sst" if only slice/service type (SST) is present
     *             "sst;mapped_sst" if SST and mapped configured SST are present
     *             "sst.sd" if SST and slice differentiator (SD) are present
     *             "sst.sd;mapped_sst" if SST, SD and mapped configured SST are present
     *             "sst.sd;mapped_sst.mapped_sd" if SST, SD, mapped configured SST and mapped configured SD are present
     *             ע�⣺�ַ����е�����Ϊ16���ƣ��ַ�ȡֵ��Χ��0-9��A-F��a-f��
     *     <Access_type>: ����ֵ����������
     *     <RQ_timer>: ����ֵ������QoS��ʱ������λs
     *     <Always-on_ind>: ����ֵ��Always-onָʾ��ǣ�
     *             0��ָʾ PDU session ���� always-on��
     *             1��ָʾ PDU session �� always-on��
     * [ʾ��]:
     *       ��ѯ�Ѽ����CID=1��ȱʡPDP��Ϣ
     *     �� ��֧��5GSʱ
     *       AT+CGCONTRDP=1
     *       +CGCONTRDP: 1,5,"CMCC","192.168.0.23","255.255.255.0"
     *       OK
     *       ֧��5GSʱ
     *          AT+CGCONTRDP=1
     *          +CGCONTRDP: 1,5,"CMCC","192.168.0.23","255.255.255.0",,1500,,,,,,,5,1,0,"1.123456;2F.654321",0,1000,0
     *          OK
     *     �� ���ش��ڼ���״̬��ȱʡPDP������<p_cid>�б������Ѽ���CID1��9��
     *       AT+CGCONTRDP=?
     *       +CGCONTRDP: (1,9)
     *       OK
     */
    { AT_CMD_CGCONTRDP,
      atSetCgcontrdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgcontrdp, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGCONTRDP", (VOS_UINT8 *)"(1-31)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ȡSecondary PDP�����Ķ�̬����
     * [˵��]: Execution�����ȡSecondary PDP��صĶ�̬�����Ĳ�����
     *         Read����ش��ڼ���״̬��Secondary PDP������<cid>�б�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGSCONTRDP[=<cid>]
     *     [���]: <CR><LF>+CGSCONTRDP: <cid>, <p_cid>, <bearer_id>[,[,[,<PDU_session_id>,<QFI>]]]]
     *             [<CR><LF>+CGSCONTRDP: <cid>, <p_cid>,  <bearer_id>[,[,[,<PDU_session_id>,<QFI>]]]]
     *             [��]] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: +CGSCONTRDP=?
     *     [���]: <CR><LF>+CGSCONTRDP: (list of <cid>s associated with active contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ���ֲ�������ʶSecondary PDP��CID��
     *     <p_cid>: ���ֲ�������ʶDefault PDP��CID��
     *     <bearer_id>: ���ֲ�����EPS����ҵ���EPSBID��ʶ����UMTS/GPRS��NSAPI��ʶ��ȡֵ��Χ5~15��
     *     <PDU_session_id>: ����ֵ��PDU session ID
     *     <QFI>: ����ֵ��QoS flow ID
     * [ʾ��]:
     *       ��ѯ�����CID=3��Secondary PDP��Ϣ
     *     �� ��֧��5GSʱ
     *       AT+CGSCONTRDP=3
     *       +CGSCONTRDP: 3,1,5
     *       OK
     *     �� ֧��5GSʱ
     *       AT+CGSCONTRDP=3
     *       +CGSCONTRDP: 3,1,5,,,5,1
     *       OK
     *     �� ���ش��ڼ���״̬��Secondary PDP������<cid>�б������Ѽ���CID2��21��
     *       AT+CGSCONTRDP=?
     *       +CGSCONTRDP: (2,21)
     *       OK
     */
    { AT_CMD_CGSCONTRDP,
      atSetCgscontrdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgscontrdp, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGSCONTRDP", (VOS_UINT8 *)"(1-31)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ȡTFT����
     * [˵��]: Execution�����ȡ<cid>��ʶ��TFT��ز�����
     *         Read����ش��ڼ���״̬��EPS���ع�����<cid>�б�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGTFTRDP[=<cid>]
     *     [���]: <CR><LF>+CGTFTRDP: <cid>, <packet filter identifier>, <evaluation precedence index>, <source address and subnet mask>, <protocol number (ipv4) / next header (ipv6)>, <destination port range>, <source port range>, <ipsec security parameter index (spi)>, <type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>, <flow label (ipv6)>, <direction>, <NW packet filter Identifier>,<local address and subnetmask>,<QRI>
     *             [<CR><LF>+CGTFTRDP: <cid>, <packet filter identifier>, <evaluation precedence index>, <source address and subnet mask>, <protocol number (ipv4) / next header (ipv6)>, <destination port range>, <source port range>, <ipsec security parameter index (spi)>, <type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>, <flow label (ipv6)>, <direction>,<NW packet filter Identifier>,<local address and subnetmask>,<QRI>
     *             [��]] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: +CGTFTRDP=?
     *     [���]: <CR><LF>+CGTFTRDP: (list of <cid>s associated with active contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <packet filter identifier>: ���ֲ�����������ID��ȡֵ��Χ1~16��
     *     <evaluation precedence index>: Packet filter�����ȼ������ж��filterƥ�����ݰ��ɹ�ʱ��ȡ���ȼ��ߵ�filter�����ֲ�����ȡֵ��Χ0-255��
     *     <source address and subnet mask>: �ַ������ͣ�Դ��ַ���������룬��ָ�ʽ��
     *             IPv4��"a1.a2.a3.a4.m1.m2.m3.m4"
     *             IPv6��"a1.a2.a3.a4.a5.a6.a7.a8.
     *             a9.a10.a11.a12.a13.a14.a15.a16.
     *             m1.m2.m3.m4.m5.m6.m7.m8.
     *             m9.m10.m11.m12.m13.m14.m15.m16"
     *     <protocol number (ipv4) / next header (ipv6)>: IPͷ��ָʾ���ϲ�Э��ţ���TCP��UDP�ȡ����ֲ�����ȡֵ��Χ0~255��
     *     <destination port range>: �ַ������ͣ�Ŀ�Ķ˿ڷ�Χ��ʮ�����Ƶ����ʽ�������ҵ����ֲ��ܳ���65535��
     *             ���磺"f.t"��f��ʾ�˿����ޣ�t��ʾ�˿����ޡ�
     *     <source port range>: �ַ������ͣ�Դ�˿ڷ�Χ��ʮ�����Ƶ����ʽ�������ҵ����ֲ��ܳ���65535��
     *             ���磺"f.t"��f��ʾ�˿����ޣ�t��ʾ�˿����ޡ�
     *     <ipsec security parameter index (spi)>: IPV6�а�ȫ��չͷ����AH��ESP���Ĳ��������ֲ�����ʮ��������ʽ��ȡֵ��Χ0x00000000~0xFFFFFFFF��
     *     <type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>: ��������/ҵ�����ͣ��ַ������ͣ������ʽ�������ҵ����ֲ��ܳ���255��
     *             ���磺"f.m"��f��ʾ������Сֵ��m��ʾ�������ֵ��
     *     <flow label (ipv6)>: ����ǩ��IPV6����ͷ�еĲ�������ʶԴ��ַ��Ŀ�ĵ�ַ��ͬ��һЩ�������ֲ�����ʮ��������ʽ��ȡֵ��Χ0x00000~0xFFFFF������IPv6��Ч��
     *     <direction>: ���ֲ�����ָ���������ķ���
     *             0��Release 7֮ǰ��TFT���������μ�3GPP TS 24.008����
     *             1�����з���
     *             2�����з���
     *             3��˫��ȱʡֵ����
     *     <NW packet filter Identifier>: ����������ʶFILTER��ID�����ֲ�����ȡֵ��Χ1~16��
     *     <local address and subnet mask>: IP��ַ�ַ������ͣ�����IP��ַ���������룬��ʽΪIPv4:��a1.a2.a3.a4.m1.m2.m3.m4��
     *             IPv6:"a1.a2.a3.a4.a5.a6.a7.a8.a9.a10.a11.a12.a13.a14.a15.a16.m1.m2.m3.m4.m5.m6.m7.m8.m9.m10.m11.m12.m13.m14.m15.m16"��
     *             ע�⣺ֻ��R11���ϰ汾֧�ִ˲���
     *     <QRI>: ����ֵ��Qos rule��identity��Э��֧��0-255.
     *             ע�⣺ֻ��R15���ϰ汾֧�ִ˲���
     * [ʾ��]:
     *     �� ��ȡCID=3��Ӧ���ص�TFT��ز���
     *       AT+CGTFTRDP=3
     *       +CGTFTRDP: 3,0,0,"192.168.0.2.255.255.255.0",0,"0.65535","0.65535",0,"0.0",0,0
     *       OK
     *     �� �����ش��ڼ���״̬��EPS���ع�����<cid>�б�
     *       AT+CGTFTRDP=?
     *       +CGTFTRDP: (1,2,21)
     *       OK
     */
    { AT_CMD_CGTFTRDP,
      atSetCgtftrdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgtftrdp, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGTFTRDP", (VOS_UINT8 *)"(1-31)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����EPS QoS����
     * [˵��]: ���������EPS Traffic Flow��Ӧ��QoS������
     *         ��ѯ�����ȡ�Ѿ������QoS������
     *         ���������ϵͳ֧�ֵĲ�����Χ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGEQOS=[<cid>[,<QCI>[,<DL_GBR>,<UL_GBR>[,<DL_MBR>,<UL_MBR]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err> <CR><LF>
     *     [����]: +CGEQOS?
     *     [���]: <CR><LF>+CGEQOS: <cid>, <QCI>, [<DL_GBR>,<UL_GBR>], [<DL_MBR>,<UL_MBR>]
     *             [<CR>>LF>+CGEQOS: <cid>, <QCI>, [<DL_GBR>,<UL_GBR>], [<DL_MBR>,<UL_MBR>]
     *             [��]] <CR><LF><CR><LF>OK<CR><LF>
     *     [����]: +CGEQOS=?
     *     [���]: <CR><LF>+CGEQOS: (list of supported <cid>s) ,(list of supported <QCI>s) ,(list of supported <DL_GBR>s), (list of supported <UL_GBR>s), (list of supported <DL_MBR>s) ,(list of supported <UL_MBR>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ���ֲ�������ʶEPS Traffic Flow��ȡֵ��Χ1~31��
     *     <QCI>: ���������ȼ���ÿ����Ӧһ��������綪���ʣ�ʱ�ӵȡ�
     *             0��������ѡ��QCI����0��Ԥ��������ģ������������ã���
     *             1~4, 71~76, 82~85��GBR Traffic Flowsȡֵ��Χ��
     *             5~9, 79��non-GBR Traffic Flowsȡֵ��Χ��
     *             128~254����Ӫ��ר��QCIȡֵ��Χ��
     *             �ο�3GPP TS 23.203��
     *     <DL_GBR>: ���ֲ��������з���GBR�ο�ֵ��ĿǰUE�˲�֧������GBR���ʣ�����λΪkbit/s���˲�����GBR QCI��Ч����non-GBR QCI���˲���ʡ�ԡ��ο�3GPP TS 24.301��
     *     <UL_GBR>: ���ֲ��������з���GBR�ο�ֵ��ĿǰUE�˲�֧������GBR���ʣ�����λΪkbit/s���˲�����GBR QCI��Ч����non-GBR QCI���˲���ʡ�ԡ��ο�3GPP TS 24.301��
     *     <DL_MBR>: ���ֲ��������з���MBR�ο�ֵ��ĿǰUE�˲�֧������MBR���ʣ�����λΪkbit/s���˲�����GBR QCI��Ч����non-GBR QCI���˲���ʡ�ԡ��ο�3GPP TS 24.301��
     *     <UL_MBR>: ���ֲ��������з���MBR�ο�ֵ��ĿǰUE�˲�֧������MBR���ʣ�����λΪkbit/s���˲�����GBR QCI��Ч����non-GBR QCI���˲���ʡ�ԡ��ο�3GPP TS 24.301��
     * [ʾ��]:
     *     �� ���������EPS Traffic Flow��Ӧ��QoS������
     *       AT+CGEQOS=3,5
     *       OK
     *     �� ��ѯ�����ȡ�Ѿ������QoS������
     *       AT+CGEQOS?
     *       +CGEQOS: 3,5,0,0,0,0
     *       OK
     *     �� ���������ϵͳ֧�ֵĲ�����Χ
     *       AT+CGEQOS=?
     *       +CGEQOS: (1-31),(0-9,71-76,79,128-254),(0-16777216),(0-16777216),(0-16777216),(0-16777216)
     *       OK
     */
    { AT_CMD_CGEQOS,
      atSetCgeqosPara, AT_SET_PARA_TIME, atQryCgeqosPara, AT_QRY_PARA_TIME, At_TestCgeqos, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGEQOS", (VOS_UINT8 *)"(1-31),(0-9,71-76,79,128-254),(0-16777216),(0-16777216),(0-16777216),(0-16777216)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ȡEPS QoS����
     * [˵��]: ���������ȡ<cid>��ʶ��EPS PDN���Ӷ�Ӧ��QoS������
     *         ��������ش��ڼ���״̬��EPS���ض�Ӧ��<cid>�б�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGEQOSRDP[=<cid>]
     *     [���]: <CR><LF>+CGEQOSRDP: <cid>, <QCI>, [<DL_GBR>,<UL_GBR>], [<DL_MBR>,<UL_MBR>]
     *             [<CR>>LF>+CGEQOSRDP: <cid>, <QCI>, [<DL_GBR>,<UL_GBR>], [<DL_MBR>,<UL_MBR>]
     *             [��]] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGEQOSRDP=?
     *     [���]: <CR><LF>+CGEQOSRDP: (list of <cid>s associated with active contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ���ֲ�������ʶEPS Traffic Flow��
     *     <QCI>: 0��������ѡ��QCI��
     *             1~4��GBR Traffic Flowsȡֵ��Χ��
     *             5~9��non-GBR Traffic Flowsȡֵ��Χ��
     *             �ο�3GPP TS 23.203��
     *     <DL_GBR>: ���ֲ��������з���GBR�ο�ֵ��ĿǰUE�˲�֧������GBR���ʣ�����λΪkbit/s���˲�����GBR QCI��Ч����non-GBR QCI���˲���ʡ�ԡ��ο�3GPP TS 24.301��
     *     <UL_GBR>: ���ֲ��������з���GBR�ο�ֵ��ĿǰUE�˲�֧������GBR���ʣ�����λΪkbit/s���˲�����GBR QCI��Ч����non-GBR QCI���˲���ʡ�ԡ��ο�3GPP TS 24.301��
     *     <DL_MBR>: ���ֲ��������з���MBR�ο�ֵ��ĿǰUE�˲�֧������MBR���ʣ�����λΪkbit/s���˲�����GBR QCI��Ч����non-GBR QCI���˲���ʡ�ԡ��ο�3GPP TS 24.301��
     *     <UL_MBR>: ���ֲ��������з���MBR�ο�ֵ��ĿǰUE�˲�֧������MBR���ʣ�����λΪkbit/s���˲�����GBR QCI��Ч����non-GBR QCI���˲���ʡ�ԡ��ο�3GPP TS 24.301��
     * [ʾ��]:
     *     �� ���������ȡCID=3��EPS PDN���Ӷ�Ӧ��QoS������
     *       AT+CGEQOSRDP=3
     *       +CGEQOSRDP: 3,5
     *       OK
     *     �� ��������ش��ڼ���״̬��EPS���ض�Ӧ��<cid>�б�
     *       AT+CGEQOSRDP=?
     *       +CGEQOSRDP: (1,9)
     *       OK
     */
    { AT_CMD_CGEQOSRDP,
      atSetCgeqosrdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgeqosrdp, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CGEQOSRDP", (VOS_UINT8 *)"(1-31)" },

#endif

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ƶ�����λ����
     * [˵��]: �����������ƶ��˷����ȡ����λ����MO-LR����ͬʱ�ڷ���λ����ʱ���ö�λ�ķ�ʽ��λ�ñ���ĸ�ʽ����Ϣ��
     * [�﷨]:
     *     [����]: +CMOLR=[<enable>[,<method>[,<hor-acc-set>[,<hor-acc>[,<ver-req>[,<ver-acc-set>[,<ver-acc>[,<vel-req>[,<rep-mode>[,<timeout>[,<interval>[,<shape-rep>[,<plane>[,<NMEA-rep>[,<third-party-address>]]]]]]]]]]]]]]]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CMOLR?
     *     [���]: <CR><LF>+CMOLR: <enable>,<method>, <hor-acc-set>,[<hor-acc>],<ver-req>, [<ver-acc-set>],[<ver-acc>],<vel-req>, <rep-mode>,<timeout>,[<interval>], <shape-rep>,[<plane>],[<NMEA-rep>], [<third-party-address>]<CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CMOLR=?
     *     [���]: <CR><LF>+CMOLR: (list of supported <enable>s),(list of supported <method>s),(list of supported <hor-acc-set>s),(list of supported <hor-acc>s),(list of supported <ver-req>s),(list of supported <ver-acc-set>s),(list of supported <ver-acc>s),(list of supported <vel-req>s),(list of supported <rep-mode>s),(list of supported <timeout>s),(list of supported <interval>s),(list of supported <shape-rep>s),(list of supported <plane>s),(list of supported <NMEA-rep>s),(list of supported <third-party-address>s)<CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ��������رն�λ��������λ���ϱ���ͬһʱ��ֻ����һ��<method>���Ա�������Ĭ��ֵΪ0��
     *             0���ر��ϱ��Ͷ�λ��������������ԣ�
     *             1������NMEA�ַ����������ϱ�����ʽΪ+CMOLRN: <NMEA-string>������ʱû������ʱ�������ϱ�+CMOLRE��
     *             2������GAD��״�������ϱ�����ʽΪ+CMOLRG: <location_parameters>������ʱû������ʱ�������ϱ�+CMOLRE��
     *             3��ͬʱ����NMEA�ַ�����GAD��״�������ϱ�����ʽΪ+CMOLRN: <NMEA-string>��+CMOLRG: <location_parameters>������ʱû������ʱ�������ϱ�+CMOLRE��
     *     <method>: ����ֵ��MO-LR�ķ�����Ĭ��ֵΪ0��
     *             0������GPS����������GPS���޸������ݣ�
     *             1������GPS��
     *             2������GANSS��
     *             3������GPS��GANSS��
     *             4����������λ�������������λ��������
     *             5��ת�����������˷�������ע����<shape-rep>��<NMEA-rep>���·���ֵ�������ԣ�����������ַ�ɲ���<third-party-address>�ṩ��
     *             6���ӵ������ջأ���������ַ�ɲ���<third-party-address>�ṩ��
     *             ע����ָ���ϱ��������������ʾ����ʱû������ʱ�������ϱ�+CMOLRE��
     *     <hor-acc-set>: ����ֵ��ˮƽ�������ñ�־��Ĭ��ֵΪ0��
     *             0����Ҫ��ˮƽ���ȣ�
     *             1��ˮƽ�����ɲ���<hor-acc>ָ����
     *     <hor-acc>: ����ֵ�������ˮƽ���Ȳ�ȷ��ָ����ȡֵ��ΧΪ0~127��Ĭ��ֵΪ60��
     *     <ver-req>: ����ֵ����ֱ��������Ĭ��ֵΪ0��
     *             0��������ֱ���꣨�߶ȣ��������ά��λ������<ver-acc-set>��<ver-acc>�����ã�
     *             1������ֱ���꣨�߶ȣ�����Ҫ��ά��λ��
     *     <ver-acc-set>: ����ֵ����ֱ�������ñ�־��Ĭ��ֵΪ0��
     *             0�������ô�ֱ���ȣ�
     *             1����ֱ�����ɲ���<ver-acc>ָ����
     *     <ver-acc>: ����ֵ������Ĵ�ֱ���Ȳ�ȷ��ָ����ȡֵ��ΧΪ0~127��Ĭ��ֵΪ60��
     *     <vel-req>: ����ֵ��������ٶ����ͣ�Ĭ��ֵΪ0��
     *             0������Ҫ�ٶȣ�
     *             1������ˮƽ�ٶȣ�
     *             2������ˮƽ����ֱ�ٶȣ�
     *             3����ȷ��Ҫ���ˮƽ�ٶȣ�
     *             4����ȷ��Ҫ���ˮƽ�ٶȼ���ֱ�ٶȡ�
     *     <rep-mode>: ����ֵ���ϱ�ģʽ��Ĭ��ֵΪ0��
     *             0���������棬MO-LR��Ӧ�ĳ�ʱʱ����<timeout>ָ����
     *             1�������ϱ���ÿ��MO-LR��Ӧ�ĳ�ʱʱ����<timeout>ָ����ÿ��MO-LR�ļ����<interval>ָ����
     *     <timeout>: ����ֵ��ָʾMS�ȴ�MO-LR��Ӧ��ʱ�䣬ȡֵ��ΧΪ1~65535����λ�룬Ĭ��ֵΪ180��
     *     <interval>: ����ֵ���˲��������������ϱ�����������MO-LR��ļ����ȡֵ��ΧΪ1~65535����λ�룬ȡֵҪ���ڵ���<timeout>��Ĭ��ֵΪ180��
     *     <shape-rep>: ����ֵ���˲����������ϱ������<location_parameters>�пɽ��ܵĴ���ĳ��GAD��״�ļ��ϣ�ȡֵ��ΧΪ1-127��Ĭ��ֵΪ1��
     *             1����Բ�㣻
     *             2����ȷ��Բ����Բ�㣻
     *             4����ȷ����Բ����Բ�㣻
     *             8������Σ�
     *             16������ֱ�������Բ�㣻
     *             32������ֱ����Ĳ�ȷ����Բ����Բ�㣻
     *             64����Բ����
     *             ע��ÿ�������ϱ�������н���ʾһ��GAD��״��
     *     <plane>: ����ֵ��ƽ�����ͣ�Ĭ��ֵΪ0��
     *             0�������棻
     *             1����ȫ�û��棨SUPL����
     *     <NMEA-rep>: �ַ������ͣ�֧��NMEA�ַ�������ʽΪ�Զ��ŷָ����ַ���������˲���ʡ�Ի�Ϊ���ַ�������û�����ƣ����е�NMEA�ַ�����֧�֡�Ĭ��ֵΪ��$GPGGA,$GPRMC,$GPGLL����
     *     <third-party-address>: �ַ������ͣ��˲����������ϱ�����������ָ���˵������ĵ�ַ������<method>ֵΪ5��6ʱ��Ч��
     * [ʾ��]:
     *     �� �����ƶ���λ����
     *       AT+CMOLR=3,1,1,20,0,,,0,1,60,100,9,0,"$GPRMC"
     *       OK
     *     �� ��ѯ���ж�λ�������
     *       AT+CMOLR?
     *       +CMOLR: 0,0,0,,0,,,0,0,180,,1,,,
     *       OK
     *     �� ִ�в�������
     *       AT+CMOLR=?
     *       +CMOLR: (0-3),(0-6),(0,1),(0-127),(0,1),(0,1),(0-127),(0-4),(0,1), (1-65535),(1-65535),(1-127),(0,1),"$GPGGA,$GPRMC,$GPGLL","<third-party-addr>"
     *       OK
     */
    { AT_CMD_CMOLR,
      AT_SetCmolrPara, AT_SET_PARA_TIME, AT_QryCmolrPara, AT_QRY_PARA_TIME, AT_TestCmolrPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CMOLR", (VOS_UINT8 *)"(0-3),(0-6),(0,1),(0-127),(0,1),(0,1),(0-127),(0-4),(0,1),(1-65535),(1-65535),(1-127),(0,1),(nmea),(thirdparty)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �������ж�λ��������ϱ���ʽ
     * [˵��]: ���������ڿ����ƶ��˷���Ķ�λ����MO-LR�����ܴ���ָʾ���ϱ���ʽ��
     * [�﷨]:
     *     [����]: +CMOLRE=[<n>]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CMOLRE?
     *     [���]: <CR><LF>+CMOLRE: <n><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CMOLRE=?
     *     [���]: <CR><LF>+CMOLRE: (list of supported <n>s)<CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��������ر��ַ������������ϱ���ʽ��Ĭ��ֵΪ0��
     *             0���رգ�
     *             1��������
     * [ʾ��]:
     *     �� �������ж�λ�������Ϊ�ַ�����ʽ
     *       AT+CMOLRE=1
     *       OK
     *     �� ��ѯ���ж�λ�����ϱ���ʽ
     *       AT+CMOLRE?
     *       +CMOLRE: 1
     *       OK
     *     �� ִ�в�������
     *       AT+CMOLRE=?
     *       +CMOLRE: (0,1)
     *       OK
     */
    { AT_CMD_CMOLRE,
      AT_SetCmolrePara, AT_NOT_SET_TIME, AT_QryCmolrePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CMOLRE", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �������ж�λ����ָʾ��ʽ
     * [˵��]: ���������ڿ������緢��Ķ�λ����MT-LR��֪ͨ�ķ�ʽ��
     * [�﷨]:
     *     [����]: +CMTLR=[<subscribe>]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CMTLR?
     *     [���]: <CR><LF>+CMTLR: <subscribe><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CMTLR=?
     *     [���]: <CR><LF>+CMTLR: (list of supported <subscribe>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <subscribe>: ����ֵ��������ر����ж�λ������ϱ���Ĭ��ֵΪ1��
     *             0���ر��ϱ��Ͷ�λ��
     *             1��ͨ���������ϱ�MT-LR֪ͨ��
     *             2��ͨ��SUPL�ϱ�MT-LR֪ͨ��
     *             3��ͨ���������SUPL�ϱ�MT-LR֪ͨ��
     * [ʾ��]:
     *     �� �������ж�λ����ͨ���������ϱ�
     *       AT+CMTLR=1
     *       OK
     *     �� ��ѯ���ж�λ�����ϱ��ķ�ʽ��ͨ��SUPL�ϱ���
     *       AT+CMTLR?
     *       +CMTLR: 2
     *       OK
     *     �� ִ�в�������
     *       AT+CMTLR=?
     *       +CMTLR: (0-3)
     *       OK
     */
    { AT_CMD_CMTLR,
      AT_SetCmtlrPara, AT_SET_PARA_TIME, AT_QryCmtlrPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CMTLR", (VOS_UINT8 *)"(0-3)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �������綨λ����Ӧ��
     * [˵��]: ���������ڽ��ջ�ܾ����緢��Ķ�λ����MT-LR����
     * [�﷨]:
     *     [����]: +CMTLRA=<allow>,<handle_id>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CMTLRA?
     *     [���]: <CR><LF>+CMTLRA: <allow>,<handle_id> <CR><LF><CR><LF>OK<CR><LF>
     *     [����]: +CMTLRA=?
     *     [���]: <CR><LF>+CMTLRA: (list of supported <allow> values)<CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <allow>: ����ֵ����������������λ�÷�������
     *             0������λ�÷��֣�
     *             1��������λ�÷��֡�
     *     <handle_id>: ����ֵ��ÿ��MT-LR��ID�������ֶ�������е�ָ������ȡֵ��ΧΪ0~255����+CMTLR���������
     * [ʾ��]:
     *     �� ���������λ�÷�������
     *       AT+CMTLRA=0,2
     *       OK
     *     �� ��ѯ���ж�λ�������Ӧ����
     *       AT+CMTLRA?
     *       +CMTLRA: 0,2
     *       OK
     *     �� ִ�в�������
     *       AT+CMTLRA=?
     *       +CMTLRA: (0,1)
     *       OK
     */
    { AT_CMD_CMTLRA,
      AT_SetCmtlraPara, AT_SET_PARA_TIME, AT_QryCmtlraPara, AT_QRY_PARA_TIME, AT_TestCmtlraPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CMTLRA", (VOS_UINT8 *)"(0,1),(0-255)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��λ����
     * [˵��]: ����������AP-Modem��̬��GPSоƬ��XML��ʽ�Ķ�λ��Ϣ�����͵�Modem�ࡣ
     *         ע����AT������󳤶����ƣ�����һ��������XML��ʽ���ݷֶ���·���XML��ʽ�ĸ��ڵ�Ϊ<pos>��
     *         ����ʹ�ó�����AP��GPSоƬ�·���
     *         ����ʹ�����ƣ�ֻ�޶���AP-Modem��̬ʹ�á�
     *         ����ʹ��ͨ����ֻ��ͨ����AP�Խӵ�ATͨ����MUXͨ���·���
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CPOS<CR>
     *             textisentered<ctrl-Z/ESC>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPOS=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <CR>: MT�������н�����<CR>����4���ַ�<CR><LF><greater_than><space>��ASCII��ֵ13��10��62��32����
     *     <ctrl-Z>: �ַ���0x1A������ʾ����������������Ͷ�λ���
     *     <ESC>: �ַ���0x1B������ʾȡ�����η��Ͷ�λ���������
     * [ʾ��]:
     *     �� GPS�·�XML����
     *       AT+CPOS
     *       ><?xml version="1.0" encoding="UTF-8"?>
     *       ><pos>
     *       ><assist_data>
     *           ><GPS_assist>
     *               ><ref_time>
     *                   ><GPS_time>
     *                       ><GPS_TOW_msec>518935680</GPS_TOW_msec>
     *                       ><GPS_week>282</GPS_week>
     *                   ></GPS_time>
     *               ></ref_time>
     *           ></GPS_assist>
     *       ></assist_data>
     *       ></pos><ctrl-Z>
     *       OK
     *     �� ��������
     *       AT+CPOS=?
     *       OK
     */
    { AT_CMD_CPOS,
      AT_SetCposPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CPOS", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �����Ƿ������ϱ���λ��Ϣ
     * [˵��]: ����������AP-Modem��̬�£������Ƿ������ϱ����ඨλ��Ϣ��AP�࣬ʹGPSоƬ��ȡ����Ĳ���ָʾ���ϵ翪��Ĭ��Ϊ�����������ϱ���
     *         ����ʹ�ó�����AP��GPSоƬ�·���
     *         ����ʹ�����ƣ�ֻ�޶���AP-Modem��̬ʹ�á�
     *         ����ʹ��ͨ����ֻ��ͨ����AP�Խӵ�ATͨ����MUXͨ���·���
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CPOSR=[<mode>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CPOSR?
     *     [���]: <CR><LF>+CPOSR: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CPOSR=?
     *     [���]: <CR><LF>+CPOSR: (list of supported <mode>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ���Ƿ����ö�λ��Ϣ�������ϱ���ȡֵ0��1��
     *             0�������������ϱ���
     *             1�����������ϱ���
     * [ʾ��]:
     *     �� ���ö�λ��������ϱ�
     *       AT+CPOSR=1
     *       OK
     *     �� ��ѯ����
     *       AT+CPOSR?
     *       +CPOSR: 1
     *       OK
     *     �� ��������
     *       AT+CPOSR=?
     *       +CPOSR: (0,1)
     *       OK
     */
    { AT_CMD_CPOSR,
      AT_SetCposrPara, AT_NOT_SET_TIME, AT_QryCposrPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CPOSR", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ���þ�������
     * [˵��]: �������о������ơ�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CMUT=<n>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CMUT?
     *     [���]: <CR><LF>+CMUT: <n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CMUT=?
     *     [���]: <CR><LF>+CMUT: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��
     *             0�����������
     *             1�����þ�����
     * [ʾ��]:
     *     �� ��������
     *       AT+CMUT=0
     *       OK
     *     �� ��ѯ����
     *       AT+CMUT?
     *       ^CMUT: 1
     *       OK
     *     �� ��������
     *       AT+CMUT=?
     *       +CMUT: (0-1)
     *       OK
     */
    { AT_CMD_CMUT,
      AT_SetCmutPara, AT_SET_VC_PARA_TIME, AT_QryCmutPara, AT_QRY_VC_PARA_TIME, AT_TestCmutPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CMUT", (VOS_UINT8 *)"(0,1)" },
    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ȡ��Ӫ������
     * [˵��]: ��ȡMT�����������Ӫ��������Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +COPN
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>+COPN: <numeric1>,<alpha1>[<CR><LF>+COPN: <numeric2>,<alpha2>[��]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: +COPN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <numericn>: �ַ������ͣ����ָ�ʽ����Ӫ����Ϣ
     *     <alphan>: �ַ������ͣ����ַ�����ʽ����Ӫ����Ϣ
     * [ʾ��]:
     *     �� ��ѯ��Ӫ����Ϣ
     *       AT+COPN
     *       +COPN: "00101","Test PLMN 1-1"
     *       ��
     *       +COPN: "00011","INMARSAT"
     *       OK
     *     �� ����COPN
     *       AT+COPN=?
     *       OK
     */
    { AT_CMD_COPN,
      AT_SetCopnPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+COPN", VOS_NULL_PTR },

    { AT_CMD_RELEASERRC,
      At_SetReleaseRrc, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"+CNMPSD", VOS_NULL_PTR },

#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ѯIMS��ע��״̬
     * [˵��]: ��ѯ��ǰIMS������ע��״̬���Լ�����״̬�ϱ���ʽ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CIREG=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CIREG?
     *     [���]: <CR><LF>+CIREG: <n>,<reg_info>[,<ext_info>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CIREG=?
     *     [���]: <CR><LF>+CIREG: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ�������ϱ���ʽ��Ĭ��ֵΪ0��
     *             0����ֹ+CIREGU�������ϱ���
     *             1��ʹ��+CIREGU: <reg_info>�������ϱ���
     *             2��ʹ��+CIREGU: <reg_info>[,<ext_info>]�������ϱ���
     *     <reg_info>: ����ֵ��ע��״̬��
     *             0��û��ע�᣻
     *             1��ע����IMS���硣
     *     <ext_info>: ����ֵ��IMS������ֵ��ȡֵ��Χ[1,0xFFFFFFFF]��ÿ��bitλ��ʾ��ͬ��������
     *             1:  RTP-based transfer of voice
     *             2:  RTP-based transfer of text
     *             4:  SMS using IMS functionality
     *             8�� RTP-based transfer of video
     *             ���磺=5��ʱ�򣬱�ʾ֧��RTP-based transfer of voice��SMS using IMS functionality
     * [ʾ��]:
     *     �� ����IMS��ע��״̬�ϱ�
     *       AT+CIREG=1
     *       OK
     *     �� <n>=1ʱ��ѯע��״̬
     *       AT+CIREG?
     *       +CIREG: 1,1
     *       OK
     *     �� <n>=2ʱ��ѯע��״̬
     *       AT+CIREG?
     *       +CIREG: 2,1,5
     *       OK
     *     �� ����CIREG
     *       AT+CIREG=?
     *       +CIREG: (0-2)
     *       OK
     */
    { AT_CMD_CIREG,
      AT_SetCiregPara, AT_SET_PARA_TIME, AT_QryCiregPara, AT_QRY_PARA_TIME, AT_TestCiregPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CIREG", (VOS_UINT8 *)"(0-2)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ѯSRVCC״̬
     * [˵��]: ��ѯSRVCC״̬���Լ�����״̬�ϱ���ʽ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CIREP=[<reporting>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CIREP?
     *     [���]: <CR><LF>+CIREP: < reporting>,< nwimsvops><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CIREP=?
     *     [���]: <CR><LF>+CIREP: (list of supported <reporting>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <reporting>: ����ֵ�������ϱ���ʽ��Ĭ��ֵΪ0��
     *             0����ֹ+CIREPH��+CIREPI�������ϱ���
     *             1��ʹ��+CIREPH��+CIREPI�������ϱ���
     *     <nwimsvops>: ����ֵ��ע��״̬��
     *             0�����粻֧��IMSVOPS��
     *             1������֧��IMSVOPS��
     * [ʾ��]:
     *     �� ����SRVCC״̬�����ϱ�
     *       AT+CIREP=1
     *       OK
     *     �� ��ѯ�����Ƿ�֧��IMSVOPS
     *       AT+CIREP?
     *       +CIREP: 1,0
     *       OK
     *     �� ����CIREP
     *       AT+CIREP=?
     *       +CIREP: (0,1)
     *       OK
     */
    { AT_CMD_CIREP,
      AT_SetCirepPara, AT_SET_PARA_TIME, AT_QryCirepPara, AT_QRY_PARA_TIME, AT_TestCirepPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CIREP", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��������������ѡģʽ
     * [˵��]: The set command is used to set the MT to operate according to the specified voice domain preference for E-UTRAN
     * [�﷨]:
     *     [����]: +CEVDP=[<setting>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             ���IMS����ҵ���ܹرջ���IMS��֧�ֵ�����£��·�AT+CEVDP=4����Ϊ����ģʽPS_ONLYʱ����ʧ��
     *             ��IMS����ҵ�����ʱ����������������������ѡģʽ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CEVDP?
     *     [���]: <CR><LF>+CEVDP: <setting><CR><LF>
     *     [����]: +CEVDP=?
     *     [���]: <CR><LF>+CEVDP: (list of supported <setting>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <setting>: ����ֵ����������ģʽ��Ĭ��ֵ�ɾ��������̾�����
     *             1��CS Voice only
     *             2��CS Voice preferred, IMS PS Voice as secondary
     *             3��IMS PS Voice preferred, CS Voice as secondary
     *             4��IMS PS Voice only
     * [ʾ��]:
     *     �� ��������CS������ģʽ
     *       AT+CEVDP=2
     *       OK
     */
    { AT_CMD_CEVDP,
      AT_SetCevdpPara, AT_SET_PARA_TIME, AT_QryCevdpPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CEVDP", (VOS_UINT8 *)"(1-4)" },

#endif

#if (FEATURE_AT_HSUART == FEATURE_ON)
    { AT_CMD_IPR,
      AT_SetIprPara, AT_NOT_SET_TIME, AT_QryIprPara, AT_NOT_SET_TIME, AT_TestIprPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+IPR", (VOS_UINT8 *)"(0-4000000)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ô���ͨ�ŵ�֡��ʽ��У���㷨
     * [˵��]: �������������ô���ͨ�ŵ�֡��ʽ��У���㷨��
     *         ������ֻ֧��HSUART�˿��·��������˿��·�ֱ�ӷ���OK��
     *         ֻ����MBBģ���Ʒ��
     * [�﷨]:
     *     [����]: AT+ICF[=<format>[,<parity>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: AT+ICF?
     *     [���]: <CR><LF>+ICF: <format>[,<parity>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: AT+ICF=?
     *     [���]: <CR><LF>+ICF: (list of supported <format>s),(list of supported <parity>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <format>: ����ֵ��֡��ʽ��
     *             0���Զ���⣨�ݲ�֧�֣���
     *             1��8������λ2��ֹͣλ��
     *             2��8������λ1��У��λ1��ֹͣλ��
     *             3��8������λ1��ֹͣλ��
     *             4��7������λ2��ֹͣλ��
     *             5��7������λ1��У��λ1��ֹͣλ��
     *             6��7������Ϊ1��ֹͣλ��
     *     <parity>: ����ֵ��У���㷨��
     *             0����У�飻
     *             1��żУ�飻
     *             2�����У�飨��֧�֣���
     *             3����У�飨��֧�֣���
     * [ʾ��]:
     *     �� ���ô���ͨ�ŵ�֡��ʽ��У���㷨
     *       AT+ICF=3,0
     *       OK
     *     �� ��ѯ����ͨ�ŵ�֡��ʽ��У���㷨
     *       AT+ICF?
     *       +ICF: 3,0
     *       OK
     *     �� ��������
     *       AT+ICF=?
     *       +ICF: (0-6),(0-1)
     *       OK
     */
    { AT_CMD_ICF,
      AT_SetIcfPara, AT_NOT_SET_TIME, AT_QryIcfPara, AT_NOT_SET_TIME, AT_TestIcfPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+ICF", (VOS_UINT8 *)"(1-6),(0-3)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ô��ڵ����ط�ʽ
     * [˵��]: �������������ô���ͨ�ŵ����ط�ʽ��
     *         ������ֻ֧��HSUART�˿���̬�������˿���ֱ̬�ӷ���OK��
     *         ֻ����MBBģ���Ʒ��������������ֻ��ͬʱ������ͬʱ�رա�
     * [�﷨]:
     *     [����]: AT+IFC[=<DCE_by_DTE>
     *             [,<DTE_by_DCE>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: AT+IFC?
     *     [���]: <CR><LF>+IFC: <DCE_by_DTE>,<DTE_by_DCE><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: AT+IFC=?
     *     [���]: <CR><LF>+IFC: (list of supported <DCE_by_DTE>s),(list of supported <DTE_by_DCE>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <DCE_by_DTE>: ����ֵ��DTE����DCE���ݷ�ʽ
     *             0����
     *             1��XON/XOFF���ƣ�������������ȥ��XON/XOFF�ַ����ݲ�֧�֣���
     *             2��Ӳ�����أ�
     *             3��XON/XOFF���ƣ���ȥ����������XON/XOFF�ַ����ݲ�֧�֣���
     *     <DTE_by_DCE>: ����ֵ��DCE����DTE���ݷ�ʽ
     *             0���ޣ�
     *             1��XON/XOFF���ƣ�������������ȥ��XON/XOFF�ַ����ݲ�֧�֣���
     *             2��Ӳ�����ء�
     * [ʾ��]:
     *     �� ���ô���ͨ�ŵ����ط�ʽ
     *       AT+IFC=2,2
     *       OK
     *     �� ��ѯ����ͨ�ŵ����ط�ʽ
     *       AT+IFC?
     *       +ICF: 2,2
     *       OK
     *     �� ��������
     *       AT+IFC=?
     *       +ICF: (0,2),(0,2)
     *       OK
     */
    { AT_CMD_IFC,
      AT_SetIfcPara, AT_NOT_SET_TIME, AT_QryIfcPara, AT_NOT_SET_TIME, AT_TestIfcPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+IFC", (VOS_UINT8 *)"(0,2),(0,2)" },
#endif
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ʱ������
     * [˵��]: ��������ʱ������������á���ѯʱ��ֵ��Ŀǰ��ѯ��ʱ��ֵ��ʵʱʱ�䣬��ֻ�ڿ���ʱ��ȡһ��ʱ�䣬֮���ѯ����ʱ�䲻�䡣
     * [�﷨]:
     *     [����]: +CCLK=<time>
     *     [���]: �ɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: +CCLK?
     *     [���]: <CR><LF +CCLK: <time><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CCLK=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <time>: �ַ������ͣ���ʽΪ��"yy/MM/dd,hh:mm:ss��zz"����"yyyy/MM/dd,hh:mm:ss��zz"��
     * [ʾ��]:
     *     �� ����ʱ��
     *       AT+CCLK="17/12/12,11:35:26+10"
     *       OK
     *     �� ��ѯʱ��
     *       AT+CCLK?
     *       +CCLK: "17/12/12,11:35:26+10"
     *       OK
     *     �� ��������
     *       AT+CCLK=?
     *       OK
     */
    { AT_CMD_CCLK,
      AT_SetCclkPara, AT_NOT_SET_TIME,
#if (FEATURE_MBB_CUST == FEATURE_ON)
      At_MbbQryCclkPara,
#else
      AT_QryCclkPara,
#endif
      AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CCLK", (VOS_UINT8 *)"(time)" },

    { AT_CMD_CTZU,
      AT_SetCtzuPara, AT_NOT_SET_TIME, AT_QryCtzuPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CTZU", (VOS_UINT8 *)"(0-1)" },
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    { AT_CMD_CRM,
      AT_SetCrmPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CRM", (VOS_UINT8 *)"(1,2)" },

    { AT_CMD_CTA,
      AT_SetCtaPara, AT_SET_PARA_TIME, AT_QryCtaPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CTA", (VOS_UINT8 *)"(0-255)" },
#endif

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��������������������
     * [˵��]: ���õ�ǰUEģʽΪ��������ģʽ������������ģʽ��
     * [�﷨]:
     *     [����]: +CEUS=[<setting>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CEUS?
     *     [���]: <CR><LF>+CEUS: <setting><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CEUS=?
     *     [���]: <CR><LF>+CEUS: (list of supported <setting>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <setting>: ����ֵ��������������ģʽ����������ʱĬ��������������
     *             0����������
     *             1����������
     * [ʾ��]:
     *     �� ������������
     *       AT+CEUS=0
     *       OK
     *     �� ��ѯ������������״̬
     *       AT+CEUS?
     *       +CEUS: 0
     *       OK
     *     �� ��������
     *       AT+CEUS=?
     *       +CEUS: (0,1)
     *       OK
     */
    { AT_CMD_UE_CENTER,
      AT_SetUECenterPara, AT_SET_PARA_TIME, AT_QryUECenterPara, AT_QRY_PARA_TIME, AT_TestUECenterPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CEUS", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���ÿ���˫���л�����
     * [˵��]: ��AT��������֪ͨmodem����˫���л�����������ػ�֮���·���
     *         �Ը�AT�����Լ����
     *         ��1����AT������Ҫ�����³���ʹ�ã�
     *         ��: EMUI���棬�û��������ҵ���л����µ��п�
     *         ��: ˫�������£��û�ȥ����������ԭ�еĸ������г���������
     *         ��: ˫��ȥ���������£�����ԭ���ĸ���(��ʱ���������)��
     *         ��2����AT����ֻ����modem��ػ����·���
     * [�﷨]:
     *     [����]: ^QUICKCARDSWITCH=<SAMPLMN>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ����ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <SAMPLMN>: ָʾ�Ƿ�Ϊ��ͬ��Ӫ�̿���˫���л�������
     *             0��Ϊ˫����ͬ��Ӫ�̳�����
     *             1��Ϊ˫����ͬ��Ӫ�̳�����
     *             ����Ϊ����˫���л�������
     *             1����EMUI���棬�û��������ҵ���л����µ��п���
     *             2����˫�������£��û�ȥ����������ԭ�еĸ������г���������
     *             3����˫��ȥ���������£�����ԭ���ĸ���(��ʱ���������)��
     * [ʾ��]:
     *     �� ָʾΪ��ͬ��Ӫ�̿���˫���л�������
     *       AT^QUICKCARDSWITCH=1��
     *       OK
     */
    { AT_CMD_QUICK_CARD_SWITCH,
      AT_SetQuickCardSwitchPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^QUICKCARDSWITCH", (VOS_UINT8 *)"(0,1)" },
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �����ն�������ģʽ
     * [˵��]: ��������ն����������ó����ݡ���������������������ģʽ��
     *         Ŀǰ�ݲ�֧�֣�����ERROR��
     * [�﷨]:
     *     [����]: +FCLASS=<cmd>
     *     [���]: ����ִ��ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <cmd>: �ն�������ģʽ��������
     * [ʾ��]:
     *     �� ִ����������
     *       AT+FCLASS=1
     *       ERROR
     *     �� ִ�в�ѯ����
     *       AT+FCLASS?
     *       ERROR
     *     �� ִ�в�������
     *       AT+FCLASS=?
     *       ERROR
     */
    { AT_CMD_FCLASS,
      AT_SetFclassPara, AT_SET_PARA_TIME, AT_QryFclassPara, AT_QRY_PARA_TIME, At_TestFclass, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+FCLASS", (VOS_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ����T.35������
     * [˵��]: ��������������T.35�����롣
     *         Ŀǰ�ݲ�֧�ַ���ERROR��
     * [�﷨]:
     *     [����]: +GCI=<cmd>
     *     [���]: ����ִ��ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <cmd>: ����T.35�����룬������
     * [ʾ��]:
     *     �� ִ����������
     *       AT+GCI=1
     *       ERROR
     *     �� ִ�в�ѯ����
     *       AT+GCI?
     *       ERROR
     *     �� ִ�в�������
     *       AT+GCI=?
     *       ERROR
     */
    { AT_CMD_GCI,
      AT_SetGciPara, AT_SET_PARA_TIME, AT_QryGciPara, AT_QRY_PARA_TIME, At_TestGci, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+GCI", (VOS_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����������ʾ
     * [˵��]: �򿪻�ر��������Ƶ���ʾ���ܣ���ѯ�û�����������ʾ����ҵ���ǩԼ����������������������ʾ�������û�ǩԼ������������ʾ����ҵ��������ʱ��ͨ�������ϱ�AT����+CNAP�ϱ��������ơ�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CNAP=<n>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: +CNAP?
     *     [���]: <CR><LF>+CNAP: <n>,<m><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CNAP=?
     *     [���]: <CR><LF>+CNAP: (list of supported <n>s)<CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ���򿪻�ر�+CNAP����������ϱ���Ĭ��ֵΪ0��
     *             0���رգ�
     *             1���򿪡�
     *     <m>: ����ֵ������������ʾҵ���������ǩԼ״̬��
     *             0��CNAPҵ��δ�ṩ��
     *             1��CNAPҵ�����ṩ��
     *             2��δ֪������ԭ�򣩡�
     * [ʾ��]:
     *     �� ������������ʾ����
     *       AT+CNAP=1
     *       OK
     *     �� ��ѯ����������ʾҵ��ǩԼ״̬
     *       AT+CNAP?
     *       +CNAP: 1,1
     *       OK
     *     �� ִ�в�������
     *       AT+CNAP=?
     *       +CNAP: (0,1)
     *       OK
     */
    { AT_CMD_CNAP,
      AT_SetCnapPara, AT_SET_PARA_TIME, AT_QryCnapPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CNAP", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����IPv6��ַ��ӡ��ʽ
     * [˵��]: ���������ڿ���AT�����������IPv6��ַ�Ĵ�ӡ��ʽ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CGPIAF=[<IPv6_AddressFormat>[,<IPv6_SubnetNotation>[,<IPv6_LeadingZeros>[,<IPv6_CompressZeros>]]]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: +CGPIAF?
     *     [���]: <CR><LF>+CGPIAF: <IPv6_AddressFormat>, <IPv6_SubnetNotation>, <IPv6_LeadingZeros>, <IPv6_CompressZeros><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CGPIAF=?
     *     [���]: <CR><LF>+CGPIAF: (list of supported <IPv6_AddressFormat>s),(list of supported <IPv6_SubnetNotation>s),(list of supported <IPv6_LeadingZeros>s),(list of supported <IPv6_CompressZeros>s)<CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <IPv6_AddressFormat>: ����ֵ��IPv6��ַ��ʽ��Ĭ��ֵΪ0��
     *             0��ʹ������IPv4�ĵ�ָ�����IP��ַ���������루������ڣ�Ҳͨ����ָ���
     *             1��ʹ������IPv6��ð�ŷָ�����IP��ַ���������루������ڣ�ͨ���ո�ָ���
     *     <IPv6_SubnetNotation>: ����ֵ�����������������<remote address and subnet mask>�ĸ�ʽ��<IPv6_AddressFormat>����Ϊ1��Ч��
     *             0��IP��ַ����������������ʾ��ͨ���ո�ָ���
     *             1��ͨ��б�߷ָ�����ǰ׺��������IP��ַ��
     *     <IPv6_LeadingZeros>: ����ֵ�������Ƿ�ʡ��ǰ��0��<IPv6_AddressFormat>����Ϊ1��Ч��
     *             0��ʡ��ǰ��0��
     *             1������ǰ��0��
     *     <IPv6_CompressZeros>: ����ֵ�����ƶ��16bitֵΪ0��ʵ���Ƿ�ʹ�á�::���滻��ѹ��0��ʽ����<IPv6_AddressFormat>����Ϊ1��Ч��
     *             0����ѹ��0��
     *             1��ѹ��0��
     * [ʾ��]:
     *     �� ����IP��ӡ��ʽ����
     *       AT+CGPIAF=0,0,0,0
     *       OK
     *     �� ��ѯIP��ӡ��ʽ����
     *       AT+CGPIAF?
     *       +CGPIAF: 0,0,0,0
     *       OK
     *     �� ��������
     *       AT+CGPIAF=?
     *       +CGPIAF: (0,1),(0,1),(0,1),(0,1)
     *       OK
     */
    { AT_CMD_CGPIAF,
      AT_SetCgpiafPara, AT_NOT_SET_TIME, AT_QryCgpiafPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CGPIAF", (VOS_UINT8 *)"(0,1),(0,1),(0,1),(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �������ڸ�ʽ
     * [˵��]: ��������Ҫ�����������ڸ�ʽ��ʽ��Ŀǰֻ֧�ֲ��ֹ��ܡ�
     * [�﷨]:
     *     [����]: +CSDF=[[<mode>][,<auxmode>]]
     *     [���]: �ɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: +CSDF?
     *     [���]: <CR><LF +CSDF: <mode>[,<auxmode>] <CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CSDF=?
     *     [���]: <CR><LF> +CSDF: (list of supported <mode>s)[,(list of supported <auxmode>s)]<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ��������ʾ��ʽ��������ʱ��֧�֣���ȡֵ��ΧΪ(1-255)��
     *             1��DD-MMM-YYYY��
     *             2��DD-MM-YY��
     *             3��MM/DD/YY��
     *             4��DD/MM/YY��
     *             5��DD.MM.YY��
     *             6��YYMMDD��
     *             7��YY-MM-DD��
     *             8-255��������
     *     <auxmode>: +CCLK��+CALA���ݲ�֧�֣���<time>��ʽ����ʽ������ֵ��ȡֵ��Χ(1,2)��Ĭ��ֵΪ1��
     *             1��yy/MM/dd��
     *             2��yyyy/MM/dd��
     * [ʾ��]:
     *     �� �������ڸ�ʽ��ʽ
     *       AT+CSDF=1,1
     *       OK
     *     �� ��ѯ���ڸ�ʽ��ʽ
     *       AT+CSDF?
     *       +CSDF: 1,1
     *       OK
     *     �� ��������
     *       AT+CSDF=?
     *       +CSDF: (1-255),(1-2)
     *       OK
     */
    { AT_CMD_CSDF,
      AT_SetCsdfPara, AT_NOT_SET_TIME, AT_QryCsdfPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CSDF", (VOS_UINT8 *)"(1-255),(1-2)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: �������¼��ϱ�������������
     * [˵��]: ��������Ҫ���ڷ������¼��ϱ����Ե��ƶ�����Ҫ����+CGEV����ݲ�֧�֣���
     * [�﷨]:
     *     [����]: +CGEREP=[<mode>[,<bfr>]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: +CGEREP?
     *     [���]: <CR><LF>+CGEREP: <mode>,<bfr><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CGEREP=?
     *     [���]: <CR><LF>+CGEREP: (list of supported <mode>s),(list of supported
     *             <bfr>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ�����������ϱ��洢���ԣ�ȡֵ��Χ(0-2)��Ĭ��ֵΪ0��
     *     <bfr>: ����ֵ�����ƻ���������ϱ���Ϊ���ԣ�ȡֵ��Χ(0-1)��Ĭ��ֵΪ0��
     * [ʾ��]:
     *     �� ���÷������¼��ϱ�����
     *       AT+CGEREP=0,0
     *       OK
     *     �� ��ѯ�������¼��ϱ�����
     *       AT+CGEREP?
     *       +CGEREP: 0,0
     *       OK
     *     �� ��������
     *       AT+CGEREP=?
     *       +CGEREP: (0-2),(0,1)
     *       OK
     */
    { AT_CMD_CGEREP,
      AT_SetCgerepPara, AT_SET_PARA_TIME, AT_QryCgerepPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CGEREP", (VOS_UINT8 *)"(0-2),(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ָ���������
     * [˵��]: ��������ָ��������Ŀǰֻ֧���ź�ָ����ơ�
     * [�﷨]:
     *     [����]: +CIND=[<ind>[,<ind>[,...]]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: +CIND?
     *     [���]: <CR><LF>+CIND: <ind>[,<ind>[,...]]<CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: +CIND=?
     *     [���]: <CR><LF>+CIND: (<descr>,(list of supported
     *             <ind>s))[,(<descr>,(list of supported
     *             <ind>s))[,...]]<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <ind>: ����ֵ��ָ�����ֵ��ȡֵ��Χ�ο�<descr>��������
     *     <descr>: Ŀǰֻ֧�֡�signal��ָ�ꡣ
     *             "signal"���ź�����(0-5)��
     * [ʾ��]:
     *     �� ����ָ��
     *       AT+CIND=1
     *       OK
     *     �� ��ѯָ��
     *       AT+CIND?
     *       +CIND: 1
     *       OK
     *     �� ��������
     *       AT+CIND=?
     *       +CIND: "signal",(0-5)
     *       OK
     */
    { AT_CMD_CIND,
      AT_SetCindPara, AT_SET_PARA_TIME, AT_QryCindPara, AT_QRY_PARA_TIME, AT_TestCindPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CIND", (VOS_UINT8 *)"(0-5)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �����ҶϿ���
     * [˵��]: �����������ATH�Ƿ���ԹҶ��������С�
     *         ��ȡ����ص�ǰģʽ
     *         ���������֧�ֵĲ�����Χ
     *         Ĭ�ϳ�ʼֵΪ0����֧��ATH�Ҷ�����ͨ��������ʼֵ��ͨ��NV
     *         en_NV_Item_Custom_Call_Cfg ���ã�������Ϊ1����֧��ATH�Ҷ�����
     * [�﷨]:
     *     [����]: +CVHU=[<mode>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CVHU?
     *     [���]: <CR><LF> +CVHU: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +CVHU=?
     *     [���]: <CR><LF>+CVHU: (list of supported <mode>s)
     *             <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: 0 ���·�ATH���ԹҶ�����ͨ��
     *             1 ���·�ATHֻ�Ƿ���OK������Ӱ������ͨ��
     * [ʾ��]:
     *       �·�AT+CVHU=0������ͨ�����·�ATH�ҶϺ��У����жϿ���
     *       �·�AT+CVHU=1������ͨ�����·�ATH�ҶϺ��У�ֻ�Ƿ���OK������δ�Ͽ���
     */
    { AT_CMD_CVHU,
      AT_SetCvhuPara, AT_SET_PARA_TIME, AT_QryCvhuPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CVHU", (VOS_UINT8 *)"(0-1)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����5G QoS
     * [˵��]: ���û�ɾ��ָ��cid��5G QoS������
     *         ���������ֻ��<cid>һ����������AT+C5GQOS=<cid>����ʾɾ����<cid>��Ӧ��5G QoS������
     *         ���������3GPP TS 27.007 ��R15���ϣ�Э�顣
     * [�﷨]:
     *     [����]: +C5GQOS=[<cid>[,<5QI>[,<DL_GFBR>,<UL_GFBR>[,<DL_MFBR>,<UL_MFBR]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err> <CR><LF>
     *     [����]: +C5GQOS?
     *     [���]: [+C5GQOS: <cid>,<5QI>[,<DL_GFBR>,<UL_GFBR>[,<DL_MFBR>,<UL_MFBR>]]
     *             [<CR><LF>+C5GQOS: <cid>,<5QI>,[<DL_GFBR>,<UL_GFBR>[,<DL_MFBR>,<UL_MFBR>]]
     *             [...]]<CR><LF><CR><LF>OK<CR><LF>
     *     [����]: +C5GQOS=?
     *     [���]: +C5GQOS: (range of supported <cid>s),(list of supported <5QI>s),(list of supported <DL_GFBR>s),(list of supported <UL_GFBR>s),(list of supported <DL_MFBR>s),(list of supported <UL_MFBR>s)
     *             <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             ȡֵ��Χ1~31��
     *     <5QI>: ����ֵ����AT��������ʱ��modem���Դ˲������м�飩
     *             0: ��ʾ��ֵ������ȷ����
     *             1-4 65,66,,67, 71-76:    values for guaranteed bit rate QoS flows
     *             5-9 69,70,79,80:  value range for non-guaranteed bit rate QoS flows
     *             10-12 16,17:     value range for delay critical guaranteed bit rate QoS flows
     *             128-254: ��Ӫ�̶���ֵ
     *     <DL_GFBR>: ����ֵ��0~4294967295����ʾGBR�µ�DL GFBR����λ��kbit/s���˲�������5QIΪGBRȡֵʱ��Ч
     *     <UL_GFBR>: ����ֵ��0~4294967295����ʾGBR�µ�UL GFBR����λ��kbit/s���˲�������5QIΪGBRȡֵʱ��Ч
     *     <DL_MFBR>: ����ֵ��0~4294967295����ʾGBR�µ�DL MFBR����λ��kbit/s���˲�������5QIΪGBRȡֵʱ��Ч
     *     <UL_MFBR>: ����ֵ��0~4294967295����ʾGBR�µ�UL MFBR����λ��kbit/s���˲�������5QIΪGBRȡֵʱ��Ч
     * [ʾ��]:
     *     �� ��������
     *       AT+C5GQOS=1,1,1000,2000,3000,4000
     *       OK
     *     �� ��ѯ����
     *       AT+C5GQOS?
     *       +C5GQOS: 1,1,1000,2000,3000,4000
     *       +C5GQOS: 2,65,1000,1000,1000,1000
     *       OK
     *     �� ��������
     *       AT+C5GQOS=?
     *       +C5GQOS: (1-31),(0-12,16-17,65-67,69-76,79-80,128-254),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)
     *       OK
     */
    { AT_CMD_C5GQOS,
      AT_SetC5gqosPara, AT_SET_PARA_TIME, AT_QryC5gqosPara, AT_QRY_PARA_TIME, At_TestC5gqos, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+C5GQOS", (VOS_UINT8 *)"(1-31),(0-12,16-17,65-67,69-76,79-80,128-254),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯ5G ��̬QoS����
     * [˵��]: ��ѯָ��cid����ȫ���Ѽ����cid�����������5G QoS������
     *         ���������ֻ��<cid>һ����������AT+C5GQOSRDP=<cid>����ʾ��ѯ<cid>��Ӧ��5G QoS������
     *         ��������в�Я��<cid>��������AT+C5GQOSRDP����ʾ��ѯ�����Ѽ����QoS flow��Ӧ��5G QoS������
     *         ��������ش��ڼ���״̬��PDP�����Ĺ�����<cid>�б�
     *         ���������3GPP TS 27.007 ��R15���ϣ�Э�顣
     * [�﷨]:
     *     [����]: +C5GQOSRDP[=<cid>]
     *     [���]: [+C5GQOSRDP: <cid>,<5QI>[,<DL_GFBR>,<UL_GFBR>[,<DL_MFBR>,<UL_MFBR>[,<DL_SAMBR>,<UL_SAMBR>[,<Averaging_window>]]]]]
     *             [<CR><LF>+C5GQOSRDP: <cid>,<5QI>[,<DL_GFBR>,<UL_GBR>[,<DL_MFBR>,<UL_MFBR>[,<DL_SAMBR>,<UL_SAMBR>[,<Averaging_window>]]]]
     *             [...]]
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +C5GQOSRDP=?
     *     [���]: <CR><LF>+C5GQOSRDP: (list of <cid>s associated with active contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             ȡֵ��Χ1~31��
     *     <5QI>: ����ֵ����AT��������ʱ��modem���Դ˲������м�飩
     *             0:             ��ʾ��ֵ������ȷ����
     *             1-4 65,66,75:    values for guaranteed bit rate QoS flows
     *             5-9 69,70,79,80:  value range for non-guaranteed bit rate QoS flows
     *             10-12 16,17:     value range for delay critical guaranteed bit rate QoS flows
     *             128-254:        ��Ӫ�̶���ֵ
     *     <DL_GFBR>: ����ֵ��0~4294967295����ʾGBR�µ�DL GFBR����λ��kbit/s���˲�������5QIΪGBRȡֵʱ��Ч
     *     <UL_GFBR>: ����ֵ��0~4294967295����ʾGBR�µ�UL GFBR����λ��kbit/s���˲�������5QIΪGBRȡֵʱ��Ч
     *     <DL_MFBR>: ����ֵ��0~4294967295����ʾGBR�µ�DL MFBR����λ��kbit/s���˲�������5QIΪGBRȡֵʱ��Ч
     *     <UL_MFBR>: ����ֵ��0~4294967295����ʾGBR�µ�UL MFBR����λ��kbit/s���˲�������5QIΪGBRȡֵʱ��Ч
     *     <UL_SAMBR>: ����ֵ��0~4294967295����ʾUL PDU session AMBR����λ��kbit/s��
     *     <DL_SAMBR>: ����ֵ��0~4294967295����ʾDL PDU session AMBR����λ��kbit/s��
     *     <Averaging_window>: ����ֵ��0~65535����ʾaveraging window����λ��ms��
     * [ʾ��]:
     *     �� ��ѯ����
     *       AT+C5GQOSRDP=1
     *       +C5GQOSRDP: 1,1,1000,2000,3000,4000,10000,10000,2000
     *       OK
     *     �� ��������
     *       AT+C5GQOSRDP=?
     *       +C5GQOSRDP: (1,9)
     *       OK
     */
    { AT_CMD_C5GQOSRDP,
      AT_SetC5gqosRdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestC5gQosRdp, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+C5GQOSRDP", (VOS_UINT8 *)"(1-31)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ������ѡ��Ƭ
     * [˵��]: ���������������û�����ѡ��Ƭ��Ϣ����������������3GPP����ѡ��Ƭ��ĿǰNON_3GPP����ѡ��Ƭ��֧�����á�Prefer��Ƭ�Ǹ�HPLMN�󶨵ģ����õ�Prefer��Ƭ����Mapped SST��Mapped SD��Ϣ��
     * [�﷨]:
     *     [����]: +C5GPNSSAI=
     *             [<Preferred_NSSAI_3gpp_length>,
     *             [<Preferred_NSSAI_3gpp>]],
     *             [<Preferred_NSSAI_non3gpp_length>,
     *             [<Preferred_NSSAI_non3gpp>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +C5GPNSSAI?
     *     [���]: <CR><LF>+C5GPNSSAI: <Preferred_NSSAI_3gpp_length>,
     *             <Preferred_NSSAI_3gpp>,
     *             <Preferred_NSSAI_non3gpp_length>,
     *             <Preferred_NSSAI_non3gpp><C
     *             R><LF><CR><LF>OK<CR><LF>
     *     [����]: +C5GPNSSAI=?
     *     [���]: +C5GPNSSAI: (range of supported <Preferred_NSSAI_3gpp_length>s),(range of supported <Preferred_NSSAI_non3gpp_length>s)
     *             <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <Preferred_NSSAI_3gpp_length>: AT�������õ�3GPP��ѡ��Ƭ�ֽڸ���
     *     <Preferred_NSSAI_3gpp>: 3GPP��ѡ��Ƭ������Ŀǰ֧�����8��Prefer��Ƭ�����ã�
     *             ������Ƭ�ĸ�ʽ��SST����SST.SD��
     *             ��ͬ����Ƭ�÷��š�:���ֿ�����������SST:SST.SD��
     *     <Preferred_NSSAI_non3gpp_length>: NON_3GPPĿǰ��֧��
     *     <Preferred_NSSAI_non3gpp>: NON_3GPPĿǰ��֧��
     * [ʾ��]:
     *     �� ������ѡ��Ƭ
     *       AT+C5GPNSSAI=8,"01:02:03:04:05:06:07:08"
     *       OK
     *     �� ��ѯ��ѡ��Ƭ
     *       AT+C5GPNSSAI?
     *       +C5GPNSSAI: 8,"01:02:03:04:05:06:07:08"
     *       OK
     */
    { AT_CMD_C5GPNSSAI,
      AT_SetC5gPNssaiPara, AT_SET_PARA_TIME, AT_QryC5gPNssaiPara, AT_QRY_PARA_TIME, At_TestC5gPNssai, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+C5GPNSSAI", (VOS_UINT8 *)"(0-159),(pnssaiStr)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ����Default Config��Ƭ
     * [˵��]: ���������������û���default Config��Ƭ��Ϣ
     * [�﷨]:
     *     [����]: +C5GNSSAI=
     *             <default_configured_nssai_length>,
     *             <default_configured_nssai>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +C5GNSSAI?
     *     [���]: <CR><LF>+C5GNSSAI: [<default_configured_nssai_length>,
     *             <default_configured_nssai>]<C
     *             R><LF><CR><LF>OK<CR><LF>
     *     [����]: +C5GNSSAI=?
     *     [���]: +C5GNSSAI: (range of supported <default_configured_nssai_length>s),(range of supported <default_configured_nssai>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <default_configured_nssai_length>: AT�������õ�Default Config��Ƭ�ֽڸ���
     *     <default_configured_nssai>: 3GPP��ѡ��Ƭ������Ŀǰ֧�����16��Default Config��Ƭ������
     *             ������Ƭ�ĸ�ʽ����SST�����ߡ�SST.SD����
     *             ��ͬ����Ƭ�÷��š�:���ֿ�����������SST:SST.SD��
     * [ʾ��]:
     *     �� ����Default config��Ƭ
     *       AT+C5GNSSAI=8,"01:02:03:04:05:06:07:08"
     *       OK
     *     �� ��ѯDefault config��Ƭ
     *       AT+C5GNSSAI?
     *       +C5GNSSAI: 8,"01:02:03:04:05:06:07:08"
     *       OK
     */
    { AT_CMD_C5GNSSAI,
      AT_SetC5gNssaiPara, AT_SET_PARA_TIME, AT_QryC5gNssaiPara, AT_QRY_PARA_TIME, At_TestC5gNssai, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+C5GNSSAI", (VOS_UINT8 *)"(0-319),(nssaiStr)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯ��̬��Ƭ��Ϣ
     * [˵��]: ���������ڲ�ѯ�û���̬�����Default Config Nssai��Reject Nssai��Config Nssai��Allow Nssai��Ƭ��Ϣ��
     * [�﷨]:
     *     [����]: +C5GNSSAIRDP
     *             [=<nssai_type>[,<plmn_id>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             [+C5GNSSAIRDP: [<default_configured_nssai_length>,
     *             <default_configured_nssai>[,<rejected_nssai_3gpp_length>,<rejected_nssai_3gpp>[,<rejected_nssai_non3gpp_length>,<rejected_nssai_non3gpp>]]][<CR><LF>+C5GNSSAIRDP: <plmn_id>[,<configured_nssai_length>,<configured_nssai>
     *             [,<allowed_nssai_3gpp_length>,<allowed_nssai_3gpp>,
     *             <allowed_nssai_non3gpp_length>,<allowed_nssai_non3gpp>]][<CR><LF>+C5GNSSAIRDP: <plmn_id>[,<configured_nssai_length>,<configured_nssai>
     *             [,<allowed_nssai_3gpp_length>,<allowed_nssai_3gpp>,<allowed_nssai_non3gpp_length>,<allowed_nssai_non3gpp>]][...]]]]
     *     [����]: +C5GNSSAIRDP=?
     *     [���]: +C5GNSSAIRDP: (list of supported <nssai_type>s),(range of supported <plmn_id>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <nssai_type>: ��ѯ����Ƭ����
     *             0������default configured NSSAI
     *             1������default configured NSSAI�Լ�rejected NSSAI(s)
     *             2������default configured NSSAI, rejected NSSAI(s) �Լ�  configured NSSAI(s)
     *             3������default configured NSSAI, rejected NSSAI(s), configured NSSAI(s) �Լ�allowed NSSAI(s)
     *     <plmn_id>: ������ʽ��PLMN��Ϣ������������ַ�������ǰ��λ��MCC������λ����λ��MNC��
     *     <default_configured_nssai_length>: Default config nssai��Ƭ�ֽڸ���
     *     <default_configured_nssai>: Default config nssai
     *     <rejected_nssai_3gpp_length>: Reject nssai��Ƭ�ֽڸ���
     *     <rejected_nssai_3gpp>: ��ǰפ��PLMN��Ӧ��Reject nssai��Ƭ����
     *     <rejected_nssai_non3gpp_length>: NON_3GPP����Ŀǰ��֧��
     *     <rejected_nssai_non3gpp>: NON_3GPP����Ŀǰ��֧��
     *     <configured_nssai_length>: Config nssai��Ƭ�ֽڸ���
     *     <configured_nssai>: Config nssai����
     *     <allowed_nssai_3gpp_length>: 3gpp Allowed nssai��Ƭ�ֽڸ���
     *     <allowed_nssai_3gpp>: 3gpp Allowed nssai����
     *     <allowed_nssai_non3gpp_length>: NON_3GPP����Ŀǰ��֧��
     *     <allowed_nssai_non3gpp>: NON_3GPP����Ŀǰ��֧��
     * [ʾ��]:
     *     �� ��ѯ������Ƭ��Ϣ
     *       AT+C5GNSSAIRDP=3,"46010"
     *       +C5GNSSAIRDP: 1,"01",2,"02:03"
     *       +C5GNSSAIRDP: "46010",1,"01",1,"01"
     *       OK
     */
    { AT_CMD_C5GNSSAIRDP,
      AT_SetC5gNssaiRdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestC5gNssaiRdp, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+C5GNSSAIRDP", (VOS_UINT8 *)"(0,1,2,3),(plmnStr)" },
#endif

#if (FEATURE_MBB_CUST == FEATURE_ON)
#if (FEATURE_PHONE_USIM == FEATURE_OFF)
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: �����޿����ٿ���
     * [˵��]: ����������һ���������޿����ٿ������ܣ������ÿ��ٿ������ܵ�ʹ�ܺ�ȥʹ�ܣ��������µ���Զ���Ϊȥʹ�ܡ�
     *         ��AT�������õĿ��ٿ���ʹ��״̬��ͨ��NV6656���õĿ��ٿ���ʹ��״̬��һ���ġ�Ψһ��������AT�������õ�״̬���������µ���ʧЧ��
     *         ��AT�������ÿ��ٿ����Ĺ�������ΪNV6656���ù��ܵ�һ�ֲ��䣬�ŵ��������������塣
     * [�﷨]:
     *     [����]: ^NOCARD=<card_mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NOCARD=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <card_mode>: ���ÿ��ٿ���ʹ�ܺ�ȥʹ�ܡ�1��ʹ�ܣ�0��ȥʹ�ܡ�
     * [ʾ��]:
     *       AT^NOCARD=1
     *       OK
     *       AT^NOCARD=0
     *       OK
     */
    { AT_CMD_NOCARD,
      AT_SetNoCard, AT_SET_PARA_TIME, AT_QueryNoCard, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NOCARD", (VOS_UINT8 *)"(0,1)" },
#endif
#endif
};

VOS_UINT32 At_RegisterExCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atExtendCmdTbl, sizeof(g_atExtendCmdTbl) / sizeof(g_atExtendCmdTbl[0]));
}

const AT_ParCmdElement* At_GetExtendCmdTable(VOS_VOID)
{
    return g_atExtendCmdTbl;
}

VOS_UINT32 At_GetExtendCmdNum(VOS_VOID)
{
    return sizeof(g_atExtendCmdTbl) / sizeof(g_atExtendCmdTbl[0]);
}

