

/* 1 ͷ�ļ����� */
#include "hmac_scan.h"
#include "hmac_chan_mgmt.h"
#include "hmac_204080_coexist.h"
#include "hmac_resource.h"
#include "hmac_fsm.h"
#include "mac_ie.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_204080_COEXIST_C

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST

oal_void hmac_get_pri_sec_chan(mac_bss_dscr_stru *pst_bss_dscr, oal_uint32 *pul_pri_chan, oal_uint32 *pul_sec_chan)
{
    *pul_pri_chan = *pul_sec_chan = 0;

    *pul_pri_chan = pst_bss_dscr->st_channel.uc_chan_number;

    oam_info_log1(0, OAM_SF_2040, "hmac_get_pri_sec_chan:pst_bss_dscr->st_channel.en_bandwidth = %d\n",
                  pst_bss_dscr->en_channel_bandwidth);

    if (pst_bss_dscr->en_channel_bandwidth == WLAN_BAND_WIDTH_40PLUS) {
        *pul_sec_chan = *pul_pri_chan + 4; /* ��20M�ŵ�����20M�ŵ�+4 */
    } else if (pst_bss_dscr->en_channel_bandwidth == WLAN_BAND_WIDTH_40MINUS) {
        *pul_sec_chan = *pul_pri_chan - 4; /* ��20M�ŵ�����20M�ŵ�-4 */
    } else {
        OAM_WARNING_LOG1(0, OAM_SF_2040, "hmac_get_pri_sec_chan:pst_bss_dscr is not support 40Mhz, *pul_sec_chan = %d",
                         *pul_sec_chan);
    }

    oam_info_log2(0, OAM_SF_2040, "*pul_pri_chan = %d, *pul_sec_chan = %d\n\n", *pul_pri_chan, *pul_sec_chan);

    return;
}


oal_void hmac_switch_pri_sec(mac_vap_stru *pst_mac_vap)
{
    if (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40PLUS) {
        pst_mac_vap->st_channel.uc_chan_number = pst_mac_vap->st_channel.uc_chan_number + 4; /* ��20M�ŵ�����20M�ŵ�+4 */
        pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_40MINUS;
    } else if (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40MINUS) {
        pst_mac_vap->st_channel.uc_chan_number = pst_mac_vap->st_channel.uc_chan_number - 4; /* ��20M�ŵ�����20M�ŵ�-4 */
        pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_40PLUS;
    } else {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                         "hmac_switch_pri_sec:en_bandwidth = %d\n not need obss scan\n",
                         pst_mac_vap->st_channel.en_bandwidth);
    }
}


oal_bool_enum_uint8 hmac_obss_check_40mhz_2g(mac_vap_stru *pst_mac_vap,
                                             hmac_scan_record_stru *pst_scan_record)
{
    oal_uint32 ul_pri_freq;
    oal_uint32 ul_sec_freq;
    oal_uint32 ul_affected_start;
    oal_uint32 ul_affected_end;

    oal_uint32 ul_pri;
    oal_uint32 ul_sec;
    oal_uint32 ul_sec_chan, ul_pri_chan;

    hmac_bss_mgmt_stru *pst_bss_mgmt = NULL;
    mac_bss_dscr_stru *pst_bss_dscr = NULL;
    hmac_scanned_bss_info *pst_scanned_bss = NULL;
    oal_dlist_head_stru *pst_entry = NULL;

    /* ��ȡ���ŵ������ŵ�����Ƶ�� */
    ul_pri_freq = (oal_int32)g_ast_freq_map_2g[pst_mac_vap->st_channel.uc_chan_number - 1].us_freq;
    if (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40PLUS) {
        ul_sec_freq = ul_pri_freq + 20; /* ��20M�ŵ�����Ƶ������20M�ŵ�����Ƶ��+20MHz */
    } else if (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40MINUS) {
        ul_sec_freq = ul_pri_freq - 20; /* ��20M�ŵ�����Ƶ������20M�ŵ�����Ƶ��-20MHz */
    } else {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_2040, "hmac_obss_check_40mhz_2g:en_bandwidth=%d not need obss",
                         pst_mac_vap->st_channel.en_bandwidth);
        return OAL_TRUE;
    }

    ul_affected_start = ((ul_pri_freq + ul_sec_freq) >> 1) - 25; /* 2.4G������,��40MHz��������Ƶ��Ϊ����,���Ҹ�25MHZ */
    ul_affected_end = ((ul_pri_freq + ul_sec_freq) >> 1) + 25;

    oam_info_log2(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                  "hmac_obss_check_40mhz_2g:40 MHz affected channel range: [%d, %d] MHz",
                  ul_affected_start, ul_affected_end);

    /* ��ȡɨ�����Ĺ���ṹ��ַ */
    pst_bss_mgmt = &(pst_scan_record->st_bss_mgmt);

    /* ��ȡ�� */
    oal_spin_lock(&(pst_bss_mgmt->st_lock));

    /* ����ɨ�赽��bss��Ϣ */
    oal_dlist_search_for_each(pst_entry, &(pst_bss_mgmt->st_bss_list_head))
    {
        pst_scanned_bss = oal_dlist_get_entry(pst_entry, hmac_scanned_bss_info, st_dlist_head);
        pst_bss_dscr = &(pst_scanned_bss->st_bss_dscr_info);
        if (pst_bss_dscr == OAL_PTR_NULL) {
            oam_warning_log0(0, OAM_SF_2040, "{hmac_obss_check_40mhz_2g::pst_bss_dscr is NULL}");
            continue;
        }
        ul_pri = (oal_int32)g_ast_freq_map_2g[pst_bss_dscr->st_channel.uc_chan_number - 1].us_freq;
        ul_sec = ul_pri;

        /* ��ȡɨ�赽��BSS���ŵ���Ƶ����Ϣ */
        hmac_get_pri_sec_chan(pst_bss_dscr, &ul_pri_chan, &ul_sec_chan);

        /* ��BSSΪ40MHz����,������ŵ�Ƶ�� */
        if (ul_sec_chan) {
            if (ul_sec_chan < ul_pri_chan)
                ul_sec = ul_pri - 20; /* ��20M�ŵ�����Ƶ������20M�ŵ�����Ƶ��+20MHz */
            else
                ul_sec = ul_pri + 20; /* ��20M�ŵ�����Ƶ������20M�ŵ�����Ƶ��-20MHz */
        }

        if (((ul_pri < ul_affected_start) || (ul_pri > ul_affected_end)) &&
            ((ul_sec < ul_affected_start) || (ul_sec > ul_affected_end)))
            continue; /* not within affected channel range */

        if (ul_sec_chan) {
            if ((ul_pri_freq != ul_pri) || (ul_sec_freq != ul_sec)) {
                oam_info_log4(0, OAM_SF_2040,
                              "hmac_obss_check_40mhz_2g:40 MHz pri/sec <%d, %d >mismatch with BSS\
                    <%d, %d>\n", ul_pri_freq,
                              ul_sec_freq, ul_pri, ul_sec);
                /* ����� */
                oal_spin_unlock(&(pst_bss_mgmt->st_lock));
                return OAL_FALSE;
            }
        }
    }

    /* ����� */
    oal_spin_unlock(&(pst_bss_mgmt->st_lock));

    return OAL_TRUE;
}


