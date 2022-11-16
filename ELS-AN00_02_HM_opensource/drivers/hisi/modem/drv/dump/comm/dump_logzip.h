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

#ifndef __DUMP_LOGZIP_H__
#define __DUMP_LOGZIP_H__

#include <linux/miscdevice.h>
#include <linux/suspend.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/err.h>
#include <linux/syscalls.h>
#include "dump_log_agent.h"

#define COMPRESS_FILE_PATH_LEN 256
#define MAX_COMPRESS_FILES 10
#define MAX_COMPRESS_FILE_NAME 32
#define DUMP_COMPRESS_TIMEOUT_CNTS 400
#define DUMP_COMPRESS_WAIT_PER_CNT 50 /* 50ms */

struct zipintf_info_s {
    char mfilepath[COMPRESS_FILE_PATH_LEN];
    u32 mfilenum;
    char pfile_list[MAX_COMPRESS_FILES][MAX_COMPRESS_FILE_NAME];
    struct dump_file_save_info_s saveinfo;
};

struct comp_log_s {
    struct zipintf_info_s *zip_info;
    wait_queue_head_t wq;   /* The wait queue for reader */
    struct miscdevice misc; /* The "misc" device representing the log */
    struct mutex mutex;     /* The mutex that protects the @buffer */
    struct list_head logs;  /* The list of log channels */
    u32 fopen_cnt;
    u32 trigger_flag;
};

struct dump_zip_stru {
    struct zipintf_info_s zipintf_info;
    struct comp_log_s *comp_log_ctrl;
};
s32 dump_trigger_compress(const char *logpath, int pathlen, struct dump_file_save_info_s *datainfo);
void dump_wait_compress_done(const char *log_path);
void dump_logzip_init(void);
#endif
