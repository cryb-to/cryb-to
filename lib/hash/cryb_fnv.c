/*-
 * Copyright (c) 2022 Dag-Erling Smørgrav
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

#include <stddef.h>
#include <stdint.h>

#include <cryb/hash.h>

#include "cryb_fnv_impl.h"

/*
 * Implementations of the 32- and 64-bit Fowler-Noll-Vo (FNV) 0, 1, and 1a hashes.
 */

uint32_t fnv0_32_hash(const void *data, size_t len)
{
	const uint8_t *p;
	uint32_t h;

	for (p = data, h = 0; len > 0; ++p, --len) {
		h *= FNV_32_PRIME;
		h ^= *p;
	}
	return (h);
}

uint64_t fnv0_64_hash(const void *data, size_t len)
{
	const uint8_t *p;
	uint64_t h;

	for (p = data, h = 0; len > 0; ++p, --len) {
		h *= FNV_64_PRIME;
		h ^= *p;
	}
	return (h);
}

uint32_t fnv1_32_hash(const void *data, size_t len)
{
	const uint8_t *p;
	uint32_t h;

	for (p = data, h = FNV_32_OFFSET_BASIS; len > 0; ++p, --len) {
		h *= FNV_32_PRIME;
		h ^= *p;
	}
	return (h);
}

uint64_t fnv1_64_hash(const void *data, size_t len)
{
	const uint8_t *p;
	uint64_t h;

	for (p = data, h = FNV_64_OFFSET_BASIS; len > 0; ++p, --len) {
		h *= FNV_64_PRIME;
		h ^= *p;
	}
	return (h);
}

uint32_t fnv1a_32_hash(const void *data, size_t len)
{
	const uint8_t *p;
	uint32_t h;

	for (p = data, h = FNV_32_OFFSET_BASIS; len > 0; ++p, --len) {
		h ^= *p;
		h *= FNV_32_PRIME;
	}
	return (h);
}

uint64_t fnv1a_64_hash(const void *data, size_t len)
{
	const uint8_t *p;
	uint64_t h;

	for (p = data, h = FNV_64_OFFSET_BASIS; len > 0; ++p, --len) {
		h ^= *p;
		h *= FNV_64_PRIME;
	}
	return (h);
}
