

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "mac_board.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_BOARD_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* HOST CRX�ӱ� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_host_crx_table[HMAC_TO_DMAC_SYN_BUTT];

/* DMACģ�飬HOST_DRX�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_tx_host_drx[DMAC_TX_HOST_DRX_BUTT];

/* DMACģ�飬WLAN_DTX�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_tx_wlan_dtx[DMAC_TX_WLAN_DTX_BUTT];

/* DMACģ�飬WLAN_CTX�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_BUTT];

/* DMACģ��,WLAN_DRX�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_wlan_drx_event_sub_table[HAL_WLAN_DRX_EVENT_SUB_TYPE_BUTT];

/* DMACģ�飬ERROR_IRQ�¼���������ע��ṹ�嶨�� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_high_prio_event_sub_table[HAL_EVENT_ERROR_IRQ_SUB_TYPE_BUTT];

/* DMACģ��,WLAN_CRX�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_wlan_crx_event_sub_table[HAL_WLAN_CRX_EVENT_SUB_TYPE_BUTT];

/* DMACģ�飬TX_COMP�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_tx_comp_event_sub_table[HAL_TX_COMP_SUB_TYPE_BUTT];

/* DMACģ��, TBTT�¼��������������� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_tbtt_event_sub_table[HAL_EVENT_TBTT_SUB_TYPE_BUTT];

/* DMACģ��, MISC�¼��������������� */
DRAM_RAM_B frw_event_sub_table_item_stru g_dmac_misc_event_sub_table[HAL_EVENT_DMAC_MISC_SUB_TYPE_BUTT];

/* WLAN_DTX �¼������ͱ� */
DRAM_RAM_B frw_event_sub_table_item_stru g_ast_hmac_wlan_dtx_event_sub_table[DMAC_TX_WLAN_DTX_BUTT];

/* HMACģ�� WLAN_DRX�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_ast_hmac_wlan_drx_event_sub_table[DMAC_WLAN_DRX_EVENT_SUB_TYPE_BUTT];

/* HMACģ�� WLAN_CRX�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_BUTT];

/* HMACģ�� TBTT�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_ast_hmac_tbtt_event_sub_table[DMAC_TBTT_EVENT_SUB_TYPE_BUTT];

/* HMACģ�� ����HOST��������¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_SYN_BUTT];

/* HMACģ�� MISC��ɢ�¼���������ע��ṹ���� */
DRAM_RAM_B frw_event_sub_table_item_stru g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_BUTT];

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
DRAM_RAM_B frw_event_sub_table_item_stru g_ast_hmac_hcc_rx_event_sub_table[DMAC_HCC_RX_EVENT_SUB_TYPE_BUTT];
#endif

frw_event_sub_table_item_stru *g_past_dmac_host_crx_table = g_dmac_host_crx_table;
frw_event_sub_table_item_stru *g_past_dmac_tx_host_drx = g_dmac_tx_host_drx;
frw_event_sub_table_item_stru *g_past_dmac_tx_wlan_dtx = g_dmac_tx_wlan_dtx;
frw_event_sub_table_item_stru *g_past_dmac_wlan_ctx_event_sub_table = g_dmac_wlan_ctx_event_sub_table;
frw_event_sub_table_item_stru *g_past_dmac_wlan_drx_event_sub_table = g_dmac_wlan_drx_event_sub_table;
frw_event_sub_table_item_stru *g_past_dmac_high_prio_event_sub_table = g_dmac_high_prio_event_sub_table;
frw_event_sub_table_item_stru *g_past_dmac_wlan_crx_event_sub_table = g_dmac_wlan_crx_event_sub_table;
frw_event_sub_table_item_stru *g_past_dmac_tx_comp_event_sub_table = g_dmac_tx_comp_event_sub_table;
frw_event_sub_table_item_stru *g_past_dmac_tbtt_event_sub_table = g_dmac_tbtt_event_sub_table;
frw_event_sub_table_item_stru *g_past_dmac_misc_event_sub_table = g_dmac_misc_event_sub_table;
frw_event_sub_table_item_stru *g_past_hmac_wlan_dtx_event_sub_table = g_ast_hmac_wlan_dtx_event_sub_table;
frw_event_sub_table_item_stru *g_past_hmac_wlan_drx_event_sub_table = g_ast_hmac_wlan_drx_event_sub_table;
frw_event_sub_table_item_stru *g_past_hmac_wlan_crx_event_sub_table = g_ast_hmac_wlan_crx_event_sub_table;
frw_event_sub_table_item_stru *g_past_hmac_tbtt_event_sub_table = g_ast_hmac_tbtt_event_sub_table;
frw_event_sub_table_item_stru *g_past_hmac_wlan_ctx_event_sub_table = g_ast_hmac_wlan_ctx_event_sub_table;
frw_event_sub_table_item_stru *g_past_hmac_wlan_misc_event_sub_table = g_ast_hmac_wlan_misc_event_sub_table;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
frw_event_sub_table_item_stru *g_past_hmac_hcc_rx_event_sub_table = g_ast_hmac_hcc_rx_event_sub_table;
#endif

