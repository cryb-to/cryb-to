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

static int
t_noop(char **desc CRYB_UNUSED, void *arg)
{
	string *str;

	(void)arg;
	if ((str = string_new()) == NULL)
		return (0);
	string_delete(str);
	return (1);
}


/***************************************************************************
 * Comparisons
 */

static struct t_compare_case {
	const char *desc;
	const char_t *s1, *s2;
	int cmp;
} t_compare_cases[] = {
	{
		"empty with empty",
		CS(""),
		CS(""),
		0,
	},
	{
		"empty with non-empty",
		CS(""),
		CS("xyzzy"),
		-1,
	},
	{
		"non-empty with empty",
		CS("xyzzy"),
		CS(""),
		1,
	},
	{
		"non-empty with same non-empty",
		CS("xyzzy"),
		CS("xyzzy"),
		0,
	},
	{
		"non-empty with later non-empty",
		CS("abba"),
		CS("baba"),
		-1,
	},
	{
		"non-empty with earlier non-empty",
		CS("baba"),
		CS("abba"),
		1,
	},
	{
		"non-empty prefix with non-empty",
		CS("baba"),
		CS("babaorum"),
		-1,
	},
	{
		"non-empty with non-empty prefix",
		CS("babaorum"),
		CS("baba"),
		1,
	},
};

static int
t_compare_test(char **desc CRYB_UNUSED, void *arg)
{
	struct t_compare_case *t = arg;
	string *s1, *s2;
	int ret;

	if ((s1 = string_new()) == NULL ||
	    string_append_cs(s1, t->s1, SIZE_MAX) < 0 ||
	    (s2 = string_new()) == NULL ||
	    string_append_cs(s2, t->s2, SIZE_MAX) < 0)
		return (0);
	ret = string_compare(s1, s2) == t->cmp;
	string_delete(s2);
	string_delete(s1);
	return (ret);
}

static int
t_equal_test(char **desc CRYB_UNUSED, void *arg)
{
	struct t_compare_case *t = arg;
	string *s1, *s2;
	int ret;

	if ((s1 = string_new()) == NULL ||
	    string_append_cs(s1, t->s1, SIZE_MAX) < 0 ||
	    (s2 = string_new()) == NULL ||
	    string_append_cs(s2, t->s2, SIZE_MAX) < 0)
		return (0);
	ret = (string_equal(s1, s2) == 0) == (t->cmp != 0);
	string_delete(s2);
	string_delete(s1);
	return (ret);
}


/***************************************************************************
 * Boilerplate
 */

int
t_prepare(int argc, char *argv[])
{
	unsigned int i;

	(void)argc;
	(void)argv;

	t_add_test(t_noop, NULL, "no-op");
	for (i = 0; i < sizeof t_compare_cases / sizeof *t_compare_cases; ++i)
		t_add_test(t_compare_test, &t_compare_cases[i],
		    "%s(%s)", "string_compare", t_compare_cases[i].desc);
	for (i = 0; i < sizeof t_compare_cases / sizeof *t_compare_cases; ++i)
		t_add_test(t_equal_test, &t_compare_cases[i],
		    "%s(%s)", "string_equal", t_compare_cases[i].desc);
	return (0);
}

void
t_cleanup(void)
{
}
