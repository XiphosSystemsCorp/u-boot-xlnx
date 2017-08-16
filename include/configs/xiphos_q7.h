/*
 * Copyright (c) 2017, Xiphos System Corp.
 *
 * Configuration settings for the Xiphos Q7 boards
 * See zynq-common.h for Zynq common configs
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_XIPHOS_Q7_H
#define __CONFIG_XIPHOS_Q7_H

#include <configs/zynq-common.h>

#undef CONFIG_ENV_OFFSET_REDUND
#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootcmd=echo; echo ' ** U-BOOT ENVIRONMENT NOT LOADED FROM FLASH **'; echo\0" \
	"config_done=pa3 config\0" \
	"legacy_load_dtb=sf read 500000 520000 20000\0" \
	"legacy_load_kernel=sf read 100000 540000 370000\0" \
	"legacy_load_initramfs=sf read 1000000 9c0000 300000\0" \
	"xsc_legacy=sf probe 0; run legacy_load_dtb; run legacy_load_kernel; run legacy_load_initramfs\0" \
	"single=setenv bootargs console=ttyPS0,115200 single; bootm 0x100000 0x1000000 0x500040\0"

#endif /* __CONFIG_XIPHOS_Q7_H */
