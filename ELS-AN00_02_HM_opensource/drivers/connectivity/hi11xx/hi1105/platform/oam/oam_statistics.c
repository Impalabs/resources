

/* ͷ�ļ����� */
#include "oam_main.h"
#include "oam_statistics.h"
#include "securec.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_OAM_STATISTICS_C

/* ͳ����Ϣȫ�ֱ��� */
oam_stat_info_stru g_oam_stat_info;

/* ����ʵ�� */
void oam_stats_report_irq_info_to_sdt(uint8_t *puc_irq_info_addr, uint16_t us_irq_info_len)
{
    uint32_t ul_tick;
    uint16_t us_skb_len; /* skb�ܳ��� */
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    int32_t ret;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return;
    }

    if (puc_irq_info_addr == NULL) {
        oal_io_print("oam_stats_report_irq_info_to_sdt::puc_irq_info_addr is null!\n");
        return;
    }

    /* Ϊ�ϱ�����������ռ�,ͷ��Ԥ��8�ֽڣ�β��Ԥ��1�ֽڣ���sdt_drv�� */
    us_skb_len = us_irq_info_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_irq_info_len = WLAN_SDT_NETBUF_MAX_PAYLOAD - sizeof(oam_ota_hdr_stru);
    }

    pst_netbuf = oam_alloc_data2sdt(us_skb_len);
    if (pst_netbuf == NULL) {
        return;
    }

    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣͷ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = OAM_OTA_TYPE_IRQ;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = 0;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_irq_info_len;
    pst_ota_data->st_ota_hdr.auc_resv[0] = OAM_OTA_TYPE_1103_HOST;

    /* ��������,��дota���� */
    ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                   (uint32_t)pst_ota_data->st_ota_hdr.us_ota_data_len,
                   (const void *)puc_irq_info_addr,
                   (uint32_t)us_irq_info_len);
    if (ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_stats_report_irq_info_to_sdt::memcpy_s failed.\n");
        return;
    }
    /* �·���sdt���ն��У����������򴮿���� */
    oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);
}

/*
 * �� �� ��  : oam_stats_report_timer_info_to_sdt
 * ��������  : �������ʱ������Ϣ�ϱ�SDT
 * �������  : puc_timer_addr:��ʱ���ṹ�ĵ�ַ
 *             uc_timer_len  :��ʱ���ṹ�ĳ���
 */
uint32_t oam_stats_report_timer_info_to_sdt(uint8_t *puc_timer_addr, uint8_t uc_timer_len)
{
    uint32_t ul_ret = OAL_SUCC;

    if (puc_timer_addr != NULL) {
        ul_ret = oam_ota_report(puc_timer_addr, uc_timer_len, 0, 0, OAM_OTA_TYPE_TIMER);
        return ul_ret;
    } else {
        oal_io_print("oam_stats_report_timer_info_to_sdt::puc_timer_addr is NULL");
        return OAL_ERR_CODE_PTR_NULL;
    }
}

/*
 * �� �� ��  : oam_stats_report_mempool_info_to_sdt
 * ��������  : ���ڴ�ص�ĳһ���ӳ��ڴ���ʹ������ϱ�sdt
 * �������  : uc_pool_id            :�ڴ��id
 *             us_pool_total_cnt     :���ڴ��һ�������ڴ��
 *             us_pool_used_cnt      :���ڴ�������ڴ��
 *             uc_subpool_id         :�ӳ�id
 *             us_subpool_total_cnt  :���ӳ��ڴ������
 *             us_subpool_free_cnt   :���ӳؿ����ڴ�����
 */
