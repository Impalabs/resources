

#ifdef _PRE_WLAN_FEATURE_SAE

#include "oam_ext_if.h"
#include "mac_ie.h"
#include "mac_frame_inl.h"
#include "hmac_fsm.h"
#include "hmac_sme_sta.h"
#include "hmac_device.h"
#include "hmac_mgmt_sta.h"
#include "frw_ext_if.h"
#include "hmac_roam_main.h"
#include "hmac_roam_connect.h"
#include "hmac_roam_alg.h"

#include "hmac_roam_main.h"
#include "mac_mib.h"
#include "hmac_ht_self_cure.h"
#include "securec.h"
#include "hmac_sae.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_SAE_C


uint32_t hmac_sta_sae_connect_info_wpas(hmac_vap_stru *hmac_sta, hmac_auth_req_stru *auth_req)
{
    hmac_user_stru *hmac_user_ap = NULL;
    uint16_t us_user_index = g_wlan_spec_cfg->invalid_user_id;
    uint32_t ret = OAL_SUCC;

    oam_warning_log0(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                     "{hmac_sta_sae_connect_info_wpas:: report external auth to wpa_s.}");

    /* ��STA ����û� */
    hmac_user_ap = (hmac_user_stru *)mac_res_get_hmac_user(hmac_sta->st_vap_base_info.us_assoc_vap_id);
    if (hmac_user_ap == NULL) {
        ret = hmac_user_add(&(hmac_sta->st_vap_base_info), hmac_sta->st_vap_base_info.auc_bssid, &us_user_index);
        if (ret != OAL_SUCC) {
            oam_warning_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                             "{hmac_sta_sae_connect_info_wpas:: add sae user failed[%d].}", ret);
            return OAL_FAIL;
        }
    }

    oal_workqueue_schedule(&(hmac_sta->st_sae_report_ext_auth_worker));

    /* �л�STA ��MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2 */
    hmac_fsm_change_state(hmac_sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2);

    /* ������֤��ʱ��ʱ�� */
    hmac_sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2;
    hmac_sta->st_mgmt_timetout_param.uc_vap_id = hmac_sta->st_vap_base_info.uc_vap_id;
    hmac_sta->st_mgmt_timetout_param.us_user_index = us_user_index;
    frw_timer_create_timer_m(&hmac_sta->st_mgmt_timer,
                             hmac_mgmt_timeout_sta,
                             auth_req->us_timeout,
                             &hmac_sta->st_mgmt_timetout_param,
                             OAL_FALSE,
                             OAM_MODULE_ID_HMAC,
                             hmac_sta->st_vap_base_info.core_id);
    return OAL_SUCC;
}


