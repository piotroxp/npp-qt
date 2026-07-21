// Utf8_16.cpp - UTF-8 / UTF-16 / BOM encoding converter for Notepad--Qt
// Ported from Utf8_16.cxx by Scott Kirkwood / Jens Lorenz (GPL)
// Source: notepad-plus-plus-translation/PowerEditor/src/Utf8_16.cpp
// INTENT: source provides BOM detection, UTF-8↔UTF-16 conversion with surrogate support.
// Target uses Qt6 for file I/O; this class handles the low-level byte-rewriting logic.

#include "Utf8_16.h"
#include "common/Types.h"
#include <QFile>
#include <QByteArray>
#include <algorithm>
#include <cstring>
#include <cassert>

// Portable byte-swap for uint16_t (MSVC _byteswap_ushort fallback)
static inline uint16_t byte_swap_u16(uint16_t v) {
    return static_cast<uint16_t>(((v & 0x00FFu) << 8) | ((v & 0xFF00u) >> 8));
}

// ============================================================================
// Utf16_Iter
// ============================================================================
Utf16_Iter::Utf16_Iter() { reset(); }

void Utf16_Iter::reset() {
    m_pBuf = nullptr;
    m_pRead = nullptr;
    m_pEnd = nullptr;
    m_state = State::eStart;
    m_out1st = 0;
    m_outLst = 0;
    m_cur16 = 0;
    m_highSurrogate = 0;
    m_encoding = Utf8_16_Mode::uni8Bit;
}

void Utf16_Iter::set(const uint8_t* pBuf, size_t nLen, Utf8_16_Mode encoding) {
    m_pBuf = pBuf;
    m_pRead = pBuf;
    m_pEnd = pBuf + nLen;
    m_encoding = encoding;
    // NOTE: m_state, m_out*, m_cur16 are NOT reset here (allows partial buffer continuation)
}

bool Utf16_Iter::get(uint8_t* c) {
    if (m_out1st != m_outLst) {
        *c = m_out[m_out1st];
        m_out1st = (m_out1st + 1) % OUT_BUF_SIZE;
        return true;
    }
    return false;
}

void Utf16_Iter::pushout(uint8_t c) {
    m_out[m_outLst] = c;
    m_outLst = (m_outLst + 1) % OUT_BUF_SIZE;
}

void Utf16_Iter::read() {
    if (m_encoding == Utf8_16_Mode::uni16LE || m_encoding == Utf8_16_Mode::uni16LE_NoBOM) {
        // Little-endian: low byte first
        m_cur16 = *m_pRead++;
        m_cur16 |= static_cast<uint16_t>(*m_pRead << 8);
    } else {
        // Big-endian: high byte first
        m_cur16 = static_cast<uint16_t>(*m_pRead++ << 8);
        m_cur16 |= *m_pRead;
    }
    ++m_pRead;
}

void Utf16_Iter::operator++() {
    if (m_out1st != m_outLst) return;

    switch (m_state) {
        case State::eStart: {
            read();
            if (m_cur16 >= 0xd800 && m_cur16 < 0xdc00) {
                // High surrogate — wait for low surrogate
                m_state = State::eSurrogate;
                m_highSurrogate = m_cur16;
            } else if (m_cur16 < 0x80) {
                // ASCII
                pushout(static_cast<uint8_t>(m_cur16));
                m_state = State::eStart;
            } else if (m_cur16 < 0x800) {
                // 2-byte UTF-8
                pushout(static_cast<uint8_t>(0xC0 | (m_cur16 >> 6)));
                pushout(static_cast<uint8_t>(0x80 | (m_cur16 & 0x3f)));
                m_state = State::eStart;
            } else {
                // 3-byte UTF-8
                pushout(static_cast<uint8_t>(0xE0 | (m_cur16 >> 12)));
                pushout(static_cast<uint8_t>(0x80 | ((m_cur16 >> 6) & 0x3f)));
                pushout(static_cast<uint8_t>(0x80 | (m_cur16 & 0x3f)));
                m_state = State::eStart;
            }
            break;
        }
        case State::eSurrogate: {
            read();
            if (m_cur16 >= 0xDC00 && m_cur16 < 0xE000) {
                // Valid low surrogate — combine into 4-byte UTF-8
                uint32_t code = 0x10000 + ((m_highSurrogate & 0x3ff) << 10) + (m_cur16 & 0x3ff);
                pushout(static_cast<uint8_t>(0xf0 | ((code >> 18) & 0x07)));
                pushout(static_cast<uint8_t>(0x80 | ((code >> 12) & 0x3f)));
                pushout(static_cast<uint8_t>(0x80 | ((code >> 6)  & 0x3f)));
                pushout(static_cast<uint8_t>(0x80 | (code & 0x3f)));
            }
            m_state = State::eStart;
            break;
        }
    }
}

