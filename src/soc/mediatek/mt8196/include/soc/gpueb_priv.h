/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8196_INCLUDE_SOC_GPUEB_PRIV_H__
#define __SOC_MEDIATEK_MT8196_INCLUDE_SOC_GPUEB_PRIV_H__

#include <soc/addressmap.h>
#include <types.h>

#define MAINPLL_SEL			(IO_PHYS + 0x0060)		 /* 0x10000060 */
#define MFG_RPC_SLV_WAY_EN_SET		(MFG_GPUEB_RPC_BASE + 0x0060)	 /* 0x4B800060 */
#define MFG_RPC_SLV_WAY_EN_CLR		(MFG_GPUEB_RPC_BASE + 0x0064)	 /* 0x4B800064 */
#define MFG_RPC_SLV_CTRL_UPDATE		(MFG_GPUEB_RPC_BASE + 0x0068)	 /* 0x4B800068 */
#define MFG_RPC_SLV_SLP_PROT_EN_SET	(MFG_GPUEB_RPC_BASE + 0x0070)	 /* 0x4B800070 */
#define MFG_RPC_SLV_SLP_PROT_EN_CLR	(MFG_GPUEB_RPC_BASE + 0x0074)	 /* 0x4B800074 */
#define MFG_RPC_SLV_SLP_PROT_RDY_STA	(MFG_GPUEB_RPC_BASE + 0x0078)	 /* 0x4B800078 */
#define MFG_RPC_MFG1_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0500)	 /* 0x4B800500 */
#define MFG_RPC_MFG0_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0504)	 /* 0x4B800504 */
#define MFG_RPC_MFG2_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0508)	 /* 0x4B800508 */
#define MFG_RPC_MFG3_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x050C)	 /* 0x4B80050C */
#define MFG_RPC_MFG4_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0510)	 /* 0x4B800510 */
#define MFG_RPC_MFG5_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0514)	 /* 0x4B800514 */
#define MFG_RPC_MFG7_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x051C)	 /* 0x4B80051C */
#define MFG_RPC_MFG8_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0520)	 /* 0x4B800520 */
#define MFG_RPC_MFG9_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0524)	 /* 0x4B800524 */
#define MFG_RPC_MFG10_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0528)	 /* 0x4B800528 */
#define MFG_RPC_MFG11_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x052C)	 /* 0x4B80052C */
#define MFG_RPC_MFG12_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0530)	 /* 0x4B800530 */
#define MFG_RPC_MFG13_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0534)	 /* 0x4B800534 */
#define MFG_RPC_MFG14_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0538)	 /* 0x4B800538 */
#define MFG_RPC_MFG15_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x053C)	 /* 0x4B80053C */
#define MFG_RPC_MFG16_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0540)	 /* 0x4B800540 */
#define MFG_RPC_MFG17_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0544)	 /* 0x4B800544 */
#define MFG_RPC_MFG18_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0548)	 /* 0x4B800548 */
#define MFG_RPC_MFG19_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x054C)	 /* 0x4B80054C */
#define MFG_RPC_MFG20_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0550)	 /* 0x4B800550 */
#define MFG_RPC_MFG23_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x055C)	 /* 0x4B80055C */
#define MFG_RPC_MFG25_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0564)	 /* 0x4B800564 */
#define MFG_RPC_MFG26_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0568)	 /* 0x4B800568 */
#define MFG_RPC_MFG27_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x056C)	 /* 0x4B80056C */
#define MFG_RPC_MFG28_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0570)	 /* 0x4B800570 */
#define MFG_RPC_MFG29_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0574)	 /* 0x4B800574 */
#define MFG_RPC_MFG30_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0578)	 /* 0x4B800578 */
#define MFG_RPC_MFG31_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x057C)	 /* 0x4B80057C */
#define MFG_RPC_MFG32_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0580)	 /* 0x4B800580 */
#define MFG_RPC_MFG33_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0584)	 /* 0x4B800584 */
#define MFG_RPC_MFG34_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0588)	 /* 0x4B800588 */
#define MFG_RPC_MFG35_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x058C)	 /* 0x4B80058C */
#define MFG_RPC_MFG36_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0590)	 /* 0x4B800590 */
#define MFG_RPC_MFG37_PWR_CON		(MFG_GPUEB_RPC_BASE + 0x0594)	 /* 0x4B800594 */
#define MFG_RPCTOP_DUMMY_REG_0		(MFG_GPUEB_RPC_BASE + 0x0650)	 /* 0x4B800650 */
#define MFG_GHPM_CFG0_CON		(MFG_GPUEB_RPC_BASE + 0x0800)	 /* 0x4B800800 */
#define MFG_RPC_GHPM_CFG13_CON		(MFG_GPUEB_RPC_BASE + 0x0834)	 /* 0x4B800834 */
#define MFG_GHPM_RO2_CON		(MFG_GPUEB_RPC_BASE + 0x09AC)	 /* 0x4B8009AC */
#define MFG_RPC_GPUEB_CFG		(MFG_VCORE_AO_CFG_BASE + 0x0074) /* 0x4B860074 */
#define MFG_RPC_AO_CLK_CFG		(MFG_VCORE_AO_CFG_BASE + 0x0078) /* 0x4B860078 */
#define MFGSYS_PROTECT_EN_SET_0		(MFG_VCORE_AO_CFG_BASE + 0x0080) /* 0x4B860080 */
#define MFGSYS_PROTECT_EN_CLR_0		(MFG_VCORE_AO_CFG_BASE + 0x0084) /* 0x4B860084 */
#define MFGSYS_PROTECT_EN_STA_0		(MFG_VCORE_AO_CFG_BASE + 0x0088) /* 0x4B860088 */
#define MFG_SODI_VRF18			(MFG_VCORE_AO_CFG_BASE + 0x00D4) /* 0x4B8600D4 */

