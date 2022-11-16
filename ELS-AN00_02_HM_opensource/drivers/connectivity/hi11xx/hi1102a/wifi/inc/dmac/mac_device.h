
#ifndef __MAC_DEVICE_H__
#define __MAC_DEVICE_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "wlan_spec.h"
#include "hal_ext_if.h"
#include "mac_vap.h"
#include "hal_commom_ops.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_DEVICE_H
/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define MAC_NET_DEVICE_NAME_LENGTH 16

#define MAC_DATARATES_PHY_80211G_NUM 12

#define DMAC_BA_LUT_IDX_BMAP_LEN ((HAL_MAX_BA_LUT_SIZE + 7) >> 3)
#define DMAC_TX_BA_LUT_BMAP_LEN  ((HAL_MAX_AMPDU_LUT_SIZE + 7) >> 3)

/* �쳣��ʱ�ϱ�ʱ�� */
#define MAC_EXCEPTION_TIME_OUT 10000

/* DMAC SCANNER ɨ��ģʽ */
#define MAC_SCAN_FUNC_MEAS       0x1
#define MAC_SCAN_FUNC_STATS      0x2
#define MAC_SCAN_FUNC_RADAR      0x4
#define MAC_SCAN_FUNC_BSS        0x8
#define MAC_SCAN_FUNC_P2P_LISTEN 0x10
#define MAC_SCAN_FUNC_ALL        (MAC_SCAN_FUNC_MEAS | MAC_SCAN_FUNC_STATS | MAC_SCAN_FUNC_RADAR | MAC_SCAN_FUNC_BSS)

#define MAC_ERR_LOG(_uc_vap_id, _puc_string)
#define MAC_ERR_LOG1(_uc_vap_id, _puc_string, _l_para1)
#define MAC_ERR_LOG2(_uc_vap_id, _puc_string, _l_para1, _l_para2)
#define MAC_ERR_LOG3(_uc_vap_id, _puc_string, _l_para1, _l_para2, _l_para3)
#define MAC_ERR_LOG4(_uc_vap_id, _puc_string, _l_para1, _l_para2, _l_para3, _l_para4)
#define MAC_ERR_VAR(_uc_vap_id, _c_fmt, ...)

#define MAC_WARNING_LOG(_uc_vap_id, _puc_string)
#define MAC_WARNING_LOG1(_uc_vap_id, _puc_string, _l_para1)
#define MAC_WARNING_LOG2(_uc_vap_id, _puc_string, _l_para1, _l_para2)
#define MAC_WARNING_LOG3(_uc_vap_id, _puc_string, _l_para1, _l_para2, _l_para3)
#define MAC_WARNING_LOG4(_uc_vap_id, _puc_string, _l_para1, _l_para2, _l_para3, _l_para4)
#define MAC_WARNING_VAR(_uc_vap_id, _c_fmt, ...)

#define MAC_INFO_LOG(_uc_vap_id, _puc_string)
#define MAC_INFO_LOG1(_uc_vap_id, _puc_string, _l_para1)
#define MAC_INFO_LOG2(_uc_vap_id, _puc_string, _l_para1, _l_para2)
#define MAC_INFO_LOG3(_uc_vap_id, _puc_string, _l_para1, _l_para2, _l_para3)
#define MAC_INFO_LOG4(_uc_vap_id, _puc_string, _l_para1, _l_para2, _l_para3, _l_para4)
#define MAC_INFO_VAR(_uc_vap_id, _c_fmt, ...)
#define MAC_EVENT_STATE_CHANGE(_puc_macaddr, _uc_vap_id, en_event_type, output_data, data_len)

/* ��ȡ�豸���㷨˽�нṹ�� */
#define MAC_DEV_ALG_PRIV(_pst_dev) ((_pst_dev)->p_alg_priv)

/* ��λ״̬ */
#define MAC_DEV_RESET_IN_PROGRESS(_pst_device, uc_value) ((_pst_device)->uc_device_reset_in_progress = uc_value)
#define MAC_DEV_IS_RESET_IN_PROGRESS(_pst_device)        ((_pst_device)->uc_device_reset_in_progress)

#define MAC_DFS_RADAR_WAIT_TIME_MS 60000

#define MAC_DEV_MAX_40M_INTOL_USER_BITMAP_LEN 4
#define MAC_SCAN_CHANNEL_INTERVAL_DEFAULT         6   /* ���6���ŵ����лع����ŵ�����һ��ʱ�� */
#define MAC_WORK_TIME_ON_HOME_CHANNEL_DEFAULT     100 /* ����ɨ��ʱ�����ع����ŵ�������ʱ�� */
#define MAC_SCAN_CHANNEL_INTERVAL_PERFORMANCE     2   /* ���2���ŵ����лع����ŵ�����һ��ʱ�� */
#define MAC_WORK_TIME_ON_HOME_CHANNEL_PERFORMANCE 60  /* WLANδ������P2P���������ع����ŵ�������ʱ�� */

#define MAC_FCS_DBAC_IGNORE     0 /* ����DBAC���� */
#define MAC_FCS_DBAC_NEED_CLOSE 1 /* DBAC��Ҫ�ر� */
#define MAC_FCS_DBAC_NEED_OPEN  2 /* DBAC��Ҫ���� */

#ifdef _PRE_WLAN_FEATURE_IP_FILTER
#define MAC_MAX_IP_FILTER_BTABLE_SIZE 512 /* rx ip���ݰ����˹��ܵĺ�������С */
#endif                                    // _PRE_WLAN_FEATURE_IP_FILTER

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/* SDT����ģʽö�� */
typedef enum {
    MAC_SDT_MODE_WRITE = 0,
    MAC_SDT_MODE_READ,

    MAC_SDT_MODE_BUTT
} mac_sdt_rw_mode_enum;
typedef oal_uint8 mac_sdt_rw_mode_enum_uint8;

typedef enum {
    MAC_CH_TYPE_NONE = 0,
    MAC_CH_TYPE_PRIMARY = 1,
    MAC_CH_TYPE_SECONDARY = 2,
} mac_ch_type_enum;
typedef oal_uint8 mac_ch_type_enum_uint8;

typedef enum {
    MAC_SCAN_OP_INIT_SCAN,
    MAC_SCAN_OP_FG_SCAN_ONLY,
    MAC_SCAN_OP_BG_SCAN_ONLY,

    MAC_SCAN_OP_BUTT
} mac_scan_op_enum;
typedef oal_uint8 mac_scan_op_enum_uint8;

typedef enum {
    MAC_CHAN_NOT_SUPPORT = 0,      /* ������֧�ָ��ŵ� */
    MAC_CHAN_AVAILABLE_ALWAYS,     /* �ŵ�һֱ����ʹ�� */
    MAC_CHAN_AVAILABLE_TO_OPERATE, /* �������(CAC, etc...)�󣬸��ŵ�����ʹ�� */
    MAC_CHAN_DFS_REQUIRED,         /* ���ŵ���Ҫ�����״��� */
    MAC_CHAN_BLOCK_DUE_TO_RADAR,   /* ���ڼ�⵽�״ﵼ�¸��ŵ���Ĳ����� */

    MAC_CHAN_STATUS_BUTT
} mac_chan_status_enum;
typedef oal_uint8 mac_chan_status_enum_uint8;

#ifdef _PRE_WLAN_DFT_STAT
typedef enum {
    MAC_DEV_MGMT_STAT_TYPE_TX = 0,
    MAC_DEV_MGMT_STAT_TYPE_RX,
    MAC_DEV_MGMT_STAT_TYPE_TX_COMPLETE,

    MAC_DEV_MGMT_STAT_TYPE_BUTT
} mac_dev_mgmt_stat_type_enum;
typedef oal_uint8 mac_dev_mgmt_stat_type_enum_uint8;
#endif

/* device resetͬ��������ö�� */
typedef enum {
    MAC_RESET_SWITCH_SET_TYPE,
    MAC_RESET_SWITCH_GET_TYPE,
    MAC_RESET_STATUS_GET_TYPE,
    MAC_RESET_STATUS_SET_TYPE,
    MAC_RESET_SWITCH_SYS_TYPE = MAC_RESET_SWITCH_SET_TYPE,
    MAC_RESET_STATUS_SYS_TYPE = MAC_RESET_STATUS_SET_TYPE,

    MAC_RESET_SYS_TYPE_BUTT
} mac_reset_sys_type_enum;
typedef oal_uint8 mac_reset_sys_type_enum_uint8;

typedef enum {
    MAC_TRY_INIT_SCAN_VAP_UP,
    MAC_TRY_INIT_SCAN_SET_CHANNEL,
    MAC_TRY_INIT_SCAN_START_DBAC,
    MAC_TRY_INIT_SCAN_RESCAN,

    MAC_TRY_INIT_SCAN_BUTT
} mac_try_init_scan_type;
typedef oal_uint8 mac_try_init_scan_type_enum_uint8;

typedef enum {
    MAC_INIT_SCAN_NOT_NEED,
    MAC_INIT_SCAN_NEED,
    MAC_INIT_SCAN_IN_SCAN,
} mac_need_init_scan_res;
typedef oal_uint8 mac_need_init_scan_res_enum_uint8;

typedef enum {
    MAC_ONE_PACKET_INDEX_1,
    MAC_ONE_PACKET_INDEX_2,
} mac_one_packet_index;
typedef oal_uint8 mac_one_packet_index_enum_uint8;

#define MAC_FCS_MAX_CHL_NUM               2
#define MAC_FCS_TIMEOUT_JIFFY             2
#define MAC_FCS_DEFAULT_PROTECT_TIME_OUT  5120  /* us */
#define MAC_FCS_DEFAULT_PROTECT_TIME_OUT2 1024  /* us */
#define MAC_FCS_DEFAULT_PROTECT_TIME_OUT3 15000 /* us */
#define MAC_FCS_DEFAULT_PROTECT_TIME_OUT4 16000 /* us */
#define MAC_FCS_DEFAULT_PROTECT_TIME_OUT5 9000  /* us ��VAP����������ɨ�跢��null֡�ĳ�ʱʱ������null֡�ķ��ͳɹ��� */

#define MAC_ONE_PACKET_TIME_OUT  1000
#define MAC_ONE_PACKET_TIME_OUT3 2000
#define MAC_ONE_PACKET_TIME_OUT4 2000

#define MAC_FCS_CTS_MAX_DURATION 32767 /* us */

