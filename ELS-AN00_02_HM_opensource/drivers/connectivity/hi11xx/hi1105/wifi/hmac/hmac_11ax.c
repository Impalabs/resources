

#include "oal_ext_if.h"
#include "mac_resource.h"
#include "mac_ie.h"
#include "securec.h"
#include "hmac_11ax.h"
#include "hmac_wifi6_self_cure.h"
#include "hmac_hcc_adapt.h"
#include "hmac_hiex.h"
#include "hmac_scan.h"
#include "hmac_p2p.h"
#include "mac_mib.h"
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_11AX_C

#if defined(_PRE_WLAN_FEATURE_11AX)
typedef struct {
    int8_t chan_offset;
    uint8_t bandwidth;
} hmac_channel_bandwidth_map;

static hmac_channel_bandwidth_map g_channel_bandwidth_map_80mhz[] = {
    { CHAN_OFFSET_PLUS_6,   WLAN_BAND_WIDTH_80PLUSPLUS },
    { CHAN_OFFSET_MINUS_2,  WLAN_BAND_WIDTH_80PLUSMINUS },
    { CHAN_OFFSET_PLUS_2,   WLAN_BAND_WIDTH_80MINUSPLUS },
    { CHAN_OFFSET_MINUS_6,  WLAN_BAND_WIDTH_80MINUSMINUS },
};

static hmac_channel_bandwidth_map g_channel_bandwidth_map_160mhz[] = {
    { CHAN_OFFSET_PLUS_14,  WLAN_BAND_WIDTH_160PLUSPLUSPLUS },
    { CHAN_OFFSET_MINUS_2,  WLAN_BAND_WIDTH_160PLUSPLUSMINUS },
    { CHAN_OFFSET_PLUS_6,   WLAN_BAND_WIDTH_160PLUSMINUSPLUS },
    { CHAN_OFFSET_MINUS_10, WLAN_BAND_WIDTH_160PLUSMINUSMINUS },
    { CHAN_OFFSET_PLUS_10,  WLAN_BAND_WIDTH_160MINUSPLUSPLUS },
    { CHAN_OFFSET_MINUS_6,  WLAN_BAND_WIDTH_160MINUSPLUSMINUS },
    { CHAN_OFFSET_PLUS_2,   WLAN_BAND_WIDTH_160MINUSMINUSPLUS },
    { CHAN_OFFSET_MINUS_14, WLAN_BAND_WIDTH_160MINUSMINUSMINUS },
};


OAL_STATIC uint32_t hmac_update_he_opern_ie_sta(mac_vap_stru *pst_mac_vap,
                                                hmac_user_stru *pst_hmac_user,
                                                uint8_t *puc_payload)
{
    mac_user_stru *pst_mac_user = NULL;

    if (oal_any_null_ptr3(pst_mac_vap, pst_hmac_user, puc_payload)) {
        oam_error_log0(0, OAM_SF_ASSOC,
            "{hmac_update_he_opern_ie_sta::pst_mac_vap or pst_hmac_user or puc_payload is null!}");
        return MAC_NO_CHANGE;
    }

    /* ֧��11ax���Ž��к����Ĵ��� */
    if (OAL_FALSE == mac_mib_get_HEOptionImplemented(pst_mac_vap)) {
        return MAC_NO_CHANGE;
    }

    pst_mac_user = &(pst_hmac_user->st_user_base_info);
    return mac_ie_proc_he_opern_ie(pst_mac_vap, puc_payload, pst_mac_user);
}


OAL_STATIC void hmac_sta_up_update_he_edca_params_mib(hmac_vap_stru *pst_hmac_sta,
                                                      mac_frame_he_mu_ac_parameter *pst_ac_param)
{
    uint8_t uc_aci;
    uint8_t uc_mu_edca_timer;
    uint32_t txop_limit;

    /*      AC Parameters Record Format           */
    /* ------------------------------------------ */
    /* |     1     |       1       |      1     | */
    /* ------------------------------------------ */
    /* | ACI/AIFSN | ECWmin/ECWmax | MU EDCA Timer | */
    /* ------------------------------------------ */
    /************* ACI/AIFSN Field ***************/
    /*     ---------------------------------- */
    /* bit |   4   |  1  |  2  |    1     |   */
    /*     ---------------------------------- */
    /*     | AIFSN | ACM | ACI | Reserved |   */
    /*     ---------------------------------- */
    uc_aci = pst_ac_param->bit_ac_index;
    ;

    /* ECWmin/ECWmax Field     */
    /*     ------------------- */
    /* bit |   4    |   4    | */
    /*     ------------------- */
    /*     | ECWmin | ECWmax | */
    /*     ------------------- */
    /* ��mib���кͼĴ����ﱣ���TXOPֵ������ 8TU Ϊ��λ��
       ==MU EDCA Timer ��λ��8TU --8 * 1024us
    */
    uc_mu_edca_timer = pst_ac_param->uc_mu_edca_timer;
    txop_limit = (uint32_t)(uc_mu_edca_timer);

    /* ������Ӧ��MIB����Ϣ */
    if (uc_aci < WLAN_WME_AC_BUTT) {
        mac_mib_set_QAPMUEDCATableIndex(&pst_hmac_sta->st_vap_base_info, uc_aci, uc_aci + 1); /* ע: Э��涨ȡֵ1 2 3 4 */
        mac_mib_set_QAPMUEDCATableCWmin(&pst_hmac_sta->st_vap_base_info, uc_aci, pst_ac_param->bit_ecw_min);
        mac_mib_set_QAPMUEDCATableCWmax(&pst_hmac_sta->st_vap_base_info, uc_aci, pst_ac_param->bit_ecw_max);
        mac_mib_set_QAPMUEDCATableAIFSN(&pst_hmac_sta->st_vap_base_info, uc_aci, pst_ac_param->bit_aifsn);
        mac_mib_set_QAPMUEDCATableTXOPLimit(&pst_hmac_sta->st_vap_base_info, uc_aci, txop_limit);
        mac_mib_set_QAPMUEDCATableMandatory(&pst_hmac_sta->st_vap_base_info, uc_aci, pst_ac_param->bit_acm);
    }
}


void hmac_sta_up_update_he_edca_params(uint8_t *puc_payload,
                                       uint16_t us_msg_len,
                                       hmac_vap_stru *pst_hmac_sta,
                                       uint8_t uc_frame_sub_type,
                                       hmac_user_stru *pst_hmac_user)
{
    uint8_t uc_ac_num_loop;
    uint32_t ret;
    mac_device_stru *pst_mac_device = NULL;
    uint8_t *puc_ie = NULL;
    mac_frame_he_mu_edca_parameter_ie_stru st_mu_edca_value;

    memset_s(&st_mu_edca_value, sizeof(mac_frame_he_mu_edca_parameter_ie_stru),
             0, sizeof(mac_frame_he_mu_edca_parameter_ie_stru));
    if (OAL_FALSE == mac_mib_get_HEOptionImplemented(&(pst_hmac_sta->st_vap_base_info))) {
        return;
    }

    if (pst_hmac_sta->uc_disable_mu_edca) {
        return;
    }

    pst_mac_device = (mac_device_stru *)mac_res_get_dev(pst_hmac_sta->st_vap_base_info.uc_device_id);
    if (pst_mac_device == NULL) {
        oam_warning_log1(0, OAM_SF_ASSOC,
                         "{hmac_sta_up_update_he_edca_params::mac_res_get_dev fail.device_id[%u]!}",
                         pst_hmac_sta->st_vap_base_info.uc_device_id);
        return;
    }

    /************************ MU EDCA Parameter Set Element ***************************/
    /* ------------------------------------------------------------------------------------------- */
    /* | EID | LEN | Ext EID|MU Qos Info |MU AC_BE Parameter Record | MU AC_BK Parameter Record  | */
    /* ------------------------------------------------------------------------------------------- */
    /* |  1  |  1  |   1    |    1       |     3                    |        3                   | */
    /* ------------------------------------------------------------------------------------------- */
    /* ------------------------------------------------------------------------------ ------------- */
    /* | MU AC_VI Parameter Record | MU AC_VO Parameter Record                                   | */
    /* ------------------------------------------------------------------------------------------- */
    /* |    3                      |     3                                                       | */
    /******************* QoS Info field when sent from WMM AP *****************/
    /* -------------------------------------------------------------------------------------------- */
    /*    | EDCA Parameter Set Update Count | Q-Ack | Queue Request |TXOP Request | More Data Ack| */
    /* --------------------------------------------------------------------------------------------- */
    /* bit |        0~3                      |  1    |  1            |   1         |     1        | */
    /* --------------------------------------------------------------------------------------------- */
    /**************************************************************************/
    puc_ie = mac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_EDCA, puc_payload, us_msg_len);
    if (puc_ie != NULL) {
        ret = mac_ie_parse_mu_edca_parameter(puc_ie, &st_mu_edca_value);
        if (ret != OAL_SUCC) {
            return;
        }

        /* ����յ�����beacon֡������param_set_countû�иı�
           ��STAҲ�������κθı䣬ֱ�ӷ��ؼ��� */
        if ((uc_frame_sub_type == WLAN_FC0_SUBTYPE_BEACON) &&
            (st_mu_edca_value.st_qos_info.bit_edca_update_count ==
             pst_hmac_sta->st_vap_base_info.uc_he_mu_edca_update_count)) {
            return;
        }

        pst_hmac_sta->st_vap_base_info.uc_he_mu_edca_update_count = st_mu_edca_value.st_qos_info.bit_edca_update_count;

        /* ���ÿһ��AC������EDCA���� */
        for (uc_ac_num_loop = 0; uc_ac_num_loop < WLAN_WME_AC_BUTT; uc_ac_num_loop++) {
            hmac_sta_up_update_he_edca_params_mib(pst_hmac_sta,
                &(st_mu_edca_value.ast_mu_ac_parameter[uc_ac_num_loop]));
        }

        /* ����EDCA��ص�MAC�Ĵ��� */
        hmac_sta_up_update_mu_edca_params_machw(pst_hmac_sta, MAC_WMM_SET_PARAM_TYPE_UPDATE_EDCA);
        return;
    }
}


void hmac_sta_up_update_he_nfrp_params(uint8_t *payload,
    uint16_t msg_len, hmac_vap_stru *hmac_sta, uint8_t frame_sub_type, hmac_user_stru *hmac_user)
{
    uint8_t tmp_nfrp_thr_exp = 0;
    mac_user_stru *mac_user = &(hmac_user->st_user_base_info);
    frw_event_mem_stru *event_mem = NULL;
    frw_event_stru *event = NULL;
    uint8_t *ie = NULL;

    if (IS_LEGACY_STA(&(hmac_sta->st_vap_base_info)) == OAL_FALSE) {
        return;
    }

    if (mac_user == NULL) {
        oam_error_log0(0, OAM_SF_ASSOC, "{hmac_sta_up_update_he_nfrp_params::mac_user NULL!}");
        return;
    }

    if (mac_mib_get_HEOptionImplemented(&(hmac_sta->st_vap_base_info)) == OAL_FALSE) {
        return;
    }

    /************ NDP Feedback Report Parameter Set Element ****************/
    /* ------------------------------------------------------------------- */
    /* | EID | LEN | Ext EID|Resource Request Buffer Threshold Exponent |  */
    /* ------------------------------------------------------------------- */
    /* |  1  |  1  |   1    |                     1                     |  */
    /* ------------------------------------------------------------------- */
    ie = mac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_NDP_FEEDBACK_REPORT, payload, msg_len);
    if (ie == NULL) {
        return;
    }

    if (mac_ie_parse_he_ndp_feedback_report_ie(ie, &tmp_nfrp_thr_exp) != OAL_SUCC) {
        return;
    }

    /* ����յ�����beacon֡������uc_resource_req_buff_threshold_expû�иı�,
       ��STAҲ�����κθı䣬ֱ�ӷ��ؼ��� */
    if ((frame_sub_type == WLAN_FC0_SUBTYPE_BEACON) && (mac_user->uc_res_req_buff_threshold_exp == tmp_nfrp_thr_exp)) {
        return;
    }
    mac_user->uc_res_req_buff_threshold_exp = tmp_nfrp_thr_exp;

    /* ���¼���dmac����resource request buffer threshold exponent��ֵ */
    event_mem = frw_event_alloc_m(sizeof(uint8_t));
    if (event_mem == NULL) {
        oam_error_log1(hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_up_update_he_nfrp_params::event_mem alloc null, size[%d].}", sizeof(uint8_t));
        return;
    }

    /* ��д�¼� */
    event = frw_get_event_stru(event_mem);
    frw_event_hdr_init(&(event->st_event_hdr), FRW_EVENT_TYPE_WLAN_CTX,
        DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_NDP_FEEDBACK_REPORT_REG,
        sizeof(uint8_t), FRW_EVENT_PIPELINE_STAGE_1, hmac_sta->st_vap_base_info.uc_chip_id,
        hmac_sta->st_vap_base_info.uc_device_id, hmac_sta->st_vap_base_info.uc_vap_id);

    /* �������� */
    if (memcpy_s(frw_get_event_payload(event_mem), sizeof(uint8_t),
        &tmp_nfrp_thr_exp, sizeof(uint8_t)) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "hmac_sta_up_update_he_nfrp_params::memcpy fail!");
        frw_event_free_m(event_mem);
        return;
    }

    /* �ַ��¼� */
    frw_event_dispatch_event(event_mem);
    frw_event_free_m(event_mem);

    return;
}


