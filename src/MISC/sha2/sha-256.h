// MISC/sha2/sha-256.h - Qt6 port of SHA-256 implementation
#pragma once

#include <cstdint>

class SHA256
{
public:
    SHA256();
    
    void update(const unsigned char* input, size_t len);
    void final(unsigned char* digest);
    
    static void hash(const unsigned char* input, size_t len, unsigned char* digest);

private:
    void init();
    void transform(const unsigned char* buffer);
    
    uint32_t m_state[8];
    uint32_t m_bitcount[2];
    unsigned char m_buffer[64];
    
    static const uint32_t K[64];
    static const unsigned char PADDING[64];
};