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
#include "at_phone_taf_cmd_tbl.h"
#include "AtParse.h"
#include "at_parse_cmd.h"
#include "at_phone_taf_set_cmd_proc.h"
#include "at_phone_taf_qry_cmd_proc.h"
#include "at_test_para_cmd.h"
#include "at_device_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PHONE_TAF_CMD_TBL_C

static const AT_ParCmdElement g_atPhoneTafCmdTbl[] = {
    /*
     * [���]: Э��AT-�绰����
     * [����]: ָʾRSSI�仯
     * [˵��]: ��RSSI�仯����5dBmʱ��MT�����ϱ���ָʾ��TE��
     * [�﷨]:
     *     [����]: ^RSSI=[<n>]
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^RSSI=?
     *     [���]: <CR><LF>^RSSI: (list of supported <value>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: -
     *     [���]: <CR><LF>^RSSI: <rssi><CR><LF>
     * [����]:
     *     <n>: �����ͣ������Ƿ������ϱ�ϵͳģʽ��ȡֵ0��1��
     *             0����ֹ�����ϱ�ϵͳģʽ��
     *             1�����������ϱ�ϵͳģʽ��
     *             �����ò���ʱ������ֹ�����ϱ�����
     *     <rssi>: ����ֵ�������ź�ǿ�ȣ�ȡֵ0��31��99��
     *             0��С�ڻ���ڨC113 dBm
     *             1���C111 dBm
     *             2��30���C109 dBm���C53 dBm
     *             31�����ڻ���ڨC51 dBm
     *             99��δ֪�򲻿ɲ⡣
     * [ʾ��]:
     *       �����ϱ���ǰRSSIֵ
     *       ^RSSI: 16
     *     �� ����RSSI
     *       AT^RSSI=?
     *       ^RSSI: (0,1)
     *       OK
     */
    { AT_CMD_RSSI,
      At_SetRssiPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^RSSI", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: �����Ƿ���������ʱ���ϱ�
     * [˵��]: �����ͨ��MM INFORMATION��GMM INFORMATION����ǰʱ�估ʱ����Ϣ֪ͨ�նˣ������ն�ʵʱ��������ʱ�䡣���������������ն��Ƿ��ϱ���Ӧ�������·���ʱ����Ϣ��
     * [�﷨]:
     *     [����]: ^TIME=<value>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TIME?
     *     [���]: <CR><LF>^TIME: <value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^TIME=?
     *     [���]: <CR><LF>^TIME: (list of supported <value>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: 0�����ϱ���
     *             1���ϱ���
     * [ʾ��]:
     *     �� ���ô�����ʱ���ϱ�
     *       AT^TIME=1
     *       OK
     *     �� ��ѯ
     *       AT^TIME?
     *       ^TIME: 1
     *       OK
     */
    { AT_CMD_TIME,
      At_SetTimePara, AT_SET_PARA_TIME, At_QryTimePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^TIME", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: �����ϵͳʱ���ѯ
     * [˵��]: SET���֧�֡�
     *         READ�������ڲ�ѯ��ǰEMM/MM INFO�е������ϵͳʱ�䡣��������û�·�EMM/MM INFO������Ҫ���ص�ǰע����PLMN���ڵ�ʱ�������֮ǰû�յ���EMM INFOҲûפ�����κ����磬����ERROR��
     *         TEST���֧�֡�
     * [�﷨]:
     *     [����]: AT^TIMEQRY?
     *     [���]: <CR><LF>^TIMEQRY: < date >,< time>,<dt><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ����ִ��ʧ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <date>: ���ں�ʱ����Ϣ����ʽΪyy/MM/dd��
     *     <time>: ��ʽΪ"hh:mm:ss+tz"��<time>�ֶ���Ϣ����������ɣ�ʱ���ʱ�������硰05:56:13+32����tz:ʱ����Ϣ��ʱ����λΪ15����,  +32 ��ʾ32x15���� �� + 8Сʱ��
     *     <dt>: ����ʱ��������û���·��˲���ʱ�����ϱ�0�������ϱ���Ӧ������ʱ������ȡֵ������������ο�table 10.5.97a/3GPP TS 24.008
     * [ʾ��]:
     *       �������ע�ᵽ�����ϣ�������·�GMM information������������ڡ�ʱ�䡢ʱ��
     *       �����ϱ����£�^TIME: 13/03/22,12:30:00+32,0
     *     �� һ��Сʱ֮���·���ѯ���
     *       AT^TIMEQRY?
     *       ^TIMEQRY: 13/03/22,13:30:00+32,0
     *       OK
     *     �� �������ע�ᵽ�����ϣ������û���·�EMM/GMM/MM information,��ǰע������Ϊ��46001����ͨ��MNC(������)�ж����й���ʱ��Ϊ���������·���ѯ���
     *       AT^TIMEQRY?
     *       ^TIMEQRY: 90/01/06,08:00:00+32,0
     *       OK
     */
    { AT_CMD_TIMEQRY,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryTimeQryPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TIMEQRY", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ������Ӫ�̶��ƽ���������
     * [˵��]: ����������Ӫ�̶��ƽ��������롣
     * [�﷨]:
     *     [����]: ^NVM=<index>,<total>,<ecc_num>,<category>,<simpresent>,<mcc>[,<abnormal_service>]
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NVM=?
     *     [���]: <CR><LF>^NVM: (list of supported <index>s),( list of supported<total>s),<ecc_num>,( list of supported <category>s),(list of supported <simpresent>s),( list of supported <mcc>s), (list of supported <abnormal_service>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <index>: ����ֵ������������������1��ʼ��ȡֵ��Χ[1-20]��
     *     <total>: ����ֵ��������������ȡֵ��Χ[1-20]��
     *     <ecc_num>: �ַ�����ʽ�Ľ��������롣
     *     <category>: ���������ͣ�ȡֵ��Χ[0-255]��
     *             1���˾���
     *             2���Ȼ���
     *             4���𾯣�
     *             8��������
     *             16����ɽӪ�ȡ�
     *     <simpresent>: ��ʾ���������п�ʱ��Ч�����޿�ʱ��Ч��ȡֵ��Χ[0,1]��
     *             0���޿�ʱ��Ч��
     *             1���п�ʱ��Ч��
     *     <mcc>: ����ֵ�������룬ȡֵ��Χ[0-65535]��
     *     <abnormal_service>: ����ֵ����ʾ������Ч���Ƿ�����CS�����������״̬���˲�������<simpresent>Ϊ1���п�ʱʹ�ã�ȡֵ��Χ[0,1]��
     *             0������״̬����Ч��
     *             1������CS�����������ʱ��Ч��
     * [ʾ��]:
     *     �� ���ƽ���������
     *       AT^NVM=1,3,"999",2,1,460,0
     *       OK
     *       AT^NVM=2,3,"120",1,1,460,1
     *       OK
     *       AT^NVM=3,3,"120",1,0,460
     *       OK
     */
    { AT_CMD_NVM,
      AT_SetNvmEccNumPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^NVM", (VOS_UINT8 *)"(1-20),(1-20),(eccnum),(0-255),(0,1),(0-65535),(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ����������ָʾ
     * [˵��]: ��ѯ����û�������ѯ���������룬�ϱ������������б�
     *         XLEMA��Դ��
     *         1.��׼Э�鶨��Ľ������룬�����п�����������޿���������
     *         2.�忨ʱSIM��USIM�����ﱣ��Ľ�������
     *         3.פ���������·��Ľ�������
     *         4.��Ʒ�߸�����Ҫ���ƵĽ������루�ο�NV�ֲᣩ
     *         �����ϱ����������к���仯ʱ�������ϱ������������б����������ϱ�������
     *         1.��Ҫ�����ϱ�ECC��NV����ο�NV�ֲᣩ
     *         2.�յ������·�ECC�仯����ͨ��^NVM���óɹ��󴥷������ϱ����ܡ�
     * [�﷨]:
     *     [����]: ^XLEMA?
     *     [���]: <CR><LF>^XLEMA: <index1>,<total>,<ecc_num>,<category>,<simpresent>, <mcc>,<mnc>,<abnormal_service>,<urns>,<source><CR><LF>
     *             ^XLEMA: <index2>,<total>,<ecc_num>,<category>,<simpresent>, <mcc>,<mnc>,<abnormal_service>,<urns>,<source><CR><LF>
     *             ��
     *             <CR><LF>OK<CR><LF>
     *     [����]: -
     *     [���]: <CR><LF>^XLEMA: <index>,<total>,<ecc_num>,<category>,<simpresent>, <mcc>,<mnc>,<abnormal_service>,<urns>,<source><CR><LF>
     * [����]:
     *     <index>: ����������������1��ʼ��
     *     <total>: ������������
     *     <ecc_num>: ���������롣
     *     <category>: ���������͡�ֵ������ϡ�
     *             0�����������ʱ�տ���Ϣ��Я��CATEGORY IE�
     *             1���˾���
     *             2���Ȼ���
     *             4���𾯣�
     *             8��������
     *             16����ɽӪ�ȣ�
     *             31�����н�����������ͬʱ��Ч��
     *     <simpresent>: ��ʾ���������п�ʱ��Ч�����޿�ʱ��Ч��
     *             0���޿�ʱ��Ч��
     *             1���п�ʱ��Ч��
     *     <mcc>: �����룬Ĭ��ֵΪfff��
     *     <mnc>: �ƶ������룬Ĭ��ֵΪff��
     *     <abnormal_service>: ����ֵ����ʾ������Ч���Ƿ�����CS�����������״̬���˲�������<simpresent>Ϊ1���п�ʱ��Ч��
     *             0������״̬����Ч��
     *             1������CS�����������ʱ��Ч��
     *     <urns>: �������к���ͳһ��Դ���ƣ���չ�������к�����У����ַ������ͣ���󳤶�Ϊ64��Ĭ��Ϊ�ա�
     *     <source>: ����ֵ��ȡֵ0-3����ʾ�������������Դ��
     *             0����Դ��APP���ã�AT^NVM������Ӫ�̶��ƣ���
     *             1����Դ��Э��涨��Ҫ��modemӲ����ģ���
     *             2����Դ�ڿ��ļ���
     *             3����Դ�������·���
     * [ʾ��]:
     *     �� XLEMA��ѯ
     *       AT^XLEMA?
     *       ^XLEMA: 1,17,110,0,1,fff,ff,0,"",2
     *       ^XLEMA: 2,17,119,0,1,fff,ff,0,"",2
     *       ^XLEMA: 3,17,120,0,1,fff,ff,0,"",2
     *       ^XLEMA: 4,17,112,0,1,fff,ff,0,"",2
     *       ^XLEMA: 5,17,119,4,1,460,ff,0,"",3
     *       ^XLEMA: 6,17,112,2,1,460,ff,0,"",3
     *       ^XLEMA: 7,17,911,31,1,460,ff,0,"",3
     *       ^XLEMA: 8,17,110,1,1,460,ff,0,"",3
     *       ^XLEMA: 9,17,122,1,1,460,ff,0,"",3
     *       ^XLEMA: 10,17,123,31,1,460,ff,0,"",3
     *       ^XLEMA: 11,17,110,0,0,fff,ff,0,"",1
     *       ^XLEMA: 12,17,112,0,0,fff,ff,0,"",1
     *       ^XLEMA: 13,17,911,0,0,fff,ff,0,"",1
     *       ^XLEMA: 14,17,999,0,0,fff,ff,0,"",1
     *       ^XLEMA: 15,17,119,0,0,fff,ff,0,"",1
     *       ^XLEMA: 16,17,120,0,0,fff,ff,0,"",1
     *       ^XLEMA: 17,17,122,0,0,fff,ff,0,"",1
     *       OK
     *       �����ϱ�
     *       ^XLEMA: 5,5,118,0,1,460,11,0,"urn:service:sos",3
     */
    { AT_CMD_XLEMA,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryXlemaPara, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^XLEMA", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ʹ�������ϱ�����ѡ��˵�
     * [˵��]: ��ָʾӦ������ѡ��˵�ʹ�ܻ���ȥʹ�ܡ�ȥʹ��ʱ������ѡ��˵������ã��û�����ͨ������ѡ��˵�����ѡ����
     *         ������ֻ��������ѡ��˵�����NV2668�򿪣����ҿ�֧�ָ�����(���ļ�6F15��PLMN_MODEλ�Ƿ����)ʱ�Ż��ϱ���
     *         ������Ϊ˽������ض���Ӫ�̶��ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+PACSP <status><CR><LF>
     *     [����]: +PACSP?
     *     [���]: <CR><LF>+PACSP <status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <status>: �����ͣ�ȡֵ0��1��
     *             0��ȥʹ������ѡ��˵�������ģʽΪ�Զ���
     *             1��ʹ������ѡ��˵�������ģʽ���ı䡣
     * [ʾ��]:
     *       ȥʹ������ѡ��˵�����ģ���ϱ���ϢEFCSP��
     *       +PACSP0
     *     �� ��ѯ����ѡ��˵��Ƿ�ʹ��
     *       AT+PACSP?
     *       +PACSP1
     *       OK
     */
    { AT_CMD_PACSP,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryPacspPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"+PACSP", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ���ż���������
     * [˵��]: �������������ø��ż����ز�����
     * [�﷨]:
     *     [����]: ^JDCFG=<rat>,<value1>,<value2>[,<value3>,<value4>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^JDCFG?
     *     [���]: <CR><LF>^JDCFG: <rat>,<value1>,<value2>[,<value3>,<value4>]
     *             [<CR><LF>^JDCFG: <rat>,<value1>,<value2>[,<value3>,<value4>][��]]
     *             <CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^JDCFG=?
     *     [���]: <CR><LF>^JDCFG: <rat>,(list of supported <value1>s),(list of supported <value2>s)[,(list of supported <value3>s),(list of supported <value4>s)]
     *             [<CR><LF>^JDCFG: <rat>,(list of supported <value1>s),(list of supported <value2>s)[,(list of supported <value3>s),(list of supported <value4>s)] [��]]<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <rat>: ����ֵ��������ʽ��
     *             0��GSM
     *             1��WCDMA
     *             2��LTE
     *     <rssi_thresh>: ����ֵ������RSSIʱ�������ź�ǿ�Ȳ���ֵ���ڸ�����ֵΪ���ɸ���Ƶ�㡣ȡֵ��Χ��0-70��
     *             GSM\\WCDMA��ʽ�£�rssi_thresh��Ӧ��ʵ������ֵΪ��
     *             0��-70dbm
     *             1~69��-69~-1dbm
     *             70��0dbm
     *             LTE��ʽ�£�rssi_thresh��Ӧ��ʵ������ֵΪ��
     *             0��-120dbm
     *             1~69��-119~-51dbm
     *             70��-50dbm
     *     <rssi_num>: ����ֵ������RSSIʱ��������Ƶ�������������ֵ���ж�Ϊ�ܵ����š�ȡֵ��Χ��0-255��
     *     <rssi_percent>: ����ֵ������RSSIʱ�������ɸ���Ƶ��ռƵ�η�ΧƵ�������İٷֱȡ�ȡֵ��Χ��0-100��
     *     <psch_thresh>: ����ֵ������PSCHʱ������ֵС�ڵ��ڸ���ֵ��Ƶ��Żᱻ�ж�Ϊ����Ƶ�㡣ȡֵ��Χ��0-65535��
     *     <psch_percent>: ����ֵ������PSCHʱ��Ƶ���ڸ���Ƶ�����ռ���ɸ��ŷ�Χ��Ƶ������İٷֱȳ�������ֵ����Ƶ�βű��ж�Ϊ�ܵ����š�ȡֵ��Χ��0-100��
     *     <pssratio_thresh>: ����ֵ������PSS Ratioʱ������ֵС�ڸ���ֵ��Ƶ��Żᱻ�ж�Ϊ����Ƶ�㡣ȡֵ��Χ��0-1000��
     *     <pssratio_percent>: ����ֵ������PSS Ratioʱ��Ƶ���ڸ���Ƶ�����ռ���ɸ��ŷ�Χ��Ƶ������İٷֱȳ�������ֵ����Ƶ�βű��ж�Ϊ�ܵ����š�ȡֵ��Χ��0-100��
     * [��]: <value1>,<value2>,<value3>,<value4>����ͬ��ʽ�¶�Ӧ�ĸ��ż�����ò������ο�������˵����
     *       <rat>, <value1>,    <value2>,     <value3>,        <value4>,
     *       GSM,   rssi_thresh, rssi_num,     ,                ,
     *       WCDMA, rssi_thresh, rssi_percent, psch_thresh,     psch_percent,
     *       LTE,   rssi_thresh, rssi_percent, pssratio_thresh, pssratio_percent,
     * [ʾ��]:
     *     �� ��ѯ��ǰ���ż�����ò���
     *       AT^JDCFG?
     *       ^JDCFG: 0,10,30
     *       ^JDCFG: 1,40,70,0,80
     *       ^JDCFG: 2,10,70,0,80
     *       OK
     *     �� ����GSM��ʽ�¸��ż�����
     *       AT^JDCFG=0,10,70
     *       OK
     *     �� ��ѯ���ż�����ò�����ȡֵ��Χ
     *       AT^JDCFG=?
     *       ^JDCFG: (0),(0-70),(0-255)
     *       ^JDCFG: (1),(0-70),(0-100),(0-65535),(0-100)
     *       ^JDCFG: (2),(0-70),(0-100),(0-1000),(0-100)
     *       OK
     */
    { AT_CMD_JDCFG,
      AT_SetJDCfgPara, AT_SET_PARA_TIME, AT_QryJDCfgPara, AT_QRY_PARA_TIME, AT_TestJDCfgPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^JDCFG", (VOS_UINT8 *)"(0,1,2),(0-70),(0-255),(0-65535),(0-100)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ���ż�⿪������
     * [˵��]: �������������ø��ż�⿪�أ����ṩ��ѯ������ʽ�¸��ż�⿪��״̬���ܡ����ż�⹦�ܿ���������һ��ɨƵ�׶���Ч��
     *         ��AT����JAM_DET����֧�ֵĲ�Ʒ��Ч����֧�ָ����ԵĲ�Ʒ��AT��������󣬷���error����Miami��Baltimore�Ȳ�Ʒ��֧�ָ�AT����
     * [�﷨]:
     *     [����]: ^JDSWITCH=<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^JDSWITCH?
     *     [���]: <CR><LF>^JDSWITCH: <mode>,<rat>[<CR><LF>^JDSWITCH: <mode>,<rat>[��]]<CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^JDSWITCH=?
     *     [���]: <CR><LF>^JDSWITCH: (list of supported <mode>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ�����ż�⹦�ܿ��ء�
     *             0���رգ�Ĭ��ֵ��
     *             1������
     *             ע�⣺��������������GUL���ż�⹦�ܣ����ĳһ��ʽ�¸��ż�⹦�ܿ���ʧ�ܣ��᷵��ʧ�ܡ���ʱ������ʹ�ò�ѯ�����ȡ������ʽ�£����ż�⹦�ܿ���״̬�����������á�
     *     <rat>: ����ֵ��������ʽ��
     *             0��GSM
     *             1��WCDMA
     *             2��LTE
     * [ʾ��]:
     *     �� ��ѯ��ǰ���ż�⿪��״̬
     *       AT^JDSWITCH?
     *       ^JDSWITCH: 1,0
     *       ^JDSWITCH: 1,1
     *       ^JDSWITCH: 1,2
     *       OK
     *     �� �������ż�⹦��
     *       AT^JDSWITCH=1
     *       OK
     */
    { AT_CMD_JDSWITCH,
      AT_SetJDSwitchPara, AT_SET_PARA_TIME, AT_QryJDSwitchPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^JDSWITCH", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: ����ͨ�Ŵ��ֻ�ģʽ
     * [˵��]: ��������������ͨ�Ŵ��ֻ�ģʽ�������ϲ㽫TTYģʽ���õ�modem���Լ�ͨ���������ѯmodem��ǰ��TTYģʽ��
     *         modem������ʼ��Ĭ��ΪTTY_OFFģʽ��
     *         ˫��˫ͨ��̬�ֻ�Ҫ��Ӧ�����������õ�TTYģʽһ�£������һ��modem�������һ�����õ�TTYģʽΪ׼��
     * [�﷨]:
     *     [����]: ^TTYMODE=<mode>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TTYMODE?
     *     [���]: <CR><LF>^TTYMODE: <mode><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TTYMODE=?
     *     [���]: <CR><LF>^TTYMODE: (list of supported <mode>s) <CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: �����ͣ�TTYģʽ��ȡֵ0~3��
     *             0��TTY_OFFģʽ��
     *             1��TTY_FULLģʽ��
     *             2��TTY_VCOģʽ��
     *             3��TTY_HCOģʽ��
     * [ʾ��]:
     *     �� ����TTYģʽΪTTY_FULLģʽ
     *       AT^TTYMODE=1
     *       OK
     *     �� ��ѯTTYģʽ
     *       AT^TTYMODE?
     *       ^TTYMODE: 1
     *       OK
     *     �� ִ�в�������
     *       AT^TTYMODE=?
     *       ^TTYMODE: (0-3)
     *       OK
     */
    { AT_CMD_TTYMODE,
      AT_SetTTYModePara, AT_SET_PARA_TIME, AT_QryTTYModePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TTYMODE", (VOS_UINT8 *)"(0-3)" },

#if (FEATURE_IOT_CMUX == FEATURE_ON)
    { AT_CMD_CMUX,
      At_SetCmuxPara, AT_SET_PARA_TIME, At_QryCmuxPara, AT_QRY_PARA_TIME, At_TestCmuxPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8*)"+CMUX", (VOS_UINT8*)"(0),(0),(1-6),(31-1540),(10-250),(0-10),(10-250),(1-255),(1-7)"},
#endif

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /* AUTO TEST */
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: C��CPU���ز�ѯ
     * [˵��]: ���������ڲ�ѯC��CPU���ء�
     * [�﷨]:
     *     [����]: ^CPULOAD?
     *     [���]: <CR><LF>^CPULOAD: <cpu_load>[��] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cpu_load>: �����ͣ�ÿ���˵ĸ���ռ�ðٷֱȡ�
     *             �������2λС����
     * [ʾ��]:
     *     �� ��ѯ����
     *       AT^CPULOAD?
     *       ^CPULOAD: 1.20,1.00,0.00,0.00
     *       OK
     */
    { AT_CMD_CPULOAD,
      TAF_NULL_PTR, AT_NOT_SET_TIME, At_QryCpuLoadPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CPULOAD", TAF_NULL_PTR },
#endif
};

/* ע��taf phone AT����� */
VOS_UINT32 AT_RegisterPhoneTafCmdTbl(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atPhoneTafCmdTbl, sizeof(g_atPhoneTafCmdTbl) / sizeof(g_atPhoneTafCmdTbl[0]));
}
