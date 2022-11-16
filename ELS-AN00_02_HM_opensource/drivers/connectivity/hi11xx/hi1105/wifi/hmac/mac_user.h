

#ifndef __MAC_USER_H__
#define __MAC_USER_H__

/* 1 ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "frw_ext_if.h"
#include "wlan_spec.h"
#include "wlan_types.h"
#include "wlan_mib.h"
#include "mac_frame.h"
#include "mac_common.h"
#ifdef _PRE_WLAN_FEATURE_HIEX
#include "hiex_msg.h"
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_USER_H

/* 2 �궨�� */
#define MAC_ACTIVE_USER_IDX_BMAP_LEN ((WLAN_ASSOC_USER_MAX_NUM >> 3) + 1) /* ��Ծ�û�����λͼ */
#define MAC_INVALID_RA_LUT_IDX       WLAN_ASSOC_USER_MAX_NUM              /* �����õ�RA LUT IDX */

#define MAC_USER_FREED   0xff /* USER��Դδ���� */
#define MAC_USER_ALLOCED 0x5a /* USER�ѱ����� */


typedef struct {
    uint8_t uc_rs_nrates;                      /* ���� */
    uint8_t auc_rs_rates[WLAN_MAX_SUPP_RATES]; /* ���� */
} mac_rate_stru;


#ifdef _PRE_WLAN_FEATURE_WMMAC
/* user�ṹ�У�ts��Ϣ�ı����� */
typedef struct mac_ts {
    uint32_t medium_time;
    uint8_t uc_tsid;
    uint8_t uc_up;
    mac_wmmac_direction_enum_uint8 en_direction;
    mac_ts_conn_status_enum_uint8 en_ts_status;
    uint8_t uc_ts_dialog_token;
    uint8_t uc_vap_id;
    uint16_t us_mac_user_idx;
    /* ÿ��ts�¶���Ҫ������ʱ�����˴����ŵ�hmac_user_stru */
    frw_timeout_stru st_addts_timer;
    uint8_t uc_second_flag;
    uint8_t auc_rsv[NUM_3_BYTES];
} mac_ts_stru;
#endif

#if defined(_PRE_WLAN_FEATURE_11AX)
#define MAC_USER_HE_HDL_STRU(_user)         (&(((mac_user_stru *)(_user))->st_he_hdl))
#define MAC_USER_IS_HE_USER(_user)          (MAC_USER_HE_HDL_STRU(_user)->en_he_capable)
#define MAC_USER_TX_DATA_INCLUDE_HTC(_user) ((_user)->bit_tx_data_include_htc)
#define MAC_USER_TX_DATA_INCLUDE_OM(_user)  ((_user)->bit_tx_data_include_om)
#define MAC_USER_ARP_PROBE_CLOSE_HTC(_user) ((_user)->bit_arp_probe_close_htc)
#endif

/* vht cap ie Extended nss bw support �ֶ�ö�� */
typedef enum {
    WLAN_EXTEND_NSS_BW_SUPP0,
    WLAN_EXTEND_NSS_BW_SUPP1,
    WLAN_EXTEND_NSS_BW_SUPP2,
    WLAN_EXTEND_NSS_BW_SUPP3,

    WLAN_EXTEND_NSS_BW_SUPP_BUTT
} wlan_extend_nss_bw_supp_enum;

