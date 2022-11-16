

/* ͷ�ļ����� */
#include "frw_timer.h"
#include "frw_main.h"
#include "frw_task.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_FRW_TIMER_C

/* ȫ�ֱ������� */
OAL_STATIC oal_dlist_head_stru g_ast_timer_list[WLAN_FRW_MAX_NUM_CORES];
OAL_STATIC oal_spin_lock_stru g_timer_list_spinlock[WLAN_FRW_MAX_NUM_CORES]; /*lint !e88 */
OAL_STATIC oal_timer_list_stru g_timer[WLAN_FRW_MAX_NUM_CORES];
OAL_STATIC oal_uint32 g_stop_timestamp = 0;
oal_uint32 g_restart_timestamp = 0;
oal_uint32 g_max_deep_sleep_time = 0;  // ��¼ƽ̨���˯��ʱ��
OAL_STATIC oal_uint32 g_need_restart = OAL_FALSE;
OAL_STATIC oal_uint32 g_frw_timer_start_stamp[WLAN_FRW_MAX_NUM_CORES] = {0};  // ά���źţ�������¼��һ�����ж϶�ʱ��������ʱ��

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)

oal_uint32 g_os_time = 0;
frw_timeout_track_stru g_timeout_track[FRW_TIMEOUT_TRACK_NUM];
oal_uint8 g_timeout_track_idx = 0;
#endif
/* ����ʵ�� */
OAL_STATIC OAL_INLINE oal_void __frw_timer_immediate_destroy_timer(oal_uint32 ul_file_id,
                                                                   oal_uint32 ul_line_num,
                                                                   frw_timeout_stru *pst_timeout);

/*
 * �� �� ��  : frw_timer_init
 * ��������  : ��ʼ����ʱ��
 */
oal_void frw_timer_init(oal_uint32 ul_delay, oal_timer_func p_func, oal_ulong arg)
{
    oal_uint32 ul_core_id;

    if (oal_unlikely(p_func == NULL)) {
        oal_warn_on(1);
        return;
    }

    for (ul_core_id = 0; ul_core_id < WLAN_FRW_MAX_NUM_CORES; ul_core_id++) {
        oal_dlist_init_head(&g_ast_timer_list[ul_core_id]);
        oal_spin_lock_init(&g_timer_list_spinlock[ul_core_id]);

        oal_timer_init(&g_timer[ul_core_id], ul_delay, p_func, arg);
        g_frw_timer_start_stamp[ul_core_id] = 0;
    }

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)
    memset_s(g_timeout_track, OAL_SIZEOF(frw_timeout_track_stru) * FRW_TIMEOUT_TRACK_NUM,
             0, OAL_SIZEOF(frw_timeout_track_stru) * FRW_TIMEOUT_TRACK_NUM);
#endif
}

/*
 * �� �� ��  : frw_timer_exit
 * ��������  : ��ʱ���˳�����
 */
oal_void frw_timer_exit(oal_void)
{
    oal_uint32 ul_core_id;

    for (ul_core_id = 0; ul_core_id < WLAN_FRW_MAX_NUM_CORES; ul_core_id++) {
        oal_timer_delete_sync(&g_timer[ul_core_id]);
        g_frw_timer_start_stamp[ul_core_id] = 0;
    }
}

/*
 * �� �� ��  : frw_timer_restart
 * ��������  : ����sys timer
 */
oal_void frw_timer_restart(oal_void)
{
    oal_uint32 ul_core_id;

    /* ������ʱ�� */
    if (g_need_restart == OAL_FALSE) {
        return;
    }

    for (ul_core_id = 0; ul_core_id < WLAN_FRW_MAX_NUM_CORES; ul_core_id++) {
        oal_timer_start(&g_timer[ul_core_id], FRW_TIMER_DEFAULT_TIME);
    }
    g_need_restart = OAL_FALSE;
}

/*
 * �� �� ��  : frw_timer_stop
 * ��������  : ֹͣsys timer
 */
oal_void frw_timer_stop(oal_void)
{
    oal_uint32 ul_core_id;

    /* stop frw sys timer,record the stop time for restart timer to recalculate timestamp */
    g_stop_timestamp = (oal_uint32)oal_time_get_stamp_ms();

    for (ul_core_id = 0; ul_core_id < WLAN_FRW_MAX_NUM_CORES; ul_core_id++) {
        oal_timer_delete(&g_timer[ul_core_id]);
        g_frw_timer_start_stamp[ul_core_id] = 0;
    }

    g_need_restart = OAL_TRUE;
}

