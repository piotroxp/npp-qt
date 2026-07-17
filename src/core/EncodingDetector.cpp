// EncodingDetector.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "EncodingDetector.h"
#include <fstream>
#include <cstring>
#include <QStringConverter>
#include <QString>
#include <QFile>
#include <QRegularExpression>

EncodingDetector::EncodingDetector() = default;
EncodingDetector::~EncodingDetector() = default;

EncodingType EncodingDetector::detect(const std::string& bytes) {
    return detectWithHints(bytes, std::string());
}

EncodingType EncodingDetector::detect(const std::string& bytes, const std::string& /*fallback*/) {
    return detectWithHints(bytes, std::string());
}

EncodingType EncodingDetector::detectWithHints(const std::string& bytes, const std::string& fileName) {
    EncodingType bom = detectBOM(bytes);
    if (bom != EncodingType::ANSI) return bom;

    // Try HTML/XML charset detection first for web files
    EncodingType fromMeta = detectFromMetaCharset(bytes);
    if (fromMeta != EncodingType::ANSI) return fromMeta;

    // Check if it looks like UTF-8
    if (isValidUtf8(bytes)) return EncodingType::UTF_8;

    // Use extension hint as fallback
    return hintFromExtension(fileName);
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
    return isValidUtf8(std::string_view(bytes.data(), bytes.size()));
}

int EncodingDetector::validateUtf8Sequence(const unsigned char* data, size_t len, size_t* outLen) const {
    if (len == 0) return -1;
    unsigned char c = data[0];
    // Bare continuation byte (0x80-0xBF) at start of sequence — invalid
    if ((c & 0xC0) == 0x80) return -1;
    if ((c & 0x80) == 0) { *outLen = 1; return 0; }
    if ((c & 0xE0) == 0xC0) *outLen = 2;
    else if ((c & 0xF0) == 0xE0) *outLen = 3;
    else if ((c & 0xF8) == 0xF0) *outLen = 4;
    else return -1;
    if (len < *outLen) return -1;
    for (size_t i = 1; i < *outLen; ++i) {
        if ((data[i] & 0xC0) != 0x80) return -1;
    }
    // ── Codepoint overlong check ──────────────────────────────────────────
    // Encode the decoded codepoint and verify it fits the sequence length.
    // Reject sequences that encode a codepoint smaller than the minimum
    // valid value for that byte count (e.g. U+0000 encoded as C0 80).
    if (*outLen == 2) {
        uint32_t cp = ((data[0] & 0x1F) << 6) | (data[1] & 0x3F);
        if (cp < 0x80) return -1;
    } else if (*outLen == 3) {
        uint32_t cp = ((data[0] & 0x0F) << 12) | ((data[1] & 0x3F) << 6) | (data[2] & 0x3F);
        if (cp < 0x800) return -1;
    } else if (*outLen == 4) {
        uint32_t cp = ((data[0] & 0x07) << 18) | ((data[1] & 0x3F) << 12)
                    | ((data[2] & 0x3F) << 6) | (data[3] & 0x3F);
        if (cp < 0x10000) return -1;
    }
    return 0;
}

bool EncodingDetector::isValidUtf8(const std::string_view bytes) const {
    size_t i = 0;
    size_t len = bytes.size();
    const unsigned char* data = reinterpret_cast<const unsigned char*>(bytes.data());

    while (i < len) {
        size_t seqLen = 0;
        if (validateUtf8Sequence(data + i, len - i, &seqLen) < 0)
            return false;
        i += seqLen;
    }
    return true;
}

static QStringConverter::Encoding encodingToQStringEncoder(EncodingType enc) {
    switch (enc) {
        case EncodingType::UTF_8:
        case EncodingType::UTF_8_BOM:
            return QStringConverter::Utf8;
        case EncodingType::UTF_16_LE:
            return QStringConverter::Utf16;
        case EncodingType::UTF_16_BE:
            return QStringConverter::Utf16;
        case EncodingType::UTF_32_LE:
            return QStringConverter::Utf32;
        case EncodingType::UTF_32_BE:
            return QStringConverter::Utf32;
        default:
            return QStringConverter::System;
    }
}

std::string EncodingDetector::toUtf8(const std::string& bytes, EncodingType from) const {
    if (from == EncodingType::UTF_8 || from == EncodingType::ANSI) return bytes;
    QStringConverter::Encoding enc = encodingToQStringEncoder(from);
    QStringDecoder decoder(enc);
    QString decoded = decoder.decode(QByteArray::fromRawData(bytes.data(), static_cast<int>(bytes.size())));
    if (decoder.hasError()) return bytes;
    return decoded.toUtf8().toStdString();  // toStdString() copies — avoids dangling pointer
}