// ============================================================================
// Utf8_Iter
// ============================================================================
Utf8_Iter::Utf8_Iter() { reset(); }

void Utf8_Iter::reset() {
    m_pBuf = nullptr;
    m_pRead = nullptr;
    m_pEnd = nullptr;
    m_state = State::eStart;
    m_out1st = 0;
    m_outLst = 0;
    m_encoding = Utf8_16_Mode::uni8Bit;
}

void Utf8_Iter::set(const uint8_t* pBuf, size_t nLen, Utf8_16_Mode encoding) {
    m_pBuf = pBuf;
    m_pRead = pBuf;
    m_pEnd = pBuf + nLen;
    m_encoding = encoding;
    // NOTE: m_state, m_code not reset
}

bool Utf8_Iter::get(uint16_t* c) {
    if (m_out1st == m_outLst) return false;
    *c = m_out[m_out1st];
    m_out1st = (m_out1st + 1) % OUT_BUF_SIZE;
    return true;
}

void Utf8_Iter::pushout(uint16_t c) {
    m_out[m_outLst] = c;
    m_outLst = (m_outLst + 1) % OUT_BUF_SIZE;
}

void Utf8_Iter::toStart() {
    bool swap = (m_encoding == Utf8_16_Mode::uni16BE || m_encoding == Utf8_16_Mode::uni16BE_NoBOM);

    if (m_code < 0x10000) {
        uint16_t c = swap ? byte_swap_u16(static_cast<uint16_t>(m_code))
                          : static_cast<uint16_t>(m_code);
        pushout(c);
    } else {
        // Encode as surrogate pair
        m_code -= 0x10000;
        uint16_t c1 = static_cast<uint16_t>(0xD800 | (m_code >> 10));
        uint16_t c2 = static_cast<uint16_t>(0xDC00 | (m_code & 0x3ff));
        if (swap) {
            c1 = byte_swap_u16(c1);
            c2 = byte_swap_u16(c2);
        }
        pushout(c1);
        pushout(c2);
    }
    m_state = State::eStart;
}

void Utf8_Iter::operator++() {
    if (m_out1st != m_outLst) return;

    switch (m_state) {
        case State::eStart: {
            if (*m_pRead < 0x80) {
                // ASCII
                m_code = *m_pRead;
                toStart();
            } else if (*m_pRead < 0xE0) {
                // 2-byte sequence
                m_code = static_cast<int>(0x1f & *m_pRead);
                m_state = State::eFollow;
                m_count = 1;
            } else if (*m_pRead < 0xF0) {
                // 3-byte sequence
                m_code = static_cast<int>(0x0f & *m_pRead);
                m_state = State::eFollow;
                m_count = 2;
            } else {
                // 4-byte sequence
                m_code = static_cast<int>(0x07 & *m_pRead);
                m_state = State::eFollow;
                m_count = 3;
            }
            break;
        }
        case State::eFollow: {
            m_code = (m_code << 6) | static_cast<int>(0x3F & *m_pRead);
            --m_count;
            if (m_count == 0) {
                toStart();
            }
            break;
        }
    }
    ++m_pRead;
}

// ============================================================================
// Utf8_16_Read
// ============================================================================
Utf8_16_Read::Utf8_16_Read() = default;