uint32_t oam_stats_report_mempool_info_to_sdt(uint8_t uc_pool_id,
                                              uint16_t us_pool_total_cnt,
                                              uint16_t us_pool_used_cnt,
                                              uint8_t uc_subpool_id,
                                              uint16_t us_subpool_total_cnt,
                                              uint16_t us_subpool_free_cnt)
{
    oam_stats_mempool_stru st_device_mempool_info;
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint16_t us_skb_len; /* skb�ܳ��� */
    uint32_t ul_tick;
    uint32_t ul_ret;
    uint16_t us_stru_len;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��дҪ�ϱ���sdt���ڴ����Ϣ�ṹ�� */
    st_device_mempool_info.uc_mem_pool_id = uc_pool_id;
    st_device_mempool_info.uc_subpool_id = uc_subpool_id;
    st_device_mempool_info.auc_resv[0] = 0;
    st_device_mempool_info.auc_resv[1] = 0;
    st_device_mempool_info.us_mem_pool_total_cnt = us_pool_total_cnt;
    st_device_mempool_info.us_mem_pool_used_cnt = us_pool_used_cnt;
    st_device_mempool_info.us_subpool_total_cnt = us_subpool_total_cnt;
    st_device_mempool_info.us_subpool_free_cnt = us_subpool_free_cnt;

    us_stru_len = sizeof(oam_stats_mempool_stru);
    /* Ϊota��Ϣ�ϱ�SDT����ռ�,ͷ��Ԥ��8�ֽڣ�β��Ԥ��1�ֽڣ���sdt_drv�� */
    us_skb_len = us_stru_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_stru_len = us_skb_len - sizeof(oam_ota_hdr_stru);
    }

    pst_netbuf = oam_alloc_data2sdt(us_skb_len);
    if (pst_netbuf == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = OAM_OTA_TYPE_MEMPOOL;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = 0;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_stru_len;

    /* ��������,��дota���� */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)pst_ota_data->st_ota_hdr.us_ota_data_len,
                      (const void *)&st_device_mempool_info,
                      (uint32_t)us_stru_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_stats_report_mempool_info_to_sdt::memcpy_s failed.\n");
        return OAL_FAIL;
    }
    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_stats_report_memblock_info_to_sdt
 * ��������  : ����׼�ڴ�����Ϣ�ϱ�SDT
 * �������  : puc_origin_data:�ڴ�����ʼ��ַ
 *             uc_user_cnt    :���ڴ�����ü���
 *             uc_pool_id     :�������ڴ��id
 *             uc_subpool_id  :�������ӳ�id
 *             us_len         :���ڴ�鳤��
 *             ul_file_id     :������ڴ����ļ�id
 *             ul_alloc_line_num :������ڴ����к�
 */
