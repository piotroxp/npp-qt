// Utf8_16.h
// Copyright (C) 2002 Scott Kirkwood
// Permission to use, copy, modify, distribute and sell this code
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies or
// any derived copies.  Scott Kirkwood makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//
// Modified 2006 Jens Lorenz — semantic lift to Qt6 2025
// Changes: Win32 types → Qt types, FILE* → QFile, IsTextUnicode removed
// (replaced with heuristic), std::unique_ptr<Win32_IO_File> → QFile*
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QtGlobal>
#include <QByteArray>
#include <QFile>
#include <QString>

// Unicode mode constants — aligned with NPP UniMode enum values.
// These are used to index k_Boms[] which has exactly 4 entries:
//   0=unknown/ANSI, 1=UTF-8 BOM, 2=UTF-16-BE BOM, 3=UTF-16-LE BOM.
// Nobom variants and extended values (7..9) are also defined but do NOT
// index into k_Boms — callers must guard those accesses.
static const unsigned utf8_16_unknown     = 0;  // uni8Bit
static const unsigned utf8_16_8bit       = 0;  // alias for unknown/ANSI (same value)
static const unsigned utf8_16_utf8        = 1;  // uniUTF8
static const unsigned utf8_16_16be        = 2;  // uni16BE
static const unsigned utf8_16_16le        = 3;  // uni16LE
static const unsigned utf8_16_16be_nobom  = 5;
static const unsigned utf8_16_16le_nobom  = 6;
static const unsigned utf8_16_utf8_nobom  = 7;
static const unsigned utf8_16_7bit        = 8;
static const unsigned utf8_16_cookie      = 9;  // HTML meta charset
static const unsigned utf8_16_end         = 10; // sentinel / array-size of extended table

// ---------------------------------------------------------------------------
// Utf8_16Types — plain struct holding typedefs only.
// Inherited by both Utf16_Iter and Utf8_16 so they share the same types
// without creating a circular inheritance chain.
// ---------------------------------------------------------------------------
struct Utf8_16Types {
    typedef unsigned short utf16;
    typedef unsigned char  utf8;
    typedef unsigned char  ubyte;
};

// ---------------------------------------------------------------------------
// Forward declarations (break circular dependency between Utf16_Iter and Utf8_16)
// ---------------------------------------------------------------------------
class Utf16_Iter;
class Utf8_16;

// ---------------------------------------------------------------------------
// Utf16_Iter — reads UTF-16 bytes and outputs UTF-8 bytes.
// Inherits from Utf8_16Types (not Utf8_16) so that typedefs are available
// immediately — Utf8_16 is not yet complete at this point.
// ---------------------------------------------------------------------------
class Utf16_Iter : public Utf8_16Types {
public:
    enum eState { eStart, eSurrogate };

    Utf16_Iter();
    void reset();
    void set(const ubyte* pBuf, size_t nLen, unsigned eEncoding);
    bool get(utf8* c);
    void operator++();
    eState getState() const { return m_eState; }
    operator bool() const { return (m_pRead + 1 < m_pEnd) || (m_out1st != m_outLst); }

protected:
    void read();
    void pushout(ubyte c);

protected:
    unsigned m_eEncoding = utf8_16_8bit;
    eState m_eState = eStart;
    utf16 m_out[8]; // 8 × 2 bytes = 16 bytes capacity, matching the original utf16[4]
    int m_out1st = 0;
    int m_outLst = 0;
    utf16 m_nCur16 = 0;
    utf16 m_highSurrogate = 0;
    const ubyte* m_pBuf = nullptr;
    const ubyte* m_pRead = nullptr;
    const ubyte* m_pEnd = nullptr;
};

// ---------------------------------------------------------------------------
// Utf8_16 — base class shared by Utf8_Iter, Utf8_16_Read, Utf8_16_Write.
// Holds the BOM table and the shared m_Iter16 pointer.
// ---------------------------------------------------------------------------
class Utf8_16 : public Utf8_16Types {
public:
    // BOM table — inline initializer mirrors NPP's compact 4-element array.
    // NPP indices: 0=ANSI/unknown, 1=UTF-8, 2=UTF-16-BE, 3=UTF-16-LE.
    static const inline utf8 k_Boms[4][3] = {
        {0x00, 0x00, 0x00},  // [0] Unknown / uni8Bit
        {0xEF, 0xBB, 0xBF},  // [1] UTF-8 BOM  / uniUTF8
        {0xFE, 0xFF, 0x00},  // [2] UTF-16 BE  / uni16BE
        {0xFF, 0xFE, 0x00},  // [3] UTF-16 LE  / uni16LE
    };

