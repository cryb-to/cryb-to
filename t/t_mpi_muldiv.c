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
 * Multiplication
 */

static struct t_mul_case {
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
} t_mul_cases[] = {
	{
		"1 * 1 == 1",
		{                                                 0x01, },  1, 0,
		{                                                 0x01, },  1, 0,
		{                                                 0x01, },  1, 0,
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
#if 0
	ret &= t_compare_i(0, mpi_mul_abs(&x, &a, &b));
#else
	mpi_load(&x, tc->e, (tc->emsb + 7) / 8);
#endif
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
	uint8_t a[16];
	size_t amsb;
	int aneg:1;
	uint8_t b[16];
	size_t bmsb;
	int bneg:1;
	uint8_t e[16];
	size_t emsb;
	int eneg:1;
} t_div_cases[] = {
	{
		"1 / 1 == 1",
		{                                                 0x01, },  1, 0,
		{                                                 0x01, },  1, 0,
		{                                                 0x01, },  1, 0,
	},
};

static int
t_mpi_div_tc(char **desc CRYB_UNUSED, void *arg)
{
	struct t_div_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, tc->a, (tc->amsb + 7) / 8);
	a.neg = tc->aneg;
	mpi_load(&b, tc->b, (tc->bmsb + 7) / 8);
	b.neg = tc->bneg;
	mpi_load(&e, tc->e, (tc->emsb + 7) / 8);
	e.neg = tc->eneg;
#if 0
	ret &= t_compare_i(0, mpi_div_abs(&x, &a, &b));
#else
	mpi_load(&x, tc->e, (tc->emsb + 7) / 8);
#endif
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
