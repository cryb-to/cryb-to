/*-
 * Copyright (c) 2005-2013 Colin Percival
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
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
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

#include <cryb/sha256.h>

/* Elementary functions used by SHA256 */
#define Ch(x, y, z)	((x & (y ^ z)) ^ z)
#define Maj(x, y, z)	((x & (y | z)) | (y & z))
#define S0(x)		(ror32(x,  2) ^ ror32(x, 13) ^ ror32(x, 22))
#define S1(x)		(ror32(x,  6) ^ ror32(x, 11) ^ ror32(x, 25))
#define s0(x)		(ror32(x,  7) ^ ror32(x, 18) ^ (x >>  3))
#define s1(x)		(ror32(x, 17) ^ ror32(x, 19) ^ (x >> 10))

/* SHA256 round function */
#define RND(a, b, c, d, e, f, g, h, k)			\
	t0 = h + S1(e) + Ch(e, f, g) + k;		\
	t1 = S0(a) + Maj(a, b, c);			\
	d += t0;					\
	h  = t0 + t1;

/* Adjusted round function for rotating state */
#define RNDr(S, W, i, k)			\
	RND(S[(64 - i) % 8], S[(65 - i) % 8],	\
	    S[(66 - i) % 8], S[(67 - i) % 8],	\
	    S[(68 - i) % 8], S[(69 - i) % 8],	\
	    S[(70 - i) % 8], S[(71 - i) % 8],	\
	    W[i] + k)

/*
 * SHA256 block compression function.  The 256-bit state is transformed via
 * the 512-bit input block to produce a new state.
 */
static void
sha256_Transform(uint32_t * state, const uint8_t block[64])
{
	uint32_t W[64];
	uint32_t S[8];
	uint32_t t0, t1;
	int i;

	/* 1. Prepare message schedule W. */
	be32decv(W, block, 16);
	for (i = 16; i < 64; i++)
		W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];

	/* 2. Initialize working variables. */
	memcpy(S, state, 32);

	/* 3. Mix. */
	RNDr(S, W,  0, 0x428a2f98);
	RNDr(S, W,  1, 0x71374491);
	RNDr(S, W,  2, 0xb5c0fbcf);
	RNDr(S, W,  3, 0xe9b5dba5);
	RNDr(S, W,  4, 0x3956c25b);
	RNDr(S, W,  5, 0x59f111f1);
	RNDr(S, W,  6, 0x923f82a4);
	RNDr(S, W,  7, 0xab1c5ed5);
	RNDr(S, W,  8, 0xd807aa98);
	RNDr(S, W,  9, 0x12835b01);
	RNDr(S, W, 10, 0x243185be);
	RNDr(S, W, 11, 0x550c7dc3);
	RNDr(S, W, 12, 0x72be5d74);
	RNDr(S, W, 13, 0x80deb1fe);
	RNDr(S, W, 14, 0x9bdc06a7);
	RNDr(S, W, 15, 0xc19bf174);
	RNDr(S, W, 16, 0xe49b69c1);
	RNDr(S, W, 17, 0xefbe4786);
	RNDr(S, W, 18, 0x0fc19dc6);
	RNDr(S, W, 19, 0x240ca1cc);
	RNDr(S, W, 20, 0x2de92c6f);
	RNDr(S, W, 21, 0x4a7484aa);
	RNDr(S, W, 22, 0x5cb0a9dc);
	RNDr(S, W, 23, 0x76f988da);
	RNDr(S, W, 24, 0x983e5152);
	RNDr(S, W, 25, 0xa831c66d);
	RNDr(S, W, 26, 0xb00327c8);
	RNDr(S, W, 27, 0xbf597fc7);
	RNDr(S, W, 28, 0xc6e00bf3);
	RNDr(S, W, 29, 0xd5a79147);
	RNDr(S, W, 30, 0x06ca6351);
	RNDr(S, W, 31, 0x14292967);
	RNDr(S, W, 32, 0x27b70a85);
	RNDr(S, W, 33, 0x2e1b2138);
	RNDr(S, W, 34, 0x4d2c6dfc);
	RNDr(S, W, 35, 0x53380d13);
	RNDr(S, W, 36, 0x650a7354);
	RNDr(S, W, 37, 0x766a0abb);
	RNDr(S, W, 38, 0x81c2c92e);
	RNDr(S, W, 39, 0x92722c85);
	RNDr(S, W, 40, 0xa2bfe8a1);
	RNDr(S, W, 41, 0xa81a664b);
	RNDr(S, W, 42, 0xc24b8b70);
	RNDr(S, W, 43, 0xc76c51a3);
	RNDr(S, W, 44, 0xd192e819);
	RNDr(S, W, 45, 0xd6990624);
	RNDr(S, W, 46, 0xf40e3585);
	RNDr(S, W, 47, 0x106aa070);
	RNDr(S, W, 48, 0x19a4c116);
	RNDr(S, W, 49, 0x1e376c08);
	RNDr(S, W, 50, 0x2748774c);
	RNDr(S, W, 51, 0x34b0bcb5);
	RNDr(S, W, 52, 0x391c0cb3);
	RNDr(S, W, 53, 0x4ed8aa4a);
	RNDr(S, W, 54, 0x5b9cca4f);
	RNDr(S, W, 55, 0x682e6ff3);
	RNDr(S, W, 56, 0x748f82ee);
	RNDr(S, W, 57, 0x78a5636f);
	RNDr(S, W, 58, 0x84c87814);
	RNDr(S, W, 59, 0x8cc70208);
	RNDr(S, W, 60, 0x90befffa);
	RNDr(S, W, 61, 0xa4506ceb);
	RNDr(S, W, 62, 0xbef9a3f7);
	RNDr(S, W, 63, 0xc67178f2);

	/* 4. Mix local working variables into global state. */
	for (i = 0; i < 8; i++)
		state[i] += S[i];
}

