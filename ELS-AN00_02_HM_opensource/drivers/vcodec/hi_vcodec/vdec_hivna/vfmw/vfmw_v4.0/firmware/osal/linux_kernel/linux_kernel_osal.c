/*
 * linux_kernel_osal.c
 *
 * This is for linux_kernel_osal proc.
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

#include "linux_kernel_osal.h"

#include <linux/sched/clock.h>
#include "public.h"

#ifdef ENV_ARMLINUX_KERNEL

/* SpinLock */
osal_irq_spin_lock g_spin_lock_scd;
osal_irq_spin_lock g_spin_lock_vdh;
osal_irq_spin_lock g_spin_lock_record;

/* Mutext */
osal_task_mutex g_int_event;

osal_task_mutex g_scd_hw_done_event;
osal_task_mutex g_vdm_hw_done_event;

/* Semaphore */
osal_sema g_scd_sem;
osal_sema g_vdh_sem;
osal_sema g_bpd_sem;

/* Extern */
extern vfmw_osal_func_ptr g_vfmw_osal_fun_ptr;

#define OSAL_PRINT  printk
#define MAX_WAIT_EVENT_CNT  100
#define time_period(begin, end) (((end) >= (begin)) ? \
	((end) - (begin)) : (0xffffffff - (begin) + (end)))

UINT64 oasl_get_time_in_ms(void)
{
	UINT64 sys_time;

	sys_time = sched_clock();
	do_div(sys_time, UM_COUNT_OF_A_MM);

	return sys_time;
}

UINT64 osal_get_time_in_us(void)
{
	UINT64 sys_time;

	sys_time = sched_clock();
	do_div(sys_time, MM_COUNT_OF_A_S);

	return sys_time;
}

static inline SINT32 osal_init_event(osal_event *event, SINT32 init_val)
{
	event->flag = init_val;
	init_waitqueue_head(&(event->queue_head));
	return OSAL_OK;
}

static inline SINT32 osal_give_event(osal_event *event)
{
	event->flag = 1;
	wake_up_interruptible(&(event->queue_head));

	return OSAL_OK;
}

static  SINT32 osal_wait_event(osal_event *event, SINT32 ms_wait_time)
{
	SINT32 ret;
	UINT32 cnt = 0;

	UINT64 start_time, cur_time;

	start_time = VFMW_OSAL_GET_TIME_IN_MS();

	do {
		/*lint -e578 -e666*/
		ret = wait_event_interruptible_timeout((event->queue_head),
			(event->flag != 0), (msecs_to_jiffies(ms_wait_time)));
		/*lint +e578 +e666*/
		if (ret < 0) {
			cur_time = VFMW_OSAL_GET_TIME_IN_MS();
			if (time_period(start_time, cur_time) >
					(UINT32)ms_wait_time) {
				dprint(PRN_ALWS, "wait event time out, time : %llu, cnt: %d\n", time_period(start_time, cur_time), cnt);
				ret = 0;
				break;
			}
		}
		cnt++;
	} while ((event->flag == 0) && (ret < 0));

	if (cnt > MAX_WAIT_EVENT_CNT)
		dprint(PRN_ALWS, "the max cnt of wait_event interrupts by singal is %d\n", cnt);

	if (ret  == 0)
		dprint(PRN_ALWS, "wait pEvent signal timeout\n");

	event->flag = 0;

	return (ret != 0) ? OSAL_OK : OSAL_ERR;
}

UINT8 *osal_register_map(UADDR phy_addr, UINT32 size)
{
	return (UINT8 *) ioremap_nocache(phy_addr, size);//lint !e446
}

void osal_register_un_map(UINT8 *vir_addr, UINT32 size)
{
	iounmap(vir_addr);
}

SINT32 osal_file_write(const char *buf, int len, struct file *filp)
{
	int writelen;
	mm_segment_t oldfs;

	if (filp == NULL || buf == NULL)
		return -ENOENT;

	if (filp->f_op->write == NULL)
		return -ENOSYS;

	if ((filp->f_flags & O_ACCMODE & (O_WRONLY | O_RDWR)) == 0)
		return -EACCES;

	oldfs = get_fs();
	set_fs(KERNEL_DS); /*lint !e501*/
	writelen = filp->f_op->write(filp, buf, len, &filp->f_pos);
	set_fs(oldfs);

	return writelen;
}

static inline void osal_sema_intit(osal_sema *sem)
{
	sema_init(sem, 1);
}

