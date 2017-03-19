/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012-2016 Dag-Erling Smørgrav
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

#include <cryb/md5.h>

/*
 * MD5 - RFC 1321
 */

/* initial state */
static const uint32_t md5_h[4] = {
	0x67452301UL, 0xefcdab89UL, 0x98badcfeUL, 0x10325476UL,
};

static const uint32_t md5_k[64] = {
	0xd76aa478UL, 0xe8c7b756UL, 0x242070dbUL, 0xc1bdceeeUL,
	0xf57c0fafUL, 0x4787c62aUL, 0xa8304613UL, 0xfd469501UL,
	0x698098d8UL, 0x8b44f7afUL, 0xffff5bb1UL, 0x895cd7beUL,
	0x6b901122UL, 0xfd987193UL, 0xa679438eUL, 0x49b40821UL,
	0xf61e2562UL, 0xc040b340UL, 0x265e5a51UL, 0xe9b6c7aaUL,
	0xd62f105dUL, 0x02441453UL, 0xd8a1e681UL, 0xe7d3fbc8UL,
	0x21e1cde6UL, 0xc33707d6UL, 0xf4d50d87UL, 0x455a14edUL,
	0xa9e3e905UL, 0xfcefa3f8UL, 0x676f02d9UL, 0x8d2a4c8aUL,
	0xfffa3942UL, 0x8771f681UL, 0x6d9d6122UL, 0xfde5380cUL,
	0xa4beea44UL, 0x4bdecfa9UL, 0xf6bb4b60UL, 0xbebfbc70UL,
	0x289b7ec6UL, 0xeaa127faUL, 0xd4ef3085UL, 0x04881d05UL,
	0xd9d4d039UL, 0xe6db99e5UL, 0x1fa27cf8UL, 0xc4ac5665UL,
	0xf4292244UL, 0x432aff97UL, 0xab9423a7UL, 0xfc93a039UL,
	0x655b59c3UL, 0x8f0ccc92UL, 0xffeff47dUL, 0x85845dd1UL,
	0x6fa87e4fUL, 0xfe2ce6e0UL, 0xa3014314UL, 0x4e0811a1UL,
	0xf7537e82UL, 0xbd3af235UL, 0x2ad7d2bbUL, 0xeb86d391UL,
};

void
md5_init(md5_ctx *ctx)
{

	memset(ctx, 0, sizeof *ctx);
	memcpy(ctx->state, md5_h, sizeof ctx->state);
}

#define md5_f(i, a, b, c, d, x, s) do {					\
		(a) += (((b) & (c)) | (~(b) & (d)));			\
		(a) += (x)[(i)] + md5_k[(i)];				\
		(a) = rol32((a), (s)) + (b);				\
	} while (0)

#define md5_g(i, a, b, c, d, x, s) do {					\
		(a) += (((b) & (d)) | ((c) & ~(d)));			\
		(a) += (x)[(5 * (i) + 1) % 16] + md5_k[(i)];		\
		(a) = rol32((a), (s)) + (b);				\
	} while (0)

#define md5_h(i, a, b, c, d, x, s) do {					\
		(a) += ((b) ^ (c) ^ (d));				\
		(a) += (x)[(3 * (i) + 5) % 16] + md5_k[(i)];		\
		(a) = rol32((a), (s)) + (b);				\
	} while (0)

#define md5_i(i, a, b, c, d, x, s) do {					\
		(a) += ((c) ^ ((b) | ~(d)));				\
		(a) += (x)[(7 * (i)) % 16] + md5_k[(i)];		\
		(a) = rol32((a), (s)) + (b);				\
	} while (0)

