

#ifndef __HOST_MAC_1106_H__
#define __HOST_MAC_1106_H__

#include "oal_ext_if.h"
#include "frw_ext_if.h"
#include "hal_common.h"
#include "host_hal_device.h"

#if (_PRE_TEST_MODE == _PRE_TEST_MODE_ST)
#define HI1106_MAC_CFG_HOST_REG_BUTT       0xBC
#define HI1106_MAC_RPT_HOST_REG_BUTT       0xBC
#define HI1106_DFX_REG_BANK_BUTT           0xBC
#define HI1106_MAC_CFG_DMAC_BANK0_REG_BUTT 0xBC
#define HI1106_MAC_RPT_DMAC_REG_BUTT       0xBC
#define HI1106_MAC_CFG_DMAC_BANK4_REG_BUTT 0xBC
#define HI1106_RX_CHN_EST_REG_BANK_BUTT    0xBC

uint32_t g_st_host_hi1106_mac_cfg_host_reg_base[HI1106_MAC_CFG_HOST_REG_BUTT];
uint32_t g_st_host_hi1106_mac_rpt_host_reg_base[HI1106_MAC_RPT_HOST_REG_BUTT];
uint32_t g_st_host_hi1106_dfx_reg_bank_base[HI1106_DFX_REG_BANK_BUTT];
uint32_t g_st_host_hi1106_mac_cfg_dmac_bank0_reg_base[HI1106_MAC_CFG_DMAC_BANK0_REG_BUTT];
uint32_t g_st_host_hi1106_mac_rpt_dmac_reg_base[HI1106_MAC_RPT_DMAC_REG_BUTT];
uint32_t g_st_host_hi1106_mac_cfg_dmac_bank4_reg_base[HI1106_MAC_CFG_DMAC_BANK4_REG_BUTT];
uint32_t g_st_host_hi1106_rx_chn_est_reg_band_base[HI1106_RX_CHN_EST_REG_BANK_BUTT];
uint32_t hi1106_rf_ctrl_reg_bank_0_base[HI1106_RX_CHN_EST_REG_BANK_BUTT];
uint32_t hi1106_cali_test_reg_bank_0_base[HI1106_RX_CHN_EST_REG_BANK_BUTT];
uint32_t hi1106_phy_glb_reg_bank_base[HI1106_RX_CHN_EST_REG_BANK_BUTT];

#define HI1106_MAC_CFG_HOST_REG_BASE             ((uint32_t)&g_st_host_hi1106_mac_cfg_host_reg_base[0]) /* 06STʹ�� */
#define HI1106_MAC_RPT_HOST_REG_BASE             ((uint32_t)&g_st_host_hi1106_mac_rpt_host_reg_base[0])
#define HI1106_DFX_REG_BANK_BASE                 ((uint32_t)&g_st_host_hi1106_dfx_reg_bank_base[0])
#define HI1106_MAC_CFG_DMAC_BANK0_REG_BASE       ((uint32_t)&g_st_host_hi1106_mac_cfg_dmac_bank0_reg_base[0])
#define HI1106_MAC_RPT_DMAC_REG_BASE             ((uint32_t)&g_st_host_hi1106_mac_rpt_dmac_reg_base[0])
#define HI1106_MAC_CFG_DMAC_BANK4_REG_BASE       ((uint32_t)&g_st_host_hi1106_mac_cfg_dmac_bank4_reg_base[0])
#define HI1106_RX_CHN_EST_REG_BANK_BASE          ((uint32_t)&g_st_host_hi1106_rx_chn_est_reg_band_base[0])
#define HI1106_RF_CTRL_REG_BANK_0_BASE           ((uint32_t)&hi1106_rf_ctrl_reg_bank_0_base[0])
#define HI1106_CALI_TEST_REG_BANK_0_BASE         ((uint32_t)&hi1106_cali_test_reg_bank_0_base[0])
#define HI1106_PHY_GLB_REG_BANK_BASE             ((uint32_t)&hi1106_phy_glb_reg_bank_base[0])
/* _PRE_TEST_MODE_ST */
#else

