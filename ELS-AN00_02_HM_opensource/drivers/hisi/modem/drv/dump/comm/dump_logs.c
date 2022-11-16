/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
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

#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <uapi/linux/fs.h>
#include "osl_malloc.h"
#include "bsp_dump.h"
#include "bsp_rfile.h"
#include "dump_logs.h"
#include "dump_config.h"
#include "dump_cp_agent.h"
#include "dump_apr.h"
#include "dump_area.h"
#include "dump_log_agent.h"

#undef THIS_MODU
#define THIS_MODU mod_dump

dump_log_ctrl_s g_log_notifier_ctrl;

void dump_save_file(const char *file_name, const void *addr, const void *phy_addr, u32 len)
{
    int ret;
    int fd;
    int bytes;

    UNUSED(phy_addr);
    if (file_name == NULL || addr == NULL || len == 0) {
        return;
    }
    fd = bsp_open(file_name, O_CREAT | O_RDWR | O_SYNC, DUMP_LOG_FILE_AUTH);
    if (fd < 0) {
        dump_error("fail to creat file %s \n", file_name);
        return;
    }

    bytes = bsp_write_sync(fd, addr, len);
    if (bytes != len) {
        dump_error("write data to %s failed, bytes %d, len %d\n", file_name, bytes, len);
        (void)bsp_close(fd);
        return;
    }

    ret = bsp_close(fd);
    if (ret != 0) {
        dump_error("fail to close file, ret = %d\n", ret);
        return;
    }
    return;
}

int dump_append_file(const char *dir, const char *filename, const void *address, u32 length, u32 max_size)
{
    int ret = BSP_OK;
    int fd;
    u32 bytes;
    int len;

    fd = bsp_mkdir((s8 *)dir, DUMP_LOG_DIR_AUTH);
    if (fd < 0) {
        dump_error("fail to create om dir ! ret = %d\n", fd);
        goto out;
    }

    fd = bsp_open((s8 *)filename, RFILE_APPEND | RFILE_CREAT | RFILE_RDWR, DUMP_LOG_FILE_AUTH);
    if (fd < 0) {
        dump_error("fail to open %s,while mode is create, ret = %d\n", filename, fd);
        goto out;
    }

    len = bsp_lseek((u32)fd, 0, SEEK_END);
    if (len == BSP_ERROR) {
        dump_error("fail to seek ! ret = %d\n", len);
        goto out1;
    }

    if ((len + length) >= max_size) {
        (void)bsp_close((u32)fd);
        ret = bsp_remove((s8 *)filename);
        if (ret != BSP_OK) {
            dump_error("fail to remove ! ret = %d\n", ret);
            goto out;
        }

        fd = bsp_open((s8 *)filename, RFILE_CREAT | RFILE_RDWR, DUMP_LOG_FILE_AUTH);
        if (fd < 0) {
            dump_error("fail to create %s ! ret = %d\n", filename, fd);
            goto out;
        }
    }

    bytes = (u32)bsp_write_sync((u32)fd, address, length);
    if (bytes != length) {
        dump_error("fail to write data ! ret = %d\n", bytes);
        ret = BSP_ERROR;
        goto out1;
    }

    (void)bsp_close((u32)fd);

    return BSP_OK;

out1:
    (void)bsp_close((u32)fd);
out:
    return ret;
}

int dump_log_save(const char *dir, const char *file_name, const void *address, const void *phy_addr, u32 length)
{
    bool link_enble = dump_get_link_status();
    char dst_name[MODEM_DUMP_FILE_NAME_LENGTH] = {
        0,
    };
    struct dump_file_save_strategy log_strategy;
    s32 ret = BSP_ERROR;

    if (dir == NULL || file_name == NULL || address == NULL || length == 0) {
        dump_error("input err\n");
        return BSP_ERROR;
    }
    if (false == link_enble) {
        goto no_link;
    }
    /* log append */
    ret = dump_match_log_strategy(file_name, &log_strategy);
    if (ret) {
        goto no_link;
    }
    log_strategy.data_head.filelength = length;
    /* append log */
    if (0 >
        snprintf_s(dst_name, sizeof(dst_name), sizeof(dst_name) - 1, "%s%s", dir, log_strategy.data_head.dstfilename)) {
        bsp_info("snp err,no link\n");
        goto no_link;
    }
    /* append head */
    if (dump_append_file(dir, dst_name, &log_strategy.data_head, sizeof(log_strategy.data_head), 0xFFFFFFFF)) {
        dump_error("add head err\n");
        return BSP_ERROR;
    }
    /* append data */
    if (dump_append_file(dir, dst_name, address, log_strategy.data_head.filelength, 0xFFFFFFFF)) {
        dump_error("add data err\n");
        return BSP_ERROR;
    }
    return BSP_OK;
no_link:
    if (0 > snprintf_s(dst_name, sizeof(dst_name), sizeof(dst_name) - 1, "%s%s", dir, file_name)) {
        dump_error("snpf err\n");
        return BSP_ERROR;
    }
    dump_save_file(dst_name, address, phy_addr, length);
    return BSP_OK;
}
/*
 * 功能描述: 创建文件接口
 */
int dump_create_file(const char *filename)
{
    int fd;

    if (!filename) {
        return BSP_ERROR;
    }
    fd = bsp_open(filename, O_CREAT | O_RDWR | O_SYNC, DUMP_LOG_FILE_AUTH);
    if (fd < 0) {
        dump_error("fail to creat file %s \n", filename);
        return BSP_ERROR;
    }

    (void)bsp_close(fd);
    return BSP_OK;
}

