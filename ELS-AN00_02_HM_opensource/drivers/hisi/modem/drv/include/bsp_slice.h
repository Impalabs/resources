/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __BSP_SLICE_H__
#define __BSP_SLICE_H__

#include <product_config.h>
#include <osl_bio.h>
#include <osl_types.h>

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#ifndef U32_MAX_VALUE
#define U32_MAX_VALUE                   (0xFFFFFFFF)
#endif
#ifndef SLICE_CONVERT_DELTA
#define SLICE_CONVERT_DELTA              (1000u) /* 1秒=1000毫秒,1毫秒=1000微秒 */
#endif
#ifndef U32_BIT_MAX
#define U32_BIT_MAX (32u)
#endif

#define SLICE_MSG_INIT 0xA5A3BC55
#define SLICE_MSG_UPDATE 0x55A3BCAA
#define SLICE_MSG_REQUEST  0xABCA5AAB
struct slice_pcie_msg {
    u64 slice;
    u32 msg_type;
    u32 reserved;
};
#ifndef __HIBOOT__
/*
 * 函 数 名  : get_timer_slice_delta
 * 功能描述  : 打点计时，直接获取时间戳前后差值
 * 输入参数  :  begin:开始时间戳
 *              end  :结束时间戳
 * 返 回 值  : 时间戳前后差值，单位为1
 * 修改记录  : 2013年1月8日   
 */
/*lint -esym(666,get_timer_slice_delta)*/
#define get_timer_slice_delta(begin, end) (((end) >= (begin)) ? ((end) - (begin)) : ((0xFFFFFFFF - (begin)) + (end)))

/*
 * 函 数 名  : print_stamp
 * 功能描述  : 打点计时记录到用户传入的地址内
 * 输入参数  : addr:打点记录地址
 * 修改记录  : 2015年2月26日   
 */
#define print_stamp(addr) writel(bsp_get_slice_value(), addr)
#define print_stamp_hrt(addr) writel(bsp_get_slice_value_hrt(), addr)
#ifdef CONFIG_MODULE_TIMER
#ifndef __KERNEL__
void udelay(unsigned us);
#endif

/*
 * 函 数 名  : bsp_get_slice_value
 * 功能描述  : 获取时间戳
 * 返 回 值  : 时间戳定时器计数值，只在p531 asic上返回timer时间戳，
 *                        其他返回系统控制器32k 时间戳，递增
 * 修改记录  : 2013年1月8日   
 */
u32 bsp_get_slice_value(void);

/*
 * 函 数 名  : bsp_get_slice_value_hrt
 * 功能描述  : 获取19.2M 高精度时间戳
 * 返 回 值  : 19.2M 时间戳，递增
 * 修改记录  : 2013年1月8日   
 */
u32 bsp_get_slice_value_hrt(void);

/*
 * 函 数 名  : bsp_get_httimer_freq
 * 功能描述  : 获取高精度时间戳timer的频率
 * 返 回 值  : 频率 单位赫兹
 * 修改记录  : 2013年1月8日   
 */
u32 bsp_get_hrtimer_freq(void);

/*
 * 函 数 名  : bsp_get_elapse_ms
 * 功能描述  : 打点计时，以毫秒为单位返回系统启动到当前时间
 * 返 回 值  : 以毫秒为单位的从系统启动到当前所流逝的时间
 * 修改记录  : 2013年6月20日   
 */
u32 bsp_get_elapse_ms(void);
void* bsp_get_stamp_addr(void);
void* bsp_get_stamp_addr_phy(void);
u32 bsp_slice_getcurtime(u64 *pcurtime);
u32 bsp_slice_getcurtime_hrt_ns(u64 *pcurtime_ns);
u32 bsp_slice_getcurtime_hrt(u64 *pcurtime);
u32 bsp_get_slice_freq(void);
void slice_resume(void);
void slice_resume_mdm(void);
void slice_resume_mdm5g(void);
int bsp_slice_init(void);
void hrt_slice_resume(void);
void bsp_ab_sync_slice_getcurtime(u64 *pcurtime);
#else
static inline int bsp_slice_init(void)
{
    return 0;
}
static inline u32 bsp_get_slice_value(void)
{
    return 0;
}
static inline u32 bsp_get_slice_value_hrt(void)
{
    return 0;
}
static inline u32 bsp_get_elapse_ms(void)
{
    return 0;
}
static inline void* bsp_get_stamp_addr(void)
{
    return NULL;
}
static inline void* bsp_get_stamp_addr_phy(void)
{
    return NULL;
}
static inline u32 bsp_slice_getcurtime(u64 *pcurtime)
{
    return 0;
}
static inline u32 bsp_slice_getcurtime_hrt_ns(u64 *pcurtime_ns)
{
    return 0;
}
static inline u32 bsp_slice_getcurtime_hrt(u64 *pcurtime)
{
    return 0;
}
static inline u32 bsp_get_slice_freq(void)
{
    return 32764; /* 32764: 32K时钟频率 */
}
static inline u32 bsp_get_hrtimer_freq(void)
{
    return 19200000; /* 19200000: 19.2M时钟频率 */
}
static inline void slice_resume(void)
{
    return;
};
static inline void slice_resume_mdm(void)
{
    return;
};
static inline void slice_resume_mdm5g(void)
{
    return;
};
static inline void bsp_ab_sync_slice_getcurtime(u64 *pcurtime)
{
    return;
};
#ifndef __KERNEL__
static inline void udelay(unsigned us)
{
    return;
}
#endif
static inline void hrt_slice_resume(void)
{
    return;
}
#endif /* CONFIG_MODULE_TIMER */
#endif /* __HIBOOT__ */

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif /* __BSP_SLICE_H__ */
