// NppIO.cpp - File I/O, backup, monitoring, and encoding utilities for Notepad--Qt
// Copyright (C) 2026 Agent Army
// Copyright (C) 2021 Don HO <don.h@free.fr>
// GPL v3
// Semantic lift: Win32 CReadDirectoryChanges / CReadFileChanges → QFileSystemWatcher

#include "NppIO.h"
#include "Buffer.h"
#include "common/Types.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QSaveFile>
#include <QByteArray>
#include <QMutexLocker>
#include <QProcess>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QStringConverter>
#include <QDateTime>
#include <QUrl>
#include <QDebug>
#include <QRegularExpression>
#include <QLockFile>
#include <QThread>
#include <thread>

// ============================================================================
// Lifecycle
// ============================================================================

NppIO::~NppIO()
{
    stopAllMonitoring();
}

// ============================================================================
// File Monitoring (replaces monitorFileOnChange / CReadDirectoryChanges)
// ============================================================================

bool NppIO::startMonitoring(const QString& filePath, Buffer* buffer)
{
    QMutexLocker locker(&_monitorMutex);

    if (_monitoredFiles.contains(filePath)) {
        // Already monitoring
        return true;
    }

    NppMonitorInfo info(buffer, filePath);
    _monitoredFiles.insert(filePath, info);

    if (!_fileWatcher.files().contains(filePath)) {
        if (!_fileWatcher.addPath(filePath)) {
            _monitoredFiles.remove(filePath);
            return false;
        }
    }

    return true;
}

void NppIO::stopMonitoring(const QString& filePath)
{
    QMutexLocker locker(&_monitorMutex);
    removeWatcher(filePath);
}

void NppIO::stopAllMonitoring()
{
    QMutexLocker locker(&_monitorMutex);
    const auto paths = _monitoredFiles.keys();
    for (const QString& path : paths) {
        removeWatcher(path);
    }
}

bool NppIO::isMonitoring(const QString& filePath) const
{
    QMutexLocker locker(&_monitorMutex);
    return _monitoredFiles.contains(filePath);
}

void NppIO::onFileChanged(const QString& path)
{
    qDebug() << "NppIO: file changed:" << path;
    emit fileChanged(path);
}

void NppIO::onFileWatchError(const QString& path)
{
    qWarning() << "NppIO: file watch error for:" << path;
    emit monitoringError(path, QStringLiteral("File system watcher error on: %1").arg(path));
}

void NppIO::removeWatcher(const QString& path)
{
    if (_monitoredFiles.contains(path)) {
        _monitoredFiles.remove(path);
        if (_fileWatcher.files().contains(path)) {
            _fileWatcher.removePath(path);
        }
    }
}

bool NppIO::addFileWatch(const QString& path)
{
    if (!_fileWatcher.files().contains(path)) {
        return _fileWatcher.addPath(path);
    }
    return true;
}

// ============================================================================
// File Read / Write (replaces CreateFile + ReadFile/WriteFile)
// ============================================================================

QByteArray NppIO::readFile(const QString& filePath,
                           bool& success,
                           QString& errorMsg,
                           bool readOnlyBom) const
{
    (void)readOnlyBom;  // BOM handled at caller level
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        success = false;
        errorMsg = QStringLiteral("Cannot open file for reading: %1\n%2")
                       .arg(filePath, file.errorString());
        return {};
    }
    success = true;
    errorMsg.clear();
    return file.readAll();
}

