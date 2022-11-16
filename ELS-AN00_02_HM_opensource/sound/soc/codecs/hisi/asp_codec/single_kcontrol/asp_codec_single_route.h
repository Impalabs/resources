/*
 * asp_codec_single_route.h -- asp codec single route driver
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 */

#ifndef __ASP_CODEC_SINGLE_ROUTE_H__
#define __ASP_CODEC_SINGLE_ROUTE_H__
#include <sound/soc.h>

int asp_codec_add_single_routes(struct snd_soc_dapm_context *dapm);
#endif

