

#ifndef __MAC_COMMON_H__
#define __MAC_COMMON_H__

/* 1 ����ͷ�ļ����� */
#include "oal_types.h"
#include "wlan_types.h"
#include "hal_common.h"
#include "mac_device_common.h"
#include "mac_user_common.h"
#include "mac_vap_common.h"

// �˴�����extern "C" UT���벻��
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_COMMON_H

#define MAC_TX_CTL_SIZE oal_netbuf_cb_size()

#define DMAC_BA_SEQNO_MASK                0x0FFF /* max sequece number */

#define DMAC_BA_SEQ_ADD(_seq1, _seq2)   (((_seq1) + (_seq2)) & DMAC_BA_SEQNO_MASK)
#define DMAC_BA_SEQ_SUB(_seq1, _seq2)   (((_seq1) - (_seq2)) & DMAC_BA_SEQNO_MASK)
#define DMAC_BA_SEQNO_ADD(_seq1, _seq2) (((_seq1) + (_seq2)) & DMAC_BA_SEQNO_MASK)
#define DMAC_BA_SEQNO_SUB(_seq1, _seq2) (((_seq1) - (_seq2)) & DMAC_BA_SEQNO_MASK)
#define MAC_INVALID_RX_BA_LUT_INDEX HAL_MAX_RX_BA_LUT_SIZE
/* ����BA���ڼ�¼seq number����������������2���������� */
#define DMAC_TID_MAX_BUFS 128
/* ����BA���ڼ�¼seq number��bitmap��ʹ�õ����ͳ��� */
#define DMAC_TX_BUF_BITMAP_WORD_SIZE 32
/* ����BA���ڼ�¼seq number��bit map�ĳ��� */
#define DMAC_TX_BUF_BITMAP_WORDS \
    ((DMAC_TID_MAX_BUFS + DMAC_TX_BUF_BITMAP_WORD_SIZE - 1) / DMAC_TX_BUF_BITMAP_WORD_SIZE)

#define IS_RW_RING_FULL(read, write)                                    \
    (((read)->st_rw_ptr.bit_rw_ptr == (write)->st_rw_ptr.bit_rw_ptr) && \
        ((read)->st_rw_ptr.bit_wrap_flag != (write)->st_rw_ptr.bit_wrap_flag))


/* ����dmac����Ϣͷ�ĳ��� */
#define CUSTOM_MSG_DATA_HDR_LEN (sizeof(custom_cfgid_enum_uint32) + sizeof(uint32_t))

#ifdef _PRE_WLAN_FEATURE_NRCOEX
#define DMAC_WLAN_NRCOEX_INTERFERE_RULE_NUM (4) /* 5gnr������Ų�����Ŀǰ��4�� */
#endif

/* 3 ö�ٶ��� */
/*****************************************************************************
  ö����  : dmac_tx_host_drx_subtype_enum
  Э����:
  ö��˵��: HOST DRX�¼������Ͷ���
*****************************************************************************/
/* WLAN_CRX�����Ͷ��� */
typedef enum {
    DMAC_TX_HOST_DRX = 0,
    HMAC_TX_HOST_DRX = 1,
    HMAC_TX_DMAC_SCH = 2, /* �������¼� */
    DMAC_TX_HOST_DTX = 3, /* H2D����netbuf, device ring txʹ�� */

    DMAC_TX_HOST_DRX_BUTT
} dmac_tx_host_drx_subtype_enum;

/*****************************************************************************
  ö����  : dmac_tx_wlan_dtx_subtype_enum
  Э����:
  ö��˵��: WLAN DTX�¼������Ͷ���
*****************************************************************************/
typedef enum {
    DMAC_TX_WLAN_DTX = 0,

    DMAC_TX_WLAN_DTX_BUTT
} dmac_tx_wlan_dtx_subtype_enum;

/*****************************************************************************
  ö����  : dmac_wlan_ctx_event_sub_type_enum
  Э����:
  ö��˵��: WLAN CTX�¼������Ͷ���
*****************************************************************************/
typedef enum {
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_MGMT = 0, /* ����֡���� */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_ADD_USER,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_NOTIFY_ALG_ADD_USER,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_DEL_USER,

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_BA_SYNC,  /* �յ�wlan��Delba��addba rsp���ڵ�dmac��ͬ�� */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_PRIV_REQ, /* 11N�Զ����������¼����� */

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_SCAN_REQ,       /* ɨ������ */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_SCHED_SCAN_REQ, /* PNO����ɨ������ */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_RESET_PSM,      /* �յ���֤���� �������󣬸�λ�û��Ľ���״̬ */

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_SET_REG,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_CONN_RESULT, /* ������� */

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_ASOC_WRITE_REG, /* AP�ദ�����ʱ���޸�SEQNUM_DUPDET_CTRL�Ĵ��� */

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_EDCA_REG,       /* STA�յ�beacon��assoc rspʱ������EDCA�Ĵ��� */

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_SWITCH_TO_NEW_CHAN, /* �л������ŵ��¼� */
    DMAC_WALN_CTX_EVENT_SUB_TYPR_SELECT_CHAN,        /* �����ŵ��¼� */
    DMAC_WALN_CTX_EVENT_SUB_TYPR_DISABLE_TX,         /* ��ֹӲ������ */
    DMAC_WALN_CTX_EVENT_SUB_TYPR_ENABLE_TX,          /* �ָ�Ӳ������ */
    DMAC_WLAN_CTX_EVENT_SUB_TYPR_RESTART_NETWORK,    /* �л��ŵ��󣬻ָ�BSS������ */
#ifdef _PRE_WLAN_FEATURE_DFS
#ifdef _PRE_WLAN_FEATURE_OFFCHAN_CAC
    DMAC_WLAN_CTX_EVENT_SUB_TYPR_SWITCH_TO_OFF_CHAN,  /* �л���offchan��off-chan cac��� */
    DMAC_WLAN_CTX_EVENT_SUB_TYPR_SWITCH_TO_HOME_CHAN, /* �л�home chan */
#endif
    DMAC_WLAN_CTX_EVENT_SUB_TYPR_DFS_TEST,
    DMAC_WALN_CTX_EVENT_SUB_TYPR_DFS_CAC_CTRL_TX, /* DFS 1min CAC��vap״̬λ��Ϊpause����up,ͬʱ��ֹ���߿���Ӳ������ */
#endif

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    DMAC_WLAN_CTX_EVENT_SUB_TYPR_EDCA_OPT, /* edca�Ż���ҵ��ʶ��֪ͨ�¼� */
#endif
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_CALI_HMAC2DMAC,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_DSCR_OPT,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_CALI_MATRIX_HMAC2DMAC,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_APP_IE_H2D,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_IP_FILTER,
#ifdef _PRE_WLAN_FEATURE_APF
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_APF_CMD,
#endif

#ifdef _PRE_WLAN_FEATURE_11AX
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_MU_EDCA_REG, /* STA�յ�beacon��assoc rspʱ������MU EDCA�Ĵ��� */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_SPATIAL_REUSE_REG,
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_UPDATE_TWT,                      /* STA�յ�twt ʱ�����¼Ĵ��� */
    /* STA�յ�beacon��assoc rspʱ������NDP Feedback report�Ĵ��� */
    DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_NDP_FEEDBACK_REPORT_REG,
#endif

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_HIEX_H2D_MSG,

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_CUST_HMAC2DMAC,

    DMAC_WLAN_CTX_EVENT_SUB_TYPE_BUTT
} dmac_wlan_ctx_event_sub_type_enum;

/* DMACģ�� WLAN_DRX�����Ͷ��� */
typedef enum {
    DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_DATA,        /* APģʽ: DMAC WLAN DRX ���� */
    DMAC_WLAN_DRX_EVENT_SUB_TYPE_TKIP_MIC_FAILE, /* DMAC tkip mic faile �ϱ���HMAC */
    DMAC_WLAN_DRX_EVENT_SUB_TYPE_WOW_RX_DATA,    /* WOW DMAC WLAN DRX ���� */

    DMAC_WLAN_DRX_EVENT_SUB_TYPE_BUTT
} dmac_wlan_drx_event_sub_type_enum;
typedef uint8_t dmac_wlan_drx_event_sub_type_enum_uint8;