#define MAC_FCS_CTS_MAX_BTCOEX_NOR_DURATION  30000 /* us */
#define MAC_FCS_CTS_MAX_BTCOEX_LDAC_DURATION 65535 /* us */

/*
 self CTS
+-------+-----------+----------------+
|frame  | duration  |      RA        |     len=10
|control|           |                |
+-------+-----------+----------------+

null data
+-------+-----------+---+---+---+--------+
|frame  | duration  |A1 |A2 |A3 |Seq Ctl | len=24
|control|           |   |   |   |        |
+-------+-----------+---+---+---+--------+
*/
typedef enum {
    MAC_FCS_NOTIFY_TYPE_SWITCH_AWAY = 0,
    MAC_FCS_NOTIFY_TYPE_SWITCH_BACK,

    MAC_FCS_NOTIFY_TYPE_BUTT
} mac_fcs_notify_type_enum;
typedef oal_uint8 mac_fcs_notify_type_enum_uint8;

typedef struct {
    mac_channel_stru st_dst_chl;
    mac_channel_stru st_src_chl;
    hal_one_packet_cfg_stru st_one_packet_cfg;
    oal_uint8 uc_src_fake_q_id; /* ԭ�ŵ���Ӧ��ٶ���id ȡֵ��Χ 0 1 */
    oal_uint8 uc_dst_fake_q_id; /* Ŀ���ŵ���Ӧ��ٶ���id ȡֵ��Χ 0 1 */
    oal_uint16 us_hot_cnt;

    mac_channel_stru st_src_chl2;
    hal_one_packet_cfg_stru st_one_packet_cfg2;
} mac_fcs_cfg_stru;

typedef enum {
    MAC_FCS_HOOK_ID_DBAC,
    MAC_FCS_HOOK_ID_ACS,

    MAC_FCS_HOOK_ID_BUTT
} mac_fcs_hook_id_enum;
typedef oal_uint8 mac_fcs_hook_id_enum_uint8;

typedef struct {
    mac_fcs_notify_type_enum_uint8 uc_notify_type;
    oal_uint8 uc_chip_id;
    oal_uint8 uc_device_id;
    oal_uint8 uc_resv[1];
    mac_fcs_cfg_stru st_fcs_cfg;
} mac_fcs_event_stru;

typedef void (*mac_fcs_notify_func)(const mac_fcs_event_stru *);

typedef struct {
    mac_fcs_notify_func p_func;
} mac_fcs_notify_node_stru;

typedef struct {
    mac_fcs_notify_node_stru ast_notify_nodes[MAC_FCS_HOOK_ID_BUTT];
} mac_fcs_notify_chain_stru;

typedef enum {
    MAC_FCS_STATE_STANDBY = 0,  // free to use
    MAC_FCS_STATE_REQUESTED,    // requested by other module, but not in switching
    MAC_FCS_STATE_IN_PROGESS,   // in switching

    MAC_FCS_STATE_BUTT
} mac_fcs_state_enum;
typedef oal_uint8 mac_fcs_state_enum_uint8;

typedef enum {
    MAC_FCS_SUCCESS = 0,
    MAC_FCS_ERR_NULL_PTR,
    MAC_FCS_ERR_INVALID_CFG,
    MAC_FCS_ERR_BUSY,
    MAC_FCS_ERR_UNKNOWN_ERR,
} mac_fcs_err_enum;
typedef oal_uint8 mac_fcs_err_enum_uint8;

typedef struct {
    oal_uint32 ul_offset_addr;
    oal_uint32 ul_value[MAC_FCS_MAX_CHL_NUM];
} mac_fcs_reg_record_stru;

typedef struct tag_mac_fcs_mgr_stru {
    volatile oal_bool_enum_uint8 en_fcs_done;
    oal_uint8 uc_chip_id;
    oal_uint8 uc_device_id;
    oal_uint8 uc_fcs_cnt;
    oal_spin_lock_stru st_lock;
    mac_fcs_state_enum_uint8 en_fcs_state;
    hal_fcs_service_type_enum_uint8 en_fcs_service_type;
    oal_uint8 uc_resv[2];
    mac_fcs_cfg_stru *pst_fcs_cfg;
    mac_fcs_notify_chain_stru ast_notify_chain[MAC_FCS_NOTIFY_TYPE_BUTT];
} mac_fcs_mgr_stru;

#define MAC_FCS_VERIFY_MAX_ITEMS 1
typedef enum {
    // isr
    MAC_FCS_STAGE_INTR_START,
    MAC_FCS_STAGE_INTR_POST_EVENT,
    MAC_FCS_STAGE_INTR_DONE,

    // event
    MAC_FCS_STAGE_EVENT_START,
    MAC_FCS_STAGE_PAUSE_VAP,
    MAC_FCS_STAGE_ONE_PKT_START,
    MAC_FCS_STAGE_ONE_PKT_INTR,
    MAC_FCS_STAGE_ONE_PKT_DONE,
    MAC_FCS_STAGE_RESET_HW_START,
    MAC_FCS_STAGE_RESET_HW_DONE,
    MAC_FCS_STAGE_RESUME_VAP,
    MAC_FCS_STAGE_EVENT_DONE,

    MAC_FCS_STAGE_COUNT
} mac_fcs_stage_enum;
typedef mac_fcs_stage_enum mac_fcs_stage_enum_uint8;

typedef struct {
    oal_bool_enum_uint8 en_enable;
    oal_uint8 auc_resv[3];
    oal_uint32 ul_item_cnt;
    oal_uint32 aul_timestamp[MAC_FCS_VERIFY_MAX_ITEMS][MAC_FCS_STAGE_COUNT];
} mac_fcs_verify_stat_stru;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/* �ϱ��ؼ���Ϣ��flags�����Ϣ����Ӧ���λΪ1������ϱ���Ӧ��Ϣ */
typedef enum {
    MAC_REPORT_INFO_FLAGS_HARDWARE_INFO = BIT(0),
    MAC_REPORT_INFO_FLAGS_QUEUE_INFO = BIT(1),
    MAC_REPORT_INFO_FLAGS_MEMORY_INFO = BIT(2),
    MAC_REPORT_INFO_FLAGS_EVENT_INFO = BIT(3),
    MAC_REPORT_INFO_FLAGS_VAP_INFO = BIT(4),
    MAC_REPORT_INFO_FLAGS_USER_INFO = BIT(5),
    MAC_REPORT_INFO_FLAGS_TXRX_PACKET_STATISTICS = BIT(6),
    MAC_REPORT_INFO_FLAGS_BUTT = BIT(7),
} mac_report_info_flags;
#endif

#ifdef _PRE_WLAN_FEATURE_IP_FILTER
/* rx����ָ�����ݰ������������� */
typedef enum {
    MAC_ICMP_FILTER = 1, /* ICMP���ݰ����� */

    MAC_FILTER_ID_BUTT
} mac_assigned_filter_id_enum;
typedef oal_uint8 mac_assigned_filter_id_enum_uint8;

/* ָ��filter���������ʽ */
typedef struct {
    mac_assigned_filter_id_enum_uint8 uc_filter_id;
    oal_bool_enum_uint8 en_enable;  /* �·����˹���ʹ�ܿ������� */
    oal_uint16 us_rx_icmp_pkgs_num; /* ��չ����ͳ�ƹ��˵�icmp�������� */
} mac_assigned_filter_cmd_stru;

/* rx ip���ݰ����˵��������� */
typedef enum {
    MAC_IP_FILTER_ENABLE = 0,        /* ��/��ip���ݰ����˹��� */
    MAC_IP_FILTER_UPDATE_BTABLE = 1, /* ���º����� */
    MAC_IP_FILTER_CLEAR = 2,         /* ��������� */

    MAC_IP_FILTER_BUTT
} mac_ip_filter_cmd_enum;
typedef oal_uint8 mac_ip_filter_cmd_enum_uint8;

/* ��������Ŀ��ʽ */
typedef struct {
    oal_uint16 us_port; /* Ŀ�Ķ˿ںţ��������ֽ����ʽ�洢 */
    oal_uint8 uc_protocol;
    oal_uint8 uc_resv;
    // oal_uint32                  ul_filter_cnt; /* Ŀǰδ����"ͳ�ƹ��˰�����"�����󣬴˳�Ա�ݲ�ʹ�� */
} mac_ip_filter_item_stru;

/* ���������ʽ */
typedef struct {
    oal_uint8 uc_item_count;
    oal_bool_enum_uint8 en_enable; /* �·�����ʹ�ܱ�־ */
    mac_ip_filter_cmd_enum_uint8 en_cmd;
    oal_uint8 uc_resv;
    mac_ip_filter_item_stru ast_filter_items_items[1];
} mac_ip_filter_cmd_stru;

#endif  // _PRE_WLAN_FEATURE_IP_FILTER

#ifdef _PRE_WLAN_FEATURE_APF
#define APF_PROGRAM_MAX_LEN 512
#define APF_VERSION         2
typedef enum {
    APF_SET_FILTER_CMD,
    APF_GET_FILTER_CMD
} mac_apf_cmd_type_enum;
typedef oal_uint8 mac_apf_cmd_type_uint8;

typedef struct {
    oal_bool_enum_uint8 en_is_enabled;
    oal_uint8 auc_res[1];
    oal_uint16 us_program_len;
    oal_uint32 ul_install_timestamp;
    oal_uint32 ul_flt_pkt_cnt;
    oal_uint8 auc_program[APF_PROGRAM_MAX_LEN];
} mac_apf_stru;

typedef struct {
    mac_apf_cmd_type_uint8 en_cmd_type;
    oal_uint8 auc_res[1];
    oal_uint16 us_program_len;
    oal_uint8 *puc_program;
} mac_apf_filter_cmd_stru;

typedef struct {
    oal_void *p_program;
} mac_apf_report_event_stru;

#endif

#ifdef _PRE_WLAN_FEATURE_FTM
typedef enum {
    NO_LOCATION = 0,
    ROOT_AP = 1,
    REPEATER = 2,
    STA = 3,
    LOCATION_TYPE_BUTT
} oal_location_type_enum;
typedef oal_uint8 oal_location_type_enum_uint8;
#endif

