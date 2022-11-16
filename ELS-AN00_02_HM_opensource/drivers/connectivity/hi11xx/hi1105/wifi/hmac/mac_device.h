

#ifndef __MAC_DEVICE_H__
#define __MAC_DEVICE_H__

/* 1 ����ͷ�ļ����� */
#include "frw_ext_if.h"
#include "wlan_mib.h"
#include "mac_regdomain.h"
#include "mac_frame.h"
#include "mac_device_common.h"
#include "mac_dfs.h"
#include "hd_event.h"
#include "hd_command.h"
#ifdef _PRE_WLAN_FEATURE_HIEX
#include "hiex_msg.h"
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_DEVICE_H

#define MAC_NET_DEVICE_NAME_LENGTH 16
#define WLAN_USER_MAX_SUPP_RATES 16 /* ���ڼ�¼�Զ��豸֧�ֵ����������� */

#define MAC_SCAN_CHANNEL_INTERVAL_PERFORMANCE     2   /* ���2���ŵ����лع����ŵ�����һ��ʱ�� */
#define MAC_WORK_TIME_ON_HOME_CHANNEL_PERFORMANCE 60  /* WLANδ������P2P���������ع����ŵ�������ʱ�� */
#define MAC_SCAN_CHANNEL_INTERVAL_HIDDEN_SSID     3   /* Я������SSID�ı���ɨ�裬���3���ŵ��ع����ŵ�����һ��ʱ�� */

#define MAC_DEVICE_GET_CAP_BW(_pst_device)     ((_pst_device)->st_device_cap.en_channel_width)
#define MAC_DEVICE_GET_NSS_NUM(_pst_device)    ((_pst_device)->st_device_cap.en_nss_num)
#define MAC_DEVICE_GET_CAP_LDPC(_pst_device)   ((_pst_device)->st_device_cap.en_ldpc_is_supp)
#define MAC_DEVICE_GET_CAP_TXSTBC(_pst_device) ((_pst_device)->st_device_cap.en_tx_stbc_is_supp)
#define MAC_DEVICE_GET_CAP_RXSTBC(_pst_device) ((_pst_device)->st_device_cap.en_rx_stbc_is_supp)
#define MAC_DEVICE_GET_CAP_SUBFER(_pst_device) ((_pst_device)->st_device_cap.en_su_bfmer_is_supp)
#define MAC_DEVICE_GET_CAP_SUBFEE(_pst_device) ((_pst_device)->st_device_cap.en_su_bfmee_is_supp)
#define MAC_DEVICE_GET_CAP_MUBFER(_pst_device) ((_pst_device)->st_device_cap.en_mu_bfmer_is_supp)
#define MAC_DEVICE_GET_CAP_MUBFEE(_pst_device) ((_pst_device)->st_device_cap.en_mu_bfmee_is_supp)
#ifdef _PRE_WLAN_FEATURE_SMPS
#define MAC_DEVICE_GET_MODE_SMPS(_pst_device) ((_pst_device)->en_mac_smps_mode)
#endif

#ifdef _PRE_WLAN_FEATURE_M2S
#define MAC_M2S_CALI_NSS_FROM_SMPS_MODE(en_smps_mode) \
    (((en_smps_mode) == WLAN_MIB_MIMO_POWER_SAVE_STATIC) ? WLAN_SINGLE_NSS : g_pst_mac_device_capability[0].en_nss_num)
#endif
typedef enum {
    MAC_DEVICE_CAP_DISABLE = 0,
    MAC_DEVICE_CAP_2G = BIT0, /* ��ǰ�Ѿ���֧��2G�������� */
    MAC_DEVICE_CAP_5G = BIT1,
    MAC_DEVICE_CAP_6G_LOW_BAND = BIT2, /* ŷ��6GƵ�� */
    MAC_DEVICE_CAP_6G_ALL_BAND = BIT3, /* ����6GƵ�� */
    MAC_DEVICE_CAP_BUTT,
} mac_device_radio_cap_enum;
typedef uint8_t mac_device_radio_cap_enum_uint8;
/* btcoex���������� */
typedef enum {
    MAC_BTCOEX_BLACKLIST_LEV0 = BIT0, /* 0������������Ҫ�����жϣ����翴mac��ַ */
    MAC_BTCOEX_BLACKLIST_LEV1 = BIT1, /* һ������������mac��ַ */
    MAC_BTCOEX_BLACKLIST_BUTT,
} mac_btcoex_blacklist_enum;
typedef uint8_t mac_btcoex_blacklist_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_APF
typedef struct {
    mac_apf_cmd_type_uint8 en_cmd_type;
    uint16_t us_program_len;
    uint8_t *puc_program;
} mac_apf_filter_cmd_stru;
#endif


