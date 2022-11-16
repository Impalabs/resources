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
#include "at_custom_taf_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_taf_set_cmd_proc.h"
#include "at_custom_taf_qry_cmd_proc.h"
#include "at_custom_mm_qry_cmd_proc.h"
#include "at_custom_mm_set_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_TAF_CMD_TBL_C

static const AT_ParCmdElement g_atCustomTafCmdTbl[] = {
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��������modem�������Ϣ
     * [˵��]: ���������ڽ�������ָ��modem�������Ϣ��
     *         ע����ǰֻ��������modem��NV 2613 (EquivalentPlmn)��NV 8451 (Taf_PdpPara_0_R7)ֵ
     * [�﷨]:
     *     [����]: ^EXCHANGEMODEMINFO=<MODEMID1>,<MODEMID2>
     *     [���]: <CR><LF>OK<CR><LF>
     *             �����������ֵ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^EXCHANGEMODEMINFO=?
     *     [���]: <CR><LF>^EXCHANGEMODEMINFO: (0-2),(0-2)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <MODEMID1>: ��һ��modem ID��ȡֵ��Χ0~2
     *     <MODEMID2>: �ڶ���modem ID��ȡֵ��Χ0~2
     * [ʾ��]:
     *     �� ����modem0��modem1����Ϣ
     *       AT^EXCHANGEMODEMINFO=0,1
     *       OK
     *     �� ��������
     *       AT^EXCHANGEMODEMINFO=?
     *       ^EXCHANGEMODEMINFO: (0-2),(0-2)
     *       OK
     */
    { AT_CMD_EXCHANGE_MODEM_INFO,
      AT_ExchangeModemInfo, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^EXCHANGEMODEMINFO", (TAF_UINT8 *)"(0-2),(0-2)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����MODEM��AT�����ϱ��˿�
     * [˵��]: AP-Modem��̬�£�AP��ҪMODEM֧�ֶ�̬ѡ�����������ϱ��Ķ˿ڣ���ѡ�����ж˿ڶ��ϱ�����ֻ��ĳ������ָ���Ķ˿��ϱ���
     *         AP���Ը����Լ�����������ã���ָ���˿ڵ������ϱ���ر�ָ���˿ڵ������ϱ���
     *         ������ֻ����AP�Խӵ�ATͨ���յ���Ч������ATͨ�����յ�����Ϊʧ�ܡ�
     *         ÿ�ο�������Ҫ�������ã��ػ���ʧЧ��
     * [�﷨]:
     *     [����]: ^APRPTPORTSEL=<portsel1>,<portsel2>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^APRPTPORTSEL?
     *     [���]: <CR><LF>^APRPTPORTSEL: <portsel1>, <portsel2><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^APRPTPORTSEL=?
     *     [���]: <CR><LF>^APRPTPORTSEL: (0-FFFFFFFF), (0-FFFFFFFF)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <portselOne>: ����ֵ���ϱ��˿�ѡ�񡣸��ֶΰ���4���ֽڣ�ÿһ��bitλ��ʶһ���˿ڡ�
     *     <portselTwo>: ����ֵ���ϱ��˿�ѡ�񡣸��ֶΰ���4���ֽڣ�ÿһ��bitλ��ʶһ���˿ڡ�
     * [��]: portselOne��bit0-31��Ӧ�˿�
     *       ,     1Oct,  1Oct,  1Oct,  1Oct, 1Oct, 1Oct,  1Oct,    1Oct,
     *       Bit,  0,     1,     2,     3,    4,    5,     6,       7,
     *       �˿�, PCUI,  CTRL,  MODEM, NDIS, UART, SOCK,  APPSOCK, HSIC1,
     *       Bit,  8,     9,     10,    11,   12,   13,    14,      15,
     *       �˿�, HSIC2, HSIC3, HSIC4, MUX1, MUX2, MUX3,  MUX4,    MUX5,
     *       Bit,  16,    17,    18,    19,   20,   21,    22,      23,
     *       �˿�, MUX6,  MUX7,  MUX8,  APP,  APP1, APP2,  APP3,    APP4,
     *       Bit,  24,    25,    26,    27,   28,   29,    30,      31,
     *       ȡֵ, APP5,  APP6,  APP7,  APP8, APP9, APP10, APP11,   APP12,
     * [��]: portselTwo��bit0-31��Ӧ�˿�
     *       ,     1Oct,  1Oct,  1Oct,      1Oct, 1Oct, 1Oct, 1Oct, 1Oct,
     *       Bit,  0,     1,     2,         3,    4,    5,    6,    7,
     *       �˿�, APP13, APP14, HSICMODEM, Ԥ��, Ԥ��, Ԥ��, Ԥ��, Ԥ��,
     *       Bit,  8,     9,     10,        11,   12,   13,   14,   15,
     *       �˿�, Ԥ��,  Ԥ��,  Ԥ��,      Ԥ��, Ԥ��, Ԥ��, Ԥ��, Ԥ��,
     *       Bit,  16,    17,    18,        19,   20,   21,   22,   23,
     *       �˿�, Ԥ��,  Ԥ��,  Ԥ��,      Ԥ��, Ԥ��, Ԥ��, Ԥ��, Ԥ��,
     *       Bit,  24,    25,    26,        27,   28,   29,   30,   31,
     *       ȡֵ, Ԥ��,  Ԥ��,  Ԥ��,      Ԥ��, Ԥ��, Ԥ��, Ԥ��, Ԥ��,
     * [ʾ��]:
     *     �� ����MODEM��AT�����ϱ��˿�ΪPCUI�˿�
     *       AT^APRPTPORTSEL=1,0
     *       OK
     *     �� ����HSIC1,2��Ҫ�����ϱ��������˿ڶ��������ϱ�
     *       AT^APRPTPORTSEL=180,0
     */
    { AT_CMD_APRPTPORTSEL,
      AT_SetApRptPortSelectPara, AT_NOT_SET_TIME, AT_QryApRptPortSelectPara, AT_NOT_SET_TIME, AT_TestApRptPortSelectPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^APRPTPORTSEL", (VOS_UINT8 *)"(@PortSelOne),(@PortSelTwo),(@PortSelThree)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����GPS�Ƿ�֧�����������λ��Ϣ����
     * [˵��]: ��������������GPSоƬ�Ƿ�֧�����������λ��Ϣ���ܣ��ϵ翪��Ĭ��ΪGPSоƬ��֧�����������λ��Ϣ���ܡ�
     *         ���������λ��Ϣ������Ҫ��������������GPS��λ����ʱ�����GPSоƬ����ĸ�����λ��Ϣ����������֮�以����š�
     * [�﷨]:
     *     [����]: ^XCPOSR=<support>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^XCPOSR?
     *     [���]: <CR><LF>^XCPOSR: <support><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^XCPOSR=?
     *     [���]: <CR><LF>
     *             ^XCPOSR: (list of supported <support>s)
     *             <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <support>: ����ֵ���Ƿ�֧�����������λ��Ϣ���ܣ�ȡֵ��Χ0~1��
     *             0����֧�����������λ��Ϣ���ܣ�
     *             1��֧�����������λ��Ϣ���ܡ�
     * [ʾ��]:
     *     �� ����GPSоƬ֧�����������λ��Ϣ����
     *       AT^XCPOSR=1
     *       OK
     *     �� ��ѯGPSоƬ�Ƿ�֧�����������λ��Ϣ����
     *       AT^XCPOSR?
     *       ^XCPOSR: 1
     *       OK
     *     �� ��������
     *       AT^XCPOSR=?
     *       ^XCPOSR: (0,1)
     *       OK
     */
    { AT_CMD_XCPOSR,
      AT_SetXcposrPara, AT_SET_PARA_TIME, AT_QryXcposrPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^XCPOSR", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����Ƿ������ϱ����������λ��Ϣָʾ
     * [˵��]: ��������������Modem �Ƿ������ϱ����������λ��Ϣ��ָʾ��GPSоƬ���ϵ翪��Ĭ��Ϊ�����������ϱ���
     *         ��������Ҫ������GPSоƬ֧�����������λ��Ϣ����ʱ������Modem�Ƿ������ϱ����������λ��Ϣ��ָʾ��GPSоƬ��
     * [�﷨]:
     *     [����]: ^XCPOSRRPT=<mode>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^XCPOSRRPT?
     *     [���]: <CR><LF>^XCPOSRRPT: <mode><CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: ^XCPOSRRPT=?
     *     [���]: <CR><LF>^XCPOSRRPT: (list of supported <mode>s)<CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ���Ƿ��������������λ��Ϣָʾ�������ϱ����ܣ�ȡֵ��Χ0~1��Ĭ��ֵΪ0��
     *             0���ر������ϱ����ܣ�
     *             1�����������ϱ����ܡ�
     * [ʾ��]:
     *     �� �����������������λ��Ϣָʾ�������ϱ����ܣ�
     *       AT^XCPOSRRPT=1
     *       OK
     *     �� ��ѯ�Ƿ��������������λ��Ϣָʾ�������ϱ����ܣ�
     *       AT^XCPOSRRPT?
     *       ^XCPOSRRPT: 1
     *       OK
     *     �� ִ�в�������
     *       AT^XCPOSRRPT=?
     *       ^XCPOSRRPT: (0,1)
     *       OK
     */
    { AT_CMD_XCPOSRRPT,
      AT_SetXcposrRptPara, AT_SET_PARA_TIME, AT_QryXcposrRptPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^XCPOSRRPT", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����GPS��Ϣ
     * [˵��]: ����GPS��Ϣ�������߶ȡ����ȡ�γ�ȡ��ٶȺ�ʱ�䡣
     *         �����������AP��Modem���͵�ǰGPS��Ϣ��������Modem�����״̬��
     * [�﷨]:
     *     [����]: ^CGPSINFO[=[<ALTITUDE>[,<LONGITUDE>[,<LATITUDE>[,<SPEED>[,<TIME>]]]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CGPSINFO=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <ALTITUDE>: �ַ������ͣ��߶���Ϣ��
     *     <LONGITUDE>: �ַ������ͣ�������Ϣ��
     *     <LATITUDE>: �ַ������ͣ�γ����Ϣ��
     *     <SPEED>: �ַ������ͣ��ٶ���Ϣ��
     *     <TIME>: �ַ������ͣ�ʱ����Ϣ��
     * [ʾ��]:
     *     �� ����GPS��Ϣ
     *       AT^CGPSINFO="0.0","121.6269374","31.2641758","0.0","201208161030"
     *       OK
     *     �� ��������
     *       AT^CGPSINFO=?
     *       OK
     */
    { AT_CMD_CGPSINFO,
      AT_SetGpsInfoPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CGPSINFO", (VOS_UINT8 *)"(@altitude),(@longitude),(@latitude),(@speed),(@time)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���ý������LOG�������
     * [˵��]: ����������AP-Modem��̬��ͨ��LOG2.0���ƣ�����ά�ɲ���Ϣ������AP���SD���ϡ�
     *         ����ʹ�����ƣ�������LOG2.0���ƵĿ�ά�ɲ������ӳ�д�빦�ܡ�
     * [�﷨]:
     *     [����]: ^LOGSAVE
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������<CR><LF>+CME ERROR:<err><CR><LF>
     * [ʾ��]:
     *     �� ��Modem�໺���LOG���������AP��
     *       AT^LOGSAVE
     *       OK
     */
    { AT_CMD_LOGSAVE,
      AT_SetLogSavePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LOGSAVE", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ/�����ϱ�MIPICLKɸѡ���
     * [˵��]: ��ѯ��ǰMIPICLK��ɸѡ�����RF����Ƶ�ʸı�������ϱ�MIPICLKɸѡ�����
     * [�﷨]:
     *     [����]: ^MIPICLK
     *     [���]: <CR><LF>^MIPICLK <bitmap><CR><LF>
     *     [����]: ^MIPICLK?
     *     [���]: <CR><LF>^MIPICLK <bitmap><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR <err><CR><LF>
     *     [����]: ^MIPICLK=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <bitmap>: ����ֵ��MIPICLKλͼ��
     * [ʾ��]:
     *       RF����Ƶ��ı�������ϱ�MIPICLKɸѡ���
     *       ^MIPICLK: 12
     *     �� ����ʱ������ѯ���µ�MIPICLKɸѡ���
     *       AT^MIPICLK?
     *       ^MIPICLK: 12
     *       OK
     */
    { AT_CMD_MIPICLK,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryMipiClkValue, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^MIPICLK", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���á���ѯ�������ϱ���������Ƶ�����
     * [˵��]: ʹ�ܻ��ֹ��������Ƶ����ŵ������ϱ����ܣ���ѯ���м�������������Ƶ����ŵ������Ϣ��������������Ƶ����ŷ����仯ʱ�������ϱ��仯��ÿ���ϱ�ֻ�ϱ�һ���仯������Ƶ����ţ����ж��Ƶ����ŷ����仯��ֶ���ָ���ϱ���������ֻ֧��MODEM0����MODEM0��Ӧ��ATͨ���·���ָ����Ч��
     *         ע���:
     *         ������ָ��Я����DeviceID��Ϊ0����NV���õ���������Device����ƥ�䣬�򷵻�ERROR��
     *         ������������ĿΪ0�����ѯ��������������ݣ�ֻ����OK��
     * [�﷨]:
     *     [����]: ^BESTFREQ=<DeviceID>,<Mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>  ERROR<CR><LF>
     *     [����]: ^BESTFREQ?
     *     [���]: [<CR><LF>^BESTFREQ: <GroupID>, <DeviceID>, <CaseID>, <Mode>
     *             [<CR><LF>^BESTFREQ: <GroupID>, <DeviceID>, <CaseID >, <Mode>[��]]]
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^BESTFREQ=?
     *     [���]: <CR><LF>^BESTFREQ: (0-255),(0-1)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: -
     *     [���]: ��������Ƶ����ŷ����仯ʱ��Modem�����ϱ���
     *             [<CR><LF>^BESTFREQ: <GroupID>, <DeviceID>, <CaseID><CR><LF>]
     * [����]:
     *     <Mode>: ����ֵ����ֹ��ʹ��^BESTFREQ���ϱ�֪ͨ   0����ֹ   1��ʹ��
     *     <GroupID>: ����ֵ����ʶ������ţ���Ч��ΧΪ[0~7]���ֱ��Ӧ���ſ���NV�е�8��group�飬ÿ��group�������һ������ID
     *     <DeviceID>: ����ֵ����ʶ����ID����Ч��ΧΪ[0~255]��1-255���ڱ�ʶ�ض�����������һ���ض��ͺŵĲ�Ʒ������ID�ǹ̶��Ҷ�һ�޶��ģ�0���ڱ�ʾ���е�����������^BESTFREQ=<DeviceID>,<Mode>ָ������Ч����������õ�DeviceID����8�������У��򷵻�ERROR
     *     <CaseID>: ����ֵ����ʶ������Ƶ����ţ���Ч��ΧΪ[0~3],��Ӧ���ſ���NV��ÿ��group�е�4��Case�ࡣһ��Case�����������һ��Ԥ��Ĺ���Ƶ���Լ������������16������Ƶ��
     * [ʾ��]:
     *     �� ʹ��ȫ��group�������ϱ�
     *       AT^BESTFREQ=0,1
     *       OK
     *     �� ��ֹȫ��group�������ϱ�
     *       AT^BESTFREQ=0,0
     *       OK
     *     �� ʹ��gourp1�����ϱ�������group1���Ӧ��DeviceIDΪ134��
     *       AT^BESTFREQ=134,1
     *       OK
     *     �� ��ֹgroup2�������ϱ�������group2���Ӧ��DeviceIDΪ104��
     *       AT^BESTFREQ=104,0
     *       OK
     *     �� ��ѯ���м������������Ϣ(�м�������)
     *       AT^BESTFREQ?
     *       ^BESTFREQ: 3, 12, 2, 1
     *       ^BESTFREQ: 5, 23, 3, 1
     *       OK
     *     �� ��ѯ���м������������Ϣ(�޼�������)
     *       AT^BESTFREQ?
     *       OK
     *       ����Ƶ����ŷ����仯�����ϱ�
     *       ^BESTFREQ: 3, 12, 2
     *       ^BESTFREQ: 5, 23, 3
     *     �� ����ָ��
     *       AT^BESTFREQ=?
     *       ^BESTFREQ: (0-255),(0-1)
     *       OK
     *     �� �����ϱ����õ�DeviceID����NV���õ�8������DeviceID�У����ش���
     *       AT^BESTFREQ=121,0
     *       ERROR
     */
    { AT_CMD_BESTFREQ,
      AT_SetBestFreqPara, AT_SET_PARA_TIME, AT_QryBestFreqPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^BESTFREQ", (VOS_UINT8 *)"(0-255),(0-1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �ֻ��˶�״̬
     * [˵��]: Sensor Hub��֪��ǰ�ֻ�״̬����ز�����
     * [�﷨]:
     *     [����]: ^SENSOR=<STATUS>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^SENSOR=?
     *     [���]: <CR><LF>^SENSOR: (list of supported <STATUS>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <STATUS>: �����ͣ���ǰ�ֻ�״̬��ȡֵ0��4294967295����0x0~0xffffffff����ֵ����������bitλ����ĸ�״ֵ̬�ĵ���ֵ����2���ֽڵ��Ǳ���λ�����統ǰ����˳��͸���״̬����ֵΪ0x401��
     *             0x00000000��UNKNOWN״̬��
     *             0x00000001������״̬��
     *             0x00000002���ﳵ״̬��
     *             0x00000004������״̬��
     *             0x00000008������״̬��
     *             0x00000010����ֹ״̬��
     *             0x00000020����б״̬��
     *             0x00000040��END״̬��
     *             0x00000080������״̬��
     *             0x00000100��С��״̬��
     *             0x00000200������״̬��
     *             0x00000400������״̬��
     *             0x00000800����·��ͨ״̬��
     *             0x00001000����·��ͨ״̬��
     *             0x00002000����ɽ״̬��
     *             0x00004000������״̬��
     *             0x00008000��ͣ��״̬��
     *             0x00010000������״̬��
     *             0x00020000��΢��״̬��
     *             0x00040000������״̬��
     * [ʾ��]:
     *     �� �ֻ�״̬��ֹ
     *       AT^SENSOR=16
     *       OK
     *     �� ��������
     *       AT^SENSOR=?
     *       ^SENSOR: (0-4294967295)
     *       OK
     */
    { AT_CMD_SENSOR,
      At_SetSensorPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^SENSOR", (VOS_UINT8 *)"(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NVˢ������
     * [˵��]: ����֪ͨMODEM��NVˢ�¡�
     *         ������ֻ�����ϵ硢������˫���л��ȳ����µ�NV�濨������
     * [�﷨]:
     *     [����]: ^NVREFRESH
     *     [���]: ִ����ȷʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     * [ʾ��]:
     *       AT^NVREFRESH
     *       OK
     */
    { AT_CMD_NVREFRESH,
      AT_SetNvRefreshPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^NVREFRESH", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ������ҪReattach���е�Detach�����ϱ�
     * [˵��]: ��������Ҫ����ȥע��ʱ�����DETACH REQUEST��Ϣ��Я����detach����Ϊ"re-attach required"����Modem��Ҫ�Ѵ��¼��ϱ���Ӧ�á�
     * [�﷨]:
     *     [����]: ^MTREATTACH=<report>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: ^MTREATTACH=?
     *     [���]: <CR><LF>^MTREATTACH: (list of supported <report>s) <CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: -
     *     [���]: <CR><LF>^MTREATTACH<CR><LF>
     * [����]:
     *     <report>: ����ֵ�������Ƿ������ϱ���Ĭ��ֵΪ0��
     *             0���������ϱ���
     *             1�� �����ϱ���
     * [ʾ��]:
     *     �� ����^MTREATTACH�������ϱ�
     *       AT^MTREATTACH=1
     *       ^MTREATTACH�������ϱ�
     *       ^MTREATTACH
     *     �� ��������
     *       AT^MTREATTACH=?
     *       ^MTREATTACH: (0,1)
     *       OK
     */
    { AT_CMD_MTREATTACH,
      AT_SetMtReattachPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^MTREATTACH", (TAF_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��Ļ״̬
     * [˵��]: �ֻ���Ļ״̬����ز�����
     * [�﷨]:
     *     [����]: ^SCREEN=<STATUS>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^SCREEN=?
     *     [���]: <CR><LF>^SCREEN: (list of supported <STATUS>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <STATUS>: �����ͣ���ǰ�ֻ���Ļ״̬��ȡֵ0��1��
     *             0������
     *             1������
     * [ʾ��]:
     *     �� �ֻ�����
     *       AT^SCREEN=0
     *       OK
     *     �� ��������
     *       AT^SCREEN=?
     *       ^SCREEN: (0-1)
     *       OK
     */
    { AT_CMD_SCREEN,
      At_SetScreenPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^SCREEN", (VOS_UINT8 *)"(0-1)" },

    { AT_CMD_LOGENALBE,
      AT_SetLogEnablePara, AT_SET_PARA_TIME, AT_QryLogEnable, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^LOGENABLE", (TAF_UINT8 *)"(0,1)" },


    { AT_CMD_ACTPDPSTUB,
      AT_SetActPdpStubPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^ACTPDPSTUB", (TAF_UINT8 *)"(0-2),(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: Modem C��ʱ��
     * [˵��]: Modem C�˵�ʱ�䣬�������ڡ�ʱ���ʱ������ز�����
     * [�﷨]:
     *     [����]: ^TIMESET=<date>,<time>,<zone>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TIMESET=?
     *     [���]: <CR><LF>^TIMESET: (list of supported <STATUS>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <date>: ��ʽΪyyyy/mm/dd������yyyyΪ4λ������[1970~2050],mmΪ��λ������Ϊ[01~12]��ddΪ��λ[01~31]
     *     <time>: ��ʽΪhh:mm:ss������hhΪ2λ������Ϊ[0~24����mmΪ��λ����Ϊ[0~59]��ssΪ��λ������Ϊ[0~59]��
     *     <zone>: ����ֵ
     *             ����Ϊ[-12~12]
     * [ʾ��]:
     *     �� ����MODEMʱ��ʧ��
     *       AT^TIMESET="2012/12/31","12:32:59","33"
     *       +CME ERROR: Incorrect parameters
     *     �� ����MODEMʱ��ɹ�
     *       AT^TIMESET="2012/12/31","12:32:59","8"
     *       OK
     *     �� ��������
     *       AT^TIMESET=?
     *       ^TIMESET: (YTD),(time),(zone)
     *       OK
     */
    { AT_CMD_TIMESET,
      AT_SetModemTimePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TIMESET", (VOS_UINT8 *)"(YTD),(time),(zone)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯModem����״̬
     * [˵��]: ���������ڲ�ѯ��ǰModem�Ƿ�������̬���Լ�����Modem������״̬�����仯ʱ�Ƿ������ϱ�֪ͨ��Ӧ�á�
     *         ��������ṩ��Modem��Wifi�����߷���ʹ�ã������ϲ��ж�Wifi�����л�MIMO��SISOģʽ��
     *         ��ǰ����״̬�仯�漰��Ҫҵ���������£�CS����ͨ���С�CS��������С�CS�����ҵ��CS�򲹳�ҵ��LAU���̡�ȥע�����̡�RAU���̡�TAU���̡�PS������ҵ���������̡�PS�����ҵ��ȡ�
     *         ������ֻ�ڶ�Modem��̬����Ч��
     *         �������ѯ������״̬������NAS��ҵ����ʼ״̬��������ȫ��ͬ����������̬��
     *         ��Modem��̬�������ϱ�����״̬�����Ҵ��ϱ�ʱ����������һ�������ϱ���
     * [�﷨]:
     *     [����]: ^CRRCONN=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^CRRCONN?
     *     [���]: <CR><LF>^CRRCONN: <enable>,<status0>,<status1>,<status2>  <CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^CRRCONN=?
     *     [���]: <CR><LF>^CRRCONN: (list of supported <enable>s) <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ��������ر�Modem����״̬�仯�����ϱ���
     *             0���ر������ϱ���
     *             1�����������ϱ���
     *     <status0>: ����ֵ��Modem0������״̬��
     *             0�������ӻ򼴽��˳�����̬��
     *             1�������ӻ򼴽���������̬��
     *     <status1>: ����ֵ��Modem1������״̬��
     *             0�������ӻ򼴽��˳�����̬��
     *             1�������ӻ򼴽���������̬��
     *     <status2>: ����ֵ��Modem2������״̬��
     *             0�������ӻ򼴽��˳�����̬��
     *             1�������ӻ򼴽���������̬��
     * [ʾ��]:
     *     �� ����Modem����״̬�仯�����ϱ����ܣ���ʱModem0��������̬
     *       AT^CRRCONN=1
     *       OK
     *       ^CRRCONN: 1,0,0
     *     �� ��ѯModem����״̬�������ϱ��򿪣�Modem1��������̬��Modem0��Modem2���ڷ�����̬
     *       AT^CRRCONN?
     *       ^CRRCONN: 1,0,1,0
     *       OK
     *     �� ִ�в�������
     *       AT^CRRCONN=?
     *       ^CRRCONN: (0,1)
     *       OK
     */
    { AT_CMD_CRRCONN,
      AT_SetCrrconnPara, AT_SET_PARA_TIME, AT_QryCrrconnPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CRRCONN", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: AP�·�EPDU����
     * [˵��]: �������ṩ��GPSоƬ��ePDU����ͨ��Modem���͸����硣��AT����������ƣ����÷�Ƭ�ķ�ʽ��ePDU����������ַ��͸�Modem������Modem��װת�����͸����硣
     *         ��������������������GPS��EPDU����������Ϣ��
     *         �������޲�ѯ���
     *         ʹ��^EPDU�����·�����EPDU����ʱ��ÿ��������^EPDU����֮�䲻Ӧ���5S���ϡ�
     * [�﷨]:
     *     [����]: ^EPDU=<transaction_id>,<msg_type>,<common_info_valid_flg>,<end_flag>,<loc_source>,<loc_calc_err>,<id>,[<name>],<total>,<index>,[<data>]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^EPDU=?
     *     [���]: <CR><LF>^EPDU: (list of supported <transaction_id>),(list of supported <msg_type>),(list of supported <common_info_valid_flg>),(list of supported <end_flag>),(list of supported <loc_source>),(list of supported <loc_calc_err>),(list of supported <id>),(list of supported <name>),(list of supported <total>),(list of supported <index>),(list of supported <data>)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <transaction_id>: ePDU������Ӧ��Transaction Id���������ͣ�ȡֵ��ΧΪ0~65535��
     *     <msg_type>: ePDU��������Ϣ���ͣ��������ͣ�ȡֵ��Χ0~7��
     *             0������������
     *             1���ṩ������
     *             2�����������ݣ�
     *             3���ṩ�������ݣ�
     *             4������λ����Ϣ��
     *             5���ṩλ����Ϣ��
     *             6��ȡ����
     *             7������
     *     <common_info_valid_flg>: LPP CommonIEs����γ����Ϣ����Ϣ�Ƿ���Ч��־λ���������ͣ�ȡֵ��Χ0~1��
     *             0����Ч��
     *             1����Ч��
     *     <end_flag>: ��λ����ΪMSBʱENDFLAG��־λ���������ͣ�ȡֵ��Χ0~1��
     *             0��ePDU���̽�����
     *             1�����к���ePDU���̡�
     *     <loc_source>: ��λ����ΪMSBʱ����λ�������������ͣ�ȡֵ��Χ0~10��
     *             0��δָ����
     *             1��AGNSS��
     *             2��OTDOA��
     *             3��EOTD��
     *             4��OTDOA Utran��
     *             5��ECID LTE��
     *             6��ECID GSM��
     *             7��ECID Utran��
     *             8��WLAN AP��
     *             9��SRN��
     *             10��Sensors��
     *     <loc_calc_err>: ��λ����ΪMSBʱ������λ����Ϣʱ����д���������ͣ�bit_map��ʽ��ռ16bit��0��ʾ��Ӧ����loc_source��λ����Ϣ����ɹ���1��ʾ����ʧ�ܣ�Ŀǰ��Ҫ���ڼ���OTDOA�ľ�γ��ʧ�ܳ�������
     *             ��Ӧbitλ���£�
     *             0��δָ����
     *             1��AGNSS��
     *             2��OTDOA��
     *             3��EOTD��
     *             4��OTDOA Utran��
     *             5��ECID LTE��
     *             6��ECID GSM��
     *             7��ECID Utran��
     *             8��WLAN AP��
     *             9��SRN��
     *             10��SENSORS��
     *     <id>: ePDU�����ı�ʶ���������ͣ�ȡֵ��Χ1~256��
     *     <name>: ePDU���������ƣ���ѡ�������ַ������ͣ����32���ַ�������ֱ�Ӷ�����
     *     <total>: ePDU�����ܷ�Ƭ�������������ͣ�ȡֵ��Χ1~9��
     *     <index>: ePDU������Ƭ�������������ͣ�ȡֵ��Χ1~9��
     *     <data>: ePDU��Ƭ��������ѡ������ͨ��Half-Byte��16���ƣ�����ɵ��ַ�������󳤶�Ϊ500���ַ�����������ʧ�ܡ�
     * [ʾ��]:
     *     �� �·�ePDU����
     *       AT^EPDU=1,2,0,0,8,0,121,"LPPE",1,1,0123456789ABCDEF0123456789ABCDEF
     *       OK
     *     �� ePDU��������
     *       at^EPDU=?
     *       ^EPDU: (0-65535),(0-7),(0-1),(0-1),(0-10),(0-10),(1-256),(str),(1-9),(1-9),(@data)
     *       OK
     */
    { AT_CMD_EPDU,
      At_SetEpduPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^EPDU", (VOS_UINT8 *)"(0-65535),(0-7),(0-1),(0-1),(0-10),(0-10),(1-256),(str),(1-9),(1-9),(@data)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �ϱ�VT����
     * [˵��]: ����Ƶ���ش��ڼ���״̬ʱ���������ϱ���Ϣÿ��2s����һ��(���ʱ��NV����)���ϱ������ݰ�����ǰ�����Ƶ���ص�����ʱ�䡢��ǰ�����Ƶ���صķ������ʡ���ǰ�����Ƶ���صĽ������ʡ���ǰ�����Ƶ���صķ�����������ǰ�����Ƶ���صĽ����������������Э�̺�ȷ����PDP���ӷ������ʺ��������Э�̺�ȷ����PDP���ӽ������ʡ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^VTFLOWRPT: <curr_vt_time>,<curr_tx_flow>,<curr_rx_flow><CR><LF>
     *     [����]: ^VTFLOWRPT=<oper>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^VTFLOWRPT=?
     *     [���]: <CR><LF>^VTFLOWRPT: (list of supported <oper>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <curr_vt_time>: 0x0000_0000��0xFFFF_FFFF��8λ16����������ʾ��ǰ�����Ƶ���ص�����ʱ�䣬��λΪ�롣
     *     <curr_tx_flow>: 0x0000_0000_0000_0000��0xFFFF_FFFF_FFFF_FFFF��16λ16����������ʾ��ǰ�����Ƶ���صķ�����������λΪ�ֽڡ�
     *     <curr_rx_flow>: 0x0000_0000_0000_0000��0xFFFF_FFFF_FFFF_FFFF��16λ16����������ʾ��ǰ�����Ƶ���صĽ�����������λΪ�ֽڡ�
     *     <oper>: ����ֵ����Ƶ���������ϱ����ء�
     *             0��������Ƶ�����Զ������ϱ���
     *             1��ʹ����Ƶ�����Զ������ϱ���
     * [ʾ��]:
     *     �� �����ϱ�
     *       AT^VTFLOWRPT=1
     *       OK
     *     �� ��������
     *       AT^VTFLOWRPT=?
     *       ^VTFLOWRPT: (0,1)
     *       OK
     */
    { AT_CMD_VTFLOWRPT,
      AT_SetVTFlowRptPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VTFLOWRPT", (VOS_UINT8 *)"(0,1)" },

#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����ҵ�񿪹�֪ͨ
     * [˵��]: ��AP������ҵ�񿪹�״̬�仯ʱ��ͨ����AT����֪ͨ��Modem��
     * [�﷨]:
     *     [����]: ^DATASWITCH=<state>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF> ERROR<CR><LF>
     *     [����]: ^DATASWITCH?
     *     [���]: <CR><LF>^DATASWITCH: <state><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^DATASWITCH=?
     *     [���]: <CR><LF>^DATASWITCH: (list of supported status) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <state>: ����ֵ������ҵ�񿪹�״̬��
     *             0��AP������ҵ�񿪹عرգ�
     *             1��AP������ҵ�񿪹ش򿪡�
     * [ʾ��]:
     *     �� ����ҵ�񿪹ش�֪ͨ
     *       AT^DATASWITCH=1
     *       OK
     *     �� ����ҵ�񿪹عر�֪ͨ
     *       AT^DATASWITCH=0
     *       OK
     *     �� ��ѯ����
     *       AT^DATASWITCH?
     *       ^DATASWITCH: 0
     *       OK
     *     �� ��������
     *       AT^DATASWITCH=?
     *       ^DATASWITCH: (0,1)
     *       OK
     */
    { AT_CMD_DATASWITCH,
      AT_SetDataSwitchStatus, AT_SET_PARA_TIME, AT_QryDataSwitchStatus, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DATASWITCH", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����ҵ�����ο���֪ͨ
     * [˵��]: ��AP������ҵ�����ο���״̬�仯ʱ��ͨ����AT����֪ͨ��Modem��
     * [�﷨]:
     *     [����]: ^DATAROAMSWITCH=<state>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF> ERROR<CR><LF>
     *     [����]: ^DATAROAMSWITCH?
     *     [���]: <CR><LF>^DATAROAMSWITCH: <state><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^DATAROAMSWITCH=?
     *     [���]: <CR><LF>^DATAROAMSWITCH: (list of supported status) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <state>: ����ֵ������ҵ�����ο���״̬��
     *             0��AP������ҵ�����ο��عرգ�
     *             1��AP������ҵ�����ο��ش򿪡�
     * [ʾ��]:
     *     �� ����ҵ�����ο��ش�֪ͨ
     *       AT^DATAROAMSWITCH=1
     *       OK
     *     �� ����ҵ�����ο��عر�֪ͨ
     *       AT^DATAROAMSWITCH=0
     *       OK
     *     �� ��ѯ����
     *       AT^DATAROAMSWITCH?
     *       ^DATAROAMSWITCH: 0
     *       OK
     *     �� ��������
     *       AT^DATAROAMSWITCH=?
     *       ^DATAROAMSWITCH: (0,1)
     *       OK
     */
    { AT_CMD_DATAROAMSWITCH,
      AT_SetDataRoamSwitchStatus, AT_SET_PARA_TIME, AT_QryDataRoamSwitchStatus, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DATAROAMSWITCH", (VOS_UINT8 *)"(0,1)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯLTEĬ�ϳ���ע����Ϣ
     * [˵��]: ���������ڻ�ȡLTEע��ɹ���Ĭ�ϳ��ص���Ϣ��Ŀǰ����LTEЯ����APNע��ɹ���֧�ֲ�ѯ��������APN�Լ�IP TYPE��Ϣ
     *         ������Ч�ͷ�LTE����ʱ���᷵��ERROR
     * [�﷨]:
     *     [����]: ^LTEATTACHINFO?
     *     [���]: <CR><LF>^LTEATTACHINFO: <PDP_type>,<APN><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <PDP_type>: ����ֵ����������Э������
     *             1��IPV4
     *             2��IPV6
     *             3��IPV4V6
     *     <APN>: �ַ���ֵ����ʾ����GGSN���ⲿ���Ľ��������������ֵΪ�գ���ʹ��ǩԼֵ
     * [ʾ��]:
     *     �� ��ȡLTEע�������Ϣ
     *       AT^LTEATTACHINFO?
     *       ^LTEATTACHINFO: 1,"Huawei.com"
     *       OK
     */
    { AT_CMD_LTEATTACHINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryLteAttachInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LTEATTACHINFO", VOS_NULL_PTR },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NSA��LTE-NR��DC˫����
     * [˵��]: ���������ڿ���LTE��ENDC˫����״̬�仯�ϱ�������ѯLTE��ģ�£���ǰ������NR���������
     *         ���������LTE��ģ��ѯ�����Ч��
     *         ���������֧��NR������²�֧���ϱ��Ͳ�ѯ�������ѯ�ظ�ʧ�ܡ�
     * [�﷨]:
     *     [����]: ^LENDC=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LENDC?
     *     [���]: <CR><LF>^LENDC: <enable>,<endc_available>, <endc_plmn_avail>, <endc_restricted>,<nr_pscell><CR><LF> <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LENDC=?
     *     [���]: <CR><LF>^LENDC: (list of supported <enable>s) <CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: LTE��ģ��ENDC״̬�仯
     *     [���]: <CR><LF>^LENDC: <endc_available>, <endc_plmn_avail>, <endc_restricted>,<nr_pscell><CR><LF>
     * [����]:
     *     <enable>: ����ֵ��ָʾLTE��ENDC˫����״̬�仯�Ƿ������ϱ���
     *             0����ֹ�����ϱ���
     *             1��ʹ�������ϱ���
     *     <endc_available>: ����ֵ����ǰС���Ƿ�֧��ENDCģʽ����LRRC��SIB2��upperLayerIndication-r15���ָʾ��
     *             0����֧�֣�
     *             1��֧�֡�
     *     <endc_plmn_avail>: ����ֵ��PLMN LIST���Ƿ���PLMN֧��ENDCģʽ��
     *             0��û��PLMN֧��ENDCģʽ��
     *             1����PLMN֧��ENDCģʽ��
     *     <endc_restricted>: ����ֵ��LNASע��ACCEPT��Ϣ��ENDC������
     *             0��������û������ENDC������
     *             1������������ENDC������
     *     <nr_pscell>: ����ֵ����ǰPSCell�Ƿ�ΪNR�����Ƿ����ENDC˫����״̬��
     *             0����ENDC״̬��
     *             1��ENDC�Ѿ�����״̬��
     *             2����ʾ����ENDCê��С��״̬��
     * [ʾ��]:
     * [ʾ��]:
     *     �� ʹ�������ϱ�
     *       AT^LENDC=1
     *       OK
     *     �� LTE�²�ѯENDC˫����״̬
     *       AT^LENDC?
     *       ^LENDC: 1,1,1,1,0
     *       OK
     *     �� ִ�в�������
     *       AT^LENDC=?
     *       ^LENDC: (0,1)
     *       OK
     *       ENDC״̬�仯Ϊ��ǰС����ENDC״̬
     *       ^LENDC: 1,1,0,0
     *       ENDC״̬�仯ΪС������ENDCê��״̬
     *       ^LENDC: 1,1,0,2
     */
    { AT_CMD_LENDC,
      AT_SetLendcPara, AT_SET_PARA_TIME, AT_QryLendcPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LENDC", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: UE policy section��Ϣ�ϱ�
     * [˵��]: �����������ʾ���������ϱ�����^CPOLICYRPT�Ƿ����á�
     *         Modem�յ�������·���UE policy section��Ϣ��ʹ��^CPOLICYRPT�����ϱ���AP��
     *         ע�⣺�����ϱ�����Ĭ�ϴ򿪣�Modem�յ������ͻ��ϱ������ն�Լ�������������ϱ�����������ݲ�����ʹ�á�
     * [�﷨]:
     *     [����]: ^CPOLICYRPT=<enable>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: -
     *     [���]: <CR><LF>^CPOLICYRPT: <total_length>,<section_num>,<protocol_ver>
     *             <CR><LF>
     * [����]:
     *     <enable>: ����ֵ���Ƿ����������ϱ�
     *             0���������ϱ�
     *             1�����������ϱ�
     *     <total_length>: ����ֵ��UE POLICY SECTION��Ϣ�������ֽ���
     *     <section_num>: ����ֵ��UE POLICY SECTION��Ϣ������Ҫ�ֶβ�ѯ�ĸ���
     *     <protocol_ver>: ����ֵ��URSP�汾��
     *             1500: 24526-F00
     *             1510: 24526-F10
     *             1520: 24526-F20
     *             1530: 24526-F30
     *             ע���ò�����NV6019�ĵڶ�����������
     * [ʾ��]:
     *     �� ���������ϱ�����
     *       AT^CPOLICYRPT=1
     *       OK
     *       �����ϱ���Ϣ
     *       ^CPOLICYRPT: 2100,2,1510
     */
    { AT_CMD_CPOLICYRPT,
      AT_SetCpolicyRptPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CPOLICYRPT", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: UE policy section��Ϣ�ֶβ�ѯ
     * [˵��]: ����Я����<index>�������ֶβ�ѯ������·���������Ϣ�����������е�ANDSP��Ϣ����ÿ�����ݷֶ�����Ϊ1000���ֽڣ�����ӡ���ַ����������Ȳ�����2000��
     * [�﷨]:
     *     [����]: ^CPOLICYCODE=<index>
     *     [���]: <CR><LF>^CPOLICYCODE: <index >,<code><CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <index>: ����ֵ���ֶα�ʶ
     *     <code>: �ַ�����UE POLICY SECTION����������С�ڵ���2000�ַ�
     *             ע���ϲ��ڽ���ʱ����2���ַ�ƴΪһ��16���Ƶ���ֵ�����磺��1234������Ϊ��0x12,0x34����
     * [ʾ��]:
     *       AT^CPOLICYCODE=2
     *       ^CPOLICYCODE: 2,"0000011111222223333344444"
     *       OK
     */
    { AT_CMD_CPOLICYCODE,
      AT_GetCpolicyCodePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CPOLICYCODE", (VOS_UINT8 *)"(1-255)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���á���ѯLADN��Ϣ
     * [˵��]: �����������AT^CLADN=<n>����ʾ���������ϱ�����^CLADNU�Ƿ����á�
     *         ��ѯ�������AT^CLADN?����ʾ��ѯ��ǰλ�������п���ʹ�õ�LADN DNN������DNN�б�����ʹ�õ�LADN DNN������ DNN�б�
     * [�﷨]:
     *     [����]: ^CLADN=<n>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CLADN?
     *     [���]: <CR><LF>^CLADNU: <n>,<allowed_ladn_dnn_num>,<allowed_dnn_list>,<nonallowed_ladn_dnn_num>, <nonallowed_dnn_list><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ���Ƿ����������ϱ�
     *             0����ֹ����^CLADNU�����ϱ�
     *             1����������^CLADNU�����ϱ�
     *     <allowed_ladn_num>: ����ֵ����ǰλ�����ڿ�������ʹ�õ�LADN DNN������
     *     <allowed_dnn_list>: �ַ������ͣ���ǰλ�����ڿ�������ʹ�õ�LADN DNN����ʽ���£���dnn1;dnn2;��dnn8��
     *     <nonallowed_ladn_dnn_num>: ����ֵ����ǰλ�����ڲ�����ʹ�õ�LADN DNN������allowed_ladn_num��nonallowed_ladn_num�ĺ�С�ڵ���8��
     *     <nonallowed_dnn_list>: �ַ������ͣ���ǰλ�����ڲ�����ʹ�õ�LADN DNN����ʽ���£���dnn1;dnn2;��dnn8��
     * [ʾ��]:
     *     �� ���������ϱ�����
     *       AT^CLADN=1
     *       OK
     *       ��ѯ����
     *     �� �����ϱ�����������/������DNN������
     *       AT^CLADN?
     *       ^CLADN: 1,1,"Ladn.dnn.01",2,"Ladn.dnn.02;Ladn.dnn.03"
     *       OK
     *     �� LADN DNN������
     *       AT^CLADN?
     *       ^CLADN:
     *       1,8,"Ladn.dnn.01;Ladn.dnn.02;Ladn.dnn.03;Ladn.dnn.04;Ladn.dnn.05;Ladn.dnn.06;Ladn.dnn.07;Ladn.dnn.08",0,""
     *       OK
     *     �� LADN DNN��������
     *       AT^CLADN?
     *       ^CLADN: 1,0,"",8,"Ladn.dnn.01;Ladn.dnn.02;Ladn.dnn.03; Ladn.dnn.04;Ladn.dnn.05;Ladn.dnn.06;Ladn.dnn.07;Ladn.dnn.08"
     *       OK
     */
    { AT_CMD_CLADN,
      AT_SetLadnRptPara, AT_SET_PARA_TIME, AT_QryLadnInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CLADN", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_C5GRSDQRY,
      AT_SetC5gRsdPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^C5GRSDQRY", (VOS_UINT8 *)C5GRSDQRY_CMD_PARA_STRING },
#endif

    { AT_CMD_CSQLVL,
      AT_SetCsqlvlPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CSQLVL", VOS_NULL_PTR },

    { AT_CMD_CSQLVLEXT,
      AT_SetCsqlvlExtPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CSQLVLEXT", (VOS_UINT8 *)"(0,20,40,60,80,99),(99)" },

    /* ͳ�ƴ��ϵ絽pdp����ɹ�������ʱ�� */
    { AT_CMD_LCSTARTTIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryLcStartTimePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^LCSTARTTIME", VOS_NULL_PTR },

    { AT_CMD_AUTHORITYVER,
      VOS_NULL_PTR, AT_NOT_SET_TIME, atQryAuthorityVer, AT_UPGRADE_TIME_5S, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^AUTHORITYVER", VOS_NULL_PTR },

    { AT_CMD_AUTHORITYID,
      VOS_NULL_PTR, AT_NOT_SET_TIME, atQryAuthorityID, AT_UPGRADE_TIME_5S, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^AUTHORITYID", VOS_NULL_PTR },


    { AT_CMD_NVRSTSTTS,
      atSetNVRstSTTS, AT_UPGRADE_TIME_5S, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^NVRSTSTTS", VOS_NULL_PTR },

    /* XML���� begin */
    { AT_CMD_APRPTSRVURL,
      AT_SetApRptSrvUrlPara, AT_NOT_SET_TIME, AT_QryApRptSrvUrlPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^APRPTSRVURL", (VOS_UINT8 *)"(@ApRptSrvUrl)" },

    { AT_CMD_APXMLINFOTYPE,
      AT_SetApXmlInfoTypePara, AT_NOT_SET_TIME, AT_QryApXmlInfoTypePara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^APXMLINFOTYPE", (VOS_UINT8 *)"(@ApXmlInfoType)" },

    { AT_CMD_APXMLRPTFLAG,
      AT_SetApXmlRptFlagPara, AT_NOT_SET_TIME, AT_QryApXmlRptFlagPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^APXMLRPTFLAG", (VOS_UINT8 *)"(0,1)" },
    /* XML���� end  */

    { AT_CMD_VERTIME,
      AT_SetVertime, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VERTIME", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: RRC״̬��ѯ
     * [˵��]: ��������������RRC״̬��פ��״̬�ϱ����Լ�RRC״̬��פ��״̬��ѯ��
     *         ע����LTE��NR���յ�RRCSTAT��ѯ����ʱmodemֱ�ӻظ�error��
     *         �������NR��ģ�᷵��INACTIVE
     *         �������LTE��ģ�᷵��camp_status����
     *         DC�������ص�ǰ��վ������ʽ��RRC״̬
     *         GU��֧�������ϱ��Ͳ�ѯ
     * [�﷨]:
     *     [����]: ^RRCSTAT=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^RRCSTAT?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^RRCSTAT: < enable > ,<rrc_status>[,<camp_status>]<CR><LF> <CR><LF>OK<CR><LF>
     *             ����ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: -
     *     [���]: <CR><LF>^RRCSTAT: <rrc_status>[,<camp_status>]<CR><LF>
     * [����]:
     *     <enable>: ����ֵ����ʶ�Ƿ��������ϱ���
     *             0���ر������ϱ���
     *             1�����������ϱ���
     *             ע�⣺RRCSTAT����CURC�����ϱ�Լ����
     *     <rrc_status>: RRC����״̬��
     *             0��RRC״̬Ϊ������̬��
     *             1��RRC״̬Ϊ����̬��
     *             2��RRC״̬ΪINACTIVE״̬��
     *             3��RRC״̬��Ч��
     *     <camp_status>: ����ֵ��פ��״̬��
     *             98��CAMPED״̬
     *             99��NOT CAMPED״̬
     * [ʾ��]:
     *     �� ����RRCSTAT�����ϱ�
     *       AT^RRCSTAT=1
     *       OK
     *       LTE��ģ�£�RRCSTAT�����ϱ���RRC CONN̬��CAMPED״̬
     *       ^RRCSTAT: 1,98
     *       NR��ģ�£�RRCSTAT�����ϱ���RRC CONN̬
     *       ^RRCSTAT: 1
     *     �� LTE��ģ�£���ѯ��ǰRRC״̬����ǰ���������ϱ���RRC IDLE��פ��̬
     *       AT^RRCSTAT?
     *       ^RRCSTAT: 1,0,99
     *       OK
     *     �� NR��ģ�£���ѯ��ǰRRC״̬����ǰ���������ϱ���RRC IDLE̬
     *       AT^RRCSTAT?
     *       ^RRCSTAT: 1,0
     *       OK
     *     �� ��ѯ��ǰRRC״̬����ǰ�������ϱ���RRC INACTIVE̬
     *       AT^RRCSTAT?
     *       ^RRCSTAT: 0,2
     *       OK
     */
    { AT_CMD_RRCSTAT,
      AT_SetRrcStatPara, AT_SET_PARA_TIME, AT_QryRrcStatPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RRCSTAT", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: LTE/NR����Ƶ�ʲ�ѯ
     * [˵��]: ͨ����AT��ȡ��ǰС��Ƶ����Ϣ��NR֧�ֶ�CC��Ƶ����Ϣ�ϱ���LTE��֧����С��Ƶ����Ϣ�ϱ���
     *         ���������LTE��NR��ģ��Ч��
     *         EN-DC������LTE/NR HFREQINFO�����ϱ���
     *         ������Σ�֧�ֶ�CC��LTE��֧����С����
     * [�﷨]:
     *     [����]: ^HFREQINFO=<n>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^HFREQINFO?
     *     [���]: ִ�гɹ�:
     *             <CR><LF>^HFREQINFO:<n>,<sysmode>,<band_class1>,<dl_fcn1>,<dl_freq1><dl_bw1>,<ul_fcn1>,<ul_freq1>,<ul_bw1>,[<band_class2>,<dl_fcn2>,<dl_freq2><dl_bw2>,<ul_fcn2>,<ul_freq2>,<ul_bw2>,[<band_class3>,<dl_fcn3>,<dl_freq3><dl_bw3>,<ul_fcn3>,<ul_freq3>,<ul_bw3>,[<band_class4>,<dl_fcn4>,<dl_freq4><dl_bw4>,<ul_fcn4>,<ul_freq4>,<ul_bw4>]]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^HFREQINFO=?
     *     [���]: <CR><LF>^HFREQINFO: (list of supported <n>,<sysmode>)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ���α���
     *             0    ��ֹ^HFREQINFO�������ϱ���Ĭ��ֵ��
     *             1    ʹ��^HFREQINFO�������ϱ�
     *     <sysmode>: ���α�������ʾ��ǰ�ն���������ģʽ��
     *             1    GSM����֧�֣�
     *             2    CDMA����֧�֣�
     *             3    WCDMA����֧�֣�
     *             4    TDSCDMA����֧�֣�
     *             6    LTE
     *             7    NR
     *             ��ǰ��AT��֧��LTE��NRģʽ
     *     <band_classN>: ���α�������ʾ��ǰ�ն�����С����Ӧ��Ƶ��
     *             LTEȡֵΪ1-44������ֵ�ο�3GPP 36.101
     *             NRȡֵΪ1-86,257,258,260,261 ����ֵ�ο�3GPP 38.101��NΪ�ز���Ŀ�����Ϊ4��
     *     <dl_fcnN>: ���α�����������С����Ƶ�㣬��Чֵȡ0��NΪ�ز���Ŀ�����Ϊ4
     *     <dl_freqN>: ���α�����������С����Ƶ��(100KHZ)����Чֵȡ0��NΪ�ز���Ŀ�����Ϊ4��
     *     <dl_bwN>: ���α�����������С����ϵͳ����(KHZ)
     *             LTEȡֵ��1400,3000,5000,10000,15000,20000
     *             NRȡֵ��1400,30000,5000,10000,15000,20000,25000,30000,40000,50000,60000,80000,90000,100000,200000,400000��NΪ�ز���Ŀ�����Ϊ4��
     *     <ul_fcnN>: ���α�����������С����Ƶ�㣬��Чֵȡ0��NΪ�ز���Ŀ�����Ϊ4��
     *     <ul_freqN>: ���α�����������С����Ƶ��(100KHZ)����Чֵȡ0��NΪ�ز���Ŀ�����Ϊ4��
     *     <ul_bwN>: ���α�����������С����ϵͳ����(KHZ)
     *             LTEȡֵ��1400,3000,5000,10000,15000,20000
     *             NRȡֵ��1400,30000,5000,10000,15000,20000,25000,30000,40000,50000,60000,80000,90000,100000,200000,400000��NΪ�ز���Ŀ�����Ϊ4��
     * [ʾ��]:
     *     �� WCDMAģʽ�£���ѯƵ����Ϣ
     *       AT^HFREQINFO?
     *       ERROR
     *     �� LTEģʽ�£���ѯƵ����Ϣ
     *       AT^HFREQINFO?
     *       ^HFREQINFO: 0,6,18,5925,8675,20000,23925,8225,20000
     *       OK
     *     �� NRģʽ�£���ѯƵ����Ϣ
     *       AT^HFREQINFO?
     *       ^HFREQINFO: 0,7,77,640000,360000,100000,640000,36000,100000
     *       OK
     *     �� DCģʽ�£���ѯƵ����Ϣ
     *       AT^HFREQINFO?
     *       ^HFREQINFO: 0,6,18,5925,8675,20000,23925,8225,20000
     *       ^HFREQINFO: 0,7,18,5925,8675,20000,23925,8225,20000
     *       OK
     *     �� DCģʽ�£������ϱ��򿪣���ѯƵ����Ϣ
     *       AT^HFREQINFO?
     *       ^HFREQINFO: 1,6,18,5925,8675,20000,23925,8225,20000
     *       ^HFREQINFO: 0,7,18,5925,8675,20000,23925,8225,20000
     *       OK
     *       �����ϱ�LTE С��Ƶ����Ϣ
     *       ^HFREQINFO: 1,6,18,5925,8675,20000,23925,8225,20000
     */
    { AT_CMD_HFREQINFO,
      AT_SetHfreqInfoPara, AT_SET_PARA_TIME, AT_QryHfreqInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^HFREQINFO", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: HIFI��λָʾ
     * [˵��]: ����������APָʾMODEM HIFI��λ��ʼ����ɡ�
     *         ��������Ҫ�������ص�AT����ͬһ��ͨ���·�
     * [�﷨]:
     *     [����]: ^HIFIRESET=<mode>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <mode>: ����ֵ��HIFI��λָʾ���ƣ�ȡֵ0~1��
     *             0��HIFI��λ��ʼָʾ��
     *             1��HIFI��λ���ָʾ��
     * [ʾ��]:
     *     �� HIFI��λ��ʼָʾ
     *       AT^HIFIRESET=0
     *       OK
     *     �� HIFI��λ���ָʾ
     *       AT^HIFIRESET=1
     *       OK
     */
    { AT_CMD_HIFIRESET,
      At_SetHifiReset, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^HIFIRESET", (VOS_UINT8 *)"(0-1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: UEģʽ֪ͨ
     * [˵��]: UE֪ͨModem��ǰ����ģʽ����ǰ��֧��˯��ģʽ��
     * [�﷨]:
     *     [����]: ^UEAIMODENTF=<mode_status>,<mode_flag>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <mode_status>: ����ֵ����ʶUE��ǰ����ģʽ
     *             0��˯��ģʽ����֧��˯��ģʽ��
     *     <mode_flag>: ����ֵ����ʶ��ǰ����ģʽ��״̬
     *             0���˳���ǰ����ģʽ��
     *             1�����뵱ǰ����ģʽ��
     * [ʾ��]:
     *     �� �˳�˯��ģʽ
     *       AT^UEAIMODENTF=0,0
     *       OK
     *     �� ����˯��ģʽ
     *       AT^UEAIMODENTF=0,1
     *       OK
     */
    { AT_CMD_UEMODENTF,
      AT_SetUeModeStatus, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8*)"^UEAIMODENTF", (VOS_UINT8 *)"(0),(0-1)" },

#if (FEATURE_IMS == FEATURE_ON)
    { AT_CMD_IMSPDPCFG,
      AT_SetImsPdpCfg, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSPDPCFG", (VOS_UINT8 *)"(0-20),(0,1)" },
#endif

    { AT_CMD_SETPID,
      At_SetpidPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE | CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SETPID", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_RATCOMBINEPRIO,
      AT_SetRATCombinePara, AT_NOT_SET_TIME, AT_QryRATCombinePara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^RATCOMBINEPRIO", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: MT���ź�MT���п���������������
     * [˵��]: ��������/��ѯMT���ſ���������MT��������������MT���������������ݲ�֧�֣���
     * [�﷨]:
     *     [����]: ^TNUMCTL=<sms_white_enable>,<call_white_enable>,<call_black_enbale>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ����ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TNUMCTL?
     *     [���]: <CR><LF>^TNUMCTL: <sms_white_enable>,<call_white_enable>,<call_black_enbale><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^TNUMCTL=?
     *     [���]: <CR><LF>^TNUMCTL: (list of supported <sms_white_enable>s),(list of supported <call_white_enable>s),(list of supported <call_black_enbale>s) <CR><LF><CR><LF>OK<CR><LR>
     * [����]:
     *     <sms_white_enable>: ����ֵ����ʾ�Ƿ����ö��ſ���������
     *             0��Disable
     *             1��Enable
     *     <call_white_enable>: ����ֵ����ʾ�Ƿ����ú��п���������
     *             0��Disable
     *             1��Enable
     *     <call_black_enbale>: ����ֵ���Ƿ����ú����������������ݲ�֧�֣�
     *             0��Disable
     *             1��Enable
     * [ʾ��]:
     *     �� ����MT���ſ�������
     *       AT^TNUMCTL=1,0,0
     *       OK
     *     �� ��ѯMT���ſ��������Ƿ�ʹ�ܣ�
     *       AT^TNUMCTL?
     *       ^TNUMCTL: 1,0,0
     *       OK
     *     �� ��ѯ����֧�ֵĲ�����Χ��
     *       AT^TNUMCTL=?^TNUMCTL:(0-1),(0-1),0
     *       OK
     */
    { AT_CMD_TNUMCTL,
    AT_SetTrustNumCtrlPara, AT_NOT_SET_TIME, AT_QryTrustNumCtrlPara, AT_NOT_SET_TIME, AT_TestTrustNumCtrlPara,
    AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8 *)"^TNUMCTL", (VOS_UINT8 *)"(0-1),(0-1),(0)" },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /* Ϊ�Զ��������������� */
    { AT_CMD_CCC,
      AT_SetCccPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"^CCC", (VOS_UINT8 *)"(0,1),(1-7)" },

    { AT_CMD_CIMEI,
      At_SetImeiPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CIMEI", (VOS_UINT8 *)"(imei)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ͨ��(U)SIM����ISDB
     * [˵��]: ����������ͨ��(U)SIM������ISDB���ܡ��������APDU͸����(U)SIM��������(U)SIM���ظ������ݷ��ء�
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     *         ���ǵ�AP��ȫ���󣬴����������HSIC ATͨ��������ATͨ���·�������Modem��������
     * [�﷨]:
     *     [����]: ^CISA=<length>,<command>
     *     [���]: <CR><LF>^CISA: <length>,<response><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CISA=?
     *     [���]: <CR><LF>^CISA: <length>,<cmd><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <length>: �����ͣ�����<command>��<response>���ַ����ȣ�ȡֵ��Χ��1-520����
     *     <command>: �·���(U)SIM����APDU����
     *             �ַ������ͣ�����Ϊ10~520byte��ʹ��Half-Byte�����Ķ�����������
     *     <response>: (U)SIM�������·���<command>����Ӧ���ݡ�
     *             �ַ������ͣ�����Ϊ4~514byte��ʹ��Half-Byte�����Ķ�����������
     * [ʾ��]:
     *     �� ����APDU
     *       AT^CISA=44,"90F2000010F0000000010001FF81FF10FFFFFFFF0201"
     *       ^CISA: 6,"019000"
     *       OK
     *     �� ��������
     *       AT^CISA=?
     *       ^CISA: (1-520),(cmd)
     *       OK
     */
    { AT_CMD_CISA,
      VOS_NULL_PTR, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CISA", (TAF_UINT8 *)"(1-520),(cmd)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ʹ��LOG�ӳ����
     * [˵��]: ���������AP+Modem��Ʒ��̬��ͨ��LogTool��AP�ౣ��HiDS����ʱʹ�á�
     *         ����������AP-Modem��̬������LogTool���ӳ�������ơ��ӳ��ϱ�������Ϊ���ܹ��ڵ��岻��USB���������Ȼ���Ա��浥��Ŀ�ά�ɲ����ݣ������ϱ����̲��Ե���Ĺ��Ĳ����ϴ�Ӱ�졣��˸ù�����Ҫ��Beta�����û�ץȡLog����ʹ�ã������з��ڲ����ܣ������ⷢ��������Ĭ�Ϲرա�
     *         ���ܴ򿪺͹رմ˹�����Ҫ�������������Ч��
     *         �򿪹���֮��������������ó���HiDS���ߵĵȴ�ʱ�䣬����޷�����HiDS���ߡ�����HiDS�޷����ӹ���ʱ����Ҫ�������ѯ����жϵ�ǰ���ܵ�״̬�Ͳ������á����ϣ���ܹ�����HiDS���ߣ���Ҫ�������²������г��ԣ�
     *         AT^LOGPORT=0��AT����سɹ������ٴ����ӣ�
     *         �����ͨ��USB����HiDS���ߵ��������ͨ��LogToolץȡLogʱ�����Ƚ��ֻ���USB�γ���������LogTool��
     *         ����������ʹ�õ�< TIMER_OUT >��<BUFFER_SIZE>������ͨ��DTS�ڿ����׶λ�ȡ�ģ�ͨ����������Բ�ѯ�������޸Ĳ�����ֵ����Ʒ�߿��Ը��ݲ�Ʒ�Ĺ�������ز�����ֵ�������������漰�Ĳ���ֵ��ΪBalong�ṩ��Ĭ�����á�
     * [�﷨]:
     *     [����]: ^LOGNVE[=[<ENABLE_FLAG>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^LOGNVE?
     *     [���]: <CR><LF>^LOGNVE: <ENABLE_FLAG>,< TIMER_OUT >,<BUFFER_SIZE><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LOGNVE=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <ENABLE_FLAG>: LOG�ӳ�д��ʹ�ܱ�־��
     *             0����ʹ�ܣ�
     *             1��ʹ�ܡ�
     *     <TIMER_OUT>: �ӳ�ʱ�䣬��minΪ��λ��ȡֵ��Χ10~20��
     *     <BUFFER_SIZE>: �ӳ�д�빦�ܴ�ʱʹ�õĻ�������С����byteΪ��λ
     * [ʾ��]:
     *     �� ʹ��LOG�ӳ�д�����
     *       AT^LOGNVE=1
     *       OK
     *     �� ��ѯ����ӳٹ�����ʹ�ܣ���ʱʱ��Ϊ10���ӣ������СΪ50M
     *       AT^LOGNVE?
     *       ^LOGNVE: 1,10,52428800
     *       OK
     *     �� ��������
     *       AT^LOGNVE=?
     *       OK
     */
    { AT_CMD_LOGNVE,
      AT_SetLogNvePara, AT_SET_PARA_TIME, AT_QryLogNvePara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LOGNVE", (VOS_UINT8 *)"(0,1)" },
    /*
     * [���]: Э��AT-LTE���
     * [����]: ���LTE��ʷƵ��
     * [˵��]: �������LTE��ʷƵ����Ϣ��
     * [�﷨]:
     *     [����]: ^CHISFREQ=<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CHISFREQ=?
     *     [���]: <CR><LF>^CHISFREQ: List of supported <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ�������ʷƵ������͡�
     *             0�������CSG��ʷƵ����Ϣ��
     *             1�����CSG��ʷƵ����Ϣ��
     *             2��������У�������CSG��CSG����ʷƵ����Ϣ
     * [ʾ��]:
     *     �� ��������
     *       AT^CHISFREQ=?
     *       ^CHISFREQ: (0,1,2)
     *        OK
     *     �� ���CSG��ʷƵ����Ϣ
     *       AT^CHISFREQ=1
     *       OK
     */
    { AT_CMD_CHISFREQ,
      AT_SetHistoryFreqPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CHISFREQ", (VOS_UINT8 *)"(0,1,2)" },

#if (FEATURE_LTE == FEATURE_ON)
#if (FEATURE_LTE_MBMS == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: ����MBMS����
     * [˵��]: ��������LTE�²�֧�֣����ڹ���MBMS������ع����������ʹ�ܻ�ȥʹ��MBMS�������ԣ�����MBMS����״̬����ѯMBMS�����б�Ͳ���������������MBMS�㲥ģʽΪ�������鲥����ѯSIB16����ʱ�䣬��ѯBSSI�ź�ǿ�ȣ���ѯ������Ϣ����ѯeMBMS����״̬���ܡ�
     *         �ڹػ�״̬��ִ�и�������ع��ܽ����ش�����Ϣ��MBMS��������Ĭ���ǹرյģ�ÿ�����ػ���ָ�ΪĬ��ֵ����Ҫ���¿�����
     *         ֻ��MBB֧�ָ�����ֻ���֧�֡�
     *         ��������ع��ܣ�����������MBMS�㲥ģʽΪ�������鲥����ѯ������Ϣ����ѯeMBMS����״̬���ܣ��ܵ�����MBMS��������ʹ�ܻ�ȥʹ������Ӱ�죬ֻ����MBMS��������ʹ�ܵ�����²���ִ����ع��ܣ�����ֱ�ӷ��ش�����Ϣ��
     * [�﷨]:
     *     [����]: ^MBMSCMD=<cmd>[,<param1>[,<param2>]]
     *     [���]: ����ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ִ�д��������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *             ��ѯִ�н����
     *             <CR><LF>^MBMSCMD: <ResponseType><CR><LF><CR><LF>OK/+CME ERROR: <err><CR><LF>
     *     [����]: ^MBMSCMD?
     *     [���]: <CR><LF>^MBMSCMD: <AreaID>,<TMGI>[,<SessionID>] <CR><LF>^MBMSCMD: <AreaID>,<TMGI>[,<SessionID>] <CR><LF>
     *             ��
     *             ^MBMSCMD: <AreaID>,<TMGI>[,<SessionID>] <CR><LF>
     *             <CR><LF>OK/+CME ERROR:<err><CR><LF>
     *     [����]: ^MBMSCMD=?
     *     [���]: <CR><LF>^MBMSCMD: List of supported <cmd><CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <cmd>: �ַ������ͣ�����MBMS�������ƣ�Ŀǰ��֧�������������ƣ�
     *             "MBMS_SERVICE_ENABLER"��ʹ�ܻ�ȥʹ��MBMS����
     *             "ACTIVATE"������MBMS����
     *             "DEACTIVATE"��ȥ����MBMS����
     *             "DEACTIVATE_ALL"��ȥ�������е�MBMS����
     *             "MBMS_PREFERENCE"������MBMS�㲥ģʽΪ�������鲥��
     *             "SIB16_GET_NETWORK_TIME"����ѯSIB16����ʱ�䣻
     *             "BSSI_SIGNAL_LEVEL"����ѯBSSI�ź�ǿ�ȣ�
     *             "NETWORK INFORMATION"����ѯ������Ϣ��
     *             "MODEM_STATUS"����ѯeMBMS����״̬��
     *     <param1>: ʹ�ܻ�ȥʹ��MBMS����ʱ����ʾMBMS�������Բ����������ͣ�ȡֵ0��1��
     *             0��ȥʹ�ܣ�
     *             1��ʹ�ܡ�
     *             ����/ȥ����MBMS����ʱ����ʾArealD�������͡�
     *             ����MBMS�㲥ģʽΪ�������鲥ʱ����ʾ�㲥ģʽ�������ͣ�ȡֵ0��1��
     *             0��������
     *             1���鲥��
     *     <param2>: ����/ȥ����MBMS����ʱ����ʾTMGI��Ϣ���ַ���������MBMS Service ID��PLMN ID��
     *     <ResponseType>: ��ѯSIB16����ʱ��ʱ����ʾ<TimingInfo>��UTCʱ�䣨0-549755813887��������ֵ��
     *             ��ѯBSSI�ź�ǿ��ʱ����ʾ<BSSI signal level>��BSSI�ź�ǿ�ȣ������ͣ�255(0xFF)��ʾBSSI�ź�ǿ����Ч��
     *             ��ѯ������Ϣʱ����ʾ<cell_id>��С��ID�������͡�
     *             ��ѯeMBMS����״̬ʱ����ʾ<modem status>�������ͣ�ȡֵ0��1��
     *             0��eMBMS���Թ��ܹرգ�
     *             1��eMBMS���Թ��ܿ�����
     *     <AreaID>: Area ID�������͡�
     *     <TMGI>: TMGI���ַ���������MBMS Service ID��PLMN ID��
     *     <SessionID>: SessionID�������͡�
     * [ʾ��]:
     *     �� ��������ʹ�ܻ�ȥʹ��MBMS�������Գɹ�
     *       AT^MBMSCMD="MBMS_SERVICE_ENABLER",1
     *       OK
     *     �� ��������MBMS����״̬Ϊȥ����ɹ�
     *       AT^MBMSCMD="DEACTIVATE_ALL"
     *       OK
     *     �� ��ѯMBMS�����б�
     *       AT^MBMSCMD?
     *       ^MBMSCMD: 1,10000146000,1
     *       ^MBMSCMD: 2,10000246001,2
     *       ^MBMSCMD: 3,10000346002,3
     *       OK
     *     �� ��������
     *       AT^MBMSCMD=?
     *       ^MBMSCMD: ("MBMS_SERVICE_ENABLER","ACTIVATE","DEACTIVATE","DEACTIVATE_ALL","MBMS_PREFERENCE","SIB16_GET_NETWORK_TIME","BSSI_SIGNAL_LEVEL","NETWORK INFORMATION","MODEM_STATUS")
     *       OK
     *     �� ����MBMS�㲥ģʽΪ�鲥�ɹ�
     *       AT^MBMSCMD="MBMS_PREFERENCE",1
     *       OK
     *     �� ��ѯSIB16����ʱ��
     *       AT^MBMSCMD="SIB16_GET_NETWORK_TIME"
     *       ^MBMSCMD: 124343
     *       OK
     *     �� ��ѯBSSI�ź�ǿ��
     *       AT^MBMSCMD="BSSI_SIGNAL_LEVEL"
     *       ^MBMSCMD: 65
     *       OK
     *     �� ��ѯ������Ϣ
     *       AT^MBMSCMD="NETWORKINFORMATION"
     *       ^MBMSCMD: 6
     *       OK
     *     �� ��ѯeMBMS����״̬
     *       AT^MBMSCMD="MODEM_STATUS"
     *       ^MBMSCMD: 1
     *       OK
     */
    { AT_CMD_MBMSCMD,
      AT_SetMBMSCMDPara, AT_SET_PARA_TIME, AT_QryMBMSCmdPara, AT_QRY_PARA_TIME, At_TestMBMSCMDPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^MBMSCMD", (TAF_UINT8 *)MBMSCMD_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����MBMS�����ϱ�
     * [˵��]: ��������LTE�²�֧�֣������ϲ�����MBMS�����ϱ�����������߲��Բ�����modem��MBMS�����ϱ�����Ĭ���ǹرյģ�ÿ�����ػ���ָ�ΪĬ��ֵ����Ҫ���¿�����MBMS�����ϱ��������óɹ���ֱ�ӷ���OK���ڹػ�״̬�»�������ʧ������·��ش�����Ϣ��
     *         ֻ��MBB֧�֣��ֻ���֧�֡�
     *         ����������ϱ�MBMS�����¼����ܵ�����MBMS�����ϱ�����Ӱ�죬ֻ���������ϱ����ô򿪵�����²��������ϱ�MBMS�����¼���
     * [�﷨]:
     *     [����]: ^MBMSEV=<cmd>
     *     [���]: ����ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ִ�д��������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^MBMSEV=?
     *     [���]: <CR><LF>^MBMSEV: List of supported <cmd><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: (unsolicitedresultcode)
     *     [���]: ^MBMSEV: <event>
     * [����]:
     *     <cmd>: �����ϱ����ã������ͣ�ȡֵ0��1��
     *             0���رգ�
     *             1���򿪣�
     *     <event>: �����ͣ�ȡֵ0-99��
     *             0��Service change event��
     *             1��No service (No LTE coverage)��
     *             2��Only unicast service available��
     *             3��LTE unicast and eMBMS service available��
     *             4-99��Reserved��
     * [ʾ��]:
     *     �� ����MBMS�����ϱ��ɹ�
     *       AT^MBMSEV=1
     *       OK
     *     �� ִ�в�������
     *       AT^MBMSEV=?
     *       ^MBMSEV: (0,1)
     *       OK
     */
    { AT_CMD_MBMSEV,
      AT_SetMBMSEVPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^MBMSEV", (TAF_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����Interest�б�
     * [˵��]: ֻ��MBB֧�֣��ֻ���֧�֡�
     *         ��������LTE�²�֧�֣������ϲ�����Interest�б����������modem�������óɹ�״̬��ֱ�ӷ���OK���ڹػ�״̬�»�������ʧ��״̬�·��ش�����Ϣ��
     * [�﷨]:
     *     [����]: ^MBMSINTERESTLIST=<freq1>,<freq2>,
     *             <freq3>,<freq4>,<freq5>,<mbms_priority>
     *     [���]: ����ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ִ�д��������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <freq1>,<freq2>,\n<freq3>,<freq4>,<freq5>: interest�б������ͣ�32λ��Ƶ�㡣Ĭ��ֵΪ0��
     *     <mbms_priority>: Mbms�������ޱ�ʶ�������ͣ�ȡֵ0��1��
     *             0���������ޣ�
     *             1��mbms���ȣ�
     * [ʾ��]:
     *     �� ����Interest�б�ɹ���mbms����
     *       AT^MBMSINTERESTLIST=0,0,0,0,0,1
     *       OK
     */
    { AT_CMD_MBMSINTERESTLIST,
      AT_SetMBMSInterestListPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^MBMSINTERESTLIST", (TAF_UINT8 *)"(@freqone),(@freqtwo),(@freqthree),(@freqfour),(@freqfive),(0,1)" },
#endif

    /*
     * [���]: Э��AT-LTE���
     * [����]: ���õ͹���
     * [˵��]: ��������LTE�²�֧�֣������ϲ����õ͹���������߲��Բ�����modem���͹������óɹ�ֱ�ӷ���OK������ʧ�ܻ��߹ػ�״̬�·��ش�����Ϣ��
     * [�﷨]:
     *     [����]: ^LTELOWPOWER=<low_power>
     *     [���]: ����ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ִ�д��������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LTELOWPOWER=?
     *     [���]: <CR><LF>^LTELOWPOWER: List of supported < low_power ><CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <low_power>: �͹��ı�ʶ�������ͣ�ȡֵ0��1��
     *             0��Normal��
     *             1��Low Power Consumption��
     * [ʾ��]:
     *     �� ���õ͹��ĳɹ�
     *       AT^LTELOWPOWER=1
     *       OK
     *     �� ִ�в�������
     *       AT^LTELOWPOWER=?
     *       ^LTELOWPOWER: (0,1)
     *       OK
     */
    { AT_CMD_LTELOWPOWER,
      AT_SetLteLowPowerPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^LTELOWPOWER", (TAF_UINT8 *)"(0,1)" },
#endif

#if (FEATURE_LTE == FEATURE_ON)
    { AT_CMD_RSRPCFG,
      AT_SetRsrpCfgPara, AT_NOT_SET_TIME, AT_QryRsrpCfgPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RSRPCFG", (VOS_UINT8 *)"(0-3),(0-200)" },

    { AT_CMD_RSCPCFG,
      AT_SetRscpCfgPara, AT_NOT_SET_TIME, AT_QryRscpCfgPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RSCPCFG", (VOS_UINT8 *)"(0-3),(0-200)" },

    { AT_CMD_ECIOCFG,
      AT_SetEcioCfgPara, AT_NOT_SET_TIME, AT_QryEcioCfgPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ECIOCFG", (VOS_UINT8 *)"(0-3),(0-200)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: FR��̬����
     * [˵��]: ���������ڿ����͹ر�FAST RETURN���ܡ�
     * [�﷨]:
     *     [����]: ^FRSTATUS=<n>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FRSTATUS=?
     *     [���]: <CR><LF>^FRSTATUS: (list of supported < n >s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��FAST RETURN���ܿ��ء�
     *             0���رգ�
     *             1��������
     * [ʾ��]:
     *     �� ����FAST RETURN
     *       AT^FRSTATUS=1
     *       OK
     *     �� ����FAST RETURN
     *       AT^FRSTATUS=?
     *       ^FRSTATUS: (0-1)
     *       OK
     */
    { AT_CMD_FRSTATUS,
      AT_SetFrStatus, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FRSTATUS", (VOS_UINT8 *)"(0-1)" },
#endif
#if (FEATURE_MBB_CUST == FEATURE_ON)
    { AT_CMD_EONS,
      AT_SetEonsPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestEonsPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^EONS", (VOS_UINT8 *)"(1,2,3,4,5),(@plmn),(1-128)" },

    { AT_CMD_NWSCAN,
      AT_SetNwScanPara, AT_SYSCFG_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^NWSCAN", (VOS_UINT8 *)"(0-8),(@band),(0-65535)" },

    { AT_CMD_CELLROAM,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryCellRoamPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CELLROAM", VOS_NULL_PTR },

    { AT_CMD_NWTIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryTimeQryPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
     (VOS_UINT8 *)"^NWTIME", VOS_NULL_PTR },

    { AT_CMD_HCSQ,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryHcsqPara, AT_QRY_PARA_TIME, AT_TestHcsqPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HCSQ", VOS_NULL_PTR },

    { AT_CMD_NETSELOPT,
      AT_SetNetSelOptPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NETSELOPT", (VOS_UINT8 *)"(1),(0-1)" },
#endif
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯSIMLock Manager�汾��
     * [˵��]: ���ڲ�ѯ���������֧�ֵ�SIMLock Manager�汾�ţ�Hi6910���ն������V2�汾���㷨��
     * [�﷨]:
     *     [����]: ^AUTHVER?
     *     [���]: <CR><LF>^AUTHVER: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �е�����ش���ʱ��
     *             <CR><LF> ERROR<CR><LF>
     * [����]:
     *     <value>: SIMLock Manager�汾�ţ�
     *             0����ʾ�ӿڲ�֧�֣��ް汾�ţ�
     *             1����ʾ�������ڵİ汾���������ȡ����1.0�汾��
     *             2����ʾ2.0�汾����ȡ�����Ż�֮��İ汾��ȡ���룻
     *             ������Ϊ����ֵ��
     */
    { AT_CMD_AUTHVER,
      TAF_NULL_PTR, AT_NOT_SET_TIME, AT_QryAuthverPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^AUTHVER", TAF_NULL_PTR },
};

/* ע��TAFװ��AT����� */
VOS_UINT32 AT_RegisterCustomTafCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomTafCmdTbl, sizeof(g_atCustomTafCmdTbl) / sizeof(g_atCustomTafCmdTbl[0]));
}

