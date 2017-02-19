/*-
 * Copyright (c) 2014-2016 Dag-Erling Smørgrav
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

#include <stddef.h>
#include <stdint.h>

#undef HAVE_STRLCMP
#include <cryb/strlcmp.h>

#include <cryb/test.h>

#define T_MAGIC_STR	"squeamish ossifrage"
#define T_MAGIC_INITIAL	's'
#define T_MAGIC_LEN	(sizeof(T_MAGIC_STR) - 1)
#define T_BUFSIZE	(T_MAGIC_LEN + 1 + T_MAGIC_LEN + 1)

struct t_case {
	const char *desc;
	const char *pfx;
	const char *str;
	size_t len;
	int res;
};

/***************************************************************************
 * Test cases
 */
static struct t_case t_cases[] = {
	{
		.desc	= "empty, empty",
		.pfx	= "",
		.str	= "",
		.len	= 0,
		.res	= 0,
	},
	{
		.desc	= "empty, short",
		.pfx	= "",
		.str	= T_MAGIC_STR,
		.len	= 0,
		.res	= 0,
	},
	{
		.desc	= "short, empty",
		.pfx	= T_MAGIC_STR,
		.str	= "",
		.len	= T_MAGIC_LEN,
		.res	= T_MAGIC_INITIAL,
	},
	{
		.desc	= "short, short",
		.pfx	= T_MAGIC_STR,
		.str	= T_MAGIC_STR,
		.len	= T_MAGIC_LEN,
		.res	= 0,
	},
	{
		.desc	= "short, long",
		.pfx	= T_MAGIC_STR,
		.str	= T_MAGIC_STR " " T_MAGIC_STR,
		.len	= T_MAGIC_LEN,
		.res	= 0,
	},
	{
		.desc	= "long, short",
		.pfx	= T_MAGIC_STR " " T_MAGIC_STR,
		.str	= T_MAGIC_STR,
		.len	= T_MAGIC_LEN + 1 + T_MAGIC_LEN,
		.res	= ' ',
	},
	{
		.desc	= "mismatch",
		.pfx	= "sin",
		.str	= "sunny",
		.len	= 3,
		.res	= 'i' - 'u',
	},
	{
		.desc	= "too short",
		.pfx	= "sun",
		.str	= "sunny",
		.len	= 5,
		.res	= '\0' - 'n',
	},
};

/***************************************************************************
 * Test function
 */
static int
t_strlcmp(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	int res;

	res = strlcmp(t->pfx, t->str, t->len);
	return (t_compare_x8(t->res, res));
}


/***************************************************************************
 * Boilerplate
 */

static int
t_prepare(int argc, char *argv[])
{
	int i, n;

	(void)argc;
	(void)argv;
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_strlcmp, &t_cases[i], t_cases[i].desc);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
