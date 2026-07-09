// test_buffer.cpp - Unit tests for Buffer and FileManager
// Copyright (C) 2026 Agent Army
// GPL v3

// Standalone test file - no Qt GUI dependency (uses <filesystem>)
// Compile with: g++ -std=c++17 -I../common -I../core test_buffer.cpp -o test_buffer

#include "core/FileManager.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#define TEST(name) void name()
#define RUN(name) do { std::cout << #name "... "; name(); std::cout << "OK\n"; } while(0)
#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::cerr << "FAILED: " << #a << " != " << #b << "\n"; exit(1); } } while(0)
#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)
#define ASSERT_FALSE(x) do { if (x) { std::cerr << "FAILED: " << #x << "\n"; exit(1); } } while(0)

// Helper to create a temporary test file
std::string createTempFile(const std::string& content) {
    static int counter = 0;
    std::string path = "/tmp/test_buffer_" + std::to_string(counter++) + ".txt";
    std::ofstream f(path);
    f << content;
    f.close();
    return path;
}

// Helper to cleanup temp files
void cleanupTempFile(const std::string& path) {
    std::filesystem::remove(path);
}

// ============================================================================
// Buffer Creation Tests
// ============================================================================
TEST(test_buffer_creation) {
    Buffer buf;
    ASSERT_EQ(buf._id, nullptr);
    ASSERT_TRUE(buf._filePath.empty());
    ASSERT_TRUE(buf._text.empty());
    ASSERT_EQ(buf._encoding, EncodingType::UTF_8);
    ASSERT_EQ(buf._eolType, EolType::EOL_LF);
    ASSERT_EQ(buf._langType, LangType::L_TEXT);
    ASSERT_FALSE(buf._modified);
    ASSERT_FALSE(buf._readOnly);
}

// ============================================================================
// Buffer Status Flags Tests
// ============================================================================
TEST(test_buffer_status_flags) {
    Buffer buf;

    // Initially regular
    ASSERT_TRUE((static_cast<uint8_t>(buf._status) & static_cast<uint8_t>(DocFileStatus::DOC_REGULAR)) != 0);

    // Set modified
    buf._modified = true;
    buf._status = DocFileStatus::DOC_MODIFIED;
    ASSERT_TRUE((static_cast<uint8_t>(buf._status) & static_cast<uint8_t>(DocFileStatus::DOC_MODIFIED)) != 0);

    // Set unnamed
    buf._status = DocFileStatus::DOC_UNNAMED;
    ASSERT_TRUE((static_cast<uint8_t>(buf._status) & static_cast<uint8_t>(DocFileStatus::DOC_UNNAMED)) != 0);
}

// ============================================================================
// BufferID Identity Tests
// ============================================================================
TEST(test_buffer_id_identity) {
    Buffer buf1;
    Buffer buf2;

    buf1._id = reinterpret_cast<BufferID>(&buf1);
    buf2._id = reinterpret_cast<BufferID>(&buf2);

    // Different buffers should have different IDs
    ASSERT_TRUE(buf1._id != buf2._id);
    ASSERT_TRUE(buf1._id == reinterpret_cast<BufferID>(&buf1));
}

TEST(test_buffer_id_invalid) {
    Buffer buf;
    ASSERT_TRUE(BUFFER_INVALID == nullptr);
    ASSERT_TRUE(buf._id == nullptr);
}

// ============================================================================
// FileManager Basic Tests
// ============================================================================
TEST(test_file_manager_creation) {
    FileManager fm;
    // Should start empty
    ASSERT_EQ(fm.getBufferCount(), 0u);
}

TEST(test_file_manager_create_buffer) {
    FileManager fm;
    BufferID id = fm.createBuffer();
    ASSERT_TRUE(id != nullptr);
    ASSERT_EQ(fm.getBufferCount(), 1u);
}

TEST(test_file_manager_create_new_file) {
    FileManager fm;
    BufferID id = fm.createNewFile();
    ASSERT_TRUE(id != nullptr);

    // Should have empty path for new file
    std::string path = fm.getBufferPath(id);
    ASSERT_TRUE(path.empty());
}

