
#ifndef __MAC_DEVICE_COMMON_H__
#define __MAC_DEVICE_COMMON_H__

#define MAC_DATARATES_PHY_80211G_NUM 12

#define MAC_RX_BA_LUT_BMAP_LEN ((HAL_MAX_RX_BA_LUT_SIZE + 7) >> 3)
#define MAC_TX_BA_LUT_BMAP_LEN ((HAL_MAX_TX_BA_LUT_SIZE + 7) >> 3)

#define MAC_SCAN_CHANNEL_INTERVAL_DEFAULT         6   /* ���6���ŵ����лع����ŵ�����һ��ʱ�� */
#define MAC_WORK_TIME_ON_HOME_CHANNEL_DEFAULT     110 /* ����ɨ��ʱ�����ع����ŵ�������ʱ�� */

#define MAC_MAX_IP_FILTER_BTABLE_SIZE 512 /* rx ip���ݰ����˹��ܵĺ�������С */

/* DMAC SCANNER ɨ��ģʽ */
#define MAC_SCAN_FUNC_MEAS       0x1
#define MAC_SCAN_FUNC_STATS      0x2
#define MAC_SCAN_FUNC_RADAR      0x4 // δ��
#define MAC_SCAN_FUNC_BSS        0x8
#define MAC_SCAN_FUNC_P2P_LISTEN 0x10
#define MAC_SCAN_FUNC_ALL        \
    (MAC_SCAN_FUNC_MEAS | MAC_SCAN_FUNC_STATS | MAC_SCAN_FUNC_RADAR | MAC_SCAN_FUNC_BSS)

/* ɨ������¼�����״̬�� */
typedef enum {
    MAC_SCAN_SUCCESS = 0,    /* ɨ��ɹ� */
    MAC_SCAN_PNO = 1,        /* pnoɨ����� */
    MAC_SCAN_TIMEOUT = 2,    /* ɨ�賬ʱ */
    MAC_SCAN_REFUSED = 3,    /* ɨ�豻�ܾ� */
    MAC_SCAN_ABORT = 4,      /* ��ֹɨ�� */
    MAC_SCAN_ABORT_SYNC = 5, /* ɨ�豻��ֹͬ��״̬�������ϲ�ȥ��������ʱǿ��abort�����ں��ϱ���dmac��Ӧabort���ϱ� */
    MAC_SCAN_STATUS_BUTT,    /* ��Ч״̬�룬��ʼ��ʱʹ�ô�״̬�� */
} mac_scan_status_enum;
typedef uint8_t mac_scan_status_enum_uint8;

typedef enum {
    MAC_CH_TYPE_NONE = 0,
    MAC_CH_TYPE_PRIMARY = 1,
    MAC_CH_TYPE_SECONDARY = 2,
} mac_ch_type_enum;
typedef uint8_t mac_ch_type_enum_uint8;

/* SDT����ģʽö�� */
typedef enum {
    MAC_SDT_MODE_WRITE = 0,
    MAC_SDT_MODE_READ,
    MAC_SDT_MODE_WRITE16,
    MAC_SDT_MODE_READ16,

    MAC_SDT_MODE_BUTT
} mac_sdt_rw_mode_enum;
typedef uint8_t mac_sdt_rw_mode_enum_uint8;

typedef enum {
    MAC_CHAN_NOT_SUPPORT = 0,      /* ������֧�ָ��ŵ� */
    MAC_CHAN_AVAILABLE_ALWAYS,     /* �ŵ�һֱ����ʹ�� */
    MAC_CHAN_AVAILABLE_TO_OPERATE, /* �������(CAC, etc...)�󣬸��ŵ�����ʹ�� */
    MAC_CHAN_DFS_REQUIRED,         /* ���ŵ���Ҫ�����״��� */
    MAC_CHAN_BLOCK_DUE_TO_RADAR,   /* ���ڼ�⵽�״ﵼ�¸��ŵ���Ĳ����� */

    MAC_CHAN_STATUS_BUTT
} mac_chan_status_enum;
typedef uint8_t mac_chan_status_enum_uint8;

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
typedef uint8_t mac_reset_sys_type_enum_uint8;

/* rx ip���ݰ����˵��������� */
typedef enum {
    MAC_IP_FILTER_ENABLE = 0,        /* ��/��ip���ݰ����˹��� */
    MAC_IP_FILTER_UPDATE_BTABLE = 1, /* ���º����� */
    MAC_IP_FILTER_CLEAR = 2,         /* ��������� */

    MAC_IP_FILTER_BUTT
} mac_ip_filter_cmd_enum;
typedef uint8_t mac_ip_filter_cmd_enum_uint8;

/* ��������Ŀ��ʽ */
typedef struct {
    uint16_t us_port; /* Ŀ�Ķ˿ںţ��������ֽ����ʽ�洢 */
    uint8_t uc_protocol;
    uint8_t uc_resv;
    // uint32_t                  filter_cnt;     /* Ŀǰδ����"ͳ�ƹ��˰�����"�����󣬴˳�Ա�ݲ�ʹ�� */
} mac_ip_filter_item_stru;

/* rx����ָ�����ݰ������������� */
typedef enum {
    MAC_ICMP_FILTER = 1, /* icmp���ݰ����˹��� */

    MAC_FILTER_ID_BUTT
} mac_assigned_filter_id_enum;
typedef uint8_t mac_assigned_filter_id_enum_uint8;

/* ָ��filter���������ʽ */
typedef struct {
    mac_assigned_filter_id_enum_uint8 en_filter_id;
    oal_bool_enum_uint8 en_enable; /* �·�����ʹ�ܱ�־ */
    uint8_t uc_resv[NUM_2_BYTES];
} mac_assigned_filter_cmd_stru;

typedef enum {
    MAC_RX_ICMP_FILTER_STOPED = 0,   // ���ܹرգ�δʹ�ܡ���������״����������˶�����
    MAC_RX_ICMP_FILTER_WORKING = 1,  // ���ܴ򿪣����չ�����������
    MAC_RX_ICMP_FILTER_BUTT
} mac_icmp_filter_state_enum;

typedef uint8_t mac_icmp_filter_state_enum_uint8;

typedef struct {
    mac_assigned_filter_id_enum_uint8 en_filter_id;
    mac_icmp_filter_state_enum_uint8 en_state;  // ����״̬�����ˡ��ǹ��˵�
    uint8_t uc_resv[NUM_2_BYTES];
} mac_rx_icmp_filter_struc;

#ifdef _PRE_WLAN_FEATURE_APF
#define APF_PROGRAM_MAX_LEN 512
#define APF_VERSION         2
#endif

#endif /* end of mac_device_common.h */
