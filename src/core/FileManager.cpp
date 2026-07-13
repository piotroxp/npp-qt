// FileManager.cpp - File I/O and buffer management implementation
// Copyright (C) 2026 Agent Army
// Copyright (C) 2021 Don HO <don.h@free.fr>
// GPL v3

#include "FileManager.h"
#include "Buffer.h"
#include "EncodingDetector.h"
#include "LanguageManager.h"
#include "common/Util.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QSaveFile>
#include <QApplication>

// ============================================================================
// Construction / Lifecycle
// ============================================================================

FileManager::FileManager()
    : QObject()
{
    _fileWatcher = new QFileSystemWatcher(this);

    connect(_fileWatcher, &QFileSystemWatcher::fileChanged,
            this, [this](const QString& path) {
        emit fileSystemChanged(path);
    });
}

FileManager::~FileManager() {
    // Clean up all buffers
    for (Buffer* buf : _buffers) {
        delete buf;
    }
    _buffers.clear();
}

// ============================================================================
// File I/O
// ============================================================================

// Helper: convert QByteArray (UTF-16 raw bytes) to QString
static QString decodeUtf16(const QByteArray& data, bool littleEndian) {
    QString result;
    result.reserve(data.size() / 2);
    for (int i = 0; i + 1 < data.size(); i += 2) {
        ushort code;
        if (littleEndian) {
            code = static_cast<uchar>(data[i]) | (static_cast<uchar>(data[i + 1]) << 8);
        } else {
            code = (static_cast<uchar>(data[i]) << 8) | static_cast<uchar>(data[i + 1]);
        }
        result.append(QChar(code));
    }
    return result;
}

// Helper: convert QString to QByteArray (raw UTF-16 bytes)
static QByteArray encodeUtf16(const QString& str, bool littleEndian) {
    QByteArray result;
    result.reserve(str.length() * 2);
    for (QChar c : str) {
        ushort code = c.unicode();
        if (littleEndian) {
            result.append(static_cast<char>(code & 0xFF));
            result.append(static_cast<char>((code >> 8) & 0xFF));
        } else {
            result.append(static_cast<char>((code >> 8) & 0xFF));
            result.append(static_cast<char>(code & 0xFF));
        }
    }
    return result;
}

bool FileManager::loadFile(const QString& path, QString& outContent, EncodingType encoding) {
    QFile file(path);
    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    file.close();

    // Strip BOM if present
    QByteArray content = data;
    if (content.size() >= 3 && (uchar)content[0] == 0xEF && (uchar)content[1] == 0xBB && (uchar)content[2] == 0xBF)
        content = content.mid(3);
    if (content.size() >= 2) {
        if ((uchar)content[0] == 0xFF && (uchar)content[1] == 0xFE) content = content.mid(2);
        if ((uchar)content[0] == 0xFE && (uchar)content[1] == 0xFF) content = content.mid(2);
    }

    // Decode based on encoding
    switch (encoding) {
        case EncodingType::UTF_8:
        case EncodingType::UTF_8_BOM:
            outContent = QString::fromUtf8(content);
            break;
        case EncodingType::UTF_16_LE:
            outContent = decodeUtf16(content, true);
            break;
        case EncodingType::UTF_16_BE:
            outContent = decodeUtf16(content, false);
            break;
        default: {
            // Try UTF-8 first, fall back to locale
            if (EncodingDetector::instance().isValidUtf8(content.toStdString()))
                outContent = QString::fromUtf8(content);
            else
                outContent = QString::fromLocal8Bit(content);
            break;
        }
    }

    return true;
}

