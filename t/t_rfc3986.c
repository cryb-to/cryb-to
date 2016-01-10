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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cryb/rfc3986.h>

#include <cryb/test.h>

struct t_case {
	const char *desc;
	int (*func)(const char *, size_t, char *, size_t *);
	const char *in;		/* input string  */
	size_t ilen;		/* input length */
	const char *out;	/* expected output string or NULL */
	size_t blen;		/* initial value for olen or 0 */
	size_t olen;		/* expected value for olen */
	int ret;		/* expected return value */
	int err;		/* expected errno if ret != 0 */
};

/* basic encoding / decoding */
#define T_ENCODE(i, o)							\
	{ .func = percent_encode,					\
	  .in = i, .ilen = sizeof i,					\
	  .out = o, .blen = sizeof o, .olen = sizeof o,			\
	  .ret = 0, .err = 0 }
#define T_DECODE(i, o)							\
	{ .func = percent_decode,					\
	  .in = i, .ilen = sizeof i,					\
	  .out = o, .blen = sizeof o, .olen = sizeof o,			\
	  .ret = 0, .err = 0 }

/* roundtrip encoding tests */
#define T_ENCDEC(p, e)							\
	T_ENCODE(p, e), T_DECODE(e, p)

/* decoding failure */
#define T_DECODE_FAIL(e, i, o)						\
	{ .func = percent_decode,					\
	  .in = i, .ilen = sizeof i - 1,				\
	  .out = o, .blen = SIZE_MAX, .olen = sizeof o,			\
	  .ret = -1, .err = e }

/* input string shorter than input length */
#define T_SHORT_INPUT_ENC(i, o)						\
	{ .desc = "encode (short input)", .func = percent_encode,	\
	  .in = i, .ilen = SIZE_MAX,					\
	  .out = o, .blen = SIZE_MAX, .olen = sizeof o }
#define T_SHORT_INPUT_DEC(i, o)						\
	{ .desc = "decode (short input)", .func = percent_decode,	\
	  .in = i, .ilen = SIZE_MAX,					\
	  .out = o, .blen = SIZE_MAX, .olen = sizeof o }

/* input string longer than input length */
#define T_LONG_INPUT_ENC(i, il, o, ol)					\
	{ .desc = "encode (long input)", .func = percent_encode,	\
	  .in = i, .ilen = il, .out = o, .blen = ol, .olen = ol }
#define T_LONG_INPUT_DEC(i, il, o, ol)					\
	{ .desc = "decode (long input)", .func = percent_decode,	\
	  .in = i, .ilen = il, .out = o, .blen = ol, .olen = ol }

/* output string longer than output length */
#define T_LONG_OUTPUT_ENC(i, o, l)					\
	{ .desc = "encode (long output)", .func = percent_encode,	\
	  .in = i, .ilen = SIZE_MAX,					\
	  .out = o, .blen = l, .olen = sizeof o,			\
	  .ret = -1, .err = ENOSPC }
#define T_LONG_OUTPUT_DEC(i, o, l)					\
	{ .desc = "decode (long output)", .func = percent_decode,	\
	  .in = i, .ilen = SIZE_MAX,					\
	  .out = o, .blen = l, .olen = sizeof o,			\
	  .ret = -1, .err = ENOSPC }

