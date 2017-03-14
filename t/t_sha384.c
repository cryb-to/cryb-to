/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012-2016 Dag-Erling Smørgrav
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

#if WITH_OPENSSL

#include <openssl/sha.h>

#define SHA384_DIGEST_LEN	SHA384_DIGEST_LENGTH
#define sha384_ctx		SHA512_CTX	/* yes, 512 is correct */
#define sha384_init(c)		SHA384_Init(c)
#define sha384_update(c, m, l)	SHA384_Update(c, m, l)
#define sha384_final(c, d)	SHA384_Final(d, c)

static void
t_sha384_complete(const void *msg, size_t msglen, uint8_t *digest)
{
	SHA512_CTX ctx;		/* yes, 512 is correct */

	SHA384_Init(&ctx);
	SHA384_Update(&ctx, msg, msglen);
	SHA384_Final(digest, &ctx);
}

#else

#include <cryb/sha384.h>

#define t_sha384_complete(msg, msglen, digest)				\
	sha384_complete(msg, msglen, digest)

#endif

static struct t_vector {
	const char *desc;
	const char *msg;
	const uint8_t digest[SHA384_DIGEST_LEN];
} t_sha384_vectors[] = {
	{
		"zero-length message",
		"",
		{
			0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38,
			0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
			0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
			0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
			0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
			0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b,
		}
	},
	{
		"FIPS 180-2 D.1 (one-block message)",
		"abc",
		{
			0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b,
			0xb5, 0xa0, 0x3d, 0x69, 0x9a, 0xc6, 0x50, 0x07,
			0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63,
			0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed,
			0x80, 0x86, 0x07, 0x2b, 0xa1, 0xe7, 0xcc, 0x23,
			0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7,
		}
	},
	{
		/*
		 * This message is *just* long enough to necessitate a
		 * second block, which consists entirely of padding.
		 */
		"FIPS 180-2 D.2 (multi-block message)",
		"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
		"hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
		{
			0x09, 0x33, 0x0c, 0x33, 0xf7, 0x11, 0x47, 0xe8,
			0x3d, 0x19, 0x2f, 0xc7, 0x82, 0xcd, 0x1b, 0x47,
			0x53, 0x11, 0x1b, 0x17, 0x3b, 0x3b, 0x05, 0xd2,
			0x2f, 0xa0, 0x80, 0x86, 0xe3, 0xb0, 0xf7, 0x12,
			0xfc, 0xc7, 0xc7, 0x1a, 0x55, 0x7e, 0x2d, 0xb9,
			0x66, 0xc3, 0xe9, 0xfa, 0x91, 0x74, 0x60, 0x39,
		}
	},
	{
		/*
		 * 1,000,000 x 'a', filled in by t_prepare()
		 */
		"FIPS 180-2 D.3 (long message)",
		NULL,
		{
			0x9d, 0x0e, 0x18, 0x09, 0x71, 0x64, 0x74, 0xcb,
			0x08, 0x6e, 0x83, 0x4e, 0x31, 0x0a, 0x4a, 0x1c,
			0xed, 0x14, 0x9e, 0x9c, 0x00, 0xf2, 0x48, 0x52,
			0x79, 0x72, 0xce, 0xc5, 0x70, 0x4c, 0x2a, 0x5b,
			0x07, 0xb8, 0xb3, 0xdc, 0x38, 0xec, 0xc4, 0xeb,
			0xae, 0x97, 0xdd, 0xd8, 0x7f, 0x3d, 0x89, 0x85,
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
			0xb1, 0x29, 0x32, 0xb0, 0x62, 0x7d, 0x1c, 0x06,
			0x09, 0x42, 0xf5, 0x44, 0x77, 0x64, 0x15, 0x56,
			0x55, 0xbd, 0x4d, 0xa0, 0xc9, 0xaf, 0xa6, 0xdd,
			0x9b, 0x9e, 0xf5, 0x31, 0x29, 0xaf, 0x1b, 0x8f,
			0xb0, 0x19, 0x59, 0x96, 0xd2, 0xde, 0x9c, 0xa0,
			0xdf, 0x9d, 0x82, 0x1f, 0xfe, 0xe6, 0x70, 0x26,
		},
	},
};

/*
 * Unit test: compute the SHA384 sum of the specified string and compare it
 * to the expected result.
 */
