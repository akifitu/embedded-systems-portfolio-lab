#include "sha256.h"

#include "security_types.h"

typedef struct {
    uint8_t data[64];
    uint32_t state[8];
    uint64_t bit_length;
    size_t data_length;
} san_sha256_ctx_t;

static const uint32_t k_san_sha256_constants[64] = {
    0x428A2F98u, 0x71374491u, 0xB5C0FBCFu, 0xE9B5DBA5u, 0x3956C25Bu,
    0x59F111F1u, 0x923F82A4u, 0xAB1C5ED5u, 0xD807AA98u, 0x12835B01u,
    0x243185BEu, 0x550C7DC3u, 0x72BE5D74u, 0x80DEB1FEu, 0x9BDC06A7u,
    0xC19BF174u, 0xE49B69C1u, 0xEFBE4786u, 0x0FC19DC6u, 0x240CA1CCu,
    0x2DE92C6Fu, 0x4A7484AAu, 0x5CB0A9DCu, 0x76F988DAu, 0x983E5152u,
    0xA831C66Du, 0xB00327C8u, 0xBF597FC7u, 0xC6E00BF3u, 0xD5A79147u,
    0x06CA6351u, 0x14292967u, 0x27B70A85u, 0x2E1B2138u, 0x4D2C6DFCu,
    0x53380D13u, 0x650A7354u, 0x766A0ABBu, 0x81C2C92Eu, 0x92722C85u,
    0xA2BFE8A1u, 0xA81A664Bu, 0xC24B8B70u, 0xC76C51A3u, 0xD192E819u,
    0xD6990624u, 0xF40E3585u, 0x106AA070u, 0x19A4C116u, 0x1E376C08u,
    0x2748774Cu, 0x34B0BCB5u, 0x391C0CB3u, 0x4ED8AA4Au, 0x5B9CCA4Fu,
    0x682E6FF3u, 0x748F82EEu, 0x78A5636Fu, 0x84C87814u, 0x8CC70208u,
    0x90BEFFFAu, 0xA4506CEBu, 0xBEF9A3F7u, 0xC67178F2u,
};

static uint32_t rotr32(uint32_t value, uint32_t count) {
    return (value >> count) | (value << (32u - count));
}

static uint32_t choose32(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

static uint32_t majority32(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

static uint32_t big_sigma0(uint32_t x) {
    return rotr32(x, 2u) ^ rotr32(x, 13u) ^ rotr32(x, 22u);
}

static uint32_t big_sigma1(uint32_t x) {
    return rotr32(x, 6u) ^ rotr32(x, 11u) ^ rotr32(x, 25u);
}

static uint32_t small_sigma0(uint32_t x) {
    return rotr32(x, 7u) ^ rotr32(x, 18u) ^ (x >> 3u);
}

static uint32_t small_sigma1(uint32_t x) {
    return rotr32(x, 17u) ^ rotr32(x, 19u) ^ (x >> 10u);
}

static void san_sha256_transform(san_sha256_ctx_t *ctx,
                                 const uint8_t data[64]) {
    uint32_t schedule[64];
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t g;
    uint32_t h;

    for (size_t i = 0; i < 16u; ++i) {
        schedule[i] = ((uint32_t)data[i * 4u] << 24u) |
                      ((uint32_t)data[i * 4u + 1u] << 16u) |
                      ((uint32_t)data[i * 4u + 2u] << 8u) |
                      ((uint32_t)data[i * 4u + 3u]);
    }

    for (size_t i = 16u; i < 64u; ++i) {
        schedule[i] = small_sigma1(schedule[i - 2u]) + schedule[i - 7u] +
                      small_sigma0(schedule[i - 15u]) + schedule[i - 16u];
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (size_t i = 0; i < 64u; ++i) {
        uint32_t temp1 = h + big_sigma1(e) + choose32(e, f, g) +
                         k_san_sha256_constants[i] + schedule[i];
        uint32_t temp2 = big_sigma0(a) + majority32(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

static void san_sha256_init(san_sha256_ctx_t *ctx) {
    ctx->data_length = 0u;
    ctx->bit_length = 0u;
    ctx->state[0] = 0x6A09E667u;
    ctx->state[1] = 0xBB67AE85u;
    ctx->state[2] = 0x3C6EF372u;
    ctx->state[3] = 0xA54FF53Au;
    ctx->state[4] = 0x510E527Fu;
    ctx->state[5] = 0x9B05688Cu;
    ctx->state[6] = 0x1F83D9ABu;
    ctx->state[7] = 0x5BE0CD19u;
}

static void san_sha256_update(san_sha256_ctx_t *ctx, const uint8_t *data,
                              size_t length) {
    for (size_t i = 0; i < length; ++i) {
        ctx->data[ctx->data_length++] = data[i];
        if (ctx->data_length == 64u) {
            san_sha256_transform(ctx, ctx->data);
            ctx->bit_length += 512u;
            ctx->data_length = 0u;
        }
    }
}

static void san_sha256_final(san_sha256_ctx_t *ctx,
                             uint8_t digest_out[SAN_HASH_SIZE]) {
    size_t i = ctx->data_length;

    ctx->bit_length += (uint64_t)ctx->data_length * 8u;
    ctx->data[i++] = 0x80u;

    if (i > 56u) {
        while (i < 64u) {
            ctx->data[i++] = 0u;
        }
        san_sha256_transform(ctx, ctx->data);
        i = 0u;
    }

    while (i < 56u) {
        ctx->data[i++] = 0u;
    }

    ctx->data[56] = (uint8_t)(ctx->bit_length >> 56u);
    ctx->data[57] = (uint8_t)(ctx->bit_length >> 48u);
    ctx->data[58] = (uint8_t)(ctx->bit_length >> 40u);
    ctx->data[59] = (uint8_t)(ctx->bit_length >> 32u);
    ctx->data[60] = (uint8_t)(ctx->bit_length >> 24u);
    ctx->data[61] = (uint8_t)(ctx->bit_length >> 16u);
    ctx->data[62] = (uint8_t)(ctx->bit_length >> 8u);
    ctx->data[63] = (uint8_t)(ctx->bit_length);
    san_sha256_transform(ctx, ctx->data);

    for (i = 0; i < 8u; ++i) {
        digest_out[i * 4u] = (uint8_t)(ctx->state[i] >> 24u);
        digest_out[i * 4u + 1u] = (uint8_t)(ctx->state[i] >> 16u);
        digest_out[i * 4u + 2u] = (uint8_t)(ctx->state[i] >> 8u);
        digest_out[i * 4u + 3u] = (uint8_t)(ctx->state[i]);
    }
}

void san_sha256_compute(const uint8_t *data, size_t length,
                        uint8_t digest_out[SAN_HASH_SIZE]) {
    san_sha256_ctx_t ctx;

    san_sha256_init(&ctx);
    san_sha256_update(&ctx, data, length);
    san_sha256_final(&ctx, digest_out);
}

