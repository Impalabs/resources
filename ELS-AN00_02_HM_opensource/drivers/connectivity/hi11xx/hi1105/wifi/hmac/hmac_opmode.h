

#ifndef __HMAC_OPMODE_H__
#define __HMAC_OPMODE_H__

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY

/* 1 ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "oam_ext_if.h"
#include "mac_resource.h"

#include "mac_user.h"
#include "mac_ie.h"
#include "hmac_vap.h"
#include "hmac_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_OPMODE_H
/* 2 �궨�� */
/* 3 ö�ٶ��� */
/* 4 ȫ�ֱ������� */
/* 5 ��Ϣͷ���� */
/* 6 ��Ϣ���� */
/* 7 STRUCT���� */
/* 8 UNION���� */
/* 9 OTHERS���� */
/* 10 �������� */
uint32_t hmac_check_opmode_notify(hmac_vap_stru *hmac_vap, uint8_t *mac_hdr,
    uint8_t *payload_offset, uint32_t msg_len, hmac_user_stru *hmac_user);
uint32_t hmac_mgmt_rx_opmode_notify_frame(hmac_vap_stru *hmac_vap,
    hmac_user_stru *hmac_user, oal_netbuf_stru *netbuf);
void mac_set_opmode_field(uint8_t *pst_vap, uint8_t *puc_buffer,
                          wlan_nss_enum_uint8 en_nss);
void mac_set_opmode_notify_ie(uint8_t *pst_vap, uint8_t *puc_buffer,
                              uint8_t *puc_ie_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* end of _PRE_WLAN_FEATURE_OPMODE_NOTIFY */

#endif /* end of hmac_opmode.h */
