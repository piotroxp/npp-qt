// MISC/sha1/sha1.cpp - Qt6 port of SHA1 implementation
#include "sha1.h"

const unsigned char SHA1::PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

SHA1::SHA1()
{
    reset();
}

void SHA1::reset()
{
    m_state[0] = 0x67452301;
    m_state[1] = 0xEFCDAB89;
    m_state[2] = 0x98BADCFE;
    m_state[3] = 0x10325476;
    m_state[4] = 0xC3D2E1F0;
    m_count[0] = m_count[1] = 0;
}

void SHA1::update(const unsigned char* input, size_t len)
{
    uint32_t i, j;

    j = (m_count[0] >> 3) & 63;
    if ((m_count[0] += static_cast<uint32_t>(len << 3)) < (len << 3))
        m_count[1]++;
    m_count[1] += static_cast<uint32_t>(len >> 29);

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

void SHA1::transform(const unsigned char* buffer)
{
    uint32_t a, b, c, d, e, t, temp;
    uint32_t w[80];

    for (i = 0; i < 16; i++) {
        w[i] = (buffer[i * 4] << 24) |
               (buffer[i * 4 + 1] << 16) |
               (buffer[i * 4 + 2] << 8) |
               (buffer[i * 4 + 3]);
    }
    for (i = 16; i < 80; i++) {
        w[i] = rotl(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
    }

    a = m_state[0];
    b = m_state[1];
    c = m_state[2];
    d = m_state[3];
    e = m_state[4];

    for (i = 0; i < 20; i++) {
        temp = rotl(a, 5) + ((b & c) | ((~b) & d)) + e + w[i] + 0x5A827999;
        e = d; d = c; c = rotl(b, 30); b = a; a = temp;
    }
    for (; i < 40; i++) {
        temp = rotl(a, 5) + (b ^ c ^ d) + e + w[i] + 0x6ED9EBA1;
        e = d; d = c; c = rotl(b, 30); b = a; a = temp;
    }
    for (; i < 60; i++) {
        temp = rotl(a, 5) + ((b & c) | (b & d) | (c & d)) + e + w[i] + 0x8F1BBCDC;
        e = d; d = c; c = rotl(b, 30); b = a; a = temp;
    }
    for (; i < 80; i++) {
        temp = rotl(a, 5) + (b ^ c ^ d) + e + w[i] + 0xCA62C1D6;
        e = d; d = c; c = rotl(b, 30); b = a; a = temp;
    }

    m_state[0] += a;
    m_state[1] += b;
    m_state[2] += c;
    m_state[3] += d;
    m_state[4] += e;
}

void SHA1::final()
{
    int i;
    unsigned char finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (m_count[(i >= 4 ? 1 : 0)] >> ((3 - (i & 3)) * 8)) & 255;
    }

    update(PADDING, 1);
    while ((m_count[0] & 63) != 56)
        update(PADDING, 1);
    update(finalcount, 8);

    for (i = 0; i < 20; i++) {
        m_digest[i] = (m_state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255;
    }

    for (i = 0; i < 40; i += 2) {
        sprintf(m_digestStr + i, "%02x", m_digest[i >> 1]);
    }
}

char* SHA1::hash(const char* data, size_t len)
{
    static SHA1 sha;
    sha.reset();
    sha.update(reinterpret_cast<const unsigned char*>(data), len);
    sha.final();
    return sha.digestString();
}

void calc_sha1(unsigned char hash[20], const void* input, size_t len)
{
    SHA1 sha;
    sha.update(static_cast<const unsigned char*>(input), len);
    sha.final();
    memcpy(hash, sha.digest(), 20);
}