typedef enum {
    MAC_P2P_SCENES_LOW_LATENCY   = 0, /* ��ʱ�ӳ��� */
    MAC_P2P_SCENES_HI_THROUGHPUT = 1, /* �����³���,�Ƚ׶�ֻ����hishare */

    AC_P2P_SCENES_BUTT
} mac_p2p_scenes_enum;
typedef uint8_t mac_p2p_scenes_enum_uint8;

typedef struct {
    uint8_t uc_p2p_device_num;                /* ��ǰdevice�µ�P2P_DEVICE���� */
    uint8_t uc_p2p_goclient_num;              /* ��ǰdevice�µ�P2P_CL/P2P_GO���� */
    uint8_t uc_p2p0_vap_idx;                  /* P2P ���泡���£�P2P_DEV(p2p0) ָ�� */
    mac_vap_state_enum_uint8 en_last_vap_state; /* P2P0/P2P_CL ����VAP �ṹ�����������±���VAP �������ǰ��״̬ */
    mac_p2p_scenes_enum_uint8 p2p_scenes;       /* p2p ҵ�񳡾�:��ʱ�ӡ������� */
    uint8_t uc_resv[1];                       /* ���� */
    uint8_t en_roc_need_switch;               /* remain on channel����Ҫ�л�ԭ�ŵ� */
    uint8_t en_p2p_ps_pause;                  /* P2P �����Ƿ���pause״̬ */
    oal_net_device_stru *pst_p2p_net_device;    /* P2P ���泡������net_device(p2p0) ָ�� */
    uint64_t ull_send_action_id;              /* P2P action id/cookie */
    uint64_t ull_last_roc_id;
    oal_ieee80211_channel_stru st_listen_channel;
    oal_nl80211_channel_type en_listen_channel_type;
    oal_net_device_stru *pst_primary_net_device; /* P2P ���泡������net_device(wlan0) ָ�� */
    oal_net_device_stru *pst_second_net_device;  /* �ŵ���������,������ʹ�ÿ���ɾ�� */
} mac_p2p_info_stru;

typedef struct {
    uint16_t us_num_networks; /* ��¼��ǰ�ŵ���ɨ�赽��BSS���� */
    uint8_t auc_resv[NUM_2_BYTES];
    uint8_t auc_bssid_array[WLAN_MAX_SCAN_BSS_PER_CH][WLAN_MAC_ADDR_LEN]; /* ��¼��ǰ�ŵ���ɨ�赽������BSSID */
} mac_bss_id_list_stru;


#define MAX_PNO_REPEAT_TIMES    4
#define PNO_SCHED_SCAN_INTERVAL (60 * 1000)

/* ɨ�赽��BSS�����ṹ�� */
typedef struct {
    /* ������Ϣ */
    uint8_t en_bss_type; /* wlan_mib_desired_bsstype_enum bss�������� */
    uint8_t uc_dtim_period;                        /* dtime���� */
    uint8_t uc_dtim_cnt;                           /* dtime cnt */
    oal_bool_enum_uint8 en_11ntxbf;                  /* 11n txbf */
    oal_bool_enum_uint8 en_new_scan_bss;             /* �Ƿ�����ɨ�赽��BSS */
    wlan_ap_chip_oui_enum_uint8 en_is_tplink_oui;
    int8_t c_rssi;                                    /* bss���ź�ǿ�� */
    int8_t ac_ssid[WLAN_SSID_MAX_LEN];                /* ����ssid */
    uint16_t us_beacon_period;                        /* beacon���� */
    uint16_t us_cap_info;                             /* ����������Ϣ */
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN];          /* ���������� mac��ַ��bssid��ͬ */
    uint8_t auc_bssid[WLAN_MAC_ADDR_LEN];             /* ����bssid */
    mac_channel_stru st_channel;                        /* bss���ڵ��ŵ� */
    uint8_t uc_wmm_cap;                               /* �Ƿ�֧��wmm */
    uint8_t uc_uapsd_cap;                             /* �Ƿ�֧��uapsd */
    oal_bool_enum_uint8 en_desired;                     /* ��־λ����bss�Ƿ��������� */
    uint8_t uc_num_supp_rates;                        /* ֧�ֵ����ʼ����� */
    uint8_t auc_supp_rates[WLAN_USER_MAX_SUPP_RATES]; /* ֧�ֵ����ʼ� */

#ifdef _PRE_WLAN_FEATURE_11D
    int8_t ac_country[WLAN_COUNTRY_STR_LEN]; /* �����ַ��� */
    uint8_t auc_resv2[1];
    uint8_t *puc_country_ie; /* ���ڴ��country ie */
