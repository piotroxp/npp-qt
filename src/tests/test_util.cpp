// test_util.cpp - Unit tests for Util utilities
// Copyright (C) 2026 Agent Army
// GPL v3

// Standalone test file - no Qt GUI dependency
// Compile with: g++ -std=c++17 -I../common test_util.cpp -o test_util

#include "common/Util.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"
#include <cassert>
#include <QDebug>
#include <iostream>
#include <QDebug>
#include <vector>
#include <QDebug>
#include <string>
#include <QDebug>
#include <fstream>
#include <QDebug>
#include <filesystem>
#include <QDebug>

#define ASSERT_EQ(a, b) do { if ((a) != (b)) { qWarning() << "FAILED: " << #a << " != " << #b << "\n"; exit(1); } } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { qWarning() << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { qWarning() << "FAILED: " << #x << " is not false\n"; exit(1); } } while(0)

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

    ini.set("Section1", "key1", std::string("value1"));
    ini.set("Section1", "key2", std::string("value2"));
    ini.set("Section2", "key3", std::string("value3"));

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
    ini1.set("General", "Name", std::string("Notepad--"));
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
    ini.set("Section1", "key", std::string("value"));
    ini.set("Section2", "key", std::string("value"));

    ASSERT_TRUE(ini.hasSection("Section1"));
    ASSERT_TRUE(ini.hasSection("Section2"));
    ASSERT_FALSE(ini.hasSection("Section3"));
}

static void test_ini_parser_getBool_comprehensive() {
    IniParser ini;
    
    // Test numeric values
    ini.set("Test", "one", std::string("1"));
    ini.set("Test", "zero", std::string("0"));
    ASSERT_TRUE(ini.getBool("Test", "one", false));
    ASSERT_FALSE(ini.getBool("Test", "zero", true));
    
    // Test string values (case insensitive)
    ini.set("Test", "true_str", std::string("true"));
    ini.set("Test", "True_str", std::string("True"));
    ini.set("Test", "TRUE_str", std::string("TRUE"));
    ini.set("Test", "false_str", std::string("false"));
    ini.set("Test", "False_str", std::string("False"));
    ini.set("Test", "FALSE_str", std::string("FALSE"));
    ASSERT_TRUE(ini.getBool("Test", "true_str", false));
    ASSERT_TRUE(ini.getBool("Test", "True_str", false));
    ASSERT_TRUE(ini.getBool("Test", "TRUE_str", false));
    ASSERT_FALSE(ini.getBool("Test", "false_str", true));
    ASSERT_FALSE(ini.getBool("Test", "False_str", true));
    ASSERT_FALSE(ini.getBool("Test", "FALSE_str", true));
    
    // Test yes/no
    ini.set("Test", "yes_val", std::string("yes"));
    ini.set("Test", "Yes_val", std::string("Yes"));
    ini.set("Test", "no_val", std::string("no"));
    ini.set("Test", "No_val", std::string("No"));
    ASSERT_TRUE(ini.getBool("Test", "yes_val", false));
    ASSERT_TRUE(ini.getBool("Test", "Yes_val", false));
    ASSERT_FALSE(ini.getBool("Test", "no_val", true));
    ASSERT_FALSE(ini.getBool("Test", "No_val", true));
    
    // Test on/off
    ini.set("Test", "on_val", std::string("on"));
    ini.set("Test", "off_val", std::string("off"));
    ASSERT_TRUE(ini.getBool("Test", "on_val", false));
    ASSERT_FALSE(ini.getBool("Test", "off_val", true));
    
    // Test default value for missing key
    ASSERT_TRUE(ini.getBool("Test", "nonexistent", true));
    ASSERT_FALSE(ini.getBool("Test", "nonexistent", false));
}

static void test_ini_parser_string_list() {
    IniParser ini;
    
    QStringList list = {"item1", "item2", "item3"};
    ini.setStringList("Test", "items", list);
    
    QStringList result = ini.getStringList("Test", "items", QStringList());
    ASSERT_EQ(result.size(), 3u);
    ASSERT_EQ(result[0], "item1");
    ASSERT_EQ(result[1], "item2");
    ASSERT_EQ(result[2], "item3");
    
    // Test default value
    QStringList defaultList = {"default"};
    QStringList empty = ini.getStringList("Test", "missing", defaultList);
    ASSERT_EQ(empty.size(), 1u);
    ASSERT_EQ(empty[0], "default");
}

