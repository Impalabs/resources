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
#include "at_device_drv_cmd_tbl.h"
#include "AtParse.h"
#include "at_cmd_proc.h"
#include "at_parse_core.h"
#include "at_test_para_cmd.h"

#include "at_device_drv_set_cmd_proc.h"
#include "at_device_drv_qry_cmd_proc.h"


#define THIS_FILE_ID PS_FILE_ID_AT_DEVICE_DRV_CMD_TBL_C

static const AT_ParCmdElement g_atDeviceDrvCmdTbl[] = {

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡ��ص�ѹ
     * [˵��]: ������������ڴ���ص��ն˲�Ʒ��Ŀǰ����E5��PAD���ֻ�������ȡ��صĵ�ѹֵ��ָ��������ṩ�������ѹֵ��
     * [�﷨]:
     *     [����]: ^TBATVOLT?
     *     [���]: <CR><LF>^TBATVOLT: <result><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <result>: ��ص�ѹֵ����λΪmV��ȡֵ��ΧΪ0��65535��
     * [ʾ��]:
     *     �� ��ѯ��ص�ѹ
     *       AT^TBATVOLT?
     *       ^TBATVOLT: 3700
     *       OK
     */
    { AT_CMD_BATVOL,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryBatVolPara, AT_QRY_PARA_TIME, At_CmdTestProcERROR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^TBATVOLT", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��Ϣ����
     * [˵��]: ��������ǰ����Ϣ���ݡ�����Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^INFORBU
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [ʾ��]:
     *     �� ��Ϣ���ݳɹ�
     *       AT^INFORBU
     *       0
     *       OK
     *     �� ��Ϣ����ʧ��
     *       AT^INFORBU
     *       ERROR
     */
    { AT_CMD_INFORBU,
      atSetNVFactoryBack, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^INFORBU", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡ�汾��
     * [˵��]: ��ȡ�汾�ţ���������汾��Ӳ���汾����Ʒ�ͺš�
     * [�﷨]:
     *     [����]: ^VERSION?
     *     [���]: <CR><LF>^VERSION: BDT:<build_time>
     *             <CR><LF>^VERSION: EXTS:<external_sw_version>
     *             <CR><LF>^VERSION: INTS:<internal_sw_version>
     *             <CR><LF>^VERSION: EXTD:<external_db_version>
     *             <CR><LF>^VERSION: INTD:<internal_db_version>
     *             <CR><LF>^VERSION: EXTH:<external_hw_version>
     *             <CR><LF>^VERSION: INTH:<internal_hw_version>
     *             <CR><LF>^VERSION: EXTU:<external _dut_name>
     *             <CR><LF>^VERSION: INTU:<internal _dut_name>
     *             <CR><LF>^VERSION: CFG:<configurate_version>
     *             <CR><LF>^VERSION: PRL:<prl_version><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <build_time>: �汾����ʱ�䣬��ʽ��Oct 15 2009 16:27:00��
     *     <external_db_version>: �ⲿ��̨����汾�š�
     *     <external_hw_version>: �ⲿӲ���汾�ţ���ʽ��Ver.A��
     *     <internal_hw_version>: �ڲ�Ӳ���汾�š�
     *     <external_dut_name>: �ⲿ��Ʒ���ƣ���ʽ��E122��
     *     <internal_dut_name>: �ڲ���Ʒ���ơ�
     *     <configurate_version>: �����ļ��汾�ţ���ʽ��1004��
     *     <prl_version>: PRL�汾�ţ�C��ʽר�á�
     * [ʾ��]:
     *     �� ��ȡ������ذ汾��
     *       AT^VERSION?
     *       ^VERSION: BDT:Jul  4 2011, 21:33:25
     *       ^VERSION: EXTS:21.131.00.00.00
     *       ^VERSION: INTS:
     *       ^VERSION: EXTD:PCSV22.001.04.00.03
     *       ^VERSION: INTD:
     *       ^VERSION: EXTH:CH2E353SM
     *       ^VERSION: INTH:
     *       ^VERSION: EXTU:E353
     *       ^VERSION: INTU:
     *       ^VERSION: CFG:1004
     *       ^VERSION: PRL:
     *       OK
     */
    { AT_CMD_VERSION,
      VOS_NULL_PTR, AT_NOT_SET_TIME, At_QryVersion, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^VERSION", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �����������ܽŵ�ƽ
     * [˵��]: ���û��߲�ѯ��Ʒ���������ܽţ���ӦGPIO����ƽ������Ʒ��֧��ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^GPIOPL=<PL>
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^GPIOPL?
     *     [���]: <CR><LF>^GPIOPL: (@GPIOPL)<PL><CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <PL>: GPIO�ܽŵ�ƽ����λ��ʾ��0Ϊ�͵�ƽ��1Ϊ�ߵ�ƽ��
     *             ATδ���ò���Ĭ��Ϊ0����ֵʹ��16��������ʾ����Ҫ����0x��ʶ��������󳤶�Ϊ20byte������ֽڼ��λ��ǰ���ֽ��ڲ�����bitλ�ߵ�����û��ʹ�õ��ĸ�λ��������ʡ�ԡ�
     * [ʾ��]:
     *     �� ����GPIO��2�͵�4���ܽ�Ϊ�ߵ�ƽ
     *       AT^GPIOPL=0x0A
     *       OK
     *     �� ��������
     *       AT^GPIOPL=?
     *       ^GPIOPL: (@GPIOPL)
     *       OK
     */
    { AT_CMD_GPIOPL,
      At_SetGPIOPL, AT_SET_PARA_TIME, At_QryGPIOPL, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^GPIOPL", (VOS_UINT8 *)"(@GPIOPL)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �ָ�����Ĭ��ֵ
     * [˵��]: ���ö�����ΪĬ��ֵ������Ʒ��֧�ֿ�ֱ�ӷ���ERROR��
     * [�﷨]:
     *     [����]: ^CSDFLT
     *     [���]: <CR><LF>OK<CR><LF>
     *             �������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CSDFLT?
     *     [���]: <CR><LF>^CSDFLT: <value_info><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *     [����]: ^CSDFLT=?
     *     [���]: <CR><LF>^CSDFLT: (list of supported < value_info >s)<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value_info>: ����ֵ��Ϣ������Ϊһ���ֽڡ�
     *             0��������Ϊ����Ĭ��ֵ��
     *             1��������Ϊ�ǵ���Ĭ��ֵ��
     * [ʾ��]:
     *     �� Ĭ��ֵ�ָ�
     *       AT^CSDFLT
     *       OK
     *     �� ��������
     *       AT^CSDFLT=?
     *       ^CSDFLT: (0,1)
     *       OK
     */
    { AT_CMD_CSDFLT,
      At_SetCsdfltPara, AT_NOT_SET_TIME, At_QryCsdfltPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CSDFLT", (VOS_UINT8 *)"(0,1)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ���Ե��
     * [˵��]: ������������ڴ���ص��ն˲�Ʒ����ȡ��صĵ�ѹֵ����ص�������Ϣ��
     * [�﷨]:
     *     [����]: ^TBAT=<type>,<opr>[,<value1>[,<value2>]]
     *     [���]: <CR><LF>^TBAT: <type>[,<value>]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^TBAT?
     *     [���]: <CR><LF>^TBAT: <mount type><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^TBAT=?
     *     [���]: <CR><LF>^TBAT: (list of supported <type>s),(list of supported <opr>s) ,(list of supported <value1>s) ,(list of supported <value2>s) <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <type>: ������Ŀ���͡�
     *             0����ص�ѹģ��ֵ��
     *             1����ص�ѹ����ֵ������ѹ��HKADCֵ������Ӧ<value>ȡֵ��ΧΪ0��65535��
     *     <opr>: ��ʾ�������������򣬷�Ϊ��ȡ���������֣��������еĲ�����Ŀ��֧�����ò�������Ҫ���ݾ�����Ŀȷ����
     *             0���ӵ����ȡ��
     *             1�����õ��塣
     *     <value>: ��ѯ�õ��ĵ�ص�ѹ����ֵ��
     *     <value1>: ���õ�ص�ѹ����ֵʱ��Ч��3.4V��Ӧ��HKADCֵ��
     *     <value2>: ���õ�ص�ѹ����ֵʱ��Ч��4.2V��Ӧ��HKADCֵ��
     *     <mount type>: ��ذ�װ��ʽ��
     *             0���޵�أ�
     *             1���ɸ�����أ�
     *             2������һ�廯��ء�
     * [ʾ��]:
     *     �� ���õ�ص�ѹ����ֵ3.4V��Ӧ��HKADCֵΪ340��4.2V��Ӧ��HKADCֵΪ420
     *       AT^TBAT=1,1,340,420
     *       OK
     *     �� ��ѯ��ص�ѹ����ֵ����ѯ���ĵ�ص�ѹ����ֵΪ370
     *       AT^TBAT=1,0
     *       ^TBAT: 1,370
     *       OK
     *     �� ��ѯ��ذ�װ��ʽ�������صİ�װ��ʽΪ�ɸ������
     *       AT^TBAT?
     *       ^TBAT: 1
     *       OK
     *     �� ��������
     *       AT^TBAT=?
     *       ^TBAT: (0,1),(0,1),(0-65535),(0-65535)
     *       OK
     */
    { AT_CMD_TBAT,
      AT_SetTbatPara, AT_SET_PARA_TIME, AT_QryTbatPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_SET_PARA_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (TAF_UINT8 *)"^TBAT", (VOS_UINT8 *)"(0,1),(0,1),(0-65535),(0-65535)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯPA/SIM/����¶�
     * [˵��]: AP�ϵĹ��̲˵�ʹ�ø�������MODEM��ѯPA��SIM���͵�ص��¶���Ϣ������PA����G PA��W PA��L PA��
     *         ������Ϊƽ̨������ð汾��֧��Lģ���ʲ���<L PAtemp>������Чֵ��
     *         AP-Modem��̬�У���ΪModem�಻���Ƶ�أ��ʲ���<BATTERYtemp>������Чֵ��
     *         AP-Modem��̬�У�ĿǰӲ����û�����¶ȼ����ƣ���SIM���¶ȷ�����Чֵ��
     *         G PA��W PAͨ��ͬһ�������������¶ȣ�����ͨ��ͬһ���Ĵ��������¶ȣ���ͬһʱ������������������¶���ͬ��
     * [�﷨]:
     *     [����]: ^CHIPTEMP?
     *     [���]: <CR><LF>^CHIPTEMP: <G PAtemp>,<W PAtemp>, <L PAtemp>,<SIMtemp>,<BATTERYtemp><CR><LF> <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^CHIPTEMP=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <G PAtemp>: ����ֵ��GSM PAоƬ��ǰ���¶ȣ�ȡֵ��Χ-200~850����λΪ0.1�档
     *     <W PAtemp>: ����ֵ��WCDMA PAоƬ��ǰ���¶ȣ�ȡֵ��Χ-200~1250����λΪ0.1�档
     *     <L PAtemp>: ����ֵ��LTE PAоƬ��ǰ���¶ȣ���ǰ��֧�֣�ֱ�ӷ���65535��
     *     <SIMtemp>: ����ֵ��SIM����ǰ���¶ȣ�ȡֵ��Χ10~100����λΪ1�档
     *     <BATTERYtemp>: ����ֵ����ص�ǰ���¶ȣ�ȡֵ��Χ-100~550����λΪ0.1�档
     * [ʾ��]:
     *     �� ��ѯPA��SIM���͵�ص��¶���Ϣ
     *       AT^CHIPTEMP?
     *       ^CHIPTEMP: 390,390,65535,65535,65535
     *       OK
     */
    { AT_CMD_CHIPTEMP,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryChipTempPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^CHIPTEMP", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯNV�����Ƿ����
     * [˵��]: ��������������Ʒ��NV�����Ƿ���ڡ�
     *         ������Ϊƽ̨�����Ŀǰֻ��AP-Modem��̬����Ч��
     * [�﷨]:
     *     [����]: ^NVBACKUPSTAT?
     *     [���]: <CR><LF>^NVBACKUPSTAT: <backupstat><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^NVBACKUPSTAT=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <backupstat>: ����ֵ��NV���������ȡֵ��ΧΪ0��1��
     *             0��NV���ݲ����ڡ�
     *             1��NV���ݴ��ڡ�
     * [ʾ��]:
     *     �� ��ѯNV�����Ƿ����
     *       AT^NVBACKUPSTAT?
     *       ^NVBACKUPSTAT: 1
     *       OK
     */
    { AT_CMD_NVBACKUPSTAT,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryNvBackupStatusPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NVBACKUPSTAT", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯNAND FLASH���л�������
     * [˵��]: AP�ϵĹ��̲˵�ʹ�ø�������MODEM��ѯNAND FLASH���л���������б�
     *         ������Ϊƽ̨�����Ŀǰֻ��AP-Modem��̬����Ч�������б�������100������������
     * [�﷨]:
     *     [����]: ^NANDBBC?
     *     [���]: <CR><LF>^NANDBBC: <totalNum>
     *             [[<CR><LF>^NANDBBC: <index>]����] <CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^NANDBBC=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <totalNum>: ����ֵ��NAND FLASH���л�����ܸ�����ȡֵ��Χ��0~65535��
     *     <index>: ����ֵ��NAND FLASH���������ֵ��ȡֵ��Χ��0~65535��
     * [ʾ��]:
     *     �� ��ѯNAND FLASH�����л��������
     *       AT^NANDBBC?
     *       ^NANDBBC: 2
     *       ^NANDBBC: 10
     *       ^NANDBBC: 28
     *       OK
     */
    { AT_CMD_NANDBBC,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryNandBadBlockPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NANDBBC", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ѯNAND FLASH�ͺ�
     * [˵��]: AP�ϵĹ��̲˵�ʹ�ø�������MODEM��ѯNAND FLASH���ͺ���Ϣ������������Ϣ������ID���������ơ��豸ID���豸����ַ�����
     *         ������Ϊƽ̨�����Ŀǰֻ��AP-Modem��̬����Ч��
     * [�﷨]:
     *     [����]: ^NANDVER?
     *     [���]: <CR><LF>^NANDVER: <mfuid>,<mfuname>,<deviceid>, <devicespec><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     *     [����]: ^NANDVER=?
     *     [���]: <CR><LF>OK<CR><LF>
     * [����]:
     *     <mfuid>: ����ֵ��NAND FLASH�ĳ���ID��
     *     <mfuname>: �ַ������ͣ��������ơ��16��ASCII�ַ���
     *     <deviceid>: ����ֵ���豸ID��
     *     <devicespec>: �ַ������ͣ��豸����ַ������32��ASCII�ַ���
     * [ʾ��]:
     *     �� ��ѯNAND FLASH���ͺ���Ϣ����SamsungΪ����
     *       AT^NANDVER?
     *       ^NANDVER: 236,"Samsung",218,"NAND 256MiB 3,3V 8-bit"
     *       OK
     */
    { AT_CMD_NANDVER,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryNandDevInfoPara, AT_QRY_PARA_TIME, At_CmdTestProcOK, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^NANDVER", VOS_NULL_PTR },

#if (FEATURE_LTE == FEATURE_ON)
    /* ����NV�ָ� */
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ����NV�ָ�����
     * [˵��]: ����NV�ӱ������ָ�������NV�ָ�ǰ��������Ҫʹ��DATALOCK����������ݽ��������򷵻�ERROR��
     * [�﷨]:
     *     [����]: ^INFORRS
     *     [���]: <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>ERROR<CR><LF>
     * [����]:
     *     <err_code>: ������
     *             0����ʾд����ʱ�����ݱ���δ����, �޷������������ݡ�
     */
    { AT_CMD_INFORRS,
      At_SetInfoRRS, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^INFORRS", VOS_NULL_PTR },
#endif

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �������ļ�����
     * [˵��]: PC������ô������ȡ����������ļ�������֧�������ļ�������XML�ļ����ı��ļ���Binary�ļ�����
     * [�﷨]:
     *     [����]: ^RSFR=<itemname>,<sub_itemname>
     *     [���]: <CR><LF>^RSFR: <itemname>,<sub_itemname>,<ref>,<total>,\
     *             <index>,<item><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             ��MT��ش���ʱ��
     *             <CR><LF>+CME ERROR: <err><CR><LF>
     *     [����]: ^RSFR=?
     *     [���]: <CR><LF>^RSFR: (list of supported <itemname>s)<CR><LF>
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
    { AT_CMD_RSFR,
      AT_SetRsfrPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, AT_TestRsfrPara, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^RSFR", (VOS_UINT8 *)"(name),(subname)" },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��Ϣ����
     * [˵��]: ���ڲ��߽���NV��ݡ�
     * [�﷨]:
     *     [����]: ^NVBACKUP
     *     [���]: <CR><LF>OK<CR><LF>
     *             ���������
     *             <CR><LF>ERROR<CR><LF>
     * [ʾ��]:
     *     �� NV���ݳɹ�
     *       AT^NVBACKUP
     *       0
     *       OK
     *     �� NV����ʧ��
     *       AT^NVBACKUP
     *       ERROR
     */
    { AT_CMD_NVBACKUP,
      atSetNVBackup, AT_UPGRADE_TIME_50S, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_ERROR, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
      (VOS_UINT8 *)"^NVBACKUP", VOS_NULL_PTR },

    /*
     * [���]: װ��AT-GUCװ��
     * [����]: ��ȡPMU_DIE_ID
     * [˵��]: ���������ڶ�ȡPMU_DIE_ID��
     *         BalongV7R22C30������5Gƽ̨�ж�������PMUоƬ����������Ի�ȡ��Ӧ���͵�PMU_DIE_ID��
     * [�﷨]:
     *     [����]: ^PMUDIESN=<value>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^PMUDIESN: 0x<DIEID><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *             ˵������������ֻ��NR��򿪰汾��֧�֡�
     *     [����]: ^PMUDIESN?
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>^PMUDIESN: 0x<DIEID><CR><LF>
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     * [����]:
     *     <value>: PMUоƬ����ö��ֵ��ȡֵ��Χ0,1,2.
     *             ˵������������ֻ��NR��򿪰汾��֧�֡�
     *     <DIEID>: �ַ������ͣ�pmudieid������16������� 20�ֽ�160bit
     * [ʾ��]:
     *     �� ִ�в������� �ײ��ȡ��PMU_DIE_ID�����
     *       AT^PMUDIESN?
     *       ^PMUDIESN: 0x0000000000000000000000000000000000000000
     *       OK
     */
    { AT_CMD_PMUDIESN,
      AT_SetPmuDieSNPara, AT_SET_PARA_TIME, AT_QryPmuDieSNPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^PMUDIESN", (VOS_UINT8 *)"(0,1,2)" },


    /*
     * [���]: װ��AT-GUCװ��
     * [����]: GNSS����֪ͨ
     * [˵��]: Ϊ����RFʱ�����ڴ򿪶�GNSS������Ӱ�죬��ҪHi110XоƬ��GNSS�����¸�modem��AT���modem���յ�AT����󣬵���RFʱ�ӿ��Ƶ���ӿڣ�֪ͨ�������ڴ���GNSS����������ʱRFʱ��û�д򿪣��򱣳ֹرգ����ϲ��RFʱ�ӣ���򿪸�ʱ�ӣ������յ��رո�ʱ�ӵ�ָ��򱣳ִ򿪣������浱ǰ�ϲ��ָ���Hi110XоƬ�˳�GNSS�����󣬻ᷢAT���������ᴫ�ݵ������������ʱ�ӵĿ���ֻ���ϲ�������ơ���Kirin980������ƽ̨��ʹ�á�
     * [�﷨]:
     *     [����]: ^GNSSNTY=<value>
     *     [���]: ִ�гɹ�ʱ��
     *             <CR><LF>OK<CR><LF>
     *             �������ʱ���أ�
     *             <CR><LF>+CME ERROR: <err_code><CR><LF>
     *     [����]: ^GNSSNTY=?
     *     [���]: <CR><LF>^GNSSNTY: (list of supported <value>s)
     *             <CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <value>: ����ֵʮ���� ��Χ��0-1����
     *             0��֪ͨ���������Ƴ�GNSS����������mdrv_pmic_rfclk_release()��
     *             1��֪ͨ�������ڴ���GNSS����������mdrv_pmic_rfclk_vote()��
     * [ʾ��]:
     *     �� ִ������
     *       AT^GNSSNTY=0
     *       OK
     */
    { AT_CMD_GNSSNTY,
      At_SetGnssNtyPara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
      (VOS_UINT8 *)"^GNSSNTY", (VOS_UINT8 *)"(0,1)" },

    /* ���翨�汾�Զ������Կ���Ӧ���ֶ����� */
    { AT_CMD_APPDIALMODE,
      At_SetAppDialModePara, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL,
      (VOS_UINT8 *)"^APPDIALMODE", (VOS_UINT8 *)"(0-1)" },
};

/* ע��DRVװ��AT����� */
VOS_UINT32 AT_RegisterDeviceDrvCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atDeviceDrvCmdTbl, sizeof(g_atDeviceDrvCmdTbl) / sizeof(g_atDeviceDrvCmdTbl[0]));
}