/*
 * �� �� ��  : frw_timer_dump
 * ��������  : �����ж�ʱ��dump����
 */
OAL_STATIC oal_void frw_timer_dump(oal_uint32 ul_core_id)
{
    const oal_uint32 ul_dump_mem_size = 64;
    oal_dlist_head_stru *pst_timeout_entry = NULL;
    frw_timeout_stru *pst_timeout_element = NULL;

    pst_timeout_entry = g_ast_timer_list[ul_core_id].pst_next;
    while (pst_timeout_entry != &g_ast_timer_list[ul_core_id]) {
        if (pst_timeout_entry == OAL_PTR_NULL) {
            OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_dump:: time broken break}");
            break;
        }

        oam_error_log2(0, OAM_SF_FRW, "{frw_timer_dump:: pst_timeout_entry[0x%x], g_ast_timer_list addr[0x%x]!}",
            (uintptr_t)pst_timeout_entry, (uintptr_t)g_ast_timer_list);
        if (pst_timeout_entry->pst_next == NULL) {
            /* If next is null,
             the pst_timeout_entry stru maybe released or memset */
            OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_dump:: pst_next is null,dump mem}");
            oal_print_hex_dump(((oal_uint8 *)pst_timeout_entry) - ul_dump_mem_size, ul_dump_mem_size * 2,
                               HEX_DUMP_GROUP_SIZE, "timer broken: "); /* dumpǰ��64�ֽڳ����ڴ� */
        }

        pst_timeout_element = oal_dlist_get_entry(pst_timeout_entry, frw_timeout_stru, st_entry);
        pst_timeout_entry = pst_timeout_entry->pst_next;
        oam_error_log3(0, OAM_SF_FRW, "{frw_timer_dump:: time_stamp[0x%x] timeout[%d]  enabled[%d]}",
                       pst_timeout_element->ul_time_stamp,
                       pst_timeout_element->ul_timeout,
                       pst_timeout_element->en_is_enabled);
        oam_error_log3(0, OAM_SF_FRW, "{frw_timer_dump:: module_id[%d] file_id[%d] line_num[%d]}",
                       pst_timeout_element->en_module_id,
                       pst_timeout_element->ul_file_id,
                       pst_timeout_element->ul_line_num);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef CONFIG_PRINTK
        if (pst_timeout_element->p_func) {
            printk(KERN_ERR "frw_timer_dump func : %pF\n", pst_timeout_element->p_func);
        }
#endif
#endif
    }
}

/*
 * �� �� ��  : frw_timer_timeout_proc
 * ��������  : ����timer����ִ�е��ڳ�ʱ����
 */
oal_uint32 frw_timer_timeout_proc(frw_event_mem_stru *pst_timeout_event)
{
    oal_dlist_head_stru *pst_timeout_entry = NULL;
    frw_timeout_stru *pst_timeout_element = NULL;
    oal_uint32 ul_present_time;
    oal_uint32 ul_end_time;
    oal_uint32 ul_runtime;
    oal_uint32 ul_core_id;
    oal_uint32 ul_runtime_func_start = 0;
    oal_uint32 ul_runtime_func_end = 0;
    oal_uint32 ul_endtime_func = 0;
    oal_uint32 ul_frw_timer_start;

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)
    oal_uint32 ul_sum_time = 0;
    oal_uint32 ul_start_time = 0;

    if (g_en_event_track_switch == OAL_TRUE) {
        memset_s(&g_timeout_track[g_timeout_track_idx], OAL_SIZEOF(frw_timeout_track_stru),
                 0, OAL_SIZEOF(frw_timeout_track_stru));
    }
