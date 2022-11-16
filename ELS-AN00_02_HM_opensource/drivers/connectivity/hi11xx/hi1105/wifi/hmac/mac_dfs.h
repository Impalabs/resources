
#ifndef __MAC_DFS_H__
#define __MAC_DFS_H__

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_DFS_H

#ifdef _PRE_WLAN_FEATURE_DFS
typedef struct {
    oal_bool_enum_uint8 en_dfs_switch; /* DFSʹ�ܿ��� bit0:dfsʹ��,bit1:��ʾAP��ΪDFS������ʱ������wait_start */
    oal_bool_enum_uint8 en_cac_switch;
    oal_bool_enum_uint8 en_offchan_cac_switch;
    uint8_t uc_debug_level;  /* bit0:��ӡ�״��ҵ��bit1:��ӡ�״���ҵ�� */
    uint8_t uc_offchan_flag; /* bit0:0��ʾhomechan,1��ʾoffchan; bit1:0��ʾ��ͨ,1��ʾoffchancac */
    uint8_t uc_offchan_num;
    uint8_t uc_timer_cnt;
    uint8_t uc_timer_end_cnt;
    uint8_t uc_cts_duration;
    uint8_t uc_dmac_channel_flag; /* dmac���ڱ�ʾ��ǰ�ŵ�off or home */
    oal_bool_enum_uint8 en_dfs_init;
    uint8_t uc_custom_next_chnum;                   /* Ӧ�ò�ָ����DFS��һ���ŵ��� */
    uint32_t dfs_cac_outof_5600_to_5650_time_ms; /* CAC���ʱ����5600MHz ~ 5650MHzƵ���⣬Ĭ��60�� */
    uint32_t dfs_cac_in_5600_to_5650_time_ms;    /* CAC���ʱ����5600MHz ~ 5650MHzƵ���ڣ�Ĭ��10���� */
    /* Off-Channel CAC���ʱ����5600MHz ~ 5650MHzƵ���⣬Ĭ��6���� */
    uint32_t off_chan_cac_outof_5600_to_5650_time_ms;
    /* Off-Channel CAC���ʱ����5600MHz ~ 5650MHzƵ���ڣ�Ĭ��60���� */
    uint32_t off_chan_cac_in_5600_to_5650_time_ms;
    uint16_t us_dfs_off_chan_cac_opern_chan_dwell_time; /* Off-channel CAC�ڹ����ŵ���פ��ʱ�� */
    uint16_t us_dfs_off_chan_cac_off_chan_dwell_time;   /* Off-channel CAC��Off-Channel�ŵ���פ��ʱ�� */
    uint32_t radar_detected_timestamp;
    int32_t l_radar_th;                  // ���õ��״������ޣ���λdb
    uint32_t custom_chanlist_bitmap;  // Ӧ�ò�ͬ���������ŵ��б�
    // (0x0000000F) /* 36--48 */
    // (0x000000F0) /* 52--64 */
    // (0x000FFF00) /* 100--144 */
    // (0x01F00000) /* 149--165 */
    wlan_channel_bandwidth_enum_uint8 en_next_ch_width_type;  // ���õ���һ���ŵ��Ĵ���ģʽ
    uint8_t uac_resv[NUM_3_BYTES];
    uint32_t dfs_non_occupancy_period_time_ms;
    uint8_t uc_radar_type;
    uint8_t uc_band;
    uint8_t uc_channel_num;
    uint8_t uc_flag;
} mac_dfs_info_stru;

typedef struct {
    frw_timeout_stru st_dfs_cac_timer;          /* CAC��ʱ�� */
    frw_timeout_stru st_dfs_off_chan_cac_timer; /* Off-Channel CAC��ʱ�� */
    frw_timeout_stru st_dfs_chan_dwell_timer;   /* �ŵ�פ����ʱ������ʱ�����ڣ�������ŵ� */
    frw_timeout_stru st_dfs_radar_timer;
    mac_dfs_info_stru st_dfs_info;
    oal_dlist_head_stru st_dfs_nol;
} mac_dfs_core_stru;
#endif

#endif /* end of mac_dfs.h */
