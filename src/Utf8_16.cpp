#include <cstdio>
// Copyright (C) 2002 Scott Kirkwood
//
// Permission to use, copy, modify, distribute and sell this code
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies or
// any derived copies.  Scott Kirkwood makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//
// Modified 2006 Jens Lorenz
// Semantic lift to Qt6 2025 — Win32 file I/O → QFile, IsTextUnicode → heuristic
////////////////////////////////////////////////////////////////////////////////

#include "Utf8_16.h"

// ---------------------------------------------------------------------------
// Utf8_16 base-class members for the shared m_Iter16 pointer.
// ---------------------------------------------------------------------------
::Utf16_Iter& Utf8_16::iter16()
{
    if (!m_Iter16)
        m_Iter16 = new Utf16_Iter();
    return *m_Iter16;
}

void Utf8_16::ensureIter16()
{
    if (!m_Iter16)
        m_Iter16 = new Utf16_Iter();
}

// qbswap — byte-swap a 16-bit value (was Qt-internal, now inlined)
static inline unsigned short qbswap(unsigned short x) {
    return (x << 8) | (x >> 8);
}

// BOM array indexed by enum (utf8_16_*).
// UTF-16 BE/LE are 2-byte BOMs; the 3rd byte is unused (kept for array shape).


// =============================================================================
// Utf16_Iter — reads UTF-16 bytes and emits UTF-8 bytes
// =============================================================================

Utf16_Iter::Utf16_Iter()
{
    reset();
}

void Utf16_Iter::reset()
{
    m_pBuf      = nullptr;
    m_pRead     = nullptr;
    m_pEnd      = nullptr;
    m_eState    = eStart;
    m_out1st    = 0;
    m_outLst    = 0;
    m_nCur16    = 0;
    m_eEncoding = utf8_16_8bit;
}

void Utf16_Iter::set(const ubyte* pBuf, size_t nLen, unsigned eEncoding)
{
    m_pBuf      = pBuf;
    m_pRead     = pBuf;
    m_pEnd      = pBuf + nLen;
    m_eEncoding = eEncoding;
    // m_eState, m_out*, m_nCur16 intentionally NOT reset here
}

bool Utf16_Iter::get(utf8* c)
{
    if (m_out1st != m_outLst)
    {
        *c = m_out[m_out1st];
        m_out1st = (m_out1st + 1) % (sizeof(m_out) / sizeof(m_out[0]));
        return true;
    }
    return false;
}

void Utf16_Iter::pushout(ubyte c)
{
    m_out[m_outLst] = c;
    m_outLst = (m_outLst + 1) % (sizeof(m_out) / sizeof(m_out[0]));
}

void Utf16_Iter::read()
{
    if (m_eEncoding == utf8_16_16le || m_eEncoding == utf8_16_16le_nobom)
    {
        // Little-endian: low byte first.
        m_nCur16 = *m_pRead++;
        m_nCur16 |= static_cast<utf16>(*m_pRead << 8);
        ++m_pRead; // point past the 16-bit word (ready for next read or bounds check)
    }
    else // utf8_16_16be or utf8_16_16be_nobom
    {
        // Big-endian: high byte first.
        m_nCur16 = static_cast<utf16>(*m_pRead << 8);
        m_nCur16 |= *++m_pRead;
        ++m_pRead; // point past the 16-bit word
    }
}

void Utf16_Iter::operator++()
{
    if (m_out1st != m_outLst)
        return;

    switch (m_eState)
    {
        case eStart:
        {
            read();
            if (m_pRead > m_pEnd)
                break;
            if ((m_nCur16 >= 0xd800) && (m_nCur16 < 0xdc00))
            {
                m_eState = eSurrogate;
                m_highSurrogate = m_nCur16;
            }
            else if (m_nCur16 < 0x80)
            {
                pushout(static_cast<ubyte>(m_nCur16));
                m_eState = eStart;
            }
            else if (m_nCur16 < 0x800)
            {
                pushout(static_cast<ubyte>(0xC0 | (m_nCur16 >> 6)));
                pushout(static_cast<ubyte>(0x80 | (m_nCur16 & 0x3f)));
                m_eState = eStart;
            }
            else
            {
                pushout(static_cast<ubyte>(0xE0 | (m_nCur16 >> 12)));
                pushout(static_cast<ubyte>(0x80 | ((m_nCur16 >> 6) & 0x3f)));
                pushout(static_cast<ubyte>(0x80 | (m_nCur16 & 0x3f)));
                m_eState = eStart;
            }
        }
        break;

        case eSurrogate:
        {
            read();
            if (m_pRead > m_pEnd)
            {
                m_eState = eStart;
                break;
            }
            if ((m_nCur16 >= 0xDC00) && (m_nCur16 < 0xE000))
            {
                unsigned int code = 0x10000
                    + ((m_highSurrogate & 0x3ff) << 10)
                    + (m_nCur16 & 0x3ff);
                pushout(static_cast<ubyte>(0xf0 | ((code >> 18) & 0x07)));
                pushout(static_cast<ubyte>(0x80 | ((code >> 12) & 0x3f)));
                pushout(static_cast<ubyte>(0x80 | ((code >> 6) & 0x3f)));
                pushout(static_cast<ubyte>(0x80 | (code & 0x3f)));
            }
            m_eState = eStart;
        }
        break;
    }
}


