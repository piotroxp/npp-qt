#pragma once
#include "WindowsCompat.h"
#include <cwchar>

#ifndef STRSAFE_E_INSUFFICIENT_BUFFER
#define STRSAFE_E_INSUFFICIENT_BUFFER ((HRESULT)0x8007007A)
#endif

inline HRESULT StringCchCopyW(wchar_t* dst, size_t cch, const wchar_t* src) {
	if (!dst || cch == 0) return STRSAFE_E_INSUFFICIENT_BUFFER;
	if (!src) { dst[0] = L'\0'; return STRSAFE_E_INSUFFICIENT_BUFFER; }
	wcsncpy(dst, src, cch - 1);
	dst[cch - 1] = L'\0';
	return S_OK;
}
#define StringCchCopy StringCchCopyW