#endif

    /* ��ȫ��ص���Ϣ */
    uint8_t *puc_rsn_ie; /* ���ڴ��beacon��probe rsp��rsn ie */
    uint8_t *puc_wpa_ie; /* ���ڴ��beacon��probe rsp��wpa ie */

    /* 11n 11ac��Ϣ */
    oal_bool_enum_uint8 en_ht_capable;                      /* �Ƿ�֧��ht */
    oal_bool_enum_uint8 en_vht_capable;                     /* �Ƿ�֧��vht */
    oal_bool_enum_uint8 en_vendor_vht_capable;              /* �Ƿ�֧��hidden vendor vht */
    wlan_bw_cap_enum_uint8 en_bw_cap;                       /* ֧�ֵĴ��� 0-20M 1-40M */
    wlan_channel_bandwidth_enum_uint8 en_channel_bandwidth; /* �ŵ����� */
    uint8_t uc_coex_mgmt_supp;                            /* �Ƿ�֧�ֹ������ */
    oal_bool_enum_uint8 en_ht_ldpc;                         /* �Ƿ�֧��ldpc */
    oal_bool_enum_uint8 en_ht_stbc;                         /* �Ƿ�֧��stbc */
    uint8_t uc_wapi;
    uint8_t en_vendor_novht_capable; /* ˽��vendor�в�����Я�� */
    oal_bool_enum_uint8 en_atheros_chip_oui;
    oal_bool_enum_uint8 en_vendor_1024qam_capable;          /* ˽��vendor���Ƿ�Я��1024QAMʹ�� */

    mac_btcoex_blacklist_enum_uint8 en_btcoex_blacklist_chip_oui; /* ps����one pkt֡������Ҫ�޶�Ϊself-cts�� */
    oal_bool_enum_uint8 en_punctured_preamble; /* �Ƿ�֧��punctured preamble���� */
    uint32_t timestamp; /* ���´�bss��ʱ��� */

#ifdef _PRE_WLAN_FEATURE_M2S
    wlan_nss_enum_uint8 en_support_max_nss; /* ��AP֧�ֵ����ռ����� */
    oal_bool_enum_uint8 en_support_opmode;  /* ��AP�Ƿ�֧��OPMODE */
    uint8_t uc_num_sounding_dim;          /* ��AP����txbf�������� */
#endif

    oal_bool_enum_uint8 en_he_capable; /* �Ƿ�֧��11ax */
    oal_bool_enum_uint8 en_he_uora;
    oal_bool_enum_uint8 en_dcm_support;
    mac_p2p_scenes_enum_uint8 en_p2p_scenes;

    oal_bool_enum_uint8 en_support_rrm; /* �Ƿ�֧��RRM */
    oal_bool_enum_uint8 en_support_neighbor; /* �Ƿ�֧��Neighbor report */

#ifdef _PRE_WLAN_FEATURE_1024QAM
    oal_bool_enum_uint8 en_support_1024qam;
#endif

#ifdef _PRE_WLAN_NARROW_BAND
    oal_bool_enum_uint8 en_nb_capable; /* �Ƿ�֧��nb */
#endif
    oal_bool_enum_uint8 en_roam_blacklist_chip_oui; /* ��֧��roam */
    oal_bool_enum_uint8 en_txbf_blacklist_chip_oui; /* ��֧��txbf */
    mac_sap_mode_enum_uint8 sap_mode;
    uint8_t resv4[NUM_1_BYTES];

    int8_t rssi[HD_EVENT_RF_NUM]; /* 4���ߵ�rssi */
    int8_t snr[HD_EVENT_RF_NUM];  /* 4���ߵ�snr */
    uint8_t  ht_opern_ccsf2; /* ����ht operation��ccsf2,���ڴ������ */
    uint8_t  supported_channel_width : 2, /* vht cap���������ڴ������ */
             extend_nss_bw_supp      : 2, /* vht cap���������ڴ������ */
             resv                    : 4;

#ifdef _PRE_WLAN_FEATURE_MBO
    uint8_t uc_bss_assoc_disallowed; /* MBO IE��ָʾAP�Ƿ�������� */
#endif
    /* ����֡��Ϣ */
    uint32_t mgmt_len; /* ����֡�ĳ��� */

    /* multi_bssid ��� */
#ifdef _PRE_WLAN_FEATURE_11AX
    mac_multi_bssid_info st_mbssid_info;
#endif
    uint64_t wpa_rpt_time;                                 /* ɨ�����ϱ�supplicantʱ�� */
    uint8_t auc_mgmt_buff[MAC_80211_QOS_HTC_4ADDR_FRAME_LEN]; /* ��¼beacon֡��probe rsp֡ */
    /* ��Ҫ�ڹ���֡��������Ԫ�أ�������ڹ���֡����֮ǰ */
    uint8_t auc_mgmt_frame_body[NUM_4_BYTES]; /* *puc_mgmt_buff --- ��¼beacon֡��probe rsp֡ */
} mac_bss_dscr_stru; // only host

