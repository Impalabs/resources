

#ifndef __HMAC_ROAM_MAIN_H__
#define __HMAC_ROAM_MAIN_H__

#include "hmac_vap.h"
#include "mac_vap.h"
#include "mac_device.h"
#include "mac_vap_common.h"
#include "oal_util.h"
#include "oam_wdk.h"
#include "hmac_device.h"
#include "hmac_11k.h"

// �˴�����extern "C" UT���벻��
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_ROAM_MAIN_H

#ifdef _PRE_WINDOWS_SUPPORT
#define ROAM_SCAN_TIME_MAX    (10 * 1000)  /* ɨ�賬ʱʱ�� ��λms */
#define ROAM_TASK_INDICATE_TIME_MAX (5 * 1000) /* ����ָʾ��ʱ�¼� ��λms */
#else
#define ROAM_SCAN_TIME_MAX    (3 * 1000)  /* ɨ�賬ʱʱ�� ��λms */
#endif
#define ROAM_CONNECT_TIME_MAX (10 * 1000) /* ������ʱʱ�� ��λms */
#define ROAM_INVALID_SCAN_MAX (5)         /* ������Чɨ������   */

/* ��ʱʱ��150ms-->100ms, ʹNR Resp��ʱ�����׸��� */
#define ROAM_NEIGHBOR_PROCESS_TIME_MAX  (150)      /* ɨ�賬ʱʱ�� ��λms */

/* ���γ���ʶ��һ��ʶ����ܼ�AP�����͵���AP���� */
#define WLAN_FULL_CHANNEL_NUM           (20)  /* �ж�ȫ�ŵ�ɨ�����Ŀ���� */
#define ROAM_ENV_CANDIDATE_GOOD_NUM     (5)   /* ͳ�����λ�����ǿ�ź�ǿ��AP�ĸ��� */
#define ROAM_ENV_CANDIDATE_WEAK_NUM     (10)  /* ͳ�����λ��������ź�ǿ��AP�ĸ��� */
#define ROAM_ENV_RSSI_NE60_DB           (-60) /* ���λ�����ǿ�ź�ǿ��AP RSSI������ */
#define ROAM_ENV_DENSE_TO_SPARSE_PERIOD (5)   /* �Ӹ��ܳ�������ܳ����л���ɨ������ */

/* ���γ���ʶ�����ʶ����������������ޡ������и����ź�ǿ��AP�ĳ��� */
#define ROAM_ENV_BETTER_RSSI_PERIOD      (4)  /* �������������ޣ������и����ź�ǿ��AP��ɨ������ */
#define ROAM_ENV_BETTER_RSSI_NULL_PERIOD (1)  /* �������������ޣ������и����ź�ǿ��AP��ɨ������ɨ��������AP������ */
#define ROAM_ENV_BETTER_RSSI_DISTANSE    (30) /* ͬһ��ESS����ǿ�ź�ǿ��AP�͵�ǰ����AP��RSSI��� */

/* ����˫����TH1 threshold */
#define ROAM_ENV_RSSI_THRESHOLD1_2G      (-70)
#define ROAM_ENV_RSSI_THRESHOLD1_5G      (-68)

#define ROAM_FAIL_FIVE_TIMES (100)

/* ���δ������� */
typedef enum {
    ROAM_TRIGGER_DMAC = 0,
    ROAM_TRIGGER_APP = 1,
    ROAM_TRIGGER_COEX = 2,
    ROAM_TRIGGER_11V = 3,
    ROAM_TRIGGER_M2S = 4,
    ROAM_TRIGGER_BSSID = 5, /* ָ��BSSID���� */
    ROAM_TRIGGER_HOME_NETWORK = 6,
    ROAM_TRIGGER_ARP_PROBE_FAIL_REASSOC = 7,
    ROAM_TRIGGER_D2H_REASSOC = 8,

    ROAM_TRIGGER_BUTT
} roam_trigger_enum;

/* DMAC���������γ������� */
typedef enum {
    ROAM_ENV_SPARSE_AP_TH2 = 0, /* default full channels scan for roaming, based on [-78, -74] RSSI threshold */
    ROAM_ENV_LINKLOSS = 1,
    ROAM_ENV_DENSE_AP = 2, /* dense AP roaming environment, based on [-72, -70] RSSI threshold */
    ROAM_ENV_SPARSE_AP_TH1 = 3, /* specific channels scan for roaming, based on [-70, -68] RSSI threshold */

    ROAM_ENV_BUTT
} roam_scenario_enum;

