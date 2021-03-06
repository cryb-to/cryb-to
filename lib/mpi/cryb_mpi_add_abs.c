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
 * Store the sum of the absolutes values of A and B in X.
 */
int
mpi_add_abs(cryb_mpi *X, const cryb_mpi *A, const cryb_mpi *B)
{
	unsigned int i;
	uint32_t c;

	/*
	 * Trivial cases: A and B are the same or equal or at least one of
	 * them is zero.
	 */
	if (A == B || mpi_eq_abs(A, B)) {
		if (X != A && X != B && mpi_copy(X, A) != 0)
			return (-1);
		if (mpi_lshift(X, 1) != 0)
			return (-1);
		X->neg = 0;
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

	/*
	 * Normalize our operands: if X is identical to either A or B, we
	 * want it to be A.  Otherwise, copy A into X.  In either case,
	 * make sure X is large enough for the largest possible result.
	 */
	if (X == B) {
		B = A;
		A = X;
	}
	if (mpi_grow(X, A->msb + 1) != 0 || mpi_grow(X, B->msb + 1) != 0)
		return (-1);
	if (X != A)
		/* this cannot fail */
		mpi_copy(X, A);

	/*
	 * From now on, we are adding B to X and A is irrelevant.
	 */

	/* add B into X word by word until we run out of B */
	for (c = i = 0; i < MPI_MSW(B); ++i) {
		X->words[i] += c;
		c = (X->words[i] < c);
		X->words[i] += B->words[i];
		c += (X->words[i] < B->words[i]);
	}
	/* keep propagating carry */
	while (c) {
		X->words[i] += c;
		c = (X->words[i] < c);
		++i;
	}
	if (X->words[i] == 0)
		--i;
	X->msb = i * 32 + flsl(X->words[i]);
	X->neg = 0;
	return (0);
}