Utf8_16_Read::~Utf8_16_Read() {
    if (m_pNewBuf && m_pNewBuf != m_pBuf) {
        delete[] m_pNewBuf;
        m_pNewBuf = nullptr;
    }
}

size_t Utf8_16_Read::convert(char* buf, size_t len) {
    static size_t s_skip = 0;  // accumulated skip across multiple calls

    m_pBuf = reinterpret_cast<uint8_t*>(buf);
    m_len = len;
    m_newSize = 0;

    if (m_firstRead) {
        determineEncoding();
        s_skip = m_skip;
        m_firstRead = false;
    }

    switch (m_encoding) {
        case Utf8_16_Mode::uni7Bit:
        case Utf8_16_Mode::uni8Bit:
        case Utf8_16_Mode::uniUTF8_NoBOM:
            // Pass-through
            m_allocSize = 0;
            m_pNewBuf = m_pBuf;
            m_newSize = len;
            break;

        case Utf8_16_Mode::uniUTF8:
            // Pass-through after BOM skip
            m_allocSize = 0;
            m_pNewBuf = m_pBuf + s_skip;
            m_newSize = len - s_skip;
            break;

        case Utf8_16_Mode::uni16BE:
        case Utf8_16_Mode::uni16LE:
        case Utf8_16_Mode::uni16BE_NoBOM:
        case Utf8_16_Mode::uni16LE_NoBOM: {
            // Convert UTF-16 → UTF-8
            size_t rawLen = len - s_skip;
            // Estimate output: UTF-16 can be up to 3x the byte size in UTF-8
            size_t estSize = rawLen * 3;
            if (m_allocSize != estSize) {
                if (m_pNewBuf && m_pNewBuf != m_pBuf) {
                    delete[] m_pNewBuf;
                }
                m_pNewBuf = nullptr;
                m_allocSize = 0;
                m_pNewBuf = new uint8_t[estSize];
                if (m_pNewBuf) {
                    m_allocSize = estSize;
                }
            }

            if (m_pNewBuf) {
                uint8_t* pCur = m_pNewBuf;
                m_iter16.set(m_pBuf + s_skip, rawLen, m_encoding);

                while (m_iter16) {
                    ++m_iter16;
                    uint8_t c;
                    while (m_iter16.get(&c)) {
                        *pCur++ = c;
                    }
                }
                m_newSize = static_cast<size_t>(pCur - m_pNewBuf);
            }
            break;
        }

        default:
            break;
    }

    s_skip = 0;
    return m_newSize;
}

void Utf8_16_Read::determineEncoding() {
    m_encoding = Utf8_16_Mode::uni8Bit;
    m_skip = 0;

    // BOM detection
    if (m_len > 1 && m_pBuf[0] == Utf8_16_BOM::utf16BE[0] && m_pBuf[1] == Utf8_16_BOM::utf16BE[1]) {
        m_encoding = Utf8_16_Mode::uni16BE;
        m_skip = BOM_SKIP_UTF16;
    } else if (m_len > 1 && m_pBuf[0] == Utf8_16_BOM::utf16LE[0] && m_pBuf[1] == Utf8_16_BOM::utf16LE[1]) {
        m_encoding = Utf8_16_Mode::uni16LE;
        m_skip = BOM_SKIP_UTF16;
    } else if (m_len > 2 && m_pBuf[0] == Utf8_16_BOM::utf8[0] &&
               m_pBuf[1] == Utf8_16_BOM::utf8[1] && m_pBuf[2] == Utf8_16_BOM::utf8[2]) {
        m_encoding = Utf8_16_Mode::uniUTF8;
        m_skip = BOM_SKIP_UTF8;
    } else if (m_len > 1 && m_len % 2 == 0 && m_pBuf[0] != 0 && m_pBuf[1] == 0) {
        // Heuristic: null byte followed by non-null → likely UTF-16 LE without BOM
        // Count null/non-null alternating pattern as evidence
        size_t nullPairs = 0;
        for (size_t i = 0; i + 1 < m_len; i += 2) {
            if (m_pBuf[i] == 0 && m_pBuf[i + 1] != 0) {
                nullPairs++;
            }
        }
        if (nullPairs > m_len / 8) {
            m_encoding = Utf8_16_Mode::uni16LE_NoBOM;
            m_skip = 0;
        } else {
            // Fall back to 7/8-bit detection
            Utf8_7bits_8bits r = detect7bits8bits();
            if (r == Utf8_7bits_8bits::utf8NoBOM) {
                m_encoding = Utf8_16_Mode::uniUTF8_NoBOM;
            } else if (r == Utf8_7bits_8bits::ascii7bits) {
                m_encoding = Utf8_16_Mode::uni7Bit;
            } else {
                m_encoding = Utf8_16_Mode::uni8Bit;
            }
            m_skip = 0;
        }
    } else {
        // 7/8-bit heuristic
        Utf8_7bits_8bits r = detect7bits8bits();
        if (r == Utf8_7bits_8bits::utf8NoBOM) {
            m_encoding = Utf8_16_Mode::uniUTF8_NoBOM;
        } else if (r == Utf8_7bits_8bits::ascii7bits) {
            m_encoding = Utf8_16_Mode::uni7Bit;
        } else {
            m_encoding = Utf8_16_Mode::uni8Bit;
        }
        m_skip = 0;
    }
}

