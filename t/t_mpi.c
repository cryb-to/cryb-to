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

#include <sys/types.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <cryb/endian.h>
#include <cryb/mpi.h>

#include <cryb/test.h>

static int
t_mpi_version(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{

	return (t_compare_str(PACKAGE_VERSION, cryb_mpi_version()));
}


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
 * Commonly used predicates
 */

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
t_mpi_grown(cryb_mpi *x)
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


/***************************************************************************
 * Miscellaneous cases
 */

/*
 * Test MPI initialization.
 */
static int
t_mpi_init(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x;

	memset(&x, 0xff, sizeof x);
	mpi_init(&x);
	return (t_mpi_is_zero(&x));
}

/*
 * Very basic value-setting test, zero.  More complex cases below.
 */
static int
t_mpi_set_zero(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x;

	mpi_init(&x);
	mpi_set(&x, 0);
	return (t_mpi_is_zero(&x));
}

/*
 * As above, but use the "fast init" logic whereby an all-zeroes MPI is
 * automatically converted to a valid MPI representing the value zero.
 */
static int
t_mpi_fast_init(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;

	mpi_set(&x, 0);
	return (t_mpi_is_zero(&x));
}

/*
 * Test successful MPI growth.
 */
static int
t_mpi_grow_ok(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&x, 1);
	t_assert(x.words[0] == 1 && x.msb == 1);
	ret &= t_compare_i(0, mpi_grow(&x, CRYB_MPI_SWORDS * 32 + 1));
	t_assert(x.words[0] == 1 && x.msb == 1);
	ret &= t_mpi_grown(&x);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Test failed MPI growth.
 */
static int
t_mpi_grow_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_grow(&x, CRYB_MPI_SWORDS * 32 + 1));
	--t_malloc_fail;
	ret &= t_mpi_not_grown(&x);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Double successful MPI growth.
 */
static int
t_mpi_grow_twice(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	uint32_t *p;
	int ret = 1;

	ret &= t_compare_i(0, mpi_grow(&x, CRYB_MPI_SWORDS * 32 + 1));
	ret &= t_mpi_grown(&x);
	p = x.words;
	ret &= t_compare_i(0, mpi_grow(&x, CRYB_MPI_SWORDS * 32 * 2 + 1));
	/* XXX we need inequality predicates */
	if (x.words == p) {
		t_printv("pointer was expected to change\n");
		ret &= 0;
	}
	ret &= t_mpi_grown(&x);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Test the destruction logic with an uninitialized MPI
 */
static int
t_mpi_destroy_uninit(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;

	mpi_destroy(&x);
	return (t_mpi_is_zero(&x));
}

/*
 * Test the destruction logic with an MPI that hasn't grown
 */
static int
t_mpi_destroy_static(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;

	t_assert(sizeof large_v >= sizeof x.swords);
	mpi_load(&x, large_v, sizeof x.swords);
	t_assert(x.words == x.swords);
	mpi_destroy(&x);
	return (t_mpi_is_zero(&x));
}

/*
 * Test the destruction logic with an MPI that has grown
 */
static int
t_mpi_destroy_grown(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;

	t_assert(sizeof large_v > sizeof x.swords);
	mpi_load(&x, large_v, sizeof large_v);
	t_assert(x.words != x.swords);
	mpi_destroy(&x);
	return (t_mpi_is_zero(&x));
}


/***************************************************************************
 * Assignment, negation, copying, swapping
 */

/*
 * Assign a positive value.
 */
static int
t_mpi_set_positive(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&x, 0x19700101);
	ret &= t_mpi_not_grown(&x);
	ret &= t_compare_x32(0x19700101, x.words[0]);
	ret &= t_compare_u(29, x.msb);
	ret &= t_compare_i(0, x.neg);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Assign a negative value.
 */
static int
t_mpi_set_negative(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&x, -0x19700101);
	ret &= t_mpi_not_grown(&x);
	ret &= t_compare_x32(0x19700101, x.words[0]);
	ret &= t_compare_u(29, x.msb);
	ret &= t_compare_u(1, x.neg);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Negate zero
 */
static int
t_mpi_negate_zero(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_zero(&x);
	t_assert(x.words[0] == 0 && x.msb == 0 && x.neg == 0);
	mpi_negate(&x);
	ret &= t_mpi_is_zero(&x);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Negate non-zero, back and forth
 */
static int
t_mpi_negate_nonzero(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&x, -0x19700101);
	ret &= t_compare_x32(0x19700101, x.words[0]);
	t_assert(x.words[0] == 0x19700101 && x.msb == 29 && x.neg == 1);
	mpi_negate(&x);
	ret &= t_mpi_not_grown(&x);
	ret &= t_compare_x32(0x19700101, x.words[0]);
	ret &= t_compare_u(29, x.msb);
	ret &= t_compare_u(0, x.neg);
	mpi_negate(&x);
	ret &= t_mpi_not_grown(&x);
	ret &= t_compare_x32(0x19700101, x.words[0]);
	ret &= t_compare_u(29, x.msb);
	ret &= t_compare_u(1, x.neg);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Copy an MPI into itself
 */
static int
t_mpi_copy_same(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	/* how do you really test this?  oh well */
	mpi_set(&x, -0x19700101);
	t_assert(x.words[0] == 0x19700101 && x.msb == 29 && x.neg == 1);
	mpi_copy(&x, &x);
	ret &= t_mpi_not_grown(&x);
	ret &= t_compare_x32(0x19700101, x.words[0]);
	ret &= t_compare_u(29, x.msb);
	ret &= t_compare_u(1, x.neg);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Copy a static MPI into another
 */
static int
t_mpi_copy_static(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO, y = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&x, -0x19700101);
	mpi_copy(&y, &x);
	ret &= t_mpi_not_grown(&y);
	ret &= t_compare_mpi(&x, &y);
	mpi_destroy(&x);
	mpi_destroy(&y);
	return (ret);
}

/*
 * Copy an all-zeroes MPI into another
 */
static int
t_mpi_copy_zero(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO, y = CRYB_MPI_ZERO;
	int ret = 1;

	t_assert(x.words == NULL && y.words == NULL);
	ret &= t_compare_i(0, mpi_copy(&y, &x));
	ret &= t_compare_mem(&z, &x, sizeof x);
	ret &= t_mpi_not_grown(&y);
	ret &= t_mpi_is_zero(&y);
	mpi_destroy(&x);
	mpi_destroy(&y);
	return (ret);
}

/*
 * Copy a force-grown MPI
 */
static int
t_mpi_copy_grown(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO, y = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&x, -0x19700101);
	/* the original is larger than necessary */
	mpi_grow(&x, CRYB_MPI_SWORDS * 32 + 1);
	t_assert(x.words != x.swords && x.size > CRYB_MPI_SWORDS);
	mpi_copy(&y, &x);
	/* the copy is just large enough to fit the actual value */
	ret &= t_mpi_not_grown(&y);
	ret &= t_compare_mpi(&x, &y);
	mpi_destroy(&x);
	mpi_destroy(&y);
	return (ret);
}

/*
 * Copy an organically-grown MPI
 */
static int
t_mpi_copy_long(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO, y = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&x, large_v, sizeof large_v);
	t_assert(x.words != x.swords && x.size > CRYB_MPI_SWORDS &&
	    memcmp(x.words, large_e, sizeof large_e) == 0);
	mpi_copy(&y, &x);
	ret &= t_mpi_grown(&y);
	ret &= t_compare_mpi(&x, &y);
	mpi_destroy(&x);
	mpi_destroy(&y);
	return (ret);
}