typedef enum {
    MAC_CSA_FLAG_NORMAL = 0,
    MAC_CSA_FLAG_START_DEBUG, /* �̶�csa ie ��beacon֡�� */
    MAC_CSA_FLAG_CANCLE_DEBUG,

    MAC_CSA_FLAG_BUTT
} mac_csa_flag_enum;
typedef oal_uint8 mac_csa_flag_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_BTCOEX
typedef enum {
    MAC_BTCOEX_CFG_ONEPKT_TYPE,  // ���ù��泡����one pkt֡������
    MAC_BTCOEX_CFG_RSP_CTS,      // ���ù��泡�����Ƿ�ظ�CTS
    MAC_BTCOEX_CFG_BA_SIZE,      // ���öԶ˷��;ۺϸ���
    MAC_BTCOEX_CFG_BUTT
} mac_btcoex_cfg_type_enum;
typedef oal_uint8 mac_btcoex_cfg_type_enum_uint8;
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
typedef struct {
    mac_btcoex_cfg_type_enum_uint8 en_cfg_type;
    oal_uint8 uc_cfg_value;
    oal_uint8 auc_res[2];
} mac_btcoex_cfg_stru;
#endif
/* device reset�¼�ͬ���ṹ�� */
typedef struct {
    mac_reset_sys_type_enum_uint8 en_reset_sys_type; /* ��λͬ������ */
    oal_uint8 uc_value;                              /* ͬ����Ϣֵ */
    oal_uint8 uc_resv[2];
} mac_reset_sys_stru;

typedef void (*mac_scan_cb_fn)(void *p_scan_record);

typedef struct {
    oal_uint16 us_num_networks;
    mac_ch_type_enum_uint8 en_ch_type;
    oal_uint8 auc_resv[1];
} mac_ap_ch_info_stru;

typedef struct {
    oal_uint16 us_num_networks; /* ��¼��ǰ�ŵ���ɨ�赽��BSS���� */
    oal_uint8 auc_resv[2];
    oal_uint8 auc_bssid_array[WLAN_MAX_SCAN_BSS_PER_CH][WLAN_MAC_ADDR_LEN]; /* ��¼��ǰ�ŵ���ɨ�赽������BSSID */
} mac_bss_id_list_stru;

#define MAX_PNO_SSID_COUNT      16
#define MAX_PNO_REPEAT_TIMES    4
#define PNO_SCHED_SCAN_INTERVAL (60 * 1000)

/* PNOɨ����Ϣ�ṹ�� */
typedef struct {
    oal_uint8 auc_ssid[WLAN_SSID_MAX_LEN];
    oal_bool_enum_uint8 en_scan_ssid;
    oal_uint8 auc_resv[2];
} pno_match_ssid_stru;

typedef struct {
    pno_match_ssid_stru ast_match_ssid_set[MAX_PNO_SSID_COUNT];
    union {
        oal_int32 l_ssid_count;     /* �·�����Ҫƥ���ssid���ĸ��� */
        oal_uint32 ul_work_time_ms; /* խ���̼�����ΪROM��,����Ϊwork_time */
    };
    union {
        oal_int32 l_rssi_thold;           /* ���ϱ���rssi���� */
        oal_uint32 ul_listen_interval_ms; /* խ���̼�����ΪROM��,����Ϊ���� */
    };

    oal_uint32 ul_pno_scan_interval;                /* pnoɨ���� */
    oal_uint8 auc_sour_mac_addr[WLAN_MAC_ADDR_LEN]; /* probe req֡��Я���ķ��Ͷ˵�ַ */
    oal_uint8 uc_pno_scan_repeat;                   /* pnoɨ���ظ����� */
    oal_bool_enum_uint8 en_is_random_mac_addr_scan; /* �Ƿ����mac */

    mac_scan_cb_fn p_fn_cb; /* ����ָ������������˼�ͨ�ų����� */
} mac_pno_scan_stru;

/* PNO����ɨ������ṹ�� */
typedef struct {
    mac_pno_scan_stru st_pno_sched_scan_params; /* pno����ɨ������Ĳ��� */
    // frw_timeout_stru        st_pno_sched_scan_timer;              /* pno����ɨ�趨ʱ�� */
    oal_void *p_pno_sched_scan_timer;           /* pno����ɨ��rtcʱ�Ӷ�ʱ�����˶�ʱ����ʱ���ܹ�����˯�ߵ�device */
    oal_uint8 uc_curr_pno_sched_scan_times;     /* ��ǰpno����ɨ����� */
    oal_bool_enum_uint8 en_is_found_match_ssid; /* �Ƿ�ɨ�赽��ƥ���ssid */
    oal_uint8 auc_resv[2];
} mac_pno_sched_scan_mgmt_stru;

typedef struct {
    oal_uint8 auc_ssid[WLAN_SSID_MAX_LEN];
    oal_uint8 auc_resv[3];
} mac_ssid_stru;

/* ɨ������ṹ�� */
typedef struct {
    wlan_mib_desired_bsstype_enum_uint8 en_bss_type; /* Ҫɨ���bss���� */
    wlan_scan_type_enum_uint8 en_scan_type;          /* ����/���� */
    oal_uint8 uc_bssid_num;                          /* ����ɨ���bssid���� */
    oal_uint8 uc_ssid_num;                           /* ����ɨ���ssid���� */

    oal_uint8 auc_sour_mac_addr[WLAN_MAC_ADDR_LEN]; /* probe req֡��Я���ķ��Ͷ˵�ַ */
    oal_uint8 uc_p2p0_listen_channel;
    oal_bool_enum_uint8 en_working_in_home_chan;

    oal_uint8 auc_bssid[WLAN_SCAN_REQ_MAX_BSSID][WLAN_MAC_ADDR_LEN]; /* ������bssid */
    mac_ssid_stru ast_mac_ssid_set[WLAN_SCAN_REQ_MAX_SSID];          /* ������ssid */

    oal_uint8 uc_max_scan_count_per_channel;           /* ÿ���ŵ���ɨ����� */
    oal_uint8 uc_max_send_probe_req_count_per_channel; /* ÿ���ŵ�����ɨ������֡�ĸ�����Ĭ��Ϊ1 */
    oal_uint8 auc_resv[2];

    oal_bool_enum_uint8 en_need_switch_back_home_channel; /* ����ɨ��ʱ��ɨ����һ���ŵ����ж��Ƿ���Ҫ�лع����ŵ����� */
    oal_uint8 uc_scan_channel_interval;                   /* ���n���ŵ����лع����ŵ�����һ��ʱ�� */
    oal_uint16 us_work_time_on_home_channel;              /* ����ɨ��ʱ�����ع����ŵ�������ʱ�� */

    oal_uint8 uc_last_channel_band;
    oal_uint8 bit_is_p2p0_scan : 1;                 /* �Ƿ�Ϊp2p0 ����ɨ�� */
    oal_uint8 bit_rsv : 7;                          /* ����λ */
    oal_bool_enum_uint8 en_is_random_mac_addr_scan; /* �Ƿ������mac addrɨ�� */
    oal_bool_enum_uint8 en_abort_scan_flag;         /* ��ֹɨ�� */

    mac_channel_stru ast_channel_list[WLAN_MAX_CHANNEL_NUM];

    oal_uint8 uc_channel_nums; /* �ŵ��б����ŵ��ĸ��� */
    oal_uint8 uc_probe_delay;  /* ����ɨ�跢probe request֮֡ǰ�ĵȴ�ʱ�� */
    oal_uint16 us_scan_time;   /* ɨ����ĳһ�ŵ�ͣ����ʱ���ɨ�����, ms��������10�������� */

    wlan_scan_mode_enum_uint8 en_scan_mode; /* ɨ��ģʽ:ǰ��ɨ�� or ����ɨ�� */
    oal_uint8 uc_curr_channel_scan_count;   /* ��¼��ǰ�ŵ���ɨ���������һ�η��͹㲥ssid��probe req֡�����淢��ָ��ssid��probe req֡ */
    oal_uint8 uc_scan_func;                 /* DMAC SCANNER ɨ��ģʽ */
    oal_uint8 uc_vap_id;                    /* �·�ɨ�������vap id */
    oal_uint64 ull_cookie;                  /* P2P �����·���cookie ֵ */

    /* ��Ҫ:�ص�����ָ��:����ָ������������˼�ͨ�ų����� */
    mac_scan_cb_fn p_fn_cb;
} mac_scan_req_stru;

/* ��ӡ���ձ��ĵ�rssi��Ϣ�ĵ��Կ�����صĽṹ�� */
typedef struct {
    oal_uint32 ul_rx_comp_isr_interval;          /* ������ٸ���������жϴ�ӡһ��rssi��Ϣ */
    oal_uint32 ul_curr_rx_comp_isr_count;        /* һ�ּ���ڣ���������жϵĲ������� */
    oal_bool_enum_uint8 en_rssi_debug_switch;    /* ��ӡ���ձ��ĵ�rssi��Ϣ�ĵ��Կ��� */
    oal_bool_enum_uint8 en_tsensor_debug_switch; /* Tsensor��Ϣ�ĵ��Կ��� */
#ifdef _PRE_WLAN_FIT_BASED_REALTIME_CALI
    oal_bool_enum_uint8 en_pdet_debug_switch; /* ��ӡоƬ�ϱ�pdetֵ�ĵ��Կ��� */
#endif
    oal_uint8 uc_edca_param_switch; /* EDCA�������ÿ��� */
    oal_uint8 uc_edca_aifsn;        /* EDCA����AFSIN */
    oal_uint8 uc_edca_cwmin;        /* EDCA����CWmin */
    oal_uint8 uc_edca_cwmax;        /* EDCA����CWmax */
    oal_uint16 us_edca_txoplimit;   /* EDCA����TXOP limit */
#ifdef _PRE_WLAN_FEATURE_DYN_BYPASS_EXTLNA
    oal_uint8 uc_extlna_chg_bypass_switch; /* ��̬bypass����LNA����: 0/1/2:no_bypass/dyn_bypass/force_bypass */
#endif
} mac_phy_debug_switch_stru;

typedef struct {
    oal_uint8 uc_opmode_switch;
} mac_opmode_switch_stru;
typedef struct {
    oal_uint32 ul_cmd_bit_map;
#ifdef _PRE_WLAN_FEATURE_APF
    oal_uint8 uc_apf_switch;
    oal_uint8 uc_ao_switch;
#endif
#ifdef _PRE_PM_DYN_SET_TBTT_OFFSET
    oal_uint8 uc_dto_switch;
    oal_uint16 us_torv_val;
#endif
} mac_pm_debug_cfg_stru;

