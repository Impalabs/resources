

#include "oal_ext_if.h"
#include "oal_net.h"
#include "mac_frame.h"
#include "mac_frame_inl.h"
#include "mac_resource.h"
#include "mac_ie.h"
#include "mac_vap.h"
#include "mac_user.h"
#include "frw_ext_if.h"

#include "mac_resource.h"
#include "wlan_types.h"
#include "mac_mib.h"
#include "hmac_mgmt_bss_comm.h"
#include "hmac_11v.h"
#include "hmac_dfx.h"
#include "hmac_roam_main.h"
#include "hmac_roam_connect.h"
#include "hmac_roam_alg.h"
#include "hmac_roam_scan.h"
#include "hmac_scan.h"
#include "hmac_p2p.h"
#ifdef _PRE_WLAN_FEATURE_MBO
#include "hmac_mbo.h"
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_11V_C

#ifdef _PRE_WLAN_FEATURE_11V_ENABLE

void hmac_11v_roam_start_worker(oal_work_stru *hmac_11v_roam_work)
{
    hmac_roam_info_stru *roam_info = NULL;
    hmac_vap_stru *hmac_vap = NULL;
    hmac_user_stru *hmac_user = NULL;
    hmac_user_11v_ctrl_stru *user_11v_ctrl_info = NULL;

    roam_info = oal_container_of(hmac_11v_roam_work, hmac_roam_info_stru, hmac_11v_roam_work);
    if (roam_info == NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_11v_roam_start_worker::roam_info is null}");
        return;
    }

    hmac_vap = roam_info->pst_hmac_vap;
    if (hmac_vap == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_11v_roam_start_worker::hmac_vap is NULL}");
        return;
    }

    /* ��ȡ���Ͷ˵��û�ָ�� */
    hmac_user = mac_res_get_hmac_user(hmac_vap->st_vap_base_info.us_assoc_vap_id);
    if (hmac_user == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_11v_roam_start_worker::hmac_user is NULL}");
        return;
    }
    user_11v_ctrl_info = &(hmac_user->st_11v_ctrl_info);

    if (user_11v_ctrl_info->uc_11v_roam_scan_times < MAC_11V_ROAM_SCAN_ONE_CHANNEL_LIMIT) {
        /* ����ָ���ŵ�ɨ������ */
        user_11v_ctrl_info->uc_11v_roam_scan_times++;
        oam_warning_log3(0, OAM_SF_ANY,
            "{hmac_11v_roam_start_worker::Trig spec chan scan, 11v_roam_scan_times[%d], limit_times[%d]. chan[%d]}",
            user_11v_ctrl_info->uc_11v_roam_scan_times, MAC_11V_ROAM_SCAN_ONE_CHANNEL_LIMIT,
            roam_info->st_bsst_rsp_info.uc_chl_num);
        roam_info->st_static.uc_scan_mode = HMAC_CHR_11V_SCAN;
        hmac_roam_start(hmac_vap, ROAM_SCAN_CHANNEL_ORG_5, OAL_TRUE, NULL, ROAM_TRIGGER_11V);
    } else if (user_11v_ctrl_info->uc_11v_roam_scan_times == MAC_11V_ROAM_SCAN_ONE_CHANNEL_LIMIT) {
        /* ����ȫ�ŵ�ɨ������ */
        user_11v_ctrl_info->uc_11v_roam_scan_times++;
        roam_info->st_scan_h2d_params.st_scan_params.uc_bss_transition_process_flag = OAL_FALSE;
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_11v_roam_start_worker::Trigger ALL Channel scan roam.}");
        roam_info->st_static.uc_scan_mode = HMAC_CHR_NORMAL_SCAN;
        hmac_roam_start(hmac_vap, ROAM_SCAN_CHANNEL_ORG_DBDC, OAL_TRUE, NULL, ROAM_TRIGGER_11V);
    }

    return;
}