typedef struct {
    /* ֧��2*2 */                            /* ֧��MU-MIMO */
    wlan_nss_enum_uint8 en_nss_num;          /* device Nss �ռ��������� */
    wlan_bw_cap_enum_uint8 en_channel_width; /* ֧�ֵĴ��� */
    oal_bool_enum_uint8 en_nb_is_supp;       /* ֧��խ�� */
    oal_bool_enum_uint8 en_1024qam_is_supp;  /* ֧��1024QAM���� */

    oal_bool_enum_uint8 en_80211_mc_is_supp; /* ֧��80211 mc */
    oal_bool_enum_uint8 en_ldpc_is_supp;     /* �Ƿ�֧�ֽ���LDPC����İ� */
    oal_bool_enum_uint8 en_tx_stbc_is_supp;  /* �Ƿ�֧������2x1 STBC���� */
    oal_bool_enum_uint8 en_rx_stbc_is_supp;  /* �Ƿ�֧��stbc����,֧��2���ռ��� */

    oal_bool_enum_uint8 en_su_bfmer_is_supp; /* �Ƿ�֧�ֵ��û�beamformer */
    oal_bool_enum_uint8 en_su_bfmee_is_supp; /* �Ƿ�֧�ֵ��û�beamformee */
    oal_bool_enum_uint8 en_mu_bfmer_is_supp; /* �Ƿ�֧�ֶ��û�beamformer */
    oal_bool_enum_uint8 en_mu_bfmee_is_supp; /* �Ƿ�֧�ֶ��û�beamformee */
    uint8_t en_11ax_switch : 1,            /* 11ax���� */
              bit_multi_bssid_switch : 1,          /* mbssid ���� */
              bit_11ax_rsv : 6;
    uint8_t _rom[NUM_3_BYTES];
    mac_hisi_priv_cap_union hisi_priv_cap;
} mac_device_capability_stru;

typedef struct {
    oal_bool_enum_uint8 en_11k;
    oal_bool_enum_uint8 en_11v;
    oal_bool_enum_uint8 en_11r;
    oal_bool_enum_uint8 en_11r_ds;
    oal_bool_enum_uint8 en_adaptive11r;
    oal_bool_enum_uint8 en_nb_rpt_11k;
    uint8_t auc_rsv[NUM_2_BYTES];
} mac_device_voe_custom_stru;

#ifdef _PRE_WLAN_FEATURE_11AX
typedef struct {
    uint8_t   bit_htc_include : 1, /* ����֡����htc ͷ,Ĭ�ϳ�ʼ��ֵδȫ F */
                bit_om_in_data  : 1, /* ÿ����������֡��Я��om�ֶ� */
                bit_rom_cap_switch : 1,
                bit_11ax_aput_switch  : 1,  /* 11ax aput���� */
                bit_ignore_non_he_cap_from_beacon : 1, /* he sta ������beacon �е�non he cap ie ���� */
                bit_11ax_aput_he_cap_switch : 1, /* 11ax aputЯ��he�������� */
                bit_twt_responder_support : 1, /* ����twt resp�������� */
                bit_twt_requester_support : 1; /* ����twt req�������� */
    uint8_t   bit_btwt_requester_support : 1, /* �㲥twt req�������� */
                bit_flex_twt_support  : 1, /* flex twt�������� */
                bit_resv : 6; /* ����6bit */
} mac_device_11ax_custom_stru;
#endif

typedef struct {
    mac_device_voe_custom_stru st_voe_custom_cfg;
#ifdef _PRE_WLAN_FEATURE_11AX
    mac_device_11ax_custom_stru st_11ax_custom_cfg;
#endif
    uint16_t us_cmd_auth_rsp_timeout;
    uint8_t  bit_forbit_open_auth : 1,
               bit_rsv              : 7;
    uint8_t  auc_rsv[1];
} mac_device_custom_cfg_stru;

#ifdef _PRE_WLAN_FEATURE_MCAST_AMPDU
typedef struct {
    oal_bool_enum_uint8 mcast_ampdu_enable; /* �Ƿ����鲥�ۺ� */
    uint8_t mcast_ampdu_bitmap; /* ֧�ֵ��鲥�ۺ�ģʽ */
    uint8_t mcast_ampdu_retry; /* �鲥�ۺ�֡�ش����� */
    uint8_t resv;
} mac_mcast_ampdu_cfg_stru;

enum mcast_ampdu_feature {
    MCAST_AMPDU_APUT = 0,
    MCAST_AMPDU_STAUT = 1,
    MCAST_AMPDU_BUTT,
};
#endif

