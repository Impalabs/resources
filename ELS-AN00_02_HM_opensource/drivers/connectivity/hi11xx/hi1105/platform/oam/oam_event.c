

/* ͷ�ļ����� */
#include "oam_event.h"
#include "oam_main.h"
#include "oam_ext_if.h"
#include "securec.h"
#ifndef _PRE_LINUX_TEST
#include "board.h"
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_OAM_EVENT_C

/*
 * �� �� ��  : oam_report_data_get_global_switch
 * ��������  : ��ȡ�շ�����֡���ܿ��أ�����ܿ��ز��������ý����ϱ��߼��ж�
 *             ĳһ���û����ϱ������Ƿ�򿪣���������mips��ֻҪ��һ���û��Ŀ�
 *             �ش����ܿ��ؾʹ�
 * �������  : en_direction:���򣬷��ͻ��ǽ���  0 ����  1����
 */
oal_switch_enum_uint8 oam_report_data_get_global_switch(oam_ota_frame_direction_type_enum_uint8 en_direction)
{
    if (en_direction >= oal_unlikely(OAM_OTA_FRAME_DIRECTION_TYPE_BUTT)) {
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }

    return g_oam_mng_ctx.st_user_track_ctx.aen_data_global_switch[en_direction];
}

/*
 * �� �� ��  : oam_report_data_set_global_switch
 * ��������  : �����ϱ�����֡��ȫ�ֿ��أ�ֻҪ��һ���û�Ҫ�ϱ������ܿ��ؾ�Ϊ����
 *             ����رգ���ֹ����mips
 */