oal_bool_enum_uint8 hmac_obss_check_40mhz_5g(mac_vap_stru *pst_mac_vap,
                                             hmac_scan_record_stru *pst_scan_record)
{
    oal_uint32 ul_pri_chan;
    oal_uint32 ul_sec_chan;
    oal_uint32 ul_pri_bss;
    oal_uint32 ul_sec_bss;
    oal_uint32 ul_bss_pri_chan;
    oal_uint32 ul_bss_sec_chan;
    oal_uint8 uc_match;
    hmac_bss_mgmt_stru *pst_bss_mgmt = NULL;
    mac_bss_dscr_stru *pst_bss_dscr = NULL;
    hmac_scanned_bss_info *pst_scanned_bss = NULL;
    oal_dlist_head_stru *pst_entry = NULL;

    oam_info_log1(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                  "hmac_obss_check_40mhz_5g:pst_mac_vap->st_channel.\
        uc_chan_number = %d\n",
                  pst_mac_vap->st_channel.uc_chan_number);

    /* ��ȡ���ŵ��ʹ��ŵ� */
    ul_pri_chan = pst_mac_vap->st_channel.uc_chan_number;
    if (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40PLUS) {
        ul_sec_chan = ul_pri_chan + 4; /* ��20M�ŵ�����20M�ŵ�+4 */
    } else if (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40MINUS) {
        ul_sec_chan = ul_pri_chan - 4; /* ��20M�ŵ�����20M�ŵ�-4 */
    } else {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                         "hmac_obss_check_40mhz_5g: en_bandwidth = %d not need obss scan",
                         pst_mac_vap->st_channel.en_bandwidth);
        return OAL_TRUE;
    }

    /* ��ȡɨ�����Ĺ���ṹ��ַ */
    pst_bss_mgmt = &(pst_scan_record->st_bss_mgmt);

    /* ��ȡ�� */
    oal_spin_lock(&(pst_bss_mgmt->st_lock));

    /* ���ڴ��ŵ���⵽Beacon, �������ŵ���û��, ����Ҫ���������ŵ� */
    ul_pri_bss = ul_sec_bss = 0;
    oal_dlist_search_for_each(pst_entry, &(pst_bss_mgmt->st_bss_list_head))
    {
        pst_scanned_bss = oal_dlist_get_entry(pst_entry, hmac_scanned_bss_info, st_dlist_head);
        pst_bss_dscr = &(pst_scanned_bss->st_bss_dscr_info);

        if (pst_bss_dscr == OAL_PTR_NULL) {
            oam_warning_log0(0, OAM_SF_2040, "{hmac_obss_check_40mhz_5g::pst_bss_dscr is NULL}");
            continue;
        }

        oam_info_log1(pst_mac_vap->uc_vap_id, OAM_SF_2040, "hmac_obss_check_40mhz_5g:bss uc_channel_number = %d\n",
                      pst_bss_dscr->st_channel.uc_chan_number);
        if (pst_bss_dscr->st_channel.uc_chan_number == ul_pri_chan)
            ul_pri_bss++;
        else if (pst_bss_dscr->st_channel.uc_chan_number == ul_sec_chan)
            ul_sec_bss++;
    }

    if (ul_sec_bss && !ul_pri_bss) {
        oam_info_log0(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                      "hmac_obss_check_40mhz_5g:Switch own primary and secondary channel \
                        to get secondary channel with no Beacons from other BSSes\n");

        hmac_switch_pri_sec(pst_mac_vap);

        /* �˴����ν������ֱ�ӷ��ؼ���, ��hostapd-2.4.0�汾�޸� */
        /* �ͷ��� */
        oal_spin_unlock(&(pst_bss_mgmt->st_lock));
        return OAL_TRUE;
    }

    /*
     * Match PRI/SEC channel with any existing HT40 BSS on the same
     * channels that we are about to use (if already mixed order in
     * existing BSSes, use own preference).
 */
    uc_match = OAL_FALSE;
    oal_dlist_search_for_each(pst_entry, &(pst_bss_mgmt->st_bss_list_head))
    {
        pst_scanned_bss = oal_dlist_get_entry(pst_entry, hmac_scanned_bss_info, st_dlist_head);
        pst_bss_dscr = &(pst_scanned_bss->st_bss_dscr_info);
        if (pst_bss_dscr == OAL_PTR_NULL) {
            oam_warning_log0(0, OAM_SF_2040, "{hmac_obss_check_40mhz_5g::pst_bss_dscr is NULL}");
            continue;
        }

        hmac_get_pri_sec_chan(pst_bss_dscr, &ul_bss_pri_chan, &ul_bss_sec_chan);
        if ((ul_pri_chan == ul_bss_pri_chan) &&
            (ul_sec_chan == ul_bss_sec_chan)) {
            uc_match = OAL_TRUE;
            break;
        }
    }

    if (!uc_match) {
        oal_dlist_search_for_each(pst_entry, &(pst_bss_mgmt->st_bss_list_head))
        {
            pst_scanned_bss = oal_dlist_get_entry(pst_entry, hmac_scanned_bss_info, st_dlist_head);
            pst_bss_dscr = &(pst_scanned_bss->st_bss_dscr_info);
            if (pst_bss_dscr == OAL_PTR_NULL) {
                oam_warning_log0(0, OAM_SF_2040, "{hmac_obss_check_40mhz_5g::pst_bss_dscr is NULL}");
                continue;
            }

            hmac_get_pri_sec_chan(pst_bss_dscr, &ul_bss_pri_chan, &ul_bss_sec_chan);

            if ((ul_pri_chan == ul_bss_sec_chan) && (ul_sec_chan == ul_bss_pri_chan)) {
                hmac_switch_pri_sec(pst_mac_vap);
                break;
            }
        }
    }

    oam_info_log1(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                  "hmac_obss_check_40mhz_5g:After check, pst_mac_vap->st_channel.uc_chan_number = %d\n",
                  pst_mac_vap->st_channel.uc_chan_number);

    /* ����� */
    oal_spin_unlock(&(pst_bss_mgmt->st_lock));

    return OAL_TRUE;
}


oal_uint32 hmac_obss_init_scan_hook(hmac_scan_record_stru *pst_scan_record,
                                    hmac_device_stru *pst_dev)
{
    /*
    ACSδ���У���Ҫ����ɨ��������OBSS�������ж�
    1����ǰVAP���ŵ���ΪĿ���ŵ�
    2���ж�֮������Ҫ����VAP���ŵ��ʹ���
 */
    oal_uint8 uc_idx;
    oal_bool_enum_uint8 en_bandwidth_40mhz = OAL_FALSE;
    oal_bool_enum_uint8 en_2g_first_vap_bandwidth_40mhz = OAL_FALSE;
    mac_vap_stru *pst_mac_vap = NULL;
    oal_uint8 uc_2g_check_band = OAL_FALSE;
    oal_uint8 uc_5g_check_band = OAL_FALSE;
    oal_uint8 uc_5g_first_vap_chan_number = 0;
    wlan_channel_bandwidth_enum_uint8 en_5g_first_vap_bandwidth = WLAN_BAND_WIDTH_20M;

    oam_warning_log0(0, OAM_SF_ACS, "hmac_obss_init_scan_hook run\n");

    /* ����ÿ��vap,����ŵ��ʹ��� */
    for (uc_idx = 0; uc_idx < pst_dev->pst_device_base_info->uc_vap_num; uc_idx++) {
        pst_mac_vap = mac_res_get_mac_vap(pst_dev->pst_device_base_info->auc_vap_id[uc_idx]);
        if (pst_mac_vap && (pst_mac_vap->en_vap_mode == WLAN_VAP_MODE_BSS_AP)) {
            oam_info_log2(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                          "hmac_obss_init_scan_hook:original channel=%d,\
                bandwidth=%d.\n",
                          pst_mac_vap->st_channel.uc_chan_number, pst_mac_vap->st_channel.en_bandwidth);

            if (pst_mac_vap->st_channel.en_band == WLAN_BAND_2G) {
                if ((pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40PLUS) ||
                    (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40MINUS)) {
                    /* ֻ����һ��vap������ĸ����һ��vap�Ĵ�����ŵ� */
                    if (uc_2g_check_band == OAL_FALSE) {
                        /* 2G vap 20/40������ */
                        en_bandwidth_40mhz = hmac_obss_check_40mhz_2g(pst_mac_vap, pst_scan_record);
                        uc_2g_check_band = OAL_TRUE;
                        en_2g_first_vap_bandwidth_40mhz = en_bandwidth_40mhz;
                    } else {
                        /* ��һ��vap������֮��,������ͬƵ��vap�ŵ��ʹ��������һ�� */
                        oam_info_log0(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                                      "hmac_obss_init_scan_hook:follow bandwidth of first 2g vap.");
                        en_bandwidth_40mhz = en_2g_first_vap_bandwidth_40mhz;
                    }
                }

                /* ���ô��� */
                if (en_bandwidth_40mhz == OAL_FALSE) {
                    oam_info_log0(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                                  "hmac_obss_init_scan_hook:cann't set 40MHz bandwidth,change to 20MHz.\n");
                    pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
                }
            } else if (pst_mac_vap->st_channel.en_band == WLAN_BAND_5G) {
                if ((pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40PLUS) ||
                    (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40MINUS)) {
                    if (uc_5g_check_band == OAL_FALSE) {
                        /* 5G vap 20/40������ */
                        en_bandwidth_40mhz = hmac_obss_check_40mhz_5g(pst_mac_vap, pst_scan_record);
                        uc_5g_check_band = OAL_TRUE;
                        uc_5g_first_vap_chan_number = pst_mac_vap->st_channel.uc_chan_number;
                        en_5g_first_vap_bandwidth = pst_mac_vap->st_channel.en_bandwidth;
                    } else {
                        /* ��һ��vap������֮��,������ͬƵ��vap�ŵ��ʹ��������һ�� */
                        oam_info_log0(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                                      "hmac_obss_init_scan_hook:follow bandwidth of first 5g vap.\n");
                        pst_mac_vap->st_channel.uc_chan_number = uc_5g_first_vap_chan_number;
                        pst_mac_vap->st_channel.en_bandwidth = en_5g_first_vap_bandwidth;
                    }
                }
            } else {
                OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_2040, "hmac_obss_init_scan_hook:en_band=%d not support",
                                 pst_mac_vap->st_channel.en_band);
                continue;
            }

            /* ��¼�ŵ��ʹ��� */
            pst_dev->ast_best_channel[pst_mac_vap->st_channel.en_band].uc_chan_number =
                pst_mac_vap->st_channel.uc_chan_number;
            pst_dev->ast_best_channel[pst_mac_vap->st_channel.en_band].en_bandwidth =
                pst_mac_vap->st_channel.en_bandwidth;
            pst_dev->pst_device_base_info->uc_max_channel = pst_mac_vap->st_channel.uc_chan_number;
            pst_mac_vap->st_ch_switch_info.en_user_pref_bandwidth = pst_mac_vap->st_channel.en_bandwidth;
            oam_info_log2(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                          "hmac_obss_init_scan_hook:After adjust, channel=%d, bandwidth=%d.\n",
                          pst_mac_vap->st_channel.uc_chan_number, pst_mac_vap->st_channel.en_bandwidth);
        }
    }

    return OAL_SUCC;
}

