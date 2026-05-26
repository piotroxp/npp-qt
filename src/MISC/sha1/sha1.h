// MISC/sha1/sha1.h - Qt6 port of SHA1 implementation
#pragma once

#include <cstdint>

class SHA1
{
public:
    SHA1();
    
    void update(const unsigned char* input, size_t len);
    void final();
    
    unsigned char* digest() { return m_digest; }
    char* digestString() { return m_digestStr; }
    
    // Convenience methods
    static char* hash(const char* data, size_t len);

private:
    void reset();
    void transform(const unsigned char* buffer);
    
    inline uint32_t rotl(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }
    
    uint32_t m_state[5];
    uint32_t m_count[2];
    unsigned char m_buffer[64];
    unsigned char m_digest[20];
    char m_digestStr[41];
    
    static const unsigned char PADDING[64];
};