

/* ͷ�ļ����� */
#include "oal_mem.h"
#include "oam_ext_if.h"
#include "securec.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_OAL_MEM_C

/*
 * �ṹ��  : oal_mem_subpool_stat
 * �ṹ˵��: ���ڴ��ͳ�ƽṹ�壬ά��ʹ��
 */
typedef struct {
    uint16_t us_free_cnt;  /* �����ڴ�ؿ����ڴ���� */
    uint16_t us_total_cnt; /* �����ڴ���ڴ������ */
} oal_mem_subpool_stat;

/*
 * �ṹ��  : oal_mem_pool_stat
 * �ṹ˵��: �����ڴ��ͳ�ƽṹ�壬ά��ʹ��
 */
typedef struct {
    uint16_t us_mem_used_cnt;  /* ���ڴ�������ڴ�� */
    uint16_t us_mem_total_cnt; /* ���ڴ��һ���ж����ڴ�� */
    oal_mem_subpool_stat ast_subpool_stat[WLAN_MEM_MAX_SUBPOOL_NUM];
} oal_mem_pool_stat;

/*
 * �ṹ��  : oal_mem_stat
 * �ṹ˵��: �ڴ��ͳ�ƽṹ�壬ά��ʹ��
 */
typedef struct {
    oal_mem_pool_stat ast_mem_start_stat[OAL_MEM_POOL_ID_BUTT]; /* ��ʼͳ����Ϣ */
    oal_mem_pool_stat ast_mem_end_stat[OAL_MEM_POOL_ID_BUTT];   /* ��ֹͳ����Ϣ */
} oal_mem_stat;

/* �����һ�������������ַ�������ַ���� */
#ifdef _PRE_WLAN_CACHE_COHERENT_SUPPORT
uint32_t g_dscr_fstvirt_addr = 0;
uint32_t g_dscr_fstphy_addr = 0;
#endif

/* �ڴ��ͳ����Ϣȫ�ֱ�����ά��ʹ�� */
OAL_STATIC oal_mem_stat g_mem_stat;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_mempool_info_to_sdt_stru g_mempool_info = {
    .p_mempool_info_func = NULL,
    .p_memblock_info_func = NULL
};
#else
oal_mempool_info_to_sdt_stru g_mempool_info = { NULL, NULL };
#endif
/* �����������ڴ��������Ϣȫ�ֱ��� */
OAL_STATIC oal_mem_subpool_cfg_stru g_shared_dscr_cfg_tab[] = {
    { WLAN_MEM_SHARED_RX_DSCR_SIZE + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE,  WLAN_MEM_SHARED_RX_DSCR_CNT },
    { WLAN_MEM_SHARED_TX_DSCR_SIZE1 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE, WLAN_MEM_SHARED_TX_DSCR_CNT1 },
    { WLAN_MEM_SHARED_TX_DSCR_SIZE2 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE, WLAN_MEM_SHARED_TX_DSCR_CNT2 },
};

/* �������֡�ڴ��������Ϣȫ�ֱ��� */
OAL_STATIC oal_mem_subpool_cfg_stru g_shared_mgmt_cfg_tab[] = {
    /* beacon��Probe Response��Auth Seq3֡��С������ */
    { WLAN_MEM_SHARED_MGMT_PKT_SIZE1 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE, WLAN_MEM_SHARED_MGMT_PKT_CNT1 },
};

/* ��������֡�ڴ��������Ϣȫ�ֱ��� */
OAL_STATIC oal_mem_subpool_cfg_stru g_shared_data_cfg_tab[] = {
    /* 802.11MAC֡ͷ+SNAPͷ+Ethernetͷ������ */
    { WLAN_MEM_SHARED_DATA_PKT_SIZE + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE, WLAN_MEM_SHARED_DATA_PKT_CNT }
};

/* TX RING�ڴ��������Ϣȫ�ֱ��� */
OAL_STATIC oal_mem_subpool_cfg_stru g_tx_ring_cfg_table[] = {
    { WLAN_MEM_TX_RING_SIZE1 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE, WLAN_MEM_TX_RING_CNT1},
};

/* ���������ڴ��������Ϣȫ�ֱ��� */
#define TOTAL_WLAN_MEM_LOCAL_SIZE1 (WLAN_MEM_LOCAL_SIZE1 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_LOCAL_SIZE2 (WLAN_MEM_LOCAL_SIZE2 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_LOCAL_SIZE3 (WLAN_MEM_LOCAL_SIZE3 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_LOCAL_SIZE4 (WLAN_MEM_LOCAL_SIZE4 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_LOCAL_SIZE5 (WLAN_MEM_LOCAL_SIZE5 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_LOCAL_SIZE6 (WLAN_MEM_LOCAL_SIZE6 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_LOCAL_SIZE  (TOTAL_WLAN_MEM_LOCAL_SIZE1 * WLAN_MEM_LOCAL_CNT1 + \
                                    TOTAL_WLAN_MEM_LOCAL_SIZE2 * WLAN_MEM_LOCAL_CNT2 + \
                                    TOTAL_WLAN_MEM_LOCAL_SIZE3 * WLAN_MEM_LOCAL_CNT3 + \
                                    TOTAL_WLAN_MEM_LOCAL_SIZE4 * WLAN_MEM_LOCAL_CNT4 + \
                                    TOTAL_WLAN_MEM_LOCAL_SIZE5 * WLAN_MEM_LOCAL_CNT5 + \
                                    TOTAL_WLAN_MEM_LOCAL_SIZE6 * WLAN_MEM_LOCAL_CNT6)

OAL_STATIC oal_mem_subpool_cfg_stru g_local_cfg_tab[] = {
    { TOTAL_WLAN_MEM_LOCAL_SIZE1, WLAN_MEM_LOCAL_CNT1 }, /* ��һ����С������ */
    { TOTAL_WLAN_MEM_LOCAL_SIZE2, WLAN_MEM_LOCAL_CNT2 }, /* �ڶ�����С������ */
    { TOTAL_WLAN_MEM_LOCAL_SIZE3, WLAN_MEM_LOCAL_CNT3 },
    { TOTAL_WLAN_MEM_LOCAL_SIZE4, WLAN_MEM_LOCAL_CNT4 },
    { TOTAL_WLAN_MEM_LOCAL_SIZE5, WLAN_MEM_LOCAL_CNT5 },
    { TOTAL_WLAN_MEM_LOCAL_SIZE6, WLAN_MEM_LOCAL_CNT6 },
};
/* ����ڴ��ӳ��Ƿ���� */
// cppcheck-suppress * // ����cppcheck�澯
#if (TOTAL_WLAN_MEM_LOCAL_SIZE1 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif
#if (TOTAL_WLAN_MEM_LOCAL_SIZE2 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif
#if (TOTAL_WLAN_MEM_LOCAL_SIZE3 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif
#if (TOTAL_WLAN_MEM_LOCAL_SIZE4 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif
#if (TOTAL_WLAN_MEM_LOCAL_SIZE5 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif
#if (TOTAL_WLAN_MEM_LOCAL_SIZE6 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif

/* �¼��ڴ��������Ϣȫ�ֱ��� */
#define TOTAL_WLAN_MEM_EVENT_SIZE1 (WLAN_MEM_EVENT_SIZE1 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_EVENT_SIZE2 (WLAN_MEM_EVENT_SIZE2 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_EVENT_SIZE  (TOTAL_WLAN_MEM_EVENT_SIZE1 * WLAN_MEM_EVENT_CNT1 \
                                   + TOTAL_WLAN_MEM_EVENT_SIZE2 * WLAN_MEM_EVENT_CNT2)

OAL_STATIC oal_mem_subpool_cfg_stru g_event_cfg_tab[] = {
    { TOTAL_WLAN_MEM_EVENT_SIZE1, WLAN_MEM_EVENT_CNT1 },
    { TOTAL_WLAN_MEM_EVENT_SIZE2, WLAN_MEM_EVENT_CNT2 },
};

#if (TOTAL_WLAN_MEM_EVENT_SIZE1 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif
#if (TOTAL_WLAN_MEM_EVENT_SIZE2 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif

/* MIB�ڴ��������Ϣȫ�ֱ��� */
#define TOTAL_WLAN_MEM_MIB_SIZE1 (WLAN_MEM_MIB_SIZE1 + OAL_MEM_INFO_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_MIB_SIZE  (TOTAL_WLAN_MEM_MIB_SIZE1 * WLAN_MEM_MIB_CNT1)

#if (TOTAL_WLAN_MEM_MIB_SIZE1 % OAL_MEM_INFO_SIZE)
#error alignment fault error
#endif

/* netbuf�ڴ��������Ϣȫ�ֱ��� */
#define TOTAL_WLAN_MEM_NETBUF_SIZE1 (WLAN_MEM_NETBUF_SIZE1 + OAL_NETBUF_MAINTAINS_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_NETBUF_SIZE2 (WLAN_MEM_NETBUF_SIZE2 + OAL_NETBUF_MAINTAINS_SIZE + OAL_DOG_TAG_SIZE)
#define TOTAL_WLAN_MEM_NETBUF_SIZE3 (WLAN_MEM_NETBUF_SIZE3 + OAL_NETBUF_MAINTAINS_SIZE + OAL_DOG_TAG_SIZE)
oal_mem_subpool_cfg_stru g_netbuf_cfg_tab[] = {
    { TOTAL_WLAN_MEM_NETBUF_SIZE1, WLAN_MEM_NETBUF_CNT1 }, /* ��¡��netbuf */
    { TOTAL_WLAN_MEM_NETBUF_SIZE2, WLAN_MEM_NETBUF_CNT2 },
    { TOTAL_WLAN_MEM_NETBUF_SIZE3, WLAN_MEM_NETBUF_CNT3 }
};

/* sdt netbuf�ڴ��������Ϣȫ�ֱ��� */
#define TOTAL_WLAN_MEM_SDT_NETBUF_SIZE1 (WLAN_MEM_SDT_NETBUF_SIZE1 + OAL_MEM_INFO_SIZE)
#define TOTAL_WLAN_MEM_SDT_NETBUF_SIZE2 (WLAN_MEM_SDT_NETBUF_SIZE2 + OAL_MEM_INFO_SIZE)
#define TOTAL_WLAN_MEM_SDT_NETBUF_SIZE3 (WLAN_MEM_SDT_NETBUF_SIZE3 + OAL_MEM_INFO_SIZE)
#define TOTAL_WLAN_MEM_SDT_NETBUF_SIZE4 (WLAN_MEM_SDT_NETBUF_SIZE4 + OAL_MEM_INFO_SIZE)
OAL_STATIC oal_mem_subpool_cfg_stru g_sdt_netbuf_cfg_tab[] = {
    { TOTAL_WLAN_MEM_SDT_NETBUF_SIZE1, WLAN_MEM_SDT_NETBUF_SIZE1_CNT },
    { TOTAL_WLAN_MEM_SDT_NETBUF_SIZE2, WLAN_MEM_SDT_NETBUF_SIZE2_CNT },
    { TOTAL_WLAN_MEM_SDT_NETBUF_SIZE3, WLAN_MEM_SDT_NETBUF_SIZE3_CNT },
    { TOTAL_WLAN_MEM_SDT_NETBUF_SIZE4, WLAN_MEM_SDT_NETBUF_SIZE4_CNT },
};

