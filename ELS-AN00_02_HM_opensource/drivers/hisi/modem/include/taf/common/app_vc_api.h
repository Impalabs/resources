/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
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
#ifndef APP_VC_API_H
#define APP_VC_API_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "taf_type_def.h"
#include "vos.h"

#pragma pack(push, 4)

#define APP_VC_MSD_DATA_LEN (140) /* ��λ:btye */

#define VC_CALL_ECALL_REDIAL_CLEAN (10) /* PSAPҪ��IVS�����Ҷ� */
#define VOC_CONFIG_NO_CHANGE (2001)
#if (FEATURE_MBB_CUST == FEATURE_ON)
#define CPCM_PARA_INDEX_NUMER 5
#define CPCM_MAIN_MODE 0
#define CPCM_SECONDRY_MODE 2
#define ECHOEX_PARA_INDEX_NUM 5
#define ECHOEX_MAP_VALUE 12
#define MN_CALL_PCM_FR_NUM 100
#define MN_CALL_PCM_FR_LEN ((MN_CALL_PCM_FR_NUM >> 1) / sizeof(VOS_INT16)) /* �����в������� */
#define MN_CALL_PCM_FR_PARA_NUM (MN_CALL_PCM_FR_NUM >> 1) /* �����в����ܸ��� */
#define MN_CALL_PCM_FR_INPUT_NUM (MN_CALL_PCM_FR_NUM * 2) /* AT����������� */
#define VC_CODEC_TYPE_AMRWB 5 /* ���������� */
#endif

/*
 * 3���Ͷ���
 */
/* �����豸��0 �ֳ֣�1 �ֳ����᣻2 �������᣻3 ������4 ������7 �������᣻8 ����ͨ�� */
enum VC_PHY_DeviceMode {
    VC_PHY_DEVICE_MODE_HANDSET,        /* �ֳ� */
    VC_PHY_DEVICE_MODE_HANDS_FREE,     /* �ֳ����� */
    VC_PHY_DEVICE_MODE_CAR_FREE,       /* �������� */
    VC_PHY_DEVICE_MODE_EARPHONE,       /* ���� */
    VC_PHY_DEVICE_MODE_BLUETOOTH,      /* ���� */
    VC_PHY_DEVICE_MODE_PCVOICE,        /* PC����ģʽ */
    VC_PHY_DEVICE_MODE_HEADPHONE,      /* ��������MIC�Ķ���ͨ�� */
    VC_PHY_DEVICE_MODE_SUPER_HANDFREE, /* �������� */
    VC_PHY_DEVICE_MODE_SMART_TALK,     /* ����ͨ�� */
    VC_PHY_DEVICE_MODEM_USBVOICE, /* USB�豸 */
    VC_PHY_DEVICE_MODEM_USB_HEADSET,     /* USB Audio type-C�����豸 */
    VC_PHY_DEVICE_MODEM_HANDFREE_STEREO, /* ����˫�� */
    VC_PHY_DEVICE_MODEM_UAC,             /* USB Audio Class */
    VC_PHY_DEVICE_MODE_BUTT /* invalid value */
};
typedef VOS_UINT16 VC_PHY_DeviceModeUint16;

/*
 * �ṹ˵��: STICK��̬�£��û�ͨ��^CVOICE˽���������õ�����ģʽö��
 */
enum APP_VC_VoiceMode {
    APP_VC_VOICE_MODE_PCVOICE    = 0,
    APP_VC_VOICE_MODE_EARPHONE   = 1,
    APP_VC_VOICE_MODE_HANDSET    = 2,
    APP_VC_VOICE_MODE_HANDS_FREE = 3,
    APP_VC_VOICE_MODE_BUTT
};
typedef VOS_UINT16 APP_VC_VoiceModeUint16;

/*
 * �ṹ˵��: ��������˿�ö��
 */
