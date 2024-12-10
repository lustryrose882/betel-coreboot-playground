/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This driver resets the 10ec:8168 NIC then tries to read
 * "macaddress" string XX:XX:XX:XX:XX:XX from CBFS.
 * If no MAC is found, it programs a default MAC address in the device
 * so that if the EEPROM/efuse is unconfigured it still has a default MAC.
 */

#include <cbfs.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <string.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <delay.h>
#include <fmap.h>
#include <types.h>

#include "chip.h"

#define NIC_TIMEOUT		1000

#define CMD_REG			0x37
#define CMD_REG_RESET		0x10
#define CMD_LED0_LED1		0x18
#define CMD_LED_FEATURE		0x94
#define CMD_LEDSEL0		0x18
#define CMD_LEDSEL2		0x84

#define CFG_9346		0x50
#define CFG_9346_LOCK		0x00
#define CFG_9346_UNLOCK		0xc0
#define CMD_REG_ASPM		0xb0
#define ASPM_L1_2_MASK		0xe059000f

#define DEVICE_INDEX_BYTE	12
#define MAX_DEVICE_SUPPORT	10

#define RTL_W8(tp, reg, val8)		outb((val8), tp + (reg))
#define RTL_W16(tp, reg, val16)		outw((val16), tp + (reg))
#define RTL_W32(tp, reg, val32)		outl((val32), tp + (reg))
#define RTL_R8(tp, reg)			inb(tp + (reg))
#define RTL_R16(tp, reg)		inw(tp + (reg))
#define RTL_R32(tp, reg)		inl(tp + (reg))

enum rtl_registers {
	MAC0		= 0,	/* Ethernet hardware address. */
	MAC4		= 4,
	MAR0		= 8,	/* Multicast filter. */
	CounterAddrLow		= 0x10,
	CounterAddrHigh		= 0x14,
	TxDescStartAddrLow	= 0x20,
	TxDescStartAddrHigh	= 0x24,
	TxHDescStartAddrLow	= 0x28,
	TxHDescStartAddrHigh	= 0x2c,
	FLASH		= 0x30,
	ERSR		= 0x36,
	ChipCmd		= 0x37,
	TxPoll		= 0x38,
	IntrMask	= 0x3c,
	IntrStatus	= 0x3e,

	TxConfig	= 0x40,
#define	TXCFG_AUTO_FIFO			(1 << 7)	/* 8111e-vl */
#define	TXCFG_EMPTY			(1 << 11)	/* 8111e-vl */

	RxConfig	= 0x44,
#define	RX128_INT_EN			(1 << 15)	/* 8111c and later */
#define	RX_MULTI_EN			(1 << 14)	/* 8111c only */
#define	RXCFG_FIFO_SHIFT		13
					/* No threshold before first PCI xfer */
#define	RX_FIFO_THRESH			(7 << RXCFG_FIFO_SHIFT)
#define	RX_EARLY_OFF			(1 << 11)
#define	RX_PAUSE_SLOT_ON		(1 << 11)	/* 8125b and later */
#define	RXCFG_DMA_SHIFT			8
					/* Unlimited maximum PCI burst. */
#define	RX_DMA_BURST			(7 << RXCFG_DMA_SHIFT)

	Cfg9346		= 0x50,
	Config0		= 0x51,
	Config1		= 0x52,
	Config2		= 0x53,
#define PME_SIGNAL			(1 << 5)	/* 8168c and later */

	Config3		= 0x54,
	Config4		= 0x55,
	Config5		= 0x56,
	PHYAR		= 0x60,
	PHYstatus	= 0x6c,
	RxMaxSize	= 0xda,
	CPlusCmd	= 0xe0,
	IntrMitigate	= 0xe2,

#define RTL_COALESCE_TX_USECS	GENMASK(15, 12)
#define RTL_COALESCE_TX_FRAMES	GENMASK(11, 8)
#define RTL_COALESCE_RX_USECS	GENMASK(7, 4)
#define RTL_COALESCE_RX_FRAMES	GENMASK(3, 0)

#define RTL_COALESCE_T_MAX	0x0fU
#define RTL_COALESCE_FRAME_MAX	(RTL_COALESCE_T_MAX * 4)