/* ������״̬��״̬ */
typedef enum {
    ROAM_MAIN_STATE_INIT = 0,
    ROAM_MAIN_STATE_FAIL = ROAM_MAIN_STATE_INIT,
    ROAM_MAIN_STATE_SCANING = 1,
    ROAM_MAIN_STATE_CONNECTING = 2,
    ROAM_MAIN_STATE_UP = 3,
    ROAM_MAIN_STATE_NEIGHBOR_PROCESS = 4,

    ROAM_MAIN_STATE_BUTT
} roam_main_state_enum;

/* ������״̬���¼����� */
typedef enum {
    ROAM_MAIN_FSM_EVENT_START = 0,
    ROAM_MAIN_FSM_EVENT_SCAN_RESULT = 1,
    ROAM_MAIN_FSM_EVENT_START_CONNECT = 2,
    ROAM_MAIN_FSM_EVENT_CONNECT_FAIL = 3,
    ROAM_MAIN_FSM_EVENT_HANDSHAKE_FAIL = 4,
    ROAM_MAIN_FSM_EVENT_CONNECT_SUCC = 5,
    ROAM_MAIN_FSM_EVENT_TIMEOUT = 6,
    ROAM_MAIN_FSM_EVENT_TYPE_BUTT
} roam_main_fsm_event_type_enum;

/* ����ɨ���ŵ��������Բ���,�����д��� */
typedef enum {
    ROAM_SCAN_CHANNEL_ORG_0 = 0, /* no scan */
    ROAM_SCAN_CHANNEL_ORG_1 = 1, /* scan only one channel */
    ROAM_SCAN_CHANNEL_ORG_3 = 2, /* 2.4G channel 1\6\11 */
    ROAM_SCAN_CHANNEL_ORG_4 = 3, /* 2.4G channel 1\5\9\13 */
    ROAM_SCAN_CHANNEL_ORG_DBDC = 4, /* scan p2p channel and the other band channels */
    ROAM_SCAN_CHANNEL_ORG_5 = 5, /* scan specific channel for 11v */
    ROAM_SCAN_CHANNEL_ORG_2G = 6, /* scan 2.4G channels only */
    ROAM_SCAN_CHANNEL_ORG_5G = 7, /* scan 5G channels only */
    ROAM_SCAN_CHANNEL_ORG_FULL = 8,  /* scan full channel for CI */
    ROAM_SCAN_CHANNEL_ORG_SPECIFIED = 9, /* scan specified channel */
    ROAM_SCAN_CHANNEL_ORG_BUTT,  /* scan full channel */
} roam_channel_org_enum;

/* �������ýṹ�� */
typedef struct {
    roam_channel_org_enum uc_scan_orthogonal; /* ɨ���ŵ��������� */
    int8_t c_trigger_rssi_2g;                     /* 2Gʱ�Ĵ������� */
    int8_t c_trigger_rssi_5g;                     /* 5Gʱ�Ĵ������� */
    uint8_t uc_delta_rssi_2g;                     /* 2Gʱ���������� */
    uint8_t uc_delta_rssi_5g;                     /* 5Gʱ���������� */
    int8_t c_dense_env_trigger_rssi_2g;           /* �ܼ�AP������2Gʱ�Ĵ������� */
    int8_t c_dense_env_trigger_rssi_5g;           /* �ܼ�AP������5Gʱ�Ĵ������� */
    oal_bool_enum_uint8 uc_scenario_enable;         /* �Ƿ�ʹ�ܳ���ʶ�� */
    int8_t c_candidate_good_rssi;                 /* �ܼ�AP����ʶ��ǿ�ź�ǿ��AP������ */
    uint8_t uc_candidate_good_num;                /* �ܼ�AP����ʶ��ǿ�ź�ǿ��AP����Ŀ */
    uint8_t uc_candidate_weak_num;                /* �ܼ�AP����ʶ�����ź�ǿ��AP����Ŀ */
    uint16_t us_roam_interval;                    /* ��̬�������δ������ */
    int8_t c_trigger_th1_2g;
    int8_t c_trigger_th1_5g;
    uint8_t uc_resv[2];                           /* ����2���ֽ� */
    uint32_t blacklist_expire_sec; /* not used for now */
    uint32_t buffer_max;           /* not used for now */
} hmac_roam_config_stru;