oal_uint16 g_us_dmac_high_prio_event_sub_table_size = OAL_SIZEOF(g_dmac_high_prio_event_sub_table);
oal_uint16 g_us_dmac_wlan_drx_event_sub_table_size = OAL_SIZEOF(g_dmac_wlan_drx_event_sub_table);
oal_uint16 g_us_dmac_wlan_crx_event_sub_table_size = OAL_SIZEOF(g_dmac_wlan_crx_event_sub_table);
oal_uint16 g_us_dmac_wlan_ctx_event_sub_table_size = OAL_SIZEOF(g_dmac_wlan_ctx_event_sub_table);
oal_uint16 g_us_dmac_tx_comp_event_sub_table_size = OAL_SIZEOF(g_dmac_tx_comp_event_sub_table);
oal_uint16 g_us_dmac_tbtt_event_sub_table_size = OAL_SIZEOF(g_dmac_tbtt_event_sub_table);
oal_uint16 g_us_dmac_misc_event_sub_table_size = OAL_SIZEOF(g_dmac_misc_event_sub_table);
oal_uint16 g_us_dmac_tx_host_drx_table_size = OAL_SIZEOF(g_dmac_tx_host_drx);
oal_uint16 g_us_dmac_host_crx_table_size = OAL_SIZEOF(g_dmac_host_crx_table);
oal_uint16 g_us_dmac_tx_wlan_dtx_table_size = OAL_SIZEOF(g_dmac_tx_wlan_dtx);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
oal_uint16 g_us_hmac_wlan_hcc_rx_event_sub_table_size = OAL_SIZEOF(g_ast_hmac_hcc_rx_event_sub_table);
#endif
oal_uint16 g_us_hmac_wlan_misc_event_sub_table_size = OAL_SIZEOF(g_ast_hmac_wlan_misc_event_sub_table);
oal_uint16 g_us_hmac_wlan_dtx_event_sub_table_size = OAL_SIZEOF(g_ast_hmac_wlan_dtx_event_sub_table);
oal_uint16 g_us_hmac_wlan_drx_event_sub_table_size = OAL_SIZEOF(g_ast_hmac_wlan_drx_event_sub_table);
oal_uint16 g_us_hmac_wlan_crx_event_sub_table_size = OAL_SIZEOF(g_ast_hmac_wlan_crx_event_sub_table);
oal_uint16 g_us_hmac_wlan_ctx_event_sub_table_size = OAL_SIZEOF(g_ast_hmac_wlan_ctx_event_sub_table);
oal_uint16 g_us_hmac_tbtt_event_sub_table_size = OAL_SIZEOF(g_ast_hmac_tbtt_event_sub_table);

oal_uint16 g_us_hal_event_dmac_misc_sub_type_butt = HAL_EVENT_DMAC_MISC_SUB_TYPE_BUTT;

/*****************************************************************************
DFX����ȫ�ֱ�������
*****************************************************************************/
#ifdef _PRE_WLAN_DFT_STAT
dfx_performance_log_switch_enum_uint8 g_auc_dfx_performance_log_switch[DFX_PERFORMANCE_LOG_BUTT] = { 0 };
#endif

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