/*
 * As above, but allocation fails.
 */
static int
t_mpi_copy_long_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO, y = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&x, large_v, sizeof large_v);
	t_assert(t_mpi_grown(&x));
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_copy(&y, &x));
	--t_malloc_fail;
	ret &= t_mpi_is_zero(&y);
	mpi_destroy(&x);
	mpi_destroy(&y);
	return (ret);
}

/*
 * Swap two values (not grown)
 */
static int
t_mpi_swap_static(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x0 = CRYB_MPI_ZERO, y0 = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO, y = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&x, -0x19700101);
	t_assert(x.words[0] == 0x19700101 && x.msb == 29 && x.neg == 1);
	mpi_copy(&x0, &x);
	mpi_set(&y, 0x20140901);
	t_assert(y.words[0] == 0x20140901 && y.msb == 30 && y.neg == 0);
	mpi_copy(&y0, &y);
	mpi_swap(&x, &y);
	ret &= t_compare_mpi(&x0, &y);
	ret &= t_compare_mpi(&y0, &x);
	mpi_destroy(&x);
	mpi_destroy(&x0);
	mpi_destroy(&y);
	mpi_destroy(&y0);
	return (ret);
}

/*
 * Swap two values (grown)
 */
static int
t_mpi_swap_grown(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x0 = CRYB_MPI_ZERO, y0 = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO, y = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&x, -0x19700101);
	mpi_copy(&x0, &x);
	mpi_grow(&x, CRYB_MPI_SWORDS * 32 + 1);
	t_assert(t_mpi_grown(&x) && t_compare_mpi(&x0, &x));
	mpi_set(&y, 0x20140901);
	mpi_copy(&y0, &y);
	mpi_grow(&y, CRYB_MPI_SWORDS * 32 + 1);
	t_assert(t_mpi_grown(&y) && t_compare_mpi(&y0, &y));
	mpi_swap(&x, &y);
	ret &= t_compare_mpi(&x0, &y);
	ret &= t_compare_mpi(&y0, &x);
	mpi_destroy(&x);
	mpi_destroy(&x0);
	mpi_destroy(&y);
	mpi_destroy(&y0);
	return (ret);
}


/***************************************************************************
 * Load / store
 */

static struct t_load_case {
	const char *desc;
	uint8_t v[16];
	size_t vlen;
	uint32_t e[4];
	unsigned int msb;
} t_load_cases[] = {
	{
		"load nothing",
		{ }, 0,
		{ 0x00000000, }, 0,
	},
	{
		"load 0x00",
		{ 0x00, }, 1,
		{ 0x00000000, }, 0,
	},
	{
		"load 0x01",
		{ 0x01, }, 1,
		{ 0x00000001, }, 1,
	},
	{
		"load 0x0102",
		{ 0x01, 0x02, }, 2,
		{ 0x00000102, }, 9,
	},
	{
		"load 0x010203",
		{ 0x01, 0x02, 0x03, }, 3,
		{ 0x00010203, }, 17,
	},
	{
		"load 0x01020304",
		{ 0x01, 0x02, 0x03, 0x04, }, 4,
		{ 0x01020304, }, 25,
	},
	{
		"load 0x0102030405",
		{ 0x01, 0x02, 0x03, 0x04, 0x05, }, 5,
		{ 0x02030405, 0x00000001, }, 33,
	},
	{
		"load 0x010203040506",
		{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, }, 6,
		{ 0x03040506, 0x00000102, }, 41,
	},
	{
		"load 0x01020304050607",
		{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, }, 7,
		{ 0x04050607, 0x00010203, }, 49,
	},
	{
		"load 0x0102030405060708",
		{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, }, 8,
		{ 0x05060708, 0x01020304, }, 57,
	},
};