/* DMACģ�� WLAN_CRX�����Ͷ��� */
typedef enum {
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_INIT,              /* DMAC �� HMAC�ĳ�ʼ������ */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_RX,                /* DMAC WLAN CRX ���� */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_DELBA,             /* DMAC���������DELBA֡ */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_EVERY_SCAN_RESULT, /* ɨ�赽һ��bss��Ϣ���ϱ���� */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_SCAN_COMP,         /* DMACɨ������ϱ���HMAC */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_OBSS_SCAN_COMP,    /* DMAC OBSSɨ������ϱ���HMAC */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_CHAN_RESULT,       /* �ϱ�һ���ŵ���ɨ���� */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_ACS_RESP,          /* DMAC ACS �ظ�Ӧ�ò�����ִ�н����HMAC */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_DISASSOC,          /* DMAC�ϱ�ȥ�����¼���HMAC, HMAC��ɾ���û� */
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_DEAUTH,            /* DMAC�ϱ�ȥ��֤�¼���HMAC */

    DMAC_WLAN_CRX_EVENT_SUB_TYPR_CH_SWITCH_COMPLETE, /* �ŵ��л�����¼� */
    DMAC_WLAN_CRX_EVENT_SUB_TYPR_DBAC,               /* DBAC enable/disable�¼� */

    DMAC_WLAN_CRX_EVENT_SUB_TYPE_HIEX_D2H_MSG,
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_RX_WOW,
    DMAC_WLAN_CRX_EVENT_SUB_TYPE_EVERY_SCAN_RESULT_RING, /* ring��ʽ���յ�ɨ���������ϱ� */

    DMAC_WLAN_CRX_EVENT_SUB_TYPE_TX_RING_ADDR,
    DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_SNIFFER_INFO,

    DMAC_WLAN_CRX_EVENT_SUB_TYPE_BUTT
} dmac_wlan_crx_event_sub_type_enum;
typedef uint8_t dmac_wlan_crx_event_sub_type_enum_uint8;

/* ����HOST��������¼� */
typedef enum {
    DMAC_TO_HMAC_SYN_UP_REG_VAL = 1,
    DMAC_TO_HMAC_CREATE_BA = 2,
    DMAC_TO_HMAC_DEL_BA = 3,
    DMAC_TO_HMAC_SYN_CFG = 4,

    DMAC_TO_HMAC_ALG_INFO_SYN = 6,
    DMAC_TO_HMAC_VOICE_AGGR = 7,
#if defined(_PRE_WLAN_FEATURE_DATA_SAMPLE) || defined(_PRE_WLAN_FEATURE_PSD_ANALYSIS)
    DMAC_TO_HMAC_SYN_UP_SAMPLE_DATA = 8,
#endif

#ifdef _PRE_WLAN_FEATURE_M2S
    DMAC_TO_HMAC_M2S_DATA = 10,
#endif

    DMAC_TO_HMAC_BANDWIDTH_INFO_SYN = 11,  /* dmac��hmacͬ���������Ϣ */
    DMAC_TO_HMAC_PROTECTION_INFO_SYN = 12, /* dmac��hmacͬ������mib��Ϣ */
    DMAC_TO_HMAC_CH_STATUS_INFO_SYN = 13,  /* dmac��hmacͬ�������ŵ��б� */
#ifdef _PRE_WLAN_FEATURE_FTM
    DMAC_TO_HMAC_FTM_CALI = 14,
#endif
    /* �¼�ע��ʱ����Ҫö��ֵ�г�������ֹ����device���host���Ժ�򿪲�һ�£�
       ��ɳ���ͬ���¼�δע�����⣬��������������ע���򿪵�һ����
    */
    DMAC_TO_HMAC_SYN_BUTT
} dmac_to_hmac_syn_type_enum;

typedef struct {
    uint32_t ftm_cali_time;
    uint8_t vap_id;
} dmac_to_hmac_ftm_ctx_event_stru; /* hd_event */

/* hmac to dmac���ƻ�����ͬ����Ϣ�ṹ */
typedef enum {
    CUSTOM_CFGID_NV_ID = 0,
    CUSTOM_CFGID_INI_ID,
    CUSTOM_CFGID_DTS_ID,
    CUSTOM_CFGID_PRIV_INI_ID,
    CUSTOM_CFGID_CAP_ID,

    CUSTOM_CFGID_BUTT,
} custom_cfgid_enum;
typedef unsigned int custom_cfgid_enum_uint32;

typedef enum {
    CUSTOM_CFGID_INI_ENDING_ID = 0,
    CUSTOM_CFGID_INI_FREQ_ID,
    CUSTOM_CFGID_INI_PERF_ID,
    CUSTOM_CFGID_INI_LINKLOSS_ID,
    CUSTOM_CFGID_INI_PM_SWITCH_ID,
    CUSTOM_CFGID_INI_PS_FAST_CHECK_CNT_ID,
    CUSTOM_CFGID_INI_FAST_MODE,

    /* ˽�ж��� */
    CUSTOM_CFGID_PRIV_INI_RADIO_CAP_ID,
    CUSTOM_CFGID_PRIV_FASTSCAN_SWITCH_ID,
    CUSTOM_CFGID_PRIV_ANT_SWITCH_ID,
    CUSTOM_CFGID_PRIV_LINKLOSS_THRESHOLD_FIXED_ID,
    CUSTOM_CFGID_PRIV_RADAR_ISR_FORBID_ID,
    CUSTOM_CFGID_PRIV_INI_BW_MAX_WITH_ID,
    CUSTOM_CFGID_PRIV_INI_LDPC_CODING_ID,
    CUSTOM_CFGID_PRIV_INI_RX_STBC_ID,
    CUSTOM_CFGID_PRIV_INI_TX_STBC_ID,
    CUSTOM_CFGID_PRIV_INI_SU_BFER_ID,
    CUSTOM_CFGID_PRIV_INI_SU_BFEE_ID,
    CUSTOM_CFGID_PRIV_INI_MU_BFER_ID,
    CUSTOM_CFGID_PRIV_INI_MU_BFEE_ID,
    CUSTOM_CFGID_PRIV_INI_11N_TXBF_ID,
    CUSTOM_CFGID_PRIV_INI_1024_QAM_ID,

    CUSTOM_CFGID_PRIV_INI_CALI_MASK_ID,
    CUSTOM_CFGID_PRIV_CALI_DATA_MASK_ID,
    CUSTOM_CFGID_PRIV_INI_AUTOCALI_MASK_ID,
    CUSTOM_CFGID_PRIV_INI_DOWNLOAD_RATELIMIT_PPS,
    CUSTOM_CFGID_PRIV_INI_TXOPPS_SWITCH_ID,

    CUSTOM_CFGID_PRIV_INI_OVER_TEMPER_PROTECT_THRESHOLD_ID,
    CUSTOM_CFGID_PRIV_INI_TEMP_PRO_ENABLE_ID,
    CUSTOM_CFGID_PRIV_INI_TEMP_PRO_REDUCE_PWR_ENABLE_ID,
    CUSTOM_CFGID_PRIV_INI_TEMP_PRO_SAFE_TH_ID,
    CUSTOM_CFGID_PRIV_INI_TEMP_PRO_OVER_TH_ID,
    CUSTOM_CFGID_PRIV_INI_TEMP_PRO_PA_OFF_TH_ID,

    CUSTOM_CFGID_PRIV_INI_DSSS2OFDM_DBB_PWR_BO_VAL_ID,
    CUSTOM_CFGID_PRIV_INI_EVM_PLL_REG_FIX_ID,
    CUSTOM_CFGID_PRIV_INI_VOE_SWITCH_ID,
    CUSTOM_CFGID_PRIV_COUNTRYCODE_SELFSTUDY_CFG_ID,
    CUSTOM_CFGID_PRIV_M2S_FUNCTION_EXT_MASK_ID,
    CUSTOM_CFGID_PRIV_M2S_FUNCTION_MASK_ID,
    CUSTOM_CFGID_PRIV_MCM_CUSTOM_FUNCTION_MASK_ID,
    CUSTOM_CFGID_PRIV_MCM_FUNCTION_MASK_ID,
    CUSTOM_CFGID_PRIV_INI_11AX_SWITCH_ID,
    CUSTOM_CFGID_PRIV_INI_HTC_SWITCH_ID,
    CUSTOM_CFGID_PRIV_INI_AC_SUSPEND_ID,
    CUSTOM_CFGID_PRIV_INI_MBSSID_SWITCH_ID,
    CUSTOM_CFGID_PRIV_DYN_BYPASS_EXTLNA_ID,
    CUSTOM_CFGID_PRIV_CTRL_FRAME_TX_CHAIN_ID,

    CUSTOM_CFGID_PRIV_CTRL_UPC_FOR_18DBM_C0_ID,
    CUSTOM_CFGID_PRIV_CTRL_UPC_FOR_18DBM_C1_ID,
    CUSTOM_CFGID_PRIV_CTRL_11B_DOUBLE_CHAIN_BO_POW_ID,
    CUSTOM_CFGID_PRIV_INI_LDAC_M2S_TH_ID,
    CUSTOM_CFGID_PRIV_INI_BTCOEX_MCM_TH_ID,
    CUSTOM_CFGID_PRIV_INI_NRCOEX_ID,
    CUSTOM_CFGID_PRIV_INI_HCC_FLOWCTRL_TYPE_ID,
    CUSTOM_CFGID_PRIV_INI_MBO_SWITCH_ID,

    CUSTOM_CFGID_PRIV_INI_DYNAMIC_DBAC_ID,
    CUSTOM_CFGID_PRIV_INI_PHY_CAP_SWITCH_ID,
    CUSTOM_CFGID_PRIV_DC_FLOWCTRL_ID,
    CUSTOM_CFGID_PRIV_INI_HAL_PS_RSSI_ID,
    CUSTOM_CFGID_PRIV_INI_HAL_PS_PPS_ID,
    CUSTOM_CFGID_PRIV_INI_OPTIMIZED_FEATURE_SWITCH_ID,
    CUSTOM_CFGID_PRIV_DDR_SWITCH_ID,
    CUSTOM_CFGID_PRIV_FEM_POW_CTL_ID,
    CUSTOM_CFGID_PRIV_INI_HIEX_CAP_ID,
    CUSTOM_CFGID_PRIV_INI_TX_SWITCH_ID,
    CUSTOM_CFGID_PRIV_INI_FTM_CAP_ID,
    CUSTOM_CFGID_PRIV_INI_MIRACAST_SINK,
    CUSTOM_CFGID_PRIV_INI_MCAST_AMPDU_ENABLE_ID,

    CUSTOM_CFGID_INI_BUTT,
} custom_cfgid_h2d_ini_enum;
typedef unsigned int custom_cfgid_h2d_ini_enum_uint32;