#if defined(_PRE_PRODUCT_ID_HI110X_DEV)
oal_void event_fsm_table_register(oal_void)
{
    /* Part1: ������Dmac�յ��¼� */
    /* ע��DMACģ��HOST_CRX�¼� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_HOST_CRX, FRW_EVENT_PIPELINE_STAGE_1, g_past_dmac_host_crx_table);

    /* ע��DMACģ��HOST_DRX�¼����������� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_HOST_DRX, FRW_EVENT_PIPELINE_STAGE_1, g_past_dmac_tx_host_drx);

    /* ע��DMACģ��WLAN_DTX�¼����������� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_WLAN_DTX, FRW_EVENT_PIPELINE_STAGE_1, g_past_dmac_tx_wlan_dtx);

    /* ע��DMACģ��WLAN_CTX�¼����������� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_WLAN_CTX, FRW_EVENT_PIPELINE_STAGE_1,
                                 g_past_dmac_wlan_ctx_event_sub_table);

    /* ע��DMACģ��WLAN_DRX�¼��ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_WLAN_DRX, FRW_EVENT_PIPELINE_STAGE_0,
                                 g_past_dmac_wlan_drx_event_sub_table);

    /* ע��DMACģ��WLAN_CRX�¼�pipeline 0�ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_WLAN_CRX, FRW_EVENT_PIPELINE_STAGE_0,
                                 g_past_dmac_wlan_crx_event_sub_table);

    /* ע��DMACģ��TX_COMP�¼��ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_WLAN_TX_COMP, FRW_EVENT_PIPELINE_STAGE_0,
                                 g_past_dmac_tx_comp_event_sub_table);

    /* ע��DMACģ��TBTT�¼��ֱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_TBTT, FRW_EVENT_PIPELINE_STAGE_0, g_past_dmac_tbtt_event_sub_table);

    /* ע��DMACģ��ERR�¼��ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_HIGH_PRIO, FRW_EVENT_PIPELINE_STAGE_0,
                                 g_past_dmac_high_prio_event_sub_table);

    /* ע��DMACģ��MISC�¼��ֱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_DMAC_MISC, FRW_EVENT_PIPELINE_STAGE_0,
                                 g_past_dmac_misc_event_sub_table);

    /* Part2: ������Hmac�յ��¼� */
    /* ע��WLAN_DTX�¼��ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_WLAN_DTX, FRW_EVENT_PIPELINE_STAGE_0,
                                 g_past_hmac_wlan_dtx_event_sub_table);

    /* ע��WLAN_DRX�¼��ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_WLAN_DRX, FRW_EVENT_PIPELINE_STAGE_1,
                                 g_past_hmac_wlan_drx_event_sub_table);

    /* ע��HMACģ��WLAN_CRX�¼��ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_WLAN_CRX, FRW_EVENT_PIPELINE_STAGE_1,
                                 g_past_hmac_wlan_crx_event_sub_table);

    /* ע��DMACģ��MISC�¼��ֱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_DMAC_MISC, FRW_EVENT_PIPELINE_STAGE_1,
                                 g_past_hmac_wlan_misc_event_sub_table);

    /* ע��TBTT�¼��ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_TBTT, FRW_EVENT_PIPELINE_STAGE_1, g_past_hmac_tbtt_event_sub_table);

    /* ע��ͳ�ƽ����ѯ�¼��ӱ� */
    frw_event_table_register_etc(FRW_EVENT_TYPE_HOST_SDT_REG, FRW_EVENT_PIPELINE_STAGE_1,
                                 g_past_hmac_wlan_ctx_event_sub_table);
}
#else
oal_void event_fsm_table_register(oal_void)
{
    /* Part1: ������Dmac�յ��¼� */
    /* ע��DMACģ��HOST_CRX�¼� */
    frw_event_table_register(FRW_EVENT_TYPE_HOST_CRX, FRW_EVENT_PIPELINE_STAGE_1, g_past_dmac_host_crx_table);

    /* ע��DMACģ��HOST_DRX�¼����������� */
    frw_event_table_register(FRW_EVENT_TYPE_HOST_DRX, FRW_EVENT_PIPELINE_STAGE_1, g_past_dmac_tx_host_drx);

    /* ע��DMACģ��WLAN_DTX�¼����������� */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_DTX, FRW_EVENT_PIPELINE_STAGE_1, g_past_dmac_tx_wlan_dtx);

    /* ע��DMACģ��WLAN_CTX�¼����������� */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_CTX, FRW_EVENT_PIPELINE_STAGE_1, g_past_dmac_wlan_ctx_event_sub_table);

    /* ע��DMACģ��WLAN_DRX�¼��ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_DRX, FRW_EVENT_PIPELINE_STAGE_0, g_past_dmac_wlan_drx_event_sub_table);

    /* ע��DMACģ��WLAN_CRX�¼�pipeline 0�ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_CRX, FRW_EVENT_PIPELINE_STAGE_0, g_past_dmac_wlan_crx_event_sub_table);

    /* ע��DMACģ��TX_COMP�¼��ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_TX_COMP, FRW_EVENT_PIPELINE_STAGE_0,
                             g_past_dmac_tx_comp_event_sub_table);

    /* ע��DMACģ��TBTT�¼��ֱ� */
    frw_event_table_register(FRW_EVENT_TYPE_TBTT, FRW_EVENT_PIPELINE_STAGE_0, g_past_dmac_tbtt_event_sub_table);

    /* ע��DMACģ��ERR�¼��ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_HIGH_PRIO, FRW_EVENT_PIPELINE_STAGE_0,
                             g_past_dmac_high_prio_event_sub_table);

    /* ע��DMACģ��MISC�¼��ֱ� */
    frw_event_table_register(FRW_EVENT_TYPE_DMAC_MISC, FRW_EVENT_PIPELINE_STAGE_0, g_past_dmac_misc_event_sub_table);

    /* Part2: ������Hmac�յ��¼� */
    /* ע��WLAN_DTX�¼��ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_DTX, FRW_EVENT_PIPELINE_STAGE_0, g_past_hmac_wlan_dtx_event_sub_table);

    /* ע��WLAN_DRX�¼��ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_DRX, FRW_EVENT_PIPELINE_STAGE_1, g_past_hmac_wlan_drx_event_sub_table);

    /* ע��HMACģ��WLAN_CRX�¼��ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_WLAN_CRX, FRW_EVENT_PIPELINE_STAGE_1, g_past_hmac_wlan_crx_event_sub_table);

    /* ע��DMACģ��MISC�¼��ֱ� */
    frw_event_table_register(FRW_EVENT_TYPE_DMAC_MISC, FRW_EVENT_PIPELINE_STAGE_1,
                             g_past_hmac_wlan_misc_event_sub_table);

    /* ע��TBTT�¼��ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_TBTT, FRW_EVENT_PIPELINE_STAGE_1, g_past_hmac_tbtt_event_sub_table);

    /* ע��ͳ�ƽ����ѯ�¼��ӱ� */
    frw_event_table_register(FRW_EVENT_TYPE_HOST_SDT_REG, FRW_EVENT_PIPELINE_STAGE_1,
                             g_past_hmac_wlan_ctx_event_sub_table);
}
#endif

