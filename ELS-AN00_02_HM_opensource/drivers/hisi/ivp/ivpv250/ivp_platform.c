/*
 * This file implements ivp initialization and control functions
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "ivp_platform.h"
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/dma-mapping.h>
#include "ivp_log.h"
#include "ivp_reg.h"
#include "ivp_rdr.h"
#include "ivp_manager.h"
#include "ivp_atf.h"
#include "ivp_sec.h"

static u32 noc_ivp_reg_read(const struct ivp_device *ivp_devp, unsigned int off)
{
	char __iomem *reg = ivp_devp->io_res.noc_ivp_base_addr + off;
	return readl(reg);
}

static void noc_ivp_reg_write(const struct ivp_device *ivp_devp,
	unsigned int off, u32 val)
{
	char __iomem *reg = ivp_devp->io_res.noc_ivp_base_addr + off;
	writel(val, reg);
}

int ivp_get_memory_section(struct platform_device *plat_dev,
	struct ivp_device *ivp_devp)
{
	int i;
	int ret;
	unsigned int size = 0;
	dma_addr_t dma_addr = 0;
	struct ivp_common *ivp_comm = NULL;

	loge_and_return_if_cond(-EINVAL, (!ivp_devp || !plat_dev),
		"invalid input param ivp_devp or plat_dev");

	ivp_comm = &ivp_devp->ivp_comm;
	ivp_comm->vaddr_memory = NULL;
	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_DYNAMIC_MEM, &size);
	if (ret || !size) {
		ivp_err("failed get dynamic mem, ret:%d size:%u", ret, size);
		return -EINVAL;
	}
	ivp_comm->dynamic_mem_size = size;
	ivp_comm->ivp_meminddr_len = (int)size;

	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_DYNAMIC_MEM_SEC_SIZE, &size);
	if (ret || !size) {
		ivp_err("failed to get dynamic section, ret:%d size:%u", ret, size);
		return -EINVAL;
	}
	ivp_comm->dynamic_mem_section_size = size;
	if ((ivp_comm->dynamic_mem_section_size *
		(ivp_comm->sect_count - DDR_SECTION_INDEX)) !=
		ivp_comm->dynamic_mem_size) {
		ivp_err("dynamic mem size or section size is error");
		return -EINVAL;
	}

	/*lint -save -e598 -e648*/
	dma_set_mask_and_coherent(&ivp_comm->ivp_pdev->dev, DMA_BIT_MASK(DMA_64BIT));
	/*lint -restore*/
	ivp_comm->vaddr_memory = dma_alloc_coherent(&ivp_comm->ivp_pdev->dev,
		ivp_comm->dynamic_mem_size, &dma_addr, GFP_KERNEL);
	if (!ivp_comm->vaddr_memory) {
		ivp_err("get vaddr_memory failed");
		return -ENOMEM;
	}

	for (i = DDR_SECTION_INDEX; i < ivp_comm->sect_count; i++) {
		if (i == DDR_SECTION_INDEX) {
			ivp_comm->sects[i].acpu_addr = dma_addr >> IVP_MMAP_SHIFT;
		} else {
			ivp_comm->sects[i].acpu_addr =
				((ivp_comm->sects[i - 1].acpu_addr << IVP_MMAP_SHIFT) +
				ivp_comm->sects[i - 1].len) >> IVP_MMAP_SHIFT;
			ivp_comm->sects[i].ivp_addr = ivp_comm->sects[i - 1].ivp_addr +
				ivp_comm->sects[i - 1].len;
		}
		ivp_comm->sects[i].len = ivp_comm->dynamic_mem_section_size;
		ivp_dbg("ivp sections 0x%lx", ivp_comm->sects[i].acpu_addr);
	}

	return ret;
}

void ivp_free_memory_section(struct ivp_device *ivp_devp)
{
	dma_addr_t dma_addr;
	struct ivp_common *ivp_comm = NULL;

	if (!ivp_devp) {
		ivp_err("invalid input param ivp_devp");
		return;
	}

	ivp_comm = &ivp_devp->ivp_comm;
	dma_addr =
		ivp_comm->sects[DDR_SECTION_INDEX].acpu_addr << IVP_MMAP_SHIFT;
	if (ivp_comm->vaddr_memory) {
		dma_free_coherent(&ivp_comm->ivp_pdev->dev,
			ivp_comm->dynamic_mem_size, ivp_comm->vaddr_memory, dma_addr);
		ivp_comm->vaddr_memory = NULL;
	}
}