typedef enum {
#ifdef _PRE_WLAN_FEATURE_APF
    MAC_PM_DEBUG_APF = 0,
    MAC_PM_DEBUG_AO = 1,
#endif
#ifdef _PRE_PM_DYN_SET_TBTT_OFFSET
    MAC_PM_DEBUG_DTO = 2,
    MAC_PM_DEBUG_TORV = 3,
#endif
    MAC_PM_DEBUG_CFG_BUTT
} mac_pm_debug_cfg_enum_uint8;

/* ACS ����ظ���ʽ */
typedef struct {
    oal_uint8 uc_cmd;
    oal_uint8 uc_chip_id;
    oal_uint8 uc_device_id;
    oal_uint8 uc_resv;

    oal_uint32 ul_len;     /* �ܳ��ȣ���������ǰ4���ֽ� */
    oal_uint32 ul_cmd_cnt; /* ����ļ��� */
} mac_acs_response_hdr_stru;

typedef struct {
    oal_uint8 uc_cmd;
    oal_uint8 auc_arg[3];
    oal_uint32 ul_cmd_cnt; /* ����ļ��� */
    oal_uint32 ul_cmd_len; /* �ܳ��ȣ���ָauc_data��ʵ�ʸ��س��� */
    oal_uint8 auc_data[4];
} mac_acs_cmd_stru;

typedef mac_acs_cmd_stru mac_init_scan_req_stru;

typedef enum {
    MAC_ACS_RSN_INIT,
    MAC_ACS_RSN_LONG_TX_BUF,
    MAC_ACS_RSN_LARGE_PER,
    MAC_ACS_RSN_MWO_DECT,
    MAC_ACS_RSN_RADAR_DECT,

    MAC_ACS_RSN_BUTT
} mac_acs_rsn_enum;
typedef oal_uint8 mac_acs_rsn_enum_uint8;

typedef enum {
    MAC_ACS_SW_NONE = 0x0,
    MAC_ACS_SW_INIT = 0x1,
    MAC_ACS_SW_DYNA = 0x2,
    MAC_ACS_SW_BOTH = 0x3,

    MAC_ACS_SW_BUTT
} en_mac_acs_sw_enum;
typedef oal_uint8 en_mac_acs_sw_enum_uint8;

typedef enum {
    MAC_ACS_SET_CH_DNYA = 0x0,
    MAC_ACS_SET_CH_INIT = 0x1,

    MAC_ACS_SET_CH_BUTT
} en_mac_acs_set_ch_enum;
typedef oal_uint8 en_mac_acs_set_ch_enum_uint8;

typedef struct {
    oal_bool_enum_uint8 en_sw_when_connected_enable : 1;
    oal_bool_enum_uint8 en_drop_dfs_channel_enable : 1;
    oal_bool_enum_uint8 en_lte_coex_enable : 1;
    en_mac_acs_sw_enum_uint8 en_acs_switch : 5;
} mac_acs_switch_stru;

/* DMAC SCAN �ŵ�ɨ��BSS��ϢժҪ�ṹ */
typedef struct {
    oal_int8 c_rssi;             /* bss���ź�ǿ�� */
    oal_uint8 uc_channel_number; /* �ŵ��� */
    oal_uint8 auc_bssid[WLAN_MAC_ADDR_LEN];

    /* 11n, 11ac��Ϣ */
    oal_bool_enum_uint8 en_ht_capable;                      /* �Ƿ�֧��ht */
    oal_bool_enum_uint8 en_vht_capable;                     /* �Ƿ�֧��vht */
    wlan_bw_cap_enum_uint8 en_bw_cap;                       /* ֧�ֵĴ��� 0-20M 1-40M */
    wlan_channel_bandwidth_enum_uint8 en_channel_bandwidth; /* �ŵ��������� */
} mac_scan_bss_stats_stru;

typedef struct {
    oal_uint8 uc_category;
    oal_uint8 uc_action_code;
    oal_uint8 auc_oui[3];
    oal_uint8 uc_eid;
    oal_uint8 uc_lenth;
    oal_uint8 uc_location_type;
    oal_uint8 auc_mac_server[WLAN_MAC_ADDR_LEN];
    oal_uint8 auc_mac_client[WLAN_MAC_ADDR_LEN];
    oal_uint8 auc_payload[4];
} mac_location_event_stru;

/* DMAC SCAN �ŵ�ͳ�Ʋ�������ṹ�� */
typedef struct {
    oal_uint8 uc_channel_number; /* �ŵ��� */
    oal_uint8 uc_stats_valid;
    oal_uint8 uc_stats_cnt;      /* �ŵ���æ��ͳ�ƴ��� */
    oal_uint8 uc_free_power_cnt; /* �ŵ����й��� */

    oal_uint8 uc_bandwidth_mode;
    oal_uint8 auc_resv[1];
    oal_int16 s_free_power_stats_20M;
    oal_int16 s_free_power_stats_40M;
    oal_int16 s_free_power_stats_80M;

    oal_uint32 ul_total_stats_time_us;
    oal_uint32 ul_total_free_time_20M_us;
    oal_uint32 ul_total_free_time_40M_us;
    oal_uint32 ul_total_free_time_80M_us;
    oal_uint32 ul_total_send_time_us;
    oal_uint32 ul_total_recv_time_us;

    oal_uint8 uc_radar_detected;
    oal_uint8 uc_radar_bw;
    oal_uint8 uc_radar_type;
    oal_uint8 uc_radar_freq_offset;
} mac_scan_chan_stats_stru;

typedef struct {
    oal_int8 c_rssi;             /* bss���ź�ǿ�� */
    oal_uint8 uc_channel_number; /* �ŵ��� */

    oal_bool_enum_uint8 en_ht_capable : 1;                      /* �Ƿ�֧��ht */
    oal_bool_enum_uint8 en_vht_capable : 1;                     /* �Ƿ�֧��vht */
    wlan_bw_cap_enum_uint8 en_bw_cap : 3;                       /* ֧�ֵĴ��� 0-20M 1-40M */
    wlan_channel_bandwidth_enum_uint8 en_channel_bandwidth : 3; /* �ŵ��������� */
} mac_scan_bss_stats_simple_stru;

typedef struct {
    oal_uint32 us_total_stats_time_ms : 9;  // max 512 ms
    oal_uint32 uc_bandwidth_mode : 3;
    oal_uint32 uc_radar_detected : 1;
    oal_uint32 uc_dfs_check_needed : 1;
    oal_uint32 uc_radar_bw : 3;
    oal_uint32 uc_radar_type : 4;
    oal_uint32 uc_radar_freq_offset : 3;
    oal_uint8 uc_channel_number; /* �ŵ��� */

    oal_int8 s_free_power_20M;  // dBm
    oal_int8 s_free_power_40M;
    oal_int8 s_free_power_80M;
    oal_uint8 uc_free_time_20M_rate;  // percent, 255 scaled
    oal_uint8 uc_free_time_40M_rate;
    oal_uint8 uc_free_time_80M_rate;
    oal_uint8 uc_total_send_time_rate;  // percent, 255 scaled
    oal_uint8 uc_total_recv_time_rate;
} mac_scan_chan_stats_simple_stru;

/* DMAC SCAN �ص��¼��ṹ�� */
typedef struct {
    oal_uint8 uc_nchans;    /* �ŵ����� */
    oal_uint8 uc_nbss;      /* BSS���� */
    oal_uint8 uc_scan_func; /* ɨ�������Ĺ��� */

    oal_uint8 uc_need_rank : 1;  // kernel write, app read
    oal_uint8 uc_obss_on : 1;
    oal_uint8 uc_dfs_on : 1;
    oal_uint8 uc_dbac_on : 1;
    oal_uint8 uc_chip_id : 2;
    oal_uint8 uc_device_id : 2;
} mac_scan_event_stru;

/* bss��ȫ�����Ϣ�ṹ�� */
typedef struct {
    oal_uint8 uc_bss_80211i_mode;                                      /* ָʾ��ǰAP�İ�ȫ��ʽ��WPA��WPA2��BIT0: WPA; BIT1:WPA2 */
    oal_uint8 uc_rsn_grp_policy;                                       /* ���ڴ��WPA2��ʽ�£�AP���鲥�����׼���Ϣ */
    oal_uint8 auc_rsn_pairwise_policy[MAC_PAIRWISE_CIPHER_SUITES_NUM]; /* ���ڴ��WPA2��ʽ�£�AP�ĵ��������׼���Ϣ */
    oal_uint8 auc_rsn_auth_policy[MAC_AUTHENTICATION_SUITE_NUM];       /* ���ڴ��WPA2��ʽ�£�AP����֤�׼���Ϣ */
    oal_uint8 uc_group_mgmt_policy;
    oal_uint8 auc_rsn_cap[2];                                          /* ���ڱ���RSN������Ϣ��ֱ�Ӵ�֡������copy���� */
    oal_uint8 auc_wpa_pairwise_policy[MAC_PAIRWISE_CIPHER_SUITES_NUM]; /* ���ڴ��WPA��ʽ�£�AP�ĵ��������׼���Ϣ */
    oal_uint8 auc_wpa_auth_policy[MAC_AUTHENTICATION_SUITE_NUM];       /* ���ڴ��WPA��ʽ�£�AP����֤�׼���Ϣ */
    oal_uint8 uc_wpa_grp_policy;                                       /* ���ڴ��WPA��ʽ�£�AP���鲥�����׼���Ϣ */
    oal_uint8 uc_grp_policy_match;                                     /* ���ڴ��ƥ����鲥�׼� */
    oal_uint8 uc_pairwise_policy_match;                                /* ���ڴ��ƥ��ĵ����׼� */
    oal_uint8 uc_auth_policy_match;                                    /* ���ڴ��ƥ�����֤�׼� */
} mac_bss_80211i_info_stru;

/* ɨ������¼�����״̬�� */
typedef enum {
    MAC_SCAN_SUCCESS = 0,    /* ɨ��ɹ� */
    MAC_SCAN_PNO = 1,        /* pnoɨ����� */
    MAC_SCAN_TIMEOUT = 2,    /* ɨ�賬ʱ */
    MAC_SCAN_REFUSED = 3,    /* ɨ�豻�ܾ� */
    MAC_SCAN_ABORT = 4,      /* ��ֹɨ�� */
    MAC_SCAN_ABORT_SYNC = 5, /* ɨ�豻��ֹͬ��״̬�������ϲ�ȥ��������ʱǿ��abort����ֱ�����ں��ϱ�scan abort�������dmac��Ӧabort��ɺ������ϱ� */
    MAC_SCAN_STATUS_BUTT,    /* ��Ч״̬�룬��ʼ��ʱʹ�ô�״̬�� */
} mac_scan_status_enum;
typedef oal_uint8 mac_scan_status_enum_uint8;

