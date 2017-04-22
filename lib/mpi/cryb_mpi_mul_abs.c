/*
 * Copyright (c) 2017 Dag-Erling Smørgrav
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
 * Store the product of the absolutes values of A and B in X.
 *
 * Note: this is loosely based on HAC 14.2.3.  However, the first
 * paragraph speaks of x and y having n and t digits respectively, with
 * the most significant digits being x[n - 1] and y[t - 1], but the rest
 * of the section refers to x and y having n + 1 and t + 1 digits.  We
 * follow the former convention, but use a and b instead of n and t to
 * distance ourselves from this confusing text.
 */
int
mpi_mul_abs(cryb_mpi *X, const cryb_mpi *A, const cryb_mpi *B)
{
	cryb_mpi *P, T = CRYB_MPI_ZERO;
	uint64_t p;
	unsigned int a, b, i, j;
	uint32_t c;

	/*
	 * Trivial cases: A and / or B is zero.
	 */
	if (A->msb == 0 || B->msb == 0) {
		mpi_zero(X);
		return (0);
	}

	/*
	 * Unlike addition or subtraction, we can't multiply in place, so
	 * if the destination is identical with one of the operands, we
	 * will have to use temporary storage.  Either way, make sure we
	 * have enough room for the product.
	 */
	P = (X == A || X == B) ? &T : X;
	mpi_zero(P);
	a = MPI_MSW(A);
	b = MPI_MSW(B);
	if (mpi_grow(P, (a + b) * 32) != 0)
		return (-1);

	/*
	 * Multiply...
	 */
	for (i = 0; i < b; ++i) {
		for (c = j = 0; j < a; ++j) {
			p = 1ULL * A->words[j] * B->words[i] + c;
			P->words[i + j] += p & 0xffffffffUL;
			c = p >> 32;
		}
		P->words[i + j] = c;
	}
	if (P->words[i = a + b - 1] == 0)
		--i;
	P->msb = i * 32 + flsl(P->words[i]);

	/*
	 * If we used temporary storage, copy back and clean up.
	 */
	if (P == &T) {
		cryb_mpi_swap(X, P);
		cryb_mpi_destroy(P);
	}

	return (0);
}
