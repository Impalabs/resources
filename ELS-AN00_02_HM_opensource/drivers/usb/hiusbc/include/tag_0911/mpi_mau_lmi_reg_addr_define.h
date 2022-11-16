// ****************************************************************************** 
// Copyright     :  Copyright (C) 2020, Hisilicon Technologies Co. Ltd.
// File name     :  mpi_mau_lmi_reg_addr_define.h
// Project line  :  Platform And Key Technologies Development
// Department    :  CAD Development Department
// Author        :  xxx
// Version       :  1.0
// Date          :  2018/9/28
// Description   :  The description of xxx project
// Others        :  Generated automatically by nManager V5.1 
// History       :  xxx 2020/09/15 10:14:51 Create file
// ******************************************************************************

#ifndef __MPI_MAU_LMI_REG_ADDR_DEFINE_H__
#define __MPI_MAU_LMI_REG_ADDR_DEFINE_H__

/////////////////////////////////////////////
////    global parameters, begin         ////
/////////////////////////////////////////////
#define    MPI_MAU_LMI_REG_BASE_ADDR                    0x17000

////             offset address for MPI_MAU_LMI_REG registers
#define MPI_MAU_LMI_REG_LMI_AXI_AWLEN_CFG_OFFSET 0x60
#define MPI_MAU_LMI_REG_CSR_LMI_LEN_CTRL_MASK 0xFF
#define MPI_MAU_LMI_REG_CSR_LMI_LEN_CTRL_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_LEN_CTRL_LEN 8
#define MPI_MAU_LMI_REG_CSR_LMI_LEN_CTRL_RESETVAL 0x02

