

#ifndef __FRW_EVENT_SCHED_H__
#define __FRW_EVENT_SCHED_H__

/* ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "frw_main.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_FRW_EVENT_SCHED_H

/* �궨�� */
/* ��ȡ�¼��������Ѿ�������¼����� */
#define frw_event_queue_get_pending_events_num(_pst_event_queue) oal_queue_get_length(&(_pst_event_queue)->st_queue)

/*
 * ö��˵��: �¼����ж�Ӧ�ĵ��Ȳ���
 */
typedef enum {
    FRW_SCHED_POLICY_HI = 0,     /* �����ȼ����� */
    FRW_SCHED_POLICY_NORMAL = 1, /* ��ͨ���ȼ����� */

    FRW_SCHED_POLICY_BUTT
} frw_sched_policy_enum;

/*
 * ö��˵��: �¼�����״̬
 */
typedef enum {
    FRW_EVENT_QUEUE_STATE_INACTIVE = 0, /* �¼����в���Ծ(���ɱ�����) */
    FRW_EVENT_QUEUE_STATE_ACTIVE,       /* �¼����л�Ծ(�ɱ�����) */

    FRW_EVENT_QUEUE_STATE_BUTT
} frw_event_queue_state_enum;

/*
 * ö��˵��: �¼���������
 */
typedef enum {
    FRW_VAP_STATE_RESUME = 0, /* VAP�ָ� */
    FRW_VAP_STATE_PAUSE,      /* VAP��ͣ */

    FRW_VAP_STATE_BUTT
} frw_vap_state_enum;

/*
 * �ṹ˵��: �ɵ��ȶ���
 */
typedef struct {
    oal_spin_lock_stru st_lock;
    uint32_t ul_total_weight_cnt; /* �ɵ��ȶ����ϵ���Ȩ�ؼ����� */
    oal_dlist_head_stru st_head;    /* �ɵ��ȶ��е��¼�����ͷ */
} frw_event_sched_queue_stru;

/*
 * �ṹ˵��: �¼����нṹ��
 */
typedef struct tag_frw_event_queue_stru {
    oal_spin_lock_stru st_lock;
    oal_queue_stru st_queue;                   /* ���� */
    uint8_t en_state; /* ����״̬ */
    uint8_t en_policy;     /* ���е��Ȳ���(�����ȼ�����ͨ���ȼ�) */

    uint8_t uc_weight;     /* WRRȨ������ֵ */
    uint8_t uc_weight_cnt; /* WRRȨ�ؼ����� */

    uint8_t en_vap_state; /* VAP��״ֵ̬��0Ϊ�ָ���1Ϊ��ͣ */
    uint8_t auc_resv[3]; /* ����oal_uint8���飬����Ϊ3 */

    oal_dlist_head_stru st_list;
} frw_event_queue_stru;

/* �������� */
uint32_t frw_event_sched_init(frw_event_sched_queue_stru *pst_sched_queue);
uint32_t frw_event_queue_init(frw_event_queue_stru *pst_event_queue, uint8_t uc_weight, uint8_t en_policy,
                              uint8_t en_state, uint8_t uc_max_events);
void frw_event_queue_destroy(frw_event_queue_stru *pst_event_queue);
void *frw_event_sched_pick_next_event_queue_wrr(frw_event_sched_queue_stru *pst_sched_queue);
uint32_t frw_event_sched_deactivate_queue(frw_event_sched_queue_stru *pst_sched_queue,
                                          frw_event_queue_stru *pst_event_queue);

uint32_t frw_event_sched_activate_queue(frw_event_sched_queue_stru *pst_sched_queue,
                                        frw_event_queue_stru *pst_event_queue);

void frw_event_sched_pause_queue(frw_event_sched_queue_stru *pst_sched_queue, frw_event_queue_stru *pst_event_queue);
void frw_event_sched_resume_queue(frw_event_sched_queue_stru *pst_sched_queue, frw_event_queue_stru *pst_event_queue);

/*
 * �� �� ��  : frw_event_sched_reset_weight
 * ��������  : ���õ��ȶ����ϸ����¼����е�Ȩ�ؼ�����
 * �������  : pst_sched_queue: ���ȶ���ָ��
 */
OAL_STATIC OAL_INLINE void frw_event_sched_reset_weight(frw_event_sched_queue_stru *pst_sched_queue)
{
    oal_dlist_head_stru *pst_list = NULL;
    frw_event_queue_stru *pst_event_queue = NULL;

    if (oal_unlikely(pst_sched_queue == NULL)) {
        oal_warn_on(1);
        return;
    }

    /* ���������������� */
    oal_dlist_search_for_each(pst_list, &pst_sched_queue->st_head)
    {
        /* ��ȡ���������е�һ���¼����� */
        pst_event_queue = oal_dlist_get_entry(pst_list, frw_event_queue_stru, st_list);
        /* ֻ�����ûָ�״̬VAP��Ȩ��ֵ */
        if (pst_event_queue->en_vap_state == FRW_VAP_STATE_RESUME) {
            /* �����¼����е�Ȩ�ؼ����� */
            pst_event_queue->uc_weight_cnt = pst_event_queue->uc_weight;

            /* ���µ��ȶ����ϵ���Ȩ�ؼ����� */
            pst_sched_queue->ul_total_weight_cnt += pst_event_queue->uc_weight;
        }
    }
}

/*
 * �� �� ��  : frw_event_schedule
 * ��������  : �¼�������ں���
 * �������  : pst_sched_queue: ���ȶ���ָ��
 */
OAL_STATIC OAL_INLINE void *frw_event_schedule(frw_event_sched_queue_stru *pst_sched_queue)
{
    void *p_event = NULL;
    frw_event_sched_queue_stru *pst_queue = NULL;

    if (oal_unlikely(pst_sched_queue == NULL)) {
        oal_warn_on(1);
        return NULL;
    }

    /* ����ȫ�������� */
    pst_queue = &pst_sched_queue[FRW_SCHED_POLICY_HI];

    /* ��������ȼ����ȶ���Ϊ�գ���ȡ��һ�����ȶ��� */
    if (!oal_dlist_is_empty(&pst_queue->st_head)) {
        /* �ӵ���������ѡ��һ����������¼� */
        p_event = frw_event_sched_pick_next_event_queue_wrr(pst_queue);
        if (p_event != NULL) {
            return p_event;
        }
    }

    pst_queue = &pst_sched_queue[FRW_SCHED_POLICY_NORMAL];

    /* �����ͨ���ȼ����ȶ���Ϊ�գ���ȡ��һ�����ȶ��� */
    if (!oal_dlist_is_empty(&pst_queue->st_head)) {
        /* �ӵ���������ѡ��һ����������¼� */
        p_event = frw_event_sched_pick_next_event_queue_wrr(pst_queue);
        if (p_event != NULL) {
            return p_event;
        }
    }

    return NULL;
}

#endif /* end of frw_event_sched.h */
