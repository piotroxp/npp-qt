// test_util.cpp - Unit tests for Util utilities
// Copyright (C) 2026 Agent Army
// GPL v3

// Standalone test file - no Qt GUI dependency
// Compile with: g++ -std=c++17 -I../common test_util.cpp -o test_util

#include "common/Util.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::cerr << "FAILED: " << #a << " != " << #b << "\n"; exit(1); } } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { std::cerr << "FAILED: " << #x << " is not false\n"; exit(1); } } while(0)

// ============================================================================
// CircularBuffer Tests
// ============================================================================
static void test_circular_buffer_push_pop() {
    CircularBuffer<int, 4> buf;

    ASSERT_TRUE(buf.empty());
    ASSERT_FALSE(buf.full());
    ASSERT_EQ(buf.size(), 0u);

    ASSERT_TRUE(buf.push(1));
    ASSERT_EQ(buf.size(), 1u);

    ASSERT_TRUE(buf.push(2));
    ASSERT_EQ(buf.size(), 2u);

    int val;
    ASSERT_TRUE(buf.pop(val));
    ASSERT_EQ(val, 1);
    ASSERT_EQ(buf.size(), 1u);

    ASSERT_TRUE(buf.pop(val));
    ASSERT_EQ(val, 2);
    ASSERT_EQ(buf.size(), 0u);

    ASSERT_TRUE(buf.empty());
}

static void test_circular_buffer_overwrite() {
    CircularBuffer<int, 3> buf;

    buf.push(1);
    buf.push(2);
    buf.push(3);
    ASSERT_TRUE(buf.full());
    ASSERT_EQ(buf.size(), 3u);

    // Overwrite oldest
    buf.push(4);
    ASSERT_TRUE(buf.full());
    ASSERT_EQ(buf.size(), 3u);

    int val;
    ASSERT_TRUE(buf.pop(val));
    ASSERT_EQ(val, 2);  // 1 was overwritten
}

static void test_circular_buffer_clear() {
    CircularBuffer<int, 5> buf;
    buf.push(1);
    buf.push(2);
    buf.push(3);
    ASSERT_EQ(buf.size(), 3u);

    buf.clear();
    ASSERT_TRUE(buf.empty());
    ASSERT_EQ(buf.size(), 0u);
}

// ============================================================================
// AtomicCounter Tests
// ============================================================================
static void test_atomic_counter_increment() {
    AtomicCounter<int> counter(0);
    ASSERT_EQ(counter.get(), 0);
    ASSERT_EQ(counter.increment(), 1);
    ASSERT_EQ(counter.get(), 1);
    ASSERT_EQ(counter.increment(), 2);
    ASSERT_EQ(counter.get(), 2);
}

static void test_atomic_counter_decrement() {
    AtomicCounter<int> counter(5);
    ASSERT_EQ(counter.decrement(), 4);
    ASSERT_EQ(counter.decrement(), 3);
    ASSERT_EQ(counter.get(), 3);
}

static void test_atomic_counter_set() {
    AtomicCounter<int> counter(0);
    counter.set(42);
    ASSERT_EQ(counter.get(), 42);
}

// ============================================================================
// IniParser Tests
// ============================================================================
static void test_ini_parser_basic() {
    IniParser ini;

    ini.set("Section1", "key1", "value1");
    ini.set("Section1", "key2", "value2");
    ini.set("Section2", "key3", "value3");

    ASSERT_EQ(ini.get("Section1", "key1", ""), "value1");
    ASSERT_EQ(ini.get("Section1", "key2", ""), "value2");
    ASSERT_EQ(ini.get("Section2", "key3", ""), "value3");
    ASSERT_EQ(ini.get("Section1", "missing", "default"), "default");
}

static void test_ini_parser_int_bool() {
    IniParser ini;

    ini.set("Settings", "count", 42);
    ini.set("Settings", "enabled", true);
    ini.set("Settings", "disabled", false);

    ASSERT_EQ(ini.getInt("Settings", "count", 0), 42);
    ASSERT_TRUE(ini.getBool("Settings", "enabled", false));
    ASSERT_FALSE(ini.getBool("Settings", "disabled", true));
}

