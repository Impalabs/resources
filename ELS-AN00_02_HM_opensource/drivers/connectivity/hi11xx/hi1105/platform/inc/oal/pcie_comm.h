

#ifndef __PCIE_COMM__
#define __PCIE_COMM__

/* feature macro define */
#include "pcie_edma.h"
#include "oal_util.h"

#define PCIE_EP_PHY_RW_TIMEOUT_CNT   100 /* ͨ��PCIE MEM�ڶ���ʱ���� */
#define PCIE_EP_PHY_SUP_ADDR_END     0x36
#define PCIE_EP_PHY_LANEN_ADDR_BEGIN 0x1000
#define PCIE_EP_PHY_LANEN_ADDR_END   0x1042

#define PCIE_PHY_LANEN_DIG_RX_OVRD_IN_LO_OFF 0x1005
#define PCIE_DEVICE_ID_1103                  0x1103
#define PCIE_DEVICE_ID_1105                  0X1105

#define PCIE_EDMA_FIFO_DEPTH 4 /* fifo ���Ϊ4 */

#define PCIE_EDMA_WRITE_RINGBUF_SIZE 16 /* refer to pcie_write_ringbuf_item */
#define PCIE_EDMA_READ_RINGBUF_SIZE  16 /* refer to pcie_read_ringbuf_item */

#define PCIE_EDMA_READ_BUSRT_COUNT  32 /* featch read buff every time */
#define PCIE_EDMA_WRITE_BUSRT_COUNT 32 /* featch read buff every time */

#define PCIE_EDMA_ELEMENTS_EACH_BUFF 2 /* how many elements each netbuf used!(mac header 64B + payload) */

#define PCIE_EDMA_TRANS_MAX_FRAME_LEN 1560

#if (PCIE_EDMA_TRANS_MAX_FRAME_LEN % 4)
#error PCIe dma element size must align to 4 bytes
#endif
#define PCIE_EDMA_FIFO_ELE_CNT (PCIE_EDMA_FIFO_DEPTH + 1 - 2)

/* Ringbuf ���ܴ�С ������2��N�η�! */
#define PCIE_EDMA_WRITE_RINGBUF_ITEMS       ((PCIE_EDMA_FIFO_DEPTH)*PCIE_EDMA_READ_BUSRT_COUNT)  /* 4*32*16=2048B */
#define PCIE_EDMA_READ_NORMAL_RINGBUF_ITEMS (PCIE_EDMA_FIFO_DEPTH * PCIE_EDMA_WRITE_BUSRT_COUNT) /* 4*32*16=2048B */
/*
 * ÿ�����б��뵥�����䣬�ߵ����ȼ����ܻ�Ϸ���(Host����)��
 * ������ʽ, �����ȼ�����Ӧ�ñ����ȵ��ȣ����ָߵ����ȼ�����Ϊ
 * ҵ�����󣬷����ð������ݰ�
 */
#define PCIE_EDMA_READ_HIGH_RINGBUF_ITEMS (32 * 2) /* �����ȼ�����Ҫ�ܸߵ����, 32*16=512B */

/* 1 for linked list element, 2 for two channels */
#ifdef _PRE_PLAT_FEATURE_PCIE_EDMA_FIFO
#define PCIE_EDMA_WRITE_MAX_ELEMENTS_EACH_CHAN (PCIE_EDMA_WRITE_BUSRT_COUNT * PCIE_EDMA_ELEMENTS_EACH_BUFF / \
                                                PCIE_EDMA_MAX_CHANNELS + 1)
#define PCIE_EDMA_READ_MAX_ELEMENTS_EACH_CHAN  (PCIE_EDMA_READ_BUSRT_COUNT * PCIE_EDMA_ELEMENTS_EACH_BUFF / \
                                                PCIE_EDMA_MAX_CHANNELS + 1)