#endif

    if (oal_unlikely(pst_timeout_event == OAL_PTR_NULL)) {
        OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_timeout_proc:: pst_timeout_event is null ptr!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_present_time = (oal_uint32)oal_time_get_stamp_ms();
    ul_core_id = oal_get_core_id();

    /* ִ�г�ʱ��ʱ�� */
    oal_spin_lock_bh(&g_timer_list_spinlock[ul_core_id]);
    pst_timeout_entry = g_ast_timer_list[ul_core_id].pst_next;

    while (pst_timeout_entry != &g_ast_timer_list[ul_core_id]) {
        if (pst_timeout_entry == OAL_PTR_NULL) {
            OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_timeout_proc:: the timer list is broken! }");
            frw_timer_dump(ul_core_id);
            break;  //lint !e527
        }

        pst_timeout_element = oal_dlist_get_entry(pst_timeout_entry, frw_timeout_stru, st_entry);
        pst_timeout_element->ul_curr_time_stamp = ul_present_time;

        /* һ����ʱ����ʱ�������д����µĶ�ʱ���������ʱ����ʱ������Ӧ�Ķ�ʱ������ɾ����ȡ��en_is_deleting���; */
        if (frw_time_after(ul_present_time, pst_timeout_element->ul_time_stamp)) {
            /* ɾ����ʱ��ʱ������������ڶ�ʱ������������ӽ�ȥ:delete first,then add periodic_timer */
            pst_timeout_element->en_is_registerd = OAL_FALSE;
            oal_dlist_delete_entry(&pst_timeout_element->st_entry);

            if ((pst_timeout_element->en_is_periodic == OAL_TRUE) ||
                (pst_timeout_element->en_is_enabled == OAL_FALSE)) {
                pst_timeout_element->ul_time_stamp = ul_present_time + pst_timeout_element->ul_timeout;
                pst_timeout_element->en_is_registerd = OAL_TRUE;
                frw_timer_add_timer(pst_timeout_element);
            }

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)
            ul_start_time = oal_5115timer_get_10ns();
#endif
            ul_runtime_func_start = (oal_uint32)oal_time_get_stamp_ms();
            if (pst_timeout_element->en_is_enabled) {
                oal_spin_unlock_bh(&g_timer_list_spinlock[ul_core_id]);
                pst_timeout_element->p_func(pst_timeout_element->p_timeout_arg);
                oal_spin_lock_bh(&g_timer_list_spinlock[ul_core_id]);
            }

            ul_endtime_func = (oal_uint32)oal_time_get_stamp_ms();
            ul_runtime_func_end = (oal_uint32)oal_time_get_runtime(ul_runtime_func_start, ul_endtime_func);
            if (ul_runtime_func_end >= (oal_uint32)oal_jiffies_to_msecs(2)) {
                oam_warning_log4(0, OAM_SF_FRW,
                                 "{frw_timer_timeout_proc:: fileid=%u, linenum=%u, moduleid=%u, runtime=%u}",
                                 pst_timeout_element->ul_file_id,
                                 pst_timeout_element->ul_line_num,
                                 pst_timeout_element->en_module_id,
                                 ul_runtime_func_end);
            }

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)

            if (g_en_event_track_switch == OAL_TRUE) {
                g_timeout_track[g_timeout_track_idx].st_timer_track[g_timeout_track[g_timeout_track_idx].uc_timer_cnt].ul_file_id = pst_timeout_element->ul_file_id;
                g_timeout_track[g_timeout_track_idx].st_timer_track[g_timeout_track[g_timeout_track_idx].uc_timer_cnt].ul_line_num = pst_timeout_element->ul_line_num;
                g_timeout_track[g_timeout_track_idx].st_timer_track[g_timeout_track[g_timeout_track_idx].uc_timer_cnt].ul_execute_time = ul_start_time - oal_5115timer_get_10ns();
                ul_sum_time += g_timeout_track[g_timeout_track_idx].st_timer_track[g_timeout_track[g_timeout_track_idx].uc_timer_cnt].ul_execute_time;
            }
#endif
        } else {
            break;
        }

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)

        if (g_en_event_track_switch == OAL_TRUE) {
            oal_incr(g_timeout_track[g_timeout_track_idx].uc_timer_cnt, FRW_TIMER_TRACK_NUM);
        }
#endif

        pst_timeout_entry = g_ast_timer_list[ul_core_id].pst_next;
    }

    /* ����������С��ʱʱ�䣬������ʱ�� */
    if (oal_dlist_is_empty(&g_ast_timer_list[ul_core_id]) == OAL_FALSE) {
        pst_timeout_entry = g_ast_timer_list[ul_core_id].pst_next;
        pst_timeout_element = oal_dlist_get_entry(pst_timeout_entry, frw_timeout_stru, st_entry);
        ul_present_time = (oal_uint32)oal_time_get_stamp_ms();
        if (frw_time_after(pst_timeout_element->ul_time_stamp, ul_present_time)) {
            ul_frw_timer_start = (oal_uint32)oal_time_get_runtime(ul_present_time, pst_timeout_element->ul_time_stamp);

            g_frw_timer_start_stamp[ul_core_id] = pst_timeout_element->ul_time_stamp;
        } else {
            ul_frw_timer_start = FRW_TIMER_DEFAULT_TIME;

            g_frw_timer_start_stamp[ul_core_id] = (ul_present_time + FRW_TIMER_DEFAULT_TIME);
        }

        oal_timer_start(&g_timer[ul_core_id], ul_frw_timer_start);
    } else {
        g_frw_timer_start_stamp[ul_core_id] = 0;
    }

    oal_spin_unlock_bh(&g_timer_list_spinlock[ul_core_id]);

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)
    if (g_en_event_track_switch == OAL_TRUE) {
        if ((ul_sum_time * 10) > 300000) {
            oal_incr(g_timeout_track_idx, FRW_TIMEOUT_TRACK_NUM);
        }
    }
