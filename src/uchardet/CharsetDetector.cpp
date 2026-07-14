// CharsetDetector.cpp — uchardet-compatible charset detection with Qt fallback
// Copyright (C) 2026 Agent Army | GPL v3
//
// Semantic lift: Win32/NP++ uchardet integration → Qt6 QStringConverter fallback.
// See CharsetDetector.h for full Win32 → Qt translation notes.
//
// uchardet is a Mozilla-derived encoding detector. If it's found at configure
// time (USE_UCHARDET=ON), we use the real library. Otherwise we use a Qt-based
// heuristic detector that uses BOM detection + byte frequency analysis.

#include "CharsetDetector.h"
#include <QDebug>
#include <QFile>
#include <QByteArray>

// ── Conditional uchardet includes ────────────────────────────────────────────
#if HAVE_UCHARDET
#include <uchardet.h>
#endif

// ── Qt fallback implementation ────────────────────────────────────────────────

// Helper: detect charset from BOM (Byte Order Mark)
static QString detectBomCharset(const char* data, size_t len) {
    if (len < 2) return QString();
    // UTF-8 BOM: EF BB BF
    if (len >= 3 && (unsigned char)data[0] == 0xEF && (unsigned char)data[1] == 0xBB
        && (unsigned char)data[2] == 0xBF) {
        return QStringLiteral("UTF-8");
    }
    // UTF-16 LE BOM: FF FE
    if ((unsigned char)data[0] == 0xFF && (unsigned char)data[1] == 0xFE) {
        // Check for UTF-32 LE BOM: FF FE 00 00
        if (len >= 4 && (unsigned char)data[2] == 0x00 && (unsigned char)data[3] == 0x00)
            return QStringLiteral("UTF-32-LE");
        return QStringLiteral("UTF-16-LE");
    }
    // UTF-16 BE BOM: FE FF
    if ((unsigned char)data[0] == 0xFE && (unsigned char)data[1] == 0xFF)
        return QStringLiteral("UTF-16-BE");
    // UTF-32 BE BOM: 00 00 FE FF
    if (len >= 4 && (unsigned char)data[0] == 0x00 && (unsigned char)data[1] == 0x00
        && (unsigned char)data[2] == 0xFE && (unsigned char)data[3] == 0xFF)
        return QStringLiteral("UTF-32-BE");
    return QString();
}

// Helper: count null bytes (indicates UTF-16 or binary content)
static int countNullChars(const char* data, size_t len) {
    int count = 0;
    for (size_t i = 0; i < len; ++i) {
        if (data[i] == '\0')
            ++count;
    }
    return count;
}

// Helper: check if data looks like valid UTF-8
static bool looksLikeUtf8(const char* data, size_t len) {
    size_t i = 0;
    int invalidSequences = 0;
    while (i < len) {
        unsigned char c = data[i];
        if (c <= 0x7F) {
            // ASCII — valid
            ++i;
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte sequence
            if (i + 1 >= len) return false;
            if ((data[i + 1] & 0xC0) != 0x80) ++invalidSequences;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte sequence
            if (i + 2 >= len) return false;
            if ((data[i + 1] & 0xC0) != 0x80) ++invalidSequences;
            if ((data[i + 2] & 0xC0) != 0x80) ++invalidSequences;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte sequence
            if (i + 3 >= len) return false;
            if ((data[i + 1] & 0xC0) != 0x80) ++invalidSequences;
            if ((data[i + 2] & 0xC0) != 0x80) ++invalidSequences;
            if ((data[i + 3] & 0xC0) != 0x80) ++invalidSequences;
            i += 4;
        } else {
            // Invalid UTF-8 start byte
            ++invalidSequences;
            ++i;
        }
    }
    // Allow up to 1% invalid bytes before declaring it non-UTF-8
    return invalidSequences * 100 < static_cast<int>(len);
}

