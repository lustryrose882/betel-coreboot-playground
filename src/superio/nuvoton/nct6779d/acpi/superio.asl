/* SPDX-License-Identifier: GPL-2.0-only */


#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME NCT6687D
#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

Name (SP1O, 0x2E)

Device(SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("NUVOTON NCT6779D Super I/O"))

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 0x02)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		PNP_ADDR_REG,	8,
		PNP_DATA_REG,	8,
	}

	IndexField (PNP_ADDR_REG, PNP_DATA_REG, ByteAcc, NoLock, Preserve)
	{
		Offset (0x07),
		PNP_LOGICAL_DEVICE,	8, /* Logical device selector */
		Offset (0x10),
		// Device IRQ TYPE Selection
		CR10,	8,
		CR11,	8,
		CR12,	8, // Reserved?
		// Device IRQ Polarity Selection
		CR13,	8,
		CR14,	8,
		Offset (0x1A),
		// Multi Function Selection
		CR1A,	8,
		CR1B,	8,
		CR1C,	8,
		CR1D,	8, // Reserved?

		Offset (0x30),
		PNP_DEVICE_ACTIVE,	1, /* Logical device activation */
		ACT1,			1, /* Logical device activation */
		ACT2,			1, /* Logical device activation */
		ACT3,			1, /* Logical device activation */
		ACT4,			1, /* Logical device activation */
		ACT5,			1, /* Logical device activation */
		ACT6,			1, /* Logical device activation */
		ACT7,			1, /* Logical device activation */

		Offset (0x60),
		PNP_IO0_HIGH_BYTE,	8, /* First I/O port base - high byte */
		PNP_IO0_LOW_BYTE,	8, /* First I/O port base - low byte */
		PNP_IO1_HIGH_BYTE,	8, /* Second I/O port base - high byte */
		PNP_IO1_LOW_BYTE,	8, /* Second I/O port base - low byte */

		Offset (0x70),
		PNP_IRQ0,		8, /* First IRQ */
		Offset (0x72),
		PNP_IRQ1,		8, /* Second IRQ */
		Offset (0x74),
		PNP_DMA0,		8, /* DRQ */
		Offset (0xE0),
		RGE0,	8,
		RGE1,	8,
		RGE2,	8,
		RGE3,	8,
		RGE4,	8,
		RGE5,	8,
		RGE6,	8,
		RGE7,	8,
		RGE8,	8,
		RGE9,	8,
		Offset (0xF0),
		OPT0,	8,
		OPT1,	8,
		OPT2,	8,
		OPT3,	8,
		OPT4,	8,
		OPT5,	8,
		OPT6,	8,
		OPT7,	8,
		OPT8,	8,
		OPT9,	8

	}

	Name (DCAT, Package (0x11)
	{
		0x02,
		0x03,
		One,
		0xFF,
		0xFF,
		0xFF,
		0xFF,
		0xFF,
		0xFF,
		0xFF,
		0x05,
		0xFF,
		0xFF,
		0xFF,
		0x05,
		0xFF,
		0x06
	})

	#undef PNP_ENTER_MAGIC_1ST
	#undef PNP_ENTER_MAGIC_2ND
	#undef PNP_ENTER_MAGIC_3RD
	#undef PNP_ENTER_MAGIC_4TH
	#undef PNP_EXIT_MAGIC_1ST
	#undef PNP_EXIT_SPECIAL_REG
	#undef PNP_EXIT_SPECIAL_VAL
	#define PNP_ENTER_MAGIC_1ST	0x87
	#define PNP_ENTER_MAGIC_2ND	0x87
	#define PNP_EXIT_MAGIC_1ST	0xaa
	#include <superio/acpi/pnp_config.asl>

	Method (CGLD, 1, NotSerialized)
	{
		Return (DerefOf (DCAT [Arg0]))
	}

	Method (_CRS)
	{
		/* Announce the used I/O ports to the OS */
		Return (ResourceTemplate () {
			IO (Decode16, SUPERIO_PNP_BASE, SUPERIO_PNP_BASE, 0x01, 0x02)
		})
	}

	Name (PMFG, Zero)
	Method (SIOW, 1, NotSerialized)
	{
		Debug = "SIOW"
		ENTER_CONFIG_MODE (0x0A)
		PMFG = OPT3
		OPT3 = 0xFF
		OPT6 &= 0xCF
		OPT2 &= 0xFE
		EXIT_CONFIG_MODE()
	}

	Method (SIOS, 1, NotSerialized)
	{
		Debug = "SIOS"
		If ((0x05 != Arg0))
		{
			ENTER_CONFIG_MODE (0x0A)
			If (KBFG)
			{
				RGE0 |= 0x41
				OPT6 &= 0xEF
			}
			Else
			{
				OPT6 &= 0xEF
				RGE0 &= 0xBF
			}

			If (MSFG)
			{
				RGE0 |= 0x20
				OPT6 &= 0xDF
				RGE0 |= 0x02
				RGE0 &= 0xEF
				RGE6 &= 0x7F
			}
			Else
			{
				RGE0 &= 0xDF
				OPT6 &= 0xDF
			}

			OPT3 = 0xFF
			OPT4 = 0xFF
			OPT2 |= One /* \_SB_.PCI0.LPCB.OPT2 */
			EXIT_CONFIG_MODE()
		}
	}

	Method (SIOH, 0, NotSerialized)
	{
		If ((PMFG & 0x10))
		{
			Notify (PS2K, 0x02) // Device Wake
 		}

		If ((PMFG & 0x20))
		{
			Notify (PS2M, 0x02) // Device Wake
		}
	}

	Name (IOST, 0x0000)

	Device (PS2K)
	{
		Name (_HID, EisaId ("PNP0303") /* IBM Enhanced Keyboard (101/102-key, PS/2 Mouse) */)  // _HID: Hardware ID
		Name (_CID, EisaId ("PNP030B"))  // _CID: Compatible ID
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			If ((IOST & 0x0400))
			{
				Return (0x0F)
			}
			Else
			{
				Return (Zero)
			}
		}

		Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
		{
			IO (Decode16, 0x0060, 0x0060, 0x00, 0x01,)
			IO (Decode16, 0x0064, 0x0064, 0x00, 0x01,)
			IRQNoFlags () {1}
		})

		Method(_SRS, 1, NotSerialized) { }
		Method(_DIS, 0, NotSerialized) { }

		Name (_PRS, ResourceTemplate ()  // _PRS: Possible Resource Settings
		{
			StartDependentFn (0x00, 0x00)
			{
				FixedIO ( 0x0060, 0x01,)
				FixedIO ( 0x0064, 0x01,)
				IRQNoFlags () {1}
			}
			EndDependentFn ()
		})
		Method (_PSW, 1, NotSerialized)  // _PSW: Power State Wake
		{
			KBFG = Arg0
		}
	}

	Scope (\)
	{
		Name (KBFG, One)

		Name (PRWP, Package (0x02)
		{
			Zero,
			Zero
		})
		Method (GPRW, 2, NotSerialized)
		{
			PRWP [Zero] = Arg0
			Local0 = (SS1 << One)
			Local0 |= (SS2 << 0x02)
			Local0 |= (SS3 << 0x03)
			Local0 |= (SS4 << 0x04)
			If (((One << Arg1) & Local0))
			{
				PRWP [One] = Arg1
			}
			Else
			{
				Local0 >>= One
				FindSetLeftBit (Local0, PRWP [One])
			}

			Return (PRWP) /* \PRWP */
		}
	}

	Method (PS2K._PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
	{
		Return (GPRW (0x1F, 0x04))
	}

	Device (PS2M)
	{
		Name (_HID, EisaId ("PNP0F03") /* Microsoft PS/2-style Mouse */)  // _HID: Hardware ID
		Name (_CID, EisaId ("PNP0F13") /* PS/2 Mouse */)  // _CID: Compatible ID
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			If ((IOST & 0x4000))
			{
				Return (0x0F)
			}
			Else
			{
				Return (Zero)
			}
		}
		Method(_SRS, 1, NotSerialized) { }
		Method(_DIS, 0, NotSerialized) { }

		Name (CRS1, ResourceTemplate ()
		{
			IRQNoFlags () {12}
		})
		Name (CRS2, ResourceTemplate ()
		{
			IO (Decode16, 0x0060, 0x0060, 0x00, 0x01,)
			IO (Decode16, 0x0064, 0x0064, 0x00, 0x01,)
			IRQNoFlags () {12}
		})
		Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
		{
			If ((IOST & 0x0400))
			{
				Return (CRS1) /* \_SB_.PCI0.LPCB.PS2M.CRS1 */
			}
			Else
			{
				Return (CRS2) /* \_SB_.PCI0.LPCB.PS2M.CRS2 */
			}
		}

		Name (_PRS, ResourceTemplate ()  // _PRS: Possible Resource Settings
		{
			StartDependentFn (0x00, 0x00)
			{
				IRQNoFlags () {12}
			}
			EndDependentFn ()
		})
		Method (_PSW, 1, NotSerialized)  // _PSW: Power State Wake
		{
			MSFG = Arg0
		}
	}

	Scope (\)
	{
		Name (MSFG, One)
	}

	Method (PS2M._PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
	{
		Return (GPRW (0x1D, 0x04))
	}
}
