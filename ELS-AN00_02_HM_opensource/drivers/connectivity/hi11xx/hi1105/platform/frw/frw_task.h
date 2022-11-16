

#ifndef __FRW_TASK_H__
#define __FRW_TASK_H__

/* ����ͷ�ļ����� */
#include "oal_ext_if.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_FRW_TASK_H

/* �̵߳�����״̬ */
typedef enum {
    FRW_TASK_STATE_IRQ_UNBIND = 0, /* �̴߳�����ͣ״̬ */
    FRW_TASK_STATE_IRQ_BIND,       /* �̴߳�������״̬ */

    FRW_TASK_BUTT
} frw_task_state_enum;

/* STRUCT ���� */
#if (_PRE_FRW_FEATURE_PROCCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCCESS_ENTITY_THREAD)
typedef struct {
    oal_task_stru *pst_event_kthread; /*  task���� */
    oal_wait_queue_head_stru frw_wq;  /* waitqueue */
    uint8_t uc_task_state;          /* �߳�״̬��0��ͣ��1���� */
    uint8_t auc_resv[3]; /* 3 */
    uint32_t ul_total_loop_cnt;
    uint32_t ul_total_event_cnt; /* ͳ���߳̿�ת���� */
    uint32_t ul_max_empty_count;
    void (*p_event_handler_func)(void *_pst_bind_cpu); /* kthread������ */
} frw_task_stru;

#elif (_PRE_FRW_FEATURE_PROCCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCCESS_ENTITY_TASKLET)
typedef struct {
    oal_tasklet_stru st_ipi_tasklet;
    oal_tasklet_stru st_event_tasklet;               /* tasklet���� */
    void (*p_event_handler_func)(oal_uint); /* tasklet������� */
} frw_task_stru;
#endif

/* ȫ�ֱ������� */
extern frw_task_stru g_st_event_task[];

/* �������� */
uint32_t frw_task_init(void);
void frw_task_exit(void);
void frw_task_event_handler_register (void (*p_func)(oal_uint));
void frw_task_set_state(uint32_t ul_core_id, uint8_t uc_task_state);
uint8_t frw_task_get_state(uint32_t ul_core_id);
void frw_task_sched(uint32_t ul_core_id);

#endif /* end of frw_task.h */
