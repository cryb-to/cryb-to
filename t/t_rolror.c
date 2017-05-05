/*-
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

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cryb/bitwise.h>

#include <cryb/test.h>

/***************************************************************************
 * Test cases
 */

static struct t_case8 {
	uint8_t		 i;
	unsigned int	 n;
	uint8_t		 o;
} t_cases8[] = {
	{ 0x0f,  0, 0x0f, },
	{ 0x0f,  1, 0x1e, },
	{ 0x0f,  2, 0x3c, },
	{ 0x0f,  3, 0x78, },

	{ 0x0f,  4, 0xf0, },
	{ 0x0f,  5, 0xe1, },
	{ 0x0f,  6, 0xc3, },
	{ 0x0f,  7, 0x87, },

	{ 0x0f,  8, 0x0f, },
};

static struct t_case16 {
	uint16_t	 i;
	unsigned int	 n;
	uint16_t	 o;
} t_cases16[] = {
	{ 0x0f1eU,  0, 0x0f1eU, },
	{ 0x0f1eU,  1, 0x1e3cU, },
	{ 0x0f1eU,  2, 0x3c78U, },
	{ 0x0f1eU,  3, 0x78f0U, },

	{ 0x0f1eU,  4, 0xf1e0U, },
	{ 0x0f1eU,  5, 0xe3c1U, },
	{ 0x0f1eU,  6, 0xc783U, },
	{ 0x0f1eU,  7, 0x8f07U, },

	{ 0x0f1eU,  8, 0x1e0fU, },
	{ 0x0f1eU,  9, 0x3c1eU, },
	{ 0x0f1eU, 10, 0x783cU, },
	{ 0x0f1eU, 11, 0xf078U, },

	{ 0x0f1eU, 12, 0xe0f1U, },
	{ 0x0f1eU, 13, 0xc1e3U, },
	{ 0x0f1eU, 14, 0x83c7U, },
	{ 0x0f1eU, 15, 0x078fU, },

	{ 0x0f1eU, 16, 0x0f1eU, },
};

static struct t_case32 {
	uint32_t	 i;
	unsigned int	 n;
	uint32_t	 o;
} t_cases32[] = {
	{ 0x0f1e2d3cUL,  0, 0x0f1e2d3cUL, },
	{ 0x0f1e2d3cUL,  1, 0x1e3c5a78UL, },
	{ 0x0f1e2d3cUL,  2, 0x3c78b4f0UL, },
	{ 0x0f1e2d3cUL,  3, 0x78f169e0UL, },

	{ 0x0f1e2d3cUL,  4, 0xf1e2d3c0UL, },
	{ 0x0f1e2d3cUL,  5, 0xe3c5a781UL, },
	{ 0x0f1e2d3cUL,  6, 0xc78b4f03UL, },
	{ 0x0f1e2d3cUL,  7, 0x8f169e07UL, },

	{ 0x0f1e2d3cUL,  8, 0x1e2d3c0fUL, },
	{ 0x0f1e2d3cUL,  9, 0x3c5a781eUL, },
	{ 0x0f1e2d3cUL, 10, 0x78b4f03cUL, },
	{ 0x0f1e2d3cUL, 11, 0xf169e078UL, },

	{ 0x0f1e2d3cUL, 12, 0xe2d3c0f1UL, },
	{ 0x0f1e2d3cUL, 13, 0xc5a781e3UL, },
	{ 0x0f1e2d3cUL, 14, 0x8b4f03c7UL, },
	{ 0x0f1e2d3cUL, 15, 0x169e078fUL, },

	{ 0x0f1e2d3cUL, 16, 0x2d3c0f1eUL, },
	{ 0x0f1e2d3cUL, 17, 0x5a781e3cUL, },
	{ 0x0f1e2d3cUL, 18, 0xb4f03c78UL, },
	{ 0x0f1e2d3cUL, 19, 0x69e078f1UL, },

	{ 0x0f1e2d3cUL, 20, 0xd3c0f1e2UL, },
	{ 0x0f1e2d3cUL, 21, 0xa781e3c5UL, },
	{ 0x0f1e2d3cUL, 22, 0x4f03c78bUL, },
	{ 0x0f1e2d3cUL, 23, 0x9e078f16UL, },

	{ 0x0f1e2d3cUL, 24, 0x3c0f1e2dUL, },
	{ 0x0f1e2d3cUL, 25, 0x781e3c5aUL, },
	{ 0x0f1e2d3cUL, 26, 0xf03c78b4UL, },
	{ 0x0f1e2d3cUL, 27, 0xe078f169UL, },

	{ 0x0f1e2d3cUL, 28, 0xc0f1e2d3UL, },
	{ 0x0f1e2d3cUL, 29, 0x81e3c5a7UL, },
	{ 0x0f1e2d3cUL, 30, 0x03c78b4fUL, },
	{ 0x0f1e2d3cUL, 31, 0x078f169eUL, },

	{ 0x0f1e2d3cUL, 32, 0x0f1e2d3cUL, },
};

