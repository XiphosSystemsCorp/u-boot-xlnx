/*
 * (C) Copyright 2017 Xiphos Inc.
 * based on previous works by Xilinx
 *
 * Configuration for Q7 and Q7S board
 * Based on Xilinx Zynq Evaluation and Development Board - ZedBoard
 * See zynq-common.h for Zynq common configs
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ZYNQ_Q7_H
#define __CONFIG_ZYNQ_Q7_H

/* Register and values required to send a "config_done" to ProASIC3
   Taken from ug585-Zynq-7000-TRM(v1.10).pdf */
/*  Register to set "direction" of the config done register to "output" */
#define XIPHOS_Q7_PA3_CONFIGDONE_DIRECTION  0xe000a204
/*  Register to set "mode" of the config done register to "output" */
#define XIPHOS_Q7_PA3_CONFIGDONE_MODE       0xe000a208
/* Value to set output to the two register above */
#define XIPHOS_Q7_PA3_CONFIGDONE_OUTPUT     0x00000e00

/* The actual "config_done" register we are trying to access
   Because PA3 is looking for rising edge, register must be toggled */
#define XIPHOS_Q7_PA3_PA3_CONFIGDONE_REG    0xe000a000
/* "Zero" and "One" value to toggle register */
#define XIPHOS_Q7_PA3_CONFIGDONE_ZERO_VALUE 0xfdff0000
#define XIPHOS_Q7_PA3_CONFIGDONE_ONE_VALUE  0xfdff0200


/* Various constant required to access ProASIC3 to get the flash copy */
#define XIPHOS_Q7_PA3_STATUS_ADDRESS  0x40100008
#define XIPHOS_Q7_PA3_MASK_FOR_COPY   0x60
#define XIPHOS_Q7_PA3_QSPI0_NOM_COPY  0x0
#define XIPHOS_Q7_PA3_QSPI1_NOM_COPY  0x20
#define XIPHOS_Q7_PA3_QSPI0_GOLD_COPY 0x40
#define XIPHOS_Q7_PA3_QSPI1_GOLD_COPY 0x60

/* Size of the RAM */
#define CONFIG_SYS_SDRAM_SIZE		(512 * 1024 * 1024)

/* Old config to tell U-Boot we don't use parallel NOR chip */
#define CONFIG_SYS_NO_FLASH

/* Tell U-Boot about some hardware it has to support */
#define CONFIG_ZYNQ_USB
#define CONFIG_ZYNQ_SDHCI0
#define CONFIG_ZYNQ_SDHCI1
#define CONFIG_ZYNQ_QSPI_SINGLE_FLASH
/* Tell U-Boot about some hardware it DOES NOT have to support */
#undef CONFIG_SPL
#undef CONFIG_CMD_SPL
#undef CONFIG_FPGA
#undef CONFIG_FPGA_XILINX
#undef CONFIG_FPGA_ZYNQPL
#undef CONFIG_CMD_FPGA
#undef CONFIG_FIT
#undef CONFIG_FIT_VERBOSE

/* Base offset for the Flash:
   Determine where is the first sector on this flash copy
   Only define them if undefined to allow define coming from Yocto*/
#ifndef XIPHOS_Q7_QSPI0_NOM_OFFSET
	#define XIPHOS_Q7_QSPI0_NOM_OFFSET   0x0000000
#endif
#ifndef XIPHOS_Q7_QSPI0_GOLD_OFFSET
	#define XIPHOS_Q7_QSPI0_GOLD_OFFSET  0x4000000
#endif
#ifndef XIPHOS_Q7_QSPI1_NOM_OFFSET
	#define XIPHOS_Q7_QSPI1_NOM_OFFSET   0x8000000
#endif
#ifndef XIPHOS_Q7_QSPI1_GOLD_OFFSET
	#define XIPHOS_Q7_QSPI1_GOLD_OFFSET  0xc000000
#endif

/* Where the environment reside on flash 
   Added to the base offset above */
#undef  CONFIG_ENV_OFFSET
#ifdef  XIPHOS_Q7_CONFIG_ENV_OFFSET
	#define CONFIG_ENV_OFFSET XIPHOS_Q7_CONFIG_ENV_OFFSET
#else
	#define CONFIG_ENV_OFFSET 0x0500000
#endif

/* This include has to be in between CONFIG_ENV_OFFSET and CONFIG_ENV_SIZE
   Otherwise U-Boot won't find "saveenv". Weird Xilinx code... */
#include <configs/zynq-common.h>

/* Define Q7 environment
   Default is 128KB *NOT REDUNDANT* (we do our own redundancy) */
#undef CONFIG_ENV_OFFSET_REDUND
#undef CONFIG_ENV_SIZE
#ifdef  XIPHOS_Q7_CONFIG_ENV_SIZE
	#define CONFIG_ENV_SIZE XIPHOS_Q7_CONFIG_ENV_SIZE
#else
	#define CONFIG_ENV_SIZE 0x20000
#endif


/* Delay before boot (the time to press anykey to stop U-Boot) */
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY 1
/* Tell U-Boot to call "preboot" before showing up the prompt */
#define CONFIG_PREBOOT

/* Replace Xilinx environment with our own */
#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"send_one_config_done=mw 0xe000a204 0xe00; mw 0xe000a208 0xe00; mw 0xe000a000 0xfdff0000; mw 0xe000a000 0xfdff0200\0" \
	"preboot=echo 'preboot done'\0" \
	"bootcmd=echo; echo ' ** U-BOOT ENVIRONMENT NOT LOADED FROM FLASH **'; echo\0" \
	"config_done=echo 'Sending config_done signal' ; run send_one_config_done ; run send_one_config_done\0" \
	"single=setenv bootargs console=ttyPS0,115200 single; bootm 0x100000 0x1000000 0x500040\0"

#endif /* __CONFIG_ZYNQ_Q7_H */
