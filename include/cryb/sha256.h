/*-
 * Copyright (c) 2005-2013 Colin Percival
 * Copyright (c) 2014-2017 Dag-Erling Smørgrav
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
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef CRYB_SHA256_H_INCLUDED
#define CRYB_SHA256_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/digest.h>

CRYB_BEGIN

#define SHA256_BLOCK_LEN		64
#define SHA256_DIGEST_LEN		32

#define sha256_digest			cryb_sha256_digest
#define sha256_ctx			cryb_sha256_ctx
#define sha256_init			cryb_sha256_init
#define sha256_update			cryb_sha256_update
#define sha256_final			cryb_sha256_final
#define sha256_complete			cryb_sha256_complete

extern digest_algorithm sha256_digest;

typedef struct {
	uint32_t	 state[8];
	uint64_t	 count;
	uint8_t		 buf[64];
} sha256_ctx;

void sha256_init(sha256_ctx *);
void sha256_update(sha256_ctx *, const void *, size_t);
void sha256_final(sha256_ctx *, uint8_t *);
void sha256_complete(const void *, size_t, uint8_t *);

CRYB_END

#endif