enum APP_VC_VoicePort {
    APP_VC_VOICE_PORT_MODEM   = 1,
    APP_VC_VOICE_PORT_DIAG    = 2,
    APP_VC_VOICE_PORT_PCUI    = 3,
    APP_VC_VOICE_PORT_PCVOICE = 4,
    APP_VC_VOICE_PORT_PCSC    = 5,
    APP_VC_VOICE_PORT_BUTT
};
typedef VOS_UINT8 APP_VC_VoicePortUint8;

enum APP_VC_Event {
    APP_VC_EVT_PARM_CHANGED, /* service para changed */
    APP_VC_EVT_SET_VOLUME,
    APP_VC_EVT_SET_VOICE_MODE,
    APP_VC_EVT_GET_VOLUME,
    APP_VC_EVT_SET_MUTE_STATUS,
    APP_VC_EVT_GET_MUTE_STATUS,
#if (FEATURE_MBB_CUST == FEATURE_ON)
    APP_VC_EVT_SET_SMUT_STATUS,
    APP_VC_EVT_GET_SMUT_STATUS,
    APP_VC_EVT_SET_CMIC_STATUS,
    APP_VC_EVT_GET_CMIC_STATUS,
    APP_VC_EVT_SET_ECHOEX,
    APP_VC_EVT_GET_ECHOEX,
    APP_VC_EVT_GET_PCMFR_STATUS,
    APP_VC_EVT_SET_PCMFR_STATUS,
    APP_VC_EVT_SET_ECHOSWITCH,
    APP_VC_EVT_GET_ECHOSWITCH,
#endif

    APP_VC_EVT_SET_ECALL_CFG,
    APP_VC_EVT_ECALL_TRANS_STATUS,
    APP_VC_EVT_ECALL_TRANS_ALACK,
    APP_VC_EVT_ECALL_ABORT_CNF,
    APP_VC_EVT_BUTT
};
typedef VOS_UINT32 APP_VC_EventUint32;

enum APP_VC_MuteStatus {
    APP_VC_MUTE_STATUS_UNMUTED = 0,
    APP_VC_MUTE_STATUS_MUTED   = 1,
    APP_VC_MUTE_STATUS_BUTT
};
typedef VOS_UINT8 APP_VC_MuteStatusUint8;


enum TAF_VC_Ttymode {
    TAF_VC_TTY_OFF  = 0,
    TAF_VC_TTY_FULL = 1,
    TAF_VC_TTY_VCO  = 2,
    TAF_VC_TTY_HCO  = 3,
    TAF_VC_TTYMODE_BUTT
};
typedef VOS_UINT8 TAF_VC_TtymodeUint8;


