// Ported to Qt6: clipboard format constants
#pragma once

// MIME/clipboard format constants used by Notepad++
static inline QString clipboardFormatHTML() { return QStringLiteral("text/html"); }
static inline QString clipboardFormatRTF() { return QStringLiteral("text/rtf"); }
static inline QString clipboardFormatNppTextLen() { return QStringLiteral("application/x-notepadpp-textlen"); }