uint32_t hmac_sta_up_update_mu_edca_params_machw(hmac_vap_stru *pst_hmac_sta,
                                                 mac_wmm_set_param_type_enum_uint8 en_type)
{
    frw_event_mem_stru *pst_event_mem = NULL;
    frw_event_stru *pst_event = NULL;
    dmac_ctx_sta_asoc_set_edca_reg_stru st_asoc_set_edca_reg_param = { 0 };
    dmac_ctx_sta_asoc_set_edca_reg_stru *pst_edca_para = NULL;

    /* ���¼���dmacд�Ĵ��� */
    /* �����¼��ڴ� */
    pst_event_mem = frw_event_alloc_m(sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru));
    if (pst_event_mem == NULL) {
        oam_error_log1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_up_update_mu_edca_params_machw::event_mem alloc null, size[%d].}",
                       sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru));
        return OAL_ERR_CODE_PTR_NULL;
    }

    st_asoc_set_edca_reg_param.uc_vap_id = pst_hmac_sta->st_vap_base_info.uc_vap_id;
    st_asoc_set_edca_reg_param.en_set_param_type = en_type;

    /* ��д�¼� */
    pst_event = frw_get_event_stru(pst_event_mem);
    frw_event_hdr_init(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_MU_EDCA_REG,
                       sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_hmac_sta->st_vap_base_info.uc_chip_id,
                       pst_hmac_sta->st_vap_base_info.uc_device_id,
                       pst_hmac_sta->st_vap_base_info.uc_vap_id);

    memcpy_s((uint8_t *)&st_asoc_set_edca_reg_param.ast_wlan_mib_qap_edac,
             (sizeof(wlan_mib_Dot11QAPEDCAEntry_stru) * WLAN_WME_AC_BUTT),
             (uint8_t *)pst_hmac_sta->st_vap_base_info.st_wlan_mib_mu_edca.ast_wlan_mib_qap_edac,
             (sizeof(wlan_mib_Dot11QAPEDCAEntry_stru) * WLAN_WME_AC_BUTT));

    /* �������� */
    pst_edca_para = (dmac_ctx_sta_asoc_set_edca_reg_stru *)frw_get_event_payload(pst_event_mem);
    memcpy_s(pst_edca_para, sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru),
             (uint8_t *)&st_asoc_set_edca_reg_param, sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru));

    /* �ַ��¼� */
    frw_event_dispatch_event(pst_event_mem);
    frw_event_free_m(pst_event_mem);

    return OAL_SUCC;
}


void hmac_sta_up_update_assoc_rsp_sr_params(uint8_t *puc_payload,
                                            uint16_t us_msg_len,
                                            hmac_vap_stru *pst_hmac_sta,
                                            uint8_t uc_frame_sub_type,
                                            hmac_user_stru *pst_hmac_user)
{
    uint32_t ret;
    uint8_t *puc_ie = NULL;
    mac_frame_he_spatial_reuse_parameter_set_ie_stru st_sr_ie;
    frw_event_mem_stru *pst_event_mem = NULL;
    frw_event_stru *pst_event = NULL;
    dmac_ctx_sta_asoc_set_sr_reg_stru *pst_sr_reg_param = NULL;

    if (OAL_FALSE == mac_mib_get_HEOptionImplemented(&(pst_hmac_sta->st_vap_base_info))) {
        return;
    }

    puc_ie = mac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_SRP, puc_payload, us_msg_len);
    if (puc_ie == NULL) {
        return;
    }

    memset_s(&st_sr_ie, sizeof(st_sr_ie), 0, sizeof(st_sr_ie));
    ret = mac_ie_parse_spatial_reuse_parameter(puc_ie, &st_sr_ie);
    if (ret != OAL_SUCC) {
        return;
    }

    /* ���¼���dmacд�Ĵ��� */
    /* �����¼��ڴ� */
    pst_event_mem = frw_event_alloc_m(sizeof(dmac_ctx_sta_asoc_set_sr_reg_stru));
    if (pst_event_mem == NULL) {
        oam_error_log1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_up_update_assoc_rsp_sr_params::event_mem alloc null, size[%d].}",
                       sizeof(dmac_ctx_sta_asoc_set_edca_reg_stru));
        return;
    }

    /* ��д�¼� */
    pst_event = frw_get_event_stru(pst_event_mem);
    frw_event_hdr_init(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_SPATIAL_REUSE_REG,
                       sizeof(dmac_ctx_sta_asoc_set_sr_reg_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_hmac_sta->st_vap_base_info.uc_chip_id,
                       pst_hmac_sta->st_vap_base_info.uc_device_id,
                       pst_hmac_sta->st_vap_base_info.uc_vap_id);
    pst_sr_reg_param = (dmac_ctx_sta_asoc_set_sr_reg_stru *)frw_get_event_payload(pst_event_mem);

    memcpy_s(&pst_sr_reg_param->st_sr_ie, sizeof(mac_frame_he_spatial_reuse_parameter_set_ie_stru),
             (uint8_t *)&st_sr_ie, sizeof(mac_frame_he_spatial_reuse_parameter_set_ie_stru));

    /* �ַ��¼� */
    frw_event_dispatch_event(pst_event_mem);
    frw_event_free_m(pst_event_mem);

    return;
}


uint32_t hmac_proc_he_uora_para_update(mac_vap_stru *mac_vap, uint8_t uora_para_change)
{
    uint32_t ret;

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_HE_UORA_OCW_UPDATE,
                                 sizeof(uint8_t), (uint8_t *)&uora_para_change);
    if (ret != OAL_SUCC) {
        oam_error_log1(mac_vap->uc_vap_id, OAM_SF_11AX,
                       "{hmac_proc_he_uora_para_update::send event[WLAN_CFGID_UORA_OCW_UPDATE] failed[%d].}", ret);
    }

    return ret;
}

uint32_t hmac_ie_check_he_sta(hmac_vap_stru *pst_hmac_sta,
                              hmac_user_stru *pst_hmac_user,
                              uint8_t *puc_payload,
                              uint16_t us_msg_len)
{
    mac_user_stru *pst_mac_user;
    mac_vap_stru *pst_mac_vap;
    mac_he_hdl_stru *pst_he_hdl;
    uint8_t *puc_tmp_ie = NULL;
    uint32_t change = 0;
    oal_bool_enum_uint8 uora_para_change;

    pst_mac_user = &(pst_hmac_user->st_user_base_info);
    pst_mac_vap  = &(pst_hmac_sta->st_vap_base_info);

    pst_he_hdl = MAC_USER_HE_HDL_STRU(pst_mac_user);
    memset_s(pst_he_hdl, sizeof(mac_he_hdl_stru), 0, sizeof(mac_he_hdl_stru));
    puc_tmp_ie = mac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_CAP, puc_payload, us_msg_len);
    if (puc_tmp_ie != NULL) {
        hmac_proc_he_cap_ie(pst_mac_vap, pst_hmac_user, puc_tmp_ie);
    }

    puc_tmp_ie = mac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_OPERATION, puc_payload, us_msg_len);
    if (puc_tmp_ie != NULL) {
        change |= hmac_update_he_opern_ie_sta(pst_mac_vap, pst_hmac_user, puc_tmp_ie);
    }

    /* ����BSS Coloe change announcement IE  */
    puc_tmp_ie = mac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_BSS_COLOR_CHANGE_ANNOUNCEMENT,
                                    puc_payload, us_msg_len);
    if (puc_tmp_ie != NULL) {
        hmac_proc_he_bss_color_change_announcement_ie(pst_mac_vap, pst_hmac_user, puc_tmp_ie);
    }

    uora_para_change = mac_proc_he_uora_parameter_set_ie(pst_mac_vap, puc_payload, us_msg_len);
    if (uora_para_change == OAL_TRUE) {
        hmac_proc_he_uora_para_update(pst_mac_vap, OAL_TRUE);
    }

    return change;
}

void hmac_mgmt_proc_he_uora(mac_vap_stru *mac_vap, hmac_scanned_bss_info *scaned_bss)
{
    if ((g_wlan_spec_cfg->feature_11ax_uora_is_supported == OAL_TRUE) && IS_LEGACY_STA(mac_vap)) {
        if (scaned_bss->st_bss_dscr_info.en_he_uora == OAL_TRUE) {
            hmac_proc_he_uora_para_update(mac_vap, OAL_TRUE);
        } else {
            hmac_proc_he_uora_para_update(mac_vap, OAL_FALSE);
        }
    }
}


OAL_STATIC void hmac_set_he_capabilities_punctured_preamble(uint8_t *puc_buffer,
    oal_bool_enum_uint8 en_punctured_preamble)
{
    mac_frame_he_phy_cap_stru *pst_he_phy_capinfo = NULL;

    /* ��ȡHE PHY Capabilities Information ����Ϣ */
    puc_buffer += MAC_IE_HDR_LEN + 1 + MAC_HE_MAC_CAP_LEN;
    pst_he_phy_capinfo = (mac_frame_he_phy_cap_stru *)puc_buffer;
    /* ���punctured_preamble�ֶ� */
    if (en_punctured_preamble == OAL_TRUE) {
        pst_he_phy_capinfo->bit_punctured_preamble_rx = PUNCTURED_PREAMBLE_RX_EN; /* punctured preambleʹ�� */
    } else {
        pst_he_phy_capinfo->bit_punctured_preamble_rx = 0; /* 0����punctured preambleδʹ�� */
    }
}


uint8_t hmac_set_he_capabilities_all_ie(hmac_scanned_bss_info *scaned_bss, mac_vap_stru *mac_vap,
    uint8_t *buffer)
{
    uint8_t ie_len = 0;
    /* ��ȫ��he capabilities��Ϣ������� */
    mac_set_he_capabilities_ie(mac_vap, buffer, &ie_len);

    /* ��HE PHY Capabilities Information����Ϣ�е�punctured_preamble�ֶ�������� */
    if (g_wlan_spec_cfg->feature_hiex_is_open) {
        hmac_set_he_capabilities_punctured_preamble(buffer, scaned_bss->st_bss_dscr_info.en_punctured_preamble);
    }
    if (scaned_bss->st_bss_dscr_info.st_channel.ext6g_band == OAL_TRUE) {
        buffer += ie_len;
        ie_len += mac_set_he_6ghz_band_cap_ie(mac_vap, buffer);
    }
    return ie_len;
}

uint32_t hmac_config_ul_ofdma_amsdu(mac_vap_stru *pst_mac_vap, uint8_t uc_len, uint8_t *puc_param)
{
    pst_mac_vap->bit_ofdma_aggr_num = *puc_param;

    return OAL_SUCC;
}

uint32_t _hmac_config_ul_ofdma_amsdu(mac_vap_stru *pst_mac_vap, uint8_t uc_len, uint8_t *puc_param)
{
    if (g_wlan_spec_cfg->feature_11ax_is_open) {
        return hmac_config_ul_ofdma_amsdu(pst_mac_vap, uc_len, puc_param);
    }

    return OAL_SUCC;
}

OAL_STATIC uint32_t hmac_config_set_send_tb_ppdu_flag(mac_vap_stru *pst_mac_vap,
    uint8_t uc_len, uint8_t *puc_param)
{
    uint8_t uc_flag;

    uc_flag = *puc_param;
    if (uc_flag == OAL_TRUE && IS_LEGACY_STA(pst_mac_vap)) {
        hmac_arp_probe_type_set(pst_mac_vap, OAL_FALSE, HMAC_VAP_ARP_PROBE_TYPE_HTC);
    }
    return OAL_SUCC;
}
uint32_t _hmac_config_set_send_tb_ppdu_flag(mac_vap_stru *pst_mac_vap,
    uint8_t uc_len, uint8_t *puc_param)
{
    if (g_wlan_spec_cfg->feature_11ax_is_open) {
        return hmac_config_set_send_tb_ppdu_flag(pst_mac_vap, uc_len, puc_param);
    }

    return OAL_SUCC;
}


uint32_t hmac_config_ax_debug(mac_vap_stru *pst_mac_vap, uint16_t us_len, uint8_t *puc_param)
{
    uint8_t uc_cmd_cnt;
    uint8_t uc_cmd_index = 0;
    uint16_t uc_set_id;
    uint32_t value;
    mac_cfg_set_str_stru *pst_set_cmd = NULL;
    hmac_vap_stru *hmac_vap;

    hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_set_cmd = (mac_cfg_set_str_stru *)puc_param;
    uc_cmd_cnt = pst_set_cmd->uc_cmd_cnt;
    if (uc_cmd_cnt > MAC_STR_CMD_MAX_SIZE) {
        oam_error_log1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_11ax_debug::cnt err[%d].}", uc_cmd_cnt);
        return OAL_ERR_CODE_PTR_NULL;
    }

    while (uc_cmd_cnt > 0) {
        uc_set_id = pst_set_cmd->us_set_id[uc_cmd_index];
        value = pst_set_cmd->value[uc_cmd_index];

        if (uc_set_id == MAC_VAP_11AX_DEBUG_MAC_PADDING) {
            hmac_vap->st_vap_base_info.bit_mac_padding = value;
        } else if (uc_set_id == MAC_VAP_11AX_DEBUG_HE_TB_PPDU_HTC_ORGER) {
            hmac_vap->uc_htc_order_flag  = value;
        } else if (uc_set_id == MAC_VAP_11AX_DEBUG_HE_TB_PPDU_HTC_VALUE) {
            hmac_vap->htc_info        = value;
        } else if(uc_set_id == MAC_VAP_11AX_DEBUG_DISABLE_MU_EDCA) {
            hmac_vap->uc_disable_mu_edca = value;
        } else if (uc_set_id == MAC_VAP_11AX_DEBUG_WIFI6_SELF_CURE_FLAG) {
            hmac_wifi6_self_cure_set_handle_type(value);
        }

        oam_warning_log2(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_11ax_debug:set id[%d] set data[%d].}",
                         uc_set_id, value);

        uc_cmd_index++;
        uc_cmd_cnt--;
    }

    return OAL_SUCC;
}


oal_bool_enum_uint8 mac_hiex_ie_get_bit_chip_type(uint8_t *frame_body, uint16_t us_frame_len)
{
    mac_hiex_vendor_ie_stru *hiex_ie = NULL;
    hiex_ie = (mac_hiex_vendor_ie_stru *)mac_find_vendor_ie(MAC_WLAN_OUI_HUAWEI,
        MAC_HISI_HIEX_IE, frame_body, (uint32_t)us_frame_len);
    /* ��ֻ��1152AP����punctured preamble���� */
    if (hiex_ie != NULL && hiex_ie->cap.bit_chip_type == MAC_HIEX_CHIP_TYPE_HI1152) {
        return OAL_TRUE;
    }
    return OAL_FALSE;
}


