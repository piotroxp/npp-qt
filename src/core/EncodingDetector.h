// EncodingDetector.h - Character encoding detection for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/Types.h"
#include "../uchardet/CharsetDetector.h"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

class EncodingDetector {
public:
    static EncodingDetector& instance() { static EncodingDetector e; return e; }
public:
    EncodingDetector();
    ~EncodingDetector();

    // Detect encoding of raw bytes
    EncodingType detect(const std::string& bytes);
    EncodingType detect(const std::string& bytes, const std::string& fallback);

    // Detect with hints
    EncodingType detectWithHints(const std::string& bytes, const std::string& fileName);

    // Detect encoding from file path (reads file, returns encoding)
    static EncodingType detectFromPath(const std::string& filePath);

    // Check for BOM
    EncodingType detectBOM(const std::string& bytes) const;

    // Validate UTF-8 sequence
    bool isValidUtf8(const std::string& bytes) const;
    bool isValidUtf8(const std::string_view bytes) const;

    // Convert to UTF-8
    std::string toUtf8(const std::string& bytes, EncodingType from) const;

    // Convert from UTF-8 to target encoding
    std::string fromUtf8(const std::string& utf8, EncodingType to) const;

    // Line ending detection
    EolType detectEol(const std::string& bytes) const;

    // Check if encoding is Unicode
    static bool isUnicode(EncodingType enc);

    // Get encoding name
    static std::string encodingName(EncodingType enc);
    static std::string encodingDisplayName(EncodingType enc);

    // Get codepage for encoding
    static int encodingToCodepage(EncodingType enc);

    // Map file extension to likely encoding (for hints)
    EncodingType hintFromExtension(const std::string& fileName) const;

    // Statistical analysis for Chinese/Japanese/Korean
    int detectCJKLikelihood(const std::string& bytes, EncodingType hintEncoding) const;

private:
    // Analyze byte frequency for single-byte vs multi-byte detection
    int analyzeByteFrequency(const std::string& bytes) const;

    // Validate individual bytes for UTF-8
    int validateUtf8Sequence(const unsigned char* data, size_t len, size_t* outLen) const;

    // Single-byte charset analysis
    EncodingType detectSingleByteCharset(const std::string& bytes) const;

    // Check for HTML/XML encoding declaration
    EncodingType detectFromMetaCharset(const std::string& bytes) const;

    // Last-resort: use CharsetDetector (uchardet / Qt fallback) to identify
    // an encoding when BOM, meta, UTF-8 validation, and extension hint all
    // come up empty. Returns ANSI if confidence is below threshold.
    EncodingType detectWithCharsetDetector(const std::string& bytes) const;

    // Minimum confidence (0.0-1.0) required to accept CharsetDetector's result.
    // Below this threshold we fall back to ANSI to avoid misdetection.
    static constexpr double m_charsetConfidenceThreshold = 0.5;
};