enum custom_optimize_feature {
    CUSTOM_OPTIMIZE_FEATURE_RA_FLT = 0,
    CUSTOM_OPTIMIZE_FEATURE_NAN = 1,
    CUSTOM_OPTIMIZE_FEATURE_CERTIFY_MODE = 2,
    CUSTOM_OPTIMIZE_TXOP_COT = 3,
    CUSTOM_OPTIMIZE_11AX20M_NON_LDPC = 4, /* ��֤ʱap 20M ��֧��LDPC���Թ�����ax */
    CUSTOM_OPTIMIZE_CHBA = 5, /* CHBA�������� */
    CUSTOM_OPTIMIZE_CE_IDLE = 6, /* CE��֤���������ر�pk mode���������������� */
    CUSTOM_OPTIMIZE_CSA_EXT = 7, /* CSA��չ ��Ƶ�ι��� ���� */
    CUSTOM_OPTIMIZE_FEATURE_BUTT,
};

typedef struct {
    custom_cfgid_enum_uint32 en_syn_id; /* ͬ������ID */
    uint32_t len;                  /* DATA payload���� */
    uint8_t auc_msg_body[NUM_4_BYTES];          /* DATA payload */
} hmac_to_dmac_cfg_custom_data_stru;

/* MISC��ɢ�¼� */
typedef enum {
    DMAC_MISC_SUB_TYPE_RADAR_DETECT,
    DMAC_MISC_SUB_TYPE_DISASOC,
    DMAC_MISC_SUB_TYPE_CALI_TO_HMAC,
    DMAC_MISC_SUB_TYPE_HMAC_TO_CALI,

    DMAC_MISC_SUB_TYPE_ROAM_TRIGGER,
    DMAC_TO_HMAC_DPD,

#ifdef _PRE_WLAN_FEATURE_APF
    DMAC_MISC_SUB_TYPE_APF_REPORT,
#endif
    HAL_EVENT_HAL_RX_RESET_SMAC,
    DMAC_MISC_SUB_TYPE_TX_SWITCH_STATE,

    DMAC_MISC_SUB_TYPE_BUTT
} dmac_misc_sub_type_enum;

typedef enum {
    DMAC_DISASOC_MISC_LINKLOSS = 0,
    DMAC_DISASOC_MISC_KEEPALIVE = 1,
    DMAC_DISASOC_MISC_CHANNEL_MISMATCH = 2,
    DMAC_DISASOC_MISC_LOW_RSSI = 3,
    DMAC_DISASOC_MISC_GET_CHANNEL_IDX_FAIL = 5,
    DMAC_DISASOC_MISC_BUTT
} dmac_disasoc_misc_reason_enum;
typedef uint16_t dmac_disasoc_misc_reason_enum_uint16;

/* HMAC to DMACͬ������ */
typedef enum {
    HMAC_TO_DMAC_SYN_INIT,
    HMAC_TO_DMAC_SYN_CREATE_CFG_VAP,
    HMAC_TO_DMAC_SYN_CFG,
    HMAC_TO_DMAC_SYN_ALG,
    HMAC_TO_DMAC_SYN_REG,
#if defined(_PRE_WLAN_FEATURE_DATA_SAMPLE) || defined(_PRE_WLAN_FEATURE_PSD_ANALYSIS)
    HMAC_TO_DMAC_SYN_SAMPLE,
#endif

    HMAC_TO_DMAC_SYN_BUTT
} hmac_to_dmac_syn_type_enum;
typedef uint8_t hmac_to_dmac_syn_type_enum_uint8;

/* BA�Ự��״̬ö�� */
typedef enum {
    DMAC_BA_INIT = 0,   /* BA�Ựδ���� */
    DMAC_BA_INPROGRESS, /* BA�Ự���������� */
    DMAC_BA_COMPLETE,   /* BA�Ự������� */
    DMAC_BA_HALTED,     /* BA�Ự������ͣ */
    DMAC_BA_FAILED,     /* BA�Ự����ʧ�� */

    DMAC_BA_BUTT
} dmac_ba_conn_status_enum;
typedef uint8_t dmac_ba_conn_status_enum_uint8;

/* ר����CB�ֶ��Զ���֡���͡�֡������ö��ֵ */
typedef enum {
    WLAN_CB_FRAME_TYPE_START = 0,  /* cbĬ�ϳ�ʼ��Ϊ0 */
    WLAN_CB_FRAME_TYPE_ACTION = 1, /* action֡ */
    WLAN_CB_FRAME_TYPE_DATA = 2,   /* ����֡ */
    WLAN_CB_FRAME_TYPE_MGMT = 3,   /* ����֡������p2p����Ҫ�ϱ�host */

    WLAN_CB_FRAME_TYPE_BUTT
} wlan_cb_frame_type_enum;
typedef uint8_t wlan_cb_frame_type_enum_uint8;

/* cb�ֶ�action֡������ö�ٶ��� */
typedef enum {
    WLAN_ACTION_BA_ADDBA_REQ = 0, /* �ۺ�action */
    WLAN_ACTION_BA_ADDBA_RSP = 1,
    WLAN_ACTION_BA_DELBA = 2,
#ifdef _PRE_WLAN_FEATURE_WMMAC
    WLAN_ACTION_BA_WMMAC_ADDTS_REQ = 3,
    WLAN_ACTION_BA_WMMAC_ADDTS_RSP = 4,
    WLAN_ACTION_BA_WMMAC_DELTS = 5,
#endif
    WLAN_ACTION_SMPS_FRAME_SUBTYPE = 6,   /* SMPS����action */
    WLAN_ACTION_OPMODE_FRAME_SUBTYPE = 7, /* ����ģʽ֪ͨaction */
    WLAN_ACTION_P2PGO_FRAME_SUBTYPE = 8,  /* host���͵�P2P go֡����Ҫ��discoverability request */

#ifdef _PRE_WLAN_FEATURE_TWT
    WLAN_ACTION_TWT_SETUP_REQ = 9,
    WLAN_ACTION_TWT_TEARDOWN_REQ = 10,
#endif
#ifdef _PRE_WLAN_FEATURE_HIEX
    WLAN_ACTION_HIEX = 11,
#endif
#ifdef _PRE_WLAN_FEATURE_NAN
    WLAN_ACTION_NAN_PUBLISH = 12,
    WLAN_ACTION_NAN_FLLOWUP = 13,
    WLAN_ACTION_NAN_SUBSCRIBE = 14,
#endif
#ifdef _PRE_WLAN_FEATURE_FTM
    WLAN_ACTION_FTM_REQUEST = 15,
    WLAN_ACTION_FTM_RESPONE = 16,
#endif
    WLAN_FRAME_TYPE_ACTION_BUTT
} wlan_cb_action_subtype_enum;
typedef uint8_t wlan_cb_frame_subtype_enum_uint8;

/* ���������������ö�� */
typedef enum {
    WLAN_BANDWITH_CAP_20M,
    WLAN_BANDWITH_CAP_40M,
    WLAN_BANDWITH_CAP_40M_DUP,
    WLAN_BANDWITH_CAP_80M,
    WLAN_BANDWITH_CAP_80M_DUP,
    WLAN_BANDWITH_CAP_160M,
    WLAN_BANDWITH_CAP_160M_DUP,
    WLAN_BANDWITH_CAP_80PLUS80,
#ifdef  _PRE_WLAN_FEATURE_11AX_ER_SU_DCM
    WLAN_BANDWITH_CAP_ER_242_TONE,
    WLAN_BANDWITH_CAP_ER_106_TONE,
#endif
    WLAN_BANDWITH_CAP_BUTT
} wlan_bandwith_cap_enum;
typedef uint8_t wlan_bandwith_cap_enum_uint8;

