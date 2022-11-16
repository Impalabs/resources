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
#include "at_phone_phy_cmd_tbl.h"
#include "AtParse.h"
#include "at_parse_cmd.h"
#include "at_phone_phy_set_cmd_proc.h"
#include "at_device_cmd.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_PHY_CMD_TBL_C

static const AT_ParCmdElement g_atPhonePhyCmdTbl[] = {
    /*
     * [���]: Э��AT-������˽��
     * [����]: DCXO����������������
     * [˵��]: ���������������ȫ�������ݡ��ָ���ɾ����ƽ�ơ�ʱ��ƫ�Ʋ�����
     *         ������������ػ�״̬����Ч��
     *         ֻ������Modem����AT���
     * [�﷨]:
     *     [����]: ^SAMPLE=<Type>,<TempRange>,<PpmOffset>,<TimeOffset>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Type>: ����ֵ���������ͣ�
     *             0�����ݵ�ǰ������
     *             1���ָ�����������
     *             2��ɾ������������
     *             3��ɾ������������
     *             4��ƽ������������ƽ��������ģ���ϻ�����
     *     <TempRange>: ����ֵ��������Χ��
     *             1: ������;
     *             2: ����;
     *             3: ���¡�������;
     *             4: ����;
     *             5: ���¡�������;
     *             6: ���¡�����;
     *             7: ���¡����¡�������;
     *             8: ����;
     *             9: ���¡�������;
     *             10: ���¡�����;
     *             11: ���¡����¡�������;
     *             12: ���¡�����;
     *             13: ���¡����¡�������;
     *             14: ���¡����¡�����;
     *             15: ���¡����¡����¡�������;
     *             16: ������;
     *             17: �����¡�������;
     *             18: �����¡�����;
     *             19: �����¡����¡�������;
     *             20: �����¡�����;
     *             21: �����¡����¡�������;
     *             22: �����¡����¡�����;
     *             23: �����¡����¡����¡�������;
     *             24: �����¡�����;
     *             25: �����¡����¡�������;
     *             26: �����¡����¡�����;
     *             27: �����¡����¡����¡�������;
     *             28: �����¡����¡�����;
     *             29: �����¡����¡����¡�������;
     *             30: �����¡����¡����¡�����;
     *             31: �����¡����¡����¡����¡�������;
     *     <PpmOffset>: ����ֵ������ƫ��������λ0.1ppm��,Ĭ��Ϊ0��
     *     <TimeOffset>: ����ֵ������ƫ��ʱ��������λ���£���Ĭ��Ϊ0���ò���Ϊ0����
     * [ʾ��]:
     *     �� ȫ��������
     *       AT^SAMPLE=0
     *       OK
     *     �� ɾ����������
     *       AT^SAMPLE=3
     *       OK
     *     �� Ҫ�Ը��ºͼ����µ�������ƫ�����ϻ�
     *       AT^SAMPLE=4,9,3,-6
     *       OK
     *       ����Ը������ͼ����������������6���º��ϻ���������ƫ0.3ppm��
     *     �� ɾ����ǰ����
     *       AT^SAMPLE=2,0
     *       OK
     */
    { AT_CMD_SAMPLE,
      AT_SetSamplePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^SAMPLE", (VOS_UINT8 *)"(0-255),(0-255)" },
};

/* ע��phone phy AT����� */
VOS_UINT32 AT_RegisterPhonePhyCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atPhonePhyCmdTbl, sizeof(g_atPhonePhyCmdTbl) / sizeof(g_atPhonePhyCmdTbl[0]));
}

