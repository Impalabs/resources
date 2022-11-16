

#ifndef __HD_COMMAND_H__
#define __HD_COMMAND_H__

#include "wlan_customize.h"

/* Power save modes specified by the user */
typedef enum {
    NO_POWERSAVE = 0,
    MIN_FAST_PS = 1,  /* idle ʱ�����ini�е����� */
    MAX_FAST_PS = 2,  /* idle ʱ�����ini�е����� */
    AUTO_FAST_PS = 3, /* �����㷨�Զ�����idleʱ����ini�е����޻�������ֵ */
    MAX_PSPOLL_PS = 4,
    NUM_PS_MODE = 5
} ps_user_mode_enum;

/* MAC TX RX common report�����Ӧö�� */
typedef enum {
    MAC_TX_COMMON_REPORT_INDEX = 0,
    MAC_TX_COMMON_REPORT_STATUS,
    MAC_TX_COMMON_REPORT_BW_MODE,
    MAC_TX_COMMON_REPORT_PROTOCOL_MODE,
    MAC_TX_COMMON_REPORT_FRAME_TYPE,
    MAC_TX_COMMON_REPORT_SUB_TYPE = 5,
    MAC_TX_COMMON_REPORT_APMDU,
    MAC_TX_COMMON_REPORT_PSDU,
    MAC_TX_COMMON_REPORT_HW_RETRY,
    MAC_TX_COMMON_REPORT_CTRL_REG_CLEAR,

    MAC_TX_COMMON_REPORT_DEBUG_BUTT
} mac_tx_common_report_debug_type_enum;
typedef uint8_t mac_tx_common_report_debug_type_enum_uint8;

typedef enum {
    MAC_RX_COMMON_REPORT_INDEX = 0,
    MAC_RX_COMMON_REPORT_STATUS,
    MAC_RX_COMMON_REPORT_BW_MODE,
    MAC_RX_COMMON_REPORT_PROTOCOL_MODE,
    MAC_RX_COMMON_REPORT_FRAME_TYPE,
    MAC_RX_COMMON_REPORT_SUB_TYPE = 5,
    MAC_RX_COMMON_REPORT_APMDU,
    MAC_RX_COMMON_REPORT_PSDU,
    MAC_RX_COMMON_REPORT_VAP_CHK,
    MAC_RX_COMMON_REPORT_BSS_CHK,
    MAC_RX_COMMON_REPORT_DIRECT_CHK = 10,
    MAC_RX_COMMON_REPORT_CTRL_REG_CLEAR,

    MAC_RX_COMMON_REPORT_DEBUG_BUTT
} mac_rx_common_report_debug_type_enum;
typedef uint8_t mac_rx_common_report_debug_type_enum_uint8;

typedef enum {
    PG_EFFICIENCY_STATISTICS_ENABLE = 0,
    PG_EFFICIENCY_INFO = 1,

    MAC_VAP_COMMON_SET_TXQ = 2,
    MAC_VAP_COMMON_TX_INFO = 3,
#ifdef _PRE_WLAN_FEATURE_MBO
    MBO_SWITCH = 4,                  /* ��������1:MBO���Կ���,out-of-the-box test */
    MBO_CELL_CAP = 5,                /* ��������2:MBO Capability Indication test */
    MBO_ASSOC_DISALLOWED_SWITCH = 6, /* ��������6 APͨ����MBO IEָʾ���������� */
#endif

#ifdef _PRE_WLAN_FEATURE_HIEX
    HIEX_DEV_CAP = 7, /* HIEX device������������ */
    HIEX_DEBUG = 8,   /* HIEX����debug���� */
#endif

    RIFS_ENABLE = 9,            /* rifs����ʹ�� */
    GREENFIELD_ENABLE = 10,     /* GreenField����ʹ�� */
    PROTECTION_LOG_SWITCH = 11, /* ����ģʽά���ӡ */

#ifdef _PRE_WLAN_FEATURE_11AX
    DMAC_RX_FRAME_STATISTICS = 12, /* ����֡��Ϣͳ�� */
    DMAC_TX_FRAME_STATISTICS = 13, /* ����֡��Ϣͳ�� */
#endif
    AUTH_RSP_TIMEOUT = 14, /* ��֤ʱ����auth �׶γ�ʱʱ�� */
    FORBIT_OPEN_AUTH = 15, /* ��ֹopen���� */
#ifdef _PRE_WLAN_RX_LISTEN_POWER_SAVING
    HAL_PS_DEBUG_SWITCH = 16,
#endif
    SU_PPDU_1XLTF_08US_GI_SWITCH = 17,
    HT_SELF_CURE_DEBUG = 18,
    USERCTL_BINDCPU = 19,  /* �û�ָ��Ӳ�жϺ��̰߳�� */
#ifdef _PRE_WLAN_FEATURE_MCAST_AMPDU
    MCAST_AMPDU_RETRY = 20, /* �鲥�ۺ�ģʽ�������鲥֡�ش����� */
#endif

    MAC_COMMON_DEBUG_BUTT
} mac_common_debug_type_enum;
typedef uint8_t mac_common_debug_type_enum_uint8;

