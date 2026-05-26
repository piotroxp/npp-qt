// MISC/md5/md5.cpp - Qt6 port of MD5 implementation
#include "md5.h"
#include <QFile>
#include <QDataStream>

void MD5::Update(unsigned char* input, unsigned int inputLen)
{
    unsigned int i, index, partLen;

    index = (unsigned int)((context.count[0] >> 3) & 0x3F);
    if ((context.count[0] += ((uint32_t)inputLen << 3)) < ((uint32_t)inputLen << 3))
        context.count[1]++;
    context.count[1] += ((uint32_t)inputLen >> 29);

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

void MD5::Final()
{
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

void MD5::Encode(unsigned char* output, uint32_t* input, unsigned int len)
{
    unsigned int i, j;
    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

void MD5::Decode(uint32_t* output, unsigned char* input, unsigned int len)
{
    unsigned int i, j;
    for (i = 0, j = 0; j < len; i++, j += 4)
        output[i] = ((uint32_t)input[j]) | (((uint32_t)input[j + 1]) << 8) |
                    (((uint32_t)input[j + 2]) << 16) | (((uint32_t)input[j + 3]) << 24);
}

void MD5::writeToString()
{
    int pos;
    for (pos = 0; pos < 16; pos++)
        sprintf(digestChars + (pos * 2), "%02x", digestRaw[pos]);
}

char* MD5::digestFile(const char* filename)
{
    Init();
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return nullptr;
    
    unsigned char buffer[1024];
    qint64 len;
    while ((len = file.read((char*)buffer, 1024)) > 0)
        Update(buffer, static_cast<unsigned int>(len));
    Final();
    file.close();
    return digestChars;
}

char* MD5::digestMemory(unsigned char* memchunk, int len)
{
    Init();
    Update(memchunk, len);
    Final();
    return digestChars;
}

char* MD5::digestString(const char* string)
{
    Init();
    Update((unsigned char*)string, static_cast<unsigned int>(strlen(string)));
    Final();
    return digestChars;
}