static inline SINT32 osal_down_interruptible(osal_sema *sem)
{
	return down_interruptible(sem);
}

static inline void osal_up(osal_sema *sem)
{
	up(sem);
}

static inline void osal_spin_lock_irq_init(osal_irq_spin_lock *intr_mutex)
{
	spin_lock_init(&intr_mutex->irq_lock);
	intr_mutex->is_init = 1;
}

static inline SINT32 osal_spin_lock_irq(osal_irq_spin_lock *intr_mutex)
{
	if (intr_mutex->is_init == 0) {
		spin_lock_init(&intr_mutex->irq_lock);
		intr_mutex->is_init = 1;
	}
	spin_lock_irqsave(&intr_mutex->irq_lock, intr_mutex->irq_lockflags);

	return OSAL_OK;
}

static inline SINT32 osal_spin_unlock_irq(osal_irq_spin_lock *intr_mutex)
{
	spin_unlock_irqrestore(&intr_mutex->irq_lock,
		intr_mutex->irq_lockflags);

	return OSAL_OK;
}

void osal_mb(void)
{
	mb();
}

void osal_u_delay(ULONG usecs)
{
	udelay(usecs);
}

void osal_m_sleep(UINT32 msecs)
{
	msleep(msecs);
}

SINT32 osal_request_irq(
	UINT32 irq, osal_irq_handler_t handler,
	ULONG flags, const char *name, void *dev)
{
	return request_irq(irq, handler, flags, name, dev);
}

void osal_fre_irq(UINT32 irq, void *dev)
{
	free_irq(irq, dev);
}

void *osal_alloc_vir_mem(SINT32 size)
{
	return vmalloc(size);
}

void osal_free_vir_mem(const void *p)
{
	if (p)
		vfree(p);
}

UINT8 *osal_mmap(UADDR phyaddr, UINT32 len)
{
	return NULL;
}

UINT8 *osal_mmap_cache(UADDR phyaddr, UINT32 len)
{
	return NULL;
}

void osal_munmap(UINT8 *p)
{
}

osal_irq_spin_lock *get_spin_lock_by_enum(spin_lock_type lock_type)
{
	osal_irq_spin_lock *spin_lock = NULL;

	switch (lock_type) {
	case G_SPINLOCK_SCD:
		spin_lock = &g_spin_lock_scd;
		break;

	case G_SPINLOCK_RECORD:
		spin_lock = &g_spin_lock_record;
		break;

	case G_SPINLOCK_VDH:
		spin_lock = &g_spin_lock_vdh;
		break;

	default:
		dprint(PRN_ERROR, "%s unkown spin_lock_type %d\n", __func__, lock_type);
		break;
	}

	return spin_lock;
}

void osal_spin_lock_init(spin_lock_type lock_type)
{
	osal_irq_spin_lock *spin_lock = NULL;

	spin_lock = get_spin_lock_by_enum(lock_type);

	osal_spin_lock_irq_init(spin_lock);
}

SINT32 osal_spin_lock(spin_lock_type lock_type)
{
	osal_irq_spin_lock *spin_lock = NULL;

	spin_lock = get_spin_lock_by_enum(lock_type);

	return osal_spin_lock_irq(spin_lock);
}

SINT32 osal_spin_unlock(spin_lock_type lock_type)
{
	osal_irq_spin_lock *spin_lock = NULL;

	spin_lock = get_spin_lock_by_enum(lock_type);

	return osal_spin_unlock_irq(spin_lock);
}

osal_sema *get_sem_by_enum(sem_type sem)
{
	osal_sema *osal_sem = NULL;

	switch (sem) {
	case G_SCD_SEM:
		osal_sem = &g_scd_sem;
		break;

	case G_VDH_SEM:
		osal_sem = &g_vdh_sem;
		break;

	case G_BPD_SEM:
		osal_sem = &g_bpd_sem;
		break;

	default:
		dprint(PRN_ERROR, "%s unkown sem_type %d\n", __func__, sem);
		break;
	}

	return osal_sem;
}

void osal_sem_init(sem_type sem)
{
	osal_sema *osal_sem = NULL;

	osal_sem = get_sem_by_enum(sem);

	osal_sema_intit(osal_sem);
}

SINT32 osal_sem_down(sem_type sem)
{
	osal_sema *osal_sem = NULL;

	osal_sem = get_sem_by_enum(sem);

	return osal_down_interruptible(osal_sem);
}

void osal_sem_up(sem_type sem)
{
	osal_sema *osal_sem = NULL;

	osal_sem = get_sem_by_enum(sem);

	osal_up(osal_sem);
}

