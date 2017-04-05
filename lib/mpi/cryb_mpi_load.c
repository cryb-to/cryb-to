/*
 * Copyright (c) 2014-2016 Dag-Erling Smørgrav
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

#include "cryb/impl.h"

#include <stddef.h>
#include <stdint.h>
#include <strings.h>

#include <cryb/bitwise.h>
#include <cryb/endian.h>
#include <cryb/mpi.h>

#include "cryb_mpi_impl.h"

/*
 * Load value from an array of bytes
 */
int
mpi_load(cryb_mpi *X, const uint8_t *a, size_t len)
{
	int i;

	mpi_zero(X);
	/* skip zeroes */
	while (len > 0 && *a == 0)
		--len, ++a;
	if (len == 0)
		return (0);
	/* make room */
	if (mpi_grow(X, len * 8) != 0)
		return (-1);
	/* load whole words */
	for (i = 0; len >= 4; ++i, len -= 4)
		X->words[i] = be32dec(&a[len - 4]);
	/* load remaining bytes */
	switch (len) {
	case 3:
		X->words[i] |= a[len - 3] << 16;
	case 2:
		X->words[i] |= a[len - 2] << 8;
	case 1:
		X->words[i] |= a[len - 1];
		break;
	case 0:
		--i;
	CRYB_NO_DEFAULT_CASE;
	}
	X->msb = i * 32 + flsl(X->words[i]);
	return (0);
}
