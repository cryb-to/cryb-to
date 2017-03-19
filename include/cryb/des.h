/*-
 * Copyright (c) 2015-2016 Dag-Erling Smørgrav
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

#ifndef CRYB_DES_H_INCLUDED
#define CRYB_DES_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/cipher.h>

CRYB_BEGIN

#define DES_BLOCK_LEN			8

#define des_cipher			cryb_des56_cipher
#define des56_cipher			cryb_des56_cipher
#define des112_cipher			cryb_des112_cipher
#define des268_cipher			cryb_des168_cipher
#define des_ctx				cryb_des_ctx
#define des_init			cryb_des_init
#define des_encrypt			cryb_des_encrypt
#define des_decrypt			cryb_des_decrypt
#define des_finish			cryb_des_finish

extern cipher_algorithm des_cipher;

typedef struct {
	uint32_t sk1[32];
	uint32_t sk2[32];
	uint32_t sk3[32];
} des_ctx;

void des_init(des_ctx *, cipher_mode mode, const uint8_t *, size_t);
size_t des_encrypt(des_ctx *, const void *, uint8_t *, size_t);
size_t des_decrypt(des_ctx *, const uint8_t *, void *, size_t);
void des_finish(des_ctx *);

CRYB_END

#endif
