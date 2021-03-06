/*-
 * Copyright (c) 2015-2017 Dag-Erling Smørgrav
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

#ifndef CRYB_RC4_H_INCLUDED
#define CRYB_RC4_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/cipher.h>

CRYB_BEGIN

#define rc4_cipher			cryb_rc4_cipher
#define rc4_ctx				cryb_rc4_ctx
#define rc4_init			cryb_rc4_init
#define rc4_keystream			cryb_rc4_keystream
#define rc4_encrypt			cryb_rc4_encrypt
#define rc4_decrypt			cryb_rc4_decrypt
#define rc4_finish			cryb_rc4_finish

extern cipher_algorithm rc4_cipher;

typedef struct {
	uint8_t s[256], i, j;
} rc4_ctx;

void rc4_init(rc4_ctx *, cipher_mode mode, const uint8_t *, size_t);
size_t rc4_keystream(rc4_ctx *, uint8_t *, size_t);
size_t rc4_encrypt(rc4_ctx *, const void *, uint8_t *, size_t);
size_t rc4_decrypt(rc4_ctx *, const uint8_t *, void *, size_t);
void rc4_finish(rc4_ctx *);

CRYB_END

#endif
