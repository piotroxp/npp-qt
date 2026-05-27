// Utf8_16.cpp - UTF-8/UTF-16 encoding conversion utilities
#include "Utf8_16.h"
#include <QFile>
#include <QByteArray>
#include <QTextCodec>

const unsigned char Utf8_16::UTF8_BOM[] = { 0xEF, 0xBB, 0xBF };
const unsigned char Utf8_16::UTF16_LE_BOM[] = { 0xFF, 0xFE };
const unsigned char Utf8_16::UTF16_BE_BOM[] = { 0xFE, 0xFF };
const unsigned char Utf8_16::UTF32_LE_BOM[] = { 0xFF, 0xFE, 0x00, 0x00 };
const unsigned char Utf8_16::UTF32_BE_BOM[] = { 0x00, 0x00, 0xFE, 0xFF };

Utf8_16::BufferInfo Utf8_16::detectAndConvert(const QByteArray& rawData) {
    BufferInfo result;
    result.data = rawData;
    
    if (rawData.size() >= 3 && 
        static_cast<unsigned char>(rawData[0]) == UTF8_BOM[0] &&
        static_cast<unsigned char>(rawData[1]) == UTF8_BOM[1] &&
        static_cast<unsigned char>(rawData[2]) == UTF8_BOM[2]) {
        // UTF-8 with BOM
        result.type = EncodingType::UTF8_BOM;
        result.hasBOM = true;
        result.data = rawData.mid(3);
        _type = EncodingType::UTF8_BOM;
        _hasBOM = true;
    } else if (rawData.size() >= 2) {
        if (static_cast<unsigned char>(rawData[0]) == UTF16_LE_BOM[0] &&
            static_cast<unsigned char>(rawData[1]) == UTF16_LE_BOM[1]) {
            // UTF-16 LE
            result.type = EncodingType::UTF16_LE;
            result.hasBOM = true;
            result.data = rawData.mid(2);
            _type = EncodingType::UTF16_LE;
            _hasBOM = true;
        } else if (static_cast<unsigned char>(rawData[0]) == UTF16_BE_BOM[0] &&
                   static_cast<unsigned char>(rawData[1]) == UTF16_BE_BOM[1]) {
            // UTF-16 BE
            result.type = EncodingType::UTF16_BE;
            result.hasBOM = true;
            result.data = rawData.mid(2);
            _type = EncodingType::UTF16_BE;
            _hasBOM = true;
        } else {
            // Try to detect if it's UTF-8 or ANSI
            bool isUtf8 = true;
            int nonAsciiCount = 0;
            for (int i = 0; i < rawData.size() && i < 4096; i++) {
                unsigned char c = static_cast<unsigned char>(rawData[i]);
                if (c > 127) {
                    nonAsciiCount++;
                }
                // Simple heuristic: if we see high bytes in patterns not valid for UTF-8, it's probably ANSI
                if (isUtf8 && (c >= 0x80 && c <= 0xC1)) {
                    // Invalid UTF-8 start byte
                    isUtf8 = false;
                }
            }
            
            if (nonAsciiCount > 0 && isUtf8) {
                result.type = EncodingType::UTF8;
                _type = EncodingType::UTF8;
            } else if (nonAsciiCount > 0) {
                result.type = EncodingType::ANSI;
                _type = EncodingType::ANSI;
            } else {
                result.type = EncodingType::ASCII;
                _type = EncodingType::ASCII;
            }
            result.hasBOM = false;
        }
    } else {
        result.type = EncodingType::ASCII;
        _type = EncodingType::ASCII;
        result.hasBOM = false;
    }
    
    return result;
}

QByteArray Utf8_16::toUtf8(const QByteArray& input, EncodingType fromType) {
    if (fromType == EncodingType::UTF8 || fromType == EncodingType::UTF8_BOM) {
        return input;
    }
    
    if (fromType == EncodingType::UTF16_LE || fromType == EncodingType::UTF16_BE) {
        // Convert UTF-16 to UTF-8
        QTextCodec* codec = QTextCodec::codecForName(fromType == EncodingType::UTF16_LE ? "UTF-16LE" : "UTF-16BE");
        if (codec) {
            QTextCodec::ConverterState state;
            return codec->toUnicode(input.constData(), input.size(), &state).toUtf8();
        }
    }
    
    // For ANSI/ASCII, just return the data as-is (it's already compatible with UTF-8 for ASCII range)
    return input;
}

EncodingType Utf8_16::getType(const char* buffer, size_t len) {
    if (len >= 3 && 
        static_cast<unsigned char>(buffer[0]) == UTF8_BOM[0] &&
        static_cast<unsigned char>(buffer[1]) == UTF8_BOM[1] &&
        static_cast<unsigned char>(buffer[2]) == UTF8_BOM[2]) {
        return EncodingType::UTF8_BOM;
    }
    
    if (len >= 2) {
        if (static_cast<unsigned char>(buffer[0]) == UTF16_LE_BOM[0] &&
            static_cast<unsigned char>(buffer[1]) == UTF16_LE_BOM[1]) {
            return EncodingType::UTF16_LE;
        }
        if (static_cast<unsigned char>(buffer[0]) == UTF16_BE_BOM[0] &&
            static_cast<unsigned char>(buffer[1]) == UTF16_BE_BOM[1]) {
            return EncodingType::UTF16_BE;
        }
    }
    
    return EncodingType::UTF8;
}

QByteArray Utf8_16::convert(const QByteArray& input, EncodingType from, EncodingType to) {
    if (from == to) {
        return input;
    }
    
    // First convert to UTF-8
    QByteArray utf8Data = toUtf8(input, from);
    
    // Then convert to target encoding
    if (to == EncodingType::UTF8 || to == EncodingType::UTF8_BOM) {
        return utf8Data;
    }
    
    if (to == EncodingType::ANSI || to == EncodingType::ASCII) {
        // Convert from UTF-8 to system encoding
        QTextCodec* codec = QTextCodec::codecForName("System");
        if (codec) {
            QString unicode = QString::fromUtf8(utf8Data);
            return codec->fromUnicode(unicode);
        }
    }
    
    return utf8Data;
}