/* mac user�ṹ��, hmac_user_stru��dmac_user_stru�������� */
typedef struct {
    /* ��ǰVAP������AP��STAģʽ�������ֶ�Ϊuser��STA��APʱ�����ֶΣ�������ֶ���ע��!!! */
    oal_dlist_head_stru st_user_dlist;              /* �����û����� */
    oal_dlist_head_stru st_user_hash_dlist;         /* ����hash���� */
    uint16_t us_user_hash_idx;                    /* ����ֵ(����) */
    uint8_t auc_user_mac_addr[WLAN_MAC_ADDR_LEN]; /* user��Ӧ��MAC��ַ */
    /* user��Ӧ��Դ������ֵ; userΪSTAʱ����ʾ���ڹ���֡�е�AID��ֵΪ�û�����Դ������ֵ1~32(Э��涨��ΧΪ1~2007) */
    uint16_t us_assoc_id;
    oal_bool_enum_uint8 en_is_multi_user;
    uint8_t uc_vap_id;                                /* vap ID */
    uint8_t uc_device_id;                             /* �豸ID */
    uint8_t uc_chip_id;                               /* оƬID */
    wlan_protocol_enum_uint8 en_protocol_mode;          /* �û�����Э�� */
    wlan_protocol_enum_uint8 en_avail_protocol_mode;    /* �û���VAPЭ��ģʽ����, ���㷨���� */
    wlan_protocol_enum_uint8 en_cur_protocol_mode;      /* Ĭ��ֵ��en_avail_protocol_modeֵ��ͬ, ���㷨�����޸� */
    wlan_nss_enum_uint8 en_avail_num_spatial_stream;    /* Tx��Rx֧��Nss�Ľ���,���㷨���� */
    wlan_nss_enum_uint8 en_user_max_cap_nss;            /* �û�֧�ֵ����ռ������� */
    wlan_nss_enum_uint8 en_avail_bf_num_spatial_stream; /* �û�֧�ֵ�Beamforming�ռ������� */
    oal_bool_enum_uint8 en_port_valid;                  /* 802.1X�˿ںϷ��Ա�ʶ */
    uint8_t uc_is_user_alloced;                       /* ��־��user��Դ�Ƿ��Ѿ������� */

    mac_rate_stru st_avail_op_rates;           /* �û���VAP���õ�11a/b/g���ʽ��������㷨���� */
    mac_user_tx_param_stru st_user_tx_info;    /* TX��ز��� */
    wlan_bw_cap_enum_uint8 en_bandwidth_cap;   /* �û�����������Ϣ */
    wlan_bw_cap_enum_uint8 en_avail_bandwidth; /* �û���VAP������������,���㷨���� */
    wlan_bw_cap_enum_uint8 en_cur_bandwidth;   /* Ĭ��ֵ��en_avail_bandwidth��ͬ,���㷨�����޸� */

    mac_user_asoc_state_enum_uint8 en_user_asoc_state; /* �û�����״̬ */

    uint16_t us_amsdu_maxsize;                    /* ���֧��amsdu ����Э�̺�amsdu���͵����ֵ */
    uint8_t uc_res_req_buff_threshold_exp; /* ��Դ���󻺴���ֵָ�� */
    wlan_nss_enum_uint8 en_smps_opmode_notify_nss;  /* smps/opmode �޸�nss ֵ */

#ifdef _PRE_WLAN_FEATURE_WMMAC
    mac_ts_stru st_ts_info[WLAN_WME_AC_BUTT]; /* ����ts�����Ϣ�� */
    uint8_t need_degrade[WLAN_WME_AC_BUTT];
#endif

    mac_user_cap_info_stru st_cap_info; /* user����������Ϣλ */
    mac_user_ht_hdl_stru st_ht_hdl;     /* HT capability IE�� operation IE�Ľ�����Ϣ */
    mac_vht_hdl_stru st_vht_hdl;        /* VHT capability IE�� operation IE�Ľ�����Ϣ */

#if defined(_PRE_WLAN_FEATURE_11AX)
    uint32_t bit_tx_data_include_om  : 1,             /* �������ݺ���om�ֶ� */
               bit_tx_data_include_htc : 1,             /* �������ݺ���htcͷ */
               bit_have_recv_he_rom_flag  : 1,          /* ���յ�htc om�޸Ĺ�bw/nss */
               bit_arp_probe_close_htc    : 1,
               bit_have_send_tb_ppdu_flag : 1,          /* �Ը�user ���͹�tb ppdu ���� */
               bit_he_rsv              : 27;
    wlan_nss_enum_uint8 en_he_rom_nss;                  /* �յ� htc rom �޸�nssֵ */
    wlan_bw_cap_enum_uint8 en_he_rom_bw;                /* �յ� htc rom �޸�bwֵ */

    uint8_t  uc_rsv[NUM_32_BYTES];
#else
    oal_bool_enum_uint8 en_he_hdl_rom_no_use; /* rom ��λ�ò��ܸı� */
    uint8_t auc_he_hdl_rom_rsv1[NUM_28_BYTES];
    uint8_t bit_he_hdl_rom_rsv2 : 5,
              bit_he_hdl_rom_vht_present_no_use : 1, /* rom ��λ�ò��ܸı� */
              bit_he_hdl_rom_rsv3 : 2;
    uint8_t auc_he_hdl_rom_rsv4[NUM_4_BYTES];
    uint8_t uc_he_hdl_rom_bw_no_use;               /* rom ��λ�ò��ܸı� */
    uint8_t uc_he_hdl_rom_center_freq_seg0_no_use; /* rom ��λ�ò��ܸı� */
    uint8_t auc_he_hdl_rom_rsv5[NUM_2_BYTES];
#endif

    uint8_t auc_alg_priv_cap[WLAN_WME_AC_BUTT];

    uint16_t us_auth_alg_num;
    mac_key_mgmt_stru st_key_info;

#if defined(_PRE_WLAN_FEATURE_11AX)
    mac_he_hdl_stru st_he_hdl; /* HE Capability IE */
#else
    uint8_t auc_rsv[NUM_4_BYTES];
#endif
#ifdef _PRE_WLAN_FEATURE_HIEX
    mac_hiex_cap_stru st_hiex_cap;
#endif
} mac_user_stru;

