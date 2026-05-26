// MISC/md5/md5.h - Qt6 port of MD5 implementation
#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>

class MD5
{
public:
    MD5() { Init(); }

    void Init();
    void Update(unsigned char* input, unsigned int inputLen);
    void Final();

    // Buffer must be 33 chars (32 hex + null terminator)
    char* digestFile(const char* filename);
    char* digestMemory(unsigned char* memchunk, int len);
    char* digestString(const char* string);

    unsigned char digestRaw[16];
    char digestChars[33];

private:
    struct __context_t {
        uint32_t state[4];
        uint32_t count[2];
        unsigned char buffer[64];
    } context;

    void MD5Transform(uint32_t state[4], unsigned char block[64]);
    static void Encode(unsigned char* output, uint32_t* input, unsigned int len);
    static void Decode(uint32_t* output, unsigned char* input, unsigned int len);
    void writeToString();

    static const unsigned char PADDING[64];
    static const int S11, S12, S13, S14, S21, S22, S23, S24, S31, S32, S33, S34, S41, S42, S43, S44;
};

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s, ac) { \
    (a) += F((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
    (a) += I((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}