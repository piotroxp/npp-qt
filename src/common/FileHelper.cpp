// FileHelper.cpp - File system utilities implementation
#include "FileHelper.h"
#include "Constants.h"
#include "StringHelper.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDebug>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cerrno>
#include <sys/mman.h>
#include <fcntl.h>
#include <utime.h>

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
    return false;
}

// ============================================================================
// Temporary files
// ============================================================================
std::string getTempDirectory() {
    const char* tmp = getenv("TMPDIR");
    if (!tmp) tmp = getenv("TMP");
    if (!tmp) tmp = "/tmp";
    return std::string(tmp);
}

std::string getTempFileName(const std::string& prefix) {
    auto tmpDir = getTempDirectory();
    std::string templatePath = tmpDir + "/" + prefix + "XXXXXX";
    char buf[1024];
    strcpy(buf, templatePath.c_str());
    if (mkstemp(buf) == -1) return {};
    return std::string(buf);
}

std::string getTempPath() {
    return getTempFileName(std::string("npp_XXXXXX"));
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
    bool looksLikeUtf8 = true;
    for (size_t i = 0; i < content->size() && looksLikeUtf8; ++i) {
        unsigned char c = (*content)[i];
        if (c >= 0x80) {
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
    return nullptr;
}

void releaseDocument(ScintillaDocument doc) {
    (void)doc;
}

// ============================================================================
// Memory mapped file
// ============================================================================
MemoryMappedFile::MemoryMappedFile(const std::string& path, bool readOnly) : _readOnly(readOnly), _path(path) {
    int flags = readOnly ? O_RDONLY : O_RDWR;
    int fd = open(path.c_str(), flags);
    if (fd < 0) return;
    struct stat st;
    if (fstat(fd, &st) != 0) { close(fd); return; }
    _size = st.st_size;
    _data = static_cast<char*>(mmap(nullptr, _size, readOnly ? PROT_READ : PROT_READ | PROT_WRITE,
        MAP_PRIVATE, fd, 0));
    close(fd);
    _isOpen = (_data != MAP_FAILED);
}

MemoryMappedFile::~MemoryMappedFile() {
    if (_data && _data != MAP_FAILED) munmap(_data, _size);
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
    if (_data && _data != MAP_FAILED) msync(_data, _size, MS_SYNC);
}

// ============================================================================
// Qt-style text/binary file operations
// ============================================================================
QString readTextFile(const QString& path, const QString& encoding, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error) *error = QString("Failed to open file for reading: %1").arg(file.errorString());
        return QString();
    }
    QTextStream stream(&file);
    Q_UNUSED(encoding);
    QString content = stream.readAll();
    file.close();
    return content;
}

bool writeTextFile(const QString& path, const QString& content, const QString& encoding, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (error) *error = QString("Failed to open file for writing: %1").arg(file.errorString());
        return false;
    }
    QTextStream stream(&file);
    Q_UNUSED(encoding);
    stream << content;
    stream.flush();
    bool success = file.flush();
    if (!success && error) {
        *error = QString("Failed to write file: %1").arg(file.errorString());
    }
    file.close();
    return success;
}

QByteArray readBinaryFile(const QString& path, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) *error = QString("Failed to open file for reading: %1").arg(file.errorString());
        return QByteArray();
    }
    QByteArray data = file.readAll();
    file.close();
    return data;
}

bool writeBinaryFile(const QString& path, const QByteArray& data, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        if (error) *error = QString("Failed to open file for writing: %1").arg(file.errorString());
        return false;
    }
    qint64 written = file.write(data);
    if (written != data.size()) {
        if (error) *error = QString("Failed to write complete file: %1").arg(file.errorString());
        file.close();
        return false;
    }
    file.close();
    return true;
}

// ============================================================================
// File info helpers
// ============================================================================
qint64 getFileSize(const QString& path) {
    QFileInfo info(path);
    return info.size();
}

QDateTime getCreationTime(const QString& path) {
    QFileInfo info(path);
    return info.birthTime();
}

QDateTime getModificationTime(const QString& path) {
    QFileInfo info(path);
    return info.lastModified();
}

