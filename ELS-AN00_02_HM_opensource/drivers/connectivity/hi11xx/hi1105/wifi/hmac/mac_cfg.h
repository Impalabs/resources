

#ifndef __MAC_CFG_H__
#define __MAC_CFG_H__

/* 1 ����ͷ�ļ����� */
#include "wlan_customize.h"
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_CFG_H

#define MAC_EXT_PA_GAIN_MAX_LVL    2     /* ���ƻ�����PA���λ�� */
#define CUS_RF_PA_BIAS_REG_NUM     43    /* ���ƻ�RF����PAƫ�üĴ�������� */
#define CUS_DY_CALI_PARAMS_NUM     14    /* ��̬У׼��������,2.4g 3��(ofdm 20/40 11b cw),5g 5*2��band */
#define CUS_DY_CALI_PARAMS_TIMES   3     /* ��̬У׼����������ϵ������ */
#define CUS_DY_CALI_DPN_PARAMS_NUM 4     /* ���ƻ���̬У׼2.4G DPN��������11b OFDM20/40 CW OR 5G 160/80/40/20 */
#define CUS_DY_CALI_2G_VAL_DPN_MAX 50  /* ��̬У׼2g dpn��ȡnvram���ֵ */
#define CUS_DY_CALI_2G_VAL_DPN_MIN (-50) /* ��̬У׼2g dpn��ȡnvram��Сֵ */
#define CUS_DY_CALI_5G_VAL_DPN_MAX 50  /* ��̬У׼5g dpn��ȡnvram���ֵ */
#define CUS_DY_CALI_5G_VAL_DPN_MIN (-50) /* ��̬У׼5g dpn��ȡnvram��Сֵ */

/* CFG VAP h2d */
typedef struct {
    oal_net_device_stru *pst_net_dev;
} mac_cfg_vap_h2d_stru;


/* ����֡��FCSͳ����Ϣ */
typedef struct {
    uint32_t data_op;    /* ���ݲ���ģʽ:<0>����,<1>��� */
    uint32_t print_info; /* ��ӡ��������:<0>�������� <1>��֡�� <2>self fcs correct, <3>other fcs correct, <4>fcs error  */
} mac_cfg_rx_fcs_info_stru;

/* �޳��û������������ */
typedef struct {
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC��ַ */
    uint16_t us_reason_code;                 /* ȥ���� reason code */
} mac_cfg_kick_user_param_stru;

/* BA�Ự��ص������������ */
typedef struct {
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* �û���MAC ADDR */
    uint8_t uc_tidno;                        /* ��Ӧ��tid�� */
    mac_ba_policy_enum_uint8 en_ba_policy;     /* BAȷ�ϲ��� */
    uint16_t us_buff_size;                   /* BA���ڵĴ�С */
    uint16_t us_timeout;                     /* BA�Ự�ĳ�ʱʱ�� */
} mac_cfg_addba_req_param_stru;

typedef struct {
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN];   /* �û���MAC ADDR */
    uint8_t uc_tidno;                          /* ��Ӧ��tid�� */
    mac_delba_initiator_enum_uint8 en_direction; /* ɾ��ba�Ự�ķ���� */
    mac_delba_trigger_enum_uint8 en_trigger;     /* ɾ��ԭ�� */
} mac_cfg_delba_req_param_stru;

/* ��ȡ��ǰ������������ַ���������ṹ�� */
typedef struct {
    int8_t ac_country[NUM_3_BYTES];
    uint8_t auc_resv[1];
} mac_cfg_get_country_stru;

/* query��Ϣ��ʽ:2�ֽ�WID x N */
typedef struct {
    wlan_tidno_enum_uint8 en_tid;
    uint8_t uc_resv[NUM_3_BYTES];
} mac_cfg_get_tid_stru;

/* P2P ���ܿ������� */
typedef struct {
    uint8_t uc_p2p_statistics_ctrl; /* 0:���P2P ͳ��ֵ�� 1:��ӡ���ͳ��ֵ */
    uint8_t auc_rsv[NUM_3_BYTES];
} mac_cfg_p2p_stat_param_stru;

