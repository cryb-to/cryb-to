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

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <cryb/endian.h>
#include <cryb/mpi.h>

#include <cryb/test.h>

#include "t_mpi.h"


/***************************************************************************
 * Test cases
 */

static struct t_gcd_case {
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
} t_gcd_cases[] = {
	{
		"gcd(0, 0) == 0",
		{                                                      },  0, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
	},
	{
		"gcd(0x20140901, 0x20140901) == 0x20140901",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
	},
	/* exercise both sides of the 0 check */
	{
		"gcd(0x20140901, 0) == 0",
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
		{                                                      },  0, 0,
		{                                                      },  0, 0,
	},
	{
		"gcd(0, 0x20140901) == 0",
		{                                                      },  0, 0,
		{                               0x20, 0x14, 0x09, 0x01 }, 30, 0,
		{                                                      },  0, 0,
	},
	/* exercise both sides of the parity reduction */
	{
		"gcd(512, 2048) == 512",
		{                                           0x02, 0x00 }, 10, 0,
		{                                           0x08, 0x00 }, 12, 0,
		{                                           0x02, 0x00 }, 10, 0,
	},
	{
		"gcd(2048, 512) == 512",
		{                                           0x08, 0x00 }, 12, 0,
		{                                           0x02, 0x00 }, 10, 0,
		{                                           0x02, 0x00 }, 10, 0,
	},
	/* HAC 14.55 */
	{
		"gcd(1764, 868) == 28",
		{                                           0x06, 0xe4 }, 11, 0,
		{                                           0x03, 0x64 }, 10, 0,
		{                                                 0x1c },  5, 0,
	},
	/* HAC 14.60 (small coprimes) */
	{
		"gcd(768454923, 542167814) == 1",
		{                               0x2d, 0xcd, 0xb1, 0x0b }, 30, 0,
		{                               0x20, 0x50, 0xd3, 0x06 }, 30, 0,
		{                                                 0x01 },  1, 0,
	},
	/* largish primes */
	{
		"gcd(83464029052373107673, 52563826518442013507) == 1",
		{ 0x04, 0x86, 0x4b, 0xcc, 0x36, 0x35, 0x95, 0x03, 0xd9 }, 59, 0,
		{ 0x02, 0xd9, 0x78, 0x39, 0xb9, 0x32, 0xfe, 0x73, 0x43 }, 58, 0,
		{                                                 0x01 },  1, 0,
	},
	/*
	 * Two examples of a pathological case for Stein's algorithm:
	 * gcd(k * n, k) where k and n are prime and n >> k.  The code
	 * goes into a loop which makes the absolute minimum amount of
	 * progress per iteration:
	 *
	 * gcd(k * n, k) = gcd(k * (n - 1) / 2, k)
	 *               = gcd(k * ((n - 1) / 2 - 1) / 2, k)
	 *               = ...
	 *
	 * for approximately log2(k * n) iterations.
	 */
	{
		"gcd(2^61-1, 1) == 1",
		{       0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 61, 0,
		{                                                 0x01 },  1, 0,
		{                                                 0x01 },  1, 0,
	},
	{
		"gcd((2^61-1)*3, 3) == 3",
		{       0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd }, 63, 0,
		{                                                 0x03 },  2, 0,
		{                                                 0x03 },  2, 0,
	},
};


/***************************************************************************
 * Test functions
 */

static int
t_mpi_gcd_tc(char **desc CRYB_UNUSED, void *arg)
{
	struct t_gcd_case *tc = arg;
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO, e = CRYB_MPI_ZERO;
	cryb_mpi x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_load(&a, tc->a, (tc->amsb + 7) / 8);
	a.neg = tc->aneg;
	mpi_load(&b, tc->b, (tc->bmsb + 7) / 8);
	b.neg = tc->bneg;
	mpi_load(&e, tc->e, (tc->emsb + 7) / 8);
	e.neg = tc->eneg;
	ret &= t_compare_i(0, mpi_gcd_abs(&x, &a, &b));
	ret &= t_compare_mpi(&e, &x);
	mpi_destroy(&a);
	mpi_destroy(&b);
	mpi_destroy(&e);
	mpi_destroy(&x);
	return (ret);
}

static int
t_mpi_gcd_ident_ab(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, x = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x20140901);
	ret &= t_compare_i(0, mpi_gcd_abs(&x, &a, &a));
	ret &= t_compare_mpi_u32(0x20140901, &x);
	mpi_destroy(&a);
	mpi_destroy(&x);
	return (ret);
}

static int
t_mpi_gcd_ident_xa(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x20140901);
	mpi_set(&b, 0x20140901);
	ret &= t_compare_i(0, mpi_gcd_abs(&a, &a, &b));
	ret &= t_compare_mpi_u32(0x20140901, &a);
	mpi_destroy(&a);
	mpi_destroy(&b);
	return (ret);
}

static int
t_mpi_gcd_ident_xb(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	cryb_mpi a = CRYB_MPI_ZERO, b = CRYB_MPI_ZERO;
	int ret = 1;

	mpi_set(&a, 0x20140901);
	mpi_set(&b, 0x20140901);
	ret &= t_compare_i(0, mpi_gcd_abs(&b, &a, &b));
	ret &= t_compare_mpi_u32(0x20140901, &b);
	mpi_destroy(&a);
	mpi_destroy(&b);
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

	for (i = 0; i < sizeof t_gcd_cases / sizeof t_gcd_cases[0]; ++i)
		t_add_test(t_mpi_gcd_tc, &t_gcd_cases[i],
		    "%s", t_gcd_cases[i].desc);
	t_add_test(t_mpi_gcd_ident_ab, NULL, "x = gcd(a, a)");
	t_add_test(t_mpi_gcd_ident_xa, NULL, "a = gcd(a, b)");
	t_add_test(t_mpi_gcd_ident_xb, NULL, "b = gcd(a, b)");

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
