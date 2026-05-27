// MISC/sha512/sha512.cpp - Qt6 port of SHA-512 implementation
#include "sha512.h"
#include <cstring>

const uint64_t SHA512::K[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1a605120ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e964ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c626c29ULL, 0x4d2c6dfc5ac42a42ULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x43121467f9f7c7caULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475708ULL
};

const unsigned char SHA512::PADDING[128] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

SHA512::SHA512()
{
    init();
}

void SHA512::init()
{
    m_state[0] = 0x6a09e667f3bcc908ULL;
    m_state[1] = 0xbb67ae8584caa73bULL;
    m_state[2] = 0x3c6ef372fe94f82bULL;
    m_state[3] = 0xa54ff53a5f1d36f1ULL;
    m_state[4] = 0x510e527fade682d1ULL;
    m_state[5] = 0x9b05688c2b3e6c1fULL;
    m_state[6] = 0x1f83d9abfb41bd6bULL;
    m_state[7] = 0x5be0cd19137e2179ULL;
    m_bitcount[0] = m_bitcount[1] = 0;
}

#define ROR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))
#define Ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define Maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROR64(x, 28) ^ ROR64(x, 34) ^ ROR64(x, 39))
#define EP1(x) (ROR64(x, 14) ^ ROR64(x, 18) ^ ROR64(x, 41))
#define SIG0(x) (ROR64(x, 1) ^ ROR64(x, 8) ^ ((x) >> 7))
#define SIG1(x) (ROR64(x, 19) ^ ROR64(x, 61) ^ ((x) >> 6))

void SHA512::transform(const unsigned char* buffer)
{
    uint64_t w[80], a, b, c, d, e, f, g, h, t1, t2;
    int i;

    for (i = 0; i < 16; i++) {
        w[i] = (static_cast<uint64_t>(buffer[i * 8]) << 56) |
               (static_cast<uint64_t>(buffer[i * 8 + 1]) << 48) |
               (static_cast<uint64_t>(buffer[i * 8 + 2]) << 40) |
               (static_cast<uint64_t>(buffer[i * 8 + 3]) << 32) |
               (static_cast<uint64_t>(buffer[i * 8 + 4]) << 24) |
               (static_cast<uint64_t>(buffer[i * 8 + 5]) << 16) |
               (static_cast<uint64_t>(buffer[i * 8 + 6]) << 8) |
               (static_cast<uint64_t>(buffer[i * 8 + 7]));
    }
    for (i = 16; i < 80; i++) {
        w[i] = SIG1(w[i-2]) + w[i-7] + SIG0(w[i-15]) + w[i-16];
    }

    a = m_state[0]; b = m_state[1]; c = m_state[2]; d = m_state[3];
    e = m_state[4]; f = m_state[5]; g = m_state[6]; h = m_state[7];

    for (i = 0; i < 80; i++) {
        t1 = h + EP1(e) + Ch(e, f, g) + K[i] + w[i];
        t2 = EP0(a) + Maj(a, b, c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }

    m_state[0] += a; m_state[1] += b; m_state[2] += c; m_state[3] += d;
    m_state[4] += e; m_state[5] += f; m_state[6] += g; m_state[7] += h;
}

#undef ROR64
#undef Ch
#undef Maj
#undef EP0
#undef EP1
#undef SIG0
#undef SIG1

void SHA512::update(const unsigned char* input, size_t len)
{
    uint64_t i, j;

    j = (m_bitcount[0] >> 3) & 127;
    if ((m_bitcount[0] += (static_cast<uint64_t>(len) << 3)) < (static_cast<uint64_t>(len) << 3))
        m_bitcount[1]++;
    m_bitcount[1] += (static_cast<uint64_t>(len) >> 29);

    if (j + len > 127) {
        memcpy(m_buffer + j, input, (i = 128 - j));
        transform(m_buffer);
        for (i = 0; i + 127 < len; i += 128)
            transform(input + i);
        j = 0;
    } else {
        i = 0;
    }
    memcpy(m_buffer + j, input + i, len - i);
}

void SHA512::final(unsigned char* digest)
{
    unsigned char finalcount[16];
    int i;

    for (i = 0; i < 8; i++) {
        finalcount[i] = (m_bitcount[(i >= 4 ? 1 : 0)] >> ((3 - (i & 3)) * 8)) & 255;
    }

    update(PADDING, 1);
    while ((m_bitcount[0] & 127) != 112)
        update(PADDING, 1);
    update(finalcount, 16);

    for (i = 0; i < 64; i++) {
        digest[i] = (m_state[i >> 3] >> ((8 - (i & 7)) * 8)) & 255;
    }
}

void SHA512::hash(const unsigned char* input, size_t len, unsigned char* digest)
{
    SHA512 sha;
    sha.update(input, len);
    sha.final(digest);
}

void calc_sha_512(unsigned char hash[64], const unsigned char* input, size_t len)
{
    SHA512::hash(input, len, hash);
}