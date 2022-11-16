

#include "oal_net.h"
#include "oal_cfg80211.h"
#include "wlan_spec.h"
#include "wal_linux_event.h"
#include "wal_linux_scan.h"
#include "wal_linux_cfg80211.h"
#include "wal_main.h"
#include "wal_linux_rx_rsp.h"
#include "hmac_vap.h"
#include "hmac_device.h"
#include "mac_device.h"
#include "hmac_resource.h"
#include "mac_mib.h"
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_pm_wlan.h"
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <net/cfg80211.h>
#endif
#include "securec.h"


#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_SCAN_C

#ifdef _PRE_WLAN_COUNTRYCODE_SELFSTUDY
uint8_t g_country_code_result[COUNTRY_CODE_LEN];
oal_bool_enum_uint8 g_country_code_result_update_flag = OAL_TRUE;
oal_bool_enum_uint8 g_country_code_self_study_flag = OAL_TRUE;
#endif


OAL_STATIC void wal_inform_bss_frame(wal_scanned_bss_info_stru *scanned_bss_info, oal_wiphy_stru *wiphy)
{
    oal_cfg80211_bss_stru *cfg80211_bss = NULL;
    oal_ieee80211_channel_stru *ieee80211_channel = NULL;

    if (oal_any_null_ptr2(scanned_bss_info, wiphy)) {
        oam_error_log0(0, OAM_SF_SCAN,
            "{wal_inform_bss_frame::input param pointer is null!}");
        return;
    }

    ieee80211_channel = oal_ieee80211_get_channel(wiphy, (int32_t)scanned_bss_info->s_freq);
    if (ieee80211_channel == NULL) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_inform_bss_frame::get channel failed, wrong s_freq[%d]}",
                         (int32_t)scanned_bss_info->s_freq);
        return;
    }

    /* wpa_supplicant�����nl80211_parse_bss_info��������RSSIʱ������100 */
    scanned_bss_info->l_signal = scanned_bss_info->l_signal * 100;

    /* ����ϱ��ں�bss ��Ϣ */
    cfg80211_bss = oal_cfg80211_inform_bss_frame(wiphy, ieee80211_channel, scanned_bss_info->pst_mgmt,
        scanned_bss_info->mgmt_len, scanned_bss_info->l_signal, GFP_ATOMIC);
    if (cfg80211_bss != NULL) {
        oal_cfg80211_put_bss(wiphy, cfg80211_bss);
    }

    return;
}


void wal_update_bss(oal_wiphy_stru *wiphy, hmac_bss_mgmt_stru *bss_mgmt, uint8_t *bssid)
{
    wal_scanned_bss_info_stru scanned_bss_info;
    oal_cfg80211_bss_stru *cfg80211_bss = NULL;
    hmac_scanned_bss_info *scanned_bss = NULL;
    mac_ieee80211_frame_stru *frame_hdr = NULL;
    oal_dlist_head_stru *entry = NULL;
    mac_bss_dscr_stru *bss_dscr = NULL;
    uint8_t chan_number;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0))
    enum nl80211_band en_band;
#else
    enum ieee80211_band en_band;
#endif
    int32_t channel;
    oal_bool_enum_uint8 en_inform_bss = OAL_FALSE;

    if (oal_any_null_ptr3(wiphy, bss_mgmt, bssid)) {
        oam_warning_log0(0, OAM_SF_ASSOC, "{wal_update_bss::null pointer.");
        return;
    }

    bss_dscr = NULL;
    /* ��ȡ�� */
    oal_spin_lock(&(bss_mgmt->st_lock));

    /* �ӱ���ɨ�����л�ȡbssid ��Ӧ��bss ��Ϣ */
    oal_dlist_search_for_each(entry, &(bss_mgmt->st_bss_list_head))
    {
        scanned_bss = oal_dlist_get_entry(entry, hmac_scanned_bss_info, st_dlist_head);
        bss_dscr = &(scanned_bss->st_bss_dscr_info);

        /* ��ɨ�����жԱȹ�����MAC ��ַ�������ͬ������ǰ�˳� */
        if (oal_memcmp(bssid, bss_dscr->auc_bssid, WLAN_MAC_ADDR_LEN) == 0) {
            break;
        }
        bss_dscr = NULL;
    }

    /* ����� */
    oal_spin_unlock(&(bss_mgmt->st_lock));

    if (bss_dscr == NULL) {
        oam_warning_log4(0, OAM_SF_ASSOC, "{wal_update_bss::do not find correspond bss from scan result."
                         "%02X:XX:XX:%02X:%02X:%02X}",
                         bssid[BYTE_OFFSET_0], bssid[BYTE_OFFSET_3],
                         bssid[BYTE_OFFSET_4], bssid[BYTE_OFFSET_5]);
        return;
    }

    /* �����Ƿ����ں��У����û�и�bss  �������bss ���������bss ʱ�� */
    cfg80211_bss = oal_cfg80211_get_bss(wiphy, NULL, bssid, (uint8_t *)(bss_dscr->ac_ssid),
                                        OAL_STRLEN(bss_dscr->ac_ssid));
    if (cfg80211_bss != NULL) {
        channel = (int32_t)oal_ieee80211_frequency_to_channel((int32_t)(cfg80211_bss->channel->center_freq));
        /*
         * ����ϵ�ssid���ŵ���һ�£���Ҫunlink֮�������ϱ��µ�FRW���ں˻�ȡ��old�ŵ���
         * ��ɺ����ٴι�����Ϊ�ŵ�������޷������ɹ�
         */
        if (bss_dscr->st_channel.uc_chan_number != (uint8_t)channel) {
            oam_warning_log2(0, OAM_SF_ASSOC,
                             "{wal_update_bss::current kernel bss channel[%d] need to update to channel[%d].",
                             (uint8_t)channel, bss_dscr->st_channel.uc_chan_number);
            oal_cfg80211_unlink_bss(wiphy, cfg80211_bss);
            en_inform_bss = OAL_TRUE;
        } else {
            oal_cfg80211_put_bss(wiphy, cfg80211_bss);
        }
    } else {
        en_inform_bss = OAL_TRUE;
    }

    /* ��Ҫinform bss���ں� */
    if (en_inform_bss) {
        chan_number = bss_dscr->st_channel.uc_chan_number;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0))
        en_band = (enum nl80211_band)bss_dscr->st_channel.en_band;
#else
        en_band = (enum ieee80211_band)bss_dscr->st_channel.en_band;
