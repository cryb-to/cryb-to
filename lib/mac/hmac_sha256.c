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
 */

#include "cryb/impl.h"

#if HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#if HAVE_ENDIAN_H
#define _BSD_SOURCE
#include <endian.h>
#endif

#include <stdint.h>
#include <string.h>

#include <cryb/hmac_sha256.h>

void
hmac_sha256_init(hmac_sha256_ctx *ctx, const void *key, size_t keylen)
{
	uint8_t keybuf[SHA256_BLOCK_LEN], pad[SHA256_BLOCK_LEN];

	/* prepare key */
	memset(keybuf, 0, sizeof keybuf);
        if (keylen > sizeof keybuf)
                sha256_complete(key, keylen, keybuf);
        else
                memcpy(keybuf, key, keylen);

	/* input pad */
	for (unsigned int i = 0; i < sizeof pad; ++i)
		pad[i] = 0x36 ^ keybuf[i];
	sha256_init(&ctx->ictx);
	sha256_update(&ctx->ictx, pad, sizeof pad);

	/* output pad */
	for (unsigned int i = 0; i < sizeof pad; ++i)
		pad[i] = 0x5c ^ keybuf[i];
	sha256_init(&ctx->octx);
	sha256_update(&ctx->octx, pad, sizeof pad);

	/* hide the evidence */
	memset(keybuf, 0, sizeof keybuf);
	memset(pad, 0, sizeof pad);
}

void
hmac_sha256_update(hmac_sha256_ctx *ctx, const void *buf, size_t len)
{

	sha256_update(&ctx->ictx, buf, len);
}

void
hmac_sha256_final(hmac_sha256_ctx *ctx, uint8_t *mac)
{
	uint8_t digest[SHA256_DIGEST_LEN];

	sha256_final(&ctx->ictx, digest);
	sha256_update(&ctx->octx, digest, sizeof digest);
	sha256_final(&ctx->octx, mac);
	memset(ctx, 0, sizeof *ctx);
}

void
hmac_sha256_complete(const void *key, size_t keylen,
    const void *buf, size_t len, uint8_t *mac)
{
	hmac_sha256_ctx ctx;

	hmac_sha256_init(&ctx, key, keylen);
	hmac_sha256_update(&ctx, buf, len);
	hmac_sha256_final(&ctx, mac);
}