typedef enum {
    MAC_VAP_11AX_DEBUG_PRINT_LOG,
    MAC_VAP_11AX_DEBUG_PRINT_RX_LOG,
    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_TID_NUM,
    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_HTC_ORGER,
    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_HTC_VALUE,
    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_FIX_POWER,
    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_POWER_VALUE,
    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_DISABLE_BA_CHECK,
    MAC_VAP_11AX_DEBUG_DISABLE_MU_EDCA,
    MAC_VAP_11AX_DEBUG_MANUAL_CFO,
    MAC_VAP_11AX_DEBUG_BSRP_CFG,
    MAC_VAP_11AX_DEBUG_BSRP_TID,
    MAC_VAP_11AX_DEBUG_BSRP_QUEUE_SIZE,
    MAC_VAP_11AX_DEBUG_MAC_PADDING,
    MAC_VAP_11AX_DEBUG_POW_TEST,
    MAC_VAP_11AX_DEBUG_TB_PPDU_LEN,
    MAC_VAP_11AX_DEBUG_TB_PPDU_AC,
    MAC_VAP_11AX_DEBUG_PRINT_RX_TRIG_FRAME,
    MAC_VAP_11AX_DEBUG_OM_AUTH_FLAG,
    MAC_VAP_11AX_DEBUG_TOM_BW_FLAG,
    MAC_VAP_11AX_DEBUG_TOM_NSS_FLAG,
    MAC_VAP_11AX_DEBUG_TOM_UL_MU_DISABLE_FLAG,
    MAC_VAP_11AX_DEBUG_WIFI6_SELF_CURE_FLAG,
    MAC_VAP_11AX_DEBUG_UORA_OCW_UPDATE,
    MAC_VAP_11AX_DEBUG_UORA_OBO_BYPASS,
    MAC_VAP_11AX_DEBUG_BSS_COLOR,
    MAC_VAP_11AX_DEBUG_TX_Q_STATUS,

    MAC_VAP_11AX_DEBUG_BUTT
} mac_vap_11ax_debug_type_enum;
typedef uint8_t mac_vap_11ax_debug_type_enum_uint8;

typedef enum {
    MAC_PS_TYPE_DBAC = BIT0,
    MAC_PS_TYPE_BTCOEX = BIT1,
    MAC_PS_TYPE_CMD = BIT2,

    MAC_PS_TYPE_BUTT
} mac_ps_type_enum;

typedef enum {
    MAC_PROBE_RESP_MODE_ACTIVE = 0, /* �����յ�probe req��������probe resp�����ظ����տ� */
    MAC_PROBE_RESP_MODE_PASSIVE = 1, /* �����յ�probe req��˳���ϲ�ľ���ѡ���Իظ�probe resp֡ */
    MAC_PROBE_RESP_MODE_BUTT
} mac_probe_resp_mode_enum;
typedef uint8_t mac_probe_resp_mode_enum_uint8; /* hd_event */

typedef struct {
    oal_bool_enum_uint8 en_probe_resp_enable;    /* ��bool���ؿ���probe_resp_status�Ƿ���Ա��Ķ� */
    mac_probe_resp_mode_enum_uint8 en_probe_resp_status;    /* probe resp�ظ���ģʽ */
    uint8_t auc_rsv[NUM_2_BYTES];
} mac_probe_resp_ctrl_stru;

