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

#ifndef __MTA_COMM_INTERFACE_H__
#define __MTA_COMM_INTERFACE_H__

#include "vos.h"
#include "at_mn_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(push, 4)

/* network monitor GSM���� �ϱ������� */
#define NETMON_MAX_GSM_NCELL_NUM 6

/* network monitor UTRAN ���� �ϱ������� */
#define NETMON_MAX_UTRAN_NCELL_NUM 16

/* network monitor LTE ���� �ϱ������� */
#define NETMON_MAX_LTE_NCELL_NUM 16

/* network monitor NR ���� �ϱ������� */
#define NETMON_MAX_NR_NCELL_NUM 16

#define NETMON_MAX_NR_CC_NUM 8

#define MTA_LRRC_MSG_TYPE_BASE 0x1000 /* MTAģ����LTE�������Ϣ���� */
#define MTA_COMM_MSG_TYPE_BASE 0x2000 /* MTAģ�������ģ�鹫����Ϣ���� */
#define MTA_NRRC_MSG_TYPE_BASE 0x3000

#define MTA_NV_REFRESH_FAIL_PID_MAX_NUM 64 /* ���Я��64��ģ��NV REFRESHʧ���¼� */

enum MTA_RRC_GsmBand {
    MTA_RRC_GSM_BAND_850 = 0x00,
    MTA_RRC_GSM_BAND_900,
    MTA_RRC_GSM_BAND_1800,
    MTA_RRC_GSM_BAND_1900,

    MTA_RRC_GSM_BAND_BUTT
};
typedef VOS_UINT16 MTA_RRC_GsmBandUint16;


enum MTA_NetmonCellType {
    MTA_NETMON_SCELL_TYPE  = 0,
    MTA_NETMON_NCELL_TYPE  = 1,
    MTA_NETMON_SSCELL_TYPE = 2,
    MTA_NETMON_CELL_TYPE_BUTT
};

typedef VOS_UINT32 MTA_NetMonCellTypeUint32;


enum MTA_NETMON_UtranType {
    MTA_NETMON_UTRAN_FDD_TYPE = 0,
    MTA_NETMON_UTRAN_TDD_TYPE = 1,
    MTA_NETMON_UTRAN_TYPE_BUTT
};
typedef VOS_UINT32 MTA_NETMON_UtranTypeUint32;


enum MTA_NETMON_Result {
    MTA_NETMON_RESULT_NO_ERR = 0,
    MTA_NETMON_RESULT_ERR    = 1,
    MTA_NETMON_RESULT_BUTT
};
typedef VOS_UINT32 MTA_NETMON_ResultUint32;

/*
 * Э����:
 * ASN.1����:
 * ö��˵��: ��ǰ�������ѯС����ģ��ö��ֵ
 */
enum MTA_NETMON_CellQryModule {
    MTA_NETMON_CELL_QRY_MODULE_AT  = 0x00, /* ATģ���������ѯС����Ϣ��AT^MONSC/AT^MONNC���� */
    MTA_NETMON_CELL_QRY_MODULE_MTC = 0x01, /* MTCģ���������ѯС����Ϣ */
    MTA_NRCELL_BAND_QRY_MODULE_AT  = 0x02, /* ATģ���������ѯNR Band��Ϣ��AT^NRCELLBAND���� */
    MTA_NETMON_CELL_QRY_MODULE_BUTT
};
typedef VOS_UINT32 MTA_NETMON_CellQryModuleUint32;

/*
 * Э����:
 * ASN.1����:
 * ö��˵��: NV REFRESH��ԭ��
 */
enum MTA_NvRefreshReason {
    MTA_NV_REFRESH_REASON_USIM_DEPENDENT = 0x00, /* �濨ƥ�� */
    MTA_NV_REFRESH_REASON_BUTT
};
typedef VOS_UINT8 MTA_NvRefreshReasonUint8;

#if (FEATURE_LTEV == FEATURE_ON)

enum TAF_MTA_VModeRatType {
    TAF_MTA_VMODE_RAT_LTEV = 0x00,
    TAF_MTA_VMODE_RAT_BUTT
};
typedef VOS_UINT8 TAF_MTA_VModeRatTypeUint8;


