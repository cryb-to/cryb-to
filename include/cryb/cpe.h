/*-
 * Copyright (c) 2014-2016 Dag-Erling Smørgrav
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

#ifndef CRYB_CPE_H_INCLUDED
#define CRYB_CPE_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

CRYB_BEGIN

const char *cryb_cpe_version(void);

#define cpe_name	cryb_cpe_name

#define cpe22_ver	202
enum cpe22_attributes {
	/* CPE 2.2 attributes */
	cpe22_part,
	cpe22_vendor,
	cpe22_product,
	cpe22_version,
	cpe22_update,
	cpe22_edition,
	cpe22_language,
	/* max */
	cpe22_nattr
};

#define cpe23_ver	203
enum cpe23_attributes {
	/* CPE 2.2 attributes */
	cpe23_part	= cpe22_part,
	cpe23_vendor	= cpe22_vendor,
	cpe23_product	= cpe22_product,
	cpe23_version	= cpe22_version,
	cpe23_update	= cpe22_update,
	cpe23_edition	= cpe22_edition,
	cpe23_language	= cpe22_language,
	/* CPE 2.3 extended attributes */
	cpe23_sw_edition,
	cpe23_target_sw,
	cpe23_target_hw,
	cpe23_other,
	/* max */
	cpe23_nattr
};

typedef struct cpe_name {
	int	 ver;
	int	 nattr;
	wchar_t	*attr[];
} cpe_name;

#define cpe_clone		cryb_cpe_clone
#define cpe_copy_attr		cryb_cpe_copy_attr
#define cpe_destroy		cryb_cpe_destroy
#define cpe_new			cryb_cpe_new
#define cpe_bind_to_fs		cryb_cpe_bind_to_fs
#define cpe_unbind_fs		cryb_cpe_unbind_fs
#define cpe_upgrade22		cryb_cpe_upgrade22
#define cpe_upgrade		cryb_cpe_upgrade
#define cpe_bind_to_uri		cryb_cpe_bind_to_uri
#define cpe_unbind_uri		cryb_cpe_unbind_uri
#define cpe_from_wfn		cryb_cpe_from_wfn
#define cpe_to_wfn		cryb_cpe_to_wfn

cpe_name *cpe_new(void);
void cpe_destroy(cpe_name *);
cpe_name *cpe_clone(const cpe_name *);

cpe_name *cpe_upgrade(const cpe_name *);

cpe_name *cpe_unbind_fs(const wstring *);
wstring *cpe_bind_to_fs(const cpe_name *);

cpe_name *cpe_unbind_uri(const wstring *);
wstring *cpe_bind_to_uri(const cpe_name *);

cpe_name *cpe_from_wfn(const wchar_t *);
wchar_t *cpe_to_wfn(const cpe_name *);

/* not part of public API */
int cpe_copy_attr(cpe_name *, const cpe_name *, int, int);
cpe_name *cpe_upgrade22(const cpe_name *);

CRYB_END

#endif
