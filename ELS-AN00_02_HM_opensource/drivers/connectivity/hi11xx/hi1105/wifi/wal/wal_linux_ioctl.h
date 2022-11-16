

#ifndef __WAL_LINUX_IOCTL_H__
#define __WAL_LINUX_IOCTL_H__

#include "oal_ext_if.h"
#include "wlan_types.h"
#include "wlan_spec.h"
#include "mac_vap.h"
#include "hmac_ext_if.h"
#include "wal_ext_if.h"
#include "wal_config.h"
#include "alg_cfg.h"

#ifdef CONFIG_DOZE_FILTER
#include <huawei_platform/power/wifi_filter/wifi_filter.h>
#endif /* CONFIG_DOZE_FILTER */
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
#include "hisi_customize_wifi.h"
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_IOCTL_H
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WLAN1_NETDEV_NAME          "wlan1"
#define MAX_PRIV_CMD_SIZE          4096
#define NET_DEV_NAME_LEN           16
#define ENABLE 1
#define DISABLE 0
#define BUFF_SIZE 64
#define CMD_LENGTH 2

#define WAL_HIPRIV_RATE_INVALID 255 /*  ��Ч��������ֵ  */

#ifdef _PRE_WLAN_RR_PERFORMANCE
#define ASPM_ENABLE                1
#define ASPM_DISABLE               0
#define FREQ_MAX_VALUE             30
#define FREQ_MAX_FLAG              0
#define FREQ_AUTO_FLAG             1
#endif /* _PRE_WLAN_RR_PERFORMANCE */

#define WAL_HIPRIV_CMD_MAX_LEN (WLAN_MEM_LOCAL_SIZE2 - 4) /* ˽�����������ַ�����󳤶ȣ���Ӧ�����ڴ��һ����С */

#define WAL_HIPRIV_CMD_NAME_MAX_LEN  80 /* �ַ�����ÿ�����ʵ���󳤶�(ԭ20) */
#define WAL_HIPRIV_CMD_VALUE_MAX_LEN 10 /* �ַ�����ĳ����Ӧ����ȡֵ�����λ�� */

#define WAL_HIPRIV_PROC_ENTRY_NAME "hipriv"

#define WAL_SIOCDEVPRIVATE 0x89F0 /* SIOCDEVPRIVATE */

#define WAL_HIPRIV_HT_MCS_MIN    0
#define WAL_HIPRIV_HT_MCS_MAX    32
#define WAL_HIPRIV_VHT_MCS_MIN   0
#define WAL_HIPRIV_VHT_MCS_MAX   11
#define WAL_HIPRIV_HE_MCS_MIN    0
#define WAL_HIPRIV_HE_MCS_MAX    11
#define WAL_HIPRIV_HE_ER_MCS_MIN 0
#define WAL_HIPRIV_HE_ER_MCS_MAX 2
#define WAL_HIPRIV_NSS_MIN       1
#define WAL_HIPRIV_NSS_MAX       4
#define WAL_HIPRIV_CH_NUM        4
#ifdef  _PRE_WLAN_FEATURE_11AX_ER_SU_DCM
#define WAL_HIPRIV_MCS_TYPE_NUM  (WAL_DSCR_PARAM_MCSAX_ER - WAL_DSCR_PARAM_MCS + 1)
#else
#define WAL_HIPRIV_MCS_TYPE_NUM  (WAL_DSCR_PARAM_MCSAX - WAL_DSCR_PARAM_MCS + 1)
#endif

#define WAL_HIPRIV_BOOL_NIM          0
#define WAL_HIPRIV_BOOL_MAX          1
#define WAL_HIPRIV_FREQ_SKEW_ARG_NUM 8
#define WAL_PHY_DEBUG_TEST_WORD_CNT  5 /* trailer�ϱ����� */

#define WAL_HIPRIV_MS_TO_S                1000   /* ms��s֮�䱶���� */
#define WAL_HIPRIV_KEEPALIVE_INTERVAL_MIN 5000   /* ��Ĭ���ϻ�����������ʱ�������� */
#define WAL_HIPRIV_KEEPALIVE_INTERVAL_MAX 0xffff /* timer���ʱ����������(oal_uin16) */

#define WAL_IWPRIV_CAP_NUM      14
#define WAL_IWPRIV_IGMP_MIN_LEN 50

