/*
 * Copyright (c) 2014-2017 Dag-Erling Smørgrav
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
#include <cryb/mpi.h>

#include "cryb_mpi_impl.h"

/*
 * Store the absolute value of the difference of the absolute values of A
 * and B in X.
 */
int
mpi_sub_abs(cryb_mpi *X, const cryb_mpi *A, const cryb_mpi *B)
{
	const cryb_mpi *L, *G;
	unsigned int i;
	uint32_t c, cn;

	/*
	 * Trivial cases: A and B are the same or equal or at least one of
	 * them is zero.
	 */
	if (A == B || mpi_eq_abs(A, B)) {
		mpi_zero(X);
		return (0);
	}
	if (A->msb == 0) {
		if (X != B && mpi_copy(X, B) != 0)
			return (-1);
		X->neg = 0;
		return (0);
	}
	if (B->msb == 0) {
		if (X != A && mpi_copy(X, A) != 0)
			return (-1);
		X->neg = 0;
		return (0);
	}

	/* we want to subtract the smaller number from the larger */
	if (mpi_cmp_abs(A, B) < 0)
		L = A, G = B;
	else
		L = B, G = A;

	/* make sure X is large enough for the largest possible result */
	if (mpi_grow(X, G->msb) != 0)
		return (-1);

	/* subtract L from G word by word until we run out of L */
	for (c = i = 0; i < MPI_MSW(L); ++i) {
		cn = G->words[i] < c ||
		    G->words[i] - c < L->words[i];
		X->words[i] = G->words[i] - L->words[i] - c;
		c = cn;
	}
	/* keep propagating carry */
	while (c) {
		cn = (G->words[i] < c);
		X->words[i] = G->words[i] - c;
		c = cn;
		++i;
	}
	while (X->words[i] == 0)
		--i;
	X->msb = i * 32 + flsl(X->words[i]);
	X->neg = 0;
	return (0);
}
