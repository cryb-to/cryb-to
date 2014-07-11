/*-
 * Copyright (c) 2005-2013 Colin Percival
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Cryb$
 */

#ifndef CRYB_SHA224_H_INCLUDED
#define CRYB_SHA224_H_INCLUDED

#include <cryb/digest.h>

#define SHA224_BLOCK_LEN		64
#define SHA224_DIGEST_LEN		28

/*
 * Use #defines in order to avoid namespace collisions with anyone else's
 * SHA224 code (e.g., the code in OpenSSL).
 */
#define sha224_digest			cryb_sha224_digest
#define sha224_ctx			cryb_sha224_ctx
#define sha224_init			cryb_sha224_init
#define sha224_update			cryb_sha224_update
#define sha224_final			cryb_sha224_final
#define sha224_complete			cryb_sha224_complete

extern digest_algorithm sha224_digest;

/* Context structure for SHA224 operations. */
typedef struct {
	uint32_t state[8];
	uint64_t count;
	uint8_t buf[64];
} sha224_ctx;

/**
 * sha224_init(ctx):
 * Initialize the SHA224 context ${ctx}.
 */
void sha224_init(sha224_ctx *);

/**
 * sha224_update(ctx, in, len):
 * Input ${len} bytes from ${in} into the SHA224 context ${ctx}.
 */
void sha224_update(sha224_ctx *, const void *, size_t);

/**
 * sha224_final(ctx, digest):
 * Output the SHA224 hash of the data input to the context ${ctx} into the
 * buffer ${digest}.
 */
void sha224_final(sha224_ctx *, uint8_t[SHA224_DIGEST_LEN]);

/**
 * sha224_complete(in, len, digest):
 * Compute the SHA224 hash of ${len} bytes from $in} and write it to ${digest}.
 */
void sha224_complete(const void *, size_t, uint8_t[SHA224_DIGEST_LEN]);

/**
 * PBKDF2_SHA224(passwd, passwdlen, salt, saltlen, c, buf, dkLen):
 * Compute PBKDF2(passwd, salt, c, dkLen) using HMAC-SHA224 as the PRF, and
 * write the output to buf.  The value dkLen must be at most 32 * (2^32 - 1).
 */
void pbkdf2_sha224(const uint8_t *, size_t, const uint8_t *, size_t,
    uint64_t, uint8_t *, size_t);

#endif
