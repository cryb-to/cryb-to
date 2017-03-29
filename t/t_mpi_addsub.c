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

#include "t_mpi.h"


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
		"|0| + |0| == 0",
		{                                                      },  0, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
	},
	{
		"|0| + |1| == 1",
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"|1| + |0| == 1",
		{                                                 0x01 },  1, 0,
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"|1| + |1| == 2",
		{                                                 0x01 },  1, 0,
		{                                                 0x01 },  1, 0,
		{                                                 0x02 },  2, 0,
	},
	{
		"|0| + |-1| == 1",
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 1,
		{                                                 0x01 },  1, 0,
	},
	{
		"|-1| + |0| == 1",
		{                                                 0x01 },  1, 1,
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"|-1| + |-1| == 2",
		{                                                 0x01 },  1, 1,
		{                                                 0x01 },  1, 1,
		{                                                 0x02 },  2, 0,
	},
	{
		"|0x20140901| + |0x19700101| == 0x39840a02",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
		{                               0x19, 0x70, 0x01, 0x01 }, 29, 0,
		{                               0x39, 0x84, 0x0a, 0x02 }, 30, 0,
	},
	{
		"|-0x20140901| + |0x19700101| == 0x39840a02",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 1,
		{                               0x19, 0x70, 0x01, 0x01 }, 29, 0,
		{                               0x39, 0x84, 0x0a, 0x02 }, 30, 0,
	},
	{
		"|0x20140901| + |-0x19700101| == 0x39840a02",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
		{                               0x19, 0x70, 0x01, 0x01 }, 29, 1,
		{                               0x39, 0x84, 0x0a, 0x02 }, 30, 0,
	},
	{
		"|-0x20140901| + |-0x19700101| == 0x39840a02",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 1,
		{                               0x19, 0x70, 0x01, 0x01 }, 29, 1,
		{                               0x39, 0x84, 0x0a, 0x02 }, 30, 0,
	},
	{
		"simple carry, "
		"|0xffffffff| + |0x01| == 0x0100000000",
		{                               0xff, 0xff, 0xff, 0xff }, 32, 0,
		{                                                 0x01 },  1, 0,
		{                         0x01, 0x00, 0x00, 0x00, 0x00 }, 33, 0,
	},
	{
		"complex carry, "
		"|0x0100000000ffffffff| + |0x01| == 0x010000000100000000",
		{ 0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff }, 64, 0,
		{                                                 0x01 },  1, 0,
		{ 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 }, 64, 0,
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
		"| |0| - |0| | == 0",
		{                                                      },  0, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
	},
	{
		"| |0| - |1| | == 1",
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"| |1| - |0| | == 1",
		{                                                 0x01 },  1, 0,
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"| |1| - |1| | == 0",
		{                                                 0x01 },  1, 0,
		{                                                 0x01 },  1, 0,
		{                                                 0x02 },  0, 0,
	},
	{
		"| |0| - |-1| | == 1",
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 1,
		{                                                 0x01 },  1, 0,
	},
	{
		"| |-1| - |0| | == 1",
		{                                                 0x01 },  1, 1,
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"| |-1| - |-1| | == 0",
		{                                                 0x01 },  1, 1,
		{                                                 0x01 },  1, 1,
		{                                                      },  0, 0,
	},
	{
		"| |4| - |2| | == 2",
		{                                                 0x04 },  3, 0,
		{                                                 0x02 },  2, 0,
		{                                                 0x02 },  2, 0,
	},
	{
		"| |2| - |4| | == 2",
		{                                                 0x02 },  2, 0,
		{                                                 0x04 },  3, 0,
		{                                                 0x02 },  2, 0,
	},
	{
		"| |0x20140901| - |0x19700101| | == 0x6a40800",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
		{                               0x19, 0x70, 0x01, 0x01 }, 29, 0,
		{                               0x06, 0xa4, 0x08, 0x00 }, 27, 0,
	},
	{
		"| |-0x20140901| - |0x19700101| | == 0x6a40800",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 1,
		{                               0x19, 0x70, 0x01, 0x01 }, 29, 0,
		{                               0x06, 0xa4, 0x08, 0x00 }, 27, 0,
	},
	{
		"| |0x20140901| - |-0x19700101| | == 0x6a40800",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
		{                               0x19, 0x70, 0x01, 0x01 }, 29, 1,
		{                               0x06, 0xa4, 0x08, 0x00 }, 27, 0,
	},
	{
		"| |-0x20140901| - |-0x19700101| | == 0x6a40800",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 1,
		{                               0x19, 0x70, 0x01, 0x01 }, 29, 1,
		{                               0x06, 0xa4, 0x08, 0x00 }, 27, 0,
	},
	{
		"0x120140901 - 0x119700101 == 0x6a40800",
		{                         0x01, 0x20, 0x14, 0x09, 0x01 }, 33, 0,
		{                         0x01, 0x19, 0x70, 0x01, 0x01 }, 33, 0,
		{                               0x06, 0xa4, 0x08, 0x00 }, 27, 0,
	},
	{
		"0x119700101 - 0x120140901 == 0x6a40800",
		{                         0x01, 0x19, 0x70, 0x01, 0x01 }, 33, 0,
		{                         0x01, 0x20, 0x14, 0x09, 0x01 }, 33, 0,
		{                               0x06, 0xa4, 0x08, 0x00 }, 27, 0,
	},
	{
		"simple carry, "
		"0x1000000000 - 0xfffffffff == 0x01",
		{                         0x10, 0x00, 0x00, 0x00, 0x00 }, 37, 0,
		{                         0x0f, 0xff, 0xff, 0xff, 0xff }, 36, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"simple carry, "
		"0x1000000000 - 0x01 == 0xfffffffff",
		{                         0x10, 0x00, 0x00, 0x00, 0x00 }, 37, 0,
		{                                                 0x01 },  1, 0,
		{                         0x0f, 0xff, 0xff, 0xff, 0xff }, 36, 0,
	},
	{
		"complex carry, "
		"0x010000000000000000 - 0xffffffffffffffff == 0x01",
		{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 65, 0,
		{       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 64, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"complex carry, "
		"0x010000000000000000 - 0x01 == 0xffffffffffffffff",
		{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 65, 0,
		{                                                 0x01 },  1, 0,
		{       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 64, 0,
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

	t_mpi_prepare();

	/* addition */
	for (i = 0; i < sizeof t_add_cases / sizeof t_add_cases[0]; ++i)
		t_add_test(t_mpi_add_tc, &t_add_cases[i],
		    "%s", t_add_cases[i].desc);
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
		t_add_test(t_mpi_sub_tc, &t_sub_cases[i],
		    "%s", t_sub_cases[i].desc);
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

	t_mpi_cleanup();
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, t_cleanup, argc, argv);
}
