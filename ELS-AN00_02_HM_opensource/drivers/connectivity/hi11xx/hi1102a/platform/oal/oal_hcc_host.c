

#define HI11XX_LOG_MODULE_NAME     "[HCC]"
#define HI11XX_LOG_MODULE_NAME_VAR hcc_loglevel
#define HISI_LOG_TAG               "[HCC]"

#include "oal_util.h"
#include "securec.h"

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
#include <linux/signal.h>
#endif
#endif

#include "oal_sdio_host_if.h"
#include "oal_hcc_host_if.h"
#include "oam_ext_if.h"

#ifdef CONFIG_MMC
#include "plat_pm_wlan.h"
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_OAL_HCC_HOST_C

#define HCC_TX_FLOW_HI_LEVEL 512
#define HCC_TX_FLOW_LO_LEVEL 128

#define HCC_TRANS_THREAD_POLICY   SCHED_FIFO
#define HCC_TRANS_THERAD_PRIORITY 10
#define HCC_TRANS_THERAD_NICE     (-10)

#define HCC_QUEUE_FC_HIGH_WATER_DROP_LINE    100

OAL_STATIC struct hcc_handler *g_hcc_tc = NULL;

OAL_STATIC oal_uint32 g_hcc_tx_max_buf_len = 4096;

OAL_STATIC oal_uint32 g_tcp_ack_wait_sche_cnt = 1;

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
struct custom_process_func_handler g_custom_process_func;
EXPORT_SYMBOL_GPL(g_custom_process_func);  //lint !e132 !e745 !e578
#endif

/* use lint -e16 to mask the error19! */
oal_uint32 g_pm_wifi_rxtx_count = 0; /* ��ƽ̨��ͳ�ƽ��շ������ݰ����� */
/*lint -e19 */
oal_module_symbol(g_pm_wifi_rxtx_count);
/*lint +e19 */
OAL_STATIC oal_uint32 g_hcc_assemble_count = 8;
oal_uint32 hcc_get_assemble_count(oal_void)
{
    return g_hcc_assemble_count;
}
oal_void hcc_set_assemble_count(oal_uint32 assemble_count)
{
    g_hcc_assemble_count = assemble_count;
}
/*lint -e19 */
oal_module_symbol(hcc_set_assemble_count);
/*lint +e19 */
/* 1 means hcc rx data process in hcc thread, 0 means process in sdio thread */
OAL_STATIC oal_uint32 g_hcc_rx_thread_enable = 1;
OAL_STATIC oal_uint32 g_hcc_credit_bottom_value = 2; /* 2��03/05��ֵ�������Ƕ��룬���ڸ����ȼ�netbuf�Ѹ�Ϊ5Ƭ�������� */
OAL_STATIC oal_uint32 g_hcc_flowctrl_detect_panic = 1;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_debug_module_param(g_hcc_tx_max_buf_len, uint, S_IRUGO | S_IWUSR);
oal_debug_module_param(g_hcc_assemble_count, uint, S_IRUGO | S_IWUSR);
oal_debug_module_param(g_hcc_rx_thread_enable, uint, S_IRUGO | S_IWUSR);
oal_debug_module_param(g_hcc_credit_bottom_value, uint, S_IRUGO | S_IWUSR);
oal_debug_module_param(g_hcc_flowctrl_detect_panic, uint, S_IRUGO | S_IWUSR);
#endif
oal_int32 hcc_send_rx_queue(struct hcc_handler *hcc, hcc_queue_type type);
oal_uint32 hcc_queues_flow_ctrl_len(struct hcc_handler *hcc, hcc_chan_type dir);
OAL_STATIC oal_void hcc_dev_flowctr_timer_del(struct hcc_handler *hcc);

oal_bool_enum_uint8 hcc_flowctl_get_device_mode(struct hcc_handler *hcc);
oal_void hcc_tx_network_start_subq(struct hcc_handler *hcc, oal_uint16 us_queue_idx);
oal_void hcc_tx_network_stop_subq(struct hcc_handler *hcc, oal_uint16 us_queue_idx);

oal_uint32 hcc_get_max_buf_len(oal_void)
{
    return g_hcc_tx_max_buf_len;
}

/* This is not good */
struct hcc_handler *hcc_get_110x_handler(oal_void)
{
    hcc_bus_dev *pst_bus_dev = hcc_get_bus_dev(HCC_CHIP_110X_DEV);
    if (oal_unlikely(pst_bus_dev == NULL)) {
        return NULL;
    }

    return pst_bus_dev->hcc;
}
/*lint -e19 */
oal_module_symbol(hcc_get_110x_handler);
/*lint +e19 */
OAL_STATIC oal_void hcc_tx_netbuf_destory(struct hcc_handler *hcc)
{
    if (oal_warn_on(hcc == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: hcc is null", __FUNCTION__);
        return;
    };

    oal_wake_unlock(&hcc->tx_wake_lock);
}

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
struct custom_process_func_handler *oal_get_custom_process_func(oal_void)
{
    return &g_custom_process_func;
}
/*lint -e19 */
oal_module_symbol(oal_get_custom_process_func);
/*lint +e19 */
#endif

OAL_STATIC oal_netbuf_stru *hcc_tx_assem_descr_get(struct hcc_handler *hcc)
{
    return oal_netbuf_delist(&hcc->tx_descr_info.tx_assem_descr_hdr);
}

OAL_STATIC oal_void hcc_tx_assem_descr_put(struct hcc_handler *hcc, oal_netbuf_stru *netbuf)
{
    oal_netbuf_list_tail(&hcc->tx_descr_info.tx_assem_descr_hdr, netbuf);
}

#ifdef CONFIG_MMC
oal_uint32 hcc_get_large_pkt_free_cnt(struct hcc_handler *hcc)
{
    return oal_sdio_get_large_pkt_free_cnt(oal_get_sdio_default_handler());
}
#endif

/* align_size must be power of 2 */
OAL_STATIC oal_netbuf_stru *hcc_netbuf_len_align(oal_netbuf_stru *netbuf,
                                                 oal_uint32 align_size)
{
    oal_int32 ret = OAL_SUCC;
    oal_uint32 len_algin, tail_room_len;
    oal_uint32 len = oal_netbuf_len(netbuf);
    if (oal_is_aligned(len, align_size)) {
        return netbuf;
    }
    /* align the netbuf */
    len_algin = OAL_ROUND_UP(len, align_size);
    if (oal_unlikely(len_algin < len)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: len_algin:%d < len:%d", __FUNCTION__, len_algin, len);
        return NULL;
    };

    tail_room_len = len_algin - len;

    if (oal_unlikely(tail_room_len > oal_netbuf_tailroom(netbuf))) {
        /* tailroom not enough */
        ret = oal_netbuf_expand_head(netbuf, 0, (oal_int32)tail_room_len, GFP_KERNEL);
        if (oal_warn_on(ret != OAL_SUCC)) {
            oal_print_hi11xx_log(HI11XX_LOG_WARN, "alloc head room failed,expand tail len=%d", tail_room_len);
            declare_dft_trace_key_info("netbuf align expand head fail", OAL_DFT_TRACE_FAIL);
            return NULL;
        }
    }

    oal_netbuf_put(netbuf, tail_room_len);

    if (oal_unlikely(!oal_is_aligned(oal_netbuf_len(netbuf), align_size))) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: netbuf len :%d not align %d",
                             __FUNCTION__, oal_netbuf_len(netbuf), align_size);
        return NULL;
    };

    return netbuf;
}

OAL_STATIC oal_int32 hcc_build_next_assem_descr_para_check(oal_netbuf_head_stru *next_assembled_head)
{
    if (oal_unlikely(!oal_netbuf_list_empty(next_assembled_head))) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: next_assembled_head is empty", __FUNCTION__);
        return OAL_FALSE;
    }
    return OAL_TRUE;
}

OAL_STATIC oal_void hcc_build_next_assem_descr(struct hcc_handler *hcc, hcc_queue_type type,
                                               oal_netbuf_head_stru *head, oal_netbuf_head_stru *next_assembled_head,
                                               oal_netbuf_stru *descr_netbuf, oal_uint32 remain_len)
{
    oal_int32 i = 0;
    oal_int32 len;
    oal_uint8 *buf = NULL;
    oal_netbuf_stru *netbuf = NULL;
    oal_netbuf_stru *netbuf_t = NULL;
    oal_uint32 assemble_max_count, queue_len, current_trans_len;

    buf = (oal_uint8 *)oal_netbuf_data(descr_netbuf);
    len = (oal_int32)oal_netbuf_len(descr_netbuf);

    if (hcc_build_next_assem_descr_para_check(next_assembled_head) != OAL_TRUE) {
        return;
    }

    assemble_max_count = oal_max(1, hcc_get_assemble_count());
    queue_len = oal_netbuf_list_len(head);
    current_trans_len = oal_min(queue_len, assemble_max_count);
    current_trans_len = oal_min(current_trans_len, remain_len);

    oal_print_hi11xx_log(HI11XX_LOG_DBG,
                         "build next descr, queue:[remain_len:%u][len:%u][trans_len:%u][max_assem_len:%u]",
                         remain_len, queue_len, current_trans_len, assemble_max_count);

    buf[0] = 0;

    if (current_trans_len == 0) {
        return;
    }

    for (;;) {
        /* move the netbuf from head queue to prepare-send queue, head->tail */
        netbuf = oal_netbuf_delist(head);
        if (netbuf == NULL) {
            oal_print_hi11xx_log(HI11XX_LOG_WARN, "why? this should never happaned! assem list len:%d",
                                 oal_netbuf_list_len(next_assembled_head));
            declare_dft_trace_key_info("buid assem error", OAL_DFT_TRACE_FAIL);
            break;
        }

        /* align the buff len to 32B */
        netbuf_t = hcc_netbuf_len_align(netbuf, HISDIO_H2D_SCATT_BUFFLEN_ALIGN);
        if (netbuf_t == NULL) {
            /* return to the list */
            oal_netbuf_addlist(head, netbuf);
            break;
        }

        current_trans_len--;

        oal_netbuf_list_tail(next_assembled_head, netbuf_t);
        if (oal_unlikely(i >= len)) {
            oal_print_hi11xx_log(HI11XX_LOG_ERR, "hcc tx scatt num :%d over buff len:%d,assem count:%u",
                                 i, len, hcc_get_assemble_count());
            break;
        }

        buf[i++] = (oal_uint8)(oal_netbuf_len(netbuf) >> HISDIO_H2D_SCATT_BUFFLEN_ALIGN_BITS);
        if (current_trans_len == 0) {
            /* send empty */
            if (i != len) {
                buf[i] = 0;
            }
            break;
        }
    }

    if (oal_likely(!oal_netbuf_list_empty(next_assembled_head))) {
        hcc->hcc_transer_info.tx_assem_info.assembled_queue_type = type;
    }
}

oal_void hcc_tx_network_stopall_queues(struct hcc_handler *hcc)
{
    if (oal_warn_on(hcc == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: hcc is null", __FUNCTION__);
        return;
    };
    if (oal_likely(hcc->hcc_transer_info.tx_flow_ctrl.net_stopall)) {
        hcc->hcc_transer_info.tx_flow_ctrl.net_stopall();
    }
}

oal_void hcc_tx_network_startall_queues(struct hcc_handler *hcc)
{
    if (oal_warn_on(hcc == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: hcc is null", __FUNCTION__);
        return;
    };

    if (oal_likely(hcc->hcc_transer_info.tx_flow_ctrl.net_startall)) {
        hcc->hcc_transer_info.tx_flow_ctrl.net_startall();
    }
}

oal_void hcc_tx_flow_ctrl_cb_register(flowctrl_cb stopall, flowctrl_cb startall)
{
    if (g_hcc_tc == NULL) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "[ERROR]hcc_tx_flow_ctrl_cb_register failed! g_hcc_tc is NULL");
        return;
    }
    g_hcc_tc->hcc_transer_info.tx_flow_ctrl.net_stopall = stopall;
    g_hcc_tc->hcc_transer_info.tx_flow_ctrl.net_startall = startall;
}

/*lint -e19 */
oal_module_symbol(hcc_tx_flow_ctrl_cb_register);
/*lint +e19 */
oal_void hcc_tx_wlan_queue_map_set(struct hcc_handler *hcc, hcc_queue_type hcc_queue_id,
                                   wlan_net_queue_type wlan_queue_id)
{
    hcc_trans_queue *pst_hcc_queue = NULL;
    if (oal_warn_on(hcc == NULL)) {
        return;
    }

    if (oal_warn_on(hcc_queue_id >= HCC_QUEUE_COUNT || wlan_queue_id >= WLAN_NET_QUEUE_BUTT)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "invaild param,hcc id:%d,wlan id:%d",
                             (oal_int32)hcc_queue_id, (oal_int32)wlan_queue_id);
        return;
    }
    pst_hcc_queue = &hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[hcc_queue_id];
    pst_hcc_queue->wlan_queue_id = wlan_queue_id;
}

/*lint -e19 */
oal_module_symbol(hcc_tx_wlan_queue_map_set);
/*lint +e19 */
oal_void hcc_msg_slave_thruput_bypass(oal_void)
{
    struct hcc_handler *hcc = hcc_get_110x_handler();
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "hcc_msg_slave_thruput_bypass.");

    hcc_bus_send_message(hcc_to_bus(hcc), H2D_MSG_HCC_SLAVE_THRUPUT_BYPASS);
}
#ifndef _PRE_PC_LINT
oal_module_symbol(hcc_msg_slave_thruput_bypass);
#endif

OAL_STATIC oal_uint32 hcc_check_header_vaild(struct hcc_header *hdr)
{
    if (oal_unlikely(hdr->main_type >= HCC_ACTION_TYPE_BUTT) ||
        (HCC_HDR_LEN + hdr->pad_hdr + hdr->pad_payload > HCC_HDR_TOTAL_LEN)) {
        declare_dft_trace_key_info("hcc_check_header_vaild_fail", OAL_DFT_TRACE_FAIL);
        return OAL_FALSE;
    }
    return OAL_TRUE;
}

OAL_STATIC oal_int32 hcc_tx_param_check(struct hcc_handler *hcc, oal_netbuf_stru *netbuf,
                                        struct hcc_transfer_param *param)
{
    if (oal_warn_on(hcc == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "HCC is null!\n");
        return -OAL_EINVAL;
    }

