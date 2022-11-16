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
#include "at_custom_lnas_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_lnas_set_cmd_proc.h"
#include "at_custom_lnas_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_LNAS_CMD_TBL_C

static const AT_ParCmdElement g_atCustomLnasCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���úͲ�ѯԤ��Ƶ��Ƶ����Ϣ
     * [˵��]: ^MCC�ϱ�֮��AP�·���MCC��Ӧ��Ԥ��Ƶ��Ƶ����Ϣ��
     * [�﷨]:
     *     [����]: ^MCCFREQ=<SEQ>,<VER>,<OPERATION>,<LENGTH>[[,<BS1>[,<BS2>[,<BS3>]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^MCCFREQ?
     *     [���]: <CR><LF>^MCCFREQ: <VER><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^MCCFREQ=?
     *     [���]: <CR><LF>^MCCFREQ: (list of supported <SEQ>s),(str),(list of supported <OPERATION>s),(list of supported <LENGTH>s),(str),(str),(str)<CR><LF><CR>
     *             <LF>OK<CR><LF>
     * [����]:
     *     <SEQ>: ��ˮ�ţ�ȡֵ��ΧΪ1��255�����һ��Ϊ255����ֻ��һ������Ϊ255
     *     <VER>: ��ͨѶƥ��Ƶ��/Ƶ�ΰ汾��
     *     <OPERATION>: 0: ����
     *             1: �h��ȫ��
     *             2: �h��ĳMCC
     *             ɾ��ȫ����ʱ�򣬳�����Ҫ����Ϊ0���Ҳ��ܰ���BS1��BS2��BS3��
     *     <LENGTH>: ����������BS1��BS2��BS3�����ܳ��ȣ����ȷ�Χ0~1500��BS1��BS2,BS3�����У�Ҳ����û�С����LENGTH���Ȳ���������������BS1��BS2��BS3�����ܳ��ȣ�ֱ�ӷ���ʧ�ܡ�
     *     <BS1>: ��������1��ʮ�������ֽ�����������ʮ�������ַ����ʾһ���ֽ�;���Ȳ�����500
     *     <BS2>: ��������2��ͬ<BS1>
     *     <BS3>: ��������3��ͬ<BS1>
     * [ʾ��]:
     *     �� ����Ԥ��Ƶ��Ƶ����Ϣ
     *       AT^MCCFREQ=255,"00.00.001",0,690,"64F004FF0005030001017C02000000020000080002015E0000000100000001FF030000FC030000260003040C9400000C94000004D2940000D294000004D4940000D494000027000504CE950000CE95000004009600000096000004049600000496000004649600006496000004909600009096000028000304269800002698000004EC980000EC980000045299000052990000FF1004010006028E2900008E29000002A7290000A729000002C0290000C029000002D9290000D929000002F2290000F2290000020B2A00000B2A000003000201AE0200007C020000047206000072060000080002017C0000005F000000020D0C00000D0C000029","000104949D0000949D0000FF3002010002044B0000004B000000046400000064000000030001042107000021070000FF1103010002044B0000004B00000004640000006400000003000104210700002107000029000104B4A00000B4A00000"
     *       OK
     *     �� ɾ������Ԥ��Ƶ��Ƶ����Ϣ
     *       AT^MCCFREQ=255,"00.00.001",1,0
     *       OK
     *     �� ��ѯ����
     *       AT^MCCFREQ?
     *       ^MCCFREQ: 00.00.001
     *       OK
     *     �� ��������
     *       AT^MCCFREQ=?
     *       ^MCCFREQ: (1-255),(str),(0-2),(0-1500),(str),(str),(str)
     *       OK
     */
    { AT_CMD_MCCFREQ,
      AT_SetMccFreqPara, AT_SET_PARA_TIME, AT_QryMccFreqPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^MCCFREQ", (TAF_UINT8 *)"(1-255),(str),(0-2),(0-1500),(str),(str),(str)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ������ͨ�������Ϣ
     * [˵��]: ATת��AP�·���CSSģ�����ͨ�����������Ϣ��
     * [�﷨]:
     *     [����]: ^CLOUDDATA=<LENGTH>[[,<BS1>[,<BS2>[,<BS3>]]]]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <LENGTH>: ����������BS1��BS2��BS3�����ܳ��ȣ����ȷ�Χ0~1500�� BS2,BS3�����У�Ҳ����û�С�����BS1��BS2��BS3�豣֤�������������ʱ��ǰ�߱�����������䡣���磺���ܳ���BS1û�����ݣ��������������BS2������BS3��������������LENGTH���Ȳ���������������BS1��BS2��BS3�����ܳ��ȣ�ֱ�ӷ���ʧ�ܡ�������Ϊ0�����ʾû��������ֱ�ӷ���ʧ�ܡ�
     *     <BS1>: ��������1��ASCII�����������������Ȳ�����500��
     *     <BS2>: ��������2��ͬ<BS1>
     *     <BS3>: ��������3��ͬ<BS1>
     * [ʾ��]:
     *     �� ������ͨ�������Ϣ
     *       AT^CLOUDDATA=112,"00000000060000000000000000FF30312E30322E3030330406000001000F0001020200041200230045000000560000002301000034020000"
     *       OK
     *     �� ��������
     *       AT^CLOUDDATA=?
     *       ^CLOUDDATA: (0-1500),(str),(str),(str)
     *       OK
     */
    { AT_CMD_CLOUDDATA,
      AT_SetCloudDataPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^CLOUDDATA", (TAF_UINT8 *)"(0-1500),(str),(str),(str)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���úͲ�ѯ��ͨ������С����Ϣ
     * [˵��]: ^REPORTBLOCKCELLMCC�����ϱ�֮��AP�·���MCC��Ӧ����ͨ������С����Ϣ��
     *         ע�⣺�������Ƿ�֧����NV��52008�����ơ�
     * [�﷨]:
     *     [����]: ^BLOCKCELLLIST=<SEQ>,<VER>,<OPERATION>,<SYSMODE>,<LENGTH>[[,<BS1>[,<BS2>[,<BS3>]]]]
     *             <CR><LF>OK<CR><LF>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^BLOCKCELLLIST?
     *     [���]: <CR><LF>^BLOCKCELLLIST: <VER>[,<MCC>,<MCC>��..]
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *             ---��ע:Ϊ����֤�汾���Ƿ�����˸ù���,��ʹmodem NV���ܹر�,modem��Ҳ�᷵�سɹ�,�汾����00.00.000
     *     [����]: ^BLOCKCELLLIST=?
     *     [���]: <CR><LF>^BLOCKCELLLIST: (list of supported <SEQ>s),(str),(list of supported <OPERATION>s),(list of supported <SYSMODE>s),(list of supported  <LENGTH>s),(str),(str),(str)<CR><LF><CR <LF>OK<CR><LF>
     * [����]:
     *     <SEQ>: ��ˮ�ţ�ȡֵ��ΧΪ1��255�����һ��Ϊ255;����ֻ��һ��
     *             ��Ϊ255
     *     <VER>: ��ͨѶƥ��汾��
     *     <OPERATION>: 0: ����
     *             1: �h��ȫ����
     *     <SYSMODE>: ϵͳ��ʽ��
     *             0��GSM��
     *             1��WCDMA��
     *             2��LTE;
     *             ����Ĭ�Ͻ�����GSM
     *     <LENGTH>: ����������BS1��BS2��BS3�����ܳ��ȣ����ȷ�Χ0~1500��
     *             BS1��BS2,BS3�����У�Ҳ����û�С����LENGTH���Ȳ���������������BS1��BS2��BS3�����ܳ��ȣ�ֱ�ӷ���ʧ�ܡ�ɾ��ȫ����ʱ�򣬳�����Ҫ����Ϊ0���Ҳ��ܰ���BS1��BS2��BS3��
     *     <BS1>: ��������1��ʮ�������ֽ�����������ʮ�������ַ����ʾһ���ֽ�;���Ȳ�����500; С��ģʽ:����int32��ֵ0x01020304�·���������04030201
     *     <BS2>: ��������2��ͬ<BS1>
     *     <BS3>: ��������3��ͬ<BS1>
     *     <MCC>: MCC��Ϣ������һ����Ҳ���ܶ����Ŀǰ���17���������ʱ�ԡ����š�����
     * [ʾ��]:
     *     �� ������ͨ������С����Ϣ, ����һ��BS1�Ϳ��Դ���
     *       AT^BLOCKCELLLIST=255,"00.00.001",0,0,34,"0203000000020F000102109E519B00250A"
     *       OK
     *     �� ������ͨ������С����Ϣ, ��Ҫ����������BS
     *       AT^BLOCKCELLLIST=255,"00.00.111",0,0,690,"0406000000010F002A6037AD00AE00AF0A6137AE00AF00B00A6237AF00B000B10A6337B000B100B20A6437B100B200B30A6537B200B300B40A6637B300B400B50A6737B400B500B60A6837B500B600B70A6937B600B700B80A6A37B700B800B90A6B37B800B900BA0A6C37B900BA00BB0A6D37BA00BB00BC0A6E37BB00BC00BD0A6F37BC00BD00BE0A7037BD00BE00BF0A7137BE00BF00C00A7237BF00C000C10A7337C000C100C20A7437C100C200C30A7537C200C300C40A7637C300C400C50A7737C400C500C60A7837C500C600C70A7937C600C700C80A7A37C700C800C90A7B37C800C900CA0A7C37C900CA00CB0A7D37CA00CB00CC0A7E","37CB00CC00CD0A7F37CC00CD00CE0A8037CD00CE00CF0A8137CE00CF00D00A8237CF00D000D10A8337D000D100D20A8437D100D200D30A8537D200D300D40A8637D300D400D50A8737D400D500D60A8837D500D600D70A8937D600D700D80A"
     *       OK
     *     �� ɾ��ȫ����ͨ������С����Ϣ
     *       AT^BLOCKCELLLIST=255,"00.00.000",1,0,0
     *       OK
     *     �� ��ѯ��ͨ������С����Ϣ
     *       AT^BLOCKCELLLIST?
     *       ^BLOCKCELLLIST: "00.00.001",460,230
     *       OK
     *     �� ��������
     *       AT^BLOCKCELLLIST=?
     *       ^BLOCKCELLLIST: (1-255),(str),(0-1),(0-2),(0-1500),(str),(str),(str)
     *       OK
     */
    { AT_CMD_BLOCKCELLLIST,
      AT_SetBlockCellListPara, AT_SET_PARA_TIME, AT_QryBlockCellListPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^BLOCKCELLLIST", (VOS_UINT8 *)"(1-255),(str),(0-1),(0-2),(0-1500),(str),(str),(str)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: AP��GPS��λ���ܿ�������
     * [˵��]: ���������ڽ�AP��GPS��λ����״̬֪ͨ��CP�ࡣ
     * [�﷨]:
     *     [����]: ^GPSLOCSET=<loc_permit>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^GPSLOCSET=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <loc_permit>: ����ֵ��AP��GPS��λ����״̬��
     *             0���رն�λ������
     *             1��������λ������Ĭ��ֵ����
     * [ʾ��]:
     *     �� ������λ����
     *       AT^GPSLOCSET=1
     *       OK
     */
    { AT_CMD_GPSLOCSET,
      AT_SetGpsLocSetPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^GPSLOCSET", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_GAMEMODE,
      AT_SetGameModePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^GAMEMODE", (VOS_UINT8 *)"(0,1)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ������ͨѶ��������
     * [˵��]: ��������������Ĭ�ϲ��������Լ������ض�PLMN�Ĳ������ԡ�
     *         ���������֧��NR�����Ĳ�Ʒ�Ͽ��á�
     * [�﷨]:
     *     [����]: ^NWDEPLOYMENT=
     *             <seq>,<version>,<BS_count>,<BS1>[,<BS2>[,<BS3>]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <seq>: �޷���8λ����ֵ����ˮ�ţ�ȡֵ��ΧΪ1��255�����һ��Ϊ255����ֻ��һ������Ϊ255��
     *     <version>: �ַ������ͣ���ͨѶ�������԰汾�ţ��̶���ʽΪxx.xx.xxx��
     *     <BS_count>: ����������ȡֵ��ΧΪ��1~3����
     *     <BS1>: ��������1���ַ�����ʽ�����Ȳ�����500��������ʽ��<strategy>,<roamingFlag>,<PLMNNUM>[,<PLMNx>,����]��
     *     <BS2>: ��������2���ַ�����ʽ�����Ȳ�����500��BS_countΪ2��3ʱ��BS2����Ч��BS2�в�����<strategy>,<roamingFlag>��<PLMNNUM>����BS1��<PLMNx>������������
     *     <BS3>: ��������3���ַ�����ʽ�����Ȳ�����500��BS_countΪ3ʱ��BS3����Ч��BS3�в�����<strategy>,<roamingFlag>��<PLMNNUM>����BS2��<PLMNx>������������
     *     <strategy>: �����������ͣ�һ��BS1ֻ����һ���������ͣ�һ��������������PLMN��������AT������·����ܳ���ԭ����Բ�ֳɶ���AT�����������ȡֵ���£�
     *             0��LTE_ONLY
     *             1��NSA_ONLY
     *             2��SA_PREFER
     *             3��NSA_PREFER
     *     <roamingFlag>: ��ʾ���������Ƿ��������Σ�
     *             0���ò����������ͷ�����
     *             1���ò�������������������
     *     <PLMNNUM>: ��������������������PLMN���������ж���PLMNʹ�øò������ԡ������ֵΪ0����˵��<strategy>���õ���Ĭ�ϵĲ����������ͣ�
     *     <PLMNx>: ��������������������PLMN������<PLMNNUM>��Ϊ0ʱ�ſ������á�
     * [ʾ��]:
     *     �� ֻ��������Ĭ�ϲ���ΪSA_PREFER:
     *       AT^NWDEPLOYMENT=255,"10.10.141",1,"2,1,0"
     *       OK
     *     �� ֻ���÷�����Ĭ�ϲ���ΪSA_PREFER:
     *       AT^NWDEPLOYMENT=255,"10.10.141",1,"2,0,0"
     *       OK
     *     �� ��������Ĭ�ϲ���ΪSA_PREFER������������PLMN 46010ΪNSA_ONLY:
     *       AT^NWDEPLOYMENT=1,"10.10.666",1,"2,1,0"
     *       OK
     *       AT^NWDEPLOYMENT=255,"10.10.666",1,"1,1,1,46010"
     *       OK
     *     �� ���÷�����Ĭ�ϲ���ΪSA_PREFER�������÷�����PLMN 46010ΪNSA_ONLY��������PLMN 46020��46030ΪNSA_PREFER��������PLMN 460510ΪLTE_ONLY:
     *       AT^NWDEPLOYMENT=1,"10.10.666",1,"2,0,0"
     *       OK
     *       AT^NWDEPLOYMENT=2,"10.10.666",1,"1,0,1,46010"
     *       OK
     *       AT^NWDEPLOYMENT=3,"10.10.666",1,"3,0,2,46020,46030"
     *       OK
     *       AT^NWDEPLOYMENT=255,"10.10.666",1,"0,0,1,460510"
     *       OK
     */
    { AT_CMD_NWDEPLOYMENT,
      AT_SetNwDeploymentPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NWDEPLOYMENT", (VOS_UINT8 *)"(1-255),(str),(1-3),(str),(str),(str)" },
#endif

#if (FEATURE_LTE == FEATURE_ON)
    { AT_CMD_ISMCOEX,
      AT_SetIsmCoexPara, AT_SET_PARA_TIME, AT_QryIsmCoexPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^ISMCOEX", (TAF_UINT8 *)"(@bw),(@bw),(@bw),(@bw),(@bw),(@bw)" },
#endif

#if (FEATURE_MBB_CUST == FEATURE_ON)
    { AT_CMD_LTECAT,
    VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryLteCatPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
    VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8 *)"^LTECAT", VOS_NULL_PTR },
#endif

#if (FEATURE_APN_BASED_BO_TIMER_PARA_RPT == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: ����APN backoff��ʱ���ϱ�״̬�Ĺ��ܿ���
     * [˵��]: �����Ƿ�ʹ��+CABTSRI���������ϱ���
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error��
     *         2����������FEATURE_APN_BASED_BO_TIMER_PARA_RPT����ơ�
     * [�﷨]:
     *     [����]: +CABTSR=[<n>]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CABTSR?
     *     [���]: <CR><LF>+CABTSR: <n><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: +CABTSR=?
     *     [���]: <CR><LF>+CABTSR: (list of supported <n>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: �������ͣ���־�Ƿ�ʹ��+CABTSRI�����ϱ���Ĭ��Ϊ0��ȡֵ���£�
     *             0���ر�CABTSRI�����ϱ���
     *             1������CABTSRI�����ϱ���
     * [ʾ��]:
     *     �� ����������Ĭ�ϲ�����+CABTSRI�����ϱ�����
     *       AT+CABTSR=
     *       OK
     *     �� ����+CABTSRI�����ϱ�����
     *       AT+CABTSR=1
     *       OK
     */
    { AT_CMD_CABTSR,
      At_SetCabtsrPara, AT_SET_PARA_TIME, At_QryCabtsrPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CABTSR", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ȡAPN backoff��ʱ����ز���
     * [˵��]: ���ڲ�ѯTHROT�ϱ���APN backoff��ʱ�������Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error��
     *         2����������FEATURE_APN_BASED_BO_TIMER_PARA_RPT����ơ�
     * [�﷨]:
     *     [����]: +CABTRDP[=<apn>]
     *     [���]: ִ�����óɹ�ʱ��
     *             �����κβ����ϱ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���в���ʱ��
     *             <CR><LF>+CABTRDP: <apn>[,<residual_backoff_time>][,<re-attempt_rat_indicator>[,<re-attempt_eplmn_indicator>[,<NSLPI>[,<procedure>]]]][<CR><LF>+CABTRDP: <apn>[,<residual_backoff_time>][,<re-attempt_rat_indicator>[,<re-attempt_eplmn_indicator>[,<NSLPI>[,<procedure>]]]][...]]<CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CABTRDP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <apn>: APN��Ϣ���ַ������ͣ���󳤶�Ϊ99�����ظ��ϱ���apnΪ���ַ�ʱ��������š�
     *     <residual_backoff_time>: �ò�����ʾbackoff��ʱ��ʣ��ʱ������λ���롣�������͡�
     *             ע��
     *             ����ʱ������deactivated ״̬ʱ����ֵʡ�ԣ�������ţ�
     *             ����ʱ������ֹͣ���߳�ʱ״̬ʱ����ֵΪ0��
     *     <re-attempt_rat_indicator>: ��ϵͳ�Ƿ�������·���Ự�������̡��������ͣ�ȡֵ���£�
     *             0����ϵͳ���������·���Ự�������̣�
     *             1����ϵͳ�󣬲��������·���Ự�������̡�
     *     <re-attempt_eplmn_indicator>: eplmn���Ƿ���Է���Ự�������̡��������ͣ�ȡֵ���£�
     *             0: ����eplmn���Ƿ���Է���Ự��������;
     *             1: ������eplmn���Ƿ���Է���Ự�������̡�
     *     <NSLPI>: ָʾ��������ȼ����������ͣ�ȡֵ���£�
     *             0������Ϊ��������ȼ�;
     *             1��û������Ϊ��������ȼ���
     *     <procedure>: ָʾ�ܵ�backoff��ʱ��Ӱ��ĻỰ�������̡��������ͣ�ȡֵ���£�
     *             0: backoff��ʱ�������������̣�
     *             1: backoff��ʱ������STAND ALONE PDN���̣�
     *             2: backoff��ʱ������BEARER ALLOC���̣�
     *             3: backoff��ʱ������BEARER MODIFY���̣�
     *             4: backoff��ʱ������PDP_ACTIVE���̣�
     *             5: backoff��ʱ������SECOND_PDP_ACTIVE���̣�
     *             6: backoff��ʱ������PDP_MODIFY���̣�
     *             7: backoff��ʱ������PDU_ESTABLISH���̣�
     *             8: backoff��ʱ������PDU_MODIFY���̣�
     *             9: backoff��ʱ������ATTACH��STAND ALONE��PDN���̡�
     * [ʾ��]:
     *     �� APN����backoff��ʱ�������У����ڶ���APN bakeoff��Ϣʱ
     *       AT+CABTRDP
     *       +CABTRDP: "abc",100,1,1,1,0
     *       +CABTRDP: "abcD",115,1,1,1,0
     *       OK
     *     �� APN����backoff��ʱ������ʱ��deactivatedʱ
     *       AT+CABTRDP
     *       +CABTRDP: "abcD",,0,1,1,1
     *       OK
     *     �� backoff��ʱ��ֹͣ�ҽ���һ��apn��Ϣʱ
     *       AT+CABTRDP
     *       +CABTRDP: "abcd",0,,,,
     *       OK
     */
    { AT_CMD_CABTRDP,
      At_SetCabtrdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CABTRDP", (VOS_UINT8 *)"(str)" },
#endif

#if (FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: ���û���S-NSSAI��back-off��ʱ�������ϱ�״̬�Ĺ��ܿ���
     * [˵��]: �����Ƿ�ʹ��+CSBTSRI���������ϱ���
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1����������FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT�����
     *         2��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error
     * [�﷨]:
     *     [����]: +CSBTSR=[<n>]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CSBTSR?
     *     [���]: <CR><LF>+CABTSR: <n><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: +CSBTSR=?
     *     [���]: <CR><LF>+CSBTSR: (list of supported <n>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: �������ͣ���־�Ƿ�ʹ��+CSBTSRI�����ϱ���Ĭ��Ϊ0��ȡֵ���£�
     *             0���ر�CSBTSRI�����ϱ���
     *             1������CSBTSRI�����ϱ���
     * [ʾ��]:
     *     �� ����������Ĭ�ϲ�����+CABTSRI�����ϱ�����
     *       AT+CSBTSR=
     *       OK
     *     �� ����+CSBTSRI�����ϱ�����
     *       AT+CSBTSR=1
     *       OK
     */
    { AT_CMD_CSBTSR,
      At_SetCsbtsrPara, AT_SET_PARA_TIME, At_QryCsbtsrPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CSBTSR", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ȡAPN backoff��ʱ����ز���
     * [˵��]: ���ڲ�ѯTHROT�ϱ���S-NSSAI backoff��ʱ�������Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1����������FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT��FEATURE_UE_MODE_NR����ƣ�
     *         2��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error��
     * [�﷨]:
     *     [����]: +CSBTRDP[=<S-NSSAI>]
     *     [���]: ִ�����óɹ�ʱ��
     *             �����κβ����ϱ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���в���ʱ��
     *             <CR><LF>+CSBTRDP: <S-NSSAI>[,<S-NSSAI_backoff_time>][,<re-attempt_rat_indicator>[,<re-attempt_eplmn_indicator>[,<procedure>]]][<CR><LF>+CSBTRDP: <S-NSSAI>[,<S-NSSAI_backoff_time>][,<re-attempt_rat_indicator>[,<re-attempt_eplmn_indicator>[,<procedure>]]][...]]<CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CSBTRDP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <S-NSSAI>: 5GS�ķ�Ƭ��Ϣ���ַ������͡�
     *                ��Ƭ��ʽ������ʾ��
     *                1��sst
     *                2��sst.sd
     *                3��sst;mappedSst
     *                4��sst.sd;mappedSst
     *                5��sst.sd;mappedSst.mappedSd
     *                ע�⣺�ַ����е�����Ϊ16���ƣ��ַ�ȡֵ��Χ��0-9��A-F,a-f��
     *     <S-NSSAI_backoff_time>: �ò�����ʾS-NSSAI backoff��ʱ��ʣ��ʱ������λ���롣�������͡�
     *             ע��
     *             ����ʱ������deactivated ״̬ʱ����ֵʡ�ԣ�������ţ�
     *             ����ʱ������ֹͣ���߳�ʱ״̬ʱ����ֵΪ0��
     *     <re-attempt_rat_indicator>: ��ϵͳ�Ƿ�������·���Ự�������̡��������ͣ�ȡֵ���£�
     *             0����ϵͳ���������·���Ự�������̣�
     *             1����ϵͳ�󣬲��������·���Ự�������̡�
     *     <re-attempt_eplmn_indicator>: eplmn���Ƿ���Է���Ự�������̡��������ͣ�ȡֵ���£�
     *             0: ����eplmn���Ƿ���Է���Ự��������;
     *             1: ������eplmn���Ƿ���Է���Ự�������̡�
     *     <procedure>: ָʾ�ܵ�T3585��ʱ��Ӱ��ĻỰ�������̡��������ͣ�ȡֵ���£�
     *             0: backoff��ʱ�������������̣�
     *             1:��ʱ������PDU_ESTABLISH���̣�
     *             2:��ʱ������PDP_MODIFY���̡�
     * [ʾ��]:
     *     �� ��ѯ��S-NSSAI backoff��ʱ��������
     *       AT+CSBTRDP
     *       +CSBTRDP: ,160,0,1,0
     *       OK
     *     �� ��ѯ����S-NSSAI backoff��ʱ������deactivated״̬
     *       AT+CSBTRDP
     *       +CSBTRDP: ,,0,1,0
     *       OK
     *     �� ��ѯ��S-NSSAI backoff��ʱ��ֹͣ
     *       AT+CSBTRDP
     *       +CSBTRDP: ,0,,,
     *       OK
     */
    { AT_CMD_CSBTRDP,
      At_SetCsbtrdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CSBTRDP", (VOS_UINT8 *)"(dnn)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ���û���S-NSSAI��DNN��back-off��ʱ�������ϱ�״̬�Ĺ��ܿ���
     * [˵��]: �����Ƿ�ʹ��+CSDBTSRI���������ϱ���
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1����������FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT�����
     *         2��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error
     * [�﷨]:
     *     [����]: +CSDBTSR=[<n>]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CSDBTSR?
     *     [���]: <CR><LF>+CABTSR: <n><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: +CSDBTSR=?
     *     [���]: <CR><LF>+CSDBTSR: (list of supported <n>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: �������ͣ���־�Ƿ�ʹ��+CSDBTSRI�����ϱ���Ĭ��Ϊ0��ȡֵ���£�
     *             0���ر�CSDBTSRI�����ϱ���
     *             1������CSDBTSRI�����ϱ���
     * [ʾ��]:
     *     �� ����������Ĭ�ϲ�����+CABTSRI�����ϱ�����
     *       AT+CSDBTSR=
     *       OK
     *     �� ����+CSDBTSRI�����ϱ�����
     *       AT+CSDBTSR=1
     *       OK
     */
    { AT_CMD_CSDBTSR,
      At_SetCsdbtsrPara, AT_SET_PARA_TIME, At_QryCsdbtsrPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CSDBTSR", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ȡAPN backoff��ʱ����ز���
     * [˵��]: ���ڲ�ѯTHROT�ϱ��Ļ���S-NSSAI��DNN��backoff��ʱ�������Ϣ��
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1����������FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT��FEATURE_UE_MODE_NR����ƣ�
     *         2��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error��
     *         3�����������3GPP TS 27.007Э�飬��ֻЯ��һ����������������ڣ���ǰЭ���������������ֲ�Ϊ׼��at+CSDBTRDP=,�ȼ���at+CSDBTRDP="",""
     * [�﷨]:
     *     [����]: +CSDBTRDP[=<S-NSSAI>,<DNN>]
     *     [���]: ִ�����óɹ�ʱ��
     *             �����κβ����ϱ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���в���ʱ��
     *             <CR><LF>+CSDBTRDP: <S-NSSAI>,<DNN>[,<S-NSSAI_DNN_backoff_time>][,<re-attempt_rat_indicator>[,<re-attempt_eplmn_indicator>[,<procedure>]]][<CR><LF>+CSDBTRDP: <S-NSSAI>,<DNN>[,<S-NSSAI_DNN_backoff_time>][,<re-attempt_rat_indicator>[,<re-attempt_eplmn_indicator>[,<procedure>]]][...]]<CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CSDBTRDP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <S-NSSAI>: 5GS�ķ�Ƭ��Ϣ���ַ������͡�
     *                ��Ƭ��ʽ������ʾ��
     *                1��sst
     *                2��sst.sd
     *                3��sst;mappedSst
     *                4��sst.sd;mappedSst
     *                5��sst.sd;mappedSst.mappedSd
     *                ע�⣺�ַ����е�����Ϊ16���ƣ��ַ�ȡֵ��Χ��0-9��A-F,a-f��
     *     <DNN>: DNN��Ϣ���ַ������ͣ���󳤶�Ϊ99��
     *     <S-NSSAI_DNN_backoff_time>>: �ò�����ʾ����S-NSSAI��DNN��backoff��ʱ��ʣ��ʱ������λ���롣�������͡�
     *             ע��
     *             ����ʱ������deactivated ״̬ʱ����ֵʡ�ԣ�������ţ�
     *             ����ʱ������ֹͣ���߳�ʱ״̬ʱ����ֵΪ0��
     *     <re-attempt_rat_indicator>: ��ϵͳ�Ƿ�������·���Ự�������̡��������ͣ�ȡֵ���£�
     *             0����ϵͳ���������·���Ự�������̣�
     *             1����ϵͳ�󣬲��������·���Ự�������̡�
     *     <re-attempt_eplmn_indicator>: eplmn���Ƿ���Է���Ự�������̡��������ͣ�ȡֵ���£�
     *             0: ����eplmn���Ƿ���Է���Ự��������;
     *             1: ������eplmn���Ƿ���Է���Ự�������̡�
     *     <procedure>: ָʾ�ܵ�backoff��ʱ��Ӱ��ĻỰ�������̡��������ͣ�ȡֵ���£�
     *             0: backoff��ʱ�������������̣�
     *             1:��ʱ������PDU_ESTABLISH���̣�
     *             2:��ʱ������PDP_MODIFY���̡�
     * [ʾ��]:
     *     �� ��ѯ������S-NSSAI��DNN��backoff��ʱ��������
     *       AT+CSDBTRDP
     *       +CSDBTRDP: "01.010101","huawei.com",32,0,1,0
     *       OK
     *     �� ��ѯ������S-NSSAI��DNN��backoff��ʱ������deactivated״̬
     *       AT+CSDBTRDP="","huawei.com"
     *       +CSDBTRDP: "01.010101","huawei.com",,0,1,0
     *       OK
     *     �� ��ѯ������S-NSSAI��DNN��backoff��ʱ��ֹͣ
     *       AT+CSDBTRDP
     *       +CSDBTRDP: ,"huawei.com",0,,,
     *       OK
     */
    { AT_CMD_CSDBTRDP,
      At_SetCsdbtrdpPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+CSDBTRDP", (VOS_UINT8 *)"(snssai),(dnn)" },
#endif

};

VOS_UINT32 AT_RegisterCustomLnasCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomLnasCmdTbl, sizeof(g_atCustomLnasCmdTbl) / sizeof(g_atCustomLnasCmdTbl[0]));
}

