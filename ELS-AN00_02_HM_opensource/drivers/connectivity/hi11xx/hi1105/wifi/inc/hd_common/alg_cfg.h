

#ifndef __ALG_CFG_H__
#define __ALG_CFG_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "oal_types.h"
#include "hal_common.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_ALG_CFG_H

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define ALG_CFG_PARAM_MAX_NUM           16    /* �㷨�������������Ŀ */
/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
typedef enum {
    ALG_CFG_TYPE_SET,
    ALG_CFG_TYPE_GET,

    ALG_CFG_TYPE_BUTT
} alg_cfg_type_enum;
typedef uint8_t alg_cfg_type_enum_uint8;

typedef enum {
    ALG_CFG_PKT_TYPE_UCAST_DATA,  /* ��������֡ */
    ALG_CFG_PKT_TYPE_MCAST_DATA,  /* �鲥����֡ */
    ALG_CFG_PKT_TYPE_BCAST_DATA,  /* �㲥����֡ */
    ALG_CFG_PKT_TYPE_MUMIMO_DATA, /* MU-MIMO����֡ */
    ALG_CFG_PKT_TYPE_OFDMA_DATA,  /* OFDMA����֡ */

    ALG_CFG_PKT_TYPE_UCAST_MGMT, /* ��������֡ */
    ALG_CFG_PKT_TYPE_MCAST_MGMT, /* �鲥����֡ */
    ALG_CFG_PKT_TYPE_BCAST_MGMT, /* �㲥����֡ */

    ALG_CFG_PKT_TYPE_PROT_CTRL,  /* ��������֡: RTS/self-CTS/MU-RTS */
    ALG_CFG_PKT_TYPE_NDP,        /* NDP  ֡���� */
    ALG_CFG_PKT_TYPE_NDPA,       /* NDPA ֡���� */
    ALG_CFG_PKT_TYPE_RESP_CTRL,  /* ��Ӧ����֡: BA/ACK/CTS */
    ALG_CFG_PKT_TYPE_OTHER_CTRL, /* Trigger and other control frame */

    ALG_CFG_PKT_TYPE_IRRELEVANT, /* pkt_type �޹� */

    ALG_CFG_PKT_TYPE_BUTT = ALG_CFG_PKT_TYPE_IRRELEVANT,
} alg_cfg_pkt_type_enum;
typedef uint8_t alg_cfg_pkt_type_enum_uint8;

/**************************** �����㷨ÿ�������Ӧ��ö�ٶ��� **********************************/
/* -------------------AUTORATE START--------------------- */
typedef enum {
    ALG_RATE_MODE_AUTO,  /* do not use fix param */
    ALG_RATE_MODE_FIXED, /* use fix param to fill tx_param_dscr */

    ALG_RATE_MODE_BUTT,
} alg_cfg_param_rate_mode_enum;
typedef uint8_t alg_cfg_param_rate_mode_enum_uint8;

/* -------------------TXMODE START--------------------- */
typedef enum {
    ALG_CFG_TX_MODE_AUTO = 0,
    ALG_CFG_TX_MODE_CHAIN0_FIX,
    ALG_CFG_TX_MODE_CHAIN1_FIX,
    ALG_CFG_TX_MODE_CHAIN2_FIX,
    ALG_CFG_TX_MODE_CHAIN3_FIX,
    ALG_CFG_TX_MODE_CSD2_FIX,
    ALG_CFG_TX_MODE_CSD3_FIX,
    ALG_CFG_TX_MODE_CSD4_FIX,
    ALG_CFG_TX_MODE_STBC2_FIX,
    ALG_CFG_TX_MODE_STBC3_FIX,
    ALG_CFG_TX_MODE_STBC4_FIX,
    ALG_CFG_TX_MODE_TXBF_FIX,
    ALG_CFG_TX_MODE_STBC_2NSS_FIX,

    ALG_CFG_TX_MODE_BUTT
} alg_cfg_param_tx_mode_enum;

