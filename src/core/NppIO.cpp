// NppIO.cpp - File I/O, backup, monitoring, and encoding utilities for Notepad--Qt
// Copyright (C) 2026 Agent Army
// Copyright (C) 2021 Don HO <don.h@free.fr>
// GPL v3
// Semantic lift: Win32 CReadDirectoryChanges / CReadFileChanges → QFileSystemWatcher

#include "NppIO.h"
#include "Buffer.h"
#include "FileManager.h"
#include "EncodingDetector.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QSaveFile>
#include <QByteArray>
#include <QBitArray>
#include <QDateTime>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QSettings>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QDataStream>
#include <QDebug>
#include <algorithm>
#include <cctype>
#include <unordered_map>

static const char UTF8_BOM[3]  = {'\xEF', '\xBB', '\xBF'};
static const char UTF16LE_BOM[2] = {'\xFF', '\xFE'};
static const char UTF16BE_BOM[2] = {'\xFE', '\xFF'};

static QMutex s_monitorMapMutex;
static std::unordered_map<QString, QFileSystemWatcher*> s_watchers;

// ============================================================================
// Construction / Lifecycle
// ============================================================================

NppIO::NppIO(QObject* parent)
    : QObject(parent)
{
}

NppIO::~NppIO() {
    stopAllMonitoring();
}

// ============================================================================
// File Monitoring (QFileSystemWatcher-based)
// ============================================================================

bool NppIO::startMonitoring(const QString& filePath, Buffer* buffer) {
    if (filePath.isEmpty())
        return false;

    QMutexLocker locker(&s_monitorMapMutex);

    // Already monitoring this path?
    if (s_watchers.contains(filePath)) {
        return true;
    }

    QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
    watcher->addPath(filePath);

    if (watcher->files().isEmpty() && !QFileInfo::exists(filePath)) {
        delete watcher;
        return false;
    }

    connect(watcher, &QFileSystemWatcher::fileChanged,
            this, &NppIO::onFileChanged, Qt::UniqueConnection);

    s_watchers.insert({filePath, watcher});
    return true;
}

void NppIO::stopMonitoring(const QString& filePath) {
    QMutexLocker locker(&s_monitorMapMutex);
    auto it = s_watchers.find(filePath);
    if (it != s_watchers.end()) {
        it->second->deleteLater();
        s_watchers.erase(it);
    }
}

void NppIO::stopAllMonitoring() {
    QMutexLocker locker(&s_monitorMapMutex);
    for (auto& pair : s_watchers) {
        pair.second->deleteLater();
    }
    s_watchers.clear();
}

bool NppIO::isMonitoring(const QString& filePath) const {
    QMutexLocker locker(&s_monitorMapMutex);
    return s_watchers.contains(filePath);
}

void NppIO::onFileChanged(const QString& path) {
    // path is the file that changed — re-emit with richer signal
    if (QFileInfo::exists(path)) {
        emit fileChanged(path);
    } else {
        emit fileDeleted(path);
    }
}

void NppIO::fileChanged(const QString& filePath) {
    // Default handler: subclasses or external handlers can react
    qDebug() << "[NppIO] fileChanged:" << filePath;
}

void NppIO::fileDeleted(const QString& filePath) {
    qDebug() << "[NppIO] fileDeleted:" << filePath;
    stopMonitoring(filePath);
}

void NppIO::monitoringError(const QString& filePath, const QString& error) {
    qWarning() << "[NppIO] monitoringError:" << filePath << error;
}

// ============================================================================
// File Read / Write
// ============================================================================

QByteArray NppIO::readFile(const QString& filePath,
                           QString* errorMsg) const
 open file for reading: %1\n%2")
                            .arg(filePath, file.errorString());
        }
        return {};
    }
    return file.readAll();
}

