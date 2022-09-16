/*
 * Copyright (c) 2006-2007 Christophe Devine
 * Copyright (c) 2017 Dag-Erling Smørgrav
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

#include <cryb/sha384.h>

static const uint64_t sha384_h[8] = {
	0xCBBB9D5DC1059ED8ULL, 0x629A292A367CD507ULL,
	0x9159015A3070DD17ULL, 0x152FECD8F70E5939ULL,
	0x67332667FFC00B31ULL, 0x8EB44A8768581511ULL,
	0xDB0C2E0D64F98FA7ULL, 0x47B5481DBEFA4FA4ULL,
};

static const uint64_t sha384_k[80] = {
	0x428A2F98D728AE22ULL, 0x7137449123EF65CDULL,
	0xB5C0FBCFEC4D3B2FULL, 0xE9B5DBA58189DBBCULL,
	0x3956C25BF348B538ULL, 0x59F111F1B605D019ULL,
	0x923F82A4AF194F9BULL, 0xAB1C5ED5DA6D8118ULL,
	0xD807AA98A3030242ULL, 0x12835B0145706FBEULL,
	0x243185BE4EE4B28CULL, 0x550C7DC3D5FFB4E2ULL,
	0x72BE5D74F27B896FULL, 0x80DEB1FE3B1696B1ULL,
	0x9BDC06A725C71235ULL, 0xC19BF174CF692694ULL,
	0xE49B69C19EF14AD2ULL, 0xEFBE4786384F25E3ULL,
	0x0FC19DC68B8CD5B5ULL, 0x240CA1CC77AC9C65ULL,
	0x2DE92C6F592B0275ULL, 0x4A7484AA6EA6E483ULL,
	0x5CB0A9DCBD41FBD4ULL, 0x76F988DA831153B5ULL,
	0x983E5152EE66DFABULL, 0xA831C66D2DB43210ULL,
	0xB00327C898FB213FULL, 0xBF597FC7BEEF0EE4ULL,
	0xC6E00BF33DA88FC2ULL, 0xD5A79147930AA725ULL,
	0x06CA6351E003826FULL, 0x142929670A0E6E70ULL,
	0x27B70A8546D22FFCULL, 0x2E1B21385C26C926ULL,
	0x4D2C6DFC5AC42AEDULL, 0x53380D139D95B3DFULL,
	0x650A73548BAF63DEULL, 0x766A0ABB3C77B2A8ULL,
	0x81C2C92E47EDAEE6ULL, 0x92722C851482353BULL,
	0xA2BFE8A14CF10364ULL, 0xA81A664BBC423001ULL,
	0xC24B8B70D0F89791ULL, 0xC76C51A30654BE30ULL,
	0xD192E819D6EF5218ULL, 0xD69906245565A910ULL,
	0xF40E35855771202AULL, 0x106AA07032BBD1B8ULL,
	0x19A4C116B8D2D0C8ULL, 0x1E376C085141AB53ULL,
	0x2748774CDF8EEB99ULL, 0x34B0BCB5E19B48A8ULL,
	0x391C0CB3C5C95A63ULL, 0x4ED8AA4AE3418ACBULL,
	0x5B9CCA4F7763E373ULL, 0x682E6FF3D6B2B8A3ULL,
	0x748F82EE5DEFB2FCULL, 0x78A5636F43172F60ULL,
	0x84C87814A1F0AB72ULL, 0x8CC702081A6439ECULL,
	0x90BEFFFA23631E28ULL, 0xA4506CEBDE82BDE9ULL,
	0xBEF9A3F7B2C67915ULL, 0xC67178F2E372532BULL,
	0xCA273ECEEA26619CULL, 0xD186B8C721C0C207ULL,
	0xEADA7DD6CDE0EB1EULL, 0xF57D4F7FEE6ED178ULL,
	0x06F067AA72176FBAULL, 0x0A637DC5A2C898A6ULL,
	0x113F9804BEF90DAEULL, 0x1B710B35131C471BULL,
	0x28DB77F523047D84ULL, 0x32CAAB7B40C72493ULL,
	0x3C9EBE0A15C9BEBCULL, 0x431D67C49C100D4CULL,
	0x4CC5D4BECB3E42B6ULL, 0x597F299CFC657E2AULL,
	0x5FCB6FAB3AD6FAECULL, 0x6C44198C4A475817ULL,
};

void
sha384_init(sha384_ctx *ctx)
{

	memset(ctx, 0, sizeof *ctx);
	memcpy(ctx->h, sha384_h, sizeof ctx->h);
}

#define S0(x)		(ror64(x,  1) ^ ror64(x,  8) ^ (x >> 7))
#define S1(x)		(ror64(x, 19) ^ ror64(x, 61) ^ (x >> 6))

#define S2(x)		(ror64(x, 28) ^ ror64(x, 34) ^ ror64(x, 39))
#define S3(x)		(ror64(x, 14) ^ ror64(x, 18) ^ ror64(x, 41))

#define F0(x, y, z)	((x & y) | (z & (x | y)))
#define F1(x, y, z)	(z ^ (x & (y ^ z)))

#define P(i, a, b, c, d, e, f, g, h) do {				\
		uint64_t t0, t1;					\
		t0 = h + S3(e) + F1(e, f, g) + sha384_k[i] + w[i];	\
		t1 = S2(a) + F0(a, b, c);				\
		d += t0; h = t0 + t1;					\
	} while (0)

static void
sha384_compute(sha384_ctx *ctx, const uint8_t *data)
{
	uint64_t w[80], A, B, C, D, E, F, G, H;
	unsigned int i;

	be64decv(w, data, 16);
	for (i = 16; i < 80; i++)
		w[i] = S1(w[i -	2]) + w[i - 7] + S0(w[i - 15]) + w[i - 16];
	A = ctx->h[0];
	B = ctx->h[1];
	C = ctx->h[2];
	D = ctx->h[3];
	E = ctx->h[4];
	F = ctx->h[5];
	G = ctx->h[6];
	H = ctx->h[7];

	P( 0, A, B, C, D, E, F, G, H);
	P( 1, H, A, B, C, D, E, F, G);
	P( 2, G, H, A, B, C, D, E, F);
	P( 3, F, G, H, A, B, C, D, E);
	P( 4, E, F, G, H, A, B, C, D);
	P( 5, D, E, F, G, H, A, B, C);
	P( 6, C, D, E, F, G, H, A, B);
	P( 7, B, C, D, E, F, G, H, A);

	P( 8, A, B, C, D, E, F, G, H);
	P( 9, H, A, B, C, D, E, F, G);
	P(10, G, H, A, B, C, D, E, F);
	P(11, F, G, H, A, B, C, D, E);
	P(12, E, F, G, H, A, B, C, D);
	P(13, D, E, F, G, H, A, B, C);
	P(14, C, D, E, F, G, H, A, B);
	P(15, B, C, D, E, F, G, H, A);

	P(16, A, B, C, D, E, F, G, H);
	P(17, H, A, B, C, D, E, F, G);
	P(18, G, H, A, B, C, D, E, F);
	P(19, F, G, H, A, B, C, D, E);
	P(20, E, F, G, H, A, B, C, D);
	P(21, D, E, F, G, H, A, B, C);
	P(22, C, D, E, F, G, H, A, B);
	P(23, B, C, D, E, F, G, H, A);

	P(24, A, B, C, D, E, F, G, H);
	P(25, H, A, B, C, D, E, F, G);
	P(26, G, H, A, B, C, D, E, F);
	P(27, F, G, H, A, B, C, D, E);
	P(28, E, F, G, H, A, B, C, D);
	P(29, D, E, F, G, H, A, B, C);
	P(30, C, D, E, F, G, H, A, B);
	P(31, B, C, D, E, F, G, H, A);

	P(32, A, B, C, D, E, F, G, H);
	P(33, H, A, B, C, D, E, F, G);
	P(34, G, H, A, B, C, D, E, F);
	P(35, F, G, H, A, B, C, D, E);
	P(36, E, F, G, H, A, B, C, D);
	P(37, D, E, F, G, H, A, B, C);
	P(38, C, D, E, F, G, H, A, B);
	P(39, B, C, D, E, F, G, H, A);

	P(40, A, B, C, D, E, F, G, H);
	P(41, H, A, B, C, D, E, F, G);
	P(42, G, H, A, B, C, D, E, F);
	P(43, F, G, H, A, B, C, D, E);
	P(44, E, F, G, H, A, B, C, D);
	P(45, D, E, F, G, H, A, B, C);
	P(46, C, D, E, F, G, H, A, B);
	P(47, B, C, D, E, F, G, H, A);

	P(48, A, B, C, D, E, F, G, H);
	P(49, H, A, B, C, D, E, F, G);
	P(50, G, H, A, B, C, D, E, F);
	P(51, F, G, H, A, B, C, D, E);
	P(52, E, F, G, H, A, B, C, D);
	P(53, D, E, F, G, H, A, B, C);
	P(54, C, D, E, F, G, H, A, B);
	P(55, B, C, D, E, F, G, H, A);

	P(56, A, B, C, D, E, F, G, H);
	P(57, H, A, B, C, D, E, F, G);
	P(58, G, H, A, B, C, D, E, F);
	P(59, F, G, H, A, B, C, D, E);
	P(60, E, F, G, H, A, B, C, D);
	P(61, D, E, F, G, H, A, B, C);
	P(62, C, D, E, F, G, H, A, B);
	P(63, B, C, D, E, F, G, H, A);

	P(64, A, B, C, D, E, F, G, H);
	P(65, H, A, B, C, D, E, F, G);
	P(66, G, H, A, B, C, D, E, F);
	P(67, F, G, H, A, B, C, D, E);
	P(68, E, F, G, H, A, B, C, D);
	P(69, D, E, F, G, H, A, B, C);
	P(70, C, D, E, F, G, H, A, B);
	P(71, B, C, D, E, F, G, H, A);

	P(72, A, B, C, D, E, F, G, H);
	P(73, H, A, B, C, D, E, F, G);
	P(74, G, H, A, B, C, D, E, F);
	P(75, F, G, H, A, B, C, D, E);
	P(76, E, F, G, H, A, B, C, D);
	P(77, D, E, F, G, H, A, B, C);
	P(78, C, D, E, F, G, H, A, B);
	P(79, B, C, D, E, F, G, H, A);

	ctx->h[0] += A;
	ctx->h[1] += B;
	ctx->h[2] += C;
	ctx->h[3] += D;
	ctx->h[4] += E;
	ctx->h[5] += F;
	ctx->h[6] += G;
	ctx->h[7] += H;
}

void
sha384_update(sha384_ctx *ctx, const void *buf, size_t len)
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
				sha384_compute(ctx, ctx->block);
				ctx->blocklen = 0;
			}
		} else {
			copylen = sizeof ctx->block;
			sha384_compute(ctx, buf);
		}
		if ((ctx->bitlen[1] += copylen * 8) < copylen * 8)
			ctx->bitlen[0]++;
		buf += copylen;
		len -= copylen;
	}
}

void
sha384_final(sha384_ctx *ctx, uint8_t *digest)
{

	ctx->block[ctx->blocklen++] = 0x80;
	memset(ctx->block + ctx->blocklen, 0,
	    sizeof ctx->block - ctx->blocklen);
	if (ctx->blocklen > 112) {
		sha384_compute(ctx, ctx->block);
		ctx->blocklen = 0;
		memset(ctx->block, 0, sizeof ctx->block);
	}
	be64encv(ctx->block + 112, ctx->bitlen, 2);
	sha384_compute(ctx, ctx->block);
	be64encv(digest, ctx->h, 6);
	memset_s(ctx, 0, sizeof *ctx, sizeof *ctx);
}

void
sha384_complete(const void *buf, size_t len, uint8_t *digest)
{
	sha384_ctx ctx;

	sha384_init(&ctx);
	sha384_update(&ctx, buf, len);
	sha384_final(&ctx, digest);
}

digest_algorithm sha384_digest = {
	.name			 = "sha384",
	.contextlen		 = sizeof(sha384_ctx),
	.blocklen		 = SHA384_BLOCK_LEN,
	.digestlen		 = SHA384_DIGEST_LEN,
	.init			 = (digest_init_func)(void *)sha384_init,
	.update			 = (digest_update_func)(void *)sha384_update,
	.final			 = (digest_final_func)(void *)sha384_final,
	.complete		 = (digest_complete_func)(void *)sha384_complete,
};
