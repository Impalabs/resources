/*
 * ��Ȩ���� (c) ��Ϊ�������޹�˾ 2020-2020
 * ����˵�� :
 * �������� : 2020��6��17��
 */
#include "wal_cfg_ioctl.h"
#include "oal_types.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "mac_vap.h"
#include "wal_config.h"
#include "hmac_package_params.h"
#include "hmac_vap.h"
#include "mac_mib.h"
#include "hmac_scan.h"
#include "oal_util.h"
#include "plat_pm_wlan.h"
#include "mac_device.h"
#include "hmac_auto_adjust_freq.h"
#include "hmac_hid2d.h"
#include "hmac_resource.h"
#include "wal_linux_ioctl.h"
#ifdef _PRE_WLAN_FEATURE_PMF
#include "hmac_11w.h"
#endif
#ifdef _PRE_WLAN_FEATURE_DFR
#include "wal_dfx.h"
#include "hmac_dfx.h"
#endif
#include "hmac_blacklist.h"
#ifdef _PRE_WLAN_TCP_OPT
#include "hmac_tcp_opt.h"
#endif
#include "hmac_roam_main.h"
#ifdef _PRE_WLAN_FEATURE_WMMAC
#include "hmac_wmmac.h"
#endif
#include "hmac_tx_switch.h"
#include "wlan_chip_i.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_CFG_IOCTL_C
uint32_t wal_get_host_cmd_table_size(void);
host_cmd_stru *wal_get_cmd_info(uint32_t cmd_id);

#ifdef _PRE_WLAN_FEATURE_HID2D_TX_DROP
/* host������ */
OAL_STATIC uint32_t wal_set_hid2d_debug_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_vap_stru *hmac_vap;
    uint32_t params_index = 0;
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_process_hid2d_debug_switch::hmac_vap is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    g_st_hid2d_debug_switch.uc_hid2d_debug_en = (uint8_t)params[params_index++];
    g_st_hid2d_debug_switch.uc_hid2d_delay_time = (uint8_t)params[params_index++];
    hmac_vap->en_is_hid2d_state = g_st_hid2d_debug_switch.uc_hid2d_debug_en;
    oam_warning_log2(mac_vap->uc_vap_id, OAM_SF_CFG, "{set_hid2d_debug_switch::ENABLE LEVEL:%d,delay time %d.}",
        g_st_hid2d_debug_switch.uc_hid2d_debug_en, g_st_hid2d_debug_switch.uc_hid2d_delay_time);
    return OAL_SUCC;
}
OAL_STATIC uint32_t wal_set_hid2d_state(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_vap_stru *hmac_vap;
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_set_hid2d_state::hmac_vap is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    hmac_vap->en_is_hid2d_state = (uint8_t)params[0];
    return OAL_SUCC;
}
#endif
OAL_STATIC uint32_t wal_set_bgscan_type(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t bgscan_state; // ö�ٶ���hmac_scan_state_enum
    bgscan_state = (uint8_t)params[0];
    return hmac_bgscan_enable(mac_vap, sizeof(bgscan_state), &bgscan_state);
}

OAL_STATIC uint32_t wal_set_random_mac_addr_scan(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_bool_enum_uint8 rand_mac_addr_scan_switch;
    rand_mac_addr_scan_switch = (oal_bool_enum_uint8)params[0];
    return hmac_config_set_random_mac_addr_scan(mac_vap, sizeof(rand_mac_addr_scan_switch),
                                                &rand_mac_addr_scan_switch);
}

OAL_STATIC uint32_t wal_set_2040_coext_support(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t csp;
    csp = (uint8_t)params[0];
    return hmac_config_set_2040_coext_support(mac_vap, sizeof(csp), &csp);
}

OAL_STATIC uint32_t wal_set_ota_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    oam_ota_switch_param_stru ota_swicth;
    ota_swicth.ota_type = (uint8_t)params[0];;
    ota_swicth.ota_switch = (uint8_t)params[1];
    hmac_config_ota_switch(&ota_swicth);

    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_PHY_EVENT_INFO
OAL_STATIC uint32_t wal_hipriv_phy_event_rpt(mac_vap_stru *mac_vap, uint32_t *params)
{
    mac_cfg_phy_event_rpt_stru phy_event_cfg;
    phy_event_cfg.event_rpt_en = params[0];
    phy_event_cfg.wp_mem_num = params[1];
    phy_event_cfg.wp_event0_type_sel = params[BIT_OFFSET_2];
    phy_event_cfg.wp_event1_type_sel = params[BIT_OFFSET_3];
    oam_warning_log4(mac_vap->uc_vap_id, OAM_SF_CFG,
        "wal_hipriv_phy_event_rpt en[%d] mem num[%d] event0 sel[%x] event1 sel[%x]", phy_event_cfg.event_rpt_en,
        phy_event_cfg.wp_mem_num, phy_event_cfg.wp_event0_type_sel, phy_event_cfg.wp_event1_type_sel);
    return OAL_SUCC;
}
#endif

