/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
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
#ifndef _BSP_RELAY_H_
#define _BSP_RELAY_H_

#include <osl_types.h>
#include <mdrv_udi.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum relay_type_id {
    RELAY_DEV_CTRL,
    RELAY_DEV_AT,
    RELAY_DEV_SHELL,
    RELAY_DEV_LTE_DIAG,
    RELAY_DEV_OM,
    RELAY_DEV_MODEM,
    RELAY_DEV_GPS,
    RELAY_DEV_3G_GPS,
    RELAY_DEV_3G_PCVOICE,
    RELAY_DEV_PCVOICE,
    RELAY_DEV_SKYTONE,
    RELAY_DEV_CDMA_LOG,
    RELAY_DEV_UART,

    RELAY_INVAL_DEV_ID = 0xFFFF
};

struct relay_write_info {
    char *virt_addr;
    char *phy_addr;
    unsigned int size;
    void *priv;
};

enum relay_evt {
    RELAY_EVT_DEV_SUSPEND = 0,
    RELAY_EVT_DEV_READY = 1,
    RELAY_EVT_DEV_BOTTOM
};

struct relay_read_buff_info {
    unsigned int buff_size;
    unsigned int buff_num;
};


typedef void (*usb_modem_msc_read_cb)(struct modem_msc_stru *modem_msc);
typedef void (*usb_modem_rel_ind_cb)(unsigned int modem_enable);


typedef void (*relay_enable_cb)(unsigned int relay_id, void *private);
typedef void (*relay_disable_cb)(unsigned int relay_id, void *private);
typedef int (*relay_read_cb)(unsigned int relay_id, void *private, char *addr, int size);
typedef void (*relay_write_donecb)(char *virt_addr, char *phy_addr, int size);
typedef void (*relay_event_cb)(enum relay_evt port_state);

int bsp_relay_reg_enablecb(unsigned int relay_id, relay_enable_cb func, void *private);
int bsp_relay_reg_disablecb(unsigned int relay_id, relay_disable_cb func);
int bsp_relay_open(unsigned int relay_id);
int bsp_relay_close(unsigned int relay_id);
int bsp_relay_write(unsigned int relay_id, char *addr, unsigned int size);
int bsp_relay_ioctl(unsigned int relay_id, unsigned int cmd, void *para);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BSP_RELAY_H_ */
