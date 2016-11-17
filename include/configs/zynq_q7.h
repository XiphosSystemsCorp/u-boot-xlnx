/*
 * (C) Copyright 2013 Xilinx, Inc.
 * (C) Copyright 2016 Xiphos, Inc.
 *
 * Configuration for Zynq Evaluation and Development Board - ZedBoard
 * See zynq-common.h for Zynq common configs
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ZYNQ_Q7_H
#define __CONFIG_ZYNQ_Q7_H

#define CONFIG_SYS_SDRAM_SIZE		(512 * 1024 * 1024)

#define CONFIG_SYS_NO_FLASH

#define CONFIG_ZYNQ_USB
#define CONFIG_ZYNQ_SDHCI0
#define CONFIG_ZYNQ_SDHCI1

#define CONFIG_ZYNQ_QSPI_SINGLE_FLASH

#undef  CONFIG_ENV_OFFSET
#define CONFIG_ENV_OFFSET 0x4c0000

#include <configs/zynq-common.h>

#undef CONFIG_ENV_SIZE
#undef CONFIG_ENV_OFFSET_REDUND
#define CONFIG_ENV_SIZE 0x10000

#define CONFIG_PREBOOT

#undef CONFIG_SPL
#undef CONFIG_CMD_SPL
#undef CONFIG_FPGA
#undef CONFIG_FPGA_XILINX
#undef CONFIG_FPGA_ZYNQPL
#undef CONFIG_CMD_FPGA
#undef CONFIG_FIT
#undef CONFIG_FIT_VERBOSE

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY 1

#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootcmd=echo; echo ' ** UNABLE TO LOAD ENV FROM FLASH **'; echo\0" \
	"config_done=mw 0xe000a204 0xe00;mw 0xe000a208 0xe00;mw 0xe000a000 0xfdff0000\0" \
	"load_nom="\
		"sf read 0x0100000 0x00500000 0x370000 && "\
		"sf read 0x0500000 0x004e0000 0x020000 && iminfo 0x500000 && "\
		"sf read 0x1000000 0x00870000 0x200000\0" \
	"single=setenv bootargs console=ttyPS0,115200 single;" \
		"sf probe 0;" \
		"run load_nom;" \
		"bootm 0x100000 0x1000000 0x500040\0"


#endif /* __CONFIG_ZYNQ_Q7_H */
