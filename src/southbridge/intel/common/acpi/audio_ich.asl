/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel ICH HDA */

// Intel High Definition Audio (Azalia) 0:1b.0

Device (HDEF)
{
	Name (_ADR, 0x001B0000)  // _ADR: Address
	OperationRegion (HDAR, PCI_Config, 0x4C, 0x10)
	Field (HDAR, WordAcc, NoLock, Preserve)
	{
		DCKA,   1,
		Offset (0x01),
		DCKM,   1,
		,   6,
		DCKS,   1,
		Offset (0x08),
		,   15,
		PMES,   1
	}

	Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
	{
		Return (GPRW (0x0D, 0x04))
	}
}
