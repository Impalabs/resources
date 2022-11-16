


#include "wal_linux_vendor.h"
#include "wal_linux_ioctl.h"
#include "wal_linux_netdev_ops.h"
#include "oal_ext_if.h"

#include "frw_ext_if.h"
#include "hmac_tx_data.h"
#include "mac_ftm.h"
#if (_PRE_FRW_FEATURE_PROCCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCCESS_ENTITY_THREAD)
#ifndef CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT
#include "frw_task.h"
#endif
#endif

#include "wlan_types.h"
#include "mac_vap.h"
#include "mac_resource.h"
#include "mac_regdomain.h"
#include "mac_ie.h"
#include "mac_device.h"

#include "hmac_ext_if.h"
#include "hmac_chan_mgmt.h"
#include "hmac_config.h"
#include "wal_main.h"
#include "wal_ext_if.h"
#include "wal_config.h"
#include "wal_regdb.h"
#include "wal_linux_scan.h"
#include "wal_linux_ioctl.h"
#include "wal_linux_bridge.h"
#include "wal_linux_flowctl.h"
#include "wal_linux_atcmdsrv.h"
#include "wal_linux_event.h"
#include "hmac_resource.h"
#include "hmac_p2p.h"
#include "hmac_stat.h"

#include "wal_linux_cfg80211.h"
#include "wal_linux_cfgvendor.h"
#include "wal_dfx.h"

#include "oal_hcc_host_if.h"
#include "plat_cali.h"

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/notifier.h>
#include <linux/inetdevice.h>
#include <net/addrconf.h>
#endif
#include "hmac_arp_offload.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 59)) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "../fs/proc/internal.h"
#endif

#include "hmac_auto_adjust_freq.h"
#ifdef _PRE_WLAN_FEATURE_MULTI_NETBUF_AMSDU
#include "hmac_tx_amsdu.h"
#endif

#include "hmac_roam_main.h"
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#include "oal_net.h"

#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_pm_wlan.h"
#include "plat_firmware.h"
#endif

#ifdef _PRE_WLAN_FEATURE_DFS
#include "hmac_dfs.h"
#endif

#ifdef _PRE_WLAN_FEATURE_11AX
#include "hmac_wifi6_self_cure.h"
#endif
#ifdef _PRE_WLAN_FEATURE_HIEX
#include "hmac_hiex.h"
#endif
#ifdef _PRE_WLAN_FEATURE_SNIFFER
#ifdef CONFIG_HW_SNIFFER
#include <hwnet/ipv4/sysctl_sniffer.h>
#endif
#endif
#include "hmac_dfx.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_NETDEV_OPS_C

#ifdef _PRE_WLAN_FEATURE_MONITOR
#define CONTINUOUS_DISABLE_ENABLE_WIFI_MAX_NUM 5
static int32_t g_continuous_disable_enable_wifi = 0; // sniffer��������:���sniffer�쳣�˳�״̬λδ��գ�����wifi��������
#endif

#ifdef _PRE_WLAN_FEATURE_MONITOR
void wal_clear_continuous_disable_enable_wifi_cnt(void)
{
    g_continuous_disable_enable_wifi = 0;
}
#endif


#ifdef _PRE_WLAN_FEATURE_DFR
OAL_STATIC oal_bool_enum_uint8 wal_dfr_recovery_check(oal_net_device_stru *pst_net_dev)
{
    oal_wireless_dev_stru *pst_wdev;
    mac_wiphy_priv_stru *pst_wiphy_priv;
    mac_device_stru *pst_mac_device;

    if (pst_net_dev == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_dfr_recovery_check::pst_net_dev is null!}");
        return OAL_FALSE;
    }

    pst_wdev = oal_netdevice_wdev(pst_net_dev);
    if (pst_wdev == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_dfr_recovery_check::pst_wdev is null!}");
        return OAL_FALSE;
    }

    pst_wiphy_priv = (mac_wiphy_priv_stru *)oal_wiphy_priv(pst_wdev->wiphy);
    if (pst_wiphy_priv == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_dfr_recovery_check::pst_wiphy_priv is null!}");
        return OAL_FALSE;
    }

    pst_mac_device = pst_wiphy_priv->pst_mac_device;
    if (pst_mac_device == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_dfr_recovery_check::pst_mac_device is null!}");
        return OAL_FALSE;
    }

    oam_warning_log2(0, OAM_SF_ANY, "{wal_dfr_recovery_check::recovery_flag:%d, uc_vap_num:%d.}",
                     g_st_dfr_info.bit_ready_to_recovery_flag, pst_mac_device->uc_vap_num);

    if ((g_st_dfr_info.bit_ready_to_recovery_flag == OAL_TRUE)
        && (!pst_mac_device->uc_vap_num)) {
        /* DFR�ָ�,�ڴ���ҵ��VAPǰ�·�У׼�Ȳ���,ֻ�·�һ�� */
        return OAL_TRUE;
    }

    return OAL_FALSE;
}
#endif /* #ifdef _PRE_WLAN_FEATURE_DFR */


