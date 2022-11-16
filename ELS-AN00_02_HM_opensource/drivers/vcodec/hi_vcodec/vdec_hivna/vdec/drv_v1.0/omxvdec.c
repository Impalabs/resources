/*
 * omxvdec.c
 *
 * This is vdec driver interface.
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

#include "omxvdec.h"

#include <linux/clk.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/hisi-iommu.h>
#include <linux/dma-mapping.h>
#include <linux/dma-iommu.h>
#include <linux/dma-buf.h>
#include <linux/iommu.h>

#include "linux_kernel_osal.h"
#include "scd_drv.h"
#include "vdm_drv.h"
#include "vfmw_intf.h"
/*lint -e774*/

#define INVALID_IDX             (-1)
#define CLK_RATE_INDEX_FOR_OMX  0
#define CLK_RATE_INDEX_FOR_HEIF 1

static hi_s32 g_is_normal_init;

static hi_bool       g_is_device_detected  = HI_FALSE;
static struct class *g_omx_vdec_class     = HI_NULL;
static const hi_char g_omx_vdec_drv_name[] = OMXVDEC_NAME;
static dev_t         g_omx_vdec_dev_num;
static vdec_mem_info g_vdec_map_info[MAX_OPEN_COUNT];
static clk_rate_e g_clk_rate = 0;

omx_vdec_entry g_omx_vdec_entry;

// Modified for 64-bit platform
typedef enum {
	T_IOCTL_ARG,
	T_IOCTL_ARG_COMPAT,
	T_BUTT,
} compat_type_e;

#define check_para_size_return(size, para_size, command) \
	do { \
		if ((size) != (para_size)) { \
			dprint(PRN_FATAL, "%s: prarameter_size is error\n", command); \
			return - EINVAL; \
		} \
	} while (0)

#define check_return(cond, else_print) \
	do { \
		if (!(cond)) { \
			dprint(PRN_FATAL, "%s : %s\n", __func__, else_print); \
			return - EINVAL; \
		} \
	} while (0)

#define check_scene_eq_return(cond, else_print) \
	do { \
		if (cond) { \
			dprint(PRN_ALWS, "%s : %s\n", __func__, else_print); \
			return - EIO; \
		} \
	} while (0)

/* get fd index */
static hi_s32 vdec_get_file_index(const struct file *file)
{
	hi_s32 index;

	for (index = 0; index < MAX_OPEN_COUNT; index++) {
		if (file == g_vdec_map_info[index].file)
			return index;
	}

	return INVALID_IDX;
}

static clk_rate_e get_clk_rate(struct performance_params_s param)
{
	hi_s32 i;
	clk_rate_e max_base_rate = (clk_rate_e)0;
	clk_rate_e clk_rate = (clk_rate_e)0;
	hi_u64 total_load = 0;

	for (i = 0; i < MAX_OPEN_COUNT; i++) {
		if (g_vdec_map_info[i].file) {
			max_base_rate = g_vdec_map_info[i].base_rate > max_base_rate ?
				g_vdec_map_info[i].base_rate : max_base_rate;
			if (ULLONG_MAX - total_load >= g_vdec_map_info[i].load)
				total_load += g_vdec_map_info[i].load;
		}
	}

	for (i = VDEC_CLK_RATE_MAX - 1; i >= 0; i--) {
		if (total_load > param.load_range_map[i].lower_limit &&
			total_load <= param.load_range_map[i].upper_limit) {
			clk_rate = param.load_range_map[i].clk_rate;
			break;
		}
	}
	dprint(PRN_ALWS, "%s total_load %llu, clk_rate %d, max_base_rate %d\n",
		__func__, total_load, clk_rate, max_base_rate);
	clk_rate = max_base_rate > clk_rate ? max_base_rate : clk_rate;

	return clk_rate;
}

static ssize_t omxvdec_show(
	struct device *dev, struct device_attribute *attr,
	char *buf)
{
#ifdef USER_DISABLE_VDEC_PROC
	if (buf == HI_NULL) {
		dprint(PRN_FATAL, "%s buf is null\n", __func__);
		return 0;
	}
	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE, "0x%pK\n", (void *)(uintptr_t)g_smmu_page_base);
#else
	return 0;
#endif
}

static DEVICE_ATTR(omxvdec_misc, 0444, omxvdec_show, NULL);