#define PCIE_EDMA_WRITE_CHAN_MAX_ELEMENTS      (PCIE_EDMA_WRITE_MAX_ELEMENTS_EACH_CHAN * PCIE_EDMA_MAX_CHANNELS * \
                                                PCIE_EDMA_FIFO_ELE_CNT)
#define PCIE_EDMA_READ_CHAN_MAX_ELEMENTS       (PCIE_EDMA_READ_MAX_ELEMENTS_EACH_CHAN * PCIE_EDMA_MAX_CHANNELS * \
                                                PCIE_EDMA_FIFO_ELE_CNT)
#else
#define PCIE_EDMA_WRITE_CHAN_MAX_ELEMENTS (PCIE_EDMA_WRITE_BUSRT_COUNT * PCIE_EDMA_ELEMENTS_EACH_BUFF + 1)
#define PCIE_EDMA_READ_CHAN_MAX_ELEMENTS  (PCIE_EDMA_READ_BUSRT_COUNT * PCIE_EDMA_ELEMENTS_EACH_BUFF + 1)
#endif

typedef enum _pcie_h2d_message_type_ {
    PCIE_H2D_MESSAGE_HOST_READY = 17, /* ǰ0~16 Ԥ����BUS���� */
    PCIE_H2D_MESSAGE_CHIP_TEST_OUTBOUND = 18
} pcie_h2d_message_type;

typedef enum _pcie_d2h_message_type_ {
    PCIE_D2H_MESSAGE_HOST_READY_ACK = 17, /* ǰ0~16 Ԥ����BUS���� */
} pcie_d2h_message_type;

typedef enum _pcie_shared_device_addr_type_ {
    PCIE_SHARED_ADDR_SCHED_CNT = 0, /* control pcie sched mips */
    PCIE_SHARED_TX_SCHED_CNT = 1,
    PCIE_SHARED_SOFT_FIFO_ENABLE = 2,
    PCIE_SHARED_SOFT_DMA_CHECK = 3,
    PCIE_SHARED_RINGBUF_BUGFIX = 4,
    PCIE_SHARED_ASPM_DYNC_CTL = 5, /* control dma transfer aspm enable */
    PCIE_SHARED_ADDR_BUTT = 16
} pcie_shared_device_addr_type;

typedef enum _pcie_comm_ringbuf_type_ {
    PCIE_COMM_RINGBUF_DMA_WRITE_FIFO = 4,
    PCIE_COMM_RINGBUF_DMA_READ_FIFO,
    PCIE_COMM_RINGBUF_BUTT
} pcie_comm_ringbuf_type;

/* Ring Buff */
typedef union _edma_paddr_t_ {
    struct {
        uint32_t low_addr;
        uint32_t high_addr;
    } bits;
    uint64_t addr;
} edma_paddr_t;

typedef enum _pcie_h2d_ringbuf_qtype_ {
    PCIE_H2D_QTYPE_HIGH = 0,   /* ʹ��DEVICE �����ȼ��ڴ棬Ԥ����5Ƭ�������ȼ� */
    PCIE_H2D_QTYPE_NORMAL = 1, /* ʹ����ͨ���ȼ��ڴ� */
    PCIE_H2D_QTYPE_BUTT
} pcie_h2d_ringbuf_qtype;

/* Ring-Buff */
typedef struct _pcie_ringbuf_ {
    /* 32�ֽڣ�CPU cache line(32λϵͳΪ32�ֽڣ�64λϵͳΪ64�ֽڣ�����Ҫ������) */
    /* ����������ֻ��һ����дһ���˶� */
    uint32_t idx;       /* ring id, used for future */
    uint32_t reserv0;   /* reserv0 */
    uint32_t rd;        /* ������, ��ָ�� */
    uint32_t wr;        /* ������,дָ�� */
    uint32_t size;      /* maximum number of items in ring */
    uint16_t item_len;  /* length of each item in the ring */
    uint16_t item_mask; /* item_len Ϊ2��N�η�ʱʹ�� */
    uint64_t base_addr; /* ����64 bit��ַ��device �õ�32bit */
} pcie_ringbuf;