// Helper: detect single-byte charset via byte frequency analysis
// Win32 → Qt: This replaces uchardet's probabilistic detection
static QString detectSingleByteCharset(const char* data, size_t len) {
    if (len < 10) return QString();

    // Count byte frequencies
    int byteCount[256] = {0};
    int highBytes = 0;        // bytes 0x80-0xFF
    int win1252Range = 0;     // bytes 0x80-0x9F (Windows-specific)
    int nullChars = 0;
    int total = 0;

    for (size_t i = 0; i < len; ++i) {
        unsigned char b = static_cast<unsigned char>(data[i]);
        ++byteCount[b];
        ++total;
        if (b >= 0x80) ++highBytes;
        if (b >= 0x80 && b <= 0x9F) ++win1252Range;
        if (b == 0x00) ++nullChars;
    }

    if (total == 0) return QString();

    // High ratio of NUL bytes → likely binary or UTF-16
    if (nullChars > total / 16)  // more than 6% null bytes
        return QString();

    // No high bytes → ASCII or UTF-8
    if (highBytes == 0)
        return QStringLiteral("ASCII");

    // Check for UTF-8 validity (may contain high bytes but valid UTF-8)
    if (looksLikeUtf8(data, len)) {
        // Valid UTF-8 but with high bytes — could be UTF-8 or a single-byte charset
        // Heuristic: if high-byte ratio is very low and byte distribution looks
        // like natural language, treat as UTF-8
        double highRatio = static_cast<double>(highBytes) / total;
        if (highRatio < 0.01)  // less than 1% high bytes
            return QStringLiteral("UTF-8");
        // For higher ratios, be conservative and use ISO-8859-1 as default
        // (uchardet would make a more informed decision)
        if (highRatio < 0.05)
            return QStringLiteral("UTF-8");
    }

    // Analyze specific byte patterns
    // Windows-1252: has printable characters in 0x80-0x9F range
    // ISO-8859-1: 0x80-0x9F are control characters (C1 control)
    // Windows-1252: 0x80-0x9F map to printable characters (euro, smart quotes, etc.)
    if (win1252Range > 0) {
        // Presence of bytes 0x80-0x9F strongly suggests Windows-1252
        return QStringLiteral("windows-1252");
    }

    // High bytes but no Windows-1252 specific range
    // Check for common ISO-8859-1 / Latin-1 patterns
    if (highBytes > 0) {
        // Check common accented letter patterns
        int accented = byteCount[0xC0] + byteCount[0xC1] + byteCount[0xC2] + byteCount[0xC3]
                     + byteCount[0xC4] + byteCount[0xC5] + byteCount[0xC6] + byteCount[0xC7]
                     + byteCount[0xC8] + byteCount[0xC9] + byteCount[0xCA] + byteCount[0xCB]
                     + byteCount[0xCC] + byteCount[0xCD] + byteCount[0xCE] + byteCount[0xCF]
                     + byteCount[0xD0] + byteCount[0xD1] + byteCount[0xD2] + byteCount[0xD3]
                     + byteCount[0xD4] + byteCount[0xD5] + byteCount[0xD6] + byteCount[0xD7]
                     + byteCount[0xD8] + byteCount[0xD9] + byteCount[0xDA] + byteCount[0xDB]
                     + byteCount[0xDC] + byteCount[0xDD] + byteCount[0xDE] + byteCount[0xDF]
                     + byteCount[0xE0] + byteCount[0xE1] + byteCount[0xE2] + byteCount[0xE3]
                     + byteCount[0xE4] + byteCount[0xE5] + byteCount[0xE6] + byteCount[0xE7]
                     + byteCount[0xE8] + byteCount[0xE9] + byteCount[0xEA] + byteCount[0xEB]
                     + byteCount[0xEC] + byteCount[0xED] + byteCount[0xEE] + byteCount[0xEF]
                     + byteCount[0xF0] + byteCount[0xF1] + byteCount[0xF2] + byteCount[0xF3]
                     + byteCount[0xF4] + byteCount[0xF5] + byteCount[0xF6] + byteCount[0xF7]
                     + byteCount[0xF8] + byteCount[0xF9] + byteCount[0xFA] + byteCount[0xFB]
                     + byteCount[0xFC] + byteCount[0xFD] + byteCount[0xFE] + byteCount[0xFF];

        if (accented > highBytes / 4) {
            // Significant accented letter usage — likely Latin-1 or Windows-1252
            // Default to ISO-8859-1 (most common 8-bit encoding)
            return QStringLiteral("ISO-8859-1");
        }

        // High bytes present but no clear pattern
        // Check for binary content (high ratio of 0x00 and unusual bytes)
        int suspicious = byteCount[0x00] + byteCount[0x01] + byteCount[0x02] + byteCount[0x03]
                      + byteCount[0x04] + byteCount[0x05] + byteCount[0x06] + byteCount[0x0E]
                      + byteCount[0x0F] + byteCount[0x10] + byteCount[0x11] + byteCount[0x12]
                      + byteCount[0x13] + byteCount[0x14] + byteCount[0x15] + byteCount[0x16]
                      + byteCount[0x17] + byteCount[0x18] + byteCount[0x19] + byteCount[0x1A]
                      + byteCount[0x1B] + byteCount[0x1C] + byteCount[0x1D] + byteCount[0x1E]
                      + byteCount[0x1F];

        if (suspicious > highBytes / 2) {
            // Looks like binary data — no meaningful text encoding
            return QStringLiteral("ISO-8859-1");  // Binary-safe default
        }
    }

    // Default fallback — ISO-8859-1 is the most common 8-bit encoding
    return QStringLiteral("ISO-8859-1");
}

