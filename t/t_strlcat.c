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

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#undef HAVE_STRLCAT
#include <cryb/strlcat.h>

#include <cryb/test.h>

#define T_MAGIC_STR	"squeamish ossifrage"
#define T_MAGIC_LEN	(sizeof(T_MAGIC_STR) - 1)
#define T_BUFSIZE	(T_MAGIC_LEN + 1 + T_MAGIC_LEN + 1)
#define T_CANARY	0x7f

struct t_case {
	const char *desc;
	const char buf[T_BUFSIZE];
	const char *in;
	const char *out;
	size_t sz;
};

/***************************************************************************
 * Test cases
 */
static struct t_case t_cases[] = {
	{
		.desc	= "empty into empty",
		.buf	= "",
		.in	= "",
		.out	= "",
		.sz	= 0,
	},
	{
		.desc	= "empty into non-empty",
		.buf	= T_MAGIC_STR,
		.in	= "",
		.out	= T_MAGIC_STR,
		.sz	= T_MAGIC_LEN,
	},
	{
		.desc	= "empty into full",
		.buf	= T_MAGIC_STR " " T_MAGIC_STR,
		.in	= "",
		.out	= T_MAGIC_STR " " T_MAGIC_STR,
		.sz	= T_MAGIC_LEN + 1 + T_MAGIC_LEN,
	},
	{
		.desc	= "non-empty into empty",
		.buf	= "",
		.in	= T_MAGIC_STR,
		.out	= T_MAGIC_STR,
		.sz	= T_MAGIC_LEN,
	},
	{
		.desc	= "empty into unterminated buffer",
		.buf	= T_MAGIC_STR " " T_MAGIC_STR " ",
		.in	= "",
		.out	= NULL,
		.sz	= T_MAGIC_LEN + 1 + T_MAGIC_LEN + 1,
	},
	{
		.desc	= "non-empty into non-empty",
		.buf	= T_MAGIC_STR,
		.in	= " " T_MAGIC_STR,
		.out	= T_MAGIC_STR " " T_MAGIC_STR,
		.sz	= T_MAGIC_LEN + 1 + T_MAGIC_LEN,
	},
	{
		.desc	= "non-empty into non-empty (overflow)",
		.buf	= T_MAGIC_STR,
		.in	= " " T_MAGIC_STR " " T_MAGIC_STR,
		.out	= T_MAGIC_STR " " T_MAGIC_STR,
		.sz	= T_MAGIC_LEN + 1 + T_MAGIC_LEN + 1 + T_MAGIC_LEN,
	},
	{
		.desc	= "non-empty into full (overflow)",
		.buf	= T_MAGIC_STR " " T_MAGIC_STR,
		.in	= " " T_MAGIC_STR,
		.out	= T_MAGIC_STR " " T_MAGIC_STR,
		.sz	= T_MAGIC_LEN + 1 + T_MAGIC_LEN + 1 + T_MAGIC_LEN,
	},
	{
		.desc	= "non-empty into unterminated buffer",
		.buf	= T_MAGIC_STR " " T_MAGIC_STR " ",
		.in	= T_MAGIC_STR,
		.out	= NULL,
		.sz	= T_MAGIC_LEN + 1 + T_MAGIC_LEN + 1 + T_MAGIC_LEN,
	},
};

/***************************************************************************
 * Test function
 */
static int
t_strlcat(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	char buf[T_BUFSIZE + 1];
	size_t sz;
	int ret;

	memcpy(buf, t->buf, sizeof t->buf);
	buf[T_BUFSIZE] = T_CANARY;
	sz = strlcat(buf, t->in, T_BUFSIZE);
	if (buf[T_BUFSIZE] != T_CANARY) {
		t_printv("buffer overflow\n");
		return (0);
	}
	ret = t_compare_sz(t->sz, sz);
	if (t->out != NULL)
		ret &= t_compare_str(t->out, buf);
	return (ret);
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
		t_add_test(t_strlcat, &t_cases[i], t_cases[i].desc);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
