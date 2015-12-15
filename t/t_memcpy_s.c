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

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#undef HAVE_MEMCPY_S
#include <cryb/memcpy_s.h>
#include <cryb/strlcpy.h>
#include <cryb/test.h>

#define T_BUF_LEN	40

struct t_plain_case {
	const char *desc;
	const char in[T_BUF_LEN];
	size_t dsz;
	const char s[T_BUF_LEN];
	size_t len;
	const char out[T_BUF_LEN];
	size_t outlen;
	int ret;
};

/***************************************************************************
 * Plain test cases
 */
static struct t_plain_case t_plain_cases[] = {
	{
		.desc	= "zero",
		.in	= "squeamish ossifrage",
		.dsz	= sizeof "squeamish ossifrage" - 1,
		.s	= "",
		.len	= 0,
		.out	= "squeamish ossifrage",
		.ret	= 0,
	},
	{
		.desc	= "short",
		.in	= "squeamish ossifrage",
		.dsz	= sizeof "squeamish ossifrage" - 1,
		.s	= "the quick",
		.len	= sizeof "the quick" - 1,
		.out	= "the quick ossifrage",
		.ret	= 0,
	},
	{
		.desc	= "exact",
		.in	= "squeamish ossifrage",
		.dsz	= sizeof "squeamish ossifrage" - 1,
		.s	= "the quick brown fox",
		.len	= sizeof "the quick brown fox" - 1,
		.out	= "the quick brown fox",
		.ret	= 0,
	},
	{
		.desc	= "long",
		.in	= "squeamish ossifrage",
		.dsz	= sizeof "squeamish ossifrage" - 1,
		.s	= "the quick brown fox jumps",
		.len	= sizeof "the quick brown fox jumps" + 1,
		.out	= "",
		.ret	= EOVERFLOW,
	},
};

static int
t_memcpy_s(char **desc CRYB_UNUSED, void *arg)
{
	struct t_plain_case *t = arg;
	char buf[T_BUF_LEN];
	int ret;

	memset(buf, 0, sizeof buf);
	strlcpy(buf, t->in, T_BUF_LEN);
	ret = memcpy_s(buf, t->dsz, t->s, t->len);
	return (t_compare_i(t->ret, ret) &
	    t_compare_mem(t->out, buf, T_BUF_LEN));
}

/***************************************************************************
 * Overlapping test cases
 */

struct t_overlap_case {
	const char *desc;
	const char *in;
	size_t s_off, s_len, d_off, d_len;
	const char out[T_BUF_LEN];
	int ret;
};

static struct t_overlap_case t_overlap_cases[] = {
	{
		/* [<<<<<<<<<< >>>>>>>>>>] */
		.desc	= "left, disjoint",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 0,
		.s_len	= 15,
		.d_off	= 16,
		.d_len	= 23,
		.out	= "the magic words the magic wordsssifrage",
		.ret	= 0,
	},
	{
		/* [<<<<<<<<<<>>>>>>>>>>] */
		.desc	= "left, adjoining",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 0,
		.s_len	= 16,
		.d_off	= 16,
		.d_len	= 23,
		.out	= "the magic words the magic words sifrage",
		.ret	= 0,
	},
	{
		/* [>>>>>>>>>> <<<<<<<<<<] */
		.desc	= "right, disjoint",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 20,
		.s_len	= 19,
		.d_off	= 0,
		.d_len	= 19,
		.out	= "squeamish ossifrage squeamish ossifrage",
		.ret	= 0,
	},
	{
		/* [>>>>>>>>>><<<<<<<<<<] */
		.desc	= "right, adjoining",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 20,
		.s_len	= 19,
		.d_off	= 0,
		.d_len	= 19,
		.out	= "squeamish ossifrage squeamish ossifrage",
		.ret	= 0,
	},
	{
		/* [<<<<<<<<XXXX>>>>>>>>] */
		.desc	= "left, overlapping",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 4,
		.s_len	= 15,
		.d_off	= 16,
		.d_len	= 23,
		.out	= "the magic words ",
		.ret	= EINVAL,
	},
	{
		/* [>>>>>>>>XXXX<<<<<<<<] */
		.desc	= "right, overlapping",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 16,
		.s_len	= 23,
		.d_off	= 0,
		.d_len	= 29,
		.out	= "\0\0\0\0\0\0\0\0\0\0"
			  "\0\0\0\0\0\0\0\0\0\0"
			  "\0\0\0\0\0\0\0\0\0"
			  " ossifrage",
		.ret	= EINVAL,
	},
	{
		/* [>>>>>>>>XXXX>>>>>>>>] */
		.desc	= "inside",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 10,
		.s_len	= 19,
		.d_off	= 0,
		.d_len	= 39,
		.out	= "",
		.ret	= EINVAL,
	},
	{
		/* [<<<<<<<<XXXX<<<<<<<<] */
		/* caught by n > sz before overlap is noticed */
		.desc	= "outside",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 0,
		.s_len	= 39,
		.d_off	= 10,
		.d_len	= 19,
		.out	= "the magic "
			  "\0\0\0\0\0\0\0\0\0\0"
			  "\0\0\0\0\0\0\0\0\0"
			  " ossifrage",
		.ret	= EOVERFLOW,
	},
	{
		/* [XXXXXXXXXXXXXXXXXXXX] */
		.desc	= "full overlap",
		.in	= "the magic words are squeamish ossifrage",
		.s_off	= 10,
		.s_len	= 19,
		.d_off	= 10,
		.d_len	= 19,
		.out	= "the magic "
			  "\0\0\0\0\0\0\0\0\0\0"
			  "\0\0\0\0\0\0\0\0\0"
			  " ossifrage",
		.ret	= EINVAL,
	},
};

static int
t_memcpy_s_overlap(char **desc CRYB_UNUSED, void *arg)
{
	struct t_overlap_case *t = arg;
	char buf[T_BUF_LEN];
	int ret;

	strlcpy(buf, t->in, sizeof buf);
	ret = memcpy_s(buf + t->d_off, t->d_len, buf + t->s_off, t->s_len);
	return (t_compare_i(t->ret, ret) &
	    t_compare_mem(t->out, buf, T_BUF_LEN));
}

/***************************************************************************
 * NULL cases
 */
static int
t_memcpy_s_null_d(char **desc CRYB_UNUSED, void *arg)
{
	char buf[T_BUF_LEN];
	int ret;

	(void)arg;
	memset(buf, 'x', sizeof buf);
	ret = memcpy_s(NULL, sizeof buf, buf, sizeof buf);
	return (t_compare_i(EINVAL, ret));
}

static int
t_memcpy_s_null_s(char **desc CRYB_UNUSED, void *arg)
{
	char buf[T_BUF_LEN];
	int ret;

	(void)arg;
	memset(buf, 0, sizeof buf);
	ret = memcpy_s(buf, sizeof buf, NULL, sizeof buf);
	return (t_compare_i(EINVAL, ret) &
	    t_compare_mem(buf, t_zero, sizeof buf));
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
	t_add_test(t_memcpy_s_null_s, NULL, "null source");
	t_add_test(t_memcpy_s_null_d, NULL, "null destination");
	n = sizeof t_plain_cases / sizeof t_plain_cases[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_memcpy_s, &t_plain_cases[i],
		    t_plain_cases[i].desc);
	n = sizeof t_overlap_cases / sizeof t_overlap_cases[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_memcpy_s_overlap, &t_overlap_cases[i],
		    t_overlap_cases[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
