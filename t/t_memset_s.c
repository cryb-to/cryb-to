/*-
 * Copyright (c) 2015 The University of Oslo
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

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#undef HAVE_MEMSET_S
#include <cryb/memset_s.h>

#include <cryb/test.h>

#define T_BUF_LEN	32

struct t_case {
	const char *desc;
	const char in[T_BUF_LEN];
	size_t dsz;
	int ch;
	size_t len;
	const char out[T_BUF_LEN];
	size_t outlen;
	int ret;
};

/***************************************************************************
 * Test cases
 */
static struct t_case t_cases[] = {
	{
		.desc	= "zero",
		.in	= "squeamish ossifrage",
		.dsz	= sizeof "squeamish ossifrage" - 1,
		.ch	= 'x',
		.len	= 0,
		.out	= "squeamish ossifrage",
		.ret	= 0,
	},
	{
		.desc	= "short",
		.in	= "squeamish ossifrage",
		.dsz	= sizeof "squeamish ossifrage" - 1,
		.ch	= 'x',
		.len	= 9,
		.out	= "xxxxxxxxx ossifrage",
		.ret	= 0,
	},
	{
		.desc	= "exact",
		.in	= "squeamish ossifrage",
		.dsz	= sizeof "squeamish ossifrage" - 1,
		.ch	= 'x',
		.len	= sizeof "squeamish ossifrage" - 1,
		.out	= "xxxxxxxxxxxxxxxxxxx",
		.ret	= 0,
	},
	{
		.desc	= "long",
		.in	= "squeamish ossifrage",
		.dsz	= sizeof "squeamish ossifrage" - 1,
		.ch	= 'x',
		.len	= sizeof "squeamish ossifrage" + 1,
		.out	= "xxxxxxxxxxxxxxxxxxx",
		.ret	= EOVERFLOW,
	},
};

/***************************************************************************
 * Test function
 */
static int
t_memset_s(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	char buf[T_BUF_LEN];
	int ret;

	memset(buf, 0, sizeof buf);
	strncpy(buf, t->in, sizeof buf);
	ret = memset_s(buf, t->dsz, t->ch, t->len);
	return (t_compare_i(t->ret, ret) &
	    t_compare_mem(t->out, buf, T_BUF_LEN));
}

/***************************************************************************
 * Test function (NULL)
 */
static int
t_memset_s_null(char **desc CRYB_UNUSED, void *arg)
{
	int ret;

	(void)arg;
	ret = memset_s(NULL, 1, 0, 1);
	return (t_compare_i(EINVAL, ret));
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
	t_add_test(t_memset_s_null, NULL, "null");
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_memset_s, &t_cases[i], "%s", t_cases[i].desc);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
