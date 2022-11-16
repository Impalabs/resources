/*
 * asp_codec_single_route.c -- asp codec single route driver
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 */

#include "asp_codec_single_route.h"

#include "audio_log.h"

#ifdef CONFIG_AUDIO_DEBUG
#define LOG_TAG "asp_codec"
#else
#define LOG_TAG "Analog_less_v1"
#endif

#define UP_SWITCH_ROUTE \
	{ "Master Capture",        "Switch", "S4_RX_INPUT" } \

#define DN_SWITCH_ROUTE \
	{ "Master Playback",       "Switch", "CODEC3_DN_FIFO_INPUT" }, \
	{ "Master Playback",       "Switch", "AUDIO_DN_FIFO_INPUT" } \

#define I2S_SWITCH_ROUTE \
	{ "Master Playback",   "Switch", "S1_RX_INPUT" } \

#define UP_OUT_ROUTE \
	{ "MDM5G_UP_OUTPUT",        NULL, "Master Capture" }, \
	{ "SMARTPA_UP_FIFO_OUTPUT", NULL, "Master Playback" } \

#define I2S_OUT_ROUTE \
	{ "S1_TX_DRV",        NULL, "Master Playback" }

static const struct snd_soc_dapm_route route_map[] = {
	UP_SWITCH_ROUTE,
	DN_SWITCH_ROUTE,
	I2S_SWITCH_ROUTE,
	UP_OUT_ROUTE,
	I2S_OUT_ROUTE,
};

int asp_codec_add_single_routes(struct snd_soc_dapm_context *dapm)
{
	if (dapm == NULL) {
		AUDIO_LOGE("dapm is null");
		return -EINVAL;
	}

	return snd_soc_dapm_add_routes(dapm, route_map, ARRAY_SIZE(route_map));
}