OAL_STATIC void hmac_scan_update_bw_cap(mac_frame_he_cap_ie_stru *pst_he_cap_value,
    mac_bss_dscr_stru *pst_bss_dscr)
{
    pst_bss_dscr->en_bw_cap = WLAN_BW_CAP_20M;
    if (pst_bss_dscr->st_channel.en_band == WLAN_BAND_2G) {              /* 2G */
        if (pst_he_cap_value->st_he_phy_cap.bit_channel_width_set & 0x1) { /* Bit0 2G �Ƿ�֧��40MHz */
            pst_bss_dscr->en_bw_cap = WLAN_BW_CAP_40M;
        }
    } else {
        if (pst_he_cap_value->st_he_phy_cap.bit_channel_width_set & 0x8) { /* B3-5G 80+80MHz */
            pst_bss_dscr->en_bw_cap = WLAN_BW_CAP_80PLUS80;
        } else if (pst_he_cap_value->st_he_phy_cap.bit_channel_width_set & 0x4) { /* B2-160MHz */
            pst_bss_dscr->en_bw_cap = WLAN_BW_CAP_160M;
        } else if (pst_he_cap_value->st_he_phy_cap.bit_channel_width_set & 0x2) { /* B2-5G֧��80MHz */
            pst_bss_dscr->en_bw_cap = WLAN_BW_CAP_80M;
        } else {
            pst_bss_dscr->en_bw_cap = WLAN_BW_CAP_20M;
        }
    }
}

OAL_STATIC void hmac_scan_update_bss_list_11ax_he_cap(mac_bss_dscr_stru *bss_dscr,
                                                      uint8_t *frame_body,
                                                      uint16_t frame_len)
{
    uint8_t *ie;
    mac_frame_he_cap_ie_stru he_cap_ie;
    uint32_t ret;

    /* HE CAP */
    ie = mac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_CAP, frame_body, frame_len);
    if ((ie != NULL) && (ie[1] >= MAC_HE_CAP_MIN_LEN)) {
        memset_s(&he_cap_ie, sizeof(he_cap_ie), 0, sizeof(he_cap_ie));
        /* �����̶����Ȳ���:MAC_Cap + PHY Cap + HE-MCS NSS(<=80MHz) */
        ret = mac_ie_parse_he_cap(ie, &he_cap_ie);
        if (ret != OAL_SUCC) {
            return;
        }

        bss_dscr->en_he_capable = OAL_TRUE; /* ֧��HE */

        bss_dscr->en_he_uora = (he_cap_ie.st_he_mac_cap.bit_ofdma_ra_support & 0x1) ? OAL_TRUE : OAL_FALSE;

        hmac_scan_update_bw_cap(&he_cap_ie, bss_dscr);

        /* ����Զ˲�֧��ldpc������Ϊ���ǽ�Э����11ac */
        if ((he_cap_ie.st_he_phy_cap.bit_ldpc_coding_in_paylod == OAL_FALSE) &&
            (!(g_optimized_feature_switch_bitmap & BIT(CUSTOM_OPTIMIZE_11AX20M_NON_LDPC)) ||
            (bss_dscr->en_bw_cap > WLAN_BW_CAP_20M))) {
            bss_dscr->en_he_capable = OAL_FALSE;
        }

        /* �����û�֧�ַ��͵�txbf������ */
        bss_dscr->uc_num_sounding_dim = (bss_dscr->en_bw_cap <= WLAN_BW_CAP_80M) ?
            he_cap_ie.st_he_phy_cap.bit_below_80mhz_sounding_dimensions_num :
            he_cap_ie.st_he_phy_cap.bit_over_80mhz_sounding_dimensions_num;
    }
}

static uint8_t hmac_scan_get_bandwidth(hmac_channel_bandwidth_map *channel_bandwidth_map,
                                       uint8_t map_size, int8_t chan_off)
{
    uint8_t i;
    hmac_channel_bandwidth_map *map_temp = channel_bandwidth_map;
    for (i = 0; i < map_size; i++) {
        map_temp = channel_bandwidth_map + i;
        if (chan_off == map_temp->chan_offset) {
            return map_temp->bandwidth;
        }
    }
    return WLAN_BAND_WIDTH_BUTT;
}


OAL_STATIC void hmac_scan_update_bss_list_11ax(mac_bss_dscr_stru *bss_dscr, uint8_t *frame_body, uint16_t frame_len)
{
    uint8_t *ie = NULL;
    int8_t chan_off;
    uint8_t bandwidth = WLAN_BAND_WIDTH_BUTT;
    mac_frame_he_oper_ie_stru he_oper_ie_value;

    hmac_scan_update_bss_list_11ax_he_cap(bss_dscr, frame_body, frame_len);

    /* HE Oper */
    ie = mac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_OPERATION, frame_body, frame_len);
    if (!((ie != NULL) && (ie[1] >= MAC_HE_OPERAION_MIN_LEN))) {
        return;
    }

    memset_s(&he_oper_ie_value, sizeof(he_oper_ie_value), 0, sizeof(he_oper_ie_value));
    if (mac_ie_parse_he_oper(ie, &he_oper_ie_value) != OAL_SUCC) {
        return;
    }

    if (he_oper_ie_value.st_he_oper_param.bit_vht_operation_info_present != 1) {
        return;
    }

    chan_off = he_oper_ie_value.st_vht_operation_info.uc_center_freq_seg0 - bss_dscr->st_channel.uc_chan_number;
    if (he_oper_ie_value.st_vht_operation_info.uc_channel_width == WLAN_MIB_VHT_OP_WIDTH_80) { /* 80MHz */
        bandwidth = hmac_scan_get_bandwidth(g_channel_bandwidth_map_80mhz,
            sizeof(g_channel_bandwidth_map_80mhz) / sizeof(hmac_channel_bandwidth_map), chan_off);
    }
#ifdef _PRE_WLAN_FEATURE_160M
    if (he_oper_ie_value.st_vht_operation_info.uc_channel_width == WLAN_MIB_VHT_OP_WIDTH_160) { /* 160MHz */
        bandwidth = hmac_scan_get_bandwidth(g_channel_bandwidth_map_160mhz,
            sizeof(g_channel_bandwidth_map_160mhz) / sizeof(hmac_channel_bandwidth_map), chan_off);
    }
#endif
    if (bandwidth != WLAN_BAND_WIDTH_BUTT) {
        bss_dscr->en_channel_bandwidth = bandwidth;
    }
}


void hmac_scan_update_11ax_ie(hmac_vap_stru *hmac_vap,
                              mac_bss_dscr_stru *pst_bss_dscr,
                              uint8_t *frame_body,
                              uint16_t us_frame_len)
{
    if (IS_CUSTOM_OPEN_11AX_SWITCH(&hmac_vap->st_vap_base_info)) {
        hmac_scan_update_bss_list_11ax(pst_bss_dscr, frame_body, us_frame_len);
    }

    if (g_wlan_spec_cfg->feature_hiex_is_open) {
        pst_bss_dscr->en_punctured_preamble = mac_hiex_ie_get_bit_chip_type(frame_body, us_frame_len);
    }
}

OAL_STATIC uint32_t hmac_scan_get_non_transmitted_bssid(uint8_t *puc_tx_bssid, uint8_t uc_tx_bssid_len,
                                                        uint8_t uc_max_indicator, uint8_t uc_non_bssid_index,
                                                        uint8_t *puc_non_tx_bssid, uint8_t uc_ntx_bssid_len)
{
#define MAC_ADDR_MASK       0xFFFFFFFFFFFF
#define MAX_BSSID_INDICATOR 46
    uint8_t uc_index;
    uint8_t auc_non_tx_bssid[WLAN_MAC_ADDR_LEN];
    uint8_t auc_bssid_tmp[WLAN_MAC_ADDR_LEN];
    uint64_t ull_bssid = 0;
    uint64_t ull_low_bssid;
    uint64_t ull_bssid_mask;
    int32_t l_ret;

    /* Э��涨������� */
    if (uc_max_indicator >= MAX_BSSID_INDICATOR) {
        return OAL_FAIL;
    }

    if ((uc_ntx_bssid_len != WLAN_MAC_ADDR_LEN) || (uc_tx_bssid_len != WLAN_MAC_ADDR_LEN)) {
        return OAL_FAIL;
    }

    ull_bssid_mask = ((MAC_ADDR_MASK << uc_max_indicator) ^ MAC_ADDR_MASK);

    /* �ֽڷ�ת */
    for (uc_index = 0; uc_index < WLAN_MAC_ADDR_LEN; uc_index++) {
        auc_bssid_tmp[uc_index] = puc_tx_bssid[5 - uc_index]; /* 5��addr���һλ������ */
    }
    l_ret = memcpy_s(&ull_bssid, sizeof(uint64_t), auc_bssid_tmp, WLAN_MAC_ADDR_LEN);
    ull_low_bssid = ull_bssid & ull_bssid_mask;
    ull_bssid &= (~ull_bssid_mask);
    ull_low_bssid = ((ull_low_bssid + uc_non_bssid_index) % (1ULL << uc_max_indicator));
    ull_bssid = ((ull_bssid & (~ull_bssid_mask)) | (ull_low_bssid));
    l_ret += memcpy_s(auc_non_tx_bssid, WLAN_MAC_ADDR_LEN, (uint8_t *)&ull_bssid, WLAN_MAC_ADDR_LEN);
    if (l_ret != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_scan_get_non_transmitted_bssid::memcpy fail!");
        return OAL_FAIL;
    }

    for (uc_index = 0; uc_index < WLAN_MAC_ADDR_LEN; uc_index++) {
        puc_non_tx_bssid[uc_index] = auc_non_tx_bssid[5 - uc_index]; /* 5��addr���һλ������ */
    }

    return OAL_SUCC;
}

OAL_STATIC void hmac_scan_proc_non_transmitted_ssid_copy_cb(oal_netbuf_stru *netbuf, oal_netbuf_stru *new_netbuf)
{
    int32_t ret;
    dmac_rx_ctl_stru *transmitted_rx_ctrl = NULL;
    dmac_rx_ctl_stru *new_rx_ctrl = NULL;

    transmitted_rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(netbuf);
    /* copy cb �ֶ� */
    new_rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(new_netbuf);
    ret = memcpy_s(new_rx_ctrl, sizeof(dmac_rx_ctl_stru), transmitted_rx_ctrl, sizeof(dmac_rx_ctl_stru));
    if (ret != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_scan_proc_non_transmitted_ssid_copy_cb::memcpy fail!");
    }
}

/* copy  ssid ie ֮ǰ�Ĳ��� */
OAL_STATIC uint32_t hmac_scan_proc_copy_ssid_ie_before(uint8_t *netbuf_data, uint8_t *mgmt_frame,
    uint16_t new_buf_len, uint16_t before_ssid_ie_len, mac_multi_bssid_frame_info_stru *frame_info)
{
    mac_ieee80211_frame_stru *frame_header = NULL;
    int32_t ret;

    ret = memcpy_s(netbuf_data, new_buf_len, mgmt_frame, before_ssid_ie_len);
    if (ret != EOK) {
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_copy_ssid_ie_before::copy mgmt_frame fail}");
        return OAL_FAIL;
    }
    // �滻mac header �е�bssid �� ta
    frame_header = (mac_ieee80211_frame_stru *)netbuf_data;
    ret = memcpy_s(frame_header->auc_address2, WLAN_MAC_ADDR_LEN,
                   frame_info->auc_non_transmitted_bssid, WLAN_MAC_ADDR_LEN);
    if (ret != EOK) {
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_copy_ssid_ie_before::copy address2 fail}");
        return OAL_FAIL;
    }
    ret = memcpy_s(frame_header->auc_address3, WLAN_MAC_ADDR_LEN,
                   frame_info->auc_non_transmitted_bssid, WLAN_MAC_ADDR_LEN);
    if (ret != EOK) {
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_copy_ssid_ie_before::copy address3 fail}");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/* copy ssid ie ֮��Ĳ��� */
OAL_STATIC uint32_t hmac_scan_proc_copy_ssid_ie_after(uint8_t *netbuf_data, uint16_t new_buf_len,
    uint8_t *after_transmitted_ssid_ie, uint16_t after_ssid_ie_len, uint16_t use_buf_len)
{
    int32_t ret;
    if (new_buf_len < use_buf_len) {
        oam_warning_log2(0, OAM_SF_SCAN, "{hmac_scan_proc_copy_ssid_ie_after::use_buf_len=[%d] over netbuf_len=[%d]}",
                         use_buf_len, new_buf_len);
        return OAL_FAIL;
    }
    ret = memcpy_s(netbuf_data, after_ssid_ie_len, after_transmitted_ssid_ie, after_ssid_ie_len);
    if (ret != EOK) {
        oam_error_log0(0, OAM_SF_SCAN, "hmac_scan_proc_copy_ssid_ie_after::memcpy fail!");
        return OAL_FAIL;
    }
    return OAL_SUCC;
}


