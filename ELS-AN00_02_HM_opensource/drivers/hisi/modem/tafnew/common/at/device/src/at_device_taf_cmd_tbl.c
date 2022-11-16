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
#include "at_device_taf_cmd_tbl.h"

#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"
#include "at_device_comm.h"

#include "at_device_taf_set_cmd_proc.h"
#include "at_device_taf_qry_cmd_proc.h"



#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_TAF_CMD_TBL_C

static const AT_ParCmdElement g_atDeviceTafCmdTbl[] = {
#if ((FEATURE_UE_MODE_NR == FEATURE_ON) && defined(FEATURE_PHONE_ENG_AT_CMD) && \
     (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: UARTͨ�ϲ��Կ��Ƽ����Խ����ѯ
     * [˵��]: ����������UARTͨ�ϲ��Կ��Ƽ����Խ����ѯ��
     * [�﷨]:
     *     [����]: ^UARTTEST
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�в�ѯ�ɹ�ʱ��
     *             <CR><LF>^UARTTEST: <result><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <result>: ���Խ��
     *             0���ɹ�����ѯһ��֮��������ó�δ���ԣ�
     *             1������У��ʧ�ܣ���ѯһ��֮��������ó�δ���ԣ�
     *             2�����Գ�ʱ����ѯһ��֮��������ó�δ���ԣ�
     *             3��ICCû��������ѯһ��֮��������ó�δ���ԣ�
     *             254�����Խ����û�з���
     *             255��δ����
     * [ʾ��]:
     *     �� UARTͨ�ϲ��Կ���
     *       AT^UARTTEST
     *       OK
     *     �� ��ѯUARTͨ�ϲ��Խ��
     *       AT^UARTTEST?
     *       ^UARTTEST: 0
     *       OK
     */
    { AT_CMD_UARTTEST,
      AT_SetUartTest, AT_SET_PARA_TIME, AT_QryUartTest, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^UARTTEST", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: I2Sͨ�ϲ��Կ��Ƽ����Խ����ѯ
     * [˵��]: ����������I2Sͨ�ϲ��Կ��Ƽ����Խ����ѯ��
     * [�﷨]:
     *     [����]: ^I2STEST
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�в�ѯ�ɹ�ʱ��
     *             <CR><LF>^I2STEST: <result><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <result>: ���Խ��
     *             0���ɹ�����ѯһ��֮��������ó�δ���ԣ�
     *             1��ʧ�ܣ���ѯһ��֮��������ó�δ���ԣ�
     *             2�����Գ�ʱ����ѯһ��֮��������ó�δ���ԣ�
     *             254�����Խ����û�з���
     *             255��δ����
     * [ʾ��]:
     *     �� I2Sͨ�ϲ��Կ���
     *       AT^I2STEST
     *       OK
     *     �� ��ѯI2Sͨ�ϲ��Խ��
     *       AT^I2STEST?
     *       ^I2STEST: 0
     *       OK
     */
    { AT_CMD_I2STEST,
      AT_SetI2sTest, AT_SET_PARA_TIME, AT_QryI2sTest, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^I2STEST", VOS_NULL_PTR },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯָ��NV���ֵ
     * [˵��]: ��NV�
     * [�﷨]:
     *     [����]: ^NVRD=<Nvid>
     *     [���]: <CR><LF>^NVRD: <length>,<data><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <Nvid>: U16�����ͣ���ʾNV���ID��ֻ֧��10���Ƽ�16���ƣ�0x��ͷ����
     *     <length>: 0��512�����ͣ���ʾ<data>��ռ���ֽڵĳ��ȡ�
     *     <data>: ʮ�������ͣ���ʾд��NV���������ݣ��ڶ��������޴˲���������������С��˳���ֽ��Կո�ָ�ֽ������Ϊ128����
     * [ʾ��]:
     *       mnc���ȴ洢��NV�У�NvidΪ0x2757��
     *       NV��Ľṹ�嶨��Ϊ��
     *       typedef struct
     *       {
     *           VOS_INT16                           shwDcxoC2fix;
     *       }UCOM_NV_DCXO_C2_FIX_STRU;
     *     �� ��mnc���ȵ�AT����Ϊ��
     *       AT^NVRD=10071
     *       ^NVRD: 2,03 00
     *       OK
     */
    { AT_CMD_NVRD,
      AT_SetNVReadPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NVRD", (VOS_UINT8 *)"(0-65535)" },


#if (FEATURE_LTE == FEATURE_ON)
    /* BEGIN: LTE ����У׼װ��AT���� */
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡNV����
     * [˵��]: ��ȡNV����ֽ������ȡ�
     * [�﷨]:
     *     [����]: ^NVRDLEN=<Nvid>
     *     [���]: <CR><LF>^NVRDLEN: <Nvid>:< length ><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR:< err_code ><CR><LF>
     * [����]:
     *     <Nvid>: U16�����ͣ���ʾNV���ID��ֻ֧��10���Ƽ�16���ƣ�0x��ͷ����
     *     <length>: �������ͣ���ʾ���ص������ֽ�����
     *     <err_code>: ������ȡֵ��
     *             0��NV������ ��
     *             1����ȡNV�ֽ�������ʧ�ܣ�
     *             2����������
     */
    { AT_CMD_NVRDLEN,
      atSetNVRDLenPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NVRDLEN", (VOS_UINT8 *)"(0-65535)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡ��NV����
     * [˵��]: ��ȡNV�����չ���֧��ָ��NV���ݵ�ƫ��λ�ã������ڴ�NV����С����2048�ֽڣ��Ķ�ȡ������
     * [�﷨]:
     *     [����]: ^NVRDEX=<Nvid>,<offset>,<length>
     *     [���]: <CR><LF>^NVRDEX: < Nvid >,< offset >,< length >,< data ><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR:< err_code ><CR><LF>
     * [����]:
     *     <Nvid>: U16�����ͣ���ʾNV���ID��ֻ֧��10���Ƽ�16���ƣ�0x��ͷ����
     *     <offfset>: ���ڵ���0����������ʾ���ζ�ȡ��data��Nvid�����е�ƫ��λ�á�
     *     <length>: 0��512�����ͣ���ʾ���ص������ֽ��������صĳ��ȿ���С���·��ĳ��ȡ�
     *             length���鲻����512�����ڲ�ͬ������˿ڣ�PCUI��MODEM��UART��VCOM��buff��С��ͬ��ʹ�ò�ͬ���͵Ķ˿ڶ�ȡNVʱ�п������ڸö˿�buff��С�����޷��ϱ���PCUI�ڿ��Զ�ȡʱ����NV IDȡֵ�����Զ�ȡ�Ĵ�С��1655~1658�ֽڡ�
     *     <data>: ���ص����ݣ���ʮ�������ַ���������ֽ�֮���Կո�ָ�������������ΪС��˳��
     *     <err_code>: ������ȡֵ��
     *             0��NV�����ڣ�
     *             1��ƫ��ֵ�Ƿ���
     *             2�����ݳ��Ȳ��ԣ�
     *             3����ȡNVʧ�ܣ�
     *             4����������
     */
    { AT_CMD_NVRDEX,
      atSetNVRDExPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NVRDEX", (VOS_UINT8 *)"(0-65535),(0-2048),(0-2048)" },

#endif


    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���úͲ�ѯ���������
     * [˵��]: �������úͲ�ѯ��������ţ�����Ʒ�ķǳ�Ʒ�ĵ�������š�����Ʒ��֧�ֿ�ֱ�ӷ���ERROR����
     * [�﷨]:
     *     [����]: ^BSN=<BSN>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^BSN?
     *     [���]: <CR><LF>^BSN: <BSN><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <BSN>: ��Ʒ�ĵ�������ţ���ʽΪXXXXXXXXXXXXXXXX����ĸ�����ͣ�����16λ�������磺�����ݿ���SN��Ϊ0391831057000001ʱ�������Ϊ��0391831057000001��
     * [ʾ��]:
     *     �� ���õ��������
     *       AT^BSN=0391831057000001
     *       OK
     */
    { AT_CMD_BSN,
      At_SetBsn, AT_NOT_SET_TIME, At_QryBsn, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^BSN", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡ����ƽ̨��Ϣ
     * [˵��]: ��ѯ��Ʒʹ�õ�ƽ̨��Ϣ��
     * [�﷨]:
     *     [����]: ^PLATFORM?
     *     [���]: <CR><LF>^PLATFORM: <providor>,<sub_platform_info> <CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <providor>: ������Ӧ����Ϣ��ʹ�����ֱ�ʾ������Ϊһ���ֽڣ�ȡֵ��ΧΪ0��255��
     *             1����Ϊ��
     *     <sub_platform _info>: ��ƽ̨��Ϣ��ȡֵ��ΧΪ0��255��
     *             ÿ���������Ҷ���0��ʼ��ţ���ͬ�������ұ�ſ����ظ�������Ϊһ���ֽڡ����������ƽ̨�ֻ�������Ƶƽ̨���Ի���ƽ̨Ϊ������ݡ�Ϊ�˱���ͳһ���������Ҫ��װ����Ա���롣
     *     <err>: �����롣
     *             0���޷���ȡƽ̨��Ϣ��
     *             1����������
     * [ʾ��]:
     *     �� ��ѯ��Ʒʹ�õ�ƽ̨��Ϣ
     *       AT^PLATFORM?
     *       ^PLATFORM: 1,71
     *       OK
     */
    { AT_CMD_PLATFORM,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryPlatForm, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PLATFORM", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ò�Ʒ���������
     * [˵��]: ������ѯ�������ò�Ʒ��SN�ţ�����Ʒ�ĳ�Ʒ������š�
     * [�﷨]:
     *     [����]: ^SN
     *     [���]: <CR><LF>^SN: <SN><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^SN?
     *     [���]: <CR><LF>ERROR<CR><LF>
     *     [����]: ^SN=?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^SN=<SN>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     * [����]:
     *     <SN>: ��Ʒ�ĳ�Ʒ������š�
     *             ��ʽ��03XXXXXXXXXXXXXX����ĸ�����ͣ�����16λ����
     *     <err>: �����롣
     *             0�����볤�ȴ���
     *             1����������
     * [��]: ���磺��MT��SN��Ϊ0391831057000001ʱ�����±��
     *       0, 3, 9, 1, 8, 3, 1, 0, 5, 7, 0, 0, 0, 0, 0, 1,
     * [��]: NV_FACTORY_INFO_I��ΪSN��Ԥ����20byte�ռ䣬��SN�Ų���20byteʱ��������ֽ����0xFF������洢��ʽ���±��
     *       30, 33, 39, 31, 38, 33, 31, 30, 35, 37, 30, 30, 30, 30, 30, 31, FF, FF, FF, FF,
     * [ʾ��]:
     *     �� ��Ʒ�����������óɹ�
     *       AT^SN=0391831057000001
     *       OK
     *     �� ��ѯ��Ʒ�ĳ�Ʒ�����
     *       AT^SN
     *       ^SN: 0391831057000001
     *       OK
     *     �� ����^SN
     *       AT^SN=?
     *       OK
     */
    { AT_CMD_SN,
      At_SetSnPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_DEVICE_OTHER_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^SN", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ò���ģʽ
     * [˵��]: ��������MT�Ĳ���ģʽ��
     * [�﷨]:
     *     [����]: ^TMODE=<test_mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^TMODE?
     *     [���]: <CR><LF>^TMODE: <status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^TMODE=?
     *     [���]: <CR><LF>^TMODE: <test_mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <test_mode>: ����ģʽ��
     *             0������ģʽ����������������ģʽ�£������л���ģʽ1��ģʽ2��
     *             1��������ģʽ�����Խ���RF����ķ�������ƣ�����ֱ���л���ģʽ2�������л���������ģʽ��Ĭ����Ƶ��·���رգ������Ʒ֧��LTE-V��������ģʽ��Ҫͬʱ����AT^VMODE=0,0
     *             2������ģʽ�������������������ֱ���л���ģʽ1��
     *             3������ģʽ�����������������
     *             4���ػ�����Ҫ����֮���ִ�йػ�������������E5����
     *             11��֧��GSM��WCDMA��LTE���ֽ��뼼���ķ�����ģʽ��
     *             12��֧��GSM��WCDMA��LTE���ֽ��뼼��������ģʽ��
     * [ʾ��]:
     *     �� ������յ��Զ��ػ��µ�ָʾ���Ȼظ�OK��Ȼ��ִ�йػ��µ����
     *       AT^TMODE=4
     *       OK
     *     �� ���÷�����ģʽ
     *       AT^TMODE=1
     *       OK
     *     ��
     *       AT^VMODE=0,0�������Ʒ֧��LTE-V����Ҫ�·���AT����֤ȫ�����������ģʽ��
     *       OK
     */
    { AT_CMD_TMODE,
      At_SetTModePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestTmodePara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TMODE", (VOS_UINT8 *)"(0,1,2,3,4,11,12,13,14,15,16,19)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����MMI���Խ��
     * [˵��]: ���������������Ҫ����MMI���Ե��ն˲�Ʒ��
     *         ����ָ���������ò��Խ�����ֶ�����ʱ����������Զ�������Խ����
     *         ��ѯָ�����ڼ���ֶ�������������
     * [�﷨]:
     *     [����]: ^TMMI=<result>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^TMMI?
     *     [���]: <CR><LF>^TMMI: <result><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <result>: ���Խ����
     *             0��ʧ�ܣ�
     *             1���ɹ���
     */
    { AT_CMD_TMMI,
      AT_SetTmmiPara, AT_NOT_SET_TIME, AT_QryTmmiPara, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TMMI", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����������
     * [˵��]: ������������ڴ������ն˲�Ʒ�����ڲ��Ը���������ʾ���ܡ�
     * [�﷨]:
     *     [����]: ^TSCREEN=<pattern>[,<index>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <pattern>: ���Ĳ���ͼ����ʽ��ȡֵ��ΧΪ0~255��
     *     <index>: ���ڶ�����Ʒ���ԣ�ָ����ǰ����������š�ȡֵ��Χ0~255��
     */
    { AT_CMD_TSCREEN,
      AT_SetTestScreenPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TSCREEN", (VOS_UINT8 *)"(0-255),(0-255)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ò�Ʒ����
     * [˵��]: ���á���ȡ��Ʒ���ơ�
     * [�﷨]:
     *     [����]: ^PRODNAME=<dutname>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^PRODNAME?
     *     [���]: <CR><LF>^PRODNAME: <dut name><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <dut name>: ��Ʒ���ƻ��ͺţ�����˫���ţ����29�ַ����������ֱ��ضϡ�
     * [ʾ��]:
     *     �� ���ò�Ʒ���Ƴɹ�
     *       AT^PRODNAME=balong
     *       OK
     *     �� ��ѯ��Ʒ���Ƴɹ�
     *       AT^PRODNAME?
     *       ^PRODNAME: balong
     *       OK
     */
    { AT_CMD_PRODNAME,
      AT_SetProdNamePara, AT_NOT_SET_TIME, AT_QryProdNamePara, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PRODNAME", (VOS_UINT8 *)"(@ProductName)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯװ����һ��AT����汾��
     * [˵��]: �����װ����һ��AT����İ汾�ţ����ڲ�ͬ�汾��ļ�����ơ�����Ʒ��֧��ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^EQVER?
     *     [���]: <CR><LF>^EQVER: <version><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <version>: װ����һ��AT����汾�š��汾������λ��ʾ��ÿλΪ0��9�������ַ����汾��ȡ���ڵ���ʵ�ֵ�AT����ο����ĵ��汾�š�
     *             114����һ���汾��
     *             ���ε������ƣ��̶�����115��
     * [ʾ��]:
     *     �� ��ѯװ����һ��AT����汾������
     *       AT^EQVER?
     *       ^EQVER: 115
     *       OK
     */
    { AT_CMD_EQVER,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryEqverPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^EQVER", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����������
     * [˵��]: �����뱣�������ݽ��н���������Ʒ��֧�ֿ�ֱ�ӷ���ERROR���������Ч��������������Ҫ�ٴν�����
     * [�﷨]:
     *     [����]: ^DATALOCK=<unlock_code>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^DATALOCK?
     *     [���]: <CR><LF>^DATALOCK: <status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <unlock_code>: �����룬���ڽ������д������
     *     <status>: ���ݱ�������״̬��
     *             0������״̬�����Բ����������ݣ�
     *             1����״̬Ϊ�ǽ��״̬���޷������������ݡ�
     * [ʾ��]:
     *     �� ��ѯ������״̬
     *       AT^DATALOCK?
     *       ^DATALOCK: 1
     *       OK
     */
    { AT_CMD_DATALOCK,
      At_SetDataLock, AT_SET_PARA_TIME, At_QryDataLock, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DATALOCK", (VOS_UINT8 *)"(@nlockCode)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯ�ͽ���SIMLOCK
     * [˵��]: ����SIMLOCK���ܺͲ�ѯSIMLOCK���ܵ�ʹ�����������ʱ������������������Σ��������塣����ʧ�ܴ�����������Խ��������������������״̬������Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^SIMLOCK=<opr>[,<param>]
     *     [���]: ��<opr>=0��1��ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��<opr>=2������ɹ�ִ��ʱ��
     *             <CR><LF>^SIMLOCK: <status><CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^SIMLOCK?
     *     [���]: <CR><LF>^SIMLOCK: <plmn-num> [<CR><LF>^SIMLOCK: <index>,<mnc-digital-num>,<begin>,<end><CR><LF>]
     *             [....]
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <opr>: �������͡�
     *             0������SIMLOCK��
     *             <param>��������������ý������룬8λ�ַ�������0~9������϶��ɡ�����<opr>=0ʱ����Ҫ�������롣
     *             1������SIMLOCK�ŶΡ�
     *             <param>��SIMLOCK�ŶΣ�������֧��20���ŶΡ�ÿ���Ŷ����ݸ�ʽΪ<mnc-digital-num>,<begin>, <end>������Ŷ�֮���ö��Ÿ�����[,<mnc-digital-num>,<begin>,<end>]��
     *             2����ѯSIMLOCK���ܵ�ʹ�������
     *     <status>: ����״̬��
     *             0��δ����SIMLOCK���ܣ�
     *             1��������SIMLOCK���ܡ�
     *     <plmn-num>: �����Ŷ�������
     *     <index>: ������������Ϊ0��20�����֡����Ϊ0��ֱ����ʾOK��
     *     <mnc-digital-num>: MNCλ����ȡֵ��ΧΪ����2��������3��
     *     <begin>: ��ʼ���ݣ�Ϊ0��9������ɵ��ַ�����
     *     <end>: �������ݣ�Ϊ0��9������ɵ��ַ�����
     *     <err>: �����롣
     *             0��д����ʱ�����ݱ���δ�������޷������������ݣ�
     *             1��������SIMLOCK���ܣ�
     *             2������ĺŶζ���20�Σ�
     *             3��<mnc-digital-num>���볬����Χ��
     * [ʾ��]:
     *     �� SIMLOCK�����ɹ�
     *       AT^SIMLOCK=0,12345678
     *       OK
     */
    { AT_CMD_SIMLOCK,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QrySimLockPlmnInfo, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMLOCK", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �����������
     * [˵��]: �������������úͲ�ѯSIMLOCK����������
     * [�﷨]:
     *     [����]: ^MAXLCKTMS=<number>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^MAXLCKTMS?
     *     [���]: <CR><LF>^MAXLCKTMS: <number><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^MAXLCKTMS=?
     *     [���]: <CR><LF>^MAXLCKTMS: (list of supported <number>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <number>: ����ֵ���������������Ϊ�Ǹ�����, (0-429496728)��
     *     <err>: �����롣
     *             0�����ݱ���δ���������ʧ�ܣ�
     *             1���������ô���
     * [ʾ��]:
     *     �� ����SIMLOCK��������Ϊ1��
     *       AT^MAXLCKTMS=1
     *       OK
     */
    { AT_CMD_MAXLCK_TIMES,
      At_SetMaxLockTimes, AT_SET_PARA_TIME, At_QryMaxLockTimes, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^MAXLCKTMS", (VOS_UINT8 *)"(0-429496728)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ú��з���
     * [˵��]: ���ú��з�����Ϣ������Ʒ��֧��ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^CALLSRV=<service>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CALLSRV?
     *     [���]: <CR><LF>^CALLSRV: <service><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CALLSRV=?
     *     [���]: <CR><LF>^CALLSRV: (list of supported < service >s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <service>: ������Ϣ������Ϊһ���ֽڡ�
     *     <err>: �����롣
     *             0�����ݱ���δ���������ʧ�ܣ�
     *             1���������ô���
     * [ʾ��]:
     *     �� ���ú��з���ɹ�
     *       AT^CALLSRV=1
     *       OK
     *     �� ��������
     *       AT^CALLSRV=?
     *       ^CALLSRV: (0,1)
     *       OK
     */
    { AT_CMD_CALLSRV,
      At_SetCallSrvPara, AT_NOT_SET_TIME, At_QryCallSrvPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CALLSRV", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ������������
     * [˵��]: ���á���ȡ�������ڡ�
     * [�﷨]:
     *     [����]: ^MDATE=<date_info>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^MDATE?
     *     [���]: <CR><LF>^MDATE: <date_info><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^MDATE=?
     *     [���]: <CR><LF>^MDATE: (@time)<PL><CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <date_info>: �������ڣ���ʽ��2009-12-31 10:10:10��
     * [ʾ��]:
     *     �� ��ȡ��������
     *       AT^MDATE?
     *       ^MDATE: 2009-12-31 10:10:10
     *       OK
     *     �� ִ�в�������
     *       AT^MDATE=?
     *       ^MDATE: (@time)
     *       OK
     */
    { AT_CMD_MDATE,
      AT_SetMDatePara, AT_NOT_SET_TIME, AT_QryMDatePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^MDATE", (VOS_UINT8 *)"(@time)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����������Ϣ
     * [˵��]: ���á���ȡ������Ϣָ�
     *         ������Ϣָװ��д�����������ص���Ϣ��������ʱ�䡢��λʶ�����Լ�������װ���ڲ�ʹ�õ���Ϣ��
     * [�﷨]:
     *     [����]: ^FACINFO=<index>,<value_info>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^FACINFO?
     *     [���]: <CR><LF>^FACINFO: 0,<info0><CR><LF>
     *             <CR><LF>^FACINFO: 1,<info1><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <index>: ������Ϣ������
     *     <value_info>: ÿ�ζ�����Ϣ�����ݣ�ȡֵ��ΧΪ<info0>��<info1>��
     *             <info0>���ֶ�0�Ķ������ݣ�����Ϊ128�ֽڣ�ÿ���ֽ���ʮ���Ʊ�ʾ��
     *             <info1>���ֶ�1�Ķ������ݣ�����Ϊ128�ֽڣ�ÿ���ֽ���ʮ���Ʊ�ʾ��
     * [ʾ��]:
     *     �� ��ȡ������Ϣ
     *       AT^FACINFO?
     *       ^FACINFO: 0,
     *       ^FACINFO: 1,
     *       OK
     */
    { AT_CMD_FACINFO,
      AT_SetFacInfoPara, AT_NOT_SET_TIME, AT_QryFacInfoPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FACINFO", (VOS_UINT8 *)"(0,1),(@valueInfo)" },


    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ò�ѯ�����
     * [˵��]: ����������д����߲�ѯ����š�����Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^PHYNUM=<type>,<number>[,<times>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^PHYNUM?
     *     [���]: <CR><LF>^PHYNUM: <type>,<number><CR><LF>
     *             ����ʾ����֧�ֵ�����ţ�
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <type>: ����ŵ����͡�
     *             IMEI������IMEI��
     *             MACWLAN��MAC��ַ��
     *             SVN(����SVN)
     *             ESN(�ݲ�֧��)
     *             MEID���ݲ�֧�֣�
     *             UMID���ݲ�֧�֣�
     *     <number>: ����ţ����ݾ����������ƣ���ʽ����3GPP��ع涨������MAC������Ч�Լ�飬��Ϊһֱ��Ч��
     *     <times>: ����������
     *             0����һ��д��
     *             1����д����д����ŵ�AT��Ҫ��������ָ��^DATALOCK��ִ�С�
     *     <err>: �����롣
     *             0������ų��ȴ���
     *             1������Ų��Ϸ���
     *             2����������ʹ���
     * [ʾ��]:
     *     �� ���������
     *       AT^PHYNUM=MACWLAN,7AFEE22111E4
     *       OK
     *     �� ��ѯ����
     *       AT^PHYNUM?
     *       ^PHYNUM: IMEI,123456789012345
     *       ^PHYNUM: MACWLAN,7AFEE22111E4
     *       ^PHYNUM: SVN,12
     *       OK
     */
    { AT_CMD_PHYNUM,
      AT_SetPhyNumPara, AT_NOT_SET_TIME, AT_QryPhyNumPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PHYNUM", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯ���ư汾��
     * [˵��]: ��������ݿ����ư汾�ţ����ư汾��ָ��Ӧ����֧�ֵĶ��ƽ׶Ρ�����Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^CSVER?
     *     [���]: <CR><LF>^CSVER: <csver><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <csver>: ���ư汾�ţ��汾������λ��ʾ��ÿλΪ0��9�������ַ���
     *             1000�����Ƶ�һ���汾��
     *             1001�����Ƶڶ����汾��
     *             ���ε������ơ�
     * [ʾ��]:
     *     �� ���ư汾�Ų�ѯ
     *       AT^CSVER?
     *       ^CSVER: 1001
     *       OK
     */
    { AT_CMD_CSVER,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryCsVer, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CSVER", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����QOS
     * [˵��]: ����QOS������Ʒ��֧��ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^QOS=<traffic_class>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^QOS?
     *     [���]: <CR><LF>^QOS: <traffic_class><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <traffic_class>: ����ֵ��QOS���ͣ�ȡֵ��Χ(0-4)��
     *     <err>: �����롣
     *             0�����ݱ���δ�����
     *             1����������
     * [ʾ��]:
     *     �� QOS���óɹ�
     *       AT^QOS=1
     *       OK
     */
    { AT_CMD_QOS,
      At_SetQosPara, AT_NOT_SET_TIME, At_QryQosPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^QOS", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯ����SIM/USIM/UIM���Ӵ�״̬
     * [˵��]: ��ѯ����SIM/USIM/UIM���Ӵ����Ƿ����á�����Ʒ�������ÿ�ֱ�ӷ���û�����ÿ���
     * [�﷨]:
     *     [����]: ^RSIM?
     *     [���]: <CR><LF>^RSIM: <state><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <state>: �Ӵ�״̬��
     *             0��û���κο��Ӵ��ϣ�
     *             1���Ѿ�����SIM/USIM/UIM����
     *             2��SIM/USIM/UIM����æ��Ҫ�ȴ���
     * [ʾ��]:
     *     �� ��ѯ����SIM���Ӵ�״̬
     *       AT^RSIM?
     *       ^RSIM: 0
     *       OK
     */
    { AT_CMD_RSIM,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryRsimPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RSIM", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����WiFi WEP
     * [˵��]: ���á���ȡ����ѯWiFi WEP��WIFI��KEY�����ݡ�������
     *         ��ȡʱ�����з����������ݡ�
     *         �������ѯ�����þ���DATALOCK������������δ��������ʱ����ѯWiFi WEP���ݷ��ؿգ�WEP�Ѷ�������Ϊ0����
     * [�﷨]:
     *     [����]: ^WIWEP=<index>,<content>,<group>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^WIWEP?
     *     [���]: <CR><LF>^WIWEP: <number><CR><LF>
     *             <CR><LF>^WIWEP: <index>,<content> ,<group><CR><LF>
     *             [��.]
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^WIWEP=?
     *     [���]: <CR><LF>^WIWEP: <total><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <index>: WiFi KEY��indexֵ��ȡֵ��Χ0~3��
     *     <content>: <index>��Ӧ��WiFi KEY���ݡ�
     *     <total>: ��λ֧�ֵ�WIFI KEY������BalongΪ16����
     *     <number>: ��λ֧�ֵ�WIFI KEY������
     *     <group>: ͬ^SSID <group>������Balong֧��0~3��
     * [ʾ��]:
     *     �� ���õ�һ��ĵ�һ��WIFI��KEYΪ12345
     *       AT^WIWEP=0,12345,0
     *       OK
     *     �� ��ѯ����洢��WIFI��KEY
     *       AT^WIWEP?
     *       ^WIWEP: 1
     *       ^WIWEP: 0,12345,0
     *       OK
     *     �� ��������
     *       AT^WIWEP=?
     *       ^WIWEP: 16
     *       OK
     */
    { AT_CMD_WIWEP,
      AT_SetWiwepPara, AT_SET_PARA_TIME, AT_QryWiwepPara, AT_QRY_PARA_TIME, AT_TestWiwepPara, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^WIWEP", (VOS_UINT8 *)"(0-3),(@wifikey),(0-3)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����/��ѯAT�����
     * [˵��]: PC������ô������ѯ����������ֱ�ӽ���AT�����ַ��������Լ�����ͨ��ATһ����������Ӧ���ַ����������ַ�������������AT�������ַ����������������ķ��ͺͽ��վ�����ڵ�����Եġ�
     * [�﷨]:
     *     [����]: ^CMDLEN=<max_rx_len>,<max_tx_len>
     *     [���]: <CR><LF>^CMDLEN: <max_rx_len>,<max_tx_len><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CMDLEN?
     *     [���]: <CR><LF>^CMDLEN: <max_rx_len>,<max_tx_len><CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CMDLEN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <max_rx_len>: ��ѯ�����У��ò���ָ����һ�������Խ��յ��ַ�������
     *             ���������У��ò���ָPCһ���������͵��ַ�������
     *             �������������Ӧ�У��ò����ǵ������PC����ֵ������֧�������еĽ�С����Ϊ���յ�Э�̽���������յ������ļ������У�PCһ�θ����巢���ַ����������ڸ�ֵ��
     *     <max_tx_len>: ��ѯ�����У��ò���ָ����һ�������Է��͸�PC���ַ�������
     *             ���������У��ò���ָPC��������һ�η��͵��ַ�������
     *             �������������Ӧ�У��ò����ǵ������PC����ֵ������֧�������еĽ�С����Ϊ���յ�Э�̽���������յ������ļ������У�����һ�θ�PC���͵��ַ����������ڸ�ֵ��
     * [ʾ��]:
     *     �� ����PC���������͵�AT�����Ϊ1000�ַ����������յ�AT������Ӧ�ַ�������Ϊ2000
     *       AT^CMDLEN=1000,2000
     *       OK
     *     �� ��ѯ������������յ�AT����Ⱥ��������͵�AT������Ӧ�ַ�������
     *       AT^CMDLEN?
     *       ^CMDLEN: 1598,5000
     *       OK
     *     �� ��������
     *       AT^CMDLEN=?
     *       OK
     */
    { AT_CMD_CMDLEN,
      AT_SetCmdlenPara, AT_SET_PARA_TIME, AT_QryCmdlenPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^CMDLEN", (VOS_UINT8 *)"(0-65535),(0-65535)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯUT��λNV�ָ�״̬
     * [˵��]: ������ֻ���ڲ���CS��λ��֤UT��λ�Ƿ������ɹ���
     * [�﷨]:
     *     [����]: ^QRYNVRESUME?
     *     [���]: <CR><LF>^QRYNVRESUME: <status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^QRYNVRESUME=?
     *     [���]: <CR><LF>^QRYNVRESUME: (list of supported<status>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <status>: ��ǰ״̬��
     *             0��NV�ָ��ɹ���
     *             1��NV�ָ�ʧ�ܡ�
     * [ʾ��]:
     *     �� ��ѯUT��λNV�ָ��ɹ�
     *       AT^QRYNVRESUME?
     *       ^QRYNVRESUME: 0
     *       OK
     *     �� ��������
     *       AT^QRYNVRESUME=?
     *       ^QRYNVRESUME: (0,1)
     *       OK
     */
    { AT_CMD_QRYNVRESUME,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryNvResumePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^QRYNVRESUME", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: дSimLock������������
     * [˵��]: ����������AP-Modem��̬��дSimLock�����������ݣ�һ�ν��������һ�����������������ݵ����룬������������������Ͷ�Ҫ������Ҫ�����������ò�������������Ҫ���߼�Ȩͨ������ִ�У����õ�����������Ϣ�������������ʹ�á�
     *         ����ʹ�ó��������߹����·���
     *         ����ʹ�����ƣ���Ҫͨ�����߼�Ȩ��ֻ�������AP�Խӵ�ר��ATͨ���·���
     * [�﷨]:
     *     [����]: ^SIMLOCKDATAWRITE=<simlock_data_write>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^SIMLOCKDATAWRITE=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <simlock_data_write>: ��̨����д��SimLock��������ΪSimLock���ݽṹ�Ķ����������������������͡�����״̬��PIN ���������������Ŷ������������Ŷ����鼰��У�����ģ�CK��UKʹ��˽Կ����RSA���ܺ�����ġ�
     *             ����ʱ��ͨ��Half-Byte�����1096�ֽڵ��ַ�����
     * [ʾ��]:
     *     �� дsimlock�����������ݣ�
     *       AT^SIMLOCKDATAWRITE=0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345670123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456701234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234
     *       OK
     *     �� ��������
     *       AT^SIMLOCKDATAWRITE=?
     *       OK
     */
    { AT_CMD_SIMLOCKDATAWRITE,
      VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestHsicCmdPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SIMLOCKDATAWRITE", (VOS_UINT8 *)"(@SimlockData)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: д�����ļ�����
     * [˵��]: PC������ô������򵥰�д�����ļ�������֧�������ļ�������XML�ļ����ı��ļ���Binary�ļ�����
     * [�﷨]:
     *     [����]: ^RSFW=<itemname>,<sub_itemname>,
     *             <ref>,<total>,<index>,<item>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^RSFW=?
     *     [���]: <CR><LF>^RSFW: (list of supported <itemname>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <itemname>: �ַ����������ļ���ʶ
     *             ����ͳ���������ַ�����ȡֵ�����ƶ���� ҵ��ƽ̨����ͳ�� ��������.doc�����硰RF60����
     *     <sub_itemname>: �ַ����������ļ��ӱ�ʶ
     *             �˲������ݲ��ڴ��Ķ��壬��TE��PCӦ�ó���������չ���������ֻ��ȡ�ļ��Ĳ���ָ�����ݡ��������Ҫ�˲�������Ϊ���ַ���������
     *     <ref>: �ο���
     *             ���Ƴ����ŵġ�Concatenated short message reference number�������[1]��9.2.3.24.1�ڡ�ȡֵ��Χ0~255�����������Ŀ�ģ���������У����ƣ�����ƴ��ʱ���ҡ�
     *             ÿ���ϱ�item������һ���µĲο��ţ����һ��item����֣�����һ���ο��š�
     *             SIMLOCK�ο���Ϊ 123
     *     <total>: ��ְ�����
     *             ���Ƴ����ŵġ�Maximum number of short messages in the concatenated short message�������[1]��9.2.3.24.1�ڡ�ȡֵ��Χ0~255��
     *             ���û�в�֣���Ϊ1��
     *     <index>: ��ְ���ǰ����
     *             ���Ƴ����ŵġ�Sequence number of the current short message�������[1]��9.2.3.24.1�ڡ�ȡֵ��Χ0~255����1��ʼ������
     *             ���û�в�֣���Ϊ1��
     *     <item>: ʵ���ϱ������ļ�������Base64�������ַ���
     */
    { AT_CMD_RSFW,
      AT_SetRsfwPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestRsfwPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^RSFW", (VOS_UINT8 *)"(name),(subname),(0-255),(0-255),(0-255),(item)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ��ѯICCID
     * [˵��]: ��ѯ(U)SIM����ICCID��Icc Identification��ֵ��
     * [�﷨]:
     *     [����]: ^ICCID
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^ICCID?
     *     [���]: <CR><LF>^ICCID: <iccid><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^ICCID=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� ��ѯICCIDֵ
     *       AT^ICCID?
     *       ^ICCID: 98680036904030021872
     *       OK
     *     �� ��������
     *       AT^ICCID=?
     *       OK
     */
    { AT_CMD_CICCID,
      At_SetIccidPara, AT_SET_PARA_TIME, At_QryIccidPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (TAF_UINT8 *)"^ICCID", TAF_NULL_PTR },


#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���÷������DACֵ
     * [˵��]: �ڵ���״̬������ĳһƵ�η������DACֵ������Ʒ��֧�ֿ�ֱ�ӷ���ERROR����
     *         �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش�����0��
     *         ��������Ҫ�����÷������ŵ���^FCHAN����ִ�С�
     * [�﷨]:
     *     [����]: ^FDAC=<tx_control_num>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FDAC?
     *     [���]: <CR><LF>^FDAC: <tx_control_num><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^FDAC=?
     *     [���]: <CR><LF>^FDAC: (list of supported <tx_control_num>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <tx_control_num>: DAC��ֵ��
     *     <err>: �����롣
     *             0��δ���õ�ǰ�ŵ���
     *             1���޷���ȡ��ǰ���õ�DACֵ��
     *             2������DAC����ʧ�ܣ�
     *             3����ȡDAC����ʧ�ܡ�
     */
    { AT_CMD_FDAC,
      AT_SetFDac, AT_SET_PARA_TIME, AT_QryFDac, AT_NOT_SET_TIME, At_TestFdacPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FDAC", (VOS_UINT8 *)"(0-65536)" },
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���÷����PA�ȼ�
     * [˵��]: �ڵ���״̬������ĳһƵ�η������PA�ȼ�������Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     *         �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش�����0��
     *         ��������Ҫ�����÷������ŵ���^FCHAN����ִ�С�
     *         ���������������Ĭ��Ϊ0��
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^FPA=<level>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^FPA?
     *     [���]: <CR><LF>^FPA: <level><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^FPA=?
     *     [���]: <CR><LF>^FPA: (list of supported < level >s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <level>: PA�ȼ�������Ϊһ���ֽڡ�
     *     <err>: �����롣
     *             0������ģʽ����
     *             1��û����������ŵ���
     *             2������PA��ʧ�ܣ�
     *             3����������
     * [ʾ��]:
     *     �� ����PA�ȼ��ɹ�
     *       AT^FPA=1
     *       OK
     *     �� ִ�в�������
     *       AT^PFA=?
     *       ^PFA: (0-3)
     *       OK
     */
    { AT_CMD_FPA,
      At_SetFpaPara, AT_SET_PARA_TIME, At_QryFpaPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FPA", (VOS_UINT8 *)FPA_CMD_PARA_STRING },

#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    /* ���ý��ջ��ȼ������������^FAGCGAIN����^FLNA */
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ý��ջ�LNA�ȼ�
     * [˵��]: �ڵ���״̬������ĳһƵ�ν��ջ���LNA�ȼ�������Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     *         �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش�����0����������Ҫ�����÷������ŵ���^FCHAN����ִ�С�
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^FLNA=<level>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FLNA?
     *     [���]: <CR><LF>^FLNA: <level><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <level>: ����LNA�ĵȼ�������Ϊһ���ֽڡ�
     *     <err>: �����롣
     *             0������ģʽ����
     *             1��û����������ŵ���
     *             2������LNA��ʧ�ܣ�
     *             3����������
     * [ʾ��]:
     *     �� ����LNA�ȼ��ɹ�
     *       AT^FLNA=1
     *       OK
     */
    { AT_CMD_FLNA,
      AT_SetFAgcgainPara, AT_SET_PARA_TIME, AT_QryFAgcgainPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_FLNA_OTHER_ERR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^FLNA", (VOS_UINT8 *)"(0-255)" },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���ý��ջ�AGCGAIN�ȼ�
     * [˵��]: �ڵ���״̬������ĳһƵ�ν��ջ���AGC�ȼ�������Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     *         �������ڷ�����ģʽ��AT^TMODE=1����ʹ�ã�����ģʽ�·��ش�����0����������Ҫ�����÷������ŵ���^FCHAN����ִ�С�
     *         ���������������Ĭ��Ϊ0��
     * [�﷨]:
     *     [����]: ^FAGCGAIN=<level>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FAGCGAIN?
     *     [���]: <CR><LF>^FAGCGAIN: <level><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <level>: ����FAGCGain�ĵȼ�������Ϊһ���ֽڣ�ȡֵ��Χ0��255��0��ʾ������漶��
     *     <err>: �����롣
     *             0������ģʽ����
     *             1��û�м���DSP��^FCHAN����
     *             50����������
     * [ʾ��]:
     *     �� ����AGCGAIN�ȼ��ɹ�
     *       AT^FAGCGAIN=1
     *       OK
     */
    { AT_CMD_FAGCGAIN,
      AT_SetFAgcgainPara, AT_SET_PARA_TIME, AT_QryFAgcgainPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^FAGCGAIN", (VOS_UINT8 *)"(0-255)" },

#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����Web UI��½����
     * [˵��]: ���á�У�顢���Web UI��½���롣
     * [�﷨]:
     *     [����]: ^WUPWD=<opr>,<password>
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <opr>: �������ͣ�����Ϊһ���ֽڡ�
     *             0������web UI�ĵ�½���룻
     *             1��У��web UI�ĵ�½���롣
     *     <password>: ��Ӧ��password�����ݡ�
     */
    { AT_CMD_WUPWD,
      AT_SetWebPwdPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^WUPWD", (VOS_UINT8 *)"(0,1),(@WUPWD)" },


#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
#if (FEATURE_LTE == FEATURE_ON)
    { AT_CMD_SFM,
      At_SetSfm, AT_SET_PARA_TIME, At_QrySfm, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SFM", (VOS_UINT8 *)"(0,1)" },
#endif
#endif
};

/* ע��TAFװ��AT����� */
VOS_UINT32 AT_RegisterDeviceTafCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDeviceTafCmdTbl, sizeof(g_atDeviceTafCmdTbl) / sizeof(g_atDeviceTafCmdTbl[0]));
}