oal_bool_enum_uint8 hmac_device_in_init_scan(mac_device_stru *pst_mac_device)
{
    hmac_device_stru *pst_hmac_device = OAL_PTR_NULL;

    pst_hmac_device = hmac_res_get_mac_dev(pst_mac_device->uc_device_id);
    if (pst_hmac_device == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_SCAN, "hmac_res_get_mac_dev:pst_hmac_device is null\n");
        return OAL_FALSE;
    } else {
        return pst_hmac_device->en_in_init_scan;
    }
}


mac_need_init_scan_res_enum_uint8 hmac_need_init_scan(hmac_device_stru *pst_hmac_device,
                                                      mac_vap_stru *pst_in_mac_vap,
                                                      mac_try_init_scan_type en_type)
{
    mac_need_init_scan_res_enum_uint8 en_result = MAC_INIT_SCAN_NOT_NEED;
    mac_device_stru *pst_mac_device = pst_hmac_device->pst_device_base_info;

    if (oal_unlikely(pst_mac_device == OAL_PTR_NULL)) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_need_init_scan::pst_device_base_info null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if ((pst_in_mac_vap->en_vap_mode != WLAN_VAP_MODE_BSS_AP) &&
        (pst_in_mac_vap->en_vap_mode != WLAN_VAP_MODE_CONFIG)) {
        return MAC_INIT_SCAN_NOT_NEED;
    }

    if ((en_type == MAC_TRY_INIT_SCAN_VAP_UP) && !pst_hmac_device->en_start_via_priv) {
        return MAC_INIT_SCAN_NOT_NEED;
    }

    if (pst_hmac_device->en_in_init_scan) {
        return MAC_TRY_INIT_SCAN_RESCAN == en_type ? MAC_INIT_SCAN_NOT_NEED : MAC_INIT_SCAN_IN_SCAN;
    }
#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    en_result |= mac_get_2040bss_switch(pst_mac_device);
#endif

    return en_result;
}


oal_uint32 hmac_init_scan_timeout(void *p_arg)
{
    hmac_device_stru *pst_dev = (hmac_device_stru *)p_arg;

    if (!pst_dev->en_init_scan) {
        return OAL_SUCC;
    }

    pst_dev->en_init_scan = OAL_FALSE;
    pst_dev->en_in_init_scan = OAL_FALSE;
    hmac_start_all_bss_of_device(pst_dev);

    return OAL_SUCC;
}


oal_void hmac_init_scan_cancel_timer(hmac_device_stru *pst_hmac_dev)
{
    if ((pst_hmac_dev != NULL) && pst_hmac_dev->st_scan_mgmt.st_init_scan_timeout.en_is_enabled) {
        /* �رճ�ʱ��ʱ�� */
        frw_immediate_destroy_timer(&pst_hmac_dev->st_scan_mgmt.st_init_scan_timeout);
    }

    return;
}


oal_void hmac_init_scan_cb(void *p_scan_record)
{
    hmac_scan_record_stru *pst_scan_record = (hmac_scan_record_stru *)p_scan_record;
    oal_uint8 uc_device_id = pst_scan_record->uc_device_id;
    hmac_device_stru *pst_hmac_dev = hmac_res_get_mac_dev(uc_device_id);
    mac_device_stru *pst_mac_dev = NULL;
    oal_uint32 ul_ret;

    oam_warning_log0(0, OAM_SF_ACS, "{hmac_init_scan_cb called}");
    if (pst_hmac_dev == OAL_PTR_NULL) {
        OAM_ERROR_LOG1(0, OAM_SF_ACS, "{hmac_init_scan_cb:pst_hmac_dev=NULL, device_id=%d}", uc_device_id);
        return;
    }
    pst_mac_dev = pst_hmac_dev->pst_device_base_info;
    if ((pst_hmac_dev == OAL_PTR_NULL) || (pst_mac_dev == OAL_PTR_NULL)) {
        OAM_ERROR_LOG1(0, OAM_SF_ACS, "{hmac_init_scan_cb:pst_mac_dev=NULL, device_id=%d}", uc_device_id);
        return;
    }
    oam_warning_log0(0, OAM_SF_ACS, "acs not enable, cancel timer\n");
    /* ACSδִ�У���ʱ�������˽��� */
    hmac_init_scan_cancel_timer(pst_hmac_dev);
    /* ��ACSδִ�л���ִ��ʧ�ܣ�����ִ�к������� */
#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    if (mac_get_2040bss_switch(pst_mac_dev)) {
        hmac_obss_init_scan_hook(pst_scan_record, pst_hmac_dev);
    }
#endif
    oam_warning_log3(0, OAM_SF_ACS, "<<< rsp=5:scan report ch=%d bss=%d init=%d\n", pst_scan_record->uc_chan_numbers,
                     pst_scan_record->st_bss_mgmt.ul_bss_num, pst_hmac_dev->en_init_scan);
    /* ACSδ���С�DFSδ���У�ֱ������BSS */
    ul_ret = hmac_init_scan_timeout(pst_hmac_dev);
    if (ul_ret != OAL_SUCC) {
    }
}


oal_uint32 hmac_init_scan_do(hmac_device_stru *pst_hmac_dev, mac_vap_stru *pst_mac_vap,
                             mac_init_scan_req_stru *pst_cmd)
{
    oal_uint8 uc_idx, uc_cnt;
    mac_scan_req_stru st_scan_req;
    wlan_channel_band_enum_uint8 en_band;
    wlan_channel_bandwidth_enum_uint8 en_bandwidth;
    oal_uint32 ul_ret = OAL_FAIL;
    hmac_vap_stru *pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);

    if (pst_hmac_vap == OAL_PTR_NULL) {
        OAM_ERROR_LOG1(0, OAM_SF_ACS, "{hmac_init_scan_do:get hmac vap=NULL, vapid=%d}", pst_mac_vap->uc_vap_id);
        return OAL_FAIL;
    }

    memset_s(&st_scan_req, OAL_SIZEOF(st_scan_req), 0, OAL_SIZEOF(st_scan_req));
    st_scan_req.en_scan_mode = pst_cmd->auc_arg[0];
    st_scan_req.en_scan_type = pst_cmd->auc_arg[1];
    st_scan_req.uc_scan_func = pst_cmd->auc_arg[2]; /* auc_arg��2�ֽڱ�ʾDMAC SCANNER ɨ��ģʽ */
    st_scan_req.en_bss_type = WLAN_MIB_DESIRED_BSSTYPE_INFRA;
    st_scan_req.uc_bssid_num = 0;
    st_scan_req.uc_ssid_num = 0;

    st_scan_req.uc_max_scan_count_per_channel = 1;
    /* ɨ��ģʽ����ǰ��ɨ�裬ÿ���ŵ�����ɨ������֡�ĸ�����2 */
    st_scan_req.uc_max_send_probe_req_count_per_channel = st_scan_req.en_scan_mode == WLAN_SCAN_MODE_FOREGROUND ? 2 :
                                                          1;
    /* ɨ��ģʽ����ǰ��ɨ�裬ɨ�����ʱ����120ms��������30ms */
    st_scan_req.us_scan_time = st_scan_req.en_scan_mode == WLAN_SCAN_MODE_FOREGROUND ? 120 : 30;
    st_scan_req.uc_probe_delay = 0;
    st_scan_req.uc_vap_id = pst_mac_vap->uc_vap_id; /* ��ʵ�Ǹ�device�µ�vap_id[0] */
    st_scan_req.p_fn_cb = hmac_init_scan_cb;

    st_scan_req.uc_channel_nums = 0; /* �ŵ��б����ŵ��ĸ��� */
    uc_cnt = 0;
    /* 2��ʾÿ�������ӡen_band��en_bandwidthռ1���ֽڣ�auc_data[uc_idx + 1]ռ1���ֽ� */
    for (uc_idx = 0; uc_idx < pst_cmd->ul_cmd_len; uc_idx += 2) {
        en_band = pst_cmd->auc_data[uc_idx] & 0X0F;
        en_bandwidth = (pst_cmd->auc_data[uc_idx] >> 4) & 0x0F; /* auc_data[uc_idx]��4λ��Ϊbandwidth */
        if (mac_is_channel_num_valid(en_band, pst_cmd->auc_data[uc_idx + 1]) != OAL_SUCC) {
            oam_warning_log3(0, OAM_SF_ACS, "{hmac_init_scan_do:invalid channel number, ch=%d, band=%d bw=%d}",
                             pst_cmd->auc_data[uc_idx + 1], en_band, en_bandwidth);
            OAL_IO_PRINT("{hmac_init_scan_do:invalid channel number, ch=%d, band=%d bw=%d}\n",
                         pst_cmd->auc_data[uc_idx + 1], en_band, en_bandwidth);

            continue;
        }

        st_scan_req.ast_channel_list[uc_cnt].uc_chan_number = pst_cmd->auc_data[uc_idx + 1];
        st_scan_req.ast_channel_list[uc_cnt].en_band = en_band;
        st_scan_req.ast_channel_list[uc_cnt].en_bandwidth = en_bandwidth;
        ul_ret = mac_get_channel_idx_from_num(st_scan_req.ast_channel_list[uc_cnt].en_band,
                                              st_scan_req.ast_channel_list[uc_cnt].uc_chan_number,
                                              &st_scan_req.ast_channel_list[uc_cnt].uc_idx);
        if (ul_ret != OAL_SUCC) {
            oam_warning_log2(0, OAM_SF_ACS, "{hmac_init_scan_do:mac_get_channel_idx_from_num failed en_band:[%d], \
                chan_number:[%d]}", st_scan_req.ast_channel_list[uc_cnt].en_band,
                             st_scan_req.ast_channel_list[uc_cnt].uc_chan_number);
        }
        uc_cnt++;
    }

    OAL_IO_PRINT(">>> got=5:do scan mode=%d func=0x%x type=%d ch_cnt=%d\n",
                 st_scan_req.en_scan_mode,
                 st_scan_req.uc_scan_func, st_scan_req.en_scan_type, uc_cnt);

    if (uc_cnt != 0) {
        st_scan_req.uc_channel_nums = uc_cnt;

        /* ֱ�ӵ���ɨ��ģ��ɨ���������� */
        ul_ret = hmac_scan_proc_scan_req_event(pst_hmac_vap, &st_scan_req);
        if (ul_ret != OAL_SUCC) {
            OAM_WARNING_LOG1(0, OAM_SF_SCAN, "hmac_init_scan_do:hmac_scan_add_req failed, ret=%d", ul_ret);
        }
    } else {
        oam_warning_log0(0, OAM_SF_SCAN, "hmac_init_scan_do:no valid channel found, not scan");
    }

    return ul_ret;
}

