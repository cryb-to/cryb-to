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

#ifndef T_MPI_H_INCLUDED
#define T_MPI_H_INCLUDED


/***************************************************************************
 * Useful constants
 */

static const cryb_mpi z;

#define SMALL_V_SIZE (size_t)64
#define SMALL_E_SIZE (size_t)16
static uint8_t small_v[SMALL_V_SIZE];
static uint32_t small_e[SMALL_E_SIZE];

#define LARGE_V_SIZE (size_t)256
#define LARGE_E_SIZE (size_t)64
static uint8_t large_v[LARGE_V_SIZE];
static uint32_t large_e[LARGE_E_SIZE];


/***************************************************************************
 * Utilities
 */

static void t_printv_mpi(const cryb_mpi *) CRYB_UNUSED;

/*
 * Print an MPI in semi-human-readable form
 */
static void
t_printv_mpi(const cryb_mpi *x)
{

	t_printv("%c", x->neg ? '-' : '+');
	t_printv("%08x", x->msb == 0 ? 0 : x->words[0]);
	for (unsigned int i = 1; i < (x->msb + 31) / 32; ++i)
		t_printv(" %08x", x->words[i]);
}


/***************************************************************************
 * Commonly used predicates
 */

static int t_mpi_not_grown(const cryb_mpi *) CRYB_UNUSED;
static int t_mpi_grown(const cryb_mpi *) CRYB_UNUSED;
static int t_mpi_is_zero(const cryb_mpi *) CRYB_UNUSED;
static int t_compare_mpi(const cryb_mpi *, const cryb_mpi *) CRYB_UNUSED;
static int t_compare_mpi_u32(uint32_t, const cryb_mpi *) CRYB_UNUSED;

/*
 * Verify that an MPI has never grown.
 */
static int
t_mpi_not_grown(const cryb_mpi *x)
{
	int ret = 1;

	ret &= t_compare_ptr(x->swords, x->words);
	ret &= t_compare_sz(CRYB_MPI_SWORDS, x->size);
	return (ret);
}

/*
 * Verify that an MPI has grown.
 */
static int
t_mpi_grown(const cryb_mpi *x)
{
	int ret = 1;

	/* XXX we need inequality predicates */
	if (x->words == x->swords) {
		t_printv("value was expected to change");
		ret &= 0;
	}
	if (x->size == CRYB_MPI_SWORDS) {
		t_printv("value was expected to change");
		ret &= 0;
	}
	return (ret);
}

/*
 * Verify that an MPI is zero.
 */
static int
t_mpi_is_zero(const cryb_mpi *x)
{
	int ret = 1;

	ret &= t_mpi_not_grown(x);
	ret &= t_compare_mem(t_zero, x->words, CRYB_MPI_SWORDS);
	ret &= t_compare_u(0, x->msb);
	ret &= t_compare_i(0, x->neg);
	return (ret);
}

/*
 * Verify that an MPI has the expected value
 */
static int
t_compare_mpi(const cryb_mpi *e, const cryb_mpi *x)
{
	int ret = 1;

	if (e == x)
		return (1);
	if (e == NULL || x == NULL)
		return (0);
	if (x->words == NULL) {
		t_printv("uninitialized MPI\n");
		return (0);
	}
	if (e->neg != x->neg || e->msb != x->msb ||
	    memcmp(e->words, x->words, (e->msb + 7) / 8) != 0) {
		t_printv("expected ");
		t_printv_mpi(e);
		t_printv("\n");
		t_printv("received ");
		t_printv_mpi(x);
		t_printv("\n");
		ret = 0;
	}
	return (ret);
}

/*
 * Verify that an MPI has the expected value
 */
static int
t_compare_mpi_u32(uint32_t e, const cryb_mpi *x)
{
	int ret = 1;

	if (x == NULL)
		return (0);
	if (x->words == NULL) {
		t_printv("uninitialized MPI\n");
		return (0);
	}
	if (x->neg || x->msb > 32 || x->words[0] != e) {
		t_printv("expected +%08x\n", e);
		t_printv("received ");
		t_printv_mpi(x);
		t_printv("\n");
		ret = 0;
	}
	return (ret);
}


/***************************************************************************
 * Boilerplate
 */

static void t_mpi_prepare(void);
static void t_mpi_cleanup(void);

/*
 * Initialize constants used in multiple test cases
 */
static void
t_mpi_prepare(void)
{
	unsigned int i;

	t_assert(sizeof small_e == sizeof small_v);
	t_assert(sizeof small_e == sizeof z.swords);
	for (i = 0; i < SMALL_V_SIZE; ++i)
		small_v[i] = ~i;
	for (i = 0; i < SMALL_E_SIZE; ++i)
		small_e[SMALL_E_SIZE - 1 - i] = be32dec(small_v + i * 4);

	t_assert(sizeof large_e == sizeof large_v);
	t_assert(sizeof large_e > sizeof z.swords);
	for (i = 0; i < LARGE_V_SIZE; ++i)
		large_v[i] = ~i;
	for (i = 0; i < LARGE_E_SIZE; ++i)
		large_e[LARGE_E_SIZE - 1 - i] = be32dec(large_v + i * 4);
}

/*
 * Cleanup and sanity checks
 */
static void
t_mpi_cleanup(void)
{

	t_assert(memcmp(&z, t_zero, sizeof z) == 0);
}


#endif
