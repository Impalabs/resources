/*
 * hisi_karma_pmu.c
 *
 * Karma Performance Monitor Unit driver
 *
 * Copyright (c) 2018-2020 Huawei Technologies Co., Ltd.
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
#define PMUNAME		"hisi_karma"
#define DRVNAME		PMUNAME "_pmu"
#undef pr_fmt
#define pr_fmt(fmt)     "karma_pmu: " fmt

#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/version.h>
#include <linux/bug.h>
#include <linux/cpuhotplug.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/list.h>
#include <linux/platform_device.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <linux/cpu_pm.h>
#include <karma_pmu_plat.h>
#include <securec.h>
#include "l3cache_common.h"
#include "hisi_karma_pmu.h"

static unsigned long g_karma_pmu_cpuhp_state;

/*
 * Select the counter register offset using the counter index
 */
#define KARMA_PMU_OFFSET	8
static u32 karma_pmu_get_counter_offset(int cntr_idx)
{
	return (u32)(KARMA_CNTR0_OFFSET + (cntr_idx * KARMA_PMU_OFFSET));
}

static inline bool karma_pmu_counter_valid(struct karma_pmu *karma_pmu, int idx)
{
	return idx >= 0 && idx < karma_pmu->num_counters;
}

static u64 karma_pmu_read_counter(struct karma_pmu *karma_pmu,
				  struct hw_perf_event *hwc)
{
	u32 idx = hwc->idx;
	unsigned long flags;
	u64 val;

	if (WARN_ON(!cpumask_test_cpu(smp_processor_id(),
				      &karma_pmu->associated_cpus)))
		return 0;

	if (!karma_pmu_counter_valid(karma_pmu, idx)) {
		dev_err(karma_pmu->dev, "Unsupported event index:%d\n", idx);
		return 0;
	}

	raw_spin_lock_irqsave(&karma_pmu->pmu_lock, flags);
	/* Read 64-bits and the upper 32 bits are RAZ */
	val = readq(karma_pmu->base + karma_pmu_get_counter_offset(idx));
	raw_spin_unlock_irqrestore(&karma_pmu->pmu_lock, flags);

	return val;
}

static void karma_pmu_write_counter(struct karma_pmu *karma_pmu,
				    struct hw_perf_event *hwc, u64 val)
{
	u32 idx = hwc->idx;
	unsigned long flags;

	if (WARN_ON(!cpumask_test_cpu(smp_processor_id(),
				      &karma_pmu->associated_cpus)))
		return;

	if (!karma_pmu_counter_valid(karma_pmu, idx)) {
		dev_err(karma_pmu->dev, "Unsupported event index:%d\n", idx);
		return;
	}

	raw_spin_lock_irqsave(&karma_pmu->pmu_lock, flags);
	/* Write 64-bits and the upper 32 bits are WI */
	writeq(val, karma_pmu->base + karma_pmu_get_counter_offset(idx));
	raw_spin_unlock_irqrestore(&karma_pmu->pmu_lock, flags);
}

static void karma_pmu_write_evtype(struct karma_pmu *karma_pmu,
				   int idx, u32 type)
{
	unsigned long flags;

	if (!karma_pmu_counter_valid(karma_pmu, idx)) {
		dev_err(karma_pmu->dev, "Unsupported event index:%d\n", idx);
		return;
	}

	raw_spin_lock_irqsave(&karma_pmu->pmu_lock, flags);
	writel(type, karma_pmu->base + KARMA_EVTYPER0_OFFSET + idx * 4);
	raw_spin_unlock_irqrestore(&karma_pmu->pmu_lock, flags);
}

static void karma_pmu_start_counters(struct karma_pmu *karma_pmu)
{
	u32 val;
	unsigned long flags;

	/*
	 * Set enable bit in KARMA_CR_EL0 register to start counting
	 * for all enabled counters.
	 */
	raw_spin_lock_irqsave(&karma_pmu->pmu_lock, flags);
	val = readl(karma_pmu->base + KARMA_CR_EL0);
	val |= KARMA_PMU_EN;
	writel(val, karma_pmu->base + KARMA_CR_EL0);
	raw_spin_unlock_irqrestore(&karma_pmu->pmu_lock, flags);
}

