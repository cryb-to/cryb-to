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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cryb/test.h>

#if WITH_OPENSSL

#include <openssl/sha.h>

#define SHA256_DIGEST_LEN SHA256_DIGEST_LENGTH

static void
t_sha256_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	SHA256_CTX ctx;

	SHA256_Init(&ctx);
	SHA256_Update(&ctx, msg, msglen);
	SHA256_Final(digest, &ctx);
}

#else

#include <cryb/sha256.h>

#define t_sha256_complete(msg, msglen, digest)				\
	sha256_complete(msg, msglen, digest)

#endif

static struct t_vector {
	const char *desc;
	const char *msg;
	const uint8_t digest[SHA256_DIGEST_LEN];
} t_sha256_vectors[] = {
	{
		"zero-length message",
		"",
		{
			0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
			0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
			0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
			0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,
		}
	},
	{
		"FIPS 180-2 B.1 (one-block message)",
		"abc",
		{
			0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
			0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
			0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
			0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad,
		}
	},
	{
		/*
		 * This message is *just* long enough to necessitate a
		 * second block, which consists entirely of padding.
		 */
		"FIPS 180-2 B.2 (multi-block message)",
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		{
			0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
			0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
			0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
			0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1,
		}
	},
	{
		/*
		 * 1,000,000 x 'a', filled in by t_prepare()
		 */
		"FIPS 180-2 B.3 (long message)",
		NULL,
		{
			0xcd, 0xc7, 0x6e, 0x5c, 0x99, 0x14, 0xfb, 0x92,
			0x81, 0xa1, 0xc7, 0xe2, 0x84, 0xd7, 0x3e, 0x67,
			0xf1, 0x80, 0x9a, 0x48, 0xa4, 0x97, 0x20, 0x0e,
			0x04, 0x6d, 0x39, 0xcc, 0xc7, 0x11, 0x2c, 0xd0,
		},
	},
	{
		/*
		 * One of the MD5 test vectors, included for the "short
		 * update" test.
		 */
		"\"1234567890\"x8",
		"1234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890",
		{
			0xf3, 0x71, 0xbc, 0x4a, 0x31, 0x1f, 0x2b, 0x00,
			0x9e, 0xef, 0x95, 0x2d, 0xd8, 0x3c, 0xa8, 0x0e,
			0x2b, 0x60, 0x02, 0x6c, 0x8e, 0x93, 0x55, 0x92,
			0xd0, 0xf9, 0xc3, 0x08, 0x45, 0x3c, 0x81, 0x3e,
		},
	},
};

/*
 * Unit test: compute the SHA256 sum of the specified string and compare it
 * to the expected result.
 */
static int
t_sha256_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA256_DIGEST_LEN];
	char msg[1000000];

	if (vector->msg) {
		t_sha256_complete(vector->msg, strlen(vector->msg), digest);
	} else {
		/* special case for FIPS test vector 3 */
		memset(msg, 'a', 1000000);
		t_sha256_complete(msg, 1000000, digest);
	}
	return (t_compare_mem(vector->digest, digest, SHA256_DIGEST_LEN));
}


#if !defined(WITH_OPENSSL) && !defined(WITH_RSAREF)
/*
 * Various corner cases and error conditions
 */
static int
t_sha256_short_updates(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA256_DIGEST_LEN];
	sha256_ctx ctx;
	int i, len;

	sha256_init(&ctx);
	len = strlen(vector->msg);
	for (i = 0; i + 5 < len; i += 5)
		sha256_update(&ctx, vector->msg + i, 5);
	sha256_update(&ctx, vector->msg + i, len - i);
	sha256_final(&ctx, digest);
	return (memcmp(digest, vector->digest, SHA256_DIGEST_LEN) == 0);
}
#endif

/*
 * Performance test: measure the time spent computing the SHA256 sum of a
 * message of the specified length.
 */
#define T_PERF_ITERATIONS 1000
static int
t_sha256_perf(char **desc, void *arg)
{
	struct timespec ts, te;
	unsigned long ns;
	uint8_t digest[SHA256_DIGEST_LEN];
	size_t msglen = *(size_t *)arg;
	char msg[msglen];

	clock_gettime(CLOCK_MONOTONIC, &ts);
	for (int i = 0; i < T_PERF_ITERATIONS; ++i)
		t_sha256_complete(msg, msglen, digest);
	clock_gettime(CLOCK_MONOTONIC, &te);
	ns = te.tv_sec * 1000000000LU + te.tv_nsec;
	ns -= ts.tv_sec * 1000000000LU + ts.tv_nsec;
	asprintf(desc, "%zu bytes: %d iterations in %'lu ns",
	    msglen, T_PERF_ITERATIONS, ns);
	return (1);
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
	n = sizeof t_sha256_vectors / sizeof t_sha256_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_sha256_vector, &t_sha256_vectors[i],
		    t_sha256_vectors[i].desc);
#if !defined(WITH_OPENSSL) && !defined(WITH_RSAREF)
	/*
	 * Run test vector 5 (md5 test vector 7, which is 80 characters
	 * long) 5 characters at a time.  This tests a) appending data to
	 * an underfull block and b) appending more data to an underfull
	 * block than it has room for (since 64 % 5 != 0).  Test vector 4
	 * and 5 already exercised the code path for computing a block
	 * directly from source (without copying it in), and all the test
	 * vectors except vector 1 exercised the general case of copying a
	 * small amount of data in without crossing the block boundary.
	 */
	t_add_test(t_sha256_short_updates, &t_sha256_vectors[4],
	    "multiple short updates");
#endif
	if (t_str_is_true(getenv("CRYB_PERFTEST"))) {
		static size_t one = 1, thousand = 1000, million = 1000000;
		t_add_test(t_sha256_perf, &one,
		    "performance test (1 byte)");
		t_add_test(t_sha256_perf, &thousand,
		    "performance test (1,000 bytes)");
		t_add_test(t_sha256_perf, &million,
		    "performance test (1,000,000 bytes)");
	}
	return (0);
}

void
t_cleanup(void)
{
}
