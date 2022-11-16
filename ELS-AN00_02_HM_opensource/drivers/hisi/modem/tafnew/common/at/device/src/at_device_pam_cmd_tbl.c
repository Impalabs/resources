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
#include "at_device_pam_cmd_tbl.h"

#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"
#include "at_device_comm.h"

#include "at_device_pam_set_cmd_proc.h"
#include "at_device_pam_qry_cmd_proc.h"



#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_PAM_CMD_TBL_C

static const AT_ParCmdElement g_atDevicePamCmdTbl[] = {
#if (FEATURE_PHONE_SC == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯ�ֻ���PAD��̬SIMLOCK����������Ϣ
     * [˵��]: ����������AP-Modem��̬�²�ѯ�ֻ���PAD��SIMLOCK����������Ϣ���ṩ�����߹��̲˵�ʹ�á�
     *         ����ʹ�ó��������̲˵��·���
     *         ����ʹ�����ƣ�����Ҫ���߼�Ȩ���ɲ�ѯ��ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^PHONESIMLOCKINFO?
     *     [���]: <CR><LF>
     *             ^PHONESIMLOCKINFO: <cat>,<indicator> [,(<code_begin>,<code_end>)...]<CR><LF>
     *             ^PHONESIMLOCKINFO: <cat>,<indicator> [,(<code_begin>,<code_end>)...]<CR><LF>
     *             ^PHONESIMLOCKINFO: <cat>,<indicator> [,(<code_begin>,<code_end>)...]<CR><LF>
     *             ^PHONESIMLOCKINFO: <cat>,<indicator>[,<total>,<flag>,(<code_begin>,<code_end>)...]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^PHONESIMLOCKINFO=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cat>: �ַ������ͣ������������ͣ�Ŀǰ��֧�������������ͣ�
     *             NET�������磻
     *             NETSUB����������
     *             SP����SP��
     *             CP�� ��������
     *     <indicator>: �������ͣ���������ҵ���Ӧ���͵ļ���״̬��
     *             0��δ���
     *             1���Ѽ��
     *     <total_group_num>: ʵ���ܹ���total�����������Ŷ�
     *     <flag>: ָʾ���޻����������ʶ
     *             0: ��������
     *             1: ��������
     *     <code_begin>: ��������ҵ�����ʼ�ŶΣ����֧��20�顣
     *     <code_end>: ��������ҵ��Ľ����ŶΣ����֧��20�顣
     * [ʾ��]:
     *     �� ��ѯ�ֻ�SIMLOCK����������Ϣ
     *       AT^PHONESIMLOCKINFO?
     *       ^PHONESIMLOCKINFO: NET,1,2,0,(46001,46002),(46006,46006)
     *       ^PHONESIMLOCKINFO: NETSUB,1,15,0,(4600101,4600102),(4600205,4600208),(4600601, 4600601)
     *       ^PHONESIMLOCKINFO: SP,0
     *       ^PHONESIMLOCKINFO: CP,0
     *       OK
     *     �� ��������
     *       AT^PHONESIMLOCKINFO=?
     *       OK
     */
    { AT_CMD_PHONESIMLOCKINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryPhoneSimlockInfoPara, AT_QRY_PARA_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PHONESIMLOCKINFO", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ������������ȫ����
     * [˵��]: AP-Modem��̬�¶�����������ȫ���ݡ����̲˵���װ�����߻��õ���
     *         ����ʹ�ó��������̲˵������߹��ߡ�
     *         ����ʹ�����ƣ�����Ҫ���߼�Ȩ���ɲ�ѯ��ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^SIMLOCKDATAREAD?
     *     [���]: <CR><LF>
     *             ^SIMLOCKDATAREAD: <cat>,<indicator>, <lock_status>,<max_times>,<remain_times><CR><LF>
     *             ^SIMLOCKDATAREAD: <cat>,<indicator>, <lock_status>,<max_times>,<remain_times><CR><LF>
     *             ^SIMLOCKDATAREAD: <cat>,<indicator>, <lock_status>,<max_times>,<remain_times><CR><LF>
     *             ^SIMLOCKDATAREAD: <cat>,<indicator>,
     *             <lock_status>,<max_times>,<remain_times><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SIMLOCKDATAREAD=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cat>: �ַ������ͣ������������ͣ�Ŀǰ��֧��������������
     *             NET�������磻
     *             NETSUB����������
     *             SP����SP��
     *             CP����������
     *     <indicator>: �������ͣ���������ҵ���Ӧ���͵ļ���״̬��
     *             0��δ���
     *             1���Ѽ��
     *     <lock_status>: �ַ������ͣ�����������Ӧ���͵Ľ���״̬��<indicator>����Ϊ����״̬ʱ��Ч��δ����ʱΪ�գ�
     *             READY������״̬��
     *             PIN��PIN��״̬��
     *             PUK��PUK��״̬��
     *     <max_times>: �������ͣ���������������Դ�����PIN����״̬��Ч������״̬�������
     *     <remain_times>: �������ͣ�ʣ��Ĵ�����PIN����״̬��Ч������״̬�������
     * [ʾ��]:
     *     �� ��simlock��������״̬��
     *       AT^SIMLOCKDATAREAD?
     *       ^SIMLOCKDATAREAD: NET,1,PIN,10,10
     *       ^SIMLOCKDATAREAD: NETSUB,1,PUK,,
     *       ^SIMLOCKDATAREAD: SP,0,,,
     *       ^ SIMLOCKDATAREAD: CP,0,,,
     *       OK
     *     �� ��������
     *       AT^SIMLOCKDATAREAD=?
     *       OK
     */
    { AT_CMD_SIMLOCKDATAREAD,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QrySimlockDataReadPara, AT_QRY_PARA_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMLOCKDATAREAD", VOS_NULL_PTR },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �����ֻ������
     * [˵��]: �������ܡ����߼�Ȩ���ơ�����������ͨ�����߼�Ȩ�����ò���������Ч��
     *         ����������AP-Modem��̬�²�ѯ�޸��ֻ���PAD�豸������š�
     *         ����ʹ�����ƣ�ֻ�������AP�Խӵ�ר��ATͨ���·�������Ҫ���߼�Ȩͨ������д��Ͷ�ȡ�������������߼�Ȩ��
     *         ��ѯ�����ʹ�á�
     * [�﷨]:
     *     [����]: ^PHONEPHYNUM=<type>,<rsa_number>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^PHONEPHYNUM?
     *     [���]: <CR><LF>
     *             ^PHONEPHYNUM: <type>,<rsa_number><CR><LF>
     *             ^PHONEPHYNUM: <type>,<rsa_number><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^PHONEPHYNUM=?
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��ѯ�����ʹ�á�
     * [����]:
     *     <type>: �ַ������ͣ�����ŵ����͡�
     *             IMEI
     *             SN
     *     <rsa_number>: �����ͨ��RSA���ܺ����ɵ�128�ֽڵ����ġ�AT�����ʱ��ͨ��Half-Byte�������γ�256�ֽڵ��ַ�����
     * [ʾ��]:
     *     �� �޸��豸��IMEI��
     *       AT^PHONEPHYNUM=IMEI,0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567
     *       OK
     *     �� ��������
     *       AT^PHONEPHYNUM=?
     *       OK
     */
    { AT_CMD_PHONEPHYNUM,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PHONEPHYNUM", (VOS_UINT8 *)"(@type),(@Phynum),(@Hmac)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ������߼�Ȩ
     * [˵��]: ��������AP-Modem��̬�²��߼�Ȩ����ڲ��߼�Ȩʱ�������֣��ṩ�����߹���ʹ�á�
     *         �����������ڻ�ȡUE�Ĳ��߼�Ȩ����������������ݡ�
     *         ����ʹ�ó��������߹��߷�����߼�Ȩ��
     *         ����ʹ�����ƣ�ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^IDENTIFYSTART
     *     [���]: <CR><LF>^IDENTIFYSTART: <identify_start_response><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IDENTIFYSTART=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <identify_start_response>: ���߼�Ȩ�������ģ���RSA���ܺ��128���ֽ����ģ�ͨ��Half-Byte�������γ�256bytes���������͸���̨���ߡ�
     *             ���߼�Ȩ�����������ɷ�ʽ��
     *             UE����һ��32λ�������ʹ�ò��߼�Ȩ��Կͨ��RSA�㷨��������128�ֽڵ����ģ���ת����256���ַ���Half-Byte���ַ�����Ϊ����<identify_start_response>�ظ����û���
     * [ʾ��]:
     *     �� ������߼�Ȩ
     *       AT^IDENTIFYSTART
     *       ^IDENTIFYSTART: 3A2ACB4FAD9571416D858870F4860B9D3EF741E7123E0D208290F66EA736FA539BA1FCFF2B7B103B8F7513CDEF425A8EA8C473201D4572791A65F5E235C75C96AC0C23B3ECB2990D6137685E500EC9FDCBD4F5349236E344D922AFB68F4CF5C6AF8D213B3C89BD6DD0F72E42EE04639DBF0079A3ABB0A3F6352044BB2FF3C5B3
     *       OK
     *     �� ��������
     *       AT^IDENTIFYSTART=?
     *       OK
     */
    { AT_CMD_IDENTIFYSTART,
      AT_SetIdentifyStartPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IDENTIFYSTART", (VOS_UINT8 *)"(@Rsa)" },

#if (FEATURE_PHONE_SC == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: OTA��Ȩ��������
     * [˵��]: ����ʹ�����ƣ�ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^IDENTIFYOTASTART
     *     [���]: <CR><LF>^IDENTIFYOTASTART: <publicId1>,<publicId2>,<publicId3>,
     *             <identifyOTAStartRsp><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <publicId1>: SC OTAʹ�ù�Կid1��ʮ�������� ��Χ��0-0xFFFFFFFF��
     *     <publicId2>: SC OTAʹ�ù�Կid2��ʮ�������� ��Χ��0-0xFFFFFFFF��
     *     <publicId3>: SC OTAʹ�ù�Կid3��ʮ�������� ��Χ��0-0xFFFFFFFF��
     *     <identifyOTAStartRsp>: ��Ȩ�������ģ���RSA���ܺ��128���ֽ����ģ�ͨ��Half-Byte�������γ�256bytes���������͸���̨���ߡ�
     *             ��Ȩ�����������ɷ�ʽ��
     *             UE����һ��32λ�������ʹ�ò��߼�Ȩ��Կͨ��RSA�㷨��������128�ֽڵ����ģ���ת����256���ַ���Half-Byte���ַ�����Ϊ����<identify_start_response>�ظ����û���
     * [ʾ��]:
     *     �� �����Ȩ
     *       AT^IDENTIFYOTASTART
     *       ^IDENTIFYOTASTART: 0x1A2B3C4D,0x1234ABCD,0xAABBCCDD,3A2ACB4FAD9571416D858870F4860B9D3EF741E7123E0D208290F66EA736FA539BA1FCFF2B7B103B8F7513CDEF425A8EA8C473201D4572791A65F5E235C75C96AC0C23B3ECB2990D6137685E500EC9FDCBD4F5349236E344D922AFB68F4CF5C6AF8D213B3C89BD6DD0F72E42EE04639DBF0079A3ABB0A3F6352044BB2FF3C5B3
     *       OK
     */
    { AT_CMD_IDENTIFYOTASTART,
      AT_SetIdentifyStartPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IDENTIFYOTASTART", (VOS_UINT8 *)"(@Rsa)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: OTA��Ȩ��������
     * [˵��]: ����ʹ�����ƣ��·�^IDENTIFYOTASTART������Ȩ��ʹ�ã�ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^IDENTIFYOTAEND=<identify_end_request>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <identify_end_ request>: ��Ȩ�������ģ���RSA���ܺ��128�ֽ����ģ�ͨ��Half-Byte�������γ�256���ַ����͸���̨���ߡ�
     *             ��Ȩ�����������ɷ�ʽ��
     *             ͨ��˽Կ��^IDENTIFYOTASTART����ķ���ֵ���ܻ��һ�������������˽Կ�Ը��������������<identify_end_request>��
     * [ʾ��]:
     *     �� �·���ɼ�Ȩ��������
     *       AT^IDENTIFYOTAEND=948f2efb7b1c4e5d5d8788580091ad5fe39cfd461a7828027fa6b17979212c049bc759ded769f1aa18b3bfc40059b4ff6be7f240f968940fdd69a0f8be992bdc5557877505e5d7bac3c19bb1788f1b6b98c329dcddf3626a747bca87a2a03a526688d7a7f854cea3908fc9369fc0e12cd506420013cb702be0215d37d60a2c39
     *       OK
     */
    { AT_CMD_IDENTIFYOTAEND,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IDENTIFYOTAEND", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: OTAдSimLock������������
     * [˵��]: ����ʹ�����ƣ���Ҫͨ��OTA�·��ļ�Ȩ��ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^SIMLOCKOTADATAWRITE=<layer>,<index>,<total>,<simlock_data>[,<hmac>]
     *     [���]: <CR><LF>
     *             ^SIMLOCKOTADATAWRITE:<index><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <layer>: ��Ҫд����һ���������Ϣ��255/0/1/2/3
     *             255��������������ͷ
     *             0��NET
     *             1��NETSUB
     *             2��SP
     *             3��CP
     *     <index>: �ڼ������ݣ�ȡֵ��Χ1-255��Ҫ���1��ʼ������ÿ�ε�����
     *     <total>: �����·�֧�ְַ��·�����Ҫ�ְ����ܰ���ȡֵ��ΧΪ1-255��indexֵ���ܴ���total��
     *     <simlock_data>: SimLock���ݽṹ�Ķ��������������ܰ����������͡�����״̬��������PIN ���������������Ŷ������������Ŷ����鼰��У�����ģ�CK��UKʹ��˽Կ����RSA���ܺ�����ġ�
     *             ����ʱ��ͨ��Half-Byte������ַ����������Ҫ��������ǲ�����SimLock���ݡ�
     *             ��������Ϊ1400���ַ���
     *     <hmac>: δ���ʱ��SimLock�����������Ա���У�����ɵ�ժҪ��Half- Byte�����·������һ������Я��hmac����������Ϊ64���ַ���
     * [ʾ��]:
     *     �� дsimlock�����������ݣ��������·�����
     *       AT^SIMLOCKOTADATAWRITE=0,1,2,012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456
     *       ^SIMLOCKOTADATAWRITE��1
     *       OK
     *       AT^SIMLOCKOTADATAWRITE=0,2,2,012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456,7890123456789012345678901234567012345678901234
     *       ^SIMLOCKOTADATAWRITE��2
     *       OK
     */
    { AT_CMD_SIMLOCKDATAWRITEEX,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMLOCKOTADATAWRITE", (VOS_UINT8 *)SIMLOCKDATAWRITEEX_CMD_PARA_STRING },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ɲ��߼�Ȩ
     * [˵��]: ����������AP-Modem��̬�²����ڼ�Ȩʱ������ֹ��̣��ṩ�����߹���ʹ�á�
     *         ���������ɺ�̨�·�������Ϊ^IDENTIFYSTART����ص�����ͨ��˽Կ���ܳ������������˽Կ���ܺ�����ġ�
     *         ����ʹ�ó��������߹�����ɲ��߼�Ȩ��
     *         ����ʹ�����ƣ��·�^IDENTIFYSTART�������߼�Ȩ��ʹ�ã�ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^IDENTIFYEND=<identify_end_request>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IDENTIFYEND=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <identify_end_ request>: ���߼�Ȩ�������ģ���RSA���ܺ��128�ֽ����ģ�ͨ��Half-Byte�������γ�256���ַ����͸���̨���ߡ�
     *             ���߼�Ȩ�����������ɷ�ʽ��
     *             ͨ��˽Կ��^IDENTIFYSTART����ķ���ֵ���ܻ��һ�������������˽Կ�Ը��������������<identify_end_request>��
     * [ʾ��]:
     *     �� �·���ɲ��߼�Ȩ��������
     *       AT^IDENTIFYEND=948f2efb7b1c4e5d5d8788580091ad5fe39cfd461a7828027fa6b17979212c049bc759ded769f1aa18b3bfc40059b4ff6be7f240f968940fdd69a0f8be992bdc5557877505e5d7bac3c19bb1788f1b6b98c329dcddf3626a747bca87a2a03a526688d7a7f854cea3908fc9369fc0e12cd506420013cb702be0215d37d60a2c39
     *       OK
     *     �� ��������
     *       AT^IDENTIFYEND=?
     *       OK
     */
    { AT_CMD_IDENTIFYEND,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IDENTIFYEND", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: HUK����д����
     * [˵��]: AP-Modem��̬�²���ʹ��^HUKдHUK���ݵ����壬�޲��߼�Ȩ�ͼ��ܵ�Ҫ�������ķ��͡�
     *         ����ʹ�ó�������������ʱʹ�á�
     *         ����ʹ�����ƣ�һ�鵥�������һ��д�룬UE�ܾ��ظ��·���д������ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^HUK=<huk>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^HUK=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <huk>: UE�豸��Ӳ��Ψһ�루Hardware Unique key����
     *             ԭʼHUK��Ϣ��128BIT��AT��������ĸ�ʽΪHalf-Byte����룬��Ҫ2���ַ���ʾһ��8BIT���ݣ���Ҫ32���ַ���ʾHUK��128bit���ݣ�
     * [ʾ��]:
     *     �� д��HUK������
     *       AT^HUK=01010202030304040101020203030404
     *       OK
     */
    { AT_CMD_HUK,
      AT_SetHukPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^HUK", (VOS_UINT8 *)"(@huk)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ò��߼�Ȩ��Կ
     * [˵��]: ���ڲ�Ʒ�߰�ȫ����Ҫ��SHA�㷨��256����Ϊ512��ԭ��FACAUTHPUBKEY�����㰲ȫ������������������������������滻ԭ�������Chicago������ƽ̨֧�ִ����
     *         ����������AP-Modem��̬�����ò��߼�Ȩ�����У�ƽ̨���ڼ���������Ĺ�Կ��
     *         ʹ�ñ�����ǰ��Ҫ��ͨ��AT����^HUK�����豸��HUK�롣
     *         ����ʹ�ó��������߹���ʹ�á�
     *         ����ʹ�����ƣ�ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^FACAUTHPUBKEYEX=<index>,<total>,<pubkey>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FACAUTHPUBKEYEX=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <index>: ��ǰ�����ֶ���������Ч��Χ1~255
     *     <total>: �����ֶ���������Ч��Χ1~255,
     *     <pubkey>: �������ɵ���Կ����������AT�������������Ϊ1600�ֽڣ���˴��������Ȳ��ܳ���1570�ֽ�
     * [ʾ��]:
     *     �� ���ò��߼�Ȩ��Կ(�������·�)
     *       AT^FACAUTHPUBKEYEX=1,2,data1
     *       OK
     *       AT^FACAUTHPUBKEYEX=2,2,data2
     *       OK
     *     �� ��������
     *       AT^FACAUTHPUBKEYEX=?
     *       OK
     */
    { AT_CMD_FACAUTHPUBKEYEX,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FACAUTHPUBKEYEX", (VOS_UINT8 *)"(1-20),(1-20),(@Pubkey)" },

#if (FEATURE_PHONE_SC == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ������������
     * [˵��]: ����������AP-Modem��̬�½���SimLock�����������ܡ���������(U)SIM�������Ϸ�(U)SIM��������¶�SimLock�����������ܽ��н����������ý����������ܵ�ǰ��״̬�����ƣ���������������������δ�����ѽ�����ֱ�ӷ���OK��
     *         ������ֻ����AP�Խӵ�ר��ATͨ���յ���Ч������ATͨ�����յ�����Ϊʧ�ܡ�
     *         �ڲ���Ƿ�(U)SIM���������ʹ�ô����������Ҫ����UE�������Ч��
     * [�﷨]:
     *     [����]: ^SIMLOCKUNLOCK=<cat>,<password>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SIMLOCKUNLOCK=?
     *     [���]: <CR><LF>^SIMLOCKUNLOCK: (list of supported <cat>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <cat>: �ַ������ͣ������������ͣ�Ŀǰ��֧�����������������ͣ�
     *             NET�������磻
     *             NETSUB����������
     *             SP����SP��
     *     <password>: �ַ������ͣ�������Ӧ�����������͵�PIN���PUK�롣����Ϊ�̶�����16�������ַ�����ÿ���ַ���ȡֵ��ΧΪ��0��~��9����
     * [ʾ��]:
     *     �� ����NET����
     *       AT^SIMLOCKUNLOCK="NET","0123456789012345"
     *       OK
     *     �� ִ�в�������
     *       AT^SIMLOCKUNLOCK=?
     *       ^SIMLOCKUNLOCK: ("NET","NETSUB","SP")
     *       OK
     */
    { AT_CMD_SIMLOCKUNLOCK,
      AT_SetSimlockUnlockPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestSimlockUnlockPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMLOCKUNLOCK", (VOS_UINT8 *)"(\"NET\",\"NETSUB\",\"SP\"),(pwd)" },
#endif

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �л�ATͨ����OMͨ��
     * [˵��]: ���������ڽ�ATͨ������OMͨ�����л���ö˿�AT����ʧЧ��
     * [�﷨]:
     *     [����]: ^AT2OM
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>CME ERROR: <err><CR><LF>
     * [ʾ��]:
     *     �� ����ATͨ������OMͨ��
     *       At^AT2OM
     *       OK
     */
    { AT_CMD_AT2OM,
      At_SetAt2OmPara, AT_NOT_SET_TIME, TAF_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_STICK,
      (TAF_UINT8 *)"^AT2OM", TAF_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: дSimLock������������
     * [˵��]: ����������AP-Modem��̬��дSimLock�����������ݣ�д��������ʱ����ͷ������дͷ���ݣ���дÿһ���������ݡ���������ͷ���ݣ�����У�������������������SP����CP����������˳��д�롣һ�ν��������һ���������������������루���һ��AT�����·�������Էְ��·�����
     *         ��������Ҫ���߼�Ȩͨ������ִ�У����õ�����������Ϣ�������������ʹ�á�
     *         ����ʹ�ó��������߹����·���
     *         ����ʹ�����ƣ���Ҫͨ�����߼�Ȩ��ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^SIMLOCKDATAWRITEEX=<layer>,<index>,<total>,<simlock_data>[,<hmac>]
     *     [���]: <CR><LF>
     *             ^SIMLOCKDATAWRITEEX: <index><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SIMLOCKDATAWRITE=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <layer>: ��Ҫд����һ���������Ϣ��255/0/1/2/3
     *             255��������������ͷ
     *             0��NET
     *             1��NETSUB
     *             2��SP
     *             3��CP
     *     <index>: �ڼ������ݣ�ȡֵ��Χ1-255��Ҫ���1��ʼ������ÿ�ε�����
     *     <total>: �����·�֧�ְַ��·�����Ҫ�ְ����ܰ���ȡֵ��ΧΪ1-255��indexֵ���ܴ���taotal��
     *     <simlock_data>: SimLock���ݽṹ�Ķ��������������ܰ����������͡�����״̬��������PIN ���������������Ŷ������������Ŷ����鼰��У�����ģ�CK��UKʹ��˽Կ����RSA���ܺ�����ġ�
     *             ����ʱ��ͨ��Half-Byte������ַ����������Ҫ��������ǲ�����SimLock���ݡ�
     *             ��������Ϊ1400���ַ���
     *     <hmac>: δ���ʱ��SimLock�����������Ա���У�����ɵ�ժҪ��Half- Byte�����·������һ������Я��hmac����������Ϊ64���ַ���
     * [ʾ��]:
     *     �� дsimlock�����������ݣ��������·�����
     *       AT^SIMLOCKDATAWRITEEX=0,1,2,
     *       012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456
     *       ^SIMLOCKDATAWRITEEX: 1
     *       OK
     *       AT^SIMLOCKDATAWRITEEX=0,2,2,
     *       012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456,7890123456789012345678901234567012345678901234
     *       ^SIMLOCKDATAWRITEEX: 2
     *       OK
     *     �� �쳣������index���Ǵ�1��ʼ��index����total����
     *       AT^SIMLOCKDATAWRITEEX=0,3,2,
     *       012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456
     *       +CME��ERROR
     *     �� ��������
     *       AT^SIMLOCKDATAWRITEEX=?
     *       OK
     */
    { AT_CMD_SIMLOCKDATAWRITEEX,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMLOCKDATAWRITEEX", (VOS_UINT8 *)SIMLOCKDATAWRITEEX_CMD_PARA_STRING },

#if (FEATURE_PHONE_SC == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯ�ֻ���PAD��̬SIMLOCK����������Ϣ
     * [˵��]: ����������AP-Modem��̬�²�ѯ�ֻ���PAD��SIMLOCK����������Ϣ���ṩ������ʹ�ã����ڻض�У��д���ֻ�������������Ϣ����ѯ����Ϣ��AT^SIMLOCKDATAWRITEEX��������ȶԡ�
     *         ����ʹ�ó��������߹��ߡ�
     *         ����ʹ�����ƣ�����Ҫ���߼�Ȩ���ɲ�ѯ��ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^SIMLOCKDATAREADEX=<layer>,<index>
     *     [���]: <CR><LF>
     *             ^SIMLOCKDATAREADEX: <layer>,<index>,<total>,<simlock_data>
     *             <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SIMLOCKDATAREADEX=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <layer>: ��Ҫд����һ���������Ϣ��255/0/1/2/3
     *             255��������������ͷ
     *             0��NET
     *             1��NETSUB
     *             2��SP
     *             3��CP
     *     <index>: ��ǰ�ϱ����ڵڼ������ݣ���1��ʼ��
     *     <total>: �����ݰ�����ÿ��Я������ַ���Ϊ1400.
     *     <simlock_data>: ��������������
     * [ʾ��]:
     *     �� ��ѯ�ֻ�SIMLOCK����������Ϣ
     *       AT^SIMLOCKDATAREADEX=0,1
     *       ^SIMLOCKDATAREADEX: 0,1,1,
     *       012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456
     *       OK
     *     �� ��������
     *       AT^SIMLOCKDATAREADEX=?
     *       OK
     */
    { AT_CMD_SIMLOCKDATAREADEX,
      AT_SimLockDataReadExPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMLOCKDATAREADEX", (VOS_UINT8 *)SIMLOCKDATAREADEX_CMD_PARA_STRING },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: eSIM�п�
     * [˵��]: ������ʵ��eUICC��UICC�ĵ�·�л���
     *         ������ֻ�е���Ӧ���ۿ�ȥ������������޿�ʱ�����·���
     * [�﷨]:
     *     [����]: ^ESIMSWITCH=<sci_num>,<card_type>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^ESIMSWITCH?
     *     [���]: <CR><LF>^ESIMSWITCH: < card_type >,< card_type > CR><LF> <CR><LF>OK<CR><LF>
     *     [����]: ^ESIMSWITCH=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <sci_num>: ���ۺţ�Ŀǰȡֵ0����1��������0���߿���1����
     *     <card_type>: ���л����ĵ�·���ͣ�Ŀǰȡֵ0����1�������л���UICC��·����eUICC��·����
     * [ʾ��]:
     *     �� ����1��UICC��·�л���eUICC��·
     *       AT^ESIMSWITCH=1,1
     *       OK
     *     �� ��ѯ��ǰ��·���ͣ�����0/1 ������UICC��·
     *       AT^ESIMSWITCH?
     *       ^ESIMSWITCH: 0,0
     *       OK
     *     �� ��ѯ��ǰ��·���ͣ�����0 UICC��·������1 eUICC��·
     *       AT^ESIMSWITCH?
     *       ^ESIMSWITCH: 0,1
     *       OK
     *     �� ����ESIMSWITCH
     *       AT^ESIMSWITCH=?
     *       OK
     */
    { AT_CMD_ESIMSWITCH,
      At_SetEsimSwitchPara, AT_SET_PARA_TIME, At_QryEsimSwitch, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ESIMSWITCH", (VOS_UINT8 *)"(0,1),(0,1)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: Profile���
     * [˵��]: ��CW��λʹ��������� AT^ESIMCLEAN ���eSIM�������ݡ�
     *         ������ֻ������CW��λCP��Ȩ��ʹ�á�
     * [�﷨]:
     *     [����]: ^ESIMCLEAN
     *     [���]: CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� ���eSIM�ڴ����
     *       AT^ESIMCLEAN
     *       OK
     */
    { AT_CMD_ESIMCLEAN,
      At_SetEsimCleanProfile, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ESIMCLEAN", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���Profile�Ƿ�Ϊ��
     * [˵��]: ��CW��λʹ�� AT^ESIMCHECK? ���ʹ��֮ǰ��ҪSIM�����һ��������������ȡeSIM���е�Profile��Ϣ��
     * [�﷨]:
     *     [����]: ^ESIMCHECK?
     *     [���]: <CR><LF>^ESIMCHECK: <result>CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <result>: 0��ʾû��Profile�ļ���1��ʾ��Profile�ļ���
     * [ʾ��]:
     *     �� ���Profile����
     *       AT^ESIMCHECK?
     *       ^ESIMCHECK: 0
     *       OK
     */
    { AT_CMD_ESIMCHECK,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryEsimCheckProfile, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ESIMCHECK", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡESIM EID
     * [˵��]: ����ʹ�ã���ѯeSIMоƬEID��Ϣ��
     * [�﷨]:
     *     [����]: ^ESIMEID?
     *     [���]: <CR><LF>^ESIMEID: <eid>
     *             CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <eid>: eSIMоƬ��EID��Ϣ��32�����֡�
     * [ʾ��]:
     *     �� ��ѯEID����
     *       AT^ESIMEID?
     *       ^ESIMEID: "89033024010400000100000000005353"
     *       OK
     */
    { AT_CMD_ESIMEID,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryEsimEid, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ESIMEID", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡESIM PKID
     * [˵��]: ����ʹ�ã���ѯeSIMоƬ��PKID��ֵ��
     * [�﷨]:
     *     [����]: ^PKID?
     *     [���]: <CR><LF>^PKID: <num>,<pkid_1>,��,<pkid_n>
     *             CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <num>: PKID�ĸ��������֧��10��
     *     <pkid_n>: �����PKID��ֵ��
     * [ʾ��]:
     *     �� ��ѯPKID����
     *       AT^PKID?
     *       ^PKID: 2,"041481370F5125D0B1D408D4C3B232E6D25E795BEBFB","041416B5D16048E3EA02BD4B606E5F77A4BF20808D83"
     *       OK
     */
    { AT_CMD_PKID,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryEsimPKID, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PKID", VOS_NULL_PTR },

#if (FEATURE_SC_NETWORK_UPDATE == FEATURE_ON)
    { AT_CMD_SIMLOCKNWDATAWRITE,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMLOCKNWDATAWRITE", (VOS_UINT8 *)SIMLOCKDATAWRITEEX_CMD_PARA_STRING },
#endif

#if (FEATURE_PHONE_SC == FEATURE_ON)
    { AT_CMD_GETMODEMSCID,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryModemScIdPara, AT_QRY_PARA_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^GETMODEMSCID", VOS_NULL_PTR },
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: IC DIE ID��ȡ����
     * [˵��]: ���������ڶ�ȡ����оƬ��DIE ID��
     * [�﷨]:
     *     [����]: ^RFICIDEX?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^RFICIDEX: <index>,<type>,<die id><CR><LF>
     *             <CR><LF>^RFICIDEX: <index>,<type>,<die id><CR><LF>
     *             ����
     *             <CR><LF>OK<CR><LF>
     *             ���ײ㷵��ʧ��ʱ���أ�
     *             <CR><LF>^RFICIDEX: <err_code><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <index>: �ϱ�����ţ�0��1��2��3����
     *     <type>: оƬ���ͣ���������ɵײ��AP���߹���Լ��
     *     <die id>: �ַ������ͣ�die id������16�����������ͬIC���Ȳ�ͬ
     *     <err_code>: �ײ������
     * [ʾ��]:
     * [ʾ��]:
     *       AT^RFICIDEX?
     *       ^RFICIDEX: 0,0,
     *       "0000000000000000000000000000000000000000000000000000000000000000"
     *       ^RFICIDEX: 1,1, "0000000000000000000000000000000000000000000000000000000000000000"
     *       ^RFICIDEX: 2,1, "0000000000000000000000000000000000000000000000000000000000000000"
     *       OK
     */
    { AT_CMD_RFICIDEX,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryRficDieIDExPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RFICIDEX", VOS_NULL_PTR },
#endif

};

/* ע��PAMװ��AT����� */
VOS_UINT32 AT_RegisterDevicePamCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDevicePamCmdTbl, sizeof(g_atDevicePamCmdTbl) / sizeof(g_atDevicePamCmdTbl[0]));
}