bool FileManager::saveFile(const QString& path, const QString& content, EncodingType encoding, EolType eol) {
    // Convert EOL first
    QString processed = content;
    if (eol == EolType::EOL_CRLF) processed.replace('\n', "\r\n");
    else if (eol == EolType::EOL_CR) processed.replace('\n', "\r");

    QByteArray bytes;
    switch (encoding) {
        case EncodingType::UTF_8:
            bytes = processed.toUtf8();
            break;
        case EncodingType::UTF_8_BOM:
            bytes = QByteArray("\xEF\xBB\xBF", 3) + processed.toUtf8();
            break;
        case EncodingType::UTF_16_LE:
            bytes = encodeUtf16(processed, true);
            break;
        case EncodingType::UTF_16_BE:
            bytes = encodeUtf16(processed, false);
            break;
        default:
            bytes = processed.toLocal8Bit();
            break;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    qint64 written = file.write(bytes);
    file.close();

    return written == bytes.size();
}

bool FileManager::deleteFile(const QString& path) {
    return QFile::moveToTrash(path);
}

bool FileManager::moveFile(const QString& src, const QString& dst) {
    QFile file(src);
    if (!file.exists())
        return false;

    // Ensure destination directory exists
    QFileInfo dstInfo(dst);
    QDir dstDir = dstInfo.dir();
    if (!dstDir.exists()) {
        if (!dstDir.mkpath("."))
            return false;
    }

    // Use QFile::rename for cross-filesystem moves
    if (file.rename(dst)) {
        // Update file watcher
        auto it = _watchDescriptors.find(src);
        if (it != _watchDescriptors.end()) {
            int desc = it->second;
            _watchDescriptors.erase(it);
            _watchDescriptors[dst] = desc;

            if (_fileWatcher) {
                _fileWatcher->removePath(src);
                _fileWatcher->addPath(dst);
            }
        }
        return true;
    }

    // Fallback: copy and delete
    if (QFile::copy(src, dst)) {
        file.remove();
        return true;
    }

    return false;
}

bool FileManager::createEmptyFile(const QString& path) {
    QFile file(path);
    if (file.exists())
        return true;

    // Ensure directory exists
    QFileInfo info(path);
    QDir dir = info.dir();
    if (!dir.exists()) {
        if (!dir.mkpath("."))
            return false;
    }

    return file.open(QIODevice::WriteOnly);
}

// ============================================================================
// File Watching
// ============================================================================

bool FileManager::watchFile(const QString& path) {
    if (_watchDescriptors.contains(path))
        return true;

    if (_fileWatcher && _fileWatcher->addPath(path)) {
        _watchDescriptors[path] = 1;
        return true;
    }
    return false;
}

void FileManager::unwatchFile(const QString& path) {
    _watchDescriptors.erase(path);
    if (_fileWatcher) {
        _fileWatcher->removePath(path);
    }
}

void FileManager::unwatchAll() {
    _watchDescriptors.clear();
    if (_fileWatcher) {
        _fileWatcher->removePaths(_fileWatcher->files());
        _fileWatcher->removePaths(_fileWatcher->directories());
    }
}

bool FileManager::isWatched(const QString& path) const {
    return _watchDescriptors.find(path) != _watchDescriptors.end();
}

QStringList FileManager::getWatchedFiles() const {
    QStringList files;
    for (auto& [f, _] : _watchDescriptors) {
        files.push_back(f);
    }
    return files;
}

// ============================================================================
// File Utilities
// ============================================================================

QString FileManager::getFileExtension(const QString& path) const {
    return QFileInfo(path).suffix();
}

QString FileManager::getFileName(const QString& path) const {
    return QFileInfo(path).fileName();
}

QString FileManager::getFileDirectory(const QString& path) const {
    return QFileInfo(path).path();
}

bool FileManager::fileExists(const QString& path) const {
    return QFileInfo(path).exists() && QFileInfo(path).isFile();
}

bool FileManager::isDirectory(const QString& path) const {
    return QFileInfo(path).isDir();
}

int64_t FileManager::getFileSize(const QString& path) const {
    QFileInfo info(path);
    if (info.exists())
        return info.size();
    return -1;
}

// ============================================================================
// Buffer Management
// ============================================================================

BufferID FileManager::createBuffer(const QString& fileName, bool isLargeFile) {
    Buffer* buf = new Buffer(this, _nextBufferId, fileName, isLargeFile);
    _buffers.push_back(buf);
    ++_nextBufferId;
    emit bufferCreated(buf->getID());
    return buf->getID();
}

Buffer* FileManager::bufferFromId(BufferID id) const {
    for (Buffer* buf : _buffers) {
        if (buf->getID() == id)
            return buf;
    }
    return nullptr;
}

BufferID FileManager::openFile(const QString& path, bool readOnly) {
    Buffer* buf = new Buffer(this, _nextBufferId, path, false);
    ++_nextBufferId;

    // Read raw bytes for encoding detection
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        delete buf;
        return nullptr;
    }
    QFileInfo fileInfo(path);
    qint64 fileSize = fileInfo.size();
    QByteArray raw = file.readAll();
    file.close();

    // Large file handling: partial load for very large files (>100MB)
    constexpr qint64 VERY_LARGE = 100 * 1024 * 1024; // 100MB
    if (fileSize > VERY_LARGE) {
        // Partial load: read first 10MB only
        QFile f(path);
        if (f.open(QIODevice::ReadOnly)) {
            QByteArray chunk = f.read(10 * 1024 * 1024);
            f.close();
            QString text = QString::fromUtf8(chunk);
            _bufferText[buf->getID()] = text;
            buf->setDocumentLength(text.length());
            buf->setPartialLoad(true);
            buf->setTotalFileSize(fileSize);
            buf->setEncoding(EncodingType::UTF_8);
            buf->setEolFormat(EolType::EOL_LF);
            buf->setLangType(LangType::L_TEXT);
            buf->setFileName(path);
            if (readOnly) buf->setFileReadOnly(true);
            if (!readOnly) watchFile(path);
            _buffers.push_back(buf);
            emit bufferCreated(buf->getID());
            emit fileLoaded(true, buf->getID(), QString());
            return buf->getID();
        }
    }

    // Detect encoding from BOM + content
    EncodingType detectedEnc = EncodingDetector::instance().detect(raw.toStdString());
    buf->setEncoding(detectedEnc);

    // Decode content using detected encoding
    QString content;
    loadFile(path, content, detectedEnc);

    // Detect EOL from decoded content
    EolType eol = EolType::EOL_LF;
    if (content.contains("\r\n")) eol = EolType::EOL_CRLF;
    else if (content.contains('\r')) eol = EolType::EOL_CR;
    buf->setEolFormat(eol);

    buf->setDocumentLength(content.length());

    // Detect language from extension
    QString ext = getFileExtension(path).toLower();
    LangType lang = LanguageManager::detect(ext.toStdString());
    buf->setLangType(lang);

    if (readOnly) {
        buf->setFileReadOnly(true);
    }

    // Set full path (this triggers language detection from filename)
    buf->setFileName(path);

    // Start watching for changes
    if (!readOnly) {
        watchFile(path);
    }

    _buffers.push_back(buf);
    emit bufferCreated(buf->getID());
    emit fileLoaded(true, buf->getID(), QString());

    return buf->getID();
}

