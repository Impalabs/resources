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

#include "at_voice_hifi_cmd_tbl.h"
#include "at_voice_hifi_set_cmd_proc.h"
#include "at_voice_hifi_qry_cmd_proc.h"
#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_VOICE_HIFI_CMD_TBL_C

static const AT_ParCmdElement g_atVoiceHifiCmdTbl[] = {
#if (FEATURE_ECALL == FEATURE_ON)
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: eCall IVS����MSD������������
     * [˵��]: ����������������eCall�Ự�ڼ䣬������ʹIVS��������MSD�������󣬵�������������´���PSAP����Ӧ״����
     * [�﷨]:
     *     [����]: ^ECLPUSH
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� ��eCall�Ự�ڼ䣬IVS����MSD������������
     *       AT^ECLPUSH
     *       OK
     */
    { AT_CMD_ECLPUSH,
      AT_SetEclpushPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLPUSH", VOS_NULL_PTR },
#endif

    /* ֧��5����̬���������� */
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��������ģʽ
     * [˵��]: ���á���ѯ����ģʽ��
     *         PC����ģʽ��NV��8471������أ���ͬ�汾��Ĭ�����ò�ͬ��
     * [�﷨]:
     *     [����]: ^VMSET=<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^VMSET?
     *     [���]: <CR><LF>^VMSET: <mode>CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^VMSET=?
     *     [���]: <CR><LF>^VMSET: (list of supported <mode>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������ģʽ��ȡֵ��Χ��0-9����
     *             0���ֳ�ģʽ
     *             1���ֳ�����ģʽ
     *             2����������ģʽ���ݲ�֧�֣�
     *             3������ģʽ
     *             4������ģʽ
     *             5��PC����ģʽ
     *             6��������˵Ķ���ģʽ
     *             7����������ģʽ
     *             8������ͨ��ģʽ
     *             9��USB�豸ģʽ
     *             10��USB��Ƶģʽ
     * [ʾ��]:
     *     �� ��������ģʽ
     *       AT^VMSET=4
     *       OK
     *     �� ��ѯ����
     *       AT^VMSET?
     *       ^VMSET: 4
     *       OK
     *     �� ��������
     *       AT^VMSET=?
     *       ^VMSET: (0-10)
     *       OK
     */
    { AT_CMD_VMSET,
      At_SetVMSETPara, AT_SET_VC_PARA_TIME, At_QryVMSETPara, AT_QRY_VC_PARA_TIME, AT_TestVmsetPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^VMSET", (VOS_UINT8 *)"(0-10)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ������������ģʽ����
     * [˵��]: ������������ģʽ��
     * [�﷨]:
     *     [����]: ^MODEMLOOP=<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^MODEMLOOP=?
     *     [���]: <CR><LF>^ MODEMLOOP: (list of supported <mode>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ������ģʽ��ȡֵ��ΧΪ0~1��
     *             0���˳�����ģʽ
     *             1�����뻷��ģʽ
     * [ʾ��]:
     *     �� ���ý��뻷��ģʽ
     *       AT^MODEMLOOP=1
     *       OK
     *     �� ��������
     *       AT^MODEMLOOP=?
     *       ^MODEMLOOP: (0,1)
     *       OK
     */
    { AT_CMD_MODEMLOOP,
      At_SetModemLoopPara, AT_NOT_SET_TIME,
#if (FEATURE_MBB_CUST == FEATURE_ON)
      AT_QryModemLoopPara,
#else
      VOS_NULL_PTR,
#endif
      AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^MODEMLOOP", (VOS_UINT8 *)"(0,1)" },

#if (FEATURE_MBB_CUST == FEATURE_ON)
    { AT_CMD_SMUT,
      AT_SetSpeakerMutePara, AT_SET_PARA_TIME, AT_QrySpeakerMutePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^SMUT", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_CMIC,
      At_SetCmicPara, AT_SET_PARA_TIME, At_QryCmicPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8*)"+CMIC", (VOS_UINT8*)"(1-12)" },

    { AT_CMD_ECHOEX,
      AT_SetEchoSuppressionPara, AT_SET_PARA_TIME, AT_QryEchoSuppressionPara, AT_QRY_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECHOEX", (VOS_UINT8 *)"(0,1),(0-24),(0-24),(0,1),(0-24)" },

    { AT_CMD_PCMFR,
      AT_SetPcmFrPara, AT_SET_PARA_TIME, AT_QryPcmFrPara, AT_QRY_PARA_TIME, AT_TestPcmFrPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^PCMFR", (VOS_UINT8 *)"(PCMFR)" },

    { AT_CMD_ECHOSWITCH,
      AT_SetEchoSwitchPara, AT_SET_PARA_TIME, AT_QryEchoSwitchPara, AT_QRY_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECHOSWITCH", (VOS_UINT8 *)"(0-1)" },

    { AT_CMD_CPCM,
      AT_SetCpcmPara, AT_NOT_SET_TIME, AT_QryCpcmPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CPCM", ( VOS_UINT8 *)"(0,2),(0),(0-4),(0),(0)" },
#endif

};

/* ע��taf����AT����� */
VOS_UINT32 AT_RegisterVoiceHifiCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atVoiceHifiCmdTbl, sizeof(g_atVoiceHifiCmdTbl) / sizeof(g_atVoiceHifiCmdTbl[0]));
}