uint32_t hmac_sta_process_sae_commit(hmac_vap_stru *pst_sta, oal_netbuf_stru *pst_netbuf)
{
    hmac_user_stru *pst_hmac_user_ap = NULL;
    mac_rx_ctl_stru *pst_rx_ctrl = NULL; /* ÿһ��MPDU�Ŀ�����Ϣ */
    uint8_t *puc_mac_hdr = NULL;
    uint16_t us_status;
    hmac_auth_rsp_stru st_auth_rsp = {{0}, };

    pst_hmac_user_ap = mac_res_get_hmac_user(pst_sta->st_vap_base_info.us_assoc_vap_id);
    if (pst_hmac_user_ap == NULL) {
        oam_error_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                       "{hmac_sta_wait_auth_seq2_rx::pst_hmac_user[%d] null.}",
                       pst_sta->st_vap_base_info.us_assoc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_rx_ctrl = (mac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf); /* ÿһ��MPDU�Ŀ�����Ϣ */
    puc_mac_hdr = (uint8_t *)MAC_GET_RX_CB_MAC_HEADER_ADDR(pst_rx_ctrl);

    oam_warning_log2(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
                     "{hmac_sta_wait_auth_seq2_rx::rx sae auth frame, status_code %d, seq_num %d.}",
                     mac_get_auth_status(puc_mac_hdr),
                     mac_get_auth_seq_num(puc_mac_hdr));

    us_status = mac_get_auth_status(puc_mac_hdr);
    if (us_status == MAC_AP_FULL) {
        oam_warning_log1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq2_rx::AP refuse STA auth reason[%d]!}",
                         us_status);

        /* ȡ����ʱ�� */
        frw_timer_immediate_destroy_timer_m(&pst_sta->st_mgmt_timer);

        st_auth_rsp.us_status_code = us_status;

        /* �ϱ���SME��֤��� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (uint8_t *)&st_auth_rsp);

        return OAL_SUCC;
    }

    /* SAE commit֡��seq number��1��confirm֡��seq number��2 */
    if (WLAN_AUTH_TRASACTION_NUM_ONE != mac_get_auth_seq_num(puc_mac_hdr)) {
        oam_warning_log0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
            "{hmac_sta_wait_auth_seq2_rx::drop sae auth frame for wrong seq num}");
        return OAL_SUCC;
    }

    pst_sta->duplicate_auth_seq4_flag = OAL_FALSE; // seq4��λ

    /* ȡ����ʱ�� */
    frw_timer_immediate_destroy_timer_m(&pst_sta->st_mgmt_timer);

    /* �л�STA ��MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4 */
    hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4);

    /* SAE �ж�seq number�Ժ��ϴ���wpas ���� */
    hmac_rx_mgmt_send_to_host(pst_sta, pst_netbuf);

    /* ������֤��ʱ��ʱ�� */
    pst_sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4;
    pst_sta->st_mgmt_timetout_param.uc_vap_id = pst_sta->st_vap_base_info.uc_vap_id;
    pst_sta->st_mgmt_timetout_param.us_user_index = pst_hmac_user_ap->st_user_base_info.us_assoc_id;
    frw_timer_create_timer_m(&pst_sta->st_mgmt_timer, hmac_mgmt_timeout_sta, pst_sta->st_mgmt_timer.timeout,
        &pst_sta->st_mgmt_timetout_param, OAL_FALSE, OAM_MODULE_ID_HMAC, pst_sta->st_vap_base_info.core_id);

    return OAL_SUCC;
}


uint32_t hmac_sta_process_sae_confirm(hmac_vap_stru *pst_sta, oal_netbuf_stru *pst_netbuf)
{
    mac_rx_ctl_stru *pst_rx_ctrl = NULL; /* ÿһ��MPDU�Ŀ�����Ϣ */
    uint8_t *puc_mac_hdr = NULL;

    pst_rx_ctrl = (mac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf); /* ÿһ��MPDU�Ŀ�����Ϣ */
    puc_mac_hdr = (uint8_t *)MAC_GET_RX_CB_MAC_HEADER_ADDR(pst_rx_ctrl);
    /* SAE commit֡��seq number��1��confirm֡��seq number��2 */
    if (WLAN_AUTH_TRASACTION_NUM_TWO != mac_get_auth_seq_num(puc_mac_hdr)) {
        oam_warning_log2(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
            "{hmac_sta_wait_auth_seq4_rx::drop sae auth frame, status_code %d, seq_num %d.}",
            mac_get_auth_status(puc_mac_hdr),
            mac_get_auth_seq_num(puc_mac_hdr));
        return OAL_SUCC;
    }

    if (pst_sta->duplicate_auth_seq4_flag == OAL_TRUE) { // wpa3 auth seq4�ظ�֡���ˣ���ֹ�ϱ�waps
        oam_warning_log2(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
            "{hmac_sta_wait_auth_seq4_rx::drop sae auth frame, status_code %d, seq_num %d.}",
            mac_get_auth_status(puc_mac_hdr), mac_get_auth_seq_num(puc_mac_hdr));
        return OAL_SUCC;
    }

    /* ȡ����ʱ�� */
    frw_timer_immediate_destroy_timer_m(&pst_sta->st_mgmt_timer);

    /* SAE �ж�seq number�Ժ��ϴ���wpas ���� */
    hmac_rx_mgmt_send_to_host(pst_sta, pst_netbuf);

    oam_warning_log2(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
                     "{hmac_sta_wait_auth_seq4_rx::rx sae auth frame, status_code %d, seq_num %d.}",
                     mac_get_auth_status(puc_mac_hdr),
                     mac_get_auth_seq_num(puc_mac_hdr));
    pst_sta->duplicate_auth_seq4_flag = OAL_TRUE;
    return OAL_SUCC;
}


