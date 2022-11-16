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
#include "at_device_mm_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"

#include "at_device_mm_set_cmd_proc.h"
#include "at_device_mm_qry_cmd_proc.h"



#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_MM_CMD_TBL_C

static const AT_ParCmdElement g_atDeviceMmCmdTbl[] = {
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����ϵͳ������
     * [˵��]: ���÷��������͡�����Ʒ��֧��ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^SDOMAIN=<srv_pref>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SDOMAIN?
     *     [���]: <CR><LF>^SDOMAIN: <srv_pref><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <srv_pref>: ���÷������ȣ�����Ϊһ���ֽڡ�
     *             0��CS ONLY��
     *             1��PS ONLY��
     *             2��CS and PS��
     *             3��Prefer any service��
     *     <err>: �����롣
     *             0�����ݱ���δ�����
     *             1����������
     * [ʾ��]:
     *     �� ����ϵͳ������ɹ�
     *       AT^SDOMAIN=1
     *       OK
     */
    { AT_CMD_SDOMAIN,
      At_SetSDomainPara, AT_NOT_SET_TIME, At_QrySDomainPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SDOMAIN", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ý������״̬
     * [˵��]: ���õ�����뵽����״̬��
     * [�﷨]:
     *     [����]: ^PSTANDBY=<timer>,<switch_duration>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^PSTANDBY=?
     *     [���]: <CR><LF>^PSTANDBY: (list of supported <timer>s),(list of supported <switch_duration>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <timer>: ����������״̬��ʱ�䳤�ȣ���λΪms��ȡֵ��Χ 0~65535��Ĭ��Ϊ5000��BALONG��֧�ִ����ָ��������ݲ�����һ��������Ҫ�µ���ָܻ���
     *     <switch_duration>: ָ��PC�·������������������״̬���л�ʱ�䣬��λΪms��ȡֵ��ΧΪ 0~65535��Ĭ��Ϊ500��BALONG��֧�ֽ������״̬���л�ʱ�䣬�����ݲ�����
     * [ʾ��]:
     *     �� ���õ���������״̬��ʱ�䳤��Ϊ6000������������״̬���л�ʱ��20
     *       AT^PSTANDBY=6000,20
     *       OK
     *     �� ��������
     *       AT^PSTANDBY=?
     *       ^PSTANDBY: (0-65535), (0-65535)
     *       OK
     */
    { AT_CMD_PSTANDBY,
      AT_SetPstandbyPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^PSTANDBY", (VOS_UINT8 *)"(0-65535),(0-65535)" },
};

/* ע��MMװ��AT����� */
VOS_UINT32 AT_RegisterDeviceMmCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDeviceMmCmdTbl, sizeof(g_atDeviceMmCmdTbl) / sizeof(g_atDeviceMmCmdTbl[0]));
}

