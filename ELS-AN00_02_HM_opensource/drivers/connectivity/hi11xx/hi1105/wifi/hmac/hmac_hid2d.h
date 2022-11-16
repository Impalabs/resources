

#ifndef __HMAC_HID2D_H__
#define __HMAC_HID2D_H__
/* 1 ����ͷ�ļ����� */
#include "hmac_vap.h"
#include "hmac_device.h"
#include "wal_linux_ioctl.h"
#include "wal_cfg_ioctl.h"
#include "wal_linux_vendor.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_HID2D_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_HID2D

/* 2 �궨�� */
#define HMAC_HID2D_CHANNEL_NUM_2G                  32     /* 2G��ȫ�����ŵ� */
#define HMAC_HID2D_CHANNEL_NUM_5G                  73     /* 5G��ȫ�����ŵ� */
#define HMAC_HID2D_MAX_CHANNELS                    4      /* ���֧��80M����4���ŵ���� */
#define HMAC_HID2D_MAX_SCAN_CHAN_NUM               25     /* ����ɨ���ŵ���Ŀ */
#define HMAC_HID2D_SCAN_CHAN_NUM_FOR_APK           6      /* ������ɨ���ŵ��� */
#define HMAC_HID2D_SCAN_TIMES_PER_CHAN_FOR_APK     10     /* ������ÿ���ŵ�ɨ����� */
#define HMAC_HID2D_SCAN_TIME_PER_CHAN_APK          150    /* ������ɨ��ÿ���ŵ��Ĳ���ʱ��Ϊ150ms */
#define HMAC_HID2D_SCAN_TIME_PER_CHAN_ACS          15     /* ACSģʽ��ÿ���ŵ��Ĳ���ʱ��Ϊ15ms */
#define HMAC_HID2D_SCAN_TIMER_CYCLE_MS             1000   /* ɨ�����Ļ�����λ */
#define HMAC_HID2D_REPORT_TIMEOUT_MS               120 * 1000 /* dmac 120s�ڲ��ϱ���Ϊ�����쳣���ر�acs���� */
#define HMAC_HID2D_DECAY                           90     /* ����ƽ���е��ϻ�ϵ�� */
#define HMAC_HID2D_CHAN_LOAD_SHIFT                 10     /* �����ŵ���æ�ȵ��������� */
#define HMAC_HID2D_CHAN_LOAD_DIFF                  50
#define HMAC_HID2D_NOISE_DIFF                      5

/* 3 ö�ٶ��� */
/* HiD2D�豸������ģʽ���� */
typedef enum {
    HMAC_HID2D_P2P_ONLY                      = 0,       /* û�й���AP����P2P���� */
    HMAC_HID2D_DBDC                          = 1,       /* AP��P2P DBDCģʽ */
    HMAC_HID2D_SAME_BAND                     = 2,       /* AP��P2PͬƵģʽ */

    HMAC_HID2D_NET_MODE_BUTT,
} hmac_hid2d_net_mode_enum;
/* �Զ��л��㷨��״̬���� */
typedef enum {
    HMAC_HID2D_ACS_DISABLE                   = 0,       /* ���������� */
    HMAC_HID2D_ACS_WAIT                      = 1,       /* ��״̬�²�����������ɨ�裬��������ŵ��л� */
    HMAC_HID2D_ACS_WORK                      = 2,       /* ��״̬�»���������ɨ�裬������ŵ��л����� */

    HMAC_HID2D_ACS_STATE_BUTT,
} hmac_hid2d_acs_state_enum;
/* HiD2D ACS�쳣�˳�ԭ�� */
typedef enum {
    HMAC_HID2D_NON_GO                        = 1,       /* ��GOģʽ */
    HMAC_HID2D_DMAC_REPORT_TIMEOUT           = 2,       /* DMAC��ʱ�䲻�ϱ��ŵ���Ϣ */
    HMAC_HID2D_P2P_LOST                      = 3,       /* P2P���� */
    HMAC_HID2D_FWK_DISABLE                   = 4,       /* �ϲ���ýӿڹر� */

    HMAC_HID2D_DISABLE_REASON_BUTT,
} hmac_hid2d_acs_disable_reason_enum;
/* HiD2D ACS�ϱ��ں˵���Ϣ���� */
typedef enum {
    HMAC_HID2D_INIT_REPORT                    = 0,       /* ��ʼ����Ϣ�ϱ� */
    HMAC_HID2D_LINK_INFO_REPORT               = 1,       /* ��ǰ��·��Ϣ���� */
    HMAC_HID2D_CHAN_STAT_REPORT               = 2,       /* ̽���ŵ���Ϣ���� */
    HMAC_HID2D_SWITCH_SUCC_REPORT             = 3,       /* �л��ɹ����� */
    HMAC_HID2D_EXTI_REPORT                    = 4,       /* �˳�ԭ���ϱ� */

    HMAC_HID2D_REPORT_TYPE_BUTT,
} hmac_hid2d_acs_report_type_enum;
typedef enum {
    HMAC_HID2D_LINK_MEAS_START_BY_CHAN_LIST   = 0,       /* ����Ĭ�ϵ��ŵ��б�ʼɨ�� */
    HMAC_HID2D_LINK_MEAS_UPDATE_CHAN_LIST     = 1,       /* ����ɨ���б� */
    HMAC_HID2D_LINK_MEAS_UPDATE_SCAN_INTERVAL = 2,       /* ����ɨ���� */
    HMAC_HID2D_LINK_MEAS_UPDATE_SCAN_TIME     = 3,       /* ����ɨ��ʱ�� */
    HMAC_HID2D_LINK_MEAS_START_BY_CHAN        = 4,       /* �·������ŵ�ɨ������� */
} hmac_hid2d_link_meas_type_enum;

