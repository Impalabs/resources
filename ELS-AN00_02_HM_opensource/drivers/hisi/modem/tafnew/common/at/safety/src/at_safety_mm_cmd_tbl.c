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

#include "at_safety_mm_cmd_tbl.h"
#include "at_safety_mm_qry_cmd_proc.h"
#include "at_safety_mm_set_cmd_proc.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_SAFETY_MM_CMD_TBL_C

static const AT_ParCmdElement g_atSafetyMmCmdTbl[] = {
    /*
     * [���]: Э��AT-��ȫ����
     * [����]: PIN����
     * [˵��]: PIN����֤�ͽ�����
     *         ��֤ʱ<pin>�������PIN�룬<newpin>�������룻
     *         ����ʱ<pin>�������PUK�룬<newpin>���滻��PIN�����PIN�롣
     *         ��ѯ�����PIN��ĵ�ǰ״̬��
     *         ������͡�AT+CPIN�����������ڴ������Ƕԡ�AT+CPIN������չ��
     * [�﷨]:
     *     [����]: ^CPIN=<pin>[,<newpin>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CPIN?
     *     [���]: <CR><LF>^CPIN: <code>,[<times>],<puk_times>,<pin_times>,<puk2_times>,<pin2_times><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CPIN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <pin>,<newpin>: �ַ������ͣ�����4��8��
     *     <code>: �ַ�������������˫���ţ���ȡֵ���£�
     *             READY��MT��������������
     *             SIM PIN��(U)SIM PIN��������
     *             SIM PUK��(U)SIM PUK��������
     *             SIM PIN2��(U)SIM PIN2��������
     *             SIM PUK2��(U)SIM PUK2��������
     *     <times>: ʣ����������������PIN��PIN2��������������3�Σ�����PUK��PUK2������������10�Ρ����������������������<times>�ֶλ������ǰ��ҪУ��������ʣ��У��������������������������������ֶ�Ϊ�ա�
     *     <puk_times>: ʣ���PUK���������������������10�Ρ�
     *     <pin_times>: ʣ���PIN���������������������3�Ρ�
     *     <puk2_times>: ʣ���PUK2���������������������10�Ρ�
     *     <pin2_times>: ʣ���PIN2���������������������3�Ρ�
     * [ʾ��]:
     *     �� PIN��֤����
     *       AT^CPIN="1234"
     *       OK
     *     �� PIN������������һ��������PUK�룬�ڶ����������µ�PIN��
     *       AT^CPIN="11111111","1234"
     *       OK
     *     �� ��ѯ��ǰPIN��״̬��ʣ�����
     *       AT^CPIN?
     *       ^CPIN: SIM PIN2,3,10,3,10,3
     *       OK
     *     �� ����CPIN
     *       AT^CPIN=?
     *       OK
     */
    { AT_CMD_CPIN_2,
      At_SetCpinPara, AT_SET_PARA_TIME, At_QryCpinPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CPIN", (TAF_UINT8 *)"(@pin),(@newpin)" },

    /*
     * [���]: Э��AT-��ȫ����
     * [����]: ��Ӫ����
     * [˵��]: ��������Ҫ���ڶ�MT���н�������������ֵ�ǰʹ��SIM/USIM��������Ӫ��ָ��SIM/USIM������ôҪ���û�����unlock_code�����򽫲��ܹ�ʹ�ø����ݿ���
     *         ÿ�����ݿ���Ӧһ��Ψһ��unlock_code�����û�����10�β���ȷ��unlock_code�󣬽��������������ݿ�������Զ���ܹ�ʹ�÷���Ӫ��ָ����SIM/USIM�������Ϊ��Ӫ��ָ����SIM/USIM������Ȼ��������ʹ�á�
     *         ����û�������һ��������ȷ֮�����뽫�ᱻ������������û���������ʹ�ø����ݿ�����ʹ��������һ�ŷ�ָ������Ҳ������Ҫ�����������롣
     * [�﷨]:
     *     [����]: ^CARDLOCK=<unlock_code>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CARDLOCK?
     *     [���]: <CR><LF>^CARDLOCK: <status>,<times>,<operator><CR><LF><CR><LF>OK<CR><LF>
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CARDLOCK=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <unlock_code>: �����ݿ��Ľ����룬����Ϊ8���ַ�����������[0, 9]��ɡ���Ҫʹ�����š�
     *     <status>: ����ֵ����ʾ��ǰ�����ݿ���״̬��ȡֵ1��3��
     *             1����Ҫ�û��ṩ<unlock_code>��
     *             2������Ҫ�û��ṩ<unlock_code>��
     *             3�����ݿ��Ѿ���������
     *     <times>: ����ֵ����Χ��[0, 10]����ʾʣ��Ŀ������������Ĵ�����������������10�Ρ�
     *     <operator>: ����ֵ����ʾ�ṩ��CARDLOCKҵ�����Ӫ�̡�Ŀǰֻ֧�ַ���0��
     * [ʾ��]:
     *     �� ��ѯ��Ӫ����״̬
     *       AT^CARDLOCK?
     *       ^CARDLOCK: 1,10,0
     *       OK
     *     �� ���Ϊ����Ӫ��ָ����,��Ҫ���������ҵ�ǰ�Ķ�Ӧ��unlock_codeΪ11111111
     *       AT^CARDLOCK="11111111"
     *       OK
     *     �� ����CARDLOCK
     *       AT^CARDLOCK=?
     *       OK
     */
    { AT_CMD_CARDLOCK,
      At_SetCardlockPara, AT_SET_PARA_TIME, At_QryCardlockPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^CARDLOCK", (TAF_UINT8 *)"(nlock_code)" },

    /*
     * [���]: Э��AT-��ȫ����
     * [����]: PIN2����
     * [˵��]: PIN2����֤�ͽ�����
     *         ��֤ʱ<pin2>�������PIN2�룬<newpin2>�������룻
     *         ����ʱ<pin2>�������PUK2�룬<newpin2>���滻��PIN2�����PIN2�롣
     *         ��ѯ�����PIN2��ĵ�ǰ״̬��
     *         ֻ����AT+CPIN?�������AT^CPIN?����ء�READY��������£������������ȷ�ķ��أ���������·��ش���Sim failure����
     * [�﷨]:
     *     [����]: ^CPIN2=<pin2>[,<newpin2>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CPIN2?
     *     [���]: <CR><LF>^CPIN2: <code>,[<times>],<puk_times>,<pin_times>,<puk2_times>,<pin2_times><CR><LF>OK<CR><LF>
     *     [����]: ^CPIN2=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <pin2>,<newpin2>: �ַ������ͣ�����4��8��
     *     <code>: �ַ�������������˫���ţ���ȡֵ���£�
     *             SIM PIN2��(U)SIM PIN2��������
     *             SIM PUK2��(U)SIM PUK2��������
     *     <times>: ����ֵ��ʣ����������������PIN2��������������3�Σ�����PUK2������������10�Ρ�
     *     <puk_times>: ����ֵ��ʣ���PUK���������������������10�Ρ�
     *     <pin_times>: ����ֵ��ʣ���PIN���������������������3�Ρ�
     *     <puk2_times>: ����ֵ��ʣ���PUK2���������������������10�Ρ�
     *     <pin2_times>: ����ֵ��ʣ���PIN2���������������������3�Ρ�
     * [ʾ��]:
     *     �� PIN2��֤����
     *       AT^CPIN2="1234"
     *       OK
     *     �� PIN2������������һ��������PUK2�룬�ڶ����������µ�PIN2��
     *       AT^CPIN2="11111111","1234"
     *       OK
     *     �� ��ѯ��ǰPIN2��״̬��ʣ�����
     *       AT^CPIN2?
     *       ^CPIN: SIM PIN2,3,10,3,10,3
     *       OK
     *     �� ����CPIN2
     *       AT^CPIN2=?
     *       OK
     */
    { AT_CMD_CPIN2,
      At_SetCpin2Para, AT_SET_PARA_TIME, At_QryCpin2Para, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (TAF_UINT8 *)"^CPIN2", (TAF_UINT8 *)"(@pin),(@newpin)" },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    { AT_CMD_CMLCK,
      At_SetCmlckPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCmlckPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"^CMLCK", (VOS_UINT8 *)"(\"PS\"),(0,1),(psw),(str)" },

    { AT_CMD_CPDW,
      At_SetCpdwPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE_E5,
      (VOS_UINT8 *)"^CPDW", VOS_NULL_PTR },
#endif
};

/* ע��MM SAFETY AT����� */
VOS_UINT32 AT_RegisterSafetyMmCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atSafetyMmCmdTbl, sizeof(g_atSafetyMmCmdTbl) / sizeof(g_atSafetyMmCmdTbl[0]));
}