/* ɨ���� */
typedef struct {
    mac_scan_status_enum_uint8 en_scan_rsp_status;
    oal_uint8 auc_resv[3];
    oal_uint64 ull_cookie;
} mac_scan_rsp_stru;

/* ɨ�赽��BSS�����ṹ�� */
typedef struct {
    /* ������Ϣ */
    wlan_mib_desired_bsstype_enum_uint8 en_bss_type; /* bss�������� */
    oal_uint8 uc_dtim_period;                        /* dtime���� */
    oal_uint8 uc_dtim_cnt;                           /* dtime cnt */
    oal_bool_enum_uint8 en_11ntxbf;                  /* 11n txbf */
    oal_bool_enum_uint8 en_new_scan_bss;             /* �Ƿ�����ɨ�赽��BSS */
    wlan_ap_chip_oui_enum_uint8 en_is_tplink_oui;
    oal_int8 c_rssi;                               /* bss���ź�ǿ�� */
    oal_int8 ac_ssid[WLAN_SSID_MAX_LEN];           /* ����ssid */
    oal_uint16 us_beacon_period;                   /* beacon���� */
    oal_uint16 us_cap_info;                        /* ����������Ϣ */
    oal_uint8 auc_mac_addr[WLAN_MAC_ADDR_LEN];     /* ���������� mac��ַ��bssid��ͬ */
    oal_uint8 auc_bssid[WLAN_MAC_ADDR_LEN];        /* ����bssid */
    mac_channel_stru st_channel;                   /* bss���ڵ��ŵ� */
    oal_uint8 uc_wmm_cap;                          /* �Ƿ�֧��wmm */
    oal_uint8 uc_uapsd_cap;                        /* �Ƿ�֧��uapsd */
    oal_bool_enum_uint8 en_desired;                /* ��־λ����bss�Ƿ��������� */
    oal_uint8 uc_num_supp_rates;                   /* ֧�ֵ����ʼ����� */
    oal_uint8 auc_supp_rates[WLAN_MAX_SUPP_RATES]; /* ֧�ֵ����ʼ� */

#ifdef _PRE_WLAN_FEATURE_11D
    oal_int8 ac_country[WLAN_COUNTRY_STR_LEN]; /* �����ַ��� */
    oal_uint8 auc_resv2[1];
#endif

    /* ��ȫ��ص���Ϣ */
    mac_bss_80211i_info_stru st_bss_sec_info; /* ���ڱ���STAģʽ�£�ɨ�赽��AP��ȫ�����Ϣ */

    /* 11n 11ac��Ϣ */
    oal_bool_enum_uint8 en_ht_capable;            /* �Ƿ�֧��ht */
    oal_bool_enum_uint8 en_vht_capable;           /* �Ƿ�֧��vht */
    oal_bool_enum_uint8 en_epigram_vht_capable;   /* �Ƿ�֧��hidden vendor vht */
    oal_bool_enum_uint8 en_epigram_novht_capable; /* ˽��vendor�в�����Я��vht */

    wlan_bw_cap_enum_uint8 en_bw_cap;                       /* ֧�ֵĴ��� 0-20M 1-40M */
    wlan_channel_bandwidth_enum_uint8 en_channel_bandwidth; /* �ŵ����� */
    oal_uint8 uc_coex_mgmt_supp;                            /* �Ƿ�֧�ֹ������ */
    oal_bool_enum_uint8 en_ht_ldpc;                         /* �Ƿ�֧��ldpc */

    oal_bool_enum_uint8 en_ht_stbc; /* �Ƿ�֧��stbc */
    oal_uint8 uc_wapi;
    oal_uint8 auc_resv[1];
    oal_bool_enum_uint8 en_btcoex_blacklist_chip_oui; /* ps����one pkt֡������Ҫ�޶�Ϊself-cts�� */
    oal_uint32 ul_timestamp;                          /* ���´�bss��ʱ��� */

#ifdef _PRE_WLAN_NARROW_BAND
    oal_bool_enum_uint8 en_nb_capable; /* �Ƿ�֧��nb */
#endif
    oal_bool_enum_uint8 en_is_realtek_chip_oui; /* �Ƿ�realtekоƬap�����bt�������� */
    oal_bool_enum_uint8 en_11k_capable; /* �Ƿ�֧��11k */
    oal_bool_enum_uint8 en_11v_capable; /* �Ƿ�֧��11v */

#if defined(_PRE_WLAN_FEATURE_11K) || defined(_PRE_WLAN_FEATURE_FTM)
    oal_bool_enum_uint8 en_support_rrm; /* �Ƿ�֧��RRM */
#endif
    wlan_nss_enum_uint8 en_support_max_nss; /* ��AP֧�ֵ����ռ����� */
    oal_uint8 uc_num_sounding_dim;          /* ��AP����txbf�������� */

#ifdef _PRE_WLAN_FEATURE_ROAM
    oal_bool_enum_uint8 en_roam_blacklist_chip_oui; /* ��֧��roam */
#endif

    /* ����֡��Ϣ */
    oal_uint32 ul_mgmt_len;     /* ����֡�ĳ��� */
    oal_uint8 auc_mgmt_buff[4]; /* ��¼beacon֡��probe rsp֡ */
    // oal_uint8                        *puc_mgmt_buff;                     /* ��¼beacon֡��probe rsp֡ */
} mac_bss_dscr_stru;

#ifdef _PRE_WLAN_DFT_STAT
/* ����֡ͳ����Ϣ */
typedef struct {
    /* ���չ���֡ͳ�� */
    oal_uint32 aul_rx_mgmt[WLAN_MGMT_SUBTYPE_BUTT];

    /* �ҵ�Ӳ�����еĹ���֡ͳ�� */
    oal_uint32 aul_tx_mgmt_soft[WLAN_MGMT_SUBTYPE_BUTT];

    /* ������ɵĹ���֡ͳ�� */
    oal_uint32 aul_tx_mgmt_hardware[WLAN_MGMT_SUBTYPE_BUTT];
} mac_device_mgmt_statistic_stru;
#endif
#ifdef _PRE_WLAN_DFT_STAT
/* �ϱ��տڻ�����ά������Ŀ��ƽṹ */
typedef struct {
    oal_uint32 ul_non_directed_frame_num;               /* ���յ��Ǳ���֡����Ŀ */
    oal_uint8 uc_collect_period_cnt;                    /* �ɼ����ڵĴ���������100����ϱ�������Ȼ���������¿�ʼ */
    oal_bool_enum_uint8 en_non_directed_frame_stat_flg; /* �Ƿ�ͳ�ƷǱ�����ַ֡�����ı�־ */
    oal_int16 s_ant_power;                              /* ���߿ڹ��� */
    frw_timeout_stru st_collect_period_timer;           /* �ɼ����ڶ�ʱ�� */
} mac_device_dbb_env_param_ctx_stru;
#endif

typedef enum {
    MAC_DFR_TIMER_STEP_1 = 0,
    MAC_DFR_TIMER_STEP_2 = 1,
} mac_dfr_timer_step_enum;
typedef oal_uint8 mac_dfr_timer_step_enum_uint8;

typedef struct {
    oal_uint32 ul_tx_seqnum;         /* ���һ��tx�ϱ���SN�� */
    oal_uint16 us_seqnum_used_times; /* ����ʹ����ul_tx_seqnum�Ĵ��� */
    oal_uint16 us_incr_constant;     /* ά����Qos ��Ƭ֡�ĵ������� */
} mac_tx_seqnum_struc;

typedef struct {
    oal_uint8 uc_p2p_device_num;                /* ��ǰdevice�µ�P2P_DEVICE���� */
    oal_uint8 uc_p2p_goclient_num;              /* ��ǰdevice�µ�P2P_CL/P2P_GO���� */
    oal_uint8 uc_p2p0_vap_idx;                  /* P2P ���泡���£�P2P_DEV(p2p0) ָ�� */
    mac_vap_state_enum_uint8 en_last_vap_state; /* P2P0/P2P_CL ����VAP �ṹ�����������±���VAP �������ǰ��״̬ */
    oal_uint8 uc_resv[2];                       /* ���� */
    oal_uint8 en_roc_need_switch;               /* remain on channel����Ҫ�л�ԭ�ŵ� */
    oal_uint8 en_p2p_ps_pause;                  /* P2P �����Ƿ���pause״̬ */
    oal_net_device_stru *pst_p2p_net_device;    /* P2P ���泡������net_device(p2p0) ָ�� */
    oal_uint64 ull_send_action_id;              /* P2P action id/cookie */
    oal_uint64 ull_last_roc_id;
    oal_ieee80211_channel_stru st_listen_channel;
    oal_nl80211_channel_type en_listen_channel_type;
    oal_net_device_stru *pst_primary_net_device; /* P2P ���泡������net_device(wlan0) ָ�� */
} mac_p2p_info_stru;

/* ��Ļ״̬ */
typedef enum {
    MAC_SCREEN_OFF = 0,
    MAC_SCREEN_ON = 1,
} mac_screen_state_enum;

typedef struct {
    oal_bool_enum_uint8 en_11k;
    oal_bool_enum_uint8 en_11v;
    oal_bool_enum_uint8 en_11r;
    oal_uint8 auc_rsv[1];
} mac_device_voe_custom_stru;

typedef struct {
    oal_uint16 us_chload_20m;
    oal_uint16 us_chload_40m;
    oal_uint16 us_chload_80m;
    oal_uint16 us_reserve;
} mac_rom_device_stru;