    if (oal_warn_on(netbuf == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "netbuf is null!\n");
        return -OAL_EINVAL;
    }

    if (oal_warn_on(param == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "param is null!\n");
        return -OAL_EINVAL;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (oal_unlikely(oal_atomic_read(&hcc->state) != HCC_ON)) {
        if (oal_atomic_read(&hcc->state) == HCC_OFF) {
            return -OAL_EBUSY;
        } else if (oal_atomic_read(&hcc->state) == HCC_EXCEPTION) {
            return -OAL_EIO;
        } else {
            oal_print_hi11xx_log(HI11XX_LOG_INFO, "invaild hcc state:%d", oal_atomic_read(&hcc->state));
            return -OAL_EINVAL;
        }
    }
#endif

    if (oal_unlikely(param->extend_len > hcc->hdr_rever_max_len)) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        WARN(1, "invaild reserved len %u ,never should over %zu\n",
             param->extend_len, HCC_HDR_RESERVED_MAX_LEN);
#endif
        /* can't free buf here */
        return -OAL_EINVAL;
    }

    if (oal_warn_on(param->main_type >= HCC_ACTION_TYPE_BUTT)) {
        oal_print_hi11xx_log(HI11XX_LOG_WARN, "wrong main type:%d", param->main_type);
        return -OAL_EINVAL;
    }

    if (oal_warn_on(param->fc_flag & (~HCC_FC_ALL))) {
        oal_print_hi11xx_log(HI11XX_LOG_WARN, "wrong fc_flag:%d", param->fc_flag);
        return -OAL_EINVAL;
    }

    if (oal_warn_on(param->queue_id >= HCC_QUEUE_COUNT)) {
        oal_print_hi11xx_log(HI11XX_LOG_WARN, "wrong queue_id:%d", param->queue_id);
        return -OAL_EINVAL;
    }

    return OAL_SUCC;
}

/*
 * Prototype    : hcc_tx
 * Description  : hcc tx function, the format like below:
 * Return Value : succ or fail
 */
oal_int32 hcc_tx(struct hcc_handler *hcc, oal_netbuf_stru *netbuf,
                 struct hcc_transfer_param *param)
{
    oal_uint32 queue_id;
    oal_int32 ret;
    oal_uint pad_payload, headroom,
             payload_len, payload_addr,
             pad_hdr;
    oal_uint8 *old_data_addr = NULL;
    oal_int32 headroom_add = 0;
    struct hcc_header *hdr = NULL;
    hcc_trans_queue *pst_hcc_queue = NULL;
    struct hcc_tx_cb_stru *pst_cb_stru = NULL;

    ret = hcc_tx_param_check(hcc, netbuf, param);
    if (oal_unlikely(ret != OAL_SUCC)) {
        return ret;
    }

    /*lint -e522*/
    oal_warn_on(oal_netbuf_len(netbuf) == 0);
    /*lint +e522*/
    queue_id = param->queue_id;
    pst_hcc_queue = &hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_id];

    if (oal_warn_on((param->fc_flag & HCC_FC_WAIT) && oal_in_interrupt())) {
        /* when in interrupt,can't sched! */
        param->fc_flag &= ~HCC_FC_WAIT;
    }

    if (param->fc_flag) {
        /* flow control process */
        /* Block if fc */
        if (HCC_FC_WAIT & param->fc_flag) {
            /*lint -e730*/
            ret = oal_wait_event_interruptible_timeout(hcc->hcc_transer_info.tx_flow_ctrl.wait_queue,
                                                       (oal_netbuf_list_len(&pst_hcc_queue->data_queue) <
                                                       pst_hcc_queue->flow_ctrl.low_waterline),
                                                       OAL_SEC_PER_MIN * OAL_TIME_HZ);
            if (ret == 0) {
                oal_print_hi11xx_log(HI11XX_LOG_ERR, "[WARN]hcc flow control wait event timeout! too much time locked");
                declare_dft_trace_key_info("hcc flow control wait timeout", OAL_DFT_TRACE_FAIL);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
                hcc_print_current_trans_info();
#endif
            } else if (ret == -ERESTARTSYS) {
                oal_print_hi11xx_log(HI11XX_LOG_WARN, "wifi task was interrupted by a signal");
                return -OAL_EFAIL;
            }
        }

        /* control net layer if fc */
        if (HCC_FC_NET & param->fc_flag) {
            /* net layer?? */
            oal_spin_lock_bh(&hcc->hcc_transer_info.tx_flow_ctrl.lock);

            if ((oal_netbuf_list_len(&pst_hcc_queue->data_queue) > pst_hcc_queue->flow_ctrl.high_waterline) &&
                (pst_hcc_queue->flow_ctrl.is_stopped == OAL_FALSE)) {
                hcc_tx_network_stop_subq(hcc, (oal_uint16)pst_hcc_queue->wlan_queue_id);
                pst_hcc_queue->flow_ctrl.is_stopped = OAL_TRUE;
            }

            oal_spin_unlock_bh(&hcc->hcc_transer_info.tx_flow_ctrl.lock);
        }

        /* control net layer if fc */
        if (HCC_FC_DROP & param->fc_flag) {
            /* 10 netbufs to buff */
            if (oal_netbuf_list_len(&pst_hcc_queue->data_queue) >
                                    pst_hcc_queue->flow_ctrl.high_waterline + HCC_QUEUE_FC_HIGH_WATER_DROP_LINE) {
                pst_hcc_queue->loss_pkts++;
                /* drop the netbuf */
                oal_netbuf_free(netbuf);
                return OAL_SUCC;
            }
        }
    }

    /* 64B + 4B equal head, tail equal 512 */
    /* only payload data */
    payload_len = oal_netbuf_len(netbuf) - param->extend_len;

    old_data_addr = oal_netbuf_data(netbuf);
    payload_addr = (uintptr_t)oal_netbuf_data(netbuf) + param->extend_len;

    /* if pad not 0, we must copy the extend data */
    pad_payload = payload_addr - OAL_ROUND_DOWN(payload_addr, 4); /* �����2bit����֤4�ֽڶ��� */
    /* should be 1 byte */
    pad_hdr = HCC_HDR_RESERVED_MAX_LEN - param->extend_len;

    headroom = HCC_HDR_LEN + pad_hdr + pad_payload;

    if (headroom > oal_netbuf_headroom(netbuf)) {
        headroom_add = (oal_int32)(headroom - oal_netbuf_headroom(netbuf));
    }

    if (headroom_add) {
        /* relloc the netbuf */
        ret = oal_netbuf_expand_head(netbuf, headroom_add, 0, GFP_ATOMIC);
        if (oal_unlikely(ret != OAL_SUCC)) {
            oal_print_hi11xx_log(HI11XX_LOG_WARN, "alloc head room failed,netbuf len is %d,expand len:%d\n",
                                 oal_netbuf_len(netbuf), headroom_add);
            return -OAL_EFAIL;
        }
        old_data_addr = oal_netbuf_data(netbuf);
    }

    hdr = (struct hcc_header *)oal_netbuf_push(netbuf, headroom);
    if (oal_unlikely(!oal_is_aligned((uintptr_t)hdr, 4))) {  /* �ж��ǲ���4�ֽڶ��� */
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "hdr:%ld not aligned len:%d", (uintptr_t)hdr, 4);
        return -OAL_EFAIL;
    };

    if (pad_payload > 0) {
        /* algin the extend data */
        if (memmove_s(old_data_addr - pad_payload, pad_payload + payload_len + param->extend_len,
                      old_data_addr, param->extend_len) != EOK) {
            oal_print_hi11xx_log(HI11XX_LOG_ERR, "memmove_s: align the extend data failed.\n");
        }
    }

    /* fill the hcc header */
    hdr->pad_payload = (oal_uint16)pad_payload;
    hdr->pay_len = (oal_uint16)payload_len;
    hdr->seq = ((oal_uint32)(oal_atomic_inc_return(&hcc->tx_seq))) & 0xFF;
    ;
    hdr->main_type = (oal_uint8)param->main_type;
    hdr->sub_type = (oal_uint8)param->sub_type;
    hdr->pad_hdr = (oal_uint8)pad_hdr;
    hdr->more = 0;
    hdr->option = 0;

    if (oal_unlikely(hdr->pad_hdr + HCC_HDR_LEN > HCC_HDR_TOTAL_LEN)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "hdr->pad_hdr + HCC_HDR_LEN:%lu > len:%d",
                             hdr->pad_hdr + HCC_HDR_LEN, HCC_HDR_TOTAL_LEN);
        return -OAL_EFAIL;
    };

    if (oal_warn_on(hdr->pay_len > g_hcc_tx_max_buf_len)) {
        /* pay_len����DEVICE ����ڴ泤�� */
        oal_print_hi11xx_log(HI11XX_LOG_ERR,
                             "[ERROR]main:%d, sub:%d,pay len:%d,netbuf len:%d, extend len:%d,pad_payload:%d,max len:%u",
                             hdr->main_type,
                             hdr->sub_type,
                             hdr->pay_len,
                             hdr->pay_len + param->extend_len,
                             param->extend_len,
                             hdr->pad_payload,
                             g_hcc_tx_max_buf_len);
        declare_dft_trace_key_info("hcc_tx_check_param_fail", OAL_DFT_TRACE_FAIL);
        return -OAL_EINVAL;
    } else {
        /* ������ ��1544 + [1~3] ֮�ڣ� ��������,��payload �ڴ�ǰ�� 1~3B����ʡDEVICE�ڴ� */
        if (hdr->pad_payload + hdr->pay_len > g_hcc_tx_max_buf_len) {
            oal_uint8 *pst_dst = (oal_uint8 *)hdr + HCC_HDR_TOTAL_LEN;
            oal_uint8 *pst_src = pst_dst + hdr->pad_payload;
            if (memmove_s((oal_void *)pst_dst, hdr->pay_len + hdr->pad_payload,
                          (const oal_void *)pst_src, hdr->pay_len) != EOK) {
                oal_print_hi11xx_log(HI11XX_LOG_ERR, "memmove_s: move the payload forward failed\n");
            }
            oal_netbuf_trim(netbuf, hdr->pad_payload);
            hdr->pad_payload = 0; /* after memmove,the pad is 0 */
            declare_dft_trace_key_info("hcc_tx_check_memmove", OAL_DFT_TRACE_SUCC);
        }
    }

    if (oal_warn_on(OAL_ROUND_UP(oal_netbuf_len(netbuf), HISDIO_H2D_SCATT_BUFFLEN_ALIGN) >
                    g_hcc_tx_max_buf_len + HCC_HDR_TOTAL_LEN)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "[E]Fatal error,netbuf's len:%d over the payload:%d",
                             OAL_ROUND_UP(oal_netbuf_len(netbuf), HISDIO_H2D_SCATT_BUFFLEN_ALIGN),
                             g_hcc_tx_max_buf_len + HCC_HDR_TOTAL_LEN);
        declare_dft_trace_key_info("hcc_tx_check_algin_fail", OAL_DFT_TRACE_FAIL);
        return -OAL_EINVAL;
    }

#ifdef CONFIG_HCC_DEBUG
    oal_print_hi11xx_log(HI11XX_LOG_VERBOSE, "hcc_tx into queue:%d, main:%d, sub:%d",
                         queue_id, param->main_type, param->sub_type);
    oal_print_hi11xx_log(HI11XX_LOG_VERBOSE, "hcc tx pkt seq:%d", hdr->seq);
    oal_print_hex_dump(oal_netbuf_data(netbuf), HCC_HDR_TOTAL_LEN, HEX_DUMP_GROUP_SIZE, "hcc hdr");
    oal_print_hex_dump(oal_netbuf_data(netbuf) + HCC_HDR_TOTAL_LEN + hdr->pad_payload,
                       hdr->pay_len, HEX_DUMP_GROUP_SIZE, "hcc payload");
#endif

    /* wakelock,one netbuf one lock */
    oal_wake_lock(&hcc->tx_wake_lock);

    pst_cb_stru = (struct hcc_tx_cb_stru *)oal_netbuf_cb(netbuf);
    pst_cb_stru->destory = hcc_tx_netbuf_destory;
    pst_cb_stru->magic = HCC_TX_WAKELOCK_MAGIC;
    pst_cb_stru->qtype = queue_id;

    oal_netbuf_list_tail(&pst_hcc_queue->data_queue, netbuf);

    hcc_sched_transfer(hcc);

    return OAL_SUCC;
}
/*lint -e19 */
oal_module_symbol(hcc_tx);
/*lint +e19 */
/*
 * Prototype    : hcc_transfer_rx_handler
 * Description  : hcc tx function, the format like below:
 */
OAL_STATIC oal_void hcc_transfer_rx_handler(struct hcc_handler *hcc, oal_netbuf_stru *netbuf)
{
    /* get the rx buf and enqueue */
    oal_netbuf_list_tail(&hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[DATA_LO_QUEUE].data_queue,
                         netbuf);
    if (g_hcc_rx_thread_enable == 0) {
        hcc_send_rx_queue(hcc, DATA_LO_QUEUE);
    }
}

/*
 * Prototype    : hcc_transfer_rx_handler_replace
 * Description  : hcc tx function, the format like below:
 */
OAL_STATIC oal_void hcc_transfer_rx_handler_replace(struct hcc_handler *hcc, oal_netbuf_stru *pst_netbuf_new)
{
    oal_netbuf_stru *pst_netbuf_old;
    hcc_trans_queue *pst_hcc_queue = &hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[DATA_LO_QUEUE];

    /* remove from head */
    pst_netbuf_old = oal_netbuf_delist(&pst_hcc_queue->data_queue);
    if (oal_likely(pst_netbuf_old != NULL)) {
        pst_hcc_queue->loss_pkts++;
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "hcc_rx loss pkts :%u", pst_hcc_queue->loss_pkts);
        oal_netbuf_free(pst_netbuf_old);
    }

    /* get the rx buf and enqueue */
    oal_netbuf_list_tail(&pst_hcc_queue->data_queue,
                         pst_netbuf_new);

    if (g_hcc_rx_thread_enable == 0) {
        hcc_send_rx_queue(hcc, DATA_LO_QUEUE);
    }
}

/*
 * Prototype    : hcc_bus_rx_handler
 * Description  : hcc tx function, the format like below:
 * Return Value : succ or fail
 */
