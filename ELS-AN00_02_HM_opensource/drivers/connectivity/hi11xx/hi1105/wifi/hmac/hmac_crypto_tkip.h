

#ifndef __HMAC_CRYPTO_FRAME_H__
#define __HMAC_CRYPTO_FRAME_H__

/* 1 ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "mac_vap.h"

/* 2 �궨�� */
#define michael_block(l, r)  do { \
        (r) ^= rotl((l), 17);   \
        (l) += (r);             \
        (r) ^= xswap((l));      \
        (l) += (r);             \
        (r) ^= rotl((l), 3);    \
        (l) += (r);             \
        (r) ^= rotr((l), 2);    \
        (l) += (r);             \
    } while (0)
#define IEEE80211_WEP_MICLEN 8 /* trailing MIC */

#define IEEE80211_FC1_DIR_NODS   0x00 /* STA->STA */
#define IEEE80211_FC1_DIR_TODS   0x01 /* STA->AP  */
#define IEEE80211_FC1_DIR_FROMDS 0x02 /* AP ->STA */
#define IEEE80211_FC1_DIR_DSTODS 0x03 /* AP ->AP  */

#define IEEE80211_NON_QOS_SEQ     16 /* index for non-QoS (including management) sequence number space */
#define IEEE80211_FC0_TYPE_MASK   0x0c
#define IEEE80211_FC0_SUBTYPE_QOS 0x80
#define IEEE80211_FC0_TYPE_DATA   0x08

#define WEP_IV_FIELD_SIZE 4 /* wep IV field size */
#define EXT_IV_FIELD_SIZE 4 /* ext IV field size */

/* 3 ö�ٶ��� */
/* 4 ȫ�ֱ������� */
/* 5 ��Ϣͷ���� */
/* 6 ��Ϣ���� */
/* 7 STRUCT���� */
/* 8 UNION���� */
/* 9 OTHERS���� */
/* 10 �������� */
OAL_STATIC OAL_INLINE uint32_t rotl(uint32_t val, int32_t bits)
{
    return (val << (uint32_t)bits) | (val >> (uint32_t)(NUM_32_BITS - bits));
}

OAL_STATIC OAL_INLINE uint32_t rotr(uint32_t val, int32_t bits)
{
    return (val >> (uint32_t)bits) | (val << (uint32_t)(NUM_32_BITS - bits));
}

uint32_t hmac_crypto_tkip_enmic(wlan_priv_key_param_stru *pst_key, oal_netbuf_stru *pst_netbuf);
uint32_t hmac_crypto_tkip_demic(wlan_priv_key_param_stru *pst_key, oal_netbuf_stru *pst_netbuf);

#endif /* end of hmac_crypto_tkip.h */
