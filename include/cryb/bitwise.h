/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012-2018 Dag-Erling Smørgrav
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

#ifndef CRYB_BITWISE_H_INCLUDED
#define CRYB_BITWISE_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

CRYB_BEGIN

#define CRYB_ROL_ROR(N)							\
	static inline uint##N##_t rol##N(uint##N##_t i, int n)		\
	{								\
		return (i << (n & (N - 1)) | i >> (-n & (N - 1)));	\
	}								\
	static inline uint##N##_t ror##N(uint##N##_t i, int n)		\
	{								\
		return (i << (-n & (N - 1)) | i >> (n & (N - 1)));	\
	}

CRYB_ROL_ROR(8);
CRYB_ROL_ROR(16);
CRYB_ROL_ROR(32);
CRYB_ROL_ROR(64);

#undef CRYB_ROL_ROR

#if !HAVE_FFS
#define ffs	cryb_ffs
#endif
#if !HAVE_FFSL
#define ffsl	cryb_ffsl
#endif
#if !HAVE_FFSLL
#define ffsll	cryb_ffsll
#endif
#if !HAVE_FLS
#define fls	cryb_fls
#endif
#if !HAVE_FLSL
#define flsl	cryb_flsl
#endif
#if !HAVE_FLSLL
#define flsll	cryb_flsll
#endif

static inline int cryb_ffs(int n) {
#if HAVE___BUILTIN_FFS
	return (__builtin_ffs(n));
#elif HAVE___BUILTIN_CTZ
	return (n ? __builtin_ctz(n) : 0);
#else
	unsigned int m;
	unsigned int i;
	if (n == 0)
		return (0);
	for (i = m = 1; i <= sizeof n * 8; ++i, m <<= 1)
		if ((unsigned int)n & m)
			break;
	return (i);
#endif
}

static inline int cryb_ffsl(long int n) {
#if HAVE___BUILTIN_FFSL
	return (__builtin_ffsl(n));
#elif HAVE___BUILTIN_CLZ
	return (n ? __builtin_ctz(n) : 0);
#else
	unsigned long int m;
	unsigned int i;
	if (n == 0)
		return (0);
	for (i = m = 1; i <= sizeof n * 8; ++i, m <<= 1)
		if ((unsigned long int)n & m)
			break;
	return (i);
#endif
}

static inline int cryb_ffsll(long long int n) {
#if HAVE___BUILTIN_FFSLL
	return (__builtin_ffsll(n));
#elif HAVE___BUILTIN_CLZ
	return (n ? __builtin_ctz(n) : 0);
#else
	unsigned long long int m;
	unsigned int i;
	if (n == 0)
		return (0);
	for (i = m = 1; i <= sizeof n * 8; ++i, m <<= 1)
		if ((unsigned long long int)n & m)
			break;
	return (i);
#endif
}

static inline int cryb_fls(int n) {
#if HAVE___BUILTIN_FLS
	return (__builtin_fls(n));
#elif HAVE___BUILTIN_CLZ
	return (n ? (8 * sizeof n) - __builtin_clz(n) : 0);
#else
	unsigned int m;
	unsigned int i;
	if (n == 0)
		return (0);
	for (i = sizeof n * 8, m = 1U << (i - 1); i > 0; --i, m >>= 1)
		if ((unsigned int)n & m)
			break;
	return (i);
#endif
}

static inline int cryb_flsl(long int n) {
#if HAVE___BUILTIN_FLSL
	return (__builtin_flsl(n));
#elif HAVE___BUILTIN_CLZ
	return (n ? (8 * sizeof n) - __builtin_clzl(n) : 0);
#else
	unsigned long int m;
	unsigned int i;
	if (n == 0)
		return (0);
	for (i = sizeof n * 8, m = 1UL << (i - 1); i > 0; --i, m >>= 1)
		if ((unsigned long int)n & m)
			break;
	return (i);
#endif
}

static inline int cryb_flsll(long long int n) {
#if HAVE___BUILTIN_FLSLL
	return (__builtin_flsll(n));
#elif HAVE___BUILTIN_CLZ
	return (n ? (8 * sizeof n) - __builtin_clzll(n) : 0);
#else
	unsigned long long int m;
	unsigned int i;
	if (n == 0)
		return (0);
	for (i = sizeof n * 8, m = 1ULL << (i - 1); i > 0; --i, m >>= 1)
		if ((unsigned long long int)n & m)
			break;
	return (i);
#endif
}

CRYB_END

#endif