uint32_t hmac_11v_roam_scan_check(hmac_vap_stru *hmac_vap)
{
    hmac_user_stru *hmac_user = NULL;
    hmac_user_11v_ctrl_stru *user_11v_ctrl_info = NULL;
    hmac_roam_info_stru *roam_info = NULL;

    roam_info = (hmac_roam_info_stru *)hmac_vap->pul_roam_info;
    if (roam_info == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_11v_roam_scan_check::pst_roam_info is NULL}");
        return OAL_ERR_CODE_ROAM_INVALID_VAP;
    }

    /* ��ȡ���Ͷ˵��û�ָ�� */
    hmac_user = mac_res_get_hmac_user(hmac_vap->st_vap_base_info.us_assoc_vap_id);
    if (hmac_user == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_11v_roam_scan_check::pst_hmac_user is NULL}");
        return OAL_ERR_CODE_ROAM_INVALID_USER;
    }
    user_11v_ctrl_info = &(hmac_user->st_11v_ctrl_info);

    if (user_11v_ctrl_info->mac_11v_callback_fn == NULL
        || user_11v_ctrl_info->en_only_scan_one_time == OAL_TRUE) {
        return OAL_SUCC;
    }

    /* 11v roam scan�������øĳ�work queue */
    if (user_11v_ctrl_info->uc_11v_roam_scan_times < MAC_11V_ROAM_SCAN_FULL_CHANNEL_LIMIT) {
        oal_workqueue_schedule(&(roam_info->hmac_11v_roam_work));
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_MBO

OAL_STATIC void hmac_set_bss_re_assoc_delay_params(hmac_bsst_req_info_stru *bsst_req_info,
    mac_user_stru *mac_user, hmac_vap_stru *hmac_vap)
{
    uint8_t bss_index_loop;

    for (bss_index_loop = 0; bss_index_loop < bsst_req_info->uc_bss_list_num; bss_index_loop++) {
        if (!oal_memcmp(mac_user->auc_user_mac_addr,
            bsst_req_info->pst_neighbor_bss_list[bss_index_loop].auc_mac_addr, WLAN_MAC_ADDR_LEN)) {
            hmac_vap->st_vap_base_info.st_mbo_para_info.re_assoc_delay_time =
                HMAC_11V_MBO_RE_ASSOC_DALAY_TIME_S_TO_MS *
                bsst_req_info->pst_neighbor_bss_list[bss_index_loop].st_assoc_delay_attr_mbo_ie.re_assoc_delay_time;
            hmac_vap->st_vap_base_info.st_mbo_para_info.btm_req_received_time =
                (uint32_t)oal_time_get_stamp_ms();
            hmac_vap->st_vap_base_info.st_mbo_para_info.en_disable_connect = OAL_TRUE;

            memcpy_s(hmac_vap->st_vap_base_info.st_mbo_para_info.auc_re_assoc_delay_bss_mac_addr,
                     WLAN_MAC_ADDR_LEN, mac_user->auc_user_mac_addr, WLAN_MAC_ADDR_LEN);
            break;
        }
    }
    return;
}
#endif


OAL_STATIC uint32_t hmac_rx_bsst_req_candidate_info_check(hmac_vap_stru *hmac_vap,
    uint8_t *channel, uint8_t *bssid)
{
    mac_bss_dscr_stru *bss_dscr = NULL;
    uint8_t candidate_channel = *channel;
    wlan_channel_band_enum_uint8 channel_band = mac_get_band_by_channel_num(candidate_channel);

    uint32_t check = mac_is_channel_num_valid(channel_band, candidate_channel, OAL_FALSE);
    bss_dscr = (mac_bss_dscr_stru *)hmac_scan_get_scanned_bss_by_bssid(&hmac_vap->st_vap_base_info, bssid);
    if (check != OAL_SUCC && bss_dscr == NULL) {
        /* ��Ч�ŵ� */
        oam_warning_log3(0, OAM_SF_CFG,
            "{hmac_rx_bsst_req_candidate_info_check::chan[%d] invalid, bssid:XX:XX:XX:XX:%02X:%02X not in scan list}",
            candidate_channel, bssid[MAC_ADDR_4], bssid[MAC_ADDR_5]);
        return OAL_FAIL;
    }
    /* ��Ч */
    if (bss_dscr != NULL && candidate_channel != bss_dscr->st_channel.uc_chan_number) {
        /* ����Ϊʵ���ŵ� */
        *channel = bss_dscr->st_channel.uc_chan_number;
        oam_warning_log4(0, OAM_SF_CFG,
            "{hmac_rx_bsst_req_candidate_info_check::bssid:XX:XX:XX:XX:%02X:%02X in bssinfo channel[%d],not[%d]}",
            bssid[MAC_ADDR_4], bssid[MAC_ADDR_5],
            bss_dscr->st_channel.uc_chan_number, candidate_channel);
    }
    /* ˫wlanģʽ���Ƽ�bss��Ƶ�κ͵�ǰ����Ƶ�β�һ��ʱ�������ø�bss */
    channel_band = mac_get_band_by_channel_num(*channel);
    if (mac_is_dual_sta_mode() &&
        channel_band != hmac_vap->st_vap_base_info.st_channel.en_band) {
        oam_warning_log3(0, OAM_SF_CFG,
            "{hmac_rx_bsst_req_candidate_info_check::dual sta mode, ignore diff band[%d] bss[xx:xx:xx:xx:%02x:%02x]}",
            channel_band, bssid[MAC_ADDR_4], bssid[MAC_ADDR_5]); /* ��ӡbssid 4 5�����ֽ� */
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

OAL_STATIC void hmac_rx_bsst_free_res(hmac_bsst_req_info_stru *bsst_req_info)
{
    if (bsst_req_info->puc_session_url != NULL) {
        oal_mem_free_m(bsst_req_info->puc_session_url, OAL_TRUE);
        bsst_req_info->puc_session_url = NULL;
    }

    if (bsst_req_info->pst_neighbor_bss_list != NULL) {
        oal_mem_free_m(bsst_req_info->pst_neighbor_bss_list, OAL_TRUE);
        bsst_req_info->pst_neighbor_bss_list = NULL;
    }
}

OAL_STATIC void hmac_11v_roam_trigger(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user,
    hmac_bsst_req_info_stru *bsst_req_info)
{
    oal_bool_enum_uint8 need_roam = OAL_TRUE;
    uint32_t ret;
    hmac_roam_info_stru *roam_info = NULL;
    uint32_t beacon_period = mac_mib_get_BeaconPeriod(&hmac_vap->st_vap_base_info);
    uint32_t disassoc_time = bsst_req_info->us_disassoc_time * beacon_period;

    /* Signal Bridge disable 11v roaming */
    ret = hmac_roam_check_signal_bridge(hmac_vap);
    if (ret != OAL_SUCC) {
        need_roam = OAL_FALSE;
    }

    /* disassoc time > 0 && disassoc time < 100ms, BSST reject */
    if ((disassoc_time < HMAC_11V_REQUEST_DISASSOC_TIME_SCAN_ONE_CHANNEL_TIME)
        && (disassoc_time > 0)) {
        oam_warning_log1(0, OAM_SF_ANY,
            "{hmac_11v_roam_trigger::btm req disassoc_time=%d, will reject directly}",
            bsst_req_info->us_disassoc_time * mac_mib_get_BeaconPeriod(&(hmac_vap->st_vap_base_info)));
        need_roam = OAL_FALSE;
    }

    roam_info = (hmac_roam_info_stru *)hmac_vap->pul_roam_info;

    /* csi�ɼ���Ҫbssפ�����ܾ����� */
    if (roam_info->stay_on_current_bss == OAL_TRUE) {
        need_roam = OAL_FALSE;
    }

    if (need_roam == OAL_TRUE) {
#ifdef _PRE_WLAN_FEATURE_MBO
        hmac_set_bss_re_assoc_delay_params(bsst_req_info, &hmac_user->st_user_base_info, hmac_vap);
#endif
        hmac_roam_start(hmac_vap, ROAM_SCAN_CHANNEL_ORG_5, OAL_TRUE, NULL, ROAM_TRIGGER_11V);
    } else {
        roam_info->st_bsst_rsp_info.uc_status_code = WNM_BSS_TM_REJECT_NO_SUITABLE_CANDIDATES;
        hmac_tx_bsst_rsp_action(hmac_vap, hmac_user, &(roam_info->st_bsst_rsp_info));
    }
}


OAL_STATIC void hmac_bsst_fill_chan_and_bssid_info(hmac_roam_info_stru *roam_info,
    mac_scan_req_stru *scan_params, hmac_neighbor_bss_info_stru *neighbor_bss_info, uint8_t bss_num)
{
    uint8_t bss_list_index;
    uint8_t channum;
    uint8_t channel_idx;
    wlan_channel_band_enum_uint8 band;
    hmac_neighbor_bss_info_stru *neighbor_bss = NULL;

    for (bss_list_index = 0; bss_list_index < bss_num; bss_list_index++) {
        neighbor_bss = &(neighbor_bss_info[bss_list_index]);
        if (neighbor_bss->valid == OAL_FALSE) {
            continue;
        }
        if (neighbor_bss->uc_max_candidate_perf != neighbor_bss->uc_candidate_perf) {
            oam_warning_log1(0, OAM_SF_ROAM,
                             "{hmac_bsst_fill_chan_and_bssid_info::candidate_pref=%d}",
                             neighbor_bss->uc_candidate_perf);
            continue;
        }

        channum = neighbor_bss->uc_chl_num;
        band = mac_get_band_by_channel_num(channum);
        if (mac_get_channel_idx_from_num(band, channum, OAL_FALSE, &channel_idx) == OAL_SUCC) {
            hmac_roam_neighbor_report_add_chan(scan_params, channum, band, channel_idx);
        }

        hmac_roam_neighbor_report_add_bssid(roam_info, neighbor_bss->auc_mac_addr);
    }
}


OAL_STATIC void hmac_bsst_neighbor_fill_chan_bssid(hmac_roam_info_stru *roam_info,
    mac_scan_req_stru *scan_params, hmac_neighbor_bss_info_stru *neighbor_bss_info, uint8_t bss_num)
{
    if (oal_unlikely(oal_any_null_ptr3(roam_info, scan_params, neighbor_bss_info))) {
        oam_warning_log0(0, OAM_SF_ROAM,
            "{hmac_bsst_neighbor_fill_chan_bssid:: hmac_vap, scan_params or neighbor_bss_info is null}");
        return;
    }

    /* ��channel list��BSSID list���ͳ����Ϣ���� */
    scan_params->uc_channel_nums = 0;
    memset_s(&scan_params->ast_channel_list[0], WLAN_MAX_CHANNEL_NUM * sizeof(mac_channel_stru),
        0, WLAN_MAX_CHANNEL_NUM * sizeof(mac_channel_stru));

    roam_info->uc_neighbor_rpt_bssid_num = 0;
    memset_s(&roam_info->auc_neighbor_rpt_bssid[0][0], ROAM_NEIGHBOR_RPT_LIST_BSSID_MAX_NUM * WLAN_MAC_ADDR_LEN,
        0, ROAM_NEIGHBOR_RPT_LIST_BSSID_MAX_NUM * WLAN_MAC_ADDR_LEN);

    hmac_bsst_fill_chan_and_bssid_info(roam_info, scan_params, neighbor_bss_info, bss_num);
    scan_params->uc_bss_transition_process_flag = OAL_TRUE;

    oam_warning_log2(0, OAM_SF_ROAM,
                     "{hmac_bsst_neighbor_fill_chan_bssid::channel_nums=%d neighbor_rpt_bssid_num=%d}",
                     scan_params->uc_channel_nums, roam_info->uc_neighbor_rpt_bssid_num);
    /* δ�������ŵ�, BSST���̵ı�־λ���� */
    if (scan_params->uc_channel_nums == 0) {
        scan_params->uc_bss_transition_process_flag = OAL_FALSE;
    }
}

OAL_STATIC oal_bool_enum_uint8 hmac_bsst_check_bssid_channel(hmac_vap_stru *hmac_vap,
    hmac_neighbor_bss_info_stru *neighbor_bss, mac_user_stru *mac_user, mac_vap_stru *p2p_vap)
{
    uint32_t ret;

    /* current connect or invalid AP */
    if (ether_is_broadcast(neighbor_bss->auc_mac_addr) || ether_is_all_zero(neighbor_bss->auc_mac_addr) ||
        !oal_memcmp(mac_user->auc_user_mac_addr, neighbor_bss->auc_mac_addr, WLAN_MAC_ADDR_LEN)) {
        return OAL_FALSE;
    }

    /* ���channel num �Ƿ���Ч */
    ret = hmac_rx_bsst_req_candidate_info_check(hmac_vap, &(neighbor_bss->uc_chl_num),
                                                neighbor_bss->auc_mac_addr);
    if (ret != OAL_SUCC) {
        return OAL_FALSE;
    }

    /* check dbac channel compared with p2p up channel */
    if (hmac_roam_check_dbac_channel(p2p_vap, neighbor_bss->uc_chl_num) == OAL_TRUE) {
        oam_warning_log3(0, OAM_SF_ROAM,
            "{hmac_roam_check_dbac_channel::dbac channel ignored, %0x:%0x chanNum=%d}",
            neighbor_bss->auc_mac_addr[4], neighbor_bss->auc_mac_addr[5], /* mac addr 4 & 5 */
            neighbor_bss->uc_chl_num);
        return OAL_FALSE;
    }

    return OAL_TRUE;
}


OAL_STATIC void hmac_bsst_set_max_pref_to_neighbor_bss(hmac_vap_stru *hmac_vap,
    hmac_bsst_req_info_stru *bsst_req_info, mac_user_stru *mac_user)
{
    uint8_t idx_loop;
    uint8_t max_bss_pref = 0;
    hmac_neighbor_bss_info_stru *neighbor_bss = NULL;
    mac_vap_stru *other_vap = mac_vap_find_another_up_vap_by_mac_vap(&(hmac_vap->st_vap_base_info));

    /* find the max candidate preference but ignore current connect or invalid AP */
    for (idx_loop = 0; idx_loop < bsst_req_info->uc_bss_list_num; idx_loop++) {
        neighbor_bss = &(bsst_req_info->pst_neighbor_bss_list[idx_loop]);
        if (hmac_bsst_check_bssid_channel(hmac_vap, neighbor_bss, mac_user, other_vap) == OAL_FALSE) {
            continue;
        }

        neighbor_bss->valid = OAL_TRUE;
        if (neighbor_bss->uc_candidate_perf > max_bss_pref) {
            max_bss_pref = neighbor_bss->uc_candidate_perf;
        }
    }

    /* Neighbor Report don't include optional subelement: Candidate Preference */
    if (max_bss_pref == 0) {
        return;
    }

    /* set the max candidate pref to neighbor bss */
    for (idx_loop = 0; idx_loop < bsst_req_info->uc_bss_list_num; idx_loop++) {
        neighbor_bss = &(bsst_req_info->pst_neighbor_bss_list[idx_loop]);
        if (neighbor_bss->valid == OAL_FALSE) {
            continue;
        }
        neighbor_bss->uc_max_candidate_perf = max_bss_pref;
        oam_warning_log2(0, OAM_SF_ROAM,
                         "{hmac_bsst_set_max_pref_to_neighbor_bss:: candidate_pref=%d max_bss_pref=%d}",
                         neighbor_bss->uc_candidate_perf, neighbor_bss->uc_max_candidate_perf);
    }
}

OAL_STATIC void hmac_btm_parse_req_mode(hmac_bsst_req_info_stru *bsst_req_info, uint8_t req_mode)
{
    bsst_req_info->st_request_mode.bit_candidate_list_include = req_mode & BIT0;
    bsst_req_info->st_request_mode.bit_abridged = (req_mode & BIT1) ? OAL_TRUE : OAL_FALSE;
    bsst_req_info->st_request_mode.bit_bss_disassoc_imminent = (req_mode & BIT2) ? OAL_TRUE : OAL_FALSE;
    bsst_req_info->st_request_mode.bit_termination_include = (req_mode & BIT3) ? OAL_TRUE : OAL_FALSE;
    bsst_req_info->st_request_mode.bit_ess_disassoc_imminent = (req_mode & BIT4) ? OAL_TRUE : OAL_FALSE;
}

OAL_STATIC void hmac_btm_parse_term_duration(hmac_bsst_req_info_stru *bsst_req_info, uint8_t *data,
    uint16_t *handle_len, uint16_t frame_len)
{
    if ((bsst_req_info->st_request_mode.bit_termination_include) &&
        (frame_len >= (*handle_len) + MAC_IE_HDR_LEN +
        HMAC_11V_TERMINATION_TSF_LENGTH + 2)) { /* 2��duration_min�Ĵ�С */
        (*handle_len) += MAC_IE_HDR_LEN; /* ȥ��Ԫ��ͷ */
        if (memcpy_s(bsst_req_info->st_term_duration.auc_termination_tsf, HMAC_11V_TERMINATION_TSF_LENGTH,
            data + (*handle_len), HMAC_11V_TERMINATION_TSF_LENGTH) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "hmac_btm_parse_term_duration::memcpy fail!");
        }
        (*handle_len) += HMAC_11V_TERMINATION_TSF_LENGTH;
        bsst_req_info->st_term_duration.us_duration_min =
            (((uint16_t)data[(*handle_len) + 1]) << BIT_OFFSET_8) | (data[(*handle_len)]);
        (*handle_len) += 2; /* 2��duration_min�Ĵ�С */
    }
}

OAL_STATIC void hmac_btm_parse_session_url(hmac_bsst_req_info_stru *bsst_req_info,
    uint8_t *data, uint16_t *handle_len, uint16_t frame_len)
{
    uint8_t url_len;

    /* ����URL */
    /* URL�ֶ� ����еĻ� URL��һ���ֽ�ΪURL���� ���붯̬�ڴ汣�� */
    bsst_req_info->puc_session_url = NULL;
    if (bsst_req_info->st_request_mode.bit_ess_disassoc_imminent == OAL_FALSE) {
        return;
    }

    /*************************************************************************
     *                           URL field Format                            *
     * --------------------------------------------------------------------  *
     * |URL Length|                      URL                                 *
     * --------------------------------------------------------------------  *
     * |    1     |                    variable                              *
     * --------------------------------------------------------------------  */
    if (*handle_len > frame_len - 1) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_btm_parse_session_url::handle_len[%d] > frame_len[%d]}",
                       *handle_len, frame_len);
        return;
    }

    url_len = data[*handle_len];
    if ((*handle_len + 1 + url_len) > frame_len) {
        oam_error_log3(0, OAM_SF_ANY, "{hmac_btm_parse_session_url::handle_len[%d]+[%d]+1 > frame_len[%d]}",
                       *handle_len, url_len, frame_len);
        return;
    }

    if (url_len == 0) {
        (*handle_len) += (url_len + 1);
        oam_warning_log1(0, OAM_SF_ANY, "{hmac_btm_parse_session_url::url_len[%d]}", url_len);
        return;
    }

    bsst_req_info->url_len = url_len;
    /* �����ڴ�������1 ���ڴ���ַ��������� */
    bsst_req_info->puc_session_url =
        (uint8_t *)oal_mem_alloc_m(OAL_MEM_POOL_ID_LOCAL, url_len + 1, OAL_TRUE);
    if (bsst_req_info->puc_session_url == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_btm_parse_session_url:: puc_session_url alloc fail.}");
        return;
    }

    if (memcpy_s(bsst_req_info->puc_session_url, url_len + 1,
                 data + (*handle_len + 1), url_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_btm_parse_session_url::memcpy_s fail!");
    }
    /* ת�����ַ��� */
    bsst_req_info->puc_session_url[url_len] = '\0';
    (*handle_len) += (url_len + 1);
}