enum APP_VC_Msg {
    /* AT->VC */
    /* ��������������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_REQ_SET_VOLUME = 0,
    /* ����ͨ��ģʽ������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_REQ_SET_MODE = 0x1,
    /* ��ѯͨ��ģʽ������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_REQ_QRY_MODE = 0x2,
    /* ���������˿�������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_REQ_SET_PORT = 0x4,
    /* ��ѯ�����˿�������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_REQ_QRY_PORT = 0x6,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_REQ_QRY_VOLUME = 0x8,
    /* ���þ���״̬������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_MUTE_STATUS_REQ = 0xA,
    /* ��ȡ����״̬������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_GET_MUTE_STATUS_REQ = 0xB,
    /* ����ǰ̨ģʽ������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_FOREGROUND_REQ = 0xC,
    /* ���ú�̨ģʽ������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_BACKGROUND_REQ = 0xE,
    /* ģʽ��ѯ������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_FOREGROUND_QRY = 0x10,
    /* ������������ģʽ��Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_MODEMLOOP_REQ = 0x12,
    /* ����MSD�������� */
    /* _MSGPARSE_Interception APP_ELF_VC_MSG_SET_MSD_REQ_STRU */
    APP_VC_MSG_SET_MSD_REQ = 0x13,
    /* ��ѯMSD�������� */
    /* _MSGPARSE_Interception ELF_MN_APP_REQ_MSG_STRU */
    APP_VC_MSG_QRY_MSD_REQ = 0x15,
    /* ����ecall�������� */
    /* _MSGPARSE_Interception APP_ELF_VC_MSG_SET_ECALL_CFG_REQ_STRU */
    APP_VC_MSG_SET_ECALL_CFG_REQ = 0x17,
    /* ��ѯecall������Ϣ���� */
    /* _MSGPARSE_Interception ELF_MN_APP_REQ_MSG_STRU */
    APP_VC_MSG_QRY_ECALL_CFG_REQ = 0x18,
    /* ����TTYģʽ������Ϣ */
    /* _MSGPARSE_Interception APP_ELF_VC_SET_TTYMODE_REQ_STRU */
    APP_VC_MSG_SET_TTYMODE_REQ = 0x1B,
    /* ��ѯTTYģʽ������Ϣ */
    /* _MSGPARSE_Interception ELF_MN_APP_REQ_MSG_STRU */
    APP_VC_MSG_QRY_TTYMODE_REQ = 0x1D,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_HIFI_RESET_BEGIN_NTF = 0x1F,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_HIFI_RESET_END_NTF = 0x20,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_ECALL_OPRTMODE_REQ = 0x21,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_ECALL_PUSH_REQ = 0x22,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_ECALL_ABORT_REQ = 0x24,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_I2S_TEST_REQ = 0x25,
    /* ERR->ERR */
    /* _MSGPARSE_Interception APP_VC_ReqMsgAsn */
    APP_VC_MSG_CNF_QRY_VOLUME = 0x9,
    /* VC->AT */
    /* ��ѯͨ��ģʽ�ظ���Ϣ */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_CNF_QRY_MODE = 0x3,
    /* ���������˿ڻظ���Ϣ */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_CNF_SET_PORT = 0x5,
    /* ��ѯ�����˿ڻظ���Ϣ */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_CNF_QRY_PORT = 0x7,
    /* ����ǰ̨ģʽ�ظ���Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_FOREGROUND_CNF = 0xD,

    /* ���ú�̨ģʽ�ظ���Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_BACKGROUND_CNF = 0xF,
    /* ģʽ��ѯ�ظ���Ϣ */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_FOREGROUND_RSP = 0x11,
    /* ����MSD���ݻظ� */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_SET_MSD_CNF = 0x14,
    /* ��ѯMSD���ݻظ� */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_QRY_MSD_CNF = 0x16,
    /* ��ѯecall������Ϣ�ظ� */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_QRY_ECALL_CFG_CNF = 0x19,
    /* DTMF Decoder Ind�ϱ���Ϣ */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_DTMF_DECODER_IND = 0x1A,
    /* ����TTYģʽ�ظ���Ϣ */
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_SET_TTYMODE_CNF = 0x1C,
    /* ��ѯTTYģʽ�ظ���Ϣ */
    /* _MSGPARSE_Interception APP_ELF_VC_SET_TTYMODE_CNF_STRU */
    APP_VC_MSG_QRY_TTYMODE_CNF = 0x1E,
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_SET_ECALL_PUSH_CNF = 0x23,
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_I2S_TEST_CNF = 0x26,
    /* _MSGPARSE_Interception MN_AT_IndEvt */
    APP_VC_MSG_I2S_TEST_RSLT_IND = 0x27,
#if (FEATURE_MBB_CUST == FEATURE_ON)
    /* AT->VC */
    /* �������о���״̬������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_SMUT_STATUS_REQ = 0x28,
    /* ��ȡ���о���״̬������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_GET_SMUT_STATUS_REQ = 0x29,
    /* �������о���״̬������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_UPDATE_SMUT_STATUS_REQ = 0x2A,
    /* ������������������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_CMIC_STATUS_REQ = 0x2B,
    /* ��ȡ��������������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_GET_CMIC_STATUS_REQ = 0x2C,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_ECHOEX_REQ = 0x2D,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_GET_ECHOEX_REQ = 0x2E,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_PCMFR_STATUS_REQ = 0x2F,
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_GET_PCMFR_STATUS_REQ = 0x30,
    /* ���û�����������������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_SET_ECHOSWITCH_REQ = 0x31,
    /* ��ȡ������������������Ϣ */
    /* _MSGPARSE_Interception APP_VC_ReqMsg */
    APP_VC_MSG_GET_ECHOSWITCH_REQ = 0x32,
#endif


