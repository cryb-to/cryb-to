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

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <cryb/endian.h>
#include <cryb/mpi.h>

#include <cryb/test.h>

#include "t_mpi.h"


/***************************************************************************
 * Multiplication
 */

static struct t_mul_case {
	const char *desc;
	int err;
	uint8_t a[16];
	size_t amsb;
	int aneg:1;
	uint8_t b[16];
	size_t bmsb;
	int bneg:1;
	uint8_t e[16];
	size_t emsb;
	int eneg:1;
} t_mul_cases[] = {
	{
		"0 * 0 == 0",                                              0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
	},
	{
		"0 * 1 == 0",                                              0,
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 0,
		{                                                      },  0, 0,
	},
	{
		"1 * 0 == 0",                                              0,
		{                                                 0x01 },  1, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
	},
	{
		"1 * 1 == 1",                                              0,
		{                                                 0x01 },  1, 0,
		{                                                 0x01 },  1, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"(2^32 + 1) * (2^32 + 1) == 2^64 + 2^33 + 1",              0,
		{                         0x01, 0x00, 0x00, 0x00, 0x01 }, 33, 0,
		{                         0x01, 0x00, 0x00, 0x00, 0x01 }, 33, 0,
		{ 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01 }, 65, 0,
	},
	{
		"(2^32 + 1) * (2^32 - 1) == 2^64 - 1",                     0,
		{                         0x01, 0x00, 0x00, 0x00, 0x01 }, 33, 0,
		{                               0xff, 0xff, 0xff, 0xff }, 32, 0,
		{       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 63, 0,
	},
	{
		"(2^32 - 1) * (2^32 + 1) == 2^64 - 1",                     0,
		{                               0xff, 0xff, 0xff, 0xff }, 32, 0,
		{                         0x01, 0x00, 0x00, 0x00, 0x01 }, 33, 0,
		{       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 63, 0,
	},
	{
		"(2^32 - 1) * (2^32 - 1) == 2^64 - 2^33 + 1",              0,
		{                               0xff, 0xff, 0xff, 0xff }, 32, 0,
		{                               0xff, 0xff, 0xff, 0xff }, 32, 0,
		{       0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x01 }, 64, 0,
	},
};

static int
t_mpi_mul_tc(char **desc CRYB_UNUSED, void *arg)
{
	struct t_mul_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, tc->a, (tc->amsb + 7) / 8);
	a.neg = tc->aneg;
	mpi_load(&b, tc->b, (tc->bmsb + 7) / 8);
	b.neg = tc->bneg;
	mpi_load(&e, tc->e, (tc->emsb + 7) / 8);
	e.neg = tc->eneg;
	ret &= t_compare_i(0, mpi_mul_abs(&x, &a, &b));
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}


/***************************************************************************
 * Division
 */

static struct t_div_case {
	const char *desc;
	int err;
	uint8_t a[16];
	size_t amsb;
	int aneg:1;
	uint8_t b[16];
	size_t bmsb;
	int bneg:1;
	uint8_t q[16];
	size_t qmsb;
	int qneg:1;
	uint8_t r[16];
	size_t rmsb;
	int rneg:1;
} t_div_cases[] = {
	{
		/* divide something by zero */
		"1 / 0 == ERROR",                                         -1,
		{                                                 0x01 },  1, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
	},
	{
		/* divide zero by something */
		"0 / 1 == 0",                                              0,
		{                                                      },  0, 0,
		{                                                 0x01 },  1, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
	},
	{
		/* divide something by one */
		"3 / 1 == 3",                                              0,
		{                                                 0x03 },  2, 0,
		{                                                 0x01 },  1, 0,
		{                                                 0x03 },  2, 0,
		{                                                      },  0, 0,
	},
	{
		/* divide something by itself */
		"3 / 3 == 1",                                              0,
		{                                                 0x03 },  2, 0,
		{                                                 0x03 },  2, 0,
		{                                                 0x01 },  1, 0,
		{                                                      },  0, 0,
	},
	{
		/* divide something by something larger */
		"127 / 254 == 0 rem 127",                                     0,
		{                                                 0x7f },  7, 0,
		{                                                 0xfe },  8, 0,
		{                                                      },  0, 0,
		{                                                 0x7f },  7, 0,
	},
	{
		/* no remainder */
		"254 / 127 = 2",                                           0,
		{                                                 0xfe },  8, 0,
		{                                                 0x7f },  7, 0,
		{                                                 0x02 },  2, 0,
		{                                                      },  0, 0,
	},
	{
		/* remainder */
		"257 / 127 == 2 rem 3",                                    0,
		{                                           0x01, 0x01 },  9, 0,
		{                                                 0x7f },  7, 0,
		{                                                 0x02 },  2, 0,
		{                                                 0x03 },  2, 0,
	},
	/* XXX needs lots more tests! */
};

static int
t_mpi_div_tc(char **desc CRYB_UNUSED, void *arg)
{
	struct t_div_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO;
	cryb_mpi q = CRYB_MPI_ZERO, r = CRYB_MPI_ZERO;
	cryb_mpi eq = CRYB_MPI_ZERO, er = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, tc->a, (tc->amsb + 7) / 8);
	a.neg = tc->aneg;
	mpi_load(&b, tc->b, (tc->bmsb + 7) / 8);
	b.neg = tc->bneg;
	mpi_load(&eq, tc->q, (tc->qmsb + 7) / 8);
	eq.neg = tc->qneg;
	mpi_load(&er, tc->r, (tc->rmsb + 7) / 8);
	er.neg = tc->rneg;
	ret &= t_compare_i(tc->err, mpi_div_abs(&q, &r, &a, &b));
	if (tc->err == 0) {
		ret &= t_compare_mpi(&eq, &q);
		ret &= t_compare_mpi(&er, &r);
	}
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&q);
	mpi_destroy(&r);
	mpi_destroy(&eq);
	mpi_destroy(&er);
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

	/* multiplication */
	for (i = 0; i < sizeof t_mul_cases / sizeof t_mul_cases[0]; ++i)
		t_add_test(t_mpi_mul_tc, &t_mul_cases[i],
		    "%s", t_mul_cases[i].desc);

	/* division */
	for (i = 0; i < sizeof t_div_cases / sizeof t_div_cases[0]; ++i)
		t_add_test(t_mpi_div_tc, &t_div_cases[i],
		    "%s", t_div_cases[i].desc);

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
