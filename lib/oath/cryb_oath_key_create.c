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

#include <stdint.h>
#include <string.h>

#include <cryb/oath.h>
#include <cryb/rand.h>
#include <cryb/strlcpy.h>

/*
 * Initialize an OATH key with the specified parameters.
 */
int
oath_key_create(oath_key *ok,
    oath_mode mode, oath_hash hash, unsigned int digits,
    const char *issuer, const char *label,
    const char *keydata, size_t keylen)
{

	memset(ok, 0, sizeof *ok);

	/* check issuer */
	if (issuer == NULL ||
	    strlcpy(ok->issuer, issuer, sizeof ok->issuer) >= sizeof ok->issuer)
		goto fail;

	/* check label */
	if (label == NULL ||
	    strlcpy(ok->label, label, sizeof ok->label) >= sizeof ok->label)
		goto fail;

	/* check mode */
	switch (mode) {
	case om_totp:
		ok->timestep = 30;
		/* fall through */
	case om_hotp:
		ok->mode = mode;
		break;
	default:
		goto fail;
	}

	/* check hash */
	switch (hash) {
	case oh_undef:
		hash = oh_sha1;
		/* fall through */
	case oh_md5:
	case oh_sha1:
	case oh_sha256:
	case oh_sha512:
		ok->hash = hash;
		break;
	default:
		goto fail;
	}

	/* check digits */
	if (digits == 0)
		digits = OATH_DEF_DIGITS;
	if (digits < OATH_MIN_DIGITS || digits > OATH_MAX_DIGITS)
		goto fail;
	ok->digits = digits;

	/* check key length */
	if (keydata == NULL && keylen == 0)
		keylen = OATH_DEF_KEYLEN;
	if (keylen < OATH_MIN_KEYLEN || keylen > OATH_MAX_KEYLEN)
		goto fail;
	ok->keylen = keylen;

	/* copy or generate key */
	if (keydata != NULL)
		memcpy(ok->key, keydata, ok->keylen);
	else if (rand_bytes(ok->key, ok->keylen) != (ssize_t)ok->keylen)
		goto fail;

	return (0);
fail:
	memset(ok, 0, sizeof *ok);
	return (-1);
}