static inline void ivp_hw_remap_ivp2ddr(struct ivp_device *ivp_devp,
	unsigned int ivp_addr, unsigned int len, unsigned long ddr_addr)
{
	unsigned int ivp_addr_base = ((ivp_addr / SIZE_1MB) <<
		IVP_REMP_ADDR_SHIFT) + (ivp_addr / SIZE_1MB);
	unsigned int ddr_addr_base = (u32)(((ddr_addr / SIZE_1MB) <<
		IVP_REMP_ADDR_SHIFT) + (ddr_addr / SIZE_1MB)); /* reg is 32bit */
	unsigned int remap_len = (len << IVP_REMP_LEN_SHIFT) + len;

	ivp_info("ivp remap ddr Enter");
	ivp_reg_write(ivp_devp, ADDR_IVP_CFG_SEC_REG_START_REMAP_ADDR, ivp_addr_base);
	ivp_reg_write(ivp_devp, ADDR_IVP_CFG_SEC_REG_REMAP_LENGTH, remap_len);
	ivp_reg_write(ivp_devp, ADDR_IVP_CFG_SEC_REG_DDR_REMAP_ADDR, ddr_addr_base);
}

int ivp_remap_addr_ivp2ddr(struct ivp_device *ivp_devp,
	unsigned int ivp_addr, int len, unsigned long ddr_addr)
{
	loge_and_return_if_cond(-EINVAL, !ivp_devp, "invalid input param ivp_devp");

	ivp_dbg("ivp_addr:0x%x, len:%d, ddr_addr:0x%lx",
		ivp_addr, len, ddr_addr);
	if ((ivp_addr & MASK_1MB) != 0 ||
		(ddr_addr & MASK_1MB) != 0 ||
		len >= MAX_DDR_LEN * SIZE_1MB) {
		ivp_err("not aligned");
		return -EINVAL;
	}
	len = (len + SIZE_1MB - 1) / SIZE_1MB - 1;
	ivp_hw_remap_ivp2ddr(ivp_devp, ivp_addr, (u32)len, ddr_addr);

	return 0;
}

int ivp_get_secure_attribute(struct platform_device *plat_dev,
	struct ivp_device *ivp_devp)
{
	int ret;
	unsigned int sec_support = 0;

	loge_and_return_if_cond(-EINVAL, (!ivp_devp || !plat_dev),
			"invalid input param ivp_devp or plat_dev");

	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_SEC_SUPPORT, &sec_support);
	if (ret) {
		ivp_err("get ivp sec support flag fail, ret:%d", ret);
		return -EINVAL;
	}
	ivp_devp->ivp_comm.ivp_sec_support = sec_support;
	ivp_info("get ivp sec support flag :%u", sec_support);

	return ret;
}

static void ivp_secure_config(void)
{
	ivpatf_change_slv_secmod(IVP_CORE0_ID, IVP_SEC);
	ivpatf_change_mst_secmod(IVP_CORE0_ID, IVP_SEC);
}