#endif
        /* ��ʼ�� */
        memset_s(&scanned_bss_info, sizeof(wal_scanned_bss_info_stru),
                 0, sizeof(wal_scanned_bss_info_stru));

        /* ��дBSS �ź�ǿ�� */
        scanned_bss_info.l_signal = bss_dscr->c_rssi;

        /* ��bss�����ŵ�������Ƶ�� */
        scanned_bss_info.s_freq = (int16_t)oal_ieee80211_channel_to_frequency(chan_number, en_band);

        /* �����ָ֡��ͳ��� */
        scanned_bss_info.pst_mgmt = (oal_ieee80211_mgmt_stru *)(bss_dscr->auc_mgmt_buff);
        scanned_bss_info.mgmt_len = bss_dscr->mgmt_len;

        /* ��ȡ�ϱ���ɨ�����Ĺ���֡��֡ͷ */
        frame_hdr = (mac_ieee80211_frame_stru *)bss_dscr->auc_mgmt_buff;

        /* ���ɨ��������յ���֡������beacon���ͣ�ͳһ�޸�Ϊprobe rsp�����ϱ���
           Ϊ�˽���ϱ��ں˵�ɨ����beacon֡�������е����⣬�����⣬��01���ֹ� */
        if (frame_hdr->st_frame_control.bit_sub_type == WLAN_BEACON) {
            /* �޸�beacon֡����Ϊprobe rsp */
            frame_hdr->st_frame_control.bit_sub_type = WLAN_PROBE_RSP;
        }

        wal_inform_bss_frame(&scanned_bss_info, wiphy);
    }

    return;
}
#ifdef _PRE_WLAN_COUNTRYCODE_SELFSTUDY

OAL_STATIC void wal_country_code_result(wal_country_code_stru *wal_country_code,
                                        hmac_vap_stru *hmac_vap)
{
    uint8_t     country_id;
    uint8_t     max_mun_country_id;
    uint8_t     max_count_num;
    uint8_t     second_max_count_num = 0;
    uint8_t     default_country_code[] = "99";

    max_count_num = wal_country_code->wal_country_code_count[0].count_num;
    max_mun_country_id = 0;
    /* ���������ս�� */
    for (country_id = 1; country_id < wal_country_code->country_type_num; country_id++) {
        if (wal_country_code->wal_country_code_count[country_id].count_num >= max_count_num) {
            second_max_count_num = max_count_num;
            max_mun_country_id = country_id;
            max_count_num = wal_country_code->wal_country_code_count[country_id].count_num;
        }
    }
    /* ��������ͳ�ƽ��Ϊ0�������ֵ������ȵ����������Ĭ�Ϲ�����99 */
    if (second_max_count_num == max_count_num || max_count_num == 0) {
        if (memcpy_s(wal_country_code->country_code_result, OAL_COUNTRY_CODE_LAN,
            default_country_code, OAL_COUNTRY_CODE_LAN) != EOK) {
            oam_warning_log0(0, OAM_SF_ANY, "wal_country_code_result: memcpy_s failed.\n");
        }
    } else {
        if (memcpy_s(wal_country_code->country_code_result, OAL_COUNTRY_CODE_LAN,
            wal_country_code->wal_country_code_count[max_mun_country_id].pc_country_code,
            OAL_COUNTRY_CODE_LAN) != EOK) {
            oam_warning_log0(0, OAM_SF_ANY, "wal_country_code_result: memcpy_s failed.\n");
        }
    }
    /* WIFI�ϵ�󣬵�һ�εڶ����ϱ�ɨ���������¹����룬����ÿ�ϱ�ɨ����3�Σ�ȷ��һ�ι����� */
    hmac_vap->uc_scan_upload_num++;
    if (hmac_vap->uc_scan_upload_num > WAL_COUNTRY_CODE_UPDATE_COUNT ||
        hmac_vap->uc_scan_upload_num == WAL_COUNTRY_CODE_UPDATE_FRIST ||
        hmac_vap->uc_scan_upload_num == WAL_COUNTRY_CODE_UPDATE_SECOND) {
        hmac_vap->uc_scan_upload_num = WAL_COUNTRY_CODE_UPDATE_NOMAL;
        /* �ж����¹������Ƿ����ı� */
        if (oal_compare_country_code(wal_country_code->country_code_result, g_country_code_result) != 0) {
            if (memcpy_s(g_country_code_result, OAL_COUNTRY_CODE_LAN,
                wal_country_code->country_code_result, OAL_COUNTRY_CODE_LAN) != EOK) {
                oam_warning_log0(0, OAM_SF_ANY, "wal_country_code_result: memcpy_s failed.\n");
            }
            g_country_code_result_update_flag = OAL_TRUE;
        } else {
            g_country_code_result_update_flag = OAL_FALSE;
        }
    }

    return;
}