/* GPUEB */
#define GPUEB_MUX				MFG_RPC_AO_CLK_CFG
#define GPUEB_SRAM_SIZE				(640 * KiB)
#define GPUEB_GPR_SIZE				128
#define GPUEB_SRAM_GPR_SIZE			4
#define GPR_BASE_START				(MFG_GPUEB_BASE + 0x0009FD00)
#define GPR_BASE_ADDR(x)			(GPR_BASE_START + (x * GPUEB_SRAM_GPR_SIZE))
#define GPUEB_INFO				GPR_BASE_ADDR(0)
#define GPUEB_ABNORMAL_BOOT			GPR_BASE_ADDR(1)
#define GPUEB_INIT_FOOTPRINT			GPR_BASE_ADDR(2)
#define GPUMPU_RSV_ADDR				GPR_BASE_ADDR(6)
#define GPUFREQ_FOOTPRINT_GPR			GPR_BASE_ADDR(17)
#define GPUEB_DIAGNOSIS_GPR			GPR_BASE_ADDR(18)
#define GPUEB_WARM_BOOT				GPR_BASE_ADDR(23)
#define GPUPTP3_BAD_SAMPLE			GPR_BASE_ADDR(30)
#define GPUEB_CFGREG_BASE			(MFG_GPUEB_BASE + 0x140100)
#define GPUEB_CFGREG_SW_RSTN			(GPUEB_CFGREG_BASE + 0x0000)
#define GPUEB_CFGREG_SW_RSTN_SW_RSTN		BIT(0)
#define GPUEB_CFGREG_SW_RSTN_DMA_BUSY_MASK	BIT(1)
#define GPUEB_CFGREG_PTCM_LOCK			BIT(3)
#define GPUEB_CKMUX_SEL_26M_BIT			BIT(6)
#define GPUEB_TCM_LOCK_MASK			0xFF
#define GPUEB_CFGREG_DBG_APB_PC			(GPUEB_CFGREG_BASE + 0x011C)
#define GPUEB_CFGREG_DBG_APB_LR			(GPUEB_CFGREG_BASE + 0x0120)
#define GPUEB_CFGREG_DBG_APB_SP			(GPUEB_CFGREG_BASE + 0x0124)

#define GPU_PMU_COUNTER_SIZE			100
#define GPU_PMU_COUNTER_END			GPR_BASE_ADDR(0)
#define GPU_PMU_COUNTER_START			(GPU_PMU_COUNTER_END - GPU_PMU_COUNTER_SIZE)
#define GPUEB_SMEM_START			(GPU_PMU_COUNTER_START & TCM_LOCK_ALIGNMENT)

#define TCM_LOCK_ADDR				GPUEB_SMEM_START
#define TCM_LOCK_ALIGNMENT			0xFFFFF000	/* 4KB alignment */

#define SW_VCORE_REQ				BIT(0)
#define SW_PMIC_REQ				BIT(1)
#define SW_CK26M_REQ				BIT(2)
#define SW_MAINPLL_REQ				BIT(3)
#define RESOURCE_BITS				(SW_MAINPLL_REQ | SW_CK26M_REQ | SW_PMIC_REQ |\
						 SW_VCORE_REQ)

#define VCORE_ACK_SYNC				BIT(28)
#define PMIC_ACK_SYNC				BIT(29)
#define CK26M_ACK_SYNC				BIT(30)
#define MAINPLL_ACK_SYNC			BIT(31)
#define RESOURCE_ACK_BITS			(MAINPLL_ACK_SYNC | CK26M_ACK_SYNC |\
						 PMIC_ACK_SYNC | VCORE_ACK_SYNC)

#define MFG_RPC_MFG0_PWR_ACK_2ND_BIT		BIT(31)
#define MFG_RPC_MFG0_PWR_ACK_1ST_BIT		BIT(30)
#define MFG_RPC_MFG0_PWR_ACK_BIT		(MFG_RPC_MFG0_PWR_ACK_1ST_BIT |\
						 MFG_RPC_MFG0_PWR_ACK_2ND_BIT)
