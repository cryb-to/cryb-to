/*
 * Copyright (c) 2014 Dag-Erling Smørgrav
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
#include <stdlib.h>

#include <cryb/mpi.h>

/*
 * Store the difference between the absolute values of A and B in X.
 */
int
mpi_sub_abs(cryb_mpi *X, cryb_mpi *A, cryb_mpi *B)
{
	cryb_mpi *L, *G;
	unsigned int i;
	uint32_t c;

	/*
	 * Trivial cases: A and B are the same or equal or at least one of
	 * them is zero.
	 */
	if (A == B || mpi_eq_abs(A, B)) {
		mpi_zero(X);
		return (0);
	}
	if (A->msb == 0)
		return (X == B ? 0 : mpi_copy(X, B));
	if (B->msb == 0)
		return (X == A ? 0 : mpi_copy(X, A));

	/* we want to subtract the smaller number from the larger */
	if (mpi_cmp_abs(A, B) < 0)
		L = A, G = B;
	else
		L = B, G = A;

	/* make sure X is large enough for the largest possible result */
	if (mpi_grow(X, G->msb) != 0)
		return (-1);

	/* subtract B from A word by word until we run out of B */
	for (c = i = 0; i < (G->msb + 31) / 32; ++i) {
		X->words[i] = G->words[i] - c;
		c = (G->words[i] < c) + (X->words[i] < L->words[i]);
		X->words[i] -= L->words[i];
	}
	/* keep propagating carry */
	while (c) {
		X->words[i] = G->words[i] - c;
		c = (G->words[i] > c);
		++i;
	}
	while (i > 0 && X->words[i] == 0)
		--i;
	/* compute msb of msw */
	/* XXX use flsl() */
	for (X->msb = 31; X->msb > 0; --X->msb)
		if (X->words[i] & (1 << X->msb))
			break;
	/* add msw offset */
	X->msb += i * 32 + 1;
	return (0);
}