/* �ܵ��ڴ��������Ϣȫ�ֱ��� */
OAL_STATIC oal_mem_pool_cfg_stru g_mem_pool_cfg_tab[] = {
    /*       �ڴ��ID                       �ڴ�����ڴ�ظ���               ���ֽڶ���  �ڴ��������Ϣ */
    { OAL_MEM_POOL_ID_EVENT,           oal_array_size(g_event_cfg_tab),       { 0, 0 }, g_event_cfg_tab },
    { OAL_MEM_POOL_ID_SHARED_DATA_PKT, oal_array_size(g_shared_data_cfg_tab), { 0, 0 }, g_shared_data_cfg_tab },
    { OAL_MEM_POOL_ID_SHARED_MGMT_PKT, oal_array_size(g_shared_mgmt_cfg_tab), { 0, 0 }, g_shared_mgmt_cfg_tab },
    { OAL_MEM_POOL_ID_LOCAL,           oal_array_size(g_local_cfg_tab),       { 0, 0 }, g_local_cfg_tab },
    { OAL_MEM_POOL_ID_SHARED_DSCR,     oal_array_size(g_shared_dscr_cfg_tab), { 0, 0 }, g_shared_dscr_cfg_tab },
    { OAL_MEM_POOL_ID_TX_RING,         oal_array_size(g_tx_ring_cfg_table),   { 0, 0 }, g_tx_ring_cfg_table },
    { OAL_MEM_POOL_ID_SDT_NETBUF,      oal_array_size(g_sdt_netbuf_cfg_tab),  { 0, 0 }, g_sdt_netbuf_cfg_tab }
};

#ifdef WIN32
OAL_STATIC uint8_t *g_mem_pool[OAL_MEM_POOL_ID_BUTT] = {0};
OAL_STATIC uint8_t g_mem_pool_event[TOTAL_WLAN_MEM_EVENT_SIZE];
OAL_STATIC uint8_t g_mem_pool_shared_data_pkt[TOTAL_WLAN_MEM_SHARED_DATA_PKT_SIZE];
OAL_STATIC uint8_t g_mem_pool_shared_mgmt_pkt[TOTAL_WLAN_MEM_SHARED_MGMT_PKT_SIZE];
OAL_STATIC uint8_t g_mem_pool_local[TOTAL_WLAN_MEM_LOCAL_SIZE];
OAL_STATIC uint8_t g_mem_pool_shared_dscr[TOTAL_WLAN_MEM_SHARED_DSCR_SIZE];
OAL_STATIC uint8_t g_mem_pool_tx_ring_dscr[TOTAL_WLAN_MEM_TX_RING_SIZE];

#else
OAL_STATIC uint8_t *g_mem_pool[OAL_MEM_POOL_ID_BUTT] = {0};
OAL_STATIC uint8_t g_mem_pool_event[TOTAL_WLAN_MEM_EVENT_SIZE] __attribute__((aligned(8)));
OAL_STATIC uint8_t g_mem_pool_shared_data_pkt[TOTAL_WLAN_MEM_SHARED_DATA_PKT_SIZE] __attribute__((aligned(8)));
OAL_STATIC uint8_t g_mem_pool_shared_mgmt_pkt[TOTAL_WLAN_MEM_SHARED_MGMT_PKT_SIZE] __attribute__((aligned(8)));
OAL_STATIC uint8_t g_mem_pool_local[TOTAL_WLAN_MEM_LOCAL_SIZE] __attribute__((aligned(8)));
OAL_STATIC uint8_t g_mem_pool_shared_dscr[TOTAL_WLAN_MEM_SHARED_DSCR_SIZE] __attribute__((aligned(8)));
OAL_STATIC uint8_t g_mem_pool_tx_ring_dscr[TOTAL_WLAN_MEM_TX_RING_SIZE] __attribute__((aligned(8)));

#endif
/* ��������netbuf�ڴ�����ڴ��ID��ӳ���ϵ */
OAL_STATIC uint32_t g_truesize_to_pool_id[oal_array_size(g_netbuf_cfg_tab)] = {0};

/* ��������sdt netbuf�ڴ�����ڴ��ID��ӳ���ϵ */
OAL_STATIC uint32_t g_truesize_to_pool_id_sdt[oal_array_size(g_sdt_netbuf_cfg_tab)] = {0};

/* netbuf�ڴ��dataָ�������headָ���ƫ�� */
uint32_t g_sdt_netbuf_def_data_offset[oal_array_size(g_sdt_netbuf_cfg_tab)] = {0};

/*
 * �ڴ����Ϣȫ�ֱ������洢�����ڴ�����������ڴ����Ϣ
 * �����ڴ����ĺ��������ڴ�ȫ�ֱ������в���
 */
OAL_STATIC oal_mem_pool_stru g_st_mem_pool[OAL_MEM_POOL_ID_BUTT];

/* malloc�ڴ�ָ���¼ */
OAL_STATIC uint8_t *g_pool_base_addr[OAL_MEM_POOL_ID_BUTT] = { NULL };

/* dt netbuf�ڴ��������ַ */
OAL_STATIC oal_netbuf_stru **g_sdt_netbuf_stack_mem;

/* ���ڴ����� */
#define OAL_MEM_BLK_TOTAL_CNT (WLAN_MEM_SHARED_RX_DSCR_CNT + WLAN_MEM_SHARED_TX_DSCR_CNT1 +   \
                               WLAN_MEM_SHARED_TX_DSCR_CNT2 + WLAN_MEM_SHARED_MGMT_PKT_CNT1 + \
                               WLAN_MEM_SHARED_DATA_PKT_CNT + WLAN_MEM_LOCAL_CNT1 +           \
                               WLAN_MEM_LOCAL_CNT2 + WLAN_MEM_LOCAL_CNT3 +                    \
                               WLAN_MEM_LOCAL_CNT4 + WLAN_MEM_LOCAL_CNT5 +                    \
                               WLAN_MEM_LOCAL_CNT6 + WLAN_MEM_EVENT_CNT1 +                    \
                               WLAN_MEM_EVENT_CNT2 + WLAN_MEM_TX_RING_CNT1)

/* һ���ڴ��ṹ��С + һ��ָ���С */
#define OAL_MEM_CTRL_BLK_SIZE (sizeof(oal_mem_stru *) + sizeof(oal_mem_stru))

/* netbuf�ڴ����� */
#define OAL_MEM_NETBUF_BLK_TOTAL_CNT (WLAN_MEM_NETBUF_CNT1 + WLAN_MEM_NETBUF_CNT2 + WLAN_MEM_NETBUF_CNT3)

/* sdt netbuf�ڴ����� */
#define OAL_MEM_SDT_NETBUF_BLK_TOTAL_CNT (WLAN_MEM_SDT_NETBUF_SIZE1_CNT + WLAN_MEM_SDT_NETBUF_SIZE2_CNT + \
                                          WLAN_MEM_SDT_NETBUF_SIZE3_CNT + WLAN_MEM_SDT_NETBUF_SIZE4_CNT)

/* netbuf�ڴ�ָ���С * 2 */
#define OAL_MEM_NETBUF_CTRL_BLK_SIZE (sizeof(oal_netbuf_stru *) * 2)

/* �ڴ��ṹ���ڴ��С */
#define OAL_MEM_CTRL_BLK_TOTAL_SIZE (OAL_MEM_BLK_TOTAL_CNT * OAL_MEM_CTRL_BLK_SIZE + \
                                     OAL_MEM_SDT_NETBUF_BLK_TOTAL_CNT * OAL_MEM_NETBUF_CTRL_BLK_SIZE)

OAL_STATIC oal_netbuf_stru *g_sdt_netbuf_base_addr[OAL_MEM_SDT_NETBUF_BLK_TOTAL_CNT] = { NULL };

/*
 * �ṹ��  : oal_mem_ctrl_blk_stru
 * �ṹ˵��: ���ڴ�����ÿռ��װ��һ���ṹ��
 */
typedef struct {
    uint8_t *puc_base_addr;
    uint32_t idx;
} oal_mem_ctrl_blk_stru;

/*
 * ���ƿ��ڴ�ռ䣬Ϊ�ڴ��ṹ���ָ���ڴ��ṹ���ָ�����ռ�
 * �ɺ���oal_mem_ctrl_blk_alloc����
 */
OAL_STATIC oal_mem_ctrl_blk_stru g_ctrl_blk = { NULL, 0 };

/* ���ֽڶ�����仺�� */
#define OAL_MEM_MAX_WORD_ALIGNMENT_BUFFER 3

/*
 * ����enhanced���͵�����ӿ����ͷŽӿڣ�ÿһ���ڴ�鶼����һ��4�ֽڵ�ͷ����
 * ����ָ���ڴ�����ṹ��oal_mem_struc�������ڴ��Ľṹ������ʾ��
 * +-------------------+---------------------------------------------------+
 * | oal_mem_stru addr |                    payload                        |
 * +-------------------+---------------------------------------------------+
 * |      4 byte       |                                                   |
 * +-------------------+---------------------------------------------------+
 */
/* �ڴ�ؼ��� */
#define oal_mem_spin_lock_bh(_st_spinlock) \
    {                                      \
        oal_spin_lock_bh(&(_st_spinlock)); \
    }

/* �ڴ�ؽ��� */
#define oal_mem_spin_unlock_bh(_st_spinlock) \
    {                                        \
        oal_spin_unlock_bh(&(_st_spinlock)); \
    }

/* �ڴ�ؼ���(���ж�) */
#define oal_mem_spin_lock_irqsave(_st_spinlock, _flag)     \
    {                                                         \
        oal_spin_lock_irq_save(&(_st_spinlock), &(_flag)); \
    }

/* �ڴ�ؽ���(���ж�) */
#define oal_mem_spin_unlock_irqrestore(_st_spinlock, _flag)     \
    {                                                              \
        oal_spin_unlock_irq_restore(&(_st_spinlock), &(_flag)); \
    }

/*
 * �� �� ��  : oal_mem_ctrl_blk_alloc
 * ��������  : Ϊÿ���ڴ��ṹ���ָ���ڴ��ṹ���ָ���ṩ�ڴ�
 * �������  : size:Ҫ�����ڴ�Ĵ�С
 * �� �� ֵ  : ָ��һ���ڴ��ָ�� ���ָ��
 */
OAL_STATIC uint8_t *oal_mem_ctrl_blk_alloc(uint32_t size)
{
    uint8_t *puc_alloc;

    size = oal_get_4byte_align_value(size);
    if ((g_ctrl_blk.idx + size) > OAL_MEM_CTRL_BLK_TOTAL_SIZE) {
        oal_io_print("[file = %s, line = %d], oal_mem_ctrl_blk_alloc, not_enough memory!\n",
                     __FILE__, __LINE__);
        return NULL;
    }

    puc_alloc = g_ctrl_blk.puc_base_addr + g_ctrl_blk.idx;
    g_ctrl_blk.idx += size;

    return puc_alloc;
}
/*
 * �� �� ��  : oal_mem_get_ctrl_node
 * ��������  : ��ȡĳ�ڴ���Ӧ����Ϣ�ڵ�
 * �������  : void * p_data   payload�ڴ���׵�ַ
 * �� �� ֵ  : ָ���ڴ����Ϣ�ڵ��ָ�� ���ָ��
 */