#define MPI_MAU_LMI_REG_LMI_AXI_QOS_MAP_CFG_OFFSET 0x64
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_QOS_1_MASK 0xF0
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_QOS_1_SHIFT 4
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_QOS_1_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_QOS_1_RESETVAL 0x2
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_QOS_0_MASK 0xF
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_QOS_0_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_QOS_0_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_QOS_0_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_AXI_CACHE_CFG_OFFSET 0x68
#define MPI_MAU_LMI_REG_CSR_AXI_BURST_CACHE_MASK 0xF
#define MPI_MAU_LMI_REG_CSR_AXI_BURST_CACHE_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_AXI_BURST_CACHE_LEN 4
#define MPI_MAU_LMI_REG_CSR_AXI_BURST_CACHE_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_AXI_WCH_STATUS_OFFSET 0x6C
#define MPI_MAU_LMI_REG_USBC_AXI_BREADY_MASK 0x20
#define MPI_MAU_LMI_REG_USBC_AXI_BREADY_SHIFT 5
#define MPI_MAU_LMI_REG_USBC_AXI_BREADY_LEN 1
#define MPI_MAU_LMI_REG_USBC_AXI_BREADY_RESETVAL 0x0
#define MPI_MAU_LMI_REG_AXI_USBC_BVALID_MASK 0x10
#define MPI_MAU_LMI_REG_AXI_USBC_BVALID_SHIFT 4
#define MPI_MAU_LMI_REG_AXI_USBC_BVALID_LEN 1
#define MPI_MAU_LMI_REG_AXI_USBC_BVALID_RESETVAL 0x0
#define MPI_MAU_LMI_REG_AXI_USBC_WREADY_MASK 0x8
#define MPI_MAU_LMI_REG_AXI_USBC_WREADY_SHIFT 3
#define MPI_MAU_LMI_REG_AXI_USBC_WREADY_LEN 1
#define MPI_MAU_LMI_REG_AXI_USBC_WREADY_RESETVAL 0x0
#define MPI_MAU_LMI_REG_USBC_AXI_WVALID_MASK 0x4
#define MPI_MAU_LMI_REG_USBC_AXI_WVALID_SHIFT 2
#define MPI_MAU_LMI_REG_USBC_AXI_WVALID_LEN 1
#define MPI_MAU_LMI_REG_USBC_AXI_WVALID_RESETVAL 0x0
#define MPI_MAU_LMI_REG_AXI_USBC_AWREADY_MASK 0x2
#define MPI_MAU_LMI_REG_AXI_USBC_AWREADY_SHIFT 1
#define MPI_MAU_LMI_REG_AXI_USBC_AWREADY_LEN 1
#define MPI_MAU_LMI_REG_AXI_USBC_AWREADY_RESETVAL 0x0
#define MPI_MAU_LMI_REG_USBC_AXI_AWVALID_MASK 0x1
#define MPI_MAU_LMI_REG_USBC_AXI_AWVALID_SHIFT 0
#define MPI_MAU_LMI_REG_USBC_AXI_AWVALID_LEN 1
#define MPI_MAU_LMI_REG_USBC_AXI_AWVALID_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_AXI_RCH_STATUS_OFFSET 0x70
#define MPI_MAU_LMI_REG_USBC_AXI_RREADY_MASK 0x8
#define MPI_MAU_LMI_REG_USBC_AXI_RREADY_SHIFT 3
#define MPI_MAU_LMI_REG_USBC_AXI_RREADY_LEN 1
#define MPI_MAU_LMI_REG_USBC_AXI_RREADY_RESETVAL 0x0
#define MPI_MAU_LMI_REG_AXI_USBC_RVALID_MASK 0x4
#define MPI_MAU_LMI_REG_AXI_USBC_RVALID_SHIFT 2
#define MPI_MAU_LMI_REG_AXI_USBC_RVALID_LEN 1
#define MPI_MAU_LMI_REG_AXI_USBC_RVALID_RESETVAL 0x0
#define MPI_MAU_LMI_REG_AXI_USBC_ARREADY_MASK 0x2
#define MPI_MAU_LMI_REG_AXI_USBC_ARREADY_SHIFT 1
#define MPI_MAU_LMI_REG_AXI_USBC_ARREADY_LEN 1
#define MPI_MAU_LMI_REG_AXI_USBC_ARREADY_RESETVAL 0x0
#define MPI_MAU_LMI_REG_USBC_AXI_ARVALID_MASK 0x1
#define MPI_MAU_LMI_REG_USBC_AXI_ARVALID_SHIFT 0
#define MPI_MAU_LMI_REG_USBC_AXI_ARVALID_LEN 1
#define MPI_MAU_LMI_REG_USBC_AXI_ARVALID_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_AXI_BUS_OSD_CFG_OFFSET 0x74
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_OSDNUM_MASK 0x7F
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_OSDNUM_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_OSDNUM_LEN 7
#define MPI_MAU_LMI_REG_CSR_LMI_AXI_OSDNUM_RESETVAL 0x10

#define MPI_MAU_LMI_REG_LMI_RCH_OSD_CFG_OFFSET 0x78
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_OSDNUM_MASK 0xFFFFFF
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_OSDNUM_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_OSDNUM_LEN 24
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_OSDNUM_RESETVAL 0x00FFFF

#define MPI_MAU_LMI_REG_LMI_WCH_OSD_CFG_OFFSET 0x7C
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_OSDNUM_MASK 0xFFFFFF
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_OSDNUM_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_OSDNUM_LEN 24
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_OSDNUM_RESETVAL 0x00FFFF

#define MPI_MAU_LMI_REG_LMI_RCH_OSD_CNT_OFFSET 0x80
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_OSDCNT_MASK 0xFFFFFFF
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_OSDCNT_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_OSDCNT_LEN 28
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_OSDCNT_RESETVAL 0x0000000

#define MPI_MAU_LMI_REG_LMI_WCH_OSD_CNT_OFFSET 0x84
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_OSDCNT_MASK 0xFFFFFFF
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_OSDCNT_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_OSDCNT_LEN 28
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_OSDCNT_RESETVAL 0x0000000

