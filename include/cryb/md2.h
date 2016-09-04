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

#ifndef CRYB_MD2_H_INCLUDED
#define CRYB_MD2_H_INCLUDED

#ifndef CRYB_TO
#include <cryb/to.h>
#endif

#include <cryb/digest.h>

#define MD2_BLOCK_LEN			16
#define MD2_DIGEST_LEN			16

#define md2_digest			cryb_md2_digest
#define md2_ctx				cryb_md2_ctx
#define md2_init			cryb_md2_init
#define md2_update			cryb_md2_update
#define md2_final			cryb_md2_final
#define md2_complete			cryb_md2_complete

extern digest_algorithm md2_digest;

/**
 * \brief          MD2 context structure
 */
typedef struct
{
    uint8_t cksum[16];		/*!< checksum of the data block */
    uint8_t state[48];		/*!< intermediate digest state	*/
    uint8_t buffer[16];		/*!< data block being processed */
    int left;			/*!< amount of data in buffer	*/
} md2_ctx;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MD2 context setup
 *
 * \param ctx      context to be initialized
 */
void md2_init( md2_ctx *ctx );

/**
 * \brief          MD2 process buffer
 *
 * \param ctx      MD2 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void md2_update( md2_ctx *ctx, const void *input, int ilen );

/**
 * \brief          MD2 final digest
 *
 * \param ctx      MD2 context
 * \param output   MD2 checksum result
 */
void md2_final( md2_ctx *ctx, uint8_t *output );

/**
 * \brief          Output = MD2( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD2 checksum result
 */
void md2_complete( const void *input, int ilen, uint8_t *output );

#ifdef __cplusplus
}
#endif

#endif /* md2.h */