OAL_STATIC oal_mem_stru *oal_mem_get_ctrl_node(void *p_data)
{
    oal_mem_stru *pst_mem;
    uint32_t data;

    pst_mem = (oal_mem_stru *)(*((uintptr_t *)((uintptr_t)p_data - OAL_MEM_INFO_SIZE)));
    if (oal_unlikely(((uint8_t *)pst_mem < g_ctrl_blk.puc_base_addr)
                     || ((uint8_t *)pst_mem >= g_ctrl_blk.puc_base_addr + OAL_MEM_CTRL_BLK_TOTAL_SIZE))) {
        // ��������Ϊǰһ���ڴ�鱻д���ˣ������ڴ��ǰ�汣����ڴ����Ϣ�ڵ�ĵ�ַ������
        data = (*((oal_uint *)((uint8_t *)p_data - OAL_MEM_INFO_SIZE - OAL_DOG_TAG_SIZE)));
        oal_io_print("oal_mem_get_ctrl_node::mem covered 0x%x \n", data);
        return NULL;  //lint !e527
    }

    if (oal_unlikely(pst_mem->puc_data != (uint8_t *)p_data)) {
        // �쳣���ڴ����Ϣ�ڵ��¼���ڴ���ַ�뵱ǰ��ַ��ͬ����������!!!
        oal_io_print("oal_mem_get_ctrl_node::payload=0x%p,ctrl_node=0x%p\n",
                     (void *)p_data,
                     (void *)pst_mem);
        oal_io_print("[mem info] data_addr: 0x%p, user_cnt: %u, pool_id: %u, subpool_id: %u, len: %u, "
                     "alloc_core_id = %u, alloc_file_id: %u, alloc_line_num: %u, alloc_time_stamp: %u, "
                     "trace_file_id: %u, trace_line_num: %u, trace_time_stamp: %u.\n",
                     pst_mem->puc_data,
                     pst_mem->uc_user_cnt,
                     pst_mem->en_pool_id,
                     pst_mem->uc_subpool_id,
                     pst_mem->us_len,
                     pst_mem->alloc_core_id[0],
                     pst_mem->alloc_file_id[0],
                     pst_mem->alloc_line_num[0],
                     pst_mem->alloc_time_stamp[0],
                     pst_mem->trace_file_id,
                     pst_mem->trace_line_num,
                     pst_mem->trace_time_stamp);
        return NULL;  //lint !e527
    }
    return pst_mem;
}

/*
 * �� �� ��  : oal_mem_get_pool
 * ��������  : �����ڴ��ID����ȡ�ڴ��ȫ�ֱ���ָ��
 * �������  : en_pool_id: �ڴ��ID
 * �� �� ֵ  : ָ���ڴ��ȫ�ֱ�����ָ�� ���ָ��
 */
oal_mem_pool_stru *oal_mem_get_pool(oal_mem_pool_id_enum_uint8 en_pool_id)
{
    if (oal_unlikely(en_pool_id >= OAL_MEM_POOL_ID_BUTT)) {
        oal_io_print("[file = %s, line = %d], oal_mem_get_pool, array overflow!\n",
                     __FILE__, __LINE__);
        return NULL;
    }

    return &g_st_mem_pool[en_pool_id];
}
/*
 * �� �� ��  : oal_mem_get_pool_cfg_table
 * ��������  : �����ڴ��ID����ȡ��Ӧ�ڴ��������Ϣ
 * �������  : en_pool_id: �ڴ��ID
 * �� �� ֵ  : �ɹ�: ��Ӧ�ڴ��������Ϣ�ṹ�ĵ�ַ
 *             ʧ��: ��ָ��
 */
oal_mem_pool_cfg_stru *oal_mem_get_pool_cfg_table(oal_mem_pool_id_enum_uint8 en_pool_id)
{
    if (oal_unlikely(en_pool_id >= OAL_MEM_POOL_ID_BUTT)) {
        oal_io_print("[file = %s, line = %d], oal_mem_get_pool_cfg_table, array overflow!\n",
                     __FILE__, __LINE__);
        return NULL;
    }

    return &g_mem_pool_cfg_tab[en_pool_id];
}

/*
 * �� �� ��  : oal_mem_find_available_netbuf
 * ��������  : �ӱ����ڴ�����ҵ�һ����õ�netbuf�ڴ�
 * �������  : pst_mem_subpool: ���ڴ�ؽṹ��ָ��
 *             en_netbuf_id: netbuf�ڴ�ر��
 * �� �� ֵ  : �ɹ�: ָ��oal_netbuf_stru�ṹ���ָ��
 *             ʧ��:NULLL
 */
OAL_STATIC oal_netbuf_stru *oal_mem_find_available_netbuf(oal_mem_subpool_stru *pst_mem_subpool,
                                                          oal_mem_pool_id_enum en_netbuf_id)
{
    oal_netbuf_stru *pst_netbuf = NULL;
    uint16_t us_top;
    uint16_t us_loop;
    uint16_t us_has_popped_netbuf = 0;
    oal_netbuf_stru **ppst_netbuf_stack_mem;

    us_top = pst_mem_subpool->us_free_cnt;

    if (en_netbuf_id == OAL_MEM_POOL_ID_SDT_NETBUF) {
        ppst_netbuf_stack_mem = g_sdt_netbuf_stack_mem;
    } else {
        return NULL;
    }

    while (us_top != 0) {
        us_top--;
        pst_netbuf = (oal_netbuf_stru *)pst_mem_subpool->ppst_free_stack[us_top];
        if (oal_netbuf_read_user(pst_netbuf) == 1) {
            break;
        }

        /* ���netbuf�����ü�����Ϊ1����¼�ѵ�����netbuf�ڴ�ָ��ĵ�ַ */
        ppst_netbuf_stack_mem[us_has_popped_netbuf++] = pst_netbuf;
    }

    /* ���ѵ�����netbuf�ڴ�ָ����ѹ�ض�ջ�� */
    for (us_loop = us_has_popped_netbuf; us_loop > 0; us_loop--) {
        pst_mem_subpool->ppst_free_stack[us_top++] = (oal_netbuf_stru *)ppst_netbuf_stack_mem[us_loop - 1];
    }

    /* ���������netbufָ����������ڴ�ؿ����ڴ����������Ϊ�����ڴ������ʱ��û�п�ʹ�õ��ڴ�(����Qdisc������) */
    if (us_has_popped_netbuf == pst_mem_subpool->us_free_cnt) {
        oal_io_print("popped netbuf index:%d\n", pst_mem_subpool->us_free_cnt);
        return NULL;
    }

    /* �������ڴ�ؿ����ڴ���� */
    pst_mem_subpool->us_free_cnt--;

    return pst_netbuf;
}

/*
 * �� �� ��  : oal_mem_release
 * ��������  : �ָ�(�ͷ�)�Ѿ�������ڴ�
 */
OAL_STATIC void oal_mem_release(void)
{
    uint32_t pool_id;

#ifdef _PRE_WLAN_CACHE_COHERENT_SUPPORT
    uint32_t ret;
    uint32_t total_bytes = 0; /* �ڴ�����ֽڱ��� */
#endif

    for (pool_id = 0; pool_id < OAL_MEM_POOL_ID_BUTT; pool_id++) {
        if (g_pool_base_addr[pool_id] != NULL) {
            g_pool_base_addr[pool_id] = NULL;
        }
    }

    if (g_ctrl_blk.puc_base_addr != NULL) {
        oal_free((void *)g_ctrl_blk.puc_base_addr);
        g_ctrl_blk.puc_base_addr = NULL;
    }
}

OAL_STATIC void oal_mem_sdt_netbuf_release(void)
{
    uint32_t loop;

    for (loop = 0; loop < OAL_MEM_SDT_NETBUF_BLK_TOTAL_CNT; loop++) {
        if (g_sdt_netbuf_base_addr[loop] == NULL) {
            continue;
        }

        /* ����netbuf���ü����Ƕ��٣�ͳһ��������Ϊ1 */
        oal_netbuf_set_user(g_sdt_netbuf_base_addr[loop], 1);

        oal_netbuf_free(g_sdt_netbuf_base_addr[loop]);

        g_sdt_netbuf_base_addr[loop] = NULL;
    }
}

/*
 * �� �� ��  : oal_netbuf_duplicate
 * ��������  : ����һ��netbuf,ֻ�������е����ݣ������Ƽ���
 * �������  : oal_netbuf_stru* pst_src_netbuf ����Դnetbuf
 *             uint8_t uc_out_subpool_id ָ���������netbuf ���ĸ��ڴ�ؿ���
 *             uint32_t add_head_room  ������headroom�ռ��С
 *             uint32_t add_tail_room  ������tailroom�ռ��С
 * �� �� ֵ  : oal_netbuf_stru һ����õ��ڴ��Ĺ���ṹ��,δ���뵽ΪNULL
 */
oal_netbuf_stru *oal_netbuf_duplicate(oal_netbuf_stru *pst_src_netbuf,
                                      uint8_t uc_out_subpool_id,
                                      uint32_t add_head_room,
                                      uint32_t add_tail_room)
{
    uint32_t dup_len = 0;
    oal_netbuf_stru *pst_dup_netbuf;
    int32_t ret;

    if (oal_warn_on(pst_src_netbuf == NULL)) {
        oal_io_print("%s error:pst_src_netbuf is null ", __FUNCTION__);
        return NULL;
    }

    dup_len += oal_netbuf_headroom(pst_src_netbuf);
    dup_len += oal_netbuf_len(pst_src_netbuf);
    dup_len += oal_netbuf_tailroom(pst_src_netbuf);
    dup_len += add_head_room;
    dup_len += add_tail_room;

    pst_dup_netbuf = oal_mem_netbuf_alloc(uc_out_subpool_id, dup_len, OAL_NETBUF_PRIORITY_MID);
    if (pst_dup_netbuf == NULL) {
        return NULL;
    }

    /* duplicate cb */
    ret = memcpy_s((void *)oal_netbuf_cb(pst_dup_netbuf), oal_netbuf_cb_size(),
                   (void *)oal_netbuf_cb(pst_src_netbuf), oal_netbuf_cb_size());
    if (ret != EOK) {
        oal_netbuf_free(pst_dup_netbuf);
        oal_io_print("duplicate cb failed.");
        return NULL;
    }
    skb_reserve(pst_dup_netbuf, (int32_t)oal_netbuf_headroom(pst_src_netbuf));
    oal_netbuf_put(pst_dup_netbuf, oal_netbuf_len(pst_src_netbuf));

    ret = memcpy_s((void *)oal_netbuf_data(pst_dup_netbuf), oal_netbuf_len(pst_dup_netbuf),
                   (void *)oal_netbuf_data(pst_src_netbuf), oal_netbuf_len(pst_src_netbuf));
    if (ret != EOK) {
        oal_netbuf_free(pst_dup_netbuf);
        oal_io_print("netbuf data copy failed.");
        return NULL;
    }

    return pst_dup_netbuf;
}

/*
 * �� �� ��  : oal_mem_init_ctrl_blk
 * ��������  : ��ʼ�����ƿ��ڴ�(����)
 */
OAL_STATIC void oal_mem_init_ctrl_blk(void)
{
    uint8_t *puc_base_addr;

    puc_base_addr = (uint8_t *)oal_memalloc(OAL_MEM_CTRL_BLK_TOTAL_SIZE);
    if (puc_base_addr == NULL) {
        oal_mem_release();
        oal_io_print("[file = %s, line = %d], oal_mem_init_ctrl_blk, memory allocation %u bytes fail!\n",
                     __FILE__, __LINE__, (uint32_t)OAL_MEM_CTRL_BLK_TOTAL_SIZE);
        return;
    }

    g_ctrl_blk.puc_base_addr = puc_base_addr;
    g_ctrl_blk.idx = 0;
}

/*
 * �� �� ��  : oal_mem_create_subpool
 * ��������  : �������ڴ��
 * �������  : en_pool_id   : �ڴ��ID
 *             puc_base_addr: �ڴ�ػ���ַ
 * �� �� ֵ  : OAL_SUCC������������
 */
