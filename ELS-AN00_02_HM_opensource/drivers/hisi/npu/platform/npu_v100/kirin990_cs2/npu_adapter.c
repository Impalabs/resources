/*
 * npu_adapter.c
 *
 * about npu adapter
 *
 * Copyright (c) 2012-2020 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#include "npu_adapter.h"

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/dma-direction.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include <linux/hisi/hisi_svm.h>

#include "npu_platform_resource.h"
#include "npu_platform_register.h"
#include "npu_common.h" /* for NPU_SEC */
#include "npu_pm_framework.h"
#include "npu_platform.h"
#include "npu_svm.h"
#include "dts/npu_reg.h"

tmp_log_buf_header_t *g_ts_buf_header_addr = NULL;
tmp_log_buf_header_t *g_aicpu_buf_header_addr = NULL;
struct task_struct *g_log_fetch_thread = NULL;

u32 g_tmp_log_switch = 0;

int npu_plat_powerup_smmu(struct device *dev)
{
	int ret;

	ret = hisi_smmu_poweron(0); /* 0: sysdma smmu */
	if (ret != 0) {
		npu_drv_err("hisi_smmu_poweron failed\n");
		return ret;
	}
	if (npu_plat_aicore_get_disable_status(0) == 0) {
		ret = hisi_smmu_poweron(1); /* 1: aicore0 smmu */
		if (ret != 0) {
			npu_drv_err("hisi_smmu_poweron failed\n");
			goto aicore0_failed;
		}
	}
	if (npu_plat_aicore_get_disable_status(1) == 0) {
		ret = hisi_smmu_poweron(2); /* 2: aicore1 smmu */
		if (ret != 0) {
			npu_drv_err("hisi_smmu_poweron failed\n");
			goto aicore1_failed;
		}
	}
	return 0;
aicore1_failed:
	if (npu_plat_aicore_get_disable_status(1) == 0)
		(void)hisi_smmu_poweroff(1);
aicore0_failed:
	if (npu_plat_aicore_get_disable_status(0) == 0)
		(void)hisi_smmu_poweroff(0);
	return ret;
}
int npu_plat_svm_bind(struct npu_dev_ctx *dev_ctx,
	struct task_struct *task, void **svm_dev)
{
	*svm_dev = (void*)hisi_svm_bind_task(dev_ctx->npu_dev, task);
	if (*svm_dev == NULL) {
		npu_drv_err("hisi_svm_bind_task failed \n");
		/* likely bound by other process */
		return -EBUSY;
	}
	return 0;
}

void npu_plat_pm_ctrl_core(struct npu_dev_ctx *dev_ctx)
{
	int ret;

	if (dev_ctx == NULL) {
		npu_drv_err("invalid para\n");
		return;
	}

	if ((dev_ctx->ctrl_core_num == 0) ||
		(dev_ctx->ctrl_core_num >= NPU_PLAT_AICORE_MAX))
		return;

	ret = npu_plat_send_ts_ctrl_core(dev_ctx->ctrl_core_num);
	if (ret) {
		npu_drv_err("send ts ipc fail ret %d\n", ret);
		return;
	}
}