oal_bool_enum_uint8 hmac_init_scan_skip_channel(hmac_device_stru *pst_hmac_dev,
                                                wlan_channel_band_enum_uint8 en_band,
                                                oal_uint8 uc_idx)
{
    /* skip any illegal channel */
    if (mac_is_channel_idx_valid(en_band, uc_idx) != OAL_SUCC) {
        return OAL_TRUE;
    }
    if (oal_unlikely(pst_hmac_dev->pst_device_base_info == OAL_PTR_NULL)) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_init_scan_skip_channel::pst_device_base_info null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (pst_hmac_dev->pst_device_base_info && (pst_hmac_dev->pst_device_base_info->en_max_band != en_band)) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}

oal_uint32 hmac_init_scan_process(hmac_device_stru *pst_hmac_dev, mac_vap_stru *pst_mac_vap,
                                  mac_scan_op_enum_uint8 en_op)
{
    /* 4��auc_data�ĳ��ȣ�wifi 5G 2.4Gȫ���ŵ�������2�� */
    oal_uint8 ast_buf[OAL_SIZEOF(mac_acs_cmd_stru) - 4 + WLAN_MAX_CHANNEL_NUM * 2];
    oal_uint8 uc_idx;
    oal_uint8 *puc_dat, uc_tot, uc_chan_num;
    oal_uint32 ul_ret;

    mac_init_scan_req_stru *pst_cmd = (mac_init_scan_req_stru *)ast_buf;

    puc_dat = pst_cmd->auc_data;

    pst_hmac_dev->en_init_scan = (en_op == MAC_SCAN_OP_INIT_SCAN) ? OAL_TRUE : OAL_FALSE;
    pst_hmac_dev->en_in_init_scan = OAL_TRUE;

    pst_cmd->uc_cmd = 5;  /* 5 ��ʾDMAC_ACS_CMD_DO_SCAN; */
    pst_cmd->auc_arg[0] = ((en_op == MAC_SCAN_OP_FG_SCAN_ONLY) ||
                           (en_op == MAC_SCAN_OP_INIT_SCAN))
                          ? WLAN_SCAN_MODE_FOREGROUND
                          : WLAN_SCAN_MODE_BACKGROUND_AP;

    pst_cmd->auc_arg[1] = WLAN_SCAN_TYPE_ACTIVE;
    /* auc_arg��2�ֽڱ�ʾDMAC SCANNER ɨ��ģʽMAC_SCAN_FUNC_MEAS �� MAC_SCAN_FUNC_STATS �� MAC_SCAN_FUNC_BSS 3�� */
    pst_cmd->auc_arg[2] = MAC_SCAN_FUNC_MEAS | MAC_SCAN_FUNC_STATS | MAC_SCAN_FUNC_BSS;
    pst_cmd->ul_cmd_cnt = 0;

    uc_tot = 0;
    for (uc_idx = 0; uc_idx < MAC_CHANNEL_FREQ_2_BUTT; uc_idx++) {
        if (!hmac_init_scan_skip_channel(pst_hmac_dev, WLAN_BAND_2G, uc_idx)) {
            mac_get_channel_num_from_idx(WLAN_BAND_2G, uc_idx, &uc_chan_num);
            *puc_dat++ = ((WLAN_BAND_WIDTH_20M) << 4) | WLAN_BAND_2G; /* 2G 20M����WLAN_BAND_WIDTH_20M����4λ�� */
            *puc_dat++ = uc_chan_num;
            uc_tot++;
        }
    }
    for (uc_idx = 0; uc_idx < MAC_CHANNEL_FREQ_5_BUTT; uc_idx++) {
        if (!hmac_init_scan_skip_channel(pst_hmac_dev, WLAN_BAND_5G, uc_idx)) {
            mac_get_channel_num_from_idx(WLAN_BAND_5G, uc_idx, &uc_chan_num);
            *puc_dat++ = ((WLAN_BAND_WIDTH_20M) << 4) | WLAN_BAND_5G; /* 5G 20M����WLAN_BAND_WIDTH_20M����4λ�� */
            *puc_dat++ = uc_chan_num;
            uc_tot++;
        }
    }

    pst_cmd->ul_cmd_len = uc_tot * 2; /* �ܳ�����2��uc_tot(��ָauc_data��ʵ�ʸ��س���) */

    /* best�ŵ����Ϊ0����start bss��Ϊ�Ϸ����ж����� */
    if (pst_hmac_dev->en_init_scan) {
        memset_s(pst_hmac_dev->ast_best_channel, OAL_SIZEOF(pst_hmac_dev->ast_best_channel),
                 0, OAL_SIZEOF(pst_hmac_dev->ast_best_channel));
    }

    /* ������ʱ��, ��ʱ��ǿ������BSS: ����ʱ�����ɨ��ʱ����APP����ʱ�� */
    {
        frw_create_timer(&pst_hmac_dev->st_scan_mgmt.st_init_scan_timeout,
                         hmac_init_scan_timeout,
                         HMAC_INIT_SCAN_TIMEOUT_MS,
                         pst_hmac_dev,
                         OAL_FALSE,
                         OAM_MODULE_ID_HMAC,
                         pst_hmac_dev->pst_device_base_info->ul_core_id);
    }

    ul_ret = hmac_init_scan_do(pst_hmac_dev, pst_mac_vap, pst_cmd);
    if (ul_ret != OAL_SUCC) {
        pst_hmac_dev->en_init_scan = OAL_FALSE;
        pst_hmac_dev->en_in_init_scan = OAL_FALSE;
    }

    return ul_ret;
}

