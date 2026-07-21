// Utf8_16.h - UTF-8 / UTF-16 / BOM encoding converter for Notepad--Qt
// Ported from Utf8_16.cxx by Scott Kirkwood / Jens Lorenz (GPL)
// Source: notepad-plus-plus-translation/PowerEditor/src/Utf8_16.h
// INTENT: source provides BOM detection, UTF-8↔UTF-16 conversion with surrogate support.
// Target uses Qt6 QStringConverter and QByteArray for actual I/O; this class
// handles the low-level byte-rewriting logic and encoding detection.

#pragma once

#include "common/Types.h"
#include <cstdint>
#include <cstddef>
#include <memory>

// ============================================================================
// Encoding Mode (matches N++ UniMode)
// ============================================================================
enum class Utf8_16_Mode : int {
    uni8Bit       = 0,  // ANSI / single-byte
    uniUTF8       = 1,  // UTF-8 with BOM
    uni16BE       = 2,  // UTF-16 Big Endian with BOM
    uni16LE       = 3,  // UTF-16 Little Endian with BOM
    uniUTF8_NoBOM = 4,  // UTF-8 without BOM
    uni7Bit       = 5,  // Pure 7-bit ASCII
    uni16BE_NoBOM = 6,  // UTF-16 Big Endian without BOM
    uni16LE_NoBOM = 7,  // UTF-16 Little Endian without BOM
    uniEnd
};

// 7-bit vs 8-bit vs UTF-8 heuristic result
enum class Utf8_7bits_8bits : int {
    utf8NoBOM  = 0,
    ascii7bits = 1,
    ascii8bits = 2
};

// ============================================================================
// Static BOM constants
// ============================================================================
struct Utf8_16_BOM {
    static constexpr uint8_t unknown[3]  = {0x00, 0x00, 0x00};
    static constexpr uint8_t utf8[3]     = {0xEF, 0xBB, 0xBF};
    static constexpr uint8_t utf16BE[3] = {0xFE, 0xFF, 0x00};
    static constexpr uint8_t utf16LE[3] = {0xFF, 0xFE, 0x00};
};

// ============================================================================
// UTF-16 → UTF-8 Iterator
// Reads UTF-16 bytes (LE or BE) and outputs UTF-8 bytes.
// ============================================================================
class Utf16_Iter {
public:
    enum class State { eStart, eSurrogate };

    Utf16_Iter();
    void reset();
    void set(const uint8_t* pBuf, size_t nLen, Utf8_16_Mode encoding);
    bool get(uint8_t* c);

    // Advance to next UTF-16 code unit (not next symbol — allows partial buffers)
    void operator++();

    State state() const { return m_state; }

    // Iteration guard: true while there is more data or pending output
    explicit operator bool() const {
        return (m_pRead + 1 < m_pEnd) || (m_out1st != m_outLst);
    }

private:
    void read();        // Read one UTF-16 code unit from input
    void pushout(uint8_t c);  // Push a byte onto the output ring buffer

    static constexpr int OUT_BUF_SIZE = 16;

    Utf8_16_Mode  m_encoding = Utf8_16_Mode::uni8Bit;
    State          m_state = State::eStart;
    uint8_t        m_out[OUT_BUF_SIZE] = {0};
    int            m_out1st = 0;
    int            m_outLst = 0;
    uint16_t       m_cur16 = 0;
    uint16_t       m_highSurrogate = 0;
    const uint8_t* m_pBuf = nullptr;
    const uint8_t* m_pRead = nullptr;
    const uint8_t* m_pEnd = nullptr;
};

// ============================================================================
// UTF-8 → UTF-16 Iterator
// Reads UTF-8 bytes and outputs UTF-16 code units (LE or BE).
// ============================================================================
class Utf8_Iter {
public:
    Utf8_Iter();
    void reset();
    void set(const uint8_t* pBuf, size_t nLen, Utf8_16_Mode encoding);
    bool get(uint16_t* c);

    bool canGet() const { return m_out1st != m_outLst; }
    void toStart();   // Encode m_code as UTF-16 and push to output

    void operator++();  // Advance to next UTF-8 byte

    // Iteration guard
    explicit operator bool() const {
        return (m_pRead < m_pEnd) || (m_out1st != m_outLst);
    }

private:
    void pushout(uint16_t c);  // Push a UTF-16 code unit onto output ring buffer

    enum class State { eStart, eFollow };

    static constexpr int OUT_BUF_SIZE = 4;

    Utf8_16_Mode  m_encoding = Utf8_16_Mode::uni8Bit;
    State         m_state = State::eStart;
    int           m_code = 0;
    int           m_count = 0;
    uint16_t      m_out[OUT_BUF_SIZE] = {0};
    int           m_out1st = 0;
    int           m_outLst = 0;
    const uint8_t* m_pBuf = nullptr;
    const uint8_t* m_pRead = nullptr;
    const uint8_t* m_pEnd = nullptr;
};

// ============================================================================
// UTF-8/16 Read Converter
// Reads raw bytes and converts them to UTF-8 internally.
// Detects encoding from BOM and statistical analysis.
// ============================================================================
class Utf8_16_Read {
public:
    Utf8_16_Read();
    ~Utf8_16_Read();

    // Main conversion: pass raw bytes, get UTF-8 bytes back.
    // Returns size of output (may be same as input for pass-through).
    size_t convert(char* buf, size_t len);

    // Pointer to the internal converted buffer (valid after convert()).
    const char* getNewBuf() const {
        return reinterpret_cast<const char*>(m_pNewBuf);
    }

    size_t getNewSize() const { return m_newSize; }

    // Current detected encoding
    Utf8_16_Mode encoding() const { return m_encoding; }

    // Detect encoding purely from BOM bytes (no statistical analysis)
    static Utf8_16_Mode detectBOM(const uint8_t* buf, size_t bufLen);

private:
    void determineEncoding();
    Utf8_7bits_8bits detect7bits8bits();

    static constexpr size_t BOM_SKIP_UNKNOWN  = 0;
    static constexpr size_t BOM_SKIP_UTF16     = 2;
    static constexpr size_t BOM_SKIP_UTF8      = 3;

    Utf8_16_Mode  m_encoding = Utf8_16_Mode::uni8Bit;
    uint8_t*      m_pBuf = nullptr;
    uint8_t*      m_pNewBuf = nullptr;
    size_t        m_newSize = 0;
    size_t        m_allocSize = 0;
    size_t        m_skip = 0;
    bool          m_firstRead = true;
    size_t        m_len = 0;
    Utf16_Iter    m_iter16;
};

// ============================================================================
// UTF-8/16 Write Converter
// Converts UTF-8 input to UTF-16 or UTF-8 and writes to file.
// ============================================================================
class Utf8_16_Write {
public:
    Utf8_16_Write();
    ~Utf8_16_Write();

    void setEncoding(Utf8_16_Mode eType);

    // In-memory conversion: UTF-8 in → UTF-16 (or UTF-8 with BOM) out.
    // Caller owns the returned buffer (delete[]).
    size_t convert(const char* p, size_t sz);
    char* getNewBuf() { return reinterpret_cast<char*>(m_pNewBuf); }

    // File-based write (for direct-to-disk saving)
    // Returns true on success.
    bool writeToFile(const QString& filePath, const char* data, size_t size);

private:
    void close();

    Utf8_16_Mode  m_encoding = Utf8_16_Mode::uni8Bit;
    uint8_t*      m_pNewBuf = nullptr;
    size_t        m_bufSize = 0;
    bool          m_firstWrite = true;
};
