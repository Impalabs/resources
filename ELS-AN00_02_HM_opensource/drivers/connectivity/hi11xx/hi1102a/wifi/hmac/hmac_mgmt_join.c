

/* 1 ͷ�ļ����� */
#include "hmac_mgmt_join.h"
#include "hmac_fsm.h"
#include "hmac_204080_coexist.h"
#include "hmac_encap_frame_sta.h"
#include "hmac_p2p.h"
#include "hmac_sme_sta.h"
#include "hmac_rx_data.h"
#include "hmac_tx_amsdu.h"
#include "hmac_11i.h"
#include "hmac_sae.h"
#include "plat_pm_wlan.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_MGMT_JOIN_C

#define mac_addr(_puc_mac) ((oal_uint32)(((oal_uint32)(_puc_mac)[2] << 24) |  \
                                         ((oal_uint32)(_puc_mac)[3] << 16) |  \
                                         ((oal_uint32)(_puc_mac)[4] << 8) |  \
                                         ((oal_uint32)(_puc_mac)[5])))

/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/

oal_uint32 hmac_mgmt_timeout_sta(oal_void *p_arg)
{
    hmac_vap_stru *pst_hmac_vap = OAL_PTR_NULL;
    hmac_mgmt_timeout_param_stru *pst_timeout_param;

    pst_timeout_param = (hmac_mgmt_timeout_param_stru *)p_arg;
    if (pst_timeout_param == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_timeout_param->uc_vap_id);
    if (pst_hmac_vap == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_fsm_call_func_sta(pst_hmac_vap, HMAC_FSM_INPUT_TIMER0_OUT, pst_timeout_param);
}

oal_void hmac_update_join_req_params_prot_sta(hmac_vap_stru *pst_hmac_vap,
                                              hmac_join_req_stru *pst_join_req)
{
    if (pst_hmac_vap->st_vap_base_info.pst_mib_info->st_wlan_mib_sta_config.en_dot11DesiredBSSType ==
        WLAN_MIB_DESIRED_BSSTYPE_INFRA) {
        pst_hmac_vap->uc_wmm_cap = pst_join_req->st_bss_dscr.uc_wmm_cap;
        mac_vap_set_uapsd_cap(&pst_hmac_vap->st_vap_base_info, pst_join_req->st_bss_dscr.uc_uapsd_cap);
    }

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    hmac_update_join_req_params_2040(&(pst_hmac_vap->st_vap_base_info), &(pst_join_req->st_bss_dscr));
#endif
}


oal_bool_enum_uint8 hmac_is_rate_support(oal_uint8 *puc_rates, oal_uint8 uc_rate_num, oal_uint8 uc_rate)
{
    oal_bool_enum_uint8 en_rate_is_supp = OAL_FALSE;
    oal_uint8 uc_loop;

    if (puc_rates == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_is_rate_support::puc_rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    for (uc_loop = 0; uc_loop < uc_rate_num; uc_loop++) {
        if ((puc_rates[uc_loop] & 0x7F) == uc_rate) {
            en_rate_is_supp = OAL_TRUE;
            break;
        }
    }

    return en_rate_is_supp;
}


oal_bool_enum_uint8 hmac_is_support_11grate(oal_uint8 *puc_rates, oal_uint8 uc_rate_num)
{
    if (puc_rates == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_is_rate_support::puc_rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((hmac_is_rate_support(puc_rates, uc_rate_num, 0x0C) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x12) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x18) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x24) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x30) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x48) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x60) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x6C) == OAL_TRUE)) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}


oal_bool_enum_uint8 hmac_is_support_11brate(oal_uint8 *puc_rates, oal_uint8 uc_rate_num)
{
    if (puc_rates == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_is_support_11brate::puc_rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((hmac_is_rate_support(puc_rates, uc_rate_num, 0x02) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x04) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x0B) == OAL_TRUE) ||
        (hmac_is_rate_support(puc_rates, uc_rate_num, 0x16) == OAL_TRUE)) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}


