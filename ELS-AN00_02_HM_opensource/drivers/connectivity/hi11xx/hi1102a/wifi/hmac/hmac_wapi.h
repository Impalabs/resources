

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "oal_net.h"
#include "mac_resource.h"
#include "hmac_vap.h"
#include "hmac_user.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_WAPI_H
#ifdef _PRE_WLAN_FEATURE_WAPI
/*****************************************************************************/
/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define WAPI_UCAST_INC                  2       /* ���ͻ��߽��յ���֡,pn�Ĳ���ֵ */
#define WAPI_BCAST_INC                  1       /* ���ͻ��߽����鲥֡,pn�Ĳ���ֵ */
#define WAPI_WAI_TYPE                  (oal_uint16)0x88B4 /* wapi����̫���� */

#define WAPI_BCAST_KEY_TYPE             1
#define WAPI_UCAST_KEY_TYPE             0

#define SMS4_MIC_LEN                    (oal_uint8)16     /* SMS4���MIC�ĳ��� */

#define SMS4_PN_LEN                     16     /* wapi pn�ĳ��� */
#define SMS4_KEY_IDX                    1      /* WAPIͷ�� keyidxռ1���ֽ� */
#define SMS4_WAPI_HDR_RESERVE           1      /* WAPIͷ�б����ֶ� */
#define HMAC_WAPI_HDR_LEN               (oal_uint8)(SMS4_PN_LEN + SMS4_KEY_IDX + SMS4_WAPI_HDR_RESERVE)
#define WAPI_PDU_LEN                    2      /* wapiͷ�У�wapi pdu len�ֶ���ռ�ֽ��� */
#define SMS4_PADDING_LEN                16     /* mic data����16�ֽڶ��� */

#define SMS4_MIC_PART1_QOS_LEN          48 /* ����Э�飬�����qos�ֶΣ�mic��һ����16�ֽڶ����ĳ��� */
#define SMS4_MIC_PART1_NO_QOS_LEN       32 /* ����Э�飬���û��qos�ֶΣ�mic��һ����16�ֽڶ����ĳ��� */

#define WAPI_IE_VERSION                     1   /* wapi��version */
#define WAPI_IE_VER_SIZE                    2   /* wapi ver-ie ��ռ�ֽ��� */
#define WAPI_IE_SUIT_TYPE_SIZE              1   /* suit type size */
#define WAPI_IE_WAPICAP_SIZE                2   /* wapi cap�ֶ���ռ�ֽ��� */
#define WAPI_IE_BKIDCNT_SIZE                2   /* wapi bkid���ֶ���ռ�ֽ��� */
#define WAPI_IE_BKID_SIZE                   16  /* һ��bkid��ռ�ֽ��� */
#define WAPI_IE_OUI_SIZE                    3   /* wapi oui�ֽ��� */
#define WAPI_IE_SMS4                        1   /* wapi��������Ϊsms4 */
#define WAPI_IE_SUITCNT_SIZE                2   /* wapi suit count��ռ�ֽ��� */

#define wapi_is_port_valid(pst_wapi) ((pst_wapi)->uc_port_valid)

#define wapi_is_work(pst_hmac_vap)   ((pst_hmac_vap)->uc_wapi)
#define padding(x, size)           (((x) + (size) - 1) & (~ ((size) - 1)))

#ifdef _PRE_WAPI_DEBUG
#define wapi_tx_drop_inc(pst_wapi)              pst_wapi->st_debug.ultx_ucast_drop++
#define wapi_tx_wai_inc(pst_wapi)               pst_wapi->st_debug.ultx_wai++
#define wapi_tx_port_valid(pst_wapi)            pst_wapi->st_debug.ultx_port_valid++
#define wapi_rx_port_valid(pst_wapi)            pst_wapi->st_debug.ulrx_port_valid++
#define wapi_rx_idx_err(pst_wapi)               pst_wapi->st_debug.ulrx_idx_err++
#define wapi_rx_netbuf_len_err(pst_wapi)        pst_wapi->st_debug.ulrx_netbuff_len_err++
#define wapi_rx_idx_update_err(pst_wapi)        pst_wapi->st_debug.ulrx_idx_update_err++
#define wapi_rx_key_en_err(pst_wapi)            pst_wapi->st_debug.ulrx_key_en_err++