/*
 * Load a string of bytes, verify result.
 */
static int
t_mpi_load(char **desc CRYB_UNUSED, void *arg)
{
	struct t_load_case *tc = arg;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&x, tc->v, tc->vlen);
	ret &= t_compare_mem(tc->e, x.words, sizeof tc->e);
	ret &= t_compare_u(tc->msb, x.msb);
	mpi_destroy(&x);
	return (ret);
}

/*
 * As above, but with a number that exactly fills the static buffer.
 */
static int
t_mpi_exact_load(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	t_assert(sizeof small_e == sizeof small_v);
	t_assert(sizeof small_v == sizeof x.swords);
	mpi_load(&x, small_v, sizeof x.swords);
	ret &= t_compare_ptr(x.swords, x.words);
	ret &= t_compare_mem(small_e, x.words, sizeof x.swords);
	ret &= t_compare_u(SMALL_V_SIZE * 8, x.msb);
	mpi_destroy(&x);
	return (ret);
}

/*
 * As above, but with a large number to force reallocation.
 */
static int
t_mpi_large_load(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	t_assert(sizeof large_e == sizeof large_v);
	t_assert(sizeof large_v > sizeof x.swords);
	mpi_load(&x, large_v, sizeof large_v);
	/* XXX we need inequality predicates */
	if (x.words == x.swords) {
		t_printv("reallocation failed to occur\n");
		ret &= 0;
	}
	/* XXX we need inequality predicates */
	if (x.size < LARGE_E_SIZE) {
		t_printv("expected at least %zu, received %zu\n",
		    LARGE_E_SIZE, x.size);
		ret &= 0;
	}
	ret &= t_compare_mem(large_e, x.words, sizeof large_e);
	ret &= t_compare_u(LARGE_V_SIZE * 8, x.msb);
	mpi_destroy(&x);
	return (ret);
}

/*
 * As above, but allocation fails.
 */
static int
t_mpi_large_load_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO, y;
	int ret = 1;

	t_assert(sizeof large_e == sizeof large_v);
	t_assert(sizeof large_v > sizeof x.swords);
	mpi_init(&x);
	y = x;
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_load(&x, large_v, sizeof large_v));
	--t_malloc_fail;
	ret &= t_compare_mem(&y, &x, sizeof x);
	mpi_destroy(&x);
	return (ret);
}


/***************************************************************************
 * Comparison
 */

static struct t_cmp_case {
	const char *desc;
	int a, b, cmp;
} t_cmp_cases[] = {
	{ "-3 == -3",	-3,	-3,	 0 },
	{ "-3 < -2",	-3,	-2,	-1 },
	{ "-3 < -1",	-3,	-1,	-1 },
	{ "-3 < 0",	-3,	 0,	-1 },
	{ "-3 < 1",	-3,	 1,	-1 },
	{ "-3 < 2",	-3,	 2,	-1 },
	{ "-3 < 3",	-3,	 3,	-1 },

	{ "-2 > -3",	-2,	-3,	 1 },
	{ "-2 == -2",	-2,	-2,	 0 },
	{ "-2 < -1",	-2,	-1,	-1 },
	{ "-2 < 0",	-2,	 0,	-1 },
	{ "-2 < 1",	-2,	 1,	-1 },
	{ "-2 < 2",	-2,	 2,	-1 },
	{ "-2 < 3",	-2,	 3,	-1 },

	{ "-1 > -3",	-1,	-3,	 1 },
	{ "-1 > -2",	-1,	-2,	 1 },
	{ "-1 == -1",	-1,	-1,	 0 },
	{ "-1 < 0",	-1,	 0,	-1 },
	{ "-1 < 1",	-1,	 1,	-1 },
	{ "-1 < 2",	-1,	 2,	-1 },
	{ "-1 < 3",	-1,	 3,	-1 },

	{ "0 > -3",	 0,	-3,	 1 },
	{ "0 > -2",	 0,	-2,	 1 },
	{ "0 > -1",	 0,	-1,	 1 },
	{ "0 == 0",	 0,	 0,	 0 },
	{ "0 < 1",	 0,	 1,	-1 },
	{ "0 < 2",	 0,	 2,	-1 },
	{ "0 < 3",	 0,	 3,	-1 },

	{ "1 > -3",	 1,	-3,	 1 },
	{ "1 > -2",	 1,	-2,	 1 },
	{ "1 > -1",	 1,	-1,	 1 },
	{ "1 > 0",	 1,	 0,	 1 },
	{ "1 == 1",	 1,	 1,	 0 },
	{ "1 < 2",	 1,	 2,	-1 },
	{ "1 < 3",	 1,	 3,	-1 },

	{ "2 > -3",	 2,	-3,	 1 },
	{ "2 > -2",	 2,	-2,	 1 },
	{ "2 > -1",	 2,	-1,	 1 },
	{ "2 > 0",	 2,	 0,	 1 },
	{ "2 > 1",	 2,	 1,	 1 },
	{ "2 == 2",	 2,	 2,	 0 },
	{ "2 < 3",	 2,	 3,	-1 },

	{ "3 > -3",	 3,	-3,	 1 },
	{ "3 > -2",	 3,	-2,	 1 },
	{ "3 > -1",	 3,	-1,	 1 },
	{ "3 > 0",	 3,	 0,	 1 },
	{ "3 > 1",	 3,	 1,	 1 },
	{ "3 > 2",	 3,	 2,	 1 },
	{ "3 == 3",	 3,	 3,	 0 },
};

