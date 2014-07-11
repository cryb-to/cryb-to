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

#ifndef CRYB_SHA256_H_INCLUDED
#define CRYB_SHA256_H_INCLUDED

#include <cryb/digest.h>

#define SHA256_BLOCK_LEN		64
#define SHA256_DIGEST_LEN		32

/*
 * Use #defines in order to avoid namespace collisions with anyone else's
 * SHA256 code (e.g., the code in OpenSSL).
 */
#define sha256_digest			cryb_sha256_digest
#define sha256_ctx			cryb_sha256_ctx
#define sha256_init			cryb_sha256_init
#define sha256_update			cryb_sha256_update
#define sha256_final			cryb_sha256_final
#define sha256_complete			cryb_sha256_complete

extern digest_algorithm sha256_digest;

/* Context structure for SHA256 operations. */
typedef struct {
	uint32_t state[8];
	uint64_t count;
	uint8_t buf[64];
} sha256_ctx;

/**
 * sha256_init(ctx):
 * Initialize the SHA256 context ${ctx}.
 */
void sha256_init(sha256_ctx *);

/**
 * sha256_update(ctx, in, len):
 * Input ${len} bytes from ${in} into the SHA256 context ${ctx}.
 */
void sha256_update(sha256_ctx *, const void *, size_t);

/**
 * sha256_final(ctx, digest):
 * Output the SHA256 hash of the data input to the context ${ctx} into the
 * buffer ${digest}.
 */
void sha256_final(sha256_ctx *, uint8_t[SHA256_DIGEST_LEN]);

/**
 * sha256_complete(in, len, digest):
 * Compute the SHA256 hash of ${len} bytes from $in} and write it to ${digest}.
 */
void sha256_complete(const void *, size_t, uint8_t[SHA256_DIGEST_LEN]);

/**
 * PBKDF2_SHA256(passwd, passwdlen, salt, saltlen, c, buf, dkLen):
 * Compute PBKDF2(passwd, salt, c, dkLen) using HMAC-SHA256 as the PRF, and
 * write the output to buf.  The value dkLen must be at most 32 * (2^32 - 1).
 */
void pbkdf2_sha256(const uint8_t *, size_t, const uint8_t *, size_t,
    uint64_t, uint8_t *, size_t);

#endif
