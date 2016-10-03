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
 */

#ifndef CRYB_TEST_H_INCLUDED
#define CRYB_TEST_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/coverage.h>

CRYB_DISABLE_COVERAGE

CRYB_BEGIN

const char *cryb_test_version(void);

/*
 * Structure describing a test.  Includes a pointer to the function that
 * performs the test, a pointer to the default description or comment
 * associated with the test, and a pointer to arbitrary data passed to the
 * test function.
 */
typedef int (t_func)(char **, void *);
struct t_test {
	t_func *func;
	void *arg;
	char *desc;
};

extern const char *t_progname;

void t_add_test(t_func *, void *, const char *, ...);
void t_add_tests(struct t_test *, int);

typedef int (*t_prepare_func)(int, char **);
typedef void (*t_cleanup_func)(void);
void t_main(t_prepare_func, t_cleanup_func, int, char **)
	CRYB_NORETURN;

void t_verbose_hex(const uint8_t *, size_t);
void t_verbose(const char *, ...)
	CRYB_PRINTF(1, 2);

#ifdef _IONBF /* proxy for <stdio.h> */
/*
 * Convenience functions for temp files
 */
struct t_file {
	char *name;
	FILE *file;
	struct t_file *prev, *next;
};

struct t_file *t_fopen(const char *);
int t_fprintf(struct t_file *, const char *, ...)
	CRYB_PRINTF(2, 3);
int t_ferror(struct t_file *);
int t_feof(struct t_file *);
void t_frewind(struct t_file *);
void t_fclose(struct t_file *);
void t_fcloseall(void);
#endif

/*
 * Various utilities
 */
int t_is_null(const void *);
int t_is_not_null(const void *);
int t_compare_mem(const void *, const void *, size_t);
int t_compare_str(const char *, const char *);
int t_compare_strn(const char *, const char *, size_t);
#define t_compare_num(n, t) int t_compare_##n(t, t);
t_compare_num(i, int);
t_compare_num(u, unsigned int);
t_compare_num(il, long);
t_compare_num(ul, unsigned long);
t_compare_num(ill, long long);
t_compare_num(ull, unsigned long long);
t_compare_num(sz, size_t);
t_compare_num(ssz, ssize_t);
t_compare_num(i8, int8_t);
t_compare_num(u8, uint8_t);
t_compare_num(x8, uint8_t);
t_compare_num(i16, int16_t);
t_compare_num(u16, uint16_t);
t_compare_num(x16, uint16_t);
t_compare_num(i32, int32_t);
t_compare_num(u32, uint32_t);
t_compare_num(x32, uint32_t);
t_compare_num(i64, int64_t);
t_compare_num(u64, uint64_t);
t_compare_num(x64, uint64_t);
t_compare_num(ptr, void *);
#undef t_compare_num

int t_str_is_true(const char *);
int t_str_is_false(const char *);

/*
 * Useful constants
 */
extern const uint8_t t_zero[4096];
extern const uint8_t t_seq8[256];

/*
 * Debugging allocator
 */
extern int t_malloc_fail;
extern int t_malloc_fail_after;
extern int t_malloc_fatal;
size_t t_malloc_snapshot(void *, size_t);
#ifdef _IONBF /* proxy for <stdio.h> */
void t_malloc_printstats(FILE *);
#endif
extern struct t_test t_memory_leak;

CRYB_END

#endif
