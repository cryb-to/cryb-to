/*-
 * Copyright (c) 2015 Dag-Erling Smørgrav
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

#include <stdint.h>
#include <string.h>

#include <cryb/bitwise.h>
#include <cryb/endian.h>
#include <cryb/hash.h>

/*
 * Simple implementation of Fletcher's checksum (64-bit version).  The
 * input length is zero-padded to the nearest multiple of 4 bytes.
 */
uint64_t
fletcher64_hash(const void *data, size_t len)
{
	const uint8_t *bytes;
	uint64_t c0, c1;
	uint32_t w;

	for (bytes = data, c0 = c1 = 0; len > 3; len -= 4, bytes += 4) {
		/* replace with be32toh() if input is aligned */
		w = be32dec(bytes);
		c0 = (c0 + w) % 0xffffffffU;
		c1 = (c1 + c0) % 0xffffffffU;
	}
	if (len > 0) {
		w = bytes[0] << 24;
		if (len > 1)
			w |= bytes[1] << 16;
		if (len > 2)
			w |= bytes[2] << 8;
		c0 = (c0 + w) % 0xffffffffU;
		c1 = (c1 + c0) % 0xffffffffU;
	}
	return (c1 << 32 | c0);
}