static void test_ini_parser_save_load() {
    std::string path = "/tmp/test_ini_parser.ini";

    // Create and save
    IniParser ini1;
    ini1.set("General", "Name", "Notepad--");
    ini1.set("General", "Version", 1);
    ini1.set("Editor", "TabWidth", 4);
    ASSERT_TRUE(ini1.save(path));

    // Load and verify
    IniParser ini2;
    ASSERT_TRUE(ini2.load(path));
    ASSERT_EQ(ini2.get("General", "Name", ""), "Notepad--");
    ASSERT_EQ(ini2.getInt("General", "Version", 0), 1);
    ASSERT_EQ(ini2.getInt("Editor", "TabWidth", 0), 4);

    // Cleanup
    std::filesystem::remove(path);
}

static void test_ini_parser_has_section() {
    IniParser ini;
    ini.set("Section1", "key", "value");
    ini.set("Section2", "key", "value");

    ASSERT_TRUE(ini.hasSection("Section1"));
    ASSERT_TRUE(ini.hasSection("Section2"));
    ASSERT_FALSE(ini.hasSection("Section3"));
}

// ============================================================================
// Version Comparison Tests
// ============================================================================
static void test_version_compare() {
    ASSERT_EQ(compareVersion("1.0.0", "1.0.0"), 0);
    ASSERT_TRUE(compareVersion("2.0.0", "1.0.0") > 0);
    ASSERT_TRUE(compareVersion("1.1.0", "1.0.0") > 0);
    ASSERT_TRUE(compareVersion("1.0.1", "1.0.0") > 0);
    ASSERT_TRUE(compareVersion("1.0.0", "2.0.0") < 0);
    ASSERT_TRUE(compareVersion("1.0.0", "1.1.0") < 0);
    ASSERT_TRUE(compareVersion("1.0.0", "1.0.1") < 0);
}

// ============================================================================
// String/Path Helper Tests
// ============================================================================
static void test_string_helper_trim() {
    using namespace StringHelper;

    ASSERT_EQ(trim("  hello  "), "hello");
    ASSERT_EQ(trimLeft("  hello"), "hello");
    ASSERT_EQ(trimRight("hello  "), "hello");
    ASSERT_EQ(trim(""), "");
    ASSERT_EQ(trim("   "), "");
}

static void test_string_helper_split() {
    using namespace StringHelper;

    auto parts = split("a,b,c", ",");
    ASSERT_EQ(parts.size(), 3u);
    ASSERT_EQ(parts[0], "a");
    ASSERT_EQ(parts[1], "b");
    ASSERT_EQ(parts[2], "c");

    auto parts2 = split("a::b::c", "::");
    ASSERT_EQ(parts2.size(), 3u);

    auto parts3 = split("a,,b,c", ",", true);
    ASSERT_EQ(parts3.size(), 3u);  // skip empty

    auto parts4 = split("a,,b,c", ",", false);
    ASSERT_EQ(parts4.size(), 4u);  // include empty
}

static void test_string_helper_replace() {
    using namespace StringHelper;

    ASSERT_EQ(replaceAll("hello world", "world", "there"), "hello there");
    ASSERT_EQ(replaceAll("aaa", "a", "b"), "bbb");
    ASSERT_EQ(replaceAll("hello", "x", "y"), "hello");  // no change
}

static void test_string_helper_starts_ends() {
    using namespace StringHelper;

    ASSERT_TRUE(startsWith("hello world", "hello"));
    ASSERT_FALSE(startsWith("hello world", "world"));
    ASSERT_TRUE(endsWith("hello world", "world"));
    ASSERT_FALSE(endsWith("hello world", "hello"));
}

static void test_string_helper_contains() {
    using namespace StringHelper;

    ASSERT_TRUE(contains("hello world", "world"));
    ASSERT_FALSE(contains("hello world", "foo"));
}

