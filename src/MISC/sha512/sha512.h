// MISC/sha512/sha512.h - Qt6 port of SHA-512 implementation
#pragma once

#include <cstdint>

class SHA512
{
public:
    SHA512();
    
    void update(const unsigned char* input, size_t len);
    void final(unsigned char* digest);
    
    static void hash(const unsigned char* input, size_t len, unsigned char* digest);

private:
    void init();
    void transform(const unsigned char* buffer);
    
    uint64_t m_state[8];
    uint64_t m_bitcount[2];
    unsigned char m_buffer[128];
    
    static const uint64_t K[80];
    static const unsigned char PADDING[128];
};

// Calculate SHA-512 hash
void calc_sha_512(unsigned char hash[64], const unsigned char* input, size_t len);