enum TAF_MTA_VModePwModeType {
    TAF_MTA_VMODE_POWEROFF = 0x00,
    TAF_MTA_VMODE_POWERON  = 0x01,
    TAF_MTA_VMODE_PWMODE_BUTT
};
typedef VOS_UINT8 TAF_MTA_VModePwModeTypeUint8;


enum TAF_MTA_ResultCode {
    TAF_MTA_OK                     = 0, /* �ɹ� */
    TAF_MTA_ERR_INPUT_NULL_PTR     = 1, /* ��ָ�� */
    TAF_MTA_ERR_ALLOC_FAIL         = 2, /* �����ڴ�ʧ�� */
    TAF_MTA_ERR_SEND_FAIL          = 3, /* ������Ϣʧ�� */
    TAF_MTA_ERR_TIMER_STATUS_WRONG = 4, /* ��ʱ��״̬���� */
    TAF_MTA_ERR_PARA_INCORRECT     = 5, /* �������� */
    TAF_MTA_ERR_TIMER_EXPIRED      = 6, /* ��ʱ����ʱ */
    TAF_MTA_ERR_UNKNOWN            = 7, /* δ֪���� */
};
#endif
enum MTA_COMM_MsgType {
    /* MTA->CC, MTA->RABM, MTA->VC, MTA->TAF, MTA->DSM, MTA->CCM, MTA->MMA, MTA->IMSA, MTA->MSCC, MTA->CSS,
       MTA->ERRC, MTA->LAPDM, MTA->RLC, MTA->LL, MTA->ERRC, MTA->LMM */
    /* _MSGPARSE_Interception MTA_NV_RefreshInd */
    ID_MTA_NV_REFRESH_IND = MTA_COMM_MSG_TYPE_BASE + 0x01,
    /* CC->MTA, RABM->MTA, VC->MTA, TAF->MTA, DSM->MTA, CCM->MTA */
    /* _MSGPARSE_Interception MTA_NV_RefreshRsp */
    ID_MTA_NV_REFRESH_RSP = MTA_COMM_MSG_TYPE_BASE + 0x02,
    /* A_TAF->MTA */
#if (FEATURE_LTEV == FEATURE_ON)
    /* _MSGPARSE_Interception TAF_MTA_VMODE_SetReq */
    ID_TAF_MTA_VMODE_SET_REQ = MTA_COMM_MSG_TYPE_BASE + 0x03, /* VMODE���ػ��������� */
    /* MTA->AT */
    /* _MSGPARSE_Interception TAF_MTA_VMODE_SetCnf */
    ID_TAF_MTA_VMODE_SET_CNF = MTA_COMM_MSG_TYPE_BASE + 0x04, /* VMODE���ػ����ûظ� */
    /* A_TAF->MTA */
    /* _MSGPARSE_Interception TAF_MTA_VMODE_QryReq */
    ID_TAF_MTA_VMODE_QRY_REQ = MTA_COMM_MSG_TYPE_BASE + 0x05, /* VMODE���ػ���ѯ���� */
    /* MTA->AT */
    /* _MSGPARSE_Interception TAF_MTA_VMODE_QryCnf */
    ID_TAF_MTA_VMODE_QRY_CNF = MTA_COMM_MSG_TYPE_BASE + 0x06, /* VMODE���ػ���ѯ�ظ� */
#endif
    /* MTA->MMC, MTA->LRRC,, MTA->NRRC, */
    /* MTA_UeAiModeStatusNtf */
    /* _MSGPARSE_Interception MTA_UeAiModeStatusNtf */
    ID_MTA_UE_AI_MODE_STATUS_NTF = MTA_COMM_MSG_TYPE_BASE + 0x07,
    ID_MTA_COMM_MSG_TYPE_BUTT

};
typedef VOS_UINT32 MTA_COMM_MsgTypeUint32;

/* ========�����ǽ������MTA֮�����Ϣ�ṹ��======== */
/* *************************network monitor��ѯ������Ϣ�ṹ�岿��************************************************ */
/* MTA������������Ϣ��ͨ�ã�������2G/3G/����С��/���� */
typedef struct {
    VOS_MSG_HEADER                      /* ��Ϣͷ */ /* _H2ASN_Skip */
    VOS_UINT32               msgName;   /* ��Ϣ���� */ /* _H2ASN_Skip */
    MTA_NetMonCellTypeUint32 celltype;  /* 0:��ѯ����С����1:��ѯ���� */
    VOS_UINT32               qryModule; /* ��ǰ��ѯ��ģ����Ϣ */
} MTA_RRC_NetmonCellQryReq;

