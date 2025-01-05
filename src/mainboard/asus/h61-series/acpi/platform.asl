/* SPDX-License-Identifier: GPL-2.0-only */
Name (GPBS, 0x0500)
Name (GPLN, 0x80)

Scope (\_SB)
{
	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C") /* Power Button Device */)  // _HID: Hardware ID
		Name (_UID, 0xAA)  // _UID: Unique ID
		Name (_STA, 0x0B)  // _STA: Status
	}
}

Method (S1RS, 1, NotSerialized) // not working?
{
	PLED = Zero
}

OperationRegion (GPBX, SystemIO, GPBS, GPLN)
Field (GPBX, ByteAcc, NoLock, Preserve)
{
	Offset (0x18),
		,   27,
	PLED,   1
}

Scope (\_GPE)
{
	Method (_L1F, 0, NotSerialized)  // _Lxx: Level-Triggered GPE, xx=0x00-0xFF
	{
		\_SB.PCI0.LPCB.SIO1.SIOH ()
		Notify (\_SB.PWRB, 0x02) // Device Wake
	}

	Method (_L1D, 0, NotSerialized)  // _Lxx: Level-Triggered GPE, xx=0x00-0xFF
	{
		\_SB.PCI0.LPCB.SIO1.SIOH ()
		Notify (\_SB.PWRB, 0x02) // Device Wake
	}
	Method (_L0D, 0, NotSerialized)  // _Lxx: Level-Triggered GPE, xx=0x00-0xFF
	{
		Notify (\_SB.PCI0.EHC1, 0x02) // Device Wake
		Notify (\_SB.PCI0.EHC2, 0x02) // Device Wake
		Notify (\_SB.PCI0.HDEF, 0x02) // Device Wake
		Notify (\_SB.PCI0.GLAN, 0x02) // Device Wake
	}
	Method (_L0B, 0, NotSerialized)  // _Lxx: Level-Triggered GPE, xx=0x00-0xFF BLUE LOBSTER JUMPSCARE
	{
		Notify (\_SB.PCI0.P0P1, 0x02) // Device Wake
	}
}

Method(_PTS, 1)
{
	\_SB.PCI0.LPCB.SIO1.SIOS (Arg0)
}

Method(_WAK, 1)
{
	S1RS (Arg0)
	\_SB.PCI0.LPCB.SIO1.SIOW (Arg0)

	Return(Package(){0, 0})
}
