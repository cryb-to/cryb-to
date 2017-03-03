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

#ifndef CRYB_AES_H_INCLUDED
#define CRYB_AES_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/cipher.h>

CRYB_BEGIN

#define AES_BLOCK_LEN			16

#define aes_ctx				cryb_aes_ctx
#define aes_init			cryb_aes_init
#define aes_update			cryb_aes_update
#define aes_finish			cryb_aes_finish

extern cipher_algorithm aes128_cipher;
extern cipher_algorithm aes192_cipher;
extern cipher_algorithm aes256_cipher;

typedef struct {
	int	 mode;
	int	 nr;
	uint32_t rk[68];
} aes_ctx;

void aes_init(aes_ctx *, cipher_mode, const uint8_t *, size_t);
void aes_update(aes_ctx *, const void *, size_t, void *);
void aes_finish(aes_ctx *);

CRYB_END

#endif
