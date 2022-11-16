

#ifndef __FRW_EXT_IF_H__
#define __FRW_EXT_IF_H__

/* ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "oam_ext_if.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_FRW_EXT_IF_H

#define frw_timer_restart_timer_m(_pst_timeout, _timeout, _en_is_periodic) \
    frw_timer_restart_timer(_pst_timeout, _timeout, _en_is_periodic)
#define frw_timer_stop_timer_m(_pst_timeout) \
    frw_timer_stop_timer(_pst_timeout)
#define frw_timer_create_timer_m(_pst_timeout, _p_timeout_func, _timeout, \
                               _p_timeout_arg, _en_is_periodic, _en_module_id, _core_id) \
    frw_timer_create_timer(THIS_FILE_ID, __LINE__, _pst_timeout, _p_timeout_func, _timeout, \
                               _p_timeout_arg, _en_is_periodic, _en_module_id, _core_id)
#define frw_timer_destroy_timer(_pst_timeout) \
    frw_timer_immediate_destroy_timer(THIS_FILE_ID, __LINE__, _pst_timeout)
#define frw_timer_immediate_destroy_timer_m(_pst_timeout) \
    frw_timer_immediate_destroy_timer(THIS_FILE_ID, __LINE__, _pst_timeout)

#define FRW_TIMER_TRACK_NUM   256
#define FRW_TIMEOUT_TRACK_NUM 256

#define FRW_PROCESS_MAX_EVENT 100

/* ö�ٶ��� */
/* ��ö�����ڶ����FRWģ�����ϸ�ģ��ĳ�ʼ��״̬ */
typedef enum {
    FRW_INIT_STATE_START,                /* ��ʾ��ʼ������������FRW��ʼ����ʼ */
    FRW_INIT_STATE_FRW_SUCC,             /* ��ʾFRWģ���ʼ���ɹ� */
    FRW_INIT_STATE_HAL_SUCC,             /* ��ʾHALģ���ʼ���ɹ� */
    FRW_INIT_STATE_DMAC_CONFIG_VAP_SUCC, /* ��ʾDMACģ��������ʼ���ɹ�������VAP������������ڴ�״̬֮���ٳ�ʼ������Ϊҵ��VAP�ĳ�ʼ�� */
    FRW_INIT_STATE_HMAC_CONFIG_VAP_SUCC, /* ��ʾHMACģ��������ʼ���ɹ�������VAP������������ڴ�״̬֮���ٳ�ʼ������Ϊҵ��VAP�ĳ�ʼ�� */
    FRW_INIT_STATE_ALL_SUCC,             /* ��״̬��ʾHMAC����ģ����ѳ�ʼ���ɹ� */

    FRW_INIT_STATE_BUTT
} frw_init_enum;

/*
 * ö����  : uint8_t
 * ö��˵��: �¼�����
 */
typedef enum {
    FRW_EVENT_TYPE_HIGH_PRIO = 0,    /* �����ȼ��¼����� */
    FRW_EVENT_TYPE_HOST_CRX,         /* ����Host�෢���������¼� */
    FRW_EVENT_TYPE_HOST_DRX,         /* ����Host�෢���������¼� */
    FRW_EVENT_TYPE_HOST_CTX,         /* ����HOST��������¼� */
    FRW_EVENT_TYPE_HOST_SDT_REG = 4, /* SDT��ȡ�Ĵ�����wifi�������ϱ�SDT */
    FRW_EVENT_TYPE_WLAN_CRX,         /* ����Wlan�෢���Ĺ���/����֡�¼� */
    FRW_EVENT_TYPE_WLAN_DRX,         /* ����Wlan�෢��������֡�¼� */
    FRW_EVENT_TYPE_WLAN_CTX,         /* ����/����֡������Wlan���¼� */
    FRW_EVENT_TYPE_WLAN_DTX,         /* ����֡������Wlan���¼� */
    FRW_EVENT_TYPE_WLAN_TX_COMP = 9, /* ��������¼� */
    FRW_EVENT_TYPE_TBTT,             /* TBTT�ж��¼� */
    FRW_EVENT_TYPE_TIMEOUT,          /* FRW��ͨ��ʱ�¼� */
    FRW_EVENT_TYPE_HMAC_MISC,        /* HMAC��ɢ�¼��������״����¼� */
    FRW_EVENT_TYPE_DMAC_MISC = 13,   /* DMAC��ɢ�¼� */
    FRW_EVENT_TYPE_HOST_DDR_DRX = 14, /* HOST DDR�¼� */

    FRW_EVENT_TYPE_BUTT
} frw_event_type_enum;

