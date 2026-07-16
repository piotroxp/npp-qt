// NppIO.h - Qt6 port of Notepad++ file I/O engine
// Semantic lift from: PowerEditor/src/NppIO.cpp
// Replaces Win32 CreateFile/ReadFile/WriteFile/CloseHandle with Qt equivalents
//
// Win32 → Qt6 mapping:
//   CreateFile(GENERIC_READ...)  → QFile::open(QIODevice::ReadOnly)
//   CreateFile(GENERIC_WRITE...) → QFile::open(QIODevice::WriteOnly)
//   ReadFile(hFile, ...)         → QFile::read() / QTextStream::readAll()
//   WriteFile(hFile, ...)        → QFile::write() / QTextStream
//   CloseHandle(hFile)           → QFile::close() / QSaveFile::commit()
//   GetFileSize(hFile)           → QFileInfo::size() / QFile::size()
//   GetFileTime/SetFileTime      → QFile::fileTime() / QFile::setFileTime()
//   GetFileAttributes            → QFileInfo exists/isDir/isReadable
//   CopyFile/MoveFileEx          → QFile::copy() / QFile::rename()
//   DeleteFile                   → QFile::remove()
//   CreateDirectory              → QDir::mkpath()
//   ShellExecute(runas)         → QProcess::startDetached with pkexec/sudo
//   ShellExecute(explore)        → QDesktopServices::openUrl
//   ShellExecute(open)           → QDesktopServices::openUrl
//   CReadDirectoryChanges        → QFileSystemWatcher + thread
//   GetLongPathName              → QFileInfo::canonicalFilePath()

#pragma once

#include <QObject>
#include <QString>
#include <QFileInfo>
#include <QDateTime>
#include <QThread>
#include <QFileSystemWatcher>
#include <QHash>
#include <QMutex>
#include <QWaitCondition>
#include <QSaveFile>
#include <atomic>
#include "../common/Types.h"

// Forward declarations
class Buffer;

// ============================================================================
// Backup Feature Types (mirrors Win32 NppIO backup modes)
// ============================================================================
enum class NppBackupMode {
    None = 0,          // No backup
    Simple = 1,        // Single .bak file (overwrites)
    Numbered = 2,      // Numbered backups: file.txt.1, file.txt.2, ...
    Verbose = 3,       // Timestamped: file.YYYY-MM-DD_HHMMSS.bak
    RememberSession = 4 // Snapshot backup (session-aware)
};

// ============================================================================
// File Monitoring Info (mirrors Win32 MonitorInfo)
// ============================================================================
struct NppMonitorInfo {
    Buffer* _buffer = nullptr;
    QString _filePath;
    QThread* _watchThread = nullptr;

    NppMonitorInfo() = default;
    NppMonitorInfo(Buffer* buffer, const QString& filePath)
        : _buffer(buffer), _filePath(filePath) {}
};

// ============================================================================
// NppIO — Qt6 File I/O Engine
// ============================================================================
class NppIO : public QObject
{
    Q_OBJECT

public:
    // === Lifecycle ===

    explicit NppIO(QObject* parent = nullptr);
    ~NppIO();

    // === File Monitoring (replaces monitorFileOnChange / CReadDirectoryChanges) ===

    /**
     * @brief Start monitoring a file for external changes.
     * @param filePath Absolute path to the file to watch.
     * @param buffer Associated buffer pointer (stored for signal dispatch).
     * @return true if monitoring started successfully.
     *
     * Uses QFileSystemWatcher with a dedicated thread for debouncing.
     * On file modification: emits fileChanged(path).
     * On file deletion/rename: emits fileDeleted(path).
     */
    bool startMonitoring(const QString& filePath, Buffer* buffer);

    /**
     * @brief Stop monitoring a file.
     * @param filePath Path previously passed to startMonitoring.
     */
    void stopMonitoring(const QString& filePath);

    /**
     * @brief Stop all active file monitors.
     */
    void stopAllMonitoring();

    /**
     * @brief Check if a file is currently being monitored.
     */
    bool isMonitoring(const QString& filePath) const;

    // === File Read / Write (replaces CreateFile + ReadFile/WriteFile) ===