#define HI1106_MAC_CFG_HOST_REG_BASE                        0x40020000
#define HI1106_MAC_RPT_HOST_REG_BASE                        0x40030000
#define HI1106_DFX_REG_BANK_BASE                            0x40064000
#define HI1106_MAC_CFG_DMAC_BANK0_REG_BASE                  0x40022000
#define HI1106_MAC_RPT_DMAC_REG_BASE                        0x40032000
#define HI1106_MAC_CFG_DMAC_BANK4_REG_BASE                  0x4002A000
#define HI1106_RX_CHN_EST_REG_BANK_BASE                     0x40072800
#define HI1106_RF_CTRL_REG_BANK_0_BASE                      0x40064800
#define HI1106_CALI_TEST_REG_BANK_0_BASE                    0x40061000
#define HI1106_PHY_GLB_REG_BANK_BASE                        0x40060800

/* != _PRE_TEST_MODE_ST */
#endif

/* PHY �Ĵ��� */
#define HI1106_MAC_HOST_INTR_CLR_REG                        (HI1106_MAC_CFG_HOST_REG_BASE + 0x50)
#define HI1106_MAC_HOST_INTR_MASK_REG                       (HI1106_MAC_CFG_HOST_REG_BASE + 0x54)
#define HI1106_DFX_REG_BANK_PHY_INTR_RPT_REG                (HI1106_DFX_REG_BANK_BASE + 0xBC)     /* PHY�ж�״̬ */
#define HI1106_DFX_REG_BANK_CFG_INTR_MASK_HOST_REG          (HI1106_DFX_REG_BANK_BASE + 0xB0)
#define HI1106_DFX_REG_BANK_CFG_INTR_CLR_REG                (HI1106_DFX_REG_BANK_BASE + 0xB8)
#define HI1106_PHY_GLB_REG_BANK_FTM_CFG_REG                 (HI1106_PHY_GLB_REG_BANK_BASE + 0x38) /* FTM���üĴ��� */
/* �ŵ�����ͨ�üĴ��� */
#define HI1106_RX_CHN_EST_REG_BANK_CHN_EST_COM_REG          (HI1106_RX_CHN_EST_REG_BANK_BASE + 0x1B0)
#define HI1106_RF_CTRL_REG_BANK_0_CFG_ONLINE_CALI_USE_RF_REG_EN_REG     (HI1106_RF_CTRL_REG_BANK_0_BASE + 0x34)
/* FIFOʹ�ܶ�Ӧ�ļĴ������õ�ַ */
#define HI1106_CALI_TEST_REG_BANK_0_FIFO_FORCE_EN_REG       (HI1106_CALI_TEST_REG_BANK_0_BASE + 0x28)
#define HI1106_PHY_GLB_REG_BANK_PHY_BW_MODE_REG             (HI1106_PHY_GLB_REG_BANK_BASE + 0x4) /* Ƶ��ģʽ���üĴ��� */