oal_netbuf_stru *hmac_scan_proc_non_transmitted_ssid_handle(oal_netbuf_stru *netbuf, uint16_t buf_len,
    mac_multi_bssid_frame_info_stru *frame_info, uint16_t *new_buf_len)
{
    uint8_t *mgmt_frame = NULL;
    uint8_t *mgmt_frame_body = NULL;
    uint8_t *transmitted_ssid = NULL;
    uint8_t *transmitted_ssid_ie = NULL;
    uint8_t *after_transmitted_ssid_ie = NULL;
    oal_netbuf_stru *new_netbuf = NULL;
    uint8_t transmitted_ssid_len = 0;
    uint16_t mgmt_frame_len;
    uint16_t mgmt_frame_body_len;
    uint16_t before_ssid_ie_len;
    uint16_t after_ssid_ie_len;
    int32_t ret;
    uint8_t *netbuf_data = NULL;
    uint16_t use_buf_len;

    /* ��ȡ����֡֡���֡�� */
    mgmt_frame = (uint8_t *)oal_netbuf_data(netbuf);

    mgmt_frame_len = buf_len - sizeof(mac_scanned_result_extend_info_stru);

    mgmt_frame_body = mgmt_frame + MAC_80211_FRAME_LEN;
    mgmt_frame_body_len = buf_len - sizeof(mac_scanned_result_extend_info_stru) - MAC_80211_FRAME_LEN;

    /* ��ȡtransimitted ֡��ssid ���� */
    transmitted_ssid = mac_get_ssid(mgmt_frame_body, (int32_t)mgmt_frame_body_len, &transmitted_ssid_len);
    if ((transmitted_ssid != NULL) && (transmitted_ssid_len == 0)) {
        return NULL;
    }

    /* ����ssid ie ֮ǰ��֡�� */
    transmitted_ssid_ie = transmitted_ssid - MAC_IE_HDR_LEN;
    before_ssid_ie_len = transmitted_ssid_ie - mgmt_frame;

    /* ����ssid ie ֮��֡�� */
    after_transmitted_ssid_ie = transmitted_ssid + transmitted_ssid_len;
    after_ssid_ie_len = buf_len - (after_transmitted_ssid_ie - mgmt_frame);

    /* ��Ҫ��������netbuf �ڴ� */
    *new_buf_len = buf_len - transmitted_ssid_len + frame_info->uc_non_transmitted_ssid_len;
    new_netbuf = (oal_netbuf_stru *)oal_mem_netbuf_alloc(OAL_NORMAL_NETBUF, *new_buf_len, OAL_NETBUF_PRIORITY_MID);
    if (new_netbuf == NULL) {
        oam_error_log1(0, OAM_SF_SCAN, "{Allco netbuf_len=[%d] fail}", *new_buf_len);
        return NULL;
    }
    hmac_scan_proc_non_transmitted_ssid_copy_cb(netbuf, new_netbuf);

    netbuf_data = (uint8_t *)oal_netbuf_data(new_netbuf);
    use_buf_len = before_ssid_ie_len;
    /* ��鳤���Ƿ��㹻 */
    if (*new_buf_len < use_buf_len) {
        oam_warning_log2(0, OAM_SF_SCAN, "{before::use_buf_len=[%d] over netbuf_len=[%d]}", use_buf_len, *new_buf_len);
        oal_netbuf_free(new_netbuf);
        return NULL;
    }
    ret = hmac_scan_proc_copy_ssid_ie_before(netbuf_data, mgmt_frame, *new_buf_len, before_ssid_ie_len, frame_info);
    if (ret != OAL_SUCC) {
        oal_netbuf_free(new_netbuf);
        return NULL;
    }
    netbuf_data += before_ssid_ie_len;
    use_buf_len += (MAC_IE_HDR_LEN + frame_info->uc_non_transmitted_ssid_len);
    if (*new_buf_len < use_buf_len) {
        oam_warning_log2(0, OAM_SF_SCAN, "{hmac_scan_proc_copy_ssid_ie::use_buf_len=[%d] over netbuf_len=[%d]}",
                         use_buf_len, *new_buf_len);
        oal_netbuf_free(new_netbuf);
        return NULL;
    }
    netbuf_data[0] = MAC_EID_SSID;
    netbuf_data[1] = frame_info->uc_non_transmitted_ssid_len;
    netbuf_data += MAC_IE_HDR_LEN;
    ret = memcpy_s(netbuf_data, frame_info->uc_non_transmitted_ssid_len,
                   frame_info->auc_non_transmitted_ssid, frame_info->uc_non_transmitted_ssid_len);
    if (ret != EOK) {
        oam_warning_log0(0, OAM_SF_SCAN, "{hmac_scan_proc_non_transmitted_ssid_handle::cp non_transmitted_ssid fail}");
        oal_netbuf_free(new_netbuf);
        return NULL;
    }

    netbuf_data += frame_info->uc_non_transmitted_ssid_len;
    use_buf_len += after_ssid_ie_len;

    ret = hmac_scan_proc_copy_ssid_ie_after(netbuf_data, *new_buf_len, after_transmitted_ssid_ie,
                                            after_ssid_ie_len, use_buf_len);
    if (ret != OAL_SUCC) {
        oal_netbuf_free(new_netbuf);
        return NULL;
    }

    return new_netbuf;
}

#ifdef _PRE_DEBUG_CODE
void hmac_scan_mbssid_printf(mac_multi_bssid_frame_info_stru *pst_mbssid)
{
    oam_error_log4(0, OAM_SF_SCAN,
        "{>>>>>>>::bit_is_non_transimitted_bss=%d,maxbssid_indicator=%d,bssid_index=%d,non_transmitted_ssid_len=%d.}",
        pst_mbssid->bit_is_non_transimitted_bss, pst_mbssid->uc_maxbssid_indicator,
        pst_mbssid->uc_bssid_index, pst_mbssid->uc_non_transmitted_ssid_len);

    oam_error_log1(0, OAM_SF_SCAN, "{>>>>>>>::us_non_tramsmitted_bssid_cap=[%d].}",
                   pst_mbssid->us_non_tramsmitted_bssid_cap);

    oam_error_log4(0, OAM_SF_UM, "{>>>>>>>::Transmitted BSSID:%02X:%02X:XX:XX:%02X:%02X.}",
                   pst_mbssid->auc_transmitted_bssid[0],
                   pst_mbssid->auc_transmitted_bssid[1],
                   pst_mbssid->auc_transmitted_bssid[MAC_ADDR_4],
                   pst_mbssid->auc_transmitted_bssid[MAC_ADDR_5]);
    oam_error_log4(0, OAM_SF_UM, "{>>>>>>>::Non_Transmitted BSSID:%02X:%02X:XX:XX:%02X:%02X.}",
                   pst_mbssid->auc_non_transmitted_bssid[0],
                   pst_mbssid->auc_non_transmitted_bssid[1],
                   pst_mbssid->auc_non_transmitted_bssid[MAC_ADDR_4],
                   pst_mbssid->auc_non_transmitted_bssid[MAC_ADDR_5]);
}
#endif

uint8_t hmac_scan_proc_non_transmit_bssid(hmac_vap_stru *hmac_vap,
    uint8_t *ie_data, mac_multi_bssid_frame_info_stru *frame_info,
    uint16_t left_len, mac_ieee80211_frame_stru *frame_header)
{
    uint8_t mbssid_ie_len;
    uint8_t auc_non_transmitted_bssid[WLAN_MAC_ADDR_LEN] = { 0 };
    uint32_t ret;
    int32_t l_ret;

    if (mac_ie_parse_multi_bssid_opt_subie(ie_data,
        frame_info, &mbssid_ie_len) != OAL_SUCC) {
        return 0;
    }

    /* �����ǰʣ��֡��С�ڽ�����mbssid ie len ����  */
    if (mbssid_ie_len == 0 || mbssid_ie_len > left_len) {
        return 0;
    }

    /* ����transimitted bssid  */
    l_ret = memcpy_s(frame_info->auc_transmitted_bssid, WLAN_MAC_ADDR_LEN,
                     frame_header->auc_address2, WLAN_MAC_ADDR_LEN);

    /* ����non-transmitted bssid */
    ret = hmac_scan_get_non_transmitted_bssid(frame_info->auc_transmitted_bssid,
                                              sizeof(frame_info->auc_transmitted_bssid),
                                              frame_info->uc_maxbssid_indicator,
                                              frame_info->uc_bssid_index,
                                              auc_non_transmitted_bssid,
                                              sizeof(auc_non_transmitted_bssid));
    if (ret != OAL_SUCC) {
        return 0;
    }

    /* non_transmitted bssid */
    l_ret += memcpy_s(frame_info->auc_non_transmitted_bssid, WLAN_MAC_ADDR_LEN,
                      auc_non_transmitted_bssid, WLAN_MAC_ADDR_LEN);
    if (l_ret != EOK) {
        oam_error_log0(0, OAM_SF_SCAN, "hmac_scan_proc_multi_bssid_scanned_bss::memcpy fail!");
        return 0;
    }

    frame_info->bit_is_non_transimitted_bss = OAL_TRUE;

    return mbssid_ie_len;
}


uint32_t hmac_scan_proc_multi_bssid_scanned_bss(hmac_vap_stru *hmac_vap, oal_netbuf_stru *netbuf, uint16_t buf_len)
{
    uint8_t *mgmt_frame = NULL;
    mac_ieee80211_frame_stru *frame_header = NULL;
    uint8_t *ie_data = NULL;
    uint16_t frame_body_len;
    uint16_t left_len;
    mac_multi_bssid_frame_info_stru frame_info;
    uint8_t *mbssid_ie = NULL;
    oal_netbuf_stru *new_netbuf = NULL;
    uint8_t mbssid_ie_len;
    uint8_t mgmt_filed_len;
    uint16_t new_buf_len = 0;
    uint8_t multi_bssid_ie_len;

    /* ��dmac�ϱ���netbuf���ݽ��н���������������ʾ */
    /***********************************************************************************************/
    /*            netbuf data����ϱ���ɨ�������ֶεķֲ�                                        */
    /* ------------------------------------------------------------------------------------------  */
    /* beacon/probe rsp body  |     ֡����渽���ֶ�(mac_scanned_result_extend_info_stru)          */
    /* -----------------------------------------------------------------------------------------   */
    /* �յ���beacon/rsp��body | rssi(4�ֽ�) | channel num(1�ֽ�)| band(1�ֽ�)|bss_tye(1�ֽ�)|���  */
    /* ------------------------------------------------------------------------------------------  */
    /*                                                                                             */
    /***********************************************************************************************/
    mgmt_frame = (uint8_t *)oal_netbuf_data(netbuf);

    /* ��ȡ����֡��֡ͷ��֡��ָ�� */
    frame_header = (mac_ieee80211_frame_stru *)mgmt_frame;
    /* ����֡�ĳ��ȵ����ϱ���netbuf�ĳ��ȼ�ȥ�ϱ���ɨ��������չ�ֶεĳ��� */
    frame_body_len = buf_len - sizeof(mac_scanned_result_extend_info_stru) - MAC_80211_FRAME_LEN;
    mgmt_filed_len = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;

    if (frame_body_len <= mgmt_filed_len) {
        return OAL_FAIL;
    }

    /* ie ��ʼλ�� */
    ie_data  = mgmt_frame + MAC_80211_FRAME_LEN + mgmt_filed_len;
    left_len = frame_body_len - mgmt_filed_len;

    if (left_len < MAC_MULTIPLE_BSSID_IE_MIN_LEN) {
        return OAL_FAIL;
    }

    /* ����multi-bssid ie */
    mbssid_ie = mac_find_ie(MAC_EID_MULTI_BSSID, ie_data, left_len);
    if (mbssid_ie == NULL) {
        return OAL_FAIL;
    }
    memset_s(&frame_info, sizeof(mac_multi_bssid_frame_info_stru), 0, sizeof(mac_multi_bssid_frame_info_stru));

    /* ����multi-bssid ie len */
    multi_bssid_ie_len = (uint8_t)(*(mbssid_ie + 1));
    ie_data            = mbssid_ie + MAC_IE_HDR_LEN;

    /* ���� MaxBssid Indicator */
    frame_info.uc_maxbssid_indicator = (uint8_t)*ie_data;
    ie_data += 1;
    left_len = multi_bssid_ie_len - 1;

    /* �����Ƿ������Ч��Multi BSSID IE */
    while (left_len > MAC_MULTIPLE_BSSID_IE_MIN_LEN) {
        mbssid_ie_len = hmac_scan_proc_non_transmit_bssid(hmac_vap, ie_data, &frame_info, left_len, frame_header);
        if (mbssid_ie_len == 0) {
            return OAL_FAIL;
        }
        new_netbuf = hmac_scan_proc_non_transmitted_ssid_handle(netbuf, buf_len, &frame_info, &new_buf_len);
        if (new_netbuf == NULL) {
            return OAL_FAIL;
        }
        hmac_scan_proc_scan_result_handle(hmac_vap, new_netbuf, new_buf_len, &frame_info);

        left_len -= mbssid_ie_len;
        ie_data  += mbssid_ie_len;
    }

    return OAL_SUCC;
}

uint8_t mac_device_trans_bandwith_to_he_capinfo(wlan_bw_cap_enum_uint8 en_max_op_bd)
{
    uint8_t uc_he_band_width_set = 0;
    switch (en_max_op_bd) {
        case WLAN_BW_CAP_20M:
            return uc_he_band_width_set;
        case WLAN_BW_CAP_40M:
            uc_he_band_width_set |= BIT0; /* Bit0:ָʾ2.4G֧��40MHz */
            return uc_he_band_width_set;
        case WLAN_BW_CAP_80M:
            uc_he_band_width_set |= BIT0 | BIT1; /* Bit1:ָʾ5G֧��40MHz\80MHz */
            return uc_he_band_width_set;
        case WLAN_BW_CAP_160M:
            uc_he_band_width_set |= BIT0 | BIT1 | BIT2; /* Bit2:ָʾ5G֧��160MHz */
            return uc_he_band_width_set;
        case WLAN_BW_CAP_80PLUS80:
            uc_he_band_width_set |= BIT0 | BIT1 | BIT2 | BIT3; /* Bit3:ָʾ5G֧��80+80MHz */
            return uc_he_band_width_set;
        default:
            oam_error_log1(0, OAM_SF_ANY,
                           "{mac_device_trans_bandwith_to_he_capinfo::bandwith[%d] is invalid.}", en_max_op_bd);
            return WLAN_MIB_VHT_SUPP_WIDTH_BUTT;
    }
}

void mac_vap_init_11ax_rates(mac_vap_stru *pst_mac_vap, mac_device_stru *pst_mac_dev)
{
    return;
}

