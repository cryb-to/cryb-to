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
#include <openssl/sha.h>

#define HMAC_SHA512_MAC_LEN SHA512_DIGEST_LENGTH

static void
t_hmac_sha512_complete(const void *key, size_t keylen,
    const void *msg, size_t msglen, uint8_t *mac)
{
	HMAC_CTX ctx;

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, keylen, EVP_sha512(), NULL);
	HMAC_Update(&ctx, msg, msglen);
	HMAC_Final(&ctx, mac, NULL);
	HMAC_CTX_cleanup(&ctx);
}

#else

#include <cryb/sha512.h>
#include <cryb/hmac.h>

#define t_hmac_sha512_complete(key, keylen, msg, msglen, mac)			\
	hmac_sha512_complete(key, keylen, msg, msglen, mac)

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
	const uint8_t mac[HMAC_SHA512_MAC_LEN];
} t_hmac_sha512_vectors[] = {
	{
		"zero-length key, zero-length message",
		t_zero,
		0,
		"",
		{
			0xb9, 0x36, 0xce, 0xe8, 0x6c, 0x9f, 0x87, 0xaa,
			0x5d, 0x3c, 0x6f, 0x2e, 0x84, 0xcb, 0x5a, 0x42,
			0x39, 0xa5, 0xfe, 0x50, 0x48, 0x0a, 0x6e, 0xc6,
			0x6b, 0x70, 0xab, 0x5b, 0x1f, 0x4a, 0xc6, 0x73,
			0x0c, 0x6c, 0x51, 0x54, 0x21, 0xb3, 0x27, 0xec,
			0x1d, 0x69, 0x40, 0x2e, 0x53, 0xdf, 0xb4, 0x9a,
			0xd7, 0x38, 0x1e, 0xb0, 0x67, 0xb3, 0x38, 0xfd,
			0x7b, 0x0c, 0xb2, 0x22, 0x47, 0x22, 0x5d, 0x47,
		},
	},
	{
		"NIST CSRC example 1 (128-byte key)",
		t_seq8,
		128,
		"Sample message for keylen=blocklen",
		{
			0xfc, 0x25, 0xe2, 0x40, 0x65, 0x8c, 0xa7, 0x85,
			0xb7, 0xa8, 0x11, 0xa8, 0xd3, 0xf7, 0xb4, 0xca,
			0x48, 0xcf, 0xa2, 0x6a, 0x8a, 0x36, 0x6b, 0xf2,
			0xcd, 0x1f, 0x83, 0x6b, 0x05, 0xfc, 0xb0, 0x24,
			0xbd, 0x36, 0x85, 0x30, 0x81, 0x81, 0x1d, 0x6c,
			0xea, 0x42, 0x16, 0xeb, 0xad, 0x79, 0xda, 0x1c,
			0xfc, 0xb9, 0x5e, 0xa4, 0x58, 0x6b, 0x8a, 0x0c,
			0xe3, 0x56, 0x59, 0x6a, 0x55, 0xfb, 0x13, 0x47,
		},
	},
	{
		"NIST CSRC example 2 (64-byte key)",
		t_seq8,
		64,
		"Sample message for keylen<blocklen",
		{
			0xfd, 0x44, 0xc1, 0x8b, 0xda, 0x0b, 0xb0, 0xa6,
			0xce, 0x0e, 0x82, 0xb0, 0x31, 0xbf, 0x28, 0x18,
			0xf6, 0x53, 0x9b, 0xd5, 0x6e, 0xc0, 0x0b, 0xdc,
			0x10, 0xa8, 0xa2, 0xd7, 0x30, 0xb3, 0x63, 0x4d,
			0xe2, 0x54, 0x5d, 0x63, 0x9b, 0x0f, 0x2c, 0xf7,
			0x10, 0xd0, 0x69, 0x2c, 0x72, 0xa1, 0x89, 0x6f,
			0x1f, 0x21, 0x1c, 0x2b, 0x92, 0x2d, 0x1a, 0x96,
			0xc3, 0x92, 0xe0, 0x7e, 0x7e, 0xa9, 0xfe, 0xdc,
		},
	},
	{
		"NIST CSRC example 3 (200-byte key)",
		t_seq8,
		200,
		/* yes, the text should say keylen>blocklen */
		"Sample message for keylen=blocklen",
		{
			0xd9, 0x3e, 0xc8, 0xd2, 0xde, 0x1a, 0xd2, 0xa9,
			0x95, 0x7c, 0xb9, 0xb8, 0x3f, 0x14, 0xe7, 0x6a,
			0xd6, 0xb5, 0xe0, 0xcc, 0xe2, 0x85, 0x07, 0x9a,
			0x12, 0x7d, 0x3b, 0x14, 0xbc, 0xcb, 0x7a, 0xa7,
			0x28, 0x6d, 0x4a, 0xc0, 0xd4, 0xce, 0x64, 0x21,
			0x5f, 0x2b, 0xc9, 0xe6, 0x87, 0x0b, 0x33, 0xd9,
			0x74, 0x38, 0xbe, 0x4a, 0xaa, 0x20, 0xcd, 0xa5,
			0xc5, 0xa9, 0x12, 0xb4, 0x8b, 0x8e, 0x27, 0xf3,
		},
	},
	{
		"NIST CSRC example 4 (49-byte key)",
		t_seq8,
		49,
		"Sample message for keylen<blocklen, with truncated tag",
		{
			0x00, 0xf3, 0xe9, 0xa7, 0x7b, 0xb0, 0xf0, 0x6d,
			0xe1, 0x5f, 0x16, 0x06, 0x03, 0xe4, 0x2b, 0x50,
			0x28, 0x75, 0x88, 0x08, 0x59, 0x66, 0x64, 0xc0,
			0x3e, 0x1a, 0xb8, 0xfb, 0x2b, 0x07, 0x67, 0x78,
			0x05, 0x63, 0xae, 0xdc, 0x64, 0x49, 0x60, 0xd4,
			0xf0, 0xc0, 0xc5, 0xd2, 0x39, 0xf6, 0x7a, 0x2a,
			0x61, 0xb1, 0x41, 0xe8, 0xc8, 0x71, 0xf3, 0xd4,
			0x0d, 0xb2, 0xc6, 0x05, 0x58, 0x8d, 0xab, 0x92,
		},
	},
};

/*
 * Unit test: compute the HMAC signature of the specified string with the
 * specified key and compare it to the expected result.
 */
static int
t_hmac_sha512_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t mac[HMAC_SHA512_MAC_LEN];

	t_hmac_sha512_complete(vector->key, vector->keylen,
	    (const uint8_t *)vector->msg, strlen(vector->msg),
	    mac);
	if (memcmp(mac, vector->mac, HMAC_SHA512_MAC_LEN) != 0) {
		t_verbose("expected ");
		t_verbose_hex(vector->mac, HMAC_SHA512_MAC_LEN);
		t_verbose("\n");
		t_verbose("got      ");
		t_verbose_hex(mac, HMAC_SHA512_MAC_LEN);
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
	n = sizeof t_hmac_sha512_vectors / sizeof t_hmac_sha512_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_hmac_sha512_vector, &t_hmac_sha512_vectors[i],
		    t_hmac_sha512_vectors[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