/* device�ṹ�� */
typedef struct {
    /* device�µ�ҵ��vap���˴�ֻ��¼VAP ID */
    uint32_t core_id;
    uint8_t auc_vap_id[WLAN_SERVICE_VAP_MAX_NUM_PER_DEVICE];
    uint8_t uc_cfg_vap_id;               /* ����vap ID */
    uint8_t uc_device_id;                /* оƬID */
    uint8_t uc_chip_id;                  /* �豸ID */
    uint8_t uc_device_reset_in_progress; /* ��λ������ */

    oal_bool_enum_uint8 en_device_state; /* ��ʶ�Ƿ��Ѿ������䣬(OAL_TRUE��ʼ����ɣ�OAL_FALSEδ��ʼ�� ) */
    uint8_t uc_vap_num;                /* ��ǰdevice�µ�ҵ��VAP����(AP+STA) */
    uint8_t uc_sta_num;                /* ��ǰdevice�µ�STA���� */
    /* begin: P2P */
    mac_p2p_info_stru st_p2p_info; /* P2P �����Ϣ */
    /* end: P2P */
    uint8_t auc_hw_addr[WLAN_MAC_ADDR_LEN]; /* ��eeprom��flash��õ�mac��ַ��ko����ʱ����hal�ӿڸ�ֵ */
    /* device������� */
    uint8_t uc_max_channel;                 /* ������VAP���ŵ��ţ�����VAP����ֵ�������ֵì�ܣ����ڷ�DBACʱʹ�� */
    wlan_channel_band_enum_uint8 en_max_band; /* ������VAP��Ƶ�Σ�����VAP����ֵ�������ֵì�ܣ����ڷ�DBACʱʹ�� */

    oal_bool_enum_uint8 en_wmm; /* wmmʹ�ܿ��� */
    wlan_tidno_enum_uint8 en_tid;
    uint8_t en_reset_switch; /* �Ƿ�ʹ�ܸ�λ���� */
    uint8_t uc_csa_vap_cnt;  /* ÿ��running AP����һ��CSA֡,�ü�����1,�������,APֹͣ��ǰӲ������,׼����ʼ�л��ŵ� */

    uint32_t beacon_interval; /* device����beacon interval,device������VAPԼ��Ϊͬһֵ */

    uint8_t uc_auth_req_sendst;
    uint8_t uc_asoc_req_sendst;

#ifdef _PRE_WLAN_FEATURE_HIEX
    mac_hiex_cap_stru st_hiex_cap;
    uint32_t hiex_debug_switch;
#else
    uint32_t resv1;
    uint32_t resv2;
#endif
    uint32_t resv3;

    /* device���� */
    wlan_protocol_cap_enum_uint8 en_protocol_cap; /* Э������ */
    wlan_band_cap_enum_uint8 en_band_cap;         /* Ƶ������ */
    /* ������VAP���������ֵ������VAP����ֵ�������ֵì�ܣ����ڷ�DBACʱʹ�� */
    wlan_channel_bandwidth_enum_uint8 en_max_bandwidth;

    int16_t s_upc_amend; /* UPC����ֵ */

    uint16_t us_device_reset_num; /* ��λ�Ĵ���ͳ�� */

    mac_data_rate_stru st_mac_rates_11g[MAC_DATARATES_PHY_80211G_NUM]; /* 11g���� */

    mac_scan_req_stru st_scan_params;                      /* ����һ�ε�ɨ�������Ϣ */
    frw_timeout_stru st_obss_scan_timer;                   /* obssɨ�趨ʱ����ѭ����ʱ�� */
    mac_channel_stru st_p2p_vap_channel;                   /* p2p listenʱ��¼p2p���ŵ�������p2p listen������ָ� */

    oal_bool_enum_uint8 en_2040bss_switch; /* 20/40 bss��⿪�� */
    uint8_t uc_in_suspend;

    /* linux�ں��е�device������Ϣ */
    /* ���ڴ�ź�VAP��ص�wiphy�豸��Ϣ����AP/STAģʽ�¾�Ҫʹ�ã����Զ��VAP��Ӧһ��wiphy */
    oal_wiphy_stru *pst_wiphy;
    mac_bss_id_list_stru st_bss_id_list; /* ��ǰ�ŵ��µ�ɨ���� */

    uint8_t uc_mac_vap_id; /* ��vap����ʱ������˯��ǰ��mac vap id */
    oal_bool_enum_uint8 en_dbac_enabled;
    oal_bool_enum_uint8 en_dbac_running;       /* DBAC�Ƿ������� */
    oal_bool_enum_uint8 en_dbac_has_vip_frame; /* ���DBAC����ʱ�յ��˹ؼ�֡ */

    uint8_t uc_arpoffload_switch;
    uint8_t uc_wapi;
#ifdef _PRE_WLAN_FEATURE_MONITOR
    uint8_t uc_monitor_ota_mode;
#else
    uint8_t uc_reserve;
#endif
    oal_bool_enum_uint8 en_is_random_mac_addr_scan; /* ���macɨ�迪��,��hmac�·� */

    uint8_t auc_mac_oui[WLAN_RANDOM_MAC_OUI_LEN]; /* ���mac��ַOUI,��ϵͳ�·� */
    oal_bool_enum_uint8 en_dbdc_running;            /* DBDC�Ƿ������� */

    mac_device_capability_stru st_device_cap; /* device�Ĳ����������������ƻ� */

#ifdef _PRE_WLAN_FEATURE_SMPS
    /* SMPS��MAC����������Ҫdevice�����е�VAP��֧��SMPS�ŻῪ��MAC��SMPS���� */
    /* ��¼��ǰMAC������SMPS����(����mac�࣬����ΪhmacҲ���������ж�) */
    uint8_t en_mac_smps_mode; /* wlan_mib_mimo_power_save_enum */
#endif

#ifdef _PRE_WLAN_FEATURE_DFS
    mac_dfs_core_stru st_dfs;
#endif
    mac_ap_ch_info_stru st_ap_channel_list[MAC_MAX_SUPP_CHANNEL];

    /* ���Host�ĳ�Ա�����ƶ���hmac_device */
    oal_bool_enum_uint8 en_vap_classify; /* �Ƿ�ʹ�ܻ���vap��ҵ����� */
    uint8_t uc_ap_chan_idx;            /* ��ǰɨ���ŵ����� */

    oal_bool_enum_uint8 en_40MHz_intol_bit_recd;
    uint8_t uc_ftm_vap_id; /* ftm�ж϶�Ӧ vap ID */

    frw_timeout_stru st_send_frame; /* send frame��ʱ�� */

    uint8_t is_ready_to_get_scan_result;
    mac_channel_stru st_best_chan_for_hid2d;
    /* �����᳡����־λ: 0��ʾ�Ƿ�����ģʽ��1��ʾ������ģʽ */
    uint8_t is_presentation_mode;
    /* ˫staģʽ��ǣ�wlan1��Ӧvap����ʱ��true��ɾ��ʱ��false */
    uint8_t is_dual_sta_mode;
    uint8_t dc_status;  /* host����Ƿ�DC״̬ */
} mac_device_stru;