OAL_STATIC void wal_netdev_open_flush_p2p_random_mac(oal_net_device_stru *pst_net_dev)
{
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    /* p2p interface mac��ַ����ֻ������add virtual������,
    ���ܱ�֤�ϲ��·������mac���µ�mib,�˴�ˢ��mac addr */
    if ((oal_strncmp("p2p-p2p0", pst_net_dev->name, OAL_STRLEN("p2p-p2p0")) == 0) ||
         (oal_strncmp("p2p-wlan0", pst_net_dev->name, OAL_STRLEN("p2p-wlan0")) == 0)) {
        wal_set_random_mac_to_mib(pst_net_dev);
    }
#endif
}
OAL_STATIC int32_t wal_netdev_set_power_on_flag(oal_net_device_stru *net_dev, int32_t pm_ret)
{
    int32_t l_ret;

    if (pm_ret != OAL_ERR_CODE_ALREADY_OPEN) {
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
        /* �����ϵ�ʱ��ΪFALSE */
        hwifi_config_init_force();
#endif

        /* �ϵ�host device_stru��ʼ�� */
        l_ret = wal_host_dev_init(net_dev);
        if (l_ret != OAL_SUCC) {
            oal_io_print("wal_host_dev_init FAIL %d \r\n", l_ret);
            return -OAL_EFAIL;
        }
    } else {
#ifdef _PRE_WLAN_FEATURE_DFR
    /* dfr ��δ����wlan1 */
        if (wal_dfr_recovery_check(net_dev)) {
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
            g_custom_cali_done = OAL_TRUE;
            wal_dfr_custom_cali();
            hwifi_config_init_force();
#endif
        }
#endif /* #ifdef _PRE_WLAN_FEATURE_DFR */
    }

    return OAL_SUCC;
}
OAL_STATIC int32_t wal_netdev_msg_process(oal_net_device_stru *net_dev)
{
    int32_t l_ret;
    uint32_t err_code;
    wal_msg_write_stru st_write_msg;
    wlan_p2p_mode_enum_uint8 en_p2p_mode;
    wal_msg_stru *pst_rsp_msg = NULL;
    oal_wireless_dev_stru *pst_wdev = NULL;

    /* ���¼���wal�㴦�� ��д��Ϣ */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_START_VAP, sizeof(mac_cfg_start_vap_param_stru));
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = net_dev;
    pst_wdev = net_dev->ieee80211_ptr;
    en_p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(pst_wdev->iftype);
    if (WLAN_P2P_BUTT == en_p2p_mode) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_open::wal_wireless_iftype_to_mac_p2p_mode return BUFF}");
        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI,
            CHR_LAYER_DRV, CHR_WIFI_DRV_EVENT_OPEN, CHR_WIFI_DRV_ERROR_POWER_ON);
        return -OAL_EINVAL;
    }
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode = en_p2p_mode;
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_mgmt_rate_init_flag = OAL_TRUE;

    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->l_ifindex = net_dev->ifindex;

    /* ������Ϣ */
    l_ret = wal_send_cfg_event(net_dev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_start_vap_param_stru),
        (uint8_t *)&st_write_msg, OAL_TRUE, &pst_rsp_msg);
    if (oal_unlikely(l_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open::wal_alloc_cfg_event err %d!}", l_ret);
        return l_ret;
    }

    /* ��������Ϣ */
    err_code = wal_check_and_release_msg_resp(pst_rsp_msg);
    if (err_code != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open::hmac start vap fail,err code[%u]!}", err_code);
        return -OAL_EINVAL;
    }

    return OAL_SUCC;
}
#ifdef _PRE_WLAN_CHBA_MGMT
uint32_t wal_start_demo(oal_net_device_stru *net_dev)
{
    wal_msg_write_stru msg;
    wal_msg_stru *rsp_msg = NULL;
    int32_t l_ret;
    uint32_t err_code;

    WAL_WRITE_MSG_HDR_INIT(&msg, WLAN_CFGID_START_DEMO_VAP, 0);

    while (!g_d2h_rate_complete) {
        oal_msleep(50);
    }

    l_ret = wal_send_cfg_event(net_dev, WAL_MSG_TYPE_WRITE, WAL_MSG_WRITE_MSG_HDR_LENGTH, (uint8_t *)&msg, OAL_TRUE,
        &rsp_msg);
    if (oal_unlikely(l_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_start_demo::wal_send_cfg_event return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ��ȡ���صĴ����� */
    err_code = wal_check_and_release_msg_resp(rsp_msg);
    if (err_code != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_start_demo::wal_send_cfg_event return err code:[%u]}", err_code);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;
}
#endif

OAL_STATIC int32_t _wal_netdev_open(oal_net_device_stru *pst_net_dev, uint8_t uc_entry_flag, int32_t pm_ret)
{
    int32_t l_ret;
    int32_t check_hw_status = 0;
    oal_netdev_priv_stru *pst_netdev_priv = NULL;

    if (oal_unlikely(pst_net_dev == NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_open::pst_net_dev null!}");
        return -OAL_EFAUL;
    }

    oal_io_print("wal_netdev_open,dev_name is:%s\n", pst_net_dev->name);
    oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open::iftype:%d.!}", pst_net_dev->ieee80211_ptr->iftype);

    l_ret = wal_netdev_set_power_on_flag(pst_net_dev, pm_ret);
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{_wal_netdev_open::wal_netdev_set_power_on_flag fail}");
        return l_ret;
    }

    /* ����vap�Ĵ���,������ͨ����־λ�ж��Ƿ���Ҫ�·�device */
    l_ret = wal_cfg_vap_h2d_event(pst_net_dev);
    if (l_ret != OAL_SUCC) {
        oal_io_print("wal_cfg_vap_h2d_event FAIL %d \r\n", l_ret);
        return -OAL_EFAIL;
    }
    oal_io_print("wal_cfg_vap_h2d_event succ \r\n");
    if (g_st_ap_config_info.l_ap_power_flag == OAL_TRUE) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_open::power state is on,in ap mode, start vap later.}");

        /* �˱�����ʱ��һ�Σ���ֹ framework����ģʽ�л�ǰ�·�����up���� */
        g_st_ap_config_info.l_ap_power_flag = OAL_FALSE;
        oal_net_tx_wake_all_queues(pst_net_dev); /* �������Ͷ��� */
        return OAL_SUCC;
    }
    /* �ŵ�����--ȡ��name�ж� */
    if (oal_value_eq_any2(pst_net_dev->ieee80211_ptr->iftype, NL80211_IFTYPE_STATION, NL80211_IFTYPE_P2P_DEVICE)) {
        l_ret = wal_init_wlan_vap(pst_net_dev);
        if (l_ret != OAL_SUCC) {
            chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI,
                CHR_LAYER_DRV, CHR_WIFI_DRV_EVENT_OPEN, CHR_WIFI_DRV_ERROR_POWER_ON);
            return -OAL_EFAIL;
        }
    } else if (NL80211_IFTYPE_AP == pst_net_dev->ieee80211_ptr->iftype) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_open::ap mode,no need to start vap.!}");
