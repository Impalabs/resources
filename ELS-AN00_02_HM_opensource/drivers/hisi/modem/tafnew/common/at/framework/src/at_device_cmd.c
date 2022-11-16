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
#include "at_device_cmd.h"
#include "securec.h"
#include "AtParse.h"
#include "at_check_func.h"
#include "at_mdrv_interface.h"
#include "at_cmd_msg_proc.h"
#include "at_input_proc.h"
#include "at_test_para_cmd.h"
#include "at_mt_msg_proc.h"

#if (FEATURE_LTE == FEATURE_ON)
#include "at_lte_common.h"
#include "nv_stru_lps.h"
#endif

#include "nv_stru_cas.h"


#include "at_device_taf_set_cmd_proc.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_DEVICECMD_C

/*
 * �������ޱ�־�궨��˵��
 * #define CMD_TBL_E5_IS_LOCKED        0x00000001     ����E5�������Ƶ�����
 * #define CMD_TBL_PIN_IS_LOCKED       0x00000002     ����PIN���������Ƶ�����
 * #define CMD_TBL_IS_E5_DOCK          0x00000004     E5 DOCK����
 * #define CMD_TBL_CLAC_IS_INVISIBLE   0x00000008     +CLAC�����в������ʾ������
 */

/*
 * ʾ��: ^CMDX �����ǲ���E5���뱣���������+CLAC�о���������ʱ����ʾ����һ�������ǲ���˫���ŵ��ַ���,
 *      �ڶ��������Ǵ�˫���ŵ��ַ����������������������Ͳ���
 * !!!!!!!!!!!ע��: param1��param2��ʾ����ʵ�ʶ�������ʱӦ��������ļ��(����߽���Ч��)!!!!!!!!!!!!!
 *  {AT_CMD_CMDX,
 *  At_SetCmdxPara, AT_SET_PARA_TIME, At_QryCmdxPara, AT_QRY_PARA_TIME, At_TestCmdxPara, AT_NOT_SET_TIME,
 *  AT_ERROR, CMD_TBL_E5_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
 *  (VOS_UINT8 *)"^CMDX", (VOS_UINT8 *)"(@param1),(param2),(0-255)"},
 */

VOS_VOID AT_GetSpecificPort(VOS_UINT8 portType, VOS_UINT8 aucRewindPortStyle[], VOS_UINT32 *portPos,
                            VOS_UINT32 *portNum)
{
    VOS_UINT32 loop;

    *portPos = AT_DEV_NONE;
    for (loop = 0; loop < AT_SETPORT_PARA_MAX_LEN; loop++) {
        if (aucRewindPortStyle[loop] == 0) {
            break;
        }

        /* �˿�ֵΪucPortType����ʾ�ҵ�ָ���˿ڣ���¼ulLoopΪ�˿���NV���е�ƫ�� */
        if (portType == aucRewindPortStyle[loop]) {
            *portPos = loop;
        }
    }

    /* �˿�ֵΪ0�򵽴�ѭ�����ޣ�ulLoop��Ϊ����Ķ˿����� */
    *portNum = loop;
}

VOS_UINT32 AT_ExistSpecificPort(VOS_UINT8 portType)
{
    AT_DynamicPidType dynamicPidType;
    VOS_UINT32        portPos;
    VOS_UINT32        portNum;

    memset_s(&dynamicPidType, sizeof(dynamicPidType), 0x00, sizeof(dynamicPidType));

    portPos = 0;
    portNum = 0;

    /* ��NV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE��ȡ��ǰ�Ķ˿�״̬ */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE, &dynamicPidType, sizeof(AT_DynamicPidType)) !=
        NV_OK) {
        AT_WARN_LOG("AT_ExistSpecificPort: Read NV fail!");
        return VOS_FALSE;
    }

    /* �ж�DIAG�˿��Ƿ��Ѿ���: �Ѿ�����ֱ�ӷ���AT_OK */
    if (dynamicPidType.nvStatus == VOS_TRUE) {
        /* ��ѯNV��NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE���Ƿ��Ѿ�����DIAG�� */
        AT_GetSpecificPort(portType, dynamicPidType.rewindPortStyle, &portPos, &portNum);

        if (portPos != AT_DEV_NONE) {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

VOS_UINT32 AT_String2Hex(VOS_UINT8 *nptr, VOS_UINT16 len, VOS_UINT32 *rtn)
{
    VOS_UINT32 c      = 0; /* current Char */
    VOS_UINT32 total  = 0; /* current total */
    VOS_UINT32 length = 0; /* current Length */

    for (length = 0; length < len; length++) {
        c = (VOS_UINT32)(*nptr); /* get next Char */
        nptr++;

        if (AT_Char2Hex(&c) != VOS_TRUE) {
            return VOS_ERR;
        }

        if (total > 0x0FFFFFFF) { /* ������ת */
            return VOS_ERR;
        } else {
            total = (total << 4) + c; /* accumulate digit */
        }
    }

    /* return result, negated if necessary */
    *rtn = total;
    return AT_SUCCESS;
}