void mac_vap_user_set_tx_data_include_om(mac_vap_stru *pst_mac_vap,
    mac_user_stru *pst_mac_user)
{
    if ((pst_mac_vap->bit_om_include_custom_switch == OAL_TRUE) &&
        (MAC_USER_HE_HDL_STRU(pst_mac_user)->st_he_cap_ie.st_he_mac_cap.bit_om_control_support)) {
        MAC_USER_TX_DATA_INCLUDE_OM(pst_mac_user) = OAL_TRUE;
    }
}
void mac_vap_11ax_cap_init(mac_vap_stru *pst_mac_vap)
{
    pst_mac_vap->en_11ax_custom_switch            = g_pst_mac_device_capability[0].en_11ax_switch;
    pst_mac_vap->bit_multi_bssid_custom_switch    = g_pst_mac_device_capability[0].bit_multi_bssid_switch;
    pst_mac_vap->bit_htc_include_custom_switch    =
        g_st_mac_device_custom_cfg.st_11ax_custom_cfg.bit_htc_include;
    pst_mac_vap->bit_om_include_custom_switch     =
        g_st_mac_device_custom_cfg.st_11ax_custom_cfg.bit_om_in_data;
    pst_mac_vap->bit_rom_custom_switch            =
        g_st_mac_device_custom_cfg.st_11ax_custom_cfg.bit_rom_cap_switch;
}

OAL_STATIC void mac_set_he_mac_capinfo_field(mac_vap_stru *mac_vap, uint8_t *puc_buffer)
{
    mac_frame_he_mac_cap_stru *pst_he_mac_capinfo = (mac_frame_he_mac_cap_stru *)puc_buffer;

    /* ��ʼ����0����������ֶ�ʱ���ڲ�֧�ֵ�����λĬ�ϲ�д���� */
    memset_s(pst_he_mac_capinfo, sizeof(mac_frame_he_mac_cap_stru), 0, sizeof(mac_frame_he_mac_cap_stru));
    /*********************** HE MAC ������Ϣ�� ************************************
    ----------------------------------------------------------------------------
     |-----------------------------------------------------------------------------------------|
     | +HTC    | TWT         | TWT         | Fragmentation | Max Num      | Min                |
     | HE        | Requester | Responder   | support       | Fragmented   | Fragment           |
     | Support | Support     |   Support   |               | MSDUs        |     Size           |
     |-----------------------------------------------------------------------------------------|
     | B0        | B1        | B2          |   B3  B4      |  B5 B6 B7    |  B8 B9             |
     |-----------------------------------------------------------------------------------------|
     |-----------------------------------------------------------------------------------------|
     | Trigger Frame | Muti-TID     | HE           | ALL        | UMRS        | BSR            |
     | MAC Padding   | Aggregation  | Link         | ACK        | Support     | Support        |
     | Duration      | Support      | Adaptation   | Support    |             |                |
     |-----------------------------------------------------------------------------------------|
     |    B10   B11  | B12      B14 | B15    B16   |   B17      |    B18      | B19            |
     |-----------------------------------------------------------------------------------------|
     |-----------------------------------------------------------------------------------------|
     | Broadcast | 32-bit BA | MU       | Ack-Enable             | Group Addressed   Multi-STA |
     | TWT       | bitmap    | Cascade  | Multi-TID              | BlockAck In DL MU           |
     | Support   | Support   | Support  | Aggregation Support    | Support                     |
     |-----------------------------------------------------------------------------------------|
     | B20       | B21       | B22      |   B23                  |   B24                       |
     |-----------------------------------------------------------------------------------------|
     |-----------------------------------------------------------------------------------------|
     | OM        | OFDMA   | Max A-MPDU  | A-MSDU          | Flexible  TWT| Rx Control         |
     | Control   | RA      | Length      | Fragmentation   |  schedule    | frame to           |
     | Support   | Support | Exponent    |  Support        | Support      | MultiBSS           |
     |---------------------------------------------------------------------------------------- |
     | B25       | B26     | B27  B28    |   B29           |   B30        |  B31               |
     |-----------------------------------------------------------------------------------------|
     |-----------------------------------------------------------------------------------------|
     | NSRP BQRP   | QTP     | BQR     | SR       | NDP     | OPS      | A-MSDU in |           |
     | A-MPDU      |         |         |          |Feedback |  Support | A-MPDU    |           |
     | Aggregation | Support | Support |Responder | Report  |          | Support   |           |
     |-----------------------------------------------------------------------------------------|
     | B32         | B33     | B34     |   B35    |   B36   |  B37     |   B38     |           |
     |-----------------------------------------------------------------------------------------|
     |-----------------------------------------------------------------------------------------|
     | Multi-TID            |                                                                  |
     | Aggregation  Tx      |         Reserved                                                 |
     | Support              |                                                                  |
     |-----------------------------------------------------------------------------------------|
     | B39  B40   B41       |         B42--B47                                                 |
     |-----------------------------------------------------------------------------------------|
    ***************************************************************************/
    /* B0:+HTC HE Support */
    pst_he_mac_capinfo->bit_htc_he_support = OAL_TRUE;

    /* B1:TWT Requester Support */
    pst_he_mac_capinfo->bit_twt_requester_support = mac_mib_get_he_TWTOptionActivated(mac_vap);

    /* B10-11:Trigger Frame MAC Padding Duration   16us */
    pst_he_mac_capinfo->bit_trigger_mac_padding_duration = mac_mib_get_he_TriggerMacPaddingDuration(mac_vap);
    /* WiFi6��֤����������,��̬����Э��mac padding */
    if (mac_vap->bit_mac_padding > 0) {
        pst_he_mac_capinfo->bit_trigger_mac_padding_duration = mac_vap->bit_mac_padding - 1;
    }

    /* B12-B14:Multi-TID Aggregation Rx support */
    pst_he_mac_capinfo->bit_mtid_aggregation_rx_support = WLAN_TID_MAX_NUM - 1;

    /* B19:BSR Support */
    pst_he_mac_capinfo->bit_bsr_support = mac_mib_get_he_BSRSupport(mac_vap);

    /* B20:Broadcast TWT Support */
    pst_he_mac_capinfo->bit_broadcast_twt_support = OAL_TRUE;

    /* B25:OM Control Support */
    if (pst_he_mac_capinfo->bit_htc_he_support) {
        pst_he_mac_capinfo->bit_om_control_support = mac_mib_get_OMIOptionImplemented(mac_vap);
    }

    /* B26: OFDMA RA Support */
    pst_he_mac_capinfo->bit_ofdma_ra_support = mac_mib_get_he_OFDMARandomAccess(mac_vap);

    /* B27-28:Maximum AMPDU Length Exponent */
    pst_he_mac_capinfo->bit_max_ampdu_length_exponent = mac_mib_get_he_MaxAMPDULengthExponent(mac_vap);

    if (IS_STA(mac_vap)) {
        /* B32:BSRP BQRP AMPDU Aggregation Support */
        pst_he_mac_capinfo->bit_bsrp_bqrp_ampdu_addregation = 1;

        /* B34:BQR Support */
        pst_he_mac_capinfo->bit_bqr_support = 1;

        /* B36:NDP Feedback Report Support */
        pst_he_mac_capinfo->bit_ndp_feedback_report_support = 0;

        /* B43:UL 2��996-tone RU Support */
        pst_he_mac_capinfo->bit_ul_2x996_ru_support = 1;

        /* B47:����HT/VHT��ʽ��Basic trigger֡ */
        pst_he_mac_capinfo->bit_he_and_vht_trigger_frame_rx_support = 1;
    }
}


OAL_STATIC void mac_set_he_phy_capinfo_field_part2(mac_vap_stru *pst_mac_vap,
    mac_frame_he_phy_cap_stru *pst_he_phy_capinfo)
{
    mac_device_stru *mac_dev = mac_res_get_dev(pst_mac_vap->uc_device_id);

    if (mac_dev == NULL) {
        oam_error_log1(pst_mac_vap->uc_vap_id, OAM_SF_ANY,
                       "mac_set_he_phy_capinfo_field_part2: mac device null. device_id %d",
                       pst_mac_vap->uc_device_id);
        return;
    }

    /* B53: HE ER SU PPDU TRX */
    pst_he_phy_capinfo->bit_partial_bandwidth_extended_range = OAL_TRUE;

    /* B55:PPE Threshold Present */
    pst_he_phy_capinfo->bit_ppe_threshold_present = mac_mib_get_PPEThresholdsRequired(pst_mac_vap);

    /* B58:HE SU PPDU And HE MU PPDU with 4x HE-LTF And 0.8usGI */
    pst_he_phy_capinfo->bit_he_su_ppdu_he_mu_ppdu_4x_he_ltf_08us_gi = 1;

    /* B59-B61:Max Nc */
    pst_he_phy_capinfo->bit_max_nc = MAC_DEVICE_GET_NSS_NUM(mac_dev);

    if (IS_STA(pst_mac_vap)) {
        /* B62:UL OFDMA STBC Tx>80MHz */
        pst_he_phy_capinfo->bit_stbc_tx_over_80mhz = 1;
    }

    /* B63:STBC Rx>80MHz */
    pst_he_phy_capinfo->bit_stbc_rx_over_80mhz = OAL_FALSE;

    /* B64:HE ER SU PPDU with 4x HE-LTF And 0.8usGI */
    pst_he_phy_capinfo->bit_he_er_su_ppdu_4x_he_ltf_08us_gi = 1;

    /* B65: STAǿ��֧��,AP reserve */
    if (IS_STA(pst_mac_vap)) {
        /* 20/40MHz HE PPDU 2.4G */
        pst_he_phy_capinfo->bit_20mhz_in_40mhz_he_ppdu_2g = 1;
        /* 5G֧��160M,����ǿ��֧�� */
        pst_he_phy_capinfo->bit_20mhz_in_160mhz_he_ppdu = 1;
        pst_he_phy_capinfo->bit_80mhz_in_160mhz_he_ppdu = 1;
    }

    /* B68:HE ER SU PPDU with 1x HE-LTF And 0.8usGI */
    pst_he_phy_capinfo->bit_he_er_su_ppdu_1x_he_ltf_08us_gi = 1;

    if (IS_STA(pst_mac_vap)) {
        pst_he_phy_capinfo->bit_longer_than_16_he_sigb_support =
            g_wlan_spec_cfg->longer_than_16_he_sigb_support;
        pst_he_phy_capinfo->bit_tx_1024qam_below_242ru_support = OAL_TRUE;
        pst_he_phy_capinfo->bit_rx_1024qam_below_242ru_support = OAL_TRUE;
        /* B76:Rx Full BW SU Using HE MU PPDU With Compressed SIGB */
        pst_he_phy_capinfo->bit_rx_full_bw_su_ppdu_with_compressed_sigb = OAL_TRUE;
        pst_he_phy_capinfo->bit_rx_full_bw_su_ppdu_with_non_compressed_sigb = OAL_TRUE;
    }
}

#ifdef _PRE_WLAN_FEATURE_M2S
OAL_STATIC OAL_INLINE void mac_set_he_sounding_bfmee_sts_field(mac_vap_stru *pst_mac_vap,
    mac_frame_he_phy_cap_stru *pst_he_phy_capinfo)
{
    mac_user_stru *pst_mac_user = NULL;
    mac_he_hdl_stru *pst_he_hdl = NULL;

    /* �ο����,���ֶθ��ݶԶ�num of sounding���Լ�������ȡ���� */
    pst_mac_user = mac_res_get_mac_user(pst_mac_vap->us_assoc_vap_id);
    if (IS_STA(pst_mac_vap) && pst_mac_user != NULL) {
        pst_he_hdl = MAC_USER_HE_HDL_STRU(pst_mac_user);
        if (pst_he_hdl->st_he_cap_ie.st_he_phy_cap.bit_below_80mhz_sounding_dimensions_num != 0) {
            pst_he_phy_capinfo->bit_beamformee_sts_below_80mhz = oal_min(
                pst_he_phy_capinfo->bit_beamformee_sts_below_80mhz,
                pst_he_hdl->st_he_cap_ie.st_he_phy_cap.bit_below_80mhz_sounding_dimensions_num);
            pst_he_phy_capinfo->bit_beamformee_sts_below_80mhz =
                oal_max(pst_he_phy_capinfo->bit_beamformee_sts_below_80mhz, 3); /* 3Ϊ���������ĵ͵� */
        }
        if (pst_he_hdl->st_he_cap_ie.st_he_phy_cap.bit_over_80mhz_sounding_dimensions_num != 0) {
            pst_he_phy_capinfo->bit_beamformee_sts_over_80mhz = oal_min(
                pst_he_phy_capinfo->bit_beamformee_sts_over_80mhz,
                pst_he_hdl->st_he_cap_ie.st_he_phy_cap.bit_over_80mhz_sounding_dimensions_num);
            pst_he_phy_capinfo->bit_beamformee_sts_over_80mhz =
                oal_max(pst_he_phy_capinfo->bit_beamformee_sts_over_80mhz, 3); /* 3Ϊ���������ĵ͵� */
        }
    }
}
#endif


