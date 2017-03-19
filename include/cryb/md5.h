/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012-2016 Dag-Erling Smørgrav
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

#ifndef CRYB_MD5_H_INCLUDED
#define CRYB_MD5_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/digest.h>

CRYB_BEGIN

#define MD5_BLOCK_LEN			64
#define MD5_DIGEST_LEN			16

#define md5_digest			cryb_md5_digest
#define md5_ctx				cryb_md5_ctx
#define md5_init			cryb_md5_init
#define md5_update			cryb_md5_update
#define md5_final			cryb_md5_final
#define md5_complete			cryb_md5_complete

extern digest_algorithm md5_digest;

typedef struct {
	uint8_t		 block[64];
	size_t		 blocklen;
	uint32_t	 state[4];
	uint64_t	 bitlen;
} md5_ctx;

void md5_init(md5_ctx *);
void md5_update(md5_ctx *, const void *, size_t);
void md5_final(md5_ctx *, uint8_t *);
void md5_complete(const void *, size_t, uint8_t *);

CRYB_END

#endif
