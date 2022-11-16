

#ifndef __MAC_BOARD_H__
#define __MAC_BOARD_H__

/* 1 ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "frw_ext_if.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "mac_vap.h"
#include "mac_common.h"
#include "host_hal_ops.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_BOARD_H

/* 4 ȫ�ֱ������� */
/* HOST CRX�ӱ� */
extern frw_event_sub_table_item_stru g_ast_host_dmac_crx_table[HMAC_TO_DMAC_SYN_BUTT];

/* DMACģ�飬HOST_DRX�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_host_tx_dmac_drx[DMAC_TX_HOST_DRX_BUTT];

/* DMACģ�飬WLAN_DTX�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_hmac_tx_wlan_dtx[DMAC_TX_WLAN_DTX_BUTT];

/* DMACģ�飬WLAN_CTX�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_host_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_BUTT];

/* DMACģ��,WLAN_DRX�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_dmac_wlan_drx_event_sub_table[HAL_WLAN_DRX_EVENT_SUB_TYPE_BUTT];

/* DMACģ�飬�����ȼ��¼�������ע��ṹ�嶨�� */
extern frw_event_sub_table_item_stru g_ast_hmac_high_prio_event_sub_table[HAL_EVENT_DMAC_HIGH_PRIO_SUB_TYPE_BUTT];

/* DMACģ��,WLAN_CRX�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_dmac_wlan_crx_event_sub_table[HAL_WLAN_CRX_EVENT_SUB_TYPE_BUTT];

/* DMACģ�飬TX_COMP�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_hmac_tx_comp_event_sub_table[HAL_TX_COMP_SUB_TYPE_BUTT];

/* WLAN_DTX �¼������ͱ� */
extern frw_event_sub_table_item_stru g_ast_hmac_wlan_dtx_event_sub_table[DMAC_TX_WLAN_DTX_BUTT];

/* HMACģ�� WLAN_DRX�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_hmac_wlan_drx_event_sub_table[DMAC_WLAN_DRX_EVENT_SUB_TYPE_BUTT];

/* HMACģ�� WLAN_CRX�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_BUTT];

/* HMACģ�� ����HOST��������¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_SYN_BUTT];

/* HMACģ�� MISC��ɢ�¼�������ע��ṹ���� */
extern frw_event_sub_table_item_stru g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_BUTT];

extern frw_event_sub_table_item_stru g_ast_hmac_ddr_drx_sub_table[HAL_WLAN_DDR_DRX_EVENT_SUB_TYPE_BUTT];

void event_fsm_unregister(void);

void event_fsm_table_register(void);

#endif /* end of mac_board */



