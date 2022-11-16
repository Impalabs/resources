

#include "mac_resource.h"
#include "hmac_vap.h"
#include "hmac_user.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_RESOURCE_C

/* 2 ȫ�ֱ������� */
OAL_STATIC wlan_mib_ieee802dot11_stru g_ast_mib_res[WLAN_SERVICE_VAP_SUPPORT_MAX_NUM_LIMIT];
OAL_STATIC hmac_vap_stru g_ast_hmac_vap[WLAN_VAP_SUPPORT_MAX_NUM_LIMIT];
OAL_STATIC hmac_user_stru g_ast_hmac_user[MAC_RES_MAX_USER_LIMIT];
OAL_STATIC mac_res_stru g_st_mac_res;

mac_res_stru *g_pst_mac_res = &g_st_mac_res;


uint32_t mac_res_alloc_hmac_dev(uint8_t *dev_idx)
{
    unsigned long dev_idx_temp;

    if (oal_unlikely(dev_idx == NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{mac_res_alloc_hmac_dev::dev_idx null.}");

        return OAL_FAIL;
    }

    dev_idx_temp = (unsigned long)(uintptr_t)oal_queue_dequeue(&(g_pst_mac_res->st_dev_res.st_queue));
    /* 0Ϊ��Чֵ */
    if (dev_idx_temp == 0) {
        oam_error_log0(0, OAM_SF_ANY, "{mac_res_alloc_hmac_dev::dev_idx_temp=0.}");

        return OAL_FAIL;
    }

    *dev_idx = (uint8_t)(dev_idx_temp - 1);

    (g_pst_mac_res->st_dev_res.auc_user_cnt[dev_idx_temp - 1])++;

    return OAL_SUCC;
}

wlan_mib_ieee802dot11_stru *mac_res_get_mib_info(uint8_t vap_id)
{
    if (oal_unlikely(vap_id == 0 || vap_id >= WLAN_VAP_SUPPORT_MAX_NUM_LIMIT)) {
        return NULL;
    }

    return &g_ast_mib_res[vap_id - 1];
}


void mac_res_vap_init(void)
{
    uint32_t loop;

    oal_queue_set(&(g_pst_mac_res->st_vap_res.st_queue),
                  g_pst_mac_res->st_vap_res.aul_idx,
                  (uint8_t)WLAN_VAP_SUPPORT_MAX_NUM_LIMIT);
    g_pst_mac_res->st_vap_res.us_hmac_priv_size = (uint16_t)OAL_OFFSET_OF(hmac_vap_stru, st_vap_base_info);

    for (loop = 0; loop < WLAN_VAP_SUPPORT_MAX_NUM_LIMIT; loop++) {
        g_pst_mac_res->st_vap_res.past_vap_info[loop] = (void *)&g_ast_hmac_vap[loop];
        memset_s(g_pst_mac_res->st_vap_res.past_vap_info[loop],
                 sizeof(hmac_vap_stru), 0,
                 sizeof(hmac_vap_stru));
        /* ��ʼֵ������Ƕ�Ӧ�����±�ֵ��1 */
        oal_queue_enqueue (&(g_pst_mac_res->st_vap_res.st_queue), (void *)(uintptr_t)(loop + 1));
        /* ��ʼ����Ӧ�����ü���ֵΪ0 */
        g_pst_mac_res->st_vap_res.auc_user_cnt[loop] = 0;
    }
}


uint32_t mac_res_user_init(void)
{
    unsigned long  loop;
    void *user_info = NULL;
    void *idx = NULL;
    void *user_cnt = NULL;
    uint32_t  one_user_info_size;

    /* ��ʼ��USER����Դ�������� */
    /* ��̬�����û���Դ������ڴ� */
    user_info = (void *)g_ast_hmac_user;

    idx = oal_memalloc(sizeof(unsigned long) * MAC_RES_MAX_USER_LIMIT);
    user_cnt = oal_memalloc(sizeof(uint8_t) * MAC_RES_MAX_USER_LIMIT);
    if (oal_any_null_ptr3(user_info, idx, user_cnt)) {
        oam_error_log0(0, OAM_SF_ANY, "{mac_res_user_init::param null.}");

        if (idx != NULL) {
            oal_free(idx);
        }

        if (user_cnt != NULL) {
            oal_free(user_cnt);
        }

        return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }

    /* �ڴ��ʼ��0 */
    memset_s(user_info, (sizeof(hmac_user_stru) * MAC_RES_MAX_USER_LIMIT),
             0, (sizeof(hmac_user_stru) * MAC_RES_MAX_USER_LIMIT));

    memset_s(idx, (sizeof(unsigned long) * MAC_RES_MAX_USER_LIMIT),
             0, (sizeof(unsigned long) * MAC_RES_MAX_USER_LIMIT));
    memset_s(user_cnt, (sizeof(uint8_t) * MAC_RES_MAX_USER_LIMIT),
             0, (sizeof(uint8_t) * MAC_RES_MAX_USER_LIMIT));

    g_pst_mac_res->st_user_res.pul_idx = idx;
    g_pst_mac_res->st_user_res.puc_user_cnt = user_cnt;

    oal_queue_set_16(&(g_pst_mac_res->st_user_res.st_queue),
                     g_pst_mac_res->st_user_res.pul_idx,
                     (uint16_t)g_wlan_spec_cfg->max_user_limit);
    g_pst_mac_res->st_user_res.us_hmac_priv_size = 0;

    one_user_info_size = 0;
    g_pst_mac_res->st_user_res.past_user_info[0] = user_info;
    for (loop = 0; loop < g_wlan_spec_cfg->max_user_limit; loop++) {
        /* ��ʼֵ������Ƕ�Ӧ�����±�ֵ��1 */
        oal_queue_enqueue_16 (&(g_pst_mac_res->st_user_res.st_queue), (void *)(uintptr_t)(loop + 1));

        /* ��ʼ����Ӧ������λ�� */
        g_pst_mac_res->st_user_res.past_user_info[loop] = (uint8_t *)user_info + one_user_info_size;
        one_user_info_size += sizeof(hmac_user_stru);
    }

    return OAL_SUCC;
}

uint32_t mac_res_exit(void)
{
    uint32_t loop;

    oal_free(g_pst_mac_res->st_user_res.pul_idx);
    oal_free(g_pst_mac_res->st_user_res.puc_user_cnt);
    for (loop = 0; loop < g_wlan_spec_cfg->max_user_limit; loop++) {
        g_pst_mac_res->st_user_res.past_user_info[loop] = NULL;
    }
    g_pst_mac_res->st_user_res.pul_idx = NULL;
    g_pst_mac_res->st_user_res.puc_user_cnt = NULL;

    return OAL_SUCC;
}

/*
 * ��������  : ��ʼ��MAC��Դ������
 */
uint32_t mac_res_init(void)
{
    unsigned long    loop;

    memset_s(g_pst_mac_res, sizeof(mac_res_stru), 0, sizeof(mac_res_stru));
    /* ��ʼ��DEV����Դ�������� */
    oal_queue_set(&(g_pst_mac_res->st_dev_res.st_queue),
                  g_pst_mac_res->st_dev_res.aul_idx,
                  MAC_RES_MAX_DEV_NUM);

    for (loop = 0; loop < MAC_RES_MAX_DEV_NUM; loop++) {
        /* ��ʼֵ������Ƕ�Ӧ�����±�ֵ��1 */
        oal_queue_enqueue (&(g_pst_mac_res->st_dev_res.st_queue), (void *)(uintptr_t)(loop + 1));

        /* ��ʼ����Ӧ�����ü���ֵΪ0 */
        g_pst_mac_res->st_dev_res.auc_user_cnt[loop] = 0;
    }

    /* lint -e413 */
    /* ��ʼ��VAP����Դ�������� */
    mac_res_vap_init();

    return mac_res_user_init();
}

/*
 * ��������  : �ͷŶ�ӦHMAC USER���ڴ�
 */
uint32_t mac_res_free_mac_user(uint16_t idx)
{
    if (oal_unlikely(idx >= g_wlan_spec_cfg->max_user_limit)) {
        return OAL_FAIL;
    }

    if (g_pst_mac_res->st_user_res.puc_user_cnt[idx] == 0) {
        oam_error_log1(0, OAM_SF_ANY, "mac_res_free_mac_user::cnt==0! idx:%d", idx);
        oal_dump_stack();
        return OAL_ERR_CODE_USER_RES_CNT_ZERO;
    }

    (g_pst_mac_res->st_user_res.puc_user_cnt[idx])--;

    if (g_pst_mac_res->st_user_res.puc_user_cnt[idx] != 0) {
        return OAL_SUCC;
    }

    /* �������ֵ��Ҫ��1���� */
    oal_queue_enqueue_16(&(g_pst_mac_res->st_user_res.st_queue), (void *)(uintptr_t)((unsigned long)idx + 1));

    return OAL_SUCC;
}

/*
 * ��������  : �ͷŶ�ӦHMAC VAP���ڴ�
 */
uint32_t mac_res_free_mac_vap(uint32_t idx)
{
    if (oal_unlikely(idx >= WLAN_VAP_SUPPORT_MAX_NUM_LIMIT)) {
        return OAL_FAIL;
    }

    if (g_pst_mac_res->st_vap_res.auc_user_cnt[idx] == 0) {
        oam_error_log1(0, OAM_SF_ANY, "mac_res_free_mac_vap::cnt==0! idx:%d", idx);
        oal_dump_stack();
        return OAL_SUCC;
    }

    (g_pst_mac_res->st_vap_res.auc_user_cnt[idx])--;

    if (g_pst_mac_res->st_vap_res.auc_user_cnt[idx] != 0) {
        return OAL_SUCC;
    }

    /* �������ֵ��Ҫ��1���� */
    oal_queue_enqueue(&(g_pst_mac_res->st_vap_res.st_queue), (void *)(uintptr_t)((unsigned long)idx + 1));

    return OAL_SUCC;
}

/*
 * ��������  : �ͷŶ�ӦDEV���ڴ�
 */
uint32_t mac_res_free_dev(uint32_t dev_idx)
{
    if (oal_unlikely(dev_idx >= MAC_RES_MAX_DEV_NUM)) {
        oam_error_log1(0, OAM_SF_ANY, "{mac_res_free_dev::invalid dev_idx[%d].}", dev_idx);

        return OAL_FAIL;
    }

    if (g_pst_mac_res->st_dev_res.auc_user_cnt[dev_idx] == 0) {
        oam_error_log1(0, OAM_SF_ANY, "mac_res_free_dev::cnt==0! idx:%d", dev_idx);
        oal_dump_stack();
        return OAL_SUCC;
    }

    (g_pst_mac_res->st_dev_res.auc_user_cnt[dev_idx])--;

    if (g_pst_mac_res->st_dev_res.auc_user_cnt[dev_idx] != 0) {
        return OAL_SUCC;
    }

    /* �������ֵ��Ҫ��1���� */
    oal_queue_enqueue(&(g_pst_mac_res->st_dev_res.st_queue), (void *)(uintptr_t)((unsigned long)dev_idx + 1));

    return OAL_SUCC;
}

/*
 * ��������  : ��ȡ��ӦMAC CHIP�������ڴ�
 */
mac_chip_stru *mac_res_get_mac_chip(uint32_t chip_idx)
{
    if (oal_unlikely(chip_idx >= WLAN_CHIP_MAX_NUM_PER_BOARD)) {
        oam_error_log1(0, OAM_SF_ANY, "{mac_res_get_mac_chip::invalid chip_idx[%d].}", chip_idx);
        return NULL;
    }

    return &(g_pst_mac_board->ast_chip[chip_idx]);
}

uint16_t mac_chip_get_max_asoc_user(uint8_t chip_idx)
{
    /* ������������û��� */
    return g_wlan_spec_cfg->max_asoc_user;
}

uint8_t mac_chip_get_max_active_user(void)
{
    /* ��������active����û��� */
    return g_wlan_spec_cfg->max_active_user;
}
uint16_t mac_board_get_max_user(void)
{
    /* ���嵥�����鲥�û����������嵥�����鲥�û���ֱ��ȡ�ú�ֵ���� */
    return g_wlan_spec_cfg->max_user_limit;
}

