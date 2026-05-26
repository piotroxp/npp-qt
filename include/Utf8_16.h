// Utf8_16.h - UTF-8/UTF-16 encoding conversion utilities
#pragma once

#include <QByteArray>
#include <QString>
#include <cstdint>

// Encoding type constants (from Scintilla)
#define SC_CP_UTF8 65001

enum class EncodingType {
   NONE = -1,
   UTF8 = 0,
   UTF8_BOM = 1,
   UTF16_LE = 2,
   UTF16_BE = 3,
   UTF32_LE = 4,
    UTF32_BE = 5,
   ASCII = 6,
    OEM = 7,
   ANSI = 8
};

enum class EolType {
    EolNone = 0,
    EolWindows = 1,    // CR + LF
    EolUnix = 2,       // LF only
    EolMac = 3          // CR only
};

class Utf8_16 {
public:
    Utf8_16() : _type(EncodingType::NONE), _hasBOM(false) {}

    struct BufferInfo {
        QByteArray data;
        EncodingType type = EncodingType::NONE;
        bool hasBOM = false;
    };

    // Detect and convert
    BufferInfo detectAndConvert(const QByteArray& rawData);
    
    // Convert to UTF-8
    QByteArray toUtf8(const QByteArray& input, EncodingType fromType);
    
    // Get encoding type from buffer
    EncodingType getType(const char* buffer, size_t len);
    
    // Convert between encodings
    QByteArray convert(const QByteArray& input, EncodingType from, EncodingType to);
    
    EncodingType getEncodingType() const { return _type; }
    bool hasBOM() const { return _hasBOM; }

private:
    EncodingType _type;
    bool _hasBOM;
    
    // BOM signatures
    static const unsigned char UTF8_BOM[];
    static const unsigned char UTF16_LE_BOM[];
    static const unsigned char UTF16_BE_BOM[];
    static const unsigned char UTF32_LE_BOM[];
    static const unsigned char UTF32_BE_BOM[];
};

// Inline implementations for commonly used conversions
inline QByteArray Utf8_16_toUtf8(const QByteArray& data) {
    Utf8_16 converter;
    auto result = converter.detectAndConvert(data);
    return result.data;
}