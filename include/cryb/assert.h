/*-
 * Copyright (c) 2016 Dag-Erling Smørgrav
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

#ifndef CRYB_ASSERT_H_INCLUDED
#define CRYB_ASSERT_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#ifndef CRYB_COVERAGE
#include <cryb/coverage.h>
#endif

#define assertion_failed		cryb_assertion_failed

void assertion_failed(const char *, const char *, unsigned int,
    const char *, ...) CRYB_NORETURN;

#ifndef CRYB_NDEBUG

#define assertf(exp, ...)						\
	do {								\
		CRYB_DISABLE_COVERAGE					\
		if (!(exp))						\
			assertion_failed(__func__, __FILE__,		\
			    __LINE__, __VA_ARGS__);			\
		CRYB_RESTORE_COVERAGE					\
	} while (0)

#define assert(exp)							\
	assertf(exp, "%s", #exp)

#else

#define assertf(exp, ...)						\
	((void)0)
#define assert(exp)							\
	((void)0)

#endif

#endif
