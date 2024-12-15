/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <soc/dramc_info.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/pcie.h>
#include <soc/sspm.h>
#include <soc/symbols.h>
#include <symbols.h>

void bootmem_platform_add_ranges(void)
{
	if (CONFIG(ARM64_BL31_OPTEE_WITH_SMC))
		bootmem_add_range((uint64_t)_resv_mem_optee,
				  REGION_SIZE(resv_mem_optee), BM_MEM_RESERVED);

	reserve_buffer_for_dramc();
}

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size());
}

static void soc_init(struct device *dev)
{
	mtk_mmu_disable_l2c_sram();
	sspm_init();
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.set_resources = noop_set_resources,
	.init = soc_init,
};

static struct device_operations pci_domain_ops = {
	.read_resources = &mtk_pcie_domain_read_resources,
	.set_resources = &mtk_pcie_domain_set_resources,
	.scan_bus = &pci_host_bridge_scan_bus,
	.enable = &mtk_pcie_domain_enable,
};

static void enable_soc_dev(struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &soc_ops;
	} else if (dev->path.type == DEVICE_PATH_DOMAIN) {
		if (mainboard_needs_pcie_init())
			dev->ops = &pci_domain_ops;
		else
			printk(BIOS_DEBUG, "Skip setting PCIe ops\n");
	}
}

struct chip_operations soc_mediatek_mt8196_ops = {
	.name = "SOC Mediatek MT8196",
	.enable_dev = enable_soc_dev,
};