/* ������̫��������Ҫ�Ĳ��� */
typedef struct {
    oam_ota_frame_direction_type_enum_uint8 en_frame_direction;
    oal_switch_enum_uint8 en_switch;
    uint8_t auc_user_macaddr[WLAN_MAC_ADDR_LEN];
} mac_cfg_eth_switch_param_stru; /* hd_event */

typedef struct {
    oal_bool_enum_uint8 en_rate_cfg_tag;
    oal_bool_enum_uint8 en_tx_ampdu_last;
    uint8_t uc_reserv[NUM_2_BYTES];
} mac_fix_rate_cfg_para_stru;

/* STA UAPSD �������� */
/* �ýṹ��host��device����,�豣֤�ṹ��һ�� */
typedef struct {
    uint8_t uc_max_sp_len;
    uint8_t uc_delivery_enabled[WLAN_WME_AC_BUTT];
    uint8_t uc_trigger_enabled[WLAN_WME_AC_BUTT];
} mac_cfg_uapsd_sta_stru;

#ifdef _PRE_WLAN_FEATURE_11R
#define MAC_MAX_FTE_LEN 257

typedef struct {
    uint16_t us_mdid; /* Mobile Domain ID */
    uint16_t us_len;  /* FTE �ĳ��� */
    uint8_t auc_ie[MAC_MAX_FTE_LEN];
} mac_cfg80211_ft_ies_stru; // only host
#endif  // _PRE_WLAN_FEATURE_11R


#ifdef _PRE_WLAN_DFT_STAT
typedef struct {
    uint8_t auc_user_macaddr[WLAN_MAC_ADDR_LEN];
    uint8_t uc_param;
    uint8_t uc_resv;
} mac_cfg_usr_queue_param_stru;
#endif

typedef struct {
    uint32_t coext_info;
    uint32_t channel_report;
} mac_cfg_set_2040_coexist_stru; // only host

typedef struct {
    uint8_t uc_scan_type;
    oal_bool_enum_uint8 en_current_bss_ignore;
    uint8_t auc_bssid[OAL_MAC_ADDR_LEN];
    oal_bool_enum_uint8 is_roaming_trigged_by_cmd; // �Ƿ��������������
    uint8_t auc_resv[NUM_3_BYTES];
} mac_cfg_set_roam_start_stru; // only host

typedef struct {
    uint8_t auc_bssid[WLAN_MAC_ADDR_LEN];
    uint8_t auc_resv0[NUM_2_BYTES];
    uint8_t auc_pmkid[WLAN_PMKID_LEN];
} mac_cfg_pmksa_param_stru; // only host

/* ���ƻ� power ref 2g 5g���ò��� */
/* customize rf cfg struct */
typedef struct {
    int8_t c_rf_gain_db_mult4; /* �ⲿPA/LNA bypassʱ������(����0.25dB) */
    uint8_t auc_resv[NUM_3_BYTES];
} mac_cfg_gain_db_per_band;

typedef struct {
    /* 2g */
    mac_cfg_gain_db_per_band ac_gain_db_2g[MAC_NUM_2G_BAND];
    /* 5g */
    mac_cfg_gain_db_per_band ac_gain_db_5g[MAC_NUM_5G_BAND];
} mac_cfg_custom_gain_db_stru;

typedef struct {
    int8_t c_cfg_delta_pwr_ref_rssi_2g[CUS_NUM_2G_DELTA_RSSI_NUM];
    int8_t c_cfg_delta_pwr_ref_rssi_5g[CUS_NUM_5G_DELTA_RSSI_NUM];
} mac_cfg_custom_delta_pwr_ref_stru;

typedef struct {
    int8_t ac_cfg_delta_amend_rssi_2g[CUS_NUM_2G_AMEND_RSSI_NUM];
    int8_t c_rsv;
    int8_t ac_cfg_delta_amend_rssi_5g[CUS_NUM_5G_AMEND_RSSI_NUM];
} mac_cfg_custom_amend_rssi_stru;

typedef struct {
    int8_t filter_narrowing_amend_rssi_2g[CUS_2G_FILTER_NARROW_AMEND_RSSI_NUM];
    int8_t filter_narrowing_amend_rssi_5g[CUS_5G_FILTER_NARROW_AMEND_RSSI_NUM];
    int8_t ac_rsv[NUM_3_BYTES];
} mac_cfg_custom_filter_narrow_amend_rssi_stru;

