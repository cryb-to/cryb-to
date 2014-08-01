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

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "t.h"

#if WITH_OPENSSL

#include <openssl/sha.h>

#define SHA224_DIGEST_LEN SHA224_DIGEST_LENGTH

static void
t_sha224_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	SHA256_CTX ctx;

	SHA224_Init(&ctx);
	SHA224_Update(&ctx, msg, msglen);
	SHA224_Final(digest, &ctx);
}

#else

#include <cryb/sha224.h>

#define t_sha224_complete(msg, msglen, digest)				\
	sha224_complete(msg, msglen, digest)

#endif

/*
 * Test vectors from NIST CSRC
 * http://csrc.nist.gov/groups/ST/toolkit/examples.html
 */
static struct t_vector {
	const char *desc;
	const char *msg;
	const uint8_t digest[SHA224_DIGEST_LEN];
} t_sha224_vectors[] = {
	{
		"zero-length message",
		"",
		{
			0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9,
			0x47, 0x61, 0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4,
			0x15, 0xa2, 0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a,
			0xc5, 0xb3, 0xe4, 0x2f,
		}
	},
	{
		"One-block message",
		"abc",
		{
			0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8, 0x22,
			0x86, 0x42, 0xa4, 0x77, 0xbd, 0xa2, 0x55, 0xb3,
			0x2a, 0xad, 0xbc, 0xe4, 0xbd, 0xa0, 0xb3, 0xf7,
			0xe3, 0x6c, 0x9d, 0xa7,
		}
	},
	{
		/*
		 * This message is *just* long enough to necessitate a
		 * second block, which consists entirely of padding.
		 */
		"Two-block message",
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		{
			0x75, 0x38, 0x8b, 0x16, 0x51, 0x27, 0x76, 0xcc,
			0x5d, 0xba, 0x5d, 0xa1, 0xfd, 0x89, 0x01, 0x50,
			0xb0, 0xc6, 0x45, 0x5c, 0xb4, 0xf5, 0x8b, 0x19,
			0x52, 0x52, 0x25, 0x25,
		}
	},
	{
		/*
		 * 1,000,000 x 'a', filled in by t_prepare()
		 */
		"Long message",
		NULL,
		{
			0x20, 0x79, 0x46, 0x55, 0x98, 0x0c, 0x91, 0xd8,
			0xbb, 0xb4, 0xc1, 0xea, 0x97, 0x61, 0x8a, 0x4b,
			0xf0, 0x3f, 0x42, 0x58, 0x19, 0x48, 0xb2, 0xee,
			0x4e, 0xe7, 0xad, 0x67,
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
			0xb5, 0x0a, 0xec, 0xbe, 0x4e, 0x9b, 0xb0, 0xb5,
			0x7b, 0xc5, 0xf3, 0xae, 0x76, 0x0a, 0x8e, 0x01,
			0xdb, 0x24, 0xf2, 0x03, 0xfb, 0x3c, 0xdc, 0xd1,
			0x31, 0x48, 0x04, 0x6e,
		},
	},
};

/*
 * Unit test: compute the SHA224 sum of the specified string and compare it
 * to the expected result.
 */
static int
t_sha224_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA224_DIGEST_LEN];
	char *msg;

	if (vector->msg) {
		t_sha224_complete(vector->msg, strlen(vector->msg), digest);
	} else {
		/* special case for FIPS test vector 3 */
		if ((msg = malloc(1000000)) == NULL)
			err(1, "malloc()");
		memset(msg, 'a', 1000000);
		t_sha224_complete(msg, 1000000, digest);
		free(msg);
	}
	return (t_compare_mem(vector->digest, digest, SHA224_DIGEST_LEN));
}


#if !defined(WITH_OPENSSL) && !defined(WITH_RSAREF)
/*
 * Various corner cases and error conditions
 */
static int
t_sha224_short_updates(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA224_DIGEST_LEN];
	sha224_ctx ctx;
	int i, len;

	sha224_init(&ctx);
	len = strlen(vector->msg);
	for (i = 0; i + 5 < len; i += 5)
		sha224_update(&ctx, vector->msg + i, 5);
	sha224_update(&ctx, vector->msg + i, len - i);
	sha224_final(&ctx, digest);
	return (memcmp(digest, vector->digest, SHA224_DIGEST_LEN) == 0);
}
#endif

/*
 * Performance test: measure the time spent computing the SHA224 sum of a
 * message of the specified length.
 */
#define T_PERF_ITERATIONS 1000
static int
t_sha224_perf(char **desc, void *arg)
{
	struct timespec ts, te;
	unsigned long ns;
	uint8_t digest[SHA224_DIGEST_LEN];
	char *msg, *comment;
	size_t msglen = *(size_t *)arg;

	if ((msg = calloc(1, msglen)) == NULL)
		err(1, "calloc()");
	clock_gettime(CLOCK_MONOTONIC_PRECISE, &ts);
	for (int i = 0; i < T_PERF_ITERATIONS; ++i)
		t_sha224_complete(msg, msglen, digest);
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


/***************************************************************************
 * Boilerplate
 */

int
t_prepare(int argc, char *argv[])
{
	int i, n;

	(void)argc;
	(void)argv;
	n = sizeof t_sha224_vectors / sizeof t_sha224_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_sha224_vector, &t_sha224_vectors[i],
		    t_sha224_vectors[i].desc);
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
	t_add_test(t_sha224_short_updates, &t_sha224_vectors[4],
	    "multiple short updates");
#endif
	if (getenv("CRYB_PERFTEST")) {
		static size_t one = 1, thousand = 1000, million = 1000000;
		t_add_test(t_sha224_perf, &one,
		    "performance test (1 byte)");
		t_add_test(t_sha224_perf, &thousand,
		    "performance test (1,000 bytes)");
		t_add_test(t_sha224_perf, &million,
		    "performance test (1,000,000 bytes)");
	}
	return (0);
}

void
t_cleanup(void)
{
}
