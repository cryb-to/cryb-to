/*-
 * Copyright (c) 2015-2016 Dag-Erling Smørgrav
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
#include <string.h>

#include <cryb/assert.h>
#include <cryb/bitwise.h>
#include <cryb/endian.h>
#include <cryb/memset_s.h>

#include <cryb/rc4.h>

void
rc4_init(rc4_ctx *ctx, cipher_mode mode CRYB_UNUSED,
    const uint8_t *key, size_t keylen)
{
	unsigned int i, j;
	uint8_t t;

	assert(mode == CIPHER_MODE_ENCRYPT || mode == CIPHER_MODE_DECRYPT);
	assert(keylen > 0);
	memset(ctx, 0, sizeof *ctx);
	for (i = 0; i < 256; ++i)
		ctx->s[i] = i;
	for (i = j = 0; i < 256; ++i) {
		j = (j + ctx->s[i] + key[i % keylen]) % 256;
		t = ctx->s[i];
		ctx->s[i] = ctx->s[j];
		ctx->s[j] = t;
	}
}

size_t
rc4_keystream(rc4_ctx *ctx, uint8_t *ks, size_t len)
{
	unsigned int i;
	uint8_t t;

	for (i = 0; i < len; ++i) {
		ctx->i = ctx->i + 1;
		ctx->j = ctx->j + ctx->s[ctx->i];
		t = ctx->s[ctx->i];
		ctx->s[ctx->i] = ctx->s[ctx->j];
		ctx->s[ctx->j] = t;
		t = ctx->s[ctx->i] + ctx->s[ctx->j];
		*ks++ = ctx->s[t];
	}
	return (len);
}
	
size_t
rc4_encrypt(rc4_ctx *ctx, const void *vpt, uint8_t *ct, size_t len)
{
	const uint8_t *pt = vpt;
	unsigned int i;
	uint8_t t, k;

	for (i = 0; i < len; ++i) {
		ctx->i = ctx->i + 1;
		ctx->j = ctx->j + ctx->s[ctx->i];
		t = ctx->s[ctx->i];
		ctx->s[ctx->i] = ctx->s[ctx->j];
		ctx->s[ctx->j] = t;
		t = ctx->s[ctx->i] + ctx->s[ctx->j];
		k = ctx->s[t];
		*ct++ = *pt++ ^ k;
	}
	return (len);
}

size_t
rc4_decrypt(rc4_ctx *ctx, const uint8_t *ct, void *vpt, size_t len)
{

	return (rc4_encrypt(ctx, ct, vpt, len));
}

void
rc4_finish(rc4_ctx *ctx)
{

	memset_s(ctx, 0, sizeof *ctx, sizeof *ctx);
}

cipher_algorithm rc4_cipher = {
	.name			 = "rc4",
	.contextlen		 = sizeof(rc4_ctx),
	.blocklen		 = 1,
	.keylen			 = 0,
	.init			 = (cipher_init_func)rc4_init,
	.keystream		 = (cipher_keystream_func)rc4_keystream,
	.encrypt		 = (cipher_encrypt_func)rc4_encrypt,
	.decrypt		 = (cipher_decrypt_func)rc4_decrypt,
	.finish			 = (cipher_finish_func)rc4_finish,
};