/* �û�pwr ref reg�Ķ��ƻ�������ֵ */
#define WAL_HIPRIV_PWR_REF_DELTA_HI 40
#define WAL_HIPRIV_PWR_REF_DELTA_LO (-40)

/* IOCTL˽����������궨�� */
#define WAL_IOCTL_PRIV_SETPARAM      (OAL_SIOCIWFIRSTPRIV + 0)
#define WAL_IOCTL_PRIV_GETPARAM      (OAL_SIOCIWFIRSTPRIV + 1)
#define WAL_IOCTL_PRIV_SET_WMM_PARAM (OAL_SIOCIWFIRSTPRIV + 3)
#define WAL_IOCTL_PRIV_GET_WMM_PARAM (OAL_SIOCIWFIRSTPRIV + 5)
#define WAL_IOCTL_PRIV_SET_COUNTRY   (OAL_SIOCIWFIRSTPRIV + 8)
#define WAL_IOCTL_PRIV_GET_COUNTRY   (OAL_SIOCIWFIRSTPRIV + 9)

#define WAL_IOCTL_PRIV_GET_MODE (OAL_SIOCIWFIRSTPRIV + 17) /* ��ȡģʽ */
#define WAL_IOCTL_PRIV_SET_MODE (OAL_SIOCIWFIRSTPRIV + 13) /* ����ģʽ ����Э�� Ƶ�� ���� */

#define WAL_IOCTL_PRIV_AP_GET_STA_LIST (OAL_SIOCIWFIRSTPRIV + 21)
#define WAL_IOCTL_PRIV_AP_MAC_FLTR     (OAL_SIOCIWFIRSTPRIV + 22)
/* netd��������������ΪGET��ʽ�·���get��ʽ������������set��ż�� */
#define WAL_IOCTL_PRIV_SET_AP_CFG      (OAL_SIOCIWFIRSTPRIV + 23)
#define WAL_IOCTL_PRIV_AP_STA_DISASSOC (OAL_SIOCIWFIRSTPRIV + 24)

#define WAL_IOCTL_PRIV_SET_MGMT_FRAME_FILTERS (OAL_SIOCIWFIRSTPRIV + 28) /* ���ù���֡���� */

#define WAL_IOCTL_PRIV_GET_BLACKLIST (OAL_SIOCIWFIRSTPRIV + 27)

#ifdef _PRE_WLAN_FEATURE_DFS
#define WAL_IOCTL_PRIV_GET_DFS_CHN_STAT (OAL_SIOCIWFIRSTPRIV + 29)
#endif

#define WAL_IOCTL_PRIV_SUBCMD_MAX_LEN 20

#define EXT_PA_ISEXIST_5G_MASK   0x000F
#define EXT_FEM_LP_STATUS_MASK   0x00F0
#define EXT_FEM_FEM_SPEC_MASK    0xFF00
#define EXT_FEM_LP_STATUS_OFFSET 4
#define EXT_FEM_FEM_SPEC_OFFSET  8

#define WAL_IFR_DATA_OFFSET 8

#define WAL_VAP_FOREACH_USER(_pst_user, _pst_vap, _pst_list_pos)                          \
    for ((_pst_list_pos) = (_pst_vap)->st_mac_user_list_head.pst_next,                    \
         (_pst_user) = oal_dlist_get_entry((_pst_list_pos), mac_user_stru, st_user_dlist); \
         (_pst_list_pos) != &((_pst_vap)->st_mac_user_list_head);                         \
         (_pst_list_pos) = (_pst_list_pos)->pst_next,                                     \
         (_pst_user) = oal_dlist_get_entry((_pst_list_pos), mac_user_stru, st_user_dlist)) \
        if (NULL != (_pst_user))

typedef uint32_t (*wal_hipriv_cmd_func)(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
typedef uint32_t (*wal_hid2d_vendor_cmd_func)(oal_net_device_stru *net_dev, int8_t *cmd);

typedef uint32_t (*pwal_get_cmd_one_arg)(int8_t *pc_cmd, int8_t *pc_arg, uint32_t arg_len, uint32_t *pul_cmd_offset);
/* wlan cfg��� */
#define WLAN_CFG_INFO_TBL(_wlan_cfg_id, _en_need_host_process, _en_need_w4_host_return, \
                          _en_need_dev_process, _en_need_w4_dev_return, _p_func) \
    { \
        _wlan_cfg_id, { 0, 0 }, { _en_need_host_process, \
                                  _en_need_w4_host_return, \
                                  _en_need_dev_process, \
                                  _en_need_w4_dev_return }, _p_func      \
    }