/* ��ӡ���ձ��ĵ�rssi��Ϣ�ĵ��Կ�����صĽṹ�� */
typedef struct {
    uint16_t us_data_len;        /*  �������������  */
    uint8_t uc_tone_tran_switch; /*  �������Ϳ���  */
    uint8_t uc_chain_idx;        /*  ��������ͨ����  */
} mac_tone_transmit_stru;

/* ��ӡ���ձ��ĵ�rssi��Ϣ�ĵ��Կ�����صĽṹ�� */
typedef struct {
    oal_bool_enum_uint8 en_debug_switch;          /* ��ӡ�ܿ��� */
    oal_bool_enum_uint8 en_rssi_debug_switch;     /* ��ӡ���ձ��ĵ�rssi��Ϣ�ĵ��Կ��� */
    oal_bool_enum_uint8 en_snr_debug_switch;      /* ��ӡ���ձ��ĵ�snr��Ϣ�ĵ��Կ��� */
    oal_bool_enum_uint8 en_trlr_debug_switch;     /* ��ӡ���ձ��ĵ�trailer��Ϣ�ĵ��Կ��� */
    oal_bool_enum_uint8 en_evm_debug_switch;      /* ��ӡ���ձ��ĵ�evm��Ϣ�ĵ��Կ��� */
    oal_bool_enum_uint8 en_sp_reuse_debug_switch; /* ��ӡ���ձ��ĵ�spatial reuse��Ϣ�ĵ��Կ��� */
    uint8_t auc_resv[BYTE_OFFSET_2];
    uint32_t curr_rx_comp_isr_count;     /* һ�ּ���ڣ���������жϵĲ������� */
    uint32_t rx_comp_isr_interval;       /* ������ٸ���������жϴ�ӡһ��rssi��Ϣ */
    mac_tone_transmit_stru st_tone_tran;      /* �������Ͳ��� */
    uint8_t auc_trlr_sel_info[BYTE_OFFSET_5];           /* trailerѡ���ϱ�����, һ���ֽڸ�4bitָʾtrlr or vect,��4��bitָʾѡ�� */
    uint8_t uc_trlr_sel_num;                /* ��¼������������ѡ������ֵ */
    uint8_t uc_iq_cali_switch;              /* iqУ׼��������  */
    oal_bool_enum_uint8 en_pdet_debug_switch; /* ��ӡоƬ�ϱ�pdetֵ�ĵ��Կ��� */
    oal_bool_enum_uint8 en_tsensor_debug_switch;
    uint8_t uc_force_work_switch;
    uint8_t uc_dfr_reset_switch;         /* dfr_reset��������: ��4bitΪreset_mac_submod, ��4bitΪreset_hw_type */
    uint8_t uc_fsm_info_switch;          /* hal fsm debug info */
    uint8_t uc_report_radar_switch;      /* radar�ϱ����� */
    uint8_t uc_extlna_chg_bypass_switch; /* ���Ĳ��Թر�����LNA����: 0/1/2:no_bypass/dyn_bypass/force_bypass */
    uint8_t uc_edca_param_switch;        /* EDCA�������ÿ��� */
    uint8_t uc_edca_aifsn;               /* edca����AIFSN */
    uint8_t uc_edca_cwmin;               /* edca����CWmin */
    uint8_t uc_edca_cwmax;               /* edca����CWmax */
    uint16_t us_edca_txoplimit;          /* edca����TXOP limit */
} mac_phy_debug_switch_stru; /* hd_event */

typedef struct {
    uint8_t uc_key_index;
    oal_bool_enum_uint8 en_unicast;
    oal_bool_enum_uint8 en_multicast;
    uint8_t auc_resv1[1];
} mac_setdefaultkey_param_stru;

typedef struct {
    uint16_t us_aid;
    uint16_t us_rudir;
} dmac_atcmdsrv_set_ru_info; /* hd_event */

typedef struct wal_atcmdsrv_wifi_rumode_info {
    uint16_t us_aid;
    uint16_t us_rudir;
} st_wal_atcmdsrv_wifi_rumode_info; /* hd_event */

/* SDT��ȡ�����üĴ����Ľṹ�壻WTBD:�Ƶ�common */
typedef struct {
    uint32_t addr;
    uint16_t us_len;
    uint8_t uc_mode;
    uint8_t auc_resv[1];
    uint32_t reg_val;
} dmac_sdt_reg_frame_stru; /* hd_event */