/* 16 Bytes */
typedef struct _pcie_write_ringbuf_item_ {
    edma_paddr_t buff_paddr; /* rc's ddr address(64 bit) */
} pcie_write_ringbuf_item;

typedef struct _pcie_read_ringbuf_item_ {
    uint32_t buf_len;
    uint32_t reserved0;
    edma_paddr_t buff_paddr; /* rc's ddr address(64 bit) */
} pcie_read_ringbuf_item;

#define PCIE_H2D_MESSAGE_MAX_ELEMENTS 32
#define PCIE_H2D_MESSAGE_ELEMENT_SIZE 4

#define PCIE_D2H_MESSAGE_MAX_ELEMENTS 32
#define PCIE_D2H_MESSAGE_ELEMENT_SIZE 4

typedef struct _pcie_ringbuf_res_ {
    pcie_ringbuf st_d2h_buf;                      /* write buff */
    pcie_ringbuf st_h2d_buf[PCIE_H2D_QTYPE_BUTT]; /* read buff */
    pcie_ringbuf st_d2h_msg;                      /* dev wakeup host gpio trigger */
    pcie_ringbuf st_h2d_msg;                      /* host2dev pcie reg triigger */
    pcie_ringbuf st_ringbuf[PCIE_COMM_RINGBUF_BUTT];
} pcie_ringbuf_res;

typedef struct _pcie_dma_write_fifo_item_ {
    uint32_t address; /* Just low 32 bits,used for check */
    uint16_t isr_count;
    uint8_t count[PCIE_EDMA_MAX_CHANNELS]; /* chan0 & chan1 */
} pcie_dma_write_fifo_item;

typedef struct _pcie_dma_read_fifo_item_ {
    uint16_t address; /* Just low 16 bits,used for check */
    uint16_t qid;     /* qid */
    uint16_t isr_count;
    uint8_t count[PCIE_EDMA_MAX_CHANNELS]; /* chan0 & chan1 */
} pcie_dma_read_fifo_item;

typedef struct _pcie_h2d_errors_ {
    uint8_t ringbuf_underflow_count;  /* RingbufΪ��,������� */
    uint8_t get_fifo_worklist_failed; /* д����ж����ˣ����û����ɵ������� */
    uint8_t count_reg_error;          /* count�Ĵ���ֵ�Ƿ� */
    uint8_t invaild_fifo_value;       /* ��Ч������ж�FIFO��Ϣ */
    uint8_t fifo_empty_error;         /* δȡ����Ч���� */
    uint8_t reserved[3];
    pcie_tx_abort_status abort_status;
} pcie_h2d_errors;

/* edma read ά��ͳ�� */
typedef struct _pcie_h2d_stat_ {
    uint32_t push_fifo_count;    /* ѹfifo���� */
    uint32_t done_isr_count;     /* ������жϼ��� */
    uint32_t doorbell_isr_count; /* �������жϼ��� */
    uint32_t trans_count;        /* ����ɵİ����� */
    uint32_t rx_count;           /* �����յ�netbuf���� */
    uint32_t rx_queue_count[PCIE_H2D_QTYPE_BUTT];

    uint32_t ringbuf_miss;
    uint32_t ringbuf_hit;
    uint32_t netbuf_miss;
    uint32_t netbuf_hit;
    uint32_t fifo_full;
    uint32_t fifo_notfull;
    uint32_t fifo_ele_empty;
    uint32_t soft_fifo_busy;

    uint32_t dma_work_list_stat;
    uint32_t dma_free_list_stat;
    uint32_t dma_pending_list_stat;

    uint32_t pm_busy_cnt;
} pcie_h2d_stat;

typedef struct _pcie_h2d_stats_ {
    pcie_h2d_stat stat;
    pcie_h2d_errors errors;
} pcie_h2d_stats;