oal_uint32 hmac_sta_get_user_protocol(mac_bss_dscr_stru *pst_bss_dscr,
                                      wlan_protocol_enum_uint8 *pen_protocol_mode)
{
    /* ��α��� */
    if ((pst_bss_dscr == OAL_PTR_NULL) || (pen_protocol_mode == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_SCAN, "{hmac_sta_get_user_protocol::param null,%x %x.}", (uintptr_t)pst_bss_dscr,
                       (uintptr_t)pen_protocol_mode);
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (pst_bss_dscr->en_vht_capable == OAL_TRUE) {
        *pen_protocol_mode = WLAN_VHT_MODE;
    } else if (pst_bss_dscr->en_ht_capable == OAL_TRUE) {
        *pen_protocol_mode = WLAN_HT_MODE;
    } else {
        /* �ж��Ƿ���5G */
        if (pst_bss_dscr->st_channel.en_band == WLAN_BAND_5G) {
            *pen_protocol_mode = WLAN_LEGACY_11A_MODE;
        } else {
            if (hmac_is_support_11grate(pst_bss_dscr->auc_supp_rates, pst_bss_dscr->uc_num_supp_rates) == OAL_TRUE) {
                *pen_protocol_mode = WLAN_LEGACY_11G_MODE;
                if (hmac_is_support_11brate(pst_bss_dscr->auc_supp_rates, pst_bss_dscr->uc_num_supp_rates) ==
                    OAL_TRUE) {
                    *pen_protocol_mode = WLAN_MIXED_ONE_11G_MODE;
                }
            } else if (hmac_is_support_11brate(pst_bss_dscr->auc_supp_rates, pst_bss_dscr->uc_num_supp_rates) ==
                       OAL_TRUE) {
                *pen_protocol_mode = WLAN_LEGACY_11B_MODE;
            } else {
                oam_warning_log0(0, OAM_SF_ANY, "{hmac_sta_get_user_protocol::get user protocol failed.}");
            }
        }
    }

    return OAL_SUCC;
}
/*
 * �� �� ��  : hmac_sta_send_auth_seq3
 * ��������  : STA����WEP SHARE KEY AUTH ���к�Ϊ3��֡(��Ȧ���Ӷ�)
 */
OAL_STATIC oal_uint32 hmac_sta_send_auth_seq3(hmac_vap_stru *pst_sta, oal_uint8 *puc_mac_hdr)
{
    oal_netbuf_stru *pst_auth_frame = OAL_PTR_NULL;
    hmac_user_stru *pst_hmac_user_ap = OAL_PTR_NULL;
    mac_tx_ctl_stru *pst_tx_ctl = OAL_PTR_NULL;
    oal_uint16 us_auth_frame_len;
    oal_uint32 ul_ret;

    /* ׼��seq = 3����֤֡ */
    pst_auth_frame = oal_mem_netbuf_alloc(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (pst_auth_frame == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::pst_auth_frame null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    oal_mem_netbuf_trace(pst_auth_frame, OAL_TRUE);

    memset_s(oal_netbuf_cb(pst_auth_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    us_auth_frame_len = hmac_mgmt_encap_auth_req_seq3(pst_sta,
                                                      (oal_uint8 *)oal_netbuf_header(pst_auth_frame),
                                                      puc_mac_hdr);
    oal_netbuf_put(pst_auth_frame, us_auth_frame_len);

    pst_hmac_user_ap = mac_res_get_hmac_user((oal_uint16)pst_sta->st_vap_base_info.uc_assoc_vap_id);
    if (pst_hmac_user_ap == OAL_PTR_NULL) {
        oal_netbuf_free(pst_auth_frame);
        OAM_ERROR_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                       "{hmac_wait_auth_sta::pst_hmac_user_ap[%d] null.}",
                       pst_sta->st_vap_base_info.uc_assoc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��д���ͺͷ��������Ҫ�Ĳ��� */
    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_auth_frame);
    pst_tx_ctl->us_mpdu_len = us_auth_frame_len;                                  /* ������Ҫ֡���� */
    pst_tx_ctl->us_tx_user_idx = pst_hmac_user_ap->st_user_base_info.us_assoc_id; /* �������Ҫ��ȡ�û� */

    /* ���¼���dmac���� */
    ul_ret = hmac_tx_mgmt_send_event(&pst_sta->st_vap_base_info, pst_auth_frame, us_auth_frame_len);
    if (ul_ret != OAL_SUCC) {
        oal_netbuf_free(pst_auth_frame);
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_wait_auth_sta::hmac_tx_mgmt_send_event failed[%d].}", ul_ret);
        return ul_ret;
    }

    frw_immediate_destroy_timer(&pst_sta->st_mgmt_timer);

    /* ����״̬ΪMAC_VAP_STATE_STA_WAIT_AUTH_SEQ4����������ʱ�� */
    hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4);

    frw_create_timer(&pst_sta->st_mgmt_timer,
                     hmac_mgmt_timeout_sta,
                     pst_sta->st_mgmt_timer.ul_timeout,
                     &pst_sta->st_mgmt_timetout_param,
                     OAL_FALSE,
                     OAM_MODULE_ID_HMAC,
                     pst_sta->st_vap_base_info.ul_core_id);
    return OAL_SUCC;
}


oal_uint32 hmac_sta_wait_join(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_join_req_stru *pst_join_req = OAL_PTR_NULL;
    hmac_join_rsp_stru st_join_rsp;
    oal_uint32 ul_ret;

    if ((pst_hmac_sta == OAL_PTR_NULL) || (pst_msg == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_SCAN, "{hmac_sta_wait_join::param null,%x %x.}", (uintptr_t)pst_hmac_sta,
                       (uintptr_t)pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 1102 P2PSTA���� todo ���²���ʧ�ܵĻ���Ҫ���ض����Ǽ����·�Join���� */
    ul_ret = hmac_p2p_check_can_enter_state(&(pst_hmac_sta->st_vap_base_info), HMAC_FSM_INPUT_ASOC_REQ);
    if (ul_ret != OAL_SUCC) {
        /* ���ܽ������״̬�������豸æ */
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_sta_wait_join fail,device busy: ul_ret=%d}\r\n", ul_ret);
        return OAL_ERR_CODE_CONFIG_BUSY;
    }

    pst_join_req = (hmac_join_req_stru *)pst_msg;

    /* ����JOIN REG params ��MIB��MAC�Ĵ��� */
    ul_ret = hmac_sta_update_join_req_params(pst_hmac_sta, pst_join_req);
    if (ul_ret != OAL_SUCC) {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_join::get hmac_sta_update_join_req_params fail[%d]!}", ul_ret);
        return ul_ret;
    }
    oam_info_log3(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                  "{hmac_sta_wait_join::Join AP channel=%d Beacon Period=%d DTIM Period=%d.}",
                  pst_join_req->st_bss_dscr.st_channel.uc_chan_number,
                  pst_join_req->st_bss_dscr.us_beacon_period,
                  pst_join_req->st_bss_dscr.uc_dtim_period);
    {
        dmac_ctx_set_dtim_tsf_reg_stru *pst_set_dtim_tsf_reg_params = OAL_PTR_NULL;
        frw_event_mem_stru *pst_event_mem;
        frw_event_stru *pst_event = OAL_PTR_NULL;

        /* ���¼���DMAC, �����¼��ڴ� */
        pst_event_mem = frw_event_alloc_m(OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru));
        if (pst_event_mem == OAL_PTR_NULL) {
            OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                           "{hmac_sta_wait_join::alloc null, size[%d].}", OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru));
            return OAL_ERR_CODE_PTR_NULL;
        }

        /* ��д�¼� */
        pst_event = (frw_event_stru *)pst_event_mem->puc_data;

        frw_event_hdr_init(&(pst_event->st_event_hdr),
                           FRW_EVENT_TYPE_WLAN_CTX,
                           DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG,
                           OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru),
                           FRW_EVENT_PIPELINE_STAGE_1,
                           pst_hmac_sta->st_vap_base_info.uc_chip_id,
                           pst_hmac_sta->st_vap_base_info.uc_device_id,
                           pst_hmac_sta->st_vap_base_info.uc_vap_id);

        pst_set_dtim_tsf_reg_params = (dmac_ctx_set_dtim_tsf_reg_stru *)pst_event->auc_event_data;

        /* ��Ap bssid��tsf REG ����ֵ�������¼�payload�� */
        pst_set_dtim_tsf_reg_params->ul_dtim_cnt = pst_join_req->st_bss_dscr.uc_dtim_cnt;
        pst_set_dtim_tsf_reg_params->ul_dtim_period = pst_join_req->st_bss_dscr.uc_dtim_period;
        pst_set_dtim_tsf_reg_params->us_tsf_bit0 = BIT0;
        memcpy_s(pst_set_dtim_tsf_reg_params->auc_bssid, WLAN_MAC_ADDR_LEN,
                 pst_hmac_sta->st_vap_base_info.auc_bssid, WLAN_MAC_ADDR_LEN);

        /* �ַ��¼� */
        frw_event_dispatch_event(pst_event_mem);
        frw_event_free_m(pst_event_mem);
    }

    if ((g_hitalk_status & NBFH_ON_MASK) == 0) {
        st_join_rsp.en_result_code = HMAC_MGMT_SUCCESS;
    }

    /* �л�STA״̬��JOIN_COMP */
    hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_JOIN_COMP);

    if ((g_hitalk_status & NBFH_ON_MASK) == 0) {
        /* ����JOIN�ɹ���Ϣ��SME hmac_handle_join_rsp_sta */
        hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP, (oal_uint8 *)&st_join_rsp);
    } else {
        oam_warning_log0(0, 0, "hmac_sta_wait_join::change to MAC_VAP_STATE_STA_JOIN_COMP");
    }

    oam_info_log3(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                  "{hmac_sta_wait_join::Join AP[%08x] HT=%d VHT=%d SUCC.}",
                  mac_addr(pst_join_req->st_bss_dscr.auc_bssid),
                  pst_join_req->st_bss_dscr.en_ht_capable,
                  pst_join_req->st_bss_dscr.en_vht_capable);

    oam_warning_log4(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                     "{hmac_sta_wait_join::Join AP channel=%d idx=%d bandwidth=%d Beacon Period=%d SUCC.}",
                     pst_join_req->st_bss_dscr.st_channel.uc_chan_number,
                     pst_join_req->st_bss_dscr.st_channel.uc_idx,
                     pst_hmac_sta->st_vap_base_info.st_channel.en_bandwidth,
                     pst_join_req->st_bss_dscr.us_beacon_period);

    return OAL_SUCC;
}


