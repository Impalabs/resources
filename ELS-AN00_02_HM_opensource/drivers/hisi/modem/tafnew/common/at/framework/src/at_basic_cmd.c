/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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

#include "at_basic_cmd.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_test_para_cmd.h"

#if (FEATURE_LTE == FEATURE_ON)
#include "at_lte_common.h"
#endif

#include "at_general_drv_set_cmd_proc.h"
#include "at_voice_taf_set_cmd_proc.h"
#include "at_voice_taf_qry_cmd_proc.h"
#include "at_data_basic_set_cmd_proc.h"
#include "at_data_basic_qry_cmd_proc.h"
#include "at_general_taf_set_cmd_proc.h"
#include "at_general_taf_qry_cmd_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_BASICCMD_C

/*
 * ʾ��: ^CMDX �����ǲ���E5���뱣���������+CLAC�о���������ʱ����ʾ����һ�������ǲ���˫���ŵ��ַ���,
 *      �ڶ��������Ǵ�˫���ŵ��ַ����������������������Ͳ���
 * !!!!!!!!!!!ע��: param1��param2��ʾ����ʵ�ʶ�������ʱӦ��������ļ��(����߽���Ч��)!!!!!!!!!!!!!
 *  {AT_CMD_CMDX,
 *  At_SetCmdxPara, AT_SET_PARA_TIME, At_QryCmdxPara, AT_QRY_PARA_TIME, At_TestCmdxPara, AT_NOT_SET_TIME,
 *  AT_ERROR, CMD_TBL_E5_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
 *  (VOS_UINT8 *)"^CMDX", (VOS_UINT8 *)"(@param1),(param2),(0-255)"},
 */

