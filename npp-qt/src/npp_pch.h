// npp_pch.h — Precompiled header for npp-qt
//
// Purpose: Ensure NppSciCompat.h (npp_sci:: constants) is available to ALL
// translation units before any source file body is parsed.
//
// Include order in this file:
//   1. NppSciCompat.h FIRST (defines npp_sci:: constants, includes scintilla headers)
//   2. Qsci headers (uses npp_sci:: constants internally)
//
// Why this order:
//   - ScintillaComponent.h uses npp_sci::SCI_* before including NppSciCompat.h
//   - If NppSciCompat.h comes AFTER ScintillaComponent.h in the PCH, the
//     preprocessor hasn't yet seen the npp_sci namespace → parse failure
//   - By including NppSciCompat.h FIRST, the npp_sci namespace is established
//     before any other header that might reference it
//
// NOTE ON SCINTILLA: NppSciCompat.h includes Scintilla.h and qsciscintillabase.h,
// undefs conflicting Qsci enum macros, and defines npp_sci:: constexpr constants.

#ifndef NPP_PCH_H
#define NPP_PCH_H

// ── 1. NppSciCompat.h FIRST ──
// Defines npp_sci:: namespace with all Scintilla constant values.
#include "NppSciCompat.h"

// ── 2. Qsci headers ──
#include <Qsci/qsciscintillabase.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciprinter.h>

// ── 3. Linux wrappers ───────────────────────────────────────────────────────────
#include <cctype>
#include <cwctype>
#include <strings.h>

inline int wcsicmp(const wchar_t* a, const wchar_t* b)
{
    while (*a && *b) {
        wint_t ua = towupper(*a);
        wint_t ub = towupper(*b);
        if (ua != ub) return static_cast<int>(ua - ub);
        ++a; ++b;
    }
    return static_cast<int>(*a - *b);
}

inline const wchar_t* LangType2Str(int /*langType*/)
{
    return L"";
}

#endif // NPP_PCH_H