/* GSM �����ṹ */
typedef struct {
    VOS_UINT32            opBsic : 1;
    VOS_UINT32            opCellId : 1;
    VOS_UINT32            opLac : 1;
    VOS_UINT32            opSpare : 29;
    VOS_UINT32            cellId; /* С��ID */
    VOS_UINT16            lac;    /* λ������ */
    VOS_UINT16            afrcn;  /* Ƶ�� */
    VOS_INT16             rssi;   /* Ƶ���RSSI */
    MTA_RRC_GsmBandUint16 band;   /* band 0-3 */
    VOS_UINT8             bsic;   /* С����վ�� */
    VOS_UINT8             reserved[3];
} MTA_NetmonGsmNcellInfo;

/* LTE �����ṹ,��ʱ��������ݽṹ��������Ҫ���е��� */
typedef struct {
    VOS_UINT32 pid;   /* ����С��ID */
    VOS_UINT32 arfcn; /* Ƶ�� */
    VOS_INT16  rsrp;  /* RSRP�ο��źŽ��չ��� */
    VOS_INT16  rsrq;  /* RSRQ�ο��źŽ������� */
    VOS_INT16  rssi;  /* Receiving signal strength in dbm */
    VOS_UINT16 bandInd; /* Ƶ��ָʾ */
} MTA_NETMON_LteNcellInfo;

/* WCDMA �����ṹ */
typedef struct {
    VOS_UINT16 arfcn; /* Ƶ�� */
    VOS_UINT16 psc;   /* ������ */
    VOS_INT16  ecn0;  /* ECN0 */
    VOS_INT16  rscp;  /* RSCP */
} MTA_NETMON_UtranNcellInfoFdd;

/* TD_SCDMA �����ṹ,��ʱ����ṹ��������Ҫ�������� */
typedef struct {
    VOS_UINT16 arfcn;  /* Ƶ�� */
    VOS_UINT16 sc;     /* ���� */
    VOS_UINT16 syncId; /* ���е�Ƶ�� */
    VOS_INT16  rscp;   /* RSCP */
} MTA_NETMON_UtranNcellInfoTdd;

/* NR �����ṹ */
typedef struct {
    VOS_UINT32 pid;   /* ����С��ID */
    VOS_UINT32 arfcn; /* Ƶ�� */
    VOS_INT16  rsrp;  /* RSRP�ο��źŽ��չ��� */
    VOS_INT16  rsrq;  /* RSRQ�ο��źŽ������� */
    VOS_INT16  sinr;
    VOS_UINT16 bandInd; /* Ƶ��ָʾ */
} MTA_NETMON_NrNcellInfo;

/* �������ݽṹ */
typedef struct {
    VOS_UINT8 gsmNCellCnt;   /* GSM �������� */
    VOS_UINT8 utranNCellCnt; /* WCDMA �������� */
    VOS_UINT8 lteNCellCnt;   /* LTE �������� */
    VOS_UINT8 nrNCellCnt;    /* NR �������� */
    /* GSM �������ݽṹ */
    MTA_NetmonGsmNcellInfo     gsmNCellInfo[NETMON_MAX_GSM_NCELL_NUM];
    MTA_NETMON_UtranTypeUint32 cellMeasTypeChoice; /* NETMONƵ����Ϣ����:FDD,TDD */
    union {
        /* FDD�������ݽṹ */
        MTA_NETMON_UtranNcellInfoFdd fddNCellInfo[NETMON_MAX_UTRAN_NCELL_NUM];
        /*  TDD�������ݽṹ */
        MTA_NETMON_UtranNcellInfoTdd tddNCellInfo[NETMON_MAX_UTRAN_NCELL_NUM];
    } u;
    /* LTE �������ݽṹ */
    MTA_NETMON_LteNcellInfo lteNCellInfo[NETMON_MAX_LTE_NCELL_NUM];
    /* NR �������ݽṹ */
    MTA_NETMON_NrNcellInfo nrNCellInfo[NETMON_MAX_NR_NCELL_NUM];
} RRC_MTA_NetmonNcellInfo;