static void karma_pmu_stop_counters(struct karma_pmu *karma_pmu)
{
	u32 val;
	unsigned long flags;

	/*
	 * Clear enable bit in KARMA_CR_EL0 register to stop counting
	 * for all enabled counters.
	 */
	raw_spin_lock_irqsave(&karma_pmu->pmu_lock, flags);
	val = readl(karma_pmu->base + KARMA_CR_EL0);
	val &= ~(KARMA_PMU_EN);
	writel(val, karma_pmu->base + KARMA_CR_EL0);
	raw_spin_unlock_irqrestore(&karma_pmu->pmu_lock, flags);
}

static void karma_pmu_enable_counter(struct karma_pmu *karma_pmu,
				     struct hw_perf_event *hwc)
{
	/* Enable counter index in KARMA_ENSET register */
	writel(BIT((unsigned int)hwc->idx),
	       karma_pmu->base + KARMA_ENSET_OFFSET);
}

static void karma_pmu_disable_counter(struct karma_pmu *karma_pmu,
				      struct hw_perf_event *hwc)
{
	/* Clear counter index in KARMA_ENCLR register */
	writel(BIT((unsigned int)hwc->idx),
	       karma_pmu->base + KARMA_ENCLR_OFFSET);
}

static int karma_pmu_init_data(struct platform_device *pdev,
			       struct karma_pmu *karma_pmu)
{
	struct device *dev = &pdev->dev;

	karma_pmu->base = of_iomap(dev->of_node, 0);
	if (karma_pmu->base == NULL) {
		dev_err(&pdev->dev, "ioremap failed for karma_pmu\n");
		return -ENOMEM;
	}

	raw_spin_lock_init(&karma_pmu->pmu_lock);
	spin_lock_init(&karma_pmu->fcm_idle_lock);

	cpumask_copy(&karma_pmu->associated_cpus, cpu_online_mask);
	return 0;
}

#ifdef CONFIG_KARMA_PMU_DEBUG
static struct attribute *karma_pmu_format_attr[] = {
	/* cppcheck-suppress */
	KARMA_FORMAT_ATTR(event, "config:0-31"),
	NULL,
};

static const struct attribute_group karma_pmu_format_group = {
	.name = "format",
	.attrs = karma_pmu_format_attr,
};

static const struct attribute_group karma_pmu_events_group = {
	.name = "events",
	.attrs = karma_pmu_events_attr,
};

static struct attribute *karma_pmu_cpumask_attrs[] = {
	KARMA_CPUMASK_ATTR(cpumask, KARMA_ACTIVE_CPU_MASK),
	KARMA_CPUMASK_ATTR(associated_cpus, KARMA_ASSOCIATED_CPU_MASK),
	NULL,
};

static const struct attribute_group karma_pmu_cpumask_attr_group = {
	.attrs = karma_pmu_cpumask_attrs,
};

static const struct attribute_group *karma_pmu_attr_groups[] = {
	&karma_pmu_cpumask_attr_group,
	&karma_pmu_events_group,
	&karma_pmu_format_group,
	NULL,
};

/*
 * PMU event attributes
 */
ssize_t karma_pmu_sysfs_event_show(struct device *dev,
				   struct device_attribute *attr, char *page)
{
	struct dev_ext_attribute *eattr = NULL;

	eattr = container_of(attr, struct dev_ext_attribute, attr);

	return snprintf_s(page, PAGE_SIZE, PAGE_SIZE - 1,
			  "config=0x%lx\n", (uintptr_t)eattr->var);
}

/*
 * PMU format attributes
 */
ssize_t karma_pmu_sysfs_format_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *eattr = NULL;

	eattr = container_of(attr, struct dev_ext_attribute, attr);

	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1,
			  "%s\n", (char *)eattr->var);
}

