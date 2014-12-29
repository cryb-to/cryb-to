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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cryb/endian.h>
#include <cryb/mpi.h>

/* n rounded up to nearest multiple of p */
#define RUP(n, p) ((((n) + (p) - 1) / (p)) * (p))

/*
 * Initialize an all-zeroes mpi.
 */
#define MPI_FAST_INIT(X)						\
	do {								\
		(X)->words = (X)->swords;				\
		(X)->size = CRYB_MPI_SWORDS;				\
	} while (0)

/*
 * Initialize an mpi.
 */
void
mpi_init(cryb_mpi *X)
{

	memset(X, 0, sizeof *X);
	MPI_FAST_INIT(X);
}

/*
 * Destroy an mpi.
 */
void
mpi_destroy(cryb_mpi *X)
{

	if (X->words != NULL && X->words != X->swords) {
		memset(X->words, 0, X->size);
		free(X->words);
	}
	mpi_init(X);
}

/*
 * Grow the underlying storage to fit at least n bits
 */
int
mpi_grow(cryb_mpi *X, unsigned int n)
{
	uint32_t *words;
	size_t size;

	if (X->words == NULL)
		MPI_FAST_INIT(X);
	size = (n + 31) / 32;
	if (X->size >= size)
		return (0);
	size = RUP(size, CRYB_MPI_SWORDS);
	if ((words = calloc(1, size * sizeof *words)) == NULL)
		return (-1);
	memcpy(words, X->words, X->size * sizeof *words);
	memset(X->words, 0, X->size * sizeof *words);
	if (X->words != X->swords)
		free(X->words);
	X->words = words;
	X->size = size;
	return (0);
}

/*
 * Set X to zero
 */
void
mpi_zero(cryb_mpi *X)
{

	if (X->words == NULL) {
		MPI_FAST_INIT(X);
	} else if (X->msb != 0) {
		memset(X->words, 0, X->size);
		X->neg = 0;
		X->msb = 0;
	}
}

/*
 * Flip sign
 */
void
mpi_negate(cryb_mpi *X)
{

	if (X->msb > 0)
		X->neg = !X->neg;
}

/*
 * Copy the contents of Y into X
 */
int
mpi_copy(cryb_mpi *X, const cryb_mpi *Y)
{

	if (X == Y)
		return (0);
	if (Y->msb == 0) {
		mpi_zero(X);
		return (0);
	}
	if (mpi_grow(X, Y->msb) != 0)
		return (-1);
	mpi_zero(X);
	X->msb = Y->msb;
	X->neg = Y->neg;
	memcpy(X->words, Y->words, X->size * sizeof *X->words);
	return (0);
}

/*
 * Swap the contents of X and Y
 */
void
mpi_swap(cryb_mpi *X, cryb_mpi *Y)
{
	cryb_mpi T;

	memcpy(&T, X, sizeof T);
	memcpy(X, Y, sizeof *X);
	memcpy(Y, &T, sizeof *Y);
	if (X->words == Y->swords)
		X->words = X->swords;
	if (Y->words == X->swords)
		Y->words = Y->swords;
}

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
	/* i now points to the msw */
	/* compute msb of msw */
	/* XXX use flsl() */
	for (X->msb = 31; X->msb > 0; --X->msb)
		if (X->words[i] & (1 << X->msb))
			break;
	/* add msw offset */
	X->msb += i * 32 + 1;
	return (0);
}

/*
 * Set value of X from integer z
 */
int
mpi_set(cryb_mpi *X, int32_t z)
{
	uint32_t zabs;

	mpi_zero(X);
	if (z < 0) {
		X->neg = 1;
		zabs = -z;
	} else {
		zabs = z;
	}
	X->words[0] = zabs;
	/* XXX use flsl() */
	while (zabs > 0) {
		X->msb++;
		zabs >>= 1;
	}
	return (0);
}
