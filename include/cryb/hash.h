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

#ifndef CRYB_HASH_H_INCLUDED
#define CRYB_HASH_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

const char *cryb_hash_version(void);

#define adler32_hash		cryb_adler32_hash
#define fletcher16_hash		cryb_fletcher16_hash
#define fletcher32_hash		cryb_fletcher32_hash
#define fletcher64_hash		cryb_fletcher64_hash
#define murmur3_32_hash		cryb_murmur3_32_hash
#define pearson_hash		cryb_pearson_hash
#define pearson_hash_str	cryb_pearson_hash_str

uint32_t adler32_hash(const void *, size_t);
uint16_t fletcher16_hash(const void *, size_t);
uint32_t fletcher32_hash(const void *, size_t);
uint64_t fletcher64_hash(const void *, size_t);
uint32_t murmur3_32_hash(const void *, size_t, uint32_t);
uint8_t pearson_hash(const void *, size_t);
uint8_t pearson_hash_str(const char *);

#endif
