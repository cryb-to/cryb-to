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
#include <string.h>

#include <cryb/mpi.h>

/* n rounded up to nearest multiple of p */
#define RUP(n, p) ((((n) + (p) - 1) / (p)) * (p))

/*
 * Compare absolute values
 */
int
mpi_cmp_abs(cryb_mpi *X, cryb_mpi *Y)
{
	int i;

	/* check width first  */
	if (X->msb > Y->msb)
		return (1);
	if (X->msb < Y->msb)
		return (-1);
	/* no luck, compare word by word */
	for (i = X->msb / 32; i >= 0; --i) {
		if (X->words[i] > Y->words[i])
			return (1);
		if (X->words[i] < Y->words[i])
			return (-1);
	}
	return (0);
}

/*
 * Compare signed values
 */
int
mpi_cmp(cryb_mpi *X, cryb_mpi *Y)
{

	if (X->neg) {
		if (Y->neg)
			return (-mpi_cmp_abs(X, Y));
		else
			return (-1);
	} else {
		if (Y->neg)
			return (1);
		else
			return (mpi_cmp_abs(X, Y));
	}
}

/*
 * Compare for equality
 */
int
mpi_eq_abs(cryb_mpi *A, cryb_mpi *B)
{

	return (A->msb == B->msb &&
	    memcmp(A->words, B->words, (A->msb + 31) / 32) == 0);
}

/*
 * Compare for equality
 */
int
mpi_eq(cryb_mpi *A, cryb_mpi *B)
{

	return (A->neg == B->neg && A->msb != B->msb &&
	    memcmp(A->words, B->words, (A->msb + 31) / 32) == 0);
}