OAL_STATIC uint32_t wal_set_sta_ps_mode(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_vap_stru *hmac_vap = NULL;

    hmac_vap = mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (oal_unlikely(hmac_vap == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }
    hmac_vap->uc_ps_mode = (uint8_t)params[0];
    oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_PWR,
                     "wal_set_sta_ps_mode ps_mode[%d]",
                     hmac_vap->uc_ps_mode);

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    if ((hmac_vap->uc_ps_mode == MAX_FAST_PS) || (hmac_vap->uc_ps_mode == AUTO_FAST_PS)) {
        wlan_pm_set_timeout((g_wlan_min_fast_ps_idle > 1) ? (g_wlan_min_fast_ps_idle - 1) : g_wlan_min_fast_ps_idle);
    } else {
#endif

        wlan_pm_set_timeout(WLAN_SLEEP_DEFAULT_CHECK_CNT);

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    }
#endif
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_sta_ps_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    if (mac_vap->en_vap_mode == WLAN_VAP_MODE_BSS_AP) {
        oam_warning_log0(0, OAM_SF_PWR, "{wal_set_sta_ps_info::ap mode has no pm fsm.}");
        return OAL_FAIL;
    }
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_uapsd_cap(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t en_uapsd = (uint8_t)params[0];
    /* ����mibֵ */
    mac_vap_set_uapsd_en(mac_vap, en_uapsd);
    g_uc_uapsd_cap = en_uapsd;
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_uapsd_para(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t idx;
    mac_cfg_uapsd_sta_stru uapsd_param;

    uapsd_param.uc_max_sp_len = (uint8_t)params[0];
    if (uapsd_param.uc_max_sp_len > 6) { /* uc_max_sp_len���Ϊ6 */
        oam_error_log1(mac_vap->uc_vap_id, OAM_SF_PWR,
            "{wal_set_uapsd_para::uc_max_sp_len[%d] > 6!}", uapsd_param.uc_max_sp_len);
        return OAL_FAIL;
    }

    for (idx = 0; idx < WLAN_WME_AC_BUTT; idx++) {
        uapsd_param.uc_delivery_enabled[idx] = (uint8_t)params[idx + 1];
        uapsd_param.uc_trigger_enabled[idx] = (uint8_t)params[idx + 1];
    }
    mac_vap_set_uapsd_para(mac_vap, &uapsd_param);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_p2p_scenes(mac_vap_stru *mac_vap, uint32_t *params)
{
    mac_device_stru *mac_device = NULL;

    mac_device = mac_res_get_dev(mac_vap->uc_device_id);
    if (mac_device == NULL) {
        oam_warning_log0(mac_vap->uc_vap_id, OAM_SF_ANY, "wal_set_p2p_scenes:: mac_device is NULL");
        return OAL_ERR_CODE_PTR_NULL;
    }
    mac_device->st_p2p_info.p2p_scenes = (uint8_t)params[0];

    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_voe_enable(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_vap_stru *hmac_vap;
    oal_bool_enum_uint8 en_read_flag;
    uint16_t voe_switch = (uint16_t)params[0];

    hmac_vap = mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_voe_enable::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    en_read_flag = (voe_switch & BIT7) ? OAL_TRUE : OAL_FALSE;
    if (en_read_flag == OAL_TRUE) {
        oam_warning_log3(mac_vap->uc_vap_id, OAM_SF_CFG,
                         "{wal_set_voe_enable::custom_11k=[%d],custom_11v=[%d],custom_11r=[%d].}",
                         g_st_mac_device_custom_cfg.st_voe_custom_cfg.en_11k,
                         g_st_mac_device_custom_cfg.st_voe_custom_cfg.en_11v,
                         g_st_mac_device_custom_cfg.st_voe_custom_cfg.en_11r);
#if defined(_PRE_WLAN_FEATURE_11K) || defined(_PRE_WLAN_FEATURE_11R)
        oam_warning_log3(mac_vap->uc_vap_id, OAM_SF_CFG,
                         "{wal_set_voe_enable::11k=[%d],11v=[%d],11r=[%d].}",
                         hmac_vap->bit_11k_enable,
                         hmac_vap->bit_11v_enable,
                         hmac_vap->bit_11r_enable);
#endif
        return OAL_SUCC;
    } else {
#if defined(_PRE_WLAN_FEATURE_11K) || defined(_PRE_WLAN_FEATURE_11R)
        hmac_vap->bit_11r_enable = (voe_switch & BIT0) ? OAL_TRUE : OAL_FALSE;
        hmac_vap->bit_11v_enable = (voe_switch & BIT1) ? OAL_TRUE : OAL_FALSE;
        hmac_vap->bit_11k_enable = (voe_switch & BIT2) ? OAL_TRUE : OAL_FALSE;
        hmac_vap->bit_11k_auth_flag = (voe_switch & BIT3) ? OAL_TRUE : OAL_FALSE;
        hmac_vap->bit_voe_11r_auth = (voe_switch & BIT4) ? OAL_TRUE : OAL_FALSE;
        hmac_vap->bit_11k_auth_oper_class = ((voe_switch >> BIT_OFFSET_5) & 0x3);
        hmac_vap->bit_11r_over_ds = ((voe_switch >> BIT_OFFSET_8) & BIT0) ? OAL_TRUE : OAL_FALSE;

        oam_warning_log2(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_voe_enable::uc_param0 = [0x%X],uc_param1 = [0x%X].}",
                         (voe_switch & 0xFF), ((voe_switch >> BIT_OFFSET_8) & 0xFF));
#endif
    }

    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_NEGTIVE_DET

OAL_STATIC uint32_t wal_set_pk_mode_debug(mac_vap_stru *mac_vap, uint32_t *params)
{
    /* pkmode���޵����ӿ� hipriv "wlan0 pk_mode_debug 0/1(high/low) 0/1/2/3/4(BW) 0/1/2/3(protocol) ��������ֵ" */
    /*
        BW:20M     40M    80M   160M   80+80M
        protocol:lagency: HT: VHT: HE:
    */
    /*
    PKģʽ���޻���:
    {(��λMbps)  20M     40M    80M   160M   80+80M
    lagency:    {valid, valid, valid, valid, valid},   (����Э��ģʽû��pk mode )
    HT:         {72,    150,   valid, valid, valid},
    VHT:        {86,    200,   433,   866,   866},
    HE:         {valid, valid, valid, valid, valid},   (�ݲ�֧��11ax��pk mode)
    };

    PKģʽ��������:
    �ߵ�λ����: g_st_pk_mode_high_th_table = PKģʽ���޻��� * 70% *1024 *1024 /8  (��λ�ֽ�)
    �͵�λ����: g_st_pk_mode_low_th_table  = PKģʽ���޻��� * 20% *1024 *1024 /8  (��λ�ֽ�)

    */
    oam_warning_log4(0, OAM_SF_CFG, "{wal_set_pk_mode_debug::set high/low = %u, BW = %u, pro = %u, valid = %u!}",
                     params[BIT_OFFSET_0], params[BIT_OFFSET_1], params[BIT_OFFSET_2], params[BIT_OFFSET_3]);

    if (params[0] == 2) { /* 2��ʾpk_mode_debug high */
        g_en_pk_mode_swtich = OAL_TRUE;
        return OAL_SUCC;
    } else if (params[0] == 3) { /* 3��ʾpk_mode_debug low */
        g_en_pk_mode_swtich = OAL_FALSE;
        return OAL_SUCC;
    }

    if (params[0] == 0) {
        oam_warning_log2(0, OAM_SF_CFG, "{wal_set_pk_mode_debug::set high th (%u) -> (%u)!}",
                         g_st_pk_mode_high_th_table[params[BIT_OFFSET_2]][params[BIT_OFFSET_1]],
                         params[BIT_OFFSET_3]);
        g_st_pk_mode_high_th_table[params[BIT_OFFSET_2]][params[BIT_OFFSET_1]] = params[BIT_OFFSET_3];
        return OAL_SUCC;
    }

    oam_warning_log2(0, OAM_SF_CFG, "{wal_set_pk_mode_debug::set low th (%u) -> (%u)!}",
                     g_st_pk_mode_low_th_table[params[BIT_OFFSET_2]][params[BIT_OFFSET_1]],
                     params[BIT_OFFSET_3]);

    g_st_pk_mode_low_th_table[params[BIT_OFFSET_2]][params[BIT_OFFSET_1]] = params[BIT_OFFSET_3];

    return OAL_SUCC;
}
#endif


OAL_STATIC uint32_t wal_set_remove_app_ie(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_vap_stru *hmac_vap = NULL;

    hmac_vap = mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_remove_app_ie::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    hmac_vap->st_remove_ie.uc_type = (uint8_t)params[0];
    hmac_vap->st_remove_ie.uc_eid = (uint8_t)params[1];

    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_HID2D
#ifdef _PRE_WLAN_FEATURE_HID2D_PRESENTATION

OAL_STATIC uint32_t wal_set_hid2d_scan_channel(mac_vap_stru *mac_vap, uint32_t *params)
{
    mac_regdomain_info_stru *regdomain_info = NULL;
    mac_device_stru *mac_device = NULL;
    uint32_t ret;

    /* ������ڷ����᳡����ֱ���˳���������ɨ�� */
    mac_device = mac_res_get_dev(mac_vap->uc_device_id);
    if (mac_device == NULL) {
        return OAL_SUCC;
    }
    mac_get_regdomain_info(&regdomain_info);
    if (mac_device->is_presentation_mode != OAL_TRUE ||
        regdomain_info->ac_country[0] != '9' || regdomain_info->ac_country[1] != '9') {
        oam_error_log0(0, OAM_SF_DFS, "{HiD2D Presentation: Not in Presentation Mode, Can not scan the full band!!!}");
        return OAL_SUCC;
    }

    /* ��mac_device�е�is_ready_to_get_scan_result����ΪOAL_FALSE */
    mac_device->is_ready_to_get_scan_result = OAL_FALSE;

    /* ����ɨ�� */
    ret = hmac_hid2d_scan_chan_start(mac_vap, mac_device);
    if (oal_unlikely(ret != OAL_SUCC)) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_hid2d_scan_channel: error}");
    }
    return ret;
}
#endif
#endif

OAL_STATIC uint32_t wal_set_global_log_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    oam_log_set_global_switch((oal_switch_enum_uint8)params[0]);
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_pm_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    return wal_ioctl_set_pm_switch(mac_vap, params);
}


OAL_STATIC uint32_t wal_set_ota_beacon_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    oam_sdt_print_beacon_rxdscr_type_enum_uint8 value = (oam_sdt_print_beacon_rxdscr_type_enum_uint8)params[0];
    /* �˴�Ϊ����0305, 06��ʹ�� */
    oam_ota_set_switch(OAM_OTA_SWITCH_BEACON, value);

    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_WMMAC

OAL_STATIC uint32_t wal_set_addts_req(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t idx = 0;
    hmac_user_stru *hmac_user = NULL;
    hmac_vap_stru *hmac_vap = NULL;
    mac_wmm_tspec_stru addts_args;
    uint8_t ac;
    /*
     * ���÷���ADDTS REQ��������:
     * hipriv "vap0 addts_req tid direction psb up nominal_msdu_size maximum_data_rate
     * minimum_data_rate mean_data_rate peak_data_rate minimum_phy_rate surplus_bandwidth_allowance"
     */
    /***********************************************************************************************
    TSPEC�ֶ�:
          --------------------------------------------------------------------------------------
          |TS Info|Nominal MSDU Size|Max MSDU Size|Min Serv Itvl|Max Serv Itvl|
          ---------------------------------------------------------------------------------------
    Octets:  | 3     |  2              |   2         |4            |4            |
          ---------------------------------------------------------------------------------------
          | Inactivity Itvl | Suspension Itvl | Serv Start Time |Min Data Rate | Mean Data Rate |
          ---------------------------------------------------------------------------------------
    Octets:  |4                | 4               | 4               |4             |  4             |
          ---------------------------------------------------------------------------------------
          |Peak Data Rate|Burst Size|Delay Bound|Min PHY Rate|Surplus BW Allowance  |Medium Time|
          ---------------------------------------------------------------------------------------
    Octets:  |4             |4         | 4         | 4          |  2                   |2          |
          ---------------------------------------------------------------------------------------

    TS info�ֶ�:
          ---------------------------------------------------------------------------------------
          |Reserved |TSID |Direction |1 |0 |Reserved |PSB |UP |Reserved |Reserved |Reserved |
          ---------------------------------------------------------------------------------------
    Bits:  |1        |4    |2         |  2  |1        |1   |3  |2        |1        |7        |
          ----------------------------------------------------------------------------------------
    ***********************************************************************************************/
    if (g_en_wmmac_switch == OAL_FALSE) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_WMMAC,
            "{wal_set_addts_req::wmmac switch is false[%d].}", g_en_wmmac_switch);
        return OAL_SUCC;
    }
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_WMMAC, "{wal_set_addts_req::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡ�û���Ӧ������ */
    hmac_user = (hmac_user_stru *)mac_res_get_hmac_user(mac_vap->us_assoc_vap_id);
    if (hmac_user == NULL) {
        oam_warning_log0(mac_vap->uc_vap_id, OAM_SF_WMMAC, "{wal_set_addts_req::pst_hmac_user null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* 0.��ȡtid��ȡֵ��Χ0~7 */
    addts_args.ts_info.bit_tsid = (uint16_t)params[idx++];

    /* 1.��ȡdirection 00:uplink 01:downlink 10:reserved 11:Bi-directional */
    addts_args.ts_info.bit_direction = (uint16_t)params[idx++];
    if (addts_args.ts_info.bit_direction == MAC_WMMAC_DIRECTION_RESERVED) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_set_addts_req::the direction is not correct! direction is[%d]!}\r\n",
            addts_args.ts_info.bit_direction);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }
    /* 2.��ȡPSB��1��ʾU-APSD��0��ʾlegacy */
    addts_args.ts_info.bit_apsd = (uint16_t)params[idx++];

    /* 3.��ȡUP */
    addts_args.ts_info.bit_user_prio = (uint16_t)params[idx++];

    /* 4.��ȡNominal MSDU Size ,��һλΪ1 */
    /*
        ------------
        |fixed|size|
        ------------
    bits:  |1    |15  |
        ------------
    */
    addts_args.us_norminal_msdu_size = (uint16_t)params[idx++];

    /* 5.��ȡmaximum MSDU size */
    addts_args.us_max_msdu_size = (uint16_t)params[idx++];

    /* 6.��ȡminimum data rate */
    addts_args.min_data_rate = (uint32_t)params[idx++];

    /* 7.��ȡmean data rate */
    addts_args.mean_data_rate = (uint32_t)params[idx++];

    /* 8.��ȡpeak data rate */
    addts_args.peak_data_rate = (uint32_t)params[idx++];

    /* 9.��ȡminimum PHY Rate */
    addts_args.min_phy_rate = (uint32_t)params[idx++];

    /* 10��ȡsurplus bandwidth allowance */
    addts_args.us_surplus_bw = (uint16_t)params[idx++];

    /* �ж϶�ӦAC��ACMλ��ֻ�и�AC��ACMΪ1ʱ����������TS�� */
    ac = WLAN_WME_TID_TO_AC(addts_args.ts_info.bit_user_prio);
    if (OAL_FALSE == mac_mib_get_QAPEDCATableMandatory(&(hmac_vap->st_vap_base_info), ac)) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_WMMAC, "{wal_set_addts_req::user prio[%d] is false.}", ac);
        return OAL_SUCC;
    }
    /* ����ADDTS REQ������TS */
    return hmac_mgmt_tx_addts_req(hmac_vap, hmac_user, &addts_args);
}