typedef enum {
    DMAC_CHANNEL1    = 1,
    DMAC_CHANNEL11   = 11,
    DMAC_CHANNEL12   = 12,
    DMAC_CHANNEL36   = 36,
    DMAC_CHANNEL48   = 48,
    DMAC_CHANNEL149  = 149,
    DMAC_CHANNEL165  = 165,
    DMAC_PC_PASSIVE_CHANNEL_BUTT
} dmac_pc_passive_channel_enum;

typedef hal_rx_ctl_stru mac_rx_ctl_stru;

/* DMACģ��������̿�����Ϣ�ṹ������ڶ�Ӧ��netbuf��CB�ֶ��У����ֵΪ48�ֽ�,
   ����޸ģ�һ��Ҫ֪ͨsdtͬ���޸ģ�����������д���!!!!!!!!!!!!!!!!!!!!!!!!! */
typedef struct {
    hal_rx_ctl_stru st_rx_info;            /* dmac��Ҫ����hmac��������Ϣ */
    hal_rx_status_stru st_rx_status;       /* ����������ͼ�֡����Ϣ */
    hal_rx_statistic_stru st_rx_statistic; /* ���������������ͳ����Ϣ */
} dmac_rx_ctl_stru;


/* 1�ֽڶ��� */
#pragma pack(push, 1)
typedef struct {
    mac_ieee80211_frame_stru *pst_frame_header; /* ��MPDU��֡ͷָ�� */
    uint16_t us_seqnum;                       /* ��¼��������seqnum */
    wlan_frame_type_enum_uint8 en_frame_type;   /* ��֡�ǿ����롢����֡������֡ */
    uint8_t bit_80211_mac_head_type : 1;      /* 0: 802.11 macͷ����skb��,�����������ڴ���;1:802.11 macͷ��skb�� */
    uint8_t en_res : 7;                       /* �Ƿ�ʹ��4��ַ����WDS���Ծ��� */
} mac_tx_expand_cb_stru;

/* ��ϵͳcb�ֶ� ֻ��20�ֽڿ���, ��ǰʹ��19�ֽ�; HCC[8]+PAD[1]+CB[19]+MAC HEAD[36] */
struct mac_tx_ctl {
    /* byte1 */
    /* ȡֵ:FRW_EVENT_TYPE_WLAN_DTX��FRW_EVENT_TYPE_HOST_DRX������:���ͷ�ʱ�������ڴ�ص�netbuf����ԭ��̬�� */
    uint8_t bit_event_type : 5;
    uint8_t bit_event_sub_type : 3;
    /* byte2-3 */
    wlan_cb_frame_type_enum_uint8 uc_frame_type;       /* �Զ���֡���� */
    wlan_cb_frame_subtype_enum_uint8 uc_frame_subtype; /* �Զ���֡������ */
    /* byte4 */
    uint8_t bit_mpdu_num : 7;   /* ampdu�а�����MPDU����,ʵ����������д��ֵΪ��ֵ-1 */
    uint8_t bit_netbuf_num : 1; /* ÿ��MPDUռ�õ�netbuf��Ŀ */
    /* ��ÿ��MPDU�ĵ�һ��NETBUF����Ч */
    /* byte5-6 */
    uint16_t us_mpdu_payload_len; /* ÿ��MPDU�ĳ��Ȳ�����mac header length */
    /* byte7 */
    uint8_t bit_frame_header_length : 6; /* 51�ĵ�ַ32 */ /* ��MPDU��802.11ͷ���� */
    uint8_t bit_is_amsdu : 1;                             /* �Ƿ�AMSDU: OAL_FALSE���ǣ�OAL_TRUE�� */
    uint8_t bit_is_first_msdu : 1;                        /* �Ƿ��ǵ�һ����֡��OAL_FALSE���� OAL_TRUE�� */
    /* byte8 */
    uint8_t bit_tid : 4;                  /* dmac tx �� tx complete ���ݵ�user�ṹ�壬Ŀ���û���ַ */
    wlan_wme_ac_type_enum_uint8 bit_ac : 3; /* ac */
    uint8_t bit_ismcast : 1;              /* ��MPDU�ǵ������Ƕಥ:OAL_FALSE������OAL_TRUE�ಥ */
    /* byte9 */
    uint8_t bit_retried_num : 4;   /* �ش����� */
    uint8_t bit_mgmt_frame_id : 4; /* wpas ���͹���֡��frame id */
    /* byte10 */
    uint8_t bit_tx_user_idx : 6;          /* ����������userindex��һ��bit���ڱ�ʶ��Чindex */
    uint8_t bit_roam_data : 1;            /* �����ڼ�֡���ͱ�� */
    uint8_t bit_is_get_from_ps_queue : 1; /* ���������ã���ʶһ��MPDU�Ƿ�ӽ��ܶ�����ȡ������ */
    /* byte11 */
    uint8_t bit_tx_vap_index : 3;
    wlan_tx_ack_policy_enum_uint8 en_ack_policy : 3;
    uint8_t bit_is_needretry : 1;
    uint8_t bit_need_rsp : 1; /* WPAS send mgmt,need dmac response tx status */
    /* byte12 */
    uint8_t en_is_probe_data : 3; /* �Ƿ�̽��֡ */
    uint8_t bit_is_eapol_key_ptk : 1;                  /* 4 �����ֹ��������õ�����ԿEAPOL KEY ֡��ʶ */
    uint8_t bit_is_m2u_data : 1;                       /* �Ƿ����鲥ת���������� */
    uint8_t bit_is_tcp_ack : 1;                        /* ���ڱ��tcp ack */
    uint8_t bit_is_rtsp : 1;
    uint8_t en_use_4_addr : 1; /* �Ƿ�ʹ��4��ַ����WDS���Ծ��� */
    /* byte13-16 */
    uint16_t us_timestamp_ms;   /* ά��ʹ����TID���е�ʱ���, ��λ1ms���� */
    uint8_t bit_is_qosnull : 1; /* ���ڱ��qos null֡ */
    uint8_t bit_is_himit : 1; /* ���ڱ��himit֡ */
    uint8_t bit_hiex_traced : 1; /* ���ڱ��hiex֡ */
    uint8_t bit_is_game_marked : 1; /* ���ڱ����Ϸ֡ */
    uint8_t bit_is_hid2d_pkt : 1; /* ���ڱ��hid2dͶ������֡ */
    uint8_t bit_is_802_3_snap : 1; /* �Ƿ���802.3 snap */
    uint8_t uc_reserved1 : 2;
    uint8_t uc_data_type : 4; /* ����֡����, ring txʹ��, ��Ӧdata_type_enum */
    uint8_t csum_type : 3;
    uint8_t reserved2 : 1;
    /* byte17-18 */
    uint8_t uc_alg_pktno;     /* �㷨�õ����ֶΣ�Ψһ��ʾ�ñ��� */
    uint8_t uc_alg_frame_tag : 2; /* �����㷨��֡���б�� */
    uint8_t uc_hid2d_tx_delay_time : 6; /* ���ڱ���hid2d����֡��dmac��������ʱ�� */
    /* byte19 */
    uint8_t bit_large_amsdu_level : 2;  /* offload�´��AMSDU�ۺ϶� */
    uint8_t bit_align_padding : 2;      /* SKB ͷ������ETHER HEADERʱ,4�ֽڶ�����Ҫ��padding */
    uint8_t bit_msdu_num_in_amsdu : 2;  /* ����ۺ�ʱÿ��AMSDU��֡�� */
    uint8_t bit_is_large_skb_amsdu : 1; /* �Ƿ��Ƕ���֡����ۺ� */
    uint8_t bit_htc_flag : 1;           /* ���ڱ��htc */

#ifndef _PRE_PRODUCT_ID_HI110X_DEV
    /* OFFLOAD�ܹ��£�HOST���DEVICE��CB���� */
    mac_tx_expand_cb_stru st_expand_cb;
#endif
} __OAL_DECLARE_PACKED;
typedef struct mac_tx_ctl mac_tx_ctl_stru;
#pragma pack(pop)

