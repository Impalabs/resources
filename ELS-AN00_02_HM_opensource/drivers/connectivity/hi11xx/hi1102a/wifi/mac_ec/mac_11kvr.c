

/* 1 ͷ�ļ����� */
#include "mac_11kvr.h"
#include "mac_frame.h"
#include "mac_vap.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_11KVR_C

#if defined(_PRE_WLAN_FEATURE_11K) || defined(_PRE_WLAN_FEATURE_FTM)

oal_void mac_set_rrm_enabled_cap_field(oal_void *pst_vap, oal_uint8 *puc_buffer, oal_uint8 *puc_ie_len)
{
    mac_vap_stru                *pst_mac_vap = (mac_vap_stru *)pst_vap;
    mac_rrm_enabled_cap_ie_stru *pst_rrm_enabled_cap_ie = OAL_PTR_NULL;
    mac_frame_rom_cb_stru *pst_mac_frame_rom_cb = mac_get_frame_rom_cb_addr();

    puc_buffer[0] = MAC_EID_RRM;
    puc_buffer[1] = MAC_RRM_ENABLE_CAP_IE_LEN;

    pst_rrm_enabled_cap_ie = (mac_rrm_enabled_cap_ie_stru *)(puc_buffer + MAC_IE_HDR_LEN);

    memset_s(pst_rrm_enabled_cap_ie, OAL_SIZEOF(mac_rrm_enabled_cap_ie_stru),
        0, OAL_SIZEOF(mac_rrm_enabled_cap_ie_stru));
    /* ֻ��bit0 4 5 6λ��1 */
    pst_rrm_enabled_cap_ie->bit_link_cap =
        pst_mac_vap->pst_mib_info->st_wlan_mib_sta_config.en_dot11RMLinkMeasurementActivated;
    pst_rrm_enabled_cap_ie->bit_bcn_passive_cap =
        pst_mac_vap->pst_mib_info->st_wlan_mib_sta_config.en_dot11RMBeaconPassiveMeasurementActivated;
    pst_rrm_enabled_cap_ie->bit_bcn_active_cap =
        pst_mac_vap->pst_mib_info->st_wlan_mib_sta_config.en_dot11RMBeaconActiveMeasurementActivated;
    pst_rrm_enabled_cap_ie->bit_bcn_table_cap =
        pst_mac_vap->pst_mib_info->st_wlan_mib_sta_config.en_dot11RMBeaconTableMeasurementActivated;

#ifdef _PRE_WLAN_FEATURE_FTM
    pst_rrm_enabled_cap_ie->bit_ftm_range_report_cap = mac_mib_get_FineTimingMsmtRangeRepActivated(pst_mac_vap);
#endif

    *puc_ie_len = MAC_IE_HDR_LEN + MAC_RRM_ENABLE_CAP_IE_LEN;

    pst_mac_frame_rom_cb->p_mac_set_rrm_enabled_cap_field(pst_vap, puc_buffer, puc_ie_len);
}
#endif //_PRE_WLAN_FEATURE_11K
#ifdef _PRE_WLAN_FEATURE_11R