	RxDescAddrLow	= 0xe4,
	RxDescAddrHigh	= 0xe8,
	EarlyTxThres	= 0xec,	/* 8169. Unit of 32 bytes. */

#define NoEarlyTx	0x3f	/* Max value : no early transmit. */

	MaxTxPacketSize	= 0xec,	/* 8101/8168. Unit of 128 bytes. */

#define TxPacketMax	(8064 >> 7)
#define EarlySize	0x27

	FuncEvent	= 0xf0,
	FuncEventMask	= 0xf4,
	FuncPresetState	= 0xf8,
	IBCR0           = 0xf8,
	IBCR2           = 0xf9,
	IBIMR0          = 0xfa,
	IBISR0          = 0xfb,
	FuncForceEvent	= 0xfc,
	MAC0_BKP	= 0x19e0,
};

enum rtl8168_registers {
	LED_CTRL		= 0x18,
	LED_FREQ		= 0x1a,
	EEE_LED			= 0x1b,
	ERIDR			= 0x70,
	ERIAR			= 0x74,
#define ERIAR_FLAG			0x80000000
#define ERIAR_WRITE_CMD			0x80000000
#define ERIAR_READ_CMD			0x00000000
#define ERIAR_ADDR_BYTE_ALIGN		4
#define ERIAR_TYPE_SHIFT		16
#define ERIAR_EXGMAC			(0x00 << ERIAR_TYPE_SHIFT)
#define ERIAR_MSIX			(0x01 << ERIAR_TYPE_SHIFT)
#define ERIAR_ASF			(0x02 << ERIAR_TYPE_SHIFT)
#define ERIAR_OOB			(0x02 << ERIAR_TYPE_SHIFT)
#define ERIAR_MASK_SHIFT		12
#define ERIAR_MASK_0001			(0x1 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_0011			(0x3 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_0100			(0x4 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_0101			(0x5 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_1111			(0xf << ERIAR_MASK_SHIFT)
	EPHY_RXER_NUM		= 0x7c,
	OCPDR			= 0xb0,	/* OCP GPHY access */
#define OCPDR_WRITE_CMD			0x80000000
#define OCPDR_READ_CMD			0x00000000
#define OCPDR_REG_MASK			0x7f
#define OCPDR_GPHY_REG_SHIFT		16
#define OCPDR_DATA_MASK			0xffff
	OCPAR			= 0xb4,
#define OCPAR_FLAG			0x80000000
#define OCPAR_GPHY_WRITE_CMD		0x8000f060
#define OCPAR_GPHY_READ_CMD		0x0000f060
	GPHY_OCP		= 0xb8,
	RDSAR1			= 0xd0,	/* 8168c only. Undocumented on 8168dp */
	MISC			= 0xf0,	/* 8168e only. */
#define TXPLA_RST			(1 << 29)
#define DISABLE_LAN_EN			(1 << 23) /* Enable GPIO pin */
#define PWM_EN				(1 << 22)
#define RXDV_GATED_EN			(1 << 19)
#define EARLY_TALLY_EN			(1 << 16)
};

enum mac_version {
	RTL_GIGA_MAC_VER_02,
	RTL_GIGA_MAC_VER_03,
	RTL_GIGA_MAC_VER_04,
	RTL_GIGA_MAC_VER_05,
	RTL_GIGA_MAC_VER_06,
	RTL_GIGA_MAC_VER_07,
	RTL_GIGA_MAC_VER_08,
	RTL_GIGA_MAC_VER_09,
	RTL_GIGA_MAC_VER_10,
	RTL_GIGA_MAC_VER_11,
	RTL_GIGA_MAC_VER_14,
	RTL_GIGA_MAC_VER_17,
	RTL_GIGA_MAC_VER_18,
	RTL_GIGA_MAC_VER_19,
	RTL_GIGA_MAC_VER_20,
	RTL_GIGA_MAC_VER_21,
	RTL_GIGA_MAC_VER_22,
	RTL_GIGA_MAC_VER_23,
	RTL_GIGA_MAC_VER_24,
	RTL_GIGA_MAC_VER_25,
	RTL_GIGA_MAC_VER_26,
	RTL_GIGA_MAC_VER_28,
	RTL_GIGA_MAC_VER_29,
	RTL_GIGA_MAC_VER_30,
	RTL_GIGA_MAC_VER_31,
	RTL_GIGA_MAC_VER_32,
	RTL_GIGA_MAC_VER_33,
	RTL_GIGA_MAC_VER_34,
	RTL_GIGA_MAC_VER_35,
	RTL_GIGA_MAC_VER_36,
	RTL_GIGA_MAC_VER_37,
	RTL_GIGA_MAC_VER_38,
	RTL_GIGA_MAC_VER_39,
	RTL_GIGA_MAC_VER_40,
	RTL_GIGA_MAC_VER_42,
	RTL_GIGA_MAC_VER_43,
	RTL_GIGA_MAC_VER_44,
	RTL_GIGA_MAC_VER_46,
	RTL_GIGA_MAC_VER_48,
	RTL_GIGA_MAC_VER_51,
	RTL_GIGA_MAC_VER_52,
	RTL_GIGA_MAC_VER_53,
	RTL_GIGA_MAC_VER_61,
	RTL_GIGA_MAC_VER_63,
	RTL_GIGA_MAC_VER_65,
	RTL_GIGA_MAC_VER_66,
	RTL_GIGA_MAC_NONE
};