OAL_STATIC oal_bool_enum_uint8 wal_country_code_is_alpha_upper(int8_t letter)
{
    if (letter >= 'A' && letter <= 'Z') {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}

OAL_STATIC oal_bool_enum_uint8 wal_country_code_illegal_confim(uint8_t *pc_country_code)
{
    if (wal_country_code_is_alpha_upper(pc_country_code[0]) && wal_country_code_is_alpha_upper(pc_country_code[1])) {
        return OAL_FALSE;
    }
    return OAL_TRUE;
}

OAL_STATIC oal_bool_enum_uint8 wal_bssid_confirm(mac_bss_dscr_stru *mac_bss_dscr_stru, uint8_t *bssid_name,
                                                 uint8_t bssid_num)
{
    uint8_t   bssid_name_id;
    oal_bool_enum_uint8   bssid_name_exist_flag = OAL_FALSE;

    if (bssid_num >= WAL_BSSID_MAX_NUM) {
        return bssid_name_exist_flag;
    }

    for (bssid_name_id = 0; bssid_name_id < bssid_num; bssid_name_id++) {
        /* ��ͬ��bssid��ַ */
        if (oal_compare_mac_addr(mac_bss_dscr_stru->auc_bssid,
            (bssid_name + WLAN_MAC_ADDR_LEN * bssid_name_id)) == 0) {
            bssid_name_exist_flag = OAL_TRUE;
        }
    }

    if (bssid_name_exist_flag == OAL_FALSE) {
        if (memcpy_s((bssid_name + bssid_num * WLAN_MAC_ADDR_LEN), OAL_MAC_ADDRESS_LAN,
            mac_bss_dscr_stru->auc_bssid, OAL_MAC_ADDRESS_LAN) != EOK) {
            oam_warning_log0(0, OAM_SF_ANY, "wal_country_code_count: memcpy_s failed.\n");
        }
    }
    return !bssid_name_exist_flag;
}


OAL_STATIC void wal_country_code_count(mac_bss_dscr_stru *mac_bss_dscr_stru,
                                       wal_country_code_stru *wal_country_code)
{
    uint8_t     *new_bssid_country_code = NULL;
    uint8_t     country_id;
    uint32_t    country_type_num;
    oal_bool_enum_uint8     country_code_exist_flag = OAL_FALSE;

    new_bssid_country_code = mac_bss_dscr_stru->ac_country;
    country_type_num = wal_country_code->country_type_num;

    if (country_type_num >= WAL_MAX_COUNTRY_CODE_NUM) {
        return;
    }
    /* ������Ϸ���У�� */
    if (wal_country_code_illegal_confim(new_bssid_country_code)) {
        return;
    }
    /* ������ͳ�� */
    for (country_id = 0; country_id < wal_country_code->country_type_num; country_id++) {
        if (oal_compare_country_code(wal_country_code->wal_country_code_count[country_id].pc_country_code,
            new_bssid_country_code) == 0) {
            wal_country_code->wal_country_code_count[country_id].count_num++;
            country_code_exist_flag = OAL_TRUE;
        }
    }
    if (country_code_exist_flag == OAL_FALSE) {
        if (memcpy_s(wal_country_code->wal_country_code_count[country_type_num].pc_country_code,
            OAL_COUNTRY_CODE_LAN, new_bssid_country_code, OAL_COUNTRY_CODE_LAN) != EOK) {
            oam_warning_log0(0, OAM_SF_ANY, "wal_country_code_count: memcpy_s failed.\n");
            }
        wal_country_code->wal_country_code_count[country_type_num].count_num++;
        wal_country_code->country_type_num++;
    }
}

OAL_STATIC void wal_countrycode_selstudy_statis(mac_bss_dscr_stru *bss_dscr,
                                                uint8_t *bssid_name,
                                                wal_country_code_stru *wal_country_code,
                                                uint8_t bssid_num)
{
    if (g_cust_country_code_ignore_flag.en_country_code_study_ingore_hipriv_flag == OAL_FALSE) {
        if (g_country_code_self_study_flag == OAL_TRUE) {
                /* ������ͳ�� */
                if (wal_bssid_confirm(bss_dscr, bssid_name, bssid_num)) {
                    bssid_num++;
                    wal_country_code_count(bss_dscr, wal_country_code);
                }
        }
    }
}

OAL_STATIC void wal_countrycode_selstudy_result(wal_country_code_stru* wal_country_code,
                                                hmac_vap_stru *hmac_vap)
{
    if (g_cust_country_code_ignore_flag.en_country_code_study_ingore_hipriv_flag == OAL_FALSE) {
    /* ��������ȷ�� */
        if (g_country_code_self_study_flag == OAL_TRUE) {
            wal_country_code_result(wal_country_code, hmac_vap);
        }
    }
}

OAL_STATIC void wal_scan_comp_regdomain_update(hmac_vap_stru *hmac_vap)
{
    int32_t ret = OAL_TRUE;
    uint8_t country_code_change = OAL_TRUE;

    if (g_country_code_self_study_flag == OAL_TRUE) {
        /* PC��������²��ԣ�WIFI�ϵ�ǰ����ɨ����¹����� */
        if (hmac_vap->uc_scan_upload_num == WAL_COUNTRY_CODE_UPDATE_FRIST ||
            hmac_vap->uc_scan_upload_num == WAL_COUNTRY_CODE_UPDATE_SECOND) {
            ret = wal_regdomain_update_selfstudy_country_code(hmac_vap->pst_net_device,
                                                              g_country_code_result);
            chr_exception_p(CHR_WIFI_COUNTRYCODE_UPDATE_EVENTID, &country_code_change, sizeof(uint8_t));
        }
        /* ����ÿ3��ɨ����й�����ȷ�� */
        if (g_country_code_result_update_flag == OAL_TRUE) {
            ret = wal_regdomain_update_selfstudy_country_code(hmac_vap->pst_net_device,
                                                              g_country_code_result);
            chr_exception_p(CHR_WIFI_COUNTRYCODE_UPDATE_EVENTID, &country_code_change, sizeof(uint8_t));
        }
        if (oal_unlikely(ret == -OAL_EFAIL)) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_scan_comp_regdomain_update::regdomain update failed!}");
        }
    }
}

void wal_counrtycode_selfstudy_scan_comp(hmac_vap_stru *hmac_vap)
{
    /* ��������� */
    if (g_cust_country_code_ignore_flag.en_country_code_study_ingore_hipriv_flag == OAL_FALSE) {
        wal_scan_comp_regdomain_update(hmac_vap);
    }
}
#endif

void wal_update_bss_for_csa(oal_wiphy_stru *wiphy, oal_net_device_stru *net_device)
{
    mac_vap_stru *mac_vap = NULL;
    oal_cfg80211_bss_stru *cfg80211_bss = NULL;
    int32_t channel;

    if (oal_any_null_ptr2(wiphy, net_device)) {
        oam_warning_log0(0, OAM_SF_CHAN, "{wal_update_bss_for_csa::null pointer.}");
        return;
    }

    mac_vap = (mac_vap_stru *)oal_net_dev_priv(net_device);
    if (oal_unlikely(mac_vap == NULL)) {
        oam_error_log0(0, OAM_SF_CHAN, "{wal_update_bss_for_csa::can't get mac vap from netdevice priv data!}");
        return ;
    }

    if (oal_any_null_ptr1(mac_vap->pst_mib_info)) {
        oam_warning_log0(0, OAM_SF_CHAN, "{wal_update_bss_for_csa::null pointer.}");
        return;
    }

    /* ɾ��kernel��ԭ���ŵ���BSS */
    cfg80211_bss = oal_cfg80211_get_bss(wiphy,
                                        NULL,
                                        mac_vap->auc_bssid,
                                        mac_mib_get_DesiredSSID(mac_vap),
                                        OAL_STRLEN(mac_mib_get_DesiredSSID(mac_vap)));
    if (cfg80211_bss != NULL) {
        channel = (int32_t)oal_ieee80211_frequency_to_channel((int32_t)(cfg80211_bss->channel->center_freq));

        oam_warning_log1(0, OAM_SF_CHAN,
                         "{wal_update_bss_for_csa::csa unlink kernel bss, channel[%d]}", (uint8_t)channel);

        oal_cfg80211_unlink_bss(wiphy, cfg80211_bss);
    }

    return;
}


OAL_STATIC void wal_inform_all_bss_init(wal_scanned_bss_info_stru *scanned_bss_info,
                                        mac_bss_dscr_stru *bss_dscr)
{
    mac_ieee80211_frame_stru *frame_hdr = NULL;
    uint8_t chan_number;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0))
    enum nl80211_band en_band;
#else
    enum ieee80211_band en_band;
#endif

    chan_number = bss_dscr->st_channel.uc_chan_number;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0))
    en_band = (enum nl80211_band)bss_dscr->st_channel.en_band;
#else
    en_band = (enum ieee80211_band)bss_dscr->st_channel.en_band;