OAL_STATIC uint32_t oal_mem_create_subpool(oal_mem_pool_id_enum_uint8 en_pool_id, uint8_t *puc_base_addr)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    oal_mem_stru *pst_mem;
    oal_mem_stru **ppst_stack_mem;
    uint8_t uc_subpool_id;
    uint32_t blk_id;

    if (puc_base_addr == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_subpool, pointer is NULL!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mem_pool = oal_mem_get_pool(en_pool_id);
    if (pst_mem_pool == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_subpool, pointer is NULL!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_GET_POOL_FAIL;
    }

    /* ��������ڴ��ַ������ÿ���ڴ������һ�Σ�����ָ��ÿ�����ڴ��ʹ�� */
    ppst_stack_mem = (oal_mem_stru **)oal_mem_ctrl_blk_alloc(sizeof(oal_mem_stru *) * pst_mem_pool->us_mem_total_cnt);
    if (ppst_stack_mem == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_subpool, pointer is NULL!\n", __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_ALLOC_CTRL_BLK_FAIL;
    }

    /* ����oal_mem_stru�ṹ�壬ÿ���ڴ������һ�Σ�����ָ��ÿ�����ڴ��ʹ�� */
    pst_mem = (oal_mem_stru *)oal_mem_ctrl_blk_alloc(sizeof(oal_mem_stru) * pst_mem_pool->us_mem_total_cnt);
    if (pst_mem == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_subpool, pointer is NULL!\n", __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_ALLOC_CTRL_BLK_FAIL;
    }

    memset_s(ppst_stack_mem, sizeof(oal_mem_stru *) * pst_mem_pool->us_mem_total_cnt,
             0, sizeof(oal_mem_stru *) * pst_mem_pool->us_mem_total_cnt);
    memset_s((void *)pst_mem, sizeof(oal_mem_stru) * pst_mem_pool->us_mem_total_cnt,
             0, sizeof(oal_mem_stru) * pst_mem_pool->us_mem_total_cnt);

    /* ��¼���ڴ�س�ʼoal_mem_stru�ṹ��ָ�룬����ڴ���Ϣʱʹ�� */
    pst_mem_pool->pst_mem_start_addr = pst_mem;

    /* ���ø��ӳ������ڴ��ṹ����Ϣ���������ڴ����payload�Ĺ�ϵ */
    for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
        /* �õ�ÿһ�����ڴ����Ϣ */
        pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[uc_subpool_id]);

        /* �������ڴ���������Ϳ����ڴ�������Ĺ�ϵ */
        pst_mem_subpool->ppst_free_stack = (void **)ppst_stack_mem;

        for (blk_id = 0; blk_id < pst_mem_subpool->us_total_cnt; blk_id++) {
            pst_mem->en_pool_id = en_pool_id;
            pst_mem->uc_subpool_id = uc_subpool_id;
            pst_mem->us_len = pst_mem_subpool->us_len;
            pst_mem->en_mem_state_flag = OAL_MEM_STATE_FREE;
            pst_mem->uc_user_cnt = 0;
            pst_mem->puc_origin_data = puc_base_addr; /* ����oal_mem_st���Ӧpayload�Ĺ�ϵ */
            pst_mem->puc_data = pst_mem->puc_origin_data;

            memset_s(pst_mem->alloc_core_id, sizeof(uint32_t) * WLAN_MEM_MAX_USERS_NUM,
                     0, sizeof(uint32_t) * WLAN_MEM_MAX_USERS_NUM);
            memset_s(pst_mem->alloc_file_id, sizeof(uint32_t) * WLAN_MEM_MAX_USERS_NUM,
                     0, sizeof(uint32_t) * WLAN_MEM_MAX_USERS_NUM);
            memset_s(pst_mem->alloc_line_num, sizeof(uint32_t) * WLAN_MEM_MAX_USERS_NUM,
                     0, sizeof(uint32_t) * WLAN_MEM_MAX_USERS_NUM);
            memset_s(pst_mem->alloc_time_stamp, sizeof(uint32_t) * WLAN_MEM_MAX_USERS_NUM,
                     0, sizeof(uint32_t) * WLAN_MEM_MAX_USERS_NUM);
            pst_mem->trace_file_id = 0;
            pst_mem->trace_line_num = 0;
            pst_mem->trace_time_stamp = 0;
            /* ���ù��� */
            *((uint32_t *)(pst_mem->puc_origin_data + pst_mem->us_len - OAL_DOG_TAG_SIZE)) = (uint32_t)OAL_DOG_TAG;

            pst_mem->return_addr = 0;

            *ppst_stack_mem = pst_mem;
            ppst_stack_mem++;
            pst_mem++;

            puc_base_addr += pst_mem_subpool->us_len;
        }
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : oal_mem_create_sdt_netbuf_subpool
 * ��������  : ����netbuf���ڴ��
 * �������  : en_pool_id: �ڴ��ID
 * �� �� ֵ  : OAL_SUCC������������
 */
OAL_STATIC uint32_t oal_mem_create_sdt_netbuf_subpool(oal_mem_pool_id_enum_uint8 en_pool_id)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    oal_netbuf_stru **ppst_stack_mem;
    oal_netbuf_stru *pst_netbuf;
    uint8_t uc_subpool_id;
    uint32_t blk_id;
    uint32_t mem_total_cnt;

    pst_mem_pool = oal_mem_get_pool(en_pool_id);
    if (pst_mem_pool == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_sdt_netbuf_subpool, pointer is NULL!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_GET_POOL_FAIL;
    }

    /* ��������ڴ��ַ������ÿ���ڴ������һ�Σ�����ָ��ÿ�����ڴ��ʹ�� */
    ppst_stack_mem = (oal_netbuf_stru **)oal_mem_ctrl_blk_alloc(sizeof(oal_netbuf_stru *) *
                                                                pst_mem_pool->us_mem_total_cnt);
    if (ppst_stack_mem == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_sdt_netbuf_subpool, pointer is NULL!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_ALLOC_CTRL_BLK_FAIL;
    }

    /* ������ʱ�ڴ��ַ���������������ҿ����ڴ�ʹ�� */
    g_sdt_netbuf_stack_mem = (oal_netbuf_stru **)oal_mem_ctrl_blk_alloc(sizeof(oal_netbuf_stru *) *
                                                                             pst_mem_pool->us_mem_total_cnt);
    if (g_sdt_netbuf_stack_mem == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_sdt_netbuf_subpool, pointer is NULL!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_ALLOC_CTRL_BLK_FAIL;
    }
    memset_s(g_sdt_netbuf_stack_mem, sizeof(oal_mem_stru *) * pst_mem_pool->us_mem_total_cnt,
             0, sizeof(oal_mem_stru *) * pst_mem_pool->us_mem_total_cnt);
    memset_s(ppst_stack_mem, sizeof(oal_mem_stru *) * pst_mem_pool->us_mem_total_cnt,
             0, sizeof(oal_mem_stru *) * pst_mem_pool->us_mem_total_cnt);

    mem_total_cnt = 0;
    for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
        /* �õ�ÿһ�����ڴ����Ϣ */
        pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[uc_subpool_id]);

        /* �������ڴ���������Ϳ����ڴ�������Ĺ�ϵ */
        pst_mem_subpool->ppst_free_stack = (void **)ppst_stack_mem;

        for (blk_id = 0; blk_id < pst_mem_subpool->us_total_cnt; blk_id++) {
            pst_netbuf = oal_mem_netbuf_alloc(OAL_NORMAL_NETBUF, pst_mem_subpool->us_len, OAL_NETBUF_PRIORITY_MID);
            if (oal_unlikely(pst_netbuf == NULL)) {
                oal_mem_sdt_netbuf_release();
                oal_io_print("[file = %s, line = %d], create_sdt_netbuf_subpool, alloc %u bytes, pointer is NULL!\n",
                             __FILE__, __LINE__, pst_mem_subpool->us_len);
                return OAL_ERR_CODE_ALLOC_MEM_FAIL;
            }

            if (blk_id == 0) {
                g_truesize_to_pool_id_sdt[uc_subpool_id] = pst_netbuf->truesize;

                g_sdt_netbuf_def_data_offset[uc_subpool_id] =
                    ((uintptr_t)pst_netbuf->data > (uintptr_t)pst_netbuf->head) ?
                        (uintptr_t)(pst_netbuf->data - pst_netbuf->head) : 0;
            }

            g_sdt_netbuf_base_addr[mem_total_cnt + blk_id] = pst_netbuf;

            *ppst_stack_mem = pst_netbuf;
            ppst_stack_mem++;
        }

        mem_total_cnt += pst_mem_subpool->us_total_cnt;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : oal_mem_create_pool
 * ��������  : ����ÿ���ڴ�ص����ڴ�ؽṹ��
 * �������  : en_pool_id       : �ڴ��ID
 *             puc_data_mem_addr: �ڴ�ػ���ַ
 * �� �� ֵ  : OAL_SUCC ������������
 */
OAL_STATIC uint32_t oal_mem_create_pool(oal_mem_pool_id_enum_uint8 en_pool_id, uint8_t *puc_base_addr)
{
    uint8_t uc_subpool_id = 0;
    uint8_t uc_subpool_cnt;
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    uint32_t ret;
    const oal_mem_pool_cfg_stru *pst_mem_pool_cfg;
    const oal_mem_subpool_cfg_stru *pst_mem_subpool_cfg;

    /* ����ж� */
    if (en_pool_id >= OAL_MEM_POOL_ID_BUTT) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_pool, array overflow!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }

    pst_mem_pool = oal_mem_get_pool(en_pool_id);
    if (pst_mem_pool == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_pool, pointer is NULL!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_GET_POOL_FAIL;
    }

    pst_mem_pool_cfg = oal_mem_get_pool_cfg_table(en_pool_id);
    if (pst_mem_pool_cfg == NULL) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_pool, pointer is NULL!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_GET_CFG_TBL_FAIL;
    }

    /* ��ʼ���ڴ�ص�ͨ�ñ��� */
    uc_subpool_cnt = pst_mem_pool_cfg->uc_subpool_cnt;

    pst_mem_pool->uc_subpool_cnt = uc_subpool_cnt;
    pst_mem_pool->us_mem_used_cnt = 0;

    pst_mem_pool->us_max_byte_len = pst_mem_pool_cfg->pst_subpool_cfg_info[uc_subpool_cnt - 1].us_size;
    if (pst_mem_pool->us_max_byte_len >= WLAN_MEM_MAX_BYTE_LEN) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_pool, exceeds the max length!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_EXCEED_MAX_LEN;
    }

    if (pst_mem_pool->uc_subpool_cnt > WLAN_MEM_MAX_SUBPOOL_NUM) {
        oal_io_print("[file = %s, line = %d], oal_mem_create_pool, exceeds the max subpool number!\n",
                     __FILE__, __LINE__);
        return OAL_ERR_CODE_OAL_MEM_EXCEED_SUBPOOL_CNT;
    }

    /* �ӳ��������ʼ���� */
    memset_s((void *)pst_mem_pool->ast_subpool_table, sizeof(pst_mem_pool->ast_subpool_table),
             0, sizeof(pst_mem_pool->ast_subpool_table));

    /* ����ÿһ�����ڴ�� */
    for (uc_subpool_id = 0; uc_subpool_id < uc_subpool_cnt; uc_subpool_id++) {
        pst_mem_subpool_cfg = pst_mem_pool_cfg->pst_subpool_cfg_info + uc_subpool_id;
        pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[uc_subpool_id]);

        pst_mem_subpool->us_free_cnt = pst_mem_subpool_cfg->us_cnt;
        pst_mem_subpool->us_total_cnt = pst_mem_subpool_cfg->us_cnt;
        pst_mem_subpool->us_len = pst_mem_subpool_cfg->us_size;

        oal_spin_lock_init(&pst_mem_subpool->st_spinlock);

        pst_mem_pool->us_mem_total_cnt += pst_mem_subpool_cfg->us_cnt; /* �������ڴ���� */
    }

    if (en_pool_id == OAL_MEM_POOL_ID_SDT_NETBUF) {
        /* ����sdt netbuf�ڴ�� */
        ret = oal_mem_create_sdt_netbuf_subpool(en_pool_id);
    } else {
        /* ������ͨ�ڴ�� */
        ret = oal_mem_create_subpool(en_pool_id, puc_base_addr);
    }

    return ret;
}

/*
 * �� �� ��  : oal_mem_init_static_mem_map
 * ��������  : ��ʼ����̬�ڴ��
 */