ssize_t karma_pmu_cpumask_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(dev_get_drvdata(dev));
	struct dev_ext_attribute *eattr = container_of(attr,
						       struct dev_ext_attribute,
						       attr);
	unsigned long mask_id = (uintptr_t)eattr->var;
	const cpumask_t *cpumask = NULL;

	switch (mask_id) {
	case KARMA_ACTIVE_CPU_MASK:
		cpumask = &karma_pmu->active_cpu;
		break;
	case KARMA_ASSOCIATED_CPU_MASK:
		cpumask = &karma_pmu->associated_cpus;
		break;
	default:
		return 0;
	}
	return cpumap_print_to_pagebuf(true, buf, cpumask);
}
#endif

static bool karma_validate_event_group(struct perf_event *event)
{
	struct perf_event *sibling = NULL;
	struct perf_event *leader = event->group_leader;
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	/* Include count for the event */
	int counters = 1;

	if (is_software_event(leader) == 0) {
		/*
		 * We must NOT create groups containing mixed PMUs, although
		 * software events are acceptable
		 */
		if (leader->pmu != event->pmu)
			return false;

		/* Increment counter for the leader */
		if (leader != event)
			counters++;
	}

	for_each_sibling_event(sibling, event->group_leader) {
		if (is_software_event(sibling))
			continue;

		if (sibling->pmu != event->pmu)
			return false;
		/* Increment counter for each sibling */
		counters++;
	}

	/* The group can not count events more than the counters in the HW */
	return counters <= karma_pmu->num_counters;
}

static int karma_pmu_get_event_idx(struct perf_event *event)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	unsigned long *used_mask = karma_pmu->pmu_events.used_mask;
	u32 num_counters = karma_pmu->num_counters;
	int idx;

	idx = find_first_zero_bit(used_mask, num_counters);
	if (idx == num_counters)
		return -EAGAIN;

	set_bit(idx, used_mask);

	return idx;
}

static void karma_pmu_clear_event_idx(struct karma_pmu *karma_pmu, int idx)
{
	if (!karma_pmu_counter_valid(karma_pmu, idx)) {
		dev_err(karma_pmu->dev, "Unsupported event index:%d!\n", idx);
		return;
	}

	clear_bit(idx, karma_pmu->pmu_events.used_mask);
}

static int karma_pmu_event_init(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	struct karma_pmu *karma_pmu = NULL;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	/*
	 * We do not support sampling as the counters are all
	 * shared by all CPU cores. Also we
	 * do not support attach to a task(per-process mode)
	 */
	if (is_sampling_event(event) != 0 ||
	    (event->attach_state & PERF_ATTACH_TASK) != 0)
		return -EOPNOTSUPP;

	/* counters do not have these bits */
	if (event->attr.exclude_user ||
	    event->attr.exclude_kernel ||
	    event->attr.exclude_host ||
	    event->attr.exclude_guest ||
	    event->attr.exclude_hv ||
	    event->attr.exclude_idle)
		return -EINVAL;

	/*
	 *  The karma counters not specific to any CPU, so cannot
	 *  support per-task
	 */
	if (event->cpu < 0)
		return -EINVAL;

	/*
	 * Validate if the events in group does not exceed the
	 * available counters in hardware.
	 */
	if (!karma_validate_event_group(event))
		return -EINVAL;

	karma_pmu = to_hisi_pmu(event->pmu);

	if (!cpumask_test_cpu(event->cpu, &karma_pmu->associated_cpus)) {
		dev_err(karma_pmu->pmu.dev,
			"Requested cpu is not associated with the karma\n");
		return -EINVAL;
	}

	if (event->attr.config > karma_pmu->check_event)
		return -EINVAL;

	if (cpumask_empty(&karma_pmu->active_cpu))
		return -EINVAL;
	/*
	 * We don't assign an index until we actually place the event onto
	 * hardware. Use -1 to signify that we haven't decided where to put it
	 * yet.
	 */
	hwc->idx = -1;
	hwc->config_base = event->attr.config;

	/* Enforce to use the same CPU for all events in this PMU */
	event->cpu = cpumask_first(&karma_pmu->active_cpu);

	return 0;
}

