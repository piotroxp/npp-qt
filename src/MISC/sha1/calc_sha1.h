// MISC/sha1/calc_sha1.h - Qt6 port of SHA1 calculation
#pragma once

#include <cstddef>
#include <cstdint>

void calc_sha1(unsigned char hash[20], const void* input, size_t len);