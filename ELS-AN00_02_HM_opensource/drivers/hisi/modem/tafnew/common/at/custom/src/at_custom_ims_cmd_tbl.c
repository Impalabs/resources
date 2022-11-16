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
#include "at_custom_ims_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_ims_set_cmd_proc.h"
#include "at_custom_ims_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_IMS_CMD_TBL_C

static const AT_ParCmdElement g_atCustomImsCmdTbl[] = {
#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: IMS����ҵ����������
     * [˵��]: ͨ�����������Դ򿪺͹ر�IMS�ڸ����뼼���µĶ����շ�������
     *         APͨ���������IMS��Ķ�������ʱ����Ҫͬʱ��IMS�ڶ�Ӧ���뼼���µ�֧���������Ա��ڸý��뼼���������շ�IMS���š�
     * [�﷨]:
     *     [����]: ^IMSSMSCFG=<wifi_enable>,<lte_enable>,<utran_enable>,<gsm_enable>
     *     [���]: ����ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ִ�д��������
     *             ����ҵ��������·��������ʧ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IMSSMSCFG?
     *     [���]: <CR><LF>^IMSSMSCFG: <wifi_enable>,<lte_enable>,<utran_enable>,<gsm_enable><CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^IMSSMSCFG=?
     *     [���]: <CR><LF>^ IMSSMSCFG: (list of supported <wifi_enable>s), (list of supported <lte_enable>s),  (list of supported <utran_enable>s), (list of supported  <gsm_enable>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <wifi_enable>: ����ֵ�� IMS��WIFI�¶��Ź��ܵĿ���״̬��
     *             0���ر�WIFI��IMS�����շ����ܣ�
     *             1����WIFI��IMS�����շ����ܡ�
     *     <lte_enable>: ����ֵ�� IMS��LTE�¶��Ź��ܵĿ���״̬��
     *             0���ر�LTE��IMS�����շ����ܣ�
     *             1����LTE��IMS�����շ����ܡ�
     *     <utran_enable>: ����ֵ�� IMS��Utran�¶��Ź��ܵĿ���״̬��
     *             0���ر�Utran��IMS�����շ����ܣ�
     *             1����Utran��IMS�����շ����ܡ�
     *     <gsm_enable>: ����ֵ�� IMS��Gsm�¶��Ź��ܵĿ���״̬��
     *             0���ر�Gsm��IMS�����շ����ܣ�
     *             1����Gsm��IMS�����շ����ܡ�
     * [ʾ��]:
     *     �� ��WIFI��LTE���IMS�����շ�����
     *       AT^IMSSMSCFG=1,1,0,0
     *       OK
     *     �� ��ѯIMS������������
     *       AT^IMSSMSCFG?
     *       ^IMSSMSCFG: 1,1,0,0
     *       OK
     *     �� ��������
     *       AT^IMSSMSCFG=?
     *       ^IMSSMSCFG: (0-1),(0-1),(0-1),(0-1)
     *       OK
     */
    { AT_CMD_IMSSMSCFG,
      AT_SetImsSmsCfgPara, AT_SET_PARA_TIME, AT_QryImsSmsCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSSMSCFG", (VOS_UINT8 *)"(0,1),(0,1),(0,1),(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����IMSע�����̴���ԭ��ֵ�ϱ�
     * [˵��]: ����IMSע��ʧ��ʱ������ԭ��ֵ�Ƿ�ͨ��AT���������ϱ���
     * [�﷨]:
     *     [����]: ^IMSREGERRRPT=<value>
     *     [���]: ����ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ִ�д��������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^IMSREGERRRPT?
     *     [���]: <CR><LF>^IMSREGERRRPT: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^IMSREGERRRPT=?
     *     [���]: <CR><LF>^IMSREGERRRPT: (list of supported <value >s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: ����ֵ���Ƿ������ϱ�IMSע��ʧ��ԭ��ֵ��Ĭ��ֵ0��
     *             0���ر������ϱ�IMSע��ʧ��ԭ��ֵ����
     *             1���������ϱ�IMSע��ʧ��ԭ��ֵ����
     * [ʾ��]:
     *     �� �������ϱ�IMSע��ʧ��ԭ��ֵ����
     *       AT^IMSREGERRRPT=1
     *       OK
     *     �� ��ѯ�����ϱ�IMSע��ʧ��ԭ��ֵ�����Ƿ��
     *       AT^IMSREGERRRPT?
     *       ^IMSREGERRRPT: 0
     *       OK
     *     �� ��ѯ�����ϱ�IMSע��ʧ��ԭ��ֵ���ܴ��ڵĿ�������
     *       AT^IMSREGERRRPT=?
     *       ^IMSREGERRRPT: (0-1)
     *       OK
     */
    { AT_CMD_IMSREGERRRPT,
      AT_SetImsRegErrRpt, AT_SET_PARA_TIME, AT_QryImsRegErrRpt, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IMSREGERRRPT", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ���һ��IMS����д�����
     * [˵��]: ��ѯ���һ��IMS���ϱ��ĺ��д���ԭ��ֵ���ı���Ϣ��
     * [�﷨]:
     *     [����]: ^CIMSERR?
     *     [���]: <CR><LF>^CIMSERR: <cause>,<text><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CIMSERR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cause>: ����ֵ�������4.5 CS������롱��
     *     <text>: ��������ı���Ϣ
     * [ʾ��]:
     *     �� IMS�����ʧ�ܣ���ѯԭ��ֵ
     *       AT^CIMSERR?
     *       ^CIMSERR: 18737,"USE PROXY"
     *       OK
     *     �� ��������
     *       AT^CIMSERR=?
     *       OK
     */
    { AT_CMD_CIMSERR,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryCimsErrPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CIMSERR", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����IMS����״̬������Ϣ�ϱ�
     * [˵��]: ��������������IMS����״̬������Ϣ�ϱ����á��򿪺͹ر�^IMSSRVSTATUS����������ϱ���
     * [�﷨]:
     *     [����]: ^IMSSRVSTATRPT=<enable>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR:  <err_code><CR><LF>
     *     [����]: ^IMSSRVSTATRPT?
     *     [���]: <CR><LF>^IMSSRVSTATRPT: <enable><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^IMSSRVSTATRPT=?
     *     [���]: <CR><LF>^IMSSRVSTATRPT: (list of supported <enable>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <enable>: ����ֵ�������ϱ����ƿ��أ�
     *             0���ر������ϱ���
     *             1�����������ϱ���
     * [ʾ��]:
     *     �� ����IMS����״̬������Ϣ�ϱ�
     *       AT^IMSSRVSTATRPT=1
     *       OK
     *     �� ��ѯIMS����״̬������Ϣ�ϱ�����
     *       AT^IMSSRVSTATRPT?
     *       ^IMSSRVSTATRPT: 1
     *       OK
     */
    { AT_CMD_IMSSRVSTATRPT,
      AT_SetImsSrvStatRptCfgPara, AT_SET_PARA_TIME, AT_QryImsSrvStatRptCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IMSSRVSTATRPT", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯIMS����״̬
     * [˵��]: ��ѯIMS����״̬����ע��״̬��
     * [�﷨]:
     *     [����]: ^IMSSRVSTATUS?
     *     [���]: <CR><LF>^IMSSRVSTATUS: <sms_srv_status>,<sms_srv_rat>,<voip_srv_status>,<voip_srv_rat>,<vt_srv_status>,<vt_srv_rat>,<vs_srv_status>,<vs_srv_rat><CR><LF>
     * [����]:
     *     <sms_srv_status>: ����ֵ��IMS���ŵķ���״̬��
     *             0�� IMS SMS service is not available
     *             1�� IMS SMS is in limited service
     *             2�� IMS SMS is in full service
     *     <sms_srv_rat>: ����ֵ��IMS���ŵķ�����
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2�� IMS service is registered on Utran
     *             3�� IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     *     <voip_srv_status>: ����ֵ��IMS VoIP�ķ���״̬��
     *             0�� IMS VoIP service is not available
     *             1�� IMS VoIP is in limited service
     *             2�� IMS VoIP is in full service
     *     <voip_srv_rat>: ����ֵ��IMS VoIP�ķ�����
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered on Utran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     *     <vt_srv_status>: ����ֵ��IMS VT�ķ���״̬��
     *             0�� IMS VT service is not available
     *             1�� IMS VT is in limited service
     *             2�� IMS VT is in full service
     *     <vt_srv_rat>: ����ֵ��IMS VT�ķ���״̬��
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered on Utran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     *     <vs_srv_status>: ����ֵ��IMS VS�ķ���״̬��
     *             0�� IMS VS service is not available
     *             1�� IMS VS is in limited service
     *             2�� IMS VS is in full service
     *     <vs_srv_rat>: ����ֵ��IMS VS�ķ�����
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered on Utran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     * [ʾ��]:
     *     �� ��ѯIMS�ķ���״̬
     *       AT^IMSSRVSTATUS?
     *       ^IMSSRVSTATUS: 2,0,2,0,2,0,2,0
     *       OK
     */
    { AT_CMD_IMSSRVSTATUS,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryImsSrvStatusPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IMSSRVSTATUS", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����WIFI�������� Address-ID
     * [˵��]: ����WIFI�������� Address-ID��
     * [�﷨]:
     *     [����]: ^WIEMCAID=<Address_ID>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^WIEMCAID=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <Address_ID>: �ַ�������󳤶���256���ַ���
     * [ʾ��]:
     *     �� ����WIFI�������� Address-ID
     *       AT^WIEMCAID="fsfs"
     *       OK
     */
    { AT_CMD_WIEMCAID,
      AT_SetEmcAIdPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^WIEMCAID", (VOS_UINT8 *)"(str)" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: RCS DM������������
     * [˵��]: ��������������RCS��DM������
     * [�﷨]:
     *     [����]: ^DMRCSCFG=<featuretag>,<devcfg>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^DMRCSCFG=?
     *     [���]: <CR><LF>^DMRCSCFG: (0-4294967295),(0-4294967295)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <featuretag>: 32λ����ֵ��RCS������Ϣ�������仯����Ҫ����ˢ��ע��
     *     <devcfg>: 32λ����ֵ��RCS������Ϣ�������仯����Ҫ����ˢ��ע��
     * [ʾ��]:
     *     �� ����RCS DM�����ɹ���
     *       AT^DMRCSCFG=1024,26
     *       OK
     *     �� ��������
     *       AT^DMRCSCFG=?
     *       ^DMRCSCFG: (0-4294967295),(0-4294967295)
     *       OK
     */
    { AT_CMD_DMRCSCFG,
      AT_SetDmRcsCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DMRCSCFG", (VOS_UINT8 *)"(0-4294967295),(0-4294967295)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: User Agent ��Ϣ��������
     * [˵��]: ��������������User Agent�����Ϣ��
     * [�﷨]:
     *     [����]: ^USERAGENTCFG=<para1>,<para2>,<para3>,<para4>,<para5>,<para6>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^USERAGENTCFG=?
     *     [���]: <CR><LF>^USERAGENTCFG: (str),(str),(str),(str),(str),(str)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <para1>: �ַ������ͣ��ַ�����󳤶�Ϊ16�ֽڡ�
     *             User Agent��Ϣ����1��
     *     <para2>: �ַ������ͣ��ַ�����󳤶�Ϊ16�ֽڡ�
     *             User Agent��Ϣ����2��
     *     <para3>: �ַ������ͣ��ַ�����󳤶�Ϊ16�ֽڡ�
     *             User Agent��Ϣ����3��
     *     <para4>: �ַ������ͣ��ַ�����󳤶�Ϊ16�ֽڡ�
     *             User Agent��Ϣ����4��
     *     <para5>: �ַ������ͣ��ַ�����󳤶�Ϊ16�ֽڡ�
     *             User Agent��Ϣ����5��
     *     <para6>: �ַ������ͣ��ַ�����󳤶�Ϊ16�ֽڡ�
     *             User Agent��Ϣ����6��
     * [ʾ��]:
     *     �� ����UA�汾��Ϣ�ɹ���
     *       AT^USERAGENTCFG="VKY-L09","C50B100","5.1","7.0",,
     *       OK
     *     �� ��������
     *       AT^USERAGENTCFG=?
     *       ^USERAGENTCFG: (str),(str),(str),(str),(str),(str)
     *       OK
     */
    { AT_CMD_USERAGENTCFG,
      AT_SetUserAgentCfgPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^USERAGENTCFG", (VOS_UINT8 *)"(str),(str),(str),(str),(str),(str)" },

    { AT_CMD_SIPPORT,
      At_SetSipTransPort, AT_SET_PARA_TIME, At_QrySipTransPort, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^TCPTHRESHOLD", (TAF_UINT8 *)"(400-16500)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ͨ��������������
     * [˵��]: ����������͸��FUSIONCALL��IMSA��IMS SDK������������Ϣ���ݡ�����Ϣ��Ҫ������ͨ������ҵ����ơ�
     * [�﷨]:
     *     [����]: ^FUSIONCALLRAW=<msg_id>,<msg_len>,<msg_context>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <msg_id>: ��Ӧ����ģ��
     *             0��IMSA
     *             1��IMS SDK
     *     <msg_len>: ��Ϣ����<msg_context>�ĳ��ȣ���Ϣ������󲻳���500
     *     <msg_context>: ģ���ӿ���Ϣ����
     * [ʾ��]:
     *     �� AP����������ͨ������ҵ��
     *       AT^FUSIONCALLRAW=0,12,"010000000000000001000000"
     *       OK
     */
    { AT_CMD_FUSIONCALLRAW,
      AT_SetFusionCallRawPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^FUSIONCALLRAW", (VOS_UINT8 *)"(0,1),(1-280),(msg)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ���ļ�Ȩ�û���
     * [˵��]: ��ѯ����IMPI����Ȩ�û�������
     * [�﷨]:
     *     [����]: ^VOLTEIMPI
     *     [���]: <CR><LF>^VOLTEIMPI: <impi><CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^VOLTEIMPI?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^VOLTEIMPI=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <impi>: ��Ȩ�û����ƣ����Ȳ�����128���ַ�����
     * [ʾ��]:
     *     �� ��ѯIMPI
     *       AT^VOLTEIMPI
     *       ^VOLTEIMPI: <target>tel:1234;phone-context=ims.mnc000.mcc460.3gppnetwork.org</target>
     *       OK
     */
    { AT_CMD_VOLTEIMPI,
      AT_SetVolteImpiPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^VOLTEIMPI", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ���Ĺ�����������
     * [˵��]: ��ѯ����Home Network Domain Name������������������
     * [�﷨]:
     *     [����]: ^VOLTEDOMAIN
     *     [���]: <CR><LF>^VOLTEDOMAIN: <domain><CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^VOLTEDOMAIN?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^VOLTEDOMAIN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <domain>: ������������(Home Network Doamin Name)�����Ȳ�����128���ַ�����
     * [ʾ��]:
     *     �� ��ѯHome Network Domain Name
     *       AT^VOLTEDOMAIN
     *       ^VOLTEDOMAIN: 3gppnetwork.org
     *       OK
     */
    { AT_CMD_VOLTEDOMAIN,
      AT_SetVolteDomainPara, AT_SET_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^VOLTEDOMAIN", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: IMS EMC PDN��̬������ѯ
     * [˵��]: ���������ڲ�ѯIMS EMC PDN��̬����������IP���͡�IP��ַ��DNS��ַ��MTU��
     *         ˵��������IMS�������ж�λ���ԡ�
     * [�﷨]:
     *     [����]: ^IMSEMCRDP?
     *     [���]: <CR><LF>^IMSEMCRDP: <ip_type>,<ip_addr>
     *             ,<prim_dns>,<sec_dns>,<mtu><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������<CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IMSEMCRDP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <ip_type>: �ַ������ͣ���������Э�����͡�   IPV4��IPV4 Э�飻
     *             IPV6��IPV6 Э�顣
     *     <ip_addr>: �ַ������ͣ�IP��ַ��
     *     <prim_dns>: �ַ������ͣ�Primary DNS��ַ��
     *     <sec_dns>: �ַ������ͣ�Secondary DNS��ַ��
     *     <mtu>: ����ֵ��MTU��
     * [ʾ��]:
     *     �� IMS EMC PDN����ΪIPV4
     *       AT^IMSEMCRDP?
     *       ^IMSEMCRDP: "IPV4","10.10.1.1","10.11.11.1","10.11.11.2",1280
     *       OK
     *     �� IMS EMC PDN����ΪIPV6
     *       AT^IMSEMCRDP?
     *       ^IMSEMCRDP: "IPV6","2001::1","2002::1","2002::2",1280
     *       OK
     *     �� IMS EMC PDN����ΪIPV4V6
     *       AT^IMSEMCRDP?
     *       ^IMSEMCRDP: "IPV4","10.10.1.1","10.11.11.1","10.11.11.2",1280
     *       ^IMSEMCRDP: "IPV6","2001::1","2002::1","2002::2",1280
     *       OK
     *     �� ��������
     *       AT^IMSEMCRDP=?
     *       OK
     */
    { AT_CMD_IMSEMCRDP,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryImsEmcRdpPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSEMCRDP", VOS_NULL_PTR },
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: ����IMS URSP����
     * [˵��]: ����������AP�����յ�������·���UE POLICY ��Ϣ�󣬽������й�IMS��URSP������Ϣ���͸�Modem��
     *         <length >����Ҫ�����������ַ��������ĳ�����ͬ��
     *         ������^IMSURSP����֮��ſ�������ims ursp��Ϣ�������ڷ����ŵĴ�������ims ursp��Ϣ��������ctrl-z��β��
     *         ��ims ursp��Ϣ�ַ������ȳ���1500����Ҫ�ֶ��·���ÿ������ʹ��CR���س�������β���ַ����ܳ��Ȳ�����3000��
     * [�﷨]:
     *     [����]: ^IMSURSP=<version>,<length>
     *     [���]: ִ�����óɹ�ʱ��<CR><LF>OK<CR><LF>
     *             ���������<CR><LF>ERROR<CR><LF>
     * [����]:
     *     <version>: AP��IMSAԼ�������������汾����ǰ��֧��ȡֵ1
     *     <length>: ����ֵ��IMS URSP�ַ������ȣ�ȡֵ��Χ��0-3000��
     * [ʾ��]:
     *     �� AP�·�IMS URSP��Ϣ
     *       AT^IMSURSP=1,20
     *       >0123456789abcdef0123
     *       >ctrl-z
     *       OK
     *       AT^IMSURSP=1,20
     *       >0123456789
     *       >abcdef0123
     *       >ctrl-z
     *       OK
     *     �� AP�·�ɾ��IMS URSP��Ϣ
     *       AT^IMSURSP=1,0
     *       OK
     */
    { AT_CMD_IMSURSP,
      AT_SetImsUrsp, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IMSURSP", (VOS_UINT8 *)"(0-255),(0-3000)" },
#endif

#if (FEATURE_ECALL == FEATURE_ON)
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ����IMS ECALL��������
     * [˵��]: ���������ڶ�������ecall over ims��ѡ��Ͷ���SIP��Ϣ�е�Content Typeֵ��
     * [�﷨]:
     *     [����]: ^ECLIMSCFG=<n>[,<ContentType>]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ��ʧ��ʱ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^ECLIMSCFG?
     *     [���]: <CR><LF>^ECLIMSCFG: <n>[,<Content Type>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <n>: 0 �C ecall over ims��ѡ��ǿ��cs prefer��
     *             1 - ����ECALL OVER IMS ģʽ����ѡ��ps prefer������ͬʱ����content type;
     *             2 - ��׼ecall over ims ģʽ����ѡ��ps prefer������Ҫ����content type��
     *     <[<Content Type>]>: ��ѡ��ַ������ͣ������û����Ƶ�Content type �ַ��������ڲ��ԡ���ķ�ն���Content Type
     * [ʾ��]:
     *     �� ����ECLIMSCFG
     *       AT^ECLIMSCFG=0
     *       OK
     *     �� ��ѯECLIMSCFG
     *       AT^ECLIMSCFG?
     *       ECLIMSCFG: 0
     *     �� ����ECLIMSCFG
     *       AT^ECLIMSCFG=1,"application/MbCallData.sCall.MSD"
     *       OK
     *     �� ��ѯECLIMSCFG
     *       AT^ECLIMSCFG?
     *       ECLIMSCFG: 1,"application/MbCallData.sCall.MSD"
     */
    { AT_CMD_ECLIMSCFG,
      AT_SetEclImsCfgPara, AT_SET_PARA_TIME, AT_QryEclImsCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^ECLIMSCFG", (VOS_UINT8 *)"(0-2),(content_type)" },
#endif
};

/* ע��IMS����AT����� */
VOS_UINT32 AT_RegisterCustomImsCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomImsCmdTbl, sizeof(g_atCustomImsCmdTbl) / sizeof(g_atCustomImsCmdTbl[0]));
}