oal_uint32 hmac_sta_wait_join_rx(hmac_vap_stru *pst_hmac_sta, oal_void *p_param)
{
    dmac_wlan_crx_event_stru *pst_mgmt_rx_event = OAL_PTR_NULL;
    dmac_rx_ctl_stru *pst_rx_ctrl = OAL_PTR_NULL;
    mac_rx_ctl_stru *pst_rx_info = OAL_PTR_NULL;
    oal_uint8 *puc_mac_hdr = OAL_PTR_NULL;
    frw_event_mem_stru *pst_event_mem = OAL_PTR_NULL;
    frw_event_stru *pst_event = OAL_PTR_NULL;
    dmac_ctx_set_dtim_tsf_reg_stru st_set_dtim_tsf_reg_params = { 0 };
    oal_uint8 *puc_tim_elm = OAL_PTR_NULL;
    oal_uint16 us_rx_len;
    oal_uint8 auc_bssid[6] = { 0 }; /* mac addr 6 byte */

    if ((pst_hmac_sta == OAL_PTR_NULL) || (p_param == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_SCAN, "{hmac_sta_wait_join_rx::param null,%x %x.}", (uintptr_t)pst_hmac_sta,
                       (uintptr_t)p_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mgmt_rx_event = (dmac_wlan_crx_event_stru *)p_param;
    pst_rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_mgmt_rx_event->pst_netbuf);
    pst_rx_info = (mac_rx_ctl_stru *)(&(pst_rx_ctrl->st_rx_info));
    puc_mac_hdr = (oal_uint8 *)(pst_rx_info->pul_mac_hdr_start_addr);
    us_rx_len = pst_rx_ctrl->st_rx_info.us_frame_len; /* ��Ϣ�ܳ���,������FCS */

    /* ��WAIT_JOIN״̬�£�������յ���beacon֡ */
    switch (mac_get_frame_sub_type(puc_mac_hdr)) {
        case WLAN_FC0_SUBTYPE_BEACON: {
            /* ��ȡBeacon֡�е�mac��ַ����AP��mac��ַ */
            mac_get_bssid(puc_mac_hdr, auc_bssid);

            /* ���STA�����AP mac��ַ�����beacon֡��mac��ַƥ�䣬�����beacon֡�е�DTIM countֵ��STA����mib���� */
            if (oal_memcmp(auc_bssid, pst_hmac_sta->st_vap_base_info.auc_bssid, WLAN_MAC_ADDR_LEN) == 0) {
                puc_tim_elm = mac_find_ie(MAC_EID_TIM, puc_mac_hdr + MAC_TAG_PARAM_OFFSET,
                                          us_rx_len - MAC_TAG_PARAM_OFFSET);
                /* ��tim IE����ȡ DTIM countֵ,д�뵽MAC H/W REG�� */
                if ((puc_tim_elm != OAL_PTR_NULL) && (puc_tim_elm[1] >= MAC_MIN_TIM_LEN)) {
                    pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_sta_config.ul_dot11DTIMPeriod =
                        puc_tim_elm[3]; /* ��dtim_period(puc_tim_elm[3])���浽ul_dot11DTIMPeriod */

                    /* ��dtim_cnt��dtim_period�������¼�payload�� */
                    /* ��dtim_cnt(puc_tim_elm[2])�������¼�payload�� */
                    st_set_dtim_tsf_reg_params.ul_dtim_cnt = puc_tim_elm[2];
                    /* ��dtim_period(puc_tim_elm[3])�������¼�payload�� */
                    st_set_dtim_tsf_reg_params.ul_dtim_period = puc_tim_elm[3];
                } else {
                    oam_warning_log0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                                     "{hmac_sta_wait_join::Do not find Tim ie.}");
                }

                /* ��Ap bssid��tsf REG ����ֵ�������¼�payload�� */
                memcpy_s(st_set_dtim_tsf_reg_params.auc_bssid, WLAN_MAC_ADDR_LEN, auc_bssid, WLAN_MAC_ADDR_LEN);
                st_set_dtim_tsf_reg_params.us_tsf_bit0 = BIT0;

                /* ���¼���DMAC, �����¼��ڴ� */
                pst_event_mem = frw_event_alloc_m(OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru));
                if (pst_event_mem == OAL_PTR_NULL) {
                    OAM_ERROR_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                                   "{hmac_sta_wait_join::pst_event_mem null.}");
                    return OAL_ERR_CODE_PTR_NULL;
                }

                /* ��д�¼� */
                pst_event = (frw_event_stru *)pst_event_mem->puc_data;

                frw_event_hdr_init(&(pst_event->st_event_hdr),
                                   FRW_EVENT_TYPE_WLAN_CTX, DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG,
                                   OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru), FRW_EVENT_PIPELINE_STAGE_1,
                                   pst_hmac_sta->st_vap_base_info.uc_chip_id,
                                   pst_hmac_sta->st_vap_base_info.uc_device_id,
                                   pst_hmac_sta->st_vap_base_info.uc_vap_id);

                /* �������� */
                if (memcpy_s(frw_get_event_payload(pst_event_mem), OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru),
                             (oal_uint8 *)&st_set_dtim_tsf_reg_params,
                             OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru)) != EOK) {
                    OAM_ERROR_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "memcpy fail!");
                    frw_event_free_m(pst_event_mem);
                    return OAL_FAIL;
                }

                /* �ַ��¼� */
                frw_event_dispatch_event(pst_event_mem);
                frw_event_free_m(pst_event_mem);
            }
        }
        break;

        case WLAN_FC0_SUBTYPE_ACTION: {
            /* do nothing */
        }
        break;

        default: {
            /* do nothing */
        }
        break;
    }

    return OAL_SUCC;
}


