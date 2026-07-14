// FileHelper.h - File system utilities for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <chrono>
#include <filesystem>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QDateTime>
#include "Types.h"
#include "Constants.h"

namespace FileHelper {

// ============================================================================
// File existence & type
// ============================================================================
bool exists(const std::string& path);
bool isFile(const std::string& path);
bool isDirectory(const std::string& path);
bool isReadable(const std::string& path);
bool isWritable(const std::string& path);

// ============================================================================
// File size
// ============================================================================
int64_t fileSize(const std::string& path);
bool isLargeFile(const std::string& path, int64_t threshold = LARGE_FILE_SIZE);

// ============================================================================
// Read / Write files
// ============================================================================
std::optional<std::string>  readFile(const std::string& path);
std::optional<std::wstring> readFileW(const std::string& path);
bool writeFile(const std::string& path, const void* data, size_t size);
bool writeFile(const std::string& path, const std::string& content);
bool writeFile(const std::string& path, const std::wstring& content);
bool appendFile(const std::string& path, const std::string& content);

// ============================================================================
// File operations
// ============================================================================
bool createDirectory(const std::string& path);
bool removeFile(const std::string& path);
bool removeDirectory(const std::string& path, bool recursive = false);
bool moveFile(const std::string& src, const std::string& dst, bool overwrite = false);
bool copyFile(const std::string& src, const std::string& dst, bool overwrite = false);
bool renameFile(const std::string& oldPath, const std::string& newPath);

// ============================================================================
// Path manipulation
// ============================================================================
std::string  absolutePath(const std::string& path);
std::string  normalize(const std::string& path);
std::string  parentPath(const std::string& path);
std::string  fileName(const std::string& path);
std::string  baseName(const std::string& path);
std::string  fileExtension(const std::string& path);
bool         setFileExtension(std::string& path, const std::string& ext);
bool         isAbsolutePath(const std::string& path);
std::string  resolvePath(const std::string& base, const std::string& relative);

// ============================================================================
// Directory listing
// ============================================================================
std::vector<std::string> listFiles(const std::string& dir, bool recursive = false);
std::vector<std::string> listFilesWithExt(const std::string& dir, const std::string& ext);
std::vector<std::string> listDirectories(const std::string& dir);

// ============================================================================
// Timestamps
// ============================================================================
std::chrono::system_clock::time_point lastModifiedTime(const std::string& path);
bool setLastModifiedTime(const std::string& path, const std::chrono::system_clock::time_point& t);

// ============================================================================
// Temporary files
// ============================================================================
std::string getTempDirectory();
std::string getTempFileName(const std::string& prefix = "npp_");
std::string getTempPath();

// ============================================================================
// App directories
// ============================================================================
std::string getAppDataDirectory();
std::string getConfigDirectory();
std::string getPluginDirectory();
std::string getUserDefinedLangsDirectory();
std::string getThemesDirectory();
std::string getSessionFilePath();
std::string getConfigFilePath();

// ============================================================================
// Encoding detection helpers
// ============================================================================
bool hasUtf8Bom(const std::string& path);
bool hasUtf16Bom(const std::string& path);
EncodingType detectEncoding(const std::string& path);

// ============================================================================
// Qt-style text/binary file operations
// ============================================================================
QString readTextFile(const QString& path, const QString& encoding = "UTF-8", QString* error = nullptr);
bool writeTextFile(const QString& path, const QString& content, const QString& encoding = "UTF-8", QString* error = nullptr);
QByteArray readBinaryFile(const QString& path, QString* error = nullptr);
bool writeBinaryFile(const QString& path, const QByteArray& data, QString* error = nullptr);

// ============================================================================
// File info helpers
// ============================================================================
qint64 getFileSize(const QString& path);
QDateTime getCreationTime(const QString& path);
QDateTime getModificationTime(const QString& path);
bool setModificationTime(const QString& path, const QDateTime& time);

// ============================================================================
// Qt-style temp file/directory
// ============================================================================
QString getTempFileName(const QString& prefix = "npp-");
QString getTempDir();

// ============================================================================
// Qt-style directory listing
// ============================================================================
QStringList listFiles(const QString& dir, const QString& filter = "*", bool recursive = false);
QStringList listDirs(const QString& dir, const QString& filter = "*", bool recursive = false);

// ============================================================================
// Recursive copy/remove
// ============================================================================
bool copyRecursive(const QString& src, const QString& dst, QString* error = nullptr);
bool removeRecursive(const QString& path, QString* error = nullptr);

// ============================================================================
// Hash computation
// ============================================================================
QString computeMD5(const QString& path);
QString computeSHA256(const QString& path);

// ============================================================================
// Binary file detection
// ============================================================================
bool isBinaryFile(const QString& path, int checkBytes = 8192);

// ============================================================================
// File watching
// ============================================================================
class FileWatcher {
public:
    virtual ~FileWatcher() = default;
    virtual void start(const std::string& path) = 0;
    virtual void stop() = 0;
    virtual bool hasChanged() const = 0;
    virtual std::string watchedPath() const = 0;
};

// ============================================================================
// Scintilla FileID (opaque type)
// ============================================================================
using ScintillaDocument = void*;
ScintillaDocument createDocument();
void releaseDocument(ScintillaDocument doc);

// ============================================================================
// Mapped file I/O (for large files)
// ============================================================================
class MemoryMappedFile {
public:
    MemoryMappedFile() = default;
    MemoryMappedFile(const std::string& path, bool readOnly = true);
    ~MemoryMappedFile();
    MemoryMappedFile(MemoryMappedFile&& other) noexcept;
    MemoryMappedFile& operator=(MemoryMappedFile&& other) noexcept;
    MemoryMappedFile(const MemoryMappedFile&) = delete;
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;

    bool isOpen() const { return _isOpen; }
    const char* data() const { return _data; }
    size_t size() const { return _size; }
    void flush();

private:
    bool _isOpen = false;
    bool _readOnly = true;
    char* _data = nullptr;
    size_t _size = 0;
    std::string _path;
    void* _fileHandle = nullptr;
    void* _mappingHandle = nullptr;
};

} // namespace FileHelper
