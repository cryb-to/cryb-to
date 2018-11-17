/*-
 * Copyright (c) 2014-2018 Dag-Erling Smørgrav
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

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

/* test our own code, not the compiler's */
#undef HAVE___BUILTIN_CLZ
#undef HAVE___BUILTIN_CTZ
#undef HAVE___BUILTIN_FFS
#undef HAVE___BUILTIN_FFSL
#undef HAVE___BUILTIN_FFSLL
#undef HAVE___BUILTIN_FLS
#undef HAVE___BUILTIN_FLSL
#undef HAVE___BUILTIN_FLSLL

#include <cryb/bitwise.h>

#include <cryb/test.h>

static int
t_ffs(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	int u;
	int n, ret;

	ret = t_compare_i(0, cryb_ffs(0));
	for (u = 1, n = 1; u != 0; u <<= 1, n++) {
		t_printv("ffs(0x%08x) == %d\n", u, cryb_ffs(u));
		ret &= t_compare_i(n, cryb_ffs(u));
	}
	return (ret);
}

static int
t_ffsl(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	long int u;
	int n, ret;

	ret = t_compare_i(0, cryb_ffs(0));
	for (u = 1, n = 1; u != 0; u <<= 1, n++)
		ret &= t_compare_i(n, cryb_ffsl(u));
	return (ret);
}

static int
t_ffsll(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	long long int u;
	int n, ret;

	ret = t_compare_i(0, cryb_ffs(0));
	for (u = 1, n = 1; u != 0; u <<= 1, n++)
		ret &= t_compare_i(n, cryb_ffsll(u));
	return (ret);
}

static int
t_fls(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	unsigned int u;
	int n, ret;

	ret = t_compare_i(0, cryb_fls(0));
	for (u = 1, n = 1; u != 0; u <<= 1, n++) {
		t_printv("fls(0x%08x) == %d\n", u, cryb_fls(u));
		ret &= t_compare_i(n, cryb_fls(u));
	}
	return (ret);
}

static int
t_flsl(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	unsigned long int u;
	int n, ret;

	ret = t_compare_i(0, cryb_flsl(0));
	for (u = 1, n = 1; u != 0; u <<= 1, n++) {
		t_printv("flsl(0x%08lx) == %d\n", u, cryb_flsl(u));
		ret &= t_compare_i(n, cryb_flsl(u));
	}
	return (ret);
}

static int
t_flsll(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	unsigned long long int u;
	int n, ret;

	ret = t_compare_i(0, cryb_flsll(0));
	for (u = 1, n = 1; u != 0; u <<= 1, n++) {
		t_printv("flsll(0x%016llx) == %d\n", u, cryb_flsll(u));
		ret &= t_compare_i(n, cryb_flsll(u));
	}
	return (ret);
}


/***************************************************************************
 * Boilerplate
 */

static int
t_prepare(int argc CRYB_UNUSED, char *argv[] CRYB_UNUSED)
{

	t_add_test(t_ffs, 0, "ffs");
	t_add_test(t_ffsl, 0, "ffsl");
	t_add_test(t_ffsll, 0, "ffsll");
	t_add_test(t_fls, 0, "fls");
	t_add_test(t_flsl, 0, "flsl");
	t_add_test(t_flsll, 0, "flsll");
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
