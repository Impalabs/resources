

/* 1 ͷ�ļ����� */
#include "mac_frame_inl.h"
#include "hmac_sme_sta.h"
#include "hmac_mgmt_join.h"
#include "hmac_encap_frame_sta.h"
#include "hmac_p2p.h"
#include "hmac_11i.h"
#include "hmac_sae.h"
#include "plat_pm_wlan.h"
#ifdef _PRE_WLAN_FEATURE_SNIFFER
#ifdef CONFIG_HW_SNIFFER
#include <hwnet/ipv4/sysctl_sniffer.h>
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_MGMT_JOIN_C

uint32_t hmac_mgmt_timeout_sta(void *arg)
{
    hmac_vap_stru *hmac_vap = NULL;
    hmac_mgmt_timeout_param_stru *timeout_param;

    timeout_param = (hmac_mgmt_timeout_param_stru *)arg;
    if (timeout_param == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(timeout_param->uc_vap_id);
    if (hmac_vap == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_fsm_call_func_sta(hmac_vap, HMAC_FSM_INPUT_TIMER0_OUT, timeout_param);
}


void hmac_update_join_req_params_2040(mac_vap_stru *mac_vap, mac_bss_dscr_stru *bss_dscr)
{
    if (((mac_mib_get_HighThroughputOptionImplemented(mac_vap) == OAL_FALSE) &&
        (mac_mib_get_VHTOptionImplemented(mac_vap) == OAL_FALSE)) ||
        ((bss_dscr->en_ht_capable == OAL_FALSE) && (bss_dscr->en_vht_capable == OAL_FALSE))) {
        mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
        return;
    }

    /* ʹ��40MHz */
    /* (1) �û�����"40MHz����"����(��STA�� dot11FortyMHzOperationImplementedΪtrue) */
    /* (2) AP��40MHz���� */
    if (OAL_TRUE == mac_mib_get_FortyMHzOperationImplemented(mac_vap)) {
        switch (bss_dscr->en_channel_bandwidth) {
            case WLAN_BAND_WIDTH_40PLUS:
            case WLAN_BAND_WIDTH_80PLUSPLUS:
            case WLAN_BAND_WIDTH_80PLUSMINUS:
                mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_40PLUS;
                break;

            case WLAN_BAND_WIDTH_40MINUS:
            case WLAN_BAND_WIDTH_80MINUSPLUS:
            case WLAN_BAND_WIDTH_80MINUSMINUS:
                mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_40MINUS;
                break;

            default:
                mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
                break;
        }
    }

    /* ����STA�������APһ�� */
    /* (1) STA AP��֧��11AC */
    /* (2) STA֧��40M����(FortyMHzOperationImplementedΪTRUE)��
           ���ƻ���ֹ2GHT40ʱ��2G��FortyMHzOperationImplemented=FALSE�������´��� */
    /* (3) STA֧��80M����(��STA�� dot11VHTChannelWidthOptionImplementedΪ0) */
    if ((OAL_TRUE == mac_mib_get_VHTOptionImplemented(mac_vap)) &&
        (bss_dscr->en_vht_capable == OAL_TRUE)) {
        if (OAL_TRUE == mac_mib_get_FortyMHzOperationImplemented(mac_vap)) {
            /* ������mac device���������� */
            mac_vap->st_channel.en_bandwidth = mac_vap_get_bandwith(mac_mib_get_dot11VapMaxBandWidth(mac_vap),
                bss_dscr->en_channel_bandwidth);
        }
    }

    oam_warning_log2(mac_vap->uc_vap_id, OAM_SF_2040,
                     "{hmac_update_join_req_params_2040::en_channel_bandwidth=%d, mac vap bw[%d].}",
                     bss_dscr->en_channel_bandwidth, mac_vap->st_channel.en_bandwidth);

    /* ���AP��STAͬʱ֧��20/40��������ܣ���ʹ��STA��Ƶ�׹����� */
    if ((OAL_TRUE == mac_mib_get_2040BSSCoexistenceManagementSupport(mac_vap)) &&
        (bss_dscr->uc_coex_mgmt_supp == 1)) {
        mac_mib_set_SpectrumManagementImplemented(mac_vap, OAL_TRUE);
    }
}


void hmac_update_join_req_params_prot_sta(hmac_vap_stru *hmac_vap, hmac_join_req_stru *join_req)
{
    if (WLAN_MIB_DESIRED_BSSTYPE_INFRA == mac_mib_get_DesiredBSSType(&hmac_vap->st_vap_base_info)) {
        hmac_vap->st_vap_base_info.st_cap_flag.bit_wmm_cap = join_req->st_bss_dscr.uc_wmm_cap;
        mac_vap_set_uapsd_cap(&hmac_vap->st_vap_base_info, join_req->st_bss_dscr.uc_uapsd_cap);
    }

    hmac_update_join_req_params_2040(&(hmac_vap->st_vap_base_info), &(join_req->st_bss_dscr));
}


void hmac_sta_bandwidth_down_by_channel(mac_vap_stru *mac_vap)
{
    oal_bool_enum_uint8 bandwidth_change_to_20M = OAL_FALSE;

    switch (mac_vap->st_channel.en_bandwidth) {
        case WLAN_BAND_WIDTH_40PLUS:
            /* 1. 64 144 161�ŵ���֧��40+ */
            if ((mac_vap->st_channel.uc_chan_number >= 64 && mac_vap->st_channel.uc_chan_number < 100) ||
                (mac_vap->st_channel.uc_chan_number >= 144 && mac_vap->st_channel.uc_chan_number < 149) ||
                (mac_vap->st_channel.uc_chan_number >= 161 && mac_vap->st_channel.uc_chan_number < 184)) {
                bandwidth_change_to_20M = OAL_TRUE;
            }
            break;

        case WLAN_BAND_WIDTH_40MINUS:
            /* 1. 100 149 184�ŵ���֧��40- */
            if ((mac_vap->st_channel.uc_chan_number > 64 && mac_vap->st_channel.uc_chan_number <= 100) ||
                (mac_vap->st_channel.uc_chan_number > 144 && mac_vap->st_channel.uc_chan_number <= 149) ||
                (mac_vap->st_channel.uc_chan_number > 161 && mac_vap->st_channel.uc_chan_number <= 184)) {
                bandwidth_change_to_20M = OAL_TRUE;
            }
            break;

        case WLAN_BAND_WIDTH_80PLUSPLUS:
        case WLAN_BAND_WIDTH_80PLUSMINUS:
        case WLAN_BAND_WIDTH_80MINUSPLUS:
        case WLAN_BAND_WIDTH_80MINUSMINUS:
            /* 165�ŵ���֧��80M, ��ʱ�����ǳ��ָ����ŵ��쳣���� */
            if (mac_vap->st_channel.uc_chan_number == 165) {
                bandwidth_change_to_20M = OAL_TRUE;
            }
            break;

        /* 160M�Ĵ���У����ʱ������ */
        default:
            break;
    }

    /* ��Ҫ������ */
    if (bandwidth_change_to_20M == OAL_TRUE) {
        oam_warning_log2(mac_vap->uc_vap_id, OAM_SF_SCAN,
            "{hmac_sta_bandwidth_down_by_channel:: channel[%d] not support bandwidth[%d], need to change to 20M.}",
            mac_vap->st_channel.uc_chan_number, mac_vap->st_channel.en_bandwidth);

        mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
    }
}


oal_bool_enum_uint8 hmac_is_rate_support(uint8_t *rates, uint8_t rate_num, uint8_t rate)
{
    oal_bool_enum_uint8 rate_is_supp = OAL_FALSE;
    uint8_t loop;

    if (rates == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_is_rate_support::rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    for (loop = 0; loop < rate_num; loop++) {
        if ((rates[loop] & 0x7F) == rate) {
            rate_is_supp = OAL_TRUE;
            break;
        }
    }

    return rate_is_supp;
}


oal_bool_enum_uint8 hmac_is_support_11grate(uint8_t *rates, uint8_t rate_num)
{
    if (rates == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_is_rate_support::rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((OAL_TRUE == hmac_is_rate_support(rates, rate_num, 0x0C))
        || (OAL_TRUE == hmac_is_rate_support(rates, rate_num, 0x12))
        || (OAL_TRUE == hmac_is_rate_support(rates, rate_num, 0x18))
        || (OAL_TRUE == hmac_is_rate_support(rates, rate_num, 0x24))
        || (OAL_TRUE == hmac_is_rate_support(rates, rate_num, 0x30))
        || (OAL_TRUE == hmac_is_rate_support(rates, rate_num, 0x48))
        || (OAL_TRUE == hmac_is_rate_support(rates, rate_num, 0x60))
        || (OAL_TRUE == hmac_is_rate_support(rates, rate_num, 0x6C))) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}


oal_bool_enum_uint8 hmac_is_support_11brate(uint8_t *rates, uint8_t rate_num)
{
    if (rates == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_is_support_11brate::rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((hmac_is_rate_support(rates, rate_num, 0x02) == OAL_TRUE)
        || (hmac_is_rate_support(rates, rate_num, 0x04) == OAL_TRUE)
        || (hmac_is_rate_support(rates, rate_num, 0x0B) == OAL_TRUE)
        || (hmac_is_rate_support(rates, rate_num, 0x16) == OAL_TRUE)) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}
void hmac_sta_get_user_protocol_by_sup_rate(mac_bss_dscr_stru *bss_dscr,
    wlan_protocol_enum_uint8 *protocol_mode)
{
    if (hmac_is_support_11grate(bss_dscr->auc_supp_rates, bss_dscr->uc_num_supp_rates) == OAL_TRUE) {
        *protocol_mode = WLAN_LEGACY_11G_MODE;
        if (hmac_is_support_11brate(bss_dscr->auc_supp_rates, bss_dscr->uc_num_supp_rates) == OAL_TRUE) {
            *protocol_mode = WLAN_MIXED_ONE_11G_MODE;
        }
    } else if (hmac_is_support_11brate(bss_dscr->auc_supp_rates, bss_dscr->uc_num_supp_rates) == OAL_TRUE) {
        *protocol_mode = WLAN_LEGACY_11B_MODE;
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_sta_get_user_protocol::get user protocol failed.}");
    }
}

void hmac_sta_get_user_protocol(mac_bss_dscr_stru *bss_dscr,
    wlan_protocol_enum_uint8 *protocol_mode)
{
    if (g_wlan_spec_cfg->feature_11ax_is_open && (bss_dscr->en_he_capable == OAL_TRUE)) {
        *protocol_mode = WLAN_HE_MODE;
    } else if (bss_dscr->en_vht_capable == OAL_TRUE) {
        *protocol_mode = WLAN_VHT_MODE;
    } else if (bss_dscr->en_ht_capable == OAL_TRUE) {
        *protocol_mode = WLAN_HT_MODE;
    } else {
        if (bss_dscr->st_channel.en_band == WLAN_BAND_5G) { /* �ж��Ƿ���5G */
            *protocol_mode = WLAN_LEGACY_11A_MODE;
        } else {
            /* Ƕ������Ż���װ */
            hmac_sta_get_user_protocol_by_sup_rate(bss_dscr, protocol_mode);
        }
    }
}


uint32_t hmac_sta_get_user_protocol_etc(mac_bss_dscr_stru *bss_dscr,
    wlan_protocol_enum_uint8 *protocol_mode)
{
    /* ��α��� */
    if (oal_any_null_ptr2(bss_dscr, protocol_mode)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_get_user_protocol_etc::bss_dscr or protocol_mode is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    hmac_sta_get_user_protocol(bss_dscr, protocol_mode);

    return OAL_SUCC;
}


OAL_STATIC uint32_t hmac_sta_send_auth_seq3(hmac_vap_stru *sta, uint8_t *mac_hdr)
{
    oal_netbuf_stru *auth_frame = NULL;
    hmac_user_stru *hmac_user_ap = NULL;
    mac_tx_ctl_stru *tx_ctl = NULL;

    uint16_t auth_frame_len;
    uint32_t ret;

    /* ׼��seq = 3����֤֡ */
    auth_frame = oal_mem_netbuf_alloc(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (auth_frame == NULL) {
        oam_error_log0(sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::pst_auth_frame null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    oal_mem_netbuf_trace(auth_frame, OAL_TRUE);

    memset_s(oal_netbuf_cb(auth_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    auth_frame_len = hmac_mgmt_encap_auth_req_seq3(sta, (uint8_t *)oal_netbuf_header(auth_frame), mac_hdr);
    oal_netbuf_put(auth_frame, auth_frame_len);

    hmac_user_ap = mac_res_get_hmac_user(sta->st_vap_base_info.us_assoc_vap_id);
    if (hmac_user_ap == NULL) {
        oal_netbuf_free(auth_frame);
        oam_error_log1(sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                       "{hmac_wait_auth_sta::pst_hmac_user[%d] null.}",
                       sta->st_vap_base_info.us_assoc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��д���ͺͷ��������Ҫ�Ĳ��� */
    tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(auth_frame);
    MAC_GET_CB_MPDU_LEN(tx_ctl) = auth_frame_len;                                 /* ������Ҫ֡���� */
    MAC_GET_CB_TX_USER_IDX(tx_ctl) = hmac_user_ap->st_user_base_info.us_assoc_id; /* �������Ҫ��ȡ�û� */

    /* ���¼���dmac���� */
    ret = hmac_tx_mgmt_send_event(&sta->st_vap_base_info, auth_frame, auth_frame_len);
    if (ret != OAL_SUCC) {
        oal_netbuf_free(auth_frame);
        oam_warning_log1(sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_wait_auth_sta::hmac_tx_mgmt_send_event failed[%d].}", ret);
        return ret;
    }

    frw_timer_immediate_destroy_timer_m(&sta->st_mgmt_timer);

    /* ����״̬ΪMAC_VAP_STATE_STA_WAIT_AUTH_SEQ4����������ʱ�� */
    hmac_fsm_change_state(sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4);

    frw_timer_create_timer_m(&sta->st_mgmt_timer, hmac_mgmt_timeout_sta, sta->st_mgmt_timer.timeout,
        &sta->st_mgmt_timetout_param, OAL_FALSE, OAM_MODULE_ID_HMAC, sta->st_vap_base_info.core_id);
    return OAL_SUCC;
}


uint32_t hmac_sta_wait_join_dtim_config(hmac_vap_stru *hmac_sta, hmac_join_req_stru *join_req)
{
    dmac_ctx_set_dtim_tsf_reg_stru *set_dtim_tsf_reg_params = NULL;
    frw_event_mem_stru *event_mem = NULL;
    frw_event_stru *event = NULL;
    uint32_t ret;

    /* ���¼���DMAC, �����¼��ڴ� */
    event_mem = frw_event_alloc_m(sizeof(dmac_ctx_set_dtim_tsf_reg_stru));
    if (event_mem == NULL) {
        oam_error_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_join_dtim_config::event_mem alloc null, size[%d].}",
                       sizeof(dmac_ctx_set_dtim_tsf_reg_stru));
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��д�¼� */
    event = frw_get_event_stru(event_mem);

    frw_event_hdr_init(&(event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG,
                       sizeof(dmac_ctx_set_dtim_tsf_reg_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       hmac_sta->st_vap_base_info.uc_chip_id,
                       hmac_sta->st_vap_base_info.uc_device_id,
                       hmac_sta->st_vap_base_info.uc_vap_id);

    set_dtim_tsf_reg_params = (dmac_ctx_set_dtim_tsf_reg_stru *)event->auc_event_data;

    /* ��Ap bssid��tsf REG ����ֵ�������¼�payload�� */
    set_dtim_tsf_reg_params->dtim_cnt = join_req->st_bss_dscr.uc_dtim_cnt;
    set_dtim_tsf_reg_params->dtim_period = join_req->st_bss_dscr.uc_dtim_period;
    set_dtim_tsf_reg_params->us_tsf_bit0 = BIT0;
    memcpy_s(set_dtim_tsf_reg_params->auc_bssid, WLAN_MAC_ADDR_LEN,
             hmac_sta->st_vap_base_info.auc_bssid, WLAN_MAC_ADDR_LEN);

    /* �ַ��¼� */
    ret = frw_event_dispatch_event(event_mem);
    frw_event_free_m(event_mem);
    return ret;
}

uint32_t hmac_sta_wait_join(hmac_vap_stru *hmac_sta, void *msg)
{
    hmac_join_req_stru *join_req = NULL;
    hmac_join_rsp_stru join_rsp;
    uint32_t ret;

    if (oal_any_null_ptr2(hmac_sta, msg)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_wait_join::hmac_sta or msg is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 1102 P2PSTA���� todo ���²���ʧ�ܵĻ���Ҫ���ض����Ǽ����·�Join���� */
    ret = hmac_p2p_check_can_enter_state(&(hmac_sta->st_vap_base_info), HMAC_FSM_INPUT_ASOC_REQ);
    if (ret != OAL_SUCC) {
        /* ���ܽ������״̬�������豸æ */
        oam_warning_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_sta_wait_join fail,device busy: ret=%d}\r\n", ret);
        return OAL_ERR_CODE_CONFIG_BUSY;
    }

    join_req = (hmac_join_req_stru *)msg;
    /* ����JOIN REG params ��MIB��MAC�Ĵ��� */
    ret = hmac_sta_update_join_req_params(hmac_sta, join_req);
    if (ret != OAL_SUCC) {
        oam_error_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_join::get hmac_sta_update_join_req_params fail[%d]!}", ret);
        return ret;
    }

    /* ��proxy staģʽʱ����Ҫ��dtim�������õ�dmac */
    ret = hmac_sta_wait_join_dtim_config(hmac_sta, join_req);
    if (ret != OAL_SUCC) {
        oam_error_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_join::get hmac_sta_wait_join_dtim_config fail[%d]!}", ret);
    return ret;
    }

    join_rsp.en_result_code = HMAC_MGMT_SUCCESS;
    /* �л�STA״̬��JOIN_COMP */
    hmac_fsm_change_state(hmac_sta, MAC_VAP_STATE_STA_JOIN_COMP);

    /* ����JOIN�ɹ���Ϣ��SME hmac_handle_join_rsp_sta */
    hmac_send_rsp_to_sme_sta(hmac_sta, HMAC_SME_JOIN_RSP, (uint8_t *)&join_rsp);

    oam_warning_log4(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                     "{hmac_sta_wait_join::Join AP channel=%d idx=%d, bandwidth=%d Beacon Period=%d SUCC.}",
                     join_req->st_bss_dscr.st_channel.uc_chan_number, join_req->st_bss_dscr.st_channel.uc_chan_idx,
                     hmac_sta->st_vap_base_info.st_channel.en_bandwidth, join_req->st_bss_dscr.us_beacon_period);

    return OAL_SUCC;
}

OAL_STATIC uint32_t hmac_sta_join_rx_pre_proc(hmac_vap_stru *hmac_sta, void *param,
    uint8_t **mac_hdr, uint16_t *rx_len)
{
    dmac_wlan_crx_event_stru *mgmt_rx_event = NULL;
    dmac_rx_ctl_stru *rx_ctrl = NULL;
    mac_rx_ctl_stru *rx_info = NULL;

    if (oal_any_null_ptr2(hmac_sta, param)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_join_rx_pre_proc::hmac_sta or param is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    mgmt_rx_event = (dmac_wlan_crx_event_stru *)param;
    rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(mgmt_rx_event->pst_netbuf);
    rx_info = (mac_rx_ctl_stru *)(&(rx_ctrl->st_rx_info));
    *mac_hdr = (uint8_t *)MAC_GET_RX_CB_MAC_HEADER_ADDR(rx_info);
    *rx_len = rx_ctrl->st_rx_info.us_frame_len; /* ��Ϣ�ܳ���,������FCS */

    return OAL_SUCC;
}

OAL_STATIC uint32_t hmac_sta_join_rx_beacon(hmac_vap_stru *hmac_sta, uint8_t *mac_hdr, uint16_t rx_len)
{
    uint8_t *bssid = NULL;
    uint8_t *tim_elm = NULL;
    frw_event_mem_stru *event_mem = NULL;
    frw_event_stru *event = NULL;
    dmac_ctx_set_dtim_tsf_reg_stru st_set_dtim_tsf_reg_params = { 0 };
    uint8_t auc_bssid[WLAN_MAC_ADDR_LEN] = { 0 };

    /* ��ȡBeacon֡�е�mac��ַ����AP��mac��ַ */
    mac_get_bssid(mac_hdr, auc_bssid, sizeof(auc_bssid));

    /* ���STA�����AP mac��ַ�����beacon֡��mac��ַƥ�䣬�����beacon֡�е�DTIM countֵ��STA����mib���� */
    bssid = &auc_bssid[0];
    if (oal_memcmp(bssid, hmac_sta->st_vap_base_info.auc_bssid, WLAN_MAC_ADDR_LEN) != 0) {
        return OAL_FAIL;
    }

    tim_elm = mac_find_ie(MAC_EID_TIM, mac_hdr + MAC_TAG_PARAM_OFFSET, rx_len - MAC_TAG_PARAM_OFFSET);
    /* ��tim IE����ȡ DTIM countֵ,д�뵽MAC H/W REG�� */
    if ((tim_elm != NULL) && (tim_elm[1] >= MAC_MIN_TIM_LEN)) {
        mac_mib_set_dot11dtimperiod(&hmac_sta->st_vap_base_info, tim_elm[BYTE_OFFSET_3]);

        /* ��dtim_cnt��dtim_period�������¼�payload�� */
        st_set_dtim_tsf_reg_params.dtim_cnt = tim_elm[BYTE_OFFSET_2];
        st_set_dtim_tsf_reg_params.dtim_period = tim_elm[BYTE_OFFSET_3];
    } else {
        oam_warning_log0(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
            "{hmac_sta_join_rx_beacon::Do not find Tim ie.}");
    }

    /* ��Ap bssid��tsf REG ����ֵ�������¼�payload�� */
    memcpy_s(st_set_dtim_tsf_reg_params.auc_bssid, WLAN_MAC_ADDR_LEN, auc_bssid, WLAN_MAC_ADDR_LEN);
    st_set_dtim_tsf_reg_params.us_tsf_bit0 = BIT0;

    /* ���¼���DMAC, �����¼��ڴ� */
    event_mem = frw_event_alloc_m(sizeof(dmac_ctx_set_dtim_tsf_reg_stru));
    if (event_mem == NULL) {
        oam_error_log0(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_sta_join_rx_beacon::event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��д�¼� */
    event = frw_get_event_stru(event_mem);

    frw_event_hdr_init(&(event->st_event_hdr), FRW_EVENT_TYPE_WLAN_CTX,
        DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG, sizeof(dmac_ctx_set_dtim_tsf_reg_stru),
        FRW_EVENT_PIPELINE_STAGE_1, hmac_sta->st_vap_base_info.uc_chip_id,
        hmac_sta->st_vap_base_info.uc_device_id, hmac_sta->st_vap_base_info.uc_vap_id);

    /* �������� */
    if (memcpy_s(frw_get_event_payload(event_mem), sizeof(dmac_ctx_set_dtim_tsf_reg_stru),
        (uint8_t *)&st_set_dtim_tsf_reg_params, sizeof(dmac_ctx_set_dtim_tsf_reg_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_SCAN, "hmac_sta_join_rx_beacon::memcpy fail!");
        frw_event_free_m(event_mem);
        return OAL_FAIL;
    }

    /* �ַ��¼� */
    frw_event_dispatch_event(event_mem);
    frw_event_free_m(event_mem);

    return OAL_SUCC;
}

uint32_t hmac_sta_wait_join_rx(hmac_vap_stru *hmac_sta, void *param)
{
    uint8_t *mac_hdr = NULL;
    uint16_t rx_len;
    uint32_t ret;

    ret = hmac_sta_join_rx_pre_proc(hmac_sta, param, &mac_hdr, &rx_len);
    if (ret != OAL_SUCC) {
        return ret;
    }
    /* ��WAIT_JOIN״̬�£�������յ���beacon֡ */
    switch (mac_get_frame_type_and_subtype(mac_hdr)) {
        case WLAN_FC0_SUBTYPE_BEACON | WLAN_FC0_TYPE_MGT: {
            ret = hmac_sta_join_rx_beacon(hmac_sta, mac_hdr, rx_len);
            if (ret != OAL_SUCC) {
                return ret;
            }
        }
            break;

        case WLAN_FC0_SUBTYPE_ACTION | WLAN_FC0_TYPE_MGT:
            break;

        default:
            break;
    }

    return OAL_SUCC;
}


uint32_t hmac_sta_wait_join_timeout(hmac_vap_stru *pst_hmac_sta, void *pst_msg)
{
    hmac_join_rsp_stru st_join_rsp = { 0 };

    if (oal_any_null_ptr2(pst_hmac_sta, pst_msg)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_wait_join_timeout::hmac_sta or msg is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oam_error_log0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                   "{hmac_sta_wait_join_timeout::join timeout.}");
    /* ����timeout��������ʾjoinû�гɹ�����join�Ľ������Ϊtimeout�ϱ���sme */
    st_join_rsp.en_result_code = HMAC_MGMT_TIMEOUT;

    /* ��hmac״̬���л�ΪMAC_VAP_STATE_STA_FAKE_UP */
    hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_FAKE_UP);

    /* ���ͳ�ʱ��Ϣ��SME */
    hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP, (uint8_t *)&st_join_rsp);

    return OAL_SUCC;
}


uint32_t hmac_sta_wait_join_misc(hmac_vap_stru *pst_hmac_sta, void *pst_msg)
{
    hmac_join_rsp_stru st_join_rsp;
    hmac_misc_input_stru *st_misc_input = (hmac_misc_input_stru *)pst_msg;

    if (oal_any_null_ptr2(pst_hmac_sta, pst_msg)) {
        oam_error_log0(0, OAM_SF_SCAN, "{hmac_sta_wait_join_misc::hmac_sta or msg is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oam_info_log0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_sta_wait_join_misc::enter func.}");
    switch (st_misc_input->en_type) {
        /* ����TBTT�ж�  */
        case HMAC_MISC_TBTT: {
            /* ���յ�TBTT�жϣ���ζ��JOIN�ɹ��ˣ�����ȡ��JOIN��ʼʱ���õĶ�ʱ��,����Ϣ֪ͨSME  */
            frw_timer_immediate_destroy_timer_m(&pst_hmac_sta->st_mgmt_timer);

            st_join_rsp.en_result_code = HMAC_MGMT_SUCCESS;

            oam_info_log0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                          "{hmac_sta_wait_join_misc::join succ.}");
            /* �л�STA״̬��JOIN_COMP */
            hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_JOIN_COMP);

            /* ����JOIN�ɹ���Ϣ��SME */
            hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP, (uint8_t *)&st_join_rsp);
        }
        break;

        default:
        {
            /* Do Nothing */
        }
        break;
    }
    return OAL_SUCC;
}


uint32_t hmac_sta_assemble_auth_seq1(hmac_vap_stru *hmac_sta, hmac_auth_req_stru *auth_req, oal_netbuf_stru *auth_frame)
{
    hmac_user_stru *hmac_user_ap = NULL;
    mac_tx_ctl_stru *tx_ctl = NULL;
    uint16_t auth_len;
    uint32_t ret;

    /* ��seq = 1 ����֤����֡ */
    auth_len = hmac_mgmt_encap_auth_req(hmac_sta, (uint8_t *)(oal_netbuf_header(auth_frame)));

    oal_netbuf_put(auth_frame, auth_len);
    hmac_user_ap = mac_res_get_hmac_user(hmac_sta->st_vap_base_info.us_assoc_vap_id);
    if (hmac_user_ap == NULL) {
        oal_netbuf_free(auth_frame);
        oam_error_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
            "{hmac_sta_assemble_auth_frame::pst_hmac_user[%d] null.}", hmac_sta->st_vap_base_info.us_assoc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* Ϊ��д����������׼������ */
    tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(auth_frame); /* ��ȡcb�ṹ�� */
    MAC_GET_CB_MPDU_LEN(tx_ctl) = auth_len;                 /* dmac������Ҫ��mpdu���� */
    /* ���������Ҫ��ȡuser�ṹ�� */
    MAC_GET_CB_TX_USER_IDX(tx_ctl) = hmac_user_ap->st_user_base_info.us_assoc_id;

    /* �����WEP����Ҫ��ap��mac��ַд��lut */
    ret = hmac_init_security(&(hmac_sta->st_vap_base_info), hmac_user_ap->st_user_base_info.auc_user_mac_addr,
        sizeof(hmac_user_ap->st_user_base_info.auc_user_mac_addr));
    if (ret != OAL_SUCC) {
        oam_error_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_assemble_auth_frame::hmac_init_security failed[%d].}", ret);
    }

    /* ���¼���dmac����֡���� */
    ret = hmac_tx_mgmt_send_event(&hmac_sta->st_vap_base_info, auth_frame, auth_len);
    if (ret != OAL_SUCC) {
        oal_netbuf_free(auth_frame);
        oam_warning_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_assemble_auth_frame::hmac_tx_mgmt_send_event failed[%d].}", ret);
        return ret;
    }

    /* ����״̬ */
    hmac_fsm_change_state(hmac_sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2);

    /* ������֤��ʱ��ʱ�� */
    hmac_sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2;
    hmac_sta->st_mgmt_timetout_param.us_user_index = hmac_user_ap->st_user_base_info.us_assoc_id;
    hmac_sta->st_mgmt_timetout_param.uc_vap_id = hmac_sta->st_vap_base_info.uc_vap_id;
    frw_timer_create_timer_m(&hmac_sta->st_mgmt_timer, hmac_mgmt_timeout_sta, auth_req->us_timeout,
        &hmac_sta->st_mgmt_timetout_param, OAL_FALSE, OAM_MODULE_ID_HMAC, hmac_sta->st_vap_base_info.core_id);

    return OAL_SUCC;
}


uint32_t hmac_sta_wait_auth(hmac_vap_stru *hmac_sta, void *msg)
{
    hmac_auth_req_stru *auth_req = NULL;
    oal_netbuf_stru *auth_frame = NULL;
    uint32_t ret;

    if (oal_any_null_ptr2(hmac_sta, msg)) {
        oam_error_log0(0, OAM_SF_AUTH, "{hmac_sta_wait_auth::hmac_sta or msg is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    auth_req = (hmac_auth_req_stru *)msg;

#ifdef _PRE_WLAN_FEATURE_SAE
    if ((mac_mib_get_AuthenticationMode(&(hmac_sta->st_vap_base_info)) == WLAN_WITP_AUTH_SAE ||
        mac_mib_get_AuthenticationMode(&(hmac_sta->st_vap_base_info)) == WLAN_WITP_AUTH_TBPEKE)
        && (hmac_sta->bit_sae_connect_with_pmkid == OAL_FALSE)) {
        ret = hmac_sta_sae_connect_info_wpas(hmac_sta, auth_req);
        return ret;
    }
#endif

    /* ������֤֡�ռ� */
    auth_frame = oal_mem_netbuf_alloc(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (auth_frame == NULL) {
        oam_error_log0(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::puc_auth_frame null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oal_mem_netbuf_trace(auth_frame, OAL_TRUE);

    memset_s(oal_netbuf_cb(auth_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    memset_s((uint8_t *)oal_netbuf_header(auth_frame), MAC_80211_FRAME_LEN, 0, MAC_80211_FRAME_LEN);

    /* ��seq = 1 ����֤����֡ */
    ret = hmac_sta_assemble_auth_seq1(hmac_sta, auth_req, auth_frame);
    if (ret != OAL_SUCC) {
        return ret;
    }

    return OAL_SUCC;
}


uint32_t hmac_sta_wait_auth_seq2_rx(hmac_vap_stru *pst_sta, void *pst_msg)
{
    dmac_wlan_crx_event_stru *pst_crx_event = NULL;
    mac_rx_ctl_stru *pst_rx_ctrl = NULL; /* ÿһ��MPDU�Ŀ�����Ϣ */
    uint8_t *puc_mac_hdr = NULL;
    uint16_t us_auth_alg;
    hmac_auth_rsp_stru st_auth_rsp = {
        { 0 },
    };

    if (oal_any_null_ptr2(pst_sta, pst_msg)) {
        oam_error_log0(0, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq2_rx::sta or msg is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_crx_event = (dmac_wlan_crx_event_stru *)pst_msg;
    pst_rx_ctrl = (mac_rx_ctl_stru *)oal_netbuf_cb(pst_crx_event->pst_netbuf); /* ÿһ��MPDU�Ŀ�����Ϣ */
    puc_mac_hdr = (uint8_t *)MAC_GET_RX_CB_MAC_HEADER_ADDR(pst_rx_ctrl);

#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if (OAL_TRUE == wlan_pm_wkup_src_debug_get()) {
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
        oam_warning_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX,
                         "{wifi_wake_src:hmac_sta_wait_auth_seq2_rx::wakeup mgmt type[0x%x]}",
                         mac_get_frame_type_and_subtype(puc_mac_hdr));
    }
#endif

    if ((WLAN_FC0_SUBTYPE_AUTH | WLAN_FC0_TYPE_MGT) != mac_get_frame_type_and_subtype(puc_mac_hdr)) {
        return OAL_SUCC;
    }

    us_auth_alg = mac_get_auth_alg(puc_mac_hdr);
#ifdef _PRE_WLAN_FEATURE_SAE
    /* ע��:mib ֵ����д��auth_alg ֵ�����ںˣ���ieee�����auth_algȡֵ��ͬ */
    if ((us_auth_alg == WLAN_MIB_AUTH_ALG_SAE || us_auth_alg == WLAN_MIB_AUTH_ALG_TBPEKE) &&
        (pst_sta->bit_sae_connect_with_pmkid == OAL_FALSE)) {
        return hmac_sta_process_sae_commit(pst_sta, pst_crx_event->pst_netbuf);
    }
#endif

    if (WLAN_AUTH_TRASACTION_NUM_TWO != mac_get_auth_seq_num(puc_mac_hdr)) {
        return OAL_SUCC;
    }

#ifdef _PRE_WLAN_FEATURE_SNIFFER
#ifdef CONFIG_HW_SNIFFER
    proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_ctrl->us_frame_len, 0);
#endif
#endif
    /* AUTH alg CHECK */
    if ((mac_mib_get_AuthenticationMode(&pst_sta->st_vap_base_info) != us_auth_alg)
        && (WLAN_WITP_AUTH_AUTOMATIC != mac_mib_get_AuthenticationMode(&pst_sta->st_vap_base_info))) {
        oam_warning_log2(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq2_rx::rcv unexpected auth alg[%d/%d].}",
                         us_auth_alg, mac_mib_get_AuthenticationMode(&pst_sta->st_vap_base_info));
    }
    st_auth_rsp.us_status_code = mac_get_auth_status(puc_mac_hdr);
    if (st_auth_rsp.us_status_code != MAC_SUCCESSFUL_STATUSCODE) {
        oam_warning_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
            "{hmac_sta_wait_auth_seq2_rx::AP refuse STA auth reason[%d]!}", st_auth_rsp.us_status_code);

        frw_timer_immediate_destroy_timer_m(&pst_sta->st_mgmt_timer);

        /* �ϱ���SME��֤��� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (uint8_t *)&st_auth_rsp);

        if (st_auth_rsp.us_status_code != MAC_AP_FULL) {
            chr_exception(chr_wifi_drv(CHR_WIFI_DRV_EVENT_CONNECT, CHR_WIFI_DRV_ERROR_AUTH_REJECTED));
        }
        return OAL_SUCC;
    }

    /* auth response status_code �ɹ����� */
    if (us_auth_alg == WLAN_WITP_AUTH_OPEN_SYSTEM) {
        frw_timer_immediate_destroy_timer_m(&pst_sta->st_mgmt_timer);

        /* ��״̬����ΪAUTH_COMP */
        hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);
        st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* �ϱ���SME��֤�ɹ� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (uint8_t *)&st_auth_rsp);
        return OAL_SUCC;
    } else if (us_auth_alg == WLAN_WITP_AUTH_SHARED_KEY) {
        return hmac_sta_send_auth_seq3(pst_sta, puc_mac_hdr);
    } else {
        frw_timer_immediate_destroy_timer_m(&pst_sta->st_mgmt_timer);

        /* ���յ�AP �ظ���auth response ��֧����֤�㷨��ǰ��֧�ֵ�����£�status code ȴ��SUCC,
            ��Ϊ��֤�ɹ������Ҽ����������� */
        oam_warning_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq2_rx::AP's auth_alg [%d] not support!}", us_auth_alg);

        /* ��״̬����ΪAUTH_COMP */
        hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);
        st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* �ϱ���SME��֤�ɹ� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (uint8_t *)&st_auth_rsp);
    }

    return OAL_SUCC;
}


uint32_t hmac_sta_wait_auth_seq4_rx(hmac_vap_stru *pst_sta, void *p_msg)
{
    dmac_wlan_crx_event_stru *pst_crx_event = NULL;
    mac_rx_ctl_stru *pst_rx_ctrl = NULL; /* ÿһ��MPDU�Ŀ�����Ϣ */
    uint8_t *puc_mac_hdr = NULL;
    hmac_auth_rsp_stru st_auth_rsp = { { 0 },
    };

    if (oal_any_null_ptr2(pst_sta, p_msg)) {
        oam_error_log0(0, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq4_rx::sta or msg is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_crx_event = (dmac_wlan_crx_event_stru *)p_msg;
    pst_rx_ctrl = (mac_rx_ctl_stru *)oal_netbuf_cb(pst_crx_event->pst_netbuf); /* ÿһ��MPDU�Ŀ�����Ϣ */
    puc_mac_hdr = (uint8_t *)MAC_GET_RX_CB_MAC_HEADER_ADDR(pst_rx_ctrl);

#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if (OAL_TRUE == wlan_pm_wkup_src_debug_get()) {
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
        oam_warning_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX,
                         "{wifi_wake_src:hmac_sta_wait_auth_seq4_rx::wakeup mgmt type[0x%x]}",
                         mac_get_frame_type_and_subtype(puc_mac_hdr));
    }
#endif

    if ((WLAN_FC0_SUBTYPE_AUTH | WLAN_FC0_TYPE_MGT) != mac_get_frame_type_and_subtype(puc_mac_hdr)) {
        return OAL_SUCC;
    }

#ifdef _PRE_WLAN_FEATURE_SAE
    /* ע��:mib ֵ����д��auth_alg ֵ�����ںˣ���ieee�����auth_algȡֵ��ͬ */
    if ((mac_get_auth_alg(puc_mac_hdr) == WLAN_MIB_AUTH_ALG_SAE ||
        mac_get_auth_alg(puc_mac_hdr) == WLAN_MIB_AUTH_ALG_TBPEKE)
        && (pst_sta->bit_sae_connect_with_pmkid == OAL_FALSE)) {
        return hmac_sta_process_sae_confirm(pst_sta, pst_crx_event->pst_netbuf);
    }
#endif

#ifdef _PRE_WLAN_FEATURE_SNIFFER
#ifdef CONFIG_HW_SNIFFER
        proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_ctrl->us_frame_len, 0);
#endif
#endif
        if ((WLAN_AUTH_TRASACTION_NUM_FOUR == mac_get_auth_seq_num(puc_mac_hdr)) &&
            (MAC_SUCCESSFUL_STATUSCODE == mac_get_auth_status(puc_mac_hdr))) {
            /* ���յ�seq = 4 ��״̬λΪsucc ȡ����ʱ�� */
            frw_timer_immediate_destroy_timer_m(&pst_sta->st_mgmt_timer);

            st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

            /* ����sta״̬ΪMAC_VAP_STATE_STA_AUTH_COMP */
            hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);

            /* ����֤����ϱ�SME */
            hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (uint8_t *)&st_auth_rsp);
        } else {
            oam_warning_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                "{hmac_sta_wait_auth_seq4_rx::transaction num.status[%d]}", mac_get_auth_status(puc_mac_hdr));
            /* �ȴ���ʱ����ʱ */
        }

    return OAL_SUCC;
}


uint32_t hmac_sta_wait_asoc(hmac_vap_stru *pst_sta, void *pst_msg)
{
    hmac_asoc_req_stru *pst_hmac_asoc_req = NULL;
    oal_netbuf_stru *pst_asoc_req_frame = NULL;
    mac_tx_ctl_stru *pst_tx_ctl = NULL;
    hmac_user_stru *pst_hmac_user_ap = NULL;
    uint32_t asoc_frame_len;
    uint32_t ret;
    uint8_t *puc_curr_bssid = NULL;

    if (oal_any_null_ptr2(pst_sta, pst_msg)) {
        oam_error_log0(0, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::sta or msg is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_asoc_req = (hmac_asoc_req_stru *)pst_msg;

    pst_asoc_req_frame = oal_mem_netbuf_alloc(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (pst_asoc_req_frame == NULL) {
        oam_error_log0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_asoc::pst_asoc_req_frame null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    oal_mem_netbuf_trace(pst_asoc_req_frame, OAL_TRUE);

    memset_s(oal_netbuf_cb(pst_asoc_req_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    /* ��mac header���� */
    memset_s((uint8_t *)oal_netbuf_header(pst_asoc_req_frame), MAC_80211_FRAME_LEN, 0, MAC_80211_FRAME_LEN);

    /* ��֡ (Re)Assoc_req_Frame */
    if (pst_sta->bit_reassoc_flag) {
        puc_curr_bssid = pst_sta->st_vap_base_info.auc_bssid;
    } else {
        puc_curr_bssid = NULL;
    }
    asoc_frame_len = hmac_mgmt_encap_asoc_req_sta(pst_sta, (uint8_t *)(oal_netbuf_header(pst_asoc_req_frame)),
        puc_curr_bssid);
    oal_netbuf_put(pst_asoc_req_frame, asoc_frame_len);

    if (asoc_frame_len == 0) {
        oam_warning_log0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_sta_wait_asoc::hmac_mgmt_encap_asoc_req_sta null.}");
        oal_netbuf_free(pst_asoc_req_frame);

        return OAL_FAIL;
    }

    if (oal_unlikely(asoc_frame_len < OAL_ASSOC_REQ_IE_OFFSET)) {
        oam_error_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_asoc::invalid asoc_req_ie_len[%u].}",
                       asoc_frame_len);
        oam_report_dft_params(BROADCAST_MACADDR, (uint8_t *)oal_netbuf_header(pst_asoc_req_frame),
            (uint16_t)asoc_frame_len, OAM_OTA_TYPE_80211_FRAME);
        oal_netbuf_free(pst_asoc_req_frame);
        return OAL_FAIL;
    }
    /* Should we change the ie buff from local mem to netbuf ? */
    /* �˴�������ڴ棬ֻ���ϱ����ں˺��ͷ� */
    pst_hmac_user_ap = (hmac_user_stru *)mac_res_get_hmac_user(pst_sta->st_vap_base_info.us_assoc_vap_id);
    if (pst_hmac_user_ap == NULL) {
        oam_error_log0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_asoc::pst_hmac_user_ap null.}");
        oal_netbuf_free(pst_asoc_req_frame);
        return OAL_ERR_CODE_PTR_NULL;
    }

    hmac_user_free_asoc_req_ie(pst_sta->st_vap_base_info.us_assoc_vap_id);
    ret = hmac_user_set_asoc_req_ie(pst_hmac_user_ap,
                                    oal_netbuf_header(pst_asoc_req_frame) + OAL_ASSOC_REQ_IE_OFFSET,
                                    asoc_frame_len - OAL_ASSOC_REQ_IE_OFFSET,
                                    (uint8_t)(pst_sta->bit_reassoc_flag));
    if (ret != OAL_SUCC) {
        oam_error_log0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_asoc::hmac_user_set_asoc_req_ie failed}");
        oal_netbuf_free(pst_asoc_req_frame);
        return OAL_FAIL;
    }

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_asoc_req_frame);

    MAC_GET_CB_MPDU_LEN(pst_tx_ctl) = (uint16_t)asoc_frame_len;
    MAC_GET_CB_TX_USER_IDX(pst_tx_ctl) = pst_hmac_user_ap->st_user_base_info.us_assoc_id;

    /* ���¼���DMAC����֡���� */
    ret = hmac_tx_mgmt_send_event(&(pst_sta->st_vap_base_info), pst_asoc_req_frame, (uint16_t)asoc_frame_len);
    if (ret != OAL_SUCC) {
        oal_netbuf_free(pst_asoc_req_frame);
        hmac_user_free_asoc_req_ie(pst_sta->st_vap_base_info.us_assoc_vap_id);
        oam_warning_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_sta_wait_asoc::hmac_tx_mgmt_send_event failed[%d].}", ret);
        return ret;
    }

    /* ����״̬ */
    hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_WAIT_ASOC);

    /* ����������ʱ��ʱ��, Ϊ�Զ�ap����һ����ʱ���������ʱapû��asoc rsp��������ʱ���� */
    pst_sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_ASOC;
    pst_sta->st_mgmt_timetout_param.us_user_index = pst_hmac_user_ap->st_user_base_info.us_assoc_id;
    pst_sta->st_mgmt_timetout_param.uc_vap_id = pst_sta->st_vap_base_info.uc_vap_id;
    pst_sta->st_mgmt_timetout_param.en_status_code = MAC_ASOC_RSP_TIMEOUT;

    frw_timer_create_timer_m(&(pst_sta->st_mgmt_timer), hmac_mgmt_timeout_sta, pst_hmac_asoc_req->us_assoc_timeout,
        &(pst_sta->st_mgmt_timetout_param), OAL_FALSE, OAM_MODULE_ID_HMAC, pst_sta->st_vap_base_info.core_id);

    return OAL_SUCC;
}



void hmac_p2p_listen_comp_cb(void *p_arg)
{
    hmac_vap_stru *pst_hmac_vap = NULL;
    mac_device_stru *pst_mac_device = NULL;
    hmac_scan_record_stru *pst_scan_record;

    pst_scan_record = (hmac_scan_record_stru *)p_arg;

    /* �ж�listen���ʱ��״̬ */
    if ((pst_scan_record->en_scan_rsp_status != MAC_SCAN_SUCCESS) &&
        (!hmac_get_feature_switch(HMAC_MIRACAST_REDUCE_LOG_SWITCH))) {
        oam_warning_log1(0, OAM_SF_P2P, "{hmac_p2p_listen_comp_cb::listen failed, listen rsp status: %d.}",
                         pst_scan_record->en_scan_rsp_status);
    }

    pst_hmac_vap = mac_res_get_hmac_vap(pst_scan_record->uc_vap_id);
    if (pst_hmac_vap == NULL) {
        oam_error_log1(0, OAM_SF_P2P, "{hmac_p2p_listen_comp_cb::pst_hmac_vap is null:vap_id %d.}",
                       pst_scan_record->uc_vap_id);
        return;
    }

    pst_mac_device = mac_res_get_dev(pst_scan_record->uc_device_id);
    if (pst_mac_device == NULL) {
        oam_error_log1(0, OAM_SF_P2P, "{hmac_p2p_listen_comp_cb::pst_mac_device is null:vap_id %d.}",
                       pst_scan_record->uc_device_id);
        return;
    }

    
    if (pst_scan_record->ull_cookie == pst_mac_device->st_p2p_info.ull_last_roc_id) {
        /* ״̬������: hmac_p2p_listen_timeout */
        if (hmac_fsm_call_func_sta(pst_hmac_vap, HMAC_FSM_INPUT_LISTEN_TIMEOUT, &(pst_hmac_vap->st_vap_base_info)) !=
            OAL_SUCC) {
            oam_warning_log0(0, OAM_SF_P2P, "{hmac_p2p_listen_comp_cb::hmac_fsm_call_func_sta fail.}");
        }
    } else {
        oam_warning_log3(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
            "{hmac_p2p_listen_comp_cb::ignore listen complete.scan_report_cookie[%x], \
            current_listen_cookie[%x], ull_last_roc_id[%x].}",
            pst_scan_record->ull_cookie,
            pst_mac_device->st_scan_params.ull_cookie,
            pst_mac_device->st_p2p_info.ull_last_roc_id);
    }

    return;
}

void hmac_mgmt_tx_roc_comp_cb(void *p_arg)
{
    hmac_vap_stru *pst_hmac_vap = NULL;
    mac_device_stru *pst_mac_device = NULL;
    hmac_scan_record_stru *pst_scan_record;

    pst_scan_record = (hmac_scan_record_stru *)p_arg;

    /* �ж�listen���ʱ��״̬ */
    if (pst_scan_record->en_scan_rsp_status != MAC_SCAN_SUCCESS) {
        oam_warning_log1(0, OAM_SF_P2P, "{hmac_mgmt_tx_roc_comp_cb::listen failed, listen rsp status: %d.}",
                         pst_scan_record->en_scan_rsp_status);
    }

    pst_hmac_vap = mac_res_get_hmac_vap(pst_scan_record->uc_vap_id);
    if (pst_hmac_vap == NULL) {
        oam_error_log1(0, OAM_SF_P2P, "{hmac_mgmt_tx_roc_comp_cb::pst_hmac_vap is null:vap_id %d.}",
                       pst_scan_record->uc_vap_id);
        return;
    }

    pst_mac_device = mac_res_get_dev(pst_scan_record->uc_device_id);
    if (pst_mac_device == NULL) {
        oam_error_log1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
                       "{hmac_mgmt_tx_roc_comp_cb::pst_mac_device is null:vap_id %d.}",
                       pst_scan_record->uc_device_id);
        return;
    }

    /* ����P2P0 ��P2P_CL ����vap �ṹ�壬������ʱ�����ؼ���ǰ�����״̬ */
    if (pst_hmac_vap->st_vap_base_info.en_vap_state >= MAC_VAP_STATE_STA_JOIN_COMP &&
        pst_hmac_vap->st_vap_base_info.en_vap_state <= MAC_VAP_STATE_STA_WAIT_ASOC) {
        oam_warning_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
                         "{hmac_mgmt_tx_roc_comp_cb::vap is connecting, can not change vap state.}");
    } else if (pst_hmac_vap->st_vap_base_info.en_vap_state == MAC_VAP_STATE_STA_FAKE_UP) {
        oam_warning_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
                         "{hmac_mgmt_tx_roc_comp_cb::vap is fake up, can not change vap state.}");
    } else if (pst_hmac_vap->st_vap_base_info.en_p2p_mode != WLAN_LEGACY_VAP_MODE) {
        mac_vap_state_change(&pst_hmac_vap->st_vap_base_info, pst_mac_device->st_p2p_info.en_last_vap_state);
    } else {
        mac_vap_state_change(&pst_hmac_vap->st_vap_base_info, pst_hmac_vap->st_vap_base_info.en_last_vap_state);
    }
    oam_warning_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P, "{hmac_mgmt_tx_roc_comp_cb}");
}


OAL_STATIC void hmac_cfg80211_prepare_listen_req_param(mac_scan_req_stru *pst_scan_params, int8_t *puc_param)
{
    hmac_remain_on_channel_param_stru *pst_remain_on_channel;
    mac_channel_stru *pst_channel_tmp = NULL;

    pst_remain_on_channel = (hmac_remain_on_channel_param_stru *)puc_param;

    /* ���ü����ŵ���Ϣ��ɨ������� */
    pst_scan_params->ast_channel_list[0].en_band = pst_remain_on_channel->en_band;
    pst_scan_params->ast_channel_list[0].en_bandwidth = pst_remain_on_channel->en_listen_channel_type;
    pst_scan_params->ast_channel_list[0].uc_chan_number = pst_remain_on_channel->uc_listen_channel;
    pst_scan_params->ast_channel_list[0].uc_chan_idx = 0;
    pst_channel_tmp = &(pst_scan_params->ast_channel_list[0]);
    if (mac_get_channel_idx_from_num(pst_channel_tmp->en_band, pst_channel_tmp->uc_chan_number,
        pst_channel_tmp->ext6g_band, &(pst_channel_tmp->uc_chan_idx)) != OAL_SUCC) {
        oam_warning_log2(0, OAM_SF_P2P,
            "{hmac_cfg80211_prepare_listen_req_param::mac_get_channel_idx_from_num fail.band[%u] channel[%u]}",
            pst_channel_tmp->en_band, pst_channel_tmp->uc_chan_number);
    }

    /* ���������������� */
    pst_scan_params->uc_max_scan_count_per_channel = 1;
    pst_scan_params->uc_channel_nums = 1;
    pst_scan_params->uc_scan_func = MAC_SCAN_FUNC_P2P_LISTEN;
    pst_scan_params->us_scan_time = (uint16_t)pst_remain_on_channel->listen_duration;
    if (pst_remain_on_channel->en_roc_type == IEEE80211_ROC_TYPE_MGMT_TX) {
        pst_scan_params->p_fn_cb = hmac_mgmt_tx_roc_comp_cb;
        if (hmac_get_feature_switch(HMAC_MIRACAST_SINK_SWITCH)) {
            pst_scan_params->bit_roc_type_tx_mgmt = OAL_TRUE;
        }
    } else {
        pst_scan_params->p_fn_cb = hmac_p2p_listen_comp_cb;
        if (hmac_get_feature_switch(HMAC_MIRACAST_SINK_SWITCH)) {
            pst_scan_params->bit_roc_type_tx_mgmt = OAL_FALSE;
        }
    }
    pst_scan_params->en_scan_mode = WLAN_SCAN_MODE_MGMT_TX;
    pst_scan_params->ull_cookie = pst_remain_on_channel->ull_cookie;
    pst_scan_params->bit_is_p2p0_scan = OAL_TRUE;
    pst_scan_params->uc_p2p0_listen_channel = pst_remain_on_channel->uc_listen_channel;

    return;
}


uint32_t hmac_p2p_listen_timeout(hmac_vap_stru *pst_hmac_vap_sta, void *p_param)
{
    mac_device_stru *pst_mac_device = NULL;
    hmac_vap_stru *pst_hmac_vap;
    mac_vap_stru *pst_mac_vap;
    hmac_device_stru *pst_hmac_device = NULL;
    hmac_scan_record_stru *pst_scan_record = NULL;

    pst_mac_vap = (mac_vap_stru *)p_param;
    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (pst_hmac_vap == NULL) {
        oam_error_log1(0, OAM_SF_P2P, "{hmac_p2p_listen_timeout::mac_res_get_hmac_vap fail.vap_id[%u]!}",
                       pst_mac_vap->uc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (pst_mac_device == NULL) {
        oam_warning_log1(0, OAM_SF_P2P, "{hmac_p2p_listen_timeout::mac_res_get_dev fail.device_id[%u]!}",
                         pst_mac_vap->uc_device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡhmac device */
    pst_hmac_device = hmac_res_get_mac_dev(pst_mac_device->uc_device_id);
    if (oal_unlikely(pst_hmac_device == NULL)) {
        oam_error_log0(0, OAM_SF_P2P, "{hmac_p2p_listen_timeout::pst_hmac_device null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oam_info_log2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
                  "{hmac_p2p_listen_timeout::current pst_mac_vap channel is [%d] state[%d]}",
                  pst_mac_vap->st_channel.uc_chan_number,
                  pst_hmac_vap->st_vap_base_info.en_vap_state);

    oam_info_log2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
                  "{hmac_p2p_listen_timeout::next pst_mac_vap channel is [%d] state[%d]}",
                  pst_mac_vap->st_channel.uc_chan_number,
                  pst_mac_device->st_p2p_info.en_last_vap_state);

    /* ����P2P0 ��P2P_CL ����vap �ṹ�壬������ʱ�����ؼ���ǰ�����״̬ */
    if (pst_hmac_vap->st_vap_base_info.en_p2p_mode != WLAN_LEGACY_VAP_MODE) {
        mac_vap_state_change(&pst_hmac_vap->st_vap_base_info, pst_mac_device->st_p2p_info.en_last_vap_state);
    } else {
        mac_vap_state_change(&pst_hmac_vap->st_vap_base_info, pst_hmac_vap->st_vap_base_info.en_last_vap_state);
    }

    pst_scan_record = &(pst_hmac_device->st_scan_mgmt.st_scan_record_mgmt);
    if (pst_scan_record->ull_cookie == pst_mac_device->st_p2p_info.ull_last_roc_id) {
        /* 3.1 ���¼���WAL ���ϱ��������� */
        hmac_p2p_send_listen_expired_to_host(pst_hmac_vap);
    }

    /* 3.2 ���¼���DMAC �����ؼ����ŵ� */
    hmac_p2p_send_listen_expired_to_device(pst_hmac_vap);

    return OAL_SUCC;
}


uint32_t hmac_p2p_remain_on_channel(hmac_vap_stru *pst_hmac_vap_sta, void *p_param)
{
    mac_device_stru *pst_mac_device = NULL;
    mac_vap_stru *pst_mac_vap;
    hmac_remain_on_channel_param_stru *pst_remain_on_channel;
    mac_scan_req_h2d_stru st_scan_h2d_params;
    uint32_t ret;

    pst_remain_on_channel = (hmac_remain_on_channel_param_stru *)p_param;

    pst_mac_vap = mac_res_get_mac_vap(pst_hmac_vap_sta->st_vap_base_info.uc_vap_id);
    if (pst_mac_vap == NULL) {
        oam_error_log1(0, OAM_SF_P2P, "{hmac_p2p_remain_on_channel::mac_res_get_mac_vap fail.vap_id[%u]!}",
                       pst_hmac_vap_sta->st_vap_base_info.uc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (oal_unlikely(pst_mac_device == NULL)) {
        oam_error_log1(0, OAM_SF_ANY,
            "{hmac_p2p_listen_timeout::pst_mac_device[%d](%d) null!}", pst_mac_vap->uc_device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    
    if (pst_hmac_vap_sta->st_vap_base_info.en_vap_state == MAC_VAP_STATE_STA_LISTEN) {
        hmac_p2p_send_listen_expired_to_host(pst_hmac_vap_sta);
        oam_warning_log1(pst_mac_vap->uc_vap_id, OAM_SF_P2P,
            "{hmac_p2p_remain_on_channel::listen nested, send remain on channel expired to host!curr_state[%d]}",
            pst_hmac_vap_sta->st_vap_base_info.en_vap_state);
    }

    /* �޸�P2P_DEVICE ״̬Ϊ����״̬ */
    
    mac_vap_state_change((mac_vap_stru *)&pst_hmac_vap_sta->st_vap_base_info, MAC_VAP_STATE_STA_LISTEN);
    oam_info_log4(pst_mac_vap->uc_vap_id, OAM_SF_P2P,
        "{hmac_p2p_remain_on_channel: get in listen state!last_state %d, channel %d, duration %d, curr_state %d}",
        pst_mac_device->st_p2p_info.en_last_vap_state,
        pst_remain_on_channel->uc_listen_channel,
        pst_remain_on_channel->listen_duration,
        pst_hmac_vap_sta->st_vap_base_info.en_vap_state);

    /* ׼���������� */
    memset_s(&st_scan_h2d_params, sizeof(mac_scan_req_h2d_stru), 0, sizeof(mac_scan_req_h2d_stru));
    hmac_cfg80211_prepare_listen_req_param(&(st_scan_h2d_params.st_scan_params), (int8_t *)pst_remain_on_channel);

    /* ����ɨ����ڣ�׼�����м������������ܼ�������ִ�гɹ���ʧ�ܣ������ؼ����ɹ� */
    /* ״̬������: hmac_scan_proc_scan_req_event */
    ret = hmac_fsm_call_func_sta(pst_hmac_vap_sta, HMAC_FSM_INPUT_SCAN_REQ, (void *)(&st_scan_h2d_params));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SCAN,
            "{hmac_p2p_remain_on_channel::hmac_fsm_call_func_sta fail[%d].}", ret);
        if (hmac_get_feature_switch(HMAC_MIRACAST_SINK_SWITCH)) {
            /* ������Ҫ�������������·�����ʧ�ܺ���֪ͨwpas�ָ�P2P״̬ΪIDLE���Ա�������ٴν�����������  */
            return ret;
        }
    }

    return OAL_SUCC;
}
