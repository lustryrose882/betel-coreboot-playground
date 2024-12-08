/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <stdint.h>
#include <option.h>

const u32 cim_verb_data[] = {
	0x11060397,	/* Codec Vendor / Device ID: VIA VT1708S */
	0x10438415,	/* Subsystem ID */
	12,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x10438415),
	AZALIA_PIN_CFG(0, 0x19, 0x410110f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x01a19036),
	AZALIA_PIN_CFG(0, 0x1b, 0x0181303e),
	AZALIA_PIN_CFG(0, 0x1c, 0x01014010),
	AZALIA_PIN_CFG(0, 0x1d, 0x0221401f),
	AZALIA_PIN_CFG(0, 0x1e, 0x02a19037),
	AZALIA_PIN_CFG(0, 0x1f, 0x503701f0),
	AZALIA_PIN_CFG(0, 0x20, 0x585600f0),
	AZALIA_PIN_CFG(0, 0x21, 0x474411f0),
	AZALIA_PIN_CFG(0, 0x22, 0x410160f0),
	AZALIA_PIN_CFG(0, 0x23, 0x410120f0),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	// this PC strangely can't run the HDA side properly in ALSA, it's already a problematic chip
	// I added the ac97 setting so it wouldn't spoil the fun
	unsigned int ac97 = get_uint_option("audio_panel_type", 0) & 0x1; // yes or no depending on the bit


	// these two pins are given like this for ac97 in vendor bios
	// oh also, I used the misc bit here in the vt1708s datasheet
	const u32 verbs[] = {
		AZALIA_VERB_12B(0, 0x1d, 0x71d, 0x11), // 0x0221411f
		AZALIA_VERB_12B(0, 0x1e, 0x71d, 0x13)  // 0x02a19137
	};

	if ((viddid == 0x11060397) && ac97) { // this PC doesn't have HDMI output, but I wanted to do it anyway, maybe it has
		azalia_program_verb_table(base, verbs, ARRAY_SIZE(verbs));
	}
}