/* -------------------TXMODE END--------------------- */
/* -------------------WATERFILLING START--------------------- */
typedef enum {
    ALG_CFG_TX_WATERFILLING_AUTO_MODE = 0,
    ALG_CFG_TX_WATERFILLING_FIX_MODE = 1,
    ALG_CFG_TX_WATERFILLING_ENABLE = 1,
    ALG_CFG_TX_WATERFILLING_DISABLE = 0,
    ALG_CFG_TX_HARDMARD_AUTO_MODE = ALG_CFG_TX_WATERFILLING_AUTO_MODE,
    ALG_CFG_TX_HARDMARD_FIX_MODE = ALG_CFG_TX_WATERFILLING_FIX_MODE,
    ALG_CFG_TX_WATERFILLING_MIN_ASNR = 0,
    ALG_CFG_TX_WATERFILLING_MAX_ASNR = 63,
    ALG_CFG_TX_WATERFILLING_MAX_LIMIT = 127,
    ALG_CFG_TX_WATERFILLING_MIN_LIMIT = 0,
    ALG_CFG_TX_WATERFILLING_SNR_NUM = 4,
    ALG_CFG_TX_WATERFILLING_BUTT
} alg_cfg_param_tx_waterfilling_enum;
typedef uint8_t alg_cfg_param_tx_waterfilling_enum_uint8;

typedef enum {
    /* TXMODE  -- start */
    MAC_ALG_CFG_TXMODE_START,
    MAC_ALG_CFG_TXMODE_MODE_SW,
    MAC_ALG_CFG_TXMODE_ALL_USER_FIX_MODE,
    MAC_ALG_CFG_TXMODE_USER_FIX_MODE,
    MAC_ALG_CFG_TXMODE_USER_FIX_CHAIN,
    MAC_ALG_CFG_TXMODE_DEBUG_LOG_SWITCH,
    MAC_ALG_CFG_TXMODE_CHAIN_PROB_SW,
    MAC_ALG_CFG_TXMODE_END,
    /* TXMODE -- End */
    /* waterfilling --Start */
    MAC_ALG_CFG_WATERFILLING_START,
    MAC_ALG_CFG_WATERFILLING_MODE,
    MAC_ALG_CFG_WATERFILLING_ENABLE,
    MAC_ALG_CFG_HARDAMARDTX_MODE,
    MAC_ALG_CFG_HARDAMARDTX_ENABLE,
    MAC_ALG_CFG_WATERFILLING_CI_ASNR,
    MAC_ALG_CFG_WATERFILLING_LIMIT_ASNR,
    MAC_ALG_CFG_WATERFILLING_DEBUG,
    MAC_ALG_CFG_WATERFILLING_END,
    /* waterfilling --End */
} mac_alg_cfg_enum;
typedef uint16_t mac_alg_cfg_enum_uint16;

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/
/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/
/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/
/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
typedef struct {
    mac_alg_cfg_enum_uint16 en_cfg_id;
    alg_cfg_type_enum_uint8 en_cfg_type;
    alg_cfg_pkt_type_enum_uint8 en_pkt_type;

    uint32_t dev_ret;

    oal_bool_enum_uint8 en_need_w4_dev_return;
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_hdr_stru;

/* device�๲����������ṹ�� */
typedef struct {
    alg_cfg_hdr_stru alg_cfg_hdr;
    uint8_t value[ALG_CFG_PARAM_MAX_NUM];  /* ���ò��� */
} alg_cfg_param_all_alg_stru;

/* -------------------txmode START--------------------- */
typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    oal_bool_enum_uint8 en_txmode_sw; /* txmode�㷨�Ŀ��� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_dev_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    oal_bool_enum_uint8 chain_sw; /* txmode chain̽��Ŀ��� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_chain_prob_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    uint8_t sub_frame_chain; /* ���ô���֡ͨ�� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_sub_frame_chain_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    uint8_t rssi_threshold; /* rssi���� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_rssi_thre_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    uint8_t uc_beam_change; /* txmode beam_change���������� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_beam_change_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN];      /* user��mac��ַ */
    uint8_t auc_resv[2];                          /* 2 Ԥ����4 �ֽڶ��� */
    alg_cfg_param_tx_mode_enum en_fix_txmode;     /* �̶�txmodeģʽ */
} alg_cfg_param_txmode_user_stru;
typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    alg_cfg_param_tx_mode_enum en_fix_txmode; /* �̶�txmodeģʽ */
} alg_cfg_param_txmode_all_user_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    uint8_t mgmt_tx_chain; /* txmode mgmt���������� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_mgmt_chain_stru;

typedef struct {
    alg_cfg_hdr_stru alg_cfg_hdr;
    uint8_t bcast_data_tx_chain; /* txmode bcast data���������� */
    uint8_t resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_bcast_data_chain_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    oal_bool_enum_uint8 en_11b_tx_switch; /* 11b_tx���� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_11b_tx_sw_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN];      /* user��mac��ַ */
    uint8_t uc_valid_probe_set;                   /* ��ǰ�û�����̽�⼯��ֻ��get���������� */
    uint8_t auc_resv;                             /* Ԥ����4 �ֽڶ��� */
    alg_cfg_param_tx_mode_enum en_tx_default;     /* ��userĬ�Ϸ��ͷ�ʽ */
} alg_cfg_param_txmode_user_info_stru;