static int ivp_read_qos_cfg(struct ivp_device *ivp_devp)
{
	int ret = 0;

	if (ivp_reg_read(ivp_devp, IVP_REG_OFF_IVP_SYSTEM_QOS_CFG) !=
		IVP_SYS_QOS_CFG_VALUE) {
		ivp_warn("IVP_REG_OFF_IVP_SYSTEM_QOS_CFG read back value:0x%x",
			ivp_reg_read(ivp_devp, IVP_REG_OFF_IVP_SYSTEM_QOS_CFG));
		ret += READ_BACK_IVP_SYS_QOS_CFG_ERROR;
	}

	if (noc_ivp_reg_read(ivp_devp, IVP_CORE_RD_QOS_MODE) !=
		NOC_IVP_QOS_CFG_VALUE) {
		ivp_warn("IVP_CORE_RD_QOS_MODE read back value:0x%x",
			noc_ivp_reg_read(ivp_devp, IVP_CORE_RD_QOS_MODE));
		ret += READ_BACK_IVP_CORE_RD_QOS_MODE_ERROR;
	}

	if (noc_ivp_reg_read(ivp_devp, IVP_CORE_WR_QOS_MODE) !=
		NOC_IVP_QOS_CFG_VALUE) {
		ivp_warn("IVP_CORE_WR_QOS_MODE read back value:0x%x",
			noc_ivp_reg_read(ivp_devp, IVP_CORE_WR_QOS_MODE));
		ret += READ_BACK_IVP_CORE_WR_QOS_MODE_ERROR;
	}

	if (noc_ivp_reg_read(ivp_devp, IVP_IDMA_RD_QOS_MODE) !=
		NOC_IVP_QOS_CFG_VALUE) {
		ivp_warn("IVP_IDMA_RD_QOS_MODE read back value:0x%x",
			noc_ivp_reg_read(ivp_devp, IVP_IDMA_RD_QOS_MODE));
		ret += READ_BACK_IVP_IDMA_RD_QOS_MODE_ERROR;
	}

	if (noc_ivp_reg_read(ivp_devp, IVP_IDMA_WR_QOS_MODE) !=
		NOC_IVP_QOS_CFG_VALUE) {
		ivp_warn("IVP_IDMA_WR_QOS_MODE read back value:0x%x",
			noc_ivp_reg_read(ivp_devp, IVP_IDMA_WR_QOS_MODE));
		ret += READ_BACK_IVP_IDMA_WR_QOS_MODE_ERROR;
	}

	return ret;
}

static void ivp_set_qos_cfg(struct ivp_device *ivp_devp)
{
	ivp_reg_write(ivp_devp, IVP_REG_OFF_IVP_SYSTEM_QOS_CFG,
		IVP_SYS_QOS_CFG_VALUE);
	noc_ivp_reg_write(ivp_devp, IVP_CORE_RD_QOS_MODE, NOC_IVP_QOS_CFG_VALUE);
	noc_ivp_reg_write(ivp_devp, IVP_CORE_WR_QOS_MODE, NOC_IVP_QOS_CFG_VALUE);
	noc_ivp_reg_write(ivp_devp, IVP_IDMA_RD_QOS_MODE, NOC_IVP_QOS_CFG_VALUE);
	noc_ivp_reg_write(ivp_devp, IVP_IDMA_WR_QOS_MODE, NOC_IVP_QOS_CFG_VALUE);
	if (ivp_read_qos_cfg(ivp_devp) != 0)
		ivp_err("ivp_read_qos_cfg failed, may get a perf problem");
}

int ivp_poweron_pri(struct ivp_device *ivp_devp)
{
	int ret;
	struct ivp_common *ivp_comm = NULL;

	loge_and_return_if_cond(-EINVAL, !ivp_devp, "invalid input param ivp_devp");

	ivp_comm = &ivp_devp->ivp_comm;
	ret = regulator_enable(ivp_devp->ivp_media2_regulator);
	/* 0.Enable the power */
	loge_and_return_if_cond(ret, (ret != 0),
		"ivp_media2_regulator enable failed %d", ret);

	/* 1.Set Clock rate */
	if ((ret = clk_set_rate(ivp_comm->clk,
		(unsigned long)ivp_comm->clk_rate)) != 0) {
		ivp_err("set rate %#x fail, ret:%d", ivp_comm->clk_rate, ret);
		goto try_down_freq;
	}

	/* 2.Enable the clock */
	if ((ret = clk_prepare_enable(ivp_comm->clk)) != 0) {
		ivp_err("clk prepare enable failed, ret = %d", ret);
		goto try_down_freq;
	}
	goto normal_frq_success;

try_down_freq:
	ivp_info("try set core freq to: %ld",
		(unsigned long)ivp_devp->lowtemp_clk_rate);
	if ((ret = clk_set_rate(ivp_comm->clk,
		(unsigned long)ivp_devp->lowtemp_clk_rate)) != 0) {
		ivp_err("set low rate %#x fail, ret:%d",
			ivp_devp->lowtemp_clk_rate, ret);
		goto err_clk_set_rate;
	}

	if ((ret = clk_prepare_enable(ivp_comm->clk)) != 0) {
		ivp_err("low clk prepare enable failed, ret = %d", ret);
		goto err_clk_prepare_enable;
	}

normal_frq_success:
	ivp_info("set core success to: %ld", clk_get_rate(ivp_comm->clk));
	/* 3.Enable the power */
	if ((ret = regulator_enable(ivp_comm->regulator)) != 0) {
		ivp_err("regularot enable failed %d", ret);
		goto err_regulator_enable_ivp;
	}

	if (ivp_comm->ivp_secmode == SECURE_MODE)
		ivp_secure_config();
	ivp_set_qos_cfg(ivp_devp);

	return ret;

err_regulator_enable_ivp:
	clk_disable_unprepare(ivp_comm->clk);

err_clk_prepare_enable:
	if ((ret = clk_set_rate(ivp_comm->clk,
		(unsigned long)ivp_devp->lowtemp_clk_rate)) != 0)
		ivp_err("err set lowfrq rate %#x fail %d",
			ivp_devp->lowtemp_clk_rate, ret);

err_clk_set_rate:
	if ((ret = regulator_disable(ivp_devp->ivp_media2_regulator)) != 0)
		ivp_err("ivp_media2_regulator disable failed %d", ret);
	return -EINVAL;
}