#endif

    scanned_bss_info->l_signal = bss_dscr->c_rssi;

    /* ��bss�����ŵ�������Ƶ�� */
    scanned_bss_info->s_freq = (int16_t)oal_ieee80211_channel_to_frequency(chan_number, en_band);

    /* �����ָ֡��ͳ��� */
    scanned_bss_info->pst_mgmt = (oal_ieee80211_mgmt_stru *)(bss_dscr->auc_mgmt_buff);
    scanned_bss_info->mgmt_len = bss_dscr->mgmt_len;

    /* ��ȡ�ϱ���ɨ�����Ĺ���֡��֡ͷ */
    frame_hdr = (mac_ieee80211_frame_stru *)bss_dscr->auc_mgmt_buff;

    /* ���ɨ��������յ���֡������beacon���ͣ�ͳһ�޸�Ϊprobe rsp�����ϱ���
       Ϊ�˽���ϱ��ں˵�ɨ����beacon֡�������е����⣬�����⣬��01���ֹ� */
    if (frame_hdr->st_frame_control.bit_sub_type == WLAN_BEACON) {
        /* �޸�beacon֡����Ϊprobe rsp */
        frame_hdr->st_frame_control.bit_sub_type = WLAN_PROBE_RSP;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* ɨ��ʹ����ʵʱ����ϱ�,���´�û��ɨ�赽��AP,���ϱ�ʱ�����ӹ̶�ֵ */
    scanned_bss_info->pst_mgmt->u.probe_resp.timestamp = bss_dscr->wpa_rpt_time;
    bss_dscr->wpa_rpt_time += WLAN_BOOTTIME_REFLUSH;
#endif
}

OAL_STATIC uint32_t wal_channel_num_valid(mac_bss_dscr_stru *bss_dscr)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0))
    enum nl80211_band en_band;
#else
    enum ieee80211_band en_band;
#endif
    uint8_t chan_number;
    uint32_t ret;

    chan_number = bss_dscr->st_channel.uc_chan_number;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0))
    en_band = (enum nl80211_band)bss_dscr->st_channel.en_band;
#else
    en_band = (enum ieee80211_band)bss_dscr->st_channel.en_band;
#endif

    /* �ж��ŵ��ǲ����ڹ������ڣ�������ڣ����ϱ��ں� */
    ret = mac_is_channel_num_valid(en_band, chan_number, bss_dscr->st_channel.ext6g_band);
    if (ret != OAL_SUCC) {
        oam_warning_log2(0, OAM_SF_SCAN,
                         "{wal_inform_all_bss::curr channel[%d] and band[%d] is not in regdomain.}",
                         chan_number, en_band);
    }
    return ret;
}

void wal_inform_all_bss(oal_wiphy_stru *wiphy, hmac_bss_mgmt_stru *bss_mgmt, uint8_t vap_id)
{
    uint8_t *puc_p2p_ie = NULL;
    uint16_t us_offset = MAC_80211_FRAME_LEN + MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    mac_bss_dscr_stru *bss_dscr = NULL;
    hmac_scanned_bss_info *scanned_bss = NULL;
    oal_dlist_head_stru *entry = NULL;
    wal_scanned_bss_info_stru st_scanned_bss_info;
    uint32_t ret;
    uint32_t bss_num_not_in_regdomain = 0;
    uint32_t bss_num = 0;

#if defined(_PRE_WLAN_FEATURE_11K) || defined(_PRE_WLAN_FEATURE_11R)
    hmac_vap_stru *hmac_vap = NULL;
    uint8_t uc_voe_11r_auth;
#ifdef _PRE_WLAN_COUNTRYCODE_SELFSTUDY
    wal_country_code_stru         wal_country_code;
    uint8_t                       bssid_name[WAL_BSSID_MAX_NUM][WLAN_MAC_ADDR_LEN];
    uint8_t                       bssid_num = 0;
    /* ��ʼ�� */
    memset_s(&wal_country_code, sizeof(wal_country_code_stru), 0, sizeof(wal_country_code_stru));
#endif
    /* ��ȡhmac vap */
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(vap_id);
    if (hmac_vap == NULL) {
        oam_warning_log0(vap_id, OAM_SF_SCAN, "{wal_inform_all_bss::hmac_vap is null, vap_id[%d]!}");
        return;
    }

    uc_voe_11r_auth = hmac_vap->bit_voe_11r_auth;
    if (uc_voe_11r_auth == 1) {
        oam_warning_log1(vap_id, OAM_SF_SCAN, "{wal_inform_all_bss::uc_11r_auth=[%d]!}", uc_voe_11r_auth);
    }
#endif
    /* ��ȡ�� */
    oal_spin_lock(&(bss_mgmt->st_lock));

    /* ����ɨ�赽��bss��Ϣ */
    oal_dlist_search_for_each(entry, &(bss_mgmt->st_bss_list_head)) {
        scanned_bss = oal_dlist_get_entry(entry, hmac_scanned_bss_info, st_dlist_head);
        if (scanned_bss == NULL) {
            oam_error_log0(vap_id, OAM_SF_SCAN, "{wal_inform_all_bss::pst_scanned_bss is null. }");
            continue;
        }
        bss_dscr = &(scanned_bss->st_bss_dscr_info);
#ifdef _PRE_WLAN_COUNTRYCODE_SELFSTUDY
        /* ��������ѧϰͳ�� */
        wal_countrycode_selstudy_statis(bss_dscr, &bssid_name[0][0], &wal_country_code, bssid_num);
#endif
        /* �ж��ŵ��ǲ����ڹ������ڣ�������ڣ����ϱ��ں� */
        ret = wal_channel_num_valid(bss_dscr);
        if (ret != OAL_SUCC) {
            bss_num_not_in_regdomain++;
            continue;
        }
#if defined(_PRE_WLAN_FEATURE_11K) || defined(_PRE_WLAN_FEATURE_11R)
        if (uc_voe_11r_auth == OAL_FALSE) {
#endif
            /* voe 11r ��֤ʱ������ʱ������ */
            /* �ϱ�WAL_SCAN_REPORT_LIMIT���ڵ�ɨ���� */
            if (oal_time_after32(oal_time_get_stamp_ms(), (bss_dscr->timestamp + WAL_SCAN_REPORT_LIMIT))) {
                continue;
            }
#if defined(_PRE_WLAN_FEATURE_11K) || defined(_PRE_WLAN_FEATURE_11R)
        }
#endif
        puc_p2p_ie = mac_find_vendor_ie(MAC_WLAN_OUI_WFA, MAC_WLAN_OUI_TYPE_WFA_WFD,
                                        (bss_dscr->auc_mgmt_buff + us_offset),
                                        (int32_t)(bss_dscr->mgmt_len - us_offset));
        if ((puc_p2p_ie != NULL) && (puc_p2p_ie[1] > MAC_P2P_MIN_IE_LEN)
            && (bss_dscr->en_new_scan_bss == OAL_FALSE)) {
            oam_warning_log2(vap_id, OAM_SF_SCAN,
                "wal_inform_all_bss::[%02X:%02X] include WFD ie, is an old scan result in 5s",
                bss_dscr->auc_bssid[MAC_ADDR_4], bss_dscr->auc_bssid[MAC_ADDR_5]);
            continue ;
        }

        /* ��ʼ�� */
        memset_s(&st_scanned_bss_info, sizeof(wal_scanned_bss_info_stru), 0, sizeof(wal_scanned_bss_info_stru));

        wal_inform_all_bss_init(&st_scanned_bss_info, bss_dscr);

        /* �ϱ�ɨ�������ں� */
        wal_inform_bss_frame(&st_scanned_bss_info, wiphy);
        bss_num++;
    }

    /* ����� */
    oal_spin_unlock(&(bss_mgmt->st_lock));

    oam_warning_log3(vap_id, OAM_SF_SCAN,
        "{wal_inform_all_bss::there are %d bss not in regdomain, so inform kernel bss num is [%d] in [%d]!}",
        bss_num_not_in_regdomain, bss_num, (bss_mgmt->bss_num - bss_num_not_in_regdomain));
#ifdef _PRE_WLAN_COUNTRYCODE_SELFSTUDY
    /* ��������ѧϰ���ȷ�� */
    wal_countrycode_selstudy_result(&wal_country_code, hmac_vap);
#endif
    return;
}