OAL_STATIC void oal_mem_init_static_mem_map(void)
{
    memset_s(g_mem_pool_event, sizeof(g_mem_pool_event), 0, sizeof(g_mem_pool_event));
    memset_s(g_mem_pool_shared_data_pkt, sizeof(g_mem_pool_shared_data_pkt),
             0, sizeof(g_mem_pool_shared_data_pkt));
    memset_s(g_mem_pool_shared_mgmt_pkt, sizeof(g_mem_pool_shared_mgmt_pkt),
             0, sizeof(g_mem_pool_shared_mgmt_pkt));
    memset_s(g_mem_pool_local, sizeof(g_mem_pool_local), 0, sizeof(g_mem_pool_local));
    memset_s(g_mem_pool_shared_dscr, sizeof(g_mem_pool_shared_dscr),
             0, sizeof(g_mem_pool_shared_dscr));

    memset_s(g_mem_pool_tx_ring_dscr, sizeof(g_mem_pool_tx_ring_dscr), 0, sizeof(g_mem_pool_tx_ring_dscr));

    g_mem_pool[OAL_MEM_POOL_ID_EVENT] = g_mem_pool_event;
    g_mem_pool[OAL_MEM_POOL_ID_SHARED_DATA_PKT] = g_mem_pool_shared_data_pkt;
    g_mem_pool[OAL_MEM_POOL_ID_SHARED_MGMT_PKT] = g_mem_pool_shared_mgmt_pkt;
    g_mem_pool[OAL_MEM_POOL_ID_LOCAL] = g_mem_pool_local;
    g_mem_pool[OAL_MEM_POOL_ID_SHARED_DSCR] = g_mem_pool_shared_dscr;
    g_mem_pool[OAL_MEM_POOL_ID_TX_RING] = g_mem_pool_tx_ring_dscr;
}

/*
 * �� �� ��  : oal_mem_init_pool
 * ��������  : ��ʼ��ȫ���ڴ��
 * �� �� ֵ  : OAL_SUCC ������������
 */
uint32_t oal_mem_init_pool(void)
{
    uint32_t total_bytes = 0; /* �ڴ�����ֽڱ��� */
    uint32_t pool_id = 0;     /* �ڴ��ѭ���������� */
    uint32_t ret;
    uint8_t *puc_base_addr; /* ����malloc������ڴ����ַ */

    memset_s((void *)g_st_mem_pool, sizeof(g_st_mem_pool), 0, sizeof(g_st_mem_pool));
    memset_s((void *)g_pool_base_addr, sizeof(g_pool_base_addr),
             0, sizeof(g_pool_base_addr));
    /* ��ʼ�����ƿ��ڴ� */
    oal_mem_init_ctrl_blk();

    /* ��ʼ����̬�ڴ��Ӧ��ϵ */
    oal_mem_init_static_mem_map();

    for (pool_id = 0; pool_id < OAL_MEM_POOL_ID_SDT_NETBUF; pool_id++) {
        puc_base_addr = g_mem_pool[pool_id];

        if (puc_base_addr == NULL) {
            oal_mem_release();
            oal_io_print("[file = %s, line = %d], oal_mem_init_pool, memory allocation %u bytes fail!\n",
                         __FILE__, __LINE__, total_bytes + OAL_MEM_MAX_WORD_ALIGNMENT_BUFFER);
            return OAL_ERR_CODE_ALLOC_MEM_FAIL;
        }

        /* ��¼ÿ���ڴ��oal_malloc����ĵ�ַ */
        g_pool_base_addr[pool_id] = puc_base_addr;

        puc_base_addr = (uint8_t *)(uintptr_t)oal_get_4byte_align_value((uintptr_t)puc_base_addr);

        ret = oal_mem_create_pool((uint8_t)pool_id, puc_base_addr);
        if (ret != OAL_SUCC) {
            oal_mem_release();
            oal_io_print("[file = %s, line = %d], oal_mem_init_pool, oal_mem_create_pool failed!\n",
                         __FILE__, __LINE__);
            return ret;
        }
    }

    /* ����sdt netbuf�ڴ�� */
    return oal_mem_create_pool(OAL_MEM_POOL_ID_SDT_NETBUF, NULL);
}

/*
 * �� �� ��  : oal_mem_alloc_enhanced
 * ��������  : �����ڴ�
 * �������  : file_id  : �����ڴ�������ļ�ID
 *             line_num : �����ڴ����������к�
 *             uc_pool_id  : �������ڴ���ڴ��ID
 *             us_len      : �������ڴ�鳤��
 *             uc_lock     : �Ƿ���Ҫ��������
 * �� �� ֵ  : ������ڴ��ṹ��ָ�룬���ָ��
 */
oal_mem_stru *oal_mem_alloc_enhanced(uint32_t file_id,
                                     uint32_t line_num,
                                     oal_mem_pool_id_enum_uint8 en_pool_id,
                                     uint16_t us_len,
                                     uint8_t uc_lock)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    oal_mem_stru *pst_mem;
    uint8_t uc_subpool_id;
    oal_uint irq_flag = 0;
    uint8_t uc_subpool_cnt;
    oal_mem_subpool_stru *pst_subpool_table;

    /* ��ȡ�ڴ�� */
    pst_mem_pool = &g_st_mem_pool[en_pool_id];

    us_len += OAL_DOG_TAG_SIZE;

    /* �쳣: ���볤�Ȳ��ڸ��ڴ����  */
    if (oal_unlikely(us_len > pst_mem_pool->us_max_byte_len)) {
        return NULL;
    }

    pst_mem = NULL;

    uc_subpool_cnt = pst_mem_pool->uc_subpool_cnt;
    pst_subpool_table = pst_mem_pool->ast_subpool_table;

    for (uc_subpool_id = 0; uc_subpool_id < uc_subpool_cnt; uc_subpool_id++) {
        pst_mem_subpool = &(pst_subpool_table[uc_subpool_id]);
        oal_mem_spin_lock_irqsave(pst_mem_subpool->st_spinlock, irq_flag)
        if ((pst_mem_subpool->us_len < us_len) || (pst_mem_subpool->us_free_cnt == 0)) {
            oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)
            continue;
        }

        /* ��ȡһ��û��ʹ�õ�oal_mem_stru��� */
        pst_mem_subpool->us_free_cnt--;
        pst_mem = (oal_mem_stru *)pst_mem_subpool->ppst_free_stack[pst_mem_subpool->us_free_cnt];

        pst_mem->puc_data = pst_mem->puc_origin_data;
        pst_mem->uc_user_cnt = 1;
        pst_mem->en_mem_state_flag = OAL_MEM_STATE_ALLOC;

        pst_mem_pool->us_mem_used_cnt++;

        pst_mem->alloc_core_id[0] = oal_get_core_id();
        pst_mem->alloc_file_id[0] = file_id;
        pst_mem->alloc_line_num[0] = line_num;
        pst_mem->alloc_time_stamp[0] = (uint32_t)oal_time_get_stamp_ms();

        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)
        break;
    }

    return pst_mem;
}

/*
 * �� �� ��  : oal_mem_free_enhanced
 * ��������  : �ͷ��ڴ�
 * �������  : file_id  : �����ڴ��ͷŵ��ļ�ID
 *             line_num : �����ڴ��ͷ������к�
 *             pst_mem     : Ҫ�ͷ��ڴ���ַ
 *             uc_lock     : �Ƿ���Ҫ��������
 * �� �� ֵ  : OAL_SUCC ��������������
 */
uint32_t oal_mem_free_enhanced(uint32_t file_id,
                               uint32_t line_num,
                               oal_mem_stru *pst_mem,
                               uint8_t uc_lock)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    uint32_t dog_tag;

    oal_uint irq_flag = 0;
    if (oal_unlikely(pst_mem == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mem_pool = &g_st_mem_pool[pst_mem->en_pool_id];

    pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[pst_mem->uc_subpool_id]);

    oal_mem_spin_lock_irqsave(pst_mem_subpool->st_spinlock, irq_flag)

    /* �쳣: �ڴ�дԽ�� */
    dog_tag = (*((uint32_t *)(pst_mem->puc_origin_data + pst_mem->us_len - OAL_DOG_TAG_SIZE)));
    if (oal_unlikely(dog_tag != OAL_DOG_TAG)) {
        /* �ָ��������� */
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)
        return OAL_ERR_CODE_OAL_MEM_DOG_TAG;
    }

    /* �쳣: �ͷ�һ���Ѿ����ͷŵ��ڴ� */
    if (oal_unlikely(pst_mem->en_mem_state_flag == OAL_MEM_STATE_FREE)) {
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)
        return OAL_ERR_CODE_OAL_MEM_ALREADY_FREE;
    }

    /* �쳣: �ͷ�һ�����ü���Ϊ0���ڴ� */
    if (oal_unlikely(pst_mem->uc_user_cnt == 0)) {
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)
        return OAL_ERR_CODE_OAL_MEM_USER_CNT_ERR;
    }

    pst_mem->alloc_core_id[pst_mem->uc_user_cnt - 1] = 0;
    pst_mem->alloc_file_id[pst_mem->uc_user_cnt - 1] = 0;
    pst_mem->alloc_line_num[pst_mem->uc_user_cnt - 1] = 0;
    pst_mem->alloc_time_stamp[pst_mem->uc_user_cnt - 1] = 0;

    pst_mem->uc_user_cnt--;

    /* ���ڴ�����Ƿ������������û���ֱ�ӷ��� */
    if (pst_mem->uc_user_cnt != 0) {
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)

        return OAL_SUCC;
    }

    /* �쳣: �����ڴ�ؿ����ڴ����Ŀ�����������ڴ�����ڴ���� */
    if (oal_unlikely(pst_mem_subpool->us_free_cnt >= pst_mem_subpool->us_total_cnt)) {
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)

        return OAL_ERR_CODE_OAL_MEM_EXCEED_TOTAL_CNT;
    }

    pst_mem->trace_file_id = 0;
    pst_mem->trace_line_num = 0;
    pst_mem->trace_time_stamp = 0;

    pst_mem->return_addr = 0;

    pst_mem->en_mem_state_flag = OAL_MEM_STATE_FREE;

    pst_mem_subpool->ppst_free_stack[pst_mem_subpool->us_free_cnt] = (void *)pst_mem;
    pst_mem_subpool->us_free_cnt++;

    pst_mem_pool->us_mem_used_cnt--;

    oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)

    return OAL_SUCC;
}

/*
 * �� �� ��  : oal_mem_free_enhanced_array
 * ��������  : �����ͷ��ڴ� (ע�⺯���в����ڴ��, Ҫ���ڴ����ͬһ���ڴ��ӳ���)
 * �������  : file_id         : �����ڴ��ͷŵ��ļ�ID
 *             line_num        : �����ڴ��ͷ������к�
 *             p_data             : Ҫ�ͷ��ڴ������ͷָ��
 *             uc_mem_num         : Ҫ�ͷŵ��ڴ����Ŀ
 * �� �� ֵ  : OAL_SUCC ��������������
 */
