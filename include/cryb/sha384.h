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
 */

#ifndef CRYB_SHA384_H_INCLUDED
#define CRYB_SHA384_H_INCLUDED

#include <cryb/digest.h>

#define SHA384_BLOCK_LEN		128
#define SHA384_DIGEST_LEN		48

#define sha384_digest			cryb_sha384_digest
#define sha384_ctx			cryb_sha384_ctx
#define sha384_init			cryb_sha384_init
#define sha384_update			cryb_sha384_update
#define sha384_final			cryb_sha384_final
#define sha384_complete			cryb_sha384_complete

extern digest_algorithm sha384_digest;

/**
 * \brief          SHA-384 context structure
 */
typedef struct
{
    uint64_t total[2];		/*!< number of bytes processed  */
    uint64_t state[8];		/*!< intermediate digest state  */
    uint8_t buffer[128];	/*!< data block being processed */
}
sha384_ctx;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-384 context setup
 *
 * \param ctx      context to be initialized
 */
void sha384_init( sha384_ctx *ctx );

/**
 * \brief          SHA-384 process buffer
 *
 * \param ctx      SHA-384 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void sha384_update( sha384_ctx *ctx, const void *input, int ilen );

/**
 * \brief          SHA-384 final digest
 *
 * \param ctx      SHA-384 context
 * \param output   SHA-384/384 checksum result
 */
void sha384_final( sha384_ctx *ctx, uint8_t *output );

/**
 * \brief          Output = SHA-384( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-384/384 checksum result
 */
void sha384_complete( const void *input, int ilen, uint8_t *output );

#ifdef __cplusplus
}
#endif

#endif
