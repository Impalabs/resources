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
#include "at_custom_lnas_set_cmd_proc.h"
#include "securec.h"

#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_lte_common.h"
#include "css_at_interface.h"
#include "throt_at_interface.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_LNAS_SET_CMD_PROC_C

#define AT_CSS_MCCINFO_SET_REQ_BUFF_LEN 4
#define AT_CSS_CLOUDDATA_SET_REQ_BUFF_LEN 4
#define AT_MCCFREQ_MIN_PARA_NUM 4
#define AT_MCCFREQ_MAX_PARA_NUM 7
#define AT_CLOUDDATA_MIN_PARA_NUM 2
#define AT_CLOUDDATA_MAX_PARA_NUM 4
#define AT_MCCFREQ_SEQ 0
#define AT_MCCFREQ_VER 1
#define AT_MCCFREQ_OPERATION 2
#define AT_MCCFREQ_LENGTH 3
#define AT_MCCFREQ_BS1 4
#define AT_MCCFREQ_BS2 5
#define AT_MCCFREQ_BS3 6
#define AT_BLOCKCELLLIST_OPERATION 2
#define AT_BLOCKCELLLIST_SYSMODE 3
#define AT_BLOCKCELLLIST_LENGTH 4
#define AT_BLOCKCELLLIST_BS1 5
#define AT_BLOCKCELLLIST_BS2 6
#define AT_BLOCKCELLLIST_BS3 7
#define AT_CLOUDDATA_PARA_LENGTH 0
#define AT_CLOUDDATA_PARA_BS1 1
#define AT_CLOUDDATA_PARA_BS2 2
#define AT_CLOUDDATA_PARA_BS3 3
#define AT_BLOCKCELLLIST_MIN_PARA_NUM 5
#define AT_BLOCKCELLLIST_MAX_PARA_NUM 8
#define AT_BLOCKCELLLIST_SEQ 0
#define AT_BLOCKCELLLIST_VER 1
#define AT_CSS_BLOCK_CELL_LIST_SET_REQ_BUFF_LEN 4
#define AT_MTA_COEXPARA_RESERVED_LEN 2