OAL_STATIC OAL_INLINE uint32_t oal_mem_free_enhanced_array(uint32_t file_id,
                                                           uint32_t line_num,
                                                           void *p_data,
                                                           uint8_t uc_mem_num)
{
    oal_mem_pool_stru *pst_mem_pool = NULL;
    oal_mem_subpool_stru *pst_mem_subpool = NULL;
    oal_mem_stru *pst_mem = NULL;
    uint8_t uc_index;
    oal_uint *past_mem;

    past_mem = (oal_uint *)p_data;

    for (uc_index = 0; uc_index < uc_mem_num; uc_index++) {
        pst_mem = (oal_mem_stru *)(*((uintptr_t *)((uintptr_t)past_mem[uc_index] - OAL_MEM_INFO_SIZE)));
        if (oal_unlikely(pst_mem == NULL)) {
            continue;
        }

        /* pool��ַֻ��Ҫȡһ�� */
        if (pst_mem_pool == NULL) {
            pst_mem_pool = &g_st_mem_pool[pst_mem->en_pool_id];
            pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[pst_mem->uc_subpool_id]);
        }

        /* �쳣: �ͷ�һ���Ѿ����ͷŵ��ڴ� */
        if (oal_unlikely(pst_mem->en_mem_state_flag == OAL_MEM_STATE_FREE)) {
            continue;
        }

        /* �쳣: �ͷ�һ�����ü���Ϊ0���ڴ� */
        if (oal_unlikely(pst_mem->uc_user_cnt == 0)) {
            continue;
        }

        pst_mem->alloc_core_id[pst_mem->uc_user_cnt - 1] = 0;
        pst_mem->alloc_file_id[pst_mem->uc_user_cnt - 1] = 0;
        pst_mem->alloc_line_num[pst_mem->uc_user_cnt - 1] = 0;

        pst_mem->uc_user_cnt--;

        /* ���ڴ�����Ƿ������������û���ֱ�ӷ��� */
        if (oal_unlikely(pst_mem->uc_user_cnt != 0)) {
            continue;
        }

        if (oal_unlikely(pst_mem_subpool == NULL)) {
            continue;
        }

        /*lint -e613*/
        /* �쳣: �����ڴ�ؿ����ڴ����Ŀ�����������ڴ�����ڴ���� */
        if (oal_unlikely(pst_mem_subpool->us_free_cnt >= pst_mem_subpool->us_total_cnt)) {
            continue;
        }

        pst_mem->en_mem_state_flag = OAL_MEM_STATE_FREE;
        /*lint -e613*/
        pst_mem_subpool->ppst_free_stack[pst_mem_subpool->us_free_cnt] = (void *)pst_mem;
        pst_mem_subpool->us_free_cnt++;
        /*lint +e613*/
        pst_mem_pool->us_mem_used_cnt--;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : oal_mem_incr_user
 * ��������  : �ڴ�����ü�����1
 * �������  : file_id  : �ļ�ID
 *             line_num : �к�
 *             pst_mem     : Ҫ�����û����ڴ��ָ��
 *             uc_lock     : �Ƿ���Ҫ��������
 * �� �� ֵ  : OAL_SUCC ������������
 */
uint32_t oal_mem_incr_user(uint32_t file_id,
                           uint32_t line_num,
                           oal_mem_stru *pst_mem,
                           uint8_t uc_lock)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    oal_uint flag = 0;

    if (oal_unlikely(pst_mem == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mem_pool = &g_st_mem_pool[pst_mem->en_pool_id];

    pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[pst_mem->uc_subpool_id]);

    oal_mem_spin_lock_irqsave(pst_mem_subpool->st_spinlock, flag)

    /* �쳣: �ڴ���Ѿ����ͷ� */
    if (oal_unlikely(pst_mem->en_mem_state_flag == OAL_MEM_STATE_FREE)) {
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, flag)

        return OAL_ERR_CODE_OAL_MEM_ALREADY_FREE;
    }

    /* �쳣: ���ڴ���ϵĹ����û�����Ϊ���ֵ */
    if (oal_unlikely((uint16_t)(pst_mem->uc_user_cnt + 1) > WLAN_MEM_MAX_USERS_NUM)) {
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, flag)

        return OAL_ERR_CODE_OAL_MEM_USER_CNT_ERR;
    }

    pst_mem->uc_user_cnt++;

    pst_mem->alloc_core_id[pst_mem->uc_user_cnt - 1] = oal_get_core_id();
    pst_mem->alloc_file_id[pst_mem->uc_user_cnt - 1] = file_id;
    pst_mem->alloc_line_num[pst_mem->uc_user_cnt - 1] = line_num;
    pst_mem->alloc_time_stamp[pst_mem->uc_user_cnt - 1] = (uint32_t)oal_time_get_stamp_ms();

    oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, flag)

    return OAL_SUCC;
}

/*
 * �� �� ��  : oal_mem_alloc
 * ��������  : �����ڴ�
 * �������  : file_id  : �����ڴ�������ļ�ID
 *             line_num : �����ڴ����������к�
 *             uc_pool_id  : �������ڴ���ڴ��ID
 *             us_len      : �������ڴ�鳤��
 *             uc_lock     : �Ƿ���Ҫ��������
 * �� �� ֵ  : �ɹ�: ָ���������ڴ���ʼ��ַ��ָ��
 *             ʧ��: ��ָ��
 */
void *oal_mem_alloc(uint32_t file_id,
                    uint32_t line_num,
                    oal_mem_pool_id_enum_uint8 en_pool_id,
                    uint16_t us_len,
                    uint8_t uc_lock)
{
    oal_mem_stru *pst_mem;

    /* �쳣: ���볤��Ϊ�� */
    if (oal_unlikely(us_len == 0)) {
        return NULL;
    }

    us_len += OAL_MEM_INFO_SIZE;

    pst_mem = oal_mem_alloc_enhanced(file_id, line_num, en_pool_id, us_len, uc_lock);
    if (oal_unlikely(pst_mem == NULL)) {
        if (en_pool_id < OAL_MEM_POOL_ID_SHARED_DSCR) {
            oal_mem_print_normal_pool_info(en_pool_id);
        }

        return NULL;
    }

    pst_mem->return_addr = OAL_RET_ADDR;

    pst_mem->puc_data = pst_mem->puc_origin_data + OAL_MEM_INFO_SIZE;
    *((uintptr_t *)pst_mem->puc_origin_data) = (uintptr_t)pst_mem;
    return (void *)pst_mem->puc_data;
}

/*
 * �� �� ��  : oal_mem_free
 * ��������  : �ͷ��ڴ�
 * �������  : file_id  : �����ڴ��ͷŵ��ļ�ID
 *             line_num : �����ڴ��ͷ������к�
 *             p_data      : Ҫ�ͷ��ڴ���ַ
 *             uc_lock     : �Ƿ���Ҫ��������
 * �� �� ֵ  : OAL_SUCC ��������������
 */
uint32_t oal_mem_free(uint32_t file_id,
                      uint32_t line_num,
                      void *p_data,
                      uint8_t uc_lock)
{
    oal_mem_stru *pst_mem;
    uint32_t ret;

    if (oal_unlikely(p_data == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mem = oal_mem_get_ctrl_node(p_data);
    if (oal_unlikely(pst_mem == NULL)) {
        return OAL_FAIL;
    }

    ret = oal_mem_free_enhanced(file_id, line_num, pst_mem, uc_lock);
    if (oal_unlikely(ret != OAL_SUCC)) {
        // ���ڴ����м������е���OAL_MEM_FREE�ӿ��ͷ��ڴ���Դʱ��δ�����쳣���������ڴ˴�ͳһ����
        // �ͷ���Դʱ�����쳣����ֵ����������!!!
        oal_io_print("oal_mem_free_enhanced failed!(%d)", ret);
    }
    return ret;
}

/*
 * �� �� ��  : oal_mem_free_array
 * ��������  : �����ͷ��ڴ� (ע�⺯���в����ڴ��, Ҫ���ڴ����ͬһ���ڴ��ӳ���)
 * �������  : file_id         : �����ڴ��ͷŵ��ļ�ID
 *             line_num        : �����ڴ��ͷ������к�
 *             p_data             : Ҫ�ͷ��ڴ������ͷָ��
 *             uc_mem_num         : Ҫ�ͷŵ��ڴ����Ŀ
 * �� �� ֵ  : OAL_SUCC ��������������
 */
/*lint -e695*/
uint32_t oal_mem_free_array(uint32_t file_id,
                            uint32_t line_num,
                            void *p_data,
                            uint8_t uc_mem_num)
{
    return oal_mem_free_enhanced_array(file_id, line_num, p_data, uc_mem_num);
}

/*lint +e695*/
/*
 * �� �� ��  : oal_mem_sdt_netbuf_alloc
 * ��������  : ����netbuf�ڴ�
 * �������  : us_len : �������ڴ�鳤��(������֡�ĳ��ȣ�������netbuf�ṹ��ĳ���)
 *             �ڴ�صĳ���Ϊnetlink��Ϣͷ+nlkpayload����
 *             nlkpaload��sdt��Ϣͷ+ʵ������+sdt��Ϣβ
 *             uc_lock: �Ƿ���Ҫ��������
 * �� �� ֵ  : �ɹ�: ָ��oal_netbuf_stru�ṹ���ָ��
 *             ʧ��:NULLL
 */
oal_netbuf_stru *oal_mem_sdt_netbuf_alloc(uint16_t us_len, uint8_t uc_lock)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    oal_netbuf_stru *pst_netbuf = NULL;
    uint8_t uc_subpool_id;
    oal_uint irq_flag = 0;
    uint32_t headroom;

    /* ��ȡ�ڴ�� */
    pst_mem_pool = &g_st_mem_pool[OAL_MEM_POOL_ID_SDT_NETBUF];

    /* �쳣: ���볤�Ȳ��ڸ��ڴ����  */
    if (oal_unlikely(us_len > pst_mem_pool->us_max_byte_len)) {
        oal_io_print("sdt netbuf alloc fail, len:%d, max len limit:%d\n", us_len, pst_mem_pool->us_max_byte_len);
        return NULL;
    }

    for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
        pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[uc_subpool_id]);

        oal_mem_spin_lock_irqsave(pst_mem_subpool->st_spinlock, irq_flag);

        /* ��������ڴ�ؿ����ڴ����Ϊ0�����߳��Ȳ����ϣ���������һ�����ڴ��Ѱ�� */
        if ((pst_mem_subpool->us_len < us_len) || (pst_mem_subpool->us_free_cnt == 0)) {
            oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag);
            continue;
        }

        /* ��������ڴ�����Ҳ���һ������ڴ�(���ü���Ϊ1���ڴ�)����������һ�����ڴ��Ѱ�� */
        pst_netbuf = oal_mem_find_available_netbuf(pst_mem_subpool, OAL_MEM_POOL_ID_SDT_NETBUF);
        if (pst_netbuf == NULL) {
            oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag);
            continue;
        }

        /* ���ü�����1 */
        oal_netbuf_increase_user(pst_netbuf);

        pst_mem_pool->us_mem_used_cnt++;

        /* netbuf��dataָ�븴λ */
        headroom = g_sdt_netbuf_def_data_offset[uc_subpool_id];

        if (oal_netbuf_headroom(pst_netbuf) > headroom) {
            oal_netbuf_push(pst_netbuf, oal_netbuf_headroom(pst_netbuf) - headroom);
        } else {
            oal_netbuf_pull(pst_netbuf, headroom - oal_netbuf_headroom(pst_netbuf));
        }

        if (oal_unlikely(oal_netbuf_headroom(pst_netbuf) != headroom)) {
            oal_io_print("%s error: oal_netbuf_headroom(pst_netbuf):%d not equal %u",
                         __FUNCTION__, (int32_t)oal_netbuf_headroom(pst_netbuf), headroom);
            oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag);
            break;
        }

        oal_netbuf_trim(pst_netbuf, pst_netbuf->len);

        if (oal_unlikely(pst_netbuf->len)) {
            oal_io_print("%s error: pst_netbuf->len:%d is not 0", __FUNCTION__, (int32_t)pst_netbuf->len);
        }
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag);

        break;
    }

    return pst_netbuf;
}

OAL_STATIC OAL_INLINE uint32_t oal_mem_find_netbuf_subpool_id(oal_netbuf_stru *pst_netbuf,
                                                              oal_mem_pool_stru *pst_mem_pool,
                                                              uint8_t *puc_subpool_id)
{
    uint8_t uc_subpool_id;

    for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
        if (g_truesize_to_pool_id[uc_subpool_id] == pst_netbuf->truesize) {
            *puc_subpool_id = uc_subpool_id;

            return OAL_SUCC;
        }
    }

    return OAL_FAIL;
}

