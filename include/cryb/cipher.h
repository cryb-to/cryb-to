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

#ifndef CRYB_CIPHER_H_INCLUDED
#define CRYB_CIPHER_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

CRYB_BEGIN

const char *cryb_cipher_version(void);

#define cipher_ctx			cryb_cipher_ctx
#define cipher_init_func		cryb_cipher_init_func
#define cipher_update_func		cryb_cipher_update_func
#define cipher_finish_func		cryb_cipher_finish_func
#define cipher_algorithm		cryb_cipher_algorithm

typedef enum {
	CIPHER_MODE_ENCRYPT	 = 0,
	CIPHER_MODE_DECRYPT	 = 1,
} cipher_mode;

typedef void cipher_ctx;
typedef void (*cipher_init_func)(cipher_ctx *, cipher_mode, const uint8_t *, size_t);
typedef void (*cipher_update_func)(cipher_ctx *, const void *, size_t, void *);
typedef void (*cipher_finish_func)(cipher_ctx *);

typedef struct {
	const char		*name;		/* algorithm name */
	size_t			 contextlen;	/* size of context structure */
	size_t			 blocklen;	/* block length */
	size_t			 keylen;	/* key length */
	cipher_init_func	 init;		/* initialization method */
	cipher_update_func	 update;	/* {enc,dec}ryption method */
	cipher_finish_func	 finish;	/* finalization method */
} cipher_algorithm;

#define get_cipher_algorithm		cryb_get_cipher_algorithm

const cipher_algorithm *get_cipher_algorithm(const char *);

#define cipher_init(alg, ctx, mode, key, keylen)			\
	(alg)->init((ctx), (mode), (key), (keylen))
#define cipher_update(alg, ctx, in, len, out)				\
	(alg)->update((ctx), (in), (len), (out))
#define cipher_finish(alg, ctx)						\
	(alg)->finish((ctx))

CRYB_END

#endif
