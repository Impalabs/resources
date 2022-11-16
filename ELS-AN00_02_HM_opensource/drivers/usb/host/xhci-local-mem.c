 /*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2020. All rights reserved.
 * Description: Support for xhci dma ops
 * Create: 2017-07-12
 *
 * This software is distributed under the terms of the GNU General
 * Public License ("GPL") as published by the Free Software Foundation,
 * either version 2 of that License or (at your option) any later version.
 */

#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>

#include "xhci.h"
#include "xhci-trace.h"
#include "xhci-local-mem.h"

#define MAX_PRE_ALLOC_NUM 120
struct dma_pool_cfg {
	size_t size;
	int pre_alloc_num;
};
static struct dma_pool_cfg xhci_dma_pool_cfg[XHCI_DMA_POOL_NUM] = {
	/* size pre_alloc_num */
	{128,	1},
	{512,	1},
	{1024,	8},
	{2048,	8},
	{4096,	120},
	{8192,	60},
	{16384,	32},
	{32768,	12},
	{65536,	6},
};
static void *pool_pre_alloc_addr[MAX_PRE_ALLOC_NUM];
static dma_addr_t pool_pre_alloc_dma[MAX_PRE_ALLOC_NUM];

void xhci_destroy_dma_pool(struct xhci_hcd *xhci)
{
	struct usb_hcd *hcd = NULL;
	int i;

	if (!xhci)
		return;

	hcd = xhci_to_hcd(xhci);
	if (!(hcd->driver->flags & HCD_LOCAL_MEM))
		return;

	for (i = 0; i < XHCI_DMA_POOL_NUM; i++) {
		struct dma_pool *pool = xhci->pool[i];

		if (pool != NULL) {
			dma_pool_destroy(pool);
			xhci->pool[i] = NULL;
		}
	}
}

static int xhci_pool_pre_alloc(struct xhci_hcd *xhci, struct dma_pool *pool,
	int index)
{
	int i;
	int ret = 0;

	for (i = 0; i < xhci_dma_pool_cfg[index].pre_alloc_num; i++) {
		if (i >= MAX_PRE_ALLOC_NUM) {
			xhci_warn(xhci, "pre alloc num exceeds MAX_PRE_ALLOC_NUM\n");
			goto dma_free;
		}
		pool_pre_alloc_addr[i] = dma_pool_alloc(pool, GFP_KERNEL,
			&pool_pre_alloc_dma[i]);
		xhci_dbg(xhci, "alloc from index %d:%pK(%llx)\n", index,
			pool_pre_alloc_addr[i], pool_pre_alloc_dma[i]);
		if (!pool_pre_alloc_addr[i]) {
			ret = -ENOMEM;
			goto dma_free;
		}
	}

dma_free:
	while (--i >= 0) {
		xhci_dbg(xhci, "free from index %d:%pK(%llx)\n", index,
			pool_pre_alloc_addr[i], pool_pre_alloc_dma[i]);
		dma_pool_free(pool, pool_pre_alloc_addr[i],
			pool_pre_alloc_dma[i]);
	}

	return ret;
}

#define XHCI_DMA_POOL_NAME_LEN	20
int xhci_create_dma_pool(struct xhci_hcd *xhci)
{
	struct usb_hcd *hcd = NULL;
	char name[XHCI_DMA_POOL_NAME_LEN] = {0};
	int i, size;

	if (!xhci)
		return -EINVAL;

	hcd = xhci_to_hcd(xhci);
	if (!(hcd->driver->flags & HCD_LOCAL_MEM))
		return -EINVAL;

	for (i = 0; i < XHCI_DMA_POOL_NUM; i++) {
		size = xhci_dma_pool_cfg[i].size;
		if (!size)
			continue;
		snprintf(name, sizeof(name), "xhci-buffer-%d", size);
		xhci->pool[i] = dma_pool_create(name, hcd->self.sysdev,
			size, size, 0);
		if (!xhci->pool[i]) {
			xhci_err(xhci, "create dma pool %s failed\n", name);
			xhci_destroy_dma_pool(xhci);
			return -ENOMEM;
		}

		if (xhci_pool_pre_alloc(xhci, xhci->pool[i], i)) {
			xhci_err(xhci, "%s pre alloc failed\n", name);
			xhci_destroy_dma_pool(xhci);
			return -ENOMEM;
		}
	}
	return 0;
}

static int __xhci_alloc_dma_check_input(struct urb *urb, struct usb_hcd *hcd)
{
	if (urb->transfer_buffer == NULL) {
		WARN_ON_ONCE(1);
		return -EFAULT;
	}

	if (!hcd->self.sysdev->dma_mask ||
	    !(hcd->driver->flags & HCD_LOCAL_MEM)) {
		WARN_ON_ONCE(1);
		return -EFAULT;
	}
	return 0;
}