/* device�ṹ�� */
typedef struct {
    oal_uint32 ul_core_id;
    oal_uint8 auc_vap_id[WLAN_SERVICE_VAP_MAX_NUM_PER_DEVICE]; /* device�µ�ҵ��vap���˴�ֻ��¼VAP ID */
    oal_uint8 uc_cfg_vap_id;               /* ����vap ID */
    oal_uint8 uc_device_id;                /* оƬID */
    oal_uint8 uc_chip_id;                  /* �豸ID */
    oal_uint8 uc_device_reset_in_progress; /* ��λ������ */

    oal_bool_enum_uint8 en_device_state; /* ��ʶ�Ƿ��Ѿ������䣬(OAL_TRUE��ʼ����ɣ�OAL_FALSEδ��ʼ�� ) */
    oal_uint8 uc_vap_num;                /* ��ǰdevice�µ�ҵ��VAP����(AP+STA) */
    oal_uint8 uc_sta_num;                /* ��ǰdevice�µ�STA���� */
    mac_p2p_info_stru st_p2p_info; /* P2P �����Ϣ */
    oal_uint8 auc_hw_addr[WLAN_MAC_ADDR_LEN]; /* ��eeprom��flash��õ�mac��ַ��ko����ʱ����hal�ӿڸ�ֵ */
    /* device������� */
    oal_uint8 uc_max_channel;                 /* ������VAP���ŵ��ţ�����VAP����ֵ�������ֵì�ܣ����ڷ�DBACʱʹ�� */
    wlan_channel_band_enum_uint8 en_max_band; /* ������VAP��Ƶ�Σ�����VAP����ֵ�������ֵì�ܣ����ڷ�DBACʱʹ�� */

    wlan_channel_bandwidth_enum_uint8 en_max_bandwidth; /* ������VAP���������ֵ������VAP����ֵ�������ֵì�ܣ����ڷ�DBACʱʹ�� */
    oal_uint8 uc_tx_chain;                              /* ����ͨ�� */
    oal_uint8 uc_rx_chain;                              /* ����ͨ�� */
    wlan_nss_enum_uint8 en_nss_num;                     /* Nss �ռ������� */

    oal_bool_enum_uint8 en_wmm; /* wmmʹ�ܿ��� */
    wlan_tidno_enum_uint8 en_tid;
    oal_uint8 en_reset_switch; /* �Ƿ�ʹ�ܸ�λ���� */
    oal_uint8 uc_csa_vap_cnt;  /* ÿ��running AP����һ��CSA֡���ü�����1���������APֹͣ��ǰӲ�����ͣ�׼����ʼ�л��ŵ� */

    hal_to_dmac_device_stru *pst_device_stru; /* Ӳmac�ṹָ�룬HAL�ṩ�������߼�������device�Ķ�Ӧ */

    oal_uint32 ul_beacon_interval;   /* device����beacon interval,device������VAPԼ��Ϊͬһֵ */
    oal_uint32 ul_duty_ratio;        /* ռ�ձ�ͳ�� */
    oal_uint32 ul_duty_ratio_lp;     /* ����͹���ǰ����ռ�ձ� */
    oal_uint32 ul_rx_nondir_duty_lp; /* ����͹���ǰ����non-direct����ռ�ձ� */
    oal_uint32 ul_rx_dir_duty_lp;    /* ����͹���ǰ����direct����ռ�ձ� */

    /* device���� */
    wlan_protocol_cap_enum_uint8 en_protocol_cap; /* Э������ */
    wlan_band_cap_enum_uint8 en_band_cap;         /* Ƶ������ */
    wlan_bw_cap_enum_uint8 en_bandwidth_cap;      /* �������� */
    oal_uint8 bit_ldpc_coding : 1,                /* �Ƿ�֧�ֽ���LDPC����İ� */
              bit_tx_stbc : 1,                          /* �Ƿ�֧������2x1 STBC���� */
              bit_rx_stbc : 3,                          /* �Ƿ�֧��stbc���� */
              bit_su_bfmer : 1,                         /* �Ƿ�֧�ֵ��û�beamformer */
              bit_su_bfmee : 1,                         /* �Ƿ�֧�ֵ��û�beamformee */
              bit_mu_bfmee : 1;                         /* �Ƿ�֧�ֶ��û�beamformee */
    oal_uint8 bit_nb_is_supp : 1,                 /* �Ƿ�֧��խ�� */
              bit_reserve : 7;

    oal_uint16 us_device_reset_num; /* ��λ�Ĵ���ͳ�� */

    mac_data_rate_stru st_mac_rates_11g[MAC_DATARATES_PHY_80211G_NUM]; /* 11g���� */

    mac_pno_sched_scan_mgmt_stru *pst_pno_sched_scan_mgmt; /* pno����ɨ������ṹ��ָ�룬�ڴ涯̬���룬�Ӷ���ʡ�ڴ� */
    mac_scan_req_stru st_scan_params;                      /* ����һ�ε�ɨ�������Ϣ */
    frw_timeout_stru st_scan_timer;                        /* ɨ�趨ʱ�������л��ŵ� */
    frw_timeout_stru st_obss_scan_timer;                   /* obssɨ�趨ʱ����ѭ����ʱ�� */
    mac_channel_stru st_p2p_vap_channel;                   /* p2p listenʱ��¼p2p���ŵ�������p2p listen������ָ� */

    oal_uint8 uc_active_user_cnt;          /* ��Ծ�û��� */
    oal_uint8 uc_asoc_user_cnt;            /* �����û��� */
    oal_bool_enum_uint8 en_2040bss_switch; /* 20/40 bss��⿪�� */
    oal_uint8 uc_in_suspend;
#ifdef _PRE_WLAN_FEATURE_11K
    // frw_timeout_stru                    st_backoff_meas_timer;
#endif
    oal_uint8 uc_mac_vap_id;             /* ��vap����ʱ������˯��ǰ��mac vap id */
    mac_bss_id_list_stru st_bss_id_list; /* ��ǰ�ŵ��µ�ɨ���� */

    oal_bool_enum_uint8 en_dbac_enabled;
    oal_bool_enum_uint8 en_dbac_running;       /* DBAC�Ƿ������� */
    oal_bool_enum_uint8 en_dbac_has_vip_frame; /* ���DBAC����ʱ�յ��˹ؼ�֡ */
    oal_uint8 uc_arpoffload_switch;
    oal_uint8 uc_wapi;
    oal_uint8 uc_reserve;
    oal_bool_enum_uint8 en_is_random_mac_addr_scan; /* ���macɨ�迪��,��hmac�·� */
    oal_uint8 auc_mac_oui[WLAN_RANDOM_MAC_OUI_LEN]; /* ���mac��ַOUI,��ϵͳ�·� */
    oal_bool_enum_uint8 en_apf_switch;
    oal_uint8 auc_rsv[1];

    /* ���Device�ĳ�Ա�����ƶ���dmac_device */
#if IS_DEVICE
    oal_uint8 auc_resv12[2];
    oal_uint16 us_total_mpdu_num;                                /* device������TID���ܹ���mpdu_num���� */
    oal_uint16 aus_ac_mpdu_num[WLAN_WME_AC_BUTT];                /* device�¸���AC��Ӧ��mpdu_num�� */
    oal_uint16 aus_vap_mpdu_num[WLAN_VAP_SUPPORT_MAX_NUM_LIMIT]; /* ͳ�Ƹ���vap��Ӧ��mpdu_num�� */

    oal_void *p_alg_priv; /* �㷨˽�нṹ�� */
    oal_uint32 ul_first_timestamp; /* ��¼����ͳ�Ƶ�һ��ʱ��� */

    oal_uint8 auc_tx_ba_index_table[DMAC_TX_BA_LUT_BMAP_LEN]; /* ���Ͷ�LUT�� */

    /* ɨ����س�Ա���� */
    oal_uint32 ul_scan_timestamp;                 /* ��¼����һ��ɨ�迪ʼ��ʱ�� */
    oal_uint8 uc_scan_chan_idx;                   /* ��ǰɨ���ŵ����� */
    mac_scan_state_enum_uint8 en_curr_scan_state; /* ��ǰɨ��״̬�����ݴ�״̬����obssɨ���host���·���ɨ�������Լ�ɨ�������ϱ����� */

    oal_uint8 uc_resume_qempty_flag; /* ʹ�ָܻ�qempty��ʶ, Ĭ�ϲ�ʹ�� */
    oal_uint8 uc_scan_count;

    mac_channel_stru st_home_channel; /* ��¼�����ŵ� ���л�ʱʹ�� */
    mac_fcs_cfg_stru st_fcs_cfg;      /* �������ŵ��ṹ�� */

    mac_scan_chan_stats_stru st_chan_result; /* dmacɨ��ʱ һ���ŵ����ŵ�������¼ */

    oal_uint8 auc_original_mac_addr[WLAN_MAC_ADDR_LEN]; /* ɨ�迪ʼǰ����ԭʼ��MAC��ַ */
    oal_uint8 uc_scan_ap_num_in_2p4;
    oal_bool_enum_uint8 en_scan_curr_chan_find_bss_flag; /* ���ŵ�ɨ���Ƿ�ɨ�赽BSS */

    /* �û���س�Ա���� */
    frw_timeout_stru st_active_user_timer; /* �û���Ծ��ʱ�� */

    oal_uint8 auc_ra_lut_index_table[WLAN_ACTIVE_USER_IDX_BMAP_LEN]; /* lut��λͼ */

    mac_fcs_mgr_stru st_fcs_mgr;

    oal_uint8 uc_csa_cnt; /* ÿ��AP����һ��CSA֡���ü�����1��AP�л����ŵ��󣬸ü������� */

    oal_bool_enum_uint8 en_txop_enable; /* �����޾ۺ�ʱ����TXOPģʽ */
    oal_uint8 uc_tx_ba_num;             /* ���ͷ���BA�Ự���� */
    oal_uint8 auc_resv[1];

    frw_timeout_stru st_keepalive_timer; /* keepalive��ʱ�� */

#ifdef _PRE_DEBUG_MODE
    frw_timeout_stru st_exception_report_timer;
#endif
    oal_uint32 aul_mac_err_cnt[HAL_MAC_ERROR_TYPE_BUTT]; /* mac ��������� */
#ifdef _PRE_WLAN_FEATURE_STA_PM
    hal_mac_key_statis_info_stru st_mac_key_statis_info; /* mac�ؼ�ͳ����Ϣ */
#endif

#endif /* IS_DEVICE */

    /* ���Host�ĳ�Ա�����ƶ���hmac_device */
#if IS_HOST
    /* linux�ں��е�device������Ϣ */
    oal_wiphy_stru *pst_wiphy; /* ���ڴ�ź�VAP��ص�wiphy�豸��Ϣ����AP/STAģʽ�¾�Ҫʹ�ã����Զ��VAP��Ӧһ��wiphy */

#ifndef _PRE_WLAN_FEATURE_AMPDU_VAP
    oal_uint8 uc_rx_ba_session_num; /* ��device�£�rx BA�Ự����Ŀ */
    oal_uint8 uc_tx_ba_session_num; /* ��device�£�tx BA�Ự����Ŀ */
    oal_uint8 auc_resv11[2];
#endif
    oal_bool_enum_uint8 en_vap_classify; /* �Ƿ�ʹ�ܻ���vap��ҵ����� */
    oal_uint8 uc_auth_req_sendst;
    oal_uint8 uc_asoc_req_sendst;
    oal_bool_enum_uint8 en_report_mgmt_req_status;

    oal_uint8 auc_rx_ba_lut_idx_table[DMAC_BA_LUT_IDX_BMAP_LEN]; /* ���ն�LUT�� */

    frw_timeout_stru st_obss_aging_timer; /* OBSS�����ϻ���ʱ�� */

    mac_ap_ch_info_stru st_ap_channel_list[MAC_MAX_SUPP_CHANNEL];
    oal_uint8 uc_ap_chan_idx;                        /* ��ǰɨ���ŵ����� */
    oal_bool_enum_uint8 en_fft_window_offset_enable; /* �Ƿ��Ѿ�����FFT���� */
    oal_uint8 auc_resv21[2];

    oal_bool_enum_uint8 en_40MHz_intol_bit_recd;
#endif /* IS_HOST */
#ifdef _PRE_WLAN_FEATURE_FTM
    oal_uint8 uc_ftm_vap_id; /* ftm�ж϶�Ӧ vap ID */
    oal_uint8 en_nbfh_running;
    oal_uint8 en_nbfh_enabled;
#else
    oal_uint8 auc_resv4[1];
    oal_uint8 en_nbfh_running;
    oal_uint8 en_nbfh_enabled;
#endif
    /* ROM������Դ��չָ�� */
    mac_rom_device_stru *pst_mac_device_rom;
} mac_device_stru;