/* ********************network monitor��ѯGSM С����Ϣ����*********************************************** */
typedef struct {
    VOS_MSG_HEADER          /* ��Ϣͷ */ /* _H2ASN_Skip */
    VOS_UINT32     msgName; /* ��Ϣ���� */ /* _H2ASN_Skip */
} MTA_GRR_NetmonTaQryReq;


typedef struct {
    VOS_MSG_HEADER                       /* ��Ϣͷ */ /* _H2ASN_Skip */
    VOS_UINT32              msgName;     /* ��Ϣ���� */ /* _H2ASN_Skip */
    MTA_NETMON_ResultUint32 result;      /* ���ؽ�� */
    VOS_UINT16              ta;          /* ���ص�TAֵ */
    VOS_UINT8               reserved[2]; /* ���ֽڶ���ı���λ */
} GRR_MTA_NetmonTaQryCnf;


typedef struct {
    VOS_UINT32            mcc;    /* �ƶ������� */
    VOS_UINT32            mnc;    /* �ƶ������� */
    VOS_UINT32            cellId; /* С��ID */
    VOS_UINT16            lac;    /* λ������ */
    VOS_UINT16            arfcn;  /* ����Ƶ��� */
    VOS_INT16             rssi;   /* Receiving signal strength in dbm */
    MTA_RRC_GsmBandUint16 band;   /* GSMƵ��(0-3) */
    VOS_UINT8             bsic;   /* С����վ�� */
    /* IDLE̬�»���PS����̬����Ч,ר��̬������ŵ�����ֵ����Χ[0,7] ,��Чֵ99 */
    VOS_UINT8 rxQuality;
    VOS_UINT8 reserved[2]; /* ���ֽڶ���ı���λ */
} GRR_MTA_NetmonScellInfo;


typedef struct {
    VOS_MSG_HEADER                         /* _H2ASN_Skip */
    VOS_UINT32               msgName;      /* ��Ϣ���� */ /* _H2ASN_Skip */
    MTA_NETMON_ResultUint32  result;
    MTA_NetMonCellTypeUint32 celltype;     /* 0:��ѯ����С����1:��ѯ���� */
    VOS_UINT32               qryModule;    /* ��ǰ��ѯ��ģ����Ϣ */
    union {
        GRR_MTA_NetmonScellInfo scellinfo; /* GSM�µķ���С����Ϣ */
        RRC_MTA_NetmonNcellInfo ncellinfo; /* GSM�µ�������Ϣ */
    } u;
} GRR_MTA_NetmonCellQryCnf;

/* *****************************network monitor��ѯUTRAN С����Ϣ����************************************************/
/* FDD ����С����Ϣ�ṹ */
typedef struct {
    VOS_UINT32 opDrx : 1;
    VOS_UINT32 opUra : 1;
    VOS_UINT32 opSpare : 30;
    VOS_UINT32 drx;  /* Discontinuous reception cycle length */
    VOS_INT16  ecn0; /* ECN0 */
    VOS_INT16  rssi; /* Receiving signal strength in dbm */
    VOS_INT16  rscp; /* Received Signal Code Power in dBm�������ź��빦�� */
    VOS_UINT16 psc;  /* ������ */
    VOS_UINT16 ura;  /* UTRAN Registration Area Identity */
    VOS_UINT8  reserved[2];

} MTA_NETMON_UtranScellInfoFdd;

typedef struct {
    VOS_UINT32 drx;    /* Discontinuous reception cycle length */
    VOS_UINT16 sc;     /* ���� */
    VOS_UINT16 syncID; /* ���е�Ƶ�� */
    VOS_UINT16 rac;    /* RAC */
    VOS_INT16  rscp;   /* RSCP */
} MTA_NETMON_UtranScellInfoTdd;

