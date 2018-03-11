/*
 * ASoC Codec for TPA Hermes-RPi.
 *
 * Author:	Gael Chauffaut <gael.chauffaut@gmail.com>
 * Based on work from:
 *  Florian Meier <florian.meier@koalo.de>
 *		Copyright 2017
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/soc.h>

static struct snd_soc_codec_driver tpa_hermes_rpi_codec;

static struct snd_soc_dai_driver tpa_hermes_rpi_dai = {
	.name = "tpa-hermes-rpi-dai",
	.playback = {
		.stream_name	= "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_KNOT,
		.formats = SNDRV_PCM_FMTBIT_S16_LE |
		SNDRV_PCM_FMTBIT_S24_LE
	},
};

static int tpa_hermes_rpi_probe(struct platform_device *pdev)
{
	return snd_soc_register_codec(&pdev->dev, &tpa_hermes_rpi_codec,
	                              &tpa_hermes_rpi_dai, 1);
}

static int tpa_hermes_rpi_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static const struct of_device_id tpa_hermes_rpi_of_match[] = {
	{ .compatible = "tpa,tpa-hermes-rpi-codec", },
	{ }
};
MODULE_DEVICE_TABLE(of, tpa_hermes_rpi_of_match);

static struct platform_driver tpa_hermes_rpi_codec_driver = {
	.probe 		= tpa_hermes_rpi_probe,
	.remove 	= tpa_hermes_rpi_remove,
	.driver		= {
		.name	= "tpa-hermes-rpi-codec",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(tpa_hermes_rpi_of_match),
	},
};

module_platform_driver(tpa_hermes_rpi_codec_driver);

MODULE_AUTHOR("Gael Chauffaut <gael.chauffaut@gmail.com>");
MODULE_DESCRIPTION("ASoC Codec for TPA Hermes-RPi");
MODULE_LICENSE("GPL v2");