// =============================================================================
// Utf8_Iter — reads UTF-8 bytes and emits UTF-16 words
// =============================================================================

Utf8_Iter::Utf8_Iter()
{
    reset();
}

void Utf8_Iter::reset()
{
    m_pBuf         = nullptr;
    m_pRead        = nullptr;
    m_pEnd         = nullptr;
    m_eState       = eStart;
    m_out1st       = 0;
    m_outLst       = 0;
    m_eEncoding    = utf8_16_8bit;
    m_code         = 0;
    m_count        = 0;
    m_leadingByte  = 0;
}

void Utf8_Iter::set(const ubyte* pBuf, size_t nLen, unsigned eEncoding)
{
    // Reset ALL iterator state before re-initializing, so stale m_eState /
    // m_code / m_count / m_leadingByte from a prior iteration cannot leak
    // into the next test and cause infinite loops or use-after-end segfaults.
    m_pBuf         = pBuf;
    m_pRead        = pBuf;
    m_pEnd         = pBuf + nLen;
    m_eEncoding    = eEncoding;
    m_eState       = eStart;
    m_out1st       = 0;
    m_outLst       = 0;
    m_code         = 0;
    m_count        = 0;
    m_leadingByte  = 0;
    // Only initialise the shared Utf16_Iter for UTF-16 encodings.
    // UTF-8 input is handled natively byte-by-byte by operator++().
    if (eEncoding == utf8_16_16be || eEncoding == utf8_16_16le ||
        eEncoding == utf8_16_16be_nobom || eEncoding == utf8_16_16le_nobom)
    {
        iter16().set(pBuf, nLen, eEncoding);
    }
}

bool Utf8_Iter::get(utf16* c)
{
    if (m_out1st == m_outLst)
        return false;
    *c = m_out[m_out1st];
    m_out1st = (m_out1st + 1) % (sizeof(m_out) / sizeof(m_out[0]));
    return true;
}

void Utf8_Iter::pushout(utf16 c)
{
    fprintf(stderr, "DBG pushout: c=0x%04X\n", c);
    m_out[m_outLst] = c;
    m_outLst = (m_outLst + 1) % (sizeof(m_out) / sizeof(m_out[0]));
}

void Utf8_Iter::toStart()
{
    // 4-byte UTF-8 sequences are accumulated with an extra left-shift.
    // Correct by shifting right: the accumulated m_code is 6 bits too high.
    if (m_leadingByte)
        m_code >>= 6;

    fprintf(stderr, "DBG toStart: m_code=0x%05X m_leadingByte=%d (high_surr=(m_code>>10)&0x3FF=0x%X, 0xD800|that=0x%X)\n",
            m_code, m_leadingByte, (unsigned)((m_code>>10)&0x3FF), (unsigned)(0xD800|((m_code>>10)&0x3FF)));

    // Utf8_Iter is an in-memory iterator: always output native-endian UTF-16.
    // The file-write path (Utf8_16_Write) handles endianness conversion.
    if (m_code < 0x10000)
    {
        pushout(static_cast<utf16>(m_code));
    }
    else
    {
        utf16 c1 = static_cast<utf16>(0xD800 | (m_code >> 10));
        utf16 c2 = static_cast<utf16>(0xDC00 | (m_code & 0x3ff));
        pushout(c1);
        pushout(c2);
    }
    m_eState = eStart;
    m_count = 0;        // Prevent wrap-around: --m_count from 0 would underflow
    m_leadingByte = 0;  // Safe default; next leading byte sets it appropriately
}