OAL_STATIC void free_scan_param_resource(mac_cfg80211_scan_param_stru *scan_param)
{
    if (scan_param->channels_2g != NULL) {
        oal_free(scan_param->channels_2g);
        scan_param->channels_2g = NULL;
    }
    if (scan_param->channels_5g != NULL) {
        oal_free(scan_param->channels_5g);
        scan_param->channels_5g = NULL;
    }

    if (scan_param->channels_6g != NULL) {
        oal_free(scan_param->channels_6g);
        scan_param->channels_6g = NULL;
    }
    if (scan_param->puc_ie != NULL) {
        oal_free(scan_param->puc_ie);
        scan_param->puc_ie = NULL;
    }
}


OAL_STATIC uint32_t wal_set_scan_channel(oal_cfg80211_scan_request_stru *request,
                                         mac_cfg80211_scan_param_stru *scan_param)
{
    uint32_t loop;
    uint32_t num_chan_2g = 0;
    uint32_t num_chan_5g = 0;
    uint32_t num_chan_6g = 0;

    if (request->n_channels == 0) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_get_scan_channel_num::channels in scan requst is zero.}");
        return OAL_FAIL;
    }

    scan_param->channels_2g = oal_memalloc(request->n_channels * sizeof(uint32_t));
    scan_param->channels_5g = oal_memalloc(request->n_channels * sizeof(uint32_t));
    scan_param->channels_6g = oal_memalloc(request->n_channels * sizeof(uint32_t));
    if (oal_any_null_ptr3(scan_param->channels_2g, scan_param->channels_5g, scan_param->channels_6g)) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_scan_work_func::channel alloc mem return null ptr!}");
        return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }

    for (loop = 0; loop < request->n_channels; loop++) {
        uint16_t center_freq;
        uint32_t chan;

        center_freq = request->channels[loop]->center_freq;

        /* ��������Ƶ�ʣ������ŵ��� */
        chan = (uint32_t)oal_ieee80211_frequency_to_channel((int32_t)center_freq);
        if (center_freq <= WAL_MAX_FREQ_2G) {
            scan_param->channels_2g[num_chan_2g++] = chan;
        } else if (center_freq <= WAL_MAX_FREQ_5G) {
            scan_param->channels_5g[num_chan_5g++] = chan;
        } else {
            scan_param->channels_6g[num_chan_6g++] = chan;
        }
    }

    scan_param->num_channels_2g = (uint8_t)num_chan_2g;
    scan_param->num_channels_5g = (uint8_t)num_chan_5g;
    scan_param->num_channels_6g = (uint8_t)num_chan_6g;

    if (num_chan_2g == 0) {
        oal_free(scan_param->channels_2g);
        scan_param->channels_2g = NULL;
    }
    if (num_chan_5g == 0) {
        oal_free(scan_param->channels_5g);
        scan_param->channels_5g = NULL;
    }
    if (num_chan_6g == 0) {
        oal_free(scan_param->channels_6g);
        scan_param->channels_6g = NULL;
    }
    return OAL_SUCC;
}


OAL_STATIC void wal_set_scan_ssid(oal_cfg80211_scan_request_stru *request,
                                  mac_cfg80211_scan_param_stru *pst_scan_param)
{
    int32_t l_loop;
    int32_t l_ssid_num;

    if (oal_any_null_ptr2(request, pst_scan_param)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_set_scan_ssid::scan failed, null ptr.}");
        return;
    }

    pst_scan_param->en_scan_type = OAL_ACTIVE_SCAN; /* active scan */
    pst_scan_param->l_ssid_num = 0;

    /* ȡ�ں��·���ssid�ĸ��� */
    l_ssid_num = request->n_ssids;
    if (l_ssid_num > WLAN_SCAN_REQ_MAX_SSID) {
        /* ����û��·���ָ��ssid�ĸ�����������֧�ֵ�����������ȡ����֧�ֵ�ָ��ssid�������� */
        l_ssid_num = WLAN_SCAN_REQ_MAX_SSID;
    }

    /* ���û��·���ssid��Ϣ��������Ӧ�Ľṹ���� */
    if ((l_ssid_num > 0) && (request->ssids != NULL)) {
        pst_scan_param->l_ssid_num = l_ssid_num;

        for (l_loop = 0; l_loop < l_ssid_num; l_loop++) {
            pst_scan_param->st_ssids[l_loop].uc_ssid_len = request->ssids[l_loop].ssid_len;

            if (pst_scan_param->st_ssids[l_loop].uc_ssid_len > OAL_IEEE80211_MAX_SSID_LEN) {
                oam_warning_log2(0, OAM_SF_ANY,
                                 "{wal_set_scan_ssid::ssid scan set failed, ssid_len[%d] is exceed, max[%d].}",
                                 pst_scan_param->st_ssids[l_loop].uc_ssid_len,
                                 OAL_IEEE80211_MAX_SSID_LEN);

                pst_scan_param->st_ssids[l_loop].uc_ssid_len = OAL_IEEE80211_MAX_SSID_LEN;
            } else if (pst_scan_param->st_ssids[l_loop].uc_ssid_len == 0) {
                continue;
            }

            if (EOK != memcpy_s(pst_scan_param->st_ssids[l_loop].auc_ssid,
                                sizeof(pst_scan_param->st_ssids[l_loop].auc_ssid),
                                request->ssids[l_loop].ssid,
                                pst_scan_param->st_ssids[l_loop].uc_ssid_len)) {
                oam_warning_log3(0, OAM_SF_ANY,
                    "wal_set_scan_ssid::memcpy do nothing. cur_idx[%d], cur_ssid_len[%d]. scan_req_ssid_num[%d]",
                    l_loop, pst_scan_param->st_ssids[l_loop].uc_ssid_len, request->n_ssids);
            }
        }
    }
}