static hi_s32 omxvdec_setup_cdev(
	omx_vdec_entry *omxvdec,
	const struct file_operations *fops)
{
	hi_s32 rc;
	struct device *dev = NULL;

	g_omx_vdec_class = class_create(THIS_MODULE, "omxvdec_class");
	if (IS_ERR(g_omx_vdec_class)) {
		rc = PTR_ERR(g_omx_vdec_class);
		g_omx_vdec_class = HI_NULL;
		dprint(PRN_FATAL, "%s call class_create failed, rc : %d\n", __func__, rc);
		return rc;
	}

	rc = alloc_chrdev_region(&g_omx_vdec_dev_num, 0, 1, "video decoder");
	if (rc) {
		dprint(PRN_FATAL, "%s call alloc_chrdev_region failed, rc : %d\n", __func__, rc);
		goto cls_destroy;
	}

	dev = device_create(g_omx_vdec_class, NULL, g_omx_vdec_dev_num, NULL, OMXVDEC_NAME);
	if (IS_ERR(dev)) {
		rc = PTR_ERR(dev);
		dprint(PRN_FATAL, "%s call device_create failed, rc : %d\n", __func__, rc);
		goto unregister_region;
	}

	rc = device_create_file(dev, &dev_attr_omxvdec_misc);
	if (rc) {
		dprint(PRN_FATAL, "%s, create device file failed\n", __func__);
		goto dev_destroy;
	}

	cdev_init(&omxvdec->cdev, fops);
	omxvdec->cdev.owner = THIS_MODULE;
	omxvdec->cdev.ops = fops;
	rc = cdev_add(&omxvdec->cdev, g_omx_vdec_dev_num, 1);
	if (rc < 0) {
		dprint(PRN_FATAL, "%s call cdev_add failed, rc : %d\n", __func__, rc);
		goto file_close;
	}

	return HI_SUCCESS;

file_close:
	device_remove_file(dev, &dev_attr_omxvdec_misc);
dev_destroy:
	device_destroy(g_omx_vdec_class, g_omx_vdec_dev_num);
unregister_region:
	unregister_chrdev_region(g_omx_vdec_dev_num, 1);
cls_destroy:
	class_destroy(g_omx_vdec_class);
	g_omx_vdec_class = HI_NULL;

	return rc;
}

static hi_s32 omxvdec_cleanup_cdev(omx_vdec_entry *omxvdec)
{
	if (!g_omx_vdec_class) {
		dprint(PRN_FATAL, "%s: Invalid g_omx_vdec_class is NULL", __func__);
		return HI_FAILURE;
	}

	device_remove_file(omxvdec->device, &dev_attr_omxvdec_misc);
	cdev_del(&omxvdec->cdev);
	device_destroy(g_omx_vdec_class, g_omx_vdec_dev_num);
	unregister_chrdev_region(g_omx_vdec_dev_num, 1);
	class_destroy(g_omx_vdec_class);
	g_omx_vdec_class = HI_NULL;

	return HI_SUCCESS;
}

static hi_s32 omxvdec_open(struct inode *inode, struct file *file)
{
	hi_s32 ret;
	omx_vdec_entry *omxvdec = HI_NULL;
	hi_s32 index;

	omxvdec = container_of(inode->i_cdev, omx_vdec_entry, cdev);

	vdec_mutex_lock(&omxvdec->omxvdec_mutex);
	vdec_mutex_lock(&omxvdec->vdec_mutex_scd);
	vdec_mutex_lock(&omxvdec->vdec_mutex_vdh);

	if (omxvdec->open_count < MAX_OPEN_COUNT) {
		omxvdec->open_count++;
		if (omxvdec->open_count == 1) {
			ret = vdec_regulator_enable();
			if (ret != HI_SUCCESS) {
				dprint(PRN_FATAL, "%s : vdec_regulator_enable failed\n", __func__);
				goto error0;
			}
			ret = vctrl_open_vfmw();
			if (ret != HI_SUCCESS) {
				dprint(PRN_FATAL, "%s : vfmw open failed\n", __func__);
				goto error1;
			}
			g_is_normal_init = 1;
		}
		/* save file */
		for (index = 0; index < MAX_OPEN_COUNT; index++) {
			if (g_vdec_map_info[index].file == HI_NULL)
				break;
		}
		if (index == MAX_OPEN_COUNT) {
			dprint(PRN_FATAL, "%s : in Map Info, open too much\n", __func__);
			ret = -EBUSY;
			goto exit;
		}
		g_vdec_map_info[index].file = file;

		file->private_data = omxvdec;
		ret = HI_SUCCESS;
	} else {
		dprint(PRN_FATAL, "%s open omxvdec instance too much\n", __func__);
		ret = -EBUSY;
	}

	dprint(PRN_ALWS, "%s, open_count : %d\n", __func__, omxvdec->open_count);
	goto exit;

error1:
	(void)vdec_regulator_disable();
error0:
	omxvdec->open_count--;
exit:
	vdec_mutex_unlock(&omxvdec->vdec_mutex_vdh);
	vdec_mutex_unlock(&omxvdec->vdec_mutex_scd);
	vdec_mutex_unlock(&omxvdec->omxvdec_mutex);

	return ret;
}

