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

#include <unistd.h>
#include <wchar.h>

#include <cryb/wstring.h>
#include <cryb/cpe.h>

/*
 * Upgrade a cpe 2.2 structure to the latest supported version.
 */
cpe_name *
cpe_upgrade22(const cpe_name *cpe)
{
	cpe_name *ncpe;

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