VOS_UINT32 AT_CheckMccFreqPara(VOS_VOID)
{
    VOS_UINT32 loop;
    VOS_UINT32 length;

    /* ������������ȷ,4~7������ */
    if ((g_atParaIndex > AT_MCCFREQ_MAX_PARA_NUM) || (g_atParaIndex < AT_MCCFREQ_MIN_PARA_NUM)) {
        AT_ERR_LOG("AT_CheckMccFreqPara:number of parameter error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������Ȳ��� */
    if ((g_atParaList[AT_MCCFREQ_SEQ].paraLen == 0) || (g_atParaList[AT_MCCFREQ_VER].paraLen != MCC_INFO_VERSION_LEN) ||
        (g_atParaList[AT_MCCFREQ_OPERATION].paraLen == 0) || (g_atParaList[AT_MCCFREQ_LENGTH].paraLen == 0)) {
        AT_ERR_LOG("AT_CheckMccFreqPara:para len error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �����ɾ�����е���Ӫ��Ԥ��Ƶ���Ƶ����Ϣ������ҪЯ��BS1��BS2��BS3����length����Ϊ0 */
    if ((g_atParaList[AT_MCCFREQ_OPERATION].paraValue == AT_CSS_SET_MCC_TYPE_DELETE_ALL_MCC) &&
        ((g_atParaIndex > AT_MCCFREQ_MIN_PARA_NUM) || (g_atParaList[AT_MCCFREQ_LENGTH].paraValue != 0))) {
        AT_ERR_LOG("AT_CheckMccFreqPara:too mang para when delete all mcc.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    length = 0;

    /* ��ȡЯ��BS1��BS2��BS3�����ܳ���,BS1�ǵ������������g_atParaList[4]������BS1�����Ϣ */
    for (loop = 4; loop < g_atParaIndex; loop++) {
        /*
         * ��������BS1������BS2������BS3���ڿն����߳��ȴ���500��ֱ�ӷ��ش���
         * ����AT^MCCFREQ=255,"00.00.001",0,100,,BS2(����BS1���ڿն�,����Ϊ0)
         * AT^MCCFREQ=255,"00.00.001",0,100,BS1,    (����BS2���ڿն�,����Ϊ0)
         */
        if ((g_atParaList[loop].paraLen == 0) || (g_atParaList[loop].paraLen > AT_CMD_MCCFREQ_BS_MAX_LENGTH)) {
            AT_ERR_LOG("AT_CheckMccFreqPara: string BS format is error");

            return AT_CME_INCORRECT_PARAMETERS;
        }

        length += g_atParaList[loop].paraLen;
    }

    /* ���BS1��BS2��BS3�����ܳ�����AT������Я����LENGTH���Ȳ���ͬ��ֱ�ӷ��ش��� */
    if (length != g_atParaList[AT_MCCFREQ_LENGTH].paraValue) {
        AT_ERR_LOG("AT_CheckMccFreqPara: total length is error");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_SetMccFreqPara(VOS_UINT8 indexNum)
{
    AT_CSS_MccInfoSetReq *msg = VOS_NULL_PTR;
    VOS_UINT32            bufLen;
    ModemIdUint16 modemId = MODEM_ID_0;
    VOS_UINT32    ret;
    VOS_UINT32    result;
    errno_t       memResult;

    ret = AT_GetModemIdFromClient(indexNum, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("AT_SetMccFreqPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* �������ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_ERR_LOG("AT_SetMccFreqPara:Cmd Opt Type is wrong.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������� */
    result = AT_CheckMccFreqPara();
    if (result != AT_SUCCESS) {
        AT_ERR_LOG("AT_SetMccFreqPara:check mcc freq para error.");

        return result;
    }

    /* Ԥ��Ƶ����Ϣ���� */
    bufLen = sizeof(AT_CSS_MccInfoSetReq);

    if (g_atParaList[AT_MCCFREQ_LENGTH].paraValue > AT_CSS_MCCINFO_SET_REQ_BUFF_LEN) {
        bufLen += g_atParaList[AT_MCCFREQ_LENGTH].paraValue - AT_CSS_MCCINFO_SET_REQ_BUFF_LEN;
    }

    /* ������Ϣ��AT_CSS_MccInfoSetReq */
    msg = (AT_CSS_MccInfoSetReq *)AT_ALLOC_MSG_WITH_HDR(bufLen);

    /* �ڴ�����ʧ�ܣ����� */
    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetMccFreqPara:memory alloc fail.");
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_INT8 *)msg + VOS_MSG_HEAD_LENGTH, bufLen - VOS_MSG_HEAD_LENGTH, 0x00,
                       bufLen - VOS_MSG_HEAD_LENGTH);
    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, PS_PID_CSS, ID_AT_CSS_MCC_INFO_SET_REQ);

    /* ��д��Ϣ���� */
    msg->modemId = modemId;
    msg->clientId       = g_atClientTab[indexNum].clientId;
    msg->seq            = (VOS_UINT8)g_atParaList[0].paraValue;
    msg->operateType    = (VOS_UINT8)g_atParaList[AT_MCCFREQ_OPERATION].paraValue;
    msg->mccInfoBuffLen = g_atParaList[AT_MCCFREQ_LENGTH].paraValue;

    if (g_atParaList[1].paraLen > 0) {
        memResult = memcpy_s(msg->versionId, sizeof(msg->versionId), g_atParaList[1].para, g_atParaList[1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(msg->versionId), g_atParaList[1].paraLen);
    }

    if (msg->mccInfoBuffLen > 0) {
        /* BS1 */
        if (g_atParaList[AT_MCCFREQ_BS1].paraLen > 0) {
            memResult = memcpy_s(msg->mccInfoBuff, msg->mccInfoBuffLen, g_atParaList[AT_MCCFREQ_BS1].para,
                                 g_atParaList[AT_MCCFREQ_BS1].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, msg->mccInfoBuffLen, g_atParaList[AT_MCCFREQ_BS1].paraLen);
        }

        /* BS1 δЯ�������е�Ƶ��Ƶ����Ϣ��BS2 Я���˲�����Ϣ */
        if (msg->mccInfoBuffLen > g_atParaList[AT_MCCFREQ_BS1].paraLen) {
            if (g_atParaList[AT_MCCFREQ_BS2].paraLen > 0) {
                memResult = memcpy_s(msg->mccInfoBuff + g_atParaList[AT_MCCFREQ_BS1].paraLen,
                                     msg->mccInfoBuffLen - g_atParaList[AT_MCCFREQ_BS1].paraLen,
                                     g_atParaList[AT_MCCFREQ_BS2].para, g_atParaList[AT_MCCFREQ_BS2].paraLen);
                TAF_MEM_CHK_RTN_VAL(memResult, msg->mccInfoBuffLen - g_atParaList[AT_MCCFREQ_BS1].paraLen,
                                    g_atParaList[AT_MCCFREQ_BS2].paraLen);
            }

            /* BS1 & BS2 δЯ�������е�Ƶ��Ƶ����Ϣ��BS3 Я���˲�����Ϣ */
            if (msg->mccInfoBuffLen > ((VOS_UINT32)g_atParaList[AT_MCCFREQ_BS1].paraLen +
                g_atParaList[AT_MCCFREQ_BS2].paraLen)) {
                if (g_atParaList[AT_MCCFREQ_BS3].paraLen > 0) {
                    memResult = memcpy_s(msg->mccInfoBuff + g_atParaList[AT_MCCFREQ_BS1].paraLen +
                                         g_atParaList[AT_MCCFREQ_BS2].paraLen, msg->mccInfoBuffLen -
                                         g_atParaList[AT_MCCFREQ_BS1].paraLen - g_atParaList[AT_MCCFREQ_BS2].paraLen,
                                         g_atParaList[AT_MCCFREQ_BS3].para, g_atParaList[AT_MCCFREQ_BS3].paraLen);
                    TAF_MEM_CHK_RTN_VAL(memResult,
                                        msg->mccInfoBuffLen - g_atParaList[AT_MCCFREQ_BS1].paraLen -
                                        g_atParaList[AT_MCCFREQ_BS2].paraLen, g_atParaList[AT_MCCFREQ_BS3].paraLen);
                }
            }
        }
    }
    /* ������Ϣ��������������״̬ */

    AT_SEND_MSG(msg);

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_MCCFREQ_SET;

    return AT_WAIT_ASYNC_RETURN;
}

LOCAL VOS_UINT32 AT_CheckCloudDataPara(VOS_VOID)
{
    VOS_UINT32 loop;
    VOS_UINT32 length;

    /* ������������ȷ,2~4������ */
    if ((g_atParaIndex > AT_CLOUDDATA_MAX_PARA_NUM) || (g_atParaIndex < AT_CLOUDDATA_MIN_PARA_NUM)) {
        AT_ERR_LOG("AT_CheckCloudDataPara:number of parameter error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������Ȳ��� */
    if (g_atParaList[AT_CLOUDDATA_PARA_LENGTH].paraLen == 0) {
        AT_ERR_LOG("AT_CheckCloudDataPara:para len error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

   /* �������lengthֵ0��ֱ�ӷ��ش��� */
    if (g_atParaList[AT_CLOUDDATA_PARA_LENGTH].paraValue == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    length = 0;

    /* ��ȡЯ��BS1��BS2��BS3�����ܳ���,BS1�ǵڶ�����������g_atParaList[1]������BS1�����Ϣ */
    for (loop = 1; loop < g_atParaIndex; loop++) {
        /*
         * ��������BS1������BS2������BS3���ڿն����߳��ȴ���500��ֱ�ӷ��ش���
         * ����AT^CLOUDDATA=100,,BS2(����BS1���ڿն�,����Ϊ0)
         * AT^CLOUDDATA=100,BS1,    (����BS2���ڿն�,����Ϊ0)
         */
        if ((g_atParaList[loop].paraLen == 0) || (g_atParaList[loop].paraLen > AT_CMD_CLOUDDATA_BS_MAX_LENGTH)) {
            AT_ERR_LOG("AT_CheckCloudDataPara: string BS format is error");

            return AT_CME_INCORRECT_PARAMETERS;
        }

        length += g_atParaList[loop].paraLen;
    }

    /* ���BS1��BS2��BS3�����ܳ�����AT������Я����LENGTH���Ȳ���ͬ��ֱ�ӷ��ش��� */
    if (length != g_atParaList[AT_CLOUDDATA_PARA_LENGTH].paraValue) {
        AT_ERR_LOG("AT_CheckCloudDataPara: total length is error");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}

VOS_VOID AT_FillCloudDataPara(VOS_UINT32 dataLen, VOS_UINT8 *data)
{
    errno_t       memResult;

    /* BS1 */
    if (g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen > 0) {
        memResult = memcpy_s(data, dataLen, g_atParaList[AT_CLOUDDATA_PARA_BS1].para,
            g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, dataLen, g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen);
    }

    /* BS1 δЯ�������е�Ƶ��Ƶ����Ϣ��BS2 Я���˲�����Ϣ */
    if (dataLen > g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen) {
        if (g_atParaList[AT_CLOUDDATA_PARA_BS2].paraLen > 0) {
            memResult = memcpy_s(data + g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen,
                dataLen - g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen,
                g_atParaList[AT_CLOUDDATA_PARA_BS2].para, g_atParaList[AT_CLOUDDATA_PARA_BS2].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, dataLen - g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen,
                g_atParaList[AT_CLOUDDATA_PARA_BS2].paraLen);
        }
    }

    /* BS1 & BS2 δЯ�������е�Ƶ��Ƶ����Ϣ��BS3 Я���˲�����Ϣ */
    if (dataLen > ((VOS_UINT32)g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen +
        g_atParaList[AT_CLOUDDATA_PARA_BS2].paraLen)) {
        if (g_atParaList[AT_CLOUDDATA_PARA_BS3].paraLen > 0) {
            memResult = memcpy_s(data + g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen +
                g_atParaList[AT_CLOUDDATA_PARA_BS2].paraLen, dataLen - g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen -
                g_atParaList[AT_CLOUDDATA_PARA_BS2].paraLen, g_atParaList[AT_CLOUDDATA_PARA_BS3].para,
                g_atParaList[AT_CLOUDDATA_PARA_BS3].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, dataLen - g_atParaList[AT_CLOUDDATA_PARA_BS1].paraLen -
                g_atParaList[ AT_CLOUDDATA_PARA_BS2].paraLen, g_atParaList[AT_CLOUDDATA_PARA_BS3].paraLen);
        }
    }
}

VOS_UINT32 AT_SetCloudDataPara(VOS_UINT8 indexNum)
{
    AT_CSS_CloudDataSetReq *msg = VOS_NULL_PTR;
    VOS_UINT32              bufLen;
    VOS_UINT32              ret;
    VOS_UINT32              result;
    ModemIdUint16           modemId = MODEM_ID_0;

    ret = AT_GetModemIdFromClient(indexNum, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("AT_SetCloudDataPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* �������ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_ERR_LOG("AT_SetCloudDataPara:Cmd Opt Type is wrong.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������� */
    result = AT_CheckCloudDataPara();
    if (result != AT_SUCCESS) {
        AT_ERR_LOG("AT_SetCloudDataPara:check cloud data para error.");

        return result;
    }

    /* Ԥ��Ƶ����Ϣ���� */
    bufLen = sizeof(AT_CSS_CloudDataSetReq);

    if (g_atParaList[AT_CLOUDDATA_PARA_LENGTH].paraValue > AT_CSS_CLOUDDATA_SET_REQ_BUFF_LEN) {
        bufLen += g_atParaList[AT_CLOUDDATA_PARA_LENGTH].paraValue - AT_CSS_CLOUDDATA_SET_REQ_BUFF_LEN;
    }

    /* ������Ϣ��AT_CSS_CloudDataSetReq */
    msg = (AT_CSS_CloudDataSetReq *)AT_ALLOC_MSG_WITH_HDR(bufLen);

    /* �ڴ�����ʧ�ܣ����� */
    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetCloudDataPara:memory alloc fail.");
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)msg + VOS_MSG_HEAD_LENGTH, bufLen - VOS_MSG_HEAD_LENGTH, 0x00, bufLen -
        VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, PS_PID_CSS, ID_AT_CSS_CLOUD_DATA_SET_REQ);

    /* ��д��Ϣ���� */
    msg->modemId  = modemId;
    msg->clientId = g_atClientTab[indexNum].clientId;
    msg->dataLen  = g_atParaList[AT_CLOUDDATA_PARA_LENGTH].paraValue;

    /* ��д��������BS1��BS2��BS3���� */
    AT_FillCloudDataPara(msg->dataLen, msg->data);
    /* ������Ϣ��������������״̬ */

    AT_SEND_MSG(msg);

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CLOUDDATA_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_CheckBlockCellListBsPara(VOS_VOID)
{
    VOS_UINT32 loop;
    VOS_UINT32 length = 0;

    /* ��ȡЯ��BS1��BS2��BS3�����ܳ���,BS1�ǵ�������������g_atParaList[5]������BS1�����Ϣ */
    for (loop = 5; loop < g_atParaIndex; loop++) {
        /*
         * ��������BS1������BS2������BS3���ڿն����߳��ȴ���500��ֱ�ӷ��ش���
         * ����AT^CLOUDBLOCKLIST=255,"00.00.001",0,0,504,,BS2 (����BS1���ڿն�,����Ϊ0),
         * AT^CLOUDBLOCKLIST=255,"00.00.001",0,0,504,BS1,     (����BS2���ڿն�,����Ϊ0)
         */
        if ((g_atParaList[loop].paraLen == 0) || (g_atParaList[loop].paraLen > AT_CMD_BLOCK_CELL_BS_MAX_LENGTH)) {
            AT_ERR_LOG("AT_CheckBlockCellListBsPara: input BS format is error");

            return AT_CME_INCORRECT_PARAMETERS;
        }

        length += g_atParaList[loop].paraLen;
    }

    /* ���BS1��BS2��BS3�����ܳ�����AT������Я����LENGTH���Ȳ���ͬ��ֱ�ӷ��ش��� */
    if (length != g_atParaList[AT_BLOCKCELLLIST_LENGTH].paraValue) {
        AT_ERR_LOG("AT_CheckBlockCellListBsPara: total length is error");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_CheckBlockCellListPara(VOS_VOID)
{
    /* ������������ȷ,5~8������ */
    if ((g_atParaIndex > AT_BLOCKCELLLIST_MAX_PARA_NUM) || (g_atParaIndex < AT_BLOCKCELLLIST_MIN_PARA_NUM)) {
        AT_ERR_LOG("AT_CheckBlockCellListPara:number of parameter error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������Ȳ��� */
    if ((g_atParaList[AT_BLOCKCELLLIST_SEQ].paraLen == 0) ||
        (g_atParaList[AT_BLOCKCELLLIST_VER].paraLen != AT_CSS_BLOCK_CELL_LIST_VERSION_LEN) ||
        (g_atParaList[AT_BLOCKCELLLIST_OPERATION].paraLen == 0) ||
        (g_atParaList[AT_BLOCKCELLLIST_SYSMODE].paraLen   == 0) ||
        (g_atParaList[AT_BLOCKCELLLIST_LENGTH].paraLen    == 0)) {
        AT_ERR_LOG("AT_CheckBlockCellListPara:para len error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �����ɾ�����е���ͨ��BlockС����Ϣ������ҪЯ��BS1��BS2��BS3����length����Ϊ0 */
    if ((g_atParaList[AT_BLOCKCELLLIST_OPERATION].paraValue == AT_CSS_BLOCK_CELL_LIST_DELETE_ALL) &&
        ((g_atParaIndex > AT_BLOCKCELLLIST_MIN_PARA_NUM) || (g_atParaList[AT_BLOCKCELLLIST_LENGTH].paraValue != 0))) {
        AT_ERR_LOG("AT_CheckBlockCellListPara:too mang para when delete all cloud block list.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �����BS��ʽ����ȷ���������ڿն���BS�ܳ��Ȳ�����length���� */
    if (AT_CheckBlockCellListBsPara() != AT_SUCCESS) {
        AT_ERR_LOG("AT_CheckBlockCellListPara:input BS para is error.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}

LOCAL VOS_VOID AT_SetBlockCellListBsInfo(AT_CSS_BlockCellListSetReq *msg)
{
    errno_t memResult;

    if (msg->blockCellListBuffLen > 0) {
        /* BS1 */
        if (g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen > 0) {
            memResult = memcpy_s(msg->blockCellListBuff, msg->blockCellListBuffLen,
                                 g_atParaList[AT_BLOCKCELLLIST_BS1].para, g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, msg->blockCellListBuffLen, g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen);
        }

        /* BS1δЯ�������е���ͨ��BlockС����Ϣ��BS2Я���˲�����Ϣ */
        if (msg->blockCellListBuffLen > g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen) {
            if (g_atParaList[AT_BLOCKCELLLIST_BS2].paraLen > 0) {
                memResult = memcpy_s(msg->blockCellListBuff + g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen,
                                     msg->blockCellListBuffLen - g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen,
                                     g_atParaList[AT_BLOCKCELLLIST_BS2].para,
                                     g_atParaList[AT_BLOCKCELLLIST_BS2].paraLen);
                TAF_MEM_CHK_RTN_VAL(memResult, msg->blockCellListBuffLen - g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen,
                                    g_atParaList[AT_BLOCKCELLLIST_BS2].paraLen);
            }

            /* BS1 & BS2δЯ�������е���ͨ��BlockС����Ϣ��BS3Я���˲�����Ϣ */
            if (msg->blockCellListBuffLen >
                ((VOS_UINT32)g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen +
                 g_atParaList[AT_BLOCKCELLLIST_BS2].paraLen)) {
                if (g_atParaList[AT_BLOCKCELLLIST_BS3].paraLen > 0) {
                    memResult = memcpy_s(msg->blockCellListBuff + g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen +
                                         g_atParaList[AT_BLOCKCELLLIST_BS2].paraLen,
                                         msg->blockCellListBuffLen - g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen -
                                         g_atParaList[AT_BLOCKCELLLIST_BS2].paraLen,
                                         g_atParaList[AT_BLOCKCELLLIST_BS3].para,
                                         g_atParaList[AT_BLOCKCELLLIST_BS3].paraLen);
                    TAF_MEM_CHK_RTN_VAL(memResult,
                                        msg->blockCellListBuffLen - g_atParaList[AT_BLOCKCELLLIST_BS1].paraLen -
                                        g_atParaList[AT_BLOCKCELLLIST_BS2].paraLen,
                                        g_atParaList[AT_BLOCKCELLLIST_BS3].paraLen);
                }
            }
        }
    }
}

VOS_UINT32 AT_SetBlockCellListPara(VOS_UINT8 indexNum)
{
    AT_CSS_BlockCellListSetReq *msg = VOS_NULL_PTR;
    VOS_UINT32                  bufLen;
    VOS_UINT32                  result;
    errno_t                     memResult;

    /* �������ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_ERR_LOG("AT_SetBlockCellListPara:Cmd Opt Type is wrong.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������� */
    result = AT_CheckBlockCellListPara();
    if (result != AT_SUCCESS) {
        AT_ERR_LOG("AT_SetBlockCellListPara:check block cell list para error.");

        return result;
    }

    bufLen = sizeof(AT_CSS_BlockCellListSetReq);

    if (g_atParaList[AT_BLOCKCELLLIST_LENGTH].paraValue > AT_CSS_BLOCK_CELL_LIST_SET_REQ_BUFF_LEN) {
        bufLen += g_atParaList[AT_BLOCKCELLLIST_LENGTH].paraValue - AT_CSS_BLOCK_CELL_LIST_SET_REQ_BUFF_LEN;
    }

    /* ������Ϣ��AT_CSS_CLOUD_BLOCK_LIST_SET_REQ_STRU */
    msg = (AT_CSS_BlockCellListSetReq *)AT_ALLOC_MSG_WITH_HDR(bufLen);

    /* �ڴ�����ʧ�ܣ����� */
    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetBlockCellListPara:memory alloc fail.");

        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_INT8 *)msg + VOS_MSG_HEAD_LENGTH, bufLen - VOS_MSG_HEAD_LENGTH, 0x00,
        bufLen - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, PS_PID_CSS, ID_AT_CSS_BLOCK_CELL_LIST_SET_REQ);

    /* ��д��Ϣ���� */
    msg->clientId = g_atClientTab[indexNum].clientId;
    msg->seq      = (VOS_UINT8)g_atParaList[0].paraValue;

    if (g_atParaList[1].paraLen > 0) {
        memResult = memcpy_s(msg->versionId, sizeof(msg->versionId), g_atParaList[1].para, g_atParaList[1].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(msg->versionId), g_atParaList[1].paraLen);
    }

    msg->operateType          = (VOS_UINT8)g_atParaList[AT_BLOCKCELLLIST_OPERATION].paraValue;
    msg->ratType              = (VOS_UINT8)g_atParaList[AT_BLOCKCELLLIST_SYSMODE].paraValue;
    msg->blockCellListBuffLen = g_atParaList[AT_BLOCKCELLLIST_LENGTH].paraValue;

    AT_SetBlockCellListBsInfo(msg);

    /* ������Ϣ��������������״̬ */
    AT_SEND_MSG(msg);

    /* ���õ�ǰ�������� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_BLOCKCELLLIST_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetGpsLocSetPara(VOS_UINT8 indexNum)
{
    AT_MTA_GpslocsetSetReq atCmd;
    VOS_UINT32             result;

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��������������1��������AT_CME_INCORRECT_PARAMETERS */
    if (g_atParaIndex != 1) {
        AT_WARN_LOG("AT_SetGpsLocSetPara: At Para Num Error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ��1����������Ϊ0������AT_CME_INCORRECT_PARAMETERS */
    if (g_atParaList[0].paraLen == 0) {
        AT_WARN_LOG("AT_SetGpsLocSetPara: para0 Length = 0");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    atCmd.locationPermitFlag = g_atParaList[0].paraValue;

    result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_GPS_LOCSET_SET_REQ,
                                    (VOS_UINT8 *)&atCmd, sizeof(AT_MTA_GpslocsetSetReq), I0_UEPS_PID_MTA);

    if (result != AT_SUCCESS) {
        AT_WARN_LOG("AT_SetGpsLocSetPara: AT_FillAndSndAppReqMsg Failed!");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_GPSLOCSET_SET;

    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetGameModePara(VOS_UINT8 indexNum)
{
    ModemIdUint16             modemId = MODEM_ID_0;
    AT_MTA_CommGameModeSetReq atCmd;

    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        AT_ERR_LOG("AT_SetGameModePara: Get modem id fail.");
        return AT_ERROR;
    }

    if (modemId != MODEM_ID_0) {
        AT_ERR_LOG("enModemId isn't MODEM 0");
        return AT_ERROR;
    }

    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaIndex != 1) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[0].paraLen == 0) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    (VOS_VOID)memset_s(&atCmd, sizeof(atCmd), 0x00, sizeof(atCmd));
    atCmd.gameMode = (VOS_UINT8)g_atParaList[0].paraValue;

    if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, 0, ID_AT_MTA_GAME_MODE_SET_REQ, (VOS_UINT8 *)&atCmd,
                               sizeof(AT_MTA_CommGameModeSetReq), I0_UEPS_PID_MTA) == TAF_SUCCESS) {
        /* ���õ�ǰ�������� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_GAME_MODE_SET;
        return AT_WAIT_ASYNC_RETURN; /* ������������״̬ */
    } else {
        return AT_ERROR;
    }
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
LOCAL VOS_UINT32 AT_CheckNwDeploymentPara(VOS_VOID)
{
    /* ��۲�����������ȷ,4~6������ */
    if ((g_atParaIndex > 6) || (g_atParaIndex < 4)) {
        AT_ERR_LOG("AT_CheckNwDeploymentPara: number of parameter error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ����������Ȳ��� */
    if ((g_atParaList[0].paraLen == 0) || (g_atParaList[1].paraLen != AT_CSS_PLMN_DEPLOYMENT_VERSION_LEN) ||
        (g_atParaList[2].paraLen == 0)) {
        AT_ERR_LOG("AT_CheckNwDeploymentPara: para len error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* BS_countһ����3��ȡֵ����������������ԣ���ô��Ϊ����������󷵻� */
    if (g_atParaIndex != g_atParaList[2].paraValue + 3) {
        AT_ERR_LOG("AT_CheckNwDeploymentPara: number of BS error.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_SUCCESS;
}

LOCAL VOS_UINT32 AT_FormatNwDeploymentMsg(VOS_UINT32 byteStrLen, ModemIdUint16 modemId,
    VOS_UINT8 atIndex)
{
    AT_CSS_PlmnDeploymentInfoSetReq *msg = VOS_NULL_PTR;
    VOS_UINT_PTR                     msgDataAddr;
    VOS_UINT32                       bufLen;
    errno_t                          memResult;

    /* Ԥ��Ƶ����Ϣ���� */
    bufLen = sizeof(AT_CSS_PlmnDeploymentInfoSetReq);

    if (byteStrLen > AT_MSG_DEFAULT_VALUE_LEN) {
        bufLen = bufLen + byteStrLen - AT_MSG_DEFAULT_VALUE_LEN;
    }

    /* ������Ϣ��AT_CSS_PlmnDeploymentInfoSetReq */
    msg = (AT_CSS_PlmnDeploymentInfoSetReq *)AT_ALLOC_MSG_WITH_HDR(bufLen);

    /* �ڴ�����ʧ�ܣ����� */
    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_SetNwDeploymentPara: memory alloc fail.");
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_INT8 *)msg + VOS_MSG_HEAD_LENGTH, (VOS_SIZE_T)bufLen - VOS_MSG_HEAD_LENGTH, 0x00,
                       (VOS_SIZE_T)bufLen - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, PS_PID_CSS, ID_AT_CSS_PLMN_DEPLOYMENT_INFO_SET_REQ);

    /* ��д��Ϣ���� */
    msg->modemId = modemId;
    msg->clientId = g_atClientTab[atIndex].clientId;
    msg->seq = (VOS_UINT8)g_atParaList[0].paraValue;
    msg->dataLen = byteStrLen;

    memResult = memcpy_s(msg->versionId, sizeof(msg->versionId), g_atParaList[1].para, g_atParaList[1].paraLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(msg->versionId), g_atParaList[1].paraLen);

    /* ��дdata[4]������<BS_count>�ĸ�������ֵ */
    msgDataAddr = (VOS_UINT_PTR)msg->data;
    if (g_atParaList[2].paraValue == 1) {
        memResult = memcpy_s((VOS_UINT8*)msgDataAddr, byteStrLen, g_atParaList[3].para, g_atParaList[3].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, byteStrLen, g_atParaList[3].paraLen);
    }else if (g_atParaList[2].paraValue == 2) {
        memResult = memcpy_s((VOS_UINT8*)msgDataAddr, byteStrLen, g_atParaList[3].para, g_atParaList[3].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, byteStrLen, g_atParaList[3].paraLen);

        memResult = memcpy_s(msg->data + g_atParaList[3].paraLen, byteStrLen - g_atParaList[3].paraLen,
                             g_atParaList[4].para, g_atParaList[4].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, byteStrLen - g_atParaList[3].paraLen, g_atParaList[4].paraLen);
    } else {
        memResult = memcpy_s((VOS_UINT8*)msgDataAddr, byteStrLen, g_atParaList[3].para, g_atParaList[3].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, byteStrLen, g_atParaList[3].paraLen);

        memResult = memcpy_s(msg->data + g_atParaList[3].paraLen, byteStrLen - g_atParaList[3].paraLen,
                             g_atParaList[4].para, g_atParaList[4].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, byteStrLen - g_atParaList[3].paraLen, g_atParaList[4].paraLen);

        memResult = memcpy_s(msg->data + g_atParaList[3].paraLen + g_atParaList[4].paraLen,
                             byteStrLen - g_atParaList[3].paraLen - g_atParaList[4].paraLen,
                             g_atParaList[5].para, g_atParaList[5].paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, byteStrLen - g_atParaList[3].paraLen - g_atParaList[4].paraLen,
                            g_atParaList[5].paraLen);
    }

    AT_SEND_MSG(msg);

    return AT_SUCCESS;
}

VOS_UINT32 AT_SetNwDeploymentPara(VOS_UINT8 atIndex)
{
    VOS_UINT32 byteStrLen;
    ModemIdUint16 modemId = MODEM_ID_0;
    VOS_UINT32 ret;
    VOS_UINT32 result;

    ret = AT_GetModemIdFromClient(atIndex, &modemId);

    if (ret != VOS_OK) {
        AT_ERR_LOG("AT_SetNwDeploymentPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* �������ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_ERR_LOG("AT_SetNwDeploymentPara: Cmd Opt Type is wrong.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* ������� */
    result = AT_CheckNwDeploymentPara();
    if (result != AT_SUCCESS) {
        AT_ERR_LOG("AT_SetNwDeploymentPara: check Nw Deployment para error.");
        return result;
    }

    switch (g_atParaList[2].paraValue) {
        case 1: {
            byteStrLen = g_atParaList[3].paraLen;
            break;
        }
        case 2: {
            byteStrLen = g_atParaList[3].paraLen + g_atParaList[4].paraLen;
            break;
        }
        case 3: {
            byteStrLen = g_atParaList[3].paraLen + g_atParaList[4].paraLen + g_atParaList[5].paraLen;
            break;
        }
        default: {
            AT_ERR_LOG("AT_SetNwDeploymentPara: check Nw Deployment para error.");
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    if (AT_FormatNwDeploymentMsg(byteStrLen, modemId, atIndex) != AT_SUCCESS) {
        AT_ERR_LOG("AT_SetNwDeploymentPara: SB is wrong.");
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    g_atClientTab[atIndex].cmdCurrentOpt = AT_CMD_NWDEPLOYMENT_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

#if (FEATURE_LTE == FEATURE_ON)

VOS_INT32 AT_GetIsmCoexParaValue(VOS_UINT8 *begain, VOS_UINT8 **ppEnd)
{
    VOS_UINT32 total    = 0;
    VOS_INT32  rstTotal = 0;
    VOS_UINT32 rst;
    VOS_UINT32 flag = 0;
    VOS_UINT8 *end  = VOS_NULL_PTR;

    end = begain;

    while ((*end != ' ') && (*end != '\0')) {
        end++;
    }

    if (*begain == '-') {
        flag = 1;
        begain++;
    }

    rst = atAuc2ul(begain, (VOS_UINT16)(end - begain), &total);

    if (rst != AT_SUCCESS) {
        rstTotal = AT_COEX_INVALID;
    } else {
        *ppEnd   = (end + 1);
        rstTotal = (VOS_INT32)(flag ? (0 - total) : total);
    }

    return rstTotal;
}

LOCAL VOS_UINT32 AT_CheckCoexEnableValue(VOS_INT32 val)
{
    if ((val < AT_COEX_PARA_COEX_ENABLE_MIN) || (val > AT_COEX_PARA_COEX_ENABLE_MAX)) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

LOCAL VOS_UINT32 AT_CheckTxBeginValue(VOS_INT32 val)
{
    if ((val < AT_COEX_PARA_TX_BEGIN_MIN) || (val > AT_COEX_PARA_TX_BEGIN_MAX)) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

LOCAL VOS_UINT32 AT_CheckTxEndValue(VOS_INT32 val)
{
    if ((val < AT_COEX_PARA_TX_END_MIN) || (val > AT_COEX_PARA_TX_END_MAX)) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

LOCAL VOS_UINT32 AT_CheckTxPowerValue(VOS_INT32 val)
{
    if ((val < AT_COEX_PARA_TX_POWER_MIN) || (val > AT_COEX_PARA_TX_POWER_MAX)) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

LOCAL VOS_UINT32 AT_CheckRxBeginValue(VOS_INT32 val)
{
    if ((val < AT_COEX_PARA_RX_BEGIN_MIN) || (val > AT_COEX_PARA_RX_BEGIN_MAX)) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

LOCAL VOS_UINT32 AT_CheckRxEndValue(VOS_INT32 val)
{
    if ((val < AT_COEX_PARA_RX_END_MIN) || (val > AT_COEX_PARA_RX_END_MAX)) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 AT_CheckIsmCoexParaValue(VOS_INT32 val, VOS_UINT32 paraNum)
{
    VOS_UINT32 rst = AT_SUCCESS;

    switch (paraNum) {
        case AT_COEX_PARA_COEX_ENABLE:
            rst = AT_CheckCoexEnableValue(val);
            break;
        case AT_COEX_PARA_TX_BEGIN:
            rst = AT_CheckTxBeginValue(val);
            break;
        case AT_COEX_PARA_TX_END:
            rst = AT_CheckTxEndValue(val);
            break;
        case AT_COEX_PARA_TX_POWER:
            rst = AT_CheckTxPowerValue(val);
            break;
        case AT_COEX_PARA_RX_BEGIN:
            rst = AT_CheckRxBeginValue(val);
            break;
        case AT_COEX_PARA_RX_END:
            rst = AT_CheckRxEndValue(val);
            break;
        default:
            rst = AT_FAILURE;
            break;
    }

    return rst;
}

VOS_VOID AT_SetL4AIsmCoexParaValue(AT_MTA_LteWifiCoexSetReq ismCoex, L4A_ISMCOEX_Req *reqToL4A, VOS_UINT8 indexNum)
{
    VOS_UINT32 i;

    reqToL4A->ctrl.clientId = g_atClientTab[indexNum].clientId;
    reqToL4A->ctrl.opId = 0;
    reqToL4A->ctrl.pid  = WUEPS_PID_AT;

    for (i = 0; i < AT_MTA_ISMCOEX_BANDWIDTH_NUM; i++) {
        reqToL4A->coex[i].flag    = (VOS_UINT32)ismCoex.coexPara[i].cfg;
        reqToL4A->coex[i].txBegin = (VOS_UINT32)ismCoex.coexPara[i].txBegin;
        reqToL4A->coex[i].txEnd   = (VOS_UINT32)ismCoex.coexPara[i].txEnd;
        reqToL4A->coex[i].txPower = (VOS_INT32)ismCoex.coexPara[i].txPower;
        reqToL4A->coex[i].rxBegin = (VOS_UINT32)ismCoex.coexPara[i].rxBegin;
        reqToL4A->coex[i].rxEnd   = (VOS_UINT32)ismCoex.coexPara[i].rxEnd;
    }
}

VOS_UINT32 AT_SetIsmCoexPara(VOS_UINT8 indexNum)
{
    AT_MTA_LteWifiCoexSetReq ismCoex;
    L4A_ISMCOEX_Req          reqToL4A = {0};
    VOS_UINT32               rst, retTemp;
    VOS_UINT32               i, j;
    VOS_INT32                ret;
    VOS_UINT16              *val  = VOS_NULL_PTR; /* ��Ҫ�洢��ֵָ�� */
    VOS_UINT8               *cur  = VOS_NULL_PTR; /* �����ַ���ʱ�ĵ�ǰָ�� */
    VOS_UINT8               *para = VOS_NULL_PTR; /* �����ַ���ͷָ�� */

    (VOS_VOID)memset_s(&ismCoex, sizeof(ismCoex), 0x00, sizeof(ismCoex));

    /* ������� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex != AT_MTA_ISMCOEX_BANDWIDTH_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    for (i = 0; i < AT_MTA_ISMCOEX_BANDWIDTH_NUM; i++) {
        cur                            = g_atParaList[i].para;
        ismCoex.coexPara[i].coexBwType = (AT_MTA_CoexBwTypeUint16)i;
        val                            = &(ismCoex.coexPara[i].cfg);

        for (j = 0; j < sizeof(AT_MTA_CoexPara) / sizeof(VOS_UINT16) - AT_MTA_COEXPARA_RESERVED_LEN; j++) {
            para = cur;
            ret  = AT_GetIsmCoexParaValue(para, &cur);

            if (ret == AT_COEX_INVALID) {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            if (AT_CheckIsmCoexParaValue(ret, j) == AT_FAILURE) {
                return AT_CME_INCORRECT_PARAMETERS;
            }

            *val = (VOS_UINT16)ret;
            val++;
        }
    }

    ismCoex.coexParaNum  = AT_MTA_ISMCOEX_BANDWIDTH_NUM;
    ismCoex.coexParaSize = sizeof(ismCoex.coexPara);

    AT_SetL4AIsmCoexParaValue(ismCoex, &reqToL4A, indexNum);

    /* ������Ϣ��L4A */
    retTemp = atSendL4aDataMsg(g_atClientTab[indexNum].clientId, I0_MSP_L4_L4A_PID, ID_MSG_L4A_ISMCOEXSET_REQ,
                               (VOS_UINT8 *)(&reqToL4A), sizeof(reqToL4A));

    /* ���Ϳ����Ϣ��C�� */
    rst = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                 ID_AT_MTA_LTE_WIFI_COEX_SET_REQ, (VOS_UINT8 *)&ismCoex, sizeof(ismCoex),
                                 I0_UEPS_PID_MTA);
    if (retTemp != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetIsmCoexPara: atSendDataMsg fail.");
    }

    if (rst != TAF_SUCCESS) {
        AT_WARN_LOG("AT_SetIsmCoexPara: AT_FillAndSndAppReqMsg fail.");
        return AT_ERROR;
    }

    /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_LTE_WIFI_COEX_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

#if (FEATURE_APN_BASED_BO_TIMER_PARA_RPT == FEATURE_ON)
/* ����CABTSRI(backoff��ʱ�������ϱ�)�Ĺ��ܿ���,�����ʽΪ:AT+CABTSR=[<n>] */
VOS_UINT32 At_SetCabtsrPara(VOS_UINT8 indexNum)
{
    AT_THROT_CabtsriConfigSetReq  *msg     = VOS_NULL_PTR;
    VOS_UINT32                     ret;
    ModemIdUint16                  modemId = MODEM_ID_0;

    ret = AT_GetModemIdFromClient(indexNum, &modemId);
    if (ret != VOS_OK) {
        AT_ERR_LOG("At_SetCabtsrPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* �������ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        AT_ERR_LOG("At_SetCabtsrPara:Cmd Opt Type is wrong.");

        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_COMMAND_MAX_PARA_NUM) {
        AT_ERR_LOG("At_SetCabtsrPara:para number is invalid!.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    msg = (AT_THROT_CabtsriConfigSetReq *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_THROT_CabtsriConfigSetReq));
    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("At_SetCabtsrPara:memory alloc fail.");
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)msg + VOS_MSG_HEAD_LENGTH, sizeof(AT_THROT_CabtsriConfigSetReq) -
        VOS_MSG_HEAD_LENGTH, 0x00, sizeof(AT_THROT_CabtsriConfigSetReq) - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, AT_GetDestPid((MN_CLIENT_ID_T)indexNum, I0_PS_PID_NAS_COMM), ID_AT_THROT_CABTSRI_CONFIGURATION_SET_REQ);

    /* ��д��Ϣ���� */
    msg->modemId  = modemId;
    msg->clientId = g_atClientTab[indexNum].clientId;

    /* ������Ϊ�� */
    if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen != 0) {
        msg->isEnableCabtsri = g_atParaList[AT_COMMAND_PARA_INDEX_0].paraValue;
    } else {
        msg->isEnableCabtsri = VOS_FALSE;
    }

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("At_SetCabtsrPara: TAF_TraceAndSendMsg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CABTSR_SET;

    return AT_WAIT_ASYNC_RETURN;
}

/* ��������: ���+CABTRDP��������Ƿ���ȷ */
LOCAL VOS_UINT32 AT_CheckCabtrdpPara(VOS_UINT16 clientId)
{

    /* ���������������������������Ϊ0�Ҳ��ܳ���1 */
    if ((g_atParaIndex == 0) || (g_atParaIndex > AT_COMMAND_MAX_PARA_NUM)) {
        AT_ERR_LOG("AT_CheckCabtrdpPara:para number is invalid!.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* APN���ȼ�� */
    if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen > NAS_THROT_MAX_APN_LEN) {
        AT_NORM_LOG("AT_CheckCabtrdpPara: APN is too long.");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* APN��ʽ��� */
    if ((g_atParaIndex == 1) && (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen != 0)) {
         if (AT_CheckApnFormat(g_atParaList[AT_COMMAND_PARA_INDEX_0].para,
             g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen, clientId) != VOS_OK) {
             AT_NORM_LOG("AT_CheckCabtrdpPara: Format of APN is wrong.");
             return AT_CME_INCORRECT_PARAMETERS;
         }
    }

    return AT_SUCCESS;
}

/* ����+CABTRDP��apn����Ϣ,���ڲ�ѯapn backoff��ʱ����Ϣ,��ʽΪ:AT+CABTRDP[=<apn>] */
VOS_UINT32 At_SetCabtrdpPara(VOS_UINT8 indexNum)
{
    AT_THROT_CabtrdpQueryReq  *msg = VOS_NULL_PTR;
    VOS_UINT32                 ret;
    errno_t                    memResult;
    ModemIdUint16              modemId = MODEM_ID_0;

    ret = AT_GetModemIdFromClient(indexNum, &modemId);
    if (ret != VOS_OK) {
        AT_ERR_LOG("At_SetCabtrdpPara: Get modem id fail.");
        return AT_ERROR;
    }

    /* �����������������Լ�������� */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_PARA_CMD) {
        /* ������� */
        if (AT_CheckCabtrdpPara(g_atClientTab[indexNum].clientId) != AT_SUCCESS) {
            AT_ERR_LOG("At_SetCabtrdpPara:AT_CheckCabtrdpPara Check Cabtrdp para error!.");
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    msg = (AT_THROT_CabtrdpQueryReq *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_THROT_CabtrdpQueryReq));
    if (msg == VOS_NULL_PTR) {
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)msg + VOS_MSG_HEAD_LENGTH, sizeof(AT_THROT_CabtrdpQueryReq) -
        VOS_MSG_HEAD_LENGTH, 0x00, sizeof(AT_THROT_CabtrdpQueryReq) - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, AT_GetDestPid((MN_CLIENT_ID_T)indexNum, I0_PS_PID_NAS_COMM), ID_AT_THROT_CABTRDP_QUERY_REQ);

    /* ��д��Ϣ���� */
    msg->modemId  = modemId;
    msg->clientId = g_atClientTab[indexNum].clientId;

    /* ���������������������APN,apnLen = 0,at����ͺ�ֱ�ӷ���OK */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_CMD_NO_PARA) {
        AT_ERR_LOG("At_SetCabtrdpPara:no apn.");
        msg->opApn = VOS_FALSE;
    }

    /* ���������������� */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_PARA_CMD) {
        /* APN����Ϊ�յ�ʱ�򣬽�apnLen��Ϊ1,apn����ȫ����Ϊ0 */
        if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen == 0) {
            AT_ERR_LOG("At_SetCabtrdpPara:APN information is empty.");
            msg->apnInfo.apnLen = 1;
        } else {
            msg->apnInfo.apnLen = (VOS_UINT8)g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen;
            memResult = memcpy_s((VOS_CHAR *)msg->apnInfo.apnName, sizeof(msg->apnInfo.apnName),
                (VOS_CHAR *)g_atParaList[AT_COMMAND_PARA_INDEX_0].para, g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(msg->apnInfo.apnName), g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen);
        }
        msg->opApn = VOS_TRUE;
    }

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("At_SetCabtrdpPara: TAF_TraceAndSendMsg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CABTRDP_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

#if (FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT == FEATURE_ON)
/* ����CSBTSRI(����S-NSSAI��back-off��ʱ�������ϱ�)�Ĺ��ܿ���,�����ʽΪ:AT+CSBTSR=[<n>] */
VOS_UINT32 At_SetCsbtsrPara(VOS_UINT8 indexNum)
{
    AT_THROT_CsbtsriConfigSetReq *msg     = VOS_NULL_PTR;
    VOS_UINT32                    ret;
    ModemIdUint16                 modemId = MODEM_ID_0;

    ret = AT_GetModemIdFromClient(indexNum, &modemId);
    if (ret != VOS_OK) {
        return AT_ERROR;
    }

    /* �������ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_COMMAND_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    msg = (AT_THROT_CsbtsriConfigSetReq *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_THROT_CsbtsriConfigSetReq));
    if (msg == VOS_NULL_PTR) {
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)msg + VOS_MSG_HEAD_LENGTH, sizeof(AT_THROT_CsbtsriConfigSetReq) -
        VOS_MSG_HEAD_LENGTH, 0x00, sizeof(AT_THROT_CsbtsriConfigSetReq) - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, AT_GetDestPid((MN_CLIENT_ID_T)indexNum, I0_PS_PID_NAS_COMM),
        ID_AT_THROT_CSBTSRI_CONFIGURATION_SET_REQ);

    /* ��д��Ϣ���� */
    msg->modemId  = modemId;
    msg->clientId = g_atClientTab[indexNum].clientId;

    /* ������Ϊ��,��ȡ��ǰisEnableCsbtsri��ֵ;������Ϊ����isEnableCsbtsriĬ��ΪVOS_FALSE */
    if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen != 0) {
        msg->isEnableCsbtsri = g_atParaList[AT_COMMAND_PARA_INDEX_0].paraValue;
    }

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("At_SetCsbtsrPara: Send msg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSBTSR_SET;

    return AT_WAIT_ASYNC_RETURN;
}

/* ��������: ���+CSBTRDP��������Ƿ���ȷ */
LOCAL VOS_UINT32 AT_CheckCsbtrdpPara(PS_S_NSSAI_STRU *nssai)
{
    /* ���������������������������Ϊ0�Ҳ��ܳ���1 */
    if ((g_atParaIndex == 0) || (g_atParaIndex != AT_COMMAND_MAX_PARA_NUM)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen != 0) {
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        /* ����<S-NSSAI>���� */
        if (AT_DecodeSNssai(g_atParaList[AT_COMMAND_PARA_INDEX_0].para, g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen,
            nssai) != VOS_OK) {
            AT_ERR_LOG("AT_CheckCsbtrdpPara: snssai is Invalid.");
            return AT_CME_INCORRECT_PARAMETERS;
        }
#endif
    }
    return AT_SUCCESS;
}

/* ����+CSBTRDP��S-NSSAI����Ϣ,���ڲ�ѯS-NSSAI backoff��ʱ����Ϣ,��ʽΪ:AT+CSBTRDP[=<S-NSSAI>] */
VOS_UINT32 At_SetCsbtrdpPara(VOS_UINT8 indexNum)
{
    AT_THROT_CsbtrdpQueryReq *msg     = VOS_NULL_PTR;
    NAS_THROT_Snssai          nssai   = {0};
    ModemIdUint16             modemId = MODEM_ID_0;

    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        return AT_ERROR;
    }

    /* �����������������Լ�������� */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_PARA_CMD) {
        if (AT_CheckCsbtrdpPara((PS_S_NSSAI_STRU *)&nssai) != AT_SUCCESS ) {
            AT_ERR_LOG("At_SetCsbtrdpPara:csbtrdp Para error!");
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    msg = (AT_THROT_CsbtrdpQueryReq *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_THROT_CsbtrdpQueryReq));
    if (msg == VOS_NULL_PTR) {
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)msg + VOS_MSG_HEAD_LENGTH, sizeof(AT_THROT_CsbtrdpQueryReq) -
        VOS_MSG_HEAD_LENGTH, 0x00, sizeof(AT_THROT_CsbtrdpQueryReq) - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, AT_GetDestPid((MN_CLIENT_ID_T)indexNum, I0_PS_PID_NAS_COMM), ID_AT_THROT_CSBTRDP_QUERY_REQ);

    /* ��д��Ϣ���� */
    msg->modemId  = modemId;
    msg->clientId = g_atClientTab[indexNum].clientId;

    /* ��������������������ʱnssai,opSnssaiΪ0,ֱ�ӷ���OK,������ʱ���������߼�ƥ��nssai */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_PARA_CMD) {
        if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen == 0) {
            msg->snssaiInfo.isEmptyNssai = VOS_TRUE;
        } else {
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
            msg->snssaiInfo.nssai = nssai;
#endif
        }

        msg->opSnssai = VOS_TRUE;
    }

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("At_SetCsbtrdpPara: Send msg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSBTRDP_SET;

    return AT_WAIT_ASYNC_RETURN;
}

/* ����CSDBTSRI(����S-NSSAI��DNN��back-off��ʱ�������ϱ�)�Ĺ��ܿ���,�����ʽΪ:AT+CSDBTSR=[<n>] */
VOS_UINT32 At_SetCsdbtsrPara(VOS_UINT8 indexNum)
{
    AT_THROT_CsdbtsriConfigSetReq *msg     = VOS_NULL_PTR;
    VOS_UINT32                     ret;
    ModemIdUint16                  modemId = MODEM_ID_0;

    ret = AT_GetModemIdFromClient(indexNum, &modemId);
    if (ret != VOS_OK) {
        return AT_ERROR;
    }

    /* �������ͼ�� */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_SET_PARA_CMD) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* �������� */
    if (g_atParaIndex > AT_COMMAND_MAX_PARA_NUM) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    msg = (AT_THROT_CsdbtsriConfigSetReq *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_THROT_CsdbtsriConfigSetReq));
    if (msg == VOS_NULL_PTR) {
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)msg + VOS_MSG_HEAD_LENGTH, sizeof(AT_THROT_CsdbtsriConfigSetReq) -
        VOS_MSG_HEAD_LENGTH, 0x00, sizeof(AT_THROT_CsdbtsriConfigSetReq) - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, AT_GetDestPid((MN_CLIENT_ID_T)indexNum, I0_PS_PID_NAS_COMM),
        ID_AT_THROT_CSDBTSRI_CONFIGURATION_SET_REQ);

    /* ��д��Ϣ���� */
    msg->modemId  = modemId;
    msg->clientId = g_atClientTab[indexNum].clientId;

    /* ������Ϊ��,��ȡ��ǰisEnableCsbtsri��ֵ;������Ϊ����isEnableCsbtsriĬ��ΪVOS_FALSE */
    if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen != 0) {
        msg->isEnableCsdbtsri = g_atParaList[AT_COMMAND_PARA_INDEX_0].paraValue;
    }

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("At_SetCsdbtsrPara: Send msg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSDBTSR_SET;

    return AT_WAIT_ASYNC_RETURN;
}


/* ��������: ���+CSDBTRDP��������Ƿ���ȷ */
LOCAL VOS_UINT32 AT_CheckCsdbtrdpPara(PS_S_NSSAI_STRU *nssai, VOS_UINT16 clientId)
{
    /* �����������������������ֻ��2 */
    if ((g_atParaIndex == 0) || (g_atParaIndex != AT_CSDBTRDP_PARA_MAX_NUM)) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* DNN���ȼ�� */
    if (g_atParaList[AT_COMMAND_PARA_INDEX_1].paraLen > NAS_THROT_MAX_APN_LEN) {
        return AT_CME_INCORRECT_PARAMETERS;
    }

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen != 0) {
        /* ����<S-NSSAI>���� */
        if (AT_DecodeSNssai(g_atParaList[AT_COMMAND_PARA_INDEX_0].para, g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen,
            nssai) != VOS_OK) {
            AT_ERR_LOG("AT_CheckCsdbtrdpPara:snssai error !");
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
#endif
    if (g_atParaList[AT_COMMAND_PARA_INDEX_1].paraLen != 0) {
        /* DNN��ʽ��� */
        if (AT_CheckApnFormat(g_atParaList[AT_COMMAND_PARA_INDEX_1].para,
            g_atParaList[AT_COMMAND_PARA_INDEX_1].paraLen, clientId) != VOS_OK) {
            AT_ERR_LOG("AT_CheckCsdbtrdpPara:dnn error !");
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    return AT_SUCCESS;
}

/* ����+CSDBTRDP��S-NSSAI,DNN����Ϣ,���ڲ�ѯ����S-NSSAI��DNN��backoff��ʱ����Ϣ,��ʽΪ:AT+CSDBTRDP[=<S-NSSAI>,<DNN>] */
VOS_UINT32 At_SetCsdbtrdpPara(VOS_UINT8 indexNum)
{
    AT_THROT_CsdbtrdpQueryReq *msg   = VOS_NULL_PTR;
    NAS_THROT_Snssai           nssai = {0};
    errno_t                    memResult;
    ModemIdUint16              modemId = MODEM_ID_0;

    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        return AT_ERROR;
    }

    /* �����������������Լ�������� */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_PARA_CMD) {
        if (AT_CheckCsdbtrdpPara((PS_S_NSSAI_STRU *)&nssai, g_atClientTab[indexNum].clientId) != AT_SUCCESS ) {
            AT_ERR_LOG("At_SetCsdbtrdpPara:Csdbtrdp Para error !");
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }

    msg = (AT_THROT_CsdbtrdpQueryReq *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_THROT_CsdbtrdpQueryReq));
    if (msg == VOS_NULL_PTR) {
        return AT_ERROR;
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)msg + VOS_MSG_HEAD_LENGTH, sizeof(AT_THROT_CsdbtrdpQueryReq) -
        VOS_MSG_HEAD_LENGTH, 0x00, sizeof(AT_THROT_CsdbtrdpQueryReq) - VOS_MSG_HEAD_LENGTH);

    /* ��д��Ϣͷ */
    AT_CFG_MSG_HDR(msg, AT_GetDestPid((MN_CLIENT_ID_T)indexNum, I0_PS_PID_NAS_COMM), ID_AT_THROT_CSDBTRDP_QUERY_REQ);

    /* ��д��Ϣ���� */
    msg->modemId  = modemId;
    msg->clientId = g_atClientTab[indexNum].clientId;

    /* ��������������������ʱnssai��dnn,opSnssai��opApnΪ0,ֱ�ӷ���OK,������ʱ���������߼�����ƥ�� */
    if (g_atParseCmd.cmdOptType == AT_CMD_OPT_SET_PARA_CMD) {
        if (g_atParaList[AT_COMMAND_PARA_INDEX_0].paraLen == 0) {
            /* ��һ����������Ϊ0ʱ��nssa������iΪ�գ�isEmptyNssai��Ϊ1,opSnssaiΪ1 */
            msg->snssaiInfo.isEmptyNssai = VOS_TRUE;
        } else {
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
            msg->snssaiInfo.nssai = nssai;
#endif
        }

        if (g_atParaList[AT_COMMAND_PARA_INDEX_1].paraLen == 0) {
            /* �ڶ�����������Ϊ0ʱ,DNN����Ϊ�յ�ʱ�򣬽�apnLen��Ϊ1,dnn����ȫ����Ϊ0,opApnΪ1 */
            msg->apnInfo.apnLen = 1;
        } else {
            msg->apnInfo.apnLen = (VOS_UINT8)g_atParaList[AT_COMMAND_PARA_INDEX_1].paraLen;
            memResult = memcpy_s((VOS_CHAR *)msg->apnInfo.apnName, sizeof(msg->apnInfo.apnName),
                (VOS_CHAR *)g_atParaList[AT_COMMAND_PARA_INDEX_1].para, g_atParaList[AT_COMMAND_PARA_INDEX_1].paraLen);
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(msg->apnInfo.apnName), g_atParaList[AT_COMMAND_PARA_INDEX_1].paraLen);
        }

        msg->opSnssai = VOS_TRUE;
        msg->opApn    = VOS_TRUE;
    }

    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_WARN_LOG("At_SetCsdbtrdpPara: Send msg fail.");
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSDBTRDP_SET;

    return AT_WAIT_ASYNC_RETURN;
}
#endif