    APP_VC_MSG_BUTT
};
typedef VOS_UINT16 APP_VC_MsgUint16;


enum APP_VC_QryGround {
    APP_VC_FOREGROUND     = 0,
    APP_VC_BACKGROUND     = 1,
    APP_VC_GROUND_INVALID = 2,
    APP_VC_GROUND_BUTT
};
typedef VOS_UINT16 APP_VC_QryGroundUint16;


enum APP_VC_StartHifiOrig {
    APP_VC_START_HIFI_ORIG_GUCALL = 0,
    APP_VC_START_HIFI_ORIG_IMSA   = 1,
    APP_VC_START_HIFI_ORIG_XCALL  = 2,
    APP_VC_START_HIFI_ORIG_BUTT
};
typedef VOS_UINT8 APP_VC_StartHifiOrigUint8;


enum APP_VC_EcallTransStatus {
    APP_VC_ECALL_MSD_TRANSMITTING_START = 0,   /* ��ǰMSD�����Ѿ���ʼ���� */
    APP_VC_ECALL_MSD_TRANSMITTING_SUCCESS,     /* ��ǰMSD�����Ѿ����ͳɹ� */
    APP_VC_ECALL_MSD_TRANSMITTING_FAIL,        /* ��ǰMSD�����Ѿ�����ʧ�� */
    APP_VC_ECALL_PSAP_MSD_REQUIRETRANSMITTING, /* PSAPҪ�����MSD���� */
    APP_VC_ECALL_TRANS_STATUS_BUTT
};
typedef VOS_UINT8 APP_VC_EcallTransStatusUint8;


enum APP_VC_EcallTransFailCause {
    APP_VC_ECALL_CAUSE_WAIT_PSAP_TIME_OUT = 0,    /* �ȴ�PSAP�Ĵ���ָʾ��ʱ */
    APP_VC_ECALL_CAUSE_MSD_TRANSMITTING_TIME_OUT, /* MSD���ݴ��䳬ʱ */
    APP_VC_ECALL_CAUSE_WAIT_AL_ACK_TIME_OUT,      /* �ȴ�Ӧ�ò��ȷ�ϳ�ʱ */
    APP_VC_ECALL_CAUSE_UNSPECIFIC_ERROR,          /* �����쳣���� */
    APP_VC_ECALL_CAUSE_NW_ERROR,                  /* ������� */
    APP_VC_ECALL_CAUSE_BUTT
};
typedef VOS_UINT8 APP_VC_EcallTransFailCauseUint8;


enum APP_VC_EcallMsdMode {
    APP_VC_ECALL_MSD_MODE_TRANSPARENT = 0, /* ͸��ģʽ */
    APP_VC_ECALL_MSD_MODE_BUTT
};
typedef VOS_UINT16 APP_VC_EcallMsdModeUint16;


enum APP_VC_EcallVocConfig {
    APP_VC_ECALL_VOC_CONFIG_NOT_ABANDON = 0, /* MSD���ݴ�����̴����� */
    APP_VC_ECALL_VOC_CONFIG_ABANDON,         /* MSD���ݴ�����̲������� */
    APP_VC_ECALL_VOC_CONFIG_BUTT
};
typedef VOS_UINT16 APP_VC_EcallVocConfigUint16;


enum APP_VC_EcallOprtMode {
    APP_VC_ECALL_OPRT_PULL = 0,
    APP_VC_ECALL_OPRT_PUSH,
    APP_VC_ECALL_OPRT_BUTT
};
typedef VOS_UINT8 APP_VC_EcallOprtModeUint8;


