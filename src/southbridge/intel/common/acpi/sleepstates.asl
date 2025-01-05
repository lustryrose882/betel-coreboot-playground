/* SPDX-License-Identifier: GPL-2.0-only */

/* S1 support: bit 0, S2 Support: bit 1, etc. S0 & S5 assumed */
Name (SS1, Zero)
Name (SS2, Zero)
Name (SS3, One)
Name (SS4, One)

/* Supported sleep states: */
Name (_S0, Package (0x04)  // _S0_: S0 System State
{
	Zero,
	Zero,
	Zero,
	Zero
})

If (SS1)
{
	Name (_S1, Package (0x04)  // _S1_: S1 System State
	{
		One,
		Zero,
		Zero,
		Zero
	})
}

If (SS3)
{
	Name (_S3, Package (0x04)  // _S3_: S3 System State
	{
		0x05,
		Zero,
		Zero,
		Zero
	})
}

If (SS4)
{
	Name (_S4, Package (0x04)  // _S4_: S4 System State
	{
		0x06,
		Zero,
		Zero,
		Zero
	})
}

Name (_S5, Package (0x04)  // _S5_: S5 System State
{
	0x07,
	Zero,
	Zero,
	Zero
})
