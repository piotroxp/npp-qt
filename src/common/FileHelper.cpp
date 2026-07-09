// FileHelper.cpp - File system utilities implementation
#include "FileHelper.h"
#include "Constants.h"
#include "StringHelper.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cerrno>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/mman.h>
#include <fcntl.h>
#endif

namespace fs = std::filesystem;
using namespace std::chrono_literals;

namespace FileHelper {

// ============================================================================
// Existence & type
// ============================================================================
bool exists(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

bool isFile(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISREG(st.st_mode);
}

bool isDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

bool isReadable(const std::string& path) {
    return access(path.c_str(), R_OK) == 0;
}

bool isWritable(const std::string& path) {
    return access(path.c_str(), W_OK) == 0;
}

// ============================================================================
// File size
// ============================================================================
int64_t fileSize(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return -1;
    return static_cast<int64_t>(st.st_size);
}

bool isLargeFile(const std::string& path, int64_t threshold) {
    return fileSize(path) > threshold;
}

// ============================================================================
// Read / Write
// ============================================================================
std::optional<std::string> readFile(const std::string& path) {
    std::ifstream fin(path, std::ios::binary);
    if (!fin) return std::nullopt;
    std::ostringstream oss;
    oss << fin.rdbuf();
    return oss.str();
}

std::optional<std::wstring> readFileW(const std::string& path) {
    std::string content = readFile(path).value_or("");
    return StringHelper::toWStr(content);
}

bool writeFile(const std::string& path, const void* data, size_t size) {
    std::ofstream fout(path, std::ios::binary);
    if (!fout) return false;
    fout.write(static_cast<const char*>(data), size);
    return fout.good();
}

bool writeFile(const std::string& path, const std::string& content) {
    return writeFile(path, content.data(), content.size());
}

bool writeFile(const std::string& path, const std::wstring& content) {
    std::string utf8 = StringHelper::toUtf8(content);
    return writeFile(path, utf8);
}

bool appendFile(const std::string& path, const std::string& content) {
    std::ofstream fout(path, std::ios::binary | std::ios::app);
    if (!fout) return false;
    fout.write(content.data(), content.size());
    return fout.good();
}

// ============================================================================
// File operations
// ============================================================================
bool createDirectory(const std::string& path) {
    return fs::create_directories(fs::path(path));
}

bool removeFile(const std::string& path) {
    return ::remove(path.c_str()) == 0 || errno == ENOENT;
}

bool removeDirectory(const std::string& path, bool recursive) {
    if (recursive) {
        return fs::remove_all(fs::path(path)) > 0;
    }
    return fs::remove(fs::path(path));
}

bool moveFile(const std::string& src, const std::string& dst, bool overwrite) {
    if (!overwrite && exists(dst)) return false;
    std::error_code ec;
    fs::rename(src, dst, ec);
    if (!ec) return true;
    return copyFile(src, dst, overwrite) && removeFile(src);
}

bool copyFile(const std::string& src, const std::string& dst, bool overwrite) {
    if (!overwrite && exists(dst)) return false;
    std::ifstream fin(src, std::ios::binary);
    if (!fin) return false;
    std::ofstream fout(dst, std::ios::binary);
    if (!fout) return false;
    fout << fin.rdbuf();
    return fout.good();
}

bool renameFile(const std::string& oldPath, const std::string& newPath) {
    return ::rename(oldPath.c_str(), newPath.c_str()) == 0;
}

// ============================================================================
// Path manipulation
// ============================================================================
std::string absolutePath(const std::string& path) {
    return fs::absolute(path).string();
}

std::string normalize(const std::string& path) {
    return fs::path(path).lexically_normal().string();
}

std::string parentPath(const std::string& path) {
    return fs::path(path).parent_path().string();
}

std::string fileName(const std::string& path) {
    return fs::path(path).filename().string();
}

std::string baseName(const std::string& path) {
    auto name = fs::path(path).filename().string();
    auto dotPos = name.rfind('.');
    if (dotPos == std::string::npos || dotPos == 0) return name;
    return name.substr(0, dotPos);
}

std::string fileExtension(const std::string& path) {
    return fs::path(path).extension().string();
}

bool setFileExtension(std::string& path, const std::string& ext) {
    auto p = fs::path(path);
    p.replace_extension(ext);
    path = p.string();
    return true;
}

bool isAbsolutePath(const std::string& path) {
    return fs::path(path).is_absolute();
}

std::string resolvePath(const std::string& base, const std::string& relative) {
    if (isAbsolutePath(relative)) return normalize(relative);
    return normalize((fs::path(base) / relative).string());
}

// ============================================================================
// Directory listing
// ============================================================================
std::vector<std::string> listFiles(const std::string& dir, bool recursive) {
    std::vector<std::string> result;
    if (!isDirectory(dir)) return result;
    if (recursive) {
        for (auto& entry : fs::recursive_directory_iterator(fs::path(dir))) {
            if (entry.is_regular_file())
                result.push_back(entry.path().string());
        }
    } else {
        for (auto& entry : fs::directory_iterator(fs::path(dir))) {
            if (entry.is_regular_file())
                result.push_back(entry.path().string());
        }
    }
    return result;
}

std::vector<std::string> listFilesWithExt(const std::string& dir, const std::string& ext) {
    auto all = listFiles(dir, false);
    std::vector<std::string> result;
    for (const auto& f : all) {
        if (fileExtension(f) == ext) result.push_back(f);
    }
    return result;
}

std::vector<std::string> listDirectories(const std::string& dir) {
    std::vector<std::string> result;
    if (!isDirectory(dir)) return result;
    for (auto& entry : fs::directory_iterator(fs::path(dir))) {
        if (entry.is_directory())
            result.push_back(entry.path().string());
    }
    return result;
}

// ============================================================================
// Timestamps
// ============================================================================
std::chrono::system_clock::time_point lastModifiedTime(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return {};
    return std::chrono::system_clock::from_time_t(st.st_mtime);
}

bool setLastModifiedTime(const std::string& path,
                                     const std::chrono::system_clock::time_point& t) {
    (void)path; (void)t;
    // Platform-specific - would need utimes/mtime syscall
    return false;
}

// ============================================================================
// Temporary files
// ============================================================================
std::string getTempDirectory() {
#if defined(_WIN32) || defined(_WIN64)
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    return std::string(tempPath);
#else
    const char* tmp = getenv("TMPDIR");
    if (!tmp) tmp = getenv("TMP");
    if (!tmp) tmp = "/tmp";
    return std::string(tmp);
#endif
}

std::string getTempFileName(const std::string& prefix) {
    auto tmpDir = getTempDirectory();
    std::string templatePath = tmpDir + "/" + prefix + "XXXXXX";
    char buf[1024];
    strcpy(buf, templatePath.c_str());
#if defined(_WIN32) || defined(_WIN64)
    _mktemp_s(buf);
#else
    mkstemp(buf);
#endif
    return std::string(buf);
}

std::string getTempPath() {
    return getTempFileName("npp_XXXXXX");
}

// ============================================================================
// App directories
// ============================================================================
std::string getAppDataDirectory() {
    const char* home = getenv("HOME");
    std::string base = home ? home : ".";
    return base + "/" + APP_DATA_DIR;
}

std::string getConfigDirectory() {
    return getAppDataDirectory();
}

std::string getPluginDirectory() {
    return getAppDataDirectory() + "/" + PLUGIN_DIR;
}

std::string getUserDefinedLangsDirectory() {
    return getAppDataDirectory() + "/" + USER_CONFIG_DIR;
}

std::string getThemesDirectory() {
    return getAppDataDirectory() + "/" + THEMES_DIR;
}

std::string getSessionFilePath() {
    return getConfigDirectory() + "/" + SESSION_FILENAME;
}

std::string getConfigFilePath() {
    return getConfigDirectory() + "/" + CONFIG_FILENAME;
}

// ============================================================================
// Encoding detection
// ============================================================================
bool hasUtf8Bom(const std::string& path) {
    auto content = readFile(path);
    if (!content || content->size() < 3) return false;
    return content->substr(0, 3) == "\xEF\xBB\xBF";
}

bool hasUtf16Bom(const std::string& path) {
    auto content = readFile(path);
    if (!content || content->size() < 2) return false;
    return content->substr(0, 2) == "\xFF\xFE" || content->substr(0, 2) == "\xFE\xFF";
}

EncodingType detectEncoding(const std::string& path) {
    auto content = readFile(path);
    if (!content) return EncodingType::ANSI;
    if (hasUtf8Bom(path)) return EncodingType::UTF_8_BOM;
    if (hasUtf16Bom(path)) {
        if (content->size() >= 2 && content->substr(0, 2) == "\xFF\xFE") return EncodingType::UTF_16_LE;
        if (content->size() >= 2 && content->substr(0, 2) == "\xFE\xFF") return EncodingType::UTF_16_BE;
    }
    // Try to detect UTF-8 without BOM
    bool looksLikeUtf8 = true;
    for (size_t i = 0; i < content->size() && looksLikeUtf8; ++i) {
        unsigned char c = (*content)[i];
        if (c >= 0x80) {
            // Check for valid UTF-8 sequence
            int cont = 0;
            if ((c & 0xE0) == 0xC0) cont = 1;
            else if ((c & 0xF0) == 0xE0) cont = 2;
            else if ((c & 0xF8) == 0xF0) cont = 3;
            else looksLikeUtf8 = false;
            while (cont > 0 && ++i < content->size()) {
                if (((*content)[i] & 0xC0) != 0x80) { looksLikeUtf8 = false; break; }
                --cont;
            }
        }
    }
    if (looksLikeUtf8) return EncodingType::UTF_8;
    return EncodingType::ANSI;
}

// ============================================================================
// Scintilla document
// ============================================================================
ScintillaDocument createDocument() {
    return nullptr;  // Will be implemented via ScintillaEditor
}

void releaseDocument(ScintillaDocument doc) {
    (void)doc;
}

// ============================================================================
// Memory mapped file
// ============================================================================
MemoryMappedFile::MemoryMappedFile(const std::string& path, bool readOnly) : _path(path), _readOnly(readOnly) {
#if defined(_WIN32) || defined(_WIN64)
    _fileHandle = CreateFileA(path.c_str(),
        readOnly ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE),
        FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (_fileHandle == INVALID_HANDLE_VALUE) return;
    _size = GetFileSize(_fileHandle, nullptr);
    _mappingHandle = CreateFileMapping(_fileHandle, nullptr,
        readOnly ? PAGE_READONLY : PAGE_READWRITE, 0, 0, nullptr);
    if (!_mappingHandle) return;
    _data = static_cast<char*>(MapViewOfFile(_mappingHandle,
        readOnly ? FILE_MAP_READ : FILE_MAP_WRITE, 0, 0, 0));
#else
    int flags = readOnly ? O_RDONLY : O_RDWR;
    int fd = open(path.c_str(), flags);
    if (fd < 0) return;
    struct stat st;
    if (fstat(fd, &st) != 0) { close(fd); return; }
    _size = st.st_size;
    _data = static_cast<char*>(mmap(nullptr, _size, readOnly ? PROT_READ : PROT_READ | PROT_WRITE,
        MAP_PRIVATE, fd, 0));
    close(fd);
#endif
    _isOpen = true;
}

MemoryMappedFile::~MemoryMappedFile() {
#if defined(_WIN32) || defined(_WIN64)
    if (_data) UnmapViewOfFile(_data);
    if (_mappingHandle) CloseHandle(_mappingHandle);
    if (_fileHandle != INVALID_HANDLE_VALUE) CloseHandle(_fileHandle);
#else
    if (_data) munmap(_data, _size);
#endif
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile&& other) noexcept {
    _isOpen = other._isOpen;
    _readOnly = other._readOnly;
    _data = other._data;
    _size = other._size;
    _path = other._path;
    other._isOpen = false;
    other._data = nullptr;
}

MemoryMappedFile& MemoryMappedFile::operator=(MemoryMappedFile&& other) noexcept {
    if (this != &other) {
        this->~MemoryMappedFile();
        _isOpen = other._isOpen;
        _readOnly = other._readOnly;
        _data = other._data;
        _size = other._size;
        _path = other._path;
        other._isOpen = false;
        other._data = nullptr;
    }
    return *this;
}

void MemoryMappedFile::flush() {
#if defined(_WIN32) || defined(_WIN64)
    if (_data) FlushViewOfFile(_data, 0);
#else
    if (_data) msync(_data, _size, MS_SYNC);
#endif
}

} // namespace FileHelper