static void
md5_compute(md5_ctx *ctx, const uint8_t *block)
{
	uint32_t w[16], a, b, c, d;

	le32decv(w, block, 16);
	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];

	md5_f( 0, a, b, c, d, w,  7);
	md5_f( 1, d, a, b, c, w, 12);
	md5_f( 2, c, d, a, b, w, 17);
	md5_f( 3, b, c, d, a, w, 22);
	md5_f( 4, a, b, c, d, w,  7);
	md5_f( 5, d, a, b, c, w, 12);
	md5_f( 6, c, d, a, b, w, 17);
	md5_f( 7, b, c, d, a, w, 22);
	md5_f( 8, a, b, c, d, w,  7);
	md5_f( 9, d, a, b, c, w, 12);
	md5_f(10, c, d, a, b, w, 17);
	md5_f(11, b, c, d, a, w, 22);
	md5_f(12, a, b, c, d, w,  7);
	md5_f(13, d, a, b, c, w, 12);
	md5_f(14, c, d, a, b, w, 17);
	md5_f(15, b, c, d, a, w, 22);

	md5_g(16, a, b, c, d, w,  5);
	md5_g(17, d, a, b, c, w,  9);
	md5_g(18, c, d, a, b, w, 14);
	md5_g(19, b, c, d, a, w, 20);
	md5_g(20, a, b, c, d, w,  5);
	md5_g(21, d, a, b, c, w,  9);
	md5_g(22, c, d, a, b, w, 14);
	md5_g(23, b, c, d, a, w, 20);
	md5_g(24, a, b, c, d, w,  5);
	md5_g(25, d, a, b, c, w,  9);
	md5_g(26, c, d, a, b, w, 14);
	md5_g(27, b, c, d, a, w, 20);
	md5_g(28, a, b, c, d, w,  5);
	md5_g(29, d, a, b, c, w,  9);
	md5_g(30, c, d, a, b, w, 14);
	md5_g(31, b, c, d, a, w, 20);

	md5_h(32, a, b, c, d, w,  4);
	md5_h(33, d, a, b, c, w, 11);
	md5_h(34, c, d, a, b, w, 16);
	md5_h(35, b, c, d, a, w, 23);
	md5_h(36, a, b, c, d, w,  4);
	md5_h(37, d, a, b, c, w, 11);
	md5_h(38, c, d, a, b, w, 16);
	md5_h(39, b, c, d, a, w, 23);
	md5_h(40, a, b, c, d, w,  4);
	md5_h(41, d, a, b, c, w, 11);
	md5_h(42, c, d, a, b, w, 16);
	md5_h(43, b, c, d, a, w, 23);
	md5_h(44, a, b, c, d, w,  4);
	md5_h(45, d, a, b, c, w, 11);
	md5_h(46, c, d, a, b, w, 16);
	md5_h(47, b, c, d, a, w, 23);

	md5_i(48, a, b, c, d, w,  6);
	md5_i(49, d, a, b, c, w, 10);
	md5_i(50, c, d, a, b, w, 15);
	md5_i(51, b, c, d, a, w, 21);
	md5_i(52, a, b, c, d, w,  6);
	md5_i(53, d, a, b, c, w, 10);
	md5_i(54, c, d, a, b, w, 15);
	md5_i(55, b, c, d, a, w, 21);
	md5_i(56, a, b, c, d, w,  6);
	md5_i(57, d, a, b, c, w, 10);
	md5_i(58, c, d, a, b, w, 15);
	md5_i(59, b, c, d, a, w, 21);
	md5_i(60, a, b, c, d, w,  6);
	md5_i(61, d, a, b, c, w, 10);
	md5_i(62, c, d, a, b, w, 15);
	md5_i(63, b, c, d, a, w, 21);

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
}

void
md5_update(md5_ctx *ctx, const void *buf, size_t len)
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
				md5_compute(ctx, ctx->block);
				ctx->blocklen = 0;
			}
		} else {
			copylen = sizeof ctx->block;
			md5_compute(ctx, buf);
		}
		ctx->bitlen += copylen * 8;
		buf += copylen;
		len -= copylen;
	}
}

void
md5_final(md5_ctx *ctx, uint8_t *digest)
{

	ctx->block[ctx->blocklen++] = 0x80;
	memset(ctx->block + ctx->blocklen, 0,
	    sizeof ctx->block - ctx->blocklen);
	if (ctx->blocklen > 56) {
		md5_compute(ctx, ctx->block);
		ctx->blocklen = 0;
		memset(ctx->block, 0, sizeof ctx->block);
	}
	le64enc(ctx->block + 56, ctx->bitlen);
	md5_compute(ctx, ctx->block);
	le32encv(digest, ctx->state, 4);
	memset_s(ctx, 0, sizeof *ctx, sizeof *ctx);
}

void
md5_complete(const void *buf, size_t len, uint8_t *digest)
{
	md5_ctx ctx;

	md5_init(&ctx);
	md5_update(&ctx, buf, len);
	md5_final(&ctx, digest);
}

digest_algorithm md5_digest = {
	.name			 = "md5",
	.contextlen		 = sizeof(md5_ctx),
	.blocklen		 = MD5_BLOCK_LEN,
	.digestlen		 = MD5_DIGEST_LEN,
	.init			 = (digest_init_func)md5_init,
	.update			 = (digest_update_func)md5_update,
	.final			 = (digest_final_func)md5_final,
	.complete		 = (digest_complete_func)md5_complete,
};