bool NppIO::readFileStreaming(const QString& filePath,
                              qint64 chunkSize,
                              std::function<bool(const QByteArray&, int chunkIndex, int totalChunks)> callback,
                              QString& errorMsg) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        errorMsg = QStringLiteral("Cannot open file for streaming read: %1\n%2")
                       .arg(filePath, file.errorString());
        return false;
    }

    const qint64 fileSize = file.size();
    const int totalChunks = static_cast<int>((fileSize + chunkSize - 1) / chunkSize);
    int chunkIndex = 0;
    bool ok = true;

    while (!file.atEnd()) {
        QByteArray chunk = file.read(chunkSize);
        if (chunk.isEmpty() && !file.atEnd()) {
            errorMsg = QStringLiteral("Read error at chunk %1: %2").arg(chunkIndex).arg(file.errorString());
            ok = false;
            break;
        }
        if (!callback(chunk, chunkIndex, totalChunks)) {
            errorMsg = QStringLiteral("Callback aborted at chunk %1").arg(chunkIndex);
            ok = false;
            break;
        }
        ++chunkIndex;
    }

    file.close();
    return ok;
}

bool NppIO::writeFile(const QString& filePath,
                      const QByteArray& data,
                      QString& errorMsg,
                      bool addBom) const
{
    QSaveFile saver(filePath);
    if (!saver.open(QIODevice::WriteOnly)) {
        errorMsg = QStringLiteral("Cannot open file for writing: %1\n%2")
                       .arg(filePath, saver.errorString());
        return false;
    }

    QByteArray toWrite = data;
    if (addBom) {
        toWrite.prepend("\xEF\xBB\xBF"); // UTF-8 BOM
    }

    qint64 written = saver.write(toWrite);
    if (written != toWrite.size()) {
        errorMsg = QStringLiteral("Partial write: %1 of %2 bytes").arg(written).arg(toWrite.size());
        saver.cancelWriting();
        return false;
    }

    if (!saver.commit()) {
        errorMsg = QStringLiteral("Failed to commit file: %1").arg(saver.errorString());
        return false;
    }

    errorMsg.clear();
    return true;
}

bool NppIO::writeTextFile(const QString& filePath,
                          const QString& text,
                          EncodingType encoding,
                          EolType eol,
                          bool addBom,
                          QString& errorMsg) const
{
    // Normalize EOL first
    QString normalized = normalizeEOL(text, eol);

    // Encode to target encoding
    QByteArray data = encodeFromQString(normalized, encoding, addBom);

    // Write via atomic writer
    return writeFile(filePath, data, errorMsg, false);
}

// ============================================================================
// File Attributes (replaces GetFileSize, GetFileTime, GetFileAttributes)
// ============================================================================

QDateTime NppIO::fileModificationTime(const QString& filePath) const
{
    return QFileInfo(filePath).lastModified();
}

QDateTime NppIO::fileLastAccessTime(const QString& filePath) const
{
    return QFileInfo(filePath).lastRead();
}

QDateTime NppIO::fileCreationTime(const QString& filePath) const
{
    QFileInfo fi(filePath);
#if defined(Q_OS_WIN) || (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QDateTime ct = fi.birthTime();
    if (!ct.isValid())
        ct = fi.lastModified();
    return ct;
#else
    return fi.lastModified();
#endif
}

bool NppIO::setFileModificationTime(const QString& filePath, const QDateTime& mtime) const
{
    // Use touch command for cross-platform compatibility
    QProcess proc;
    QStringList args = { "-m", "-t", mtime.toString(Qt::ISODate).replace("T", "").left(12) + "00", filePath };
    proc.start(QStringLiteral("touch"), args);
    if (!proc.waitForFinished(5000)) {
        proc.kill();
        return false;
    }
    return proc.exitCode() == 0;
}





QString NppIO::canonicalPath(const QString& filePath) const
{
    return QFileInfo(filePath).canonicalFilePath();
}



QString NppIO::fileExtension(const QString& filePath) const
{
    QString suffix = QFileInfo(filePath).suffix();
    if (!suffix.isEmpty() && !suffix.startsWith('.'))
        suffix.prepend('.');
    return suffix;
}

// ============================================================================
// Backup Management (replaces CopyFileEx + MoveFileEx backup logic)
// ============================================================================