#ifdef _PRE_WLAN_CHBA_MGMT
        if ((g_optimized_feature_switch_bitmap & BIT(CUSTOM_OPTIMIZE_CHBA)) &&
            (oal_strcmp("chba0", pst_net_dev->name) == 0)) {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_open::wal_setup_chba.!}");
            return wal_setup_chba(pst_net_dev);
        }
#endif
        oal_net_tx_wake_all_queues(pst_net_dev); /* �������Ͷ��� */
        return OAL_SUCC;
    }

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* �͹��Ķ��ƻ����� */
    (g_wlan_host_pm_switch == OAL_TRUE) ? wlan_pm_enable() : wlan_pm_disable();
#endif
    wal_netdev_open_flush_p2p_random_mac(pst_net_dev);
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

    /* ���¼���wal�㴦�� ��д��Ϣ */
    l_ret = wal_netdev_msg_process(pst_net_dev);
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{_wal_netdev_open::wal_netdev_msg_process fail}");
        return l_ret;
    }

    oal_netdevice_flags(pst_net_dev) |= OAL_IFF_RUNNING;

    pst_netdev_priv = (oal_netdev_priv_stru *)oal_net_dev_wireless_priv(pst_net_dev);
    if (pst_netdev_priv->uc_napi_enable && (!pst_netdev_priv->uc_state) &&
        (oal_value_eq_any3(pst_net_dev->ieee80211_ptr->iftype,
            NL80211_IFTYPE_STATION, NL80211_IFTYPE_P2P_DEVICE, NL80211_IFTYPE_P2P_CLIENT))) {
        oal_napi_enable(&pst_netdev_priv->st_napi);
        pst_netdev_priv->uc_state = 1;
    }

    oal_net_tx_wake_all_queues(pst_net_dev); /* �������Ͷ��� */
    /* 1102Ӳ��fem��DEVICE����HOST�麸lna�ջټ��,ֻ��wlan0ʱ��ӡ */
    if ((pst_net_dev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION) &&
        (0 == (oal_strcmp("wlan0", pst_net_dev->name)))) {
        wal_atcmsrv_ioctl_get_fem_pa_status(pst_net_dev, &check_hw_status);
    }

    return OAL_SUCC;
}

