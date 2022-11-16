

#ifndef __HOST_HAL_OPS_H__
#define __HOST_HAL_OPS_H__

#include "oal_net.h"
#include "host_hal_device.h"
#include "host_dscr_1106.h"
#include "hal_common.h"
#include "mac_common.h"

typedef enum {
    HAL_WLAN_DDR_DRX_EVENT_SUBTYPE,    /* DDR DRX ���� */
    HAL_WLAN_FTM_IRQ_EVENT_SUBTYPE,    /* FTM ���� */
    HAL_WLAN_DDR_DRX_EVENT_SUB_TYPE_BUTT
} hal_wlan_ddr_drx_event_enum;

typedef enum {
    HAL_TPC_INIT_MODE      = 0,    /* tpc��ʼ����ģʽ */
    HAL_TPC_SLEEP_MODE,            /* tpc˯�߹���ģʽ */
    HAL_TPC_THIRD_MODE,            /* tpc��������ģʽ   */

    HAL_TPC_WORK_MODE_BUTT
}hal_tpc_work_mode_enum;
typedef uint8_t hal_tpc_work_mode_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_MONITOR

typedef enum {
    WLAN_MONITOR_OFF, /* monitorץ�����ܹر� */
    WLAN_MONITOR_ON,  /* monitorץ�����ܿ��� */
    WLAN_MONITOR_STATE_BUTT
} wlan_monitor_state_enum;
typedef uint8_t wlan_monitor_state_enum_uint8;

/* ����ץ����غ� */
#define HMAC_PKT_CAP_SIGNAL_OFFSET (-94)
#define HMAC_PKT_CAP_NOISE_MAX     0
#define HMAC_PKT_CAP_NOISE_MIN     (-100)
#define HMAC_PKT_CAP_RATE_UNIT     500

#define HMAC_PKT_CAP_5G_RATE 6000
#define WLAN_2G_CENTER_FREQ_BASE 2407
#define WLAN_5G_CENTER_FREQ_BASE 5000
#define WLAN_CHN_NUM2FREQ_STEP   5    /* 5MHz */

/* 8 bytes */
typedef struct {
    uint8_t it_version;  /* ʹ��radiotap header����Ҫ�汾, Ŀǰ����Ϊ0 */
    uint8_t it_pad;      /* Ŀǰ��δʹ��, ֻ��Ϊ��4�ֽڶ��� */
    uint16_t it_len;     /* radiotap�ĳ���, ������header+fields */
    uint32_t it_present; /* ͨ��bitλ����fields����Щ��Ա */
} ieee80211_radiotap_header_stru;

/* 30 bytes */
typedef struct {
    uint64_t ull_timestamp;    /* ��ǰ֡��ʱ���, ��λΪus */
    uint8_t uc_flags;          /* ��־λ */
    uint8_t uc_data_rate;      /* TX/RX��������, ��λΪ500Kbps */
    uint16_t us_channel_freq;  /* AP�����ŵ�������Ƶ��, ��λMHz */
    uint16_t us_channel_type;  /* �ŵ�����, ��ʶ5G����2G */
    int8_t c_ssi_signal;       /* �ź�ǿ��, ��λΪdBm */
    int8_t c_ssi_noise;

    union {
        uint8_t uc_mcs_info_flags;
        struct {
            uint8_t uc_mcs_info_rate;
            int16_t s_signal_quality;  /* �������岻��, ��Ʒ���ļ��㷽����RSSI + 94 */
            uint16_t us_vht_known; /* vht��Ϣ, 11acЭ��ʱ���ֶ���Ч */
            uint8_t uc_mcs_info_known; /* mcs��Ϣ, 11nЭ��ʱ���ֶ���Ч */
            uint8_t uc_vht_flags;
            uint8_t uc_mcs_info_flags;
            uint8_t uc_vht_bandwidth;
            uint8_t uc_vht_mcs_nss[BYTE_OFFSET_4];
            uint8_t uc_vht_coding;
            uint8_t uc_vht_group_id;
            uint16_t us_vht_partial_aid;
        } st_legacy_wifi_info; /* radiotap info before WiFi5  */
        struct {
            uint16_t he_info[6]; // 16+12 = 28
            uint16_t he_mu_info[2]; // +4
            uint8_t  he_mu_ru_ch1[4]; // +4
            uint8_t  he_mu_ru_ch2[4]; // +4
        } st_wifi6_info; /* RadioTap info of wifi6 */
    } extra_info;
} ieee80211_radiotap_fields_stru;