oal_uint32 hmac_sta_wait_join_timeout(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_join_rsp_stru st_join_rsp = { 0 };

    if ((pst_hmac_sta == OAL_PTR_NULL) || (pst_msg == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_SCAN, "{hmac_sta_wait_join_timeout::param null,%x %x.}", (uintptr_t)pst_hmac_sta,
                       (uintptr_t)pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAM_ERROR_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                   "{hmac_sta_wait_join_timeout::join timeout.}");
    /* ����timeout��������ʾjoinû�гɹ�����join�Ľ������Ϊtimeout�ϱ���sme */
    st_join_rsp.en_result_code = HMAC_MGMT_TIMEOUT;

    /* ��hmac״̬���л�ΪMAC_VAP_STATE_STA_FAKE_UP */
    hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_FAKE_UP);

    /* ���ͳ�ʱ��Ϣ��SME */
    hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP, (oal_uint8 *)&st_join_rsp);

    return OAL_SUCC;
}


oal_uint32 hmac_sta_wait_join_misc(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_join_rsp_stru st_join_rsp;
    hmac_misc_input_stru *st_misc_input = (hmac_misc_input_stru *)pst_msg;

    if ((pst_hmac_sta == OAL_PTR_NULL) || (pst_msg == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_SCAN, "{hmac_sta_wait_join_misc::param null,%x %x.}", (uintptr_t)pst_hmac_sta,
                       (uintptr_t)pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    oam_info_log0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_sta_wait_join_misc::enter func.}");
    switch (st_misc_input->en_type) {
        /* ����TBTT�ж� */
        case HMAC_MISC_TBTT: {
            /* ���յ�TBTT�жϣ���ζ��JOIN�ɹ��ˣ�����ȡ��JOIN��ʼʱ���õĶ�ʱ��,����Ϣ֪ͨSME */
            frw_immediate_destroy_timer(&pst_hmac_sta->st_mgmt_timer);

            st_join_rsp.en_result_code = HMAC_MGMT_SUCCESS;

            oam_info_log0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                          "{hmac_sta_wait_join_misc::join succ.}");
            /* �л�STA״̬��JOIN_COMP */
            hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_JOIN_COMP);

            /* ����JOIN�ɹ���Ϣ��SME */
            hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP, (oal_uint8 *)&st_join_rsp);
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

oal_void hmac_set_auth_assoc_send_status_report_flag(
    hmac_vap_stru *pst_hmac_vap, oal_bool_enum_uint8 en_status)
{
    mac_device_stru *pst_mac_device = OAL_PTR_NULL;
    if (!IS_LEGACY_STA(&pst_hmac_vap->st_vap_base_info)) {
        return;
    }
    pst_mac_device = mac_res_get_dev(pst_hmac_vap->st_vap_base_info.uc_device_id);
    if (oal_unlikely(pst_mac_device == OAL_PTR_NULL)) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_set_auth_assoc_send_status_report_flag:mac_res_get_dev fail.}");
        return;
    }
    pst_mac_device->en_report_mgmt_req_status = en_status;
}