bool NppIO::readFileStreaming(const QString& filePath,
                              std::function<bool(const QByteArray&, qint64 offset)> callback,
                              qint64 maxChunkSize) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    qint64 offset = 0;
    QByteArray chunk;
    chunk.reserve(static_cast<int>(maxChunkSize));

    while (!file.atEnd()) {
        chunk = file.read(maxChunkSize);
        if (chunk.isEmpty())
            break;
        if (!callback(chunk, offset))
            return false;
        offset += chunk.size();
    }
    return true;
}

bool NppIO::writeFile(const QString& filePath,
                      const QByteArray& data,
                      QString* errorMsg) const
{
    QSaveFile sf(filePath);
    if (!sf.open(QIODevice::WriteOnly)) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("Cannot open file for writing: %1\n%2")
                            .arg(filePath, sf.errorString());
        }
        return false;
    }
    qint64 written = sf.write(data);
    if (written < 0 || !sf.commit()) {
        if (errorMsg)
            *errorMsg = sf.errorString();
        return false;
    }
    return true;
}

bool NppIO::writeTextFile(const QString& filePath,
                          const QString& text,
                          EncodingType encoding,
                          EolType eol,
                          bool addBom,
                          QString* errorMsg) const
{
    QByteArray encoded = encodeFromQString(text, encoding, addBom);
    return writeFile(filePath, encoded, errorMsg);
}

// ============================================================================
// File Attribute Queries
// ============================================================================

qint64 NppIO::fileSize(const QString& filePath) const {
    return QFileInfo(filePath).size();
}

QDateTime NppIO::fileModificationTime(const QString& filePath) const {
    return QFileInfo(filePath).lastModified();
}

QDateTime NppIO::fileLastAccessTime(const QString& filePath) const {
    return QFileInfo(filePath).lastRead();
}

QDateTime NppIO::fileCreationTime(const QString& filePath) const {
    return QFileInfo(filePath).birthTime();
}

bool NppIO::setFileModificationTime(const QString& filePath, const QDateTime& mtime) const {
    QFile file(filePath);
    return file.open(QIODevice::ReadWrite)
        && file.setFileTime(mtime, QFileDevice::FileModificationTime);
}

bool NppIO::fileExists(const QString& filePath) const {
    return QFileInfo::exists(filePath);
}

bool NppIO::isDirectory(const QString& path) const {
    return QFileInfo(path).isDir();
}

bool NppIO::isReadOnly(const QString& filePath) const {
    return QFileInfo(filePath).isReadOnly();
}

bool NppIO::pathExists(const QString& path) const {
    return QFileInfo::exists(path);
}

QString NppIO::canonicalPath(const QString& filePath) const {
    return QFileInfo(filePath).canonicalFilePath();
}

QString NppIO::parentDirectory(const QString& filePath) const {
    QFileInfo fi(filePath);
    return fi.absolutePath();
}

QString NppIO::fileName(const QString& filePath) const {
    return QFileInfo(filePath).fileName();
}

QString NppIO::fileExtension(const QString& filePath) const {
    return QFileInfo(filePath).suffix();
}

// ============================================================================
// Backup Management
// ============================================================================

static QString makeBackupPath(const QString& originalPath, const QString& suffix) {
    QFileInfo fi(originalPath);
    return fi.absolutePath() + QDir::separator() + fi.completeBaseName()
           + suffix + "." + fi.suffix();
}

bool NppIO::createSimpleBackup(const QString& originalPath, QString& errorMsg) const {
    QString backupPath = makeBackupPath(originalPath, ".bak");
    if (QFile::exists(backupPath)) {
        QFile::remove(backupPath);
    }
    if (!QFile::copy(originalPath, backupPath)) {
        errorMsg = QStringLiteral("Failed to create backup: %1").arg(backupPath);
        return false;
    }
    return true;
}