OAL_STATIC uint32_t wal_set_delts(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_user_stru *hmac_user = NULL;
    hmac_vap_stru *hmac_vap = NULL;
    mac_wmm_tspec_stru delts_args;

    delts_args.ts_info.bit_tsid = (uint8_t)params[0];

    if (g_en_wmmac_switch == OAL_FALSE) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_WMMAC,
            "{wal_set_delts::wmmac switch is false[%d].}", g_en_wmmac_switch);
        return OAL_SUCC;
    }
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_WMMAC, "{wal_set_delts::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡ�û���Ӧ������ */
    hmac_user = (hmac_user_stru *)mac_res_get_hmac_user(mac_vap->us_assoc_vap_id);
    if (hmac_user == NULL) {
        oam_warning_log0(mac_vap->uc_vap_id, OAM_SF_WMMAC, "{wal_set_delts::pst_hmac_user null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_mgmt_tx_delts(hmac_vap, hmac_user, &delts_args);
}
#endif


OAL_STATIC uint32_t wal_set_mem_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_mem_pool_id_enum_uint8 pool_id;
    /* ��ȡ�ڴ��ID */
    pool_id = (oal_mem_pool_id_enum_uint8)params[0];

    /* ��ӡ�ڴ����Ϣ */
    oal_mem_info(pool_id);

    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_mem_leak(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_mem_pool_id_enum_uint8 pool_id;
    /* ��ȡ�ڴ��ID */
    pool_id = (oal_mem_pool_id_enum_uint8)params[0];

    /* ����ڴ��й©�ڴ�� */
    oal_mem_leak(pool_id);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_show_stat_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    oam_stats_report_info_to_sdt(OAM_OTA_TYPE_DEV_STAT_INFO);
    oam_stats_report_info_to_sdt(OAM_OTA_TYPE_VAP_STAT_INFO);
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_clear_stat_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    oam_stats_clear_stat_info();
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_user_stat_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint16_t user_id = (uint16_t)params[0];
    oam_stats_report_usr_info(user_id);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_list_channel(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_config_list_channel(mac_vap);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_event_queue_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    return frw_event_queue_info();
}
#ifdef _PRE_WLAN_DFT_STAT

OAL_STATIC uint32_t wal_set_clear_vap_stat_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    oam_stats_clear_vap_stat_info(mac_vap->uc_vap_id);
    return OAL_SUCC;
}
#endif


OAL_STATIC uint32_t wal_get_flowctl_stat(mac_vap_stru *mac_vap, uint32_t *params)
{
    /* ����host flowctl ��ز��� */
    hcc_host_get_flowctl_stat();
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_start_deauth(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_config_send_deauth(mac_vap, mac_vap->auc_bssid);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_all_ether_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    /* sh hipriv.sh "Hisilicon0 ether_all 0|1(֡����tx|rx) 0|1(����)" */
    uint8_t frame_direction = (uint8_t)params[0];
    uint8_t ether_switch = (uint8_t)params[1];
    uint16_t user_num;

    /* ���ÿ��� */
    for (user_num = 0; user_num < WLAN_USER_MAX_USER_LIMIT; user_num++) {
        oam_report_eth_frame_set_switch(user_num, ether_switch, frame_direction);
    }
    /* ͬʱ���ù㲥arp dhcp֡���ϱ����� */
    oam_report_dhcp_arp_set_switch(ether_switch);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_dhcp_arp_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_switch_enum_uint8 dhcp_arp_switch = (oal_switch_enum_uint8)params[0];

    oam_report_dhcp_arp_set_switch(dhcp_arp_switch);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_frag_threshold(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint32_t frag_threshold = params[0]; /* ��ΧWLAN_FRAG_THRESHOLD_MIN ~ WLAN_FRAG_THRESHOLD_MAX */
    if (mac_vap->pst_mib_info == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_frag_threshold:pst_mib_info is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    mac_mib_set_FragmentationThreshold(mac_vap, frag_threshold);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_80211_mcast_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    mac_cfg_80211_mcast_switch_stru mcast_switch = { 0 };
    oam_80211_frame_ctx_union oam_switch_param = { 0 };
    uint8_t idx = 0;
    uint32_t ret;
    /*
     * sh hipriv.sh "Hisilicon0 80211_mc_switch 0|1(֡����tx|rx) 0|1(֡����:����֡|����֡)
     * 0|1(֡���ݿ���) 0|1(CB����) 0|1(����������)"
     */
    mcast_switch.en_frame_direction = (uint8_t)params[idx++]; /* ��ȡ80211֡���� */
    mcast_switch.en_frame_type      = (uint8_t)params[idx++]; /* ��ȡ֡���� */
    mcast_switch.en_frame_switch    = (uint8_t)params[idx++]; /* ��ȡ֡���ݴ�ӡ���� */
    mcast_switch.en_cb_switch       = (uint8_t)params[idx++]; /* ��ȡ֡CB�ֶδ�ӡ���� */
    mcast_switch.en_dscr_switch     = (uint8_t)params[idx++]; /* ��ȡ��������ӡ���� */

    /* ����������ṹת��ΪOAM�ṹ */
    oam_switch_param.frame_ctx.bit_content    = mcast_switch.en_frame_switch;
    oam_switch_param.frame_ctx.bit_cb         = mcast_switch.en_cb_switch;
    oam_switch_param.frame_ctx.bit_dscr       = mcast_switch.en_dscr_switch;

    ret = oam_report_80211_mcast_set_switch(mcast_switch.en_frame_direction,
                                            mcast_switch.en_frame_type,
                                            &oam_switch_param);
    if (ret != OAL_SUCC) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_CFG,
                         "{wal_set_80211_mcast_switch::oam_report_80211_mcast_set_switch failed[%d].}", ret);
        return ret;
    }
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_list_sta(mac_vap_stru *mac_vap, uint32_t *params)
{
    return hmac_config_list_sta(mac_vap);
}

OAL_STATIC uint32_t wal_set_probe_switch(mac_vap_stru *mac_vap, uint32_t *params)
{
    mac_cfg_probe_switch_stru probe_switch = { 0 };
    oam_80211_frame_ctx_union oam_switch_param = { 0 };
    uint8_t idx = 0;
    uint32_t ret;
    /*
     * sh hipriv.sh "Hisilicon0 probe_switch 0|1(֡����tx|rx) 0|1(֡���ݿ���)
     * 0|1(CB����) 0|1(����������)"
     */
    probe_switch.en_frame_direction = (uint8_t)params[idx++]; /* ��ȡ֡���� */
    probe_switch.en_frame_switch    = (uint8_t)params[idx++]; /* ��ȡ֡���ݴ�ӡ���� */
    probe_switch.en_cb_switch       = (uint8_t)params[idx++]; /* ��ȡ֡CB�ֶδ�ӡ���� */
    probe_switch.en_dscr_switch     = (uint8_t)params[idx++]; /* ��ȡ��������ӡ���� */

    /* ����������ṹת��ΪOAM�ṹ */
    oam_switch_param.frame_ctx.bit_content    = probe_switch.en_frame_switch;
    oam_switch_param.frame_ctx.bit_cb         = probe_switch.en_cb_switch;
    oam_switch_param.frame_ctx.bit_dscr       = probe_switch.en_dscr_switch;

    ret = oam_report_80211_probe_set_switch(probe_switch.en_frame_direction, &oam_switch_param);
    if (ret != OAL_SUCC) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_CFG,
                         "{wal_set_probe_switch::oam_report_80211_probe_set_switch failed[%d].}", ret);
        return ret;
    }
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_stbc_cap(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_bool_enum_uint8 cap_value = (oal_bool_enum_uint8)params[0];

    if (oal_unlikely(mac_vap->pst_mib_info == NULL)) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_stbc_cap::pst_mac_vap->pst_mib_info null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* siso����ʱ�����������stbc��TX���� */
    if (mac_vap->en_vap_rx_nss >= WLAN_DOUBLE_NSS) {
        mac_mib_set_TxSTBCOptionImplemented(mac_vap, cap_value);
        mac_mib_set_TxSTBCOptionActivated(mac_vap, cap_value);
        mac_mib_set_VHTTxSTBCOptionImplemented(mac_vap, cap_value);
        mac_mib_set_he_STBCTxBelow80M(mac_vap, cap_value);
    } else {
        mac_mib_set_TxSTBCOptionImplemented(mac_vap, OAL_FALSE);
        mac_mib_set_TxSTBCOptionActivated(mac_vap, OAL_FALSE);
        mac_mib_set_VHTTxSTBCOptionImplemented(mac_vap, OAL_FALSE);
        mac_mib_set_he_STBCTxBelow80M(mac_vap, OAL_FALSE);
    }

    mac_mib_set_RxSTBCOptionImplemented(mac_vap, cap_value);
    mac_mib_set_VHTRxSTBCOptionImplemented(mac_vap, cap_value);
    mac_mib_set_he_STBCRxBelow80M(mac_vap, cap_value);

    oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_stbc_cap::set stbc cap [%d].}", cap_value);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_all_ota(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_switch_enum_uint8 all_ota_switch = (oal_switch_enum_uint8)params[0];

    oam_report_set_all_switch(all_ota_switch);
    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_TXOPPS

OAL_STATIC uint32_t wal_set_txop_ps_machw(mac_vap_stru *mac_vap, uint32_t *params)
{
    /* sh hipriv.sh "stavap_name txopps_hw_en 0|1(txop_ps_en) 0|1(condition1) 0|1(condition2)" */
    /* ��ȡtxop psʹ�ܿ��� */
    oal_switch_enum_uint8 machw_txopps_en = (oal_switch_enum_uint8)params[0];
    if (mac_vap->pst_mib_info == NULL) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_TXOP,
                         "{wal_set_txop_ps_machw::mib info is null, vap mode[%d].}", mac_vap->en_vap_mode);
        return OAL_FAIL;
    }
    /* 110x txopps�˲��Խӿ�ͨ������mib�����򿪹��ܣ��Ĵ����Ĵ��ں����߼��ж���ִ�� */
    mac_mib_set_txopps(mac_vap, machw_txopps_en);
    return OAL_SUCC;
}
#endif

