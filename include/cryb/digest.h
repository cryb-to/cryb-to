/*-
 * Copyright (c) 2012 Universitetet i Oslo
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
 * $Id$
 */

#ifndef CRYB_DIGEST_H_INCLUDED
#define CRYB_DIGEST_H_INCLUDED

typedef void *(*digest_init_func)(void);
typedef void (*digest_update_func)(void *, const void *, size_t);
typedef void (*digest_final_func)(void *, void *);
typedef int (*digest_complete_func)(const void *, size_t, void *);

struct digest_algorithm {
	const char		*name;		/* algorithm name */
	size_t			 ctxsize;	/* size of context structure */
	size_t			 resultlen;	/* length of the result */
	digest_init_func	 init;
	digest_update_func	 update;
	digest_final_func	 final;
	digest_complete_func	 complete;
};

const struct digest_algorithm *cryb_digest_algorithm(const char *);

void *cryb_digest_init(const char *);
void cryb_digest_update(void *, const void *, size_t);
void cryb_digest_final(void *, void *);
int cryb_digest_complete(const char *, const void *, size_t, void *);

static inline void *
digest_init(const char *alg)
{
	return (cryb_digest_init(alg));
}

static inline void
digest_update(void *ctx, const void *msg, size_t msglen)
{
	cryb_digest_update(ctx, msg, msglen);
}

static inline void
digest_final(void *ctx, void *md)
{
	cryb_digest_final(ctx, md);
}

static inline int
digest_complete(const char *alg, const void *msg, size_t msglen, void *md)
{
	return (cryb_digest_complete(alg, msg, msglen, md));
}

#endif