void hmac_report_ext_auth_worker(oal_work_stru *pst_sae_report_ext_auth_worker)
{
    hmac_vap_stru *pst_hmac_vap;

    pst_hmac_vap = oal_container_of(pst_sae_report_ext_auth_worker, hmac_vap_stru, st_sae_report_ext_auth_worker);
    if (pst_hmac_vap == NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{hmac_report_ext_auth_worker:: hmac_vap is null}");
        return;
    }

    oam_warning_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_CFG,
                     "{hmac_report_ext_auth_worker:: hmac_report_external_auth_req auth start}");

    hmac_report_external_auth_req(pst_hmac_vap, NL80211_EXTERNAL_AUTH_START);
}


OAL_STATIC uint32_t hmac_config_external_auth_param_check(mac_vap_stru *pst_mac_vap,
                                                          hmac_external_auth_req_stru *pst_ext_auth)
{
    if (oal_any_null_ptr2(pst_mac_vap, pst_ext_auth)) {
        oam_error_log0(0, OAM_SF_SAE,
                       "{hmac_config_external_auth_param_check::null ptr}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��legacy sta��ֱ�ӷ��� */
    if (!IS_LEGACY_STA(pst_mac_vap)) {
        oam_warning_log2(pst_mac_vap->uc_vap_id, OAM_SF_SAE,
                         "{hmac_config_external_auth_param_check::wrong vap. vap_mode %d, p2p_mode %d}",
                         pst_mac_vap->en_vap_mode,
                         pst_mac_vap->en_p2p_mode);
        return OAL_FAIL;
    }

    /* �ǹ�����bssid�� ֱ�ӷ��� */
    if (oal_memcmp(pst_mac_vap->auc_bssid, pst_ext_auth->auc_bssid, WLAN_MAC_ADDR_LEN) != 0) {
        oam_warning_log3(pst_mac_vap->uc_vap_id, OAM_SF_SAE,
                         "{hmac_config_external_auth_param_check::wrong bssid %02X:XX:XX:XX:%02X:%02X}",
                         pst_ext_auth->auc_bssid[0],
                         pst_ext_auth->auc_bssid[MAC_ADDR_4],
                         pst_ext_auth->auc_bssid[MAC_ADDR_5]);
        return OAL_FAIL;
    }

    /* �ǹ�����SSID��ֱ�ӷ��� */
    if ((pst_ext_auth->st_ssid.uc_ssid_len != OAL_STRLEN(mac_mib_get_DesiredSSID(pst_mac_vap))) ||
        oal_memcmp(mac_mib_get_DesiredSSID(pst_mac_vap),
                   pst_ext_auth->st_ssid.auc_ssid, pst_ext_auth->st_ssid.uc_ssid_len) != 0) {
        oam_warning_log2(pst_mac_vap->uc_vap_id, OAM_SF_SAE,
                         "{hmac_config_external_auth_param_check::DesiredSSID len [%d], ext_auth ssid len [%d]}",
                         OAL_STRLEN(mac_mib_get_DesiredSSID(pst_mac_vap)),
                         pst_ext_auth->st_ssid.uc_ssid_len);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}


uint32_t hmac_config_external_auth(mac_vap_stru *pst_mac_vap, uint16_t us_len, uint8_t *puc_param)
{
    uint32_t ret;
    hmac_external_auth_req_stru *pst_ext_auth;
    hmac_vap_stru *pst_hmac_vap = NULL;
    hmac_user_stru *pst_hmac_user = NULL;
    mac_cfg_kick_user_param_stru st_kick_user_param;
    hmac_auth_rsp_stru st_auth_rsp = {
        { 0 },
    };

    pst_ext_auth = (hmac_external_auth_req_stru *)puc_param;

    ret = hmac_config_external_auth_param_check(pst_mac_vap, pst_ext_auth);
    if (ret != OAL_SUCC) {
        return ret;
    }

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (pst_hmac_vap == NULL) {
        oam_warning_log1(pst_mac_vap->uc_vap_id, OAM_SF_SAE,
                         "{hmac_config_external_auth::pst_hmac_vap null, vap_idx:%d}",
                         pst_mac_vap->uc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡ�û�ָ�� */
    pst_hmac_user = mac_res_get_hmac_user(pst_hmac_vap->st_vap_base_info.us_assoc_vap_id);
    if (pst_hmac_user == NULL) {
        oam_warning_log1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
                         "hmac_config_external_auth::pst_hmac_user[%d] NULL.",
                         pst_hmac_vap->st_vap_base_info.us_assoc_vap_id);
        return OAL_FAIL;
    }

    /* ext_auth ״̬Ϊʧ������£��Ͽ����� */
    if (pst_ext_auth->us_status == MAC_UNSPEC_FAIL) {
        
        hmac_handle_connect_failed_result(pst_hmac_vap, pst_ext_auth->us_status);
        return OAL_SUCC;
    } else if (pst_ext_auth->us_status != MAC_SUCCESSFUL_STATUSCODE) {
        st_kick_user_param.us_reason_code = pst_ext_auth->us_status;
        memcpy_s(st_kick_user_param.auc_mac_addr, WLAN_MAC_ADDR_LEN, pst_mac_vap->auc_bssid, WLAN_MAC_ADDR_LEN);
        hmac_config_kick_user (pst_mac_vap, sizeof(st_kick_user_param), (uint8_t *)(&st_kick_user_param));
        return OAL_SUCC;
    }

    if (pst_hmac_vap->st_vap_base_info.en_vap_state == MAC_VAP_STATE_ROAMING) {
        hmac_roam_sae_config_reassoc_req(pst_hmac_vap);
        return OAL_SUCC;
    }

    /* ext_auth ״̬Ϊ�ɹ���ʧ��ִ��SAE���� */
    /* ȡ����ʱ�� */
    frw_timer_immediate_destroy_timer_m(&pst_hmac_vap->st_mgmt_timer);

    /* ��״̬����ΪAUTH_COMP */
    hmac_fsm_change_state(pst_hmac_vap, MAC_VAP_STATE_STA_AUTH_COMP);
    st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

    /* �ϱ�SME��֤�ɹ���ִ�й��� */
    hmac_send_rsp_to_sme_sta(pst_hmac_vap, HMAC_SME_AUTH_RSP, (uint8_t *)&st_auth_rsp);

    return OAL_SUCC;
}


OAL_STATIC oal_bool_enum_uint8 is_sae_connect_with_pmkid(uint8_t *puc_rsn_ie, uint8_t uc_rsn_ie_len)
{
    /*************************************************************************/
    /*                  RSN Element Format                                   */
    /* --------------------------------------------------------------------- */
    /* |Element ID | Length | Version | Group Cipher Suite | Pairwise Suite  */
    /* --------------------------------------------------------------------- */
    /* |     1     |    1   |    2    |      0 or 4        |     0 or 2      */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /*  Count| Pairwise Cipher Suite List | AKM Suite Count | AKM Suite List */
    /* --------------------------------------------------------------------- */
    /*       |       0 or 4*m             |     0 or 2      |   0 or 4*n     */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* |RSN Capabilities|PMKID Count|PMKID List|Group Management Cipher Suite */
    /* --------------------------------------------------------------------- */
    /* |      0 or 2    |   0 or 2  | 0 or 16 *s  |          0 or 4        | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /*************************************************************************/
    /* ����wpa_s�·�������rsn_ie�����鲥�����׼������������׼�����Կ�����׼���RSN capability��
     * ���Բ��ÿ���û�������׼��ĳ���
     */
    struct hmac_rsn_connect_pmkid_stru {
        uint8_t uc_eid;
        uint8_t uc_ie_len;
        uint16_t us_rsn_ver;
        uint32_t group_cipher;
        uint16_t us_pairwise_cipher_cnt;
        uint32_t pairwise_cipher;
        uint16_t us_akm_cnt;
        uint32_t akm;
        uint16_t us_rsn_cap;
        uint16_t us_pmkid_cnt;
        uint8_t auc_pmkid[WLAN_PMKID_LEN];
    };

    /* RSN���ȹ��̣�������PMKID */
    if (uc_rsn_ie_len < sizeof(struct hmac_rsn_connect_pmkid_stru)) {
        return OAL_FALSE;
    }

    return OAL_TRUE;
}


void hmac_update_sae_connect_param(hmac_vap_stru *pst_hmac_vap, uint8_t *puc_ie, uint32_t ie_len)
{
    uint8_t *puc_rsn_ie = NULL;
    mac_crypto_settings_stru st_crypto = {0};
    oal_bool_enum_uint8 en_sae_connect_with_pmkid;

    pst_hmac_vap->bit_sae_connect_with_pmkid = OAL_FALSE;
    pst_hmac_vap->en_sae_connect = OAL_FALSE;

    if (puc_ie == NULL) {
        return;
    }

    /* wpa_s�·�SAE������ȻЯ��RSN IE */
    puc_rsn_ie = mac_find_ie(MAC_EID_RSN, puc_ie, ie_len);
    if (puc_rsn_ie == NULL) {
        return;
    }

    mac_ie_get_rsn_cipher(puc_rsn_ie, &st_crypto);
    if (st_crypto.aul_akm_suite[0] != MAC_RSN_AKM_SAE && st_crypto.aul_akm_suite[0] != MAC_RSN_AKM_TBPEKE) {
        oam_warning_log1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
            "aul_akm_suite=0x%x", st_crypto.aul_akm_suite[0]);
        return ;
    }
    pst_hmac_vap->en_sae_connect = OAL_TRUE;

    en_sae_connect_with_pmkid = is_sae_connect_with_pmkid(puc_rsn_ie, puc_rsn_ie[1]);
    if ((mac_mib_get_AuthenticationMode(&(pst_hmac_vap->st_vap_base_info)) == WLAN_WITP_AUTH_SAE ||
        mac_mib_get_AuthenticationMode(&(pst_hmac_vap->st_vap_base_info)) == WLAN_WITP_AUTH_TBPEKE) &&
        (en_sae_connect_with_pmkid == OAL_TRUE)) {
        oam_error_log0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
                       "hmac_update_sae_connect_param:: auth_type=SAE with PMKID=1");
        return ;
    }

    /* ��RSN IE�в���Я��PMKID���ݣ�������sae_connect_with_pmkid = true;
     * δЯ��PMKID���ݣ�����sae_connect_with_pmkid = false
     */
    pst_hmac_vap->bit_sae_connect_with_pmkid = en_sae_connect_with_pmkid;

    if (st_crypto.aul_akm_suite[0] == MAC_RSN_AKM_SAE) {
        oam_warning_log2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
                         "hmac_update_sae_connect_param::use sae connect[%d]. with PMKID [%d]",
                         pst_hmac_vap->en_sae_connect, pst_hmac_vap->bit_sae_connect_with_pmkid);
    } else if (st_crypto.aul_akm_suite[0] == MAC_RSN_AKM_TBPEKE) {
        oam_warning_log2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SAE,
                         "hmac_update_sae_connect_param::use tbpeke connect[%d]. with PMKID [%d]",
                         pst_hmac_vap->en_sae_connect, pst_hmac_vap->bit_sae_connect_with_pmkid);
    }
}
#endif