oal_int32 hcc_bus_rx_handler(oal_void *data)
{
    oal_int32 ret;
    oal_uint32 scatt_count;
    oal_netbuf_stru *netbuf = NULL;
    oal_netbuf_head_stru head;
    struct hcc_handler *hcc = (struct hcc_handler *)data;
    struct hcc_header *pst_hcc_head = NULL;

    if (oal_warn_on(!hcc)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "hcc is null:%s\n", __FUNCTION__);
        return -OAL_EINVAL;
    };

    oal_netbuf_head_init(&head);

    /* ����bus�ײ��ʵ�崦��ӿڻ�ȡnetbuf list */
    ret = hcc_bus_rx_netbuf_list(hcc_to_bus(hcc), &head);
    if (oal_unlikely(ret != OAL_SUCC)) {
        return ret;
    }

    scatt_count = oal_netbuf_list_len(&head);
    if (oal_unlikely(scatt_count >= HCC_RX_ASSEM_INFO_MAX_NUM)) {
        oal_print_hi11xx_log(HI11XX_LOG_WARN, "WARN:why so much scatt buffs[%u]?", scatt_count);
        scatt_count = 0;
    }

    hcc->hcc_transer_info.rx_assem_info.info[scatt_count]++;

    for (;;) {
        netbuf = oal_netbuf_delist(&head);
        if (netbuf == NULL) {
            break;
        }

        /* RX ���أ�����������������ʱ������ɵ����ݰ�,SDIO��ȥ��DEVICE����ס */
        pst_hcc_head = (struct hcc_header *)oal_netbuf_data(netbuf);
        if (oal_unlikely(hcc_check_header_vaild(pst_hcc_head) != OAL_TRUE)) {
            oal_print_hex_dump((oal_uint8 *)pst_hcc_head, HCC_HDR_TOTAL_LEN, HEX_DUMP_GROUP_SIZE,
                               "invaild hcc header: ");
            hcc_bus_try_to_dump_device_mem(hcc_get_current_110x_bus(), OAL_FALSE);
        }

        if (hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[DATA_LO_QUEUE].flow_ctrl.enable == OAL_TRUE) {
            if (oal_netbuf_list_len(&hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[DATA_LO_QUEUE].data_queue) >
                hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[DATA_LO_QUEUE].flow_ctrl.high_waterline) {
                hcc_transfer_rx_handler_replace(hcc, netbuf);
            } else {
                hcc_transfer_rx_handler(hcc, netbuf);
            }
        } else {
            hcc_transfer_rx_handler(hcc, netbuf);
        }
    }

    /* sched hcc thread */
    if (g_hcc_rx_thread_enable == 1) {
        hcc_sched_transfer(hcc);
    }
#ifdef CONFIG_HCC_DEBUG
#endif
    return OAL_SUCC;
}

oal_void hcc_rx_submit(struct hcc_handler *hcc, oal_netbuf_stru *pst_netbuf)
{
    if (oal_unlikely((hcc == NULL) || (pst_netbuf == NULL))) {
        oal_warn_on(1);
        return;
    }
    oal_netbuf_list_tail(&hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[DATA_LO_QUEUE].data_queue,
                         pst_netbuf);
}

oal_int32 sdio_credit_info_update_handler(oal_void *data)
{
    return OAL_SUCC;
}

OAL_STATIC OAL_INLINE hcc_netbuf_queue_type hcc_queue_map_to_netbuf_queue(hcc_queue_type qtype)
{
    if (qtype == CTRL_QUEUE || qtype == DATA_HI_QUEUE) {
        return HCC_NETBUF_HIGH_QUEUE;
    }

    return HCC_NETBUF_NORMAL_QUEUE;
}

OAL_STATIC oal_int32 _queues_not_flowctrl_len_check(struct hcc_handler *hcc,
                                                    hcc_chan_type dir)
{
    oal_int32 i;
    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        if (hcc->hcc_transer_info.hcc_queues[dir].queues[i].flow_ctrl.enable != OAL_TRUE &&
            hcc->hcc_transer_info.hcc_queues[dir].queues[i].flow_ctrl.flow_type != HCC_FLOWCTRL_CREDIT) {
            if (oal_netbuf_list_len(&hcc->hcc_transer_info.hcc_queues[dir].queues[i].data_queue)) {
                if (oal_unlikely((hcc_to_dev(hcc) == NULL) || (hcc_to_bus(hcc) == NULL))) {
                    oal_print_hi11xx_log(HI11XX_LOG_WARN, "hcc'dev is %p, hcc'dev bus is %p",
                                         hcc_to_dev(hcc), hcc_to_dev(hcc) ? hcc_to_bus(hcc) : NULL);
                } else {
                    if (hcc_bus_check_tx_condition(hcc_to_bus(hcc),
                                                   hcc_queue_map_to_netbuf_queue((hcc_queue_type)i)) == OAL_TRUE) {
                        /* ����ͨ����ͨ */
                        oal_print_hi11xx_log(HI11XX_LOG_VERBOSE, "sdio tx cond true");
                        return OAL_TRUE;
                    }
                }
            }
        }
    }

    return OAL_FALSE;
}

OAL_STATIC oal_int32 _queues_pcie_len_check(struct hcc_handler *hcc,
                                            hcc_chan_type dir)
{
    oal_int32 i;
    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        if (oal_netbuf_list_len(&hcc->hcc_transer_info.hcc_queues[dir].queues[i].data_queue)) {
            if (oal_unlikely((hcc_to_dev(hcc) == NULL) || (hcc_to_bus(hcc) == NULL))) {
                oal_print_hi11xx_log(HI11XX_LOG_WARN, "hcc'dev is %p, hcc'dev bus is %p",
                                     hcc_to_dev(hcc), hcc_to_dev(hcc) ? hcc_to_bus(hcc) : NULL);
            } else {
                if (OAL_TRUE == hcc_bus_check_tx_condition(hcc_to_bus(hcc),
                                                           hcc_queue_map_to_netbuf_queue((hcc_queue_type)i))) {
                    /* ����ͨ����ͨ */
                    oal_print_hi11xx_log(HI11XX_LOG_VERBOSE, "pcie check tx cond true");
                    return OAL_TRUE;
                }
            }
        }
    }

    return OAL_FALSE;
}

/* wether the hcc flow ctrl queues have data. */
OAL_STATIC oal_int32 _queues_flow_ctrl_len_check(struct hcc_handler *hcc, hcc_chan_type dir)
{
    oal_int32 i;
    hcc_tx_flow_ctrl_info *pst_flowctrl_info;

    pst_flowctrl_info = &hcc->hcc_transer_info.tx_flow_ctrl;

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        if (hcc->hcc_transer_info.hcc_queues[dir].queues[i].flow_ctrl.enable == OAL_TRUE) {
            if (hcc->hcc_transer_info.hcc_queues[dir].queues[i].flow_ctrl.flow_type == HCC_FLOWCTRL_SDIO) {
                if (oal_netbuf_list_len(&hcc->hcc_transer_info.hcc_queues[dir].queues[i].data_queue)
                    && (oal_atomic_read(&pst_flowctrl_info->flowctrl_flag) == D2H_MSG_FLOWCTRL_ON)) {
                    oal_print_hi11xx_log(HI11XX_LOG_VERBOSE, "sdio fc true");
                    return 1;
                }
            }
        } else {
            if (hcc->hcc_transer_info.hcc_queues[dir].queues[i].flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT) {
                /* credit flowctrl */
                if (oal_netbuf_list_len(&hcc->hcc_transer_info.hcc_queues[dir].queues[i].data_queue)
                    && (pst_flowctrl_info->uc_hipriority_cnt > g_hcc_credit_bottom_value)) {
                    oal_print_hi11xx_log(HI11XX_LOG_VERBOSE, "sdio fcr true");
                    return 1;
                }
            }
        }
    }

    return 0;
}

oal_uint32 hcc_queues_flow_ctrl_len(struct hcc_handler *hcc, hcc_chan_type dir)
{
    oal_int32 i;
    oal_uint32 total = 0;
    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        if (hcc->hcc_transer_info.hcc_queues[dir].queues[i].flow_ctrl.enable == OAL_TRUE) {
            total += oal_netbuf_list_len(&hcc->hcc_transer_info.hcc_queues[dir].queues[i].data_queue);
        }
    }
    return total;
}

/* wether the hcc queue have data. */
OAL_STATIC oal_int32 _queues_len_check(struct hcc_handler *hcc, hcc_chan_type dir)
{
    oal_int32 i;
    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        if (oal_netbuf_list_len(&hcc->hcc_transer_info.hcc_queues[dir].queues[i].data_queue)) {
            oal_print_hi11xx_log(HI11XX_LOG_VERBOSE, "d:%d, qid:%d had pkts", dir, i);
            return OAL_TRUE;
        }
    }
    return OAL_FALSE;
}

/*
 * Prototype    : hcc_thread_wait_event_cond_check
 * Description  : the hcc thread process conditon check, check whether queue empty
 * Input        : struct hcc_handler* hcc
 */
