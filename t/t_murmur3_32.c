/*-
 * Copyright (c) 2014 Dag-Erling Smørgrav
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

#include <cryb/test.h>

struct t_case {
	const char *desc;
	const char *data;
	size_t len;
	uint32_t seed;
	uint32_t hash;
};

/***************************************************************************
 * Test cases
 */
static struct t_case t_cases[] = {
	{
		.desc	= "00000000 { }",
		.data	= "murmur3",
		.len	= 0,
		.seed	= 0x00000000,
		.hash	= 0x00000000,
	},
	{
		.desc	= "ffffffff { }",
		.data	= "murmur3",
		.len	= 0,
		.seed	= 0xffffffff,
		.hash	= 0x81f16f39,
	},
	{
		.desc	= "00000000 { 'm' }",
		.data	= "murmur3",
		.len	= 1,
		.seed	= 0x00000000,
		.hash	= 0x5ae4385c,
	},
	{
		.desc	= "ffffffff { 'm' }",
		.data	= "murmur3",
		.len	= 1,
		.seed	= 0xffffffff,
		.hash	= 0x7aca074a,
	},
	{
		.desc	= "00000000 { 'm' 'u' }",
		.data	= "murmur3",
		.len	= 2,
		.seed	= 0x00000000,
		.hash	= 0xb93a5ae8,
	},
	{
		.desc	= "ffffffff { 'm' 'u' }",
		.data	= "murmur3",
		.len	= 2,
		.seed	= 0xffffffff,
		.hash	= 0xb3906f0a,
	},
	{
		.desc	= "00000000 { 'm' 'u' 'r' }",
		.data	= "murmur3",
		.len	= 3,
		.seed	= 0x00000000,
		.hash	= 0x44b7f9ca,
	},
	{
		.desc	= "ffffffff { 'm' 'u' 'r' }",
		.data	= "murmur3",
		.len	= 3,
		.seed	= 0xffffffff,
		.hash	= 0xe75eba0,
	},
	{
		.desc	= "00000000 { 'm' 'u' 'r' 'm' }",
		.data	= "murmur3",
		.len	= 4,
		.seed	= 0x00000000,
		.hash	= 0x48bcf87d,
	},
	{
		.desc	= "ffffffff { 'm' 'u' 'r' 'm' }",
		.data	= "murmur3",
		.len	= 4,
		.seed	= 0xffffffff,
		.hash	= 0x5771291b,
	},
	{
		.desc	= "00000000 { 'm' 'u' 'r' 'm' 'u' }",
		.data	= "murmur3",
		.len	= 5,
		.seed	= 0x00000000,
		.hash	= 0x6865a0a3,
	},
	{
		.desc	= "ffffffff { 'm' 'u' 'r' 'm' 'u' }",
		.data	= "murmur3",
		.len	= 5,
		.seed	= 0xffffffff,
		.hash	= 0xbed94c57,
	},
	{
		.desc	= "00000000 { 'm' 'u' 'r' 'm' 'u' 'r' }",
		.data	= "murmur3",
		.len	= 6,
		.seed	= 0x00000000,
		.hash	= 0x73f313cd,
	},
	{
		.desc	= "ffffffff { 'm' 'u' 'r' 'm' 'u' 'r' }",
		.data	= "murmur3",
		.len	= 6,
		.seed	= 0xffffffff,
		.hash	= 0x1a77802a,
	},
	{
		.desc	= "00000000 { 'm' 'u' 'r' 'm' 'u' 'r' '3' }",
		.data	= "murmur3",
		.len	= 7,
		.seed	= 0x00000000,
		.hash	= 0x845098a5,
	},
	{
		.desc	= "ffffffff { 'm' 'u' 'r' 'm' 'u' 'r' '3' }",
		.data	= "murmur3",
		.len	= 7,
		.seed	= 0xffffffff,
		.hash	= 0x6e66a973,
	},
	{
		.desc	= "00000000 { 'm' 'u' 'r' 'm' 'u' 'r' '3' '\\0' }",
		.data	= "murmur3",
		.len	= 8,
		.seed	= 0x00000000,
		.hash	= 0x7d525b5e,
	},
	{
		.desc	= "ffffffff { 'm' 'u' 'r' 'm' 'u' 'r' '3' '\\0' }",
		.data	= "murmur3",
		.len	= 8,
		.seed	= 0xffffffff,
		.hash	= 0x18b2eab2,
	},
};

/***************************************************************************
 * Test function
 */
static int
t_murmur3_32(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	uint32_t hash;

	hash = murmur3_32_hash(t->data, t->len, t->seed);
	return (t_compare_x32(t->hash, hash));
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
		t_add_test(t_murmur3_32, &t_cases[i], t_cases[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