static void test_ini_parser_remove_section() {
    IniParser ini;
    ini.set("Section1", "key1", std::string("value1"));
    ini.set("Section2", "key2", std::string("value2"));
    ini.set("Section3", "key3", std::string("value3"));
    ASSERT_TRUE(ini.hasSection("Section1"));
    ASSERT_TRUE(ini.hasSection("Section2"));
    ASSERT_TRUE(ini.hasSection("Section3"));
    
    ini.removeSection("Section2");
    
    ASSERT_TRUE(ini.hasSection("Section1"));
    ASSERT_FALSE(ini.hasSection("Section2"));
    ASSERT_TRUE(ini.hasSection("Section3"));
    
    // Section1 and Section3 should still be accessible
    ASSERT_EQ(ini.get("Section1", "key1", ""), "value1");
    ASSERT_EQ(ini.get("Section3", "key3", ""), "value3");
}

static void test_ini_parser_sections() {
    IniParser ini;
    ini.set("Section1", "key1", std::string("value1"));
    ini.set("Section2", "key2", std::string("value2"));
    ini.set("Section3", "key3", std::string("value3"));
    const auto& sections = ini.sections();
    ASSERT_EQ(sections.size(), 3u);
}

static void test_ini_parser_comments() {
    std::string path = "/tmp/test_ini_with_comments.ini";
    
    // Create file with comments
    {
        std::ofstream fout(path);
        fout << "; This is a comment\n";
        fout << "# This is also a comment\n";
        fout << "[Section1]\n";
        fout << "key1=value1\n";
        fout << "  ; inline comment after value\n";
        fout << "[Section2]\n";
        fout << "key2=value2\n";
    }
    
    IniParser ini;
    ASSERT_TRUE(ini.load(path));
    ASSERT_EQ(ini.get("Section1", "key1", ""), "value1");
    ASSERT_EQ(ini.get("Section2", "key2", ""), "value2");
    
    std::filesystem::remove(path);
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

    ASSERT_EQ(trim(QString("  hello  ")), QString("hello"));
    ASSERT_EQ(trimLeft(QString("  hello")), QString("hello"));
    ASSERT_EQ(trimRight(QString("hello  ")), QString("hello"));
    ASSERT_EQ(trim(QString()), QString());
    ASSERT_EQ(trim(QString("   ")), QString());
}

static void test_string_helper_split() {
    using namespace StringHelper;

    auto parts = split(QString("a,b,c"), QString(","));
    ASSERT_EQ(parts.size(), 3u);
    ASSERT_EQ(parts[0], "a");
    ASSERT_EQ(parts[1], "b");
    ASSERT_EQ(parts[2], "c");

    auto parts2 = split(QString("a::b::c"), QString("::"));
    ASSERT_EQ(parts2.size(), 3u);

    auto parts3 = split(QString("a,,b,c"), QString(","), true);
    ASSERT_EQ(parts3.size(), 3u);  // skip empty

    auto parts4 = split(QString("a,,b,c"), QString(","), false);
    ASSERT_EQ(parts4.size(), 4u);  // include empty
}

static void test_string_helper_replace() {
    using namespace StringHelper;

    ASSERT_EQ(replaceAll(QString("hello world"), QString("world"), QString("there")), QString("hello there"));
    ASSERT_EQ(replaceAll(QString("aaa"), QString("a"), QString("b")), QString("bbb"));
    ASSERT_EQ(replaceAll(QString("hello"), QString("x"), QString("y")), QString("hello"));  // no change
}

static void test_string_helper_starts_ends() {
    using namespace StringHelper;

    ASSERT_TRUE(startsWith(QString("hello world"), QString("hello")));
    ASSERT_FALSE(startsWith(QString("hello world"), QString("world")));
    ASSERT_TRUE(endsWith(QString("hello world"), QString("world")));
    ASSERT_FALSE(endsWith(QString("hello world"), QString("hello")));
}

static void test_string_helper_contains() {
    using namespace StringHelper;

    ASSERT_TRUE(contains(QString("hello world"), QString("world")));
    ASSERT_FALSE(contains(QString("hello world"), QString("foo")));
}

static void test_string_helper_to_int() {
    using namespace StringHelper;

    ASSERT_EQ(toInt(QString("42")), 42);
    ASSERT_EQ(toInt(QString("  42  ")), 42);
    ASSERT_EQ(toInt(QString("notanumber"), 99), 99);
    ASSERT_EQ(toInt(QString("3.14"), 0), 0);  // "3.14" not a valid integer for QString::toInt
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
    std::cout << "test_ini_parser_getBool_comprehensive... "; test_ini_parser_getBool_comprehensive(); std::cout << "OK\n";
    std::cout << "test_ini_parser_string_list... "; test_ini_parser_string_list(); std::cout << "OK\n";
    std::cout << "test_ini_parser_remove_section... "; test_ini_parser_remove_section(); std::cout << "OK\n";
    std::cout << "test_ini_parser_sections... "; test_ini_parser_sections(); std::cout << "OK\n";
    std::cout << "test_ini_parser_comments... "; test_ini_parser_comments(); std::cout << "OK\n";

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

