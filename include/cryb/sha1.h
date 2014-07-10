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

#ifndef CRYB_SHA1_H_INCLUDED
#define CRYB_SHA1_H_INCLUDED

#include <cryb/digest.h>

#define SHA1_BLOCK_LEN			64
#define SHA1_DIGEST_LEN			20

#define sha1_digest			cryb_sha1_digest
#define sha1_ctx			cryb_sha1_ctx
#define sha1_init			cryb_sha1_init
#define sha1_update			cryb_sha1_update
#define sha1_final			cryb_sha1_final
#define sha1_complete			cryb_sha1_complete

extern digest_algorithm sha1_digest;

typedef struct {
	uint8_t block[64];
	size_t blocklen;
	uint64_t bitlen;
	uint32_t h[5], k[4];
} sha1_ctx;

extern digest_algorithm sha1_algorithm;

void sha1_init(sha1_ctx *);
void sha1_update(sha1_ctx *, const void *, size_t);
void sha1_final(sha1_ctx *, void *);
void sha1_complete(const void *, size_t, void *);

#endif