typedef struct {
    ieee80211_radiotap_header_stru st_radiotap_header; /* radiotapͷ�ṹ�� */
    ieee80211_radiotap_fields_stru st_radiotap_fields; /* radiotap����ṹ�� */
} ieee80211_radiotap_stru;

/* Radiotap��չ���ֵ�vht info���ӳ�Աbandwidth */
typedef enum {
    IEEE80211_RADIOTAP_VHT_BW_20 = 0,
    IEEE80211_RADIOTAP_VHT_BW_40 = 1,
    IEEE80211_RADIOTAP_VHT_BW_80 = 4,
    IEEE80211_RADIOTAP_VHT_BW_160 = 11,

    IEEE80211_RADIOTAP_VHT_BW_BUTT
} ieee80211_radiotap_vht_bandwidth;
typedef uint8_t ieee80211_radiotap_vht_bandwidth_uint8;

#define IEEE80211_RADIOTAP_F_SHORTGI 0x80 /* short gi */


#endif

typedef enum {
    /* "��ǰ֡������Ϊ0������1���ϱ� 2������ 3�������ⶪ��4���ظ�֡����others������" */
    HAL_RX_PROC_FLAGS_RSV = 0,
    HAL_RX_PROC_FLAGS_RELEASE = 1,
    HAL_RX_PROC_FLAGS_BUFFER = 2,
    HAL_RX_PROC_FLAGS_OUT_OF_WIND_DROP = 3,
    HAL_RX_PROC_FLAGS_DUPLICATE_DROP = 4,

    HAL_RX_PROC_FLAGS_BUTT
} hal_rx_proc_flags_enum;
typedef uint8_t hal_rx_proc_flags_enum_uint8;

typedef void (*host_board_init_func)(void);
typedef void (*host_rx_add_buff_func)(hal_host_device_stru *hal_device, uint8_t en_queue_num);
typedef void (*host_chip_irq_init_func)(void);
typedef uint8_t (*host_get_rx_msdu_status_func)(oal_netbuf_stru *netbuf);
typedef void (*host_rx_amsdu_list_build_func)(hal_host_device_stru *hal_device, oal_netbuf_stru *netbuf);
typedef oal_netbuf_stru *(*rx_get_next_sub_msdu_func)(hal_host_device_stru *hal_device, oal_netbuf_stru *rx_msdu_dscr);
typedef uint32_t (*host_rx_get_msdu_info_dscr_func)(hal_host_device_stru *hal_dev,
    oal_netbuf_stru *netbuf, mac_rx_ctl_stru *rx_info);
typedef uint32_t (*rx_host_start_dscr_queue_func)(uint8_t hal_dev_id);
typedef int32_t (*rx_host_init_dscr_queue_func)(uint8_t hal_dev_id);
typedef int32_t (*rx_host_stop_dscr_queue_func)(uint8_t hal_dev_id);
typedef void (*rx_free_res_func)(hal_host_device_stru *hal_device);
typedef void (*tx_ba_info_dscr_get_func)(uint8_t *ba_info, uint32_t size, hal_tx_ba_info_stru *tx_ba_info);
typedef void (*tx_msdu_dscr_info_get_func)(oal_netbuf_stru *netbuf, hal_tx_msdu_dscr_info_stru *tx_msdu_info);
typedef void (*host_tx_intr_init_func)(uint8_t hal_dev_id);
typedef uint32_t (*host_rx_reset_smac_handler_func)(frw_event_mem_stru *event_mem);
typedef void (*rx_alloc_list_free_func)(hal_host_device_stru *hal_dev, hal_host_rx_alloc_list_stru *alloc_list);
typedef void (*host_al_rx_fcs_info_func)(hmac_vap_stru *hmac_vap);
typedef void (*host_get_rx_pckt_info_func)(hmac_vap_stru *hmac_vap,
    dmac_atcmdsrv_atcmd_response_event *rx_pkcg_event_info);
