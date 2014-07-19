/*-
 * Copyright (c) 2012-2014 Dag-Erling Smørgrav
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
 *
 * Author: Dag-Erling Smørgrav <des@des.no>
 *
 * $Cryb$
 */

#include "cryb/impl.h"

#include <err.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "t.h"

/*
 * Compare two buffers, and print a verbose message if they differ.
 */
int
t_compare_mem(const void *expected, const void *received, size_t len)
{

	if (memcmp(expected, received, len) != 0) {
		t_verbose("expected ");
		t_verbose_hex(expected, len);
		t_verbose("\n");
		t_verbose("received ");
		t_verbose_hex(received, len);
		t_verbose("\n");
		return (0);
	}
	return (1);
}

/*
 * Compare two strings, and print a verbose message if they differ.
 */
int
t_compare_str(const char *expected, const char *received)
{

	if (strcmp(expected, received) != 0) {
		t_verbose("expected %s\n", expected);
		t_verbose("received %s\n", received);
		return (0);
	}
	return (1);
}

/*
 * Compare two numbers, and print a verbose message if they differ.
 */
#define t_compare_num(n, t, pf)						\
int									\
t_compare_##n(t expected, t received)					\
{									\
									\
	if (received != expected) {					\
		t_verbose("expected %" pf "\n", expected);		\
		t_verbose("received %" pf "\n", received);		\
		return (0);						\
	}								\
	return (1);							\
}

t_compare_num(i, int, "d");
t_compare_num(u, unsigned int, "u");
t_compare_num(il, long, "ld");
t_compare_num(ul, unsigned long, "lu");
t_compare_num(ill, long long, "lld");
t_compare_num(ull, unsigned long long, "llu");
t_compare_num(sz, size_t, "zu");
t_compare_num(i8, int8_t, PRId8);
t_compare_num(u8, uint8_t, PRIu8);
t_compare_num(x8, uint8_t, "#" PRIx8);
t_compare_num(i16, int16_t, PRId16);
t_compare_num(u16, uint16_t, PRIu16);
t_compare_num(x16, uint16_t, "#" PRIx16);
t_compare_num(i32, int32_t, PRId32);
t_compare_num(u32, uint32_t, PRIu32);
t_compare_num(x32, uint32_t, "#" PRIx32);
t_compare_num(i64, int64_t, PRId64);
t_compare_num(u64, uint64_t, PRIu64);
t_compare_num(x64, uint64_t, "#" PRIx64);

#undef t_compare_num
