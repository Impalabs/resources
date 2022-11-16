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
#include "at_test_as_cmd_tbl.h"
#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_test_as_set_cmd_proc.h"
#include "at_test_as_qry_cmd_proc.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_TEST_AS_CMD_TBL_C

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
static const AT_ParCmdElement g_atTestAsCmdTbl[] = {
    /*
     * [���]: Э��AT-������˽��
     * [����]: ����GSM
     * [˵��]: ������ֻ��GSM����Ч��
     *         ����������ָ������Ƶ�㡢��Ƶ�㣬��ѯGSM״̬�����ԡ�
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^CGAS=0
     *     [���]: <CR><LF><GAS STATUS><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CGAS=1
     *     [���]: <CR><LF><cell1>:<Arfcn>,<Bsic>,<Rxlev>
     *             <CR><LF><cell2>:<Arfcn>,<Bsic>,<Rxlev>
     *             <CR><LF><cell3>:<Arfcn>,<Bsic>,<Rxlev>
     *             <CR><LF><cell4>:<Arfcn>,<Bsic>,<Rxlev>
     *             <CR><LF><cell5>:<Arfcn>,<Bsic>,<Rxlev>
     *             <CR><LF><cell6>:<Arfcn>,<Bsic>,<Rxlev>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CGAS=2
     *     [���]: <CR><LF><Arfcn>,<Bsic>,<Rxlev><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CGAS=5/6
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^CGAS=7,<Freq>
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^CGAS=8,<arfcn>,<band>,<bsic>,<barTimeLen>
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF> ERROR<CR><LF>
     *     [����]: ^CGAS=9,<arfcn>,<band>,<bsic>
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF> ERROR <CR><LF>
     *     [����]: ^CGAS=10
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^CGAS=11,<enable>
     *     [���]: <CR><LF> enable <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF> ERROR <CR><LF>
     * [����]:
     *     <GAS STATUS>: ����ֵ��GSM�ڲ�״̬��
     *             1������̬��
     *             2��ר��̬��
     *             3������̬��
     *     <Arfcn>: ����ֵ������Ƶ��š�
     *     <Bsic>: ʮ������ֵ��BSIC�š�
     *     <Rxlev>: ����ֵ���źŲ���ֵ��
     *     <Freq>: ʮ���ƣ�����Ƶ����ʾ��Ƶ��š�������ΪƵ����Ƶ��֮�͡�
     *             0x0000��850MƵ������GSM850��
     *             0x1000��900MƵ������EGSM900��PGSM900��RGSM900��
     *             0x2000��1800MƵ������GSM DCS��
     *             0x3000��1900MƵ������GSM PCS 1900��
     *     <band>: ����ֵ��Ƶ��ֵ:
     *             0: GSM 850Ƶ��
     *             1: GSM 900 Ƶ��
     *             2: GSM 1800Ƶ��
     *             3: GSM 1900Ƶ��
     *     <barTimeLen>: ����ֵ��barʱ����
     *     <enable>: ����ֵ��ȡֵ0��1��
     *             0����DSDS��������һ��modem֪ͨ��modem�˳���Ϸģʽ��
     *             1����DSDS��������һ��modem֪ͨ��modem������Ϸģʽ��
     *             ����ֵ��Ч��
     * [ʾ��]:
     *       ��ѯGSMЭ��״̬
     *     �� ������GSM��ǰ����IDLE״̬
     *       AT^CGAS=0
     *       1
     *       OK
     *       ��ѯGSM����״̬
     *     �� ����1����ǰ������������Ƶ��ֱ�Ϊ80��85��BSIC��Ϊ0x22��0x2A��������ƽֵΪ-85��-90
     *       AT^CGAS=1
     *       1:80,0x22,-85
     *       2:85,0x2A,-90
     *       OK
     *     �� ����2����ǰû������
     *       AT^CGAS=1
     *       0
     *       OK
     *       ��С��δͬ���ϣ���BSIC����ʾΪ0xFF��
     *       ��ѯGSM����С��״̬
     *     �� ����1����ǰ����С����Ƶ��ֱ�Ϊ80��BSIC��Ϊ0x22��������ƽֵΪ-85
     *       AT^CGAS=2
     *       80,0x22,-85
     *       OK
     *     �� ����Ƶ��
     *       AT^CGAS=5
     *       OK
     *       �����������ǰ������Ч����ֹ������ѡ/��������ѡ��
     *       ����1����ǰΪ�ػ�״̬������AT^CGAS=5������פ��֮�󣬽�ֹ������ѡ��������Ŀǰ���ڿ���֮ǰʹ����Ч��
     *     �� ֪ͨ��modem������Ϸģʽ
     *       AT^CGAS=11,1
     *       1
     *       OK
     *     �� ֪ͨ��modem�˳���Ϸģʽ
     *       AT^CGAS=11,0
     *       0
     *       OK
     *       Ƶ�����
     *     �� ����1����ǰ�Ѿ�����Ƶ�㣬������ѡ����ֹ��ʹ�ø����������������������ѡ
     *       AT^CGAS=6
     *       OK
     *       ָ������Ƶ��
     *     �� ����1��ָ��MS��������פ��900MƵ�ε�114Ƶ�㣬�û������<Freq>ӦΪƵ��0x1000��114Ƶ��ĺ�4210��
     *       AT^CGAS=7,4210
     *       OK
     *     �� ����2��ָ��MS��������פ��1800MƵ�ε�521Ƶ�㣬�û������<Freq>ӦΪƵ��0x2000��521Ƶ��ĺ�8713��
     *       AT^CGAS=7,8713
     *       OK
     *       ����GSM FORBIDС��
     *     �� ����1������GSM FORBIDС����С����Ƶ����80��Ƶ����900Ƶ�Σ�BSICΪ0x22��С�������ͷ�1000��
     *       AT^CGAS=8,80,1,0x22,1000
     *       OK
     *       ɾ��GSM FORBIDС��
     *     �� ����1��ɾ��GSM FORBIDС����С����Ƶ����80��Ƶ����900Ƶ�Σ�BSICΪ0x22
     *       AT^CGAS=9,80,1,0x22
     *       OK
     *     �� ɾ��GС��ʵ�����ʷϵͳ��Ϣ��פ��G��ʱ������Ч
     *       AT^CGAS=10
     *       OK
     */
    { AT_CMD_CGAS,
      At_SetCGasPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CGAS", (VOS_UINT8 *)"(0-65535),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-������˽��
     * [����]: ����WAS����
     * [˵��]: ������ֻ��WCDMA����Ч��
     *         ^FREQLOCK/^WFREQLOCK/^CWAS����ͬʱʹ�á�
     *         AT^CWAS=4/5��AT^CWAS=8/9����ͬʱʹ�á�
     *         ������������ѯ������W��ز�����
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^CWAS=<n>
     *     [���]: <CR><LF>result<CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^CWAS=<n>,<enable>,<freq>,<scrcode>
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��ȡֵ0��9��
     *             0����ӡ��ǰС����Ƶ����Ϣ������������ʧ��ʱ����ӡerror��
     *             1��DCH״̬�£���ӡ���л��С�������롣
     *             ��DCH״̬�£���ӡ��ǰפ��С�������롣
     *             2����ǰЭ��ջ������Э��״̬���ܹ���⵱ǰ�Ƿ���ʧ��״̬���Ƿ����������result����Ϊ0-DCH̬��1-FACH̬��2-PCH̬��3-URA-PCH̬��4-IDLE̬��5-BUTT����ǰ�漸��״̬֮���״̬������δפ����������δ�����ȣ���
     *             3��Reserved���ù���Ŀǰ��ʹ�á�
     *             4����Ƶ��
     *             5����Ƶ������롣
     *             6��Reserved���ù���Ŀǰ��ʹ�á�
     *             7����ӡֵ����ΪCS��PS��UTRAN��DrxCycLenCoeff����ЧֵΪ0��
     *             8��·����Ƶ���ܣ���ֹ��Ƶ����ϵͳ��ѡ���л����ض���CCO��ǿ����Ƶ�л���Ŀ��Ƶ�㡣
     *             9��·����Ƶ�������빦�ܣ���ֹͬƵ����Ƶ����ϵͳ��ѡ���л����ض���CCO��ǿ��ͬƵ����Ƶ�л���Ŀ��С����
     *             10����DSDS��������һ��modem֪ͨ��modem��������˳���Ϸģʽ��
     *             �������޺��壬����error��
     *     <enable>: ����ֵ��ȡֵ0��1��
     *             0����������֪ͨ��modem�˳���Ϸģʽ��
     *             1����������֪ͨ��modem������Ϸģʽ��
     *             ����ֵ��Ч��
     *     <freq>: ����ֵ����ʾ��ǰ������Ƶ��ֵ��
     *             enable=0ʱfreq���Բ��
     *     <scrcode>: ����ֵ����ʾ��ǰ����������ֵ��
     *             enable=0ʱscrcode���Բ��
     * [ʾ��]:
     *     �� ��ѯ��ǰС������
     *       AT^CWAS=1
     *       48
     *       OK
     *     �� ��ѯ��ǰRRC״̬
     *       AT^CWAS=2
     *       4
     *       OK
     *       ����ֵ4��ʾIDLE״̬��
     *     �� ֪ͨ��modem������Ϸģʽ
     *       AT^CWAS=10,1
     *       1
     *       OK
     *     �� ֪ͨ��modem�˳���Ϸģʽ
     *       AT^CWAS=10,0
     *       0
     *       OK
     */
    { AT_CMD_CWAS,
      At_SetCwasPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CWAS", (VOS_UINT8 *)"(0-255),(0-65535),(0-65535),(0-65535)" },

    /*
     * [���]: Э��AT-������˽��
     * [����]: ��ѯWCDMAģʽDCH�����10�β�������
     * [˵��]: ������������ѯDCH�����10�β������档
     *         �ϱ����¼�Ϊ1ϵ�У�2ϵ�У�3ϵ�У������¼����ϱ������1ϵ���¼������С�����롣
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     *         ������ֻ��WCDMA����Ч��
     * [�﷨]:
     *     [����]: ^MEANRPT
     *     [���]: <CR><LF>cellinfo<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     * [��]: Event Number��Э����Event ID�Ķ�Ӧ��ϵ
     *       Event Number, ��ź��壨��ο�Э��25331��Event ID��,
     *       0x0,          1A,
     *       0x1,          1B,
     *       0x2,          1C,
     *       0x3,          1D,
     *       0x4,          1E,
     *       0x5,          1F,
     *       0x100,        2A,
     *       0x101,        2B,
     *       0x102,        2C,
     *       0x103,        2D,
     *       0x105,        2F,
     *       0x200,        3A,
     * [ʾ��]:
     *     �� ��ѯDCH�����10�β�������
     *       AT^MEANRPT
     *       2
     *       0x103,0
     *       0x103,0
     *       OK
     */
    { AT_CMD_MEANRPT,
      At_GetMeanRpt, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^MEANRPT", VOS_NULL_PTR },

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-������˽��
     * [����]: NR����Ƶ��С������
     * [˵��]: ���������ڲ�ѯ�����á�ȡ��NR����Ƶ���ܡ�
     *         ��������ڵ���֧��NRʱ����ʹ�ã�����ǰ����֧��NR��������
     *         ENDC������֧��NR��Ƶ������
     * [�﷨]:
     *     [����]: ^NRFREQLOCK=<enable>[,<scstype>,<band>[,<arfcn>[,<cellid>]]]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NRFREQLOCK?
     *     [���]: <CR><LF>^NRFREQLOCK: <enable> [,<scstype>,<band>,<freq>,<cellid>]<CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: ^NRFREQLOCK=?
     *     [���]: <CR><LF>^NRFREQLOCK: (list of supported <enable>s),(list of supported <scstype>s),(list of supported <band>s),(list of supported <arfcn>s),(list of supported <cellid>s)<CR><LF> <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ����ʶ��Ƶ�����Ƿ����ã�
     *             0���ر���Ƶ���ܣ��·��ر���Ƶ����ʱ����Ҫ�ٴ�����������
     *             1����������Ƶ�㹦�ܣ�
     *             2����������С�����ܣ�
     *             3����������Band���ܡ�
     *             ע�⣺�����ǰ�Ѿ�����enable״̬��������Ƶ���ͣ���Ҫͨ�����·�0���ԭ��ģʽ���ٴ��·���Ƶ��С������
     *     <scstype>: Ƶ���scstype��Ϣ��3�������������øò�����
     *             ����ֵ����Чֵ���£�
     *             0��SCS_TYPE_COMM_15��
     *             1��SCS_TYPE_COMM_30��
     *             2��SCS_TYPE_COMM_60��
     *             3��SCS_TYPE_COMM_120��
     *             4��SCS_TYPE_COMM_240��
     *     <band>: ����ֵ����Ƶ��Ƶ�Σ���������Ƶ���ظ���Ƶ�Σ���Band����UE����֧�ֵķ�Χ��3�������������øò�����ȡֵ��Χ0��65535��
     *     <arfcn>: ����ֵ����ǰ������Ƶ�㣬ȡֵ��Χ0��4294967295��
     *             ��<enable>ֵΪ1����2ʱ�������øò�����ֵΪ3ʱ�������øò�������Ƶ������ΪС����Ƶ����Ϣ������Ҫ������Ƶ����Ϣ�����Ҵ���Band֧�ֵķ�Χ֮�ڡ�
     *     <cellid>: ��Ҫ������С��ID��<enable>ֵΪ2ʱ�������øò�����ֵΪ1��3ʱ�������øò���������ֵ��ȡֵ��Χ0~1007��
     * [ʾ��]:
     *     �� ����NRС��Cell IDΪ374
     *       AT^NRFREQLOCK=2,1,77,633984,374
     *       OK
     *     �� ����NR bandΪ77
     *       AT^NRFREQLOCK=3,1,77
     *       OK
     *     �� ����NR Ƶ��Ϊ633984
     *       AT^NRFREQLOCK=1,1,77,633984
     *       OK
     *     �� �ر���Ƶ����
     *       AT^NRFREQLOCK=0
     *       OK
     *     �� ִ�в�������
     *       AT^NRFREQLOCK=?
     *       ^NRFREQLOCK: (0-3),(0-4),(0-65535),(0-4294967295),(0-1007)
     *       OK
     */
    { AT_CMD_NRFREQLOCK,
      At_SetNrFreqLockPara, AT_SET_PARA_TIME, AT_QryNrFreqLockPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NRFREQLOCK", (VOS_UINT8 *)"(0-3),(0-4),(0-65535),(0-4294967295),(0-1007)" },
#endif
};

/* ע��AS����������AT����� */
VOS_UINT32 AT_RegisterTestAsCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atTestAsCmdTbl, sizeof(g_atTestAsCmdTbl) / sizeof(g_atTestAsCmdTbl[0]));
}
#endif
