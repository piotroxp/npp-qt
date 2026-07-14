// test_buffer.cpp - Unit tests for Buffer and FileManager
// Copyright (C) 2026 Agent Army
// GPL v3

#include "core/FileManager.h"
#include "core/Buffer.h"
#include "core/EncodingDetector.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <QDir>
#include <QFile>

#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::cerr << "FAILED: " << #a << " != " << #b << "\n"; exit(1); } } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)

static void test_file_manager_singleton() {
    FileManager& fm1 = FileManager::instance();
    FileManager& fm2 = FileManager::instance();
    ASSERT_TRUE(&fm1 == &fm2);
}

static void test_file_manager_empty() {
    FileManager& fm = FileManager::instance();
    fm.closeFile(BUFFER_INVALID);
    ASSERT_EQ(fm.getBufferCount(), static_cast<size_t>(0));
}

static void test_file_manager_new_file() {
    FileManager& fm = FileManager::instance();
    BufferID id = fm.createNewFile();
    ASSERT_TRUE(id != BUFFER_INVALID);
    ASSERT_EQ(fm.getBufferCount(), static_cast<size_t>(1));
    fm.closeFile(id);
}

static void test_file_manager_set_buffer_text() {
    FileManager& fm = FileManager::instance();
    BufferID id = fm.createNewFile();
    QString text = QStringLiteral("Hello, World!");
    ASSERT_TRUE(fm.setBufferText(id, text));
    ASSERT_EQ(fm.getBufferText(id), text);
    fm.closeFile(id);
}

static void test_file_manager_set_buffer_path() {
    FileManager& fm = FileManager::instance();
    BufferID id = fm.createNewFile();
    QString path = QStringLiteral("/tmp/test.txt");
    fm.setBufferPath(id, path);
    ASSERT_EQ(fm.getBufferPath(id), path);
    fm.closeFile(id);
}

static void test_file_manager_encoding() {
    FileManager& fm = FileManager::instance();
    BufferID id = fm.createNewFile();
    fm.setEncoding(id, EncodingType::UTF_8_BOM);
    ASSERT_EQ(fm.getEncoding(id), EncodingType::UTF_8_BOM);
    fm.closeFile(id);
}

static void test_file_manager_close_invalid() {
    FileManager& fm = FileManager::instance();
    fm.closeFile(BUFFER_INVALID);
    ASSERT_EQ(fm.getBufferCount(), static_cast<size_t>(0));
}

static void test_encoding_utf8() {
    std::string data = "Hello, world!";
    EncodingType enc = EncodingDetector::instance().detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_8);
}

static void test_encoding_utf8_bom() {
    std::string data = "\xEF\xBB\xBFHello";
    EncodingType enc = EncodingDetector::instance().detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_8_BOM);
}

static void test_encoding_utf16_le() {
    std::string data = "\xFF\xFEH\x00e\x00l\x00l\x00o\x00";
    EncodingType enc = EncodingDetector::instance().detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_16_LE);
}

static void test_encoding_ascii_subset() {
    std::string data = "Plain text content.";
    EncodingType enc = EncodingDetector::instance().detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_8);
}

int main() {
    std::cout << "Running Buffer/FileManager tests...\n";

    std::cout << "test_file_manager_singleton... ";
    test_file_manager_singleton();
    std::cout << "OK\n";

    std::cout << "test_file_manager_empty... ";
    test_file_manager_empty();
    std::cout << "OK\n";

    std::cout << "test_file_manager_new_file... ";
    test_file_manager_new_file();
    std::cout << "OK\n";

    std::cout << "test_file_manager_set_buffer_text... ";
    test_file_manager_set_buffer_text();
    std::cout << "OK\n";

    std::cout << "test_file_manager_set_buffer_path... ";
    test_file_manager_set_buffer_path();
    std::cout << "OK\n";

    std::cout << "test_file_manager_encoding... ";
    test_file_manager_encoding();
    std::cout << "OK\n";

    std::cout << "test_file_manager_close_invalid... ";
    test_file_manager_close_invalid();
    std::cout << "OK\n";

    std::cout << "test_encoding_utf8... ";
    test_encoding_utf8();
    std::cout << "OK\n";

    std::cout << "test_encoding_utf8_bom... ";
    test_encoding_utf8_bom();
    std::cout << "OK\n";

    std::cout << "test_encoding_utf16_le... ";
    test_encoding_utf16_le();
    std::cout << "OK\n";

    std::cout << "test_encoding_ascii_subset... ";
    test_encoding_ascii_subset();
    std::cout << "OK\n";

    std::cout << "\nAll tests passed.\n" << std::flush;
    return 0;
}