static void _rtl_eri_write(u16 io_base, int addr, u32 mask,
			   u32 val, int type)
{
	u32 cmd = ERIAR_WRITE_CMD | type | mask | addr;

	RTL_W32(io_base, ERIDR, val);
	RTL_W32(io_base, ERIAR, cmd);

	udelay(100);
}

static void rtl_eri_write(u16 io_base, int addr, u32 mask,
			  u32 val)
{
	_rtl_eri_write(io_base, addr, mask, val, ERIAR_EXGMAC);
}

static u32 _rtl_eri_read(u16 io_base, int addr, int type)
{
	u32 cmd = ERIAR_READ_CMD | type | ERIAR_MASK_1111 | addr;

	RTL_W32(io_base, ERIAR, cmd);
	return RTL_R32(io_base, ERIDR);
}

static u32 rtl_eri_read(u16 io_base, int addr)
{
	return _rtl_eri_read(io_base, addr, ERIAR_EXGMAC);
}

static enum mac_version rtl8169_get_mac_version(u16 xid, bool gmii)
{

	static const struct rtl_mac_info {
		u16 mask;
		u16 val;
		enum mac_version ver;
	} mac_info[] = {
		/* 8126A family. */
		{ 0x7cf, 0x64a,	RTL_GIGA_MAC_VER_66 },
		{ 0x7cf, 0x649,	RTL_GIGA_MAC_VER_65 },

		/* 8125B family. */
		{ 0x7cf, 0x641,	RTL_GIGA_MAC_VER_63 },

		/* 8125A family. */
		{ 0x7cf, 0x609,	RTL_GIGA_MAC_VER_61 },
		/* It seems only XID 609 made it to the mass market.
		 * { 0x7cf, 0x608,	RTL_GIGA_MAC_VER_60 },
		 * { 0x7c8, 0x608,	RTL_GIGA_MAC_VER_61 },
		 */

		/* RTL8117 */
		{ 0x7cf, 0x54b,	RTL_GIGA_MAC_VER_53 },
		{ 0x7cf, 0x54a,	RTL_GIGA_MAC_VER_52 },

		/* 8168EP family. */
		{ 0x7cf, 0x502,	RTL_GIGA_MAC_VER_51 },
		/* It seems this chip version never made it to
		 * the wild. Let's disable detection.
		 * { 0x7cf, 0x501,      RTL_GIGA_MAC_VER_50 },
		 * { 0x7cf, 0x500,      RTL_GIGA_MAC_VER_49 },
		 */

		/* 8168H family. */
		{ 0x7cf, 0x541,	RTL_GIGA_MAC_VER_46 },
		/* It seems this chip version never made it to
		 * the wild. Let's disable detection.
		 * { 0x7cf, 0x540,	RTL_GIGA_MAC_VER_45 },
		 */
		/* Realtek calls it RTL8168M, but it's handled like RTL8168H */
		{ 0x7cf, 0x6c0,	RTL_GIGA_MAC_VER_46 },

		/* 8168G family. */
		{ 0x7cf, 0x5c8,	RTL_GIGA_MAC_VER_44 },
		{ 0x7cf, 0x509,	RTL_GIGA_MAC_VER_42 },
		/* It seems this chip version never made it to
		 * the wild. Let's disable detection.
		 * { 0x7cf, 0x4c1,	RTL_GIGA_MAC_VER_41 },
		 */
		{ 0x7cf, 0x4c0,	RTL_GIGA_MAC_VER_40 },

		/* 8168F family. */
		{ 0x7c8, 0x488,	RTL_GIGA_MAC_VER_38 },
		{ 0x7cf, 0x481,	RTL_GIGA_MAC_VER_36 },
		{ 0x7cf, 0x480,	RTL_GIGA_MAC_VER_35 },

		/* 8168E family. */
		{ 0x7c8, 0x2c8,	RTL_GIGA_MAC_VER_34 },
		{ 0x7cf, 0x2c1,	RTL_GIGA_MAC_VER_32 },
		{ 0x7c8, 0x2c0,	RTL_GIGA_MAC_VER_33 },

		/* 8168D family. */
		{ 0x7cf, 0x281,	RTL_GIGA_MAC_VER_25 },
		{ 0x7c8, 0x280,	RTL_GIGA_MAC_VER_26 },

		/* 8168DP family. */
		/* It seems this early RTL8168dp version never made it to
		 * the wild. Support has been removed.
		 * { 0x7cf, 0x288,      RTL_GIGA_MAC_VER_27 },
		 */
		{ 0x7cf, 0x28a,	RTL_GIGA_MAC_VER_28 },
		{ 0x7cf, 0x28b,	RTL_GIGA_MAC_VER_31 },

		/* 8168C family. */
		{ 0x7cf, 0x3c9,	RTL_GIGA_MAC_VER_23 },
		{ 0x7cf, 0x3c8,	RTL_GIGA_MAC_VER_18 },
		{ 0x7c8, 0x3c8,	RTL_GIGA_MAC_VER_24 },
		{ 0x7cf, 0x3c0,	RTL_GIGA_MAC_VER_19 },
		{ 0x7cf, 0x3c2,	RTL_GIGA_MAC_VER_20 },
		{ 0x7cf, 0x3c3,	RTL_GIGA_MAC_VER_21 },
		{ 0x7c8, 0x3c0,	RTL_GIGA_MAC_VER_22 },

		/* 8168B family. */
		{ 0x7c8, 0x380,	RTL_GIGA_MAC_VER_17 },
		/* This one is very old and rare, let's see if anybody complains.
		 * { 0x7c8, 0x300,	RTL_GIGA_MAC_VER_11 },
		 */

		/* 8101 family. */
		{ 0x7c8, 0x448,	RTL_GIGA_MAC_VER_39 },
		{ 0x7c8, 0x440,	RTL_GIGA_MAC_VER_37 },
		{ 0x7cf, 0x409,	RTL_GIGA_MAC_VER_29 },
		{ 0x7c8, 0x408,	RTL_GIGA_MAC_VER_30 },
		{ 0x7cf, 0x349,	RTL_GIGA_MAC_VER_08 },
		{ 0x7cf, 0x249,	RTL_GIGA_MAC_VER_08 },
		{ 0x7cf, 0x348,	RTL_GIGA_MAC_VER_07 },
		{ 0x7cf, 0x248,	RTL_GIGA_MAC_VER_07 },
		{ 0x7cf, 0x240,	RTL_GIGA_MAC_VER_14 },
		{ 0x7c8, 0x348,	RTL_GIGA_MAC_VER_09 },
		{ 0x7c8, 0x248,	RTL_GIGA_MAC_VER_09 },
		{ 0x7c8, 0x340,	RTL_GIGA_MAC_VER_10 },

		/* 8110 family. */
		{ 0xfc8, 0x980,	RTL_GIGA_MAC_VER_06 },
		{ 0xfc8, 0x180,	RTL_GIGA_MAC_VER_05 },
		{ 0xfc8, 0x100,	RTL_GIGA_MAC_VER_04 },
		{ 0xfc8, 0x040,	RTL_GIGA_MAC_VER_03 },
		{ 0xfc8, 0x008,	RTL_GIGA_MAC_VER_02 },

		/* Catch-all */
		{ 0x000, 0x000,	RTL_GIGA_MAC_NONE   }
	};
	const struct rtl_mac_info *p = mac_info;
	enum mac_version ver;