oal_void event_fsm_unregister(oal_void)
{
    /* Part1: ������Dmac����¼� */
    /* ȥע��DMACģ��HOST_CRX�¼� */
    memset_s(g_past_dmac_host_crx_table, g_us_dmac_host_crx_table_size, 0, g_us_dmac_host_crx_table_size);

    /* ȥע��DMACģ��HOST_DRX�¼����������� */
    memset_s(g_past_dmac_tx_host_drx, g_us_dmac_tx_host_drx_table_size, 0, g_us_dmac_tx_host_drx_table_size);

    /* ȥע��DMACģ��WLAN_DTX�¼����������� */
    memset_s(g_past_dmac_tx_wlan_dtx, g_us_dmac_tx_wlan_dtx_table_size, 0, g_us_dmac_tx_wlan_dtx_table_size);

    /* ȥע��DMACģ��WLAN_DRX�¼��ӱ� */
    memset_s(g_past_dmac_wlan_drx_event_sub_table, g_us_dmac_wlan_drx_event_sub_table_size,
        0, g_us_dmac_wlan_drx_event_sub_table_size);

    /* ȥע��DMACģ��WLAN_CRX�¼��ӱ� */
    memset_s(g_past_dmac_wlan_crx_event_sub_table, g_us_dmac_wlan_crx_event_sub_table_size,
        0, g_us_dmac_wlan_crx_event_sub_table_size);

    /* ȥע��DMACģ��TX_COMP�¼��ӱ� */
    memset_s(g_past_dmac_tx_comp_event_sub_table, g_us_dmac_tx_comp_event_sub_table_size,
        0, g_us_dmac_tx_comp_event_sub_table_size);

    /* ȥע��DMACģ��TBTT�¼��ֱ� */
    memset_s(g_past_dmac_tbtt_event_sub_table, g_us_dmac_tbtt_event_sub_table_size,
        0, g_us_dmac_tbtt_event_sub_table_size);

    /* ȥע��DMACģ��MAC_ERROR�¼��ӱ� */
    memset_s(g_past_dmac_high_prio_event_sub_table, g_us_dmac_high_prio_event_sub_table_size,
        0, g_us_dmac_high_prio_event_sub_table_size);

    /* ȥע��DMACģ��MAC_ERROR�¼��ӱ� */
    memset_s(g_past_dmac_misc_event_sub_table, g_us_dmac_misc_event_sub_table_size,
        0, g_us_dmac_misc_event_sub_table_size);

    /* Part2: ������Hmac����¼� */
    memset_s(g_past_hmac_wlan_dtx_event_sub_table, g_us_hmac_wlan_dtx_event_sub_table_size,
        0, g_us_hmac_wlan_dtx_event_sub_table_size);

    memset_s(g_past_hmac_wlan_drx_event_sub_table, g_us_hmac_wlan_drx_event_sub_table_size,
        0, g_us_hmac_wlan_drx_event_sub_table_size);

    memset_s(g_past_hmac_wlan_crx_event_sub_table, g_us_hmac_wlan_crx_event_sub_table_size,
        0, g_us_hmac_wlan_crx_event_sub_table_size);

    memset_s(g_past_hmac_tbtt_event_sub_table, g_us_hmac_tbtt_event_sub_table_size,
        0, g_us_hmac_tbtt_event_sub_table_size);

    memset_s(g_past_hmac_wlan_ctx_event_sub_table, g_us_hmac_wlan_ctx_event_sub_table_size,
        0, g_us_hmac_wlan_ctx_event_sub_table_size);

    memset_s(g_past_hmac_wlan_misc_event_sub_table, g_us_hmac_wlan_misc_event_sub_table_size,
        0, g_us_hmac_wlan_misc_event_sub_table_size);
}
/*lint -e19*/
oal_module_symbol(g_past_dmac_host_crx_table);
oal_module_symbol(g_past_dmac_tx_host_drx);
oal_module_symbol(g_past_dmac_tx_wlan_dtx);
oal_module_symbol(g_past_dmac_wlan_ctx_event_sub_table);
oal_module_symbol(g_past_dmac_wlan_drx_event_sub_table);
oal_module_symbol(g_past_dmac_high_prio_event_sub_table);
oal_module_symbol(g_past_dmac_wlan_crx_event_sub_table);
oal_module_symbol(g_past_dmac_tx_comp_event_sub_table);
oal_module_symbol(g_past_dmac_tbtt_event_sub_table);
oal_module_symbol(g_past_dmac_misc_event_sub_table);

