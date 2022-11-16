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
#include "at_custom_hifi_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_hifi_set_cmd_proc.h"
#include "at_custom_hifi_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_HIFI_CMD_TBL_C

static const AT_ParCmdElement g_atCustomHifiCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����˫ͨ����ʱHIFI�л�
     * [˵��]: ������������·�绰���������У���������ͨ·���л���
     *         ǰ̨ģʽ������������������������������������˳���̨ģʽ�����ó�ǰ̨ģʽ�򼤻�ĳ·�绰������ͬ����
     *         ��̨ģʽ�������н��뾲��״̬�����е��������ͨ·�Ͽ��������������ú�̨ģʽ��ȥ����ĳ·�绰��ʾ����ͬ������˼����
     *         ����ʹ�ó�����˫��ͨ���л�ʱAP���·���
     * [�﷨]:
     *     [����]: ^CBG=<n>
     *     [���]: ���óɹ���
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CBG?
     *     [���]: ��ѯ�ɹ���
     *             <CR><LF>^CBG: <n><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CBG=?
     *     [���]: <CR><LF>^CBG: (0,1)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: ����ֵ������ǰ��̨���ء�
     *             0��ǰ̨ģʽ��
     *             1����̨ģʽ��
     * [ʾ��]:
     *     �� ����ǰ̨ģʽ
     *       AT^CBG=0
     *       OK
     *     �� ��ѯǰ��̨ģʽ
     *       AT^CBG?
     *       ^CBG: 0
     *       OK
     *     �� ��������
     *       AT^CBG=?
     *       ^CBG: (0,1)
     *       OK
     */
    { AT_CMD_CBG,
      AT_SetCbgPara, AT_SET_VC_PARA_TIME, AT_QryCbgPara, AT_QRY_VC_PARA_TIME, AT_TestCbgPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CBG", (VOS_UINT8 *)"(0,1)" },
};

/* ע��HIFI����AT����� */
VOS_UINT32 AT_RegisterCustomHifiCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomHifiCmdTbl, sizeof(g_atCustomHifiCmdTbl) / sizeof(g_atCustomHifiCmdTbl[0]));
}

