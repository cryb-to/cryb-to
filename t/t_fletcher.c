/*-
 * Copyright (c) 2015 Dag-Erling Smørgrav
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "cryb/impl.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cryb/endian.h>
#include <cryb/hash.h>

#include <cryb/test.h>

#define FLETCHER16_INVALID	0xffffU
#define FLETCHER32_INVALID	0xffffffffLU
#define FLETCHER64_INVALID	0xffffffffffffffffLLU

static uint8_t ones8[65536];
static uint16_t ones16[65536];
static uint32_t ones32[65536];

struct t_case {
	const char *desc;
	const void *data;
	size_t len;
	uint16_t sum16;
	uint32_t sum32;
	uint64_t sum64;
};

/***************************************************************************
 * Test cases
 *
 * Most of these test cases use input sequences where each word has the
 * value 1.  To understand the test cases, it helps to realize that (until
 * it overflows) the low word of the Fletcher checksum of such a sequence
 * of length n is equal to n, while the high word is equal to the nth
 * triangular number, i.e. (n*(n+1))/2+n.  Also, remember that the input
 * width for Fletcher is half the checksum width.
 */
#define TRI(n) (n * (n + 1) / 2)
static struct t_case t_cases[] = {
	/* trivial cases */
	{
		.desc	= "all-zeroes message",
		.data	= t_zero,
		.len	= sizeof t_zero,
		.sum16	= 0x00,
		.sum32	= 0x0000,
		.sum64	= 0x00000000,
	},
	{
		.desc	= "length 0",
		.data	= ones8,
		.len	= 0,
		.sum16	= 0x00,
		.sum32	= 0x0000,
		.sum64	= 0x00000000,
	},

	/* padding */
	{
		.desc	= "length 1",
		.data	= ones8,
		.len	= 1,
		.sum16	= 0x0101,
		.sum32	= 0x01000100,
		.sum64	= 0x0100000001000000,
	},
	{
		.desc	= "length 2",
		.data	= ones8,
		.len	= 2,
		.sum16	= 0x0302,
		.sum32	= 0x01010101,
		.sum64	= 0x0101000001010000,
	},
	{
		.desc	= "length 3",
		.data	= ones8,
		.len	= 3,
		.sum16	= 0x0603,
		.sum32	= 0x03020201,
		.sum64	= 0x0101010001010100,
	},
	{
		.desc	= "length 4",
		.data	= ones8,
		.len	= 4,
		.sum16	= 0x0a04,
		.sum32	= 0x03030202,
		.sum64	= 0x0101010101010101,
	},
	{
		.desc	= "length 5",
		.data	= ones8,
		.len	= 5,
		.sum16	= 0x0f05,
		.sum32	= 0x06050302,
		.sum64	= 0x0302020202010101,
	},
	{
		.desc	= "length 6",
		.data	= ones8,
		.len	= 6,
		.sum16	= 0x1506,
		.sum32	= 0x06060303,
		.sum64	= 0x0303020202020101,
	},
	{
		.desc	= "length 7",
		.data	= ones8,
		.len	= 7,
		.sum16	= 0x1c07,
		.sum32	= 0x0a090403,
		.sum64	= 0x0303030202020201,
	},
	{
		.desc	= "length 8",
		.data	= ones8,
		.len	= 8,
		.sum16	= 0x2408,
		.sum32	= 0x0a0a0404,
		.sum64	= 0x0303030302020202,
	},

	/* overflow (16-bit checksum) */
	{
		.desc	= "high overflow - 1",
		.data	= ones8,
		.len	= 22,
		.sum16	= (TRI(22) % 255) << 8 | 22,
		.sum32	= FLETCHER32_INVALID,
		.sum64	= FLETCHER64_INVALID,
	},
	{
		.desc	= "high overflow",
		.data	= ones8,
		.len	= 23,
		.sum16	= (TRI(23) % 255) << 8 | 23,
		.sum32	= FLETCHER32_INVALID,
		.sum64	= FLETCHER64_INVALID,
	},
	{
		.desc	= "low overflow - 1",
		.data	= ones8,
		.len	= 254,
		.sum16	= (TRI(254) % 255) << 8 | 254,
		.sum32	= FLETCHER32_INVALID,
		.sum64	= FLETCHER64_INVALID,
	},
	{
		.desc	= "low overflow",
		.data	= ones8,
		.len	= 255,
		.sum16	= (TRI(254) % 255) << 8 | 0,
		.sum32	= FLETCHER32_INVALID,
		.sum64	= FLETCHER64_INVALID,
	},
	{
		.desc	= "low overflow + 1",
		.data	= ones8,
		.len	= 256,
		.sum16	= ((TRI(254) + 1) % 255) << 8 | 1,
		.sum32	= FLETCHER32_INVALID,
		.sum64	= FLETCHER64_INVALID,
	},