oal_uint32 hmac_init_scan_try(mac_device_stru *pst_mac_device, mac_vap_stru *pst_in_mac_vap,
                              mac_try_init_scan_type en_type)
{
    oal_uint8 uc_idx;
    mac_vap_stru *pst_mac_vap = NULL;
    mac_vap_stru *pst_mac_vap_scan = NULL;
    hmac_vap_stru *pst_hmac_vap = NULL;
    oal_uint8 en_scan_type;
    hmac_device_stru *pst_hmac_device = hmac_res_get_mac_dev(pst_mac_device->uc_device_id);
    oal_bool_enum_uint8 en_need_do_init_scan = OAL_FALSE;

    if (pst_hmac_device == OAL_PTR_NULL) {
        OAM_WARNING_LOG1(0, OAM_SF_SCAN, "{hmac_init_scan_try:mac_res_get_hmac_vap failed. device_id:%d.}",
                         pst_mac_device->uc_device_id);
        return OAL_FAIL;
    }
    en_scan_type = hmac_need_init_scan(pst_hmac_device, pst_in_mac_vap, en_type);
    if (en_scan_type == MAC_INIT_SCAN_NOT_NEED) {
        return OAL_FAIL;
    } else if (en_scan_type == MAC_INIT_SCAN_IN_SCAN) {
        OAL_IO_PRINT("just in init scan\n");

        mac_vap_init_rates(pst_in_mac_vap);
        pst_hmac_vap = mac_res_get_hmac_vap(pst_in_mac_vap->uc_vap_id);
        if (pst_hmac_vap == OAL_PTR_NULL) {
            OAM_WARNING_LOG1(0, OAM_SF_SCAN, "{hmac_init_scan_try:mac_res_get_hmac_vap failed vap_id:%d.}",
                             pst_in_mac_vap->uc_vap_id);
            return OAL_FAIL;
        }
        if (pst_hmac_vap->st_vap_base_info.en_vap_state == MAC_VAP_STATE_UP) {
            hmac_fsm_change_state(pst_hmac_vap, MAC_VAP_STATE_AP_WAIT_START);
        }

        return OAL_SUCC;
    } else {
        oam_warning_log0(0, OAM_SF_SCAN, "hmac_init_scan_try: need init scan");
    }

    for (uc_idx = 0; uc_idx < pst_hmac_device->pst_device_base_info->uc_vap_num; uc_idx++) {
        pst_mac_vap = mac_res_get_mac_vap(pst_hmac_device->pst_device_base_info->auc_vap_id[uc_idx]);
        if (pst_mac_vap == NULL) {
            continue;
        }

        if (pst_mac_vap->en_vap_mode == WLAN_VAP_MODE_BSS_AP) {
            mac_vap_init_rates(pst_mac_vap);

            /* ǿ������AP��״̬��Ϊ WAIT_START����Ϊ��Ҫִ�г�ʼ�ŵ���� */
            pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
            if (pst_hmac_vap == OAL_PTR_NULL) {
                oam_warning_log0(pst_mac_vap->uc_vap_id, OAM_SF_SCAN, "hmac_init_scan_try:pst_hmac_vap is null");
                continue;
            }
            switch (pst_hmac_vap->st_vap_base_info.en_vap_state) {
                case MAC_VAP_STATE_UP:
                case MAC_VAP_STATE_PAUSE:  // dbac
                    hmac_fsm_change_state(pst_hmac_vap, MAC_VAP_STATE_AP_WAIT_START);
                // no break here!
                case MAC_VAP_STATE_AP_WAIT_START:
                    en_need_do_init_scan = OAL_TRUE;
                    pst_mac_vap_scan = pst_mac_vap;
                    break;
                default:
                    break;
            }
        }
    }

    if (en_need_do_init_scan) {
        return hmac_init_scan_process(pst_hmac_device, pst_mac_vap_scan, MAC_SCAN_OP_INIT_SCAN);
    }

    return OAL_SUCC;
}

oal_bool_enum_uint8 hmac_chan_is_this_40MHz_set_possible(
    mac_device_stru *pst_mac_device, mac_vap_stru *pst_mac_vap, oal_uint8 uc_pri_chan_idx,
    mac_sec_ch_off_enum_uint8 en_sec_chan_offset, hmac_eval_scan_report_stru *pst_chan_scan_report)
{
    mac_ap_ch_info_stru *pst_ap_channel_list = OAL_PTR_NULL;
    wlan_channel_band_enum_uint8 en_band = pst_mac_vap->st_channel.en_band;
    oal_uint8 uc_num_supp_chan = mac_get_num_supp_channel(en_band);
    oal_uint8 uc_af_ch_idx_offset = mac_get_affected_ch_idx_offset(en_band);
    oal_uint8 uc_sec_ch_idx_offset = mac_get_sec_ch_idx_offset(en_band);
    oal_uint8 uc_ch_idx;
    oal_uint8 uc_sec_chan_idx;
    oal_uint8 uc_affected_chan_lo = 0;
    oal_uint8 uc_affected_chan_hi = 0;
    hmac_network_type_enum_uint8 en_network_type;
    hmac_chan_op_enum_uint8 en_allowed_bit;
    oal_bool_enum_uint8 en_flag_2040_op_permitted = OAL_TRUE;
    oal_uint32 ul_ret;

    ul_ret = mac_is_channel_idx_valid(en_band, uc_pri_chan_idx);
    if (ul_ret != OAL_SUCC) {
        return OAL_FALSE;
    }

    if (en_sec_chan_offset == MAC_SCA) {
        /* ������ŵ����� */
        if (uc_num_supp_chan > uc_pri_chan_idx + uc_sec_ch_idx_offset) {
            uc_sec_chan_idx = uc_pri_chan_idx + uc_sec_ch_idx_offset;
        } else {
            return OAL_FALSE;
        }

        en_network_type = HMAC_NETWORK_SCA;
        en_allowed_bit = HMAC_SCA_ALLOWED;

        /* ������Ӱ����ŵ��������� */
        uc_affected_chan_lo = (uc_pri_chan_idx >= uc_af_ch_idx_offset) ? (uc_pri_chan_idx - uc_af_ch_idx_offset) : 0;

        /* ������Ӱ����ŵ��������� */
        uc_affected_chan_hi = (uc_num_supp_chan > uc_sec_chan_idx + uc_af_ch_idx_offset) ?
                              (uc_sec_chan_idx + uc_af_ch_idx_offset) : (uc_num_supp_chan - 1);
    } else if (en_sec_chan_offset == MAC_SCB) {
        /* ������ŵ����� */
        if (uc_pri_chan_idx >= uc_sec_ch_idx_offset) {
            uc_sec_chan_idx = uc_pri_chan_idx - uc_sec_ch_idx_offset;
        } else {
            return OAL_FALSE;
        }

        en_network_type = HMAC_NETWORK_SCB;
        en_allowed_bit = HMAC_SCB_ALLOWED;

        /* ������Ӱ����ŵ��������� */
        uc_affected_chan_lo = (uc_sec_chan_idx >= uc_af_ch_idx_offset) ? (uc_sec_chan_idx - uc_af_ch_idx_offset) : 0;

        /* ������Ӱ����ŵ��������� */
        uc_affected_chan_hi = (uc_num_supp_chan > uc_pri_chan_idx + uc_af_ch_idx_offset) ?
                              (uc_pri_chan_idx + uc_af_ch_idx_offset) : (uc_num_supp_chan - 1);
    } else {
        return OAL_FALSE;
    }

    ul_ret = mac_is_channel_idx_valid(en_band, uc_sec_chan_idx);
    if (ul_ret != OAL_SUCC) {
        return OAL_FALSE;
    }

    if (uc_affected_chan_hi >= MAC_MAX_SUPP_CHANNEL) {
        return OAL_FALSE;
    }

    /* ���ڸ�����"���ŵ� + ���ŵ�ƫ����"�������ķ�Χ��(����Ƶ�� +/- 5���ŵ�)���ж��ܷ���40MHz BSS */
    for (uc_ch_idx = uc_affected_chan_lo; uc_ch_idx <= uc_affected_chan_hi; uc_ch_idx++) {
        ul_ret = mac_is_channel_idx_valid(en_band, uc_ch_idx);
        if (ul_ret != OAL_SUCC) {
            continue;
        }

        pst_ap_channel_list = &(pst_mac_device->st_ap_channel_list[uc_ch_idx]);

        /* ��������ŵ��ϴ���BSS */
        if (pst_ap_channel_list->en_ch_type != MAC_CH_TYPE_NONE) {
            /* �ۼ������ŵ���ɨ�赽��BSS���� */
            pst_chan_scan_report[uc_pri_chan_idx].aus_num_networks[en_network_type] +=
            pst_ap_channel_list->us_num_networks;

            /* ��BSS�����ŵ����������е�20/40MHz BSS�����ŵ��غ� */
            if (uc_ch_idx == uc_pri_chan_idx) {
                if (pst_ap_channel_list->en_ch_type == MAC_CH_TYPE_PRIMARY) {
                    continue;
                }
            }

            /* ��BSS�Ĵ��ŵ����������е�20/40MHz BSS�Ĵ��ŵ��غ� */
            if (uc_ch_idx == uc_sec_chan_idx) {
                if (pst_ap_channel_list->en_ch_type == MAC_CH_TYPE_SECONDARY) {
                    continue;
                }
            }

            en_flag_2040_op_permitted = OAL_FALSE;
        }
    }

    /* ���20/40MHz����û��ʹ�ܣ�����Ϊ���Խ���40MHz BSS�������û�������"40MHz������"λ */
    if ((mac_mib_get_2040BSSCoexistenceManagementSupport(pst_mac_vap) == OAL_FALSE) &&
        (mac_mib_get_FortyMHzIntolerant(pst_mac_vap) == OAL_FALSE)) {
        en_flag_2040_op_permitted = OAL_TRUE;
    } else if (en_band == WLAN_BAND_5G) { /* 5GHz����²�����20/40M�������ú�40M���������ã�һ������40MHz BSS */
        en_flag_2040_op_permitted = OAL_TRUE;
    }

    if (en_flag_2040_op_permitted == OAL_TRUE) {
        pst_chan_scan_report[uc_pri_chan_idx].en_chan_op |= en_allowed_bit;
        return OAL_TRUE;
    }

    return OAL_FALSE;
}