/* HCC�¼������Ͷ��� */
typedef enum {
    DMAC_HCC_TX_EVENT_SUB_TYPE,

    DMAC_HCC_TX_EVENT_SUB_TYPE_BUTT
} dmac_hcc_tx_event_sub_type_enum;

typedef enum {
    DMAC_HCC_RX_EVENT_SUB_TYPE,

    DMAC_HCC_RX_EVENT_SUB_TYPE_BUTT
} dmac_hcc_rx_event_sub_type_enum;

typedef struct {
    uint8_t en_nest_type; /* Ƕ�׵�ҵ���¼��������� */
    uint8_t uc_nest_sub_type;             /* Ƕ�׵�ҵ���¼��������� */
    void *pst_netbuf;                       /* ����������¼������Ӧ������ͷnetbuf�ĵ�ַ;�����¼������Ӧ��buff�׵�ַ */
    uint32_t buf_len;                  /* ����������¼������Ӧ��netbuf����;�����¼������Ӧ��buff len */
} hcc_event_stru;

/* record the data type by the hcc */
#pragma pack(push, 1)
/* 4B */
struct frw_hcc_extend_hdr {
    uint8_t en_nest_type;
    uint8_t uc_nest_sub_type;
    uint8_t chip_id : 2; /* this is not good */
    uint8_t device_id : 2;
    uint8_t vap_id : 4;
} __OAL_DECLARE_PACKED;
#pragma pack(pop)

typedef uint32_t (*frw_timeout_func)(void *);

typedef struct {
    void *p_timeout_arg;             /* ��ʱ��������� */
    frw_timeout_func p_func;             /* ��ʱ������ */
    uint32_t time_stamp;            /* ��ʱ������ʱ�� */
    uint32_t curr_time_stamp;       /* ��ʱ�����뵱ǰʱ�� */
    uint32_t timeout;               /* ���೤ʱ�䶨ʱ����ʱ */
    oal_bool_enum_uint8 en_is_registerd; /* ��ʱ���Ƿ��Ѿ�ע�� */
    oal_bool_enum_uint8 en_is_periodic;  /* ��ʱ���Ƿ�Ϊ���ڵ� */
    oal_bool_enum_uint8 en_is_enabled;   /* ��ʱ���Ƿ�ʹ�� */
    uint8_t uc_pad;
    oam_module_id_enum_uint16 en_module_id; /* ά����ģ��id */
    uint32_t core_id;                  /* �󶨵ĺ�id */
    uint32_t file_id;
    uint32_t line_num;
    oal_dlist_head_stru st_entry; /* �������������� */
} frw_timeout_stru;
/*
 * ö����  : uint8_t
 * ö��˵��: �¼��ֶκţ�ȡֵ[0, 1]
 */
typedef enum {
    FRW_EVENT_PIPELINE_STAGE_0 = 0,  // ���¼�
    FRW_EVENT_PIPELINE_STAGE_1,      // ֱ�ӵ���
    FRW_EVENT_PIPELINE_STAGE_BUTT
} frw_event_pipeline_enum;

#define FRW_RX_EVENT_TRACK_NUM 256
#define FRW_EVENT_TRACK_NUM    128

typedef oal_mem_stru frw_event_mem_stru; /* �¼��ṹ���ڴ��ת���� */

