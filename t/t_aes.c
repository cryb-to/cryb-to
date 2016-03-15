/*-
 * Copyright (c) 2015 Dag-Erling Smørgrav
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
#include <string.h>

#include <cryb/aes.h>

#include <cryb/test.h>

struct t_case {
	const char *desc;
	int keylen;
	const uint8_t key[32];
	const uint8_t ptext[AES_BLOCK_LEN];
	const uint8_t ctext[AES_BLOCK_LEN];
};

/***************************************************************************
 * Test cases
 */
static struct t_case t_cases[] = {
	/* FIPS-197 test vectors */
	{
		.desc	= "FIPS-197 C.1",
		.keylen = 128,
		.key	= {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		},
		.ptext	= {
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
			0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
		},
		.ctext	= {
			0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
			0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
		},
	},
	{
		.desc	= "FIPS-197 C.2",
		.keylen = 192,
		.key	= {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		},
		.ptext	= {
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
			0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
		},
		.ctext	= {
			0xdd, 0xa9, 0x7c, 0xa4, 0x86, 0x4c, 0xdf, 0xe0,
			0x6e, 0xaf, 0x70, 0xa0, 0xec, 0x0d, 0x71, 0x91,
		},
	},
	{
		.desc	= "FIPS-197 C.3",
		.keylen = 256,
		.key	= {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
			0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		},
		.ptext	= {
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
			0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
		},
		.ctext	= {
			0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
			0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89,
		},
	},
};

/***************************************************************************
 * Test functions
 */
static int
t_aes_enc(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	uint8_t out[AES_BLOCK_LEN];
	aes_ctx ctx;

	asprintf(desc, "%s (encrypt)", t->desc);
	aes_init(&ctx, CIPHER_MODE_ENCRYPT, t->key, t->keylen / 8);
	aes_update(&ctx, t->ptext, AES_BLOCK_LEN, out);
	aes_finish(&ctx);
	return (t_compare_mem(t->ctext, out, AES_BLOCK_LEN));
}

static int
t_aes_dec(char **desc CRYB_UNUSED, void *arg)
{
	struct t_case *t = arg;
	uint8_t out[AES_BLOCK_LEN];
	aes_ctx ctx;

	asprintf(desc, "%s (decrypt)", t->desc);
	aes_init(&ctx, CIPHER_MODE_DECRYPT, t->key, t->keylen / 8);
	aes_update(&ctx, t->ctext, AES_BLOCK_LEN, out);
	aes_finish(&ctx);
	return (t_compare_mem(t->ptext, out, AES_BLOCK_LEN));
}


/***************************************************************************
 * Boilerplate
 */

static int
t_prepare(int argc, char *argv[])
{
	unsigned int i, n;

	(void)argc;
	(void)argv;
	n = sizeof t_cases / sizeof t_cases[0];
	for (i = 0; i < n; ++i) {
		t_add_test(t_aes_enc, &t_cases[i], t_cases[i].desc);
		t_add_test(t_aes_dec, &t_cases[i], t_cases[i].desc);
	}
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
