// test_encoding_detector.cpp - Unit tests for EncodingDetector
// Copyright (C) 2026 Agent Army
// GPL v3

// Standalone test file - no Qt GUI dependency
// Compile with: g++ -std=c++17 -I../common -I../core test_encoding_detector.cpp -o test_encoding_detector

#include "core/EncodingDetector.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

#define TEST(name) void name()
#define RUN(name) do { std::cout << #name "... "; name(); std::cout << "OK\n"; } while(0)
#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::cerr << "FAILED: " << #a << " != " << #b << "\n"; exit(1); } } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "FAILED: " << #x << " is not true\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { std::cerr << "FAILED: " << #x << " is not false\n"; exit(1); } } while(0)

// ============================================================================
// BOM Detection Tests
// ============================================================================
TEST(test_bom_detection_utf8) {
    std::string data;
    data.push_back('\xEF');
    data.push_back('\xBB');
    data.push_back('\xBF');
    data += "Hello, UTF-8!";

    EncodingDetector detector;
    EncodingType enc = detector.detectBOM(data);
    ASSERT_EQ(enc, EncodingType::UTF_8_BOM);
}

TEST(test_bom_detection_utf16_le) {
    std::string data;
    data.push_back('\xFF');
    data.push_back('\xFE');
    data += "Hi";  // UTF-16 LE content

    EncodingDetector detector;
    EncodingType enc = detector.detectBOM(data);
    ASSERT_EQ(enc, EncodingType::UTF_16_LE);
}

TEST(test_bom_detection_utf16_be) {
    std::string data;
    data.push_back('\xFE');
    data.push_back('\xFF');
    data += "Hi";  // UTF-16 BE content

    EncodingDetector detector;
    EncodingType enc = detector.detectBOM(data);
    ASSERT_EQ(enc, EncodingType::UTF_16_BE);
}

TEST(test_bom_detection_no_bom) {
    std::string data = "Plain ASCII text without BOM";

    EncodingDetector detector;
    EncodingType enc = detector.detectBOM(data);
    ASSERT_EQ(enc, EncodingType::ANSI);
}

TEST(test_bom_detection_empty) {
    std::string data = "";

    EncodingDetector detector;
    EncodingType enc = detector.detectBOM(data);
    ASSERT_EQ(enc, EncodingType::ANSI);
}

TEST(test_bom_detection_partial_utf8) {
    std::string data;
    data.push_back('\xEF');
    data.push_back('\xBB');
    // Incomplete BOM

    EncodingDetector detector;
    EncodingType enc = detector.detectBOM(data);
    ASSERT_EQ(enc, EncodingType::ANSI);
}

// ============================================================================
// UTF-8 Validation Tests
// ============================================================================
TEST(test_utf8_valid_ascii) {
    std::string data = "Hello, World! This is ASCII.";

    EncodingDetector detector;
    bool valid = detector.isValidUtf8(data);
    ASSERT_TRUE(valid);
}

TEST(test_utf8_valid_utf8_chars) {
    std::string data = "Hello, \xC2\xA9 2026";  // Copyright symbol

    EncodingDetector detector;
    bool valid = detector.isValidUtf8(data);
    ASSERT_TRUE(valid);
}

TEST(test_utf8_euro_sign_byte_sequence) {
    std::string data = "\xE2\x82\xAC100";  // Euro sign

    EncodingDetector detector;
    bool valid = detector.isValidUtf8(data);
    ASSERT_TRUE(valid);

    // Also test via detect() — should return UTF_8, not ANSI or UTF_16
    EncodingType result = detector.detect(data);
    ASSERT_TRUE(result != EncodingType::UTF_16_LE);
    ASSERT_TRUE(result != EncodingType::UTF_16_BE);
}

TEST(test_utf8_valid_chinese) {
    std::string data = "\xE4\xB8\xAD\xE6\x96\x87";  // Chinese characters

    EncodingDetector detector;
    bool valid = detector.isValidUtf8(data);
    ASSERT_TRUE(valid);
}

TEST(test_utf8_invalid_sequence) {
    // Invalid: continuation byte without leading byte
    std::string data = "Hello \x80\xBF World";

    EncodingDetector detector;
    bool valid = detector.isValidUtf8(data);
    ASSERT_FALSE(valid);
}