	while ((xid & p->mask) != p->val)
		p++;
	ver = p->ver;

	if (ver != RTL_GIGA_MAC_NONE && !gmii) {
		if (ver == RTL_GIGA_MAC_VER_42)
			ver = RTL_GIGA_MAC_VER_43;
		else if (ver == RTL_GIGA_MAC_VER_46)
			ver = RTL_GIGA_MAC_VER_48;
	}

	return ver;
}

static bool rtl_is_8168evl_up(enum mac_version ver)
{
	return ver >= RTL_GIGA_MAC_VER_34 &&
	       ver != RTL_GIGA_MAC_VER_39 &&
	       ver <= RTL_GIGA_MAC_VER_53;
}

static bool rtl_is_8125(enum mac_version ver)
{
	return ver >= RTL_GIGA_MAC_VER_61;
}

/**
 * search: Find first instance of string in a given region
 * @param p       string to find
 * @param a       start address of region to search
 * @param lengthp length of string to search for
 * @param lengtha length of region to search in
 * @return offset offset from start address a where string was found.
 *                If string not found, return lengtha.
 */
static size_t search(const char *p, const u8 *a, size_t lengthp,
		     size_t lengtha)
{
	size_t i, j;

	if (lengtha < lengthp)
		return lengtha;
	/* Searching */
	for (j = 0; j <= lengtha - lengthp; j++) {
		for (i = 0; i < lengthp && p[i] == a[i + j]; i++)
			;
		if (i >= lengthp && a[j - 1] == lengthp)
			return j;
	}
	return lengtha;
}