static hi_bool omxvdec_is_heif_scene(vdec_mem_info *mem_info)
{
	mem_buffer_s *vdh_mem = NULL;

	if (mem_info == NULL)
		return HI_FALSE;

	vdh_mem = mem_info->vdh;
	if (vdh_mem->scene == SCENE_HEIF)
		return HI_TRUE;

	return HI_FALSE;
}

static hi_s32 handle_omxvdec_open_count(omx_vdec_entry *omxvdec)
{
	hi_s32 ret = HI_SUCCESS;

	if (omxvdec->open_count > 0)
		omxvdec->open_count--;

	if (omxvdec->open_count > 0) {
		ret = vdec_regulator_set_clk_rate(g_clk_rate);
		if (ret != HI_SUCCESS)
			dprint(PRN_FATAL, "vdec set clk rate faied\n");
	} else if (omxvdec->open_count == 0) {
		g_clk_rate = 0;

		vctrl_close_vfmw();
		vdec_regulator_disable();
		if (omxvdec->device_locked) {
			vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_vdh);
			vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_scd);
			omxvdec->device_locked = HI_FALSE;
		}
		g_is_normal_init = 0;   //lint !e456
	}

	return ret;
}

static hi_s32 omxvdec_release(struct inode *inode, struct file *file)
{
	omx_vdec_entry *omxvdec = HI_NULL;
	hi_s32 ret = HI_SUCCESS;
	hi_s32 index;

	omxvdec = file->private_data;
	if (omxvdec == HI_NULL) {
		dprint(PRN_FATAL, "%s: invalid omxvdec is null\n", __func__);
		return -EFAULT;
	}

	vdec_mutex_lock(&omxvdec->omxvdec_mutex);
	vdec_mutex_lock(&omxvdec->vdec_mutex_scd);
	vdec_mutex_lock(&omxvdec->vdec_mutex_vdh);

	/* release file info */
	index = vdec_get_file_index(file);
	if (index == INVALID_IDX) {
		dprint(PRN_FATAL, "%s file is not open\n", __func__);
		ret = -EFAULT;
		goto exit;
	}

#ifdef MSG_POOL_ADDR_CHECK
	if (omxvdec_is_heif_scene(&g_vdec_map_info[index])) {
		ret = memset_s(&(g_vdec_map_info[index].vdh[VDH_SHAREFD_MESSAGE_POOL]),
			sizeof(g_vdec_map_info[index].vdh[VDH_SHAREFD_MESSAGE_POOL]), 0,
			sizeof(g_vdec_map_info[index].vdh[VDH_SHAREFD_MESSAGE_POOL]));
		if (ret != EOK) {
			dprint(PRN_FATAL, " %s %d memset_s err in function\n", __func__, __LINE__);
			ret = -EFAULT;
			goto exit;
		}
	}
#endif

	ret = memset_s(&g_vdec_map_info[index], sizeof(g_vdec_map_info[index]),
		0, sizeof(g_vdec_map_info[index]));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memset_s err in function\n", __func__, __LINE__);
		ret = -EFAULT;
		goto exit;
	}

	ret = handle_omxvdec_open_count(omxvdec);
	if (ret != HI_SUCCESS)
		goto exit;

	file->private_data = HI_NULL;

exit:
	dprint(PRN_ALWS, "exit %s, open_count: %d\n", __func__, omxvdec->open_count);

	vdec_mutex_unlock(&omxvdec->vdec_mutex_vdh);
	vdec_mutex_unlock(&omxvdec->vdec_mutex_scd);
	vdec_mutex_unlock(&omxvdec->omxvdec_mutex);

	return ret;
}

/* Modified for 64-bit platform */
static hi_s32 omxvdec_compat_get_data(
	compat_type_e eType, void __user *pUser, void *pData)
{
	hi_s32 ret = HI_SUCCESS;
	hi_s32 s32Data = 0;
	compat_ulong_t CompatData = 0;
	vdec_ioctl_msg *pIoctlMsg = (vdec_ioctl_msg *)pData;

	if (pUser == HI_NULL || pData == HI_NULL) {
		dprint(PRN_FATAL, "%s: param is null\n", __func__);
		return HI_FAILURE;
	}

	switch (eType) {
	case T_IOCTL_ARG:
		if (copy_from_user(pIoctlMsg, pUser, sizeof(*pIoctlMsg))) {
			dprint(PRN_FATAL, "%s: puser copy failed\n", __func__);
			ret = HI_FAILURE;
		}
		break;
	case T_IOCTL_ARG_COMPAT: {
		compat_ioctl_msg __user *pCompatMsg = pUser;

		ret += get_user(s32Data, &(pCompatMsg->chan_num));
		pIoctlMsg->chan_num = s32Data;

		ret += get_user(s32Data, &(pCompatMsg->in_size));
		pIoctlMsg->in_size = s32Data;

		ret += get_user(s32Data, &(pCompatMsg->out_size));
		pIoctlMsg->out_size = s32Data;

		ret += get_user(CompatData, &(pCompatMsg->in));
		pIoctlMsg->in = (void *)(uintptr_t)CompatData;

		ret += get_user(CompatData, &(pCompatMsg->out));
		pIoctlMsg->out = (void *)(uintptr_t)CompatData;

		ret = (ret == HI_SUCCESS) ? HI_SUCCESS : HI_FAILURE;
	}
		break;
	default:
		dprint(PRN_FATAL, "%s: unkown type %d\n", __func__, eType);
		ret = HI_FAILURE;
		break;
	}

	return ret;
}

