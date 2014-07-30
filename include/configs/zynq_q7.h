/*
 * (C) Copyright 2013 Xilinx, Inc.
 *
 * Configuration for Zynq Evaluation and Development Board - ZedBoard
 * See zynq-common.h for Zynq common configs
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ZYNQ_ZED_H
#define __CONFIG_ZYNQ_ZED_H

#define CONFIG_SYS_SDRAM_SIZE		(512 * 1024 * 1024)

/* PIM FTDI as  Console */
#ifdef CONSOLE_FTDI
	#define CONFIG_SYS_NS16550
	#define CONFIG_SYS_NS16550_SERIAL
	#define CONFIG_SYS_NS16550_REG_SIZE (-4)
	#define CONFIG_SYS_NS16550_CLK (100000000)
	/* This might be off by 0x1000 */
	#define CONFIG_SYS_NS16550_COM1 0x43011000 
	#define CONFIG_CONS_INDEX 1

/* PS Serial As Console */
#elif defined  CONSOLE_UART0
	#define CONFIG_ZYNQ_SERIAL_UART0
#else
	#error "CONSOLE is not set. Please define CONSOLE_FTDI or CONSOLE_UART0."
#endif

#define CONFIG_ZYNQ_GEM0
#define CONFIG_ZYNQ_GEM_PHY_ADDR0	0

#define CONFIG_SYS_NO_FLASH

#define CONFIG_ZYNQ_USB
#define CONFIG_ZYNQ_SDHCI0
#define CONFIG_ZYNQ_SDHCI1
#define CONFIG_ZYNQ_SDHCI_FMAX 25000000
#define CONFIG_ZYNQ_SDHCI_FLIMIT 1000000
#define CONFIG_ZYNQ_QSPI
#define CONFIG_ZYNQ_QSPI_SINGLE_FLASH

#define CONFIG_ZYNQ_BOOT_FREEBSD
#define CONFIG_DEFAULT_DEVICE_TREE	zynq-zed

#define CONFIG_ENV_OFFSET 0x4c0000
#define CONFIG_CMD_MEMTEST
#define CONFIG_CMD_BLKTEST

#include <configs/zynq-common.h>

#endif /* __CONFIG_ZYNQ_ZED_H */