enum APP_VC_MsdSendMode {
    APP_VC_ECALL_MSD_SEND_SIP = 1,
    APP_VC_ECALL_MSD_SEND_IN_BAND = 2,
    APP_VC_ECALL_MSD_SEND_MODE_BUTT
};
typedef VOS_UINT8 APP_VC_MsdSendModeUint8;


enum APP_VC_I2sTestRslt {
    APP_VC_I2S_TEST_RSLT_SUCCESS  = 0x0000,
    APP_VC_I2S_TEST_RSLT_FAILED   = 0x0001,
    APP_VC_I2S_TEST_RSLT_TIME_OUT = 0x0002,
    APP_VC_I2S_TEST_RSLT_BUTT
};
typedef VOS_UINT32 APP_VC_I2sTestRsltUint32;


enum APP_VC_HifiResetType {
    APP_VC_HIFI_RESET_FROM_AP = 0, /* SOC��ƴƬAP�ഥ��HIFI RESET */
    APP_VC_HIFI_RESET_FROM_CP = 1, /* ƴƬCP�ഥ��HIFI RESET */

    APP_VC_HIFI_RESET_TYPE_BUTT
};
typedef VOS_UINT8 APP_VC_HifiResetTypeUint8;

#if (FEATURE_MBB_CUST == FEATURE_ON)
enum AppVcCpcmMode {
    APP_VC_CPCM_MODE_SECONDRY = 0,
    APP_VC_CPCM_MODE_MAIN = 1,
    APP_VC_CPCM_MODE_BUTT
};

enum AT_CMD_CMIC_VALUE_ENUM {
    AT_CMD_CMIC_LEV_1 = 64,    /* ��1������ֵ */
    AT_CMD_CMIC_LEV_2 = 67,    /* ��2������ֵ */
    AT_CMD_CMIC_LEV_3 = 70,    /* ��3������ֵ */
    AT_CMD_CMIC_LEV_4 = 73,    /* ��4������ֵ */
    AT_CMD_CMIC_LEV_5 = 76,    /* ��5������ֵ */
    AT_CMD_CMIC_LEV_6 = 79,    /* ��6������ֵ */
    AT_CMD_CMIC_LEV_7 = 82,    /* ��7������ֵ */
    AT_CMD_CMIC_LEV_8 = 84,    /* ��8������ֵ */
    AT_CMD_CMIC_LEV_9 = 86,    /* ��9������ֵ */
    AT_CMD_CMIC_LEV_10 = 88,   /* ��10������ֵ */
    AT_CMD_CMIC_LEV_11 = 90,   /* ��11������ֵ */
    AT_CMD_CMIC_LEV_12 = 92,   /* ��12������ֵ */
};
#endif

struct APP_VC_HifiResetInfo {
    APP_VC_HifiResetTypeUint8 hifiResetType; /* ����HIFI RESET���� */
    VOS_UINT8                 reserved[3];
};

typedef struct {
    VOS_MSG_HEADER                 /* _H2ASN_Skip */
    APP_VC_MsgUint16  msgName;     /* ��Ϣ�� */
    VOS_UINT8         reserve1[2]; /* ���� */
    MN_CLIENT_ID_T    clientId;    /* Client ID */
    MN_OPERATION_ID_T opId;        /* Operation ID */
    VOS_UINT8         reserve2;    /* ���� */
    VOS_UINT8         content[4];  /* ��Ϣ���� */
#if (FEATURE_MBB_CUST == FEATURE_ON)
    VOS_INT16 echoEnable;
    VOS_INT16 echoInputGain;
    VOS_INT16 echoLevelControl;
    VOS_INT16 echoClearFilterButton;
    VOS_INT16 echoVadInputLevel; /* Voice Activity Detection,VAD */
    VOS_UINT16 echoSwitchState;
    VOS_INT16 anfuAudioSndEq[MN_CALL_PCM_FR_LEN];
    VOS_INT16 anfuAudioRcvEq[MN_CALL_PCM_FR_LEN];
#endif
} APP_VC_ReqMsg;