void Utf8_Iter::operator++()
{
    if (m_out1st != m_outLst)
        return;

    // Guard: after the last byte of a UTF-8 sequence is consumed, m_pRead
    // equals m_pEnd but m_eState is still eFollow.  Without this check the
    // next ++ would dereference *m_pEnd (out-of-bounds → garbage, often 0,
    // which prematurely ends the iterator and drops buffered output).
    if (m_pRead >= m_pEnd)
        return;

    switch (m_eState)
    {
        case eStart:
        {
            if (*m_pRead < 0x80)
            {
                m_code = *m_pRead;
                toStart();
            }
            else if (*m_pRead < 0xE0)
            {
                m_code = static_cast<utf16>(0x1f & *m_pRead);
                m_eState = eFollow;
                m_count = 1;
            }
            else if (*m_pRead < 0xF0)
            {
                m_code = static_cast<utf16>(0x0f & *m_pRead);
                m_eState = eFollow;
                m_count = 2;
                m_leadingByte = 0; // 3-byte sequence
            }
            else if (*m_pRead < 0xF5)
            {
                // 4-byte UTF-8: mask top 3 bits of 11110xxx
                m_code = static_cast<utf16>(0x07 & *m_pRead);
                m_eState = eFollow;
                m_count = 3;
                m_leadingByte = 0; // 4-byte sequences don't need toStart correction
            }
            else
            {
                // Invalid or reserved UTF-8 lead byte (> U+10FFFF range).
                // Treat as raw byte.
                m_code = *m_pRead;
                toStart();
            }
        }
        break;

        case eFollow:
        {
            m_code = (m_code << 6) | static_cast<utf8>(0x3F & *m_pRead);
            if (--m_count == 0) {
                // Sequence complete.  2/3-byte: leading byte contributed 5/4 bits,
                // accumulated code is 6 bits too high → right-shift once.
                // 4-byte: leading byte contributed 3 bits, already correct (no shift).
                if (m_leadingByte)
                    m_code >>= 6;
                toStart();
            }
        }
        break;
    }
    ++m_pRead;
}


// =============================================================================
// Utf8_16_Read
// =============================================================================

Utf8_16_Read::~Utf8_16_Read()
{
    if ((m_eEncoding == utf8_16_16be) || (m_eEncoding == utf8_16_16le)
        || (m_eEncoding == utf8_16_16be_nobom) || (m_eEncoding == utf8_16_16le_nobom))
    {
        delete[] m_pNewBuf;
        m_pNewBuf = nullptr;
    }
}

// Returns: 0 = utf8NoBOM, 1 = ascii7bits, 2 = ascii8bits
u78 Utf8_16_Read::utf8_7bits_8bits()
{
    int rv = 1;
    int ASCII7only = 1;
    const utf8* sx = reinterpret_cast<const utf8*>(m_pBuf);
    const utf8* endx = sx + m_nLen;

    while (sx < endx)
    {
        if (*sx == '\0')
        {
            ASCII7only = 0;
            rv = 0;
            break;
        }
        else if ((*sx & 0x80) == 0x0)
        {
            ++sx;
        }
        else if ((*sx & (0x80 + 0x40)) == 0x80)
        {
            ASCII7only = 0;
            rv = 0;
            break;
        }
        else if ((*sx & (0x80 + 0x40 + 0x20)) == (0x80 + 0x40))
        {
            ASCII7only = 0;
            if (static_cast<size_t>(endx - sx) < 2)
            {
                rv = 0;
                break;
            }
            if ((sx[1] & (0x80 + 0x40)) != 0x80)
            {
                rv = 0;
                break;
            }
            sx += 2;
        }
        else if ((*sx & (0x80 + 0x40 + 0x20 + 0x10)) == (0x80 + 0x40 + 0x20))
        {
            ASCII7only = 0;
            if (static_cast<size_t>(endx - sx) < 3)
            {
                rv = 0;
                break;
            }
            if ((sx[1] & (0x80 + 0x40)) != 0x80
                || (sx[2] & (0x80 + 0x40)) != 0x80)
            {
                rv = 0;
                break;
            }
            sx += 3;
        }
        else if ((*sx & (0x80 + 0x40 + 0x20 + 0x10 + 0x8)) == (0x80 + 0x40 + 0x20 + 0x10))
        {
            ASCII7only = 0;
            if (static_cast<size_t>(endx - sx) < 4)
            {
                rv = 0;
                break;
            }
            if ((sx[1] & (0x80 + 0x40)) != 0x80
                || (sx[2] & (0x80 + 0x40)) != 0x80
                || (sx[3] & (0x80 + 0x40)) != 0x80)
            {
                rv = 0;
                break;
            }
            sx += 4;
        }
        else
        {
            ASCII7only = 0;
            rv = 0;
            break;
        }
    }

    if (ASCII7only)
        return ascii7bits;
    if (rv)
        return utf8NoBOM;
    return ascii8bits;
}