#define HI1106_MAC_HOST_INTR_STATUS_REG                     (HI1106_MAC_RPT_HOST_REG_BASE + 0x0)  /* Host���ж�״̬ */
/* BA INFO BUFFER��дָ���ϱ� */
#define HI1106_MAC_BA_INFO_BUF_WPTR_REG                     (HI1106_MAC_RPT_HOST_REG_BASE + 0x4)
#define HI1106_MAC_RX_NORM_DATA_FREE_RING_RPTR_REG          (HI1106_MAC_RPT_HOST_REG_BASE + 0x8)  /* rx-ndata-frrptr */
#define HI1106_MAC_RX_SMALL_DATA_FREE_RING_RPTR_REG         (HI1106_MAC_RPT_HOST_REG_BASE + 0xC)  /* rx-sdata-frrptr */
#define HI1106_MAC_RX_DATA_CMP_RING_WPTR_REG                (HI1106_MAC_RPT_HOST_REG_BASE + 0x10)
#define HI1106_MAC_RPT_HOST_FREE_RING_STATUS_REG            (HI1106_MAC_RPT_HOST_REG_BASE + 0x14) /* F RING�ϱ���Ϣ */
#define HI1106_MAC_RX_PPDU_HOST_DESC_FREE_RING_RPTR_REG     (HI1106_MAC_RPT_HOST_REG_BASE + 0x18)
#define HI1106_MAC_RPT_PPDU_HOST_FREE_RING_STATUS_REG       (HI1106_MAC_RPT_HOST_REG_BASE + 0x1C)
/* TX BA INFO BUFFER������üĴ��� */
#define HI1106_MAC_TX_BA_INFO_BUF_DEPTH_REG                 (HI1106_MAC_CFG_HOST_REG_BASE + 0x0)
/* TX BA INFO BUFFER����ַ���üĴ��� */
#define HI1106_MAC_TX_BA_INFO_BUF_ADDR_LSB_REG              (HI1106_MAC_CFG_HOST_REG_BASE + 0x4)
/* TX BA INFO BUFFER����ַ���üĴ��� */
#define HI1106_MAC_TX_BA_INFO_BUF_ADDR_MSB_REG              (HI1106_MAC_CFG_HOST_REG_BASE + 0x8)
/* TX BA INFO WPTR BUFFER����ַ���üĴ��� */
#define HI1106_MAC_TX_BA_INFO_WPTR_REG                      (HI1106_MAC_CFG_HOST_REG_BASE + 0xC)
/* TX BA INFO RPTR BUFFER����ַ���üĴ��� */
#define HI1106_MAC_TX_BA_INFO_RPTR_REG                      (HI1106_MAC_CFG_HOST_REG_BASE + 0x10)
#define HI1106_MAC_RX_DATA_BUFF_LEN_REG                     (HI1106_MAC_CFG_HOST_REG_BASE + 0x1C)
#define HI1106_MAC_RX_NORM_DATA_FREE_RING_ADDR_LSB_REG      (HI1106_MAC_CFG_HOST_REG_BASE + 0x20)
#define HI1106_MAC_RX_NORM_DATA_FREE_RING_ADDR_MSB_REG      (HI1106_MAC_CFG_HOST_REG_BASE + 0x24)
#define HI1106_MAC_RX_SMALL_DATA_FREE_RING_ADDR_LSB_REG     (HI1106_MAC_CFG_HOST_REG_BASE + 0x28)
#define HI1106_MAC_RX_SMALL_DATA_FREE_RING_ADDR_MSB_REG     (HI1106_MAC_CFG_HOST_REG_BASE + 0x2C)
#define HI1106_MAC_RX_DATA_CMP_RING_ADDR_LSB_REG            (HI1106_MAC_CFG_HOST_REG_BASE + 0x30)
#define HI1106_MAC_RX_DATA_CMP_RING_ADDR_MSB_REG            (HI1106_MAC_CFG_HOST_REG_BASE + 0x34)
#define HI1106_MAC_RX_NORM_DATA_FREE_RING_SIZE_REG          (HI1106_MAC_CFG_HOST_REG_BASE + 0x38)
#define HI1106_MAC_RX_SMALL_DATA_FREE_RING_SIZE_REG         (HI1106_MAC_CFG_HOST_REG_BASE + 0x3C)
#define HI1106_MAC_RX_DATA_CMP_RING_SIZE_REG                (HI1106_MAC_CFG_HOST_REG_BASE + 0x40)
#define HI1106_MAC_RX_NORM_DATA_FREE_RING_WPTR_REG          (HI1106_MAC_CFG_HOST_REG_BASE + 0x44)
#define HI1106_MAC_RX_SMALL_DATA_FREE_RING_WPTR_REG         (HI1106_MAC_CFG_HOST_REG_BASE + 0x48)
#define HI1106_MAC_RX_DATA_CMP_RING_RPTR_REG                (HI1106_MAC_CFG_HOST_REG_BASE + 0x4C)
#define HI1106_MAC_HOST_INTR_CLR_REG                        (HI1106_MAC_CFG_HOST_REG_BASE + 0x50) /* Host�ж���� */
#define HI1106_MAC_HOST_INTR_MASK_REG                       (HI1106_MAC_CFG_HOST_REG_BASE + 0x54) /* Host�ж����� */
#define HI1106_MAC_RX_DATA_CMP_RING_WPTR_CFG_REG            (HI1106_MAC_CFG_HOST_REG_BASE + 0x58)
#define HI1106_MAC_RX_NORM_DATA_FREE_RING_RPTR_CFG_REG      (HI1106_MAC_CFG_HOST_REG_BASE + 0x5C)
#define HI1106_MAC_RX_SMALL_DATA_FREE_RING_RPTR_CFG_REG     (HI1106_MAC_CFG_HOST_REG_BASE + 0x60)
#define HI1106_MAC_RX_FRAMEFILT_REG                         (HI1106_MAC_CFG_DMAC_BANK0_REG_BASE + 0x54)  /* RX���˿��ƼĴ��� */
#define HI1106_MAC_RX_FRAMEFILT2_REG                        (HI1106_MAC_CFG_DMAC_BANK0_REG_BASE + 0x58)  /* RX���˿��ƼĴ���2 */