/* edma write ά��ͳ�� */
typedef struct _pcie_d2h_stat_ {
    uint32_t push_fifo_count;    /* ѹfifo���� */
    uint32_t done_isr_count;     /* д����жϼ��� */
    uint32_t doorbell_isr_count; /* д�����жϼ��� */
    uint32_t trans_count;        /* ��ɷ��͵İ����� */
    uint32_t tx_count;           /* �����͵�netbuf���� */

    uint32_t ringbuf_miss;
    uint32_t ringbuf_hit;
    uint32_t fifo_full;
    uint32_t fifo_notfull;
    uint32_t fifo_ele_empty;
    uint32_t fifo_dma_idle;
    uint32_t fifo_dma_busy;
    uint32_t soft_fifo_busy;

    uint32_t dma_work_list_stat;
    uint32_t dma_free_list_stat;
    uint32_t dma_pending_list_stat;

    uint32_t pm_busy_cnt;
} pcie_d2h_stat;

/* edma write ����ͳ�� */
typedef struct _pcie_d2h_errors_ {
    uint8_t get_fifo_worklist_failed; /* д����ж����ˣ����û����ɵ������� */
    uint8_t count_reg_error;          /* count�Ĵ���ֵ�Ƿ� */
    uint8_t invaild_fifo_value;       /* ��Ч������ж�FIFO��Ϣ */
    uint8_t reserved;
    pcie_rx_abort_status abort_status;
} pcie_d2h_errors;

typedef struct _pcie_d2h_stats_ {
    pcie_d2h_stat stat;
    pcie_d2h_errors errors;
} pcie_d2h_stats;

typedef struct _pcie_stat_ {
    int32_t inited;              /* init flag, ��0��ʾʧ�ܣ�0��ʾ�ɹ����� δ��ʼ�� */
    uint32_t rst_cnt;            /* total reset count */
    uint32_t l3_isr_count;       /* L3 �жϴ������� */
    uint32_t msg_isr_count;      /* ��Ϣ�жϴ������� */
    uint32_t h2d_isr_count;      /* H2D�жϴ������� */
    uint32_t link_down_count;    /* PCIE�����ж� */
    uint32_t pm_gpio_sleep_cnt;  /* gpio low cnt */
    uint32_t pm_gpio_wakeup_cnt; /* gpio high cnt */
    uint32_t isr_per_max_cnt;    /* ���ж��ﴦ���жϴ������ļ��� */
    uint32_t msg_fifo_busy_cnt;
    uint32_t l1_wake_l1_hit;
    uint32_t l1_wake_l1_miss;
    uint32_t l1_wake_state_err_cnt;
    uint32_t l1_wake_timeout_cnt;
    uint32_t l1_wake_timeout_max_cnt;
    uint32_t l1_wake_force_push_cnt;
} pcie_stat;

typedef struct _pcie_stats_ {
    pcie_stat comm_stat;
    pcie_h2d_stats h2d_stats;
    pcie_d2h_stats d2h_stats;
} pcie_stats;

/* pcie rc/ep shared mem stru */
typedef struct _pcie_share_mem_stru_ {
    uint32_t ringbuf_res_paddr;
    uint32_t write_llt_paddr; /* llt address */
    uint32_t read_llt_paddr;
#ifdef _PRE_PLAT_FEATURE_PCIE_DEVICE_STAT
    uint32_t device_stat_paddr;
#endif
#ifdef _PRE_PLAT_FEATURE_PCIE_EDMA_ORI
    uint32_t write_chan_cnt_addr;
    uint32_t read_chan_cnt_addr;
#endif
    uint32_t device_addr[PCIE_SHARED_ADDR_BUTT];
} pcie_share_mem_stru;

/* after Hi1106 */
typedef enum _pcie_ip_nums_type_ {
    PCIE_IP_IDX_0 = 0,
    PCIE_IP_IDX_1 = 1,
    PCIE_IP_IDX_NUMS = 2
} pcie_ip_nums_type;

#endif
