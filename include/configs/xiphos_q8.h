/*
 * Copyright (c) 2017, Xiphos System Corp.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * Based in part on:
 *   include/configs/xilinx_zynqmp_zcu102.h
 */

#ifndef __CONFIG_XIPHOS_Q8_H
#define __CONFIG_XIPHOS_Q8_H

#include <configs/xilinx_zynqmp.h>

#define CONFIG_MTD_PARTITIONS

/* cleanup include */
#undef CONFIG_CMD_THOR_DOWNLOAD
#undef CONFIG_CMD_FASTBOOT
#undef CONFIG_PREBOOT

/* MTD partition settings */
#define MTDIDS_DEFAULT		"nor0=flash"
#define MTDPARTS_DEFAULT	"mtdparts=flash:" \
				"128k(qspi0-nom-xscinfo)," \
				"10m(qspi0-nom-fw)," \
				"54m(qspi0-nom-rootfs)," \
				"128k(qspi0-gold-xscinfo)," \
				"10m(qspi0-gold-fw)," \
				"-(qspi0-gold-rootfs)"

/* U-Boot env settings: ENV_IS_IN_UBIFS not yet supported */
#undef CONFIG_ENV_IS_IN_FAT

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"config_done=echo sending config_done (x3); pa3 config; pa3 config; pa3 config\0" \
	"autoload=no\0" \
	"tftpserver=192.168.200.111\0" \
	"boot_dhcp= " \
		"dhcp; " \
		"tftpboot 0x00100000 ${tftpserver}:q8/q8-test.dtb; " \
		"tftpboot 0x00500000 ${tftpserver}:q8/Image; " \
		"tftpboot 0x02000000 ${tftpserver}:q8/xsc-image-initramfs-q8-reva.cpio.gz.u-boot; " \
		"setenv bootargs console=ttyPS0,115200 earlycon clk_ignore_unused init=/bin/sh mem=2G;" \
		"booti 0x00500000 0x02000000 0x00100000\0" \
	"bootcmd=echo; echo ' ** WARNING: USING DEFAULT U-BOOT ENVIRONMENT **'; echo\0"

#endif /* __CONFIG_XIPHOS_Q8_H */
