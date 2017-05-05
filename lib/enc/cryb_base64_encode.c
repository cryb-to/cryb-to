/*-
 * Copyright (c) 2013-2014 The University of Oslo
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
#include <stddef.h>
#include <stdint.h>

#include <cryb/rfc4648.h>

static const char b64enc[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/*
 * Encode data in RFC 4648 base 64 representation.  The target buffer must
 * have room for base64_enclen(len) characters and a terminating NUL.
 */
int
base64_encode(const uint8_t *in, size_t ilen, char *out, size_t *olen)
{
	uint32_t bits;

	if (*olen <= base64_enclen(ilen)) {
		*olen = base64_enclen(ilen) + 1;
		errno = ENOSPC;
		return (-1);
	}
	*olen = base64_enclen(ilen) + 1;
	while (ilen >= 3) {
		bits = 0;
		bits |= (uint32_t)*in++ << 16;
		bits |= (uint32_t)*in++ << 8;
		bits |= (uint32_t)*in++;
		ilen -= 3;
		*out++ = b64enc[bits >> 18 & 0x3f];
		*out++ = b64enc[bits >> 12 & 0x3f];
		*out++ = b64enc[bits >> 6 & 0x3f];
		*out++ = b64enc[bits & 0x3f];
	}
	if (ilen > 0) {
		bits = 0;
		switch (ilen) {
		case 2:
			bits |= (uint32_t)in[1] << 8;
		case 1:
			bits |= (uint32_t)in[0] << 16;
		CRYB_NO_DEFAULT_CASE;
		}
		*out++ = b64enc[bits >> 18 & 0x3f];
		*out++ = b64enc[bits >> 12 & 0x3f];
		*out++ = ilen > 1 ? b64enc[bits >> 6 & 0x3f] : '=';
		*out++ = '=';
	}
	*out++ = '\0';
	return (0);
}