#pragma pack(push, 1)
/* �ϱ���ɨ��������չ��Ϣ�������ϱ�host��Ĺ���֡netbuf�ĺ��� */
typedef struct {
    oal_int32 l_rssi;                                /* �ź�ǿ�� */
    wlan_mib_desired_bsstype_enum_uint8 en_bss_type; /* ɨ�赽��bss���� */
    oal_uint8 auc_resv[3];                           /* Ԥ���ֶ� */
} mac_scanned_result_extend_info_stru;
#pragma pack(pop)

/* chip�ṹ�� */
typedef struct {
    oal_uint8 auc_device_id[WLAN_DEVICE_MAX_NUM_PER_CHIP]; /* CHIP�¹ҵ�DEV������¼��Ӧ��ID����ֵ */
    oal_uint8 uc_device_nums;                              /* chip��device����Ŀ */
    oal_uint8 uc_chip_id;                                  /* оƬID */
    oal_bool_enum_uint8 en_chip_state;                     /* ��ʶ�Ƿ��ѳ�ʼ����OAL_TRUE��ʼ����ɣ�OAL_FALSEδ��ʼ�� */
    oal_uint32 ul_chip_ver;                                /* оƬ�汾 */
    hal_to_dmac_chip_stru *pst_chip_stru;                  /* Ӳmac�ṹָ�룬HAL�ṩ�������߼�������chip�Ķ�Ӧ */
} mac_chip_stru;

#ifdef _PRE_WLAN_FEATURE_IP_FILTER
typedef enum {
    MAC_RX_IP_FILTER_STOPED = 0,   // ���ܹرգ�δʹ�ܡ���������״�����������˶�����
    MAC_RX_IP_FILTER_WORKING = 1,  // ���ܴ򿪣����չ�����������
    MAC_RX_IP_FILTER_BUTT
} mac_ip_filter_state_enum;
typedef oal_uint8 mac_ip_filter_state_enum_uint8;

typedef struct {
    mac_ip_filter_state_enum_uint8 en_state;  // ����״̬�����ˡ��ǹ��˵�
    oal_uint8 uc_btable_items_num;            // ��������Ŀǰ�洢��items����
    oal_uint8 uc_btable_size;                 // ��������С����ʾ���洢��items����
    oal_uint8 uc_resv;
    mac_ip_filter_item_stru *pst_filter_btable;  // ������ָ��
} mac_rx_ip_filter_struc;
#endif  // _PRE_WLAN_FEATURE_IP_FILTER

/* board�ṹ�� */
typedef struct {
    mac_chip_stru ast_chip[WLAN_CHIP_MAX_NUM_PER_BOARD]; /* board�ҽӵ�оƬ */
    oal_uint8 uc_chip_id_bitmap;                         /* ��ʶchip�Ƿ񱻷����λͼ */
    oal_uint8 auc_resv[3];                               /* �ֽڶ��� */
#ifdef _PRE_WLAN_FEATURE_IP_FILTER
    mac_rx_ip_filter_struc st_rx_ip_filter; /* rx ip���˹��ܵĹ����ṹ�� */
#endif                                      // _PRE_WLAN_FEATURE_IP_FILTER
} mac_board_stru;

typedef struct {
    mac_device_stru *pst_mac_device;
} mac_wiphy_priv_stru;

typedef struct {
    wlan_csa_mode_tx_enum_uint8 en_mode;
    oal_uint8 uc_channel;
    oal_uint8 uc_cnt;
    wlan_channel_bandwidth_enum_uint8 en_bandwidth;
    mac_csa_flag_enum_uint8 en_debug_flag; /* 0:�������ŵ�; 1:��beacon֡�к���csa,�ŵ����л�;2:ȡ��beacon֡�к���csa */
    oal_uint8 auc_reserv[3];
} mac_csa_debug_stru;

/* �������Կ�����صĽṹ�� */
typedef struct {
    oal_uint32 ul_cmd_bit_map;
    oal_bool_enum_uint8 en_band_force_switch; /* �ı�������� */
    oal_uint8 auc_resv[3];
    mac_csa_debug_stru st_csa_debug;
} mac_protocol_debug_switch_stru;

typedef oal_rom_cb_result_enum_uint8 (*mac_device_init_rom_cb)(mac_device_stru *pst_mac_device,
                                                               oal_uint32 ul_chip_ver,
                                                               oal_uint8 uc_chip_id,
                                                               oal_uint8 uc_device_id,
                                                               oal_uint32 *pul_cb_ret);
typedef wlan_bw_cap_enum_uint8 (*mac_device_max_band_rom_cb)(oal_void);

typedef struct {
    mac_device_init_rom_cb p_device_init_cb;
    mac_device_max_band_rom_cb p_device_max_band_cb;
} mac_device_rom_stru;

typedef enum {
    MAC_PSM_QUERY_BEACON_CNT,
    MAC_PSM_QUERY_FLT_STAT,
    MAC_PSM_QUERY_TYPE_BUTT
}mac_psm_query_type_enum;
typedef oal_uint8 mac_psm_query_type_enum_uint8;
#define MAC_PSM_QUERY_MSG_MAX_STAT_ITEM 10
typedef struct {
    oal_uint32                          ul_query_item;
    oal_uint32                          aul_val[MAC_PSM_QUERY_MSG_MAX_STAT_ITEM];
}mac_psm_query_stat_stru;

typedef struct {
    mac_psm_query_type_enum_uint8 en_query_type;
    oal_uint8 auc_resv[3]; /* 3��pad���� */
    mac_psm_query_stat_stru st_stat;
}mac_psm_query_msg;

typedef struct {
    oal_uint32                          ul_ao_drop_cnt;
    oal_uint32                          ul_ao_send_rsp_cnt;
    oal_uint32                          ul_apf_flt_drop_cnt;
    oal_uint32                          ul_icmp_flt_drop_cnt;
}mac_psm_flt_stat_stru;

/*****************************************************************************
  8 UNION����
*****************************************************************************/
/*****************************************************************************
  9 OTHERS����
*****************************************************************************/
extern oal_uint32 mac_get_band_5g_enabled(oal_void);
extern oal_void mac_set_band_5g_enabled(oal_uint32 band_5g_enabled);
/* ���߼��в��뿴���� */
#ifdef _PRE_WLAN_FEATURE_DBAC
#define MAC_DBAC_ENABLE(_pst_device) (_pst_device->en_dbac_enabled == OAL_TRUE)
#else
#define MAC_DBAC_ENABLE(_pst_device) (OAL_FALSE)
#endif
#ifdef _PRE_WLAN_FEATURE_WMMAC
extern oal_bool_enum_uint8 g_en_wmmac_switch;
#endif


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

#ifdef _PRE_WLAN_FEATURE_DBAC

OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_need_enqueue_tid_for_dbac(mac_device_stru *pst_device,
                                                                        mac_vap_stru *pst_vap)
{
    return (oal_bool_enum_uint8)((pst_device->en_dbac_enabled == OAL_TRUE) &&
                                 (pst_vap->en_vap_state == MAC_VAP_STATE_PAUSE));
}
#endif
#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_get_2040bss_switch(mac_device_stru *pst_mac_device)
{
    return pst_mac_device->en_2040bss_switch;
}
OAL_STATIC OAL_INLINE oal_void mac_set_2040bss_switch(mac_device_stru *pst_mac_device,
                                                      oal_bool_enum_uint8 en_switch)
{
    pst_mac_device->en_2040bss_switch = en_switch;
}
#endif

#if IS_DEVICE
OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_device_is_scaning(mac_device_stru *pst_mac_device)
{
    return (oal_bool_enum_uint8)(pst_mac_device->en_curr_scan_state == MAC_SCAN_STATE_RUNNING);
}

OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_device_is_listening(mac_device_stru *pst_mac_device)
{
    return (oal_bool_enum_uint8)((pst_mac_device->en_curr_scan_state == MAC_SCAN_STATE_RUNNING) &&
            (pst_mac_device->st_scan_params.uc_scan_func & MAC_SCAN_FUNC_P2P_LISTEN));
}
#endif /* IS_DEVICE */

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern mac_device_voe_custom_stru *mac_get_voe_custom_param_addr(oal_void);
/*****************************************************************************
  10.1 �����ṹ���ʼ����ɾ��
*****************************************************************************/
extern oal_uint32 mac_device_init(mac_device_stru *pst_mac_device, oal_uint32 ul_chip_ver, oal_uint8 chip_id,
                                  oal_uint8 uc_device_id);