#endif

    ul_end_time = (oal_uint32)oal_time_get_stamp_ms();
    ul_runtime = (oal_uint32)oal_time_get_runtime(ul_present_time, ul_end_time);
    /* ͬdevice������־ʱ��һ�� */
    if (ul_runtime > (oal_uint32)oal_jiffies_to_msecs(2)) {
        OAM_WARNING_LOG1(0, OAM_SF_FRW,
                         "{frw_timer_timeout_proc:: timeout process exucte time too long time[%d]}",
                         ul_runtime);
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : frw_timer_add_in_order
 * ��������  : �������а���С�����˳�����ڵ�
 * �������  : pst_new: Ҫ������½ڵ�
 *             pst_head: ����ͷָ��
 */
OAL_STATIC oal_void frw_timer_add_in_order(oal_dlist_head_stru *pst_new, oal_dlist_head_stru *pst_head)
{
    oal_dlist_head_stru *pst_timeout_entry = NULL;
    frw_timeout_stru *pst_timeout_element = NULL;
    frw_timeout_stru *pst_timeout_element_new;
    oal_uint32 ul_core_id;

    pst_timeout_element_new = oal_dlist_get_entry(pst_new, frw_timeout_stru, st_entry);

    ul_core_id = oal_get_core_id();

    /* �����������ҵ�һ����pst_timeout_element_new->ul_time_stamp���λ�� */
    if (pst_head != NULL) {
        pst_timeout_entry = pst_head->pst_next;

        while (pst_timeout_entry != pst_head) {
            if (pst_timeout_entry == OAL_PTR_NULL) {
                OAM_ERROR_LOG0(0, OAM_SF_FRW, "{Driver frw_timer_add_in_order:: the timer list is broken! }");
                oam_error_log3(0, OAM_SF_FRW,
                               "{new frw_timer_add_in_order:: time_stamp[0x%x] timeout[%d]  enabled[%d]}",
                               pst_timeout_element_new->ul_time_stamp,
                               pst_timeout_element_new->ul_timeout,
                               pst_timeout_element_new->en_is_enabled);
                oam_error_log3(0, OAM_SF_FRW,
                               "{new frw_timer_add_in_order:: module_id[%d] file_id[%d] line_num[%d]}",
                               pst_timeout_element_new->en_module_id,
                               pst_timeout_element_new->ul_file_id,
                               pst_timeout_element_new->ul_line_num);
                frw_timer_dump(ul_core_id);
                break;
            }

            pst_timeout_element = oal_dlist_get_entry(pst_timeout_entry, frw_timeout_stru, st_entry);
            if (frw_time_after(pst_timeout_element->ul_time_stamp, pst_timeout_element_new->ul_time_stamp)) {
                break;
            }

            pst_timeout_entry = pst_timeout_entry->pst_next;
        }

        if ((pst_timeout_entry != NULL) && (pst_timeout_entry->pst_prev != NULL)) {
            oal_dlist_add(pst_new, pst_timeout_entry->pst_prev, pst_timeout_entry);
        } else {
            OAM_ERROR_LOG0(0, OAM_SF_FRW, "{Driver frw_timer_add_in_order::timer list is broken !}");
            frw_timer_dump(ul_core_id);
#ifdef _PRE_WLAN_REPORT_WIFI_ABNORMAL
            oal_report_wifi_abnormal(OAL_ABNORMAL_FRW_TIMER_BROKEN, OAL_ACTION_REBOOT, 0, 0);
#endif
        }
    }
}

/*
 * �� �� ��  : frw_timer_add_timer
 * ��������  : ɾ����ʱ��
 */
oal_void frw_timer_add_timer(frw_timeout_stru *pst_timeout)
{
    oal_int32 l_val;

    if (pst_timeout == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_add_timer:: OAL_PTR_NULL == pst_timeout}");
        return;
    }

    if (oal_dlist_is_empty(&g_ast_timer_list[pst_timeout->ul_core_id]) == OAL_TRUE) {
        g_frw_timer_start_stamp[pst_timeout->ul_core_id] = 0;
    }

    /* ��Frw�����������Ϊ���� */
    frw_timer_add_in_order(&pst_timeout->st_entry, &g_ast_timer_list[pst_timeout->ul_core_id]);

    l_val = frw_time_after(g_frw_timer_start_stamp[pst_timeout->ul_core_id], pst_timeout->ul_time_stamp);
    if ((g_frw_timer_start_stamp[pst_timeout->ul_core_id] == 0) || (l_val > 0)) {
        oal_timer_start(&g_timer[pst_timeout->ul_core_id], pst_timeout->ul_timeout);
        g_frw_timer_start_stamp[pst_timeout->ul_core_id] = pst_timeout->ul_time_stamp;
    }

    return;
}