/*
 * Set the counter to count the event that we're interested in,
 * and enable interrupt and counter.
 */
static void karma_pmu_enable_event(struct perf_event *event)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	karma_pmu_write_evtype(karma_pmu, hwc->idx,
			       PMU_GET_EVENTID(event));

	karma_pmu_enable_counter(karma_pmu, hwc);
}

/*
 * Disable counter and interrupt.
 */
static void karma_pmu_disable_event(struct perf_event *event)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	karma_pmu_disable_counter(karma_pmu, hwc);
}

static void karma_pmu_set_event_period(struct perf_event *event)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	/*
	 * The HiSilicon PMU counters support 32 bits or 48 bits, depending on
	 * the PMU. We reduce it to 2^(counter_bits - 1) to account for the
	 * extreme interrupt latency. So we could hopefully handle the overflow
	 * interrupt before another 2^(counter_bits - 1) events occur and the
	 * counter overtakes its previous value.
	 */
	u64 val = BIT_ULL(karma_pmu->counter_bits - 1);

	local64_set(&hwc->prev_count, val);
	/* Write start value to the hardware event counter */
	karma_pmu_write_counter(karma_pmu, hwc, val);
}

static void karma_pmu_event_update(struct perf_event *event)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u64 delta, prev_raw_count, new_raw_count;

	do {
		/* Read the count from the counter register */
		new_raw_count = karma_pmu_read_counter(karma_pmu, hwc);
		prev_raw_count = local64_read(&hwc->prev_count);
	} while (local64_cmpxchg(&hwc->prev_count, prev_raw_count,
				 new_raw_count) != prev_raw_count);
	/*
	 * compute the delta
	 */
	delta = (new_raw_count - prev_raw_count) &
		PMU_MAX_PERIOD(karma_pmu->counter_bits);
	local64_add(delta, &event->count);
}

static void karma_pmu_start(struct perf_event *event, int flags)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	if (WARN_ON_ONCE(!((unsigned int)hwc->state & PERF_HES_STOPPED)))
		return;

	WARN_ON_ONCE(!((unsigned int)hwc->state & PERF_HES_UPTODATE));
	hwc->state = 0;
	karma_pmu_set_event_period(event);

	if (((unsigned int)flags & PERF_EF_RELOAD) != 0) {
		u64 prev_raw_count =  local64_read(&hwc->prev_count);

		karma_pmu_write_counter(karma_pmu, hwc, prev_raw_count);
	}

	karma_pmu_enable_event(event);
	perf_event_update_userpage(event);
}

static void karma_pmu_stop(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;

	karma_pmu_disable_event(event);
	WARN_ON_ONCE((unsigned int)hwc->state & PERF_HES_STOPPED);
	hwc->state = (int)((unsigned int)hwc->state | PERF_HES_STOPPED);

	if (((unsigned int)hwc->state & PERF_HES_UPTODATE) != 0)
		return;

	/* Read hardware counter and update the perf counter statistics */
	karma_pmu_event_update(event);
	hwc->state = (int)((unsigned int)hwc->state | PERF_HES_UPTODATE);
}

static int karma_pmu_add(struct perf_event *event, int flags)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	int idx;

	if (WARN_ON_ONCE(!cpumask_test_cpu(smp_processor_id(),
					   &karma_pmu->associated_cpus))) {
		return -ENOENT;
	}

	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;

	/* Get an available counter index for counting */
	idx = karma_pmu_get_event_idx(event);
	if (idx < 0)
		return idx;

	event->hw.idx = idx;
	karma_pmu->pmu_events.hw_events[idx] = event;

	if (((unsigned int)flags & PERF_EF_START) != 0)
		karma_pmu_start(event, PERF_EF_RELOAD);

	return 0;
}

