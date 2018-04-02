/*-
 * Copyright (c) 2012-2017 Dag-Erling Smørgrav
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
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <cryb/test.h>

/*
 * Print a verbose message if a pointer is not null.
 */
int
t_is_null(const void *ptr)
{

	if (ptr != NULL) {
		t_printv("expected null pointer, got non-null pointer\n");
		return (0);
	}
	return (1);
}

/*
 * Print a verbose message if a pointer is null.
 */
int
t_is_not_null(const void *ptr)
{

	if (ptr == NULL) {
		t_printv("expected non-null pointer, got null pointer\n");
		return (0);
	}
	return (1);
}

/*
 * Compare two pointers, and print a verbose message if they differ.
 */
int
t_compare_ptr(const void *expected, const void *received)
{

	if (expected == NULL && received != NULL) {
		t_printv("expected null pointer\n");
		t_printv("received %p\n", received);
		return (0);
	} else if (received == NULL) {
		t_printv("expected %p\n", expected);
		t_printv("received null pointer\n");
		return (0);
	} else if (expected != received) {
		t_printv("expected %p\n", expected);
		t_printv("received %p\n", received);
		return (0);
	}
	return (1);
}

/*
 * Compare two buffers, and print a verbose message if they differ.
 */
int
t_compare_mem(const void *expected, const void *received, size_t len)
{

	if (expected == NULL && received != NULL) {
		t_printv("expected null pointer\n");
		t_printv("received ");
		t_printv_hex(received, len);
		t_printv("\n");
		return (0);
	} else if (received == NULL) {
		t_printv("expected ");
		t_printv_hex(expected, len);
		t_printv("\n");
		t_printv("received null pointer\n");
		return (0);
	} else if (memcmp(expected, received, len) != 0) {
		t_printv("expected ");
		t_printv_hex(expected, len);
		t_printv("\n");
		t_printv("received ");
		t_printv_hex(received, len);
		t_printv("\n");
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

	if (expected == NULL && received != NULL) {
		t_printv("expected null pointer\n");
		t_printv("received \"%s\"\n", received);
		return (0);
	} else if (expected != NULL && received == NULL) {
		t_printv("expected \"%s\"\n", expected);
		t_printv("received null pointer\n");
		return (0);
	} else if (strcmp(expected, received) != 0) {
		t_printv("expected %s\n", expected);
		t_printv("received %s\n", received);
		return (0);
	}
	return (1);
}

/*
 * Compare two strings up to a specific length, and print a verbose
 * message if they differ.
 */
int
t_compare_strn(const char *expected, const char *received, size_t len)
{

	if (expected == NULL && received != NULL) {
		t_printv("expected null pointer\n");
		t_printv("received \"%.*s\"\n", (int)len, received);
		return (0);
	} else if (expected != NULL && received == NULL) {
		t_printv("expected \"%.*s\"\n", (int)len, expected);
		t_printv("received null pointer\n");
		return (0);
	} else if (strncmp(expected, received, len) != 0) {
		t_printv("expected %.*s\n", (int)len, expected);
		t_printv("received %.*s\n", (int)len, received);
		return (0);
	}
	return (1);
}

/*
 * Compare to errno values, and print a verbose message if they differ.
 */
int
t_compare_errno(int expected, int received)
{
	char errbuf[256];

	if (expected != received) {
		if (expected == 0) {
			t_printv("expected no errno\n");
		} else {
			strerror_r(expected, errbuf, sizeof errbuf);
			t_printv("expected errno: %s\n", strerror(expected));
		}
		if (received == 0) {
			t_printv("received no errno\n");
		} else {
			strerror_r(received, errbuf, sizeof errbuf);
			t_printv("received errno: %s\n", strerror(received));
		}
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
		t_printv("expected " pf "\n", expected);		\
		t_printv("received " pf "\n", received);		\
		return (0);						\
	}								\
	return (1);							\
}

t_compare_num(i,	int,			"%d");
t_compare_num(u,	unsigned int,		"%u");
t_compare_num(il,	long,			"%ld");
t_compare_num(ul,	unsigned long,		"%lu");
t_compare_num(ill,	long long,		"%lld");
t_compare_num(ull,	unsigned long long,	"%llu");
t_compare_num(sz,	size_t,			"%zu");
t_compare_num(ssz,	ssize_t,		"%zd");
t_compare_num(i8,	int8_t,			"%" PRId8);
t_compare_num(u8,	uint8_t,		"%" PRIu8);
t_compare_num(x8,	uint8_t,		"0x%" PRIx8);
t_compare_num(i16,	int16_t,		"%" PRId16);
t_compare_num(u16,	uint16_t,		"%" PRIu16);
t_compare_num(x16,	uint16_t,		"0x%" PRIx16);
t_compare_num(i32,	int32_t,		"%" PRId32);
t_compare_num(u32,	uint32_t,		"%" PRIu32);
t_compare_num(x32,	uint32_t,		"0x%" PRIx32);
t_compare_num(i64,	int64_t,		"%" PRId64);
t_compare_num(u64,	uint64_t,		"%" PRIu64);
t_compare_num(x64,	uint64_t,		"0x%" PRIx64);

#undef t_compare_num

/*
 * Return non-zero if str is not NULL and points to a string which
 * compares equal to something we interpret as "true".
 */
int
t_str_is_true(const char *str)
{

	if (str == NULL || *str == '\0')
		return (0);
	if (strcmp(str, "1") == 0 ||
	    strcasecmp(str, "on") == 0 ||
	    strcasecmp(str, "y") == 0 ||
	    strcasecmp(str, "yes") == 0 ||
	    strcasecmp(str, "t") == 0 ||
	    strcasecmp(str, "true") == 0)
		return (1);
	return (0);
}

/*
 * Return non-zero if str is not NULL and points to a string which
 * compares equal to something we interpret as "false".
 */
int
t_str_is_false(const char *str)
{

	if (str == NULL || *str == '\0')
		return (0);
	if (strcmp(str, "0") == 0 ||
	    strcasecmp(str, "off") == 0 ||
	    strcasecmp(str, "n") == 0 ||
	    strcasecmp(str, "no") == 0 ||
	    strcasecmp(str, "f") == 0 ||
	    strcasecmp(str, "false") == 0)
		return (1);
	return (0);
}