TEST(test_file_manager_set_get_text) {
    FileManager fm;
    BufferID id = fm.createNewFile();

    std::string text = "Hello, World!";
    ASSERT_TRUE(fm.setBufferText(id, text));
    ASSERT_EQ(fm.getBufferText(id), text);
}

TEST(test_file_manager_set_encoding) {
    FileManager fm;
    BufferID id = fm.createNewFile();

    ASSERT_TRUE(fm.setEncoding(id, EncodingType::UTF_8_BOM));
    ASSERT_EQ(fm.getEncoding(id), EncodingType::UTF_8_BOM);
}

TEST(test_file_manager_modified_flag) {
    FileManager fm;
    BufferID id = fm.createNewFile();

    // New buffer should not be modified
    ASSERT_FALSE(fm.isBufferModified(id));

    // Setting text should mark as modified
    fm.setBufferText(id, "New content");
    ASSERT_TRUE(fm.isBufferModified(id));
}

// ============================================================================
// File Loading Tests
// ============================================================================
TEST(test_file_manager_open_file) {
    std::string path = createTempFile("Test content\nLine 2\nLine 3");
    FileManager fm;

    BufferID id = fm.openFile(path, false);
    ASSERT_TRUE(id != nullptr);
    ASSERT_EQ(fm.getBufferCount(), 1u);

    std::string content = fm.getBufferText(id);
    ASSERT_TRUE(content.find("Test content") != std::string::npos);

    cleanupTempFile(path);
}

TEST(test_file_manager_get_file_name) {
    std::string path = createTempFile("Content");
    FileManager fm;

    BufferID id = fm.openFile(path, false);
    auto nameOpt = fm.getFileName(id);

    ASSERT_TRUE(nameOpt.has_value());
    ASSERT_TRUE(nameOpt->find("test_buffer_") != std::string::npos);

    cleanupTempFile(path);
}

TEST(test_file_manager_get_file_directory) {
    std::string path = createTempFile("Content");
    FileManager fm;

    BufferID id = fm.openFile(path, false);
    std::string filePath = fm.getBufferPath(id);

    ASSERT_TRUE(filePath.find("/tmp/") != std::string::npos);

    cleanupTempFile(path);
}

// ============================================================================
// Buffer Path Tests
// ============================================================================
TEST(test_file_manager_set_buffer_path) {
    FileManager fm;
    BufferID id = fm.createNewFile();

    std::string path = "/path/to/file.txt";
    ASSERT_TRUE(fm.setBufferPath(id, path));
    ASSERT_EQ(fm.getBufferPath(id), path);
}

// ============================================================================
// Active Buffer Tests
// ============================================================================
TEST(test_file_manager_active_buffer) {
    FileManager fm;

    BufferID id1 = fm.createNewFile();
    BufferID id2 = fm.createNewFile();

    // Set active buffer
    fm.setActiveBuffer(id1);
    ASSERT_EQ(fm.getActiveBuffer(), id1);

    fm.setActiveBuffer(id2);
    ASSERT_EQ(fm.getActiveBuffer(), id2);
}

TEST(test_file_manager_get_buffer_at) {
    FileManager fm;

    BufferID id1 = fm.createNewFile();
    BufferID id2 = fm.createNewFile();

    ASSERT_EQ(fm.getBufferAt(0), id1);
    ASSERT_EQ(fm.getBufferAt(1), id2);
}

// ============================================================================
// Close Buffer Tests
// ============================================================================
TEST(test_file_manager_close_file) {
    FileManager fm;

    BufferID id = fm.createNewFile();
    ASSERT_EQ(fm.getBufferCount(), 1u);

    ASSERT_TRUE(fm.closeFile(id));
    ASSERT_EQ(fm.getBufferCount(), 0u);
}