static long handle_set_clk_rate(
	vdec_ioctl_msg vdec_msg, omx_vdec_entry *omxvdec,
	struct file *file)
{
	hi_s32 ret;
	struct performance_params_s performance_params;
	hi_u32 index;

	check_return(vdec_msg.in != HI_NULL, "VDEC_IOCTL_SET_CLK_RATE, invalid input prarameter");
	check_para_size_return(sizeof(performance_params), vdec_msg.in_size, "VDEC_IOCTL_SET_CLK_RATE");
	if (copy_from_user(&performance_params, vdec_msg.in, sizeof(performance_params))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_SET_CLK_RATE : copy_from_user failed\n");
		return -EFAULT;
	}

	vdec_mutex_lock(&omxvdec->omxvdec_mutex);
	vdec_mutex_lock(&omxvdec->vdec_mutex_scd);
	vdec_mutex_lock(&omxvdec->vdec_mutex_vdh);

	index = vdec_get_file_index(file);
	if (index == INVALID_IDX) {
		dprint(PRN_FATAL, "%s file is wrong\n", __func__);
		ret = -EIO;
		goto exit;
	}
	g_vdec_map_info[index].load = performance_params.load;
	g_vdec_map_info[index].base_rate =  performance_params.base_rate;

	g_clk_rate = get_clk_rate(performance_params);

	ret = vdec_regulator_set_clk_rate(g_clk_rate);
	if (ret != HI_SUCCESS)
		dprint(PRN_FATAL, "vdec_regulator_set_clk_rate faied\n");

exit:
	vdec_mutex_unlock(&omxvdec->vdec_mutex_vdh);
	vdec_mutex_unlock(&omxvdec->vdec_mutex_scd);
	vdec_mutex_unlock(&omxvdec->omxvdec_mutex);

	return ret;
}

static long handle_vdm_proc(
	vdec_ioctl_msg vdec_msg, omx_vdec_entry *omxvdec,
	const struct file *file)
{
	hi_s32 ret;
	hi_s32 fd_index;
	vdmhal_backup_s vdm_state_reg;
	omxvdh_reg_cfg_s vdm_reg_cfg;

	ret = memset_s(&vdm_state_reg, sizeof(vdm_state_reg), 0, sizeof(vdm_state_reg));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memset_s err in function\n", __func__, __LINE__);
		return -EFAULT;
	}
	check_return(vdec_msg.in != HI_NULL,
		"VDEC_IOCTL_VDM_PROC, invalid input prarameter");
	check_return(vdec_msg.out != HI_NULL,
		"VDEC_IOCTL_VDM_PROC, invalid output prarameter");
	check_para_size_return(sizeof(vdm_reg_cfg),
		vdec_msg.in_size, "VDEC_IOCTL_VDM_PROC_IN");
	check_para_size_return(sizeof(vdm_state_reg),
		vdec_msg.out_size, "VDEC_IOCTL_VDM_PROC_OUT");
	if (copy_from_user(&vdm_reg_cfg,
			vdec_msg.in, sizeof(vdm_reg_cfg))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_VDM_PROC : copy_from_user failed\n");
		return -EFAULT;
	}

	vdec_mutex_lock(&omxvdec->vdec_mutex_sec_vdh);
	vdec_mutex_lock(&omxvdec->vdec_mutex_vdh);
	fd_index = vdec_get_file_index(file);
	if (fd_index == INVALID_IDX) {
		dprint(PRN_FATAL, "%s file is wrong\n", __func__);
		vdec_mutex_unlock(&omxvdec->vdec_mutex_vdh);
		vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_vdh);
		return -EIO;
	}

	dsb(sy);

	ret = vctrl_vdm_hal_process(&vdm_reg_cfg, &vdm_state_reg,
		&(g_vdec_map_info[fd_index].vdh[0]));
	if (ret != HI_SUCCESS) {
		dprint(PRN_FATAL, "vctrl_vdm_hal_process failed\n");
		vdec_mutex_unlock(&omxvdec->vdec_mutex_vdh);
		vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_vdh);
		return -EIO;
	}

	vdec_mutex_unlock(&omxvdec->vdec_mutex_vdh);
	vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_vdh);
	if (copy_to_user(vdec_msg.out,
			&vdm_state_reg, sizeof(vdm_state_reg))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_VDM_PROC : copy_to_user failed\n");
		return -EFAULT;
	}

	return ret;
}

