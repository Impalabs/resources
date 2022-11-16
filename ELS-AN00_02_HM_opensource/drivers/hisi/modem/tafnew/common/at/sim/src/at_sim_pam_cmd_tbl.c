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
#include "at_sim_pam_cmd_tbl.h"
#include "AtParse.h"
#include "at_parse_cmd.h"
#include "at_sim_pam_set_cmd_proc.h"
#include "at_sim_pam_qry_cmd_proc.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_SIM_PAM_CMD_TBL_C

static const AT_ParCmdElement g_atSimPamCmdTbl[] = {
    /* PHONEBOOK */
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ȡ�绰��
     * [˵��]: ��ȡ��ǰ�洢����λ����<index1>��<index2>֮�䣨�����䣩�ĵ绰����¼��
     *         ���ֻ����<index1>����ֻ������<index1>λ�õĵ绰����¼��
     *         ��������ص�ǰ�绰���洢��֧�ֵ�<number>��<text>����󳤶ȡ�
     *         �������Ƕԡ�AT+CPBR���������չ��
     *         ע�⣺����index1-index2��ֵ������200���������ʳ��ֲ�ѯ������ز�ȫ���⣬ԭ����USB����Ϊ256�������һ�β�ѯ���࣬PC��������������USB��������л��棬�������������᷵�ط���ʧ�ܣ������ϱ�ͨ��û�з�ѹ���ƣ�ֻ�ܶ�����Ϣ�������ϱ������ȫ��
     * [�﷨]:
     *     [����]: ^CPBR=<index1>[,<index2>]
     *     [���]: <CR><LF> [^CPBR: <index1>,<number>,<type>,<text>,<coding>[<CR><LF>^CPBR: <index2>,<number>,<type>,<text>,<coding>]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CPBR=?
     *     [���]: <CR><LF>^CPBR: (list of supported <index>s),[<nlength>],[<tlength>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <index1>\n<index2>: ����ֵ���绰�����ڴ洢���е�λ�á�
     *             <index1>��<index2>ȡֵ�����㲢��С�ڵ���+CPBS?����ص�<total>��
     *             <index1>С�ڵ���<index2>��
     *             ˵��
     *             ���绰����ʵ�ʴ洢�ĵ绰������С����<index1>��<index2>ȷ��������ʱ������ʵ�ʴ洢�ĵ绰����¼����ʾ��
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <text>: �ַ������ͣ�������
     *     <nlength>: ����ֵ���绰�������󳤶ȡ�
     *     <tlength>: ����ֵ����������󳤶ȡ�
     *     <coding>: ���뷽ʽ����ʾ<text>�ֶε��ַ����벢ָ�����ԡ�
     *             0��GSM 7 bit Default Alphabet�����Է�ѹ����7bitֵ�ϴ�����
     *             1��RAW mode������<text>��ԭʼ���ݸ�ʽ�ϴ��������ݵ�ͷ����UNICODE���뷽ʽ��
     * [ʾ��]:
     *     �� ��ȡ��¼��AT��CPBS����Ϊ��SM����
     *       AT^CPBR=1,5
     *       ^CPBR: 1,"12345678",129,"reinhardt",0
     *       ^CPBR: 2,"7654321",129,"sophia",0
     *       ^CPBR: 3,"111111111",129,"David",0
     *       ^CPBR: 4,"68644808",129,"80534E4E3AFF",1
     *       ^CPBR: 5,"+8613902934081",145," 82038363800031FF",1
     *       OK
     *     �� ��ȡ��¼��AT+CPBS����Ϊ��ON����
     *       AT^CPBR=1
     *       ^CPBR: 1,"+8612345678",145,"reinhardt",0
     *       OK
     *     �� ��ѯλ�÷�Χ���������������󳤶�
     *       AT^CPBR=?
     *       ^CPBR: (1-65535),20,100
     *       OK
     */
    { AT_CMD_CPBR,
      At_SetCpbrPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, AT_TestCpbrPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^CPBR", (TAF_UINT8 *)"(1-65535),(1-65535)" },
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: д��绰��
     * [˵��]: �ڵ�ǰ�洢������ӡ��޸ġ�ɾ��һ���绰����¼��
     *         ������������ֻ����index����index��Ӧλ�õĵ绰����Ŀ����ɾ����
     *         ���index�ֶα�ʡ�ԣ��������к���number�ֶΣ��������绰����Ŀ����д����һ����λ�á������ʱû�п�λ�ã����ϱ���+CME ERROR: memory full��������ѡ�绰������Ϊ��SM���͡�ON���е�һ��ʱ�ɽ���д������������������ִ��д��������AT�����з��ز���������Ĵ�����ʾ��
     *         ��������ص�ǰ�洢����<number>��<text>����󳤶ȡ�
     *         �������Ƕԡ�AT+CPBW������չ��
     * [�﷨]:
     *     [����]: ^CPBW=[<index>][,<number>[,<type>[,<text>,<coding>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CPBW=?
     *     [���]: <CR><LF>^CPBW: (list of supported <index>s),[<nlength>],(list of supported <type>s),[<tlength>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <text>: �ַ������ͣ�������
     *     <index>: ����ֵ���绰�����ڴ洢���е�λ�á�
     *     <number>: �绰���룬�ַ������ͣ��Ϸ��ַ�Ϊ����0������9������#������*������+������(������)������-������
     *             ��(������)���͡�-�������ַ����۳������κ�λ�ã�������򶼺������ǣ�������Ϊ�Ƿ��ַ�������+��ֻ�ܳ����ں������ʼλ�á�
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <coding>: ����ֵ����ʾ<text>�ֶε��ַ����뷽ʽ��ָ�����ԡ�
     *             0����IRA����ASCII�룻
     *             1����UCS2����UNICODE�룻
     *     <nlength>: ����ֵ���绰�������󳤶ȡ�
     *     <tlength>: ����ֵ����������󳤶ȡ�
     * [ʾ��]:
     *     �� �޸�һ����¼
     *       AT^CPBW=3,"12345678",129,"reinhardt",0
     *       OK
     *       AT^CPBW=1,��28780808��,129,��80534E4E3A��,1
     *       OK
     *     �� ���һ����¼
     *       AT^CPBW=,"12345678",129,"reinhardt",0
     *       OK
     *     �� ɾ��һ����¼
     *       AT^CPBW=7
     *       OK
     *     �� ��ѯ�洢��λ�÷�Χ��������󳤶ȣ��������ͺ�������󳤶�
     *       AT^CPBW=?
     *       ^CPBW: (1-65535),20,(0-255),100
     *       OK
     */
    { AT_CMD_CPBW,
      At_SetCpbwPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, AT_TestCpbrPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^CPBW", (TAF_UINT8 *)"(1-65535),(number),(0-255),(text),(0,1)" },
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: д��绰��
     * [˵��]: ������ֻ�����ڶ��ư汾��ֻ����NV�����ú����ʹ�á����߰汾��֧�ִ����
     *         ���������ڸ�����չ�绰������ͨ�绰�������������ֵ绰�������嶼����ܹ�д�����Ϣд�뵽����ָ���ļ��У����ڲ���д������ݽ�������֮�����д��������AT���������
     *         ������绰����Ŀд�뵱ǰ�Ѿ�ѡ�е绰���ڴ���indexָ����λ�á�
     *         ������������ֻ����index����indexλ�õĵ绰����Ŀ����ɾ�������index�ֶα�ʡ�ԣ��������к���num1~num4�ֶΣ��������绰����Ŀ����д����һ����λ�á������ʱû�п�λ�ã����ϱ���+CME ERROR: memory full��
     * [�﷨]:
     *     [����]: ^SCPBW=[<index>][,<num1>[,<type>[,<num2>[,<type>[,<num3>[,<type>[,<num4>[,<type>[,<text>,<coding>[,email]]]]]]]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SCPBW=?
     *     [���]: <CR><LF>^SCPBW: (list of supported <index>s), [<nlength>],(list of supported <type>s),[<tlength>], [<mlength>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <text>: �ַ������ͣ�������
     *     <index>: ����ֵ���绰�����ڴ洢���е�λ�á�
     *     <num1>\n<num2>\n<num3>\n<num4>: �绰���룬�ַ������ͣ��Ϸ��ַ�Ϊ����0������9������#������*������+������(������)������-������
     *             ��(������)���͡�-�������ַ����۳������κ�λ�ã�������򶼺������ǣ�������Ϊ�Ƿ��ַ�������+��ֻ�ܳ����ں������ʼλ�á�
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <coding>: ����ֵ��<text>�ֶε��ַ����뷽ʽ��ָ�����ԡ�
     *             0����IRA����ASCII�룻
     *             1����UCS2����UNICODE�룻
     *     <email>: �ַ������ͣ���ʾemail��ַ�����ֶ�ֻ�ڴ洢����ΪUSIM��ʱ��Ч
     *     <tlength>: ����ֵ����������󳤶ȡ�
     *     <nlenght>: ����ֵ���������󳤶ȡ�
     *     <mlength>: ����ֵ��Email������ַ�����
     * [ʾ��]:
     *     �� д��һ����¼
     *       AT^SCPBW=1,"28780808",129,"",129,"",129,"",129,"80534E4E3A",1,"HW@huawei.com"
     *       OK
     *       ע�⣺���ݽ���1���ü�¼����USIM���е�λ�ã�,��28780808�����绰���룩��129���������ͣ�����80534E4E3A������������Ϊ����1�����������ʽUCS2������HW@huawei.com����email��ַ����
     */
    { AT_CMD_SCPBW,
      At_SetScpbwPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, AT_TestCpbrPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^SCPBW", (TAF_UINT8 *)"(1-65535),(numa),(0-255),(numb),(0-255),(numc),(0-255),(numd),(0-255),(text),(0,1),(email)" },
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ȡ�绰��
     * [˵��]: ������ֻ�����ڶ��ư汾��ֻ����NV�����ú����ʹ�á����߰汾��֧�ִ����
     *         ����index1-index2��ֵ������200���������ʳ��ֲ�ѯ������ز�ȫ���⣬ԭ����USB����Ϊ256�������һ�β�ѯ���࣬PC��������������USB��������л��棬�������������᷵�ط���ʧ�ܣ������ϱ�ͨ��û�з�ѹ���ƣ�ֻ�ܶ�����Ϣ�������ϱ������ȫ��
     *         ������ص�ǰ�Ѿ�ѡ�еĵ绰���ڴ���λ��index1��index2֮��ĵ绰����Ŀ�����index1��index2֮�����е�λ�ö�û�е绰����Ŀ�������أ�+CME ERROR: not found��
     *         Ҳ����ֻ����index1����ʱֻ�᷵��index1λ�õĵ绰����¼����index1���޼�¼��Ҳ�᷵��+CME ERROR: not found��
     *         MT��SIM���б��������͸����TE��
     *         test����ص�ǰ�Ѿ�ѡ�еĵ绰���ڴ��λ�÷�Χ���Լ�<number>��<text>��<email>����󳤶ȡ�
     *         ��������ֶ�Ҫ�������й��ƶ�����USIM�绰��vCard�����ֶ�Ҫ��涨��
     * [�﷨]:
     *     [����]: ^SCPBR=<index1>
     *             [,<index2>]
     *     [���]: [<CR><LF>^SCPBR: <index1>,<num1>,<type>,<num2>,<type>, <num3>,<type>,<num4>,<type>,<text>,<coding>[,<email>][[...]
     *             <CR><LF>^SCPBR: <index2>,<num1>,<type>,<num2>,<type>, <num3>,<type>,<num4>,<type>,<text>],<coding>[,<email>]]<CR><LF>]
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CSPBR=?
     *     [���]: <CR><LF>^SCPBR: (list of supported <index>s),[<nlength>], [<tlength>],[<mlenth>]<CR><LF><CR><LF>OK<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <text>: �ַ������ͣ�������
     *     <index1>��\n<index2>: ����ֵ���绰�����ڴ洢���е�λ�á�
     *     <num1>��\n<num2>��\n<num3>��\n<num4>: �绰���룬�ַ������ͣ��Ϸ��ַ�Ϊ����0������9������#������*������+������(������)������-������
     *             ��(������)���͡�-�������ַ����۳������κ�λ�ã�������򶼺������ǣ�������Ϊ�Ƿ��ַ�������+��ֻ�ܳ����ں������ʼλ�á�
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <coding>: ����ֵ�����뷽ʽ����ʾ<text>�ֶε��ַ����벢ָ�����ԣ�ȡֵ0��1��
     *             0��GSM 7 bit Default Alphabet�����Է�ѹ����7bitֵ�ϴ�����
     *             1��RAW mode������<text>��ԭʼ���ݸ�ʽ�ϴ��������ݵ�ͷ����UNICODE���뷽ʽ��
     *     <email>: �ַ������ͣ���ʾemail��ַ��Ŀǰ�����ֶ�ֻ�ڴ洢����ΪUSIM��ʱ��Ч
     *     <tlength>: ����ֵ����������󳤶ȡ�
     *     <nlenght>: ����ֵ���������󳤶ȡ�
     *     <mlength>: ����ֵ��Email������ַ�����
     * [ʾ��]:
     *     �� ��ȡһ����¼
     *       AT^SCPBR=1
     *       ^CPBR: 1,"13902934081",129,"",129,"",129,"",129,"80534E4E3A",1,"HW@huawei.com"
     *       OK
     *       ע�����ݽ��� 1���ü�¼���ڴ洢���е�λ�ã�����13902934081�����绰���룩��129���������ͣ�����80534E4E3A������������Ϊ����1�����������ʽUCS2������HW@huawei.com����email ��ַ����
     */
    { AT_CMD_SCPBR,
      At_SetScpbrPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, AT_TestCpbrPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^SCPBR", (TAF_UINT8 *)"(1-65535),(1-65535)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ����(U)SIM״̬�仯�ϱ�
     * [˵��]: ���Ƶ�(U)SIM״̬�����仯ʱ��MT�Ƿ������ϱ���TE��
     * [�﷨]:
     *     [����]: ^SIMST=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SIMST=?
     *     [���]: <CR><LF>^SIMST: (list of supported <n>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: �����ͣ��Ƿ������ϱ�(U)SIM״̬��Ĭ��ֵ1��
     *             0����ֹ�����ϱ�(U)SIM״̬��
     *             1�����������ϱ�(U)SIM״̬��
     * [ʾ��]:
     *     �� ��ֹ�����ϱ�(U)SIM״̬
     *       AT^SIMST=0
     *       OK
     *     �� ���������ϱ�(U)SIM״̬
     *       AT^SIMST=1
     *       OK
     */
    { AT_CMD_SIMST,
      At_SetSimstPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMST", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: SIM��״̬�ϱ�ģʽ���úͲ�ѯ
     * [˵��]: SET�������ڿ��������ϱ�^SIMSQ����ʾ����<mode>=0ʱ����^SIMSQ�����ϱ�����<mode>=1����SIM��״̬�����仯ʱ��^SIMSQ: <mode>,<sim_status>�ϱ���
     *         READ�������ڲ�ѯ��ǰ�ϱ�ģʽ��SIM����ǰ״̬��
     *         TEST�������ڲ�ѯ��֧���ϱ�ģʽ��Χ��
     *         MBB��̬ʹ�ã��ֻ���̬��֧��
     * [�﷨]:
     *     [����]: AT^SIMSQ=<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��صĴ���
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: AT^SIMSQ?
     *     [���]: <CR><LF>^SIMSQ: <mode>,<sim_status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: AT^SIMSQ=?
     *     [���]: <CR><LF>^SIMSQ: (list of supported <mode>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: -
     *     [���]: ��<mode>=1ʱ������SIM��״̬�仯SIMSQ�����ϱ���
     *             <CR><LF>^SIMSQ: <mode>,<sim_status><CR><LF>
     * [����]:
     *     <mode>: ����ֵ�����ڿ���SIM�������ϱ�ģʽ��Ĭ��Ϊ0
     *             0   �ر�SIM��״̬�����ϱ�^SIMSQ
     *             1   ʹ��SIM��״̬�����ϱ�^SIMSQ: <mode>,<sim_status>
     *     <sim_status>: ����ֵ��SIM��״̬
     *             0     ������λ SIM not Inserted
     *             1  ���Ѳ��� SIM Inserted
     *             2     ������   SIM PIN/PUK locked
     *             3     SIMLOCK ����(�ݲ�֧���ϱ�)
     *             10    ���ļ����ڳ�ʼ��  SIM Initializing
     *             11    ����ʼ����� ���ɽ������磩 SIM Initialized  (Network service available)
     *             12    ����ʼ����� �����ź͵绰�����Խ��룩SIM Ready  (PBM and SMS access)
     *             98    ������ʧЧ  ��PUK�������߿�����ʧЧ��
     *             99    ���Ƴ�  SIM removed
     *             100   �����󣨳�ʼ�������У���ʧ�ܣ�
     *             Note1:  <sim_status>��4-9��13-97��ΪԤ��״̬
     *             Note2:  �ݲ���������SIM��
     * [ʾ��]:
     *     �� SIM��PIN���ȴ��û�����ʱ����ѯSIM��״̬
     *       AT^SIMSQ?
     *       ^SIMSQ: 1,2
     *       OK
     *     �� SIM��PUK���ȴ��û�����ʱ����ѯSIM��״̬��δ���������ϱ���
     *       AT^SIMSQ?
     *       ^SIMSQ: 0,2
     *       OK
     *       ��ģ���ϵ�󣬼�⵽δ����SIM��ʱ
     *       ^SIMSQ: 1,0
     *       ��ģ���⵽SIM���Ѳ���ʱ
     *       ^SIMSQ: 1,1
     *       SIM�����γ�ʱ
     *       ^SIMSQ: 1,0
     *       SIM�������ȴ��û���PINʱ
     *       ^SIMSQ: 1,2
     *       SIM�������ʼ������ʱ
     *       ^SIMSQ: 1,10
     */
    { AT_CMD_SIMSQ,
      AT_SetParaRspSimsqPara, AT_SET_PARA_TIME, AT_QryParaRspSimsqPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^SIMSQ", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ȡPNN
     * [˵��]: ��ȡ(U)SIM����PNN�ļ����ݡ�
     * [�﷨]:
     *     [����]: ^PNN=<pnn_type>[,<index>]
     *     [���]: <CR><LF>^PNN: <full_name_seg>,<short_name_seg>[,<additional_information>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^PNN?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^PNN=?
     *     [���]: <CR><LF>^PNN: (list of supported <pnn_type>s), (list of supported <record_range>s), (list of supported <record_len>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <pnn_type>: ����ֵ��PNN�ļ����ͣ�ȡֵ0��1��
     *             0����ȡSIM����PNN�ļ���
     *             1����ȡUSIM����PNN�ļ���
     *     <index>: ����ֵ��PNN�ļ�¼����������PNN�ĵ�index����¼��
     *             ���������index�����������PNN�ļ�¼�б������index�����������ָ��������¼����
     *     <record_range>: PNN��������Χ��
     *     <full_name_seg>: �����ֶΣ����������ı�־�������Ⱥ��������ݡ�
     *     <short_name_seg>: �����ֶΣ����������ı�־�������Ⱥ��������ݡ�
     *     <additional_information>: ������Ϣ��Ŀǰû��ʵ����;��ֻ��������ʾSIM��PNN�ļ�ʵ�����ݡ�
     *     <record_len>: ��¼�ĳ��ȡ�
     * [ʾ��]:
     *       �ޡ�
     */
    { AT_CMD_PNN,
      At_SetPNNPara, AT_SET_PARA_TIME, At_QryPNNPara, AT_QRY_PARA_TIME, AT_TestPnnPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^PNN", (TAF_UINT8 *)"(0,1),(0-255)" },
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ȡ��ǰע�������Ӧ��PNN
     * [˵��]: ��ȡ��ǰע�������Ӧ�Ŀ���PNN�ļ����ݡ�û��ע������ʱ����ERROR��
     *         ִ������ʵ��PNNѡ���߼�������Э�鷵����Ҫ��ʾ��PNN��������ʱ������SIM��PNN�ĵ�һ����¼������ʱ�����ݵ�ǰע����������磬��OPL�ҵ��뵱ǰPLMN��ͬ��PLMN���ٸ���OPL��¼��PNN������PNN��
     *         ���������PNN�ļ����ͣ���Ҫע����ǵ�SIM/USIM������û��opl�ļ�����û��pnn�ļ�ʱ������ERROR��Ϣ��
     *         ������ִ����EFust�ļ�Լ����
     * [�﷨]:
     *     [����]: ^CPNN=<pnn_type>
     *     [���]: <CR><LF>^CPNN: <full_name>,<full_name_coding>,<full_name_ci>, <short_name>,<short_name_coding>,<shor_name_ci><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CPNN?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^CPNN=?
     *     [���]: <CR><LF>^CPNN: (list of supported <pnn_type>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <pnn_type>: ����ֵ��PNN�ļ����ͣ�ȡֵ0��1��
     *             0����ȡSIM����PNN�ļ���
     *             1����ȡUSIM����PNN�ļ���
     *     <full_name>: PLMN�ĳ�����ֻ����PNN��full name�ֶε��ַ����֡�
     *     <full_name_coding>: 0��������ѹ��GSM7λ���룻
     *             1��������UCS2���롣
     *     <full_name_ci>: ������add ciֵ��ȡֵ0��1��
     *     <short_name>: PLMN�Ķ�����ֻ����PNN��short name�ֶε��ַ����֡�
     *     <short_name_coding>: 0��������ѹ��GSM7λ���룻
     *             1��������UCS2���롣
     *     <short name_ci>: ������add ciֵ��ȡֵ0��1��
     * [ʾ��]:
     *     �� ��ѯ��ǰע�������CPNN
     *       AT^CPNN=1
     *       ^CPNN: "D456F32D4EB3CB2022",0,0,"",0,0
     *       OK
     *     �� ����CPNN
     *       AT^CPNN=?
     *       ^CPNN: (0,1)
     *       OK
     */
    { AT_CMD_CPNN,
      At_SetCPNNPara, AT_SET_PARA_TIME, At_QryCPNNPara, AT_QRY_PARA_TIME, AT_TestCpnnPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^CPNN", (TAF_UINT8 *)"(0,1)" },
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ȡOPL
     * [˵��]: ��ȡ(U)SIM����OPL�ļ����ݡ�
     *         ��Ҫע����ǲ��������SIM/USIM������û��opl�ļ�ʱ������ERROR���ļ������ڣ�����Ϣ����SIM/USIM���������opl�ļ�������opl�ļ�����û������ʱ����ֱ�ӷ���OK����������������ء�
     *         ������ִ����EFust�ļ�Լ����
     * [�﷨]:
     *     [����]: ^OPL=<opl_type>[,<index>]
     *     [���]: <CR><LF>^OPL: <plmn id>,<lac_range>,<index_pnn><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^OPL?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^OPL=?
     *     [���]: [<CR><LF>^OPL: <opl_type>, (list of supported
     *             <record_range>s),<record_len><CR><LF>]
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <opl_type>: ����ֵ��OPL�ļ����ͣ�ȡֵ0��1��
     *             0����ȡSIM����OPL�ļ���
     *             1����ȡUSIM����OPL�ļ���
     *     <index>: ����ֵ��OPL�ļ�¼����������OPL�ĵ�index����¼��
     *             ���������index�����������OPL�ļ�¼�б������index�����������ָ��������¼��
     *     <plmn id>: ����ֵ��PLMN ID��
     *     <lac_range>: ����ֵ�����������뷶Χ��
     *     <index_pnn>: ����ֵ��ָ��PNN�ĵ�index_pnn����¼��
     *     <record_range>: ����ֵ��PNN��������Χ��
     *     <record_len>: ����ֵ����¼�ĳ��ȡ�
     * [ʾ��]:
     *     �� ��ȡOPL�ļ���һ����¼
     *       AT^OPL=1,1
     *       ^OPL: 23201,0x0-0xFFFE,1
     *       OK
     */
    { AT_CMD_OPL,
      At_SetOPLPara, AT_SET_PARA_TIME, At_QryOPLPara, AT_QRY_PARA_TIME, AT_TestOplPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^OPL", (TAF_UINT8 *)"(0,1),(0-255)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ȡSPN
     * [˵��]: ����������TEͨ��ME��ѯ��ǰSIM/USIM���ϴ洢��2G/3G��SPN�ļ���
     * [�﷨]:
     *     [����]: ^SPN=<spn_type>
     *     [���]: <CR><LF>^SPN: <disp_cond>,<coding>,<spn_name><CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^SPN=?
     *     [���]: <CR><LF>^SPN: (list of supported <spn_type>)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <spn_type>: ����ֵ��SPN�ļ����͡�
     *             0��GSM_SPN��
     *             1��USIM_SPN��
     *     <disp_cond>: ����ֵ��������ʱ����ʾ����������������λ��b1����RPLMN��ʾ������b2����SPN��ʾ������b2����E5��̬�ϱ�����
     *             b1=0������ʾ��
     *             b1=1����ʾ��
     *             b2=0����ʾ��
     *             b2=1������ʾ��
     *             99�����ֶ���Ч���������ٶ�ȡspn_name�ֶΡ�
     *     <coding>: ����ֵ��spn_name�ֶε��ַ����뷽����ָ�����ԡ�
     *             0��GSM 7 bit Default Alphabet�����Է�ѹ����7bitֵ�ϴ�����
     *             1��RAW mode������spn_name��ԭʼ���ݸ�ʽ�ϴ�����
     *     <spn_name>: �ַ�����SPN���ݡ�
     *             ��<coding>=0ʱ��������16���ַ���
     *             ��<coding>=1ʱ��SPN�����ԡ�16�����ı�ֵ���������������Ȳ�����32�������ϱ�ʱ��ͳһ����ȥ��0x��16������д��
     * [ʾ��]:
     *     �� ��ѯ��ǰSPN��7bit����
     *       AT^SPN=1
     *       ^SPN: 1,0,"vodafone"
     *       OK
     *     �� ��ѯ��ǰSPN��UCS2����
     *       AT^SPN=1
     *       ^SPN: 1,1,"804E2D56FD79FB52A8"
     *       OK
     */
    { AT_CMD_SPN,
      At_SetSPNPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^SPN", (VOS_UINT8 *)"(0-1)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ʶ��SIM/USIM��ģʽ
     * [˵��]: �������ѯ��ǰ�����SIM�������͡�
     *         CLģʽ�¸����֧�֡�
     * [�﷨]:
     *     [����]: ^CARDMODE
     *     [���]: <CR><LF>^CARDMODE: <sim_type><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <sim_type>: ����ֵ��SIM�������ͣ�ȡֵ0��2��
     *             0���������ڣ�
     *             1��SIM����
     *             2��USIM����
     * [ʾ��]:
     *     �� ��ѯ��ǰ������
     *       AT^CARDMODE
     *       ^CARDMODE: 2
     *       OK
     */
    { AT_CMD_CARDMODE,
      At_SetCardModePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CARDMODE", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: vSIM��״̬
     * [˵��]: �������������û��ѯSIMӲ����vSIM���⿨�ļ���״̬��
     *         ����ͬʱ����SIMӲ����vSIM���⿨����ѯ����ķ��ؽ���У�����2�̶�Ϊ1������4�����ļ���״̬��0��ȥ���1�����
     * [�﷨]:
     *     [����]: ^HVSST=<index>,<enable>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^HVSST?
     *     [���]: ^HVSST: <index>,<enable>,<slot>,<state><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^HVSST=?
     *     [���]: <CR><LF>^HVSST: (list of supported <index>s),(list of supported <enable>s) <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <index>: vSIM��/ SIMӲ����Ϣ������ֵ������ֵ��
     *             1��SIMӲ����
     *             11��vSIM���⿨��
     *     <enable>: �����ȥ����vSIM���⿨/SIMӲ��������ֵ��ȡֵ��Χ0~1��
     *             0��ȥ�����Ӧ<index>�Ŀ���Modem������ʹ�ö�Ӧ�Ŀ�ע�����磻
     *             1�������Ӧ<index>�Ŀ���Modem��ʹ�ö�Ӧ�Ŀ�ע�����硣
     *     <slot>: ��ǰModem��Ӧ��SIM���ۣ�����ֵ��
     *             0��SIM����0��
     *             1��SIM����1��
     *             2��SIM����2��
     *     <state>: ��Ӧ<index>�Ŀ��Ƿ����ڽ������磬����ֵ��
     *             0��δ���ڽ������磻
     *             1�����ڽ������硣
     * [ʾ��]:
     *     �� �ڵ�ǰModem�ϼ���vSIM���⿨
     *       AT^HVSST=11,1
     *       OK
     *     �� ��ѯvSIM���⿨/SIMӲ������״̬������vSIM���⿨�Ѿ����ã���Modem��ӦSIM����0
     *       AT^HVSST?
     *       ^HVSST: 11,1,0,1
     *       OK
     *     �� ��������
     *       AT^HVSST=?
     *       ^HVSST: (1,11),(0,1)
     *       OK
     */
    { AT_CMD_HVSST,
      At_SetHvsstPara, AT_SET_PARA_TIME, At_QryHvsstPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HVSST", (VOS_UINT8 *)"(1,11),(0,1)" },

#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ����(U)SIM����
     * [˵��]: ����������֧�ֲ�ͬӦ�����͵Ŀ��ļ��ķ��ʣ�IMS GBA����ʹ�á�
     * [�﷨]:
     *     [����]: ^CURSM=<app_type>,<command>,<fileid>,<record_number>[,<data>[,<pathid>]]
     *     [���]: <CR><LF>^CURSM: <data><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CURSM=?
     *     [���]: <CR><LF>^CURSM: (list of supported< app_type >s),(list of supported< command >s) (list of supported < fileid >s),(list of supported < record_number >s) (<data>),(<path>)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <app_type>: USIMӦ������
     *             0��USIM��
     *             1��ISIM��
     *     <command>: ����ֵ�����ķ������
     *             0����ȡ�ļ���
     *             1�������ļ���
     *     <fileid>: ����ֵ��EF�ļ� ID��
     *     <record_number>: ����ֵ����¼�š�
     *     <data>: �ַ������ͣ���󳤶�512�ֽڡ�
     *     <pathid>: ��ʱ��ʹ�á�
     */
    { AT_CMD_CURSM,
      AT_SetUrsmPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CURSM", (VOS_UINT8 *)"(0,1),(0,1),(1-65535),(1-255),(data),(path)" },
#endif

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: CDMA��������ѯ
     * [˵��]: ���������CL+GƴƬ��Ʒ��̬�²�ѯCDMA��������ʱʹ�á����������ʹ�ÿ��ܻ�õ�����ķ���ֵ��
     *         ����������CL+GƴƬ��Ʒ��̬�²�ѯCDMA��������������UICC����������CSIMӦ�ú�/��USIMӦ�á�����ICC����������DF_CDMA��/��DF_GSM��ͨ����������Ի�ȡCDMA���Ͼ�������ЩӦ�û�DF��
     * [�﷨]:
     *     [����]: ^CARDTYPE?
     *     [���]: <CR><LF>^CARDTYPE: <CARDMODE>,<HAS_C>,<HAS_G><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <CARDMODE>: �����ͱ�ʶ��
     *             1��ICC����
     *             2��UICC����
     *     <HAS_C>: ��ʶ�����Ƿ����֧��CDMA����������UICC���ϴ���CSIM APPʱΪ1��ICC���ϴ���DF_CDMAʱΪ1����֧��CDMAʱΪ0��
     *     <HAS_G>: ��ʾ�����Ƿ���UMTS/GSM��������UICC���ϴ���USIM APPʱΪ1��ICC���ϴ���DF_GSMʱΪ1����֧��UMTS/GSMʱΪ0��
     * [ʾ��]:
     *     �� ʹ����ͨUSIM��ʱ��ѯ���
     *       AT^CARDTYPE?
     *       ^CARDTYPE: 2, 0, 1
     *       OK
     *     �� ʹ�õ���4G��(CSIM+USIM����ʱ��ѯ���
     *       AT^CARDTYPE?
     *       ^CARDTYPE: 2, 1, 1
     *       OK
     *     �� ʹ���ƶ�ICC��(DF_GSM)ʱ��ѯ���
     *       AT^CARDTYPE?
     *       ^CARDTYPE: 1, 0, 1
     *       OK
     *     �� ʹ�õ���ICC������(DF_CDMA+DF_GSM)ʱ��ѯ���
     *       AT^CARDTYPE?
     *       ^CARDTYPE: 1, 1, 1
     *       OK
     *     �� ʹ�õ���ICC������(ֻ��DF_CDMA)ʱ��ѯ���
     *       AT^CARDTYPE?
     *       ^CARDTYPE: 1, 1, 0
     *       OK
     */
    { AT_CMD_CARDTYPE,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryCardTypePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CARDTYPE", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: CDMA��������ѯ
     * [˵��]: ���������ڲ�ѯCDMA��������������UICC����������CSIMӦ�ú�/��USIMӦ�á�����ICC����������DF_CDMA��/��DF_GSM��ͨ����������Ի�ȡCDMA���Ͼ�������ЩӦ�û�DF��
     * [�﷨]:
     *     [����]: ^CARDTYPEEX?
     *     [���]: <CR><LF>^CARDTYPE: <CARDMODE>,<HAS_C>,<HAS_G><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <CARDMODE>: �����ͱ�ʶ��
     *             1��ICC����
     *             2��UICC����
     *     <HAS_C>: ��ʶ�����Ƿ����֧��CDMA����������UICC���ϴ���CSIM APPʱΪ1��ICC���ϴ���DF_CDMAʱΪ1����֧��CDMAʱΪ0��
     *     <HAS_G>: ��ʾ�����Ƿ���UMTS/GSM��������UICC���ϴ���USIM APPʱΪ1��ICC���ϴ���DF_GSMʱΪ1����֧��UMTS/GSMʱΪ0��
     * [ʾ��]:
     *     �� ʹ����ͨUSIM��ʱ��ѯ���
     *       AT^CARDTYPEEX?
     *       ^CARDTYPEEX: 2, 0, 1
     *       OK
     *     �� ʹ�õ���4G��(CSIM+USIM����ʱ��ѯ���
     *       AT^CARDTYPEEX?
     *       ^CARDTYPEEX: 2, 1, 1
     *       OK
     *     �� ʹ���ƶ�ICC��(DF_GSM)ʱ��ѯ���
     *       AT^CARDTYPEEX?
     *       ^CARDTYPEEX: 1, 0, 1
     *       OK
     *     �� ʹ�õ���ICC������(DF_CDMA+DF_GSM)ʱ��ѯ���
     *       AT^CARDTYPEEX?
     *       ^CARDTYPEEX: 1, 1, 1
     *       OK
     *     �� ʹ�õ���ICC������(ֻ��DF_CDMA)ʱ��ѯ���
     *       AT^CARDTYPEEX?
     *       ^CARDTYPEEX: 1, 1, 0
     *       OK
     */
    { AT_CMD_CARDTYPEEX,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryCardTypeExPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CARDTYPEEX", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ����6F7E���ļ���Ϣ
     * [˵��]: ����������AP֪ͨ���úͲ�ѯ6f7e���ļ���Ϣ��
     * [�﷨]:
     *     [����]: ^EFLOCIINFO=<tmsi>,<plmn>,<lac>,<location_update_status>,<rfu>
     *     [���]: ������ȷ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<CR><LF>
     *     [����]: ^EFLOCIINFO?
     *     [���]: <CR><LF>^EFLOCIINFO: <tmsi>,<plmn><lac>,<location_update_status>,<rfu><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^EFLOCIINFO=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <tmsi>: 4���ֽ�, TMSI��Ϣ��
     *     <plmn>: �ַ������ͣ�PLMN��Ϣ��
     *     <lac>: �����ֽڣ�LAC��Ϣ��
     *     <location_update_status>: 1���ֽڣ�
     *             0: updated
     *             1: not updated
     *             2: plmn not allowed
     *             3: location area not allowed
     *             4~255: reserved��
     *     <rfu>: �����ֶΡ�
     * [ʾ��]:
     *     �� ���6f7e���ļ���Ϣ��
     *       AT^EFLOCIINFO=0xffffffff,"ffffff",0x0000,1,0xff
     *       OK
     *     �� ��ѯ6f7e���ļ���Ϣ:
     *       AT^EFLOCIINFO?
     *       ^EFLOCIINFO: 0xffffffff,"ffffff",0x0000,1,0xff
     *       OK
     */
    { AT_CMD_EFLOCIINFO,
      At_SetEflociInfoPara, AT_SET_PARA_TIME, At_QryEflociInfoPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^EFLOCIINFO", (VOS_UINT8 *)"(0-4294967295),(PLMN),(0-65535),(0-255),(0-255)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ����6F73���ļ���Ϣ
     * [˵��]: ����������AP֪ͨ���úͲ�ѯ6f73���ļ���Ϣ��
     * [�﷨]:
     *     [����]: ^EFPSLOCIINFO=<ptmsi>,<Ptmsi_signature><plmn>,<lac>,<rac><routing_area_update_status>
     *     [���]: ������ȷ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<CR><LF>
     *     [����]: ^EFPSLOCIINFO?
     *     [���]: <CR><LF>^EFPSLOCIINFO: <Ptmsi>,<Ptmsi_signature>,<plmn>,<lac>,<rac><routing_area_update_status><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^EFPSLOCIINFO=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <ptmsi>: �ĸ��ֽ�, PTMSI��Ϣ��
     *     <ptmsi_signature>: �����ֽڣ�PTMSI SIGNATURE��Ϣ
     *     <plmn>: �ַ������ͣ�PLMN��Ϣ��
     *     <lac>: �����ֽڣ�LAC��Ϣ��
     *     <rac>: һ���ֽڣ�RAC��Ϣ
     *     <routing_area_update_status>: 1���ֽڣ�
     *             0: updated
     *             1: not updated
     *             2: plmn not allowed
     *             3: routing area not allowed
     *             4~255: reserved��
     * [ʾ��]:
     *     �� ���6f73���ļ���Ϣ��
     *       AT^EFPSLOCIINFO=0xffffffff,0xffffff,"ffffff",0x0000,0xff,1
     *       OK
     *     �� ��ѯ6f73���ļ���Ϣ:
     *       AT^EFPSLOCIINFO?
     *       ^EFPSLOCIINFO: 0xffffffff,0xffffff,"ffffff",0x0000,0xff,1
     *       OK
     */
    { AT_CMD_EFPSLOCIINFO,
      At_SetPsEflociInfoPara, AT_SET_PARA_TIME, At_QryPsEflociInfoPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^EFPSLOCIINFO", (VOS_UINT8 *)"(0-4294967295),(0-16777215),(PLMN),(0-65535),(0-255),(0-255)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ˽�д��߼�ͨ��
     * [˵��]: ���������ڴ��߼�ͨ����
     * [�﷨]:
     *     [����]: ^CCHO=<dfname>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^CCHO: <sessionId>,<length>,<response><CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^CCHO=?
     *     [���]: <CR><LF>OK CR><LF>
     * [����]:
     *     <dfname>: ����ֵ��Ӧ�����ƣ�����1~16�ֽڡ�
     *     <sessionId>: ����ֵ���߼�ͨ���š�����AT�����еı�ʶ������������APDU���UICCͨ����
     *     <length>: ����ֵ��ѡ��Ӧ�÷������ݵĳ���
     *     <response>: ѡ��Ӧ�÷��ص�����
     * [ʾ��]:
     *     �� ���߼�ͨ��
     *       AT^CCHO="A0000000871002FF86FF0389FFFFFFFF"
     *       ^CCHO: 123456,4,"9000"
     *     �� ����^CCHO
     *       AT^CCHO=?
     *       OK
     */
    { AT_CMD_PRIVATECCHO,
      At_SetPrivateCchoPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^CCHO", (TAF_UINT8 *)"(dfname)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ˽�д��߼�ͨ��
     * [˵��]: ���������ڴ��߼�ͨ����
     * [�﷨]:
     *     [����]: ^CCHP=<dfname>,<P2>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^CCHP: <sessionId>,<length>,<response><CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^CCHP=?
     *     [���]: <CR><LF>OK CR><LF>
     * [����]:
     *     <dfname>: ����ֵ��Ӧ�����ƣ�����1~16�ֽڡ�
     *     <P2>: ����ֵ��APDU �����P2������ȡֵ 0~255��Ŀǰ֧�֣�
     *             0x00:����Ӧ�á�
     *             0x40:��ֹӦ�á�
     *             0x04:���߼�ͨ������Ҫ����  FCP��
     *             0x0C:���߼�ͨ����û�����ݴ��ء�
     *     <sessionId>: ����ֵ���߼�ͨ���š�����AT�����еı�ʶ������������APDU���UICCͨ����
     *     <length>: ����ֵ��ѡ��Ӧ�÷������ݵĳ���
     *     <response>: ѡ��Ӧ�÷��ص�����
     * [ʾ��]:
     *     �� ���߼�ͨ��
     *       AT^CCHP="A0000000871002FF86FF0389FFFFFFFF",4
     *       ^CCHP:450647875,102,"622F820278218410A0000000871002FF86FF0389FFFFFFFF8A01058B036F0629C60F9001A09501008301018301819000"
     *     �� ����^CCHP
     *       AT^CCHP=?
     *       OK
     */
    { AT_CMD_PRIVATECCHP,
      At_SetPrivateCchpPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^CCHP", (TAF_UINT8 *)"(dfname),(0-255)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ���ÿ�����BUSY�ĵȴ�ʱ��
     * [˵��]: �������������ÿ��ƿ�����BUSYʱ�ĵȴ�ʱ�䣬���޸�NV4012�еĵȴ�ʱ�䡣
     * [�﷨]:
     *     [����]: ^BWT=<newTime>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^BWT=?
     *     [���]: <CR><LF>^BWT: (list of supported <newTime>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <newTime>: ������Ҫ�ȴ���ʱ����ȡֵ��Χ0~65535����λΪ�롣
     * [ʾ��]:
     *     �� ���ÿ�����BUSY�ĵȴ�ʱ��
     *       AT^BWT=5
     *       OK
     *     �� ��������
     *       AT^BWT=?
     *       ^BWT: (0-65535)
     *       OK
     */
    { AT_CMD_BWT,
      At_SetBwtPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^BWT", (VOS_UINT8 *)"(0-65535)" },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: SIM����ʼ����λ��ѯ
     * [˵��]: ���������ڲ�ѯSIM����ʼ��ʧ�ܳ������Ƿ���SIM����λ��
     * [�﷨]:
     *     [����]: ^CARDHISTORYATR
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^CARDHISTORYATR: <ATR><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^CARDHISTORYATR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <ATR>: SIM�����ص�ATR����
     * [ʾ��]:
     *     �� SIM����ʼ��ʧ�ܳ����²�ѯsim����ATR���ݣ�
     *       AT^CARDHISTORYAT
     *       ^CARDHISTORYAT: "3B9F94801FC78031E073FE21135712291102010000C3"
     *       OK
     */
    { AT_CMD_CARDHISTORYATR,
      At_SetCardHistoryATRPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^CARDHISTORYATR", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ��ѯ��Ӧ��SESSION ID
     * [˵��]: ���������UICC����ʱʹ�ã���ICC����ʹ�û�õ�����ķ���ֵ��
     *         ���������ڲ�ѯUICC����Ӧ�õ�SESSSION ID��Ŀǰ����CSIM��USIM��ISIMӦ�ö�Ӧ��ֵ��������������CRLA֮ǰȷ�Ϸ���ͨ����Session ID��ֵ��
     * [�﷨]:
     *     [����]: ^CARDSESSION?
     *     [���]: <CR><LF>^CARDSESSION: CSIM, <sessionid>, USIM, <sessionid>, ISIM, <sessionid>, <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <sessionid>: ʮ��������ֵ��0�����Ӧ�ò����ڣ�����ֵΪ������
     * [ʾ��]:
     *     �� ��ѯUICC��Ӧ��
     *       AT^CARDSESSION?
     *       ^CARDSESSION: CSIM,123456,USIM,0,ISIM,234567
     *       OK
     */
    { AT_CMD_CARDSESSION,
      TAF_NULL_PTR, AT_NOT_SET_TIME, At_QryCardSession, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CARDSESSION", VOS_NULL_PTR },


    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: �����߼�ͨ������
     * [˵��]: ������������FEATURE_UE_UICC_MULTI_APP_SUPPORT��򿪵İ汾�ϣ�AP�ڿ����߼�ͨ���������UICC�����ļ���ICC����֧�ֶ��߼�ͨ��������ʹ�ø�����ʱ����Ȼ��Ҫ�����ӦӦ�õ�Session ID��
     *         ICC������μ���3GPP 51.011���ġ�9.2 Coding of the commands����������ı��롣
     *         UICC������μ���ETSI TS 102.221���ġ�11 Commands����������ı��롣
     *         ��������ӡ�3GPP TS 27.007��Э�顣
     * [�﷨]:
     *     [����]: ^CRLA=<sessionid>,<command>[,<fileid>[,<P1>,<P2>,<P3>[,<data>[,<pathid>]]]]
     *     [���]: <CR><LF>^CRLA: <sw1>,<sw2>[,<response>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CRLA=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <sessionid>: ������AT^CARDSESSION�ķ���ֵ������AT^CARDSESSION����ʧ�����AT�����޷�����ʹ��
     *     <command>: ����ֵ��SIM�������
     *             162����ѯ��¼�ļ���Ч��¼��Ϣ��
     *             176����ȡ�������ļ���
     *             178����ȡ��¼���ݣ�
     *             192����ȡ��Ӧ��
     *             214�����¶������ļ���
     *             220�����¼�¼���ݣ�
     *             242��״̬��ѯ��
     *     <fileid>: ����ֵ��EF�ļ�ID����״̬��ѯ���������������·��ò�����
     *     <P1>,<P2>,<P3>: ����ֵ������״̬��ѯ�����Щ��������ָ�����μ���3GPP TS 51.011  9.2 Coding of the commands����
     *     <data>: ��ʮ�����Ƹ�ʽ����Ϣ�ֶΡ�
     *     <pathid>: �ַ������ͣ���������EF�ļ���·������ο�ETSI TS 102 221���������fileid�ֶΣ���pathid�������·����3F 00��ʼ
     *     <sw1>,<sw2>: ����ֵ������ִ�к�SIM�����ص���Ӧ��
     *     <response>: Я������ִ�гɹ����ϱ������ݣ����ڶ����Ƹ��ºͼ�¼�������<response>�����ء�
     * [ʾ��]:
     *     �� ��״̬��ѯ
     *       AT^CRLA=123456,242
     *       ^CRLA: 90,0,"622D82027821840CA0000000871002FF49FF0589A503DA01038A0 1058B03 2F0603C60C90016083010183010C830181"
     *       OK
     *     �� ��ȡIMSI
     *       AT^CRLA=123456,176,28423,0,0,9,,"3F007FFF"
     *       ^CRLA: 90,0,"08490620281190 8074"
     *       OK
     *     �� ��ȡ��Ӧ
     *       AT^CRLA=123456,192,20272,0,0,15,,"3F007F105F3A"
     *       ^CRLA: 144,0,"621E82054221004C0283024F30A5038001718A01058B036F060 5800200988800"
     *     �� ��ѯ��¼�ļ���Ч��¼��Ϣ
     *       AT^CRLA=71938592,162,0x6F3B,1,4,0x1C,"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF","3F007FFF"
     *       ^CRLA: 144,0,"0102030405060708090A"
     *       OK
     */
    { AT_CMD_CRLA,
      At_SetCrlaPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CRLA", (TAF_UINT8 *)"(1-4294967295),(0-255),(1-65535),(0-255),(0-255),(0-255),(cmd),(pathid)" },
};

/* ע��sim pam AT����� */
VOS_UINT32 AT_RegisterSimPamCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atSimPamCmdTbl, sizeof(g_atSimPamCmdTbl) / sizeof(g_atSimPamCmdTbl[0]));
}