bool NppIO::createVerboseBackup(const QString& originalPath, QString& errorMsg) const {
    QString timestamp = QDateTime::currentDateTime()
                           .toString(QStringLiteral("_yyyy-MM-dd_HH-mm-ss"));
    QString backupPath = makeBackupPath(originalPath, timestamp);
    if (!QFile::copy(originalPath, backupPath)) {
        errorMsg = QStringLiteral("Failed to create verbose backup: %1").arg(backupPath);
        return false;
    }
    return true;
}

bool NppIO::createNumberedBackup(const QString& originalPath,
                                 int maxBackups,
                                 QString& errorMsg) const
{
    QFileInfo fi(originalPath);
    QString base = fi.absolutePath() + QDir::separator() + fi.completeBaseName();
    QString ext  = fi.suffix();

    // Rotate: file.ext.N → file.ext.N+1
    for (int i = maxBackups - 1; i >= 1; --i) {
        QString src = QStringLiteral("%1.%2.%3").arg(base, QString::number(i), ext);
        QString dst = QStringLiteral("%1.%2.%3").arg(base, QString::number(i + 1), ext);
        if (QFile::exists(src)) {
            QFile::remove(dst);
            QFile::rename(src, dst);
        }
    }

    QString first = QStringLiteral("%1.1.%2").arg(base, ext);
    if (QFile::exists(first))
        QFile::remove(first);
    if (!QFile::copy(originalPath, first)) {
        errorMsg = QStringLiteral("Failed to create numbered backup: %1").arg(first);
        return false;
    }
    return true;
}

bool NppIO::createBackup(const QString& originalPath,
                          BackupMode mode,
                          int maxBackups,
                          QString& errorMsg) const
{
    switch (mode) {
    case BackupMode::Simple:
        return createSimpleBackup(originalPath, errorMsg);
    case BackupMode::Verbose:
        return createVerboseBackup(originalPath, errorMsg);
    case BackupMode::Numbered:
        return createNumberedBackup(originalPath, maxBackups, errorMsg);
    default:
        return true;
    }
}

bool NppIO::rotateNumberedBackups(const QString& originalPath,
                                   int maxBackups) const
{
    QString dummy;
    return createNumberedBackup(originalPath, maxBackups, dummy);
}

// ============================================================================
// File Operations
// ============================================================================

bool NppIO::copyFile(const QString& source,
                     const QString& dest,
                     QString* errorMsg) const
1 → %2").arg(source, dest);
        return false;
    }
    return true;
}

bool NppIO::renameFile(const QString& oldPath,
                       const QString& newPath,
                       QString* errorMsg) const
{
    if (QFile::exists(newPath) && !QFile::remove(newPath)) {
        if (errorMsg)
            *errorMsg = QStringLiteral("Cannot overwrite existing file: %1").arg(newPath);
        return false;
    }
    if (!QFile::rename(oldPath, newPath)) {
        if (errorMsg)
            *errorMsg = QStringLiteral("Failed to rename %1 → %2: %3")
                            .arg(oldPath, newPath, QFile(newPath).errorString());
        return false;
    }
    return true;
}

bool NppIO::deleteFile(const QString& filePath,
                       QString* errorMsg) const
{
    if (!QFile::remove(filePath)) {
        if (errorMsg)
            *errorMsg = QStringLiteral("Failed to delete file: %1").arg(filePath);
        return false;
    }
    return true;
}

bool NppIO::createDirectory(const QString& path, QString& errorMsg) const {
    QDir d;
    if (!d.mkpath(path)) {
        errorMsg = QStringLiteral("Failed to create directory: %1").arg(path);
        return false;
    }
    return true;
}

bool NppIO::removeDirectory(const QString& path, QString& errorMsg) const {
    QDir d(path);
    if (!d.removeRecursively()) {
        errorMsg = QStringLiteral("Failed to remove directory: %1").arg(path);
        return false;
    }
    return true;
}