/* 4 ȫ�ֱ������� */
/* 5 ��Ϣͷ���� */
/* 6 ��Ϣ���� */
/* 7 STRUCT���� */
/* ȫ�ִ�ѡ�ŵ���ÿ���ŵ��ı�ʾ */
typedef struct {
    uint8_t idx;                            /* ��� */
    uint8_t uc_chan_idx;                    /* ��20MHz�ŵ������� */
    uint8_t uc_chan_number;                 /* ��20MHz�ŵ��� */
    wlan_channel_bandwidth_enum_uint8 en_bandwidth; /* ����ģʽ */
}hmac_hid2d_chan_stru;

/* ��ǰ�ŵ�����Ϣ�ṹ�� */
typedef struct {
    uint8_t is_chan_load_valid;             /* ���alg_intf_det�㷨ûʹ�ܣ���ֵΪOAL_FALSE */
    int8_t data_rssi;                       /* ��ǰ���ź�ǿ�� */
    int16_t chan_load;                      /* ���ŵ���ռ�ձ�: (total_time - rx_nondir_time) / total_time */
    uint16_t link_speed;                    /* ÿ1s�ڵ�ƽ������ */
    uint32_t rx_succ_pkts;               /* ÿ1s�ڽ��ճɹ���֡�� */
    uint32_t rx_dropped_pkts;            /* ÿ1s�ڽ���ʧ�ܵ�֡�� */
    uint32_t tx_succ_pkts;               /* ÿ1s�ڷ��͵�֡�� */
    uint32_t tx_fail_pkts;               /* ÿ1s���ش��Ĵ��� */
    uint32_t tx_dropped_pkts;            /* ÿ1s�ڶ��������ݰ� */
} hmac_hid2d_chan_stat_stru;

/* ��ѡ�ŵ�����Ϣ�ṹ�� */
typedef struct {
    uint8_t good_cnt;                       /* ���ŵ��ŵ�������õ������� */
    int16_t update_num;                     /* ���ŵ��ŵ���Ϣ���´��������ڻ���ƽ�� */
    int16_t chan_noise_rssi;                /* ���ŵ��ĵ��� */
    int16_t chan_load;                      /* ���ŵ���ռ�ձ�: free_time / total_time */
    int16_t estimated_snr;                  /* ���ŵ�Ԥ����SNR */
    int32_t estimated_throughput;           /* ���ŵ�Ԥ���������� */
}hmac_hid2d_chan_info_stru;

	/* ��ʼ�����ϱ��ں˵���Ϣ */
typedef struct {
    hmac_hid2d_acs_report_type_enum acs_info_type;     /* �ں�����������Ϣ���� */
    hmac_hid2d_acs_disable_reason_enum acs_exit_type;  /* ���������жϳ���ǰ����֧��acs�������֧����������֧�ֵ�ԭ�� */
    uint8_t cur_band;                                  /* HiD2D������P2P����Ƶ�� */
    uint8_t chan_number;                               /* ��ǰ���ŵ��� */
    uint8_t bandwidth;                                 /* ��ǰ�Ĵ��� */
    hmac_hid2d_net_mode_enum go_mode;                  /* GO������ģʽ */
    uint8_t supp_chan_list[HMAC_HID2D_CHANNEL_NUM_5G];
    uint8_t supp_chan_num_max;
    uint8_t scan_chan_list[HMAC_HID2D_CHANNEL_NUM_5G];
    uint8_t scan_chan_num_max;
} hmac_hid2d_acs_init_report_stru;