OAL_STATIC void mac_set_he_sounding_capinfo_field(mac_vap_stru *pst_mac_vap,
    mac_frame_he_phy_cap_stru *pst_he_phy_capinfo)
{
    uint8_t uc_below80mhz;
    uint8_t uc_over80mhz;

    /* B31:SU Beamformer */
    pst_he_phy_capinfo->bit_su_beamformer = mac_mib_get_he_SUBeamformer(pst_mac_vap);

    /* B32:SU Beamformee */
    pst_he_phy_capinfo->bit_su_beamformee = mac_mib_get_he_SUBeamformee(pst_mac_vap);

    /* B33:MU Beamformer */
    pst_he_phy_capinfo->bit_mu_beamformer = mac_mib_get_he_MUBeamformer(pst_mac_vap);

    uc_below80mhz = mac_mib_get_he_BeamformeeSTSBelow80Mhz(pst_mac_vap);
    uc_over80mhz = mac_mib_get_he_BeamformeeSTSOver80Mhz(pst_mac_vap);
    if ((uc_below80mhz >= 1) && (uc_over80mhz >= 1)) {
        /* B34-B36:Beamformee STS �� 80MHz,��СֵΪ3�� 1103֧��4*2 1105֧��8*2 */
        pst_he_phy_capinfo->bit_beamformee_sts_below_80mhz = oal_max(uc_below80mhz - 1, 3);
        /* B37-B39:Beamformee STS > 80MHz,��СֵΪ3�� 1103֧��4*2 1105֧��8*2 */
        pst_he_phy_capinfo->bit_beamformee_sts_over_80mhz = oal_max(uc_over80mhz - 1, 3);
    }

    if (pst_he_phy_capinfo->bit_su_beamformer == 1) {
        /* B40-B42: beamformer֧�ֵ���������� */
        pst_he_phy_capinfo->bit_below_80mhz_sounding_dimensions_num =
            mac_mib_get_HENumberSoundingDimensionsBelow80Mhz(pst_mac_vap);

        /* B43-B45:beamformer֧�ֵ���������� */
        pst_he_phy_capinfo->bit_over_80mhz_sounding_dimensions_num =
            mac_mib_get_HENumberSoundingDimensionsOver80Mhz(pst_mac_vap);
    }

#ifdef _PRE_WLAN_FEATURE_M2S
    mac_set_he_sounding_bfmee_sts_field(pst_mac_vap, pst_he_phy_capinfo); // ��װ������������
#endif

    /* B46:Ng = 16 SU Feedback */
    pst_he_phy_capinfo->bit_ng16_su_feedback = mac_mib_get_HENg16SUFeedback(pst_mac_vap);

    /* B47:Ng = 16 MU Feedback */
    pst_he_phy_capinfo->bit_ng16_mu_feedback = mac_mib_get_HENg16MUFeedback(pst_mac_vap);

    /* B48:CodeBook Size(��,��)={4,2} SU Feedback */
    pst_he_phy_capinfo->bit_codebook_42_su_feedback = mac_mib_get_HECodebook42SUFeedback(pst_mac_vap);

    /* B49:CodeBook Size(��,��)={7,5} MU Feedback */
    pst_he_phy_capinfo->bit_codebook_75_mu_feedback = mac_mib_get_HECodebook75MUFeedback(pst_mac_vap);

    /* B50:trigger_su_beamforming_feedback */
    pst_he_phy_capinfo->bit_trigger_su_beamforming_feedback = OAL_FALSE;

    /* B51:trigger_mu_beamforming_partialBW_feedback */
    pst_he_phy_capinfo->bit_trigger_mu_beamforming_partialbw_feedback = OAL_FALSE;

    /* B52:trigger CQI feedback */
    pst_he_phy_capinfo->bit_trigger_cqi_feedback = OAL_FALSE;
}


OAL_STATIC uint8_t mac_set_he_channel_width(mac_vap_stru *pst_mac_vap)
{
    if ((pst_mac_vap->en_vap_mode == WLAN_VAP_MODE_BSS_AP) && (pst_mac_vap->st_channel.en_band == WLAN_BAND_2G)) {
            return mac_mib_get_2GFortyMHzOperationImplemented(pst_mac_vap);
    }

    return mac_device_trans_bandwith_to_he_capinfo(mac_mib_get_dot11VapMaxBandWidth(pst_mac_vap));
}


OAL_STATIC void mac_set_he_phy_capinfo_field(mac_vap_stru *mac_vap, uint8_t *puc_buffer)
{
    mac_frame_he_phy_cap_stru *pst_he_phy_capinfo = (mac_frame_he_phy_cap_stru *)puc_buffer;

    /************************************** HE PHY ������Ϣ�� **************************************
    -----------------------------------------------------------------------------------------------|
     |---------------------------------------------------------------------------------------------|
     | Reserved  | Channel    | Punctured  | Device  | LDPC        | HE SU PPDU With  | Midamble   |
     |           | Width      | Preamble   | Class   | Coding In   | 1x HE-LTF And    | Rx Max     |
     |           | Set        | RX         |         | Payload     | 0.8 us GI        | NSTS       |
     |---------------------------------------------------------------------------------------------|
     | B0        | B1 B7      | B8 B11     | B12     | B13         | B14              | B15 B16    |
     |---------------------------------------------------------------------------------------------|
     |---------------------------------------------------------------------------------------------|
     | NDP With      | STBC Tx   | STBC Rx   | Doppler | Doppler | Full           | Partial        |
     | 4x HE-LTF And | <= 80 MHz | <= 80 MHz | Tx      | Rx      | Bandwidth UL   | Bandwidth UL   |
     | 3.2 ms GI     |           |           |         |         | MU-MIMO        | MU-MIMO        |
     |---------------------------------------------------------------------------------------------|
     | B17           | B18       | B19       | B20     | B21     | B22            | B23            |
     |---------------------------------------------------------------------------------------------|
     |---------------------------------------------------------------------------------------------|
     | DCM Max       | DCM     | DCM Max       | DCM    | Rx HE MU PPDU  | SU         | SU         |
     | Constellation | Max     | Constellation | Max    | From           | Beamformer | Beamformee |
     | Tx            | NSS Tx  | Rx            | NSS Rx | Non-AP STA     |            |            |
     |---------------------------------------------------------------------------------------------|
     | B24 B25       | B26     | B27 B28       | B29    | B30            | B31        | B32        |
     |---------------------------------------------------------------------------------------------|
     |---------------------------------------------------------------------------------------------|
     | MU         | Beamformee | Beamformee  | Number Of Sounding | Number Of Sounding | Ng = 16   |
     | Beamformer | STS <= 80  | STS >= 80   | Dimensions         | Dimensions         | SU        |
     |            | MHz        | MHz         | <= 80              | >= 80              | Feedback  |
     |---------------------------------------------------------------------------------------------|
     | B33        | B34 B36    | B37  B39    | B40 B42            | B43 B45            | B46       |
     |---------------------------------------------------------------------------------------------|
     |---------------------------------------------------------------------------------------------|
     | Ng = 16   | Codebook     | Codebook     | Triggered SU  | Triggered MU         | Triggered  |
     | MU        | Size = {4,2} | Size = {7,5} | Beamforming   | Beamforming          | CQI        |
     | Feedback  | SU Feedback  | MU Feedback  | Feedback      | Partial BW Feedback  | Feedback   |
     |---------------------------------------------------------------------------------------------|
     | B47       | B48          | B49          | B50           | B51                  | B52        |
     |---------------------------------------------------------------------------------------------|
     |---------------------------------------------------------------------------------------------|
     | Partial        | Partial      | PPE       | SRP-based  | Power Boost  | HE SU PPDU And      |
     | Bandwidth      | Bandwidth    | Threshold | SR         | Factor       | HE MU PPDU With 4x  |
     | Extended Range | DL MU-MIMO   | Present   | Support    | Support      | HE-LTF And 0.8us GI |
     |---------------------------------------------------------------------------------------------|
     | B53            | B54          | B55       | B56        | B57          | B58                 |
     |---------------------------------------------------------------------------------------------|
     |---------------------------------------------------------------------------------------------|
     | Max     | STBC      | STBC      | HE ER SU PPDU   | 20 MHz In 40 MHz  | 20 MHz In           |
     | Nc      | Tx        | Rx        | With 4x HE-LTF  | HE PPDU In        | 160/80+80 MHZ       |
     |         | > 80MHz   | > 80MHz   | And 0.8us GI    | 2.4GHz Band       | HE PPDU             |
     |---------------------------------------------------------------------------------------------|
     | B59 B61 | B62       | B63       | B64             | B65               | B66                 |
     |---------------------------------------------------------------------------------------------|
     |---------------------------------------------------------------------------------------------|
     | 80 MHz In      | HE ER SU PPDU   | Midamble Rx 2x   |                                       |
     | 160/80+80 MHZ  | With 1x HE-LTF  | And              |                Reserve                |
     | HE PPDU        | And 0.8us GI    | 1x HE-LTF        |                                       |
     |---------------------------------------------------------------------------------------------|
     | B67            | B68             | B69              |                B70 B71                |
     |---------------------------------------------------------------------------------------------|
    ************************************************************************************************/
    /* ��ʼ����0����������ֶ�ʱ���ڲ�֧�ֵ�����λĬ�ϲ�д���� */
    memset_s(pst_he_phy_capinfo, sizeof(mac_frame_he_phy_cap_stru), 0, sizeof(mac_frame_he_phy_cap_stru));

    /* B1-B7:channel width set, ����vap�µĴ������� */
    pst_he_phy_capinfo->bit_channel_width_set = mac_set_he_channel_width(mac_vap);

    /* B8-B11:punctured preamble rx */
    if (mac_vap->en_vap_mode == WLAN_VAP_MODE_BSS_STA) {
        pst_he_phy_capinfo->bit_punctured_preamble_rx = 0;
    }
    /* B12:device class */
    pst_he_phy_capinfo->bit_device_class = PHY_DEVICE_CLASS_A;

    /* B13:LDPC Coding in payload,��֤����5.24��5.25Ҫ��֧�� */
    pst_he_phy_capinfo->bit_ldpc_coding_in_paylod = mac_mib_get_he_LDPCCodingInPayload(mac_vap);

    /* B14:HE SU PPDU with 1x HE-LTF and 0.8us GI */
    pst_he_phy_capinfo->bit_he_su_ppdu_1x_he_ltf_08us_gi =
        mac_mib_get_HESUPPDUwith1xHELTFand0point8GIlmplemented(mac_vap);

    /* B17:HE NDP PPDU֧��4x HE-LTF+3.2us Gi */
    pst_he_phy_capinfo->bit_ndp_4x_he_ltf_32us_gi = 1;

    /* B18:UL OFDMA STBC Tx<=80MHz */
    if (IS_STA(mac_vap)) {
        pst_he_phy_capinfo->bit_stbc_tx_below_80mhz = mac_mib_get_he_STBCTxBelow80M(mac_vap);
    }

    /* B19:STBC Rx<=80MHz */
    if (IS_STA(mac_vap)) {
        pst_he_phy_capinfo->bit_stbc_rx_below_80MHz =
            (mac_vap->st_cap_flag.bit_rx_stbc == OAL_TRUE) ? mac_mib_get_he_STBCRxBelow80M(mac_vap) : OAL_FALSE;
    } else {
        pst_he_phy_capinfo->bit_stbc_rx_below_80MHz = OAL_TRUE;
    }

    /* B22:Full Bandwidth UL MU-MIMO */
    if (IS_STA(mac_vap)) {
        pst_he_phy_capinfo->bit_full_bandwidth_ul_mu_mimo = OAL_FALSE;
    }

    /* sounding���� */
    mac_set_he_sounding_capinfo_field(mac_vap, pst_he_phy_capinfo);

    mac_set_he_phy_capinfo_field_part2(mac_vap, pst_he_phy_capinfo);
}

OAL_STATIC void mac_set_tx_rx_he_mcs(mac_frame_he_basic_mcs_nss_stru *tx_rx_he_mcs, uint8_t nss_num)
{
    tx_rx_he_mcs->bit_max_he_mcs_1ss = MAC_MAX_SUP_MCS11_11AX_EACH_NSS;
    tx_rx_he_mcs->bit_max_he_mcs_2ss = nss_num >= WLAN_DOUBLE_NSS ?
        MAC_MAX_SUP_MCS11_11AX_EACH_NSS : MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    tx_rx_he_mcs->bit_max_he_mcs_3ss = nss_num >= WLAN_TRIPLE_NSS ?
        MAC_MAX_SUP_MCS11_11AX_EACH_NSS : MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    tx_rx_he_mcs->bit_max_he_mcs_4ss = nss_num >= WLAN_FOUR_NSS ?
        MAC_MAX_SUP_MCS11_11AX_EACH_NSS : MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    tx_rx_he_mcs->bit_max_he_mcs_5ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    tx_rx_he_mcs->bit_max_he_mcs_6ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    tx_rx_he_mcs->bit_max_he_mcs_7ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    tx_rx_he_mcs->bit_max_he_mcs_8ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
}


OAL_STATIC uint8_t mac_set_he_tx_rx_mcs_nss_field(mac_vap_stru *vap, uint8_t channel_width_b2,
    uint8_t channel_width_b3, uint8_t *buffer)
{
    uint8_t uc_len = 0;
    mac_device_stru *mac_dev = mac_res_get_dev(vap->uc_device_id);
    wlan_nss_enum_uint8 nss_num_tx = 0;
    wlan_nss_enum_uint8 nss_num_rx = 0;
    mac_frame_he_supported_he_mcs_nss_set_stru *he_tx_rx_mcs_nss_info =
        (mac_frame_he_supported_he_mcs_nss_set_stru *)buffer;
    if (mac_dev == NULL) {
        oam_error_log1(vap->uc_vap_id, OAM_SF_ANY,
                       "mac_set_he_tx_rx_mcs_nss_field: mac device null. device_id %d",
                       vap->uc_device_id);
        return uc_len;
    }
    mac_vap_ini_get_nss_num(mac_dev, &nss_num_rx, &nss_num_tx);

    /******************************** HE Supported HE-MCS And NSS Set *********************************
    |-------------------------------------------------------------------------------------------------|
    | Rx HE-MCS Map | Tx HE-MCS Map | Rx HE-MCS Map  | Tx HE-MCS Map  | Rx HE-MCS Map | Tx HE-MCS Map |
    | <= 80 MHz     | <= 80 MHz     | 160 MHz        | 160 MHz        | 80+80 MHz     | 80+80 MHz     |
    |-------------------------------------------------------------------------------------------------|
    | 2 Octets      | 2 Octets      | 0 or 2 Octets  | 0 or 2 Octets  | 0 or 2 Octets | 0 or 2 Octets |
    |-------------------------------------------------------------------------------------------------|
    **************************************************************************************************/
    memset_s(he_tx_rx_mcs_nss_info, sizeof(mac_frame_he_supported_he_mcs_nss_set_stru),
             0, sizeof(mac_frame_he_supported_he_mcs_nss_set_stru));
    /* ����<=80MHz��rx���� */
    mac_set_tx_rx_he_mcs(&(he_tx_rx_mcs_nss_info->st_rx_he_mcs_map_below_80mhz), nss_num_rx);

    uc_len += sizeof(mac_frame_he_basic_mcs_nss_stru);

    /* ����<=80MHz��tx���� */
    mac_set_tx_rx_he_mcs(&(he_tx_rx_mcs_nss_info->st_tx_he_mcs_map_below_80mhz), nss_num_tx);

    uc_len += sizeof(mac_frame_he_basic_mcs_nss_stru);

    if (channel_width_b2 != 0) {
        /* Rx HE-MCS Map 160 MHz */
        mac_set_tx_rx_he_mcs(&(he_tx_rx_mcs_nss_info->st_rx_he_mcs_map_160mhz), nss_num_rx);

        uc_len += sizeof(mac_frame_he_basic_mcs_nss_stru);

        /* Tx HE-MCS Map 160 MHz */
        mac_set_tx_rx_he_mcs(&(he_tx_rx_mcs_nss_info->st_tx_he_mcs_map_160mhz), nss_num_tx);

        uc_len += sizeof(mac_frame_he_basic_mcs_nss_stru);
    }

    if (channel_width_b3 != 0) {
        /* TRx HE-MCS Map 80+80 MHz: 05�ݲ�֧�� */
    }

    return uc_len;
}