/*
 * �� �� ��  : frw_timer_create_timer
 * ��������  : ������ʱ��
 * �������  : en_is_periodic: �ö�ʱ���Ƿ���Ҫ����ѭ��
 *             en_module_id: ģ�� IDά����
 *             p_timeout_arg :��ʱ����ʱ��������Ҫ�����
 *             us_timeout: ��ʱʱ��
 */
oal_void frw_timer_create_timer(oal_uint32 ul_file_id,
                                oal_uint32 ul_line_num,
                                frw_timeout_stru *pst_timeout,
                                frw_timeout_func p_timeout_func,
                                oal_uint32 ul_timeout,
                                oal_void *p_timeout_arg,
                                oal_bool_enum_uint8 en_is_periodic,
                                oam_module_id_enum_uint16 en_module_id,
                                oal_uint32 ul_core_id)
{
    if (pst_timeout == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_create_timer:: OAL_PTR_NULL == pst_timeout}");
        return;
    }

    if (ul_timeout == 0 && en_is_periodic == OAL_TRUE) {
        oam_error_log4(0, OAM_SF_FRW,
                       "{frw_timer_create_timer::timeout value invalid! fileid: %d, line: %d, module: %d, core: %d}",
                       ul_file_id, ul_line_num, en_module_id, ul_core_id);
        return;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    pst_timeout->ul_core_id = 0;
#else
    pst_timeout->ul_core_id = ul_core_id;
#endif

    oal_spin_lock_bh(&g_timer_list_spinlock[pst_timeout->ul_core_id]);

    pst_timeout->p_func = p_timeout_func;
    pst_timeout->p_timeout_arg = p_timeout_arg;
    pst_timeout->ul_timeout = ul_timeout;
    pst_timeout->ul_time_stamp = (oal_uint32)oal_time_get_stamp_ms() + ul_timeout;
    pst_timeout->en_is_periodic = en_is_periodic;
    pst_timeout->en_module_id = en_module_id;
    pst_timeout->ul_file_id = ul_file_id;
    pst_timeout->ul_line_num = ul_line_num;
    pst_timeout->en_is_enabled = OAL_TRUE; /* Ĭ��ʹ�� */

    if (pst_timeout->en_is_registerd != OAL_TRUE) {
        pst_timeout->en_is_registerd = OAL_TRUE; /* Ĭ��ע�� */
        frw_timer_add_timer(pst_timeout);
    } else {
        oal_dlist_delete_entry(&pst_timeout->st_entry);
        frw_timer_add_timer(pst_timeout);
    }

    oal_spin_unlock_bh(&g_timer_list_spinlock[pst_timeout->ul_core_id]);

    return;
}

/*
 * �� �� ��  : __frw_timer_immediate_destroy_timer
 * ��������  : ����ɾ����ʱ��������
 */
OAL_STATIC OAL_INLINE oal_void __frw_timer_immediate_destroy_timer(oal_uint32 ul_file_id,
                                                                   oal_uint32 ul_line_num,
                                                                   frw_timeout_stru *pst_timeout)
{
    if (pst_timeout->st_entry.pst_prev == OAL_PTR_NULL || pst_timeout->st_entry.pst_next == OAL_PTR_NULL) {
        return;
    }

    if (pst_timeout->en_is_registerd == OAL_FALSE) {
        oam_warning_log0(0, OAM_SF_FRW,
                         "{frw_timer_immediate_destroy_timer::This timer is not enabled it should not be deleted}");

        return;
    }

    pst_timeout->en_is_enabled = OAL_FALSE;
    pst_timeout->en_is_registerd = OAL_FALSE;

    oal_dlist_delete_entry(&pst_timeout->st_entry);

    if (oal_dlist_is_empty(&g_ast_timer_list[pst_timeout->ul_core_id]) == OAL_TRUE) {
        g_frw_timer_start_stamp[pst_timeout->ul_core_id] = 0;
    }
}

