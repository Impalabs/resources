

/* Header File Including */
#define HI11XX_LOG_MODULE_NAME     "[WLAN_PM]"
#define HI11XX_LOG_MODULE_NAME_VAR wlan_pm_loglevel
#include <linux/module.h> /* kernel module definitions */
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/kobject.h>
#include <linux/irq.h>

#include <linux/mmc/sdio.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/host.h>
#include <linux/gpio.h>
#include <linux/pm_wakeup.h>

#include "oal_sdio_comm.h"
#include "oal_sdio_host_if.h"
#include "oal_hcc_bus.h"

#include "plat_type.h"
#include "board.h"
#include "plat_pm_wlan.h"
#include "plat_pm.h"

#include "lpcpu_feature.h"
#include "oal_hcc_host_if.h"
#include "oam_ext_if.h"
#include "bfgx_exception_rst.h"
#include "securec.h"

#include "platform_oneimage_define.h"
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_PLAT_PM_WLAN_C

/* Global Variable Definition */
struct wlan_pm_s *g_gpst_wlan_pm_info = OAL_PTR_NULL;

pm_callback_stru g_gst_wlan_pm_callback = {
    .pm_wakeup_dev = wlan_pm_wakeup_dev,
    .pm_state_get = wlan_pm_state_get,
    .pm_wakeup_host = wlan_pm_wakeup_host,
    .pm_feed_wdg = wlan_pm_feed_wdg,
    .pm_wakeup_dev_ack = wlan_pm_wakeup_dev_ack,
    .pm_disable = wlan_pm_disable_check_wakeup,

};

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static RAW_NOTIFIER_HEAD(wifi_pm_chain);
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_uint8 g_custom_cali_done = OAL_FALSE;
oal_uint8 wlan_pm_get_custom_cali_done(void)
{
    return g_custom_cali_done;
}
oal_module_symbol(wlan_pm_get_custom_cali_done);

void wlan_pm_set_custom_cali_done(oal_uint8 cali_done)
{
    g_custom_cali_done = cali_done;
}
oal_module_symbol(wlan_pm_set_custom_cali_done);
#endif
oal_bool_enum g_wlan_pm_switch = OAL_FALSE;
oal_uint8 wlan_pm_get_switch(void)
{
    return g_wlan_pm_switch;
}
oal_module_symbol(wlan_pm_get_switch);

void wlan_pm_set_switch(oal_uint8 pm_switch)
{
    g_wlan_pm_switch = pm_switch;
}
oal_module_symbol(wlan_pm_set_switch);

oal_uint8 g_wlan_device_pm_switch = OAL_TRUE;  // device �͹��Ŀ���

oal_uint8 g_wlan_fast_ps_mode_dyn_ctl = 0;  // app layer dynamic ctrl enable
oal_uint8 g_wlan_fast_check_cnt = 10; // deviceÿ20ms���һ��������g_wlan_fast_check_cnt�����������շ������͹���ģʽ
oal_uint8 wlan_pm_get_fast_check_cnt(void)
{
    return g_wlan_fast_check_cnt;
}
oal_module_symbol(wlan_pm_get_fast_check_cnt);

void wlan_pm_set_fast_check_cnt(oal_uint8 fast_check_cnt)
{
    g_wlan_fast_check_cnt = fast_check_cnt;
}
oal_module_symbol(wlan_pm_set_fast_check_cnt);
#ifdef _PRE_WLAN_RF_AUTOCALI
oal_uint8 g_autocali_switch = OAL_FALSE;
EXPORT_SYMBOL_GPL(g_autocali_switch);
#endif

#ifdef _PRE_WLAN_DOWNLOAD_PM
oal_uint16 g_download_rate_limit_pps = 0;
oal_uint16 wlan_pm_get_download_rate_limit_pps(void)
{
    return g_download_rate_limit_pps;
}
oal_module_symbol(wlan_pm_get_download_rate_limit_pps);

void wlan_pm_set_download_rate_limit_pps(oal_uint16 rate_limit_pps)
{
    g_download_rate_limit_pps = rate_limit_pps;
}
oal_module_symbol(wlan_pm_set_download_rate_limit_pps);
#endif

EXPORT_SYMBOL_GPL(g_wlan_pm_switch);
EXPORT_SYMBOL_GPL(g_wlan_device_pm_switch);
EXPORT_SYMBOL_GPL(g_wlan_fast_check_cnt);
EXPORT_SYMBOL_GPL(g_wlan_fast_ps_mode_dyn_ctl);

/* 30000ms/100ms = 300 cnt */
oal_uint32 g_wlan_sleep_request_forbid_limit = (30000) / (WLAN_SLEEP_TIMER_PERIOD * WLAN_SLEEP_DEFAULT_CHECK_CNT);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
EXPORT_SYMBOL_GPL(g_custom_cali_done);
#endif

void wlan_pm_wakeup_work(oal_work_stru *pst_worker);
void wlan_pm_sleep_work(oal_work_stru *pst_worker);

void wlan_pm_wdg_timeout(unsigned long data);

int32 wlan_pm_wakeup_done_callback(void *data);
int32 wlan_pm_close_done_callback(void *data);
int32 wlan_pm_open_bcpu_done_callback(void *data);
int32 wlan_pm_close_bcpu_done_callback(void *data);
int32 wlan_pm_halt_bcpu_done_callback(void *data);
void wlan_pm_deepsleep_delay_timeout(unsigned long data);
int32 wlan_pm_stop_deepsleep_delay_timer(struct wlan_pm_s *pm_data);

int32 wlan_pm_stop_wdg(struct wlan_pm_s *pst_wlan_pm_info);
oal_int wlan_pm_work_submit(struct wlan_pm_s *pst_wlan_pm, oal_work_stru *pst_worker);
void wlan_pm_info_clean(void);


#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_int32 wlan_pm_register_notifier(struct notifier_block *nb)
{
    return raw_notifier_chain_register(&wifi_pm_chain, nb);
}

oal_void wlan_pm_unregister_notifier(struct notifier_block *nb)
{
    raw_notifier_chain_unregister(&wifi_pm_chain, nb);
}
#endif

#ifdef _PRE_WLAN_FEATURE_DFR
extern oal_uint32 g_cali_excp_dfr_count;     /* ����У׼�쳣����dfr�ָ��Ĵ��� */
dfr_info_stru g_st_dfr_info;                 /* DFR�쳣��λ���� */
oal_module_symbol(g_st_dfr_info);
#endif

#ifdef CONFIG_HUAWEI_DSM

static struct dsm_dev g_dsm_wifi = {
    .name = "dsm_wifi",
    .device_name = NULL,
    .ic_name = NULL,
    .module_name = NULL,
    .fops = NULL,
    .buff_size = DSM_DEV_BUFF_SIZE,
};

struct dsm_client *g_hw_1102a_dsm_client = NULL;

void hw_1102a_register_wifi_dsm_client(void)
{
    if (g_hw_1102a_dsm_client == NULL) {
        g_hw_1102a_dsm_client = dsm_register_client(&g_dsm_wifi);
    }
}
void hw_1102a_unregister_wifi_dsm_client(void)
{
    if (g_hw_1102a_dsm_client != NULL) {
        dsm_unregister_client(g_hw_1102a_dsm_client, &g_dsm_wifi);
        g_hw_1102a_dsm_client = NULL;
    }
}
#define LOG_BUF_SIZE 512
void hw_1102a_dsm_client_notify(int dsm_id, const char *fmt, ...)
{
    char buf[LOG_BUF_SIZE] = {0};
    va_list ap;
    int size;

    declare_dft_trace_key_info("hw_1102a_dsm_client_notify", OAL_DFT_TRACE_FAIL);
    if (g_hw_1102a_dsm_client != NULL) {
        if (fmt != NULL) {
            va_start(ap, fmt);
            size = vsnprintf_s(buf, LOG_BUF_SIZE, LOG_BUF_SIZE - 1, fmt, ap);
            va_end(ap);
            if (size < 0) {
                oam_warning_log0(0, OAM_SF_PWR, "buf copy failed");
                return;
            }
        }

        if (!dsm_client_ocuppy(g_hw_1102a_dsm_client)) {
            dsm_client_record(g_hw_1102a_dsm_client, buf);
            dsm_client_notify(g_hw_1102a_dsm_client, dsm_id);
            OAL_IO_PRINT("[I]wifi dsm client notify success, dsm_id=%d[%s]\n", dsm_id, buf);
        } else {
            dsm_client_unocuppy(g_hw_1102a_dsm_client);
            if (!dsm_client_ocuppy(g_hw_1102a_dsm_client)) {
                dsm_client_record(g_hw_1102a_dsm_client, buf);
                dsm_client_notify(g_hw_1102a_dsm_client, dsm_id);
                OAL_IO_PRINT("[I]wifi dsm notify success, dsm_id=%d[%s]\n", dsm_id, buf);
            } else {
                OAL_IO_PRINT("[E]wifi dsm client ocuppy, dsm notify failed, dsm_id=%d\n", dsm_id);
            }
        }
    } else {
        OAL_IO_PRINT("[E]wifi dsm client is null, dsm notify failed, dsm_id=%d\n", dsm_id);
    }
}
EXPORT_SYMBOL(hw_1102a_dsm_client_notify);
#endif

/*
 * �� �� ��  : wlan_pm_get_drv
 * ��������  : ��ȡȫ��wlan�ṹ
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
struct wlan_pm_s *wlan_pm_get_drv(oal_void)
{
    return g_gpst_wlan_pm_info;
}

EXPORT_SYMBOL_GPL(wlan_pm_get_drv);

/*
 * �� �� ��  : wlan_pm_sleep_request
 * ��������  : ����sleep �����device
 * �� �� ֵ  : SUCC/FAIL
 */
OAL_STATIC oal_int32 wlan_pm_sleep_request(struct wlan_pm_s *pst_wlan_pm)
{
    int32_t ret;
    uint32_t time_left;

    pst_wlan_pm->ul_sleep_stage = SLEEP_REQ_SND;

    oal_init_completion(&pst_wlan_pm->st_sleep_request_ack);

    if (oal_print_rate_limit(PRINT_RATE_MINUTE)) { /* 1���Ӵ�ӡһ�� */
        hcc_bus_chip_info(pst_wlan_pm->pst_bus, OAL_FALSE, OAL_FALSE);
    }

    ret = hcc_bus_send_message(pst_wlan_pm->pst_bus, H2D_MSG_SLEEP_REQ);
    if (ret != OAL_SUCC) {
        pst_wlan_pm->ul_sleep_fail_request++;
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_sleep_request fail !\n");
        return OAL_ERR_CODE_SLEEP_FAIL;
    }

    oal_print_hi11xx_log(HI11XX_LOG_INFO, "sleep request send!");
    up(&pst_wlan_pm->pst_bus->rx_sema);

    time_left = oal_wait_for_completion_timeout(&pst_wlan_pm->st_sleep_request_ack,
                                                (uint32_t)oal_msecs_to_jiffies(WLAN_SLEEP_MSG_WAIT_TIMEOUT));
    if (time_left == 0) {
        pst_wlan_pm->ul_sleep_fail_wait_timeout++;
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_sleep_work wait completion fail !\n");
        return OAL_ERR_CODE_SLEEP_FAIL;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : wlan_pm_allow_sleep_callback
 * ��������  : deviceӦ��allow_sleep��Ϣ����
 */
oal_int32 wlan_pm_allow_sleep_callback(void *data)
{
    struct wlan_pm_s *pst_wlan_pm = (struct wlan_pm_s *)data;

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "wlan_pm_allow_sleep_callback");

    pst_wlan_pm->ul_sleep_stage = SLEEP_ALLOW_RCV;
    oal_complete(&pst_wlan_pm->st_sleep_request_ack);

    return SUCCESS;
}

/*
 * �� �� ��  : wlan_pm_disallow_sleep_callback
 * ��������  : deviceӦ��allow_sleep��Ϣ����
 */
oal_int32 wlan_pm_disallow_sleep_callback(void *data)
{
    struct wlan_pm_s *pst_wlan_pm = (struct wlan_pm_s *)data;

    if (oal_print_rate_limit(PRINT_RATE_SECOND)) { /* 1s��ӡһ�� */
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "sleep request dev disalow, device busy");
    }

    pst_wlan_pm->ul_sleep_stage = SLEEP_DISALLOW_RCV;
    oal_complete(&pst_wlan_pm->st_sleep_request_ack);

    return SUCCESS;
}

OAL_STATIC hcc_switch_action g_plat_pm_switch_action;
OAL_STATIC oal_int32 wlan_switch_action_callback(oal_uint32 dev_id, hcc_bus *old_bus, hcc_bus *new_bus, oal_void *data)
{
    struct wlan_pm_s *pst_wlan_pm = NULL;

    if (data == NULL) {
        return -OAL_EINVAL;
    }

    if (dev_id != HCC_CHIP_110X_DEV) {
        /* ignore other wlan dev */
        return OAL_SUCC;
    }

    pst_wlan_pm = (struct wlan_pm_s *)data;

    /* Update new bus */
    pst_wlan_pm->pst_bus = new_bus;
    pst_wlan_pm->pst_bus->pst_pm_callback = &g_gst_wlan_pm_callback;

    return OAL_SUCC;
}

