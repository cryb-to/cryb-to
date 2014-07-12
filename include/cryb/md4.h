/*
 * Copyright (c) 2006-2007 Christophe Devine
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
 *
 * $Cryb$
 */

#ifndef CRYB_MD4_H_INCLUDED
#define CRYB_MD4_H_INCLUDED

#include <cryb/digest.h>

#define MD4_BLOCK_LEN			64
#define MD4_DIGEST_LEN			16

#define md4_digest			cryb_md4_digest
#define md4_ctx				cryb_md4_ctx
#define md4_init			cryb_md4_init
#define md4_update			cryb_md4_update
#define md4_final			cryb_md4_final
#define md4_complete			cryb_md4_complete

extern digest_algorithm md4_digest;

/**
 * \brief          MD4 context structure
 */
typedef struct
{
    uint32_t total[2];		/*!< number of bytes processed  */
    uint32_t state[4];		/*!< intermediate digest state  */
    uint8_t buffer[64];		/*!< data block being processed */
}
md4_ctx;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MD4 context setup
 *
 * \param ctx      context to be initialized
 */
void md4_init( md4_ctx *ctx );

/**
 * \brief          MD4 process buffer
 *
 * \param ctx      MD4 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void md4_update( md4_ctx *ctx, const void *input, int ilen );

/**
 * \brief          MD4 final digest
 *
 * \param ctx      MD4 context
 * \param output   MD4 checksum result
 */
void md4_final( md4_ctx *ctx, uint8_t *output );

/**
 * \brief          Output = MD4( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD4 checksum result
 */
void md4_complete( const void *input, int ilen, uint8_t *output );

#ifdef __cplusplus
}
#endif

#endif
