/*-
 * Copyright (c) 2013-2014 The University of Oslo
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

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <cryb/rfc4648.h>

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
#define T_ENCODE_N(N, i, o)						\
	{ "base"#N"_encode("#i")", b##N##enc, i, sizeof i - 1,		\
	  o, sizeof o, sizeof o, 0, 0 }
#define T_DECODE_N(N, i, o)						\
	{ "base"#N"_decode("#i")", b##N##dec, i, sizeof i - 1,		\
	  o, sizeof o - 1, sizeof o - 1, 0, 0 }
#define T_ENCODE(p, b32, b64)						\
	T_ENCODE_N(32, p, b32), T_ENCODE_N(64, p, b64)
#define T_DECODE(p, b32, b64)						\
	T_DECODE_N(32, b32, p), T_DECODE_N(64, b64, p)

/* roundtrip encoding tests */
#define T_ENCDEC_N(N, p, e)						\
	T_ENCODE_N(N, p, e), T_DECODE_N(N, e, p)
#define T_ENCDEC(p, b32, b64)						\
	T_ENCDEC_N(32, p, b32), T_ENCDEC_N(64, p, b64)

/* decoding failure */
#define T_DECODE_FAIL_N(N, e, i)					\
	{ "base"#N"_decode("#i")", b##N##dec, i, sizeof i - 1,		\
	  NULL, 0, 0, -1, e }
#define T_DECODE_FAIL(e, b32, b64)					\
	T_DECODE_FAIL_N(32, e, b32), T_DECODE_FAIL_N(64, e, b64)

