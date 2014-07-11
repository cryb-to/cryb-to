/*-
 * Copyright (c) 2012-2014 The University of Oslo
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
 * $Cryb$
 */

#ifndef CRYB_HMAC_SHA512_H_INCLUDED
#define CRYB_HMAC_SHA512_H_INCLUDED

#include <cryb/sha512.h>

#define HMAC_SHA512_MAC_LEN		64

#define hmac_sha512_ctx			cryb_hmac_sha512_ctx
#define hmac_sha512_init		cryb_hmac_sha512_init
#define hmac_sha512_update		cryb_hmac_sha512_update
#define hmac_sha512_final		cryb_hmac_sha512_final
#define hmac_sha512_complete		cryb_hmac_sha512_complete

typedef struct {
	sha512_ctx ictx;
	sha512_ctx octx;
} hmac_sha512_ctx;

void hmac_sha512_init(hmac_sha512_ctx *, const void *, size_t);
void hmac_sha512_update(hmac_sha512_ctx *, const void *, size_t);
void hmac_sha512_final(hmac_sha512_ctx *, void *);
void hmac_sha512_complete(const void *, size_t, const void *, size_t, void *);

#endif
