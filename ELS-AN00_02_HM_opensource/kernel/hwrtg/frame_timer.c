/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
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
 * frame freq timer
 */

#include "include/frame_timer.h"

#include <linux/timer.h>
#include <linux/kthread.h>
#include <trace/events/sched.h>

#ifdef CONFIG_HW_RTG_SCHED
#include <linux/sched/frame.h>
#endif
#ifdef CONFIG_HW_MTK_RTG_SCHED
#include <linux/wait_bit.h>
#include <mtkrtg/frame.h>
#endif

#include "include/proc_state.h"
#include "include/aux.h"
#include "include/set_rtg.h"

struct timer_list g_frame_timer;
atomic_t g_timer_on = ATOMIC_INIT(0);

static struct task_struct *g_timer_thread;
unsigned long g_thread_flag;
#define DISABLE_FRAME_SCHED 0

static int frame_thread_func(void *data)
{
#ifdef CONFIG_HW_RTG_MULTI_FRAME
	int id;
#endif

	for (;;) {
		wait_on_bit(&g_thread_flag, DISABLE_FRAME_SCHED,
			TASK_INTERRUPTIBLE);
		set_frame_min_util(rtg_frame_info(DEFAULT_RT_FRAME_ID), 0, true);
#ifdef CONFIG_HW_RTG_MULTI_FRAME
		for (id = MULTI_FRAME_ID; id < (MULTI_FRAME_ID + MULTI_FRAME_NUM); id++)
			set_frame_min_util(rtg_frame_info(id), 0, true);
#endif
		set_aux_boost_util(0);
		set_boost_thread_min_util(0);

		if (!is_rtg_sched_enable(DEFAULT_RT_FRAME_ID))
			set_frame_sched_state(rtg_frame_info(DEFAULT_RT_FRAME_ID), false);
#ifdef CONFIG_HW_RTG_MULTI_FRAME
		for (id = MULTI_FRAME_ID; id < (MULTI_FRAME_ID + MULTI_FRAME_NUM); id++) {
			if (!is_rtg_sched_enable(id))
				set_frame_sched_state(rtg_frame_info(id), false);
		}
#endif
		set_bit(DISABLE_FRAME_SCHED, &g_thread_flag);
	}
	return 0;
}

static void wake_thread(void)
{
	clear_bit(DISABLE_FRAME_SCHED, &g_thread_flag);
	smp_mb__after_atomic();
	wake_up_bit(&g_thread_flag, DISABLE_FRAME_SCHED);
}

static void on_timer_timeout(unsigned long pdata)
{
#ifdef CONFIG_FRAME_RTG
	trace_rtg_frame_sched(0, "g_frame_timer", 0);
#endif
#ifdef CONFIG_HW_MTK_RTG_SCHED
	trace_rtg_frame_sched("g_frame_timer", 0);
#endif
	wake_thread();
}

void init_frame_timer(void)
{
	if (atomic_read(&g_timer_on) == 1)
		return;

	atomic_set(&g_timer_on, 1);
	init_timer(&g_frame_timer);
	g_frame_timer.function = on_timer_timeout;
	g_frame_timer.data = 0;

	/* create thread */
	set_bit(DISABLE_FRAME_SCHED, &g_thread_flag);
	g_timer_thread = kthread_create(frame_thread_func, NULL, "frame_sched");
	if (g_timer_thread)
		wake_up_process(g_timer_thread);
	else
		pr_err("[AWARE_RTG] g_timer_thread create failed\n");
}

void deinit_frame_timer(void)
{
	if (atomic_read(&g_timer_on) == 0)
		return;

	atomic_set(&g_timer_on, 0);
	del_timer_sync(&g_frame_timer);
}

void start_boost_timer(u32 duration, u32 min_util)
{
	unsigned long dur = msecs_to_jiffies(duration);
	int id = DEFAULT_RT_FRAME_ID;

	if (atomic_read(&g_timer_on) == 0)
		return;

	if (timer_pending(&g_frame_timer) &&
		time_after(g_frame_timer.expires, jiffies + dur))
		return;

	set_frame_min_util(rtg_frame_info(id), min_util, true);
#ifdef CONFIG_HW_RTG_MULTI_FRAME
	for (id = MULTI_FRAME_ID; id < (MULTI_FRAME_ID + MULTI_FRAME_NUM); id++)
		set_frame_min_util(rtg_frame_info(id), min_util, true);
#endif
	set_aux_boost_util(min_util);
	set_boost_thread_min_util(min_util);

	mod_timer(&g_frame_timer, jiffies + dur);
#ifdef CONFIG_FRAME_RTG
	trace_rtg_frame_sched(0, "g_frame_timer", dur);
#endif
#ifdef CONFIG_HW_MTK_RTG_SCHED
	trace_rtg_frame_sched("g_frame_timer", dur);
#endif
}