    /**
     * @brief Read an entire file into a QByteArray.
     * @param filePath Path to the file.
     * @param[out] success Set to true on success, false on failure.
     * @param[out] errorMsg Human-readable error description.
     * @param readOnlyBom If true, the raw bytes include the BOM if present.
     * @return Raw file bytes. Use EncodingDetector for text decoding.
     *
     * Uses QFile::open() with QIODevice::ReadOnly flag.
     * Handles large files (>2GB) via streaming read.
     */
    QByteArray readFile(const QString& filePath,
                        bool& success,
                        QString& errorMsg,
                        bool readOnlyBom = true) const;

    /**
     * @brief Read file in chunks for very large files.
     * @param filePath Path to the file.
     * @param chunkSize Bytes per chunk (default 64MB).
     * @param callback Called with each chunk: (chunkBytes, chunkIndex, totalChunks).
     *                 Return false from callback to abort.
     * @return true if entire file was read successfully.
     */
    bool readFileStreaming(const QString& filePath,
                            qint64 chunkSize,
                            std::function<bool(const QByteArray&, int, int)> callback,
                            QString& errorMsg) const;

    /**
     * @brief Write data to a file using atomic save (QSaveFile).
     * @param filePath Destination path.
     * @param data Raw bytes to write.
     * @param[out] errorMsg Error description on failure.
     * @param addBom Append UTF-8 BOM if true.
     * @return true on success; uses QSaveFile for atomic commit.
     *
     * Replaces CreateFile(GENERIC_WRITE) + WriteFile + CloseHandle.
     */
    bool writeFile(const QString& filePath,
                   const QByteArray& data,
                   QString& errorMsg,
                   bool addBom = false) const;

    /**
     * @brief Write text with encoding and EOL conversion.
     * @param filePath Destination path.
     * @param text Text content (UTF-8).
     * @param encoding Target encoding.
     * @param eol Line ending convention.
     * @param addBom Append BOM for UTF-8 BOM / UTF-16.
     * @param errorMsg Error output.
     * @return true on success.
     */
    bool writeTextFile(const QString& filePath,
                       const QString& text,
                       EncodingType encoding,
                       EolType eol,
                       bool addBom,
                       QString& errorMsg) const;

    // === File Attributes (replaces GetFileSize, GetFileTime, GetFileAttributes) ===

    /**
     * @brief Get file size in bytes.
     * Replaces GetFileSize / GetFileSizeEx.
     */
    qint64 fileSize(const QString& filePath) const;

    /**
     * @brief Get file modification time (last modified).
     * Replaces GetFileTime / FILETIME.
     * Returns invalid QDateTime on failure.
     */
    QDateTime fileModificationTime(const QString& filePath) const;

    /**
     * @brief Get file last access time.
     * Replaces GetFileTime with ftLastAccessTime.
     */
    QDateTime fileLastAccessTime(const QString& filePath) const;

    /**
     * @brief Get file creation time.
     * Replaces GetFileTime with ftCreationTime.
     */
    QDateTime fileCreationTime(const QString& filePath) const;

    /**
     * @brief Set file modification time.
     * Replaces SetFileTime.
     * @return true on success.
     */
    bool setFileModificationTime(const QString& filePath, const QDateTime& mtime) const;

    /**
     * @brief Check if file exists on disk.
     * Replaces GetFileAttributes != INVALID_FILE_ATTRIBUTES.
     */
    bool fileExists(const QString& filePath) const;

    /**
     * @brief Check if path is a directory.
     * Replaces GetFileAttributes with FILE_ATTRIBUTE_DIRECTORY.
     */
    bool isDirectory(const QString& path) const;

    /**
     * @brief Check if file is read-only (permissions).
     * Replaces GetFileAttributes with FILE_ATTRIBUTE_READONLY.
     */
    bool isReadOnly(const QString& filePath) const;

    /**
     * @brief Check if path exists (file or directory).
     * Replaces GetFileAttributes != 0xFFFFFFFF.
     */
    bool pathExists(const QString& path) const;

    /**
     * @brief Get canonical/absolute path (resolves symlinks, short names).
     * Replaces GetLongPathName.
     */
    QString canonicalPath(const QString& filePath) const;

    /**
     * @brief Get the parent directory of a file.
     * Replaces PathRemoveFileSpec / PathGetDirName.
     */
    QString parentDirectory(const QString& filePath) const;