int32_t wal_netdev_open_ext(oal_net_device_stru *pst_net_dev)
{
#ifdef  _PRE_CONFIG_HISI_S3S4_POWER_STATE
    if (g_st_recovery_info.device_s3s4_process_flag) {
        oam_warning_log0(0, OAM_SF_CFG, "{wal_netdev_open_ext::s3s4 is processing!}\r\n");
        return -OAL_EFAIL;
    }
#endif
    oam_warning_log0(0, OAM_SF_CFG, "{wal_netdev_open_ext::netdev open!}\r\n");
    return wal_netdev_open(pst_net_dev, OAL_TRUE);
}

#ifdef _PRE_WLAN_FEATURE_SNIFFER
#ifdef CONFIG_HW_SNIFFER
void wal_init_sniffer_cmd_function(void)
{
    static int is_func_set = 0;
    if (!is_func_set) {
        is_func_set = 1;
        proc_set_hipriv_func(wal_hipriv_parse_cmd);
    }
}
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_MONITOR
void wal_netdev_open_en_monitor_limit(oal_net_device_stru *net_dev)
{
    mac_vap_stru *mac_vap = NULL;
    hmac_device_stru *hmac_device = NULL;

    mac_vap = oal_net_dev_priv(net_dev);
    if (mac_vap == NULL) {
        return;
    }
    hmac_device = hmac_res_get_mac_dev(mac_vap->uc_device_id);
    if (hmac_device == NULL) {
        return;
    }
#ifdef _PRE_WLAN_FEATURE_SNIFFER
#ifdef CONFIG_HW_SNIFFER
    // netdev_open����sniffer�·�ָ��
    wal_init_sniffer_cmd_function();
#endif
#endif

    g_continuous_disable_enable_wifi++;
    if ((hmac_device->en_monitor_mode == OAL_TRUE) &&
        (g_continuous_disable_enable_wifi >= CONTINUOUS_DISABLE_ENABLE_WIFI_MAX_NUM)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open_en_monitor_limit:: g_continuous_disable_enable_wifi=%d}\n",
                         g_continuous_disable_enable_wifi);
        hmac_device->en_monitor_mode = OAL_FALSE;
        g_continuous_disable_enable_wifi = 0;
    }
}
#endif
/*
 * �� �� ��  : wal_wait_host_rf_cali_init_complete
 * ��������  : �ȴ��������µ�У׼������ɣ������ڿ���У׼δ���ʱ�ϲ�Ӧ�ô�wifi
 */
void wal_wait_host_rf_cali_init_complete(void)
{
    int32_t ret;

    /* �ȴ�����У׼������� */
    ret = oal_wait_event_timeout_m(g_wlan_cali_complete_wq, hmac_get_wlan_first_power_on_cali_flag() == OAL_FALSE,
        (uint32_t)oal_msecs_to_jiffies(WAL_WAIT_CALI_COMPLETE_TIME));
    if (ret == 0) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_netdev_open::wait first powon rf cali complete [%d]ms timeout!}",
            WAL_WAIT_CALI_COMPLETE_TIME);
    }
}

int32_t wal_netdev_open(oal_net_device_stru *pst_net_dev, uint8_t uc_entry_flag)
{
    int32_t ret, pm_ret;

    if (oal_netdevice_flags(pst_net_dev) & OAL_IFF_RUNNING) {
        return OAL_SUCC;
    }
    wal_wait_host_rf_cali_init_complete();
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    g_netdev_is_open = OAL_TRUE;
#endif
    pm_ret = wlan_pm_open();
    if (pm_ret == OAL_FAIL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_open::wlan_pm_open Fail!}");
        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI,
            CHR_LAYER_DRV, CHR_WIFI_DRV_EVENT_OPEN, CHR_WIFI_DRV_ERROR_POWER_ON);
        return -OAL_EFAIL;
    }
#ifdef _PRE_WLAN_FEATURE_DFR
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_lock(&g_st_dfr_info.wifi_excp_mutex);
#endif
#endif

    wal_wake_lock();
    ret = _wal_netdev_open(pst_net_dev, uc_entry_flag, pm_ret);
    wal_wake_unlock();

