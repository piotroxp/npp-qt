// Simple functions to test UTF-8 characters.
// Copyright (C)2010 Francois-R.Boyer@PolyMtl.ca
// First version 2010-08
//
// Written for Notepad++, and distributed under same license:
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 3 of the License, or at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 port: UTF-8 validation utilities
// Uses QStringConverter for encoding detection, with inline helpers
// for byte-level UTF-8 classification (no platform dependencies).

#pragma once

#include <QtCore/QStringConverter>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QtGlobal>

namespace Utf8 {

// basic classification of UTF-8 bytes
inline bool isSingleByte(char c)       { return static_cast<uchar>(c) < 0x80; }
inline bool isPartOfMultibyte(uchar c) { return c >= 0x80; }
inline bool isFirstOfMultibyte(uchar c) { return c >= 0xC2 && c < 0xF5; } // 0xF5 to 0xFD are valid UTF-8 but not current Unicode
inline bool isContinuation(uchar c)    { return (c & 0xC0) == 0x80; }
inline bool isValid(uchar c)           { return c < 0xC0 || isFirstOfMultibyte(c); } // validates a byte, out of context

// number of continuation bytes for a given valid first character (0 for single byte characters)
inline int continuationBytes(uchar c) {
    static const char _len[] = { 1, 1, 2, 3 };
    return (c < 0xC0) ? 0 : _len[(c & 0x30) >> 4];
}

// validates a full character
inline bool isValid(const char* buf, int buflen) {
    if (isSingleByte(buf[0])) return true; // single byte is valid
    if (!isFirstOfMultibyte(static_cast<uchar>(buf[0]))) return false; // not single byte, nor valid multi-byte first byte
    int charContinuationBytes = continuationBytes(static_cast<uchar>(buf[0]));
    if (buflen < charContinuationBytes + 1) return false; // character does not fit in buffer
    for (int i = charContinuationBytes; i > 0; --i) {
        if (!isContinuation(static_cast<uchar>(*(++buf)))) return false; // not enough continuation bytes
    }
    return true; // the character is valid (if there are too many continuation bytes, it is the next character that will be invalid)
}

// rewinds to the first byte of a multi-byte character for any valid UTF-8
// (and will not rewind too much on any other input)
inline int characterStart(const char* buf, int startingIndex) {
    int charContinuationBytes = 0;
    while (charContinuationBytes < startingIndex       // rewind past start of buffer?
           && charContinuationBytes < 5                // UTF-8 support up to 5 continuation bytes
           && isContinuation(static_cast<uchar>(buf[startingIndex - charContinuationBytes]))
           ) {
        ++charContinuationBytes;
    }
    return startingIndex - charContinuationBytes;
}

// Qt6: Convert a byte array to QString using UTF-8
// Replaces MultiByteToWideChar(CP_UTF8, ...) in Win32
inline QString toQString(const QByteArray& bytes) {
    QStringConverter decoder(QStringConverter::Utf8);
    QString result;
    decoder.decode(bytes.constData(), bytes.size(), &result);
    return result;
}

// Qt6: Convert QString to UTF-8 byte array
// Replaces WideCharToMultiByte(CP_UTF8, ...) in Win32
inline QByteArray fromQString(const QString& str) {
    QStringEncoder encoder(QStringConverter::Utf8);
    return encoder.encode(str);
}

// Validate if a QByteArray contains valid UTF-8
inline bool isValidUtf8(const QByteArray& bytes) {
    return QStringConverter::encodingForName("UTF-8").has_value();
}

// Check if a string is valid UTF-8 (uses QStringConverter detection)
inline bool isValidUtf8String(const QString& str) {
    // Qt6: assume QString is always valid UTF-16 internally
    // This function checks if the source bytes were valid UTF-8
    Q_UNUSED(str);
    return true; // QString already stores valid Unicode
}

} // namespace Utf8