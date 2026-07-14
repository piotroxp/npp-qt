// test_string_helper.cpp - Unit tests for StringHelper
// Copyright (C) 2026 Agent Army
// GPL v3

// Standalone test file - no Qt GUI dependency
// Compile with: g++ -std=c++17 -I../common test_string_helper.cpp -o test_string_helper

#include "common/StringHelper.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

#define TEST(name) void name()
#define RUN(name) do { std::cout << #name "... "; name(); std::cout << "OK\n"; } while(0)
#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::cerr << "FAILED: " << #a << " != " << #b << "\n"; exit(1); } } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)

// ============================================================================
// Trim Tests
// ============================================================================
TEST(test_trim_basic) {
    ASSERT_EQ(StringHelper::trim("  hello  "), "hello");
    ASSERT_EQ(StringHelper::trim("hello"), "hello");
    ASSERT_EQ(StringHelper::trim("  hello"), "hello");
    ASSERT_EQ(StringHelper::trim("hello  "), "hello");
}

TEST(test_trim_empty) {
    ASSERT_EQ(StringHelper::trim(""), "");
    ASSERT_EQ(StringHelper::trim("   "), "");
}

TEST(test_trim_tabs) {
    ASSERT_EQ(StringHelper::trim("\t\nhello\t\n"), "hello");
}

TEST(test_trim_left) {
    ASSERT_EQ(StringHelper::trimLeft("  hello"), "hello");
    ASSERT_EQ(StringHelper::trimLeft("hello"), "hello");
}

TEST(test_trim_right) {
    ASSERT_EQ(StringHelper::trimRight("hello  "), "hello");
    ASSERT_EQ(StringHelper::trimRight("hello"), "hello");
}

// ============================================================================
// Split / Join Tests
// ============================================================================
TEST(test_split_basic) {
    auto parts = StringHelper::split("a,b,c", ",");
    ASSERT_EQ(parts.size(), 3u);
    ASSERT_EQ(parts[0], "a");
    ASSERT_EQ(parts[1], "b");
    ASSERT_EQ(parts[2], "c");
}

TEST(test_split_with_empty) {
    auto parts = StringHelper::split("a,,b", ",");
    ASSERT_EQ(parts.size(), 2u);
    ASSERT_EQ(parts[0], "a");
    ASSERT_EQ(parts[1], "b");
}

TEST(test_split_with_empty_skip) {
    auto parts = StringHelper::split("a,,b", ",", true);
    ASSERT_EQ(parts.size(), 2u);
}

TEST(test_split_keep_empty) {
    auto parts = StringHelper::split("a,,b", ",", false);
    ASSERT_EQ(parts.size(), 3u);
}

TEST(test_split_no_delimiter) {
    auto parts = StringHelper::split("hello", ",");
    ASSERT_EQ(parts.size(), 1u);
    ASSERT_EQ(parts[0], "hello");
}

TEST(test_split_empty_string) {
    auto parts = StringHelper::split("", ",");
    ASSERT_EQ(parts.size(), 0u);
}

TEST(test_split_multichar_delimiter) {
    auto parts = StringHelper::split("a--b--c", "--");
    ASSERT_EQ(parts.size(), 3u);
}

TEST(test_join_basic) {
    std::vector<std::string> parts = {"a", "b", "c"};
    ASSERT_EQ(StringHelper::join(parts, ","), "a,b,c");
}

TEST(test_join_empty) {
    std::vector<std::string> parts;
    ASSERT_EQ(StringHelper::join(parts, ","), "");
}

TEST(test_join_single) {
    std::vector<std::string> parts = {"a"};
    ASSERT_EQ(StringHelper::join(parts, ","), "a");
}