    /**
     * @brief Get the file name (without directory) from a path.
     * Replaces PathFindFileName.
     */
    QString fileName(const QString& filePath) const;

    /**
     * @brief Get the file extension (including the dot).
     * Replaces PathFindExtension.
     */
    QString fileExtension(const QString& filePath) const;

    // === Backup Management (replaces CopyFileEx + MoveFileEx backup logic) ===

    /**
     * @brief Create a simple backup (copies file to path + ".bak").
     * Replaces CopyFileEx with COPY_FILE_NO_BUFFERING.
     */
    bool createSimpleBackup(const QString& originalPath, QString& errorMsg) const;

    /**
     * @brief Create a timestamped verbose backup.
     * Creates: originalName.YYYY-MM-DD_HHMMSS.bak
     */
    bool createVerboseBackup(const QString& originalPath, QString& errorMsg) const;

    /**
     * @brief Create a numbered backup with rotation.
     * Creates: file.txt.1, file.txt.2, ... up to maxBackups.
     * When maxBackups exceeded, deletes the oldest.
     */
    bool createNumberedBackup(const QString& originalPath,
                               int maxBackups,
                               QString& errorMsg) const;

    /**
     * @brief Create a backup based on configured backup mode.
     */
    bool createBackup(const QString& originalPath,
                      NppBackupMode mode,
                      int maxBackups,
                      QString& errorMsg) const;

    /**
     * @brief Rotate numbered backups, optionally deleting oldest if exceeds limit.
     */
    bool rotateNumberedBackups(const QString& originalPath,
                               int maxBackups,
                               QString& errorMsg) const;

    // === File System Operations (replaces Win32 SHFileOperation, etc.) ===

    /**
     * @brief Copy a file from source to destination.
     * Replaces CopyFile / CopyFileEx.
     * @param overwrite Replace destination if it exists.
     */
    bool copyFile(const QString& source,
                  const QString& destination,
                  bool overwrite,
                  QString& errorMsg) const;

    /**
     * @brief Move/rename a file.
     * Replaces MoveFileEx with MOVEFILE_REPLACE_EXISTING.
     */
    bool renameFile(const QString& oldPath,
                    const QString& newPath,
                    QString& errorMsg) const;

    /**
     * @brief Delete a file.
     * Replaces DeleteFile.
     * @param allowReadOnly Delete even if file is read-only.
     */
    bool deleteFile(const QString& filePath,
                    bool allowReadOnly,
                    QString& errorMsg) const;

    /**
     * @brief Create a directory (and parents if needed).
     * Replaces CreateDirectory / SHCreateDirectory.
     */
    bool createDirectory(const QString& path, QString& errorMsg) const;

    /**
     * @brief Remove an empty directory.
     * Replaces RemoveDirectory.
     */
    bool removeDirectory(const QString& path, QString& errorMsg) const;

    /**
     * @brief Atomically replace a file (like ReplaceFile on Windows).
     * Replaces ::ReplaceFile.
     * On Unix: renames replacement over target.
     */
    bool replaceFile(const QString& replacementPath,
                      const QString& targetPath,
                      QString& errorMsg) const;

    /**
     * @brief Ensure a directory exists, creating it if necessary.
     */
    bool ensureDirectoryExists(const QString& path, QString& errorMsg) const;

    /**
     * @brief Expand environment variables in a path string.
     * Replaces ExpandEnvironmentStrings.
     */
    QString expandEnvironmentStrings(const QString& input) const;

    /**
     * @brief Get full absolute path (resolves relative paths).
     * Replaces GetFullPathName.
     */
    QString absolutePath(const QString& path) const;

    // === External Program Execution (replaces ShellExecute) ===

    /**
     * @brief Get the path to the current application executable.
     * Replaces GetModuleFileName(NULL, ...).
     */
    static QString applicationFilePath();

    /**
     * @brief Get the directory containing the application.
     */
    static QString applicationDirPath();

    /**
     * @brief Open a file or URL using the system's default handler.
     * Replaces ShellExecute(NULL, L"open", path, ...).
     */
    bool openExternally(const QString& pathOrUrl) const;

    /**
     * @brief Open the containing folder and select the file.
     * Replaces ShellExecute(NULL, L"explore", ...) on the parent dir.
     */
    bool openContainingFolder(const QString& filePath) const;