OAL_STATIC oal_int32 hcc_thread_wait_event_cond_check(struct hcc_handler *hcc)
{
    oal_int32 ret;
    if (oal_warn_on(hcc == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: hcc is null", __FUNCTION__);
        return OAL_FALSE;
    };
    /* please first check the condition which may be ok likely to reduce the cpu mips */
    if (hcc->bus_dev->cur_bus->bus_type == HCC_BUS_PCIE) {
        ret = ((_queues_len_check(hcc, HCC_RX)) ||
               (_queues_pcie_len_check(hcc, HCC_TX)) ||
               (hcc->p_hmac_tcp_ack_need_schedule_func && hcc->p_hmac_tcp_ack_need_schedule_func() == OAL_TRUE));
    } else if (hcc->bus_dev->cur_bus->bus_type == HCC_BUS_SDIO) {
        ret = (_queues_flow_ctrl_len_check(hcc, HCC_TX) ||
               (_queues_len_check(hcc, HCC_RX)) ||
               (_queues_not_flowctrl_len_check(hcc, HCC_TX)) ||
               (hcc->p_hmac_tcp_ack_need_schedule_func && hcc->p_hmac_tcp_ack_need_schedule_func() == OAL_TRUE));
    } else {
        ret = OAL_FALSE;
    }

#ifdef _PRE_CONFIG_WLAN_THRANS_THREAD_DEBUG
    if (ret == OAL_TRUE) {
        hcc->hcc_transer_info.thread_stat.wait_event_run_count++;
    }
    if (ret == OAL_FALSE) {
        hcc->hcc_transer_info.thread_stat.wait_event_block_count++;
    }
#endif
    oal_print_hi11xx_log(HI11XX_LOG_VERBOSE, "wait_cond:%d", ret);
    return ret;
}

OAL_STATIC oal_int32 hcc_send_single_descr(struct hcc_handler *hcc, oal_netbuf_stru *netbuf)
{
    oal_netbuf_head_stru head_send;
    oal_reference(hcc);

    if (oal_warn_on(netbuf == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "netbuf is null!%s\n", __FUNCTION__);
        return -OAL_EINVAL;
    }

    oal_netbuf_list_head_init(&head_send);
    oal_netbuf_list_tail(&head_send, netbuf);

    return hcc_bus_tx_netbuf_list(hcc_to_bus(hcc), &head_send, HCC_NETBUF_NORMAL_QUEUE);
}

oal_int32 hcc_send_descr_control_data(struct hcc_handler *hcc, hcc_descr_type descr_type,
                                      oal_void *data, oal_uint32 ul_size)
{
    oal_int32 ret;
    oal_netbuf_stru *netbuf = NULL;
    struct hcc_descr_header *dscr_hdr = NULL;
    netbuf = hcc_tx_assem_descr_get(hcc);
    if (oal_warn_on(netbuf == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "No descr mem!");
        return -OAL_ENOMEM;
    }

    dscr_hdr = (struct hcc_descr_header *)oal_netbuf_data(netbuf);
    dscr_hdr->descr_type = descr_type;

    if (ul_size) {
        if (oal_warn_on(data == NULL)) {
            hcc_tx_assem_descr_put(hcc, netbuf);
            return -OAL_EINVAL;
        }
        if (oal_warn_on(ul_size + OAL_SIZEOF(struct hcc_descr_header) > oal_netbuf_len(netbuf))) {
            OAL_IO_PRINT("invaild request size:%u,max size:%u\r\n",
                         (oal_uint32)(ul_size + OAL_SIZEOF(struct hcc_descr_header)),
                         (oal_uint32)oal_netbuf_len(netbuf));
            hcc_tx_assem_descr_put(hcc, netbuf);
            return -OAL_EINVAL;
        }

        /*lint -e124*/
        if (memcpy_s((oal_void *)((oal_uint8 *)oal_netbuf_data(netbuf) + OAL_SIZEOF(struct hcc_descr_header)),
                      oal_netbuf_len(netbuf) - OAL_SIZEOF(struct hcc_descr_header), data, ul_size) != EOK) {
            oal_print_hi11xx_log(HI11XX_LOG_ERR, "memcpy_s error, destlen=%u, srclen=%u\n ",
                                (oal_uint32)(oal_netbuf_len(netbuf) - OAL_SIZEOF(struct hcc_descr_header)), ul_size);
            hcc_tx_assem_descr_put(hcc, netbuf);
            return -OAL_EFAIL;
        }
    }

    ret = hcc_send_single_descr(hcc, netbuf);

    hcc_tx_assem_descr_put(hcc, netbuf);
    return ret;
}

oal_int32 hcc_tx_netbuf_queue_switch(struct hcc_handler *hcc, hcc_netbuf_queue_type queue_type)
{
    return hcc_send_descr_control_data(hcc, HCC_NETBUF_QUEUE_SWITCH, &queue_type, OAL_SIZEOF(queue_type));
}

oal_int32 hcc_tx_netbuf_test_and_switch_high_pri_queue(struct hcc_handler *hcc, hcc_netbuf_queue_type pool_type)
{
    oal_int32 ret = OAL_EFAIL;
    if (pool_type == HCC_NETBUF_HIGH_QUEUE) {
        ret = hcc_tx_netbuf_queue_switch(hcc, HCC_NETBUF_HIGH_QUEUE);
    }
    return ret;
}

oal_int32 hcc_tx_netbuf_restore_normal_pri_queue(struct hcc_handler *hcc, hcc_netbuf_queue_type pool_type)
{
    oal_int32 ret = OAL_EFAIL;
    if (pool_type == HCC_NETBUF_HIGH_QUEUE) {
        ret = hcc_tx_netbuf_queue_switch(hcc, HCC_NETBUF_NORMAL_QUEUE);
    }
    return ret;
}

/* �黹�����Ͷ��� */
oal_void hcc_restore_tx_netbuf(struct hcc_handler *hcc, oal_netbuf_stru *pst_netbuf)
{
    struct hcc_tx_cb_stru *pst_cb_stru = NULL;
    hcc_trans_queue *pst_hcc_queue = NULL;

    if (oal_unlikely((hcc == NULL) || (pst_netbuf == NULL))) {
        oal_warn_on(1);
        return;
    }

    pst_cb_stru = (struct hcc_tx_cb_stru *)oal_netbuf_cb(pst_netbuf);
    if (oal_unlikely(pst_cb_stru->magic != HCC_TX_WAKELOCK_MAGIC || pst_cb_stru->qtype >= HCC_QUEUE_COUNT)) {
#ifdef CONFIG_PRINTK
        printk(KERN_EMERG "BUG: tx netbuf:%p on CPU#%d,magic:%08x should be %08x, qtype:%u\n", pst_cb_stru,
               raw_smp_processor_id(), pst_cb_stru->magic, HCC_TX_WAKELOCK_MAGIC, pst_cb_stru->qtype);
        print_hex_dump(KERN_ERR, "tx_netbuf_magic", DUMP_PREFIX_ADDRESS, 16, 1,
                       (oal_uint8 *)pst_netbuf, sizeof(oal_netbuf_stru), true); /* �ں˺����̶����� */
        printk(KERN_ERR "\n");
#endif
        declare_dft_trace_key_info("tx_restore_wakelock_crash", OAL_DFT_TRACE_EXCEP);
        return;
    }

    pst_hcc_queue = &hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[pst_cb_stru->qtype];

    oal_netbuf_addlist(&pst_hcc_queue->data_queue, pst_netbuf);
}

oal_void hcc_restore_assemble_netbuf_list(struct hcc_handler *hcc)
{
    hcc_queue_type type;
    oal_netbuf_head_stru *assembled_head = NULL;

    if (oal_unlikely(hcc == NULL)) {
        oal_warn_on(1);
        return;
    }

    type = hcc->hcc_transer_info.tx_assem_info.assembled_queue_type;
    assembled_head = &hcc->hcc_transer_info.tx_assem_info.assembled_head;

    if (type >= HCC_QUEUE_COUNT) {
        type = DATA_LO_QUEUE;
    }

    if (!oal_netbuf_list_empty(assembled_head))
        oal_netbuf_splice_sync(&hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[type].data_queue,
                               assembled_head);
}

OAL_STATIC oal_int32 hcc_send_assemble_reset(struct hcc_handler *hcc)
{
    oal_int32 ret;

    hcc->hcc_transer_info.tx_flow_ctrl.flowctrl_reset_count++;

    /* ��ֻ����һ���ۺ��������������Ҿۺϸ���Ϊ0����֪ͨDevice ���þۺ���Ϣ */
    ret = hcc_send_descr_control_data(hcc, HCC_DESCR_ASSEM_RESET, NULL, 0);

    hcc_restore_assemble_netbuf_list(hcc);

    return ret;
}

OAL_STATIC oal_int32 hcc_send_data_packet(struct hcc_handler *hcc, oal_netbuf_head_stru *head,
                                          hcc_queue_type type, oal_netbuf_head_stru *next_assembled_head,
                                          hcc_send_mode mode, oal_uint32 *remain_len)
{
    oal_uint8 *buf = NULL;
    oal_uint32 total_send;
    oal_int32 ret = OAL_SUCC;
    oal_netbuf_head_stru head_send;
    oal_netbuf_stru *netbuf = NULL;
    oal_netbuf_stru *descr_netbuf = NULL;
    oal_netbuf_stru *netbuf_t = NULL;
    oal_uint32 *info = NULL;
    hcc_trans_queue *pst_hcc_queue = NULL;
    oal_uint8 uc_credit;

    if (*remain_len == 0) {
        return OAL_SUCC;
    }

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "send queue:%d, mode:%d,next assem len:%d",
                         type, mode, oal_netbuf_list_len(next_assembled_head));

    pst_hcc_queue = &hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[type];

    if (pst_hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT) {
        /* credit flowctrl */
        uc_credit = hcc->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt;
        /* �����ȼ�����: creditֵΪ0ʱ������ */
        if (!(uc_credit > g_hcc_credit_bottom_value)) {
            return OAL_SUCC;
        }
    }

    descr_netbuf = hcc_tx_assem_descr_get(hcc);
    if (descr_netbuf == NULL) {
        return -OAL_ENOMEM;
    }

    info = hcc->hcc_transer_info.tx_assem_info.info;

    oal_netbuf_list_head_init(&head_send);

    if (oal_netbuf_list_empty(next_assembled_head)) {
        /* single send */
        netbuf = oal_netbuf_delist(head);
        if (netbuf == NULL) {
            OAL_IO_PRINT("netbuf is NULL [len:%d]\n", oal_netbuf_list_len(head));
            ret = OAL_SUCC;
            /*lint -e801*/
            goto failed_get_sig_buff;
        }

        netbuf_t = hcc_netbuf_len_align(netbuf, HISDIO_H2D_SCATT_BUFFLEN_ALIGN);
        if (oal_unlikely(netbuf_t == NULL)) {
            /* return to the list */
            oal_netbuf_addlist(head, netbuf);
            /*lint -e801*/
            goto failed_align_netbuf;
        }

        oal_netbuf_list_tail(&head_send, netbuf);
        info[0]++;
    } else {
        oal_uint32 assemble_len = oal_netbuf_list_len(next_assembled_head);
        if (oal_unlikely(assemble_len > HISDIO_HOST2DEV_SCATT_SIZE)) {
            oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: assemble_len:%d > HISDIO_HOST2DEV_SCATT_SIZE:%d",
                                 __FUNCTION__, assemble_len, HISDIO_HOST2DEV_SCATT_SIZE);
        } else {
            info[assemble_len]++;
        }
        /* move the assem list to send queue */
        oal_netbuf_splice_init(next_assembled_head, &head_send);
    }

    total_send = oal_netbuf_list_len(&head_send);
    if (*remain_len >= total_send) {
        *remain_len -= total_send;
    } else {
        *remain_len = 0;
    }

    if (oal_warn_on(!oal_netbuf_list_empty(next_assembled_head))) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: next_assembled_head is empty", __FUNCTION__);
    };

    if (mode == HCC_ASSEM_SEND) {
        hcc_build_next_assem_descr(hcc, type, head, next_assembled_head, descr_netbuf, *remain_len);
    } else {
        buf = oal_netbuf_data(descr_netbuf);
        *((oal_uint32 *)buf) = 0;
    }

    /* add the assem descr buf */
    oal_netbuf_addlist(&head_send, descr_netbuf);

    ret = hcc_bus_tx_netbuf_list(hcc_to_bus(hcc), &head_send, hcc_queue_map_to_netbuf_queue(type));

    g_pm_wifi_rxtx_count += total_send;  // ���Ͱ�ͳ�� for pm

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[type].total_pkts += total_send;

    /* �����ȼ�����: ����creditֵ */
    if (pst_hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT) {
        oal_spin_lock(&(hcc->hcc_transer_info.tx_flow_ctrl.st_hipri_lock));

        uc_credit = hcc->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt;
        uc_credit = (uc_credit > total_send) ? (oal_uint8)(uc_credit - total_send) : 0;
        hcc->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt = uc_credit;

        oal_spin_unlock(&(hcc->hcc_transer_info.tx_flow_ctrl.st_hipri_lock));
    }

    descr_netbuf = oal_netbuf_delist(&head_send);
    if (descr_netbuf == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hcc_send_data_packet::oal_netbuf_delist fail.descr_netbuf is NULL.");
        ret = OAL_SUCC;
        goto failed_get_sig_buff;
    }

    hcc_tx_assem_descr_put(hcc, descr_netbuf);

    /* free the sent netbuf,release the wakelock */
    hcc_tx_netbuf_list_free(&head_send);

    return ret;
failed_align_netbuf:
    hcc_tx_assem_descr_put(hcc, descr_netbuf);
failed_get_sig_buff:
    return ret;
}

oal_void hcc_clear_all_queues(struct hcc_handler *hcc, oal_int32 is_need_lock)
{
    oal_int32 i;
    oal_netbuf_head_stru *pst_head = NULL;

    if (oal_warn_on(hcc == NULL)) {
        OAL_IO_PRINT("[E]hcc is null\n");
        return;
    }

    if (is_need_lock == OAL_TRUE) {
        hcc_tx_transfer_lock(hcc);
    }

    /* Restore assem queues */
    hcc_restore_assemble_netbuf_list(hcc);

    /* Clear all tx queues */
    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        oal_uint32 ul_list_len;
        pst_head = &hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].data_queue;
        ul_list_len = oal_netbuf_list_len(pst_head);
        hcc_tx_netbuf_list_free(pst_head);
        if (ul_list_len) {
            OAL_IO_PRINT("Clear queue:%d,total %u hcc tx pkts!\n", i, ul_list_len);
        }
    }

    if (is_need_lock == OAL_TRUE) {
        hcc_tx_transfer_unlock(hcc);
    }

    /* Clear all rx queues */
    if (is_need_lock == OAL_TRUE) {
        hcc_rx_transfer_lock(hcc);
    }

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        oal_uint32 ul_list_len;
        pst_head = &hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[i].data_queue;
        ul_list_len = oal_netbuf_list_len(pst_head);
        oal_netbuf_queue_purge(pst_head);
        if (ul_list_len) {
            OAL_IO_PRINT("Clear queue:%d,total %u hcc rx pkts!\n", i, ul_list_len);
        }
    }

    if (is_need_lock == OAL_TRUE) {
        hcc_rx_transfer_unlock(hcc);
    }
}

oal_void hcc_change_state(struct hcc_handler *hcc, oal_int32 state)
{
    oal_int32 old_state, new_state;
    if (oal_warn_on(hcc == NULL)) {
        return;
    }

    old_state = oal_atomic_read(&hcc->state);

    oal_atomic_set(&hcc->state, state);

    new_state = oal_atomic_read(&hcc->state);
    if (old_state != new_state) {
        OAL_IO_PRINT("hcc state %s[%d]=>%s[%d]\n",
                     old_state == HCC_ON ? "on " : "off",
                     old_state,
                     new_state == HCC_ON ? "on " : "off",
                     new_state);
    }
}

oal_void hcc_change_state_exception(oal_void)
{
    hcc_change_state(hcc_get_110x_handler(), HCC_EXCEPTION);
}

oal_void hcc_enable(struct hcc_handler *hcc, oal_int32 is_need_lock)
{
    oal_int32 i;
    if (oal_warn_on(hcc == NULL)) {
        return;
    }

    if (is_need_lock == OAL_TRUE) {
        hcc_transfer_lock(hcc);
    }

    hcc_clear_all_queues(hcc, OAL_FALSE);

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        oal_spin_lock_bh(&hcc->hcc_transer_info.tx_flow_ctrl.lock);
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].flow_ctrl.is_stopped = OAL_FALSE;
        oal_spin_unlock_bh(&hcc->hcc_transer_info.tx_flow_ctrl.lock);
    }

    hcc_change_state(hcc, HCC_ON);

    if (is_need_lock == OAL_TRUE) {
        hcc_transfer_unlock(hcc);
    }
}

oal_void hcc_disable(struct hcc_handler *hcc, oal_int32 is_need_lock)
{
    if (oal_unlikely(hcc == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_WARN, "hcc is null");
        return;
    }

    hcc_change_state(hcc, HCC_OFF);

    /* disable flow ctrl detect timer */
    hcc_dev_flowctr_timer_del(hcc);
    oal_cancel_delayed_work_sync(&hcc->hcc_transer_info.tx_flow_ctrl.worker);

    if (is_need_lock == OAL_TRUE) {
        hcc_transfer_lock(hcc);
    }

    hcc_clear_all_queues(hcc, OAL_FALSE);

    if (is_need_lock == OAL_TRUE) {
        hcc_transfer_unlock(hcc);
    }
    /* must wake up tx thread after clear the hcc queues */
    oal_wait_queue_wake_up_interrupt(&hcc->hcc_transer_info.tx_flow_ctrl.wait_queue);
}

/*
 * Prototype    : hcc_send_tx_queue
 * Description  : send the special
 */
