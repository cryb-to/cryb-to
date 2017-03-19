/*
 * Copyright (c) 2014-2017 Dag-Erling Smørgrav
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

#include <stdint.h>
#include <string.h>

#include <cryb/bitwise.h>
#include <cryb/endian.h>
#include <cryb/memset_s.h>

#include <cryb/md4.h>

/*
 * MD4 - RFC 1320
 */

/* initial state */
static const uint32_t md4_h[4] = {
	0x67452301UL, 0xefcdab89UL, 0x98badcfeUL, 0x10325476UL,
};

void
md4_init(md4_ctx *ctx)
{

	memset(ctx, 0, sizeof *ctx);
	memcpy(ctx->state, md4_h, sizeof ctx->state);
}

static void
md4_compute(md4_ctx *ctx, const uint8_t *data)
{
	uint32_t X[16];
	uint32_t A, B, C, D;

	le32decv(X, data, 16);

	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];

#define F(x, y, z)	(x & y | ~x & z)
#define md4_round1(a, b, c, d, k, s)					\
	a = rol32(a + F(b, c, d) + X[k], s)

#define G(x, y, z)	(x & y | x & z | y & z)
#define md4_round2(a, b, c, d, k, s)					\
	a = rol32(a + G(b, c, d) + X[k] + 0x5a827999, s)

#define H(x, y, z)	(x ^ y ^ z)
#define md4_round3(a, b, c, d, k, s)					\
	a = rol32(a + H(b,c,d) + X[k] + 0x6ed9eba1, s)

	md4_round1(A, B, C, D,  0,  3);
	md4_round1(D, A, B, C,  1,  7);
	md4_round1(C, D, A, B,  2, 11);
	md4_round1(B, C, D, A,  3, 19);
	md4_round1(A, B, C, D,  4,  3);
	md4_round1(D, A, B, C,  5,  7);
	md4_round1(C, D, A, B,  6, 11);
	md4_round1(B, C, D, A,  7, 19);
	md4_round1(A, B, C, D,  8,  3);
	md4_round1(D, A, B, C,  9,  7);
	md4_round1(C, D, A, B, 10, 11);
	md4_round1(B, C, D, A, 11, 19);
	md4_round1(A, B, C, D, 12,  3);
	md4_round1(D, A, B, C, 13,  7);
	md4_round1(C, D, A, B, 14, 11);
	md4_round1(B, C, D, A, 15, 19);

	md4_round2(A, B, C, D,  0,  3);
	md4_round2(D, A, B, C,  4,  5);
	md4_round2(C, D, A, B,  8,  9);
	md4_round2(B, C, D, A, 12, 13);
	md4_round2(A, B, C, D,  1,  3);
	md4_round2(D, A, B, C,  5,  5);
	md4_round2(C, D, A, B,  9,  9);
	md4_round2(B, C, D, A, 13, 13);
	md4_round2(A, B, C, D,  2,  3);
	md4_round2(D, A, B, C,  6,  5);
	md4_round2(C, D, A, B, 10,  9);
	md4_round2(B, C, D, A, 14, 13);
	md4_round2(A, B, C, D,  3,  3);
	md4_round2(D, A, B, C,  7,  5);
	md4_round2(C, D, A, B, 11,  9);
	md4_round2(B, C, D, A, 15, 13);

	md4_round3(A, B, C, D,  0,  3);
	md4_round3(D, A, B, C,  8,  9);
	md4_round3(C, D, A, B,  4, 11);
	md4_round3(B, C, D, A, 12, 15);
	md4_round3(A, B, C, D,  2,  3);
	md4_round3(D, A, B, C, 10,  9);
	md4_round3(C, D, A, B,  6, 11);
	md4_round3(B, C, D, A, 14, 15);
	md4_round3(A, B, C, D,  1,  3);
	md4_round3(D, A, B, C,  9,  9);
	md4_round3(C, D, A, B,  5, 11);
	md4_round3(B, C, D, A, 13, 15);
	md4_round3(A, B, C, D,  3,  3);
	md4_round3(D, A, B, C, 11,  9);
	md4_round3(C, D, A, B,  7, 11);
	md4_round3(B, C, D, A, 15, 15);

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
}

void
md4_update(md4_ctx *ctx, const void *buf, size_t len)
{
	size_t copylen;

	while (len) {
		if (ctx->blocklen > 0 || len < sizeof ctx->block) {
			copylen = sizeof ctx->block - ctx->blocklen;
			if (copylen > len)
				copylen = len;
			memcpy(ctx->block + ctx->blocklen, buf, copylen);
			ctx->blocklen += copylen;
			if (ctx->blocklen == sizeof ctx->block) {
				md4_compute(ctx, ctx->block);
				ctx->blocklen = 0;
			}
		} else {
			copylen = sizeof ctx->block;
			md4_compute(ctx, buf);
		}
		ctx->bitlen += copylen * 8;
		buf += copylen;
		len -= copylen;
	}
}

void
md4_final(md4_ctx *ctx, uint8_t *digest)
{

	ctx->block[ctx->blocklen++] = 0x80;
	memset(ctx->block + ctx->blocklen, 0,
	    sizeof ctx->block - ctx->blocklen);
	if (ctx->blocklen > 56) {
		md4_compute(ctx, ctx->block);
		ctx->blocklen = 0;
		memset(ctx->block, 0, sizeof ctx->block);
	}
	le64enc(ctx->block + 56, ctx->bitlen);
	md4_compute(ctx, ctx->block);
	le32encv(digest, ctx->state, 4);
	memset_s(ctx, 0, sizeof *ctx, sizeof *ctx);
}

void
md4_complete(const void *buf, size_t len, uint8_t *digest)
{
	md4_ctx ctx;

	md4_init(&ctx);
	md4_update(&ctx, buf, len);
	md4_final(&ctx, digest);
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