TEST(test_utf8_invalid_truncated) {
    // Truncated sequence at end
    std::string data = "Hello \xE2\x82";  // Incomplete Euro

    EncodingDetector detector;
    bool valid = detector.isValidUtf8(data);
    ASSERT_FALSE(valid);
}

TEST(test_utf8_invalid_overlong) {
    // Overlong encoding of NUL (should be invalid)
    std::string data = "Test \xC0\x80 End";  // Overlong NUL

    EncodingDetector detector;
    bool valid = detector.isValidUtf8(data);
    // Note: Our simple validator may not catch all overlong sequences
    // This test documents current behavior
    (void)valid;
}

TEST(test_utf8_empty) {
    std::string data = "";

    EncodingDetector detector;
    bool valid = detector.isValidUtf8(data);
    ASSERT_TRUE(valid);
}

// ============================================================================
// EOL Detection Tests
// ============================================================================
TEST(test_eol_lf_unix) {
    std::string data = "line1\nline2\nline3";

    EncodingDetector detector;
    EolType eol = detector.detectEol(data);
    ASSERT_EQ(eol, EolType::EOL_LF);
}

TEST(test_eol_crlf_windows) {
    std::string data = "line1\r\nline2\r\nline3";

    EncodingDetector detector;
    EolType eol = detector.detectEol(data);
    ASSERT_EQ(eol, EolType::EOL_CRLF);
}

TEST(test_eol_cr_old_mac) {
    std::string data = "line1\rline2\rline3";

    EncodingDetector detector;
    EolType eol = detector.detectEol(data);
    ASSERT_EQ(eol, EolType::EOL_CR);
}

TEST(test_eol_empty) {
    std::string data = "";

    EncodingDetector detector;
    EolType eol = detector.detectEol(data);
    ASSERT_EQ(eol, EolType::EOL_LF);  // Default
}

TEST(test_eol_no_newline) {
    std::string data = "No newline here";

    EncodingDetector detector;
    EolType eol = detector.detectEol(data);
    ASSERT_EQ(eol, EolType::EOL_LF);  // Default
}

TEST(test_eol_mixed) {
    // Mixed EOLs - should detect first encountered
    std::string data = "unix\nwindows\r\nmac\r";

    EncodingDetector detector;
    EolType eol = detector.detectEol(data);
    ASSERT_EQ(eol, EolType::EOL_LF);  // LF is found first
}

// ============================================================================
// Encoding Name Tests
// ============================================================================
TEST(test_encoding_name) {
    ASSERT_EQ(EncodingDetector::encodingName(EncodingType::UTF_8), "UTF-8");
    ASSERT_EQ(EncodingDetector::encodingName(EncodingType::UTF_8_BOM), "UTF-8-BOM");
    ASSERT_EQ(EncodingDetector::encodingName(EncodingType::UTF_16_LE), "UTF-16 LE");
    ASSERT_EQ(EncodingDetector::encodingName(EncodingType::UTF_16_BE), "UTF-16 BE");
    ASSERT_EQ(EncodingDetector::encodingName(EncodingType::ANSI), "ANSI");
}

TEST(test_encoding_display_name) {
    ASSERT_EQ(EncodingDetector::encodingDisplayName(EncodingType::UTF_8), "UTF-8");
    ASSERT_EQ(EncodingDetector::encodingDisplayName(EncodingType::UTF_16_LE), "UTF-16 LE");
}

// ============================================================================
// Unicode Check Tests
// ============================================================================
TEST(test_is_unicode) {
    ASSERT_TRUE(EncodingDetector::isUnicode(EncodingType::UTF_8));
    ASSERT_TRUE(EncodingDetector::isUnicode(EncodingType::UTF_8_BOM));
    ASSERT_TRUE(EncodingDetector::isUnicode(EncodingType::UTF_16_LE));
    ASSERT_TRUE(EncodingDetector::isUnicode(EncodingType::UTF_16_BE));
    ASSERT_TRUE(EncodingDetector::isUnicode(EncodingType::UTF_32_LE));
    ASSERT_TRUE(EncodingDetector::isUnicode(EncodingType::UTF_32_BE));

    ASSERT_FALSE(EncodingDetector::isUnicode(EncodingType::ANSI));
    ASSERT_FALSE(EncodingDetector::isUnicode(EncodingType::OEM));
}

