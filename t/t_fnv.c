/*-
 * Copyright (c) 2014-2022 Dag-Erling Smørgrav
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
#include <unistd.h>

#include <cryb/hash.h>

#include <cryb/test.h>

#include "cryb_fnv_impl.h"

struct t_case {
	const char *str;
	uint32_t h0_32, h1_32, h1a_32;
	uint64_t h0_64, h1_64, h1a_64;
};

/***************************************************************************
 * Test cases
 */

/*
 * Test vectors from the IETF draft, expanded to also cover FNV-0 and FNV-1.
 */
static struct t_case t_cases[] = {
	{
		.str	= "",
		.h0_32	= 0x00000000UL,
		.h1_32	= 0x811c9dc5UL,
		.h1a_32	= 0x811c9dc5UL,
		.h0_64	= 0x0000000000000000ULL,
		.h1_64	= 0xcbf29ce484222325ULL,
		.h1a_64 = 0xcbf29ce484222325ULL,
	},
	{
		.str	= "a",
		.h0_32	= 0x00000061UL,
		.h1_32	= 0x050c5d7eUL,
		.h1a_32	= 0xe40c292cUL,
		.h0_64	= 0x0000000000000061ULL,
		.h1_64	= 0xaf63bd4c8601b7beULL,
		.h1a_64	= 0xaf63dc4c8601ec8cULL,
	},
	{
		.str	= "foobar",
		.h0_32	= 0xb74bb5efUL,
		.h1_32	= 0x31f0b262UL,
		.h1a_32	= 0xbf9cf968UL,
		.h0_64	= 0x0b91ae3f7ccdc5efULL,
		.h1_64	= 0x340d8765a4dda9c2ULL,
		.h1a_64	= 0x85944171f73967e8ULL,
	},
};

/*
 * Test case for the 32- and 64-bit offset bases.
 */
static struct t_case t_offset_basis = {
	.str	= FNV_SIGNATURE,
	.h0_32	= FNV_32_OFFSET_BASIS,
	.h0_64	= FNV_64_OFFSET_BASIS,
};

/***************************************************************************
 * Test functions
 */
#define T_FNV(V, B)							\
	static int							\
	t_fnv##V##_##B(char **desc, void *arg)				\
	{								\
		struct t_case *t = arg;					\
		uint##B##_t h;						\
									\
		if (!**desc) {						\
			(void)asprintf(desc,				\
			    "FNV-" #V "-" #B " \"%s\"", t->str);	\
		}							\
		h = fnv##V##_##B##_hash(t->str, strlen(t->str));	\
		return (t_compare_x##B(t->h##V##_##B, h));		\
	}

T_FNV(0, 32)
T_FNV(0, 64)
T_FNV(1, 32)
T_FNV(1, 64)
T_FNV(1a, 32)
T_FNV(1a, 64)


/***************************************************************************
 * Boilerplate
 */

static int
t_prepare(int argc, char *argv[])
{
	int i, n;

	(void)argc;
	(void)argv;
	t_add_test(t_fnv0_32, &t_offset_basis, "FNV-32 offset basis");
	t_add_test(t_fnv0_64, &t_offset_basis, "FNV-64 offset basis");
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i) {
		t_add_test(t_fnv0_32, &t_cases[i], "FNV-0-32");
		t_add_test(t_fnv0_64, &t_cases[i], "FNV-0-64");
		t_add_test(t_fnv1_32, &t_cases[i], "FNV-1-32");
		t_add_test(t_fnv1_64, &t_cases[i], "FNV-1-64");
		t_add_test(t_fnv1a_32, &t_cases[i], "FNV-1a-32");
		t_add_test(t_fnv1a_64, &t_cases[i], "FNV-1a-64");
	}
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