QString NppIO::backupBasePath(const QString& numberedBackupPath) const
{
    // Strip trailing .N from numbered backup: file.txt.3 → file.txt
    QString base = numberedBackupPath;
    int dotPos = base.lastIndexOf('.');
    if (dotPos > 0) {
        bool ok;
        base.left(dotPos).toInt(&ok);
        if (ok) {
            base.chop(base.size() - dotPos);
            return base;
        }
    }
    return numberedBackupPath;
}

QString NppIO::backupPathForMode(const QString& originalPath,
                                 NppBackupMode mode,
                                 const QString& customBackupDir) const
{
    QFileInfo fi(originalPath);
    QString baseName = fi.baseName();
    QString suffix = fi.suffix();
    QString dir = customBackupDir.isEmpty() ? fi.absolutePath() : customBackupDir;

    switch (mode) {
    case NppBackupMode::Simple:
        return dir + '/' + baseName + (suffix.isEmpty() ? QString() : '.' + suffix) + ".bak";
    case NppBackupMode::Verbose: {
        QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss");
        return dir + '/' + baseName + '.' + ts + ".bak";
    }
    case NppBackupMode::Numbered:
        // caller will append .N
        return dir + '/' + baseName + (suffix.isEmpty() ? QString() : '.' + suffix);
    default:
        return originalPath + ".bak";
    }
}

bool NppIO::ensureBackupDirectory(const QString& backupDir, QString& errorMsg) const
{
    if (backupDir.isEmpty())
        return true;
    QDir d;
    if (!d.mkpath(backupDir)) {
        errorMsg = QStringLiteral("Failed to create backup directory: %1").arg(backupDir);
        return false;
    }
    return true;
}

bool NppIO::createSimpleBackup(const QString& originalPath, QString& errorMsg) const
{
    QString backupPath = backupPathForMode(originalPath, NppBackupMode::Simple);
    if (QFile::exists(backupPath))
        QFile::remove(backupPath);
    if (!QFile::copy(originalPath, backupPath)) {
        errorMsg = QStringLiteral("Failed to create simple backup: %1 → %2")
                       .arg(originalPath, backupPath);
        return false;
    }
    return true;
}

bool NppIO::createVerboseBackup(const QString& originalPath, QString& errorMsg) const
{
    QString backupPath = backupPathForMode(originalPath, NppBackupMode::Verbose);
    if (!QFile::copy(originalPath, backupPath)) {
        errorMsg = QStringLiteral("Failed to create verbose backup: %1 → %2")
                       .arg(originalPath, backupPath);
        return false;
    }
    return true;
}

bool NppIO::createNumberedBackup(const QString& originalPath,
                                 int maxBackups,
                                 QString& errorMsg) const
{
    // Rotate: shift 1→2, 2→3, ..., max-1→max, delete max, then create .1
    if (maxBackups <= 0)
        maxBackups = 10;

    QFileInfo fi(originalPath);
    QString base = fi.absolutePath() + '/' + fi.fileName(); // full original path

    // Find existing numbered backups: file.txt.1, file.txt.2, ...
    QStringList filters;
    filters << (fi.fileName() + ".%1").arg(maxBackups);
    QDir dir(fi.absolutePath());
    QStringList existing = dir.entryList({fi.fileName() + ".*"}, QDir::Files | QDir::Hidden);

    // Find highest number
    int highest = 0;
    for (const QString& name : existing) {
        if (name.startsWith(fi.fileName() + '.')) {
            bool ok;
            int n = name.mid(fi.fileName().size() + 1).toInt(&ok);
            if (ok && n > highest)
                highest = n;
        }
    }

    // Delete the one that would exceed maxBackups
    QString toDelete = fi.fileName() + '.' + QString::number(maxBackups);
    if (dir.exists(toDelete))
        QFile::remove(dir.absoluteFilePath(toDelete));

    // Rename existing down
    for (int i = highest; i >= 1; --i) {
        QString oldName = fi.fileName() + '.' + QString::number(i);
        QString newName = fi.fileName() + '.' + QString::number(i + 1);
        if (dir.exists(oldName))
            QFile::rename(dir.absoluteFilePath(oldName), dir.absoluteFilePath(newName));
    }

    // Copy current to .1
    QString newBackup = fi.absoluteFilePath() + ".1";
    if (!QFile::copy(originalPath, newBackup)) {
        errorMsg = QStringLiteral("Failed to create numbered backup: %1 → %2")
                       .arg(originalPath, newBackup);
        return false;
    }
    return true;
}