BufferID FileManager::createNewFile() {
    static int untitledCounter = 1;
    QString name = QString("new %1").arg(untitledCounter++);
    BufferID id = createBuffer(name);
    setActiveBuffer(id);

    Buffer* buf = bufferFromId(id);
    if (buf) {
        buf->setDocumentLength(0);
        buf->setDirty(false);
    }

    return id;
}

BufferID FileManager::duplicateBuffer(BufferID buffer) {
    Buffer* src = bufferFromId(buffer);
    if (!src)
        return BUFFER_INVALID;

    Buffer* dup = new Buffer(this, _nextBufferId, src->getFileName(), src->isLargeFile());
    ++_nextBufferId;

    dup->setLangType(src->getLangType());
    dup->setEncoding(src->getEncoding());
    dup->setEolFormat(src->getEolFormat());
    dup->setDocumentLength(src->getDocumentLength());
    dup->setDirty(true);  // Mark as dirty since it's a new copy

    _buffers.push_back(dup);
    emit bufferCreated(dup->getID());

    return dup->getID();
}

QString FileManager::getFileName(BufferID buffer) const {
    Buffer* buf = bufferFromId(buffer);
    if (!buf)
        return QString();
    return QFileInfo(buf->getFullPathName()).fileName();
}

BufferID FileManager::getActiveBuffer() const {
    return _activeBuffer;
}

BufferID FileManager::getBufferAt(int index, int) const {
    if (index < 0 || index >= static_cast<int>(_buffers.size()))
        return BUFFER_INVALID;
    return _buffers[index]->getID();
}

size_t FileManager::getBufferCount() const {
    return _buffers.size();
}

int FileManager::getBufferIndexByID(BufferID id) const {
    for (size_t i = 0; i < _buffers.size(); ++i) {
        if (_buffers[i]->getID() == id)
            return static_cast<int>(i);
    }
    return -1;
}

Buffer* FileManager::getBufferByIndex(size_t index) const {
    if (index >= _buffers.size())
        return nullptr;
    return _buffers[index];
}