static void wlan_pm_init_ext2(struct wlan_pm_s *pst_wlan_pm)
{
    hcc_message_register(hcc_get_110x_handler(),
                         D2H_MSG_WAKEUP_SUCC,
                         wlan_pm_wakeup_done_callback,
                         pst_wlan_pm);
    hcc_message_register(hcc_get_110x_handler(),
                         D2H_MSG_ALLOW_SLEEP,
                         wlan_pm_allow_sleep_callback,
                         pst_wlan_pm);
    hcc_message_register(hcc_get_110x_handler(),
                         D2H_MSG_DISALLOW_SLEEP,
                         wlan_pm_disallow_sleep_callback,
                         pst_wlan_pm);
    hcc_message_register(hcc_get_110x_handler(),
                         D2H_MSG_POWEROFF_ACK,
                         wlan_pm_close_done_callback,
                         pst_wlan_pm);
    hcc_message_register(hcc_get_110x_handler(),
                         D2H_MSG_OPEN_BCPU_ACK,
                         wlan_pm_open_bcpu_done_callback,
                         pst_wlan_pm);
    hcc_message_register(hcc_get_110x_handler(),
                         D2H_MSG_CLOSE_BCPU_ACK,
                         wlan_pm_close_bcpu_done_callback,
                         pst_wlan_pm);
    hcc_message_register(hcc_get_110x_handler(),
                         D2H_MSG_HALT_BCPU,
                         wlan_pm_halt_bcpu_done_callback,
                         pst_wlan_pm);

    pst_wlan_pm->pst_bus->data_int_count = 0;
    pst_wlan_pm->pst_bus->wakeup_int_count = 0;
}

static int32 wlan_pm_init_ext1(struct wlan_pm_s *pst_wlan_pm)
{
    pst_wlan_pm->ul_wlan_pm_enable = OAL_FALSE;
    pst_wlan_pm->ul_apmode_allow_pm_flag = OAL_TRUE; /* Ĭ�������µ� */

    /* work queue��ʼ�� */
    pst_wlan_pm->pst_pm_wq = oal_create_singlethread_workqueue("wlan_pm_wq");
    if (pst_wlan_pm->pst_pm_wq == NULL) {
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "Failed to create wlan_pm_wq!");
        kfree(pst_wlan_pm);
        return OAL_PTR_NULL;
    }
    /* register wakeup and sleep work */
    oal_init_work(&pst_wlan_pm->st_wakeup_work, wlan_pm_wakeup_work);
    oal_init_work(&pst_wlan_pm->st_sleep_work, wlan_pm_sleep_work);

    /* ��ʼ��оƬ�Լ�work */
    oal_init_work(&pst_wlan_pm->st_ram_reg_test_work, wlan_device_mem_check_work);

    /* sleep timer��ʼ�� */
    init_timer(&pst_wlan_pm->st_watchdog_timer);
    pst_wlan_pm->st_watchdog_timer.data = (uintptr_t)pst_wlan_pm;
    pst_wlan_pm->st_watchdog_timer.function = (void *)wlan_pm_wdg_timeout;
    pst_wlan_pm->ul_wdg_timeout_cnt = WLAN_SLEEP_DEFAULT_CHECK_CNT;
    pst_wlan_pm->ul_wdg_timeout_curr_cnt = 0;
    pst_wlan_pm->ul_packet_cnt = 0;
    pst_wlan_pm->ul_packet_total_cnt = 0;
    pst_wlan_pm->ul_packet_check_time = 0;
    pst_wlan_pm->ul_sleep_forbid_check_time = 0;

    /* sleep timer��ʼ�� */
    init_timer(&pst_wlan_pm->st_deepsleep_delay_timer);
    pst_wlan_pm->st_deepsleep_delay_timer.data = (uintptr_t)pst_wlan_pm;
    pst_wlan_pm->st_deepsleep_delay_timer.function = (void *)wlan_pm_deepsleep_delay_timeout;

    oal_wake_lock_init(&pst_wlan_pm->st_deepsleep_wakelock, "wifi_deeepsleep_delay_wakelock");

    pst_wlan_pm->ul_wlan_power_state = POWER_STATE_SHUTDOWN;
    pst_wlan_pm->ul_wlan_dev_state = HOST_ALLOW_TO_SLEEP;
    pst_wlan_pm->ul_sleep_stage = SLEEP_STAGE_INIT;

    pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_get_pm_pause_func = OAL_PTR_NULL;
    pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_open_notify = OAL_PTR_NULL;
    pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_pm_state_notify = OAL_PTR_NULL;

#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    pst_wlan_pm->ul_wkup_src_print_en = OAL_FALSE;
#endif
    g_gpst_wlan_pm_info = pst_wlan_pm;

    oal_init_completion(&pst_wlan_pm->st_open_bcpu_done);
    oal_init_completion(&pst_wlan_pm->st_close_bcpu_done);
    oal_init_completion(&pst_wlan_pm->st_close_done);
    oal_init_completion(&pst_wlan_pm->st_wakeup_done);
    oal_init_completion(&pst_wlan_pm->st_wifi_powerup_done);
    oal_init_completion(&pst_wlan_pm->st_sleep_request_ack);
    oal_init_completion(&pst_wlan_pm->st_halt_bcpu_done);

    return OAL_TRUE;
}

/*
 * �� �� ��  : wlan_pm_init
 * ��������  : WLAN PM��ʼ���ӿ�
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
struct wlan_pm_s *wlan_pm_init(oal_void)
{
    struct wlan_pm_s *pst_wlan_pm = NULL;
    board_info *pst_board = get_hi110x_board_info();

    if (pst_board == OAL_PTR_NULL) {
        return OAL_PTR_NULL;
    }

    pst_wlan_pm = kzalloc(sizeof(struct wlan_pm_s), GFP_KERNEL);
    if (pst_wlan_pm == NULL) {
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "no mem to allocate wlan_pm_data");
        return OAL_PTR_NULL;
    }

    memset_s(pst_wlan_pm, sizeof(struct wlan_pm_s), 0, sizeof(struct wlan_pm_s));
    memset_s((oal_void *)&g_plat_pm_switch_action, OAL_SIZEOF(g_plat_pm_switch_action),
             0, OAL_SIZEOF(g_plat_pm_switch_action));
    g_plat_pm_switch_action.name = "plat_pm_wlan";
    g_plat_pm_switch_action.switch_notify = wlan_switch_action_callback;
    hcc_switch_action_register(&g_plat_pm_switch_action, (oal_void *)pst_wlan_pm);

    pst_wlan_pm->pst_bus = hcc_get_current_110x_bus();
    if (pst_wlan_pm->pst_bus == NULL) {
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "hcc bus is NULL, failed to create wlan_pm_wq!");
        kfree(pst_wlan_pm);
        return OAL_PTR_NULL;
    }
    pst_wlan_pm->pst_bus->pst_pm_callback = &g_gst_wlan_pm_callback;

    if (wlan_pm_init_ext1(pst_wlan_pm) == OAL_PTR_NULL) {
        return OAL_PTR_NULL;
    }

    wlan_pm_init_ext2(pst_wlan_pm);

    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wlan_pm_init ok!");
    return pst_wlan_pm;
}

/*
 * �� �� ��  : wlan_pm_exit
 * ��������  : WLAN pm�˳��ӿ�
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_uint wlan_pm_exit(oal_void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return OAL_SUCC;
    }

    wlan_pm_stop_wdg(pst_wlan_pm);

    wlan_pm_stop_deepsleep_delay_timer(pst_wlan_pm);

    hcc_bus_message_unregister(pst_wlan_pm->pst_bus, D2H_MSG_WAKEUP_SUCC);
    hcc_bus_message_unregister(pst_wlan_pm->pst_bus, D2H_MSG_WLAN_READY);
    hcc_bus_message_unregister(pst_wlan_pm->pst_bus, D2H_MSG_ALLOW_SLEEP);
    hcc_bus_message_unregister(pst_wlan_pm->pst_bus, D2H_MSG_DISALLOW_SLEEP);
    hcc_bus_message_unregister(pst_wlan_pm->pst_bus, D2H_MSG_POWEROFF_ACK);
    hcc_bus_message_unregister(pst_wlan_pm->pst_bus, D2H_MSG_OPEN_BCPU_ACK);
    hcc_bus_message_unregister(pst_wlan_pm->pst_bus, D2H_MSG_CLOSE_BCPU_ACK);
    hcc_bus_message_unregister(pst_wlan_pm->pst_bus, D2H_MSG_HALT_BCPU);

    hcc_switch_action_unregister(&g_plat_pm_switch_action);

    kfree(pst_wlan_pm);

    g_gpst_wlan_pm_info = OAL_PTR_NULL;
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wlan_pm_exit ok!");

    return OAL_SUCC;
}

/*
 * �� �� ��  : wlan_pm_is_poweron
 * ��������  : wifi�Ƿ��ϵ�
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_uint32 wlan_pm_is_poweron(oal_void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return OAL_FALSE;
    }

    if (pst_wlan_pm->ul_wlan_power_state == POWER_STATE_OPEN) {
        return OAL_TRUE;
    } else {
        return OAL_FALSE;
    }
}
EXPORT_SYMBOL_GPL(wlan_pm_is_poweron);

/*
 * �� �� ��  : wlan_pm_get_wifi_srv_handler
 * ��������  : ��ȡ�ص�handlerָ��
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
struct wifi_srv_callback_handler *wlan_pm_get_wifi_srv_handler(oal_void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return NULL;
    }

    return &pst_wlan_pm->st_wifi_srv_handler;
}
EXPORT_SYMBOL_GPL(wlan_pm_get_wifi_srv_handler);


oal_void wlan_clear_cali_excp_dfr(oal_void)
{
#ifdef _PRE_WLAN_FEATURE_DFR
    if (g_st_dfr_info.bit_device_reset_process_flag == OAL_FALSE) {
        g_cali_excp_dfr_count = 0;
    } else {
        oam_warning_log0(0, 0, "wlan_clear_cali_excp_dfr::cannot set g_cali_excp_dfr_count to 0 in dfr!!!");
    }
#endif
}

oal_void wlan_dfr_custom_cali(oal_void)
{
    hcc_bus *pst_bus = NULL;
    pst_bus = hcc_get_current_110x_bus();
    if (pst_bus == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, 0, "wlan_dfr_custom_cali::pst_bus is NULL!");
        return;
    }
    oal_init_completion(&pst_bus->st_device_ready);
    if (g_custom_process_func.p_custom_cali_func != OAL_PTR_NULL) {
        if (g_custom_process_func.p_custom_cali_func() == OAL_SUCC) {
            if (oal_wait_for_completion_timeout(&pst_bus->st_device_ready,
                (oal_uint32)oal_msecs_to_jiffies(HOST_WAIT_BOTTOM_INIT_TIMEOUT)) == 0) {
                OAM_ERROR_LOG0(0, 0, "wlan_dfr_custom_cali::timeout!");
            }
        }
    }
}
EXPORT_SYMBOL_GPL(wlan_dfr_custom_cali);

STATIC int32_t wlan_pm_open_check(struct wlan_pm_s *wlan_pm)
{
    if (wlan_pm == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_open:pst_wlan_pm is NULL!");
        return OAL_FAIL;
    }

    if (wlan_pm->pst_bus == NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_open::get 110x bus failed!");
        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_PLAT, CHR_LAYER_DRV,
                             CHR_PLT_DRV_EVENT_OPEN, CHR_PLAT_DRV_ERROR_POWER_ON_NON_BUS);
        return OAL_FAIL;
    }

    if (wlan_pm->ul_wlan_power_state == POWER_STATE_OPEN) {
        oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_open::aleady opened");
        return OAL_ERR_CODE_ALREADY_OPEN;
    }

    if (!wlan_pm->pst_bus->st_bus_wakelock.lock_count) {
        /* make sure open only lock once */
        hcc_bus_wake_lock(wlan_pm->pst_bus);
        wlan_pm_idle_sleep_vote(DISALLOW_IDLESLEEP);
        oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_idle_sleep_vote DISALLOW::lpcpu_idle_sleep_vote ID_WIFI 1!");
    }

    OAM_WARNING_LOG1(0, OAM_SF_PWR, "wlan_pm_open::get wakelock %lu!",
                     wlan_pm->pst_bus->st_bus_wakelock.lock_count);

    wlan_pm->ul_open_cnt++;
    return OAL_SUCC;
}