/*
 * �� �� ��  : oal_mem_find_sdt_netbuf_subpool_id
 * ��������  : ����sdt netbuf���ӳ�
 */
OAL_STATIC OAL_INLINE uint32_t oal_mem_find_sdt_netbuf_subpool_id(oal_netbuf_stru *pst_netbuf,
                                                                  oal_mem_pool_stru *pst_mem_pool,
                                                                  uint8_t *puc_subpool_id)
{
    uint8_t uc_subpool_id;

    for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
        if (g_truesize_to_pool_id_sdt[uc_subpool_id] == pst_netbuf->truesize) {
            *puc_subpool_id = uc_subpool_id;

            return OAL_SUCC;
        }
    }

    return OAL_FAIL;
}

/*
 * �� �� ��  : oal_mem_sdt_netbuf_free
 * ��������  : �ͷ�netbuf�ڴ�
 * �������  : pst_netbuf: ָ��oal_netbuf_stru�ṹ���ָ��
 *             uc_lock   : �Ƿ���Ҫ��������
 * �� �� ֵ  : OAL_SUCC ��������������
 */
uint32_t oal_mem_sdt_netbuf_free(oal_netbuf_stru *pst_netbuf, uint8_t uc_lock)
{
    uint8_t uc_subpool_id;
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    uint32_t ret;
    oal_uint irq_flag = 0;

    if (oal_unlikely(pst_netbuf == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mem_pool = &g_st_mem_pool[OAL_MEM_POOL_ID_SDT_NETBUF];

    /* ��ȡ��netbuf�ڴ����������ڴ��ID */
    ret = oal_mem_find_sdt_netbuf_subpool_id(pst_netbuf, pst_mem_pool, &uc_subpool_id);
    if (oal_unlikely(ret != OAL_SUCC)) {
        oal_netbuf_decrease_user(pst_netbuf);
        return OAL_ERR_CODE_OAL_MEM_SKB_SUBPOOL_ID_ERR;
    }

    pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[uc_subpool_id]);

    oal_mem_spin_lock_irqsave(pst_mem_subpool->st_spinlock, irq_flag)

    /* �쳣: �����ڴ�ؿ����ڴ����Ŀ�����������ڴ�����ڴ���� */
    if (oal_unlikely(pst_mem_subpool->us_free_cnt >= pst_mem_subpool->us_total_cnt)) {
        oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)

        return OAL_ERR_CODE_OAL_MEM_EXCEED_TOTAL_CNT;
    }

    /* ����netbuf�ڴ�黹����Ӧ�����ڴ�� */
    pst_mem_subpool->ppst_free_stack[pst_mem_subpool->us_free_cnt] = (void *)pst_netbuf;
    pst_mem_subpool->us_free_cnt++;

    pst_mem_pool->us_mem_used_cnt--;
    oal_netbuf_decrease_user(pst_netbuf);
    oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, irq_flag)

    return OAL_SUCC;
}

/*
 * �� �� ��  : oal_mempool_info_to_sdt_register
 * ��������  : �ṩ��oamģ���ע�ắ����ע��ĺ��������ǽ��ڴ����Ϣ�ϱ�SDT
 */
uint32_t oal_mempool_info_to_sdt_register(oal_stats_info_up_to_sdt p_up_mempool_info,
                                          oal_memblock_info_up_to_sdt p_up_memblock_info)
{
    g_mempool_info.p_mempool_info_func = p_up_mempool_info;
    g_mempool_info.p_memblock_info_func = p_up_memblock_info;

    return OAL_SUCC;
}

void oal_mem_info(oal_mem_pool_id_enum_uint8 en_pool_id)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    uint8_t uc_subpool_id;

    pst_mem_pool = oal_mem_get_pool(en_pool_id);
    if (oal_unlikely(pst_mem_pool == NULL)) {
        return;
    }

    oal_io_print("pool(%u) used/total=%u/%u:\r\n",
                 en_pool_id, pst_mem_pool->us_mem_used_cnt, pst_mem_pool->us_mem_total_cnt);
    for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
        pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[uc_subpool_id]);

        /* �ӳ�ʹ�������oal_mem_leak�����л��ϱ�ÿ���ڴ�����Ϣ */
        if (g_mempool_info.p_mempool_info_func != NULL) {
            g_mempool_info.p_mempool_info_func(en_pool_id,
                                               pst_mem_pool->us_mem_total_cnt,
                                               pst_mem_pool->us_mem_used_cnt,
                                               uc_subpool_id,
                                               pst_mem_subpool->us_total_cnt,
                                               pst_mem_subpool->us_free_cnt);
        }

        oal_io_print("    subpool(%d) free/total=%u/%u\r\n",
                     uc_subpool_id, pst_mem_subpool->us_free_cnt,
                     pst_mem_subpool->us_total_cnt);
    }
}

/*
 * �� �� ��  : oal_get_func_name
 * ��������  : ��������ַת��Ϊ��������
 * �������  : call_func_ddr: ����������������ص�ַ
 * �������  : buff: ����������洢��������
 * �� �� ֵ  : buf_cnt: buff�洢�ֽڳ���
 */
uint8_t oal_get_func_name(uint8_t *buff, uintptr_t call_func_ddr)
{
    uint8_t buf_cnt;

#if (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    buf_cnt = sprint_symbol(buff, call_func_ddr);
#else
    buf_cnt = 0;
    *buff = '\0';
#endif

    return buf_cnt;
}

/*
 * �� �� ��  : oal_print_func
 * ��������  : linux �´�ӡ�������ù�ϵ
 */
void oal_mem_print_funcname(uintptr_t func_addr)
{
    uint8_t size;
    uint8_t buff[sizeof("Func: ") + OAL_MEM_SPRINT_SYMBOL_SIZE] = {"Func: "};

    size = oal_get_func_name(buff + strlen("Func: "), func_addr);
    if (size > 0) {
        oam_print(buff); /* OTA��ӡ�������� */
    } else {
        oal_io_print("mem printf func name failed.\n");
    }
}
oal_module_symbol(oal_mem_print_funcname);

/*
 * �� �� ��  : oal_mem_return_addr_count
 * ��������  : ͳ��ĳ���ڴ���У�ĳ�������ܹ�ռ���ڴ�����
 */
uint32_t oal_mem_return_addr_count(oal_mem_subpool_stru *pst_mem_subpool, oal_mem_stru *pst_mem_base,
                                   uintptr_t call_func_addr)
{
    uint16_t us_loop;
    oal_mem_stru *pst_mem;
    uint32_t us_count = 0;

    pst_mem = pst_mem_base;

    for (us_loop = 0; us_loop < pst_mem_subpool->us_total_cnt; us_loop++) {
        if ((pst_mem->en_mem_state_flag == OAL_MEM_STATE_ALLOC) && (pst_mem->return_addr == call_func_addr)) {
            us_count++;
        }
        pst_mem++;
    }

    return us_count;
}

/*
 * �� �� ��  : oal_mem_func_addr_is_registerd
 * ��������  : ͳ���������Ƿ��Ѿ��洢ĳ��������ַ
 * �������  : ua_func_addr: ���溯����ַ������
 *             call_func_addr: ������ַ
 * �������  : p_func_loop: �����е�һ��Ϊ0���±�ֵ
 */
uint8_t oal_mem_func_addr_is_registerd(uintptr_t *ua_func_addr, uint8_t uc_func_size,
                                       uint8_t *p_func_loop, uintptr_t call_func_addr)
{
    uint8_t uc_loop = 0;

    /* �������ж��Ƿ�����ַ��ͬ */
    while (ua_func_addr[uc_loop]) {
        /* ��������򷵻�true */
        if (ua_func_addr[uc_loop] == call_func_addr) {
            return OAL_TRUE;
        }

        uc_loop++;
        if (uc_func_size == uc_loop) {
            break;
        }
    }

    if (uc_func_size == uc_loop) {
        uc_loop = 0;
    }

    /* ����������Ҫ��¼�����±� */
    *p_func_loop = uc_loop;

    return OAL_FALSE;
}

/*
 * �� �� ��  : oal_mem_print_normal_pool_info
 * ��������  : �ڴ�����ʱ��ͳ���ϱ���ͨ�ڴ�ռ�����
 */
void oal_mem_print_normal_pool_info(oal_mem_pool_id_enum_uint8 en_pool_id)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    oal_mem_stru *pst_mem;
    oal_mem_stru *pst_mem_base;
    uint16_t us_loop;
    uint8_t uc_subpool_id = 0;
    uintptr_t ua_func_addr[50] = {0}; /* 50��Ԫ���㹻���һ���ַ */
    uint8_t us_func_loop = 0;
    uint32_t us_ret_count = 0;
    uint8_t buff[sizeof("[%d] mem blocks occupied by ") + OAL_MEM_SPRINT_SYMBOL_SIZE] = {
        "[%d] mem blocks occupied by "};
    uint8_t size;

    pst_mem_pool = oal_mem_get_pool(en_pool_id);
    if (oal_unlikely(pst_mem_pool == NULL)) {
        return;
    }

    oam_warning_log4(0, OAM_SF_CFG,
                     "{host memory info: pool id=%d, subpool cnt=%d, mem block total cnt=%d, used cnt=%d.}",
                     en_pool_id, pst_mem_pool->uc_subpool_cnt,
                     pst_mem_pool->us_mem_total_cnt,
                     pst_mem_pool->us_mem_used_cnt);

    pst_mem = pst_mem_pool->pst_mem_start_addr;

    /* ѭ��ÿһ���ӳ� */
    for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
        /* �õ�ÿһ�����ڴ����Ϣ */
        pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[uc_subpool_id]);

        oam_warning_log4(0, OAM_SF_CFG, "{host subpool id[%d] info: len =%d, mem block total cnt=%d, free cnt=%d.}",
                         uc_subpool_id, pst_mem_subpool->us_len,
                         pst_mem_subpool->us_total_cnt,
                         pst_mem_subpool->us_free_cnt);

        /* �������ӳػ���ַ */
        pst_mem_base = pst_mem;

        /* ѭ����ѯÿһ���ӳص�mem block */
        for (us_loop = 0; us_loop < pst_mem_subpool->us_total_cnt; us_loop++) {
            if ((pst_mem->return_addr == 0) && (pst_mem->en_mem_state_flag == OAL_MEM_STATE_ALLOC)) {
                oam_warning_log2(0, OAM_SF_CFG,
                    "{oal_mem_print_normal_pool_info:: subpool id[%d] mem block[%d] has no call func addr.}",
                    uc_subpool_id, us_loop);
            } else if ((pst_mem->en_mem_state_flag == OAL_MEM_STATE_ALLOC) &&
                       (oal_mem_func_addr_is_registerd(ua_func_addr, (uint8_t)sizeof(ua_func_addr), &us_func_loop,
                                                       pst_mem->return_addr) == OAL_FALSE)) {
                ua_func_addr[us_func_loop] = pst_mem->return_addr;

                us_ret_count = oal_mem_return_addr_count(pst_mem_subpool, pst_mem_base, pst_mem->return_addr);

                /* OTA��ӡ�������� */
                size = oal_get_func_name(buff + strlen("[%d] mem blocks occupied by "), pst_mem->return_addr);
                if (size > 0) {
                    oam_print(buff);
                }
            }
            /* ��ѯ��һ���ڴ�� */
            pst_mem++;
        }
        /* ��ѯ��һ���ӳأ�������� */
        memset_s(ua_func_addr, sizeof(ua_func_addr), 0, sizeof(ua_func_addr));
    }
}
oal_module_symbol(oal_mem_print_normal_pool_info);

/*
 * �� �� ��  : oal_mem_print_pool_info
 * ��������  : ͳ����������ڴ����Ϣ
 */
void oal_mem_print_pool_info(void)
{
    uint8_t uc_loop;

    for (uc_loop = 0; uc_loop <= OAL_MEM_POOL_ID_SHARED_DSCR; uc_loop++) {
        oal_mem_print_normal_pool_info(uc_loop);
    }
}
oal_module_symbol(oal_mem_print_pool_info);

