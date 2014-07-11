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

#include <openssl/sha.h>

#define SHA1_DIGEST_LEN SHA_DIGEST_LENGTH

static void
t_sha1_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	SHA_CTX ctx;

	SHA1_Init(&ctx);
	SHA1_Update(&ctx, msg, msglen);
	SHA1_Final(digest, &ctx);
}

#else

#include <cryb/sha1.h>

#define t_sha1_complete(msg, msglen, digest)				\
	sha1_complete(msg, msglen, digest)

#endif

static struct t_vector {
	const char *desc;
	const char *msg;
	const uint8_t digest[SHA1_DIGEST_LEN];
} t_sha1_vectors[] = {
	{
		"zero-length message",
		"",
		{
			0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
			0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90,
			0xaf, 0xd8, 0x07, 0x09,
		}
	},
	{
		"FIPS 180-2 A.1 (one-block message)",
		"abc",
		{
			0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
			0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
			0x9c, 0xd0, 0xd8, 0x9d,
		}
	},
	{
		/*
		 * This message is *just* long enough to necessitate a
		 * second block, which consists entirely of padding.
		 */
		"FIPS 180-2 A.2 (multi-block message)",
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		{
			0x84, 0x98, 0x3e, 0x44, 0x1c, 0x3b, 0xd2, 0x6e,
			0xba, 0xae, 0x4a, 0xa1, 0xf9, 0x51, 0x29, 0xe5,
			0xe5, 0x46, 0x70, 0xf1,
		}
	},
	{
		/*
		 * 1,000,000 x 'a', filled in by t_prepare()
		 */
		"FIPS 180-2 A.3 (long message)",
		NULL,
		{
			0x34, 0xaa, 0x97, 0x3c, 0xd4, 0xc4, 0xda, 0xa4,
			0xf6, 0x1e, 0xeb, 0x2b, 0xdb, 0xad, 0x27, 0x31,
			0x65, 0x34, 0x01, 0x6f,
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
			0x50, 0xab, 0xf5, 0x70, 0x6a, 0x15, 0x09, 0x90,
			0xa0, 0x8b, 0x2c, 0x5e, 0xa4, 0x0f, 0xa0, 0xe5,
			0x85, 0x55, 0x47, 0x32,
		},
	},
};

/*
 * Unit test: compute the SHA1 sum of the specified string and compare it
 * to the expected result.
 */
static int
t_sha1_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA1_DIGEST_LEN];
	char *msg;

	if (vector->msg) {
		t_sha1_complete(vector->msg, strlen(vector->msg), digest);
	} else {
		/* special case for FIPS test vector 3 */
		if ((msg = malloc(1000000)) == NULL)
			err(1, "malloc()");
		memset(msg, 'a', 1000000);
		t_sha1_complete(msg, 1000000, digest);
		free(msg);
	}
	if (memcmp(digest, vector->digest, SHA1_DIGEST_LEN) != 0) {
		t_verbose("expected ");
		t_verbose_hex(vector->digest, SHA1_DIGEST_LEN);
		t_verbose("\n");
		t_verbose("got      ");
		t_verbose_hex(digest, SHA1_DIGEST_LEN);
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
t_sha1_short_updates(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA1_DIGEST_LEN];
	sha1_ctx ctx;
	int i, len;

	sha1_init(&ctx);
	len = strlen(vector->msg);
	for (i = 0; i + 5 < len; i += 5)
		sha1_update(&ctx, vector->msg + i, 5);
	sha1_update(&ctx, vector->msg + i, len - i);
	sha1_final(&ctx, digest);
	return (memcmp(digest, vector->digest, SHA1_DIGEST_LEN) == 0);
}
#endif

/*
 * Performance test: measure the time spent computing the SHA1 sum of a
 * message of the specified length.
 */
#define T_PERF_ITERATIONS 1000
static int
t_sha1_perf(char **desc, void *arg)
{
	struct timespec ts, te;
	unsigned long ns;
	uint8_t digest[SHA1_DIGEST_LEN];
	char *msg, *comment;
	size_t msglen = *(size_t *)arg;

	if ((msg = calloc(1, msglen)) == NULL)
		err(1, "calloc()");
	clock_gettime(CLOCK_MONOTONIC_PRECISE, &ts);
	for (int i = 0; i < T_PERF_ITERATIONS; ++i)
		t_sha1_complete(msg, msglen, digest);
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
	n = sizeof t_sha1_vectors / sizeof t_sha1_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_sha1_vector, &t_sha1_vectors[i],
		    t_sha1_vectors[i].desc);
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
	t_add_test(t_sha1_short_updates, &t_sha1_vectors[4],
	    "multiple short updates");
#endif
	if (getenv("CRYB_PERFTEST")) {
		static size_t one = 1, thousand = 1000, million = 1000000;
		t_add_test(t_sha1_perf, &one,
		    "performance test (1 byte)");
		t_add_test(t_sha1_perf, &thousand,
		    "performance test (1,000 bytes)");
		t_add_test(t_sha1_perf, &million,
		    "performance test (1,000,000 bytes)");
	}
	return (0);
}

void
t_cleanup(void)
{
}