STATIC int32_t wlan_pm_wait_cali_complete(struct wlan_pm_s *wlan_pm)
{
    if (g_custom_process_func.p_custom_cali_func == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_open::NO g_custom_process_func registered");
        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV,
                             CHR_WIFI_DRV_EVENT_PLAT, CHR_WIFI_DRV_ERROR_POWER_ON_NO_CUSTOM_CALL);
        return OAL_FAIL;
    }

    oal_init_completion(&wlan_pm->pst_bus->st_device_ready);

   /* ���У׼�·��ɹ���ȴ�device ready�����������wifi */
    if (g_custom_process_func.p_custom_cali_func() != OAL_SUCC) {
        return OAL_SUCC;
    }

#ifdef _PRE_WLAN_RF_AUTOCALI
    /* ������ִ���Զ���У׼ */
    if ((g_autocali_switch == OAL_FALSE) || (wlan_pm_get_custom_cali_done() == OAL_FALSE))
#endif
    {
        if (oal_wait_for_completion_timeout(&wlan_pm->pst_bus->st_device_ready,
                                            (oal_uint32)oal_msecs_to_jiffies(HOST_WAIT_BOTTOM_INIT_TIMEOUT)) == 0) {
            declare_dft_trace_key_info("wlan_wait_custom_cali_fail", OAL_DFT_TRACE_FAIL);

            OAM_ERROR_LOG1(0, OAM_SF_PWR, "wlan_pm_open::wlan_pm_wait_custom_cali timeout %d !!!!!!",
                           HOST_WAIT_BOTTOM_INIT_TIMEOUT);

            chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV,
                                 CHR_WIFI_DRV_EVENT_PLAT, CHR_WIFI_DRV_ERROR_POWER_ON_CALL_TIMEOUT);

            if (oal_trigger_bus_exception(wlan_pm->pst_bus, OAL_TRUE) == OAL_TRUE) {
                oal_print_hi11xx_log(HI11XX_LOG_WARN, "dump device mem when cali custom failed!");
            }
            return OAL_FAIL;
        }
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : wlan_pm_custom_cali
 * ��������  : wifi���ƻ������·����ϵ�У׼
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
STATIC int32_t wlan_pm_custom_cali(struct wlan_pm_s *wlan_pm)
{
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1102A_HOST)
    if (g_custom_process_func.p_priv_ini_config_func == OAL_PTR_NULL) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_open::NO p_priv_ini_config_func registered");
        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV,
                             CHR_WIFI_DRV_EVENT_PLAT, CHR_WIFI_DRV_ERROR_POWER_ON_NO_CUSTOM_CALL);
        return OAL_FAIL;
    }

    /* ˽�ж��ƻ��·� */
    if (g_custom_process_func.p_priv_ini_config_func() != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_CUSTOM, "wlan_pm_open::p_priv_ini_config_func fail");
    }
#endif

    if (wlan_pm_wait_cali_complete(wlan_pm) != OAL_SUCC) {
        return OAL_FAIL;
    }
#endif

    return OAL_SUCC;
}

STATIC void wlan_pm_open_notify(struct wlan_pm_s *wlan_pm)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    raw_notifier_call_chain(&wifi_pm_chain, WIFI_PM_POWERUP_EVENT, (oal_void *)wlan_pm); /* powerup chain */
#endif

    wlan_pm_enable();

    /* WIFI�����ɹ���,֪ͨҵ��� */
    if (wlan_pm->st_wifi_srv_handler.p_wifi_srv_open_notify != OAL_PTR_NULL) {
        wlan_pm->st_wifi_srv_handler.p_wifi_srv_open_notify(OAL_TRUE);
    }

    /* ��timeoutֵ�ָ�ΪĬ��ֵ����������ʱ�� */
    wlan_pm_set_timeout(WLAN_SLEEP_DEFAULT_CHECK_CNT);

    wlan_clear_cali_excp_dfr();
}

/*
 * �� �� ��  : wlan_pm_open
 * ��������  : open wifi,���bfgxû�п���,�ϵ�,���������WCPU
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
int32_t wlan_pm_open(oal_void)
{
    int32_t ret;
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();
    struct pm_drv_data *pm_data = pm_get_drvdata();

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "wlan_pm_open enter");

    mutex_lock(&pm_data->host_mutex);

    ret = wlan_pm_open_check(pst_wlan_pm);
    if (ret != OAL_SUCC) {
        mutex_unlock(&pm_data->host_mutex);
        return ret;
    }

    ret = wlan_power_on();
    if (ret != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_open::wlan_power_on fail!");
        pst_wlan_pm->ul_wlan_power_state = POWER_STATE_SHUTDOWN;
        hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
        wlan_pm_idle_sleep_vote(ALLOW_IDLESLEEP);
        mutex_unlock(&pm_data->host_mutex);
        declare_dft_trace_key_info("wlan_power_on_fail", OAL_DFT_TRACE_FAIL);
        return OAL_FAIL;
    }

    if (wlan_pm_custom_cali(pst_wlan_pm) != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_open::cali fail!");
        wifi_power_fail_process(WIFI_POWER_ON_CALI_FAIL);
        pst_wlan_pm->ul_wlan_power_state = POWER_STATE_SHUTDOWN;
        hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
        wlan_pm_idle_sleep_vote(ALLOW_IDLESLEEP);
        mutex_unlock(&pm_data->host_mutex);
        return OAL_FAIL;
    }

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_open::wlan_pm_open SUCC!!");
    declare_dft_trace_key_info("wlan_open_succ", OAL_DFT_TRACE_SUCC);

    wlan_pm_open_notify(pst_wlan_pm);

    mutex_unlock(&pm_data->host_mutex);

    return OAL_SUCC;
}

EXPORT_SYMBOL_GPL(wlan_pm_open);

int32_t hitalk_pm_open(oal_void)
{
    int32_t ret;
    struct wlan_pm_s *pst_hitalk_pm = wlan_pm_get_drv();
    struct pm_drv_data *pm_data = pm_get_drvdata();

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "hitalk_pm_open enter");

    mutex_lock(&pm_data->host_mutex);

    ret = wlan_pm_open_check(pst_hitalk_pm);
    if (ret != OAL_SUCC) {
        mutex_unlock(&pm_data->host_mutex);
        return ret;
    }

    ret = hitalk_power_on();
    if (ret != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "hitalk_pm_open::hitalk_power_on fail!");
        pst_hitalk_pm->ul_wlan_power_state = POWER_STATE_SHUTDOWN;
        hcc_bus_wake_unlock(pst_hitalk_pm->pst_bus);
        wlan_pm_idle_sleep_vote(ALLOW_IDLESLEEP);
        mutex_unlock(&pm_data->host_mutex);
        declare_dft_trace_key_info("hitalk_power_on_fail", OAL_DFT_TRACE_FAIL);
        return OAL_FAIL;
    }

    if (wlan_pm_custom_cali(pst_hitalk_pm) != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_open::cali fail!");
        wifi_power_fail_process(WIFI_POWER_ON_CALI_FAIL);
        pst_hitalk_pm->ul_wlan_power_state = POWER_STATE_SHUTDOWN;
        hcc_bus_wake_unlock(pst_hitalk_pm->pst_bus);
        wlan_pm_idle_sleep_vote(ALLOW_IDLESLEEP);
        mutex_unlock(&pm_data->host_mutex);
        return OAL_FAIL;
    }

    oam_warning_log0(0, OAM_SF_PWR, "hitalk_pm_open::wlan_pm_open SUCC!!");
    declare_dft_trace_key_info("hitalk_open_succ", OAL_DFT_TRACE_SUCC);

    wlan_pm_open_notify(pst_hitalk_pm);

    mutex_unlock(&pm_data->host_mutex);

    return OAL_SUCC;
}
EXPORT_SYMBOL_GPL(hitalk_pm_open);

#if (defined(_PRE_PRODUCT_ID_HI110X_DEV) || defined(_PRE_PRODUCT_ID_HI110X_HOST))
oal_int32 g_ram_reg_test_result = OAL_SUCC;
unsigned long long g_ram_reg_test_time = 0;
oal_int32 g_wlan_mem_check_mdelay = 1000;
oal_int32 g_bfgx_mem_check_mdelay = 1500;

wlan_memdump_t g_st_wlan_memdump_cfg = { 0x60000000, 0x1000 };

oal_uint32 set_wlan_mem_check_mdelay(int32 mdelay)
{
    g_wlan_mem_check_mdelay = mdelay;
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "set_wlan_mem_check_mdelay::set delay:%dms!!", g_wlan_mem_check_mdelay);
    return 0;
}
oal_uint32 set_bfgx_mem_check_mdelay(int32 mdelay)
{
    g_bfgx_mem_check_mdelay = mdelay;
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "bfgx_mem_check_mdelay::set delay:%dms!!", g_bfgx_mem_check_mdelay);
    return 0;
}

EXPORT_SYMBOL_GPL(set_wlan_mem_check_mdelay);

wlan_memdump_t *get_wlan_memdump_cfg(void)
{
    return &g_st_wlan_memdump_cfg;
}

oal_uint32 set_wlan_mem_check_memdump(int32 addr, int32 len)
{
    g_st_wlan_memdump_cfg.addr = addr;
    g_st_wlan_memdump_cfg.len = len;
    g_st_wlan_memdump_cfg.en = 1;
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "set_wlan_mem_check_memdump set ok: addr:0x%x,len:%d", addr, len);
    return 0;
}

EXPORT_SYMBOL_GPL(set_wlan_mem_check_memdump);

oal_int32 wlan_device_mem_check(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (oal_warn_on(!pst_wlan_pm)) {
        OAM_ERROR_LOG1(0, OAM_SF_PWR, "%s fail!:pst_wlan_pm is null", __FUNCTION__);
        return -OAL_FAIL;
    }

    g_ram_reg_test_result = OAL_SUCC;
    g_ram_reg_test_time = 0;
    hcc_bus_wake_lock(pst_wlan_pm->pst_bus);

    if (wlan_pm_work_submit(pst_wlan_pm, &pst_wlan_pm->st_ram_reg_test_work) != 0) {
        hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
        oam_warning_log0(0, OAM_SF_PWR, "st_ram_reg_test_work submit work fail !");
    }

    return OAL_SUCC;
}
oal_int32 wlan_device_mem_check_result(unsigned long long *time)
{
    *time = g_ram_reg_test_time;
    return g_ram_reg_test_result;
}
oal_void wlan_device_mem_check_work(oal_work_stru *pst_worker)
{
    struct pm_drv_data *pm_data = pm_get_drvdata();
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    mutex_lock(&pm_data->host_mutex);

    hcc_bus_disable_state(pm_data->pst_wlan_pm_info->pst_bus, OAL_BUS_STATE_ALL);
    g_ram_reg_test_result = device_mem_check(&g_ram_reg_test_time);
    hcc_bus_enable_state(pm_data->pst_wlan_pm_info->pst_bus, OAL_BUS_STATE_ALL);

    hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);

    mutex_unlock(&pm_data->host_mutex);
}
EXPORT_SYMBOL_GPL(wlan_device_mem_check);
EXPORT_SYMBOL_GPL(wlan_device_mem_check_result);
#endif

STATIC uint32_t wlan_pm_close_pre(struct wlan_pm_s *pst_wlan_pm)
{
    if (pst_wlan_pm == OAL_PTR_NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "pst_wlan_pm is null");
        return OAL_FAIL;
    }

    if (!pst_wlan_pm->ul_apmode_allow_pm_flag) {
        oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_close,AP mode,do not shutdown power.");
        return OAL_ERR_CODE_FOBID_CLOSE_DEVICE;
    }

    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wlan_pm_close start!!");

    pst_wlan_pm->ul_close_cnt++;

    if (pst_wlan_pm->ul_wlan_power_state == POWER_STATE_SHUTDOWN) {
        return OAL_ERR_CODE_ALREADY_CLOSE;
    }

    /* WIFI�ر�ǰ,֪ͨҵ��� */
    if (pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_open_notify != OAL_PTR_NULL) {
        pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_open_notify(OAL_FALSE);
    }

    wlan_pm_disable();

    wlan_pm_stop_wdg(pst_wlan_pm);

    wlan_pm_stop_deepsleep_delay_timer(pst_wlan_pm);

    wlan_pm_info_clean();

    /* mask rx ip data interrupt */
    hcc_bus_rx_int_mask(hcc_get_current_110x_bus());

    return OAL_SUCC;
}

STATIC void wlan_pm_close_after(struct wlan_pm_s *pst_wlan_pm)
{
    pst_wlan_pm->ul_wlan_power_state = POWER_STATE_SHUTDOWN;

    /* unmask rx ip data interrupt */
    hcc_bus_rx_int_unmask(hcc_get_current_110x_bus());

    hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    raw_notifier_call_chain(&wifi_pm_chain, WIFI_PM_POWERDOWN_EVENT, (oal_void *)pst_wlan_pm); /* powerdown chain */
#endif

    wlan_pm_idle_sleep_vote(ALLOW_IDLESLEEP);

    OAM_WARNING_LOG1(0, OAM_SF_PWR, "wlan_pm_close release wakelock %lu!",
                     pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);

    hcc_bus_wakelocks_release_detect(pst_wlan_pm->pst_bus);
}

