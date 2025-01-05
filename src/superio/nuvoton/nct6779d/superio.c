/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <option.h>
#include <superio/conf_mode.h>

#include "nct6779d.h"

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#define NCT6779D_ACPI_POWER_LOSS_CONTROL_MASK	0x60
#define NCT6779D_ACPI_POWER_LOSS_CONTROL_SHIFT	5
#define NCT6779D_ACPI_POWER_ALWAYS_OFF		(0 << NCT6779D_ACPI_POWER_LOSS_CONTROL_SHIFT)
#define NCT6779D_ACPI_POWER_ALWAYS_ON		(1 << NCT6779D_ACPI_POWER_LOSS_CONTROL_SHIFT)
#define NCT6779D_ACPI_POWER_PREV_STATE		(2 << NCT6779D_ACPI_POWER_LOSS_CONTROL_SHIFT)
#define NCT6779D_ACPI_POWER_USER_DEFINED	(3 << NCT6779D_ACPI_POWER_LOSS_CONTROL_SHIFT)

static void nct6779d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	uint8_t byte;
	uint8_t power_status;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case NCT6779D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	// these are same as other nuvoton chips, but stay safe!
	case NCT6779D_ACPI:
		/* Set power state after power fail */
		power_status = get_uint_option("power_on_after_fail",
				CONFIG_MAINBOARD_POWER_FAILURE_STATE);
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		byte = pnp_read_config(dev, 0xe4);
		byte &= ~NCT6779D_ACPI_POWER_LOSS_CONTROL_MASK;
		if (power_status == MAINBOARD_POWER_ON)
			byte |= NCT6779D_ACPI_POWER_ALWAYS_ON;
		else if (power_status == MAINBOARD_POWER_KEEP)
			byte |= NCT6779D_ACPI_POWER_PREV_STATE;
		pnp_write_config(dev, 0xe4, byte);
		pnp_exit_conf_mode(dev);
		printk(BIOS_INFO, "set power %s after power fail\n",
		       power_status ? "on" : "off");
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct6779d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT6779D_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8, },
	{ NULL, NCT6779D_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT6779D_SP2, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT6779D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ NULL, NCT6779D_CIR, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT6779D_ACPI, PNP_MSC2,},
	{ NULL, NCT6779D_HWM_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0,
		0x0ffe, 0x0ffe, },
	{ NULL, NCT6779D_WDT1},
	{ NULL, NCT6779D_CIRWKUP, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT6779D_GPIO_PP_OD},
	{ NULL, NCT6779D_PRT80},
	{ NULL, NCT6779D_DSLP},
	{ NULL, NCT6779D_GPIOBASE, PNP_IO0, 0x0ff8, },
	{ NULL, NCT6779D_GPIO0},
	{ NULL, NCT6779D_GPIO1},
	{ NULL, NCT6779D_GPIO2},
	{ NULL, NCT6779D_GPIO3},
	{ NULL, NCT6779D_GPIO4},
	{ NULL, NCT6779D_GPIO5, PNP_MSC4 | PNP_MSC5},
	{ NULL, NCT6779D_GPIO6, PNP_MSC4 | PNP_MSC5},
	{ NULL, NCT6779D_GPIO7},
	{ NULL, NCT6779D_GPIO8},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct6779d_ops = {
	.name = "NUVOTON NCT6779D Super I/O",
	.enable_dev = enable_dev,
};
