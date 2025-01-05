	/* SPDX-License-Identifier: GPL-2.0-only */

	#include <acpi/acpi.h>

	#if CONFIG(BOARD_ASUS_P8H61_M_LX)
	#define BOARD_DSDT_REVISION 0x20171231	/* OEM revision */
	#else
	#define BOARD_DSDT_REVISION 0x20141018
	#endif

	DefinitionBlock(
		"dsdt.aml",
		"DSDT",
		ACPI_DSDT_REV_2,
		"ALASKA",
		"A M I",
		0x00000022
	)
	{
		#include <acpi/dsdt_top.asl>
		#include "acpi/platform.asl"
		#include <cpu/intel/common/acpi/cpu.asl>
		#include <southbridge/intel/common/acpi/platform.asl>
		#include <southbridge/intel/bd82x6x/acpi/globalnvs.asl>
		#include <southbridge/intel/common/acpi/sleepstates.asl>

		Device (\_SB.PCI0)
		{
			#include <northbridge/intel/sandybridge/acpi/sandybridge.asl>
			#include <southbridge/intel/bd82x6x/acpi/pch.asl>

			Device (USB1)
			{
				Name (_ADR, 0x001D0001)  // _ADR: Address
			}

			Device (USB2)
			{
				Name (_ADR, 0x001D0002)  // _ADR: Address
			}

			Device (USB3)
			{
				Name (_ADR, 0x001D0003)  // _ADR: Address
			}

			Device (USB4)
			{
				Name (_ADR, 0x001D0004)  // _ADR: Address
			}

			Device (USB5)
			{
				Name (_ADR, 0x001A0001)  // _ADR: Address
			}

			Device (USB6)
			{
				Name (_ADR, 0x001A0002)  // _ADR: Address
			}

			Device (USB7)
			{
				Name (_ADR, 0x001A0003)  // _ADR: Address
			}

			Device (GLAN)
			{
				Name (_ADR, 0x00190000)  // _ADR: Address
				Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
				{
					Return (GPRW (0x0D, 0x04))
				}
			}

			Device (P0P1)
			{
				Name (_ADR, 0x001E0000)
				Method (_PRW, 0, NotSerialized)
				{
					Return (GPRW (0x0B, 0x04))
				}
				Method (_PRT, 0, NotSerialized)
				{
					If (PICM)
					{
						Return (Package (0x10)
						{
							Package (0x04) {0xFFFF,Zero,Zero,0x15},
							Package (0x04) {0xFFFF,One,Zero,0x16},
							Package (0x04) {0xFFFF,0x02,Zero,0x17},
							Package (0x04) {0xFFFF,0x03,Zero,0x14},
							Package (0x04) {0x0001FFFF,Zero,Zero,0x16},
							Package (0x04) {0x0001FFFF,One,Zero,0x15},
							Package (0x04) {0x0001FFFF,0x02,Zero,0x14},
							Package (0x04) {0x0001FFFF,0x03,Zero,0x17},
							Package (0x04) {0x0002FFFF,Zero,Zero,0x12},
							Package (0x04) {0x0002FFFF,One,Zero,0x13},
							Package (0x04) {0x0002FFFF,0x02,Zero,0x11},
							Package (0x04) {0x0002FFFF,0x03,Zero,0x10},
							Package (0x04) {0x0003FFFF,Zero,Zero,0x13},
							Package (0x04) {0x0003FFFF,One,Zero,0x12},
							Package (0x04) {0x0003FFFF,0x02,Zero,0x15},
							Package (0x04) {0x0003FFFF,0x03,Zero,0x16}
						})
					}
					Return (Package (0x10)
					{
						Package (0x04) {0xFFFF,Zero,\_SB.PCI0.LPCB.LNKF,Zero},
						Package (0x04) {0xFFFF,One,\_SB.PCI0.LPCB.LNKG,Zero},
						Package (0x04) {0xFFFF,0x02,\_SB.PCI0.LPCB.LNKH,Zero},
						Package (0x04) {0xFFFF,0x03,\_SB.PCI0.LPCB.LNKE,Zero},
						Package (0x04) {0x0001FFFF,Zero,\_SB.PCI0.LPCB.LNKG,Zero},
						Package (0x04) {0x0001FFFF,One,\_SB.PCI0.LPCB.LNKF,Zero},
						Package (0x04) {0x0001FFFF,0x02,\_SB.PCI0.LPCB.LNKE,Zero},
						Package (0x04) {0x0001FFFF,0x03,\_SB.PCI0.LPCB.LNKH,Zero},
						Package (0x04) {0x0002FFFF,Zero,\_SB.PCI0.LPCB.LNKC,Zero},
						Package (0x04) {0x0002FFFF,One,\_SB.PCI0.LPCB.LNKD,Zero},
						Package (0x04) {0x0002FFFF,0x02,\_SB.PCI0.LPCB.LNKB,Zero},
						Package (0x04) {0x0002FFFF,0x03,\_SB.PCI0.LPCB.LNKA,Zero},
						Package (0x04) {0x0003FFFF,Zero,\_SB.PCI0.LPCB.LNKD,Zero},
						Package (0x04) {0x0003FFFF,One,\_SB.PCI0.LPCB.LNKC,Zero},
						Package (0x04) {0x0003FFFF,0x02,\_SB.PCI0.LPCB.LNKF,Zero},
						Package (0x04) {0x0003FFFF,0x03,\_SB.PCI0.LPCB.LNKG,Zero}
					})
				}
			}

		}
	}
