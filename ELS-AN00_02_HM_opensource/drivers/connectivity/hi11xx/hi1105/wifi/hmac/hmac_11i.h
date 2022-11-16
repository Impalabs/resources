

#ifndef __HMAC_11I_H__
#define __HMAC_11I_H__

/* 1 ͷ�ļ����� */
#include "mac_resource.h"
#include "hmac_vap.h"
#include "hmac_user.h"


#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_11I_H

/* 2 �궨�� */
/* 3 ö�ٶ��� */
/* 4 ȫ�ֱ������� */
/* 5 ��Ϣͷ���� */
/* 6 ��Ϣ���� */
/* 7 STRUCT���� */
typedef struct {
    oal_net_device_stru *pst_netdev;
    uint8_t uc_key_index;
    oal_bool_enum_uint8 en_pairwise;
    uint8_t auc_resv1[NUM_2_BYTES];
    uint8_t *puc_mac_addr;
    void *cookie;
    void (*callback)(void *cookie, oal_key_params_stru *key_param);
} hmac_getkey_param_stru; // only host

/* 8 UNION���� */
/* 9 OTHERS���� */
/* 10 �������� */
uint32_t hmac_init_security(mac_vap_stru *pst_mac_vap, uint8_t *puc_addr, uint8_t uc_mac_len);
uint32_t hmac_sta_protocol_down_by_chipher(mac_vap_stru *pst_mac_vap, mac_bss_dscr_stru *pst_bss_dscr);
uint32_t hmac_en_mic(hmac_vap_stru *pst_hmac_vap,
                     hmac_user_stru *pst_hmac_user, oal_netbuf_stru *pst_netbuf, uint8_t *puc_iv_len);
uint32_t hmac_de_mic(hmac_user_stru *pst_hmac_user, oal_netbuf_stru *pst_netbuf);
uint32_t hmac_rx_tkip_mic_failure_process(frw_event_mem_stru *pst_event_mem);
uint32_t hmac_11i_ether_type_filter(hmac_vap_stru *pst_vap,
                                    hmac_user_stru *pst_hmac_user, uint16_t us_ether_type);
#endif /* end of hmac_11i.h */
