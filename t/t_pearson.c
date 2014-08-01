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
#include <stdio.h>
#include <string.h>

#include <cryb/hash.h>

#include "t.h"

#define T_MAGIC_WORD_1	"squeamish"
#define T_MAGIC_WORD_2	"ossifrage"
#define T_MAGIC_STR	T_MAGIC_WORD_1 " " T_MAGIC_WORD_2

struct t_case {
	const char *desc;
	const char *str;
	uint8_t hash;
};

/***************************************************************************
 * Test cases
 */
static struct t_case t_cases[] = {
	{
		.desc	= "\"\"",
		.str	= "",
		.hash	= 0x00,
	},
	{
		.desc	= "\"" T_MAGIC_WORD_1 "\"",
		.str	= T_MAGIC_WORD_1,
		.hash	= 0xe9,
	},
	{
		.desc	= "\"" T_MAGIC_WORD_2 "\"",
		.str	= T_MAGIC_WORD_2,
		.hash	= 0x47,
	},
	{
		.desc	= "\"" T_MAGIC_STR "\"",
		.str	= T_MAGIC_STR,
		.hash	= 0xba,
	},
};

/***************************************************************************
 * Test function
 */
static int
t_pearson(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	int ret;

	ret = t_compare_x8(t->hash, pearson_hash(t->str, strlen(t->str)));
	ret &= t_compare_x8(t->hash, pearson_hash_str(t->str));
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
		t_add_test(t_pearson, &t_cases[i], t_cases[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