oal_bool_enum_uint8 hmac_chan_get_2040_op_chan_list(mac_vap_stru *pst_mac_vap,
                                                    hmac_eval_scan_report_stru *pst_chan_scan_report)
{
    mac_device_stru       *pst_mac_device = OAL_PTR_NULL;
    oal_uint8              uc_chan_idx;
    oal_uint8              uc_num_supp_chan = mac_get_num_supp_channel(pst_mac_vap->st_channel.en_band);
    oal_bool_enum_uint8    en_fortymhz_poss = OAL_FALSE;
    oal_bool_enum_uint8    en_flag;

    oam_info_log0(pst_mac_vap->uc_vap_id, OAM_SF_2040, "{hmac_chan_get_2040_op_chan_list}");

    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (oal_unlikely(pst_mac_device == OAL_PTR_NULL)) {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_SCAN,
                       "{hmac_chan_get_2040_op_chan_list::pst_mac_device null.}");
        return OAL_FALSE;
    }

    for (uc_chan_idx = 0; uc_chan_idx < uc_num_supp_chan; uc_chan_idx++) {
        /* �ж��ܷ���SCA���͵�40MHz BSS */
        en_flag = hmac_chan_is_this_40MHz_set_possible(pst_mac_device, pst_mac_vap, uc_chan_idx, MAC_SCA,
                                                       pst_chan_scan_report);
        if (en_flag == OAL_TRUE) {
            en_fortymhz_poss = OAL_TRUE;
        }

        /* �ж��ܷ���SCB���͵�40MHz BSS */
        en_flag = hmac_chan_is_this_40MHz_set_possible(pst_mac_device, pst_mac_vap, uc_chan_idx,
                                                       MAC_SCB, pst_chan_scan_report);
        if (en_flag == OAL_TRUE) {
            en_fortymhz_poss = OAL_TRUE;
        }
    }

    /* 2.4GHz�£����"40MHz������"λ�Ƿ����ã���������40MHz BSS */
    if (pst_mac_vap->st_channel.en_band == WLAN_BAND_2G) {
        if ((pst_mac_device->en_40MHz_intol_bit_recd == OAL_TRUE) ||
            (mac_mib_get_FortyMHzIntolerant(pst_mac_vap) == OAL_TRUE)) {
            en_fortymhz_poss = OAL_FALSE;
        }
    }

    return en_fortymhz_poss;
}


oal_uint32 hmac_chan_update_40m_intol_user(mac_vap_stru *pst_mac_vap,
                                           mac_user_stru *pst_mac_user, oal_bool_enum_uint8 en_40m_intol_bit)
{
    hmac_vap_stru   *pst_hmac_vap;
    oal_uint16       us_user_id;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (pst_hmac_vap == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                       "{hmac_chan_update_40m_intol_user::pst_hmac_vap null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    us_user_id = pst_mac_user->us_assoc_id;
    if (us_user_id >= 32 * MAC_DEV_MAX_40M_INTOL_USER_BITMAP_LEN) { /* 32��ʾ�û�����Դ���������ֵ */
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                       "{hmac_chan_update_40m_intol_user:: user id exceed max user id %d}",
                       us_user_id);
        return OAL_ERR_CODE_BUTT;
    }

    if (en_40m_intol_bit == OAL_TRUE) {
        /* 32��ʾ�û�����Դ���������ֵ */
        pst_hmac_vap->aul_40m_intol_user_bitmap[us_user_id / 32] |= (1 << (us_user_id % 32));
    } else {
        /* 32��ʾ�û�����Դ���������ֵ */
        pst_hmac_vap->aul_40m_intol_user_bitmap[us_user_id / 32] &= ~((oal_uint32)(1 << (us_user_id % 32)));
    }

    oam_info_log4(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                  "{hmac_chan_update_40m_intol_user:: bitmap 0x%x 0x%x 0x%x 0x%x}",
                  pst_hmac_vap->aul_40m_intol_user_bitmap[0], pst_hmac_vap->aul_40m_intol_user_bitmap[1],
                  /* aul_40M_intol_user_bitmap��0��1��2��3byteΪ�������log */
                  pst_hmac_vap->aul_40m_intol_user_bitmap[2], pst_hmac_vap->aul_40m_intol_user_bitmap[3]);

    return OAL_SUCC;
}


oal_void hmac_chan_prepare_for_40m_recovery(
    hmac_vap_stru *pst_hmac_vap, wlan_channel_bandwidth_enum_uint8 en_bandwidth)
{
    mac_device_stru *pst_mac_device;

    pst_mac_device = mac_res_get_dev(pst_hmac_vap->st_vap_base_info.uc_device_id);
    if (pst_mac_device == OAL_PTR_NULL) {
        OAM_ERROR_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_2040,
                       "{hmac_chan_prepare_for_40m_recovery::pst_device null,uc_device_id=%d.}",
                       pst_hmac_vap->st_vap_base_info.uc_device_id);
        return;
    }
    pst_mac_device->en_40MHz_intol_bit_recd = OAL_FALSE;

    // st_ap_channel_list�ŵ�?????????????
    /* ����VAP����ģʽΪ20MHz */
    pst_hmac_vap->st_vap_base_info.st_channel.en_bandwidth = en_bandwidth;

    /* ���ô����л�״̬��������������һ��DTIMʱ���л���20MHz���� */
    pst_hmac_vap->st_vap_base_info.st_ch_switch_info.en_bw_switch_status = WLAN_BW_SWITCH_20_TO_40;

    return;
}


oal_uint32 hmac_chan_40m_recovery_timeout_fn(void *p_arg)
{
    hmac_vap_stru          *pst_hmac_vap = OAL_PTR_NULL;
    mac_vap_stru           *pst_mac_vap = OAL_PTR_NULL;

    oam_info_log0(0, OAM_SF_2040, "{hmac_chan_40m_recovery_timeout_fn::40M recovery timer time out.}");

    pst_hmac_vap = (hmac_vap_stru *)p_arg;
    if (oal_unlikely(pst_hmac_vap == OAL_PTR_NULL)) {
        OAM_ERROR_LOG0(0, OAM_SF_2040, "{hmac_chan_40m_recovery_timeout_fn::pst_dmac_vap null.}");
        return OAL_FAIL;
    }

    /* ���20M��ص���Ϣ */
    if (pst_hmac_vap->en_40m_bandwidth == WLAN_BAND_WIDTH_20M) { /* ���ap��ʼ����Ϊ20M, �� */
        oam_warning_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_2040,
                         "{hmac_chan_40m_recovery_timeout_fn::no need 40M recovery because init 20M.}");
        return OAL_SUCC;
    }

    if (pst_hmac_vap->aul_40m_intol_user_bitmap[0] | pst_hmac_vap->aul_40m_intol_user_bitmap[1]
        /* aul_40M_intol_user_bitmap 0��1��2��3byte���л����㣬�ж��Ƿ���� */
        | pst_hmac_vap->aul_40m_intol_user_bitmap[2] | pst_hmac_vap->aul_40m_intol_user_bitmap[3]) {
        oam_warning_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_2040,
                         "{hmac_chan_40m_recovery_timeout_fn::no need 40M recovery because 40M intol sta assoc}");
        return OAL_SUCC;
    }

    pst_mac_vap = &pst_hmac_vap->st_vap_base_info;
    if (pst_mac_vap->st_channel.en_bandwidth != WLAN_BAND_WIDTH_20M) {
        oam_warning_log0(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                         "{hmac_chan_40m_recovery_timeout_fn::no need 40M recovery because already 20M}");
        hmac_chan_stop_40m_recovery_timer(pst_mac_vap);
        return OAL_SUCC;
    }
    hmac_chan_prepare_for_40m_recovery(pst_hmac_vap, pst_hmac_vap->en_40m_bandwidth);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_chan_sync(pst_mac_vap, pst_mac_vap->st_channel.uc_chan_number,
                   pst_mac_vap->st_channel.en_bandwidth, OAL_FALSE);
#endif

    return OAL_SUCC;
}


oal_void hmac_chan_start_40m_recovery_timer(mac_vap_stru *pst_mac_vap)
{
    hmac_vap_stru                 *pst_hmac_vap;
    oal_uint32                     ul_timeout;

    /* ���ݷ���ɨ���vap id��ȡhmac vap */
    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (pst_hmac_vap == OAL_PTR_NULL) {
        oam_warning_log0(0, OAM_SF_2040, "{hmac_chan_start_40m_recovery_timer:: pst_hmac_vap is NULL.}");
        return;
    }

#if 1
    ul_timeout = mac_mib_get_BSSWidthTriggerScanInterval(pst_mac_vap) * 2 * 1000; /* ��ʱʱ��BSSɨ��ʱ���� * 2 *1000 */
#else
    ul_timeout = 10000; /* ��ʱʱ��10000ms */
#endif
    oam_info_log1(0, OAM_SF_2040, "{hmac_chan_start_40m_recovery_timer::ul_timeout=%d}", ul_timeout);
    if (pst_hmac_vap->st_40m_recovery_timer.en_is_registerd == OAL_FALSE) {
        frw_create_timer(&(pst_hmac_vap->st_40m_recovery_timer),
                         hmac_chan_40m_recovery_timeout_fn,
                         ul_timeout,
                         (void *)pst_hmac_vap,
                         OAL_TRUE,
                         OAM_MODULE_ID_HMAC,
                         pst_mac_vap->ul_core_id);
    } else {
        frw_timer_restart_timer(&(pst_hmac_vap->st_40m_recovery_timer), ul_timeout, OAL_TRUE);
    }

    return;
}


