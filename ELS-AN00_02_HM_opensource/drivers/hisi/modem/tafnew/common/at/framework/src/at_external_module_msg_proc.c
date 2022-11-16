/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
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
 */

#include "securec.h"
#include "at_external_module_msg_proc.h"

#if (VOS_OS_VER == VOS_LINUX)
#include <linux/bsearch.h>
#include <linux/sort.h>
#endif


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_EXTERNAL_MODULE_MSG_PROC_C

/*
 * ������A��ʱ��A��linux kernel�в�ʹ��qsort������
 * ��ʹ�õ���sort����(�μ�kernel��./lib/sort.c�ļ�)
 * ���Դ˴����ϵͳ���ֶ���
 */
#if (VOS_OS_VER == VOS_LINUX)
#define LIB_SORT(base, num, size, cmp) sort((base), (num), (size), (cmp), VOS_NULL_PTR)
#else
#define LIB_SORT(base, num, size, cmp) qsort((base), (num), (size), (cmp))
#endif

STATIC CONST AT_ModuleMsgProcInfo *g_atModuleMsgProcInfoTbl[AT_PROC_MODULE_BUTT];

STATIC VOS_INT32 AT_CmpMsgProcEntryHead(CONST void *a, CONST void *b)
{
    CONST AT_MsgProcEntryHead *headA = (AT_MsgProcEntryHead *)a;
    CONST AT_MsgProcEntryHead *headB = (AT_MsgProcEntryHead *)b;
    VOS_UINT64  keyA, keyB;

    keyA = headA->msgId;
    keyA = keyA << 32 | headA->opt;
    keyB = headB->msgId;
    keyB = keyB << 32 | headB->opt;

    return keyA < keyB ? -1 : keyA == keyB ? 0 : 1;
}

STATIC VOS_UINT32 AT_GetMsgId(CONST VOS_VOID *msg)
{
    return ((MSG_Header *)msg)->msgName;
}

STATIC AT_MsgProcEntry *AT_GetProcMsgEntry(CONST AT_MsgProcEntryTblInfo *tblInfo, AT_MsgProcEntryHead *key)
{
    return (AT_MsgProcEntry *)bsearch(key, tblInfo->tbl, tblInfo->entryNum, tblInfo->entryLen, AT_CmpMsgProcEntryHead);
}

STATIC AT_MsgProcEntry *AT_FindProcMsgEntry(CONST AT_ModuleMsgProcInfo *procInfo, CONST VOS_VOID *msg, VOS_UINT8 indexNum)
{
    AT_MsgProcEntry      *procEntry = VOS_NULL_PTR;
    AT_MsgProcEntryHead   key;

    /* ��ģ��û���ṩ�Լ���GetMsgId����ʱ��ʹ��ͨ�ú���AT_GetMsgId */
    key.msgId = (procInfo->GetMsgId == VOS_NULL_PTR) ? AT_GetMsgId(msg) : procInfo->GetMsgId(msg);

    /*
     * �ǹ㲥��Ϣʹ��cmdCurrentOpt��MSGID���ʧ�ܣ���Ҫʹ�á�AT_CMD_OPT_NO_NEED_CHECK����msgId�������
     * ԭ�����ڣ�1���ǹ㲥��Ϣ�лظ�����Ϣ�͵�����Ϣ�������ͣ��޷�ʶ��ǹ㲥��Ϣ�ǻظ�����Ϣ�����ǵ�����Ϣ
     *           2��������Ϣʹ�á�AT_CMD_OPT_NO_NEED_CHECK������
     *
     */
    if (!AT_IsBroadcastClientIndex(indexNum)) {
        key.opt = g_atClientTab[indexNum].cmdCurrentOpt;
        procEntry = AT_GetProcMsgEntry(procInfo->tblInfo, &key);
        if (procEntry != VOS_NULL_PTR) {
            return procEntry;
        }
        AT_NORM_LOG2("AT_FindProcMsgEntry: find rsp fail!", key.msgId, key.opt);
    }

    key.opt = AT_CMD_OPT_NO_NEED_CHECK;
    procEntry = AT_GetProcMsgEntry(procInfo->tblInfo, &key);
    if (procEntry == VOS_NULL_PTR) {
        AT_ERR_LOG1("AT_FindProcMsgEntry: find no check opt fail!", key.msgId);
    }

    return procEntry;
}