oal_uint32 hmac_sta_wait_auth(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_auth_req_stru *pst_auth_req = OAL_PTR_NULL;
    oal_netbuf_stru *pst_auth_frame = OAL_PTR_NULL;
    oal_uint16 us_auth_len;
    mac_tx_ctl_stru *pst_tx_ctl = OAL_PTR_NULL;
    hmac_user_stru *pst_hmac_user_ap = OAL_PTR_NULL;
    oal_uint32 ul_ret;

    if ((pst_hmac_sta == OAL_PTR_NULL) || (pst_msg == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_AUTH, "{hmac_sta_wait_auth::param null,%x %x.}", (uintptr_t)pst_hmac_sta,
                       (uintptr_t)pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_auth_req = (hmac_auth_req_stru *)pst_msg;

#ifdef _PRE_WLAN_FEATURE_SAE
    if ((pst_hmac_sta->en_auth_mode == WLAN_WITP_AUTH_SAE) &&
        (pst_hmac_sta->bit_sae_connect_with_pmkid == OAL_FALSE)) {
        oal_uint16 us_user_index = MAC_INVALID_USER_ID;
        /* STA��һ��SAE����,������pmkid,�ϱ�external auth�¼���wpa_s;
         * ����������WPA2���� */
        oam_warning_log0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_sta_wait_auth:: report external auth to wpa_s.}");

        /* ��STA ����û� */
        pst_hmac_user_ap = (hmac_user_stru *)mac_res_get_hmac_user(pst_hmac_sta->st_vap_base_info.uc_assoc_vap_id);
        if (pst_hmac_user_ap == OAL_PTR_NULL) {
            ul_ret = hmac_user_add(&(pst_hmac_sta->st_vap_base_info), pst_hmac_sta->st_vap_base_info.auc_bssid,
                                   &us_user_index);
            if (ul_ret != OAL_SUCC) {
                OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                                 "{hmac_sta_wait_auth:: add sae user failed[%d].}", ul_ret);
                return OAL_FAIL;
            }
        }

        /* �ϱ�����external auth��wpa_s(hmac_report_ext_auth_worker) */
        oal_workqueue_schedule(&(pst_hmac_sta->st_sae_report_ext_auth_worker));

        /* �л�STA ��MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2 */
        hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2);

        /* ������֤��ʱ��ʱ�� */
        pst_hmac_sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2;
        pst_hmac_sta->st_mgmt_timetout_param.uc_vap_id = pst_hmac_sta->st_vap_base_info.uc_vap_id;
        pst_hmac_sta->st_mgmt_timetout_param.us_user_index = us_user_index;
        frw_create_timer(&pst_hmac_sta->st_mgmt_timer,
                         hmac_mgmt_timeout_sta,
                         pst_auth_req->us_timeout,
                         &pst_hmac_sta->st_mgmt_timetout_param,
                         OAL_FALSE, OAM_MODULE_ID_HMAC,
                         pst_hmac_sta->st_vap_base_info.ul_core_id);
        return OAL_SUCC;
    }
#endif /* _PRE_WLAN_FEATURE_SAE */

    /* ������֤֡�ռ� */
    pst_auth_frame = oal_mem_netbuf_alloc(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (pst_auth_frame == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                       "{hmac_wait_auth_sta::puc_auth_frame null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oal_mem_netbuf_trace(pst_auth_frame, OAL_TRUE);

    memset_s(oal_netbuf_cb(pst_auth_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    memset_s((oal_uint8 *)oal_netbuf_header(pst_auth_frame), MAC_80211_FRAME_LEN, 0, MAC_80211_FRAME_LEN);

    /* ��seq = 1 ����֤����֡ */
    us_auth_len = hmac_mgmt_encap_auth_req(pst_hmac_sta, (oal_uint8 *)(oal_netbuf_header(pst_auth_frame)));
    if (us_auth_len == 0) {
        /* ��֡ʧ�� */
        oam_warning_log0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_wait_auth_sta::hmac_mgmt_encap_auth_req failed.}");

        oal_netbuf_free(pst_auth_frame);
        hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_FAKE_UP);
    } else {
        oal_netbuf_put(pst_auth_frame, us_auth_len);
        pst_hmac_user_ap = mac_res_get_hmac_user((oal_uint16)pst_hmac_sta->st_vap_base_info.uc_assoc_vap_id);
        if (pst_hmac_user_ap == OAL_PTR_NULL) {
            oal_netbuf_free(pst_auth_frame);
            OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                           "{hmac_wait_auth_sta::user_ap[%d] null.}", pst_hmac_sta->st_vap_base_info.uc_assoc_vap_id);
            return OAL_ERR_CODE_PTR_NULL;
        }

        /* Ϊ��д����������׼������ */
        pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_auth_frame);                /* ��ȡcb�ṹ�� */
        pst_tx_ctl->us_mpdu_len = us_auth_len;                                        /* dmac������Ҫ��mpdu���� */
        pst_tx_ctl->us_tx_user_idx = pst_hmac_user_ap->st_user_base_info.us_assoc_id; /* ���������Ҫ��ȡuser�ṹ�� */

        /* �����WEP����Ҫ��ap��mac��ַд��lut */
        ul_ret = hmac_init_security(&(pst_hmac_sta->st_vap_base_info),
                                    pst_hmac_user_ap->st_user_base_info.auc_user_mac_addr,
                                    OAL_SIZEOF(pst_hmac_user_ap->st_user_base_info.auc_user_mac_addr));
        if (ul_ret != OAL_SUCC) {
            OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{init_secure failed[%d].}", ul_ret);
        }
        /* ���¼���dmac����֡����,����� */
        hmac_set_auth_assoc_send_status_report_flag(pst_hmac_sta, OAL_FALSE);
        ul_ret = hmac_tx_mgmt_send_event(&pst_hmac_sta->st_vap_base_info, pst_auth_frame, us_auth_len);
        if (ul_ret != OAL_SUCC) {
            oal_netbuf_free(pst_auth_frame);
            OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                             "{hmac_wait_auth_sta::hmac_tx_mgmt_send_event failed[%d].}", ul_ret);
            return ul_ret;
        }

        /* ����״̬ */
        hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2);

        /* ������֤��ʱ��ʱ�� */
        pst_hmac_sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2;
        pst_hmac_sta->st_mgmt_timetout_param.us_user_index = pst_hmac_user_ap->st_user_base_info.us_assoc_id;
        pst_hmac_sta->st_mgmt_timetout_param.uc_vap_id = pst_hmac_sta->st_vap_base_info.uc_vap_id;
        frw_create_timer(&pst_hmac_sta->st_mgmt_timer,
                         hmac_mgmt_timeout_sta,
                         pst_auth_req->us_timeout,
                         &pst_hmac_sta->st_mgmt_timetout_param,
                         OAL_FALSE,
                         OAM_MODULE_ID_HMAC,
                         pst_hmac_sta->st_vap_base_info.ul_core_id);
    }

    return OAL_SUCC;
}