oal_void mac_set_md_ie(oal_void *pst_vap, oal_uint8 *puc_buffer, oal_uint8 *puc_ie_len)
{
    mac_vap_stru *pst_mac_vap = (mac_vap_stru *)pst_vap;

    if (pst_mac_vap->pst_mib_info->st_wlan_mib_fast_bss_trans_cfg.en_dot11FastBSSTransitionActivated != OAL_TRUE) {
        *puc_ie_len = 0;
        return;
    }

    /***************************************************************************
                    ---------------------------------------------------
                    |Element ID | Length | MDID| FT Capbility & Policy|
                    ---------------------------------------------------
           Octets:  |1          | 1      | 2   |  1                   |
                    ---------------------------------------------------
    ***************************************************************************/
    *puc_buffer       = MAC_EID_MOBILITY_DOMAIN;
    *(puc_buffer + 1) = 3; /* Length��ֵ��3 */
    /* auc_dot11FTMobilityDomainID[0]��ֵ��MDID��1���ֽ�(puc_bufferƫ��2byte) */
    *(puc_buffer + 2) = pst_mac_vap->pst_mib_info->st_wlan_mib_fast_bss_trans_cfg.auc_dot11FTMobilityDomainID[0];
    /* auc_dot11FTMobilityDomainID[1]��ֵ��MDID��2���ֽ�(puc_bufferƫ��3byte) */
    *(puc_buffer + 3) = pst_mac_vap->pst_mib_info->st_wlan_mib_fast_bss_trans_cfg.auc_dot11FTMobilityDomainID[1];
    *(puc_buffer + 4) = 0; /* FT����λ(puc_bufferƫ��4byte)���� */

    /***************************************************************************
                    ------------------------------------------
                    |FT over DS| RRP Capability | Reserved   |
                    ------------------------------------------
             Bits:  |1         | 1              | 6          |
                    ------------------------------------------
    ***************************************************************************/
    if (pst_mac_vap->pst_mib_info->st_wlan_mib_fast_bss_trans_cfg.en_dot11FTOverDSActivated == OAL_TRUE) {
        *(puc_buffer + 4) += 1; /* FT����λ(puc_bufferƫ��4byte)��1 */
    }
    if (pst_mac_vap->pst_mib_info->st_wlan_mib_fast_bss_trans_cfg.en_dot11FTResourceRequestSupported == OAL_TRUE) {
        *(puc_buffer + 4) += 2; /* FT����λ(puc_bufferƫ��4byte)��2 */
    }
    *puc_ie_len = 5; /* Element ID��Length��MDID��FT Capbility & Policyռ5�ֽ� */
}
oal_void mac_set_rde_ie(oal_void *pst_mac_vap, oal_uint8 *puc_buffer, oal_uint8 *puc_ie_len)
{
    /*********************************************************************************************
              -------------------------------------------------------------------
              |Element ID | Length | RDE Identifier | Res Dscr Cnt| Status Code |
              -------------------------------------------------------------------
     Octets:  |1          | 1      | 1              |  1          |   2         |
              -------------------------------------------------------------------
    ***********************************************************************************************/
    *puc_buffer       = MAC_EID_RDE;
    *(puc_buffer + 1) = 4; /* Lengthֵ��4 */
    *(puc_buffer + 2) = 0; /* RDE Identifierλ(puc_bufferƫ��2byte)���� */
    *(puc_buffer + 3) = 0; /* Res Dscr Cntλ(puc_bufferƫ��3byte)���� */
    *(puc_buffer + 4) = 0; /* Status Codeλ(puc_bufferƫ��4��5byte)���� */
    *(puc_buffer + 5) = 0;

    *puc_ie_len = 6; /* rde ie�� Element ID�� Length��RDE Identifier��Res Dscr Cnt��Status Code��ռ6�ֽ� */
}