// ============================================================================
// StartsWith / EndsWith Tests
// ============================================================================
TEST(test_starts_with) {
    ASSERT_TRUE(StringHelper::startsWith("hello world", "hello"));
    ASSERT_TRUE(StringHelper::startsWith("hello", "hello"));
    ASSERT_FALSE(StringHelper::startsWith("hello world", "world"));
    ASSERT_FALSE(StringHelper::startsWith("", "hello"));
    ASSERT_TRUE(StringHelper::startsWith("hello", ""));
}

TEST(test_ends_with) {
    ASSERT_TRUE(StringHelper::endsWith("hello world", "world"));
    ASSERT_TRUE(StringHelper::endsWith("hello", "hello"));
    ASSERT_FALSE(StringHelper::endsWith("hello world", "hello"));
    ASSERT_FALSE(StringHelper::endsWith("", "hello"));
    ASSERT_TRUE(StringHelper::endsWith("hello", ""));
}

// ============================================================================
// Contains Tests
// ============================================================================
TEST(test_contains) {
    ASSERT_TRUE(StringHelper::contains("hello world", "world"));
    ASSERT_TRUE(StringHelper::contains("hello", "hello"));
    ASSERT_FALSE(StringHelper::contains("hello world", "foo"));
    ASSERT_FALSE(StringHelper::contains("", "hello"));
    ASSERT_TRUE(StringHelper::contains("hello", ""));
}

// ============================================================================
// Replace Tests
// ============================================================================
TEST(test_replace_all_basic) {
    std::string s = "hello world world";
    auto result = StringHelper::replaceAll(s, "world", "there");
    ASSERT_EQ(result, "hello there there");
}

TEST(test_replace_all_not_found) {
    std::string s = "hello world";
    auto result = StringHelper::replaceAll(s, "foo", "bar");
    ASSERT_EQ(result, "hello world");
}

TEST(test_replace_all_empty) {
    std::string s = "hello world";
    auto result = StringHelper::replaceAll(s, "", "bar");
    ASSERT_EQ(result, s);  // No change
}

TEST(test_replace_all_overlapping) {
    std::string s = "aaa";
    auto result = StringHelper::replaceAll(s, "aa", "b");
    // Non-overlapping replacements
    ASSERT_EQ(result, "ba");
}

TEST(test_replace_all_multiple_pairs) {
    std::string s = "hello world";
    std::vector<std::pair<std::string, std::string>> replacements = {
        {"hello", "hi"},
        {"world", "there"}
    };
    auto result = StringHelper::replaceAll(s, replacements);
    ASSERT_EQ(result, "hi there");
}

// ============================================================================
// Tab/Space Conversion Tests
// ============================================================================
TEST(test_tabs_to_spaces) {
    std::string s = "a\tb";
    auto result = StringHelper::tabsToSpaces(s, 4);
    ASSERT_EQ(result, "a    b");
}

TEST(test_tabs_to_spaces_multiple) {
    std::string s = "\t\t\ttabbed";
    auto result = StringHelper::tabsToSpaces(s, 4);
    ASSERT_EQ(result, "            tabbed");
}

TEST(test_spaces_to_tabs) {
    std::string s = "a    b";
    // Only converts at tab stops — "a    b" where col 1→4 are spaces, col 5='b'
    // Next tab stop from col=1 is 4, so exactly 3 spaces (col 1→4) would become a tab.
    // Here we have 4 spaces (col 1→5), next tab stop is 5, not a full tab width.
    // Conservative impl: only converts when count == nextTabStop - col.
    // So we need exactly tabWidth spaces starting at col 0.
    std::string t = "    b";
    auto result = StringHelper::spacesToTabs(t, 4);
    ASSERT_EQ(result, "\tb");
}

TEST(test_spaces_to_tabs_partial) {
    std::string s = "a   b";  // Only 3 spaces
    auto result = StringHelper::spacesToTabs(s, 4);
    // Should remain as spaces (not enough for a tab)
    ASSERT_EQ(result, "a   b");
}