    /**
     * @brief Launch the application with elevated (admin) privileges.
     * On Linux: tries pkexec, then kdesudo, then sudo.
     * On Windows: uses ShellExecute with "runas".
     * On macOS: uses osascript for privilege escalation.
     * Replaces ShellExecute(NULL, L"runas", nppFullPath, ...).
     * @return true if launch command was dispatched.
     */
    bool launchElevated(const QString& programPath,
                        const QStringList& arguments,
                        const QString& workingDirectory,
                        QString& errorMsg) const;

    /**
     * @brief Run a command and wait for it to complete.
     * Replaces _wsystem / system().
     */
    int runCommand(const QString& command,
                   const QStringList& arguments,
                   const QString& workingDirectory,
                   QString& output,
                   QString& errorMsg,
                   int timeoutMs) const;

    // === BOM Detection ===

    /**
     * @brief Detect encoding from raw bytes using BOM.
     * Checks for UTF-8 BOM (\xEF\xBB\xBF), UTF-16 LE (\xFF\xFE),
     * UTF-16 BE (\xFE\xFF), UTF-32 LE (\xFF\xFE 0x00 0x00), etc.
     */
    EncodingType detectEncodingBOM(const QByteArray& rawBytes) const;

    /**
     * @brief Strip BOM from raw bytes if present, return clean bytes.
     */
    QByteArray stripBOM(const QByteArray& rawBytes) const;

    /**
     * @brief Check if raw bytes have a UTF-8 BOM.
     */
    bool hasUtf8BOM(const QByteArray& rawBytes) const;

    /**
     * @brief Check if raw bytes have a UTF-16 LE BOM.
     */
    bool hasUtf16LEBOM(const QByteArray& rawBytes) const;

    /**
     * @brief Check if raw bytes have a UTF-16 BE BOM.
     */
    bool hasUtf16BEBOM(const QByteArray& rawBytes) const;

    // === Encoding Conversion ===

    /**
     * @brief Convert raw bytes from one encoding to UTF-8 QString.
     * Handles UTF-8, UTF-16 LE/BE, UTF-32, ANSI (system locale).
     */
    QString decodeToQString(const QByteArray& bytes, EncodingType encoding) const;

    /**
     * @brief Convert QString (UTF-16) to target encoding bytes.
     */
    QByteArray encodeFromQString(const QString& text, EncodingType encoding, bool addBom) const;

    /**
     * @brief Normalize line endings in text to target EOL type.
     */
    QString normalizeEOL(const QString& text, EolType eol) const;

signals:
    /**
     * Emitted when a monitored file is externally modified.
     * @param filePath Absolute path of the changed file.
     */
    void fileChanged(const QString& filePath);

    /**
     * Emitted when a monitored file is deleted or renamed.
     * @param filePath Absolute path of the removed file.
     */
    void fileDeleted(const QString& filePath);

    /**
     * Emitted when file system watching encounters an error.
     * @param filePath File path that had the error.
     * @param error Localized error string.
     */
    void monitoringError(const QString& filePath, const QString& error);

private slots:
    void onFileChanged(const QString& path);
    void onFileWatchError(const QString& path);

private:
    // === Private helpers ===

    // Get the base path for numbered backups (strips trailing .N)
    QString backupBasePath(const QString& numberedBackupPath) const;

    // Get the backup path for a given mode (constructs the backup filename)
    QString backupPathForMode(const QString& originalPath,
                               NppBackupMode mode,
                               const QString& customBackupDir = QString()) const;

    // Ensure backup directory exists
    bool ensureBackupDirectory(const QString& backupDir, QString& errorMsg) const;

    // Thread-safe watcher management
    void removeWatcher(const QString& path);

    // Internal: start a file watch on a specific path
    bool addFileWatch(const QString& path);

    mutable QMutex _monitorMutex;
    QHash<QString, NppMonitorInfo> _monitoredFiles;
    mutable QFileSystemWatcher _fileWatcher; // lives on main thread
};


// ============================================================================
// File Watcher Thread (mirrors Win32 monitorFileOnChange thread)
// ============================================================================
// Win32: monitorFileOnChange() used WaitForMultipleObjects on dirChanges and buf->getMonitoringEvent()
// Qt: FileWatchThread uses QFileSystemWatcher + QWaitCondition for stop signaling
// Debounces rapid changes with a 250ms sleep to limit refresh rate

