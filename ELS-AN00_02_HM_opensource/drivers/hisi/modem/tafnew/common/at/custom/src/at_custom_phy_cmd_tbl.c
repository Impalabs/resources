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
#include "at_custom_phy_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"
#include "at_lte_common.h"

#include "at_custom_phy_set_cmd_proc.h"
#include "at_custom_phy_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_PHY_CMD_TBL_C

static const AT_ParCmdElement g_atCustomPhyCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���������ͨ������
     * [˵��]: ����������֧�����������ͨ�����
     * [�﷨]:
     *     [����]: ^PHYCOMCFG=<cmd_type>,
     *             <rat>,
     *             <para1>,
     *             [<para2>,
     *             [<para3>]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <cmd_type>: ����ֵʮ���� ��Χ��0-65535��
     *             ��������
     *     <rat>: ����ֵʮ���� ��Χ��0-65535��
     *             ����ģʽ����bitΪ��ʾ�������Ҫ���ö��ģʽ���򽫶�Ӧ��bitλ������
     *             1��00000001����GSMʵ������ʮ������ 1
     *             2��00000010����WCDMAʵ������ʮ������ 2
     *             4��00000100����LTEʵ������ʮ������ 4
     *             8��00001000����TDSʵ������ʮ������ 8
     *             16384��100000000000000����NRʵ������ʮ������16384
     *             64��01000000����LTE��NR Band��չ��band65-band128��ʵ������ʮ������64
     *             128��10000000����LTE��NR Band��չ��band129-band192��ʵ������ʮ������128
     *             256(100000000) ��NR Band��չ��band193-band256��ʵ������ʮ������256
     *             512��1000000000����NR Band��չ��band257-band320��ʵ������ʮ������512
     *             ���Ҫ����������ʽ��������Ϊ255��11111111��
     *     <para1>: ����ֵʮ���� ��Χ��0-4294967295��
     *             ����cmd_typeȡֵ��ͬ��ʾ��ͬ����
     *     <para2>: ����ֵʮ���� ��Χ��0-4294967295��
     *             ����cmd_typeȡֵ��ͬ��ʾ��ͬ����
     *             ��������ã��Զ���Ϊ0
     *     <para3>: ����ֵʮ���� ��Χ��0-4294967295��
     *             ����cmd_typeȡֵ��ͬ��ʾ��ͬ����
     *             ��������ã��Զ���Ϊ0
     * [��]: �����ȡֵ˵��
     *       <cmd_type>,                  �����趨ֵ,
     *       0: ����Ϊ����Ϊ������,       para1: 0��ȥ������1��TAS���棬2��TASֱͨ.
     *                                    para2��  bit0-bit31��Ӧ3GPP band1 �C band32
     *                                    para3: bit0-bit31��Ӧ3GPP band33 �C band64
     *                                    para2, para3ȫΪ0��ʾ����band
     *                                    RAT LTE Band��չ���Ϻ�para2, para3���ʾband65 �C band192
     *                                    ������������ÿ��ģ��TAS�����ֱͨ��Band��Ϣ�����һ������Ϊ׼��
     *                                    Dsds2.0ֻ��ҵ��ͨ����ִ������,
     *       1������HiTune֧�ֵĵ�г����, <rat>��0���ɣ�<para1>Ϊ0��ʾ��֧��HiTune��г��1��ʾ֧�ֿ�����2��ʾ֧�������ջ���3��ʾ֧�ֿ��ٱջ�(ֻ���ն��ض����Գ���ʹ��)�������������漰,
     *       2��HALLֵѭ������,           <rat>��0���ɣ������������漰,
     *       3���±���PA���书������,     <rat>����Ϊ255���ɣ�<para1>��bit0-bit7 ΪGUCLT�Ĺ��ʻ��˵�λ��Ϣ��bit8-bit15 ΪNR�Ĺ��ʻ��˵�λ��Ϣ��bit16-bit31Ϊ�����ֶΣ������������漰,
     * [ʾ��]:
     *     �� ��Wģ����BANDִ������������
     *       AT^PHYCOMCFG=0,2,1,0,0
     *       OK
     *     �� ��Wģ����BANDִ��������ֱͨ
     *       AT^PHYCOMCFG=0,2,2,0,0
     *       OK
     *     �� ��Wģ����BANDִ�����߽���
     *       AT^PHYCOMCFG=0,2,0,0,0
     *       OK
     *     �� ����HiTune֧�ֿ���
     *       AT^PHYCOMCFG=1,2,1,0,0
     *       OK
     *     �� ����HALLֵѭ������
     *       AT^PHYCOMCFG=2,1,0,0,0
     *       OK
     *     �� �����±����ʻ��˵�λ
     *       AT^PHYCOMCFG=3,255,7,0,0
     *       OK
     */
    { AT_CMD_PHYCOMCFG,
      AT_SetPhyComCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      /* cmd_type, ratbitmap, para1, para2, para3 */
      (VOS_UINT8 *)"^PHYCOMCFG", (VOS_UINT8 *)"(0-65535),(0-65535),(0-4294967295),(0-4294967295),(0-4294967295)" },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���������ͨ������
     * [˵��]: �����������NRģʽ�£����ڸ�NRPHY���в������á�
     * [�﷨]:
     *     [����]: ^NRPHYCOMCFG=<cmd_type>,<para1>
     *             [,<para2>,<para3>,
     *             <para4>,<para5>,
     *             <para6>,<para7>,
     *             <para8>,<para9>,
     *             <para10>,<para11>,
     *             <para12>,<para13>,
     *             <para14>,<para15>]
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ������ʧ��ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <cmdtype>: ����ֵʮ���� ��Χ��0-65535����������
     *     <para1>: ����ֵʮ���� ��Χ��0-4294967295������cmd_typeȡֵ��ͬ��ʾ��ͬ����
     *     <para2>: ����ֵʮ���� ��Χ��0-4294967295������cmd_typeȡֵ��ͬ��ʾ��ͬ����
     *     <para3>: ����ֵʮ���� ��Χ��0-4294967295������cmd_typeȡֵ��ͬ��ʾ��ͬ����
     *     <��>: ��.
     *     <para15>: ����ֵʮ���� ��Χ��0-4294967295������cmd_typeȡֵ��ͬ��ʾ��ͬ����
     * [��]: �����ȡֵ˵��
     *       < cmd_type >,                �����趨ֵ,
     *       0: MicroSleep 2.0,           para1��0���˳�MicroSleep��1������MicroSleep,
     *       1��Local BWP,                para1��0����ʾ���ܲ���Ч��1����ʾ������Ч,
     *       2: ��̬������������书��, para1��0����ʾ���ܲ���Ч��1����ʾ������Ч,
     * [ʾ��]:
     *     �� ���� MicroSleep 2.0
     *       AT^NRPHYCOMCFG=0,1
     *       OK
     *     �� �˳� MicroSleep 2.0
     *       AT^NRPHYCOMCFG=0,0
     *       OK
     *     �� Local BWP������Ч
     *       AT^NRPHYCOMCFG=1,1
     *       OK
     *     �� Local BWP���ܲ���Ч
     *       AT^NRPHYCOMCFG=1,0
     *       OK
     *     �� ��̬������������书����Ч
     *       AT^NRPHYCOMCFG=2,1
     *       OK
     *     �� ��̬������������书�ʲ���Ч
     *       AT^NRPHYCOMCFG=2,0
     *       OK
     */
    { AT_CMD_NRPHYCOMCFG,
      AT_SetNrphyComCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      /* cmd_type, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15 */
      (VOS_UINT8 *)"^NRPHYCOMCFG", (VOS_UINT8 *)"(0-65535),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯAFCʱ��Ƶƫ��Ϣ
     * [˵��]: ���������ڲ�ѯModem��AFCʱ�ӵ�����״̬��Ƶƫ��Ϣ����ʱ�ӷ����ṩ��GPSоƬ����ȡ��ǰAFCʱ�ӵ�Ƶƫ��Ϣ�Ա�У׼ʱ�Ӳ����ǡ�
     *         ����ص�Ƶƫ��Ϣ����ʱ�Ӵ�������״̬ʱ����Ч��
     * [�﷨]:
     *     [����]: ^AFCCLKINFO?
     *     [���]: <CR><LF>^AFCCLKINFO: <status>[,<deviation>]�� <sTemp>,<eTemp>,<a0_m>,<a0_e>,<a1_m >, <a1_e >, <a2_m >,<a2_e >,<a3_m >,<a3_e >,<rat>��<ModemId><CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^AFCCLKINFO=?
     *     [���]: <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <status>: ����ֵ��AFCʱ������״̬��
     *             0����������
     *             1��������
     *     <deviation>: ����ֵ��ʱ��Ƶƫ��Ϣ��ȡֵ��Χ-30000~30000����λppb��
     *     <sTemp>: ����ֵ���¶ȷ�Χ������¶ȣ�ȡֵ��Χ-400~1100����λ0.1�档
     *     <eTemp>: ����ֵ���¶ȷ�Χ������¶ȣ�ȡֵ��Χ-400~1100����λ0.1�棬����¶ȸ�������¶ȡ�
     *     <a0_m>: ����ʽϵ��a0��β��(mantissa)��ȡֵ��Χ0x00000000~0xFFFFFFFF��
     *     <a0_e>: ����ʽϵ��a0��ָ��(exponent)��ȡֵ��Χ0x0000~0xFFFF��
     *     <a1_m>: ����ʽϵ��a1��β��(mantissa)��ȡֵ��Χ0x00000000~0xFFFFFFFF��
     *     <a1_e>: ����ʽϵ��a1��ָ��(exponent)��ȡֵ��Χ0x0000~0xFFFF��
     *     <a2_m>: ����ʽϵ��a2��β��(mantissa)��ȡֵ��Χ0x00000000~0xFFFFFFFF��
     *     <a2_e>: ����ʽϵ��a2��ָ��(exponent)��ȡֵ��Χ0x0000~0xFFFF��
     *     <a3_m>: ����ʽϵ��a3��β��(mantissa)��ȡֵ��Χ0x00000000~0xFFFFFFFF��
     *     <a3_e>: ����ʽϵ��a3��ָ��(exponent)��ȡֵ��Χ0x0000~0xFFFF��
     *     <rat>: �������ͣ���������ʽ��
     *             0��GSM��
     *             1��WCDMA��
     *             2��LTE��
     *             3��TDS-CDMA��
     *             4��CDMA_1X��
     *             5��HRPD��
     *             6��NR��
     *     <ModemId>: �������ͣ�������Modem Id��
     *             0��Modem 0��
     *             1��Modem 1��
     *             2��Modem 2��
     * [ʾ��]:
     *     �� ��ѯAFCʱ��Ƶƫ��Ϣ
     *       AT^AFCCLKINFO?
     *       ^AFCCLKINFO: 1,-1754,562,-88,769032704,4253851137,485040176,2896011700,49182,49169,49153,16370,1,0
     *       OK
     *     �� ִ�в�������
     *       AT^AFCCLKINFO=?
     *       ERROR
     */
    { AT_CMD_AFCCLKINFO,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryAfcClkInfo, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^AFCCLKINFO", VOS_NULL_PTR },


    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����Body SAR����
     * [˵��]: ���������ڿ����ر�Body SAR���ܣ����ɻ�ȡ��ǰBody SAR���ܵ�״̬��
     *         ��Body SAR���ش��ҵ�ǰ��ʽ�Ĺ������޲��������õ�����£�Body SAR���ܲŻ���Ч�����Խ�����ʹ��AT^BODYSARWCDMA��AT^BODYSARGSM���úø�Ƶ�ε�����书�����޲���ֵ����ʹ�ô�������������ر�Body SAR���ܡ�
     *         �������ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^BODYSARON=<on>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^BODYSARON?
     *     [���]: <CR><LF>^BODYSARON: <on><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^BODYSARON=?
     *     [���]: <CR><LF>^BODYSARON: (0,1)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <on>: ����ֵ��Body SAR���ܿ���״̬��
     *             0���ر�Body SAR���ܣ�
     *             1������Body SAR���ܡ�
     * [ʾ��]:
     *     �� ����Body SAR����
     *       AT^BODYSARON=1
     *       OK
     *     �� ��ѯBody SAR����״̬
     *       AT^BODYSARON?
     *       ^BODYSARON: 1
     *       OK
     *     �� ִ�в�������
     *       AT^BODYSARON=?
     *       ^BODYSARON: (0,1)
     *       OK
     */
    { AT_CMD_BODYSARON,
      AT_SetBodySarOnPara, AT_SET_PARA_TIME, AT_QryBodySarOnPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^BODYSARON", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �趨WCDMA����书������ֵ
     * [˵��]: �����������趨��ѯWCDMA��Ƶ�ε�����书������ֵ��
     *         ������������Ƶ��λ���ֵ����ѡ�е�Ƶ�ν����趨������ͬƵ�ε��趨ֵ��ͬʱ���ɷֶ���������в�ͬ����ֵ���趨��
     *         �������ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^BODYSARWCDMA=<power>[,<band>[,<power,<band>]��]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^BODYSARWCDMA?
     *     [���]: <CR><LF>^BODYSARWCDMA: (list of (<power>,<band>)s)
     *             <CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^BODYSARWCDMA=?
     *     [���]: <CR><LF>^BODYSARWCDMA: (17,24),<band> <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <power>: ����ֵ����ӦƵ�ε�����书������ֵ����λΪdbm��ȡֵ��Χ17~24��
     *     <band>: 16�������ִ���Ƶ��λ�򡣳���Ϊ4�ֽڣ�ÿ��Bitλ��Ӧһ��Ƶ�Σ�ȡֵΪ��13-4�и�����ֵ�����0x3FFFFFFF����ĸ���������ֵ��
     * [��]: WCDMAƵ�ζ�Ӧ����ֵ��
     *       Ƶ��,                    ����ֵ,
     *       WCDMA_I_2100,            00000001,
     *       WCDMA_II_1900,           00000002,
     *       WCDMA_III_1800,          00000004,
     *       WCDMA_IV_1700,           00000008,
     *       WCDMA_V_850,             00000010,
     *       WCDMA_VI_800,            00000020,
     *       WCDMA_VII_2600,          00000040,
     *       WCDMA_VIII_900,          00000080,
     *       WCDMA_IX_1700,           00000100,
     *       WCDMA_X���ݲ�֧�֣�,     00000200,
     *       WCDMA_XI_1500,           00000400,
     *       WCDMA_XII���ݲ�֧�֣�,   00000800,
     *       WCDMA_XIII���ݲ�֧�֣�,  00001000,
     *       WCDMA_XIV���ݲ�֧�֣�,   00002000,
     *       WCDMA_XV���ݲ�֧�֣�,    00004000,
     *       WCDMA_XVI���ݲ�֧�֣�,   00008000,
     *       WCDMA_XVII���ݲ�֧�֣�,  00010000,
     *       WCDMA_XVIII���ݲ�֧�֣�, 00020000,
     *       WCDMA_XIX_850,           00040000,
     *       ����֧�ֵ�Ƶ��,          3FFFFFFF,
     * [ʾ��]:
     *     �� ����WCDMA I������书������ֵΪ20��WCDMA II��III������书������ֵΪ18
     *       AT^BODYSARWCDMA=20,00000001,18,00000006
     *       OK
     *     �� ��ѯ��ǰ����֧�ֵ�WCDMAƵ������书������ֵΪ19
     *       AT^BODYSARWCDMA?
     *       ^BODYSARWCDMA: (19,3FFFFFFF)
     *       OK
     *     �� ��ѯ��ǰ֧�ֵ�WCDMAƵ��I������书������ֵΪ19��Ƶ��II��IIIΪ20
     *       AT^BODYSARWCDMA?
     *       ^BODYSARWCDMA: (19,00000001),(20,00000006)
     *       OK
     *     �� ִ�в��������ǰ֧��WCDMAƵ��I��II��III��IV
     *       AT^BODYSARWCDMA=?
     *       ^BODYSARWCDMA: (17,24),0000000F
     *       OK
     */
    { AT_CMD_BODYSARWCDMA,
      AT_SetBodySarWcdmaPara, AT_NOT_SET_TIME, AT_QryBodySarWcdmaPara, AT_NOT_SET_TIME, AT_TestBodySarWcdmaPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^BODYSARWCDMA", (VOS_UINT8 *)"(17-24),(@band),(17-24),(@band),(17-24),(@band),(17-24),(@band),(17-24),(@band),(17-24),(@band),(17-24),(@band),(17-24),(@band)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �趨GSM����书������ֵ
     * [˵��]: �����������趨��ѯGSM��Ƶ�ε�����书������ֵ��
     *         ������������Ƶ��λ���ֵ����ѡ�е�Ƶ�ν����趨������ͬƵ�ε��趨ֵ��ͬʱ���ɷֶ���������в�ͬ����ֵ���趨��
     *         �������ݲ�֧�֡�
     * [�﷨]:
     *     [����]: ^BODYSARGSM=<power>[,<band>[,<power,<band>]��]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^BODYSARGSM?
     *     [���]: <CR><LF>^BODYSARGSM: (list of (<power>,<band>)s)
     *             <CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^BODYSARGSM=?
     *     [���]: <CR><LF>^BODYSARGSM: (15,33),<band> <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <power>: ����ֵ����ӦƵ�ε�����书������ֵ����λΪdbm��ȡֵ��Χ15~33��
     *     <band>: 16�������ִ���Ƶ��λ�򡣳���Ϊ4�ֽڣ�ÿ��Bitλ��Ӧһ��Ƶ�Σ�ȡֵΪ��2-23�и�����ֵ�����0x3FFFFFFF����ĸ���������ֵ��
     * [��]: GSMƵ�ζ�Ӧ����ֵ��
     *       Ƶ��,           ����ֵ,
     *       GSM850(GPRS),   00000001,
     *       GSM900(GPRS),   00000002,
     *       GSM1800(GPRS),  00000004,
     *       GSM1900(GPRS),  00000008,
     *       GSM850(EDGE),   00010000,
     *       GSM900(EDGE),   00020000,
     *       GSM1800(EDGE),  00040000,
     *       GSM1900(EDGE),  00080000,
     *       ����֧�ֵ�Ƶ��, 3FFFFFFF,
     * [ʾ��]:
     *     �� ����GSM850������书������ֵΪ19�����������书������ֵ��Ϊ20
     *       AT^BODYSARGSM=19,00010001,20,000E000E
     *       OK
     *     �� ��ѯ��ǰ����֧�ֵ�GSMƵ������书������ֵ��Ϊ20
     *       AT^BODYSARGSM?
     *       ^BODYSARGSM: (20,3FFFFFFF)
     *       OK
     *     �� ��ѯ��ǰ֧�ֵ�GSMƵ��GSM850������书������ֵΪ18��Ƶ��GSM900Ϊ20������Ϊ19
     *       AT^BODYSARGSM?
     *       ^BODYSARGSM: (18,00010001),(20,00020002),(19,000A000A)
     *       OK
     *     �� ִ�в��������ǰ֧��GSM850��GSM900��GSM1800��GSM1900Ƶ��
     *       AT^BODYSARGSM=?
     *       ^BODYSARGSM: (15,33),000F000F
     *       OK
     */
    { AT_CMD_BODYSARGSM,
      AT_SetBodySarGsmPara, AT_NOT_SET_TIME, AT_QryBodySarGsmPara, AT_NOT_SET_TIME, AT_TestBodySarGsmPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^BODYSARGSM", (VOS_UINT8 *)"(15-33),(@band),(15-33),(@band),(15-33),(@band),(15-33),(@band),(15-33),(@band),(15-33),(@band),(15-33),(@band),(15-33),(@band)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����GPS�ο�ʱ��״̬�����ϱ�
     * [˵��]: ����������AP-Modem��̬�¿���GPS�ο�ʱ��״̬�������ϱ�����ѯ���Ա��ⲻȷ���Ĳο�ʱ��״̬��Ӱ�졣
     *         ����ʹ�ó�����GPSоƬ�·���
     *         ����ʹ�����ƣ�ֻ�޶���AP-Modem��̬ʹ�á�
     *         ����ʹ��ͨ����ֻ��ͨ����AP�Խӵ�ATͨ���·���
     * [�﷨]:
     *     [����]: ^REFCLKFREQ=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^REFCLKFREQ?
     *     [���]: ��ѯ�ɹ���
     *             <CR><LF>^REFCLKFREQ: <ver>,<freq>,<precision> <status><CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^REFCLKFREQ=?
     *     [���]: <CR><LF>^REFCLKFREQ: (list of supported <status>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ��GPS�ο�ʱ��״̬�ϱ����ƣ�
     *             0���������ϱ���
     *             1�������ϱ���
     *     <ver>: ����ֵ��������İ汾ID����ǰΪ0��
     *     <freq>: ����ֵ��GPS�ο�ʱ�ӵ�Ƶ��ֵ����λHz��
     *     <precision>: ����ֵ����ǰGPS�ο�ʱ�ӵľ��ȣ���λppb��
     *     <status>: ����ֵ��GPS�ο�ʱ��Ƶ������״̬��
     *             0��δ������
     *             1��������
     * [ʾ��]:
     *     �� ����GPS�ο�ʱ��״̬�����ϱ�
     *       AT^REFCLKFREQ=1
     *       OK
     *     �� ��ѯGPS�ο�ʱ��״̬
     *       AT^REFCLKFREQ?
     *       ^REFCLKFREQ: 0,19200000,100,1
     *       OK
     *     �� ִ�в�������
     *       AT^REFCLKFREQ=?
     *       ^REFCLKFREQ: (0,1)
     *       OK
     */
    { AT_CMD_REFCLKFREQ,
      AT_SetRefclkfreqPara, AT_SET_PARA_TIME, AT_QryRefclkfreqPara, AT_QRY_PARA_TIME, AT_TestRefclkfreqPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^REFCLKFREQ", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����MODEM�л����ּ�����
     * [˵��]: AP-Modem��̬�£�AP����sensor�㷨�б������֣�֪ͨmodem�����л��ּ����ߡ�
     * [�﷨]:
     *     [����]: ^HANDLEDECT=<handletype>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^HANDLEDECT?
     *     [���]: <CR><LF>^HANDLEDECT: <handletype><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^HANDLEDECT=?
     *     [���]: <CR><LF>^HANDLEDECT: (0-4)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <handletype>: ����ֵ���ֳ�phone��pad��λ�����͡�
     *             0��no_handle��
     *             1�����ֽ��գ�
     *             2�����ֽ��գ�
     *             3�������Ͻ��գ�
     *             4�������½��ա�
     * [ʾ��]:
     *     �� ����MODEM�������ֽ����������ּ�����
     *       AT^HANDLEDECT=1
     *       OK
     */
    { AT_CMD_HANDLEDECT,
      At_SetHandleDect, AT_SET_PARA_TIME, At_QryHandleDect, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HANDLEDECT", (VOS_UINT8 *)"(0-4)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ȫ��ͨ��Ʒ����RF ProfileId
     * [˵��]: ֻ����ȫ��ͨ����ʹ�ܵ�����²�������RF ProfileId��
     * [�﷨]:
     *     [����]: ^RATRFSWITCH=<RFProfileId>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^RATRFSWITCH?
     *     [���]: <CR><LF>^RATRFSWITCH: <enable>,<RFProfileId><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR <err><CR><LF>
     *     [����]: ^RATRFSWITCH=?
     *     [���]: <CR><LF>^RATRFSWITCH: (list of supported <RFProfileId>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <RFProfileId>: RFʹ�õ�RFProfileId������ֵ��ȡֵ��ΧΪ0~7����Ӧ��Profile Id�����浽NV NV_TRI_MODE_FEM_PROFILE_ID��
     *             0��ProfileId 0;
     *             1��ProfileId 1;
     *             2��ProfileId 2;
     *     <enable>: ����ֵ��ȫ��ͨ�����Ƿ�ʹ��
     *             0��ȫ��ͨ����û��ʹ�ܣ�
     *             1��ȫ��ͨ����ʹ�ܡ�
     * [ʾ��]:
     *     �� ��ѯRF ProfileId
     *       AT^RATRFSWITCH?
     *       RATRFSWITCH: 0,0
     *       OK
     *     �� ����RF ProfileIdΪ1
     *       AT^RATRFSWITCH=1
     *       OK
     */
    { AT_CMD_RATRFSWITCH,
      At_SetRatRfSwitch, AT_NOT_SET_TIME, At_QryRatRfSwitch, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RATRFSWITCH", (VOS_UINT8 *)"(0-7)" },

    { AT_CMD_MCS,
      AT_SetMcsPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^MCS", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: GUL���书�ʲ�ѯ
     * [˵��]: ͨ����AT��ȡ��ǰmodem���书�ʣ�����LTE��˵�Ǹ��ŵ���PRACH/PUCCH/PUSCH/SRS���ķ��书�ʡ�����GU������stxpwr��Ч��
     *         ���������GUL����Ч����ѯ��ǰ��ģ�ķ��书�ʣ�ENDC������ѯ����LTE�ķ��书�ʡ�
     * [�﷨]:
     *     [����]: ^TXPOWER?
     *     [���]: <CR><LF>^TXPOWER: <stxpwr>,<PPusch>,<PPucch>,<PSrs>,<PPrach><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <stxpwr>: ����ֵ����λ�� 0.1dBm;�����2G����ʱ�� ȡֵ��ΧΪ0��33dBm��ʵ���ϱ�ֵΪ0��330�������Ϊ3G����ʱ��ȡֵ��ΧΪ-50��24dBm��ʵ���ϱ�ֵΪ-510��240��,4Gʱ���ò���ֵΪ999��
     *     <PPusch>: ����ֵ��<=23dbm����PUSCH���书�ʣ���Ϊ2G����3Gʱ����ֵΪ999��
     *     <PPucch>: ����ֵ��<=23dbm����PUCCH���书�ʣ���Ϊ2G����3Gʱ����ֵΪ999��
     *     <PSrs>: ����ֵ��<=23dbm����SRS���书�ʣ���Ϊ2G����3Gʱ����ֵΪ999��
     *     <PPrach>: ����ֵ��<=23dbm����PRACH���书�ʣ���Ϊ2G����3Gʱ����ֵΪ999��
     * [ʾ��]:
     *     �� ������ѯLTE�ķ��书��
     *       AT^TXPOWER?
     *       ^TXPOWER: 999,23,23,22,23
     *       OK
     *     �� ������ѯG/W�ķ��书��
     *       AT^TXPOWER?
     *       ^TXPOWER: 240,999,999,999,999
     *       OK
     */
    { AT_CMD_TXPOWER,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryTxPowerPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^TXPOWER", VOS_NULL_PTR },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR���书�ʲ�ѯ
     * [˵��]: ͨ����AT��ȡ��ǰmodem���书�ʣ�����NR��˵�Ǹ��ŵ���PRACH/PUCCH/PUSCH/SRS���ķ��书�ʡ�֧�ֶ�CC�ķ��书���ϱ���
     *         ���������NR/L����Ч����ѯNR�ķ��书�ʣ�ENDC������ѯ����NR�ķ��书�ʡ�
     * [�﷨]:
     *     [����]: ^NTXPOWER?
     *     [���]: ִ�гɹ���
     *             <CR><LF>^NTXPOWER: <PPusch1>,<PPucch1>,<PSrs1>,<PPrach1>,<Freq1>,[<PPusch2>,<PPucch2>,<PSrs1>,<PPrach1>,<Freq2>,[<PPusch3>,<PPucch3>,<PSrs3>,<PPrach3>,<Freq3>,[<PPusch4>,<PPucch4>,<PSrs4>,<PPrach4>,<Freq4>]]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <PPuschN>: ����ֵ��<=26dbm����PUSCH���书�ʣ���Чֵ999��NΪ�ز���Ŀ�����Ϊ4��
     *     <PPucchN>: ����ֵ��<=26dbm����PUCCH���书�ʣ���Чֵ999��NΪ�ز���Ŀ�����Ϊ4��
     *     <PSrsN>: ����ֵ��<=26dbm����SRS���书�ʣ���Чֵ999��NΪ�ز���Ŀ�����Ϊ4��
     *     <PPrachN>: ����ֵ��<=26dbm����PRACH���书�ʣ���Чֵ999��NΪ�ز���Ŀ�����Ϊ4��
     *     <FreqN>: ���ͱ�������ʾ��ǰ�ն�����С����Ƶ�ʣ���λΪ(100kHz)����Чֵȡ0��NΪ�ز���Ŀ�����Ϊ4��
     * [ʾ��]:
     *     �� ������ѯNR�ķ��书�� 2CC
     *       AT^NTXPOWER?
     *       ^NTXPOWER: 23,3,23,22,8675,23,2,22,23,8677
     *       OK
     *     �� ������ѯNR�ķ��书�ʵ�С��
     *       AT^NTXPOWER?
     *       ^NTXPOWER: 23,3,23,22,8675
     *       OK
     */
    { AT_CMD_NTXPOWER,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryNtxPowerPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NTXPOWER", VOS_NULL_PTR },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����/��ѯLTE����������
     * [˵��]: ���ӿ�����֧�ֶԺ�˼�����ʵ�ֵĶ�̬��/��CC�Ͷ�̬����RI�Ľ����Ĺ��ܣ����ϲ�Ӧ�û���ʵ��ʹ�ó���ʹ�á�
     *         ��ҪbitCloseCaAndMimoCtrl����֧�֡�
     *         ע��^LTEPWRCTRL=1,1�·��󣬵�ǰֻ֧�ֽ�RIΪ0����1��
     * [�﷨]:
     *     [����]: ^LTEPWRCTRL=<Mode>,<Para>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LTEPWRCTRL=?
     *     [���]: <CR><LF>^LTEPWRCTRL: (list of supported <Mode>s),(list of supported <Para>s)<CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <Mode>: ����ֵ��ʮ�������֣�ȡֵ0~2��
     *             0������CC����
     *             1������RIֵ��
     *             2������BSR���ݲ�֧�֣�
     *     <Para>: ����ֵ��ʮ�������֣�ȡֵ-8~63��
     *             ��Mode=0ʱ����Χ��-8~8������ʾҪ������ֵ������������ֵ����CC����
     *             ��Mode=1ʱ����Χ��0~1����1��ʾʹ�ܽ�RI���ܣ�0��ʾ�رս�RI���ܣ�
     *             ��Mode=2ʱ����ΧΪ0~63����ʾ֧�ֵ�BSR�����ֵ��Ŀǰ�ݲ�֧�֣�
     * [ʾ��]:
     *     �� SET����
     *       AT^LTEPWRCTRL=1,1
     *       OK
     */
    { AT_CMD_LTEPWRCTRL,
      AT_SetLtePwrDissPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestLtePwrDissPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LTEPWRCTRL", (VOS_UINT8 *)"(0-2),(@para)" },

#if (FEATURE_LTE == FEATURE_ON)
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR��RI����
     * [˵��]: Ŀǰʵ��NR��RI���ܡ�
     * [�﷨]:
     *     [����]: ^NRPWRCTRL=<Mode>,<Para>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NRPWRCTRL=?
     *     [���]: <CR><LF>^NRPWRCTRL: (list of supported <Mode>s),(list of supported <Para>s)<CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <Mode>: ����ֵ��ʮ�������֣�ȡֵ0~2����Ŀǰֻʵ��mode=1�Ĺ��ܣ�
     *             1������RIֵ��
     *     <Para>: ����ֵ��ʮ�������֡�
     *             ��Mode=1ʱ��ȡֵ��ΧΪParaΪ0,1
     *             1��ʾʹ�ܽ�RI����
     *             0��ʾ�رս�RI����
     * [ʾ��]:
     *     �� ʹ�ܽ�RI����
     *       AT^NRPWRCTRL=1,1
     *       OK
     */
    { AT_CMD_NRPWRCTRL,
      AT_SetNrPwrCtrlPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestNrPwrCtrlPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NRPWRCTRL", (VOS_UINT8 *)"(0-2),(@para)" },
#endif
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    { AT_CMD_PDMCTRL,
      AT_SetPdmCtrlPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^PDMCTRL", (TAF_UINT8 *)"(0-255),(0-65535),(0-65535),(0-65535)" }, /* value */
#endif

    { AT_CMD_TFDPDTQRY,
      At_SetQryTfDpdtPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^TFDPDTQRY", (VOS_UINT8 *)DPDT_QRY_PARA_STRING },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ѯLTE����Ƶ����Ϣ
     * [˵��]: �������ѯ��ǰLTE�����Ƶ����Ϣ���ϲ�Ӧ��ͨ����ѯ��ǰLTE��Ƶ����Ϣ�����ڹ����WIFIģ��ͬƵ���š�LTE��WIFI��Ƶ�ʸ��ŷ�ΧĬ����(23700-24000)����ͨ��NV�̬������
     * [�﷨]:
     *     [����]: ^LWCLASH?
     *     [���]: <CR><LF>^LWCLASH: <state>,<ulfreq>,<ulbw>,<dlfreq>,<dlbw>,<band>,<ant_num>,<ant1_ulfreq>,<ant1_ulbw>,<ant1_dlfreq>,<ant1_dlbw>,<ant2_ulfreq>,<ant2_ulbw>,<ant2_dlfreq>,<ant2_dlbw>,<ant3_ulfreq>,<ant3_ulbw>,<ant3_dlfreq>,<ant3_dlbw>,<ant4_ulfreq>,<ant4_ulbw>,<ant4_dlfreq>,<ant4_dlbw>,<ant_dlmm><ant1_dlmm><ant2_dlmm><ant3_dlmm><ant4_dlmm>,<dl256QAMflag><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <state>: ��ǰLTEƵ����WIFIƵ�ʳ�ͻ״̬��
     *             0������ͻ����LTE��WIFI������Ƶ�ʸ��ŷ�Χ��
     *             1����ͻ����LTE��WIFI����Ƶ�ʸ��ŷ�Χ��
     *             2����Чֵ����ǰLTE�ǽ���״̬���޹̶�������Ƶ�ʣ���
     *     <ulfreq>: LTE����Ƶ�ʣ���λ100kHz��ȡֵ��Χ��0~4294967295��
     *     <ulbw>: LTE���д���
     *             0��1.4M��
     *             1��3M��
     *             2��5M��
     *             3��10M��
     *             4��15M��
     *             5��20M��
     *     <dlfreq>: LTE����Ƶ�ʣ���λ100kHz��ȡֵ��Χ��0~4294967295��
     *     <dlbw>: LTE���д���
     *             0��1.4M��
     *             1��3M��
     *             2��5M��
     *             3��10M��
     *             4��15M��
     *             5��20M��
     *     <band>: LTEģʽ��Ƶ�κţ�ȡֵ��Χ��1~70��
     *     <ant_num>: ���ߵ�Ƶ���Ƶ�θ�����ȡֵ��Χ0~3��
     *     <dlmm>: UE MIMO����
     *     <dl256QAMflag>: �Ƿ�֧������256QAM��
     *             0����֧�֣�
     *             1��֧�֡�
     */
    { AT_CMD_LWCLASH,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryLwclashPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^LWCLASH", VOS_NULL_PTR },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ָʾLPHY/NPHY����͹���ģʽ
     * [˵��]: ͨ����AT����ָʾLPHY��NR PHY����͹���ģʽ��
     * [�﷨]:
     *     [����]: ^LOWPWRMODE=<mode>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <mode>: ���뼼����ȡֵ0,1
     *             0��LTE
     *             1��NR
     * [ʾ��]:
     *     �� ָʾLPHY����͹���ģʽ
     *       AT^LOWPWRMODE=0
     *       OK
     */
    { AT_CMD_LOWPOWERMODE,
      At_SetLowPowerModePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LOWPWRMODE", (VOS_UINT8 *)LOWPWRMODE_CMD_PARA_STRING },
#endif
};

/* ע��PHY����AT����� */
VOS_UINT32 AT_RegisterCustomPhyCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomPhyCmdTbl, sizeof(g_atCustomPhyCmdTbl) / sizeof(g_atCustomPhyCmdTbl[0]));
}