/*
 * �� �� ��  : frw_timer_immediate_destroy_timer
 * ��������  : ����ɾ����ʱ��
 */
oal_void frw_timer_immediate_destroy_timer(oal_uint32 ul_file_id,
                                           oal_uint32 ul_line_num,
                                           frw_timeout_stru *pst_timeout)
{
    oal_spin_lock_bh(&g_timer_list_spinlock[pst_timeout->ul_core_id]);
    __frw_timer_immediate_destroy_timer(ul_file_id, ul_line_num, pst_timeout);
    oal_spin_unlock_bh(&g_timer_list_spinlock[pst_timeout->ul_core_id]);
}

/*
 * �� �� ��  : frw_timer_restart_timer
 * ��������  : ������ʱ��
 */
oal_void frw_timer_restart_timer(frw_timeout_stru *pst_timeout, oal_uint32 ul_timeout,
                                 oal_bool_enum_uint8 en_is_periodic)
{
    if (pst_timeout == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_restart_timer:: OAL_PTR_NULL == pst_timeout}");
        return;
    }
    /* ɾ����ǰ��ʱ�� */
    if (pst_timeout->st_entry.pst_prev == OAL_PTR_NULL || pst_timeout->st_entry.pst_next == OAL_PTR_NULL) {
        oam_error_log4(0, OAM_SF_FRW,
                       "{frw_timer_restart_timer::This timer has been deleted!file_id=%d,line=%d,core=%d,mod=%d}",
                       pst_timeout->ul_file_id, pst_timeout->ul_line_num,
                       pst_timeout->ul_core_id, pst_timeout->en_module_id);
        return;
    }

    if (pst_timeout->en_is_registerd == OAL_FALSE) {
        oam_error_log4(0, OAM_SF_FRW,
                       "{frw_timer_restart_timer::This timer is not registerd!file_id=%d,line=%d,core=%d,mod=%d}",
                       pst_timeout->ul_file_id, pst_timeout->ul_line_num,
                       pst_timeout->ul_core_id, pst_timeout->en_module_id);
        return;
    }
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if (pst_timeout->ul_core_id != 0) {
        oam_error_log4(0, OAM_SF_FRW,
            "{frw_timer_restart_timer::ul_core_id is not 0!file_id=%d,line=%d,core=%d,mod=%d}",
            pst_timeout->ul_file_id, pst_timeout->ul_line_num,
            pst_timeout->ul_core_id, pst_timeout->en_module_id);
        pst_timeout->ul_core_id = 0;
    }
#endif
    oal_spin_lock_bh(&g_timer_list_spinlock[pst_timeout->ul_core_id]);
    oal_dlist_delete_entry(&pst_timeout->st_entry);

    pst_timeout->ul_time_stamp = (oal_uint32)oal_time_get_stamp_ms() + ul_timeout;
    pst_timeout->ul_timeout = ul_timeout;
    pst_timeout->en_is_periodic = en_is_periodic;
    pst_timeout->en_is_enabled = OAL_TRUE;

    frw_timer_add_timer(pst_timeout);
    oal_spin_unlock_bh(&g_timer_list_spinlock[pst_timeout->ul_core_id]);
}

/*
 * �� �� ��  : frw_timer_stop_timer
 * ��������  : ֹͣ��ʱ��
 */
oal_void frw_timer_stop_timer(frw_timeout_stru *pst_timeout)
{
    if (pst_timeout == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_stop_timer:: OAL_PTR_NULL == pst_timeout}");
        return;
    }

    pst_timeout->en_is_enabled = OAL_FALSE;
}

oal_uint8 g_timer_pause = OAL_FALSE;
#if defined(_PRE_FRW_TIMER_BIND_CPU) && defined(CONFIG_NR_CPUS)
oal_uint32 g_frw_timer_cpu_count[CONFIG_NR_CPUS] = {0};
#endif

/*
 * �� �� ��  : frw_timer_timeout_proc_event
 * ��������  : 15���붨ʱ�жϴ���
 */