typedef struct {
    uint16_t us_baw_start;                          /* ��һ��δȷ�ϵ�MPDU�����к� */
    uint16_t us_last_seq_num;                       /* ���һ�����͵�MPDU�����к� */
    uint16_t us_baw_size;                           /* Block_Ack�Ự��buffer size��С */
    uint16_t us_baw_head;                           /* bitmap�м�¼�ĵ�һ��δȷ�ϵİ���λ�� */
    uint16_t us_baw_tail;                           /* bitmap����һ��δʹ�õ�λ�� */
    oal_bool_enum_uint8 en_is_ba;                     /* Session Valid Flag */
    dmac_ba_conn_status_enum_uint8 en_ba_conn_status; /* BA�Ự��״̬ */
    mac_back_variant_enum_uint8 en_back_var;          /* BA�Ự�ı��� */
    uint8_t uc_dialog_token;                        /* ADDBA����֡��dialog token */
    uint8_t uc_ba_policy;                           /* Immediate=1 Delayed=0 */
    oal_bool_enum_uint8 en_amsdu_supp;                /* BLOCK ACK֧��AMSDU�ı�ʶ */
    uint8_t *puc_dst_addr;                          /* BA�Ự���ն˵�ַ */
    uint16_t us_ba_timeout;                         /* BA�Ự������ʱʱ�� */
    uint8_t uc_ampdu_max_num;                       /* BA�Ự�£��ܹ��ۺϵ�����mpdu�ĸ��� */
    uint8_t uc_tx_ba_lut;                           /* BA�ỰLUT session index */
    uint16_t us_mac_user_idx;
#ifdef _PRE_WLAN_FEATURE_DFR
    uint16_t us_pre_baw_start;    /* ��¼ǰһ���ж�ba���Ƿ���ʱ��ssn */
    uint16_t us_pre_last_seq_num; /* ��¼ǰһ���ж�ba���Ƿ���ʱ��lsn */
    uint16_t us_ba_jamed_cnt;     /* BA������ͳ�ƴ��� */
#else
    uint8_t auc_resv[NUM_2_BYTES];
#endif
    uint32_t aul_tx_buf_bitmap[DMAC_TX_BUF_BITMAP_WORDS];
} dmac_ba_tx_stru;

typedef enum {
    OAL_QUERY_STATION_INFO_EVENT = 1,
    OAL_ATCMDSRV_DBB_NUM_INFO_EVENT = 2,
    OAL_ATCMDSRV_FEM_PA_INFO_EVENT = 3,
    OAL_ATCMDSRV_GET_RX_PKCG = 4,
    OAL_ATCMDSRV_LTE_GPIO_CHECK = 5,
    OAL_ATCMDSRV_GET_ANT = 6,
    OAL_QUERY_EVNET_BUTT
} oal_query_event_id_enum;

typedef union {
    uint16_t rw_ptr; /* write index */
    struct {
        uint16_t bit_rw_ptr    : 15,
                 bit_wrap_flag : 1;
    } st_rw_ptr;
} un_rw_ptr;

typedef enum {
    TID_CMDID_CREATE                 = 0,
    TID_CMDID_DEL,
    TID_CMDID_ENQUE,
    TID_CMDID_DEQUE,

    TID_CMDID_BUTT,
} tid_cmd_enum;
typedef uint8_t tid_cmd_enum_uint8;

typedef enum {
    RING_PTR_EQUAL = 0,
    RING_PTR_SMALLER,
    RING_PTR_GREATER,
} ring_ptr_compare_enum;
typedef uint8_t ring_ptr_compare_enum_uint8;

typedef enum {
    HOST_RING_TX_MODE = 0,
    DEVICE_RING_TX_MODE = 1,
    DEVICE_TX_MODE = 2,
    H2D_SWITCHING_MODE = 3,
    D2H_SWITCHING_MODE = 4,
    TX_MODE_DEBUG_DUMP = 5,

    TX_MODE_BUTT,
} ring_tx_mode_enum;
typedef uint8_t ring_tx_mode_enum_uint8;

#define TX_RING_MSDU_INFO_LEN  12
#define DEVICE_TX_RING_MAX_NUM 16

typedef struct {
    uint32_t msdu_addr_lsb;
    uint32_t msdu_addr_msb;
    uint32_t msdu_len      : 12;
    uint32_t data_type     : 4;
    uint32_t frag_num      : 4;
    uint32_t to_ds         : 1;
    uint32_t from_ds       : 1;
    uint32_t more_frag     : 1;
    uint32_t reserved      : 1;
    uint32_t aggr_msdu_num : 4;
    uint32_t first_msdu    : 1;
    uint32_t csum_type     : 3;
} msdu_info_stru;

/* Host Device���ò��� */
typedef struct {
    uint8_t size;
    uint8_t max_amsdu_num;
    uint8_t reserved[2];
    uint32_t base_addr_lsb;
    uint32_t base_addr_msb;
    uint16_t read_index;
    uint16_t write_index;
} msdu_info_ring_stru; /* hal_common.h? */

typedef struct {
    uint16_t user_id;
    uint8_t tid_no;
    uint8_t cmd;
    uint8_t ring_index; /* �������ڴ�, device ring��tid�޷�����1:1����,
                         * host��h2d�л�ʱ, ��Ҫ��bitmap���ҵ���һ���ɷ����ring index,
                         * device����index��ring�ڴ���л�ȡ���õ�ring,
                         * d2h�л�ʱͬ��, host����Ҫ��ĳ����ռ�õ�ring index����Ϊ����
                         */
    uint8_t resv[3];
} tx_state_switch_stru;

#ifdef _PRE_WLAN_FEATURE_NRCOEX
typedef struct {
    uint32_t freq;
    uint32_t gap0_40m_20m;
    uint32_t gap0_160m_80m;
    uint32_t gap1_40m_20m;
    uint32_t gap1_160m_80m;
    uint32_t gap2_140m_20m;
    uint32_t gap2_160m_80m;
    uint32_t smallgap0_act;
    uint32_t gap01_act;
    uint32_t gap12_act;
    int32_t l_rxslot_rssi;
} nrcoex_rule_stru;

typedef struct {
    uint32_t us_freq_low_limit : 16,
               us_freq_high_limit : 16;
    uint32_t us_20m_w2m_gap0 : 16,
               us_40m_w2m_gap0 : 16;
    uint32_t us_80m_w2m_gap0 : 16,
               us_160m_w2m_gap0 : 16;
    uint32_t us_20m_w2m_gap1 : 16,
               us_40m_w2m_gap1 : 16;
    uint32_t us_80m_w2m_gap1 : 16,
               us_160m_w2m_gap1 : 16;
    uint32_t us_20m_w2m_gap2 : 16,
               us_40m_w2m_gap2 : 16;
    uint32_t us_80m_w2m_gap2 : 16,
               us_160m_w2m_gap2 : 16;
    uint32_t uc_20m_smallgap0_act : 8,
               uc_40m_smallgap0_act : 8,
               uc_80m_smallgap0_act : 8,
               uc_160m_smallgap0_act : 8;
    uint32_t uc_20m_gap01_act : 8,
               uc_40m_gap01_act : 8,
               uc_80m_gap01_act : 8,
               uc_160m_gap01_act : 8;
    uint32_t uc_20m_gap12_act : 8,
               uc_40m_gap12_act : 8,
               uc_80m_gap12_act : 8,
               uc_160m_gap12_act : 8;
    uint32_t uc_20m_rxslot_rssi : 8,
               uc_40m_rxslot_rssi : 8,
               uc_80m_rxslot_rssi : 8,
               uc_160m_rxslot_rssi : 8;
} nrcoex_rule_detail_stru;

typedef union {
    nrcoex_rule_stru st_nrcoex_rule_data;
    nrcoex_rule_detail_stru st_nrcoex_rule_detail_data;
} nrcoex_rule_data_union;

typedef struct {
    uint8_t uc_nrcoex_enable;
    uint8_t rsv[NUM_3_BYTES];  /* �����ֶ�:3�ֽ� */
    nrcoex_rule_data_union un_nrcoex_rule_data[DMAC_WLAN_NRCOEX_INTERFERE_RULE_NUM];
} nrcoex_cfg_info_stru;
#endif

typedef struct {
    uint8_t cfg_type;
    oal_bool_enum_uint8 need_w4_dev_return;
    uint8_t resv[NUM_2_BYTES]; /* 4 �ֽڶ��� */
    uint32_t dev_ret;
} mac_cfg_cali_hdr_stru;

#ifdef _PRE_WLAN_FEATURE_PHY_EVENT_INFO
typedef struct {
    uint8_t               event_rpt_en;
    uint8_t               rsv[NUM_3_BYTES]; /* 4 �ֽڶ��� */
    uint32_t              wp_mem_num;       /* �¼�ͳ�Ƹ��� */
    uint32_t              wp_event0_type_sel;  /* event0�¼�����ѡ����� */
    uint32_t              wp_event1_type_sel;  /* event1�¼�����ѡ����� */
} mac_cfg_phy_event_rpt_stru;
#endif

#define RING_DEPTH(SIZE)  ((128) << (SIZE))

