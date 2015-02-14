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
#ifdef CONSOLE_ZYNQ_PL
	#define CONFIG_SYS_NS16550
	#define CONFIG_SYS_NS16550_SERIAL
	#define CONFIG_SYS_NS16550_REG_SIZE (-4)
	#ifndef Q7_LOGIC_CLK_FREQ
		#error "Please define Q7_LOGIC_CLK_FREQ"
	#endif
	#define CONFIG_SYS_NS16550_CLK (Q7_LOGIC_CLK_FREQ)
	/* This might be off by 0x1000 */
	#define CONFIG_SYS_NS16550_COM1 0x43011000 
	#define CONFIG_CONS_INDEX 1

/* PS Serial As Console */
#elif defined  CONSOLE_UART0
	#define CONFIG_ZYNQ_SERIAL_UART0
#elif defined  CONSOLE_UART1
	#define CONFIG_ZYNQ_SERIAL_UART1
#else
	#error "CONSOLE is not set. Please define CONSOLE_ZYNQ_PL or CONSOLE_UART0 or CONSOLE_UART1."
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
#define CONFIG_CMD_SETEXPR

#include <configs/zynq-common.h>

/* override size */
#undef CONFIG_ENV_SIZE
#undef CONFIG_ENV_OFFSET_REDUND

#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#define CONFIG_ENV_SIZE (64 << 10) 
/* This needs to become something like 0x1440000 */
#define CONFIG_ENV_OFFSET_REDUND (CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
#define CONFIG_ZYNQ_Q7

#undef CONFIG_SPL
#undef CONFIG_CMD_SPL
#undef CONFIG_FPGA
#undef CONFIG_FPGA_XILINX
#undef CONFIG_FPGA_ZYNQPL
#undef CONFIG_CMD_FPGA
#undef CONFIG_FIT
#undef CONFIG_FIT_VERBOSE

/* declare Q7S default environment */
#undef CONFIG_EXTRA_ENV_SETTINGS 
#define CONFIG_EXTRA_ENV_SETTINGS	\
	"real_serverip=192.168.200.218\0" \
	"serverip=192.168.200.218\0" \
	"gatewayip=192.168.200.1\0" \
	"ipaddr=192.168.200.241\0" \
	"dnsip=192.168.200.168\0" \
	"do_dhcp=set real_serverip ${serverip};set autoload no;dhcp; set serverip ${real_serverip}\0" \
	"ethact=Gem.e000b000\0" \
	"ethaddr=00:0a:35:00:01:22\0"	\
	"netmask=255.255.255.0\0" \
	"autoload=no\0" \
	"boot_gold0=run load_gold0 && setenv bootargs ${bootargs} q7boot=gold0 && run bootm_q7\0" \
	"boot_gold1=run load_gold1 && setenv bootargs ${bootargs} q7boot=gold1 && run bootm_q7\0" \
	"boot_nom0=run load_nom0 && setenv bootargs ${bootargs} q7boot=nom0 && run bootm_q7\0" \
	"boot_nom1=run load_nom1 && setenv bootargs ${bootargs} q7boot=nom1 && run bootm_q7\0" \
	"bootargs=console=ttyPS0,115200 reboot=g panic=2\0" \
	"bootcmd=run tryboot\0" \
	"bootm_q7=run config_done; bootm 0x100000 0x1000000 0x500040\0" \
	"config_done=mw 0xe000a204 0xe00;mw 0xe000a208 0xe00;mw 0xe000a000 0xfdff0000\0" \
	"fdtaddr=500040\0" \
	"load_gold=sf read 0x100000 0x00a90000 0x370000 && sf read 0x500000 0x00a70000 0x20000 && iminfo 0x500000 && sf read 0x1000000 0x00e00000 0x200000\0" \
	"load_gold0=sf probe 0 && run load_gold\0" \
	"load_gold1=sf probe 1 && run load_gold\0" \
	"load_nom=sf read 0x100000 0x00500000 0x370000 && sf read 0x500000 0x004e0000 0x20000 && iminfo 0x500000 && sf read 0x1000000 0x00870000 0x200000\0" \
	"load_nom0=sf probe 0 && run load_nom\0" \
	"load_nom1=sf probe 1 && run load_nom\0" \
	"sf0add_dtb_gold=0x00a70000\0" \
	"sf0add_dtb_nom=0x004e0000\0" \
	"sf0add_initramfs_gold=0x00e00000\0" \
	"sf0add_initramfs_nom=0x00870000\0" \
	"sf0add_uimage_gold=0x00a90000\0" \
	"sf0add_uimage_nom=0x00500000\0" \
	"sf1add_dtb_gold=0x04a70000\0" \
	"sf1add_dtb_nom=0x044e0000\0" \
	"sf1add_initramfs_gold=0x04e00000\0" \
	"sf1add_initramfs_nom=0x04870000\0" \
	"sf1add_uimage_gold=0x04a90000\0" \
	"sf1add_uimage_nom=0x04500000\0" \
	"start_watchdog=mw 0x40300010 0x1dcd6500;mw 0x40300014 0x1dcd6500;mw 0x40300000 0xb45efa83;mw 0x40300004 0x50ad54ee\0" \
	"stderr=serial\0" \
	"stdin=serial\0" \
	"stdout=serial\0" \
	"tryboot=run boot_nom0; run boot_nom1; run boot_gold0; run boot_gold1\0" 

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY 1
#endif /* __CONFIG_ZYNQ_ZED_H */