#define HI1106_MAC_DMAC_INTR_MASK_REG                       (HI1106_MAC_CFG_DMAC_BANK0_REG_BASE + 0x4)   /* DMAC�ж����μĴ��� */
#define HI1106_MAC_RX_CTRL_REG                              (HI1106_MAC_CFG_DMAC_BANK0_REG_BASE + 0x20)  /* ���տ��ƼĴ��� */
#define HI1106_MAC_CSI_PROCESS_REG                          (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x0)   /* CSI�����ϱ����üĴ��� */
#define HI1106_MAC_CSI_BUF_BASE_ADDR_LSB_REG                (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x4)   /* CSI�����ϱ�Ƭ��ѭ��BUF����ַLSB */
#define HI1106_MAC_CSI_BUF_BASE_ADDR_MSB_REG                (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x8)   /* CSI�����ϱ�Ƭ��ѭ��BUF����ַMSB */
#define HI1106_MAC_CSI_BUF_SIZE_REG                         (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0xC)   /* CSI�����ϱ�Ƭ��ѭ��BUF��С */
#define HI1106_MAC_CSI_WHITELIST_ADDR_LSB_0_REG             (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x10)  /* CSI������������32λ */
#define HI1106_MAC_CSI_WHITELIST_ADDR_MSB_0_REG             (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x14)  /* CSI������������16λ */

#define HI1106_MAC_LOCATION_INFO_MASK_REG                   (HI1106_MAC_RPT_DMAC_REG_BASE + 0x70)  /* ��λ��Ϣ�ϱ� */
#define HI1106_MAC_CSI_INFO_ADDR_MSB_REG                    (HI1106_MAC_RPT_DMAC_REG_BASE + 0x1B0) /* CSI�ϱ���ַ��32bit */
#define HI1106_MAC_CSI_INFO_ADDR_LSB_REG                    (HI1106_MAC_RPT_DMAC_REG_BASE + 0x1B4) /* CSI�ϱ���ַ��32bit */
#define HI1106_MAC_FTM_INFO_ADDR_MSB_REG                    (HI1106_MAC_RPT_DMAC_REG_BASE + 0x1B8) /* FTM�ϱ���ַ��32bit */
#define HI1106_MAC_FTM_INFO_ADDR_LSB_REG                    (HI1106_MAC_RPT_DMAC_REG_BASE + 0x1BC) /* FTM�ϱ���ַ��32bit */