OAL_STATIC uint32_t wal_wait_for_scan_timeout_fn(void *p_arg)
{
    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)p_arg;
    mac_vap_stru *mac_vap = &(hmac_vap->st_vap_base_info);
    hmac_device_stru *hmac_device = NULL;
    hmac_bss_mgmt_stru *bss_mgmt = NULL;
    hmac_scan_stru *scan_mgmt = NULL;
    oal_wiphy_stru *wiphy = NULL;

    oam_warning_log0(mac_vap->uc_vap_id, OAM_SF_SCAN,
                     "{wal_wait_for_scan_timeout_fn:: 5 seconds scan timeout proc.}");

    /* ���ݵ�ǰɨ������ͺ͵�ǰvap��״̬�������л�vap��״̬��ɨ���쳣�����У��ϱ��ں�ɨ��״̬Ϊɨ����� */
    if ((mac_vap->en_vap_mode == WLAN_VAP_MODE_BSS_STA) &&
        (mac_vap->en_vap_state == MAC_VAP_STATE_STA_WAIT_SCAN)) {
        /* �ı�vap״̬��SCAN_COMP */
        mac_vap_state_change(mac_vap, MAC_VAP_STATE_STA_SCAN_COMP);
    }

    /* ��ȡhmac device */
    hmac_device = hmac_res_get_mac_dev(mac_vap->uc_device_id);
    if (hmac_device == NULL) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_wait_for_scan_complete_time_out::pst_hmac_device[%d] is null.}",
                         mac_vap->uc_device_id);
        return OAL_FAIL;
    }

    scan_mgmt = &(hmac_device->st_scan_mgmt);
    wiphy = hmac_device->pst_device_base_info->pst_wiphy;

    /* ��ȡɨ�����Ĺ���ṹ��ַ */
    bss_mgmt = &(hmac_device->st_scan_mgmt.st_scan_record_mgmt.st_bss_mgmt);

    /* �����ں��·���ɨ��request��Դ���� */
    oal_spin_lock(&(scan_mgmt->st_scan_request_spinlock));

    if (scan_mgmt->pst_request != NULL) {
        /* �ϱ�ɨ�赽�����е�bss */
        wal_inform_all_bss(wiphy, bss_mgmt, mac_vap->uc_vap_id);

        /* ֪ͨ kernel scan �Ѿ����� */
        oal_cfg80211_scan_done(scan_mgmt->pst_request, 0);

        scan_mgmt->pst_request = NULL;
        scan_mgmt->en_complete = OAL_TRUE;

        /* �ñ������Ż�ʱ��֤OAL_WAIT_QUEUE_WAKE_UP�����ִ�� */
        oal_smp_mb();
        oal_wait_queue_wake_up_interrupt(&scan_mgmt->st_wait_queue);
    }

    /* ֪ͨ���ںˣ��ͷ���Դ����� */
    oal_spin_unlock(&(scan_mgmt->st_scan_request_spinlock));

    return OAL_SUCC;
}


OAL_STATIC void wal_start_timer_for_scan_timeout(uint8_t vap_id)
{
    hmac_vap_stru *hmac_vap = NULL;
    mac_device_stru *mac_device = NULL;
    uint32_t timeout;

    /* ��ʱʱ��Ϊ5�� */
    timeout = WAL_MAX_SCAN_TIME_PER_SCAN_REQ;

    /* ��ȡhmac vap */
    hmac_vap = mac_res_get_hmac_vap(vap_id);
    if (hmac_vap == NULL) {
        oam_error_log0(vap_id, OAM_SF_SCAN, "{wal_start_timer_for_scan_timeout::pst_hmac_vap is null!}");
        return;
    }

    /* ��ȡmac device */
    mac_device = mac_res_get_dev(hmac_vap->st_vap_base_info.uc_device_id);
    if (mac_device == NULL) {
        oam_error_log0(vap_id, OAM_SF_SCAN, "{wal_start_timer_for_scan_timeout::pst_mac_device is null!}");
        return;
    }

    /* ����ɨ�豣����ʱ������ָ��ʱ��û���ϱ�ɨ�����������ϱ�ɨ����� */
    frw_timer_create_timer_m(&(hmac_vap->st_scan_timeout),
                             wal_wait_for_scan_timeout_fn,
                             timeout,
                             hmac_vap,
                             OAL_FALSE,
                             OAM_MODULE_ID_WAL,
                             mac_device->core_id);

    return;
}


uint32_t wal_scan_work_func(hmac_scan_stru *scan_mgmt,
                            oal_net_device_stru *netdev,
                            oal_cfg80211_scan_request_stru *request)
{
    mac_cfg80211_scan_param_stru st_scan_param;
    uint32_t ret;
    mac_vap_stru *mac_vap = oal_net_dev_priv(netdev);
    hmac_vap_stru *hmac_vap = NULL;
    uint8_t vap_id;
    uint8_t *puc_ie = NULL;

    if (mac_vap == NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_scan_work_func::pst_mac_vap is null!}");
        return OAL_FAIL;
    }

    vap_id = mac_vap->uc_vap_id;

    memset_s(&st_scan_param, sizeof(mac_cfg80211_scan_param_stru), 0, sizeof(mac_cfg80211_scan_param_stru));

    /* �����ں��·���ɨ���ŵ��б� */
    ret = wal_set_scan_channel(request, &st_scan_param);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_CFG,
                       "{wal_scan_work_func::wal_set_scan_channel proc failed, err_code[%d]!}", ret);
        free_scan_param_resource(&st_scan_param);
        return OAL_FAIL;
    }

    /* �����ں��·���ssid */
    wal_set_scan_ssid(request, &st_scan_param);

    /* �����ں��·���ie */
    st_scan_param.ie_len = request->ie_len;
    if (st_scan_param.ie_len > 0) {
        puc_ie = (uint8_t *)oal_memalloc(request->ie_len);
        if (puc_ie == NULL) {
            oam_error_log1(0, OAM_SF_CFG, "{wal_scan_work_func::puc_ie(%d) alloc mem return null ptr!}",
                           request->ie_len);
            free_scan_param_resource(&st_scan_param);
            return OAL_ERR_CODE_ALLOC_MEM_FAIL;
        }
        if (EOK != memcpy_s(puc_ie, request->ie_len, request->ie, st_scan_param.ie_len)) {
            oam_error_log0(0, OAM_SF_CFG, "wal_scan_work_func::memcpy fail!");
            oal_free(puc_ie);
            free_scan_param_resource(&st_scan_param);
            return OAL_FAIL;
        }
        st_scan_param.puc_ie = puc_ie;
    }

    /* �û����·���scan flag ת������һ�׶ν����������ݲ�����dmac,�� P �汾�ϲ�������ɺ��ٵ���ʵ��Ӧ�� */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
    if (request->flags & WLAN_SCAN_FLAG_LOW_POWER) {
        st_scan_param.scan_flag |= WLAN_SCAN_FLAG_LOW_POWER; /* ʹ�ò���ɨ�� */
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_scan_work_func::kernel set fast scan!}\r\n");
    }
