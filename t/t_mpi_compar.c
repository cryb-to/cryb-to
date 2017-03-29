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
 * Comparison
 */

static struct t_cmp_case {
	const char *desc;
	int32_t a, b;
	int cmp;
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

static int
t_mpi_eq(char **desc CRYB_UNUSED, void *arg)
{
	struct t_cmp_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, tc->a);
	mpi_set(&b, tc->b);
	ret &= t_compare_i(tc->cmp == 0, mpi_eq(&a, &b));
	mpi_destroy(&a);
	mpi_destroy(&b);
	return (ret);
}

/*
 * Compare an MPI with an integer
 */
static int
t_mpi_cmp_i32(char **desc CRYB_UNUSED, void *arg)
{
	struct t_cmp_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, tc->a);
	ret &= t_compare_i(tc->cmp, mpi_cmp_i32(&a, tc->b));
	mpi_destroy(&a);
	return (ret);
}

static int
t_mpi_eq_i32(char **desc CRYB_UNUSED, void *arg)
{
	struct t_cmp_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, tc->a);
	ret &= t_compare_i(tc->cmp == 0, mpi_eq_i32(&a, tc->b));
	mpi_destroy(&a);
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

	/* comparison */
	for (i = 0; i < sizeof t_cmp_cases / sizeof t_cmp_cases[0]; ++i)
		t_add_test(t_mpi_cmp, &t_cmp_cases[i],
		    "mpi cmp mpi (%s)", t_cmp_cases[i].desc);
	for (i = 0; i < sizeof t_cmp_cases / sizeof t_cmp_cases[0]; ++i)
		t_add_test(t_mpi_eq, &t_cmp_cases[i],
		    "mpi eq mpi (%s)", t_cmp_cases[i].desc);
	for (i = 0; i < sizeof t_cmp_cases / sizeof t_cmp_cases[0]; ++i)
		t_add_test(t_mpi_cmp_i32, &t_cmp_cases[i],
		    "mpi cmp i32 (%s)", t_cmp_cases[i].desc);
	for (i = 0; i < sizeof t_cmp_cases / sizeof t_cmp_cases[0]; ++i)
		t_add_test(t_mpi_eq_i32, &t_cmp_cases[i],
		    "mpi eq i32 (%s)", t_cmp_cases[i].desc);

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
