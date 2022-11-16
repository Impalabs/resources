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

#include "at_phone_ims_cmd_tbl.h"
#include "AtParse.h"
#include "at_parse_cmd.h"
#include "at_phone_ims_set_cmd_proc.h"
#include "at_phone_ims_qry_cmd_proc.h"
#include "at_device_cmd.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_IMS_CMD_TBL_C

#if (FEATURE_IMS == FEATURE_ON)
static const AT_ParCmdElement g_atPhoneImsCmdTbl[] = {
    /*
     * [���]: Э��AT-�绰����
     * [����]: ����IMS�ĵ�ǰ��ص�����Ϣ
     * [˵��]: ����������AP֪ͨIMS��ǰ�ĵ�ص�����Ϣ��
     * [�﷨]:
     *     [����]: ^BATTERYINFO=<Battery_Info>
     *     [���]: ������ȷ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^BATTERYINFO?
     *     [���]: ��ѯ�ɹ���
     *             <CR><LF>^BATTERYINFO: <Battery_Info><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^BATTERYINFO=?
     *     [���]: <CR><LF>^ BATTERYINFO:  (list of supported <Battery_Info>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Battery_Info>: Ĭ����дΪ0��
     *             0: ����;
     *             1: �͵�;
     *             2: ��غľ�
     * [ʾ��]:
     *     �� ���õ�����ϢΪ�͵磺
     *       AT^BATTERYINFO=1
     *       OK
     *     �� ��ѯ���
     *       AT^BATTERYINFO?
     *       ^BATTERYINFO: 1
     *       OK
     *     �� �������
     *       AT^BATTERYINFO=?
     *       ^BATTERYINFO: (0-2)
     *       OK
     */
    { AT_CMD_BATTERYINFO,
      AT_SetBatteryInfoPara, AT_SET_PARA_TIME, AT_QryBatteryInfoPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^BATTERYINFO", (VOS_UINT8 *)"(0-2)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����������������ָʾ
     * [˵��]: ͨ���������֪ͨIMS UE�Ƿ�֧���������ܡ�
     * [�﷨]:
     *     [����]: ^CALLENCRYPT=<state>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CALLENCRYPT=?
     *     [���]: <CR><LF>^CALLENCRYPT: (list of supported <state>)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <state>: ����ֵ������״̬��
     *             0����֧����������
     *             1��֧����������
     * [ʾ��]:
     *     �� ����֧����������
     *       AT^CALLENCRYPT=1
     *       OK
     *     �� ִ�в�������
     *       AT^CALLENCRYPT=?
     *       ^CALLENCRYPT: (0,1)
     *       OK
     */
    { AT_CMD_ENCRYPT,
      AT_SetCallEncryptPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CALLENCRYPT", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����IMS��PCSCF��ַ
     * [˵��]: ��������������IMS���PCSCF��ַ��
     * [�﷨]:
     *     [����]: ^IMSPCSCF=<Source>,[<Ipv6Address1>],[<Ipv6Address2>],[<Ipv6Address3>],[<Ipv4Address4>],[<Ipv4Address5>],[<Ipv4Address6>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF> +CME ERROR:<err><CR><LF>
     *     [����]: ^IMSPCSCF?
     *     [���]: <CR><LF>^IMSPCSCF: <Source>,[<Ipv6Address1>],[<Ipv6Address2>],[<Ipv6Address3>],[<Ipv4Address4>],[<Ipv4Address5>],[<Ipv4Address6>]<CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^IMSPCSCF=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <Source>: Ŀǰֻ֧��0��1��
     *             0��ʾ����Ĭ�ϵ�P-CSCF����
     *             1��ʾDM������P-CSCF���ã�
     *             ����ֵ���ں�����չ��
     *             ��<Source>����Ϊ0ʱ��<Ipv6Address1>,<Ipv6Address2>,<Ipv6Address3>,<Ipv4Address1>,
     *             <Ipv4Address2>��<Ipv4Address3> ���õĶ���Ĭ�ϵ�P-CSCF��ַ����Щ����û������ѯ�������������ڲ�ѯ����ķ���ֵ�У���Ϊ��ѯ����ֻ��ӳ����DM��P-CSCF��ַ���ã����ʾ��
     *     <Ipv6Address1>: ����P-CSCF�ĵ�һ��IPV6��ַ���ַ������͡�
     *             Я���˿ںŵĸ�ʽ������
     *             ��[1234:5678:90ab:cdef:1234:5678:90ab:cdef]:8080��
     *             ���У��������ڵ�ΪIPV6��ַ��ʮ�����Ƹ�ʽ�����������⡰:�����Ϊ�˿ںţ�ʮ���Ƹ�ʽ���˿ںŲ���Ϊ0���ߴ���65535����
     *             û��Я���˿ںŵĸ�ʽ������
     *             ��1234:5678:90ab:cdef:1234:5678:90ab:cdef��
     *             �����ڵ�ΪIPV6��ַ��ʮ�����Ƹ�ʽ��
     *             �˲���Ϊ��ѡ�����д����ֻ��д����ʱ��ʾ��Ҫ���֮ǰ����ĵ�һ��IPV6��ַ
     *     <Ipv6Address2>: ����P-CSCF�ĵڶ���IPV6��ַ���ַ������͡�
     *             ��ַ��ʽͬ<Ipv6Address1>��
     *             �˲���Ϊ��ѡ�����д����ֻ��д����ʱ��ʾ��Ҫ���֮ǰ����ĵڶ���IPV6��ַ
     *     <Ipv6Address3>: ����P-CSCF�ĵ�����IPV6��ַ���ַ������͡�
     *             ��ַ��ʽͬ<Ipv6Address1>��
     *             �˲���Ϊ��ѡ�����д����ֻ��д����ʱ��ʾ��Ҫ���֮ǰ����ĵ�����IPV6��ַ
     *     <Ipv4Address1>: ����P-CSCF�ĵ�һ��IPV4��ַ���ַ������͡�
     *             Я���˿ںŵĸ�ʽΪ��
     *             ��123.456.789.123:8080��
     *             ���У���:��ǰ��ΪIPV4��ַ����:�����Ϊ�˿ںţ���Ϊʮ���Ƹ�ʽ�����˿ںŲ���Ϊ0���ߴ���65535��
     *             �˲���Ϊ��ѡ�����д����ֻ��д����ʱ��ʾ��Ҫ���֮ǰ����ĵ�һ��IPV4��ַ
     *     <Ipv4Address2>: ����P-CSCF�ĵڶ���IPV4��ַ���ַ������͡�
     *             ��ַ��ʽͬ<Ipv6Address1>��
     *             �˲���Ϊ��ѡ�����д����ֻ��д����ʱ��ʾ��Ҫ���֮ǰ����ĵڶ���IPV4��ַ
     *     <Ipv4Address3>: ����P-CSCF�ĵ�����IPV4��ַ���ַ������͡�
     *             ��ַ��ʽͬ<Ipv6Address1>��
     *             �˲���Ϊ��ѡ�����д����ֻ��д����ʱ��ʾ��Ҫ���֮ǰ����ĵ�����IPV4��ַ
     * [ʾ��]:
     *     �� ���õ�һ��IPV4��ַ��ɾ���������IPV4��ַ������IPV6��ַ
     *       AT^IMSPCSCF?
     *       ^IMSPCSCF: 1,"1234:0:0:0:0:0:0:5678","[1234:0:0:0:0:0:0:5678]:80","1234:0:0:0:0:0:5678:90ab","1.1.1.1","11.11.11.11","111.111.111.111"
     *       OK
     *       AT^IMSPCSCF=1,,,,"10.40.102.100:80",,
     *       OK
     *       AT^IMSPCSCF?
     *       ^IMSPCSCF: 1,,,,"10.40.102.100:80",,
     *       OK
     *     �� ���õ�һ��IPV6��ַ ��ɾ���������IPV6��ַ������IPV4��ַ
     *       AT^IMSPCSCF=1,"[fe80:0:0:0:75a1:16a4:9252:a03a]:56","","","","",""
     *       OK
     *       AT^IMSPCSCF?
     *       ^IMSPCSCF: 1,"[fe80:0:0:0:75a1:16a4:9252:a03a]:56",,,,,
     *     �� ������õ�����DM��P-CSCF��ַ
     *       AT^IMSPCSCF=1,,,,,,
     *       OK
     *       AT^IMSPCSCF?
     *       ^IMSPCSCF: 1,,,,,,
     *       OK
     *     �� ����Ĭ�ϵ�P-CSCF��ַ����ѯ�����ʾ����Ĭ�ϵ�P-CSCF��ַ
     *       AT^IMSPCSCF=0,"1234:0:0:0:0:0:0:5678","[1234:0:0:0:0:0:0:5678]:80","1234:0:0:0:0:0:5678:90ab","1.1.1.1","11.11.11.11","111.111.111.111"
     *       OK
     *       AT^IMSPCSCF?
     *       ^IMSPCSCF: 1,,,,,,
     *       OK
     */
    { AT_CMD_PCSCF,
      AT_SetImsPcscfPara, AT_SET_PARA_TIME, AT_QryImsPcscfPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSPCSCF", (VOS_UINT8 *)"(0-4294967295),(data),(data),(data),(data),(data),(data)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ����DM�Ķ�̬�仯����
     * [˵��]: ������ΪLGU+ר�������������IMS��Ŀ��Ʋ�����
     * [�﷨]:
     *     [����]: ^DMDYN=[<AMR_WB_octet_aligned>],[<AMR_WB_bandwidth_efficient>],[<AMR_octet_aligned],[AMR_bandwidth_efficient>],[<AMR_WB_mode>],[<DTMF_WB>],[<DTMF_NB>],[<Speech_start>],[<Speech_end>],[<Video_start>],[<Video_end>],[<RegRetryBaseTime>],[<RegRetryMaxTime>],[<PhoneContext>],[<Public_user_identity>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR:<err><CR><LF>
     *     [����]: ^DMDYN?
     *     [���]: <CR><LF>^DMDYN: [<AMR_WB_octet_aligned>],[<AMR_WB_bandwidth_efficient>],[<AMR_octet_aligned>],[<AMR_bandwidth_efficient>],[<AMR_WB_mode>],[<DTMF_WB>],[<DTMF_NB>],[<Speech_start>],[<Speech_end>],[<Video_start>],[<Video_end>],[<RegRetryBaseTime>],[<RegRetryMaxTime>],[<PhoneContext>],[<Public_user_identity>]<CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^DMDYN=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <AMR_WB_octet_aligned>: AMR-WB �ֽڶ���ģʽ��payload type��ȡֵ��Χ��0x60~0x7F
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <AMR_WB_bandwidth_efficient>: AMR-WB �����ʡģʽ��payload type��ȡֵ��Χ��0x60~0x7F
     *             �˲���Ϊ��ѡ�����˵���˲���û�����á�Ŀǰ�˲���ΪIMSAԤ�����IMSA��ʹ�ô˲������ã����·������������ô˲����󣬲�ѯ����ķ���ֵ����ʾ����Ϊδ���ã����ʾ��
     *     <AMR_octet_aligned>: AMR�ֽڶ���ģʽ��payload type��ȡֵ��Χ��0x60~0x7F
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <AMR_bandwidth_efficient>: AMR �����ʡģʽ��payload type��ȡֵ��Χ��0x60~0x7F
     *             �˲���Ϊ��ѡ�����˵���˲���û�����á�Ŀǰ�˲���ΪIMSAԤ�����IMSA��ʹ�ô˲������ã����·������������ô˲����󣬲�ѯ����ķ���ֵ����ʾ����Ϊδ���ã����ʾ��
     *     <AMR_WB_mode>: AMR-WB���ʣ�ȡֵ��Χ�� 0~8
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <DTMF_WB>: DTMF 16000ʱ��Ƶ���µ�payload type��ȡֵ��Χ��0x60~0x7F
     *             �˲���Ϊ��ѡ�����˵���˲���û�����á�Ŀǰ�˲���ΪIMSAԤ�����IMSA��ʹ�ô˲������ã����·������������ô˲����󣬲�ѯ����ķ���ֵ����ʾ����Ϊδ���ã����ʾ��
     *     <DTMF_NB>: DTMF 8000ʱ��Ƶ���µ�payload type��ȡֵ��Χ��0x60~0x7F
     *             �˲���Ϊ��ѡ�����˵���˲���û�����á�Ŀǰ�˲���ΪIMSAԤ�����IMSA��ʹ�ô˲������ã����·������������ô˲����󣬲�ѯ����ķ���ֵ����ʾ����Ϊδ���ã����ʾ��
     *     <Speech_start>: ����RTP port��Сֵ��ֻ����ż��, 0Ϊ��Чֵ
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <Speech_end>: ����RTP port���ֵ��ֻ����ż��, 0Ϊ��Чֵ
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <Video_start>: ��ƵRTP port��Сֵ��ֻ����ż��, 0Ϊ��Чֵ
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <Video_end>: ��ƵRTP port���ֵ��ֻ����ż��, 0Ϊ��Чֵ
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <RegRetryBaseTime>: IMSAѡ����һ�γ���P-CSCF��ַ����Ҫֹͣ��ʱ�䣬��λΪ�롣
     *             <RegRetryBaseTime>��һ������������ʧ�ܴ�����ָ������
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <RegRetryMaxTime>: <RegRetryBaseTime>�������ӵ������ʱ�䣬��λΪ�롣
     *             <RegRetryBaseTime>���õ�ֵ��Ҫ����<RegRetryBaseTime>
     *             �˲���Ϊ��ѡ�����˵���˲���û������
     *     <PhoneContext>: IMSʹ�ò������ַ������飬ȡֵ��Χ0-128bytes
     *             �˲���Ϊ��ѡ��������ֻ���˵���˲���û������
     *     <Public_user_identity>: IMSʹ�ò������ַ������飬ȡֵ��Χ0-128bytes
     *             �˲���Ϊ��ѡ��������ֻ���˵���˲���û������
     * [ʾ��]:
     *       ����DM��̬���ò�����ͨ����ѯ����鿴����ǰ��������ñ仯
     *       AMR_WB_octet_aligned = 96;
     *       AMR_WB_bandwidth_efficient= 100;
     *       AMR_octet_aligned = 102
     *       AMR_bandwidth_efficient = 104,
     *       AMR_WB_mode = 8,
     *       DTMF_WB= 106,
     *       DTMF_NB= 108,
     *       Speech_start= 2
     *       Speech_end= 2
     *       Video_start������
     *       Video_end������
     *       RegRetryBaseTime= 5s
     *       RegRetryMaxTime= 30s
     *       PhoneContext������
     *     �� Public_user_identity= ""
     *       AT^DMDYN?
     *       ^DMDYN: 96,,96,,1,,,4,6,8,10,30,1800,"test_PhoneContext","test_public_identity"
     *       OK
     *       AT^DMDYN=96,100,102,104,8,106,106,2,2,,,5,30,,""
     *       OK
     *       AT^DMDYN?
     *       ^DMDYN: 96,,102,,8,,,2,2,,,5,30,"test_PhoneContext","test_public_identity"
     *       OK
     */
    { AT_CMD_DMDYN,
      AT_SetDmDynPara, AT_SET_PARA_TIME, AT_QryDmDynPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^DMDYN", (VOS_UINT8 *)"(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295),(phoneContext),(public_user_id)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ���úͲ�ѯIMS�Ķ�ʱ����Ϣ
     * [˵��]: �������������úͲ�ѯIMS��Ķ�ʱ����Ϣ��
     * [�﷨]:
     *     [����]: ^DMTIMER=[<Timer_T1>],[<Timer_T2>],[<Timer_T4>],[<Timer_TA>],[<Timer_TB>],[<Timer_TC>],[<Timer_TD>],[<Timer_TE>],[<Timer_TF>],[<Timer_TG>],[<Timer_TH>],[<Timer_TI>],[<Timer_TJ>],[<Timer_TK>]
     *     [���]: ������ȷ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF> +CME ERROR:<err><CR><LF>
     *     [����]: ^DMTIMER?
     *     [���]: <CR><LF>^DMTIMER: [<Timer_T1>],[< Timer_T2>],[<  Timer_T4>],[< Timer_TA>],[< Timer_TB>],[< Timer_TC>],[<  Timer_TD>],[< Timer_TE>],[< Timer_TF>],[< Timer_TG>],[<  Timer_TH>],[< Timer_TI>],[< Timer_TJ>],[< Timer_TK>] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^DMTIMER=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <Timer_T1>: IMS T1��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_T2>: IMS T2��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_T4>: IMS T4��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TA>: IMS A��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TB>: IMS B��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TC>: IMS C��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TD>: IMS D��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TE>: IMS E��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TF>: IMS F��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TG>: IMS G��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TH>: IMS H��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TI>: IMS I��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TJ>: IMS J��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     *     <Timer_TK>: IMS K��ʱ������λΪ���룬ȡֵ��Χ��0~128000�������ѡ��������д��˵���˶�ʱ��û������ʱ����
     * [ʾ��]:
     *     �� ����Timer_T1=2��Timer_TD=3�Ĳ���
     *       AT^DMTIMER=2,,,,,,3,,,,,,,
     *       OK
     *     �� ��ѯ��ʱ����Ϣ��
     *       AT^DMTIMER?
     *       ^DMTIMER: 2,,,,,,3,,,,,,,
     *       OK
     */
    { AT_CMD_DMTIMER,
      AT_SetImsTimerPara, AT_SET_PARA_TIME, AT_QryImsTimerPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^DMTIMER",
      (VOS_UINT8 *)"(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000),(0-128000)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ���ò�ѯIMS����ŵĹ��������ʶ��Ϣ
     * [˵��]: �������������úͲ�ѯIMS��Ĺ��������ʶ��PSI����Ϣ��
     * [�﷨]:
     *     [����]: ^IMSPSI=<SMS_PSI>
     *     [���]: ������ȷ��
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF> +CME ERROR:<err><CR><LF>
     *     [����]: ^IMSPSI?
     *     [���]: <CR><LF>^IMSPSI: <SMS_PSI ><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^IMSPSI=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <SMS_PSI>: �ַ������ͣ����ŵĹ��������ʶ��Ϣ���ַ������ȷ�Χ0-128bytes
     * [ʾ��]:
     *     �� ����IMS��Ķ��Ź�����ʶ��Ϣ
     *       AT^IMSPSI="test_sms_psi"
     *       OK
     *     �� ��ѯIMS��Ķ��Ź�����ʶ��Ϣ
     *       AT^IMSPSI?
     *       ^IMSPSI: "test_sms_psi"
     *       OK
     */
    { AT_CMD_IMSPSI,
      AT_SetImsSmsPsiPara, AT_SET_PARA_TIME, AT_QryImsSmsPsiPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^IMSPSI", (VOS_UINT8 *)"(data)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ��ѯIMS����û���Ϣ
     * [˵��]: ���������ڲ�ѯIMS����û���Ϣ��
     * [�﷨]:
     *     [����]: ^DMUSER?
     *     [���]: <CR><LF>^DMUSER: <Private_user_identity_IMPI>
     *             ,<Public_user_identity_IMPU>
     *             ,<Home_network_domain_name>
     *             , <Voice_Domain_Preference_EUTRAN>
     *             ,<SMS_over_IP_Networks_Indication>
     *             ,<Ipsec_enabled >
     *             <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^DMUSER=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <Private_user_identity_IMPI>: IMSʹ�ò������ַ������飬ȡֵ��Χ0-128bytes
     *     <Public_user_identity_IMPU>: IMSʹ�ò������ַ������飬ȡֵ��Χ0-128bytes
     *     <Home_network_domain_name>: IMSʹ�ò������ַ������飬ȡֵ��Χ0-128bytes
     *     <Voice_Domain_Preference_EUTRAN>: ����ֵ����������ģʽ��Ĭ��ֵ�ɾ��������̾�����
     *             0��CS Voice only
     *             1��IMS PS Voice only
     *             2��CS Voice preferred, IMS PS Voice as secondary
     *             3��IMS PS Voice preferred, CS Voice as secondary
     *     <SMS_over_IP_Networks_Indication>: ����ֵ��������ѡ��
     *             0����ʹ�� IMS���Ͷ��ţ�
     *             1������ʹ��IMS���Ͷ��š�
     *     <Ipsec_Enabled>: 0��IPSEC��ʹ��
     *             1��IPSEC����ʹ��
     * [ʾ��]:
     *     �� ��ѯIMS����û���Ϣ
     *       AT^DMUSER?
     *       ^DMUSER: "test_IMPI", "test_IMPU","test_domain",3,1,0
     *       OK
     */
    { AT_CMD_DMUSER,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryDmUserPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^DMUSER", (VOS_UINT8 *)"(char),(char),(char),(0-3),(0,1),(0,1)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: �����������Ƿ�֧��IMS����
     * [˵��]: ���������������������Ƿ�֧��IMS���ԣ������ϲ㽫���������Ƿ�֧��IMS�������õ�modem���Լ�ͨ���������ѯmodem��ǰ���������Ƿ�֧��IMS���ԡ�
     * [�﷨]:
     *     [����]: ^ROAMIMSSERVICE=<InternationalIMSService>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^ROAMIMSSERVICE?
     *     [���]: <CR><LF>^ROAMIMSSERVICE: < International IMS Service ><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^ROAMIMSSERVICE=?
     *     [���]: <CR><LF>^ROAMIMSSERVICE: (list of supported < International IMS Service >s) <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <International IMS Service>: �����ͣ��������Ƿ�֧��IMS�������ã�ȡֵ0,1��
     *             0����֧�֣�
     *             1��֧�֣�
     * [ʾ��]:
     *     �� �����������Ƿ�֧��IMS����
     *       AT^ROAMIMSSERVICE=1
     *       OK
     *     �� ��ѯ�������Ƿ�֧��IMS����
     *       AT^ROAMIMSSERVICE?
     *       ^ROAMIMSSERVICE: 1
     *       OK
     *     �� ִ�в�������
     *       AT^ROAMIMSSERVICE=?
     *       ^ROAMIMSSERVICE: (0,1)
     *       OK
     */
    { AT_CMD_ROAMIMSSERVICE,
      AT_SetRoamImsServicePara, AT_SET_PARA_TIME, AT_QryRoamImsServicePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ROAMIMSSERVICE", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: ֪ͨ����IMSע������
     * [˵��]: ����������֪ͨUE����IMSע�����̡�
     * [�﷨]:
     *     [����]: ^VOLTEREG
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ִ�д���ʱ��
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� ֪ͨ����IMSע������
     *       AT^VOLTEREG
     *       OK
     */
    { AT_CMD_VOLTEREG,
      AT_SetVolteRegPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VOLTEREG", VOS_NULL_PTR },
};

/* ע��phone phy AT����� */
VOS_UINT32 AT_RegisterPhoneImsCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atPhoneImsCmdTbl, sizeof(g_atPhoneImsCmdTbl) / sizeof(g_atPhoneImsCmdTbl[0]));
}
#endif
