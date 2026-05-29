#pragma once
// Stub for MSVC <process.h> on non-Windows builds.
#include <cstdint>

inline uintptr_t _beginthreadex(void*, unsigned, unsigned (*)(void*), void*, unsigned, unsigned*) {
	return 0;
}
