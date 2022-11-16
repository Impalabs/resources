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

#include "at_data_drv_cmd_tbl.h"
#include "at_data_drv_set_cmd_proc.h"
#include "at_data_drv_qry_cmd_proc.h"
#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_DATA_DRV_CMD_TBL_C

static const AT_ParCmdElement g_atDataDrvCmdTbl[] = {
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯModem/NDIS֧����̬
     * [˵��]: ���ں�̨��ѯ��Ӧ�Ĳ��ŷ�ʽ��������ֻ�ܲ�ѯ���ŷ�ʽ���������ò��ŷ�ʽ��
     *         ע�⣺^DIALMODE?�����ɿͻ����Ʋ�����Χ����˼Ĭ�Ϸ���0,0��
     * [�﷨]:
     *     [����]: ^DIALMODE?
     *     [���]: <CR><LF>^DIALMODE: <value>,<cdc_spec><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^DIALMODE=?
     *     [���]: <CR><LF>^DIALMODE: (list of supported <value>s), (list of supported <cdc_spec>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: ����ֵ�����ŷ�ʽ��
     *             0��ʹ��Modem���ţ�
     *             1��ʹ��NDIS���ţ�
     *             2��Modem���ź�NDIS���Ź��档
     *     <cdc_spec>: ����ֵ��CDC�淶���϶ȡ�
     *             0��Modem��NDIS�˿ڶ�������CDC�淶��
     *             1����Modem�˿ڷ���CDC�淶��
     *             2����NDIS�˿ڷ���CDC�淶��
     *             3��Modem��NDIS�˿ڶ�����CDC�淶��
     *             4-255��������
     * [ʾ��]:
     *     �� ��ѯ����
     *       AT^DIALMODE?
     *       ^DIALMODE: 0,0
     *       OK
     *     �� ��������
     *       AT^DIALMODE=?
     *       ^DIALMODE: (0-2),(0-255)
     *       OK
     */
    { AT_CMD_DIALMODE,
      At_SetDialModePara, AT_NOT_SET_TIME, At_QryDialModePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_STICK,
      (VOS_UINT8 *)"^DIALMODE", (VOS_UINT8 *)"(0-2),(0-255)" },
};

/* ע���������ҵ��AT����� */
VOS_UINT32 AT_RegisterDataDrvCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDataDrvCmdTbl, sizeof(g_atDataDrvCmdTbl) / sizeof(g_atDataDrvCmdTbl[0]));
}

