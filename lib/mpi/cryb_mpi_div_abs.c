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

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <strings.h>

#include <cryb/bitwise.h>
#include <cryb/mpi.h>

#include "cryb_mpi_impl.h"

/*
 * Store the quotient and remainder of A divided by B in Q and R.
 *
 * Assumes Q != R, but any combination of {Q,R} == {A,B} is fine.  Either
 * Q or R or both may be NULL, althought the latter is pointless.
 */
int
mpi_div_abs(cryb_mpi *Q, cryb_mpi *R, const cryb_mpi *A, const cryb_mpi *B)
{
	cryb_mpi AA = CRYB_MPI_ZERO, QQ = CRYB_MPI_ZERO;
	int cmp;

	/* trivial cases */
	if (B->msb == 0) {
		/* divide something by zero */
		errno = EINVAL;
		return (-1);
	} else if (A->msb == 0) {
		/* divide zero by something */
		if (Q != NULL)
			mpi_zero(Q);
		if (R != NULL)
			mpi_zero(R);
		return (0);
	} else if (B->msb == 1) {
		/* divide something by one */
		if (Q != A && Q != NULL && mpi_copy(Q, A) != 0)
			return (-1);
		if (R != NULL)
			mpi_zero(R);
		return (0);
	} else if ((cmp = mpi_cmp_abs(A, B)) == 0) {
		/* divide something by itself */
		if (Q != NULL)
			mpi_set(Q, 1);
		if (R != NULL)
			mpi_zero(R);
		return (0);
	} else if (cmp < 0) {
		/* divide something by something larger */
		if (Q != NULL)
			mpi_zero(Q);
		if (R != A && R != NULL && mpi_copy(R, A) != 0)
			return (-1);
		return (0);
	}

	/* division is destructive, so we work on copies */
	if (mpi_copy(&AA, A) != 0)
		return (-1);
	mpi_zero(&QQ);

	/*
	 * Repeatedly subtract B from A until they are the same length,
	 * then one last time if A is still greater than B.
	 */
	while (AA.msb > B->msb) {
		mpi_sub_abs(&AA, &AA, B);
		mpi_inc_abs(&QQ);
	}
	if (mpi_cmp_abs(&AA, B) >= 0) {
		mpi_sub_abs(&AA, &AA, B);
		mpi_inc_abs(&QQ);
	}

	/* store result where requested and clean up */
	if (Q != NULL)
		mpi_swap(Q, &QQ);
	if (R != NULL)
		mpi_swap(R, &AA);
	mpi_destroy(&AA);
	mpi_destroy(&QQ);
	return (0);
}