typedef enum {
    MAC_DBDC_CHANGE_HAL_DEV = 0, /* vap change hal device hal vap */
    MAC_DBDC_SWITCH = 1,         /* DBDC������� */
    MAC_FAST_SCAN_SWITCH = 2,    /* ����ɨ�迪�� */
    MAC_DBDC_STATUS = 3,         /* DBDC״̬��ѯ */

    MAC_DBDC_CMD_BUTT
} mac_dbdc_cmd_enum;
typedef uint8_t mac_dbdc_cmd_enum_uint8; /* hd_event */

typedef struct {
    uint32_t cmd_bit_map;
    uint8_t uc_dst_hal_dev_id; /* ��ҪǨ�Ƶ���hal device id */
    uint8_t uc_dbdc_enable;
    oal_bool_enum_uint8 en_fast_scan_enable; /* �Ƿ���Բ���,XXԭ��ʹӲ��֧��Ҳ���ܿ���ɨ�� */
    uint8_t uc_dbdc_status;
} mac_dbdc_debug_switch_stru; /* hd_event */

enum {
    FREQ_SET_MODE = 0,
    /* sync ini data */
    FREQ_SYNC_DATA = 1,
    /* for device debug */
    FREQ_SET_FREQ = 2,
    FREQ_SET_PLAT_FREQ = 3,
    FREQ_GET_FREQ = 4,
    FREQ_SET_FREQ_TC_EN = 5,
    FREQ_SET_FREQ_TC_EXIT = 6,
    FREQ_SET_BUTT
};
typedef uint8_t freq_sync_enum_uint8;

typedef struct {
    uint32_t speed_level;    /* ���������� */
    uint32_t cpu_freq_level; /* CPUƵ��level */
} device_level_stru;

typedef struct {
    uint8_t uc_set_type;
    uint8_t uc_set_freq;
    uint8_t uc_device_freq_enable;
    uint8_t uc_resv;
    device_level_stru st_device_data[DEV_WORK_FREQ_LVL_NUM];
} config_device_freq_h2d_stru; /* hd_event */


typedef struct {               /* hi1102-cb */
    uint8_t uc_shortgi_type; /* shortgi 20/40/80     */
    uint8_t uc_enable;       /* 1:enable; 0:disable  */
    uint8_t auc_resv[NUM_2_BYTES];
} shortgi_cfg_stru; /* hd_event */

/* ���÷����������ڲ�Ԫ�ؽṹ�� */
typedef enum {
    RF_PAYLOAD_ALL_ZERO = 0,
    RF_PAYLOAD_ALL_ONE,
    RF_PAYLOAD_RAND,
    RF_PAYLOAD_BUTT
} mac_rf_payload_enum;
typedef uint8_t mac_rf_payload_enum_uint8;

typedef struct {
    uint8_t uc_param; /* ��ѯ��������Ϣ */
    wlan_phy_protocol_enum_uint8 en_protocol_mode;
    mac_rf_payload_enum_uint8 en_payload_flag;
    wlan_tx_ack_policy_enum_uint8 en_ack_policy;
    uint32_t payload_len;
    uint32_t write_index;
    uint32_t base_addr_lsb;
    uint32_t base_addr_msb;
} mac_cfg_tx_comp_stru; /* hd_event */

/* оƬ��֤������֡/����֡���� */
typedef enum {
    MAC_TEST_MGMT_BCST = 0,               /* ��beacon�㲥����֡ */
    MAC_TEST_MGMT_MCST = 1,               /* ��beacon�鲥����֡ */
    MAC_TEST_ATIM_UCST = 2,               /* ����ATIM֡ */
    MAC_TEST_UCST = 3,                    /* ��������֡ */
    MAC_TEST_CTL_BCST = 4,                /* �㲥����֡ */
    MAC_TEST_CTL_MCST = 5,                /* �鲥����֡ */
    MAC_TEST_CTL_UCST = 6,                /* ��������֡ */
    MAC_TEST_ACK_UCST = 7,                /* ACK����֡ */
    MAC_TEST_CTS_UCST = 8,                /* CTS����֡ */
    MAC_TEST_RTS_UCST = 9,                /* RTS����֡ */
    MAC_TEST_BA_UCST = 10,                /* BA����֡ */
    MAC_TEST_CF_END_UCST = 11,            /* CF-End����֡ */
    MAC_TEST_TA_RA_EUQAL = 12,            /* RA,TA��ͬ֡ */
    MAC_TEST_FTM = 13,                    /* FTM����֡ */
    MAC_TEST_MGMT_ACTION = 14,            /* ��������Action֡ */
    MAC_TEST_MGMT_BEACON_INCLUDE_IE = 15, /* Beacon ֡��β����������IE��ϢԪ�� */
    MAC_TEST_MAX_TYPE_NUM
} mac_test_frame_type;
typedef uint8_t mac_test_frame_type_enum_uint8;

