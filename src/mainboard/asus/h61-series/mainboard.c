/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <drivers/ocp/dmi/ocp_dmi.h>
#include <uuid.h>
#include <pc80/i8254.h>

// p8h61-m-lx3plus-r2.0 specific
struct smbios_onboard_device ethernet_device = {
	.type = SMBIOS_DEVICE_TYPE_ETHERNET,
	.status = 1,  // Enabled or disabled???
	.description = "Onboard Ethernet"
};

// p8h61-m-lx3plus-r2.0 specific
static const struct port_information smbios_type8_info[] = { // I think it's exactly the same
	{"PS/2 Mouse", CONN_NONE, "PS/2 Mouse", CONN_PS_2, TYPE_MOUSE_PORT},
	{"PS/2 Keyboard", CONN_NONE, "PS/2 Keyboard", CONN_PS_2, TYPE_KEYBOARD_PORT},
	{"USB_12", CONN_NONE, "USB_12", CONN_ACCESS_BUS_USB, TYPE_USB},
	{"USB_34", CONN_NONE, "USB_34", CONN_ACCESS_BUS_USB, TYPE_USB},
	{"USB_56", CONN_NONE, "USB_56", CONN_ACCESS_BUS_USB, TYPE_USB},
	{"GbE LAN", CONN_NONE, "GbE LAN", CONN_RJ_45, TYPE_NETWORK_PORT},
	{"AUDIO", CONN_NONE, "AUDIO", CONN_OTHER, TYPE_AUDIO_PORT},
	{"SATA3G_1", CONN_SAS_SATA, "Not Specified", CONN_NONE, TYPE_SATA},
	{"SATA3G_2", CONN_SAS_SATA, "Not Specified", CONN_NONE, TYPE_SATA},
	{"SATA3G_3", CONN_SAS_SATA, "Not Specified", CONN_NONE, TYPE_SATA},
	{"SATA3G_4", CONN_SAS_SATA, "Not Specified", CONN_NONE, TYPE_SATA},
	{"USB_78", CONN_ACCESS_BUS_USB, "Not Specified", CONN_NONE, TYPE_USB},
	{"USB_910", CONN_ACCESS_BUS_USB, "Not Specified", CONN_NONE, TYPE_USB},
	{"AAFP", CONN_MINI_JACK_HEADPHONES, "Not Specified", CONN_NONE, TYPE_AUDIO_PORT},
	{"CPU_FAN", CONN_OTHER, "Not Specified", CONN_NONE, TYPE_OTHER_PORT},
	{"CHA_FAN", CONN_OTHER, "Not Specified", CONN_NONE, TYPE_OTHER_PORT},
	{"VGA", CONN_OTHER, "Not Specified", CONN_NONE, TYPE_OTHER_PORT}
};

// p8h61-m-lx3plus-r2.0 specific
static int mainboard_smbios_data(struct device *dev, int *handle, unsigned long *current)
{
	int len = 0;

	len += smbios_write_type8(
		current, handle,
		smbios_type8_info,
		ARRAY_SIZE(smbios_type8_info)
		);

	len += smbios_write_type10(current, handle, &ethernet_device);

	len += smbios_write_type11(current, handle);

	return len;
}

// p8h61-m-lx3plus-r2.0 specific
static void mainboard_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	t->count = smbios_add_string(t->eos,"08606E7B0B7D"); // DO NOT STEAL MY MAC ADDRESS
	t->count = smbios_add_string(t->eos,TBF);
}

// p8h61-m-lx3plus-r2.0 specific
void smbios_system_set_uuid(u8 *const uuid)
{
	uint8_t system_uuid[UUID_LEN];

	if (!parse_uuid(system_uuid, "5995bfe3-0810-9ea4-f01f-08606e7b0b7d"))
		memcpy(uuid, system_uuid, UUID_LEN);
}

// p8h61-m-lx3plus-r2.0 specific
u8 smbios_mainboard_feature_flags(void)
{
	return SMBIOS_FEATURE_FLAGS_HOSTING_BOARD | SMBIOS_FEATURE_FLAGS_REPLACEABLE;
}

// p8h61-m-lx3plus-r2.0 specific
const char *smbios_mainboard_location_in_chassis(void)
{
	return TBF;
}

// p8h61-m-lx3plus-r2.0 specific
const char *smbios_system_sku(void)
{
	return "SKU";
}

static void mainboard_enable(struct device *dev)
{
	// both are p8h61-m-lx3plus-r2.0 specific
	dev->ops->get_smbios_data = mainboard_smbios_data;
	dev->ops->get_smbios_strings = mainboard_smbios_strings;

	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_NONE,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

static void mainboard_final(void *chip_info)
{
	// p8h61-m-lx3plus-r2.0 specific
	beep(1500, 100);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};
