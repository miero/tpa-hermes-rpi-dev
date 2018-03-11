/*
 * ASoC Driver for TPA Hermes-RPi.
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

#include <linux/module.h>
#include <linux/types.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/jack.h>
#include <sound/control.h>

static struct gpio_descs *mult_gpios;
static unsigned int tpa_hermes_rpi_rate;

static bool max192;

static const unsigned int tpa_hermes_rpi_rates[] = {
	44100, 48000, 88200, 96000, 176400, 192000, 352800, 384000,
};

static const unsigned int tpa_hermes_rpi_rates_max192[] = {
	44100, 48000, 88200, 96000, 176400, 192000,
};

static struct snd_pcm_hw_constraint_list tpa_hermes_rpi_constraints = {
	.list = tpa_hermes_rpi_rates,
	.count = ARRAY_SIZE(tpa_hermes_rpi_rates),
};

static struct snd_pcm_hw_constraint_list tpa_hermes_rpi_constraints_max192 = {
	.list = tpa_hermes_rpi_rates_max192,
	.count = ARRAY_SIZE(tpa_hermes_rpi_rates_max192),
};

static int snd_tpa_hermes_rpi_init(struct snd_soc_pcm_runtime *rtd)
{
	return snd_soc_dai_set_bclk_ratio(rtd->cpu_dai, 32 * 2);
}

static int tpa_hermes_rpi_startup(struct snd_pcm_substream *substream)
{
	int err = 0;

	if (max192) {
		err = snd_pcm_hw_constraint_list(substream->runtime, 0,
		                                 SNDRV_PCM_HW_PARAM_RATE,
		                                 &tpa_hermes_rpi_constraints_max192);
	} else {
		err = snd_pcm_hw_constraint_list(substream->runtime, 0,
		                                 SNDRV_PCM_HW_PARAM_RATE,
		                                 &tpa_hermes_rpi_constraints);
	}

	if (err < 0)
		return err;

	err = snd_pcm_hw_constraint_single(
	              substream->runtime,
	              SNDRV_PCM_HW_PARAM_CHANNELS,
	              2
	      );

	if (err < 0)
		return err;

	err = snd_pcm_hw_constraint_mask64(
	              substream->runtime,
	              SNDRV_PCM_HW_PARAM_FORMAT,
	              SNDRV_PCM_FMTBIT_S16_LE |
	              SNDRV_PCM_FMTBIT_S24_LE
	      );

	if (err < 0)
		return err;

	return 0;
}

static int snd_tpa_hermes_rpi_hw_params(struct snd_pcm_substream *substream,
                                        struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;

	tpa_hermes_rpi_rate = params_rate(params);

	return snd_soc_dai_set_bclk_ratio(cpu_dai, 32 * 2);
}

static int tpa_hermes_rpi_trigger(struct snd_pcm_substream *substream,
                                  int cmd)
{
	int mult[4];
	mult[0] = 0;
	mult[1] = 0;
	mult[2] = 0;
	mult[3] = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		switch (tpa_hermes_rpi_rate) {
		case 384000:
			mult[0] = 1;
		case 352800:
			break;
		case 192000:
			mult[0] = 1;
		case 176400:
			mult[1] = 1;
			break;
		case 96000:
			mult[0] = 1;
		case 88200:
			mult[1] = 1;
			mult[2] = 1;
			break;
		case 48000:
			mult[0] = 1;
		case 44100:
			mult[2] = 1;
			mult[3] = 1;
			break;
		default:
			return -EINVAL;
		}
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		gpiod_set_array_value_cansleep(mult_gpios->ndescs, 
		                               mult_gpios->desc,
		                               mult);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}


/* machine stream operations */
static struct snd_soc_ops snd_tpa_hermes_rpi_ops = {
	.startup	= tpa_hermes_rpi_startup,
	.hw_params = snd_tpa_hermes_rpi_hw_params,
	.trigger = tpa_hermes_rpi_trigger,
};

static struct snd_soc_dai_link snd_tpa_hermes_rpi_dai[] = {
	{
		.name		= "TPA-Hermes-RPi",
		.stream_name	= "TPA-Hermes-RPi HiFi",
		.cpu_dai_name	= "bcm2708-i2s.0",
		.codec_dai_name	= "tpa-hermes-rpi-dai",
		.platform_name	= "bcm2708-i2s.0",
		.codec_name	= "tpa-hermes-rpi-codec",
		.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
		SND_SOC_DAIFMT_CBM_CFS,
		.ops		= &snd_tpa_hermes_rpi_ops,
		.init		= snd_tpa_hermes_rpi_init,
	},
};

/* audio machine driver */
static struct snd_soc_card snd_tpa_hermes_rpi = {
	.name         = "snd_tpa_hermes_rpi",
	.owner        = THIS_MODULE,
	.dai_link     = snd_tpa_hermes_rpi_dai,
	.num_links    = ARRAY_SIZE(snd_tpa_hermes_rpi_dai),
};

static int snd_tpa_hermes_rpi_probe(struct platform_device *pdev)
{
	int ret = 0;

	snd_tpa_hermes_rpi.dev = &pdev->dev;

	if (pdev->dev.of_node) {
		struct device_node *i2s_node;
		struct snd_soc_dai_link *dai = &snd_tpa_hermes_rpi_dai[0];
		i2s_node = of_parse_phandle(pdev->dev.of_node, "i2s-controller", 0);

		mult_gpios = devm_gpiod_get_array_optional(&pdev->dev, "mult", GPIOD_OUT_LOW);
		if (IS_ERR(mult_gpios))
			return PTR_ERR(mult_gpios);

		if (i2s_node) {
			dai->cpu_dai_name = NULL;
			dai->cpu_of_node = i2s_node;
			dai->platform_name = NULL;
			dai->platform_of_node = i2s_node;
		}

		max192 = of_property_read_bool(pdev->dev.of_node, "tpa-hermes-rpi,max192");
	}

	ret = snd_soc_register_card(&snd_tpa_hermes_rpi);
	if (ret)
		dev_err(&pdev->dev, "snd_soc_register_card() failed: %d\n", ret);

	return ret;
}

static int snd_tpa_hermes_rpi_remove(struct platform_device *pdev)
{
	return snd_soc_unregister_card(&snd_tpa_hermes_rpi);
}

static const struct of_device_id snd_tpa_hermes_rpi_of_match[] = {
	{ .compatible = "tpa,tpa-hermes-rpi", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_tpa_hermes_rpi_of_match);

static struct platform_driver snd_tpa_hermes_rpi_driver = {
	.driver = {
		.name	= "snd-tpa-hermes-rpi",
		.owner	= THIS_MODULE,
		.of_match_table = snd_tpa_hermes_rpi_of_match,
	},
	.probe	= snd_tpa_hermes_rpi_probe,
	.remove	= snd_tpa_hermes_rpi_remove,
};

module_platform_driver(snd_tpa_hermes_rpi_driver);

MODULE_AUTHOR("Gael Chauffaut <gael.chauffaut@gmail.com>");
MODULE_DESCRIPTION("ASoC Driver for TPA Hermes-RPi");
MODULE_LICENSE("GPL v2");
