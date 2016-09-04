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

#include "cryb/impl.h"

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <cryb/ctype.h>
#include <cryb/rfc3986.h>

#define unhex(ch)							\
	((ch >= '0' && ch <= '9') ? ch - '0' :				\
	 (ch >= 'A' && ch <= 'F') ? 0xa + ch - 'A' :			\
	 (ch >= 'a' && ch <= 'f') ? 0xa + ch - 'a' : 0)

/*
 * Decodes a string in RFC 3986 percent-encoded representation.
 */
int
percent_decode(const char *in, size_t ilen, char *out, size_t *olen)
{
	size_t len;

	for (len = 0; ilen > 0 && *in != '\0'; --ilen, ++in) {
		if (*in != '%') {
			if (++len < *olen && out != NULL)
				*out++ = *in;
		} else if (ilen >= 3 && is_xdigit(in[1]) && is_xdigit(in[2])) {
			if (++len < *olen && out != NULL)
				*out++ = unhex(in[1]) << 4 | unhex(in[2]);
			in += 2;
			ilen -= 2;
		} else {
			if (*olen > 0 && out != NULL)
				*out = '\0';
			*olen = ++len;
			errno = EINVAL;
			return (-1);
		}
	}
	if (*olen > 0 && out != NULL)
		*out = '\0';
	if (++len > *olen && out != NULL) {
		/* overflow */
		*olen = len;
		errno = ENOSPC;
		return (-1);
	}
	*olen = len;
	return (0);
}