#ifdef _PRE_WLAN_FEATURE_MONITOR
    wal_netdev_open_en_monitor_limit(pst_net_dev);
#endif

#ifdef _PRE_WLAN_FEATURE_DFR
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_unlock(&g_st_dfr_info.wifi_excp_mutex);
#endif
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)) && \
    (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* ��¼wlan0 ����ʱ�� */
    if (oal_strncmp("wlan0", pst_net_dev->name, OAL_STRLEN("wlan0")) == 0) {
        g_st_wifi_radio_stat.ull_wifi_on_time_stamp = OAL_TIME_JIFFY;
    }
#endif
    wlan_chip_send_20m_all_chn_cali_data(pst_net_dev);

    return ret;
}

oal_net_device_stats_stru *wal_netdev_get_stats(oal_net_device_stru *pst_net_dev)
{
    oal_net_device_stats_stru *pst_stats = &(pst_net_dev->stats);
    mac_vap_stru *mac_vap = NULL;
    hmac_vap_stru *hmac_vap = NULL;
    hmac_vap_stat_stru *hmac_vap_stats = NULL;

    mac_vap = oal_net_dev_priv(pst_net_dev);
    if (mac_vap == NULL) {
        return pst_stats;
    }

    hmac_vap = mac_res_get_hmac_vap(mac_vap->uc_vap_id);
    if (hmac_vap == NULL) {
        return pst_stats;
    }

    hmac_vap_stats = hmac_stat_get_vap_stat(hmac_vap);

    /* ����ͳ����Ϣ��net_device */
    pst_stats->rx_packets = hmac_vap_stats->rx_packets;
    pst_stats->rx_bytes = hmac_vap_stats->rx_bytes;

    pst_stats->tx_packets = hmac_vap_stats->tx_packets;
    pst_stats->tx_bytes = hmac_vap_stats->tx_bytes;
    return pst_stats;
}


