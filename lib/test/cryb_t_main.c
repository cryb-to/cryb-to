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

#include <err.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <cryb/test.h>

/* program name */
const char *t_progname;

/* verbose flag */
static int verbose;

/* whether to check for leaks */
static int leaktest;

/*
 * If verbose flag is set, print an array of bytes in hex
 */
void
t_verbose_hex(const uint8_t *buf, size_t len)
{

	if (verbose) {
		while (len--) {
			fprintf(stderr, "%02x", *buf++);
			if (len > 0 && len % 4 == 0)
				fprintf(stderr, " ");
		}
	}
}

/*
 * If verbose flag is set, print a message
 */
void
t_verbose(const char *fmt, ...)
{
	va_list ap;

	if (verbose) {
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}

/*
 * Assertion failed - test cannot proceed
 */
void
t_assertion_failed(const char *func, const char *file, unsigned int line,
    const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "assertion failed in %s() on %s:%u\n",
	    func, file, line);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	raise(SIGABRT);
	_exit(1);
}

/*
 * Test plan
 */
static struct t_test **t_plan;
static size_t t_plan_len, t_plan_size;

/*
 * Grow the test plan to accomodate at least n more entries.
 */
static void
t_grow(int n)
{
	struct t_test **p;

	if (t_plan_len + n < t_plan_size)
		return;
	if (t_plan_size == 0)
		t_plan_size = 16;
	while (t_plan_len + n >= t_plan_size)
		t_plan_size *= 2;
	if ((p = realloc(t_plan, t_plan_size * sizeof *p)) == NULL)
		err(1, "realloc()");
	t_plan = p;
}

/*
 * Add a single entry to the test plan.
 */
void
t_add_test(t_func *func, void *arg, const char *fmt, ...)
{
	struct t_test *t;
	va_list ap;

	if ((t = malloc(sizeof *t)) == NULL)
		err(1, "malloc()");
	t->func = func;
	va_start(ap, fmt);
	(void)vasprintf(&t->desc, fmt, ap);
	va_end(ap);
	t->arg = arg;
	t_grow(1);
	t_plan[t_plan_len++] = t;
	t_plan[t_plan_len] = NULL;
}

/*
 * Add multiple entries to the test plan.
 */
void
t_add_tests(struct t_test *t, int n)
{
	int i;

	t_grow(n);
	for (i = 0; i < n; ++i)
		t_plan[t_plan_len++] = &t[i];
	t_plan[t_plan_len] = NULL;
}

/*
 * Signal handler for tests
 */
static jmp_buf sigjmp;
static int sigs[] = {
	/* it is not safe to catch SIGABRT if raised by abort(3) */
	/* SIGABRT, */
	SIGBUS, SIGSEGV, SIGSYS, SIGPIPE, SIGALRM, SIGTERM,
	0
};

static void
t_handle_signal(int signo)
{

	longjmp(sigjmp, signo);
}

/*
 * Run a single test
 */
static int
t_run_test(struct t_test *t, int n)
{
	unsigned long snap1[16], snap2[16];
	size_t snaplen;
	char *desc;
	int i, ret, signo;

	if (leaktest && verbose)
		t_malloc_snapshot(snap1, sizeof snap1);
	for (i = 0; sigs[i] > 0; ++i)
		signal(sigs[i], t_handle_signal);
	desc = t->desc;
	ret = 0;
	if ((signo = setjmp(sigjmp)) == 0)
		ret = (*t->func)(&desc, t->arg);
	if (t_malloc_fail != 0) {
		t_verbose("WARNING: test case left t_malloc_fail = %d\n",
		    t_malloc_fail);
		t_malloc_fail = 0;
	}
	if (t_malloc_fail_after != 0) {
		t_verbose("WARNING: test case left t_malloc_fail_after = %d\n",
		    t_malloc_fail_after);
		t_malloc_fail_after = 0;
	}
	if (ret > 0)
		printf("ok %d - %s\n", n, desc ? desc : "no description");
	else if (ret < 0)
		printf("ok %d - # skip\n", n);
	else if (signo == 0)
		printf("not ok %d - %s\n", n, desc ? desc : "no description");
	else
		printf("not ok %d - caught signal %d\n", n, signo);
	if (desc != t->desc)
		free(desc);
	for (i = 0; sigs[i] > 0; ++i)
		signal(sigs[i], SIG_DFL);
	if (leaktest && verbose) {
		snaplen = t_malloc_snapshot(snap2, sizeof snap2);
		if (snaplen > sizeof snap2)
			snaplen = sizeof snap2;
		t_compare_mem(snap1, snap2, snaplen);
		if (memcmp(snap1, snap2, snaplen) != 0)
			t_verbose("WARNING: allocator state changed\n");
	}
	return (ret);
}

/*
 * Print usage string and exit.
 */
static void
usage(void)
{

	fprintf(stderr, "usage: %s [-Llv]\n", t_progname);
	exit(1);
}

void
t_main(t_prepare_func t_prepare, t_cleanup_func t_cleanup,
    int argc, char *argv[])
{
	unsigned int n, pass, fail;
	size_t nt;
	int opt;

	/* make all unintentional allocation failures fatal */
	t_malloc_fatal = 1;

	/* make stdout line-buffered to preserve ordering */
	setvbuf(stdout, NULL, _IOLBF, 0);

	/* clean up temp files in case of premature exit */
	atexit(t_fcloseall);

	/* determine our own name, for naming temp files etc. */
	if ((t_progname = strrchr(argv[0], '/')) != NULL)
		t_progname++; /* one past the slash */
	else
		t_progname = argv[0];

	/* check for leaks, unless doing coverage analysis */
#if CRYB_COVERAGE
	leaktest = t_str_is_true(getenv("CRYB_LEAKTEST"));
#else
	leaktest = !t_str_is_false(getenv("CRYB_LEAKTEST"));
#endif

	/* parse command line options */
	while ((opt = getopt(argc, argv, "Llv")) != -1)
		switch (opt) {
		case 'L':
			leaktest = 0;
			break;
		case 'l':
			leaktest = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			usage();
		}

	argc -= optind;
	argv += optind;

	/* prepare the test plan */
	if (t_prepare != NULL)
		t_prepare(argc, argv);
	if (t_plan_len == 0)
		errx(1, "no plan");

	/* run the tests */
	nt = leaktest ? t_plan_len + 1 : t_plan_len;
	printf("1..%zu\n", nt);
	for (n = pass = fail = 0; n < t_plan_len; ++n)
		t_run_test(t_plan[n], n + 1) ? ++pass : ++fail;

	/* clean up as much as possible before we exit */
	if (t_cleanup != NULL)
		t_cleanup();
	t_fcloseall();
	for (n = 0; n < t_plan_len; ++n) {
		free(t_plan[n]->desc);
		free(t_plan[n]);
	}
	free(t_plan);
	setvbuf(stdout, NULL, _IONBF, 0);
	if (leaktest) {
		if (verbose)
			t_malloc_printstats(stderr);
		t_run_test(&t_memory_leak, nt) ? ++pass : ++fail;
	}
	t_verbose("%d out of %zd tests passed\n", pass, nt);
	exit(fail > 0 ? 1 : 0);
}