static void karma_pmu_del(struct perf_event *event, int flags)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	karma_pmu_stop(event, PERF_EF_UPDATE);
	karma_pmu_clear_event_idx(karma_pmu, hwc->idx);
	perf_event_update_userpage(event);
	karma_pmu->pmu_events.hw_events[hwc->idx] = NULL;
}

static void karma_pmu_read(struct perf_event *event)
{
	/* Read hardware counter and update the perf counter statistics */
	karma_pmu_event_update(event);
}

static void karma_pmu_enable(struct pmu *pmu)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(pmu);
	int enabled = bitmap_weight(karma_pmu->pmu_events.used_mask,
				   KARMA_PMU_MAX_HW_CNTRS);

	if (enabled == 0)
		return;

	karma_pmu_start_counters(karma_pmu);
}

static void karma_pmu_disable(struct pmu *pmu)
{
	struct karma_pmu *karma_pmu = to_hisi_pmu(pmu);

	karma_pmu_stop_counters(karma_pmu);
}

struct cpu_pm_karma_pmu_args {
	struct karma_pmu	*karma_pmu;
	unsigned long	cmd;
	int		cpu;
	int		ret;
};

#ifdef CONFIG_CPU_PM
static void cpu_pm_karma_pmu_setup(struct karma_pmu *karma_pmu,
				   unsigned long cmd)
{
	struct karma_pmu_hwevents *hw_evts = &karma_pmu->pmu_events;
	struct perf_event *event = NULL;
	int idx;

	for (idx = 0; idx < karma_pmu->num_counters; idx++) {
		/*
		 * If the counter is not used skip it, there is no
		 * need of stopping/restarting it.
		 */
		if (!test_bit(idx, hw_evts->used_mask))
			continue;

		event = hw_evts->hw_events[idx];
		if (event == NULL)
			continue;

		/*
		 * Check if an attempt was made to free this event during
		 * the CPU went offline.
		 */
		if (event->state != PERF_EVENT_STATE_ACTIVE)
			continue;

		switch (cmd) {
		case CPU_PM_ENTER:
			/*
			 * Stop and update the counter
			 */
			karma_pmu_stop(event, PERF_EF_UPDATE);
			break;
		case CPU_PM_EXIT:
		case CPU_PM_ENTER_FAILED:
			 /*
			  * Restore and enable the counter.
			  * karma_pmu_start() indirectly calls
			  *
			  * perf_event_update_userpage()
			  *
			  * that requires RCU read locking to be functional,
			  * wrap the call within RCU_NONIDLE to make the
			  * RCU subsystem aware this cpu is not idle from
			  * an RCU perspective for the karma_pmu_start() call
			  * duration.
			  */
			RCU_NONIDLE(karma_pmu_start(event, PERF_EF_RELOAD));
			break;
		default:
			break;
		}
	}
}

static void cpu_pm_karma_pmu_common(void *info)
{
	struct cpu_pm_karma_pmu_args *data = info;
	struct karma_pmu *karma_pmu = data->karma_pmu;
	unsigned long cmd = data->cmd;
	int cpu = data->cpu;
	struct karma_pmu_hwevents *hw_events = &karma_pmu->pmu_events;
	int enabled = bitmap_weight(hw_events->used_mask,
				    karma_pmu->num_counters);
	bool fcm_pwrdn = 0;

	if (!cpumask_test_cpu(cpu, &karma_pmu->associated_cpus)) {
		data->ret = NOTIFY_DONE;
		return;
	}

	if (enabled == 0) {
		data->ret = NOTIFY_OK;
		return;
	}

	data->ret = NOTIFY_OK;

	switch (cmd) {
	case CPU_PM_ENTER:
		spin_lock(&karma_pmu->fcm_idle_lock);
		fcm_pwrdn = lpcpu_fcm_cluster_pwrdn();
		if (fcm_pwrdn && !karma_pmu->fcm_idle) {
			karma_pmu->fcm_idle = true;
			karma_pmu_disable(&karma_pmu->pmu);
			cpu_pm_karma_pmu_setup(karma_pmu, cmd);
		}
		spin_unlock(&karma_pmu->fcm_idle_lock);
		break;
	case CPU_PM_EXIT:
	case CPU_PM_ENTER_FAILED:
		spin_lock(&karma_pmu->fcm_idle_lock);
		if (karma_pmu->fcm_idle) {
			karma_pmu->fcm_idle = false;
			cpu_pm_karma_pmu_setup(karma_pmu, cmd);
			karma_pmu_enable(&karma_pmu->pmu);
		}
		spin_unlock(&karma_pmu->fcm_idle_lock);
		break;
	default:
		data->ret = NOTIFY_DONE;
		break;
	}
}

