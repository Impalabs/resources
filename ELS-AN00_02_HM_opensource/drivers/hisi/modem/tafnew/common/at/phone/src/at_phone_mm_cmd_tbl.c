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
#include "at_phone_mm_cmd_tbl.h"
#include "AtParse.h"
#include "at_parse_cmd.h"
#include "at_phone_mm_set_cmd_proc.h"
#include "at_phone_mm_qry_cmd_proc.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_MM_CMD_TBL_C

static const AT_ParCmdElement g_atPhoneMmCmdTbl[] = {
#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯϵͳ��Ϣ
     * [˵��]: ��ѯ��ǰϵͳ��Ϣ����ϵͳ����״̬��domain������״̬��ϵͳģʽ��SIM��״̬�ȡ�
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     *         Lģ�¸����֧�֡�
     * [�﷨]:
     *     [����]: ^SYSINFO
     *     [���]: <CR><LF>^SYSINFO: <srv_status>,<srv_domain>, <roam_status>, <sys_mode>, <sim_state> [,<lock_state>, <sys_submode>]<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <srv_status>: �����ͣ�ϵͳ����״̬��ȡֵ0��4��
     *             0���޷���
     *             1�������Ʒ���
     *             2��������Ч��
     *             3�������Ƶ��������
     *             4��ʡ�����˯״̬��
     *             ע�⣺
     *             ��NV_SYSTEM_APP_CONFIG(121)���õ�ǰ�ϲ�Ӧ��Ϊandroidʱ��
     *             1�������ǰ������˯״̬���Ҳ����ṩ�κη������ѯ�ķ���״̬Ϊ4��
     *             2�������ǰ��������˯״̬���Ҳ����ṩ�κη������ѯ�ķ���״̬Ϊ0��
     *             3�������ǰ�����ṩ���Ʒ������ѯ�ķ���״̬Ϊ1��
     *             ��NV_SYSTEM_APP_CONFIG(121)���õ�ǰ�ϲ�Ӧ�ò�Ϊandroidʱ��
     *             1�������ǰ������˯״̬�������ǰ�޷��������ṩ�����������ѯ�ķ���״̬Ϊ4��
     *     <srv_domain>: �����ͣ�������ȡֵ0��4��255��
     *             0���޷���
     *             1����CS����
     *             2����PS����
     *             3��PS+CS����
     *             4��CS��PS��δע�ᣬ����������״̬��
     *             ע�⣺<srv_status>ȡֵΪ0���޷���ʱ��������ȡֵ�����涨��û��ʵ�ʺ��壬�ںϷ���Χ�ڼ��ɡ�
     *     <roam_status>: ����״̬��
     *             2��δ֪
     *             ע�⣺
     *             1)�ɹ�ע�ᵽHPLMN��ʱ����ѯ����е�����״̬��ʾΪ0��
     *             2)�ɹ�ע�ᵽ����������ʱ����ѯ����е�����״̬��ʾΪ1��
     *             3)��������£���ѯ����е�����״̬��ʾΪ2��
     *     <sys_mode>: �����ͣ�ϵͳģʽ��ȡֵ0��8,15��
     *             0���޷���
     *             1��������
     *             2��������
     *             3��GSM/GPRSģʽ��
     *             4��������
     *             5��WCDMAģʽ��
     *             6��GPSģʽ����֧�֣���
     *             7��GSM/WCDMA��
     *             15��TD_SCDMAģʽ��
     *             ע�⣺<srv_status>ȡֵΪ0���޷���ʱ��������ȡֵ�����涨��û��ʵ�ʺ��壬�ںϷ���Χ�ڼ��ɡ�
     *     <sim_state>: �����ͣ���״̬��ȡֵ0��4��240��255��
     *             0��USIM��״̬��Ч��
     *             1��USIM��״̬��Ч��
     *             2��USIM��CS����Ч��
     *             3��USIM��PS����Ч��
     *             4��USIM��PS+CS�¾���Ч��
     *             240��ROMSIM��
     *             255��USIM�������ڡ�
     *     <lock_state>: �����ͣ���״̬��ȡֵ0��1��
     *             0��SIM��δ��CardLock����������
     *             1��SIM����CardLock����������
     *             ע�⣺�˲�������E5��̬�����翨��Ч��
     *     <sys_submode>: �����ͣ�ϵͳ��ģʽ��ȡֵ0��18��
     *             0���޷���
     *             1��GSMģʽ��
     *             2��GPRSģʽ��
     *             3��EDGEģʽ��
     *             4��WCDMAģʽ��
     *             5��HSDPAģʽ��
     *             6��HSUPAģʽ��
     *             7��HSDPA and HSUPAģʽ����TDҪ���̨��ʾΪHSPA��
     *             8��TD_SCDMAģʽ��
     *             9��HSPA+ģʽ��
     *             14��UMB��
     *             16��3xRTT��
     *             17��HSPA+(64QAM)ģʽ��
     *             18��HSPA+(MIMO)ģʽ��
     *             ע�⣺<srv_status>ȡֵΪ0���޷���ʱ��������ȡֵ�����涨��û��ʵ�ʺ��壬�ںϷ���Χ�ڼ��ɡ�
     * [ʾ��]:
     *     �� ϵͳ��Ϣ��ѯ
     *       AT^SYSINFO
     *       ^SYSINFO: 0,0,2,0,0,,0
     *       OK
     */
    { AT_CMD_SYSINFO,
      At_SetSystemInfo, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SYSINFO", TAF_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ����ϵͳ���òο�
     * [˵��]: ����ϵͳģʽ��G/W�������Ƶ��������֧�ֺ�domain�����ԡ�
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     *         Lģ��֧�ָ����
     * [�﷨]:
     *     [����]: ^SYSCFG=<mode>,<acqorder>,<band>,<roam>,<srvdomain>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SYSCFG?
     *     [���]: <CR><LF>^SYSCFG: <mode>,<acqorder>,<band>,<roam>,<srvdomain><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^SYSCFG=?
     *     [���]: <CR><LF>^SYSCFG: (list of supported <mode>s),
     *             (list of supported <acqorder>s),
     *             (list of supported( <band>)s),
     *             (list of supported <roam>s),
     *             (list of supported <srvdomain>s) <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������ģʽ��
     *             2���Զ�ѡ��
     *             3��CDMAģʽ��
     *             4��HDRģʽ��
     *             8��CDMA/HDR HYBRID ģʽ��
     *             13��GSM ONLY��
     *             14��WCDMA ONLY��
     *             16�����ı䡣
     *     <acqorder>: ����ֵ���������ȼ���ȡֵ0��3��
     *             0���Զ���
     *             1����GSM��WCDMA��
     *             2����WCDMA��GSM��
     *             3�����ı䡣
     *             ע�⣺<mode>Ϊ��ģʱ��<acqorder>���õ�ֵ��Ч����<mode>Ϊ13ʱ��MS��<acqorder>Ϊ1��<mode>Ϊ14ʱ��MS��<acqorder>Ϊ2��
     *             ����CDMA/HDRģʽ��˵��û�н������ȼ����
     *     <band>: �����ͣ�Ƶ��ѡ�񣬲���Ϊ16���ƴ���ȡֵΪ���и�����������0x3FFF_FFFF��0x4000_0000����ĸ���������ֵ����ѯ����иò�������UEʵ��֧�ֵ�Ƶ����Ϣ��䡣
     *             0x80��GSM DCS systems��
     *             0x100��Extended GSM 900��
     *             0x200��Primary GSM 900��
     *             0x8_0000��GSM850��
     *             0x10_0000��GSM Railway 900��
     *             0x20_0000��GSM PCS 1900��
     *             0x1_0000��WCDMA IMT-E 2600��BAND-VII��;
     *             0x40_0000��WCDMA IMT 2000��BAND-I����
     *             0x80_0000��WCDMA US PCS 1900��BAND-II����
     *             0x100_0000��WCDMA 1800��BAND-III����
     *             0x200_0000��WCDMA US 1700��BAND-IV����
     *             0x400_0000��WCDMA US 850��BAND-V����
     *             0x800_0000��WCDMA 800��BAND-VI��;
     *             0x2_0000_0000_0000��WCDMA US 900��BAND-VIII����
     *             0x4_0000_0000_0000��WCDMA US 1700��BAND-IX����
     *             0x1000_0000_0000_0000��WCDMA 850��BAND-XIX��;
     *             0x2000_0000_0000_0000��WCDMA 1500��BAND-XI��;
     *             0x4000_0000�����ı䣻
     *             0x3FFF_FFFF���κ�Ƶ����Ĭ��֧�ֵ�Ƶ��0x4F8_0380����
     *     <roam>: ��������δ����ʱ��
     *             ����ֵ������֧�֣�ȡֵ0��2��
     *             0��MBB CUST���ʱ��֧�ֹر����Σ���������Ϊ0�����򣬲�֧�֣�
     *             1���������Σ�
     *             2�����ı䡣
     *             �������Լ���ʱ��
     *             ����ֵ������֧�֣�ȡֵ0��3��
     *             0���������ڹ������Σ�
     *             1�������������Σ��رչ������Σ�
     *             2���رչ������Σ������������Σ�
     *             3���رչ��ڹ������Ρ�
     *             ע�⣺��������ͨ��NV������Ƿ񼤻���ζ���CDMA/HDR��Ч�����ԣ����ò����ᱣ�档
     *     <srvdomain>: ����ֵ��������ȡֵ0��4��
     *             0��CS ONLY��������ע��CS������Ҫ����PS�����ݷ����ʱ���ٽ���PS���ţ������ڽ���PS�����ʱ�����PSȥ���ţ���
     *             1��PS ONLY��������ע��PS������Ҫ����CS�����������ʱ���ٽ���CSע�ᣬ�����ڽ���CS�����ʱ�����CSȥע�ᣩ��
     *             2��CS_PS��
     *             3��ANY����ͬ��0����
     *             4�����ı䡣
     *             ע������CDMA/HDR��˵����������ԣ����Ĳ����ᱣ�档
     * [ʾ��]:
     *     �� ����Ϊ˫ģ��WCDMAģʽ���ȣ�֧��WCDMA BAND-I��GSM����Ƶ��
     *       AT^SYSCFG=2,2,780380,2,4
     *       OK
     *     �� ����WCDMA ONLY��Ƶ�����ı䣬CS ONLY
     *       AT^SYSCFG=14,3,40000000,2,0
     *       OK
     *     �� ��ѯϵͳ����
     *       AT^SYSCFG?
     *       ^SYSCFG: 14,2,780380,1,0
     *       OK
     *     �� ����SYSCFG
     *       AT^SYSCFG=?
     *       ^SYSCFG: (2,13,14,16),(0-3),(-Band-),(0-2),(0-4)
     *       OK
     */
    { AT_CMD_SYSCFG,
      At_SetSysCfgPara, AT_SYSCFG_SET_PARA_TIME, At_QrySysCfgPara, AT_QRY_PARA_TIME, AT_TestSyscfg, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SYSCFG", (VOS_UINT8 *)"(2,3,4,8,13,14,16),(0-3),(@Band),(0-3),(0-4)" },
#endif
#if (FEATURE_LTE == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: ������չϵͳ����
     * [˵��]: ����������ϵͳģʽ������������Ƶ��������֧�ֺ�domain�����ԡ�
     *         acqorder���������֧��6����ʽ�����ã���ȫ��ʽ�����á�
     *         ��������ڵ���֧��ר��ʱ����ʹ�ã�����ǰ����֧��LTE������֧�� NR ��������
     * [�﷨]:
     *     [����]: ^SYSCFGEX=<acqorder>,<band>,<roam>,<srvdomain>,<lteband>,<reserve1>,<reserve2>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SYSCFGEX?
     *     [���]: <CR><LF>^SYSCFGEX: <acqorder>,<band>,<roam>,<srvdomain>,<lteband><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^SYSCFGEX=?
     *     [���]: ^SYSCFGEX: (list of supported <acqorder>s),(list of supported( <band>,<band_name>)s),(list of supported <roam>s),(list of supported <srvdomain>s),(list of supported( <lteband>,<lteband_name>)s),<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ע��list of supported <acqorder>s��֮ǰ��01��02,03,00,09��01,02,03�Ķ�����ϸ�Ϊ���ڵ�01,02,03,04,07,08ֻ�ϱ����е�����ʽ.��
     *             01��GSM��
     *             02��WCDMA��
     *             03��LTE��
     *             08��NR��
     * [����]:
     *     <acqorder>: �����������ַ������͡���ȡֵ�ǡ�00������99���������²�������ϡ�
     *             ��00����GUL+NRģʽ��
     *             ��01����GSM��
     *             ��02����WCDMA��
     *             ��03����LTE��
     *             ��04����CDMA 1X��
     *             ��07����CDMA EVDO��
     *             ��08����NR��
     *             ��99�����ޱ仯��
     *             ע��TDSCDMA��WCDMA����һ�����뼼������SYSCFGEX���ý���ģʽ����02ʱ����ʾTDSCDMA��WCDMAͬʱ֧�֣���֮����֧�֡�
     *             ���磺��03����ʾLTE Only����030201����ʾ��LTE��WCDMA�����GSM (LTE->WCDMA->GSM)����0302����ʾ��LTE��WCDMA������GSM��
     *             ��99����������ֵ��ϣ�����ʾ���޸ĵ��������������
     *             ��00����������ֵ��ϣ�����ʾ�޸ĵ��������������ΪGUL+NRģʽ�����������������ɵ���ȷ����
     *     <band>: Ƶ��ѡ�񣬺�ģʽ��ѡ����أ�����ȡ���ڵ������ܣ������Ϊ16���ƣ�ȡֵΪ���и�����������0x3FFFFFFF��0x40000000����ĸ���������ֵ����ѯ����иò�������UEʵ��֧�ֵ�Ƶ����Ϣ��䡣
     *             80��CM_BAND_PREF_GSM_DCS_1800����GSM DCS systems��
     *             100��CM_BAND_PREF_GSM_EGSM_900����Extended GSM 900��
     *             200��CM_BAND_PREF_GSM_PGSM_900����Primary GSM 900��
     *             100000��CM_BAND_PREF_GSM_RGSM_900����Railways GSM 900;
     *             200000��CM_BAND_PREF_GSM_PCS_1900����GSM PCS��
     *             10000��CM_BAND_PREF_WCDMA_VII_2600����WCDMA 2600;
     *             400000��CM_BAND_PREF_WCDMA_I_IMT_2000����WCDMA IMT 2000��
     *             800000��CM_BAND_PREF_WCDMA_II_PCS_1900����WCDMA PCS��
     *             1000000��CM_BAND_PREF_WCDMA_III_1800����WCDMA 1800��
     *             2000000��CM_BAND_PREF_WCDMA_IV_1700����WCDMA 1700;
     *             4000000��CM_BAND_PREF_WCDMA_V_850����WCDMA 850;
     *             8000000��CM_BAND_PREF_WCDMA_VI_800����WCDMA 800;
     *             3FFFFFFF��CM_BAND_PREF_ANY�����κ�Ƶ����
     *             40000000��CM_BAND_PREF_NO_CHANGE����Ƶ�����仯��
     *             0004000000000000 (CM_BAND_PREF_WCDMA_IX_1700)��WCDMA_IX_1700��
     *             0000000000400000 (CM_BAND_PREF_WCDMA_IMT)��WCDMA_IMT(2100)��
     *             0002000000000000��WCDMA 900��
     *             1000000000000000(CM_BAND_PREF_WCDMA_XIX_850)��WCDMA_XIX_850��
     *             2000000000000000(CM_BAND_PREF_WCDMA_XI_1500)��WCDMA_XI_1500��
     *             00680380��Automatic��
     *             00080000��GSM 850��
     *     <band_name>: �ַ������ͣ�Ƶ�����ơ�
     *     <roam>: ��������δ����ʱ��
     *             ����ֵ��ȡֵ0��2��
     *             0��MBB CUST���ʱ��֧�ֹر����Σ���������Ϊ0�����򣬲�֧�֣�
     *             1���������Σ�
     *             2�����ı䡣
     *             �������Լ���ʱ��
     *             ����ֵ������֧�֣�ȡֵ0��3��
     *             0���������ڹ������Σ�
     *             1�������������Σ��رչ������Σ�
     *             2���رչ������Σ������������Σ�
     *             3���رչ��ڹ������Ρ�
     *             ע�⣺��������ͨ��NV������Ƿ񼤻
     *     <srvdomain>: �����á�
     *             0��CS_ONLY��
     *             1��PS_ONLY��
     *             2��CS_PS��
     *             3��ANY��
     *             4���ޱ仯��
     *             5��NULL��
     *             ע��
     *             1������������ΪNULL����ʾCS��PS�����򶼲����ã�
     *             2�����õ�ģʽ�ﺬ��L��NR����������������Ϊ0��3��
     *     <lteband>: LTEƵ��ѡ�������Ϊ16���ƣ�ȡֵΪ���и�����������0x7FFFFFFFFFFFFFFF����ĸ���������ֵ����ѯ����иò�������UEʵ��֧�ֵ�Ƶ����Ϣ��䡣
     *             7FFFFFFFFFFFFFFF��CM_BAND_PREF_ANY�����κ�Ƶ����
     *             1 (CM_BAND_PREF_LTE_EUTRAN_BAND1)��LTE BC1��
     *             40 (CM_BAND_PREF_LTE_EUTRAN_BAND7)��LTE BC7��
     *             1000 (CM_BAND_PREF_LTE_EUTRAN_BAND13)��LTE BC13��
     *             10000 (CM_BAND_PREF_LTE_EUTRAN_BAND17)��LTE BC17��
     *             80 0000 0000 (CM_BAND_PREF_LTE_EUTRAN_BAND40)��LTE BC40��
     *             40000000��CM_BAND_PREF_NO_CHANGE����Ƶ�����仯
     *             ȱʡ��ͬ����40000000һ����Ƶ�����仯
     *             FF��FF����64������ͬ0x7FFFFFFFFFFFFFFFһ������ʾ����Ƶ������
     *     <lteband_name>: �ַ������ͣ�Ƶ�����ơ�
     *     <reserve1>: �����ֶ�1��
     *     <reserve2>: �����ֶ�2��
     * [ʾ��]:
     *     �� ����ϵͳ����
     *       AT^SYSCFGEX="00",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,0,0
     *       OK
     *       �������ã���RAT����˳��ΪNR->LTE->WCDMA->GSM�������Ƿ�Я��NR/LTE��NR/LTE����ơ�
     *     �� ��ѯϵͳ����
     *       AT^SYSCFGEX?
     *       ^SYSCFGEX: "00",4C00000,0,2,8000011041
     *       OK
     *     �� ����ϵͳ����
     *       AT^SYSCFGEX=?
     *        ^SYSCFGEX: "01","02","03","04,"07","08",(-band-),(0-2),(0-4),((7FFFFFFFFFFFFFFF, "All bands"))
     *        OK
     *     �� CSҵ�����ʱ������ϵͳ����
     *       AT^SYSCFGEX="00",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,0,0
     *       OK
     */
    { AT_CMD_SYSCFGEX,
      AT_SetSysCfgExPara, AT_SYSCFG_SET_PARA_TIME, AT_QrySysCfgExPara, AT_QRY_PARA_TIME, AT_TestSyscfgEx, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SYSCFGEX", (VOS_UINT8 *)"(Acqorder),(@Band),(0-3),(0-5),(@Lteband)" },
#endif

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯ��չϵͳ��Ϣ
     * [˵��]: �������ѯ��ǰϵͳ��Ϣ����ϵͳ����״̬��domain���Ƿ����Σ�ϵͳģʽ��SIM��״̬��.
     * [�﷨]:
     *     [����]: ^SYSINFOEX
     *     [���]: <CR><LF>^SYSINFOEX: <srv_status>,<srv_domain>,<roam_status>,<sim_state>,<lock_state>,<sysmode>,<sysmode_name><submode>,<submode_name><CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <srv_status>: ϵͳ����״̬��
     *             0���޷���
     *             1�������Ʒ���
     *             2��������Ч��
     *             3�������Ƶ��������
     *             4��ʡ�����˯״̬��
     *             ע�⣺
     *             ��NV_SYSTEM_APP_CONFIG(NVID=121)���õ�ǰ�ϲ�Ӧ��Ϊandroidʱ��
     *             1�������ǰ������˯״̬���Ҳ����ṩ�κη������ѯ�ķ���״̬Ϊ4��
     *             2�������ǰ��������˯״̬���Ҳ����ṩ�κη������ѯ�ķ���״̬Ϊ0��
     *             3�������ǰ�����ṩ���Ʒ������ѯ�ķ���״̬Ϊ1��
     *             ��NV_SYSTEM_APP_CONFIG(NVID=121)���õ�ǰ�ϲ�Ӧ�ò�Ϊandroidʱ��
     *             1�������ǰ������˯״̬����ǰ�޷��������ṩ�����������ѯ�ķ���״̬Ϊ4��
     *     <srv_domain>: ϵͳ������
     *             0���޷���
     *             1����CS����
     *             2����PS����
     *             3��PS+CS����
     *             4��CS��PS��δע�ᣬ����������״̬��
     *     <roam_status>: ����״̬��
     *             0��������״̬��
     *             1������״̬��
     *             2��δ֪��
     *             ע�⣺
     *             1)�ɹ�ע�ᵽHPLMN��ʱ����ѯ����е�����״̬��ʾΪ0��
     *             2)�ɹ�ע�ᵽ����������ʱ����ѯ����е�����״̬��ʾΪ1��
     *             3)��������£���ѯ����е�����״̬��ʾΪ2��
     *     <sim_state>: SIM��״̬��
     *             0��USIM��״̬��Ч��
     *             1��USIM��״̬��Ч��
     *             2��USIM��CS����Ч��
     *             3��USIM��PS����Ч��
     *             4��USIM��PS+CS�¾���Ч��
     *             240��ROMSIM�汾��
     *             255��USIM�������ڡ�
     *     <lock_state>: SIM����LOCK״̬��
     *             0��SIM��δ��CardLock����������
     *             1��SIM����CardLock����������
     *             ע�⣺�˲�������E5��̬�����翨��Ч��
     *     <sysmode>: ϵͳ��ʽ��
     *             0��NO SERVICE��
     *             1��GSM��
     *             3��WCDMA��
     *             4��TD-SCDMA��
     *             5��WIMAX��
     *             6��LTE��
     *             9��SVLTE/SRLTE��
     *             10��EUTRAN-5GC��
     *             11��NR-5GC��
     *             ע�⣺���ò����ķ���ֵ���ڱ��淶�ĺϷ���Χ��0~12������̨Ĭ�ϰ���3��WCDMA��������������
     *     <sysmode_name>: ϵͳ��ʽ��Ӧ���ַ�����ʾ��ʽ��
     *             �ò������ַ�������ʽ���ص�ǰϵͳģʽ���ƣ���ȡֵ��Ӧ���������<sysmode>ȡֵ����Ӧ���ַ���������<sysmode> = 3, <sysmode_name>="WCDMA"��
     *     <submode>: ϵͳ��ģʽ��δ�г���ֵ��ʾδʹ�á�
     *             0��NO SERVICE��
     *             1��GSM��
     *             2��GPRS��
     *             3��EDGE��
     *             41��WCDMA��
     *             42��HSDPA��
     *             43��HSUPA��
     *             44��HSPA��
     *             45��HSPA+��
     *             46��DC-HSPA+��
     *             61��TD-SCDMA��
     *             62��HSDPA��
     *             63��HSUPA��
     *             64��HSPA��
     *             65��HSPA+��
     *             81��802.16e��
     *             101��LTE��
     *             110��EUTRAN-5GC��
     *             111��NR-5GC��
     *             ����ģʽ�µķ��ز���ֵ�Ƿ�����̨Ĭ�ϰ�����ģʽ�ķ���ֵ������ʾ����
     *     <submode _name>: ϵͳ��ģʽ��Ӧ���ַ�����ʾ��ʽ��
     *             �ò������ַ�������ʽ���ص�ǰ������ģʽ���ƣ���ȡֵ��Ӧ������ڶ�������<submode>ȡֵ����Ӧ���ַ���������<submode> = 45, <submode _name> = "HSPA+"��
     * [ʾ��]:
     *     �� UE��ǰ������WCDMAģʽ�µ�DC-HSPA+�����¡�
     *       AT^SYSINFOEX
     *       ^SYSINFOEX: 2,3,1,1,1,3,"WCDMA",46,"DC-HSPA+"
     *       OK
     */
    { AT_CMD_SYSINFOEX,
      At_SetSystemInfoEx, AT_QRY_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SYSINFOEX", TAF_NULL_PTR },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: Э��AT-�绰����
     * [����]: ����MT����ģʽ
     * [˵��]: ����MT����ģʽ��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     *         Lģ��֧�ָ����
     * [�﷨]:
     *     [����]: ^CPAM=[<act>[,<pri>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CPAM?
     *     [���]: <CR><LF>^CPAM: <act>,<pri><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CPAM=?
     *     [���]: <CR><LF>^CPAM: (list of supported <act>s), (list of supported <pri>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <act>: ����ֵ������ģʽ��ȡֵ0��3��Ĭ��ֵΪ3��
     *             0��WCDMA��ģ��
     *             1��WCDMA/GSM˫ģ��
     *             2��GSM��ģ��
     *             3�����ı䡣
     *     <pri>: ����ֵ���������ȼ���ȡֵ0��2��Ĭ��ֵΪ2��
     *             0��GSM���ȣ�
     *             1��WCDMA���ȣ�
     *             2�����ı䡣
     * [ʾ��]:
     *     �� ����Ϊ˫ģ��WCDMAģʽ����
     *       AT^CPAM=1,1
     *       OK
     *     �� ����ΪWCDMA��ģ
     *       AT^CPAM=0,1
     *       OK
     *     �� ����ΪGSM��ģ
     *       AT^CPAM=2,0
     *       OK
     *     �� ��ѯ����ģʽ
     *       AT^CPAM?
     *       ^CPAM: 1,1
     *       OK
     *     �� ����CPAM
     *       AT^CPAM=?
     *       ^CPAM: (0-3),(0-2)
     *       OK
     */
    { AT_CMD_CPAM,
      At_SetCpamPara, AT_SET_PARA_TIME, At_QryCpamPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CPAM", (VOS_UINT8 *)"(0-3),(0-2)" },
#endif
    /*
     * [���]: Э��AT-�绰����
     * [����]: CS��PS����
     * [˵��]: CS��PS������ز�����
     *         ע��ֻ֧��stateΪ����״̬(��0)��modeΪ˫�򣨼�3��ʱ���������õ�����reason������reason=1Ϊ����̽���Ź���������
     *         ��L��ʹ�ô�AT����ʱ��LMM�����ڲ�������ʱ����ĿǰLMM����ö�ʱ��ʱ��Ϊ15s������ʱ����ʱ����������ȥע�᣻Э������Ϊ������15s��T3421��ʱ����ǰ�Ĵζ�ʱ����ʱ�󣬶��������·���ȥע�����̣�ͬʱ��������T3421��ʱ������5�ζ�ʱ����ʱ�󣬱���ȥע�ᡣ�˴�ʵ����Э�鲻����
     * [�﷨]:
     *     [����]: ^CGCATT=<state>,<mode>[,<reason>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CGCATT?
     *     [���]: <CR><LF>^CGCATT: <PS state>,<CS state><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CGCATT=?
     *     [���]: <CR><LF>^CGCATT: (list of supported <state>s), (list of supported <mode>s), (list of supported <reason>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <state>: ����ֵ��CS��PS�����״̬��
     *             0������״̬��
     *             1������״̬��
     *     <mode>: ����ֵ������ģʽ��
     *             1���޸�PS��ĸ���״̬��
     *             2���޸�CS��ĸ���״̬��
     *             3��ͬʱ�޸�CS��PS��ĸ���״̬��
     *     <PS state>: PS�����״̬������ͬ<state>��
     *     <CS state>: CS�����״̬������ͬ<state>��
     *     <reason>: ����ֵ������ԭ��ֵ��
     *             0����ԭ��
     *             1������̽���Ź���������
     * [ʾ��]:
     *     �� ����CS��PS��ȥ���ţ�ԭ��Ϊ����̽���Ź�������
     *       AT^CGCATT=0,3,1
     *       OK
     */
    { AT_CMD_CGCATT,
      At_SetCgcattPara, AT_CGATT_SET_PARA_TIME, At_QryCgcattPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CGCATT", (VOS_UINT8 *)"(0,1),(1-3),(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ���÷���״̬�仯
     * [˵��]: ���÷���״̬�����仯ʱ��MT�Ƿ������ϱ��µķ���״̬��TE��
     * [�﷨]:
     *     [����]: ^SRVST=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SRVST=?
     *     [���]: <CR><LF>^SRVST: (list of supported <n>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: �����ͣ������Ƿ������ϱ�����״̬��ȡֵ0��1��
     *             0����ֹ�����ϱ�����״̬(Ĭ��ֵ)��
     *             1�����������ϱ�����״̬��
     * [ʾ��]:
     *     �� ���������ϱ�����״̬
     *       AT^SRVST=1
     *       OK
     */
    { AT_CMD_SRVST,
      At_SetSrvstPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SRVST", (TAF_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ֹPLMN�б����
     * [˵��]: ��ֹPLMN�б�Ĳ�ѯ����Ӻ�ɾ��������
     *         ʹ�ø����PLMN��ӵ���ֹ�б��ӽ�ֹ�б�ɾ��ʱ��������ػ������µ���ܶ�EPLMN��Ч��
     * [�﷨]:
     *     [����]: ^CFPLMN=<mode>[,<oper>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *             ע��
     *             OK��ʾ��ֹ�б��޸ĳɹ���
     *             +CME ERROR: <err>��ʾ��ֹ�б������޸�
     *     [����]: ^CFPLMN?
     *     [���]: <CR><LF>^CFPLMN: <num>[,( numeric <oper>)s] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CFPLMN=?
     *     [���]: <CR><LF>^CFPLMN: (list of supported <mode>s), (list of supported <oper>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������ģʽ��ȡֵ0��1��
     *             0��ɾ����ֹPLMN����ָ��<oper>ʱ��ʾɾ����ֹPLMN�б�
     *             1����ӽ�ֹPLMN��
     *     <oper>: �ַ������ͣ���Ӫ��ID��
     *     <num>: ����ֵ����ǰ��ֹPLMN�ĸ�����
     * [ʾ��]:
     *     �� ɾ���ǿս�ֹPLMN�б�
     *       AT^CFPLMN=0
     *       OK
     *     �� ���PLMN��"12345"��"12346"����ֹPLMN�б���
     *       AT^CFPLMN=1,"12345"
     *       OK
     *       AT^CFPLMN=1,"12346"
     *       OK
     *     �� �ӽ�ֹPLMN�б���ɾ��PLMN IDΪ"12345"��PLMN
     *       AT^CFPLMN=0,"12345"
     *       OK
     *     �� ��ѯ��ǰ��ֹPLMN�б�
     *       AT^CFPLMN?
     *       ^CFPLMN: 1,"12346"
     *       OK
     *     �� ����^CFPLMN
     *       AT^CFPLMN=?
     *       ^CFPLMN: (0,1),(oper)
     *       OK
     */
    { AT_CMD_CFPLMN,
      At_SetFplmnPara, AT_SET_PARA_TIME, At_QryFPlmnPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CFPLMN", (VOS_UINT8 *)"(0,1),(oper)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ����ϵͳģʽ�仯
     * [˵��]: ���õ�ϵͳģʽ�仯ʱ��MT�Ƿ������ϱ���ָʾ��TE��
     * [�﷨]:
     *     [����]: ^MODE=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^MODE=?
     *     [���]: <CR><LF>^MODE: (list of supported <n>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: �����ͣ������Ƿ������ϱ�ϵͳģʽ��ȡֵ0��1��Ĭ��ֵΪ0��
     *             0����ֹ�����ϱ�ϵͳģʽ��
     *             1�����������ϱ�ϵͳģʽ��
     * [ʾ��]:
     *     �� ���������ϱ�ϵͳģʽ
     *       AT^MODE=1
     *       OK
     */
    { AT_CMD_SYSMODE,
      At_SetModePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^MODE", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: �����Ƿ���ٿ���
     * [˵��]: �����Ƿ�Ϊ���ٿ���ģʽ��
     * [�﷨]:
     *     [����]: ^CQST=<value>
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CQST?
     *     [���]: <CR><LF>^CQST: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CQST=?
     *     [���]: <CR><LF>^CQST: (list of supported <value>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: 0���ǿ��ٿ���ģʽ��
     *             1�����ٿ���ģʽ��
     * [ʾ��]:
     *     �� ���ÿ��ٿ���
     *       AT^CQST=1
     *       OK
     *     �� ��ѯ
     *       AT^CQST?
     *       ^CQST: 1
     *       OK
     */
    { AT_CMD_CQST,
      At_SetQuickStart, AT_SET_PARA_TIME, At_QryQuickStart, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CQST", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: פ������仯
     * [˵��]: ��ѯ����û���ѯPLMN��Ϣ��
     *         �����ϱ���פ������仯�������ϱ�PLMN��
     * [�﷨]:
     *     [����]: ^PLMN?
     *     [���]: <CR><LF>^PLMN: [<mcc>],[<mnc>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: -
     *     [���]: <CR><LF>^PLMN: <mcc>,<mnc><CR><LF>
     * [����]:
     *     <mcc>: �����롣
     *     <mnc>: �����롣
     * [ʾ��]:
     *     �� PLMN��ѯ
     *       AT^PLMN?
     *       ^PLMN: 460,01
     *       OK
     *       �����ϱ�
     *       ^PLMN: 460,01
     */
    { AT_CMD_PLMN,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryPlmnPara, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PLMN", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: С�������ֹ��Ϣ�ϱ�
     * [˵��]: ���������������ϱ�С�������ֹ��Ϣ�Լ��ֶ���ѯ���һ�ε�С�������ֹ��Ϣ��<srv_domain> ,<cell_ac>,<reg_restrict>,<paging_restrict>��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ACINFO: <srv_domain>,<cell_ac>,<reg_restrict>,<paging_restrict><CR><LF>
     *     [����]: ^ACINFO?
     *     [���]: <CR><LF>^ACINFO: <srv_domain>,<cell_ac>,<reg_restrict>,<paging_restrict>
     *             [��]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <srv_domain>: 0��CS��
     *             1��PS��
     *     <cell_ac>: 0�������ޣ�
     *             1����������ҵ�����ޣ�
     *             2�����ƽ���ҵ�����ޣ�
     *             3����������ҵ��ͽ���ҵ��
     *     <reg_restrict>: 0��������srv_domainָ���������ע�᣻
     *             1������srv_domainָ���������ע�ᡣ
     *     <paging_restrict>: 0��������srv_domainָ���������Ѱ����
     *             1������srv_domainָ���������Ѱ����
     * [ʾ��]:
     *       CS����ҵ������PSѰ������ʱ��ACINFO�ϱ�����
     *       ^ACINFO: 0,2,0,0
     *       ^ACINFO: 1,0,0,1
     *     �� ��ѯ���һ��С���Ľ����ֹ��Ϣ���
     *       AT^ACINFO?
     *       ^ACINFO: 0,3,1,1
     *       ^ACINFO: 1,0,0,0
     *       OK
     *     �� LTE�²�ѯACINFO
     *       AT^ACINFO?
     *       ERROR
     */
    { AT_CMD_ACINFO,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryAcInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^ACINFO", VOS_NULL_PTR },


    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯMM INFO����Ӫ������
     * [˵��]: ���������ڲ�ѯMM INFO�е���Ӫ�����ơ���MM INFO��Чʱ�����ؿա�
     * [�﷨]:
     *     [����]: ^MMPLMNINFO?
     *     [���]: <CR><LF>^MMPLMNINFO: <long_name>,<short_name>
     *             <CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <long_name>: ��ǰ��Ӫ�̵ĳ����ƣ���UCS-2��ʽ���롣��󳤶�40�ֽڣ�80��ASCII�ַ�����
     *     <short_name>: ��ǰ��Ӫ�̵Ķ����ƣ���UCS-2��ʽ���롣��󳤶�36�ֽڣ�72��ASCII�ַ�����
     * [ʾ��]:
     *       ��ѯMM INFO�е���Ӫ������
     *     �� ��Ӫ�̳�����ΪL700��������ΪS700����Ӧ��UCS2����ֱ�Ϊ004C003700300030��0053003700300030��
     *       AT^MMPLMNINFO?
     *       ^MMPLMNINFO: 004C003700300030,0053003700300030
     *       OK
     */
    { AT_CMD_MMPLMNINFO,
      VOS_NULL_PTR, AT_SET_PARA_TIME, At_QryMmPlmnInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^MMPLMNINFO", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ע���ҵ�����������DETACH��������ܾ�ԭ��ֵ�����ϱ�
     * [˵��]: ����������AP-Modem ��̬��Modem �����ϱ�ע���ҵ�����������DETACH���������·��ľܾ�ԭ��ֵ��USIM ��Ȩʧ�ܵ�ԭ��ֵ����Modem ע���ҵ�����������DETACH�����У������·��ľܾ�ԭ��ֵ��USIM ��Ȩʧ�ܵ�ԭ��ֵͨ����AT ��������ϱ���
     *         ֻ�ϱ�Э������ȷ�涨�ľܾ�ԭ��ֵ��Balong�ڲ���չ�ļ�Ȩʧ�ܣ�#256 Authentication Failure��������ע����CSʧ�ܵ�Other Cause��#258����#2��#16��#17��#22������ԭ��ֵ����CS PSע����������Ӧ��#301����CS PSע�Ὠ���쳣��#302��#303����
     *         USIM��Ȩʧ�ܵ�ԭ��ֵҲͨ���������ϱ���USIM�ľܾ�ԭ��ֵ��65537��ʼ������(#65537��65538��65539��65540��65541)��
     * [�﷨]:
     *     [����]: ^REJINFO?
     *     [���]: [<CR><LF>^REJINFO: <PLMN ID>, <Service Domain>,<Reject Cause>,<Rat Type>,<Reject Type>,<Original Reject Cause >,<Lac>,<Rac>, <Cell Id><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ע��������յ�����ܾ�ԭ��ֵʱ
     *     [���]: ^REJINFO: <PLMN ID>,<Service Domain>,<Reject Cause>,<Rat Type>,<Reject Type>,<Original Reject Cause >,<Lac>,<Rac>, <Cell Id>
     * [����]:
     *     <PLMN ID>: ����ID��
     *     <Service Domain>: �ϱ��ܾ�ԭ��ֵ�ķ�����
     *             0��CS only��
     *             1��PS only��
     *             2��CS+PS��
     *     <Reject Cause>: ����ܾ�ԭ��ֵ���ϱ�Э������ȷ�涨�ľܾ�ԭ��ֵ��Balong�ڲ���չ�ļ�Ȩʧ�ܣ�#256 Authentication Failure��������ע����CSʧ�ܵ�Other Cause��#258����#2��#16��#17��#22������ԭ��ֵ����CS PSע����������Ӧ��#301����CS PSע�Ὠ���쳣��#302��#303����
     *             ����USIM��Ȩʧ�ܵ�ԭ��ֵҲͨ���������ϱ���USIM�ľܾ�ԭ��ֵ��65537��ʼ������(#65537��65538��65539��65540��65541)��
     *     <Rat Type>: ���ܵ��������͡�
     *             0��GERAN��
     *             1��UTRAN��
     *             2��E-UTRAN��
     *             5��NR-5GC��
     *             6��������
     *     <Reject Type>: ���类�����ͣ�
     *             0��LAU����
     *             1����Ȩʧ��
     *             2��ҵ�����󱻾�
     *             3������detach����
     *             4��ATTACH����
     *             5��RAU����
     *             6��TAU����
     *     <Original Reject Cause>: ����ܾ���ԭʼԭ��ֵ�����û������ܾ���ԭ��ֵ������0��
     *     <Lac>: ��2G��3G��4G��Ϊ2byte��16���������ַ������ͣ�λ������Ϣ�����磺��00C3����ʾ10���Ƶ�195��
     *             ��5G��Ϊ3byte��16���������ַ������ͣ�λ������Ϣ�������ַ���16���Ʊ�ʾ����������0000C3����10���Ƶ�195����
     *     <Rac>: ·��������
     *     <Cell Id>: 8bytes��16���������ַ������ͣ�С����Ϣ��ʹ��Half-Byte�루���ֽڣ����룬����000000000A444202����ʾ��4�ֽ���0����4�ֽ���0x0A444202��
     *     <Esm Reject Cause>: ��LNASע�� ���ܾ�#19ʱ,��������ֵ
     * [ʾ��]:
     *       �����ϱ�4G��
     *       ^REJINFO: 46000,1,40,2,3,40,"26F8","FF","000000000A444202"
     *       �����ϱ�5G��
     *       ^REJINFO: 46000,1,40,5,3,40,"0026F8","FF","0000000F0A444202"
     *     �� ��ѯ�����ѯ�����Ǳ�4G�ܾ���
     *       AT^REJINFO?
     *       ^REJINFO: 46000,1,40,2,3,40,"26F8","FF","000000000A444202"
     *       OK
     *     �� ��ѯ�����ѯ�����Ǳ�5G�ܾ���
     *       AT^REJINFO?
     *       ^REJINFO: 46000,1,40,5,3,40,"0026F8","FF","0000000F0A444202"
     *       OK
     */
    { AT_CMD_REJINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryRejInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^REJINFO", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: Ƶ����Ϣɨ��
     * [˵��]: ���������֧�ֵ�С�����������޼����뼼���ķ�Χ
     *         ��ȡָ�����뼼����С��Ƶ����Ϣ�б�
     *         ��ȡƵ����Ϣ�����У���ͨ���������ַ��������ַ�����ͨ��NV en_NV_Item_AT_ABORT_CMD_PARA���ã������ô���ַ���ΪANY��ASCII�룬���ʾ�����ַ�����ϣ��������ַ�����NV�����õ��ַ�������ϸù��̣������ɺ������ǰ�Ѿ�ɨ�赽��Ƶ����Ϣ�Լ���OK��������ַ�������NV�����ã���
     *         GUΪ��ģʱ�·�L��NRģʽ��NETSCAN����᷵��ERROR��LΪ��ģʱ�·�GUģʽ��NETSCAN����᷵��ERROR��NRΪ��ģʱ��ֻ�����·�NRģʽ��NETSCAN���
     *         Lģ����NV (NV_ID_SWITCH_PARA)���ơ�NVĬ�Ϲرա�
     * [�﷨]:
     *     [����]: ^NETSCAN=<n>,<pow>[,<mode>[,<band>]]
     *     [���]: [<CR><LF>^NETSCAN: [<arfcn>],[<c1>],[<c2>],[<lac>],[<mcc>],[<mnc>],[<bsic>], [<rxlev>][,<cid>,[<band>],[psc],[pci][,<5GSCS>, <5GRSRP>, <5GRSRQ>,<5GRSSNR>]]<CR><LF> [<CR><LF>^NETSCAN: [<arfcn>],[<c1>],[<c2>],[<lac>],[<mcc>],[<mnc>],[<bsic>], [<rxlev>][,<cid>,[<band>],[psc],[pci][,<5GSCS>, <5GRSRP>,<5GRSRQ>,<5GRSSNR>]]<CR><LF>[...]]] <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NETSCAN=?
     *     [���]: <CR><LF>^NETSCAN: (list of supported <n>s),(list of supported <pow>s),(list of supported <mode>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ��Ҫɨ����������޵�С��������ȡֵ��Χ[1,20]��
     *     <pow>: С����������ֵ��GU��ȡֵ��Χ[-110dBm,-47dBm]��LTE��NR��ȡֵ��Χ[-125dBm,-47dBm]��
     *     <mode>: ���뼼����
     *             0��GSM��ȱʡֵ����
     *             1��UMTS����FDD����
     *             2��LTE��
     *             3��LTE-NRDC����LTE��ģ��ʹ�ã�ɨ��NR���磩��
     *             4��NR����NR��ģ��ʹ�ã���
     *     <band>: ͬ^SYSCFGEX�����е�<band>��<lteband>������<mode>Ϊ3��4ʱ�˲�����ÿ��Bit��ӦNR��Band����Я����ɨ��ȫ��֧�ֵ�NRģʽBand��
     *     <arfcn>: Ƶ�㡣
     *     <C1>: ����GSMС����ѡ��C1ֵ���ݲ�֧�֣���
     *     <C2>: ����GSMС����ѡ��C2ֵ���ݲ�֧�֣���
     *     <lac>: λ�����룬16�������ֱ�ʾ��
     *     <mcc>: PLMN�����룬3λ���ֱ�ʾ�������й�PLMN�Ĺ������ʾΪ460��
     *     <mnc>: PLMN�����룬2λ��3λ���ֱ�ʾ��
     *     <bsic>: GSMС����վ�롣
     *     <rxlev>: ���յ����ź�ǿ�ȣ���λdBm��
     *     <cid>: С��ID��16������ʾ��
     *     <psc>: �����룬ʮ������ʾ��WCDMA��ʽ�²���ֵ��Ч����
     *     <pci>: ����С��ID��ʮ������ʾ��LTE��NR��ʽ�²���ֵ��Ч����
     *     <5GSCS>: NR���ز������Ϣ��5G SubcarrierSpacing information������ֵ��
     *             0��15kHz��
     *             1��30kHz��
     *             2��60kHz��
     *             3��120kHz��
     *             4��240khz��
     *     <5GRSRP>: NR�ο��źŽ��չ��ʣ�5G Reference Signal Received Power������ֵ����Чֵ99����λ��0.5dBm��
     *     <5GRSRQ>: NR�ο��źŽ���������5G Reference Signal Received Quality������ֵ����Чֵ99����λ��dB��
     *     <5GRSSNR>: ��ӦNR������SINR��5G Receiving signal strength in dbm������ֵ����Чֵ99����λ��dBm��
     * [ʾ��]:
     *     �� �������뼼����Ƶ��ɨ��
     *       AT^NETSCAN=2,-110
     *       ^NETSCAN: 15,,,4,460,09,5,-64,2,80,0,0
     *       ^NETSCAN: 31,,,6,460,09,2,-100,2,80,0,0
     *       OK
     *     �� �����뼼������Ƶ��ɨ��
     *       AT^NETSCAN=2,-110,0
     *       ^NETSCAN: 15,,,4,460,09,5,-64,2,80,0,0
     *       ^NETSCAN: 31,,,6,460,09,2,-100,2,80,0,0
     *       OK
     *       AT^NETSCAN=20,-100,2
     *       ^NETSCAN: 100,,,2540,460,11,0,-80,2632709,1,0,375
     *       ^NETSCAN: 42590,,,d,440,10,0,-94,d5a,20000000000,0,139
     *       ^NETSCAN: 1350,,,64,460,50,0,-94,0,4,0,0
     *       ^NETSCAN: 40886,,,64,460,50,0,-95,13,10000000000,0,19
     *       ^NETSCAN: 300,,,64,460,50,0,-100,1,1,0,1
     *       OK
     *     �� �����뼼����Ƶ��ɨ��
     *       AT^NETSCAN=2,-110,0,300
     *       ^NETSCAN: 15,,,4,460,09,-64,2,300,0,0
     *       ^NETSCAN: 31,,,6,460,09,-1000,2,100300,0,0
     *       OK
     *     �� LTE��ģʱɨ��NRģʽ
     *       AT^NETSCAN=3,-125,3
     *       ^NETSCAN: 633984,,,1,460,10,0,0,1088000,20000000000000000000,0,285,1,-95, -21,52
     *       OK
     *     �� ��������
     *       AT^NETSCAN=?
     *       ^NETSCAN: (1-20),(-125--47),(0-4)
     *       OK
     *     �� ��ȡƵ����Ϣ���
     *       AT^NETSCAN=2,-110,0,300
     *       ABCD(��ϣ����������ַ����)
     *       ^NETSCAN: 15,,,4,460,09,5,-64,2,300,0,0
     *       ^NETSCAN: 31,,,6,460,09,2,-100,2,300,0,0
     *       OK
     */
    { AT_CMD_NETSCAN,
      AT_SetNetScanPara, AT_NETSCAN_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestNetScanPara, AT_NOT_SET_TIME,
      AT_AbortNetScanPara, AT_ABORT_NETSCAN_SET_PARA_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NETSCAN", (VOS_UINT8 *)NETSCAN_CMD_PARA_STRING },

    /* ���ݸ���Ԥ����������AT����^EMRSSICFG */
    /*
     * [���]: Э��AT-�绰����
     * [����]: RSSIԤ���ϱ�������������
     * [˵��]: ��������������GUL��ʽ�£�^EMRSSIRPT�ϱ�����ֵ��������С��RSSIֵ�仯������ֵʱ���ϱ�^EMRSSIRPT��ָʾ��ǰRSSIֵ��
     * [�﷨]:
     *     [����]: ^EMRSSICFG=<rat>,<rssi_thresh>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^EMRSSICFG?
     *     [���]: <CR><LF>^EMRSSICFG: <rat>,<rssi_thresh>[<CR><LF>^EMRSSICFG: <rat>,<rssi_thresh>[��]]<CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^EMRSSICFG=?
     *     [���]: <CR><LF>^EMRSSICFG: (list of supported <rat>s), (list of supported <rssi_thresh>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <rat>: ����ֵ��������ʽ��
     *             0��GSM
     *             1��WCDMA
     *             2��LTE
     *     <rssi_thresh>: ����ֵ������С��RSSIֵ�仯��������ֵʱ���ϱ�^EMRSSIRPTָʾ��ǰRSSIֵ��ȡֵ��Χ��[0��5-70]����λdbm��
     * [ʾ��]:
     *     �� ����LTE����С��RSSI�ϱ�����
     *       AT^EMRSSICFG=2,10
     *       OK
     */
    { AT_CMD_EMRSSICFG,
      AT_SetEmRssiCfgPara, AT_SET_PARA_TIME, AT_QryEmRssiCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^EMRSSICFG", (VOS_UINT8 *)"(0-2),(0,5-70)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯ�ź���������
     * [˵��]: ���������ڷ����յ����ź�����������
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: +CESQ
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>+CESQ: <rxlev>,<ber>,<rscp>,<ecno>,<rsrq>,<rsrp><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ:
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +CESQ=?
     *     [���]: <CR><LF>+CESQ: (list of supported <rxlev>s),(list of supported<ber>s),(list of supported <rscp>s),(list of supported<ecno>s),(list of supported <rsrq>s),(list of supported <rsrp>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <rxlev>: ����ֵ���ź�ǿ��ˮƽ��
     *             ��Ч��Χ����0-63,99�� ����99Ϊ��Чֵ
     *     <ber>: ����ֵ���ŵ������ʡ�
     *             ��Ч��Χ����0-7,99������99Ϊ��Чֵ
     *     <rscp>: ����ֵ�������ź��빦�ʡ�
     *             ��Ч��Χ����0-96,255������255Ϊ��Чֵ
     *     <ecno>: ����ֵ��ÿ��PN��Ƭ�������빦�����ܶȵı�ֵ��
     *             ��Ч��Χ����0-49,255������255Ϊ��Чֵ
     *     <rsrq>: ����ֵ���ο��źŵĽ���������
     *             ��Ч��Χ����0-34,255������255Ϊ��Чֵ
     *     <rsrp>: ����ֵ���ο��źŵĹ��ʡ�
     *             ��Ч��Χ����0-97,255������255Ϊ��Чֵ
     * [ʾ��]:
     *     �� ��ѯCS��PS�������
     *       AT+CESQ
     *       +CESQ: 99,99,50,30,255,255
     *       OK
     *     �� ��������
     *       AT+CESQ=?
     *       +CESQ: (0-63,99),(0-7,99),(0-96,255),(0-49,255),(0-34,255),(0-97,255)
     *       OK
     */
    { AT_CMD_CESQ,
      At_SetCesqPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCesqPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"+CESQ", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯEONS��UCS2�����PLMN����������
     * [˵��]: ���������ڲ�ѯEONS��UCS2�����PLMN���������ơ�
     * [�﷨]:
     *     [����]: ^EONSUCS2?
     *     [���]: ִ�в�ѯ�ɹ�ʱ��
     *             <CR><LF>^EONSUCS2: <long_name>,<short_name><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��CSG����£�
     *             <CR><LF>^EONSUCS2: <home NodeB Name>< CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <long_name>: �ַ������͵�PLMN���������ƣ������ơ���UCS-2��ʽ���롣
     *     <short_name>: �ַ������͵�PLMN���������ƣ������ơ���UCS-2��ʽ���롣
     *     <home NodeB Name>: home NodeB Name����󳤶�48�ֽڡ�
     */
    { AT_CMD_EONSUCS2,
      VOS_NULL_PTR, AT_SET_PARA_TIME, At_QryEonsUcs2Para, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^EONSUCS2", VOS_NULL_PTR },

    /* NASӦ�ն�Ҫ������AT���� */
    /*
     * [���]: Э��AT-NDIS����
     * [����]: ��ѯ��Ӫ��
     * [˵��]: ���������ڲ�ѯ��Ӫ�̵�PLMN�������ͳ�����������<name>��ֵ��ѯ��Ӫ�̵���Ϣ��
     * [�﷨]:
     *     [����]: ^CRPN[=<name_index>,<name>]
     *     [���]: <CR><LF>^CRPN: <plmn>,<short name>,<full name>[<CR><LF>^CRPN: <plmn>,<short name>,<full name> [...]] <CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CRPN?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^CRPN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <name_index>: ����ֵ��ȡֵ���£�
     *             0���ڶ�������<name>Ϊ��Ӫ�̳�����
     *             1���ڶ�������<name>Ϊ��Ӫ�̶�����
     *             2���ڶ�������<name>Ϊ��Ӫ��PLMN��
     *     <name>: ���ݵ�һ������<name_index>�����ã�������Ӫ�̵�PLMN������������
     *     <plmn>: �ַ������ͣ���Ӫ�̵�PLMN��0��6byte��
     *     <short name>: �ַ������ͣ���Ӫ�̶�����
     *     <full name>: �ַ������ͣ���Ӫ�̳�����
     * [ʾ��]:
     *     �� ������Ӫ�̳�����ѯ��Ӫ����Ϣ
     *       AT^CRPN=0,"O2-CZ"
     *       ^CRPN: "23002","O2 - CZ","O2 - CZ"
     *       OK
     *     �� ������Ӫ�̶�����ѯ��Ӫ����Ϣ
     *       AT^CRPN=1,"O2-CZ"
     *       ^CRPN: "23002","O2 - CZ","O2 - CZ"
     *       OK
     *     �� ������Ӫ��PLMN��ѯ��Ӫ����Ϣ
     *       AT^CRPN=2,"23002"
     *       ^CRPN: "23002","O2 - CZ","O2 - CZ"
     *       OK
     *     �� ��ѯ����
     *       AT^CRPN?
     *       OK
     *     �� ��������
     *       AT^CRPN=?
     *       OK
     */
    { AT_CMD_CRPN,
      At_SetCrpnPara, AT_SET_PARA_TIME, At_QryCrpnPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CRPN", (VOS_UINT8 *)"(0-2),(PLMN)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ��ǰ�Ƿ���ҵ�����
     * [˵��]: ���������ڲ�ѯ��ǰ�Ƿ���CSҵ���PSҵ����ڣ�Ŀǰ�ݲ��ṩPSҵ��״̬�Ĳ�ѯ��ֻ�����ýӿڣ��ڶ��������̶����0������ʵ�����塣
     * [�﷨]:
     *     [����]: ^USERSRVSTATE?
     *     [���]: <CR><LF>^USERSRVSTATE: <n,m><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^USERSRVSTATE=?
     *     [���]: <CR><LF>^USERSRVSTATE: (list of supported <n>s),(list of supported <m>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ��ǰ�Ƿ���CSҵ����ڡ�
     *             0����CSҵ��
     *             1����CSҵ��
     *     <m>: ��ǰ�Ƿ���PSҵ����ڣ�ȡֵΪ0��1��
     *             ע�⣺Ŀǰ�ݲ��ṩPSҵ��״̬�Ĳ�ѯ��ֻ�����ýӿڣ�mȡֵ�̶�Ϊ0������ʵ�����塣
     * [ʾ��]:
     *     �� ��ѯ�Ƿ���ҵ�����
     *       AT^USERSRVSTATE?
     *       ^USERSRVSTATE: 1,0
     *       OK
     */
    { AT_CMD_USERSRVSTATE,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryUserSrvStatePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^USERSRVSTATE", (VOS_UINT8 *)"(0,1),(0,1)" },
};

/* ע��taf phone AT����� */
VOS_UINT32 AT_RegisterPhoneMmCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atPhoneMmCmdTbl, sizeof(g_atPhoneMmCmdTbl) / sizeof(g_atPhoneMmCmdTbl[0]));
}

