// MISC/sha2/sha-256.cpp - Qt6 port of SHA-256 implementation
#include "sha-256.h"

const uint32_t SHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9d4, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const unsigned char SHA256::PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

SHA256::SHA256()
{
    init();
}

void SHA256::init()
{
    m_state[0] = 0x6a09e667;
    m_state[1] = 0xbb67ae85;
    m_state[2] = 0x3c6ef372;
    m_state[3] = 0xa54ff53a;
    m_state[4] = 0x510e527f;
    m_state[5] = 0x9b05688c;
    m_state[6] = 0x1f83d9ab;
    m_state[7] = 0x5be0cd19;
    m_bitcount[0] = m_bitcount[1] = 0;
}

#define ROR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define Ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define Maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROR(x, 2) ^ ROR(x, 13) ^ ROR(x, 22))
#define EP1(x) (ROR(x, 6) ^ ROR(x, 11) ^ ROR(x, 25))
#define SIG0(x) (ROR(x, 7) ^ ROR(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROR(x, 17) ^ ROR(x, 19) ^ ((x) >> 10))

void SHA256::transform(const unsigned char* buffer)
{
    uint32_t w[64], a, b, c, d, e, f, g, h, t1, t2;
    int i;

    for (i = 0; i < 16; i++) {
        w[i] = (buffer[i * 4] << 24) |
               (buffer[i * 4 + 1] << 16) |
               (buffer[i * 4 + 2] << 8) |
               buffer[i * 4 + 3];
    }
    for (i = 16; i < 64; i++) {
        w[i] = SIG1(w[i-2]) + w[i-7] + SIG0(w[i-15]) + w[i-16];
    }

    a = m_state[0]; b = m_state[1]; c = m_state[2]; d = m_state[3];
    e = m_state[4]; f = m_state[5]; g = m_state[6]; h = m_state[7];

    for (i = 0; i < 64; i++) {
        t1 = h + EP1(e) + Ch(e, f, g) + K[i] + w[i];
        t2 = EP0(a) + Maj(a, b, c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }

    m_state[0] += a; m_state[1] += b; m_state[2] += c; m_state[3] += d;
    m_state[4] += e; m_state[5] += f; m_state[6] += g; m_state[7] += h;
}

#undef ROR
#undef Ch
#undef Maj
#undef EP0
#undef EP1
#undef SIG0
#undef SIG1

void SHA256::update(const unsigned char* input, size_t len)
{
    uint32_t i, j;

    j = (m_bitcount[0] >> 3) & 63;
    if ((m_bitcount[0] += static_cast<uint32_t>(len << 3)) < (len << 3))
        m_bitcount[1]++;
    m_bitcount[1] += static_cast<uint32_t>(len >> 29);

    if (j + len > 63) {
        memcpy(m_buffer + j, input, (i = 64 - j));
        transform(m_buffer);
        for (i = 0; i + 63 < len; i += 64)
            transform(input + i);
        j = 0;
    } else {
        i = 0;
    }
    memcpy(m_buffer + j, input + i, len - i);
}

void SHA256::final(unsigned char* digest)
{
    unsigned char finalcount[8];
    int i;

    for (i = 0; i < 8; i++) {
        finalcount[i] = (m_bitcount[(i >= 4 ? 1 : 0)] >> ((3 - (i & 3)) * 8)) & 255;
    }

    update(PADDING, 1);
    while ((m_bitcount[0] & 63) != 56)
        update(PADDING, 1);
    update(finalcount, 8);

    for (i = 0; i < 32; i++) {
        digest[i] = (m_state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255;
    }
}

void SHA256::hash(const unsigned char* input, size_t len, unsigned char* digest)
{
    SHA256 sha;
    sha.update(input, len);
    sha.final(digest);
}

void calc_sha_256(unsigned char hash[32], const unsigned char* input, size_t len)
{
    SHA256::hash(input, len, hash);
}