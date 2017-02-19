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

#ifndef CRYB_WSTRING_H_INCLUDED
#define CRYB_WSTRING_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

CRYB_BEGIN

#define wstring_new		cryb_wstring_new
#define wstring_len		cryb_wstring_len
#define wstring_buf		cryb_wstring_buf
#define wstring_dup		cryb_wstring_dup
#define wstring_dup_wcs		cryb_wstring_dup_wcs
#define wstring_delete		cryb_wstring_delete
#define wstring_expand		cryb_wstring_expand
#define wstring_shrink		cryb_wstring_shrink
#define wstring_trunc		cryb_wstring_trunc
#define wstring_append_wc	cryb_wstring_append_wc
#define wstring_append_wcs	cryb_wstring_append_wcs
#define wstring_append_wstring	cryb_wstring_append_wstring
#define wstring_printf		cryb_wstring_printf
#define wstring_vprintf		cryb_wstring_vprintf
#define wstring_compare		cryb_wstring_compare
#define wstring_compare_wcs	cryb_wstring_compare_wcs
#define wstring_equal		cryb_wstring_equal
#define wstring_equal_wcs	cryb_wstring_equal_wcs

typedef struct cryb_wstring wstring;

wstring	*wstring_new(void);
size_t	 wstring_len(const wstring *);
const wchar_t *wstring_buf(const wstring *);
wstring	*wstring_dup(const wstring *);
wstring	*wstring_dup_wcs(const wchar_t *, size_t);
void	 wstring_delete(wstring *);
int	 wstring_expand(wstring *, size_t);
void	 wstring_shrink(wstring *);
ssize_t	 wstring_trunc(wstring *, size_t);
ssize_t	 wstring_append_wc(wstring *, wchar_t);
ssize_t	 wstring_append_wcs(wstring *, const wchar_t *, size_t);
ssize_t	 wstring_append_wstring(wstring *, const wstring *, size_t);
ssize_t	 wstring_printf(wstring *, const wchar_t *, ...);
#ifdef va_start
ssize_t	 wstring_vprintf(wstring *, const wchar_t *, va_list);
#endif
int	 wstring_compare(const wstring *, const wstring *);
int	 wstring_compare_wcs(const wstring *, const wchar_t *, size_t);
int	 wstring_equal(const wstring *, const wstring *);
int	 wstring_equal_wcs(const wstring *, const wchar_t *, size_t);

CRYB_END

#endif