static const AT_ParCmdElement g_atBasicCmdTbl[] = {
    /* CS */
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �Ҷϵ绰
     * [˵��]: �ڵ�ģʽ���μ�+CMOD����¶Ͽ���Զ���û������ӡ��෽����ʱ�����ӵ�ÿ���û����ᱻ�Ͽ���
     *         ���������ITU T Recommendation V.250Э�顣
     *         +CHUP�����H���������+CHUP������Ӧ���ڶ�ģʽ���У����ܴ���H���
     * [�﷨]:
     *     [����]: H[<value>]
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: ����ֵ����֧��ȡֵΪ0��
     *             ȡ����ֵʱ����Ӧ�����ΪERROR��
     *             ����������<value>�������书�ܺ�ATH0һ������ATH��ATH0�Ĺ���һ������
     * [ʾ��]:
     *     �� �Ҷϵ�ǰ���еĺ���
     *       ATH
     *       OK
     */
    { AT_CMD_H,
      At_SetHPara, AT_DETACT_PDP_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"H", (VOS_UINT8 *)"(0)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��������
     * [˵��]: ������ǰ�����硣
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: A
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��������ʧ�ܣ�
     *             <CR><LF>NO CARRIER<CR><LF>
     *             ��������
     *             <CR><LF> ERROR<CR><LF>
     *             ����
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� ����һ������
     *       ATA
     *       OK
     */
    { AT_CMD_A,
      At_SetAPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"A", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �������
     * [˵��]: ����һ�����У��ù��̿��Ա���ϣ���
     *         ���������ITU T Recommendation V.250Э�顣
     *         MTִ����TE���ⲿPDN��Packet Data Network���佨��ͨ��������Ĳ���������������״̬��
     *         ���������3GPP TS 27.007Э�顣
     *         ��֧��CUG����ҵ��
     *         ��֧��ME�洢���ʵĵ绰�������Բ�֧�ִ�ME�еĵ绰��������С�
     * [�﷨]:
     *     [����]: D[<dial_string>][I][G][;]
     *     [���]: �ú���<dial_string>����һ�����С�
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             �Է��û�æ
     *             <CR><LF>BUSY<CR><LF>
     *             �޷���������
     *             <CR><LF>NO CARRIER<CR><LF>
     *             ��������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: D>mem<n>[I][G][;]
     *     [���]: ��ָ���洢��mem�Ĵ洢λ��<n>��ȡ���к��룬���øú��뷢����У��洢������ͨ������+CPBS=?��ѯ��
     *     [����]: D><n>[I][G][;]
     *     [���]: �ӵ�ǰ�洢���Ĵ洢λ��<n>��ȡ���к��룬���øú��뷢����С�
     *     [����]: D><name>[I][G][;]
     *     [���]: ���ݵ绰���д洢������<name>��ȡ���к��룬���øú��뷢����С�
     *     [����]: D*<mmi_string>[#]
     *     [���]: ����MMI�ַ���mmi_string��
     *     [����]: D*<GPRS_SC>[*[<called_address>][*[<L2P>][*[<cid>]]]]#
     *     [���]: ���������ҵ��
     *             <CR><LF>CONNECT<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: D*<GPRS_SC_IP>[*<cid>]#
     *     [���]: ���������IPҵ��
     *             <CR><LF>CONNECT<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <I>: ��������д���I����ʾ����κ����н�ֹ��ʾ���к��룻
     *             ��������д���i����ʾ����κ�����������ʾ���к��롣
     *     <G>: ��������д���G����g����ʾ����κ�����ʹ��CUG����ҵ��
     *     <�ֺš�;��>: �������ĩβ�зֺţ���ʾ�����������У�
     *             ���û�зֺţ����ʾ�������ݺ��С�
     *     <dial_string>: ���к��롣
     *     <GPRS_SC>: GPRS������룬ֵΪ99��
     *     <called_address>: Ŀǰ���Ըò�����
     *     <L2P>: TE��MT֮��Ĳ��Э�����ͣ�֧������ȡֵ��Ĭ��ֵΪPPPЭ�顣
     *             NULL����ʹ�ò��Э�飻
     *             PPP�����ʹ��PPPЭ�顣
     *     <cid>: ����ֵ��PDP�����ı�ʶ���������������Ĭ��cid����һ��PDP��
     *     <GPRS_SC_IP>: GPRSΪIP�ṩ�ķ�����룬ֵΪ98��
     * [ʾ��]:
     *     �� �����������У�������κ����н�ֹ��ʾ���к���
     *       ATD13900000000I;
     *       OK
     *     �� �õ绰���ĵ�28���洢λ�ô洢�ĺ��뷢��һ����������
     *       ATD>28;
     *       OK
     *     �� ���������ҵ�񣬲��ŷ���PDP����
     *       ATD*99**PPP*1#
     *       CONNECT
     *     �� ���������IPҵ�񣬲��ŷ���PDP����
     *       ATD*98*1#
     *       CONNECT
     */
    { AT_CMD_D,
      At_SetDPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"D", VOS_NULL_PTR },

#if (FEATURE_AT_HSUART == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����ģʽ�л�������ģʽ
     * [˵��]: �����������л���������ģʽ������ģʽ��
     *         ֻ����MBBģ���Ʒ��
     * [�﷨]:
     *     [����]: O
     *     [���]: <CR><LF>CONNECT [<value >]<CR><LF>
     *             ���������������
     *             <CR><LF>NO CARRIER<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <value>: ����ֵ��
     *             0������ģʽ�л�������ģʽ��
     * [ʾ��]:
     *     �� ����ģʽ�л�������ģʽ
     *       ATO
     *       CONNECT 9600
     */
    { AT_CMD_O,
      AT_SetOPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"O", (VOS_UINT8 *)"(0)" },
#endif

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �����Զ�Ӧ��
     * [˵��]: �����Զ�Ӧ��
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: S0=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: S0?
     *     [���]: <CR><LF><n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: S0=?
     *     [���]: <CR><LF>S0: (list of supported <n>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ��Ĭ��ֵΪ0��
     *             0���ر��Զ�Ӧ��
     *             1��255�������Զ�Ӧ���ʱ�䣬��λΪs��
     * [ʾ��]:
     *     �� �����Զ�Ӧ��ʱ��Ϊ5s
     *       ATS0=5
     *       OK
     *     �� ��ѯ�Զ�Ӧ������
     *       ATS0?
     *       5
     *       OK
     *     �� ����S0
     *       ATS0=?
     *       S0: (0-255)
     *       OK
     */
    { AT_CMD_S0,
      At_SetS0Para, AT_NOT_SET_TIME, At_QryS0Para, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"S0", (VOS_UINT8 *)"(0-255)" },

    /* ATC */
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �����н�����
     * [˵��]: ���������н�����S3��
     *         ����S3��Ӧ��ASCII��ֵ�������á�
     *         ��TE��������ʾһ���������ֹ����MTʶ��ȷ�ϡ�
     *         ��MT���������빹�ɷ���ֵ�н�������Ӧ��Ϣ��ͷ����β���ͽ�����־��
     *         ��ʹ��S3=<value>����������S3ʱ����ʹ�õ�ǰ��S3�ַ�����Ϊ��������Ľ�����������������Ľ�����л�����ʹ�������õ�S3�ַ�������ȵ���һ�����
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: ATS3=[<value>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ATS3?
     *     [���]: <CR><LF><value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ATS3=?
     *     [���]: <CR><LF>S3: (list of supported <value>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: ����ֵ�������н�������Ĭ��ֵΪ13��
     *             0��127����ASCII��ֵ��ʽ����S3�ַ���
     * [ʾ��]:
     *     �� ����S3
     *       ATS3=13
     *       OK
     *     �� ��ѯS3
     *       ATS3?
     *       013
     *       OK
     *     �� ����S3
     *       ATS3=?
     *       S3: (0-127)
     *       OK
     */
    { AT_CMD_S3,
      At_SetS3Para, AT_NOT_SET_TIME, At_QryS3Para, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"S3", (VOS_UINT8 *)"(0-127)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �����л��з�
     * [˵��]: ���������л��з�S4���û�Ӧ������S4��Ӧ��ASCII��ֵ�������á����ַ�Ҳ����MT���������빹�ɷ���ֵ�н�������Ӧ��Ϣ��ͷ����β���ͽ�����־��
     *         �����һ�������иı���S4�ַ�����ô�ڴ�������Ľ�����н�����ʹ���µ�S4�ַ���������ȵ���һ�����
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: ATS4=[<value>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ATS4?
     *     [���]: <CR><LF><value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ATS4=?
     *     [���]: <CR><LF>S4: (list of supported <value>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: ����ֵ�������л��з���Ĭ��ֵΪ10��
     *             0��127����ASCII��ֵ��ʽ����S4�ַ���
     * [ʾ��]:
     *     �� ����S4
     *       ATS4=10
     *       OK
     *     �� ��ѯS4
     *       ATS4?
     *       010
     *       OK
     *     �� ����S4
     *       ATS4=?
     *       S4: (0-127)
     *       OK
     */
    { AT_CMD_S4,
      At_SetS4Para, AT_NOT_SET_TIME, At_QryS4Para, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"S4", (VOS_UINT8 *)"(0-127)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �������˸��
     * [˵��]: �����������˸��S5���û�Ӧ������S5��Ӧ��ASCII��ֵ�������á����ַ���TE��������ʾɾ��ǰһ���ַ�����MTʶ��ȷ�ϡ�
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: ATS5=[<value>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ATS5?
     *     [���]: <CR><LF><value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ATS5=?
     *     [���]: <CR><LF>S5: (list of supported <value>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: ����ֵ���������˸����Ĭ��ֵΪ8��
     *             0��127����ASCII��ֵ��ʽ����S5�ַ���
     * [ʾ��]:
     *     �� ����S5
     *       ATS5=8
     *       OK
     *     �� ��ѯS5
     *       ATS5?
     *       008
     *       OK
     *     �� ����S5
     *       ATS5=?
     *       S5: (0-127)
     *       OK
     */
    { AT_CMD_S5,
      At_SetS5Para, AT_NOT_SET_TIME, At_QryS5Para, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"S5", (VOS_UINT8 *)"(0-127)" },

    { AT_CMD_S6,
      At_SetS6Para, AT_NOT_SET_TIME, At_QryS6Para, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"S6", (VOS_UINT8 *)"(2-10)" },

    { AT_CMD_S7,
      At_SetS7Para, AT_NOT_SET_TIME, At_QryS7Para, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"S7", (VOS_UINT8 *)"(1-255)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �������
     * [˵��]: ����MT�Ƿ���ʹ�TE���յ��ַ���
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: E[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <n>: ����ֵ�����Կ��ƣ�Ĭ��ֵΪ1��
     *             0��MT�����ʹ�TE���յ��ַ���
     *             1��MT���ʹ�TE���յ��ַ���
     * [ʾ��]:
     *     �� ���û���
     *       ATE1
     *       OK
     *     �� �رջ���
     *       ATE0
     *       OK
     */
    { AT_CMD_E,
      At_SetEPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_E5_IS_LOCKED | CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"E", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ���÷�����Ϣ��ʽ
     * [˵��]: ����AT�����������Ӧ��Ϣ�ĸ�ʽ�����У�����������ֺ��ַ���������ʽ��
     *         ���������ITU T Recommendation V.250Э�顣
     *         ע1��<CR>��ʾS3�ַ���
     *         ע2��<LF>��ʾS4�ַ���
     * [�﷨]:
     *     [����]: V[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <n>: ����ֵ��������Ϣ��ʽ��Ĭ��ֵΪ1��
     *             0���������Ե�ͷ����β����������������ʽ�Ľ���룻
     *             1��������ȫ��ͷ����β�����������ַ�����ʽ�Ľ���롣
     * [ʾ��]:
     *     �� <n>Ϊ0ʱ
     *       ATV0
     *       0
     *     �� <n>Ϊ1ʱ
     *       ATV1
     *       OK
     *     �� ��������
     *       ATV=?
     *       ERROR
     */
    { AT_CMD_V,
      At_SetVPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"V", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯMS����ID��Ϣ
     * [˵��]: ��ѯMS�����ID��Ϣ��������Ϣ��+GMI������Ʒģ�ͺţ�+GMM��������汾�ţ�+GMR����ESN/IMEI��+GSN���������б�+GCAP����
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: I[<n>]
     *     [���]: <CR><LF><GMI>, <GMM>, <GMR>, <GSN>, <GCAP><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ����ѯ���ƣ�Ĭ��ֵΪ0��
     *             0��255����ѯMS����ID��Ϣ��
     *     <GMI>: ������Ϣ��
     *     <GMM>: ��Ʒģ�ͺš�
     *     <GMR>: ����汾�š�
     *     <GSN>: ESN/IMEI��
     *     <GCAP>: �����б�
     * [ʾ��]:
     *     �� ��ѯMS����ID��Ϣ
     *       ATI0
     *       Manufacturer: Huawei Technologies CO.,Ltd.
     *       Model: H8180
     *       Revision: TEMP_VERSION
     *       IMEI: 350447370464090
     *       +GCAP: +CGSM,+DS,+ES
     *       OK
     */
    { AT_CMD_I,
      At_SetMsIdInfo, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_E5_IS_LOCKED | CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"I", (VOS_UINT8 *)"(0-255)" },

    { AT_CMD_T,
      At_SetTPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (TAF_UINT8 *)"T", TAF_NULL_PTR },

    { AT_CMD_P,
      At_SetPPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"P", TAF_NULL_PTR },

    { AT_CMD_X,
      At_SetXPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"X", (VOS_UINT8 *)"(0-4)" },

    { AT_CMD_Z,
      At_SetZPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"Z", (VOS_UINT8 *)"(0)" },

    { AT_CMD_Q,
      At_SetQPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"Q", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����DCD�ź�ģʽ
     * [˵��]: ��������������DCD�ź�ģʽ��
     *         ������ֻ֧��HSUART�˿��·��������˿��·�ֱ�ӷ���OK��
     *         ֻ����MBBģ���Ʒ��
     * [�﷨]:
     *     [����]: &C[value]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <value>: ����ֵ
     *             0��DCD�ź�һֱ����ON
     *             1����⵽�������ز��ź�ʱ��DCD�źű���ON��û�м�⵽�ز��ź�ʱ��DCD�źű���OFF��
     * [ʾ��]:
     *     �� ����DCD�ź�ģʽ
     *       AT&C0
     *       OK
     */
    { AT_CMD_AMP_C,
      At_SetAmpCPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"&C", (VOS_UINT8 *)"(0-2)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����DTR�ź�ģʽ
     * [˵��]: ���������������ڶ˿ڴ�������ģʽʱDTR�źű仯��ON��OFFʱ��DCE����Ӧ����
     *         ������ֻ֧��HSUART�˿��·��������˿��·�ֱ�ӷ���OK��
     *         ֻ����MBBģ���Ʒ��
     * [�﷨]:
     *     [����]: &D[value]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <value>: ����ֵ��
     *             0��DCE����DTR�ܽ��źű仯��
     *             1��DCE�л�������ģʽ��
     *             2��DCE�ͷ��������ӡ�
     * [ʾ��]:
     *     �� ����DTR�ź�ģʽ
     *       AT&D2
     *       OK
     */
    { AT_CMD_AMP_D,
      At_SetAmpDPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"&D", (VOS_UINT8 *)"(0-2)" },

#if (FEATURE_AT_HSUART == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����DSR�ź�ģʽ
     * [˵��]: ��������������DSR�ź�ģʽ��
     *         ������ֻ֧��HSUART�˿��·��������˿��·�ֱ�ӷ���OK��
     *         ֻ����MBBģ���Ʒ��
     * [�﷨]:
     *     [����]: &S[value]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <value>: ����ֵ
     *             0��DSR�ź�һֱ����ON��
     *             1��ͨ����·����ʱ��DSR�źű���ON��
     * [ʾ��]:
     *     �� ����DSR�ź�ģʽ
     *       AT&S0
     *       OK
     */
    { AT_CMD_AMP_S,
      At_SetAmpSPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"&S", (VOS_UINT8 *)"(0-1)" },
#endif

    { AT_CMD_L,
      AT_SetLPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"L", (VOS_UINT8 *)"(0)" },

    { AT_CMD_M,
      AT_SetMPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"M", (VOS_UINT8 *)"(1)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �ָ���������
     * [˵��]: ���������ڻָ��������á����ڴ���APPͨ����E5�����翨��Ʒ���ָ��������ò����ɹ��󣬻���APPͨ�������ϱ�^FACTORY�ַ���֪ͨӦ�ûָ��������á�
     *         ���������ITU T Recommendation V.250Э�顣
     * [�﷨]:
     *     [����]: &F[<value>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <value>: ����ֵ��Ĭ��ֵΪ0��
     *             Stick��̬��ֵ0���ָ���Ƶ������
     *             E5��̬��ֵ0���ָ���Ƶ�������û�������
     * [ʾ��]:
     *     �� ��ԭ�ɳ�������
     *       AT&F
     *       0
     *       OK
     */
    { AT_CMD_AMP_F,
      atSetNVFactoryRestore, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"&F", (VOS_UINT8 *)"(0)" },
};

VOS_UINT32 At_RegisterBasicCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atBasicCmdTbl, sizeof(g_atBasicCmdTbl) / sizeof(g_atBasicCmdTbl[0]));
}

const AT_ParCmdElement* At_GetBasicCmdTable(VOS_VOID)
{
    return g_atBasicCmdTbl;
}

VOS_UINT32 At_GetBasicCmdNum(VOS_VOID)
{
    return sizeof(g_atBasicCmdTbl) / sizeof(g_atBasicCmdTbl[0]);
}

VOS_UINT32 At_CheckIfDCmdValidChar(VOS_UINT8 charValue)
{
    /*
     * LINUXϵͳ�µĲ��������ʽΪ:ATDT"*99#"���ȹ淶�Ĳ�������������ţ������Ų���
     * 27007��ITUT-V.250�淶�еĲ�������Ϸ��ַ����У���Ϊ�˱�֤LINUX���ųɹ�������
     * ����Ϊ��������ĺϷ��ַ�
     */
    const VOS_UINT8 dCmdValidChar[] = {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '#', '+', 'A',
        'B', 'C', ',', 'T', 'P', '!', 'W', '@', '>', 'I', 'i', 'G', 'g', '"'
    };

    VOS_UINT32 indexNum = 0;

    while (indexNum < sizeof(dCmdValidChar)) {
        if (charValue == dCmdValidChar[indexNum]) {
            return AT_SUCCESS;
        }

        indexNum++;
    }

    return AT_FAILURE;
}

VOS_UINT8* At_GetFirstBasicCmdAddr(VOS_UINT8 *data, VOS_UINT32 *len)
{
    VOS_UINT32 i           = 0;
    VOS_UINT32 j           = 0;
    VOS_UINT8 *addr        = NULL;
    VOS_UINT8 *curAddr     = NULL;
    VOS_UINT32 dCmdLen     = 0;
    VOS_UINT32 basicCmdNum = sizeof(g_atBasicCmdTbl) / sizeof(AT_ParCmdElement);

    for (i = 0; i < basicCmdNum; i++) {
        if (g_atBasicCmdTbl[i].cmdName == NULL) {
            break;
        }

        curAddr = (VOS_UINT8 *)strstr((VOS_CHAR *)data, (VOS_CHAR *)(g_atBasicCmdTbl[i].cmdName));
        if (curAddr != NULL) {
            if ((addr == NULL) || (curAddr < addr)) {
                if (g_atBasicCmdTbl[i].cmdIndex == AT_CMD_D) { /* D�����п����������ַ�����Ҫ���⴦�� */
                    dCmdLen = strlen((VOS_CHAR *)curAddr);

                    /* ���μ��D����������Ч�ַ���1��ʾ'D'�ַ������ */
                    for (j = 1; j < dCmdLen; j++) {
                        if (At_CheckIfDCmdValidChar(*(curAddr + j)) != AT_SUCCESS) {
                            /* ������D����Ĳ����ַ����˳� */
                            break;
                        }
                    }

                    *len = j;
                } else {
                    *len = strlen((VOS_CHAR *)(g_atBasicCmdTbl[i].cmdName));
                }

                addr = curAddr;
            }
        }
    }

    return addr;
}