size_t FileManager::getNbDirtyBuffers() const {
    size_t count = 0;
    for (Buffer* buf : _buffers) {
        if (buf->isDirty())
            ++count;
    }
    return count;
}

void FileManager::setActiveBuffer(BufferID buffer) {
    _activeBuffer = buffer;
}

bool FileManager::isBufferModified(BufferID buffer) const {
    Buffer* buf = bufferFromId(buffer);
    return buf ? buf->isDirty() : false;
}

QString FileManager::getBufferText(BufferID buffer) const {
    Buffer* buf = bufferFromId(buffer);
    if (!buf)
        return QString();
    auto it = _bufferText.find(buffer);
    if (it != _bufferText.end())
        return it->second;
    return QString();
}

bool FileManager::setBufferText(BufferID buffer, const QString& text) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf)
        return false;
    _bufferText[buffer] = text;
    buf->setDocumentLength(text.length());
    buf->setDirty(true);
    return true;
}

EncodingType FileManager::getEncoding(BufferID buffer) const {
    Buffer* buf = bufferFromId(buffer);
    return buf ? buf->getEncoding() : EncodingType::UTF_8;
}

bool FileManager::setEncoding(BufferID buffer, EncodingType enc) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf)
        return false;
    buf->setEncoding(enc);
    return true;
}

EolType FileManager::getEolFormat(BufferID buffer) const {
    Buffer* buf = bufferFromId(buffer);
    return buf ? buf->getEolFormat() : EolType::EOL_LF;
}

bool FileManager::setEolFormat(BufferID buffer, EolType eol) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf)
        return false;
    buf->setEolFormat(eol);
    return true;
}

QString FileManager::getBufferPath(BufferID buffer) const {
    Buffer* buf = bufferFromId(buffer);
    return buf ? buf->getFullPathName() : QString();
}

bool FileManager::setBufferPath(BufferID buffer, const QString& path) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf)
        return false;
    buf->setFileName(path);
    return true;
}

bool FileManager::saveFile(BufferID buffer, const QString& path) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf)
        return false;

    QString savePath = path.isEmpty() ? buf->getFullPathName() : path;
    if (savePath.isEmpty())
        return false;

    // In real implementation, would get text from Scintilla editor
    QString content;  // Would come from Scintilla
    return saveFile(savePath, content, buf->getEncoding(), buf->getEolFormat());
}

bool FileManager::saveAllFiles() {
    bool allSuccess = true;
    for (Buffer* buf : _buffers) {
        if (buf->isDirty() && !buf->getFullPathName().isEmpty()) {
            if (!saveFile(buf->getID())) {
                allSuccess = false;
            }
        }
    }
    return allSuccess;
}

bool FileManager::closeFile(BufferID buffer) {
    int index = getBufferIndexByID(buffer);
    if (index == -1)
        return false;

    Buffer* buf = _buffers[index];

    // Stop watching
    if (!buf->getFullPathName().isEmpty()) {
        unwatchFile(buf->getFullPathName());
    }

    // Remove all references
    for (ScintillaEditor* editor : buf->editors()) {
        Q_UNUSED(editor);
        // Would notify editor that buffer is closing
    }

    _buffers.erase(_buffers.begin() + index);
    emit bufferClosed(buffer);
    delete buf;
    return true;
}

bool FileManager::closeAllFiles() {
    while (!_buffers.empty()) {
        BufferID id = _buffers.back()->getID();
        closeFile(id);
    }
    return true;
}

// ============================================================================
// Buffer References
// ============================================================================

void FileManager::addBufferReference(BufferID buffer, ScintillaEditor* editor) {
    Buffer* buf = bufferFromId(buffer);
    if (buf) {
        buf->addReference(editor);
    }
}

void FileManager::closeBuffer(BufferID buffer, const ScintillaEditor* identifier) {
    Buffer* buf = bufferFromId(buffer);
    if (!buf)
        return;

    int refs = buf->removeReference(identifier);

    if (refs <= 0) {
        // Buffer can be deallocated
        closeFile(buffer);
    }
}

// ============================================================================
// Document Length
// ============================================================================

size_t FileManager::documentLength(BufferID buffer) const {
    Buffer* buf = bufferFromId(buffer);
    return buf ? buf->getDocumentLength() : 0;
}

// ============================================================================
// Untitled Buffer Numbering
// ============================================================================