static int cpu_pm_karma_pmu_notify(struct notifier_block *b,
				   unsigned long cmd, void *v)
{
	struct cpu_pm_karma_pmu_args data = {
		.karma_pmu = container_of(b, struct karma_pmu, cpu_pm_nb),
		.cmd = cmd,
		.cpu = smp_processor_id(),
	};

	cpu_pm_karma_pmu_common(&data);
	return data.ret;
}

static int cpu_pm_karma_pmu_register(struct karma_pmu *karma_pmu)
{
	karma_pmu->cpu_pm_nb.notifier_call = cpu_pm_karma_pmu_notify;
	return cpu_pm_register_notifier(&karma_pmu->cpu_pm_nb);
}

static void cpu_pm_karma_pmu_unregister(struct karma_pmu *karma_pmu)
{
	cpu_pm_unregister_notifier(&karma_pmu->cpu_pm_nb);
}

#else
static inline int cpu_pm_karma_pmu_register(struct karma_pmu *karma_pmu)
{
	return 0;
}

static inline void cpu_pm_karma_pmu_unregister(struct karma_pmu *karma_pmu) { }
static void cpu_pm_karma_pmu_common(void *info) { }
#endif

static int karma_pmu_dev_probe(struct platform_device *pdev,
			       struct karma_pmu *karma_pmu)
{
	int ret;

	ret = karma_pmu_init_data(pdev, karma_pmu);
	if (ret != 0)
		return ret;

	karma_pmu->num_counters = KARMA_NR_COUNTERS;
	karma_pmu->counter_bits = 32;
	karma_pmu->dev = &pdev->dev;
	karma_pmu->check_event = KARMA_EV_ID_MAX;

	return 0;
}

static int karma_pmu_probe(struct platform_device *pdev)
{
	struct karma_pmu *karma_pmu = NULL;
	char *name = NULL;
	int ret;

	karma_pmu = devm_kzalloc(&pdev->dev, sizeof(*karma_pmu), GFP_KERNEL);
	if (karma_pmu == NULL)
		return -ENOMEM;

	platform_set_drvdata(pdev, karma_pmu);

	ret = karma_pmu_dev_probe(pdev, karma_pmu);
	if (ret != 0)
		return ret;

	ret = cpuhp_state_add_instance(g_karma_pmu_cpuhp_state,
				       &karma_pmu->node);
	if (ret != 0) {
		dev_err(&pdev->dev, "Error %d registering hotplug\n", ret);
		goto err;
	}

	name = devm_kasprintf(&pdev->dev, GFP_KERNEL, "%s_%u",
			      PMUNAME, karma_pmu->index_id);
	karma_pmu->pmu = (struct pmu) {
		.name			= name,
		.task_ctx_nr	= perf_invalid_context,
		.event_init		= karma_pmu_event_init,
		.pmu_enable		= karma_pmu_enable,
		.pmu_disable	= karma_pmu_disable,
		.add			= karma_pmu_add,
		.del			= karma_pmu_del,
		.start			= karma_pmu_start,
		.stop			= karma_pmu_stop,
		.read			= karma_pmu_read,
#ifdef CONFIG_KARMA_PMU_DEBUG
		.attr_groups	= karma_pmu_attr_groups,
#endif
	};

	ret = cpu_pm_karma_pmu_register(karma_pmu);
	if (ret != 0) {
		dev_err(karma_pmu->dev, "cpu_pm register failed!\n");
		cpuhp_state_remove_instance(g_karma_pmu_cpuhp_state,
					    &karma_pmu->node);
		goto err;
	}

	ret = perf_pmu_register(&karma_pmu->pmu, name, PERF_TYPE_KARMA);
	if (ret != 0) {
		dev_err(karma_pmu->dev, "KARMA PMU register failed!\n");
		cpu_pm_karma_pmu_unregister(karma_pmu);
		cpuhp_state_remove_instance(g_karma_pmu_cpuhp_state,
					    &karma_pmu->node);
		goto err;
	}

	return ret;

err:
	iounmap(karma_pmu->base);
	return ret;
}