static int xhci_alloc_32bit_dma(struct xhci_hcd *xhci, struct urb *urb,
	gfp_t mem_flags)
{
	struct usb_hcd *hcd = xhci_to_hcd(xhci);
	u32 size;
	void *vaddr = NULL;
	dma_addr_t dma = 0;
	int i, dir;

	if (__xhci_alloc_dma_check_input(urb, hcd))
		return -EFAULT;

	dir = usb_urb_dir_in(urb);
	if (dir)
		size = max(urb->transfer_buffer_length,
			(u32)usb_endpoint_maxp(&urb->ep->desc));
	else
		size = urb->transfer_buffer_length;

	for (i = 0; i < XHCI_DMA_POOL_NUM; i++) {
		if (size <= xhci_dma_pool_cfg[i].size) {
			vaddr = dma_pool_alloc(xhci->pool[i],
				mem_flags, &dma);
			if (!vaddr)
				xhci_err(xhci, "dma_pool_alloc failed, size %u\n", size);
			break;
		}
	}

	if (!vaddr) {
		xhci_info(xhci, "%s:%d dma_alloc_coherent size %u\n", __func__,
				__LINE__, size);
		vaddr = dma_alloc_coherent(hcd->self.sysdev,
			size, &dma, mem_flags);
		if (!vaddr) {
			xhci_err(xhci, "dma_alloc_coherent failed\n");
			return -ENOMEM;
		}
		urb->transfer_flags |= URB_MAP_LOCAL_REALLOC;
	}

	if (!dir)
		memcpy(vaddr, urb->transfer_buffer,
			urb->transfer_buffer_length);

	urb->origin_transfer_buffer = urb->transfer_buffer;
	urb->origin_dma = urb->transfer_dma;
	urb->transfer_buffer = vaddr;
	urb->transfer_dma = dma;

	xhci_dbg(xhci, "buffer re-alloc %pK, dma is 0x%llx\n",
		urb->transfer_buffer, urb->transfer_dma);

	return 0;
}

static void xhci_free_32bit_dma(struct xhci_hcd *xhci, struct urb *urb)
{
	struct usb_hcd *hcd = xhci_to_hcd(xhci);
	struct xhci_local_dma *local_dma = NULL;
	unsigned long flags;
	u32 size;
	int i, dir;

	if (!hcd->self.sysdev->dma_mask ||
	    !(hcd->driver->flags & HCD_LOCAL_MEM)) {
		WARN_ON_ONCE(1);
		return;
	}

	dir = usb_urb_dir_in(urb);
	if (dir) {
		memcpy(urb->origin_transfer_buffer, urb->transfer_buffer,
			urb->transfer_buffer_length);

		size = max_t(u32, urb->transfer_buffer_length,
			(u32)usb_endpoint_maxp(&urb->ep->desc));
	} else {
		size = urb->transfer_buffer_length;
	}

	if (urb->transfer_flags & URB_MAP_LOCAL_REALLOC) {
		local_dma = kmalloc(sizeof(*local_dma), GFP_ATOMIC);
		if (!local_dma) {
			WARN_ON(1);
		} else {
			local_dma->vaddr = urb->transfer_buffer;
			local_dma->dma = urb->transfer_dma;
			local_dma->size = size;
			spin_lock_irqsave(&xhci->dma_manager.lock, flags);
			list_add_tail(&local_dma->list,
				&xhci->dma_manager.dma_free_list);
			if (!queue_work(system_power_efficient_wq,
				&xhci->dma_manager.dma_free_wk))
				xhci_info(xhci, "dma_free_wk already on the queue\n");
			spin_unlock_irqrestore(&xhci->dma_manager.lock, flags);
		}
	} else {
		for (i = 0; i < XHCI_DMA_POOL_NUM; i++) {
			if (size <= xhci_dma_pool_cfg[i].size) {
				dma_pool_free(xhci->pool[i],
					urb->transfer_buffer, urb->transfer_dma);
				break;
			}
		}
	}

	/* Restore origin_transfer_buffer */
	urb->transfer_buffer = urb->origin_transfer_buffer;
	urb->transfer_dma = urb->origin_dma;
	urb->origin_transfer_buffer = 0;
	urb->origin_dma = 0;
}

static int xhci_check_dma_addr(struct xhci_hcd *xhci, struct urb *urb,
	enum dma_data_direction dir, gfp_t mem_flags)
{
	struct usb_hcd *hcd = xhci_to_hcd(xhci);
	int ret = 0;

	if (urb->transfer_dma & ~(DMA_BIT_MASK(32))) { /* DMA bit width is 32 */
		WARN_ON(urb->transfer_flags & URB_NO_TRANSFER_DMA_MAP);
		if (urb->transfer_flags & URB_DMA_MAP_PAGE)
			dma_unmap_page(hcd->self.sysdev,
				       urb->transfer_dma,
				       urb->transfer_buffer_length,
				       dir);
		else if (urb->transfer_flags & URB_DMA_MAP_SINGLE)
			dma_unmap_single(hcd->self.sysdev,
					 urb->transfer_dma,
					 urb->transfer_buffer_length,
					 dir);
		urb->transfer_flags &= ~(URB_DMA_MAP_PAGE | URB_DMA_MAP_SINGLE);

		ret = xhci_alloc_32bit_dma(xhci, urb, mem_flags);
		if (!ret)
			urb->transfer_flags |= URB_MAP_LOCAL;
	}

