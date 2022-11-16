

#ifndef __HMAC_DEGRADATION_H_
#define __HMAC_DEGRADATION_H_

/* 1 ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "mac_vap.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_DEGRADATION_H

/* 2 �궨�� */
#define HMAC_DEGRADATION_WRITELIST_MAX_NUM 64

/* 3 ö�ٶ��� */
typedef struct {
    uint8_t user_mac_addr[WLAN_MAC_ADDR_LEN];
    uint16_t resv;
} hmac_degradation_writelist_stru;

typedef struct {
    hmac_degradation_writelist_stru writelist[HMAC_DEGRADATION_WRITELIST_MAX_NUM];
    uint8_t list_num;          /* �������ܸ��� */
    uint8_t curr_location;     /* ָ��ǰ��Ҫ���µ�λ�� */
    uint8_t resv;
} hmac_degradation_info_stru;

uint8_t hmac_degradation_is_whitelist_type(uint8_t *mac_addr);
void hmac_degradation_whitelist_set(mac_vap_stru *mac_vap);

#endif /* end of __HMAC_MCM_DEGRADATION_H_ */