#ifdef _PRE_WLAN_FEATURE_HIEX
#define MAC_USER_GET_HIEX_CAP(user)     (&((user)->st_hiex_cap))
#define MAC_USER_HIEX_ENABLED(user)     ((user)->st_hiex_cap.bit_hiex_enable)
#define MAC_USER_HIMIT_ENABLED(user)    ((user)->st_hiex_cap.bit_himit_enable)
#endif

#define MAC_USR_UAPSD_EN   0x01 // U-APSDʹ��
#define MAC_USR_UAPSD_TRIG 0x02 // U-APSD���Ա�trigger
#define MAC_USR_UAPSD_SP   0x04 // u-APSD��һ��Service Period������

#define MAC_11I_ASSERT(_cond, _errcode) \
    do {                                \
        if (!(_cond)) {                 \
            return _errcode;            \
        }                               \
    } while (0);

#define MAC_11I_IS_PTK(en_macaddr_is_zero, en_pairwise) \
    ((OAL_TRUE != (en_macaddr_is_zero)) && (OAL_TRUE == (en_pairwise)))


OAL_STATIC OAL_INLINE uint8_t mac_user_get_ra_lut_index(uint8_t *puc_index_table,
    uint16_t us_start, uint16_t us_stop)
{
    return oal_get_lut_index(puc_index_table, WLAN_ACTIVE_USER_IDX_BMAP_LEN,
        MAC_INVALID_RA_LUT_IDX, us_start, us_stop);
}

OAL_STATIC OAL_INLINE void mac_user_del_ra_lut_index(uint8_t *puc_index_table, uint8_t uc_ra_lut_index)
{
    oal_del_lut_index(puc_index_table, uc_ra_lut_index);
}

OAL_STATIC OAL_INLINE void mac_user_set_smps_opmode_notify_nss(mac_user_stru *pst_mac_user, uint8_t uc_nss)
{
    pst_mac_user->en_smps_opmode_notify_nss = uc_nss;
}

OAL_STATIC OAL_INLINE wlan_nss_enum_uint8 mac_user_get_smps_opmode_notify_nss(mac_user_stru *pst_mac_user)
{
    return pst_mac_user->en_smps_opmode_notify_nss;
}

#if defined(_PRE_WLAN_FEATURE_11AX)
OAL_STATIC OAL_INLINE void mac_user_set_he_rom_nss(mac_user_stru *pst_mac_user, uint8_t uc_nss)
{
    pst_mac_user->en_he_rom_nss = uc_nss;
}
OAL_STATIC OAL_INLINE void mac_user_set_he_rom_bw(mac_user_stru *pst_mac_user, uint8_t uc_bw)
{
    pst_mac_user->en_he_rom_bw = uc_bw;
}
#endif
OAL_STATIC OAL_INLINE wlan_nss_enum_uint8 mac_user_get_avail_num_spatial_stream(mac_user_stru *pst_mac_user)
{
    return pst_mac_user->en_avail_num_spatial_stream;
}

/* 10 �������� */
uint32_t mac_user_add_wep_key(mac_user_stru *pst_mac_user, uint8_t uc_key_index,
    mac_key_params_stru *pst_key);
uint32_t mac_user_add_rsn_key(mac_user_stru *pst_mac_user, uint8_t uc_key_index,
    mac_key_params_stru *pst_key);
uint32_t mac_user_add_bip_key(mac_user_stru *pst_mac_user, uint8_t uc_key_index,
    mac_key_params_stru *pst_key);
wlan_priv_key_param_stru *mac_user_get_key(mac_user_stru *pst_mac_user, uint8_t uc_key_id);

uint32_t mac_user_init(mac_user_stru *pst_mac_user, uint16_t us_user_idx, uint8_t *puc_mac_addr,
    uint8_t uc_chip_id, uint8_t uc_device_id, uint8_t uc_vap_id);

void mac_user_set_port(mac_user_stru *pst_mac_user, oal_bool_enum_uint8 en_port_valid);

void mac_user_avail_bf_num_spatial_stream(mac_user_stru *pst_mac_user, uint8_t uc_value);
void mac_user_set_avail_num_spatial_stream(mac_user_stru *pst_mac_user, uint8_t uc_value);
void mac_user_set_num_spatial_stream(mac_user_stru *pst_mac_user, uint8_t uc_value);
void mac_user_set_bandwidth_cap(mac_user_stru *pst_mac_user,
    wlan_bw_cap_enum_uint8 en_bandwidth_value);
void mac_user_set_bandwidth_info(mac_user_stru *pst_mac_user,
    wlan_bw_cap_enum_uint8 en_avail_bandwidth, wlan_bw_cap_enum_uint8 en_cur_bandwidth);
void mac_user_get_sta_cap_bandwidth(mac_user_stru *pst_mac_user,
    wlan_bw_cap_enum_uint8 *pen_bandwidth_cap);