OAL_STATIC void hmac_btm_parse_bss_list(hmac_vap_stru *hmac_vap, hmac_bsst_req_info_stru *bsst_req_info,
    uint8_t *data, uint16_t handle_len, uint16_t frame_len)
{
    hmac_user_11v_ctrl_stru *ctrl_info_11v = NULL;
    /* hmac_vap�� mac_user��roam_infoԭ�����Ѿ��й��գ����ﲻ���п� */
    mac_user_stru *mac_user =  mac_res_get_mac_user(hmac_vap->st_vap_base_info.us_assoc_vap_id);
    hmac_roam_info_stru *roam_info = (hmac_roam_info_stru *)hmac_vap->pul_roam_info;
    hmac_user_stru *hmac_user = mac_res_get_hmac_user(hmac_vap->st_vap_base_info.us_assoc_vap_id);
    if (hmac_user == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_btm_parse_bss_list::hmac_user is null}");
        return;
    }

    ctrl_info_11v = &(hmac_user->st_11v_ctrl_info);
    memset_s(ctrl_info_11v, sizeof(hmac_user_11v_ctrl_stru), 0, sizeof(hmac_user_11v_ctrl_stru));

    bsst_req_info->pst_neighbor_bss_list = NULL;
    /* �쳣������� */
    if (handle_len > frame_len) {
        oam_error_log2(0, OAM_SF_ANY, "{hmac_btm_parse_bss_list::handle_len[%d] > frame_len[%d]}",
                       handle_len, frame_len);
        return;
    }

    if (bsst_req_info->st_request_mode.bit_candidate_list_include) {
        bsst_req_info->pst_neighbor_bss_list = hmac_get_neighbor_ie(
            (data + handle_len), frame_len - handle_len, &(bsst_req_info->uc_bss_list_num));
        hmac_bsst_set_max_pref_to_neighbor_bss(hmac_vap, bsst_req_info, mac_user);

        /* �����������ھ�AP��channel list��BSSID list */
        hmac_bsst_neighbor_fill_chan_bssid(roam_info, &ctrl_info_11v->st_scan_h2d_params.st_scan_params,
                                           bsst_req_info->pst_neighbor_bss_list,
                                           bsst_req_info->uc_bss_list_num);
    }
}

