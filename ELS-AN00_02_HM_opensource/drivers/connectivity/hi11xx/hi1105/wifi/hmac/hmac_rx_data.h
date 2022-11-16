

#ifndef __HMAC_RX_DATA_H__
#define __HMAC_RX_DATA_H__

/* 1 ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "frw_ext_if.h"
#include "mac_frame.h"

#include "hmac_user.h"
#include "oal_net.h"
#include "hmac_tcp_opt.h"
#include "hmac_device.h"
#include "hmac_vap.h"
#include "hmac_resource.h"
#include "host_hal_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_RX_DATA_H
/* 2 �궨�� */
#define mac_tid_is_valid(tid)         (((uint16_t)(tid)) < WLAN_TID_MAX_NUM)
#define HMAC_RX_DATA_ETHER_OFFSET_LENGTH 6      /* ������lan�İ���protocol type����̫ͷ��Ҫ��ǰƫ��6 */
#define HMAC_RX_SMALL_PKT_MIPS_LIMIT     30000L /* rxС��MIPS���ޣ���ǰΪ30us */
/* 3 ö�ٶ��� */
/* 4 ȫ�ֱ������� */
/* 5 ��Ϣͷ���� */
/* 6 ��Ϣ���� */
/* 7 STRUCT���� */
/* HMAC ���� TKIP MIC FAILE �쳣���ݸ���ʱ���Ĳ��� */
typedef struct {
    uint8_t uc_vap_id; /* ���ݸ���ʱ���� vap_id */
    uint8_t uc_rsv[3]; // 3����4�ֽڶ��룬�����ֶ�
    frw_timeout_stru *pst_timeout; /* MIC faile ����ʱ����Դ */
} hmac_drx_mic_faile_stru;
typedef struct {
    hal_host_device_stru *hal_dev;
    hmac_vap_stru *hmac_vap;
    hmac_user_stru *hmac_user;
    oal_netbuf_stru *netbuf;
    mac_rx_ctl_stru *cb;
} hmac_host_rx_context_stru;

/* ����MPDU��MSDU�Ĵ���״̬�Ľṹ��Ķ��� */
typedef struct {
    oal_netbuf_stru *pst_curr_netbuf;     /* ��ǰ�����netbufָ�� */
    uint8_t *puc_curr_netbuf_data;      /* ��ǰ�����netbuf��dataָ�� */
    uint16_t us_submsdu_offset;         /* ��ǰ�����submsdu��ƫ����,   */
    uint8_t uc_msdu_nums_in_netbuf;     /* ��ǰnetbuf�������ܵ�msdu��Ŀ */
    uint8_t uc_procd_msdu_in_netbuf;    /* ��ǰnetbuf���Ѵ����msdu��Ŀ */
    uint8_t uc_netbuf_nums_in_mpdu;     /* ��ǰMPDU���е��ܵ�netbuf����Ŀ */
    uint8_t uc_procd_netbuf_nums;       /* ��ǰMPDU���Ѵ����netbuf����Ŀ */
    uint8_t uc_procd_msdu_nums_in_mpdu; /* ��ǰMPDU���Ѵ����MSDU��Ŀ */

    uint8_t uc_flag;
} dmac_msdu_proc_state_stru;

/* ÿһ��MSDU���������ݵĽṹ��Ķ��� */
typedef struct {
    uint8_t auc_sa[WLAN_MAC_ADDR_LEN]; /* MSDU���͵�Դ��ַ */
    uint8_t auc_da[WLAN_MAC_ADDR_LEN]; /* MSDU���յ�Ŀ�ĵ�ַ */
    uint8_t auc_ta[WLAN_MAC_ADDR_LEN]; /* MSDU���յķ��͵�ַ */
    uint8_t uc_priority;
    uint8_t auc_resv[1];

    oal_netbuf_stru *pst_netbuf; /* MSDU��Ӧ��netbufָ��(����ʹclone��netbuf) */
} dmac_msdu_stru;