static int
t_sha384_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA384_DIGEST_LEN];
	char msg[1000000];

	if (vector->msg) {
		t_sha384_complete(vector->msg, strlen(vector->msg), digest);
	} else {
		/* special case for FIPS test vector 3 */
		memset(msg, 'a', 1000000);
		t_sha384_complete(msg, 1000000, digest);
	}
	return (t_compare_mem(vector->digest, digest, SHA384_DIGEST_LEN));
}


#if !defined(WITH_OPENSSL) && !defined(WITH_RSAREF)
/*
 * Various corner cases and error conditions
 */
static int
t_sha384_short_updates(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t digest[SHA384_DIGEST_LEN];
	sha384_ctx ctx;
	int i, len;

	sha384_init(&ctx);
	len = strlen(vector->msg);
	for (i = 0; i + 5 < len; i += 5)
		sha384_update(&ctx, vector->msg + i, 5);
	sha384_update(&ctx, vector->msg + i, len - i);
	sha384_final(&ctx, digest);
	return (t_compare_mem(digest, vector->digest, SHA384_DIGEST_LEN));
}
#endif


/*
 * Performance test: measure the time spent computing the SHA384 sum of a
 * message of the specified length.
 */
#define T_PERF_ITERATIONS 1000
static int
t_sha384_perf(char **desc, void *arg)
{
	struct timespec ts, te;
	unsigned long ns;
	uint8_t digest[SHA384_DIGEST_LEN];
	size_t msglen = *(size_t *)arg;
	char msg[msglen];

	clock_gettime(CLOCK_MONOTONIC, &ts);
	for (int i = 0; i < T_PERF_ITERATIONS; ++i)
		t_sha384_complete(msg, msglen, digest);
	clock_gettime(CLOCK_MONOTONIC, &te);
	ns = te.tv_sec * 1000000000LU + te.tv_nsec;
	ns -= ts.tv_sec * 1000000000LU + ts.tv_nsec;
	(void)asprintf(desc, "%zu bytes: %d iterations in %'lu ns",
	    msglen, T_PERF_ITERATIONS, ns);
	return (1);
}


/*
 * Test the carry operation on the byte counter.
 */
static int
t_sha384_carry(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	sha384_ctx ctx;
	uint8_t digest[SHA384_DIGEST_LEN];
	static uint8_t expect[SHA384_DIGEST_LEN] = {
		0x04, 0xa8, 0xab, 0x2a, 0x7d, 0xe6, 0x68, 0x22,
		0xcd, 0x45, 0xfd, 0xc5, 0x41, 0x62, 0x32, 0xca,
		0x6c, 0x59, 0x92, 0x41, 0x77, 0x99, 0xca, 0xa7,
		0xe2, 0xf0, 0x28, 0x77, 0x2b, 0x33, 0xbe, 0xa0,
		0xbe, 0xee, 0x4d, 0xd1, 0x9e, 0x18, 0xc4, 0x5f,
		0x47, 0x91, 0xb3, 0xd1, 0x9c, 0x3a, 0x81, 0xfb,
	};

	sha384_init(&ctx);
#if WITH_OPENSSL
	/* openssl counts bits */
	ctx.Nl = 0xffffffffffffff80LLU << 3LLU;
	ctx.Nh = 0xffffffffffffff80LLU >> 61LLU;
#else
	/* cryb counts bytes and multiplies at the end */
	ctx.total[0] = 0xffffffffffffff80LLU;
#endif
	sha384_update(&ctx, t_seq8, 256);
	sha384_final(&ctx, digest);
	return (t_compare_mem(expect, digest, SHA384_DIGEST_LEN));
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
	n = sizeof t_sha384_vectors / sizeof t_sha384_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_sha384_vector, &t_sha384_vectors[i],
		    "%s", t_sha384_vectors[i].desc);
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
	t_add_test(t_sha384_short_updates, &t_sha384_vectors[4],
	    "multiple short updates");
#endif
	if (t_str_is_true(getenv("CRYB_PERFTEST"))) {
		static size_t one = 1, thousand = 1000, million = 1000000;
		t_add_test(t_sha384_perf, &one,
		    "performance test (1 byte)");
		t_add_test(t_sha384_perf, &thousand,
		    "performance test (1,000 bytes)");
		t_add_test(t_sha384_perf, &million,
		    "performance test (1,000,000 bytes)");
	}
	t_add_test(t_sha384_carry, NULL, "byte counter carry");
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
