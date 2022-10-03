/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012-2022 Dag-Erling Smørgrav
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
#include <unistd.h>

#include <cryb/test.h>

#if WITH_OPENSSL

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/md5.h>

#define HMAC_MD5_MAC_LEN MD5_DIGEST_LENGTH

static void
t_hmac_md5_complete(const void *key, size_t keylen,
    const void *msg, size_t msglen, uint8_t *mac)
{
	HMAC_CTX *ctx;

	ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, keylen, EVP_md5(), NULL);
	HMAC_Update(ctx, msg, msglen);
	HMAC_Final(ctx, mac, NULL);
	HMAC_CTX_free(ctx);
}

#else

#include <cryb/md5.h>
#include <cryb/hmac.h>

#define t_hmac_md5_complete(key, keylen, msg, msglen, mac)			\
	hmac_md5_complete(key, keylen, msg, msglen, mac)

#endif

/*
 * Test vectors from RFC 2104
 */
static struct t_vector {
	const char *desc;
	const uint8_t *key;
	size_t keylen;
	const uint8_t *msg;
	size_t msglen;
	const uint8_t mac[HMAC_MD5_MAC_LEN];
} t_hmac_md5_vectors[] = {
	{
		"zero-length key, zero-length message",
		(uint8_t[]){},
		0,
		(uint8_t[]){},
		0,
		{
			0x74, 0xe6, 0xf7, 0x29, 0x8a, 0x9c, 0x2d, 0x16,
			0x89, 0x35, 0xf5, 0x8c, 0x00, 0x1b, 0xad, 0x88,
		},
	},
	{
		"RFC 2104 example 1",
		(uint8_t[]){
			0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
			0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
		},
		16,
		(uint8_t *)"Hi There",
		8,
		{
			0x92, 0x94, 0x72, 0x7a, 0x36, 0x38, 0xbb, 0x1c,
			0x13, 0xf4, 0x8e, 0xf8, 0x15, 0x8b, 0xfc, 0x9d,
		},
	},
	{
		"RFC 2104 example 2",
		(uint8_t *)"Jefe",
		4,
		(uint8_t *)"what do ya want for nothing?",
		28,
		{
			0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03,
			0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38,
		},
	},
	{
		"RFC 2104 example 3",
		(uint8_t[]){
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		},
		16,
		(uint8_t[]){
			0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
			0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
			0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
			0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
			0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
			0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
			0xdd, 0xdd,
		},
		50,
		{
			0x56, 0xbe, 0x34, 0x52, 0x1d, 0x14, 0x4c, 0x88,
			0xdb, 0xb8, 0xc7, 0x33, 0xf0, 0xe8, 0xb3, 0xf6,
		},
	},
};

/*
 * Unit test: compute the HMAC signature of the specified string with the
 * specified key and compare it to the expected result.
 */
static int
t_hmac_md5_vector(char **desc CRYB_UNUSED, void *arg)
{
	struct t_vector *vector = (struct t_vector *)arg;
	uint8_t mac[HMAC_MD5_MAC_LEN];

	t_hmac_md5_complete(vector->key, vector->keylen,
	    (const uint8_t *)vector->msg, vector->msglen,
	    mac);
	return (t_compare_mem(vector->mac, mac, HMAC_MD5_MAC_LEN));
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
	n = sizeof t_hmac_md5_vectors / sizeof t_hmac_md5_vectors[0];
	for (i = 0; i < n; ++i)
		t_add_test(t_hmac_md5_vector, &t_hmac_md5_vectors[i],
		    "%s", t_hmac_md5_vectors[i].desc);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