// ── CharsetDetector implementation ───────────────────────────────────────────

CharsetDetector::CharsetDetector() {
#if HAVE_UCHARDET
    m_uchardetHandle = uchardet_new();
    m_hasUchardet = (m_uchardetHandle != nullptr);
#else
    m_hasUchardet = false;
    m_uchardetHandle = nullptr;
#endif
}

CharsetDetector::~CharsetDetector() {
#if HAVE_UCHARDET
    if (m_uchardetHandle) {
        uchardet_delete(static_cast<uchardet_t>(m_uchardetHandle));
        m_uchardetHandle = nullptr;
    }
#else
    // Nothing to free for Qt fallback
#endif
}

void CharsetDetector::feed(const char* data, size_t len) {
    m_errorString.clear();

    if (!data || len == 0) return;

#if HAVE_UCHARDET
    if (m_hasUchardet && m_uchardetHandle) {
        int result = uchardet_handle_data(static_cast<uchardet_t>(m_uchardetHandle), data, len);
        if (result != 0) {
            m_errorString = QStringLiteral("uchardet feed error: code %1").arg(result);
            qWarning() << "[CharsetDetector]" << m_errorString;
        }
        return;
    }
#else
    Q_UNUSED(data);
    Q_UNUSED(len);
#endif

    // Qt fallback: accumulate data and analyze
    // We only need to keep a sample for detection (first 64KB is sufficient)
    if (m_accumulatedLen < 65536) {
        size_t toAdd = qMin(len, static_cast<size_t>(65536 - m_accumulatedLen));
        m_accumulatedData.append(data, static_cast<int>(toAdd));
        m_accumulatedLen += toAdd;
    }

    // Track statistics for heuristic detection
    m_totalAnalyzed += static_cast<int>(len);
    for (size_t i = 0; i < len; ++i) {
        unsigned char b = static_cast<unsigned char>(data[i]);
        if (b >= 0x80) ++m_highByteCount;
        if (b >= 0x80 && b <= 0x9F) ++m_win1252Markers;
        if (b == 0x00) ++m_nullCharCount;
    }
}