std::string EncodingDetector::fromUtf8(const std::string& utf8, EncodingType to) const {
    if (to == EncodingType::UTF_8 || to == EncodingType::ANSI) return utf8;
    QStringConverter::Encoding enc = encodingToQStringEncoder(to);
    QStringEncoder encoder(enc);
    QString decoded = QString::fromUtf8(utf8);
    QByteArray result = encoder.encode(decoded);
    if (encoder.hasError()) return utf8;
    return std::string(result.constData(), result.size());
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
    QString name = QString::fromStdString(fileName).toLower();
    if (name.endsWith(".txt") || name.endsWith(".csv") || name.endsWith(".log"))
        return EncodingType::ANSI; // plain text: use system locale
    if (name.endsWith(".xml") || name.endsWith(".html") || name.endsWith(".htm")
        || name.endsWith(".xhtml") || name.endsWith(".svg"))
        return EncodingType::UTF_8; // web files: UTF-8 by default
    if (name.endsWith(".py") || name.endsWith(".js") || name.endsWith(".ts")
        || name.endsWith(".tsx") || name.endsWith(".jsx") || name.endsWith(".json")
        || name.endsWith(".md") || name.endsWith(".cmake") || name.endsWith(".sh")
        || name.endsWith(".bash") || name.endsWith(".yaml") || name.endsWith(".yml")
        || name.endsWith(".toml") || name.endsWith(".ini") || name.endsWith(".cfg")
        || name.endsWith(".conf") || name.endsWith(".txt") || name.endsWith(".rst")
        || name.endsWith(".c") || name.endsWith(".cpp") || name.endsWith(".h")
        || name.endsWith(".hpp") || name.endsWith(".cxx") || name.endsWith(".cc")
        || name.endsWith(".java") || name.endsWith(".go") || name.endsWith(".rs")
        || name.endsWith(".rb") || name.endsWith(".php") || name.endsWith(".cs"))
        return EncodingType::UTF_8;
    if (name.endsWith(".java") && name.contains("encoding"))
        return EncodingType::UTF_8;
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
    // Heuristic: if bytes contain high-freq Windows-1252 chars (smart quotes, em-dash, etc.)
    // that are not valid UTF-8, likely Windows-1252. Otherwise ANSI/ISO-8859-1.
    bool has1252Markers = false;
    for (size_t i = 0; i < bytes.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(bytes[i]);
        // Windows-1252 specific bytes in the 0x80-0x9F range (these are different from ISO-8859-1)
        if (c >= 0x80 && c <= 0x9F) {
            // In ISO-8859-1 these are control chars; in Win-1252 they are printable
            // Simple heuristic: if we see common Win-1252 chars, assume Win-1252
            static const unsigned char win1252Markers[] = {
                0x82, 0x83, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
                0x99, 0x9C, 0x9E, 0x9F
            };
            for (unsigned char m : win1252Markers) {
                if (c == m) { has1252Markers = true; break; }
            }
        }
    }
    return has1252Markers ? EncodingType::ANSI : EncodingType::ANSI; // Win-1252 and ISO-8859-1 both return ANSI here (our EncodingType doesn't distinguish single-byte codepages, treated as system locale)
}

EncodingType EncodingDetector::detectFromMetaCharset(const std::string& bytes) const {
    // Look for <meta charset="..."> HTML5 style
    QString content = QString::fromUtf8(bytes);
    QRegularExpression html5Rx("<meta\\s+[^>]*charset\\s*=\\s*[\"']?([^\"'\\s>]+)",
                                QRegularExpression::CaseInsensitiveOption);
    auto match = html5Rx.match(content);
    if (match.hasMatch()) {
        QString charset = match.captured(1).toUpper();
        if (charset == "UTF-8" || charset == "UTF8") return EncodingType::UTF_8;
        if (charset == "UTF-16" || charset == "UTF16") return EncodingType::UTF_16_LE;
        if (charset == "ISO-8859-1" || charset == "LATIN1") return EncodingType::ANSI;
    }
    // Look for <meta http-equiv="Content-Type" content="text/html; charset=...">
    QRegularExpression httpEquivRx("<meta\\s+[^>]*http-equiv\\s*=\\s*[\"']?Content-Type[\"']?[^>]*content\\s*=\\s*[\"'][^\"']*charset\\s*=\\s*([^\"'\\s;]+)",
                                   QRegularExpression::CaseInsensitiveOption);
    match = httpEquivRx.match(content);
    if (match.hasMatch()) {
        QString charset = match.captured(1).toUpper();
        if (charset == "UTF-8" || charset == "UTF8") return EncodingType::UTF_8;
        if (charset == "UTF-16" || charset == "UTF16") return EncodingType::UTF_16_LE;
        if (charset == "ISO-8859-1" || charset == "LATIN1") return EncodingType::ANSI;
    }
    // Look for <?xml version="..." encoding="..."?>
    QRegularExpression xmlRx("<\\?xml[^>]+encoding\\s*=\\s*[\"']([^\"']+)[\"']",
                             QRegularExpression::CaseInsensitiveOption);
    match = xmlRx.match(content);
    if (match.hasMatch()) {
        QString charset = match.captured(1).toUpper();
        if (charset == "UTF-8" || charset == "UTF8") return EncodingType::UTF_8;
        if (charset == "UTF-16" || charset == "UTF16") return EncodingType::UTF_16_LE;
        if (charset == "ISO-8859-1" || charset == "LATIN1") return EncodingType::ANSI;
    }
    return EncodingType::ANSI;
}

// Static convenience: detect encoding from file path
EncodingType EncodingDetector::detectFromPath(const std::string& filePath) {
    std::ifstream ifs(filePath, std::ios::binary | std::ios::ate);
    if (!ifs) return EncodingType::ANSI;
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::string bytes(size, '\0');
    if (!ifs.read(bytes.data(), size)) return EncodingType::ANSI;
    return instance().detectWithHints(bytes, filePath);
}
