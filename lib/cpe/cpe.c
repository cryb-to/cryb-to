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

#include <stdlib.h>
#include <wchar.h>

#include <cryb/cpe.h>

/*
 * Free all memory allocated to a cpe structure.
 */
void
cpe_destroy(struct cpe_name *cpe)
{

	if (cpe == NULL)
		return;
	for (int i = 0; i < cpe->nattr; ++i)
		free(cpe->attr[i]);
	free(cpe);
}

/*
 * Copy one or more attributes from a source cpe structure to a
 * destination cpe structure.  If an attribute in the source is NULL, the
 * corresponding attribute in the destination will be an empty string.
 */
static int
cpe_copy_attr(struct cpe_name *dst, const struct cpe_name *src, int base, int nattr)
{

	if (base < 0 || nattr < 0 || base + nattr > src->nattr ||
	    base + nattr > dst->nattr)
		return (-1);
	if (nattr == 0)
		nattr = src->nattr - base;
	for (int i = base; i < base + nattr; ++i) {
		if (src->attr[i] != NULL)
			dst->attr[i] = wcsdup(src->attr[i]);
		else
			dst->attr[i] = wcsdup(L"");
		if (dst->attr[i] == NULL)
			return (-1);
	}
	return (nattr);
}

/*
 * Duplicate a cpe structure.  Any attributes that are NULL in the source
 * will be empty strings in the destination.
 */
struct cpe_name *
cpe_clone(const struct cpe_name *cpe)
{
	struct cpe_name *ncpe;

	ncpe = calloc(1, sizeof *ncpe +
	    cpe->nattr * sizeof *ncpe->attr);
	if (ncpe == NULL)
		return (NULL);
	if (cpe_copy_attr(ncpe, cpe, 0, 0) < 0) {
		cpe_destroy(ncpe);
		return (NULL);
	}
	return (ncpe);		
}

/*
 * Allocate a new cpe structure.
 */
struct cpe_name *
cpe_new(void)
{
	struct cpe_name *ncpe;

	if ((ncpe = calloc(1, sizeof *ncpe)) == NULL)
		return (NULL);
	ncpe->ver = cpe23_ver;
	ncpe->nattr = cpe23_nattr;
	return (ncpe);
}

/*
 * Upgrade a cpe 2.2 structure to the latest supported version.
 */
struct cpe_name *
cpe_upgrade22(const struct cpe_name *cpe)
{
	struct cpe_name *ncpe;

	if ((ncpe = cpe_new()) == NULL)
		return (NULL);
	/* copy existing attributes */
	if (cpe_copy_attr(ncpe, cpe, 0, cpe22_nattr) < 0) {
		cpe_destroy(ncpe);
		return (NULL);
	}
	/* extended attributes? */
	if (ncpe->attr[cpe22_edition][0] == L'~') {
		/*
		 * XXX pseudo-code:
		 *
		 * - Split into fields (return an error if there are more
                 *   than four)
		 * - Assign these to sw_edition, target_sw, target_hw and
                 *   other, in that order.
		 */
	}
	return (ncpe);
}

/*
 * Upgrade a cpe structure to the latest supported version.
 */
struct cpe_name *
cpe_upgrade(const struct cpe_name *cpe)
{

	switch (cpe->ver) {
	case cpe22_ver:
		/* already latest */
		return (cpe_clone(cpe));
	case cpe23_ver:
		return (cpe_upgrade22(cpe));
	default:
		return (NULL);
	}
}
