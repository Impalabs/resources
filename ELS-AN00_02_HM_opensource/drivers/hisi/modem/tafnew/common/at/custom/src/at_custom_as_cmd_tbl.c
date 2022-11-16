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
#include "at_custom_as_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"
#include "at_lte_common.h"

#include "at_custom_as_set_cmd_proc.h"
#include "at_custom_as_qry_cmd_proc.h"



#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_AS_CMD_TBL_C

static const AT_ParCmdElement g_atCustomAsCmdTbl[] = {
#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����GPS Fine Time/SFN����
     * [˵��]: �����������ṩ��GPSоƬ��ȡ�߾�������ʱ�䡣
     *         ����Ϊ�첽��������OK�󣬴�����Ӧ�����������ϱ��������Ӧ������ʱ����ͨ��^FINETIMEINFO�������^SFN���������ϱ���
     * [�﷨]:
     *     [����]: ^FINETIMEREQ=<type>[,<num>[,<rat>]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             Ŀǰ��֧��LTEģʽ������ģʽʱ�����ERROR��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^FINETIMEREQ=?
     *     [���]: <CR><LF>^FINETIMEREQ: (list of supported <type>s),(1-256), (list of supported <rat>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: �������ͣ������������ͣ�
     *             0��ǿ�ƴ������壨�麸���ʹ�ã���
     *             1������GPS Fine Time��SYNCʱ�ӣ����壻
     *             2������SFN֡���壨����LTEģʽ����
     *     <num>: �����ͣ����崥���ĸ�����ȡֵ��Χ1~256��<type>Ϊ0ǿ�ƴ��������1 SYNC����ʱ��������Ч���������·���Ĭ��ֵΪ����2��SYNC���塣
     *     <rat>: �������ͣ��ṩFine Time�Ľ��뼼�����������·�Ĭ��ΪLTE��
     *             0��GSM���ݲ�֧�֣���
     *             1��WCDMA���ݲ�֧�֣���
     *             2��LTE��
     *             3��TDS-CDMA���ݲ�֧�֣���
     *             4��CDMA_1X���ݲ�֧�֣���
     *             5��HRPD���ݲ�֧�֣���
     *             6��NR��
     * [ʾ��]:
     *     �� ��ѯGPS Fine Time��������ͨ����
     *       AT^FINETIMEREQ=1
     *       OK
     *     �� �麸��ⴥ������
     *       AT^FINETIMEREQ=0,250,6
     *       OK
     *     �� ִ�в�������
     *       AT^FINETIMEREQ=?
     *       ^FINETIMEREQ: (0-2),(1-256),(2,6)
     *       OK
     */
    { AT_CMD_FINE_TIME,
      AT_SetFineTimeReqPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^FINETIMEREQ", (TAF_UINT8 *)"(0-2),(1-256),(2,6)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ��ǿ��С��ID��Ϣ
     * [˵��]: �����������ṩ����֧��RAT��GSM��WCDMA��LTE��NR������ǿ��С��ID��Ϣ����ǿ��С��ID��Ϣ��������С��������С����Ϣ��GPSоƬ��ʹ����Щ��Ϣ��֧��SUPL1.0��2.0��λ�ñ�ʶ��
     * [�﷨]:
     *     [����]: +ECID=<version>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>+ECID=<version>,<rat>,[<cell_description>] <CR><LF><CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <version>: ����ֵ������汾�ţ���ǰΪ0��
     *     <rat>: �ַ������ͣ���ǰ�����߽��뼼����
     *             ��NONE:������ǰModem���ڷ���ģʽ��û��С��ID��Ϣ��
     *             ��GSM:������ǰ���뼼��ΪGSM��
     *             ��WCDMA:������ǰ���뼼��ΪWCDMA��
     *             ��LTE:������ǰ���뼼��ΪLTE;
     *             ��NR:������ǰ���뼼��ΪNR��
     *     <cell_description>: ��ǿ��С��ID��Ϣ��
     *             ���뼼��ΪGSMʱ��
     *             <MCC>,<MNC>,<LAC>,<CID>,<TA>,<NB_CELL> [,<BSIC>,<ARFCN>,<RX_level>]*NB_CELL
     *             ���뼼��ΪWCDMAʱ��
     *             <MCC>,<MNC>,<UCID>,<Primary_Scrambling>, <UARFCN_UL>,<UARFCN_DL>,<NB_FREQ> [,<UARFCN_UL>,<UARFCN_DL>,<UC_RSSI>,<CMR_NB> [,<Cell_ID>, <Primary_CPICH>,<CPICH_Ec_NO>, <CPICH_RSCP>,<Pathloss>]*CMR_NB]*NB_FREQ
     *             ���뼼��ΪLTEʱ��
     *             <MCC>,<MNC>,<TAC>,<LCID>,<L_NB_CELL> [,<Cell_Code>,<EARFCN>,<RSRP>,<RSRQ>,<L_TA>]*NB_CELL
     *             ˵����LTEʱС���б��е�һ��С��Ϊ����С���������Ϊ������<TA>����ֻ�ڷ���С��ʹ�á�
     *             ���뼼��ΪNRʱ��
     *             <MCC>,<MNC>,<TAC>,<NCID>,<N_NB_CELL> [,<Cell_Code>,<ARFCN-NR>,<5GSCS>, <5GRSRP>, <5GRSRQ>,<NR_TA>]*NB_CELL
     *             NRʱС���б��е�һ��С��Ϊ����С���������Ϊ������<TA>����ֻ�ڷ���С��ʹ�á�
     *     <MCC>: ����ֵ����ǰ�����롣
     *     <MNC>: ����ֵ����ǰ�����롣
     *     <LAC>: ����ֵ����ǰλ�����롣
     *     <CID>: ����ֵ����ǰС��ID��
     *     <TA>: ����ֵ����ǰʱ����ǰ������֧�ַ���-1��
     *     <NB_CELL>: ����ֵ������������
     *     <BSIC>: ����ֵ����������С����վ�롣
     *     <ARFCN>: ����ֵ����������Ƶ�㡣
     *     <RX_level>: ����ֵ���������Ĳ�����ƽ��
     *     <UCID>: ����ֵ��UTRA��ǰС��ID��
     *     <Primary_Scrambling>: ����ֵ�������룬��֧�ַ���-1��
     *     <UARFCN_UL>: ����ֵ������Ƶ�㣬��֧�ַ���-1��
     *     <UARFCN_DL>: ����ֵ������Ƶ�㣬��֧�ַ���-1��
     *     <NB_FREQ>: ����ֵ��Ƶ�������
     *     <UC_RSSI>: ����ֵ�������ź�ǿ�ȡ�
     *     <CMR_NB>: ����ֵ����Ƶ���²���������������
     *     <Cell_ID>: ����ֵ��������С��ID����֧�ַ���-1��
     *     <Primary_CPICH>: ����ֵ�������������롣
     *     <CPICH_Ec_NO>: ����ֵ��������EC NO����֧�ַ���-1��
     *     <CPICH_RSCP>: ����ֵ��������RSCP����֧�ַ���-127��
     *     <Pathloss>: ����ֵ��������Pathloss����֧�ַ���-1��
     *     <TAC>: ����ֵ��С�������롣
     *     <LCID>: ����ֵ��LTE��ǰС��ID��
     *     <L_NB_CELL>: ����ֵ��LTEС��������
     *     <Cell_Code>: ����ֵ��С���룬PhysCellId��
     *     <EARFCN>: ����ֵ��С��Ƶ�㣬��֧�ַ���-1��
     *     <RSRP>: ����ֵ���ο��źŽ��չ��ʡ�
     *     <RSRQ>: ����ֵ���ο��źŽ���������
     *     <L_TA>: ����ֵ��LTE��ʱ����ǰ������֧�ַ���-1��
     *     <NCID>: ����ֵ��NR��ǰС��ID��Cell Identity��
     *     <ARFCN-NR>: ����ֵ��NRС��SSBƵ�㣬��֧�ַ���-1��
     *     <N_NB_CELL>: ����ֵ��NRС��������
     *     <5GSCS>: ����ֵ��NR���ز������Ϣ��
     *             0��15kHz��
     *             1��30kHz��
     *             2��60kHz��
     *             3��120kHz��
     *             4��240khz��
     *     <5GRSRP>: ����ֵ��NR�ο��źŽ��չ��ʣ�ȡֵ��Χ0~127��
     *     <5GRSRQ>: ����ֵ��NR�ο��źŽ���������ȡֵ��Χ0~127��
     *     <NR_TA>: ����ֵ��NR��ʱ����ǰ������֧�ַ���-1��
     * [ʾ��]:
     *     �� ��ǰ���κ�С����Ϣ
     *       AT+ECID=0
     *       +ECID=0,NONE:,
     *       OK
     *     �� ��ǰ���뼼��ΪGSM������1������С����2������
     *       AT+ECID=0
     *       +ECID=0,GSM:,310,410,6042,4626,3,2,1,653,39,19,234,36
     *       OK
     *     �� ��ǰ���뼼��ΪWCDMA������1������С����3��Ƶ�㼰30������
     *       AT+ECID=0
     *       +ECID=0,WCDMA:,310,410,131353446,447,1314,1539,3,1314,1539,63,24,1,204,15,14,-1,2,183,-1,-127,-1,3,168,-1,-127,-1,4,104,7,10,-1,5,402,-1,-127,-1,6,186,-1,-127,-1,7,480,13,13,-1,8,420,-1,-127,-1,9,391,-1,-127,-1,10,374,14,14,-1,11,347,-1,-127,-1,12,167,-1,-127,-1,13,137,-1,-127,-1,14,99,-1,-127,-1,15,133,-1,-127,-1,16,199,-1,-127,-1,17,320,-1,-127,-1,18,470,-1,-127,-1,19,451,-1,-127,-1,20,120,-1,-127,-1,21,306,-1,-127,-1,22,411,-1,-127,-1,23,110,9,13,-1,24,144,-1,-127,-1,212,612,78,3,0,420,-1,-127,-1,1,402,-1,-127,-1,2,447,20,17,-1,1687,1912,83,3,3,447,18,14,-1,4,402,-1,-127,-1,5,420,-1,-127,-1
     *       OK
     *     �� ��ǰ���뼼��ΪLTE������1������С����7������
     *       AT+ECID=0
     *       +ECID=0,LTE:,310,410,35653,169078288,8,265,12345,60,25,256,289,23456,42,0,-1,290,34567,48,19,-1,19,45678,37,9,-1,20,56789,38,10,-1,21,01234,39,11,1,22,02345,40,12,2,23,03456,41,13,3
     *       OK
     */
    { AT_CMD_ECID,
      AT_SetEcidPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+ECID", (VOS_UINT8 *)"(0)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ��ǰ����С������
     * [˵��]: ���������ڲ�ѯ��ǰ����С�����ܡ�
     *         <PSC>,<Cell_ID>,<LAC>,<RSCP>,<RXLEV>,<EC/N0>,<DRX>,<URA>��Щ�������ڵ�һ��פ��С����С����ѡ��С���л�������̬�л�������̬ʱ�����ܶ�ʱ���ڻ�ȡ������Чֵ���������յ������
     *         ��һ��פ��LTEС��ʱ����ʱ���ڿ����޷���ȡ�˲�������Чֵ���������յ������
     *         GUL�£����L��NV 53877����NETSCAN�Ŀ���û���Ļ��������ϱ�L�ķ���С����Ϣ��
     *         RSRP��RSRQ��SINRĿǰ�����ϱ��ǻ���SS�Ĳ��������CSI-RS��������ϱ��ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^MONSC
     *     [���]: <CR><LF>^MONSC: <RAT>[,<cell_paras>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �޷���״̬ʱ��
     *             <CR><LF> ERROR<CR><LF>
     * [����]:
     *     <RAT>: ���뼼�����ַ������ͣ�
     *             GSM��פ����GSMС����������<cell_paras>������
     *             WCDMA��פ����WCDMAС����������<cell_paras>������
     *             TD_SCDMA��פ����TD_SCDMAС����������<cell_paras>������
     *             LTE��פ����LTEС����������<cell_paras>������
     *             NR��פ����NRС����������<cell_paras>���� ��
     *             NONE�������磬������<cell_paras>������
     *     <cell_paras>: ����С�����������ݽ��뼼����ͬ�����ز�ͬ�����������Ѿ���ȡ���Ĳ�������������������仯���ı䣬��ʱδ��ȡ���Ĳ������ա�
     *     <GSM����ģʽ����>:
     *     <MCC>: �ƶ������룬����ֵ��
     *     <MNC>: �ƶ������룬����ֵ��
     *     <BAND>: Ƶ�Σ�����ֵ����Χ��0-3����
     *             0��GSM 850��
     *             1��GSM 900��
     *             2��GSM 1800��
     *             3��GSM 1900��
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier������ֵ��ȡֵ��Χ��0~16383��
     *     <BSIC>: С����վ�룬ʮ���ƣ�ȡֵ��Χ��0~63��
     *     <Cell_ID>: ʮ�����ƣ�SIB3��Ϣ�е�CELL IDENTITY������ȡֵ��Χ��0~FFFF��
     *     <LAC>: λ�����룬ʮ�����ƣ�ȡֵ��Χ��0~FFFF��
     *     <RXLEV>: ����ֵ������ֵ����λdbm��ȡֵ��Χ��-120~-37��
     *             ���ڵ�һ��פ��С����С����ѡ��С���л�ʱ�����ܶ�ʱ���޷���ȡ��Чֵ���������յ������
     *     <RxQuality>: ��������������̬��ר��̬��Ч������ֵ��ȡֵ��Χ��0~7��
     *     <TA>: ʱ����ǰ����������������������̬ʱ���ܻ�ȡ��Чֵ������������գ�����ֵ��ȡֵ��Χ��0~63��
     *     <WCDMA����ģʽ����>:
     *     <MCC>: �ƶ������룬����ֵ
     *     <MNC>: �ƶ������룬����ֵ
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier������ֵ��ȡֵ��Χ��0~16383��
     *     <PSC>: �����룬����ֵ��ȡֵ��Χ��0~511��
     *     <Cell_ID>: ʮ�����ƣ�SIB3��Ϣ�е�CELL IDENTITY����������RNC ID�� CELL ID��ȡֵ��Χ��0~FFFFFFF��
     *     <LAC>: λ�����룬ʮ�����ƣ�ȡֵ��Χ��0~FFFF��
     *     <RSCP>: �����ź��빦�ʣ�Received Signal Code Power in dBm������ֵ��ȡֵ��Χ��-120~-25��
     *     <RXLEV>: ����ֵ��Receiving signal strength in dbm������ֵ����λdbm��ȡֵ��Χ��-120~-25��
     *     <EC/N0>: ÿ���Ʊ��ع��ʺ�����Ƶ���ܶȵı��ʣ�����ֵ
     *     <DRX>: Discontinuous reception cycle length������ֵ��ֻ����CS DRXֵ��ȡֵ��Χ��6~9��
     *     <URA>: UTRAN Registration Area Identity������ֵ��ȡֵ��Χ��0~65535��
     *     <LTE����ģʽ����>:
     *     <MCC>: �ƶ������룬����ֵ��
     *     <MNC>: �ƶ������룬����ֵ��
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier������ֵ��ȡֵ��Χ��0~ 133121��
     *     <Cell_ID>: SIB1�е�cell identity��ʮ�����ƣ�ȡֵ��Χ��0~0xFFFFFFF��
     *     <PCI>: ����С��ID������ֵ��ʮ�����ƣ�ȡֵ��Χ��0~0x1F7��
     *     <TAC>: ���������룬Tracking Area Code��ʮ�����ƣ�ȡֵ��Χ��0~0xFFFF��
     *     <RSRP>: �ο��źŽ��չ��ʣ�Reference Signal Received Power������ֵ��ȡֵ��Χ��-140~-44����λ��dBm��
     *     <RSRQ>: �ο��źŽ���������Reference Signal Received Quality������ֵ��ȡֵ��Χ��-19.5~-3����λ��dB��
     *     <RSSI>: �ź�ǿ�ȣ�Receiving signal strength in dbm������ֵ��ȡֵ��Χ��-90~-25����λ��dBm��
     *     <TD_SCDMA����ģʽ����>:
     *     <MCC>: �ƶ������룬����ֵ��
     *     <MNC>: �ƶ������룬����ֵ��
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier����9400-9600������10050-10125������11500-12000��
     *     <SYNC_ID>: ���е�Ƶ�룬����ֵ��ȡֵ��Χ��0~31��
     *     <SC>: ���룬����ֵ��ȡֵ��Χ��0~127��
     *     <CELL_ID>: cell identity��ʮ�����ƣ�ȡֵ��Χ��0~FFFFFFF��
     *     <LAC>: λ�����룬ʮ�����ƣ�ȡֵ��Χ��0~FFFF��
     *     <RSCP>: Received Signal Code Power in dBm�������ź��빦�ʣ�ȡֵ��Χ��-116~0��
     *     <DRX>: Discontinuous reception cycle length������ֵ��ȡֵ��Χ��6~9��
     *     <RAC>: ·������ţ�ʮ�����ƣ�ȡֵ��Χ��0~FFFF��
     *     <NR����ģʽ����>:
     *     <MCC>: �ƶ������룬����ֵ��
     *     <MNC>: �ƶ������룬����ֵ��
     *     <ARFCN-NR>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier������ֵ��ȡֵ��Χ��0~ 2279166��
     *     <SCS>: SubcarrierSpacing information������ֵ����Χ��0-4����
     *             0��15kHz��
     *             1��30kHz��
     *             2��60kHz��
     *             3��120kHz��
     *             4��240khz��
     *     <Cell_ID>: Cell identity��ʮ�����ơ�ȡֵ��Χ��0~0xFFFFFFFFFFFFFFFF
     *     <PCI>: ����С��ID��ʮ�����ƣ�ȡֵ��Χ��0~0x3EF��
     *     <TAC>: ���������룬Tracking Area Code��ʮ�����ƣ�ȡֵ��Χ��0~0xFFFFFF��
     *     <RSRP>: �ο��źŽ��չ��ʣ�Reference Signal Received Power������ֵ��ȡֵ��Χ��-156~-31����Чֵ��-1256(-157*8)����λ��dBm��
     *     <RSRQ>: �ο��źŽ���������Reference Signal Received Quality������ֵ��ȡֵ��Χ��-43~20����Чֵ��-348 (-43.5*8)����λ��dB��
     *     <SINR>: �ź�ǿ�ȣ�Receiving signal strength in dbm������ֵ��ȡֵ��Χ��-23~40����Чֵ��-188(-23.5*8)����λ��dBm��
     * [ʾ��]:
     *     �� ��ѯ��ǰ����С��
     *       AT^MONSC
     *       ^MONSC: LTE,460,01,1650,A5C933,1F3,183D,-98,-5,-72
     *       OK
     *     �� ��ѯ��ǰNR����С��
     *       AT^MONSC
     *       ^MONSC: NR,460,01,1650,1,AB12345678,1F3,1234AB,-80,-10,32
     *       OK
     */
    { AT_CMD_MONSC,
      At_SetNetMonSCellPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^MONSC", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ��ǰ��������
     * [˵��]: ���������ڲ�ѯ��ǰ�����������ܡ�
     *         <BSIC>,<Cell_ID>,<LAC>��Щ�����������绷�����ȶ�ʱ�������ڶ�ʱ�����޷���ȡ��Чֵ���������յ������<LAC>,<Cell_ID>�ڷ���С����LTEС��ʱ����ȡ������Чֵ�����ա�
     *         NR�¿����ϱ�LTE��������NR�������������ϱ�G��W��������L�¿����ϱ�G��W��L��NR��������
     *         RSRP��RSRQ��SINRĿǰ�����ϱ��ǻ���SS�Ĳ��������CSI-RS��������ϱ��ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^MONNC
     *     [���]: <CR><LF>^MONNC: <RAT>[,<cell_paras>] [<CR><LF>^MONNC: <RAT>[,<cell_paras>]][��]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <RAT>: ���뼼�����ַ�����ʾ��
     *             GSM��������GSMС����
     *             WCDMA��������WCDMAС����
     *             TD_SCDMA��������TD_SCDMAС����
     *             LTE��������LTEС����
     *             NR��������NRС��;
     *             NONE����������������<cell_paras>������
     *     <cell_paras>: �����Ĳ��������ݽ��뼼����ͬ�����ز�ͬ�����������Ѿ���ȡ���Ĳ�������������������仯���ı䣬��ʱδ��ȡ���Ĳ������ա�
     *     <GSM����ģʽ����>:
     *     <BAND>: Ƶ�Σ�����ֵ����Χ��0-3����
     *             0��GSM 850��
     *             1��GSM 900��
     *             2��GSM 1800��
     *             3��GSM 1900��
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier��ȡֵ��Χ��0~1023��
     *     <BSIC>: С����վ�룬ȡֵ��Χ��0~63��
     *     <Cell_ID>: ʮ�����ƣ�SIB3��Ϣ�е�CELL IDENTITY������ȡֵ��Χ��0~FFFF��
     *             (����С����LTEʱ���޷���ȡ����������)
     *     <LAC>: λ�����룬ȡֵ��Χ��0~FFFF��
     *             (����С����LTEʱ���޷���ȡ����������)
     *     <RXLEV>: Receiving signal strength in dbm��ȡֵ��Χ��-120~-37��
     *     <WCDMA����ģʽ����>:
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier��ȡֵ��Χ��0~16383��
     *     <PSC>: �����룬ȡֵ��Χ��0~511��
     *     <RSCP>: Received Signal Code Power in dBm�������ź��빦�ʣ�ȡֵ��Χ��-120~-25��
     *     <EC/N0>: ÿ���Ʊ��ع��ʺ�����Ƶ���ܶȵı��ʣ�ȡֵ��Χ��-25~0��
     *     <LTE����ģʽ����>:
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier��ȡֵ��Χ��0~133121��
     *     <PCI>: ����С��ID������ֵ��ȡֵ��Χ��0~503��
     *     <RSRP>: Reference Signal Received Power, �ο��źŽ��չ��ʣ�����ֵ��ȡֵ��Χ��-140~-44����λ��dBm��
     *     <RSRQ>: Reference Signal Received Quality, �ο��źŽ�������������ֵ��ȡֵ��Χ��-19.5~-3����λ��dB��
     *     <RXLEV>: Receiving signal strength in dbm������ֵ����λ��-90~-25����λ��dBm��
     *     <TD_SCDMA����ģʽ����>:
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier, ��9400-9600������10050-10125������11500-12000��
     *     <SC>: ���룬����ֵ��ȡֵ��Χ��0~127��
     *     <SYNC_ID>: ���е�Ƶ�룬����ֵ��ȡֵ��Χ��0~31��
     *     <RSCP>: Received Signal Code Power in dBm�������ź��빦�ʣ�����ֵ��ȡֵ��Χ��-116~0��
     *     <NR������ģʽ����>:
     *     <ARFCN-NR>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH carrier��ȡֵ��Χ��0~ 2279166��
     *     <PCI>: ����С��ID��ʮ�����ƣ�ȡֵ��Χ��0~0x3EF��
     *     <RSRP>: �ο��źŽ��չ��ʣ�Reference Signal Received Power������ֵ��ȡֵ��Χ��-156~-31����Чֵ��-1256(-157*8)����λ��dBm��
     *     <RSRQ>: �ο��źŽ���������Reference Signal Received Quality������ֵ��ȡֵ��Χ��-43~20����Чֵ��-348 (-43.5*8)����λ��dB��
     *     <SINR>: �ź�ǿ�ȣ�Receiving signal strength in dbm������ֵ��ȡֵ��Χ��-23~40����Чֵ��-188(-23.5*8)����λ��dBm��
     * [ʾ��]:
     *     �� ��ѯ��ǰ����
     *       AT^MONNC
     *       ^MONNC: GSM,3,128,9,0,0,-66
     *       ^MONNC: GSM,1,130,9,0,0,-69
     *       ^MONNC: WCDMA,10750,301,-78,-2
     *       ^MONNC: WCDMA,10562,351,-79,-3
     *       OK
     *     �� ��ǰNR�£���ѯ��ǰ����
     *       AT^MONNC
     *       ^MONNC: LTE,10000,1F,-82,-11,-1
     *       ^MONNC: NR,10001,1F3,-80,-10,32
     *       OK
     */
    { AT_CMD_MONNC,
      At_SetNetMonNCellPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^MONNC", VOS_NULL_PTR },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NSA����²�ѯ�����ӷ���С����Ϣ����
     * [˵��]: ���������ڲ�ѯNSA����²�ѯ�����ӷ���С����С����Ϣ��
     *         �������NSA������¶��᷵�ز�ѯʧ�ܡ�
     *         �����������֧��8CC������
     * [�﷨]:
     *     [����]: ^MONSSC
     *     [���]: <CR><LF>^MONSSC: <RAT>[,<cell_paras>]
     *             [<CR><LF>^MONSSC: <RAT>[,<cell_paras>]][,��]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �в�ѯ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <RAT>: ���뼼�����ַ�����ʾ��
     *             NR��ENDC�·���NR����С����
     *             LTE��NEDC �·���LTE����С����
     *             NONE����ENDC ״̬��������<cell_paras>������
     *     <cell_paras>: �����Ĳ��������ݽ��뼼����ͬ�����ز�ͬ�����������Ѿ���ȡ���Ĳ�������������������仯���ı䣬��ʱδ��ȡ���Ĳ������ա�
     *             NR �µĲ�����<ARFCNNR>,<PCI>,<RSRP>,<RSRQ>,<SINR>,<MEASTYPE>
     *             LTE �µĲ�����Ŀǰ�ݲ�֧�֣������������ٲ��䣩
     *     <ARFCN>: Ƶ�㣬Absolute Radio Frequency Channel Number of the BCCH
     *             carrier��ȡֵ��Χ��0~ 2279166��
     *     <PCI>: ����С��ID��ʮ�����ƣ�ȡֵ��Χ��0~0x3EF��
     *     <RSRP>: �ο��źŽ��չ��ʣ�Reference Signal Received Power������ֵ��ȡֵ��Χ��-156~-31����Чֵ��-1256(-157*8)����λ��dBm��
     *     <RSRQ>: �ο��źŽ���������Reference Signal Received Quality������ֵ��ȡֵ��Χ��-43~20����Чֵ��-348 (-43.5*8)����λ��dB��
     *     <SINR>: �ź�ǿ�ȣ�Receiving signal strength in dbm������ֵ��ȡֵ��Χ��-23~40����Чֵ��-188(-23.5*8)����λ��dBm��
     *     <MEASTYPE>: <RSRP><RSRQ><SINR>ȡֵ�Ķ�Ӧ����������0��SSB������1��CSI-RS��������RSRP/RSRQ/SINR����Чֵʱ����������дBUTT��
     * [ʾ��]:
     *     �� ��ѯ������Ϊ��CC����
     *       AT^MONSSC
     *       ^MONSSC: NR,633988,1,-70,-20,-10,0
     *       OK
     *     �� ��ѯ������Ϊ2CC����
     *       AT^MONSSC
     *       ^MONSSC: NR,633988,1,-70,-20,-10,0
     *       ^MONSSC: NR,633400,2,-68,-10,1,0
     *       OK
     *     �� ��ѯ������Ϊ2CC��������Scell��ǰ��û���ϱ��ź�
     *       AT^MONSSC
     *       ^MONSSC: NR,633988,1,-70,-20,-10,1
     *       ^MONSSC: NR,633400,2,-1256,-348,-188,2
     *       OK
     *     �� ��ѯ��С����֧�ָ�����
     *       AT^MONSSC
     *       ^MONSSC: NONE
     *       OK
     */
    { AT_CMD_MONSSC,
      At_SetNetMonSSCellPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^MONSSC", VOS_NULL_PTR },

    { AT_CMD_NRCELLBAND,
      At_SetNrCellBandPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NRCELLBAND", VOS_NULL_PTR },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ERRC��LTE������������
     * [˵��]: ͨ�����������Զ�̬�򿪺͹ر�ERRC��ĳ�����͵Ŀ���������
     * [�﷨]:
     *     [����]: ^ERRCCAPCFG=<cap_type>,<para1>[,<para2>[,<para3>]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^ERRCCAPCFG=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cap_type>: ����ֵ��ERRC��LTE�������ͣ�ȡֵ��Χ0~255��
     *             Ŀǰʵ��0~5��δʵ�ֵ�ΪԤ��ֵ��
     *             ÿ��ȡֵ��ʾ��ͬ���͵�LTE��������������:
     *             0���Ƿ�֧��CA�������
     *             1���Ƿ�֧��catg15,16,17,18����
     *             2��CA�������֧��MIMO����
     *             3�����֧�ֵ��ز�����
     *             4��EN_DC״̬��MIMO���� & �ز�����
     *             5���ͷ�NR��·������ͨ���ϱ�overheating��ʽ�ͷţ����overheating��֧�ֻ����粻�ͷţ���ͨ��SCG failure��ʽ�ͷ�
     *     <para1>: ����ֵ�����ö�Ӧ����LTE���������ĵĵ�һ���������������ȡֵ��Χ��ƽ̨ʵ�ʹ�����ơ�
     *             cap_type0��0��ʾ�ر�CA��1��ʾ֧��CA
     *             cap_type1��0��ʾ�ر�catg15,16,17,18������1��ʾ֧��catg15,16,17,18������
     *             cap_type2��0��ʾ�رգ�����ֵ��ȡ 2,4,8 ����ʾMIMO������
     *             cap_type3��0��ʾ�رգ��ز���������ΧΪ[1��оƬ֧�ֵ����CA�ز�����],ĿǰоƬ֧�ֵ����CA�ز�����Ϊ5��
     *             cap_type4�� MIMO������ֵ��0��ʾ�رգ�����ֵ��ȡ 2��4�����û��Я���������Ĭ����0��
     *             cap_type5��0��ʾenable EN-DC��1��ʾdisable EN-DC��
     *     <para2>: ����ֵ�����ö�Ӧ����LTE���������ĵĵڶ������:
     *             ˵����
     *             �ò����ǿ�ѡ�����ÿ�����������2������������Բ����øò���
     *             cap_type4���ز�������0��ʾ�رգ���ΧΪ[1��оƬ֧�ֵ����CA�ز�����]��ĿǰоƬ֧�ֵ����CA�ز�����Ϊ5�����û��Я���������Ĭ����0��
     *             cap_type5��0��ʾ����TAU��Ϣ��1��ʾ������TAU��Ϣ��
     *     <para3>: ����ֵ�����ö�Ӧ����LTE���������ĵĵ����������
     *             ˵����
     *             �ò����ǿ�ѡ�����ÿ�����������3������������Բ����øò���
     * [ʾ��]:
     *     �� ��ERRC����Ϊ1�Ŀ�������,�ÿ�������ֻ��1������
     *       AT^ERRCCAPCFG=1,1
     *       OK
     *     �� �ر�ERRC����Ϊ1�Ŀ����������ÿ�������ֻ��1������
     *       AT^ERRCCAPCFG=1,0
     *       OK
     */
    { AT_CMD_ERRCCAPCFG,
      AT_SetErrcCapCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ERRCCAPCFG", (VOS_UINT8 *)"(0-255),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ERRC��LTE����������ѯ
     * [˵��]: ͨ����AT�����ѯERRCĳ�����Ϳ����������������
     * [�﷨]:
     *     [����]: ^ERRCCAPQRY=<cap_type>
     *     [���]: ִ�гɹ�ʱ��
     *             ^ERRCCAPQRY: <cap_type>[<para1>,<para2>��<para3>]
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^ERRCCAPQRY=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cap_type>: ����ֵ��ERRC��LTE�������ͣ�ȡֵ��Χ0~255��
     *             Ŀǰʵ��0~5��δʵ�ֵ�ΪԤ��ֵ��
     *             ÿ��ȡֵ��ʾ��ͬ���͵�LTE��������������:
     *             0���Ƿ�֧��CA�������
     *             1���Ƿ�֧��catg15,16,17,18����
     *             2��CA�������֧��MIMO����
     *             3�����֧�ֵ�Scell����
     *             4��EN_DC״̬��MIMO���� & �ز�����
     *             5��ͨ��SCG failure�ͷ�NR��·
     *     <para1>: ����ֵ�����ö�Ӧ����LTE���������ĵĵ�һ�����
     *     <para2>: ����ֵ�����ö�Ӧ����LTE���������ĵĵ�һ�����
     *     <para3>: ����ֵ�����ö�Ӧ����LTE���������ĵĵ�һ�����
     * [ʾ��]:
     *     �� ��ѯERRC������Ϊ2�Ŀ�������
     *       AT^ERRCCAPQRY=2
     *       ^ERRCCAPQRY:2,1,0,1
     *       OK
     */
    { AT_CMD_ERRCCAPQRY,
      AT_SetErrcCapQryPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ERRCCAPQRY", (VOS_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����α��վС����Ϣ֪ͨ
     * [˵��]: ����������AP������ʶ������αС���󣬽���Ϣ֪ͨ��modem��
     * [�﷨]:
     *     [����]: ^PSEUCELL=<Type>,<Rat>,<PLMN>,<Lac>,<CellID>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <Type>: ֪ͨ���ͣ�����ֵ
     *             0��ͨ������ʶ���αС��
     *             Ŀǰֻ֧��0��
     *     <Rat>: ���뼼��������ֵ
     *             1��GSM
     *             3��WCDMA
     *             4��TD-SCDMA
     *             6��LTE
     *             Ŀǰֻ֧��1:GSM��
     *     <PLMN>: ����PLMN��Ϣ��
     *             �ַ������ͣ���Ӫ����Ϣ��
     *     <Lac>: λ�����룺
     *             2byte��16���������ַ������ͣ�λ������Ϣ�����磺��00C3����ʾ10���Ƶ�195��
     *     <CellID>: С��ID��
     *             4byte��16���������ַ������ͣ�С����Ϣ��
     * [ʾ��]:
     *     �� ��������α��վС����Ϣ
     *       AT^PSEUCELL=0,1,"46000","00C3","0000001A"
     *       OK
     *     �� ��ѯ����α��վС����Ϣ
     *       AT^PSEUCELL?
     *       ERROR
     *     �� ��������
     *       AT^PSEUCELL=?
     *       ERROR
     */
    { AT_CMD_PSEUCELL,
      AT_SetPseucellInfoPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^PSEUCELL", (VOS_UINT8 *)"(0),(1,3,4,6),(str),(str),(str)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����ECC״̬�����ϱ�
     * [˵��]: ͨ�����������Զ�̬�򿪺͹ر�ECC�ϱ����ܡ�
     * [�﷨]:
     *     [����]: ^ECCCFG=<enable>,<interval>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^ECCCFG=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ����ԴECCʹ�ܣ�
     *             0���ر��ϱ���
     *             1�����ϱ���
     *     <interval>: ����ֵ�������ϱ�ʱ��������λ���룬�Ƽ�ֵ1500��������С��1000��
     * [ʾ��]:
     *     �� ��ECC�ϱ�����
     *       AT^ECCCFG=1,1500
     *       OK
     */
    { AT_CMD_ECCCFG,
      AT_SetEccCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ECCCFG", (VOS_UINT8 *)"(0-1),(1000-65535)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯα��վ��ز���
     * [˵��]: ����������AP���ѯα��վ�Ƿ�֧�ֻ�α��վ���ش�����
     * [�﷨]:
     *     [����]: ^PSEUDBTS=<RAT>,<TYPE>
     *     [���]: ִ����ȷʱ��
     *             <CR><LF>^PSEUDBTS: <RSLT><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *             ע����֧�ֲ�ѯ����Ͳ�������
     * [����]:
     *     <RAT>: 1:GSM
     *             2:WCDMA���ݲ�֧�֣�
     *             3:LTE
     *     <TYPE>: 1:��ѯα��վ�Ƿ�֧��
     *             2:��ѯα��վ���ش���
     *     <RSLT>: TYPEΪ1ʱ����Ӧ��α��վ�Ƿ�֧�֣�����ֵ1��ʾ֧�֣�0��ʾ��֧�֣�
     *             0����֧�֣�
     *             1��֧�֣�
     *             TYPEΪ2ʱ����Ӧ��α��վ���ش���
     * [ʾ��]:
     *     �� AP��ѯ2Gα��վ���ش�����
     *       AT^PSEUDBTS=1,2
     *       ^PSEUDBTS: 10
     *       OK
     */
    { AT_CMD_PSEUDBTS,
      AT_SetPseudBtsPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^PSEUDBTS", (VOS_UINT8 *)"(1,3),(1,2)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���û���ѧϰ�ӷ�����ģ�Ͳ���
     * [˵��]: ����������AP��ѻ���ѧϰ�ӷ�����ģ�͵������ݲ����·���MODEM��
     * [�﷨]:
     *     [����]: ^SUBCLFSPARAM=<SEQ>,<VER>,<Type>,<FUNCSET>,<PROBA>,<CLFSNUM>[[,<BS1>[,<BS2>[,
     *             <BS3>]]]]
     *     [���]: ִ����ȷʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *             ע����������ֱ�ӷ���ERROR
     *     [����]: ^SUBCLFSPARAM?
     *     [���]: <CR><LF>^SUBCLFSPARAM:<VER><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <SEQ>: ��ˮ�ţ�ȡֵ��ΧΪ1��255�����һ��Ϊ255����ֻ��һ������Ϊ255
     *     <VER>: ��ͨѶ�汾�ţ���ʽΪxx.xx.xxx
     *     <Type>: �ӷ��������ͣ�
     *             0������ѧϰGSMα��վģ�Ͳ���
     *             Ŀǰֻ֧��0������ֵ��ʧ�ܴ���
     *     <FUNC SET>: �ӷ��������Ͷ�Ӧ�Ĺ������ã�
     *             0�����ܹر�
     *             1�����ܴ�
     *     <PROBA>: ����ѧϰ�������޲�����
     *             ������ΧΪ0~99999������
     *             �����磺99999����ʾ99.999%�ĸ��ʡ�����ѧϰ�㷨ʹ��ʱ����ת����
     *     <CLFS NUM>: ����AT������CLFS����������
     *             ����Ϊ0ʱ��������<FUNC SET>���أ�������BS������
     *             ���CLFS NUM����������ģ�Ͳ�����BS1��BS2��BS3������������ֱ�ӷ���ʧ�ܡ�
     *     <BS1>: CLFSģ�Ͳ������ݡ�
     *             BS1��BS2,BS3�����У�Ҳ����û�С�ÿ��BS���500�ַ���
     *             ����ѧϰģ�Ͳ�����Type0����ʽ��
     *             ÿ�����4��������ǰ3��Ϊ�����������1����Ϊ��1����-1����ÿ�������Զ��Ÿ��������磺
     *             1200,2,2782075,1,
     *             6399,3,774506,-1
     *     <BS2>: ��ʽͬBS1��ÿ�����4���������Զ��Ÿ�����
     *     <BS3>: ��ʽͬBS1��ÿ�����4���������Զ��Ÿ�����
     * [ʾ��]:
     *     �� AP�·�����ѧϰģ�Ͳ������
     *       AT^SUBCLFSPARAM=1,"01.01.001",0,1,99999[g1],2,"1200,2,2782075,1,6399,3,774506,-1"
     *       OK
     *       AT^SUBCLFSPARAM=255,"01.01.001",0,1,99999[g2],1,"4800,0,1261034,1"
     *       OK
     *     �� AP�·����û���ѧϰGSMα��վ���ܹر����
     *       AT^SUBCLFSPARAM=255,"01.01.001",0,0,99999[g3],0
     *       OK
     *     �� ��ѯ���
     *       AT^SUBCLFSPARAM?
     *       ^SUBCLFSPARAM: "01.01.001"
     *       OK
     */
    { AT_CMD_SUBCLFSPARAM,
      AT_SetSubClfsParamPara, AT_SET_PARA_TIME, AT_QrySubClfsParamPara, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^SUBCLFSPARAM", (VOS_UINT8 *)"(1-255),(str),(0),(0,1),(0-99999),(0-65535),(str),(str),(str)" },

#if (FEATURE_LTE == FEATURE_ON)
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����֪ͨLRRC�����ָ����ӿ������SCG
     * [˵��]: �������ٻ�5G��NSA������������������SCG���������£�AP�·�������·�ָ��������SCG�Ĳ�����5G��ʹ�á�
     * [�﷨]:
     *     [����]: ^CONNECTRECOVERY
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� AP�·�������·�ָ��������SCG����
     *       AT^CONNECTRECOVERY
     *       OK
     */
    { AT_CMD_CONNECT_RECOVERY,
      AT_SetConnectRecovery, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8*)"^CONNECTRECOVERY", VOS_NULL_PTR },
#endif
#endif

    { AT_CMD_TFDPDT,
      At_SetTfDpdtPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^TFDPDT", (VOS_UINT8 *)DPDT_CMD_PARA_STRING },

#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����LTE��UE�������ݲ���
     * [˵��]: ����������AP�ཫ�ƶ˻��߰汾Ԥ�õ���PLMN����LTE��UE�������ݲ����·���MODEM����^LRRCUECAPINFONTF�������ʹ�á�
     *         ע��AT�����ܳ��Ȳ�����1600��
     * [�﷨]:
     *     [����]: ^LRRCUECAPPARAMSET=<seq>,<EndFlag>,<CapParamNum>,<para1>[,<para2>[,<para3>]]
     *     [���]: ִ����ȷʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LRRCUECAPPARAMSET=?
     *     [���]: <CR><LF>^LRRCUECAPPARAMSET:(list of supported <Seq>s),(list of supported <EndFlag>s),(list of supported <CapParamNum>s),<para>,<para>,<para><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Seq>: ���ͣ���ˮ�ţ�ȡֵ��Χ��[1,65535]
     *     <EndFlag>: ���ͣ�����������ϱ�ʶ��
     *             0��δ���
     *             1�������
     *     <CapParamNum>: ���ͣ�����AT������UE�������ݲ���������ȡֵ��Χ��[1,3]
     *             ���CapParamNum��������������������Para1��Para2��Para3������������ֱ�ӷ���ʧ�ܡ�
     *     <Para1>: UE�������ݲ�������1���ɽ���������������Ȳ�����560���ҳ��ȱ���Ϊż����
     *     <Para2>: UE�������ݲ�������2���ɽ���������������Ȳ�����560���ҳ��ȱ���Ϊż����
     *     <Para3>: UE�������ݲ�������3���ɽ���������������Ȳ�����560���ҳ��ȱ���Ϊż����
     * [ʾ��]:
     *     �� AP�·�UE�������ݲ������
     *       AT^LRRCUECAPPARAMSET=1,0,3,"1111111111","22222222","3333333333"
     *       OK
     *       AT^LRRCUECAPPARAMSET=2,1,2,"4444444444","55555555"
     *       OK
     *     �� ���Ա�����
     *       AT^LRRCUECAPPARAMSET=?
     *       (1-65535),(0-1),(1-3),(str),(str),(str)
     *       OK
     */
    { AT_CMD_LRRC_UE_CAP_SET,
      AT_SetLrrcUeCapPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^LRRCUECAPPARAMSET", (TAF_UINT8 *)"(1-65535),(0-1),(1-3),(str),(str),(str)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���Ȼ�������AT����
     * [˵��]: ���ӿ�������������UE���ù�������������վ�����Ȼ���Ĵ�ʩ���Բ�ͬ������ʽ��������Я����ͬ�Ĳ�������Ŀ��Ϊ�ն��ṩ�Ȼ��ⷽ�����ն�UE�����Ȼ������󣬻�վִ�С�ֻ�Ե�ǰ��ģ���á�
     * [�﷨]:
     *     [����]: ^OVERHEATINGCFG=<type>,<overheatingFlag>,<setParamFlag>[,<para1>,<para2>,<para3>,<para4>[,<para5>,<para6>>,<para7>,<para8>,<para9>,<para10>]]
     *     [���]: ִ����ȷʱ��
     *             <CR><LF>OK<CR><LF>
     *             �д���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^OVERHEATINGCFG=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: ����ֵ
     *             ��ǰ���뼼��ָʾ
     *             0��LTE
     *             1:  NR
     *     <overheatingFlag>: ����ֵ
     *             0����⵽���ȣ����в�������
     *             1���˳�����״̬
     *     <setParamFlag>: ����ֵ
     *             ���õĲ�����־λ��
     *             ��type=0ʱ:
     *             bit0 : currentReportCcs �Ƿ���Ч
     *             bit1��currentReportCat �Ƿ���Ч
     *             bit2-bit30������λ����ǰ�汾����Ϊ0
     *             bit31���Ƿ�������Ч������NR��Ч
     *             ��type=1ʱ:
     *             bit0 : currentReportCcs �Ƿ���Ч
     *             bit1��currentReportMimoFr1�Ƿ���Ч
     *             bit2��currentReportBwFr1�Ƿ���Ч
     *             bit3��currentReportMimoFr2�Ƿ���Ч
     *             bit4��currentReportBwFr2�Ƿ���Ч
     *             bit5-bit30������λ����ǰ�汾����Ϊ0
     *             bit31���Ƿ�������Ч������NR��Ч
     *     <Para1>: ����ֵ��ʮ��������
     *             Para1Ϊ���Ȼ����ϱ���DL CC��
     *             ��type=1ʱ��Э��ȡֵ��ΧΪ[0,31]��ʵ��ȡֵΪ[0,����Ʒ֧�ֵ�DL CC�� - 1]
     *     <Para2>: ����ֵ��ʮ��������
     *             Para2Ϊ���Ȼ����ϱ���UL CC��
     *             ��type=1ʱ��Э��ȡֵ��ΧΪ[0,31]��ʵ��ȡֵΪ[0,����Ʒ֧�ֵ�UL CC�� - 1]
     *     <Para3>: ����ֵ��ʮ��������
     *             ��type=0ʱ��Para3Ϊ���Ȼ����ϱ���DL Category
     *             ��type=1ʱ��Para3Ϊ���Ȼ����ϱ���FR1 DL MIMO
     *             AP��MIMOö��ת����UU����ֵ�� MIMOö��ֵӳ���ϵ���£�
     *             0��layers��2��
     *             1��layers��4��
     *             2��layers��8��
     *     <Para4>: ����ֵ��ʮ��������
     *             ��type=0ʱ��Para4Ϊ���Ȼ����ϱ���UL Category
     *             ��type=1ʱ��Para4Ϊ���Ȼ����ϱ���FR1 UL MIMO
     *             AP��MIMOö��ת����UU����ֵ�� MIMOö��ֵӳ���ϵ���£�
     *             0��layers��1��
     *             1��layers��2��
     *             2��layers��4��
     *     <Para5>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para5Ϊ���Ȼ����ϱ���FR1 DLĿ�����BandWidthDLö��ֵ
     *     <Para6>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para6Ϊ���Ȼ����ϱ���FR1 ULĿ�����BandWidthULö��ֵ
     *     <Para7>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para7Ϊ���Ȼ����ϱ���FR2 DL MIMO
     *             AP��MIMOö��ת����UU����ֵ�� MIMOö��ֵӳ���ϵ���£�
     *             0��layers��2��
     *             1��layers��4��
     *             2��layers��8��
     *     <Para8>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para8Ϊ���Ȼ����ϱ���FR2 UL MIMO
     *             AP��MIMOö��ת����UU����ֵ�� MIMOö��ֵӳ���ϵ���£�
     *             0��layers��1��
     *             1��layers��2��
     *             2��layers��4��
     *     <Para9>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para9Ϊ���Ȼ����ϱ���FR2 DLĿ�����BandWidthDLö��ֵ
     *     <Para10>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para10Ϊ���Ȼ����ϱ���FR2 ULĿ�����BandWidthULö��ֵ
     * [ʾ��]:
     *     �� �ϱ�LTE OVERHEATING���Ʋ�����������CC������Ϊ2������cat��������Ϊ8
     *       AT^OVERHEATINGCFG=0,0,3,2,2,8,8
     *       OK
     */
    { AT_CMD_OVER_HEATING_SET,
      AT_SetOverHeatingCfgPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^OVERHEATINGCFG", (TAF_UINT8 *)"(0-1),(0-1),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���Ȼ����ѯAT����
     * [˵��]: ���ӿ������������ڲ�ѯ��ǰע��С���Ƿ�֧�ֹ��Ȼ��������Լ���ǰ���뼼����LTE��NR������һ�����õĹ��Ȼ������ò�����ֻ�Ե�ǰ��ģ��ѯ��
     * [�﷨]:
     *     [����]: ^OVERHEATINGQRY=<type>
     *     [���]: ִ����ȷʱ��
     *             ^OVERHEATINGQRY: <OverHeatingSupportFlag> ,<lastReportParamFlag>,<para1>,<para2>��<para3>,<para4>,<para5>,<para6>,<para7>,<para8>,<para9>,<para10>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^OVERHEATINGQRY=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: ����ֵ, ʮ��������
     *             ��ǰ���뼼��ָʾ
     *             0��LTE
     *             1:  NR
     *     <OverHeatingSupportFlag>: ����ֵ, ʮ��������
     *             ָʾ��ǰС���Ƿ�֧��OVERHEATING
     *             0����֧��
     *             1��֧��
     *     <lastReportParamFlag>: ����ֵ, ʮ��������
     *             ���һ���ϱ��Ĳ����Ƿ���Ч��־λ��0��Ч��1��Ч
     *             ����ֵ
     *             ���õĲ�����־λ��
     *             ��type=0ʱ:
     *             bit0 : currentReportCcs �Ƿ���Ч
     *             bit1��currentReportCat �Ƿ���Ч
     *             bit2-bit30������λ����ǰ�汾����Ϊ0
     *             bit31���Ƿ�������Ч������NR��Ч
     *             ��type=1ʱ:
     *             bit0 : currentReportCcs �Ƿ���Ч
     *             bit1��currentReportMimoFr1�Ƿ���Ч
     *             bit2��currentReportBwFr1�Ƿ���Ч
     *             bit3��currentReportMimoFr2�Ƿ���Ч
     *             bit4��currentReportBwFr2�Ƿ���Ч
     *             bit5-bit30������λ����ǰ�汾����Ϊ0
     *             bit31���Ƿ�������Ч������NR��Ч
     *     <Para1>: ����ֵ��ʮ��������
     *             Para1Ϊ���һ�ι��Ȼ����ϱ���DL CC��
     *             ��type=1ʱ��Э��ȡֵ��ΧΪ[0,31]��ʵ��ȡֵΪ[0,����Ʒ֧�ֵ�DL CC�� - 1]
     *     <Para2>: ����ֵ��ʮ��������
     *             Para2Ϊ���һ�ι��Ȼ����ϱ���UL CC��
     *             ��type=1ʱ��Э��ȡֵ��ΧΪ[0,31]��ʵ��ȡֵΪ[0,����Ʒ֧�ֵ�UL CC�� - 1]
     *     <Para3>: ����ֵ��ʮ��������
     *             ��type=0ʱ��Para3Ϊ���һ�ι��Ȼ����ϱ���DL Category
     *             ��type=1ʱ��Para3Ϊ���һ�ι��Ȼ����ϱ���DL MIMO
     *     <Para4>: ����ֵ��ʮ��������
     *             ��type=0ʱ��Para4Ϊ���һ�ι��Ȼ����ϱ���UL Category
     *             ��type=1ʱ��Para4Ϊ���һ�ι��Ȼ����ϱ���UL MIMO
     *     <Para5>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para5Ϊ���һ�ι��Ȼ����ϱ���DLĿ�����BandWidthDLö��ֵ
     *     <Para6>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para6Ϊ���һ�ι��Ȼ����ϱ���ULĿ�����BandWidthULö��ֵ
     *     <Para7>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para7Ϊ���Ȼ����ϱ���FR2 DL MIMO
     *     <Para8>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para8Ϊ���Ȼ����ϱ���FR2 UL MIMO
     *     <Para9>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para9Ϊ���Ȼ����ϱ���FR2 DLĿ�����BandWidthDLö��ֵ
     *     <Para10>: ����ֵ��ʮ��������
     *             ��type=0ʱ���޴˲���
     *             ��type=1ʱ��Para10Ϊ���Ȼ����ϱ���FR2 ULĿ�����BandWidthULö��ֵ
     * [ʾ��]:
     *       AT^OVERHEATINGQRY=1
     *       ^OVERHEATINGQRY: 1,1,1,1,1,1,4,4,1,1,4,4
     *       OK
     */
    { AT_CMD_OVER_HEATING_QRY,
      AT_SetOverHeatingQryParam, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^OVERHEATINGQRY", (TAF_UINT8 *)"(0-1)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NRRC����������������
     * [˵��]: ͨ�����������Զ�̬�򿪺͹ر�NRRC��ĳ�����͵Ŀ���������
     * [�﷨]:
     *     [����]: ^NRRCCAPCFG=<mode>,<para1>,<para2>,<para3>,<para4>,<para5>,<para6>,<para7>,<para8>[,<para9>,<para10>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             �д���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NRRCCAPCFG=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ��
     *             0��NR�µĸ����±����ò�������Ӧ����8��para�������������µ����Ч�����ú���Ҫ���������Ч�����AT����Я����������ȥmode������8���򷵻�error��
     *             1��EN-DC�µĸ����±����ò�������Ӧ�����10��para�������������µ����Ч�����ú���Ҫ���������Ч�����AT����Я����������ȥmode������10���򷵻�error��
     *             2��VONR������������Ӧ�����para1�������µ����Ȼ��Ч�����ú���Ҫ���������Ч�����AT����Я����������ȥmode������1���򷵻�error��
     *             3��NR CA�������أ���Ӧ�����para1�������µ����Ȼ��Ч�����ú���Ҫ���������Ч�����AT����Я����������ȥmode������1���򷵻�error��
     *             4��SAģ�ⲻ���ʱ����������Ӧ�����7�����������ü���Ч������ز��������µ�ָ�NVĬ��ֵ�����AT����Я����������ȥmode������7���򷵻�error��
     *             5����������DSS�������ز�������Ӧ�����2���������µ����Ȼ��Ч�����ú���Ҫ���������Ч�����AT����Я����������ȥmode������2���򷵻�error��
     *             �������޺��壬����error��
     *     <para1>: ����ֵ�����ö�Ӧ����NR���������ĵĵ�һ�����
     *             <mode> Ϊ 0��1 �µĲ������壺
     *             NR�������֧��CA�ز�����(maxNumberCCsDL)��
     *             ȡֵ��Χ
     *             1��оƬ֧�ֵ�NR�������CA�ز�������
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             <mode>Ϊ 2 ��VONR�Ĳ������壺
     *             0����ʾ�ر�VONR��
     *             1��ʾ֧��FR1-cd .VoNR;
     *             2��ʾ֧��FR2-VoNR;
     *             3��ʾ֧��FR1-VoNR��FR2-VoNR
     *             <mode>Ϊ 3 ��NR CA�Ĳ������壺
     *             0 ��ʾ�ر�NR CA��
     *             1 ��ʾ����NR CA��
     *             <mode>Ϊ4��SAģ�ⲻ���ʱ���������壺
     *             para1ΪsaInactivityEnable���أ�1��ʾʶ�𵽳�ʱ�������ݻص�idle̬��0��ʾ���ص�idle̬��
     *             <mode>Ϊ5��DSS����rateMatchingLTE-CRS�������أ�0��ʾ���ı��Ʒ����ֵ��1��ʾ��ÿ��Band��rateMatchingLTE-CRS����Ϊ��֧�֡�
     *     <para2>: ����ֵ��NR�������֧��MIMO��(maxMIMOsDL)��
     *             ȡֵ2��4��8��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             <mode>Ϊ4��SAģ�ⲻ���ʱ���������壺
     *             para2ΪdefaultPeriod������defaultPeriod * M���յ�L2��noData��Ϣ����ʶ��Ϊ��ʱ�������ݣ�M��̬��ȡmultipleTimes����Ԫ�أ�
     *             <mode>Ϊ5��DSS����additionalDMRS-DL-Alt�������أ�
     *             0��ʾ���ı��Ʒ����ֵ��1��ʾ��ÿ����ϣ�����NRCA��ENDC��NR-DC��ÿ����ϣ���additionalDMRS-DL-Alt����Ϊ��֧�֡�
     *     <para3>: ����ֵ��NR����FR1���ۺϴ���(maxFR1BwDL)��
     *             ȡֵ[5��оƬ֧�ֵ�����FR1���ۺϴ���]��UE����NR����FR1���ۺϴ���С�ڵ��ڸ�����ֵ��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             ˵����
     *             1������CA��FR1���ۺϴ������������ޡ�����CA��FR1��CC���������ܺͣ����ܺͳ��������ޣ������CA���ϱ������������������ϱ���
     *             2������FR1���ز���non-CA���������������С��Band��ȡ����С����ֵ����ô����ȡ�󣬵�CC����ȡ��Band��ȡ����С����ֵ��
     *             <mode>Ϊ4��SAģ�ⲻ���ʱ���������壺
     *             para3��protectTime���ͷű���ʱ�䣬��λΪ�롣
     *     <para4>: ����ֵ��NR����FR2���ۺϴ���(maxFR2BwDL)��
     *             ȡֵ[50��оƬ֧�ֵ�����FR2���ۺϴ���]��UE����NR����FR2���ۺϴ���С�ڵ��ڸ�����ֵ��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             ˵����
     *             1������CA��FR2���ۺϴ������������ޡ�����CA��FR2��CC���������ܺͣ����ܺͳ��������ޣ������CA���ϱ������������������ϱ���
     *             2������FR2���ز���non-CA���������������С��Band��ȡ����С����ֵ����ô����ȡ�󣬵�CC����ȡ��Band��ȡ����С����ֵ��
     *             <mode>Ϊ4��SAģ�ⲻ���ʱ���������壺
     *             para4��rauAfterRel��1��ʾ�ص�idle��������ע�ᣬ0��ʾ������ע��ͣ����idle̬��
     *     <para5>: ����ֵ��NR�������֧��CA�ز�����(maxNumberCCsUL)��
     *             ȡֵ��Χ[1��оƬ֧�ֵ�NR�������CA�ز�����]��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             <mode>Ϊ4��SAģ�ⲻ���ʱ���������壺
     *             Para5��MultipleTimesNum����ʾmultipleTimes����Ĵ�С��
     *     <para6>: ����ֵ��NR�������֧��MIMO��(maxMIMOsUL)��
     *             ȡֵ1��2��4��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             <mode>Ϊ4��SAģ�ⲻ���ʱ���������壺
     *             Para6�ɱ�ʾΪ0xAABBCCDD��DD��ʾmultipleTimes[0]��CC��ʾ multipleTimes[1]��BB��ʾ multipleTimes[2]��AA��ʾ multipleTimes[3]��
     *     <para7>: ����ֵ��NR����FR1���ۺϴ���(maxFR1BwUL)��
     *             ȡֵ[5��оƬ֧�ֵ�����FR1���ۺϴ���]��UE����NR����FR1���ۺϴ���С�ڵ��ڸ�����ֵ��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             ˵����
     *             1������CA��FR1���ۺϴ������������ޡ�����CA��FR1��CC���������ܺͣ����ܺͳ��������ޣ������CA���ϱ������������������ϱ���
     *             2������FR1���ز���non-CA���������������С��Band��ȡ����С����ֵ����ô����ȡ�󣬵�CC����ȡ��Band��ȡ����С����ֵ��
     *             <mode>Ϊ4��SAģ�ⲻ���ʱ���������壺
     *             Para7�ɱ�ʾΪ0xAABBCCDD��DD��ʾmultipleTimes[4]��CC��ʾ multipleTimes[5]��BB��ʾ multipleTimes[6]��AA��ʾ multipleTimes[7]��
     *             ע�⣬��ʹmultipleTimes���鳤��С��4����7��������ʹ�ã�ҲҪ��AT������ϣ���Ȼ������ʧ�ܡ�
     *     <para8>: ����ֵ��NR����FR2���ۺϴ���(maxFR2BwUL)��
     *             ȡֵ[50��оƬ֧�ֵ�����FR2���ۺϴ���]��UE����NR����FR2���ۺϴ���С�ڵ��ڸ�����ֵ��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             ˵����
     *             1������CA��FR2���ۺϴ������������ޡ�����CA��FR2��CC���������ܺͣ����ܺͳ��������ޣ������CA���ϱ������������������ϱ���
     *             2������FR2���ز���non-CA���������������С��Band��ȡ����С����ֵ����ô����ȡ�󣬵�CC����ȡ��Band��ȡ����С����ֵ��
     *     <para9>: ����ֵ��EN-DC������LTE���֧��CA�ز�����(maxNumberCCsLTE)��
     *             ȡֵ��Χ[1��оƬ֧�ֵ�LTE���CA�ز�����]��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             ˵�����ò����ǿ�ѡ����mode=0����Ҫ���øò��������mode=1��Ҫ���øò�����
     *     <para10>: ����ֵ��EN-DC������LTE���֧��MIMO��(maxMIMOsLTE)��
     *             ȡֵ2��4��8��
     *             0��ȡ�����ƣ��ָ�Ĭ�ϲ�����
     *             ˵�����ò����ǿ�ѡ����mode=0����Ҫ���øò��������mode=1��Ҫ���øò�����
     * [ʾ��]:
     *     �� ����SA����NR�������ƣ����Ʋ���ֵ���û�ָ��
     *       AT^NRRCCAPCFG=0,1,2,20,50,1,1,20,50
     *       OK
     *     �� ȡ��SA����NR��������
     *       AT^NRRCCAPCFG=0,0,0,0,0,0,0,0,0
     *       OK
     *     �� ����EN-DC����NR�������ƣ����Ʋ���ֵ���û�ָ��(˵������Ҫͬʱ�·�EN-DC��������LTE��������AT����)
     *       AT^NRRCCAPCFG=1,1,2,20,50,1,1,20,50,1,2
     *       OK
     *       AT^ERRCCAPCFG=4,2,1
     *       OK
     *     �� ȡ��EN-DC����NR�������� (˵������Ҫͬʱ�·�EN-DC����ȡ��LTE��������AT����)
     *       AT^NRRCCAPCFG=1,0,0,0,0,0,0,0,0,0,0
     *       OK
     *       AT^ERRCCAPCFG=4,0,0
     *       OK
     *     �� ���ÿ���NR CA
     *       AT^NRRCCAPCFG=3,1
     *       OK
     *       ����SA�����ʱ������
     *          AT^NRRCCAPCFG = 4,1,3,180,1,3,0x00040201,0
     *          OK
     *       ˵����
     *       ��1��������ʾsaInactivity��1
     *       ��2��������ʾdefaultPeriod��3
     *       ��3��������ʾprotectTime��180��
     *       ��4��������ʾ��ʾrauAfterRel��1
     *       ��5��������ʾmultipleTimesNum��3�������multipleTimes���鳤��Ϊ3
     *       ��6������0x00040201��01��multipleTimes[0]��02��multipleTimes [1]��04��multipleTimes[2]��00multipleTimes[3]��
     *       ��7������0x04030201��01��multipleTimes[4]��02��multipleTimes [5]��03��multipleTimes[6]��04��multipleTimes[7]��
     *       ע�⣬��ʹmultipleTimes���鳤��Ϊ3����7��������ʹ�ã�ҲҪ��AT������ϣ���Ȼ������ʧ�ܡ�
     */
    { AT_CMD_NRRCCAPCFG,
      AT_SetNrrcCapCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NRRCCAPCFG", (VOS_UINT8 *)"(0-255),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NRRC�������Ʋ�ѯ����
     * [˵��]: ͨ����AT�����ѯNRRCĳ��ģʽ�µ��������������
     * [�﷨]:
     *     [����]: ^NRRCCAPQRY=<mode>
     *     [���]: ִ�гɹ�ʱ��
     *             ^NRRCCAPQRY: < mode >[<para1>,<para2>��<para3>,<para4>,<para5>,<para6>,<para7>,<para8>,<para9>,<para10>]
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRRCCAPQRY=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ��
     *             0��NR
     *             1��EN-DC
     *             2��VONR����
     *             3��NR CA����
     *             5��DSS����
     *             �������޺��壬����error��
     *     <para1>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵ�һ�����
     *     <para2>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵڶ������
     *     <para3>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵ��������
     *     <para4>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵ��������
     *     <para5>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵ��������
     *     <para6>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵ��������
     *     <para7>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵ��������
     *     <para8>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵڰ������
     *     <para9>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵھ������
     *     <para10>: ����ֵ�����ö�Ӧ����ΪNR�����������õĵ�ʮ�����
     * [ʾ��]:
     *     �� ��ѯNRRC EN-DCģʽ�µ���������
     *       AT^NRRCCAPQRY=1
     *       ^NRRCCAPQRY: 1,1,2,20,50,1,1,20,50,1,2
     *       OK
     *     �� ��ѯNRRC CA��������
     *       AT^NRRCCAPQRY=3
     *       ^NRRCCAPQRY: 3,0,0,0,0,0,0,0,0,0,0
     *       OK
     */
    { AT_CMD_NRRCCAPQRY,
      AT_SetNrrcCapQryPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NRRCCAPQRY", (VOS_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��������N2L�����Ż���������
     * [˵��]: ���������ڶ�����������N2L�����Ż����á�����ȡֵ��ο�����ȡֵ��Χ���ã��������ܶԲ����������Ͻ��о�����
     * [�﷨]:
     *     [����]: ^AUTON2LOPTCFG=<SwitchEnable>,<ParaEnable>,<AutoOptIdleSwitch>,<AutoOptConnSwitch>,<RsrpNrLowThreshold>,<SinrNrLowThreshold>,<RsrpLteHighThreshold>,<SinrLteHighThreshold>,<RsrpL2NJdgDelta>,<SinrL2nJdgDelta>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <SwitchEnable>: ����ָʾ�������ã���3��͵�4��Ƿ���Ч
     *             0������Ч
     *             1����Ч
     *     <ParaEnable>: ����ָʾ�������ã���5-10��Ƿ���Ч
     *             0������Ч
     *             1����Ч
     *     <AutoOptIdleSwitch>: Idle̬N2L�Ż��ܿ���
     *             0���ر�
     *             1����
     *     <AutoOptConnSwitch>: ����̬N2L�Ż��ܿ���
     *             0���ر�
     *             1����
     *     <RsrpNrLowThreshold>: NRС���ź�RSRP���ڴ������������Ż�,��λdbm, ����ȡֵ��Χ[-156,-31]
     *     <SinrNrLowThreshold>: NRС���ź�SINR���ڴ������������Ż�,��λdbm, ����ȡֵ��Χ[-23,40]
     *     <RsrpLteHighThreshold>: LTEС���ź�RSRP���ڴ������������Ż�,��λdbm, ����ȡֵ��Χ[-141,-44]
     *     <SinrLteHighThreshold>: LTEС���ź�SINR���ڴ������������Ż�,��λdbm, ����ȡֵ��Χ[-23,40]
     *     <RsrpL2NJdgDelta>: L2N��ѡ���߲ⱨ�ж�ʱ, NRС����RSRP��Ҫ����߳���Deltaֵ�Ż�������, ��λdbm, ����ȡֵ��Χ[0,10]
     *     <SinrL2nJdgDelta>: L2N��ѡ���߲ⱨ�ж�ʱ, NRС����SINR��Ҫ����߳���Deltaֵ�Ż�������, ��λdbm, ����ȡֵ��Χ[0,10]
     * [ʾ��]:
     *     �� ���ÿ���������Ч
     *       AT^AUTON2LOPTCFG=1,0,0,1,-100,30,-60,20,5,8
     *       OK
     *     �� ���ò���������Ч
     *       AT^AUTON2LOPTCFG=0,1,0,1,-70,10,-55,-3,2,4
     *       OK
     */
    { AT_CMD_AUTON2LOPTCFG,
      AT_SetAutoN2LOptCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^AUTON2LOPTCFG", (VOS_UINT8 *)"(0-1),(0-1),(0-1),(0-1),(@rsrpNrLowThresh),(@sinrNrLowThresh),(@rsrpLteHighThresh),(@sinrLteHighThresh),(@rsrpLnJdgDelta),(@sinrLnJdgDelta)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����NR��UE�������ݲ���
     * [˵��]: ����������AP�ཫ�ƶ˻��߰汾Ԥ�õ���PLMN����NR��UE�������ݲ����·���MODEM����^NRRCUECAPINFONTF�������ʹ�á�
     *         ע��AT�����ܳ��Ȳ�����1600��
     * [�﷨]:
     *     [����]: ^NRRCUECAPPARAMSET=<seq>,<EndFlag>,<CapParamNum>,<para1>[,<para2>[,<para3>]]
     *     [���]: ִ����ȷʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NRRCUECAPPARAMSET=?
     *     [���]: <CR><LF>^NRRCUECAPPARAMSET: (list of supported <Seq>s),(list of supported <EndFlag>s),(list of supported <CapParamNum>s),<para>,<para>,<para><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Seq>: ���ͣ���ˮ�ţ�ȡֵ��Χ��[1,65535]
     *     <EndFlag>: ���ͣ�����������ϱ�ʶ��
     *             0��δ���
     *             1�������
     *     <CapParamNum>: ���ͣ�����AT������UE�������ݲ���������ȡֵ��Χ��[1,3]
     *             ���CapParamNum��������������������Para1��Para2��Para3������������ֱ�ӷ���ʧ�ܡ�
     *     <Para1>: UE�������ݲ�������1���ɽ���������������Ȳ�����560���ҳ��ȱ���Ϊż����
     *     <Para2>: UE�������ݲ�������2���ɽ���������������Ȳ�����560���ҳ��ȱ���Ϊż����
     *     <Para3>: UE�������ݲ�������3���ɽ���������������Ȳ�����560���ҳ��ȱ���Ϊż����
     * [ʾ��]:
     *     �� AP�·�UE�������ݲ������
     *       AT^NRRCUECAPPARAMSET=1,0,3,"1111111111","22222222","3333333333"
     *       OK
     *       AT^NRRCUECAPPARAMSET=2,1,2,"4444444444","55555555"
     *       OK
     *     �� ���Ա�����
     *       AT^NRRCUECAPPARAMSET=?
     *       (1-65535),(0-1),(1-3),(str),(str),(str)
     *       OK
     */
    { AT_CMD_NRRC_UE_CAP_SET,
      AT_SetNrrcUeCapPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^NRRCUECAPPARAMSET", (TAF_UINT8 *)"(1-65535),(0-1),(1-3),(str),(str),(str)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯNRС��CA״̬��Ϣ
     * [˵��]: ���ڲ�ѯNRС�����С�����CA����״̬������ܲ鵽8��CAС����Ϣ��
     *         �����������ģΪNR��LTE����פ������ʱ��ѯ��Ч��SA��NSA�����¿ɲ�ѯ����Ч��NR CA��Ϣ��
     * [�﷨]:
     *     [����]: ^NRCACELL?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^NRCACELL: <total_num>[,<cell_index>,<dl_cfg>,<nul_cfg>,<sul_cfg>[,<cell_index>,<dl_cfg>,<nul_cfg>,<sul_cfg> <[��]]]<CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRCACELL=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <total_num>: ����ֵ�������ϱ���NRС������
     *     <cell_index>: ����ֵ��NRС��������0��ʾPCell������ΪPSCell��Scell��
     *     <dl_cfg>: ����ֵ����С�������Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     *     <nul_cfg>: ����ֵ����С������Nul�Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     *     <sul_flg>: ����ֵ����С������Sul�Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     * [ʾ��]:
     *     �� UEפ����NR����ʱ��ѯNR CA״̬��Ϣ,CAС��������Ϊ0
     *       AT^NRCACELL?
     *       ^NRCACELL: 2,0,1,1,1,1,1,1,1
     *       OK
     *     �� UEפ����NR����ʱ��ѯNR CA״̬��Ϣ,CAС������Ϊ0
     *       AT^NRCACELL?
     *       ^NRCACELL: 0
     *       OK
     *     �� ִ�в�������
     *       AT^NRCACELL=?
     *       OK
     */
    { AT_CMD_NRCACELL,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryNrCaCellPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NRCACELL", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����NRС��CA��Ϣ�����ϱ�
     * [˵��]: ���������ڿ���UE��פ��NR��LTE NSA����ʱ��CA��Ϣ�仯���Ƿ������ϱ�^NRCACELLRPT�������֪ͨAP��ǰģʽ��С��NR CA״̬��Ϣ�仯�����óɹ�������Ч�����˷���ģʽ��Ȼ��Ч���µ��ʧЧ�������ϵ�Ĭ�Ϲر������ϱ���
     * [�﷨]:
     *     [����]: ^NRCACELLRPTCFG=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRCACELLRPTCFG?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^NRCACELLRPTCFG: <enable><CR><LF> <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRCACELLRPTCFG=?
     *     [���]: <CR><LF>^NRCACELLRPTCFG:(0,1)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ�������ϱ�^NRCACELLRPT����Ŀ��أ�
     *             0���ر�^NRCACELLRPT�����ϱ���
     *             1����^NRCACELLRPT�����ϱ���CA��Ϣ�б仯���ϱ���CA��Ϣ�仯�����������޵��У����е��޺ʹ��е��С����е����Ƿ�仯����cellIdx�жϡ�
     * [ʾ��]:
     *     �� ��^NRCACELLRPT�����ϱ�
     *       AT^NRCACELLRPTCFG=1
     *       OK
     *     �� ��ѯ^NRCACELLRPT�����ϱ�����״̬
     *       AT^NRCACELLRPTCFG?
     *       ^NRCACELLRPTCFG: 1
     *       OK
     *     �� ִ�в�������
     *       AT^NRCACELLRPTCFG=?
     *       ^NRCACELLRPTCFG: (0,1)
     *       OK
     */
    { AT_CMD_NRCACELLRPTCFG,
      AT_SetNrCaCellRptCfgPara, AT_SET_PARA_TIME, AT_QryNrCaCellRptCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR,
      AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NRCACELLRPTCFG", (TAF_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR�������������ϱ�����
     * [˵��]: ͨ����AT�����NR��ĳ�����������仯�������ϱ���
     * [�﷨]:
     *     [����]: ^NRNWCAPRPTCFG=<type>,<switch>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRNWCAPRPTCFG=?
     *     [���]: <CR><LF>^NRCACELLRPTCFG:(0
     *             -255),(0,1)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: ����ֵ��ȡֵ0��255��
     *             1�������SA���������ϱ�����
     *             �����������ر��壬Ԥ��ʹ��
     *     <switch>: 0���ر������ϱ���
     *             1���������ϱ���
     * [ʾ��]:
     *     �� �ر������ϱ���ǰ����SA�����仯
     *       AT^NRNWCAPRPTCFG=1,0
     *       OK
     */
    { AT_CMD_NRNWCAPRPTCFG,
      AT_SetNrNwCapRptCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR,
      AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NRNWCAPRPTCFG", (TAF_UINT8 *)"(0-255),(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR�������������ϱ����ز�ѯ
     * [˵��]: ͨ����AT�����ѯNR��ĳ���������������ϱ������
     * [�﷨]:
     *     [����]: ^NRNWCAPRPTQRY=<type>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^NRNWCAPRPTQRY: < type>,<switch><CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRNWCAPRPTQRY=?
     *     [���]: <CR><LF>^NRNWCAPRPTQRY:(0-255)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: ����ֵ��ȡֵ0��255��
     *             1������SA�����仯�����ϱ�����
     *             �����������ر��壬Ԥ��ʹ��
     *     <switch>: ����ֵ��ȡֵ��Χ:
     *             0�������ϱ�Ϊ�ر�״̬;
     *             1�������ϱ�Ϊ����״̬��
     * [ʾ��]:
     *     �� ��ѯ����SA�����仯�����ϱ����أ� ��ǰ�ϱ����ؿ���
     *       AT^NRNWCAPRPTQRY=1
     *       ^NRNWCAPRPTQRY: 1,1
     *       OK
     */
    { AT_CMD_NRNWCAPRPTQRY,
      AT_SetNrNwCapRptQryPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR,
      AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NRNWCAPRPTQRY", (TAF_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR����������ѯ
     * [˵��]: ͨ����AT�����ѯNR��ĳ���������������
     * [�﷨]:
     *     [����]: ^NRNWCAPQRY=<type>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^NRNWCAPQRY: <type>,<para1>,<para2>��<para3>,<para4>,<para5>,<para6>,<para7>,<para8>,<para9>,<para10><CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRNWCAPQRY=?
     *     [���]: <CR><LF>^NRNWCAPQRY:(0-255)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: ����ֵ��ȡֵ0��255��
     *             1����ǰ����SA��������Ӧ��ѯ���<para1>��Ч����ǰ��֧�֡�
     *             �����������ر��壬Ԥ��ʹ��
     *     <para1>: ����ֵ����ӦNR���������ĵ�һ�����
     *             <type>Ϊ1ʱ��ȡֵ��Χ:
     *             0�����粻֧��SA;
     *             1������֧��SA��
     *     <para2>: ����ֵ����ӦNR���������ĵڶ������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para3>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para4>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para5>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para6>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para7>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para8>: ����ֵ����ӦNR���������ĵڰ������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para9>: ����ֵ����ӦNR���������ĵĵھ������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para10>: ����ֵ����ӦNR���������ĵ�ʮ�����
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     * [ʾ��]:
     *     �� ��ѯ����SA����������ǰ����֧��SA(����Ϊ����ʾ������������ʵ֧�����)
     *       AT^NRNWCAPQRY=1
     *       ^NRNWCAPQRY: 1,1,0,0,0,0,0,0,0,0,0
     *       OK
     */
    { AT_CMD_NRNWCAPQRY,
      AT_SetNrNwCapQryPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR,
      AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NRNWCAPQRY", (TAF_UINT8 *)"(0-255)" },
#endif
#endif

    { AT_CMD_RSRP,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryRsrp, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+RSRP", VOS_NULL_PTR },

    { AT_CMD_RSRQ,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryRsrq, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+RSRQ", VOS_NULL_PTR },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR�͹�����������
     * [˵��]: ��������������NRģʽ�µĽ����͹������ò���
     * [�﷨]:
     *     [����]: ^NRPOWERSAVINGCFG=<cmd_type>[,<para1>,<para2>,<para3>,��������,<para23>]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRPOWERSAVINGCFG=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cmd_type>: ����ֵ��ȡֵ0��1
     *             0����ʱԤ�������ܲ���Ч��
     *             1���͹����������ͣ�RRC�����ͷţ�����MODEM������������Ч�����غ����µ����ûᱻ���������ĿǰЯ��4��para������
     *             �������޺��壬����error��
     *     <para1>: ����ֵ�����ö�Ӧ�͹������õĵ�һ�����
     *             <cmd_type> Ϊ1 �µĲ������壺
     *             �����ͷ������Ƿ���Ч�ı�־
     *             ȡֵ��Χ0~1
     *             0�������ͷ����Թر�
     *             1�������ͷ����Դ�
     *     <para2>: ����ֵ�����ö�Ӧ�͹������õĵڶ������
     *             <cmd_type> Ϊ1 �µĲ������壺
     *             �����ͷ�����������Ч�ı�־
     *             ȡֵ��Χ0~1
     *             0�������ͷ�������Чδʹ��
     *             1�������ͷ�������Чʹ�ܣ�UE����ִ�п����ͷ�һ��
     *     <para3>: ����ֵ�����ö�Ӧ�͹������õĵ��������
     *             <cmd_type> Ϊ1 �µĲ������壺
     *             �����ͷ�����������UE״̬����
     *             ȡֵ��Χ0~2
     *             0��IDLE̬
     *             1��INACTIVE̬
     *             2��CONN̬
     *             �������޺��壬����error��
     *     <para4>: ����ֵ�����ö�Ӧ�͹������õĵ��������
     *             <cmd_type> Ϊ1 �µĲ������壺
     *             �����ͷ����Լ������������ʱ������λ��
     *             ȡֵ��Χ0~4,294,967,295
     *             0�����տ����ͷ����Թرմ������ټ��������
     *             ��������������ʱ��Ϊ���ڼ��������
     *     <para5>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para6>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para7>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para8>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para9>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para10>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para11>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para12>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para13>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para14>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para15>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para16>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para17>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para18>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para19>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para20>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para21>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para22>: ����ֵʮ���ƣ����޺��壬Ԥ��
     *     <para23>: ����ֵʮ���ƣ����޺��壬Ԥ��
     * [ʾ��]:
     *     �� ���õ͹������Կ����ͷ�����
     *       AT^NRPOWERSAVINGCFG=1,1,0,0,2
     *       OK
     *       ��1������1��ʾ�������������ǿ����ͷ�����
     *       ��2������1��ʾ�����ͷ�����ʹ��
     *       ��3������0��ʾ��������Ч�����ͷ�
     *       ��4������0��ʾ�����ͷŵ�����״̬��IDLE
     *       ��5������2��ʾ��������ݵ�����Ϊ2��
     */
    { AT_CMD_NRPOWERSAVINGCFG,
      AT_SetNrPowerSavingCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NRPOWERSAVINGCFG", (VOS_UINT8 *)"(0-65535),(0-4294967295),(0-4294967295),(0-4294967295),\
(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),\
(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),\
(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR�͹��Ĳ�ѯ����
     * [˵��]: ���������ڲ�ѯNRģʽ�µĽ����͹������ò���
     * [�﷨]:
     *     [����]: ^NRPOWERSAVINGQRY=<cme_type>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>^NRPOWERSAVINGQRY��<cmd_type>,<para1>,<para2>,����<para23>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^NRPOWERSAVINGQRY=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cme_type>: ȡֵ��Χ0~65535
     *             0����ʱԤ��������Ч
     *             1����ѯRRC�����ͷŲ���
     *             ����
     *     <para1>: cmd_type��Ӧ��ѯ�Ľ��������������޴˲�����Ĭ��Ϊ0
     *     <para2>: cmd_type��Ӧ��ѯ�Ľ��������������޴˲�����Ĭ��Ϊ0
     *     <����>: ����
     *     <para23>: cmd_type��Ӧ��ѯ�Ľ��������������޴˲�����Ĭ��Ϊ0
     * [ʾ��]:
     *       AT^NRPOWERSAVINGQRY=0
     *       ^NRPOWERSAVINGQRY: 1,1,2,20,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     *       OK
     */
    { AT_CMD_NRPOWERSAVINGQRY,
      AT_SetNrPowerSavingQryPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NRPOWERSAVINGQRY", (VOS_UINT8 *)"(0-65535)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯС��SSB��Ϣ
     * [˵��]: �����������ṩNRС����Ӧ��SSB��Ϣ��SSB��Ϣ��������С��������С����SSB��Ϣ���ϲ㽫ʹ����Щ��Ϣ��֧��λ�õı�ʶ������֧��NR������ģʽ����Ч��
     *         ���������֧��NR�����Ĳ�Ʒ�Ͽ��á�
     *         SA�����£������������ģ����̬�����������ò���ʱ��Ч��
     *         ENDC�����£����������ENDC�����ɹ������������ò���ʱ��Ч��
     * [�﷨]:
     *     [����]: ^NRSSBID?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^NRSSBID: <ARFCN-NR>,<CID>,<PCI>,<RSRP>,
     *             <SINR>,<TA>,<Scell_description>,<N_NB_CELL>[,<Ncell_description>]
     *             <CR><LF><CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err code><CR><LF>
     * [����]:
     *     <Scell_description>: ����С��SSB��Ϣ��Beam�������������Ҫ�����������ϱ����������
     *             <S_SSBID0>,<S_ID0_RSRP>,<S_SSBID1>,<S_ID1_RSRP>, <S_SSBID2>,<S_ID2_RSRP>,<S_SSBID3>,<S_ID3_RSRP>, <S_SSBID4>,<S_ID4_RSRP>,<S_SSBID5>,<S_ID5_RSRP>, <S_SSBID6>,<S_ID6_RSRP>,<S_SSBID7>,<S_ID7_RSRP>
     *     <Ncell_description>: ��С��SSB��Ϣ��Beam�������������Ҫ����������������ϱ�����������ϱ�ʱ����ע������������reportCfg�е����޻��߿�������С����  [,<NB_PCI>,<NB_ARFCN>,<NB_RSRP>,<NB_SINR>, <NB_SSBID0>,<NB_ID0_RSRP>,<NB_SSBID1>, <NB_ID1_RSRP>,<NB_SSBID2>,<NB_ID2_RSRP>, <NB_SSBID3>,<NB_ID3_RSRP>]*N_NB_CELL
     *     <ARFCN-NR>: �޷���32λ����ֵ��NR����С��SSBƵ�㣬��Чֵ��0xFFFFFFFF
     *     <CID>: 8���ֽڣ�16���ַ���16���ƣ���ʾ��ǰ����С��ID��ʹ��Half-Byte�루���ֽڣ����룬����000000010000001A����ʾ��4�ֽ���0x00000001����4�ֽ���0x0000001A��ע��NSA���ϱ�����LTEС����CID��SA��Ҳ��һ���ܲ鵽�����ѯʱ����ء���Чֵ��0xFFFFFFFFFFFFFFFF
     *     <PCI>: �޷���16λ����ֵ����ǰ��������С��ID����Чֵ[0,1007]����Чֵ:0xFFFF
     *     <RSRP>: �з���16λ����ֵ����ǰ����С���ο��źŽ��չ��ʡ�
     *             ��Чֵ��0x7FFF
     *     <SINR>: �з���16λ����ֵ����ǰ����С���źŸ��������ȡ�
     *             ��Чֵ��0x7FFF
     *     <TA>: �з���32λ����ֵ������С����ǰʱ����ǰ������Чֵ��-1��
     *     <S_SSBIDx>: �޷���8λ����ֵ��������SSB ID��ID��0��7��8��������Чֵ��;0xFF
     *     <S_IDx_RSRP>: �з���16λ����ֵ��������SSBID��Ӧ�Ĳο��źŽ��չ��ʣ�ID��0��7��8��������Чֵ��0x7FFF
     *     <N_NB_CELL>: �޷���8λ����ֵ������̬NR�������������4������Beam���������ǰ4��С���������û����0,�ϲ�ʶ��ò���Ϊ0ʱ�����ٶ�ȡ������ֵ��
     *     <NB_PCI>: �޷���16λ����ֵ����������С��ID����Чֵ��0xFFFF
     *     <NB_ARFCN>: �޷���32λ����ֵ������С��SSBƵ�㣬��Чֵ��0xFFFFFFFF
     *     <NB_RSRP>: �з���16λ����ֵ�������ο��źŽ��չ��ʡ���Чֵ��0x7FFF
     *     <NB_SINR>: �з���16λ����ֵ�������ο��źŽ��չ��ʡ���Чֵ��0x7FFF
     *     <NB_SSBIDx>: �޷���8λ����ֵ������SSB ID��IDֵ��Χ0��7���ϱ���SSBIDΪ����RSRP�����������ǰ4��������Чֵ��0xFF
     *     <NB_IDx_RSRP>: �з���16λ����ֵ������SSBID��ID��0��7��������4������Ӧ�Ĳο��źŽ��չ��ʡ���Чֵ��0x7FFF
     * [ʾ��]:
     *     �� ��SA�ϲ�ѯNRSSBID:
     *       AT^NRSSBID?
     *       ^NRSSBID=430010,00000001000000d5,110,-83,-10,-1,1,-50,2,-60,255,32767,255,32767,255,32767,255,32767,255,32767,255,32767,2,142,430040,-50,-15,3,-43,2,-55,4,-56,255,32767,143,430036,-51,-16,4,-43,2,-55,1,-56,255,32767
     *       OK
     *     �� ��NSA�ϲ�ѯNRSSBID:
     *       AT^NRSSBID?
     *       ^NRSSBID=430010,0000000000000087,110,-83,-10,-1,1,-50,2,-60,255,32767,255,32767,255,32767,255,32767,255,32767,255,32767,1,142,430040,-50,-15,3,-43,2,-55,4,-56,255,32767
     *       OK
     */
    { AT_CMD_NRSSBID,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryNrSsbIdPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NRSSBID", VOS_NULL_PTR },
#endif

    { AT_CMD_NCELLMONITOR,
      AT_SetNCellMonitorPara, AT_SET_PARA_TIME, AT_QryNCellMonitorPara, AT_QRY_PARA_TIME, AT_TestNCellMonitorPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NCELLMONITOR", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_ANQUERY,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryAnQuery, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^ANQUERY", VOS_NULL_PTR },

#if (FEATURE_LTE == FEATURE_ON)
    { AT_CMD_LFROMCONNTOIDLE,
      At_SetLFromConnToIdlePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^LFROMCONNTOIDLE", VOS_NULL_PTR },
#endif

    /*
     * [���]: Э��AT-LTE���
     * [����]: LTE CA��С����Ϣ��ѯ����
     * [˵��]: ���������ڲ�ѯLTE CA״̬��SCELL��Ϣ����ǰ���֧��4��SCELL��Ϣ��ѯ��
     * [�﷨]:
     *     [����]: ^CASCELLINFO?
     *     [���]: ^CASCELLINFO: <index>,<pci>,<rssi>,<rsrp>,<rsrq>,<band_class>,<ulfreqpoint>,<dlfreqpoint>,<ulfreq>,<dlfreq>,<ulbw>,<dlbw><CR><LF>[[^CASCELLINFO: <index>,<pci>,<rssi>,<rsrp>,<rsrq>,<band_class>,<ulfreqpoint>,<dlfreqpoint>,<ulfreq>,<dlfreq>,<ulbw>,<dlbw><CR><LF>][��]]<CR><LF>OK<CR><LF>
     *             ����������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <index>: ����ֵ��ָʾ��ǰSCELLС������ֵ��ȡֵ��Χ1~4��
     *     <pci>: ����ֵ��ָʾ��ǰSCELLС��PCI��
     *     <rssi>: ����ֵ��ָʾ��ǰSCELLС��rssiֵ��
     *             ȡֵ���£�
     *             -120 ~ -25, ��λdBm��
     *     <rsrp>: ����ֵ��ָʾ��ǰSCELLС��rsrpֵ��
     *             ȡֵ���£�
     *             -140 ~ -44, ��λdBm��
     *     <rsrq>: ����ֵ��ָʾ��ǰSCELLС��rsrqֵ��
     *             ȡֵ���£�
     *             -19~ -3, ��λdB��
     *     <band_class>: ����ֵ����ʾ��ǰ�ն�����С����Ӧ��Ƶ�Ρ�
     *     <ulfreqpoint>: ����ֵ��ָʾ��ǰSCELLС������Ƶ�㡣
     *     <dlfreqpoint>: ����ֵ��ָʾ��ǰSCELLС������Ƶ�㡣
     *     <ulfreq>: ����ֵ��ָʾ��ǰSCELLС������Ƶ�ʣ���λΪ(100kHz)��
     *     <dlfreq>: ����ֵ��ָʾ��ǰSCELLС������Ƶ�ʣ���λΪ(100kHz)��
     *     <ulbw>: ����ֵ��ָʾ��ǰSCELLС�����д���
     *             ȡֵ���£�
     *             0����ʾ1.4M����
     *             1����ʾ3M����
     *             2����ʾ5M����
     *             3����ʾ10M����
     *             4����ʾ15M����
     *             5����ʾ20M����
     *     <dlbw>: ����ֵ��ָʾ��ǰSCELLС�����д���ȡֵ���£�
     *             0����ʾ1.4M����
     *             1����ʾ3M����
     *             2����ʾ5M����
     *             3����ʾ10M����
     *             4����ʾ15M����
     *             5����ʾ20M����
     * [ʾ��]:
     *     �� ��������:
     *       AT^CASCELLINFO=?
     *       OK
     *     �� CA����ʱ��ѯ���2CC CA��
     *       AT^CASCELLINFO?
     *       ^CASCELLINFO: 1,417,-60,-80,-5,18,23925,5925,8225,8675,5,5
     *       OK
     *     �� CAδ����ʱ��ѯ����:
     *       AT^CASCELLINFO?
     *       ERROR
     *     �� CA����ʱ��ѯ���3CC CA��:
     *       AT^CASCELLINFO?
     *       ^CASCELLINFO: 1,417,-60,-80,-5,18,23925,5925,8225,8675,3,3
     *       ^CASCELLINFO: 2,418,-61,-81,-7,18,23925,5925,8225,8675,3,3
     *        OK
     */
    { AT_CMD_CASCELLINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryCaScellInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CASCELLINFO", VOS_NULL_PTR },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /* Э������,��HSPA�����÷�Χ��(0-3),����Ϊ(0-7),����4��ӦR8,5��ӦR9���������������Ժ����չ */
    { AT_CMD_HSPA,
      At_SetRRCVersion, AT_SET_PARA_TIME, At_QryRRCVersion, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HSPA", (VOS_UINT8 *)"(0-7)" },
#endif
};

/* ע��AS����AT����� */
VOS_UINT32 AT_RegisterCustomAsCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomAsCmdTbl, sizeof(g_atCustomAsCmdTbl) / sizeof(g_atCustomAsCmdTbl[0]));
}

