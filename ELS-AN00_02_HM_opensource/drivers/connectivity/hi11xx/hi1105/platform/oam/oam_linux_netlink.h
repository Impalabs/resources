

#ifndef __OAM_LINUX_NETLINK_H__
#define __OAM_LINUX_NETLINK_H__

/* ����ͷ�ļ����� */
#include "oal_ext_if.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_OAM_LINUX_NETLINK_H
/* �궨�� */
#if (_PRE_CONFIG_TARGET_PRODUCT == _PRE_TARGET_PRODUCT_TYPE_WS835DMB)
#define OAM_NETLINK_ID 29  // 1151honor835 �޸ĳ�29����ֹ��������Ʒ��ko�����Լ�ҵ������ʱ������netlink������ͻ
#elif (_PRE_CONFIG_TARGET_PRODUCT == _PRE_TARGET_PRODUCT_TYPE_E5) ||  \
      (_PRE_CONFIG_TARGET_PRODUCT == _PRE_TARGET_PRODUCT_TYPE_CPE)
#define OAM_NETLINK_ID 26  // E5 �޸ĳ�26
#elif (_PRE_CONFIG_TARGET_PRODUCT == _PRE_TARGET_PRODUCT_TYPE_5630HERA)
#define OAM_NETLINK_ID 26  // HERA �޸ĳ�26
#else
#define OAM_NETLINK_ID 25
#endif

/* STRUCT���� */
typedef struct {
    oal_sock_stru *pst_nlsk;
    uint32_t ul_pid;
} oam_netlink_stru;

typedef struct {
    uint32_t (*p_oam_sdt_func)(uint8_t *puc_data, uint32_t ul_len);
    uint32_t (*p_oam_hut_func)(uint8_t *puc_data, uint32_t ul_len);
    uint32_t (*p_oam_alg_func)(uint8_t *puc_data, uint32_t ul_len);
    uint32_t (*p_oam_daq_func)(uint8_t *puc_data, uint32_t ul_len);
    uint32_t (*p_oam_reg_func)(uint8_t *puc_data, uint32_t ul_len);
    uint32_t (*p_oam_acs_func)(uint8_t *puc_data, uint32_t ul_len);
    uint32_t (*p_oam_psta_func)(uint8_t *puc_data, uint32_t ul_len);
} oam_netlink_proto_ops;

/* �������� */
extern uint32_t oam_netlink_kernel_create(void);
extern void oam_netlink_kernel_release(void);
#endif /* end of oam_linux_netlink.h */