	return ret;
}

static int __check_dma_mapping_err(struct usb_hcd *hcd, struct urb *urb,
	unsigned int transfer_flags)
{
	int ret = 0;

	if (dma_mapping_error(hcd->self.sysdev, urb->transfer_dma))
		ret = -EAGAIN;
	else
		urb->transfer_flags |= transfer_flags;
	return ret;
}

int xhci_map_urb_for_dma(struct usb_hcd *hcd, struct urb *urb,
	gfp_t mem_flags)
{
	struct xhci_hcd *xhci = NULL;
	enum dma_data_direction dir;
	int ret;

	if (!hcd || !urb)
		return -EINVAL;

	xhci = hcd_to_xhci(hcd);

	dir = usb_urb_dir_in(urb) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;

	if (urb->transfer_buffer_length == 0)
		return 0;

	if (!(urb->transfer_flags & URB_NO_TRANSFER_DMA_MAP)) {
		if (urb->num_sgs) {
			pr_warn("%s:%d do not support sg list\n",
				__func__, __LINE__);
			ret = -EINVAL;
			goto err;
		} else if (urb->sg) {
			struct scatterlist *sg = urb->sg;

			urb->transfer_dma = dma_map_page(
				hcd->self.sysdev, sg_page(sg),
				sg->offset, urb->transfer_buffer_length, dir);
			ret = __check_dma_mapping_err(hcd, urb,
				URB_DMA_MAP_PAGE);
			if (ret)
				goto err;
		} else if (is_vmalloc_addr(urb->transfer_buffer)) {
			WARN_ONCE(1, "transfer buffer not dma capable\n");
			ret = -EAGAIN;
			goto err;
		} else {
			urb->transfer_dma = dma_map_single(
				hcd->self.sysdev, urb->transfer_buffer,
				urb->transfer_buffer_length, dir);
			ret = __check_dma_mapping_err(hcd, urb,
				URB_DMA_MAP_SINGLE);
			if (ret)
				goto err;
		}
	}

	ret = xhci_check_dma_addr(xhci, urb, dir, mem_flags);
	if (ret)
		goto err;
	else
		WARN_ON(upper_32_bits(urb->transfer_dma));

	return 0;

err:
	xhci_err(xhci, "xhci_check_dma_addr failed, ret %d\n", ret);
	return ret;
}

void xhci_unmap_urb_for_dma(struct usb_hcd *hcd, struct urb *urb)
{
	struct xhci_hcd *xhci = NULL;
	enum dma_data_direction dir;

	if (!hcd || !urb)
		return;

	xhci = hcd_to_xhci(hcd);

	dir = usb_urb_dir_in(urb) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
	if (urb->transfer_flags & URB_DMA_MAP_SINGLE)
		dma_unmap_single(hcd->self.sysdev,
				 urb->transfer_dma,
				 urb->transfer_buffer_length,
				 dir);
	else if (urb->transfer_flags & URB_DMA_MAP_PAGE)
		dma_unmap_page(hcd->self.sysdev,
			       urb->transfer_dma,
			       urb->transfer_buffer_length,
			       dir);
	else if (urb->transfer_flags & URB_MAP_LOCAL)
		xhci_free_32bit_dma(xhci, urb);

	/* Make it safe to call this routine more than once */
	urb->transfer_flags &= ~(URB_DMA_MAP_SG | URB_DMA_MAP_PAGE |
		URB_DMA_MAP_SINGLE | URB_MAP_LOCAL | URB_MAP_LOCAL_REALLOC);
}

void xhci_dma_free_handler(struct work_struct *work)
{
	struct xhci_hcd *xhci = container_of(work, struct xhci_hcd,
		dma_manager.dma_free_wk);
	struct usb_hcd *hcd = xhci_to_hcd(xhci);
	struct xhci_local_dma *local_dma = NULL;
	struct xhci_local_dma *next = NULL;
	unsigned long flags;

	pr_info("+\n");
	spin_lock_irqsave(&xhci->dma_manager.lock, flags);
	list_for_each_entry_safe(local_dma, next,
			&xhci->dma_manager.dma_free_list, list) {
		list_del(&local_dma->list);
		spin_unlock_irqrestore(&xhci->dma_manager.lock, flags);
		xhci_info(xhci, "free add %pK, dma %llx\n",
			local_dma->vaddr, local_dma->dma);
		dma_free_coherent(hcd->self.sysdev, local_dma->size,
			local_dma->vaddr, local_dma->dma);
		kfree(local_dma);
		spin_lock_irqsave(&xhci->dma_manager.lock, flags);
	}
	spin_unlock_irqrestore(&xhci->dma_manager.lock, flags);
	pr_info("-\n");
}