TEST(test_file_manager_close_all) {
    FileManager fm;

    fm.createNewFile();
    fm.createNewFile();
    fm.createNewFile();
    ASSERT_EQ(fm.getBufferCount(), 3u);

    ASSERT_TRUE(fm.closeAllFiles());
    ASSERT_EQ(fm.getBufferCount(), 0u);
}

// ============================================================================
// Duplicate Buffer Tests
// ============================================================================
TEST(test_file_manager_duplicate_buffer) {
    FileManager fm;

    BufferID original = fm.createNewFile();
    fm.setBufferText(original, "Original content");

    BufferID duplicate = fm.duplicateBuffer(original);
    ASSERT_TRUE(duplicate != nullptr);
    ASSERT_TRUE(duplicate != original);
    ASSERT_EQ(fm.getBufferCount(), 2u);

    // Content should be copied
    ASSERT_EQ(fm.getBufferText(duplicate), "Original content");
}

// ============================================================================
// File Extension Tests
// ============================================================================
TEST(test_file_manager_extension) {
    FileManager fm;

    ASSERT_EQ(fm.getFileExtension("/path/to/file.cpp"), "cpp");
    ASSERT_EQ(fm.getFileExtension("file.txt"), "txt");
    ASSERT_EQ(fm.getFileExtension("file"), "");
    ASSERT_EQ(fm.getFileExtension("/path/to/file.tar.gz"), "gz");  // Only last ext
}

TEST(test_file_manager_filename) {
    FileManager fm;

    ASSERT_EQ(fm.getFileName("/path/to/file.txt"), "file.txt");
    ASSERT_EQ(fm.getFileName("file.txt"), "file.txt");
    ASSERT_EQ(fm.getFileName("/path/to/"), "");
}

// ============================================================================
// File Existence Tests
// ============================================================================
TEST(test_file_exists) {
    FileManager fm;

    std::string path = createTempFile("Test");
    ASSERT_TRUE(fm.fileExists(path));
    ASSERT_FALSE(fm.fileExists("/nonexistent/file.txt"));
    ASSERT_FALSE(fm.isDirectory(path));

    cleanupTempFile(path);
}

TEST(test_file_directory) {
    FileManager fm;

    ASSERT_TRUE(fm.fileExists("/tmp"));
    ASSERT_TRUE(fm.isDirectory("/tmp"));
    ASSERT_FALSE(fm.fileExists("/nonexistent"));
}

// ============================================================================
// Main
// ============================================================================
int main() {
    std::cout << "=== Buffer/FileManager Unit Tests ===\n\n";

    std::cout << "Buffer Creation:\n";
    RUN(test_buffer_creation);
    RUN(test_buffer_status_flags);
    RUN(test_buffer_id_identity);
    RUN(test_buffer_id_invalid);

    std::cout << "\nFileManager Basic:\n";
    RUN(test_file_manager_creation);
    RUN(test_file_manager_create_buffer);
    RUN(test_file_manager_create_new_file);
    RUN(test_file_manager_set_get_text);
    RUN(test_file_manager_set_encoding);
    RUN(test_file_manager_modified_flag);

    std::cout << "\nFile Loading:\n";
    RUN(test_file_manager_open_file);
    RUN(test_file_manager_get_file_name);
    RUN(test_file_manager_get_file_directory);

    std::cout << "\nBuffer Paths:\n";
    RUN(test_file_manager_set_buffer_path);

    std::cout << "\nActive Buffer:\n";
    RUN(test_file_manager_active_buffer);
    RUN(test_file_manager_get_buffer_at);

    std::cout << "\nClose Operations:\n";
    RUN(test_file_manager_close_file);
    RUN(test_file_manager_close_all);

    std::cout << "\nDuplicate Buffer:\n";
    RUN(test_file_manager_duplicate_buffer);

    std::cout << "\nFile Extensions:\n";
    RUN(test_file_manager_extension);
    RUN(test_file_manager_filename);

    std::cout << "\nFile Existence:\n";
    RUN(test_file_exists);
    RUN(test_file_directory);

    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}