int hcc_send_tx_queue(struct hcc_handler *hcc, hcc_queue_type type)
{
    oal_int32 count = 0;
    oal_netbuf_head_stru *head = NULL;
    hcc_trans_queue *pst_hcc_queue = NULL;
    hcc_netbuf_queue_type qtype = HCC_NETBUF_NORMAL_QUEUE;
    hcc_tx_flow_ctrl_info *pst_flowctrl_info = NULL;

    if (oal_unlikely(hcc == NULL)) {
        OAL_IO_PRINT("%s error: hcc is null", __FUNCTION__);
        return 0;
    };

    if (oal_unlikely(type >= HCC_QUEUE_COUNT)) {
        OAL_IO_PRINT("unkown hcc queue type %d\n", type);
        return count;
    }

    pst_flowctrl_info = &hcc->hcc_transer_info.tx_flow_ctrl;
    pst_hcc_queue = &hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[type];
    head = &pst_hcc_queue->data_queue;

    hcc_tx_transfer_lock(hcc);
#ifdef WIN32
    if (0)
#else
    if (hcc->bus_dev->cur_bus->bus_type == HCC_BUS_PCIE)
#endif
    {
        /* �ο� hcc_send_data_packet */
        if (oal_netbuf_list_empty(head)) {
            oal_print_hi11xx_log(HI11XX_LOG_DBG, "queue type %d is empty\n", type);
            hcc_tx_transfer_unlock(hcc);
            return count;
        }

        qtype = hcc_queue_map_to_netbuf_queue(type);
        count += hcc_bus_tx_netbuf_list(hcc_to_bus(hcc), head, qtype);
        hcc->hcc_transer_info.tx_assem_info.pkt_cnt += count;
        g_pm_wifi_rxtx_count += (oal_uint32)count; /* delete p_auto_freq_count_func,add this */
        oal_spin_lock_bh(&pst_flowctrl_info->lock);

        if ((oal_netbuf_list_len(&pst_hcc_queue->data_queue) < pst_hcc_queue->flow_ctrl.low_waterline)) {
            if (pst_hcc_queue->flow_ctrl.is_stopped == OAL_TRUE) {
                pst_hcc_queue->flow_ctrl.is_stopped = OAL_FALSE;

                hcc_tx_network_start_subq(hcc, (oal_uint16)pst_hcc_queue->wlan_queue_id);
            }
            oal_wait_queue_wake_up_interrupt(&pst_flowctrl_info->wait_queue);
        }

        oal_spin_unlock_bh(&pst_flowctrl_info->lock);
        hcc_tx_transfer_unlock(hcc);
        return count;
    } else { /* SDIO & USB */
        oal_uint8 uc_credit;
        oal_int32 ret = 0;
        oal_uint32 queue_len, remain_len, remain_len_t;
        oal_netbuf_head_stru *next_assembled_head = NULL;
        hcc_send_mode send_mode;
        hcc_netbuf_queue_type pool_type;
        oal_uint32 ul_pool_type_flag = OAL_FALSE;

        if (oal_netbuf_list_empty(head)) {
            oal_print_hi11xx_log(HI11XX_LOG_DBG, "queue type %d is empty", type);
            hcc_tx_transfer_unlock(hcc);
            return count;
        }

        queue_len = oal_netbuf_list_len(head);
        remain_len = queue_len;

        oal_print_hi11xx_log(HI11XX_LOG_DBG,
                             "before_update: hcc_send_tx_queue: queue_type = %d, burst_limit = %d, remain_len = %d",
                             type, pst_hcc_queue->burst_limit, remain_len);
        remain_len = oal_min(pst_hcc_queue->burst_limit, remain_len);

        remain_len_t = remain_len;
        next_assembled_head = &hcc->hcc_transer_info.tx_assem_info.assembled_head;

        if (oal_unlikely(!oal_netbuf_list_empty(next_assembled_head))) {
            /* First enter the queue, should single send */
            OAL_IO_PRINT("[WARN]reset assemble package![queue type:%d,len:%d]\n",
                         (oal_int32)type, oal_netbuf_list_len(next_assembled_head));
            if (hcc_send_assemble_reset(hcc) != OAL_SUCC) {
                hcc_tx_transfer_unlock(hcc);
                /* send one pkt */
                return 1;
            }
        }

        send_mode = pst_hcc_queue->send_mode;
        pool_type = (hcc_netbuf_queue_type)pst_hcc_queue->netbuf_pool_type;

        for (;;) {
            if (remain_len == 0) {
                break;
            }

#ifdef CONFIG_SDIO_MSG_FLOWCTRL
            if (pst_hcc_queue->flow_ctrl.enable == OAL_TRUE) {
                if (pst_hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_SDIO) {
                    /* flow ctrl */
                    if (oal_atomic_read(&pst_flowctrl_info->flowctrl_flag) != D2H_MSG_FLOWCTRL_ON) {
                        oal_print_hi11xx_log(HI11XX_LOG_DBG, "[WARNING]can't send data, flow off");
                        if (!oal_netbuf_list_empty(&hcc->hcc_transer_info.tx_assem_info.assembled_head)) {
                            if (hcc_send_assemble_reset(hcc) != OAL_SUCC) {
                                hcc_tx_transfer_unlock(hcc);
                                count++;
                                /* send one pkt */
                                return count;
                            }
                        }
                        hcc_tx_transfer_unlock(hcc);
                        return count;
                    }
                }
            }
#endif
            if (pst_hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT) {
                uc_credit = pst_flowctrl_info->uc_hipriority_cnt;

                /* �����ȼ����û���ڴ棬ֱ�ӷ��ع����ѭ�����⡣ */
                if (!(uc_credit > g_hcc_credit_bottom_value)) {
                    if (ul_pool_type_flag == OAL_TRUE) {
                        /* �ָ�����ͨ���ȼ� */
                        hcc_tx_netbuf_restore_normal_pri_queue(hcc, pool_type);
                    }
                    hcc_tx_transfer_unlock(hcc);
                    return OAL_SUCC;
                }

                if (ul_pool_type_flag == OAL_FALSE) {
                    if (hcc_tx_netbuf_test_and_switch_high_pri_queue(hcc, pool_type) == OAL_SUCC) {
                        ul_pool_type_flag = OAL_TRUE;
                    }
                }
            }

            ret = hcc_send_data_packet(hcc, head, type, next_assembled_head, send_mode, &remain_len);

            if (oal_unlikely(remain_len > remain_len_t)) {
                oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: remain_len:%d > remain_len_t:%d",
                                     __FUNCTION__, remain_len, remain_len_t);
                hcc_tx_transfer_unlock(hcc);
                return OAL_FAIL;
            };

            if (oal_likely(ret == OAL_SUCC)) {
                count += (oal_int32)(remain_len_t - remain_len);
                hcc->hcc_transer_info.tx_assem_info.pkt_cnt += (remain_len_t - remain_len);
            }

#ifdef CONFIG_SDIO_MSG_FLOWCTRL
            oal_spin_lock_bh(&hcc->hcc_transer_info.tx_flow_ctrl.lock);

            if ((oal_netbuf_list_len(&pst_hcc_queue->data_queue) < pst_hcc_queue->flow_ctrl.low_waterline)) {
                if (pst_hcc_queue->flow_ctrl.is_stopped == OAL_TRUE) {
                    pst_hcc_queue->flow_ctrl.is_stopped = OAL_FALSE;

                    hcc_tx_network_start_subq(hcc, (oal_uint16)pst_hcc_queue->wlan_queue_id);
                }
                oal_wait_queue_wake_up_interrupt(&hcc->hcc_transer_info.tx_flow_ctrl.wait_queue);
            }

            oal_spin_unlock_bh(&hcc->hcc_transer_info.tx_flow_ctrl.lock);
#endif
        }

        if (pst_hcc_queue->flow_ctrl.flow_type == HCC_FLOWCTRL_CREDIT) {
            if (ul_pool_type_flag == OAL_TRUE) {
                hcc_tx_netbuf_restore_normal_pri_queue(hcc, pool_type);
            }
        }

        hcc_tx_transfer_unlock(hcc);
        return count;
    }
}

/*
 * Prototype    : hcc_rx_register
 * Description  : register cb function
 */
oal_int32 hcc_rx_register(struct hcc_handler *hcc, oal_uint8 mtype, hcc_rx_post_do post_do, hcc_rx_pre_do pre_do)
{
    hcc_rx_action *rx_action = NULL;

    if (oal_warn_on(hcc == NULL)) {
        return -OAL_EINVAL;
    }

    rx_action = &(hcc->hcc_transer_info.rx_action_info.action[mtype]);

    if (oal_warn_on(rx_action->post_do != NULL)) {
#ifdef CONFIG_PRINTK
        OAL_IO_PRINT("repeat regist hcc action :%d,it's had %pF\n", mtype, rx_action->post_do);
#else
        OAL_IO_PRINT("repeat regist hcc action :%d,it's had %p\n", mtype, rx_action->post_do);
#endif
        return -OAL_EBUSY;
    }
    /* �˴���ʱ���ӻ������������̱�֤�� */
    rx_action->post_do = post_do;
    rx_action->pre_do = pre_do;

    return OAL_SUCC;
}
/*lint -e19*/
oal_module_symbol(hcc_rx_register);
/*lint +e19*/
oal_int32 hcc_rx_unregister(struct hcc_handler *hcc, oal_uint8 mtype)
{
    hcc_rx_action *rx_action = NULL;

    if (oal_warn_on((hcc == NULL) || (mtype >= HCC_ACTION_TYPE_BUTT))) {
        OAL_IO_PRINT("Invalid params:hcc = %p, main type = %u\n", hcc, mtype);
        return -OAL_EFAIL;
    }

    rx_action = &hcc->hcc_transer_info.rx_action_info.action[mtype];

    memset_s((oal_void *)rx_action, OAL_SIZEOF(hcc_rx_action), 0, OAL_SIZEOF(hcc_rx_action));

    return OAL_SUCC;
}
/*lint -e19*/
oal_module_symbol(hcc_rx_unregister);
/*lint +e19*/
OAL_STATIC oal_uint32 hcc_check_rx_netbuf_vaild(struct hcc_header *hdr, oal_int32 netbuf_len)
{
    if (oal_unlikely(hcc_check_header_vaild(hdr) != OAL_TRUE)) {
        return OAL_FALSE;
    }

    if (oal_unlikely(hdr->pad_payload + HCC_HDR_TOTAL_LEN + hdr->pay_len > netbuf_len)) {
        return OAL_FALSE;
    }

    return OAL_TRUE;
}

/*
 * Prototype    : hcc_rx
 * Description  : trim the hcc header etc.
 */
OAL_STATIC oal_int32 hcc_rx(struct hcc_handler *hcc, oal_netbuf_stru *netbuf)
{
    struct hcc_header *hdr = NULL;
    oal_int32 netbuf_len;
    oal_int32 extend_len;

    if (oal_unlikely(!hcc)) {
        OAL_IO_PRINT("hcc is null r failed!%s\n", __FUNCTION__);
        return -OAL_EINVAL;
    }

    hdr = (struct hcc_header *)oal_netbuf_data(netbuf);
    netbuf_len = (oal_int32)oal_netbuf_len(netbuf);
    if (oal_unlikely(hcc_check_rx_netbuf_vaild(hdr, netbuf_len) != OAL_TRUE)) {
        oal_print_hex_dump((oal_uint8 *)hdr, (oal_int32)oal_netbuf_len(netbuf),
                           HEX_DUMP_GROUP_SIZE, "invaild hcc_rx header: ");
        if (hcc_to_dev(hcc)) {
            hcc_bus_try_to_dump_device_mem(hcc_to_bus(hcc), OAL_FALSE);
        }
        return -OAL_EFAIL;
    }

    extend_len = HCC_HDR_TOTAL_LEN - HCC_HDR_LEN - hdr->pad_hdr;

    if (hdr->pad_payload) {
        /* memmove */
        oal_uint8 *extend_addr = (oal_uint8 *)hdr + HCC_HDR_LEN + hdr->pad_hdr;
        if (memmove_s(extend_addr + hdr->pad_payload, (oal_uint32)extend_len,
                      extend_addr, (oal_uint32)extend_len) != EOK) {
            oal_print_hi11xx_log(HI11XX_LOG_ERR, "memmove_s failed.\n");
        }
    }

    if (HCC_HDR_LEN + hdr->pad_hdr + hdr->pad_payload > oal_netbuf_len(netbuf)) {
        /* add check detail */
        OAL_IO_PRINT("[Error]wrong pad_hdr,hcc hdr too long,len:%d!\n", (oal_int32)oal_netbuf_len(netbuf));
        oal_print_hex_dump((oal_uint8 *)oal_netbuf_data(netbuf), (oal_int32)oal_netbuf_len(netbuf),
                           HEX_DUMP_GROUP_SIZE, "rx wrong data: ");
        /* print the data buff here! */
        return -OAL_EINVAL;
    }

#ifdef CONFIG_HCC_DEBUG
    OAL_IO_PRINT("[WIFI]hcc rx pkt seq:%d\n", hdr->seq);
    oal_print_hex_dump((oal_uint8 *)hdr, HCC_HDR_TOTAL_LEN, HEX_DUMP_GROUP_SIZE, "hcc hdr:");
    oal_print_hex_dump(oal_netbuf_data(netbuf) + HCC_HDR_TOTAL_LEN + hdr->pad_payload,
                       hdr->pay_len, HEX_DUMP_GROUP_SIZE, "hcc payload:");
#endif

    oal_netbuf_pull(netbuf, HCC_HDR_LEN + hdr->pad_hdr + hdr->pad_payload);

    /* ����ȥ��netbuf len ����extend_len����! */
    oal_netbuf_trim(netbuf, oal_netbuf_len(netbuf) - hdr->pay_len - (oal_uint32)extend_len);

    oal_netbuf_next(netbuf) = NULL;
    oal_netbuf_prev(netbuf) = NULL;

    return OAL_SUCC;
}

