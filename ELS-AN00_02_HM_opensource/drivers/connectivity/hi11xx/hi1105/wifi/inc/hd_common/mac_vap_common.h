
#ifndef __MAC_VAP_COMMON_H__
#define __MAC_VAP_COMMON_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "mac_ftm_common.h"

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define MAC_STR_CMD_MAX_SIZE 16

#define MAC_FCS_DBAC_IGNORE     0 /* ����DBAC���� */
#define MAC_FCS_DBAC_NEED_CLOSE 1 /* DBAC��Ҫ�ر� */
#define MAC_FCS_DBAC_NEED_OPEN  2 /* DBAC��Ҫ���� */

#define MAC_NUM_DR_802_11A  8 /* 11A 5gģʽʱ����������(DR)���� */
#define MAC_NUM_BR_802_11A  3 /* 11A 5gģʽʱ�Ļ�������(BR)���� */
#define MAC_NUM_NBR_802_11A 5 /* 11A 5gģʽʱ�ķǻ�������(NBR)���� */

#define MAC_NUM_DR_802_11B  4 /* 11B 2.4Gģʽʱ����������(DR)���� */
#define MAC_NUM_BR_802_11B  2 /* 11B 2.4Gģʽʱ����������(BR)���� */
#define MAC_NUM_NBR_802_11B 2 /* 11B 2.4Gģʽʱ����������(NBR)���� */

#define MAC_NUM_DR_802_11G  8 /* 11G 2.4Gģʽʱ����������(DR)���� */
#define MAC_NUM_BR_802_11G  3 /* 11G 2.4Gģʽʱ�Ļ�������(BR)���� */
#define MAC_NUM_NBR_802_11G 5 /* 11G 2.4Gģʽʱ�ķǻ�������(NBR)���� */

#define MAC_NUM_DR_802_11G_MIXED      12 /* 11G ���ģʽʱ����������(DR)���� */
#define MAC_NUM_BR_802_11G_MIXED_ONE  4  /* 11G ���1ģʽʱ�Ļ�������(BR)���� */
#define MAC_NUM_NBR_802_11G_MIXED_ONE 8  /* 11G ���1ģʽʱ�ķǻ�������(NBR)���� */

#define MAC_NUM_BR_802_11G_MIXED_TWO  7 /* 11G ���2ģʽʱ�Ļ�������(BR)���� */
#define MAC_NUM_NBR_802_11G_MIXED_TWO 5 /* 11G ���2ģʽʱ�ķǻ�������(NBR)���� */

/* 11N MCS��ص����� */
#define MAC_MAX_RATE_SINGLE_NSS_20M_11N 0 /* 1���ռ���20MHz��������� */
#define MAC_MAX_RATE_SINGLE_NSS_40M_11N 0 /* 1���ռ���40MHz��������� */
#define MAC_MAX_RATE_DOUBLE_NSS_20M_11N 0 /* 2���ռ���20MHz��������� */
#define MAC_MAX_RATE_DOUBLE_NSS_40M_11N 0 /* 2���ռ���40MHz��������� */
#define MAC_MAX_RATE_TRIPLE_NSS_20M_11N 0 /* 3���ռ���20MHz��������� */
#define MAC_MAX_RATE_TRIPLE_NSS_40M_11N 0 /* 3���ռ���40MHz��������� */
#define MAC_MAX_RATE_FOUR_NSS_20M_11N   0 /* 4���ռ���20MHz��������� */
#define MAC_MAX_RATE_FOUR_NSS_40M_11N   0 /* 4���ռ���40MHz��������� */

/* 11AC MCS��ص����� */
#define MAC_MAX_SUP_MCS7_11AC_EACH_NSS  0 /* 11AC���ռ���֧�ֵ����MCS��ţ�֧��0-7 */
#define MAC_MAX_SUP_MCS8_11AC_EACH_NSS  1 /* 11AC���ռ���֧�ֵ����MCS��ţ�֧��0-8 */
#define MAC_MAX_SUP_MCS9_11AC_EACH_NSS  2 /* 11AC���ռ���֧�ֵ����MCS��ţ�֧��0-9 */
#define MAC_MAX_UNSUP_MCS_11AC_EACH_NSS 3 /* 11AC���ռ���֧�ֵ����MCS��ţ���֧��n���ռ��� */