/*
 * Compare two MPIs
 */
static int
t_mpi_cmp(char **desc CRYB_UNUSED, void *arg)
{
	struct t_cmp_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, tc->a);
	mpi_set(&b, tc->b);
	ret &= t_compare_i(tc->cmp, mpi_cmp(&a, &b));
	mpi_destroy(&a);
	mpi_destroy(&b);
	return (ret);
}


/***************************************************************************
 * Left / right shift
 */

static struct t_lsh_case {
	const char *desc;
	uint8_t v[16];
	size_t vlen;
	unsigned int n;
	uint8_t e[16];
	size_t elen;
} t_lsh_cases[] = {
	{
		"0x00 << 0 == 0x00",
		{                         0x00, }, 1,
		0,
		{                         0x00, }, 1,
	},
	{
		"0x00 << 1 == 0x00",
		{                         0x00, }, 1,
		1,
		{                         0x00, }, 1,
	},
	{
		"0x01 << 0 == 0x01",
		{                         0x01, }, 1,
		0,
		{                         0x01, }, 1,
	},
	{
		"0x01 << 1 == 0x02",
		{                         0x01, }, 1,
		1,
		{                         0x02, }, 1,
	},
	{
		"0x11 << 1 == 0x22",
		{                         0x11, }, 1,
		1,
		{                         0x22, }, 1,
	},
	{
		"0x11 << 32 == 0x1100000000",
		{       0x00, 0x00, 0x00, 0x11, }, 4,
		32,
		{ 0x11, 0x00, 0x00, 0x00, 0x00, }, 5,
	},
	{
		"0x22 << 31 == 0x1100000000",
		{       0x00, 0x00, 0x00, 0x22, }, 4,
		31,
		{ 0x11, 0x00, 0x00, 0x00, 0x00, }, 5,
	},
	{
		"0x80000000 << 1 == 0x100000000",
		{       0x80, 0x00, 0x00, 0x00, }, 4,
		1,
		{ 0x01, 0x00, 0x00, 0x00, 0x00, }, 5,
	},
};

/*
 * Load a number, left-shift it and verify the result.
 */
static int
t_mpi_lsh(char **desc CRYB_UNUSED, void *arg)
{
	struct t_lsh_case *tc = arg;
	cryb_mpi x = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&x, tc->v, tc->vlen);
	mpi_load(&e, tc->e, tc->elen);
	ret &= t_compare_i(0, mpi_lshift(&x, tc->n));
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&x);
	mpi_destroy(&e);
	return (ret);
}

/*
 * As above, but with an initial number large enough to cause an overflow
 * and reallocation.
 */
static int
t_mpi_large_lsh(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&x, small_v, sizeof small_v);
	t_assert(t_mpi_not_grown(&x));
	ret &= t_compare_i(0, mpi_lshift(&x, 32));
	/* XXX we need inequality predicates */
	if (x.words == x.swords) {
		t_printv("reallocation failed to occur\n");
		return (0);
	}
	ret &= t_compare_mem(t_zero, x.words, sizeof x.words[0]);
	ret &= t_compare_mem(small_e, x.words + 1, sizeof small_e);
	ret &= t_compare_u(SMALL_V_SIZE * 8 + 32, x.msb);
	mpi_destroy(&x);
	return (ret);
}

/*
 * As above, but allocation will fail.
 */
static int
t_mpi_large_lsh_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi x = CRYB_MPI_ZERO, y;
	int ret = 1;

	mpi_load(&x, small_v, sizeof small_v);
	t_assert(t_mpi_not_grown(&x));
	y = x;
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_lshift(&x, 32));
	--t_malloc_fail;
	ret &= t_compare_mem(&y, &x, sizeof x);
	mpi_destroy(&x);
	return (ret);
}

static struct t_rsh_case {
	const char *desc;
	uint8_t v[16];
	size_t vlen;
	unsigned int n;
	uint8_t e[16];
	size_t elen;
} t_rsh_cases[] = {
	{
		"0x00 >> 0 == 0x00",
		{                         0x00, }, 1,
		0,
		{                         0x00, }, 1,
	},
	{
		"0x00 >> 1 == 0x00",
		{                         0x00, }, 1,
		1,
		{                         0x00, }, 1,
	},
	{
		"0x01 >> 0 == 0x01",
		{                         0x01, }, 1,
		0,
		{                         0x01, }, 1,
	},
	{
		"0x01 >> 1 == 0x00",
		{                         0x01, }, 1,
		1,
		{                         0x00, }, 1,
	},
	{
		"0x02 >> 1 == 0x01",
		{                         0x02, }, 1,
		1,
		{                         0x01, }, 1,
	},
	{
		"0x22 >> 1 == 0x11",
		{                         0x22, }, 1,
		1,
		{                         0x11, }, 1,
	},
	{
		"0x1100000000 >> 32 == 0x11",
		{ 0x11, 0x00, 0x00, 0x00, 0x00, }, 5,
		32,
		{       0x00, 0x00, 0x00, 0x11, }, 4,
	},
	{
		"0x1100000000 >> 31 == 0x22",
		{ 0x11, 0x00, 0x00, 0x00, 0x00, }, 5,
		31,
		{       0x00, 0x00, 0x00, 0x22, }, 4,
	},
	{
		"0x100000000 >> 1 == 0x80000000",
		{ 0x01, 0x00, 0x00, 0x00, 0x00, }, 5,
		1,
		{       0x80, 0x00, 0x00, 0x00, }, 4,
	},
};

