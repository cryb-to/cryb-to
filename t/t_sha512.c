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

#define SHA512_DIGEST_LEN	SHA512_DIGEST_LENGTH
#define sha512_ctx		SHA512_CTX
#define sha512_init(c)		SHA512_Init(c)
#define sha512_update(c, m, l)	SHA512_Update(c, m, l)
#define sha512_final(c, d)	SHA512_Final(d, c)

static void
t_sha512_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	SHA512_CTX ctx;

	SHA512_Init(&ctx);
	SHA512_Update(&ctx, msg, msglen);
	SHA512_Final(digest, &ctx);
}

#else

#include <cryb/sha512.h>

#define t_sha512_complete(msg, msglen, digest)				\
	sha512_complete(msg, msglen, digest)

#endif

static struct t_vector {
	const char *desc;
	const char *msg;
	const uint8_t digest[SHA512_DIGEST_LEN];
} t_sha512_vectors[] = {
	{
		"zero-length message",
		"",
		{
			0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd,
			0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
			0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
			0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
			0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
			0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
			0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81,
			0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e,
		}
	},
	{
		"FIPS 180-2 C.1 (one-block message)",
		"abc",
		{
			0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba,
			0xcc, 0x41, 0x73, 0x49, 0xae, 0x20, 0x41, 0x31,
			0x12, 0xe6, 0xfa, 0x4e, 0x89, 0xa9, 0x7e, 0xa2,
			0x0a, 0x9e, 0xee, 0xe6, 0x4b, 0x55, 0xd3, 0x9a,
			0x21, 0x92, 0x99, 0x2a, 0x27, 0x4f, 0xc1, 0xa8,
			0x36, 0xba, 0x3c, 0x23, 0xa3, 0xfe, 0xeb, 0xbd,
			0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c, 0xe8, 0x0e,
			0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4, 0x9f,
		}
	},
	{
		/*
		 * This message is *just* long enough to necessitate a
		 * second block, which consists entirely of padding.
		 */
		"FIPS 180-2 C.2 (multi-block message)",
		"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
		"hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
		{
			0x8e, 0x95, 0x9b, 0x75, 0xda, 0xe3, 0x13, 0xda,
			0x8c, 0xf4, 0xf7, 0x28, 0x14, 0xfc, 0x14, 0x3f,
			0x8f, 0x77, 0x79, 0xc6, 0xeb, 0x9f, 0x7f, 0xa1,
			0x72, 0x99, 0xae, 0xad, 0xb6, 0x88, 0x90, 0x18,
			0x50, 0x1d, 0x28, 0x9e, 0x49, 0x00, 0xf7, 0xe4,
			0x33, 0x1b, 0x99, 0xde, 0xc4, 0xb5, 0x43, 0x3a,
			0xc7, 0xd3, 0x29, 0xee, 0xb6, 0xdd, 0x26, 0x54,
			0x5e, 0x96, 0xe5, 0x5b, 0x87, 0x4b, 0xe9, 0x09,
		}
	},
	{
		/*
		 * 1,000,000 x 'a', filled in by t_prepare()
		 */
		"FIPS 180-2 C.3 (long message)",
		NULL,
		{
			0xe7, 0x18, 0x48, 0x3d, 0x0c, 0xe7, 0x69, 0x64,
			0x4e, 0x2e, 0x42, 0xc7, 0xbc, 0x15, 0xb4, 0x63,
			0x8e, 0x1f, 0x98, 0xb1, 0x3b, 0x20, 0x44, 0x28,
			0x56, 0x32, 0xa8, 0x03, 0xaf, 0xa9, 0x73, 0xeb,
			0xde, 0x0f, 0xf2, 0x44, 0x87, 0x7e, 0xa6, 0x0a,
			0x4c, 0xb0, 0x43, 0x2c, 0xe5, 0x77, 0xc3, 0x1b,
			0xeb, 0x00, 0x9c, 0x5c, 0x2c, 0x49, 0xaa, 0x2e,
			0x4e, 0xad, 0xb2, 0x17, 0xad, 0x8c, 0xc0, 0x9b,
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
			0x72, 0xec, 0x1e, 0xf1, 0x12, 0x4a, 0x45, 0xb0,
			0x47, 0xe8, 0xb7, 0xc7, 0x5a, 0x93, 0x21, 0x95,
			0x13, 0x5b, 0xb6, 0x1d, 0xe2, 0x4e, 0xc0, 0xd1,
			0x91, 0x40, 0x42, 0x24, 0x6e, 0x0a, 0xec, 0x3a,
			0x23, 0x54, 0xe0, 0x93, 0xd7, 0x6f, 0x30, 0x48,
			0xb4, 0x56, 0x76, 0x43, 0x46, 0x90, 0x0c, 0xb1,
			0x30, 0xd2, 0xa4, 0xfd, 0x5d, 0xd1, 0x6a, 0xbb,
			0x5e, 0x30, 0xbc, 0xb8, 0x50, 0xde, 0xe8, 0x43,
		},
	},
};

