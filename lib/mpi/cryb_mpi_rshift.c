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

#include <stdint.h>
#include <string.h>

#include <cryb/mpi.h>

#include "cryb_mpi_impl.h"

/*
 * Right-shift X by c bytes
 */
int
mpi_rshift(cryb_mpi *X, unsigned int c)
{
	unsigned int cl, i;

	/* operands are zero */
	if (c == 0 || X->msb == 0)
		return (0);
	/* shift wider than number, result is zero */
	if (X->msb <= c) {
		mpi_zero(X);
		return (0);
	}
	/* preemptively adjust msb */
	X->msb -= c;
	/* try to move whole words first */
	if (c >= 32) {
		cl = c / 32;
		c = c % 32;
		memmove(X->words, X->words + cl,
		    (X->size - cl) * sizeof *X->words);
		memset(X->words + X->size - cl, 0, cl * sizeof *X->words);
	}
	/* done? */
	if (c == 0)
		return (0);
	/* the rest has to be done the hard way */
	for (i = 0; i < X->size - 1; ++i) {
		X->words[i] >>= c;
		X->words[i] |= X->words[i + 1] << (32 - c);
	}
	X->words[X->size - 1] >>= c;
	/* done! */
	return (0);
}