oal_int32 hcc_send_rx_queue(struct hcc_handler *hcc, hcc_queue_type type)
{
    oal_int32 count = 0;
    oal_int32 pre_ret = OAL_SUCC;
#ifndef WIN32
    oal_uint ul_irq_flag;
#endif
    oal_uint8 *pst_pre_context = NULL;
    oal_netbuf_head_stru *netbuf_hdr = NULL;
    hcc_rx_action *rx_action = NULL;
    oal_netbuf_stru *pst_netbuf = NULL;
    hcc_netbuf_stru st_hcc_netbuf;
    struct hcc_header *pst_hcc_head = NULL;
    oal_netbuf_head_stru st_netbuf_header;

    if (oal_warn_on(type >= HCC_QUEUE_COUNT)) {
        return -OAL_EINVAL;
    }

    netbuf_hdr = &hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[type].data_queue;
    oal_netbuf_list_head_init(&st_netbuf_header);
#ifndef WIN32
    /* hcc���л����жϲ�����������irq_save */
    spin_lock_irqsave(&netbuf_hdr->lock, ul_irq_flag);
    oal_netbuf_splice_init(netbuf_hdr, &st_netbuf_header);
    spin_unlock_irqrestore(&netbuf_hdr->lock, ul_irq_flag);
#else
    oal_netbuf_splice_sync(&st_netbuf_header, netbuf_hdr);
#endif

    /* ���δ��������ÿ��netbuf */
    for (;;) {
        pst_netbuf = oal_netbuf_delist_nolock(&st_netbuf_header);
        if (pst_netbuf == NULL) {
            break;
        }

        pst_hcc_head = (struct hcc_header *)oal_netbuf_data(pst_netbuf);
        if (oal_unlikely(hcc_check_header_vaild(pst_hcc_head) != OAL_TRUE)) {
            oal_print_hex_dump((oal_uint8 *)pst_hcc_head, HCC_HDR_TOTAL_LEN, HEX_DUMP_GROUP_SIZE,
                               "invaild hcc header: ");
            declare_dft_trace_key_info("hcc_rx_hdr_err", OAL_DFT_TRACE_EXCEP);
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "invaild hcc rx head");
            count++;
            oal_netbuf_free(pst_netbuf);
            return count;
        }

        rx_action = &hcc->hcc_transer_info.rx_action_info.action[pst_hcc_head->main_type];

        pre_ret = OAL_SUCC;

        /* prepare */
        if (rx_action->pre_do) {
            st_hcc_netbuf.pst_netbuf = pst_netbuf;
            st_hcc_netbuf.len = (oal_int32)oal_netbuf_len(pst_netbuf);
            pre_ret = rx_action->pre_do(hcc, pst_hcc_head->sub_type, &st_hcc_netbuf, &pst_pre_context);
        }

        if (pre_ret == OAL_SUCC) {
            if (oal_likely(hcc_rx(hcc, pst_netbuf) == OAL_SUCC)) {
                if (oal_likely(rx_action->post_do != NULL)) {
                    st_hcc_netbuf.pst_netbuf = pst_netbuf;
                    st_hcc_netbuf.len = (oal_int32)oal_netbuf_len(pst_netbuf);
                    rx_action->post_do(hcc, pst_hcc_head->sub_type, &st_hcc_netbuf, pst_pre_context);
                } else {
                    oam_error_log2(0, OAM_SF_ANY, "hcc mtype:%d,stype:%d did't register cb function!",
                                   pst_hcc_head->main_type,
                                   pst_hcc_head->sub_type);
                    oal_print_hex_dump((oal_uint8 *)pst_hcc_head, HCC_HDR_TOTAL_LEN,
                                       HEX_DUMP_GROUP_SIZE, "hcc invaild header: ");
                    oal_print_hex_dump(oal_netbuf_data(pst_netbuf), (oal_int32)oal_netbuf_len(pst_netbuf),
                                       HEX_DUMP_GROUP_SIZE, "hcc invaild header(payload): ");
                    oal_netbuf_free(pst_netbuf);
                }
            } else {
                oal_netbuf_free(pst_netbuf);
            }
            hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[type].total_pkts++;
            count++;
        } else {
            /*
             * simple process, when pre do failed,
             * keep the netbuf in list,
             * and skip the loop
             */
            /* pre doʧ�ܣ�netbuf������ӣ�ͬʱͬ������ */
            oal_netbuf_list_tail_nolock(&st_netbuf_header, pst_netbuf);
            oal_netbuf_splice_sync(netbuf_hdr, &st_netbuf_header);
            break;
        }
    }

    hcc->hcc_transer_info.rx_assem_info.pkt_cnt += count;

    return count;
}

oal_void hcc_tx_network_start_subq(struct hcc_handler *hcc, oal_uint16 us_queue_idx)
{
    if (oal_warn_on(!hcc)) {
        OAL_IO_PRINT("buf is null r failed!%s\n", __FUNCTION__);
        return;
    }

    if (oal_likely(hcc->hcc_transer_info.tx_flow_ctrl.start_subq)) {
        hcc->hcc_transer_info.tx_flow_ctrl.start_subq(us_queue_idx);
    }
}

oal_void hcc_tx_network_stop_subq(struct hcc_handler *hcc, oal_uint16 us_queue_idx)
{
    if (oal_warn_on(!hcc)) {
        OAL_IO_PRINT("buf is null r failed!%s\n", __FUNCTION__);
        return;
    }

    if (oal_likely(hcc->hcc_transer_info.tx_flow_ctrl.stop_subq)) {
        hcc->hcc_transer_info.tx_flow_ctrl.stop_subq(us_queue_idx);
    }
}

oal_void hcc_flowctl_operate_subq_register(hcc_flowctl_start_subq start_subq, hcc_flowctl_stop_subq stop_subq)
{
    if (g_hcc_tc == NULL) {
        OAL_IO_PRINT("[ERROR]hcc_flowctl_operate_subq_register failed! g_hcc_tc is NULL\n");
        return;
    }
    g_hcc_tc->hcc_transer_info.tx_flow_ctrl.start_subq = start_subq;
    g_hcc_tc->hcc_transer_info.tx_flow_ctrl.stop_subq = stop_subq;
    return;
}

oal_void hcc_host_set_flowctl_param(oal_uint8 uc_queue_type, oal_uint16 us_burst_limit,
                                    oal_uint16 us_low_waterline, oal_uint16 us_high_waterline)
{
    if (uc_queue_type >= HCC_QUEUE_COUNT) {
        OAL_IO_PRINT("CONFIG_ERROR: hcc_host_set_flowctl_param: uc_queue_type = %d\r\n", uc_queue_type);
        return;
    }
    g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[uc_queue_type].burst_limit = us_burst_limit;
    g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[uc_queue_type].flow_ctrl.low_waterline = us_low_waterline;
    g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[uc_queue_type].flow_ctrl.high_waterline = us_high_waterline;

    OAL_IO_PRINT("hcc_host_set_flowctl_param, queue[%d]: burst limit = %d, low_waterline = %d, high_waterline =%d\r\n",
                 uc_queue_type, us_burst_limit, us_low_waterline, us_high_waterline);
    return;
}

oal_void hcc_host_get_flowctl_param(oal_uint8 uc_queue_type, oal_uint16 *pus_burst_limit,
                                    oal_uint16 *pus_low_waterline, oal_uint16 *pus_high_waterline)
{
    if (uc_queue_type >= HCC_QUEUE_COUNT) {
        OAL_IO_PRINT("CONFIG_ERROR: hcc_host_get_flowctl_param: uc_queue_type = %d\r\n", uc_queue_type);
        return;
    }

    *pus_burst_limit = g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[uc_queue_type].burst_limit;
    *pus_low_waterline = g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[uc_queue_type].flow_ctrl.low_waterline;
    *pus_high_waterline = g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[uc_queue_type].flow_ctrl.high_waterline;
    return;
}

oal_void hcc_host_get_flowctl_stat(oal_void)
{
    oal_uint16 queue_idx;

    /* ����������е�״̬��Ϣ */
    for (queue_idx = 0; queue_idx < HCC_QUEUE_COUNT; queue_idx++) {
        OAL_IO_PRINT("Q[%d]:bst_lmt[%d],low_wl[%d],high_wl[%d]\r\n",
                     queue_idx,
                     g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_idx].burst_limit,
                     g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_idx].flow_ctrl.low_waterline,
                     g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_idx].flow_ctrl.high_waterline);

        OAL_IO_PRINT("      wlan_q[%d],stoped?[%d],q_len[%d],total_pkt[%d],loss_pkt[%d]\r\n",
                     g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_idx].wlan_queue_id,
                     g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_idx].flow_ctrl.is_stopped,
                     oal_netbuf_list_len(&g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_idx].data_queue),
                     g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_idx].total_pkts,
                     g_hcc_tc->hcc_transer_info.hcc_queues[HCC_TX].queues[queue_idx].loss_pkts);
    }

    return;
}

oal_bool_enum_uint8 hcc_flowctl_get_device_mode(struct hcc_handler *hcc)
{
    if (oal_warn_on(!hcc)) {
        OAL_IO_PRINT("buf is null r failed!%s\n", __FUNCTION__);
        return OAL_FALSE;
    }

    if (oal_likely(hcc->hcc_transer_info.tx_flow_ctrl.get_mode)) {
        return hcc->hcc_transer_info.tx_flow_ctrl.get_mode();
    }

    return OAL_FALSE;
}

oal_void hcc_flowctl_get_device_mode_register(hcc_flowctl_get_mode get_mode)
{
    if (g_hcc_tc == OAL_PTR_NULL) {
        OAL_IO_PRINT("[ERROR]hcc_flowctl_get_device_mode_register failed! g_hcc_tc is NULL\n");
        return;
    }
    g_hcc_tc->hcc_transer_info.tx_flow_ctrl.get_mode = get_mode;
}

oal_void hcc_host_update_vi_flowctl_param(oal_uint32 be_cwmin, oal_uint32 vi_cwmin)
{
    oal_uint16 us_burst_limit;
    oal_uint16 us_low_waterline;
    oal_uint16 us_high_waterline;

    /* ���vi��be��edca��������Ϊһ�£������VI��ӵ�����Ʋ��� */
    if (be_cwmin == vi_cwmin) {
        hcc_host_get_flowctl_param(DATA_UDP_BE_QUEUE, &us_burst_limit, &us_low_waterline, &us_high_waterline);
        hcc_host_set_flowctl_param(DATA_UDP_VI_QUEUE, us_burst_limit, us_low_waterline, us_high_waterline);
    } else {  // ��������vi��ӵ�����Ʋ���ΪĬ��ֵ
        hcc_host_set_flowctl_param(DATA_UDP_VI_QUEUE, UDP_VI_BURST_LIMIT,
                                   UDP_VI_LOW_WATERLINE, UDP_VI_HIGH_WATERLINE);
    }
}

/*lint -e19 */
oal_module_symbol(hcc_flowctl_get_device_mode_register);
oal_module_symbol(hcc_flowctl_operate_subq_register);
oal_module_symbol(hcc_host_update_vi_flowctl_param);

/*lint +e19 */

/*
 * Prototype    : hcc_thread_process
 * Description  : hcc main thread
 */
oal_int32 hcc_thread_process(struct hcc_handler *hcc)
{
    oal_int32 ret = 0;
    oal_bool_enum_uint8 en_device_is_sta;

    en_device_is_sta = hcc_flowctl_get_device_mode(hcc);
    if (en_device_is_sta == OAL_TRUE) {
        /* Tx Tcp Data queue > Rx Tcp Ack
          Rx Tcp Data > Tx Tcp Ack
          Tx Tcp Data queue > Rx Tcp Data queue */
        ret += hcc_send_rx_queue(hcc, CTRL_QUEUE);
        ret += hcc_send_tx_queue(hcc, CTRL_QUEUE);

        ret += hcc_send_rx_queue(hcc, DATA_HI_QUEUE);
        ret += hcc_send_tx_queue(hcc, DATA_HI_QUEUE);

        /* ����TCP���� */
        ret += hcc_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
        ret += hcc_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE);

        ret += hcc_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE);
        ret += hcc_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);

        /* Tx Lo < Rx Lo */
        ret += hcc_send_rx_queue(hcc, DATA_LO_QUEUE);
        ret += hcc_send_tx_queue(hcc, DATA_LO_QUEUE);

        ret += hcc_send_rx_queue(hcc, DATA_UDP_VO_QUEUE);
        ret += hcc_send_tx_queue(hcc, DATA_UDP_VO_QUEUE);

        ret += hcc_send_rx_queue(hcc, DATA_UDP_VI_QUEUE);
        ret += hcc_send_tx_queue(hcc, DATA_UDP_VI_QUEUE);

        ret += hcc_send_rx_queue(hcc, DATA_UDP_BE_QUEUE);
        ret += hcc_send_tx_queue(hcc, DATA_UDP_BE_QUEUE);

        ret += hcc_send_rx_queue(hcc, DATA_UDP_BK_QUEUE);
        ret += hcc_send_tx_queue(hcc, DATA_UDP_BK_QUEUE);
    } else {
        /*
         * Tx Tcp Data queue > Rx Tcp Ack
         * Rx Tcp Data > Tx Tcp Ack
         * Tx Tcp Data queue < Rx Tcp Data queue
         */
        ret += hcc_send_tx_queue(hcc, CTRL_QUEUE);
        ret += hcc_send_rx_queue(hcc, CTRL_QUEUE);

        ret += hcc_send_tx_queue(hcc, DATA_HI_QUEUE);
        ret += hcc_send_rx_queue(hcc, DATA_HI_QUEUE);

        ret += hcc_send_tx_queue(hcc, DATA_TCP_DATA_QUEUE);
        ret += hcc_send_rx_queue(hcc, DATA_TCP_ACK_QUEUE);

        ret += hcc_send_rx_queue(hcc, DATA_TCP_DATA_QUEUE);
        ret += hcc_send_tx_queue(hcc, DATA_TCP_ACK_QUEUE);

        /* Tx Lo < Rx Lo */
        ret += hcc_send_tx_queue(hcc, DATA_LO_QUEUE);
        ret += hcc_send_rx_queue(hcc, DATA_LO_QUEUE);

        /* udpҵ�� */
        ret += hcc_send_tx_queue(hcc, DATA_UDP_VO_QUEUE);
        ret += hcc_send_rx_queue(hcc, DATA_UDP_VO_QUEUE);

        ret += hcc_send_tx_queue(hcc, DATA_UDP_VI_QUEUE);
        ret += hcc_send_rx_queue(hcc, DATA_UDP_VI_QUEUE);

        ret += hcc_send_tx_queue(hcc, DATA_UDP_BE_QUEUE);
        ret += hcc_send_rx_queue(hcc, DATA_UDP_BE_QUEUE);

        ret += hcc_send_tx_queue(hcc, DATA_UDP_BK_QUEUE);
        ret += hcc_send_rx_queue(hcc, DATA_UDP_BK_QUEUE);
    }

    return ret;
}

oal_void hcc_set_tcpack_cnt(oal_uint32 ul_val)
{
    g_tcp_ack_wait_sche_cnt = ul_val;
}

oal_int32 hcc_transfer_thread(oal_void *data)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_uint32 group_ret;
    oal_int32 ret = 0;
    struct hcc_handler *hcc = NULL;
    struct sched_param param;
#ifdef _PRE_WLAN_TCP_OPT
    OAL_STATIC oal_uint8 ack_loop_count = 0;