/*
 * �ṹ��  : frw_event_hdr_stru
 * �ṹ˵��: �¼�ͷ�ṹ��,
 * ��ע    : uc_length��ֵΪ(payload���� + �¼�ͷ���� - 2)
 */
typedef struct {
    uint8_t en_type;         /* �¼����� */
    uint8_t uc_sub_type;                     /* �¼������� */
    uint16_t us_length;                      /* �¼����峤�� */
    uint8_t en_pipeline; /* �¼��ֶκ� */
    uint8_t uc_chip_id;                      /* оƬID */
    uint8_t uc_device_id;                    /* �豸ID */
    uint8_t uc_vap_id;                       /* VAP ID */
} frw_event_hdr_stru;

/*
 * �ṹ��  : frw_event_stru
 * �ṹ˵��: �¼��ṹ��
 */
typedef struct {
    frw_event_hdr_stru st_event_hdr; /* �¼�ͷ */
    uint8_t auc_event_data[4];     /* payload 4data */
} frw_event_stru;

/*
 * �ṹ��  : frw_event_sub_table_item_stru
 * �ṹ˵��: �¼��ӱ�ṹ��
 */
typedef struct {
    uint32_t (*p_func)(frw_event_mem_stru *); /* (type, subtype, pipeline)���͵��¼���Ӧ�Ĵ����� */
    uint32_t (*p_tx_adapt_func)(frw_event_mem_stru *);
    frw_event_mem_stru *(*p_rx_adapt_func)(frw_event_mem_stru *);
} frw_event_sub_table_item_stru;

/*
 * �ṹ��  : frw_event_table_item_stru
 * �ṹ˵��: �¼���ṹ��
 */
typedef struct {
    frw_event_sub_table_item_stru *pst_sub_table; /* ָ���ӱ��ָ�� */
} frw_event_table_item_stru;

/*
 * �ṹ��  : frw_ipc_msg_header_stru
 * �ṹ˵��: IPC(�˼�ͨ��)ͷ�ṹ��
 */
typedef struct {
    uint16_t us_seq_number;  /* �˼���Ϣ��� */
    uint8_t uc_target_cpuid; /* Ŀ���cpuid frw_ipc_cpu_id_enum_uint8 */
    uint8_t uc_msg_type;     /* ��Ϣ���� uint8_t */
} frw_ipc_msg_header_stru;

typedef struct {
    uint32_t event_cnt;
    uint32_t event_time[FRW_EVENT_TRACK_NUM];
    uint16_t us_event_type[FRW_EVENT_TRACK_NUM];
    uint16_t us_event_sub_type[FRW_EVENT_TRACK_NUM];
} frw_event_track_time_stru;

#define FRW_IPC_MSG_HEADER_LENGTH (sizeof(frw_ipc_msg_header_stru))

/* �¼�ͷ���� */
#define FRW_EVENT_HDR_LEN sizeof(frw_event_hdr_stru)

/* �¼����������� */
#define FRW_EVENT_MAX_NUM_QUEUES (FRW_EVENT_TYPE_BUTT * WLAN_VAP_SUPPORT_MAX_NUM_LIMIT)

#define frw_field_setup(_p, _m, _v) ((_p)->_m = (_v))

/* �¼�ͷ�޸ĺ�(�޸��¼�ͷ�е�pipeline��subtype) */
#define frw_event_hdr_modify_pipeline_and_subtype(_pst_event_hdr, _uc_sub_type) \
    do {                                                                        \
        frw_field_setup((_pst_event_hdr), en_pipeline, 1);                      \
        frw_field_setup((_pst_event_hdr), uc_sub_type, (_uc_sub_type));         \
    } while (0)

#define frw_event_hdr_modify_pipeline_and_type_and_subtype(_pst_event_hdr, _en_type, _uc_sub_type) \
    do {                                                                                           \
        frw_field_setup((_pst_event_hdr), en_pipeline, 1);                                         \
        frw_field_setup((_pst_event_hdr), uc_sub_type, (_uc_sub_type));                            \
        frw_field_setup((_pst_event_hdr), en_type, (_en_type));                                    \
    } while (0)

