/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012-2014 Dag-Erling Smørgrav
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
 * Author: Dag-Erling Smørgrav <des@des.no>
 * Sponsor: the University of Oslo
 *
 * $Cryb$
 */

#include "cryb/impl.h"

#ifdef HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#ifdef HAVE_ENDIAN_H
#define _BSD_SOURCE
#include <endian.h>
#endif

#include <stdint.h>
#include <string.h>

#include <cryb/bitwise.h>
#include <cryb/md5.h>

/* initial state */
static const uint32_t md5_h[4] = {
	0x67452301U, 0xefcdab89U, 0x98badcfeU, 0x10325476U,
};

static const uint32_t md5_k[64] = {
	0xd76aa478U, 0xe8c7b756U, 0x242070dbU, 0xc1bdceeeU,
	0xf57c0fafU, 0x4787c62aU, 0xa8304613U, 0xfd469501U,
	0x698098d8U, 0x8b44f7afU, 0xffff5bb1U, 0x895cd7beU,
	0x6b901122U, 0xfd987193U, 0xa679438eU, 0x49b40821U,
	0xf61e2562U, 0xc040b340U, 0x265e5a51U, 0xe9b6c7aaU,
	0xd62f105dU, 0x02441453U, 0xd8a1e681U, 0xe7d3fbc8U,
	0x21e1cde6U, 0xc33707d6U, 0xf4d50d87U, 0x455a14edU,
	0xa9e3e905U, 0xfcefa3f8U, 0x676f02d9U, 0x8d2a4c8aU,
	0xfffa3942U, 0x8771f681U, 0x6d9d6122U, 0xfde5380cU,
	0xa4beea44U, 0x4bdecfa9U, 0xf6bb4b60U, 0xbebfbc70U,
	0x289b7ec6U, 0xeaa127faU, 0xd4ef3085U, 0x04881d05U,
	0xd9d4d039U, 0xe6db99e5U, 0x1fa27cf8U, 0xc4ac5665U,
	0xf4292244U, 0x432aff97U, 0xab9423a7U, 0xfc93a039U,
	0x655b59c3U, 0x8f0ccc92U, 0xffeff47dU, 0x85845dd1U,
	0x6fa87e4fU, 0xfe2ce6e0U, 0xa3014314U, 0x4e0811a1U,
	0xf7537e82U, 0xbd3af235U, 0x2ad7d2bbU, 0xeb86d391U,
};

void
md5_init(md5_ctx *ctx)
{

	memset(ctx, 0, sizeof *ctx);
	memcpy(ctx->h, md5_h, sizeof ctx->h);
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

	memcpy(w, block, 64);
	for (int i = 0; i < 16; ++i)
		w[i] = le32toh(w[i]);
	a = ctx->h[0];
	b = ctx->h[1];
	c = ctx->h[2];
	d = ctx->h[3];

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

	ctx->h[0] += a;
	ctx->h[1] += b;
	ctx->h[2] += c;
	ctx->h[3] += d;
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
				memset(ctx->block, 0, sizeof ctx->block);
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
	if (ctx->blocklen > 56) {
		md5_compute(ctx, ctx->block);
		ctx->blocklen = 0;
		memset(ctx->block, 0, sizeof ctx->block);
	}
	le32enc(ctx->block + 56, ctx->bitlen & 0xffffffffUL);
	le32enc(ctx->block + 60, ctx->bitlen >> 32);
	md5_compute(ctx, ctx->block);
	le32enc(digest, ctx->h[0]);
	le32enc(digest + 4, ctx->h[1]);
	le32enc(digest + 8, ctx->h[2]);
	le32enc(digest + 12, ctx->h[3]);
	memset(ctx, 0, sizeof *ctx);
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