static long handle_get_vdm_hwstate(
	vdec_ioctl_msg vdec_msg, omx_vdec_entry *omxvdec)
{
	hi_s32 vdm_is_run;

	check_return(vdec_msg.out != HI_NULL,
		"VDEC_IOCTL_GET_VDM_HWSTATE, invalid output prarameter");
	check_para_size_return(sizeof(vdm_is_run),
		vdec_msg.out_size, "VDEC_IOCTL_GET_VDM_HWSTATE");
	vdec_mutex_lock(&omxvdec->vdec_mutex_sec_vdh);
	vdec_mutex_lock(&omxvdec->vdec_mutex_vdh);
	vdm_is_run = vctrl_vdm_hal_is_run();

	vdec_mutex_unlock(&omxvdec->vdec_mutex_vdh);
	vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_vdh);
	if (copy_to_user(vdec_msg.out, &vdm_is_run, sizeof(vdm_is_run))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_GET_VDM_HWSTATE : copy_to_user failed\n");
		return -EFAULT;
	}

	return HI_SUCCESS;
}

static long handle_scd_proc(
	vdec_ioctl_msg vdec_msg, omx_vdec_entry *omxvdec,
	const struct file *file)
{
	omx_scd_reg_cfg_s scd_reg_cfg;
	scd_state_reg_s scd_state_reg;
	hi_s32 fd_index;
	hi_s32 ret;

	check_return(vdec_msg.in != HI_NULL,
		"VDEC_IOCTL_SCD_PROC, invalid input prarameter");
	check_return(vdec_msg.out != HI_NULL,
		"VDEC_IOCTL_SCD_PROC, invalid output prarameter");
	check_para_size_return(sizeof(scd_reg_cfg),
		vdec_msg.in_size, "VDEC_IOCTL_SCD_PROC_IN");
	check_para_size_return(sizeof(scd_state_reg),
		vdec_msg.out_size, "VDEC_IOCTL_SCD_PROC_OUT");
	if (copy_from_user(&scd_reg_cfg,
			vdec_msg.in, sizeof(scd_reg_cfg))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_SCD_PROC :  copy_from_user failed\n");
		return -EFAULT;
	}

	vdec_mutex_lock(&omxvdec->vdec_mutex_sec_scd);
	vdec_mutex_lock(&omxvdec->vdec_mutex_scd);
	fd_index = vdec_get_file_index(file);
	if (fd_index == INVALID_IDX) {
		vdec_mutex_unlock(&omxvdec->vdec_mutex_scd);
		vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_scd);
		dprint(PRN_FATAL, "%s file is wrong\n", __func__);
		return -EIO;
	}

	dsb(sy);

	ret = vctrl_scd_hal_process(&scd_reg_cfg,
		&scd_state_reg, &(g_vdec_map_info[fd_index].scd[0]));
	if (ret != HI_SUCCESS) {
		dprint(PRN_FATAL, "vctrl_scd_hal_process failed\n");
		vdec_mutex_unlock(&omxvdec->vdec_mutex_scd);
		vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_scd);
		return -EIO;
	}

	vdec_mutex_unlock(&omxvdec->vdec_mutex_scd);
	vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_scd);
	if (copy_to_user(vdec_msg.out,
			&scd_state_reg, sizeof(scd_state_reg))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_SCD_PROC : copy_to_user failed\n");
		return -EFAULT;
	}

	return ret;
}

static long handle_iommu_map(vdec_ioctl_msg vdec_msg)
{
	hi_s32 ret;
	vdec_buffer_record buf_record;

	check_return(vdec_msg.in != HI_NULL,
		"VDEC_IOCTL_IOMMU_MAP, invalid input prarameter");
	check_return(vdec_msg.out != HI_NULL,
		"VDEC_IOCTL_IOMMU_MAP, invalid output prarameter");
	check_para_size_return(sizeof(buf_record),
		vdec_msg.in_size, "VDEC_IOCTL_IOMMU_MAP_IN");
	check_para_size_return(sizeof(buf_record),
		vdec_msg.out_size, "VDEC_IOCTL_IOMMU_MAP_OUT");
	if (copy_from_user(&buf_record,
			vdec_msg.in, sizeof(buf_record))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_IOMMU_MAP : copy_from_user failed\n");
		return -EFAULT;
	}

	ret = vdec_mem_iommu_map(buf_record.share_fd, &buf_record.iova);
	if (ret != HI_SUCCESS) {
		dprint(PRN_FATAL, "%s:VDEC_IOCTL_IOMMU_MAP failed\n", __func__);
		return -EFAULT;
	}

	if (copy_to_user(vdec_msg.out,
			&buf_record, sizeof(buf_record))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_IOMMU_MAP : copy_to_user failed\n");
		return -EFAULT;
	}

	return ret;
}