oal_uint32 hmac_sta_wait_beacon_before_auth(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_join_rsp_stru st_join_rsp;
    dmac_wlan_crx_event_stru *pst_mgmt_rx_event = OAL_PTR_NULL;
    dmac_rx_ctl_stru *pst_rx_ctrl = OAL_PTR_NULL;
    mac_rx_ctl_stru *pst_rx_info = OAL_PTR_NULL;
    oal_uint8 *puc_mac_hdr = OAL_PTR_NULL;

    oal_uint8 auc_bssid[6] = { 0 }; /* mac addr 6 byte */

    if ((pst_hmac_sta == OAL_PTR_NULL) || (pst_msg == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_SCAN, "{hmac_sta_wait_beacon_before_auth::param null,%x %x.}",
                       (uintptr_t)pst_hmac_sta, (uintptr_t)pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mgmt_rx_event = (dmac_wlan_crx_event_stru *)pst_msg;
    pst_rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_mgmt_rx_event->pst_netbuf);
    pst_rx_info = (mac_rx_ctl_stru *)(&(pst_rx_ctrl->st_rx_info));
    puc_mac_hdr = (oal_uint8 *)pst_rx_info->pul_mac_hdr_start_addr;

    /* ��JOIN COMP״̬�£�������յ���beacon֡ */
    switch (mac_get_frame_type_and_subtype(puc_mac_hdr)) {
        case WLAN_FC0_SUBTYPE_BEACON | WLAN_FC0_TYPE_MGT: {
            /* ��ȡBeacon֡�е�mac��ַ����AP��mac��ַ */
            mac_get_bssid(puc_mac_hdr, auc_bssid);

            /* ���STA�����AP mac��ַ�����beacon֡��mac��ַƥ�䣬�����beacon֡�е�DTIM countֵ��STA����mib���� */
            if (memcmp(auc_bssid, pst_hmac_sta->st_vap_base_info.auc_bssid, WLAN_MAC_ADDR_LEN) == 0) {
                /* ���յ�TBTT�жϣ���ζ��խ����������Ƶ��ȡ�����õĶ�ʱ��,����Ϣ֪ͨSME */
                frw_immediate_destroy_timer(&pst_hmac_sta->st_mgmt_timer);

                st_join_rsp.en_result_code = HMAC_MGMT_SUCCESS;
                /* �л�STA״̬��JOIN_COMP */
                hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_NBFH_COMP);

                oam_warning_log0(0, 0, "hmac_sta_wait_beacon_before_auth called ");

                /* ����JOIN�ɹ���Ϣ��SME */
                hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP, (oal_uint8 *)&st_join_rsp);
            }
        }
    }

    return OAL_SUCC;
}


oal_uint32 hmac_sta_wait_auth_seq2_rx(hmac_vap_stru *pst_sta, oal_void *pst_msg)
{
    dmac_wlan_crx_event_stru *pst_crx_event = OAL_PTR_NULL;
    hmac_rx_ctl_stru *pst_rx_ctrl = OAL_PTR_NULL; /* ÿһ��MPDU�Ŀ�����Ϣ */
    oal_uint8 *puc_mac_hdr = OAL_PTR_NULL;
    oal_uint16 us_auth_alg;
    hmac_auth_rsp_stru st_auth_rsp = {
        { 0 },
    };

    if ((pst_sta == OAL_PTR_NULL) || (pst_msg == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq2_rx::param null,%x %x.}", (uintptr_t)pst_sta,
                       (uintptr_t)pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_crx_event = (dmac_wlan_crx_event_stru *)pst_msg;
    pst_rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(pst_crx_event->pst_netbuf); /* ÿһ��MPDU�Ŀ�����Ϣ */
    puc_mac_hdr = (oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if (wlan_pm_wkup_src_debug_get() == OAL_TRUE) {
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX,
                         "{wifi_wake_src:hmac_sta_wait_auth_seq2_rx_etc::wakeup mgmt type[0x%x]}",
                         mac_get_frame_type_and_subtype(puc_mac_hdr));
    }
#endif
#endif

    if (mac_get_frame_sub_type(puc_mac_hdr) != WLAN_FC0_SUBTYPE_AUTH) {
        return OAL_SUCC;
    }

    us_auth_alg = mac_get_auth_alg(puc_mac_hdr);
#ifdef _PRE_WLAN_FEATURE_SAE
    /* ע��:mib ֵ����д��auth_alg ֵ�����ںˣ���ieee�����auth_algȡֵ��ͬ */
    if ((us_auth_alg == WLAN_MIB_AUTH_ALG_SAE) &&
        (pst_sta->bit_sae_connect_with_pmkid == OAL_FALSE)) {
        return hmac_sta_process_sae_commit(pst_sta, pst_crx_event->pst_netbuf);
    }
#endif

    if (mac_get_auth_seq_num(puc_mac_hdr) != WLAN_AUTH_TRASACTION_NUM_TWO) {
        return OAL_SUCC;
    }

#ifdef _PRE_WLAN_FEATURE_SNIFFER
    proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_ctrl->st_rx_info.us_frame_len, 0);
#endif
    /* AUTH alg CHECK */
    if ((pst_sta->en_auth_mode != us_auth_alg) &&
        (pst_sta->en_auth_mode != WLAN_WITP_AUTH_AUTOMATIC)) {
        oam_warning_log2(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "sta_wait_auth_seq2_rx::rcv unexpected auth alg[%d/%d].", us_auth_alg, pst_sta->en_auth_mode);
    }
    st_auth_rsp.us_status_code = mac_get_auth_status(puc_mac_hdr);
    if (st_auth_rsp.us_status_code != MAC_SUCCESSFUL_STATUSCODE) {
        frw_immediate_destroy_timer(&pst_sta->st_mgmt_timer);

        /* �ϱ���SME��֤��� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq2_rx::AP refuse STA auth reason[%d]!}", st_auth_rsp.us_status_code);
#ifdef _PRE_WLAN_1102A_CHR
        if (st_auth_rsp.us_status_code != MAC_AP_FULL) {
            chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV,
                                 CHR_WIFI_DRV_EVENT_CONNECT, CHR_WIFI_DRV_ERROR_AUTH_REJECTED);
        }
#endif
        return OAL_SUCC;
    }

    /* auth response status_code �ɹ����� */
    if (us_auth_alg == WLAN_WITP_AUTH_OPEN_SYSTEM) {
        frw_immediate_destroy_timer(&pst_sta->st_mgmt_timer);

        /* ��״̬����ΪAUTH_COMP */
        hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);
        st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* �ϱ���SME��֤�ɹ� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);
        return OAL_SUCC;
    } else if (us_auth_alg == WLAN_WITP_AUTH_SHARED_KEY) {
        return hmac_sta_send_auth_seq3(pst_sta, puc_mac_hdr);
    } else {
        frw_immediate_destroy_timer(&pst_sta->st_mgmt_timer);

        /* ���յ�AP �ظ���auth response ��֧����֤�㷨��ǰ��֧�ֵ�����£�status code ȴ��SUCC,
            ��Ϊ��֤�ɹ������Ҽ����������� */
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq2_rx::AP's auth_alg [%d] not support!}", us_auth_alg);

        /* ��״̬����ΪAUTH_COMP */
        hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);
        st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* �ϱ���SME��֤�ɹ� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);
    }

    return OAL_SUCC;
}


