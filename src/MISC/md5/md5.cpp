// MISC/md5/md5.cpp - Qt6 port of MD5 implementation
#include "md5.h"
#include <QFile>
#include <QDataStream>
#include <cstdio>
#include <cstring>

// MD5 initialization constants
const unsigned char MD5::PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// MD5 transformation constants
const int MD5::S11 = 7;
const int MD5::S12 = 12;
const int MD5::S13 = 17;
const int MD5::S14 = 22;
const int MD5::S21 = 5;
const int MD5::S22 = 9;
const int MD5::S23 = 14;
const int MD5::S24 = 20;
const int MD5::S31 = 4;
const int MD5::S32 = 11;
const int MD5::S33 = 16;
const int MD5::S34 = 23;
const int MD5::S41 = 6;
const int MD5::S42 = 10;
const int MD5::S43 = 15;
const int MD5::S44 = 21;

// F, G, H, I are basic MD5 functions
#define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define MD5_G(x, y, z) (((x) & (z) | ((y) & ~z))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | ~z))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

void MD5::Init() {
    context.count[0] = context.count[1] = 0;
    context.state[0] = 0x67452301;
    context.state[1] = 0xefcdab89;
    context.state[2] = 0x98badcfe;
    context.state[3] = 0x10325476;
    memset(digestRaw, 0, 16);
    memset(digestChars, 0, 33);
}

void MD5::MD5Transform(uint32_t state[4], unsigned char block[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];
    Decode(x, block, 64);
    
    #define MD5_FF(a,b,c,d,x,s,ac) { (a) += MD5_F((b),(c),(d)) + x + ac; (a) = ROTATE_LEFT((a),s) + (b); }
    #define MD5_GG(a,b,c,d,x,s,ac) { (a) += MD5_G((b),(c),(d)) + x + ac; (a) = ROTATE_LEFT((a),s) + (b); }
    #define MD5_HH(a,b,c,d,x,s,ac) { (a) += MD5_H((b),(c),(d)) + x + ac; (a) = ROTATE_LEFT((a),s) + (b); }
    #define MD5_II(a,b,c,d,x,s,ac) { (a) += MD5_I((b),(c),(d)) + x + ac; (a) = ROTATE_LEFT((a),s) + (b); }
    
    MD5_FF(a,b,c,d,x[0],S11,0xd76aa478); MD5_FF(d,a,b,c,x[1],S12,0xe8c7b756);
    MD5_FF(c,d,a,b,x[2],S13,0x242070db); MD5_FF(b,c,d,a,x[3],S14,0xc1bdceee);
    MD5_FF(a,b,c,d,x[4],S11,0xf57c0faf); MD5_FF(d,a,b,c,x[5],S12,0x4787c62a);
    MD5_FF(c,d,a,b,x[6],S13,0xa8304613); MD5_FF(b,c,d,a,x[7],S14,0xfd469501);
    MD5_FF(a,b,c,d,x[8],S11,0x698098d8); MD5_FF(d,a,b,c,x[9],S12,0x8b44f7af);
    MD5_FF(c,d,a,b,x[10],S13,0xffff5bb1); MD5_FF(b,c,d,a,x[11],S14,0x895cd7be);
    MD5_FF(a,b,c,d,x[12],S11,0x6b901122); MD5_FF(d,a,b,c,x[13],S12,0xfd987193);
    MD5_FF(c,d,a,b,x[14],S13,0xa679438e); MD5_FF(b,c,d,a,x[15],S14,0x49b40821);
    
    MD5_GG(a,b,c,d,x[1],S21,0xf61e2562); MD5_GG(d,a,b,c,x[6],S22,0xc040b340);
    MD5_GG(c,d,a,b,x[11],S23,0x265e5a51); MD5_GG(b,c,d,a,x[0],S24,0xe9b6c7aa);
    MD5_GG(a,b,c,d,x[5],S21,0xd62f105d); MD5_GG(d,a,b,c,x[10],S22,0x2441453);
    MD5_GG(c,d,a,b,x[15],S23,0xd8a1e681); MD5_GG(b,c,d,a,x[4],S24,0xe7d3fbc8);
    MD5_GG(a,b,c,d,x[9],S21,0x21e1cde6); MD5_GG(d,a,b,c,x[14],S22,0xc33707d6);
    MD5_GG(c,d,a,b,x[3],S23,0xf4d50d87); MD5_GG(b,c,d,a,x[8],S24,0x455a14ed);
    MD5_GG(a,b,c,d,x[13],S21,0xa9e3e905); MD5_GG(d,a,b,c,x[2],S22,0xfcefa3f8);
    MD5_GG(c,d,a,b,x[7],S23,0x676f02d9); MD5_GG(b,c,d,a,x[12],S24,0x8d2a4c8a);
    
    MD5_HH(a,b,c,d,x[5],S31,0xfffa3942); MD5_HH(d,a,b,c,x[8],S32,0x8771f681);
    MD5_HH(c,d,a,b,x[11],S33,0x6d9d6122); MD5_HH(b,c,d,a,x[14],S34,0xfde5380c);
    MD5_HH(a,b,c,d,x[1],S31,0xa4beea44); MD5_HH(d,a,b,c,x[4],S32,0x4bdecfa9);
    MD5_HH(c,d,a,b,x[7],S33,0xf6bb4b60); MD5_HH(b,c,d,a,x[10],S34,0xbebfbc70);
    MD5_HH(a,b,c,d,x[13],S31,0x289b7ec6); MD5_HH(d,a,b,c,x[0],S32,0xeaa127fa);
    MD5_HH(c,d,a,b,x[3],S33,0xd4ef3085); MD5_HH(b,c,d,a,x[6],S34,0x4881d05);
    MD5_HH(a,b,c,d,x[9],S31,0xd9d4d039); MD5_HH(d,a,b,c,x[12],S32,0xe6db99e5);
    MD5_HH(c,d,a,b,x[15],S33,0x1fa27cf8); MD5_HH(b,c,d,a,x[2],S34,0xc4ac5665);
    
    MD5_II(a,b,c,d,x[0],S41,0xf4292244); MD5_II(d,a,b,c,x[7],S42,0x432aff97);
    MD5_II(c,d,a,b,x[14],S43,0xab9423a7); MD5_II(b,c,d,a,x[5],S44,0xfc93a039);
    MD5_II(a,b,c,d,x[12],S41,0x655b59c3); MD5_II(d,a,b,c,x[3],S42,0x8f0ccc92);
    MD5_II(c,d,a,b,x[10],S43,0xffeff47d); MD5_II(b,c,d,a,x[1],S44,0x85845dd1);
    MD5_II(a,b,c,d,x[8],S41,0x6fa87e4f); MD5_II(d,a,b,c,x[15],S42,0xfe2ce6e0);
    MD5_II(c,d,a,b,x[6],S43,0xa3014314); MD5_II(b,c,d,a,x[13],S44,0x4e0811a1);
    MD5_II(a,b,c,d,x[4],S41,0xf7537e82); MD5_II(d,a,b,c,x[11],S42,0xbd3af235);
    MD5_II(c,d,a,b,x[2],S43,0x2ad7d2bb); MD5_II(b,c,d,a,x[9],S44,0xeb86d391);
    
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    memset(x, 0, sizeof(x));
}

