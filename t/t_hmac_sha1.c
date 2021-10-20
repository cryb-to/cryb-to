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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cryb/test.h>

#if WITH_OPENSSL

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#define HMAC_SHA1_MAC_LEN SHA_DIGEST_LENGTH

static void
t_hmac_sha1_complete(const void *key, size_t keylen,
    const void *msg, size_t msglen, uint8_t *mac)
{
	HMAC_CTX *ctx;

	ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, keylen, EVP_sha1(), NULL);
	HMAC_Update(ctx, msg, msglen);
	HMAC_Final(ctx, mac, NULL);
	HMAC_CTX_free(ctx);
}

#else

#include <cryb/sha1.h>
#include <cryb/hmac.h>

#define t_hmac_sha1_complete(key, keylen, msg, msglen, mac)			\
	hmac_sha1_complete(key, keylen, msg, msglen, mac)

#endif

/*
 * Test vectors from NIST CSRC
 * http://csrc.nist.gov/groups/ST/toolkit/examples.html
 */
static struct t_vector {
	const char *desc;
	const uint8_t *key;
	size_t keylen;
	const char *msg;
	const uint8_t mac[HMAC_SHA1_MAC_LEN];
} t_hmac_sha1_vectors[] = {
	{
		"zero-length key, zero-length message",
		t_zero,
		0,
		"",
		{
			0xfb, 0xdb, 0x1d, 0x1b, 0x18, 0xaa, 0x6c, 0x08,
			0x32, 0x4b, 0x7d, 0x64, 0xb7, 0x1f, 0xb7, 0x63,
			0x70, 0x69, 0x0e, 0x1d,
		},
	},
	{
		"NIST CSRC example 1 (64-byte key)",
		t_seq8,
		64,
		"Sample message for keylen=blocklen",
		{
			0x5f, 0xd5, 0x96, 0xee, 0x78, 0xd5, 0x55, 0x3c,
			0x8f, 0xf4, 0xe7, 0x2d, 0x26, 0x6d, 0xfd, 0x19,
			0x23, 0x66, 0xda, 0x29,
		},
	},
	{
		"NIST CSRC example 2 (20-byte key)",
		t_seq8,
		20,
		"Sample message for keylen<blocklen",
		{
			0x4c, 0x99, 0xff, 0x0c, 0xb1, 0xb3, 0x1b, 0xd3,
			0x3f, 0x84, 0x31, 0xdb, 0xaf, 0x4d, 0x17, 0xfc,
			0xd3, 0x56, 0xa8, 0x07,
		},
	},
	{
		"NIST CSRC example 3 (100-byte key)",
		t_seq8,
		100,
		/* yes, the text should say keylen>blocklen */
		"Sample message for keylen=blocklen",
		{
			0x2d, 0x51, 0xb2, 0xf7, 0x75, 0x0e, 0x41, 0x05,
			0x84, 0x66, 0x2e, 0x38, 0xf1, 0x33, 0x43, 0x5f,
			0x4c, 0x4f, 0xd4, 0x2a,
		},
	},
	{
		"NIST CSRC example 4 (49-byte key)",
		t_seq8,
		49,
		"Sample message for keylen<blocklen, with truncated tag",
		{
			0xfe, 0x35, 0x29, 0x56, 0x5c, 0xd8, 0xe2, 0x8c,
			0x5f, 0xa7, 0x9e, 0xac, 0x9d, 0x80, 0x23, 0xb5,
			0x3b, 0x28, 0x9d, 0x96,
		},
	},
};

/*
 * Unit test: compute the HMAC signature of the specified string with the
 * specified key and compare it to the expected result.
 */
static int
t_hmac_sha1_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t mac[HMAC_SHA1_MAC_LEN];

	t_hmac_sha1_complete(vector->key, vector->keylen,
	    (const uint8_t *)vector->msg, strlen(vector->msg),
	    mac);
	return (t_compare_mem(vector->mac, mac, HMAC_SHA1_MAC_LEN));
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
	n = sizeof t_hmac_sha1_vectors / sizeof t_hmac_sha1_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_hmac_sha1_vector, &t_hmac_sha1_vectors[i],
		    "%s", t_hmac_sha1_vectors[i].desc);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