SINT32 osal_init_wait_que(mutex_type mutext_type, SINT32 init_val)
{
	SINT32 ret_val = OSAL_ERR;

	switch (mutext_type) {
	case G_SCDHWDONEEVENT:
		ret_val = osal_init_event(&g_scd_hw_done_event, init_val);
		break;

	case G_VDMHWDONEEVENT:
		ret_val = osal_init_event(&g_vdm_hw_done_event, init_val);
		break;

	default:
		break;
	}
	return ret_val;
}

SINT32 osal_wake_up_wait_que(mutex_type mutex_type)
{
	SINT32 ret_val = OSAL_ERR;

	switch (mutex_type) {
	case G_SCDHWDONEEVENT:
		ret_val = osal_give_event(&g_scd_hw_done_event);
		break;

	case G_VDMHWDONEEVENT:
		ret_val = osal_give_event(&g_vdm_hw_done_event);
		break;

	default:
		break;
	}

	return ret_val;
}

SINT32 osal_wait_wait_que(mutex_type mutex_type, SINT32 wait_time_in_ms)
{
	SINT32 ret_val = OSAL_ERR;

	switch (mutex_type) {
	case G_SCDHWDONEEVENT:
		ret_val = osal_wait_event(&g_scd_hw_done_event, wait_time_in_ms);
		break;

	case G_VDMHWDONEEVENT:
		ret_val = osal_wait_event(&g_vdm_hw_done_event, wait_time_in_ms);
		break;

	default:
		break;
	}

	return ret_val;
}

void osal_init_interface(void)
{
	hi_s32 ret;

	ret = memset_s(&g_vfmw_osal_fun_ptr, sizeof(g_vfmw_osal_fun_ptr), 0,
		sizeof(g_vfmw_osal_fun_ptr));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memset_s err in function\n", __func__, __LINE__);
		return;
	}

	g_vfmw_osal_fun_ptr.pfun_osal_get_time_in_ms = oasl_get_time_in_ms;
	g_vfmw_osal_fun_ptr.pfun_osal_get_time_in_us = osal_get_time_in_us;
	g_vfmw_osal_fun_ptr.pfun_osal_spin_lock_init = osal_spin_lock_init;
	g_vfmw_osal_fun_ptr.pfun_osal_spin_lock      = osal_spin_lock;
	g_vfmw_osal_fun_ptr.pfun_osal_spin_unlock    = osal_spin_unlock;
	g_vfmw_osal_fun_ptr.pfun_osal_sema_init      = osal_sem_init;
	g_vfmw_osal_fun_ptr.pfun_osal_sema_down      = osal_sem_down;
	g_vfmw_osal_fun_ptr.pfun_osal_sema_up        = osal_sem_up;
	g_vfmw_osal_fun_ptr.pfun_osal_print          = OSAL_PRINT;
	g_vfmw_osal_fun_ptr.pfun_osal_m_sleep        = osal_m_sleep;
	g_vfmw_osal_fun_ptr.pfun_osal_mb             = osal_mb;
	g_vfmw_osal_fun_ptr.pfun_osal_u_delay        = osal_u_delay;
	g_vfmw_osal_fun_ptr.pfun_osal_init_event     = osal_init_wait_que;
	g_vfmw_osal_fun_ptr.pfun_osal_give_event     = osal_wake_up_wait_que;
	g_vfmw_osal_fun_ptr.pfun_osal_wait_event     = osal_wait_wait_que;
	g_vfmw_osal_fun_ptr.pfun_osal_request_irq    = osal_request_irq;
	g_vfmw_osal_fun_ptr.pfun_osal_free_irq       = osal_fre_irq;
	g_vfmw_osal_fun_ptr.pfun_osal_register_map   = osal_register_map;
	g_vfmw_osal_fun_ptr.pfun_osal_register_unmap = osal_register_un_map;
	g_vfmw_osal_fun_ptr.pfun_osal_mmap           = osal_mmap;
	g_vfmw_osal_fun_ptr.pfun_osal_mmap_cache     = osal_mmap_cache;
	g_vfmw_osal_fun_ptr.pfun_osal_mun_map        = osal_munmap;
	g_vfmw_osal_fun_ptr.pfun_osal_alloc_vir_mem  = osal_alloc_vir_mem;
	g_vfmw_osal_fun_ptr.pfun_osal_free_vir_mem   = osal_free_vir_mem;
}

#endif