int ivp_poweroff_pri(struct ivp_device *ivp_devp)
{
	int ret;
	int i = 0;
	unsigned int waiti;
	struct ivp_common *ivp_comm = NULL;

	loge_and_return_if_cond(-EINVAL, !ivp_devp, "invalid input param ivp_devp");

	ivp_comm = &ivp_devp->ivp_comm;
	waiti = ivp_reg_read(ivp_devp, IVP_REG_OFF_PWAITMODE);
	while (((waiti & IVP_DSP_PWAITMODE) == 0) && (i < 3)) { /* 3 is loop max */
		udelay(100); /* 100 us */
		ivp_err("ivp core is not in wfi mode, 0x%x", waiti);
		waiti = ivp_reg_read(ivp_devp, IVP_REG_OFF_PWAITMODE);
		i++;
	}

	ret = regulator_disable(ivp_comm->regulator);
	if (ret)
		ivp_err("regulator power off failed %d", ret);

	clk_disable_unprepare(ivp_comm->clk);
	ret = clk_set_rate(ivp_comm->clk,
		(unsigned long)ivp_comm->lowfrq_pd_clk_rate);
	if (ret)
		ivp_warn("set lfrq pd rate %#x fail, ret:%d",
			ivp_comm->lowfrq_pd_clk_rate, ret);

	ret = regulator_disable(ivp_devp->ivp_media2_regulator);
	if (ret)
		ivp_err("ivp_media2_regulator power off failed %d", ret);

	return ret;
}

int ivp_setup_regulator(struct platform_device *plat_dev,
	struct ivp_device *ivp_devp)
{
	struct regulator *regulator = NULL;
	struct regulator *ivp_media2_regulator = NULL;

	loge_and_return_if_cond(-EINVAL, (!ivp_devp || !plat_dev),
		"invalid input param ivp_devp or plat_dev");

	regulator = devm_regulator_get(&plat_dev->dev, IVP_REGULATOR);
	if (IS_ERR(regulator)) {
		ivp_err("Get ivp regulator failed");
		return -ENODEV;
	} else {
		ivp_devp->ivp_comm.regulator = regulator;
	}

	ivp_media2_regulator = devm_regulator_get(&plat_dev->dev,
		IVP_MEDIA_REGULATOR);
	if (IS_ERR(ivp_media2_regulator)) {
		ivp_err("Get ivp media regulator failed");
		return -ENODEV;
	} else {
		ivp_devp->ivp_media2_regulator = ivp_media2_regulator;
	}

	return 0;
}

int ivp_setup_clk(struct platform_device *plat_dev,
	struct ivp_device *ivp_devp)
{
	int ret;
	struct ivp_common *ivp_comm = NULL;

	loge_and_return_if_cond(-EINVAL, (!ivp_devp || !plat_dev),
		"invalid input param ivp_devp or plat_dev");