void Utf8_16_Read::determineEncoding(const unsigned char* buf, size_t len)
{
    m_eEncoding = utf8_16_8bit;
    m_nSkip = 0;
    m_pBuf = buf;
    m_nLen = len;

    // UTF-16 BE with BOM
    if (len > 1 && buf[0] == k_Boms[utf8_16_16be][0] && buf[1] == k_Boms[utf8_16_16be][1])
    {
        m_eEncoding = utf8_16_16be;
        m_nSkip = 2;
        return;
    }
    // UTF-16 LE with BOM
    if (len > 1 && buf[0] == k_Boms[utf8_16_16le][0] && buf[1] == k_Boms[utf8_16_16le][1])
    {
        m_eEncoding = utf8_16_16le;
        m_nSkip = 2;
        return;
    }
    // UTF-8 with BOM
    if (len > 2 && buf[0] == k_Boms[utf8_16_utf8][0]
        && buf[1] == k_Boms[utf8_16_utf8][1]
        && buf[2] == k_Boms[utf8_16_utf8][2])
    {
        m_eEncoding = utf8_16_utf8;
        m_nSkip = 3;
        return;
    }

    // Heuristic UTF-16 LE without BOM (replaces IsTextUnicode)
    if (isLikelyUtf16Le(buf, len))
    {
        m_eEncoding = utf8_16_16le_nobom;
        m_nSkip = 0;
        return;
    }

    // Content-based detection
    u78 detected = utf8_7bits_8bits();
    if (detected == utf8NoBOM)
        m_eEncoding = utf8_16_utf8_nobom;
    else if (detected == ascii7bits)
        m_eEncoding = utf8_16_7bit;
    else
        m_eEncoding = utf8_16_8bit;
    m_nSkip = 0;
}

unsigned Utf8_16_Read::determineEncodingFromBOM(const unsigned char* buf, size_t bufLen)
{
    if (bufLen > 1 && buf[0] == k_Boms[utf8_16_16be][0] && buf[1] == k_Boms[utf8_16_16be][1])
        return utf8_16_16be;
    if (bufLen > 1 && buf[0] == k_Boms[utf8_16_16le][0] && buf[1] == k_Boms[utf8_16_16le][1])
        return utf8_16_16le;
    if (bufLen > 2 && buf[0] == k_Boms[utf8_16_utf8][0]
        && buf[1] == k_Boms[utf8_16_utf8][1]
        && buf[2] == k_Boms[utf8_16_utf8][2])
        return utf8_16_utf8;
    return utf8_16_8bit;
}

bool Utf8_16_Read::isLikelyUtf16Le(const unsigned char* buf, size_t len)
{
    if (len < 4 || (len % 2) != 0)
        return false;
    // Count zero bytes at even vs odd positions.
    // For UTF-16 LE of Latin/CJK text: high byte goes in odd position (0x00 or 0xNN)
    // so odd positions tend to be null or non-ASCII, even positions tend to be
    // the actual character data (ASCII for Latin, non-zero for CJK).
    // This catches both 0x00NN (Latin) and 0xNN0N (CJK with non-null high bytes).
    size_t zerosEven = 0;
    size_t zerosOdd = 0;
    for (size_t i = 0; i + 1 < len; i += 2)
    {
        if (buf[i] == 0) ++zerosEven;
        if (buf[i + 1] == 0) ++zerosOdd;
    }
    // Also detect CJK-style UTF-16 LE where neither byte is zero:
    // high byte at odd position is 0x80-0xFF (non-ASCII), low byte at even
    // position is the actual character code (ASCII or CJK low byte).
    size_t nonzeroOdd = 0;
    size_t nonzeroEven = 0;
    for (size_t i = 0; i + 1 < len; i += 2)
    {
        if (buf[i] != 0)     ++nonzeroEven;
        if (buf[i + 1] != 0) ++nonzeroOdd;
    }
    // UTF-16 LE: high byte (odd) tends to be non-ASCII; low byte (even) carries data.
    // CJK chars: high byte is non-zero (e.g. 0x4E, 0x65) so odd non-zero > even non-zero.
    // null-based: many null high-bytes → Latin-1/ASCII in UTF-16 LE
    // non-ASCII-based: high-byte (odd) and low-byte (even) both non-ASCII,
    // which is the signature of CJK characters (e.g. 中=U+4E2D → LE=2d 4e,
    // 文=U+6587 → LE=87 65).  Use >= so symmetric non-ASCII also counts.
    bool nullBased     = (zerosOdd   >= zerosEven * 3) && (zerosOdd   >= len / 8);
    bool nonAsciiBased = (nonzeroOdd >= nonzeroEven)   && (nonzeroOdd >= len / 3);
    return nullBased || nonAsciiBased;
}

