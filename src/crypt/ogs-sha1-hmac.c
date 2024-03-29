/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
/*-
 * HMAC-SHA-224/256/384/512 implementation
 * Last update: 06/15/2005
 * Issue date:  06/15/2005
 *
 * Copyright (C) 2005 Olivier Gay <olivier.gay@a3.epfl.ch>
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "ogs-crypt.h"

void ogs_hmac_sha1_init(ogs_hmac_sha1_ctx *ctx, uint8_t *key,
                      uint32_t key_size)
{
    uint32_t fill;
    uint32_t num;

    uint8_t *key_used;
    uint8_t key_temp[OGS_SHA1_DIGEST_SIZE];
    int i;

    if (key_size == OGS_SHA1_BLOCK_SIZE) {
        key_used = key;
        num = OGS_SHA1_BLOCK_SIZE;
    } else {
        if (key_size > OGS_SHA1_BLOCK_SIZE){
            key_used = key_temp;
            num = OGS_SHA1_DIGEST_SIZE;
            ogs_sha1(key, key_size, key_used);
        } else { /* key_size > SHA1_BLOCK_SIZE */
            key_used = key;
            num = key_size;
        }
        fill = OGS_SHA1_BLOCK_SIZE - num;

        memset(ctx->block_ipad + num, 0x36, fill);
        memset(ctx->block_opad + num, 0x5c, fill);
    }

    for (i = 0; i < num; i++) {
        ctx->block_ipad[i] = key_used[i] ^ 0x36;
        ctx->block_opad[i] = key_used[i] ^ 0x5c;
    }

    ogs_sha1_init(&ctx->ctx_inside);
    ogs_sha1_update(&ctx->ctx_inside, ctx->block_ipad, OGS_SHA1_BLOCK_SIZE);

    ogs_sha1_init(&ctx->ctx_outside);
    ogs_sha1_update(&ctx->ctx_outside, ctx->block_opad,
                  OGS_SHA1_BLOCK_SIZE);

    /* for hmac_reinit */
    memcpy(&ctx->ctx_inside_reinit, &ctx->ctx_inside,
           sizeof(ogs_sha1_ctx));
    memcpy(&ctx->ctx_outside_reinit, &ctx->ctx_outside,
           sizeof(ogs_sha1_ctx));
}

void ogs_hmac_sha1_reinit(ogs_hmac_sha1_ctx *ctx)
{
    memcpy(&ctx->ctx_inside, &ctx->ctx_inside_reinit,
           sizeof(ogs_sha1_ctx));
    memcpy(&ctx->ctx_outside, &ctx->ctx_outside_reinit,
           sizeof(ogs_sha1_ctx));
}

void ogs_hmac_sha1_update(ogs_hmac_sha1_ctx *ctx, uint8_t *message,
                        uint32_t message_len)
{
    ogs_sha1_update(&ctx->ctx_inside, message, message_len);
}

void ogs_hmac_sha1_final(ogs_hmac_sha1_ctx *ctx, uint8_t *mac,
                       uint32_t mac_size)
{
    uint8_t digest_inside[OGS_SHA1_DIGEST_SIZE];
    uint8_t mac_temp[OGS_SHA1_DIGEST_SIZE];

    ogs_sha1_final(&ctx->ctx_inside, digest_inside);
    ogs_sha1_update(&ctx->ctx_outside, digest_inside, OGS_SHA1_DIGEST_SIZE);
    ogs_sha1_final(&ctx->ctx_outside, mac_temp);
    memcpy(mac, mac_temp, mac_size);
}

void ogs_hmac_sha1(uint8_t *key, uint32_t key_size,
          uint8_t *message, uint32_t message_len,
          uint8_t *mac, uint32_t mac_size)
{
    ogs_hmac_sha1_ctx ctx;

    ogs_hmac_sha1_init(&ctx, key, key_size);
    ogs_hmac_sha1_update(&ctx, message, message_len);
    ogs_hmac_sha1_final(&ctx, mac, mac_size);
}
