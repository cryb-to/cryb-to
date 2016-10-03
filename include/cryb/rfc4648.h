/*-
 * Copyright (c) 2012-2013 The University of Oslo
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

#ifndef CRYB_RFC4648_H_INCLUDED
#define CRYB_RFC4648_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

CRYB_BEGIN

/* estimate of output length for base32 encoding / decoding */
#define base32_enclen(l) (size_t)(((l + 4) / 5) * 8)
#define base32_declen(l) (size_t)(((l + 7) / 8) * 5)

/* base32 encoding / decoding */
#define base32_encode cryb_base32_encode
#define base32_decode cryb_base32_decode
int base32_encode(const uint8_t *, size_t, char *, size_t *);
int base32_decode(const char *, size_t, uint8_t *, size_t *);

/* estimate of output length for base64 encoding / decoding */
#define base64_enclen(l) (size_t)(((l + 2) / 3) * 4)
#define base64_declen(l) (size_t)(((l + 3) / 4) * 3)

/* base64 encoding / decoding */
#define base64_encode cryb_base64_encode
#define base64_decode cryb_base64_decode
int base64_encode(const uint8_t *, size_t, char *, size_t *);
int base64_decode(const char *, size_t, uint8_t *, size_t *);

CRYB_END

#endif
