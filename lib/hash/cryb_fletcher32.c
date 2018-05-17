/*-
 * Copyright (c) 2015-2016 Dag-Erling Smørgrav
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
 * Simple implementation of the 32-bit variant of Fletcher's checksum,
 * described in Fletcher, J. G. (January 1982), "An Arithmetic Checksum
 * for Serial Transmissions", IEEE Transactions on Communications. COM-30
 * (1): 247–252, doi:10.1109/tcom.1982.1095369
 *
 * The paper assumes that the input length is a multiple of the checksum
 * algorithm's word size (half the checksum size).  This implementation
 * will zero-pad the input up to the nearest multiple of the word size.
 */
#if 0
uint32_t
fletcher32_hash(const void *data, size_t len)
{
	const uint8_t *bytes;
	uint32_t c0, c1;
	uint16_t w;

	for (bytes = data, c0 = c1 = 0; len > 1; len -= 2, bytes += 2) {
		/* replace with be16toh() if input is aligned */
		w = be16dec(bytes);
		c0 = (c0 + w) % 0xffffU;
		c1 = (c1 + c0) % 0xffffU;
	}
	if (len > 0) {
		w = bytes[0] << 8;
		c0 = (c0 + w) % 0xffffffffU;
		c1 = (c1 + c0) % 0xffffffffU;
	}
	return (c1 << 16 | c0);
}
#else
#include <stdio.h>
uint32_t
nakassi32(const uint16_t *data, size_t len)
{
	uint32_t d0, d1;
	unsigned int i;

	for (d0 = d1 = 0; len > 360; len -= 360) {
		for (i = 0; i < 360; ++i) {
			d0 = d0 + be16toh(*data++);
			d1 = d1 + d0;
		}
		d0 = d0 % 0xffffU;
		d1 = d1 % 0xffffU;
	}
	for (i = 0; i < len; ++i) {
		d0 = d0 + be16toh(*data++);
		d1 = d1 + d0;
	}
	d0 = d0 % 0xffffU;
	d1 = d1 % 0xffffU;
	return (d1 << 16 | d0);
}

uint32_t
fletcher32_hash(const void *data, size_t len)
{
	uint32_t c;

	fprintf(stderr, "%s(%p, %zu) ", __func__, data, len);
	c = nakassi32(data, len / 2);
	fprintf(stderr, "= %08x\n", c);
	return (c);
}
#endif