uint32_t oam_stats_report_memblock_info_to_sdt(uint8_t *puc_origin_data,
                                               uint8_t uc_user_cnt,
                                               uint8_t uc_pool_id,
                                               uint8_t uc_subpool_id,
                                               uint16_t us_len,
                                               uint32_t ul_file_id,
                                               uint32_t ul_alloc_line_num)
{
    oam_memblock_info_stru st_memblock_info;
    uint16_t us_memblock_info_len;
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint16_t us_skb_len; /* skb�ܳ��� */
    uint32_t ul_tick;
    uint32_t ul_ret;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (puc_origin_data == NULL) {
        oal_io_print("oam_stats_report_memblock_info_to_sdt:puc_origin_data is null!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    us_memblock_info_len = sizeof(oam_memblock_info_stru);

    /* ��дҪ�ϱ���sdt���ڴ����Ϣ�ṹ�� */
    st_memblock_info.uc_pool_id = uc_pool_id;
    st_memblock_info.uc_subpool_id = uc_subpool_id;
    st_memblock_info.uc_user_cnt = uc_user_cnt;
    st_memblock_info.auc_resv[0] = 0;
    st_memblock_info.ul_alloc_line_num = ul_alloc_line_num;
    st_memblock_info.ul_file_id = ul_file_id;

    /* Ϊota��Ϣ�ϱ�SDT����ռ�,ͷ��Ԥ��8�ֽڣ�β��Ԥ��1�ֽڣ���sdt_drv�� */
    us_skb_len = us_memblock_info_len + us_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        if ((us_memblock_info_len + sizeof(oam_ota_hdr_stru)) < us_skb_len) {
            us_len = us_skb_len - us_memblock_info_len - (uint16_t)sizeof(oam_ota_hdr_stru);
        } else {
            us_memblock_info_len = us_skb_len - sizeof(oam_ota_hdr_stru);
            us_len = 0;
        }
    }

    pst_netbuf = oam_alloc_data2sdt(us_skb_len);
    if (pst_netbuf == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_ota_data = (oam_ota_stru *)oal_netbuf_data(pst_netbuf);

    /* ��ȡϵͳTICKֵ */
    ul_tick = (uint32_t)oal_time_get_stamp_ms();

    /* ��дota��Ϣ�ṹ�� */
    pst_ota_data->st_ota_hdr.ul_tick = ul_tick;
    pst_ota_data->st_ota_hdr.en_ota_type = OAM_OTA_TYPE_MEMBLOCK;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = (uint8_t)us_memblock_info_len;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_memblock_info_len + us_len;

    /* ��дota���ݲ���,���ȸ����ڴ�����Ϣ�ṹ�� */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)pst_ota_data->st_ota_hdr.us_ota_data_len,
                      (const void *)&st_memblock_info,
                      (uint32_t)us_memblock_info_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_stats_report_memblock_info_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }

    /* �����ڴ��ľ������� */ /*lint -e416*/
    ul_ret = memcpy_s((void *)(pst_ota_data->auc_ota_data + us_memblock_info_len),
                      (uint32_t)(pst_ota_data->st_ota_hdr.us_ota_data_len - us_memblock_info_len),
                      (const void *)puc_origin_data,
                      (uint32_t)us_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_stats_report_memblock_info_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }

    /*lint +e416*/
    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_stats_report_event_queue_info_to_sdt
 * ��������  : ���¼�������ÿ���¼����¼�ͷ��Ϣ�ϱ�SDT
 * �������  : puc_event_queue_addr:�¼�������Ϣ��ַ
 *             uc_event_queue_info_len:�¼�������Ϣ����
 */
uint32_t oam_stats_report_event_queue_info_to_sdt(uint8_t *puc_event_queue_addr,
                                                  uint16_t us_event_queue_info_len)
{
    uint32_t ul_tick;
    uint16_t us_skb_len; /* skb�ܳ��� */
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint32_t ul_ret;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (puc_event_queue_addr == NULL) {
        oal_io_print("oam_stats_report_event_queue_info_to_sdt::puc_event_queue_addr is null!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* Ϊ�ϱ�����������ռ�,ͷ��Ԥ��8�ֽڣ�β��Ԥ��1�ֽڣ���sdt_drv�� */
    us_skb_len = us_event_queue_info_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_event_queue_info_len = WLAN_SDT_NETBUF_MAX_PAYLOAD - sizeof(oam_ota_hdr_stru);
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
    pst_ota_data->st_ota_hdr.en_ota_type = OAM_OTA_TYPE_EVENT_QUEUE;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = 0;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_event_queue_info_len;

    /* ��������,��дota���� */
    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)pst_ota_data->st_ota_hdr.us_ota_data_len,
                      (const void *)puc_event_queue_addr,
                      (uint32_t)us_event_queue_info_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_stats_report_event_queue_info_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }

    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_stats_report_stat_info_to_sdt
 * ��������  : ������ά��ͳ����Ϣ�ϱ�SDT����
 */
uint32_t oam_stats_report_info_to_sdt(oam_ota_type_enum_uint8 en_ota_type)
{
    uint32_t ul_tick;
    uint16_t us_skb_len; /* skb�ܳ��� */
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint32_t ul_ret = OAL_SUCC;
    uint16_t us_stat_info_len;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    switch (en_ota_type) {
        case OAM_OTA_TYPE_DEV_STAT_INFO:
            us_stat_info_len = sizeof(oam_device_stat_info_stru) * WLAN_DEVICE_MAX_NUM_PER_CHIP;

            break;

        case OAM_OTA_TYPE_VAP_STAT_INFO:
            us_stat_info_len = (uint16_t)(sizeof(oam_vap_stat_info_stru) * WLAN_VAP_SUPPORT_MAX_NUM_LIMIT);

            break;

        default:
            us_stat_info_len = 0;

            break;
    }

    if (us_stat_info_len == 0) {
        oal_io_print("oam_stats_report_info_to_sdt::ota_type invalid-->%d!\n", en_ota_type);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /* Ϊ�ϱ�ͳ����Ϣ����ռ�,ͷ��Ԥ��8�ֽڣ�β��Ԥ��1�ֽڣ���sdt_drv�� */
    us_skb_len = us_stat_info_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_stat_info_len = WLAN_SDT_NETBUF_MAX_PAYLOAD - sizeof(oam_ota_hdr_stru);
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
    pst_ota_data->st_ota_hdr.en_ota_type = en_ota_type;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = 0;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_stat_info_len;

    /* ��������,��дota���� */
    if (en_ota_type == OAM_OTA_TYPE_DEV_STAT_INFO) {
        ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                          (uint32_t)pst_ota_data->st_ota_hdr.us_ota_data_len,
                          (const void *)g_oam_stat_info.ast_dev_stat_info,
                          (uint32_t)us_stat_info_len);
    } else {
        ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                          (uint32_t)pst_ota_data->st_ota_hdr.us_ota_data_len,
                          (const void *)g_oam_stat_info.ast_vap_stat_info,
                          (uint32_t)us_stat_info_len);
    }
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_stats_report_info_to_sdt:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }

    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*
 * �� �� ��  : oam_stats_report_usr_info
 * ��������  : ��ĳ���û���ͳ����Ϣ�ϱ�sdt
 */
uint32_t oam_stats_report_usr_info(uint16_t us_usr_id)
{
    uint32_t ul_tick;
    uint16_t us_skb_len; /* skb�ܳ��� */
    oal_netbuf_stru *pst_netbuf = NULL;
    oam_ota_stru *pst_ota_data = NULL;
    uint32_t ul_ret;
    uint16_t us_stat_info_len;

    if (oal_unlikely(g_oam_sdt_func_hook.p_sdt_report_data_func == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (us_usr_id >= WLAN_USER_MAX_USER_LIMIT) {
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    us_stat_info_len = sizeof(oam_device_stat_info_stru);

    /* Ϊ�ϱ�ͳ����Ϣ����ռ�,ͷ��Ԥ��8�ֽڣ�β��Ԥ��1�ֽڣ���sdt_drv�� */
    us_skb_len = us_stat_info_len + sizeof(oam_ota_hdr_stru);
    if (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) {
        us_skb_len = WLAN_SDT_NETBUF_MAX_PAYLOAD;
        us_stat_info_len = WLAN_SDT_NETBUF_MAX_PAYLOAD - sizeof(oam_ota_hdr_stru);
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
    pst_ota_data->st_ota_hdr.en_ota_type = OAM_OTA_TYPE_USER_STAT_INFO;
    pst_ota_data->st_ota_hdr.uc_frame_hdr_len = 0;
    pst_ota_data->st_ota_hdr.us_ota_data_len = us_stat_info_len;

    ul_ret = memcpy_s((void *)pst_ota_data->auc_ota_data,
                      (uint32_t)pst_ota_data->st_ota_hdr.us_ota_data_len,
                      (const void *)&g_oam_stat_info.ast_user_stat_info[us_usr_id],
                      (uint32_t)us_stat_info_len);
    if (ul_ret != EOK) {
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        oal_io_print("oam_stats_report_usr_info:: memcpy_s failed\r\n");
        return OAL_FAIL;
    }

    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_OTA, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

void oam_stats_clear_stat_info(void)
{
    memset_s(&g_oam_stat_info, sizeof(oam_stat_info_stru), 0, sizeof(oam_stat_info_stru));
}

/*
 * �� �� ��  : oam_stats_clear_vap_stat_info
 * ��������  : vap������ʱ�������Ӧ��ͳ����Ϣ
 */
uint32_t oam_stats_clear_vap_stat_info(uint8_t uc_vap_id)
{
    if (uc_vap_id >= WLAN_VAP_SUPPORT_MAX_NUM_LIMIT) {
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    memset_s(&g_oam_stat_info.ast_vap_stat_info[uc_vap_id], sizeof(oam_vap_stat_info_stru),
             0, sizeof(oam_vap_stat_info_stru));

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_stats_clear_user_stat_info
 * ��������  : �û�������ʱ�������Ӧ��ͳ����Ϣ����Ϊuser_id�Ǹ��õ�
 */
uint32_t oam_stats_clear_user_stat_info(uint16_t us_usr_id)
{
    if (us_usr_id >= WLAN_USER_MAX_USER_LIMIT) {
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    memset_s(&g_oam_stat_info.ast_user_stat_info[us_usr_id], sizeof(oam_user_stat_info_stru),
             0, sizeof(oam_user_stat_info_stru));

    return OAL_SUCC;
}

uint32_t oam_statistics_init(void)
{
#if ((_PRE_OS_VERSION_WIN32_RAW != _PRE_OS_VERSION))
    oal_mempool_info_to_sdt_register(oam_stats_report_mempool_info_to_sdt,
                                     oam_stats_report_memblock_info_to_sdt);

    memset_s(&g_oam_stat_info, sizeof(oam_stat_info_stru), 0, sizeof(oam_stat_info_stru));
#endif
    return OAL_SUCC;
}

/*lint -e19*/
oal_module_symbol(oam_stats_report_irq_info_to_sdt);
oal_module_symbol(oam_stats_report_timer_info_to_sdt);
oal_module_symbol(oam_stats_report_mempool_info_to_sdt);
oal_module_symbol(oam_statistics_init);
oal_module_symbol(oam_stats_report_memblock_info_to_sdt);
oal_module_symbol(oam_stats_report_event_queue_info_to_sdt);
oal_module_symbol(g_oam_stat_info);
oal_module_symbol(oam_stats_report_info_to_sdt);
oal_module_symbol(oam_stats_clear_stat_info);
oal_module_symbol(oam_stats_clear_user_stat_info);
oal_module_symbol(oam_stats_clear_vap_stat_info);
oal_module_symbol(oam_stats_report_usr_info);
