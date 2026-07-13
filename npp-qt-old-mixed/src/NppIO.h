// npp-qt: Qt6 port of Notepad++ NppIO
// Semantic lift from: PowerEditor/src/NppIO.cpp
// File I/O: CreateFile/ReadFile/WriteFile → QFile/QDataStream

#pragma once

#include <QObject>
#include <QString>
#include <QFileInfo>
#include <QDateTime>
#include <QThread>
#include <QFileSystemWatcher>
#include <QEventLoop>
#include <QHash>

// Forward declarations
class Buffer;

/**
 * NppIO — Qt6 port of Notepad++ file I/O operations
 *
 * Replaces Win32 file I/O:
 *   CreateFile() / ReadFile() / WriteFile() / CloseHandle()
 * With Qt equivalents:
 *   QFile::open() / QFile::read() / QFile::write() / QFile::close()
 *
 * Also handles:
 *   - File monitoring (Win32 CReadDirectoryChanges → QFileSystemWatcher)
 *   - Backup management (Win32 MoveFileEx → QFile::rename)
 *   - Encoding detection (UTF-8 BOM handling via Utf8_16)
 */
class NppIO : public QObject
{
    Q_OBJECT

public:
    explicit NppIO(QObject* parent = nullptr);
    ~NppIO();

    // === File monitoring (replaces monitorFileOnChange thread) ===

    /**
     * Start monitoring a file for changes.
     * Uses QFileSystemWatcher instead of Win32 CReadDirectoryChanges.
     */
    void startMonitoring(const QString& filePath, Buffer* buffer);

    /**
     * Stop monitoring a file.
     */
    void stopMonitoring(const QString& filePath);

    // === File operations ===

    /**
     * Read a file into a buffer with encoding detection.
     * Replaces CreateFile + ReadFile in doOpen().
     */
    QByteArray readFile(const QString& filePath, bool& success, QString& errorMsg);

    /**
     * Write data to a file.
     * Replaces CreateFile + WriteFile in doSave().
     */
    bool writeFile(const QString& filePath, const QByteArray& data, QString& errorMsg);

    /**
     * Get file modification time.
     * Replaces GetFileTime / FILETIME.
     */
    QDateTime fileModificationTime(const QString& filePath) const;

    /**
     * Get file size in bytes.
     * Replaces GetFileSize.
     */
    qint64 fileSize(const QString& filePath) const;

    /**
     * Check if file exists and is readable.
     * Replaces GetFileAttributes checking for INVALID_FILE_ATTRIBUTES.
     */
    bool fileExists(const QString& filePath) const;

    /**
     * Check if path is a directory.
     * Replaces GetFileAttributes with FILE_ATTRIBUTE_DIRECTORY.
     */
    bool isDirectory(const QString& path) const;

    /**
     * Create a backup copy of a file.
     * Replaces CopyFileEx with COPY_FILE_NO_BUFFERING.
     */
    bool createBackup(const QString& originalPath, const QString& backupPath, bool* cancelRequested = nullptr) const;

    /**
     * Rename/move a file (used for backup rotation).
     * Replaces MoveFileEx with MOVEFILE_REPLACE_EXISTING.
     */
    bool renameFile(const QString& oldPath, const QString& newPath) const;

    /**
     * Delete a file.
     * Replaces DeleteFile.
     */
    bool deleteFile(const QString& filePath) const;

    /**
     * Create a directory.
     * Replaces CreateDirectory.
     */
    bool createDirectory(const QString& path) const;

    /**
     * Get the application executable path.
     * Replaces GetModuleFileName(NULL, ...).
     */
    static QString applicationFilePath();

    /**
     * Open a file/URL using the system handler.
     * Replaces ShellExecute with "runas" or "open" verbs.
     */
    bool openExternally(const QString& pathOrUrl, bool elevated = false) const;

    /**
     * Open containing folder in system file explorer.
     * Replaces ShellExecute with "explore" verb.
     */
    bool openContainingFolder(const QString& filePath) const;

signals:
    void fileChanged(const QString& filePath);
    void monitoringError(const QString& filePath, const QString& error);

private slots:
    void onFileChanged(const QString& path);
    void onFileWatchError(const QString& path);

private:
    QFileSystemWatcher _fileWatcher;
    QHash<QString, Buffer*> _monitoredBuffers;
};

// ============================================================================
// FILE WATCHER THREAD
// ============================================================================
// Win32: monitorFileOnChange() was a separate thread using WaitForMultipleObjects
// Qt: Use QFileSystemWatcher + signals/slots (main thread)

class FileWatchThread : public QThread
{
    Q_OBJECT

public:
    explicit FileWatchThread(const QString& filePath, QObject* parent = nullptr);
    ~FileWatchThread();

    void requestStop();
    bool waitHandle() const;  // Replaces HANDLE from GetWaitHandle()

signals:
    void fileChanged(const QString& filePath);
    void finished();

protected:
    void run() override;

private:
    QString _filePath;
    std::atomic<bool> _stopRequested{false};
    QEventLoop* _loop = nullptr;
};
