/*
 * Copyright (c) 2017, Xiphos System Corp.
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

/*
 * This defines functions used to get/set values in flash.
 * It assumes the xscinfo is 2 flash sectors long. the first
 * one is ro and the second is rw. variables defined in  RO
 * cannot be overwritten.
 *
 */

#define DEBUG

#include <malloc.h>
#include <common.h>
#include <search.h>
#include <command.h>
#include <common.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <jffs2/jffs2.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) "debug: %s: " fmt, __func__

#define EOL '\0'

#ifdef CONFIG_XSCINFO_ENABLE_RO_WRITE
#define XSCINFO_HTABLE(ro) (ro ? &ro_htab : &rw_htab)
#else
#define XSCINFO_HTABLE(ro) (&rw_htab)
#endif

static struct spi_flash *flash;
unsigned long base_addr;
struct hsearch_data ro_htab;

static int xscinfo_rw_validate_change(const ENTRY *item, const char *newval,
				      enum env_op op, int flag)
{
	ENTRY e, *ep;

	e.key = item->key;
	e.data = NULL;

	hsearch_r(e, FIND, &ep, &ro_htab, flag);
	if (ep != NULL) {
		puts("## Error: attempting to override read-only variable\n");
		return 1;
	}

	return 0;
}

struct hsearch_data rw_htab = {
	.change_ok = xscinfo_rw_validate_change,
};

/*
 * This command is used to initialize the subsystem.
 * probe spi_flash and load 2 sectors from addr to htable.
 *
 * param:
 *   id: can be hex offset or partition name as in mtdparts
 */
int xscinfo_addr(char *id)
{
	int dev = 0;
	loff_t offset, len, maxsize;

	struct udevice *new;
	void *buf;
	u32 sz;

	/* speed and mode will be read from DT */
	if (spi_flash_probe_bus_cs(CONFIG_SF_DEFAULT_BUS,
				   CONFIG_SF_DEFAULT_CS, 0, 0, &new)) {
		puts("## Error: unable to probe flash\n");
		return 1;
	}

	flash = dev_get_uclass_priv(new);
	sz = flash->erase_size;
	debug("flash erase_size is 0x%X\n", sz);

	buf = calloc(sz, sizeof(uint8_t));

	if (mtd_arg_off(id, &dev, &offset, &len,
			&maxsize, MTD_DEV_TYPE_NOR, flash->size))
		offset = simple_strtoul(id, NULL, 16);

	base_addr = offset;

	/* the xscinfo starts with a 64K of ro information */
	if (spi_flash_read(flash, base_addr, sz, buf)) {
		puts("## Error: unable to read read-only section from flash\n");
		goto err;
	}

	if (himport_r(&ro_htab, (char *)buf, sz, EOL, 0, 0, 0, NULL) == 0) {
		puts("## Error: xscinfo read-only section import failed\n");
		goto err;
	}

	/* the second 64K block is rw information */
	if (spi_flash_read(flash, base_addr + sz, sz, buf)) {
		puts("## Error: unable to read spi flash\n");
		goto err;
	}

	if (himport_r(&rw_htab, (char *)buf, sz, EOL, 0, 0, 0, NULL) == 0) {
		puts("## Error: xscinfo read-write section import failed\n");
		goto err;
	}

	return 0;

err:
	free(flash);
	flash = NULL;
	return 1;
}

static int do_xscinfo_addr(cmd_tbl_t *cmdtp, int flag, int argc,
			   char * const argv[])
{
	if (argc < 2)
		return CMD_RET_USAGE;

	return xscinfo_addr(argv[1]);
}

static int print_all(int flag)
{
	char *ro = NULL, *rw = NULL;
	ssize_t len, ret = 0;

	len = hexport_r(&ro_htab, '\n', flag, &ro, 0, 0, NULL);
	if (len > 0) {
		puts("-- read-only section\n");
		puts(ro);
		free(ro);
		puts("\n");
	}
	ret = len;

	len = hexport_r(&rw_htab, '\n', flag, &rw, 0, 0, NULL);
	if (len > 0) {
		puts("-- read-write section\n");
		puts(rw);
		free(rw);
		puts("\n");
	}

	return ret + len;
}

int xscinfo_get(char *name, ENTRY **ep, int *rw_flag)
{
	ENTRY e;

	e.key = name;
	e.data = NULL;
	*rw_flag = 0;

	hsearch_r(e, FIND, ep, &ro_htab, 0);
	if (*ep == NULL) {
		hsearch_r(e, FIND, ep, &rw_htab, 0);
		*rw_flag = 1;
		if (*ep == NULL)
			return 1;
	}

	return 0;
}

/*
 * Command interface: print one or all environment variables
 * search ro, if nothing is found, search rw if still nothing -> error
 *
 */
static int do_xscinfo_get(cmd_tbl_t *cmdtp, int flag, int argc,
			  char * const argv[])
{
	char *name = NULL;
	ENTRY *ep;
	int is_rw;

	name = argv[1];
	if (!name) {
		print_all(flag);
		return CMD_RET_SUCCESS;
	}

	if (xscinfo_get(name, &ep, &is_rw))
		return CMD_RET_FAILURE;

	printf("%s[%s]=%s\n", ep->key, is_rw ? "rw" : "ro", ep->data);

	return CMD_RET_SUCCESS;
}