/* FTM�����ϱ����üĴ��� */
#define HI1106_MAC_FTM_PROCESS_REG                          (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x110)
/* FTM�����ϱ�Ƭ��ѭ��BUF����ַLSB */
#define HI1106_MAC_FTM_BUF_BASE_ADDR_LSB_REG                (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x114)
/* FTM�����ϱ�Ƭ��ѭ��BUF����ַMSB */
#define HI1106_MAC_FTM_BUF_BASE_ADDR_MSB_REG                (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x118)
/* FTM������������32λ */
#define HI1106_MAC_FTM_WHITELIST_ADDR_LSB_0_REG             (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x12C)
/* FTM������������16λ */
#define HI1106_MAC_FTM_WHITELIST_ADDR_MSB_0_REG             (HI1106_MAC_CFG_DMAC_BANK4_REG_BASE + 0x130)

/* master: 0x4002_0000~0x4003_ffff slave: 0x4004_0000~0x4005_ffff */
#define HI1106_MAC_BANK_REG_OFFSET 0x20000
/* fpga �׶���δ������ƫ�� �����޸����� */
#define HI1106_PHY_BANK_REG_OFFSET 0x40000

#define HI1106_MAC_DMAC_COMMON_TIMER_MASK_REG       (HI1106_MAC_CFG_DMAC_BANK0_REG_BASE + 0x124) /* DMACͨ�ü������ж����μĴ��� */
#define HI1106_MAC_COMMON_TIMER_CTRL_0_REG          (HI1106_MAC_CFG_DMAC_BANK0_REG_BASE + 0x128) /* ͨ�ö�ʱ����ز������� */
#define HI1106_MAC_COMMON_TIMER_VAL_0_REG           (HI1106_MAC_CFG_DMAC_BANK0_REG_BASE + 0x1A8) /* ͨ�ö�ʱ����ʱֵ���� */

#define HI1106_MAC_CFG_SMAC_REG_BASE          (0x4002E000)
#define HI1106_MAC_CFG_DMAC_BANK2_REG_BASE    (0x40026000)
#define HI1106_MAC_HWMAC_STARTUP_REG          (HI1106_MAC_CFG_SMAC_REG_BASE + 0x140) /* HWMAC�������üĴ��� */
#define HI1106_MAC_COEX_ABORT_CTRL_REG        (HI1106_MAC_CFG_DMAC_BANK2_REG_BASE + 0x300) /* ������ƼĴ��� */
#define HI1106_MAC_CFG_COEX_ABORT_EN_MASK      0x1
#define HI1106_MAC_CFG_HWMAC_STARTUP_EN_MASK   0x1
#define HI1106_MAC_COMMON_TIMER_OFFSET         0x4

#define HI1106_MAC_MAX_COMMON_TIMER 31 /* ͨ�ö�ʱ������ */

void  hi1106_host_mac_irq_mask(hal_host_device_stru *hal_device, uint32_t irq);
void  hi1106_host_mac_irq_unmask(hal_host_device_stru *hal_device, uint32_t irq);
void hi1106_clear_host_mac_int_status(hal_host_device_stru *hal_device, uint32_t status);
void hi1106_get_host_mac_int_mask(hal_host_device_stru *hal_device, uint32_t *p_mask);
void hi1106_get_host_mac_int_status(hal_host_device_stru *hal_device, uint32_t *p_status);
uint32_t hi1106_regs_addr_transfer(hal_host_device_stru *hal_device, uint32_t reg_addr);
void hi1106_clear_host_phy_int_status(hal_host_device_stru *hal_device, uint32_t status);
void hi1106_get_host_phy_int_mask(hal_host_device_stru *hal_device, uint32_t *p_mask);
void hi1106_get_host_phy_int_status(hal_host_device_stru *hal_device, uint32_t *p_status);
int32_t hi1106_host_regs_addr_init(hal_host_device_stru *hal_device);
uint32_t hi1106_get_dev_rx_filt_mac_status(uint64_t *reg_status);
uint32_t hi1106_regs_addr_get_offset(uint8_t device_id, uint32_t reg_addr);
#endif