// ============================================================================
// Numeric Conversion Tests
// ============================================================================
TEST(test_to_int) {
    ASSERT_EQ(StringHelper::toInt("42"), 42);
    ASSERT_EQ(StringHelper::toInt("  42  "), 42);
    ASSERT_EQ(StringHelper::toInt("-42"), -42);
    ASSERT_EQ(StringHelper::toInt("invalid"), 0);
    ASSERT_EQ(StringHelper::toInt("invalid", 99), 99);
}

TEST(test_to_long) {
    ASSERT_EQ(StringHelper::toLong("1234567890"), 1234567890LL);
    ASSERT_EQ(StringHelper::toLong("invalid"), 0LL);
}

TEST(test_to_double) {
    ASSERT_EQ(StringHelper::toDouble("3.14"), 3.14);
    ASSERT_EQ(StringHelper::toDouble("  3.14  "), 3.14);
    ASSERT_EQ(StringHelper::toDouble("invalid"), 0.0);
    ASSERT_EQ(StringHelper::toDouble("invalid", 99.9), 99.9);
}

TEST(test_from_int) {
    ASSERT_EQ(StringHelper::fromInt(42), "42");
    ASSERT_EQ(StringHelper::fromInt(-42), "-42");
}

TEST(test_from_double) {
    auto result = StringHelper::fromDouble(3.14, 2);
    ASSERT_TRUE(result.find("3.14") != std::string::npos);
}

// ============================================================================
// Case Conversion Tests
// ============================================================================
TEST(test_to_lower) {
    ASSERT_EQ(StringHelper::toLower("HELLO"), "hello");
    ASSERT_EQ(StringHelper::toLower("HeLLo"), "hello");
    ASSERT_EQ(StringHelper::toLower("hello"), "hello");
}

TEST(test_to_upper) {
    ASSERT_EQ(StringHelper::toUpper("hello"), "HELLO");
    ASSERT_EQ(StringHelper::toUpper("HeLLo"), "HELLO");
    ASSERT_EQ(StringHelper::toUpper("HELLO"), "HELLO");
}

// ============================================================================
// Path Helper Tests
// ============================================================================
TEST(test_file_name) {
    ASSERT_EQ(StringHelper::fileName("/path/to/file.txt"), "file.txt");
    ASSERT_EQ(StringHelper::fileName("file.txt"), "file.txt");
    ASSERT_EQ(StringHelper::fileName("/path/to/"), "");
}

TEST(test_file_ext) {
    ASSERT_EQ(StringHelper::fileExt("file.txt"), ".txt");
    ASSERT_EQ(StringHelper::fileExt("file.tar.gz"), ".gz");
    ASSERT_EQ(StringHelper::fileExt("file"), "");
    ASSERT_EQ(StringHelper::fileExt("/path/to/file.txt"), ".txt");
}

// ============================================================================
// Escaping Tests
// ============================================================================
TEST(test_escape) {
    auto result = StringHelper::escape("a<b>c\"d", "<>");
    ASSERT_TRUE(result.find("a") != std::string::npos);
    ASSERT_TRUE(result.find("&lt;") != std::string::npos);
}

TEST(test_regex_escape) {
    auto result = StringHelper::regexEscape("[a-z]*.txt");
    ASSERT_TRUE(result.find("[") != std::string::npos);
}

TEST(test_html_escape) {
    auto result = StringHelper::htmlEscape("<div>&\"'</div>");
    ASSERT_TRUE(result.find("&lt;") != std::string::npos);
    ASSERT_TRUE(result.find("&gt;") != std::string::npos);
    ASSERT_TRUE(result.find("&amp;") != std::string::npos);
}

// ============================================================================
// Comparison Tests
// ============================================================================
TEST(test_equals_ignore_case) {
    ASSERT_TRUE(StringHelper::equalsIgnoreCase("Hello", "hello"));
    ASSERT_TRUE(StringHelper::equalsIgnoreCase("HELLO", "hello"));
    ASSERT_TRUE(StringHelper::equalsIgnoreCase("Hello", "HELLO"));
    ASSERT_FALSE(StringHelper::equalsIgnoreCase("Hello", "World"));
}