typedef struct {
    uint16_t us_lna_on2off_time_ns; /* LNA����LNA�ص�ʱ��(ns) */
    uint16_t us_lna_off2on_time_ns; /* LNA�ص�LNA����ʱ��(ns) */

    int8_t c_lna_bypass_gain_db;   /* �ⲿLNA bypassʱ������(dB) */
    int8_t c_lna_gain_db;          /* �ⲿLNA����(dB) */
    int8_t c_pa_gain_b0_db;        /* �ⲿPA b0 ����(dB) */
    uint8_t uc_ext_switch_isexist; /* �Ƿ�ʹ���ⲿswitch */

    uint8_t uc_ext_pa_isexist;                  /* �Ƿ�ʹ���ⲿpa */
    oal_fem_lp_state_enum_uint8 en_fem_lp_enable; /* �Ƿ�֧��fem�͹��ı�־ */
    int8_t c_fem_spec_value;                    /* fem spec���ʵ� */
    uint8_t uc_ext_lna_isexist;                 /* �Ƿ�ʹ���ⲿlna */

    int8_t c_pa_gain_b1_db;     /* �ⲿPA b1����(dB) */
    uint8_t uc_pa_gain_lvl_num; /* �ⲿPA ���浵λ�� */
    uint8_t auc_resv[NUM_2_BYTES];
} mac_cfg_custom_ext_rf_stru;

typedef struct {
    mac_cfg_custom_gain_db_stru ast_rf_gain_db_rf[WLAN_RF_CHANNEL_NUMS];           /* 2.4g 5g ���� */
    mac_cfg_custom_ext_rf_stru ast_ext_rf[WLAN_BAND_BUTT][WLAN_RF_CHANNEL_NUMS];   /* 2.4g 5g fem */
    mac_cfg_custom_delta_pwr_ref_stru ast_delta_pwr_ref_cfg[WLAN_RF_CHANNEL_NUMS]; /* delta_rssi */
    mac_cfg_custom_amend_rssi_stru ast_rssi_amend_cfg[WLAN_RF_CHANNEL_NUMS];       /* rssi_amend */

    uint8_t uc_far_dist_pow_gain_switch;           /* ��Զ���빦�����濪�� */
    uint8_t uc_far_dist_dsss_scale_promote_switch; /* ��Զ��11b 1m 2m dbb scale����ʹ�ܿ��� */
    uint8_t uc_chn_radio_cap;
    int8_t c_delta_cca_ed_high_80th_5g;

    /* ע�⣬����޸��˶�Ӧ��λ�ã���Ҫͬ���޸ĺ���: hal_config_custom_rf  */
    int8_t c_delta_cca_ed_high_20th_2g;
    int8_t c_delta_cca_ed_high_40th_2g;
    int8_t c_delta_cca_ed_high_20th_5g;
    int8_t c_delta_cca_ed_high_40th_5g;
    uint32_t aul_5g_upc_mix_gain_rf_reg[MAC_NUM_5G_BAND];
    uint32_t aul_2g_pa_bias_rf_reg[CUS_RF_PA_BIAS_REG_NUM]; /* ���ƻ�RF����PAƫ�üĴ���reg236~244, reg253~284 */
    mac_cfg_custom_filter_narrow_amend_rssi_stru filter_narrow_rssi_amend[WLAN_RF_CHANNEL_NUMS];
} mac_cfg_customize_rf; // only host

/* RF�Ĵ������ƻ��ṹ�� */
typedef struct {
    uint16_t us_rf_reg117;
    uint16_t us_rf_reg123;
    uint16_t us_rf_reg124;
    uint16_t us_rf_reg125;
    uint16_t us_rf_reg126;
    uint8_t auc_resv[NUM_2_BYTES];
} mac_cus_dts_rf_reg;

/* FCC��֤ �����ṹ�� */
typedef struct {
    uint8_t uc_index;       /* �±��ʾƫ�� */
    uint8_t uc_max_txpower; /* ����͹��� */
    uint16_t us_dbb_scale;  /* dbb scale */
} mac_cus_band_edge_limit_stru;