/*
 * Load a number, right-shift it and verify the result.
 */
static int
t_mpi_rsh(char **desc CRYB_UNUSED, void *arg)
{
	struct t_rsh_case *tc = arg;
	cryb_mpi x = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&x, tc->v, tc->vlen);
	mpi_load(&e, tc->e, tc->elen);
	mpi_rshift(&x, tc->n);
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&x);
	mpi_destroy(&e);
	return (ret);
}


/***************************************************************************
 * Addition
 */

static struct t_add_case {
	const char *desc;
	uint8_t a[16];
	size_t amsb;
	int aneg:1;
	uint8_t b[16];
	size_t bmsb;
	int bneg:1;
	uint8_t e[16];
	size_t emsb;
	int eneg:1;
} t_add_cases[] = {
	{
		"0 + 0 == 0",
		{                                                       },  0, 0,
		{                                                       },  0, 0,
		{                                                       },  0, 0,
	},
	{
		"0 + 1 == 1",
		{                                                       },  0, 0,
		{                                                 0x01, },  1, 0,
		{                                                 0x01, },  1, 0,
	},
	{
		"1 + 0 == 1",
		{                                                 0x01, },  1, 0,
		{                                                       },  0, 0,
		{                                                 0x01, },  1, 0,
	},
	{
		"2 + 2 == 4",
		{                                                 0x02, },  2, 0,
		{                                                 0x02, },  2, 0,
		{                                                 0x04, },  3, 0,
	},
	{
		"|0x20140901| + |0x19700101| == 0x39840a02",
		{                               0x20, 0x14, 0x09, 0x01, }, 30, 0,
		{                               0x19, 0x70, 0x01, 0x01, }, 29, 0,
		{                               0x39, 0x84, 0x0a, 0x02, }, 30, 0,
	},
	{
		"|-0x20140901| + |0x19700101| == 0x39840a02",
		{                               0x20, 0x14, 0x09, 0x01, }, 30, 1,
		{                               0x19, 0x70, 0x01, 0x01, }, 29, 0,
		{                               0x39, 0x84, 0x0a, 0x02, }, 30, 0,
	},
	{
		"|0x20140901| + |-0x19700101| == 0x39840a02",
		{                               0x20, 0x14, 0x09, 0x01, }, 30, 0,
		{                               0x19, 0x70, 0x01, 0x01, }, 29, 1,
		{                               0x39, 0x84, 0x0a, 0x02, }, 30, 0,
	},
	{
		"|-0x20140901| + |-0x19700101| == 0x39840a02",
		{                               0x20, 0x14, 0x09, 0x01, }, 30, 1,
		{                               0x19, 0x70, 0x01, 0x01, }, 29, 1,
		{                               0x39, 0x84, 0x0a, 0x02, }, 30, 0,
	},
	{
		"simple carry, "
		"0xffffffff + 0x01 == 0x0100000000",
		{                               0xff, 0xff, 0xff, 0xff, }, 32, 0,
		{                                                 0x01, },  1, 0,
		{                         0x01, 0x00, 0x00, 0x00, 0x00, }, 33, 0,
	},
	{
		"complex carry, "
		"0x0100000000ffffffff + 0x01 == 0x010000000100000000",
		{ 0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, }, 64, 0,
		{                                                 0x01, },  1, 0,
		{ 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, }, 64, 0,
	},
};

static int
t_mpi_add_tc(char **desc CRYB_UNUSED, void *arg)
{
	struct t_add_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, tc->a, (tc->amsb + 7) / 8);
	a.neg = tc->aneg;
	mpi_load(&b, tc->b, (tc->bmsb + 7) / 8);
	b.neg = tc->bneg;
	mpi_load(&e, tc->e, (tc->emsb + 7) / 8);
	e.neg = tc->eneg;
	ret &= t_compare_i(0, mpi_add_abs(&x, &a, &b));
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}

/*
 * All operands are different
 */
static int
t_mpi_add(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x19700101 + 0x20140901);
	ret &= t_compare_i(0, mpi_add_abs(&x, &a, &b));
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}

/*
 * As above, but first allocation will fail
 */
static int
t_mpi_add_fail1(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, large_v, sizeof large_v);
	mpi_set(&b, 0x19700101);
	mpi_set(&x, 0x20140901);
	mpi_set(&e, 0x20140901);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_add_abs(&x, &a, &b));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}

/*
 * As above, but second allocation will fail
 */
static int
t_mpi_add_fail2(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_load(&b, large_v, sizeof large_v);
	mpi_set(&x, 0x20140901);
	mpi_set(&e, 0x20140901);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_add_abs(&x, &a, &b));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}

/*
 * Target is the first operand
 */
static int
t_mpi_add_b_to_a(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x19700101 + 0x20140901);
	ret &= t_compare_i(0, mpi_add_abs(&a, &a, &b));
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * As above, but allocation will fail
 */
static int
t_mpi_add_b_to_a_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_load(&b, large_v, sizeof large_v);
	mpi_set(&e, 0x19700101);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_add_abs(&a, &a, &b));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is the first operand and both operands are equal
 */
