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

#include <sys/types.h>

#include <stdint.h>
#include <string.h>

#include <cryb/endian.h>
#include <cryb/hash.h>

#include <cryb/test.h>

#define ADLER32_INVALID	0xffffffffLU

#define TRI(n)			(n * (n + 1) / 2)

static uint8_t ones8[65536];

struct t_case {
	const char *desc;
	const void *data;
	size_t len;
	uint32_t sum;
};

/***************************************************************************
 * Test cases
 *
 * Most of these test cases use input sequences where each word has the
 * value 1.  To understand the test cases, it helps to realize that (until
 * it overflows) the low word of the Adler checksum of such a sequence of
 * length n is equal to n + 1, while the high word is equal to the nth
 * triangular number plus n, i.e. (n*(n+1))/2+n.  Also, unlike Fletcher32,
 * the input width for Adler32 is always 8 bits.
 */
#define TRIpN(n) ((n * (n + 1) / 2) + n)
static struct t_case t_cases[] = {
	/* trivial cases */
	{
		.desc	= "all-zeroes message",
		.data	= t_zero,
		.len	= sizeof t_zero,
		.sum	= sizeof t_zero << 16 | 0x0001U,
	},
	{
		.desc	= "length 0",
		.data	= ones8,
		.len	= 0,
		.sum	= 0x00000001,
	},
	{
		.desc	= "squeamish ossifrage",
		.data	= "the magic words are squeamish ossifrage",
		.len	= 39,
		.sum	= 0x23880edd,
	},

	/* overflow */
	{
		.desc	= "high overflow - 1",
		.data	= ones8,
		.len	= 359,
		.sum	= (TRIpN(359U) % 65521U) << 16 | 360U,
	},
	{
		.desc	= "high overflow",
		.data	= ones8,
		.len	= 363,
		.sum	= (TRIpN(363U) % 65521U) << 16 | 364U,
	},
	{
		.desc	= "low overflow - 1",
		.data	= ones8,
		.len	= 65519,
		.sum	= (TRIpN(65519U) % 65521U) << 16 | 65520U,
	},
	{
		.desc	= "low overflow",
		.data	= ones8,
		.len	= 65520,
		.sum	= (TRIpN(65520U) % 65521U) << 16 | 0U,
	},
	{
		.desc	= "low overflow + 1",
		.data	= ones8,
		.len	= 65521,
		.sum	= ((TRIpN(65520U) + 1) % 65521U) << 16 | 1U,
	},
};

/***************************************************************************
 * Test function
 */
static int
t_adler32(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;

	return (t_compare_x32(t->sum, adler32_hash(t->data, t->len)));
}


/***************************************************************************
 * Boilerplate
 */

static int
t_prepare(int argc, char *argv[])
{
	unsigned int i, n;

	(void)argc;
	(void)argv;
	memset(&ones8, 1, sizeof ones8);
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i)
		if (t_cases[i].sum != 0xffffffffLU)
			t_add_test(t_adler32, &t_cases[i], t_cases[i].desc);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