/* ���ƻ� У׼���ò��� */
typedef struct {
    /* dts */
    int16_t aus_cali_txpwr_pa_dc_ref_2g_val_chan[MAC_2G_CHANNEL_NUM]; /* txpwr���ŵ�refֵ */
    int16_t aus_cali_txpwr_pa_dc_ref_5g_val_band[MAC_NUM_5G_BAND];
} mac_cus_cali_stru;

/* ���ƻ� DPDУ׼���ò��� */
typedef struct {
    uint32_t aul_dpd_cali_cus_dts[MAC_DPD_CALI_CUS_PARAMS_NUM];
} mac_dpd_cus_cali_stru;

typedef struct {
    int32_t al_dy_cali_base_ratio_params[CUS_DY_CALI_PARAMS_NUM][CUS_DY_CALI_PARAMS_TIMES]; /* ���ⶨ�ƻ��������� */
    int32_t al_dy_cali_base_ratio_ppa_params[CUS_DY_CALI_PARAMS_TIMES]; /* ppa-pow���ƻ��������� */
    int8_t ac_dy_cali_2g_dpn_params[MAC_2G_CHANNEL_NUM][CUS_DY_CALI_DPN_PARAMS_NUM];
    int8_t ac_dy_cali_5g_dpn_params[MAC_NUM_5G_BAND][WLAN_BW_CAP_80PLUS80];
    int16_t as_extre_point_val[CUS_DY_CALI_NUM_5G_BAND];
    uint8_t uc_rf_id;
} mac_cus_dy_cali_param_stru;

typedef struct {
    mac_cus_cali_stru ast_cali[WLAN_RF_CHANNEL_NUMS];
    mac_dpd_cus_cali_stru ast_dpd_cali_para[WLAN_RF_CHANNEL_NUMS];
    uint16_t aus_dyn_cali_dscr_interval[WLAN_BAND_BUTT]; /* ��̬У׼����2.4g 5g */
    int16_t gm0_db10_amend[WLAN_RF_CHANNEL_NUMS];

    oal_bool_enum_uint8 en_dyn_cali_opt_switch;
    int8_t uc_band_5g_enable;
    uint8_t uc_tone_amp_grade;
    uint8_t auc_resv[1];
} mac_cus_dts_cali_stru;

typedef struct {
    uint16_t us_tx_ratio;        /* txռ�ձ� */
    uint16_t us_tx_pwr_comp_val; /* ���书�ʲ���ֵ */
} mac_tx_ratio_vs_pwr_stru;

/* ���ƻ�TXռ�ձ�&�¶Ȳ������书�ʵĲ��� */
typedef struct {
    mac_tx_ratio_vs_pwr_stru ast_txratio2pwr[3]; /* 3��ռ�ձȷֱ��Ӧ���ʲ���ֵ */
    uint32_t more_pwr;                      /* �����¶ȶ��ⲹ���ķ��书�� */
} mac_cfg_customize_tx_pwr_comp_stru;

/* ����Ϊ�����ں����ò���ת��Ϊ�����ڲ������·��Ľṹ�� */
/* �����ں����õ�ɨ��������·���������ɨ����� */
typedef struct {
    oal_ssids_stru st_ssids[WLAN_SCAN_REQ_MAX_SSID];
    int32_t l_ssid_num;

    uint8_t *puc_ie;
    uint32_t ie_len;

    oal_scan_enum_uint8 en_scan_type;
    uint8_t num_channels_2g;
    uint8_t num_channels_5g;
    uint8_t num_channels_6g;

    uint32_t *channels_2g;
    uint32_t *channels_5g;
    uint32_t *channels_6g;
    uint32_t scan_flag;

    /* WLAN/P2P ��������£�p2p0 ��p2p-p2p0 cl ɨ��ʱ����Ҫʹ�ò�ͬ�豸������bit_is_p2p0_scan������ */
    uint8_t bit_is_p2p0_scan : 1; /* �Ƿ�Ϊp2p0 ����ɨ�� */
    uint8_t bit_rsv : 7;          /* ����λ */
    uint8_t auc_rsv[NUM_3_BYTES];           /* ����λ */
} mac_cfg80211_scan_param_stru;

#endif /* end of mac_cfg.h */
