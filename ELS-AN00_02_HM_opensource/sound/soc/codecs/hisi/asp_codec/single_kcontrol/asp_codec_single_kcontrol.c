/*
 * asp_codec_single_kcontrol.c -- asp codec single kcontrol driver
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 */

#include "asp_codec_single_kcontrol.h"

#include <sound/core.h>
#include "audio_log.h"
#include "asp_codec_utils.h"
#include "asp_codec_regs.h"

#ifdef CONFIG_AUDIO_DEBUG
#define LOG_TAG "asp_codec"
#else
#define LOG_TAG "Analog_less_v1"
#endif

#define MAX_VOLUME 100
#define MIN_VOLUME 0
#define MIN_GAIN_DB (-50)
#define MAX_GAIN_DB 0

static signed char convert_volume_to_gain(int volume)
{
	signed char gain;

	if (volume > MAX_VOLUME)
		volume = MAX_VOLUME;
	else if(volume < MIN_VOLUME)
		volume = MIN_VOLUME;

	gain = volume * (MAX_GAIN_DB - MIN_GAIN_DB) / (MAX_VOLUME - MIN_VOLUME) + MIN_GAIN_DB;

	return gain;
}

static int convert_gain_to_volume(signed char gain)
{
	int volume;

	if (gain > MAX_GAIN_DB)
		gain = MAX_GAIN_DB;
	else if (gain < MIN_GAIN_DB)
		gain = MIN_GAIN_DB;

	volume = (gain - MIN_GAIN_DB) * (MAX_VOLUME - MIN_VOLUME) / (MAX_GAIN_DB - MIN_GAIN_DB);

	return volume;
}

static int master_playback_volume_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *codec = NULL;
	unsigned int reg_value;
	signed char gain;

	IN_FUNCTION;

	codec = snd_soc_kcontrol_component(kcontrol);
	reg_value = asp_codec_reg_read(codec, AUDIO_L_DN_PGA_CTRL_REG);
	gain = reg_value >> AUDIO_L_DN_PGA_GAIN_OFFSET;
	ucontrol->value.integer.value[0] = convert_gain_to_volume(gain);

	OUT_FUNCTION;

	return 0;
}

static int master_playback_volume_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	signed char gain;

	IN_FUNCTION;

	gain = convert_volume_to_gain(ucontrol->value.integer.value[0]);

	asp_codec_reg_update(AUDIO_L_DN_PGA_CTRL_REG,
			MAX_VAL_ON_BIT(AUDIO_L_DN_PGA_GAIN_LEN) << AUDIO_L_DN_PGA_GAIN_OFFSET,
			gain << AUDIO_L_DN_PGA_GAIN_OFFSET);
	asp_codec_reg_update(AUDIO_R_DN_PGA_CTRL_REG,
			MAX_VAL_ON_BIT(AUDIO_R_DN_PGA_GAIN_LEN) << AUDIO_R_DN_PGA_GAIN_OFFSET,
			gain << AUDIO_R_DN_PGA_GAIN_OFFSET);
	asp_codec_reg_update(CODEC3_L_DN_PGA_CTRL_REG,
			MAX_VAL_ON_BIT(CODEC3_L_DN_PGA_GAIN_LEN) << CODEC3_L_DN_PGA_GAIN_OFFSET,
			gain << CODEC3_L_DN_PGA_GAIN_OFFSET);
	asp_codec_reg_update(CODEC3_R_DN_PGA_CTRL_REG,
			MAX_VAL_ON_BIT(CODEC3_R_DN_PGA_GAIN_LEN) << CODEC3_R_DN_PGA_GAIN_OFFSET,
			gain << CODEC3_R_DN_PGA_GAIN_OFFSET);

	OUT_FUNCTION;

	return 0;
}

static int capture_volume_get(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *codec = NULL;
	unsigned int reg_value;
	signed char gain;

	IN_FUNCTION;

	codec = snd_soc_kcontrol_component(kcontrol);
	reg_value = asp_codec_reg_read(codec, MDM_5G_L_UP_PGA_CTRL_REG);
	gain = reg_value >> MDM_5G_L_UP_PGA_GAIN_OFFSET;
	ucontrol->value.integer.value[0] = convert_gain_to_volume(gain);

	OUT_FUNCTION;

	return 0;
}

static int capture_volume_put(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	signed char gain;

	IN_FUNCTION;

	gain = convert_volume_to_gain(ucontrol->value.integer.value[0]);

	asp_codec_reg_update(MDM_5G_L_UP_PGA_CTRL_REG,
			MAX_VAL_ON_BIT(MDM_5G_L_UP_PGA_GAIN_LEN) << MDM_5G_L_UP_PGA_GAIN_OFFSET,
			gain << MDM_5G_L_UP_PGA_GAIN_OFFSET);
	asp_codec_reg_update(MDM_5G_R_UP_PGA_CTRL_REG,
			MAX_VAL_ON_BIT(MDM_5G_R_UP_PGA_GAIN_LEN) << MDM_5G_R_UP_PGA_GAIN_OFFSET,
			gain << MDM_5G_R_UP_PGA_GAIN_OFFSET);

	OUT_FUNCTION;

	return 0;
}

#define MIXER_PGA_GIN_KCONTROLS \
	SOC_SINGLE_EXT("Master Playback Volume", \
		FS_CTRL3_REG, FS_I2S2_RX_R_PGA_OFFSET, \
		0xFFFF, 0, master_playback_volume_get, master_playback_volume_put), \
	SOC_SINGLE_EXT("Master Capture Volume", \
		FS_CTRL3_REG, FS_I2S2_RX_R_PGA_OFFSET, \
		0xFFFF, 0, capture_volume_get, capture_volume_put) \

static const struct snd_kcontrol_new snd_controls[] = {
	MIXER_PGA_GIN_KCONTROLS
};

int asp_codec_add_single_kcontrols(struct snd_soc_component *codec)
{
	if (codec == NULL) {
		AUDIO_LOGE("codec is null");
		return -EINVAL;
	}

	return snd_soc_add_component_controls(codec, snd_controls,
		ARRAY_SIZE(snd_controls));
}