size_t FileManager::nextUntitledNewNumber() const {
    size_t maxNumber = 0;

    for (Buffer* buf : _buffers) {
        if (buf->isUntitled()) {
            QString name = buf->getFileName();
            // Parse "new X" format
            if (name.startsWith("new ")) {
                bool ok = false;
                int num = name.mid(4).toInt(&ok);
                if (ok && num > static_cast<int>(maxNumber)) {
                    maxNumber = num;
                }
            }
        }
    }

    return maxNumber + 1;
}

// ============================================================================
// File System Change Detection
// ============================================================================

void FileManager::checkFilesystemChanges(bool checkOnlyCurrentBuffer) {
    if (checkOnlyCurrentBuffer) {
        Buffer* buf = bufferFromId(_activeBuffer);
        if (buf) {
            buf->checkFileState();
        }
    } else {
        for (Buffer* buf : _buffers) {
            buf->checkFileState();
        }
    }
}

// ============================================================================
// Notification Handling
// ============================================================================

void FileManager::notifyBufferChanged(Buffer* buffer, int mask) {
    emit bufferChanged(buffer->getID(), mask);
}

// ============================================================================
// Private Helpers
// ============================================================================

bool FileManager::loadFileIntoBuffer(BufferID id, const QString& path) {
    Buffer* buf = bufferFromId(id);
    if (!buf)
        return false;

    QString content;
    EncodingType enc = EncodingDetector::detectFromPath(path.toStdString());

    if (!loadFile(path, content, enc))
        return false;

    buf->setEncoding(enc);
    buf->setDocumentLength(content.length());
    buf->setDirty(false);

    return true;
}

EncodingType FileManager::detectEncoding(const QByteArray& data) const {
    // Check for BOM
    if (data.startsWith("\xEF\xBB\xBF"))
        return EncodingType::UTF_8_BOM;
    if (data.startsWith("\xFF\xFE"))
        return EncodingType::UTF_16_LE;
    if (data.startsWith("\xFE\xFF"))
        return EncodingType::UTF_16_BE;

    // Check for valid UTF-8
    bool isUtf8 = true;
    for (int i = 0; i < data.size(); ++i) {
        unsigned char c = data[i];
        if (c > 127) {
            // Check UTF-8 continuation bytes
            int count = 0;
            if ((c & 0xE0) == 0xC0) count = 1;
            else if ((c & 0xF0) == 0xE0) count = 2;
            else if ((c & 0xF8) == 0xF0) count = 3;
            else { isUtf8 = false; break; }

            for (int j = 0; j < count && i + j < data.size(); ++j) {
                if ((data[i + j + 1] & 0xC0) != 0x80) {
                    isUtf8 = false;
                    break;
                }
            }
            i += count;
        }
    }

    return isUtf8 ? EncodingType::UTF_8 : EncodingType::ANSI;
}

LangType FileManager::detectLanguage(const QByteArray& data, const QString& fileName) const {
    // First try extension-based detection
    QString ext = getFileExtension(fileName).toLower();
    LangType lang = LanguageManager::detect(ext.toStdString());
    if (lang != LangType::L_TEXT)
        return lang;

    // Try content-based detection (shebang, etc.)
    QByteArray firstLine = data.left(256);
    if (firstLine.startsWith("#!")) {
        if (firstLine.contains("bash") || firstLine.contains("sh"))
            return LangType::L_BATCH;
        if (firstLine.contains("python"))
            return LangType::L_PYTHON;
        if (firstLine.contains("perl"))
            return LangType::L_PERL;
        if (firstLine.contains("ruby"))
            return LangType::L_RUBY;
        if (firstLine.contains("node"))
            return LangType::L_JS;
    }

    // Check for XML/HTML
    if (firstLine.contains("<html") || firstLine.contains("<!DOCTYPE"))
        return LangType::L_HTML;
    if (firstLine.contains("<?xml"))
        return LangType::L_XML;
    if (firstLine.contains("<?php"))
        return LangType::L_PHP;

    return LangType::L_TEXT;
}

QString FileManager::resolveFullPath(const QString& path) const {
    QFileInfo info(path);
    if (info.isAbsolute())
        return path;

    // Make relative path absolute based on current directory
    return QDir::current().absoluteFilePath(path);
}

bool FileManager::ensureDirectoryExists(const QString& dirPath) {
    QDir dir(dirPath);
    if (dir.exists())
        return true;
    return dir.mkpath(".");
}