static int
t_mpi_add_b_to_a_equal(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&b, 0x19700101);
	mpi_set(&e, 0x19700101 + 0x19700101);
	ret &= t_compare_i(0, mpi_add_abs(&a, &a, &b));
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is the second operand
 */
static int
t_mpi_add_a_to_b(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x19700101 + 0x20140901);
	ret &= t_compare_i(0, mpi_add_abs(&b, &a, &b));
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * As above, but allocation will fail
 */
static int
t_mpi_add_a_to_b_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, large_v, sizeof large_v);
	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x20140901);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_add_abs(&b, &a, &b));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is the second operand and both operands are equal
 */
static int
t_mpi_add_a_to_b_equal(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&b, 0x19700101);
	mpi_set(&e, 0x19700101 + 0x19700101);
	ret &= t_compare_i(0, mpi_add_abs(&b, &a, &b));
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is both operands
 */
static int
t_mpi_add_a_to_a(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&e, 0x19700101 + 0x19700101);
	ret &= t_compare_i(0, mpi_add_abs(&a, &a, &a));
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&e);
	return (ret);
}

/*
 * As above, but allocation will fail
 */
static int
t_mpi_add_a_to_a_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, large_v, sizeof large_v);
	mpi_load(&e, large_v, sizeof large_v);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_add_abs(&a, &a, &a));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Operands are the same
 */
static int
t_mpi_add_a_and_a(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&e, 0x19700101 + 0x19700101);
	ret &= t_compare_i(0, mpi_add_abs(&b, &a, &a));
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * As above, but allocation will fail
 */
static int
t_mpi_add_a_and_a_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, large_v, sizeof large_v);
	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x20140901);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_add_abs(&b, &a, &a));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
	return (ret);
}

/*
 * Target is the first operand, second operand is zero
 */
static int
t_mpi_add_zero_to_a(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_zero(&b);
	mpi_copy(&e, &a);
	ret &= t_compare_i(0, mpi_add_abs(&a, &a, &b));
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is the second operand, first operand is zero
 */
static int
t_mpi_add_zero_to_b(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_zero(&a);
	mpi_set(&b, 0x20140901);
	mpi_copy(&e, &b);
	ret &= t_compare_i(0, mpi_add_abs(&b, &a, &b));
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}


/***************************************************************************
 * Subtraction
 */

static struct t_sub_case {
	const char *desc;
	uint8_t a[16];
	size_t amsb;
	int aneg:1;
	uint8_t b[16];
	size_t bmsb;
	int bneg:1;
	uint8_t e[16];
	size_t emsb;
	int eneg:1;
} t_sub_cases[] = {
	{
		"0 - 0 == 0",
		{                                                       },  0, 0,
		{                                                       },  0, 0,
		{                                                       },  0, 0,
	},
	{
		"0 - 1 == 1",
		{                                                       },  0, 0,
		{                                                 0x01, },  1, 0,
		{                                                 0x01, },  1, 0,
	},
	{
		"1 - 0 == 1",
		{                                                 0x01, },  1, 0,
		{                                                       },  0, 0,
		{                                                 0x01, },  1, 0,
	},
	{
		"1 - 1 == 0",
		{                                                 0x01, },  1, 0,
		{                                                 0x01, },  1, 0,
		{                                                       },  0, 0,
	},
	{
		"4 - 2 == 2",
		{                                                 0x04, },  3, 0,
		{                                                 0x02, },  2, 0,
		{                                                 0x02, },  2, 0,
	},
	{
		"2 - 4 == 2",
		{                                                 0x02, },  2, 0,
		{                                                 0x04, },  3, 0,
		{                                                 0x02, },  2, 0,
	},
	{
		"| |0x20140901| - |0x19700101| | == 0x6a40800",
		{                               0x20, 0x14, 0x09, 0x01, }, 30, 0,
		{                               0x19, 0x70, 0x01, 0x01, }, 29, 0,
		{                               0x06, 0xa4, 0x08, 0x00, }, 27, 0,
	},
	{
		"| |-0x20140901| - |0x19700101| | == 0x6a40800",
		{                               0x20, 0x14, 0x09, 0x01, }, 30, 1,
		{                               0x19, 0x70, 0x01, 0x01, }, 29, 0,
		{                               0x06, 0xa4, 0x08, 0x00, }, 27, 0,
	},
	{
		"| |0x20140901| - |-0x19700101| | == 0x6a40800",
		{                               0x20, 0x14, 0x09, 0x01, }, 30, 0,
		{                               0x19, 0x70, 0x01, 0x01, }, 29, 1,
		{                               0x06, 0xa4, 0x08, 0x00, }, 27, 0,
	},
	{
		"| |-0x20140901| - |-0x19700101| | == 0x6a40800",
		{                               0x20, 0x14, 0x09, 0x01, }, 30, 1,
		{                               0x19, 0x70, 0x01, 0x01, }, 29, 1,
		{                               0x06, 0xa4, 0x08, 0x00, }, 27, 0,
	},
	{
		"0x120140901 - 0x119700101 == 0x6a40800",
		{                         0x01, 0x20, 0x14, 0x09, 0x01, }, 33, 0,
		{                         0x01, 0x19, 0x70, 0x01, 0x01, }, 33, 0,
		{                               0x06, 0xa4, 0x08, 0x00, }, 27, 0,
	},
	{
		"0x119700101 - 0x120140901 == 0x6a40800",
		{                         0x01, 0x19, 0x70, 0x01, 0x01, }, 33, 0,
		{                         0x01, 0x20, 0x14, 0x09, 0x01, }, 33, 0,
		{                               0x06, 0xa4, 0x08, 0x00, }, 27, 0,
	},
	{
		"simple carry, "
		"0x1000000000 - 0xfffffffff == 0x01",
		{                         0x10, 0x00, 0x00, 0x00, 0x00, }, 37, 0,
		{                         0x0f, 0xff, 0xff, 0xff, 0xff, }, 36, 0,
		{                                                 0x01, },  1, 0,
	},
	{
		"simple carry, "
		"0x1000000000 - 0x01 == 0xfffffffff",
		{                         0x10, 0x00, 0x00, 0x00, 0x00, }, 37, 0,
		{                                                 0x01, },  1, 0,
		{                         0x0f, 0xff, 0xff, 0xff, 0xff, }, 36, 0,
	},
	{
		"complex carry, "
		"0x010000000000000000 - 0xffffffffffffffff == 0x01",
		{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, }, 65, 0,
		{       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, }, 64, 0,
		{                                                 0x01, },  1, 0,
	},
	{
		"complex carry, "
		"0x010000000000000000 - 0x01 == 0xffffffffffffffff",
		{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, }, 65, 0,
		{                                                 0x01, },  1, 0,
		{       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, }, 64, 0,
	},
};

static int
t_mpi_sub_tc(char **desc CRYB_UNUSED, void *arg)
{
	struct t_sub_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, tc->a, (tc->amsb + 7) / 8);
	a.neg = tc->aneg;
	mpi_load(&b, tc->b, (tc->bmsb + 7) / 8);
	b.neg = tc->bneg;
	mpi_load(&e, tc->e, (tc->emsb + 7) / 8);
	e.neg = tc->eneg;
	ret &= t_compare_i(0, mpi_sub_abs(&x, &a, &b));
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}