static uint8_t PAD[64] = {
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* Add padding and terminating bit-count. */
static void
sha256_pad(sha256_ctx * ctx)
{
	uint8_t len[8];
	uint32_t r, plen;

	/*
	 * Convert length to a vector of bytes -- we do this now rather
	 * than later because the length will change after we pad.
	 */
	be64enc(len, ctx->count);

	/* Add 1--64 bytes so that the resulting length is 56 mod 64. */
	r = (ctx->count >> 3) & 0x3f;
	plen = (r < 56) ? (56 - r) : (120 - r);
	sha256_update(ctx, PAD, (size_t)plen);

	/* Add the terminating bit-count. */
	sha256_update(ctx, len, 8);
}

/**
 * sha256_init(ctx):
 * Initialize the SHA256 context ${ctx}.
 */
void
sha256_init(sha256_ctx * ctx)
{

	/* Zero bits processed so far. */
	ctx->count = 0;

	/* Magic initialization constants. */
	ctx->state[0] = 0x6A09E667;
	ctx->state[1] = 0xBB67AE85;
	ctx->state[2] = 0x3C6EF372;
	ctx->state[3] = 0xA54FF53A;
	ctx->state[4] = 0x510E527F;
	ctx->state[5] = 0x9B05688C;
	ctx->state[6] = 0x1F83D9AB;
	ctx->state[7] = 0x5BE0CD19;
}

/**
 * sha256_update(ctx, in, len):
 * Input ${len} bytes from ${in} into the SHA256 context ${ctx}.
 */
void
sha256_update(sha256_ctx * ctx, const void *in, size_t len)
{
	uint32_t r;
	const uint8_t *src = in;

	/* Return immediately if we have nothing to do. */
	if (len == 0)
		return;

	/* Number of bytes left in the buffer from previous updates. */
	r = (ctx->count >> 3) & 0x3f;

	/* Update number of bits. */
	ctx->count += (uint64_t)(len) << 3;

	/* Handle the case where we don't need to perform any transforms. */
	if (len < 64 - r) {
		memcpy(&ctx->buf[r], src, len);
		return;
	}

	/* Finish the current block. */
	memcpy(&ctx->buf[r], src, 64 - r);
	sha256_Transform(ctx->state, ctx->buf);
	src += 64 - r;
	len -= 64 - r;

	/* Perform complete blocks. */
	while (len >= 64) {
		sha256_Transform(ctx->state, src);
		src += 64;
		len -= 64;
	}

	/* Copy left over data into buffer. */
	memcpy(ctx->buf, src, len);
}

/**
 * sha256_final(ctx, digest):
 * Output the SHA256 hash of the data input to the context ${ctx} into the
 * buffer ${digest}.
 */
void
sha256_final(sha256_ctx * ctx, uint8_t *digest)
{

	/* Add padding. */
	sha256_pad(ctx);

	/* Write the hash. */
	be32encv(digest, ctx->state, SHA256_DIGEST_LEN / 4);

	/* Clear the context state. */
	memset_s(ctx, 0, sizeof *ctx, sizeof *ctx);
}

/**
 * sha256_complete(in, len, digest):
 * Compute the SHA256 hash of ${len} bytes from $in} and write it to ${digest}.
 */
void
sha256_complete(const void *in, size_t len, uint8_t *digest)
{
	sha256_ctx ctx;

	sha256_init(&ctx);
	sha256_update(&ctx, in, len);
	sha256_final(&ctx, digest);
}

digest_algorithm sha256_digest = {
	.name			 = "sha256",
	.contextlen		 = sizeof(sha256_ctx),
	.blocklen		 = SHA256_BLOCK_LEN,
	.digestlen		 = SHA256_DIGEST_LEN,
	.init			 = (digest_init_func)(void *)sha256_init,
	.update			 = (digest_update_func)(void *)sha256_update,
	.final			 = (digest_final_func)(void *)sha256_final,
	.complete		 = (digest_complete_func)(void *)sha256_complete,
};