bool setModificationTime(const QString& path, const QDateTime& time) {
    QFile file(path);
    if (!file.exists()) return false;
    return file.setFileTime(time, QFileDevice::FileTime::FileModificationTime);
}

// ============================================================================
// Qt-style temp file/directory
// ============================================================================
QString getTempFileName(const QString& prefix) {
    QString templateName = QDir::temp().filePath(prefix + "XXXXXX");
    QByteArray tpl = templateName.toLocal8Bit();
    int fd = mkstemp(tpl.data());
    if (fd < 0) return QString();
    ::close(fd);
    const QString resultPath = QString::fromLocal8Bit(tpl);
    ::remove(tpl.constData());
    return resultPath;
}

QString getTempDir() {
    return QDir::tempPath();
}

// ============================================================================
// Qt-style directory listing
// ============================================================================
QStringList listFiles(const QString& dir, const QString& filter, bool recursive) {
    QStringList result;
    if (!QDir(dir).exists()) return result;

    QDirIterator::IteratorFlags flags = recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    QDirIterator it(dir, QStringList() << filter, QDir::Files, flags);
    while (it.hasNext()) {
        result.append(it.next());
    }
    return result;
}

QStringList listDirs(const QString& dir, const QString& filter, bool recursive) {
    QStringList result;
    if (!QDir(dir).exists()) return result;

    QDirIterator::IteratorFlags flags = recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    QDirIterator it(dir, QStringList() << filter, QDir::Dirs | QDir::NoDotAndDotDot, flags);
    while (it.hasNext()) {
        result.append(it.next());
    }
    return result;
}

// ============================================================================
// Recursive copy/remove
// ============================================================================
bool copyRecursive(const QString& src, const QString& dst, QString* error) {
    QFileInfo srcInfo(src);
    if (srcInfo.isDir()) {
        QDir dstDir(dst);
        if (!dstDir.exists()) {
            if (!dstDir.mkpath(".")) {
                if (error) *error = QString("Failed to create directory: %1").arg(dst);
                return false;
            }
        }
        QDir srcDir(src);
        for (const QFileInfo& info : srcDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString srcPath = info.absoluteFilePath();
            QString dstPath = dstDir.filePath(info.fileName());
            if (info.isDir()) {
                if (!copyRecursive(srcPath, dstPath, error)) return false;
            } else {
                if (!QFile::copy(srcPath, dstPath)) {
                    if (error) *error = QString("Failed to copy file: %1 -> %2").arg(srcPath, dstPath);
                    return false;
                }
            }
        }
        return true;
    } else {
        if (!QFile::copy(src, dst)) {
            if (error) *error = QString("Failed to copy file: %1 -> %2").arg(src, dst);
            return false;
        }
        return true;
    }
}

bool removeRecursive(const QString& path, QString* error) {
    QFileInfo info(path);
    if (info.isDir()) {
        QDir dir(path);
        for (const QFileInfo& entry : dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString entryPath = entry.absoluteFilePath();
            if (entry.isDir()) {
                if (!removeRecursive(entryPath, error)) return false;
            } else {
                if (!QFile::remove(entryPath)) {
                    if (error) *error = QString("Failed to remove file: %1").arg(entryPath);
                    return false;
                }
            }
        }
        if (!dir.rmdir(path)) {
            if (error) *error = QString("Failed to remove directory: %1").arg(path);
            return false;
        }
        return true;
    } else {
        if (!QFile::remove(path)) {
            if (error) *error = QString("Failed to remove file: %1").arg(path);
            return false;
        }
        return true;
    }
}

// ============================================================================
// Hash computation
// ============================================================================
QString computeMD5(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return QString();
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(&file);
    file.close();
    return QString::fromLatin1(hash.result().toHex());
}

QString computeSHA256(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return QString();
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(&file);
    file.close();
    return QString::fromLatin1(hash.result().toHex());
}

// ============================================================================
// Binary file detection
// ============================================================================
bool isBinaryFile(const QString& path, int checkBytes) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray data = file.read(checkBytes);
    file.close();

    for (char byte : data) {
        unsigned char c = static_cast<unsigned char>(byte);
        if (c == 0 || (c < 32 && c != 9 && c != 10 && c != 13)) {
            return true;
        }
    }
    return false;
}

} // namespace FileHelper