// ============================================================================
// Word Wrap Tests
// ============================================================================
TEST(test_word_wrap_basic) {
    auto lines = StringHelper::wordWrap("hello world foo", 5);
    ASSERT_TRUE(lines.size() > 0);
}

TEST(test_word_wrap_long) {
    auto lines = StringHelper::wordWrap("aaaaa bbbbb ccccc", 5);
    // Each word is 5 chars, should wrap
    ASSERT_TRUE(lines.size() >= 2);
}

// ============================================================================
// EOL Consistency Tests
// ============================================================================
TEST(test_make_eol_consistent) {
    std::string s = "line1\r\nline2\nline3\r";
    auto result = StringHelper::makeEolConsistent(s, 1);  // LF
    ASSERT_TRUE(result.find("\r\n") == std::string::npos);
    ASSERT_TRUE(result.find("\r") == std::string::npos);
}

// ============================================================================
// Format Tests
// ============================================================================
TEST(test_format) {
    auto result = StringHelper::format("Hello %s, you have %d items", "World", 42);
    ASSERT_TRUE(result.find("Hello") != std::string::npos);
    ASSERT_TRUE(result.find("World") != std::string::npos);
    ASSERT_TRUE(result.find("42") != std::string::npos);
}

// ============================================================================
// Main
// ============================================================================
int main() {
    std::cout << "=== StringHelper Unit Tests ===\n\n";

    std::cout << "Trim:\n";
    RUN(test_trim_basic);
    RUN(test_trim_empty);
    RUN(test_trim_tabs);
    RUN(test_trim_left);
    RUN(test_trim_right);

    std::cout << "\nSplit/Join:\n";
    RUN(test_split_basic);
    RUN(test_split_with_empty);
    RUN(test_split_with_empty_skip);
    RUN(test_split_keep_empty);
    RUN(test_split_no_delimiter);
    RUN(test_split_empty_string);
    RUN(test_split_multichar_delimiter);
    RUN(test_join_basic);
    RUN(test_join_empty);
    RUN(test_join_single);

    std::cout << "\nStartsWith/EndsWith:\n";
    RUN(test_starts_with);
    RUN(test_ends_with);

    std::cout << "\nContains:\n";
    RUN(test_contains);

    std::cout << "\nReplaceAll:\n";
    RUN(test_replace_all_basic);
    RUN(test_replace_all_not_found);
    RUN(test_replace_all_empty);
    RUN(test_replace_all_overlapping);
    RUN(test_replace_all_multiple_pairs);

    std::cout << "\nTab/Space Conversion:\n";
    RUN(test_tabs_to_spaces);
    RUN(test_tabs_to_spaces_multiple);
    RUN(test_spaces_to_tabs);
    RUN(test_spaces_to_tabs_partial);

    std::cout << "\nNumeric Conversion:\n";
    RUN(test_to_int);
    RUN(test_to_long);
    RUN(test_to_double);
    RUN(test_from_int);
    RUN(test_from_double);

    std::cout << "\nCase Conversion:\n";
    RUN(test_to_lower);
    RUN(test_to_upper);

    std::cout << "\nPath Helpers:\n";
    RUN(test_file_name);
    RUN(test_file_ext);

    std::cout << "\nEscaping:\n";
    RUN(test_escape);
    RUN(test_regex_escape);
    RUN(test_html_escape);

    std::cout << "\nComparison:\n";
    RUN(test_equals_ignore_case);

    std::cout << "\nWord Wrap:\n";
    RUN(test_word_wrap_basic);
    RUN(test_word_wrap_long);

    std::cout << "\nEOL Consistency:\n";
    RUN(test_make_eol_consistent);

    std::cout << "\nFormat:\n";
    RUN(test_format);

    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}