/*
 * �� �� ��  : wlan_pm_close
 * ��������  : close wifi,���bfgxû�п�,�µ磬�����������WCPU
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
uint32_t wlan_pm_close(oal_void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    struct pm_drv_data *pm_data = pm_get_drvdata();

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "wlan_pm_close enter");
    mutex_lock(&pm_data->host_mutex);
    if (wlan_pm_close_pre(pst_wlan_pm) != OAL_SUCC) {
        mutex_unlock(&pm_data->host_mutex);
        return OAL_FAIL;
    }

    if (wlan_power_off() != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_power_off FAIL!");
        mutex_unlock(&pm_data->host_mutex);
        declare_dft_trace_key_info("wlan_power_off_fail", OAL_DFT_TRACE_FAIL);
        return OAL_FAIL;
    }

    wlan_pm_close_after(pst_wlan_pm);
    mutex_unlock(&pm_data->host_mutex);

    hcc_dev_flowctrl_on(hcc_get_110x_handler(), 0);

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_close succ!");
    declare_dft_trace_key_info("wlan_close_succ", OAL_DFT_TRACE_SUCC);
    return OAL_SUCC;
}
EXPORT_SYMBOL_GPL(wlan_pm_close);

uint32_t hitalk_pm_close(oal_void)
{
    struct wlan_pm_s *pst_hitalk_pm = wlan_pm_get_drv();

    struct pm_drv_data *pm_data = pm_get_drvdata();

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "hitalk_pm_close enter");

    mutex_lock(&pm_data->host_mutex);
    wlan_pm_close_pre(pst_hitalk_pm);
    if (hitalk_power_off() != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "hitalk_power_off FAIL!");
        mutex_unlock(&pm_data->host_mutex);
        declare_dft_trace_key_info("hitalk_power_off_fail", OAL_DFT_TRACE_FAIL);
        return OAL_FAIL;
    }

    wlan_pm_close_after(pst_hitalk_pm);

    mutex_unlock(&pm_data->host_mutex);
    hcc_dev_flowctrl_on(hcc_get_110x_handler(), 0);

    oam_warning_log0(0, OAM_SF_PWR, "hitalk_pm_close succ!");
    declare_dft_trace_key_info("hitalk_close_succ", OAL_DFT_TRACE_SUCC);

    return OAL_SUCC;
}
EXPORT_SYMBOL_GPL(hitalk_pm_close);

/*
 * �� �� ��  : wlan_pm_enable
 * ��������  : ʹ��wlanƽ̨�͹���
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_uint32 wlan_pm_enable(oal_void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (!wlan_pm_get_switch()) {
        return OAL_SUCC;
    }

    hcc_tx_transfer_lock(hcc_get_110x_handler());

    if (pst_wlan_pm->ul_wlan_pm_enable == OAL_TRUE) {
        oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_enable already enabled!");
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_SUCC;
    }

    pst_wlan_pm->ul_wlan_pm_enable = OAL_TRUE;

    wlan_pm_feed_wdg();

    hcc_tx_transfer_unlock(hcc_get_110x_handler());

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_enable SUCC!");

    return OAL_SUCC;
}
EXPORT_SYMBOL_GPL(wlan_pm_enable);

/*
 * �� �� ��  : wlan_pm_disable_check_wakeup
 * ��������  : ȥʹ��wlanƽ̨��黽��
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_uint32 wlan_pm_disable_check_wakeup(oal_int32 flag)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    hcc_tx_transfer_lock(hcc_get_110x_handler());

    if (pst_wlan_pm->ul_wlan_pm_enable == OAL_FALSE) {
        oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_disable already disabled!");
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_SUCC;
    }

    if (flag == OAL_TRUE) {
        if (wlan_pm_wakeup_dev() != OAL_SUCC) {
            oam_warning_log0(0, OAM_SF_PWR, "pm wake up dev fail!");
        }
    }

    pst_wlan_pm->ul_wlan_pm_enable = OAL_FALSE;

    hcc_tx_transfer_unlock(hcc_get_110x_handler());

    wlan_pm_stop_wdg(pst_wlan_pm);

    oal_cancel_work_sync(&pst_wlan_pm->st_wakeup_work);
    oal_cancel_work_sync(&pst_wlan_pm->st_sleep_work);

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_disable SUCC!");

    return OAL_SUCC;
}
EXPORT_SYMBOL_GPL(wlan_pm_disable_check_wakeup);

oal_uint32 wlan_pm_disable(oal_void)
{
    return wlan_pm_disable_check_wakeup(OAL_TRUE);
}
EXPORT_SYMBOL_GPL(wlan_pm_disable);

oal_uint32 wlan_pm_statesave(oal_void)
{
    if (wlan_pm_get_switch()) {
        return wlan_pm_disable();
    } else {
        return OAL_SUCC;
    }
}

EXPORT_SYMBOL_GPL(wlan_pm_statesave);

oal_uint32 wlan_pm_staterestore(oal_void)
{
    if (wlan_pm_get_switch()) {
        return wlan_pm_enable();
    } else {
        return OAL_SUCC;
    }
}
EXPORT_SYMBOL_GPL(wlan_pm_staterestore);

/*
 * �� �� ��  : wlan_pm_init_dev
 * ��������  : ��ʼ��device��״̬
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_uint wlan_pm_init_dev(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();
    oal_int ret;
    hcc_bus *pst_bus = NULL;

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return OAL_FAIL;
    }

    pst_bus = hcc_get_current_110x_bus();
    if (oal_warn_on(pst_bus == NULL)) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_init get non bus!");

        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_PLAT, CHR_LAYER_DRV,
                             CHR_PLT_DRV_EVENT_PM, CHR_PLAT_DRV_ERROR_PM_INIT_NO_BUS);
        return OAL_FAIL;
    }

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_init_dev!\n");

    pst_wlan_pm->ul_wlan_dev_state = HOST_DISALLOW_TO_SLEEP;

    /* wait for bus wakeup */
    ret = down_timeout(&pst_bus->sr_wake_sema, 6 * HZ);
    if (ret == -ETIME) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "host bus controller is not ready!");
        hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
        declare_dft_trace_key_info("wifi_controller_wait_init_fail", OAL_DFT_TRACE_FAIL);
        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_PLAT, CHR_LAYER_DRV,
                             CHR_PLT_DRV_EVENT_PM, CHR_PLAT_DRV_ERROR_PM_SDIO_NO_READY);
        return OAL_FAIL;
    }
    up(&pst_bus->sr_wake_sema);

    return (oal_uint)hcc_bus_wakeup_request(pst_wlan_pm->pst_bus);
}

static void ssi_dump_for_wlan_pm_wakeup_dev_func_only(oal_void)
{
    if (hi11xx_get_os_build_variant() == HI1XX_OS_BUILD_VARIANT_USER) {
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "dump when dfr");
    } else {
        if (oal_print_rate_limit(30 * PRINT_RATE_SECOND)) {  /* 30s��ӡһ�� */
            ssi_dump_device_regs(SSI_MODULE_MASK_COMM | SSI_MODULE_MASK_BCTRL | SSI_MODULE_MASK_WCTRL);
        }
    }
}

/*
 * �� �� ��  : wlan_pm_wakeup_dev
 * ��������  : ����device
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_uint wlan_pm_wakeup_dev(oal_void)
{
    oal_uint32 ul_ret;
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();
    oal_int32 ret;
    oal_uint8 uc_retry;
    oal_uint8 uc_wakeup_retry = 0;
    static oal_uint8 uc_wakeup_err_count = 0;
    hcc_bus *pst_bus = NULL;
    ktime_t time_start, time_stop;
    oal_uint64 trans_us;

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return OAL_FAIL;
    }

    if (pst_wlan_pm->ul_wlan_pm_enable == OAL_FALSE) {
        if (pst_wlan_pm->ul_wlan_dev_state == HOST_ALLOW_TO_SLEEP) {
            /* ��������û���겻���������� */
            return OAL_EFAIL;
        } else {
            return OAL_SUCC;
        }
    }

    if (pst_wlan_pm->ul_wlan_dev_state == HOST_DISALLOW_TO_SLEEP) {
        return OAL_SUCC;
    }

    pst_bus = hcc_get_current_110x_bus();
    if (oal_warn_on(pst_bus == NULL)) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_wakeup_dev get non bus!\n");

        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_PLAT, CHR_LAYER_DRV,
                             CHR_PLT_DRV_EVENT_PM, CHR_PLAT_DRV_ERROR_PM_WKUP_NON_BUS);
        return OAL_FAIL;
    }
    wlan_pm_stop_deepsleep_delay_timer(pst_wlan_pm);

wakeup_again:
    time_start = ktime_get();
    hcc_bus_wake_lock(pst_wlan_pm->pst_bus);
    wlan_pm_idle_sleep_vote(DISALLOW_IDLESLEEP);

    oal_init_completion(&pst_wlan_pm->st_wakeup_done);
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wait bus wakeup");

    /* wait for bus wakeup */
    ret = down_timeout(&pst_bus->sr_wake_sema, 6 * HZ);
    if (ret == -ETIME) {
        pst_wlan_pm->ul_wakeup_fail_wait_sdio++;
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wifi controller is not ready!");
        declare_dft_trace_key_info("wifi_controller_wait_fail", OAL_DFT_TRACE_FAIL);
        hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
        return OAL_FAIL;
    }
    up(&pst_bus->sr_wake_sema);

    if (pst_wlan_pm->pst_bus->bus_type == HCC_BUS_PCIE) {
        /* ����������GPIO �����ѣ���ʱ��������ϢPCIE ����ȷ���Ƿ��Ѿ����ѣ�PCIEͨ�������� */
        oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_FALSE);
        oal_atomic_set(&g_wakeup_dev_wait_ack, 1);
        oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_TRUE);
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "wifi wakeup cmd send,wakelock cnt %lu",
                             pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);
        ret = hcc_bus_wakeup_request(pst_wlan_pm->pst_bus);
        if (ret != OAL_SUCC) {
            oal_print_hi11xx_log(HI11XX_LOG_INFO, "wakeup request failed ret=%d", ret);
            declare_dft_trace_key_info("wifi wakeup cmd send fail", OAL_DFT_TRACE_FAIL);
            ssi_dump_for_wlan_pm_wakeup_dev_func_only();
            hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
            goto wakeup_fail;
        }

        pst_wlan_pm->ul_wlan_dev_state = HOST_DISALLOW_TO_SLEEP;
    } else if (pst_wlan_pm->pst_bus->bus_type == HCC_BUS_SDIO) {
#ifdef _PRE_PLAT_FEATURE_HI110X_SDIO_GPIO_WAKE
        /*
         * use gpio to wakeup sdio device
         * 1.����������
         * 2.sdio wakeup �Ĵ���д0,д0��ȡ��sdio mem pg����
         */
        for (uc_retry = 0; uc_retry < WLAN_SDIO_MSG_RETRY_NUM; uc_retry++) {
            oal_init_completion(&pst_wlan_pm->st_wakeup_done);
            oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_FALSE);
            oal_atomic_set(&g_wakeup_dev_wait_ack, 1);
            oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_TRUE);
            board_host_wakeup_dev_set(0); /* wakeup dev */
            oal_udelay(100);
            board_host_wakeup_dev_set(1); /* wakeup dev */
            ul_ret = oal_wait_for_completion_timeout(&pst_wlan_pm->st_wakeup_done,
                                                     (oal_uint32)oal_msecs_to_jiffies(WLAN_WAKUP_MSG_WAIT_TIMEOUT));
            if (ul_ret != 0) {
                /* sdio gpio wakeup dev sucess */
                declare_dft_trace_key_info("gpio_wakeup_sdio_succ", OAL_DFT_TRACE_SUCC);
                break;
            }
        }

        if (uc_retry == WLAN_SDIO_MSG_RETRY_NUM) {
            OAM_ERROR_LOG1(0, OAM_SF_PWR, "oal_sdio_gpio_wakeup_dev retry %d failed", uc_retry);
            declare_dft_trace_key_info("oal_sdio_gpio_wakeup_dev final fail", OAL_DFT_TRACE_EXCEP);
            ssi_dump_for_wlan_pm_wakeup_dev_func_only();
            hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
            goto wakeup_fail;
        }

        hcc_bus_enable_state(pst_wlan_pm->pst_bus, OAL_BUS_STATE_ALL);

        oal_init_completion(&pst_wlan_pm->st_wakeup_done);