bool Utf8_16_Read::isLikelyUtf16Be(const unsigned char* buf, size_t len)
{
    if (len < 4 || (len % 2) != 0)
        return false;
    size_t zerosEven = 0;
    size_t zerosOdd = 0;
    for (size_t i = 0; i + 1 < len; i += 2)
    {
        if (buf[i] == 0) ++zerosEven;
        if (buf[i + 1] == 0) ++zerosOdd;
    }
    // For BE: high byte at even position; non-zero at even, zero/null at odd.
    size_t nonzeroEven = 0;
    size_t nonzeroOdd = 0;
    for (size_t i = 0; i + 1 < len; i += 2)
    {
        if (buf[i] != 0)     ++nonzeroEven;
        if (buf[i + 1] != 0) ++nonzeroOdd;
    }
    bool nullBased     = (zerosEven > zerosOdd * 3)   && (zerosEven > len / 8);
    bool nonAsciiBased = (nonzeroEven > nonzeroOdd) && (nonzeroEven > len / 3);
    return nullBased || nonAsciiBased;
}

QByteArray Utf8_16_Read::convert(const char* buf, size_t len)
{
    return convert(QByteArray(buf, static_cast<int>(len)));
}

QByteArray Utf8_16_Read::convert(const QByteArray& data)
{
    const unsigned char* buf = reinterpret_cast<const unsigned char*>(data.constData());
    size_t len = static_cast<size_t>(data.size());

    m_pBuf = buf;
    m_nLen = len;
    m_nNewBufSize = 0;

    if (m_bFirstRead)
    {
        determineEncoding(buf, len);
        m_bFirstRead = false;
    }

    switch (m_eEncoding)
    {
        case utf8_16_7bit:
        case utf8_16_8bit:
        case utf8_16_utf8_nobom:
        {
            m_nAllocatedBufSize = 0;
            m_pNewBuf = const_cast<char*>(data.constData());
            m_nNewBufSize = len;
            return data;
        }

        case utf8_16_utf8:
        {
            size_t skip = (m_eEncoding == utf8_16_utf8) ? 3 : 0;
            m_nAllocatedBufSize = 0;
            m_pNewBuf = const_cast<char*>(data.constData() + skip);
            m_nNewBufSize = len - skip;
            return QByteArray(data.constData() + skip, static_cast<int>(len - skip));
        }

        case utf8_16_16be:
        case utf8_16_16le:
        case utf8_16_16be_nobom:
        case utf8_16_16le_nobom:
        {
            // Convert UTF-16 → UTF-8
            size_t skip = (m_eEncoding == utf8_16_16be || m_eEncoding == utf8_16_16le) ? 2 : 0;
            size_t dataLen = len - skip;

            // Estimate output: worst case 3 UTF-8 bytes per UTF-16 word
            size_t newSize = (dataLen + (dataLen % 2)) + (dataLen + (dataLen % 2)) / 2;
            if (m_nAllocatedBufSize != newSize)
            {
                delete[] m_pNewBuf;
                m_pNewBuf = nullptr;
                m_nAllocatedBufSize = 0;
                m_pNewBuf = new char[newSize];
                if (m_pNewBuf)
                    m_nAllocatedBufSize = newSize;
            }

            if (!m_nAllocatedBufSize)
                return QByteArray();

            unsigned char* pCur = reinterpret_cast<unsigned char*>(m_pNewBuf);
            m_myIter16.set(m_pBuf + skip, dataLen, m_eEncoding);

            while (m_myIter16)
            {
                ++m_myIter16;
                utf8 c;
                while (m_myIter16.get(&c))
                    *pCur++ = c;
            }
            m_nNewBufSize = pCur - reinterpret_cast<unsigned char*>(m_pNewBuf);
            return QByteArray(m_pNewBuf, static_cast<int>(m_nNewBufSize));
        }

        default:
            break;
    }

    return QByteArray();
}