static long handle_iommu_unmap(vdec_ioctl_msg vdec_msg)
{
	hi_s32 ret;
	vdec_buffer_record buf_record;

	check_return(vdec_msg.in != HI_NULL, "VDEC_IOCTL_IOMMU_UNMAP, invalid input prarameter");
	check_para_size_return(sizeof(buf_record), vdec_msg.in_size, "VDEC_IOCTL_IOMMU_UNMAP_IN");
	if (copy_from_user(&buf_record,
			vdec_msg.in, sizeof(buf_record))) {
		dprint(PRN_FATAL, "VDEC_IOCTL_IOMMU_UNMAP : copy_from_user failed\n");
		return -EFAULT;
	}

	ret = vdec_mem_iommu_unmap(buf_record.share_fd, buf_record.iova);
	if (ret != HI_SUCCESS) {
		dprint(PRN_FATAL, "%s:VDEC_IOCTL_IOMMU_UNMAP failed\n", __func__);
		return -EFAULT;
	}

	return ret;
}

static long handle_lock_hw(unsigned int cmd, omx_vdec_entry *omxvdec)
{
	hi_bool x_scene;

	vdec_mutex_lock(&omxvdec->vdec_mutex_sec_scd);
	vdec_mutex_lock(&omxvdec->vdec_mutex_sec_vdh);
	x_scene = vctrl_scen_ident(cmd);
	if (x_scene == HI_FALSE) {
		vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_vdh);
		vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_scd);
		dprint(PRN_ALWS, "%s : lock hw error\n", __func__);
		return -EIO;
	}
	if (omxvdec->device_locked)
		dprint(PRN_ALWS, "hw have locked\n");
	omxvdec->device_locked = HI_TRUE;

	return HI_SUCCESS;
}

static long handle_unlock_hw(unsigned int cmd, omx_vdec_entry *omxvdec)
{
	hi_bool x_scene;

	x_scene = vctrl_scen_ident(cmd);
	check_scene_eq_return(x_scene == HI_FALSE, "unlock hw error");
	if (omxvdec->device_locked) {
		vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_vdh);
		vdec_mutex_unlock(&omxvdec->vdec_mutex_sec_scd);
		omxvdec->device_locked = HI_FALSE;
	}

	return HI_SUCCESS;
}

static long omxvdec_ioctl_common(
	struct file *file, unsigned int cmd,
	unsigned long arg, compat_type_e type)
{
	hi_s32 ret;
	vdec_ioctl_msg  vdec_msg;
	void __user *u_arg   = (void __user *)(uintptr_t)arg;
	omx_vdec_entry    *omxvdec = file->private_data;
	check_return(omxvdec != HI_NULL, "omxvdec is null");
	ret = memset_s(&vdec_msg, sizeof(vdec_msg), 0, sizeof(vdec_msg));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memset_s err in function\n", __func__, __LINE__);
		return -EFAULT;
	}
	if ((cmd != VDEC_IOCTL_UNLOCK_HW) && (cmd != VDEC_IOCTL_LOCK_HW)) {
		ret = omxvdec_compat_get_data(type, u_arg, &vdec_msg);
		check_return(ret == HI_SUCCESS, "compat data get failed");
	}

	switch (cmd) {
	case VDEC_IOCTL_SET_CLK_RATE:
		ret = handle_set_clk_rate(vdec_msg, omxvdec, file);
		break;
	case VDEC_IOCTL_VDM_PROC:
		ret = handle_vdm_proc(vdec_msg, omxvdec, file);
		break;
	case VDEC_IOCTL_GET_VDM_HWSTATE:
		ret = handle_get_vdm_hwstate(vdec_msg, omxvdec);
		break;

	case VDEC_IOCTL_SCD_PROC:
		ret = handle_scd_proc(vdec_msg, omxvdec, file);
		break;
	case VDEC_IOCTL_IOMMU_MAP:
		ret = handle_iommu_map(vdec_msg);
		break;
	case VDEC_IOCTL_IOMMU_UNMAP:
		ret = handle_iommu_unmap(vdec_msg);
		break;
		/*lint -e454 -e456 -e455*/
	case VDEC_IOCTL_LOCK_HW:
		ret = handle_lock_hw(cmd, omxvdec);
		break;
	case VDEC_IOCTL_UNLOCK_HW:
		ret = handle_unlock_hw(cmd, omxvdec);
		break;
		/*lint +e454 +e456 +e455*/
	default:
		/* could not handle ioctl */
		dprint(PRN_FATAL, "%s %d:  cmd : %d is not supported\n", __func__, __LINE__, _IOC_NR(cmd));
		return -ENOTTY;
	}

	return ret;    //lint !e454
}