#endif
        /* set sdio register */
        oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_FALSE);

        oal_print_hi11xx_log(HI11XX_LOG_INFO, "wifi wakeup cmd send,wakelock cnt %lu",
                             pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);
        ret = hcc_bus_wakeup_request(pst_wlan_pm->pst_bus);
        if (ret != OAL_SUCC) {
            declare_dft_trace_key_info("wifi wakeup cmd send fail", OAL_DFT_TRACE_FAIL);
            for (uc_retry = 0; uc_retry < WLAN_SDIO_MSG_RETRY_NUM; uc_retry++) {
                msleep(10);
                ret = hcc_bus_wakeup_request(pst_wlan_pm->pst_bus);
                if (ret == OAL_SUCC) {
                    break;
                }

                ssi_dump_for_wlan_pm_wakeup_dev_func_only();
                oam_error_log2(0, OAM_SF_PWR, "oal_wifi_wakeup_dev retry %d ret = %d", uc_retry, ret);
                declare_dft_trace_key_info("wifi wakeup cmd send retry fail", OAL_DFT_TRACE_FAIL);
            }

            /* after max retry still fail,log error */
            if (ret != OAL_SUCC) {
                pst_wlan_pm->ul_wakeup_fail_set_reg++;
                OAM_ERROR_LOG1(0, OAM_SF_PWR, "oal_wifi_wakeup_dev Fail ret = %d", ret);
                declare_dft_trace_key_info("oal_wifi_wakeup_dev final fail", OAL_DFT_TRACE_EXCEP);
                oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_TRUE);
                hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
                goto wakeup_fail;
            }
        }

        oal_atomic_set(&g_wakeup_dev_wait_ack, 1);

        pst_wlan_pm->ul_wlan_dev_state = HOST_DISALLOW_TO_SLEEP;
        oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_TRUE);
        up(&pst_wlan_pm->pst_bus->rx_sema);
    } else {
        declare_dft_trace_key_info("oal_wifi_wakeup_dev final fail", OAL_DFT_TRACE_EXCEP);
        hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
        goto wakeup_fail;
    }

#ifdef PLATFORM_DEBUG_ENABLE
    if (!is_dfr_test_en(WIFI_WKUP_FAULT)) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "[dfr test] trigger wkup fail!");
        uc_wakeup_err_count = WLAN_WAKEUP_FAIL_MAX_TIMES;
        hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
        goto wakeup_fail;
    }
#endif

    ul_ret = oal_wait_for_completion_timeout(&pst_wlan_pm->st_wakeup_done,
                                             (oal_uint32)oal_msecs_to_jiffies(WLAN_WAKUP_MSG_WAIT_TIMEOUT));
    if (ul_ret == 0) {
        oal_int32 sleep_state = hcc_bus_get_sleep_state(pst_wlan_pm->pst_bus);
        if ((sleep_state == DISALLOW_TO_SLEEP_VALUE) || (sleep_state < 0)) {
            if (oal_unlikely(sleep_state < 0)) {
                OAM_ERROR_LOG1(0, OAM_SF_PWR, "get state failed, sleep_state=%d", sleep_state);
            }

            pst_wlan_pm->ul_wakeup_fail_timeout++;
            oam_warning_log0(0, OAM_SF_PWR, "oal_wifi_wakeup_dev SUCC to set 0xf0 = 0");
            hcc_bus_sleep_request(pst_wlan_pm->pst_bus);
            pst_wlan_pm->ul_wlan_dev_state = HOST_ALLOW_TO_SLEEP;
            hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
            if (uc_wakeup_retry == 0) {
                OAM_WARNING_LOG1(0, OAM_SF_PWR,
                                 "wlan_pm_wakeup_dev wait device complete fail,wait time %d ms!,try again",
                                 WLAN_WAKUP_MSG_WAIT_TIMEOUT);
                uc_wakeup_retry++;
                goto wakeup_again;
            } else {
                oam_error_log2(0, OAM_SF_PWR, "wlan_pm_wakeup_dev [%d]wait device complete fail,wait time %d ms!",
                               uc_wakeup_err_count, WLAN_WAKUP_MSG_WAIT_TIMEOUT);
                oal_print_hi11xx_log(HI11XX_LOG_INFO,
                                     KERN_ERR "wlan_pm_wakeup_dev [%d]wait device complete fail,wait time %d ms!",
                                     uc_wakeup_err_count, WLAN_WAKUP_MSG_WAIT_TIMEOUT);
                goto wakeup_fail;
            }
        } else {
            pst_wlan_pm->ul_wakeup_fail_set_reg++;
            OAM_ERROR_LOG0(0, OAM_SF_PWR, "wakeup_dev Fail to set 0xf0 = 0");
            oal_print_hi11xx_log(HI11XX_LOG_INFO, KERN_ERR "wakeup_dev Fail to set 0xf0 = 0");
            pst_wlan_pm->ul_wlan_dev_state = HOST_ALLOW_TO_SLEEP;
            hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
            goto wakeup_fail;
        }
    }

    pst_wlan_pm->ul_wakeup_succ++;
    declare_dft_trace_key_info("wlan_wakeup_succ", OAL_DFT_TRACE_SUCC);

    pst_wlan_pm->ul_wdg_timeout_curr_cnt = 0;
    pst_wlan_pm->ul_packet_cnt = 0;
    pst_wlan_pm->ul_packet_check_time = jiffies + msecs_to_jiffies(WLAN_PACKET_CHECK_TIME);
    pst_wlan_pm->ul_packet_total_cnt = 0;

    /* HOST WIFI�˳��͹���,֪ͨҵ��࿪����ʱ�� */
    if (pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_pm_state_notify != OAL_PTR_NULL) {
        pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_pm_state_notify(OAL_TRUE);
    }

    wlan_pm_feed_wdg();

    uc_wakeup_err_count = 0;

    oal_usleep_range(500, 510);

    hcc_bus_wakeup_complete(pst_wlan_pm->pst_bus);

    pst_wlan_pm->ul_sleep_fail_forbid_cnt = 0;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    raw_notifier_call_chain(&wifi_pm_chain, WIFI_PM_WAKEUP_EVENT, (oal_void *)pst_wlan_pm); /* wakeup chain */
#endif

    time_stop = ktime_get();
    trans_us = (oal_uint64)ktime_to_us(ktime_sub(time_stop, time_start));
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wakeup dev succ, cost %llu us", trans_us);
    return OAL_SUCC;
wakeup_fail:
    declare_dft_trace_key_info("wlan_wakeup_fail", OAL_DFT_TRACE_FAIL);
    uc_wakeup_err_count++;

    /* pm����ʧ�ܳ������ޣ�����dfr���� */
    if (uc_wakeup_err_count > WLAN_WAKEUP_FAIL_MAX_TIMES) {
        OAM_ERROR_LOG1(0, OAM_SF_PWR, "Now ready to enter DFR process after [%d]times wlan_wakeup_fail!",
                       uc_wakeup_err_count);
        uc_wakeup_err_count = 0;
        hcc_bus_exception_submit(pst_wlan_pm->pst_bus, WIFI_WAKEUP_FAIL);
    }
    chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_PLAT, CHR_LAYER_DRV,
                         CHR_PLT_DRV_EVENT_PM, CHR_PLAT_DRV_ERROR_WIFI_WKUP_DEV);

    return OAL_FAIL;
}

oal_void wlan_pm_wakeup_dev_ack(oal_void)
{
    struct wlan_pm_s *pst_wlan_pm = NULL;

    if (oal_atomic_read(&g_wakeup_dev_wait_ack)) {
        pst_wlan_pm = wlan_pm_get_drv();
        if (pst_wlan_pm == OAL_PTR_NULL) {
            return;
        }

        pst_wlan_pm->ul_wakeup_dev_ack++;

        oal_complete(&pst_wlan_pm->st_wakeup_done);

        oal_atomic_set(&g_wakeup_dev_wait_ack, 0);
    }

    return;
}

/*
 * �� �� ��  : wlan_pm_open_bcpu
 * ��������  : ����BCPU
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_uint wlan_pm_open_bcpu(oal_void)
{
#define RETRY_TIMES 3
    oal_uint32 i;
    oal_int32 ret = OAL_FAIL;
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();
    oal_int32 ul_ret;

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return OAL_FAIL;
    }

    /* ͨ��sdio��������⸴λBCPU */
    oam_warning_log0(0, OAM_SF_PWR, "open BCPU");

    hcc_tx_transfer_lock(hcc_get_110x_handler());

    for (i = 0; i < RETRY_TIMES; i++) {
        ret = wlan_pm_wakeup_dev();
        if (ret == OAL_SUCC) {
            break;
        }
    }

    if (ret != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_wakeup_dev fail!");
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_FAIL;
    }

    OAM_WARNING_LOG1(0, OAM_SF_PWR, "wlan_pm_wakeup_dev succ, retry times [%d]", i);

    oal_init_completion(&pst_wlan_pm->st_open_bcpu_done);

    ret = hcc_bus_send_message(pst_wlan_pm->pst_bus, H2D_MSG_RESET_BCPU);
    if (ret == OAL_SUCC) {
        /* �ȴ�deviceִ������ */
        up(&pst_wlan_pm->pst_bus->rx_sema);
        ul_ret = oal_wait_for_completion_timeout(&pst_wlan_pm->st_open_bcpu_done,
                                                 (oal_uint32)oal_msecs_to_jiffies(WLAN_OPEN_BCPU_WAIT_TIMEOUT));
        if (ul_ret == 0) {
            OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_open_bcpu wait device ACK timeout !");
            hcc_tx_transfer_unlock(hcc_get_110x_handler());
            return OAL_FAIL;
        }

        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_SUCC;
    } else {
        OAM_ERROR_LOG1(0, OAM_SF_PWR, "fail to send H2D_MSG_RESET_BCPU, ret=%d", ret);
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_FAIL;
    }
}

/*
 * �� �� ��  : wlan_pm_wakeup_host
 * ��������  : device����host
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_uint wlan_pm_wakeup_host(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (oal_warn_on(!pst_wlan_pm)) {
        OAM_ERROR_LOG1(0, OAM_SF_PWR, "%s fail!:pst_wlan_pm is null", __FUNCTION__);
        return -OAL_FAIL;
    }

    hcc_bus_wake_lock(pst_wlan_pm->pst_bus);
    wlan_pm_idle_sleep_vote(DISALLOW_IDLESLEEP);
    oam_info_log1(0, OAM_SF_PWR, "wlan_pm_wakeup_host get wakelock %lu!",
                  pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);

    if (wlan_pm_work_submit(pst_wlan_pm, &pst_wlan_pm->st_wakeup_work) != 0) {
        pst_wlan_pm->ul_wakeup_fail_submit_work++;
        hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
    } else {
        pst_wlan_pm->ul_wakeup_succ_work_submit++;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : wlan_pm_work_submit
 * ��������  : �ύһ��kernel work
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_int wlan_pm_work_submit(struct wlan_pm_s *pst_wlan_pm, oal_work_stru *pst_worker)
{
    oal_int i_ret = 0;

    if (oal_work_is_busy(pst_worker)) {
        /* If comm worker is processing,
          we need't submit again */
        i_ret = -OAL_EBUSY;
        goto done;
    } else {
        oam_info_log1(0, OAM_SF_PWR, "WiFi %lX Worker Submit\n", (uintptr_t)pst_worker->func);
        if (queue_work(pst_wlan_pm->pst_pm_wq, pst_worker) == false) {
            i_ret = -OAL_EFAIL;
        }
    }
done:
    return i_ret;
}

/*
 * �� �� ��  : wlan_pm_wakeup_work
 * ��������  : device����host work
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
void wlan_pm_wakeup_work(oal_work_stru *pst_worker)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();
    oal_uint ret;

    oam_info_log0(0, OAM_SF_PWR, "wlan_pm_wakeup_work start!");

    hcc_tx_transfer_lock(hcc_get_110x_handler());

    ret = wlan_pm_wakeup_dev();
    if (oal_unlikely(ret != OAL_SUCC)) {
        declare_dft_trace_key_info("wlan_wakeup_fail", OAL_DFT_TRACE_FAIL);
    }

    hcc_tx_transfer_unlock(hcc_get_110x_handler());

    /* match for the work submit */
    hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
    declare_dft_trace_key_info("wlan_d2h_wakeup_succ", OAL_DFT_TRACE_SUCC);
    oam_info_log1(0, OAM_SF_PWR, "wlan_pm_wakeup_work release wakelock %lu!",
                  pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);

    return;
}

/*
 * �� �� ��  : wlan_pm_wakeup_done_callback
 * ��������  : deviceӦ��wakeup succ��Ϣ����
 */
int32 wlan_pm_wakeup_done_callback(void *data)
{
    struct wlan_pm_s *pst_wlan_pm = (struct wlan_pm_s *)data;

    oam_info_log0(0, OAM_SF_PWR, "wlan_pm_wakeup_done_callback !");

    pst_wlan_pm->ul_wakeup_done_callback++;

    wlan_pm_wakeup_dev_ack();

    return SUCCESS;
}