// =============================================================================
// Utf8_16_Write
// =============================================================================

Utf8_16_Write::Utf8_16_Write()
{
    m_eEncoding = utf8_16_8bit;
    m_pNewBuf = nullptr;
    m_bFirstWrite = true;
    m_nBufSize = 0;
}

Utf8_16_Write::~Utf8_16_Write()
{
    closeFile();
}

bool Utf8_16_Write::openFile(const QString& fileName)
{
    m_lastFileError.clear();
    m_file.setFileName(fileName);
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        m_lastFileError = m_file.errorString();
        return false;
    }
    m_bFirstWrite = true;
    return true;
}

bool Utf8_16_Write::writeFile(const void* p, size_t _size)
{
    if (!m_file.isOpen())
        return false;

    if (m_bFirstWrite)
    {
        switch (m_eEncoding)
        {
            case utf8_16_utf8:
            {
                if (m_file.write(reinterpret_cast<const char*>(k_Boms[m_eEncoding]), 3) != 3)
                    return false;
            }
            break;

            case utf8_16_16be:
            case utf8_16_16le:
            {
                if (m_file.write(reinterpret_cast<const char*>(k_Boms[m_eEncoding]), 2) != 2)
                    return false;
            }
            break;

            default:
                break;
        }
        m_bFirstWrite = false;
    }

    qint64 written = m_file.write(static_cast<const char*>(p), static_cast<qint64>(_size));
    return (written == static_cast<qint64>(_size));
}

void Utf8_16_Write::setEncoding(unsigned eType)
{
    m_eEncoding = eType;
}

void Utf8_16_Write::closeFile()
{
    delete[] m_pNewBuf;
    m_pNewBuf = nullptr;
    m_nBufSize = 0;

    if (m_file.isOpen())
    {
        m_file.close();
        if (m_file.error() != QFileDevice::NoError)
            m_lastFileError = m_file.errorString();
    }
}

QByteArray Utf8_16_Write::convert(const char* p, size_t _size)
{
    delete[] m_pNewBuf;
    m_pNewBuf = nullptr;
    m_nBufSize = 0;

    try
    {
        switch (m_eEncoding)
        {
            case utf8_16_7bit:
            case utf8_16_8bit:
            case utf8_16_utf8_nobom:
            {
                m_pNewBuf = new char[_size];
                m_nBufSize = _size;
                memcpy(m_pNewBuf, p, _size);
            }
            break;

            case utf8_16_utf8:
            {
                m_pNewBuf = new char[_size + 3];
                m_nBufSize = _size + 3;
                memcpy(m_pNewBuf, k_Boms[m_eEncoding], 3);
                memcpy(m_pNewBuf + 3, p, _size);
            }
            break;

            case utf8_16_16be_nobom:
            case utf8_16_16le_nobom:
            case utf8_16_16be:
            case utf8_16_16le:
            {
                utf16* pCur = nullptr;
                size_t allocSize = 0;

                if (m_eEncoding == utf8_16_16be || m_eEncoding == utf8_16_16le)
                {
                    allocSize = sizeof(utf16) * (_size + 1);
                    m_pNewBuf = new char[allocSize];
                    memcpy(m_pNewBuf, k_Boms[m_eEncoding], 2);
                    pCur = reinterpret_cast<utf16*>(m_pNewBuf + 2);
                }
                else
                {
                    allocSize = sizeof(utf16) * _size;
                    m_pNewBuf = new char[allocSize];
                    pCur = reinterpret_cast<utf16*>(m_pNewBuf);
                }

                Utf8_Iter iter8;
                iter8.set(reinterpret_cast<const ubyte*>(p), _size, m_eEncoding);

                for (; iter8; ++iter8)
                {
                    if (iter8.canGet())
                    {
                        iter8.get(pCur++);
                    }
                }
                m_nBufSize = reinterpret_cast<char*>(pCur) - m_pNewBuf;
            }
            break;

            default:
                break;
        }
    }
    catch (const std::bad_alloc&)
    {
        m_nBufSize = 0;
    }

    return QByteArray(m_pNewBuf, static_cast<int>(m_nBufSize));
}

