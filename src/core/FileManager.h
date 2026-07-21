// FileManager.h - File I/O and buffer management
// Copyright (C) 2026 Agent Army
// Copyright (C) 2021 Don HO <don.h@free.fr>
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include "common/Observer.h"
#include "../common/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <QObject>
#include <QString>
#include <QFileSystemWatcher>
#include <QDateTime>
#include <QThread>

// Forward declaration of the global FileLoaderWorker (defined in
// src/workers/FileLoaderWorker.h). Done at file scope so the class name
// inside class FileManager resolves to the global type, not a nested type.
// The full definition is only needed inside FileManager.cpp.
class FileLoaderWorker;

// Forward declaration - FileNotification is defined in Application.h
struct FileNotification;

// Forward declarations
class Buffer;
class ScintillaEditor;
class EncodingDetector;

// ============================================================================
// File Notification (for file watcher observer)
// ============================================================================
// FileNotification is defined in Application.h

// ============================================================================
// FileManager - Manages all buffers and file I/O
// ============================================================================
class FileManager : public QObject, public NonCopyable {
    Q_OBJECT

public:
    // Singleton access
    static FileManager& instance() { static FileManager fm; return fm; }

    FileManager();
    ~FileManager() override;

    // ========================================================================
    // File I/O
    // ========================================================================

    bool loadFile(const QString& path, QString& outContent, EncodingType encoding);
    bool saveFile(const QString& path, const QString& content, EncodingType encoding, EolType eol);
    bool deleteFile(const QString& path);
    bool moveFile(const QString& src, const QString& dst);
    bool createEmptyFile(const QString& path);

    // ========================================================================
    // File watching
    // ========================================================================

    bool watchFile(const QString& path);
    void unwatchFile(const QString& path);
    void unwatchAll();
    bool isWatched(const QString& path) const;
    QStringList getWatchedFiles() const;

    // ========================================================================
    // File utilities
    // ========================================================================

    QString getFileExtension(const QString& path) const;
    QString getFileName(const QString& path) const;
    QString getFileDirectory(const QString& path) const;
    bool fileExists(const QString& path) const;
    bool isDirectory(const QString& path) const;
    int64_t getFileSize(const QString& path) const;

    Observer<FileNotification>& fileWatcherObserver() { return _fileWatcherObserver; }

    // ========================================================================
    // Buffer management
    // ========================================================================

    // Create/manage buffers
    BufferID createBuffer(const QString& fileName = QString(), bool isLargeFile = false);
    Buffer* bufferFromId(BufferID id) const;
    BufferID openFile(const QString& path, bool readOnly = false);

    // Async variant: returns immediately, emits `fileLoaded` once the load
    // is complete. Uses the bundled FileLoaderWorker on a dedicated worker
    // thread so the GUI thread never blocks on QFile::readAll() for large
    // files. While loading, `loadingProgress(percent)` fires from the
    // worker. The sync `openFile()` path remains unchanged for callers
    // that need immediate access (tests, command-line paths).
    void openFileAsync(const QString& path, bool readOnly = false);

    BufferID createNewFile();
    BufferID duplicateBuffer(BufferID buffer);

    // Buffer accessors
    QString getFileName(BufferID buffer) const;
    BufferID getActiveBuffer() const;
    BufferID getBufferAt(int index, int view = 0) const;
    size_t getBufferCount() const;
    int getBufferIndexByID(BufferID id) const;
    Buffer* getBufferByIndex(size_t index) const;
    size_t getNbDirtyBuffers() const;

    // Buffer operations
    void setActiveBuffer(BufferID buffer);
    bool isBufferModified(BufferID buffer) const;
    QString getBufferText(BufferID buffer) const;
    bool setBufferText(BufferID buffer, const QString& text);
    EncodingType getEncoding(BufferID buffer) const;
    bool setEncoding(BufferID buffer, EncodingType enc);
    EolType getEolFormat(BufferID buffer) const;
    bool setEolFormat(BufferID buffer, EolType eol);
    QString getBufferPath(BufferID buffer) const;
    bool setBufferPath(BufferID buffer, const QString& path);
    bool saveFile(BufferID buffer, const QString& path = QString());
    bool saveAllFiles();
    bool closeFile(BufferID buffer);
    bool closeAllFiles();

    // Buffer references (for multi-view support)
    void addBufferReference(BufferID buffer, ScintillaEditor* editor);
    void closeBuffer(BufferID buffer, const ScintillaEditor* identifier);

    // Document length query
    size_t documentLength(BufferID buffer) const;

    // Untitled buffer numbering
    size_t nextUntitledNewNumber() const;

    // ========================================================================
    // File system change detection
    // ========================================================================

    void checkFilesystemChanges(bool checkOnlyCurrentBuffer = false);

    // ========================================================================
    // Notification handling (called by Buffer)
    // ========================================================================

    void notifyBufferChanged(Buffer* buffer, int mask);

    // ========================================================================
    // Encoding detection settings
    // ========================================================================

    void enableAutoDetectEncoding() { _autoDetectEncoding = true; }
    void disableAutoDetectEncoding() { _autoDetectEncoding = false; }
    bool isAutoDetectEncodingEnabled() const { return _autoDetectEncoding; }

    // ============================================================================
    // Buffer→Editor sync (used by saveFile to retrieve live document text)
    // ============================================================================

    // Returned function should fill `outText` with the current editor content
    // for the given buffer. Return true if content was filled, false to skip.
    using BufferTextProvider = std::function<bool(BufferID buffer, QString& outText)>;
    void setBufferTextProvider(BufferTextProvider fn) { _bufferTextProvider = std::move(fn); }

signals:
    void bufferChanged(BufferID buffer, int changeMask);
    void bufferCreated(BufferID buffer);
    void bufferClosed(BufferID buffer);
    void fileSystemChanged(const QString& path);
    void loadingProgress(int percent);
    void fileLoaded(bool success, BufferID buffer, const QString& error);

private:
    // FileLoaderWorker is forward-declared at file scope (above this class)
    // to avoid the nested-class trap. Only a pointer is held in FileManager;
    // the full type is used inside the .cpp where FileLoaderWorker.h is included.
    FileLoaderWorker* _loader = nullptr;
    QThread* _loaderThread = nullptr;
    // Internal helpers
    bool loadFileIntoBuffer(BufferID id, const QString& path);
    EncodingType detectEncoding(const QByteArray& data) const;
    LangType detectLanguage(const QByteArray& data, const QString& fileName) const;
    QString resolveFullPath(const QString& path) const;
    bool ensureDirectoryExists(const QString& dirPath);

    // Member variables
    std::unordered_map<QString, int> _watchDescriptors;
    Observer<FileNotification> _fileWatcherObserver;
    std::vector<Buffer*> _buffers;
    QFileSystemWatcher* _fileWatcher = nullptr;
    BufferID _nextBufferId = reinterpret_cast<BufferID>(1);
    BufferID _activeBuffer = BUFFER_INVALID;
    bool _autoDetectEncoding = true;
    std::unordered_map<BufferID, QString> _bufferText;
    BufferTextProvider _bufferTextProvider;
};
