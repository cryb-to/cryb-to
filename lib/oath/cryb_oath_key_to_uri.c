/*-
 * Copyright (c) 2013 The University of Oslo
 * Copyright (c) 2018 Dag-Erling Smørgrav
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

#include <cryb/rfc3986.h>
#include <cryb/rfc4648.h>
#include <cryb/oath.h>

static inline void
append_char(char *buf, size_t size, size_t *pos, int ch)
{

	if (*pos + 1 < size)
		buf[*pos] = ch;
	(*pos)++;
	if (*pos < size)
		buf[*pos] = '\0';
}

static inline void
append_str(char *buf, size_t size, size_t *pos, const char *str)
{

	while (*str != '\0') {
		if (*pos + 1 < size)
			buf[*pos] = *str++;
		(*pos)++;
	}
	if (*pos < size)
		buf[*pos] = '\0';
}

static inline void
append_num(char *buf, size_t size, size_t *pos, uintmax_t num)
{
	char numbuf[32], *p;

	p = numbuf + sizeof numbuf - 1;
	*p-- = '\0';
	do {
		*p-- = '0' + num % 10;
		num /= 10;
	} while (num > 0);
	append_str(buf, size, pos, p + 1);
}

static inline void
append_percent(char *buf, size_t size, size_t *pos, const char *str)
{
	size_t res;

	res = *pos < size ? size - *pos : 0;
	percent_encode(str, SIZE_MAX, buf + *pos, &res);
	*pos += res - 1;
}

static inline void
append_base32(char *buf, size_t size, size_t *pos,
    const uint8_t *data, size_t len)
{
	size_t res;

	res = *pos < size ? size - *pos : 0;
	base32_encode(data, len, buf + *pos, &res);
	*pos += res - 1;
}

int
oath_key_to_uri(const oath_key *ok, char *buf, size_t *size)
{
	const char *mode, *hash;
	size_t pos;

	pos = 0;
	append_str(buf, *size, &pos, "otpauth://");
	switch (ok->mode) {
	case om_hotp:
		mode = "hotp";
		break;
	case om_totp:
		mode = "totp";
		break;
	default:
		return (0);
	}
	append_str(buf, *size, &pos, mode);
	append_char(buf, *size, &pos, '/');
	append_percent(buf, *size, &pos, ok->label);
	append_str(buf, *size, &pos, "?algorithm=");
	switch (ok->hash) {
	case oh_sha1:
		hash = "SHA1";
		break;
	case oh_sha256:
		hash = "SHA256";
		break;
	case oh_sha512:
		hash = "SHA512";
		break;
	case oh_md5:
		hash = "MD5";
		break;
	default:
		return (0);
	}
	append_str(buf, *size, &pos, hash);
	append_str(buf, *size, &pos, "&digits=");
	append_num(buf, *size, &pos, ok->digits);
	if (ok->mode == om_hotp) {
		append_str(buf, *size, &pos, "&counter=");
		append_num(buf, *size, &pos, (uintmax_t)ok->counter);
	} else if (ok->mode == om_totp) {
		append_str(buf, *size, &pos, "&period=");
		append_num(buf, *size, &pos, (uintmax_t)ok->timestep);
		append_str(buf, *size, &pos, "&lastused=");
		append_num(buf, *size, &pos, (uintmax_t)ok->lastused);
	} else {
		return (0);
	}
	if (ok->issuerlen > 0) {
		append_str(buf, *size, &pos, "&issuer=");
		append_percent(buf, *size, &pos, ok->issuer);
	}
	append_str(buf, *size, &pos, "&secret=");
	append_base32(buf, *size, &pos, ok->key, ok->keylen);
	pos++; // terminating NUL
	if (pos > *size) {
		*size = pos;
		errno = ENOSPC;
		return (-1);
	}
	*size = pos;
	return (0);
}