#ifdef _PRE_WLAN_FEATURE_1024QAM
#define MAC_MAX_RATE_SINGLE_NSS_20M_11AC  107  /* 1���ռ���20MHz�����Long GI���� */
#define MAC_MAX_RATE_SINGLE_NSS_40M_11AC  225  /* 1���ռ���40MHz�����Long GI���� */
#define MAC_MAX_RATE_SINGLE_NSS_80M_11AC  487  /* 1���ռ���80MHz�����Long GI���� */
#define MAC_MAX_RATE_SINGLE_NSS_160M_11AC 975  /* 1���ռ���160MHz�����Long GI���� */
#define MAC_MAX_RATE_DOUBLE_NSS_20M_11AC  216  /* 2���ռ���20MHz�����Long GI���� */
#define MAC_MAX_RATE_DOUBLE_NSS_40M_11AC  450  /* 2���ռ���40MHz�����Long GI���� */
#define MAC_MAX_RATE_DOUBLE_NSS_80M_11AC  975  /* 2���ռ���80MHz�����Long GI���� */
#define MAC_MAX_RATE_DOUBLE_NSS_160M_11AC 1950 /* 2���ռ���160MHz�����Long GI���� */
#define MAC_MAX_RATE_TRIPLE_NSS_20M_11AC  325  /* 3���ռ���20MHz�����Long GI���� */
#define MAC_MAX_RATE_TRIPLE_NSS_40M_11AC  675  /* 3���ռ���40MHz�����Long GI���� */
#define MAC_MAX_RATE_TRIPLE_NSS_80M_11AC  1462  /* 3���ռ���80MHz�����Long GI���� */
#define MAC_MAX_RATE_TRIPLE_NSS_160M_11AC 2925 /* 3���ռ���160MHz�����Long GI���� */
#define MAC_MAX_RATE_FOUR_NSS_20M_11AC  433  /* 4���ռ���20MHz�����Long GI���� */
#define MAC_MAX_RATE_FOUR_NSS_40M_11AC  900  /* 4���ռ���40MHz�����Long GI���� */
#define MAC_MAX_RATE_FOUR_NSS_80M_11AC  1950  /* 4���ռ���80MHz�����Long GI���� */
#define MAC_MAX_RATE_FOUR_NSS_160M_11AC 3900 /* 4���ռ���160MHz�����Long GI���� */
#else
/* ����Э��Ҫ��(9.4.2.158.3�½�)���޸�Ϊlong gi���� */
#define MAC_MAX_RATE_SINGLE_NSS_20M_11AC  86   /* 1���ռ���20MHz��������� */
#define MAC_MAX_RATE_SINGLE_NSS_40M_11AC  180  /* 1���ռ���40MHz��������� */
#define MAC_MAX_RATE_SINGLE_NSS_80M_11AC  390  /* 1���ռ���80MHz��������� */
#define MAC_MAX_RATE_SINGLE_NSS_160M_11AC 780  /* 1���ռ���160MHz��������� */
#define MAC_MAX_RATE_DOUBLE_NSS_20M_11AC  173  /* 2���ռ���20MHz��������� */
#define MAC_MAX_RATE_DOUBLE_NSS_40M_11AC  360  /* 2���ռ���40MHz��������� */
#define MAC_MAX_RATE_DOUBLE_NSS_80M_11AC  780  /* 2���ռ���80MHz��������� */
#define MAC_MAX_RATE_DOUBLE_NSS_160M_11AC 1560 /* 2���ռ���160MHz��������� */
#define MAC_MAX_RATE_TRIPLE_NSS_20M_11AC  260  /* 3���ռ���20MHz�����Long GI���� */
#define MAC_MAX_RATE_TRIPLE_NSS_40M_11AC  540  /* 3���ռ���40MHz�����Long GI���� */
#define MAC_MAX_RATE_TRIPLE_NSS_80M_11AC  1170  /* 3���ռ���80MHz�����Long GI���� */
#define MAC_MAX_RATE_TRIPLE_NSS_160M_11AC 2106 /* 3���ռ���160MHz�����Long GI���� */
#define MAC_MAX_RATE_FOUR_NSS_20M_11AC  312  /* 4���ռ���20MHz�����Long GI���� */
#define MAC_MAX_RATE_FOUR_NSS_40M_11AC  720  /* 4���ռ���40MHz�����Long GI���� */
#define MAC_MAX_RATE_FOUR_NSS_80M_11AC  1560  /* 4���ռ���80MHz�����Long GI���� */
#define MAC_MAX_RATE_FOUR_NSS_160M_11AC 3120 /* 4���ռ���160MHz�����Long GI���� */
#endif

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
typedef enum {
    MAC_SET_BEACON = 0,
    MAC_ADD_BEACON = 1,

    MAC_BEACON_OPERATION_BUTT
} mac_beacon_operation_type;
typedef uint8_t mac_beacon_operation_type_uint8;

