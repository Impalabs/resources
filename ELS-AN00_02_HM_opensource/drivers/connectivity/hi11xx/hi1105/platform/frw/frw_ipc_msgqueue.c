

/* ͷ�ļ����� */
#include "frw_main.h"
#include "frw_ipc_msgqueue.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_FRW_IPC_MSGQUEUE_C

/* ȫ�ֱ������� */
OAL_STATIC frw_ipc_msg_callback_stru g_gst_ipc_msg_callback;

/*
 * �� �� ��  : frw_ipc_msg_queue_init
 * ��������  : ��Ϣ���г�ʼ�����ڴ����
 * �������  : pst_msg_queue: ��Ϣ���нṹ��ָ��
 *             ul_queue_len: ��Ϣ���г���
 * �� �� ֵ  : �ɹ�����OAL_SUCC��
 *             ��μ��Ϊ��ָ��: OAL_ERR_CODE_PTR_NULL
 *             �ڴ����ʧ��: OAL_ERR_CODE_ALLOC_MEM_FAIL
 */
uint32_t frw_ipc_msg_queue_init(frw_ipc_msg_queue_stru *pst_msg_queue, uint32_t ul_queue_len)
{
    uint16_t us_queue_size;

    if (oal_unlikely(pst_msg_queue == NULL)) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_msg_queue_init:: pst_msg_queue is null ptr!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* Ϊ������Ϣ���з����ڴ� */
    us_queue_size = (uint16_t)(sizeof(frw_ipc_msg_dscr_stru) * ul_queue_len);
    pst_msg_queue->pst_dscr = (frw_ipc_msg_dscr_stru *)oal_mem_alloc_m(OAL_MEM_POOL_ID_LOCAL, us_queue_size, OAL_TRUE);
    if (oal_unlikely(pst_msg_queue->pst_dscr == NULL)) {
        oam_warning_log0(0, OAM_SF_FRW, "{frw_ipc_msg_queue_init:: pst_msg_queue->pst_dscr is null ptr!}");
        return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }

    /* �ṹ�������ʼ�� */
    pst_msg_queue->ul_head = 0;
    pst_msg_queue->ul_tail = 0;
    pst_msg_queue->ul_max_num = ul_queue_len;

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_msg_queue_destroy
 * ��������  : �ͷ���Ϣ�����ڴ�ռ�
 * �������  : pst_msg_queue: ��Ϣ���нṹ��ָ��
 */
uint32_t frw_ipc_msg_queue_destroy(frw_ipc_msg_queue_stru *pst_msg_queue)
{
    if (pst_msg_queue->pst_dscr == NULL) {
        oam_warning_log0(0, OAM_SF_FRW, "{frw_ipc_msg_queue_destroy:: pst_msg_queue->pst_dscr is null ptr}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oal_mem_free_m(pst_msg_queue->pst_dscr, OAL_TRUE);

    pst_msg_queue->pst_dscr = NULL;

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_msg_queue_recv
 * ��������  : ��Ϣ���պ��������˼䷢�������ж�ʱ������ú˼��жϴ���������
 *             �жϴ����жϴ�����������ipc_msg_queue_recv������Ϣ���ú�
 *             ���������ж������ġ�
 */
uint32_t frw_ipc_msg_queue_recv(void *p_arg)
{
    frw_ipc_msg_queue_stru *pst_ipc_rx_msg_queue = NULL;
    uint32_t ul_head = 0;

    if (oal_unlikely(p_arg == NULL)) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_msg_queue_recv:: p_arg is null ptr!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_ipc_rx_msg_queue = (frw_ipc_msg_queue_stru *)(((oal_irq_dev_stru *)p_arg)->p_drv_arg);
    if (oal_unlikely(g_gst_ipc_msg_callback.p_rx_complete_func == NULL)) {
        oam_warning_log0(0, OAM_SF_FRW,
                         "{frw_ipc_msg_queue_recv:: g_gst_ipc_msg_callback.p_rx_complete_func is null ptr!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���в��� */
    do {
        ul_head = (pst_ipc_rx_msg_queue->ul_head);
        frw_ipc_ring_rx_incr(pst_ipc_rx_msg_queue->ul_head);

        /* �ص�ipc_recv() */
        g_gst_ipc_msg_callback.p_rx_complete_func(pst_ipc_rx_msg_queue->pst_dscr[ul_head].pst_msg_mem);
    } while (!frw_ipc_ring_empty(pst_ipc_rx_msg_queue->ul_head, pst_ipc_rx_msg_queue->ul_tail));

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_msg_queue_send(ipc_node *node, oal_netbuf_t *buf);
 * ��������  : �˼���Ϣ���Ͳ�����
 * �������  : pst_ipc_tx_msg_queue: ������Ϣ���нṹ��ָ��
 *             pst_msg_input: �˼�ͨѶ��Ϣ��ַ
 *             uc_flags: �Ƿ�������жϵı�־
 *             uc_cpuid: ����Ŀ���ID
 */
uint32_t frw_ipc_msg_queue_send(frw_ipc_msg_queue_stru *pst_ipc_tx_msg_queue,
                                frw_ipc_msg_mem_stru *pst_msg_input,
                                uint8_t uc_flags, uint8_t uc_cpuid)
{
    uint32_t ul_tail;

    if (oal_unlikely((pst_ipc_tx_msg_queue == NULL) || pst_msg_input == NULL)) {
        oam_error_log2(0, OAM_SF_FRW,
                       "{frw_ipc_msg_queue_send: pst_ipc_tx_msg_queue/pst_msg_input is null ptr: %d %d}",
                       (uintptr_t)pst_ipc_tx_msg_queue, (uintptr_t)pst_msg_input);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* �ж϶����Ƿ��� */
    if (oal_unlikely(frw_ipc_ring_full(pst_ipc_tx_msg_queue->ul_head,
                                       pst_ipc_tx_msg_queue->ul_tail,
                                       pst_ipc_tx_msg_queue->ul_max_num))) {
        oam_warning_log0(0, OAM_SF_FRW,
                         "{frw_ipc_msg_queue_send:: frw_ipc_ring_full OAL_ERR_CODE_IPC_QUEUE_FULL.}");
        return OAL_ERR_CODE_IPC_QUEUE_FULL;
    }

    ul_tail = pst_ipc_tx_msg_queue->ul_tail;
    frw_ipc_ring_tx_incr(pst_ipc_tx_msg_queue->ul_tail);

    pst_ipc_tx_msg_queue->pst_dscr[ul_tail].pst_msg_mem = pst_msg_input;

    /* Ŀ������Ϊ����������Ŀ���Ӳ���ж� */
    if (uc_flags == FRW_IPC_TX_CTRL_ENABLED) {
        oal_irq_trigger(uc_cpuid);
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_msg_queue_register_callback
 * ��������  : ��������ע��
 * �������  : p_ipc_msg_handler: �������ӽṹ��ָ��
 */
uint32_t frw_ipc_msg_queue_register_callback(frw_ipc_msg_callback_stru *p_ipc_msg_handler)
{
    if (oal_unlikely(p_ipc_msg_handler == NULL)) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_msg_queue_register_callback:: p_ipc_msg_handler is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    g_gst_ipc_msg_callback.p_rx_complete_func = p_ipc_msg_handler->p_rx_complete_func;
    g_gst_ipc_msg_callback.p_tx_complete_func = p_ipc_msg_handler->p_tx_complete_func;

    return OAL_SUCC;
}

uint32_t frw_ipc_log_exit(frw_ipc_log_stru *pst_log)
{
    return OAL_SUCC;
}

uint32_t frw_ipc_log_init(frw_ipc_log_stru *pst_log)
{
    if (pst_log == NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_log_init:: pst_log is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_log->ul_stats_assert = 0;
    pst_log->ul_stats_send_lost = 0;
    pst_log->ul_stats_recv_lost = 0;
    pst_log->ul_stats_recv = 0;
    pst_log->ul_stats_send = 0;
    pst_log->ul_tx_index = 0;
    pst_log->ul_rx_index = 0;

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_log_recv_alarm
 * ��������  : �˼�ͨ�Ž��ն����澯
 * �������  : pst_log: ά����Ϣ�ṹ��ָ��
 *             ul_lost: ��������
 */
uint32_t frw_ipc_log_recv_alarm(frw_ipc_log_stru *pst_log, uint32_t ul_lost)
{
    int32_t l_lost;
    int32_t l_assert;

    if (pst_log == NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_log_recv_alarm:: pst_log is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_log->ul_stats_recv_lost += ul_lost; /* ���������� */
    pst_log->ul_stats_assert++;             /* �澯�������� */

    l_lost = (int32_t)pst_log->ul_stats_recv_lost;
    l_assert = (int32_t)pst_log->ul_stats_assert;

    frw_ipc_lost_warning_log2(0, "The number of rx lost package respectively are ", l_lost, l_assert);
    oam_warning_log2(0, OAM_SF_FRW,
                     "{frw_ipc_log_recv_alarm::The number of rx lost package respectively are  %d %d}",
                     l_lost, l_assert);

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_log_send_alarm
 * ��������  : �˼�ͨ�ŷ��Ͷ����澯
 * �������  : pst_log: ά����Ϣ�ṹ��ָ��
 */
uint32_t frw_ipc_log_send_alarm(frw_ipc_log_stru *pst_log)
{
    int32_t l_lost;

    if (pst_log == NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_log_send_alarm:: pst_log is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_log->ul_stats_send_lost++;
    pst_log->ul_stats_assert++; /* �澯�������� */

    l_lost = (int32_t)pst_log->ul_stats_send_lost;

    frw_ipc_lost_warning_log1(0, "The number of tx lost package respectively are ", l_lost);
    oam_warning_log1(0, OAM_SF_FRW, "{frw_ipc_log_send_alarm::the number of tx lost packets are %d. }\r\n", l_lost);
    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_log_send
 * ��������  : ͳ�Ʒ�����Ϣ
 */
uint32_t frw_ipc_log_send(frw_ipc_log_stru *pst_log, uint16_t us_seq_num,
                          uint8_t uc_target_cpuid, uint8_t uc_msg_type)
{
    if (pst_log == NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_log_send:: pst_log is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (pst_log->ul_tx_index != MAX_LOG_RECORD) {
        pst_log->ul_stats_send++;
        pst_log->st_tx_stats_record[pst_log->ul_tx_index].us_seq_num = us_seq_num;
        pst_log->st_tx_stats_record[pst_log->ul_tx_index].uc_target_cpuid = uc_target_cpuid;
        pst_log->st_tx_stats_record[pst_log->ul_tx_index].uc_msg_type = uc_msg_type;
        pst_log->st_tx_stats_record[pst_log->ul_tx_index].l_time_stamp = oal_time_get_stamp_ms();
        pst_log->ul_tx_index++;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_log_recv
 * ��������  : ͳ�ƽ�����Ϣ
 */
uint32_t frw_ipc_log_recv(frw_ipc_log_stru *pst_log, uint16_t us_seq_num,
                          uint8_t uc_target_cpuid, uint8_t uc_msg_type)
{
    if (pst_log == NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_log_recv:: pst_log is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (pst_log->ul_rx_index != MAX_LOG_RECORD) {
        pst_log->ul_stats_recv++;
        pst_log->st_rx_stats_record[pst_log->ul_rx_index].us_seq_num = us_seq_num;
        pst_log->st_rx_stats_record[pst_log->ul_rx_index].uc_target_cpuid = uc_target_cpuid;
        pst_log->st_rx_stats_record[pst_log->ul_rx_index].uc_msg_type = uc_msg_type;
        pst_log->st_rx_stats_record[pst_log->ul_rx_index].l_time_stamp = oal_time_get_stamp_ms();
        pst_log->ul_rx_index++;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_log_tx_print
 * ��������  : ����¼�ڷ�����־�����е���־��ӡ
 * �������  : pst_log: ��־��Ϣ�ṹ��
 */
uint32_t frw_ipc_log_tx_print(frw_ipc_log_stru *pst_log)
{
    uint16_t us_log_index = 0;

    if (pst_log == NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_log_tx_print:: pst_log is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    for (us_log_index = 0; us_log_index < pst_log->ul_tx_index; us_log_index++) {
        frw_ipc_log_info_print4(0, "SEND SEQUENCE NUMBER:  TARGET CPUID:  MESSAGE TYPE: TIME STAMP: ",
                                (int32_t)pst_log->st_tx_stats_record[us_log_index].us_seq_num,
                                (int32_t)pst_log->st_tx_stats_record[us_log_index].uc_target_cpuid,
                                (int32_t)pst_log->st_tx_stats_record[us_log_index].uc_msg_type,
                                (int32_t)pst_log->st_tx_stats_record[us_log_index].l_time_stamp);
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_ipc_log_rx_print
 * ��������  : ����¼�ڽ�����־�����е���־��Ϣ��ӡ
 * �������  : pst_log: ��־��Ϣ�ṹ��
 */
uint32_t frw_ipc_log_rx_print(frw_ipc_log_stru *pst_log)
{
    uint16_t us_log_index = 0;

    if (pst_log == NULL) {
        oam_error_log0(0, OAM_SF_FRW, "{frw_ipc_log_rx_print:: pst_log is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    frw_ipc_log_info_print1(0, "times of recieve:", (int32_t)pst_log->ul_stats_recv);

    for (us_log_index = 0; us_log_index < pst_log->ul_rx_index; us_log_index++) {
        frw_ipc_log_info_print4(0, "RECEIVE SEQUENCE NUMBER: TARGET CPUID: MESSAGE TYPE:  TIME STAMP:",
                                (int32_t)pst_log->st_rx_stats_record[us_log_index].us_seq_num,
                                (int32_t)pst_log->st_rx_stats_record[us_log_index].uc_target_cpuid,
                                (int32_t)pst_log->st_rx_stats_record[us_log_index].uc_msg_type,
                                (int32_t)pst_log->st_rx_stats_record[us_log_index].l_time_stamp);
    }

    return OAL_SUCC;
}
