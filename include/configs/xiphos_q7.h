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
	"rootfs_mtd=11\0" \
	"bootargs=earlyprintk console=ttyPS0,115200 noinitrd init=/sbin/init\0" \
	\
	"load_flash=" \
		"sf probe;" \
		"sf read 500000 ${copyid}-devicetree 20000;" \
		"sf read 100000 ${copyid}-kernel 370000;" \
		"\0" \
	\
	"boot_ubi=" \
		"setenv bootargs ${bootargs} rootwait=1 rw rootfstype=ubifs ubi.mtd=${rootfs_mtd} root=ubi0:rootfs;" \
		"bootm 100000 - 500040;" \
		"\0" \
	\
	"single=setenv bootargs console=ttyPS0,115200 single; bootm 100000 1000000 500040\0"

#endif /* __CONFIG_XIPHOS_Q7_H */