/*
 * All operands are different
 */
static int
t_mpi_sub(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x20140901 - 0x19700101);
	ret &= t_compare_i(0, mpi_sub_abs(&x, &a, &b));
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}

/*
 * As above, but allocation will fail
 */
static int
t_mpi_sub_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, small_v, sizeof small_v);
	mpi_load(&b, large_v, sizeof large_v);
	mpi_set(&x, 0x20140901);
	mpi_set(&e, 0x20140901);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_sub_abs(&x, &a, &b));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}


/*
 * Target is the first operand
 */
static int
t_mpi_sub_b_from_a(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x20140901);
	mpi_set(&b, 0x19700101);
	mpi_set(&e, 0x20140901 - 0x19700101);
	ret &= t_compare_i(0, mpi_sub_abs(&a, &a, &b));
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * As above, but allocation will fail
 */
static int
t_mpi_sub_b_from_a_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_load(&b, large_v, sizeof large_v);
	mpi_set(&e, 0x19700101);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_sub_abs(&a, &a, &b));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is the second operand
 */
static int
t_mpi_sub_a_from_b(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x20140901 - 0x19700101);
	ret &= t_compare_i(0, mpi_sub_abs(&b, &a, &b));
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * As above, but allocation will fail
 */
static int
t_mpi_sub_a_from_b_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, large_v, sizeof large_v);
	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x20140901);
	++t_malloc_fail;
	ret &= t_compare_i(-1, mpi_sub_abs(&b, &a, &b));
	--t_malloc_fail;
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is both operands
 */
static int
t_mpi_sub_a_from_a(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x19700101);
	mpi_set(&e, 0);
	ret &= t_compare_i(0, mpi_sub_abs(&a, &a, &a));
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is the first operand, second operand is zero
 */
static int
t_mpi_sub_zero_from_a(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x20140901);
	mpi_set(&e, 0x20140901);
	ret &= t_compare_i(0, mpi_sub_abs(&a, &a, &b));
	ret &= t_compare_mpi(&e, &a);
	mpi_destroy(&a);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is the second operand, first operand is zero
 */
static int
t_mpi_sub_b_from_zero(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&b, 0x20140901);
	mpi_set(&e, 0x20140901);
	ret &= t_compare_i(0, mpi_sub_abs(&b, &a, &b));
	ret &= t_compare_mpi(&e, &b);
	mpi_destroy(&b);
	mpi_destroy(&e);
	return (ret);
}

/*
 * Target is negative
 */
static int
t_mpi_sub_neg_target(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x20140901);
	mpi_set(&b, 0x19700101);
	mpi_set(&e, 0x20140901 - 0x19700101);
	mpi_set(&x, -1);
	ret &= t_compare_i(0, mpi_sub_abs(&x, &a, &b));
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}


/***************************************************************************
 * Boilerplate
 */