OAL_STATIC uint32_t wal_set_ldpc_cap(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_bool_enum_uint8 cap_value = (oal_bool_enum_uint8)params[0];

    if (oal_unlikely(mac_vap->pst_mib_info == NULL)) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_ldpc_cap::pst_mac_vap->pst_mib_info null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (cap_value > 1) {
        oam_error_log1(mac_vap->uc_vap_id, OAM_SF_CFG,
            "{wal_set_ldpc_cap::ldpc_value is limit! value = [%d].}", cap_value);
        return OAL_ERR_CODE_CONFIG_EXCEED_SPEC;
    }
    mac_mib_set_LDPCCodingOptionImplemented(mac_vap, cap_value);
    mac_mib_set_LDPCCodingOptionActivated(mac_vap, cap_value);
    mac_mib_set_VHTLDPCCodingOptionImplemented(mac_vap, cap_value);
#ifdef _PRE_WLAN_FEATURE_11AX
    if (g_wlan_spec_cfg->feature_11ax_is_open) {
        mac_mib_set_he_LDPCCodingInPayload(mac_vap, cap_value);
    }
#endif
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_txbf_cap(mac_vap_stru *mac_vap, uint32_t *params)
{
    /************************************************************
      TXBF���ÿ��ص�����: sh hipriv "vap0 set_txbf_cap 0 | 1 | 2 |3"
             bit0��ʾRX(bfee)����     bit1��ʾTX(bfer)����
    *************************************************************/
    mac_device_stru *mac_device = NULL;
    uint8_t cap_value = (uint8_t)params[0];
    uint8_t rx_sts_num;
    oal_bool_enum_uint8 rx_switch;
    oal_bool_enum_uint8 tx_switch;

    if (oal_unlikely(mac_vap->pst_mib_info == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    mac_device = mac_res_get_dev(mac_vap->uc_device_id);
    if (mac_device == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    rx_switch = cap_value & 0x1;
    tx_switch = (cap_value & 0x2) ? OAL_TRUE : OAL_FALSE;
    rx_sts_num = (rx_switch == OAL_TRUE) ? VHT_BFEE_NTX_SUPP_STS_CAP : 1;

    /* siso����ʱ�����������txbf��TX���� */
    if (mac_vap->en_vap_rx_nss >= WLAN_DOUBLE_NSS) {
#ifdef _PRE_WLAN_FEATURE_TXBF_HT
        mac_mib_set_TransmitStaggerSoundingOptionImplemented(mac_vap, tx_switch);
        mac_vap->st_txbf_add_cap.bit_exp_comp_txbf_cap = tx_switch;
#endif
        mac_mib_set_VHTSUBeamformerOptionImplemented(mac_vap, tx_switch);
        mac_mib_set_VHTNumberSoundingDimensions(mac_vap, MAC_DEVICE_GET_NSS_NUM(mac_device));
    } else {
        mac_mib_set_TransmitStaggerSoundingOptionImplemented(mac_vap, OAL_FALSE);
        mac_mib_set_VHTSUBeamformerOptionImplemented(mac_vap, OAL_FALSE);
        mac_mib_set_VHTNumberSoundingDimensions(mac_vap, WLAN_SINGLE_NSS);
    }

#ifdef _PRE_WLAN_FEATURE_TXBF_HT
    mac_mib_set_ReceiveStaggerSoundingOptionImplemented(mac_vap, rx_switch);
    if (rx_switch == OAL_FALSE) {
        mac_mib_set_NumberCompressedBeamformingMatrixSupportAntenna(mac_vap, rx_sts_num);
    }
    mac_mib_set_ExplicitCompressedBeamformingFeedbackOptionImplemented(mac_vap, rx_switch & WLAN_MIB_HT_ECBF_DELAYED);
    mac_vap->st_txbf_add_cap.bit_channel_est_cap = rx_switch;
#endif
    mac_mib_set_VHTSUBeamformeeOptionImplemented(mac_vap, rx_switch);
    if (rx_switch == OAL_FALSE) {
        mac_mib_set_VHTBeamformeeNTxSupport(mac_vap, rx_sts_num);
    }
#ifdef _PRE_WLAN_FEATURE_11AX
    if (g_wlan_spec_cfg->feature_11ax_is_open) {
        mac_mib_set_he_SUBeamformee(mac_vap, rx_switch);
    }
#endif
    oam_warning_log3(0, 0, "{wal_set_txbf_cap::rx_cap[%d], tx_cap[%d], rx_sts_nums[%d].}",
        rx_switch, tx_switch, rx_sts_num);

    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_PMF

OAL_STATIC uint32_t wal_set_enable_pmf(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t pmf_status = (uint8_t)params[0];
    /* ����һ��hmac��оƬ��֤���� */
    hmac_enable_pmf(mac_vap, &pmf_status);
    return OAL_SUCC;
}
#endif
#ifdef _PRE_WLAN_FEATURE_DFR

OAL_STATIC uint32_t wal_test_dfr_start(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint32_t cfg_rst;
    mac_device_stru *mac_device = mac_res_get_dev(mac_vap->uc_device_id);
    if (mac_device == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_test_dfr_start::mac_device is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if ((!g_st_dfr_info.bit_device_reset_enable) || g_st_dfr_info.bit_device_reset_process_flag) {
        oam_warning_log2(0, OAM_SF_ANY,
            "{wal_test_dfr_start::now DFR disabled or in DFR process, enable=%d, reset_flag=%d}",
            g_st_dfr_info.bit_device_reset_enable, g_st_dfr_info.bit_device_reset_process_flag);
        return OAL_ERR_CODE_RESET_INPROGRESS;
    }

    g_st_dfr_info.bit_device_reset_enable = OAL_TRUE;
    g_st_dfr_info.bit_device_reset_process_flag = OAL_FALSE;
    g_st_dfr_info.netdev_num = 0;
    memset_s((uint8_t *)(g_st_dfr_info.past_netdev),
             sizeof(g_st_dfr_info.past_netdev[0]) * WLAN_VAP_SUPPORT_MAX_NUM_LIMIT, 0,
             sizeof(g_st_dfr_info.past_netdev[0]) * WLAN_VAP_SUPPORT_MAX_NUM_LIMIT);

    cfg_rst = wal_dfr_excp_rx(mac_device->uc_device_id, 0);
    if (oal_unlikely(cfg_rst != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_test_dfr_start::wal_send_cfg_event return err_code [%d]!}\r\n", cfg_rst);
        return cfg_rst;
    }

    return OAL_SUCC;
}
#endif

OAL_STATIC uint32_t wal_set_bw_fixed(mac_vap_stru *mac_vap, uint32_t *params)
{
    /* �豸��up״̬���������ã�������down */
    if (mac_vap->en_vap_state != MAC_VAP_STATE_INIT) {
        oam_error_log1(mac_vap->uc_vap_id, OAM_SF_ANY,
            "{wal_set_bw_fixed::device is busy, please down it first %d!}\r\n", mac_vap->en_vap_state);
        return OAL_FAIL;
    }
    mac_vap->bit_bw_fixed = (uint8_t)params[0];
    oam_warning_log1(0, OAM_SF_CFG, "{wal_set_bw_fixed:bw_fixed = [%d].}", mac_vap->bit_bw_fixed);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_flowctl_param(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t idx = 0;
    uint8_t queue_type;
    uint16_t burst_limit;
    uint16_t low_waterline;
    uint16_t high_waterline;

    queue_type     = (uint8_t)params[idx++];   /* ��ȡ�������Ͳ��� */
    burst_limit    = (uint16_t)params[idx++];  /* ���ö��ж�Ӧ��ÿ�ε��ȱ��ĸ��� */
    low_waterline  = (uint16_t)params[idx++];  /* ���ö��ж�Ӧ������low_waterline */
    high_waterline = (uint16_t)params[idx++];  /* ���ö��ж�Ӧ������high_waterline */

    /* ����host flowctl ��ز��� */
    hcc_host_set_flowctl_param(queue_type, burst_limit, low_waterline, high_waterline);

    oam_warning_log4(mac_vap->uc_vap_id, OAM_SF_ANY,
                     "wal_set_flowctl_param, queue[%d]: burst limit = %d, low_waterline = %d, high_waterline =%d",
                     queue_type, burst_limit, low_waterline, high_waterline);
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_send_2040_coext(mac_vap_stru *mac_vap, uint32_t *params)
{
    mac_cfg_set_2040_coexist_stru param_2040_coexist;
    param_2040_coexist.coext_info = params[0];
    param_2040_coexist.channel_report = params[1];

    hmac_config_send_2040_coext(mac_vap, sizeof(mac_cfg_set_2040_coexist_stru), (uint8_t *)&param_2040_coexist);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_blacklist_show(mac_vap_stru *mac_vap, uint32_t *params)
{
    if ((mac_vap->en_vap_mode != WLAN_VAP_MODE_BSS_AP) && (mac_vap->en_vap_mode != WLAN_VAP_MODE_BSS_STA)) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_ANY,
                         "{wal_set_blacklist_show::not valid vap mode=%d!}\r\n",
                         mac_vap->en_vap_mode);
        return OAL_SUCC;
    }
    hmac_show_blacklist_info(mac_vap);
    return OAL_SUCC;
}
#ifdef _PRE_WLAN_RR_PERFORMENCE_DEBUG

OAL_STATIC uint32_t wal_set_show_rr_time_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_bool_enum_uint8 switch_state = (oal_bool_enum_uint8)params[0];
    g_host_rr_perform_timestamp.rr_switch = (switch_state == OAL_TRUE) ? OAL_SWITCH_ON : OAL_SWITCH_OFF;
    return OAL_SUCC;
}
#endif
#ifdef _PRE_WLAN_TCP_OPT

OAL_STATIC uint32_t wal_get_tcp_ack_stream_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_vap_stru *hmac_vap;

    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_ANY,
                       "{wal_get_tcp_ack_stream_info fail: hmac_vap is null}");
        return OAL_FAIL;
    }

    hmac_tcp_opt_ack_show_count(hmac_vap);
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_tcp_tx_ack_opt_enable(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint32_t tcp_ack_opt_val;
    hmac_device_stru *hmac_device;

    hmac_device = hmac_res_get_mac_dev(mac_vap->uc_device_id);
    if (hmac_device == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_ANY,
                       "{wal_set_tcp_tx_ack_opt_enable fail: hmac_device is null}");
        return OAL_FAIL;
    }

    tcp_ack_opt_val = params[0];
    if (tcp_ack_opt_val == 0) {
        hmac_device->sys_tcp_tx_ack_opt_enable = OAL_FALSE;
    } else {
        hmac_device->sys_tcp_tx_ack_opt_enable = OAL_TRUE;
    }
    oam_warning_log1(0, OAM_SF_ANY, "{wal_set_tcp_tx_ack_opt_enable:sys_tcp_tx_ack_opt_enable = %d}",
                     hmac_device->sys_tcp_tx_ack_opt_enable);
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_tcp_rx_ack_opt_enable(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint32_t tcp_rx_ack_opt_val;
    hmac_device_stru *hmac_device;

    hmac_device = hmac_res_get_mac_dev(mac_vap->uc_device_id);
    if (hmac_device == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_ANY,
                       "{wal_set_tcp_rx_ack_opt_enable fail: hmac_device is null}");
        return OAL_FAIL;
    }

    tcp_rx_ack_opt_val = params[0];
    if (tcp_rx_ack_opt_val == 0) {
        hmac_device->sys_tcp_rx_ack_opt_enable = OAL_FALSE;
    } else {
        hmac_device->sys_tcp_rx_ack_opt_enable = OAL_TRUE;
    }
    oam_warning_log1(0, OAM_SF_ANY, "{wal_set_tcp_rx_ack_opt_enable:sys_tcp_tx_ack_opt_enable = %d}",
                     hmac_device->sys_tcp_rx_ack_opt_enable);
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_tcp_tx_ack_limit(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint32_t tcp_tx_ack_limit_val;
    hmac_vap_stru *hmac_vap;

    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_ANY,
                       "{wal_set_tcp_tx_ack_limit fail: hmac_vap is null}");
        return OAL_FAIL;
    }

    tcp_tx_ack_limit_val = params[0];
    if (tcp_tx_ack_limit_val >= DEFAULT_TX_TCP_ACK_THRESHOLD) {
        hmac_vap->st_hmac_tcp_ack[HCC_TX].filter_info.ack_limit = DEFAULT_TX_TCP_ACK_THRESHOLD;
    } else {
        hmac_vap->st_hmac_tcp_ack[HCC_TX].filter_info.ack_limit = tcp_tx_ack_limit_val;
    }
    oam_warning_log1(0, OAM_SF_ANY, "{wal_set_tcp_tx_ack_limit:ul_ack_limit = %ld}",
                     hmac_vap->st_hmac_tcp_ack[HCC_TX].filter_info.ack_limit);
    return OAL_SUCC;
}

OAL_STATIC uint32_t wal_set_tcp_rx_ack_limit(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint32_t tcp_rx_ack_limit_val;
    hmac_vap_stru *hmac_vap;

    hmac_vap = mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_ANY,
                       "{wal_set_tcp_rx_ack_limit fail: hmac_vap is null}");
        return OAL_FAIL;
    }

    tcp_rx_ack_limit_val = params[0];

    if (tcp_rx_ack_limit_val >= DEFAULT_RX_TCP_ACK_THRESHOLD) {
        hmac_vap->st_hmac_tcp_ack[HCC_RX].filter_info.ack_limit = DEFAULT_RX_TCP_ACK_THRESHOLD;
    } else {
        hmac_vap->st_hmac_tcp_ack[HCC_RX].filter_info.ack_limit = tcp_rx_ack_limit_val;
    }
    oam_warning_log1(0, OAM_SF_ANY, "{wal_set_tcp_rx_ack_limit:ul_ack_limit = %ld}",
                     hmac_vap->st_hmac_tcp_ack[HCC_RX].filter_info.ack_limit);
    return OAL_SUCC;
}
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE

OAL_STATIC uint32_t wal_set_dev_customize_info(mac_vap_stru *mac_vap, uint32_t *params)
{
    wlan_chip_show_customize_info();
    return OAL_SUCC;
}
#endif
#if defined(_PRE_WLAN_FEATURE_11V_ENABLE)

OAL_STATIC uint32_t wal_set_11v_cfg_bsst(mac_vap_stru *mac_vap, uint32_t *params)
{
    oal_bool_enum_uint8 switch_11v_cfg = (oal_bool_enum_uint8)params[0];

    /* ���ýӿ�����11v���Կ��� */
    mac_mib_set_MgmtOptionBSSTransitionActivated(mac_vap, switch_11v_cfg);

    oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_11v_cfg_bsst:: Set BSST_Actived=[%d].}",
                     mac_mib_get_MgmtOptionBSSTransitionActivated(mac_vap));
    return OAL_SUCC;
}
#endif

OAL_STATIC uint32_t wal_set_start_join(mac_vap_stru *mac_vap, uint32_t *params)
{
    mac_bss_dscr_stru *bss_dscr;
    uint32_t bss_idx = params[0];

    /* ����bss index���Ҷ�Ӧ��bss dscr�ṹ��Ϣ */
    bss_dscr = hmac_scan_find_scanned_bss_dscr_by_index(mac_vap->uc_device_id, bss_idx);
    if (bss_dscr == NULL) {
        oam_warning_log0(mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_start_join::find bss failed by index!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_sta_initiate_join(mac_vap, bss_dscr);
}

OAL_STATIC uint32_t wal_set_amsdu_tx_on(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t aggr_tx_on = (uint8_t)params[0];
    hmac_vap_stru *hmac_vap = NULL;

    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_amsdu_tx_on::hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    mac_mib_set_CfgAmsduTxAtive(&hmac_vap->st_vap_base_info, aggr_tx_on);

    oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_amsdu_tx_on::ENABLE[%d].}", aggr_tx_on);
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_set_roam_enable(mac_vap_stru *mac_vap, uint32_t *params)
{
    hmac_vap_stru *hmac_vap;
    uint8_t roam_value = (params[0] == 0) ? 0 : 1;

    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_set_roam_enable::hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_roam_enable(hmac_vap, roam_value);
}

OAL_STATIC uint32_t wal_set_default_key(mac_vap_stru *mac_vap, uint32_t *params)
{
    uint8_t idx = 0;
    mac_setdefaultkey_param_stru payload_setdefaultkey_params = { 0 };
    /* sh hipriv.sh "vap0 set_default_key x(key_index) 0|1(en_unicast) 0|1(multicast)" */
    /* ��ȡ���� */
    payload_setdefaultkey_params.uc_key_index = params[idx++];  /* ��ȡkey_index */
    payload_setdefaultkey_params.en_unicast = params[idx++];    /* ��ȡen_unicast */
    payload_setdefaultkey_params.en_multicast = params[idx++];  /* ��ȡmulticast */

    return hmac_config_11i_set_default_key(
            mac_vap, sizeof(mac_setdefaultkey_param_stru), (uint8_t *)&payload_setdefaultkey_params);
}


uint32_t wal_params_check(uint32_t cmd_id, int32_t *params)
{
    host_cmd_stru *cmd_info = wal_get_cmd_info(cmd_id);
    uint32_t index;
    const int32_t *check_table = NULL;
    /* ���check_tableΪ��˵����������Ҫ��� */
    if (cmd_info->param_check_table == NULL) {
        return OAL_SUCC;
    }
    check_table = cmd_info->param_check_table;
    for (index = 0; index < cmd_info->param_num; index++) {
        /* С����Сֵ, �������ֵ,Ϊ�Ƿ�ֵ */
        if ((params[index] < check_table[index * PARAM_CHECK_TABLE_LEN + MIN_VALUE_INDEX]) ||
            (params[index] > check_table[index * PARAM_CHECK_TABLE_LEN + MAX_VALUE_INDEX])) {
            oam_error_log4(0, OAM_SF_ANY, "wal_params_check:check fail,cmd_id=%d, param=%d,min=%d,max=%d",
                cmd_id, params[index], check_table[index * PARAM_CHECK_TABLE_LEN + MIN_VALUE_INDEX],
                check_table[index * PARAM_CHECK_TABLE_LEN + MAX_VALUE_INDEX]);
            return OAL_FAIL;
        }
    }
    return OAL_SUCC;
}


OAL_STATIC uint32_t wal_get_cmd_netdev(int8_t *cmd, oal_net_device_stru **netdev, uint32_t *offset)
{
    oal_net_device_stru *temp_netdev = NULL;
    int8_t dev_name[CMD_NAME_MAX_LEN];
    uint32_t ret;

    if (oal_any_null_ptr3(cmd, netdev, offset)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_get_cmd_netdev::cmd/netdev/pul_off_set is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    ret = wal_get_cmd_one_arg(cmd, dev_name, CMD_NAME_MAX_LEN, offset);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_get_cmd_netdev::get cmd one arg err[%d]!}", ret);
        return ret;
    }
    /* ����dev_name�ҵ�dev */
    temp_netdev = wal_config_get_netdev(dev_name, OAL_STRLEN(dev_name));
    if (temp_netdev == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_get_cmd_netdev::wal_config_get_netdev return null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* ����oal_dev_get_by_name�󣬱������oal_dev_putʹnet_dev�����ü�����һ */
    oal_dev_put(temp_netdev);
    *netdev = temp_netdev;
    return OAL_SUCC;
}


uint32_t wal_get_cmd_id(int8_t *cmd, uint32_t *cmd_id, uint32_t *offset)
{
    int8_t cmd_name[CMD_NAME_MAX_LEN];
    uint32_t ret;
    uint32_t cmd_index;
    uint32_t host_cmd_table_size = wal_get_host_cmd_table_size();
    host_cmd_stru *cmd_info = NULL;
    ret = wal_get_cmd_one_arg(cmd, cmd_name, CMD_NAME_MAX_LEN, offset);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_get_cmd_id::get cmd one arg err[%d]!}", ret);
        return ret;
    }
    for (cmd_index = 0; cmd_index < host_cmd_table_size; cmd_index++) {
        cmd_info = wal_get_cmd_info(cmd_index);
        if (oal_strcmp(cmd_info->name, cmd_name) == 0) {
            *cmd_id = cmd_index;
            return OAL_SUCC;
        }
    }
    return OAL_FAIL;
}


uint32_t wal_get_cmd_params(int8_t *cmd, uint32_t cmd_id, int32_t *params)
{
    uint32_t param_index;
    uint32_t offset = 0;
    uint32_t ret = 0;
    int8_t arg[CMD_VALUE_MAX_LEN] = {0};
    host_cmd_stru *cmd_info = wal_get_cmd_info(cmd_id);
    /* ��ȡ���� */
    for (param_index = 0; param_index < cmd_info->param_num; param_index++) {
        ret = wal_get_cmd_one_arg(cmd, arg, CMD_VALUE_MAX_LEN, &offset);
        if (ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{wal_process_cmd_params::get cmd one arg err[%d]!}", ret);
            return ret;
        }
        params[param_index] = (uint32_t)oal_atoi(arg);
        cmd += offset;
    }
    return OAL_SUCC;
}