OAL_STATIC void hmac_btm_handle_11v_roam(hmac_vap_stru *hmac_vap, hmac_bsst_req_info_stru *bsst_req_info)
{
    hmac_user_11v_ctrl_stru *ctrl_info_11v;
    hmac_bsst_rsp_info_stru *bsst_rsp_info;
    hmac_roam_info_stru *roam_info = NULL;
    uint32_t beacon_period = mac_mib_get_BeaconPeriod(&hmac_vap->st_vap_base_info);
    uint32_t disassoc_time = bsst_req_info->us_disassoc_time * beacon_period;

    /* hmac_vap ԭ�����Ѿ��й��գ����ﲻ���п� */
    hmac_user_stru *hmac_user = mac_res_get_hmac_user(hmac_vap->st_vap_base_info.us_assoc_vap_id);
    if (hmac_user == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_btm_handle_11v_roam::hmac_user is null}");
        return;
    }

    roam_info = (hmac_roam_info_stru *)hmac_vap->pul_roam_info;
    if (roam_info == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_btm_handle_11v_roam::roam info is null}");
        return;
    }

    ctrl_info_11v = &(hmac_user->st_11v_ctrl_info); // already parsed from candidate bss list
    bsst_rsp_info = &(roam_info->st_bsst_rsp_info);
    memset_s(bsst_rsp_info, sizeof(hmac_bsst_rsp_info_stru), 0, sizeof(hmac_bsst_rsp_info_stru));

    /* ˢ��Token */
    ctrl_info_11v->uc_user_bsst_token = bsst_req_info->dialog_token;

    if (ctrl_info_11v->st_scan_h2d_params.st_scan_params.uc_channel_nums > 0) {
        /* ����BSS Rsp֡���� ����Response��AP */
        bsst_rsp_info->uc_status_code = 0;       /* Ĭ������Ϊͬ���л� */
        bsst_rsp_info->uc_termination_delay = 0; /* ����״̬��Ϊ5ʱ��Ч���˴���������Ϊ0 */
        bsst_rsp_info->uc_chl_num =
            ctrl_info_11v->st_scan_h2d_params.st_scan_params.ast_channel_list[0].uc_chan_number;
        bsst_rsp_info->c_rssi =
            hmac_get_rssi_from_scan_result(hmac_vap, hmac_vap->st_vap_base_info.auc_bssid);

        /* register BSS Transition Response callback function:
           * so that check roaming scan results firstly, and then send bsst rsp frame with right status code */
        ctrl_info_11v->mac_11v_callback_fn = hmac_tx_bsst_rsp_action;

        /* us_disassoc_time��ʾThe number of beacon transmission times (TBTTs)������ʱ����Ҫ���� beacon period */
        if (((disassoc_time >= HMAC_11V_REQUEST_DISASSOC_TIME_SCAN_ONE_CHANNEL_TIME) &&
            (disassoc_time < HMAC_11V_REQUEST_DISASSOC_TIME_SCAN_ALL_CHANNEL_TIME)) ||
            (ctrl_info_11v->st_scan_h2d_params.st_scan_params.uc_channel_nums >
            MAC_11V_ROAM_SCAN_ONE_TIME_CHANNEL_NUM)) {
            ctrl_info_11v->en_only_scan_one_time = OAL_TRUE;
        }

        ctrl_info_11v->uc_11v_roam_scan_times = 1;

        hmac_11v_roam_trigger(hmac_vap, hmac_user, bsst_req_info);
    } else {
        /* ɨ���ŵ���Ϊ0�����������Ρ����Disassociation Timer��Ϊ0����Ҫ�ظ��ܾ��� */
        if (bsst_req_info->us_disassoc_time > 0) {
            roam_info->st_bsst_rsp_info.uc_status_code = WNM_BSS_TM_REJECT_NO_SUITABLE_CANDIDATES;
            hmac_tx_bsst_rsp_action(hmac_vap, hmac_user, &(roam_info->st_bsst_rsp_info));
        }
    }
}


