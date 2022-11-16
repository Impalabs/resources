/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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
#ifndef _ATDEVICECMD_H_
#define _ATDEVICECMD_H_

#include "v_typdef.h"
#include "at_cmd_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(push, 4)

/* ����ʹ�õļ��ܽṹ */

/* WIFI KEY�ַ������ȣ���NVIMģ��Ipcom_parameters_handle.h�ļ��� NV_WLKEY_LEN ����һ�� */
#define AT_NV_WLKEY_LEN 27

/* ��Ȩģʽ�ַ������ȣ���NVIMģ��Ipcom_parameters_handle.h�ļ��� NV_WLAUTHMODE_LEN ����һ�� */
#define AT_NV_WLAUTHMODE_LEN 16

/* ����ģʽ�ַ������ȣ���NVIMģ��Ipcom_parameters_handle.h�ļ��� NV_ENCRYPTIONMODES_LEN ����һ�� */
#define AT_NV_ENCRYPTIONMODES_LEN 5

/* WPA�������ַ������ȣ���NVIMģ��Ipcom_parameters_handle.h�ļ��� NV_WLWPAPSK_LEN ����һ�� */
#define AT_NV_WLWPAPSK_LEN 65

/* NV��50012�����ݽṹnv_wifisec_type WIFI KEY��������ݳ��� */
#define AT_NV_WIFISEC_OTHERDATA_LEN 72

/* �����֧�ֵ�WIFI KEY���� */
#define AT_WIWEP_CARD_WIFI_KEY_TOTAL 4

/* ��λ��֧�ֵ�WIFI KEY���� */
#define AT_WIWEP_TOOLS_WIFI_KEY_TOTAL 16

/* PHYNUM�������������MAC������ų��� */
#define AT_PHYNUM_MAC_LEN 12

/* PHYNUM�������������MAC���������^WIFIGLOBAL����ƥ����Ҫ���ӵ�ð�Ÿ��� */
#define AT_PHYNUM_MAC_COLON_NUM 5

#define AT_TSELRF_PATH_TOTAL 4

#define AT_WIFIGLOBAL_MAC_LEN_BETWEEN_COLONS 2 /* MAC��ַ��ð��֮����ַ����� */

#define BAND_WIDTH_NUMS 6




enum AT_TbatOperationType {
    AT_TBAT_BATTERY_ANALOG_VOLTAGE  = 0,
    AT_TBAT_BATTERY_DIGITAL_VOLTAGE = 1,
    AT_TBAT_BATTERY_VOLUME          = 2,
    AT_TBAT_OPERATION_TYPE_BUTT
};
typedef VOS_UINT32 AT_TbatOperationTypeUint32;


enum AT_TbatOperationDirection {
    AT_TBAT_READ_FROM_UUT,
    AT_TBAT_SET_TO_UUT,
    AT_TBAT_OPERATION_DIRECTION_BUTT
};
typedef VOS_UINT32 AT_TbatOperationDirectionUint32;


enum AT_KeyType {
    AT_KEY_TYPE_DIEID    = 1,
    AT_KEY_TYPE_TBOX_SMS = 2,
    AT_KEY_TYPE_SOCID    = 3,
    AT_KEY_TYPE_AUTHKEY  = 4,
    AT_KEY_TYPE_BUTT
};

enum AT_SecureState {
    AT_NOT_SET_STATE            = 0, /* δ����оƬģʽ��״̬ */
    AT_NO_DX_SECU_DISABLE_STATE = 0, /* ��DX��������ǰ�ȫģʽ��״̬ */
    AT_NO_DX_SECU_ENABLE_STATE  = 1, /* ��DX�������氲ȫģʽ��״̬ */
    AT_DX_RMA_STATE             = 2, /* ����DX��������ǰ�ȫģʽ��״̬ */
    AT_DX_SECURE_STATE          = 3, /* ����DX�������氲ȫģʽ��״̬ */
    AT_SECURE_STATE_BUTT
};

enum AT_DRV_ReturnState {
    AT_DRV_NOT_SETTED_STATE = 0, /* δ����оƬ״̬ */
    AT_DRV_STATE_SECURE     = 1, /* ��ȫ״̬ */
    AT_DRV_STATE_RMA        = 2, /* �ǰ�ȫ״̬ */
    AT_DRV_STATE_BUTT
};

enum AT_SetState {
    AT_SET_SECURE_STATE = 1, /* ����Ϊ��ȫ״̬ */
    AT_SET_RMA_STATE    = 2, /* ����Ϊ�ǰ�ȫ״̬ */
    AT_SET_STATE_BUTT
};

#define SLT_SUCCESS 0
#define SLT_ERROR 1

#define AT_AUTHKEY_LEN 8   /* 64 Bits��AUTH KEY �������� */
#define AT_KCE_LEN 16      /* 128bit KCE�ӽ���keyֵ���� */
#define AT_SOCID_LEN 32    /* 256Bits��SOCID�������� */
#define AT_KEY_HASH_LEN 32 /* SOCID����AUTHKEY��hashֵ���� */
#define AT_KEYBUFF_LEN 32
#ifdef MBB_SLT
#define AT_BSN_LEN 16 /* BSN���볤�� */
#endif

#define AT_SECUREDEBUG_VALUE 3 /* ���ð�ȫDEBUG��Ȩ�ɰ�ȫ֤����� */
#define AT_SECDBGRESET_VALUE 1 /* �԰�ȫ�����Coresight ����ʱ����ʱ��λSecEngine */
#define AT_CSRESET_VALUE 1     /* �԰�ȫ�����Coresight ����ʱ����ʱ��λSecEngine */
#define AT_DFTSEL_VALUE 1      /* ����ͨ��������֤��ʽ������ȫDFT���� */
#define AT_DFTSEL_DISABLE_VALUE 3 /* ��ֹDFT���Թ��� */

/*
 * ���� g_stATDislogPwd �е� DIAG�ڵ�״̬Ҫ���뱸��NV�б�; �����벻�ñ���
 * �ʽ� g_stATDislogPwd �е��������, ��ʹ�����е� DIAG ��״̬;
 * ���¶���NV��������������
 */
extern VOS_INT8 g_atOpwordPwd[AT_OPWORD_PWD_LEN + 1];


typedef struct {
    VOS_UINT8 aucwlKeys[AT_NV_WLKEY_LEN];
} AT_WifisecWiwepInfo;

VOS_UINT32 AT_ExistSpecificPort(VOS_UINT8 portType);
extern VOS_UINT32 AT_AsciiToHex(VOS_UINT8 *src, VOS_UINT8 *dst);
extern VOS_UINT32 AT_AsciiToHexCode_Revers(VOS_UINT8 *src, VOS_UINT16 dataLen, VOS_UINT8 *dst);

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of at_device_cmd.h */
