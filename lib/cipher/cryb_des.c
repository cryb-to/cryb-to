/*-
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

#include <cryb/des.h>

/*
 * Expanded DES S-boxes
 */
static const uint32_t sb1[64] = {
	0x01010400, 0x00000000, 0x00010000, 0x01010404,
	0x01010004, 0x00010404, 0x00000004, 0x00010000,
	0x00000400, 0x01010400, 0x01010404, 0x00000400,
	0x01000404, 0x01010004, 0x01000000, 0x00000004,
	0x00000404, 0x01000400, 0x01000400, 0x00010400,
	0x00010400, 0x01010000, 0x01010000, 0x01000404,
	0x00010004, 0x01000004, 0x01000004, 0x00010004,
	0x00000000, 0x00000404, 0x00010404, 0x01000000,
	0x00010000, 0x01010404, 0x00000004, 0x01010000,
	0x01010400, 0x01000000, 0x01000000, 0x00000400,
	0x01010004, 0x00010000, 0x00010400, 0x01000004,
	0x00000400, 0x00000004, 0x01000404, 0x00010404,
	0x01010404, 0x00010004, 0x01010000, 0x01000404,
	0x01000004, 0x00000404, 0x00010404, 0x01010400,
	0x00000404, 0x01000400, 0x01000400, 0x00000000,
	0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const uint32_t sb2[64] = {
	0x80108020, 0x80008000, 0x00008000, 0x00108020,
	0x00100000, 0x00000020, 0x80100020, 0x80008020,
	0x80000020, 0x80108020, 0x80108000, 0x80000000,
	0x80008000, 0x00100000, 0x00000020, 0x80100020,
	0x00108000, 0x00100020, 0x80008020, 0x00000000,
	0x80000000, 0x00008000, 0x00108020, 0x80100000,
	0x00100020, 0x80000020, 0x00000000, 0x00108000,
	0x00008020, 0x80108000, 0x80100000, 0x00008020,
	0x00000000, 0x00108020, 0x80100020, 0x00100000,
	0x80008020, 0x80100000, 0x80108000, 0x00008000,
	0x80100000, 0x80008000, 0x00000020, 0x80108020,
	0x00108020, 0x00000020, 0x00008000, 0x80000000,
	0x00008020, 0x80108000, 0x00100000, 0x80000020,
	0x00100020, 0x80008020, 0x80000020, 0x00100020,
	0x00108000, 0x00000000, 0x80008000, 0x00008020,
	0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const uint32_t sb3[64] = {
	0x00000208, 0x08020200, 0x00000000, 0x08020008,
	0x08000200, 0x00000000, 0x00020208, 0x08000200,
	0x00020008, 0x08000008, 0x08000008, 0x00020000,
	0x08020208, 0x00020008, 0x08020000, 0x00000208,
	0x08000000, 0x00000008, 0x08020200, 0x00000200,
	0x00020200, 0x08020000, 0x08020008, 0x00020208,
	0x08000208, 0x00020200, 0x00020000, 0x08000208,
	0x00000008, 0x08020208, 0x00000200, 0x08000000,
	0x08020200, 0x08000000, 0x00020008, 0x00000208,
	0x00020000, 0x08020200, 0x08000200, 0x00000000,
	0x00000200, 0x00020008, 0x08020208, 0x08000200,
	0x08000008, 0x00000200, 0x00000000, 0x08020008,
	0x08000208, 0x00020000, 0x08000000, 0x08020208,
	0x00000008, 0x00020208, 0x00020200, 0x08000008,
	0x08020000, 0x08000208, 0x00000208, 0x08020000,
	0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const uint32_t sb4[64] = {
	0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802080, 0x00800081, 0x00800001, 0x00002001,
	0x00000000, 0x00802000, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00800080, 0x00800001,
	0x00000001, 0x00002000, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002001, 0x00002080,
	0x00800081, 0x00000001, 0x00002080, 0x00800080,
	0x00002000, 0x00802080, 0x00802081, 0x00000081,
	0x00800080, 0x00800001, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00000000, 0x00802000,
	0x00002080, 0x00800080, 0x00800081, 0x00000001,
	0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802081, 0x00000081, 0x00000001, 0x00002000,
	0x00800001, 0x00002001, 0x00802080, 0x00800081,
	0x00002001, 0x00002080, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const uint32_t sb5[64] = {
	0x00000100, 0x02080100, 0x02080000, 0x42000100,
	0x00080000, 0x00000100, 0x40000000, 0x02080000,
	0x40080100, 0x00080000, 0x02000100, 0x40080100,
	0x42000100, 0x42080000, 0x00080100, 0x40000000,
	0x02000000, 0x40080000, 0x40080000, 0x00000000,
	0x40000100, 0x42080100, 0x42080100, 0x02000100,
	0x42080000, 0x40000100, 0x00000000, 0x42000000,
	0x02080100, 0x02000000, 0x42000000, 0x00080100,
	0x00080000, 0x42000100, 0x00000100, 0x02000000,
	0x40000000, 0x02080000, 0x42000100, 0x40080100,
	0x02000100, 0x40000000, 0x42080000, 0x02080100,
	0x40080100, 0x00000100, 0x02000000, 0x42080000,
	0x42080100, 0x00080100, 0x42000000, 0x42080100,
	0x02080000, 0x00000000, 0x40080000, 0x42000000,
	0x00080100, 0x02000100, 0x40000100, 0x00080000,
	0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const uint32_t sb6[64] = {
	0x20000010, 0x20400000, 0x00004000, 0x20404010,
	0x20400000, 0x00000010, 0x20404010, 0x00400000,
	0x20004000, 0x00404010, 0x00400000, 0x20000010,
	0x00400010, 0x20004000, 0x20000000, 0x00004010,
	0x00000000, 0x00400010, 0x20004010, 0x00004000,
	0x00404000, 0x20004010, 0x00000010, 0x20400010,
	0x20400010, 0x00000000, 0x00404010, 0x20404000,
	0x00004010, 0x00404000, 0x20404000, 0x20000000,
	0x20004000, 0x00000010, 0x20400010, 0x00404000,
	0x20404010, 0x00400000, 0x00004010, 0x20000010,
	0x00400000, 0x20004000, 0x20000000, 0x00004010,
	0x20000010, 0x20404010, 0x00404000, 0x20400000,
	0x00404010, 0x20404000, 0x00000000, 0x20400010,
	0x00000010, 0x00004000, 0x20400000, 0x00404010,
	0x00004000, 0x00400010, 0x20004010, 0x00000000,
	0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const uint32_t sb7[64] = {
	0x00200000, 0x04200002, 0x04000802, 0x00000000,
	0x00000800, 0x04000802, 0x00200802, 0x04200800,
	0x04200802, 0x00200000, 0x00000000, 0x04000002,
	0x00000002, 0x04000000, 0x04200002, 0x00000802,
	0x04000800, 0x00200802, 0x00200002, 0x04000800,
	0x04000002, 0x04200000, 0x04200800, 0x00200002,
	0x04200000, 0x00000800, 0x00000802, 0x04200802,
	0x00200800, 0x00000002, 0x04000000, 0x00200800,
	0x04000000, 0x00200800, 0x00200000, 0x04000802,
	0x04000802, 0x04200002, 0x04200002, 0x00000002,
	0x00200002, 0x04000000, 0x04000800, 0x00200000,
	0x04200800, 0x00000802, 0x00200802, 0x04200800,
	0x00000802, 0x04000002, 0x04200802, 0x04200000,
	0x00200800, 0x00000000, 0x00000002, 0x04200802,
	0x00000000, 0x00200802, 0x04200000, 0x00000800,
	0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const uint32_t sb8[64] = {
	0x10001040, 0x00001000, 0x00040000, 0x10041040,
	0x10000000, 0x10001040, 0x00000040, 0x10000000,
	0x00040040, 0x10040000, 0x10041040, 0x00041000,
	0x10041000, 0x00041040, 0x00001000, 0x00000040,
	0x10040000, 0x10000040, 0x10001000, 0x00001040,
	0x00041000, 0x00040040, 0x10040040, 0x10041000,
	0x00001040, 0x00000000, 0x00000000, 0x10040040,
	0x10000040, 0x10001000, 0x00041040, 0x00040000,
	0x00041040, 0x00040000, 0x10041000, 0x00001000,
	0x00000040, 0x10040040, 0x00001000, 0x00041040,
	0x10001000, 0x00000040, 0x10000040, 0x10040000,
	0x10040040, 0x10000000, 0x00040000, 0x10001040,
	0x00000000, 0x10041040, 0x00040040, 0x10000040,
	0x10040000, 0x10001000, 0x10001040, 0x00000000,
	0x10041040, 0x00041000, 0x00041000, 0x00001040,
	0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
 * PC1: left and right halves bit-swap
 */
static const uint32_t lhs[16] = {
	0x00000000, 0x00000001, 0x00000100, 0x00000101,
	0x00010000, 0x00010001, 0x00010100, 0x00010101,
	0x01000000, 0x01000001, 0x01000100, 0x01000101,
	0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const uint32_t rhs[16] = {
	0x00000000, 0x01000000, 0x00010000, 0x01010000,
	0x00000100, 0x01000100, 0x00010100, 0x01010100,
	0x00000001, 0x01000001, 0x00010001, 0x01010001,
	0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
 * Initial Permutation macro
 */
#define DES_IP(x, y) \
	do {								\
		t = (x >>  4 ^ y) & 0x0f0f0f0f; y ^= t; x ^= t <<  4;	\
		t = (x >> 16 ^ y) & 0x0000ffff; y ^= t; x ^= t << 16;	\
		t = (y >>  2 ^ x) & 0x33333333; x ^= t; y ^= t <<  2;	\
		t = (y >>  8 ^ x) & 0x00ff00ff; x ^= t; y ^= t <<  8;	\
		y = rol32(y, 1) & 0xffffffff;				\
		t = (x ^ y) & 0xaaaaaaaa; y ^= t; x ^= t;		\
		x = rol32(x, 1) & 0xffffffff;				\
	} while (0)

/*
 * Final Permutation macro
 */
#define DES_FP(x, y) \
	do {								\
		x = ror32(x, 1) & 0xffffffff;				\
		t = (x ^ y) & 0xaaaaaaaa; x ^= t; y ^= t;		\
		y = ror32(y, 1) & 0xffffffff;				\
		t = (y >>  8 ^ x) & 0x00ff00ff; x ^= t; y ^= t <<  8;	\
		t = (y >>  2 ^ x) & 0x33333333; x ^= t; y ^= t <<  2;	\
		t = (x >> 16 ^ y) & 0x0000ffff; y ^= t; x ^= t << 16;	\
		t = (x >>  4 ^ y) & 0x0f0f0f0f; y ^= t; x ^= t <<  4;	\
	} while (0)

/*
 * DES round macro
 */
#define DES_ROUND(x, y)							\
	do {								\
		t = *sk++ ^ x;						\
		y ^=							\
		    sb8[t >>  0 & 0x3f] ^				\
		    sb6[t >>  8 & 0x3f] ^				\
		    sb4[t >> 16 & 0x3f] ^				\
		    sb2[t >> 24 & 0x3f];				\
		t = *sk++ ^ ror32(x, 4);				\
		y ^=							\
		    sb7[t >>  0 & 0x3f] ^				\
		    sb5[t >>  8 & 0x3f] ^				\
		    sb3[t >> 16 & 0x3f] ^				\
		    sb1[t >> 24 & 0x3f];				\
	} while (0)

#define SWAP(a, b)							\
	do {								\
		(a) ^= (b); (b) ^= (a); (a) ^= (b);			\
	} while (0)

static void
des_setkey(cipher_mode mode, uint32_t sk[32], const uint8_t key[8])
{
	uint32_t x, y, t;
	unsigned int i;

	x = be32dec(key + 0);
	y = be32dec(key + 4);

	/* permuted choice 1 */
	t = (y >>  4 ^ x) & 0x0f0f0f0f; x ^= t; y ^= t <<  4;
	t = (y >>  0 ^ x) & 0x10101010; x ^= t; y ^= t <<  0;
	x = lhs[x >>  0 & 0xf] << 3 | lhs[x >>	8 & 0xf] << 2 |
	    lhs[x >> 16 & 0xf] << 1 | lhs[x >> 24 & 0xf] << 0 |
	    lhs[x >>  5 & 0xf] << 7 | lhs[x >> 13 & 0xf] << 6 |
	    lhs[x >> 21 & 0xf] << 5 | lhs[x >> 29 & 0xf] << 4;
	y = rhs[y >>  1 & 0xf] << 3 | rhs[y >>	9 & 0xf] << 2 |
	    rhs[y >> 17 & 0xf] << 1 | rhs[y >> 25 & 0xf] << 0 |
	    rhs[y >>  4 & 0xf] << 7 | rhs[y >> 12 & 0xf] << 6 |
	    rhs[y >> 20 & 0xf] << 5 | rhs[y >> 28 & 0xf] << 4;
	x &= 0x0FFFFFFF;
	y &= 0x0FFFFFFF;

	/* calculate subkeys */
	for (i = 0; i < 16; i++) {
		if (i < 2 || i == 8 || i == 15) {
			x = (x <<  1 | x >> 27) & 0x0FFFFFFF;
			y = (y <<  1 | y >> 27) & 0x0FFFFFFF;
		} else {
			x = (x <<  2 | x >> 26) & 0x0FFFFFFF;
			y = (y <<  2 | y >> 26) & 0x0FFFFFFF;
		}
		sk[i * 2] =
		    (x <<  4 & 0x24000000) | (x << 28 & 0x10000000) |
		    (x << 14 & 0x08000000) | (x << 18 & 0x02080000) |
		    (x <<  6 & 0x01000000) | (x <<  9 & 0x00200000) |
		    (x >>  1 & 0x00100000) | (x << 10 & 0x00040000) |
		    (x <<  2 & 0x00020000) | (x >> 10 & 0x00010000) |
		    (y >> 13 & 0x00002000) | (y >>  4 & 0x00001000) |
		    (y <<  6 & 0x00000800) | (y >>  1 & 0x00000400) |
		    (y >> 14 & 0x00000200) | (y >>  0 & 0x00000100) |
		    (y >>  5 & 0x00000020) | (y >> 10 & 0x00000010) |
		    (y >>  3 & 0x00000008) | (y >> 18 & 0x00000004) |
		    (y >> 26 & 0x00000002) | (y >> 24 & 0x00000001);
		sk[i * 2 + 1] =
		    (x << 15 & 0x20000000) | (x << 17 & 0x10000000) |
		    (x << 10 & 0x08000000) | (x << 22 & 0x04000000) |
		    (x >>  2 & 0x02000000) | (x <<  1 & 0x01000000) |
		    (x << 16 & 0x00200000) | (x << 11 & 0x00100000) |
		    (x <<  3 & 0x00080000) | (x >>  6 & 0x00040000) |
		    (x << 15 & 0x00020000) | (x >>  4 & 0x00010000) |
		    (y >>  2 & 0x00002000) | (y <<  8 & 0x00001000) |
		    (y >> 14 & 0x00000808) | (y >>  9 & 0x00000400) |
		    (y >>  0 & 0x00000200) | (y <<  7 & 0x00000100) |
		    (y >>  7 & 0x00000020) | (y >>  3 & 0x00000011) |
		    (y <<  2 & 0x00000004) | (y >> 21 & 0x00000002);
	}
	if (mode == CIPHER_MODE_DECRYPT) {
		for (i = 0; i < 16; i += 2) {
			SWAP(sk[i + 0], sk[30 - i]);
			SWAP(sk[i + 1], sk[31 - i]);
		}
	}
}

/*
 * DES-ECB block encryption / decryption
 */
static void
des_crypt_ecb(des_ctx *ctx, const uint8_t input[8], uint8_t output[8])
{
	uint32_t x, y, t, *sk;
	unsigned int i;

	x = be32dec(input + 0);
	y = be32dec(input + 4);
	sk = ctx->sk1;
	DES_IP(x, y);
	for (i = 0; i < 8; ++i) {
		DES_ROUND(y, x);
		DES_ROUND(x, y);
	}
	DES_FP(y, x);
	SWAP(x, y);
	sk = ctx->sk2;
	DES_IP(x, y);
	for (i = 0; i < 8; ++i) {
		DES_ROUND(y, x);
		DES_ROUND(x, y);
	}
	DES_FP(y, x);
	SWAP(x, y);
	sk = ctx->sk3;
	DES_IP(x, y);
	for (i = 0; i < 8; ++i) {
		DES_ROUND(y, x);
		DES_ROUND(x, y);
	}
	DES_FP(y, x);
	be32enc(output + 0, y);
	be32enc(output + 4, x);
}

void
des_init(des_ctx *ctx, cipher_mode mode, const uint8_t *key, size_t keylen)
{
	cipher_mode m1, m2, m3;

	if (mode == CIPHER_MODE_DECRYPT) {
		m1 = m3 = CIPHER_MODE_DECRYPT;
		m2 = CIPHER_MODE_ENCRYPT;
	} else {
		m1 = m3 = CIPHER_MODE_ENCRYPT;
		m2 = CIPHER_MODE_DECRYPT;
	}
	des_setkey(m1, ctx->sk1, key);
	switch (keylen) {
	case 24:
		des_setkey(m2, ctx->sk2, key + 8);
		des_setkey(m3, ctx->sk3, key + 16);
		break;
	case 16:
		des_setkey(m2, ctx->sk2, key + 8);
		des_setkey(m3, ctx->sk3, key);
		break;
	case 8:
		des_setkey(m2, ctx->sk2, key);
		des_setkey(m3, ctx->sk3, key);
		break;
	}
}

size_t
des_encrypt(des_ctx *ctx, const void *vpt, uint8_t *ct, size_t len)
{
	const uint8_t *pt = vpt;
	unsigned int i;

	len -= len % DES_BLOCK_LEN;
	for (i = 0; i < len; i += DES_BLOCK_LEN) {
		des_crypt_ecb(ctx, pt, ct);
		pt += DES_BLOCK_LEN;
		ct += DES_BLOCK_LEN;
	}
	return (len);
}

size_t
des_decrypt(des_ctx *ctx, const uint8_t *ct, void *vpt, size_t len)
{
	uint8_t *pt = vpt;
	unsigned int i;

	len -= len % DES_BLOCK_LEN;
	for (i = 0; i < len; i += DES_BLOCK_LEN) {
		des_crypt_ecb(ctx, ct, pt);
		ct += DES_BLOCK_LEN;
		pt += DES_BLOCK_LEN;
	}
	return (len);
}

void
des_finish(des_ctx *ctx)
{

	memset_s(ctx, 0, sizeof *ctx, sizeof *ctx);
}

cipher_algorithm des56_cipher = {
	.name			 = "des",
	.contextlen		 = sizeof(des_ctx),
	.blocklen		 = DES_BLOCK_LEN,
	.keylen			 = DES_BLOCK_LEN,
	.init			 = (cipher_init_func)des_init,
	.encrypt		 = (cipher_encrypt_func)des_encrypt,
	.decrypt		 = (cipher_decrypt_func)des_decrypt,
	.finish			 = (cipher_finish_func)des_finish,
};

cipher_algorithm des112_cipher = {
	.name			 = "2des",
	.contextlen		 = sizeof(des_ctx),
	.blocklen		 = DES_BLOCK_LEN,
	.keylen			 = DES_BLOCK_LEN * 2,
	.init			 = (cipher_init_func)des_init,
	.encrypt		 = (cipher_encrypt_func)des_encrypt,
	.decrypt		 = (cipher_decrypt_func)des_decrypt,
	.finish			 = (cipher_finish_func)des_finish,
};

cipher_algorithm des168_cipher = {
	.name			 = "3des",
	.contextlen		 = sizeof(des_ctx),
	.blocklen		 = DES_BLOCK_LEN,
	.keylen			 = DES_BLOCK_LEN * 3,
	.init			 = (cipher_init_func)des_init,
	.encrypt		 = (cipher_encrypt_func)des_encrypt,
	.decrypt		 = (cipher_decrypt_func)des_decrypt,
	.finish			 = (cipher_finish_func)des_finish,
};
