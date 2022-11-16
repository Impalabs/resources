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

#ifndef HI_MAA_V1_H
#define HI_MAA_V1_H

#define MAA_GLB_CTRL 0x0000
#define MAA_AXI_BUS_CFG 0x0004
#define MAA_VERSION 0x0008
#define MAA_LP_EN 0x000C
#define MAA_LP_STAT 0x0010
#define MAA_OPIPE_STAT 0x0018
#define MAA_IPIPE_STAT0 0x001C
#define MAA_IPIPE_STAT1 0x0020
#define MAA_OPIPE_INT_MSK 0x0024
#define MAA_OPIPE_INT_STAT 0x0028
#define MAA_OPIPE_INT_RAW 0x002C

#define MAA_RLS_FULL_WR_INT_MASK BIT(18)
#define MAA_SAFE_CHK_INT_MASK BIT(17)
#define MAA_DDR_RLS_INT_MASK BIT(16)
#define MAA_IPIPE_INT_MASK BIT(1)
#define MAA_INT_SEC_RLS_MASK (MAA_RLS_FULL_WR_INT_MASK | MAA_SAFE_CHK_INT_MASK | MAA_DDR_RLS_INT_MASK)

#define MAA_IPIPE_INT_RAW0 0x0030
#define MAA_IPIPE_INT_MSK0 0x0034
#define MAA_IPIPE_INT_STAT0 0x0038
#define MAA_IPIPE_INT_RAW1 0x003C
#define MAA_IPIPE_INT_MSK1 0x0040
#define MAA_IPIPE_INT_STAT1 0x0044
#define MAA_IPIPE_INT_RAW2 0x0048
#define MAA_IPIPE_INT_MSK2 0x004C
#define MAA_IPIPE_INT_STAT2 0x0050
#define MAA_IPIPE_INT_RAW3 0x0054
#define MAA_IPIPE_INT_MSK3 0x0058
#define MAA_IPIPE_INT_STAT3 0x005C
#define MAA_IPIPE_INT_RAW4 0x0060
#define MAA_IPIPE_INT_MSK4 0x0064
#define MAA_IPIPE_INT_STAT4 0x0068
#define MAA_IPIPE_INT_RAW5 0x006C
#define MAA_IPIPE_INT_MSK5 0x0070
#define MAA_IPIPE_INT_STAT5 0x0074
#define MAA_TIMER_EN 0x0090
#define MAA_TIMER_WATCHCNT 0x0094
#define MAA_ALLOC_ADDR_L(opipes, src_ptrs) (0x200 + 0x80 * (opipes) + 0x8 * (src_ptrs))
#define MAA_ALLOC_ADDR_H(opipes, src_ptrs) (0x204 + 0x80 * (opipes) + 0x8 * (src_ptrs))
#define MAA_RLS_POOL_ADDR_L 0x500
#define MAA_RLS_POOL_ADDR_H 0x504
#define MAA_RLS_POOL_DEPTH 0x508
#define MAA_RLS_POOL_WPTR 0x50C
#define MAA_RLS_POOL_RPTR 0x510
#define MAA_RLS_POOL_UP_THRH 0x514
#define MAA_ACORE_ALLOC_CNT(levels) (0x900 + 0x80 * (levels))
#define MAA_ALLOC_FIFO_CNT(levels) (0x904 + 0x80 * (levels))
#define MAA_ALLOC_MAA_CNT(levels) (0x90C + 0x80 * (levels))
#define MAA_ALLOC_DESTN_CNT(destns, levels) (0x910 + 0x4 * (destns) + 0x80 * (levels))
#define MAA_DESTN_ALLOC_CNT(destns, levels) (0x930 + 0x4 * (destns) + 0x80 * (levels))
#define MAA_RLS_CNT(levels) (0x960 + 0x80 * (levels))
#define MAA_RLS_ABANDON_CNT 0xC00
#define MAA_CNT_CLK_EN 0xD00
#define MAA_CNT_CLR 0xD04
#define MAA_HAC_BP_DBG 0x1000
#define MAA_HAC_DATA_DBG 0x1004
#define MAA_HAC_PUSH_DBG 0x1008
#define MAA_HAC_DBG 0x100C
#define MAA_GS_DBG 0x1010
#define MAA_HAC_BP_INVALID_DATA_L 0x1018
#define MAA_HAC_BP_INVALID_DATA_H 0x101C
#define MAA_ALLOC_ADDR_RD_L(opipes, src_ptrs) (0x1100 + 0x100 * (opipes) + 0x8 * (src_ptrs))
#define MAA_ALLOC_ADDR_RD_H(opipes, src_ptrs) (0x1104 + 0x100 * (opipes) + 0x8 * (src_ptrs))
#define MAA_OPIPE_SECCTRL 0x3000
#define MAA_IPIPE_SECCTRL0 0x3008
#define MAA_IPIPE_SECCTRL1 0x300C
#define MAA_IPIPE_SECCTRL2 0x3010
#define MAA_MAA_OPIPE_MID 0x3014
#define MAA_MAA_IPIPE_MID0 0x3018
#define MAA_MAA_IPIPE_MID1 0x301C
#define MAA_ALLOC_ADDR_STR_L(sec_rgns) (0x3020 + 0x10 * (sec_rgns))
#define MAA_ALLOC_ADDR_STR_H(sec_rgns) (0x3024 + 0x10 * (sec_rgns))
#define MAA_ALLOC_ADDR_END_L(sec_rgns) (0x3028 + 0x10 * (sec_rgns))
#define MAA_ALLOC_ADDR_END_H(sec_rgns) (0x302C + 0x10 * (sec_rgns))
#define MAA_ALLOC_ADDR_PT_CTRL 0x3100
#define MAA_PTR_SIZE0 0x3104
#define MAA_PTR_SIZE1 0x3108
#define MAA_PTR_SIZE2 0x310C
#define MAA_PTR_SIZE3 0x3110
#define MAA_PTR_SIZE4 0x3114
#define MAA_PTR_SIZE5 0x3118
#define MAA_OPIPE_BASE_ADDR_L(opipes) (0x4000 + 0x80 * (opipes))
#define MAA_OPIPE_BASE_ADDR_H(opipes) (0x4004 + 0x80 * (opipes))
#define MAA_OPIPE_DEPTH(opipes) (0x4008 + 0x80 * (opipes))
#define MAA_OPIPE_WPTR(opipes) (0x400C + 0x80 * (opipes))
#define MAA_OPIPE_RPTR(opipes) (0x4010 + 0x80 * (opipes))
#define MAA_OPIPE_UP_THRH(opipes) (0x4020 + 0x80 * (opipes))
#define MAA_OPIPE_DN_THRH(opipes) (0x4024 + 0x80 * (opipes))
#define MAA_OPIPE_SPACE(opipes) (0x4028 + 0x80 * (opipes))
#define MAA_OPIPE_EN(opipes) (0x4040 + 0x80 * (opipes))
#define MAA_OPIPE_CFG_DONE(opipes) (0x4044 + 0x80 * (opipes))
#define MAA_OPIPE_DBG1(opipes) (0x4050 + 0x80 * (opipes))
#define MAA_OPIPE_DBG0(opipes) (0x4054 + 0x80 * (opipes))
#define MAA_IPIPE_BASE_ADDR_L(ipipes) (0x6000 + 0x80 * (ipipes))
#define MAA_IPIPE_BASE_ADDR_H(ipipes) (0x6004 + 0x80 * (ipipes))
#define MAA_IPIPE_DEPTH(ipipes) (0x6008 + 0x80 * (ipipes))
#define MAA_IPIPE_WPTR(ipipes) (0x600C + 0x80 * (ipipes))
#define MAA_IPIPE_RPTR(ipipes) (0x6010 + 0x80 * (ipipes))
#define MAA_IPIPE_THRH(ipipes) (0x6014 + 0x80 * (ipipes))
#define MAA_IPIPE_PTR_ADDR_L(ipipes) (0x6018 + 0x80 * (ipipes))
#define MAA_IPIPE_PTR_ADDR_H(ipipes) (0x601C + 0x80 * (ipipes))
#define MAA_IPIPE_CTRL(ipipes) (0x604C + 0x80 * (ipipes))
#define MAA_IPIPE_DBG1(ipipes) (0x6050 + 0x80 * (ipipes))
#define MAA_IPIPE_DBG0(ipipes) (0x6054 + 0x80 * (ipipes))

#endif