mac_device_stru *wal_get_macdev_by_netdev(oal_net_device_stru *pst_net_dev)
{
    oal_wireless_dev_stru *pst_wdev;
    mac_wiphy_priv_stru *pst_wiphy_priv = NULL;

    pst_wdev = oal_netdevice_wdev(pst_net_dev);
    if (pst_wdev == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_get_macdev_by_netdev::pst_wdev is null!}");
        return NULL;
    }
    pst_wiphy_priv = (mac_wiphy_priv_stru *)(oal_wiphy_priv(pst_wdev->wiphy));
    return pst_wiphy_priv->pst_mac_device;
}
OAL_STATIC int32_t wal_netdev_stop_msg_process(oal_net_device_stru *net_dev)
{
    int32_t l_ret;
    uint32_t err_code;
    wal_msg_write_stru st_write_msg;
    oal_wireless_dev_stru *pst_wdev = NULL;
    wal_msg_stru *pst_rsp_msg = NULL;
    wlan_p2p_mode_enum_uint8 en_p2p_mode;

    /* ���¼���wal�㴦�� */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DOWN_VAP, sizeof(mac_cfg_down_vap_param_stru));
    ((mac_cfg_down_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = net_dev;
    pst_wdev = net_dev->ieee80211_ptr;
    en_p2p_mode = wal_wireless_iftype_to_mac_p2p_mode(pst_wdev->iftype);
    if (WLAN_P2P_BUTT == en_p2p_mode) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_stop::wal_wireless_iftype_to_mac_p2p_mode return BUFF}");
        return -OAL_EINVAL;
    }
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode = en_p2p_mode;

    /* ������Ϣ */
    l_ret = wal_send_cfg_event(net_dev, WAL_MSG_TYPE_WRITE,
        WAL_MSG_WRITE_MSG_HDR_LENGTH + sizeof(mac_cfg_down_vap_param_stru),
        (uint8_t *)&st_write_msg, OAL_TRUE, &pst_rsp_msg);
    if (oal_unlikely(l_ret != OAL_SUCC)) {
        if (oal_net_dev_priv(net_dev) == NULL) {
            /* �ر�net_device���������Ӧvap ��null�����flags running��־ */
            oal_netdevice_flags(net_dev) &= (~OAL_IFF_RUNNING);
            oam_warning_log1(0, OAM_SF_ANY,
                "{wal_netdev_stop::netdev's vap null, set flag not running, if_idx:%d}", net_dev->ifindex);
        }
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::wal_alloc_cfg_event err %d!}", l_ret);
        return l_ret;
    }

    /* ��������Ϣ */
    err_code = wal_check_and_release_msg_resp(pst_rsp_msg);
    if (err_code != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::hmac stop vap fail!err code [%d]}", err_code);
        return -OAL_EFAIL;
    }
    return OAL_SUCC;
}
OAL_STATIC int32_t wal_netdev_stop_get_netdev(oal_net_device_stru *net_dev,
    mac_device_stru *mac_dev, mac_vap_stru *mac_vap)
{
    int32_t l_ret;
    mac_device_stru *pst_mac_device = NULL;
    int8_t ac_wlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH] = { 0 };
    int8_t ac_p2p_netdev_name[MAC_NET_DEVICE_NAME_LENGTH] = { 0 };
    int8_t ac_p2p_group_name[MAC_NET_DEVICE_NAME_LENGTH] = { 0 };

    /* ͨ��device id��ȡnetdev���� */
    snprintf_s(ac_wlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "wlan%d", mac_dev->uc_device_id);
    snprintf_s(ac_p2p_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "p2p%d", mac_dev->uc_device_id);
    snprintf_s(ac_p2p_group_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "p2p-p2p%d", mac_dev->uc_device_id);

    if ((NL80211_IFTYPE_STATION == net_dev->ieee80211_ptr->iftype) &&
        ((0 == oal_strcmp((const char *)ac_wlan_netdev_name, net_dev->name)) ||
        (0 == oal_strcmp(WLAN1_NETDEV_NAME, net_dev->name)))) {
        l_ret = wal_deinit_wlan_vap(net_dev);
        if (l_ret != OAL_SUCC) {
            return l_ret;
        }
    }

    /* p2p0 downʱ ɾ��VAP */
    // ����ϲ�ɾ��p2p group(GC or GO)ʱ������wal_del_p2p_groupɾ��p2p group��vap����work queueɾ��GC or GO netdev
    // ����ϲ㲻�·�ɾ��p2p group���������ɾ��p2p device��ʱ��ɾ��p2p group����Ҫ��ɾ��wlan0��ʱ��ɾ��p2p group
    if ((net_dev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION)
        && (0 == oal_strcmp((const char *)ac_p2p_netdev_name, net_dev->name))) {
        /* ����ɾ��p2pС�� */
        mac_vap = oal_net_dev_priv(net_dev);
        if (mac_vap == NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_stop::pst_mac_vap is null, netdev released.}");
            return OAL_SUCC;
        }
        pst_mac_device = mac_res_get_dev(mac_vap->uc_device_id);
        if (pst_mac_device != NULL) {
            wal_del_p2p_group(pst_mac_device);
        }
    }

    if (((net_dev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION) ||
        (net_dev->ieee80211_ptr->iftype == NL80211_IFTYPE_P2P_DEVICE)) &&
        (0 == oal_strcmp((const char *)ac_p2p_netdev_name, net_dev->name))) {
        l_ret = wal_deinit_wlan_vap(net_dev);
        if (l_ret != OAL_SUCC) {
            return l_ret;
        }
    }

    return OAL_SUCC;
}