#endif

    hcc = (struct hcc_handler *)data;

    if (oal_warn_on(hcc == NULL)) {
        OAL_IO_PRINT("%s error: hcc is null", __FUNCTION__);
        return OAL_FAIL;
    };

    OAL_IO_PRINT("hisi wifi hcc transfer thread enter\n");
    param.sched_priority = 1;
    oal_set_thread_property(current,
                            SCHED_FIFO,
                            &param,
                            HCC_TRANS_THERAD_NICE);

    allow_signal(SIGTERM);

    for (;;) {
        group_ret = 0;
        if (oal_unlikely(kthread_should_stop())) {
            OAL_IO_PRINT("hisi wifi hcc transfer thread leave\n");
            break;
        }

        ret = oal_wait_event_interruptible(hcc->hcc_transer_info.hcc_transfer_wq,
                                           (hcc_thread_wait_event_cond_check(hcc) == OAL_TRUE));
        if (oal_unlikely(ret == -ERESTARTSYS)) {
            OAL_IO_PRINT("wifi task %s was interrupted by a signal\n", oal_get_current_task_name());
            break;
        }
#ifdef _PRE_WLAN_TCP_OPT
        if (hcc->p_hmac_tcp_ack_process_func != OAL_PTR_NULL) {
            ack_loop_count++;
            if (ack_loop_count >= g_tcp_ack_wait_sche_cnt) {
                ack_loop_count = 0;
                hcc->p_hmac_tcp_ack_process_func();
            }
        }
#endif
        group_ret |= (oal_uint32)hcc_thread_process(hcc);
#ifdef _PRE_CONFIG_WLAN_THRANS_THREAD_DEBUG
        if (group_ret) {
            hcc->hcc_transer_info.thread_stat.loop_have_data_count++;
        } else {
            hcc->hcc_transer_info.thread_stat.loop_no_data_count++;
        }
#else
        oal_reference(group_ret);
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        if (!group_ret) {
            /* ��ת */
            cpu_relax();
        }
#endif
    }
#else
    OAL_IO_PRINT("hisi wifi hcc transfer thread done!%p\n", data);
#endif
    OAL_IO_PRINT("hisi wifi hcc transfer thread done!\n");
    return OAL_SUCC;
}
/*lint -e19*/
oal_module_symbol(hcc_thread_process);
oal_module_symbol(hcc_transfer_thread);
oal_module_symbol(g_tcp_ack_wait_sche_cnt);
/*lint +e19*/
OAL_STATIC oal_void hcc_dev_flowctr_timer_del(struct hcc_handler *hcc)
{
    if (oal_in_interrupt()) {
        oal_timer_delete(&hcc->hcc_transer_info.tx_flow_ctrl.flow_timer);
    } else {
        oal_timer_delete_sync(&hcc->hcc_transer_info.tx_flow_ctrl.flow_timer);
    }
}

oal_void hcc_dev_flowctrl_on(struct hcc_handler *hcc, oal_uint8 need_notify_dev)
{
    hcc_tx_flow_ctrl_info *pst_flowctrl_info = NULL;

    if (oal_unlikely(hcc == NULL)) {
        oal_warn_on(1);
        return;
    }

    pst_flowctrl_info = &hcc->hcc_transer_info.tx_flow_ctrl;
    pst_flowctrl_info->flowctrl_on_count++;

    if (oal_atomic_read(&pst_flowctrl_info->flowctrl_flag) == D2H_MSG_FLOWCTRL_OFF) {
        hcc_dev_flowctr_timer_del(hcc);
        oal_atomic_set(&pst_flowctrl_info->flowctrl_flag, D2H_MSG_FLOWCTRL_ON);
        hcc_sched_transfer(hcc);
    }

    if (need_notify_dev) {
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "Host turn on dev flow ctrl...");
        hcc_bus_send_message(hcc_to_bus(hcc), H2D_MSG_FLOWCTRL_ON);
    }
}

oal_void hcc_dev_flowctrl_off(struct hcc_handler *hcc)
{
    hcc_tx_flow_ctrl_info *pst_flowctrl_info = NULL;

    if (oal_unlikely(hcc == NULL)) {
        oal_warn_on(1);
        return;
    }

    pst_flowctrl_info = &hcc->hcc_transer_info.tx_flow_ctrl;
    pst_flowctrl_info->flowctrl_off_count++;

    if (oal_atomic_read(&pst_flowctrl_info->flowctrl_flag) == D2H_MSG_FLOWCTRL_ON) {
        oal_timer_start(&pst_flowctrl_info->flow_timer, pst_flowctrl_info->timeout);
        oal_atomic_set(&pst_flowctrl_info->flowctrl_flag, D2H_MSG_FLOWCTRL_OFF);
    }
}

oal_void hcc_transfer_queues_init(struct hcc_handler *hcc)
{
    oal_int32 i, j;
    for (i = 0; i < HCC_DIR_COUNT; i++) {
        for (j = 0; j < HCC_QUEUE_COUNT; j++) {
            oal_netbuf_head_init(&hcc->hcc_transer_info.hcc_queues[i].queues[j].data_queue);
        }
    }
}

oal_int32 hcc_tx_assem_descr_init(struct hcc_handler *hcc)
{
    oal_int32 i;
    oal_int32 ret = OAL_SUCC;
    oal_netbuf_stru *netbuf = NULL;

    oal_netbuf_head_init(&hcc->tx_descr_info.tx_assem_descr_hdr);

    /* assem descr ping-pong buff, 2 should be ok */
    hcc->tx_descr_info.descr_num = 2;

    for (i = 0; i < hcc->tx_descr_info.descr_num; i++) {
        netbuf = oal_netbuf_alloc(HISDIO_HOST2DEV_SCATT_SIZE, 0, 0);
        if (netbuf == NULL) {
            /*lint -e801*/
            goto failed_netbuf_alloc;
        }

        oal_netbuf_put(netbuf, HISDIO_HOST2DEV_SCATT_SIZE);
        memset_s(oal_netbuf_data(netbuf), oal_netbuf_len(netbuf), 0, oal_netbuf_len(netbuf));
        oal_netbuf_list_tail(&hcc->tx_descr_info.tx_assem_descr_hdr, netbuf);
        if (oal_warn_on(!oal_is_aligned(((uintptr_t)oal_netbuf_data(netbuf)), 4))) {  /* �ж��ǲ���4�ֽڶ��� */
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{(oal_uint)oal_netbuf_data(netbuf):%d not align 4}",
                             (uintptr_t)oal_netbuf_data(netbuf));
        }
    }

    oal_build_bug_on(HISDIO_HOST2DEV_SCATT_SIZE < 4);

    return ret;
failed_netbuf_alloc:
    oal_netbuf_list_purge(&hcc->tx_descr_info.tx_assem_descr_hdr);
    return -OAL_ENOMEM;
}

oal_void hcc_tx_assem_descr_exit(struct hcc_handler *hcc)
{
    oal_netbuf_list_purge(&hcc->tx_descr_info.tx_assem_descr_hdr);
}

oal_void hcc_tx_assem_info_reset(struct hcc_handler *hcc)
{
    if (oal_unlikely(hcc == NULL)) {
        oal_warn_on(1);
        return;
    }
    memset_s(hcc->hcc_transer_info.tx_assem_info.info, OAL_SIZEOF(hcc->hcc_transer_info.tx_assem_info.info),
             0, OAL_SIZEOF(hcc->hcc_transer_info.tx_assem_info.info));
}

oal_void oal_sdio_rx_assem_info_reset(struct hcc_handler *hcc)
{
    if (oal_unlikely(hcc == NULL)) {
        oal_warn_on(1);
        return;
    }
    memset_s(hcc->hcc_transer_info.rx_assem_info.info, OAL_SIZEOF(hcc->hcc_transer_info.rx_assem_info.info),
             0, OAL_SIZEOF(hcc->hcc_transer_info.rx_assem_info.info));
}

oal_void hcc_assem_info_init(struct hcc_handler *hcc)
{
    hcc->hcc_transer_info.tx_assem_info.assemble_max_count = hcc_get_assemble_count();
    hcc_tx_assem_info_reset(hcc);
    oal_sdio_rx_assem_info_reset(hcc);
    oal_netbuf_list_head_init(&hcc->hcc_transer_info.tx_assem_info.assembled_head);
}

oal_void hcc_trans_limit_parm_init(struct hcc_handler *hcc)
{
    oal_int32 i;
    hcc_trans_queues *pst_hcc_tx_queues = &hcc->hcc_transer_info.hcc_queues[HCC_TX];

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].burst_limit = (oal_uint32)HCC_FLUSH_ALL;
        hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[i].burst_limit = (oal_uint32)HCC_FLUSH_ALL;
    }

    pst_hcc_tx_queues->queues[CTRL_QUEUE].burst_limit          = CTRL_BURST_LIMIT;
    pst_hcc_tx_queues->queues[DATA_HI_QUEUE].burst_limit       = DATA_HI_BURST_LIMIT;
    pst_hcc_tx_queues->queues[DATA_LO_QUEUE].burst_limit       = DATA_LO_BURST_LIMIT;
    pst_hcc_tx_queues->queues[DATA_TCP_DATA_QUEUE].burst_limit = TCP_DATA_BURST_LIMIT;
    pst_hcc_tx_queues->queues[DATA_TCP_ACK_QUEUE].burst_limit  = TCP_ACK_BURST_LIMIT;
    pst_hcc_tx_queues->queues[DATA_UDP_BK_QUEUE].burst_limit   = UDP_BK_BURST_LIMIT;
    pst_hcc_tx_queues->queues[DATA_UDP_BE_QUEUE].burst_limit   = UDP_BE_BURST_LIMIT;
    pst_hcc_tx_queues->queues[DATA_UDP_VI_QUEUE].burst_limit   = UDP_VI_BURST_LIMIT;
    pst_hcc_tx_queues->queues[DATA_UDP_VO_QUEUE].burst_limit   = UDP_VO_BURST_LIMIT;
}

oal_void hcc_trans_send_mode_init(struct hcc_handler *hcc)
{
    oal_int32 i;

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].send_mode = HCC_ASSEM_SEND;
    }
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].send_mode = HCC_SINGLE_SEND;
}

OAL_STATIC OAL_VOLATILE oal_int32 g_flowctrl_info_flag = 0;
OAL_STATIC oal_void hcc_dev_flow_ctrl_timeout_isr(uintptr_t arg)
{
    struct hcc_handler *hcc = (struct hcc_handler *)arg;
    if (hcc == NULL) {
        OAL_IO_PRINT("hcc is null\n");
        return;
    }
    /* flowctrl lock too much time. */
    declare_dft_trace_key_info("hcc_flow_lock_timeout", OAL_DFT_TRACE_EXCEP);
    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{SDIO flow ctrl had off for %u ms, it's a long time}",
                     (oal_uint32)hcc->hcc_transer_info.tx_flow_ctrl.timeout);
    g_flowctrl_info_flag = 0;

    /* If work is idle,queue a new work. */
    if (oal_work_is_busy(&hcc->hcc_transer_info.tx_flow_ctrl.worker.work) == 0) {
        oal_queue_delayed_system_work(&hcc->hcc_transer_info.tx_flow_ctrl.worker, 0);
    }
}

oal_void hcc_flowctrl_deadlock_detect_worker(oal_work_stru *pst_flow_work)
{
    struct hcc_handler *hcc = hcc_get_110x_handler();
    oal_uint8 uc_credit;
    if (hcc == NULL) {
        OAL_IO_PRINT("hcc_flowctrl_deadlock_detect_worker hcc is null\n");
        return;
    }
    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hcc_flowctrl_deadlock_detect_worker action,%d}",
                     g_flowctrl_info_flag);
    if (g_flowctrl_info_flag == 0) {
        g_flowctrl_info_flag = 1;
        oal_smp_mb();
        /* queue system_wq delay work,and send other message 20ms later. */
        /* print device mem */
        hcc_print_device_mem_info();
        oal_queue_delayed_system_work(&hcc->hcc_transer_info.tx_flow_ctrl.worker, oal_msecs_to_jiffies(20));
    } else if (g_flowctrl_info_flag == 1) {
        uc_credit = hcc->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt;
        /* print device mem */
        if (g_hcc_flowctrl_detect_panic && uc_credit <= g_hcc_credit_bottom_value) {
            hcc_trigger_device_panic();
            hcc->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt = HCC_FLOW_HIGH_PRI_BUFF_CNT;
#ifdef _PRE_WLAN_1102A_CHR
            chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV,
                CHR_WIFI_DRV_EVENT_PLAT, CHR_PLAT_DRV_ERROR_HCC_DEADLOCK);
#endif
        } else {
            hcc_print_device_mem_info();
        }
    }

    return;
}

oal_void hcc_trans_flow_ctrl_info_reset(struct hcc_handler *hcc)
{
    hcc_tx_flow_ctrl_info *pst_flowctrl_info = NULL;

    if (oal_unlikely(hcc == NULL)) {
        oal_warn_on(1);
        return;
    }

    pst_flowctrl_info = &hcc->hcc_transer_info.tx_flow_ctrl;

    /* disable flow ctrl detect timer */
    hcc_dev_flowctr_timer_del(hcc);
    oal_cancel_delayed_work_sync(&pst_flowctrl_info->worker);

    oal_atomic_set(&pst_flowctrl_info->flowctrl_flag, D2H_MSG_FLOWCTRL_OFF);
    pst_flowctrl_info->uc_hipriority_cnt = 0; /* Ĭ�ϲ������͵ȴ�wcpu����credit */
}