oal_void hmac_chan_stop_40m_recovery_timer(mac_vap_stru *pst_mac_vap)
{
    hmac_vap_stru *pst_hmac_vap;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (pst_hmac_vap == OAL_PTR_NULL) {
        oam_warning_log0(0, OAM_SF_2040, "{hmac_chan_stop_40m_recovery_timer:: pst_hmac_vap is NULL.}");
        return;
    }
    oam_info_log0(0, OAM_SF_2040, "{hmac_chan_stop_40m_recovery_timer}");
    frw_timer_stop_timer(&(pst_hmac_vap->st_40m_recovery_timer));
    return;
}

oal_uint8 hmac_calc_up_and_wait_vap(hmac_device_stru *pst_hmac_dev)
{
    mac_vap_stru *pst_vap = NULL;
    oal_uint8 uc_vap_idx;
    oal_uint8 ul_up_ap_num = 0;
    mac_device_stru *pst_mac_device = NULL;

    if (pst_hmac_dev->pst_device_base_info == OAL_PTR_NULL) {
        return 0;
    }

    pst_mac_device = pst_hmac_dev->pst_device_base_info;

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++) {
        pst_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (pst_vap == OAL_PTR_NULL) {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "vap is null, vap id is %d", pst_mac_device->auc_vap_id[uc_vap_idx]);
            continue;
        }

        if ((pst_vap->en_vap_state == MAC_VAP_STATE_UP) || (pst_vap->en_vap_state == MAC_VAP_STATE_AP_WAIT_START)) {
            ul_up_ap_num++;
        }
    }

    return ul_up_ap_num;
}

oal_uint32 hmac_config_enable_2040bss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_device_stru *pst_mac_device = NULL;
    oal_bool_enum_uint8 en_2040bss_switch;

    if (oal_unlikely((pst_mac_vap == OAL_PTR_NULL) || (puc_param == OAL_PTR_NULL))) {
        oam_error_log2(0, OAM_SF_CFG, "{hmac_config_enable_2040bss::param null,pst_mac_vap=%x puc_param=%x.}",
                       (uintptr_t)pst_mac_vap, (uintptr_t)puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (pst_mac_device == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_config_enable_2040bss:: pst_mac_device is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    en_2040bss_switch = (*puc_param == 0) ? OAL_FALSE : OAL_TRUE;
    mac_set_2040bss_switch(pst_mac_device, en_2040bss_switch);

    oam_info_log1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_config_enable_2040bss:: set 2040bss switch[%d]}",
                  en_2040bss_switch);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_2040BSS_ENABLE, us_len, puc_param);
#else
    return OAL_SUCC;
#endif
}

oal_bool_enum_uint8 hmac_ap_up_rx_should_ingore_2040(
    wlan_channel_band_enum_uint8 en_band, oal_bool_enum_uint8 en_support_2040)
{
    return ((en_band == WLAN_BAND_5G) || (en_support_2040 == OAL_FALSE));
}

oal_void hmac_ap_up_rx_2040_coext(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf)
{
    mac_device_stru *pst_mac_device = NULL;
    mac_vap_stru *pst_mac_vap = &(pst_hmac_vap->st_vap_base_info);
    dmac_rx_ctl_stru *pst_rx_ctrl = NULL;
    oal_uint32 ul_index;
    oal_uint8 uc_coext_info;
    oal_uint8 *puc_data = NULL;
    oal_uint32 ul_ret = OAL_SUCC;

    /* 5GHzƵ�κ��� 20/40 BSS�������֡ */
    if (hmac_ap_up_rx_should_ingore_2040(pst_mac_vap->st_channel.en_band,
                                         mac_mib_get_2040BSSCoexistenceManagementSupport(pst_mac_vap)) == OAL_TRUE) {
        oam_info_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_2040,
                      "{hmac_ap_up_rx_2040_coext::Now in 5GHz.}");
        return;
    }

    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (oal_unlikely(pst_mac_device == OAL_PTR_NULL)) {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_2040, "{hmac_ap_up_rx_2040_coext::pst_mac_device null.}");
        return;
    }

    pst_rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf);

    /* ��ȡ֡��ָ�� */
    puc_data = (oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr + pst_rx_ctrl->st_rx_info.uc_mac_header_len;

    ul_index = MAC_ACTION_OFFSET_ACTION + 1;

    /* 20/40 BSS Coexistence IE */
    if (puc_data[ul_index] == MAC_EID_2040_COEXT) {
        uc_coext_info = puc_data[ul_index + MAC_IE_HDR_LEN];

        /* BIT1 - Forty MHz Intolerant */
        /* BIT2 - 20 MHz BSS Width Request */
        if (uc_coext_info & (BIT1 | BIT2)) {
            hmac_chan_start_40m_recovery_timer(pst_mac_vap);

            /* ���BIT1��BIT2����Ϊ1���ҵ�ǰ�����ŵ���ȴ���20MHz����AP��Ҫ�л���20MHz���� */
            if (pst_mac_vap->st_channel.en_bandwidth > WLAN_BAND_WIDTH_20M) {
                hmac_chan_multi_switch_to_20MHz_ap(pst_hmac_vap);
            } else {
                pst_mac_device->en_40MHz_intol_bit_recd = OAL_TRUE;
            }
        }

        ul_index += (MAC_IE_HDR_LEN + puc_data[ul_index + 1]);
    }

    /* 20/40 BSS Intolerant Channel Report IE */
    if (puc_data[ul_index] == MAC_EID_2040_INTOLCHREPORT) {
        oal_uint8 uc_len = puc_data[ul_index + 1];
        oal_uint8 uc_chan_idx = 0;
        oal_uint8 uc_loop;
        oal_bool_enum_uint8 en_reval_chan = OAL_FALSE;

        ul_index += (MAC_IE_HDR_LEN + 1); /* skip Element ID��Length��Operating Class */
        for (uc_loop = 0; (uc_len >= 1) && (uc_loop < uc_len - 1); uc_loop++, ul_index++) {
            ul_ret = mac_get_channel_idx_from_num(pst_mac_vap->st_channel.en_band, puc_data[ul_index], &uc_chan_idx);
            if (ul_ret != OAL_SUCC) {
                OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_2040,
                                 "{hmac_ap_up_rx_2040_coext::mac_get_channel_idx_from_num failed[%d].}", ul_ret);
                continue;
            }

            ul_ret = mac_is_channel_idx_valid(pst_mac_vap->st_channel.en_band, uc_chan_idx);
            if (ul_ret != OAL_SUCC) {
                oam_warning_log2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_2040,
                                 "hmac_ap_up_rx_2040_coext::chan_indx=%d not valid, return[%d]", uc_chan_idx, ul_ret);
                continue;
            }

            if (pst_mac_device->st_ap_channel_list[uc_chan_idx].en_ch_type != MAC_CH_TYPE_PRIMARY) {
                pst_mac_device->st_ap_channel_list[uc_chan_idx].en_ch_type = MAC_CH_TYPE_PRIMARY;
                en_reval_chan = OAL_TRUE;
            }
        }

        if (en_reval_chan == OAL_TRUE) {
            oam_warning_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_2040,
                             "{hmac_ap_up_rx_2040_coext::Re-evaluation needed as some channel status changed.}");

            /* ��������ŵ����ߴ����л����Ž����л� */
            if (pst_hmac_vap->en_2040_switch_prohibited == 0) {
                hmac_chan_start_40m_recovery_timer(pst_mac_vap);
                /* ���������Ƿ���Ҫ�����ŵ��л� */
                hmac_chan_reval_status(pst_mac_device, pst_mac_vap);
            }
        }
    }
}

oal_bool_enum_uint8 hmac_ap_is_40mhz_intol_bit_set(oal_uint8 *puc_payload, oal_uint16 us_payload_len)
{
    oal_uint16 us_ht_cap_info = 0;
    oal_uint8 *puc_ht_cap = OAL_PTR_NULL;

    if (us_payload_len <= (MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN)) {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_ap_is_40mhz_intol_bit_set::payload_len[%d]}", us_payload_len);
        return OAL_FALSE;
    }

    us_payload_len -= (MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN);
    puc_payload += (MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN);

    puc_ht_cap = mac_find_ie(MAC_EID_HT_CAP, puc_payload, us_payload_len);
    if (puc_ht_cap != OAL_PTR_NULL) {
        us_ht_cap_info = oal_make_word16(puc_ht_cap[MAC_IE_HDR_LEN], puc_ht_cap[MAC_IE_HDR_LEN + 1]);
        /* Forty MHz Intolerant BIT Ϊ1 */
        if (us_ht_cap_info & BIT14) {
            return OAL_TRUE;
        }
    }

    return OAL_FALSE;
}


