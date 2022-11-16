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
#include "at_msg_print.h"
#include "securec.h"
#include "at_cmd_proc.h"
#include "product_config.h"
#include "at_check_func.h"


#define THIS_FILE_ID PS_FILE_ID_AT_MSG_PRINT_C


#define AT_BYTE_ALIGNMENT_VALUE 4  /* ���õ���Ĵ������ݷ��ͽӿ�ʱ���������ָ�����Ϊ4�ֽڶ����ʽ */

/*
 * 3���Ͷ���
 */
VOS_UINT32 AT_AsciiToBcdCode(VOS_CHAR asciiCode, VOS_UINT8 *bcdCode)
{
    if (bcdCode == VOS_NULL_PTR) {
        AT_NORM_LOG("AT_AsciiToBcdCode: Parameter of the function is null.");
        return MN_ERR_NULLPTR;
    }

    if ((asciiCode >= '0') && (asciiCode <= '9')) {
        *bcdCode = (VOS_UINT8)(asciiCode - '0');
    } else if (asciiCode == '*') {
        *bcdCode = 0x0a;
    } else if (asciiCode == '#') {
        *bcdCode = 0x0b;
    } else if ((asciiCode == 'a') || (asciiCode == 'b') || (asciiCode == 'c')) {
        *bcdCode = (VOS_UINT8)((asciiCode - 'a') + 0x0c);
    } else if ((asciiCode == 'A') || (asciiCode == 'B') || (asciiCode == 'C')) {
        *bcdCode = (VOS_UINT8)((asciiCode - 'A') + 0x0c);
    }
    else if (asciiCode == '+') {
        return MN_ERR_PLUS_SIGN;
    }
    else {
        AT_NORM_LOG("AT_AsciiToBcdCode: Parameter of the function is invalid.");
        return MN_ERR_INVALID_ASCII;
    }

    return MN_ERR_NO_ERROR;
}

VOS_UINT32 AT_AsciiNumberToBcd(const VOS_CHAR *pcAsciiNumber, VOS_UINT8 *bcdNumber, VOS_UINT8 *bcdLen)
{
    VOS_UINT8 inputLoop;
    VOS_UINT8 outputLoop;
    VOS_UINT8  bcdCode = 0;
    VOS_UINT32 ret;

    if ((pcAsciiNumber == TAF_NULL_PTR) || (bcdNumber == TAF_NULL_PTR) || (bcdLen == TAF_NULL_PTR)) {
        AT_NORM_LOG("AT_AsciiNumberToBcd: Parameter of the function is null.");
        return MN_ERR_NULLPTR;
    }

    for (inputLoop = 0, outputLoop = 0; pcAsciiNumber[inputLoop] != '\0'; inputLoop++) {
        ret = AT_AsciiToBcdCode(pcAsciiNumber[inputLoop], &bcdCode);

        /* AT_IsDCmdValidChar��ǰ�ǽ������е�+�˵� */
        /* �����������˵�����ΪBCD����û�ж�+�ı�ʾ */
        if ((ret == MN_ERR_INVALID_ASCII) || (ret == MN_ERR_NULLPTR)) {
            return ret;
        } else if (ret == MN_ERR_PLUS_SIGN) {
            continue;
        } else {
            /* for lint */
        }

        /* ����ǰ��Ҫ����Ŀռ���0 */
        /* һ���ֽڵ���λ�͸���λ������һλbcd������ */
        bcdNumber[(outputLoop / 2)] &= (VOS_UINT8)(((outputLoop % 2) == 1) ? 0x0F : 0xF0);

        /* ������������Ӧ�Ŀռ� */
        bcdNumber[(outputLoop / 2)] |= (((outputLoop % 2) == 1) ? ((bcdCode << 4) & 0xF0) : (bcdCode & 0x0F));

        outputLoop++;
    }

    /* �������Ϊ�����������һ���ַ���Ҫ�� F */
    if ((outputLoop % 2) == 1) {
        bcdNumber[(outputLoop / 2)] |= 0xF0;
    }

    *bcdLen = (outputLoop + 1) / 2;

    return MN_ERR_NO_ERROR;
}