bool NppIO::createBackup(const QString& originalPath,
                         NppBackupMode mode,
                         int maxBackups,
                         QString& errorMsg) const
{
    switch (mode) {
    case NppBackupMode::None:
        return true;
    case NppBackupMode::Simple:
        return createSimpleBackup(originalPath, errorMsg);
    case NppBackupMode::Verbose:
        return createVerboseBackup(originalPath, errorMsg);
    case NppBackupMode::Numbered:
        return createNumberedBackup(originalPath, maxBackups, errorMsg);
    case NppBackupMode::RememberSession:
        // For session-aware: same as simple for now
        return createSimpleBackup(originalPath, errorMsg);
    default:
        errorMsg = QStringLiteral("Unknown backup mode: %1").arg(static_cast<int>(mode));
        return false;
    }
}

bool NppIO::rotateNumberedBackups(const QString& originalPath,
                                  int maxBackups,
                                  QString& errorMsg) const
{
    return createNumberedBackup(originalPath, maxBackups, errorMsg);
}

// ============================================================================
// File System Operations (replaces Win32 SHFileOperation, etc.)
// ============================================================================

bool NppIO::copyFile(const QString& source,
                     const QString& destination,
                     bool overwrite,
                     QString& errorMsg) const
{
    if (overwrite && QFile::exists(destination))
        QFile::remove(destination);
    if (!QFile::copy(source, destination)) {
        errorMsg = QStringLiteral("Failed to copy file: %1 → %2\n%3")
                       .arg(source, destination, QFile(destination).errorString());
        return false;
    }
    return true;
}

bool NppIO::renameFile(const QString& oldPath,
                       const QString& newPath,
                       QString& errorMsg) const
{
    if (QFile::exists(newPath)) {
        if (!QFile::remove(newPath)) {
            errorMsg = QStringLiteral("Cannot overwrite existing file: %1").arg(newPath);
            return false;
        }
    }
    if (!QFile::rename(oldPath, newPath)) {
        errorMsg = QStringLiteral("Rename failed: %1 → %2\n%3")
                       .arg(oldPath, newPath, QFile(oldPath).errorString());
        return false;
    }
    return true;
}

bool NppIO::deleteFile(const QString& filePath,
                       bool allowReadOnly,
                       QString& errorMsg) const
{
    if (allowReadOnly) {
        QFile::setPermissions(filePath, QFile::ReadOwner | QFile::WriteOwner);
    }
    if (!QFile::remove(filePath)) {
        errorMsg = QStringLiteral("Delete failed: %1\n%2")
                       .arg(filePath, QFile(filePath).errorString());
        return false;
    }
    return true;
}

bool NppIO::createDirectory(const QString& path, QString& errorMsg) const
{
    QDir d;
    if (!d.mkpath(path)) {
        errorMsg = QStringLiteral("Failed to create directory: %1").arg(path);
        return false;
    }
    return true;
}

bool NppIO::removeDirectory(const QString& path, QString& errorMsg) const
{
    QDir d(path);
    if (!d.exists()) {
        errorMsg = QStringLiteral("Directory does not exist: %1").arg(path);
        return false;
    }
    if (!d.rmdir(path)) {
        errorMsg = QStringLiteral("Failed to remove directory (may not be empty): %1").arg(path);
        return false;
    }
    return true;
}

