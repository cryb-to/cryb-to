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

#include <cryb/memset_s.h>

#include <cryb/md2.h>

/*
 * MD2 - RFC 1319
 *
 * Note 1: the pseudocode for the checksum calculation in the RFC contains
 * a significant error - consult the errata.
 *
 * Note 2: the RFC describes the algorithm as having five steps: 1) append
 * padding, 2) compute checksum, 3) initialize state, 4) compute digest
 * and 5) output.  This implementation merges the checksum and digest
 * computations, which are independent of eachother, which avoids
 * processing the message twice.  The padding is then appended and fed
 * through the checksum and digest computation, and finally the checksum
 * itself is fed through.  Note that we update the checksum *after* the
 * digest, otherwise the final step (mixing the checksum into the digest)
 * would clobber the checksum before it was used.
 */

static const uint8_t md2_s[256] = {
	0x29, 0x2e, 0x43, 0xc9, 0xa2, 0xd8, 0x7c, 0x01,
	0x3d, 0x36, 0x54, 0xa1, 0xec, 0xf0, 0x06, 0x13,
	0x62, 0xa7, 0x05, 0xf3, 0xc0, 0xc7, 0x73, 0x8c,
	0x98, 0x93, 0x2b, 0xd9, 0xbc, 0x4c, 0x82, 0xca,
	0x1e, 0x9b, 0x57, 0x3c, 0xfd, 0xd4, 0xe0, 0x16,
	0x67, 0x42, 0x6f, 0x18, 0x8a, 0x17, 0xe5, 0x12,
	0xbe, 0x4e, 0xc4, 0xd6, 0xda, 0x9e, 0xde, 0x49,
	0xa0, 0xfb, 0xf5, 0x8e, 0xbb, 0x2f, 0xee, 0x7a,
	0xa9, 0x68, 0x79, 0x91, 0x15, 0xb2, 0x07, 0x3f,
	0x94, 0xc2, 0x10, 0x89, 0x0b, 0x22, 0x5f, 0x21,
	0x80, 0x7f, 0x5d, 0x9a, 0x5a, 0x90, 0x32, 0x27,
	0x35, 0x3e, 0xcc, 0xe7, 0xbf, 0xf7, 0x97, 0x03,
	0xff, 0x19, 0x30, 0xb3, 0x48, 0xa5, 0xb5, 0xd1,
	0xd7, 0x5e, 0x92, 0x2a, 0xac, 0x56, 0xaa, 0xc6,
	0x4f, 0xb8, 0x38, 0xd2, 0x96, 0xa4, 0x7d, 0xb6,
	0x76, 0xfc, 0x6b, 0xe2, 0x9c, 0x74, 0x04, 0xf1,
	0x45, 0x9d, 0x70, 0x59, 0x64, 0x71, 0x87, 0x20,
	0x86, 0x5b, 0xcf, 0x65, 0xe6, 0x2d, 0xa8, 0x02,
	0x1b, 0x60, 0x25, 0xad, 0xae, 0xb0, 0xb9, 0xf6,
	0x1c, 0x46, 0x61, 0x69, 0x34, 0x40, 0x7e, 0x0f,
	0x55, 0x47, 0xa3, 0x23, 0xdd, 0x51, 0xaf, 0x3a,
	0xc3, 0x5c, 0xf9, 0xce, 0xba, 0xc5, 0xea, 0x26,
	0x2c, 0x53, 0x0d, 0x6e, 0x85, 0x28, 0x84, 0x09,
	0xd3, 0xdf, 0xcd, 0xf4, 0x41, 0x81, 0x4d, 0x52,
	0x6a, 0xdc, 0x37, 0xc8, 0x6c, 0xc1, 0xab, 0xfa,
	0x24, 0xe1, 0x7b, 0x08, 0x0c, 0xbd, 0xb1, 0x4a,
	0x78, 0x88, 0x95, 0x8b, 0xe3, 0x63, 0xe8, 0x6d,
	0xe9, 0xcb, 0xd5, 0xfe, 0x3b, 0x00, 0x1d, 0x39,
	0xf2, 0xef, 0xb7, 0x0e, 0x66, 0x58, 0xd0, 0xe4,
	0xa6, 0x77, 0x72, 0xf8, 0xeb, 0x75, 0x4b, 0x0a,
	0x31, 0x44, 0x50, 0xb4, 0x8f, 0xed, 0x1f, 0x1a,
	0xdb, 0x99, 0x8d, 0x33, 0x9f, 0x11, 0x83, 0x14,
};

void
md2_init(md2_ctx *ctx)
{

	memset(ctx, 0, sizeof *ctx);
}

static void
md2_compute(md2_ctx *ctx, const uint8_t *block)
{
	unsigned int j, k;
	uint8_t l, t;

	for (j = 0; j < 16; j++) {
		ctx->state[j + 16] = block[j];
		ctx->state[j + 32] = block[j] ^ ctx->state[j];
	}
	t = 0;
	for (j = 0; j < 18; ++j) {
		for (k = 0; k < 48; ++k) {
			ctx->state[k] = ctx->state[k] ^ md2_s[t];
			t = ctx->state[k];
		}
		t = t + j;
	}
	l = ctx->cksum[15];
	for (j = 0; j < 16; ++j) {
		ctx->cksum[j] ^= md2_s[block[j] ^ l];
		l = ctx->cksum[j];
	}
}

void
md2_update(md2_ctx *ctx, const void *buf, size_t len)
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
				md2_compute(ctx, ctx->block);
				ctx->blocklen = 0;
			}
		} else {
			copylen = sizeof ctx->block;
			md2_compute(ctx, buf);
		}
		buf += copylen;
		len -= copylen;
	}
}

void
md2_final(md2_ctx *ctx, uint8_t *digest)
{
	unsigned int i;
	uint8_t x;

	x = sizeof ctx->block - ctx->blocklen;
	for (i = ctx->blocklen; i < sizeof ctx->block; i++)
		ctx->block[i] = x;
	md2_compute(ctx, ctx->block);
	md2_compute(ctx, ctx->cksum);
	memcpy(digest, ctx->state, MD2_DIGEST_LEN);
	memset_s(&ctx, 0, sizeof ctx, sizeof ctx);
}

void md2_complete(const void *buf, size_t len, uint8_t *digest)
{
	md2_ctx ctx;

	md2_init(&ctx);
	md2_update(&ctx, buf, len);
	md2_final(&ctx, digest);
}

digest_algorithm md2_digest = {
	.name			 = "md2",
	.contextlen		 = sizeof(md2_ctx),
	.blocklen		 = MD2_BLOCK_LEN,
	.digestlen		 = MD2_DIGEST_LEN,
	.init			 = (digest_init_func)md2_init,
	.update			 = (digest_update_func)md2_update,
	.final			 = (digest_final_func)md2_final,
	.complete		 = (digest_complete_func)md2_complete,
};
