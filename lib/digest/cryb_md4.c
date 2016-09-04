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
/*
 *  The MD4 algorithm was designed by Ron Rivest in 1990.
 *
 *  http://www.ietf.org/rfc/rfc1186.txt
 *  http://www.ietf.org/rfc/rfc1320.txt
 */

#include "cryb/impl.h"

#include <stdint.h>
#include <string.h>

#include <cryb/endian.h>
#include <cryb/md4.h>

/*
 * MD4 context setup
 */
void md4_init( md4_ctx *ctx )
{
    ctx->total = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}

static void md4_process( md4_ctx *ctx, const uint8_t *data )
{
    uint32_t X[16], A, B, C, D;

    X[ 0] = le32dec(data +  0);
    X[ 1] = le32dec(data +  4);
    X[ 2] = le32dec(data +  8);
    X[ 3] = le32dec(data + 12);
    X[ 4] = le32dec(data + 16);
    X[ 5] = le32dec(data + 20);
    X[ 6] = le32dec(data + 24);
    X[ 7] = le32dec(data + 28);
    X[ 8] = le32dec(data + 32);
    X[ 9] = le32dec(data + 36);
    X[10] = le32dec(data + 40);
    X[11] = le32dec(data + 44);
    X[12] = le32dec(data + 48);
    X[13] = le32dec(data + 52);
    X[14] = le32dec(data + 56);
    X[15] = le32dec(data + 60);

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];

#define F(x, y, z) ((x & y) | ((~x) & z))
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x; a = S(a,s); }

    P( A, B, C, D, X[ 0],  3 );
    P( D, A, B, C, X[ 1],  7 );
    P( C, D, A, B, X[ 2], 11 );
    P( B, C, D, A, X[ 3], 19 );
    P( A, B, C, D, X[ 4],  3 );
    P( D, A, B, C, X[ 5],  7 );
    P( C, D, A, B, X[ 6], 11 );
    P( B, C, D, A, X[ 7], 19 );
    P( A, B, C, D, X[ 8],  3 );
    P( D, A, B, C, X[ 9],  7 );
    P( C, D, A, B, X[10], 11 );
    P( B, C, D, A, X[11], 19 );
    P( A, B, C, D, X[12],  3 );
    P( D, A, B, C, X[13],  7 );
    P( C, D, A, B, X[14], 11 );
    P( B, C, D, A, X[15], 19 );

#undef P
#undef F

#define F(x,y,z) ((x & y) | (x & z) | (y & z))
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x + 0x5A827999; a = S(a,s); }

    P( A, B, C, D, X[ 0],  3 );
    P( D, A, B, C, X[ 4],  5 );
    P( C, D, A, B, X[ 8],  9 );
    P( B, C, D, A, X[12], 13 );
    P( A, B, C, D, X[ 1],  3 );
    P( D, A, B, C, X[ 5],  5 );
    P( C, D, A, B, X[ 9],  9 );
    P( B, C, D, A, X[13], 13 );
    P( A, B, C, D, X[ 2],  3 );
    P( D, A, B, C, X[ 6],  5 );
    P( C, D, A, B, X[10],  9 );
    P( B, C, D, A, X[14], 13 );
    P( A, B, C, D, X[ 3],  3 );
    P( D, A, B, C, X[ 7],  5 );
    P( C, D, A, B, X[11],  9 );
    P( B, C, D, A, X[15], 13 );

#undef P
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x + 0x6ED9EBA1; a = S(a,s); }

    P( A, B, C, D, X[ 0],  3 );
    P( D, A, B, C, X[ 8],  9 );
    P( C, D, A, B, X[ 4], 11 );
    P( B, C, D, A, X[12], 15 );
    P( A, B, C, D, X[ 2],  3 );
    P( D, A, B, C, X[10],  9 );
    P( C, D, A, B, X[ 6], 11 );
    P( B, C, D, A, X[14], 15 );
    P( A, B, C, D, X[ 1],  3 );
    P( D, A, B, C, X[ 9],  9 );
    P( C, D, A, B, X[ 5], 11 );
    P( B, C, D, A, X[13], 15 );
    P( A, B, C, D, X[ 3],  3 );
    P( D, A, B, C, X[11],  9 );
    P( C, D, A, B, X[ 7], 11 );
    P( B, C, D, A, X[15], 15 );

#undef F
#undef P

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}

/*
 * MD4 process buffer
 */
void md4_update( md4_ctx *ctx, const void *input, int ilen )
{
    int fill;
    uint32_t left;

    if( ilen <= 0 )
        return;

    left = ctx->total & 0x3F;
    fill = 64 - left;

    ctx->total += ilen;

    if( left && ilen >= fill )
    {
        memcpy( (ctx->buffer + left), input, fill );
        md4_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 )
    {
        md4_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
    {
        memcpy( (ctx->buffer + left), input, ilen );
    }
}

static const uint8_t md4_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * MD4 final digest
 */
void md4_final( md4_ctx *ctx, uint8_t *output )
{
    uint32_t last, padn;
    uint8_t msglen[8];

    le64enc(msglen, ctx->total << 3);

    last = ctx->total & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    md4_update( ctx, md4_padding, padn );
    md4_update( ctx, msglen, 8 );

    le32enc(output +  0,  ctx->state[0]);
    le32enc(output +  4,  ctx->state[1]);
    le32enc(output +  8,  ctx->state[2]);
    le32enc(output + 12,  ctx->state[3]);
}

/*
 * output = MD4( input buffer )
 */
void md4_complete( const void *input, int ilen, uint8_t *output )
{
    md4_ctx ctx;

    md4_init( &ctx );
    md4_update( &ctx, input, ilen );
    md4_final( &ctx, output );

    memset( &ctx, 0, sizeof( md4_ctx ) );
}

digest_algorithm md4_digest = {
	.name			 = "md4",
	.contextlen		 = sizeof(md4_ctx),
	.blocklen		 = MD4_BLOCK_LEN,
	.digestlen		 = MD4_DIGEST_LEN,
	.init			 = (digest_init_func)md4_init,
	.update			 = (digest_update_func)md4_update,
	.final			 = (digest_final_func)md4_final,
	.complete		 = (digest_complete_func)md4_complete,
};