#define WLAN_CFG_ALG_SUB_ALG_ANALYSIS_TBL(_en_cfg_id, _p_func) \
    {                                                          \
        _en_cfg_id, { 0, 0 }, _p_func                          \
    }
#define WAL_WLAN_CFG_NEED_PROCESS(_process_info)   ((_process_info)->st_wlan_cfg_process_info.en_need_host_process ||  \
                                                    (_process_info)->st_wlan_cfg_process_info.en_need_device_process)

/* if TV && built-in, open wlan0 is early and WAL_WAIT_CALI_COMPLETE_TIME should be longer */
#if (defined _PRE_PRODUCT_HI3751V811) && (!defined CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT)
#define WAL_WAIT_CALI_COMPLETE_TIME  15000
#else
#define WAL_WAIT_CALI_COMPLETE_TIME  6000
#endif

#ifdef _PRE_LINUX_TEST
#define BOARD_SUCC OAL_SUCC
#endif

#ifdef _PRE_WLAN_FEATURE_NAN
/* NAN���ڶ̻��߼���ʱ�䳤Ӱ��ҵ�� */
#define WAL_NAN_MAX_DURATION 80
#define WAL_NAN_MIN_PERIOD 200
#endif

typedef enum {
    WAL_DSCR_PARAM_PA_GAIN_LEVEL = 0, /* pa����ȼ� */
    WAL_DSCR_PARAM_MICRO_TX_POWER_GAIN_LEVEL,
    WAL_DSCR_PARAM_TXRTS_ANTENNA,
    WAL_DSCR_PARAM_RXCTRL_ANTENNA,
    WAL_DSCR_PARAM_CHANNAL_CODE,
    WAL_DSCR_PARAM_POWER,
    WAL_DSCR_PARAM_SHORTGI,
    WAL_DSCR_PARAM_PREAMBLE_MODE,
    WAL_DSCR_PARAM_RTSCTS,
    WAL_DSCR_PARAM_LSIGTXOP,
    WAL_DSCR_PARAM_SMOOTH,
    WAL_DSCR_PARAM_SOUNDING,
    WAL_DSCR_PARAM_TXBF,
    WAL_DSCR_PARAM_STBC,
    WAL_DSCR_PARAM_GET_ESS,
    WAL_DSCR_PARAM_DYN_BW,
    WAL_DSCR_PARAM_DYN_BW_EXIST,
    WAL_DSCR_PARAM_CH_BW_EXIST,
    WAL_DSCR_PARAM_RATE,
    WAL_DSCR_PARAM_MCS,
    WAL_DSCR_PARAM_MCSAC,
    WAL_DSCR_PARAM_MCSAX,
    WAL_DSCR_PARAM_MCSAX_ER,
    WAL_DSCR_PARAM_NSS,
    WAL_DSCR_PARAM_BW,
    WAL_DSCR_PARAM_LTF,
    WLA_DSCR_PARAM_GI,
    WLA_DSCR_PARAM_TXCHAIN,
    WLA_DSCR_PARAM_DCM,
    WLA_DSCR_PARAM_PROTOCOL_MODE,

    WAL_DSCR_PARAM_BUTT
} wal_dscr_param_enum;

typedef uint8_t wal_dscr_param_enum_uint8;

/* rx ip���ݰ����˹��ܺ��ϲ�Э��(��ʽ)�ṹ�壬TBD �ϲ�ӿ���δ��ȷ���������޸� */
#ifdef CONFIG_DOZE_FILTER
typedef hw_wifi_filter_item wal_hw_wifi_filter_item;
typedef struct hw_wlan_filter_ops wal_hw_wlan_filter_ops;

#else
typedef struct {
    unsigned short protocol;  // Э������
    unsigned short port;      // Ŀ�Ķ˿ں�
    unsigned int filter_cnt;  // ���˱�����
} wal_hw_wifi_filter_item;

typedef struct {
    int (*set_filter_enable)(int);
    int (*set_filter_enable_ex)(int, int);
    int (*add_filter_items)(wal_hw_wifi_filter_item *, int);
    int (*clear_filters)(void);
    int (*get_filter_pkg_stat)(wal_hw_wifi_filter_item *, int, int *);
} wal_hw_wlan_filter_ops;
#endif