static int do_xscinfo_set(cmd_tbl_t *cmdtp, int flag, int argc,
			  char * const argv[])
{
	int i, len;
#ifdef CONFIG_XSCINFO_ENABLE_RO_WRITE
	int ro = 0;
#endif
	char  *name, *value, *s;
	ENTRY e, *ep;

	name = argv[1];
#ifdef CONFIG_XSCINFO_ENABLE_RO_WRITE
	if (name[0] == '~') {
		debug("using RO table\n");
		ro = 1;
		name++;
	} else {
		debug("using RW table\n");
	}
#else
	if (name[0] == '~') {
		printf("## Error: illegal character '~'"
		       "in variable name \"%s\"\n", name);
		return 1;
	}
#endif

	if (strchr(name, '=')) {
		printf("## Error: illegal character '='"
		       "in variable name \"%s\"\n", name);
		return 1;
	}

	/* Delete only ? */
	if (argc < 3 || argv[2] == NULL) {
		int rc = hdelete_r(name, XSCINFO_HTABLE(ro), 0);
		return !rc;
	}

	/* Insert / Replace value */
	for (i = 2, len = 0; i < argc; ++i)
		len += strlen(argv[i]) + 1;

	value = malloc(len);
	if (value == NULL) {
		printf("## Can't malloc %d bytes\n", len);
		return 1;
	}
	for (i = 2, s = value; i < argc; ++i) {
		char *v = argv[i];

		while ((*s++ = *v++) != '\0')
			;
		*(s - 1) = ' ';
	}
	if (s != value)
		*--s = '\0';

	e.key	= name;
	e.data	= value;
	hsearch_r(e, ENTER, &ep, XSCINFO_HTABLE(ro), 0);
	free(value);
	if (!ep) {
		printf("## Failed to set \"%s\" variable\n", name);
		return 1;
	}
	return CMD_RET_SUCCESS;
}

static int do_xscinfo_load(cmd_tbl_t *cmdtp, int flag, int argc,
			  char * const argv[])
{
	char *name = NULL;
	ENTRY *ep;
	int is_rw;

	name = argv[1];
	if (!name)
		return CMD_RET_USAGE;

	if (xscinfo_get(name, &ep, &is_rw)) {
		debug("variable \"%s\" not found\n", name);
		return CMD_RET_FAILURE;
	}

	env_set(ep->key, ep->data);
	return CMD_RET_SUCCESS;
}

/*
 * this does not unlock the qspi make sure to do it before
 */
static int xscinfo_save_table(struct hsearch_data *table,
			      unsigned long offset)
{
	char *res = NULL;
	ssize_t len;

	len = hexport_r(table, EOL, 0, &res, 0, 0, NULL);

	if (len > flash->erase_size) {
		puts("## Error: RAM copy is larger that partition\n");
		return 1;
	} else if (len <= 0) {
		debug("nothing to write\n");
		return 0;
	}

	if (spi_flash_erase(flash, offset, flash->erase_size)) {
		puts("## Error: unable to erase flash\n");
		return 1;
	}

	debug("write at: 0x%lX len: 0x%lX\n", offset, len);
	if (spi_flash_write(flash, offset, len, res)) {
		puts("## Error: unable to write to flash\n");
		return 1;
	}

	return 0;
}

static int do_xscinfo_save(cmd_tbl_t *cmdtp, int flag, int argc,
			   char * const argv[])
{

#ifdef CONFIG_XSCINFO_ENABLE_RO_WRITE
	if (xscinfo_save_table(&ro_htab, base_addr))
		return CMD_RET_FAILURE;
#endif
	if (xscinfo_save_table(&rw_htab, base_addr + flash->erase_size))
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_xscinfo_sub[] = {
	U_BOOT_CMD_MKENT(addr, 1, 0, do_xscinfo_addr, "", ""),
	U_BOOT_CMD_MKENT(get, 1, 1, do_xscinfo_get, "", ""),
	U_BOOT_CMD_MKENT(set, CONFIG_SYS_MAXARGS, 0, do_xscinfo_set, "", ""),
	U_BOOT_CMD_MKENT(load, 1, 1, do_xscinfo_load, "", ""),
	U_BOOT_CMD_MKENT(save, 0, 0, do_xscinfo_save, "", ""),
};

static int do_xscinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading command name 'xscinfo' */
	argc--;
	argv++;

	if (strncmp(argv[0], "addr", 4) && !flash) {
		puts("No xscinfo address configured. Please run 'xscinfo addr'\n");
		return CMD_RET_FAILURE;
	}

	c = find_cmd_tbl(argv[0], &cmd_xscinfo_sub[0], ARRAY_SIZE(cmd_xscinfo_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	xscinfo, CONFIG_SYS_MAXARGS, 0, do_xscinfo,
	"xsc persistent info sub-system",
	"addr offset   - initialize sub-system with address in flash\n"
	"xscinfo get VAR [-v]  - get value of VAR\n"
	"xscinfo set VAR value - set value of VAR\n"
	"xscinfo load VAR      - load VAR to environment\n"
	"xscinfo save          - save RW variables to flash\n"
);