#define wapi_rx_pn_odd_err(pst_wapi, pn)                                         \
    do {(pst_wapi)->st_debug.ulrx_pn_odd_err++;                                  \
        if (memcpy_s((pst_wapi)->st_debug.aucrx_pn, WAPI_PN_LEN, pn, WAPI_PN_LEN) != EOK) {  \
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wapi_rx_pn_odd_err::memcpy fail!");   \
        }                                                                        \
}while (0)

#define wapi_rx_pn_replay_err(pst_wapi, pn)                                         \
    do {(pst_wapi)->st_debug.ulrx_pn_replay_err++;                                  \
        if (memcpy_s((pst_wapi)->st_debug.aucrx_pn, WAPI_PN_LEN, pn, WAPI_PN_LEN) != EOK) { \
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wapi_rx_pn_replay_err::memcpy fail!");   \
        }                                                                           \
}while (0)

#define wapi_rx_memalloc_err(pst_wapi)          pst_wapi->st_debug.ulrx_memalloc_err++
#define wapi_rx_mic_err(pst_wapi)               pst_wapi->st_debug.ulrx_mic_calc_fail++
#define wapi_rx_decrypt_ok(pst_wapi)            pst_wapi->st_debug.ulrx_decrypt_ok++

#define wapi_tx_memalloc_err(pst_wapi)          pst_wapi->st_debug.ultx_memalloc_err++
#define wapi_tx_mic_err(pst_wapi)               pst_wapi->st_debug.ultx_mic_calc_fail++
#define wapi_tx_encrypt_ok(pst_wapi)            pst_wapi->st_debug.ultx_encrypt_ok++

#else
#define wapi_tx_drop_inc(pst_wapi)
#define wapi_tx_wai_inc(pst_wapi)
#define wapi_tx_port_valid(pst_wapi)
#define wapi_rx_port_valid(pst_wapi)
#define wapi_rx_idx_err(pst_wapi)
#define wapi_rx_netbuf_len_err(pst_wapi)
#define wapi_rx_idx_update_err(pst_wapi)
#define wapi_rx_key_en_err(pst_wapi)
#define wapi_rx_pn_odd_err(pst_wapi, pn)
#define wapi_rx_pn_replay_err(pst_wapi, pn)
#define wapi_rx_memalloc_err(pst_wapi)
#define wapi_rx_mic_err(pst_wapi)
#define wapi_rx_decrypt_ok(pst_wapi)
#define wapi_tx_memalloc_err(pst_wapi)
#define wapi_tx_mic_err(pst_wapi)
#define wapi_tx_wai_drop_inc(pst_wapi)
#define wapi_tx_encrypt_ok(pst_wapi)

#endif /* #ifdef WAPI_DEBUG_MODE */
/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/
/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/
/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/
/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
typedef struct {
    oal_uint8   auc_framectl[2];        /* ֡���� */
    oal_uint8   auc_adress1[OAL_MAC_ADDR_LEN];         /* ��ַ1 */
    oal_uint8   auc_adress2[OAL_MAC_ADDR_LEN];         /* ��ַ2 */
    oal_uint8   auc_seqctl[2];          /* ���п��� */
    oal_uint8   auc_adress3[OAL_MAC_ADDR_LEN];         /* ��ַ3 */
    oal_uint8   auc_adress4[OAL_MAC_ADDR_LEN];         /* ��ַ4 */
}wapi_mic_hdr_stru;

/*****************************************************************************
  8 UNION����
*****************************************************************************/
/*****************************************************************************
  9 OTHERS����
*****************************************************************************/
/*****************************************************************************
  10 ��������
*****************************************************************************/
extern oal_void hmac_wapi_deinit(hmac_wapi_stru *pst_wapi);
extern oal_void hmac_wapi_init(hmac_wapi_stru *pst_wapi, oal_uint8 uc_pairwise);

#ifdef _PRE_WAPI_DEBUG
oal_uint32 hmac_wapi_display_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_usr_idx);
#endif
extern oal_uint32 hmac_wapi_add_key(hmac_wapi_stru *pst_wapi, oal_uint8 uc_key_index, oal_uint8 *puc_key);
extern oal_uint32 hmac_wapi_reset_port(hmac_wapi_stru *pst_wapi);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