OAL_STATIC uint32_t oam_report_data_set_global_switch(oam_ota_frame_direction_type_enum_uint8 direction)
{
    uint16_t usr_idx;
    oal_switch_enum_uint8 mcast_switch = OAL_SWITCH_OFF;
    oal_switch_enum_uint8 ucast_switch = OAL_SWITCH_OFF;

    if (direction >= oal_unlikely(OAM_OTA_FRAME_DIRECTION_TYPE_BUTT)) {
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }

    /* �ȼ�鵥������֡�Ƿ����û��Ѿ����� */
    for (usr_idx = 0; usr_idx < WLAN_USER_MAX_USER_LIMIT; usr_idx++) {
        if (g_oam_mng_ctx.st_user_track_ctx.ast_80211_ucast_data_ctx[usr_idx][direction].value != 0) {
            ucast_switch = OAL_SWITCH_ON;
            break;
        }
    }

    /* �ټ���鲥����֡�Ƿ񿪹ش��� */
    if (g_oam_mng_ctx.st_user_track_ctx.ast_80211_mcast_data_ctx[direction].value != 0) {
        mcast_switch = OAL_SWITCH_ON;
    }

    g_oam_mng_ctx.st_user_track_ctx.aen_data_global_switch[direction] = (ucast_switch | mcast_switch);

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_eth_frame_set_switch
 * ��������  : �����ϱ���̫��֡�Ŀ���
 * �������  : us_user_idx:�û���Դ��id
 *             en_switch  :�򿪻��ǹر�
 *             en_eth_direction:���ͻ��ǽ��շ���
 */
uint32_t oam_report_eth_frame_set_switch(uint16_t us_user_idx,
                                         oal_switch_enum_uint8 en_switch,
                                         oam_ota_frame_direction_type_enum_uint8 en_eth_direction)
{
    uint8_t ota_switch_id;

    if (us_user_idx >= WLAN_USER_MAX_USER_LIMIT) {
        oal_io_print("oam_report_eth_frame_set_switch::user_idx exceeds!\n");
        return OAL_ERR_CODE_OAM_EVT_USER_IDX_EXCEED;
    }

    if (en_eth_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        oal_io_print("oam_report_eth_frame_set_switch::eth_direction exceeds!\n");
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    g_oam_mng_ctx.st_user_track_ctx.aen_eth_data_ctx[us_user_idx][en_eth_direction] = en_switch;

    ota_switch_id = en_eth_direction == OAM_OTA_FRAME_DIRECTION_TYPE_TX ?
                                        OAM_OTA_SWITCH_TX_ETH_FRAME : OAM_OTA_SWITCH_RX_ETH_FRAME;
    oam_ota_set_switch(ota_switch_id, en_switch);

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_eth_frame_get_switch
 * ��������  : ��ȡ�ϱ���̫��֡�Ŀ���
 * �������  : us_user_idx:�û���Դ��id
 *             en_eth_direction:���ͻ��ǽ���
 *             pen_eth_switch:��̫��֡��ӡ����
 */
uint32_t oam_report_eth_frame_get_switch(uint16_t us_user_idx,
                                         oam_ota_frame_direction_type_enum_uint8 en_eth_direction,
                                         oal_switch_enum_uint8 *pen_eth_switch)
{
    if (us_user_idx >= WLAN_USER_MAX_USER_LIMIT) {
        return OAL_ERR_CODE_OAM_EVT_USER_IDX_EXCEED;
    }

    if (en_eth_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    if (pen_eth_switch == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    *pen_eth_switch = g_oam_mng_ctx.st_user_track_ctx.aen_eth_data_ctx[us_user_idx][en_eth_direction];

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_80211_mcast_set_switch
 * ��������  : �����ϱ�80211�鲥\�㲥���ݻ��߹���֡�Ŀ���
 * �������  : en_mcast_direction:֡�Ƿ��ͻ��ǽ���
 *             en_frame_type     :����֡���ǹ���֡(��������֡)
 *             en_frame_switch   :֡����
 *             en_cb_switch      :CB����
 *             en_dscr_switch    :����������
 */
uint32_t oam_report_80211_mcast_set_switch(oam_ota_frame_direction_type_enum_uint8 en_mcast_direction,
                                           oam_user_track_frame_type_enum_uint8 en_frame_type,
                                           oam_80211_frame_ctx_union *oam_frame_report)
{
    if (en_mcast_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    if (en_frame_type >= OAM_USER_TRACK_FRAME_TYPE_BUTT) {
        return OAL_ERR_CODE_CONFIG_UNSUPPORT;
    }

    /* Ϊ�鲥����֡���ÿ��� */
    if (en_frame_type == OAM_USER_TRACK_FRAME_TYPE_MGMT) {
        g_oam_mng_ctx.st_user_track_ctx.ast_80211_mcast_mgmt_ctx[en_mcast_direction].value = oam_frame_report->value;
    } else {
        g_oam_mng_ctx.st_user_track_ctx.ast_80211_mcast_data_ctx[en_mcast_direction].value = oam_frame_report->value;

        /* �����ܿ��� */
        oam_report_data_set_global_switch(en_mcast_direction);
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_80211_mcast_get_switch
 * ��������  : ��ȡ�ϱ�80211�鲥֡�Ŀ���
 * �������  : en_mcast_direction:֡�Ƿ��ͻ��ǽ���
 *             en_frame_type     :����֡���ǹ���֡(��������֡)
 *             pen_frame_switch  :֡����
 *             pen_cb_switch     :CB����
 *             pen_dscr_switch   :����������
 */
uint32_t oam_report_80211_mcast_get_switch(oam_ota_frame_direction_type_enum_uint8 en_mcast_direction,
                                           oam_user_track_frame_type_enum_uint8 en_frame_type,
                                           oam_80211_frame_ctx_union *oam_frame_report)
{
    if (en_mcast_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    if (en_frame_type >= OAM_USER_TRACK_FRAME_TYPE_BUTT) {
        return OAL_ERR_CODE_CONFIG_UNSUPPORT;
    }

    if (oal_unlikely(oal_any_null_ptr1(oam_frame_report))) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (en_frame_type == OAM_USER_TRACK_FRAME_TYPE_MGMT) {
        oam_frame_report->value = g_oam_mng_ctx.st_user_track_ctx.ast_80211_mcast_mgmt_ctx[en_mcast_direction].value;
    } else {
        oam_frame_report->value = g_oam_mng_ctx.st_user_track_ctx.ast_80211_mcast_data_ctx[en_mcast_direction].value;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_80211_ucast_set_switch
 * ��������  : ����80211����֡�ϱ��Ŀ���
 * �������  : en_ucast_direction:֡�Ƿ��ͻ��ǽ���
 *             en_frame_type     :����֡���ǹ���֡(��������֡)
 *             en_frame_switch   :֡����
 *             en_cb_switch      :CB����
 *             en_dscr_switch    :����������
 *             us_user_idx       :�û���Դ��id
 */
uint32_t oam_report_80211_ucast_set_switch(oam_ota_frame_direction_type_enum_uint8 en_ucast_direction,
                                           oam_user_track_frame_type_enum_uint8 en_frame_type,
                                           oam_80211_frame_ctx_union *oam_switch_param,
                                           uint16_t us_user_idx)
{
    if (en_ucast_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    if (en_frame_type >= OAM_USER_TRACK_FRAME_TYPE_BUTT) {
        return OAL_ERR_CODE_CONFIG_UNSUPPORT;
    }

    if (us_user_idx >= WLAN_USER_MAX_USER_LIMIT) {
        return OAL_ERR_CODE_OAM_EVT_USER_IDX_EXCEED;
    }

    if (en_frame_type == OAM_USER_TRACK_FRAME_TYPE_MGMT) {
        g_oam_mng_ctx.st_user_track_ctx.ast_80211_ucast_mgmt_ctx[us_user_idx][en_ucast_direction].value =
            oam_switch_param->value;
    } else {
        g_oam_mng_ctx.st_user_track_ctx.ast_80211_ucast_data_ctx[us_user_idx][en_ucast_direction].value =
            oam_switch_param->value;

        /* �����ܿ��� */
        oam_report_data_set_global_switch(en_ucast_direction);
    }
    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_80211_ucast_get_switch
 * ��������  : ��ȡ80211����֡���ϱ�����
 * �������  : en_ucast_direction:֡�Ƿ��ͻ��ǽ���
 *             en_frame_type     :����֡���ǹ���֡(��������֡)
 *             pen_frame_switch  :֡����
 *             pen_cb_switch     :CB����
 *             pen_dscr_switch   :����������
 *             us_user_idx       :�û���Դ��id
 */
uint32_t oam_report_80211_ucast_get_switch(oam_ota_frame_direction_type_enum_uint8 en_ucast_direction,
                                           oam_user_track_frame_type_enum_uint8 en_frame_type,
                                           oam_80211_frame_ctx_union *oam_frame_report_un,
                                           uint16_t us_user_idx)
{
    if (en_ucast_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    if (en_frame_type >= OAM_USER_TRACK_FRAME_TYPE_BUTT) {
        return OAL_ERR_CODE_CONFIG_UNSUPPORT;
    }

    if (us_user_idx >= WLAN_USER_MAX_USER_LIMIT) {
        return OAL_ERR_CODE_OAM_EVT_USER_IDX_EXCEED;
    }

    if (oal_unlikely(oal_any_null_ptr1(oam_frame_report_un))) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (en_frame_type == OAM_USER_TRACK_FRAME_TYPE_MGMT) {
        oam_frame_report_un->value =
            g_oam_mng_ctx.st_user_track_ctx.ast_80211_ucast_mgmt_ctx[us_user_idx][en_ucast_direction].value;
    } else {
        oam_frame_report_un->value =
            g_oam_mng_ctx.st_user_track_ctx.ast_80211_ucast_data_ctx[us_user_idx][en_ucast_direction].value;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_80211_probe_set_switch
 * ��������  : ����probe request �� probe response�Ĵ�ӡ����
 * �������  : en_ucast_direction:֡�Ƿ��ͻ��ǽ���
 *             en_frame_switch   :֡����
 *             en_cb_switch      :CB����
 *             en_dscr_switch    :����������
 */
uint32_t oam_report_80211_probe_set_switch(oam_ota_frame_direction_type_enum_uint8 en_probe_direction,
                                           oam_80211_frame_ctx_union *oam_switch_param)
{
    if (en_probe_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    if (oam_switch_param == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    g_oam_mng_ctx.st_user_track_ctx.aen_80211_probe_switch[en_probe_direction].value = oam_switch_param->value;

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_80211_probereq_get_switch
 * ��������  : ��ȡprebe request��probe response�Ĵ�ӡ����
 * �������  : en_ucast_direction:֡�Ƿ��ͻ��ǽ���
 *             pen_frame_switch  :֡����
 *             pen_cb_switch     :CB����
 *             pen_dscr_switch   :����������
 */
uint32_t oam_report_80211_probe_get_switch(oam_ota_frame_direction_type_enum_uint8 en_probe_direction,
                                           oam_80211_frame_ctx_union *oam_frame_report_un)
{
    if (en_probe_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    if (oal_unlikely(oal_any_null_ptr1(oam_frame_report_un))) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    oam_frame_report_un->value = g_oam_mng_ctx.st_user_track_ctx.aen_80211_probe_switch[en_probe_direction].value;

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_dhcp_arp_set_switch
 * ��������  : ���ù㲥��dhcp��arp���ϱ�����
 */
uint32_t oam_report_dhcp_arp_set_switch(oal_switch_enum_uint8 en_switch)
{
    g_oam_mng_ctx.st_user_track_ctx.en_tx_mcast_dhcp_arp_switch = en_switch;

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_dhcp_arp_get_switch
 * ��������  : ��ȡ�ϱ��㲥dhcp��arp�Ŀ���
 */
oal_switch_enum_uint8 oam_report_dhcp_arp_get_switch(void)
{
    return g_oam_mng_ctx.st_user_track_ctx.en_tx_mcast_dhcp_arp_switch;
}

static void oam_ota_set_global_switch(uint8_t ota_switch_id, uint8_t ota_switch)
{
    if (ota_switch == OAL_SWITCH_ON) {
        g_oam_mng_ctx.ast_ota_ctx.golble_switch |= (uint32_t)((uint32_t)ota_switch << ota_switch_id);
    } else {
        /* static�������ϲ㺯���Ѿ���֤��ota_switchֻ��ON��OFF������� */
        g_oam_mng_ctx.ast_ota_ctx.golble_switch &= (uint32_t)((uint32_t)ota_switch << ota_switch_id);
    }
}

void oam_ota_set_switch(uint8_t ota_switch_id, uint8_t ota_switch)
{
    if ((ota_switch_id >= OAM_OTA_SWITCH_BUTT) || (ota_switch >= OAL_SWITCH_BUTT)) {
        return;
    }

    g_oam_mng_ctx.ast_ota_ctx.ota_switch[ota_switch_id] = ota_switch;

    /* ����ota�ϱ�ȫ�ֿ���mib���ϱ�ʱ�����ж�ȫ�ֿ���mib��Ϊ0����ֱ�ӷ��أ�����mips */
    oam_ota_set_global_switch(ota_switch_id, ota_switch);
}

/* ota�ϱ�ȫ�ֿ���mib���ϱ�ʱ�����жϸ�ֵ�����Ϊ0����ֱ�ӷ��أ���������mips */
uint32_t oam_ota_get_global_switch(void)
{
    return g_oam_mng_ctx.ast_ota_ctx.golble_switch;
}

oal_switch_enum_uint8 oam_ota_get_switch(uint8_t ota_switch_id)
{
    if (ota_switch_id >= OAM_OTA_SWITCH_BUTT) {
        return OAL_SWITCH_OFF;
    }

    return g_oam_mng_ctx.ast_ota_ctx.ota_switch[ota_switch_id];
}

OAL_STATIC void oam_set_ota_board_type(oam_ota_stru *ota_data)
{
    int32_t chip_type = get_hi110x_subchip_type();
    switch (chip_type) {
        case BOARD_VERSION_HI1103:
            ota_data->st_ota_hdr.auc_resv[0] = OAM_OTA_TYPE_1103_HOST;
            break;
        case BOARD_VERSION_HI1105:
            ota_data->st_ota_hdr.auc_resv[0] = OAM_OTA_TYPE_1105_HOST;
            break;
        case BOARD_VERSION_HI1106:
            ota_data->st_ota_hdr.auc_resv[0] = OAM_OTA_TYPE_1106_HOST;
            break;
        default:
            ota_data->st_ota_hdr.auc_resv[0] = OAM_OTA_TYPE_CHIP_INVALID;
            break;
    }
}

uint32_t oam_event_init(void)
{
    /* ���õ��û�������ص����п��� */
    oam_report_set_all_switch(OAL_SWITCH_OFF);

    oam_report_dhcp_arp_set_switch(OAL_SWITCH_OFF);

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_ota_report_to_std
 * ��������  : ��OTA���ݴ�ӡ����׼����:
 *             (1)��������������ж���Ϣ���ڴ����Ϣ����ʱ����Ϣ,�����¼�������Ϣ,
 *                ����Ļ��ÿ�д�ӡ4�ֽڣ�
 *             (2)�����֡�����ڴ����Ϣ����ÿ�д�ӡ20�ֽ�
 *             (3)�����֡�Ļ�������֡��̫������֡��ֻ��ӡǰ300���ֽ�
 * �������   : puc_param_one_addr:����������������ʾ�������ĵ�ַ;�����֡����ʾ֡ͷ��ַ
 *              ul_param_one_len  :����������������ʾ����������;�����֡����ʾ֡ͷ����
 *              puc_param_two_addr:���������������Ϊ0;�����֡����ʾ֡���ַ
 *              ul_param_two_len  :���������������Ϊ0;�����֡����ʾ֡�峤��
 *              en_ota_type       :OTA����
 */
OAL_STATIC uint32_t oam_ota_report_to_std(uint8_t *puc_param_one_addr,
                                          uint16_t us_param_one_len,
                                          uint8_t *puc_param_two_addr,
                                          uint16_t us_param_two_len,
                                          oam_ota_type_enum_uint8 en_ota_type)
{
    if ((us_param_two_len == 0) || (puc_param_two_addr == NULL)) {
        oal_io_print("\n\nOTA TYPE is--> %d and OTA DATA is:\n", en_ota_type);
        oam_dump_buff_by_hex(puc_param_one_addr, us_param_one_len, 4); /* 4��ʾÿ��ӡ4�����������һ�λ��� */
    } else {
        /* ��ӡ��һ������ */
        oal_io_print("\n\nOTA TYPE is--> %d and OTA DATA the first part is:\n", en_ota_type);
        oam_dump_buff_by_hex(puc_param_one_addr, us_param_one_len, OAM_PRINT_CRLF_NUM);

        /* ��ӡ�ڶ������� */
        oal_io_print("\nOTA DATA tht second part is:\n");

        if (en_ota_type == OAM_OTA_TYPE_80211_FRAME) {
            us_param_two_len = oal_min(us_param_two_len, OAM_OTA_DATA_TO_STD_MAX_LEN);
        }

        oam_dump_buff_by_hex(puc_param_two_addr, us_param_two_len, OAM_PRINT_CRLF_NUM);
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_ota_report_to_sdt
 * ��������  : ���տ���Ϣ�ϱ�SDT���ߣ�����������������������������֡
 * �������  : puc_param_one_addr:����������������ʾ�������ĵ�ַ;�����֡����ʾ֡ͷ��ַ
 *             ul_param_one_len  :����������������ʾ����������;�����֡����ʾ֡ͷ����
 *             puc_param_two_addr:���������������Ϊ0;�����֡����ʾ֡���ַ
 *             ul_param_two_len  :���������������Ϊ0;�����֡����ʾ֡�峤��
 *             en_ota_type       :OTA����
 */
/*lint -e662*/
uint32_t oam_ota_report_to_sdt(uint8_t *puc_param_one_addr,
                               uint16_t us_param_one_len,
                               uint8_t *puc_param_two_addr,
                               uint16_t us_param_two_len,
                               oam_ota_type_enum_uint8 en_ota_type)
{
    uint32_t ul_ret = OAL_SUCC;
#if ((_PRE_OS_VERSION_RAW != _PRE_OS_VERSION) && (_PRE_OS_VERSION_WIN32_RAW != _PRE_OS_VERSION))
    uint32_t ul_tick;
    uint16_t us_skb_len;
    oal_netbuf_stru *pst_netbuf;
    oam_ota_stru *pst_ota_data;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* Ϊ�ϱ�OTA��������ռ䣬�������������us_param_two_lenΪ0 */
    us_skb_len = us_param_one_len + us_param_two_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        if ((us_param_one_len + sizeof(oam_ota_hdr_stru)) < us_skb_len) {
            us_param_two_len = us_skb_len - us_param_one_len - (uint16_t)sizeof(oam_ota_hdr_stru);
        } else {
            us_param_one_len = us_skb_len - sizeof(oam_ota_hdr_stru);
            us_param_two_len = 0;
        }
    }

    pst_netbuf = oam_alloc_data2sdt(us_skb_len);
    if (pst_netbuf == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = en_ota_type;
    oam_set_ota_board_type(pst_ota_data);

    switch (en_ota_type) {
        /* ��װ���ջ��߷�����������ӦOTA�ṹ���������Ա */
        case OAM_OTA_TYPE_RX_DSCR:
        case OAM_OTA_TYPE_TX_DSCR:
        case OAM_OTA_TYPE_IRQ:
        case OAM_OTA_TYPE_EVENT_QUEUE:
        case OAM_OTA_TYPE_TIMER:
        case OAM_OTA_TYPE_MEMPOOL:
        case OAM_OTA_TYPE_HMAC_VAP:
        case OAM_OTA_TYPE_DMAC_VAP:
        case OAM_OTA_TYPE_HMAC_USER:
        case OAM_OTA_TYPE_DMAC_USER:
        case OAM_OTA_TYPE_HMAC_VAP_MEMBER_SIZE:
        case OAM_OTA_TYPE_DMAC_VAP_MEMBER_SIZE:
        case OAM_OTA_TYPE_RX_DSCR_PILOT:
        case OAM_OTA_TYPE_TX_DSCR_PILOT:
            pst_ota_data->st_ota_hdr.us_ota_data_len = us_param_one_len;
            ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                              (uint32_t)(us_param_one_len + us_param_two_len),
                              (const void *)puc_param_one_addr,
                              (uint32_t)us_param_one_len);
            if (ul_ret != EOK) {
                oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
                oal_io_print("oam_ota_report_to_sdt::memcpy_s failed.\n");
                return OAL_FAIL;
            }
            break;

        /* ��װ֡��ӦOTA�ṹ���������Ա */
        case OAM_OTA_TYPE_80211_FRAME:
        case OAM_OTA_TYPE_MEMBLOCK:
            pst_ota_data->st_ota_hdr.uc_frame_hdr_len = (uint8_t)us_param_one_len;
            pst_ota_data->st_ota_hdr.us_ota_data_len = us_param_one_len + us_param_two_len;

            /* ����֡ͷ */
            ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                              (uint32_t)(us_param_one_len + us_param_two_len),
                              (const void *)puc_param_one_addr,
                              (uint32_t)us_param_one_len);
            if (ul_ret != EOK) {
                oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
                oal_io_print("oam_ota_report_to_sdt::memcpy_s failed.\n");
                return OAL_FAIL;
            }

            /* ����֡�� */
            if (puc_param_two_addr != NULL) {
                ul_ret = memcpy_s((void *)(pst_ota_data->auc_ota_data + us_param_one_len),
                                  (uint32_t)(us_param_two_len),
                                  (const void *)puc_param_two_addr,
                                  (uint32_t)us_param_two_len);
                if (ul_ret != EOK) {
                    oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
                    oal_io_print("oam_ota_report_to_sdt::memcpy_s failed.\n");
                    return OAL_FAIL;
                }
            }
            break;

        /* ��Чֵ */
        default:
            oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
            return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /* �ж�sdt������Ϣ�����Ƿ�������������������� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);
#endif
    return ul_ret;
}

/*
 * �� �� ��  : oam_ota_report
 * ��������  : �ϱ�OTA(over the air)�¼�,����������������������������֡���֡�
 * �������  : puc_param_one_addr:����������������ʾ�������ĵ�ַ;�����֡����ʾ֡ͷ��ַ
 *             ul_param_one_len  :����������������ʾ����������;�����֡����ʾ֡ͷ����
 *             puc_param_two_addr:���������������Ϊ0;�����֡����ʾ֡���ַ
 *             ul_param_two_len  :���������������Ϊ0;�����֡����ʾ֡�峤��
 *             en_ota_type       :OTA����
 */
uint32_t oam_ota_report(uint8_t *puc_param_one_addr,
                        uint16_t us_param_one_len,
                        uint8_t *puc_param_two_addr,
                        uint16_t us_param_two_len,
                        oam_ota_type_enum_uint8 en_ota_type)
{
    uint32_t ul_rslt = OAL_ERR_CODE_BUTT;

    switch (g_oam_mng_ctx.en_output_type) {
        /* ���������̨ */
        case OAM_OUTPUT_TYPE_CONSOLE:
            ul_rslt = oam_ota_report_to_std(puc_param_one_addr,
                                            us_param_one_len,
                                            puc_param_two_addr,
                                            us_param_two_len,
                                            en_ota_type);

            break;

        /* �����SDT���� */
        case OAM_OUTPUT_TYPE_SDT:
            ul_rslt = oam_ota_report_to_sdt(puc_param_one_addr,
                                            us_param_one_len,
                                            puc_param_two_addr,
                                            us_param_two_len,
                                            en_ota_type);

            break;

        default:
            ul_rslt = OAL_ERR_CODE_INVALID_CONFIG;

            break;
    }

    if (ul_rslt != OAL_SUCC) {
        return ul_rslt;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_80211_frame_to_console
 * ��������  : ��80211֡���ݴ�ӡ������̨��һ���Ǵ��ڣ�
 * �������  : puc_mac_hdr_addr  :mac֡ͷ��ַ
 *             us_mac_hdr_len    :mac֡ͷ����
 *             puc_mac_body_addr :mac֡���ַ
 *             us_mac_frame_len  :mac֡�ܳ���(֡ͷ+֡��)
 *             en_frame_direction:mac����(tx���̻���rx����)
 */
OAL_STATIC uint32_t oam_report_80211_frame_to_console(uint8_t *puc_mac_hdr_addr,
                                                      uint8_t uc_mac_hdr_len,
                                                      uint8_t *puc_mac_body_addr,
                                                      uint16_t us_mac_frame_len,
                                                      oam_ota_frame_direction_type_enum_uint8 en_frame_direction)
{
    uint16_t us_80211_frame_body_len;

    if (en_frame_direction == OAM_OTA_FRAME_DIRECTION_TYPE_TX) {
        oal_io_print("oam_report_80211_frame_to_console::tx_80211_frame header:\n");
    } else {
        oal_io_print("oam_report_80211_frame_to_console::rx_80211_frame header:\n");
    }

    oam_dump_buff_by_hex(puc_mac_hdr_addr, uc_mac_hdr_len, OAM_PRINT_CRLF_NUM);

    if (uc_mac_hdr_len > us_mac_frame_len) {
        oal_io_print("oam_report_80211_frame_to_console::rx_80211_frame invalid frame\n");
        return OAL_FAIL;
    }

    us_80211_frame_body_len = us_mac_frame_len - uc_mac_hdr_len;

    oal_io_print("oam_report_80211_frame_to_console::80211_frame body:\n");
    oam_dump_buff_by_hex(puc_mac_body_addr, us_80211_frame_body_len, OAM_PRINT_CRLF_NUM);

    return OAL_SUCC;
}
/*
 * �� �� ��  : oam_hide_mac_addr
 * ��������  : ���ڰ�ȫ�����ڸ���־��OTA��֡�ڵ�mac��ַ
 */
OAL_STATIC void oam_hide_mac_addr(uint8_t *puc_mac_hdr, uint8_t uc_beacon_hdr_len)
{
    if (puc_mac_hdr == NULL || uc_beacon_hdr_len < WLAN_MGMT_FRAME_HEADER_LEN) {
        return;
    }
    /* addr1 */
    puc_mac_hdr[5] = 0xff; /* ��1����ַ�Ĵ��λ��������ĵ�5�� */
    puc_mac_hdr[6] = 0xff; /* ��1����ַ�Ĵ��λ��������ĵ�6�� */
    puc_mac_hdr[7] = 0xff; /* ��1����ַ�Ĵ��λ��������ĵ�7�� */

    /* addr2 */
    puc_mac_hdr[11] = 0xff; /* ��2����ַ�Ĵ��λ��������ĵ�11�� */
    puc_mac_hdr[12] = 0xff; /* ��2����ַ�Ĵ��λ��������ĵ�12�� */
    puc_mac_hdr[13] = 0xff; /* ��2����ַ�Ĵ��λ��������ĵ�13�� */

    /* addr3 */
    puc_mac_hdr[17] = 0xff; /* ��3����ַ�Ĵ��λ��������ĵ�17�� */
    puc_mac_hdr[18] = 0xff; /* ��3����ַ�Ĵ��λ��������ĵ�18�� */
    puc_mac_hdr[19] = 0xff; /* ��3����ַ�Ĵ��λ��������ĵ�19�� */
}

OAL_STATIC uint32_t oam_report_80211_frame_to_sdt(uint8_t *puc_user_macaddr,
                                                  uint8_t *puc_mac_hdr_addr,
                                                  uint8_t uc_mac_hdr_len,
                                                  uint8_t *puc_mac_body_addr,
                                                  uint16_t us_mac_frame_len,
                                                  oam_ota_frame_direction_type_enum_uint8 en_frame_direction)
{
    uint32_t ul_tick;
    uint16_t us_skb_len;
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint32_t ul_ret;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* Ϊ�ϱ�80211֡����ռ� */
    us_skb_len = us_mac_frame_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_mac_frame_len = WLAN_SDT_NETBUF_MAX_PAYLOAD - sizeof(oam_ota_hdr_stru);
    }

    pst_netbuf = oam_alloc_data2sdt(us_skb_len);
    if (pst_netbuf == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_ota_data = (oam_ota_stru *)oal_netbuf_header(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣͷ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = OAM_OTA_TYPE_80211_FRAME;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = uc_mac_hdr_len;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_mac_frame_len;
    pst_ota_data->st_ota_hdr.en_frame_direction = en_frame_direction;
    oal_set_mac_addr(pst_ota_data->st_ota_hdr.auc_user_macaddr, puc_user_macaddr);
    oam_set_ota_board_type(pst_ota_data);

    /* ����֡ͷ */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)us_mac_frame_len,
                      (const void *)puc_mac_hdr_addr,
                      (uint32_t)uc_mac_hdr_len);
    /* ����֡�� */
    ul_ret += memcpy_s((void *)(pst_ota_data->auc_ota_data + uc_mac_hdr_len),
                       (uint32_t)(us_mac_frame_len - uc_mac_hdr_len),
                       (const void *)puc_mac_body_addr,
                       (uint32_t)(us_mac_frame_len - uc_mac_hdr_len));
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_report_80211_frame_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }
    oam_hide_mac_addr(pst_ota_data->auc_ota_data, uc_mac_hdr_len);

    /* �ж�sdt������Ϣ�����Ƿ�������������������� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_80211_frame
 * ��������  : �ϱ�802.11֡
 * �������  : puc_user_macaddr  :Ŀ���û�mac��ַ
 *             puc_mac_hdr_addr  :mac֡ͷ��ַ
 *             us_mac_hdr_len    :mac֡ͷ����
 *             puc_mac_body_addr :mac֡���ַ
 *             us_mac_frame_len  :mac֡�ܳ���(֡ͷ+֡��)
 *             en_frame_direction:mac����(tx���̻���rx����)
 */
uint32_t oam_report_80211_frame(uint8_t *puc_user_macaddr,
                                uint8_t *puc_mac_hdr_addr,
                                uint8_t uc_mac_hdr_len,
                                uint8_t *puc_mac_body_addr,
                                uint16_t us_mac_frame_len,
                                oam_ota_frame_direction_type_enum_uint8 en_frame_direction)
{
    uint32_t ul_ret = OAL_SUCC;
    uint32_t ul_oam_ret = OAL_SUCC;
    uint32_t ul_return_addr = 0;

    /* Less IO_print, less oam_report_80211_frame_to_console, use oam_report_80211_frame_to_sdt for log.
       Suggested by gongxiangling & wangzhenzhong */
#if (_PRE_OS_VERSION_RAW == _PRE_OS_VERSION)
    ul_return_addr = __return_address();
#endif

    if (oal_unlikely(oal_any_null_ptr3(puc_mac_hdr_addr, puc_mac_body_addr, puc_user_macaddr))) {
        oam_error_log4(0, OAM_SF_ANY,
                       "{oam_report_80211_frame:[device] puc_mac_hdr_addr = 0x%X, puc_mac_body_addr = 0x%X, puc_user_macaddr = 0x%X, __return_address = 0x%X}",
                       (uintptr_t)puc_mac_hdr_addr, (uintptr_t)puc_mac_body_addr,
                       (uintptr_t)puc_user_macaddr, ul_return_addr);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���֡ͷ���ȺϷ��� */
    if ((uc_mac_hdr_len > WLAN_MAX_FRAME_HEADER_LEN) || (uc_mac_hdr_len < WLAN_MIN_FRAME_HEADER_LEN)) {
        oam_warning_log4(0, OAM_SF_ANY,
                         "{oam_report_80211_frame:HEAD CHECK! HDR_LEN_INVALID!!hearder_len = %d, frame_len = %d, en_frame_direction = %d, return_addres = 0x%X}",
                         uc_mac_hdr_len, us_mac_frame_len, en_frame_direction, ul_return_addr);
    }

    /* ���mac֡�ܳ��ȺϷ��� */
    if (uc_mac_hdr_len > us_mac_frame_len) {
        oam_report_dft_params(BROADCAST_MACADDR, puc_mac_hdr_addr,
                              uc_mac_hdr_len, OAM_OTA_TYPE_80211_FRAME);
        oam_warning_log4(0, OAM_SF_ANY,
                         "{oam_report_80211_frame:HEAD/FRAME CHECK! hearder_len = %d, frame_len = %d, en_frame_direction = %d, return_addres = 0x%X}",
                         uc_mac_hdr_len, us_mac_frame_len, en_frame_direction, ul_return_addr);
        return OAL_ERR_CODE_OAM_EVT_FR_LEN_INVALID;
    }

    us_mac_frame_len = (us_mac_frame_len > WLAN_MAX_FRAME_LEN) ? WLAN_MAX_FRAME_LEN : us_mac_frame_len;

    if (oal_unlikely(en_frame_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT)) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    switch (g_oam_mng_ctx.en_output_type) {
        /* ���������̨ */
        case OAM_OUTPUT_TYPE_CONSOLE:
            ul_ret = oam_report_80211_frame_to_console(puc_mac_hdr_addr,
                                                       uc_mac_hdr_len,
                                                       puc_mac_body_addr,
                                                       us_mac_frame_len,
                                                       en_frame_direction);
            break;

        /* �����SDT���� */
        case OAM_OUTPUT_TYPE_SDT:
            ul_oam_ret = oam_report_80211_frame_to_sdt(puc_user_macaddr,
                                                       puc_mac_hdr_addr,
                                                       uc_mac_hdr_len,
                                                       puc_mac_body_addr,
                                                       us_mac_frame_len,
                                                       en_frame_direction);
            break;

        default:
            ul_oam_ret = OAL_ERR_CODE_INVALID_CONFIG;
            break;
    }

    if ((ul_oam_ret != OAL_SUCC) || (ul_ret != OAL_SUCC)) {
        oam_warning_log4(0, OAM_SF_ANY,
                         "{oam_report_80211_frame:[device] en_frame_direction = %d, ul_ret = %d, ul_oam_ret = %d, return_addres = 0x%X}",
                         en_frame_direction, ul_ret, ul_oam_ret, ul_return_addr);
    }

    return ((ul_ret != OAL_SUCC) ? (ul_ret) : (ul_oam_ret));
}

/*
 * �� �� ��  : oam_report_dscr_to_console
 * ��������  : ����������Ϣ��ӡ������̨��һ����Ǵ��ڣ�
 * �������  : puc_dscr_addr :��������ַ
 *             us_dscr_len   :����������
 *             en_ota_type   :ota����
 */
OAL_STATIC uint32_t oam_report_dscr_to_console(uint8_t *puc_dscr_addr,
                                               uint16_t us_dscr_len,
                                               oam_ota_type_enum_uint8 en_ota_type)
{
    if ((en_ota_type == OAM_OTA_TYPE_RX_DSCR) || (en_ota_type == OAM_OTA_TYPE_RX_DSCR_PILOT)) {
        oal_io_print("oam_report_dscr_to_console::rx_dscr info:\n\n");
    } else {
        oal_io_print("oam_report_dscr_to_console::tx_dscr info:\n\n");
    }

    oam_dump_buff_by_hex(puc_dscr_addr, us_dscr_len, 4); /* 4��ʾÿ��ӡ4�����������һ�λ��� */

    return OAL_SUCC;
}

/*
 * ���� ��   : oam_report_dscr_to_sdt
 * ��������  : ����������Ϣ�ϱ�SDT
 * �������  : puc_user_macaddr  :Ŀ���û�mac��ַ
 *             puc_dscr_addr     :��������ַ
 *             us_dscr_len       :����������
 *             en_ota_type       :ota����
 */
OAL_STATIC uint32_t oam_report_dscr_to_sdt(uint8_t *puc_user_macaddr,
                                           uint8_t *puc_dscr_addr,
                                           uint16_t us_dscr_len,
                                           oam_ota_type_enum_uint8 en_ota_type)
{
    uint32_t ul_tick;
    uint16_t us_skb_len;
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint32_t ul_ret;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* Ϊ�ϱ�����������ռ� */
    us_skb_len = us_dscr_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_dscr_len = WLAN_SDT_NETBUF_MAX_PAYLOAD - sizeof(oam_ota_hdr_stru);
    }

    pst_netbuf = oam_alloc_data2sdt(us_skb_len);
    if (oal_unlikely(pst_netbuf == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣͷ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = en_ota_type;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_dscr_len;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = 0;
    oal_set_mac_addr(pst_ota_data->st_ota_hdr.auc_user_macaddr, puc_user_macaddr);
    oam_set_ota_board_type(pst_ota_data);

    /* ��������,��дota���� */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)us_dscr_len,
                      (const void *)puc_dscr_addr,
                      (uint32_t)us_dscr_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_report_dscr_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }

    /* �ж�sdt������Ϣ�����Ƿ�������������������� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_dscr
 * ��������  : ���������ϱ�sdt,�����ϱ�tx��������rx������
 *            ע��::����rx��������󳤶�Ϊ64��tx��������󳤶�Ϊ256�������������
 *            us_dscr_len����256�Ļ����������������ֱ�ӷ���
 * �������  : puc_user_macaddr  :Ŀ���û�mac��ַ
 *             puc_dscr_addr     :��������ַ
 *             us_dscr_len       :����������
 *             en_ota_type       :ota����(rx����������tx������)���������������ϱ�
 *             ��ͨ��otaͨ���������Ҫ���������SDT������ʱ�������ʾ�ϱ�����rx��
 *             ��������tx������
 */
uint32_t oam_report_dscr(uint8_t *puc_user_macaddr,
                         uint8_t *puc_dscr_addr,
                         uint16_t us_dscr_len,
                         oam_ota_type_enum_uint8 en_ota_type)
{
    uint32_t ul_ret;

    if (oal_unlikely(oal_any_null_ptr2(puc_user_macaddr, puc_dscr_addr))) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((us_dscr_len > WLAN_MEM_SHARED_TX_DSCR_SIZE2) ||
        (us_dscr_len == 0)) {
        oam_dump_buff_by_hex(puc_dscr_addr, us_dscr_len, OAM_PRINT_CRLF_NUM);
        return OAL_ERR_CODE_OAM_EVT_DSCR_LEN_INVALID;
    }

    if (en_ota_type >= OAM_OTA_TYPE_BUTT) {
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    switch (g_oam_mng_ctx.en_output_type) {
        /* ���������̨ */
        case OAM_OUTPUT_TYPE_CONSOLE:
            ul_ret = oam_report_dscr_to_console(puc_dscr_addr, us_dscr_len, en_ota_type);

            break;

        /* �����SDT���� */
        case OAM_OUTPUT_TYPE_SDT:
            ul_ret = oam_report_dscr_to_sdt(puc_user_macaddr, puc_dscr_addr, us_dscr_len, en_ota_type);

            break;

        default:
            ul_ret = OAL_ERR_CODE_INVALID_CONFIG;

            break;
    }

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_beacon_to_console
 * ��������  : ��beacon֡���ݴ�ӡ������̨��һ���Ǵ��ڣ�
 * �������  : puc_beacon_hdr_addr :beacon֡��ַ
 *             us_beacon_len       :beacon֡����
 *             en_beacon_direction :��beacon֡�ǽ��������еĻ��Ƿ��������е�
 */
OAL_STATIC uint32_t oam_report_beacon_to_console(uint8_t *puc_beacon_hdr_addr,
                                                 uint16_t us_beacon_len,
                                                 oam_ota_frame_direction_type_enum_uint8 en_beacon_direction)
{
    uint8_t *puc_beacon_body_addr = NULL;
    uint16_t us_beacon_body_len;

    if (en_beacon_direction == OAM_OTA_FRAME_DIRECTION_TYPE_TX) {
        oal_io_print("oam_report_beacon_to_console::tx_beacon info:\n");
    } else {
        oal_io_print("oam_report_beacon_to_console::rx_beacon info:\n");
    }
    oal_io_print("oam_report_beacon_to_console::beacon_header:\n");

    oam_dump_buff_by_hex(puc_beacon_hdr_addr, OAM_BEACON_HDR_LEN, OAM_PRINT_CRLF_NUM);

    puc_beacon_body_addr = puc_beacon_hdr_addr + OAM_BEACON_HDR_LEN;
    us_beacon_body_len = us_beacon_len - OAM_BEACON_HDR_LEN;

    oal_io_print("oam_report_beacon_to_console::beacon_body:\n");
    oam_dump_buff_by_hex(puc_beacon_body_addr, us_beacon_body_len, OAM_PRINT_CRLF_NUM);

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_beacon_to_sdt
 * ��������  : ��beacon֡�����ϱ�SDT
 */
OAL_STATIC uint32_t oam_report_beacon_to_sdt(uint8_t *puc_beacon_hdr_addr,
                                             uint8_t uc_beacon_hdr_len,
                                             uint8_t *puc_beacon_body_addr,
                                             uint16_t us_beacon_len,
                                             oam_ota_frame_direction_type_enum_uint8 en_beacon_direction)
{
    uint32_t ul_tick;
    uint16_t us_skb_len;
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint32_t ul_ret;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* Ϊ�ϱ�beacon֡����ռ� */
    us_skb_len = us_beacon_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_beacon_len = WLAN_SDT_NETBUF_MAX_PAYLOAD - sizeof(oam_ota_hdr_stru);
    }

    pst_netbuf = oam_alloc_data2sdt(us_skb_len);
    if (pst_netbuf == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣͷ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = OAM_OTA_TYPE_BEACON;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = OAM_BEACON_HDR_LEN;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_beacon_len;
    pst_ota_data->st_ota_hdr.en_frame_direction = en_beacon_direction;
    oam_set_ota_board_type(pst_ota_data);

    /* ��������,��дota���� */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)us_beacon_len,
                      (const void *)puc_beacon_hdr_addr,
                      (uint32_t)uc_beacon_hdr_len);

    ul_ret += memcpy_s((void *)(pst_ota_data->auc_ota_data + uc_beacon_hdr_len),
                       (uint32_t)(us_beacon_len - uc_beacon_hdr_len),
                       (const void *)puc_beacon_body_addr,
                       (uint32_t)(us_beacon_len - uc_beacon_hdr_len));
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_report_beacon_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }

    oam_hide_mac_addr(pst_ota_data->auc_ota_data, uc_beacon_hdr_len);

    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_beacon
 * ��������  : ��beacon�����ϱ�
 */
uint32_t oam_report_beacon(uint8_t *puc_beacon_hdr_addr,
                           uint8_t uc_beacon_hdr_len,
                           uint8_t *puc_beacon_body_addr,
                           uint16_t us_beacon_len,
                           oam_ota_frame_direction_type_enum_uint8 en_beacon_direction)
{
    uint32_t ul_ret;

    if (en_beacon_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    if ((oam_ota_get_switch(OAM_OTA_SWITCH_BEACON) != OAM_SDT_PRINT_BEACON_RXDSCR_TYPE_BEACON) &&
        (oam_ota_get_switch(OAM_OTA_SWITCH_BEACON) != OAM_SDT_PRINT_BEACON_RXDSCR_TYPE_BOTH)) {
        return OAL_SUCC;
    }

    if (oal_any_null_ptr2(puc_beacon_hdr_addr, puc_beacon_body_addr)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((us_beacon_len > WLAN_MAX_FRAME_LEN) ||
        (us_beacon_len <= WLAN_MGMT_FRAME_HEADER_LEN)) {
        oam_dump_buff_by_hex(puc_beacon_hdr_addr, us_beacon_len, OAM_PRINT_CRLF_NUM);
        return OAL_ERR_CODE_OAM_EVT_FR_LEN_INVALID;
    }

    switch (g_oam_mng_ctx.en_output_type) {
        /* ���������̨ */
        case OAM_OUTPUT_TYPE_CONSOLE:
            ul_ret = oam_report_beacon_to_console(puc_beacon_hdr_addr,
                                                  us_beacon_len,
                                                  en_beacon_direction);

            break;

        /* �����SDT���� */
        case OAM_OUTPUT_TYPE_SDT:
            ul_ret = oam_report_beacon_to_sdt(puc_beacon_hdr_addr,
                                              uc_beacon_hdr_len,
                                              puc_beacon_body_addr,
                                              us_beacon_len,
                                              en_beacon_direction);

            break;

        default:
            ul_ret = OAL_ERR_CODE_INVALID_CONFIG;

            break;
    }

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_beacon_to_console
 * ��������  : ��beacon֡���ݴ�ӡ������̨��һ���Ǵ��ڣ�
 */
OAL_STATIC uint32_t oam_report_eth_frame_to_console(uint8_t *puc_eth_frame_hdr_addr,
                                                    uint16_t us_eth_frame_len,
                                                    oam_ota_frame_direction_type_enum_uint8 en_eth_frame_direction)
{
    uint8_t *puc_eth_frame_body_addr = NULL;
    uint16_t us_eth_frame_body_len;

    if (en_eth_frame_direction == OAM_OTA_FRAME_DIRECTION_TYPE_TX) {
        oal_io_print("oam_report_eth_frame_to_console::recv frame from eth:\n");
    } else {
        oal_io_print("oam_report_eth_frame_to_console::report frame to eth:\n");
    }

    oal_io_print("oam_report_eth_frame_to_console::eth_frame header:\n");
    oam_dump_buff_by_hex(puc_eth_frame_hdr_addr, ETHER_HDR_LEN, OAM_PRINT_CRLF_NUM);

    puc_eth_frame_body_addr = puc_eth_frame_hdr_addr + ETHER_HDR_LEN;
    us_eth_frame_body_len = us_eth_frame_len - ETHER_HDR_LEN;

    oal_io_print("oam_report_eth_frame_to_console::eth_frame body:\n");
    oam_dump_buff_by_hex(puc_eth_frame_body_addr, us_eth_frame_body_len, OAM_PRINT_CRLF_NUM);

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_report_eth_frame_to_sdt
 * ��������  : ����̫��֡�����ϱ�SDT
 * �������  : puc_user_mac_addr      :Ŀ���û�mac��ַ
 *             puc_eth_frame_hdr_addr :��̫��֡ͷ��ַ
 *             us_eth_frame_len       :��̫��֡����(֡ͷ+֡��)
 *             en_eth_frame_direction :������̫��(tx����)���Ƿ�����̫��(rx����)
 */
OAL_STATIC uint32_t oam_report_eth_frame_to_sdt(uint8_t *puc_user_mac_addr,
                                                uint8_t *puc_eth_frame_hdr_addr,
                                                uint16_t us_eth_frame_len,
                                                oam_ota_frame_direction_type_enum_uint8 en_eth_frame_direction)
{
    uint32_t ul_tick;
    uint16_t us_skb_len;
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint32_t ul_ret;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* Ϊ�ϱ�beacon֡����ռ� */
    us_skb_len = us_eth_frame_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_eth_frame_len = WLAN_SDT_NETBUF_MAX_PAYLOAD - sizeof(oam_ota_hdr_stru);
    }

    pst_netbuf = oam_alloc_data2sdt(us_skb_len);
    if (oal_unlikely(pst_netbuf == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣͷ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = OAM_OTA_TYPE_ETH_FRAME;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = ETHER_HDR_LEN;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_eth_frame_len;
    pst_ota_data->st_ota_hdr.en_frame_direction = en_eth_frame_direction;
    oal_set_mac_addr(pst_ota_data->st_ota_hdr.auc_user_macaddr, puc_user_mac_addr);
    oam_set_ota_board_type(pst_ota_data);

    /* ��������,��дota���� */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)us_eth_frame_len,
                      (const void *)puc_eth_frame_hdr_addr,
                      (uint32_t)us_eth_frame_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_report_eth_frame_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }
    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_eth_frame
 * ��������  : �ϱ���̫��֡���˽ӿ�ֻ�����������ط�:
 *             (1)�������̣�����̫���յ���֮����wal_bridge_vap_xmit��
 *             (2)�������̣���֡�ϱ���̫��֮ǰ����hmac_rx_transmit_msdu_to_lan�е���
 *             oal_netif_rx֮ǰ
 * �������  : puc_user_mac_addr      :Ŀ���û�mac��ַ
 *             us_user_idx            :�û���Դ��id
 *             puc_eth_frame_hdr_addr :��̫��֡ͷ��ַ
 *             us_eth_frame_len       :��̫��֡����(֡ͷ+֡��)
 *             en_eth_frame_direction :������̫��(tx����)���Ƿ�����̫��(rx����)
 */
uint32_t oam_report_eth_frame(uint8_t *puc_user_mac_addr,
                              uint8_t *puc_eth_frame_hdr_addr,
                              uint16_t us_eth_frame_len,
                              oam_ota_frame_direction_type_enum_uint8 en_eth_frame_direction)
{
    uint32_t ul_ret;

    if (oal_unlikely(oal_any_null_ptr2(puc_user_mac_addr, puc_eth_frame_hdr_addr))) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((us_eth_frame_len > ETHER_MAX_LEN) ||
        (us_eth_frame_len < ETHER_HDR_LEN)) {
        oam_dump_buff_by_hex(puc_eth_frame_hdr_addr, us_eth_frame_len, OAM_PRINT_CRLF_NUM);

        return OAL_ERR_CODE_OAM_EVT_FR_LEN_INVALID;
    }

    if (oal_unlikely(en_eth_frame_direction >= OAM_OTA_FRAME_DIRECTION_TYPE_BUTT)) {
        return OAL_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID;
    }

    switch (g_oam_mng_ctx.en_output_type) {
        /* ���������̨ */
        case OAM_OUTPUT_TYPE_CONSOLE:
            ul_ret = oam_report_eth_frame_to_console(puc_eth_frame_hdr_addr,
                                                     us_eth_frame_len,
                                                     en_eth_frame_direction);

            break;

        /* �����SDT���� */
        case OAM_OUTPUT_TYPE_SDT:
            ul_ret = oam_report_eth_frame_to_sdt(puc_user_mac_addr,
                                                 puc_eth_frame_hdr_addr,
                                                 us_eth_frame_len,
                                                 en_eth_frame_direction);
            break;

        default:
            ul_ret = OAL_ERR_CODE_INVALID_CONFIG;

            break;
    }

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_netbuf_cb_to_sdt
 * ��������  : ��80211֡��CB�ֶ��ϱ�SDT
 * �������  : puc_user_mac_addr  :Ŀ���û�mac��ַ
 *             puc_netbuf_cb      :Ҫ�ϱ���CB�ֶ�
 *             en_frame_direction :���ջ��Ƿ���
 */
OAL_STATIC uint32_t oam_report_netbuf_cb_to_sdt(uint8_t *user_mac_addr,
                                                uint8_t *netbuf_cb,
                                                uint16_t cb_len,
                                                oam_ota_type_enum_uint8 en_ota_type)
{
    uint32_t ul_ret = OAL_SUCC;
#if ((_PRE_OS_VERSION_RAW != _PRE_OS_VERSION) && (_PRE_OS_VERSION_WIN32_RAW != _PRE_OS_VERSION))
    uint32_t ul_tick;
    uint16_t us_ota_data_len;
    uint16_t us_skb_len;
    oal_netbuf_stru *pst_netbuf;
    oam_ota_stru *pst_ota_data;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    us_skb_len = cb_len + sizeof(oam_ota_hdr_stru) + OAM_RESERVE_SKB_LEN;
    us_ota_data_len = cb_len + sizeof(oam_ota_hdr_stru);

    pst_netbuf = oal_mem_sdt_netbuf_alloc(us_skb_len, OAL_TRUE);
    if (pst_netbuf == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }
    oal_netbuf_reserve(pst_netbuf, OAM_RESERVE_SKB_HEADROOM_LEN);

    oal_netbuf_put(pst_netbuf, us_ota_data_len);
    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣͷ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = en_ota_type;
    pst_ota_data->st_ota_hdr.us_ota_data_len = cb_len;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = 0;
    oal_set_mac_addr(pst_ota_data->st_ota_hdr.auc_user_macaddr, user_mac_addr);
    oam_set_ota_board_type(pst_ota_data);

    /* ��������,��дota���� */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      cb_len,
                      (const void *)netbuf_cb,
                      cb_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_report_netbuf_cb_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }
    /* �ϱ�SDT */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);
#endif
    return ul_ret;
}

/*
 * �� �� ��  : oam_report_netbuf_cb
 * ��������  : �ϱ�80211��cb�ֶ�
 * �������  : puc_user_mac_addr  :Ŀ���û�mac��ַ
 *             puc_netbuf_cb      :Ҫ�ϱ���CB�ֶ�
 *             en_frame_direction :���ջ��Ƿ���
 */
uint32_t oam_report_netbuf_cb(uint8_t *user_mac_addr,
                              uint8_t *netbuf_cb,
                              uint16_t cb_len,
                              oam_ota_type_enum_uint8 en_ota_type)
{
    uint32_t ul_ret;

    if (oal_any_null_ptr2(user_mac_addr, netbuf_cb)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (en_ota_type >= OAM_OTA_TYPE_BUTT || cb_len > OAM_SKB_CB_LEN) {
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    ul_ret = oam_report_netbuf_cb_to_sdt(user_mac_addr, netbuf_cb, cb_len, en_ota_type);

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_dft_params_to_sdt
 * ��������  : ��ά����Ϣ�����ϱ�sdt
 */
uint32_t oam_report_dft_params_to_sdt(uint8_t *puc_user_mac_addr,
                                      uint8_t *puc_param,
                                      uint16_t us_param_len,
                                      oam_ota_type_enum_uint8 en_type)
{
    uint32_t ul_tick;
    uint16_t us_ota_data_len;
    uint16_t us_skb_len;
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint32_t ul_ret;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    us_ota_data_len = us_param_len + sizeof(oam_ota_hdr_stru);
    us_skb_len = us_ota_data_len + OAM_RESERVE_SKB_LEN;

    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        return OAL_FAIL;
    }

    pst_netbuf = oal_mem_sdt_netbuf_alloc(us_skb_len, OAL_TRUE);
    if (pst_netbuf == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }
    oal_netbuf_reserve(pst_netbuf, OAM_RESERVE_SKB_HEADROOM_LEN);

    oal_netbuf_put(pst_netbuf, us_ota_data_len);
    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣͷ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = en_type;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_param_len;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = 0;
    oal_set_mac_addr(pst_ota_data->st_ota_hdr.auc_user_macaddr, puc_user_mac_addr);
    oam_set_ota_board_type(pst_ota_data);

    /* ��������,��дota���� */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)us_param_len,
                      (const void *)puc_param,
                      (uint32_t)us_param_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_report_dft_params_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }
    /* �ϱ�SDT */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_report_phy_stat
 * ��������  : �ϱ�ά����Ϣ����
 */
uint32_t oam_report_dft_params(uint8_t *puc_user_mac_addr,
                               uint8_t *puc_param,
                               uint16_t us_param_len,
                               oam_ota_type_enum_uint8 en_type)
{
    if (oal_unlikely(oal_any_null_ptr2(puc_user_mac_addr, puc_param))) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (en_type >= OAM_OTA_TYPE_BUTT) {
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    if (us_param_len != 0) {
        return oam_report_dft_params_to_sdt(puc_user_mac_addr, puc_param, us_param_len, en_type);
    }

    return OAL_ERR_CODE_INVALID_CONFIG;
}

/*
 * �� �� ��  : oam_report_set_all_switch
 * ��������  : ���������û�֡�ϱ������п��أ������1�����ϱ���������֡��֡���ݣ�
 *             cb�ֶΣ��������������0����ʲô�����ϱ�
 */
void oam_report_set_all_switch(oal_switch_enum_uint8 en_switch)
{
    memset_s(&g_oam_mng_ctx.st_user_track_ctx, sizeof(oam_user_track_ctx_stru),
        en_switch == OAL_SWITCH_OFF ? 0x00 : 0xFF, sizeof(oam_user_track_ctx_stru));
    memset_s(&g_oam_mng_ctx.ast_ota_ctx, sizeof(oam_ota_ctx_stru),
        en_switch == OAL_SWITCH_OFF ? OAL_SWITCH_OFF : OAL_SWITCH_ON, sizeof(oam_ota_ctx_stru));
    g_oam_mng_ctx.ast_ota_ctx.golble_switch = en_switch == OAL_SWITCH_ON ? 0xFFFFFFFF : 0x0;
}

/*
 * �� �� ��  : oam_report_backtrace
 * ��������  : ͨ��OAM�ϱ�����ջ
 */
void oam_report_backtrace(void)
{
    uint8_t *puc_buff = (uint8_t *)oal_mem_alloc_m(OAL_MEM_POOL_ID_LOCAL, OAM_REPORT_MAX_STRING_LEN, OAL_TRUE);

    if (puc_buff == NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{oam_report_backtrace::pc_print_buff null.}");
        return;
    }

    if (oal_dump_stack_str(puc_buff, OAM_REPORT_MAX_STRING_LEN) > 0) {
        oam_print((int8_t *)puc_buff);
    } else {
        oam_warning_log0(0, OAM_SF_CFG, "{oam_report_backtrace::dump stack str failed.}");
    }

    oal_mem_free_m(puc_buff, OAL_TRUE);
}

/*lint -e19*/
oal_module_symbol(oam_report_backtrace);
oal_module_symbol(oam_ota_report);
oal_module_symbol(oam_report_dscr);
oal_module_symbol(oam_report_beacon);
oal_module_symbol(oam_report_eth_frame);
oal_module_symbol(oam_report_80211_frame);
oal_module_symbol(oam_ota_set_switch);
oal_module_symbol(oam_ota_get_global_switch);
oal_module_symbol(oam_ota_get_switch);
oal_module_symbol(oam_report_eth_frame_set_switch);
oal_module_symbol(oam_report_eth_frame_get_switch);
oal_module_symbol(oam_report_80211_mcast_set_switch);
oal_module_symbol(oam_report_80211_mcast_get_switch);
oal_module_symbol(oam_report_80211_ucast_set_switch);
oal_module_symbol(oam_report_80211_ucast_get_switch);
oal_module_symbol(oam_report_80211_probe_set_switch);
oal_module_symbol(oam_report_80211_probe_get_switch);
oal_module_symbol(oam_report_netbuf_cb);
oal_module_symbol(oam_report_set_all_switch);
oal_module_symbol(oam_report_dhcp_arp_get_switch);
oal_module_symbol(oam_report_dhcp_arp_set_switch);
oal_module_symbol(oam_report_dft_params);
oal_module_symbol(oam_report_data_get_global_switch);