uint32_t mac_user_update_bandwidth(mac_user_stru *pst_mac_user, wlan_bw_cap_enum_uint8 en_bwcap);
uint32_t mac_user_update_ap_bandwidth_cap(mac_user_stru *pst_mac_user);
uint32_t mac_user_update_ap_bandwidth_cap_cb(mac_user_stru *pst_mac_user);

void mac_user_set_assoc_id(mac_user_stru *pst_mac_user, uint16_t us_assoc_id);
void mac_user_set_avail_protocol_mode(mac_user_stru *pst_mac_user,
    wlan_protocol_enum_uint8 en_avail_protocol_mode);
void mac_user_set_cur_protocol_mode(mac_user_stru *pst_mac_user,
    wlan_protocol_enum_uint8 en_cur_protocol_mode);
void mac_user_set_cur_bandwidth(mac_user_stru *pst_mac_user, wlan_bw_cap_enum_uint8 en_cur_bandwidth);
void mac_user_set_protocol_mode(mac_user_stru *pst_mac_user, wlan_protocol_enum_uint8 en_protocol_mode);
void mac_user_set_asoc_state(mac_user_stru *pst_mac_user, mac_user_asoc_state_enum_uint8 en_value);
void mac_user_set_avail_op_rates(mac_user_stru *pst_mac_user, uint8_t uc_rs_nrates, uint8_t *puc_rs_rates);
void mac_user_set_vht_hdl(mac_user_stru *pst_mac_user, mac_vht_hdl_stru *pst_vht_hdl);
void mac_user_get_vht_hdl(mac_user_stru *pst_mac_user, mac_vht_hdl_stru *pst_ht_hdl);

#ifdef _PRE_WLAN_FEATURE_11AX
void mac_user_set_he_hdl(mac_user_stru *pst_mac_user, mac_he_hdl_stru *pst_he_hdl);
void mac_user_get_he_hdl(mac_user_stru *pst_mac_user, mac_he_hdl_stru *pst_he_hdl);
void mac_user_set_he_capable(mac_user_stru *pst_mac_user, oal_bool_enum_uint8 en_he_capable);
oal_bool_enum_uint8 mac_user_get_he_capable(mac_user_stru *pst_mac_user);
#endif

void mac_user_set_ht_hdl(mac_user_stru *pst_mac_user, mac_user_ht_hdl_stru *pst_ht_hdl);
void mac_user_get_ht_hdl(mac_user_stru *pst_mac_user, mac_user_ht_hdl_stru *pst_ht_hdl);
void mac_user_set_ht_capable(mac_user_stru *pst_mac_user, oal_bool_enum_uint8 en_ht_capable);
#ifdef _PRE_WLAN_FEATURE_SMPS
void mac_user_set_sm_power_save(mac_user_stru *pst_mac_user, uint8_t uc_sm_power_save);
#endif
void mac_user_set_pmf_active(mac_user_stru *pst_mac_user, oal_bool_enum_uint8 en_pmf_active);
void mac_user_set_barker_preamble_mode(mac_user_stru *pst_mac_user,
    oal_bool_enum_uint8 en_barker_preamble_mode);
void mac_user_set_qos(mac_user_stru *pst_mac_user, oal_bool_enum_uint8 en_qos_mode);
void mac_user_set_spectrum_mgmt(mac_user_stru *pst_mac_user, oal_bool_enum_uint8 en_spectrum_mgmt);
void mac_user_set_apsd(mac_user_stru *pst_mac_user, oal_bool_enum_uint8 en_apsd);

void mac_user_init_key(mac_user_stru *pst_mac_user);

void mac_user_set_key(mac_user_stru *pst_multiuser, wlan_cipher_key_type_enum_uint8 en_keytype,
    wlan_ciper_protocol_type_enum_uint8 en_ciphertype, uint8_t uc_keyid);

uint32_t mac_user_update_wep_key(mac_user_stru *pst_mac_usr, uint16_t us_multi_user_idx);
oal_bool_enum_uint8 mac_addr_is_zero(const unsigned char *puc_mac);
void *mac_res_get_mac_user(uint16_t us_idx);

uint32_t mac_user_init_rom(mac_user_stru *pst_mac_user, uint16_t us_user_idx);
void mac_user_set_num_spatial_stream_160M(mac_user_stru *pst_mac_user,
    uint8_t uc_value);
uint8_t mac_user_get_sta_cap_bandwidth_11ac(wlan_channel_band_enum_uint8 en_band,
    mac_user_ht_hdl_stru *pst_mac_ht_hdl, mac_vht_hdl_stru *pst_mac_vht_hdl,
    mac_user_stru *pst_mac_user);
oal_bool_enum_uint8 mac_user_get_port(mac_user_stru *mac_user);
#endif /* end of mac_user.h */
