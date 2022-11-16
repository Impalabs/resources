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
#include "at_custom_drv_cmd_tbl.h"
#include "AtParse.h"

#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_custom_drv_set_cmd_proc.h"
#include "at_custom_drv_qry_cmd_proc.h"



#define THIS_FILE_ID PS_FILE_ID_AT_CUSTOM_DRV_CMD_TBL_C


static const AT_ParCmdElement g_atCustomDrvCmdTbl[] = {
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����/�ر�USB PHY
     * [˵��]: ����/�ر�USB PHY��
     *         ����ʹ��ͨ����ֻ��ͨ����AP�Խӵ�ATͨ����MUXͨ���·���
     * [�﷨]:
     *     [����]: +USBSWITCH=<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: +USBSWITCH?
     *     [���]: <CR><LF>+USBSWITCH: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: +USBSWITCH=?
     *     [���]: <CR><LF>+USBSWITCH: (list of supported <mode>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ����ʾ������ر�USB PHY��ȡֵ1��2��
     *             1��USB PHY�л���Modem�ࣻ
     *             2��USB PHY�л���AP�ࡣ
     * [ʾ��]:
     *     �� AP��Ҫ��Modem������USB PHY
     *       AT+USBSWITCH=1
     *       OK
     */
    { AT_CMD_USBSWITCH,
      At_SetUsbSwitchPara, AT_NOT_SET_TIME, At_QryUsbSwitchPara, AT_NOT_SET_TIME, AT_TestUsbSwitchPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"+USBSWITCH", (VOS_UINT8 *)"(1,2)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ��������״̬
     * [˵��]: ��AT����������AP��ѯƽ̨��ǰ��������״̬�﷨��
     * [�﷨]:
     *     [����]: ^ANTSTATE
     *     [���]: <CR><LF>^ANTSTATE: <Value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <Value>: ����ֵ����������״̬��
     *             0������ģʽ��
     *             1������ģʽ��
     * [ʾ��]:
     *     �� AP��ѯ��ǰ����ģʽ
     *       AT^ANTSTATE
     *       ^ANTSTATE: 1
     *       OK
     */
    { AT_CMD_ANTSTATE,
      AT_QryAntState, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ANTSTATE", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����/��ѯOM�˿ڵ�Ĭ������豸�ӿ�
     * [˵��]: ����/��ѯOMĬ������豸�ӿڣ�Ĭ��������Ч��
     * [�﷨]:
     *     [����]: ^LOGPORT=<port_type>[,<config>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^LOGPORT?
     *     [���]: <CR><LF>^LOGPORT: <port_type><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF> ERROR<CR><LF>
     *     [����]: ^LOGPORT=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <port_type>: �����ͣ�OM�˿����ͣ�
     *             0��USB
     *             1��VCOM
     *             2��WiFi
     *             3��SD��
     *             4�������ļ�ϵͳ
     *             5��HSIC
     *             ������������
     *             Ŀǰ��֧��USB��VCOM�������ݲ�֧�֡�
     *             �����������ó�USB��VCOM�ⷵ�ش������ú�������Ч��
     *     <config>: �����ͣ�<port_type>���ú��Ƿ�������Ч��
     *             0����ʱ��Ч��������ָ���
     *             1��������Ч��
     * [ʾ��]:
     *     �� ���ÿ�ά�ɲ��USB�����������Ч
     *       AT^LOGPORT=0,1
     *       OK
     *     �� ��ѯ��ǰOMͨ������
     *       AT^LOGPORT?
     *       ^LOGPORT: 0
     *       OK
     *     �� ��������
     *       AT^LOGPORT=?
     *       OK
     */
    { AT_CMD_LOGPORT,
      At_SetLogPortPara, AT_NOT_SET_TIME, At_QryLogPortPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
#if (FEATURE_MBB_CUST == FEATURE_ON)
      (VOS_UINT8 *)"^LOGPORT", (VOS_UINT8 *)"(0-2),(0,1)" },
#else
      (VOS_UINT8 *)"^LOGPORT", (VOS_UINT8 *)"(0,1),(0,1)" },
#endif

#if (FEATURE_MBB_CUST == FEATURE_ON)
    { AT_CMD_GETDONGLEINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryMbbDongleInfo, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (const VOS_UINT8 *)"^GETDONGLEINFO", VOS_NULL_PTR },

    { AT_CMD_CHECKAUTHORITY,
      At_CheckMbbAuthority, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (const VOS_UINT8 *)"^CHECKAUTHORITY", (const VOS_UINT8 *)"(@Rsa)" },

    { AT_CMD_CONFORMAUTHORITY,
      At_SetMbbConfirmAuthority, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (const VOS_UINT8 *)"^CONFORMAUTHORITY", VOS_NULL_PTR },

    { AT_CMD_NETDOGINTEGRITY,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryMbbNetDogIntegrity, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (const VOS_UINT8 *)"^NETDOGINT", VOS_NULL_PTR },

    { AT_CMD_WAKEUPCFG,
      AT_MbbSetWakeUpCfgPara, AT_SET_PARA_TIME, AT_MbbQryWakeupcfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (const VOS_UINT8 *)"^WAKEUPCFG", (VOS_UINT8 *)"(0-1),(0-3),(0-63)" },
#endif
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ�ն�оƬDIE���к�
     * [˵��]: ��ѯ�ն�оƬDIE���к��룬������װ����Ϣϵͳ�У���Ϊ�ն�������Ϣ��DIE ���к�ʵ�ʳ���156 BIT��
     * [�﷨]:
     *     [����]: ^DIESN
     *     [���]: <SN>
     *             +CME ERROR: <err>
     *     [����]: ^DIESN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <SN>: оƬDIE���кţ�16���ƣ�20�ֽڳ���
     *             ��ע����Ϊʵ��DIE ���к���156 BIT���ڸ�λ����ֽڸ�4 BIT���0x0��
     *             ����0x0f000000��000078�� ��ɫ��ʾ��0����Ч
     * [ʾ��]:
     *     �� ��ѯDIE ���к�
     *       AT^DIESN
     *       ^DIESN: 0x0f0d0000040000f5000000000000000000000008
     *       OK
     */
    { AT_CMD_DIESN,
      At_SetDieSNPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DIESN", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����LOG�ӳ�д�볬ʱʱ��
     * [˵��]: ���������AP+Modem��Ʒ��̬��LOG�ӳ�д�빦�ܿ���ʱ���ó�ʱʱ��������塣���������������������������Ч������Ҫ�������塣
     * [�﷨]:
     *     [����]: ^LOGCFG[=[<TIMEOUT_TYPE>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^LOGCFG=?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^LOGCFG?
     *     [���]: <CR><LF> LOGCFG: <TIMEOUT_TYPE><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <TIMEOUT_TYPE>: �ӳ�д�볬ʱʱ�����͡�
     *             0��ʹ��SOCP��Ĭ�ϳ�ʱʱ�䣬10ms��
     *             1��ʹ��DTS�����õ��ӳ�д�볬ʱʱ�䡣
     * [ʾ��]:
     *     �� ʹ��SOCP��Ĭ�ϳ�ʱʱ��
     *       AT^LOGCFG=0
     *       OK
     */
    { AT_CMD_LOGCFG,
      AT_SetLogCfgPara, AT_SET_PARA_TIME, At_QryLogCfgPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LOGCFG", (VOS_UINT8 *)"(0,1,2)" },

    { AT_CMD_LOGCPS,
      AT_SetLogCpsPara, AT_SET_PARA_TIME, At_QryLogCpsPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^LOGCPS", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����/�ָ���Ӫ�̶���NV����
     * [˵��]: ����/�ָ���Ӫ�̶���NV���� ACTIVENCFG��������ֻ�����ֻ�ƽ̨������ƽ̨��֧�֡�
     * [�﷨]:
     *     [����]: ^ACTIVENCFG=<enable>[,<carrierName>,<effectiveWay>]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ����ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^ACTIVENCFG?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^ACTIVENCFG: < carrierName >,< effectiveWay><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ����ʱ��
     *             <CR><LF>ERROR ><CR><LF>
     *             ��<CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <enable>: ����ֵ������/�ָ���Ӫ�̶���NV����
     *             0��Modem����ػָ�comm.mbn����ɺ�ˢ��NV�ļ�ϵͳ��
     *             1��Modem�����carrier.bin����ɺ�ˢ��NV�ļ�ϵͳ��
     *             2��Modem���ȼ���comm.mbn����ɺ󣬼���carrier.bin�����ˢ��NV�ļ�ϵͳ��
     *             3��ֻ���д�濨NV����ִ��FLUSH������
     *     <carrierName>: �ַ������ͣ��濨��Ӫ�����ƣ���󳤶�Ϊ48�ֽڡ�
     *             ˵��:�ַ�ʹ��ASCII�洢��
     *     <effectiveWay>: ����ֵ����Ӫ�̶���NV��Ч��ʽ��ȡֵ��Χ0~2��Ĭ��Ϊ0��
     *             0����������Modem�����µ磻
     *             1��NV��Ч��ҪModem������
     *             2��NV��Ч��Ҫ�����µ硣
     *             �����浽NV_ITEM_USIM_DEPENDENT_RESULT_INFO��effectiveWay�С�
     * [ʾ��]:
     *     �� Modem����ػָ�comm.mbn
     *       AT^ACTIVENCFG=0
     *       OK
     *     �� Modem�����carrier.bin
     *       AT^ACTIVENCFG=1
     *       OK
     *     �� Modem���ȼ���comm.mbn���ټ���carrier.bin
     *       AT^ACTIVENCFG=2
     *       OK
     *     �� Modem�����carrier.bin���������濨��Ӫ�����ƺ�NV��Ч��ʽĬ��Ϊ0
     *       AT^ACTIVENCFG=1,"cmcc.com"
     *       OK
     *     �� Modem�����carrier.bin���������濨��Ӫ�����ƺ�NV��Ч��ʽ1
     *       AT^ACTIVENCFG=1,"cmcc.com",1
     *       OK
     *     �� ��ѯ�����濨��Ӫ�����ƺ�NV��Ч��ʽ
     *       AT^ACTIVENCFG?
     *       ^ACTIVENCFG: "cmcc.com", 1
     *       OK
     */
    { AT_CMD_NVLOAD,
      AT_SetNvLoadPara, AT_SET_PARA_TIME, AT_QryNvLoadPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (TAF_UINT8 *)"^ACTIVENCFG", (TAF_UINT8 *)"(0,1,2),(str),(0,1,2),(0,1)" },

    { AT_CMD_FLASHINFO,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryFlashInfoPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^FLASHINFO", VOS_NULL_PTR },

    { AT_CMD_NVCHK,
      AT_SetNVCHKPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^NVCHK", (TAF_UINT8 *)"(0-3)" }, /* mode */

    { AT_CMD_APPDMVER,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryAppdmverPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^APPDMVER", VOS_NULL_PTR },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    { AT_CMD_MAXFREELOCKSIZE,
      TAF_NULL_PTR, AT_NOT_SET_TIME, At_QryMaxFreelockSizePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
      (TAF_UINT8 *)"^MFREELOCKSIZE", TAF_NULL_PTR },

    { AT_CMD_SDREBOOT,
      AT_SetSdrebootCmd, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^SDREBOOT", (VOS_UINT8 *)"(pwd)" },

    { AT_CMD_YJCX,
      At_SetYjcxPara, AT_SET_PARA_TIME, At_QryYjcxPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^YJCX", (VOS_UINT8 *)"(0,1)" },


#endif
};

/* ע��DRV����AT����� */
VOS_UINT32 AT_RegisterCustomDrvCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atCustomDrvCmdTbl, sizeof(g_atCustomDrvCmdTbl) / sizeof(g_atCustomDrvCmdTbl[0]));
}

