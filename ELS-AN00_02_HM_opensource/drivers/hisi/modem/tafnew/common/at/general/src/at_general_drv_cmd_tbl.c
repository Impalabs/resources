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

#include "at_general_drv_cmd_tbl.h"
#include "at_general_drv_set_cmd_proc.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_test_para_cmd.h"



/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_GENERAL_DRV_CMD_TBL_C

static const AT_ParCmdElement g_atGeneralDrvCmdTbl[] = {
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯӲ���汾��
     * [˵��]: �������ѯ��ǰ��Ӳ���汾�š�
     *         V9R1��ʹ�ô������ѯӲ���汾�ţ���ͨ��A���豸�ڵ�boardid��ѯ��
     * [�﷨]:
     *     [����]: ^HWVER
     *     [���]: <CR><LF>^HWVER: <hardversion><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <hardversion>: Ӳ���汾�ţ����Ȳ�����30���ַ�����
     * [ʾ��]:
     *     �� ��ѯ��Ʒ��Ӳ���汾��
     *       AT^HWVER
     *       ^HWVER: "BALONGSPC2H"
     *       OK
     */
    { AT_CMD_HWVER,
      At_SetHwVerPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HWVER", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯӲ���汾��
     * [˵��]: �������ѯ��ǰ��Ӳ���汾�š�
     *         ������͡�AT^HWVER�����������������ڲ�ͬ�ĺ�̨��
     * [�﷨]:
     *     [����]: ^HVER
     *     [���]: <CR><LF>^HVER: <hardversion><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <hardversion>: Ӳ���汾�ţ����Ȳ�����30���ַ�����
     * [ʾ��]:
     *     �� ��ѯӲ���汾
     *       AT^HVER
     *       ^HVER: "CH1E122XM"
     *       OK
     */
    { AT_CMD_HVER,
      At_SetHwVerPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HVER", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯ���������Ӳ���汾��
     * [˵��]: �������ѯ��ǰ�����������Ӳ���汾�š�
     * [�﷨]:
     *     [����]: ^FHVER
     *     [���]: <CR><LF>^FHVER:<fullversion><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <fullversion>: �ַ������ڲ���Ʒ��+����汾+Ӳ���汾�š�
     * [ʾ��]:
     *     �� ��ѯ�����Ӳ���汾
     *       AT^FHVER
     *       ^FHVER:"E1221 11.002.00.00.00,CH1E122XM Ver.A"
     *       OK
     */
    { AT_CMD_FHVER,
      At_SetFHVerPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FHVER", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ��ѯƽ̨����汾��
     * [˵��]: �������ѯ��ǰƽ̨����汾�š�
     * [�﷨]:
     *     [����]: ^PFVER
     *     [���]: <CR><LF>^PFVER: <flatversion><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <flatversion>: �ַ�������Ʒʹ�õ�ƽ̨����汾�š�
     * [ʾ��]:
     *     �� ��ѯƽ̨����汾��
     *       AT^PFVER
     *       ^PFVER: "V3R1C31B112SP22 Jul 12 2011 19:45:12"
     *       OK
     */
    { AT_CMD_PFVER,
      At_SetPfverPara, AT_SET_PARA_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PFVER", VOS_NULL_PTR },
};

/* ע��DRVͨ��AT����� */
VOS_UINT32 AT_RegisterGeneralDrvCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atGeneralDrvCmdTbl, sizeof(g_atGeneralDrvCmdTbl) / sizeof(g_atGeneralDrvCmdTbl[0]));
}

