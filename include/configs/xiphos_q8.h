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
#define CONFIG_PREBOOT "run load_nor && run load_scratch_env"

/* MTD partition settings */
#define MTDIDS_DEFAULT		"nor0=flash"
#define MTDPARTS_DEFAULT	"mtdparts=flash:" \
				"0x00A00000(qspi0-nom-fw)," \
				"0x00020000(qspi0-nom-xscinfo)," \
				"0x035e0000(qspi0-nom-rootfs)," \
				"0x00A00000(qspi0-gold-fw)," \
				"0x00020000(qspi0-gold-xscinfo)," \
				"0x035e0000(qspi0-gold-rootfs)," \
				"0x00A00000(qspi1-nom-fw)," \
				"0x00020000(qspi1-nom-xscinfo)," \
				"0x035e0000(qspi1-nom-rootfs)," \
				"0x00A00000(qspi1-gold-fw)," \
				"0x00020000(qspi1-gold-xscinfo)," \
				"0x035e0000(qspi1-gold-rootfs)"

/* U-Boot env settings: ENV_IS_IN_UBIFS not yet supported */
#undef CONFIG_ENV_IS_IN_FAT

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"config_done=echo sending config_done (x3); pa3 config; pa3 config; pa3 config\0" \
	"bootargs=console=ttyPS0,115200 earlycon clk_ignore_unused\0" \
	\
	"bitstream_ram_addr=0x40000000\0" \
	"kernel_ram_addr=0x00500000\0" \
	"kernel_gz_ram_addr=0x03000000\0" \
	"dtb_ram_addr=0x00100000\0" \
	"dtb_ram_addr_no_header=0x00100040\0" \
	"initramfs_ram_addr=0x02000000\0" \
	\
	"autoload=no\0" \
	"tftpserver=192.168.200.113\0" \
	"tftpprefix=q8\0" \
	\
	"debug= " \
		"setenv tftpprefix debug\0" \
	\
	"lub= " \
		"dhcp && "\
		"tftpboot " __stringify(CONFIG_SYS_TEXT_BASE) " ${tftpserver}:${tftpprefix}/u-boot.bin && " \
		"go " __stringify(CONFIG_SYS_TEXT_BASE) "\0" \
	"load_scratch_env=env import -b 0xa2402000 0x100\0" \
	"load_tftp= " \
		"dhcp && " \
		"tftpboot ${dtb_ram_addr} ${tftpserver}:${tftpprefix}/devicetree.img && " \
		"tftpboot ${kernel_gz_ram_addr} ${tftpserver}:${tftpprefix}/Image.gz && " \
		"unzip ${kernel_gz_ram_addr} ${kernel_ram_addr} && " \
		"tftpboot ${initramfs_ram_addr} ${tftpserver}:${tftpprefix}/xsc-image-initramfs-q8-reva.cpio.gz.u-boot && " \
		"tftpboot ${bitstream_ram_addr} ${tftpserver}:${tftpprefix}/system.bit && " \
		"fpga loadb 0 ${bitstream_ram_addr} $filesize\0" \
	"load_nor= " \
		"sf probe 0 && " \
		"mtdparts && " \
		"ubi part ${xsc_prefix}-rootfs && " \
		"ubifsmount ubi0:q8-reva-rootfs && " \
		"ubifsload ${bitstream_ram_addr}  /boot/system.bit && " \
		"fpga loadb 0 ${bitstream_ram_addr} ${filesize} && " \
		"ubifsload ${kernel_gz_ram_addr}  /boot/Image.gz && " \
		"unzip ${kernel_gz_ram_addr} ${kernel_ram_addr} && " \
		"ubifsload ${dtb_ram_addr}  /boot/devicetree.img && " \
		"ubi detach\0" \
	"boot_initramfs= "\
		"booti ${kernel_ram_addr} ${initramfs_ram_addr} ${dtb_ram_addr_no_header}\0" \
	"boot_nor= " \
		"setenv bootargs $bootargs serial=${serial} rootwait=1 rw rootfstype=ubifs ubi.mtd=${xsc_prefix}-rootfs root=ubi0:q8-reva-rootfs;" \
		"booti ${kernel_ram_addr} - ${dtb_ram_addr_no_header}\0" \
	\
	"bootcmd=run boot_nor\0"

#endif /* __CONFIG_XIPHOS_Q8_H */