OAL_STATIC uint8_t mac_set_he_ppe_thresholds_field(uint8_t *puc_buffer)
{
    uint8_t uc_len;
    mac_frame_ppe_thresholds_pre_field_stru *pst_ppe_thresholds;

    pst_ppe_thresholds = (mac_frame_ppe_thresholds_pre_field_stru *)puc_buffer;

    memset_s(pst_ppe_thresholds, sizeof(mac_frame_ppe_thresholds_pre_field_stru),
             0, sizeof(mac_frame_ppe_thresholds_pre_field_stru));

    pst_ppe_thresholds->bit_nss = 1;            /* ˫�� */
    pst_ppe_thresholds->bit_ru_index_mask = 0xC; /* ��ʾ160M�������HE SU PPDUֻ֧��PE=16us */

    pst_ppe_thresholds->bit_ppet16_nss0_ru0 = 0; /* ��ʾ�κ�mcs����Ҫ16us */
    pst_ppe_thresholds->bit_ppet8_nss0_ru0 = 7;  /* 7��bit=111��PPET8����Ϊnone */
    pst_ppe_thresholds->bit_ppet16_nss0_ru1 = 0;
    pst_ppe_thresholds->bit_ppet8_nss0_ru1 = 7; /* 7��bit=111 */
    pst_ppe_thresholds->bit_ppet16_nss1_ru0 = 0;
    pst_ppe_thresholds->bit_ppet8_nss1_ru0 = 7; /* 7��bit=111 */
    pst_ppe_thresholds->bit_ppet16_nss1_ru1 = 0;
    pst_ppe_thresholds->bit_ppet8_nss1_ru1 = 7; /* 7��bit=111 */

    uc_len = sizeof(mac_frame_ppe_thresholds_pre_field_stru);

    return uc_len;
}

OAL_STATIC oal_bool_enum_uint8 mac_set_he_forbid_encap_he_ie(mac_vap_stru *p_vap)
{
    if (OAL_TRUE != mac_mib_get_HEOptionImplemented(p_vap) || !MAC_VAP_IS_WORK_HE_PROTOCOL(p_vap)) {
        return OAL_TRUE;
    }

    /* PF��֤Ҫ�����WEP��TKIP ���ܷ�ʽ���ܹ�����HEģʽ */
    if ((OAL_TRUE == mac_is_wep_enabled(p_vap)) || (OAL_TRUE == mac_is_tkip_only(p_vap))) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}


/*lint -save -e438 */
void mac_set_he_capabilities_ie(mac_vap_stru *mac_vap, uint8_t *puc_buffer, uint8_t *puc_ie_len)
{
    uint8_t *puc_ie_length = NULL;
    uint8_t uc_info_length;
    mac_frame_he_phy_cap_stru *pst_he_phy_capinfo = NULL;
    uint8_t uc_channel_width_b2;
    uint8_t uc_channel_width_b3;

    *puc_ie_len = 0;

    /* PF��֤Ҫ�����WEP��TKIP ���ܷ�ʽ���ܹ�����HEģʽ */
    if (mac_set_he_forbid_encap_he_ie(mac_vap) == OAL_TRUE) {
        return;
    }

    /***************************************************************************
    -------------------------------------------------------------------------
    |EID |Length |EID Extension|HE MAC Capa. Info |HE PHY Capa. Info|
    -------------------------------------------------------------------------
    |1   |1          |1                  |5                         |9                       |
    -------------------------------------------------------------------------
    |Tx Rx HE MCS NSS Support |PPE Thresholds(Optional)|
    -------------------------------------------------------------------------
    |2 or More                         |Variable                       |
    -------------------------------------------------------------------------
    ***************************************************************************/
    *puc_buffer = MAC_EID_HE;
    puc_ie_length = puc_buffer + 1;

    puc_buffer += MAC_IE_HDR_LEN;
    *puc_ie_len += MAC_IE_HDR_LEN;

    *puc_buffer = MAC_EID_EXT_HE_CAP;
    puc_buffer += 1;

    *puc_ie_len += 1;

    /* ���HE mac capabilities information����Ϣ */
    mac_set_he_mac_capinfo_field(mac_vap, puc_buffer);
    puc_buffer += MAC_HE_MAC_CAP_LEN;
    *puc_ie_len += MAC_HE_MAC_CAP_LEN;

    /* ���HE PHY Capabilities Information ����Ϣ */
    pst_he_phy_capinfo = (mac_frame_he_phy_cap_stru *)puc_buffer;
    mac_set_he_phy_capinfo_field(mac_vap, puc_buffer);
    puc_buffer += MAC_HE_PHY_CAP_LEN;
    *puc_ie_len += MAC_HE_PHY_CAP_LEN;

    /* ��� HE tx rx he mcs nss support */
    uc_channel_width_b2 = pst_he_phy_capinfo->bit_channel_width_set & BIT2;
    uc_channel_width_b3 = pst_he_phy_capinfo->bit_channel_width_set & BIT3;
    uc_info_length = mac_set_he_tx_rx_mcs_nss_field(mac_vap, uc_channel_width_b2, uc_channel_width_b3, puc_buffer);
    puc_buffer += uc_info_length;
    *puc_ie_len += uc_info_length;

    /* ��� PPE Thresholds field */
    if (mac_mib_get_PPEThresholdsRequired(mac_vap)) {
        uc_info_length = mac_set_he_ppe_thresholds_field(puc_buffer);
        puc_buffer += uc_info_length;
        *puc_ie_len += uc_info_length;
    }
    *puc_ie_length = *puc_ie_len - MAC_IE_HDR_LEN;
}

OAL_STATIC OAL_INLINE void mac_set_he_nss_bit_map_para(
    mac_frame_he_basic_mcs_nss_stru *pst_he_mcs_nss_bit_map)
{
    if (g_optimized_feature_switch_bitmap & BIT(CUSTOM_OPTIMIZE_FEATURE_CERTIFY_MODE)) {
        pst_he_mcs_nss_bit_map->bit_max_he_mcs_1ss = MAC_MAX_SUP_MCS7_11AX_EACH_NSS;
    } else {
        pst_he_mcs_nss_bit_map->bit_max_he_mcs_1ss = MAC_MAX_SUP_MCS11_11AX_EACH_NSS;
    }
    pst_he_mcs_nss_bit_map->bit_max_he_mcs_2ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    pst_he_mcs_nss_bit_map->bit_max_he_mcs_3ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    pst_he_mcs_nss_bit_map->bit_max_he_mcs_4ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    pst_he_mcs_nss_bit_map->bit_max_he_mcs_5ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    pst_he_mcs_nss_bit_map->bit_max_he_mcs_6ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    pst_he_mcs_nss_bit_map->bit_max_he_mcs_7ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
    pst_he_mcs_nss_bit_map->bit_max_he_mcs_8ss = MAC_MAX_SUP_INVALID_11AX_EACH_NSS;
}


void mac_set_he_operation_ie(mac_vap_stru *mac_vap, uint8_t *puc_buffer, uint8_t *puc_ie_len)
{
    uint8_t                              *puc_ie_length = NULL;
    mac_frame_he_operation_param_stru    *pst_he_opern_param = NULL;
    mac_frame_he_bss_color_info_stru     *pst_he_bss_color = NULL;
    mac_frame_he_basic_mcs_nss_stru    *pst_he_mcs_nss_bit_map = NULL;
    *puc_ie_len = 0;

    /* PF��֤Ҫ�����WEP��TKIP ���ܷ�ʽ���ܹ�����HEģʽ */
    if (mac_set_he_forbid_encap_he_ie(mac_vap) == OAL_TRUE) {
        return;
    }

    /*******************HE Operation element**********************************
    --------------------------------------------------------------------
    |EID |Length |EID Extension|HE Operation Parameters |BSS Color Info|
    --------------------------------------------------------------------
    |1   |1      |1            |3                       |1             |
    --------------------------------------------------------------------
    |Basic HE-MCS |VHT Operation |Max Co-Hosted    |6 GHz Operation    |
    |And NSS Set  |Info          |BSSID Indicator  |Info               |
    --------------------------------------------------------------------
    |2            |0 or 3        | 0 or 1          |0 or 4             |
    --------------------------------------------------------------------
    ***************************************************************************/
    *puc_buffer    = MAC_EID_HE;
    puc_ie_length  = puc_buffer + 1;

    puc_buffer    += MAC_IE_HDR_LEN;
    *puc_ie_len   += MAC_IE_HDR_LEN;

    *puc_buffer    = MAC_EID_EXT_HE_OPERATION;
    puc_buffer    += 1;
    *puc_ie_len   += 1;

    pst_he_opern_param = (mac_frame_he_operation_param_stru *)puc_buffer;
    /*******************HE Operation Parameters********************************
    --------------------------------------------------------------------
    |Default PE |TWT      |TXOP Duration |VHT Operation |Co-Hosted |
    |Duration   |Required |RTS Threshold |Info. Present |BSS       |
    --------------------------------------------------------------------
    |3          |1        |10            |1             |1         |
    --------------------------------------------------------------------
    |ER SU   |6 GHz Operation |Reserved |
    |Disable |Info. Present   |         |
    -------------------------------------
    |1       |1               |6        |
    -------------------------------------
    ***************************************************************************/
    /* B0-B2:Default PE Duration, ��֧�֣�5-7reserved */
    pst_he_opern_param->bit_default_pe_duration = MAC_HE_DEFAULT_PE_DURATION;

    /* B3:TWT Required */
    pst_he_opern_param->bit_twt_required = 0;

    /* B4-B13:TXOP Duration RTS Threshold, 1023��ʾ���ֶβ����� */
    pst_he_opern_param->bit_he_duration_rts_threshold = MAC_HE_DURATION_RTS_THRESHOLD;

    /* B14:VHT Operation Info. Present */
    /* ����beacon��probe rsp��һ��Я��VHT��Ϣ���ʴ�λ��0 */
    pst_he_opern_param->bit_vht_operation_info_present = 0;

    /* B15:Co-Hosted BSS */
    pst_he_opern_param->bit_co_located_bss = 0;

    /* B16:ER SU Disable��ȡֵ��0��ʾ֧�ֽ���242-tone HE ER SU PPDU */
    pst_he_opern_param->bit_er_su_disable = 0;

    /* B17:6 GHz Operation Info. Present��ȡֵ, ��֧��6GHz band */
    /* ��λ��0,��ʾHE Operation��6GHz Operation Info�ֶβ����� */
    pst_he_opern_param->bit_6g_oper_info_present = 0;

    /* B18-B23���� */
    pst_he_opern_param->bit_reserved = 0;

    puc_buffer    += MAC_HE_OPE_PARAM_LEN;
    *puc_ie_len   += MAC_HE_OPE_PARAM_LEN;

    pst_he_bss_color = (mac_frame_he_bss_color_info_stru *)puc_buffer;
    memset_s(pst_he_bss_color, sizeof(mac_frame_he_bss_color_info_stru),
             0, sizeof(mac_frame_he_bss_color_info_stru));
    /*******************BSS Color Info*********************
    -------------------------------------------------------
    |BSS Color |Partial BSS Color    |BSS Color Disabled |
    -------------------------------------------------------
    |6         |1                    |1                  |
    -------------------------------------------------------
    *******************************************************/
    /* ֧��bss color */
    pst_he_bss_color->bit_bss_color = mac_vap->aput_bss_color_info;
    if (pst_he_bss_color->bit_bss_color == 0) {
        pst_he_bss_color->bit_bss_color_disable = OAL_TRUE;
    } else {
        pst_he_bss_color->bit_bss_color_disable = OAL_FALSE;
    }
    puc_buffer    += 1;
    *puc_ie_len   += 1;

    /* ��� Basic HE-MCS And NSS Set, ���ֶ�ΪAP�Ļ�������, ����֧�ֽ���AP����С����Ϊ����MCS7 */
    pst_he_mcs_nss_bit_map = (mac_frame_he_basic_mcs_nss_stru *)puc_buffer;
    memset_s(pst_he_mcs_nss_bit_map, sizeof(mac_frame_he_basic_mcs_nss_stru),
             0, sizeof(mac_frame_he_basic_mcs_nss_stru));

    mac_set_he_nss_bit_map_para(pst_he_mcs_nss_bit_map); // ��װ������������

    puc_buffer    += MAC_HE_OPE_BASIC_MCS_NSS_LEN;
    *puc_ie_len   += MAC_HE_OPE_BASIC_MCS_NSS_LEN;
    *puc_ie_length = *puc_ie_len - MAC_IE_HDR_LEN;
}


void mac_get_htc_uph_om_value(uint8_t uc_nss, uint8_t uc_bw,
    uint8_t uc_mimo_resound, uint8_t uc_ul_mu_disable, uint32_t *pul_htc_value)
{
    mac_htc_a_control_field_union   *pst_he_om_info;
    wlan_bw_cap_enum_uint8           en_bw_cap;

    en_bw_cap = WLAN_BANDWIDTH_TO_BW_CAP(uc_bw);

    *pul_htc_value = HTC_INVALID_VALUE;
    pst_he_om_info = (mac_htc_a_control_field_union *)pul_htc_value;

    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_vht_flag             = 1;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_he_flag              = 1;

    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_uph_id               = MAC_HTC_A_CONTROL_TYPE_UPH;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_ul_power_headroom    = 0;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_min_transmit_power_flag    = 0;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_uph_rsv    = 0;

    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_om_id                = MAC_HTC_A_CONTROL_TYPE_OM;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_rx_nss               = uc_nss;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_channel_width        = en_bw_cap;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_ul_mu_disable        = uc_ul_mu_disable;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_tx_nsts              = uc_nss;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_er_su_disable        = 0;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_dl_mu_mimo_resound_recommendation  = uc_mimo_resound;
    pst_he_om_info->un_a_control_info.st_uph_om_info.bit_ul_mu_data_disable   = 0;
}