typedef uint32_t (*host_rx_proc_msdu_dscr_func)(hal_host_device_stru *hal_dev, oal_netbuf_stru *netbuf);
typedef uint32_t (*host_ftm_csi_init_func)(hal_host_device_stru *hal_device);
typedef uint32_t (*host_csi_config_func)(mac_vap_stru *mac_vap, uint16_t len, uint8_t *param);
typedef uint32_t (*host_get_csi_info_func)(hmac_csi_info_stru *hmac_csi_info, uint8_t *addr);
typedef hmac_vap_stru *(*host_vap_get_hal_vap_id_func)(uint8_t hal_device_id, uint8_t hal_vap_id);
typedef uint32_t (*host_ftm_reg_init_func)(uint8_t hal_dev_id);
typedef uint32_t (*host_ftm_get_info_func)(hal_ftm_info_stru *hal_ftm_info, uint8_t *addr);
typedef void (*host_ftm_get_divider_func)(hal_host_device_stru *hal_device, uint8_t *divider);
typedef void (*host_ftm_set_sample_func)(hal_host_device_stru *hal_device, oal_bool_enum_uint8 ftm_status);
typedef void (*host_ftm_set_enable_func)(hal_host_device_stru *hal_device, oal_bool_enum_uint8 ftm_status);
typedef void (*host_ftm_set_m2s_phy_func)(hal_host_device_stru *hal_device, uint8_t tx_chain_selection,
                                          uint8_t tx_rssi_selection);
typedef void (*host_ftm_set_buf_base_addr_func)(hal_host_device_stru *hal_device, uint64_t devva);
typedef void (*host_ftm_set_buf_size_func)(hal_host_device_stru *hal_device, uint16_t cfg_ftm_buf_size);
typedef uint32_t (*host_ftm_set_white_list_func)(hal_host_device_stru *hal_device, uint8_t idx, uint8_t *mac_addr);
typedef int32_t (*host_init_common_timer)(hal_mac_common_timer *mac_timer);
typedef void (*host_set_mac_common_timer)(hal_mac_common_timer *mac_common_timer);

struct hal_common_ops_stru {
    host_chip_irq_init_func        host_chip_irq_init;
    host_rx_add_buff_func          host_rx_add_buff;
    host_get_rx_msdu_status_func   host_get_rx_msdu_status;
    host_rx_amsdu_list_build_func  host_rx_amsdu_list_build;
    rx_get_next_sub_msdu_func      rx_get_next_sub_msdu;
    host_rx_get_msdu_info_dscr_func host_rx_get_msdu_info_dscr;
    rx_host_start_dscr_queue_func  rx_host_start_dscr_queue;
    rx_host_init_dscr_queue_func   rx_host_init_dscr_queue;
    rx_host_stop_dscr_queue_func   rx_host_stop_dscr_queue;
    rx_free_res_func              rx_free_res;
    tx_ba_info_dscr_get_func       tx_ba_info_dscr_get;
    tx_msdu_dscr_info_get_func     tx_msdu_dscr_info_get;
    host_tx_intr_init_func         host_tx_intr_init;
    host_rx_reset_smac_handler_func host_rx_reset_smac_handler;
    rx_alloc_list_free_func        rx_alloc_list_free;
    host_al_rx_fcs_info_func       host_al_rx_fcs_info;
    host_get_rx_pckt_info_func     host_get_rx_pckt_info;
    host_rx_proc_msdu_dscr_func    host_rx_proc_msdu_dscr;
    host_init_common_timer    host_init_common_timer;
    host_set_mac_common_timer host_set_mac_common_timer;
#ifdef _PRE_WLAN_FEATURE_CSI
    host_get_csi_info_func         host_get_csi_info;
    host_csi_config_func           host_csi_config;
    host_ftm_csi_init_func         host_ftm_csi_init;
#endif
#ifdef _PRE_WLAN_FEATURE_FTM
    host_vap_get_hal_vap_id_func   host_vap_get_hal_vap_id;
    host_ftm_reg_init_func         host_ftm_reg_init;
    host_ftm_get_info_func         host_ftm_get_info;
    host_ftm_get_divider_func      host_ftm_get_divider;
    host_ftm_set_sample_func       host_ftm_set_sample;
    host_ftm_set_enable_func       host_ftm_set_enable;
    host_ftm_set_m2s_phy_func      host_ftm_set_m2s_phy;
    host_ftm_set_buf_base_addr_func host_ftm_set_buf_base_addr;
    host_ftm_set_buf_size_func     host_ftm_set_buf_size;
    host_ftm_set_white_list_func   host_ftm_set_white_list;
#endif
};

typedef struct {
    hal_host_device_stru        *hal_dev;
    oal_netbuf_head_stru         netbuf_head;
} hal_host_rx_event;

typedef struct {
    uint8_t  chip_id;
    uint8_t  hal_device_id;
    uint32_t ftm_csi_info;
    uint64_t csi_info_addr;
    uint64_t ftm_info_addr;
} hal_host_location_isr_stru;
#endif