#ifdef _PRE_WLAN_FEATURE_HID2D
typedef struct {
    uint8_t link_meas_cmd_type;
    uint8_t scan_chan;
    uint16_t scan_interval;
    uint16_t scan_time;
} mac_hid2d_link_meas_stru;
#endif

#ifdef _PRE_WLAN_FEATURE_HIEX
#define MAC_BAND_GET_HIEX_CAP(band)     (&(band)->st_hiex_cap)
#define MAC_BAND_HIEX_ENABLED(band)     ((band)->st_hiex_cap.bit_hiex_enable)
#define MAC_BAND_HIMIT_ENABLED(band)    ((band)->st_hiex_cap.bit_himit_enable)
#define MAC_BAND_ERSRU_ENABLED(band)    ((band)->st_hiex_cap.bit_ersru_enable)
#endif

typedef struct {
    uint8_t auc_tx_ba_index_table[MAC_TX_BA_LUT_BMAP_LEN];   /* ���Ͷ�BA LUT��λͼ */
    uint8_t auc_rx_ba_lut_idx_table[MAC_RX_BA_LUT_BMAP_LEN]; /* ���ն�BA LUT��λͼ */
    uint8_t auc_ra_lut_index_table[WLAN_ACTIVE_USER_IDX_BMAP_LEN]; /* �����û� LUT��λͼ */
} mac_lut_table_stru;

/* chip�ṹ�� */
typedef struct {
    uint8_t auc_device_id[WLAN_SERVICE_DEVICE_MAX_NUM_PER_CHIP]; /* CHIP�¹ҵ�DEV������¼��Ӧ��ID����ֵ */
    uint8_t uc_device_nums;                                      /* chip��device����Ŀ */
    uint8_t uc_chip_id;                                          /* оƬID */
    /* ��ʶ�Ƿ��ѳ�ʼ����OAL_TRUE��ʼ����ɣ�OAL_FALSEδ��ʼ�� */
    oal_bool_enum_uint8 en_chip_state;
    uint32_t chip_ver;              /* оƬ�汾 */
    mac_lut_table_stru st_lut_table;     /* ���ά��LUT����Դ�Ľṹ�� */
    void *p_alg_priv;                /* chip�����㷨˽�нṹ�� */

    /* �û���س�Ա���� */
    frw_timeout_stru st_active_user_timer; /* �û���Ծ��ʱ�� */
    uint8_t uc_assoc_user_cnt;  /* �����û��� */
    uint8_t uc_active_user_cnt; /* ��Ծ�û��� */
} mac_chip_stru;

typedef enum {
    MAC_RX_IP_FILTER_STOPED = 0,   // ���ܹرգ�δʹ�ܡ���������״����������˶�����
    MAC_RX_IP_FILTER_WORKING = 1,  // ���ܴ򿪣����չ�����������
    MAC_RX_IP_FILTER_BUTT
} mac_ip_filter_state_enum;
typedef uint8_t mac_ip_filter_state_enum_uint8;

