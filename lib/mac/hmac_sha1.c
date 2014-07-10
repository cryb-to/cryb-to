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

#ifdef HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#ifdef HAVE_ENDIAN_H
#define _BSD_SOURCE
#include <endian.h>
#endif

#include <stdint.h>
#include <string.h>

#include <cryb/hmac_sha1.h>

void
hmac_sha1_init(hmac_sha1_ctx *ctx, const void *key, size_t keylen)
{
	uint8_t ipad[64];

	memset(ctx, 0, sizeof *ctx);
#if 1
        if (keylen > sizeof ctx->key)
                sha1_complete(key, keylen, ctx->key);
        else
                memcpy(ctx->key, key, keylen);
#else
	uint8_t keybuf[sizeof ctx->key] = { 0 };
	sha1_complete(key, keylen, keybuf);
	if (keylen > sizeof ctx->key)
		memcpy(ctx->key, keybuf, SHA1_DIGEST_LEN);
	else
		memcpy(ctx->key, key, keylen);
#endif
	sha1_init(&ctx->sha1_ctx);
	for (unsigned int i = 0; i < sizeof ipad; ++i)
		ipad[i] = 0x36 ^ ctx->key[i];
	sha1_update(&ctx->sha1_ctx, ipad, sizeof ipad);
}

void
hmac_sha1_update(hmac_sha1_ctx *ctx, const void *buf, size_t len)
{

	sha1_update(&ctx->sha1_ctx, buf, len);
}

void
hmac_sha1_final(hmac_sha1_ctx *ctx, void *mac)
{
	uint8_t digest[20], opad[64];

	sha1_final(&ctx->sha1_ctx, digest);
	for (unsigned int i = 0; i < sizeof opad; ++i)
		opad[i] = 0x5c ^ ctx->key[i];
	sha1_init(&ctx->sha1_ctx);
	sha1_update(&ctx->sha1_ctx, opad, sizeof opad);
	sha1_update(&ctx->sha1_ctx, digest, sizeof digest);
	sha1_final(&ctx->sha1_ctx, mac);
	memset(ctx, 0, sizeof *ctx);
}

void
hmac_sha1_complete(const void *key, size_t keylen,
    const void *buf, size_t len, void *mac)
{
	hmac_sha1_ctx ctx;

	hmac_sha1_init(&ctx, key, keylen);
	hmac_sha1_update(&ctx, buf, len);
	hmac_sha1_final(&ctx, mac);
}