typedef struct {
    VOS_BOOL                success; /* set(get) success or failure */
    VOS_UINT32              errCode;
    MN_CLIENT_ID_T          clientId;
    MN_OPERATION_ID_T       opId;
    APP_VC_VoicePortUint8   voicePort;
    VC_PHY_DeviceModeUint16 devMode;
    VOS_UINT16              volume;
    APP_VC_MuteStatusUint8  muteStatus;
    VOS_UINT8               rsv[2];

    APP_VC_EcallTransStatusUint8 ecallState;
    VOS_UINT32                   ecallDescription;
    VOS_UINT8                    ecallReportAlackValue;
    VOS_UINT8                    reserved[3];
#if (FEATURE_MBB_CUST == FEATURE_ON)
    VOS_UINT8               speakerMuteStatus;
    VOS_UINT8               reserved2[3];
    VOS_UINT16              upVolume;
    VOS_INT16               echo[ECHOEX_PARA_INDEX_NUM];
    VOS_INT16               pcmFr[MN_CALL_PCM_FR_PARA_NUM];
    VOS_UINT16              currEchoSwitchStatus;
    VOS_UINT16              codecType;
#endif
} APP_VC_EventInfo;


typedef struct {
    VOS_UINT8              qryRslt; /* ��ѯ success or failure */
    VOS_UINT8              reserved;
    APP_VC_QryGroundUint16 ground; /* ��ѯ��� */
} APP_VC_QryGroungRsp;


typedef struct {
    VOS_UINT8 msdData[APP_VC_MSD_DATA_LEN]; /* MSD�������� */
} APP_VC_MsgSetMsdReq;


typedef struct {
    VOS_UINT8 qryRslt; /* ��ѯ��� */
    VOS_UINT8 reserved[3];
    VOS_UINT8 msdData[APP_VC_MSD_DATA_LEN]; /* MSD�������� */
} APP_VC_MsgQryMsdCnf;


typedef struct {
    APP_VC_EcallMsdModeUint16   mode;      /* ���ݴ���ģʽ */
    APP_VC_EcallVocConfigUint16 vocConfig; /* ��������ʱ�Ƿ�������� */
} APP_VC_MsgSetEcallCfgReq;


typedef struct {
    VOS_UINT8                   qryRslt; /* ��ѯ��� */
    VOS_UINT8                   reserved[3];
    APP_VC_EcallMsdModeUint16   mode;      /* MSD���ݴ���ģʽ */
    APP_VC_EcallVocConfigUint16 vocConfig; /* ��������ʱ�Ƿ�������� */
} APP_VC_MsgQryEcallCfgCnf;


typedef struct {
    VOS_UINT8 rslt; /* ��� */
    VOS_UINT8 reserved[3];
} APP_VC_SetMsdCnf;


typedef struct {
    VOS_UINT32 result; /* ��� */
} APP_VC_I2sTestCnf;


typedef struct {
    APP_VC_I2sTestRsltUint32 result; /* ��� */
} APP_VC_I2sTestRsltInd;


typedef struct {
    APP_VC_EcallOprtModeUint8 ecallOpMode; /* operate mode:pull or push */
    VOS_UINT8                 reserved[3];
} APP_VC_SetOprtmodeReq;


typedef struct {
    VOS_UINT8           qryRslt; /* ��ѯ success or failure */
    TAF_VC_TtymodeUint8 ttyMode; /* ��ѯ��� */
    VOS_UINT8           reserved[2];
} APP_VC_QryTtymodeCnf;


typedef struct {
    TAF_VC_TtymodeUint8 ttyMode; /* ����TTY MODE */
    VOS_UINT8           reserved[3];
} APP_VC_SetTtymodeReq;


typedef struct {
    VOS_UINT8 rslt;
    VOS_UINT8 reserved[3];
} APP_VC_MsgCnfAsn;