    // Shared UTF-16 iterator (lazy, owned pointer).
    // Uses ::Utf16_Iter (outer scope) — no nested declaration to avoid shadowing.
    ::Utf16_Iter* m_Iter16 = nullptr;

protected:
    // Note: return type must use ::Utf16_Iter to refer to the outer class,
    // not the (non-existent) nested Utf16_Iter within Utf8_16.
    ::Utf16_Iter& iter16();
    void ensureIter16();
};

// ---------------------------------------------------------------------------
// Utf8_Iter — reads UTF-8 / UTF-16 / raw bytes and outputs UTF-16 code units.
// For UTF-16 input it delegates to the shared m_Iter16 in the base class.
// ---------------------------------------------------------------------------
class Utf8_Iter : public Utf8_16 {
public:
    Utf8_Iter();
    void reset();
    void set(const ubyte* pBuf, size_t nLen, unsigned eEncoding);
    bool get(utf16* c);
    bool canGet() const { return m_out1st != m_outLst; }
    void toStart();
    void operator++();
    operator bool() const { return (m_pRead < m_pEnd) || (m_out1st != m_outLst); }

protected:
    enum eState { eStart, eFollow };
    void pushout(utf16 c);

protected:
    unsigned m_eEncoding = utf8_16_8bit;
    eState m_eState = eStart;
    // m_code needs at least 21 bits for 4-byte UTF-8 sequences; int is only
    // 16 bits on some platforms, so use uint_least32_t to avoid overflow.
    uint_least32_t m_code = 0;
    int m_count = 0;
    // Store the leading byte of a multi-byte sequence so toStart() can encode
    // it correctly (3-byte: shift by 12; 4-byte: shift by 18).
    int m_leadingByte = 0;
    utf16 m_out[4];
    int m_out1st = 0, m_outLst = 0;
    const ubyte* m_pBuf = nullptr;
    const ubyte* m_pRead = nullptr;
    const ubyte* m_pEnd = nullptr;
};

// Read in a UTF-8 buffer and write out to UTF-16 or UTF-8
enum u78 { utf8NoBOM = 0, ascii7bits = 1, ascii8bits = 2 };

class Utf8_16_Read : public Utf8_16 {
public:
    Utf8_16_Read() = default;
    ~Utf8_16_Read();

    QByteArray convert(const char* buf, size_t len);
    QByteArray convert(const QByteArray& data);

    const char* getNewBuf() const { return m_pNewBuf; }
    size_t getNewSize() const { return m_nNewBufSize; }
    unsigned getEncoding() const { return m_eEncoding; }

    static unsigned determineEncodingFromBOM(const unsigned char* buf, size_t bufLen);
    static bool isLikelyUtf16Le(const unsigned char* buf, size_t len);
    static bool isLikelyUtf16Be(const unsigned char* buf, size_t len);

protected:
    void determineEncoding(const unsigned char* buf, size_t len);
    u78 utf8_7bits_8bits();

private:
    unsigned m_eEncoding = utf8_16_8bit;
    const unsigned char* m_pBuf = nullptr;
    char* m_pNewBuf = nullptr;
    size_t m_nNewBufSize = 0;
    size_t m_nAllocatedBufSize = 0;
    size_t m_nSkip = 0;
    bool m_bFirstRead = true;
    size_t m_nLen = 0;
    // Utf8_16_Read needs its own Utf16_Iter (not shared with Utf8_Iter).
    Utf16_Iter m_myIter16;

public:
    // Access the internal UTF-16→UTF-8 converter (call read() + get() after convert()).
    Utf16_Iter& utf16Iter() { return m_myIter16; }
};

class Utf8_16_Write : public Utf8_16 {
public:
    Utf8_16_Write();
    ~Utf8_16_Write();

    void setEncoding(unsigned eType);
    bool openFile(const QString& fileName);
    bool writeFile(const void* p, size_t _size);
    void closeFile();
    QByteArray convert(const char* p, size_t _size);
    char* getNewBuf() { return m_pNewBuf; }
    size_t getNewBufSize() const { return m_nBufSize; }
    QString getLastFileError() const { return m_lastFileError; }

protected:
    unsigned m_eEncoding = utf8_16_8bit;
    QFile m_file;
    char* m_pNewBuf = nullptr;
    size_t m_nBufSize = 0;
    bool m_bFirstWrite = true;
    QString m_lastFileError;
};