#define frw_event_alloc_m(_us_len) \
    frw_event_alloc(THIS_FILE_ID, __LINE__, _us_len);

#define frw_event_free_m(_pst_event_mem) \
    frw_event_free(THIS_FILE_ID, __LINE__, _pst_event_mem)

/* Hi10X���ִ�������51��Hi10XΪ�˱���ʱ���м�飬�������ܻ��е��� */
#define frw_event_alloc_big(_us_len) \
    frw_event_alloc(THIS_FILE_ID, __LINE__, _us_len);

#define frw_event_alloc_large(_us_len) \
    frw_event_alloc(THIS_FILE_ID, __LINE__, _us_len);

/* �¼�ͷ��ʼ���� */
#define frw_event_hdr_init(_pst_event_hdr, _en_type, _uc_sub_type, _us_length,              \
                           _en_pipeline, _uc_chip_id, _uc_device_id, _uc_vap_id)            \
    do {                                                                                    \
        frw_field_setup((_pst_event_hdr), us_length, ((_us_length) + FRW_EVENT_HDR_LEN));     \
        frw_field_setup((_pst_event_hdr), en_type, (_en_type));                             \
        frw_field_setup((_pst_event_hdr), uc_sub_type, (_uc_sub_type));                     \
        frw_field_setup((_pst_event_hdr), en_pipeline, (_en_pipeline));                     \
        frw_field_setup((_pst_event_hdr), uc_chip_id, (_uc_chip_id));                       \
        frw_field_setup((_pst_event_hdr), uc_device_id, (_uc_device_id));                   \
        frw_field_setup((_pst_event_hdr), uc_vap_id, (_uc_vap_id));                         \
    } while (0)

/* Ϊ��hi110x��51���ִ���һ�£����ﱣ���ú궨�壬ʹ��ʱע���frw_get_event_stru�������� */
#define frw_get_event_data(pst_event_mem) ((pst_event_mem)->puc_data)

#define frw_get_event_stru(pst_event_mem)    ((frw_event_stru *)(pst_event_mem)->puc_data)
#define frw_get_event_hdr(pst_event_mem)     ((frw_event_hdr_stru *)(&((frw_event_stru *) \
                                                                    (pst_event_mem)->puc_data)->st_event_hdr))
#define frw_get_event_payload(pst_event_mem) ((uint8_t *)((frw_event_stru *) \
                                                            (pst_event_mem)->puc_data)->auc_event_data)

/*
 * �� �� ��  : frw_event_alloc
 * ��������  : �����¼��ڴ�
 * �������  : us_length: payload���� + �¼�ͷ����
 * �� �� ֵ  : �ɹ�: ָ��frw_event_mem_stru��ָ��
 *             ʧ��:NULLL
 */
OAL_STATIC OAL_INLINE frw_event_mem_stru *frw_event_alloc(uint32_t file_id, uint32_t line_num, uint16_t payload_length)
{
    frw_event_mem_stru *pst_event_mem;

    payload_length += OAL_MEM_INFO_SIZE;

    pst_event_mem = oal_mem_alloc_enhanced(file_id, line_num, OAL_MEM_POOL_ID_EVENT,
                                           (payload_length + FRW_EVENT_HDR_LEN), OAL_TRUE);
    if (oal_unlikely(pst_event_mem == NULL)) {
        oal_mem_print_normal_pool_info(OAL_MEM_POOL_ID_EVENT);

        return NULL;
    }

    pst_event_mem->return_addr = OAL_RET_ADDR;

#if (_PRE_MULTI_CORE_MODE_SMP == _PRE_MULTI_CORE_MODE)
    pst_event_mem->puc_data += FRW_IPC_MSG_HEADER_LENGTH;
#endif

    return pst_event_mem;
}

