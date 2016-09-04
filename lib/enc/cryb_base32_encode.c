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

#include <sys/types.h>

#include <errno.h>
#include <stdint.h>

#include <cryb/rfc4648.h>

static const char b32enc[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

/*
 * Encode data in RFC 4648 base 32 representation.  The target buffer must
 * have room for base32_enclen(len) characters and a terminating NUL.
 */
int
base32_encode(const uint8_t *in, size_t ilen, char *out, size_t *olen)
{
	uint64_t bits;

	if (*olen <= base32_enclen(ilen)) {
		*olen = base32_enclen(ilen) + 1;
		errno = ENOSPC;
		return (-1);
	}
	*olen = base32_enclen(ilen) + 1;
	while (ilen >= 5) {
		bits = 0;
		bits |= (uint64_t)*in++ << 32;
		bits |= (uint64_t)*in++ << 24;
		bits |= (uint64_t)*in++ << 16;
		bits |= (uint64_t)*in++ << 8;
		bits |= (uint64_t)*in++;
		*out++ = b32enc[bits >> 35 & 0x1f];
		*out++ = b32enc[bits >> 30 & 0x1f];
		*out++ = b32enc[bits >> 25 & 0x1f];
		*out++ = b32enc[bits >> 20 & 0x1f];
		*out++ = b32enc[bits >> 15 & 0x1f];
		*out++ = b32enc[bits >> 10 & 0x1f];
		*out++ = b32enc[bits >> 5 & 0x1f];
		*out++ = b32enc[bits & 0x1f];
		ilen -= 5;
	}
	if (ilen > 0) {
		bits = 0;
		switch (ilen) {
		case 4:
			bits |= (uint64_t)in[3] << 8;
		case 3:
			bits |= (uint64_t)in[2] << 16;
		case 2:
			bits |= (uint64_t)in[1] << 24;
		case 1:
			bits |= (uint64_t)in[0] << 32;
		CRYB_NO_DEFAULT_CASE;
		}
		*out++ = b32enc[bits >> 35 & 0x1f];
		*out++ = b32enc[bits >> 30 & 0x1f];
		*out++ = ilen > 1 ? b32enc[bits >> 25 & 0x1f] : '=';
		*out++ = ilen > 1 ? b32enc[bits >> 20 & 0x1f] : '=';
		*out++ = ilen > 2 ? b32enc[bits >> 15 & 0x1f] : '=';
		*out++ = ilen > 3 ? b32enc[bits >> 10 & 0x1f] : '=';
		*out++ = ilen > 3 ? b32enc[bits >> 5 & 0x1f] : '=';
		*out++ = '=';
	}
	*out++ = '\0';
	return (0);
}
