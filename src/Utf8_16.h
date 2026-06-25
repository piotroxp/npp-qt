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

// unsigned is defined as enum class in Buffer.h; include it to avoid conflicts
// Unicode mode constants (matching N++ original enum values)
static const unsigned uniUnknown   = 0;
static const unsigned uni8Bit       = 1;
static const unsigned uniUTF8       = 2;
static const unsigned uni16BE       = 3;
static const unsigned uni16LE       = 4;
static const unsigned uni16BE_NoBOM = 5;
static const unsigned uni16LE_NoBOM = 6;
static const unsigned uniUTF8_NoBOM = 7;
static const unsigned uni7Bit       = 8;
static const unsigned uniCookie     = 9;   // HTML meta charset
static const unsigned uniEnd        = 10;  // array size sentinel

class Utf8_16 {
public:
    typedef unsigned short utf16; // 16 bits
    typedef unsigned char utf8;    // 8 bits
    typedef unsigned char ubyte;
    static const utf8 k_Boms[uniEnd][3];
};

// Reads UTF-16 and outputs UTF-8
class Utf16_Iter : public Utf8_16 {
public:
    enum eState {
        eStart,
        eSurrogate
    };

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
    unsigned m_eEncoding = uni8Bit;
    eState m_eState = eStart;
    utf8 m_out[16];
    int m_out1st = 0;
    int m_outLst = 0;
    utf16 m_nCur16 = 0;
    utf16 m_highSurrogate = 0;
    const ubyte* m_pBuf = nullptr;
    const ubyte* m_pRead = nullptr;
    const ubyte* m_pEnd = nullptr;
};

// Reads UTF-8 and outputs UTF-16
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
    unsigned m_eEncoding = uni8Bit;
    eState m_eState = eStart;
    int m_code = 0;
    int m_count = 0;
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

    // Main conversion entry point: converts raw bytes to appropriate encoding.
    // Returns the converted QByteArray (ownership transferred).
    // The unsigned encoding is auto-detected from BOM or content.
    QByteArray convert(const char* buf, size_t len);
    QByteArray convert(const QByteArray& data);

    const char* getNewBuf() const { return m_pNewBuf; }
    size_t getNewSize() const { return m_nNewBufSize; }

    unsigned getEncoding() const { return m_eEncoding; }

    // Static BOM detection
    static unsigned determineEncodingFromBOM(const unsigned char* buf, size_t bufLen);

    // Detect UTF-16 without BOM using a heuristic (replaces Win32 IsTextUnicode)
    static bool isLikelyUtf16Le(const unsigned char* buf, size_t len);
    static bool isLikelyUtf16Be(const unsigned char* buf, size_t len);

protected:
    void determineEncoding(const unsigned char* buf, size_t len);

    // Returns: 0 = utf8NoBOM, 1 = ascii7bits, 2 = ascii8bits
    u78 utf8_7bits_8bits();

private:
    unsigned m_eEncoding = uni8Bit;
    const unsigned char* m_pBuf = nullptr;
    char* m_pNewBuf = nullptr;
    size_t m_nNewBufSize = 0;
    size_t m_nAllocatedBufSize = 0;
    size_t m_nSkip = 0;
    bool m_bFirstRead = true;
    size_t m_nLen = 0;
    Utf16_Iter m_Iter16;
};

// Read in a UTF-8 buffer and write out to UTF-16 or UTF-8 (file-backed)
class Utf8_16_Write : public Utf8_16 {
public:
    Utf8_16_Write();
    ~Utf8_16_Write();

    void setEncoding(unsigned eType);

    // Opens the file for writing (wchar_t path → QString path in Qt)
    bool openFile(const QString& fileName);
    bool writeFile(const void* p, size_t _size);
    void closeFile();

    // In-memory conversion
    QByteArray convert(const char* p, size_t _size);
    char* getNewBuf() { return m_pNewBuf; }
    size_t getNewBufSize() const { return m_nBufSize; }

    QString getLastFileError() const { return m_lastFileError; }

protected:
    unsigned m_eEncoding = uni8Bit;
    QFile m_file;              // Qt replacement for Win32_IO_File
    char* m_pNewBuf = nullptr;
    size_t m_nBufSize = 0;
    bool m_bFirstWrite = true;
    QString m_lastFileError;
};
