/*-
 * Copyright (c) 2015-2018 The University of Oslo
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include <cryb/coverage.h>
#include <cryb/memcpy_s.h>
#include <cryb/memset_s.h>

/*
 * Like memcpy(), but checks for overflow and overwrites the destination
 * range with zeroes on error.
 */
int
cryb_memcpy_s(void *d, rsize_t dsz, const void *s, rsize_t n)
{
	unsigned int i;

	/* unrecoverable errors */
	if (d == NULL)
		return (EINVAL);
CRYB_DISABLE_COVERAGE
	if (dsz > RSIZE_MAX)
		return (ERANGE);
CRYB_RESTORE_COVERAGE
	/* recoverable errors */
	if (s == NULL || n > dsz ||
CRYB_DISABLE_COVERAGE
	    n > RSIZE_MAX ||
CRYB_RESTORE_COVERAGE
	    (s >= d && s < d + dsz) || (d >= s && d < s + n)) {
		memset_s(d, dsz, 0, dsz);
		if (n > dsz)
			return (EOVERFLOW);
CRYB_DISABLE_COVERAGE
		if (n > RSIZE_MAX)
			return (ERANGE);
CRYB_RESTORE_COVERAGE
		return (EINVAL);
	}
	for (i = 0; i < dsz && i < n; ++i)
		((volatile uint8_t *)d)[i] = ((const uint8_t *)s)[i];
	return (0);
}