/* �˳�ʱ�ϱ���Ϣ */
typedef struct {
    hmac_hid2d_acs_report_type_enum acs_info_type;     /* �ں�����������Ϣ���� */
    hmac_hid2d_acs_disable_reason_enum acs_exit_type;    /* �˳���ԭ�� */
} hmac_hid2d_acs_exit_report_stru;

/* ��ǰ��·��Ϣ�ϱ��ں� */
typedef struct {
    hmac_hid2d_acs_report_type_enum acs_info_type;    /* �ں�����������Ϣ���� */
    uint8_t chan_number;                              /* ��ǰ���ŵ��� */
    uint8_t bandwidth;                                /* ��ǰ�Ĵ��� */
    hmac_hid2d_net_mode_enum go_mode;                 /* GO������ģʽ */
    hmac_hid2d_chan_stat_stru link_stat;              /* �ϱ��ĵ�ǰ��·��Ϣ */
} hmac_hid2d_link_stat_report_stru;

/* ɨ����Ϣ�ϱ��ں� */
typedef struct {
    hmac_hid2d_acs_report_type_enum acs_info_type;    /* �ں�����������Ϣ���� */
    uint8_t scan_chan_sbs;                            /* ��ǰɨ���ŵ���ɨ��list�е��±� */
    uint8_t scan_chan_idx;                            /* ��ǰɨ���ŵ����ŵ��� */
    uint32_t total_stats_time_us;
    uint32_t total_free_time_20m_us;
    uint32_t total_free_time_40m_us;
    uint32_t total_free_time_80m_us;
    uint32_t total_send_time_us;
    uint32_t total_recv_time_us;
    uint8_t free_power_cnt;                           /* �ŵ����й��ʲ������� */
    int16_t free_power_stats_20m;
    int16_t free_power_stats_40m;
    int16_t free_power_stats_80m;
    int16_t free_power_stats_160m;
} hmac_hid2d_chan_stat_report_stru;

/* �л��ɹ�����Ϣ�ϱ��ں� */
typedef struct {
    hmac_hid2d_acs_report_type_enum acs_info_type;    /* �ں�����������Ϣ���� */
    uint8_t chan_number;                              /* ��ǰ���ŵ��� */
    uint8_t bandwidth;                                /* ��ǰ�Ĵ��� */
} hmac_hid2d_switch_report_stru;

typedef struct {
    uint8_t uc_vap_id;                          /* P2P��Ӧ��mac vap id */
    uint8_t uc_apk_scan;                        /* ������apk������5Gȫ�ŵ�ɨ�裬����һ��ҵ��ģʽ�µ�ɨ���ϱ� */
    uint8_t uc_acs_mode;                        /* �Զ��ŵ��л��㷨ʹ�� */
    hmac_hid2d_acs_state_enum acs_state;       /* �Զ��ŵ��л��㷨��״̬ */

    wlan_channel_band_enum_uint8 uc_cur_band;     /* HiD2D������P2P����Ƶ�� */
    uint8_t uc_cur_chan_idx;                    /* ��ǰ�ŵ���ȫ�ִ�ѡ�ŵ��б�������Ӧ���±� */
    uint8_t target_chan_idx;                      /* ��¼�л���Ŀ���ŵ��� */
    uint8_t uc_cur_scan_chan_idx;               /* ��ǰɨ���ŵ���ɨ��list�е��±� */
    uint8_t scan_chan_idx;                        /* ��ǰɨ���ŵ���ȫ�ִ�ѡ�ŵ��ж�Ӧ���±� */
    uint16_t scan_interval;                       /* ɨ�������ͣ����ݵ�ǰ�ŵ������仯 */
    int16_t update_num;
    int16_t max_update_num;                     /* ���ڻ���ƽ�� */
    uint16_t us_scan_times_for_apk_scan;        /* APKɨ��ģʽ��ɨ����� */
    uint32_t scan_time;                         /* ��ÿ���ŵ���ͣ����ʱ�� (ms) */

    frw_timeout_stru st_scan_chan_timer;          /* �ŵ�ɨ�����ڶ�ʱ�� */
    frw_timeout_stru st_chan_stat_report_timer;   /* dmac���ϱ���Ϣ��ʱ��ʱ�� */
    hmac_hid2d_chan_stru *candidate_list;     /* ָ��ǰband��Ӧ��ȫ�ִ�ѡ�ŵ����� */
    hmac_hid2d_chan_info_stru chan_info_5g[HMAC_HID2D_CHANNEL_NUM_5G]; /* 5G��ѡ�ŵ���Ϣ */
} hmac_hid2d_auto_channel_switch_stru;