int npu_plat_pm_powerup(struct npu_dev_ctx *dev_ctx, u32 work_mode)
{
	u32 *stage = NULL;
	int ret = 0;

	npu_drv_boot_time_tag("start npu_plat_powerup_till_npucpu \n");
	cond_return_error(dev_ctx == NULL, -EINVAL, "invalid para\n");

	stage = &dev_ctx->power_stage;

	if (*stage == NPU_PM_DOWN) {
		ret = npu_plat_powerup_till_npucpu(work_mode);
		cond_goto_error(ret != 0, failed, ret, ret,
			"npu_plat_powerup_till_npucpu failed ,ret=%d\n", ret);
		*stage = NPU_PM_NPUCPU;
	}

	npu_drv_boot_time_tag("start npu_plat_powerup_till_ts \n");

	if (*stage == NPU_PM_NPUCPU) {
		ret = npu_plat_powerup_till_ts(work_mode, NPU_SC_TESTREG0_OFFSET);
		cond_goto_error(ret != 0, ts_failed, ret, ret,
			"npu_plat_powerup_till_ts failed ret=%d\n", ret);
		*stage = NPU_PM_TS;
	}
	npu_drv_boot_time_tag("start npu_plat_powerup_smmu \n");
	if (work_mode != NPU_SEC)
		npu_plat_pm_ctrl_core(dev_ctx);
	if (*stage == NPU_PM_TS) {
		// power up smmu in non_secure npu mode
		if (work_mode != NPU_SEC) {
			ret = npu_plat_powerup_smmu(dev_ctx->npu_dev);
			cond_goto_error(ret != 0, smmu_failed, ret, ret,
				"npu_plat_powerup_smmu failed ret=%d\n", ret);
		} else {
			npu_drv_warn("secure npu power up ,no need to power up smmu"
				"in linux non_secure world, smmu power up will"
				"be excuted on tee secure world \n");
		}
	}
	*stage = NPU_PM_UP;
	npu_plat_set_npu_power_status(DRV_NPU_POWER_ON_FLAG);
	npu_drv_warn("npu_plat_pm_powerup success\n");
	return 0;

smmu_failed:
	(void)npu_plat_powerdown_ts(NPU_SC_TESTREG8_OFFSET, work_mode);
	// continue even if gic grace exit failed
	*stage = NPU_PM_NPUCPU;
ts_failed:
	if (npu_plat_powerdown_npucpu(0x1 << 5, work_mode) != 0)
		return ret;
	*stage = NPU_PM_DOWN;
failed:
	return ret;
}

int npu_plat_pm_open(uint32_t devid)
{
	if (npu_plat_ioremap(NPU_REG_POWER_STATUS) != 0) {
		npu_drv_err("npu_plat_ioremap failed\n");
		return -1;
	}
	return 0;
}

int npu_plat_pm_release(uint32_t devid)
{
	int ret = npu_clear_pid_ssid_table(devid, 1);
	npu_plat_iounmap(NPU_REG_POWER_STATUS);
	if (ret != 0) {
		npu_drv_err("npu_clear_pid_ssid_table failed\n");
		return ret;
	}
	return 0;
}

int npu_plat_res_mailbox_send(void *mailbox, int mailbox_len,
	const void *message, int message_len)
{
	u8 *message_buf = NULL;

	if (message_len > mailbox_len) {
		npu_drv_err("message len =%d, too long", message_len);
		return -1;
	}

	message_buf = vmalloc(mailbox_len);
	if (message_buf == NULL) {
		npu_drv_err("message buf alloc failed");
		return -1;
	}
	memset(message_buf, 0, mailbox_len);
	memcpy(message_buf, message, message_len);
	memcpy(mailbox, message_buf, mailbox_len);
	mb();
	vfree(message_buf);
	return 0;
}

void __iomem *npu_plat_sram_remap(struct platform_device *pdev,
	resource_size_t sram_addr, resource_size_t sram_size)
{
	return devm_ioremap_nocache(&pdev->dev, sram_addr, sram_size);
}

void npu_plat_sram_unmap(struct platform_device *pdev, void *sram_addr)
{
	devm_iounmap(&pdev->dev, (void __iomem*)sram_addr);
	return;
}

int npu_plat_poweroff_smmu(uint32_t devid)
{
	npu_clear_pid_ssid_table(devid, 0);
	(void)hisi_smmu_poweroff(0); /* 0: sysdma smmu */
	if (npu_plat_aicore_get_disable_status(0) == 0)
		(void)hisi_smmu_poweroff(1); /* 1: aicore0 smmu */
	if (npu_plat_aicore_get_disable_status(1) == 0)
		(void)hisi_smmu_poweroff(2); /* 2: aicore1 smmu */
	return 0;
}