/* UTRAN ����С���ṹ */
typedef struct {
    VOS_UINT32                 opCellID : 1;
    VOS_UINT32                 opLAC : 1;
    VOS_UINT32                 opSpare : 30;
    VOS_UINT32                 mcc;                /* �ƶ������� */
    VOS_UINT32                 mnc;                /* �ƶ������� */
    VOS_UINT32                 cellID;             /* С��ID */
    VOS_UINT16                 arfcn;              /* Ƶ�� */
    VOS_UINT16                 lac;                /* λ������ */
    MTA_NETMON_UtranTypeUint32 cellMeasTypeChoice; /* NETMONƵ����Ϣ����:FDD,TDD */
    union {
        MTA_NETMON_UtranScellInfoFdd cellMeasResultsFDD; /* FDD */
        MTA_NETMON_UtranScellInfoTdd cellMeasResultsTDD; /*  TDD */
    } u;
} RRC_MTA_NetmonUtranScellInfo;


typedef struct {
    VOS_MSG_HEADER                      /* ��Ϣͷ */ /* _H2ASN_Skip */
    VOS_UINT32               msgName;   /* ��Ϣ���� */ /* _H2ASN_Skip */
    MTA_NETMON_ResultUint32  result;
    MTA_NetMonCellTypeUint32 celltype;  /* 0:��ѯ����С����1:��ѯ���� */
    VOS_UINT32               qryModule; /* ��ǰ��ѯ��ģ����Ϣ */
    union {
        RRC_MTA_NetmonUtranScellInfo scellinfo; /* UTRAN�µķ���С����Ϣ */
        RRC_MTA_NetmonNcellInfo      ncellinfo; /* UTRAN�µ�������Ϣ */
    } u;
} RRC_MTA_NetmonCellInfoQryCnf;

/* LTE ����С���ṹ */
typedef struct {
    VOS_UINT32 mcc;    /* �ƶ������� */
    VOS_UINT32 mnc;    /* �ƶ������� */
    VOS_UINT32 cellID; /* С��ID */
    VOS_UINT32 pid;    /* ����С��ID */
    VOS_UINT32 arfcn;  /* Ƶ�� */
    VOS_UINT16 tac;
    VOS_INT16  rsrp;
    VOS_INT16  rsrq;
    VOS_INT16  rssi;
} MTA_NETMON_EutranScellInfo;


typedef struct {
    VOS_MSG_HEADER                      /* ��Ϣͷ */ /* _H2ASN_Skip */
    VOS_UINT32               msgName;   /* ��Ϣ���� */ /* _H2ASN_Skip */
    MTA_NetMonCellTypeUint32 celltype;  /* 0����ѯ����С����1����ѯ���� */
    VOS_UINT32               qryModule; /* ��ǰ��ѯ��ģ����Ϣ */
} MTA_LRRC_NetmonCellQryReq;


typedef struct {
    VOS_MSG_HEADER                      /* ��Ϣͷ */ /* _H2ASN_Skip */
    VOS_UINT32               msgName;   /* ��Ϣ���� */ /* _H2ASN_Skip */
    MTA_NETMON_ResultUint32  result;
    MTA_NetMonCellTypeUint32 celltype;  /* 0:��ѯ����С����1:��ѯ���� */
    VOS_UINT32               qryModule; /* ��ǰ��ѯ��ģ����Ϣ */
    union {
        MTA_NETMON_EutranScellInfo scellinfo; /* LTE�µķ���С����Ϣ */
        RRC_MTA_NetmonNcellInfo    ncellinfo; /* LTE�µ�������Ϣ */
    } u;
} LRRC_MTA_NetmonCellInfoQryCnf;


typedef struct {
    VOS_MSG_HEADER                    /* _H2ASN_Skip */
    VOS_UINT32               msgName; /* _H2ASN_Skip */
    MTA_NvRefreshReasonUint8 reason;  /* NV REFRESHԭ�� */
    VOS_UINT8                rsv[3];
} MTA_NV_RefreshInd;


typedef struct {
    VOS_MSG_HEADER          /* _H2ASN_Skip */
    VOS_UINT32     msgName; /* _H2ASN_Skip */
    VOS_UINT32     rslt;    /* VOS_OK:�ɹ�������:ʧ�� */
} MTA_NV_RefreshRsp;

