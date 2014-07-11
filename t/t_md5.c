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

#include <openssl/md5.h>

#define MD5_DIGEST_LEN MD5_DIGEST_LENGTH

static void
t_md5_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	MD5_CTX ctx;

	MD5_Init(&ctx);
	MD5_Update(&ctx, msg, msglen);
	MD5_Final(digest, &ctx);
}

#elif WITH_RSAREF

#include <rsaref.h>

#define MD5_DIGEST_LEN 16

static void
t_md5_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	MD5_CTX ctx;

	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)(uintptr_t)msg, msglen);
	MD5Final(digest, &ctx);
}

#else

#include <cryb/md5.h>

#define t_md5_complete(msg, msglen, digest)				\
	md5_complete(msg, msglen, digest)

#endif

/*
 * Test vectors from RFC 1321
 */
static struct t_vector {
	const char *msg;
	const uint8_t digest[MD5_DIGEST_LEN];
} t_md5_vectors[] = {
	{
		"",
		{
			0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
			0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e,
		}
	},
	{
		"a",
		{
			0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
			0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61,
		}
	},
	{
		"abc",
		{
			0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
			0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72,
		}
	},
	{
		"message digest",
		{
			0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d,
			0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0,
		}
	},
	{
		"abcdefghijklmnopqrstuvwxyz",
		{
			0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00,
			0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b,
		}
	},
	{
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789",
		{
			0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5,
			0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f,
		}
	},
	{
		"1234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890",
		{
			0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55,
			0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a,
		}
	},
};

/*
 * Unit test: compute the MD5 sum of the specified string and compare it
 * to the expected result.
 */
static int
t_md5_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[MD5_DIGEST_LEN];

	t_md5_complete(vector->msg, strlen(vector->msg), digest);
	if (memcmp(digest, vector->digest, MD5_DIGEST_LEN) != 0) {
		t_verbose("expected ");
		t_verbose_hex(vector->digest, MD5_DIGEST_LEN);
		t_verbose("\n");
		t_verbose("got      ");
		t_verbose_hex(digest, MD5_DIGEST_LEN);
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
t_md5_short_updates(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[MD5_DIGEST_LEN];
	md5_ctx ctx;
	int i, len;

        md5_init(&ctx);
	len = strlen(vector->msg);
	for (i = 0; i + 5 < len; i += 5)
		md5_update(&ctx, vector->msg + i, 5);
	md5_update(&ctx, vector->msg + i, len - i);
        md5_final(&ctx, digest);
	return (memcmp(digest, vector->digest, MD5_DIGEST_LEN) == 0);
}
#endif


#ifdef BENCHMARKS
/*
 * Performance test: measure the time spent computing the MD5 sum of a
 * message of the specified length.
 */
#define T_PERF_ITERATIONS 1000
static int
t_md5_perf(char **desc, void *arg)
{
	struct timespec ts, te;
	unsigned long ns;
	uint8_t digest[MD5_DIGEST_LEN];
	char *msg, *comment;
	size_t msglen = *(size_t *)arg;

	if ((msg = calloc(1, msglen)) == NULL)
		err(1, "calloc()");
	clock_gettime(CLOCK_MONOTONIC_PRECISE, &ts);
	for (int i = 0; i < T_PERF_ITERATIONS; ++i)
		t_md5_complete(msg, msglen, digest);
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
	n = sizeof t_md5_vectors / sizeof t_md5_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_md5_vector, &t_md5_vectors[i],
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
	t_add_test(t_md5_short_updates, &t_md5_vectors[6],
	    "multiple short updates");
#endif
#ifdef BENCHMARKS
	static size_t one = 1, thousand = 1000, million = 1000000;
	t_add_test(t_md5_perf, &one,
	    "performance test (1 byte)");
	t_add_test(t_md5_perf, &thousand,
	    "performance test (1,000 bytes)");
	t_add_test(t_md5_perf, &million,
	    "performance test (1,000,000 bytes)");
#endif
	return (0);
}

void
t_cleanup(void)
{
}