/* ������offload�ܹ���CB�ֶ� */
#define MAC_GET_CB_IS_4ADDRESS(_pst_tx_ctrl)        ((_pst_tx_ctrl)->en_use_4_addr)
/* ����Ƿ���С��AMSDU֡ */
#define MAC_GET_CB_IS_AMSDU(_pst_tx_ctrl)           ((_pst_tx_ctrl)->bit_is_amsdu)
/* ����Ӧ�㷨�����ľۺ϶� */
#define MAC_GET_CB_AMSDU_LEVEL(_pst_tx_ctrl)        ((_pst_tx_ctrl)->bit_large_amsdu_level)
/* MSDU֡ͷ�������ֽ� */
#define MAC_GET_CB_ETHER_HEAD_PADDING(_pst_tx_ctrl) ((_pst_tx_ctrl)->bit_align_padding)
/* ʵ����ɵĴ��AMSDU֡��������֡�� */
#define MAC_GET_CB_HAS_MSDU_NUMBER(_pst_tx_ctrl)    ((_pst_tx_ctrl)->bit_msdu_num_in_amsdu)
/* ����Ƿ�����˴��AMSDU֡ */
#define MAC_GET_CB_IS_LARGE_SKB_AMSDU(_pst_tx_ctrl) ((_pst_tx_ctrl)->bit_is_large_skb_amsdu)
#define MAC_GET_CB_IS_FIRST_MSDU(_pst_tx_ctrl)      ((_pst_tx_ctrl)->bit_is_first_msdu)
#define MAC_GET_CB_IS_NEED_RESP(_pst_tx_ctrl)       ((_pst_tx_ctrl)->bit_need_rsp)
#define MAC_GET_CB_IS_EAPOL_KEY_PTK(_pst_tx_ctrl)   ((_pst_tx_ctrl)->bit_is_eapol_key_ptk)
#define MAC_GET_CB_IS_ROAM_DATA(_pst_tx_ctrl)       ((_pst_tx_ctrl)->bit_roam_data)
#define MAC_GET_CB_IS_FROM_PS_QUEUE(_pst_tx_ctrl)   ((_pst_tx_ctrl)->bit_is_get_from_ps_queue)
#define MAC_GET_CB_IS_MCAST(_pst_tx_ctrl)           ((_pst_tx_ctrl)->bit_ismcast)
#define MAC_GET_CB_IS_NEEDRETRY(_pst_tx_ctrl)       ((_pst_tx_ctrl)->bit_is_needretry)
#define MAC_GET_CB_IS_PROBE_DATA(_pst_tx_ctrl)      ((_pst_tx_ctrl)->en_is_probe_data)
#define MAC_GET_CB_IS_RTSP(_pst_tx_ctrl)            ((_pst_tx_ctrl)->bit_is_rtsp)
#define MAC_GET_CB_ALG_TAGS(_pst_tx_ctrl)           ((_pst_tx_ctrl)->uc_alg_frame_tag)

#define MAC_GET_CB_MGMT_FRAME_ID(_pst_tx_ctrl) ((_pst_tx_ctrl)->bit_mgmt_frame_id)
#define MAC_GET_CB_MPDU_LEN(_pst_tx_ctrl)      ((_pst_tx_ctrl)->us_mpdu_payload_len)
#define MAC_GET_CB_FRAME_TYPE(_pst_tx_ctrl)    ((_pst_tx_ctrl)->uc_frame_type)
#define MAC_GET_CB_FRAME_SUBTYPE(_pst_tx_ctrl) ((_pst_tx_ctrl)->uc_frame_subtype)
#define MAC_GET_CB_DATA_TYPE(_pst_tx_ctrl)     ((_pst_tx_ctrl)->uc_data_type)
#define MAC_GET_CB_IS_802_3_SNAP(_pst_tx_ctrl)    ((_pst_tx_ctrl)->bit_is_802_3_snap)
#define MAC_GET_CB_TIMESTAMP(_pst_tx_ctrl)     ((_pst_tx_ctrl)->us_timestamp_ms)
#define MAC_GET_CB_IS_QOSNULL(_pst_tx_ctrl)    ((_pst_tx_ctrl)->bit_is_qosnull)
#ifdef _PRE_WLAN_FEATURE_HIEX
#define MAC_GET_CB_HIEX_TRACED(_pst_tx_ctrl) ((_pst_tx_ctrl)->bit_hiex_traced)
#define MAC_GET_RX_CB_HIEX_TRACED(_pst_rx_ctl) ((_pst_rx_ctl)->bit_hiex_traced)
#endif
/* VIP����֡ */
#define MAC_GET_CB_IS_EAPOL(ptx_ctrl)                             \
    ((MAC_GET_CB_FRAME_TYPE(ptx_ctrl) == WLAN_CB_FRAME_TYPE_DATA) && \
        (MAC_GET_CB_FRAME_SUBTYPE(ptx_ctrl) == MAC_DATA_EAPOL))
#define MAC_GET_CB_IS_ARP(_pst_tx_ctrl)                             \
        ((MAC_GET_CB_FRAME_TYPE(_pst_tx_ctrl) == WLAN_CB_FRAME_TYPE_DATA) && \
            ((MAC_GET_CB_FRAME_SUBTYPE(_pst_tx_ctrl) == MAC_DATA_ARP_REQ) || \
            (MAC_GET_CB_FRAME_SUBTYPE(_pst_tx_ctrl) == MAC_DATA_ARP_RSP)))
#define MAC_GET_CB_IS_VIPFRAME(_pst_tx_ctrl)                             \
    ((WLAN_CB_FRAME_TYPE_DATA == MAC_GET_CB_FRAME_TYPE(_pst_tx_ctrl)) && \
        (MAC_DATA_VIP_FRAME >= MAC_GET_CB_FRAME_SUBTYPE(_pst_tx_ctrl)))

#define MAC_GET_CB_IS_SMPS_FRAME(_pst_tx_ctrl)                             \
    ((WLAN_CB_FRAME_TYPE_ACTION == MAC_GET_CB_FRAME_TYPE(_pst_tx_ctrl)) && \
        (WLAN_ACTION_SMPS_FRAME_SUBTYPE == MAC_GET_CB_FRAME_SUBTYPE(_pst_tx_ctrl)))
#define MAC_GET_CB_IS_OPMODE_FRAME(_pst_tx_ctrl)                           \
    ((WLAN_CB_FRAME_TYPE_ACTION == MAC_GET_CB_FRAME_TYPE(_pst_tx_ctrl)) && \
        (WLAN_ACTION_OPMODE_FRAME_SUBTYPE == MAC_GET_CB_FRAME_SUBTYPE(_pst_tx_ctrl)))
#define MAC_GET_CB_IS_P2PGO_FRAME(_pst_tx_ctrl)                          \
    ((WLAN_CB_FRAME_TYPE_MGMT == MAC_GET_CB_FRAME_TYPE(_pst_tx_ctrl)) && \
        (WLAN_ACTION_P2PGO_FRAME_SUBTYPE == MAC_GET_CB_FRAME_SUBTYPE(_pst_tx_ctrl)))
#ifdef _PRE_WLAN_FEATURE_TWT
#define MAC_GET_CB_IS_TWT_SETUP_FRAME(_pst_tx_ctrl)                        \
    ((WLAN_CB_FRAME_TYPE_ACTION == MAC_GET_CB_FRAME_TYPE(_pst_tx_ctrl)) && \
        (WLAN_ACTION_TWT_SETUP_REQ == MAC_GET_CB_FRAME_SUBTYPE(_pst_tx_ctrl)))
#endif
#define mac_dbdc_send_m2s_action(mac_device, tx_ctl) \
    (mac_is_dbdc_running((mac_device)) && (MAC_GET_CB_IS_SMPS_FRAME((tx_ctl)) || MAC_GET_CB_IS_OPMODE_FRAME((tx_ctl))))

