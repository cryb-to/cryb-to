/*-
 * Copyright (c) 2016 Dag-Erling Smørgrav
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
#include <sys/wait.h>

#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cryb/assert.h>

#include <cryb/test.h>

/*
 * Verify that an assertion failure causes a SIGABRT.  Note that we can't
 * just catch the signal, because abort(3) has other side effects (such as
 * flushing and closing streams) from which we couldn't recover.
 */
static void
t_assert_child(void)
{
#if defined(HAVE_SETRLIMIT) && defined(RLIMIT_CORE)
	struct rlimit crl = { 0, 0 };
#endif

	/* reset SIGABRT handler in case of shenanigans */
	signal(SIGABRT, SIG_DFL);
#if defined(HAVE_SETRLIMIT) && defined(RLIMIT_CORE)
	/* prevent core dump */
	if (setrlimit(RLIMIT_CORE, &crl) != 0)
		t_verbose("failed to disable core dump\n");
#endif
	/* suppress assertion message */
	fclose(stderr);
	assert(0);
}

int
t_assert_fail(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	pid_t pid;
	int signo, status;

	t_assert((pid = fork()) != -1);
	if (pid == 0) {
		t_assert_child();
		_exit(1);
	}
	t_assert(waitpid(pid, &status, 0) == pid);
	if (!WIFSIGNALED(status)) {
		t_verbose("expected child to raise a signal\n");
		return (0);
	}
#ifdef WCOREDUMP
	if (WCOREDUMP(status))
		t_verbose("warning: child dumped core\n");
#endif
	if ((signo = WTERMSIG(status)) != SIGABRT) {
		t_verbose("expected child to raise signal %d (SIGABRT), "
		    "got signal %d\n", SIGABRT, signo);
		return (0);
	}
	return (1);
}


/***************************************************************************
 * Boilerplate
 */

static int
t_prepare(int argc, char *argv[])
{

	(void)argc;
	(void)argv;
	t_add_test(t_assert_fail, NULL, "assertion failed");
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