uint32_t hmac_rx_bsst_req_action(hmac_vap_stru *hmac_vap, hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf)
{
    uint16_t handle_len;
    dmac_rx_ctl_stru *rx_ctrl = NULL;
    mac_rx_ctl_stru *rx_info = NULL;
    uint16_t frame_len;
    uint8_t *data = NULL;
    hmac_bsst_req_info_stru bsst_req_info = {0};
    mac_user_stru *mac_user = NULL;

    if (oal_any_null_ptr3(hmac_vap, hmac_user, netbuf)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_rx_bsst_req_action::vap or user or buf is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����δ�򿪲����� */
    if (mac_mib_get_MgmtOptionBSSTransitionActivated(&hmac_vap->st_vap_base_info) == OAL_FALSE) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_rx_bsst_req_action:: BSSTransitionActivated is disabled}");
        return OAL_SUCC;
    }

    mac_user = mac_res_get_mac_user(hmac_vap->st_vap_base_info.us_assoc_vap_id);
    if (mac_user == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_rx_bsst_req_action::mac_user is NULL}");
        return OAL_ERR_CODE_ROAM_INVALID_USER;
    }

    rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    rx_info = (mac_rx_ctl_stru *)(&(rx_ctrl->st_rx_info));
    /* ��ȡ֡��ָ�� */
    data = MAC_GET_RX_PAYLOAD_ADDR(rx_info, netbuf);
    frame_len = MAC_GET_RX_CB_PAYLOAD_LEN(rx_info); /* ֡�峤�� */
    /* ֡�����С����Ϊ7 С��7���ʽ�쳣 */
    if (frame_len < HMAC_11V_REQUEST_FRAME_BODY_FIX) {
        oam_error_log1(0, OAM_SF_ANY, "{hmac_rx_bsst_req_action:: frame length error %d.}", frame_len);
        return OAL_FAIL;
    }

    /* ��֡�ĸ��ֲ����������� ���ϲ���� */
    memset_s(&bsst_req_info, sizeof(hmac_bsst_req_info_stru), 0, sizeof(hmac_bsst_req_info_stru));

    /* ����Token */
    bsst_req_info.dialog_token = data[BYTE_OFFSET_2];

    /* ����request mode */
    hmac_btm_parse_req_mode(&bsst_req_info, data[BYTE_OFFSET_3]); /* ֡��offset3��Request Mode field */

    /* us_disassoc_time = The number of beacon transmission times (TBTTs) */
    bsst_req_info.us_disassoc_time = ((uint16_t)(data[BYTE_OFFSET_5]) << BIT_OFFSET_8) | data[BYTE_OFFSET_4];

    bsst_req_info.uc_validity_interval = data[BYTE_OFFSET_6];
    handle_len = 7; /* ǰ��7���ֽ��ѱ������� */
    /* 12�ֽڵ�termination duration ����еĻ� */
    hmac_btm_parse_term_duration(&bsst_req_info, data, &handle_len, frame_len);

    hmac_btm_parse_session_url(&bsst_req_info, data, &handle_len, frame_len);

    /* Candidate bss list����STA��Response frameΪ��ѡ ��Ҫ�����������ڴ˽ṹ���� ���ϲ㴦�� */
    hmac_btm_parse_bss_list(hmac_vap, &bsst_req_info, data, handle_len, frame_len);

    oam_warning_log4(0, OAM_SF_ANY,
        "{hmac_rx_bsst_req_action::associated user mac address=xx:xx:xx:%02x:%02x:%02x bss_list_num=%d}",
        mac_user->auc_user_mac_addr[MAC_ADDR_3], mac_user->auc_user_mac_addr[MAC_ADDR_4],
        mac_user->auc_user_mac_addr[MAC_ADDR_5], bsst_req_info.uc_bss_list_num);

    /* �����ն�����ʵ��11v���� */
    hmac_btm_handle_11v_roam(hmac_vap, &bsst_req_info);

    /* �ͷŶ�̬������ڴ� */
    hmac_rx_bsst_free_res(&bsst_req_info);

    return OAL_SUCC;
}


