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

#include "at_general_taf_cmd_tbl.h"
#include "at_general_taf_set_cmd_proc.h"
#include "at_general_taf_qry_cmd_proc.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_test_para_cmd.h"



/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_GENERAL_TAF_CMD_TBL_C

static const AT_ParCmdElement g_atGeneralTafCmdTbl[] = {
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ʹ��/��ֹ�����ϱ�
     * [˵��]: ��������AP-Modem��̬�����ڿ���AT����������ϱ���ÿһ�������ϱ������Ӧһ��������Bitλ���ɰ��������������ÿһ���ɿ������ϱ�����ɿ�����Ĭ��ȫ�������ϱ������������ϱ��������Լ����������������Ҫͬʱ���á�
     * [�﷨]:
     *     [����]: ^CURC=<mode>[,<report_cfg>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CURC?
     *     [���]: <CR><LF>^CURC: <mode>[,<report_flag>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CURC=?
     *     [���]: <CR><LF>^CURC: (list of supported <mode>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ�����������ϱ�����ģʽ��ȡֵ0~2��һ��Ĭ��Ϊ1����ͬ��Ʒ�޸ĺ�Ĭ��ֵ�п��ܲ�ͬ��
     *             0���رձ�2-4������������ϱ���
     *             1���򿪱�2-4������������ϱ���Ĭ��ֵ����
     *             2����ģʽ��Ҫ����<report_cfg>�Ա�2-4������������ϱ��������á�
     *     <report_cfg>: 16�������ַ��������������ϱ���ʶ������Ϊ8���ֽڣ���ÿ��Bitλ��Ӧһ�������ϱ�AT�����Ӧ��ϵ����2-4��
     *             ȡֵ��ΧΪ0x0000000000000000~0xFFFFFFFFFFFFFFFF��ÿ��Bitλ��ȡֵ���£�
     *             0���ر������ϱ���
     *             1���������ϱ���
     * [��]: �������������ϱ�Bitλ[63-0]�������Ӧ��ϵ
     *       bit:   3,          2,          1,                                        0,             Oct0,
     *              ^SRVST,     ^REJINFO,   ^RSSI,                                    ^MODE,         Oct0,
     *       bit:   7,          6,          5,                                        4,             Oct0,
     *              ,           ^TIME,      ^SIMST,                                   ^PLMNSELEINFO, Oct0,
     *       bit:   11,         10,         9,                                        8,             Oct1,
     *              ^SMMEMFULL, ,           ,                                         ^ANLEVEL,      Oct1,
     *       bit:   15,         14,         13,                                       12,            Oct1,
     *              ,           ,           ,                                         ^ELEVATOR,     Oct1,
     *       bit:   19,         18,         17,                                       16,            Oct2,
     *              +CTZV,      ,           ,                                         ,              Oct2,
     *       bit:   23,         22,         21,                                       20,            Oct2,
     *              ,           ^DSFLOWRPT, ,                                         ,              Oct2,
     *       bit:   27,         26,         25,                                       24,            Oct3,
     *              ^CEND,      ^CONN,      ^CONF,                                    ^ORIG,         Oct3,
     *       bit:   31,         30,         29,                                       28,            Oct3,
     *              ,           ,           ^STIN��^CCIN��^CSIN��^CSMN��^CSTR��^SCEN, ,              Oct3,
     *       bit:   35,         34,         33,                                       32,            Oct4,
     *              ,           ,           ,                                         ,              Oct4,
     *       bit:   39,         38,         37,                                       36,            Oct4,
     *              ,           ,           ,                                         ,              Oct4,
     *       bit:   43,         42,         41,                                       40,            Oct5,
     *              ,           ,           ,                                         ,              Oct5,
     *       bit:   47,         46,         45,                                       44,            Oct5,
     *              ,           ,           ,                                         ,              Oct5,
     *       bit:   51,         50,         49,                                       48,            Oct6,
     *              ^ACINFO,    ^XLEMA,     ^LWURC,                                   ^CERSSI,       Oct6,
     *       bit:   55,         54,         53,                                       52,            Oct6,
     *              ,           ,           ^CALLSTATE,                               ^PLMN,         Oct6,
     *       bit:   59,         58,         57,                                       56,            Oct7,
     *              ,           ,           ,                                         ,              Oct7,
     *       bit:   63,         62,         61,                                       60,            Oct7,
     *              ,           ,           ,                                         ,              Oct7,
     * [ʾ��]:
     *     �� �ر�ȫ�������ϱ�����
     *       AT^CURC=0
     *       OK
     *     �� ����^RSSI�������ϱ�
     *       AT^CURC=2,0000000000000002
     *       OK
     *     �� ��ѯ��ǰ�����ϱ�����״̬
     *       AT^CURC?
     *       ^CURC: 1
     *       OK
     *       AT^CURC?
     *       ^CURC: 2,0x0000000000000002
     *       OK
     *     �� ��������
     *       AT^CURC=?
     *       ^CURC: (0-2)
     *       OK
     */
    { AT_CMD_CURC,
      At_SetCurcPara, AT_NOT_SET_TIME, At_QryCurcPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CURC", (VOS_UINT8 *)"(0-2)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �����豸��̬�л�
     * [˵��]: ���������PID�Ż�����δʹ��ʱʹ�ã������ò������ֻ���
     *         SET��������ʵ���豸��̬�л����ܡ��ֶ�����������������òŻ���Ч��������Ч�󵥰彫һֱ���ָ��豸��̬��ֱ���ٴ�ʹ�ñ�����豸��̬�����л���
     *         READ�������ڲ�ѯ��ǰ���豸��ֵ̬��
     * [�﷨]:
     *     [����]: ^U2DIAG=<val>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^U2DIAG?
     *     [���]: <CR><LF>^U2DIAG: <val><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^U2DIAG=?
     *     [���]: <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <val>: ����ֵ����ʾPC�����modem��diag��PCUI��U�̣�CDROM����NDIS��PCSC��NEMA��SD��DVB���豸��̬��������ȡֵ��
     *             ��2-5�������˲��ֿ��ܵ��豸��̬ȡֵ������ض���Ʒ��֧�ֵ��豸��̬������Ĳ�Ʒ������ĵ���
     *             ˵����Ŀǰֻ��HILINKʹ�ø������valֻ��Ϊ118����119������ֵ��119����
     * [��]: <val>ȡֵ����
     *       <val>, �豸��̬����,
     *       0,     Modem+DIAG+PCUI,
     *       1,     CDROM/Modem+PCUI+CDROM,
     *       2,     Modem+PCUI,
     *       5,     CDROM/DIAG+CDROM+PCUI+NDIS,
     *       6,     PCUI+NDIS,
     *       7,     PCUI+NDIS+DIAG,
     *       11,    CDROM/PCUI+CDROM+NDIS,
     *       12,    CDROM/Modem+DIAG+PCUI+CDROM,
     *       19,    CDROM/Modem+DIAG+PCUI+CDROM,
     *       20,    CDROM/Modem+NDIS+DIAG+PCUI+CDROM,
     *       25,    CDROM/Modem+PCUI,
     *       32,    CDROM/Modem+NDIS+PCUI+CDROM,
     *       36,    Modem+NDIS+DIAG+PCUI,
     *       41,    CDROM/Modem+NDIS+DIAG+PCUI,
     *       118,   SHELL+NDIS(����ģʽ),
     *       119,   NDIS(�û�ģʽ),
     *       257,   CDROM/Modem+PCUI+CDROM+SD,
     *       268,   CDROM/Modem+DIAG+PCUI+CDROM+SD,
     *       267,   CDROM/PCUI+CDROM+NDIS+SD,
     *       256,   Modem+DIAG+PCUI+SD,
     *       258,   Modem+PCUI+SD,
     *       262,   CDROM/PCUI+NDIS+SD,
     *       263,   CDROM/DIAG+PCUI+NDIS+SD,
     *       281,   CDROM/Modem+PCUI+SD,
     *       261,   CDROM/DIAG+CDROM+PCUI+NDIS+SD,
     *       276,   CDROM/Modem+NDIS+DIAG+PCUI+CDROM+SD,
     *       288,   CDROM/Modem+NDIS+PCUI+CDROM+SD,
     *       2316,  CDROM/Modem+OM+AT+CDROM+SD+ISHELL,
     *       4372,  CDROM/Modem+NDIS+DIAG+PCUI+CDROM+SD+BT,
     * [ʾ��]:
     *     �� �����豸��̬
     *       At^U2DIAG=118
     *       OK
     *     �� ��ѯ��ǰ�豸��̬
     *       At^U2DIAG?
     *       ^U2DIAG: 118
     *       OK
     */
    { AT_CMD_U2DIAG,
      At_SetU2DiagPara, AT_SET_PARA_TIME, At_QryU2DiagPara, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE_STICK | CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^U2DIAG", (VOS_UINT8 *)"(0-4294967295)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �豸��̬�л�
     * [˵��]: ���������PID�Ż����أ�NV:2601��ʹ��ʱʹ�á�
     *         ����E5��̬���л�����豸��̬�в��ܴ���MODEM�豸��̬(1)���л���ĵ�һ���豸����ΪMASS�豸(A1��A2)��
     *         BalongV300R300��USB FIFO�����Ż������ֻ��֧��8��interface����8��USB�豸������FIFO��С�޷�֧���л������豸�������ᵼ��USBö��ʧ�ܡ�
     *         ����������ʵ���豸��̬�л����ܣ����ú�����������Ч��
     *         READ�������ڲ�ѯ��ǰ��Ч���豸��ֵ̬��
     *         �����������ڲ�ѯ�˿���̬������ַ��������ַ������Ķ�Ӧ��ϵ��
     *         �����������ַ�������ʾһ��˿���̬�������л�ǰ���л���Ķ˿���̬������֮���Էֺ����֣��˿���̬������ַ��������ַ�������Ӧ��ϵ���£�
     *         1��3G MODEM
     *         2��3G PCUI
     *         3��3G DIAG
     *         5��3G GPS
     *         10��4G MODEM
     *         12��4G PCUI
     *         13��4G DIAG
     *         14��4G GPS
     *         16��4G MODEM
     *         A��BLUE TOOTH
     *         A1��CDROM
     *         A2��SD
     *         A3��RNDIS
     *         �Զ˿����õ�Լ�����£�
     *         1�����ö˿ڵ��ַ����ַ����������оٵĻ�FF�����ж�Ϊ�Ƿ�������Error��
     *         2���л�ǰ�Ķ˿����õ�һλ����ΪA1(CDROM)���ڶ�λ����У���������A2(SD)�������˿ڳ������л�ǰ�ķֺ�ǰʱ��һ���ж�Ϊ�Ƿ�������Error��
     *         3������Ҫ����Ϊ���л�ǰ�Ķ˿���̬�����л�ǰ�˿���̬��дΪFF��
     *         4���л���Ķ˿���̬�б�����12(PCUI)�������ж�Ϊ�Ƿ��˿���̬������Error��
     *         5����������������������ͬ�Ķ˿ڣ�������PCUI��������Ϊ�Ƿ�������Error��
     * [�﷨]:
     *     [����]: ^SETPORT=<PORT_Type>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^SETPORT?
     *     [���]: <CR><LF>^SETPORT: <PORT_Type><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^SETPORT=?
     *     [���]: <CR><LF>^SETPORT: A1: CDROM
     *             <CR><LF>^SETPORT: A2: SD
     *             <CR><LF>^SETPORT: A3: RNIDS
     *             <CR><LF>^SETPORT: A: BLUE TOOTH
     *             <CR><LF>^SETPORT: 1: 3G MODEM
     *             <CR><LF>^SETPORT: 2: 3G PCUI
     *             <CR><LF>^SETPORT: 3: 3G DIAG
     *             <CR><LF>^SETPORT: 5: 3G GPS
     *             <CR><LF>^SETPORT: 10: 4G MODEM
     *             <CR><LF>^SETPORT: 12: 4G PCUI
     *             <CR><LF>^SETPORT: 13: 4G DIAG
     *             <CR><LF>^SETPORT: 14: 4G GPS
     *             <CR><LF>^SETPORT: 16: NCM<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <PORT_Type>: �ַ������ͣ�����Ϊ0��50byte�������л�ǰ���л���Ķ˿���̬��
     *             �л�ǰ���л���Ķ˿���̬�б��÷ֺŷָ������ֶ˿���̬֮���ö��ŷָ���
     * [ʾ��]:
     *     �� ��ǰ�˿���̬ΪCDROM/Modem+PCUI���û���Ҫ�޸Ķ˿���̬ΪCDROM+ SD /Modem+ DIAG +PCUI��
     *       AT^SETPORT="A1,A2;1,3,12"
     *       OK
     *     �� ��Ϊ�û�������Ҫ�豸��������Ч���������̲�ѯ�õ��Ļ����޸�ǰ�Ķ˿���̬��
     *       AT^SETPORT?
     *       ^SETPORT: A1;1,12
     *       OK
     *     �� �����������л�ǰ�˿�ΪCDROM+SD���л���˿�ΪModem+ DIAG+PCUI��
     *       AT^SETPORT?
     *       ^SETPORT: A1,A2;1,3,12
     *       OK
     *     �� ����������л�ǰ�˿���̬���л���˿�ΪModem+PCUI��
     *       AT^SETPORT="FF;1,12"
     *       OK
     *       ��ѯ��ǰ�˿���̬
     *     �� �л�ǰ�˿�ΪCDROM+SD���л���˿�ΪModem+ PCUI+DIAG
     *       AT^SETPORT?
     *       ^SETPORT: A1,A2;1,12,3
     *       OK
     *     �� ��������
     *       AT^SETPORT=?
     *       ^SETPORT: A1: CDROM
     *       ^SETPORT: A2: SD
     *       ^SETPORT: A3: RNIDS
     *       ^SETPORT: A: BLUE TOOTH
     *       ^SETPORT: 1: 3G MODEM
     *       ^SETPORT: 2: 3G PCUI
     *       ^SETPORT: 3: 3G DIAG
     *       ^SETPORT: 5: 3G GPS
     *       ^SETPORT: 10: 4G MODEM
     *       ^SETPORT: 12: 4G PCUI
     *       ^SETPORT: 13: 4G DIAG
     *       ^SETPORT: 14: 4G GPS
     *       ^SETPORT: 16: NCM
     *       OK
     */
    { AT_CMD_PORT,
      At_SetPort, AT_SET_PARA_TIME, At_QryPort, AT_QRY_PARA_TIME, AT_TestSetPort, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL | CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SETPORT", (VOS_UINT8 *)"(Port)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: �Ƿ�֧��PCSC�˿�
     * [˵��]: ���������ڲ�ѯ�����õ�ǰ�豸�Ƿ�֧��PCSC�˿ڡ��޸����ú���Ҫ�������������Ч��
     *         �����֧�ֲ�����Χ��ѯ��
     *         ��ʹ�ø�����ǰ��������Ҫʹ�����AT^U2DIAG=288�����豸��̬�е�288��
     * [�﷨]:
     *     [����]: ^PCSCINFO=<value>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^PCSCINFO?
     *     [���]: <CR><LF>^PCSCINFO: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^PCSCINFO=?
     *     [���]: <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <value>: 0����֧��PCSC�˿ڣ�
     *             1��֧��PCSC�˿ڡ�
     * [ʾ��]:
     *     �� ����֧��PCSC�˿�
     *       AT^PCSCINFO=1
     *       OK
     *     �� ��ѯ
     *       AT^PCSCINFO?
     *       ^PCSCINFO: 1
     *       OK
     */
    { AT_CMD_PCSCINFO,
      At_SetPcscInfo, AT_NOT_SET_TIME, At_QryPcscInfo, AT_NOT_SET_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PCSCINFO", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ���ز�ѯ���ݿ�����
     * [˵��]: �������е�PID�����ݿ������ظ������õ��ļ����в�ѯ���Ի�ȡ���ݿ��Ķ˿�˳����Ϣ��
     *         ���ں���������PID���������л�����˿��Ժ󣬶�ÿ���˿������·���������в�ѯ������з��صĶ˿ڣ����·���������в�ѯ���Եõ����ݿ����ͺͶ˿ڵ�˳��
     * [�﷨]:
     *     [����]: ^GETPORTMODE
     *     [���]: <CR><LF>^GETPORTMODE: <TYPE><оƬ����>,<�˿�><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <TYPE>: ���ݿ����ͣ������ַ���������
     *     <оƬ����>: ���ݿ�оƬ�ṩ�̡�
     *     <�˿�>: ֧�ֵĶ˿ڡ�
     * [ʾ��]:
     *     �� ��ѯ���ݿ�����
     *       AT^GETPORTMODE
     *       ^GETPORTMODE: TYPE:WCDMA:huawei,NDIS:0,CDROM:1,DIAG:2,PCUI:3,GPS:4,PCVoice:5,SHELL:6
     *       Ok
     */
    { AT_CMD_GETPORTMODE,
      At_SetGetportmodePara, AT_NOT_SET_TIME, At_QryGetportmodePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE_STICK,
      (VOS_UINT8 *)"^GETPORTMODE", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ����PCUI��PCUI2��CTRL�ڵĲ�����־λ
     * [˵��]: ��������������PCUI��PCUI2��CTRL�ڵĲ���������
     * [�﷨]:
     *     [����]: ^PORTCONCURRENT=<curflag>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^PORTCONCURRENT=?
     *     [���]: <CR><LF>^PORTCONCURRENT: (list of supported < curflag>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <curflag>: �����·������־λ��ȡֵ��Χ0~1��
     * [ʾ��]:
     *     �� ����PCUI��PCUI2��CTRL�ڲ����·�����
     *       AT^PORTCONCURRENT=1
     *       OK
     *     �� ��������
     *       AT^PORTCONCURRENT=?
     *       ^PORTCONCURRENT: (0-1)
     *       OK
     */
    { AT_CMD_PORTCONCURRENT,
      AT_SetPcuiCtrlConcurrentFlagForTest, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PORTCONCURRENT", (VOS_UINT8 *)"(0-1)" },
};

/* ע��TAFͨ��AT����� */
VOS_UINT32 AT_RegisterGeneralTafCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atGeneralTafCmdTbl, sizeof(g_atGeneralTafCmdTbl) / sizeof(g_atGeneralTafCmdTbl[0]));
}

