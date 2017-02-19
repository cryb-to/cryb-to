/*-
 * Copyright (c) 2012-2014 The University of Oslo
 * Copyright (c) 2013-2016 Dag-Erling Smørgrav
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

#ifndef OATH_H_INCLUDED
#define OATH_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/oath_constants.h>
#include <cryb/oath_types.h>
#include <cryb/oath_hotp.h>
#include <cryb/oath_totp.h>

CRYB_BEGIN

const char *cryb_oath_version(void);

#define oath_key_alloc		cryb_oath_key_alloc
#define oath_key_create		cryb_oath_key_create
#define oath_key_dummy		cryb_oath_key_dummy
#define oath_key_free		cryb_oath_key_free
#define oath_key_from_uri	cryb_oath_key_from_uri
#define oath_key_to_uri		cryb_oath_key_to_uri
#define oath_mode_name		cryb_oath_mode_name
#define oath_mode_value		cryb_oath_mode_value

struct oath_key *oath_key_alloc(void);
struct oath_key *oath_key_create(const char *, enum oath_mode,
    enum oath_hash, const char *, size_t);
void oath_key_free(struct oath_key *);
struct oath_key *oath_key_from_uri(const char *);
struct oath_key *oath_key_from_file(const char *);
char *oath_key_to_uri(const struct oath_key *);

struct oath_key *oath_key_dummy(enum oath_mode, enum oath_hash, unsigned int);

const char *oath_mode_name(enum oath_mode);
enum oath_mode oath_mode_value(const char *);

CRYB_END

#endif