STATIC CONST AT_ModuleMsgProcInfo *AT_GetModuleMsgProcInfo(AT_ProcModuleTypeUint32 moduleId)
{
    if (moduleId >= AT_PROC_MODULE_BUTT) {
        return VOS_NULL_PTR;
    }

    return g_atModuleMsgProcInfoTbl[moduleId];
}

/*
 * ÿ��ģ�����Ϣ������Ϣע��
 * *procInfo������ģ�����Ϣ������Ϣ
 */
VOS_UINT32 AT_RegModuleMsgProcInfo(CONST AT_ModuleMsgProcInfo *procInfo)
{
    CONST AT_MsgProcEntryTblInfo *tblInfo = VOS_NULL_PTR;

    if (procInfo == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_RegModuleMsgProcInfo: procInfo is null!");
        return VOS_ERR;
    }

    /* GetMsgId����ȱʡ�����������쳣�ж� */
    if (procInfo->moduleId >= AT_PROC_MODULE_BUTT ||
        procInfo->tblInfo == VOS_NULL_PTR ||
        procInfo->GetUserId == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_RegModuleMsgProcInfo: procInfo abnormal!");
        return VOS_ERR;
    }

    g_atModuleMsgProcInfoTbl[procInfo->moduleId] = procInfo;

    tblInfo = procInfo->tblInfo;
    LIB_SORT(tblInfo->tbl, tblInfo->entryNum, tblInfo->entryLen, AT_CmpMsgProcEntryHead);

    return VOS_OK;
}

/*
 * ��������: ATģ���ⲿ��Ϣ����������
 * ��    ��: ���޷�ʹ�øù����߼������Բο�AT_ProcBbicMsg���д���
 */
VOS_UINT32 AT_ProcExternalModuleMsg(VOS_VOID *msg, AT_ProcModuleTypeUint32 moduleId)
{
    CONST AT_ModuleMsgProcInfo *procInfo = VOS_NULL_PTR;
    CONST AT_MsgProcEntry      *procEntry = VOS_NULL_PTR;
    VOS_UINT32                  ret;
    VOS_UINT8                   indexNum;

    procInfo = AT_GetModuleMsgProcInfo(moduleId);
    if (procInfo == VOS_NULL_PTR) {
        AT_ERR_LOG1("AT_ProcExternalModuleMsg: module procInfo is NULL!", moduleId);
        return VOS_ERR;
    }

    /* ��ȡindexNum */
    if (procInfo->GetUserId(msg, &indexNum) != VOS_OK) {
        AT_WARN_LOG("AT_ProcExternalModuleMsg: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    procEntry = AT_FindProcMsgEntry(procInfo, msg, indexNum);
    if (procEntry == VOS_NULL_PTR || procEntry->func == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_ProcExternalModuleMsg: entry abnormal!");
        return VOS_ERR;
    }
    ret = procEntry->func(msg, indexNum);

    /*
     * AT_CMD_OPT_NO_NEED_CHECK  ��������־��ϢΪ�㲥��֪ͨ��Ϣ����ATģ�����״̬�޹�
     * AT_CMD_NO_NEED_FORMAT_RSLT��������־procEntry->func�������Ѿ�������ɣ�
     *                             ����Ҫʹ��ģ���е�format����
     * �����жϵ������߼�Ϊ��
     * 1���㲥���߸÷�֧
     * 2�������˿ڵ�֪ͨ��Ϣ���߸÷�֧
     * 3����Ӧ�������Ѿ��ϱ����������Ϣ���߸÷�֧
     */
    if (indexNum < AT_CLIENT_ID_BUTT &&
        procEntry->opt != AT_CMD_OPT_NO_NEED_CHECK &&
        ret != AT_CMD_NO_NEED_FORMAT_RSLT) {
        /* ��λAT״̬ */
        AT_StopTimerCmdReady(indexNum);
        At_FormatResultData(indexNum, ret);
    }

    return VOS_OK;
}