typedef struct {
    VOS_UINT8      pidNum;                                  /* NV REFRESHʧ���¼�PID���� */
    VOS_UINT8      rsv[3];                                  /* ����λ */
    VOS_UINT32     pid[MTA_NV_REFRESH_FAIL_PID_MAX_NUM]; /* NV REFRESHʧ���¼�PID */
} MTA_NV_RefreshPidInfo;

#if (FEATURE_LTEV == FEATURE_ON)
/*
 * ��ǰTAF_MTA_Ctrl�ṹֻ��VMODE_SetReq ��VMODE_QryReqʹ�ã�����׼��ɾ��
 */
typedef struct {
    VOS_UINT16 clientId; /* moduleId��AT�ģ�����AT��Ӧ��clientId����AT�ģ���0 */
    VOS_UINT8  opId;
    VOS_UINT8  reserved;

    /* AT����MTA��Ϣʱ���Ȼ�ȡclientid���˴���moduleIdŲ���·� */
    VOS_UINT32 moduleId; /* ����PID */
} TAF_MTA_Ctrl;

typedef struct {
    VOS_MSG_HEADER                        /* _H2ASN_Skip */
    VOS_UINT32                   msgName; /* _H2ASN_Skip */
    TAF_MTA_Ctrl                 ctrl;    /* ���ƶ˿���Ϣ */
    TAF_MTA_VModeRatTypeUint8    rat;     /* ��ʱֻ֧��LTE��ʽ */
    TAF_MTA_VModePwModeTypeUint8 mode;    /* 0��ʾ�ػ���1��ʾ���� */
    VOS_UINT8                    reserved[2];
} TAF_MTA_VMODE_SetReq;


typedef struct {
    VOS_MSG_HEADER          /* _H2ASN_Skip */
    VOS_UINT32     msgName; /* _H2ASN_Skip */
    TAF_MTA_Ctrl   ctrl;    /* ���ƶ˿���Ϣ */
    VOS_UINT32     result;  /* TAF_MTA_OK:�ɹ�������:ʧ�� */
} TAF_MTA_VMODE_SetCnf;


typedef struct {
    VOS_MSG_HEADER          /* _H2ASN_Skip */
    VOS_UINT32     msgName; /* _H2ASN_Skip */
    TAF_MTA_Ctrl   ctrl;    /* ���ƶ˿���Ϣ */
} TAF_MTA_VMODE_QryReq;


typedef struct {
    VOS_MSG_HEADER                        /* _H2ASN_Skip */
    VOS_UINT32                   msgName; /* _H2ASN_Skip */
    TAF_MTA_Ctrl                 ctrl;    /* ���ƶ˿���Ϣ */
    VOS_UINT32                   result;  /* TAF_MTA_OK:�ɹ�������:ʧ�� */
    TAF_MTA_VModePwModeTypeUint8 mode;    /* 0��ʾ�ػ���1��ʾ���� */
    VOS_UINT8                    reserved[3];
} TAF_MTA_VMODE_QryCnf;
#endif


typedef struct {

    VOS_UINT16                originalId;
    VOS_UINT16                terminalId;
    VOS_UINT32                timeStamp;
    VOS_UINT32                sn;
} MTA_OmHeader;


typedef struct {
    VOS_UINT32                moudleId;
    union {
        AT_APPCTRL            appHeader;
        MTA_OmHeader          omHeader;
    } u;

} MTA_AtOmCommHeader;


enum MTA_UeAiModeRegType {
    MTA_UE_AI_MODE_REG_TYPE_MOTION = 0x00, /* �˶�״̬ע�� */
    MTA_UE_AI_MODE_REG_TYPE_SCENE  = 0x01, /* ����״̬ע�� */
    MTA_UE_AI_MODE_REG_TYPE_BUTT
};
typedef VOS_UINT32 MTA_UeAiModeRegTypeUint32;