/*
 * �� �� ��  : wlan_pm_close_done_callback
 * ��������  : deviceӦ��poweroff ack��Ϣ����
 */
int32 wlan_pm_close_done_callback(void *data)
{
    struct wlan_pm_s *pst_wlan_pm = (struct wlan_pm_s *)data;

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_close_done_callback !");

    /* �ر�RXͨ������ֹSDIO RX thread��������SDIO */
    hcc_bus_disable_state(hcc_get_current_110x_bus(), OAL_BUS_STATE_RX);

    pst_wlan_pm->ul_close_done_callback++;
    oal_complete(&pst_wlan_pm->st_close_done);

    oam_warning_log0(0, OAM_SF_PWR, "complete H2D_MSG_PM_WLAN_OFF done!");

    return SUCCESS;
}

/*
 * �� �� ��  : wlan_pm_open_bcpu_done_callback
 * ��������  : deviceӦ��open bcpu ack��Ϣ����
 */
int32 wlan_pm_open_bcpu_done_callback(void *data)
{
    struct wlan_pm_s *pst_wlan_pm = (struct wlan_pm_s *)data;

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_open_bcpu_done_callback !");

    pst_wlan_pm->ul_open_bcpu_done_callback++;
    oal_complete(&pst_wlan_pm->st_open_bcpu_done);

    oam_warning_log0(0, OAM_SF_PWR, "complete H2D_MSG_RESET_BCPU done!");

    return SUCCESS;
}

/*
 * �� �� ��  : wlan_pm_close_bcpu_done_callback
 * ��������  : deviceӦ��open bcpu ack��Ϣ����
 */
int32 wlan_pm_close_bcpu_done_callback(void *data)
{
    struct wlan_pm_s *pst_wlan_pm = (struct wlan_pm_s *)data;

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_close_bcpu_done_callback !");

    pst_wlan_pm->ul_close_bcpu_done_callback++;
    oal_complete(&pst_wlan_pm->st_close_bcpu_done);

    oam_warning_log0(0, OAM_SF_PWR, "complete H2D_MSG_PM_BCPU_OFF done!");

    return SUCCESS;
}

/*
 * �� �� ��  : wlan_pm_halt_bcpu_done_callback
 * ��������  : deviceӦ��open bcpu ack��Ϣ����
 */
int32 wlan_pm_halt_bcpu_done_callback(void *data)
{
    struct wlan_pm_s *pst_wlan_pm = (struct wlan_pm_s *)data;

    oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_halt_bcpu_done_callback !");

    oal_complete(&pst_wlan_pm->st_halt_bcpu_done);

    oam_warning_log0(0, OAM_SF_PWR, "complete wlan_pm_halt_bcpu_done_callback done!");

    return SUCCESS;
}

OAL_STATIC oal_void sleep_request_host_forbid_print(struct wlan_pm_s *pst_wlan_pm,
                                                    const oal_uint32 ul_host_forbid_sleep_limit)
{
    if (pst_wlan_pm->ul_sleep_request_host_forbid >= ul_host_forbid_sleep_limit) {
        /* ��ֹƵ����ӡ */
        if (oal_print_rate_limit(10 * PRINT_RATE_SECOND)) { /* 10s��ӡһ�� */
            oal_int32 allow_print;
            oam_warning_log2(0, OAM_SF_PWR, "wlan_pm_sleep_work_etc host forbid sleep %ld, forbid_cnt:%u",
                             pst_wlan_pm->ul_sleep_stage, pst_wlan_pm->ul_sleep_request_host_forbid);
            allow_print = oal_print_rate_limit(10 * PRINT_RATE_MINUTE); /* 10���Ӵ�ӡһ�� */
            hcc_bus_print_trans_info(pst_wlan_pm->pst_bus,
                                     allow_print ?
                                     (HCC_PRINT_TRANS_FLAG_DEVICE_STAT | HCC_PRINT_TRANS_FLAG_DEVICE_REGS) : 0x0);
        }
    } else {
        /* ��ֹƵ����ӡ */
        if (oal_print_rate_limit(10 * PRINT_RATE_SECOND)) { /* 10s��ӡһ�� */
            oam_warning_log2(0, OAM_SF_PWR, "wlan_pm_sleep_work_etc host forbid sleep %ld, forbid_cnt:%u",
                             pst_wlan_pm->ul_sleep_stage, pst_wlan_pm->ul_sleep_request_host_forbid);
        }
    }
}

/*
 * �� �� ��  : wlan_pm_sleep_work_check
 * ����: ����Ƿ����˯��
 */
OAL_STATIC uint32_t  wlan_pm_sleep_check(struct wlan_pm_s *pst_wlan_pm)
{
    oal_bool_enum_uint8 en_wifi_pause_pm = OAL_FALSE;
    if (pst_wlan_pm->ul_wlan_pm_enable == OAL_FALSE) {
        wlan_pm_feed_wdg();
        return OAL_FAIL;
    }

    /* Э��ջ�ص���ȡ�Ƿ�pause�͹��� */
    if (pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_get_pm_pause_func) {
        en_wifi_pause_pm = pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_get_pm_pause_func();
    }

    if (en_wifi_pause_pm == OAL_TRUE) {
        wlan_pm_feed_wdg();
        return OAL_FAIL;
    }

    if (pst_wlan_pm->ul_wlan_dev_state == HOST_ALLOW_TO_SLEEP) {
        oal_print_hi11xx_log(HI11XX_LOG_DBG, "wakeuped,need not do again");
        wlan_pm_feed_wdg();
        return OAL_FAIL;
    }
    return OAL_SUCC;
}

OAL_STATIC uint32_t wlan_pm_sleep_request_host(struct wlan_pm_s *pst_wlan_pm)
{
    const uint32_t host_forbid_sleep_limit = 10;
    int32_t ret = hcc_bus_sleep_request_host(pst_wlan_pm->pst_bus);
    if (ret != OAL_SUCC) {
        pst_wlan_pm->ul_sleep_request_host_forbid++;
        declare_dft_trace_key_info("wlan_forbid_sleep_host", OAL_DFT_TRACE_SUCC);
        sleep_request_host_forbid_print(pst_wlan_pm, host_forbid_sleep_limit);
        return OAL_ERR_CODE_SLEEP_FORBID;
    } else {
        pst_wlan_pm->ul_sleep_request_host_forbid = 0;
    }

    return OAL_SUCC;
}

/*
 * �� �� ��  : wlan_pm_sleep_cmd_send
 * ��������  : ���� sleep cmd msg
 */
OAL_STATIC int32_t wlan_pm_sleep_cmd_send(struct wlan_pm_s *pst_wlan_pm)
{
    int32_t ret;
    uint8_t retry;

    oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_FALSE);

    pst_wlan_pm->ul_wlan_dev_state = HOST_ALLOW_TO_SLEEP;

    if (oal_print_rate_limit(PRINT_RATE_SECOND)) { /* 1s��ӡһ�� */
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "wifi sleep cmd send,pkt_num:[%d], wakelock cnt %lu",
                             pst_wlan_pm->ul_packet_total_cnt, pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);
    }

    for (retry = 0; retry < WLAN_SDIO_MSG_RETRY_NUM; retry++) {
        ret = hcc_bus_sleep_request(pst_wlan_pm->pst_bus);
        if (ret == OAL_SUCC) {
            break;
        }
        oam_error_log2(0, OAM_SF_PWR, "sleep_dev retry %d ret = %d", retry, ret);
        oal_msleep(10);
    }

    /* after max retry still fail,log error */
    if (ret != OAL_SUCC) {
        pst_wlan_pm->ul_sleep_fail_set_reg++;
        declare_dft_trace_key_info("wlan_sleep_cmd_fail", OAL_DFT_TRACE_FAIL);
        OAM_ERROR_LOG1(0, OAM_SF_PWR, "sleep_dev Fail ret = %d\r\n", ret);
        pst_wlan_pm->ul_wlan_dev_state = HOST_DISALLOW_TO_SLEEP;
        oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_TRUE);
        return OAL_FAIL;
    }

    oal_wlan_gpio_intr_enable(hbus_to_dev(pst_wlan_pm->pst_bus), OAL_TRUE);

    pst_wlan_pm->ul_sleep_fail_forbid_cnt = 0;
    pst_wlan_pm->ul_sleep_fail_forbid = 0;

    return OAL_SUCC;
}

/*
 * �� �� ��  : wlan_pm_sleep_forbid_debug
 * ����      : ��ȡHCC��ı���ͳ������
*/
OAL_STATIC void wlan_pm_sleep_forbid_debug(struct wlan_pm_s *pst_wlan_pm)
{
    /* ���debug�Աȼ�����������Ϊstatic���� */
    static uint64_t g_old_tx, g_old_rx;
    static uint64_t g_new_tx, g_new_rx;

    pst_wlan_pm->ul_sleep_fail_forbid++;
    if (pst_wlan_pm->ul_sleep_fail_forbid == 1) {
        pst_wlan_pm->ul_sleep_forbid_check_time = jiffies + msecs_to_jiffies(WLAN_SLEEP_FORBID_CHECK_TIME);
    } else if ((pst_wlan_pm->ul_sleep_fail_forbid != 0) &&
               (time_after(jiffies, pst_wlan_pm->ul_sleep_forbid_check_time))) {
        /* ��ʱ����2���ӱ�forbid sleep���ϱ�һ��CHR�����������پ���������DFR */
        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_PLAT, CHR_LAYER_DRV,
                             CHR_PLT_DRV_EVENT_PM, CHR_PLAT_DRV_ERROR_SLEEP_FORBID);
        pst_wlan_pm->ul_sleep_fail_forbid = 0;
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "device_forbid_sleep for %ld second",
                             WLAN_SLEEP_FORBID_CHECK_TIME / MSEC_PER_SEC);
        if (oal_trigger_bus_exception(pst_wlan_pm->pst_bus, OAL_TRUE) == OAL_TRUE) {
            oal_print_hi11xx_log(HI11XX_LOG_WARN, "tigger dump device mem for device_forbid_sleep %ld second",
                                 WLAN_SLEEP_FORBID_CHECK_TIME / MSEC_PER_SEC);
        }
    }

    pst_wlan_pm->ul_sleep_fail_forbid_cnt++;
    if (pst_wlan_pm->ul_sleep_fail_forbid_cnt <= 1) {
        /* get hcc trans count */
        hcc_bus_get_trans_count(pst_wlan_pm->pst_bus, &g_old_tx, &g_old_rx);
    } else {
        /* ul_sleep_fail_forbid_cnt > 1 */
        hcc_bus_get_trans_count(pst_wlan_pm->pst_bus, &g_new_tx, &g_new_rx);
        /* trans pending */
        if (pst_wlan_pm->ul_sleep_fail_forbid_cnt >= g_wlan_sleep_request_forbid_limit) {
            /* maybe device memleak */
            declare_dft_trace_key_info("wlan_forbid_sleep_print_info", OAL_DFT_TRACE_SUCC);
            oam_warning_log2(0, OAM_SF_PWR,
                             "wlan_pm_sleep_work device forbid sleep %ld, forbid_cnt:%u try dump device mem info",
                             pst_wlan_pm->ul_sleep_stage, pst_wlan_pm->ul_sleep_fail_forbid_cnt);
            oam_warning_log4(0, OAM_SF_PWR, "old[tx:%u rx:%u]new[tx:%u rx:%u]", g_old_tx, g_old_rx, g_new_tx, g_new_rx);
            pst_wlan_pm->ul_sleep_fail_forbid_cnt = 0;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
            hcc_print_current_trans_info();
#endif
            hcc_bus_send_message(pst_wlan_pm->pst_bus, H2D_MSG_DEVICE_MEM_DUMP);
            // ��ӡ10��
        } else if ((pst_wlan_pm->ul_sleep_fail_forbid_cnt % (g_wlan_sleep_request_forbid_limit / 10)) == 0) {
            oal_print_hi11xx_log(HI11XX_LOG_INFO,
                "sleep request too many forbid %ld, device busy, forbid_cnt:%u, old[tx:%u rx:%u] new[tx:%u rx:%u]",
                pst_wlan_pm->ul_sleep_stage, pst_wlan_pm->ul_sleep_fail_forbid_cnt,
                (uint32_t)g_old_tx, (uint32_t)g_old_rx, (uint32_t)g_new_tx, (uint32_t)g_new_rx);
        } else {
            oal_print_hi11xx_log(HI11XX_LOG_DBG,
                "sleep request forbid %ld, device busy, forbid_cnt:%u, old[tx:%u rx:%u] new[tx:%u rx:%u]",
                pst_wlan_pm->ul_sleep_stage, pst_wlan_pm->ul_sleep_fail_forbid_cnt,
                (uint32_t)g_old_tx, (uint32_t)g_old_rx, (uint32_t)g_new_tx, (uint32_t)g_new_rx);
        }
    }

    return;
}