typedef enum {
    AMPDU_SWITCH_BY_DEL_BA = 0,
    AMPDU_SWITCH_BY_BA_LUT,

    AMPDU_SWITCH_MODE_BUTT
} mac_ampdu_switch_mode;
typedef uint8_t mac_ampdu_switch_mode_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_TCP_ACK_BUFFER
typedef enum {
    MAC_TCP_ACK_BUF_ENABLE,
    MAC_TCP_ACK_BUF_TIMEOUT,
    MAC_TCP_ACK_BUF_MAX,

    MAC_TCP_ACK_BUF_TYPE_BUTT
} mac_tcp_ack_buf_cfg_cmd_enum;
typedef uint8_t mac_tcp_ack_buf_cfg_cmd_enum_uint8;
#endif

typedef enum {
    MAC_RSSI_LIMIT_SHOW_INFO,
    MAC_RSSI_LIMIT_ENABLE,
    MAC_RSSI_LIMIT_DELTA,
    MAC_RSSI_LIMIT_THRESHOLD,

    MAC_RSSI_LIMIT_TYPE_BUTT
} mac_rssi_limit_type_enum;
typedef uint8_t mac_rssi_limit_type_enum_uint8;

/* ��������ά���������� */
typedef enum {
    MAC_SET_POW_RF_REG_CTL = 0, /* �����Ƿ�RF�Ĵ����� */
    MAC_SET_POW_FIX_LEVEL,      /* �̶����ʵȼ� */
    MAC_SET_POW_MAG_LEVEL,      /* ����֡���ʵȼ� */
    MAC_SET_POW_CTL_LEVEL,      /* ����֡���ʵȼ� */
    MAC_SET_POW_AMEND,          /* ����UPC code */
    MAC_SET_POW_NO_MARGIN,      /* ���ʲ������� */
    MAC_SET_POW_SHOW_LOG,       /* ��־��ʾ */
    MAC_SET_POW_SAR_LVL_DEBUG,  /* ��sar  */
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
    MAC_TAS_POW_CTRL_DEBUG,  /* TAS������  */
    MAC_TAS_RSSI_CALI_DEBUG, /* TAS���߲���  */
    MAC_TAS_ANT_SWITCH,      /* TAS�����л�  */
#endif
    MAC_SHOW_TPC_TABLE_GAIN,
    MAC_POW_SAVE,
    MAC_POW_GET_PD_INFO,
    MAC_POW_SET_TPC_IDX,

    MAC_SET_POW_BUTT
} mac_set_pow_type_enum;
typedef uint8_t mac_set_pow_type_enum_uint8;

typedef enum {
    MAC_VAP_CONFIG_UCAST_DATA = 0,
    MAC_VAP_CONFIG_MCAST_DATA,
    MAC_VAP_CONFIG_BCAST_DATA,
    MAC_VAP_CONFIG_UCAST_MGMT_2G,
    MAC_VAP_CONFIG_UCAST_MGMT_5G,
    MAC_VAP_CONFIG_MBCAST_MGMT_2G,
    MAC_VAP_CONFIG_MBCAST_MGMT_5G,
    MAC_VAP_CONFIG_BUTT,
} mac_vap_config_dscr_frame_type_enum;
typedef uint8_t mac_vap_config_dscr_frame_type_uint8;