enum MTA_MotionStatSupport {
    MTA_MOTION_STAT_SUPPORT_UNKNOWN         = 0x00000000, /* UNKNOWN״̬ */
    MTA_MOTION_STAT_SUPPORT_IN_VEHICLE      = 0x00000001, /* ֧�ֳ��� */
    MTA_MOTION_STAT_SUPPORT_RIDING          = 0x00000002, /* ֧���ﳵ */
    MTA_MOTION_STAT_SUPPORT_WALK_SLOW       = 0x00000004, /* ֧������ */
    MTA_MOTION_STAT_SUPPORT_RUN_FAST        = 0x00000008, /* ֧�ֿ��� */
    MTA_MOTION_STAT_SUPPORT_STATIONARY      = 0x00000010, /* ֧�־�ֹ */
    MTA_MOTION_STAT_SUPPORT_TILT            = 0x00000020, /* ֧����б */
    MTA_MOTION_STAT_SUPPORT_END             = 0x00000040,
    MTA_MOTION_STAT_SUPPORT_BUS             = 0x00000080, /* ֧�ֹ��� */
    MTA_MOTION_STAT_SUPPORT_CAR             = 0x00000100, /* ֧��С�� */
    MTA_MOTION_STAT_SUPPORT_METRO           = 0x00000200, /* ֧�ֵ��� */
    MTA_MOTION_STAT_SUPPORT_HIGH_SPEED_RAIL = 0x00000400, /* ֧�ָ��� */
    MTA_MOTION_STAT_SUPPORT_AUTO            = 0x00000800, /* ֧�ֹ�·��ͨ */
    MTA_MOTION_STAT_SUPPORT_RAIL            = 0x00001000, /* ֧����·��ͨ */
    MTA_MOTION_STAT_SUPPORT_CLIMBING_MOUNT  = 0x00002000, /* ֧����ɽ */
    MTA_MOTION_STAT_SUPPORT_FAST_WALK       = 0x00004000, /* ֧�ֿ��� */
    MTA_MOTION_STAT_SUPPORT_STOP_VEHICLE    = 0x00008000, /* ֧��ͣ�� */
    MTA_MOTION_STAT_SUPPORT_ELEVATOR        = 0x00010000, /* ֧�ֵ��� */
    MTA_MOTION_STAT_SUPPORT_RELATIVE_STILL  = 0x00020000, /* ֧��΢�� */
    MTA_MOTION_STAT_SUPPORT_GARAGE          = 0x00040000, /* ֧�ֳ��� */
};
typedef VOS_UINT32 MTA_MotionStatSupportUint32;


enum MTA_UeSceneStat {
    MTA_UE_SCENE_STAT_NORMAL = 0x00000000,
    MTA_UE_SCENE_STAT_SLEEP  = 0x00000001, /* ����˯��ģʽ */
    MTA_UE_SCENE_STAT_BUTT
};
typedef VOS_UINT32 MTA_UeSceneStatUint32;


typedef struct {
    VOS_MSG_HEADER      /* _H2ASN_Skip */
    VOS_UINT32 msgName; /* _H2ASN_Skip */
    VOS_UINT32 motionStatus; /* 0Ϊ��ͨ״̬����0ȡֵ�ο�MTA_MotionStatSupportUint32ö��ֵ����(��λ��) */
    VOS_UINT32 sceneStatus;  /* 0Ϊ��ͨ״̬����0ȡֵ�ο�MTA_UeSceneStatUint32ö��ֵ����(��λ��) */
} MTA_UeAiModeStatusNtf;

VOS_UINT32 NAS_RegNvRefreshIndMsg(ModemIdUint16 modemId, VOS_UINT32 regPid);

/*
 * 1. MODEM_FUSION_VERSION�꿪ʱNRRCʹ��ע����ƣ����ʱ����ע��Ĭ�ϸ�NRRC������Ϣ
 * 2. ʹ��ע����Ƶ������Ҫ֪��TAF������Ϣ��������δ֪��ʹ��ע�����TAF��������Ϣ��������
 */
VOS_UINT32 TAF_MTA_RegUeAiModeStatusNtfMsg(ModemIdUint16 modemId, VOS_UINT32 regPid,
    MTA_UeAiModeRegTypeUint32 regModeStatus);

#if (FEATURE_LTEV == FEATURE_ON)
VOS_UINT32 TAF_MTA_VModeSetReq(TAF_MTA_Ctrl *ctrl, TAF_MTA_VModeRatTypeUint8 rat, TAF_MTA_VModePwModeTypeUint8 mode);
VOS_UINT32 TAF_MTA_VModeQryReq(TAF_MTA_Ctrl *ctrl);
#endif

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of mta_comm_interface.h */