OAL_STATIC int32_t _wal_netdev_stop(oal_net_device_stru *pst_net_dev)
{
    int32_t l_ret;
    mac_vap_stru *pst_mac_vap = NULL;
    mac_device_stru *pst_mac_dev = NULL;
    oal_netdev_priv_stru *pst_netdev_priv = NULL;

    if (oal_unlikely(pst_net_dev == NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_stop::pst_net_dev null!}");
        return -OAL_EFAUL;
    }

    /* stop the netdev's queues */
    oal_net_tx_stop_all_queues(pst_net_dev); /* ֹͣ���Ͷ��� */
    wal_force_scan_complete(pst_net_dev, OAL_TRUE);

    oal_io_print("wal_netdev_stop,dev_name is:%s\n", pst_net_dev->name);
    oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::iftype:%d.!}", pst_net_dev->ieee80211_ptr->iftype);

    /* APģʽ��,��ģʽ�л�ʱdown��ɾ�� vap */
    if (pst_net_dev->ieee80211_ptr->iftype == NL80211_IFTYPE_AP) {
        l_ret = wal_netdev_stop_ap(pst_net_dev);
        return l_ret;
    }

    /* ���netdev����running״̬����ֱ�ӷ��سɹ� */
    if ((oal_netdevice_flags(pst_net_dev) & OAL_IFF_RUNNING) == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "{_wal_netdev_stop::vap is already down!}");
        return OAL_SUCC;
    }

    /* ���netdev��mac vap�Ѿ��ͷţ���ֱ�ӷ��سɹ� */
    pst_mac_vap = oal_net_dev_priv(pst_net_dev);
    if (oal_unlikely(pst_mac_vap == NULL)) {
        oam_warning_log1(0, OAM_SF_CFG, "{_wal_netdev_stop:: mac vap of netdevice is down!iftype:[%d]}",
            pst_net_dev->ieee80211_ptr->iftype);
        return OAL_SUCC;
    }

    if (pst_mac_vap->en_vap_state == MAC_VAP_STATE_INIT) {
        oam_warning_log3(0, OAM_SF_ANY, "{_wal_netdev_stop::vap is already down!iftype:[%d] vap mode[%d] p2p mode[%d]}",
                         pst_net_dev->ieee80211_ptr->iftype, pst_mac_vap->en_vap_mode, pst_mac_vap->en_p2p_mode);
        if (WLAN_P2P_DEV_MODE != pst_mac_vap->en_p2p_mode) {
            return OAL_SUCC;
        }
    }

    /* ���¼���wal�㴦�� */
    l_ret = wal_netdev_stop_msg_process(pst_net_dev);
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{_wal_netdev_stop::wal_netdev_stop_msg_process fail}");
        return l_ret;
    }

    pst_mac_dev = wal_get_macdev_by_netdev(pst_net_dev);
    if (pst_mac_dev == NULL) {
        oal_io_print("wal_deinit_wlan_vap::wal_get_macdev_by_netdev FAIL");
        return -OAL_EFAIL;
    }
    /* ͨ��device id��ȡnetdev���� */
    l_ret = wal_netdev_stop_get_netdev(pst_net_dev, pst_mac_dev, pst_mac_vap);
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{_wal_netdev_stop::wal_netdev_stop_get_netdev fail}");
        return l_ret;
    }

    pst_netdev_priv = (oal_netdev_priv_stru *)oal_net_dev_wireless_priv(pst_net_dev);
    if (pst_netdev_priv->uc_napi_enable &&
        ((NL80211_IFTYPE_STATION == pst_net_dev->ieee80211_ptr->iftype) ||
         (NL80211_IFTYPE_P2P_DEVICE == pst_net_dev->ieee80211_ptr->iftype) ||
         (NL80211_IFTYPE_P2P_CLIENT == pst_net_dev->ieee80211_ptr->iftype))) {
#ifndef WIN32
        oal_netbuf_list_purge(&pst_netdev_priv->st_rx_netbuf_queue);
#endif
        oal_napi_disable(&pst_netdev_priv->st_napi);
        pst_netdev_priv->uc_state = 0;
    }

    return OAL_SUCC;
}


int32_t wal_netdev_stop_ap(oal_net_device_stru *pst_net_dev)
{
    int32_t l_ret;

    if (NL80211_IFTYPE_AP != pst_net_dev->ieee80211_ptr->iftype) {
        return OAL_SUCC;
    }

    /* ����ɨ��,�Է���20/40Mɨ������йر�AP */
    wal_force_scan_complete(pst_net_dev, OAL_TRUE);

    /* AP�ر��л���STAģʽ,ɾ�����vap */
    if (OAL_SUCC != wal_stop_vap(pst_net_dev)) {
        oam_warning_log0(0, OAM_SF_CFG, "{wal_netdev_stop_ap::wal_stop_vap enter a error.}");
    }
    l_ret = wal_deinit_wlan_vap(pst_net_dev);
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_CFG, "{wal_netdev_stop_ap::wal_deinit_wlan_vap enter a error.}");
        return l_ret;
    }

    /* Del aput����Ҫ�л�netdev iftype״̬��station */
    pst_net_dev->ieee80211_ptr->iftype = NL80211_IFTYPE_STATION;

    /* aput�µ� */
    wal_set_power_mgmt_on(OAL_TRUE);
    l_ret = wal_set_power_on(pst_net_dev, OAL_FALSE);
    if (l_ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_CFG, "{wal_netdev_stop_ap::wal_set_power_on fail [%d]!}", l_ret);
        return l_ret;
    }
    return OAL_SUCC;
}

int32_t wal_netdev_stop(oal_net_device_stru *pst_net_dev)
{
    int32_t ret;

    if (oal_unlikely(pst_net_dev == NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_stop::pst_net_dev null!}");
        return -OAL_EFAUL;
    }

#ifdef _PRE_WLAN_FEATURE_DFR
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_lock(&g_st_dfr_info.wifi_excp_mutex);
#endif
#endif

    wal_wake_lock();
    ret = _wal_netdev_stop(pst_net_dev);
    wal_wake_unlock();

    if (ret == OAL_SUCC) {
        oal_netdevice_flags(pst_net_dev) &= ~OAL_IFF_RUNNING;
    }

#ifdef _PRE_WLAN_FEATURE_DFR
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_unlock(&g_st_dfr_info.wifi_excp_mutex);
#endif
#endif
    return ret;
}