static int karma_pmu_remove(struct platform_device *pdev)
{
	struct karma_pmu *karma_pmu = platform_get_drvdata(pdev);

	perf_pmu_unregister(&karma_pmu->pmu);
	cpu_pm_karma_pmu_unregister(karma_pmu);
	cpuhp_state_remove_instance(g_karma_pmu_cpuhp_state,
				    &karma_pmu->node);
	iounmap(karma_pmu->base);
	return 0;
}

static const struct of_device_id karma_pmu_match[] = {
	{ .compatible = "hisi,karma_pmu", },
	{},
};

MODULE_DEVICE_TABLE(of, karma_pmu_match);

static struct platform_driver karma_pmu_driver = {
	.driver = {
		.name = DRVNAME,
		.of_match_table = karma_pmu_match,
	},
	.probe = karma_pmu_probe,
	.remove = karma_pmu_remove,
};

static void karma_pmu_set_active_cpu(int cpu, struct karma_pmu *karma_pmu)
{
	cpumask_set_cpu(cpu, &karma_pmu->active_cpu);
}

static int karma_pmu_online_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct karma_pmu *karma_pmu = hlist_entry_safe(node, struct karma_pmu,
						     node);

	if (karma_pmu == NULL ||
	    !cpumask_test_cpu(cpu, &karma_pmu->associated_cpus))
		return 0;

	/* If another CPU is already managing this PMU, simply return. */
	if (!cpumask_empty(&karma_pmu->active_cpu))
		return 0;

	karma_pmu_set_active_cpu(cpu, karma_pmu);

	return 0;
}

static int get_online_cpu_any_but(struct karma_pmu *karma_pmu, int cpu)
{
	struct cpumask online_supported;

	cpumask_and(&online_supported,
		    &karma_pmu->associated_cpus, cpu_online_mask);
	return cpumask_any_but(&online_supported, cpu);
}

static int karma_pmu_offline_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct karma_pmu *karma_pmu = hlist_entry_safe(node, struct karma_pmu,
						     node);
	unsigned int target;

	if (karma_pmu == NULL ||
	    !cpumask_test_and_clear_cpu(cpu, &karma_pmu->active_cpu)) {
		return 0;
	}

	target = get_online_cpu_any_but(karma_pmu, cpu);
	if (target >= nr_cpu_ids)
		return 0;

	perf_pmu_migrate_context(&karma_pmu->pmu, cpu, target);
	karma_pmu_set_active_cpu(target, karma_pmu);

	return 0;
}

static int __init karma_pmu_module_init(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN,
				      DRVNAME,
				      karma_pmu_online_cpu,
				      karma_pmu_offline_cpu);
	if (ret < 0) {
		pr_err("KARMA PMU: Error setup hotplug, ret = %d\n", ret);
		return ret;
	}

	g_karma_pmu_cpuhp_state = ret;

	ret = platform_driver_register(&karma_pmu_driver);
	if (ret != 0)
		cpuhp_remove_multi_state(g_karma_pmu_cpuhp_state);

	return ret;
}
module_init(karma_pmu_module_init);

static void __exit karma_pmu_module_exit(void)
{
	platform_driver_unregister(&karma_pmu_driver);
	cpuhp_remove_multi_state(g_karma_pmu_cpuhp_state);
}
module_exit(karma_pmu_module_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("HiSilicon Karma PMU Driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