/* ģ�鷢�����̿�����Ϣ�ṹ�����ϢԪ�ػ�ȡ */
#define MAC_GET_CB_MPDU_NUM(_pst_tx_ctrl)            ((_pst_tx_ctrl)->bit_mpdu_num)
#define MAC_GET_CB_NETBUF_NUM(_pst_tx_ctrl)          ((_pst_tx_ctrl)->bit_netbuf_num)
#define MAC_GET_CB_FRAME_HEADER_LENGTH(_pst_tx_ctrl) ((_pst_tx_ctrl)->bit_frame_header_length) /* ToBeDel */
#define MAC_GET_MACHDR_LEN(_pst_tx_ctrl)             ((_pst_tx_ctrl)->bit_frame_header_length)
#define MAC_GET_CB_ACK_POLACY(_pst_tx_ctrl)          ((_pst_tx_ctrl)->en_ack_policy)
#define MAC_GET_CB_TX_VAP_INDEX(_pst_tx_ctrl)        ((_pst_tx_ctrl)->bit_tx_vap_index)
#define MAC_GET_CB_TX_USER_IDX(_pst_tx_ctrl)         ((_pst_tx_ctrl)->bit_tx_user_idx)
#define MAC_GET_CB_WME_AC_TYPE(_pst_tx_ctrl)         ((_pst_tx_ctrl)->bit_ac)
#define MAC_GET_CB_WME_TID_TYPE(_pst_tx_ctrl)        ((_pst_tx_ctrl)->bit_tid)
#define MAC_GET_CB_EVENT_TYPE(_pst_tx_ctrl)          ((_pst_tx_ctrl)->bit_event_type)
#define MAC_GET_CB_EVENT_SUBTYPE(_pst_tx_ctrl)       ((_pst_tx_ctrl)->bit_event_sub_type)
#define MAC_GET_CB_RETRIED_NUM(_pst_tx_ctrl)         ((_pst_tx_ctrl)->bit_retried_num)
#define MAC_GET_CB_ALG_PKTNO(_pst_tx_ctrl)           ((_pst_tx_ctrl)->uc_alg_pktno)
#define MAC_GET_CB_TCP_ACK(_pst_tx_ctrl)             ((_pst_tx_ctrl)->bit_is_tcp_ack)
#define MAC_GET_CB_HTC_FLAG(_pst_tx_ctrl)            ((_pst_tx_ctrl)->bit_htc_flag)
#ifdef _PRE_WLAN_FEATURE_HIEX
#define MAC_GET_CB_IS_HIMIT_FRAME(_pst_tx_ctrl)      ((_pst_tx_ctrl)->bit_is_himit)
#define MAC_GET_CB_IS_GAME_MARKED_FRAME(_pst_tx_ctrl)((_pst_tx_ctrl)->bit_is_game_marked)
#endif
#ifdef _PRE_WLAN_FEATURE_HID2D_TX_DROP
#define MAC_GET_CB_IS_HID2D_FRAME(_pst_tx_ctrl)((_pst_tx_ctrl)->bit_is_hid2d_pkt)
#define MAC_GET_CB_HID2D_TX_DELAY_TIME(_pst_tx_ctrl)((_pst_tx_ctrl)->uc_hid2d_tx_delay_time)
#endif
#define MAC_GET_CB_IS_DATA_FRAME(_pst_tx_ctrl)                                \
    ((MAC_GET_CB_WLAN_FRAME_TYPE(_pst_tx_ctrl) == WLAN_DATA_BASICTYPE) &&     \
     ((MAC_GET_CB_WLAN_FRAME_SUBTYPE(_pst_tx_ctrl) == WLAN_DATA) ||  \
     (MAC_GET_CB_WLAN_FRAME_SUBTYPE(_pst_tx_ctrl) == WLAN_QOS_DATA) || \
     (MAC_GET_CB_WLAN_FRAME_SUBTYPE(_pst_tx_ctrl) == WLAN_QOS_NULL_FRAME)))

#ifdef _PRE_WLAN_FEATURE_TXBF_HW
#define MAC_FRAME_TYPE_IS_VHT_NDPA(_uc_type, _uc_sub_type) \
        ((WLAN_CONTROL == (_uc_type)) && (WLAN_VHT_NDPA == (_uc_sub_type)))
#endif

#if defined(_PRE_PRODUCT_ID_HI110X_DEV)
#if ((_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1105_DEV))
#define MAC_CB_FRM_OFFSET OAL_MAX_CB_LEN
#else
#define MAC_CB_FRM_OFFSET (OAL_MAX_CB_LEN + MAX_MAC_HEAD_LEN)
#endif

#define MAC_GET_CB_SEQ_NUM(_pst_tx_ctrl)             \
    (((mac_ieee80211_frame_stru *)((uint8_t *)_pst_tx_ctrl + OAL_MAX_CB_LEN))->bit_seq_num)
#define MAC_SET_CB_80211_MAC_HEAD_TYPE(_pst_tx_ctrl, _flag)
#define MAC_GET_CB_80211_MAC_HEAD_TYPE(_pst_tx_ctrl) 1 /* offload�ܹ�,MAC HEAD��netbuf index����,����Ҫ�ͷ� */

#define MAC_GET_CB_WLAN_FRAME_TYPE(_pst_tx_ctrl) \
    (((mac_ieee80211_frame_stru *)((uint8_t *)(_pst_tx_ctrl) + MAC_CB_FRM_OFFSET))->st_frame_control.bit_type)
#define MAC_GET_CB_WLAN_FRAME_SUBTYPE(_pst_tx_ctrl) \
    (((mac_ieee80211_frame_stru *)((uint8_t *)(_pst_tx_ctrl) + MAC_CB_FRM_OFFSET))->st_frame_control.bit_sub_type)

#define MAC_SET_CB_FRAME_HEADER_ADDR(_pst_tx_ctrl, _addr)
#define MAC_GET_CB_FRAME_HEADER_ADDR(_pst_tx_ctrl) \
    ((mac_ieee80211_frame_stru *)((uint8_t *)_pst_tx_ctrl + OAL_MAX_CB_LEN))

#define MAC_SET_CB_IS_QOS_DATA(_pst_tx_ctrl, _flag)
#define MAC_GET_CB_IS_QOS_DATA(_pst_tx_ctrl) \
    ((WLAN_DATA_BASICTYPE == MAC_GET_CB_WLAN_FRAME_TYPE(_pst_tx_ctrl)) &&  \
        ((WLAN_QOS_DATA == MAC_GET_CB_WLAN_FRAME_SUBTYPE(_pst_tx_ctrl)) ||  \
            (WLAN_QOS_NULL_FRAME == MAC_GET_CB_WLAN_FRAME_SUBTYPE(_pst_tx_ctrl))))

#define MAC_SET_CB_IS_BAR(_pst_tx_ctrl, _flag)
#define MAC_GET_CB_IS_BAR(_pst_tx_ctrl) ((WLAN_CONTROL == MAC_GET_CB_WLAN_FRAME_TYPE(_pst_tx_ctrl)) && \
        (WLAN_BLOCKACK_REQ == MAC_GET_CB_WLAN_FRAME_SUBTYPE(_pst_tx_ctrl)))

/* OFFLOAD �ܹ�����Ҫ */
#define MAC_SET_CB_BAR_DSCR_ADDR(_pst_tx_ctrl, _addr)
#define MAC_GET_CB_BAR_DSCR_ADDR(_pst_tx_ctrl) NULL
#else
#define MAC_GET_CB_WLAN_FRAME_TYPE(_pst_tx_ctrl)            ((_pst_tx_ctrl)->st_expand_cb.en_frame_type)
#define MAC_GET_CB_WLAN_FRAME_SUBTYPE(_pst_tx_ctrl) (((_pst_tx_ctrl)->pst_frame_header)->st_frame_control.bit_sub_type)
#define MAC_GET_CB_SEQ_NUM(_pst_tx_ctrl)                    ((_pst_tx_ctrl)->st_expand_cb.us_seqnum)
#define MAC_SET_CB_FRAME_HEADER_ADDR(_pst_tx_ctrl, _addr)   ((_pst_tx_ctrl)->st_expand_cb.pst_frame_header = (_addr))
#define MAC_GET_CB_FRAME_HEADER_ADDR(_pst_tx_ctrl)          ((_pst_tx_ctrl)->st_expand_cb.pst_frame_header)
#define MAC_SET_CB_80211_MAC_HEAD_TYPE(_pst_tx_ctrl, _flag) \
    ((_pst_tx_ctrl)->st_expand_cb.bit_80211_mac_head_type = (_flag))
#define MAC_GET_CB_80211_MAC_HEAD_TYPE(_pst_tx_ctrl)        ((_pst_tx_ctrl)->st_expand_cb.bit_80211_mac_head_type)
#define MAC_SET_CB_IS_QOS_DATA(_pst_tx_ctrl, _flag)
#define MAC_GET_CB_IS_QOS_DATA(_pst_tx_ctrl)                OAL_FALSE
#endif  // #if defined(_PRE_PRODUCT_ID_HI110X_DEV)

#define ALG_SCHED_PARA(tx_ctl) \
    ((MAC_GET_CB_IS_DATA_FRAME(tx_ctl)) && (!MAC_GET_CB_IS_VIPFRAME(tx_ctl)) && (!MAC_GET_CB_IS_MCAST(tx_ctl)))

#define MAC_GET_RX_CB_FRAME_LEN(_pst_rx_ctl)         ((_pst_rx_ctl)->us_frame_len)
#define MAC_GET_RX_CB_MAC_HEADER_LEN(_pst_rx_ctl)    ((_pst_rx_ctl)->uc_mac_header_len)
#define MAC_GET_RX_CB_MAC_VAP_ID(_pst_rx_ctl)        ((_pst_rx_ctl)->uc_mac_vap_id)
#define MAC_GET_RX_CB_HAL_VAP_IDX(_pst_rx_ctl)       ((_pst_rx_ctl)->bit_vap_id)
#define MAC_RXCB_VAP_ID(_pst_rx_ctl)                 ((_pst_rx_ctl)->uc_mac_vap_id)