#define MFG_RPC_MFG0_PWR_SRAM_PDN_ACK		BIT(12)
#define MFG_RPC_MFG0_PWR_SRAM_SLEEP_B_ACK	BIT(23)
#define MFG0_PWR_CON				MFG_RPC_MFG0_PWR_CON
#define MFG0_PWR_ACK_BIT			MFG_RPC_MFG0_PWR_ACK_BIT
#define MFG0_PWR_SRAM_SLEEP_B_ACK		MFG_RPC_MFG0_PWR_SRAM_SLEEP_B_ACK
#define MFG0_PWR_SRAM_PDN_ACK			MFG_RPC_MFG0_PWR_SRAM_PDN_ACK
#define GPUEB_BOOT_UP_COMPLETE			0x55667788
#define GPUEB_PT_MAGIC				0x58901690
#define GPUEB_BUSPLL_RES_STA_GPR		GPR_BASE_ADDR(25)
#define MFG0_PWR_SRAM_PDN			BIT(8)
#define MFG_PWR_OFF_VALUE0			0x00001112
#define MFG_PWR_OFF_VALUE2			0x00003312
#define MFG_PWR_OFF_VALUE3			0x00700112
#define SPM_MFG_PWR_OFF_VALUE			0x02000372
#define MFG_PWR_OFF_VALUE1			0x60001112
#define MFG0_PWR_ON_VALUE			0xD0B0000D

#define BOOT_TIMEOUT_US				1000000
#define MTCMOS_DELAY_TIME_US			3
#define MFG0_PWR_ON_DELAY_US			50
#define PAGE_SHIFT				12
#define SWITCH_MUX_WAIT_US			10
#define ACK_SYNC_COUNT				50
#define POWER_ACK_DELAY_COUNT			50
#define POWER_2ND_ACK_DELAY_COUNT		1000

enum pt_fw_idx {
	PT_FW_IDX_GPUEB_FW				= 1,
	PT_FW_IDX_AUTO_DMA_BRCAST_PART1_FW		= 2,
	PT_FW_IDX_AUTO_DMA_BRCAST_PART2_FW		= 3,
	PT_FW_IDX_AUTO_DMA_BRCAST_PART3_FW		= 4,
	PT_FW_IDX_AUTO_DMA_SMMU_FW			= 5,
	PT_FW_IDX_AUTO_DMA_PREOC_FW			= 6,
	PT_FW_IDX_AUTO_DMA_HBVC_LOGGING_FW		= 7,
	PT_FW_IDX_AUTO_DMA_CCMD_CH1_FW			= 8,
	PT_FW_IDX_AUTO_DMA_CCMD_CH2_FW			= 9,
	PT_FW_IDX_AUTO_DMA_CCMD_CH0_FW			= 10,
	PT_FW_IDX_AUTO_DMA_HBVC_CONFIG_GRP0_FW		= 11,
	PT_FW_IDX_AUTO_DMA_HBVC_CONFIG_GRP1_FW		= 12,
	PT_FW_IDX_AUTO_DMA_GPM_OUT_OF_RATIO_FW		= 13,
	PT_FW_IDX_AUTO_DMA_GPM_OUT_OF_SUM_MAX_FW	= 14,
	PT_FW_IDX_AUTO_DMA_TOP_SESV8_FW			= 15,
	PT_FW_IDX_AUTO_DMA_STK_SESV8_FW			= 16,
	PT_FW_IDX_AUTO_DMA_CS_BRCAST_PART1_FW		= 17,
	PT_FW_IDX_AUTO_DMA_CS_BRCAST_PART2_FW		= 18,
	PT_FW_IDX_AUTO_DMA_CS_BRCAST_PART3_FW		= 19,
	PT_FW_IDX_AUTO_DMA_CS_HBVC_LOGGING_FW		= 20,
	PT_FW_IDX_AUTO_DMA_CS_TOP_SESV8_FW		= 21,
	PT_FW_IDX_AUTO_DMA_CS_STK_SESV8_FW		= 22,
	PT_FW_NUM					= 23,

	PT_FW_IDX_AUTO_DMA_START = PT_FW_IDX_AUTO_DMA_BRCAST_PART1_FW,
	PT_FW_IDX_AUTO_DMA_END   = PT_FW_IDX_AUTO_DMA_STK_SESV8_FW,
};

struct gpueb_pt_hdr {
	unsigned int magic;	/* magic number*/
	unsigned int hdr_size;	/* data size */
	unsigned int img_size;	/* img size */
	unsigned int align;	/* align */
	unsigned int id;	/* image id */
	unsigned int addr;	/* the FW address is in the 4GB range */
};

#endif /* __SOC_MEDIATEK_MT8196_INCLUDE_SOC_GPUEB_PRIV_H__ */