bool NppIO::replaceFile(const QString& replacementPath,
                        const QString& targetPath,
                        QString& errorMsg) const
{
    // Atomic replace: rename replacement over target
    if (QFile::exists(targetPath))
        QFile::remove(targetPath);
    if (!QFile::rename(replacementPath, targetPath)) {
        errorMsg = QStringLiteral("Replace file failed: %1 → %2\n%3")
                       .arg(replacementPath, targetPath, QFile(replacementPath).errorString());
        return false;
    }
    return true;
}

bool NppIO::ensureDirectoryExists(const QString& path, QString& errorMsg) const
{
    return createDirectory(path, errorMsg);
}

QString NppIO::expandEnvironmentStrings(const QString& input) const
{
    QString result = input;
    const QStringList envList = QProcess::systemEnvironment();
    auto envGet = [&envList](const QString& var) -> QString {
        for (const QString& e : envList) {
            int eq = e.indexOf('=');
            if (eq > 0 && e.left(eq) == var)
                return e.mid(eq + 1);
        }
        return QString();
    };

    // Expand ${VAR} patterns
    QRegularExpression envVarPattern("\\$\\{([A-Za-z_][A-Za-z0-9_]*)\\}");
    QRegularExpressionMatchIterator it = envVarPattern.globalMatch(result);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        QString varName = m.captured(1);
        QString replacement = envGet(varName);
        if (!replacement.isEmpty())
            result.replace(m.capturedStart(), m.capturedLength(), replacement);
    }

    // Expand $VAR patterns (simple, avoid expanding ${...})
    QRegularExpression simplePattern("\\$([A-Za-z_][A-Za-z0-9_]*)");
    it = simplePattern.globalMatch(result);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        QString varName = m.captured(1);
        // Skip if part of ${...}
        if (m.capturedStart() > 0 && result[m.capturedStart() - 1] == '{')
            continue;
        QString replacement = envGet(varName);
        if (!replacement.isEmpty())
            result.replace(m.capturedStart(), m.capturedLength(), replacement);
    }

    return result;
}


// ============================================================================
// External Program Execution (replaces ShellExecute)
// ============================================================================

QString NppIO::applicationFilePath()
{
    return QCoreApplication::applicationFilePath();
}

QString NppIO::applicationDirPath()
{
    return QCoreApplication::applicationDirPath();
}

bool NppIO::openExternally(const QString& pathOrUrl) const
{
    QUrl url(pathOrUrl);
    if (!url.isValid())
        url = QUrl::fromLocalFile(pathOrUrl);
    return QDesktopServices::openUrl(url);
}

bool NppIO::openContainingFolder(const QString& filePath) const
{
    QFileInfo fi(filePath);
    QString dir = fi.absolutePath();
    return openExternally(dir);
}

bool NppIO::launchElevated(const QString& programPath,
                            const QStringList& arguments,
                            const QString& workingDirectory,
                            QString& errorMsg) const
{
    (void)errorMsg;  // TODO: propagate error to caller
    // Try pkexec first (Linux), then kdesudo, then sudo
    QStringList elevCmd = { QStringLiteral("pkexec"),
                            QStringLiteral("--disable-internal-agent"),
                            programPath };
    elevCmd.append(arguments);

    QProcess proc;
    if (!workingDirectory.isEmpty())
        proc.setWorkingDirectory(workingDirectory);
    proc.setProgram(elevCmd.first());
    proc.setArguments(elevCmd.mid(1));
    proc.startDetached();
    return true;
}

