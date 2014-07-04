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
 *
 * Author: Dag-Erling Smørgrav <des@des.no>
 *
 * $Cryb$
 */

#include "cryb/impl.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cryb/rfc3986.h>

#include "t.h"

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
	{ "percent_encode("#i")", percent_encode,			\
	  i, sizeof i - 1, o, sizeof o, sizeof o - 1, 0, 0 }
#define T_DECODE(i, o)							\
	{ "percent_decode("#i")", percent_decode,			\
	  i, sizeof i - 1, o, sizeof o, sizeof o - 1, 0, 0 }

/* roundtrip encoding tests */
#define T_ENCDEC(p, e)							\
	T_ENCODE(p, e), T_DECODE(e, p)

/* decoding failure */
#define T_DECODE_FAIL(e, i)						\
	{ "percent_decode("#i")", percent_decode, i,			\
	  sizeof i - 1, NULL, 0, 0, -1, e }

/* input string shorter than input length */
#define T_SHORT_INPUT_ENC(i, l)						\
	{ "percent_encode (short input)", percent_encode,		\
	  i, l, i, sizeof i, sizeof i - 1, 0, 0 }
#define T_SHORT_INPUT_DEC(i, l)						\
	{ "percent_decode (short input)", percent_decode,		\
	  i, l, i, sizeof i, sizeof i - 1, 0, 0 }

/* output string longer than output length */
#define T_LONG_OUTPUT_ENC(i, l)					\
	{ "percent_encode (long output)", percent_encode,		\
	  i, sizeof i - 1, NULL, l, sizeof i - 1, -1, ENOSPC }
#define T_LONG_OUTPUT_DEC(i, l)						\
	{ "percent_decode (long output)", percent_decode,		\
	  i, sizeof i - 1, NULL, l, sizeof i - 1, -1, ENOSPC }

const char unreserved[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-._~";

const char reserved[] =
    ":/?#[]@"			/* gen-delims */
    "!$&'()*+,;=";		/* sub-delims */
const char reserved_upper[] =
    "%3A%2F%3F%23%5B%5D%40"
    "%21%24%26%27%28%29%2A%2B%2C%3B%3D";
const char reserved_lower[] =
    "%3a%2f%3f%23%5b%5d%40"
    "%21%24%26%27%28%29%2a%2b%2c%3b%3d";

static struct t_case t_cases[] = {
	/* empty string */
	T_ENCDEC("",			""),

	/* unreserved characters */
	T_ENCDEC(unreserved, unreserved),

	/* reserved characters */
	T_ENCDEC(reserved, reserved_upper),
	T_DECODE(reserved_lower, reserved),

	/* individual reserved characters */
	/* XXX */

	/* too few characters after % */
	T_DECODE_FAIL(EINVAL,		"%"),

	/* non-hex characters after % */
	T_DECODE_FAIL(EINVAL,		"%0z"),
	T_DECODE_FAIL(EINVAL,		"%zz"),
	T_DECODE_FAIL(EINVAL,		"%z0"),

	/* input shorter than claimed */
	T_SHORT_INPUT_ENC("-", 2),
	T_SHORT_INPUT_DEC("-", 2),

	/* encoding into short buffer */
	T_LONG_OUTPUT_ENC(unreserved, 1),
	T_LONG_OUTPUT_ENC(reserved, 1),
	T_LONG_OUTPUT_ENC(reserved, 2),
	T_LONG_OUTPUT_ENC(reserved, 3),

	/* decoding into short buffer */
	T_LONG_OUTPUT_DEC(unreserved, 1),
	T_LONG_OUTPUT_DEC(reserved_lower, 1),
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

	len = t->blen ? t->blen : sizeof buf;
	ret = t->func(t->in, t->ilen, buf, &len);
	if (ret != t->ret) {
		t_verbose("expected return code %d, got %d\n",
		    t->ret, ret);
		return (0);
	}
	if (t->out && len != t->olen) {
		t_verbose("expected output length %zu, got %zu\n",
		    t->olen, len);
		return (0);
	}
	if (t->ret != 0 && errno != t->err) {
		t_verbose("expected errno %d, got %d\n",
		    t->err, errno);
		return (0);
	}
	if (t->ret == 0 && t->out && strncmp(buf, t->out, len) != 0) {
		t_verbose("expected '%.*s' got '%.*s'\n",
		    (int)t->olen, t->out, (int)len, buf);
		return (0);
	}
	return (1);
}

/*
 * Generate the test plan
 */
int
t_prepare(int argc, char *argv[])
{
	int i, n;

	(void)argc;
	(void)argv;
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_rfc3986, &t_cases[i], t_cases[i].desc);
	return (0);
}

/*
 * Cleanup
 */
void
t_cleanup(void)
{
}
