/*
 * analog_less_machine.c
 *
 * analog_less_machine driver
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

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/of.h>
#include <sound/soc.h>

#include "audio_log.h"

#define LOG_TAG "analog_less_machine"

#define DAI_LINK_CODEC_NAME          "asp-codec-codecless"
#define DAI_LINK_CODEC_DAI_NAME      "asp-codec-codecless-dai"
#define ANALOG_LESS_MACHINE_DRIVER_NAME "analog_less_machine"
#define ANALOG_LESS_MACHINE_CARD_NAME   "analog_less_machine_card"

static struct snd_soc_dai_link g_analog_less_dai_link[] = {
	{
		.name           = "analog_less_machine_pb_normal",
		.stream_name    = "analog_less_machine_pb_normal",
		.codec_name     = DAI_LINK_CODEC_NAME,
		.cpu_dai_name   = "asp-pcm-mm",
		.codec_dai_name = DAI_LINK_CODEC_DAI_NAME,
		.platform_name  = "asp-pcm-hifi",
	},
	{
		.name           = "analog_less_machine_voice",
		.stream_name    = "analog_less_machine_voice",
		.codec_name     = DAI_LINK_CODEC_NAME,
		.cpu_dai_name   = "asp-pcm-modem",
		.codec_dai_name = DAI_LINK_CODEC_DAI_NAME,
		.platform_name  = "snd-soc-dummy",
	},
	{
		.name           = "analog_less_machine_fm1",
		.stream_name    = "analog_less_machine_fm1",
		.codec_name     = DAI_LINK_CODEC_NAME,
		.cpu_dai_name   = "asp-pcm-fm",
		.codec_dai_name = DAI_LINK_CODEC_DAI_NAME,
		.platform_name  = "snd-soc-dummy",
	},
	{
		.name           = "analog_less_machine_pb_dsp",
		.stream_name    = "analog_less_machine_pb_dsp",
		.codec_name     = DAI_LINK_CODEC_NAME,
		.cpu_dai_name   = "asp-pcm-lpp",
		.codec_dai_name = DAI_LINK_CODEC_DAI_NAME,
		.platform_name  = "asp-pcm-hifi",
	},
	{
		.name           = "analog_less_machine_pb_direct",
		.stream_name    = "analog_less_machine_pb_direct",
		.codec_name     = DAI_LINK_CODEC_NAME,
		.cpu_dai_name   = "asp-pcm-direct",
		.codec_dai_name = DAI_LINK_CODEC_DAI_NAME,
		.platform_name  = "asp-pcm-hifi",
	},
	{
		.name           = "analog_less_machine_lowlatency",
		.stream_name    = "analog_less_machine_lowlatency",
		.codec_name     = DAI_LINK_CODEC_NAME,
		.cpu_dai_name   = "asp-pcm-fast",
		.codec_dai_name = DAI_LINK_CODEC_DAI_NAME,
#ifdef AUDIO_LOW_LATENCY_LEGACY
		.platform_name  = "hi3xxx-pcm-asp-dma",
#else
		.platform_name  = "asp-pcm-hifi",
#endif
	},
	{
		.name           = "analog_less_machine_mmap",
		.stream_name    = "analog_less_machine_mmap",
		.codec_name     = DAI_LINK_CODEC_NAME,
		.cpu_dai_name   = "audio-pcm-mmap",
		.codec_dai_name = DAI_LINK_CODEC_DAI_NAME,
		.platform_name  = "asp-pcm-hifi",
	},
};

static struct snd_soc_card g_analog_less_card = {
	.name = ANALOG_LESS_MACHINE_CARD_NAME,
	.owner = THIS_MODULE,
	.dai_link = g_analog_less_dai_link,
	.num_links = ARRAY_SIZE(g_analog_less_dai_link),
};

static int analog_less_probe(struct platform_device *pdev)
{
	int ret;
	struct snd_soc_card *card = &g_analog_less_card;

	if (!pdev) {
		AUDIO_LOGE("pdev is null, fail");
		return -EINVAL;
	}

	AUDIO_LOGI("analog less probe");

	card->dev = &pdev->dev;

	ret = snd_soc_register_card(card);
	if (ret != 0)
		AUDIO_LOGE("sound card register failed %d", ret);

	return ret;
}

static int analog_less_remove(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);

	if (card)
		snd_soc_unregister_card(card);

	return 0;
}

static const struct of_device_id g_analog_less_of_match[] = {
	{ .compatible = "hisilicon,analog-less-machine", },
	{ },
};
MODULE_DEVICE_TABLE(of, g_analog_less_of_match);

static struct platform_driver g_analog_less_driver = {
	.driver = {
		.name = ANALOG_LESS_MACHINE_DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = g_analog_less_of_match,
	},
	.probe = analog_less_probe,
	.remove = analog_less_remove,
};

static int __init analog_less_init(void)
{
	AUDIO_LOGI("analog less init");

	return platform_driver_register(&g_analog_less_driver);
}
late_initcall(analog_less_init);

static void __exit analog_less_exit(void)
{
	platform_driver_unregister(&g_analog_less_driver);
}
module_exit(analog_less_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("ALSA SoC for analog less machine driver");