/* ����ͳ�ƽṹ�� */
typedef struct {
    uint32_t trigger_rssi_cnt;     /* rssi��������ɨ����� */
    uint32_t trigger_linkloss_cnt; /* linkloss��������ɨ����� */
    uint32_t scan_cnt;             /* ����ɨ����� */
    uint32_t scan_result_cnt;      /* ����ɨ�践�ش��� */
    uint32_t connect_cnt;          /* �������Ӽ��� */
    uint32_t roam_old_cnt;         /* ����ʧ�ܼ��� */
    uint32_t roam_new_cnt;         /* ���γɹ����� */
    uint32_t roam_scan_fail;
#ifdef _PRE_WLAN_FEATURE_11V_ENABLE
    uint32_t roam_11v_scan_fail;
#endif
    uint32_t roam_eap_fail;
    uint32_t scan_start_timetamp;    /* ����ɨ�迪ʼʱ��� */
    uint32_t scan_end_timetamp;      /* ����ɨ�迪ʼʱ��� */
    uint32_t connect_start_timetamp; /* ���ι�����ʼʱ��� */
    uint32_t connect_end_timetamp;   /* ���ι������ʱ��� */
    uint8_t uc_roam_mode;               /* ����ģʽ */
    uint8_t uc_scan_mode;               /* ɨ��ģʽ */
} hmac_roam_static_stru;

typedef enum {
    WPAS_CONNECT_STATE_INIT = 0,
    WPAS_CONNECT_STATE_START = 1,
    WPAS_CONNECT_STATE_ASSOCIATED = 2,
    WPAS_CONNECT_STATE_HANDSHAKED = 3,
    WPAS_CONNECT_STATE_IPADDR_OBTAINED = 4,
    WPAS_CONNECT_STATE_IPADDR_REMOVED = 5,
    WPAS_CONNECT_STATE_BUTT
} hmac_wpas_connect_state_enum;
typedef uint32_t hmac_wpas_connect_state_enum_uint32;

uint32_t hmac_roam_enable(hmac_vap_stru *hmac_vap, uint8_t enable);
uint32_t hmac_roam_check_signal_bridge(hmac_vap_stru *hmac_vap);
uint32_t hmac_roam_start(hmac_vap_stru *hmac_vap, roam_channel_org_enum scan_type,
    oal_bool_enum_uint8 current_bss_ignore, uint8_t *target_bssid, roam_trigger_enum roam_trigger);
uint32_t hmac_roam_handle_home_network(hmac_vap_stru *hmac_vap);
uint32_t hmac_roam_init(hmac_vap_stru *hmac_vap);
uint32_t hmac_roam_info_init(hmac_vap_stru *hmac_vap);
uint32_t hmac_roam_exit(hmac_vap_stru *hmac_vap);
uint32_t hmac_roam_test(hmac_vap_stru *hmac_vap);
uint32_t hmac_roam_resume_user(hmac_vap_stru *hmac_vap, void *param);
uint32_t hmac_roam_pause_user(hmac_vap_stru *hmac_vap, void *param);
uint32_t hmac_sta_roam_rx_mgmt(hmac_vap_stru *hmac_vap, void *param);
uint32_t hmac_roam_trigger_handle(hmac_vap_stru *hmac_vap, int8_t rssi, oal_bool_enum_uint8 current_bss_ignore);
void hmac_roam_tbtt_handle(hmac_vap_stru *hmac_vap);
uint32_t hmac_roam_scan_complete(hmac_vap_stru *hmac_vap);
void hmac_roam_connect_complete(hmac_vap_stru *hmac_vap, uint32_t result);
uint32_t hmac_roam_connect_ft_ds_change_to_air(hmac_vap_stru *hmac_vap, mac_bss_dscr_stru *param);
void hmac_roam_add_key_done(hmac_vap_stru *hmac_vap);
void hmac_roam_wpas_connect_state_notify(hmac_vap_stru *hmac_vap, hmac_wpas_connect_state_enum_uint32 conn_state);
uint32_t hmac_roam_rssi_trigger_type(hmac_vap_stru *hmac_vap, roam_scenario_enum val);
void hmac_roam_check_psk(hmac_vap_stru *hmac_vap, mac_conn_security_stru *conn_sec);
oal_bool_enum_uint8 hmac_roam_check_dbac_channel(mac_vap_stru *p2p_vap, uint8_t chan_num);
#ifdef _PRE_WLAN_FEATURE_11R
uint32_t hmac_roam_reassoc(hmac_vap_stru *hmac_vap);
#endif
#ifdef _PRE_WLAN_FEATURE_11K
uint32_t hmac_roam_rx_neighbor_response_action(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf);
oal_bool_enum_uint8 hmac_roam_is_neighbor_report_allowed(hmac_vap_stru *hmac_vap);
#endif
void hmac_roam_timeout_test(hmac_vap_stru *hmac_vap);
uint32_t hmac_roam_pri_sta_join_check(hmac_vap_stru *hmac_vap, mac_bss_dscr_stru *new_bss, mac_vap_stru *other_vap);
void hmac_roam_kick_secondary_sta(hmac_vap_stru *hmac_vap, int8_t rssi);

#ifdef _PRE_WINDOWS_SUPPORT
    uint32_t hmac_config_roam_task_indicate(uint8_t flag);
#endif
void hmac_roam_down_dc_p2p_group(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hmac_roam_main.h */
