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

#define TEST(name) void name()
#define RUN(name) do { std::cout << #name "... "; name(); std::cout << "OK\n"; } while(0)
#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::cerr << "FAILED: " << #a << " != " << #b << "\n"; exit(1); } } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)

// ============================================================================
// FileManager singleton tests
// ============================================================================

TEST(FileManager_singleton) {
    FileManager& fm1 = FileManager::instance();
    FileManager& fm2 = FileManager::instance();
    ASSERT_TRUE(&fm1 == &fm2);
    RUN(FileManager_singleton);
}

TEST(FileManager_empty) {
    FileManager& fm = FileManager::instance();
    fm.closeFile(BUFFER_INVALID);
    ASSERT_EQ(fm.getBufferCount(), static_cast<size_t>(0));
    RUN(FileManager_empty);
}

TEST(FileManager_newFile) {
    FileManager& fm = FileManager::instance();
    BufferID id = fm.createNewFile();
    ASSERT_TRUE(id != BUFFER_INVALID);
    ASSERT_EQ(fm.getBufferCount(), static_cast<size_t>(1));
    fm.closeFile(id);
    RUN(FileManager_newFile);
}

TEST(FileManager_setBufferText) {
    FileManager& fm = FileManager::instance();
    BufferID id = fm.createNewFile();
    QString text = QStringLiteral("Hello, World!");
    ASSERT_TRUE(fm.setBufferText(id, text));
    ASSERT_EQ(fm.getBufferText(id), text);
    fm.closeFile(id);
    RUN(FileManager_setBufferText);
}

TEST(FileManager_setBufferPath) {
    FileManager& fm = FileManager::instance();
    BufferID id = fm.createNewFile();
    QString path = QStringLiteral("/tmp/test.txt");
    fm.setBufferPath(id, path);
    ASSERT_EQ(fm.getBufferPath(id), path);
    fm.closeFile(id);
    RUN(FileManager_setBufferPath);
}

TEST(FileManager_encoding) {
    FileManager& fm = FileManager::instance();
    BufferID id = fm.createNewFile();
    fm.setEncoding(id, EncodingType::UTF_8_BOM);
    ASSERT_EQ(fm.getEncoding(id), EncodingType::UTF_8_BOM);
    fm.closeFile(id);
    RUN(FileManager_encoding);
}

TEST(FileManager_closeFile_invalid) {
    FileManager& fm = FileManager::instance();
    fm.closeFile(BUFFER_INVALID);
    ASSERT_EQ(fm.getBufferCount(), static_cast<size_t>(0));
    RUN(FileManager_closeFile_invalid);
}

// ============================================================================
// Encoding detection tests
// ============================================================================

TEST(EncodingDetector_utf8) {
    std::string data = "Hello, world!";
    EncodingType enc = EncodingDetector::instance().detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_8);
    RUN(EncodingDetector_utf8);
}

TEST(EncodingDetector_utf8BOM) {
    std::string data = "\xEF\xBB\xBFHello";
    EncodingType enc = EncodingDetector::instance().detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_8_BOM);
    RUN(EncodingDetector_utf8BOM);
}

TEST(EncodingDetector_utf16LE) {
    std::string data = "\xFF\xFEH\x00e\x00l\x00l\x00o\x00";
    EncodingType enc = EncodingDetector::instance().detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_16_LE);
    RUN(EncodingDetector_utf16LE);
}

TEST(EncodingDetector_asciiSubset) {
    std::string data = "Plain text content.";
    EncodingType enc = EncodingDetector::instance().detect(data);
    ASSERT_EQ(enc, EncodingType::UTF_8);
    RUN(EncodingDetector_asciiSubset);
}

// ============================================================================
// Main
// ============================================================================

int main() {
    std::cout << "Running Buffer/FileManager tests...\n";

    FileManager_singleton();
    FileManager_empty();
    FileManager_newFile();
    FileManager_setBufferText();
    FileManager_setBufferPath();
    FileManager_encoding();
    FileManager_closeFile_invalid();

    EncodingDetector_utf8();
    EncodingDetector_utf8BOM();
    EncodingDetector_utf16LE();
    EncodingDetector_asciiSubset();

    std::cout << "\nAll tests passed.\n";
    return 0;
}
