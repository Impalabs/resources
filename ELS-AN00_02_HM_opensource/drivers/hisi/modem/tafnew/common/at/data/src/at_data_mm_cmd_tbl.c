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

#include "at_data_mm_cmd_tbl.h"
#include "at_data_mm_qry_cmd_proc.h"

#include "securec.h"
#include "AtParse.h"
#include "at_cmd_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_DATA_MM_CMD_TBL_C

static const AT_ParCmdElement g_atDataMmCmdTbl[] = {
#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯPS��ǰʹ�õļ����㷨
     * [˵��]: ������������ѯNR��LTE��WCDMA��GSM��PS��ǰʹ�õļ����㷨��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^CIPHERQRY?
     *     [���]: LTE��WCDMA��GSM�£�
     *             <CR><LF>^CIPHERQRY: <EA><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             NR�£�
     *             <CR><LF>^CIPHERQRY: <EA>,<EA1><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <EA>: ����ֵ��ȡֵ��Χ��0-21��
     *             0��û����������(NR��GSM��WCDMA��LTEģʽ��ʹ��)��
     *             1��GEA1(GSMģʽ��ʹ��)��
     *             2��GEA2(GSMģʽ��ʹ��)��
     *             3��GEA3(GSMģʽ��ʹ��)��
     *             4��GEA4(GSMģʽ��ʹ��)��
     *             5��GEA5(GSMģʽ��ʹ��)��
     *             6��GEA6(GSMģʽ��ʹ��)��
     *             7��GEA7(GSMģʽ��ʹ��)��
     *             8��UEA1(WCDMAģʽ��ʹ��)��
     *             9��UEA2(WCDMAģʽ��ʹ��)��
     *             10��UEA3(WCDMA ģʽ��ʹ��)��
     *             11��UEA4(WCDMA ģʽ��ʹ��);
     *             12��UEA5(WCDMA ģʽ��ʹ��)��
     *             13��UEA6(WCDMA ģʽ��ʹ��);
     *             14��UEA7(WCDMA ģʽ��ʹ��)��
     *             15��EEA1(LTE ģʽ��ʹ��)��
     *             16��EEA2(LTE ģʽ��ʹ��);
     *             17��EEA3(LTE ģʽ��ʹ��)��
     *             18��EEA4(LTE ģʽ��ʹ��);
     *             19��EEA5(LTE ģʽ��ʹ��)��
     *             20��EEA6(LTE ģʽ��ʹ��);
     *             21��EEA7(LTE ģʽ��ʹ��)��
     *             22��NEA1(NR ģʽ��ʹ��)��
     *             23��NEA2(NR ģʽ��ʹ��)��
     *             24��NEA3(NR ģʽ��ʹ��)��
     *             25��SPARE4(NR ģʽ��ʹ��)��
     *             26��SPARE4 (NR ģʽ��ʹ��)��
     *             27��SPARE4 (NR ģʽ��ʹ��)��
     *             28��SPARE4 (NR ģʽ��ʹ��)��
     *     <EA1>: 0��û����������(NR ģʽ��ʹ��)��
     *             22��NEA1(NR ģʽ��ʹ��)��
     *             23��NEA2(NR ģʽ��ʹ��)��
     *             24��NEA3(NR ģʽ��ʹ��)��
     *             25��SPARE4(NR ģʽ��ʹ��)��
     *             26��SPARE4 (NR ģʽ��ʹ��)��
     *             27��SPARE4 (NR ģʽ��ʹ��)��
     *             28��SPARE4 (NR ģʽ��ʹ��)��
     * [ʾ��]:
     *     �� ��ѯPS��ǰʹ�õļ����㷨ΪUEA1
     *       AT^CIPHERQRY?
     *       ^CIPHERQRY: 8
     *       OK
     *     �� ��ѯNR�µ�ǰʹ�õļ����㷨����С����(MCG)��NEA1 ��С����(SCG)��NEA2
     *       AT^CIPHERQRY?
     *       ^CIPHERQRY: 22,23
     *       OK
     */
    { AT_CMD_CIPHERQRY,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryCipherPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CIPHERQRY", VOS_NULL_PTR },
#endif
};

/* ע��MM�������ҵ��AT����� */
VOS_UINT32 AT_RegisterDataMmCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDataMmCmdTbl, sizeof(g_atDataMmCmdTbl) / sizeof(g_atDataMmCmdTbl[0]));
}

