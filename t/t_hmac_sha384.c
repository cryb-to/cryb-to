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

#define HMAC_SHA384_MAC_LEN SHA384_DIGEST_LENGTH

static void
t_hmac_sha384_complete(const void *key, size_t keylen,
    const void *msg, size_t msglen, uint8_t *mac)
{
	HMAC_CTX ctx;

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, keylen, EVP_sha384(), NULL);
	HMAC_Update(&ctx, msg, msglen);
	HMAC_Final(&ctx, mac, NULL);
	HMAC_CTX_cleanup(&ctx);
}

#else

#include <cryb/sha384.h>
#include <cryb/hmac.h>

#define t_hmac_sha384_complete(key, keylen, msg, msglen, mac)			\
	hmac_sha384_complete(key, keylen, msg, msglen, mac)

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
	const uint8_t mac[HMAC_SHA384_MAC_LEN];
} t_hmac_sha384_vectors[] = {
	{
		"zero-length key, zero-length message",
		t_zero,
		0,
		"",
		{
			0x6c, 0x1f, 0x2e, 0xe9, 0x38, 0xfa, 0xd2, 0xe2,
			0x4b, 0xd9, 0x12, 0x98, 0x47, 0x43, 0x82, 0xca,
			0x21, 0x8c, 0x75, 0xdb, 0x3d, 0x83, 0xe1, 0x14,
			0xb3, 0xd4, 0x36, 0x77, 0x76, 0xd1, 0x4d, 0x35,
			0x51, 0x28, 0x9e, 0x75, 0xe8, 0x20, 0x9c, 0xd4,
			0xb7, 0x92, 0x30, 0x28, 0x40, 0x23, 0x4a, 0xdc,
		},
	},
	{
		"NIST CSRC example 1 (128-byte key)",
		t_seq8,
		128,
		"Sample message for keylen=blocklen",
		{
			0x63, 0xc5, 0xda, 0xa5, 0xe6, 0x51, 0x84, 0x7c,
			0xa8, 0x97, 0xc9, 0x58, 0x14, 0xab, 0x83, 0x0b,
			0xed, 0xed, 0xc7, 0xd2, 0x5e, 0x83, 0xee, 0xf9,
			0x19, 0x5c, 0xd4, 0x58, 0x57, 0xa3, 0x7f, 0x44,
			0x89, 0x47, 0x85, 0x8f, 0x5a, 0xf5, 0x0c, 0xc2,
			0xb1, 0xb7, 0x30, 0xdd, 0xf2, 0x96, 0x71, 0xa9,
		},
	},
	{
		"NIST CSRC example 2 (48-byte key)",
		t_seq8,
		48,
		"Sample message for keylen<blocklen",
		{
			0x6e, 0xb2, 0x42, 0xbd, 0xbb, 0x58, 0x2c, 0xa1,
			0x7b, 0xeb, 0xfa, 0x48, 0x1b, 0x1e, 0x23, 0x21,
			0x14, 0x64, 0xd2, 0xb7, 0xf8, 0xc2, 0x0b, 0x9f,
			0xf2, 0x20, 0x16, 0x37, 0xb9, 0x36, 0x46, 0xaf,
			0x5a, 0xe9, 0xac, 0x31, 0x6e, 0x98, 0xdb, 0x45,
			0xd9, 0xca, 0xe7, 0x73, 0x67, 0x5e, 0xee, 0xd0,
		},
	},
	{
		"NIST CSRC example 3 (200-byte key)",
		t_seq8,
		200,
		/* yes, the text should say keylen>blocklen */
		"Sample message for keylen=blocklen",
		{
			0x5b, 0x66, 0x44, 0x36, 0xdf, 0x69, 0xb0, 0xca,
			0x22, 0x55, 0x12, 0x31, 0xa3, 0xf0, 0xa3, 0xd5,
			0xb4, 0xf9, 0x79, 0x91, 0x71, 0x3c, 0xfa, 0x84,
			0xbf, 0xf4, 0xd0, 0x79, 0x2e, 0xff, 0x96, 0xc2,
			0x7d, 0xcc, 0xbb, 0xb6, 0xf7, 0x9b, 0x65, 0xd5,
			0x48, 0xb4, 0x0e, 0x85, 0x64, 0xce, 0xf5, 0x94,
		},
	},
	{
		"NIST CSRC example 4 (49-byte key)",
		t_seq8,
		49,
		"Sample message for keylen<blocklen, with truncated tag",
		{
			0xc4, 0x81, 0x30, 0xd3, 0xdf, 0x70, 0x3d, 0xd7,
			0xcd, 0xaa, 0x56, 0x80, 0x0d, 0xfb, 0xd2, 0xba,
			0x24, 0x58, 0x32, 0x0e, 0x6e, 0x1f, 0x98, 0xfe,
			0xc8, 0xad, 0x9f, 0x57, 0xf4, 0x38, 0x00, 0xdf,
			0x36, 0x15, 0xce, 0xb1, 0x9a, 0xb6, 0x48, 0xe1,
			0xec, 0xdd, 0x8c, 0x73, 0x0a, 0xf9, 0x5c, 0x8a,
		},
	},
};

/*
 * Unit test: compute the HMAC signature of the specified string with the
 * specified key and compare it to the expected result.
 */
static int
t_hmac_sha384_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t mac[HMAC_SHA384_MAC_LEN];

	t_hmac_sha384_complete(vector->key, vector->keylen,
	    (const uint8_t *)vector->msg, strlen(vector->msg),
	    mac);
	if (memcmp(mac, vector->mac, HMAC_SHA384_MAC_LEN) != 0) {
		t_verbose("expected ");
		t_verbose_hex(vector->mac, HMAC_SHA384_MAC_LEN);
		t_verbose("\n");
		t_verbose("got      ");
		t_verbose_hex(mac, HMAC_SHA384_MAC_LEN);
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
	n = sizeof t_hmac_sha384_vectors / sizeof t_hmac_sha384_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_hmac_sha384_vector, &t_hmac_sha384_vectors[i],
		    t_hmac_sha384_vectors[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