typedef enum {
    WAL_TX_POW_PARAM_SET_RF_REG_CTL = 0,
    WAL_TX_POW_PARAM_SET_FIX_LEVEL,
    WAL_TX_POW_PARAM_SET_MAG_LEVEL,
    WAL_TX_POW_PARAM_SET_CTL_LEVEL,
    WAL_TX_POW_PARAM_SET_AMEND,
    WAL_TX_POW_PARAM_SET_NO_MARGIN,
    WAL_TX_POW_PARAM_SET_SHOW_LOG,
    WAL_TX_POW_PARAM_SET_SAR_LEVEL,
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
    WAL_TX_POW_PARAM_TAS_POW_CTRL,
    WAL_TX_POW_PARAM_TAS_RSSI_MEASURE,
    WAL_TX_POW_PARAM_TAS_ANT_SWITCH,
#endif
    WAL_TX_POW_PARAM_SHOW_TPC_TABLE_GAIN,
    WAL_TX_POW_POW_SAVE,
    WAL_TX_POW_GET_PD_INFO,
    WAL_TX_POW_SET_TPC_IDX,

    WAL_TX_POW_PARAM_BUTT
} wal_tx_pow_param_enum;

typedef enum {
    WAL_AMPDU_DISABLE,
    WAL_AMPDU_ENABLE,

    WAL_AMPDU_CFG_BUTT
} wal_ampdu_idx_enum;
#ifdef _PRE_WLAN_RR_PERFORMANCE
/* pc����ö������ */
typedef enum {
    WAL_EXT_PRI_CMD_SET_DEVICE_FREQ_MAX,     // 0 ��DeviceƵ�����������
    WAL_EXT_PRI_CMD_SET_DEVICE_FREQ_AUTO,    // 1 ��DeviceƵ�����óɶ�̬��Ƶ
    WAL_EXT_PRI_CMD_ENBLE_PCIE_ASPM,         // 2 ��PCIE�͹���
    WAL_EXT_PRI_CMD_DISABLE_PCIE_ASPM,       // 3 �ر�PCIE�͹���

    WAL_EXT_PRI_CMD_BUFF
} wal_ext_pri_cmd_neum;
#endif /* _PRE_WLAN_RR_PERFORMANCE */

/* HiD2D APK����ö��ֵ���ϲ��·����� */
typedef enum {
    WAL_HID2D_INIT_CMD = 0,
    WAL_HID2D_SCAN_START_CMD = 1,
    WAL_HID2D_CHAN_SWITCH_CMD = 2,
    WAL_HID2D_HIGH_BW_MCS_CMD = 3,
    WAL_HID2D_RTS_CMD = 4,
    WAL_HID2D_TPC_CMD = 5,
    WAL_HID2D_CCA_CMD = 6,
    WAL_HID2D_EDCA_CMD = 7,
    WAL_HID2D_ACS_CMD = 8,
    WAL_HID2D_ACS_STATE_CMD = 9,
    WAL_HID2D_LINK_MEAS_CMD = 10,

    WAL_HID2D_CMD_BUTT,
} hmac_hid2d_cmd_enum;

extern oal_iw_handler_def_stru g_st_iw_handler_def;
extern oal_net_device_ops_stru g_st_wal_net_dev_ops;
extern oal_ethtool_ops_stru g_st_wal_ethtool_ops;
extern oal_wait_queue_head_stru g_wlan_cali_complete_wq;

/* 110x ���ڽ��ϲ��·��ַ����������л� */
typedef struct {
    uint8_t     *pc_priv_cmd;
    uint32_t     case_entry;
} wal_ioctl_priv_cmd_stru;
/* ˽��������ڽṹ���� */
typedef struct {
    int8_t *pc_cmd_name;      /* �����ַ��� */
    wal_hipriv_cmd_func p_func; /* �����Ӧ������ */
} wal_hipriv_cmd_entry_stru;

/* VoWiFi�����ת���ṹ�� */
typedef struct {
    int8_t *pc_vowifi_cmd_name;            /* �����ַ��� */
    mac_vowifi_cmd_enum_uint8 en_vowifi_cmd; /* �����Ӧ���� */
    uint8_t auc_resv[3]; /* 3ʵ���ֽڶ��� */
} wal_vowifi_cmd_stru;