#define MPI_MAU_LMI_REG_LMI_FCTRL_CFG_OFFSET 0x88
#define MPI_MAU_LMI_REG_CSR_LMI_RD_FLOWCTRL_EN_MASK 0x2
#define MPI_MAU_LMI_REG_CSR_LMI_RD_FLOWCTRL_EN_SHIFT 1
#define MPI_MAU_LMI_REG_CSR_LMI_RD_FLOWCTRL_EN_LEN 1
#define MPI_MAU_LMI_REG_CSR_LMI_RD_FLOWCTRL_EN_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WR_FLOWCTRL_EN_MASK 0x1
#define MPI_MAU_LMI_REG_CSR_LMI_WR_FLOWCTRL_EN_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WR_FLOWCTRL_EN_LEN 1
#define MPI_MAU_LMI_REG_CSR_LMI_WR_FLOWCTRL_EN_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_RCTRL_EN_OFFSET 0x8C
#define MPI_MAU_LMI_REG_CSR_LMI_RDCTRL_EN_MASK 0x1
#define MPI_MAU_LMI_REG_CSR_LMI_RDCTRL_EN_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_RDCTRL_EN_LEN 1
#define MPI_MAU_LMI_REG_CSR_LMI_RDCTRL_EN_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_AXI_DFX_CFG_OFFSET 0x94
#define MPI_MAU_LMI_REG_CSR_LMI_NULL_RCMD_CLR_MASK 0xF0000000
#define MPI_MAU_LMI_REG_CSR_LMI_NULL_RCMD_CLR_SHIFT 28
#define MPI_MAU_LMI_REG_CSR_LMI_NULL_RCMD_CLR_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_NULL_RCMD_CLR_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_NULL_WCMD_CLR_MASK 0xF000000
#define MPI_MAU_LMI_REG_CSR_LMI_NULL_WCMD_CLR_SHIFT 24
#define MPI_MAU_LMI_REG_CSR_LMI_NULL_WCMD_CLR_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_NULL_WCMD_CLR_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_DEC_CLR_R_MASK 0xF00000
#define MPI_MAU_LMI_REG_CSR_LMI_DEC_CLR_R_SHIFT 20
#define MPI_MAU_LMI_REG_CSR_LMI_DEC_CLR_R_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_DEC_CLR_R_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_DEC_CLR_W_MASK 0xF0000
#define MPI_MAU_LMI_REG_CSR_LMI_DEC_CLR_W_SHIFT 16
#define MPI_MAU_LMI_REG_CSR_LMI_DEC_CLR_W_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_DEC_CLR_W_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_SLV_CLR_R_MASK 0xF000
#define MPI_MAU_LMI_REG_CSR_LMI_SLV_CLR_R_SHIFT 12
#define MPI_MAU_LMI_REG_CSR_LMI_SLV_CLR_R_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_SLV_CLR_R_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_SLV_CLR_W_MASK 0xF00
#define MPI_MAU_LMI_REG_CSR_LMI_SLV_CLR_W_SHIFT 8
#define MPI_MAU_LMI_REG_CSR_LMI_SLV_CLR_W_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_SLV_CLR_W_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_CFG_TO_CLR_R_MASK 0xF0
#define MPI_MAU_LMI_REG_CSR_LMI_CFG_TO_CLR_R_SHIFT 4
#define MPI_MAU_LMI_REG_CSR_LMI_CFG_TO_CLR_R_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_CFG_TO_CLR_R_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_CFG_TO_CLR_W_MASK 0xF
#define MPI_MAU_LMI_REG_CSR_LMI_CFG_TO_CLR_W_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_CFG_TO_CLR_W_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_CFG_TO_CLR_W_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_AXI_ERR_INT_OFFSET 0x98
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_MASK 0xF0000000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_SHIFT 28
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_MASK 0xF000000
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_SHIFT 24
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_MASK 0xF00000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_SHIFT 20
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_MASK 0xF0000
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_SHIFT 16
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_MASK 0xF000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_SHIFT 12
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_MASK 0xF00
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_SHIFT 8
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_MASK 0xF0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_SHIFT 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_MASK 0xF
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_AXI_ERR_INT_EN_OFFSET 0x9C
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_INT_EN_MASK 0xF0000000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_INT_EN_SHIFT 28
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_INT_EN_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_INT_EN_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_INT_EN_MASK 0xF000000
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_INT_EN_SHIFT 24
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_INT_EN_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_INT_EN_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_INT_EN_MASK 0xF00000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_INT_EN_SHIFT 20
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_INT_EN_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_INT_EN_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_INT_EN_MASK 0xF0000
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_INT_EN_SHIFT 16
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_INT_EN_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_INT_EN_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_INT_EN_MASK 0xF000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_INT_EN_SHIFT 12
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_INT_EN_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_INT_EN_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_INT_EN_MASK 0xF00
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_INT_EN_SHIFT 8
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_INT_EN_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_INT_EN_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_INT_EN_MASK 0xF0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_INT_EN_SHIFT 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_INT_EN_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_INT_EN_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_INT_EN_MASK 0xF
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_INT_EN_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_INT_EN_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_INT_EN_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_AXI_ERR_INT_SET_OFFSET 0xA0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_INT_SET_MASK 0xF0000000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_INT_SET_SHIFT 28
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_INT_SET_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_TIMEOUT_INT_SET_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_INT_SET_MASK 0xF000000
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_INT_SET_SHIFT 24
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_INT_SET_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_TIMEOUT_INT_SET_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_INT_SET_MASK 0xF00000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_INT_SET_SHIFT 20
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_INT_SET_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_NULL_CMD_INT_SET_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_INT_SET_MASK 0xF0000
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_INT_SET_SHIFT 16
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_INT_SET_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_NULL_CMD_INT_SET_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_INT_SET_MASK 0xF000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_INT_SET_SHIFT 12
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_INT_SET_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_DECERR_INT_SET_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_INT_SET_MASK 0xF00
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_INT_SET_SHIFT 8
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_INT_SET_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_DECERR_INT_SET_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_INT_SET_MASK 0xF0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_INT_SET_SHIFT 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_INT_SET_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_SLVERR_INT_SET_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_INT_SET_MASK 0xF
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_INT_SET_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_INT_SET_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_SLVERR_INT_SET_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_DEBUG_STATE_OFFSET 0xA4
#define MPI_MAU_LMI_REG_LMI_CSR_CH_STA_MASK 0xFFFF00
#define MPI_MAU_LMI_REG_LMI_CSR_CH_STA_SHIFT 8
#define MPI_MAU_LMI_REG_LMI_CSR_CH_STA_LEN 16
#define MPI_MAU_LMI_REG_LMI_CSR_CH_STA_RESETVAL 0x0000
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_STATE_MASK 0xF0
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_STATE_SHIFT 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_STATE_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_RCH_STATE_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_STATE_MASK 0xF
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_STATE_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_STATE_LEN 4
#define MPI_MAU_LMI_REG_CSR_LMI_WCH_STATE_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_PRO_CFG_OFFSET 0xA8
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_ID_W_MASK 0x3E000
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_ID_W_SHIFT 13
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_ID_W_LEN 5
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_ID_W_RESETVAL 0x00
#define MPI_MAU_LMI_REG_CSR_LMI_WPRO_CLR_MASK 0x1000
#define MPI_MAU_LMI_REG_CSR_LMI_WPRO_CLR_SHIFT 12
#define MPI_MAU_LMI_REG_CSR_LMI_WPRO_CLR_LEN 1
#define MPI_MAU_LMI_REG_CSR_LMI_WPRO_CLR_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_ID_R_MASK 0x3E0
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_ID_R_SHIFT 5
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_ID_R_LEN 5
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_ID_R_RESETVAL 0x00
#define MPI_MAU_LMI_REG_CSR_LMI_RPRO_CLR_MASK 0x10
#define MPI_MAU_LMI_REG_CSR_LMI_RPRO_CLR_SHIFT 4
#define MPI_MAU_LMI_REG_CSR_LMI_RPRO_CLR_LEN 1
#define MPI_MAU_LMI_REG_CSR_LMI_RPRO_CLR_RESETVAL 0x0
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_WIN_MASK 0x1
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_WIN_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_WIN_LEN 1
#define MPI_MAU_LMI_REG_CSR_LMI_PRO_WIN_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_WPRO_DBG_OFFSET 0xAC
#define MPI_MAU_LMI_REG_CSR_LMI_WPRO_MASK 0xFFFFFFFF
#define MPI_MAU_LMI_REG_CSR_LMI_WPRO_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WPRO_LEN 32
#define MPI_MAU_LMI_REG_CSR_LMI_WPRO_RESETVAL 0x00000000

