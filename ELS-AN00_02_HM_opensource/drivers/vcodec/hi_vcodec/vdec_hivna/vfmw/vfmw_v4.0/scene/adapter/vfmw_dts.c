/*
 * vfmw_dts.c
 *
 * This is vfmw handle dts config interface.
 *
 * Copyright (c) 2017-2020 Huawei Technologies CO., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "vfmw_dts.h"
#include "sysconfig.h"
#include "public.h"

UINT32  g_is_fpga;
UINT32  g_vdh_reg_base_addr;
UINT32  g_scd_reg_base_addr;//lint !e552
UINT32  g_vdh_reg_range;
UINT32  g_soft_rst_req_addr;//lint !e552
UINT32  g_soft_rst_ok_addr;//lint !e552
UINT64  g_smmu_page_base;
UINT32  g_pericrg_reg_base_addr;
UINT32  g_vdec_qos_mode;//lint !e552

/* irq num */
UINT32  g_vdec_irq_num_norm;
UINT32  g_vdec_irq_num_prot;
UINT32  g_vdec_irq_num_safe;

SINT32 vfmw_set_dts_config(vfmw_dts_config_s *dts_config)
{
	if (dts_config == NULL) {
		dprint(PRN_ERROR, "%s : dts_config is NULL\n", __func__);
		return VDEC_ERR;
	}

#ifndef HISMMUV300
	if (dts_config->smmu_page_base_addr == 0) {
		dprint(PRN_ERROR, "smmu page base addr is 0\n");
		return VDEC_ERR;
	}
#endif
	if (dts_config->vdec_irq_num_norm == 0 ||
		dts_config->vdec_irq_num_prot == 0 ||
		dts_config->vdec_irq_num_safe == 0 ||
		dts_config->vdh_reg_base_addr == 0 ||
		dts_config->vdh_reg_range == 0 ||
		dts_config->pericrg_reg_base_addr == 0) {
		dprint(PRN_ERROR, "%s invalid param: is_fpga : %d, vdec_irq_num_norm : %d\n", __func__, dts_config->is_fpga,
			dts_config->vdec_irq_num_norm);
		dprint(PRN_ERROR, "vdec_irq_num_prot : %d, vdec_irq_num_safe : %d, vdh_reg_base_addr : %pK\n",
			dts_config->vdec_irq_num_prot, dts_config->vdec_irq_num_safe,
			(void *)(uintptr_t)(dts_config->vdh_reg_base_addr));
		dprint(PRN_ERROR, "VdhRegSize : %d, pericrg_reg_base_addr : %pK\n",
			dts_config->vdh_reg_range,
			(void *)(uintptr_t)(dts_config->pericrg_reg_base_addr));
		return VDEC_ERR;
	}

	g_is_fpga                = dts_config->is_fpga;
	g_vdec_irq_num_norm      = dts_config->vdec_irq_num_norm;
	g_vdec_irq_num_prot      = dts_config->vdec_irq_num_prot;
	g_vdec_irq_num_safe      = dts_config->vdec_irq_num_safe;

	g_vdh_reg_base_addr      = dts_config->vdh_reg_base_addr;
	g_vdh_reg_range          = dts_config->vdh_reg_range;
	g_smmu_page_base         = dts_config->smmu_page_base_addr;
	g_pericrg_reg_base_addr  = dts_config->pericrg_reg_base_addr;
	g_vdec_qos_mode          = dts_config->vdec_qos_mode;

	g_scd_reg_base_addr      = g_vdh_reg_base_addr + SCD_REG_OFFSET;
	g_soft_rst_req_addr      = g_vdh_reg_base_addr + SOFTRST_REQ_OFFSET;
	g_soft_rst_ok_addr       = g_vdh_reg_base_addr + SOFTRST_OK_OFFSET;

	return VDEC_OK;
}

SINT32 vfmw_get_dts_config(vfmw_dts_config_s *dts_config)
{
	if (dts_config == NULL) {
		dprint(PRN_ERROR, "%s FATAL: dts_config is NULL\n", __func__);
		return VDEC_ERR;
	}

	dts_config->is_fpga               = g_is_fpga;
	dts_config->vdec_irq_num_norm     = g_vdec_irq_num_norm;
	dts_config->vdec_irq_num_prot     = g_vdec_irq_num_prot;
	dts_config->vdec_irq_num_safe     = g_vdec_irq_num_safe;

	dts_config->vdh_reg_base_addr     = g_vdh_reg_base_addr;
	dts_config->vdh_reg_range         = g_vdh_reg_range;
	dts_config->smmu_page_base_addr   = g_smmu_page_base;

	dts_config->pericrg_reg_base_addr = g_pericrg_reg_base_addr;

	return VDEC_OK;
}
#ifdef ENV_ARMLINUX_KERNEL
EXPORT_SYMBOL(vfmw_set_dts_config);//lint !e580
#endif