OAL_STATIC int32_t _wal_netdev_set_mac_addr(oal_net_device_stru *net_dev, oal_sockaddr_stru *mac_addr)
{
    oal_wireless_dev_stru  *wdev = NULL; /* ����P2P ������p2p0 �� p2p-p2p0 MAC ��ַ��wlan0��ȡ */
#if defined(_PRE_WLAN_FEATURE_SMP_SUPPORT)
    mac_vap_stru *pst_mac_vap = NULL;
#endif

    if (oal_unlikely(oal_any_null_ptr2(net_dev, mac_addr))) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::net_dev or p_addr null ptr}");
        return -OAL_EFAUL;
    }

    if (oal_netif_running(net_dev)) {
    oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::cannot set address; device running!}\r\n");
    return -OAL_EBUSY;
    }

    if (ether_is_multicast(mac_addr->sa_data)) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::can not set group/broadcast addr!}\r\n");
        return -OAL_EINVAL;
    }

    oal_set_mac_addr ((uint8_t *)(net_dev->dev_addr), (uint8_t *)(mac_addr->sa_data));

    wdev = (oal_wireless_dev_stru *)net_dev->ieee80211_ptr;
    if (wdev == NULL) {
        return -OAL_EFAUL;
    }

    oam_warning_log4(0, OAM_SF_ANY,
                     "{wal_netdev_set_mac_addr::iftype [%d], mac_addr[%.2x:xx:xx:xx:%.2x:%.2x]..}\r\n",
                     wdev->iftype, net_dev->dev_addr[MAC_ADDR_0],
                     net_dev->dev_addr[MAC_ADDR_4], net_dev->dev_addr[MAC_ADDR_5]);
    return OAL_SUCC;
}

int32_t wal_netdev_set_mac_addr(oal_net_device_stru *pst_net_dev, void *p_addr)
{
    int32_t ret;
    oal_sockaddr_stru *mac_addr = NULL;

    mac_addr = (oal_sockaddr_stru *)p_addr;
    wal_wake_lock();
    ret = _wal_netdev_set_mac_addr(pst_net_dev, mac_addr);
    wal_wake_unlock();

    if (ret != OAL_SUCC) {
        chr_exception_report(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI,
            CHR_LAYER_DRV, CHR_WIFI_DRV_EVENT_OPEN, CHR_WIFI_DRV_ERROR_POWER_ON_SET_MAC_ADDR);
    }

    return ret;
}


int32_t wal_net_device_ioctl(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr, int32_t cmd)
{
    int32_t l_ret = 0;

    if (oal_any_null_ptr2(pst_net_dev, pst_ifr)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_net_device_ioctl::pst_dev or pst_ifr null!}");
        return -OAL_EFAUL;
    }

    if (pst_ifr->ifr_data == NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_net_device_ioctl::pst_ifr->ifr_data is NULL, cmd[0x%x]!}", cmd);
        return -OAL_EFAUL;
    }

    /* 1102 wpa_supplicant ͨ��ioctl �·����� */
    if (cmd == (WAL_SIOCDEVPRIVATE + 1)) {
        l_ret = wal_vendor_priv_cmd(pst_net_dev, pst_ifr, cmd);
        return l_ret;
    }
#if (_PRE_OS_VERSION_WIN32 != _PRE_OS_VERSION)
    /* atcmdsrv ͨ��ioctl�·�����ϲ�����+2 */
    if (cmd == (WAL_SIOCDEVPRIVATE + 2)) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        if (!capable(CAP_NET_ADMIN)) {
            return -EPERM;
        }
#endif
        if (hi11xx_get_os_build_variant() == HI1XX_OS_BUILD_VARIANT_ROOT) {
            wal_wake_lock();
            l_ret = wal_atcmdsrv_wifi_priv_cmd(pst_net_dev, pst_ifr, cmd);
            wal_wake_unlock();
        }

        return l_ret;
    }
#ifdef _PRE_WLAN_RR_PERFORMANCE
    /* PC APP ͨ��ioctl�·�������ϲ�����+3 */
    if (cmd == (WAL_SIOCDEVPRIVATE + 3)) {
        l_ret = wal_ext_priv_cmd(pst_net_dev, pst_ifr);
        return l_ret;
    }
#endif /* _PRE_WLAN_RR_PERFORMANCE */
#endif
    oam_warning_log1(0, OAM_SF_ANY, "{wal_net_device_ioctl::unrecognised cmd[0x%x]!}", cmd);
    return OAL_SUCC;
}
