/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012 Dag-Erling Smørgrav
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

#include "t.h"

#if WITH_OPENSSL

#include <openssl/evp.h>
#include <openssl/hmac.h>

#define HMAC_SHA256_MAC_LEN 32

static void
t_hmac_sha256_complete(const void *key, size_t keylen,
    const void *msg, size_t msglen, uint8_t *mac)
{
	HMAC_CTX ctx;

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, keylen, EVP_sha256(), NULL);
	HMAC_Update(&ctx, msg, msglen);
	HMAC_Final(&ctx, mac, NULL);
	HMAC_CTX_cleanup(&ctx);
}

#else

#include <cryb/sha256.h>
#include <cryb/hmac.h>

#define t_hmac_sha256_complete(key, keylen, msg, msglen, mac)			\
	hmac_sha256_complete(key, keylen, msg, msglen, mac)

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
	const uint8_t mac[HMAC_SHA256_MAC_LEN];
} t_hmac_sha256_vectors[] = {
	{
		"zero-length key, zero-length message",
		t_zero,
		0,
		"",
		{
			0xb6, 0x13, 0x67, 0x9a, 0x08, 0x14, 0xd9, 0xec,
			0x77, 0x2f, 0x95, 0xd7, 0x78, 0xc3, 0x5f, 0xc5,
			0xff, 0x16, 0x97, 0xc4, 0x93, 0x71, 0x56, 0x53,
			0xc6, 0xc7, 0x12, 0x14, 0x42, 0x92, 0xc5, 0xad,
		},
	},
	{
		"NIST CSRC example 1 (64-byte key)",
		t_seq8,
		64,
		"Sample message for keylen=blocklen",
		{
			0x8b, 0xb9, 0xa1, 0xdb, 0x98, 0x06, 0xf2, 0x0d,
			0xf7, 0xf7, 0x7b, 0x82, 0x13, 0x8c, 0x79, 0x14,
			0xd1, 0x74, 0xd5, 0x9e, 0x13, 0xdc, 0x4d, 0x01,
			0x69, 0xc9, 0x05, 0x7b, 0x13, 0x3e, 0x1d, 0x62,
		},
	},
	{
		"NIST CSRC example 2 (32-byte key)",
		t_seq8,
		32,
		"Sample message for keylen<blocklen",
		{
			0xa2, 0x8c, 0xf4, 0x31, 0x30, 0xee, 0x69, 0x6a,
			0x98, 0xf1, 0x4a, 0x37, 0x67, 0x8b, 0x56, 0xbc,
			0xfc, 0xbd, 0xd9, 0xe5, 0xcf, 0x69, 0x71, 0x7f,
			0xec, 0xf5, 0x48, 0x0f, 0x0e, 0xbd, 0xf7, 0x90,
		},
	},
	{
		"NIST CSRC example 3 (100-byte key)",
		t_seq8,
		100,
		/* yes, the text should say keylen>blocklen */
		"Sample message for keylen=blocklen",
		{
			0xbd, 0xcc, 0xb6, 0xc7, 0x2d, 0xde, 0xad, 0xb5,
			0x00, 0xae, 0x76, 0x83, 0x86, 0xcb, 0x38, 0xcc,
			0x41, 0xc6, 0x3d, 0xbb, 0x08, 0x78, 0xdd, 0xb9,
			0xc7, 0xa3, 0x8a, 0x43, 0x1b, 0x78, 0x37, 0x8d,
		},
	},
	{
		"NIST CSRC example 4 (49-byte key)",
		t_seq8,
		49,
		"Sample message for keylen<blocklen, with truncated tag",
		{
			0x27, 0xa8, 0xb1, 0x57, 0x83, 0x9e, 0xfe, 0xac,
			0x98, 0xdf, 0x07, 0x0b, 0x33, 0x1d, 0x59, 0x36,
			0x18, 0xdd, 0xb9, 0x85, 0xd4, 0x03, 0xc0, 0xc7,
			0x86, 0xd2, 0x3b, 0x5d, 0x13, 0x2e, 0x57, 0xc7,
		},
	},
};

/*
 * Unit test: compute the HMAC signature of the specified string with the
 * specified key and compare it to the expected result.
 */
static int
t_hmac_sha256_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t mac[HMAC_SHA256_MAC_LEN];

	t_hmac_sha256_complete(vector->key, vector->keylen,
	    (const uint8_t *)vector->msg, strlen(vector->msg),
	    mac);
	if (memcmp(mac, vector->mac, HMAC_SHA256_MAC_LEN) != 0) {
		t_verbose("expected ");
		t_verbose_hex(vector->mac, HMAC_SHA256_MAC_LEN);
		t_verbose("\n");
		t_verbose("got      ");
		t_verbose_hex(mac, HMAC_SHA256_MAC_LEN);
		t_verbose("\n");
		return (0);
	}
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
	n = sizeof t_hmac_sha256_vectors / sizeof t_hmac_sha256_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_hmac_sha256_vector, &t_hmac_sha256_vectors[i],
		    t_hmac_sha256_vectors[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