Utf8_7bits_8bits Utf8_16_Read::detect7bits8bits() {
    bool asciiOnly = true;
    bool isUtf8 = true;

    const uint8_t* sx = m_pBuf;
    const uint8_t* endx = sx + m_len;

    while (sx < endx) {
        if (*sx == 0) {
            // NUL byte → not pure ASCII/UTF-8 (binary file indicator)
            asciiOnly = false;
            isUtf8 = false;
            break;
        } else if ((*sx & 0x80) == 0x00) {
            // ASCII byte
            ++sx;
        } else if ((*sx & 0xC0) == 0x80) {
            // Continuation byte without a start → not UTF-8
            asciiOnly = false;
            isUtf8 = false;
            break;
        } else if ((*sx & 0xE0) == 0xC0) {
            // 2-byte sequence start (110xxxxx)
            asciiOnly = false;
            if (sx + 1 >= endx || (sx[1] & 0xC0) != 0x80) {
                isUtf8 = false;
                break;
            }
            sx += 2;
        } else if ((*sx & 0xF0) == 0xE0) {
            // 3-byte sequence start (1110xxxx)
            asciiOnly = false;
            if (sx + 2 >= endx || (sx[1] & 0xC0) != 0x80 || (sx[2] & 0xC0) != 0x80) {
                isUtf8 = false;
                break;
            }
            sx += 3;
        } else if ((*sx & 0xF8) == 0xF0) {
            // 4-byte sequence start (11110xxx)
            asciiOnly = false;
            if (sx + 3 >= endx || (sx[1] & 0xC0) != 0x80 ||
                (sx[2] & 0xC0) != 0x80 || (sx[3] & 0xC0) != 0x80) {
                isUtf8 = false;
                break;
            }
            sx += 4;
        } else {
            asciiOnly = false;
            isUtf8 = false;
            break;
        }
    }

    if (asciiOnly) return Utf8_7bits_8bits::ascii7bits;
    if (isUtf8)    return Utf8_7bits_8bits::utf8NoBOM;
    return Utf8_7bits_8bits::ascii8bits;
}

Utf8_16_Mode Utf8_16_Read::detectBOM(const uint8_t* buf, size_t bufLen) {
    if (bufLen > 1 && buf[0] == Utf8_16_BOM::utf16BE[0] && buf[1] == Utf8_16_BOM::utf16BE[1]) {
        return Utf8_16_Mode::uni16BE;
    }
    if (bufLen > 1 && buf[0] == Utf8_16_BOM::utf16LE[0] && buf[1] == Utf8_16_BOM::utf16LE[1]) {
        return Utf8_16_Mode::uni16LE;
    }
    if (bufLen > 2 && buf[0] == Utf8_16_BOM::utf8[0] &&
        buf[1] == Utf8_16_BOM::utf8[1] && buf[2] == Utf8_16_BOM::utf8[2]) {
        return Utf8_16_Mode::uniUTF8;
    }
    return Utf8_16_Mode::uni8Bit;
}