int NppIO::runCommand(const QString& command,
                      const QStringList& arguments,
                      const QString& workingDirectory,
                      QString& output,
                      QString& errorMsg,
                      int timeoutMs) const
{
    QProcess proc;
    if (!workingDirectory.isEmpty())
        proc.setWorkingDirectory(workingDirectory);
    proc.setProgram(command);
    proc.setArguments(arguments);
    proc.setProcessChannelMode(QProcess::MergedChannels);

    QObject::connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     &proc, [&](int exitCode, QProcess::ExitStatus) {
        (void)exitCode;  // TODO: report exit code to caller
        output = QString::fromLocal8Bit(proc.readAll());
    });

    proc.start();
    if (!proc.waitForStarted(5000)) {
        errorMsg = QStringLiteral("Failed to start command: %1").arg(command);
        return -1;
    }
    if (!proc.waitForFinished(timeoutMs > 0 ? timeoutMs : 30000)) {
        proc.kill();
        errorMsg = QStringLiteral("Command timed out: %1").arg(command);
        return -1;
    }
    output = QString::fromLocal8Bit(proc.readAll());
    errorMsg.clear();
    return proc.exitCode();
}

// ============================================================================
// Encoding Conversion
// ============================================================================

QString NppIO::decodeToQString(const QByteArray& bytes, EncodingType encoding) const
{
    QStringConverter::Encoding enc = QStringConverter::Utf8;
    switch (encoding) {
    case EncodingType::UTF_8:
    case EncodingType::UTF_8_BOM:
        enc = QStringConverter::Utf8;
        break;
    case EncodingType::UTF_16_LE:
        enc = QStringConverter::Utf16;
        break;
    case EncodingType::UTF_16_BE:
        enc = QStringConverter::Utf16;
        break;
    case EncodingType::UTF_32_LE:
        enc = QStringConverter::Utf32;
        break;
    case EncodingType::UTF_32_BE:
        enc = QStringConverter::Utf32;
        break;
    case EncodingType::ANSI:
    default:
        // For ANSI, try system locale; fall back to Latin-1
        enc = QStringConverter::System;
        break;
    }
    QStringDecoder decoder(enc);
    return decoder.decode(bytes);
}

QByteArray NppIO::encodeFromQString(const QString& text, EncodingType encoding, bool addBom) const
{
    QStringConverter::Encoding enc = QStringConverter::Utf8;
    switch (encoding) {
    case EncodingType::UTF_8:
    case EncodingType::ANSI:
        enc = QStringConverter::Utf8;
        break;
    case EncodingType::UTF_8_BOM:
        enc = QStringConverter::Utf8;
        break;
    case EncodingType::UTF_16_LE:
        enc = QStringConverter::Utf16;
        break;
    case EncodingType::UTF_16_BE:
        enc = QStringConverter::Utf16;
        break;
    case EncodingType::UTF_32_LE:
        enc = QStringConverter::Utf32;
        break;
    case EncodingType::UTF_32_BE:
        enc = QStringConverter::Utf32;
        break;
    default:
        enc = QStringConverter::System;
        break;
    }
    QStringEncoder encoder(enc);
    QByteArray result = encoder.encode(text);

    if (addBom) {
        switch (encoding) {
        case EncodingType::UTF_8_BOM:
            result.prepend("\xEF\xBB\xBF");
            break;
        case EncodingType::UTF_16_LE:
            result.prepend("\xFF\xFE");
            break;
        case EncodingType::UTF_16_BE:
            result.prepend("\xFE\xFF");
            break;
        default:
            break;
        }
    }
    return result;
}

QString NppIO::normalizeEOL(const QString& text, EolType eol) const
{
    QString eolStr;
    switch (eol) {
    case EolType::EOL_CRLF:
        eolStr = "\r\n";
        break;
    case EolType::EOL_LF:
        eolStr = "\n";
        break;
    case EolType::EOL_CR:
        eolStr = "\r";
        break;
    default:
        return text; // No conversion needed
    }

    QString result = text;
    // First normalize all to \n, then convert to target
    result.replace("\r\n", "\n");
    result.replace("\r", "\n");
    // Now convert \n to target
    result.replace("\n", eolStr);
    return result;
}

// ============================================================================
// FileWatchThread (mirrors Win32 monitorFileOnChange thread)
// ============================================================================

