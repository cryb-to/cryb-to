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

#define HMAC_SHA224_MAC_LEN SHA224_DIGEST_LENGTH

static void
t_hmac_sha224_complete(const void *key, size_t keylen,
    const void *msg, size_t msglen, uint8_t *mac)
{
	HMAC_CTX ctx;

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, keylen, EVP_sha224(), NULL);
	HMAC_Update(&ctx, msg, msglen);
	HMAC_Final(&ctx, mac, NULL);
	HMAC_CTX_cleanup(&ctx);
}

#else

#include <cryb/sha224.h>
#include <cryb/hmac.h>

#define t_hmac_sha224_complete(key, keylen, msg, msglen, mac)			\
	hmac_sha224_complete(key, keylen, msg, msglen, mac)

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
	const uint8_t mac[HMAC_SHA224_MAC_LEN];
} t_hmac_sha224_vectors[] = {
	{
		"zero-length key, zero-length message",
		t_zero,
		0,
		"",
		{
			0x5c, 0xe1, 0x4f, 0x72, 0x89, 0x46, 0x62, 0x21,
			0x3e, 0x27, 0x48, 0xd2, 0xa6, 0xba, 0x23, 0x4b,
			0x74, 0x26, 0x39, 0x10, 0xce, 0xdd, 0xe2, 0xf5,
			0xa9, 0x27, 0x15, 0x24,
		},
	},
	{
		"NIST CSRC example 1 (64-byte key)",
		t_seq8,
		64,
		"Sample message for keylen=blocklen",
		{
			0xc7, 0x40, 0x5e, 0x3a, 0xe0, 0x58, 0xe8, 0xcd,
			0x30, 0xb0, 0x8b, 0x41, 0x40, 0x24, 0x85, 0x81,
			0xed, 0x17, 0x4c, 0xb3, 0x4e, 0x12, 0x24, 0xbc,
			0xc1, 0xef, 0xc8, 0x1b,
		},
	},
	{
		"NIST CSRC example 2 (28-byte key)",
		t_seq8,
		28,
		"Sample message for keylen<blocklen",
		{
			0xe3, 0xd2, 0x49, 0xa8, 0xcf, 0xb6, 0x7e, 0xf8,
			0xb7, 0xa1, 0x69, 0xe9, 0xa0, 0xa5, 0x99, 0x71,
			0x4a, 0x2c, 0xec, 0xba, 0x65, 0x99, 0x9a, 0x51,
			0xbe, 0xb8, 0xfb, 0xbe,
		},
	},
	{
		"NIST CSRC example 3 (100-byte key)",
		t_seq8,
		100,
		/* yes, the text should say keylen>blocklen */
		"Sample message for keylen=blocklen",
		{
			0x91, 0xc5, 0x25, 0x09, 0xe5, 0xaf, 0x85, 0x31,
			0x60, 0x1a, 0xe6, 0x23, 0x00, 0x99, 0xd9, 0x0b,
			0xef, 0x88, 0xaa, 0xef, 0xb9, 0x61, 0xf4, 0x08,
			0x0a, 0xbc, 0x01, 0x4d,
		},
	},
	{
		"NIST CSRC example 4 (49-byte key)",
		t_seq8,
		49,
		"Sample message for keylen<blocklen, with truncated tag",
		{
			0xd5, 0x22, 0xf1, 0xdf, 0x59, 0x6c, 0xa4, 0xb4,
			0xb1, 0xc2, 0x3d, 0x27, 0xbd, 0xe0, 0x67, 0xd6,
			0x15, 0x3b, 0xa9, 0x72, 0x5f, 0xd5, 0xcd, 0xe0,
			0xaf, 0x4a, 0x2a, 0x42,
		},
	},
};

/*
 * Unit test: compute the HMAC signature of the specified string with the
 * specified key and compare it to the expected result.
 */
static int
t_hmac_sha224_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t mac[HMAC_SHA224_MAC_LEN];

	t_hmac_sha224_complete(vector->key, vector->keylen,
	    (const uint8_t *)vector->msg, strlen(vector->msg),
	    mac);
	return (t_compare_mem(vector->mac, mac, HMAC_SHA224_MAC_LEN));
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
	n = sizeof t_hmac_sha224_vectors / sizeof t_hmac_sha224_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_hmac_sha224_vector, &t_hmac_sha224_vectors[i],
		    "%s", t_hmac_sha224_vectors[i].desc);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
