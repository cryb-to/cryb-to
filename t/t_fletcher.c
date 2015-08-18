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
#include <string.h>

#include <cryb/hash.h>

#include "t.h"

struct t_case {
	const char *desc;
	const char *data;
	size_t len;
	uint16_t sum16;
	uint32_t sum32;
	uint64_t sum64;
};

/***************************************************************************
 * Test cases
 */
static struct t_case t_cases[] = {
	{
		.desc	= "len == 0",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 0,
		.sum16	= 0x00,
		.sum32	= 0x0000,
		.sum64	= 0x00000000,
	},
	{
		.desc	= "len == 1",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 1,
		.sum16	= 0x0101,
		.sum32	= 0x01000100,
		.sum64	= 0x0100000001000000,
	},
	{
		.desc	= "len == 2",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 2,
		.sum16	= 0x0403,
		.sum32	= 0x01020102,
		.sum64	= 0x0102000001020000,
	},
	{
		.desc	= "len == 3",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 3,
		.sum16	= 0x0a06,
		.sum32	= 0x05040402,
		.sum64	= 0x102030001020300,
	},
	{
		.desc	= "len == 4",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 4,
		.sum16	= 0x140a,
		.sum32	= 0x05080406,
		.sum64	= 0x0102030401020304,
	},
	{
		.desc	= "len == 5",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 5,
		.sum16	= 0x230f,
		.sum32	= 0x0e0e0906,
		.sum64	= 0x0704060806020304,
	},
	{
		.desc	= "len == 6",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 6,
		.sum16	= 0x3815,
		.sum32	= 0x0e14090c,
		.sum64	= 0x070a060806080304,
	},
	{
		.desc	= "len == 7",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 7,
		.sum16	= 0x541c,
		.sum32	= 0x1e20100c,
		.sum64	= 0x070a0d0806080a04,
	},
	{
		.desc	= "len == 8",
		.data	= "\x01\x02\x03\x04\x05\x06\x07\x08",
		.len	= 8,
		.sum16	= 0x7824,
		.sum32	= 0x1e281014,
		.sum64	= 0x070a0d1006080a0c,
	},
	/* not really useful... */
	{
		.desc	= "zeroes",
		.data	= (const char *)t_zero,
		.len	= sizeof t_zero,
		.sum16	= 0x00,
		.sum32	= 0x0000,
		.sum64	= 0x00000000,
	},
	/* XXX need non-zero cases which exercise modulo */
};

/***************************************************************************
 * Test function
 */
static int
t_fletcher(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	int ret;

	ret = t_compare_x16(t->sum16, fletcher16_hash(t->data, t->len)) &
	    t_compare_x32(t->sum32, fletcher32_hash(t->data, t->len)) &
	    t_compare_x64(t->sum64, fletcher64_hash(t->data, t->len));
	return (ret);
}


/***************************************************************************
 * Boilerplate
 */

int
t_prepare(int argc, char *argv[])
{
	int i, n;

	(void)argc;
	(void)argv;
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_fletcher, &t_cases[i], t_cases[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