uint32_t hmac_tx_bsst_rsp_action(void *param1, void *param2, void *param3)
{
    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)param1;
    hmac_user_stru *hmac_user = (hmac_user_stru *)param2;
    hmac_bsst_rsp_info_stru *bsst_rsp_info = (hmac_bsst_rsp_info_stru *)param3;
    oal_netbuf_stru *bsst_rsp_buf = NULL;
    uint16_t frame_len;
    mac_tx_ctl_stru *tx_ctl = NULL;
    uint32_t ret;
    uint8_t vap_id;
    hmac_user_11v_ctrl_stru *ctrl_info_11v = NULL;
    if (oal_any_null_ptr3(hmac_vap, hmac_user, bsst_rsp_info)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_tx_bsst_rsp_action::hmac_vap or hmac_user or bsst_rsp_info is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ctrl_info_11v = &(hmac_user->st_11v_ctrl_info);
    vap_id = hmac_vap->st_vap_base_info.uc_vap_id;

    /* ����bss transition request����֡�ڴ� */
    bsst_rsp_buf = oal_mem_netbuf_alloc(OAL_MGMT_NETBUF, WLAN_MGMT_NETBUF_SIZE, OAL_NETBUF_PRIORITY_HIGH);
    if (bsst_rsp_buf == NULL) {
        oam_error_log0(vap_id, OAM_SF_ANY, "{hmac_tx_bsst_rsp_action::bsst_rsq_buf null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oal_mem_netbuf_trace(bsst_rsp_buf, OAL_TRUE);
    oal_set_netbuf_prev(bsst_rsp_buf, NULL);
    oal_set_netbuf_next(bsst_rsp_buf, NULL);
    /* ���÷�װ����֡�ӿ� */
    frame_len = hmac_encap_bsst_rsp_action(hmac_vap, hmac_user, bsst_rsp_info, bsst_rsp_buf);
    if (frame_len == 0) {
        oal_netbuf_free(bsst_rsp_buf);
        oam_error_log0(vap_id, OAM_SF_ANY, "{hmac_tx_bsst_rsp_action::encap btm rsp action frame failed.}");
        return OAL_FAIL;
    }
    /* ��ʼ��CB */
    memset_s(oal_netbuf_cb(bsst_rsp_buf), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());
    tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(bsst_rsp_buf);
    MAC_GET_CB_TX_USER_IDX(tx_ctl) = hmac_user->st_user_base_info.us_assoc_id;
    MAC_GET_CB_WME_AC_TYPE(tx_ctl) = WLAN_WME_AC_MGMT;
    MAC_GET_CB_MPDU_LEN(tx_ctl) = frame_len;
    oal_netbuf_put(bsst_rsp_buf, frame_len);
    oam_warning_log2(vap_id, OAM_SF_ANY, "{hmac_tx_bsst_rsp_action::11v btm rsp frame, frame_len=%d frametype=%d.}",
                     frame_len, MAC_GET_CB_FRAME_TYPE(tx_ctl));
    /* ���¼���dmac����֡���� */
    ret = hmac_tx_mgmt_send_event(&hmac_vap->st_vap_base_info, bsst_rsp_buf, frame_len);
    if (ret != OAL_SUCC) {
        oal_netbuf_free(bsst_rsp_buf);
        oam_error_log0(vap_id, OAM_SF_ANY, "{hmac_tx_bsst_req_action::tx btm rsp action frame failed.}");
        return ret;
    }
    /* STA������Response�� һ�ν������̾������ ��Ҫ��user�µ�Tokenֵ��1 ���´η���ʹ�� */
    ctrl_info_11v->uc_user_bsst_token = (ctrl_info_11v->uc_user_bsst_token != HMAC_11V_TOKEN_MAX_VALUE) ?
                                        (ctrl_info_11v->uc_user_bsst_token + 1) : 1;
    return OAL_SUCC;
}


uint16_t hmac_encap_bsst_rsp_action(hmac_vap_stru *hmac_vap,
                                    hmac_user_stru *hmac_user,
                                    hmac_bsst_rsp_info_stru *bsst_rsp_info,
                                    oal_netbuf_stru *buffer)
{
    uint16_t index;
    uint8_t *mac_header = NULL;
    uint8_t *payload_addr = NULL;
    hmac_user_11v_ctrl_stru *ctrl_info_11v = NULL;

    if (oal_any_null_ptr4(hmac_vap, hmac_user, bsst_rsp_info, buffer)) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_encap_bsst_rsp_action::vap or user or bsst_rsp_info or buffer is null.}");
        return 0;
    }

    ctrl_info_11v = &(hmac_user->st_11v_ctrl_info);

    mac_header = oal_netbuf_header(buffer);
    payload_addr = mac_netbuf_get_payload(buffer);
    /*************************************************************************/
    /* Management Frame Format */
    /* -------------------------------------------------------------------- */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS| */
    /* -------------------------------------------------------------------- */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  | */
    /* -------------------------------------------------------------------- */
    /*************************************************************************/
    /*************************************************************************/
    /* Set the fields in the frame header */
    /*************************************************************************/
    /* Frame Control Field ��ֻ��Ҫ����Type/Subtypeֵ����������Ϊ0 */
    mac_hdr_set_frame_control(mac_header, WLAN_PROTOCOL_VERSION | WLAN_FC0_TYPE_MGT | WLAN_FC0_SUBTYPE_ACTION);
    /* DA is address of STA addr */
    oal_set_mac_addr(mac_header + WLAN_HDR_ADDR1_OFFSET, hmac_user->st_user_base_info.auc_user_mac_addr);
    /* SA��ֵΪ�����MAC��ַ */
    oal_set_mac_addr(mac_header + WLAN_HDR_ADDR2_OFFSET, mac_mib_get_StationID(&hmac_vap->st_vap_base_info));
    /* TA��ֵΪVAP��BSSID */
    oal_set_mac_addr(mac_header + WLAN_HDR_ADDR3_OFFSET, hmac_vap->st_vap_base_info.auc_bssid);

    /*************************************************************************************************************/
    /* Set the contents of the frame body */
    /*************************************************************************************************************/
    /*************************************************************************************************************/
    /* BSS Transition Response Frame - Frame Body */
    /* ---------------------------------------------------------------------------------------------------------- */
    /* |Category |Action | Token| Status Code | Termination Delay | Target BSSID |   BSS Candidate List Entry */
    /* --------------------------------------------------------------------------------------------------------- */
    /* |1        |1      | 1    |  1          | 1                 | 0-6          |    Optional */
    /* --------------------------------------------------------------------------------------------------------- */
    /*************************************************************************************************************/
    /* ������ָ��frame body��ʼλ�� */
    index = 0;
    /* ����Category */
    payload_addr[index] = MAC_ACTION_CATEGORY_WNM;
    index++;
    /* ����Action */
    payload_addr[index] = MAC_WNM_ACTION_BSS_TRANSITION_MGMT_RESPONSE;
    index++;
    /* ����Dialog Token */
    payload_addr[index] = ctrl_info_11v->uc_user_bsst_token;
    index++;
    /* ����Status Code */
    payload_addr[index] = bsst_rsp_info->uc_status_code;
    index++;
    /* ����Termination Delay */
    payload_addr[index] = bsst_rsp_info->uc_termination_delay;
    index++;
    /* ����Target BSSID */
    if (bsst_rsp_info->uc_status_code == 0) {
        if (memcpy_s(payload_addr + index, WLAN_MGMT_NETBUF_SIZE - index,
                     bsst_rsp_info->auc_target_bss_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "hwifi_config_init_fcc_ce_txpwr_nvram::memcpy fail!");
            return 0;
        }
        index += WLAN_MAC_ADDR_LEN;
    }
    return (uint16_t)(index + MAC_80211_FRAME_LEN);
}