/*
 * 功能描述: 创建cp_log的目录保证hidp等工具能够正常导log
 */
int dump_create_dir(char *path)
{
    int fd;
    char *dir_name = NULL;
    u32 len;

    if (path == NULL) {
        return BSP_ERROR;
    }
    dump_print("create dir path is %s\n", path);

    /* rfile创建目录不允许使用/结束路径 */
    len = strlen(path);
    if (path[len - 1] == '/') {
        dir_name = kmalloc((len + 1), GFP_KERNEL);
        if (dir_name == NULL) {
            dump_error("fail to malloc memry \n");
            return BSP_ERROR;
        }
        if (EOK != memset_s(dir_name, (len + 1), '\0', (len + 1))) {
            bsp_debug("err\n");
        }
        if (EOK != memcpy_s(dir_name, (len + 1), path, (len - 1))) {
            bsp_debug("err\n");
        }
        fd = bsp_mkdir(dir_name, DUMP_LOG_DIR_AUTH);
    } else {
        fd = bsp_mkdir(path, DUMP_LOG_DIR_AUTH);
    }
    if (dir_name != NULL) {
        kfree(dir_name);
    }
    if (fd < 0) {
        dump_error("fail to create dir ,fd = %d\n ", fd);
        return BSP_ERROR;
    }
    return BSP_OK;
}

/*
 * 功能描述: log保存时初始化
 */
void dump_save_log_notifier_init(void)
{
    if (g_log_notifier_ctrl.init_flag == true) {
        return;
    }
    spin_lock_init(&g_log_notifier_ctrl.lock);

    INIT_LIST_HEAD(&g_log_notifier_ctrl.log_list);

    g_log_notifier_ctrl.init_flag = true;

    return;
}

/*
 * 功能描述: 注册异常时需要保存的log
 */
/*lint -save -e429*/
s32 bsp_dump_register_log_notifier(u32 modem_type, log_save_fun save_fun, const char *name)
{
    unsigned long flags;
    dump_log_notifier_s *log_notifier = NULL;
    if (name == NULL || save_fun == NULL) {
        dump_error("input error param\n");
        return BSP_ERROR;
    }
    if (g_log_notifier_ctrl.init_flag != true) {
        dump_save_log_notifier_init();
    }

    log_notifier = osl_malloc(sizeof(dump_log_notifier_s));
    if (log_notifier == NULL) {
        dump_error("fail to malloc space\n");
        return BSP_ERROR;
    }
    log_notifier->save_fun = save_fun;
    log_notifier->modem_type = modem_type;
    if (EOK != memcpy_s(log_notifier->name, sizeof(log_notifier->name), name, strlen(name))) {
        bsp_debug("err\n");
    }
    spin_lock_irqsave(&g_log_notifier_ctrl.lock, flags);
    list_add(&log_notifier->list, &g_log_notifier_ctrl.log_list);
    spin_unlock_irqrestore(&g_log_notifier_ctrl.lock, flags);
    return BSP_OK;
}

/*lint -restore -e429*/
/*
 * 功能描述: 去注册异常时需要保存的log
 */
s32 bsp_dump_unregister_log_notifier(log_save_fun save_fun)
{
    unsigned long flags;
    dump_log_notifier_s *log_notifier = NULL;
    dump_log_notifier_s *log_notifier_node = NULL;

    if (g_log_notifier_ctrl.init_flag != true) {
        return BSP_ERROR;
    }

    spin_lock_irqsave(&g_log_notifier_ctrl.lock, flags);
    list_for_each_entry(log_notifier, &g_log_notifier_ctrl.log_list, list)
    {
        if (log_notifier->save_fun == save_fun) {
            log_notifier_node = log_notifier;
        }
    }
    if (log_notifier_node == NULL) {
        spin_unlock_irqrestore(&g_log_notifier_ctrl.lock, flags);
        return BSP_ERROR;
    }
    list_del(&log_notifier_node->list);
    osl_free(log_notifier_node);
    spin_unlock_irqrestore(&g_log_notifier_ctrl.lock, flags);

    return BSP_OK;
}

/*
 * 功能描述: 执行需要保存的log
 */
void bsp_dump_log_notifer_callback(u32 modem_type, const char *path)
{
    struct list_head *p = NULL;
    struct list_head *n = NULL;
    dump_log_notifier_s *log_notifier = NULL;
    uintptr_t fun_addr = 0;

    if (g_log_notifier_ctrl.init_flag == false) {
        return;
    }

    if (list_empty(&g_log_notifier_ctrl.log_list)) {
        return;
    }

    list_for_each_safe(p, n, &g_log_notifier_ctrl.log_list)
    {
        log_notifier = list_entry(p, dump_log_notifier_s, list);
        if ((log_notifier != NULL) && (log_notifier->save_fun != NULL) && (log_notifier->modem_type & modem_type)) {
            fun_addr = (uintptr_t)(void *)(log_notifier->save_fun);
            fun_addr = ((fun_addr & 0xffffffff00000000) >> 32) & 0xffffffff;
            log_notifier->save_fun(path);
        }
    }
}
int bsp_dump_save_file(const char *dir, const char *file_name, const void *addr, const void *phy_addr, u32 len)
{
    return dump_log_save(dir, file_name, addr, phy_addr, len);
}
