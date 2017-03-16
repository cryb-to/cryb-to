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

#ifndef CRYB_CHACHA_H_INCLUDED
#define CRYB_CHACHA_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/cipher.h>

CRYB_BEGIN

#define chacha_cipher			cryb_chacha_cipher
#define chacha_ctx			cryb_chacha_ctx
#define chacha_init			cryb_chacha_init
#define chacha_reset			cryb_chacha_reset
#define chacha_keystream		cryb_chacha_keystream
#define chacha_encrypt			cryb_chacha_encrypt
#define chacha_decrypt			cryb_chacha_decrypt
#define chacha_finish			cryb_chacha_finish

extern cipher_algorithm chacha_cipher;

typedef struct {
	uint32_t state[16];
	unsigned int rounds;
} chacha_ctx;

void chacha_init(chacha_ctx *, cipher_mode, const uint8_t *, size_t);
void chacha_reset(chacha_ctx *, const uint8_t *, unsigned int);
size_t chacha_keystream(chacha_ctx *, uint8_t *, size_t);
size_t chacha_encrypt(chacha_ctx *, const void *, uint8_t *, size_t);
size_t chacha_decrypt(chacha_ctx *, const uint8_t *, void *, size_t);
void chacha_finish(chacha_ctx *);

CRYB_END

#endif
