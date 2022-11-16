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
#include "at_device_as_cmd_tbl.h"

#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_device_as_set_cmd_proc.h"
#include "at_device_as_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_AS_CMD_TBL_C

static const AT_ParCmdElement g_atDeviceAsCmdTbl[] = {
#if (FEATURE_MBB_CUST == FEATURE_OFF)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯ��Ʒ����
     * [˵��]: ��ѯ��Ʒ֧�ֵ����ԣ�������ʽ��Ƶ�Σ��͹��ܣ��ּ�����ء�����������wifi��FM��GPS��������LAN��NSF������Ϣ��
     * [�﷨]:
     *     [����]: ^SFEATURE?
     *     [���]: <CR><LF>^SFEATURE: <number><CR><LF>
     *             <CR><LF>^SFEATURE: <feature>[,<content>]<CR><LF>
     *             [��.]
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <number>: ���ܵ�������
     *     <feature>: ֧�ֵĹ������ƣ��ϱ�ʱֱ���ϱ����ƣ���ʹ��˫���š�
     *             LTE
     *             HSPA+
     *             HSDPA
     *             HSUPA
     *             DIVESITY
     *             UMTS
     *             EDGE
     *             GPRS
     *             GSM
     *             WIMAX
     *             WIFI
     *             GPS
     *             TD
     *             NR
     *     <content>: ���ܶ�Ӧ����Ϣ��
     *             LTE��֧�ֵ�Ƶ����Ϣ[5]chater 5����B1��B2�ȣ���Ƶ���м��ö��Ÿ�����
     *             HSPA+��֧�ֵ�Ƶ����Ϣ����BC1��BC2��BC3����Ƶ���м��ö��Ÿ�����
     *             HSDPA��֧�ֵ�Ƶ����Ϣ����BC1��BC2��BC3����Ƶ���м��ö��Ÿ�����
     *             HSUPA��֧�ֵ�Ƶ����Ϣ����BC1��BC2��BC3����Ƶ���м��ö��Ÿ�����
     *             DIVESITY��֧�ֵķּ���Ƶ����Ϣ��Ƶ�β���Index�±귽ʽ��������������������һ�£���ʽʹ��һ���ַ���ʾ���磺��L��-LTE����U��-UMTS����W��-WiMAX����F��-WiFi�ȣ���ʵ��ʱ�ַ��������ţ�
     *             UMTS��֧�ֵ�Ƶ����Ϣ����BC1��BC2��BC3����Ƶ���м��ö��Ÿ�����
     *             EDGE��֧�ֵ�Ƶ����Ϣ����850/900/1800/1900��ʾ����Ƶ���м��ö��Ÿ�����
     *             GPRS��֧�ֵ�Ƶ����Ϣ����850/900/1800/1900��ʾ����Ƶ���м��ö��Ÿ�����
     *             GSM��֧�ֵ�Ƶ����Ϣ����850/900/1800/1900��ʾ����Ƶ���м��ö��Ÿ�����
     *             WIMAX��֧�ֵ�Ƶ����Ϣ����2300/2500/3500��ʾ����Ƶ���м��ö��Ÿ�����
     *             WIFI��֧�ֵ���ʽ����a/b/g/n��ʾ������ʽ�м��ö��Ÿ�����
     *             GPS��GPS�ķ��࣬��GPSONE��GPS�ȣ�
     *             TD: ֧�ֵ�Ƶ����Ϣ����2000�� 2300��1900�ȣ�
     *             NR: ֧�ֵ�Ƶ����Ϣ��
     */
    { AT_CMD_FEATURE,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryFeaturePara, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SFEATURE", VOS_NULL_PTR },
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����HSDPA�����ȼ�
     * [˵��]: ����HSDPA�������͡�����Ʒ��֧��ֱ�ӷ���ERROR��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^DPACAT=<rate>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^DPACAT?
     *     [���]: <CR><LF>^DPACAT: <rate> <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <rate>: ���ʵȼ�������Ϊһ���ֽڡ�
     *             0��֧������Ϊ3.6M��
     *             1��֧������Ϊ7.2M��
     *             2��֧������Ϊ1.8M��
     *             3��֧������Ϊ14.4M��
     *             4��֧������Ϊ21M��
     * [ʾ��]:
     *     �� ����HSDPA�������ͳɹ�
     *       AT^DPACAT=0
     *       OK
     */
    { AT_CMD_DPACAT,
      At_SetDpaCat, AT_NOT_SET_TIME, At_QryDpaCat, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DPACAT", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����HSPA֧������
     * [˵��]: ����HSPA֧�����͵Ľӿڡ�����Ʒ��֧��ֱ�ӷ���ERROR��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^HSSPT=<rrc_ver>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^HSSPT?
     *     [���]: <CR><LF>^HSSPT: <rrc_ver> <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <rrc_ver>: RRC�汾������Ϊһ���ֽڡ�
     *             0��֧��WCDMA����֧��HSDPA/HSUPA���ܣ�
     *             1��֧��HSDPA+WCDMA����֧��HSUPA���ܣ�
     *             2��֧��WCDMA+HSDPA+HSUPA���ܣ�
     *             6��֧��WCDMA+HSPA+HSPA plus��
     *     <err>: 0�����ݱ���δ���������ʧ�ܣ�
     *             1���������ô���
     * [ʾ��]:
     *     �� ����HSPA���ͳɹ�
     *       AT^HSSPT=0
     *       OK
     */
    { AT_CMD_HSSPT,
      AT_SetHsspt, AT_NOT_SET_TIME, AT_QryHsspt, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HSSPT", (VOS_UINT8 *)"(0,1,2,6)" },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����GPRS��ʱ��
     * [˵��]: ����GPRS��ʱ��������Ʒ��֧��ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^GTIMER=<tgprs>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^GTIMER?
     *     [���]: <CR><LF>^GTIMER: <tgprs><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^GTIMER=?
     *     [���]: <CR><LF>^GTIMER: (list of supported < tgprs >s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <tgprs>: GPRS�Ķ�ʱֵ������Ϊ�ĸ��ֽڣ���λ�롣
     * [ʾ��]:
     *     �� ����GPRS��ʱ���ɹ�
     *       AT^GTIMER=1
     *       OK
     *     �� ��������
     *       AT^GTIMER=?
     *       ^GTIMER: (0-429496728)
     *       ok
     */
    { AT_CMD_GTIMER,
      AT_SetGTimerPara, AT_NOT_SET_TIME, AT_QryGTimerPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^GTIMER", (VOS_UINT8 *)"(0-429496728)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯNMR����
     * [˵��]: ������������ѯ����ʽ�������NMR��Network Measurement Report��������
     *         CLģʽ�¸����֧�֡�
     * [�﷨]:
     *     [����]: ^CNMR=<n>
     *     [���]: <CR><LF>^CNMR: <indexn>,<index1>,<nmr data>
     *             <CR><LF>^CNMR: <indexn>,<index2>,<nmr data>
     *             ...
     *             ^CNMR: <indexn>,<indexn>,<nmr data><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CNMR=?
     *     [���]: <CR><LF>^CNMR: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: 1����ѯ2G�µ�NMR���ݣ�
     *             2����ѯ3G�µ�NMR���ݣ�
     *             3����ѯ4G�µ�NMR���ݡ�
     *     <indexn>: ָʾ��ѯ������ж�����nmr���ݡ�
     *             2G��ȡֵ��ΧΪ[0,15]
     *             3G��ȡֵ��ΧΪ[0,8]
     *             4G��ȡֵ��ΧΪ[0,1]
     *     <index1>-<indexn-1>: ָʾ��ǰ�ϱ���nmr���ݵ�����ֵ��
     *             2G��Ϊ1�ϱ����Ƿ���С��NMR��Ϣ��Ϊ2�ϱ����ǵ�һ������NMR��Ϣ��Ϊ3�ǵڶ�������NMR��Ϣ���Դ����ƣ�����ϱ�14������NMR��Ϣ��
     *             3G��Ϊ1�ϱ����ǵ�һ��Ƶ���NMR��Ϣ��Ϊ2�ϱ����ǵڶ���Ƶ���NMR��Ϣ���Դ����ƣ�����ϱ�8��Ƶ��NMR��Ϣ��
     *             4GΪ1�ϱ�����С��NMR��Ϣ��
     *     <NMR Data>: ָʾindex��Ӧ��nmr���ݣ��������ŵ�2byte��16���������ַ�����
     *             ����ʽ��NMR���ݸ�ʽ��ͬ��2G��ʽ����3-1�е��ֽ�5-8��3G��ʽ����3-2�е��ֽ�5-796��4G��ʽ����3-3�е��ֽ�1-20��
     * [��]: 2G���ݸ�ʽ
     *       ����,                               ����,
     *       С������Ƶ��ţ�ȡֵ��Χ[0,1023]��, 2,
     *       С��BSIC��ȡֵ��Χ[0,63]��,         1,
     *       С��������ƽ��ȡֵ��Χ[0,63]��,     1,
     * [��]: 3G���ݸ�ʽ
     *       ����,                                                               ����,
     *       ��1��bitλ����n��Ƶ���Ƿ���frequencyInfo
     *       ��2��bitλ����n��Ƶ���Ƿ���utra-CarrierRSSI
     *       ��3��bitλ����n��Ƶ���Ƿ���cellMeasuredResultsList,                 1,
     *       Ԥ����,                                                             3,
     *       ��n��Ƶ���requencyInfo��WCDMAģʽ����TD_WCDMA��,                   4,
     *       ��1��bitλ��WCDMAģʽ�ĵ�n��Ƶ���requencyInfo������Ƶ���Ƿ���ڡ�, 1,
     *       Ԥ����,                                                             3,
     *       ����Ƶ�㡣,                                                         2,
     *       ����Ƶ�㡣,                                                         2,
     *       ��n��Ƶ���RSSI��,                                                  4,
     *       ��n��Ƶ���µ�С��������,                                            4,
     *       ��1��bitλ����n��Ƶ���µĵ�һ��С���Ƿ���cellid��,                  1,
     *       Ԥ����,                                                             3,
     *       ��n��Ƶ���µĵ�һ��С����cellid��,                                  4,
     *       ��n��Ƶ���µ�С����WCDMA����TD-WCDMAС����,                         4,
     *       ��1��bitλ����n��Ƶ��ĵ�һ��С���Ƿ���ecn0��
     *       ��2��bitλ����n��Ƶ���Ƿ���Rscp��
     *       ��3��bitλ����n��Ƶ���Ƿ���pathloss��,                              1,
     *       Ԥ����,                                                             3,
     *       ��n��Ƶ��ĵ�һ��С��������primary CPICH-Info��,                    2,
     *       ��n��Ƶ��ĵ�һ��С����ecn0��,                                      1,
     *       ��n��Ƶ��ĵ�һ��С����Rscp��,                                      1,
     *       ��n��Ƶ��ĵ�һ��С����pathloss��,                                  1,
     *       Ԥ����,                                                             3,
     *       ��n��Ƶ��ĵ�2��С����Ϣ��,                                         24,
     *       ��,                                                                 ,
     *       ��n��Ƶ��ĵ�32��С����Ϣ��,                                        24,
     * [��]: 4G���ݸ�ʽ
     *       �ֽ�,  ����,         ����,
     *       1-8,   CellGlobalID, 8,
     *       9-12,  CELLID,       4,
     *       13-14, TAC,          2,
     *       15-16, physCellId,   2,
     *       17-18, RSRP,         2,
     *       19-20, RSRQ,         2,
     *       21-22, TA,           2,
     *       23-24, ����,         2,
     * [ʾ��]:
     *     �� פ��2G������С���ľ���Ƶ���Ϊ0x00b2��178����BSICΪ0x30��48����������ƽΪ0x3f��63���������ľ���Ƶ���Ϊ0x0041��65����BSICΪ0x39��57����������ƽΪ0x3e��62����NMR���ݲ�ѯ��ʾ���£�
     *       AT^CNMR=1
     *       ^CNMR: 2,1,b200303f
     *       ^CNMR: 2,2,4100393e
     *       OK
     *     �� ��������
     *       AT^CNMR=?
     *       ^CNMR: (1,2,3)
     *       OK
     */
    { AT_CMD_CNMR,
      At_SetCnmrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CNMR", (VOS_UINT8 *)"(1,2,3)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����/����ָʾ��ѡ
     * [˵��]: ����������AP��֪ͨL��W������ѡ��
     * [�﷨]:
     *     [����]: ^WLTHRESHOLDCFG=<OffOnFlg>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^WLTHRESHOLDCFG=?
     *     [���]: <CR><LF>^WLTHRESHOLDCFG: (list of supported < OffOnFlg >s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <OffOnFlg>: 0��������
     *             1��������
     * [ʾ��]:
     *     �� ������Ļ
     *       AT^WLTHRESHOLDCFG=0
     *       OK
     *     �� ��������
     *       AT^WLTHRESHOLDCFG=?
     *       ^WLTHRESHOLDCFG: (0,1)
     *       OK
     */
    { AT_CMD_WLTHRESHOLDCFG,
      At_SetWlthresholdcfgPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^WLTHRESHOLDCFG", (VOS_UINT8 *)"(0,1)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: TRX TAS��������
     * [˵��]: ��������������TRX��TAS���ƣ�Ŀǰֻ������NRģ��
     *         ע����������ڷ������·�^FTXON����TX��֮���������ִ�С�
     * [�﷨]:
     *     [����]: ^TRXTAS=<mode>,<rat>,<cmd>[,<tas_value>]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <mode>: 0������ģʽ
     *             1��������ģʽ
     *     <rat>: 0��GSM
     *             1��WCDMA
     *             2��LTE
     *             3��TDSCDMA
     *             5��NR
     *             ˵����
     *             Ŀǰ����ͷ������¶�ֻ֧��NR��
     *     <cmd>: 0����ʾ���㷨 (�����֧��)��
     *             1����ʾ���ò�������Ҫ��<tas_value>������
     *             2����ʾ���㷨���ָ��ֻ�ԭ�е�trx tas�㷨(�����֧��)��
     *     <tas_value>: ����ֵ��͸����trxtas���� ,ʹ��UINT32�����5bit:  2bit UE������5bit�еĸ�λ2bit����0��ʾ1T2R��1��ʾ1T4R��2��ʾ2T4R  1bit TX������0��ʾTX0��1��ʾTX1  2bit ����port�ţ�0��1��2��3����ʾָ����TX�����е��ĸ�port��
     *     <err_code>: �����룺
     *             1: ��������û�д�TX��
     *             2: ��������<cmd>����ֻ������Ϊ1����������û�п����㷨��
     *             3: <rat>��������
     *             4: �������£�^TRXTAS��^FCHAN��RAT��ƥ�䣻
     *             50: ��������
     * [ʾ��]:
     *     �� �ر��㷨:
     *       at^trxtas=0,5,0
     *       1T2R�е�port0��at^trxtas=0,5,1,0
     *       1T2R�е�port1��at^trxtas=0,5,1,1
     *       1T4R�е�port0��at^trxtas=0,5,1,8
     *       1T4R�е�port1��at^trxtas=0,5,1,9
     *       1T4R�е�port2��at^trxtas=0,5,1,10
     *       1T4R�е�port3��at^trxtas=0,5,1,11
     *       2T4R TX0�е�port0��at^trxtas=0,5,1,16
     *       2T4R TX0�е�port1��at^trxtas=0,5,1,17
     *       2T4R TX1�е�port2��at^trxtas=0,5,1,22
     *       2T4R TX1�е�port3��at^trxtas=0,5,1,23
     *     �� �ָ��㷨:
     *       at^trxtas=0,5,2
     *       ˵����
     *       1T2R��ֻ��TX0����port 0 1֮����
     *       1T4R��ֻ��TX0����port 0 1 2 3֮����
     *       2T4R��TX0��port 0 1֮���У�TX1��port 2 3֮����
     *       NSA��ʱ��ʹ�õ���TX0������Ƶͨ·��TX0/1�޹أ�
     */
    { AT_CMD_TRXTAS,
      At_SetTrxTasPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^TRXTAS", (VOS_UINT8 *)"(0,1),(5),(0-2),(0-4294967295)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: TRX TAS��ѯ����
     * [˵��]: ����������TRX��TAS���Ʋ�ѯ��Ŀǰֻ������NR������ģʽ��
     *         ע�������֧�ֲ�ѯ��
     * [�﷨]:
     *     [����]: ^TRXTASQRY=<mode>,<rat>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>^TRXTASQRY: <rat>,<tas_value>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <mode>: 0������ģʽ
     *             1��������ģʽ
     *     <rat>: 0��GSM
     *             1��WCDMA
     *             2��LTE
     *             3��TDSCDMA
     *             5��NR
     *     <tas_value>: ��ѯ����TRXTAS������
     *             2bit UE������6bit�еĸ�λ2bit����0��ʾ1T2R��1��ʾ1T4R��2��ʾ2T4R��3��ʾ��Чֵ  2bit TX0����port�ţ�0��1��2��3����ʾTX0���ĸ�port  2bit TX1����port�ţ�0��1��2��3����ʾTX1���ĸ�port
     * [ʾ��]:
     *     �� ��ѯNR�·�����ģʽ�µ�TRXTASֵ��
     *       AT^TRXTASQRY=1,5
     *       ^TRXTASQRY: 0
     *       OK
     */
    { AT_CMD_TRXTASQRY,
      At_QryTrxTasPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^TRXTASQRY", (VOS_UINT8 *)"(0,1),(5)" },
#endif
};

/* ע��ASװ��AT����� */
VOS_UINT32 AT_RegisterDeviceAsCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDeviceAsCmdTbl, sizeof(g_atDeviceAsCmdTbl) / sizeof(g_atDeviceAsCmdTbl[0]));
}

