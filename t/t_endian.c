/*-
 * Copyright (c) 2014-2016 Dag-Erling Smørgrav
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

#include <stddef.h>
#include <stdint.h>

/* test our own code, not the compiler's */
#undef HAVE___BUILTIN_BSWAP16
#undef HAVE___BUILTIN_BSWAP32
#undef HAVE___BUILTIN_BSWAP64

#include <cryb/endian.h>

#include <cryb/test.h>

static uint16_t be16 = 0xf001U;
static uint32_t be32 = 0xdeadbeefUL;
static uint64_t be64 = 0xdeadbeefcafef001ULL;
static uint16_t le16 = 0x01f0U;
static uint32_t le32 = 0xefbeaddeUL;
static uint64_t le64 = 0x01f0fecaefbeaddeULL;
static uint16_t h16;
static uint32_t h32;
static uint64_t h64;
static const void *s16 = "\xf0\x01";
static const void *s32 = "\xde\xad\xbe\xef";
static const void *s64 = "\xde\xad\xbe\xef\xca\xfe\xf0\x01";

#define T_BSWAP(w)							\
	static int							\
	t_bswap##w(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)	\
	{								\
		uint##w##_t swap;					\
									\
		swap = cryb_bswap##w(be##w);				\
		return (t_compare_x##w(le##w, swap));			\
	}

T_BSWAP(16);
T_BSWAP(32);
T_BSWAP(64);

#define T_DEC(e, w)							\
	static int							\
	t_##e##w##dec(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)	\
	{								\
		uint##w##_t dec;					\
									\
		dec = cryb_##e##w##dec(s##w);				\
		return (t_compare_x##w(e##w, dec));			\
	}

T_DEC(be, 16)
T_DEC(be, 32)
T_DEC(be, 64)
T_DEC(le, 16)
T_DEC(le, 32)
T_DEC(le, 64)

#define T_ENC(e, w)							\
	static int							\
	t_##e##w##enc(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)	\
	{								\
		uint##w##_t enc;					\
									\
		cryb_##e##w##enc(&enc, e##w);				\
		return (t_compare_x##w(*(const uint##w##_t *)s##w, enc)); \
	}

T_ENC(be, 16)
T_ENC(be, 32)
T_ENC(be, 64)
T_ENC(le, 16)
T_ENC(le, 32)
T_ENC(le, 64)

#define T_HTO(e, w)							\
	static int							\
	t_hto##e##w(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)	\
	{								\
		uint##w##_t e;						\
									\
		e = cryb_hto##e##w(h##w);				\
		return (t_compare_x##w(e##w, e));			\
	}

T_HTO(be, 16);
T_HTO(be, 32);
T_HTO(be, 64);
T_HTO(le, 16);
T_HTO(le, 32);
T_HTO(le, 64);

#define T_TOH(e, w)							\
	static int							\
	t_##e##w##toh(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)	\
	{								\
		uint##w##_t h;						\
									\
		h = cryb_##e##w##toh(e##w);				\
		return (t_compare_x##w(h##w, h));			\
	}

T_TOH(be, 16);
T_TOH(be, 32);
T_TOH(be, 64);
T_TOH(le, 16);
T_TOH(le, 32);
T_TOH(le, 64);
	

/***************************************************************************
 * Boilerplate
 */

#define T_BSWAP_ADD(w)							\
	t_add_test(t_bswap##w, 0, "bswap" #w);
#define T_DEC_ADD(e, w)							\
	t_add_test(t_##e##w##dec, 0, #e #w "dec");
#define T_ENC_ADD(e, w)							\
	t_add_test(t_##e##w##enc, 0, #e #w "enc");
#define T_HTO_ADD(e, w)							\
	t_add_test(t_hto##e##w, 0, "hto" #e #w);
#define T_TOH_ADD(e, w)							\
	t_add_test(t_##e##w##toh, 0, #e #w "toh");

static int
t_prepare(int argc, char *argv[])
{

	(void)argc;
	(void)argv;

#if WORD_BIGENDIAN
	h16 = be16;
	h32 = be32;
	h64 = be64;
#else
	h16 = le16;
	h32 = le32;
	h64 = le64;
#endif

	T_BSWAP_ADD(16);
	T_BSWAP_ADD(32);
	T_BSWAP_ADD(64);
	T_DEC_ADD(be, 16);
	T_DEC_ADD(be, 32);
	T_DEC_ADD(be, 64);
	T_DEC_ADD(le, 16);
	T_DEC_ADD(le, 32);
	T_DEC_ADD(le, 64);
	T_ENC_ADD(be, 16);
	T_ENC_ADD(be, 32);
	T_ENC_ADD(be, 64);
	T_ENC_ADD(le, 16);
	T_ENC_ADD(le, 32);
	T_ENC_ADD(le, 64);
	T_HTO_ADD(be, 16);
	T_HTO_ADD(be, 32);
	T_HTO_ADD(be, 64);
	T_HTO_ADD(le, 16);
	T_HTO_ADD(le, 32);
	T_HTO_ADD(le, 64);
	T_TOH_ADD(be, 16);
	T_TOH_ADD(be, 32);
	T_TOH_ADD(be, 64);
	T_TOH_ADD(le, 16);
	T_TOH_ADD(le, 32);
	T_TOH_ADD(le, 64);
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