typedef enum { /* hi1102-cb */
    SHORTGI_20_CFG_ENUM,
    SHORTGI_40_CFG_ENUM,
    SHORTGI_80_CFG_ENUM,
    SHORTGI_BUTT_CFG
} short_gi_cfg_type;

/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
typedef struct {
    uint8_t uc_announced_channel;                           /* ���ŵ��� */
    wlan_channel_bandwidth_enum_uint8 en_announced_bandwidth; /* �´���ģʽ */
    uint8_t uc_ch_switch_cnt;                               /* �ŵ��л����� */
    wlan_ch_switch_status_enum_uint8 en_ch_switch_status;     /* �ŵ��л�״̬ */
    wlan_bw_switch_status_enum_uint8 en_bw_switch_status;     /* �����л�״̬ */
    oal_bool_enum_uint8 en_csa_present_in_bcn;                /* Beacon֡���Ƿ����CSA IE */
    wlan_csa_mode_tx_enum_uint8 en_csa_mode;
    uint8_t uc_start_chan_idx;
    uint8_t uc_end_chan_idx;
    wlan_channel_bandwidth_enum_uint8 en_user_pref_bandwidth;

    /* VAPΪSTAʱ�����г�Ա
     *
     *  ---|--------|--------------------|-----------------|-----------
     *     0        3                    0                 0
     *     X        A                    B                 C
     *
     *  sta���ŵ��л���ͨ����ͼ�������, ����Ϊ�л���������
     *  X->A A֮ǰΪδ���������ŵ��л�ʱ,�л�������Ϊ0
     *  ��A->Bʱ���Ϊsta�ȴ��л�״̬: en_waiting_to_shift_channelΪtrue
     *  ��B->CΪsta�ŵ��л���,���ȴ�ap��beacon״̬: en_waiting_for_apΪtrue
     *  C-> Ϊsta�յ���ap��beacon����׼�ŵ��л�����
     *
     *  A��ͨ�����յ�csa ie(beacon/action...), B��ͨ��Ϊtbtt�ж����л���������Ϊ
     *  0����csa ie�м�����Ϊ0��C����Ϊ�յ�beacon
     *
     *  ��A->C�Ĺ����У�������ظ��յ���csa ie�����ŵ��л�����
     *
     */
    uint8_t uc_new_channel;                           /* ���Կ��Ǹ�����ϲ� */
    wlan_channel_bandwidth_enum_uint8 en_new_bandwidth; /* ���Կ��Ǹ�����ϲ� */
    oal_bool_enum_uint8 en_waiting_to_shift_channel;    /* �ȴ��л��ŵ� */
    uint8_t auc_rsv2[1];
    oal_bool_enum_uint8 en_te_b;
    uint8_t bit_wait_bw_change;        /* �յ�action֡,�ȴ��л����� */
    uint8_t uc_rsv1[NUM_2_BYTES];       /* ap��һ�η��͵��л����� */
    uint8_t uc_linkloss_change_chanel; /* ���Կ��Ǹ�����ϲ� */
    wlan_linkloss_scan_switch_chan_enum_uint8 en_linkloss_scan_switch_chan;
    uint32_t chan_report_for_te_a;
    mac_channel_stru st_old_channel; /* �ŵ��л�ʱ�����л�ǰ�ŵ���Ϣ */
    uint8_t _rom[NUM_4_BYTES];
} mac_ch_switch_info_stru;

#ifdef _PRE_WLAN_FEATURE_TWT
/* TWT���� */
typedef struct {
    uint64_t ull_twt_start_time;
    uint64_t ull_twt_interval;
    uint32_t twt_duration; /* Nominal Minimum TWT Wake Duration */
    uint8_t uc_twt_flow_id;
    uint8_t uc_twt_announce_bit;
    uint8_t uc_twt_session_enable;
    uint8_t uc_twt_is_trigger_enabled;
    uint8_t uc_next_twt_size;
    uint8_t uc_twt_dialog_token;
    uint8_t twt_type; /* 0Ϊ���� 1Ϊ�㲥 */
    uint8_t twt_idx; /* ʹ��vap�µڼ�����Դ */
} mac_cfg_twt_stru;
#endif