static int
t_prepare(int argc, char *argv[])
{
	unsigned int i;

	(void)argc;
	(void)argv;
	t_add_test(t_mpi_version, NULL, "version");

	/* initialize constants used in multiple test cases */
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

	/* basic tests */
	t_add_test(t_mpi_init, NULL, "init");
	t_add_test(t_mpi_set_zero, NULL, "set to 0");
	t_add_test(t_mpi_fast_init, NULL, "fast init");
	t_add_test(t_mpi_grow_ok, NULL, "grow (success)");
	t_add_test(t_mpi_grow_fail, NULL, "grow (failure)");
	t_add_test(t_mpi_grow_twice, NULL, "grow (twice)");
	t_add_test(t_mpi_destroy_uninit, NULL, "destroy static");
	t_add_test(t_mpi_destroy_static, NULL, "destroy static");
	t_add_test(t_mpi_destroy_grown, NULL, "destroy grown");

	/* assignment, copying, negation, swapping */
	t_add_test(t_mpi_set_positive, NULL, "set to positive value");
	t_add_test(t_mpi_set_negative, NULL, "set to negative value");
	t_add_test(t_mpi_negate_zero, NULL, "negate zero");
	t_add_test(t_mpi_negate_nonzero, NULL, "negate nonzero");
	t_add_test(t_mpi_copy_same, NULL, "copy (same)");
	t_add_test(t_mpi_copy_static, NULL, "copy (static)");
	t_add_test(t_mpi_copy_zero, NULL, "copy (zero)");
	t_add_test(t_mpi_copy_grown, NULL, "copy (grown)");
	t_add_test(t_mpi_copy_long, NULL, "copy (long)");
	t_add_test(t_mpi_copy_long_fail, NULL, "copy (long) (failure)");
	t_add_test(t_mpi_swap_static, NULL, "swap (static)");
	t_add_test(t_mpi_swap_grown, NULL, "swap (grown)");

	/* load / store */
	for (i = 0; i < sizeof t_load_cases / sizeof t_load_cases[0]; ++i)
		t_add_test(t_mpi_load, &t_load_cases[i], t_load_cases[i].desc);
	t_add_test(t_mpi_exact_load, NULL, "exact load");
	t_add_test(t_mpi_large_load, NULL, "large load");
	t_add_test(t_mpi_large_load_fail, NULL, "large load (failure)");

	/* comparison */
	for (i = 0; i < sizeof t_cmp_cases / sizeof t_cmp_cases[0]; ++i)
		t_add_test(t_mpi_cmp, &t_cmp_cases[i], t_cmp_cases[i].desc);

	/* left / right shift */
	for (i = 0; i < sizeof t_lsh_cases / sizeof t_lsh_cases[0]; ++i)
		t_add_test(t_mpi_lsh, &t_lsh_cases[i], t_lsh_cases[i].desc);
	t_add_test(t_mpi_large_lsh, NULL, "large left shift");
	t_add_test(t_mpi_large_lsh_fail, NULL, "large left shift (failure)");
	for (i = 0; i < sizeof t_rsh_cases / sizeof t_rsh_cases[0]; ++i)
		t_add_test(t_mpi_rsh, &t_rsh_cases[i], t_rsh_cases[i].desc);

	/* addition */
	for (i = 0; i < sizeof t_add_cases / sizeof t_add_cases[0]; ++i)
		t_add_test(t_mpi_add_tc, &t_add_cases[i], t_add_cases[i].desc);
	t_add_test(t_mpi_add, NULL, "x = a + b");
	t_add_test(t_mpi_add_fail1, NULL, "x = a + b (failure 1)");
	t_add_test(t_mpi_add_fail2, NULL, "x = a + b (failure 2)");
	t_add_test(t_mpi_add_b_to_a, NULL, "a = a + b");
	t_add_test(t_mpi_add_b_to_a_fail, NULL, "a = a + b (failure)");
	t_add_test(t_mpi_add_b_to_a_equal, NULL, "a = a + b (a == b)");
	t_add_test(t_mpi_add_a_to_b, NULL, "b = a + b");
	t_add_test(t_mpi_add_a_to_b_fail, NULL, "b = a + b (failure)");
	t_add_test(t_mpi_add_a_to_b_equal, NULL, "b = a + b (a == b)");
	t_add_test(t_mpi_add_a_to_a, NULL, "a = a + a");
	t_add_test(t_mpi_add_a_to_a_fail, NULL, "a = a + a (failure)");
	t_add_test(t_mpi_add_a_and_a, NULL, "b = a + a");
	t_add_test(t_mpi_add_a_and_a_fail, NULL, "b = a + a (failure)");
	t_add_test(t_mpi_add_zero_to_a, NULL, "a = a + 0");
	t_add_test(t_mpi_add_zero_to_b, NULL, "b = 0 + b");

	/* subtraction */
	for (i = 0; i < sizeof t_sub_cases / sizeof t_sub_cases[0]; ++i)
		t_add_test(t_mpi_sub_tc, &t_sub_cases[i], t_sub_cases[i].desc);
	t_add_test(t_mpi_sub, NULL, "x = a - b");
	t_add_test(t_mpi_sub_fail, NULL, "x = a - b (failure)");
	t_add_test(t_mpi_sub_b_from_a, NULL, "a = a - b");
	t_add_test(t_mpi_sub_b_from_a_fail, NULL, "a = a - b (failure)");
	t_add_test(t_mpi_sub_a_from_b, NULL, "b = a - b");
	t_add_test(t_mpi_sub_a_from_b_fail, NULL, "b = a - b (failure)");
	t_add_test(t_mpi_sub_a_from_a, NULL, "a = a - a");
	t_add_test(t_mpi_sub_b_from_zero, NULL, "b = 0 - b");
	t_add_test(t_mpi_sub_zero_from_a, NULL, "a = a - z");
	t_add_test(t_mpi_sub_neg_target, NULL, "x = a - b (x initially < 0)");

	return (0);
}

static void
t_cleanup(void)
{

	t_assert(memcmp(&z, t_zero, sizeof z) == 0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, t_cleanup, argc, argv);
}