bool NppIO::replaceFile(const QString& replacementPath,
                        const QString& targetPath,
                        QString* errorMsg) const
{
    // Atomic replace: copy replacement over target
    if (!QFile::exists(replacementPath)) {
        if (errorMsg)
            *errorMsg = QStringLiteral("Replacement file does not exist: %1").arg(replacementPath);
        return false;
    }
    // Remove target first
    if (QFile::exists(targetPath) && !QFile::remove(targetPath)) {
        if (errorMsg)
            *errorMsg = QStringLiteral("Cannot remove target file: %1").arg(targetPath);
        return false;
    }
    if (!QFile::copy(replacementPath, targetPath)) {
        if (errorMsg)
            *errorMsg = QStringLiteral("Failed to copy replacement to target");
        return false;
    }
    return true;
}

bool NppIO::ensureDirectoryExists(const QString& path, QString& errorMsg) const {
    QDir d;
    if (!d.exists(path) && !d.mkpath(path)) {
        errorMsg = QStringLiteral("Failed to ensure directory exists: %1").arg(path);
        return false;
    }
    return true;
}

// ============================================================================
// Path Utilities
// ============================================================================

QString NppIO::expandEnvironmentStrings(const QString& input) const {
    QString result = input;
    // Qt doesn't have direct env var expansion, do simple %VAR% replacement
    QRegularExpression envRe(R"(%([^%]+)%)");
    result.replace(envRe, [](const QString& match) {
        QString varName = match.mid(1, match.length() - 2);
        QString value = qgetenv(varName.toLatin1().constData());
        return value.isEmpty() ? match : value;
    });
    return result;
}

QString NppIO::absolutePath(const QString& path) const {
    QFileInfo fi(path);
    return fi.absoluteFilePath();
}

QString NppIO::applicationFilePath() const {
    return QCoreApplication::applicationFilePath();
}

QString NppIO::applicationDirPath() const {
    return QCoreApplication::applicationDirPath();
}

QString NppIO::applicationFilePath() {
    return QCoreApplication::applicationFilePath();
}

QString NppIO::applicationDirPath() {
    return QCoreApplication::applicationDirPath();
}

// ============================================================================
// External Programs / Shell
// ============================================================================

bool NppIO::openExternally(const QString& pathOrUrl) const {
    QUrl url = QUrl::fromUserInput(pathOrUrl);
    return QDesktopServices::openUrl(url);
}