/*
 * �� �� ��  : wlan_pm_sleep_cmd_proc
 * ����: ����Ƿ����˯��
 */
OAL_STATIC uint32_t  wlan_pm_sleep_cmd_proc(struct wlan_pm_s *pst_wlan_pm)
{
    uint32_t ret;

    if (pst_wlan_pm->ul_sleep_stage == SLEEP_ALLOW_RCV) {
        ret = wlan_pm_sleep_cmd_send(pst_wlan_pm);
        if (ret == OAL_FAIL) {
            return OAL_ERR_CODE_SLEEP_FAIL;
        }
    } else {
        wlan_pm_sleep_forbid_debug(pst_wlan_pm);
        declare_dft_trace_key_info("wlan_forbid_sleep", OAL_DFT_TRACE_SUCC);
        return OAL_ERR_CODE_SLEEP_FORBID;
    }

    pst_wlan_pm->ul_sleep_stage = SLEEP_CMD_SND;
    pst_wlan_pm->ul_sleep_succ++;
    pst_wlan_pm->fail_sleep_count = 0;

    wlan_pm_idle_sleep_vote(ALLOW_IDLESLEEP);
    /* ��������500ms, ��ֹϵͳƵ�������˳�PM */
    oal_wake_lock(&pst_wlan_pm->st_deepsleep_wakelock);

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "wlan_pm_sleep_work hold deepsleep_wakelock....%lu",
                         pst_wlan_pm->st_deepsleep_wakelock.lock_count);
    mod_timer(&pst_wlan_pm->st_deepsleep_delay_timer, jiffies + msecs_to_jiffies(WLAN_WAKELOCK_HOLD_TIME));

    declare_dft_trace_key_info("wlan_sleep_ok", OAL_DFT_TRACE_SUCC);
    return OAL_SUCC;
}

/*
 * �� �� ��  : wlan_pm_sleep_notify
 * ����: ˯�߳ɹ�֪ͨ�ܱ�ģ��
 */
OAL_STATIC void  wlan_pm_sleep_notify(struct wlan_pm_s *pst_wlan_pm)
{
    /* HOST WIFI����͹���,֪ͨҵ���رն�ʱ�� */
    if (pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_pm_state_notify != OAL_PTR_NULL) {
        pst_wlan_pm->st_wifi_srv_handler.p_wifi_srv_pm_state_notify(OAL_FALSE);
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    raw_notifier_call_chain(&wifi_pm_chain, WIFI_PM_SLEEP_EVENT, (oal_void *)pst_wlan_pm); /* sleep chain */
#endif

    return;
}

/*
 * �� �� ��  : wlan_pm_sleep_forbid_proc
 * ����: ˯��ʧ�ܴ���
 */
OAL_STATIC void  wlan_pm_sleep_forbid_proc(struct wlan_pm_s *pst_wlan_pm)
{
    pst_wlan_pm->fail_sleep_count = 0;
    wlan_pm_feed_wdg();
    return;
}

/*
 * �� �� ��  : wlan_pm_sleep_fail_proc
 * ����: ˯��ʧ�ܴ���
 */
OAL_STATIC void  wlan_pm_sleep_fail_proc(struct wlan_pm_s *pst_wlan_pm)
{
    pst_wlan_pm->fail_sleep_count++;
    wlan_pm_feed_wdg();

    /* ʧ�ܳ������ޣ�����dfr���� */
    if (pst_wlan_pm->fail_sleep_count > WLAN_WAKEUP_FAIL_MAX_TIMES) {
        OAM_ERROR_LOG1(0, OAM_SF_PWR, "Now ready to enter DFR process after [%d]times wlan_sleep_fail!",
                       pst_wlan_pm->fail_sleep_count);
        pst_wlan_pm->fail_sleep_count = 0;
        wlan_pm_stop_wdg(pst_wlan_pm);
        hcc_bus_exception_submit(pst_wlan_pm->pst_bus, WIFI_WAKEUP_FAIL);
    }
    chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV,
                         CHR_PLT_DRV_EVENT_PM, CHR_PLAT_DRV_ERROR_WIFI_SLEEP_REQ);
}

/*
 * �� �� ��  : wlan_pm_sleep_work
 * ��������  : pm˯��
 * �� �� ֵ  : �ɹ���ʧ��ԭ��
 */
void wlan_pm_sleep_work(oal_work_stru *pst_worker)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();
    int32_t ret;

    hcc_tx_transfer_lock(hcc_get_110x_handler());

    if (wlan_pm_sleep_check(pst_wlan_pm) != OAL_SUCC) {
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return;
    }

    // check host
    ret = wlan_pm_sleep_request_host(pst_wlan_pm);
    if (ret != OAL_SUCC) {
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        wlan_pm_sleep_forbid_proc(pst_wlan_pm);
        return;
    }

    ret = wlan_pm_sleep_request(pst_wlan_pm);
    if (ret == OAL_ERR_CODE_SLEEP_FAIL) {
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        wlan_pm_sleep_fail_proc(pst_wlan_pm);
        return;
    }

    ret = wlan_pm_sleep_cmd_proc(pst_wlan_pm);
    if (ret == OAL_ERR_CODE_SLEEP_FAIL) {
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        wlan_pm_sleep_fail_proc(pst_wlan_pm);
        return;
    } else if (ret == OAL_ERR_CODE_SLEEP_FORBID) {
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        wlan_pm_sleep_forbid_proc(pst_wlan_pm);
        return;
    }

    hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
    hcc_tx_transfer_unlock(hcc_get_110x_handler());

    if (pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count != 0) {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "wlan_pm_sleep_work release wakelock %lu!\n",
                         pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);
    }

    wlan_pm_sleep_notify(pst_wlan_pm);

    return;
}

/*
 * �� �� ��  : wlan_pm_state_get
 * ��������  : ��ȡpm��sleep״̬
 * �� �� ֵ  : 1:allow to sleep; 0:disallow to sleep
 */
oal_uint wlan_pm_state_get(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    return pst_wlan_pm->ul_wlan_dev_state;
}

/*
 * �� �� ��  : wlan_pm_state_set
 * ��������  : ��ȡpm��sleep״̬
 * �������  :
 * �������  :
 * �� �� ֵ  : 1:allow to sleep; 0:disallow to sleep
 */
oal_void wlan_pm_state_set(struct wlan_pm_s *pst_wlan_pm, oal_uint ul_state)
{
    pst_wlan_pm->ul_wlan_dev_state = ul_state;
}
/*
 * �� �� ��  : wlan_pm_set_timeout
 * ��������  : ˯�߶�ʱ����ʱʱ������
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_void wlan_pm_set_timeout(oal_uint32 ul_timeout)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return;
    }

    OAM_WARNING_LOG1(0, OAM_SF_PWR, "wlan_pm_set_timeout[%d]", ul_timeout);

    pst_wlan_pm->ul_wdg_timeout_cnt = ul_timeout;

    pst_wlan_pm->ul_wdg_timeout_curr_cnt = 0;

    pst_wlan_pm->ul_packet_cnt = 0;

    wlan_pm_feed_wdg();
}
EXPORT_SYMBOL_GPL(wlan_pm_set_timeout);
/*
 * �� �� ��  : wlan_pm_feed_wdg
 * ��������  : ����50ms˯�߶�ʱ��
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
oal_void wlan_pm_feed_wdg(oal_void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    pst_wlan_pm->ul_sleep_feed_wdg_cnt++;

#ifdef _PRE_WLAN_DOWNLOAD_PM
    if (wlan_pm_get_download_rate_limit_pps() != 0) {
        mod_timer(&pst_wlan_pm->st_watchdog_timer, jiffies + msecs_to_jiffies(10));
    } else {
        mod_timer(&pst_wlan_pm->st_watchdog_timer, jiffies + msecs_to_jiffies(WLAN_SLEEP_TIMER_PERIOD));
    }
#else
    mod_timer(&pst_wlan_pm->st_watchdog_timer, jiffies + msecs_to_jiffies(WLAN_SLEEP_TIMER_PERIOD));
#endif
}

/*
 * �� �� ��  : wlan_pm_stop_wdg
 * ��������  : ֹͣ50ms˯�߶�ʱ��
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
int32 wlan_pm_stop_wdg(struct wlan_pm_s *pst_wlan_pm_info)
{
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wlan_pm_stop_wdg");

    pst_wlan_pm_info->ul_wdg_timeout_curr_cnt = 0;
    pst_wlan_pm_info->ul_packet_cnt = 0;

    if (in_interrupt()) {
        return del_timer(&pst_wlan_pm_info->st_watchdog_timer);
    } else {
        return del_timer_sync(&pst_wlan_pm_info->st_watchdog_timer);
    }
}

static int wlan_pm_submit_sleep_work(struct wlan_pm_s *pm_data)
{
    if (pm_data->ul_packet_cnt == 0) {
        pm_data->ul_wdg_timeout_curr_cnt++;
        if ((pm_data->ul_wdg_timeout_curr_cnt >= pm_data->ul_wdg_timeout_cnt)) {
            if (wlan_pm_work_submit(pm_data, &pm_data->st_sleep_work) == 0) {
                /* �ύ��sleep work�󣬶�ʱ���������������ظ��ύsleep work */
                pm_data->ul_sleep_work_submit++;
                pm_data->ul_wdg_timeout_curr_cnt = 0;
                return OAL_SUCC;
            }
            oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_sleep_work_etc submit fail,work is running !");
        }
    } else {
        oal_print_hi11xx_log(HI11XX_LOG_DBG, "plat:wlan_pm_wdg_timeout_etc %d have packet %d....",
                             pm_data->ul_wdg_timeout_curr_cnt, pm_data->ul_packet_cnt);
        pm_data->ul_wdg_timeout_curr_cnt = 0;
        pm_data->ul_packet_cnt = 0;

        /* �б����շ�,����forbid sleep�������� */
        pm_data->ul_sleep_fail_forbid = 0;
    }
    return -OAL_EFAIL;
}

/*
 * �� �� ��  : wlan_pm_wdg_timeout
 * ��������  : 50ms˯�߶�ʱ����ʱ�����ύһ��sleep work
 * �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 */
void wlan_pm_wdg_timeout(unsigned long data)
{
    struct wlan_pm_s *pm_data = (struct wlan_pm_s *)(uintptr_t)data;
    if (pm_data == NULL) {
        return;
    }
    oal_print_hi11xx_log(HI11XX_LOG_DBG, "wlan_pm_wdg_timeout_etc....%d", pm_data->ul_wdg_timeout_curr_cnt);

    /* hcc bus switch process */
    hcc_bus_performance_core_schedule(HCC_CHIP_110X_DEV);

    pm_data->ul_packet_cnt += g_pm_wifi_rxtx_count;  // ��hmac��ͳ���շ�������

    pm_data->ul_packet_total_cnt += g_pm_wifi_rxtx_count;
    if (time_after(jiffies, pm_data->ul_packet_check_time)) {
        oal_print_hi11xx_log(HI11XX_LOG_WARN, "pkt_num:WIFI[%d]", pm_data->ul_packet_total_cnt);
        pm_data->ul_packet_check_time = jiffies + msecs_to_jiffies(WLAN_PACKET_CHECK_TIME);
    }

    g_pm_wifi_rxtx_count = 0;

    /* �͹��Ĺر�ʱtimer����ͣ */
    if (pm_data->ul_wlan_pm_enable) {
        if (wlan_pm_submit_sleep_work(pm_data) == OAL_SUCC) {
            return;
        }
    } else {
        pm_data->ul_packet_cnt = 0;
    }

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "wlan_pm_feed_wdg");
    wlan_pm_feed_wdg();

    return;
}

void wlan_pm_deepsleep_delay_timeout(unsigned long data)
{
    struct wlan_pm_s *pm_data = (struct wlan_pm_s *)(uintptr_t)data;
    if (pm_data == NULL) {
        return;
    }

    oal_wake_unlock(&pm_data->st_deepsleep_wakelock);

    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wlan_pm_deepsleep_delay_timeout release wakelock....%lu",
                         pm_data->st_deepsleep_wakelock.lock_count);

    if (pm_data->st_deepsleep_wakelock.lock_count != 0) {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "wlan_pm_deepsleep_delay_timeout release wakelock %lu!",
                         pm_data->st_deepsleep_wakelock.lock_count);
    }
}

int32 wlan_pm_stop_deepsleep_delay_timer(struct wlan_pm_s *pm_data)
{
    oal_wake_unlock(&pm_data->st_deepsleep_wakelock);

    oal_print_hi11xx_log(HI11XX_LOG_DBG, "wlan_pm_stop_deepsleep_delay_timer release wakelock....%lu",
                         pm_data->st_deepsleep_wakelock.lock_count);

    if (pm_data->st_deepsleep_wakelock.lock_count != 0) {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "wlan_pm_stop_deepsleep_delay_timer release wakelock %lu!",
                         pm_data->st_deepsleep_wakelock.lock_count);
    }
    if (in_interrupt()) {
        return del_timer(&pm_data->st_deepsleep_delay_timer);
    } else {
        return del_timer_sync(&pm_data->st_deepsleep_delay_timer);
    }
}

