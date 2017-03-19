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

#ifndef CRYB_SHA224_H_INCLUDED
#define CRYB_SHA224_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/digest.h>

CRYB_BEGIN

#define SHA224_BLOCK_LEN		64
#define SHA224_DIGEST_LEN		28

#define sha224_digest			cryb_sha224_digest
#define sha224_ctx			cryb_sha224_ctx
#define sha224_init			cryb_sha224_init
#define sha224_update			cryb_sha224_update
#define sha224_final			cryb_sha224_final
#define sha224_complete			cryb_sha224_complete

extern digest_algorithm sha224_digest;

typedef struct {
	uint32_t	 state[8];
	uint64_t	 count;
	uint8_t		 buf[64];
} sha224_ctx;

void sha224_init(sha224_ctx *);
void sha224_update(sha224_ctx *, const void *, size_t);
void sha224_final(sha224_ctx *, uint8_t *);
void sha224_complete(const void *, size_t, uint8_t *);

CRYB_END

#endif