typedef struct {
    mac_ip_filter_state_enum_uint8 en_state;  // ����״̬�����ˡ��ǹ��˵�
    uint8_t uc_btable_items_num;            // ��������Ŀǰ�洢��items����
    uint8_t uc_btable_size;                 // ��������С����ʾ���洢��items����
    uint8_t uc_resv;
    mac_ip_filter_item_stru *pst_filter_btable;  // ������ָ��
} mac_rx_ip_filter_struc;

/* board�ṹ�� */
typedef struct {
    mac_chip_stru ast_chip[WLAN_CHIP_MAX_NUM_PER_BOARD]; /* board�ҽӵ�оƬ */
    uint8_t uc_chip_id_bitmap;                         /* ��ʶchip�Ƿ񱻷����λͼ */
    uint8_t auc_resv[NUM_3_BYTES];                               /* �ֽڶ��� */
    mac_rx_ip_filter_struc st_rx_ip_filter; /* rx ip���˹��ܵĹ���ṹ�� */
    mac_rx_icmp_filter_struc st_rx_icmp_filter;
} mac_board_stru;

typedef struct {
    mac_device_stru *pst_mac_device;
} mac_wiphy_priv_stru;

/* ������ */
typedef struct {
    uint8_t auc_mac_addr[OAL_MAC_ADDR_LEN]; /* mac��ַ          */
    uint8_t auc_reserved[NUM_2_BYTES];                /* �ֽڶ���         */
    uint32_t cfg_time;                   /* �����������ʱ�� */
    uint32_t aging_time;                 /* �ϻ�ʱ��         */
    uint32_t drop_counter;               /* ���Ķ���ͳ��     */
} mac_blacklist_stru;

/* �Զ������� */
typedef struct {
    uint8_t auc_mac_addr[OAL_MAC_ADDR_LEN]; /* mac��ַ  */
    uint8_t auc_reserved[NUM_2_BYTES];                /* �ֽڶ��� */
    uint32_t cfg_time;                   /* ��ʼʱ�� */
    uint32_t asso_counter;               /* �������� */
} mac_autoblacklist_stru;

/* 2.9.12 ˽�а�ȫ��ǿ */
#define WLAN_BLACKLIST_MAX 32

/* �Զ���������Ϣ */
typedef struct {
    uint8_t uc_enabled;                                          /* ʹ�ܱ�־ 0:δʹ��  1:ʹ�� */
    uint8_t list_num;                                            /* �ж��ٸ��Զ�������        */
    uint8_t auc_reserved[NUM_2_BYTES];                                     /* �ֽڶ���                  */
    uint32_t threshold;                                       /* ����                      */
    uint32_t reset_time;                                      /* ����ʱ��                  */
    uint32_t aging_time;                                      /* �ϻ�ʱ��                  */
    mac_autoblacklist_stru ast_autoblack_list[WLAN_BLACKLIST_MAX]; /* �Զ���������              */
} mac_autoblacklist_info_stru;

/* �ڰ�������Ϣ */
typedef struct {
    uint8_t uc_mode;                                     /* �ڰ�����ģʽ   */
    uint8_t uc_list_num;                                 /* ������         */
    uint8_t uc_blacklist_vap_index;                      /* ������vap index */
    uint8_t uc_blacklist_device_index;                   /* ������device index */
    mac_autoblacklist_info_stru st_autoblacklist_info;     /* �Զ���������Ϣ */
    mac_blacklist_stru ast_black_list[WLAN_BLACKLIST_MAX]; /* ��Ч�ڰ������� */
} mac_blacklist_info_stru;

typedef struct {
    uint32_t                          ao_drop_cnt;
    uint32_t                          ao_send_rsp_cnt;
    uint32_t                          apf_flt_drop_cnt;
    uint32_t                          icmp_flt_drop_cnt;
}mac_psm_flt_stat_stru; // only device
typedef struct {
    uint32_t                          ps_short_idle_cnt;
    uint32_t                          ps_long_idle_cnt;
}mac_psm_fastsleep_stat_stru; // only device

#ifdef _PRE_WLAN_FEATURE_PSM_ABN_PKTS_STAT
typedef struct {
    uint32_t small_amsdu_total_cnt; // С��amsdu����
    uint32_t large_amsdu_total_cnt; // ���amsdu����
    uint32_t small_amsdu_mcast_ucast_cnt; // С��amsdu�鲥����������
} mac_psm_abnormal_stat_stru;
#endif

/* 8 UNION���� */
/* 9 OTHERS���� */
/* ���߼��в��뿴���� */
#define MAC_DBAC_ENABLE(_pst_device) ((_pst_device)->en_dbac_enabled == OAL_TRUE)

extern mac_device_capability_stru *g_pst_mac_device_capability;

#ifdef _PRE_WLAN_FEATURE_WMMAC
extern oal_bool_enum_uint8 g_en_wmmac_switch;
#endif

extern mac_board_stru *g_pst_mac_board;


OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_is_dbac_enabled(mac_device_stru *pst_device)
{
    return pst_device->en_dbac_enabled;
}


OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_is_dbac_running(mac_device_stru *pst_device)
{
    if (pst_device->en_dbac_enabled == OAL_FALSE) {
        return OAL_FALSE;
    }

    return pst_device->en_dbac_running;
}

OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_is_dbdc_running(mac_device_stru *pst_mac_device)
{
    return pst_mac_device->en_dbdc_running;
}

OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_get_2040bss_switch(mac_device_stru *pst_mac_device)
{
    return pst_mac_device->en_2040bss_switch;
}
OAL_STATIC OAL_INLINE void mac_set_2040bss_switch(mac_device_stru *pst_mac_device, oal_bool_enum_uint8 en_switch)
{
    pst_mac_device->en_2040bss_switch = en_switch;
}

OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_chip_run_band(uint8_t uc_chip_id, wlan_channel_band_enum_uint8 en_band)
{
    /*
     * �ж�ָ��оƬ�Ƿ����������ָ��BAND��
     *     -˫оƬʱ��оƬֻ������ָ����BAND����������˫оƬ��Ƶ���޸Ĵ˴�
     *     -��оƬ˫Ƶʱ��������������BAND
     *     -��оƬ��Ƶʱֻ���������ں궨��ָ����BAND
     *     -note:Ŀǰ����witp wifiоƬ��֧��˫Ƶ���������е�ƵоƬ����Ҫ��������
     *      plat_chip_supp_band(chip_id, band)�Ľӿڣ����ڴ˴������ж�
     */
    if ((en_band != WLAN_BAND_2G) && (en_band != WLAN_BAND_5G)) {
        return OAL_FALSE;
    }

    return OAL_TRUE;
}

uint32_t mac_device_init(mac_device_stru *pst_mac_device, uint32_t chip_ver,
    uint8_t chip_id, uint8_t uc_device_id);
uint32_t mac_chip_init(mac_chip_stru *pst_chip, uint8_t uc_device_max);
uint32_t mac_board_init(void);

uint32_t mac_device_exit(mac_device_stru *pst_device);
uint32_t mac_chip_exit(mac_board_stru *pst_board, mac_chip_stru *pst_chip);
uint32_t mac_board_exit(mac_board_stru *pst_board);

/* 10.2 ������Ա���ʲ��� */
void mac_chip_inc_assoc_user(mac_chip_stru *pst_mac_chip);
void mac_chip_dec_assoc_user(mac_chip_stru *pst_mac_chip);

void mac_device_set_dfr_reset(mac_device_stru *pst_mac_device, uint8_t uc_device_reset_in_progress);
void mac_device_set_state(mac_device_stru *pst_mac_device, uint8_t en_device_state);

void mac_device_set_beacon_interval(mac_device_stru *pst_mac_device, uint32_t beacon_interval);

void mac_blacklist_get_pointer(wlan_vap_mode_enum_uint8 en_vap_mode, uint8_t uc_dev_id,
    uint8_t uc_chip_id, uint8_t uc_vap_id, mac_blacklist_info_stru **pst_blacklist_info);

void *mac_device_get_all_rates(mac_device_stru *pst_dev);
oal_bool_enum_uint8 mac_device_band_is_support(uint8_t mac_device_id, mac_device_radio_cap_enum_uint8 radio_cap);

extern uint32_t g_ddr_freq;
extern mac_device_custom_cfg_stru g_st_mac_device_custom_cfg;
extern uint8_t g_optimized_feature_switch_bitmap;
extern uint8_t g_rx_filter_frag;
extern uint8_t g_auc_mac_device_radio_cap[];
#ifdef _PRE_WLAN_FEATURE_MCAST_AMPDU
extern mac_mcast_ampdu_cfg_stru g_mcast_ampdu_cfg;
#endif
#ifdef _PRE_WLAN_CHBA_MGMT
extern int32_t g_d2h_rate_complete;
#endif


OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_device_check_5g_enable_per_chip(void)
{
    uint8_t uc_dev_idx = 0;

    while (uc_dev_idx < WLAN_SERVICE_DEVICE_MAX_NUM_PER_CHIP) {
        if (mac_device_band_is_support(uc_dev_idx, MAC_DEVICE_CAP_5G) == OAL_TRUE) {
            return OAL_TRUE;
        }
        uc_dev_idx++;
    }

    return OAL_FALSE;
}

wlan_mib_vht_supp_width_enum mac_device_trans_bandwith_to_vht_capinfo(wlan_bw_cap_enum_uint8 en_max_op_bd);
void mac_set_dual_sta_mode(uint8_t mode);
uint8_t mac_is_dual_sta_mode(void);
#ifdef _PRE_WLAN_FEATURE_MCAST_AMPDU
oal_bool_enum mac_get_mcast_ampdu_switch(void);
#endif
#endif /* end of mac_device.h */