/*
 * Unit test: compute the SHA512 sum of the specified string and compare it
 * to the expected result.
 */
static int
t_sha512_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA512_DIGEST_LEN];
	char msg[1000000];

	if (vector->msg) {
		t_sha512_complete(vector->msg, strlen(vector->msg), digest);
	} else {
		/* special case for FIPS test vector 3 */
		memset(msg, 'a', 1000000);
		t_sha512_complete(msg, 1000000, digest);
	}
	return (t_compare_mem(vector->digest, digest, SHA512_DIGEST_LEN));
}


#if !defined(WITH_OPENSSL) && !defined(WITH_RSAREF)
/*
 * Various corner cases and error conditions
 */
static int
t_sha512_short_updates(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA512_DIGEST_LEN];
	sha512_ctx ctx;
	int i, len;

	sha512_init(&ctx);
	len = strlen(vector->msg);
	for (i = 0; i + 5 < len; i += 5)
		sha512_update(&ctx, vector->msg + i, 5);
	sha512_update(&ctx, vector->msg + i, len - i);
	sha512_final(&ctx, digest);
	return (memcmp(digest, vector->digest, SHA512_DIGEST_LEN) == 0);
}
#endif


/*
 * Performance test: measure the time spent computing the SHA512 sum of a
 * message of the specified length.
 */
#define T_PERF_ITERATIONS 1000
static int
t_sha512_perf(char **desc, void *arg)
{
	struct timespec ts, te;
	unsigned long ns;
	uint8_t digest[SHA512_DIGEST_LEN];
	size_t msglen = *(size_t *)arg;
	char msg[msglen];

	clock_gettime(CLOCK_MONOTONIC, &ts);
	for (int i = 0; i < T_PERF_ITERATIONS; ++i)
		t_sha512_complete(msg, msglen, digest);
	clock_gettime(CLOCK_MONOTONIC, &te);
	ns = te.tv_sec * 1000000000LU + te.tv_nsec;
	ns -= ts.tv_sec * 1000000000LU + ts.tv_nsec;
	asprintf(desc, "%zu bytes: %d iterations in %'lu ns",
	    msglen, T_PERF_ITERATIONS, ns);
	return (1);
}


/*
 * Test the carry operation on the byte counter.
 */
static int
t_sha512_carry(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	sha512_ctx ctx;
	uint8_t digest[SHA512_DIGEST_LEN];
	static uint8_t expect[SHA512_DIGEST_LEN] = {
		0xbd, 0x25, 0x6d, 0xa8, 0xbf, 0xe0, 0x6c, 0xf0,
		0xc1, 0x8c, 0xe9, 0x58, 0xb8, 0xce, 0x43, 0xc7,
		0x9a, 0x3d, 0xec, 0x10, 0x58, 0x55, 0x00, 0x7f,
		0xe7, 0x75, 0x48, 0x66, 0xb2, 0x18, 0xc3, 0x98,
		0x91, 0x11, 0x75, 0x88, 0x53, 0x3e, 0xb3, 0x4b,
		0x83, 0x93, 0xca, 0x18, 0x8a, 0xbe, 0x32, 0x7d,
		0x4a, 0x54, 0x16, 0xbb, 0xdf, 0x9e, 0x9c, 0x3a,
		0xd7, 0x22, 0xc8, 0x0d, 0x71, 0x0f, 0x76, 0xc0,
	};

	sha512_init(&ctx);
#if WITH_OPENSSL
	/* openssl counts bits */
	ctx.Nl = 0xffffffffffffff80LLU << 3LLU;
	ctx.Nh = 0xffffffffffffff80LLU >> 61LLU;
#else
	/* cryb counts bytes and multiplies at the end */
	ctx.total[0] = 0xffffffffffffff80LLU;
#endif
	sha512_update(&ctx, t_seq8, 256);
	sha512_final(&ctx, digest);
	return (t_compare_mem(expect, digest, SHA512_DIGEST_LEN));
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
	n = sizeof t_sha512_vectors / sizeof t_sha512_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_sha512_vector, &t_sha512_vectors[i],
		    t_sha512_vectors[i].desc);
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
	t_add_test(t_sha512_short_updates, &t_sha512_vectors[4],
	    "multiple short updates");
#endif
	if (t_str_is_true(getenv("CRYB_PERFTEST"))) {
		static size_t one = 1, thousand = 1000, million = 1000000;
		t_add_test(t_sha512_perf, &one,
		    "performance test (1 byte)");
		t_add_test(t_sha512_perf, &thousand,
		    "performance test (1,000 bytes)");
		t_add_test(t_sha512_perf, &million,
		    "performance test (1,000,000 bytes)");
	}
	t_add_test(t_sha512_carry, NULL, "byte counter carry");
	return (0);
}

void
t_cleanup(void)
{
}