OAL_STATIC oal_void hcc_trans_flow_ctrl_info_init(struct hcc_handler *hcc)
{
    oal_int32 i;
    hcc_tx_flow_ctrl_info *pst_flowctrl_info;

    pst_flowctrl_info = &hcc->hcc_transer_info.tx_flow_ctrl;

    oal_atomic_set(&pst_flowctrl_info->flowctrl_flag, D2H_MSG_FLOWCTRL_ON);
    pst_flowctrl_info->flowctrl_off_count = 0;
    pst_flowctrl_info->flowctrl_on_count = 0;
    oal_spin_lock_init(&pst_flowctrl_info->lock);
    pst_flowctrl_info->timeout = 20 * 1000;
    oal_init_delayed_work(&pst_flowctrl_info->worker, hcc_flowctrl_deadlock_detect_worker);
    oal_timer_init(&pst_flowctrl_info->flow_timer,
                   pst_flowctrl_info->timeout,
                   hcc_dev_flow_ctrl_timeout_isr,
                   (uintptr_t)hcc);

    pst_flowctrl_info->uc_hipriority_cnt = HCC_FLOW_HIGH_PRI_BUFF_CNT;
    oal_spin_lock_init(&pst_flowctrl_info->st_hipri_lock);

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].flow_ctrl.enable = OAL_TRUE;
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].flow_ctrl.flow_type = HCC_FLOWCTRL_SDIO;
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].flow_ctrl.is_stopped = OAL_FALSE;
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].flow_ctrl.low_waterline = HCC_TX_LOW_WATERLINE;
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].flow_ctrl.high_waterline = HCC_TX_HIGH_WATERLINE;
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].netbuf_pool_type = HCC_NETBUF_NORMAL_QUEUE;
    }

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].flow_ctrl.flow_type = HCC_FLOWCTRL_CREDIT;

    for (i = 0; i < HCC_QUEUE_COUNT; i++) {
        hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[i].flow_ctrl.enable = OAL_TRUE;
        hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[i].flow_ctrl.is_stopped = OAL_FALSE;
        hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[i].flow_ctrl.low_waterline = HCC_RX_LOW_WATERLINE;
        hcc->hcc_transer_info.hcc_queues[HCC_RX].queues[i].flow_ctrl.high_waterline = HCC_RX_HIGH_WATERLINE;
    }

    /* DEVICE û�и������ȼ�Ԥ���ڴ棬���ж��ж���Ҫ���ء� */
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].flow_ctrl.enable = OAL_FALSE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[CTRL_QUEUE].flow_ctrl.low_waterline = CTRL_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[CTRL_QUEUE].flow_ctrl.high_waterline = CTRL_HIGH_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[CTRL_QUEUE].netbuf_pool_type = HCC_NETBUF_HIGH_QUEUE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].flow_ctrl.low_waterline = DATA_HI_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].flow_ctrl.high_waterline = DATA_HI_HIGH_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_HI_QUEUE].netbuf_pool_type = HCC_NETBUF_HIGH_QUEUE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_LO_QUEUE].flow_ctrl.low_waterline = DATA_LO_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_LO_QUEUE].flow_ctrl.high_waterline = DATA_LO_HIGH_WATERLINE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_TCP_DATA_QUEUE].flow_ctrl.low_waterline = TCP_DATA_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_TCP_DATA_QUEUE].flow_ctrl.high_waterline = TCP_DATA_HIGH_WATERLINE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_TCP_ACK_QUEUE].flow_ctrl.low_waterline = TCP_ACK_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_TCP_ACK_QUEUE].flow_ctrl.high_waterline = TCP_ACK_HIGH_WATERLINE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_UDP_BK_QUEUE].flow_ctrl.low_waterline = UDP_BK_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_UDP_BK_QUEUE].flow_ctrl.high_waterline = UDP_BK_HIGH_WATERLINE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_UDP_BE_QUEUE].flow_ctrl.low_waterline = UDP_BE_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_UDP_BE_QUEUE].flow_ctrl.high_waterline = UDP_BE_HIGH_WATERLINE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_UDP_VI_QUEUE].flow_ctrl.low_waterline = UDP_VI_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_UDP_VI_QUEUE].flow_ctrl.high_waterline = UDP_VI_HIGH_WATERLINE;

    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_UDP_VO_QUEUE].flow_ctrl.low_waterline = UDP_VO_LOW_WATERLINE;
    hcc->hcc_transer_info.hcc_queues[HCC_TX].queues[DATA_UDP_VO_QUEUE].flow_ctrl.high_waterline = UDP_VO_HIGH_WATERLINE;
}
oal_int32 hcc_flow_on_callback(oal_void *data)
{
    hcc_dev_flowctrl_on((struct hcc_handler *)data, 0);
    return OAL_SUCC;
}

oal_int32 hcc_flow_off_callback(oal_void *data)
{
    hcc_dev_flowctrl_off((struct hcc_handler *)data);
    return OAL_SUCC;
}

/*
 * �� �� ��  : hcc_credit_update_callback
 * ��������  : D2H_MSG_CREDIT_UPDATE msg��Ӧ�Ĵ�����
 */
oal_int32 hcc_credit_update_callback(oal_void *data)
{
    oal_uint8 uc_large_cnt;
    struct hcc_handler *hcc = (struct hcc_handler *)data;
    struct oal_sdio *hi_sdio = oal_get_sdio_default_handler();

    if (oal_warn_on(!hi_sdio)) {
        OAL_IO_PRINT("hcc_credit_update_callback set fail: hi_sdio is null!\n");
        return OAL_FAIL;
    }

    uc_large_cnt = hisdio_large_pkt_get(hi_sdio->sdio_extend->credit_info);

    oal_spin_lock(&(hcc->hcc_transer_info.tx_flow_ctrl.st_hipri_lock));

    hcc->hcc_transer_info.tx_flow_ctrl.uc_hipriority_cnt = uc_large_cnt;

    oal_spin_unlock(&(hcc->hcc_transer_info.tx_flow_ctrl.st_hipri_lock));

    hcc->hcc_transer_info.tx_flow_ctrl.flowctrl_hipri_update_count++;

    hcc_sched_transfer(hcc);

    return OAL_SUCC;
}

oal_int32 hcc_high_pkts_loss_callback(oal_void *data)
{
    oal_reference(data);
    declare_dft_trace_key_info("sdio_high_pkts_loss", OAL_DFT_TRACE_EXCEP);
    return OAL_SUCC;
}

/*
 * Prototype    : hcc_module_init
 * Description  : hcc init function, also probe sdio function.
 * Input        : hcc_msg_rx dev_ready_cb   the callback function called when device ready
 *                oal_void* init_data       the input param for the callback function.
 * Return Value : struct hcc_handler*
 */
struct hcc_handler *hcc_module_init(hcc_bus_dev *pst_bus_dev)
{
    oal_uint32 ul_tx_max_len;
    struct hcc_handler *hcc = NULL;

    oal_build_bug_on(HCC_HDR_LEN > HCC_HDR_TOTAL_LEN);

    /* main_type:4 ֻ�ܱ�ʾ16������ */
    oal_build_bug_on(HCC_ACTION_TYPE_BUTT > 15);

    /* 1544-the max netbuf len of device */
    /*lint -e778*/
    g_hcc_tx_max_buf_len = OAL_ROUND_DOWN(HSDIO_HOST2DEV_PKTS_MAX_LEN, HISDIO_H2D_SCATT_BUFFLEN_ALIGN);
    /* one byte,2^8=256 */
    ul_tx_max_len = (1 << (8 + HISDIO_H2D_SCATT_BUFFLEN_ALIGN_BITS));
    g_hcc_tx_max_buf_len = oal_min(g_hcc_tx_max_buf_len, ul_tx_max_len);
    /*lint +e778*/
    hcc = (struct hcc_handler *)oal_memalloc(OAL_SIZEOF(struct hcc_handler));
    if (hcc == NULL) {
        OAL_IO_PRINT("hcc mem alloc  failed!\n");
        return NULL;
    }

    if (oal_warn_on(pst_bus_dev == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: pst_bus_dev is null", __FUNCTION__);
        oal_free(hcc);
        return NULL;
    };

    memset_s((oal_void *)hcc, OAL_SIZEOF(struct hcc_handler), 0, OAL_SIZEOF(struct hcc_handler));

    hcc->bus_dev = pst_bus_dev;

    hcc->hdr_rever_max_len = HCC_HDR_RESERVED_MAX_LEN;

    /* disable hcc default */
    oal_atomic_set(&hcc->state, HCC_OFF);

    oal_wait_queue_init_head(&hcc->hcc_transer_info.hcc_transfer_wq); /* queues init */
    hcc_transfer_queues_init(hcc);

    hcc_trans_flow_ctrl_info_init(hcc);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_init(&hcc->tx_transfer_lock);
#endif

    oal_wait_queue_init_head(&hcc->hcc_transer_info.tx_flow_ctrl.wait_queue);

#if !(defined(_PRE_WLAN_TCP_OPT) || defined(WIN32))
    hcc->hcc_transer_info.hcc_transfer_thread = oal_thread_create(hcc_transfer_thread,
                                                                  hcc,
                                                                  NULL,
                                                                  "hisi_hcc",
                                                                  HCC_TRANS_THREAD_POLICY,
                                                                  HCC_TRANS_THERAD_PRIORITY,
                                                                  -1);
    if (!hcc->hcc_transer_info.hcc_transfer_thread) {
        OAL_IO_PRINT("hcc thread create failed!\n");
        goto failed_create_hcc_thread;
    }
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (hcc_transfer_rx_register(hcc, (oal_void *)hcc, hcc_bus_rx_handler) != OAL_SUCC) {
        OAL_IO_PRINT("sdio rx transfer callback register failed!\n");
        goto failed_rx_cb_reg;
    }
#endif

    hcc_assem_info_init(hcc);
    hcc_trans_limit_parm_init(hcc);
    hcc_trans_send_mode_init(hcc);

    oal_build_bug_on(!oal_is_aligned(HISDIO_HOST2DEV_SCATT_SIZE, HISDIO_H2D_SCATT_BUFFLEN_ALIGN));

    if (hcc_tx_assem_descr_init(hcc) != OAL_SUCC) {
        OAL_IO_PRINT("hcc tx assem descrt alloc failed!\n");
        goto failed_tx_assem_descr_alloc;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (hcc_message_register(hcc, D2H_MSG_FLOWCTRL_ON, hcc_flow_on_callback, hcc) != OAL_SUCC) {
        OAL_IO_PRINT("register flow ctrl on failed!\n");
        goto failed_reg_flowon_msg;
    }

    if (hcc_message_register(hcc, D2H_MSG_FLOWCTRL_OFF, hcc_flow_off_callback, hcc) != OAL_SUCC) {
        OAL_IO_PRINT("register flow ctrl off failed!\n");
        goto failed_reg_flowoff_msg;
    }

    hcc_message_register(hcc, D2H_MSG_CREDIT_UPDATE, hcc_credit_update_callback, hcc);

    hcc_message_register(hcc, D2H_MSG_HIGH_PKT_LOSS, hcc_high_pkts_loss_callback, hcc);
#endif
    if (hcc_test_init_module(hcc) != OAL_SUCC) {
        OAL_IO_PRINT("register flow ctrl off failed!\n");
        goto failed_hcc_test_init;
    }

    oal_wake_lock_init(&hcc->tx_wake_lock, "hcc_tx");

    OAL_IO_PRINT("hcc_module_init dev id:%d succ\n", pst_bus_dev->dev_id);

    /* g_hcc_tc is used to test! */
    g_hcc_tc = hcc;

    return hcc;
failed_hcc_test_init:
    hcc_message_unregister(hcc, D2H_MSG_FLOWCTRL_OFF);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
failed_reg_flowoff_msg:
    hcc_message_unregister(hcc, D2H_MSG_FLOWCTRL_ON);
failed_reg_flowon_msg:
#endif
    hcc_tx_assem_descr_exit(hcc);
failed_tx_assem_descr_alloc:
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hcc_bus_transfer_rx_unregister(hcc_to_bus(hcc));
failed_rx_cb_reg:
#endif
#if !defined(_PRE_WLAN_TCP_OPT) || defined(WIN32)
    oal_thread_stop(hcc->hcc_transer_info.hcc_transfer_thread, NULL);
    hcc->hcc_transer_info.hcc_transfer_thread = NULL;
#endif
#if !(defined(_PRE_WLAN_TCP_OPT) || defined(WIN32))
failed_create_hcc_thread:
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_destroy(&hcc->tx_transfer_lock);
#endif
#endif
    oal_free(hcc);
    /*lint +e801*/
    return NULL;
}

/*
 * Prototype    : hcc_module_exit
 * Description  : the exit function for hcc.
 */
oal_void hcc_module_exit(struct hcc_handler *hcc)
{
    if (oal_unlikely(hcc == NULL)) {
        oal_warn_on(1);
        return;
    }

    oal_wake_lock_exit(&hcc->tx_wake_lock);
    hcc_test_exit_module(hcc);
    hcc_message_unregister(hcc, D2H_MSG_FLOWCTRL_OFF);
    hcc_message_unregister(hcc, D2H_MSG_FLOWCTRL_ON);
    hcc_tx_assem_descr_exit(hcc);
    hcc_bus_transfer_rx_unregister(hcc_to_bus(hcc));
#if !defined(_PRE_WLAN_TCP_OPT) || defined(WIN32)
    oal_thread_stop(hcc->hcc_transer_info.hcc_transfer_thread, NULL);
    hcc->hcc_transer_info.hcc_transfer_thread = NULL;
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_destroy(&hcc->tx_transfer_lock);
#endif

    oal_timer_delete_sync(&hcc->hcc_transer_info.tx_flow_ctrl.flow_timer);
    oal_cancel_delayed_work_sync(&hcc->hcc_transer_info.tx_flow_ctrl.worker);

    oal_free(hcc);
    g_hcc_tc = NULL;
}

#ifdef CONFIG_PRINTK
oal_void hcc_sched_transfer_test(oal_void)
{
    hcc_sched_transfer(g_hcc_tc);
}

oal_void hcc_dev_flowctrl_on_test(oal_void)
{
    if (g_hcc_tc != NULL) {
        hcc_dev_flowctrl_on(g_hcc_tc, 1);
    }
}
#endif
#ifdef CONFIG_MMC
oal_void hcc_device_info_dump(oal_void)
{
    struct hcc_handler *hcc = hcc_get_110x_handler();
    OAL_IO_PRINT("hcc_device_info_dump\n");
    hcc_bus_send_message(hcc_to_bus(hcc), H2D_MSG_DEVICE_INFO_DUMP);
}

oal_void hcc_device_mem_dump(oal_void)
{
    struct hcc_handler *hcc = hcc_get_110x_handler();
    OAL_IO_PRINT("hcc_device_mem_dump\n");
    hcc_bus_send_message(hcc_to_bus(hcc), H2D_MSG_DEVICE_MEM_DUMP);
}
#endif
/*lint -e19 */
oal_void hcc_trigger_device_panic(oal_void)
{
#ifdef CONFIG_MMC
    struct hcc_handler *hcc = hcc_get_110x_handler();
    wlan_pm_disable();
    hcc_bus_send_message(hcc_to_bus(hcc), H2D_MSG_TEST);
    wlan_pm_enable();
#endif
}
oal_module_symbol(hcc_trigger_device_panic);

oal_void hcc_print_device_mem_info(oal_void)
{
#ifdef CONFIG_MMC
    struct hcc_handler *hcc = hcc_get_110x_handler();

    wlan_pm_disable();
    hcc_bus_send_message(hcc_to_bus(hcc), H2D_MSG_DEVICE_MEM_INFO);
    wlan_pm_enable();
#endif
}
oal_module_symbol(hcc_print_device_mem_info);

oal_module_symbol(hcc_host_set_flowctl_param);
oal_module_symbol(hcc_host_get_flowctl_param);
oal_module_symbol(hcc_host_get_flowctl_stat);