class FileWatchThread : public QThread
{
    Q_OBJECT

public:
    explicit FileWatchThread(const QString& filePath, QObject* parent = nullptr);
    ~FileWatchThread();

    // Request the thread to stop (mirrors signaling the monitoring event)
    void requestStop();

    // Returns true if the watch loop is still active
    bool isRunning() const;

signals:
    // Emitted when the watched file is modified externally
    void fileModified(const QString& filePath);

    // Emitted when the watched file is deleted or renamed away
    void fileDeleted(const QString& filePath);

    // Emitted when monitoring encounters an error
    void errorOccurred(const QString& filePath, const QString& errorMessage);

protected:
    // Main watch loop (replaces WaitForMultipleObjects with 250ms timeout)
    void run() override;

private:
    // Check if the monitored file has changed since last check
    bool detectFileChange(const QString& filePath, const QDateTime& lastKnownMtime) const;

    // Sleep helper that respects stop requests
    void sleepWithInterrupt(int milliseconds);

    QString _filePath;
    QString _parentDir;
    std::atomic<bool> _stopRequested{false};
    std::atomic<bool> _isRunning{false};

    QDateTime _lastKnownMtime;
    int _debounceMs = 250; // matches original: Sleep(250)

    // Secondary watcher: poll file mtime for cases where the directory
    // watcher misses events (e.g., file changed from a different process on NFS)
    qint64 _lastKnownSize = -1;
};


// ============================================================================
// Scoped File Lock (reentrant lock for thread-safe file access)
// ============================================================================
// Mirrors the Win32 concept of a file HANDLE held for the duration of an operation
// but in a cross-platform, exception-safe wrapper.

class ScopedFileLock {
public:
    explicit ScopedFileLock(const QString& filePath);
    ~ScopedFileLock();

    bool isLocked() const { return _locked; }
    void release();

private:
    QString _filePath;
    bool _locked = false;
};


// ============================================================================
// Atomic File Writer (wraps QSaveFile for commit-on-success pattern)
// ============================================================================
// Replaces the Win32 pattern:
//   HANDLE hFile = CreateFile(..., CREATE_ALWAYS, ...);
//   WriteFile(hFile, ...);
//   CloseHandle(hFile);
//   if (failed) DeleteFile(path);

class AtomicFileWriter : public QObject {
    Q_OBJECT
public:
    explicit AtomicFileWriter(const QString& filePath, QObject* parent = nullptr);
    ~AtomicFileWriter();

    // Open for writing (creates or truncates)
    bool open(QString& errorMsg);

    // Write raw bytes
    qint64 write(const QByteArray& data);

    // Write from a QIODevice
    bool write(QIODevice* source, QString& errorMsg);

    // Commit the file (atomically replace target)
    // Returns false and cancels on failure.
    bool commit(QString& errorMsg);

    // Cancel and remove the temporary file
    void cancel();

    // Check if file is open
    bool isOpen() const;

    // Get the target file path
    QString targetPath() const { return _targetPath; }

signals:
    void bytesWritten(qint64 bytes);

private:
    QString _targetPath;
    QString _tempFilePath;
    QSaveFile* _saveFile = nullptr;
    bool _committing = false;
};


// ============================================================================
// Inline Template Implementations
// ============================================================================

inline NppIO::NppIO(QObject* parent)
    : QObject(parent)
{
    // QFileSystemWatcher parent must be this QObject
    _fileWatcher.moveToThread(this->thread());
    connect(&_fileWatcher, &QFileSystemWatcher::fileChanged,
            this, &NppIO::onFileChanged, Qt::QueuedConnection);
}

