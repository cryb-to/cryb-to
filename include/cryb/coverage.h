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

#ifndef CRYB_COVERAGE_H_INCLUDED
#define CRYB_COVERAGE_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#if _BullseyeCoverage
# define CRYB_COVERAGE		1
# define CRYB_DISABLE_COVERAGE	_Pragma("BullseyeCoverage save off")
# define CRYB_RESTORE_COVERAGE	_Pragma("BullseyeCoverage restore")
#else
# define CRYB_COVERAGE		0
# define CRYB_DISABLE_COVERAGE
# define CRYB_RESTORE_COVERAGE
#endif

/*
 * Some switch constructs may leave you trapped between a static analyzer
 * that complains about the lack of a default case and a dynamic analyzer
 * that complains about dead code.  Use this macro at the end of a switch
 * that does not need a default case.
 */
#define CRYB_NO_DEFAULT_CASE						\
	CRYB_DISABLE_COVERAGE						\
	break; default: do { *(volatile int *)0 = 0; } while (0)	\
	CRYB_RESTORE_COVERAGE

/*
 * Use wherever a static analyzer is unable to correctly determine that a
 * loop will never terminate, an if-else chain will always cover all
 * possible cases, etc.
 */
#define CRYB_UNREACHABLE						\
	CRYB_DISABLE_COVERAGE						\
	do { *(volatile int *)0 = 0; } while (0)			\
	CRYB_RESTORE_COVERAGE

#endif
