// clipboardFormats.h — Cross-platform clipboard format identifiers
// Ported from Win32 clipboardFormats.h in notepad-plus-plus-translation
// Copyright (C) 2026 Agent Army | GPL v3
//
// Maps Win32 clipboard format constants to MIME type strings for Qt6/Linux.
// These constants are used throughout the codebase for clipboard operations
// so that both Win32 and Qt6/Linux builds use the same identifiers.
//
// Win32 source: PowerEditor/src/clipboardFormats.h
//
// Format map:
//   CF_UNICODETEXT  → "text/plain"                     (via QClipboard)
//   CF_TEXT         → "text/plain;charset=us-ascii"    (via QClipboard)
//   CF_HTML         → "text/html"                      (via QMimeData)
//   CF_RTF          → "text/rtf"                       (via QMimeData)
//   CF_HDROP        → "text/uri-list"                  (via QMimeData)
//   CF_NPPTEXTLEN   → "application/x-npp-binary-length" (Notepad++ internal)

#pragma once

#include <QString>

// ─── Notepad++ internal formats ──────────────────────────────────────────────

// NPPTEXTLEN carries the original byte length of the clipboard content when
// binary data is copied internally by Notepad++.  On Win32 it is registered
// via RegisterClipboardFormat("Notepad++ Binary Length").
// On Linux/Qt6 it is stored as a custom MIME type for round-trip fidelity.
#define CF_NPPTEXTLEN "application/x-npp-text-length"

// ─── Standard format strings (MIME types) ─────────────────────────────────────

// These serve as both identifiers in the Qt code and MIME-type strings for
// QMimeData operations.
#define CF_PLAIN_TEXT     "text/plain"
#define CF_HTML           "text/html"
#define CF_RTF            "text/rtf"
#define CF_URI_LIST       "text/uri-list"
#define CF_IMAGE_PNG      "image/png"
#define CF_IMAGE_BMP      "image/bmp"

// ─── NppTextLen helper namespace ──────────────────────────────────────────────
// Provides compile-time string constants for the NPP internal format.
// Mirrors the Win32 RegisterClipboardFormat() approach with a fixed string.
namespace NppClipboardFormat {
    // Must match the string used by the Win32 build exactly.
    constexpr const char* NppTextLen = "Notepad++ Binary Length";
    // MIME equivalent used on Linux/Qt6 builds.
    constexpr const char* NppTextLenMime = CF_NPPTEXTLEN;
}
