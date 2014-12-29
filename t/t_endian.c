/*-
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

#include "cryb/impl.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cryb/endian.h>

#include "t.h"

static uint32_t be32 = 0xdeadbeefUL;
static uint64_t be64 = 0xdeadbeefcafef001ULL;
static uint32_t le32 = 0xefbeaddeUL;
static uint64_t le64 = 0x01f0fecaefbeaddeULL;
static const void *s32 = "\xde\xad\xbe\xef";
static const void *s64 = "\xde\xad\xbe\xef\xca\xfe\xf0\x01";

#define T_DEC(e, w)							\
	static int							\
	t_##e##w##dec(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)	\
	{								\
		uint##w##_t dec;					\
									\
		dec = cryb_##e##w##dec(s##w);				\
		return (t_compare_x##w(e##w, dec));			\
	}

T_DEC(be, 32)
T_DEC(be, 64)
T_DEC(le, 32)
T_DEC(le, 64)

#define T_ENC(e, w)							\
	static int							\
	t_##e##w##enc(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)	\
	{								\
		uint##w##_t enc;					\
									\
		cryb_##e##w##enc(&enc, e##w);				\
		return (t_compare_x##w(*(uint##w##_t *)s##w, enc));	\
	}

T_ENC(be, 32)
T_ENC(be, 64)
T_ENC(le, 32)
T_ENC(le, 64)


/***************************************************************************
 * Boilerplate
 */

#define T_DEC_ADD(e, w)							\
	t_add_test(t_##e##w##dec, 0, #e #w "dec");
#define T_ENC_ADD(e, w)							\
	t_add_test(t_##e##w##enc, 0, #e #w "enc");

int
t_prepare(int argc, char *argv[])
{

	(void)argc;
	(void)argv;
	T_DEC_ADD(be, 32);
	T_DEC_ADD(be, 64);
	T_DEC_ADD(le, 32);
	T_DEC_ADD(le, 64);
	T_ENC_ADD(be, 32);
	T_ENC_ADD(be, 64);
	T_ENC_ADD(le, 32);
	T_ENC_ADD(le, 64);
	return (0);
}

void
t_cleanup(void)
{
}