/*
 * �� �� ��  : wlan_pm_poweroff_cmd
 * ��������  : ����Ϣ��device��wifi device�ر�wifiϵͳ��Դ���ȴ�bcpu�����µ�
 * �� �� ֵ  :
 */
oal_int32 wlan_pm_poweroff_cmd(oal_void)
{
    oal_int32 ret;
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    oam_warning_log0(0, OAM_SF_PWR, "Send H2D_MSG_PM_WLAN_OFF cmd");

    hcc_tx_transfer_lock(hcc_get_110x_handler());

    if (wlan_pm_wakeup_dev() != OAL_SUCC) {
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_FAIL;
    }

    ret = hcc_bus_send_message(pst_wlan_pm->pst_bus, H2D_MSG_PM_WLAN_OFF);
    if (ret == OAL_SUCC) {
        /* �ȴ�deviceִ������ */
        msleep(20);
        if (board_get_wlan_wkup_gpio_val() == 0) {
            OAM_ERROR_LOG1(0, OAM_SF_PWR, "wlan_pm_poweroff_cmd  wait device ACK timeout && GPIO_LEVEL[%d] !",
                           board_get_wlan_wkup_gpio_val());

#ifdef PLATFORM_DEBUG_ENABLE
            debug_uart_read_wifi_mem(OAL_FALSE);
#endif
            hcc_tx_transfer_unlock(hcc_get_110x_handler());

            return OAL_FAIL;
        }
    } else {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "fail to send H2D_MSG_PM_WLAN_OFF");
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_FAIL;
    }

    hcc_tx_transfer_unlock(hcc_get_110x_handler());

    return OAL_SUCC;
}

void wlan_pm_wkup_src_debug_set(oal_uint32 ul_en)
{
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return;
    }
    pst_wlan_pm->ul_wkup_src_print_en = ul_en;

#endif
}

EXPORT_SYMBOL_GPL(wlan_pm_wkup_src_debug_set);

oal_uint32 wlan_pm_wkup_src_debug_get(void)
{
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if (pst_wlan_pm == OAL_PTR_NULL) {
        return OAL_FALSE;
    }
    return pst_wlan_pm->ul_wkup_src_print_en;
#else
    return OAL_FALSE;
#endif
}
EXPORT_SYMBOL_GPL(wlan_pm_wkup_src_debug_get);

/*
 * �� �� ��  : wlan_pm_shutdown_bcpu_cmd
 * ��������  : ����Ϣ��device��wifi device�ر�BCPU
 */
oal_int32 wlan_pm_shutdown_bcpu_cmd(oal_void)
{
#define RETRY_TIMES 3
    oal_uint32 i;
    oal_int32 ret = OAL_FAIL;
    oal_uint32 ul_ret;
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    oam_warning_log0(0, OAM_SF_PWR, "Send H2D_MSG_PM_BCPU_OFF cmd");

    hcc_tx_transfer_lock(hcc_get_110x_handler());

    for (i = 0; i < RETRY_TIMES; i++) {
        ret = wlan_pm_wakeup_dev();
        if (ret == OAL_SUCC) {
            break;
        }
    }

    if (ret != OAL_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_wakeup_dev fail!");
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_FAIL;
    }

    OAM_WARNING_LOG1(0, OAM_SF_PWR, "wlan_pm_wakeup_dev succ, retry times [%d]", i);

    oal_init_completion(&pst_wlan_pm->st_close_bcpu_done);

    ret = hcc_bus_send_message(pst_wlan_pm->pst_bus, H2D_MSG_PM_BCPU_OFF);
    if (ret == OAL_SUCC) {
        /* �ȴ�deviceִ������ */
        ul_ret = oal_wait_for_completion_timeout(&pst_wlan_pm->st_close_bcpu_done,
                                                 (oal_uint32)oal_msecs_to_jiffies(WLAN_POWEROFF_ACK_WAIT_TIMEOUT));
        if (ul_ret == 0) {
            OAM_ERROR_LOG0(0, OAM_SF_PWR, "wlan_pm_shutdown_bcpu_cmd wait device ACK timeout !");
            hcc_tx_transfer_unlock(hcc_get_110x_handler());
            return OAL_FAIL;
        }
    } else {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "fail to send H2D_MSG_PM_BCPU_OFF");
        hcc_tx_transfer_unlock(hcc_get_110x_handler());
        return OAL_FAIL;
    }

    hcc_tx_transfer_unlock(hcc_get_110x_handler());

    return OAL_SUCC;
}

/*
 * �� �� ��  : wlan_pm_dump_info
 * ��������  : debug, ����Ϣ��device���������ά����Ϣ
 */
void wlan_pm_dump_host_info(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();
    struct oal_sdio *pst_sdio = oal_get_sdio_default_handler();

    if (pst_wlan_pm == NULL) {
        return;
    }

    OAL_IO_PRINT("----------wlan_pm_dump_host_info begin-----------\n");
    OAL_IO_PRINT("power on:%ld, enable:%ld,g_wlan_pm_switch:%d\n",
                 pst_wlan_pm->ul_wlan_power_state, pst_wlan_pm->ul_wlan_pm_enable, wlan_pm_get_switch());
    OAL_IO_PRINT("dev state:%ld, sleep stage:%ld\n", pst_wlan_pm->ul_wlan_dev_state, pst_wlan_pm->ul_sleep_stage);
    OAL_IO_PRINT("open:%d,close:%d\n", pst_wlan_pm->ul_open_cnt, pst_wlan_pm->ul_close_cnt);
    if (pst_sdio != NULL) {
        OAL_IO_PRINT("sdio suspend:%d,sdio resume:%d\n", pst_sdio->ul_sdio_suspend, pst_sdio->ul_sdio_resume);
    }
    OAL_IO_PRINT("gpio_intr[no.%d]:%llu\n",
                 pst_wlan_pm->pst_bus->bus_dev->ul_wlan_irq, pst_wlan_pm->pst_bus->gpio_int_count);
    OAL_IO_PRINT("data_intr:%llu\n", pst_wlan_pm->pst_bus->data_int_count);
    OAL_IO_PRINT("wakeup_intr:%llu\n", pst_wlan_pm->pst_bus->wakeup_int_count);
    OAL_IO_PRINT("D2H_MSG_WAKEUP_SUCC:%d\n", pst_wlan_pm->pst_bus->msg[D2H_MSG_WAKEUP_SUCC].count);
    OAL_IO_PRINT("D2H_MSG_ALLOW_SLEEP:%d\n", pst_wlan_pm->pst_bus->msg[D2H_MSG_ALLOW_SLEEP].count);
    OAL_IO_PRINT("D2H_MSG_DISALLOW_SLEEP:%d\n", pst_wlan_pm->pst_bus->msg[D2H_MSG_DISALLOW_SLEEP].count);

    OAL_IO_PRINT("g_wakeup_dev_wait_ack:%d\n", oal_atomic_read(&g_wakeup_dev_wait_ack));
    OAL_IO_PRINT("wakeup_succ:%d\n", pst_wlan_pm->ul_wakeup_succ);
    OAL_IO_PRINT("wakeup_dev_ack:%d\n", pst_wlan_pm->ul_wakeup_dev_ack);
    OAL_IO_PRINT("wakeup_done_callback:%d\n", pst_wlan_pm->ul_wakeup_done_callback);
    OAL_IO_PRINT("wakeup_succ_work_submit:%d\n", pst_wlan_pm->ul_wakeup_succ_work_submit);
    OAL_IO_PRINT("wakeup_fail_wait_sdio:%d\n", pst_wlan_pm->ul_wakeup_fail_wait_sdio);
    OAL_IO_PRINT("wakeup_fail_timeout:%d\n", pst_wlan_pm->ul_wakeup_fail_timeout);
    OAL_IO_PRINT("wakeup_fail_set_reg:%d\n", pst_wlan_pm->ul_wakeup_fail_set_reg);
    OAL_IO_PRINT("wakeup_fail_submit_work:%d\n", pst_wlan_pm->ul_wakeup_fail_submit_work);
    OAL_IO_PRINT("sleep_succ:%d\n", pst_wlan_pm->ul_sleep_succ);
    OAL_IO_PRINT("sleep feed wdg:%d\n", pst_wlan_pm->ul_sleep_feed_wdg_cnt);
    OAL_IO_PRINT("sleep_fail_request:%d\n", pst_wlan_pm->ul_sleep_fail_request);
    OAL_IO_PRINT("sleep_fail_set_reg:%d\n", pst_wlan_pm->ul_sleep_fail_set_reg);
    OAL_IO_PRINT("sleep_fail_wait_timeout:%d\n", pst_wlan_pm->ul_sleep_fail_wait_timeout);
    OAL_IO_PRINT("sleep_fail_forbid:%d\n", pst_wlan_pm->ul_sleep_fail_forbid);
    OAL_IO_PRINT("sleep_work_submit:%d\n", pst_wlan_pm->ul_sleep_work_submit);
    OAL_IO_PRINT("wklock_cnt:%lu\n \n", pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);
    OAL_IO_PRINT("----------wlan_pm_dump_host_info end-----------\n");
}

void wlan_pm_dump_device_info(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    hcc_bus_send_message(pst_wlan_pm->pst_bus, H2D_MSG_PM_DEBUG);
}

void wlan_pm_info_clean(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();
    struct oal_sdio *pst_sdio = oal_get_sdio_default_handler();

    pst_wlan_pm->pst_bus->data_int_count = 0;
    pst_wlan_pm->pst_bus->wakeup_int_count = 0;

    pst_wlan_pm->pst_bus->msg[D2H_MSG_WAKEUP_SUCC].count = 0;
    pst_wlan_pm->pst_bus->msg[D2H_MSG_ALLOW_SLEEP].count = 0;
    pst_wlan_pm->pst_bus->msg[D2H_MSG_DISALLOW_SLEEP].count = 0;

    if (pst_sdio != NULL) {
        pst_sdio->ul_sdio_suspend = 0;
        pst_sdio->ul_sdio_resume = 0;
    }

    pst_wlan_pm->ul_wakeup_succ = 0;
    pst_wlan_pm->ul_wakeup_dev_ack = 0;
    pst_wlan_pm->ul_wakeup_done_callback = 0;
    pst_wlan_pm->ul_wakeup_succ_work_submit = 0;
    pst_wlan_pm->ul_wakeup_fail_wait_sdio = 0;
    pst_wlan_pm->ul_wakeup_fail_timeout = 0;
    pst_wlan_pm->ul_wakeup_fail_set_reg = 0;
    pst_wlan_pm->ul_wakeup_fail_submit_work = 0;

    pst_wlan_pm->ul_sleep_succ = 0;
    pst_wlan_pm->ul_sleep_feed_wdg_cnt = 0;
    pst_wlan_pm->ul_wakeup_done_callback = 0;
    pst_wlan_pm->ul_sleep_fail_set_reg = 0;
    pst_wlan_pm->ul_sleep_fail_wait_timeout = 0;
    pst_wlan_pm->ul_sleep_fail_forbid = 0;
    pst_wlan_pm->ul_sleep_work_submit = 0;

    return;
}

oal_void wlan_pm_debug_sleep(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if ((pst_wlan_pm != NULL) && pst_wlan_pm->pst_bus) {
        hcc_bus_sleep_request(pst_wlan_pm->pst_bus);

        pst_wlan_pm->ul_wlan_dev_state = HOST_ALLOW_TO_SLEEP;
    }

    return;
}

oal_void wlan_pm_debug_wakeup(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    if ((pst_wlan_pm != NULL) && pst_wlan_pm->pst_bus) {
        hcc_bus_wakeup_request(pst_wlan_pm->pst_bus);

        pst_wlan_pm->ul_wlan_dev_state = HOST_DISALLOW_TO_SLEEP;
    }

    return;
}

oal_void wlan_pm_debug_wake_lock(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    hcc_bus_wake_lock(pst_wlan_pm->pst_bus);
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wlan_pm_debug_wake_lock:wklock_cnt = %lu",
                         pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);

    return;
}

oal_void wlan_pm_debug_wake_unlock(void)
{
    struct wlan_pm_s *pst_wlan_pm = wlan_pm_get_drv();

    hcc_bus_wake_unlock(pst_wlan_pm->pst_bus);
    oal_print_hi11xx_log(HI11XX_LOG_INFO, "wlan_pm_debug_wake_unlock:wklock_cnt = %lu",
                         pst_wlan_pm->pst_bus->st_bus_wakelock.lock_count);

    return;
}
