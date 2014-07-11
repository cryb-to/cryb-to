/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012-2014 Dag-Erling Smørgrav
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
 * Sponsor: the University of Oslo
 *
 * $Cryb$
 */

#include "cryb/impl.h"

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "t.h"

#if WITH_OPENSSL

#include <openssl/md4.h>

#define MD4_DIGEST_LEN MD4_DIGEST_LENGTH

static void
t_md4_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	MD4_CTX ctx;

	MD4_Init(&ctx);
	MD4_Update(&ctx, msg, msglen);
	MD4_Final(digest, &ctx);
}

#elif WITH_RSAREF

#include <rsaref.h>

#define MD4_DIGEST_LEN 16

static void
t_md4_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	MD4_CTX ctx;

	MD4Init(&ctx);
	MD4Update(&ctx, (unsigned char *)(uintptr_t)msg, msglen);
	MD4Final(digest, &ctx);
}

#else

#include <cryb/md4.h>

#define t_md4_complete(msg, msglen, digest)				\
	md4_complete(msg, msglen, digest)

#endif

/*
 * Test vectors from RFC 1320
 */
static struct t_vector {
	const char *msg;
	const uint8_t digest[MD4_DIGEST_LEN];
} t_md4_vectors[] = {
	{
		"",
		{
			0x31, 0xd6, 0xcf, 0xe0, 0xd1, 0x6a, 0xe9, 0x31,
			0xb7, 0x3c, 0x59, 0xd7, 0xe0, 0xc0, 0x89, 0xc0,
		}
	},
	{
		"a",
		{
			0xbd, 0xe5, 0x2c, 0xb3, 0x1d, 0xe3, 0x3e, 0x46,
			0x24, 0x5e, 0x05, 0xfb, 0xdb, 0xd6, 0xfb, 0x24,
		}
	},
	{
		"abc",
		{
			0xa4, 0x48, 0x01, 0x7a, 0xaf, 0x21, 0xd8, 0x52,
			0x5f, 0xc1, 0x0a, 0xe8, 0x7a, 0xa6, 0x72, 0x9d,
		}
	},
	{
		"message digest",
		{
			0xd9, 0x13, 0x0a, 0x81, 0x64, 0x54, 0x9f, 0xe8,
			0x18, 0x87, 0x48, 0x06, 0xe1, 0xc7, 0x01, 0x4b,
		}
	},
	{
		"abcdefghijklmnopqrstuvwxyz",
		{
			0xd7, 0x9e, 0x1c, 0x30, 0x8a, 0xa5, 0xbb, 0xcd,
			0xee, 0xa8, 0xed, 0x63, 0xdf, 0x41, 0x2d, 0xa9,
		}
	},
	{
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789",
		{
			0x04, 0x3f, 0x85, 0x82, 0xf2, 0x41, 0xdb, 0x35,
			0x1c, 0xe6, 0x27, 0xe1, 0x53, 0xe7, 0xf0, 0xe4,
		}
	},
	{
		"1234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890",
		{
			0xe3, 0x3b, 0x4d, 0xdc, 0x9c, 0x38, 0xf2, 0x19,
			0x9c, 0x3e, 0x7b, 0x16, 0x4f, 0xcc, 0x05, 0x36,
		}
	},
};

/*
 * Unit test: compute the MD4 sum of the specified string and compare it
 * to the expected result.
 */
static int
t_md4_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[MD4_DIGEST_LEN];

	t_md4_complete(vector->msg, strlen(vector->msg), digest);
	if (memcmp(digest, vector->digest, MD4_DIGEST_LEN) != 0) {
		t_verbose("expected ");
		t_verbose_hex(vector->digest, MD4_DIGEST_LEN);
		t_verbose("\n");
		t_verbose("got	    ");
		t_verbose_hex(digest, MD4_DIGEST_LEN);
		t_verbose("\n");
		return (0);
	}
	return (1);
}


#if !defined(WITH_OPENSSL) && !defined(WITH_RSAREF)
/*
 * Various corner cases and error conditions
 */
static int
t_md4_short_updates(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[MD4_DIGEST_LEN];
	md4_ctx ctx;
	int i, len;

	md4_init(&ctx);
	len = strlen(vector->msg);
	for (i = 0; i + 5 < len; i += 5)
		md4_update(&ctx, vector->msg + i, 5);
	md4_update(&ctx, vector->msg + i, len - i);
	md4_final(&ctx, digest);
	return (memcmp(digest, vector->digest, MD4_DIGEST_LEN) == 0);
}
#endif

#ifdef BENCHMARKS
/*
 * Performance test: measure the time spent computing the MD4 sum of a
 * message of the specified length.
 */
#define T_PERF_ITERATIONS 1000
static int
t_md4_perf(char **desc, void *arg)
{
	struct timespec ts, te;
	unsigned long ns;
	uint8_t digest[MD4_DIGEST_LEN];
	char *msg, *comment;
	size_t msglen = *(size_t *)arg;

	if ((msg = calloc(1, msglen)) == NULL)
		err(1, "calloc()");
	clock_gettime(CLOCK_MONOTONIC_PRECISE, &ts);
	for (int i = 0; i < T_PERF_ITERATIONS; ++i)
		t_md4_complete(msg, msglen, digest);
	clock_gettime(CLOCK_MONOTONIC_PRECISE, &te);
	free(msg);
	ns = te.tv_sec * 1000000000LU + te.tv_nsec;
	ns -= ts.tv_sec * 1000000000LU + ts.tv_nsec;
	asprintf(&comment, "%zu bytes: %d iterations in %'lu ns",
	    msglen, T_PERF_ITERATIONS, ns);
	if (comment == NULL)
		err(1, "asprintf()");
	*desc = comment;
	return (1);
}
#endif


/***************************************************************************
 * Boilerplate
 */

int
t_prepare(int argc, char *argv[])
{
	int i, n;

	(void)argc;
	(void)argv;
	n = sizeof t_md4_vectors / sizeof t_md4_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_md4_vector, &t_md4_vectors[i],
		    "RFC 1321 test vector %d", i + 1);
#if !defined(WITH_OPENSSL) && !defined(WITH_RSAREF)
	/*
	 * Run test vector 7 (which is 80 characters long) 5 characters at
	 * a time.  This tests a) appending data to an underfull block and
	 * b) appending more data to an underfull block than it has room
	 * for (since 64 % 5 != 0).  Test vector 7 already exercised the
	 * code path for computing a block directly from source (without
	 * copying it in), and all the test vectors except vector 1
	 * exercised the general case of copying a small amount of data in
	 * without crossing the block boundary.
	 */
	t_add_test(t_md4_short_updates, &t_md4_vectors[6],
	    "multiple short updates");
#endif
#ifdef BENCHMARKS
	static size_t one = 1, thousand = 1000, million = 1000000;
	t_add_test(t_md4_perf, &one,
	    "performance test (1 byte)");
	t_add_test(t_md4_perf, &thousand,
	    "performance test (1,000 bytes)");
	t_add_test(t_md4_perf, &million,
	    "performance test (1,000,000 bytes)");
#endif
	return (0);
}

void
t_cleanup(void)
{
}
