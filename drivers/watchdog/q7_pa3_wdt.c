 /* Watchdog driver for Q7 ProASIC3 Watchdog logic.
 *
 * Copyright (C) 2014 Xiphos Systems Corp.
 * All rights reserved.
 *
 * Author: Joshua Lamorie <joshua.lamorie@xiphos.ca>
 *
 * Based on xilinx_wdt driver.
 * Copyright (c) 2011-2013 Xilinx Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <watchdog.h>

#define REG_ENABLE1 0
#define Q6S_WDT_ENABLE1_MAGIC 0xb45efa83
#define REG_ENABLE2 0x4
#define Q6S_WDT_ENABLE2_MAGIC 0x50ad54ee
#define REG_NO_WAY_OUT 0x08
#define REG_TICKLE 0x0c
#define REG_COMPINT 0x10
#define REG_COMPRST 0x14
#define REG_COUNTER 0x18
#define REG_STATUS 0x20

#define Q6S_WDT_WD_RESET_ACTIVE 0x80000000
#define Q6S_WDT_WATCHDOG_ACTIVE 0x4
#define Q6S_WDT_ENABLE2_ACTIVE 0x2
#define Q6S_WDT_ENABLE1_ACTIVE 0x1

struct q7pa3_watchdog_regs {
	u32 enable1; /* 0x0 */
	u32 enable2; /* 0x4 */
	u32 no_way_out; /* 0x8 */
	u32 tickle; /* 0xc*/
	u32 comp_int; /* 0x10 */
	u32 comp_rst; /* 0x14 */
	u32 counter; /* 0x18 */
	u32 status; /* 0x1c */
};

static struct q7pa3_watchdog_regs *watchdog_base =
			(struct q7pa3_watchdog_regs *)CONFIG_WATCHDOG_BASEADDR;

void hw_watchdog_reset(void)
{
	writel(0xbeefbeef, &watchdog_base->tickle);
}

void hw_watchdog_disable(void)
{
	u32 reg;

	/* Read the present contents of counter to see if it is running */
	reg = readl(&watchdog_base->counter);

  if (reg) {
		writel(Q6S_WDT_ENABLE1_MAGIC, &watchdog_base->enable1);
		writel(Q6S_WDT_ENABLE2_MAGIC, &watchdog_base->enable2);
	}
		
	puts("Watchdog disabled!\n");
}

void hw_watchdog_init(void)
{
	int reg;
	/* check if interrupt has occurred */
	reg = readl(&watchdog_base->status);
	if (reg) {
		puts("Watchdog reset has occurred!");
	}

	/* Read the present contents of counter to see if it is running */
	reg = readl(&watchdog_base->counter);


  if (!reg) {

		/* set up for 30 seconds */
		writel((30 * 25000000),&watchdog_base->comp_int);
		writel((30 * 25000000),&watchdog_base->comp_rst);

		puts("Configuring the watchdog for 30 seconds\n");

		writel(Q6S_WDT_ENABLE1_MAGIC, &watchdog_base->enable1);
		writel(Q6S_WDT_ENABLE2_MAGIC, &watchdog_base->enable2);
	} else {
		puts("Watchdog already active\n");
	}
  
}