#endif

    /* P2P WLAN/P2P ��������£�����ɨ���ssid �ж��Ƿ�Ϊp2p device �����ɨ�裬
        ssid Ϊ"DIRECT-"����Ϊ��p2p device �����ɨ�� */
    /* �����·�ɨ���device �Ƿ�Ϊp2p device(p2p0) */
    st_scan_param.bit_is_p2p0_scan = OAL_FALSE;

    if (is_p2p_scan_req(request)) {
        st_scan_param.bit_is_p2p0_scan = OAL_TRUE;
    }

    /* ���¼�ǰ��ֹ�첽�������ɨ���,����ͬ������ */
    scan_mgmt->en_complete = OAL_FALSE;

    /* ������ʱ��������ɨ�賬ʱ���� */
    /* ����������post event�����п��ܴ������ȣ�����ɨ������¼����ڶ�ʱ�����������յ��³�ʱ��ʱ������ */
    wal_start_timer_for_scan_timeout(vap_id);

    /* ���¼���֪ͨ��������ɨ�� */
    ret = wal_cfg80211_start_scan(netdev, &st_scan_param);
    if (ret != OAL_SUCC) {
        hmac_vap = mac_res_get_hmac_vap(vap_id);
        if (hmac_vap != NULL && hmac_vap->st_scan_timeout.en_is_registerd) {
            frw_timer_immediate_destroy_timer_m(&hmac_vap->st_scan_timeout);
        }

        oam_error_log1(0, OAM_SF_CFG,
                       "{wal_scan_work_func::wal_cfg80211_start_scan proc failed, err_code[%d]!}", ret);
        /* ��������£���hmac�ͷ� */
        free_scan_param_resource(&st_scan_param);

        scan_mgmt->en_complete = OAL_TRUE;
        return OAL_FAIL;
    }

    return OAL_SUCC;
}


int32_t wal_send_scan_abort_msg(oal_net_device_stru *net_dev)
{
    wal_msg_write_stru st_write_msg;
    uint32_t pedding_data = 0; /* ������ݣ���ʹ�ã�ֻ��Ϊ�˸��ýӿ� */
    int32_t ret;
    wal_msg_stru *rsp_msg = NULL;

    /* ���¼�֪ͨdevice����ֹɨ�� */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SCAN_ABORT, sizeof(pedding_data));

    if (EOK != memcpy_s(st_write_msg.auc_value, sizeof(st_write_msg.auc_value),
                        (int8_t *)&pedding_data, sizeof(pedding_data))) {
        oam_error_log0(0, OAM_SF_SCAN, "wal_send_scan_abort_msg::memcpy fail!");
        return OAL_FAIL;
    }

    ret = wal_send_cfg_event(net_dev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(pedding_data),
                             (uint8_t *)&st_write_msg,
                             OAL_TRUE,
                             &rsp_msg);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_send_scan_abort_msg::fail to stop scan, error[%d]}", ret);
        return ret;
    }

    if (OAL_SUCC != wal_check_and_release_msg_resp(rsp_msg)) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_send_scan_abort_msg::wal_check_and_release_msg_resp fail.}");
    }

    return OAL_SUCC;
}


void wal_force_internal_scan_complete(oal_net_device_stru *net_dev, hmac_scan_stru *scan_mgmt, mac_vap_stru *mac_vap)
{
    /* �ж��Ƿ�����ڲ�ɨ�裬������ڣ���Ҫֹͣ */
    if ((scan_mgmt->en_is_scanning == OAL_TRUE) &&
        (mac_vap->uc_vap_id == scan_mgmt->st_scan_record_mgmt.uc_vap_id)) {
        oam_warning_log0(mac_vap->uc_vap_id, OAM_SF_SCAN,
                         "{wal_force_inner_scan_complete::may be internal scan, stop scan!}");
        /* ��ֹɨ�� */
        wal_send_scan_abort_msg(net_dev);
    }
}


int32_t wal_force_scan_complete(oal_net_device_stru *net_dev, oal_bool_enum en_is_aborted)
{
    mac_vap_stru *mac_vap = oal_net_dev_priv(net_dev);
    hmac_vap_stru *hmac_vap = NULL;
    hmac_device_stru *hmac_device = NULL;
    hmac_scan_stru *scan_mgmt = NULL;

    if (mac_vap == NULL) {
        oam_warning_log1(0, OAM_SF_SCAN,
            "{wal_force_scan_complete:: mac_vap of net_dev is deleted!iftype:[%d]}", net_dev->ieee80211_ptr->iftype);
        return OAL_SUCC;
    }

    /* ��ȡhmac device */
    hmac_device = hmac_res_get_mac_dev(mac_vap->uc_device_id);
    if (hmac_device == NULL) {
        return -OAL_EINVAL;
    }

    /* ��ȡhmac vap */
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_warning_log1(0, OAM_SF_SCAN,
            "{wal_force_scan_complete::hmac_vap is null vap_id[%d]!}", mac_vap->uc_vap_id);
        return -OAL_EINVAL;
    }

    scan_mgmt = &(hmac_device->st_scan_mgmt);

    /* ����������ڲ���ɨ�� */
    if (scan_mgmt->pst_request == NULL) {
        wal_force_internal_scan_complete(net_dev, scan_mgmt, mac_vap);
        return OAL_SUCC;
    }

    /* �����ں��·���ɨ��request��Դ���� */
    oal_spin_lock(&(scan_mgmt->st_scan_request_spinlock));

    /* ������ϲ��·���ɨ��������֪ͨ�ں�ɨ��������ڲ�ɨ�費��֪ͨ */
    if ((scan_mgmt->pst_request != NULL) && oal_wdev_match(net_dev, scan_mgmt->pst_request)) {
        /* ɾ���ȴ�ɨ�賬ʱ��ʱ�� */
        if (hmac_vap->st_scan_timeout.en_is_registerd == OAL_TRUE) {
            frw_timer_immediate_destroy_timer_m(&(hmac_vap->st_scan_timeout));
        }

        /* �ϱ��ں�ɨ���� */
        wal_inform_all_bss(hmac_device->pst_device_base_info->pst_wiphy,
                           &(scan_mgmt->st_scan_record_mgmt.st_bss_mgmt), mac_vap->uc_vap_id);

        /* ֪ͨ�ں�ɨ����ֹ */
        oal_cfg80211_scan_done(scan_mgmt->pst_request, en_is_aborted);

        scan_mgmt->pst_request = NULL;
        scan_mgmt->en_complete = OAL_TRUE;
        /* ֪ͨ���ںˣ��ͷ���Դ����� */
        oal_spin_unlock(&(scan_mgmt->st_scan_request_spinlock));
        /* �·�device��ֹɨ�� */
        wal_send_scan_abort_msg(net_dev);

        /* ���ɨ�����ϱ��Ļص������������ϱ� */
        if (hmac_device->st_scan_mgmt.st_scan_record_mgmt.uc_vap_id == mac_vap->uc_vap_id) {
            hmac_device->st_scan_mgmt.st_scan_record_mgmt.p_fn_cb = NULL;
        }
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_force_scan_complete::vap_id[%d] notify kernel scan abort!}",
                         mac_vap->uc_vap_id);
        /* �ñ������Ż�ʱ��֤OAL_WAIT_QUEUE_WAKE_UP�����ִ�� */
        oal_smp_mb();
        oal_wait_queue_wake_up_interrupt(&scan_mgmt->st_wait_queue);
    } else {
        /* ֪ͨ���ںˣ��ͷ���Դ����� */
        oal_spin_unlock(&(scan_mgmt->st_scan_request_spinlock));
    }

    return OAL_SUCC;
}