#define MPI_MAU_LMI_REG_LMI_RPRO_DBG_OFFSET 0xB0
#define MPI_MAU_LMI_REG_CSR_LMI_RPRO_MASK 0xFFFFFFFF
#define MPI_MAU_LMI_REG_CSR_LMI_RPRO_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_RPRO_LEN 32
#define MPI_MAU_LMI_REG_CSR_LMI_RPRO_RESETVAL 0x00000000

#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_EN_OFFSET 0x100
#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_EN_MASK 0x1
#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_EN_SHIFT 0
#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_EN_LEN 1
#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_EN_RESETVAL 0x0

#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_OFFSET 0x104
#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_MASK 0x1
#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_SHIFT 0
#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_LEN 1
#define MPI_MAU_LMI_REG_MPI_MAU_AMD_TOP_INT_RESETVAL 0x0

#define MPI_MAU_LMI_REG_LMI_TIMEOUT_WVAL_CFG_OFFSET 0x110
#define MPI_MAU_LMI_REG_CSR_LMI_WCMD_VAL_MASK 0xFFFFF
#define MPI_MAU_LMI_REG_CSR_LMI_WCMD_VAL_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_WCMD_VAL_LEN 20
#define MPI_MAU_LMI_REG_CSR_LMI_WCMD_VAL_RESETVAL 0xFFFFF

