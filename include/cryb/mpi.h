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

#ifndef CRYB_MPI_H_INCLUDED
#define CRYB_MPI_H_INCLUDED

#define CRYB_MPI_SWORDS 16

typedef struct cryb_mpi {
	uint32_t	*words;		/* pointer to words */
	size_t		 size;		/* number of words */
	unsigned int	 msb;		/* most significant bit */
	unsigned int	 neg:1;		/* negative */
	uint32_t	 swords[CRYB_MPI_SWORDS];
} cryb_mpi;

/* all-zeroes MPI - not the same as a zero-value MPI! */
#define CRYB_MPI_ZERO	{ 0, 0, 0, 0, { 0 } }

void mpi_init(cryb_mpi *);
void mpi_destroy(cryb_mpi *);
int mpi_grow(cryb_mpi *, unsigned int);
void mpi_zero(cryb_mpi *);
void mpi_negate(cryb_mpi *);
int mpi_copy(cryb_mpi *, const cryb_mpi *);
void mpi_swap(cryb_mpi *, cryb_mpi *);
int mpi_load(cryb_mpi *, const uint8_t *, size_t);
int mpi_set(cryb_mpi *, int32_t);
int mpi_lshift(cryb_mpi *, unsigned int);
int mpi_rshift(cryb_mpi *, unsigned int);
int mpi_add_abs(cryb_mpi *, cryb_mpi *, cryb_mpi *);
int mpi_sub_abs(cryb_mpi *, cryb_mpi *, cryb_mpi *);
int mpi_cmp_abs(cryb_mpi *, cryb_mpi *);
int mpi_cmp(cryb_mpi *, cryb_mpi *);

#endif
