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
 */

#include "cryb/impl.h"

#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cryb/test.h>

#if WITH_RSAREF

#include <rsaref.h>

#define MD2_DIGEST_LEN 16

static void
t_md2_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	MD2_CTX ctx;

	MD2Init(&ctx);
	MD2Update(&ctx, (unsigned char *)(uintptr_t)msg, msglen);
	MD2Final(digest, &ctx);
}

#else

#include <cryb/md2.h>

#define t_md2_complete(msg, msglen, digest)				\
	md2_complete(msg, msglen, digest)

#endif

/*
 * Test vectors from RFC 1319
 */
static struct t_vector {
	const char *msg;
	const uint8_t digest[MD2_DIGEST_LEN];
} t_md2_vectors[] = {
	{
		"",
		{
			0x83, 0x50, 0xe5, 0xa3, 0xe2, 0x4c, 0x15, 0x3d,
			0xf2, 0x27, 0x5c, 0x9f, 0x80, 0x69, 0x27, 0x73,
		}
	},
	{
		"a",
		{
			0x32, 0xec, 0x01, 0xec, 0x4a, 0x6d, 0xac, 0x72,
			0xc0, 0xab, 0x96, 0xfb, 0x34, 0xc0, 0xb5, 0xd1,
		}
	},
	{
		"abc",
		{
			0xda, 0x85, 0x3b, 0x0d, 0x3f, 0x88, 0xd9, 0x9b,
			0x30, 0x28, 0x3a, 0x69, 0xe6, 0xde, 0xd6, 0xbb,
		}
	},
	{
		"message digest",
		{
			0xab, 0x4f, 0x49, 0x6b, 0xfb, 0x2a, 0x53, 0x0b,
			0x21, 0x9f, 0xf3, 0x30, 0x31, 0xfe, 0x06, 0xb0,
		}
	},
	{
		"abcdefghijklmnopqrstuvwxyz",
		{
			0x4e, 0x8d, 0xdf, 0xf3, 0x65, 0x02, 0x92, 0xab,
			0x5a, 0x41, 0x08, 0xc3, 0xaa, 0x47, 0x94, 0x0b,
		}
	},
	{
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789",
		{
			0xda, 0x33, 0xde, 0xf2, 0xa4, 0x2d, 0xf1, 0x39,
			0x75, 0x35, 0x28, 0x46, 0xc3, 0x03, 0x38, 0xcd,
		}
	},
	{
		"1234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890",
		{
			0xd5, 0x97, 0x6f, 0x79, 0xd8, 0x3d, 0x3a, 0x0d,
			0xc9, 0x80, 0x6c, 0x3c, 0x66, 0xf3, 0xef, 0xd8,
		}
	},
};

/*
 * Unit test: compute the MD2 sum of the specified string and compare it
 * to the expected result.
 */
static int
t_md2_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[MD2_DIGEST_LEN];

	t_md2_complete(vector->msg, strlen(vector->msg), digest);
	return (t_compare_mem(vector->digest, digest, MD2_DIGEST_LEN));
}


#if !defined(WITH_RSAREF)
/*
 * Various corner cases and error conditions
 */
static int
t_md2_short_updates(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[MD2_DIGEST_LEN];
	md2_ctx ctx;
	int i, len;

	md2_init(&ctx);
	len = strlen(vector->msg);
	for (i = 0; i + 5 < len; i += 5)
		md2_update(&ctx, vector->msg + i, 5);
	md2_update(&ctx, vector->msg + i, len - i);
	md2_final(&ctx, digest);
	return (memcmp(digest, vector->digest, MD2_DIGEST_LEN) == 0);
}
#endif


/*
 * Performance test: measure the time spent computing the MD2 sum of a
 * message of the specified length.
 */
#define T_PERF_ITERATIONS 1000
static int
t_md2_perf(char **desc, void *arg)
{
	struct timespec ts, te;
	unsigned long ns;
	uint8_t digest[MD2_DIGEST_LEN];
	size_t msglen = *(size_t *)arg;
	char msg[msglen];

	clock_gettime(CLOCK_MONOTONIC, &ts);
	for (int i = 0; i < T_PERF_ITERATIONS; ++i)
		t_md2_complete(msg, msglen, digest);
	clock_gettime(CLOCK_MONOTONIC, &te);
	ns = te.tv_sec * 1000000000LU + te.tv_nsec;
	ns -= ts.tv_sec * 1000000000LU + ts.tv_nsec;
	(void)asprintf(desc, "%zu bytes: %d iterations in %'lu ns",
	    msglen, T_PERF_ITERATIONS, ns);
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
	n = sizeof t_md2_vectors / sizeof t_md2_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_md2_vector, &t_md2_vectors[i],
		    "RFC 1321 test vector %d", i + 1);
#if !defined(WITH_RSAREF)
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
	t_add_test(t_md2_short_updates, &t_md2_vectors[6],
	    "multiple short updates");
#endif
	if (t_str_is_true(getenv("CRYB_PERFTEST"))) {
		static size_t one = 1, thousand = 1000, million = 1000000;
		t_add_test(t_md2_perf, &one,
		    "performance test (1 byte)");
		t_add_test(t_md2_perf, &thousand,
		    "performance test (1,000 bytes)");
		t_add_test(t_md2_perf, &million,
		    "performance test (1,000,000 bytes)");
	}
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