bool NppIO::openContainingFolder(const QString& filePath) const {
    QFileInfo fi(filePath);
    QString dir = fi.absolutePath();
    return QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

bool NppIO::launchElevated(const QString& programPath,
                           const QString& arguments,
                           QString* errorMsg) const
{
    // On Linux, use pkexec for elevation
    QStringList args;
    args << programPath;
    if (!arguments.isEmpty())
        args << arguments;

    QProcess p;
    p.setProgram("pkexec");
    p.setArguments(args);

    if (errorMsg) {
        // Start synchronously for result
        bool ok = p.execute(programPath, arguments.isEmpty() ? QStringList() : QStringList{arguments}) >= 0;
        if (!ok)
            *errorMsg = p.errorString();
        return ok;
    } else {
        p.startDetached();
        return true;
    }
}

int NppIO::runCommand(const QString& command,
                       const QString& workingDir,
                       QString* output,
                       QString* errorMsg,
                       int timeoutMs) const
{
    QProcess p;
    if (!workingDir.isEmpty())
        p.setWorkingDirectory(workingDir);

    p.setProgram("sh");
    p.setArguments({"-c", command});

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    p.setProcessEnvironment(env);

    p.start();

    if (!p.waitForStarted()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("Failed to start command: %1").arg(command);
        return -1;
    }

    bool finished = p.waitForFinished(timeoutMs > 0 ? timeoutMs : 30000);

    if (!finished) {
        p.kill();
        if (errorMsg)
            *errorMsg = "Process timed out";
        return -1;
    }

    if (output)
        *output = QString::fromLocal8Bit(p.readAllStandardOutput());
    if (errorMsg) {
        QString err = QString::fromLocal8Bit(p.readAllStandardError());
        if (!err.isEmpty())
            *errorMsg = err;
    }

    return p.exitCode();
}

// ============================================================================
// Encoding / BOM Utilities
// ============================================================================

EncodingType NppIO::detectEncodingBOM(const QByteArray& rawBytes) const {
    if (rawBytes.size() >= 3 &&
        rawBytes[0] == UTF8_BOM[0] &&
        rawBytes[1] == UTF8_BOM[1] &&
        rawBytes[2] == UTF8_BOM[2]) {
        return EncodingType::UTF8_BOM;
    }
    if (rawBytes.size() >= 2 &&
        rawBytes[0] == UTF16LE_BOM[0] &&
        rawBytes[1] == UTF16LE_BOM[1]) {
        return EncodingType::UTF16_LE_BOM;
    }
    if (rawBytes.size() >= 2 &&
        rawBytes[0] == UTF16BE_BOM[0] &&
        rawBytes[1] == UTF16BE_BOM[1]) {
        return EncodingType::UTF16_BE_BOM;
    }
    return EncodingType::UTF_8;
}

QByteArray NppIO::stripBOM(const QByteArray& rawBytes) const {
    EncodingType bom = detectEncodingBOM(rawBytes);
    if (bom == EncodingType::UTF8_BOM)
        return rawBytes.mid(3);
    if (bom == EncodingType::UTF16_LE_BOM || bom == EncodingType::UTF16_BE_BOM)
        return rawBytes.mid(2);
    return rawBytes;
}

bool NppIO::hasUtf8BOM(const QByteArray& rawBytes) const {
    return detectEncodingBOM(rawBytes) == EncodingType::UTF8_BOM;
}

bool NppIO::hasUtf16LEBOM(const QByteArray& rawBytes) const {
    return detectEncodingBOM(rawBytes) == EncodingType::UTF16_LE_BOM;
}

bool NppIO::hasUtf16BEBOM(const QByteArray& rawBytes) const {
    return detectEncodingBOM(rawBytes) == EncodingType::UTF16_BE_BOM;
}

QString NppIO::decodeToQString(const QByteArray& bytes, EncodingType encoding) const {
    QTextCodec* codec = QTextCodec::codecForName(EncodingDetector::qtEncodingName(encoding));
    if (!codec)
        codec = QTextCodec::codecForName("UTF-8");
    return codec->toUnicode(bytes);
}

QByteArray NppIO::encodeFromQString(const QString& text, EncodingType encoding, bool addBom) const {
    QTextCodec* codec = QTextCodec::codecForName(EncodingDetector::qtEncodingName(encoding));
    if (!codec)
        codec = QTextCodec::codecForName("UTF-8");
    QByteArray result = codec->fromUnicode(text);
    if (addBom) {
        switch (encoding) {
        case EncodingType::UTF8_BOM:
            result.prepend(QByteArray(UTF8_BOM, 3));
            break;
        case EncodingType::UTF16_LE_BOM:
            result.prepend(QByteArray(UTF16LE_BOM, 2));
            break;
        case EncodingType::UTF16_BE_BOM:
            result.prepend(QByteArray(UTF16BE_BOM, 2));
            break;
        default:
            break;
        }
    }
    return result;
}

QString NppIO::normalizeEOL(const QString& text, EolType eol) const {
    QString eolStr = [eol]() -> QString {
        switch (eol) {
        case EolType::EOL_CRLF: return QStringLiteral("\r\n");
        case EolType::EOL_LF:   return QStringLiteral("\n");
        case EolType::EOL_CR:   return QStringLiteral("\r");
        default:                return QStringLiteral("\n");
        }
    }();

    QString result = text;
    // Replace CRLF → \n first, then \r → \n, then \n → eolStr
    result.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    result.replace('\r', '\n');
    result.replace('\n', eolStr);
    return result;
}