/* HiD2D APK ALG�������ýṹ�� */
typedef struct {
    uint8_t alg_cfg_type;  /* ������������ hmac_hid2d_cmd_enum */
    int8_t *name;                           /* ���������Ӧ���ַ��� */
    uint8_t resv[3];                        /* 3ʵ���ֽڶ��� */
} hmac_ioctl_hid2d_apk_cfg_stru;
/* HiD2D �������ýṹ�� */
typedef struct {
    uint8_t hid2d_cfg_type;  /* ������������ hmac_hid2d_cmd_enum */
    wal_hid2d_vendor_cmd_func func;           /* �����Ӧ������ */
} hmac_hid2d_cmd_entry_stru;

extern hmac_hid2d_chan_stru g_aus_channel_candidate_list_5g[];
extern uint8_t g_scan_chan_list[];
extern uint8_t g_scan_chan_num_max;
extern hmac_hid2d_auto_channel_switch_stru g_st_hmac_hid2d_acs_info;
/* 8 UNION���� */
/* 9 OTHERS���� */
/* 10 �������� */
uint32_t hmac_hid2d_set_acs_mode(mac_vap_stru *mac_vap, uint8_t acs_mode);
uint32_t hmac_hid2d_update_acs_state(mac_vap_stru *mac_vap, uint16_t len, uint8_t *param);
uint32_t hmac_hid2d_link_meas(mac_vap_stru *mac_vap, uint16_t len, uint8_t *param);
uint32_t hmac_hid2d_scan_chan_start(mac_vap_stru *pst_mac_vap, mac_device_stru *pst_mac_device);
uint32_t hmac_hid2d_scan_complete_handler(hmac_scan_record_stru *pst_scan_record,
    uint8_t uc_scan_idx);
uint32_t hmac_hid2d_cur_chan_stat_handler(mac_vap_stru *mac_vap, uint8_t len, uint8_t *param);
void hmac_hid2d_acs_switch_completed(mac_vap_stru *mac_vap);
uint32_t wal_hipriv_hid2d_scan_channel(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_hid2d_switch_channel(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
int32_t wal_hid2d_sleep_mode(oal_net_device_stru *net_dev, uint8_t ctrl,
    wal_wifi_priv_cmd_stru *priv_cmd);
int32_t wal_hid2d_napi_mode(oal_net_device_stru *pst_net_dev, uint8_t uc_ctrl);
int32_t wal_hid2d_freq_boost(oal_net_device_stru *pst_net_dev, uint8_t uc_ctrl);
#ifdef _PRE_WLAN_FEATURE_HID2D_PRESENTATION
uint32_t wal_hipriv_hid2d_get_best_chan(oal_net_device_stru *net_dev, oal_ifreq_stru *ifr);
uint32_t wal_hipriv_hid2d_presentation_init(oal_net_device_stru *net_dev, int8_t *cmd);
uint32_t wal_hipriv_hid2d_opt_cfg(oal_net_device_stru *net_dev, int8_t *cmd, uint8_t cmd_type);
uint32_t wal_hipriv_hid2d_swchan(oal_net_device_stru *net_dev, int8_t *cmd);
uint32_t wal_hipriv_hid2d_edca_cfg(oal_net_device_stru *net_dev, int8_t *cmd);
uint32_t wal_vendor_priv_cmd_hid2d_apk(oal_net_device_stru *net_dev, oal_ifreq_stru *ifr,
    uint8_t *cmd);
#endif
#ifdef _PRE_WLAN_FEATURE_HID2D_TX_DROP
int32_t wal_ioctl_set_hid2d_state(oal_net_device_stru *pst_net_dev, uint8_t uc_param,
    wal_wifi_priv_cmd_stru *priv_cmd);
#endif
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif /* end of hmac_hid2d.h */