oal_bool_enum_uint8 hmac_ap_update_2040_chan_info(
    mac_device_stru *pst_mac_device, mac_vap_stru *pst_mac_vap, oal_uint8 *puc_payload,
    oal_uint16 us_payload_len, oal_uint8 uc_pri_chan_idx, mac_sec_ch_off_enum_uint8 en_sec_ch_offset)
{
    wlan_channel_band_enum_uint8 en_band = pst_mac_vap->st_channel.en_band;
    mac_ap_ch_info_stru *pst_ap_ch_list = pst_mac_device->st_ap_channel_list;
    oal_bool_enum_uint8 en_status_change = OAL_FALSE;
    oal_uint8 uc_sec_ch_idx_offset = mac_get_sec_ch_idx_offset(en_band);
    oal_uint8 uc_sec_chan_idx = 0;
    oal_uint32 ul_ret;

    if (hmac_ap_is_40mhz_intol_bit_set(puc_payload, us_payload_len) == OAL_TRUE) {
        pst_mac_device->en_40MHz_intol_bit_recd = OAL_TRUE;
    }

    if (pst_ap_ch_list[uc_pri_chan_idx].en_ch_type != MAC_CH_TYPE_PRIMARY) {
        pst_ap_ch_list[uc_pri_chan_idx].en_ch_type = MAC_CH_TYPE_PRIMARY;
        en_status_change = OAL_TRUE;
    }

    if (en_sec_ch_offset != MAC_SCN) {
        if (en_sec_ch_offset == MAC_SCA) {
            uc_sec_chan_idx = uc_pri_chan_idx + uc_sec_ch_idx_offset;
        } else if (en_sec_ch_offset == MAC_SCB) {
            uc_sec_chan_idx = uc_pri_chan_idx - uc_sec_ch_idx_offset;
        }

        ul_ret = mac_is_channel_idx_valid(en_band, uc_sec_chan_idx);
        if (ul_ret != OAL_SUCC) {
            return en_status_change;
        }

        if (uc_sec_chan_idx >= MAC_MAX_SUPP_CHANNEL) {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                             "{hmac_ap_update_2040_chan_info::invalid uc_sec_chan_idx[%d].}", uc_sec_chan_idx);
            return en_status_change;
        }

        if (pst_ap_ch_list[uc_sec_chan_idx].en_ch_type != MAC_CH_TYPE_PRIMARY) {
            if (pst_ap_ch_list[uc_sec_chan_idx].en_ch_type != MAC_CH_TYPE_SECONDARY) {
                pst_ap_ch_list[uc_sec_chan_idx].en_ch_type = MAC_CH_TYPE_SECONDARY;
                en_status_change = OAL_TRUE;
            }
        }
    }

    return en_status_change;
}


oal_void hmac_ap_process_obss_40mhz_intol(
    hmac_vap_stru *pst_hmac_vap, oal_uint8 *puc_payload, oal_uint16 us_payload_len)
{
    mac_device_stru *pst_mac_device = NULL;

    /* ��HTվ�㲻֧��OBSS Beacon/Probe Response֡�Ĵ��� */
    if ((mac_mib_get_HighThroughputOptionImplemented(&(pst_hmac_vap->st_vap_base_info)) == OAL_FALSE)) {
        oam_info_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                      "{hmac_ap_process_obss_40mhz_intol::HT not support.}");
        return;
    }

    pst_mac_device = mac_res_get_dev(pst_hmac_vap->st_vap_base_info.uc_device_id);
    if (oal_unlikely(pst_mac_device == OAL_PTR_NULL)) {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                       "{hmac_ap_process_obss_40mhz_intol::pst_mac_device null.}");
        return;
    }

    if (hmac_ap_is_40mhz_intol_bit_set(puc_payload, us_payload_len) == OAL_TRUE) {
        hmac_chan_start_40m_recovery_timer(&pst_hmac_vap->st_vap_base_info);
        /* ��BSS��ǰ�����ŵ���ȴ���20MHz����AP��Ҫ�л���20MHz���� */
        if ((pst_hmac_vap->st_vap_base_info.st_channel.en_bandwidth > WLAN_BAND_WIDTH_20M) &&
            (pst_hmac_vap->st_vap_base_info.st_ch_switch_info.en_bw_switch_status == WLAN_BW_SWITCH_DONE)) {
            oam_info_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                          "{hmac_ap_process_obss_40mhz_intol::40MHz Intolerant AP in neighbourhood.}");
            hmac_chan_multi_switch_to_20MHz_ap(pst_hmac_vap);
        } else {
            pst_mac_device->en_40MHz_intol_bit_recd = OAL_TRUE;
        }
    }
}

oal_void hmac_rx_notify_channel_width(mac_vap_stru *pst_mac_vap, oal_netbuf_stru *pst_netbuf)
{
    hmac_user_stru *pst_hmac_user = NULL;
    dmac_rx_ctl_stru *pst_rx_ctrl = NULL;
    oal_uint8 auc_sta_addr[WLAN_MAC_ADDR_LEN];
    oal_uint8 *puc_data = NULL;
    oal_uint32 ul_ret;

    if (mac_mib_get_HighThroughputOptionImplemented(pst_mac_vap) == OAL_FALSE) {
        return;
    }

    pst_rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf);

    /* ��ȡ���Ͷ˵�ַ */
    mac_get_address2((oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr, auc_sta_addr);

    /* ���ݷ��Ͷ˵��ҵ���ӦAP USER�ṹ */
    pst_hmac_user = mac_vap_get_hmac_user_by_addr(pst_mac_vap, auc_sta_addr);
    if (pst_hmac_user == OAL_PTR_NULL) {
        oam_warning_log0(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                         "{hmac_rx_notify_channel_width::pst_hmac_user null.}");
        return;
    }

    /* ��ȡ֡��ָ�� */
    puc_data = (oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr + pst_rx_ctrl->st_rx_info.uc_mac_header_len;

    /* ���µ�"STA Channel Width" field */
    mac_ie_proc_chwidth_field(pst_mac_vap, &(pst_hmac_user->st_user_base_info),
                              (puc_data[MAC_ACTION_OFFSET_ACTION + 1] & BIT0));

    /* �����û�֧�ִ���������Э�̳���ǰ����dmac offload�ܹ��£�ͬ��������Ϣ��device */
    ul_ret = hmac_config_user_info_syn(pst_mac_vap, &(pst_hmac_user->st_user_base_info));
    if (ul_ret != OAL_SUCC) {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_rx_notify_channel_width::usr_info_syn failed[%d].}", ul_ret);
    }
}

oal_void hmac_update_join_req_params_2040(mac_vap_stru *pst_mac_vap, mac_bss_dscr_stru *pst_bss_dscr)
{
    if (((mac_mib_get_HighThroughputOptionImplemented(pst_mac_vap) == OAL_FALSE) &&
         (mac_mib_get_VHTOptionImplemented(pst_mac_vap) == OAL_FALSE)) ||
        ((pst_bss_dscr->en_ht_capable == OAL_FALSE) && (pst_bss_dscr->en_vht_capable == OAL_FALSE))) {
        pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
        return;
    }

    /* ʹ��40MHz */
    /* (1) �û�����"40MHz����"����(��STA�� dot11FortyMHzOperationImplementedΪtrue) */
    /* (2) AP��40MHz���� */
    if (mac_mib_get_FortyMHzOperationImplemented(pst_mac_vap) == OAL_TRUE) {
        switch (pst_bss_dscr->en_channel_bandwidth) {
            case WLAN_BAND_WIDTH_40PLUS:
            case WLAN_BAND_WIDTH_80PLUSPLUS:
            case WLAN_BAND_WIDTH_80PLUSMINUS:
                pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_40PLUS;
                break;

            case WLAN_BAND_WIDTH_40MINUS:
            case WLAN_BAND_WIDTH_80MINUSPLUS:
            case WLAN_BAND_WIDTH_80MINUSMINUS:
                pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_40MINUS;
                break;

            default:
                pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
                break;
        }
    }

    /* ����STA�������APһ�� */
    /* (1) STA AP��֧��11AC */
    /* (2) STA֧��40M����(FortyMHzOperationImplementedΪTRUE)��
           ���ƻ���ֹ2GHT40ʱ��2G��FortyMHzOperationImplemented=FALSE�������´��� */
    /* (3) STA֧��80M����(��STA�� dot11VHTChannelWidthOptionImplementedΪ0) */
    if ((mac_mib_get_VHTOptionImplemented(pst_mac_vap) == OAL_TRUE) &&
        (pst_bss_dscr->en_vht_capable == OAL_TRUE)) {
        if ((mac_mib_get_FortyMHzOperationImplemented(pst_mac_vap) == OAL_TRUE) &&
            (mac_mib_get_VHTChannelWidthOptionImplemented(pst_mac_vap) == WLAN_MIB_VHT_SUPP_WIDTH_80)) {
#if (_PRE_WLAN_CHIP_ASIC == _PRE_WLAN_CHIP_VERSION)
            pst_mac_vap->st_channel.en_bandwidth = pst_bss_dscr->en_channel_bandwidth;
#else
            oam_warning_log0(pst_mac_vap->uc_vap_id, OAM_SF_2040,
                             "{hmac_update_join_req_params_2040::fpga is not support 80M.}");
#endif
        }
    }

    /* ���AP��STAͬʱ֧��20/40��������ܣ���ʹ��STA��Ƶ�׹����� */
    if ((mac_mib_get_2040BSSCoexistenceManagementSupport(pst_mac_vap) == OAL_TRUE) &&
        (pst_bss_dscr->uc_coex_mgmt_supp == 1)) {
        mac_mib_set_SpectrumManagementImplemented(pst_mac_vap, OAL_TRUE);
    }
}

oal_module_symbol(hmac_config_enable_2040bss);
#endif