typedef enum {
    HMAC_RX_FRAME_CTRL_GOON,   /* ������֡��Ҫ�������� */
    HMAC_RX_FRAME_CTRL_DROP,   /* ������֡��Ҫ���� */
    HMAC_RX_FRAME_CTRL_BUTT
} hmac_rx_frame_ctrl_enum;
/* 8 UNION���� */
/* 9 OTHERS���� */
/* 10 �������� */
uint32_t hmac_rx_process_data_event(frw_event_mem_stru *pst_event_mem);
uint32_t hmac_dev_rx_process_data_event(frw_event_mem_stru *pst_event_mem);
void hmac_rx_process_data_ap_tcp_ack_opt(hmac_vap_stru *pst_vap,
    oal_netbuf_head_stru *pst_netbuf_header);
uint32_t hmac_rx_process_data_sta_tcp_ack_opt(hmac_vap_stru *pst_vap,
    oal_netbuf_head_stru *pst_netbuf_header);
#ifdef _PRE_WLAN_DFT_DUMP_FRAME
void hmac_rx_report_eth_frame(mac_vap_stru *pst_mac_vap,
    oal_netbuf_stru *pst_netbuf);
#endif
void hmac_rx_free_netbuf(oal_netbuf_stru *pst_netbuf, uint16_t us_nums);
void hmac_rx_free_netbuf_list(oal_netbuf_head_stru *pst_netbuf_hdr, uint16_t us_nums_buf);
void hmac_rx_lan_frame(oal_netbuf_head_stru *pst_netbuf_header);

void hmac_rx_mpdu_to_netbuf_list(oal_netbuf_head_stru *netbuf_head, oal_netbuf_stru *netbuf_mpdu);
void hmac_rx_data_sta_proc(oal_netbuf_head_stru *netbuf_header);
void hmac_rx_data_ap_proc(hmac_vap_stru *hmac_vap, oal_netbuf_head_stru *p_netbuf_head);
uint32_t hmac_rx_frame_80211_to_eth(oal_netbuf_stru *pst_netbuf,
    const unsigned char *puc_da, const unsigned char *puc_sa);
oal_bool_enum_uint8 hmac_rx_is_qos_data_frame(mac_rx_ctl_stru *rx_ctl);
void hmac_rx_update_submsdu_rxctl(mac_rx_ctl_stru *dst_rxcb, mac_rx_ctl_stru *src_rxcb);
void hmac_rx_netbuf_list_free(oal_netbuf_stru *netbuf);
void hmac_rx_enqueue(oal_netbuf_stru *netbuf, oal_net_device_stru *net_dev);
void hmac_rx_wlan_to_wlan_proc(hmac_vap_stru *hmac_vap,
    oal_netbuf_head_stru *pw2w_netbuf_hdr);
void hmac_rx_vip_info(mac_vap_stru *vap, uint8_t data_type,
                      oal_netbuf_stru *pst_buf, mac_rx_ctl_stru *pst_rx_ctrl);
uint8_t *hmac_rx_get_eth_da(oal_netbuf_stru *netbuf);
uint32_t hmac_rx_copy_netbuff(oal_netbuf_stru **ppst_dest_netbuf,
                              oal_netbuf_stru *pst_src_netbuf, uint8_t uc_vap_id,
                              mac_ieee80211_frame_stru **ppul_mac_hdr_start_addr);
void hmac_rx_prepare_to_wlan(oal_netbuf_head_stru *netbuf_header, oal_netbuf_stru *netbuf);
uint32_t hmac_rx_data_wnd_proc(hmac_host_rx_context_stru *rx_context,
    hmac_ba_rx_stru *ba_rx_hdl, oal_netbuf_head_stru *rpt_list, uint8_t *buff_is_valid);
uint32_t hmac_rx_location_data_event(frw_event_mem_stru *event_mem);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hmac_rx_data.h */