static long omxvdec_ioctl(
	struct file *file, unsigned int cmd,
	unsigned long arg)
{
	return omxvdec_ioctl_common(file, cmd, arg, T_IOCTL_ARG);
}

/* Modified for 64-bit platform */
#ifdef CONFIG_COMPAT
static long omxvdec_compat_ioctl(
	struct file *file, unsigned int cmd,
	unsigned long arg)
{
	void *user_ptr = compat_ptr(arg);

	return  omxvdec_ioctl_common(file, cmd,
		(unsigned long)(uintptr_t)user_ptr, T_IOCTL_ARG_COMPAT);
}
#endif

static const struct file_operations omxvdec_fops = {
	.owner          = THIS_MODULE,
	.open           = omxvdec_open,
	.unlocked_ioctl = omxvdec_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl   = omxvdec_compat_ioctl,
#endif
	.release        = omxvdec_release,
};

static hi_s32 omxvdec_device_probe(struct platform_device *pltdev)
{
	hi_u32 fpga = 0;
	hi_u32 idle;
	hi_u32 peri_state  = 0;
	hi_u32 peri_mask   = 0;
	hi_u32 *pctrl_addr = HI_NULL;
	struct device *dev = &pltdev->dev;

	if (of_property_read_u32(dev->of_node, "vdec_fpga", &fpga) || fpga == 0) {
		dprint(PRN_ALWS, "not vdec fpga platform, return succ as normal mode\n");
		return HI_SUCCESS;
	}

	dprint(PRN_ALWS, "vdec fpga platform, check whether rtl logic exist\n");

	if (of_property_read_u32(dev->of_node, "pctrl_peri_state", &peri_state)) {
		dprint(PRN_ALWS, "pctrlPeriState read failed, do not init vdec driver\n");
		return HI_FAILURE;
	}
	if (of_property_read_u32(dev->of_node, "pctrl_peri_vdec_mask", &peri_mask)) {
		dprint(PRN_ALWS, "pctrlPeriMask read failed, do not init vdec driver\n");
		return HI_FAILURE;
	}

	pctrl_addr = (hi_u32 *)ioremap(peri_state, (unsigned long)4); //lint !e446
	if (pctrl_addr == HI_NULL) {
		dprint(PRN_ALWS, "ioremap vdec state reg failed, do not init vdec driver\n");
		return HI_FAILURE;
	}

	idle = readl(pctrl_addr) & peri_mask;
	iounmap(pctrl_addr);

	return ((idle != 0) ? HI_SUCCESS : HI_FAILURE);
}

static hi_s32 omxvdec_probe(struct platform_device *pltdev)
{
	hi_s32 ret;

	if (omxvdec_device_probe(pltdev) == HI_FAILURE) {
		dprint(PRN_ERROR, "fpga platform check vdec rtl failed, bypass driver probe\n");
		return HI_FAILURE;
	}

	if (g_is_device_detected == HI_TRUE) {
		dprint(PRN_DBG, "Already probe omxvdec\n");
		return HI_SUCCESS;
	}

	platform_set_drvdata(pltdev, HI_NULL);
	ret = memset_s(&g_omx_vdec_entry, sizeof(omx_vdec_entry), 0,
		sizeof(omx_vdec_entry));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memset_s err in function, ret = %d\n", __func__, __LINE__, ret);
		return HI_FAILURE;
	}
	ret = memset_s(g_vdec_map_info, sizeof(g_vdec_map_info), 0, sizeof(g_vdec_map_info));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memset_s err in function, ret = %d\n", __func__, __LINE__, ret);
		return HI_FAILURE;
	}
	vdec_init_mutex(&g_omx_vdec_entry.omxvdec_mutex);
	vdec_init_mutex(&g_omx_vdec_entry.vdec_mutex_scd);
	vdec_init_mutex(&g_omx_vdec_entry.vdec_mutex_vdh);
	vdec_init_mutex(&g_omx_vdec_entry.vdec_mutex_sec_scd);
	vdec_init_mutex(&g_omx_vdec_entry.vdec_mutex_sec_vdh);

	ret = vdec_mem_probe(&pltdev->dev);
	if (ret != HI_SUCCESS) {
		dprint(PRN_FATAL, "%s call vdec_mem_probe failed, ret = %d\n", __func__, ret);
		return HI_FAILURE;
	}

	ret = omxvdec_setup_cdev(&g_omx_vdec_entry, &omxvdec_fops);
	if (ret < 0) {
		dprint(PRN_FATAL, "%s call omxvdec_setup_cdev failed, ret = %d\n", __func__, ret);
		return HI_FAILURE;
	}

	g_omx_vdec_entry.device = &pltdev->dev;

	ret = vdec_regulator_probe(&pltdev->dev);
	if (ret != HI_SUCCESS) {
		dprint(PRN_FATAL, "%s call Regulator_Initialize failed, ret = %d\n", __func__, ret);
		omxvdec_cleanup_cdev(&g_omx_vdec_entry);
		return HI_FAILURE;
	}

	platform_set_drvdata(pltdev, &g_omx_vdec_entry);
	g_is_device_detected = HI_TRUE;

	dprint(PRN_ALWS, "vdec probe success\n");
	return HI_SUCCESS;
}