OAL_STATIC OAL_INLINE void hmac_parse_neighbor_report_subelement(int16_t sub_ie_len,
    uint8_t *ie_data, hmac_neighbor_bss_info_stru *bss_list_alloc, uint8_t bss_list_index)
{
    while (sub_ie_len > 1) {
        switch (ie_data[0]) {
            case HMAC_NEIGH_SUB_ID_BSS_CANDIDATE_PERF: /* ռ��3���ֽ� */
                if (sub_ie_len < 3) { // ��ie���ٰ���3���ֽ�
                    oam_error_log1(0, OAM_SF_ANY, "hmac_parse_neighbor_report_subelement::PREF len[%d]", sub_ie_len);
                    return;
                }
                bss_list_alloc[bss_list_index].uc_candidate_perf = ie_data[BYTE_OFFSET_2];
                sub_ie_len -= (HMAC_11V_PERFERMANCE_ELEMENT_LEN + MAC_IE_HDR_LEN);
                ie_data += (HMAC_11V_PERFERMANCE_ELEMENT_LEN + MAC_IE_HDR_LEN);
                break;
            case HMAC_NEIGH_SUB_ID_TERM_DURATION: /* ռ��12���ֽ� */
                if (sub_ie_len < 12) { // ��ie���ٰ���12���ֽ�
                    oam_error_log1(0, OAM_SF_ANY,
                        "hmac_parse_neighbor_report_subelement::DURATION len[%d]", sub_ie_len);
                    return;
                }
                if (memcpy_s(bss_list_alloc[bss_list_index].st_term_duration.auc_termination_tsf,
                             sizeof(bss_list_alloc[bss_list_index].st_term_duration.auc_termination_tsf),
                             ie_data + 2, HMAC_11V_TERMINATION_TSF_LENGTH) != EOK) { /* 2��ͷ�ĳ��� */
                    oam_error_log0(0, OAM_SF_ANY, "hmac_handle_ie_by_data_length::memcpy fail!");
                    return;
                }
                bss_list_alloc[bss_list_index].st_term_duration.us_duration_min =
                   (((uint16_t)ie_data[BYTE_OFFSET_11]) << BIT_OFFSET_8) | (ie_data[BYTE_OFFSET_10]);
                sub_ie_len -= (HMAC_11V_TERMINATION_ELEMENT_LEN + MAC_IE_HDR_LEN);
                ie_data += (HMAC_11V_TERMINATION_ELEMENT_LEN + MAC_IE_HDR_LEN);
                break;
#ifdef _PRE_WLAN_FEATURE_MBO
            case HMAC_NEIGH_SUB_ID_VENDOR_SPECIFIC:
                if (sub_ie_len < 10) { // ��ie���ٰ���10���ֽ�
                    oam_error_log1(0, OAM_SF_ANY, "hmac_parse_neighbor_report_subelement::MBO len[%d]", sub_ie_len);
                    return;
                }
                hmac_handle_ie_specific_mbo(ie_data, bss_list_alloc, bss_list_index); // ��װ������������
                sub_ie_len -= (ie_data[1] + MAC_IE_HDR_LEN);
                ie_data += (ie_data[1] + MAC_IE_HDR_LEN);
                break;
#endif
            /* ����IE���� ������ */
            default:
                sub_ie_len -= (ie_data[1] + MAC_IE_HDR_LEN);
                ie_data += (ie_data[1] + MAC_IE_HDR_LEN);
                break;
        }
    }
}

OAL_STATIC OAL_INLINE void hmac_analysis_ie_bssid_info(hmac_neighbor_bss_info_stru *bss_list_alloc,
    uint8_t bss_list_index, uint8_t *ie_data)
{
    bss_list_alloc[bss_list_index].st_bssid_info.bit_ap_reachability =
        (ie_data[BYTE_OFFSET_8] & BIT1) | (ie_data[BYTE_OFFSET_8] & BIT0); /* bit0-1 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_security =
        (ie_data[BYTE_OFFSET_8] & BIT2) ? OAL_TRUE : OAL_FALSE; /* bit2 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_key_scope =
        (ie_data[BYTE_OFFSET_8] & BIT3) ? OAL_TRUE : OAL_FALSE; /* bit3 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_spectrum_mgmt =
        (ie_data[BYTE_OFFSET_8] & BIT4) ? OAL_TRUE : OAL_FALSE; /* bit4 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_qos =
        (ie_data[BYTE_OFFSET_8] & BIT5) ? OAL_TRUE : OAL_FALSE; /* bit5 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_apsd =
        (ie_data[BYTE_OFFSET_8] & BIT6) ? OAL_TRUE : OAL_FALSE; /* bit6 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_radio_meas =
        (ie_data[BYTE_OFFSET_8] & BIT7) ? OAL_TRUE : OAL_FALSE; /* bit7 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_delay_block_ack =
        (ie_data[BYTE_OFFSET_9] & BIT0) ? OAL_TRUE : OAL_FALSE; /* bit0 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_immediate_block_ack =
        (ie_data[BYTE_OFFSET_9] & BIT1) ? OAL_TRUE : OAL_FALSE; /* bit1 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_mobility_domain =
        (ie_data[BYTE_OFFSET_9] & BIT2) ? OAL_TRUE : OAL_FALSE; /* bit2 */
    bss_list_alloc[bss_list_index].st_bssid_info.bit_high_throughput =
        (ie_data[BYTE_OFFSET_9] & BIT3) ? OAL_TRUE : OAL_FALSE; /* bit3 */
    /* �����ֶβ����� */
    bss_list_alloc[bss_list_index].uc_opt_class = ie_data[BYTE_OFFSET_12];
    bss_list_alloc[bss_list_index].uc_chl_num = ie_data[BYTE_OFFSET_13];
    bss_list_alloc[bss_list_index].uc_phy_type = ie_data[BYTE_OFFSET_14];
}