uint32_t wal_send_cmd_params_to_device(mac_vap_stru *mac_vap, host_cmd_stru *cmd_info, int32_t *params, uint32_t cmd_id)
{
    uint16_t new_params_len = 0;
    uint8_t  new_params[CMD_DEVICE_PARAMS_MAX_LEN] = {0};
    uint32_t ret;

    if (cmd_info->package_params == NULL) {
        oam_error_log1(mac_vap->uc_vap_id, OAM_SF_ANY,
            "wal_send_cmd_params_to_device:cmd_id[%d],package_params func is null!", cmd_id);
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* ������װ */
    ret = cmd_info->package_params(mac_vap, params, new_params, &new_params_len);
    if (ret != OAL_SUCC) {
        oam_error_log1(mac_vap->uc_vap_id, OAM_SF_ANY,
            "wal_send_cmd_params_to_device:cmd_id[%d],package_params fail!", cmd_id);
        return ret;
    }
    /* ���¼� */
    ret = hmac_config_send_event(mac_vap, cmd_info->cfg_id, new_params_len, new_params);
    if (ret != OAL_SUCC) {
        oam_error_log1(mac_vap->uc_vap_id, OAM_SF_ANY,
            "wal_send_cmd_params_to_device:cmd_id[%d],send event fail!", cmd_id);
    }
    return ret;
}


uint32_t wal_process_cmd_params(oal_net_device_stru *netdev, uint32_t cmd_id, int32_t *params)
{
    uint32_t ret;
    host_cmd_stru *cmd_info = NULL;
    mac_vap_stru *mac_vap = NULL;

    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* ��������Ч�� */
    ret = wal_params_check(cmd_id, params);
    if (ret != OAL_SUCC) {
        return ret;
    }
    cmd_info = wal_get_cmd_info(cmd_id);
    /* host����� */
    if (cmd_info->process_cmd != NULL) {
        ret = cmd_info->process_cmd(mac_vap, params);
        if (ret != OAL_SUCC) {
            oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_ANY, "wal_process_cmd:cmd_id[%d],cmd process fail!", cmd_id);
            return ret;
        }
    }
    /* �ж��Ƿ����¼���device */
    if (cmd_info->to_device == OAL_FALSE) {
        return OAL_SUCC;
    }
    /* ���¼� */
    ret = wal_send_cmd_params_to_device(mac_vap, cmd_info, params, cmd_id);
    if (ret != OAL_SUCC) {
        oam_error_log1(mac_vap->uc_vap_id, OAM_SF_ANY, "wal_process_cmd:cmd_id[%d],send event fail!", cmd_id);
        return ret;
    }
    return ret;
}

uint32_t wal_process_cmd(int8_t *cmd)
{
    uint32_t offset = 0;
    oal_net_device_stru *netdev = NULL;
    uint32_t ret;
    uint32_t cmd_id;
    int32_t  params[CMD_PARAMS_MAX_CNT] = {0};

    /* ����mac_vap */
    ret = wal_get_cmd_netdev(cmd, &netdev, &offset);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "wal_process_cmd:find netdev fail");
        return ret;
    }
    cmd += offset;
    /* ��������ID */
    ret = wal_get_cmd_id(cmd, &cmd_id, &offset);
    if (ret != OAL_SUCC) {
        return ret;
    }
    cmd += offset;
    /* ������� */
    ret = wal_get_cmd_params(cmd, cmd_id, params);
    if (ret != OAL_SUCC) {
        return ret;
    }
    return wal_process_cmd_params(netdev, cmd_id, params);
}

/* ��������б� */
#ifdef _PRE_WLAN_FEATURE_HID2D_TX_DROP
const int32_t g_hid2d_debug_switch_check_table[] = {
    /* hid2d_debug_en */
    0, 1,
    /* hid2d_delay_time */
    0, 256 };
#endif
const int32_t g_switch_check_table[] = {
    /* ���ص�ȡֵ��ΧΪ0|1 */
    0, 1 };
const int32_t g_support_2040_coext_check_table[] = {
    /* 0��ʾ20/40MHz����ʹ�ܣ�1��ʾ20/40MHz���治ʹ�� */
    0, 1 };
#ifdef _PRE_WLAN_FEATURE_NEGTIVE_DET
const int32_t g_pk_mode_check_table[] = {
    /* ������/������ */
    0, 3,
    /* BW */
    0, WLAN_BW_CAP_BUTT - 1,
    /* protocol */
    0, WLAN_PROTOCOL_CAP_BUTT - 1,
    /* ����ֵ */
    MIN_INT32, MAX_INT32 };
#endif
const int32_t g_remove_app_ie_check_table[] = {
    /* remove type Ϊ1��ʾ�Ƴ���IE��0Ϊ�ָ�IE */
    0, 1,
    MIN_INT32, MAX_INT32 };
const int32_t g_pm_switch_check_table[] = {
    /* pm_switch_cfgö�ٷ�Χ 0 -- 4 */
    0, 4 };
const int32_t g_beacon_chan_switch_check_table[] = {
    /* 0Ϊ��ͨ��ͨ�����ͣ�1Ϊ����˫ͨ��������������,2Ϊ����˫ͨ���������� */
    0, 2 };
const int32_t g_btcoex_preempt_type_check_table[] = {
    0, 6,
    MIN_INT32, MAX_INT32 };
#ifdef _PRE_WLAN_FEATURE_WMMAC
const int32_t g_addts_req_check_table[] = {
    0, 7,                   /* tid��ȡֵ��Χ0~7 */
    MIN_INT32, MAX_INT32,   /* direction 00:uplink 01:downlink 10:reserved 11:Bi-directional */
    MIN_INT32, MAX_INT32,   /* PSB��1��ʾU-APSD��0��ʾlegacy */
    MIN_INT32, MAX_INT32,   /* UP */
    MIN_INT32, MAX_INT32,   /* Nominal MSDU Size ,��һλΪ1 */
    MIN_INT32, MAX_INT32,   /* maximum MSDU size */
    MIN_INT32, MAX_INT32,   /* minimum data rate */
    MIN_INT32, MAX_INT32,   /* mean data rate */
    MIN_INT32, MAX_INT32,   /* peak data rate */
    MIN_INT32, MAX_INT32,   /* minimum PHY Rate */
    MIN_INT32, MAX_INT32,   /* surplus bandwidth allowance */
    };
const int32_t g_delts_check_table[] = {
    0, WLAN_TID_MAX_NUM - 1 };
#endif
const int32_t g_mem_pool_id_check_table[] = {
    0, OAL_MEM_POOL_ID_BUTT - 1 };
const int32_t g_user_id_check_table[] = {
    /* user_id��ȡֵ��Χ */
    0, WLAN_ASSOC_USER_MAX_NUM - 1};

const int32_t g_frag_threshold_check_table[] = {
    WLAN_FRAG_THRESHOLD_MIN, WLAN_FRAG_THRESHOLD_MAX };

#ifdef _PRE_WLAN_FEATURE_PHY_EVENT_INFO
const int32_t g_phy_event_rpt_check_table[] = {
    /* phy event report en 0|1 */
    0, 1,
    /* phy event mem num */
    MIN_INT32, MAX_INT32,
    /* phy event0 select  */
    MIN_INT32, MAX_INT32,
    /* phy event1 select */
    MIN_INT32, MAX_INT32,
};
#endif
const int32_t g_txbf_cap_check_table[] = {
    /* txbf_cap value is 0 | 1 | 2 | 3
     * bit0��ʾRX(bfee)���� bit1��ʾTX(bfer)���� */
    0, 3 };
const int32_t g_icmp_filter_check_table[] = {
    1, 1,   /* filter_id must be 1 */
    0, 1 }; /* 0 :�ر�1: �� */