/* Э��ģʽ���ַ���ӳ�� */
typedef struct {
    int8_t *pc_name;                              /* ģʽ���ַ��� */
    wlan_protocol_enum_uint8 en_mode;               /* Э��ģʽ */
    wlan_channel_band_enum_uint8 en_band;           /* Ƶ�� */
    wlan_channel_bandwidth_enum_uint8 en_bandwidth; /* ���� */
    uint8_t auc_resv[1];
} wal_ioctl_mode_map_stru;

/* �㷨�������ýṹ�� */
typedef struct {
    int8_t *pc_name;                           /* ���������ַ��� */
    mac_dyn_cali_cfg_enum_uint8 en_dyn_cali_cfg; /* ���������Ӧ��ö��ֵ */
    uint8_t auc_resv[3];                       /* 3ʵ���ֽڶ��� */
} wal_ioctl_dyn_cali_stru;

/* �㷨�������ýṹ�� */
typedef struct {
    int8_t *pc_name;                 /* ���������ַ��� */
    mac_alg_cfg_param_enum_uint8 en_alg_cfg; /* ���������Ӧ��ö��ֵ */
    uint8_t auc_resv[3];             /* 3ʵ���ֽڶ��� */
} wal_ioctl_alg_cfg_stru;

#ifdef _PRE_WLAN_RR_PERFORMANCE
typedef struct wal_wifi_pc_cmd {
    int32_t   l_verify;    /* У��λ */
    uint32_t  cmd;      /* ����� */
} wal_wifi_pc_cmd_stru;
#endif /* _PRE_WLAN_RR_PERFORMANCE */
typedef struct {
    int8_t *pc_name;                    /* ���������ַ��� */
    wlan_cfgid_enum_uint16 en_tlv_cfg_id; /* ���������Ӧ��ö��ֵ */
    uint8_t auc_resv[2];                /* 2ʵ���ֽڶ��� */
} wal_ioctl_tlv_stru;

typedef struct {
    int8_t *pc_name;                    /* ���������ַ��� */
    wlan_cfgid_enum_uint16 en_tlv_cfg_id; /* ���������Ӧ��ö��ֵ */
    wal_ioctl_tlv_stru *pst_cfg_table;
} wal_ioctl_str_stru;
typedef struct {
    uint32_t ap_max_user;           /* ap����û��� */
    int8_t ac_ap_mac_filter_mode[257]; /* 257����AP mac��ַ�����������,�256 */
    int32_t l_ap_power_flag;           /* AP�ϵ��־ */
} wal_ap_config_stru;

typedef struct {
    int8_t *pc_country;                    /* �����ַ��� */
    mac_dfs_domain_enum_uint8 en_dfs_domain; /* DFS �״��׼ */
} wal_dfs_domain_entry_stru;
/* ��������������⹫��������ݽṹ */
typedef struct {
    oal_net_device_stru   *pst_net_dev;
    wlan_cfgid_enum_uint16 en_wlan_cfg_id;

    /* ����ַ�ָ�����飬����ָ��ÿ�������ַ����׵�ַ */
    uint8_t              uc_args_num; /* ��θ��� */
    char              *apc_args[WLAN_CFG_MAX_ARGS_NUM];

    /* �����Զ��崦�����ַ�����д�� */
    char              *pc_output;
    uint16_t             us_output_len;

    uint32_t             process_ret;
} wlan_cfg_param_stru;
typedef struct {
    oal_bool_enum_uint8 en_need_host_process;     /* �Ƿ���Ҫhost����̴߳��� */
    oal_bool_enum_uint8 en_need_w4_host_return;   /* �Ƿ���Ҫ����host����̴߳����� */
    oal_bool_enum_uint8 en_need_device_process;   /* �Ƿ���Ҫdevice�ദ�� */
    oal_bool_enum_uint8 en_need_w4_device_return; /* �Ƿ���Ҫ����device�ദ���� */
} wlan_cfg_process_info_stru;

/* AUTORATE LOG �㷨����ö�٣�����ֵ */
typedef struct {
    mac_alg_cfg_param_enum_uint8 en_alg_cfg;         /* ��������ö�� */
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC��ַ */
    uint8_t uc_ac_no;                        /* AC���� */
    uint8_t auc_resv[NUM_2_BYTES];
    uint16_t us_value; /* ���ò���ֵ */
} wal_ioctl_alg_ar_log_param_stru;

