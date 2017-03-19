/*
 * Copyright (c) 2006-2007 Christophe Devine
 * Copyright (c) 2017 Dag-Erling Smørgrav
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

#ifndef CRYB_SHA512_H_INCLUDED
#define CRYB_SHA512_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/digest.h>

CRYB_BEGIN

#define SHA512_BLOCK_LEN		128
#define SHA512_DIGEST_LEN		64

#define sha512_digest			cryb_sha512_digest
#define sha512_ctx			cryb_sha512_ctx
#define sha512_init			cryb_sha512_init
#define sha512_update			cryb_sha512_update
#define sha512_final			cryb_sha512_final
#define sha512_complete			cryb_sha512_complete

extern digest_algorithm sha512_digest;

typedef struct {
	uint8_t		 block[128];
	unsigned int	 blocklen;
	uint64_t	 h[8];
	uint64_t	 bitlen[2];
} sha512_ctx;

void sha512_init(sha512_ctx *);
void sha512_update(sha512_ctx *, const void *, size_t);
void sha512_final(sha512_ctx *, uint8_t *);
void sha512_complete(const void *, size_t, uint8_t *);

CRYB_END

#endif
