/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <fw_config.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/bl31.h>
#include <soc/dpm_v2.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/pcie.h>
#include <soc/spm_common.h>
#include <soc/usb.h>

#include "gpio.h"

#define AFE_SE_SECURE_CON1	(AUDIO_BASE + 0x5634)

static void configure_i2s(void)
{
	/* Switch to normal mode */
	write32p(AFE_SE_SECURE_CON1, 0x0);

	/* SoC I2S */
	gpio_set_mode(GPIO_I2SI4_BCK, GPIO_FUNC(I2SIN1_BCK, I2SIN4_BCK));
	gpio_set_mode(GPIO_I2SI4_LRCK, GPIO_FUNC(I2SIN1_LRCK, I2SIN4_LRCK));
	gpio_set_mode(GPIO_I2SO4_D0, GPIO_FUNC(I2SOUT1_DO, I2SOUT4_DATA0));
}

static void configure_audio(void)
{
	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_TAS2563))) {
		mtk_i2c_bus_init(I2C3, I2C_SPEED_FAST);
		configure_i2s();
	}

	printk(BIOS_INFO, "%s: done\n", __func__);
}

static void power_on_fpmcu(void)
{
	/* Power on the fingerprint MCU */
	gpio_output(GPIO_EN_PWR_FP, 1);
	gpio_output(GPIO_FP_RST_1V8_S3_L, 1);
}

bool mainboard_needs_pcie_init(void)
{
	return true;
}

static void mainboard_init(struct device *dev)
{
	setup_usb_host();
	power_on_fpmcu();
	configure_audio();

	if (CONFIG(RAURU_SDCARD_INIT))
		mtk_msdc_configure_sdcard();

	if (dpm_init())
		printk(BIOS_ERR, "dpm init failed, DVFS may not work\n");

	if (spm_init())
		printk(BIOS_ERR, "spm init failed, Suspend may not work\n");

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE))
		register_reset_to_bl31(GPIO_AP_EC_WARM_RST_REQ.id, true);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