static u8 get_hex_digit(const u8 c)
{
	u8 ret = 0;

	ret = c - '0';
	if (ret > 0x09) {
		ret = c - 'A' + 0x0a;
		if (ret > 0x0f)
			ret = c - 'a' + 0x0a;
	}
	if (ret > 0x0f) {
		printk(BIOS_ERR, "Invalid hex digit found: "
				 "%c - 0x%02x\n", (char)c, c);
		ret = 0;
	}
	return ret;
}

#define MACLEN 17

/* Returns MAC address based on the key that is passed in. */
static enum cb_err fetch_mac_vpd_key(u8 *macstrbuf, const char *vpd_key)
{
	struct region_device rdev;
	void *search_address;
	size_t search_length;
	size_t offset;

	if (fmap_locate_area_as_rdev("RO_VPD", &rdev)) {
		printk(BIOS_ERR, "Couldn't find RO_VPD region.");
		return CB_ERR;
	}
	search_address = rdev_mmap_full(&rdev);
	if (search_address == NULL) {
		printk(BIOS_ERR, "LAN: VPD not found.\n");
		return CB_ERR;
	}

	search_length = region_device_sz(&rdev);
	offset = search(vpd_key, search_address, strlen(vpd_key),
			search_length);

	if (offset == search_length) {
		printk(BIOS_WARNING, "Could not locate '%s' in VPD\n", vpd_key);
		rdev_munmap(&rdev, search_address);
		return CB_ERR;
	}
	printk(BIOS_DEBUG, "Located '%s' in VPD\n", vpd_key);

	offset += strlen(vpd_key) + 1;	/* move to next character */

	if (offset + MACLEN > search_length) {
		rdev_munmap(&rdev, search_address);
		printk(BIOS_ERR, "Search result too small!\n");
		return CB_ERR;
	}
	memcpy(macstrbuf, search_address + offset, MACLEN);
	rdev_munmap(&rdev, search_address);

	return CB_SUCCESS;
}

/* Prepares vpd_key by concatenating ethernet_mac with device_index */
static enum cb_err fetch_mac_vpd_dev_idx(u8 *macstrbuf, u8 device_index)
{
	char key[] = "ethernet_mac "; /* Leave a space at tail to stuff an index */

	/*
	 * Map each NIC on the DUT to "ethernet_macN", where N is [0-9].
	 * Translate index number from integer to ascii by adding '0' char.
	 */
	key[DEVICE_INDEX_BYTE] = device_index + '0';

	return fetch_mac_vpd_key(macstrbuf, key);
}