typedef struct {
    alg_cfg_hdr_stru st_alg_cfg_hdr;
    uint8_t mac_addr[WLAN_MAC_ADDR_LEN];      /* user��mac��ַ */
    uint8_t fix_chain; /* �̶�����ͨ�� */
    uint8_t resv[1];   /* 1 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_fix_chain_stru;

typedef struct {
    alg_cfg_hdr_stru alg_cfg_hdr;
    uint8_t debug_log_switch;   /* txmode ��־��ӡ���� */
    uint8_t resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_txmode_log_switch_stru;
/* -------------------txmode END------------------- */
/* -------------------waterfilling START--------------------- */
typedef struct {
    alg_cfg_hdr_stru    alg_cfg_hdr;
    oal_bool_enum_uint8 waterfilling_mode; /* waterfilling�㷨ģʽ */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_waterfilling_mode_stru;

typedef struct {
    alg_cfg_hdr_stru    alg_cfg_hdr;
    oal_bool_enum_uint8 waterfilling_enable; /* waterfilling�㷨���� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_waterfilling_enable_stru;

typedef struct {
    alg_cfg_hdr_stru    alg_cfg_hdr;
    oal_bool_enum_uint8 hardamard_mode; /* waterfilling�㷨���� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_hardamard_mode_stru;

typedef struct {
    alg_cfg_hdr_stru    alg_cfg_hdr;
    oal_bool_enum_uint8 hardamard_enable; /* waterfilling�㷨���� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_hardamard_enable_stru;

typedef struct {
    alg_cfg_hdr_stru    st_alg_cfg_hdr;
    int8_t waterfilling_limit; /* waterfilling�㷨���� */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_waterfilling_limit_stru;

typedef struct {
    alg_cfg_hdr_stru    st_alg_cfg_hdr;
    uint8_t waterfilling_asnr[4]; /* 4��waterfillingעˮֵ */
} alg_cfg_param_waterfilling_asnr_stru;

typedef struct {
    alg_cfg_hdr_stru    alg_cfg_hdr;
    oal_bool_enum_uint8 waterfilling_debug; /* 4��waterfillingעˮֵ */
    uint8_t auc_resv[3];  /* 3 Ԥ����4 �ֽڶ��� */
} alg_cfg_param_waterfilling_debug_stru;
/* -------------------waterfilling END---------------------- */

typedef enum {
    /* ҵ������㷨���ò���,����ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_SCHEDULE_START,

    MAC_ALG_CFG_SCHEDULE_SCH_METHOD,
    MAC_ALG_CFG_SCHEDULE_FIX_SCH_MODE,

    MAC_ALG_CFG_SCHEDULE_END,

    /* AUTORATE�㷨���ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_AUTORATE_START,

    MAC_ALG_CFG_AUTORATE_ENABLE,
    MAC_ALG_CFG_AUTORATE_USE_LOWEST_RATE,
    MAC_ALG_CFG_AUTORATE_SHORT_STAT_NUM,
    MAC_ALG_CFG_AUTORATE_SHORT_STAT_SHIFT,
    MAC_ALG_CFG_AUTORATE_LONG_STAT_NUM,
    MAC_ALG_CFG_AUTORATE_LONG_STAT_SHIFT,
    MAC_ALG_CFG_AUTORATE_MIN_PROBE_UP_INTVL_PKTNUM,
    MAC_ALG_CFG_AUTORATE_MIN_PROBE_DOWN_INTVL_PKTNUM,
    MAC_ALG_CFG_AUTORATE_MAX_PROBE_INTVL_PKTNUM,
    MAC_ALG_CFG_AUTORATE_PROBE_INTVL_KEEP_TIMES,
    MAC_ALG_CFG_AUTORATE_DELTA_GOODPUT_RATIO,
    MAC_ALG_CFG_AUTORATE_VI_PROBE_PER_LIMIT,
    MAC_ALG_CFG_AUTORATE_VO_PROBE_PER_LIMIT,
    MAC_ALG_CFG_AUTORATE_AMPDU_DURATION,
    MAC_ALG_CFG_AUTORATE_MCS0_CONT_LOSS_NUM,
    MAC_ALG_CFG_AUTORATE_UP_PROTOCOL_DIFF_RSSI,
    MAC_ALG_CFG_AUTORATE_RTS_MODE,
    MAC_ALG_CFG_AUTORATE_LEGACY_1ST_LOSS_RATIO_TH,
    MAC_ALG_CFG_AUTORATE_HT_VHT_1ST_LOSS_RATIO_TH,
    MAC_ALG_CFG_AUTORATE_LOG_ENABLE,
    MAC_ALG_CFG_AUTORATE_VO_RATE_LIMIT,
    MAC_ALG_CFG_AUTORATE_JUDGE_FADING_PER_TH,
    MAC_ALG_CFG_AUTORATE_MAX_AGGR_NUM,
    MAC_ALG_CFG_AUTORATE_LIMIT_1MPDU_PER_TH,
    MAC_ALG_CFG_AUTORATE_BTCOEX_PROBE_ENABLE,
    MAC_ALG_CFG_AUTORATE_BTCOEX_AGGR_ENABLE,
    MAC_ALG_CFG_AUTORATE_COEX_STAT_INTVL,
    MAC_ALG_CFG_AUTORATE_COEX_LOW_ABORT_TH,
    MAC_ALG_CFG_AUTORATE_COEX_HIGH_ABORT_TH,
    MAC_ALG_CFG_AUTORATE_COEX_AGRR_NUM_ONE_TH,
    MAC_ALG_CFG_AUTORATE_DYNAMIC_BW_ENABLE,
    MAC_ALG_CFG_AUTORATE_THRPT_WAVE_OPT,
    MAC_ALG_CFG_AUTORATE_GOODPUT_DIFF_TH,
    MAC_ALG_CFG_AUTORATE_PER_WORSE_TH,
    MAC_ALG_CFG_AUTORATE_RX_CTS_NO_BA_NUM,
    MAL_ALG_CFG_AUTORATE_VOICE_AGGR,
    MAC_ALG_CFG_AUTORATE_FAST_SMOOTH_SHIFT,
    MAC_ALG_CFG_AUTORATE_FAST_SMOOTH_AGGR_NUM,
    MAC_ALG_CFG_AUTORATE_SGI_PUNISH_PER,
    MAC_ALG_CFG_AUTORATE_SGI_PUNISH_NUM,
    MAL_ALG_CFG_AUTORATE_LAST_RATE_RANK_INDEX,
    MAC_ALG_CFG_AUTORATE_WEAK_RSSI_TH,
    MAC_ALG_CFG_AUTORATE_RXCH_STAT_PERIOD,
    MAC_ALG_CFG_AUTORATE_RTS_ONE_TCP_DBG,
    MAC_ALG_CFG_AUTORATE_SCAN_USER_OPT,
    MAC_ALG_CFG_AUTORATE_MAX_TX_COUNT,
    MAC_ALG_CFG_AUTORATE_COLLISION_DET_EN,
    MAC_ALG_CFG_AUTORATE_SWITCH_11B,
    MAC_ALG_CFG_AUTORATE_ROM_NSS_AUTH,
    MAC_ALG_CFG_AUTORATE_END,

    MAC_ALG_CFG_AUTORATE_AGGR_START,
    MAC_ALG_CFG_AUTORATE_AGGR_OPT,
    MAC_ALG_CFG_AUTORATE_AGGR_MODE,
    MAC_ALG_CFG_AUTORATE_AGGR_TIME_IDX,
    MAC_ALG_CFG_AUTORATE_AGGR_PROBE_INTVL_NUM,
    MAC_ALG_CFG_AUTORATE_AGGR_STAT_SHIFT,
    MAC_ALG_CFG_AUTORATE_DBAC_AGGR_TIME,
    MAC_ALG_CFG_AUTORATE_DBG_VI_STATUS,
    MAC_ALG_CFG_AUTORATE_DBG_AGGR_LOG,
    MAC_ALG_CFG_AUTORATE_AGGR_NON_PROBE_PCK_NUM,
    MAC_ALG_CFG_AUTORATE_AGGR_MIN_AGGR_TIME_IDX,
    MAC_ALG_CFG_AUTORATE_AGGR_250US_DELTA_PER_TH,
    MAC_ALG_CFG_AUTORATE_AGGR_END,

    /* AUTORATE�㷨ϵͳ�����������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_AUTORATE_TEST_START,
    MAC_ALG_CFG_AUTORATE_DISPLAY_RATE_SET,
    MAC_ALG_CFG_AUTORATE_TEST_END,

    /* TXBF�㷨���ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_TXBF_START,
    MAC_ALG_CFG_TXBF_TXMODE_ENABLE,
    MAC_ALG_CFG_TXBF_11N_BFEE_ENABLE,
    MAC_ALG_CFG_TXBF_2G_BFER_ENABLE,
    MAC_ALG_CFG_TXBF_2NSS_BFER_ENABLE,
    MAC_ALG_CFG_TXBF_FIX_MODE,
    MAC_ALG_CFG_TXBF_FIX_SOUNDING,
    MAC_ALG_CFG_TXBF_PROBE_INT,
    MAC_ALG_CFG_TXBF_REMOVE_WORST,
    MAC_ALG_CFG_TXBF_STABLE_NUM,
    MAC_ALG_CFG_TXBF_PROBE_COUNT,
    MAC_ALG_CFG_TXBF_END,

    /* �������㷨���ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_ANTI_INTF_START,

    MAC_ALG_CFG_ANTI_INTF_IMM_ENABLE,
    MAC_ALG_CFG_ANTI_INTF_UNLOCK_ENABLE,
    MAC_ALG_CFG_ANTI_INTF_RSSI_STAT_CYCLE,
    MAC_ALG_CFG_ANTI_INTF_UNLOCK_CYCLE,
    MAC_ALG_CFG_ANTI_INTF_UNLOCK_DUR_TIME,
    MAC_ALG_CFG_ANTI_INTF_NAV_IMM_ENABLE,
    MAC_ALG_CFG_ANTI_INTF_GOODPUT_FALL_TH,
    MAC_ALG_CFG_ANTI_INTF_KEEP_CYC_MAX_NUM,
    MAC_ALG_CFG_ANTI_INTF_KEEP_CYC_MIN_NUM,
    MAC_ALG_CFG_ANTI_INTF_TX_TIME_FALL_TH,
    MAC_ALG_CFG_ANTI_INTF_PER_PROBE_EN,
    MAC_ALG_CFG_ANTI_INTF_PER_FALL_TH,
    MAC_ALG_CFG_ANTI_INTF_GOODPUT_JITTER_TH,
    MAC_ALG_CFG_ANTI_INTF_DEBUG_MODE,

    MAC_ALG_CFG_ANTI_INTF_END,

    /* ���ż���㷨���ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_INTF_DET_START,

    MAC_ALG_CFG_INTF_DET_CYCLE,
    MAC_ALG_CFG_INTF_DET_MODE,
    MAC_ALG_CFG_INTF_DET_DEBUG,
    MAC_ALG_CFG_INTF_DET_COCH_THR_STA,
    MAC_ALG_CFG_INTF_DET_ACI_HIGH_TH,
    MAC_ALG_CFG_INTF_DET_ACI_LOW_TH,
    MAC_ALG_CFG_INTF_DET_ACI_SYNC_TH,
    MAC_ALG_CFG_INTF_DET_COCH_NOINTF_STA,
    MAC_ALG_CFG_INTF_DET_COCH_THR_UDP,
    MAC_ALG_CFG_INTF_DET_COCH_THR_TCP,
    MAC_ALG_CFG_INTF_DET_ADJCH_SCAN_CYC,
    MAC_ALG_CFG_INTF_DET_ADJRATIO_THR,
    MAC_ALG_CFG_INTF_DET_SYNC_THR,
    MAC_ALG_CFG_INTF_DET_AVE_RSSI,
    MAC_ALG_CFG_INTF_DET_NO_ADJRATIO_TH,
    MAC_ALG_CFG_INTF_DET_NO_ADJCYC_TH,
    MAC_ALG_CFG_INTF_DET_COLLISION_TH,
    MAC_ALG_CFG_NEG_DET_NONPROBE_TH,

    MAC_ALG_CFG_INTF_DET_END,

    /* EDCA�Ż��㷨���ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_EDCA_OPT_START,

    MAC_ALG_CFG_EDCA_OPT_AP_EN_MODE,
    MAC_ALG_CFG_EDCA_OPT_STA_EN,
    MAC_ALG_CFG_TXOP_LIMIT_STA_EN,

    MAC_ALG_CFG_EDCA_OPT_END,

    /* CCA�Ż��㷨���ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_CCA_OPT_START,

    MAC_ALG_CFG_CCA_OPT_ALG_EN_MODE,
    MAC_ALG_CFG_CCA_OPT_DEBUG_MODE,
    MAC_ALG_CFG_CCA_OPT_SET_CCA_TH_DEBUG,
    MAC_ALG_CFG_CCA_OPT_LOG,

    MAC_ALG_CFG_CCA_OPT_END,

    /* �㷨��־���ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_INTF_DET_STAT_LOG_START,
    MAC_ALG_CFG_INTF_DET_STAT_LOG_WRITE,

    /* TPC�㷨���ò���, ����ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_TPC_START,

    MAC_ALG_CFG_TPC_MODE,
    MAC_ALG_CFG_TPC_DEBUG,
    MAC_ALG_CFG_TPC_LEVEL,
    MAC_ALG_CFG_TPC_LOG,
    MAC_ALG_CFG_TPC_OVER_TMP_TH,
    MAC_ALG_CFG_TPC_DPD_ENABLE_RATE,
    MAC_ALG_CFG_TPC_TARGET_RATE_11B,
    MAC_ALG_CFG_TPC_TARGET_RATE_11AG,
    MAC_ALG_CFG_TPC_TARGET_RATE_HT20,
    MAC_ALG_CFG_TPC_TARGET_RATE_HT40,
    MAC_ALG_CFG_TPC_TARGET_RATE_VHT20,
    MAC_ALG_CFG_TPC_TARGET_RATE_VHT40,
    MAC_ALG_CFG_TPC_TARGET_RATE_VHT80,
    MAC_ALG_CFG_TPC_TARGET_RATE_VHT160,

    MAC_ALG_CFG_TPC_END,

    /* TPC�㷨��־���ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_TPC_GET_FRAME_POW,

#ifdef _PRE_WLAN_FEATURE_MU_TRAFFIC_CTL
    /* ���û����������㷨���ò��� */
    MAC_ALG_CFG_TRAFFIC_CTL_START,

    MAC_ALG_CFG_TRAFFIC_CTL_ENABLE,
    MAC_ALG_CFG_TRAFFIC_CTL_TIMEOUT,
    MAC_ALG_CFG_TRAFFIC_CTL_MIN_THRESHOLD,
    MAC_ALG_CFG_TRAFFIC_CTL_LOG_DEBUG,
    MAC_ALG_CFG_TRAFFIC_CTL_BUF_THRESHOLD,
    MAC_ALG_CFG_TRAFFIC_CTL_BUF_ADJ_ENABLE,
    MAC_ALG_CFG_TRAFFIC_CTL_BUF_ADJ_NUM,
    MAC_ALG_CFG_TRAFFIC_CTL_BUF_ADJ_CYCLE,
    MAC_ALG_CFG_TRAFFIC_CTL_RX_RESTORE_ENABLE,
    MAC_ALG_CFG_TRAFFIC_RX_RESTORE_NUM,
    MAC_ALG_CFG_TRAFFIC_RX_RESTORE_THRESHOLD,
    MAC_ALG_CFG_TRAFFIC_CTL_END,

    /* ��device���ն������������㷨���ò��� */
    MAC_ALG_CFG_RX_DSCR_CTL_START,
    MAC_ALG_CFG_RX_DSCR_CTL_ENABLE,
    MAC_ALG_CFG_RX_DSCR_CTL_LOG_DEBUG,
    MAC_ALG_CFG_RX_DSCR_CTL_END,
#endif

    MAC_ALG_CFG_SPATIAL_REUSE_START = 155,
    MAC_ALG_CFG_NON_SRG_ENABLE,
    MAC_ALG_CFG_SRG_ENABLE,
    MAC_ALG_CFG_SR_LOG_ENABLE,
    MAC_ALG_CFG_SR_DUTY_TH,
    MAC_ALG_CFG_SPATIAL_REUSE_END,
    /* HiD2D��ؽӿ����ò���������ӵ���Ӧ��START��END֮�� */
    MAC_ALG_CFG_HID2D_START,
    MAC_ALG_CFG_HID2D_DEBUG_ENABLE,
    MAC_ALG_CFG_HID2D_RTS_ENABLE,
    MAC_ALG_CFG_HID2D_HIGH_BW_MCS_DISABLE,
    MAC_ALG_CFG_HID2D_HIGH_TXPOWER_ENABLE,
    MAC_ALG_CFG_HID2D_SET_APK_CCA_TH,
    MAC_ALG_CFG_HID2D_END,

    MAC_ALG_CFG_BUTT
} mac_alg_cfg_param_enum;
typedef uint8_t mac_alg_cfg_param_enum_uint8;

/* �㷨����ö�٣�����ֵ */
typedef struct {
    mac_alg_cfg_param_enum_uint8 en_alg_cfg; /* ��������ö�� */
    uint8_t uc_resv[NUM_3_BYTES];              /* �ֽڶ��� */
    uint32_t value;               /* ���ò���ֵ */
} mac_ioctl_alg_param_stru;

/* AUTORATE ������ص�������� */
typedef struct {
    mac_alg_cfg_param_enum_uint8 en_alg_cfg;         /* ��������ö�� */
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC��ַ */
    uint8_t auc_resv[1];
    uint16_t us_value; /* ������� */
} mac_ioctl_alg_ar_test_param_stru;

/* �㷨��������ӿ� */
typedef struct {
    uint8_t uc_argc;
    uint8_t auc_argv_offset[DMAC_ALG_CONFIG_MAX_ARG];
} mac_ioctl_alg_config_stru; /* hd_event */

/*****************************************************************************
  8 UNION����
*****************************************************************************/
#endif /* end of alg_cfg.h */