/* CHR2.0ʹ�õ�STAͳ����Ϣ */
typedef struct {
    uint8_t uc_distance;      /* �㷨��tpc���룬��Ӧdmac_alg_tpc_user_distance_enum */
    uint8_t uc_cca_intr;      /* �㷨��cca_intr���ţ���Ӧalg_cca_opt_intf_enum */
    int8_t c_snr_ant0;        /* ����0�ϱ���SNRֵ */
    int8_t c_snr_ant1;        /* ����1�ϱ���SNRֵ */
    uint32_t bcn_cnt;      /* �յ���beacon���� */
    uint32_t bcn_tout_cnt; /* beacon��ʧ�ļ��� */
} station_info_extend_stru;

/*****************************************************************************
  8 UNION����
*****************************************************************************/
typedef enum mac_rate_info_flags {
    MAC_RATE_INFO_FLAGS_MCS = BIT(0),
    MAC_RATE_INFO_FLAGS_VHT_MCS = BIT(1),
    MAC_RATE_INFO_FLAGS_40_MHZ_WIDTH = BIT(2),
    MAC_RATE_INFO_FLAGS_80_MHZ_WIDTH = BIT(3),
    MAC_RATE_INFO_FLAGS_SHORT_MIDDLE = BIT(4),
    MAC_RATE_INFO_FLAGS_160_MHZ_WIDTH = BIT(5),
    MAC_RATE_INFO_FLAGS_SHORT_GI = BIT(6),
    MAC_RATE_INFO_FLAGS_HE_MCS = BIT(7),
} mac_rate_info_flags_enum;

typedef enum {
    MAC_WMM_SET_PARAM_TYPE_DEFAULT,
    MAC_WMM_SET_PARAM_TYPE_UPDATE_EDCA,

    MAC_WMM_SET_PARAM_TYPE_BUTT
} mac_wmm_set_param_type_enum;
typedef uint8_t mac_wmm_set_param_type_enum_uint8;

typedef struct {
    /* word 0 */
    wlan_prot_mode_enum_uint8 en_protection_mode; /* ����ģʽ */
    uint8_t uc_obss_non_erp_aging_cnt;          /* ָʾOBSS��non erp վ����ϻ�ʱ�� */
    uint8_t uc_obss_non_ht_aging_cnt;           /* ָʾOBSS��non ht վ����ϻ�ʱ�� */
    uint8_t bit_auto_protection : 1;            /* ָʾ���������Ƿ�����OAL_SWITCH_ON �򿪣� OAL_SWITCH_OFF �ر� */
    uint8_t bit_obss_non_erp_present : 1;       /* ָʾobss���Ƿ����non ERP��վ�� */
    uint8_t bit_obss_non_ht_present : 1;        /* ָʾobss���Ƿ����non HT��վ�� */
    uint8_t bit_rts_cts_protect_mode : 1;       /* ָrts_cts ���������Ƿ��, OAL_SWITCH_ON �򿪣� OAL_SWITCH_OFF �ر� */
    uint8_t bit_lsig_txop_protect_mode : 1;     /* ָʾL-SIG protect�Ƿ��, OAL_SWITCH_ON �򿪣� OAL_SWITCH_OFF �ر� */
    uint8_t bit_reserved : 3;

    /* word 1 */
    uint8_t uc_sta_no_short_slot_num;     /* ��֧��short slot��STA���� */
    uint8_t uc_sta_no_short_preamble_num; /* ��֧��short preamble��STA���� */
    uint8_t uc_sta_non_erp_num;           /* ��֧��ERP��STA���� */
    uint8_t uc_sta_non_ht_num;            /* ��֧��HT��STA���� */
    /* word 2 */
    uint8_t uc_sta_non_gf_num;        /* ֧��ERP/HT,��֧��GF��STA���� */
    uint8_t uc_sta_20M_only_num;      /* ֻ֧��20M Ƶ�ε�STA���� */
    uint8_t uc_sta_no_40dsss_cck_num; /* ����40M DSSS-CCK STA����  */
    uint8_t uc_sta_no_lsig_txop_num;  /* ��֧��L-SIG TXOP Protection STA���� */
} mac_protection_stru;