/* TXMODE LOG �㷨����ö�٣�����ֵ */
typedef struct {
    mac_alg_cfg_param_enum_uint8 en_alg_cfg;         /* ��������ö�� */
    uint8_t uc_ac_no;                        /* AC���� */
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC��ַ */
    uint8_t auc_resv1[NUM_2_BYTES];
    uint16_t us_value; /* ���ò���ֵ */
} wal_ioctl_alg_txbf_log_param_stru;

/* TPC LOG �㷨����ö�٣�����ֵ */
typedef struct {
    mac_alg_cfg_param_enum_uint8 en_alg_cfg;         /* ��������ö�� */
    uint8_t auc_mac_addr[WLAN_MAC_ADDR_LEN]; /* MAC��ַ */
    uint8_t uc_ac_no;                        /* AC���� */
    uint16_t us_value;                       /* ���ò���ֵ */
    int8_t *pc_frame_name;                   /* ��ȡ�ض�֡����ʹ�øñ��� */
} wal_ioctl_alg_tpc_log_param_stru;

/* cca opt LOG �㷨����ö�٣�����ֵ */
typedef struct {
    mac_alg_cfg_param_enum_uint8 en_alg_cfg; /* ��������ö�� */
    uint16_t us_value;               /* ͳ����ʱ�� */
    uint8_t auc_resv;
} wal_ioctl_alg_intfdet_log_param_stru;

/* HiD2D �������ýṹ�� */
typedef struct {
    uint8_t hid2d_cfg_type;  /* ������������ hmac_hid2d_cmd_enum */
    wal_hid2d_vendor_cmd_func func;           /* �����Ӧ������ */
} wal_hid2d_cmd_entry_stru;

/*
����      : wlan_cfg ��cfg_id������ں���, ���зǿ��߳�ֱ�Ӵ����򽫴������ָ�ʽ��Ϊ�ض��ṹ���ڴ�
�������  : wlan_cfg_param_stru        *pst_wlan_cfg_param
�������  : uint8_t    *pen_msg_type
            uint8_t                  *puc_trans_by_event
            uint16_t                 *pus_trans_len
            wlan_cfg_process_info_stru *pst_process_info
            ���ڽ�1�����ã����������ı䣬���������㷨��2�����ã�����Ϣ����Ϊ���ν�����Ӧ�޸�
�� �� ֵ  : int32_t

 */
typedef int32_t (*wal_wlan_cfg_process_entry_func)(wlan_cfg_param_stru *pst_wlan_cfg_param, uint8_t *pen_msg_type,
    uint8_t *puc_mem_block, uint16_t *pus_mem_len, wlan_cfg_process_info_stru *pst_process_info);
typedef struct {
    wlan_cfgid_enum_uint16          en_wlan_cfg_id;           /* wlan_cfg_id */
    uint8_t                       auc_resv[2];                /* 2�������ֽ������ṹ�����ֽڶ��� */
    wlan_cfg_process_info_stru      st_wlan_cfg_process_info; /* process info */
    /* process entry function of wlan_cfg_id, analysis args and fill the mem_block needed to transfer to wal */
    wal_wlan_cfg_process_entry_func p_func;
} wal_wlan_cfg_process_info_table_stru;

