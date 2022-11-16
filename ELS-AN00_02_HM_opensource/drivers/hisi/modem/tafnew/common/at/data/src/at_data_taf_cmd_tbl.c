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

#include "at_data_taf_set_cmd_proc.h"
#include "at_data_taf_qry_cmd_proc.h"
#include "at_data_taf_cmd_tbl.h"
#include "at_cmd_proc.h"
#include "at_test_para_cmd.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_DATA_TAF_CMD_TBL_C

static const AT_ParCmdElement g_atDataTafCmdTbl[] = {
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����DNS
     * [˵��]: ���û�ɾ��PDP������ʹ�õ�DNS��Domain Name Server����Ϣ������������DNS��
     *         ���������ֻ��<cid>һ����������AT^CGDNS=<cid>����ʾɾ����<cid>��Ӧ��PDP�����ĵ�DNS��Ϣ��
     *         ִ��at^cgdns���á�ɾ������ʱ��cid�Ĳ�����ΧΪ1-11��
     *         ʹ��at^cgdns?��at^cgdns=?ʱ�����ص�cid��ΧΪ1-31�����û��PDP�����ģ����ؿգ�������ȱʡ���غ�ר�г��أ���
     *         ע�⣺ʹ��at^cgdns=?ʱ������ֵΪ����+CGACT�Ѽ����PID��
     * [�﷨]:
     *     [����]: ^CGDNS=<cid>,[<PriDns>,[<SecDns>]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CGDNS?
     *     [���]: <CR><LF>^CGDNS: <cid>,[<PriDns>,[<SecDns>]]
     *             [<CR><LF>^CGDNS: <cid>,[<PriDns>,[<SecDns>]]
     *             [...]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CGDNS=?
     *     [���]: <CR><LF>^CGDNS: (list of <cid>s associated with active contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����ȡֵ��ΧΪ1��11��
     *     <PriDns>: �ַ������ͣ�IPV4���͵���DNS��ַ��
     *     <SecDns>: �ַ������ͣ�IPV4���͵ĸ�DNS��ַ��
     * [ʾ��]:
     *     �� ����DNS����
     *       AT^CGDNS=1,"192.168.0.10","192.168.0.200"
     *       OK
     *     �� ��ѯ����
     *       AT^CGDNS?
     *       ^CGDNS: 1,"192.168.0.10","192.168.0.200"
     *       OK
     *     �� ����������ؼ����cid
     *       AT^CGDNS=?
     *       ^CGDNS: (1)
     *       OK
     */
    { AT_CMD_CGDNS,
      AT_SetCgdnsPara, AT_SET_PARA_TIME, AT_QryCgdnsPara, AT_QRY_PARA_TIME, At_TestCgdnsPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^CGDNS", (VOS_UINT8 *)"(1-11),(PriDns),(SecDns)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯ������
     * [˵��]: ��ѯ����ĳ������������Ĵ����룬Ŀǰ���ṩ��ѯ����ʧ��ʱ�Ĵ����롣�������������ƴ������ϱ��Ŀ�����NV�����ʹ�ã����򷵻�ERROR��
     * [�﷨]:
     *     [����]: ^GLASTERR=<type>
     *     [���]: <CR><LF>^GLASTERR: <type>,<code><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^GLASTERR?
     *     [���]: <CR><LF>^GLASTERR: <type><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^GLASTERR=?
     *     [���]: <CR><LF>^GLASTERR: (list of supported <type>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: �������͡�
     *             0��������
     *             1�����������
     *             2~255��������
     *     <code>: �����롣
     *             0~65535����������롣
     * [ʾ��]:
     *     �� PPP����ʧ�ܺ�ʹ��AT^GLASTERR=1��ѯʧ�ܵ�ԭ��.
     *       AT^GLASTERR=1
     *       ^GLASTERR: 1,8
     *       OK
     *     �� ��ѯ����
     *       AT^GLASTERR?
     *       ^GLASTERR: 1
     *       OK
     *     �� ��������
     *       AT^GLASTERR=?
     *       ^GLASTERR: (0-255)
     *       OK
     */
    { AT_CMD_GLASTERR,
      At_SetGlastErrPara, AT_SET_PARA_TIME, At_QryGlastErrPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^GLASTERR", (VOS_UINT8 *)"(0-255)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯ������Э�̺��������DNS
     * [˵��]: PDP�����APͨ���������ѯ������Э�̵���DNS����DNS��Ϣ����AP����ʱʹ�á�����CIDָ��PDPδ�����������ֱ�ӷ���ERROR��
     *         ĿǰAP��֧��IPv4���͡�
     * [�﷨]:
     *     [����]: ^DNSQUERY=<cid>
     *     [���]: <CR><LF>^DNSQUERY: <pDns>,<sDns><CR><LF> <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^DNSQUERY=?
     *     [���]: <CR><LF>^DNSQUERY: (list of supported<cid>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP �����ı�ʶ����
     *             LTE�汾ȡֵ��Χ1~31��
     *             �����汾ȡֵ��Χ1~11��
     *     <pDns>: �ַ������ͣ�ָ����<cid>��Ӧ���� DNS ��ַ��
     *     <sDns>: �ַ������ͣ�ָ����<cid>��Ӧ�ĸ� DNS ��ַ��
     * [ʾ��]:
     *     �� ��ѯCIDΪ1��������DNS��Ϣ����ǰCID 1�Ѿ����
     *       AT^DNSQUERY=1
     *       ^DNSQUERY: "192.168.0.10","192.168.0.200"
     *       OK
     */
    { AT_CMD_DNSQUERY,
      AT_SetDnsQueryPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^DNSQUERY", (VOS_UINT8 *)DNSQUERY_TEST_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����Fast Dormancy
     * [˵��]: ʹ����ģ����ٽ������߻���ʡ��ģʽ��һ��ܣ�AP���ж�����һ��������ʹ�ø�ָ��ָʾ���߿���Modem��Fast Dormancy���ܽ������á�
     * [�﷨]:
     *     [����]: ^FASTDORM=<type>[,<timer_length>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^FASTDORM?
     *     [���]: <CR><LF>^FASTDORM: <type>,<timer_length><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^FASTDORM=?
     *     [���]: <CR><LF>^FASTDORM: (list of supported <type>s)[, (list of
     *             supported <timer_length>s)]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: ����ֵ��Fast Dormancy�����������͡�
     *             0��ֹͣ���ߣ�
     *             1��ֻ����Fast Dormancy��
     *             2��ֻ����ASCR��Autonomous Signalling Connection Release����
     *             3������ASCR��Fast Dormancy��
     *     <timer_length>: ����ֵ����ǰ״̬�����������ÿ��Խ���Fast Dormancy������Ϊ��ѡ�û������ʱʹ��֮ǰ���õ�ʱ����UEĬ�ϵ�ֵ����λΪs��ȡֵ��Χ1~30��Ĭ��ֵΪ5��
     * [ʾ��]:
     *     �� ����ASCR��Fast Dormancy���ͣ�������������ͳ��ʱ��Ϊ5��
     *       AT^FASTDORM=3,5
     *       OK
     *     �� ��ѯ��ǰFast Dormancy����
     *       AT^FASTDORM?
     *       ^FASTDORM: 3,5
     *       OK
     *     �� ��������
     *       AT^FASTDORM=?
     *       ^FASTDORM: (0,1,2,3),(1-30)
     *       OK
     */
    { AT_CMD_FASTDORM,
      AT_SetFastDormPara, AT_SET_PARA_TIME, AT_QryFastDormPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^FASTDORM", (VOS_UINT8 *)"(0,1,2,3),(1-30)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯIPV6����
     * [˵��]: ���������ڲ�ѯIPV6������
     *         ��̨����ʱ�����ô�AT��ѯ��ǰMT��IPV6������
     * [�﷨]:
     *     [����]: ^IPV6CAP?
     *     [���]: <CR><LF>^IPV6CAP: <cfg_value><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^IPV6CAP=?
     *     [���]: <CR><LF>^IPV6CAP: (list of supported <cap_value>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             IPV6��������������NV��ƣ�Ϊ������չ���˴�IPv6�����б�̶���ʾΪ<0-0xFF>
     * [����]:
     *     <cap_value>: IPV6��������ֵ��ȡֵ��Χ��0��0xFF��
     *             0x01��IPV4 ONLY��
     *             0x02��IPV6 ONLY��
     *             0x07��IPV4 ONLY��IPV6 ONLY��IPV4V6ʹ����ͬAPN��
     *             0x0B��IPV4 ONLY��IPV6 ONLY��IPV4V6ʹ�ò�ͬAPN��
     *             ����ֵ������
     * [ʾ��]:
     *     �� MT��֧��IPV4
     *       AT^IPV6CAP?
     *       ^IPV6CAP: 1
     *       OK
     *     �� MT��֧��IPV6
     *       AT^IPV6CAP?
     *       ^IPV6CAP: 2
     *       OK
     *     �� MT֧��IPV4��IPV6��IPV4V6������3GPP R9�汾��
     *       AT^IPV6CAP?
     *       ^IPV6CAP: 7
     *       OK
     *     �� ��������
     *       AT^IPV6CAP=?
     *       ^IPV6CAP: <0-0xFF>
     *       OK
     */
    { AT_CMD_IPV6CAP,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryIpv6CapPara, AT_NOT_SET_TIME, AT_TestIpv6capPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IPV6CAP", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-NDIS����
     * [����]: ����ҵ�񲦺�
     * [˵��]: ����������ʵ��NDIS���š�
     *         at^ndisdup=1,1��NDIS���ţ�
     *         at^ndisdup=1,0���Ͽ�NDIS�������ӣ�
     *         ֧��E5��STICK���ŷ�ʽ��
     *         HiB5000֧��ʹ��CID 1~20���в��ţ�ͨ��ҵ����ֻʹ��1-11��12-20����������������Գ���(��GCF����)��
     * [�﷨]:
     *     [����]: ^NDISDUP=<cid>,<connect>[,<APN>[,<username>[,<passwd>[,<authpref>][,<ipaddr>][,<bitRatType>]]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������:
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^NDISDUP?
     *     [���]: <CR><LF>OK<CR><LF>
     *     [����]: ^NDISDUP=?
     *     [���]: <CR><LF>^NDISDUP: (list of supported <cid>s) , (list of supported <connect>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             GUΪ1��16��Ŀǰֻ֧��11����������չ��16����
     *             GULΪ1��20��
     *     <connect>: �������͡�
     *             0���Ͽ����ӣ�
     *             1���������ӣ�
     *             2��ǿ��ȥ�������ӣ����LTE��ֻ����Ĭ�ϳ�������£���
     *     <APN>: ��������ƣ��ַ�����ȡֵ��Χ0-99bytes��
     *     <username>: �û������ַ�����ȡֵ��Χ0-99bytes��
     *     <passwd>: ���룬�ַ�����ȡֵ��Χ0-99bytes��
     *     <authpref>: ����ֵ����֤Э�顣ȡֵ���£�
     *             GUTLģʽ
     *             0��NONE������Ȩ����
     *             1��PAP��
     *             2��CHAP��
     *             3��MsChapV2��Ŀǰ�ݲ�֧�֣���
     *             CLģʽ
     *             0��NONE������Ȩ����
     *             1��PAP��
     *             2��CHAP��
     *             3��PAP or CHAP��
     *     <ipaddr>: IP��ַ����ʽΪ��192.168.11.8��
     *     <bitRatType>: �˴����ݲ��ſ�������Щ���뼼���³��ԣ������ʽʡ��ʱ���Բ��Ų������뼼�������ƣ�
     *             0���Բ��Ų������뼼�������ƣ�
     *             36��������LTEģʽ�³��Բ���
     *             ������ο��±�
     *             ע�⣺�˲���ֻ����CLģʽ��ʹ�ã�GUL��L��ģ�����ɡ�
     * [��]: <BitRatType>bitλ��
     *       bit:   7,        6,        5,        4,        3,        2,    1,     0,
     *              RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, LTE,  WCDMA, GSM,
     * [ʾ��]:
     *     �� NDIS����
     *       AT^NDISDUP=1,1,"HUAWEI.COM"
     *       OK
     *     �� ��ѯ����
     *       AT^NDISDUP?
     *       OK
     *     �� ��������
     *       AT^NDISDUP=?
     *       ^NDISDUP�� (1-20),(0-1)
     *       OK
     */
    { AT_CMD_NDISDUP,
      AT_SetNdisdupPara, AT_SET_PARA_TIME, At_QryNdisdupPara, AT_QRY_PARA_TIME, At_TestNdisDup, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^NDISDUP", (VOS_UINT8 *)NDISDUP_TEST_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: DS��������
     * [˵��]: ��DS�������㣬����DS�ۼ�����ʱ�䡢DS�ۼƷ���������DS�ۼƽ������������һ��DS������ʱ�䡢���һ��DS�ķ������������һ��DS�Ľ�����������6�����ݵ�˵�����^DSFLOWQRY���������6�������ڳ���ʱ��ʼ������Ϊ0��
     * [�﷨]:
     *     [����]: ^DSFLOWCLR
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� ������NSAPIִ�з�������ͳ����Ϣ�������
     *       AT^DSFLOWCLR
     *       OK
     */
    { AT_CMD_DSFLOWCLR,
      AT_SetDsFlowClrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DSFLOWCLR", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯDS����
     * [˵��]: ��ѯ���һ��DS������ʱ����������Լ�DS�ۼƵ�����ʱ���������
     *         �����ǰ����online_data״̬�����һ��DSָ���ǵ�ǰ���DS������ָ���Ǵ�ǰ���һ�η�����DS��DS�ۼƵ�����ʱ�������ָ��DS�������һ����������ǰ���ʱ�������з�����DS������ʱ����������ۼ�ֵ��
     *         Stick��E5��̬�������²��죺
     *         E5��ѯ�õ��������ǵ����������������е����һ��DS������ʱ�䡢���������������������Լ�DS�ۼƵ�����ʱ�䡢��������������������
     *         STICK��ѯ�õ��������ǵ����ϵ��ڼ�����һ��DS������ʱ�䡢���������������������Լ�DS�ۼƵ�����ʱ�䡢��������������������
     *         ��FEATURE_MBB_CUST��򿪣�����CID����ʱ����ʾ��ѯ���е�CID�����������CID����ʱ��CID��ΧΪ1-11�������ѯָ��CID������
     * [�﷨]:
     *     [����]: ^DSFLOWQRY=[cid]
     *     [���]: <CR><LF>^DSFLOWQRY: <last_ds_time>,<last_tx_flow>, <last_rx_flow>,<total_ds_time>,<total_tx_flow>, <total_rx_flow><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <last_ds_time>: 0x0000_0000��0xFFFF_FFFF��8λ16����������ʾ���һ��DS������ʱ�䣬��λΪ�롣
     *     <last_tx_flow>: 0x0000_0000_0000_0000��0xFFFF_FFFF_FFFF_FFFF��16λ16����������ʾ���һ��DS�ķ�����������λΪ�ֽڡ�
     *     <last_rx_flow>: 0x0000_0000_0000_0000��0xFFFF_FFFF_FFFF_FFFF��16λ16����������ʾ���һ��DS�Ľ�����������λΪ�ֽڡ�
     *     <total_ds_time>: 0x0000_0000��0xFFFF_FFFF��8λ16����������ʾDS�ۼƵ�����ʱ�䣬��λΪ�롣
     *     <total_tx_flow>: 0x0000_0000_0000_0000��0xFFFF_FFFF_FFFF_FFFF��16λ16����������ʾDS�ۼƵķ�����������λΪ�ֽڡ�
     *     <total_rx_flow>: 0x0000_0000_0000_0000��0xFFFF_FFFF_FFFF_FFFF��16λ16����������ʾDS�ۼƵĽ�����������λΪ�ֽڡ�
     * [ʾ��]:
     *     �� ��ѯDS���һ���Լ��ۼƵ�����ʱ�������
     *       AT^DSFLOWQRY
     *       ^DSFLOWQRY: 0000002D,0000000000019A01,0000000000736A52,0000002D,0000000000019A01,0000000000736A52
     *       OK
     *     �� ��ѯָ��CID��DS���һ���Լ��ۼƵ�����ʱ�������
     *       AT^DSFLOWQRY=1
     *       ^DSFLOWQRY: 0000003A,0000000000015863,0000000000834B41,0000003A, 0000000000015863,0000000000834B41
     *       OK
     */
    { AT_CMD_DSFLOWQRY,
      AT_SetDsFlowQryPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DSFLOWQRY", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: �ϱ�DS����
     * [˵��]: ��MT����online_data״̬ʱ���������ϱ���Ϣ2s�ϱ�һ�Σ��͹�����CPU˯��̬���������ping���������ϱ�ʱ�������ܴ���2s���ϱ������ݰ�����ǰ���DS������ʱ�䡢��ǰ�ķ������ʡ���ǰ�Ľ������ʡ���ǰ���DS�ķ�����������ǰ���DS�Ľ����������������Э�̺�ȷ����PDP���ӷ������ʺ��������Э�̺�ȷ����PDP���ӽ������ʡ�
     *         ��FEATURE_MBB_CUST��򿪣�Ĭ���ϱ�NV 2590 ��CID�б�����������ر�ʱ���ϱ��������ӵ���������
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^DSFLOWRPT: <curr_ds_time>,<tx_rate>,<rx_rate>,<curr_tx_flow>,<curr_rx_flow>, <qos_tx_rate>, <qos_rx_rate><CR><LF>
     *     [����]: ^DSFLOWRPT=<oper>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^DSFLOWRPT=?
     *     [���]: <CR><LF>^DSFLOWRPT: (list of supported <oper>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <curr_ds_time>: 0x0000_0000��0xFFFF_FFFF��8λ16����������ʾ��ǰ���DS������ʱ�䣬��λΪ�롣
     *     <tx_rate>: 0x0000_0000��0xFFFF_FFFF��8λ16����������ʾ��ǰ�ķ������ʣ���λΪ�ֽ�ÿ�롣
     *     <rx_rate>: 0x0000_0000��0xFFFF_FFFF��8λ16����������ʾ��ǰ�Ľ������ʣ���λΪ�ֽ�ÿ�롣
     *     <curr_tx_flow>: 0x0000_0000_0000_0000��0xFFFF_FFFF_FFFF_FFFF��16λ16����������ʾ��ǰ���DS�ķ�����������λΪ�ֽڡ�
     *     <curr_rx_flow>: 0x0000_0000_0000_0000��0xFFFF_FFFF_FFFF_FFFF��16λ16����������ʾ��ǰ���DS�Ľ�����������λΪ�ֽڡ�
     *     <qos_tx_rate>: 0x0000_0000��0xFFFF_FFFF��8λ16���������������Э�̺�ȷ����PDP���ӷ������ʣ���λΪ�ֽ�ÿ�롣
     *     <qos_rx_rate>: 0x0000_0000��0xFFFF_FFFF��8λ16���������������Э�̺�ȷ����PDP���ӽ������ʣ���λΪ�ֽ�ÿ�롣
     *     <oper>: ����ֵ�������ϱ����ء�
     *             0�������Զ������ϱ���
     *             1��ʹ���Զ������ϱ���
     * [ʾ��]:
     *     �� �����ϱ�
     *       AT^DSFLOWRPT=1
     *       OK
     *     �� ��������
     *       AT^DSFLOWRPT=?
     *       ^DSFLOWRPT: (0,1)
     *       OK
     */
    { AT_CMD_DSFLOWRPT,
      AT_SetDsFlowRptPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DSFLOWRPT", (VOS_UINT8 *)"(0,1)" },

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
#if (FEATURE_UE_MODE_G == FEATURE_ON)
    /* ����GCF�������� */
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����GCF��������
     * [˵��]: GCF���ԣ��ڲ������������ӵ�����·������ݡ�
     *         ע����������FEATURE_PHONE_ENG_AT_CMD����ơ�
     * [�﷨]:
     *     [����]: ^TRIG=<mode>,<nsapi>,<length>,<times>,<millisecond>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TRIG=?
     *     [���]: <CR><LF>^TRIG: (list of supported <mode>s),(list of supported <nsapi>s),(list of supported <length>s),(list of supported <times>s),(list of supported <millisecond>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ�����ݴ���LLCģʽ��
     *             0����ȷ��ģʽ��
     *             1��ȷ��ģʽ��
     *     <nsapi>: ����ֵ��ָ�����ݷ��͵�NSAPI��
     *             ȡֵ��ΧΪ5��15��
     *     <length>: ����ֵ��ÿ�����ݷ��ͳ��ȣ���λΪ�ֽڣ�ȡֵ��Χ1��65535��
     *     <times>: ����ֵ���������ݴ�����ȡֵ��ΧΪ1��40��
     *     <millisecond>: ����ֵ��ÿ�����ݷ��ͼ��ʱ�䣬��λΪ���룬ȡֵ��Χ1��10000��
     * [ʾ��]:
     *     �� LLC��ȷ��ģʽ�·���500byte����
     *       AT^TRIG=0,5,500,1,1000
     *       OK
     *     �� LLC��ȷ��ģʽ�·���5000byte����
     *       AT^TRIG=0,5,1000,5,1000
     *       OK
     */
    { AT_CMD_TRIG,
      At_SetTrigPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^TRIG", (VOS_UINT8 *)"(0,1),(5-15),(1-65535),(1-40),(1-10000)" },
#endif
#endif
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��������ͨ��/������CID��ӳ���ϵ
     * [˵��]: �����AP/APP�ڷ���PDP����ǰ��������ͨ����CID��ӳ���ϵ����ӳ���ϵ��PDPȥ������Զ�ע�����ϵ��Ĭ��û��ӳ���ϵ���µ��ӳ���ϵҲʧЧ��
     *         ���������ֻ��<cid>һ����������AT^CHDATA=<cid>����ʾɾ����<cid>��Ӧ������ͨ��ӳ���ϵ��
     *         ָ��<cid>�Ѿ�����󣬲�����ɾ�����޸Ĵ�<cid>��Ӧ������ͨ��ӳ���ϵ��ֱ�ӷ���ERROR��
     *         ��������ͬ��<cid>ӳ�䵽ͬһ��<datachannel>�ϡ�
     * [�﷨]:
     *     [����]: ^CHDATA=<cid>,<datachannel>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CHDATA?
     *     [���]: <CR><LF>^CHDATA: <cid>,<datachannel><CR><LF>
     *             [[<CR><LF>^CHDATA: <cid>,<datachannel><CR><LF>]����]
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CHDATA=?
     *     [���]: <CR><LF>^CHDATA: (list of supported <cid>s), (list of supported <datachannel>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP �����ı�ʶ����
     *             GU�汾��
     *             ȡֵ��ΧΪ1��11��
     *             GUL/GUTL�汾��
     *             ȡֵ��ΧΪ1��31��
     *     <datachannel>: ����ֵ������ͨ��������ID��
     *             ����ģʽ��ȡֵ��ΧΪ1��8��
     *             ˫��ģʽ��ȡֵ��ΧΪ1~10��
     *             ����ģʽ��ȡֵ��ΧΪ1~12��
     * [ʾ��]:
     *     �� �������������ͨ��/����2��CID 1ӳ��
     *       AT^CHDATA=1,2
     *       OK
     *     �� ��ѯ��������Ѿ�������ͨ��/����2��CID 1ӳ��
     *       AT^CHDATA?
     *       ^CHDATA: 1,2
     *       OK
     *     �� ��������(GUģ��˫���²��ԣ�����APPʹ�õ�ͨ����HSIC MUX��VCOM��֧��)
     *       AT^CHDATA=?
     *       ^CHDATA: (1-11),(1-5)
     *       OK
     */
    { AT_CMD_CHDATA,
      AT_SetChdataPara, AT_NOT_SET_TIME, AT_QryChdataPara, AT_NOT_SET_TIME, AT_TestChdataPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CHDATA", (VOS_UINT8 *)CHDATA_TEST_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��ѯ����״̬
     * [˵��]: ��ѯ���Ź����е�ǰ������״̬��
     *         ָ��CID ��ѯ����״̬ʱ��ֻ����ʹ��^NDISDUP ������PDP����ָ��CID δ����򷵻�ERROR��
     *         MBB��֧��ָ��CID��ѯ��ָ��CID��ѯ���ṩ���ֻ���Ʒʹ�õġ�
     * [�﷨]:
     *     [����]: ^APCONNST=<cid>
     *     [���]: <CR><LF>^APCONNST: <status>,<IPV4>,<status>,<IPV6>[,<status>,<Ethernet>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^APCONNST?
     *     [���]: (list of <CR><LF>^APCONNST: <cid>,<status>,<IPV4>,<status>,<IPV6>[,<status>,<Ethernet>]<CR><LF>)<CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^APCONNST=?
     *     [���]: <CR><LF>^APCONNST: (list of supported <cid>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: cid��Ϣ
     *     <status>: ����״̬��
     *             0�����ڲ���״̬��
     *             1��������ɣ��������ӣ�
     *             2���Ͽ�������ɡ�
     *     <pdp_type>: ȡֵΪ�ַ�����
     *             "IPV4"
     *             "IPV6"
     *             "Ethernet"
     * [ʾ��]:
     *       ���ųɹ���ָ�� CID ��ѯ����״̬
     *     �� Ipv4
     *       AT^APCONNST=1
     *       ^APCONNST: 1,"IPV4"
     *       OK
     *     �� Ipv6
     *       AT^APCONNST=1
     *       ^APCONNST: 1,"IPV6"
     *       OK
     *     �� Ipv4v6
     *       AT^APCONNST=1
     *       ^APCONNST: 1,"IPV4",1,"IPV6"
     *       OK
     *     �� ��̫������
     *       AT^APCONNST=1
     *       ^APCONNST: 2,"IPV4",2,"IPV6",1,��Ethernet��
     *       OK
     *     �� ���ųɹ�����ѯ����״̬
     *       AT^APCONNST?
     *       ^APCONNST: 1,1,"IPV4",1,"IPV6"
     *       OK
     */
    { AT_CMD_APCONNST,
      At_SetApConnStPara, AT_SET_PARA_TIME, At_QryApConnStPara, AT_QRY_PARA_TIME, AT_TestApConnStPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^APCONNST", (VOS_UINT8 *)"(1-11)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: �����û���������
     * [˵��]: ���ر���һ����<cid>Ϊ�������û�������ȣ�ÿһ����������û�������һ��������Э����صĲ�����
     *         SET�������Э���һ�����������<cid>Ϊ���������ݴ洢���С�ÿ�����ݴ洢���ʼ����δ����ģ�ͨ��SET�������һ����������Ϊ�Ѷ���״̬��<cid>��ȡֵ��Χ�������ܱ�����Ѷ�������ݴ洢�����Ŀ��
     *         �����SET����^AUTHDATA=<cid>���<cid>ָʾ�Ĵ洢��������ʱ���ݴ洢�鷵��δ����״̬��
     *         READ����������Ѷ���Ĳ���ֵ����������Э��֮�任����ʾ��
     *         TEST���������֧�ֵ�ȡֵ������֮�任����ʾ��
     * [�﷨]:
     *     [����]: ^AUTHDATA=<cid>[,<Auth_type>[<PLMN>[,<passwd>[,<username>]]]
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^AUTHDATA?
     *     [���]: <CR><LF>^AUTHDATA: <cid>, <Auth_type>, <passwd>,<username>,<PLMN> [<CR><LF>^AUTHDATA: <cid>, <Auth_type>, <passwd>,<username>,<PLMN>[...]]<CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^AUTHDATA=?
     *     [���]: <CR><LF>^AUTHDATA: (list of supported <cid>s), (list of supported <Auth_type>s),,<CR><LF><CR><LF>OK<CR><LF>
     *     [����]: ^AUTHDATA
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             LTE�汾ȡֵ��Χ0~31��12~31�ݲ�֧�֡�
     *             �����汾ȡֵ��ΧΪ1��11��
     *     <Auth_type>: ����ֵ������Э�飬ȡֵ���£�
     *             0����ʹ������Э�飻
     *             1��PAP��
     *             2��CHAP��
     *     <PLMN>: �ַ������ͣ���Ӫ�̵�PLMN��0��6byte��
     *     <passwd>: �ַ������ͣ����룬0��99byte��
     *     <username>: �ַ������ͣ��û�����0��99byte��
     * [ʾ��]:
     *     �� �����û���������
     *       AT^AUTHDATA=1,1,"46000","password","username"
     *       OK
     *     �� ��ѯ����
     *       AT^AUTHDATA?
     *       ^AUTHDATA: 1,1, "password","username","46000"
     *       OK
     *     �� ��������
     *       AT^AUTHDATA=?
     *       ^AUTHDATA: (0-31),(0-2),,
     *       OK
     */
    { AT_CMD_AUTHDATA,
      At_SetAuthdataPara, AT_SET_PARA_TIME, AT_QryAuthdataPara, AT_QRY_PARA_TIME, At_TestAuhtdata, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^AUTHDATA", (VOS_UINT8 *)AUTHDATA_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����ҵ��������䵥Ԫ
     * [˵��]: ����ҵ�񼤻�֮�󣬲�ѯ��������������䵥Ԫ����MTUֵ��Maximum Transmission Unit����������֧�ֲ�ѯ��
     * [�﷨]:
     *     [����]: ^CGMTU=<CID>
     *     [���]: <CR><LF>^CGMTU: <CID>,<IPV4_MTU>,<IPV6_MTU>[,<non-IP_MTU>]<CR><LF><CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CGMTU=?
     *     [���]: <CR><LF>^CGMTU: (0-31)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <CID>: ����ֵ��PDP�����ı�ʶ����ȡֵ��Χ0~31��
     *     <IPV4_MTU>: ����ֵ��IPV4����������Э�̵�����䵥Ԫ������û������0��
     *     <IPV6_MTU>: ����ֵ��IPV6����������Э�̵�����䵥Ԫ������û������0��
     *     <non-IP_MTU>: ����ֵ��Ethernet��non-IP����UnstructuredЭ����������������Э�̵�����䵥Ԫ������û������0��
     * [ʾ��]:
     *     �� ��ѯCid 1�� MTUֵ
     *       AT^CGMTU=1
     *       ^CGMTU: 1,1500,1500
     *       OK
     *     �� ��������
     *       AT^CGMTU=?
     *       ^CGMTU: (0-31)
     *       OK
     */
    { AT_CMD_CGMTU,
      AT_SetCgmtuPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^CGMTU", (VOS_UINT8 *)"(1-11)" },

    /* �������MBB��Ʒ������ֲ����������MBB���ƴ��� */
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��������״̬��ѯ����
     * [˵��]: ����^DCONNSTAT?���ڲ�ѯmodem ������м���cid����������״̬�������ӵ�cid����APN��IPv4����IPv6��Ethernet���� ������״̬�����ӵ����ͣ�δ���ӵĽ�����cid��^DCONNSTAT=?���ڲ�ѯmodem �����м���cid�д�������״̬�����������С������ӡ��Ͽ���״̬����cid��
     * [�﷨]:
     *     [����]: ^DCONNSTAT?
     *     [���]: <CR><LF>^DCONNSTAT: <cid>[,<APN>,<ipv4_stat>,<ipv6_stat>,<dconn_type>[,<ether_stat>]][<CR><LF>^DCONNSTAT:<cid>[,<APN>,<ipv4_stat>,<ipv6_stat>,<dconn_type>[,<ether_stat>] ][��]<CR><LF><CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^DCONNSTAT=?
     *     [���]: <CR><LF>^DCONNSTAT: (list of connected<cid>s)<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: cid��ֵ
     *     <APN>: �ַ���ֵ����ʾ�����������֣���ǰCID��������״̬�ŷ��ء�
     *     <ipv4_stat>: IPv4���͵�����״̬����ǰCID��������״̬�ŷ��أ�ȡֵ�������£�
     *             0�����ӶϿ�
     *             1��������
     *     <ipv6_stat>: IPv6���͵�����״̬����ǰCID��������״̬�ŷ��أ�ȡֵ�������£�
     *             0�����ӶϿ�
     *             1��������
     *     <dconn_type>: ��ʾ��ǰ�����ӵ��������ͣ���ǰCID��������״̬ʱ�ŷ��أ�
     *             1��APP����
     *             2��NDIS����
     *             3�����������͵Ĳ���
     *     <ether_stat>: Ethernet���͵�����״̬����ǰCID��������״̬�ŷ��أ�ȡֵ�������£�
     *             0�����ӶϿ�
     *             1��������
     * [ʾ��]:
     *     �� ��ѯ���м���cid����������״̬
     *       AT^DCONNSTAT?
     *       ^DCONNSTAT: 1,��Huawei.com��,1,1,1
     *       ^DCONNSTAT: 2,��Huawei1.com��,0,0,1,1
     *       ^DCONNSTAT: 3
     *       ^DCONNSTAT: 4
     *       ^DCONNSTAT: 5
     *       ^DCONNSTAT: 6
     *       ^DCONNSTAT: 7
     *       ^DCONNSTAT: 8
     *       ^DCONNSTAT: 9
     *       ^DCONNSTAT: 10
     *       ^DCONNSTAT: 11
     *       OK
     */
    { AT_CMD_DCONNSTAT,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryDconnStatPara, AT_QRY_PARA_TIME, AT_TestDconnStatPara, AT_TEST_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DCONNSTAT", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ����APN����IP����
     * [˵��]: �������������ò�ͬcid��ӦAPN�����γ�����IP���ͣ���Ҫ����2554����APN����NV�����������ֻ��<cid>һ����������AT^ROAMPDPTYPE =<cid>����ʾɾ����<cid>��Ӧ��IP���͡�
     * [�﷨]:
     *     [����]: ^ROAMPDPTYPE=<cid>,<iptype>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���ô���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP������������ͬ+CGDCONT��Ŀǰֻ֧��0��
     *     <iptype>: IP���ͣ�ֻ��������IP��IPV6��IPV4V6�������ͣ���PPP���ݲ�֧��
     * [ʾ��]:
     *     �� ��������IP����ΪIP
     *       AT^ROAMPDPTYPE=0,"IP"
     *       OK
     */
    { AT_CMD_ROAMPDPTYPE,
      AT_SetRoamPdpTypePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^ROAMPDPTYPE", (VOS_UINT8 *)ROAMPDPTYPE_CMD_PARA_STRING },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: SINGLEPDN���Կ�������
     * [˵��]: ���������ڶ�̬�Ŀ���SINGLEPDN���ԣ���SINGLEPDN����ʱ����8451NV�����޸ģ�ǰ��λ�޸�Ϊ1,0,1,11����Ĭ��ʹ��CID1����ע���ҵ����SINGLEPDN�ر�ʱ�������޸�ʹ��λ��ǰ��λ�޸�Ϊ1,0,0,0����IP type���Ͳ��������޸ķ�Χ�ڡ�
     * [�﷨]:
     *     [����]: ^SINGLEPDNSWITCH=<switch>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT ��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err> <CR><LF>
     *     [����]: ^SINGLEPDNSWITCH?
     *     [���]: <CR><LF>^ SINGLEPDNSWITCH: <switch><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     * [����]:
     *     <switch>: ����ֵ�����Կ���
     *             ȡֵ˵����ȡֵ��ΧΪ0��1
     *             0 �ر�SINGLEPDN����
     *             1 ����SINGLEPDN����
     * [ʾ��]:
     *     �� ����SINGLEPDN���Կ���Ϊ��״̬
     *       AT^SINGLEPDNSWITCH=1
     *       OK
     *     �� ��ѯSINGLEPDN���Կ���״̬
     *       AT^SINGLEPDNSWITCH?
     *       ^ SINGLEPDNSWITCH: 1
     *       OK
     */
    { AT_CMD_SINGLEPDNSWITCH,
      AT_SetSinglePdnSwitch, AT_SET_PARA_TIME, AT_QrySinglePdnSwitch, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SINGLEPDNSWITCH", (VOS_UINT8 *)"(0,1)" },

#if (FEATURE_MBB_CUST == FEATURE_ON)
    { AT_CMD_DSAMBR,
      AT_SetDsambrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DSAMBR", (VOS_UINT8*)"(1)" },
#endif

    /*
     * [���]: Э��AT-NDIS����
     * [����]: ��ѯDHCP��Ϣ
     * [˵��]: ����������PC��ѯDHCP���IPֵ����������IP��ַ��Ĭ�����ء����������DHCP��������
     *         �����������Ժ�PC�·��������Ի�ȡ��Ӧ��IP��ַ�����δ���Ż��߲���ʧ�ܣ���ѯDHCP������ERROR��
     *         ָ��CID��ѯDHCP�����Ϣʱ��ֻ����ʹ��^NDISDUP������PDP����ָ��CIDδ����򷵻�ERROR��
     * [�﷨]:
     *     [����]: ^DHCP=<cid>
     *     [���]: <CR><LF>^DHCP: <clip>,<netmask>,<gate>,<dhcp>,<pDNS>,
     *             <sDNS>,<max_rx_data>,<max_tx_data><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������:<CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^DHCP?
     *     [���]: <CR><LF>^DHCP: <clip>,<netmask>,<gate>,<dhcp>,<pDNS>,<sDNS>,<max_rx_data>,<max_tx_data><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������<CR><LF>ERROR<CR><LF>
     *     [����]: ^DHCP=?
     *     [���]: <CR><LF>^DHCP: (list of supported <cid>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ��ǰPDP�����CID��
     *     <clip>: ����IP��ַ��ȡֵ��Χ0x0000_0000��0xFFFF_FFFF��
     *     <netmask>: �������룬ȡֵ��Χ0x0000_00FF��0xFCFF_FFFF��
     *     <gate>: Ĭ�����أ�ȡֵ��Χ0x0000_0000��0xFFFF_FFFF��
     *     <dhcp>: DHCP server�ĵ�ַ��ȡֵ��Χ0x0000_0000��0xFFFF_FFFF��
     *     <pDNS>: ��ѡDNS�ĵ�ַ��ȡֵ��Χ0x0000_0000��0xFFFF_FFFF��
     *     <sDNS>: ����DNS�ĵ�ַ��ȡֵ��Χ0x0000_0000��0xFFFF_FFFF��
     *     <max_rx_data>: ���������ʣ�bit/s����
     *     <max_tx_data>: ��������ʣ�bit/s����
     * [ʾ��]:
     *     �� ָ��CID��ѯDHCP
     *       AT^DHCP=1
     *       ^DHCP: 16007e0a,fcffffff,15007e0a,15007e0a,6537480a,64ff480a,7200000,384000
     *       OK
     *     �� ��ѯDHCP
     *       AT^DHCP?
     *       ^DHCP: 16007e0a,fcffffff,15007e0a,15007e0a,6537480a,64ff480a,7200000,384000
     *       OK
     *     �� ��������
     *       AT^DHCP=?
     *       ^DHCP: (1-11)
     *       OK
     */
    { AT_CMD_DHCP,
      At_SetDhcpPara, AT_SET_PARA_TIME, At_QryDhcpPara, AT_QRY_PARA_TIME, AT_TestDhcpPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DHCP", (VOS_UINT8 *)"(1-11)" },

    /*
     * [���]: Э��AT-NDIS����
     * [����]: ��ѯ����״̬
     * [˵��]: ��ѯMT��ECM��NDIS/WWAN������״̬��
     * [�﷨]:
     *     [����]: ^NDISSTATQRY?
     *     [���]: (list of <CR><LF>^NDISSTATQRY: <cid>,<stat>[,<err>[,<wx_state>[,<PDP_type>]]] [,<stat>,<err>,<wx_state>,<PDP_type>]<CR><LF>)
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^NDISSTATQRY=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: cid��Ϣ
     *     <stat>: ����ֵ������״̬��
     *             0�����ӶϿ���
     *             1�������ӣ�
     *             2�������У����ڵ����Զ�����ʱ�ϱ����ݲ�֧�֣���
     *             3���Ͽ��У����ڵ����Զ�����ʱ�ϱ����ݲ�֧�֣���
     *     <err>: ����ֵ�������룬���ڲ���ʧ��ʱ�ϱ��ò�����
     *             0��unknow error/ unspecified error��
     *             ����ֵ����ѭ��3GPP TS 24.008 V5.5.0 (2002-09)���������汾��10.5.6.6SM Cause������
     *     <wx_state>: ����ֵ��WiMax���ݿ���״̬���ݲ�֧�֡�
     *             1��DL Synchronization��
     *             2��Handover DL acquisition��
     *             3��UL Acquisition��
     *             4��Ranging��
     *             5��Handover ranging��
     *             6��Capabilities negotiation��
     *             7��Authorization��
     *             8��Registration��
     *     <PDP_type>: ȡֵΪ�ַ�����
     *             "IPV4"
     *             "IPV6"
     *             "Ethernet"
     * [ʾ��]:
     *       ����MT֧�ֵ�IPV6�����ϱ�һ�����������״̬��
     *     �� ���MT��֧��IPV4 Onlyʱ��IPV4����״̬Ϊ������״̬��ֻ�ϱ�һ������״̬��
     *       AT^NDISSTATQRY?
     *       ^NDISSTATQRY: 1,1,,,"IPV4"
     *       OK
     *     �� ���MT֧��IPV4V6˫ջʱ��IPV4����״̬Ϊδ����״̬��IPV6����״̬Ϊ������״̬���ϱ���������״̬�������ǲ���ǰ���ǲ��ź�
     *       AT^NDISSTATQRY?
     *       ^NDISSTATQRY: 1,0,,,"IPV4",1,,,"IPV6"
     *       OK
     *     �� ���ΪEthernet���͵Ĳ���ʱ������״̬Ϊ������״̬��
     *       AT^NDISSTATQRY?
     *       ^NDISSTATQRY: 1,1,,,"Ethernet"
     *       OK
     *     �� ���Դ�����
     *       AT^NDISSTATQRY=?
     *       OK
     */
#if (FEATURE_MBB_CUST == FEATURE_ON)
    /* MBB��Ʒ֧��ͨ��ָ��CID��ѯ����״̬ */
    { AT_CMD_NDISSTATQRY,
      AT_SetNdisStatQryPara, AT_SET_PARA_TIME, AT_QryNdisStatPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8*)"^NDISSTATQRY", (VOS_UINT8*)"(1-11)"},
#else
    { AT_CMD_NDISSTATQRY,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryNdisStatPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NDISSTATQRY", VOS_NULL_PTR },
#endif

    /*
     * [���]: Э��AT-NDIS����
     * [����]: ��ѯDHCPV6��Ϣ
     * [˵��]: ���������ڲ�ѯDHCPv6���IPv6ֵ����������IPv6��ַ��Ĭ�����ء����������DHCPv6 ��������
     *         IPv6�����������Ժ�PC�·��������Ի�ȡ��Ӧ��IPv6��ַ�����δ���Ż��߲���ʧ�ܣ���ѯDHCPv6������ERROR��
     * [�﷨]:
     *     [����]: ^DHCPV6=<cid>
     *     [���]: <CR><LF>(list of ^DHCPV6: <clip_v6>,<netmask_v6>,<gate_v6>,<dhcp_v6>,<pDNS_v6>,<sDNS_v6>,<max_rx_data>,<max_tx_data><CR><LF>)<CR><LF>OK<CR><LF>
     *             �������:<CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^DHCPV6?
     *     [���]: <CR><LF>^DHCPV6: <clip_v6>,<netmask_v6>,<gate_v6>,<dhcp_v6>,<pDNS_v6>,<sDNS_v6>,<max_rx_data>,<max_tx_data><CR><LF><CR><LF>OK<CR><LF>
     *             �������<CR><LF>ERROR<CR><LF>
     *     [����]: ^DHCPV6=?
     *     [���]: <CR><LF>^DHCPV6: (list of supported <cid>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: CID��Ϣ
     *     <clip_v6>: ����IPv6��ַ��
     *     <netmask_v6>: IPv6�������롣
     *     <gate_v6>: IPv6Ĭ�����ء�
     *     <dhcp_v6>: DHCPv6 server�ĵ�ַF��
     *     <pDNS_v6>: ��ѡDNSv6�ĵ�ַ��
     *     <sDNS_v6>: ����DNSv6�ĵ�ַ��
     *     <max_rx_data_v6>: ���������ʣ�bit/s����
     *     <max_tx_data_v6>: ��������ʣ�bit/s����
     * [ʾ��]:
     *     �� ��ѯDHCPV6
     *       AT^DHCPV6?
     *       ^DHCPV6: ::,::,:: ,::, fe80::e1ec:e44a:a28f:aeb1,::,7200000,384000
     *       OK
     *     �� ָ��CID��ѯDHCPV6
     *       AT^DHCPV6=1
     *       ^DHCPV6: ::,::,:: ,::, fe80::e1ec:e44a:a28f:aeb1,::,7200000,384000
     *       OK
     *     �� ��������
     *       AT^DHCPV6=?
     *       ^DHCPV6: (1-11)
     *       OK
     */
    { AT_CMD_DHCPV6,
      AT_SetDhcpv6Para, AT_SET_PARA_TIME, AT_QryDhcpv6Para, AT_QRY_PARA_TIME, AT_TestDhcpv6Para, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^DHCPV6", (VOS_UINT8 *)"(1-11)" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ѯ���һ��PS����д�����
     * [˵��]: ���������ڲ�ѯ���һ��PS����д���ԭ�򣬰���PS�����ʧ�ܺ������쳣�Ͽ�PS����С�
     * [�﷨]:
     *     [����]: ^CPSERR?
     *     [���]: <CR><LF>^CPSERR: <cause><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CPSERR=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <cause>: ����ֵ��������
     *             0~65535�������������
     * [ʾ��]:
     *     �� PS�����ʧ�ܣ���ѯʧ��ԭ��
     *       AT^CPSERR?
     *       ^CPSERR: 1
     *       OK
     *     �� ��������
     *       AT^CPSERR=?
     *       OK
     */
    { AT_CMD_CPSERR_SET,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryCPsErrPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CPSERR", VOS_NULL_PTR },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���������������£����ٲ��RRC����
     * [˵��]: �����羫��Խ�ʱ�������������������£�AP��ҪMODEM֧�ֿ��ٲ��RRC���ӡ�
     *         CLģʽ�¸����֧�֣�����W������ʽ��ʹ�ã�����������ʽ���ظ�error��
     * [�﷨]:
     *     [����]: ^RELEASERRC
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^RELEASERRC?
     *     [���]: <CR><LF>ERROR<CR><LF>
     *     [����]: ^RELEASERRC=?
     *     [���]: <CR><LF>ERROR<CR><LF>
     * [����]:
     * [ʾ��]:
     *     �� ��������������ʱ�����RRC����
     *       AT^RELEASERRC
     *       OK
     */
    { AT_CMD_RELEASERRC,
      At_SetReleaseRrc, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^RELEASERRC", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯIPv6Route����
     * [˵��]: ����������Ӧ�ò�ѯIPv6 RA��Ϣ����ز���ȡֵ������MTU��ǰ׺��ǰ׺��preferred lifetime��Valid lifetime��
     *         IPv6�����������Ժ�Ӧ���·��������Ի�ȡ��Ӧ��RA��Ϣ������������RADVD�����δ���Ż��߲���ʧ�ܣ���ѯAPRAINFO������ERROR��
     *         ָ��CID��ѯIPv6Route������ֻ����ʹ��^NDISDUP������PDP����ָ��CIDδ����򷵻�ERROR��
     * [�﷨]:
     *     [����]: ^APRAINFO=<cid>
     *     [���]: <CR><LF>^APRAINFO: <APIPV6MTU>,<APPREFIX>,<APPreferredLifetime>,<APValidLifetime><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^APRAINFO?
     *     [���]: (list of <CR><LF>^APRAINFO: <cid>,<APIPV6MTU>,<APPREFIX>,<APPreferredLifetime>,<APValidLifetime><CR><LF>)< CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^APRAINFO=?
     *     [���]: <CR><LF>^APRAINFO:  (list of supported <cid>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: cid��Ϣ
     *     <APIPV6MTU>: ʮ�������֣�������RA��Ϣ�й㲥��IPv6��MTU��ȡֵ��
     *     <APPREFIX>: IPV6ǰ׺����ʽΪ�������������ַ�������XXX/YY����XXXΪRFC5952�淶��ʽ��Ipv6��ַ��YYΪǰ׺���ȣ���400::/64���ַ������Ȳ�����
     *     <APPreferredLifetime>: ǰ׺��Preferred lifetimeȡֵ��ʮ�������֡�
     *     <APValidLifetime>: ǰ׺��Valid lifetimeȡֵ��ʮ�������֡�
     * [ʾ��]:
     *     �� ָ��CID����ѯ^APRAINFO
     *       AT^APRAINFO=1
     *       ^APRAINFO: 1280,"400::/64",300,1000
     *       OK
     *     �� ��ѯ^APRAINFO
     *       AT^APRAINFO?
     *       ^APRAINFO: 1,1280,"400::/64",300,1000
     *       OK
     *     �� ��������
     *       AT^APRAINFO=?
     *       ^APRAINFO: (1-11)
     *       OK
     */
    { AT_CMD_APRAINFO,
      AT_SetApRaInfoPara, AT_SET_PARA_TIME, AT_QryApRaInfoPara, AT_QRY_PARA_TIME, AT_TestApRaInfoPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^APRAINFO", (VOS_UINT8 *)"(1-11)" },

    { AT_CMD_USBTETHERINFO,
      At_SetUsbTetherInfo, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestUsbTetherInfo, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^USBTETHERINFO", (VOS_UINT8 *)"(0,1),(rnmet_name)" },

    { AT_CMD_RMNETCFG,
      AT_SetRmnetCfg, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^RMNETCFG", (VOS_UINT8 *)RMNETCFG_CMD_PARA_STRING },

    { AT_CMD_IPV6TEMPADDR,
      AT_SetIPv6TempAddrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestIPv6TempAddrPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^IPV6TEMPADDR", (VOS_UINT8 *)"(1-11)" },

    { AT_CMD_APDIALMODE,
      VOS_NULL_PTR, AT_SET_PARA_TIME, At_QryApDialModePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^APDIALMODE", (VOS_UINT8 *)"(0-3),(0-2),(30-2592000)" },

    { AT_CMD_OPWORD,
      AT_SetOpwordPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^OPWORD", VOS_NULL_PTR },

    { AT_CMD_CPWORD,
      AT_SetCpwordPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CPWORD", VOS_NULL_PTR },

    { AT_CMD_DISLOG,
      AT_SetDislogPara, AT_NOT_SET_TIME, AT_QryDislogPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^DISLOG", (VOS_UINT8 *)"(0,1,255)" },

#if (FEATURE_SECURITY_SHELL == FEATURE_ON)
    { AT_CMD_SHELL,
      AT_SetShellPara, AT_NOT_SET_TIME, AT_QryShellPara, AT_NOT_SET_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^SHELL", (VOS_UINT8 *)"(0-2)" },
#endif

#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
    { AT_CMD_CSND,
      At_SetCsndPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CSND", (VOS_UINT8 *)"(0-34),(1-2048)" },
#else
    { AT_CMD_CSND,
      At_SetCsndPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^CSND", (VOS_UINT8 *)"(5-15),(1-2048)" },
#endif

    { AT_CMD_DWINS,
      AT_SetDwinsPara, AT_SET_PARA_TIME, At_QryDwinsPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_CLAC_IS_INVISIBLE,
      (VOS_UINT8 *)"^DWINS", (VOS_UINT8 *)"(0,1)" },

    { AT_CMD_APDSFLOWRPTCFG,
      AT_SetApDsFlowRptCfgPara, AT_SET_PARA_TIME, AT_QryApDsFlowRptCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^APDSFLOWRPTCFG", (VOS_UINT8 *)"(0,1),(0-4294967295)" },

    { AT_CMD_DSFLOWNVWRCFG,
      AT_SetDsFlowNvWriteCfgPara, AT_SET_PARA_TIME, AT_QryDsFlowNvWriteCfgPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^DSFLOWNVWRCFG", (VOS_UINT8 *)"(0,1),(0-255)" },

#if (FEATURE_HUAWEI_VP == FEATURE_ON)
    { AT_CMD_VOICEPREFER,
      AT_SetVoicePreferPara, AT_SET_PARA_TIME, AT_QryVoicePreferPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VOICEPREFER", (VOS_UINT8 *)"(0-4294967295)" },
#endif

#if (FEATURE_MBB_CUST == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: LTE ATTACH PROFILE LIST����PROFILE���úͲ�ѯ
     * [˵��]: ����������LTE ATTACH PROFILE LIST���ܡ�
     *         �����������ڲ�ѯ������Χ������INDEXΪʵ����Чindex���ܸ�����NV��Ч+TABLE����Ч��
     *         ���������������ģʽ�Ĳ�ͬ��Ϊ��ȡPROFILE������д��PROFILE����2�����ܣ�
     *         д�������֧��PROFILE 0���޸ĺ���ӣ�����ֻ������ʱ��ʾ���profile��Ϣ��
     *         ��ȡ����ɸ��ݴ����INDEX����Ԥ�Ƶ���ЧPROFILE��Ϣ��INDEX�����򷵻�ERROR��
     *         ��FEATURE_MBB_CUST���ʱ��������Ч��
     * [�﷨]:
     *     [����]: AT^LTEPROFILE=<mode>,<index>[,<PDP_type>,<IMSI_prefix>,<APN>,<username>,<passwd>,<Auth_type>,<profilename>]
     *     [���]: д��ģʽʱ��
     *             <CR><LF>OK<CR><LF>
     *             ��ȡģʽʱ��
     *             <CR><LF>^LTEPROFILE: <index>,<PDP_type>,<IMSI_prefix>,<APN>,<UserName>,<UserPwd>,<Auth_type>,<profilename><CR><LF>OK<CR><LF>
     *             ��MT��صĴ���
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *             ����������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: AT^LTEPROFILE=?
     *     [���]: <CR><LF>^LTEPROFILE: (range of supported <mode>S),(range of supported <index>s),<MaxImsiPrefixLen>,<MaxApnLen>,<MaxUserNameLen>,<MaxUserPwdLen>,(range of supported <AuthType>s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ����ֵ
     *             ����ģʽ��
     *             0����ȡģʽ����ȡģʽʱ��ֻ��������������
     *             1��д��ģʽ��Ϊд��APN��Ϣ��
     *             ��֧��д��<index>Ϊ0��PROFILE��Ϣ���Ա����´�ע��ʱ����ƥ�䡣д��ʱ�����ֻ��������������ʾɾ��ԭ�������ݡ�д���ɾ����PROFILE���´�ע��ʱ��Ч��
     *     <index>: ����ֵ������APN��������š�
     *             <index>Ϊ0ר���ڲ�ѯд���APN��Ϣ�����������ڲ�ѯ���Ƶ�APN��
     *     <PDP_type>: �ַ������ͣ����Ƶ�ע��IP���ͣ�
     *             ��IP��        IPV4����
     *             ��IPV6��      IPV6����
     *             ��IPV4V6��     IPV4V6˫ջ����
     *     <IMSI_prefix>: �ַ������ͣ���Ҫƥ���SIM��IMSIǰ׺����Ч����Ϊ0-10��
     *     <APN>: �ַ������ͣ�ע��ʱ��������ƣ���Ч����Ϊ1-32��
     *     <username>: �ַ������ͣ�ע��ʱ������Ӧ�ļ�Ȩ�û�������Ч����Ϊ0-32��
     *     <passwd>: �ַ������ͣ�ע��ʱ������Ӧ�ļ�Ȩ���룬��Ч����Ϊ0-32��
     *     <Auth_type>: ����ֵ����Ȩ���ͣ�
     *             0��None����(�û������붼û��ʱ�ſ�����)
     *             1��PAP��Ȩ����
     *             2��CHAP��Ȩ����
     *     <profilename>: ����Profile�����ƣ�д�뱣����Ч�ֽڳ���Ϊ0-18��
     *             �����������У�ʹ�ó������ַ��ᱻ�ضϡ�ʹ�õ���UTF8������HEX��ʽ���硱abc������AT�����в�ѯ���Ϊ616263������Ϊ3�������ġ���ѯ���ΪE4B8ADE69687��ʵ���ֽڳ���Ϊ6��
     * [ʾ��]:
     *     �� ��ѯ������Χ
     *       AT^LTEPROFILE=?
     *       ^LTEPROFILE: (0,1),(0-10),10,32,32,32,(0-3)
     *       OK
     *     �� д���û����õ�PROFILE��Ϣ
     *       AT^LTEPROFILE=1,0,"IPV4V6",46050,"v4v6","","",0,E4B8ADE69687
     *       OK
     *     �� ��ȡ���Ƶĵ�5��PROFILE��Ϣ
     *       AT^LTEPROFILE=0,5
     *       ^LTEPROFILE: 5, "IPV6","46050","ipv6","","",0, E4B8ADE69687
     *       OK
     */
    { AT_CMD_LTEPROFILE,
    AT_SetCustProfilePara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestCustProfilePara, AT_TEST_PARA_TIME,
    VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
    (VOS_UINT8 *)"^LTEPROFILE", (VOS_UINT8 *)"(0,1),(0-65535),(\"IP\",\"IPV6\",\"IPV4V6\"),(@ImsiPrefix),(APN),(UserName),(UserPwd),(0-3),(@profileName)" },

    /*
     * [���]: Э��AT-LTE���
     * [����]: LTE ATTACH PROFILE LIST �����������ѯ����
     * [˵��]: ����������LTE ATTACH PROFILE LIST���ܣ������������ڿ���/�ر����ԣ���ѯ�������ڲ�ѯ���Կ��غ���ѵ״̬,��FEATURE_MBB_CUST���ʱ��������Ч��
     * [�﷨]:
     *     [����]: ^LTEAPNATTACH=<switch>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err> <CR><LF>
     *     [����]: ^LTEAPNATTACH?
     *     [���]: <CR><LF>^LTEAPNATTACH: <switch>,<retry_status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <switch>: ����ֵ�����Կ��أ�Ĭ�ϳ�ʼֵΪ0
     *             ȡֵ˵����ȡֵ��ΧΪ0��1��
     *             0��LTE ATTACH PROFILE LIST����δʹ�ܡ�
     *             1��LTE ATTACH PROFILE LIST����ʹ�ܡ�
     *     <retry_status>: ����ֵ��ȡֵ��ΧΪ0��1��
     *             0����ѯ�ѽ�����
     *             1��������ѯ�С�
     *             ��ע:<retry_status>״̬����û��ע��ʱ��Ч������Ѿ�ע���������ò�����Ч��AP����û��ע����LTEʱ�·���ѯ�������״̬��ѯ��
     * [ʾ��]:
     *     �� ����LTE ATTACH PROFILE LIST���ܿ���
     *       AT^LTEAPNATTACH=1
     *       OK
     *     �� ����LTE ATTACH PROFILE LIST���ܹر�
     *       AT^LTEAPNATTACH=0
     *       OK
     */
    { AT_CMD_LTEAPNATTACH,
    AT_SetCustProfileAttachSwitch, AT_SET_PARA_TIME, AT_QryCustProfileAttachSwitch, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
    VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
    (VOS_UINT8 *)"^LTEAPNATTACH", (VOS_UINT8 *)"(0,1)" },
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: ����UE Policy����
     * [˵��]: ����������AP�����յ�������·���UE POLICY ��Ϣ�󣬶�������Ӧ��ظ����Լ�����AP�ѱ��ر����UPSI��UE�Ƿ�֧��ANDSP��UE��OS ID�Ȳ����ϴ������硣
     *         ע�⣺
     *         ��message_typeΪMANAGE UE POLICY COMMAND REJECT����UE state indicationʱ��<UE_policy_information_length>����Ҫ�����������ַ��������ĳ�����ͬ��
     *         ������^CSUEPOLICY ����֮��ſ�������UE_policy_information��Ϣ�������ڷ����ŵĴ�������UE_policy_information��Ϣ��������ctrl-z��β��
     *         ��UE_policy_information��Ϣ����1500�ֽڣ���Ҫ�ֶ��·���ÿ������ʹ��CR���س�������β���ܳ��Ȳ�����4000�ֽڡ�
     * [�﷨]:
     *     [����]: ^CSUEPOLICY=<pti>,<message_type>
     *             [,<UE_policy_information_length>[,<UE_policy_classmark>[,<UE_OS_id>]]],[<protocol_ver>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CSUEPOLICY=?
     *     [���]: <CR><LF>^CSUEPOLICY: (list of supported < pti >s), (list of supported < message_type >s), (list of supported < UE_policy_information_length >s),(< PolicyClassMarkStr >),(< osIdStr >),(list of supported < protocol_ver >s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <pti>: ����ֵ��ȡֵ��Χ��0-254��
     *             0������ UE STATE INDICATION���͵���Ϣ
     *             1-254������MANAGE UE POLICY COMPLETE ��MANAGE UE POLICY COMMAND REJECT
     *     <message_type>: ����ֵ����Ϣ����
     *             0��MANAGE UE POLICY COMPLETE
     *             1��MANAGE UE POLICY COMMAND REJECT
     *             2��UE STATE INDICATION
     *     <UE_policy_information_length>: ����ֵ��UE Policy Information�ַ������ȣ�ȡֵ��Χ��0-4000��
     *             UE Policy Information������
     *             ���������ݲ���Э���½ڣ�
     *             3GPP TS 24.501 [161] subclause D.6.3, D.6.4
     *     <UE_policy_classmark>: �ַ������ͣ����8���ַ�
     *             ���������ݲ���Э���½ڣ�
     *             3GPP TS 24.501 [161] subclause D.6.5
     *     <UE_OS_id>: �ַ������ͣ����480���ַ�
     *             ���������ݲ���Э���½ڣ�
     *             3GPP TS 24.501 [161] subclause D.6.6
     *     <protocol_ver>: ����ֵ��ȡֵ���£�1510��1520��1530
     * [ʾ��]:
     *     �� AP�·�MANAGE UE POLICY COMPLETE��Ϣ
     *       AT^CSUEPOLICY=1,0
     *       OK
     *     �� AP�·�MANAGE UE POLICY COMMAND REJECT��Ϣ
     *       AT^CSUEPOLICY=1,1,22
     *       >000901261030000100016f
     *       >ctrl-z
     *       OK
     *     �� AP�·�UE STATE INDICATION��Ϣ
     *       AT^CSUEPOLICY=1,2,22,"00","0123456789abcdef",1520
     *       >000901261030000100016f
     *       >ctrl-z
     *       OK
     *     �� AP�·�UE STATE INDICATION��Ϣ����Я���κ���Ϣ
     *       AT^CSUEPOLICY=1,2
     *       OK
     */
    { AT_CMD_CSUEPOLICY,
      AT_SetCsUePolicy, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CSUEPOLICY", (VOS_UINT8 *)"(0-254),(0,1,2),(0-4000),(PolicyClassMarkStr),(osIdStr),(1510,1520,1530)" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��������ҵ��������5G�¿��Գ��Ե�������Դ���
     * [˵��]: ��������ҵ��������Գ��Ե���󴫵ݴ�����Ŀǰ������5Gģʽ�µļ���ҵ�������У�
     *         �������Կ�����չ���������̣���������ģʽ��Ŀǰ����5G�¼���������֧�֣��������̺���ʽ��֧�֣�������������򿪡�
     * [�﷨]:
     *     [����]: ^CGCONTEX=<cid>[,<process>,<times>]
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^CGCONTEX=?
     *     [���]: <CR><LF>^CGCONTEX: (list of <cid>s associated with configured contexts) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cid>: ����ֵ��PDP�����ı�ʶ����
     *             ȡֵ��Χ1~31��
     *     <process>: ����ֵ��
     *             1: ��������;
     *             2: modify����;
     *             3: ȥ�������̡�
     *     Ŀǰ����5G�¼���������֧�֣��������̺���ʽ��֧�֣��������������
     *     <times>: ����ֵ�����������ȡֵ��Χ1~5��
     * [ʾ��]:
     *     �� ��������
     *       AT^CGCONTEX=1,1,5
     *       OK
     *     �� ��������
     *       AT^CGCONTEX=?
     *       ^CGCONTEX: (1-31),(1-3),(1-5)
     *       OK
     */
    { AT_CMD_CGCONTEX,
      AT_SetCgcontexPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, At_TestCgcontexPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CGCONTEX", (VOS_UINT8 *)"(1-31),(1-3),(1-5)" },

#endif

#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: IMS��������֪ͨ
     * [˵��]: ����������AP�Ķ�λҵ��SUPL��modem���͸�AT���֪ͨmodem����IMS�������أ�Ȼ���ڽ��������Ϸ��Ͷ�λ���ݣ�SUPLҵ����������͸�AT����֪ͨmodemȥ����IMS�������ء�
     * [�﷨]:
     *     [����]: ^IMSEMCSTAT=<Option>
     *     [���]: <CR><LF>^IMSEMCSTAT: <Option><Status><CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <Option>: ����ֵ��IMS�������ز�����
     *             0��ȥ����IMS�������أ�
     *             1������IMS�������أ�
     *     <Status>: ����ֵ��IMS��������״̬��
     *             0��IMS��������ȥ���
     *             1��IMS�������ؼ��
     *             2��IMS��������HOLD��
     * [ʾ��]:
     *     �� IMS�������ؼ���
     *       AT^IMSEMCSTAT=1
     *       ^IMSEMCSTAT: 1
     *       OK
     *     �� IMS��������ȥ����ɹ�
     *       AT^IMSEMCSTAT=0
     *       ^IMSEMCSTAT: 0
     *       OK
     *     �� IMS��������ȥ����ʧ��
     *       AT^IMSEMCSTAT=0
     *       ^IMSEMCSTAT: 2
     *       OK
     */
    { AT_CMD_EMC_STATUS,
      AT_SetEmcStatus, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^IMSEMCSTAT", (VOS_UINT8 *)"(0,1)" },
#endif

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ��������ҵ������״̬�����ϱ�����
     * [˵��]: ������������������ҵ������״̬�����ϱ����������Բ�ͬ������̬���е������ƣ�Ŀǰֻ֧��STICK��̬���š�
     * [�﷨]:
     *     [����]: ^PSCALLRPTCMD=<mode>,<cmdtype>
     *     [���]: ִ�����óɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             ���ô���ʱ��
     *             <CR><LF>+CME ERROR: <err ><CR><LF>
     *     [����]: ^PSCALLRPTCMD?
     *     [���]: <CR><LF>^PSCALLRPTCMD: <mode>, <cmdtype>CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <mode>: ������̬
     *             0��STICKģʽ
     *             Ŀǰֻ֧��STICKģʽ��
     *     <cmdtype>: �ϱ��������
     *             0���ϱ�����Ϊ^NDISSTAT��
     *             1���ϱ�����ΪЯ��cid��^NDISSTAT
     *             2���ϱ�����Ϊ^NDISSTATEX
     * [ʾ��]:
     *     �� ����STICK�����ϱ�����Ϊ^NDISSTAT
     *       AT^PSCALLRPTCMD=0,0
     *       OK
     *     �� ��ѯ��ǰ����
     *       AT^PSCALLRPTCMD?
     *       ^PSCALLRPTCMD: 0,0
     *       OK
     */
    { AT_CMD_ROAMPDPTYPE,
      AT_SetPsCallRptCmdPara, AT_SET_PARA_TIME, AT_QryPsCallRptCmdPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PSCALLRPTCMD", (VOS_UINT8 *)"(0),(0-2)" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ��ѯLANȫ�ֵ�ַ
     * [˵��]: E5��PDP������ѯLANȫ�ֵ�ַ��
     *         ָ��CID��ѯLANȫ�ֵ�ַ��ֻ����ʹ��^NDISDUP������PDP����ָ��CIDδ����򷵻�ERROR��
     *         ��E5ģʽ��֧�ֲ�ѯ��ָ��CID��ѯLANȫ�ֵ�ַ�����ֻ���֧�ֵģ�E5ģʽ�²�֧��ָ��CID��ѯLANȫ�ֵ�ַ��
     * [�﷨]:
     *     [����]: ^APLANADDR=<cid>
     *     [���]: <CR><LF>^APLANADDR: <ADDR>,<PrefixLen><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^APLANADDR?
     *     [���]: (list of <CR><LF>^APLANADDR: <CID>,<ADDR>,<PrefixLen><CR><LF>)<CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^APLANADDR=?
     *     [���]: <CR><LF>^APLANADDR: (list of supported <cid>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <CID>: cid��Ϣ��
     *     <ADDR>: LANȫ�ֵ�ַ����64 bits��ǰ׺��64 bist��MAC��ַ��ɡ�
     *     <PrefixLen>: ǰ׺���ȣ���λbit��
     * [ʾ��]:
     *     �� ָ��CID����ѯLANȫ�ֵ�ַ
     *       AT^APLANADDR=1
     *       ^APLANADDR: "300::2",64
     *       OK
     *     �� ��ѯLANȫ�ֵ�ַ
     *       AT^APLANADDR?
     *       ^APLANADDR: 1,"300::2",64
     *       OK
     */
    { AT_CMD_APLANADDR,
      AT_SetApLanAddrPara, AT_SET_PARA_TIME, AT_QryApLanAddrPara, AT_QRY_PARA_TIME, AT_TestApLanAddrPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^APLANADDR", (VOS_UINT8 *)"(1-11)" },
};

/* ע��TAF�������ҵ��AT����� */
VOS_UINT32 AT_RegisterDataTafCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDataTafCmdTbl, sizeof(g_atDataTafCmdTbl) / sizeof(g_atDataTafCmdTbl[0]));
}