oal_uint32 hmac_sta_wait_auth_seq4_rx(hmac_vap_stru *pst_sta, oal_void *p_msg)
{
    dmac_wlan_crx_event_stru *pst_crx_event = OAL_PTR_NULL;
    hmac_rx_ctl_stru *pst_rx_ctrl = OAL_PTR_NULL; /* ÿһ��MPDU�Ŀ�����Ϣ */
    oal_uint8 *puc_mac_hdr = OAL_PTR_NULL;
    oal_uint16 us_auth_status;
    hmac_auth_rsp_stru st_auth_rsp = {
        { 0 },
    };

    if ((p_msg == OAL_PTR_NULL) || (pst_sta == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq4_rx::param null,%x %x.}", (uintptr_t)p_msg,
                       (uintptr_t)pst_sta);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_crx_event = (dmac_wlan_crx_event_stru *)p_msg;
    pst_rx_ctrl = (hmac_rx_ctl_stru *)oal_netbuf_cb(pst_crx_event->pst_netbuf); /* ÿһ��MPDU�Ŀ�����Ϣ */
    puc_mac_hdr = (oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr;

#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if (wlan_pm_wkup_src_debug_get() == OAL_TRUE) {
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX,
                         "{wifi_wake_src:hmac_sta_wait_auth_seq4_rx_etc::wakeup mgmt type[0x%x]}",
                         mac_get_frame_type_and_subtype(puc_mac_hdr));
    }
#endif

    if (mac_get_frame_type_and_subtype(puc_mac_hdr) != (WLAN_FC0_SUBTYPE_AUTH | WLAN_FC0_TYPE_MGT)) {
        return OAL_SUCC;
    }

#ifdef _PRE_WLAN_FEATURE_SAE
    /* ע��:mib ֵ����д��auth_alg ֵ�����ںˣ���ieee�����auth_algȡֵ��ͬ */
    if ((mac_get_auth_alg(puc_mac_hdr) == WLAN_MIB_AUTH_ALG_SAE) &&
        (pst_sta->bit_sae_connect_with_pmkid == OAL_FALSE)) {
        return hmac_sta_process_sae_confirm(pst_sta, pst_crx_event->pst_netbuf);
    }
#endif

#ifdef _PRE_WLAN_FEATURE_SNIFFER
    proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_ctrl->st_rx_info.us_frame_len, 0);
#endif
    us_auth_status = mac_get_auth_status(puc_mac_hdr);
    if ((mac_get_auth_seq_num(puc_mac_hdr) == WLAN_AUTH_TRASACTION_NUM_FOUR) &&
        (us_auth_status == MAC_SUCCESSFUL_STATUSCODE)) {
        /* ���յ�seq = 4 ��״̬λΪsucc ȡ����ʱ�� */
        frw_immediate_destroy_timer(&pst_sta->st_mgmt_timer);

        st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* ����sta״̬ΪMAC_VAP_STATE_STA_AUTH_COMP */
        hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);
        /* ����֤����ϱ�SME */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);
    } else {
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq4_rx::transaction num.status[%d]}", us_auth_status);
        /* �ȴ���ʱ����ʱ */
    }

    return OAL_SUCC;
}
static void hmac_sta_start_mgmt_timer(hmac_vap_stru *sta,
                                      hmac_asoc_req_stru *hmac_asoc_req, hmac_user_stru *hmac_user_ap)
{
    sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_ASOC;
    sta->st_mgmt_timetout_param.us_user_index = hmac_user_ap->st_user_base_info.us_assoc_id;
    sta->st_mgmt_timetout_param.uc_vap_id = sta->st_vap_base_info.uc_vap_id;
    sta->st_mgmt_timetout_param.en_status_code = MAC_ASOC_RSP_TIMEOUT;
    frw_create_timer(&(sta->st_mgmt_timer), hmac_mgmt_timeout_sta,
                     hmac_asoc_req->us_assoc_timeout, &(sta->st_mgmt_timetout_param),
                     OAL_FALSE, OAM_MODULE_ID_HMAC, sta->st_vap_base_info.ul_core_id);
}

