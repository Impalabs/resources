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
#include "at_device_phy_cmd_tbl.h"

#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"
#include "at_lte_common.h"

#include "at_device_phy_set_cmd_proc.h"
#include "at_device_phy_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_PAM_CMD_TBL_C

static const AT_ParCmdElement g_atDevicePhyCmdTbl[] = {
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯ��Ʒ��̬
     * [˵��]: ������������ѯ��Ʒ��̬��
     * [�﷨]:
     *     [����]: ^PRODTYPE?
     *     [���]: <CR><LF>^PRODTYPE: <result>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <result>: �ն˲�Ʒ��̬��ȡֵ��ΧΪ0��255��
     *             0�����ݿ���
     *             1��ģ�飻
     *             2��E5��
     *             3��CPE��
     *             �����������塣
     */
    { AT_CMD_PRODTYPE,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryProdTypePara, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PRODTYPE", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����ģ��RFIC_DIE_ID��ȡ
     * [˵��]: ���������ڶ�ȡ����ģ��RFIC_DIE_ID��
     *         BalongV7R22C60������
     *         �������ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^RFICID?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^RFICID: (list of supported<DIEID>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <DIEID>: �ַ������ͣ�RFIC_DIE_ID������16������� 32�ֽ�256bit
     *             �������������û�С�
     * [ʾ��]:
     *     �� ִ�в������� �ײ��ȡ������RFICID�����
     *       AT^RFICID?
     *       ^RFICID: 0, "0000000000000000000000000000000000000000000000000000000000000000"
     *       ^RFICID: 1, "0000000000000000000000000000000000000000000000000000000000000000"
     *       OK
     *     �� ִ�в������� �ײ��ȡ��һ��RFICID�����
     *       AT^RFICID?
     *       ^RFICID: 0, "0000000000000000000000000000000000000000000000000000000000000000"
     *       OK
     *     �� ִ�в������� �ײ�û�л�ȡ��RFICID�����
     *       AT^RFICID?
     *       OK
     */
    { AT_CMD_RFICID,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryRficDieIDPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RFICID", VOS_NULL_PTR },

    { AT_CMD_RFFEID,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryRffeDieIDPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RFFEID", VOS_NULL_PTR },

    { AT_CMD_LTCOMMCMD,
      AT_SetLTCommCmdPara, AT_SET_PARA_TIME, atQryLTCommCmdPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LTCOMMCMD", (VOS_UINT8 *)"(0-65535),(0-2000),(0-65535),(0-65535),(@data),(@data),(@data),(@data)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���÷������µĲ���
     * [˵��]: �ڷ������·���ָ�������źš�����Ʒ��֧��ֱ�ӷ��ش���Ƶ������ʹ��^FCHAN��
     *         ��������Ҫ��^FCHAN���ִ�С�
     *         ���������������Ĭ��Ϊ0��
     * [�﷨]:
     *     [����]: ^FWAVE=<type>,<amplitue_dbm_percent>[,<slot>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <type>: ����Ĳ�������:
     *             0: BPSK
     *             1: PI2_BPSK
     *             2: QPSK
     *             3: 16QAM
     *             4: 64QAM
     *             5: 256QAM
     *             6: GMSK
     *             7: 8PSK
     *             8: ������
     *             Gģ֧��6��7��8���ֲ��Σ�GģĬ�ϰ�1ʱ϶���ԣ���Gģ������ƣ�
     *             Wģ֧��2��8���ֲ��Σ�
     *             Cģ֧��2��8���ֲ��Σ�
     *             Tģ֧��8һ�ֲ��Σ�
     *             Lģ֧��2��3��4��5��8���ֲ��Σ�
     *             NRģ֧��0~5��8���ֲ��Σ�
     *             ĿǰNRֻҪ�����CP-OFDM���ԣ�
     *     <amplitue_dbm_percent>: ���䲨�εĹ��ʴ�С����0.01dBmΪ��λ
     *     <slot>: GSM�����źŵ�ʱ϶��
     *             0��8ʱ϶���䣻
     *             1��1ʱ϶���䣻
     *             2��2ʱ϶���䣻
     *             3��3ʱ϶���䣻
     *             4��4ʱ϶���䣻
     * [ʾ��]:
     *       �������²������óɹ�
     *     �� W:
     *       AT^FWAVE=0,100
     *       OK
     *     �� G:
     *       AT^FWAVE=0,1000
     *       OK
     */
    { AT_CMD_FWAVE,
      AT_SetFwavePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FWAVE", (VOS_UINT8 *)FWAVE_CMD_PARA_STRING },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���÷�������ŵ�
     * [˵��]: �ڵ���״̬������ĳһƵ��UL��DL�ŵ������ú󣬵����Զ��������õ��ŵ�ͬ��������Ӧ�����к������ŵ�������Ʒ��֧�ֿ�ֱ�ӷ���ERROR����ָ��һ������Ч����������������������Ӳ�����������������µ粻���棩��
     *         �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش����롣
     *         ���������������Ĭ��Ϊ0��
     *         ÿ���·����������Ὣԭ�е����������Ҳ����һ��������������֮ǰ��������Ϣ���������
     * [�﷨]:
     *     [����]: ^FCHAN=<mode>,<band>,<channel>,<band_width>,[<sub_carrier_space>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FCHAN?
     *     [���]: <CR><LF>^FCHAN: <mode>,<band>,<ul_channel>,<dl_channel>,<band_width>,[<sub_carrier_space>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^FCHAN=?
     *     [���]: <CR><LF>^FCHAN: (list of supported <mode>s),(list of supported <band>s),(list of supported < ul_channel >s) , (list of supported < dl_channel >s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ģʽ��
     *             0��WCDMA��
     *             2��TD-SCDMA(Ŀǰ��NR��оƬ��֧��TD)��
     *             3��GSM��
     *             4��EDGE��
     *             5��AWS��
     *             6��FDD LTE��
     *             7��TDD LTE��
     *             8��WIFI
     *             9��NR
     *             10��LTE-V
     *     <band>: Ƶ��Ƶ�ʡ�
     *             ������ǰGUCTLģ��band����ôת���ģ������ͳһ���հ�Э��band���ã�
     *             Gģ��Band��Ӧ��ϵ���£�
     *             GSM850��Ӧband5;
     *             GSM900��ӦBand8��
     *             GSM1800��Ӧband3;
     *             GSM1900��ӦBand2;
     *             ���<mode>=WIFI��Band, �������Ӧ����Ϊ15������ǰ���ݣ���
     *     <channel>: �ŵ��ţ�ȡֵ��ΧΪ0��4294967295��
     *             Xģȡֵ��ΧΪ[1,799]��[991,1323]��
     *             ������Щband���������ŵ��ǲ��ԳƵģ���������Ե��ֵ��ر�ע�⡣�����Ҫ���Բ��ԳƵ�band���ڴ�TX����RXǰ�赥�����������е��ŵ���
     *             ��LTE��Band: 66, 70;
     *             ��NR��Band: 66, 70, 75, 76;
     *     <ul_channel>: �����ŵ���������к�����һ��Ҳ��Ҫ�ϱ�����
     *     <dl_channel>: �����ŵ���������к�����һ��Ҳ��Ҫ�ϱ�����
     *     <band_width>: ����ֵ������������Ÿ�����ģ���ɲ�Ʒ����ʱ�Լ���������ģ�Ĵ�����
     *             0: 200K
     *             1: 1.2288M
     *             2: 1.28M
     *             3: 1.4M
     *             4: 3M
     *             5: 5M
     *             6: 10M
     *             7: 15M
     *             8: 20M
     *             9: 25M
     *             10: 30M
     *             11: 40M
     *             12: 50M
     *             13: 60M
     *             14: 80M
     *             15: 90M
     *             16: 100M
     *             17: 200M
     *             18: 400M
     *             19: 800M
     *             20: 1G
     *     <sub_carrier_space>: NR���ز������NR�ش���������
     *             0: 15K
     *             1: 30K
     *             2: 60K
     *             3: 120K
     *             4: 240K
     *             ע��NR sub6Gֻ֧��30K
     *     <err>: ������:
     *             0������ģʽ����
     *             1������Ƶ����Ϣ�޷���Ӧ��
     *             2�������ŵ���Ϣʧ�ܣ�
     *             3������Ƶ�κ��ŵ���Ϣ�޷���ϣ�
     *             4���������󣨰����������󣬲�����������ȷ�ȣ���
     *             5��WIFI��band���ô���
     *             6����֧��WIFI��
     *             7��WIFIû���ҿ���
     *             8������ģʽ����ȷ��
     *             9��band width����
     *             10�����ز������������
     *             11�����ͼ���DSP��Ϣʧ�ܣ�
     *             12������DSPʧ�ܣ�
     *             13: û������SCS����
     *             50����������
     *             536��band��channel ��ƥ�䣻
     * [ʾ��]:
     *     �� �����Ѿ����������ģʽ������band41
     *       AT^FCHAN=10,28,27210,8
     *       OK
     */
    { AT_CMD_FCHAN,
      At_SetFChanPara, AT_SET_PARA_TIME, At_QryFChanPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FCHAN", (VOS_UINT8 *)FCHAN_CMD_PARA_STRING },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ѡ����Ƶͨ·
     * [˵��]: �������������õ������Ƶͨ·���������һ����Ч������󲻱��档
     *         ���������������Ĭ��Ϊ0��
     * [�﷨]:
     *     [����]: ^TSELRF=<path>,<tx_or_rx>[,<mimo_type>,<group>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^TSELRF?
     *     [���]: <CR><LF>^TSELRF: <number><CR><LF>
     *             <CR><LF>^TSELRF: <path>
     *             <CR><LF>^TSELRF: <path><CR><LF>
     *             ����
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^TSELRF=?
     *     [���]: ��֧��5Gʱ��
     *             <CR><LF>^TSELRF: (list of supported <path>s), (list of supported <tx_or_rx>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ֧��5Gʱ��
     *             <CR><LF>^TSELRF: (list of supported <path>s), (list of supported <tx_or_rx>s), (list of supported <mimo_type>s), (list of supported <group>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <path>: �������Ƶͨ·��š�
     *             1��GSMͨ·������EDGE/GPRS/EGSM�ȵ�ȫ������
     *             2��WCDMA������
     *             3��WCDMA�ּ����ּ�����ʱ����ͨ·��ȻΪ������������ͨ·Ϊ�ּ�����
     *             6��TDͨ����
     *             7��WIFIͨ����
     *             8��WiMaxͨ·����Ʒ�߶���ֵ��HISI��֧�֣���
     *             9��FDD LTE������
     *             10��FDD LTE �ּ���
     *             11��FDD LTE MIMO��
     *             12��TDD LTE/LTE-V ������
     *             13��TDD LTE/LTE-V �ּ���
     *             14��TDD LTE/ MIMO��
     *             15��Navigationͨ·����Ʒ�߶���ֵ��HISI��֧�֣���
     *             16��NR ������
     *             17��NR �ּ���
     *             18��NR MIMO��
     *     <number>: ����֧�ֵ��ܵ�ͨ·����ͬһ��<path>�������<group>��ֻ��¼һ�Ρ�
     *     <group>: ����(�������ܲ���MIMO)����Χ��0~255��
     *     <mimo_type>: ��ʾMIMO���ͣ���Χ��1-3��
     *             1��2��TX����2��RX��
     *             2��4��RX��
     *             3��8��RX��
     *     <group>: ��ʾ�����Ǹ����ߣ�����<mimo_type>������ͬ��ȡֵ��Χ��ͬ��
     *             ��<mimo_type> = 1����ȡֵ���£�
     *             1����1�����ߣ�
     *             2����2�����ߣ�
     *             3��ͬʱ��������1������2����ֻ֧�ֵ�����
     *             ��<mimo_type> = 2����ȡֵ���£�
     *             1����1�����ߣ�
     *             2����2�����ߣ�
     *             3����3�����ߣ�
     *             4����4�����ߣ�
     *             ��<mimo_type> = 3����ȡֵ���£�
     *             1����1�����ߣ�
     *             2����2�����ߣ�
     *             3����3�����ߣ�
     *             4����4�����ߣ�
     *             5����5�����ߣ�
     *             6����6�����ߣ�
     *             7����7�����ߣ�
     *             8����8�����ߣ�
     *     <tx_or_rx>: ��ʾ��TX����RX��
     *             0����ʾTX��
     *             1����ʾRX��
     *     <err>: �������˵����
     *             0����ǰģʽ����
     *             1��Path ����
     *             2��TX ����RX ��������
     *             6����֧��WIFI��
     *             7��û�д�wifi;
     *             50����������
     * [ʾ��]:
     *     �� RXѡ����Ƶͨ·ΪNR MIMO(4R)�ĵ���������
     *       AT^TSELRF=18,1,2,3
     *       OK
     */
    { AT_CMD_TSELRF,
      AT_SetTSelRfPara, AT_SET_PARA_TIME, AT_QryTSelRfPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^TSELRF", (VOS_UINT8 *)TSELRF_CMD_PARA_STRING },

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �������߱ջ���г
     * [˵��]: ���������ڷ�����ģʽ�£��������߱ջ���г�Ƿ�ʹ�ܡ��������ֻ�������^FCHAN������MODEΪWCDMA��AWS��FDD LTE��TDD LTE��Ч��
     *         ע��
     *         1�����������Ҫ��^FTXONǰ�·���
     *         2�����������֧��NR�İ汾��������ʽ�ݲ�֧�֣��������ݲ�Ʒ������ʵ�֡�
     * [�﷨]:
     *     [����]: ^CLT=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^CLT=?
     *     [���]: <CR><LF>^CLT: (list of supported <enable>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ���ߵ�г�ջ��Ƿ�ʹ�ܣ�
     *             0���رձջ�
     *             1��ʹ�ܱջ�
     * [ʾ��]:
     *     �� ���ߵ�г�ջ�ʹ��
     *       AT^CLT=1
     *       OK
     *     �� ִ�в�������
     *       AT^CLT=?
     *       ^CLT: (0,1)
     *       OK
     */
    { AT_CMD_CLT,
      At_SetCltPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CLT", (VOS_UINT8 *)"(0,1)" },
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: CLT�����߱ջ���г��״̬��Ϣ��ѯ
     * [˵��]: ��ѯ��ǰMT��λCLT״̬��Ϣ(ֻ��WPHY��LPHY�Ż���״̬��Ϣ)��
     *         ע��
     *         1��CLT INFO��Ϣֻ֧�ֲ�ѯһ�Σ�ÿ����ѯ��modem��¼��CLT INFO��Ϣ�ͻ����������ٴβ�ѯ����ֻ����OK��ֱ����������µ���Ϣ��������
     *         2�����������֧��NR�İ汾��������ʽ�ݲ�֧�֣��������ݲ�Ʒ������ʵ�֡�
     * [�﷨]:
     *     [����]: ^CLTINFO?
     *     [���]: ����ǰmodem��¼��CLTINFO��Ϣ��
     *             <CR><LF>^CLTINTO: <GammaReal>, <GammaImag>, <GammaAmpUc0>, <GammaAmpUc1>, <GammaAmpUc2>, <GammaAntCoarseTune>, <FomcoarseTune>, <CltAlgState>, <CltDetectCount>, <Dac0>, <Dac1>, <Dac2>, <Dac3> <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��modemû�м�¼��CLTINFO��Ϣ��
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <GammaReal>: �з�������ֵ������ϵ��ʵ��
     *     <GammaImag>: �з�������ֵ������ϵ���鲿
     *     <GammaAmpUc0>: �޷�������ֵ��פ����ⳡ��0����ϵ������
     *     <GammaAmpUc1>: �޷�������ֵ��פ����ⳡ��1����ϵ������
     *     <GammaAmpUc2>: �޷�������ֵ��פ����ⳡ��2����ϵ������
     *     <GammaAntCoarseTune>: �޷�������ֵ���ֵ����λ��
     *     <FomcoarseTune>: �޷�������ֵ���ֵ�FOMֵ
     *     <CltAlgState>: �޷�������ֵ���ջ��㷨����״̬
     *     <CltDetectCount>: �޷�������ֵ���ջ������ܲ���
     *     <Dac0>: �޷�������ֵ��Dac0
     *     <Dac1>: �޷�������ֵ��Dac1
     *     <Dac2>: �޷�������ֵ��Dac2
     *     <Dac3>: �޷�������ֵ��Dac3
     * [ʾ��]:
     *     �� ��ѯ�ϱ�
     *       AT^CLTINFO?
     *       ^CLTINFO: 3150,0,4465,2214,3150,22536,185,17,3,106,11,0,0
     *       OK
     */
    { AT_CMD_CLTINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryCltInfo, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CLTINFO", VOS_NULL_PTR },
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    { AT_CMD_MIPIRD,
      AT_SetMipiRdPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^MIPIRD", (TAF_UINT8 *)"(0-9),(0-1),(0-15),(0-255)" },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �������´򿪷����
     * [˵��]: �ڷ������´򿪻�رշ����������Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     *         ע��
     *         1.     �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش�����0��
     *         2.     ��������Ҫ�����÷������ŵ���^FCHAN����ѡ����Ƶͨ����^TSELRF����ִ�С�
     *         3.     ���������������Ĭ��Ϊ0��
     *         4.     ����������ģʽ�����ʹ��At�����������У���Ҫ��ͨ��At^FRXON�����������У����������������̰�Tx Tuner���óɹ������������С�
     * [�﷨]:
     *     [����]: ^FTXON=<switch>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^FTXON?
     *     [���]: <CR><LF>^FTXON: <switch><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^FTXON=?
     *     [���]: <CR><LF>^FTXON: (list of supported <switch>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <switch>: ��/�رշ������֧��5GʱȡֵΪ0-1��
     *             0���رշ������
     *             1���򿪷�����������������ƺ�RF����ͨ����
     *             2���򿪷������ֻ��RFIC Transmitter��PA��
     *     <err>: �����롣
     *             0������ģʽ����
     *             1��û����������ŵ���
     *             2���򿪷����ʧ�ܣ�
     *             3����������
     */
    { AT_CMD_FTXON,
      At_SetFTxonPara, AT_SET_PARA_TIME, At_QryFTxonPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_FTXON_OTHER_ERR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FTXON", (VOS_UINT8 *)FTXON_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������˽��
     * [����]: ����DPDT���Կ���
     * [˵��]: ������ֻ���ڲ���ģʽ�����������㷨��/�رա�����������ģʽ����ָ�ԭ��Ĭ��̬�������µ��������塣
     *         ע����������ڴ�NR�İ汾��MT��λ��֧�֣�������BBIC����֧�������ATģ���ֱ�ӷ���OK��
     * [�﷨]:
     *     [����]: ^DPDTTEST=<RatMode>,<Flag>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR <err><CR><LF>
     *     [����]: ^DPDTTEST=?
     *     [���]: <CR><LF>^DPDTTEST: (list of supported <RatMode>s),(list of supported <Flag>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <RatMode>: ����ֵ������ģʽ��
     *             0��GSM��
     *             1��WCDMA��
     *             2��LTE��
     *             3��TDSCDMA��
     *             4��CDMA
     *             5��NR
     *     <Flag>: ����ֵ��DPDT���Կ��ء�
     *             0���ر�˫�����㷨��
     *             1����˫�����㷨��
     *             2���ָ�Ĭ��״̬��
     *             ע����ǰ��(0-2)��ȡֵ��Ч��Ϊ������չ��Flagȡֵ��Χ��Ϊ(0-65535)��
     * [ʾ��]:
     *     �� Gģ�´�˫�����㷨
     *       AT^DPDTTEST=0,1
     *       OK
     */
    { AT_CMD_DPDTTEST,
      At_SetDpdtTestFlagPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^DPDTTEST", (VOS_UINT8 *)DPDT_TEST_CMD_PATA_STRING },

    /*
     * [���]: Э��AT-������˽��
     * [����]: ����DPDT״̬
     * [˵��]: ���������л�״̬���������ڳ�d����Ҫ��������˫���߿����֣���d��ʾ�ҵ绰����ֹͣ����ҵ��
     *         ��������ظ��ɹ���ֻ�������������OK�ģ�����������Ӳ�����óɹ���
     *         Ŀǰ��֧�ֺ��ײ���
     *         ע��
     *         1�����������������Ĭ��Ϊ0��
     *         2����ǰHAL״̬û��ʵ�֡�
     *         ��������޲���ģʽ��ʹ�á�
     * [�﷨]:
     *     [����]: ^DPDT=<RatMode>,<DpdtValue>,<WorkType>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^DPDT=?
     *     [���]: ��֧��5Gʱ��
     *             <CR><LF>^DPDT: (list of supported <RatMode>s),(list of supported <DpdtValue>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ֧��5Gʱ��
     *             <CR><LF>^DPDT: (list of supported <RatMode>s),(list of supported <DpdtValue>s), (list of supported <WorkType>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <RatMode>: ����ֵ������ģʽ��
     *             0��GSM��
     *             1��WCDMA��
     *             2��LTE��
     *             3��TDSCDMA��
     *             4��CDMA��
     *             5��NR
     *     <DpdtValue>: ����ֵ��Ŀǰֻ�õ���8λ��
     *             ��4bit��ʾTAS/MAS״̬��
     *             0��TASֱͨMASֱͨ
     *             1��TAS����MASֱͨ
     *             2��TASֱͨMAS����
     *             3��TAS����MAS����
     *             ��4bit��ʾHall״̬��
     *             Bit4��Hall��Ч��־λ��0��ʾ��������Hall̬��1��ʾ����Bit5����Hall̬
     *             Bit5��Hall״̬λ��0��ʾ��Hall̬��1��ʾHall̬
     *             ע����ǰ��(0,1,2,3,16,17,18,19,48,49,50,51)ֵ��Ч��Ϊ������չ��DpdtValueȡֵ��Χ��Ϊ(0-65535)��
     *     <WorkType>: ����״̬��
     *             0��ҵ��״̬��
     *             1������״̬��
     * [ʾ��]:
     *     �� Wģ����������Ϊֱ̬ͨ
     *       AT^DPDT=1,0,1
     *       OK
     *     �� Wģ����������Ϊ����̬��Hall̬
     *       AT^DPDT=1,49,1
     *       OK
     *     �� Wģ����������Ϊ����̬�ͷ�Hall̬
     *       AT^DPDT=1,17,1
     *       OK
     *     �� Lģ����������Ϊ����̬
     *       AT^DPDT=2,1,1
     *       OK
     *     �� Lģ������Hall̬
     *       AT^DPDT=2,48,1
     *       OK
     *     �� Lģ�����÷�Hall̬
     *       AT^DPDT=2,16,1
     *       OK
     *       ע��TD-SCDMA��֧������Hall̬��LTE��֧����һ��AT����ͬʱ����Hall��Dpdt״̬��
     */
    { AT_CMD_DPDT,
      At_SetDpdtPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^DPDT", (VOS_UINT8 *)DPDT_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������˽��
     * [����]: ��ѯDPDT Valueֵ
     * [˵��]: ��ѯ���ߵ�ǰ״̬,���ص�ǰ����DPDTֵ��
     *         ˵�����޸����ߺ���������㷨������ת�����״̬���ܻᱻ�ı䣻����ʹ�ø������л�DPDT���ٴ�ʹ��[AT^DPDT=RatMode,0]�ָ�Ĭ��״̬��
     *         ˵��:
     *         ���������������Ĭ��Ϊ0��
     *         ��������޲���ģʽ��ʹ�á�
     * [�﷨]:
     *     [����]: ^DPDTQRY=<RatMode>,<WorkType>
     *     [���]: <CR><LF>^DPDTQRY: <DpdtValue><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^DPDTQRY=?
     *     [���]: ��֧��5Gʱ��
     *             <CR><LF>^DPDTQRY: (list of supported <RatMode>s)<CR><LF>
     *             <CR><LF>OK<CR><LF
     *             ֧��5Gʱ��
     *             <CR><LF>^DPDTQRY: (list of supported <RatMode>s), (list of supported <WorkType>s)<CR><LF>
     *             <CR><LF>OK<CR><LF
     * [����]:
     *     <RatMode>: ����ֵ������ģʽ��
     *             0��GSM��
     *             1��WCDMA��
     *             2��LTE��
     *             3��TDSCDMA��
     *             4��CDMA
     *             5��NR
     *     <DpdtValue>: ����ֵ��
     *             0��TASֱͨMASֱͨ
     *             1��TAS����MASֱͨ
     *             2��TASֱͨMAS����
     *             3��TAS����MAS����
     *             ע����ǰ��(0,1,2,3)ֵ��Ч��Ϊ������չ��DpdtValueȡֵ��Χ��Ϊ(0-65535)��
     *     <WorkType>: ����״̬��
     *             0��ҵ��״̬��
     *             1������״̬��
     * [ʾ��]:
     *     �� Wģ�²�ѯ����״̬
     *       AT^DPDTQRY=1,1
     *       ^DPDTQRY :0
     *       OK
     */
    { AT_CMD_DPDTQRY,
      At_SetQryDpdtPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^DPDTQRY", (VOS_UINT8 *)DPDT_QRY_PARA_STRING },

    /*
     * [���]: Э��AT-������˽��
     * [����]: ��������ͨ������ģʽ
     * [˵��]: ������������������ͨ���Ĳ���ģʽ�����ڲ��߲��ԡ����óɹ�ֱ�ӷ���OK������ʧ�ܷ��ش�����Ϣ��
     * [�﷨]:
     *     [����]: ^RXTESTMODE=<cmd>
     *     [���]: ����ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ִ�д��������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <cmd>: ����ͨ������ģʽ�������ͣ�ȡֵ0��1��Ĭ��ֵΪ0��
     *             0���˳���������ģʽ��
     *             1��������������ģʽ��
     * [ʾ��]:
     *     �� ���ý�����������ģʽ�ɹ�
     *       AT^RXTESTMODE=1
     *       OK
     *     �� ִ�в�������
     *       AT^RXTESTMODE=?
     *       ^RXTESTMODE: (0,1)
     *       OK
     */
    { AT_CMD_RXTESTMODE,
      AT_SetRxTestModePara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^RXTESTMODE", (TAF_UINT8 *)"(0,1)" },

#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ѯLTEС��CA״̬��Ϣ
     * [˵��]: ���ڲ�ѯLTEС�����С�����CA����״̬��CA����״̬��LaaС����ʶ��BandƵ�Ρ�ռ�ô���Ƶ����Ϣ��
     *         ���������LTEΪ��ģ����פ����LTE����ʱ��ѯ��Ч��
     * [�﷨]:
     *     [����]: ^LCACELLEX?
     *     [���]: ִ�гɹ�ʱ��
     *             [<CR><LF>^LCACELLEX: <cell_index>,<ul_cfg>, <dl_cfg>,<act_flg>,<laa_flg>,<band>,<band_width>, <earfcn>,<CR><LF> [<CR><LF>^LCACELLEX: <cell_index>,<ul_cfg>,<dl_cfg>,<act_flg>,<laa_flg>, <band>,<band_width>,<earfcn>,<CR><LF>[��]]] <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^LCACELLEX=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cell_index>: ����ֵ��LTEС��������0��ʾPCell������ΪSCell��
     *     <ul_cfg>: ����ֵ����С������CA�Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     *     <dl_cfg>: ����ֵ����С������CA�Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     *     <act_flg>: ����ֵ����С�� CA�Ƿ񱻼��
     *             0��δ���
     *             1���Ѽ��
     *     <laa_flg>: ����ֵ����С���Ƿ�ΪLaaС����
     *             0������LaaС����
     *             1����LaaС����
     *     <band>: ����ֵ����С����BandƵ�Σ���7����Band VII��
     *     <band_width>: ����ֵ����С��ռ�ô���
     *             0������Ϊ1.4MHz��
     *             1������Ϊ3MHz��
     *             2������Ϊ5MHz��
     *             3������Ϊ10MHz��
     *             4������Ϊ15MHz��
     *             5������Ϊ20MHz��
     *     <earfcn>: ����ֵ����С��Ƶ�㡣
     * [ʾ��]:
     *     �� UEפ����LTE����ʱ��ѯС��CA״̬��Ϣ
     *       AT^LCACELLEX?
     *       ^LCACELLEX: 0,1,1,1,0,7,2,21120
     *       ^LCACELLEX: 1,1,1,1,1,34,1,36230
     *       OK
     *     �� ִ�в�������
     *       AT^LCACELLEX=?
     *       OK
     */
    { AT_CMD_LCACELLEX,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryLCaCellExPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^LCACELLEX", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����LTEС��CA��Ϣ�����ϱ�
     * [˵��]: ���������ڿ���UE��פ��LTE����ʱ��SCell����״̬�仯��CA���/�ͷŵȳ��������ϱ�^LCACELLINFO���֪ͨAP��ǰLTEģʽ��С��CA״̬��Ϣ��
     * [�﷨]:
     *     [����]: ^LCACELLRPTCFG=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^LCACELLRPTCFG?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^LCACELLRPTCFG: <enable><CR><LF> <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^LCACELLRPTCFG=?
     *     [���]: <CR><LF>^LCACELLRPTCFG:(0,1)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ�������ϱ�^LCACELLINFO����Ŀ��أ�
     *             0���ر�^LCACELLINFO����������ϱ���
     *             1����^LCACELLINFO����������ϱ���
     * [ʾ��]:
     *     �� ��^LCACELLINFO����������ϱ�
     *       AT^LCACELLRPTCFG=1
     *       OK
     *     �� ��ѯ^LCACELLINFO����������ϱ�����״̬
     *       AT^LCACELLRPTCFG?
     *       ^LCACELLRPTCFG: 1
     *       OK
     *     �� ִ�в�������
     *       AT^LCACELLRPTCFG=?
     *       ^LCACELLRPTCFG: (0,1)
     *       OK
     */
    { AT_CMD_LCACELLRPTCFG,
      AT_SetLcaCellRptCfgPara, AT_SET_PARA_TIME, AT_QryLcaCellRptCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^LCACELLRPTCFG", (TAF_UINT8 *)"(0,1)" },
#endif

    /*
     * [���]: Э��AT-LTE���
     * [����]: LTEС��CA״̬��ѯ
     * [˵��]: ���ڲ�ѯLTEС�����С�����CA����״̬��CA����״̬��
     *         ͬʱ��ʾ��С���Ͷ����С����CA״̬��
     *         �ն�Ӧ�ñ�֤��LTEģ��ʹ�ø����������ģ�£���Ҫ�·��ò�ѯ��
     *         �����2G/3Gģʽ�»�LTEδפ��ʱ��Modem�յ��ò�ѯ����򷵻�����CAС����δ���á�δ����״̬��
     * [�﷨]:
     *     [����]: ^LCACELL?
     *     [���]: <CR><LF>^ LCACELL: "<cell_id> <ul_cfg> <dl_cfg> <act>"[, "<cell_id> <ul_cfg> <dl_cfg> <act>"[,......]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <cell_id>: ����ֵ��cell id��0��ʾPcell��������ʾScell��
     *     <ul_cfg>: ����ֵ����cell����CA�Ƿ����ã�0��ʾδ���ã�1��ʾ�����á�
     *     <dl_cfg>: ����ֵ����cell����CA�Ƿ����ã�0��ʾδ���ã�1��ʾ�����á�
     *     <act>: ����ֵ����cell CA�Ƿ񱻼��0��ʾδ���1��ʾ�Ѽ��
     * [ʾ��]:
     *     �� ��ѯLTEС��CA����״̬
     *       AT^LCACELL?
     *       ^LCACELL: "0 0 0 0", "1 1 1 1", "2 1 1 1", "3 0 0 0", "4 0 0 0", "5 0 0 0", "6 0 0 0", "7 0 0 0"
     *       OK
     */
    { AT_CMD_LCACELL,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryLcacellPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^LCACELL", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����UE�汾��
     * [˵��]: ����UEЭ��汾�ţ��Ѱ汾����Ϣд��NV�����һ�ο�������Ч��
     * [�﷨]:
     *     [����]: ^RADVER=<mod>,<ver>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^RADVER=?
     *     [���]: <CR><LF>^RADVER: (list of supported <mod>s),(list of supported < ver>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mod>: ����ֵ��Ŀǰֻ֧��2(LTE)��ȡֵ��ΧΪ(0-3)��δ������ֵ����Ԥ��ֵ��
     *             0��GSM��
     *             1��WCDMA��
     *             2��LTE��
     *     <ver>: ����ֵ��Э��汾�ţ�Ŀǰֻ֧����������ֵ��ȡֵ��ΧΪ(4-16)��δ������ֵ����Ԥ��ֵ��
     *             9��release version 9;
     *             10��release version 10;
     *             11��release version 11;
     *             12��release version 12;
     *             13��release version 13;
     *             14��release version 14;
     *             15��release version 15��
     * [ʾ��]:
     *     �� ����LTEЭ��汾��Ϊversion 9
     *       AT^RADVER=2,9
     *       OK
     */
    { AT_CMD_RADVER,
      AT_SetRadverPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^RADVER", (VOS_UINT8 *)"(0-3),(4-16)" },

    /*
     * [���]: Э��AT-������˽��
     * [����]: ���������·�����������Ϣ
     * [˵��]: ���������ڲ���У׼�����£�֪ͨLPHYǿ�����n�����塣
     *         �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش�����0�����ұ�����LTE��ģ�·���������򷵻�ERROR��
     * [�﷨]:
     *     [����]: ^FORCESYNC=n
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <n>: LPHY��Ҫ�·������������ȡֵ��Χ1~256��
     * [ʾ��]:
     *     �� ���������·�����������Ϣ
     *       AT^FORCESYNC=100
     *       OK
     */
    { AT_CMD_FORCESYNC,
      AT_SetForceSyncPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^FORCESYNC", (VOS_UINT8 *)"(1-256)" },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �ϻ���������
     * [˵��]: ��������������rfic mem�ϻ����ԺͲ�ѯ�ϻ����ԵĽ����
     *         ˵�������������Ҫ�ڷ������¼�����DSP��������ִ��, ����������Ҫ��ػ���������Ҫ�·�FCHAN����Ƶǰ�˱�������Ϊ����.
     *         ��������AT^TMODE=1;
     *         ����DSPʹ�ã�AT^FCHAN=X,X��
     * [�﷨]:
     *     [����]: ^FRFICMEM
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^FRFICMEM?
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>^FRFICMEM: <result><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <result>: 0x0��pass���ɹ���
     *             0x7FFFFFFE��running������ִ�У��ȴ��ײ�����
     *             0x7FFFFFFF��not start test (û�������µĲ���)
     *             ����ֵ(0~0x7FFFFFFE)��ʾ�ײ㷵�صĴ����롣
     *     <err_code>: 1��������û�м���DSP��
     * [ʾ��]:
     *     �� ����ִ�гɹ�
     *       AT^FRFICMEM
     *       OK
     *     �� ��ѯ�ɹ�
     *       AT^FRFICMEM?
     *       FRFICMEM: 1
     *       OK
     */
    { AT_CMD_FRFICMEM,
      At_SetRfIcMemTest, AT_NOT_SET_TIME, At_QryRfIcMemTest, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^FRFICMEM", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �ϻ���������
     * [˵��]: ��������������serdes�ϻ����ԺͲ�ѯ�ϻ����ԵĽ����
     *         ˵�������������Ҫ�ڷ�������ִ��
     *         ��������AT^TMODE=1;
     * [�﷨]:
     *     [����]: ^FSERDESRT=<testnum>,<ulsynclen>,<ulbursttime>,<ulstalltime>,<dlsynclen>,<dlbursttime>,<dlstalltime>[<testtime>,[<fwdClkAmp>]]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^FSERDESRT?
     *     [���]: ��ѯ�ɹ�ʱ��
     *             <CR><LF>FSERDESRT: <result><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <testnum>: ��ʾ��Ҫ���ԵĴ�������λ�������
     *     <ulsynclen>: ����Sync Rept���ȸ���
     *     <ulbursttime>: ����Burstʱ�䣬��λ�ͣ�
     *     <ulstalltime>: ����Stallʱ�䣬��λ�ͣ�
     *     <dlsynclen>: ����Sync Rept���ȸ���
     *     <dlbursttime>: ����Burstʱ�䣬��λ�ͣ�
     *     <dlstalltime>: ����Stallʱ�䣬��λ�ͣ�
     *     <result>: -2�����Ի�û���
     *             -1��û����������
     *             0���ɹ�
     *             ������ֵ����ʾ����ʧ�ܵ��ܴ���
     *     <testtime>: 1.8G�Ĳ���ʱ�䣨��λ�����ӣ�Ĭ��Ϊ0��
     *     <fwdClkAmp>: �ڷ���Ĭ��Ϊ0��
     * [ʾ��]:
     *     �� ����ִ�гɹ�
     *       AT^FSERDESRT=2000,1,2,3,4,5,6
     *       OK
     *     �� ��ѯ�ɹ����ײ�ִ�гɹ���
     *       AT^FSERDESRT?
     *       FRFICMEM: 0
     *       OK
     */
    { AT_CMD_FSERDESRT,
      At_SetFSerdesRt, AT_NOT_SET_TIME, At_QryFSerdesRt, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^FSERDESRT", (VOS_UINT8 *)"(0-4294967295),(0-65535),(0-65535),(0-65535),(0-65535),(0-65535),(0-65535),(0-65535),(0-65535)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: SERDES��������
     * [˵��]: �������������úͲ�ѯserdes������ز�����
     *         ˵����
     *         ����������ǰ���Ƚ������ģʽ��
     *         ����������첽���
     * [�﷨]:
     *     [����]: ^SERDESTESTASYNC=<operatetype>,<type>[,<para1>,[,<para2>[,<para3>[,<para4>[,<para5>[,<para6>[,<para7>[,<para8>[,<para9>[,<para110>[,<para11>[,<para12>[,<para13>[,<para14>[,<para15>[,<para16>[,<para17>[,<para18>[,<para19>[,<para20>]]]]]]]]]]]]]]]]]]]]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *             ����ǲ�ѯ���Խ����
     *             <CR><LF>^SERDESTESTASYNC: <result><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���Խ������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <operatetype>: �������ͣ�
     *             0�����ã�
     *             1����ѯ��
     *     <type>: �������ͣ�
     *             0�� ��ʾ���������ϱ���
     *             1~255����ʾ�����Ĳ������ͣ�
     *     <para1 ~ para20>: ���ݲ����������õĲ�ͬ������
     *             ��<type> = 0��para1��ʾ�Ǵ򿪻��ǹر������ϱ���
     *             0���ر������ϱ���
     *             1���������ϱ���
     *     <result>: ��ѯ�����
     *             0: �ɹ�
     *             1: ʧ��
     *             -1: ���ڲ���
     *             -2: û����������
     *             -3: ��ѯ���ͺͲ������Ͳ�һ��
     */
    { AT_CMD_SERDESTESTASYNC,
      At_SetSerdesAsyncTest, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^SERDESTESTASYNC", (VOS_UINT8 *)"(0-1),(0-255),(0-4294967295),(0-4294967295),(0-4294967295),\
(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),\
(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),\
(0-4294967295),(0-4294967295),(0-4294967295)" },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��DCXO�����¶�Ƶƫ��������
     * [˵��]: ���������ڷ�����ģʽ�£���DCXO�����¶�Ƶƫ�������á�
     *         ע��
     *         1���ڴ�NR�İ汾��Ŀǰֻ�ڵ�����ʵ��;
     *         2�����������������Ĭ��Ϊ0��
     *         3�� ���������BAND��Ƶ����أ���ĳһ��BAND��Ƶ�������ú���Ҫ�����BAND��Ƶ���Ϲرգ������Ӱ���̲��ԣ�
     *         4����֧��NR�İ汾�ϣ����������Ҫ��^FTXON��ִ�У�
     * [�﷨]:
     *     [����]: ^DCXOTEMPCOMP=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^DCXOTEMPCOMP?
     *     [���]: <CR><LF>^DCXOTEMPCOMP: <enable><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^DCXOTEMPCOMP=?
     *     [���]: <CR><LF>^DCXOTEMPCOMP: (list of supported <enable>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ��DCXO�����¶�Ƶƫ�����Ƿ�ʹ�ܣ�
     *             0���رն�DCXO�����¶�Ƶƫ����
     *             1��ʹ�ܶ�DCXO�����¶�Ƶƫ����
     *     <err>: �����롣
     *             0������ģʽ����
     *             11����Ϣ����ʧ��
     *             50����������
     * [ʾ��]:
     *     �� ʹ�ܶ�DCXO�����¶�Ƶƫ����
     *       AT^DCXOTEMPCOMP=1
     *       OK
     *     �� ��ѯ^DCXOTEMPCOMP����
     *       AT^DCXOTEMPCOMP?
     *       ^DCXOTEMPCOMP: 1
     *       OK
     *     �� ����^DCXOTEMPCOMP����
     *       AT^DCXOTEMPCOMP=?
     *       ^DCXOTEMPCOMP: (0,1)
     *       OK
     */
    { AT_CMD_DCXOTEMPCOMP,
      AT_SetDcxotempcompPara, AT_SET_PARA_TIME, AT_QryDcxotempcompPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DCXOTEMPCOMP", (VOS_UINT8 *)"(0,1)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡͨ���¶�
     * [˵��]: ���ڼ�������¶ȼ�������Ƿ�ʧЧ������ͨ�����ͺ�ͨ���ţ���ȡ��Ӧ���¶�ֵ��
     *         ���������������Ĭ��Ϊ0��
     * [�﷨]:
     *     [����]: ^FTEMPRPT=<channel_type>,<channel_num>
     *     [���]: <CR><LF>^FTEMPRPT: < channel_type >,< channel_num >,<temperature><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <channel_type>: ͨ�����͡�
     *             0���߼�ͨ��
     *             1������ͨ��
     *     <channel_num>: ͨ����
     *     <temperature>: �¶�ֵ������PA 0.1  XO��0.001��
     */
    { AT_CMD_FTEMPRPT,
      AT_QryFtemprptPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FTEMPRPT", (VOS_UINT8 *)"(0,1),(0-65535)" },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �������´򿪽��ջ�
     * [˵��]: �ڵ���״̬�´򿪻�رս��ջ�������Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     *         �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش�����0����������Ҫ�����÷������ŵ���^FCHAN����ѡ����Ƶͨ����^TSELRF����ִ�С�
     *         ���������������Ĭ��Ϊ0��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^FRXON=<switch>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^FRXON?
     *     [���]: <CR><LF>^FRXON: <switch><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <switch>: �Ƿ�򿪽��ջ���
     *             0���رս��ջ���
     *             1���򿪽��ջ���
     *     <err>: �����롣
     *             0������ģʽ����
     *             1��û�м���DSP��
     *             2���򿪽��ջ�ʧ�ܣ�
     *             3����������
     *             4��û������path(^TSELRF);
     *             11��������Ϣʧ�ܣ�
     *             50����������
     * [ʾ��]:
     *     �� �������´򿪽��ջ��ɹ�
     *       AT^FRXON=1
     *       OK
     */
    { AT_CMD_FRXON,
      At_SetFRxonPara, AT_SET_PARA_TIME, At_QryFRxonPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_FRXON_OTHER_ERR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FRXON", (VOS_UINT8 *)"(0-1)" },
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: Mipi��������д��
     * [˵��]: ���������ڶ�дMIPI��ֵ��
     *         ���������������Ĭ��Ϊ0��
     * [�﷨]:
     *     [����]: ^MIPIOPERATE=<operate_type>,<mipi_id>,<secondary_id>,<reg_addr>,<byte_cnt>[,<param1>]
     *     [���]: дִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��ִ�гɹ�ʱ��
     *             <CR><LF>^MIPIOPERATE: <value1>[,<value2>[,<value3>[,<value4>]]] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <operate_type>: ����ֵʮ���� ��Χ��0-1��
     *             0: ��ʾ��ȡmipiֵ
     *             1: ��ʾдmipiֵ
     *     <mipi_id>: ����ֵʮ���� ��Χ��0-15��
     *     <secondary_id>: ����ֵʮ���� ��Χ��0-15��
     *     <reg_addr>: ����ֵʮ���� ��Χ��0-255��
     *     <byte_cnt>: ����ֵʮ���� ��Χ��1-4��
     *             ��<operate_type>Ϊ1ʱ���������ֵӦ��Ϊ1��Ҳ����дֻ��дһ��ֵ
     *     <param1>: ����ֵʮ���� ��Χ��0~0xFF��
     *             ��<operate_type>Ϊ0ʱ��������ʾ��ȡ�ٶȣ�
     *             ��<operate_type>Ϊ1ʱ��������ʾдMIPI��ֵ��
     *     <value1~4>: MIPI��ֵ
     *     <err>: ������:
     *             50:��������
     * [ʾ��]:
     *     �� ִ�ж�ȡ����
     *       AT^MIPIOPERATE=0,0,7,3,1,0
     *       ^MIPIOPERATE: 14
     *       OK
     *     �� ִ��д������
     *       AT^MIPIOPERATE=1,0,7,3,1,5
     *       OK
     */
    { AT_CMD_MIPIOPERATE,
      AT_SetMipiOpeRatePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^MIPIOPERATE", (VOS_UINT8 *)"(0,1),(0-15),(0-15),(0-255),(1-4),(0-255)" },

    { AT_CMD_FRBINFO,
      AT_SetFRbInfoPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^FRBINFO", (VOS_UINT8 *)"(0-4294967295),(0-4294967295)" },
#endif
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    { AT_CMD_MIPIWR,
      AT_SetMipiWrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      /* mode, secondary_id,  address, data,channel */
      (TAF_UINT8 *)"^MIPIWR", (TAF_UINT8 *)"(0-9),(0-15),(0-255),(0-65535),(0-3)" },

    { AT_CMD_SSIWR,
      AT_SetSSIWrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^SSIWR", (TAF_UINT8 *)"(0-9),(0-1),(0-255),(0-65535)" }, /* mode, channel, address, data */

#endif

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: Mipi��ȡͨ������
     * [˵��]: ���������ڶ�ȡMIPI��ֵ����Kirin970������ƽ̨��ʹ�á�
     * [�﷨]:
     *     [����]: ^MIPIREAD=<read_type>,<mipi_id>,<secondary_id>,<reg_addr>,<speed_type>,<read_bit_mask>,<reserved1>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^MIPIREAD: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>+CME ERROR:  <err_code><CR><LF>
     *     [����]: ^MIPIREAD=?
     *     [���]: <CR><LF>^MIPIREAD: (list of supported <read_type>s) ,(list of supported <mipi_id>s) ,(list of supported <secondary_id>s), (list of supported <reg_addr>s), (list of supported <speed_type>s) ,(list of supported <read_bit_mask>s) ,(list of supported  <reserved1>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <read_type>: ����ֵʮ���� ��Χ��0-1����
     *             0����ʾRFIC��
     *             1����ʾRFFE���ݲ�֧�֣���
     *     <mipi_id>: ����ֵʮ���� ��Χ��0-15����
     *     <secondary_id>: ����ֵʮ���� ��Χ��0-15����
     *     <reg_addr>: ����ֵʮ���� ��Χ��0-65535����
     *     <speed_type>: ����ֵʮ���� ��Χ��0-2����
     *             0����ʾ1/4���ʣ�
     *             1����ʾ�����ʣ�
     *             2����ʾȫ���ʡ�
     *     <read_bit_mask>: ����ֵʮ���� ��Χ��0-255����
     *     <reserved1>: ����λ ��Χ��0-255����
     *             ע�������·�ͳһ��0��
     *     <value>: ��ȡ�����ص�ֵ��
     * [ʾ��]:
     *     �� ִ�ж�ȡ����
     *       AT^MIPIREAD=0,0,7,3,1,255,0
     *       ^MIPIREAD: 14
     *       OK
     */
    { AT_CMD_MIPIREAD,
      AT_SetMipiReadPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      /* read_type, mipi_id, secondary_id, reg_addr, speed_type, read_bit_mask, reserved1 */
      (TAF_UINT8 *)"^MIPIREAD", (TAF_UINT8 *)"(0,1),(0-15),(0-15),(0-65535),(0-2),(0-255),(0-255)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: Mipiд��ͨ������
     * [˵��]: ����������֪ͨ�����д��MIPI��ֵ����Kirin980������ƽ̨��ʹ�á�
     *         �������ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^PHYMIPIWRITE=<write_type>,<mipi_id>,<secondary_id>,<reg_addr>,<mipi_data>,<reserved1>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^PHYMIPIWRITE=?
     *     [���]: <CR><LF>^PHYMIPIWRITE: (list of supported <write_type>s) ,(list of supported <mipi_id>s) ,(list of supported <secondary_id>s), (list of supported <reg_addr>s), (list of supported <mipi_data>s),(list of supported <reserved1>s)
     *             <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <write_type>: ����ֵʮ���� ��Χ��0-1����
     *             0����ʾRFIC��
     *             1����ʾRFFE���ݲ�֧�֣���
     *     <mipi_id>: ����ֵʮ���� ��Χ��0-15����
     *     <secondary_id>: ����ֵʮ���� ��Χ��0-15����
     *     <reg_addr>: ����ֵʮ���� ��Χ��0-65535����
     *     <mipi_data>: ����ֵʮ���� ��Χ��0-255����
     *     <reserved1>: ����λ ��Χ��0-255����
     *             ע�������·�ͳһ��0��
     * [ʾ��]:
     *     �� ִ�ж�ȡ����
     *       AT^PHYMIPIWRITE=0,0,7,3,1,0
     *       OK
     */
    { AT_CMD_PHYMIPIWRITE,
      AT_SetPhyMipiWritePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      /* read_type, mipi_id, secondary_id, reg_addr, speed_type, read_bit_mask, reserved1 */
      (TAF_UINT8 *)"^PHYMIPIWRITE", (TAF_UINT8 *)"(0,1),(0-15),(0-15),(0-65535),(0-255),(0-255)" },
#endif
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: SLT�ӿڲ���
     * [˵��]: SLTͨ�ò��������ǰ֧�����¹��ܲ��ԣ�
     *         SERDES����
     *         alink����
     * [�﷨]:
     *     [����]: ^SLTTEST=<CMD>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SLTTEST=?
     *     [���]: <CR><LF>^SLTTEST: <CMD value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <CMD>: ����ֵ����Ҫִ�еľ�������ȡֵ��Χ8��9��
     *             8��serdes_test
     *             9��alink_test
     * [ʾ��]:
     *     �� SERDES����
     *       AT^SLTTEST=8
     *       OK
     */
    { AT_CMD_SLTTEST,
      AT_SetSltTestPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SLTTEST", (VOS_UINT8 *)AT_SET_SLT_TEST_TYPF },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡ�Ĵ���PDEG��ֵ
     * [˵��]: ���������ڶ�ȡPDEG��ֵ��
     * [�﷨]:
     *     [����]: ^FPOWDET?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^FPOWDET: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>ERROR<CR><LF>
     *             ����GSM�µĵ����źţ�
     *             <CR><LF>^FPOWDET: <value1>[,<value2>[,<value3>[,<value4>[,<value5>,<value6>,<value7>,<value8>]]]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <value>: PDEG��ֵ��
     * [ʾ��]:
     *     �� ִ�в�ѯ����
     *       AT^FPOWDET?
     *       ^FPOWDET: 10
     *       OK
     */
    { AT_CMD_FPOWDET,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryFpowdetTPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FPOWDET", VOS_NULL_PTR },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��õ�ǰ�ŵ�RSSIֵ
     * [˵��]: �ڵ���ģʽ�¶�ȡRSSIֵ������Ҫ����SIM/USIM��������Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     *         �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش�����0����������Ҫ�ڴ�RX��^FRXON����ִ�С�
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^FRSSI?
     *     [���]: <CR><LF>^FRSSI: <rvalue><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <rvalue>: ��ȡ��RSSIֵ��������ֵ��ʾ����ȷ��0.1dBm����ֵ��Ϣ�������ǰ��RSSIֵΪ-85.1dBm����ΧֵΪ851��
     *     <err>: �����롣
     *             0������ģʽ����
     *             1��û����������ŵ���
     *             2��û�д򿪽��ջ���
     *             3�����ź�������ⲻ���źţ�
     *             4����������
     * [ʾ��]:
     *     �� ��ȡ��ǰRSSIֵ
     *       AT^FRSSI?
     *       ^FRSSI: 851
     *       OK
     */
    { AT_CMD_FRSSI,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryFrssiPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FRSSI", VOS_NULL_PTR },
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯPLL״̬��Ϣ
     * [˵��]: ����������װ�����߲�ѯPLL����״̬��
     *         ע�����������Ҫ�ڳɹ�����^FCHAN�����ʹ�ã�����DSP������쳣��
     *         ��������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^FPLLSTATUS?
     *     [���]: <CR><LF>^FPLLSTATUS: <tx_status>,<rx_status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ����ʱ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^FPLLSTATUS=?
     *     [���]: <CR><LF>^FPLLSTATUS: (list of supported <tx_status>s), (list of supported <rx_status>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^FPLLSTATUS=<tx_or_rx>
     *     [���]: TX��ѯ�ɹ���
     *             <CR><LF>^FPLLSTATUS: <tx_status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             RX��ѯ�ɹ���
     *             <CR><LF>^FPLLSTATUS: <rx_status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ����ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <tx_status>: 0����ʾTx PLLʧ����
     *             1����ʾTx PLL������
     *     <rx_status>: 0����ʾRx PLLʧ����
     *             1����ʾRx PLL������
     *     <tx_or_rx>: 0: ��ʾTX��
     *             1����ʾRX��
     * [ʾ��]:
     *     �� ��ѯFPLLSTATUS
     *       AT^FPLLSTATUS?
     *       ^FPLLSTATUS: 1,1
     *       OK
     *     �� ��ѯTX״̬
     *       AT^FPLLSTATUS=0
     *       ^FPLLSTATUS: 1
     *       OK
     *     �� ��������
     *       AT^FPLLSTATUS=?
     *       ^FPLLSTATUS: (0,1),(0,1)
     *       OK
     */
    { AT_CMD_FPLLSTATUS,
      AT_SetFPllStatusQryPara, AT_SET_PARA_TIME, AT_QryFPllStatusPara, AT_QRY_PARA_TIME, AT_TestFPllStatusPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FPLLSTATUS", (VOS_UINT8 *)"(0,1)" },
#endif
};

/* ע��PHYװ��AT����� */
VOS_UINT32 AT_RegisterDevicePhyCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDevicePhyCmdTbl, sizeof(g_atDevicePhyCmdTbl) / sizeof(g_atDevicePhyCmdTbl[0]));
}

