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

#include "at_general_mm_cmd_tbl.h"
#include "at_general_mm_set_cmd_proc.h"
#include "at_general_mm_qry_cmd_proc.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_GENERAL_MM_CMD_TBL_C

static const AT_ParCmdElement g_atGeneralMmCmdTbl[] = {
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: TE��MT����
     * [˵��]: ����������TE��MT֮�����֡�TE������ֹͣʱ��ͨ���������һ����Ϣ��MT��MT���յ���Ϣ����ӦTE��
     * [�﷨]:
     *     [����]: ^HS=<id>,<action>
     *     [���]: <CR><LF>^HS: <id>,<protocol>,<is_offline>,<product_class>,<product_id>,[<software_id>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^HS=?
     *     [���]: <CR><LF>^HS: (list of supported <id>s),(list of supported <action>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <id>: �����ͣ�8λ���֣���MT�ϱ�����ʶ��MT�ı�ʶ��TE��һ���·�ʱ����id��Ϊ0��������id��ΪMT�ϱ���ֵ��
     *     <action>: �����ͣ���ʾTE�������ͣ�ȡֵ0��1��
     *             0��TE����Living״̬�����TE��һ�η������ʾ��������
     *             1��TEֹͣ��
     *     <protocol>: �����ͣ���ʾ���嵱ǰ������ͨ��Э��״̬��ȡֵ0��1��
     *             0������������APP��
     *             1������������BOOT��
     *     <is_offline>: �����ͣ���ǰ״̬��ȡֵ0��2��
     *             0��online״̬��
     *             1��offline״̬��
     *             2��LPM���͹���״̬����
     *     <product_class>: �����ͣ���Ʒ���͡�
     *             0��GW��
     *     <product_id>: �����ͣ���Ʒ��ID��ÿһ���Ʒ��Ϊ�������һ��Ψһ��ID��
     *     <software_id>: �����ͣ�������
     * [ʾ��]:
     *     �� ��������
     *       AT^HS=0,0
     *       ^HS: 12345678,0,1,0,55
     *       OK
     */
    { AT_CMD_HS,
      At_SetHSPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HS", (VOS_UINT8 *)"(0-99999999),(0,1)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯλ����Ϣ
     * [˵��]: ������������ѯ��ǰפ�������<PLMN>��<LAC>��<RAC>��<CELL ID>��
     * [�﷨]:
     *     [����]: ^LOCINFO?
     *     [���]: <CR><LF>^LOCINFO: <PLMN>,<LAC>,<RAC>,<CELL ID><CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <PLMN>: PLMN��Ϣ������������ַ�������ǰ��λ��MCC������λ����λ��MNC��
     *     <LAC>: ���Ϊ3bytes��16���������ַ������ͣ�λ������Ϣ�����磺��0xC3����ʾ10���Ƶ�195��
     *             ע��פ��LTE/NR�´˲�����ʾTAC��λ������Ϣ����ʽͬLAC����
     *     <RAC>: ���Ϊ2bytes��16���������ַ������ͣ�·��������Ϣ�����磺��0xC3����ʾ10���Ƶ�195��
     *     <CELL ID>: 8bytes��16���������ַ������ͣ�С����Ϣ������ͬ<LAC>��ʹ��Half-Byte�루���ֽڣ����룬����0x100000005����ʾ��4�ֽ���0x00000001����4�ֽ���0x00000005��
     * [ʾ��]:
     *     �� ��ѯUE��ǰλ����Ϣ
     *       AT^LOCINFO?
     *       ^LOCINFO:24005,0xC3,0xC3,0x5
     *       OK
     *       ��ǰNR�£���ѯUE��ǰλ����Ϣ
     *          AT^LOCINFO?
     *          ^LOCINFO:24005,0xC3,0xC3,0x100000005
     *          OK
     */
    { AT_CMD_LOCINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryLocinfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^LOCINFO", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ����OPLMN�б�����
     * [˵��]: �������������úͲ�ѯ����OPLMN�б����õ�OPLMN�б�����NV���У���SIM���е�OPLMN�б��⣬������OPLMN�б�����ȼ�����SIM���е�OPLMN�б�
     *         ����OPLMN����ʹ����NV���е����Կ��غ�ʹ��IMSI�б���ơ�������Ϊ�������󣬵�ǰ���֧��Ԥ��256��OPLMN��Ϣ����Ҫ��6�����ã�ÿ�������������50��OPLMN��
     *         ������1�������ҽ���length����Ϊ0��pdu����Ϊ��ʱ�����ʾ��Ҫ������õ�OPLMN���ݡ�
     *         CLģʽ�¸����֧�֡�
     * [�﷨]:
     *     [����]: ^EOPLMN=<version>,<index>,<length>,<pdu>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^EOPLMN?
     *     [���]: <CR><LF>^EOPLMN: <version>,<length>,<pdu><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^EOPLMN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <version>: �ַ������ͣ��������õĳ���Ϊ0��7����8λΪ�ַ�������������ʾ��ǰ���õ�OPLMN�汾�š�
     *     <index>: ����ֵ����ʾ���õ�������ţ�ȡֵ��ΧΪ0~5��
     *             0����1��
     *             1����2��
     *             2����3��
     *             3����4��
     *             4����5��
     *             5����6��
     *     <length>: ����ֵ����ʾPDU���ȣ�ȡֵ��ΧΪ[0,500]��
     *     <pdu>: �ַ������ͣ�����Ϊ[0,500]�������뼼����OPLMN��ÿ��OPLMN��10���ֽڱ�ʾ��
     * [ʾ��]:
     *     �� ��������OPLMN�б�
     *       AT^EOPLMN="1.0",0,10,"64F0008080"
     *       OK
     *     �� ��ѯ����OPLMN�б�
     *       AT^EOPLMN?
     *       ^EOPLMN: "1.0",10,"64F0000808"
     *       OK
     *     �� ��������OPLMN �б�
     *       AT^EOPLMN=?
     *       OK
     */
    { AT_CMD_EOPLMN,
      AT_SetUserCfgOPlmnPara, AT_SET_PARA_TIME, AT_QryUserCfgOPlmnPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^EOPLMN", (VOS_UINT8 *)"(@version),(0-5),(0-500),(@PDU_DATA)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ���õ�ǰUE֧�ֵ�ģʽ
     * [˵��]: ���������������Լ���ѯ��ǰUE֧�ֵ�ģʽ��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +WS46=<n>
     *     [���]: ���������
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: +WS46?
     *     [���]: <CR><LF><n><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: +WS46=?
     *     [���]: <CR><LF> (list of supported <n>s) <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <+WS46=<n>������<n>>: <n>:integer type
     *             12: 3GPP System
     *             22,25,28,29,30,31������12���д���
     *     <+WS46=?��\n����ֵ<n>>: 12��GSM Digital Cellular Systems (GERAN only)
     *             22��UTRAN only
     *             25��3GPP Systems (GERAN, UTRAN and E-UTRAN)
     *             28��E-UTRAN only
     *             29��GERAN and UTRAN
     *             30��GERAN and E-UTRAN
     *             31��UTRAN and E-UTRAN
     * [ʾ��]:
     *     �� ��������
     *       AT+WS46=12
     *       OK
     *     �� ��ѯ����
     *       AT+WS46?
     *       25
     *       OK
     *     �� ��������
     *       AT+WS46=?
     *       (12,22,25,28,29,30,31)
     *       OK
     */
    { AT_CMD_WS46,
      AT_SetWs46Para, AT_SET_PARA_TIME, AT_QryWs46Para, AT_QRY_PARA_TIME, AT_TestWs46Para, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+WS46", (VOS_UINT8 *)"(12,22,25,28,29,30,31)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ָʾRSSI�仯
     * [˵��]: ��RSSI �仯����NdB��N������Ϊ1-5�����������ϴ��ϱ���ʱ������С��M(M������Ϊ1-20s)ʱ��MT�����ϱ�ָʾ��TE��
     *         ��AT����������ϱ���^CURC���ơ�
     * [�﷨]:
     *     [����]: ^CERSSI=<n>,<m>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CERSSI?
     *     [���]: �ֻ����ԣ�
     *             <CR><LF>^CERSSI: <n>,<m>,<RSSI>,<RSCP>,<EC/IO>,
     *             <RSRP>,<RSRQ>,<RSSNR>,<RI>,<CQI0>,<CQI1>,<5GRSRP>,<5GRSRQ>,<5GRSSNR><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             MBB���ԣ�
     *             <CR><LF>^CERSSI: <n>,<m>,<RSSI>,<RSCP>,<EC/IO>,<RSRP>,<RSRQ>,<RSSNR>,<RI>,<CQI0>,<CQI1>,<RXANTNUM >,
     *             <RSRPRX0>,<RSRPRX1>,<RSRPRX2>,<RSRPRX3>,<SINRRX0>,<SINRRX1>,<SINRRX2>,<SINRRX3>,<5GRSRP>,<5GRSRQ>,<5GRSSNR><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *             ע�⣺
     *             1���ڿ�����û��פ��ʱ��ʹ�ø������ѯ��MBBģʽ�õ�<EC/IO>,<RSRP>,<RSRQ>,<RSSNR>,<RI>[,<CQI0>][,<CQI1>],<rxANTNum>,<rsrpRx0>,<rsrpRx1>,<rsrpRx2>,<rsrpRx3>,<sinrRx0>,<sinrRx1>,<sinrRx2>,<sinrRx3>,<s5GRsrp>,<s5GRsrq>,<l5GSinr>����Ϊ��Чֵ��255, 0, 0, 0, 0, 32639, 32639, 0, 99, 99, 99, 99, 99, 99, 99, 99, 0, 0, 0
     *             2��ANDROIDģʽ�õ�<EC/IO>,<RSRP>,<RSRQ>,<RSSNR>,<RI>[,<CQI0>][,<CQI1>],<s5GRsrp>,<s5GRsrq>,<l5GSinr>����Ϊ��Чֵ��255, 0, 0, 0, 0, 32639, 32639, 0, 0, 0
     *             3��ֻ����פ��״̬�²�ѯ��������ص��ź�ǿ�Ȳű�֤��ȷ��
     *     [����]: ^CERSSI=?
     *     [���]: <CR><LF>^CERSSI: (list of supported <n>s), (list of supported<m>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: -
     *     [���]: 1��NV_System_APP_Config����Ϊ��ANDROID��MBB��ģʽ: <CR><LF>^CERSSI: <RSSI>,<RSCP>,<EC/IO>,<RSRP>,<RSRQ>,<RSSNR>,<RI>,<CQI0>,<CQI1>,<RXANTNUM >,<RSRPRX0>,<RSRPRX1>,<RSRPRX2>,<RSRPRX3>,<SINRRX0>,<SINRRX1>,<SINRRX2>,<SINRRX3>,<5GRSRP>,<5GRSRQ>,<5GRSSNR><CR><LF>
     *             2��NV_System_APP_Config����ΪANDROIDģʽ:
     *             <CR><LF>^CERSSI: <RSSI>,<RSCP>,<EC/IO>,<RSRP>,<RSRQ>,<RSSNR>,<RI>,<CQI0>,<CQI1>,<5GRSRP>,<5GRSRQ>,<5GRSSNR><CR><LF>
     * [����]:
     *     <n>: 0���������ϱ���չ���ź�����,��<n>Ϊ0ʱ��<m>������Ч��Ĭ��Ϊ0��
     *             1~5�����ź������仯����ndBm�������ϱ���չ���ź�����������ģʽ
     *             �л�ʱ��ֻ��פ��С����Ž��������ϱ���
     *     <m>: 0���ϱ���չ�ź�������ʱ�����ƣ�
     *             1-20�������ϱ��ź���������С���ʱ�䣬��λ��S��
     *             ��ѡ����������дʱĬ��Ϊ0��
     *     <RSSI>: UEפ��2Gʱ����С�����ź�������פ����2GС��ʱ������Ϊ0��
     *     <RSCP>: UEפ��3Gʱ����С�����ź�������פ����3GС��ʱ������Ϊ0��
     *     <EC/IO>: UEפ��3G FDDС��ʱ�ķ���С������ȣ�פ����3G FDDС��ʱ������Ϊ255��
     *     <RSRP>: UEפ��4Gʱ����С���ο��źŽ��չ��ʣ���Чֵ99������Ϊ��ֵ��פ����4GС��ʱ������Ϊ0��
     *     <RSRQ>: UEפ��4Gʱ����С���ο��źŽ�����������Чֵ99������Ϊ��ֵ��פ����4GС��ʱ������Ϊ0��
     *     <RSSNR>: UEפ��4Gʱ����С����Ч��פ����4GС��ʱ������Ϊ0��
     *             ��Ӧ������SINR��ȡֵ��Χ[-10,40]����ЧֵΪ99��
     *     <RI>: RIָʾΪ1ʱ���ϱ�һ�����ֵ�CQI��������CQI0�У�����һ�����ֶ�Ӧ��CQI�ϱ���Чֵ����Чֵ��Ϊ0x7F7F����������CQI1�С�
     *             RIָʾΪ2ʱ���ϱ��������ֵ�CQI���ֱ�ΪCQI0��CQI1��
     *             UEפ����4Gʱ��RIָʾΪ0������CQI0��CQI1������
     *             [CQI0]��UEפ��4Gʱ����С����������ָʾ��
     *             [CQI1]��UEפ��4Gʱ����С����������ָʾ��
     *     <5GRSRP>: UE פ��5Gʱ����С���ο��źŽ��չ��ʣ���Χ��(-141,-44)����Чֵ99��פ����5GС��ʱ������Ϊ0��
     *     <5GRSRQ>: UE פ��5Gʱ����С���ο��źŽ�����������Χ��(-40, -6) ����Чֵ99��פ����5GС��ʱ������Ϊ0��
     *     <5GRSSNR>: UE פ��5Gʱ����С����Ч��פ����5GС��ʱ������Ϊ0����Ӧ������SINR��ȡֵ��Χ[-10,40]����ЧֵΪ99��
     *     <RXANTNUM>: ���ߵ����������Ϊ4����
     *     <RSRPRX>: RSRPRX0��UE פ��4G ʱ����0�Ĳο��źŽ��չ��ʣ�פ����4G С��ʱ������Ϊ99��
     *             RSRPRX1��UEפ��4G ʱ����1�Ĳο��źŽ��չ��ʣ�פ����4G С��ʱ������Ϊ99��
     *             RSRPRX2��UEפ��4G ʱ����2�Ĳο��źŽ��չ��ʣ�פ����4G С��ʱ������Ϊ99��
     *             RSRPRX3��UEפ��4G ʱ����3�Ĳο��źŽ��չ��ʣ�פ����4G С��ʱ������Ϊ99��
     *     <SINRRX>: SINRRX0������0������ȣ�UEפ��4Gʱ����С����Ч��פ����4GС��ʱ������Ϊ99��
     *             SINRRX1������1������ȣ�UEפ��4Gʱ����С����Ч��פ����4GС��ʱ������Ϊ99��
     *             SINRRX2������2������ȣ�UEפ��4Gʱ����С����Ч��פ����4GС��ʱ������Ϊ99��
     *             SINRRX3������3������ȣ�UEפ��4Gʱ����С����Ч��פ����4GС��ʱ������Ϊ99��
     * [ʾ��]:
     *     �� פ�� 2G�������ź������ı䳬��5dB �����ϱ�
     *       AT^CERSSI=5
     *       OK
     *       ?6?1 �ź������ı䳬�� 5dB�������ϱ�
     *       ^CERSSI: -42,0,255,0,0,0,0,32639,32639,0,0,0
     *       OK
     *     �� ?6?1 ��ѯ����
     *       AT^CERSSI?
     *       ^CERSSI: 5,0,-42,0,255,0,0,0,0,32639,32639,0,0,0
     *       OK
     *     �� פ�� 3G�������ź������ı䳬��3dB ���Ҽ��ʱ�䳬��5s�������ϱ�
     *       AT^CERSSI=3,5
     *       OK
     *       ?6?1 �ź������ı䳬�� 3dB ���Ҿ����ϴ��ϱ�����5s�������ϱ�
     *       ^CERSSI: 0,-68,-6,0,0,0,0,32639,32639,0,0,0
     *       OK
     *     �� ?6?1 ��ѯ����
     *       AT^CERSSI?
     *       ^CERSSI: 3,5,0,-68,-6,0,0,0,0,32639,32639,0,0,0
     *       OK
     *     �� פ�� 4G�������ź������ı䳬��4dB ���Ҽ��ʱ�䳬��5s�������ϱ�
     *       AT^CERSSI=4,5
     *       OK
     *       ?6?1 �ź������ı䳬�� 4dB ���Ҽ��ʱ�䳬��5s�������ϱ�
     *       ^CERSSI: 0,0,255,-61,-42,5,2,2,0,0,0,0
     *       OK
     *     �� ?6?1 ��ѯ����
     *       AT^CERSSI?
     *       ^CERSSI: 4,5,0,0,255,-61,-42,5,2,2,0,0,0,0
     *       OK
     *     �� פ�� 5G�������ź������ı䳬��4dB ���Ҽ��ʱ�䳬��5s�������ϱ�
     *       AT^CERSSI=4,5
     *       OK
     *       ?6?1 �ź������ı䳬�� 4dB ���Ҽ��ʱ�䳬��5s�������ϱ�
     *       ^CERSSI: 0,0,255,0,0,0,0,32639,32639,-61,-42,5
     *       OK
     *     �� ?6?1 ��ѯ����
     *       AT^CERSSI?
     *       ^CERSSI: 4,5,0,0,255,0,0,0,0,32639,32639,-61,-42,5
     *       OK
     *     �� ִ�в�������
     *       AT^CERSSI=?
     *       ^CERSSI: (0,1,2,3,4,5),(0-20)
     *       OK
     */
    { AT_CMD_CERSSI,
      At_SetCerssiPara, AT_SET_PARA_TIME, At_QryCerssiPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CERSSI", (VOS_UINT8 *)"(0,1,2,3,4,5),(0-20)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: ָʾNSAģʽ�¸����� RSSI �仯
     * [˵��]: ��NSA����£��������ӵ�RSSI �仯����NdB��N ������Ϊ1-5�����������ϴ��ϱ���ʱ������С��M(M������Ϊ1-20s)ʱ��MT �����ϱ�ָʾ��TE��
     *         ���������֧��NR������²�֧���ϱ��Ͳ�ѯ�������ѯ�ظ�ʧ�ܡ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CSERSSI: <RSSI>,<RSCP>,<EC/IO>,<RSRP>,<RSRQ>,<RSSNR>,<RI>,<CQI0>,<CQI1>,<5GRSRP>,<5GRSRQ
     *             >,<5GRSSNR><CR><LF>
     *     [����]: ^CSERSSI?
     *     [���]: <CR><LF>^CSERSSI: <n>,<m>,<RSSI>,<RSCP>,<EC/IO>,<RSRP>,<RSRQ>,<RSSNR>,<RI>,<CQI0>,<CQI1>,<5GRSRP>,<5GRSRQ>,<5GRSSNR><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *             ע�⣺�ڿ�����û��פ��ʱ��ʹ�ø������ѯ���õ���
     *             <RSSI>,<RSCP>,<EC/IO>,<RSRP>,<RSRQ>,<RSSNR>,<RI>
     *             , <CQI0>,<CQI1>,<5GRSRP>,<5GRSRQ>,<5GRSSNR>����Ϊ��Чֵ��0,0,255,0,0,0,0,32639,32639,0,0,0
     *             ֻ����פ��״̬�²�ѯ��������ص��ź�ǿ�Ȳű�֤��ȷ
     * [����]:
     *     <n>: 0���������ϱ���չ���ź�����,��<n>Ϊ0 ʱ��<m>������Ч��Ĭ��Ϊ
     *             0��
     *             1~5�����ź������仯����ndBm�������ϱ���չ���ź�����������ģʽ
     *             �л�ʱ��ֻ��פ��С����Ž��������ϱ���
     *     <m>: 0���ϱ���չ�ź�������ʱ�����ƣ�
     *             1-20�������ϱ��ź���������С���ʱ�䣬��λ��S��
     *             ��ѡ����������дʱĬ��Ϊ0��
     *     <RSSI>: UE פ��2G ʱ����С�����ź�������פ����2G С��ʱ������Ϊ0��
     *     <RSCP>: UE פ��3G ʱ����С�����ź�������פ����3G С��ʱ������Ϊ0��
     *     <EC/IO>: UE פ��3G FDD С��ʱ�ķ���С������ȣ�פ����3G FDD С��ʱ��
     *             ����Ϊ255��
     *     <RSRP>: UE פ��4G ʱ����С������ȣ���Чֵ99������Ϊ��ֵ��פ����4G С
     *             ��ʱ������Ϊ0��
     *     <RSRQ>: UE פ��4G ʱ����С������ȣ���Чֵ99������Ϊ��ֵ��פ����4G С
     *             ��ʱ������Ϊ0��
     *     <RSSNR>: UE פ��4G ʱ����С����Ч��פ����4G С��ʱ������Ϊ0��
     *             ��Ӧ������SINR����ЧֵΪ99��
     *     <RI>: RI ָʾΪ1 ʱ���ϱ�һ�����ֵ�CQI��������CQI0 �У�����һ������
     *             ��Ӧ��CQI �ϱ���Чֵ����Чֵ��Ϊ0x7F7F����������CQI1 �С�
     *             RI ָʾΪ2 ʱ���ϱ��������ֵ�CQI���ֱ�ΪCQI0 ��CQI1��
     *             UE פ����4G ʱ��RI ָʾΪ0��CQI0��CQI1�ϱ���Чֵ����ЧֵΪ32639����
     *             [CQI0]��UE פ��4G ʱ����С����������ָʾ��
     *             [CQI1]��UE פ��4G ʱ����С����������ָʾ��
     *     <5GRSRP>: UE פ��5Gʱ����С������ȣ���Чֵ99������Ϊ��ֵ��פ����5GС��ʱ������Ϊ0��
     *     <5GRSRQ>: UE פ��5Gʱ����С������ȣ���Чֵ99������Ϊ��ֵ��פ����5GС��ʱ������Ϊ0��
     *     <5GRSSNR>: UE פ��5Gʱ����С����Ч��פ����5GС��ʱ������Ϊ0��
     *             ��Ӧ������SINR��ȡֵ��Χ[-10,40]����ЧֵΪ99��
     * [ʾ��]:
     *     �� ���������ϱ�����Ϊ�ź������ı䳬��4dB ���Ҽ��ʱ�䳬��5s
     *       AT^CERSSI=4,5
     *       AT^CSERSSI?
     *       ^CSERSSI: 4,5,0,0,255,0,0,0,0,32639,32639,0,0,0
     *       OK
     *     �� ��ѯ���פ�� 3G
     *       AT^CSERSSI?
     *       ^CSERSSI: 4,5,0,0,255,0,0,0,0,32639,32639,0,0,0
     *       OK
     *     �� ��ѯ���פ�� 4G��SAģʽ
     *       AT^CSERSSI?
     *       ^CSERSSI: 4,5,0,0,255,0,0,0,0,32639,32639,0,0,0
     *       OK
     *     �� ��ѯ���פ�� 5G��SAģʽ
     *       AT^CSERSSI?
     *       ^CSERSSI: 4,5,0,0,255,0,0,0,0,32639,32639,0,0,0
     *       OK
     *     �� ��ѯ���פ�� 4G��NSAģʽ
     *       AT^CSERSSI?
     *       ^CSERSSI: 4,5,0,0,255,0,0,0,0,32639,32639,-61,-42,5
     *       OK
     *       �����ϱ���פ�� 4G��NSAģʽ
     *       ?6?1 NR�ź������ı䳬�� 4dB ���Ҽ��ʱ�䳬��5s�������ϱ�
     *       ^CSERSSI: 0,0,255,0,0,0,0,32639,32639,-61,-42,5
     *       OK
     */
    { AT_CMD_CSERSSI,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryCserssiPara, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CSERSSI", VOS_NULL_PTR },
#endif
};

/* ע��LMMͨ��AT����� */
VOS_UINT32 AT_RegisterGeneralMmCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atGeneralMmCmdTbl, sizeof(g_atGeneralMmCmdTbl) / sizeof(g_atGeneralMmCmdTbl[0]));
}

