

#include "mac_vap.h"
#include "mac_frame.h"
#include "hmac_mgmt_ap.h"
#include "hmac_encap_frame_ap.h"
#include "hmac_mgmt_bss_comm.h"
#include "hmac_rx_data.h"
#include "hmac_uapsd.h"
#include "hmac_config.h"
#include "securec.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_UAPSD_C

#define HMAC_UAPSD_SEND_ALL 0xff /* ���Ͷ��������б���,����ΪUINT8�������ֵ */
#define HMAC_UAPSD_WME_LEN  8

OAL_STATIC uint8_t hmac_uapsd_set_status(mac_user_uapsd_status_stru *uapsd_status, uint8_t *wmm_ie)
{
    uint8_t en_uapsd = OAL_FALSE;
    uint8_t flag = 0;
    uint8_t max_sp;

    uapsd_status->uc_qos_info = wmm_ie[HMAC_UAPSD_WME_LEN];

    /* 8ΪWMM IE���� */
    if ((wmm_ie[HMAC_UAPSD_WME_LEN] & BIT0) == BIT0) {
        uapsd_status->uc_ac_trigger_ena[WLAN_WME_AC_VO] = 1;
        uapsd_status->uc_ac_delievy_ena[WLAN_WME_AC_VO] = 1;
        en_uapsd = OAL_TRUE;
    }

    if ((wmm_ie[HMAC_UAPSD_WME_LEN] & BIT1) == BIT1) {
        uapsd_status->uc_ac_trigger_ena[WLAN_WME_AC_VI] = 1;
        uapsd_status->uc_ac_delievy_ena[WLAN_WME_AC_VI] = 1;
        en_uapsd = OAL_TRUE;
    }

    if ((wmm_ie[HMAC_UAPSD_WME_LEN] & BIT2) == BIT2) {
        uapsd_status->uc_ac_trigger_ena[WLAN_WME_AC_BK] = 1;
        uapsd_status->uc_ac_delievy_ena[WLAN_WME_AC_BK] = 1;
        en_uapsd = OAL_TRUE;
    }

    if ((wmm_ie[HMAC_UAPSD_WME_LEN] & BIT3) == BIT3) {
        uapsd_status->uc_ac_trigger_ena[WLAN_WME_AC_BE] = 1;
        uapsd_status->uc_ac_delievy_ena[WLAN_WME_AC_BE] = 1;
        en_uapsd = OAL_TRUE;
    }

    if (en_uapsd == OAL_TRUE) {
        flag |= MAC_USR_UAPSD_EN;
    }

    /* ����max SP���� */
    max_sp = (wmm_ie[HMAC_UAPSD_WME_LEN] >> 5) & 0x3; // ��5,6bit��ʾ�ڷ������ڣ�SP���ڣ�������AP�����MSDU��MMPDU����

    /* max_spֵΪ0��Э��涨���Ͳ��Ϸ�������3��ʱ��AP����ȫ���Ļ���֡��������2 * max_sp������֡��������Qos-NULL֡�� */
    uapsd_status->uc_max_sp_len = (oal_value_not_in_valid_range(max_sp, 1, 3)) ?
        HMAC_UAPSD_SEND_ALL : max_sp * 2; /* ����2 * max_sp������֡��������Qos-NULL֡�� */
    return flag;
}


void hmac_uapsd_update_user_para(hmac_user_stru *hmac_user, uint8_t *payload, uint32_t msg_len)
{
    uint32_t ret;
    uint8_t uapsd_flag;
    uint8_t *wmm_ie = NULL;
    mac_vap_stru *mac_vap = NULL;
    h2d_event_user_uapsd_info h2d_user_uapsd_info = {0};

    /* Send uapsd_flag & uapsd_status syn to dmac */
    mac_vap = (mac_vap_stru *)mac_res_get_mac_vap(hmac_user->st_user_base_info.uc_vap_id);
    if (oal_unlikely(mac_vap == NULL)) {
        oam_error_log0(hmac_user->st_user_base_info.uc_vap_id, OAM_SF_CFG,
                       "{hmac_uapsd_update_user_para::get mac_vap null.}");
        return;
    }

    wmm_ie = mac_get_wmm_ie(payload, (uint16_t)msg_len);
    /* ������WMM IE,ֱ�ӷ��� */
    if (oal_unlikely(wmm_ie == NULL)) {
        oam_warning_log1(hmac_user->st_user_base_info.uc_vap_id, OAM_SF_PWR,
                         "Could not find WMM IE in assoc req,user_id[%d]\n",
                         hmac_user->st_user_base_info.us_assoc_id);
        return;
    }
    uapsd_flag = hmac_uapsd_set_status(&h2d_user_uapsd_info.mac_user_uapsd_status, wmm_ie);

    // user_index
    h2d_user_uapsd_info.user_id = hmac_user->st_user_base_info.us_assoc_id;

    // uapsd_flag
    h2d_user_uapsd_info.uapsd_flag = uapsd_flag;

    ret = hmac_config_send_event(mac_vap, WLAN_CFGID_UAPSD_UPDATE, sizeof(h2d_user_uapsd_info),
        (uint8_t *)(uintptr_t)(&h2d_user_uapsd_info));
    if (oal_unlikely(ret != OAL_SUCC)) {
        oam_warning_log1(mac_vap->uc_vap_id, OAM_SF_CFG,
                         "{hmac_uapsd_update_user_para::hmac_config_send_event fail. erro code is %u}", ret);
    }
}

