/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/booker.h>
#include <bootblock_common.h>
#include <soc/early_init.h>
#include <soc/lastbus_v2.h>
#include <soc/mmu_operations.h>
#include <soc/pll.h>
#include <soc/spm_mtcmos.h>
#include <soc/wdt.h>

void bootblock_soc_init(void)
{
	booker_init();
	mtk_mmu_init();
	lastbus_init();
	mtk_wdt_init();
	mt_pll_init();
	mtcmos_init();
	mt_pll_post_init();
	mtcmos_post_init();
	early_init_clear();
}