#define MAC_TEST_INCLUDE_FRAME_BODY_LEN 64
typedef struct {
    uint8_t auc_mac_ra[WLAN_MAC_ADDR_LEN];
    uint8_t uc_pkt_num;
    mac_test_frame_type_enum_uint8 en_frame_type;
    uint8_t uc_frame_body[MAC_TEST_INCLUDE_FRAME_BODY_LEN]; /* 64 ֡�峤�� */
    uint8_t uc_vap_idx;
    uint8_t uc_frame_cnt;
    uint8_t uc_frame_body_length;
    uint8_t uc_resv[1];
} mac_cfg_send_frame_param_stru;  /* hd_event */

typedef struct {
    uint8_t uc_aggr_num_switch; /* ���ƾۺϸ������� */
    uint8_t uc_aggr_num;        /* �ۺϸ��� */
    uint8_t auc_resv[NUM_2_BYTES];
} mac_cfg_aggr_num_stru; /* hd_event */


typedef enum {
    MAC_DYN_CALI_CFG_SET_EN_REALTIME_CALI_ADJUST,
    MAC_DYN_CALI_CFG_SET_2G_DSCR_INT,
    MAC_DYN_CALI_CFG_SET_5G_DSCR_INT,
    MAC_DYN_CALI_CFG_SET_CHAIN_INT,
    MAC_DYN_CALI_CFG_SET_PDET_MIN_TH,
    MAC_DYN_CALI_CFG_SET_PDET_MAX_TH,
    MAC_DYN_CALI_CFG_BUFF,
} mac_dyn_cali_cfg_enum;
typedef uint8_t mac_dyn_cali_cfg_enum_uint8;

/* ��̬У׼����ö�٣�����ֵ */
typedef struct {
    mac_dyn_cali_cfg_enum_uint8 en_dyn_cali_cfg; /* ��������ö�� */
    uint8_t uc_resv;                           /* �ֽڶ��� */
    uint16_t us_value;                         /* ���ò���ֵ */
} mac_ioctl_dyn_cali_param_stru; /* hd_event */

typedef struct {
    uint8_t uc_cmd_type;
    uint8_t uc_len;
    uint16_t us_cfg_id;
    uint32_t value;
} mac_cfg_set_tlv_stru;

typedef struct {
    uint16_t us_cfg_id;
    uint8_t uc_cmd_cnt;
    uint8_t uc_len;
    uint16_t us_set_id[MAC_STR_CMD_MAX_SIZE];
    uint32_t value[MAC_STR_CMD_MAX_SIZE];
} mac_cfg_set_str_stru;

typedef enum {
    MAC_MEMINFO_USER = 0,
    MAC_MEMINFO_VAP,
    MAC_MEMINFO_POOL_INFO,
    MAC_MEMINFO_POOL_DBG,

    MAC_MEMINFO_DSCR,
    MAC_MEMINFO_NETBUFF,
    MAC_MEMINFO_SDIO_TRX,
    MAC_MEMINFO_SAMPLE_ALLOC,
    MAC_MEMINFO_SAMPLE_FREE,
    MAC_MEMINFO_ALL,

    MAC_MEMINFO_BUTT
} mac_meminfo_cmd_enum;
typedef uint8_t mac_meminfo_cmd_enum_uint8;

typedef struct {
    mac_meminfo_cmd_enum_uint8 uc_meminfo_type;
    uint8_t uc_object_index;
} mac_cfg_meminfo_stru;

#endif /* end of hd_command.h */

