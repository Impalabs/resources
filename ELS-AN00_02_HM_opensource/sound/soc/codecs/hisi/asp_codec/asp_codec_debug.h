/*
 * asp_codec_debug.h -- asp codec debug define
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 */

#ifndef __ASP_CODEC_DEBUG_H__
#define __ASP_CODEC_DEBUG_H__

#include "soc_acpu_baseaddr_interface.h"
#include "asp_codec_utils.h"

#define PAGE_SOCCODEC_BASE_ADDR (SOC_ACPU_ASP_CODEC_BASE_ADDR)

#define DBG_SOCCODEC_START_ADDR \
	(PAGE_SOCCODEC_BASE_ADDR + SOCCODEC_START_OFFSET)
#define DBG_SOCCODEC_END_ADDR (PAGE_SOCCODEC_BASE_ADDR + SOCCODEC_END_OFFSET)
#endif