inline EncodingType NppIO::detectEncodingBOM(const QByteArray& rawBytes) const
{
    if (rawBytes.size() >= 3 &&
        static_cast<unsigned char>(rawBytes[0]) == 0xEF &&
        static_cast<unsigned char>(rawBytes[1]) == 0xBB &&
        static_cast<unsigned char>(rawBytes[2]) == 0xBF) {
        return EncodingType::UTF_8_BOM;
    }
    if (rawBytes.size() >= 2) {
        if (static_cast<unsigned char>(rawBytes[0]) == 0xFF &&
            static_cast<unsigned char>(rawBytes[1]) == 0xFE) {
            // Could be UTF-16 LE or UTF-32 LE — check for UTF-32 signature
            if (rawBytes.size() >= 4 &&
                static_cast<unsigned char>(rawBytes[2]) == 0x00 &&
                static_cast<unsigned char>(rawBytes[3]) == 0x00) {
                return EncodingType::UTF_32_LE;
            }
            return EncodingType::UTF_16_LE;
        }
        if (static_cast<unsigned char>(rawBytes[0]) == 0xFE &&
            static_cast<unsigned char>(rawBytes[1]) == 0xFF) {
            return EncodingType::UTF_16_BE;
        }
    }
    if (rawBytes.size() >= 4 &&
        static_cast<unsigned char>(rawBytes[0]) == 0xFF &&
        static_cast<unsigned char>(rawBytes[1]) == 0xFE &&
        static_cast<unsigned char>(rawBytes[2]) == 0x00 &&
        static_cast<unsigned char>(rawBytes[3]) == 0x00) {
        return EncodingType::UTF_32_LE;
    }
    if (rawBytes.size() >= 4 &&
        static_cast<unsigned char>(rawBytes[0]) == 0x00 &&
        static_cast<unsigned char>(rawBytes[1]) == 0x00 &&
        static_cast<unsigned char>(rawBytes[2]) == 0xFE &&
        static_cast<unsigned char>(rawBytes[3]) == 0xFF) {
        return EncodingType::UTF_32_BE;
    }
    return EncodingType::ANSI; // No BOM → treat as system ANSI
}

inline QByteArray NppIO::stripBOM(const QByteArray& rawBytes) const
{
    EncodingType bom = detectEncodingBOM(rawBytes);
    int skipBytes = 0;
    switch (bom) {
        case EncodingType::UTF_8_BOM:   skipBytes = 3; break;
        case EncodingType::UTF_16_LE:
        case EncodingType::UTF_16_BE:  skipBytes = 2; break;
        case EncodingType::UTF_32_LE:
        case EncodingType::UTF_32_BE:  skipBytes = 4; break;
        default:                        skipBytes = 0; break;
    }
    return (skipBytes > 0 && rawBytes.size() > skipBytes)
           ? rawBytes.mid(skipBytes) : rawBytes;
}

inline bool NppIO::hasUtf8BOM(const QByteArray& rawBytes) const
{
    return rawBytes.size() >= 3 &&
           static_cast<unsigned char>(rawBytes[0]) == 0xEF &&
           static_cast<unsigned char>(rawBytes[1]) == 0xBB &&
           static_cast<unsigned char>(rawBytes[2]) == 0xBF;
}

inline bool NppIO::hasUtf16LEBOM(const QByteArray& rawBytes) const
{
    return rawBytes.size() >= 2 &&
           static_cast<unsigned char>(rawBytes[0]) == 0xFF &&
           static_cast<unsigned char>(rawBytes[1]) == 0xFE;
}

inline bool NppIO::hasUtf16BEBOM(const QByteArray& rawBytes) const
{
    return rawBytes.size() >= 2 &&
           static_cast<unsigned char>(rawBytes[0]) == 0xFE &&
           static_cast<unsigned char>(rawBytes[1]) == 0xFF;
}

inline qint64 NppIO::fileSize(const QString& filePath) const
{
    QFileInfo info(filePath);
    return info.exists() ? info.size() : -1;
}

inline bool NppIO::fileExists(const QString& filePath) const
{
    return QFileInfo::exists(filePath);
}

inline bool NppIO::pathExists(const QString& path) const
{
    return QFileInfo::exists(path);
}

inline bool NppIO::isDirectory(const QString& path) const
{
    return QFileInfo(path).isDir();
}

inline bool NppIO::isReadOnly(const QString& filePath) const
{
    QFileInfo info(filePath);
    // Check both permissions and the read-only attribute
    return !info.isWritable() || (info.exists() && (info.permissions() & QFile::ReadOwner) == 0);
}

inline QString NppIO::parentDirectory(const QString& filePath) const
{
    QFileInfo info(filePath);
    return info.path();
}

inline QString NppIO::fileName(const QString& filePath) const
{
    return QFileInfo(filePath).canonicalFilePath();
}

inline QString NppIO::absolutePath(const QString& path) const
{
    return QFileInfo(path).absoluteFilePath();
}
