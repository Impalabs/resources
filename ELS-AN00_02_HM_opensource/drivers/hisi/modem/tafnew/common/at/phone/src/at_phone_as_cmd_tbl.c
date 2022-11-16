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
#include "at_phone_as_cmd_tbl.h"
#include "AtParse.h"
#include "at_parse_cmd.h"
#include "at_phone_as_set_cmd_proc.h"
#include "at_phone_as_qry_cmd_proc.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_AS_CMD_TBL_C

static const AT_ParCmdElement g_atPhoneAsCmdTbl[] = {
    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯRSCP��ECIO
     * [˵��]: ���ź�ǿ��RSCP/ECIO�Ĳ�ѯ��RSCP/ECIO�ź�ֵ�Ļ�ȡ������RSSI�źŵı仯��
     *         RSCP/ECIO/RSSI�������µĶ�Ӧ��ϵ��RSCP=ECIO+RSSI�����У�RSCP/ECIO/RSSI��������ʵ�ʵľ���ֵ�����������ֵ��
     *         ECIO��ָ��Ƶ����Ƭƽ�������ʹ����ܹ����ܶȱȡ�
     * [�﷨]:
     *     [����]: ^CSNR
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^CSNR?
     *     [���]: <CR><LF>^CSNR: <rscp>,<ecio><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CSNR=?
     *     [���]: <CR><LF>^CSNR: (list of supported <rscp>s), (list of supported <ecio>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <rscp>: ����ֵ��RSCPֵ����λΪdBm��ȡֵΪ-145��-60��
     *     <ecio>: ����ֵ��ECIOֵ����λΪdBm��ȡֵΪ-32��0��
     * [ʾ��]:
     *     �� ��ѯ��ǰ�ź�ǿ��
     *       AT^CSNR?
     *       ^CSNR: -109,-9
     *       OK
     */
    { AT_CMD_CSNR,
      At_SetCSNR, AT_NOT_SET_TIME, At_QryCSNR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CSNR", (VOS_UINT8 *)"(-145 - -60),(-32 - 0)" },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /* WASӦ�ն�Ҫ������AT���� */
    /*
     * [���]: Э��AT-�绰����
     * [����]: Wģ��Ƶ
     * [˵��]: ��ѯ������/ȡ��Wģ��Ƶ���ܡ�
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^WFREQLOCK=<enable>[,<freq>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^WFREQLOCK?
     *     [���]: <CR><LF>^FREQLOCK: <enable>[,<freq>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^WFREQLOCK=?
     *     [���]: <CR><LF>^FREQLOCK: (list of supported <enable>) (list of supported <freq>)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ��Wģ��Ƶ�����Ƿ�������
     *             0����Ƶ����û�����ã�
     *             1����������Ƶ���ܡ�
     *     <freq>: ����ֵ����ǰ������Ƶ��ֵ��ȡֵ�������Э�顢��Ʒ���ȷ����Ч��Χ����
     *             <enable>=0ʱ��<freq>���Բ��
     * [ʾ��]:
     *     �� ����Ƶ��
     *       AT^WFREQLOCK=1,10600
     *       OK
     */
    { AT_CMD_FREQLOCK,
      At_SetFreqLock, AT_SET_PARA_TIME, At_QryFreqLock, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^WFREQLOCK", (VOS_UINT8 *)"(0,1),(412-10838)" },
#endif

#if (FEATURE_PROBE_FREQLOCK == FEATURE_ON)
    /* ģ���Ʒ��GUTL��Ƶ���� */
    /*
     * [���]: Э��AT-�绰����
     * [����]: ��Ƶ
     * [˵��]: ·��ʹ�ã���ѯ�����á�ȡ����Ƶ���ܡ�
     *         ��ע��ͬһʱ��ͬһ��ʽ�½�֧��һ��Ƶ�����Ƶ������·�������Ƶ����ʱ�����ͬһ��ʽ���Ѿ���һ��Ƶ������Ƶ״̬��������һ��Ƶ��֮ǰ����Ҫ�ȹر�ǰһ��Ƶ�����Ƶ״̬��
     *         AT^ M2MFREQLOCK������FEATURE_PROBE_FREQLOCK���ʱʹ�ã����Ҳ��ܺ�������Ƶ������á�
     * [�﷨]:
     *     [����]: ^M2MFREQLOCK=<enable>[,<mode>,<freq>,[<band>],[<psc>],[<pci>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^M2MFREQLOCK?
     *     [���]: <CR><LF>^M2MFREQLOCK: <enable>,<mode>[,<freq>,[<band>],[<psc>],[<pci>]]
     *             <CR><LF>^M2MFREQLOCK: <enable>,<mode>[,<freq>,[<band>],[<psc>],[<pci>]]
     *             <CR><LF>^M2MFREQLOCK: <enable>,<mode>[,<freq>,[<band>],[<psc>],[<pci>]]
     *             <CR><LF>^M2MFREQLOCK: <enable>,<mode>[,<freq>,[<band>],[<psc>],[<pci>]]
     *             <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ����ʶ��Ƶ�����Ƿ����á�
     *             0����Ƶ����û�����ã�
     *             1����������Ƶ���ܡ�
     *     <mode>: �ַ����ͣ���Ƶ����ʽ��
     *             ��01��          GSM
     *             ��02��          WCDMA
     *             ��03��          TD-SCDMA
     *             ��04��          LTE
     *     <freq>: ����ֵ����ǰ������Ƶ��ֵ��
     *     <band>: �ַ����ͣ���Ƶ��Ƶ�Σ���������Ƶ���ظ���Ƶ�Ρ�ֻ��GSM��ʽ�����á�
     *             ��00��          850
     *             ��01��          900
     *             ��02��          1800
     *             ��03��          1900
     *     <psc>: �ַ����ͣ������������WCDMA��ʽ�¸ò�����Ч������Ϊ��ѡ�δ���øò���ʱ��ΪWCDMA��ʽ����Ƶ�㣻���øò�����ΪWCDMA��ʽ����Ƶ��+������ϡ�
     *     <pci>: ����С��ID������ֵ���������Э�顢��Ʒ���ȷ��ȡֵ��Χ����ֻ��LTE��ʽ����Ч���������ֲ�ͬLTEС�����źš�
     * [ʾ��]:
     *     �� ����39250Ƶ��
     *       AT^M2MFREQLOCK=1,"04",39250,,,
     *       OK
     *     �� ����39250Ƶ��С��333
     *       AT^M2MFREQLOCK=1,"04",39250,,,333
     *       OK
     *     �� ��ѯ��ǰ��Ƶ��������Ϣ
     *       AT^M2MFREQLOCK?
     *       ^M2MFREQLOCK: 0,"01"
     *       ^M2MFREQLOCK: 0,"02"
     *       ^M2MFREQLOCK: 0,"03"
     *       ^M2MFREQLOCK: 1,"04",39250,,,333
     *       OK
     *     �� �����ǰ��������Ϣ
     *       AT^M2MFREQLOCK=0
     *       OK
     */
    { AT_CMD_M2MFREQLOCK,
      At_SetM2MFreqLock, AT_SET_PARA_TIME, At_QryM2MFreqLock, AT_QRY_PARA_TIME, At_TestM2MFreqLock, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^M2MFREQLOCK", (VOS_UINT8 *)"(0,1),(Mode),(0-4294967295),(Band),(0-65535),(0-503)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��Ƶ
     * [˵��]: ��ѯ�����á�ȡ��GUTL��Ƶ���ܡ�
     *         ��ע��ͬһʱ��ͬһ��ʽ�½�֧��һ��Ƶ�����Ƶ�����Խ�����Ƶ����ʱ�����ͬһ��ʽ���Ѿ���һ��Ƶ������Ƶ״̬��������һ��Ƶ��֮ǰ����Ҫ�ȹر�ǰһ��Ƶ�����Ƶ״̬��
     *         AT^FREQLOCK������FEATURE_PROBE_FREQLOCK���ʱʹ�ã����Ҳ��ܺ�������Ƶ������á�
     * [�﷨]:
     *     [����]: ^FREQLOCK=<enable>[,<mode>,<freq>,[<band>],[<psc>],[<pci>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FREQLOCK?
     *     [���]: <CR><LF>^FREQLOCK: <enable>,<mode>[,<freq>,[<band>],[<psc>],[<pci>]]
     *             <CR><LF>^FREQLOCK: <enable>,<mode>[,<freq>,[<band>],[<psc>],[<pci>]]
     *             <CR><LF>^FREQLOCK: <enable>,<mode>[,<freq>,[<band>],[<psc>],[<pci>]]
     *             <CR><LF>^FREQLOCK: <enable>,<mode>[,<freq>,[<band>],[<psc>],[<pci>]]
     *             <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ����ʶ��Ƶ�����Ƿ����á�
     *             0����Ƶ����û�����ã�
     *             1����������Ƶ���ܡ�
     *     <mode>: �ַ����ͣ���Ƶ����ʽ��
     *             ��01��          GSM
     *             ��02��          WCDMA
     *             ��03��            TD-SCDMA
     *             ��04��          LTE
     *     <freq>: ����ֵ����ǰ������Ƶ��ֵ��
     *     <band>: �ַ����ͣ���Ƶ��Ƶ�Σ���������Ƶ���ظ���Ƶ�Ρ�ֻ��GSM��ʽ�����á�
     *             ��00��          850
     *             ��01��          900
     *             ��02��          1800
     *             ��03��          1900
     *     <psc>: �ַ����ͣ������������WCDMA��ʽ�¸ò�����Ч������Ϊ��ѡ�δ���øò���ʱ��ΪWCDMA��ʽ����Ƶ�㣻���øò�����ΪWCDMA��ʽ����Ƶ��+������ϡ�
     *     <pci>: ����С��ID������ֵ���������Э�顢��Ʒ���ȷ��ȡֵ��Χ����ֻ��LTE��ʽ����Ч���������ֲ�ͬLTEС�����źš�
     * [ʾ��]:
     *     �� ����39250Ƶ��
     *       AT^FREQLOCK=1,"04",39250,,,
     *       OK
     *     �� ����39250Ƶ��С��333
     *       AT^FREQLOCK=1,"04",39250,,,333
     *       OK
     *     �� ��ѯ��ǰ��Ƶ��������Ϣ
     *       AT^FREQLOCK?
     *       ^FREQLOCK: 0,"01"
     *       ^FREQLOCK: 0,"02"
     *       ^FREQLOCK: 0,"03"
     *       ^FREQLOCK: 1,"04",39250,,,333
     *       OK
     *     �� �����ǰ��������Ϣ
     *       AT^FREQLOCK=0
     *       OK
     */
    { AT_CMD_M2MFREQLOCK,
      At_SetM2MFreqLock, AT_SET_PARA_TIME, At_QryM2MFreqLock, AT_QRY_PARA_TIME, At_TestM2MFreqLock, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FREQLOCK", (VOS_UINT8 *)"(0,1),(Mode),(0-4294967295),(Band),(0-65535),(0-503)" },
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: Э��AT-�绰����
     * [����]: Gģ��Ƶ
     * [˵��]: ����/ȡ��Gģ��Ƶ���ܡ�
     *         ��������������£��·����AT������ܱ�֤��Ƶ�ɹ���
     *         ����û������
     *         ���嵱ǰ��֧��Gģ��
     *         ���岻֧�ֵ�ǰ��Ҫ������Ƶ�㣻
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^GFREQLOCK=<enable>[,<freq>,<band>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^GFREQLOCK?
     *     [���]: <CR><LF>^GFREQLOCK: <enable>[,<freq>,<band>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             EnableΪ0ʱ��freq��band���ϱ�
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^GFREQLOCK=?
     *     [���]: <CR><LF>^GFREQLOCK: (list of supported<enable>)(list of support<freq>)(list of support<band>)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ����Ƶ�����Ƿ�����
     *             0: ��������Ƶ����
     *             1: ������Ƶ����
     *     <freq>: ����ֵ����ǰ������Ƶ�㣬ȡֵ��Χ0~1023
     *             ��<enable>=0ʱ��<freq>���Բ���
     *     <band>: ����ֵ����ǰ����Ƶ�ʶ�Ӧ��bandֵ:
     *             0: GSM 850Ƶ��
     *             1: GSM 900 Ƶ��
     *             2: GSM 1800Ƶ��
     *             3: GSM 1900Ƶ��
     *             ��<enable>=0ʱ��<band>���Բ���
     * [ʾ��]:
     *     �� ����900Ƶ�ε�25��Ƶ��
     *       AT^GFREQLOCK=1,25,1
     *       OK
     *     �� ��ѯ��Ƶ״̬
     *       AT^GFREQLOCK?
     *       ^GFREQLCOK: 1,25,1
     *       OK
     */
    { AT_CMD_GSMFREQLOCK,
      AT_SetGFreqLock, AT_SET_PARA_TIME, AT_QryGFreqLock, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^GFREQLOCK", (VOS_UINT8 *)"(0,1),(0-1023),(0-3)" },
#endif

#if (FEATURE_DSDS == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: ����PS����ģʽ
     * [˵��]: ע�⣺����V8R1ʹ�á�
     *         ��DSDS���Ժ꿪��ʱ�����ô���������PS����ģʽ���ڼ���PS����ģʽ�������ὫPSҵ��������CSҵ����д���
     * [�﷨]:
     *     [����]: ^PSPROTECTMODE=<Type>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <Type>: �����ͣ�PS ����ģʽ���ͣ�ȡֵ0��1��
     *             0��ȥ����PS����ģʽ
     *             1������PS����ģʽ
     * [ʾ��]:
     *     �� ����PS����ģʽ
     *       AT^PSPROTECTMODE=1
     *       OK
     */
    { AT_CMD_PSPROTECTMODE,
      At_SetPsProtectModePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^PSPROTECTMODE", (VOS_UINT8 *)"(0,1)" },
#endif

#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯ��Ӫ�̱�ʶ��С��ID
     * [˵��]: ���������ڲ�ѯ4GС����Ϣ��
     * [�﷨]:
     *     [����]: ^CECELLID?
     *     [���]: <CR><LF>^CECELLID: < PLMNID>,< CI >,< CELLID><TAC> <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <PLMNID>: PLMN��Ϣ������������ַ�������ǰ��λ��MCC������λ����λ��MNC��
     *     <CI>: С����Ϣ���˸��ַ���16���Ʊ�ʾ��
     *     <CELLID>: 4bytes��16���������ַ������ͣ�С����Ϣ������ͬ<LAC>
     *     <TAC>: λ������Ϣ�������ַ���16���Ʊ�ʾ��
     * [ʾ��]:
     *     �� ��ѯ��ǰ4GС����Ϣ
     *       AT^CECELLID?
     *       ^CECELLID: 47031,101120,458,1
     *       OK
     */
    { AT_CMD_CECELLID,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryCecellidPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CECELLID", VOS_NULL_PTR },
#endif

    /*
     * [���]: Э��AT-������˽��
     * [����]: ��ѯWCDMA��С������������Ϣ
     * [˵��]: ������������ѯ��С������������Ϣ��
     *         CLģʽ�¸����֧�֡�CPMS
     *         ������ֻ��WCDMA����Ч��
     * [�﷨]:
     *     [����]: ^CELLINFO=<n>
     *     [���]: <CR><LF>cellinfo<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CELLINFO=?
     *     [���]: <CR><LF>^CELLINFO: (list of supported <n>s) <CR><LF>
     * [����]:
     *     <n>: ����ֵ��ȡֵ0��1��
     *             0����ѯ��С��������Ϣ��
     *             �������������ΪƵ�㣬���룬RSCP��ECN0����DCH״̬��������Ǽ����С����Ϣ����DCH״̬�������С����Ϣ������ȡ��������ֵ�����no cellinfo rslt��
     *             1����ѯ����������Ϣ��
     *             �������������ΪƵ�㣬���룬RSCP��ECN0��������8����������Ϣ��
     * [ʾ��]:
     *     �� ��ѯ��С��������Ϣ
     *       AT^CELLINFO=0
     *       10700,100,-65,-14
     *       OK
     */
    { AT_CMD_CELLINFO,
      At_SetCellInfoPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CELLINFO", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-������˽��
     * [����]: ����WCDMAС��������ʽ
     * [˵��]: ��������������WCDMAС��������ʽ��
     *         ������ֻ��WCDMA����Ч��
     * [�﷨]:
     *     [����]: ^CELLSRCH=<n>
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CELLSRCH?
     *     [���]: <CR><LF>^CELLSRCH: <n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������:<CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CELLSRCH=?
     *     [���]: <CR><LF>^CELLSRCH: (list of supported <n> s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��ȡֵ0��1��
     *             0����������ģʽΪ����ģʽ��
     *             1����������ģʽΪ��ʹ��������Ϣģʽ��
     * [ʾ��]:
     *     �� ����WCDMAС��������ʽ
     *       AT^CELLSRCH=0
     *       OK
     */
    { AT_CMD_CELLSRCH,
      At_SetCellSearch, AT_SET_PARA_TIME, At_QryCellSearch, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"^CELLSRCH", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: LTE����ģʽ��ѯ����
     * [˵��]: ���������ڲ�ѯLTE���뼼���µĴ���ģʽ��
     * [�﷨]:
     *     [����]: ^TRANSMODE?
     *     [���]: <CR><LF>^TRANSMODE: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <mode>: LTE�µĴ���ģʽ(Transmision Mode)������ֵ��
     *             1��TM1��
     *             2��TM2��
     *             3��TM3��
     *             4��TM4��
     *             5��TM5��
     *             6��TM6��
     *             8��TM8��
     *             9��TM9��
     *             10��TM10��
     * [ʾ��]:
     *     �� WCDMAģʽ�£���ѯLTE����ģʽ��
     *       AT^TRANSMODE?
     *       ERROR
     *     �� LTEģʽ�£���ѯLTE����ģʽ����ǰ����ģʽΪTM2��
     *       AT^TRANSMODE?
     *       ^TRANSMODE: 2
     *       OK
     */
    { AT_CMD_TRANSMODE,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryTransModePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^TRANSMODE", VOS_NULL_PTR },  // CMD_TBL_PIN_IS_LOCKED?CMD_TBL_NO_LIMITED?
};

/* ע��phone as AT����� */
VOS_UINT32 AT_RegisterPhoneAsCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atPhoneAsCmdTbl, sizeof(g_atPhoneAsCmdTbl) / sizeof(g_atPhoneAsCmdTbl[0]));
}
