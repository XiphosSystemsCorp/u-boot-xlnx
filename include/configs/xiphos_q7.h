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

#define CONFIG_MTD_PARTITIONS

#undef CONFIG_ENV_OFFSET_REDUND
#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootcmd=echo; echo ' ** U-BOOT ENVIRONMENT NOT LOADED FROM FLASH **'; echo\0" \
	"config_done=pa3 config\0" \
	"bootargs=earlyprintk console=ttyPS0,115200 noinitrd init=/sbin/init\0" \
	\
	"nom0-rootfs-id=" CONFIG_XSC_ROOTFS_NOM0_ID "\0" \
	"gold0-rootfs-id=" CONFIG_XSC_ROOTFS_GOLD0_ID "\0" \
	"nom1-rootfs-id=" CONFIG_XSC_ROOTFS_NOM1_ID "\0" \
	"gold1-rootfs-id=" CONFIG_XSC_ROOTFS_GOLD1_ID "\0" \
	"set_rootfs_mtd=" \
		"if test $copyid = \"nom0\"; then setenv rootfs_mtd " CONFIG_XSC_ROOTFS_NOM0_ID ";" \
		"elif test $copyid = \"gold0\"; then setenv rootfs_mtd " CONFIG_XSC_ROOTFS_GOLD0_ID ";" \
		"elif test $copyid = \"nom1\"; then setenv rootfs_mtd " CONFIG_XSC_ROOTFS_NOM1_ID ";" \
		"elif test $copyid = \"gold1\"; then setenv rootfs_mtd " CONFIG_XSC_ROOTFS_GOLD1_ID ";" \
		"fi;\0" \
	\
	"load_flash=" \
		"sf probe;" \
		"sf read 500000 ${copyid}-devicetree 20000;" \
		"sf read 100000 ${copyid}-kernel 370000;" \
		"\0" \
	\
	"rootfs_integrity_check=" \
		"echo '';" \
		"echo ' [SKIP] rootfs integrity check';" \
		"echo '';" \
		"\0" \
	\
	"boot_ubi=" \
		"run set_rootfs_mtd;" \
		"run rootfs_integrity_check;" \
		"setenv bootargs ${bootargs} rootwait=1 rw rootfstype=ubifs ubi.mtd=${rootfs_mtd} root=ubi0:rootfs;" \
		"bootm 100000 - 500040;" \
		"\0" \
	\
	"single=setenv bootargs console=ttyPS0,115200 single; bootm 100000 1000000 500040\0"

#endif /* __CONFIG_XIPHOS_Q7_H */