host_cmd_stru g_host_cmd_table[] = {
#ifdef _PRE_WLAN_FEATURE_HID2D_TX_DROP
    { "hid2d_debug_switch", 2, g_hid2d_debug_switch_check_table, wal_set_hid2d_debug_switch,
        hmac_package_tlv_params, OAL_TRUE, WLAN_CFGID_HID2D_DEBUG_SWITCH },
    { "hid2d_switch", 1, NULL, wal_set_hid2d_state,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_HID2D_STATE },
#endif
    { "bgscan_enable", 1, NULL, wal_set_bgscan_type, NULL, OAL_FALSE, 0 },
    { "arp_offload_enable", 1, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_ENABLE_ARP_OFFLOAD },
    /* �������mac addrɨ�迪�أ�sh hipriv.sh "Hisilicon0 random_mac_addr_scan 0|1(��|�ر�)" */
    { "random_mac_addr_scan", 1, g_switch_check_table, wal_set_random_mac_addr_scan, NULL, OAL_FALSE, 0 },
    /* ����20/40����ʹ��: sh hipriv.sh "wlan0 2040_coexistence 0|1" 0��ʾ20/40MHz����ʹ�ܣ�1��ʾ20/40MHz���治ʹ�� */
    { "2040_coexistence", 1, g_support_2040_coext_check_table, wal_set_2040_coext_support, NULL, OAL_FALSE, 0 },
    /* ����PSPOLL���� sh hipriv.sh 'wlan0 set_ps_mode 3' */
    { "set_ps_mode", 1, NULL, wal_set_sta_ps_mode, hmac_package_sta_ps_mode_params, OAL_TRUE, WLAN_CFGID_SET_PS_MODE },
    /* sta psm��ͳ����Ϣ sh hipriv.sh 'wlan0 psm_info_debug 1' */
    { "psm_info_debug", 2, NULL, wal_set_sta_ps_info,
        hmac_package_sta_ps_info_params, OAL_TRUE, WLAN_CFGID_SHOW_PS_INFO },
    { "set_fast_sleep_para", 4, NULL, NULL, hmac_package_fasts_sleep_params, OAL_TRUE, WLAN_CFGID_SET_FASTSLEEP_PARA },
    /* sh hipriv.sh "wlan0 uapsd_en_cap 0|1" */
    { "uapsd_en_cap", 1, g_switch_check_table, wal_set_uapsd_cap,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_UAPSD_EN },
    /* ����uapsd�Ĳ�����Ϣ sh hipriv.sh 'wlan0 set_uapsd_para 3 1 1 1 1 */
    { "set_uapsd_para", 5, NULL, wal_set_uapsd_para, hmac_package_uapsd_params, OAL_TRUE, WLAN_CFGID_SET_UAPSD_PARA },
    /* ���ó�����Ŀ:      sh hipriv.sh "vap0 al_tx_num <value>" */
    { "al_tx_num", 1, NULL, NULL, hmac_package_uint32_params, OAL_TRUE, WLAN_CFGID_SET_ALWAYS_TX_NUM },
    /* ���ݳ����ۺ���Ŀ���� */
    { "al_tx_aggr_num", 1, NULL, NULL, hmac_package_uint32_params, OAL_TRUE, WLAN_CFGID_SET_ALWAYS_TX_AGGR_NUM },
    { "p2p_scenes", 1, NULL, wal_set_p2p_scenes, hmac_package_uint8_params, OAL_TRUE, WALN_CFGID_SET_P2P_SCENES },
#ifdef _PRE_WLAN_FEATURE_11K
    /* sh hipriv.sh "wlan0 beacon_req_table_switch 0/1" */
    { "beacon_req_table_switch", 1, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_BCN_TABLE_SWITCH },
#endif
    /* VOE����ʹ�ܿ��ƣ�Ĭ�Ϲر� sh hipriv.sh "wlan0 voe_enable 0/1"
    (Bit0:11r  Bit1:11V Bit2:11K Bit3:�Ƿ�ǿ�ư���IE70(voe ��֤��Ҫ),
     Bit4:11r��֤,B5-B6:11k auth operating class  Bit7:��ȡ��Ϣ) */
    { "voe_enable", 1, NULL, wal_set_voe_enable, hmac_package_uint16_params, OAL_TRUE, WLAN_CFGID_VOE_ENABLE },
#ifdef _PRE_WLAN_FEATURE_NEGTIVE_DET
    /* pkmode���ܵ����޵����ӿ� hipriv "wlan0 pk_mode_debug 0/1(high/low) 0/1/2/3/4(BW) 0/1/2/3(protocol) ��������" */
    { "pk_mode_debug", 4, g_pk_mode_check_table, wal_set_pk_mode_debug, NULL, OAL_FALSE, 0 },
#endif
    /* ͨ��eid�Ƴ��û�̬�·���ĳ��IE sh hipriv.sh "wlan0 remove_app_ie 0/1 eid" 0�ָ���ie,1���θ�ie */
    { "remove_app_ie", 2, g_remove_app_ie_check_table, wal_set_remove_app_ie, NULL, OAL_FALSE, 0 },
#ifdef _PRE_WLAN_FEATURE_HID2D
#ifdef _PRE_WLAN_FEATURE_HID2D_PRESENTATION
    { "scan_chan", 1, NULL, wal_set_hid2d_scan_channel, NULL, OAL_FALSE, 0 },
#endif
#endif
    /* ȫ����־����:  hipriv "Hisilicon0 global_log_switch 0 | 1 */
    { "global_log_switch", 1, g_switch_check_table, wal_set_global_log_switch, NULL, OAL_FALSE, 0},
    /* log�͹���ģʽ: hipriv "Hisilicon0 pm_switch 0 | 1"��log pmģʽ���� */
    { "pm_switch", 1, g_pm_switch_check_table, wal_set_pm_switch,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_PM_SWITCH},
    /* log�͹���ģʽ: hipriv "Hisilicon0 power_test 0 | 1"�����Ĳ���ģʽ�� */
    { "power_test", 1, g_switch_check_table, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_POWER_TEST},
    /* �����Ƿ��ϱ�beacon֡����: hipriv "Hisilicon0 ota_beacon_switch 0 | 1"��������������е�VAP */
    { "ota_beacon_on", 1, NULL, wal_set_ota_beacon_switch,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_OTA_BEACON_SWITCH},
    /* ����beacon֡���Ͳ�����������: hipriv "vap0 beacon_chain_switch 0/1"
       Ŀǰ��ȡ���ǵ�ͨ��ģʽ(ʹ��ͨ��0)��0��ʾ�ر�˫·�������ͣ�1��ʾ���������������ĳһ��VAP */
    { "beacon_chain_switch", 1, g_beacon_chan_switch_check_table, NULL,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_BEACON_CHAIN_SWITCH},
    /* ����preempt_type��sh hipriv.sh "vap0 coex_preempt_type  0/1(Ӳ���������) 0-3/0-1"
       0 noframe 1 self-cts 2 nulldata 3 qosnull  ���ps�򿪻��߹ر� 2 slot��ǰ�� */
    { "coex_preempt_type", 2, g_btcoex_preempt_type_check_table, NULL,
        hmac_package_btcoex_preempt_type_params, OAL_TRUE, WLAN_CFGID_BTCOEX_PREEMPT_TYPE},
#ifdef _PRE_WLAN_FEATURE_WMMAC
    /* ���ý���TS��������ADDTS REQ����������:
       hipriv "vap0 addts_req tid direction apsd up nominal_msdu_size max_msdu_size minimum_data_rate
       mean_data_rate peak_data_rate minimum_phy_rate surplus_bandwidth_allowance" ���������ĳһ��VAP */
    { "addts_req", 11, g_addts_req_check_table, wal_set_addts_req, NULL, OAL_FALSE, 0},
    /* ����ɾ��TS��������DELTS����������: hipriv "vap0 tidno" ���������ĳһ��VAP */
    { "delts", 1, g_delts_check_table, wal_set_delts, NULL, OAL_FALSE, 0},
#endif
    /* ��ӡ�ڴ����Ϣ: hipriv "Hisilicon0 meminfo poolid" */
    { "meminfo", 1, g_mem_pool_id_check_table, wal_set_mem_info, NULL, OAL_FALSE, 0},
    /* ���ָ���ڴ��й©�ڴ��: hipriv "Hisilicon0 memleak poolid" */
    { "memleak", 1, g_mem_pool_id_check_table, wal_set_mem_leak, NULL, OAL_FALSE, 0 },
    { "ota_set_switch", 2, NULL, wal_set_ota_switch, hmac_package_ota_switch_params, OAL_TRUE, WLAN_CFGID_OTA_SWITCH},
    /* ��ȡ����ά��ͳ����Ϣ: hipriv "Hisilicon0 wifi_stat_info" */
    { "wifi_stat_info", 0, NULL, wal_set_show_stat_info, NULL, OAL_FALSE, 0 },
    /* ��������ά��ͳ����Ϣ: hipriv "Hisilicon0 clear_stat_info" */
    { "clear_stat_info", 0, NULL, wal_set_clear_stat_info, NULL, OAL_FALSE, 0 },
    /* �ϱ�ĳ��user�µ�ά��ͳ����Ϣ: sh hipriv.sh "Hisilicon0 usr_stat_info usr_id" */
    { "usr_stat_info", 1, g_user_id_check_table, wal_set_user_stat_info, NULL, OAL_FALSE, 0 },
    /* ֧���ŵ��б� hipriv "Hisilicon0 list_channel" */
    { "list_channel", 0, NULL, wal_set_list_channel, NULL, OAL_FALSE, 0 },
    /* ��ӡ�¼�������Ϣ������ӡ��ÿһ���ǿ��¼��������¼��ĸ������Լ�ÿһ���¼�ͷ��Ϣ, hipriv "Hisilicon0 event_queue" */
    { "event_queue", 0, NULL, wal_set_event_queue_info, NULL, OAL_FALSE, 0 },
#ifdef _PRE_WLAN_DFT_STAT
    /* ����ָ��VAP��ͳ����Ϣ: hipriv "vap_name clear_vap_stat_info" */
    { "clear_vap_stat_info", 0, NULL, wal_set_clear_vap_stat_info, NULL, OAL_FALSE, 0 },
    /* �ϱ�����ֹͣ�ϱ�vap����ͳ����Ϣ: sh hipriv.sh "vap_name vap _stat  0|1" */
    { "vap_stat", 1, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_VAP_STAT },
#endif
    /* ��ȡ�����ȼ����ĵ�ͳ����� */
    { "get_hipkt_stat", 0, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_GET_HIPKT_STAT },
    /* ��ȡ�������״̬��Ϣ */
    { "get_flowctl_stat", 0, NULL, wal_get_flowctl_stat, NULL, OAL_FALSE, 0 },
    /* ��ӡAP������STA�б�: hipriv "sta0 list_sta" */
    { "list_sta", 0, NULL, wal_set_list_sta, NULL, OAL_FALSE, 0 },
    /* ����staȥ��֤: hipriv "vap0 start_deauth" */
    { "start_deauth", 0, NULL, wal_start_deauth, NULL, OAL_FALSE, 0 },
    /* ���������û�����̫�����أ�sh hipriv.sh "Hisilicon0 ether_all 0|1(֡����tx|rx) 0|1(����)" */
    { "ether_all", 2, NULL, wal_set_all_ether_switch, NULL, OAL_FALSE, 0 },
    /* ���÷��͹㲥arp��dhcp���أ�sh hipriv.sh "Hisilicon0 dhcp_arp_switch 0|1(����)" */
    { "dhcp_arp_switch", 1, NULL, wal_set_dhcp_arp_switch, NULL, OAL_FALSE, 0 },
    /* ����AMSDU�ۺϸ���: sh hipriv.sh "wlan0 amsdu_aggr_num num" */
    { "amsdu_aggr_num", 1, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_AMSDU_AGGR_NUM },
    /* �����Զ��������� */
    { "auto_protection", 1, NULL, NULL, hmac_package_uint32_params, OAL_TRUE, WLAN_CFGID_SET_AUTO_PROTECTION },
    /* ���÷�Ƭ���޵���������: hipriv "vap0 frag_threshold (len)" ���������ĳһ��VAP */
    { "frag_threshold", 1, g_frag_threshold_check_table, wal_set_frag_threshold, NULL, OAL_FALSE, 0},
    /* ����80211�鲥\�㲥֡�ϱ��Ŀ��أ�sh hipriv.sh "Hisilicon0 80211_mc_switch
       0|1(֡����tx|rx) 0|1(֡����:����֡|����֡) 0|1(֡���ݿ���) 0|1(CB����) 0|1(����������)" */
    { "80211_mc_switch", 5, NULL, wal_set_80211_mcast_switch,
        hmac_package_80211_mcast_params, OAL_TRUE, WLAN_CFGID_80211_MCAST_SWITCH},
    /* ����probe req��rsp�ϱ��Ŀ��أ�sh hipriv.sh "Hisilicon0 probe_switch
       0|1(֡����tx|rx) 0|1(֡���ݿ���) 0|1(CB����) 0|1(����������)" */
    { "probe_switch", 4, NULL, wal_set_probe_switch,
        hmac_package_probe_switch_params, OAL_TRUE, WLAN_CFGID_PROBE_SWITCH },
    /* ��������ota�ϱ������Ϊ1������������֡��cb���������������Ϊ0��ʲô��������
       sh hipriv.sh "Hisilicon0 set_all_ota 0|1" */
    { "set_all_ota", 1, NULL, wal_set_all_ota, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_ALL_OTA},
#ifdef _PRE_WLAN_FEATURE_TXOPPS
    /* ����mac txop psʹ�ܼĴ�����sh hipriv.sh "stavap_name txopps_hw_en
       0|1(txop_ps_en) 0|1(condition1) 0|1(condition2)" */
    { "txopps_hw_en", 3, NULL, wal_set_txop_ps_machw, hmac_package_txop_ps_params, OAL_TRUE, WLAN_CFGID_TXOP_PS_MACHW},
#endif
#ifdef _PRE_WLAN_FEATURE_PHY_EVENT_INFO
    /* ����PHY�¼���ʹ��:hipriv "wlan0/p2p0 phy_event 0|1(0��ʹ�ܣ�1ʹ��) 1(mem����) 1(event0�¼��Ŀ���ʹ��(32bits)
        1(event1�¼��Ŀ���ʹ��(32bits), 0~31λ����32���¼�����)" */
    { "phy_event", 4, g_phy_event_rpt_check_table, wal_hipriv_phy_event_rpt, hmac_phy_event_rpt_params,
        OAL_TRUE, WLAN_CFGID_PHY_EVENT_RPT},
#endif
    /* ����STBC���� STBC���ÿ��ص�����: hipriv "vap0 set_stbc_cap 0 | 1" */
    { "set_stbc_cap", 1, g_switch_check_table, wal_set_stbc_cap,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_STBC_CAP },
    /* ����LDPC���� LDPC���ÿ��ص�����: hipriv "vap0 set_ldpc_cap 0 | 1 */
    { "set_ldpc_cap", 1, g_switch_check_table, wal_set_ldpc_cap,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_LDPC_CAP },
    /* ������ر�txbf�� ����bit0/����bit1 ���� hipriv "vap0 alg_txbf_switch 0|1|2|3" */
    { "set_txbf_cap", 1, g_txbf_cap_check_table, wal_set_txbf_cap,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_TXBF_SWITCH },
    /* sh hipriv.sh 'wlan0 set_psm_para 100 40 */
    { "set_psm_para", 4, NULL, NULL, hmac_package_sta_psm_params, OAL_TRUE, WLAN_CFGID_SET_PSM_PARAM },
#ifdef _PRE_WLAN_FEATURE_PMF
    /* ����chip test��ǿ��ʹ��pmf���� (���ڹ���֮��) sh hipriv.sh "vap0 enable_pmf 0|1|2(0��ʹ�ܣ�1 enable, 2ǿ��)  */
    { "enable_pmf", 1, NULL, wal_set_enable_pmf, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_PMF_ENABLE },
#endif
#ifdef _PRE_WLAN_FEATURE_DFR
    /* dfr���ܴ�׮�����ӿ� sh hipriv.sh "vap0 dfr_start 0(dfr�ӹ���:0-device�쳣��λ )" */
    { "dfr_start", 0, NULL, wal_test_dfr_start, NULL, OAL_FALSE, 0 },
#endif
#ifdef _PRE_WLAN_FEATURE_NRCOEX
    /* ����NR���������sh hipriv.sh "wlan0 nrcoex_set_prior 0/1/2/3" */
    { "nrcoex_set_prior", 1, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_NRCOEX_PRIORITY_SET },
    /* ����nrcoex�Ĳ������ǰֻ��06ʹ�� hipriv.sh "wlan0 nrcoex_msg value" */
    { "nrcoex_msg", 1, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_NRCOEX_TEST },
#endif
    /* ����apģʽ��vap����̶� */
    { "bw_fixed", 1, g_switch_check_table, wal_set_bw_fixed,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_BW_FIXED },
    /* ����������ز��� sh hipriv.sh "Hisilicon0 set_flowctl_param 0/1/2/3 20 20 40"  0/1/2/3 �ֱ����be,bk,vi,vo */
    { "set_flowctl_param", 4, NULL, wal_set_flowctl_param, NULL, OAL_FALSE, 0 },
    /* ����20/40�������֡: hipriv "Hisilicon0 send_2040_coext coext_info chan_report" */
    { "send_2040_coext", 2, NULL, wal_send_2040_coext, NULL, OAL_FALSE, 0 },
#ifdef _PRE_WLAN_FEATURE_PSD_ANALYSIS
    /* ʹ��PSD�ɼ�: hipriv "Hisilicon0 set_psd 0/1"��ʹ��֮ǰ��config  */
    { "set_psd", 1, g_switch_check_table, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_PSD },
    /* ʹ��PSD�ɼ�: hipriv "Hisilicon0 cfg_psd" */
    { "cfg_psd", 0, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_CFG_PSD },
#endif
    /* wal_config_blacklist_show */
    { "blacklist_show", 0, NULL, wal_set_blacklist_show, NULL, OAL_FALSE, 0 },
#ifdef _PRE_WLAN_RR_PERFORMENCE_DEBUG
    /* ��ӡRR�����ʱ�����sh hipriv.sh "Hisilicon0 show_rr_timeinfo 0|1" */
    { "show_rr_timeinfo", 1, g_switch_check_table, wal_set_show_rr_time_info,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_ENABLE_RR_TIMEINFO },
#endif
    /* ��ʾDevice���¼��IP��ַ:sh hipriv.sh "wlan0 show_ip_addr 1 1" */
    { "show_arpoffload_info", 2, NULL, NULL,
        hmac_package_show_arpoffload_info_params, OAL_TRUE, WLAN_CFGID_SHOW_ARPOFFLOAD_INFO },
#ifdef _PRE_WLAN_TCP_OPT
    /* ��ʾTCP ACK ����ͳ��ֵ sh hipriv.sh "vap0 get_tx_ack_stream_info */
    { "get_tcp_ack_stream_info", 0, NULL, wal_get_tcp_ack_stream_info, NULL, OAL_FALSE, 0 },
    /* ���÷���TCP ACK�Ż�ʹ��  sh hipriv.sh "vap0 tcp_tx_ack_opt_enable 0 | 1 */
    { "tcp_tx_ack_opt_enable", 1, NULL, wal_set_tcp_tx_ack_opt_enable, NULL, OAL_FALSE, 0 },
    /* ���ý���TCP ACK�Ż�ʹ�� sh hipriv.sh "vap0 tcp_rx_ack_opt_enable 0 | 1 */
    { "tcp_rx_ack_opt_enable", 1, NULL, wal_set_tcp_rx_ack_opt_enable, NULL, OAL_FALSE, 0 },
    /* ���÷���TCP ACK LIMIT sh hipriv.sh "vap0 tcp_tx_ack_opt_limit X */
    { "tcp_tx_ack_opt_limit", 1, NULL, wal_set_tcp_tx_ack_limit, NULL, OAL_FALSE, 0 },
    /* ���ý���TCP ACKLIMIT  sh hipriv.sh "vap0 tcp_tx_ack_opt_limit X */
    { "tcp_rx_ack_opt_limit", 1, NULL, wal_set_tcp_rx_ack_limit, NULL, OAL_FALSE, 0 },
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    /* ��ӡ���ƻ���Ϣ */
    { "custom_info", 1, g_switch_check_table, wal_set_dev_customize_info,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SHOW_DEV_CUSTOMIZE_INFOS },
#endif
#if defined(_PRE_WLAN_FEATURE_11V_ENABLE)
    /* 11v bss transition�������� sh hipriv.sh "vap0 11v_cfg_bsst 0|1" 0:�ر�11V���ԣ�1:��11V����  */
    { "11v_cfg_bsst", 1, g_switch_check_table, wal_set_11v_cfg_bsst,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_11V_BSST_SWITCH },
#endif
    /* �����״��¼�sh hipriv.sh "wlan0 radar_set" */
    { "radar_set", 0, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SET_RADAR },
    /* ����sta���벢��֤����: hipriv "sta0 start_join 1" 1��ʾɨ�赽��AP��deviceд�����±�� */
    { "start_join", 1, NULL, wal_set_start_join, NULL, OAL_FALSE, 0 },
    /* ������ر�ampdu���͹��� hipriv "vap0 amsdu_tx_on 0\1" */
    { "amsdu_tx_on", 1, NULL, wal_set_amsdu_tx_on, NULL, OAL_FALSE, 0 },
    /* ���ݱ��λ�ϱ�vap�Ķ�Ӧ��Ϣ sh hipriv.sh "wlan0 report_vap_info 1" */
    { "report_vap_info", 1, NULL, NULL, hmac_package_uint32_params, OAL_TRUE, WLAN_CFGID_REPORT_VAP_INFO },
    /* �������ο��� */
    { "roam_enable", 1, NULL, wal_set_roam_enable, NULL, OAL_FALSE, 0 },
    /* fem�͹�������: sh hipriv.sh "wlan0 fem_lowpower 0|1 |2" */
    { "fem_lowpower", 1, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_SPEC_FEMLP_EN },
    /* icmp��������: sh hipriv.sh "wlan0 icmp_filter 1 0|1" */
    { "icmp_filter", 2, g_icmp_filter_check_table, NULL, hmac_package_icmp_filter_params,
        OAL_TRUE, WLAN_CFGID_ASSIGNED_FILTER},
    /* ����chip test��ǿ��ʹ��pmf���� (���ڹ���֮��)
       sh hipriv.sh "vap0 set_default_key x(key_index) 0|1(en_unicast) 0|1(multicast)"  */
    { "set_default_key", 3, NULL, wal_set_default_key,
        hmac_package_default_key_params, OAL_TRUE, WLAN_CFGID_DEFAULT_KEY},
    { "rx_mode_switch", 1, NULL, NULL,
        hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_RX_MODE_SWITCH_DEBUG},
    /* ������һ·�����״�CAC sh hipriv.sh "wlan0 radar_cac 36 7" */
    { "radar_cac", 2, NULL, NULL, hmac_package_cac_params, OAL_TRUE, WLAN_CFGID_START_ANOTHER_DEV_CAC },
#ifdef _PRE_WLAN_FEATURE_FTM
    /* ����ftm_set_delay�Ĳ������ǰֻ��06ʹ�� hipriv.sh "wlan0 ftm_set_delay value" */
    { "ftm_set_delay", 1, NULL, NULL, hmac_package_ftm_params, OAL_TRUE, WLAN_CFGID_FTM_DBG },
#endif
    /*
     * �л�ĳ��user tid�ķ��ͷ�ʽ hipriv.sh "wlan0 tx_state_switch user tid mode"
     * - user: user_addʱ�����user id
     * - tid : 0 - 6(��֧��tid 7)
     * - mode: 0: host ring tx; 1: device ring tx; 5: ���״̬��ӡ
     */
    { "tx_mode_switch", 3, NULL, hmac_tx_mode_switch, NULL, OAL_FALSE, 0 },
#ifdef _PRE_WLAN_FEATURE_11AX
    /* sh hipriv.sh "wlan0 tid_queue_size 0|1" */
    { "tid_queue_size", 1, NULL, NULL, hmac_package_uint8_params, OAL_TRUE, WLAN_CFGID_REPORT_TID_QUEUE_SIZE},
#endif
};

uint32_t wal_get_host_cmd_table_size(void)
{
    return oal_array_size(g_host_cmd_table);
}
host_cmd_stru *wal_get_cmd_info(uint32_t cmd_id)
{
    if (cmd_id >= oal_array_size(g_host_cmd_table)) {
        oam_error_log2(0, OAM_SF_ANY, "hmac_get_cmd_info:cmd_id=%d > %d is error",
            cmd_id, oal_array_size(g_host_cmd_table));
        return NULL;
    }
    return &g_host_cmd_table[cmd_id];
}