static uint8_t hmac_get_neighbor_ie_number(uint8_t *ie_data_find, uint16_t len)
{
    uint8_t bss_number = 0;
    uint8_t *ie_data = NULL;

    /* ��ȷ�Ϻ��ж��ٸ�neighbor list */
    while (ie_data_find != NULL) {
        ie_data = mac_find_ie(MAC_EID_NEIGHBOR_REPORT, ie_data_find, len);
        /* û�ҵ����˳�ѭ�� */
        if (ie_data == NULL) {
            break;
        }
        bss_number++; /* Neighbor Report IE ������1 */

        if (len >= ie_data[1] + MAC_IE_HDR_LEN) {
            ie_data_find += (ie_data[1] + MAC_IE_HDR_LEN);
            len -= (ie_data[1] + MAC_IE_HDR_LEN);
        } else {
            oam_warning_log2(0, OAM_SF_ANY,
                             "{hmac_get_neighbor_ie_number::ie len[%d] greater than remain frame len[%d]!}",
                             ie_data[1] + MAC_IE_HDR_LEN, len);
            return 0;
        }
    }

    return bss_number;
}

OAL_STATIC uint32_t hmac_get_neighbor_bss_info(hmac_neighbor_bss_info_stru *bss_list_alloc,
    uint8_t *ie_data_find, uint8_t bss_number, uint16_t len_find)
{
    uint8_t minmum_ie_len = 13;
    uint8_t bss_list_index, neighbor_ie_len;
    int16_t sub_ie_len;
    uint8_t *ie_data = NULL;

    for (bss_list_index = 0; bss_list_index < bss_number; bss_list_index++) {
        /* ǰ���Ѿ���ѯ��һ�Σ����ﲻ�᷵�ؿգ����Բ����ж� */
        ie_data = mac_find_ie(MAC_EID_NEIGHBOR_REPORT, ie_data_find, len_find);
        if (ie_data == NULL) {
            oam_error_log2(0, OAM_SF_ANY,
                           "{hmac_get_neighbor_bss_info::not find ie! bss_list_index[%d], bss_number[%d].}",
                           bss_list_index, bss_number);
            break;
        }

        neighbor_ie_len = ie_data[1];  // Ԫ�س���
        if (neighbor_ie_len < minmum_ie_len) {
            oam_warning_log2(0, OAM_SF_ANY, "{hmac_get_neighbor_ie::neighbor_ie_len[%d] < minmum_ie_len[%d].}",
                             neighbor_ie_len, minmum_ie_len);
            oal_mem_free_m(bss_list_alloc, OAL_TRUE);
            return OAL_FAIL;
        }
        /* ����Neighbor Report IE�ṹ�� ֡��ֻ����subelement 3 4������subelement�ѱ����˵� */
        if (memcpy_s(bss_list_alloc[bss_list_index].auc_mac_addr, WLAN_MAC_ADDR_LEN,
            ie_data + MAC_IE_HDR_LEN, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "hmac_get_neighbor_bss_info::memcpy fail!");
        }

        /* ����BSSID Information */
        hmac_analysis_ie_bssid_info(bss_list_alloc, bss_list_index, ie_data);

        /* ����Subelement ���ȴ�����Сie���ȲŴ���subelement ֻ����3 4 subelement */
        /* CSEC:��ʱ�����жϱ�֤neighbor_ie_len����Ϊ14,����ie_data���ʲ���Խ�� */
        if (neighbor_ie_len > minmum_ie_len) {
            sub_ie_len = neighbor_ie_len - minmum_ie_len; /* subelement���� */
            ie_data += (minmum_ie_len + MAC_IE_HDR_LEN);   /* ֡�������ƶ���subelement�� */
            hmac_parse_neighbor_report_subelement(sub_ie_len, ie_data, bss_list_alloc, bss_list_index);
        }
        oam_info_log4(0, OAM_SF_ANY,
            "hmac_get_neighbor_ie::bss_index=%d, chl_num=%d, candidate_pref=%d, max_pref=%d",
            bss_list_index, bss_list_alloc[bss_list_index].uc_chl_num,
            bss_list_alloc[bss_list_index].uc_candidate_perf,
            bss_list_alloc[bss_list_index].uc_max_candidate_perf);
        ie_data_find += (neighbor_ie_len + MAC_IE_HDR_LEN);
        len_find -= (neighbor_ie_len + MAC_IE_HDR_LEN);
    }
    return OAL_SUCC;
}

hmac_neighbor_bss_info_stru *hmac_get_neighbor_ie(uint8_t *data, uint16_t len, uint8_t *bss_num)
{
    /* Neighbor Report element format */
    /* ------------------------------------------------------------------------------ -------------_----------- */
    /* | Ele ID | Length | BSSID | BSSID Info | Operating Class | Chan Num |  PHY Type | Optional Subelements | */
    /* -------------------------------------------------------------------------------------------------------- */
    /* |   1    |    1   |   6   |     4      |        1        |    1     |     1     |       variable       | */
    /* -------------------------------------------------------------------------------------------------------- */
    uint8_t bss_number;
    hmac_neighbor_bss_info_stru *bss_list_alloc = NULL;

    if (oal_any_null_ptr2(data, bss_num)) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_get_neighbor_ie::data or bss_num is null.}");
        if (bss_num != NULL) {
            *bss_num = 0;
        }
        return NULL;
    }

    *bss_num = 0;

    /* �����֡����Ϊ0������Ҫ���н����� */
    if (len == 0) {
        return NULL;
    }

    bss_number = hmac_get_neighbor_ie_number(data, len);
    /* ���neighbor ie ����Ϊ0 ֱ�ӷ��� */
    if (bss_number == 0) {
        return NULL;
    }
    /* ���ݻ�ԭ���ٴδ�ͷ�������� */
    bss_list_alloc = (hmac_neighbor_bss_info_stru *)oal_mem_alloc_m(OAL_MEM_POOL_ID_LOCAL,
        bss_number * sizeof(hmac_neighbor_bss_info_stru), OAL_TRUE);
    if (bss_list_alloc == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_get_neighbor_ie::pst_bss_list null pointer.}");
        return NULL;
    }
    memset_s(bss_list_alloc, bss_number * sizeof(hmac_neighbor_bss_info_stru),
             0, bss_number * sizeof(hmac_neighbor_bss_info_stru));
    if (hmac_get_neighbor_bss_info(bss_list_alloc, data, bss_number, len) != OAL_SUCC) {
        return NULL;
    }
    *bss_num = bss_number;
    return bss_list_alloc;
}
#endif  // _PRE_WLAN_FEATURE_11V_ENABLE
