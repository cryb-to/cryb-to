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

#include <sys/types.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <cryb/test.h>

/*
 * This test isn't very meaningful, but it does exercise the allocator.
 * It's a little difficult to create meaningful tests when the code you're
 * testing is the same as the code that runs your tests.
 */
static int
t_malloc_fill(char **desc CRYB_UNUSED, void *arg CRYB_UNUSED)
{
	void *head, **next;
	unsigned int i;
	int ret, stmf;

	ret = 1;
	stmf = t_malloc_fatal;
	t_malloc_fatal = 0;
	head = NULL;
	for (i = 0; (next = malloc(sizeof *next)) != NULL; ++i) {
		*next = head;
		head = next;
	}
	t_printv("%u successful allocations\n", i);
	for (; (next = head) != NULL; --i) {
		head = *next;
		free(next);
	}
	ret &= t_compare_u(0, i);
	t_malloc_fatal = stmf;
	return (ret);
}

static int
t_prepare(int argc, char *argv[])
{

	(void)argc;
	(void)argv;
	t_add_test(t_malloc_fill, NULL, "fill a bucket");
	return (0);
}

int
main(int argc, char *argv[])
{

	t_main(t_prepare, NULL, argc, argv);
}