oal_module_symbol(g_past_hmac_wlan_dtx_event_sub_table);
oal_module_symbol(g_past_hmac_wlan_drx_event_sub_table);
oal_module_symbol(g_past_hmac_wlan_crx_event_sub_table);
oal_module_symbol(g_past_hmac_tbtt_event_sub_table);
oal_module_symbol(g_past_hmac_wlan_ctx_event_sub_table);
oal_module_symbol(g_past_hmac_wlan_misc_event_sub_table);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
oal_module_symbol(g_past_hmac_hcc_rx_event_sub_table);
#endif

oal_module_symbol(event_fsm_table_register);

oal_module_symbol(event_fsm_unregister);
/*lint +e19*/
/*****************************************************************************
DFX���ú���ʵ��
*****************************************************************************/
#ifdef _PRE_WLAN_DFT_STAT

oal_uint32 dfx_get_performance_log_switch_enable(dfx_performance_log_switch_enum_uint8 uc_performance_log_switch_type)
{
    return g_auc_dfx_performance_log_switch[uc_performance_log_switch_type];
}


oal_void dfx_set_performance_log_switch_enable(
    dfx_performance_log_switch_enum_uint8 uc_performance_log_switch_type, oal_uint8 uc_value)
{
    if (uc_performance_log_switch_type >= DFX_PERFORMANCE_LOG_BUTT) {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "dfx_set_performance_log_switch_enable::log_switch type:%d.",
                         uc_performance_log_switch_type);
        return;
    }
    g_auc_dfx_performance_log_switch[uc_performance_log_switch_type] = uc_value;
}
#endif

