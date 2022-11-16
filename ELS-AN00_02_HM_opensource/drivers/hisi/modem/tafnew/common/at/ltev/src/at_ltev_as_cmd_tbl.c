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
#include "at_ltev_as_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_ltev_as_set_cmd_proc.h"
#include "at_ltev_as_qry_cmd_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_LTEV_AS_CMD_TBL_C

#if (FEATURE_LTEV == FEATURE_ON)
static const AT_ParCmdElement g_atLtevAsCmdTbl[] = {
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: �������ģʽ
     * [˵��]: �������������ü������ģʽ��
     * [�﷨]:
     *     [����]: +CATM=[<status>[,<test_loop_mode>]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: +CATM?
     *     [���]: ִ�гɹ�ʱ��
     *             +CATM: <status>[,<test_loop_mode>]
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: +CATM=?
     *     [���]: +CATM: (list of supported <status>s),(list of supported <test_loop_mode>s)
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <status>: ����ģʽ����״̬��ȡֵ��Χ0~1��Ĭ��ֵ0��
     *             0��ʾȥ���1��ʾ����
     *     <test_loop_mode>: ����ģʽ���ͣ�ȡֵ��Χ1��
     *             1��ʾ����ģʽE
     * [ʾ��]:
     *     �� ���ü������ģʽ
     *       AT+CATM=1,1
     *       OK
     *     �� ��ѯ����ģʽ����״̬����ǰ���ڲ���ģʽE����״̬��
     *       AT+CATM?
     *       +CATM: 1,1
     *       OK
     */
    { AT_CMD_CATM,
      AT_SetCatm, AT_SET_PARA_TIME, AT_QryCatm, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CATM", (VOS_UINT8 *)"(0,1),(1)" },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: �޸Ĳ���ģʽ������L2ID
     * [˵��]: ������������ +CATM�����ʹ��+CCUTLE��CV2XDTS������з���ʱ�����ݰ���L2 src Id��dst Id��
     * [�﷨]:
     *     [����]: ^CV2XL2ID=<srcId>,<dstId>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^CV2XL2ID?
     *     [���]: ִ�гɹ�ʱ��
     *             ^CV2XL2ID: <src Id>,<dst Id>
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <src Id>: ����ģʽ������L2IDԴ��ID����Χ0-16777215��
     *     <dst Id>: ����ģʽ������L2IDĿ��ID����Χ0-16777215��
     * [ʾ��]:
     *     �� ���ò���ģʽL2ID
     *       AT^CV2XL2ID=187,187
     *       OK
     *     �� ��ѯ����ģʽL2ID
     *       AT^CV2XL2ID?
     *       ^CV2XL2ID: 187,187
     *       OK
     */
    { AT_CMD_CV2XL2ID,
      AT_SetCv2xL2Id, AT_SET_PARA_TIME, AT_QryCv2xL2Id, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CV2XL2ID", (VOS_UINT8 *)"(0-16777215),(0-16777215)" },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: �ջ�����ģʽE
     * [˵��]: �������������ñջ�����ģʽE����3GPP TS 35.509���½�5.4.2��5.4.4c��5.4.5�ж��壩���������ڲ���ģʽ��������²���ʹ�á�
     * [�﷨]:
     *     [����]: +CCUTLE=<status>[,<direction>[,<format>,<length>,<monitor_list>]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: +CCUTLE?
     *     [���]: ִ�гɹ�ʱ��
     *             +CCUTLE: <status>[,<direction>[,<format>,<length>,<monitor_list>]]
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: +CCUTLE=?
     *     [���]: +CCUTLE: (list of supported <status>s),(list of supported <direction>s),(list of supported <format>s)(maximum supported <length>)
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <status>: ����ģʽE�ջ�״̬��ȡֵ��Χ0~1��Ĭ��ֵ1��
     *             0��ʾ�ջ���1��ʾ����
     *     <direction>: ͨ�ŷ���ȡֵ��Χ0~1��
     *             0��ʾ���ݽ��գ�1��ʾ���ݷ���
     *     <format>: ����б������ʽ��ȡֵ��Χ1��
     *             1��ʾ�����Ƹ�ʽ
     *     <length>: ����б�����24bit��ʾ��Ŀ��Layer-2 ID���������֧��16��
     *     <monitor_list>: V2XͨѶ����б�
     * [ʾ��]:
     *     �� ���ý������ģʽE���ݽ���״̬
     *       AT+CCUTLE=0,0,1,1,"000000"
     *       OK
     *     �� ��ѯ����ģʽ״̬����ǰ���ڲ���ģʽE���ݽ���״̬��
     *       AT+CCUTLE?
     *       +CCUTLE: 0,0,1,1,"000000"
     *       OK
     */
    { AT_CMD_CCUTLE,
      AT_SetCcutle, AT_SET_PARA_TIME, AT_QryCcutle, AT_QRY_PARA_TIME, AT_TestCcutle, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CCUTLE", (VOS_UINT8 *)"(0,1),(0,1),(1),(0-16),(str)" },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ��·���ݰ�����
     * [˵��]: ��������������UE�����·�ɹ����յ���PSCCH����顢STCH PDCP SDU����PSSCH������������������ڲ���ģʽ��������²���ʹ�á�
     * [�﷨]:
     *     [����]: +CUSPCREQ
     *     [���]: ִ�гɹ�ʱ��
     *             +CUSPCREQ: [<type1>,<format>,<length1>,<counter1>],[<type2>,<format>,<length2>,<counter2>],[<type3>,<format>,<length3>,<counter3>]
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <type>: V2Xͨ�����ͣ�ȡֵ��Χ1��
     *             1��ʾPSCCH�����
     *     <type2>: V2Xͨ�����ͣ�ȡֵ��Χ1��
     *             1��ʾSTCH PDCP SDU��
     *     <type3>: V2Xͨ�����ͣ�ȡֵ��Χ1��
     *             1��ʾPSSCH�����
     *     <format>: ����������ĸ�ʽ��ȡֵ��Χ1��
     *             1��ʾ�����Ƹ�ʽ
     *     <length1>: <counter1>��Ԫ���ֽ���
     *     <length2>: <counter2>��Ԫ���ֽ���
     *     <length3>: <counter3>��Ԫ���ֽ���
     *     <counter1>: PSCCH���������
     *     <counter2>: STCH PDCP SDU������
     *     <counter3>: PSSCH���������
     * [ʾ��]:
     *     �� �����·���ݰ�������PSCCH����顢STCH PDCP SDU����PSSCH�������������һ��Layer-2 ID��ֻ�յ�һ�����ݰ���
     *       AT+CUSPCREQ
     *       +CUSPCREQ: 1,1,4,"00000000000000000000000000000001",1,1,4,"00000000000000000000000000000001",1,1,4,"00000000000000000000000000000001"
     *       OK
     */
    { AT_CMD_CUSPCREQ,
      AT_SetCuspcreq, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CUSPCREQ", VOS_NULL_PTR },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: UTCʱ������
     * [˵��]: �������������õ�ǰUTCʱ�䡣�������ڲ���ģʽ��������²���ʹ�á�
     * [�﷨]:
     *     [����]: +CUTCR
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� UTCʱ������
     *       AT+CUTCR
     *       OK
     */
    { AT_CMD_CUTCR,
      AT_SetCutcr, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CUTCR", VOS_NULL_PTR },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ����ģʽ���ŵ�æ������
     * [˵��]: ��������������PC5�ϲ�������V2Xͨ���ŵ�æ�ʡ��������ڲ���ģʽ��������²���ʹ�á�
     * [�﷨]:
     *     [����]: +CCBRREQ
     *     [���]: ִ�гɹ�ʱ��
     *             +CCBRREQ: <cbr-pssch>[,<cbr-pscch>]
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <cbr-pssch>: PC5��V2Xͨ��PSSCH�ŵ�æ�ʡ�
     *             ����ֵ��ȡֵ��Χ��1-100
     *             0��ʾCBR=0.0��
     *             1��ʾCBR=0.01��
     *             2��ʾCBR=0.02��
     *             ����
     *             100��ʾCBR=1.00
     *     <cbr-pscch>: PC5��V2Xͨ��PSCCH�ŵ�æ�ʡ�
     *             ����ֵ,ȡֵ��Χ��1-100
     *             0��ʾCBR=0.0��
     *             1��ʾCBR=0.01��
     *             2��ʾCBR=0.02��
     *             ����
     *             100��ʾCBR=1.00
     *             ������3GPP TS 36.214������5.1.30��ָ���ķ�������Դ���д���PSSCH��PSCCHʱ��UE�ŷ��ش˲���
     * [ʾ��]:
     *     �� �ŵ�æ������
     *       AT+CCBRREQ
     *       +CCBRREQ: 7
     *       OK
     */
    { AT_CMD_CCBRREQ,
      AT_SetCcbrreq, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CCBRREQ", VOS_NULL_PTR },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: PC5��V2X���ݴ���
     * [˵��]: ���������ڴ���UE��ʼ��ֹͣ��PC5�Ϸ���V2X���ݣ����ݸ�ʽ������3GPP TS 36.213�����������ڲ���ģʽ��������²���ʹ�á�
     * [�﷨]:
     *     [����]: +CV2XDTS=<action>[,<data_size>,<periodicity>]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: +CV2XDTS?
     *     [���]: ִ�гɹ�ʱ��
     *             +CV2XDTS: <action>[,<data_size>,<periodicity>]
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: +CV2XDTS=?
     *     [���]: +CV2XDTS: (list of supported <action>s)[,(maximum supported <data_size>),(minimum supported <periodicity>)]
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <action>: UE��ʼ��ֹͣ��PC5�Ϸ������ݣ�ȡֵ��Χ0~1��Ĭ��ֵ0��
     *             0��ʾֹͣ�������ݣ�1��ʾ��ʼ��������
     *     <data_size>: ָʾҪ���͵������ֽ�����ȡֵ��Χ0~8176
     *     <periodicity>: UE����ָ���ֽ�data_size��Ҫ��ʱ�䣬��λms��ȡֵ��Χ10~10000��ע�������20MB������С����Ϊ10ms�������10MB����ʵ����С����Ϊ20ms
     * [ʾ��]:
     *     �� ���ÿ�ʼ����V2X���ݣ�100ms����100�ֽ�
     *       AT+CV2XDTS=1,100,100
     *       OK
     *     �� ��ѯV2X���ݷ���״̬����ǰ���ڿ�ʼ����״̬��100�ֽ�/100ms��
     *       AT+CV2XDTS?
     *       +CV2XDTS: 1,100,100
     *       OK
     */
    { AT_CMD_CV2XDTS,
      AT_SetCv2xdts, AT_SET_PARA_TIME, AT_QryCv2xdts, AT_QRY_PARA_TIME, AT_TestCv2xdts, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CV2XDTS", (VOS_UINT8 *)"(0,1),(0-8176),(10-10000)" },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ͬ��Դ��ѯ
     * [˵��]: ���������ڻ�ȡ��ǰͬ��Դ��Ϣ��֧�������ϱ���
     * [�﷨]:
     *     [����]: ^VSYNCSRC?
     *     [���]: ִ�гɹ�ʱ��
     *             ^VSYNCSRC: <source>,<state>,<earfcn>,<slss_id>,<subslss_id>
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <source>: ͬ��Դ��ȡֵ��Χ0~3��
     *             0��ʾGNSS��1��ʾeNodeB��2��ʾ�ο�UE��3��ʾ��ͬ����ע��Ŀǰ�ݲ�֧��eNodeB��Ϊͬ��Դ��
     *     <state>: ͬ��״̬��ȡֵ��Χ0~1��
     *             0��ʾͬ����1��ʾʧ��
     *     <earfcn>: ��ǰͬ��Դ����Ƶ��
     *     <slss_id>: ȡֵ��Χ��0-335
     *     <subslss_id>: ȡֵ��Χ��0-65535
     * [ʾ��]:
     *     �� ��ѯͬ��Դ
     *       AT^VSYNCSRC?
     *       ^VSYNCSRC: 0,0,10,200,255
     *       OK
     */
    { AT_CMD_VSYNCSRC,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryVsyncsrc, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VSYNCSRC", VOS_NULL_PTR },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ����ͬ��Դ�ϱ�
     * [˵��]: ��������������ͬ��Դ�仯�ϱ���
     * [�﷨]:
     *     [����]: ^VSYNCSRCRPT=<status>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^VSYNCSRCRPT?
     *     [���]: ִ�гɹ�ʱ��
     *             ^VSYNCSRCRPT: <status>
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^VSYNCSRCRPT=?
     *     [���]: ^VSYNCSRCRPT: (0,1)
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <status>: ʹ��״̬��ȡֵ��Χ0~1��
     *             0��ʾȥʹ�ܣ�1��ʾʹ��
     * [ʾ��]:
     *     �� ʹ��ͬ��Դ�ϱ�
     *       AT^VSYNCSRCRPT=1
     *       OK
     */
    { AT_CMD_VSYNCSRCRPT,
      AT_SetVsyncsrcrpt, AT_SET_PARA_TIME, AT_QryVsyncsrcrpt, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VSYNCSRCRPT", (VOS_UINT8 *)"(0,1)" },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ����ͬ��ģʽ
     * [˵��]: ��������������ͬ��ģʽ��
     * [�﷨]:
     *     [����]: ^VSYNCMODE=<mode>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^VSYNCMODE?
     *     [���]: ִ�гɹ�ʱ��
     *             ^VSYNCMODE: <mode>
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^VSYNCMODE=?
     *     [���]: ^VSYNCMODE: (0,1)
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ͬ��ģʽ��ȡֵ��Χ0~1��
     *             0��ʾ��GNSS��1��ʾGNSS+�ο�UE+��ͬ��
     * [ʾ��]:
     *     �� ����ͬ��ģʽΪGNSS+�ο�UE+��ͬ��
     *       AT^VSYNCMODE=1
     *       OK
     */
    { AT_CMD_VSYNCMODE,
      AT_SetVsyncmode, AT_SET_PARA_TIME, AT_QryVsyncmode, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VSYNCMODE", (VOS_UINT8 *)"(0,1)" },
    /*
     * [���]: Э��AT-LTE-V���
     * [����]: �ŵ�æ������
     * [˵��]: ��������������PC5�ϲ�������V2Xͨ���ŵ�æ�ʣ�֧�������ϱ���
     * [�﷨]:
     *     [����]: ^CBR?
     *     [���]: ִ�гɹ�ʱ��
     *             ^CBR: <cbr-pssch>[,<cbr-pscch>]
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <cbr-pssch>: PC5��V2Xͨ��PSSCH�ŵ�æ�ʡ�
     *             ����ֵ��ȡֵ��Χ��1-100
     *             0��ʾCBR=0.0��
     *             1��ʾCBR=0.01��
     *             2��ʾCBR=0.02��
     *             ����
     *             100��ʾCBR=1.00
     *     <cbr-pscch>: PC5��V2Xͨ��PSCCH�ŵ�æ�ʡ�
     *             ����ֵ,ȡֵ��Χ��1-100
     *             0��ʾCBR=0.0��
     *             1��ʾCBR=0.01��
     *             2��ʾCBR=0.02��
     *             ����
     *             100��ʾCBR=1.00
     *             ������3GPP TS 36.214������5.1.30��ָ���ķ�������Դ���д���PSSCH��PSCCHʱ��UE�ŷ��ش˲�����
     * [ʾ��]:
     *     �� �����ŵ�æ��
     *       AT^CBR?
     *       ^CBR: 7
     *       OK
     */
    { AT_CMD_CBR,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryCbr, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CBR", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-LTE-V���
     * [����]: װ������GNSS��������
     * [˵��]: ����������װ�������������GNSS���ԡ�
     * [�﷨]:
     *     [����]: ^GNSSTEST
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <NA>: NA
     * [ʾ��]:
     *     �� ��������
     *       AT^GNSSTEST
     *       OK
     */
    { AT_CMD_GNSSTEST,
      AT_SetGnssInfo, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^GNSSTEST", TAF_NULL_PTR },

    /*
     * [���]: Э��AT-LTE-V���
     * [����]: װ����ѯͬ��״̬
     * [˵��]: ����������װ����ѯPC5��ͬ��״̬��
     * [�﷨]:
     *     [����]: ^QUERYDATA?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^QUERYDATA: <1ppsStat>,<SyncStat>,<SatNumStat><CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <1ppsStat>: 1PPS��״̬��1��������0��������  (������^GNSSTEST��ȡ1ppsͬ��״̬)
     *     <SyncStat>: �����ź�������1���ţ�0����
     *     <SatNum>: ��������
     * [ʾ��]:
     *     �� ��ѯ����
     *       AT^QUERYDATA?
     *       ^QUERYDATA: 1,1,1
     *       OK
     */
    { AT_CMD_QRYDATA,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryData, AT_QRY_PARA_TIME, AT_TestQryData, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^QUERYDATA", (VOS_UINT8 *)"(0,1),(0,1),(0-50)" },

    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ��ѯGNSS��Ϣ
     * [˵��]: ���������ڲ�ѯGNSS��Ϣ��
     * [�﷨]:
     *     [����]: ^GNSSINFO
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^GNSSINFO: <CR><LF><longitude>,<latitude><CR><LF><SatNum><CR><LF>No<i>:<GnssSystemType>        <CN0><CR><LF>[����]
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <longitude>: ����
     *     <latitude>: ά��
     *     <SatNum>: ��������
     *     <i>: ��ʾ���
     *     <GnssSystemType>: ����ϵͳ���ͣ�
     *             0��GPS
     *             1��Galileo
     *             2��Glonass
     *             3��QZSS
     *             4��SBAS
     *             5��BDS
     *     <CN0>: CN0ֵ
     * [ʾ��]:
     *     �� ��ѯGNSS��Ϣ����
     *       AT^GNSSINFO
     *       ^GNSSINFO��
     *       108.835647,34.201000
     *       3
     *       No0:0      45
     *       No1:0      41
     *       No2:0      57
     *       OK
     */
    { AT_CMD_GNSSINFO,
      AT_GnssInfo, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^GNSSINFO", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ��ѯLTE-V������շ�ͳ��
     * [˵��]: ���������ڲ�ѯLTE-V������շ�ͳ�ơ�
     * [�﷨]:
     *     [����]: ^VPHYSTAT?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^VPHYSTAT: <FirstSADecSucSum>,<SecSADecSucSum>,<TwiSADecSucSum>,
     *             <UplinkPackSum>,<DownlinkPackSum>,<PhyUplinkPackSum>
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <FirstSADecSucSum>: SA��һ�δ����������Ŀ
     *     <SecSADecSucSum>: SA�ڶ��δ����������Ŀ
     *     <TwiSADecSucSum>: SA���δ��䶼��������Ŀ
     *     <UplinkPackSum>: ���з�����
     *     <DownlinkPackSum>: �����հ���
     *     <PhyUplinkPackSum>: ��������з�����
     * [ʾ��]:
     *     �� ��ѯLTE-V������շ�ͳ��
     *       AT^VPHYSTAT?
     *       ^VPHYSTAT: 1,2,2,2,2,XXXXX
     *       OK
     */
    { AT_CMD_VPHYSTAT,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryVPhyStat, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VPHYSTAT", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ���LTE-V������շ�ͳ��
     * [˵��]: �������������LTE-V������շ�ͳ�ơ�
     * [�﷨]:
     *     [����]: ^VPHYSTATCLR=0
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <NA>: NA
     * [ʾ��]:
     *     �� ���LTE-V������շ�ͳ��
     *       AT^VPHYSTATCLR=0
     *       OK
     */
    { AT_CMD_VPHYSTATCLR,
      AT_SetVPhyStatClr, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VPHYSTATCLR", (VOS_UINT8 *)"(0)" },

    /*
     * [���]: Э��AT-LTE-V���
     * [����]: LTE-V PC5��SINR��RSRP��ѯ����
     * [˵��]: ����������LTE-V PC5��SINR��RSRP��ѯ��
     * [�﷨]:
     *     [����]: ^VSNRRSRP?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^VSNRRSRP:<MsgNum><CR><LF>
     *             ^VSNRRSRP: <SAMainSNR>      <SADiversitySNR>
     *             <DAMainSNR>      <DADiversitySNR>      <DARSRP>
     *             <DAMainMaxRsrp>      <DADiversityMaxRsrp>[<CR><LF>����]
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <MsgNum>: ����ֵ��
     *             ���ؽ����
     *     <SAMainSNR>: ����ֵ���������ռ5���ַ�λ������λ���������ţ�С��5ʱ�ÿո�������롣
     *             SA����SINR
     *     <SADiversitySNR>: ����ֵ���������ռ5���ַ�λ������λ���������ţ�С��5ʱ�ÿո�������롣
     *             SA�ּ�SINR
     *     <DAMainSNR>: ����ֵ���������ռ5���ַ�λ������λ���������ţ�С��5ʱ�ÿո�������롣
     *             DA����SINR
     *     <DADiversitySNR>: ����ֵ���������ռ5���ַ�λ������λ���������ţ�С��5ʱ�ÿո�������롣
     *             DA�ּ�SINR
     *     <DARSRP>: ����ֵ���������ռ5���ַ�λ������λ���������ţ�С��5ʱ�ÿո�������롣
     *             DA RSRP
     *     <DAMainMaxRsrp>: ����ֵ���������ռ5���ַ�λ������λ���������ţ�С��5ʱ�ÿո�������롣
     *             ����PSSCH RSRP���ֵ������ĿǰΪ200ms
     *     <DADiversityMaxRsrp>: ����ֵ���������ռ5���ַ�λ������λ���������ţ�С��5ʱ�ÿո�������롣
     *             �ּ�PSSCH RSRP���ֵ������ĿǰΪ200ms
     * [ʾ��]:
     *     �� ��ѯPC5��SINR��RSRP
     *       AT^VSNRRSRP?
     *       ^VSNRRSRP: 1
     *       ^VSNRRSRP:   -56        -11         15         24        -93       -126       -126
     *       OK
     */
    { AT_CMD_VSNRRSRP,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryVSnrRsrp, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VSNRRSRP", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ��ѯLTE-V PC5�����ּ�RSSI����
     * [˵��]: ����������LTE-V PC5�����ּ�RSSI�����ѯ��
     * [�﷨]:
     *     [����]: ^V2XRSSI?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^V2XRSSI: <DAMainRSSIMax >
     *             <CR><LF><DADiversityRSSIMax >
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <DAMainRSSIMax>: DA�������RSSIֵ
     *     <DADiversityRSSIMax>: DA�ּ����RSSIֵ
     * [ʾ��]:
     *     �� ��ѯPC5�����ּ�RSSI
     *       AT^V2XRSSI?
     *       ^V2XRSSI: -80,-90
     *       OK
     */
    { AT_CMD_V2XRSSI,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryV2xRssi, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^V2XRSSI", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-LTE-V���
     * [����]: ���������������
     * [˵��]: �������������õ���LTE-V����������ʡ�
     *         ע��ֻ֧�����������֧�ֲ�ѯ�Ͳ��ԡ�
     *         ���Ҫ�������������ʣ����Խ���������200~240֮�伴�ɡ�
     * [�﷨]:
     *     [����]: ^VTXPOWER=<power>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <power>: ��С����Ϊ��-62dbm����������ã�30dbm��
     *             ��1/8dBmΪ���ȵ�����AT���Χ-496 ~240��
     *             �����������ù���Ϊ12.5dBm����ͨ��AT�����µ���ֵΪ100��
     * [ʾ��]:
     *     �� �����������Ϊ10dBm
     *       AT^VTXPOWER=80
     *       OK
     */
    { AT_CMD_VTXPOWER,
      AT_SetVTxPower, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VTXPOWER", (VOS_UINT8 *)"(@Power)" },
};

/* ע��AS LTEV AT����� */
VOS_UINT32 AT_RegisterLtevAsCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atLtevAsCmdTbl, sizeof(g_atLtevAsCmdTbl) / sizeof(g_atLtevAsCmdTbl[0]));
}
#endif

