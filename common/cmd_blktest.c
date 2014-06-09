/*
 * Copyright (C) 2014 Xiphos System Corporations
 * Author: Berke DURAK
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <malloc.h>

static inline uint32_t prng_next_bit(uint32_t q)
{
  return (q << 1) | (((q >> 31) ^ (q >> 4) ^ q) & 1);
}

static uint32_t prng_next(uint32_t q)
{
  int i;

  for (i = 0; i < 32; i ++)
  {
    q = prng_next_bit(q);
  }

  return q;
}

static void sequence_fill(uint32_t *p, uint32_t x0, uint32_t m)
{
  while (m --) {
    *(p ++) = x0 ++;
  }
}

static bool sequence_check(uint32_t *p, uint32_t x0, uint32_t m)
{
  while (m --) {
    if ((*(p ++)) != (x0 ++)) {
      return false;
    }
  }
  return true;
}

int blktest(block_dev_desc_t *dev,
	lbaint_t start_sector, lbaint_t num_sectors, int iterations,
	uint32_t seed, int max_sectors_per_op, bool verbose)
{
	uint32_t x0 = seed;
	int i, j, o;
  bool write_not_read;
	int rc = 1;
	int n;
	ALLOC_CACHE_ALIGN_BUFFER(uint32_t, buffer, max_sectors_per_op * dev->blksz);
	uint32_t sectors[num_sectors];
	unsigned long sector_size, words_per_sector;

	sector_size = dev->blksz;
	words_per_sector = sector_size / sizeof(uint32_t);

  memset(sectors, 0, num_sectors * sizeof(uint32_t));

	if (verbose) {
		printf("Block test\n"
			"  Start sector: %u\n"
			"  Num sectors: %u\n"
			"  Iterations: %u\n"
			"  Seed: 0x%x\n"
			"  Max sectors per op: %u\n"
			"  Sector state buffer: %p\n"
			"  Sector content buffer: %p\n"
			"  Device: %p\n"
			"  Number of blocks in device: %u\n"
			"  Block size: %u\n",
			start_sector,
			num_sectors,
			iterations,
			seed,
			max_sectors_per_op,
			sectors, buffer,
			dev,
			(unsigned) dev->lba,
			(unsigned) dev->blksz);
	}

  while (iterations --) {
		if (ctrlc()) {
			printf("^C\n");
			break;
		}

    /* Select sector */
    i = x0 % num_sectors;
    x0 = prng_next(x0);

    /* Select number of sectors */
		if (max_sectors_per_op > 1)
			o = 1 + x0 % (max_sectors_per_op - 1);
		else
			o = 1;

    if (i + o > num_sectors)
      o = num_sectors - i;
    x0 = prng_next(x0);

    /* lseek(fd, o_start_sector + i * SECTOR_SIZE, SEEK_SET) */

    write_not_read = !!(x0 & 1);
    x0 = prng_next_bit(x0);

    /* Read or write ? */
    if (write_not_read) {
      /* Write */
      if (verbose) printf("W %u %u", start_sector + i, o);

      for (j = 0; j < o; j ++) {
        sectors[i + j] = x0;
        sequence_fill(buffer + j * sector_size, x0, words_per_sector);
        if (verbose) printf(" 0x%08x", x0);
        x0 = prng_next(x0);
      }
      if (verbose) printf("\n");

			n = dev->block_write(dev->dev, start_sector + i, o, buffer);
			if (n != o) {
				printf("Write error.\n");
				goto fail;
			}
    } else {
      /* Read */
      if(verbose) printf("R %u %u\n", start_sector + i, o);

			n = dev->block_read(dev->dev, start_sector + i, o, buffer);

			if (n != o) {
				printf("Read error (got %d, expected %d).\n", n, o);
				goto fail;
			}

      for (j = 0; j < o; j ++) {
        /* Verify */
        if (sectors[i + j]) {
          if (!sequence_check(buffer + j * sector_size,
								sectors[i + j], words_per_sector)) {
            printf("READBACK MISMATCH on sector %u "
                    "(expected pattern 0x%08x, got 0x%08x)\n",
                    i + j,
                    sectors[i + j],
                    buffer + j * sector_size);
            sectors[i + j] = 0;
          }
        }
      }
    }
  }
	rc = 0;

fail:
	return rc;
}

int do_blktest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	block_dev_desc_t *dev = NULL;
	disk_partition_t info;
	int part = 1;
	uint32_t start_offset, num_sectors;
	uint32_t iterations = 100;
	uint32_t seed = 1234, max_sectors_per_op = 1;
	bool verbose = false;

	part = get_device_and_partition(argv[1], argv[2], &dev, &info, 1);
	if (part < 0)
		return 1;

	start_offset = simple_strtoul(argv[3], NULL, 16);
	num_sectors = simple_strtoul(argv[4], NULL, 16);
	if (argc > 5) {
		iterations = simple_strtoul(argv[5], NULL, 10);
		if (argc > 6) {
			seed = simple_strtoul(argv[6], NULL, 16);
			if (argc > 7) {
				max_sectors_per_op = simple_strtoul(argv[7], NULL, 16);
				if (argc > 8) {
					verbose = argv[8][0] == 'y';
				}
			}
		}
	}

	if (num_sectors == 0 || max_sectors_per_op == 0) {
		printf("Invalid arguments.\n");
		return CMD_RET_USAGE;
	}

	return blktest(dev,
		info.start + start_offset, num_sectors, iterations, seed,
		max_sectors_per_op, verbose);
}

U_BOOT_CMD(
	blktest, 9, 4, do_blktest,
	"destructive block device test",
	"<interface> <dev[:part]> <start-offset:hex> <num-sectors:hex> [<iterations> "
	"[<seed:hex> "
	"[<max-sectors-per-op:hex> "
	"[<verbose>]]]]"
);
