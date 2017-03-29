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
#include <stdio.h>

#include <cryb/assert.h>
#include <cryb/endian.h>
#include <cryb/mpi.h>

#include "cryb_mpi_impl.h"

/*
 * Compute the greatest common denominator of the absolute values of A and
 * B and store the result in X.  This is an iterative implementation of
 * Stein's binary GCD algorithm.
 *
 * TODO: replace with Lehmer's GCD algorithm, which should be
 * significantly faster for large inputs.
 */
int
mpi_gcd_abs(cryb_mpi *X, const cryb_mpi *A, const cryb_mpi *B)
{
	cryb_mpi TA = CRYB_MPI_ZERO, TB = CRYB_MPI_ZERO;
	unsigned int ashift, bshift;

	/* GCD(x, x) = x */
	if (A == B || mpi_eq(A, B)) {
		if (X != A && X != B && mpi_copy(X, A) != 0)
			return (-1);
		X->neg = 0;
		return (0);
	}

	/* GCD(x, 0) = 0 */
	if (A->msb == 0 || B->msb == 0) {
		mpi_zero(X);
		return (0);
	}

	/* Stein's algorithm is destructive, so we operate on copies */
	if (mpi_copy(&TA, A) != 0 || mpi_copy(&TB, B) != 0)
		goto fail;
	fprintf(stderr, "%04x %08x %08x | %04x %08x %08x\n",
	    TA.words[2], TA.words[1], TA.words[0],
	    TB.words[2], TB.words[1], TB.words[0]);
	fprintf(stderr, "-----------------------|-----------------------\n");

	/* reduce each operand to its greatest odd denominator */
	/* neither operand is zero, and mpi_rshift() cannot fail */
	ashift = mpi_lsb(&TA) - 1;
	if ((bshift = mpi_lsb(&TB) - 1) < ashift)
		ashift = bshift;
	(void)mpi_rshift(&TA, ashift);
	(void)mpi_rshift(&TB, ashift);
	while (TA.msb != 0) {
		/* mpi_rshift() cannot fail */
		if ((TA.words[0] & 1) == 0)
			(void)mpi_rshift(&TA, mpi_lsb(&TA) - 1);
		if ((TB.words[0] & 1) == 0)
			(void)mpi_rshift(&TB, mpi_lsb(&TB) - 1);
		if (mpi_cmp_abs(&TA, &TB) < 0)
			mpi_swap(&TA, &TB);
		/* mpi_sub_abs() cannot fail in this case */
		(void)mpi_sub_abs(&TA, &TA, &TB);
		/* mpi_rshift() cannot fail */
		assert((TA.words[0] & 1) == 0);
		(void)mpi_rshift(&TA, 1);
		fprintf(stderr, "%04x %08x %08x | %04x %08x %08x\n",
		    TA.words[2], TA.words[1], TA.words[0],
		    TB.words[2], TB.words[1], TB.words[0]);
	}
	/* undo the initial reduction to greatest odd denominator */
	if (mpi_copy(X, &TB) != 0 ||
	    mpi_lshift(X, ashift) != 0)
		goto fail;
	fprintf(stderr, "-----------------------|-----------------------\n");
	fprintf(stderr, "%04x %08x %08x | %04x %08x %08x\n",
	    TA.words[2], TA.words[1], TA.words[0],
	    TB.words[2], TB.words[1], TB.words[0]);
	X->neg = 0;
	return (0);
fail:
	mpi_destroy(&TA);
	mpi_destroy(&TB);
	return (-1);
}