VOS_UINT32 AT_BcdToAsciiCode(VOS_UINT8 bcdCode, VOS_CHAR *pcAsciiCode)
{
    VOS_CHAR asciiCode;

    if (pcAsciiCode == TAF_NULL_PTR) {
        AT_NORM_LOG("AT_BcdToAsciiCode: Parameter of the function is null.");
        return MN_ERR_NULLPTR;
    }

    if (bcdCode <= 0x09) {
        asciiCode = (VOS_CHAR)(bcdCode + 0x30);
    } else if (bcdCode == 0x0A) {
        asciiCode = (VOS_CHAR)(bcdCode + 0x20); /* �ַ�'*' */
    } else if (bcdCode == 0x0B) {
        asciiCode = (VOS_CHAR)(bcdCode + 0x18); /* �ַ�'#' */
    } else if ((bcdCode == 0x0C) || (bcdCode == 0x0D) || (bcdCode == 0x0E)) {
        asciiCode = (VOS_CHAR)(bcdCode + 0x55); /* �ַ�'a', 'b', 'c' */
    } else {
        AT_NORM_LOG("AT_BcdToAsciiCode: Parameter of the function is invalid.");
        return MN_ERR_INVALID_BCD;
    }

    *pcAsciiCode = asciiCode;

    return MN_ERR_NO_ERROR;
}

VOS_UINT32 AT_BcdNumberToAscii(const VOS_UINT8 *bcdNumber, VOS_UINT8 bcdLen, VOS_CHAR *pcAsciiNumber)
{
    VOS_UINT8  loop;
    VOS_UINT8  len;
    VOS_UINT8  bcdCode;
    VOS_UINT32 ret;

    if ((bcdNumber == TAF_NULL_PTR) || (pcAsciiNumber == TAF_NULL_PTR)) {
        AT_NORM_LOG("AT_BcdNumberToAscii: Parameter of the function is null.");
        return MN_ERR_NULLPTR;
    }

    /* ��������ַ�����ȥ����Ч��0XFF���� */
    while (bcdLen > 1) {
        if (bcdNumber[bcdLen - 1] == 0xFF) {
            bcdLen--;
        } else {
            break;
        }
    }

    /*
     * �ж�pucBcdAddress��ָ����ַ��������һ���ֽڵĸ�λ�Ƿ�Ϊ1111��
     * ����ǣ�˵������λ��Ϊ����������Ϊż��
     */
    if ((bcdNumber[bcdLen - 1] & 0xF0) == 0xF0) {
        len = (VOS_UINT8)((bcdLen * 2) - 1);
    } else {
        len = (VOS_UINT8)(bcdLen * 2);
    }

    /* �������� */
    for (loop = 0; loop < len; loop++) {
        /* �жϵ�ǰ�����������λ���뻹��ż��λ���룬��0��ʼ����ż�� */
        if ((loop % 2) == 1) {
            /* ���������λ���룬��ȡ��4λ��ֵ */
            bcdCode = ((bcdNumber[(loop / 2)] >> 4) & 0x0F);
        } else {
            /* �����ż��λ���룬��ȡ��4λ��ֵ */
            bcdCode = (bcdNumber[(loop / 2)] & 0x0F);
        }

        /* ������������ת����Ascii����ʽ */
        ret = AT_BcdToAsciiCode(bcdCode, &(pcAsciiNumber[loop]));
        if (ret != MN_ERR_NO_ERROR) {
            return ret;
        }
    }

    pcAsciiNumber[loop] = '\0'; /* �ַ���ĩβΪ0 */

    return MN_ERR_NO_ERROR;
}

/*
 * History:
 *  1.Date: 2005-04-19
 *    Author:
 *    Modification: Created function
 */
TAF_UINT32 At_CheckNumLen(TAF_UINT16 max, TAF_UINT16 len)
{
    /* ������������ֱ�ӷ��ش��� */
    if (g_atCscsType == AT_CSCS_UCS2_CODE) {
        if ((max * AT_BYTE_ALIGNMENT_VALUE) < len) {
            return AT_FAILURE;
        }
    } else {
        if (max < len) {
            return AT_FAILURE;
        }
    }
    return AT_SUCCESS;
}

VOS_VOID AT_JudgeIsPlusSignInDialString(const VOS_CHAR *pcAsciiNumber, VOS_UINT16 len, VOS_UINT8 *isExistPlusSign,
                                        VOS_UINT8 *plusSignLocation)
{
    VOS_UINT8 loop;

    for (loop = 0; loop < len && pcAsciiNumber[loop] != '\0'; loop++) {
        if (pcAsciiNumber[loop] == '+') {
            *isExistPlusSign  = VOS_TRUE;
            *plusSignLocation = loop;
            break;
        }
    }
}