void oal_mem_leak(oal_mem_pool_id_enum_uint8 en_pool_id)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_stru *pst_mem;
    oal_bool_enum_uint8 en_flag = OAL_TRUE;
    uint16_t us_loop;

    pst_mem_pool = oal_mem_get_pool(en_pool_id);
    if (oal_unlikely(pst_mem_pool == NULL)) {
        return;
    }

    pst_mem = pst_mem_pool->pst_mem_start_addr;

    for (us_loop = 0; us_loop < pst_mem_pool->us_mem_total_cnt; us_loop++) {
        /* ����к� != 0����˵�����ڴ�û�б��ͷ�(�������ڵ�0�������ڴ�) */
        if (pst_mem->alloc_line_num[0] != 0) {
            /* ��ӡ��ǰʱ��� */
            if (en_flag == OAL_TRUE) {
                oal_io_print("[memory leak] current time stamp: %u.\n", (uint32_t)oal_time_get_stamp_ms());
                en_flag = OAL_FALSE;
            }

            oal_io_print("[memory leak] user_cnt: %u, pool_id: %u, subpool_id: %u, len: %u, "
                         "alloc_core_id = %u, alloc_file_id: %u, alloc_line_num: %u, alloc_time_stamp: %u, "
                         "trace_file_id: %u, trace_line_num: %u, trace_time_stamp: %u.\n",
                         pst_mem->uc_user_cnt,
                         pst_mem->en_pool_id,
                         pst_mem->uc_subpool_id,
                         pst_mem->us_len,
                         pst_mem->alloc_core_id[0],
                         pst_mem->alloc_file_id[0],
                         pst_mem->alloc_line_num[0],
                         pst_mem->alloc_time_stamp[0],
                         pst_mem->trace_file_id,
                         pst_mem->trace_line_num,
                         pst_mem->trace_time_stamp);
        }

        /* ÿ���ڴ�����Ϣ����oal_mem_info�л��ϱ�ÿ���ӳص���Ϣ zouhongliang SDT */
        if (g_mempool_info.p_memblock_info_func != NULL) {
            g_mempool_info.p_memblock_info_func(pst_mem->puc_origin_data,
                                                pst_mem->uc_user_cnt,
                                                pst_mem->en_pool_id,
                                                pst_mem->uc_subpool_id,
                                                pst_mem->us_len,
                                                pst_mem->alloc_file_id[0],
                                                pst_mem->alloc_line_num[0]);
        }

        pst_mem++;
    }
}

/*
 * �� �� ��  : oal_mem_stat
 * ��������  : ͳ�Ƹ����ڴ��ʹ�����
 */
OAL_STATIC void oal_mem_statistics(oal_mem_pool_stat *past_mem_pool_stat)
{
    oal_mem_pool_id_enum_uint8 en_pool_id;
    uint8_t uc_subpool_id;
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;

    for (en_pool_id = 0; en_pool_id < OAL_MEM_POOL_ID_BUTT; en_pool_id++) {
        pst_mem_pool = &g_st_mem_pool[en_pool_id];

        /* ��¼���ڴ��ʹ������ */
        past_mem_pool_stat[en_pool_id].us_mem_used_cnt = pst_mem_pool->us_mem_used_cnt;
        past_mem_pool_stat[en_pool_id].us_mem_total_cnt = pst_mem_pool->us_mem_total_cnt;

        /* ��¼���ӳ�ʹ��״�� */
        for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
            pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[uc_subpool_id]);

            past_mem_pool_stat[en_pool_id].ast_subpool_stat[uc_subpool_id].us_free_cnt = pst_mem_subpool->us_free_cnt;
            past_mem_pool_stat[en_pool_id].ast_subpool_stat[uc_subpool_id].us_total_cnt = pst_mem_subpool->us_total_cnt;
        }
    }
}

/*
 * �� �� ��  : oal_mem_check
 * ��������  : ����ڴ���Ƿ���й©
 */
OAL_STATIC uint32_t oal_mem_check(oal_mem_pool_stat *past_stat_start, oal_mem_pool_stat *past_stat_end)
{
    uint8_t uc_bitmap = 0;
    oal_mem_pool_id_enum_uint8 en_pool_id;
    uint8_t uc_subpool_id;
    oal_mem_pool_stru *pst_mem_pool;

    for (en_pool_id = 0; en_pool_id < OAL_MEM_POOL_ID_BUTT; en_pool_id++) {
        /* �鿴���ڴ������(����ͳ�ƽ���Ƿ�һ��) */
        if ((past_stat_start[en_pool_id].us_mem_used_cnt != past_stat_end[en_pool_id].us_mem_used_cnt) ||
            (past_stat_start[en_pool_id].us_mem_total_cnt != past_stat_end[en_pool_id].us_mem_total_cnt)) {
            uc_bitmap |= (uint8_t)(1 << en_pool_id);
            continue;
        }

        pst_mem_pool = &g_st_mem_pool[en_pool_id];

        /* �鿴�����ڴ��ʹ��״��(����ͳ�ƽ���Ƿ�һ��) */
        for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
            if ((past_stat_start[en_pool_id].ast_subpool_stat[uc_subpool_id].us_free_cnt !=
                 past_stat_end[en_pool_id].ast_subpool_stat[uc_subpool_id].us_free_cnt) ||
                (past_stat_start[en_pool_id].ast_subpool_stat[uc_subpool_id].us_total_cnt !=
                 past_stat_end[en_pool_id].ast_subpool_stat[uc_subpool_id].us_total_cnt)) {
                uc_bitmap |= (uint8_t)(1 << en_pool_id);
                continue;
            }
        }
    }

    /* ����ͳ�ƽ��һ��(û���ڴ�й©)������ */
    if (uc_bitmap == 0) {
        oal_io_print("no memory leak!\n");
        return OAL_FALSE;
    }

    /* ����ͳ�ƽ����һ��(���ڴ�й©)����ӡ��й©���ڴ�ص�ͳ����Ϣ */
    oal_io_print("memory leak!\n");
    for (en_pool_id = 0; en_pool_id < OAL_MEM_POOL_ID_BUTT; en_pool_id++) {
        if (!(uc_bitmap & (1 << en_pool_id))) {
            continue;
        }

        pst_mem_pool = &g_st_mem_pool[en_pool_id];

        oal_io_print("                      Start\t\tEnd\t\n");
        oal_io_print("pool(%d) used cnt:     %d\t\t%d\t\n",
                     en_pool_id, past_stat_start[en_pool_id].us_mem_used_cnt,
                     past_stat_end[en_pool_id].us_mem_used_cnt);
        oal_io_print("pool(%d) total cnt:    %d\t\t%d\t\n",
                     en_pool_id, past_stat_start[en_pool_id].us_mem_total_cnt,
                     past_stat_end[en_pool_id].us_mem_total_cnt);

        for (uc_subpool_id = 0; uc_subpool_id < pst_mem_pool->uc_subpool_cnt; uc_subpool_id++) {
            oal_io_print("subpool(%d) free cnt:  %d\t\t%d\t\n",
                         uc_subpool_id, past_stat_start[en_pool_id].ast_subpool_stat[uc_subpool_id].us_free_cnt,
                         past_stat_end[en_pool_id].ast_subpool_stat[uc_subpool_id].us_free_cnt);
            oal_io_print("subpool(%d) total cnt: %d\t\t%d\t\n",
                         uc_subpool_id, past_stat_start[en_pool_id].ast_subpool_stat[uc_subpool_id].us_total_cnt,
                         past_stat_end[en_pool_id].ast_subpool_stat[uc_subpool_id].us_total_cnt);
        }

        oal_io_print("\n");
    }

    return OAL_TRUE;
}

/*
 * �� �� ��  : oal_mem_start_stat
 * ��������  : �ڴ���ӿ�(��Ҫ��oal_mem_end_stat���ʹ��)
 */
void oal_mem_start_stat(void)
{
    memset_s(&g_mem_stat, sizeof(g_mem_stat), 0, sizeof(g_mem_stat));

    /* ��¼���ڴ��ʹ��״�� */
    oal_mem_statistics(g_mem_stat.ast_mem_start_stat);
}

/*
 * �� �� ��  : oal_mem_end_stat
 * ��������  : �ڴ���ӿ�(��Ҫ��oal_mem_start_stat���ʹ��)
 * �� �� ֵ  : OAL_TRUE:  ���ڴ�й©
 *             OAL_FALSE: ���ڴ�й©
 */
uint32_t oal_mem_end_stat(void)
{
    oal_mem_statistics(g_mem_stat.ast_mem_end_stat);

    /* ����ڴ���Ƿ���й© */
    return oal_mem_check(g_mem_stat.ast_mem_start_stat, g_mem_stat.ast_mem_end_stat);
}

uint32_t oal_mem_trace_enhanced(uint32_t file_id,
                                uint32_t line_num,
                                oal_mem_stru *pst_mem,
                                uint8_t uc_lock)
{
    oal_mem_pool_stru *pst_mem_pool;
    oal_mem_subpool_stru *pst_mem_subpool;
    oal_uint flag = 0;

    if (oal_unlikely(pst_mem == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mem_pool = &g_st_mem_pool[pst_mem->en_pool_id];

    pst_mem_subpool = &(pst_mem_pool->ast_subpool_table[pst_mem->uc_subpool_id]);

    oal_mem_spin_lock_irqsave(pst_mem_subpool->st_spinlock, flag)

    pst_mem->trace_file_id = file_id;
    pst_mem->trace_line_num = line_num;
    pst_mem->trace_time_stamp = (uint32_t)oal_time_get_stamp_ms();

    oal_mem_spin_unlock_irqrestore(pst_mem_subpool->st_spinlock, flag)

    return OAL_SUCC;
}

uint32_t oal_mem_trace(uint32_t file_id,
                       uint32_t line_num,
                       const void *p_data,
                       uint8_t uc_lock)
{
    oal_mem_stru *pst_mem;

    if (oal_unlikely(p_data == NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mem = (oal_mem_stru *)(*((uintptr_t *)((uintptr_t)p_data - OAL_MEM_INFO_SIZE)));

    return oal_mem_trace_enhanced(file_id, line_num, pst_mem, uc_lock);
}

/*
 * �� �� ��  : oal_mem_exit
 * ��������  : �ڴ�ģ��ж�ؽӿ�
 */
void oal_mem_exit(void)
{
    /* ж����ͨ�ڴ�� */
    oal_mem_release();

    oal_mem_sdt_netbuf_release();

    memset_s(g_st_mem_pool, sizeof(g_st_mem_pool), 0, sizeof(g_st_mem_pool));

    return;
}

/*lint -e19*/
oal_module_symbol(oal_mem_free);
oal_module_symbol(oal_mem_free_array);

oal_module_symbol(oal_mem_alloc);

/*lint -e19*/
oal_module_symbol(oal_netbuf_duplicate);
oal_module_symbol(oal_mem_alloc_enhanced);
oal_module_symbol(oal_mem_free_enhanced);
oal_module_symbol(oal_mem_incr_user);
oal_module_symbol(oal_mem_sdt_netbuf_alloc);
oal_module_symbol(oal_mem_sdt_netbuf_free);
oal_module_symbol(oal_mem_leak);
oal_module_symbol(oal_mem_info);
oal_module_symbol(oal_mem_trace_enhanced);
oal_module_symbol(oal_mem_trace);
oal_module_symbol(oal_mempool_info_to_sdt_register);
oal_module_symbol(g_sdt_netbuf_base_addr);
#ifdef _PRE_WLAN_CACHE_COHERENT_SUPPORT
oal_module_symbol(g_dscr_fstvirt_addr);
oal_module_symbol(g_dscr_fstphy_addr);
#endif