void mac_get_htc_om_value(uint8_t uc_nss, uint8_t uc_bw,
    uint8_t uc_mimo_resound, uint8_t uc_ul_mu_disable, uint32_t *pul_htc_value)
{
    mac_htc_a_control_field_union   *pst_he_om_info;
    wlan_bw_cap_enum_uint8           en_bw_cap;

    en_bw_cap = WLAN_BANDWIDTH_TO_BW_CAP(uc_bw);

    *pul_htc_value = HTC_INVALID_VALUE;
    pst_he_om_info = (mac_htc_a_control_field_union *)pul_htc_value;

    pst_he_om_info->un_a_control_info.st_om_info.bit_vht_flag             = 1;
    pst_he_om_info->un_a_control_info.st_om_info.bit_he_flag              = 1;
    pst_he_om_info->un_a_control_info.st_om_info.bit_om_id                = MAC_HTC_A_CONTROL_TYPE_OM;
    pst_he_om_info->un_a_control_info.st_om_info.bit_rx_nss               = uc_nss;
    pst_he_om_info->un_a_control_info.st_om_info.bit_channel_width        = en_bw_cap;
    pst_he_om_info->un_a_control_info.st_om_info.bit_ul_mu_disable        = uc_ul_mu_disable;
    pst_he_om_info->un_a_control_info.st_om_info.bit_tx_nsts              = uc_nss;
    pst_he_om_info->un_a_control_info.st_om_info.bit_er_su_disable        = 0;
    pst_he_om_info->un_a_control_info.st_om_info.bit_dl_mu_mimo_resound_recommendation  = uc_mimo_resound;
    pst_he_om_info->un_a_control_info.st_om_info.bit_ul_mu_data_disable   = 0;
}


void mac_set_he_ie_in_assoc_rsp(mac_vap_stru *mac_vap, uint16_t us_assoc_id,
    uint8_t *puc_asoc_rsp, uint8_t *puc_ie_len)
{
    mac_user_stru     *pst_mac_user = NULL;
    uint8_t          uc_ie_len    = 0;

    pst_mac_user = mac_res_get_mac_user(us_assoc_id);

    *puc_ie_len = 0;

    /* ap û�й���ax, �Ͳ�Я��he cap �� */
    if (!MAC_VAP_IS_WORK_HE_PROTOCOL(mac_vap)) {
        return ;
    }

    if ((pst_mac_user != NULL) && (mac_user_get_he_capable(pst_mac_user) == OAL_TRUE)) {
        /* ���� HE Capabilities IE */
        mac_set_he_capabilities_ie(mac_vap, puc_asoc_rsp, &uc_ie_len);
        puc_asoc_rsp += uc_ie_len;
        *puc_ie_len  += uc_ie_len;

        /* ���� HE Operation IE */
        mac_set_he_operation_ie(mac_vap, puc_asoc_rsp, &uc_ie_len);
        *puc_ie_len  += uc_ie_len;
    }
}


void hmac_sta_update_join_multi_bssid_info(mac_vap_stru *pst_mac_vap, dmac_ctx_join_req_set_reg_stru *pst_reg_params,
    mac_multi_bssid_info *pst_mbssid_info)
{
    if (IS_CUSTOM_OPEN_MULTI_BSSID_SWITCH(pst_mac_vap)) {
        memcpy_s(&pst_reg_params->st_mbssid_info, sizeof(mac_multi_bssid_info),
                 pst_mbssid_info, sizeof(mac_multi_bssid_info));
        mac_mib_set_he_MultiBSSIDActived(pst_mac_vap, pst_mbssid_info->bit_ext_cap_multi_bssid_activated);
    }
}


OAL_STATIC oal_bool_enum_uint8 hmac_sta_join_can_not_start_he(mac_vap_stru *p_mac_vap,
    mac_bss_dscr_stru *p_bss_dscr)
{
    if (hmac_wifi6_self_cure_mac_is_wifi6_blacklist_type(p_bss_dscr->auc_bssid)) {
        oam_warning_log3(0, OAM_SF_SCAN, "hmac_sta_join_can_not_start_he::mac:%02X:XX:XX:XX:%02X:%02X \
            in wifi6 balcklist, close HEOptionImplemented",
            p_bss_dscr->auc_bssid[0], /* 0Ϊmac��ַ�ֽ�λ�� */
            p_bss_dscr->auc_bssid[4], /* 4Ϊmac��ַ�ֽ�λ�� */
            p_bss_dscr->auc_bssid[5]); /* 5Ϊmac��ַ�ֽ�λ�� */
        return OAL_TRUE;
    }

    if (p_bss_dscr->en_p2p_scenes ==  MAC_P2P_SCENES_LOW_LATENCY || mac_is_secondary_sta(p_mac_vap)) {
        return mac_vap_can_not_start_he_protocol(p_mac_vap);
    }

    return OAL_FALSE;
}


void hmac_sta_join_update_protocol_mib(mac_vap_stru *p_mac_vap,
    mac_bss_dscr_stru *p_bss_dscr)
{
    mac_mib_set_HEOptionImplemented(p_mac_vap, OAL_FALSE);
    if ((p_bss_dscr->en_he_capable == OAL_TRUE) && (IS_CUSTOM_OPEN_11AX_SWITCH(p_mac_vap))) {
        mac_mib_set_HEOptionImplemented(p_mac_vap, OAL_TRUE);
        if (OAL_TRUE == hmac_sta_join_can_not_start_he(p_mac_vap, p_bss_dscr)) {
            p_bss_dscr->en_he_capable = OAL_FALSE;
            mac_mib_set_HEOptionImplemented(p_mac_vap, OAL_FALSE);
        }
    }

    /* 2g 11ax sta������ʱ,gc 5g �������˵�11Э�� */
    hmac_vap_join_avoid_dbac_back_to_11n_handle(p_mac_vap, p_bss_dscr);
    oam_warning_log3(0, OAM_SF_SCAN, "hmac_sta_join_update_protocol_mib::bss_he_cap=%d,bss_vht_cap=%d \
        en_p2p_scenes=%d", p_bss_dscr->en_he_capable, p_bss_dscr->en_vht_capable,
        p_bss_dscr->en_p2p_scenes);
    oam_warning_log3(0, OAM_SF_SCAN, "hmac_sta_join_update_protocol_mib::he_mib=%d, \
        VAP_IS_SUPPORT_11AX=%d, MAC_VAP_GET_CAP_BW=%d",
        mac_mib_get_HEOptionImplemented(p_mac_vap),
        MAC_VAP_IS_SUPPORT_11AX(p_mac_vap), p_mac_vap->st_channel.en_bandwidth);
}

uint32_t hmac_proc_he_cap_ie(mac_vap_stru *pst_mac_vap, hmac_user_stru *pst_hmac_user, uint8_t *puc_he_cap_ie)
{
    mac_he_hdl_stru *pst_mac_he_hdl;
    mac_he_hdl_stru st_mac_he_hdl;
    mac_user_stru *pst_mac_user;
    uint32_t ret;

    /* ����he cap IE */
    if (oal_any_null_ptr2(pst_mac_vap, pst_hmac_user)) {
        oam_error_log0(0, OAM_SF_11AX, "{hmac_proc_he_cap_ie::mac_vap or hmac_user is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (puc_he_cap_ie == NULL) {
        oam_warning_log0(0, OAM_SF_11AX, "{hmac_proc_he_cap_ie::he_cap_ie is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (puc_he_cap_ie[1] < MAC_HE_CAP_MIN_LEN) {
        oam_warning_log1(0, OAM_SF_11AX, "{hmac_proc_he_cap_ie::invalid he cap ie len[%d].}", puc_he_cap_ie[1]);
        return OAL_FAIL;
    }

    pst_mac_user = &pst_hmac_user->st_user_base_info;

    /* ֧��11ax���Ž��к����Ĵ��� */
    if (OAL_FALSE == mac_mib_get_HEOptionImplemented(pst_mac_vap)) {
        return OAL_SUCC;
    }

    pst_mac_he_hdl = &st_mac_he_hdl;
    mac_user_get_he_hdl(pst_mac_user, pst_mac_he_hdl);

    /* ���� HE MAC Capabilities Info */
    ret = mac_ie_parse_he_cap(puc_he_cap_ie, &pst_mac_he_hdl->st_he_cap_ie);
    if (ret != OAL_SUCC) {
        return OAL_FAIL;
    }

    pst_mac_he_hdl->en_he_capable = OAL_TRUE;

    mac_user_set_he_hdl(pst_mac_user, pst_mac_he_hdl);

    /* ����user֧�ֵ�Beamforming�ռ������� */
    pst_mac_user->en_avail_bf_num_spatial_stream =
        (pst_mac_vap->st_channel.en_bandwidth <= WLAN_BAND_WIDTH_80MINUSMINUS) ?
        pst_mac_he_hdl->st_he_cap_ie.st_he_phy_cap.bit_beamformee_sts_below_80mhz :
        pst_mac_he_hdl->st_he_cap_ie.st_he_phy_cap.bit_beamformee_sts_over_80mhz;

    return OAL_SUCC;
}


uint32_t hmac_proc_he_bss_color_change_announcement_ie(mac_vap_stru *pst_mac_vap, hmac_user_stru *pst_hmac_user,
    uint8_t *puc_bss_color_ie)
{
    mac_frame_bss_color_change_annoncement_ie_stru st_bss_color_info;
    mac_he_hdl_stru *pst_mac_he_hdl;

    if (OAL_FALSE == mac_mib_get_HEOptionImplemented(pst_mac_vap)) {
        return OAL_SUCC;
    }

    memset_s(&st_bss_color_info, sizeof(mac_frame_bss_color_change_annoncement_ie_stru),
             0, sizeof(mac_frame_bss_color_change_annoncement_ie_stru));
    if (OAL_SUCC != mac_ie_parse_he_bss_color_change_announcement_ie(puc_bss_color_ie, &st_bss_color_info)) {
        return OAL_SUCC;
    }

    pst_mac_he_hdl = MAC_USER_HE_HDL_STRU(&pst_hmac_user->st_user_base_info);
    pst_mac_he_hdl->bit_change_announce_bss_color = st_bss_color_info.bit_new_bss_color;
    pst_mac_he_hdl->bit_change_announce_bss_color_exist = 1;

    oam_warning_log2(0, 0, "hmac_proc_he_bss_color_change_announcement_ie,bss_color=%d, bss_color_exist=[%d]",
        pst_mac_he_hdl->bit_change_announce_bss_color, pst_mac_he_hdl->bit_change_announce_bss_color_exist);

    return OAL_SUCC;
}


uint8_t mac_set_he_6ghz_band_cap_ie(mac_vap_stru *mac_vap, uint8_t *buffer)
{
    mac_he_6ghz_band_cap_info_stru *he_6g_capinfo = NULL;

    if (mac_mib_get_HEOptionImplemented(mac_vap) != OAL_TRUE ||
        ((mac_device_band_is_support(0, MAC_DEVICE_CAP_6G_LOW_BAND) != OAL_TRUE) &&
        (mac_device_band_is_support(0, MAC_DEVICE_CAP_6G_ALL_BAND) != OAL_TRUE))) {
        return 0;
    }

    /***********************************************
    ------------------------------------------------
    |EID |Length |EID Extension|Capabilities Info |
    ------------------------------------------------
    |1   |1      |1            |2                 |
    ------------------------------------------------
    ************************************************/
    *buffer++ = MAC_EID_HE;
    *buffer++ = MAC_HE_6GHZ_BAND_CAP_LEN + 1;
    *buffer++ = MAC_EID_EXT_HE_6GHZ_BAND_CAP;

    /*******************Capabilities Info**************************
    ---------------------------------------------------------------
    |Min MPDU Start |Max A-MPDU Length |Max MPDU |Reserved |
    |Spacing        |Exponent          |Length   |         |
    ---------------------------------------------------------------
    |3              |3                 |2        |1        |
    ---------------------------------------------------------------
    |SM Power |RD        |Rx Ant Patter |Tx Ant Patter |Reserved |
    |Save     |Responder |Consistency   |Consistency   |         |
    ---------------------------------------------------------------
    |2        |1         |1             |1             |2        |
    ---------------------------------------------------------------
    ***************************************************************/
    he_6g_capinfo = (mac_he_6ghz_band_cap_info_stru *)buffer;
    memset_s(he_6g_capinfo, sizeof(mac_he_6ghz_band_cap_info_stru), 0, sizeof(mac_he_6ghz_band_cap_info_stru));

    he_6g_capinfo->min_mpdu_start_spacing = mac_mib_get_min_mpdu_start_spacing(mac_vap);
    he_6g_capinfo->max_ampdu_len_exponent = mac_mib_get_max_ampdu_len_exponent(mac_vap);
    he_6g_capinfo->max_mpdu_len = mac_mib_get_maxmpdu_length(mac_vap);;
#ifdef _PRE_WLAN_FEATURE_SMPS
    he_6g_capinfo->sm_power_save = mac_calc_smps_field(mac_mib_get_smps(mac_vap));
#else
    he_6g_capinfo->sm_power_save = MAC_SMPS_MIMO_MODE;
#endif
    he_6g_capinfo->rd_responder = mac_mib_get_rd_rsp(mac_vap);
    he_6g_capinfo->rx_ant_pattern = 0; /* �ڸù����в��ı�����ģʽ����Ϊ1,���ı�����Ϊ0 */
    he_6g_capinfo->tx_ant_pattern = 0; /* �ڸù����в��ı�����ģʽ����Ϊ1,���ı�����Ϊ0 */

    return MAC_IE_HDR_LEN + MAC_HE_6GHZ_BAND_CAP_LEN;
}
#endif