FileWatchThread::FileWatchThread(const QString& filePath, QObject* parent)
    : QThread(parent), _filePath(filePath)
{
    QFileInfo fi(filePath);
    _parentDir = fi.absolutePath();
}

FileWatchThread::~FileWatchThread() {
    requestStop();
    wait(3000);
}

void FileWatchThread::requestStop() {
    _stopRequested = true;
}

bool FileWatchThread::isRunning() const {
    return _isRunning && !_stopRequested;
}

bool FileWatchThread::detectFileChange(const QString& filePath, const QDateTime& lastKnownMtime) const {
    QFileInfo fi(filePath);
    if (!fi.exists()) return true;
    return fi.lastModified() != lastKnownMtime || fi.size() != _lastKnownSize;
}

void FileWatchThread::sleepWithInterrupt(int ms) {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    while (!_stopRequested && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void FileWatchThread::run() {
    _isRunning = true;
    QFileInfo fi(_filePath);
    if (fi.exists()) {
        _lastKnownMtime = fi.lastModified();
        _lastKnownSize = fi.size();
    }

    QFileSystemWatcher watcher;
    watcher.addPath(_parentDir);

    while (!_stopRequested) {
        // Poll for changes every 250ms
        QThread::sleep(1); // seconds
        // Use QTimer-based polling via QFileSystemWatcher fileChanged signal instead
        // Since we can't use waitForChanged in Qt 6.4, just poll file mtime directly
        if (detectFileChange(_filePath, _lastKnownMtime)) {
            if (QFileInfo::exists(_filePath)) {
                _lastKnownMtime = QFileInfo(_filePath).lastModified();
                _lastKnownSize = QFileInfo(_filePath).size();
                emit fileModified(_filePath);
            } else {
                emit fileDeleted(_filePath);
            }
        }
    }
    _isRunning = false;
}

// ============================================================================
// ScopedFileLock
// ============================================================================

ScopedFileLock::ScopedFileLock(const QString& filePath)
    : _filePath(filePath)
{
    QLockFile lockFile(filePath + ".lock");
    _locked = lockFile.tryLock(100);
}

ScopedFileLock::~ScopedFileLock() {
    release();
}

void ScopedFileLock::release() {
    if (_locked) {
        QLockFile lockFile(_filePath + ".lock");
        lockFile.unlock();
        _locked = false;
    }
}

// ============================================================================
// AtomicFileWriter
// ============================================================================

AtomicFileWriter::AtomicFileWriter(const QString& filePath, QObject* parent)
    : QObject(parent), _targetPath(filePath)
{
    _saveFile = new QSaveFile(filePath, this);
}

AtomicFileWriter::~AtomicFileWriter() {
    if (_saveFile && _saveFile->isOpen())
        _saveFile->cancelWriting();
}

bool AtomicFileWriter::open(QString& errorMsg) {
    if (!_saveFile->open(QIODevice::WriteOnly)) {
        errorMsg = _saveFile->errorString();
        return false;
    }
    return true;
}

qint64 AtomicFileWriter::write(const QByteArray& data) {
    return _saveFile->write(data);
}

bool AtomicFileWriter::write(QIODevice* source, QString& errorMsg) {
    if (!_saveFile->isOpen()) {
        errorMsg = QStringLiteral("File not open");
        return false;
    }
    qint64 total = 0;
    char buf[8192];
    qint64 r;
    while ((r = source->read(buf, sizeof(buf))) > 0) {
        qint64 w = _saveFile->write(buf, r);
        if (w != r) {
            errorMsg = _saveFile->errorString();
            return false;
        }
        total += w;
    }
    return true;
}

bool AtomicFileWriter::commit(QString& errorMsg) {
    if (!_saveFile->commit()) {
        errorMsg = _saveFile->errorString();
        return false;
    }
    return true;
}

void AtomicFileWriter::cancel() {
    _saveFile->cancelWriting();
}

bool AtomicFileWriter::isOpen() const {
    return _saveFile && _saveFile->isOpen();
}
