/*-
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

#ifndef CRYB_SALSA_H_INCLUDED
#define CRYB_SALSA_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/cipher.h>

CRYB_BEGIN

#define salsa_cipher			cryb_salsa_cipher
#define salsa_ctx			cryb_salsa_ctx
#define salsa_init			cryb_salsa_init
#define salsa_reset			cryb_salsa_reset
#define salsa_keystream			cryb_salsa_keystream
#define salsa_encrypt			cryb_salsa_encrypt
#define salsa_decrypt			cryb_salsa_decrypt
#define salsa_finish			cryb_salsa_finish

extern cipher_algorithm salsa_cipher;

typedef struct {
	uint32_t state[16];
	unsigned int rounds;
} salsa_ctx;

void salsa_init(salsa_ctx *, cipher_mode, const uint8_t *, size_t);
void salsa_reset(salsa_ctx *, const uint8_t *, unsigned int);
size_t salsa_keystream(salsa_ctx *, uint8_t *, size_t);
size_t salsa_encrypt(salsa_ctx *, const void *, uint8_t *, size_t);
size_t salsa_decrypt(salsa_ctx *, const uint8_t *, void *, size_t);
void salsa_finish(salsa_ctx *);

CRYB_END

#endif
