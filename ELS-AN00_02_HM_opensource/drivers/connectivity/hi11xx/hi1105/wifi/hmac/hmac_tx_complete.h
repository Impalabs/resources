

#ifndef __HMAC_TX_COMPLETE_H__
#define __HMAC_TX_COMPLETE_H__

#include "mac_vap.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_TX_COMPLETE_H
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    uint16_t user_id;    /* �û�AID */
    uint16_t read_index; /* RING��ָ�� */
    uint16_t ba_ssn;     /* BA������ʼ���к� */
    uint8_t tid;         /* �û�TID */
    uint8_t ba_info_vld; /* ��Ч��� */
    uint32_t *ba_bitmap; /* ָ��ba info�е�ba bitmap, ���ڻ�ȡmsdu���ͽ�� */
} mac_tx_ba_info_stru;

uint32_t hmac_tx_complete_event_handler(frw_event_mem_stru *event_mem);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