static struct t_case64 {
	uint64_t	 i;
	unsigned int	 n;
	uint64_t	 o;
} t_cases64[] = {
	{ 0x0f1e2d3c4b5a6987ULL,  0, 0x0f1e2d3c4b5a6987ULL, },
	{ 0x0f1e2d3c4b5a6987ULL,  1, 0x1e3c5a7896b4d30eULL, },
	{ 0x0f1e2d3c4b5a6987ULL,  2, 0x3c78b4f12d69a61cULL, },
	{ 0x0f1e2d3c4b5a6987ULL,  3, 0x78f169e25ad34c38ULL, },

	{ 0x0f1e2d3c4b5a6987ULL,  4, 0xf1e2d3c4b5a69870ULL, },
	{ 0x0f1e2d3c4b5a6987ULL,  5, 0xe3c5a7896b4d30e1ULL, },
	{ 0x0f1e2d3c4b5a6987ULL,  6, 0xc78b4f12d69a61c3ULL, },
	{ 0x0f1e2d3c4b5a6987ULL,  7, 0x8f169e25ad34c387ULL, },

	{ 0x0f1e2d3c4b5a6987ULL,  8, 0x1e2d3c4b5a69870fULL, },
	{ 0x0f1e2d3c4b5a6987ULL,  9, 0x3c5a7896b4d30e1eULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 10, 0x78b4f12d69a61c3cULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 11, 0xf169e25ad34c3878ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 12, 0xe2d3c4b5a69870f1ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 13, 0xc5a7896b4d30e1e3ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 14, 0x8b4f12d69a61c3c7ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 15, 0x169e25ad34c3878fULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 16, 0x2d3c4b5a69870f1eULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 17, 0x5a7896b4d30e1e3cULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 18, 0xb4f12d69a61c3c78ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 19, 0x69e25ad34c3878f1ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 20, 0xd3c4b5a69870f1e2ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 21, 0xa7896b4d30e1e3c5ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 22, 0x4f12d69a61c3c78bULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 23, 0x9e25ad34c3878f16ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 24, 0x3c4b5a69870f1e2dULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 25, 0x7896b4d30e1e3c5aULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 26, 0xf12d69a61c3c78b4ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 27, 0xe25ad34c3878f169ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 28, 0xc4b5a69870f1e2d3ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 29, 0x896b4d30e1e3c5a7ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 30, 0x12d69a61c3c78b4fULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 31, 0x25ad34c3878f169eULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 32, 0x4b5a69870f1e2d3cULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 33, 0x96b4d30e1e3c5a78ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 34, 0x2d69a61c3c78b4f1ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 35, 0x5ad34c3878f169e2ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 36, 0xb5a69870f1e2d3c4ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 37, 0x6b4d30e1e3c5a789ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 38, 0xd69a61c3c78b4f12ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 39, 0xad34c3878f169e25ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 40, 0x5a69870f1e2d3c4bULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 41, 0xb4d30e1e3c5a7896ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 42, 0x69a61c3c78b4f12dULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 43, 0xd34c3878f169e25aULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 44, 0xa69870f1e2d3c4b5ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 45, 0x4d30e1e3c5a7896bULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 46, 0x9a61c3c78b4f12d6ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 47, 0x34c3878f169e25adULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 48, 0x69870f1e2d3c4b5aULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 49, 0xd30e1e3c5a7896b4ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 50, 0xa61c3c78b4f12d69ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 51, 0x4c3878f169e25ad3ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 52, 0x9870f1e2d3c4b5a6ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 53, 0x30e1e3c5a7896b4dULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 54, 0x61c3c78b4f12d69aULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 55, 0xc3878f169e25ad34ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 56, 0x870f1e2d3c4b5a69ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 57, 0x0e1e3c5a7896b4d3ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 58, 0x1c3c78b4f12d69a6ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 59, 0x3878f169e25ad34cULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 60, 0x70f1e2d3c4b5a698ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 61, 0xe1e3c5a7896b4d30ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 62, 0xc3c78b4f12d69a61ULL, },
	{ 0x0f1e2d3c4b5a6987ULL, 63, 0x878f169e25ad34c3ULL, },

	{ 0x0f1e2d3c4b5a6987ULL, 64, 0x0f1e2d3c4b5a6987ULL, },
};


/***************************************************************************
 * Test functions
 */

#define T_ROLROR(N)							\
	static int							\
	t_rol##N(char **desc, void *arg)				\
	{								\
		struct t_case##N *t = arg;				\
		(void)asprintf(desc, "rol" #N "(%u)", t->n);		\
		return (t_compare_x##N(t->o, rol##N(t->i, t->n)));	\
	}								\
	static int							\
	t_ror##N(char **desc, void *arg)				\
	{								\
		struct t_case##N *t = arg;				\
		(void)asprintf(desc, "ror" #N "(%u)", N - t->n);	\
		return (t_compare_x##N(t->o, ror##N(t->i, N - t->n)));	\
	}

T_ROLROR(8);
T_ROLROR(16);
T_ROLROR(32);
T_ROLROR(64);

#undef T_ROLROR


/***************************************************************************
 * Boilerplate
 */

static int
t_prepare(int argc, char *argv[])
{
	unsigned int i, n;

	(void)argc;
	(void)argv;
#define T_ROLROR(N)							\
	do {								\
		n = sizeof t_cases##N / sizeof t_cases##N[0];		\
		for (i = 0; i < n; ++i)					\
			t_add_test(t_rol##N, &t_cases##N[i], "rol" #N);	\
		for (i = n; i > 0; --i)					\
			t_add_test(t_ror##N, &t_cases##N[i - 1], "ror" #N); \
	} while (0)

	T_ROLROR(8);
	T_ROLROR(16);
	T_ROLROR(32);
	T_ROLROR(64);

#undef T_ROLROR

	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