/* input string shorter than input length */
#define T_SHORT_INPUT_DEC(N, i)						\
	{ "base"#N"_decode (short input)", b##N##dec, i, sizeof i + 2,	\
	  NULL, 0, base##N##_declen(sizeof i - 1), 0, 0 }
#define T_SHORT_INPUT()							\
	T_SHORT_INPUT_DEC(32, "AAAAAAAA"),				\
	T_SHORT_INPUT_DEC(64, "AAAA")

/* output string longer than output length */
#define T_LONG_OUTPUT_ENC(N, i)						\
	{ "base"#N"_enc (long output)", b##N##enc, i, sizeof i - 1,	\
	  NULL, 1, base##N##_enclen(sizeof i - 1) + 1, -1, ENOSPC }
#define T_LONG_OUTPUT_DEC(N, i)						\
	{ "base"#N"_decode (long output)", b##N##dec, "AAAAAAAA", 8,	\
	  NULL, 1, base##N##_declen(sizeof i - 1), -1, ENOSPC }
#define T_LONG_OUTPUT()							\
	T_LONG_OUTPUT_ENC(32, "foo"),					\
	T_LONG_OUTPUT_DEC(32, "AAAAAAAA"),				\
	T_LONG_OUTPUT_ENC(64, "foo"),					\
	T_LONG_OUTPUT_DEC(64, "AAAA")

static const char b64alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
static const char b64complete[] = {
	0x00, 0x10, 0x83, 0x10, 0x51, 0x87,
	0x20, 0x92, 0x8b, 0x30, 0xd3, 0x8f,
	0x41, 0x14, 0x93, 0x51, 0x55, 0x97,
	0x61, 0x96, 0x9b, 0x71, 0xd7, 0x9f,
	0x82, 0x18, 0xa3, 0x92, 0x59, 0xa7,
	0xa2, 0x9a, 0xab, 0xb2, 0xdb, 0xaf,
	0xc3, 0x1c, 0xb3, 0xd3, 0x5d, 0xb7,
	0xe3, 0x9e, 0xbb, 0xf3, 0xdf, 0xbf,
	0x00
};

static const char b32alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static const char b32complete[] = {
	0x00, 0x44, 0x32, 0x14, 0xc7,
	0x42, 0x54, 0xb6, 0x35, 0xcf,
	0x84, 0x65, 0x3a, 0x56, 0xd7,
	0xc6, 0x75, 0xbe, 0x77, 0xdf,
	0x00
};

static int
b32enc(const char *in, size_t ilen, char *out, size_t *olen)
{

	return (base32_encode((const uint8_t *)in, ilen, out, olen));
}

static int
b32dec(const char *in, size_t ilen, char *out, size_t *olen)
{

	return (base32_decode(in, ilen, (uint8_t *)out, olen));
}

static int
b64enc(const char *in, size_t ilen, char *out, size_t *olen)
{

	return (base64_encode((const uint8_t *)in, ilen, out, olen));
}

static int
b64dec(const char *in, size_t ilen, char *out, size_t *olen)
{

	return (base64_decode(in, ilen, (uint8_t *)out, olen));
}

static struct t_case t_cases[] = {
	/* complete alphabet */
	T_ENCDEC_N(32, b32complete, b32alphabet),
	T_ENCDEC_N(64, b64complete, b64alphabet),

	/* test vectors from RFC 4648 */
	/*	 plain		base32			base64 */
	T_ENCDEC("",		"",			""),
	T_ENCDEC("f",		"MY======",		"Zg=="),
	T_ENCDEC("fo",		"MZXQ====",		"Zm8="),
	T_ENCDEC("foo",		"MZXW6===",		"Zm9v"),
	T_ENCDEC("foob",	"MZXW6YQ=",		"Zm9vYg=="),
	T_ENCDEC("fooba",	"MZXW6YTB",		"Zm9vYmE="),
	T_ENCDEC("foobar",	"MZXW6YTBOI======",	"Zm9vYmFy"),

	/* zeroes */
	T_ENCDEC("\0\0\0",	"AAAAA===",		"AAAA"),

	/* sloppy padding */
	T_DECODE_N(32,		"MY=====",		"f"),
	T_DECODE_N(32,		"MY====",		"f"),
	T_DECODE_N(32,		"MY===",		"f"),
	T_DECODE_N(32,		"MY==",			"f"),
	T_DECODE_N(32,		"MY=",			"f"),
	T_DECODE_N(32,		"MY",			"f"),
	T_DECODE_N(64,		"Zg=",			"f"),
	T_DECODE_N(64,		"Zg",			"f"),

	/* whitespace */
	/*	 plain		base32			base64 */
	T_DECODE("tst",		"ORZX I===",		"dH N0"),
	T_DECODE("tst",		"ORZX\tI===",		"dH\tN0"),
	T_DECODE("tst",		"ORZX\rI===",		"dH\rN0"),
	T_DECODE("tst",		"ORZX\nI===",		"dH\nN0"),

	/* invalid character in data */
	T_DECODE_FAIL(EINVAL,	"AA!AAAAAA",		"AA!A"),

	/* invalid character in padding */
	T_DECODE_FAIL(EINVAL,	"AAAAA==!",		"AA=!"),

	/* padding with no data */
	T_DECODE_FAIL(EINVAL,	"AAAAAAAA=",		"AAAA="),

	/* data after padding */
	T_DECODE_FAIL(EINVAL,	"AA=A",			"AA=A"),

	/* padding in the wrong place, or non-zero bits in padding */
	T_DECODE_FAIL_N(32, EINVAL, "A======="),
	T_DECODE_N     (32,         "AA======", "\x00"),
	T_DECODE_FAIL_N(32, EINVAL, "AB======"),
	T_DECODE_FAIL_N(32, EINVAL, "AC======"),
	T_DECODE_FAIL_N(32, EINVAL, "AD======"),
	T_DECODE_N     (32,         "AE======", "\x01"),
	T_DECODE_FAIL_N(32, EINVAL, "AAA====="),
	T_DECODE_N     (32,         "AAAA====", "\x00\x00"),
	T_DECODE_FAIL_N(32, EINVAL, "AAAB===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAC===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAD===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAE===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAF===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAG===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAH===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAI===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAJ===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAK===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAL===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAM===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAN===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAO===="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAP===="),
	T_DECODE_N     (32,         "AAAQ====", "\x00\x01"),
	T_DECODE_N     (32,         "AAAAA===", "\x00\x00\x00"),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAB==="),
	T_DECODE_N     (32,         "AAAAC===", "\x00\x00\x01"),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAAA=="),
	T_DECODE_N     (32,         "AAAAAAA=", "\x00\x00\x00\x00"),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAAAB="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAAAC="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAAAD="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAAAE="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAAAF="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAAAG="),
	T_DECODE_FAIL_N(32, EINVAL, "AAAAAAH="),
	T_DECODE_N     (32,         "AAAAAAI=", "\x00\x00\x00\x01"),
	T_DECODE_N     (32,         "AAAAAAAA", "\x00\x00\x00\x00\x00"),

	T_DECODE_FAIL_N(64, EINVAL, "A==="),
	T_DECODE_N     (64,         "AA==", "\x00"),
	T_DECODE_FAIL_N(64, EINVAL, "AB=="),
	T_DECODE_FAIL_N(64, EINVAL, "AC=="),
	T_DECODE_FAIL_N(64, EINVAL, "AD=="),
	T_DECODE_FAIL_N(64, EINVAL, "AE=="),
	T_DECODE_FAIL_N(64, EINVAL, "AF=="),
	T_DECODE_FAIL_N(64, EINVAL, "AG=="),
	T_DECODE_FAIL_N(64, EINVAL, "AH=="),
	T_DECODE_FAIL_N(64, EINVAL, "AI=="),
	T_DECODE_FAIL_N(64, EINVAL, "AJ=="),
	T_DECODE_FAIL_N(64, EINVAL, "AK=="),
	T_DECODE_FAIL_N(64, EINVAL, "AL=="),
	T_DECODE_FAIL_N(64, EINVAL, "AM=="),
	T_DECODE_FAIL_N(64, EINVAL, "AN=="),
	T_DECODE_FAIL_N(64, EINVAL, "AO=="),
	T_DECODE_FAIL_N(64, EINVAL, "AP=="),
	T_DECODE_N     (64,         "AQ==", "\x01"),
	T_DECODE_N     (64,         "AAA=", "\x00\x00"),
	T_DECODE_FAIL_N(64, EINVAL, "AAB="),
	T_DECODE_FAIL_N(64, EINVAL, "AAC="),
	T_DECODE_FAIL_N(64, EINVAL, "AAD="),
	T_DECODE_N     (64,         "AAE=", "\x00\x01"),

	/* various error conditions */
	T_SHORT_INPUT(),
	T_LONG_OUTPUT(),
};

/*
 * Encoding test function
 */
static int
t_rfc4648(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	char buf[256];
	size_t len;
	int ret;

	len = t->blen ? t->blen : sizeof buf;
	ret = t->func(t->in, t->ilen, buf, &len);
	if (ret != t->ret) {
		t_printv("expected return code %d, got %d\n",
		    t->ret, ret);
		return (0);
	}
	if (t->out && len != t->olen) {
		t_printv("expected output length %zu, got %zu\n",
		    t->olen, len);
		return (0);
	}
	if (t->ret != 0 && errno != t->err) {
		t_printv("expected errno %d, got %d\n",
		    t->err, errno);
		return (0);
	}
	if (t->ret == 0 && t->out && strncmp(buf, t->out, len) != 0) {
		t_printv("expected '%.*s' got '%.*s'\n",
		    (int)t->olen, t->out, (int)len, buf);
		return (0);
	}
	return (1);
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
		t_add_test(t_rfc4648, &t_cases[i], "%s", t_cases[i].desc);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