// ============================================================================
// Utf8_16_Write
// ============================================================================
Utf8_16_Write::Utf8_16_Write() {
    m_encoding = Utf8_16_Mode::uni8Bit;
    m_pNewBuf = nullptr;
    m_bufSize = 0;
    m_firstWrite = true;
}

Utf8_16_Write::~Utf8_16_Write() {
    close();
}

void Utf8_16_Write::setEncoding(Utf8_16_Mode eType) {
    m_encoding = eType;
}

size_t Utf8_16_Write::convert(const char* p, size_t sz) {
    if (m_pNewBuf) {
        delete[] m_pNewBuf;
        m_pNewBuf = nullptr;
        m_bufSize = 0;
    }

    switch (m_encoding) {
        case Utf8_16_Mode::uni7Bit:
        case Utf8_16_Mode::uni8Bit:
        case Utf8_16_Mode::uniUTF8_NoBOM: {
            // Pass-through
            m_pNewBuf = new uint8_t[sz];
            m_bufSize = sz;
            std::memcpy(m_pNewBuf, p, sz);
            break;
        }
        case Utf8_16_Mode::uniUTF8: {
            // UTF-8 with BOM prefix
            m_pNewBuf = new uint8_t[sz + 3];
            m_bufSize = sz + 3;
            std::memcpy(m_pNewBuf, Utf8_16_BOM::utf8, 3);
            std::memcpy(m_pNewBuf + 3, p, sz);
            break;
        }
        case Utf8_16_Mode::uni16BE:
        case Utf8_16_Mode::uni16LE:
        case Utf8_16_Mode::uni16BE_NoBOM:
        case Utf8_16_Mode::uni16LE_NoBOM: {
            // Convert UTF-8 → UTF-16
            size_t bomSize = (m_encoding == Utf8_16_Mode::uni16BE || m_encoding == Utf8_16_Mode::uni16LE) ? 2 : 0;
            size_t outSize = bomSize + sz * 2;  // UTF-8 max 1 byte/char; UTF-16 min 2 bytes/char

            m_pNewBuf = new uint8_t[outSize];
            m_bufSize = outSize;

            if (bomSize > 0) {
                // Write BOM
                if (m_encoding == Utf8_16_Mode::uni16BE) {
                    m_pNewBuf[0] = Utf8_16_BOM::utf16BE[0];
                    m_pNewBuf[1] = Utf8_16_BOM::utf16BE[1];
                } else {
                    m_pNewBuf[0] = Utf8_16_BOM::utf16LE[0];
                    m_pNewBuf[1] = Utf8_16_BOM::utf16LE[1];
                }
            }

            const uint8_t* pIn = reinterpret_cast<const uint8_t*>(p);
            uint16_t* pOut = reinterpret_cast<uint16_t*>(m_pNewBuf + bomSize);
            size_t outIndex = 0;

            Utf8_Iter iter8;
            iter8.set(pIn, sz, m_encoding);

            for (; iter8; ++iter8) {
                if (iter8.canGet()) {
                    iter8.get(&pOut[outIndex++]);
                }
            }
            m_bufSize = bomSize + outIndex * sizeof(uint16_t);
            break;
        }
        default:
            break;
    }

    return m_bufSize;
}

bool Utf8_16_Write::writeToFile(const QString& filePath, const char* data, size_t size) {
    // Convert in memory first
    size_t outSize = convert(data, size);
    if (outSize == 0) return false;

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    qint64 written = f.write(reinterpret_cast<const char*>(m_pNewBuf),
                              static_cast<qint64>(outSize));
    f.close();

    return written == static_cast<qint64>(outSize);
}

void Utf8_16_Write::close() {
    if (m_pNewBuf) {
        delete[] m_pNewBuf;
        m_pNewBuf = nullptr;
        m_bufSize = 0;
    }
}
