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

#include <stdint.h>
#include <string.h>

#include <cryb/bitwise.h>
#include <cryb/endian.h>
#include <cryb/hash.h>

/*
 * Simple implementation of the Murmur3-32 hash function.
 *
 * This implementation is slow but safe.  It can be made significantly
 * faster if the caller guarantees that the input is correctly aligned for
 * 32-bit reads, and slightly faster yet if the caller guarantees that the
 * length of the input is always a multiple of 4 bytes.
 */
uint32_t
murmur3_32_hash(const void *data, size_t len, uint32_t seed)
{
	const uint8_t *bytes;
	uint32_t hash, k;
	size_t res;

	/* initialization */
	bytes = data;
	res = len;
	hash = seed;

	/* main loop */
	while (res >= 4) {
		/* replace with le32toh() if input is aligned */
		k = le32dec(bytes);
		bytes += 4;
		res -= 4;
		k *= 0xcc9e2d51;
		k = rol32(k, 15);
		k *= 0x1b873593;
		hash ^= k;
		hash = rol32(hash, 13);
		hash *= 5;
		hash += 0xe6546b64;
	}

	/* remainder */
	/* remove if input length is a multiple of 4 */
	if (res > 0) {
		k = 0;
		switch (res) {
		case 3:
			k |= bytes[2] << 16;
		case 2:
			k |= bytes[1] << 8;
		case 1:
			k |= bytes[0];
			k *= 0xcc9e2d51;
			k = rol32(k, 15);
			k *= 0x1b873593;
			hash ^= k;
			break;
		CRYB_NO_DEFAULT_CASE;
		}
	}

	/* finalize */
	hash ^= (uint32_t)len;
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;
	return (hash);
}