void MD5::Update(unsigned char* input, unsigned int inputLen) {
    unsigned int i, index, partLen;
    index = (unsigned int)((context.count[0] >> 3) & 0x3F);
    if ((context.count[0] += ((uint32_t)inputLen << 3) < ((uint32_t)inputLen << 3)))
        context.count[1]++;
    context.count[1] += (inputLen >> 29);
    partLen = 64 - index;
    if (inputLen >= partLen) {
        memcpy((char*)&context.buffer[index], (char*)input, partLen);
        MD5Transform(context.state, context.buffer);
        for (i = partLen; i + 63 < inputLen; i += 64)
            MD5Transform(context.state, &input[i]);
        index = 0;
    } else {
        i = 0;
    }
    memcpy((char*)&context.buffer[index], (char*)&input[i], inputLen - i);
}

void MD5::Final() {
    unsigned char bits[8];
    unsigned int index, padLen;
    Encode(bits, context.count, 8);
    index = (unsigned int)((context.count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    Update((unsigned char*)PADDING, padLen);
    Update(bits, 8);
    Encode(digestRaw, context.state, 16);
    memset((char*)&context, 0, sizeof(context));
    writeToString();
}

void MD5::Encode(unsigned char* output, uint32_t* input, unsigned int len) {
    unsigned int i, j;
    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

void MD5::Decode(uint32_t* output, unsigned char* input, unsigned int len) {
    unsigned int i, j;
    for (i = 0, j = 0; j < len; i++, j += 4)
        output[i] = ((uint32_t)input[j]) | (((uint32_t)input[j + 1] << 8) |
                    (((uint32_t)input[j + 2] << 16) | (((uint32_t)input[j + 3] << 24));
}

void MD5::writeToString() {
    int pos;
    for (pos = 0; pos < 16; pos++)
        sprintf(digestChars + (pos * 2), "%02x", digestRaw[pos]);
    digestChars[32] = '\0';
}

char* MD5::digestFile(const char* filename) {
    Init();
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return nullptr;
    unsigned char buffer[1024];
    qint64 len;
    while ((len = file.read((char*)buffer, 1024)) > 0)
        Update(buffer, static_cast<unsigned int>(len));
    Final();
    file.close();
    return digestChars;
}

char* MD5::digestMemory(unsigned char* memchunk, int len) {
    Init();
    Update(memchunk, len);
    Final();
    return digestChars;
}

char* MD5::digestString(const char* string) {
    Init();
    Update((unsigned char*)string, static_cast<unsigned int>(strlen(string)));
    Final();
    return digestChars;
}