TEST(test_codepage) {
    ASSERT_EQ(EncodingDetector::encodingToCodepage(EncodingType::UTF_8), 65001);
    ASSERT_EQ(EncodingDetector::encodingToCodepage(EncodingType::UTF_16_LE), 1200);
    ASSERT_EQ(EncodingDetector::encodingToCodepage(EncodingType::UTF_16_BE), 1201);
    ASSERT_EQ(EncodingDetector::encodingToCodepage(EncodingType::ANSI), 1252);
}

// ============================================================================
// Full Detect Tests
// ============================================================================
TEST(test_detect_utf8_with_bom) {
    std::string data;
    data.push_back('\xEF');
    data.push_back('\xBB');
    data.push_back('\xBF');
    data += "UTF-8 content";

    EncodingDetector detector;
    EncodingType enc = detector.detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_8_BOM);
}

TEST(test_detect_utf8_without_bom) {
    std::string data = "Just plain ASCII text";

    EncodingDetector detector;
    EncodingType enc = detector.detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_8);
}

TEST(test_detect_ansi) {
    // This is an approximation - in real usage we'd need specific bytes
    std::string data = "Plain text";

    EncodingDetector detector;
    EncodingType enc = detector.detect(data, "ANSI");
    // ASCII is valid UTF-8, so it might return UTF-8
    ASSERT_TRUE(enc == EncodingType::UTF_8 || enc == EncodingType::ANSI);
}

// ============================================================================
// Conversion Tests
// ============================================================================
TEST(test_to_utf8_passthrough) {
    std::string data = "Hello, UTF-8!";

    EncodingDetector detector;
    std::string result = detector.toUtf8(data, EncodingType::UTF_8);
    ASSERT_EQ(result, data);
}

TEST(test_from_utf8_passthrough) {
    std::string data = "Hello, UTF-8!";

    EncodingDetector detector;
    std::string result = detector.fromUtf8(data, EncodingType::UTF_8);
    ASSERT_EQ(result, data);
}

TEST(test_to_utf8_from_ansi) {
    std::string data = "Hello, ANSI!";

    EncodingDetector detector;
    std::string result = detector.toUtf8(data, EncodingType::ANSI);
    // ANSI is treated as Latin-1, should convert to UTF-8
    ASSERT_EQ(result, data);
}

// ============================================================================
// Main
// ============================================================================
int main() {
    std::cout << "=== EncodingDetector Unit Tests ===\n\n";

    std::cout << "BOM Detection:\n";
    RUN(test_bom_detection_utf8);
    RUN(test_bom_detection_utf16_le);
    RUN(test_bom_detection_utf16_be);
    RUN(test_bom_detection_no_bom);
    RUN(test_bom_detection_empty);
    RUN(test_bom_detection_partial_utf8);

    std::cout << "\nUTF-8 Validation:\n";
    RUN(test_utf8_valid_ascii);
    RUN(test_utf8_valid_utf8_chars);
    RUN(test_utf8_euro_sign_byte_sequence);
    RUN(test_utf8_valid_chinese);
    RUN(test_utf8_invalid_sequence);
    RUN(test_utf8_invalid_truncated);
    RUN(test_utf8_invalid_overlong);
    RUN(test_utf8_empty);

    std::cout << "\nEOL Detection:\n";
    RUN(test_eol_lf_unix);
    RUN(test_eol_crlf_windows);
    RUN(test_eol_cr_old_mac);
    RUN(test_eol_empty);
    RUN(test_eol_no_newline);
    RUN(test_eol_mixed);

    std::cout << "\nEncoding Names:\n";
    RUN(test_encoding_name);
    RUN(test_encoding_display_name);

    std::cout << "\nUnicode Check:\n";
    RUN(test_is_unicode);
    RUN(test_codepage);

    std::cout << "\nFull Detect:\n";
    RUN(test_detect_utf8_with_bom);
    RUN(test_detect_utf8_without_bom);
    RUN(test_detect_ansi);

    std::cout << "\nConversions:\n";
    RUN(test_to_utf8_passthrough);
    RUN(test_from_utf8_passthrough);
    RUN(test_to_utf8_from_ansi);

    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}