#define MPI_MAU_LMI_REG_LMI_TIMEOUT_RVAL_CFG_OFFSET 0x114
#define MPI_MAU_LMI_REG_CSR_LMI_RCMD_VAL_MASK 0xFFFFF
#define MPI_MAU_LMI_REG_CSR_LMI_RCMD_VAL_SHIFT 0
#define MPI_MAU_LMI_REG_CSR_LMI_RCMD_VAL_LEN 20
#define MPI_MAU_LMI_REG_CSR_LMI_RCMD_VAL_RESETVAL 0xFFFFF

#define MPI_MAU_LMI_REG_MPI_RDY_NUM_OFFSET 0x1FC
#define MPI_MAU_LMI_REG_RDY_LAST_NUM_MASK 0xFF0000
#define MPI_MAU_LMI_REG_RDY_LAST_NUM_SHIFT 16
#define MPI_MAU_LMI_REG_RDY_LAST_NUM_LEN 8
#define MPI_MAU_LMI_REG_RDY_LAST_NUM_RESETVAL 0x00
#define MPI_MAU_LMI_REG_RDY_DELAY_NUM_MASK 0xFF
#define MPI_MAU_LMI_REG_RDY_DELAY_NUM_SHIFT 0
#define MPI_MAU_LMI_REG_RDY_DELAY_NUM_LEN 8
#define MPI_MAU_LMI_REG_RDY_DELAY_NUM_RESETVAL 0x04


#endif // __MPI_MAU_LMI_REG_ADDR_DEFINE_H__