	ivp_comm = &ivp_devp->ivp_comm;
	ivp_comm->clk = devm_clk_get(&plat_dev->dev, OF_IVP_CLK_NAME);
	if (IS_ERR(ivp_comm->clk)) {
		ivp_err("get clk failed");
		return -ENODEV;
	}
	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_CLK_RATE_NAME, &ivp_comm->clk_rate);
	if (ret) {
		ivp_err("get clk rate failed, ret:%d", ret);
		return -ENOMEM;
	}
	ivp_info("get clk rate:%u", ivp_comm->clk_rate);

	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_MIDDLE_CLK_RATE_NAME, &ivp_devp->middle_clk_rate);
	loge_and_return_if_cond(-ENOMEM, (ret != 0),
		"middle freq rate failed, ret:%d", ret);
	ivp_info("middle freq clk rate:%u", ivp_devp->middle_clk_rate);

	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_LOW_CLK_RATE_NAME, &ivp_devp->low_clk_rate);
	loge_and_return_if_cond(-ENOMEM, (ret != 0),
		"low freq rate failed, ret:%d", ret);
	ivp_info("low freq clk rate:%u", ivp_devp->low_clk_rate);

	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_ULTRA_LOW_CLK_RATE_NAME, &ivp_devp->ultra_low_clk_rate);
	loge_and_return_if_cond(-ENOMEM, (ret != 0),
		"ultra low freq clk rate, ret:%d", ret);
	ivp_info("ultra low freq clk rate:%u", ivp_devp->ultra_low_clk_rate);

	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_LOWFREQ_CLK_RATE_NAME, &ivp_comm->lowfrq_pd_clk_rate);
	loge_and_return_if_cond(-ENOMEM, (ret != 0),
		"lowfreq pd clk rate failed, ret:%d", ret);
	ivp_info("lowfrq pd clk rate:%u", ivp_comm->lowfrq_pd_clk_rate);

	ret = of_property_read_u32(plat_dev->dev.of_node,
		OF_IVP_LOW_TEMP_RATE_NAME, &ivp_devp->lowtemp_clk_rate);
	loge_and_return_if_cond(-ENOMEM, (ret != 0),
		"low temperature rate failed, ret:%d", ret);
	ivp_info("low temperature clk rate:%u", ivp_devp->lowtemp_clk_rate);

	return ret;
}

int ivp_change_clk(struct ivp_device *ivp_devp, unsigned int level)
{
	int ret;
	struct ivp_common *ivp_comm = NULL;

	loge_and_return_if_cond(-EINVAL, !ivp_devp, "invalid input param ivp_devp");

	ivp_comm = &ivp_devp->ivp_comm;
	ivp_info("try to change freq level from %u to %u",
		ivp_comm->clk_level, level);

	if (ivp_comm->clk_level == IVP_CLK_LEVEL_LOW) {
		ret = clk_set_rate(ivp_comm->clk,
			(unsigned long)ivp_comm->lowfrq_pd_clk_rate);
		loge_and_return_if_cond(ret, (ret != 0), "set clk rate %#x fail, ret:%d",
			ivp_comm->lowfrq_pd_clk_rate, ret);
	}

	ivp_comm->clk_level = level;
	switch (ivp_comm->clk_level) {
	case IVP_CLK_LEVEL_ULTRA_LOW:
		ivp_info("ivp freq to ultra low level");
		ivp_comm->clk_usrsetrate = ivp_devp->ultra_low_clk_rate;
		break;

	case IVP_CLK_LEVEL_LOW:
		ivp_info("ivp freq to low level");
		ivp_comm->clk_usrsetrate = ivp_devp->low_clk_rate;
		break;

	case IVP_CLK_LEVEL_MEDIUM:
		ivp_info("ivp freq to media level");
		ivp_comm->clk_usrsetrate = ivp_devp->middle_clk_rate;
		break;

	case IVP_CLK_LEVEL_HIGH:
		ivp_info("ivp freq to high level");
		ivp_comm->clk_usrsetrate = ivp_comm->clk_rate;
		break;

	default:
		ivp_info("use default freq");
		ivp_comm->clk_usrsetrate = ivp_comm->clk_rate;
		break;
	}

	/* Set Clock rate */
	ivp_info("set clock rate");
	ret = clk_set_rate(ivp_comm->clk, (unsigned long)ivp_comm->clk_usrsetrate);
	if (!ret) {
		ivp_info("set core success to: %ld", clk_get_rate(ivp_comm->clk));
		return ret;
	}

	ivp_info("try set core freq to: %ld",
		(unsigned long)ivp_devp->low_clk_rate);
	ret = clk_set_rate(ivp_comm->clk,
		(unsigned long)ivp_devp->low_clk_rate);
	if (ret)
		ivp_err("set low rate %#x fail, ret:%d",
			ivp_devp->low_clk_rate, ret);

	return ret;
}