/* VoWiFi�ź��������� �� ���ò����ṹ�� */
typedef enum {
    VOWIFI_DISABLE_REPORT = 0,
    VOWIFI_LOW_THRES_REPORT,
    VOWIFI_HIGH_THRES_REPORT,
    VOWIFI_CLOSE_REPORT = 3, /* �ر�VoWIFI */

    VOWIFI_MODE_BUTT = 3
} mac_vowifi_mode;

/*
 * enum nl80211_mfp - Management frame protection state
 * @NL80211_MFP_NO: Management frame protection not used
 * @NL80211_MFP_REQUIRED: Management frame protection required
 */
typedef enum {
    MAC_NL80211_MFP_NO,
    MAC_NL80211_MFP_REQUIRED,

    MAC_NL80211_MFP_BUTT
} mac_nl80211_mfp_enum;
typedef uint8_t mac_nl80211_mfp_enum_uint8;

/* ����80211����֡������Ҫ�Ĳ��� */
typedef struct {
    uint8_t bit_msdu_dscr : 1,
              bit_ba_info : 1,
              bit_himit_dscr : 1,
              bit_mu_dscr : 1,
              bit_rsv : 4;
} oam_sub_switch_stru;

typedef union {
    uint8_t value;
    oam_sub_switch_stru stru;
} oam_sub_switch_union;

typedef struct {
    uint8_t en_frame_direction;
    uint8_t en_frame_type;
    oal_switch_enum_uint8 en_frame_switch;
    oal_switch_enum_uint8 en_cb_switch;
    oal_switch_enum_uint8 en_dscr_switch;
    oam_sub_switch_union sub_switch;
    uint8_t auc_user_macaddr[WLAN_MAC_ADDR_LEN];
} mac_cfg_80211_ucast_switch_stru; /* hd_event */

/* ����80211�鲥\�㲥֡������Ҫ�Ĳ��� */
typedef struct {
    uint8_t en_frame_direction;
    uint8_t en_frame_type;
    oal_switch_enum_uint8 en_frame_switch;
    oal_switch_enum_uint8 en_cb_switch;
    oal_switch_enum_uint8 en_dscr_switch;
    oam_sub_switch_union  sub_switch;
    uint8_t auc_resv[NUM_2_BYTES];
} mac_cfg_80211_mcast_switch_stru;

/* ����probe request��probe response������Ҫ�Ĳ��� */
typedef struct {
    uint8_t en_frame_direction;
    oal_switch_enum_uint8 en_frame_switch;
    oal_switch_enum_uint8 en_cb_switch;
    oal_switch_enum_uint8 en_dscr_switch;
    oam_sub_switch_union  sub_switch;
    uint8_t             resv[NUM_3_BYTES];
} mac_cfg_probe_switch_stru;

typedef enum {
    MAC_LOG_ENCAP_AMPDU,
    MAC_LOG_SET_ALG_OTA,
    MAC_LOG_SET_TEMP_PROTECT_LOG,
    MAC_LOG_SET_HIMIT_PROBE_ENABLE,
    MAC_LOG_SET_HIMIT_HTC,
    MAC_LOG_MONITOR_OTA_RPT

} mac_debug_log_type_enum;

#ifdef _PRE_WLAN_FEATURE_HID2D_TX_DROP
typedef struct {
    uint8_t uc_hid2d_debug_en;
    uint8_t uc_hid2d_delay_time;
    uint16_t us_resv;
} mac_hid2d_debug_switch_stru;
extern mac_hid2d_debug_switch_stru g_st_hid2d_debug_switch;
#endif

typedef struct {
    wlan_data_collect_enum_uint8 uc_type;
    uint32_t reg_num;
} mac_data_collect_cfg_stru;
extern mac_data_collect_cfg_stru g_st_data_collect_cfg;

typedef struct {
    uint8_t cac_channel;
    uint8_t cac_bandwidth;
} mac_cfg_cac_stru;

#endif /* end of mac_vap_common.h */
