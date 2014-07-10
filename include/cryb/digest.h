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
 * $Cryb$
 */

#ifndef CRYB_DIGEST_H_INCLUDED
#define CRYB_DIGEST_H_INCLUDED

#define digest_ctx			cryb_digest_ctx
#define digest_init_func		cryb_digest_init_func
#define digest_update_func		cryb_digest_update_func
#define digest_final_func		cryb_digest_final_func
#define digest_complete_func		cryb_digest_complete_func
#define digest_algorithm		cryb_digest_algorithm

typedef void digest_ctx;
typedef void (*digest_init_func)(digest_ctx *);
typedef void (*digest_update_func)(digest_ctx *, const void *, size_t);
typedef void (*digest_final_func)(digest_ctx *, void *);
typedef int (*digest_complete_func)(const void *, size_t, void *);

typedef struct {
	const char		*name;		/* algorithm name */
	size_t			 contextlen;	/* size of context structure */
	size_t			 blocklen;	/* block length */
	size_t			 digestlen;	/* length of the digest */
	digest_init_func	 init;		/* initialization method */
	digest_update_func	 update;	/* update method */
	digest_final_func	 final;		/* finalization method */
	digest_complete_func	 complete;	/* one-shot method */
} digest_algorithm;

#define get_digest_algorithm		cryb_get_digest_algorithm

const digest_algorithm *get_digest_algorithm(const char *);

#define digest_init(alg, ctx)						\
	(alg)->init((ctx))
#define digest_update(alg, ctx, buf, len)				\
	(alg)->update((ctx), (buf), (len))
#define digest_final(alg, ctx, md)					\
	(alg)->final((ctx), (md))
#define digest_complete(alg, buf, len, md)				\
	(alg)->update((buf), (len), (md))

#endif