oal_void frw_timer_timeout_proc_event(oal_ulong arg)
{
    frw_event_mem_stru *pst_event_mem = NULL;
    frw_event_stru *pst_event = NULL;
    oal_uint32 ul_core_id = 0;

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)
    oal_uint32 ul_time = 0;

    if (g_en_event_track_switch == OAL_TRUE) {
        if (g_os_time == 0) {
            g_os_time = oal_5115timer_get_10ns();
        } else {
            ul_time = oal_5115timer_get_10ns();
            g_timeout_track[g_timeout_track_idx].ul_os_timer_interval = g_os_time - ul_time;
            g_os_time = ul_time;
        }
    }
#endif

#if defined(_PRE_FRW_TIMER_BIND_CPU) && defined(CONFIG_NR_CPUS)
    do {
        oal_uint32 cpu_id = smp_processor_id();
        if (cpu_id < CONFIG_NR_CPUS) {
            g_frw_timer_cpu_count[cpu_id]++;
        }
    } while (0);
#endif

    if (g_timer_pause == OAL_TRUE) {
        return;
    }

    /*lint -e539*/ /*lint -e830*/
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
    for (ul_core_id = 0; ul_core_id < WLAN_FRW_MAX_NUM_CORES; ul_core_id++) {
        if (frw_task_get_state(ul_core_id)) {
#endif
            pst_event_mem = frw_event_alloc_m(OAL_SIZEOF(frw_event_stru));
            /* ����ֵ��� */
            if (oal_unlikely(pst_event_mem == OAL_PTR_NULL)) {
                /* ������ʱ�� */
#ifdef _PRE_FRW_TIMER_BIND_CPU
                oal_timer_start_on(&g_timer[ul_core_id], FRW_TIMER_DEFAULT_TIME, 0);
#else
                oal_timer_start(&g_timer[ul_core_id], FRW_TIMER_DEFAULT_TIME);
#endif
                OAM_ERROR_LOG0(0, OAM_SF_FRW, "{frw_timer_timeout_proc_event:: frw_event_alloc failed!}");
                return;
            }

            pst_event = frw_get_event_stru(pst_event_mem);

            /* ����¼�ͷ */
            frw_field_setup((&pst_event->st_event_hdr), en_type, (FRW_EVENT_TYPE_TIMEOUT));
            frw_field_setup((&pst_event->st_event_hdr), uc_sub_type, (FRW_TIMEOUT_TIMER_EVENT));
            frw_field_setup((&pst_event->st_event_hdr), us_length, (WLAN_MEM_EVENT_SIZE1));
            frw_field_setup((&pst_event->st_event_hdr), en_pipeline, (FRW_EVENT_PIPELINE_STAGE_0));
            frw_field_setup((&pst_event->st_event_hdr), uc_chip_id, (0));
            frw_field_setup((&pst_event->st_event_hdr), uc_device_id, (0));
            frw_field_setup((&pst_event->st_event_hdr), uc_vap_id, (0));

            /* ���¼� */
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
            frw_event_post_event(pst_event_mem, ul_core_id);
#else
            frw_event_dispatch_event(pst_event_mem);
#endif
            frw_event_free_m(pst_event_mem);
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
        }
    }
#endif
/*lint +e539*/ /*lint +e830*/
}

/*
 * �� �� ��  : frw_timer_delete_all_timer
 * ��������  : ����ʱ�������е����ж�ʱ��ɾ���������쳣ʱ�ⲿģ������ʱ��
 *             �����ڶ�ʱ���ص������е���
 */
oal_void frw_timer_delete_all_timer(oal_void)
{
    oal_dlist_head_stru *pst_timeout_entry = NULL;
    frw_timeout_stru *pst_timeout_element = NULL;

    oal_uint32 ul_core_id;

    for (ul_core_id = 0; ul_core_id < WLAN_FRW_MAX_NUM_CORES; ul_core_id++) {
        oal_spin_lock_bh(&g_timer_list_spinlock[ul_core_id]);
        /* ɾ�����д�ɾ����ʱ�� */
        pst_timeout_entry = g_ast_timer_list[ul_core_id].pst_next;

        while (pst_timeout_entry != &g_ast_timer_list[ul_core_id]) {
            pst_timeout_element = oal_dlist_get_entry(pst_timeout_entry, frw_timeout_stru, st_entry);

            pst_timeout_entry = pst_timeout_entry->pst_next;

            /* ɾ����ʱ�� */
            oal_dlist_delete_entry(&pst_timeout_element->st_entry);
        }

        g_frw_timer_start_stamp[ul_core_id] = 0;
        oal_spin_unlock_bh(&g_timer_list_spinlock[ul_core_id]);
    }
}