typedef struct {
    VOS_UINT8 content[4];
} APP_VC_ReqMsgAsn;
#if (FEATURE_MBB_CUST == FEATURE_ON)
typedef struct {
    VOS_INT16 echoEnable;
    VOS_INT16 echoInputGain;
    VOS_INT16 echoLevelControl;
    VOS_INT16 echoClearFilterButton;
    VOS_INT16 echoVadInputLevel; /* Voice Activity Detection,VAD */
    VOS_INT16 reserve;
} AppVcEchoexReq;
typedef struct {
    VOS_UINT8 pcmFr[MN_CALL_PCM_FR_NUM];
} APP_VC_PcmFrReq;
#endif

VOS_UINT32 APP_VC_SetVoiceVolume(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT8 voiceVolume);

VOS_UINT32 APP_VC_SetVoiceMode(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT8 voiceMode, VOS_UINT32 rcvPid);

VOS_UINT32 APP_VC_GetVoiceMode(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId);

extern VOS_UINT32 APP_VC_GetVoiceVolume(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId);

/*
 * ��������: ���þ���״̬
 */
VOS_UINT32 APP_VC_SetMuteStatus(VOS_UINT16 clientId, VOS_UINT8 opId, APP_VC_MuteStatusUint8 muteStatus);

/*
 * ��������: ��ȡ����״̬
 */
VOS_UINT32 APP_VC_GetMuteStatus(VOS_UINT16 clientId, VOS_UINT8 opId);

/*
 * ��������: ���ý�����˳���������
 */
VOS_UINT32 APP_VC_SetModemLoop(VOS_UINT16 clientId, VOS_UINT8 opId, VOS_UINT8 modemLoop);

VOS_UINT32 APP_VC_SendHifiResetNtf(VOS_UINT32 rcvPid, APP_VC_HifiResetTypeUint8 hifiResetType, APP_VC_MsgUint16 msgName);

#if (FEATURE_MBB_CUST == FEATURE_ON)
/* ��������: �������о���״̬ */
VOS_UINT32 APP_VC_SetSpeakerMuteStatus(VOS_UINT16 clientId, VOS_UINT8 opId, VOS_UINT8 muteStatus);

/* ��������: ��ȡ���о���״̬ */
VOS_UINT32 APP_VC_GetSpeakerMuteStatus(VOS_UINT16 clientId, VOS_UINT8 opId);

/* ��������: �������о���״̬ */
VOS_UINT32 APP_VC_UpdateSpeakerMuteStatus(VOS_UINT16 clientId, VOS_UINT8 opId, VOS_UINT8 speakerMute);

/* ��������: ������������ */
VOS_UINT32 APP_VC_SetUpVoiceVolume(TAF_UINT16 clientId, TAF_UINT8 opId, VOS_UINT8 voiceVolume);

/* ��������: ��ȡ�������� */
VOS_UINT32 APP_VC_GetUpVoiceVolume(VOS_UINT16 clientId, VOS_UINT8 opId);

VOS_UINT32 APP_VC_SetEchoSuppression(VOS_UINT16 clientId, VOS_UINT8 opId, AppVcEchoexReq stEchoex);
VOS_UINT32 APP_VC_GetEchoSuppression(VOS_UINT16 clientId, VOS_UINT8 opId);
VOS_UINT32 APP_VC_SetPcmFr(VOS_UINT16 clientId, VOS_UINT8 opId, APP_VC_PcmFrReq pcmFrPara);
VOS_UINT32 APP_VC_GetPcmFr(VOS_UINT16 clientId, VOS_UINT8 opId);
VOS_UINT32 APP_VC_SetEchoSwitch(VOS_UINT16 clientId, VOS_UINT8 opId, VOS_UINT16 echoState);
VOS_UINT32 APP_VC_GetEchoSwitch(VOS_UINT16 clientId, VOS_UINT8 opId);
#endif

typedef struct {
    VOS_CHAR   ucDtmfCode;
    VOS_UINT8  ucReserved;
    VOS_UINT16 usOntime;
} APP_VC_DtmfDecoderInd;



#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* APP_VC_API_H */
