/*
 * Copyright (c) 2017, Xiphos System Corp.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * Based in part on:
 *   board/xilinx/zynq/board.c
 */

#include <common.h>
#include <fdtdec.h>
#include <fpga.h>
#include <mmc.h>
#include <zynqpl.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_CMD_PA3
int board_pa3_config(void)
{
	/* set "direction" of the config done register to "output" */
	__raw_writel(0x00000e00, 0xe000a204);
	/* set "mode" of the config done register to "output" */
	__raw_writel(0x00000e00, 0xe000a208);

	/* toggle config_done register (rising edge) */
	__raw_writel(0xfdff0000, 0xe000a000);
	__raw_writel(0xfdff0200, 0xe000a000);

	return 0;
}
int board_pa3_status(uint8_t *chip, uint8_t *segment, uint8_t *retry)
{
	uint32_t raw_status;

	raw_status = __raw_readl(0x40100008);
	*retry   = (raw_status & BIT(4)) >> 4;
	*chip    = (raw_status & BIT(5)) >> 5;
	*segment = (raw_status & BIT(6)) >> 6;

	return 0;
}
#endif /* CONFIG_CMD_PA3 */

int board_init(void)
{
#ifdef CONFIG_CMD_PA3
	board_pa3_config();
#endif /* CONFIG_CMD_PA3 */
	return 0;
}

int board_late_init(void)
{

#ifdef CONFIG_XIPHOS_SPACE
	setenv("xsc_space", "enable");
#endif

	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	puts("Board: Xiphos Q7\n");
	return 0;
}
#endif

int zynq_board_read_rom_ethaddr(unsigned char *ethaddr)
{
	return 0;
}

#if !defined(CONFIG_SYS_SDRAM_BASE) && !defined(CONFIG_SYS_SDRAM_SIZE)
int dram_init_banksize(void)
{
	return fdtdec_setup_memory_banksize();
}

int dram_init(void)
{
	if (fdtdec_setup_memory_size() != 0)
		return -EINVAL;

	zynq_ddrc_init();

	return 0;
}
#else
int dram_init(void)
{
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;

	zynq_ddrc_init();

	return 0;
}
#endif
