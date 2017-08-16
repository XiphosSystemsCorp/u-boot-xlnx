/*
 * Copyright (c) 2017, Xiphos System Corp.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * ProAsic3 Functions
 *
 */

#include <asm/io.h>

#include <common.h>
#include <command.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) "debug: %s: " fmt, __func__

__weak int board_pa3_config(void)
{
	puts("## Error: no board_pa3_config defined\n");
	return 1;
}

__weak int board_pa3_status(uint8_t *chip, uint8_t *segment, uint8_t *retry)
{
	puts("## Error: no board_pa3_status defined\n");
	return 1;
}

static int do_pa3_config(cmd_tbl_t *cmdtp, int flag, int argc,
			 char * const argv[])
{
	debug("Sending config_done signal...\n");
	if (board_pa3_config())
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

static int do_pa3_status(cmd_tbl_t *cmdtp, int flag, int argc,
			 char * const argv[])
{
	uint8_t chip, segment, retry;
	char name[10];

	if (board_pa3_status(&chip, &segment, &retry))
		return CMD_RET_FAILURE;

	sprintf(name, "%s%d", segment ? "gold" : "nom", chip);

	env_set("pa3_copy", name);
	env_set_hex("pa3_retry", retry);

	printf("Chip:    %d\n"
	       "Segment: %d\n"
	       "Retry:   %d\n"
	       "Copy ID: %s\n", chip, segment, retry, name);

	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_pa3_sub[] = {
	U_BOOT_CMD_MKENT(config, 0, 1, do_pa3_config, "", ""),
	U_BOOT_CMD_MKENT(status, 0, 0, do_pa3_status, "", ""),
};

static int do_pa3(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading command name 'pa3' */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_pa3_sub[0], ARRAY_SIZE(cmd_pa3_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	pa3, 3, 1, do_pa3,
	"proasic sub-system",
	"pa3 config - Send a config_done signal to to PA3\n"
	"pa3 status - Get status information form the PA3\n"
	"             This writes pa3_copy and pa3_retry to the env.\n"
);