const char* CharsetDetector::getCharsetName() const {
    // Clear cached result first
    m_detectedCharset.clear();

#if HAVE_UCHARDET
    if (m_hasUchardet && m_uchardetHandle) {
        const char* charset = uchardet_get_charset(static_cast<uchardet_t>(m_uchardetHandle));
        if (charset && charset[0] != '\0') {
            m_detectedCharset = QString::fromLatin1(charset);
            return qPrintable(m_detectedCharset);
        }
    }
#else
    // Qt fallback detection

    // Step 1: Check BOM first
    if (!m_bomChecked) {
        m_bomCharset = detectBomCharset(m_accumulatedData.constData(),
                                          static_cast<size_t>(m_accumulatedData.size()));
        m_bomChecked = true;
        if (!m_bomCharset.isEmpty()) {
            m_detectedCharset = m_bomCharset;
            m_confidence = 1.0;  // BOM detection is certain
            return qPrintable(m_detectedCharset);
        }
    }

    // Step 2: Check for null bytes (UTF-16 or binary)
    if (m_nullCharCount > m_totalAnalyzed / 16) {
        // High null byte ratio — could be UTF-16
        // Check if nulls appear at regular intervals (UTF-16 pattern)
        if (m_accumulatedData.size() >= 4) {
            bool evenNulls = false;
            for (int i = 0; i < m_accumulatedData.size() - 1; i += 2) {
                if (m_accumulatedData[i] == '\0' && m_accumulatedData[i + 1] != '\0') {
                    evenNulls = true;
                    break;
                }
            }
            if (evenNulls && m_accumulatedData.size() >= 2
                && m_accumulatedData[1] != '\0') {
                m_detectedCharset = QStringLiteral("UTF-16-LE");
                m_confidence = 0.9;
                return qPrintable(m_detectedCharset);
            }
        }
    }

    // Step 3: Check if it looks like valid UTF-8
    if (looksLikeUtf8(m_accumulatedData.constData(),
                       static_cast<size_t>(m_accumulatedData.size()))) {
        // Could be UTF-8 or ASCII
        if (m_highByteCount > 0) {
            m_detectedCharset = QStringLiteral("UTF-8");
            m_confidence = 0.8;
        } else {
            m_detectedCharset = QStringLiteral("ASCII");
            m_confidence = 1.0;
        }
        return qPrintable(m_detectedCharset);
    }

    // Step 4: Single-byte charset analysis
    QString singleByte = detectSingleByteCharset(m_accumulatedData.constData(),
                                                  static_cast<size_t>(m_accumulatedData.size()));
    if (!singleByte.isEmpty()) {
        m_detectedCharset = singleByte;
        m_confidence = 0.6;
        return qPrintable(m_detectedCharset);
    }

    // Fallback: treat as ISO-8859-1 (most common 8-bit encoding)
    m_detectedCharset = QStringLiteral("ISO-8859-1");
    m_confidence = 0.5;
    return qPrintable(m_detectedCharset);
#endif

    // No charset detected
    return "";
}

void CharsetDetector::reset() {
#if HAVE_UCHARDET
    if (m_uchardetHandle) {
        uchardet_reset(static_cast<uchardet_t>(m_uchardetHandle));
    }
#else
    // Nothing special needed
#endif

    m_accumulatedData.clear();
    m_accumulatedLen = 0;
    m_detectedCharset.clear();
    m_confidence = 0.0;
    m_errorString.clear();
    m_bomChecked = false;
    m_bomCharset.clear();
    m_highByteCount = 0;
    m_win1252Markers = 0;
    m_nullCharCount = 0;
    m_totalAnalyzed = 0;
}

double CharsetDetector::getConfidence() const {
#if HAVE_UCHARDET
    if (m_hasUchardet && m_uchardetHandle) {
        // uchardet doesn't expose confidence directly
        // We return 1.0 when a charset is detected, 0.0 otherwise
        const char* charset = uchardet_get_charset(static_cast<uchardet_t>(m_uchardetHandle));
        return (charset && charset[0] != '\0') ? 1.0 : 0.0;
    }
#else
    return m_confidence;
#endif
    return 0.0;
}

void CharsetDetector::setConfidenceThreshold(double threshold) {
    m_confidenceThreshold = qBound(0.0, threshold, 1.0);
}

QStringConverter::Encoding CharsetDetector::getEncoding() const {
    const char* charset = getCharsetName();
    if (!charset || charset[0] == '\0')
        return QStringConverter::Utf8;

    QString cs(charset);
    if (cs == u"UTF-8" || cs == u"UTF8")
        return QStringConverter::Utf8;
    if (cs == u"UTF-16" || cs == u"UTF-16-LE" || cs == u"UTF16" || cs == u"UTF16-LE")
        return QStringConverter::Utf16;
    if (cs == u"UTF-16-BE")
        return QStringConverter::Utf16;  // Qt handles endianness via BOM or decoder
    if (cs == u"UTF-32" || cs == u"UTF-32-LE" || cs == u"UTF32" || cs == u"UTF32-LE")
        return QStringConverter::Utf32;
    if (cs == u"UTF-32-BE")
        return QStringConverter::Utf32;  // Qt 6 handles via BOM
    if (cs == u"ASCII" || cs == u"ANSI_X3.4-1968")
        return QStringConverter::Ascii;
    if (cs == u"ISO-8859-1" || cs == u"latin1" || cs == u"LATIN1")
        return QStringConverter::Latin1;

    // Unknown charset — use system default
    return QStringConverter::System;
}
