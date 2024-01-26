/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_CEZANNE_ACPI_H
#define AMD_CEZANNE_ACPI_H

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include <stdint.h>

#define ACPI_SCI_IRQ 9

unsigned long soc_acpi_write_tables(const struct device *device, unsigned long current,
				    acpi_rsdp_t *rsdp);

#endif /* AMD_CEZANNE_ACPI_H */