static void test_string_helper_to_int() {
    using namespace StringHelper;

    ASSERT_EQ(toInt("42"), 42);
    ASSERT_EQ(toInt("  42  "), 42);
    ASSERT_EQ(toInt("notanumber", 99), 99);
    ASSERT_EQ(toInt("3.14", 0), 3);  // stops at decimal
}

static void test_string_helper_join() {
    using namespace StringHelper;

    std::vector<std::string> parts = {"a", "b", "c"};
    ASSERT_EQ(join(parts, ","), "a,b,c");
    ASSERT_EQ(join(parts, " | "), "a | b | c");
}

// ============================================================================
// FileHelper Tests
// ============================================================================
static void test_file_helper_exists() {
    ASSERT_TRUE(FileHelper::exists("/tmp"));
    ASSERT_FALSE(FileHelper::exists("/nonexistent_path_12345"));
}

static void test_file_helper_file_operations() {
    std::string path = "/tmp/test_util_file.txt";

    // Write
    ASSERT_TRUE(FileHelper::writeFile(path, "hello world"));

    // Read
    auto content = FileHelper::readFile(path);
    ASSERT_TRUE(content.has_value());
    ASSERT_EQ(content.value(), "hello world");

    // Exists
    ASSERT_TRUE(FileHelper::exists(path));
    ASSERT_TRUE(FileHelper::isFile(path));
    ASSERT_FALSE(FileHelper::isDirectory(path));

    // Size
    ASSERT_TRUE(FileHelper::fileSize(path) > 0);

    // Remove
    ASSERT_TRUE(FileHelper::removeFile(path));
    ASSERT_FALSE(FileHelper::exists(path));
}

// ============================================================================
// Run All Tests
// ============================================================================
int main() {
    std::cout << "=== test_util.cpp ===\n";

    // CircularBuffer
    std::cout << "test_circular_buffer_push_pop... "; test_circular_buffer_push_pop(); std::cout << "OK\n";
    std::cout << "test_circular_buffer_overwrite... "; test_circular_buffer_overwrite(); std::cout << "OK\n";
    std::cout << "test_circular_buffer_clear... "; test_circular_buffer_clear(); std::cout << "OK\n";

    // AtomicCounter
    std::cout << "test_atomic_counter_increment... "; test_atomic_counter_increment(); std::cout << "OK\n";
    std::cout << "test_atomic_counter_decrement... "; test_atomic_counter_decrement(); std::cout << "OK\n";
    std::cout << "test_atomic_counter_set... "; test_atomic_counter_set(); std::cout << "OK\n";

    // IniParser
    std::cout << "test_ini_parser_basic... "; test_ini_parser_basic(); std::cout << "OK\n";
    std::cout << "test_ini_parser_int_bool... "; test_ini_parser_int_bool(); std::cout << "OK\n";
    std::cout << "test_ini_parser_save_load... "; test_ini_parser_save_load(); std::cout << "OK\n";
    std::cout << "test_ini_parser_has_section... "; test_ini_parser_has_section(); std::cout << "OK\n";

    // Version
    std::cout << "test_version_compare... "; test_version_compare(); std::cout << "OK\n";

    // StringHelper
    std::cout << "test_string_helper_trim... "; test_string_helper_trim(); std::cout << "OK\n";
    std::cout << "test_string_helper_split... "; test_string_helper_split(); std::cout << "OK\n";
    std::cout << "test_string_helper_replace... "; test_string_helper_replace(); std::cout << "OK\n";
    std::cout << "test_string_helper_starts_ends... "; test_string_helper_starts_ends(); std::cout << "OK\n";
    std::cout << "test_string_helper_contains... "; test_string_helper_contains(); std::cout << "OK\n";
    std::cout << "test_string_helper_to_int... "; test_string_helper_to_int(); std::cout << "OK\n";
    std::cout << "test_string_helper_join... "; test_string_helper_join(); std::cout << "OK\n";

    // FileHelper
    std::cout << "test_file_helper_exists... "; test_file_helper_exists(); std::cout << "OK\n";
    std::cout << "test_file_helper_file_operations... "; test_file_helper_file_operations(); std::cout << "OK\n";

    std::cout << "\nAll tests passed.\n" << std::flush;
    return 0;
}