static void fetch_mac_string_vpd(struct drivers_net_config *config, u8 *macstrbuf)
{
	if (!config)
		return;

	/* Current implementation is up to 10 NIC cards */
	if (config->device_index > MAX_DEVICE_SUPPORT) {
		printk(BIOS_ERR, "r8168: the maximum device_index should be less then %d\n."
					" Using default 00:e0:4c:00:c0:b0\n", MAX_DEVICE_SUPPORT);
		return;
	}

	if (fetch_mac_vpd_dev_idx(macstrbuf, config->device_index) == CB_SUCCESS)
		return;

	if (!CONFIG(RT8168_SUPPORT_LEGACY_VPD_MAC)) {
		printk(BIOS_ERR, "r8168: mac address not found in VPD,"
						 " using default 00:e0:4c:00:c0:b0\n");
		return;
	}

	if (fetch_mac_vpd_key(macstrbuf, "ethernet_mac") != CB_SUCCESS)
		printk(BIOS_ERR, "r8168: mac address not found in VPD,"
					 " using default 00:e0:4c:00:c0:b0\n");
}

static enum cb_err fetch_mac_string_cbfs(u8 *macstrbuf)
{
	if (!cbfs_load("rt8168-macaddress", macstrbuf, MACLEN)) {
		printk(BIOS_ERR, "r8168: Error reading MAC from CBFS\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static void get_mac_address(u8 *macaddr, const u8 *strbuf)
{
	size_t offset = 0;
	int i;

	if ((strbuf[2] != ':') || (strbuf[5] != ':') ||
	    (strbuf[8] != ':') || (strbuf[11] != ':') ||
	    (strbuf[14] != ':')) {
		printk(BIOS_ERR, "r8168: ignore invalid MAC address in cbfs\n");
		return;
	}

	for (i = 0; i < 6; i++) {
		macaddr[i] = 0;
		macaddr[i] |= get_hex_digit(strbuf[offset]) << 4;
		macaddr[i] |= get_hex_digit(strbuf[offset + 1]);
		offset += 3;
	}
}

static void program_mac_address(struct device *dev, u16 io_base)
{
	enum mac_version chipset;
	u32 txconfig;
	u16 xid;
	u8 macstrbuf[MACLEN] = { 0 };
	int i = 0;
	/* Default MAC Address of 00:E0:4C:00:C0:B0 */
	u8 mac[6] = { 0x00, 0xe0, 0x4c, 0x00, 0xc0, 0xb0 };
	struct drivers_net_config *config = dev->chip_info;

	/* check the VPD for the mac address */
	if (CONFIG(RT8168_GET_MAC_FROM_VPD)) {
		fetch_mac_string_vpd(config, macstrbuf);
	} else {
		if (fetch_mac_string_cbfs(macstrbuf) != CB_SUCCESS)
			printk(BIOS_ERR, "r8168: Error reading MAC from CBFS,"
							" using default 00:e0:4c:00:c0:b0\n");
	}
	get_mac_address(mac, macstrbuf);

	u32 mac_low = mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24);
	u32 mac_high = mac[4] | (mac[5] << 8);

	/* Reset NIC */
	printk(BIOS_DEBUG, "r8168: Resetting NIC...");
	RTL_W8(io_base, CMD_REG, CMD_REG_RESET);

	/* Poll for reset, with 1sec timeout */
	while (i < NIC_TIMEOUT && (RTL_R8(io_base, CMD_REG) & CMD_REG_RESET)) {
		udelay(1000);
		if (++i >= NIC_TIMEOUT)
			printk(BIOS_ERR, "timeout waiting for nic to reset\n");
	}
	if (i < NIC_TIMEOUT)
		printk(BIOS_DEBUG, "done\n");

	printk(BIOS_DEBUG, "r8168: Programming MAC Address...");

	/* Disable register protection */
	RTL_W8(io_base, CFG_9346, CFG_9346_UNLOCK);

	txconfig = RTL_R32(io_base, TxConfig);
	if (txconfig == ~0U)
		printk(BIOS_ERR, "r8168: PCI read failed\n");

	xid = (txconfig >> 20) & 0xfcf;

	chipset = rtl8169_get_mac_version(xid, true); // TODO: add gmii bool as well

	RTL_W8(io_base, Config4, 0x51); // p8h61-m-lx3plus-r2.0 specific: I added it here because why not?

	/* Set MAC address: only 4-byte write accesses allowed */
	RTL_W32(io_base, MAC0, mac_low);
	RTL_R32(io_base, MAC0);
	RTL_W32(io_base, MAC4, mac_high);
	RTL_R32(io_base, MAC4);

	/*
	 IMPORTANT: some NICs with a specific MAC version can't receive MAC address directly from MAC0.
	 This means that it reads the ERIAR or MAC0_BKP side before the MAC0 side.
	 This is why we can't get the the correct value.
	*/
	if (rtl_is_8168evl_up(chipset) && chipset != RTL_GIGA_MAC_VER_34) {
		rtl_eri_write(io_base, 0xe0, ERIAR_MASK_1111, mac_low);
		rtl_eri_read(io_base, 0xe0);
		rtl_eri_write(io_base, 0xe4, ERIAR_MASK_1111, mac_high);
		rtl_eri_read(io_base, 0xe4);
	} else if (rtl_is_8125(chipset)) {
		RTL_W32(io_base, MAC0_BKP, mac_low);
		RTL_R32(io_base, MAC0_BKP);
		RTL_W32(io_base, MAC0_BKP + 4, mac_high);
		RTL_R32(io_base, MAC0_BKP + 4);
	}

	/* Lock config regs */
	RTL_W8(io_base, CFG_9346, CFG_9346_LOCK);

	printk(BIOS_DEBUG, "done\n");
}

static void enable_aspm_l1_2(u16 io_base)
{
	printk(BIOS_INFO, "rtl: Enable ASPM L1.2\n");

	/* Disable register protection */
	RTL_W8(io_base, CFG_9346, CFG_9346_UNLOCK);

	/* Enable ASPM_L1.2 */
	RTL_W32(io_base, CMD_REG_ASPM, ASPM_L1_2_MASK);

	/* Lock config regs */
	RTL_W8(io_base, CFG_9346, CFG_9346_LOCK);
}

static void r8168_set_customized_led(struct device *dev, u16 io_base)
{
	struct drivers_net_config *config = dev->chip_info;

	if (!config)
		return;

	if (dev->device == PCI_DID_REALTEK_8125) {
		/* Set LED global Feature register */
		RTL_W8(io_base, CMD_LED_FEATURE, config->led_feature);
		printk(BIOS_DEBUG, "r8125: read back LED global feature setting as 0x%x\n",
		RTL_R8(io_base, CMD_LED_FEATURE));

		/*
		 * Refer to RTL8125 datasheet 5.Customizable LED Configuration
		 * Register Name	IO Address
		 * LEDSEL0		0x18
		 * LEDSEL2		0x84
		 * LEDFEATURE		0x94
		 *
		 * LEDSEL Bit[]		Description
		 * Bit0			Link10M
		 * Bit1			Link100M
		 * Bit3			Link1000M
		 * Bit5			Link2.5G
		 * Bit9			ACT
		 * Bit10		preboot enable
		 * Bit11		lp enable
		 * Bit12		active low/high
		 *
		 * LEDFEATURE		Description
		 * Bit0			LED Table V1/V2
		 * Bit1~3		Reserved
		 * Bit4~5		LED Blinking Duty Cycle	12.5%/ 25%/ 50%/ 75%
		 * Bit6~7		LED Blinking Freq. 240ms/160ms/80ms/Link-Speed-Dependent
		 */

		/* Set customized LED0 register */
		RTL_W16(io_base, CMD_LEDSEL0, config->customized_led0);
		printk(BIOS_DEBUG, "r8125: read back LED0 setting as 0x%x\n",
			RTL_R16(io_base, CMD_LEDSEL0));
		/* Set customized LED2 register */
		RTL_W16(io_base, CMD_LEDSEL2, config->customized_led2);
		printk(BIOS_DEBUG, "r8125: read back LED2 setting as 0x%x\n",
			RTL_R16(io_base, CMD_LEDSEL2));
	} else {
		/* Read the customized LED setting from devicetree */
		printk(BIOS_DEBUG, "r8168: Customized LED 0x%x\n", config->customized_leds);

		/*
		 * Refer to RTL8111H datasheet 7.2 Customizable LED Configuration
		 * Starting from offset 0x18
		 * Bit[15:12]	LED Feature Control(FC)
		 * Bit[11:08]	LED Select for PINLED2
		 * Bit[07:04]	LED Select for PINLED1
		 * Bit[03:00]	LED Select for PINLED0
		 *
		 * Speed	Link10M		Link100M	Link1000M	ACT/Full
		 * LED0		Bit0		Bit1		Bit2		Bit3
		 * LED1		Bit4		Bit5		Bit6		Bit7
		 * LED2		Bit8		Bit9		Bit10		Bit11
		 * FC		Bit12		Bit13		Bit14		Bit15
		 */

		/* Set customized LED registers */
		RTL_W16(io_base, CMD_LED0_LED1, config->customized_leds);
		printk(BIOS_DEBUG, "r8168: read back LED setting as 0x%x\n",
			RTL_R16(io_base, CMD_LED0_LED1));
	}
}

static void r8168_init(struct device *dev)
{
	/* Get the resource of the NIC mmio */
	struct resource *nic_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	u16 io_base = (u16)nic_res->base;

	/* Check if the base is invalid */
	if (!io_base) {
		printk(BIOS_ERR, "r8168: Error can't find IO resource\n");
		return;
	}
	/* Enable but do not set bus master */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Program MAC address based on CBFS "macaddress" containing
	 * a string AA:BB:CC:DD:EE:FF */
	program_mac_address(dev, io_base);

	/* Program customized LED mode */
	if (CONFIG(RT8168_SET_LED_MODE))
		r8168_set_customized_led(dev, io_base);

	struct drivers_net_config *config = dev->chip_info;
	if (CONFIG(PCIEXP_ASPM) && config->enable_aspm_l1_2)
		enable_aspm_l1_2(io_base);
}

#if CONFIG(HAVE_ACPI_TABLES)
#define R8168_ACPI_HID "R8168"
static void r8168_net_fill_ssdt(const struct device *dev)
{
	struct drivers_net_config *config = dev->chip_info;
	const char *path = acpi_device_path(dev->upstream->dev);
	u32 address;

	if (!path || !config)
		return;

	/* Device */
	acpigen_write_scope(path);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", R8168_ACPI_HID);
	acpi_device_write_uid(dev);

	if (dev->chip_ops)
		acpigen_write_name_string("_DDN", dev->chip_ops->name);
	acpigen_write_STA(ACPI_STATUS_DEVICE_HIDDEN_ON);

	/* Power Resource */
	if (CONFIG(RT8168_GEN_ACPI_POWER_RESOURCE) && config->has_power_resource) {
		const struct acpi_power_res_params power_res_params = {
			.stop_gpio = &config->stop_gpio,
			.stop_delay_ms = config->stop_delay_ms,
			.stop_off_delay_ms = config->stop_off_delay_ms
		};
		acpi_device_add_power_res(&power_res_params);
	}

	/* Address */
	address = PCI_SLOT(dev->path.pci.devfn) & 0xffff;
	address <<= 16;
	address |= PCI_FUNC(dev->path.pci.devfn) & 0xffff;
	acpigen_write_name_dword("_ADR", address);

	/* Wake capabilities */
	if (config->wake)
		acpigen_write_PRW(config->wake, 3);

	if (config->add_acpi_dma_property)
		acpi_device_add_dma_property(NULL);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s.%s: %s %s\n", path, acpi_device_name(dev),
		dev->chip_ops ? dev->chip_ops->name : "", dev_path(dev));
}

static const char *r8168_net_acpi_name(const struct device *dev)
{
	return "RLTK";
}
#endif

static struct device_operations r8168_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = r8168_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = r8168_net_acpi_name,
	.acpi_fill_ssdt   = r8168_net_fill_ssdt,
#endif
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_REALTEK_8168,
	PCI_DID_REALTEK_8125,
	PCI_DID_REALTEK_8111,
	0
};

static const struct pci_driver r8168_driver __pci_driver = {
	.ops    = &r8168_ops,
	.vendor = PCI_VID_REALTEK,
	.devices = pci_device_ids,
};

struct chip_operations drivers_net_ops = {
	.name = "Realtek r8168",
};