/*
 * �� �� ��  : frw_timer_dump_timer
 * ��������  : ��ӡ����timer��ά����Ϣ
 */
oal_void frw_timer_dump_timer(oal_uint32 ul_core_id)
{
    oal_dlist_head_stru *pst_dlist_entry = NULL;
    frw_timeout_stru *pst_timer = NULL;
    oal_uint32 ul_cnt = 0;

    oam_warning_log0(0, OAM_SF_ANY, "frw_timer_dump_timer::timer dump start.");

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "frw_timer_dump_timer::g_ul_frw_timer_start_stamp [%u]",
                     g_frw_timer_start_stamp[ul_core_id]);

    oal_dlist_search_for_each(pst_dlist_entry, &g_ast_timer_list[ul_core_id])
    {
        pst_timer = oal_dlist_get_entry(pst_dlist_entry, frw_timeout_stru, st_entry);

        oam_warning_log4(0, OAM_SF_ANY, "Driver:TIMER NO.%d, file id[%d], line num[%d], func addr[0x%08x]",
                         ul_cnt, pst_timer->ul_file_id, pst_timer->ul_line_num, (uintptr_t)pst_timer->p_func);
        oam_warning_log3(0, OAM_SF_ANY, "Driver:timer enabled[%d], registerd[%d], period[%d] ",
                         pst_timer->en_is_enabled, pst_timer->en_is_registerd, pst_timer->en_is_periodic);
        oam_warning_log4(0, OAM_SF_ANY, "Driver:core id[%d], timeout[%u], time stamp[%u], curr time stamp[%u]",
                         ul_core_id, pst_timer->ul_timeout, pst_timer->ul_time_stamp, pst_timer->ul_curr_time_stamp);
        ul_cnt++;
    }
    oam_warning_log0(0, OAM_SF_ANY, "frw_timer_dump_timer::timer dump end.");
}
/*
 * �� �� ��  : frw_timer_clean_timer
 * ��������  : ɾ��ָ��ģ����������ж�ʱ��
 *             ���������ܽ��������ʱ�����������⣬һ�������в�������Ҫ���д���
 * �������  : ģ��id
 */
oal_void frw_timer_clean_timer(oam_module_id_enum_uint16 en_module_id)
{
    oal_dlist_head_stru *pst_timeout_entry = NULL;
    frw_timeout_stru *pst_timeout_element = NULL;
    oal_uint32 ul_core_id;

    for (ul_core_id = 0; ul_core_id < WLAN_FRW_MAX_NUM_CORES; ul_core_id++) {
        oal_spin_lock_bh(&g_timer_list_spinlock[ul_core_id]);
        pst_timeout_entry = g_ast_timer_list[ul_core_id].pst_next;

        while (pst_timeout_entry != &g_ast_timer_list[ul_core_id]) {
            if (pst_timeout_entry == NULL) {
                OAL_IO_PRINT("!!!====TIMER LIST BROKEN====!!!\n");
                break;
            }

            pst_timeout_element = oal_dlist_get_entry(pst_timeout_entry, frw_timeout_stru, st_entry);
            pst_timeout_entry = pst_timeout_entry->pst_next;

            if (en_module_id == pst_timeout_element->en_module_id) {
                oal_dlist_delete_entry(&pst_timeout_element->st_entry);
            }
        }

        if (oal_dlist_is_empty(&g_ast_timer_list[ul_core_id])) {
            g_frw_timer_start_stamp[ul_core_id] = 0;
        }
        oal_spin_unlock_bh(&g_timer_list_spinlock[ul_core_id]);
    }
}

/*lint -e578*/ /*lint -e19*/
oal_module_symbol(frw_timer_restart_timer);
oal_module_symbol(frw_timer_create_timer);
oal_module_symbol(frw_timer_dump_timer);
oal_module_symbol(frw_timer_stop_timer);
oal_module_symbol(frw_timer_add_timer);
oal_module_symbol(frw_timer_immediate_destroy_timer);
oal_module_symbol(frw_timer_delete_all_timer);
oal_module_symbol(g_timer_pause);
oal_module_symbol(frw_timer_clean_timer);

#if defined(_PRE_DEBUG_MODE) && defined(_PRE_TIMEOUT_TRACK_DEBUG)
oal_module_symbol(g_timeout_track);
oal_module_symbol(g_timeout_track_idx);
#endif

oal_module_symbol(frw_timer_restart);
oal_module_symbol(frw_timer_stop);