static hi_s32 omxvdec_remove(struct platform_device *pltdev)
{
	omx_vdec_entry *omxvdec = HI_NULL;

	omxvdec = platform_get_drvdata(pltdev);
	if (omxvdec != HI_NULL) {
		if (IS_ERR(omxvdec)) {
			dprint(PRN_ERROR, "call platform_get_drvdata err, errno : %ld\n", PTR_ERR(omxvdec));
		} else {
			omxvdec_cleanup_cdev(omxvdec);
			vdec_regulator_remove(&pltdev->dev);
			platform_set_drvdata(pltdev, HI_NULL);
			g_is_device_detected = HI_FALSE;
		}
	}

	return 0;
}
omx_vdec_entry omx_vdec_get_entry(void)
{
	return g_omx_vdec_entry;
}

static hi_s32 omxvdec_suspend(
	struct platform_device *pltdev,
	pm_message_t state)
{
	SINT32 ret;

	dprint(PRN_ALWS, "%s +\n", __func__);

	if (g_is_normal_init != 0)
		vctrl_suspend();

	ret = vdec_regulator_disable();
	if (ret != HI_SUCCESS)
		dprint(PRN_FATAL, "%s disable regulator failed\n", __func__);

	dprint(PRN_ALWS, "%s -\n", __func__);

	return HI_SUCCESS;
}

static hi_s32 omxvdec_resume(struct platform_device *pltdev)
{
	SINT32 ret;
	clk_rate_e resume_clk = VDEC_CLK_RATE_NORMAL;

	dprint(PRN_ALWS, "%s +\n", __func__);
	vdec_regulator_get_clk_rate(&resume_clk);

	if (g_is_normal_init != 0) {
		ret = vdec_regulator_enable();
		if (ret != HI_SUCCESS) {
			dprint(PRN_FATAL, "%s, enable regulator failed\n", __func__);
			return HI_FAILURE;
		}

		ret = vdec_regulator_set_clk_rate(resume_clk);
		if (ret != HI_SUCCESS)
			dprint(PRN_ERROR, "%s, set clk failed\n", __func__);

		vctrl_resume();
	}

	dprint(PRN_ALWS, "%s -\n", __func__);

	return HI_SUCCESS;
}

static const struct of_device_id vdec_match_table[] = {
	{.compatible = "hisilicon,HiVCodecV200-vdec", },
	{.compatible = "hisilicon,HiVCodecV210-vdec", },
	{.compatible = "hisilicon,HiVCodecV300-vdec", },
	{.compatible = "hisilicon,HiVCodecV310-vdec", },
	{.compatible = "hisilicon,HiVCodecV500-vdec", },
	{.compatible = "hisilicon,HiVCodecV520-vdec", },
	{ }
};

MODULE_DEVICE_TABLE(of, vdec_match_table);

static struct platform_driver omxvdec_driver = {

	.probe   = omxvdec_probe,
	.remove  = omxvdec_remove,
	.suspend = omxvdec_suspend,
	.resume  = omxvdec_resume,
	.driver  = {
		.name  = (hi_pchar) g_omx_vdec_drv_name,
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(vdec_match_table),
	},
};

hi_s32 __init omx_vdec_drv_mod_init(void)
{
	int ret = platform_driver_register(&omxvdec_driver);
	if (ret) {
		dprint(PRN_FATAL, "%s register platform driver failed\n", __func__);
		return ret;
	}

#ifdef MODULE
	dprint(PRN_ALWS, "Load hi_omxvdec.ko :%d success\n", OMXVDEC_VERSION);
#endif

	return ret;
}

void __exit omx_vdec_drv_mod_exit(void)
{
	platform_driver_unregister(&omxvdec_driver);

#ifdef MODULE
	dprint(PRN_ALWS, "Unload hi_omxvdec.ko : %d success\n", OMXVDEC_VERSION);
#endif
}

module_init(omx_vdec_drv_mod_init);
module_exit(omx_vdec_drv_mod_exit);

MODULE_AUTHOR("gaoyajun@hisilicon.com");
MODULE_DESCRIPTION("vdec driver");
MODULE_LICENSE("GPL");