/*
 * �� �� ��  : frw_event_free
 * ��������  : �ͷ��¼���ռ�õ��ڴ�
 */
OAL_STATIC OAL_INLINE uint32_t frw_event_free(uint32_t file_id,
                                              uint32_t line_num,
                                              frw_event_mem_stru *pst_event_mem)
{
    uint32_t ret;
    frw_event_stru *pst_frw_event = NULL;

    ret = oal_mem_free_enhanced(file_id, line_num, pst_event_mem, OAL_TRUE);
    if (oal_warn_on(ret != OAL_SUCC)) {
        pst_frw_event = frw_get_event_stru(pst_event_mem);
        oal_io_print("[E]frw event free failed!, ret:%d, type:%d, subtype:%d\r\n",
                     ret, pst_frw_event->st_event_hdr.en_type,
                     pst_frw_event->st_event_hdr.uc_sub_type);
        oal_dump_stack();
    }

    return ret;
}

/*
  10 ��������
 */
extern int32_t frw_main_init(void);
extern void frw_main_exit(void);
extern void frw_set_init_state(uint16_t en_init_state);
extern uint16_t frw_get_init_state(void);
extern uint32_t frw_event_exit(void);
extern uint32_t frw_event_init(void);
extern uint32_t frw_event_dispatch_event(frw_event_mem_stru *pst_mem);

extern void frw_event_table_register(uint8_t en_event_type,
                                     frw_event_pipeline_enum en_pipeline,
                                     frw_event_sub_table_item_stru *pst_sub_table);
void frw_event_sub_rx_adapt_table_init(frw_event_sub_table_item_stru *pst_sub_table, uint32_t table_nums,
                                       frw_event_mem_stru *(*p_rx_adapt_func)(frw_event_mem_stru *));
extern void frw_event_dump_event(uint8_t *puc_event);
extern uint32_t frw_event_flush_event_queue(uint8_t uc_event_type);
extern void frw_event_vap_pause_event(uint8_t uc_vap_id);
extern void frw_event_vap_resume_event(uint8_t uc_vap_id);
extern uint32_t frw_event_vap_flush_event(uint8_t uc_vap_id,
                                          uint8_t en_event_type,
                                          oal_bool_enum_uint8 en_drop);
extern void frw_timer_create_timer(uint32_t file_id,
                                   uint32_t line_num,
                                   frw_timeout_stru *pst_timeout,
                                   frw_timeout_func p_timeout_func,
                                   uint32_t timeout,
                                   void *p_timeout_arg,
                                   oal_bool_enum_uint8 en_is_periodic,
                                   oam_module_id_enum_uint16 en_module_id,
                                   uint32_t core_id);
extern void frw_timer_immediate_destroy_timer(uint32_t file_id,
                                              uint32_t line_num,
                                              frw_timeout_stru *pst_timeout);
extern void frw_timer_restart_timer(frw_timeout_stru *pst_timeout, uint32_t timeout,
                                    oal_bool_enum_uint8 en_is_periodic);
extern void frw_timer_add_timer(frw_timeout_stru *pst_timeout);
extern void frw_timer_stop_timer(frw_timeout_stru *pst_timeout);
extern void frw_timer_delete_all_timer(void);
extern uint32_t frw_event_queue_info(void);
extern void frw_event_process_all_event(oal_uint ui_data);
extern oal_bool_enum_uint8 frw_is_event_queue_empty(uint8_t uc_event_type);
extern oal_bool_enum_uint8 frw_is_vap_event_queue_empty(uint32_t core_id, uint8_t uc_vap_id,
                                                        uint8_t event_type);
extern uint8_t frw_task_thread_condition_check(uint32_t core_id);

extern void hcc_host_update_vi_flowctl_param(uint32_t be_cwmin, uint32_t vi_cwmin);
extern void frw_timer_clean_timer(oam_module_id_enum_uint16 en_module_id);

#endif /* end of frw_ext_if.h */
