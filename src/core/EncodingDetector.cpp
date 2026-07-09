// EncodingDetector.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "EncodingDetector.h"
#include <cstring>

EncodingDetector::EncodingDetector() = default;
EncodingDetector::~EncodingDetector() = default;

EncodingType EncodingDetector::detect(const std::string& bytes) {
    return detect(bytes, "UTF-8");
}

EncodingType EncodingDetector::detect(const std::string& bytes, const std::string& fallback) {
    (void)fallback;
    EncodingType bom = detectBOM(bytes);
    if (bom != EncodingType::ANSI) return bom;
    if (isValidUtf8(bytes)) return EncodingType::UTF_8;
    return EncodingType::ANSI;
}

EncodingType EncodingDetector::detectWithHints(const std::string& bytes, const std::string& fileName) {
    EncodingType fromExt = hintFromExtension(fileName);
    EncodingType bom = detectBOM(bytes);
    if (bom != EncodingType::ANSI) return bom;
    if (isValidUtf8(bytes)) return EncodingType::UTF_8;
    (void)fromExt;
    return EncodingType::ANSI;
}

EncodingType EncodingDetector::detectBOM(const std::string& bytes) const {
    if (bytes.size() >= 3 && (unsigned char)bytes[0] == 0xEF && (unsigned char)bytes[1] == 0xBB && (unsigned char)bytes[2] == 0xBF)
        return EncodingType::UTF_8_BOM;
    if (bytes.size() >= 2 && (unsigned char)bytes[0] == 0xFF && (unsigned char)bytes[1] == 0xFE)
        return EncodingType::UTF_16_LE;
    if (bytes.size() >= 2 && (unsigned char)bytes[0] == 0xFE && (unsigned char)bytes[1] == 0xFF)
        return EncodingType::UTF_16_BE;
    return EncodingType::ANSI;
}

bool EncodingDetector::isValidUtf8(const std::string& bytes) const {
    return isValidUtf8(std::string_view(bytes));
}

bool EncodingDetector::isValidUtf8(const std::string_view bytes) const {
    size_t len = 0;
    return validateUtf8Sequence(reinterpret_cast<const unsigned char*>(bytes.data()), bytes.size(), &len) >= 0;
}

int EncodingDetector::validateUtf8Sequence(const unsigned char* data, size_t len, size_t* outLen) const {
    if (len == 0) return -1;
    unsigned char c = data[0];
    if ((c & 0x80) == 0) { *outLen = 1; return 0; }
    if ((c & 0xE0) == 0xC0) *outLen = 2;
    else if ((c & 0xF0) == 0xE0) *outLen = 3;
    else if ((c & 0xF8) == 0xF0) *outLen = 4;
    else return -1;
    if (len < *outLen) return -1;
    for (size_t i = 1; i < *outLen; ++i) {
        if ((data[i] & 0xC0) != 0x80) return -1;
    }
    return 0;
}

std::string EncodingDetector::toUtf8(const std::string& bytes, EncodingType from) const {
    if (from == EncodingType::UTF_8 || from == EncodingType::ANSI) return bytes;
    return bytes;
}

std::string EncodingDetector::fromUtf8(const std::string& utf8, EncodingType to) const {
    if (to == EncodingType::UTF_8 || to == EncodingType::ANSI) return utf8;
    return utf8;
}

EolType EncodingDetector::detectEol(const std::string& bytes) const {
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (bytes[i] == '\n') return EolType::EOL_LF;
        if (bytes[i] == '\r') {
            if (i + 1 < bytes.size() && bytes[i + 1] == '\n') return EolType::EOL_CRLF;
            return EolType::EOL_CR;
        }
    }
    return EolType::EOL_LF;
}

bool EncodingDetector::isUnicode(EncodingType enc) {
    return enc == EncodingType::UTF_8 || enc == EncodingType::UTF_8_BOM ||
           enc == EncodingType::UTF_16_LE || enc == EncodingType::UTF_16_BE ||
           enc == EncodingType::UTF_32_LE || enc == EncodingType::UTF_32_BE;
}

std::string EncodingDetector::encodingName(EncodingType enc) {
    switch (enc) {
        case EncodingType::UTF_8: return "UTF-8";
        case EncodingType::UTF_8_BOM: return "UTF-8-BOM";
        case EncodingType::UTF_16_LE: return "UTF-16 LE";
        case EncodingType::UTF_16_BE: return "UTF-16 BE";
        case EncodingType::UTF_32_LE: return "UTF-32 LE";
        case EncodingType::UTF_32_BE: return "UTF-32 BE";
        default: return "ANSI";
    }
}

std::string EncodingDetector::encodingDisplayName(EncodingType enc) {
    return encodingName(enc);
}

int EncodingDetector::encodingToCodepage(EncodingType enc) {
    switch (enc) {
        case EncodingType::UTF_8: return 65001;
        case EncodingType::UTF_16_LE: return 1200;
        case EncodingType::UTF_16_BE: return 1201;
        default: return 1252;
    }
}

EncodingType EncodingDetector::hintFromExtension(const std::string& fileName) const {
    (void)fileName;
    return EncodingType::ANSI;
}

int EncodingDetector::detectCJKLikelihood(const std::string& bytes, EncodingType hintEncoding) const {
    (void)bytes; (void)hintEncoding;
    return 0;
}

int EncodingDetector::analyzeByteFrequency(const std::string& bytes) const {
    (void)bytes;
    return 0;
}

EncodingType EncodingDetector::detectSingleByteCharset(const std::string& bytes) const {
    (void)bytes;
    return EncodingType::ANSI;
}

EncodingType EncodingDetector::detectFromMetaCharset(const std::string& bytes) const {
    (void)bytes;
    return EncodingType::ANSI;
}