int32_t wal_force_scan_abort_then_scan_comp(oal_net_device_stru *net_dev)
{
    mac_vap_stru *mac_vap;
    hmac_vap_stru *hmac_vap = NULL;
    hmac_device_stru *hmac_device = NULL;
    hmac_scan_stru *scan_mgmt = NULL;

    mac_vap = oal_net_dev_priv(net_dev);
    if (mac_vap == NULL) {
        oam_warning_log1(0, OAM_SF_SCAN,
                         "{wal_force_scan_abort_then_scan_comp:: mac_vap of net_dev is deleted!iftype:[%d]}",
                         net_dev->ieee80211_ptr->iftype);
        return OAL_SUCC;
    }

    /* ��ȡhmac device */
    hmac_device = hmac_res_get_mac_dev(mac_vap->uc_device_id);
    if (hmac_device == NULL) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_SCAN,
                         "{wal_force_scan_abort_then_scan_comp::pst_hmac_device[%d] is null!}",
                         mac_vap->uc_device_id);
        return -OAL_EINVAL;
    }

    /* ��ȡhmac vap */
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_SCAN,
                         "{wal_force_scan_abort_then_scan_comp::hmac_vap is null, vap_id[%d]!}",
                         mac_vap->uc_vap_id);
        return -OAL_EINVAL;
    }

    scan_mgmt = &(hmac_device->st_scan_mgmt);

    /* ����������ڲ���ɨ�� */
    if (scan_mgmt->pst_request == NULL) {
        /* �ж��Ƿ�����ڲ�ɨ�裬������ڣ�Ҳ��Ҫֹͣ */
        if ((scan_mgmt->en_is_scanning == OAL_TRUE) &&
            (mac_vap->uc_vap_id == scan_mgmt->st_scan_record_mgmt.uc_vap_id)) {
            oam_warning_log0(mac_vap->uc_vap_id, OAM_SF_SCAN,
                             "{wal_force_scan_abort_then_scan_comp::may be internal scan, stop scan!}");
            /* ��ֹɨ�� */
            wal_send_scan_abort_msg(net_dev);
        }

        return OAL_SUCC;
    }

    
    if ((scan_mgmt->pst_request != NULL) && oal_wdev_match(net_dev, scan_mgmt->pst_request)) {
        /* ɾ���ȴ�ɨ�賬ʱ��ʱ�� */
        if (hmac_vap->st_scan_timeout.en_is_registerd == OAL_TRUE) {
            frw_timer_immediate_destroy_timer_m(&(hmac_vap->st_scan_timeout));
        }

        /* �·�device��ֹɨ�� */
        wal_send_scan_abort_msg(net_dev);

        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_SCAN,
                         "{wal_force_scan_abort_then_scan_comp::vap_id[%d] notify kernel scan abort!}",
                         mac_vap->uc_vap_id);
    }

    return OAL_SUCC;
}


int32_t wal_stop_sched_scan(oal_net_device_stru *netdev)
{
    hmac_device_stru *hmac_device = NULL;
    hmac_scan_stru *scan_mgmt = NULL;
    mac_vap_stru *mac_vap = NULL;
    wal_msg_write_stru write_msg;
    uint32_t pedding_data = 0; /* ������ݣ���ʹ�ã�ֻ��Ϊ�˸��ýӿ� */
    int32_t ret = 0;

    /* �����Ϸ��Լ�� */
    if (netdev == NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_stop_sched_scan::pst_netdev is null}");
        return -OAL_EINVAL;
    }

    /* ͨ��net_device �ҵ���Ӧ��mac_device_stru �ṹ */
    mac_vap = oal_net_dev_priv(netdev);
    if (mac_vap == NULL) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_stop_sched_scan:: pst_mac_vap is null!}");
        return -OAL_EINVAL;
    }

    hmac_device = (hmac_device_stru *)hmac_res_get_mac_dev(mac_vap->uc_device_id);
    if (hmac_device == NULL) {
        oam_error_log1(0, OAM_SF_SCAN, "{wal_stop_sched_scan:: pst_mac_device[%d] is null!}", mac_vap->uc_device_id);
        return -OAL_EINVAL;
    }

    scan_mgmt = &(hmac_device->st_scan_mgmt);

    oam_warning_log1(0, OAM_SF_SCAN,
                     "{wal_stop_sched_scan::sched scan complete[%d]}", scan_mgmt->en_sched_scan_complete);

    if ((scan_mgmt->pst_sched_scan_req != NULL) && (scan_mgmt->en_sched_scan_complete != OAL_TRUE)) {
        /* �������ɨ������δִ�У����ϱ�����ɨ���� */
        oal_cfg80211_sched_scan_result(hmac_device->pst_device_base_info->pst_wiphy);

        scan_mgmt->pst_sched_scan_req = NULL;
        scan_mgmt->en_sched_scan_complete = OAL_TRUE;

        /* ���¼�֪ͨdevice��ֹͣPNO����ɨ�� */
        WAL_WRITE_MSG_HDR_INIT(&write_msg, WLAN_CFGID_CFG80211_STOP_SCHED_SCAN, sizeof(pedding_data));
        if (EOK != memcpy_s(write_msg.auc_value, sizeof(write_msg.auc_value),
                            (int8_t *)&pedding_data, sizeof(pedding_data))) {
            oam_error_log0(0, OAM_SF_SCAN, "wal_stop_sched_scan::memcpy fail!");
            return OAL_FAIL;
        }

        ret = wal_send_cfg_event(netdev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(pedding_data),
                                 (uint8_t *)&write_msg, OAL_FALSE, NULL);
        if (ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_SCAN,
                             "{wal_stop_sched_scan::fail to stop pno sched scan, error[%d]}", ret);
        }
    }

    return OAL_SUCC;
}