#define MAC_GET_RX_CB_SEQ_NUM(_pst_rx_ctl)           ((_pst_rx_ctl)->us_seq_num)
#define MAC_GET_RX_CB_TID(_pst_rx_ctl)               ((_pst_rx_ctl)->rx_tid)
#define MAC_GET_RX_CB_PROC_FLAG(_pst_rx_ctl)         ((_pst_rx_ctl)->bit_process_flag)
#define MAC_GET_RX_CB_REL_IS_VALID(_pst_rx_ctl)      ((_pst_rx_ctl)->bit_release_valid)
#define MAC_GET_RX_CB_REL_END_SEQNUM(_pst_rx_ctl)    ((_pst_rx_ctl)->bit_release_end_sn)
#define MAC_GET_RX_CB_SSN(_pst_rx_ctl)               ((_pst_rx_ctl)->bit_start_seqnum)
#define MAC_GET_RX_CB_REL_START_SEQNUM(_pst_rx_ctl)  ((_pst_rx_ctl)->bit_release_start_sn)
#define MAC_GET_RX_CB_DST_IS_VALID(_pst_rx_ctl)      ((_pst_rx_ctl)->bit_dst_is_valid)
#define MAC_GET_RX_DST_USER_ID(_pst_rx_ctl)          ((_pst_rx_ctl)->dst_user_id)

#define MAC_RXCB_IS_AMSDU(_pst_rx_ctl)               ((_pst_rx_ctl)->bit_amsdu_enable)
#define MAC_RX_CB_IS_DEST_CURR_BSS(_pst_rx_ctl)     \
    (((_pst_rx_ctl)->bit_dst_is_valid) && (((_pst_rx_ctl)->bit_band_id) == ((_pst_rx_ctl)->bit_dst_band_id)) && \
    (((_pst_rx_ctl)->bit_vap_id) == ((_pst_rx_ctl)->dst_hal_vap_id)))


/* DMACģ��������̿�����Ϣ�ṹ�����ϢԪ�ػ�ȡ */
#define MAC_RXCB_TA_USR_ID(_pst_rx_ctl) ((_pst_rx_ctl)->bit_ta_user_idx)
#define MAC_GET_RX_CB_TA_USER_IDX(_pst_rx_ctl) ((_pst_rx_ctl)->bit_ta_user_idx)
#define MAC_GET_RX_CB_PAYLOAD_LEN(_pst_rx_ctl) ((_pst_rx_ctl)->us_frame_len - (_pst_rx_ctl)->uc_mac_header_len)
#define MAC_GET_RX_CB_NAN_FLAG(_pst_rx_ctl) ((_pst_rx_ctl)->bit_nan_flag)
#if defined(_PRE_PRODUCT_ID_HI110X_DEV)
#if (_PRE_PRODUCT_ID != _PRE_PRODUCT_ID_HI1106_DEV && _PRE_PRODUCT_ID != _PRE_PRODUCT_ID_BISHENG_DEV)
#define MAC_GET_RX_PAYLOAD_ADDR(_pst_rx_ctl, _pst_buf) \
    (get_netbuf_payload(_pst_buf) + ((_pst_rx_ctl)->uc_mac_header_len) - ((_pst_rx_ctl)->uc_mac_header_len))
#else
#define MAC_GET_RX_PAYLOAD_ADDR(_pst_rx_ctl, _pst_buf) \
    (get_netbuf_payload(_pst_buf) + ((_pst_rx_ctl)->uc_mac_header_len))
#endif
#define MAC_GET_RX_CB_MAC_HEADER_ADDR(_prx_cb_ctrl)    (mac_get_rx_cb_mac_hdr(_prx_cb_ctrl))
#else
#define MAC_GET_RX_CB_MAC_HEADER_ADDR(_pst_rx_ctl)     ((_pst_rx_ctl)->st_expand_cb.pul_mac_hdr_start_addr)
#define MAC_GET_RX_CB_DA_USER_IDX(_pst_rx_ctl)         ((_pst_rx_ctl)->st_expand_cb.us_da_user_idx)
#define MAC_GET_RX_PAYLOAD_ADDR(_pst_rx_ctl, _pst_buf) \
    ((uint8_t *)(mac_get_rx_cb_mac_hdr(_pst_rx_ctl)) + MAC_GET_RX_CB_MAC_HEADER_LEN(_pst_rx_ctl))
#endif

#if (defined(_PRE_PRODUCT_ID_HI110X_HOST) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1106_DEV) || \
    (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_BISHENG_DEV))
#define MAC_RXCB_STATUS(_pst_rx_ctl)                 ((_pst_rx_ctl)->rx_status)
#define MAC_RX_CB_IS_MULTICAST(_pst_rx_ctl)          ((_pst_rx_ctl)->bit_mcast_bcast)
#define MAC_GET_RX_CB_IS_FIRST_SUB_MSDU(_pst_rx_ctl) ((_pst_rx_ctl)->bit_first_sub_msdu)
#define MAC_GET_RX_CB_IS_REO_TIMEOUT(_pst_rx_ctl)    ((_pst_rx_ctl)->bit_is_reo_timeout)
#define MAC_RXCB_IS_AMSDU_SUB_MSDU(_pst_rx_ctl) \
    ((MAC_RXCB_IS_AMSDU(_pst_rx_ctl) == OAL_TRUE) && (MAC_GET_RX_CB_IS_FIRST_SUB_MSDU(_pst_rx_ctl) == OAL_FALSE))
#define MAC_RXCB_IS_FIRST_AMSDU(_pst_rx_ctl) \
    ((MAC_RXCB_IS_AMSDU(_pst_rx_ctl) == OAL_TRUE) && (MAC_GET_RX_CB_IS_FIRST_SUB_MSDU(_pst_rx_ctl) == OAL_TRUE))
#endif


OAL_STATIC OAL_INLINE uint32_t *mac_get_rx_cb_mac_hdr(mac_rx_ctl_stru *pst_cb_ctrl)
{
#if defined(_PRE_PRODUCT_ID_HI110X_DEV)
#if (_PRE_PRODUCT_ID != _PRE_PRODUCT_ID_HI1106_DEV && (_PRE_PRODUCT_ID != _PRE_PRODUCT_ID_BISHENG_DEV))
    return (uint32_t *)((uint8_t *)pst_cb_ctrl + OAL_MAX_CB_LEN);
#else
    return (uint32_t *)((uint8_t *)pst_cb_ctrl + OAL_MAX_CB_LEN + MAX_MAC_HEAD_LEN);
#endif
#else
    return MAC_GET_RX_CB_MAC_HEADER_ADDR(pst_cb_ctrl);
#endif
}


static inline uint8_t *mac_netbuf_get_payload(oal_netbuf_stru *netbuf)
{
#if (defined(_PRE_PRODUCT_ID_HI110X_DEV) && (_PRE_PRODUCT_ID != _PRE_PRODUCT_ID_HI1106_DEV) && \
    (_PRE_PRODUCT_ID != _PRE_PRODUCT_ID_BISHENG_DEV))
    return get_netbuf_payload(netbuf);
#else
    /* ע��! �������mac header���Ȳ���24�ֽڵĲ�Ҫʹ�øú��� */
    return get_netbuf_payload(netbuf) + MAC_80211_FRAME_LEN;
#endif
}


OAL_STATIC OAL_INLINE uint8_t wlan_get_center_freq_idx(mac_channel_stru *pst_channel_info)
{
    uint8_t uc_center_freq_idx;
    int8_t  c_freq_shift;
    int8_t ac_center_chan_offset[WLAN_BAND_WIDTH_BUTT] = {
        0, 2, -2, 6, -2, 2, -6,
#ifdef _PRE_WLAN_FEATURE_160M
        14, -2, 6, -10, 10, -6, 2, -14,
#endif
        0, 0
    };

    c_freq_shift = ac_center_chan_offset[pst_channel_info->en_bandwidth];
    uc_center_freq_idx = (uint8_t)(pst_channel_info->uc_chan_number + c_freq_shift);

    return uc_center_freq_idx;
}


static inline uint16_t wlan_get_center_freq_2g(mac_channel_stru *channel_info)
{
    uint16_t center_freq;
    uint8_t center_freq_chnnum = wlan_get_center_freq_idx(channel_info);
    if (center_freq_chnnum == 14) {            /* JP chn 14 */
        center_freq = 2484;                   /* freq is 2484 */
    } else {
        center_freq = 2407 + 5 * center_freq_chnnum;   /* 2G center freq = 2407 + 5 * (channel number) */
    }

    return center_freq;
}


static inline uint16_t wlan_get_center_freq_5g(mac_channel_stru *channel_info)
{
    uint16_t center_freq;
    uint8_t center_freq_chnnum = wlan_get_center_freq_idx(channel_info);
    if (center_freq_chnnum <= 178) {                    /* JP ����178�ŵ��ŵĻ�����4000 */
        center_freq = 5000 + 5 * center_freq_chnnum;    /* 5G center freq = 5000 + 5 * (channel number) */
    } else {
        center_freq = 4000 + 5 * center_freq_chnnum;    /* JP chn freq = 4000 + 5 * (channel number) */
    }

    return center_freq;
}


static inline uint16_t wlan_get_center_freq_6g(mac_channel_stru *channel_info)
{
    uint16_t center_freq;
    uint8_t center_freq_chnnum = wlan_get_center_freq_idx(channel_info);

    center_freq = 5950 + 5 * center_freq_chnnum; /* center frequency = 5950 + 5 �� nch (MHz) */
    return center_freq;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of mac_common.h */