oal_void mac_set_tspec_ie(oal_void *pst_mac_vap, oal_uint8 *puc_buffer, oal_uint8 *puc_ie_len, oal_uint8 uc_tid)
{
    oal_uint8           uc_len = 0;
    mac_ts_info_stru    *pst_ts_info = OAL_PTR_NULL;
/***********************************************************************************************
          --------------------------------------------------------------------------------------
          |Element ID|Length|TS Info|Nominal MSDU Size|Max MSDU Size|Min Serv Itvl|Max Serv Itvl|
          ---------------------------------------------------------------------------------------
 Octets:  |1         | 1(55)| 3     |  2              |   2         |4            |4            |
          ---------------------------------------------------------------------------------------
          | Inactivity Itvl | Suspension Itvl | Serv Start Time |Min Data Rate | Mean Data Rate |
          ---------------------------------------------------------------------------------------
 Octets:  |4                | 4               | 4               |4             |  4             |
          ---------------------------------------------------------------------------------------
          |Peak Data Rate|Burst Size|Delay Bound|Min PHY Rate|Surplus BW Allowance  |Medium Time|
          ---------------------------------------------------------------------------------------
 Octets:  |4             |4         | 4         | 4          |  2                   |2          |
          ---------------------------------------------------------------------------------------

***********************************************************************************************/
    *puc_buffer       = MAC_EID_TSPEC;
    *(puc_buffer + 1) = 55; /* Lengthֵ��55 */
    uc_len += 2; /* ����Element ID��Length(��2�ֽ�) */

    memset_s(puc_buffer + uc_len, *(puc_buffer + 1), 0, *(puc_buffer + 1));

    pst_ts_info = (mac_ts_info_stru *)(puc_buffer + uc_len); // TS Info
    uc_len += 3; /* ����TS Info(3�ֽ�) */

    pst_ts_info->bit_tsid       = uc_tid;
    pst_ts_info->bit_direction  = 3;  // 3��ʾ˫��
    pst_ts_info->bit_apsd       = 1;
    pst_ts_info->bit_user_prio  = uc_tid;

    *(oal_uint16 *)(puc_buffer + uc_len) =  0x812c; // Nominal MSDU Size
    uc_len += 28; /* ����28�ֽڵ�Mean Data Rate */

    *(oal_uint32 *)(puc_buffer + uc_len) = 0x0001D4C0; // Mean Data Rate
    uc_len += 16; /* ����Mean Data Rate��Peak Data Rate��Burst Size��Delay Bound 16�ֽ� */

    *(oal_uint16 *)(puc_buffer + uc_len) = 0x3000; // Surplus BW Allowance

    *puc_ie_len = 57; /* tspec ie ռ57�ֽ� */
}

#endif //_PRE_WLAN_FEATURE_11R
#ifdef _PRE_WLAN_FEATURE_FTM

oal_void mac_ftm_add_to_ext_capabilities_ie(oal_void *pst_vap, oal_uint8 *puc_buffer, oal_uint8 *puc_ie_len)
{
    mac_vap_stru *pst_mac_vap = OAL_PTR_NULL;
    mac_ext_cap_ftm_ie_stru *pst_ext_cap_ftm = OAL_PTR_NULL;
    mac_ftm_mode_enum_uint8 en_ftm_mode;

    if (oal_unlikely((pst_vap == OAL_PTR_NULL) || (puc_buffer == OAL_PTR_NULL) || (puc_ie_len == OAL_PTR_NULL))) {
        OAM_ERROR_LOG0(0, OAM_SF_RRM, "{mac_ftm_add_to_ext_capabilities_ie::pst_dmac_vap NULL!}");
        return;
    }

    pst_mac_vap = (mac_vap_stru *)pst_vap;
    en_ftm_mode = mac_check_ftm_enable(pst_mac_vap);
    if (en_ftm_mode == MAC_FTM_DISABLE_MODE) {
        return;
    }

    puc_buffer[1] = MAC_XCAPS_EX_FTM_LEN;

    pst_ext_cap_ftm = (mac_ext_cap_ftm_ie_stru *)(puc_buffer + MAC_IE_HDR_LEN);

    switch (en_ftm_mode) {
        case MAC_FTM_RESPONDER_MODE:
            pst_ext_cap_ftm->bit_ftm_resp = OAL_TRUE;
            pst_ext_cap_ftm->bit_ftm_int = OAL_FALSE;
            break;

        case MAC_FTM_INITIATOR_MODE:
            pst_ext_cap_ftm->bit_ftm_resp = OAL_FALSE;
            pst_ext_cap_ftm->bit_ftm_int = OAL_TRUE;
            break;

        case MAC_FTM_MIX_MODE:
            pst_ext_cap_ftm->bit_ftm_resp = OAL_TRUE;
            pst_ext_cap_ftm->bit_ftm_int = OAL_TRUE;
            break;

        default:
            break;
    }

    (*puc_ie_len)++;
}
#endif

