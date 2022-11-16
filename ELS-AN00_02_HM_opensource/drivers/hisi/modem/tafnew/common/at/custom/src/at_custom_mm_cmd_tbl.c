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
#include "at_custom_mm_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_mm_set_cmd_proc.h"
#include "at_custom_mm_qry_cmd_proc.h"



#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_MM_CMD_TBL_C

static const AT_ParCmdElement g_atCustomMmCmdTbl[] = {
#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: RCS��̬��������
     * [˵��]: �������������úͲ�ѯRCS�Ķ�̬���ء�
     * [�﷨]:
     *     [����]: ^RCSSWITCH=<status>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^RCSSWITCH?
     *     [���]: <CR><LF>^RCSSWITCH: <status>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <status>: ����ֵ��RCS����״̬��
     *             0�� off��
     *             1�� on��
     * [ʾ��]:
     *     �� ��RCS��̬���سɹ���
     *       AT^RCSSWITCH=1
     *       OK
     *     �� ��ѯRCS��̬����״̬�ɹ���
     *       AT^RCSSWITCH?
     *       ^RCSSWITCH: 1
     *       OK
     */
    { AT_CMD_RCSSWITCH,
      AT_SetRcsSwitch, AT_SET_PARA_TIME, AT_QryRcsSwitch, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RCSSWITCH", (VOS_UINT8 *)"(0,1)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ��ǰIMSI�ź�HPLMN
     * [˵��]: ��ѯ��ǰIMSI�ź�HPLMN��
     * [�﷨]:
     *     [����]: ^HPLMN?
     *     [���]: <CR><LF> ^HPLMN: <IMSI>,<MNC Length>,<EHPLMN NUM>,<EHPLMN><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �����������ֵ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <IMSI>: IMSI��
     *     <MNC Length>: MNC���ȣ��ӿ��ļ��л�ȡ
     *     <EHPLMN NUM>: EHPLMN����
     *     <EHPLMN>: EHPLMN�б�
     * [ʾ��]:
     *     �� ��ѯ����
     *       AT^HPLMN?
     *       ^HPLMN: 46002123456789,2,2,46000,46007
     *       OK
     */
    { AT_CMD_HPLMN,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryHplmnPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^HPLMN", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���úͲ�ѯDPLMN�б�
     * [˵��]: ���ÿ���Ӧ��ȫ��Ԥ��DPLMN�б�
     *         ��ѯ֮ǰ��Ҫ��������ͨѶDPLMN�汾��NV(2360)Ϊ��Чֵ��
     * [�﷨]:
     *     [����]: ^DPLMNLIST=<SEQ>,<VER>,<EHPLMNNUM>,<EHPLMN>,<BSCOUNT>,<BS1>,[<BS2>,<BS3>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             �����������ֵ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^DPLMNLIST?
     *     [���]: <CR><LF>^DPLMNLIST: <VER><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^DPLMNLIST=?
     *     [���]: <CR><LF>^DPLMNLIST: (list of supported <SEQ>s),(str),(list of supported <EHPLMN NUM>s),(str),(list of supported <BS COUNT>s),(str),(str),(str)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <SEQ>: ��ˮ�ţ�ȡֵ��ΧΪ1��255�����һ��Ϊ255����ֻ��һ������Ϊ255
     *     <VER>: ��ͨѶDPLMN�汾��
     *     <EHPLMN NUM>: EHPLMN������ȡֵ��Χ1~16��
     *     <EHPLMN>: EHPLMN�б�
     *     <BS COUNT>: <BS>������ȡֵΪ1-3
     *     <BS1>: <DPLMN NUM>,<DPLMN>,<RAT>[,<DPLMN>,<RAT>,����]
     *             ����
     *             <DPLMN NUM>������AT������DPLMN�ܸ���
     *             DPLMN��RAT֮���ԡ����š�������RAT���¸�DPLMN(�����)֮��Ҳ�ԡ����š�����
     *             <RAT>��1: GSM (Bit1)
     *             2: WCDMA (Bit2)
     *             3: GW (Bit1,Bit2)
     *             4: LTE (Bit3)
     *             5: GL (Bit1,Bit3)
     *             6: WL (Bit2,Bit3)
     *             7: GWL(Bit1,Bit2,Bit3)
     *             8: NR(Bit4)
     *             9: G+NR(Bit1,Bit4)
     *             A:W+NR(Bit2,Bit4)
     *             B:G+W+NR(Bit1,Bit2,Bit4)
     *             C:L+NR(Bit3,Bit4)
     *             D:G+L+NR(Bit1,Bit3,Bit4)
     *             E:W+L+NR(Bit2,Bit3,Bit4)
     *             F:G+W+L+NR(Bit1,Bit2,Bit3,Bit4)
     *             ע��RATֵ����16��������д
     *     <BS2>: <DPLMN>,<RAT>,<DPLMN>,<RAT>,����
     *             BS COUNTΪ2��3ʱ��BS2����Ч��
     *             DPLMN��RAT֮���ԡ����š�������RAT���¸�DPLMN(�����)֮��Ҳ�ԡ����š�������
     *     <BS3>: <DPLMN>,<RAT>,<DPLMN>,<RAT>,����
     *             BS COUNTΪ3ʱ��BS3����Ч��
     *             DPLMN��RAT֮���ԡ����š�������RAT���¸�DPLMN(�����)֮��Ҳ�ԡ����š�������
     * [ʾ��]:
     *     �� ���ÿ���Ӧ��ȫ��Ԥ��DPLMN�б�
     *       AT^DPLMNLIST=01,"01.01.001",2,"46000,46007",1,"1,310410,2"
     *       OK
     *     �� ��ѯ����
     *       AT^DPLMNLIST?
     *       ^DPLMNLIST: 01.01.001
     *       OK
     *     �� ֧��NRʱ�����ÿ���Ӧ��ȫ��Ԥ��DPLMN�б�����֧��GULNR��
     *       AT^DPLMNLIST=01,"01.01.001",2,"46000,46007",1,"1,310410,F"
     *       OK
     */
    { AT_CMD_DPLMNLIST,
      AT_SetDplmnListPara, AT_SET_PARA_TIME, AT_QryDplmnListPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^DPLMNLIST", (VOS_UINT8 *)"(1-255),(str),(1-16),(str),(1-3),(str),(str),(str)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���õ��״̬
     * [˵��]: FRAT����ʱ�ĵ��״̬����ز�����
     * [�﷨]:
     *     [����]: ^FRATIGNITION=<STATUS>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FRATIGNITION?
     *     [���]: <CR><LF>^ FRATIGNITION: < STATUS > <CR><LF>
     *             <CR><LF> OK<CR><LF>
     *     [����]: ^FRATIGNITION=?
     *     [���]: <CR><LF>^ FRATIGNITION: (list of supported <STATUS>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <STATUS>: �����ͣ����״̬��ȡֵ0��1��
     *             0����
     *             1����
     * [ʾ��]:
     *     �� �������״̬
     *       AT^FRATIGNITION=1
     *       OK
     *     �� ��ѯ���״̬
     *       AT^FRATIGNITION?
     *       ^FRATIGNITION: 1
     *       OK
     */
    { AT_CMD_FRATIGNITION,
      AT_SetFratIgnitionPara, AT_SET_PARA_TIME, AT_QryFratIgnitionInfo, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^FRATIGNITION", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �߾���Ϣ����
     * [˵��]: ���ÿ���Ӧ��Ԥ�ñ߾���Ϣ�б�
     * [�﷨]:
     *     [����]: ^BORDERINFO=<OPERATRION>
     *             [,<SEQ>,<VER>,<EHPLMNNUM>,<EHPLMNLIST>,<COUNT>,<BS1>[,<BS2>[,<BS3��]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR: <err><CR><LF>
     *     [����]: ^BORDERINFO?
     *     [���]: <CR><LF>^BORDERINFO: <VER><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^BORDERINFO=?
     *     [���]: <CR><LF>^BORDERINFO: (list of supported < OPERATRION>s), (list of supported <SEQ>s),
     *             (str), (list of supported <EHPLMN NUM>s), (str), (list of supported <BS COUNT>s),(str),(str),
     *             (str)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <OPERATION>: 0: ����
     *             1: ɾ����ɾ�����еı߾���Ϣ������ɾ��ĳһ���߾���Ϣ��
     *             ɾ��ʱ����OPERATION�⣬���Բ��������Ĳ�����
     *     <SEQ>: ���к�: 01,02��FF
     *             ��Ϊ���һ������ֻ��һ���������к�ΪFF
     *     <VER>: �߾�������Ϣ�汾�ţ���9���ֽڱ�ʾ������"01.00.001"
     *     <EHPLMN NUM>: EHPLMN ����
     *     <EHPLMN LIST>: EHPLMN �б�
     *     <COUNT>: ������<COUNT>��BS����, <COUNT>ȡֵ��ΧΪ{1,2,3}
     *     <BS1>: <BS1>�����ĸ�ʽΪ<NumBorderPlmn>,<BorderArea>,<BorderArea>,����
     *             ����AT����ÿ�������ĳ��������ƣ�����<BS1>�а���<NumBorderPlmn>��M��<BorderArea>,���<BS1>���·��������еı߾���Ϣ����NumBorderPlmn����M��������<BorderArea>����<BS2>����<BS3>�����<BS2>�л��·��������еı߾���Ϣ)�У�<BS2>��<BS3>�в���<NumBorderPlmn>��
     *             ����
     *             <NumBorderPlmn>:����AT�����·��ı߾������а�����PLMN�ĸ���
     *             <BorderArea>:�����߾���������ÿ��plmn��LAI/TAI�Ľṹ����ʽΪ��{PLMN, Type,nLAC, LAC List}
     *             �������������£�
     *             PLMN: ��HPLMN���ڱ߾���ϵ��VPLMN
     *             Type:ָʾ����������GU LAC����LTE��TAC�����У�0��ʾLAC��1��ʾTAC
     *             nLAC:�˱߾������PLMN�µ�LAC/TAC����
     *             LAC List:����nLAC��LAC/TAC��ɵ�LIST��ÿ��LAC/TAC������ʮ�����Ʊ�ʾ�ġ�
     *     <BS2>: �����<BS2>��<BS2>�ĸ�ʽΪ<BorderArea>,<BorderArea>,����
     *     <BS3>: �����<BS3>��<BS3>�ĸ�ʽΪ<BorderArea>,<BorderArea>,����
     * [ʾ��]:
     *     �� ���ÿ���Ӧ��Ԥ�ñ߾���Ϣ�б�
     *       AT^BORDERINFO=0,255,"01.00.001",2,"46000,46002",1,"2,45400,0,2,47F,2F27,45401,1,1,104"
     *       OK
     *     �� ��ѯ����
     *       AT^BORDERINFO?
     *       ^BORDERINFO: 01.01.001
     *       OK
     */
    { AT_CMD_BORDERINFO,
      AT_SetBorderInfoPara, AT_SET_PARA_TIME, AT_QryBorderInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^BORDERINFO", (TAF_UINT8 *)"(0,1),(1-255),(str),(1-16),(str),(1-3),(str),(str),(str)" },

    /* ���ݸ���Ԥ����������AT����^EMRSSIRPT */
    /*
     * [���]: Э��AT-�绰����
     * [����]: RSSIԤ���ϱ�ʹ�ܿ���
     * [˵��]: ����������ʹ�ܻ��߽�ֹ^EMRSSIRPT�����ϱ������Ʒ���С��RSSI�仯������ֵʱ���Ƿ��ϱ�^EMRSSIRPT��
     * [�﷨]:
     *     [����]: ^EMRSSIRPT=<n>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^EMRSSIRPT?
     *     [���]: <CR><LF>^EMRSSIRPT: <n><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^EMRSSIRPT=?
     *     [���]: <CR><LF>^EMRSSIRPT: (list of supported <n>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��^EMRSSIRPT�ϱ�ʹ�ܿ��ء�
     *             0����ֹ^EMRSSIRPT�������ϱ���Ĭ��ֵ��
     *             1��ʹ��^EMRSSIRPT�������ϱ�
     * [ʾ��]:
     *     �� ʹ��^EMRSSIRPT�ϱ�����
     *       AT^EMRSSIRPT=1
     *       OK
     */
    { AT_CMD_EMRSSIRPT,
      AT_SetEmRssiRptPara, AT_SET_PARA_TIME, AT_QryEmRssiRptPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^EMRSSIRPT", (VOS_UINT8 *)"(0-1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: AP�·���������
     * [˵��]: ����������AP�·��������
     *         ������Ŀǰ��֧��AP�·�ָ����ʷ��LTE��NR��PLMN��
     * [�﷨]:
     *     [����]: ^PLMNSRCH=<isbgtype>,<plmn>,<rat>,<srchtype>[,<rsrpthreshold>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <isbgtype>: ����ֵ�����������Ƿ�ΪBG�ѣ�
     *             0��ָ���ѣ�
     *             1��BG�ѣ��ݲ�֧�֣���
     *     <plmn>: �ַ������ͣ�Ҫ���������ָ�ʽ��PLMN ID��
     *     <rat>: ����ֵ�����߽��뼼����
     *             0��GSM/GPRS��
     *             2��WCDMA��
     *             7��LTE��
     *             12��NR��
     *     <srchtype>: ����ֵ����չ���������ͣ�
     *             0��Full Band�ѣ�
     *             1��History�ѣ�
     *             2��Pref Band�ѣ��ݲ�֧�֣���
     *     <rsrpthreshold>: ����ֵ���ź�ǿ�������ֵ����ѡ��������LTE��NR֧�֡�
     *             ����<rat>ΪLTE��ʽʱ��ȡֵ��Χ44~141��
     *             ����<rat>ΪNR��ʽʱ��ȡֵ��Χ31~156��
     *             ˵��������Ӹò���ʱ������פ��С���ź�ǿ�ȱ������ָ����ֵ�������ָ����ʽ��PLMN(��NV2356��detectAndUserHisSrchActiveFlgΪ��״̬������ָ��PLMNΪHPLMNʱ�����ж�ָ��PLMN�Լ�EHPLMN)�в������ź�ǿ�ȸ��ڸ���ֵ��С��������ʧ�ܡ�
     * [ʾ��]:
     *     �� ��Я���ź�ǿ����ֵ��ָ��Full Band��LTE��ʽPlmn 46085��
     *       AT^PLMNSRCH=0,46085,7,0
     *       OK
     *     �� Я���ź�ǿ����ֵ��ָ����ʷ��LTE��ʽPlmn 46085��
     *       AT^PLMNSRCH=0,46085,7,1,100
     *       OK
     *     �� ��Я���ź�ǿ����ֵ��ָ����ʷ��NR��ʽPlmn 45403��
     *       AT^PLMNSRCH=0,45403,12,1
     *       OK
     *     �� Я���ź�ǿ����ֵ��ָ����ʷ��NR��ʽPlmn 46085��
     *       AT^PLMNSRCH=0,46085,12,1,120
     *       OK
     *     �� Я���ź�ǿ����ֵ��ָ����Χ��
     *       AT^PLMNSRCH=0,46085,7,1,142
     *     �� +CME ERROR: Incorrect parameters
     *       AT^PLMNSRCH=0,46085,7,1,10
     *       +CME ERROR: Incorrect parameters
     */
    { AT_CMD_PLMNSRCH,
      AT_SetPlmnSrchPara, AT_COPS_SPEC_SRCH_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PLMNSRCH", (VOS_UINT8 *)PLMNSRCH_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ������Ϣ�����ϱ�
     * [˵��]: ����������AP-Modem��̬��Modem�����ϱ�һ��������ʼ��������������Ϣ��������������ʱЯ�������������AT����������ϱ�����^CURC���ơ�
     *         �ϱ�������Ϊ������ʼʱ����Ϣ�е���������ֶ�ΪBUTTֵ����AT�������ϱ�ֻ��AT^CURC���ƣ�ʹ��^CURC��һ���ֽڵĵ�������λ��Ϊ����λ��
     * [�﷨]:
     *     [����]: һ��������Ϣ�������ϱ���������ʼ������������
     *     [���]: ^PLMNSELEINFO: <PLMN SELE FLAG>,<PLMN SELE RLST>
     * [����]:
     *     <PLMN SELE FLAG>: ������ʼ/���������ı�־λ��
     *             0��������ʼ��
     *             1������������
     *             2���������жϡ�
     *     <PLMN SELE RLST>: ����������ķ���״̬��
     *             0����������
     *             1�����Ʒ���
     *             2���޷���
     *             3���������ж�
     *             4����Чֵ���ϱ�������ʼʱЯ����ֵ��
     * [ʾ��]:
     *       ������ʼ
     *       ^PLMNSELEINFO: 0,4
     *       �����������������Ϊ��������
     *       ^PLMNSELEINFO: 1,0
     */
    { AT_CMD_PLMNSELEINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PLMNSELEINFO", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �ź�ǿ��̽�������������ϱ�
     * [˵��]: ����������AP�·����ź�ǿ��̽�����Ŀǰ��֧����GU��HRPD��������״̬�£�̽��ָ��PLMN��LTE��ʽ�µ�����ź�ǿ��(��NV2356��ucBack2LActiveFlgΪ��״̬������ָ��PLMNΪHPLMNʱ��NAS��Ĭ��̽��ָ��PLMN�Լ�EHPLMN)��
     * [�﷨]:
     *     [����]: ^DETECTPLMN=<plmn>,<rat>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^DETECTPLMN=?
     *     [���]: <CR><LF>^DETECTPLMN: (list of supported <plmn>s),(list of supported <rat>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <plmn>: �������ͣ�Ҫ������PLMN��Ϣ��
     *     <rat>: ����ֵ�����߽��뼼��(��ǰֻ֧��LTE)��
     *             0��GSM/GPRS��
     *             2��WCDMA��
     *             7��LTE;
     *             12��NR��
     * [ʾ��]:
     *     �� AP�·�̽������
     *       AT^DETECTPLMN=46085,7
     *       OK
     *       ^DETECTPLMN: 46085,7,-100
     *     �� AP�·�̽�����󲻺Ϸ���ֱ�ӷ���ʧ�ܣ����ٴ���̽����̣�
     *       AT^DETECTPLMN=46085,7,1
     *       +CME ERR:50
     */
    { AT_CMD_DETECTPLMN,
      At_SetDetectPlmnRsrp, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DETECTPLMN", (VOS_UINT8 *)"(@oper),(0,2,7,12)" },

#if (FEATURE_DSDS == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: DSDS״̬�����Ƿ�����ϱ�
     * [˵��]: ����������AP��modem�����Ƿ���������ϱ�DSDS��״̬��
     * [�﷨]:
     *     [����]: ^DSDSSTATE=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <enable>: ����ֵ��������ر�DSDS״̬�仯�����ϱ���
     *             0���ر������ϱ���
     *             1�����������ϱ���
     * [ʾ��]:
     *     �� ����DSDS״̬�仯�����ϱ����ܣ���ʱMODEM����DSDS3.0��Ȼ��MODEM��һ��DSDS״̬�����ϱ���
     *       AT^DSDSSTATE=1
     *       OK
     *       ^DSDSSTATE: 1
     *     �� ִ�в�������
     *       AT^DSDSSTATE=?
     *       ^DSDSSTATE: (0,1)
     *       OK
     */
    { AT_CMD_DSDSSTATE,
      AT_SetDsdsStatePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DSDSSTATE", (VOS_UINT8 *)"(0,1)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����PSҵ��״̬���ü���ѯ
     * [˵��]: ����������AP֪ͨModem������ǰ��PSҵ��Ŀ�ʼ�ͽ�����ͬʱָʾ��PSҵ�����ʱ�Ƿ���Ҫ����PS DETACH��
     *         ע�⣺
     *         1������������úͲ�ѯ����ֻ���Modem1��Ч��
     *         2��state=1ʱ��ps_detach  ����Ϊ0��
     *         3��������CLģʽ����Ч��
     *         4��PS DETACH����ֻ��G��W��ʽ����Ч��
     * [�﷨]:
     *     [����]: ^PSSCENE=<state>,<ps_detach>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^PSSCENE?
     *     [���]: ִ�в�ѯ�ɹ�ʱ��
     *             <CR><LF>^PSSCENE: (list of supported <state>), (list of supported <ps_detach>)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <state>: ����ֵ�����ø���PSҵ��״̬��ȡֵ��Χ 0~1��
     *             0����������PSҵ��
     *             1��������ʼPSҵ��
     *     <ps_detach>: ����ֵ�������Ƿ���Ҫִ��PS DETACH��
     *             0������ҪDETACH PS��
     *             1����ҪDETACH PS��
     * [ʾ��]:
     *     �� ָʾ����PSҵ��ʼ
     *       AT^PSSCENE=1,0
     *       OK
     *     �� ָʾ����PSҵ�����������PS��DETACH
     *       AT^PSSCENE=0,1
     *       OK
     *     �� ��ѯ����PSҵ��״̬
     *       AT^PSSCENE?
     *       ^PSSCENE��1
     *       OK
     *     �� ִ�в�������
     *       AT^PSSCENE=?
     *       ^PSSCENE: (0,1),(0,1)
     *       OK
     */
    { AT_CMD_PSSCENE,
      At_SetPsScenePara, AT_SET_PARA_TIME, AT_QryPsScenePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^PSSCENE", (VOS_UINT8 *)"(0,1),(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����̽���Ź��������ر�
     * [˵��]: �����������·�����̽���Ź��������͹رա�
     *         ע����������Ҫ��CGCATT���������·����������ʧ�ܣ��ϲ���Ҫ���������·���ָʾ��
     * [�﷨]:
     *     [����]: ^SMSANTIATTACK=<state>
     *     [���]: ����ֵ
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <state>: ����ֵ���·��ķ���̽����״̬��
     *             0���������رգ�
     *             1��������������
     * [ʾ��]:
     *       AT^SMSANTIATTACK=1
     *       OK
     */
    { AT_CMD_SMSANTIATTACK,
      AT_SetSmsAntiAttackState, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SMSANTIATTACK", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ�Ƿ�֧�ַ���̽���Ź���
     * [˵��]: ���������ڲ�ѯmodem��ǰ�Ƿ�֧�ַ���̽���Ź�����
     * [�﷨]:
     *     [����]: ^SMSANTIATTACKCAP=<smstype>
     *     [���]: <CR><LF>^SMSANTIATTACKCAP: <result>,<cause><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <sms type>: ����ֵ���������ͣ�
     *             0����֤����ţ�
     *             1����Ĭ���ţ���Ϊ������Ĭ������չ�������ݣ�
     *     <result>: ����ֵ���Ƿ�֧�ַ���̽������
     *             0��֧�֣�
     *             1����֧�֣�
     *     <caule>: ����ֵ����֧��ԭ��
     *             0����ԭ��result=0ʱЯ����
     *             1���ѿ�������̽�������ܣ�
     *             2����̽��������NV�رգ�
     *             3����������NV9054����Ϊ��֧�֣�
     *             4����ǰפ���ڷ�NVָ�������磻
     *             5����ǰ����CS��IMS���У�
     *             6����ǰSENSOR״̬������NV���ã�
     *             7����ǰ��������֧�־�Ĭ����
     * [ʾ��]:
     *       AT^SMSANTIATTACKCAP=0
     *       ^SMSANTIATTACKCAP: 0,0
     *       OK
     *       ��֧�ֵ������
     *       ^SMSANTIATTACKCAP: 1,2
     *       OK
     */
    { AT_CMD_SMSANTIATTACKCAP,
      AT_SetSmsAntiAttackCapQrypara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^SMSANTIATTACKCAP", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_SCID,
      At_SetScidPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SCID", VOS_NULL_PTR },

    { AT_CMD_CACDC,
      AT_SetCacdcPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+CACDC", (VOS_UINT8 *)"(@osid),(@osappid),(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���ö�̬OOS��������
     * [˵��]: ��AT�������������ض������µ�OOS�������ԡ�
     * [�﷨]:
     *     [����]: ^OOSSRCHSTGY=<enable>,<data>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <enable>: ����ֵ��OOS������̬��������״̬��ȡֵ��Χ��0- 1
     *             1������OOS�������ԣ�
     *             0����������������ԡ�
     *     <data>: ʮ�������ַ�����������Ҫ���õ��������ԣ�enable����1ʱ��Ч��
     *     <RecordNum>: ʮ����������ֵ����Ҫ���õ���������������
     *             ռ���ֽڴ�С��һ�ֽ�
     *             ȡֵ��Χ��1~10
     *     <Record[]>: �������ͣ�ÿ����¼Ϊһ�����Գ����ľ����������ԣ�
     *             ÿ�����Լ�¼ռ��15�ֽڣ�
     *     <scene>: ʮ����������ֵ����Ҫ��̬���õ��������Եĳ�����
     *             0��΢������
     *             1������ҵ�񿨳���
     *             2��������ҵ�񿨳���
     *             3������OOS��λ1����
     *             4������OOS��λ2����
     *             5������OOS��λ3����
     *             6������OOS��λ4����
     *             7������OOS��λ5����
     *             ռ���ֽڴ�С��һ�ֽ�
     *     <TotalTimerLen1>: ʮ����������ֵ��һ�׶�������ʱ������λ����
     *             ռ���ֽڴ�С�����ֽ�
     *     <SleepTimerLen1>: ʮ����������ֵ��һ�׶����������ʱ��ʱ������λ����
     *             ռ���ֽڴ�С�����ֽ�
     *     <HistoryNum1>: ʮ����������ֵ��һ�׶εڼ���history֮�����pref band�ѻ�full band��
     *             ռ���ֽڴ�С��һ�ֽ�
     *     <PrefbandNum1>: ʮ����������ֵ��һ�׶εڼ���pref band֮�����full band�ѡ�
     *             ռ���ֽڴ�С��һ�ֽ�
     *     <FullbandNum1>: ʮ����������ֵ��һ�׶εڼ���full band�Ѻ�˽׶ν�����������һ�׶�
     *             ռ���ֽڴ�С��һ�ֽ�
     *     <TotalTimerLen2>: ʮ����������ֵ�����׶�������ʱ������λ����
     *             ռ���ֽڴ�С�����ֽ�
     *     <SleepTimerLen2>: ʮ����������ֵ�����׶����������ʱ��ʱ������λ����
     *             ռ���ֽڴ�С�����ֽ�
     *     <HistoryNum2>: ʮ����������ֵ�����׶εڼ���history֮�����pref band�ѻ�full band��
     *             ռ���ֽڴ�С��һ�ֽ�
     *     <PrefbandNum2>: ʮ����������ֵ�����׶εڼ���pref band֮�����full band�ѡ�
     *             ռ���ֽڴ�С��һ�ֽ�
     *     <FullbandNum2>: ʮ����������ֵ�����׶εڼ���full band�Ѻ�˽׶ν�����������һ�׶�
     *             ռ���ֽڴ�С��һ�ֽ�
     * [ʾ��]:
     *     �� ����OOS�������Գɹ�
     *       AT^OOSSRCHSTGY=1,"01010064000A02010001F4000F030302"
     *       OK
     *     �� ���OOS�������Գɹ�
     *       AT^OOSSRCHSTGY=0
     *       OK
     */
    { AT_CMD_OOSSRCHSTGY,
      AT_SetOosSrchStgyPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^OOSSRCHSTGY", (VOS_UINT8 *)"(0,1),(@data)" },
#if (FEATURE_MBB_CUST == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ�������������Ե�λ
     * [˵��]: �������ڲ�ѯ������OOS����������Ӧ��5�����Ƶ�λ������OOS��λ1������OOS��λ2������OOS��λ3��
               ����OOS��λ4������OOS��λ5��5����λ�Ĳ������òο�AT����OOSSRCHSTRTG����AT����Ϊ�첽�����MBB CUST����ơ�
               ע�⣺����������������Ӧ��5�����Ƶ�λǰ��������ͨ��OOSSRCHSTRTG�����·��õ�λ���������ԡ�
               TEST���֧�֡�
     * [�﷨]:
     *     [����]: ^CAROOSGEARS=<gears>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CAROOSGEARS?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^CAROOSGEARS:<gears><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <gears>: ʮ�������ַ�����������Ҫ�����ĵ�λ����ȡֵ��1��2��3��4��5�� ����ǰ5����λ��
     *             ע�⣺��ѯǰ���δ���õ�λ����0��
     * [ʾ��]:
     *     �� δ���õ�λ����ѯ������λֵ����0
     *       AT^CAROOSGEARS?
     *       CAROOSGEARS: 0
     *       OK
     *     �� ���õ�λ1����������Ч
     *       AT^CAROOSGEARS=1
     *       OK
     *     �� �ٴβ�ѯ������λֵ:
     *       AT^CAROOSGEARS?
     *       CAROOSGEARS: 1
     *       OK
     */
    { AT_CMD_CAROOSGEARS,
      AT_SetCarOosGearsPara, AT_SET_PARA_TIME, AT_QryCarOosGearsPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CAROOSGEARS", (VOS_UINT8 *)"(1-5)" },
#endif
    /* SVLTE��ĿAP��Ҫ���޿�ʱ������CAATT���� */
    { AT_CMD_CAATT,
      At_SetAutoAttach, AT_SET_PARA_TIME, At_QryAutoAttach, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE_E5 | CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CAATT", (VOS_UINT8 *)"(0,1)" },

#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: IMSҵ����������
     * [˵��]: ͨ�����������Զ�̬�򿪺͹ر�IMS���ܶ�LTE��Utran��Gsm���뼼����֧��������
     *         ע���򿪺͹ر�NR��IMS��������^NRIMSSWITCH���
     * [�﷨]:
     *     [����]: ^IMSSWITCH=<lte_enable>[,[<utran_enable>],[<gsm_enable>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             ��ǰ����IMSҵ��ʱ�����ʧ�ܣ�
     *             ��ǰ������������ѡģʽΪPS_ONLY���·��ر�LTE���ϵ�IMS����ʱ����ʧ�ܣ�
     *             �л�IMSЭ��ջ��LTE��Utran��Gsm���뼼��֧������ʧ�ܡ�
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^IMSSWITCH?
     *     [���]: <CR><LF>^IMSSWITCH:  <lte_enable>,<utran_enable>,<gsm_enable><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^IMSSWITCH=?
     *     [���]: <CR><LF>^IMSSWITCH: (list of supported <lte_enable>s),(list of supported <utran_enable>s), (list of supported <gsm_enable>s)<CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <lte_enable>: ����ֵ���򿪻�ر�LTE�����µ�IMS��������
     *             0���رգ�
     *             1���򿪡�
     *     <utran_enable>: ����ֵ���򿪻�ر�Utran�����µ�IMS��������Ĭ��ֵΪ0��
     *             0���رգ�
     *             1���򿪡�
     *     <gsm_enable>: ����ֵ���򿪻�ر�Gsm�����µ�IMS��������Ĭ��ֵΪ0��
     *             0���رգ�
     *             1���򿪡�
     * [ʾ��]:
     *     �� ��LTE���뼼����IMS�������ر�Utran��Gsm���뼼����IMS����
     *       AT^IMSSWITCH=1,0,0
     *       OK
     */
    { AT_CMD_IMSSWITCH,
      AT_SetImsSwitchPara, AT_SET_PARA_TIME, AT_QryImsSwitchPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSSWITCH", (VOS_UINT8 *)"(0,1),(0,1),(0,1)" },

    { AT_CMD_IMSSWITCH,
      AT_SetImsSwitchPara, AT_SET_PARA_TIME, AT_QryImsSwitchPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^LTEIMSSWITCH", (VOS_UINT8 *)"(0,1),(0,1),(0,1)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR IMSҵ����������
     * [˵��]: ͨ������������ͨ������NR IMS���������úͻָ�NR��ʽ��������
     *         ע��1���ر�NR�����Ĳ���������NR������SA���²��ҵ�ǰ��NR�ϵ�IMSҵ���ʱ�����ִ�У�
     *         2����NR�����Ĳ�����ҪMODEM���ڿ���״̬��
     *         3��������ʹ�ú󲻻����IMSA��ص�NV�������ã���MODEM���µ���߷��п�����ͻָ���ԭʼNV������
     * [�﷨]:
     *     [����]: ^NRIMSSWITCH=<nr_enable>
     *     [���]: <CR><LF>OK<CR><LF>
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^NRIMSSWITCH?
     *     [���]: <CR><LF>^NRIMSSWITCH: <nr_enable><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^NRIMSSWITCH=?
     *     [���]: <CR><LF>^NRIMSSWITCH: (list of supported <nr_enable>s) <CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <nr_enable>: ����ֵ���򿪻�ر�NR�����µ�IMS��������
     *             0���رգ�
     *             1���򿪡�
     * [ʾ��]:
     *     �� ��NR���뼼����IMS����
     *       AT^NRIMSSWITCH=1
     *       OK
     *     �� �ر�NR���뼼����IMS����
     *       AT^NRIMSSWITCH=0
     *       OK
     */
    { AT_CMD_NRIMSSWITCH,
      AT_SetNrImsSwitchPara, AT_SET_PARA_TIME, AT_QryNrImsSwitchPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NRIMSSWITCH", (VOS_UINT8 *)"(0,1)" },
#endif

#endif

    { AT_CMD_APPWRONREG,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryAppWronReg, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^APPWRONREG", VOS_NULL_PTR },


    { AT_CMD_APHPLMN,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryApHplmn, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^APHPLMN", VOS_NULL_PTR },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    { AT_CMD_USIM_STUB,
      At_SetUsimStub, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^USIMSTUB", VOS_NULL_PTR },

    { AT_CMD_REFRESH_STUB,
      At_SetRefreshStub, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^REFRESHSTUB", VOS_NULL_PTR },

    { AT_CMD_AUTO_RESEL_STUB,
      At_SetAutoReselStub, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^AUTORESELSTUB", (TAF_UINT8 *)"(0,1)" },

    { AT_CMD_CMM,
      At_SetCmmPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CMM", (VOS_UINT8 *)"(0-12),(0-255)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �����Ƿ�����LTE����
     * [˵��]: �������������úͲ�ѯ�Ƿ�����LTE���Ρ�����ʱ���޸�LTE���ζ���NV��ҽ��޸ĸ�NV����������α�־λ����Ӱ���NV���������Ա��ֵ������һ�ο�����ʱ�������Ч��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^CLTEROAMALLOW=<roam_allow>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������<CR><LF> +CME ERROR: <err> <CR><LF>
     *     [����]: ^CLTEROAMALLOW?
     *     [���]: <CR><LF>^CLTEROAMALLOW :<roam_allow><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CLTEROAMALLOW=?
     *     [���]: <CR><LF>^CLTEROAMALLOW: (list of supported <roam_allow>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <roam_allow>: �Ƿ�����LTE���Ρ�
     *             0��������LTE����(��ʱ�ܷ�פ��LTE������LTE���ζ���NV���еĹ������б�)��
     *             1������LTE���Ρ�
     * [ʾ��]:
     *     �� ������������
     *       AT^CLTEROAMALLOW=1
     *       OK
     *     �� ��ѯ����
     *       AT^CLTEROAMALLOW?
     *       ^CLTEROAMALLOW: 1
     *       OK
     *     �� ��������
     *       AT^CLTEROAMALLOW=?
     *       ^CLTEROAMALLOW: (0,1)
     *       OK
     */
    { AT_CMD_CLTEROAMALLOW,
      AT_SetCLteRoamAllowPara, AT_SET_PARA_TIME, AT_QryCLteRoamAllowPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CLTEROAMALLOW", (VOS_UINT8 *)"(0,1)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����С������Ƶ���ϱ�����
     * [˵��]: �������������õ�LTE/WCDMA/HRPDƵ�ʱ仯ʱ�����ϱ�����С������Ƶ����Ϣ��ʹ�ܡ�
     *         ��ǰϵͳ����L/W/HRPDʱ����ѯ�����^ULFREQRPT: 0,0,0,0��
     * [�﷨]:
     *     [����]: ^ULFREQRPT=<mode>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^ULFREQRPT?
     *     [���]: <CR><LF>^ulfreqrpt: <mode>,<rat>,<ulfreq>,<ulbw> <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ���Ƿ�ʹ�������ϱ���
     *             0���رգ�
     *             1��ʹ�ܣ�
     *             ע������Ĭ��Ϊ0��
     *     <rat>: ����ֵ�����뼼����
     *             0���޷���
     *             1��WCDMA
     *             2��HRPD
     *             3��LTE
     *     <ulfreq>: ����ֵ������С������Ƶ�ʡ���λ100kHz��
     *     <ulbw>: ����ֵ������С�����д�����λ1kHz��
     * [ʾ��]:
     *     �� ���÷���С��Ƶ���ϱ�ʹ��
     *       AT^ULFREQRPT=1
     *       OK
     *       ^ULFREQRPT: 1,19200,5000
     *     �� ִ�в�ѯ����
     *       AT^ULFREQRPT?
     *       ^ULFREQRPT: 1,1,19200,5000
     *       OK
     */
    { AT_CMD_ULFREQ,
      AT_SetUlFreqRptPara, AT_SET_PARA_TIME, AT_QryUlFreqPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ULFREQRPT", (VOS_UINT8 *)"(0-1)" },
};

/* ע��MM����AT����� */
VOS_UINT32 AT_RegisterCustomMmCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomMmCmdTbl, sizeof(g_atCustomMmCmdTbl) / sizeof(g_atCustomMmCmdTbl[0]));
}