extern wal_ap_config_stru g_st_ap_config_info;
extern const wal_ioctl_str_stru g_ast_set_str_table[];
extern const wal_hipriv_cmd_entry_stru g_ast_hipriv_cmd[];
extern oal_ieee80211_supported_band g_st_supported_band_2ghz_info;
static inline oal_bool_enum_uint8 wal_ioctl_is_invalid_fixed_rate(int32_t val, mac_cfg_set_dscr_param_stru *stu)
{
    return (((val == WAL_HIPRIV_RATE_INVALID) &&
            (stu->uc_function_index >= WAL_DSCR_PARAM_RATE) &&
            (stu->uc_function_index <= WAL_DSCR_PARAM_MCSAC)) ||
            (val == 0 && (stu->uc_function_index == WAL_DSCR_PARAM_RATE)));
}
/* 8 UNION���� */
/* 9 OTHERS���� */
/* 10 �������� */
uint32_t wal_hipriv_ampdu_tx_on(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_amsdu_tx_on(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_rate(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_mcs(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_mcsac(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
#ifdef _PRE_WLAN_FEATURE_11AX
uint32_t wal_hipriv_set_mcsax(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
#ifdef  _PRE_WLAN_FEATURE_11AX_ER_SU_DCM
uint32_t _wal_hipriv_set_mcsax_er(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
#endif
#endif
uint32_t wal_hipriv_vap_info(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_ip_filter(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_create_proc(void *p_proc_arg);
uint32_t wal_hipriv_del_vap(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_remove_proc(void);
uint32_t wal_alloc_cfg_event(oal_net_device_stru *pst_net_dev, frw_event_mem_stru **ppst_event_mem,
                             void *pst_resp_addr, wal_msg_stru **ppst_cfg_msg, uint16_t us_len);
int32_t wal_send_cfg_event(oal_net_device_stru *pst_net_dev, uint8_t en_msg_type, uint16_t us_len, uint8_t *puc_param,
    oal_bool_enum_uint8 en_need_rsp, wal_msg_stru **ppst_rsp_msg);
uint32_t wal_hipriv_process_rate_params(oal_net_device_stru *pst_net_dev, int8_t *pc_cmd,
    mac_cfg_set_dscr_param_stru *pc_stu);
int32_t wal_start_vap(oal_net_device_stru *pst_net_dev);
int32_t wal_stop_vap(oal_net_device_stru *pst_net_dev);
int32_t wal_init_wlan_vap(oal_net_device_stru *pst_net_dev);
#ifdef _PRE_WLAN_CHBA_MGMT
int32_t wal_setup_chba(oal_net_device_stru *net_dev);
int32_t wal_init_chba_netdev(oal_wiphy_stru *pst_wiphy, const char *dev_name);
#endif
int32_t wal_deinit_wlan_vap(oal_net_device_stru *pst_net_dev);
int32_t wal_init_wlan_netdev(oal_wiphy_stru *pst_wiphy, const char *dev_name);
int32_t wal_setup_ap(oal_net_device_stru *pst_net_dev);
int32_t wal_host_dev_init(oal_net_device_stru *pst_net_dev);
#ifdef _PRE_WLAN_FEATURE_NAN
void wal_init_nan_netdev(oal_wiphy_stru *wiphy);
#endif

#ifdef _PRE_WLAN_FEATURE_11D
uint32_t wal_regdomain_update_sta(uint8_t uc_vap_id);
int32_t wal_regdomain_update_country_code(oal_net_device_stru *pst_net_dev, int8_t *pc_country);
#ifdef _PRE_WLAN_COUNTRYCODE_SELFSTUDY
int32_t wal_regdomain_update_selfstudy_country_code(oal_net_device_stru *pst_net_dev, int8_t *pc_country);
void wal_selfstudy_regdomain_update_by_ap(frw_event_stru *pst_event);
#endif
#endif

int32_t wal_set_random_mac_to_mib(oal_net_device_stru *pst_net_dev);
wlan_p2p_mode_enum_uint8 wal_wireless_iftype_to_mac_p2p_mode(enum nl80211_iftype en_iftype);
uint32_t wal_hipriv_arp_offload_enable(oal_net_device_stru *pst_net_dev, int8_t *pc_param);

#ifdef _PRE_WLAN_FEATURE_M2S_MSS
uint32_t wal_ioctl_set_m2s_mss(oal_net_device_stru *pst_net_dev, uint8_t uc_m2s_mode);
#endif
int32_t wal_cfg_vap_h2d_event(oal_net_device_stru *pst_net_dev);

int32_t wal_set_custom_process_func(void);
uint32_t wal_custom_cali(void);
void hwifi_config_init_force(void);
void hwifi_config_host_global_ini_param(void);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 44))
uint32_t wal_hipriv_set_essid(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
int wal_ioctl_set_essid(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info,
                        oal_iwreq_data_union *pst_wrqu, char *pc_extra);
#endif
uint32_t wal_get_cmd_one_arg(int8_t *pc_cmd, int8_t *pc_arg, uint32_t arg_len, uint32_t *pul_cmd_offset);
uint32_t wal_hipriv_send_cfg_uint32_data(oal_net_device_stru *pst_net_dev,
                                         int8_t *pc_param, wlan_cfgid_enum_uint16 cfgid);
#ifdef _PRE_WLAN_CFGID_DEBUG
uint32_t wal_hipriv_get_debug_cmd_size(void);
#endif
uint32_t wal_hipriv_alg_cfg(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_alg_cfg_entry(oal_net_device_stru *net_dev, int8_t *param);

uint32_t wal_hipriv_parse_cmd(int8_t *pc_cmd);
int32_t wal_set_assigned_filter_enable(int32_t l_filter_id, int32_t l_on);
int32_t wal_set_ip_filter_enable(int32_t l_on);
int32_t wal_add_ip_filter_items(wal_hw_wifi_filter_item *pst_items, int32_t l_count);
int32_t wal_clear_ip_filter(void);
#ifdef _PRE_WLAN_FEATURE_M2S
uint32_t wal_hipriv_set_m2s_switch(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
#endif
int32_t wal_register_ip_filter(wal_hw_wlan_filter_ops *pg_st_ip_filter_ops);
int32_t wal_unregister_ip_filter(void);
int32_t wlan_enter_light_suspend(void);
int32_t wlan_exit_light_suspend(void);
void wal_register_lights_suspend(void);
void wal_unregister_lights_suspend(void);

uint32_t wal_hipriv_dyn_cali_cfg(oal_net_device_stru *pst_net_dev, int8_t *puc_param);

uint32_t wal_ioctl_set_ap_mode(oal_net_device_stru *pst_net_dev, uint8_t uc_param);
uint32_t wal_hipriv_set_mcast_data_dscr_param(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_ioctl_get_psm_stat(oal_net_device_stru *pst_net_dev, mac_psm_query_type_enum_uint8 en_query_type,
    oal_ifreq_stru *pst_ifr);
uint32_t wal_hipriv_sta_pm_on(oal_net_device_stru *pst_cfg_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_tlv_cmd(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_val_cmd(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_phy_debug_switch(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
void wal_vap_get_wlan_mode_para(wal_msg_write_stru *st_write_msg);
int32_t wal_host_dev_exit(oal_net_device_stru *pst_net_dev);
uint32_t wal_hipriv_setcountry(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint32_t wal_ioctl_set_sta_ps_mode(oal_net_device_stru *pst_cfg_net_dev, int8_t uc_ps_mode);
uint32_t wal_ioctl_set_fast_sleep_para(oal_net_device_stru *pst_cfg_net_dev, int8_t *pc_param);
void wal_set_power_mgmt_on(uint8_t power_mgmt_flag);
int32_t wal_set_power_on(oal_net_device_stru *pst_net_dev, int32_t power_flag);
int32_t wal_ext_priv_cmd(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr);
#ifdef _PRE_WLAN_FEATURE_CSI
uint32_t wal_ioctl_set_csi_switch(oal_net_device_stru *pst_cfg_net_dev, int8_t *pc_param);
#endif
uint32_t wal_ioctl_set_mode(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_ioctl_set_freq(oal_net_device_stru *net_dev, int8_t *pc_param);
#ifdef _PRE_WLAN_FEATURE_MONITOR
uint32_t wal_ioctl_set_sniffer(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_ioctl_set_monitor_mode(oal_net_device_stru *net_dev, int8_t *pc_param);
#endif
uint32_t wal_ioctl_set_al_rx(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_ioctl_set_pm_switch(mac_vap_stru *mac_vap, uint32_t *params);
uint32_t wal_algcmd_char_extra_adapt(oal_net_device_stru *net_dev,
    const char *in_param, signed long in_len, char *out_param, signed long out_len);
uint32_t wal_hipriv_fem_lowpower(oal_net_device_stru *pst_net_dev, int8_t *pc_param);
uint8_t wal_cfg80211_convert_value_to_vht_width(int32_t l_channel_value);
uint32_t wal_hipriv_set_rfch(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_rxch(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_nss(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_hipriv_send_cw_signal(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_hipriv_adjust_ppm(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_hipriv_chip_check(oal_net_device_stru *net_dev, int8_t *pc_param);
uint32_t wal_hipriv_set_txpower(oal_net_device_stru *net_dev, int8_t *pc_param);

#ifdef _PRE_WLAN_FEATURE_NAN
oal_bool_enum_uint8 wal_nan_check_param(mac_nan_param_stru *param);
#endif
#ifdef _PRE_WLAN_FEATURE_HID2D
uint32_t wal_ioctl_set_hid2d_acs_mode(oal_net_device_stru *net_dev, int8_t *param);
uint32_t wal_ioctl_hid2d_link_meas(oal_net_device_stru *net_dev, int8_t *param);
uint32_t wal_ioctl_update_acs_state(oal_net_device_stru *net_dev, int8_t *param);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* end of wal_linux_ioctl.h */