extern oal_uint32 mac_chip_init(mac_chip_stru *pst_chip, oal_uint8 uc_chip_id);
extern oal_uint32 mac_board_init(mac_board_stru *pst_board);

extern oal_uint32 mac_device_exit(mac_device_stru *pst_device);
extern oal_uint32 mac_chip_exit(mac_board_stru *pst_board, mac_chip_stru *pst_chip);
extern oal_uint32 mac_board_exit(mac_board_stru *pst_board);

/*****************************************************************************
  10.2 ������Ա���ʲ���
*****************************************************************************/
extern oal_void mac_device_set_vap_id(mac_device_stru *pst_mac_device, mac_vap_stru *pst_mac_vap,
                                      oal_uint8 uc_vap_idx, wlan_vap_mode_enum_uint8 en_vap_mode,
                                      wlan_p2p_mode_enum_uint8 en_p2p_mode,
                                      oal_uint8 is_add_vap);
extern oal_void mac_device_set_dfr_reset(mac_device_stru *pst_mac_device, oal_uint8 uc_device_reset_in_progress);
extern oal_void mac_device_set_state(mac_device_stru *pst_mac_device, oal_uint8 en_device_state);

extern oal_void mac_device_set_channel(mac_device_stru *pst_mac_device,
                                       mac_cfg_channel_param_stru *pst_channel_param);
extern oal_void mac_device_get_channel(mac_device_stru *pst_mac_device,
                                       mac_cfg_channel_param_stru *pst_channel_param);

extern oal_void mac_device_set_txchain(mac_device_stru *pst_mac_device, oal_uint8 uc_tx_chain);
extern oal_void mac_device_set_rxchain(mac_device_stru *pst_mac_device, oal_uint8 uc_rx_chain);
extern oal_void mac_device_set_beacon_interval(mac_device_stru *pst_mac_device, oal_uint32 ul_beacon_interval);
extern oal_void mac_device_inc_active_user(mac_device_stru *pst_mac_device);

extern oal_void mac_device_dec_active_user(mac_device_stru *pst_mac_device);

extern oal_void *mac_device_get_all_rates(mac_device_stru *pst_dev);
/*****************************************************************************
  10.3 ���������
*****************************************************************************/
extern oal_uint32 mac_device_find_legacy_sta(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap);
extern oal_uint32 mac_device_find_up_vap(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap);
extern mac_vap_stru *mac_device_find_another_up_vap(mac_device_stru *pst_mac_device, oal_uint8 uc_vap_id_self);
extern oal_uint32 mac_device_find_up_ap(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap);
extern oal_uint8 mac_device_calc_up_vap_num(mac_device_stru *pst_mac_device);
extern oal_uint32 mac_device_calc_work_vap_num(mac_device_stru *pst_mac_device);
extern oal_uint32 mac_device_find_up_p2p_go(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap);
extern oal_uint32 mac_device_find_2up_vap(mac_device_stru *pst_mac_device,
                                          mac_vap_stru **ppst_mac_vap1,
                                          mac_vap_stru **ppst_mac_vap2);
extern oal_uint32 mac_fcs_dbac_state_check(mac_device_stru *pst_mac_device);
extern oal_uint32 mac_device_find_up_sta(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap);

extern oal_uint32 mac_device_is_p2p_connected(mac_device_stru *pst_mac_device);
/*****************************************************************************
  10.4 ���Ƴ�
*****************************************************************************/
/*****************************************************************************
  11 inline��������
*****************************************************************************/

OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_is_hide_ssid(oal_uint8 *puc_ssid_ie, oal_uint8 uc_ssid_len)
{
    return (oal_bool_enum_uint8)((puc_ssid_ie == OAL_PTR_NULL) || (uc_ssid_len == 0) || (puc_ssid_ie[0] == '\0'));
}


OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_device_is_auto_chan_sel_enabled(mac_device_stru *pst_mac_device)
{
    /* BSS����ʱ������û�û�������ŵ�����Ĭ��Ϊ�����Զ��ŵ�ѡ�� */
    return (!pst_mac_device->uc_max_channel);
}
extern hal_fcs_protect_type_enum_uint8 mac_fcs_get_protect_type(mac_vap_stru *pst_mac_vap);
extern oal_uint32 mac_fcs_init(mac_fcs_mgr_stru *pst_fcs_mgr,
                               oal_uint8 uc_chip_id,
                               oal_uint8 uc_device_id);

extern mac_fcs_err_enum_uint8 mac_fcs_request(mac_fcs_mgr_stru *pst_fcs_mgr,
                                              mac_fcs_state_enum_uint8 *puc_state,
                                              mac_fcs_cfg_stru *pst_fcs_cfg);

extern void mac_fcs_release(mac_fcs_mgr_stru *pst_fcs_mgr);

extern mac_fcs_err_enum_uint8 mac_fcs_start(mac_fcs_mgr_stru *pst_fcs_mgr,
                                            mac_fcs_cfg_stru *pst_fcs_cfg,
                                            hal_one_packet_status_stru *pst_status,
                                            oal_uint8 uc_fake_tx_q_id);
extern oal_void mac_fcs_set_one_pkt_timeout_time(mac_fcs_mgr_stru *pst_fcs_mgr);

extern mac_fcs_err_enum_uint8 mac_fcs_start_enhanced(mac_fcs_mgr_stru *pst_fcs_mgr,
                                                     mac_fcs_cfg_stru *pst_fcs_cfg);
extern oal_void mac_fcs_send_one_packet_start(mac_fcs_mgr_stru *pst_fcs_mgr,
                                              hal_one_packet_cfg_stru *pst_one_packet_cfg,
                                              hal_to_dmac_device_stru *pst_device,
                                              hal_one_packet_status_stru *pst_status,
                                              oal_bool_enum_uint8 en_ps);
extern oal_uint32 mac_fcs_notify_chain_register(mac_fcs_mgr_stru *pst_fcs_mgr,
                                                mac_fcs_notify_type_enum_uint8 uc_notify_type,
                                                mac_fcs_hook_id_enum_uint8 en_hook_id,
                                                mac_fcs_notify_func p_func);

extern oal_uint32 mac_fcs_notify(mac_fcs_mgr_stru *pst_fcs_mgr,
                                 mac_fcs_notify_type_enum_uint8 uc_notify_type);

extern oal_uint32 mac_fcs_notify_chain_unregister(mac_fcs_mgr_stru *pst_fcs_mgr,
                                                  mac_fcs_notify_type_enum_uint8 uc_notify_type,
                                                  mac_fcs_hook_id_enum_uint8 en_hook_id);

extern oal_uint32 mac_fcs_notify_chain_destroy(mac_fcs_mgr_stru *pst_fcs_mgr);

extern oal_uint32 mac_fcs_get_prot_mode(mac_vap_stru *pst_src_vap);
extern oal_uint32 mac_fcs_get_prot_datarate(mac_vap_stru *pst_src_vap);
extern oal_void mac_fcs_prepare_one_packet_cfg(mac_vap_stru *pst_mac_vap,
                                               hal_one_packet_cfg_stru *pst_one_packet_cfg,
                                               oal_uint16 us_protect_time,
                                               mac_one_packet_index_enum_uint8 uc_one_packet_index);

extern oal_void mac_fcs_flush_event_by_channel(mac_device_stru *pst_mac_device, mac_channel_stru *pst_chl);

extern oal_uint32 mac_fcs_set_channel(mac_device_stru *pst_mac_device,
                                      mac_channel_stru *pst_channel);
extern mac_fcs_err_enum_uint8 mac_fcs_start_same_channel(mac_fcs_mgr_stru *pst_fcs_mgr,
                                                         mac_fcs_cfg_stru *pst_fcs_cfg,
                                                         hal_one_packet_status_stru *pst_status,
                                                         oal_uint8 uc_fake_tx_q_id);

extern mac_fcs_err_enum_uint8 mac_fcs_start_enhanced_same_channel(mac_fcs_mgr_stru *pst_fcs_mgr,
                                                                  mac_fcs_cfg_stru *pst_fcs_cfg);

extern oal_rom_cb_result_enum_uint8 mac_device_init_cb(mac_device_stru *pst_mac_device,
                                                       oal_uint32 ul_chip_ver,
                                                       oal_uint8 uc_chip_id,
                                                       oal_uint8 uc_device_id,
                                                       oal_uint32 *pul_cb_ret);
extern wlan_bw_cap_enum_uint8 mac_device_max_band_cb(oal_void);
extern oal_uint32 mac_fcs_wait_one_packet_done_same_channel(mac_fcs_mgr_stru *pst_fcs_mgr);
extern oal_void mac_fcs_send_one_packet_start_same_channel(mac_fcs_mgr_stru *pst_fcs_mgr,
                                                           hal_one_packet_cfg_stru *pst_one_packet_cfg,
                                                           hal_to_dmac_device_stru *pst_device,
                                                           hal_one_packet_status_stru *pst_status,
                                                           oal_bool_enum_uint8 en_ps);


OAL_STATIC OAL_INLINE oal_bool_enum_uint8 mac_fcs_is_same_channel(mac_channel_stru *pst_channel_dst,
                                                                  mac_channel_stru *pst_channel_src)
{
    return (oal_bool_enum_uint8)(pst_channel_dst->uc_chan_number == pst_channel_src->uc_chan_number);
}


OAL_STATIC OAL_INLINE oal_uint8 mac_fcs_get_protect_cnt(mac_vap_stru *pst_mac_vap)
{
    if (pst_mac_vap->en_vap_mode == WLAN_VAP_MODE_BSS_AP) {
        return HAL_FCS_PROTECT_CNT_1;
    }

    return HAL_FCS_PROTECT_CNT_3;
}

#if (_PRE_TEST_MODE_BOARD_ST == _PRE_TEST_MODE)
extern oal_void mac_fcs_verify_init(oal_void);
extern oal_void mac_fcs_verify_start(oal_void);
extern oal_void mac_fcs_verify_timestamp(mac_fcs_stage_enum_uint8 en_stage);
extern oal_void mac_fcs_verify_stop(oal_void);

#else
#define mac_fcs_verify_init()
#define mac_fcs_verify_start()
#define mac_fcs_verify_timestamp(a)
#define mac_fcs_verify_stop()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of mac_device.h */