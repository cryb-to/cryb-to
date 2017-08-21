/*-
 * Copyright (c) 2017 Dag-Erling Smørgrav
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
#include <unistd.h>

#include <cryb/strchrnul.h>

#include <cryb/test.h>

typedef char *(*strchrnul_f)(const char *, int);

#define T_MAGIC1_STR	"Squeamish"
#define T_MAGIC1_LEN	(sizeof(T_MAGIC1_STR) - 1)
#define T_MAGIC2_STR	"ossifragE"
#define T_MAGIC2_LEN	(sizeof(T_MAGIC2_STR) - 1)
#define T_MAGIC_STR	T_MAGIC1_STR " " T_MAGIC2_STR
#define T_MAGIC_LEN	(sizeof(T_MAGIC_STR) - 1)

const char t_empty_str[] = "";
const char t_magic_str[] = T_MAGIC_STR;

struct t_case {
	const char *desc;
	const char *str;
	int chr;
	const char *out;
};

/***************************************************************************
 * Test cases
 */
static struct t_case t_cases[] = {
	{
		.desc	= "zero in empty",
		.str	= t_empty_str,
		.chr	= '\0',
		.out	= t_empty_str,
	},
	{
		.desc	= "non-zero in empty",
		.str	= t_empty_str,
		.chr	= 'q',
		.out	= t_empty_str,
	},
	{
		.desc	= "zero in non-empty",
		.str	= t_magic_str,
		.chr	= '\0',
		.out	= t_magic_str + T_MAGIC_LEN,
	},
	{
		.desc	= "miss in non-empty",
		.str	= t_magic_str,
		.chr	= 'Z',
		.out	= t_magic_str + T_MAGIC_LEN,
	},
	{
		.desc	= "first in non-empty",
		.str	= t_magic_str,
		.chr	= 'S',
		.out	= t_magic_str,
	},
	{
		.desc	= "middle in non-empty",
		.str	= t_magic_str,
		.chr	= ' ',
		.out	= t_magic_str + T_MAGIC1_LEN,
	},
	{
		.desc	= "last in non-empty",
		.str	= t_magic_str,
		.chr	= 'E',
		.out	= t_magic_str + T_MAGIC_LEN - 1,
	},
};

/***************************************************************************
 * Test function
 */
static int
t_strchrnul(strchrnul_f func, const struct t_case *t)
{

	return (t_compare_ptr(t->out, func(t->str, t->chr)));
}

static int
t_cryb_strchrnul(char **desc CRYB_UNUSED, void *arg)
{
	const struct t_case *t = arg;

	return (t_strchrnul(cryb_strchrnul, t));
}

#if HAVE_STRCHRNUL
static int
t_libc_strchrnul(char **desc CRYB_UNUSED, void *arg)
{
	const struct t_case *t = arg;

	return (t_strchrnul(strchrnul, t));
}
#endif


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
		t_add_test(t_cryb_strchrnul, &t_cases[i],
		    "%s (cryb)", t_cases[i].desc);
#if HAVE_STRCHRNUL
	for (i = 0; i < n; ++i)
		t_add_test(t_libc_strchrnul, &t_cases[i],
		    "%s (libc)", t_cases[i].desc);
#endif
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
