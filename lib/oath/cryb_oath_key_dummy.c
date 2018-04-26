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

#include <inttypes.h>
#include <string.h>

#include <cryb/oath.h>
#include <cryb/strlcpy.h>

/*
 * Initialize an OATH key with dummy parameters.
 */
int
oath_key_dummy(oath_key *ok, oath_mode mode, oath_hash hash,
    unsigned int digits)
{

	memset(ok, 0, sizeof *ok);
	ok->dummy = 1;
	ok->mode = mode;
	ok->hash = hash;
	ok->digits = digits;
	ok->timestep = 30;
	ok->labellen =
	    strlcpy(ok->issuer, OATH_DUMMY_LABEL, sizeof ok->label);
	ok->issuerlen =
	    strlcpy(ok->issuer, OATH_DUMMY_ISSUER, sizeof ok->issuer);
	ok->keylen = sizeof ok->key;
	memset(ok->key, 0xff, ok->keylen);
	return (0);
}