	/* overflow (32-bit checksum) */
	{
		.desc	= "high overflow - 1",
		.data	= ones16,
		.len	= 362 * sizeof ones16[0],
		.sum16	= FLETCHER16_INVALID,
		.sum32	= (TRI(362U) % 65535U) << 16 | 362U,
		.sum64	= FLETCHER64_INVALID,
	},
	{
		.desc	= "high overflow",
		.data	= ones16,
		.len	= 363 * sizeof ones16[0],
		.sum16	= FLETCHER16_INVALID,
		.sum32	= (TRI(363U) % 65535U) << 16 | 363U,
		.sum64	= FLETCHER64_INVALID,
	},
	{
		.desc	= "low overflow - 1",
		.data	= ones16,
		.len	= 65534 * sizeof ones16[0],
		.sum16	= FLETCHER16_INVALID,
		.sum32	= (TRI(65534U) % 65535U) << 16 | 65534U,
		.sum64	= FLETCHER64_INVALID,
	},
	{
		.desc	= "low overflow",
		.data	= ones16,
		.len	= 65535 * sizeof ones16[0],
		.sum16	= FLETCHER16_INVALID,
		.sum32	= (TRI(65534U) % 65535U) << 16 | 0U,
		.sum64	= FLETCHER64_INVALID,
	},
	{
		.desc	= "low overflow + 1",
		.data	= ones16,
		.len	= 65536 * sizeof ones16[0],
		.sum16	= FLETCHER16_INVALID,
		.sum32	= ((TRI(65534U) + 1) % 65535U) << 16 | 1U,
		.sum64	= FLETCHER64_INVALID,
	},

	/* overflow (64-bit checksum */
	/*
	 * This is currently not realistic.  We need to change the
	 * fletcher*() API to take a seed so it can do partial /
	 * incremental checksums, and provide a precomputed seed just
	 * below the overflow point.
	 */
};

/***************************************************************************
 * Test function
 */
static int
t_fletcher16(char **desc, void *arg)
{
	struct t_case *t = arg;

	asprintf(desc, "(16-bit) %s", t->desc);
	return (t_compare_x16(t->sum16, fletcher16_hash(t->data, t->len)));
}

static int
t_fletcher32(char **desc, void *arg)
{
	struct t_case *t = arg;

	asprintf(desc, "(32-bit) %s", t->desc);
	return (t_compare_x32(t->sum32, fletcher32_hash(t->data, t->len)));
}

static int
t_fletcher64(char **desc, void *arg)
{
	struct t_case *t = arg;

	asprintf(desc, "(64-bit) %s", t->desc);
	return (t_compare_x64(t->sum64, fletcher64_hash(t->data, t->len)));
}


/***************************************************************************
 * Boilerplate
 */

int
t_prepare(int argc, char *argv[])
{
	unsigned int i, n;

	(void)argc;
	(void)argv;
	ones8[0] = 1;
	be16enc(&ones16[0], 1);
	be32enc(&ones32[0], 1);
	for (i = 1; i < 65536; ++i) {
		ones8[i] = ones8[0];
		ones16[i] = ones16[0];
		ones32[i] = ones32[0];
	}
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i)
		if (t_cases[i].sum16 != 0xffffU)
			t_add_test(t_fletcher16, &t_cases[i], t_cases[i].desc);
	for (i = 0; i < n; ++i)
		if (t_cases[i].sum32 != 0xffffffffLU)
			t_add_test(t_fletcher32, &t_cases[i], t_cases[i].desc);
	for (i = 0; i < n; ++i)
		if (t_cases[i].sum64 != 0xffffffffffffffffLLU)
			t_add_test(t_fletcher64, &t_cases[i], t_cases[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