oal_uint32 hmac_sta_wait_asoc(hmac_vap_stru *pst_sta, oal_void *pst_msg)
{
    hmac_asoc_req_stru *pst_hmac_asoc_req = OAL_PTR_NULL;
    oal_netbuf_stru *pst_asoc_req_frame = OAL_PTR_NULL;
    mac_tx_ctl_stru *pst_tx_ctl = OAL_PTR_NULL;
    hmac_user_stru *pst_hmac_user_ap = OAL_PTR_NULL;
    oal_uint32 ul_asoc_frame_len;
    oal_uint32 ul_ret;
    oal_uint8 *puc_curr_bssid = OAL_PTR_NULL;
    oal_int32 l_ret;

    if ((pst_sta == OAL_PTR_NULL) || (pst_msg == OAL_PTR_NULL)) {
        oam_error_log2(0, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::null,%x %x.}", (uintptr_t)pst_sta, (uintptr_t)pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_asoc_req = (hmac_asoc_req_stru *)pst_msg;

    pst_asoc_req_frame = oal_mem_netbuf_alloc(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (pst_asoc_req_frame == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_asoc::pst_asoc_req_frame null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    oal_mem_netbuf_trace(pst_asoc_req_frame, OAL_TRUE);

    memset_s(oal_netbuf_cb(pst_asoc_req_frame), oal_netbuf_cb_size(), 0, oal_netbuf_cb_size());

    /* ��mac header���� */
    memset_s((oal_uint8 *)oal_netbuf_header(pst_asoc_req_frame), MAC_80211_FRAME_LEN, 0, MAC_80211_FRAME_LEN);

    /* ��֡ (Re)Assoc_req_Frame */
    if (pst_sta->bit_reassoc_flag) {
        puc_curr_bssid = pst_sta->st_vap_base_info.auc_bssid;
    }
    ul_asoc_frame_len = hmac_mgmt_encap_asoc_req_sta(pst_sta, (oal_uint8 *)(oal_netbuf_header(pst_asoc_req_frame)),
                                                     puc_curr_bssid);
    oal_netbuf_put(pst_asoc_req_frame, ul_asoc_frame_len);

    if (ul_asoc_frame_len == 0) {
        oam_warning_log0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_sta_wait_asoc::hmac_mgmt_encap_asoc_req_sta null.}");
        oal_netbuf_free(pst_asoc_req_frame);

        return OAL_FAIL;
    }

    if (pst_sta->puc_asoc_req_ie_buff != OAL_PTR_NULL) {
        oal_mem_free_m(pst_sta->puc_asoc_req_ie_buff, OAL_TRUE);
        pst_sta->puc_asoc_req_ie_buff = OAL_PTR_NULL;
        pst_sta->ul_asoc_req_ie_len = 0;
    }

    if (oal_unlikely(ul_asoc_frame_len < OAL_ASSOC_REQ_IE_OFFSET)) {
        OAM_ERROR_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_asoc::invalid ul_asoc_req_ie_len[%u].}", ul_asoc_frame_len);
        oam_report_dft_params(BROADCAST_MACADDR, (oal_uint8 *)oal_netbuf_header(pst_asoc_req_frame),
                              (oal_uint16)ul_asoc_frame_len, OAM_OTA_TYPE_80211_FRAME);
        oal_netbuf_free(pst_asoc_req_frame);
        return OAL_FAIL;
    }

    /* Should we change the ie buff from local mem to netbuf ? */
    /* �˴�������ڴ棬ֻ���ϱ����ں˺��ͷ� */
    pst_sta->ul_asoc_req_ie_len = (pst_sta->bit_reassoc_flag) ? (ul_asoc_frame_len - OAL_ASSOC_REQ_IE_OFFSET -
                                                                 OAL_MAC_ADDR_LEN)
                                  : (ul_asoc_frame_len - OAL_ASSOC_REQ_IE_OFFSET);
    pst_sta->puc_asoc_req_ie_buff = oal_mem_alloc_m(OAL_MEM_POOL_ID_LOCAL,
                                                    (oal_uint16)(pst_sta->ul_asoc_req_ie_len), OAL_TRUE);
    if (pst_sta->puc_asoc_req_ie_buff == OAL_PTR_NULL) {
        OAM_ERROR_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_asoc::puc_asoc_req_ie_buff null,alloc %u bytes failed}",
                       (oal_uint16)(pst_sta->ul_asoc_req_ie_len));
        oal_netbuf_free(pst_asoc_req_frame);
        return OAL_FAIL;
    }

    if (pst_sta->bit_reassoc_flag) {
        l_ret = memcpy_s(pst_sta->puc_asoc_req_ie_buff, pst_sta->ul_asoc_req_ie_len,
            oal_netbuf_header(pst_asoc_req_frame) + OAL_ASSOC_REQ_IE_OFFSET + OAL_MAC_ADDR_LEN,
            pst_sta->ul_asoc_req_ie_len);
    } else {
        l_ret = memcpy_s(pst_sta->puc_asoc_req_ie_buff, pst_sta->ul_asoc_req_ie_len,
            oal_netbuf_header(pst_asoc_req_frame) + OAL_ASSOC_REQ_IE_OFFSET, pst_sta->ul_asoc_req_ie_len);
    }
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::memcpy failed}");
    }
    pst_hmac_user_ap = (hmac_user_stru *)mac_res_get_hmac_user((oal_uint16)pst_sta->st_vap_base_info.uc_assoc_vap_id);
    if (pst_hmac_user_ap == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::user_ap null.}");
        oal_netbuf_free(pst_asoc_req_frame);
        oal_mem_free_m(pst_sta->puc_asoc_req_ie_buff, OAL_TRUE);
        pst_sta->puc_asoc_req_ie_buff = OAL_PTR_NULL;
        pst_sta->ul_asoc_req_ie_len = 0;

        return OAL_ERR_CODE_PTR_NULL;
    }
    if (pst_sta->bit_reassoc_flag) {
        // �ع������������user�µķ�Ƭ���棬��ֹ�ع�������rekey���̱������鹥��
        hmac_user_clear_defrag_res(pst_hmac_user_ap);
    }

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_asoc_req_frame);

    pst_tx_ctl->us_mpdu_len = (oal_uint16)ul_asoc_frame_len;
    pst_tx_ctl->us_tx_user_idx = pst_hmac_user_ap->st_user_base_info.us_assoc_id;

    /* ���¼���DMAC����֡����,����� */
    hmac_set_auth_assoc_send_status_report_flag(pst_sta, OAL_FALSE);
    ul_ret = hmac_tx_mgmt_send_event(&(pst_sta->st_vap_base_info), pst_asoc_req_frame, (oal_uint16)ul_asoc_frame_len);
    if (ul_ret != OAL_SUCC) {
        oal_netbuf_free(pst_asoc_req_frame);
        oal_mem_free_m(pst_sta->puc_asoc_req_ie_buff, OAL_TRUE);
        pst_sta->puc_asoc_req_ie_buff = OAL_PTR_NULL;
        pst_sta->ul_asoc_req_ie_len = 0;

        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{tx_mgmt_send_event fail[%d].}", ul_ret);
        return ul_ret;
    }

    /* ����״̬ */
    hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_WAIT_ASOC);

    /* ����������ʱ��ʱ��, Ϊ�Զ�ap����һ����ʱ���������ʱapû��asoc rsp��������ʱ���� */
    hmac_sta_start_mgmt_timer(pst_sta, pst_hmac_asoc_req, pst_hmac_user_ap);

    return OAL_SUCC;
}
