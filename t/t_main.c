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

#include "cryb/impl.h"

#include <err.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "t.h"

/* program name */
const char *t_progname;

/* verbose flag */
static int verbose;

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
	vasprintf(&t->desc, fmt, ap);
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
 * Print usage string and exit.
 */
static void
usage(void)
{

	fprintf(stderr, "usage: %s [-v]\n", t_progname);
	exit(1);
}

int
main(int argc, char *argv[])
{
	unsigned int n, pass, fail;
	char *desc;
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

	/* parse command line options */
	while ((opt = getopt(argc, argv, "v")) != -1)
		switch (opt) {
		case 'v':
			verbose = 1;
			break;
		default:
			usage();
		}

	argc -= optind;
	argv += optind;

	/* prepare the test plan */
	t_prepare(argc, argv);
	if (t_plan_len == 0)
		errx(1, "no plan\n");

	/* run the tests */
	printf("1..%zu\n", t_plan_len);
	for (n = pass = fail = 0; n < t_plan_len; ++n) {
		desc = t_plan[n]->desc ? t_plan[n]->desc : "no description";
		if ((*t_plan[n]->func)(&desc, t_plan[n]->arg)) {
			printf("ok %d - %s\n", n + 1, desc);
			++pass;
		} else {
			printf("not ok %d - %s\n", n + 1, desc);
			++fail;
		}
	}

	/* clean up and exit */
	t_cleanup();
	for (n = 0; n < t_plan_len; ++n) {
		free(t_plan[n]->desc);
		free(t_plan[n]);
	}
	free(t_plan);
	if (verbose)
		t_malloc_printstats(stderr);
	exit(fail > 0 ? 1 : 0);
}