static const char unreserved[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-._~";

static const char reserved[] =
    ":/?#[]@"			/* gen-delims */
    "!$&'()*+,;=";		/* sub-delims */
static const char reserved_upper[] =
    "%3A%2F%3F%23%5B%5D%40"
    "%21%24%26%27%28%29%2A%2B%2C%3B%3D";
static const char reserved_lower[] =
    "%3a%2f%3f%23%5b%5d%40"
    "%21%24%26%27%28%29%2a%2b%2c%3b%3d";

static const char mixed[] =
    "Magic = xyzzy";
static const char mixed_upper[] =
    "Magic%20%3D%20xyzzy";
static const char mixed_lower[] =
    "Magic%20%3d%20xyzzy";

static struct t_case t_cases[] = {
	/* empty string */
	T_ENCDEC("", ""),

	/* unreserved characters */
	T_ENCDEC(unreserved, unreserved),

	/* reserved characters */
	T_ENCDEC(reserved, reserved_upper),
	T_DECODE(reserved_lower, reserved),

	/* individual reserved characters */
	T_ENCDEC("!", "%21"),
	T_ENCDEC("#", "%23"),
	T_ENCDEC("$", "%24"),
	T_ENCDEC("%", "%25"),
	T_ENCDEC("&", "%26"),
	T_ENCDEC("'", "%27"),
	T_ENCDEC("(", "%28"),
	T_ENCDEC(")", "%29"),
	T_ENCDEC("*", "%2A"),
	T_ENCDEC("+", "%2B"),
	T_ENCDEC(",", "%2C"),
	T_ENCDEC("/", "%2F"),
	T_ENCDEC(";", "%3B"),
	T_ENCDEC(":", "%3A"),
	T_ENCDEC("=", "%3D"),
	T_ENCDEC("?", "%3F"),
	T_ENCDEC("@", "%40"),
	T_ENCDEC("[", "%5B"),
	T_ENCDEC("]", "%5D"),
	T_DECODE("%2a",	"*"),
	T_DECODE("%2b",	"+"),
	T_DECODE("%2c",	","),
	T_DECODE("%2f",	"/"),
	T_DECODE("%3a",	":"),
	T_DECODE("%3b",	";"),
	T_DECODE("%3d",	"="),
	T_DECODE("%3f",	"?"),
	T_DECODE("%5b",	"["),
	T_DECODE("%5d",	"]"),

	/* too few characters after % */
	T_DECODE_FAIL(EINVAL, "x%", "x"),
	T_DECODE_FAIL(EINVAL, "x%0", "x"),

	/* non-hex characters after % */
	T_DECODE_FAIL(EINVAL, "x%0z", "x"),
	T_DECODE_FAIL(EINVAL, "x%zz", "x"),
	T_DECODE_FAIL(EINVAL, "x%z0", "x"),

	/* input shorter than claimed */
	T_SHORT_INPUT_ENC(reserved, reserved_upper),
	T_SHORT_INPUT_DEC(reserved_lower, reserved),

	/* input longer than claimed */
	T_LONG_INPUT_ENC(mixed, 6, mixed_upper, 9),
	T_LONG_INPUT_DEC(mixed_lower, 8, mixed, 7),

	/* encoding into short buffer */
	T_LONG_OUTPUT_ENC(mixed, mixed_upper, 6),
	T_LONG_OUTPUT_ENC(mixed, mixed_upper, 7),
	T_LONG_OUTPUT_ENC(mixed, mixed_upper, 8),
	T_LONG_OUTPUT_ENC(mixed, mixed_upper, 9),

	/* decoding into short buffer */
	T_LONG_OUTPUT_DEC(unreserved, unreserved, 5),
	T_LONG_OUTPUT_DEC(reserved_lower, reserved, 5),

	/* various no-output scenarios (decoding) */
	{
		.desc	= "decode (no output)",
		.func	= percent_decode,
		.in	= mixed_lower,
		.ilen	= SIZE_MAX,
		.out	= NULL,
		.blen	= SIZE_MAX,
		.olen	= sizeof mixed,
		/* success */
	},
	{
		.desc	= "decode (no output, invalid sequence)",
		.func	= percent_decode,
		.in	= "x%",
		.ilen	= SIZE_MAX,
		.out	= NULL,
		.blen	= SIZE_MAX,
		.olen	= 2,
		.ret	= -1,
		.err	= EINVAL,
	},
	{
		.desc	= "decode (no space)",
		.func	= percent_decode,
		.in	= mixed_lower,
		.ilen	= SIZE_MAX,
		.out	= "",
		.blen	= 0,
		.olen	= sizeof mixed,
		.ret	= -1,
		.err	= ENOSPC,
	},
	{
		.desc	= "decode (no space, invalid sequence)",
		.func	= percent_decode,
		.in	= "x%",
		.ilen	= SIZE_MAX,
		.out	= "",
		.blen	= 0,
		.olen	= 2,
		/* invalid sequence trumps no space */
		.ret	= -1,
		.err	= EINVAL,
	},
	{
		.desc	= "decode (no output, no space)",
		.func	= percent_decode,
		.in	= mixed_lower,
		.ilen	= SIZE_MAX,
		.out	= NULL,
		.blen	= 0,
		.olen	= sizeof mixed,
		/* success */
	},
	{
		.desc	= "decode (no output, no space, invalid sequence)",
		.func	= percent_decode,
		.in	= "x%",
		.ilen	= SIZE_MAX,
		.out	= NULL,
		.blen	= 0,
		.olen	= 2,
		/* invalid sequence trumps no space */
		.ret	= -1,
		.err	= EINVAL,
	},

	/* various no-output scenarios (encoding) */
	{
		.desc	= "encode (no output)",
		.func	= percent_encode,
		.in	= mixed,
		.ilen	= SIZE_MAX,
		.out	= NULL,
		.blen	= SIZE_MAX,
		.olen	= sizeof mixed_upper,
		/* success */
	},
	{
		.desc	= "encode (no space)",
		.func	= percent_encode,
		.in	= mixed,
		.ilen	= SIZE_MAX,
		.out	= "",
		.blen	= 0,
		.olen	= sizeof mixed_upper,
		.ret	= -1,
		.err	= ENOSPC,
	},
	{
		.desc	= "encode (no output, no space)",
		.func	= percent_encode,
		.in	= mixed,
		.ilen	= SIZE_MAX,
		.out	= NULL,
		.blen	= 0,
		.olen	= sizeof mixed_upper,
		/* success */
	},
};

/*
 * Encoding test function
 */
static int
t_rfc3986(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	char buf[256];
	size_t len;
	int ret;

	memset(buf, 0, sizeof buf);
	errno = 0;
	len = t->blen > sizeof buf ? sizeof buf : t->blen;
	ret = t_compare_i(t->ret,
	    t->func(t->in, t->ilen, t->out ? buf : NULL, &len));
	ret &= t_compare_sz(t->olen, len);
	if (t->ret != 0 || errno != 0)
		ret &= t_compare_i(t->err, errno);
	if (t->out)
		ret &= t_compare_strn(t->out, buf, t->blen - 1);
	return (ret);
}

/*
 * Generate the test plan
 */
int
t_prepare(int argc, char *argv[])
{
	struct t_case *t;
	int i, n;

	(void)argc;
	(void)argv;
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i) {
		t = &t_cases[i];
		if (t->desc) {
			t_add_test(t_rfc3986, t, t->desc);
		} else if (t->ret == 0) {
			t_add_test(t_rfc3986, t,
			    "%s \"%s\" -> \"%s\"",
			    t->func == percent_encode ? "encode" : "decode",
			    t->in, t->out);
		} else {
			t_add_test(t_rfc3986, t,
			    "%s \"%s\" (failure)",
			    t->func == percent_encode ? "encode" : "decode",
			    t->in);
		}
	}
	return (0);
}

/*
 * Cleanup
 */
void
t_cleanup(void)
{
}