int npu_plat_pm_powerdown(uint32_t devid, u32 is_secure, u32 *stage)
{
	int ret = 0;

	npu_plat_set_npu_power_status(DRV_NPU_POWER_OFF_FLAG);
	if (*stage == NPU_PM_UP) {
		if (is_secure != NPU_SEC) {
			ret = npu_plat_poweroff_smmu(devid);
			if (ret != 0)
				npu_drv_err("npu_plat_poweroff_smmu FAILED \n");
		} else {
			npu_drv_warn("secure npu power down ,no need to power down smmu"
				"in linux non_secure world, smmu power down has"
				"been excuted on tee secure world \n");
		}
		*stage = NPU_PM_TS;
	}

	if (*stage == NPU_PM_TS) {
		ret = npu_plat_powerdown_ts(NPU_SC_TESTREG8_OFFSET, is_secure);
		// continue even if gic grace exit failed
		*stage = NPU_PM_NPUCPU;
	}
	if (*stage == NPU_PM_NPUCPU) {
		ret = npu_plat_powerdown_npucpu(0x1 << 5, is_secure);
		if (ret != 0)
			npu_drv_err("npu_plat_pm_powerdown FAILED \n");
		ret = npu_plat_powerdown_nputop();
		if (ret != 0)
			npu_drv_err("npu_plat_powerdown_nputop FAILED \n");
	}
	*stage = NPU_PM_DOWN;
	npu_drv_warn("npu_plat_pm_powerdown success\n");
	return ret;
}

int npu_plat_res_ctrl_core(struct npu_dev_ctx *dev_ctx, u32 core_num)
{
	if (dev_ctx == NULL) {
		npu_drv_err("invalid para\n");
		return -EINVAL;
	}

	if ((atomic_read(&dev_ctx->power_access) == 0) &&
		(bitmap_get(dev_ctx->pm.work_mode, NPU_NONSEC))) {
		// npu is power on
		npu_drv_info("npu is power on, send ipc to ts\n");
		npu_plat_send_ts_ctrl_core(core_num);
	}
	// update dev_ctx data
	dev_ctx->ctrl_core_num = core_num;

	return 0;
}

int npu_testreg_poll_wait(u64 offset)
{
	unused(offset);
	return -1;
}

int npu_testreg_clr(u64 offset)
{
	unused(offset);
	return 0;
}

int npu_plat_handle_irq_tophalf(u32 irq_index)
{
	unused(irq_index);
	return 0;
}

int npu_plat_attach_sc(int fd, u64 offset, u64 size)
{
	unused(fd);
	unused(offset);
	unused(size);
	return 0;
}

int npu_plat_set_sc_prio(u32 prio)
{
	unused(prio);
	return 0;
}

int npu_plat_switch_sc(u32 switch_sc)
{
	unused(switch_sc);
	return 0;
}

int npu_smmu_evt_register_notify(struct notifier_block *n)
{
	return hisi_smmu_evt_register_notify(n);
}

int npu_smmu_evt_unregister_notify(struct notifier_block *n)
{
	return hisi_smmu_evt_unregister_notify(n);
}

int npu_plat_switch_hwts_aicore_pool(struct npu_dev_ctx *dev_ctx,
	struct npu_work_mode_info *work_mode_info, uint32_t power_status)
{
	unused(dev_ctx);
	unused(work_mode_info);
	unused(power_status);
	return 0;
}

void npu_plat_aicore_pmu_enable(uint32_t subip_set)
{
	unused(subip_set);
	return;
}

void npu_plat_mntn_reset(void)
{
	return;
}

int npu_plat_dev_pm_suspend(void)
{
	return 0;
}

void npu_exception_timeout_record(struct npu_dev_ctx *dev_ctx)
{
	(void)(dev_ctx);
	return;
}

int npu_plat_bypass_status(void)
{
	return NPU_NON_BYPASS;
}
