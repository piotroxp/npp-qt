#include "FileLoaderWorker.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QRegularExpression>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

// MIME type signatures for binary detection
static const char* const BINARY_SIGNATURES[] = {
    "\x89PNG",      // PNG
    "\xff\xd8\xff", // JPEG
    "GIF87a",       // GIF87a
    "GIF89a",       // GIF89a
    "RIFF",         // RIFF (AVI, WAV, WEBP)
    "OggS",         // OGG
    "%PDF",         // PDF
    "PK\x03\x04",   // ZIP, DOCX, XLSX
    "\x1f\x8b",     // GZIP
    "BZh",          // BZIP2
    "Rar!",         // RAR
    "\x00\x00\x01\x00", // ICO
    "\x00\x00\x02\x00", // CUR
    "BM",           // BMP
    "II\x2a\x00",   // TIFF little-endian
    "MM\x00\x2a",   // TIFF big-endian
    "\xCA\xFE\xBA\xBE", // Java class / Mach-O
    "\xfe\xed\xfa\xce", // Mach-O 32-bit
    "\xfe\xed\xfa\xcf", // Mach-O 64-bit
    "\x7fELF",       // ELF
    "MZ",           // Windows EXE/DLL
    nullptr
};

// Null bytes per sample size threshold for binary detection
static constexpr int BINARY_NULL_THRESHOLD = 32;
static constexpr int BINARY_SAMPLE_SIZE = 8192;

FileLoaderWorker::FileLoaderWorker(const QString& path, QObject* parent)
    : QObject(parent), _source(LoadSource::LocalFile), _path(path) {}

FileLoaderWorker::FileLoaderWorker(const QUrl& url, QObject* parent)
    : QObject(parent), _source(LoadSource::RemoteURL), _url(url) {}

FileLoaderWorker::FileLoaderWorker(QObject* parent)
    : QObject(parent), _source(LoadSource::Stdin), _loadFromStdin(true) {}

FileLoaderWorker::~FileLoaderWorker() {
    cancel();
    if (_networkManager) {
        _networkManager->deleteLater();
    }
}

// =============================================================================
// Entry point
// =============================================================================

void FileLoaderWorker::start() {
    QMutexLocker locker(&_mutex);
    if (_running) {
        emitError("File loader is already running", -2);
        return;
    }
    _running = true;
    _cancelled = false;
    _loadedBytes = 0;
    _lastReportedPercent = -1;
    locker.unlock();

    switch (_source) {
        case LoadSource::LocalFile:
            QTimer::singleShot(0, this, &FileLoaderWorker::loadLocalFile);
            break;
        case LoadSource::RemoteURL:
            if (_options.loadRemoteFiles) {
                QTimer::singleShot(0, this, &FileLoaderWorker::loadRemoteFile);
            } else {
                emitError("Remote file loading is disabled", -1);
                return;
            }
            break;
        case LoadSource::Stdin:
            QTimer::singleShot(0, this, &FileLoaderWorker::loadFromStdinStream);
            break;
    }
}

void FileLoaderWorker::cancel() {
    QMutexLocker locker(&_mutex);
    _cancelled = true;
    if (_networkReply && !_networkReply->isFinished()) {
        _networkReply->abort();
    }
}

// =============================================================================
// Local file loading
// =============================================================================

void FileLoaderWorker::loadLocalFile() {
    if (_path.isEmpty()) {
        emitError("No file path specified", -1);
        return;
    }

    QFileInfo fileInfo(_path);
    if (!fileInfo.exists()) {
        emitError(QString("File not found: %1").arg(_path), 404);
        return;
    }
    if (!fileInfo.isFile()) {
        emitError(QString("Not a regular file: %1").arg(_path), -1);
        return;
    }
    if (!fileInfo.isReadable()) {
        emitError(QString("File is not readable: %1").arg(_path), -1);
        return;
    }

    const qint64 totalSize = fileInfo.size();
    {
        QMutexLocker locker(&_mutex);
        _totalBytes = totalSize;
    }

    // Memory safety check
    constexpr quint64 MEMORY_SAFETY_LIMIT = static_cast<quint64>(1) << 30; // 1 GB
    if (static_cast<quint64>(totalSize) > MEMORY_SAFETY_LIMIT && !_options.allowPartialLoad) {
        emit memoryWarning(static_cast<quint64>(totalSize), MEMORY_SAFETY_LIMIT);
    }

    // Large file check
    if (totalSize > _options.largeFileThreshold) {
        emit progressDetail(tr("Large file detected (%1 MB)").arg(totalSize / (1024 * 1024)), 0, 0, totalSize);
    }

    // Streaming for very large files
    if (totalSize > _options.streamingThreshold) {
        QFile file(_path);
        if (!file.open(QIODevice::ReadOnly)) {
            emitError(QString("Could not open file: %1").arg(_path), -1);
            return;
        }
        loadLargeFileStreaming(file, totalSize);
        return;
    }

    // Normal file loading with progress
    QFile file(_path);
    if (!file.open(QIODevice::ReadOnly)) {
        emitError(QString("Could not open file: %1").arg(_path), -1);
        return;
    }

    QByteArray content;
    content.reserve(static_cast<int>(qMin(totalSize, static_cast<qint64>(100 * 1024 * 1024))));

    constexpr int CHUNK = 1024 * 1024; // 1 MB
    qint64 loaded = 0;
    QByteArray chunk;
    QCryptographicHash hash(_options.checksumAlgorithm);

    while (!file.atEnd()) {
        {
            QMutexLocker locker(&_mutex);
            if (_cancelled) {
                file.close();
                emit cancelled();
                return;
            }
        }

        chunk = file.read(CHUNK);
        if (chunk.isEmpty()) break;

        content.append(chunk);
        loaded += chunk.size();

        // Update progress
        if (totalSize > 0) {
            int percent = static_cast<int>((loaded * 100) / totalSize);
            {
                QMutexLocker locker(&_mutex);
                _loadedBytes = loaded;
            }
            if (percent != _lastReportedPercent) {
                _lastReportedPercent = percent;
                emit progress(percent);
                emit progressDetail(tr("Loading..."), percent, loaded, totalSize);
            }
        }

        // Streaming checksum
        if (_options.computeChecksum) {
            hash.addData(chunk);
        }
    }
    file.close();

    {
        QMutexLocker locker(&_mutex);
        _loadedBytes = loaded;
        _running = false;
    }

    // Binary check
    if (isBinaryFile(content)) {
        // For now, still load as text but warn
        qDebug() << "Warning: file appears to be binary:" << _path;
    }

    // Encoding detection
    EncodingType encoding;
    if (!_requestedEncoding.isEmpty()) {
        encoding = detectEncoding(content);
        emit encodingDetected(encoding);
    } else {
        encoding = detectEncoding(content);
        emit encodingDetected(encoding);
    }

    // Partial load warning
    bool isPartial = (totalSize > _options.partialLoadSize && _options.allowPartialLoad);
    if (isPartial) {
        qint64 warningSize = _options.partialLoadSize;
        emitLargeFileWarning(totalSize, warningSize);
    }

    // Decode content
    QString text = decodeContent(content, encoding);
    if (text.isNull()) {
        emitError("Failed to decode file content", -1);
        return;
    }

    // Checksum
    QString checksum;
    if (_options.computeChecksum) {
        checksum = QString::fromLatin1(hash.result().toHex());
    }

    emit progress(100);
    emit loadComplete(text, encoding, isPartial, checksum, totalSize);
}

// =============================================================================
// Streaming load for very large files (> streamingThreshold)
// =============================================================================

void FileLoaderWorker::loadLargeFileStreaming(QFile& file, qint64 totalSize) {
    QByteArray chunk;
    chunk.reserve(_options.chunkSize);

    QByteArray preview; // First few KB for encoding detection
    preview.reserve(BINARY_SAMPLE_SIZE);

    QCryptographicHash hash(_options.checksumAlgorithm);
    qint64 loaded = 0;
    int lastPercent = -1;

    // Phase 1: Load preview for encoding detection
    preview = file.read(qMin(_options.chunkSize, static_cast<qint64>(BINARY_SAMPLE_SIZE)));
    if (preview.isEmpty()) {
        emitError("File is empty", -1);
        return;
    }

    // Binary check on preview
    if (isBinaryFile(preview)) {
        emitError("Binary files cannot be loaded as text", -1);
        return;
    }

    // Detect encoding from preview
    EncodingType encoding = detectEncoding(preview);
    emit encodingDetected(encoding);

    // Memory warning
    if (static_cast<quint64>(totalSize) > 500 * (1024 * 1024)) {
        emit memoryWarning(static_cast<quint64>(totalSize), static_cast<quint64>(totalSize) / 2);
    }

    // Phase 2: Streaming read for checksum + partial load
    bool partialLoad = (_options.allowPartialLoad && totalSize > _options.partialLoadSize);
    QByteArray partialContent;
    if (partialLoad) {
        partialContent.reserve(static_cast<int>(_options.partialLoadSize));
        partialContent = preview;
        loaded = preview.size();
    }

    // Restart from beginning for streaming checksum (or continue if file supports)
    file.seek(0);
    while (!file.atEnd()) {
        {
            QMutexLocker locker(&_mutex);
            if (_cancelled) {
                emit cancelled();
                return;
            }
        }

        chunk = file.read(_options.chunkSize);
        if (chunk.isEmpty()) break;

        if (_options.computeChecksum) {
            hash.addData(chunk);
        }

        loaded += chunk.size();
        int percent = static_cast<int>((loaded * 100) / totalSize);
        if (percent != lastPercent) {
            lastPercent = percent;
            emit progress(percent);
            emit progressDetail(tr("Computing checksum..."), percent, loaded, totalSize);

            // For partial load, accumulate only up to threshold
            if (partialLoad && partialContent.size() < _options.partialLoadSize) {
                qint64 room = _options.partialLoadSize - partialContent.size();
                partialContent.append(chunk.left(static_cast<int>(room)));
            }
        }
    }

    {
        QMutexLocker locker(&_mutex);
        _running = false;
    }

    QString checksum;
    if (_options.computeChecksum) {
        checksum = QString::fromLatin1(hash.result().toHex());
        emit checksumProgress(checksum, 100);
    }

    EncodingType enc = detectEncoding(partialLoad ? partialContent : preview);
    QString text = decodeContent(partialLoad ? partialContent : preview, enc);

    if (text.isNull()) {
        emitError("Failed to decode file content", -1);
        return;
    }

    if (partialLoad) {
        emitLargeFileWarning(totalSize, _options.partialLoadSize);
    }

    emit progress(100);
    emit loadComplete(text, enc, partialLoad, checksum, totalSize);
}

// =============================================================================
// Remote file loading (HTTP/HTTPS/FTP)
// =============================================================================

void FileLoaderWorker::loadRemoteFile() {
    if (!_options.loadRemoteFiles) {
        emitError("Remote file loading is disabled", -1);
        return;
    }

    if (!_url.isValid()) {
        emitError(QString("Invalid URL: %1").arg(_url.toString()), -1);
        return;
    }

    // Only HTTP(S) and FTP are supported
    QString scheme = _url.scheme().toLower();
    if (scheme != "http" && scheme != "https" && scheme != "ftp") {
        emitError(QString("Unsupported URL scheme: %1").arg(scheme), -1);
        return;
    }

    if (!_networkManager) {
        _networkManager = new QNetworkAccessManager(this);
    }

    QNetworkRequest request(_url);
    request.setRawHeader("User-Agent", "npp-qt/1.0");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                        QNetworkRequest::NoLessSafeRedirectPolicy);

    _networkReply = _networkManager->get(request);

    connect(_networkReply, &QNetworkReply::finished, this, &FileLoaderWorker::onNetworkFinished,
            Qt::DirectConnection);
    connect(_networkReply, &QNetworkReply::errorOccurred,
            this, &FileLoaderWorker::onNetworkError, Qt::DirectConnection);
    connect(_networkReply, &QNetworkReply::downloadProgress,
            this, &FileLoaderWorker::onNetworkDownloadProgress, Qt::DirectConnection);
}

void FileLoaderWorker::onNetworkFinished() {
    QMutexLocker locker(&_mutex);
    if (_cancelled) {
        emit cancelled();
        return;
    }

    if (_networkReply->error() != QNetworkReply::NoError) {
        QString err = _networkReply->errorString();
        int code = _networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        locker.unlock();
        emitError(QString("Network error: %1").arg(err), code);
        return;
    }

    QByteArray data = _networkReply->readAll();
    locker.unlock();

    if (isBinaryFile(data)) {
        emitError("Remote file appears to be binary", -1);
        return;
    }

    EncodingType encoding = detectEncoding(data);
    emit encodingDetected(encoding);

    QString text = decodeContent(data, encoding);
    if (text.isNull()) {
        emitError("Failed to decode remote file content", -1);
        return;
    }

    QString checksum;
    if (_options.computeChecksum) {
        QCryptographicHash hash(_options.checksumAlgorithm);
        hash.addData(data);
        checksum = QString::fromLatin1(hash.result().toHex());
    }

    {
        QMutexLocker l(&_mutex);
        _running = false;
    }

    emit loadComplete(text, encoding, false, checksum, data.size());
}

void FileLoaderWorker::onNetworkError(QNetworkReply::NetworkError error) {
    QMutexLocker locker(&_mutex);
    if (_cancelled) return;

    QString errMsg = _networkReply->errorString();
    int code = _networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    locker.unlock();

    emitError(QString("Network error (%1): %2").arg(static_cast<int>(error)).arg(errMsg), code);
}

void FileLoaderWorker::onNetworkDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    QMutexLocker locker(&_mutex);
    if (_cancelled) return;

    _loadedBytes = bytesReceived;
    _totalBytes = bytesTotal;

    if (bytesTotal > 0) {
        int percent = static_cast<int>((bytesReceived * 100) / bytesTotal);
        if (percent != _lastReportedPercent) {
            _lastReportedPercent = percent;
            emit progress(percent);
            emit progressDetail(tr("Downloading..."), percent, bytesReceived, bytesTotal);
        }
    }
}

// =============================================================================
// Stdin loading
// =============================================================================

void FileLoaderWorker::loadFromStdinStream() {
    // Read all from stdin
    QByteArray data;
    QFile stdinFile;
    if (stdinFile.open(stdin, QIODevice::ReadOnly)) {
        data = stdinFile.readAll();
        stdinFile.close();
    } else {
        emitError("Could not read from stdin", -1);
        return;
    }

    if (data.isEmpty()) {
        emitError("Empty input", -1);
        return;
    }

    {
        QMutexLocker locker(&_mutex);
        _totalBytes = data.size();
        _loadedBytes = data.size();
        _running = false;
    }

    if (isBinaryFile(data)) {
        qDebug() << "Warning: stdin input appears to be binary";
    }

    EncodingType encoding = detectEncoding(data);
    emit encodingDetected(encoding);

    QString text = decodeContent(data, encoding);
    if (text.isNull()) {
        emitError("Failed to decode stdin content", -1);
        return;
    }

    QString checksum;
    if (_options.computeChecksum) {
        QCryptographicHash hash(_options.checksumAlgorithm);
        hash.addData(data);
        checksum = QString::fromLatin1(hash.result().toHex());
    }

    emit loadComplete(text, encoding, false, checksum, data.size());
}

// =============================================================================
// Encoding detection
// =============================================================================

EncodingType FileLoaderWorker::detectEncoding(const QByteArray& data) const {
    if (!_options.detectEncoding || data.isEmpty()) {
        return EncodingType::UTF_8;
    }

    // Check for BOM first
    if (data.size() >= 3 && data.startsWith("\xef\xbb\xbf")) {
        return EncodingType::UTF_8_BOM;
    }
    if (data.size() >= 2) {
        if (data.startsWith("\xff\xfe")) {
            return data.size() >= 4 && data[2] == '\x00' && data[3] == '\x00'
                       ? EncodingType::UTF_32_LE : EncodingType::UTF_16_LE_BOM;
        }
        if (data.startsWith("\xfe\xff")) {
            return data.size() >= 4 && data[2] == '\x00' && data[3] == '\x00'
                       ? EncodingType::UTF_16_BE : EncodingType::UTF_16_BE_BOM;
        }
        if (data.startsWith("\xff\xfe\x00\x00")) {
            return EncodingType::UTF_32_LE;
        }
        if (data.startsWith("\x00\x00\xfe\xff")) {
            return EncodingType::UTF_16_BE;
        }
    }

    return detectEncodingFromData(data);
}

EncodingType FileLoaderWorker::detectEncodingFromData(const QByteArray& sample) const {
    const QByteArray& data = sample.isEmpty() ? QByteArray() : sample;
    const int size = data.size();

    // Sample size for analysis
    int analysisSize = qMin(size, BINARY_SAMPLE_SIZE);
    if (analysisSize == 0) return EncodingType::UTF_8;

    // Check for UTF-16 LE (null bytes interleaved with ASCII text)
    int utf16leScore = 0;
    for (int i = 0; i < analysisSize - 1; i += 2) {
        if (data[i] == '\0' && data[i + 1] != '\0') {
            utf16leScore++;
        }
    }
    if (utf16leScore > analysisSize / 4) {
        return EncodingType::UTF_16_LE;
    }

    // Check for UTF-16 BE
    int utf16beScore = 0;
    for (int i = 0; i < analysisSize - 1; i += 2) {
        if (data[i + 1] == '\0' && data[i] != '\0') {
            utf16beScore++;
        }
    }
    if (utf16beScore > analysisSize / 4) {
        return EncodingType::UTF_16_BE;
    }

    // Check for valid UTF-8
    bool validUtf8 = true;
    int nonAscii = 0;
    for (int i = 0; i < analysisSize; ++i) {
        unsigned char c = data[i];
        if (c >= 0x80) nonAscii++;

        // UTF-8 validation
        if ((c & 0x80) == 0x00) {
            // ASCII - OK
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte sequence
            if (i + 1 >= analysisSize || (data[i + 1] & 0xC0) != 0x80) {
                validUtf8 = false;
                break;
            }
            ++i;
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte sequence
            if (i + 2 >= analysisSize || (data[i + 1] & 0xC0) != 0x80 || (data[i + 2] & 0xC0) != 0x80) {
                validUtf8 = false;
                break;
            }
            i += 2;
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte sequence
            if (i + 3 >= analysisSize || (data[i + 1] & 0xC0) != 0x80 ||
                (data[i + 2] & 0xC0) != 0x80 || (data[i + 3] & 0xC0) != 0x80) {
                validUtf8 = false;
                break;
            }
            i += 3;
        } else {
            validUtf8 = false;
            break;
        }
    }

    if (validUtf8 && nonAscii > 0) {
        return EncodingType::UTF_8;
    }

    // Check for high bytes that aren't valid UTF-8
    bool allPrintable = true;
    for (int i = 0; i < analysisSize; ++i) {
        unsigned char c = data[i];
        if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
            allPrintable = false;
            break;
        }
    }

    if (allPrintable && nonAscii == 0) {
        return EncodingType::ASCII_7;
    }

    // Default to system encoding
    return EncodingType::UTF_8;
}

QString FileLoaderWorker::encodingToString(EncodingType enc) const {
    switch (enc) {
        case EncodingType::UTF_8:      return QStringLiteral("UTF-8");
        case EncodingType::UTF_8_BOM:   return QStringLiteral("UTF-8 BOM");
        case EncodingType::UTF_16_LE:   return QStringLiteral("UTF-16 LE");
        case EncodingType::UTF_16_BE:   return QStringLiteral("UTF-16 BE");
        case EncodingType::UTF_16_LE_BOM: return QStringLiteral("UTF-16 LE BOM");
        case EncodingType::UTF_16_BE_BOM: return QStringLiteral("UTF-16 BE BOM");
        case EncodingType::UTF_32_LE:   return QStringLiteral("UTF-32 LE");
        case EncodingType::UTF_32_BE:   return QStringLiteral("UTF-32 BE");
        case EncodingType::ASCII_7:     return QStringLiteral("ASCII");
        case EncodingType::ISO88591:  return QStringLiteral("ISO-8859-1");
        case EncodingType::Windows1252: return QStringLiteral("Windows-1252");
        case EncodingType::Other:    return QStringLiteral("Other");
        default:                      return QStringLiteral("Unknown");
    }
}

// =============================================================================
// Content decoding
// =============================================================================

QString FileLoaderWorker::decodeContent(const QByteArray& data, EncodingType encoding) const {
    switch (encoding) {
        case EncodingType::UTF_8:
        case EncodingType::UTF_8_BOM:
        case EncodingType::ASCII_7:
            return QString::fromUtf8(data);

        case EncodingType::UTF_16_LE:
        case EncodingType::UTF_16_LE_BOM:
            return QString::fromUtf16(reinterpret_cast<const char16_t*>(data.constData()),
                                      data.size() / 2);

        case EncodingType::UTF_16_BE:
        case EncodingType::UTF_16_BE_BOM: {
            // Swap bytes for big-endian
            QByteArray swapped = data;
            for (int i = 0; i < swapped.size() - 1; i += 2) {
                char tmp = swapped[i];
                swapped[i] = swapped[i + 1];
                swapped[i + 1] = tmp;
            }
            return QString::fromUtf16(reinterpret_cast<const char16_t*>(static_cast<const void*>(swapped.constData())),
                                      swapped.size() / 2);
        }

        case EncodingType::UTF_32_LE:
            return QString::fromUcs4(reinterpret_cast<const char32_t*>(data.constData()),
                                     data.size() / 4);

        case EncodingType::UTF_32_BE: {
            QByteArray swapped = data;
            for (int i = 0; i < swapped.size() - 3; i += 4) {
                qSwap(swapped[i], swapped[i + 3]);
                qSwap(swapped[i + 1], swapped[i + 2]);
            }
            return QString::fromUcs4(reinterpret_cast<const char32_t*>(static_cast<const void*>(swapped.constData())),
                                     swapped.size() / 4);
        }

        case EncodingType::ISO88591:
            return QString::fromLatin1(data);

        case EncodingType::Windows1252:
            // Fall through - treat as Latin1 with Windows-1252 specific chars
            return QString::fromLatin1(data);

        default:
            // Try UTF-8 first, fall back to local 8-bit
            QString result = QString::fromUtf8(data);
            if (result.isNull() || result.contains(QChar(0xFFFD))) {
                result = QString::fromLocal8Bit(data);
            }
            return result;
    }
}

// =============================================================================
// Binary file detection
// =============================================================================

bool FileLoaderWorker::isBinaryFile(const QByteArray& data) const {
    if (data.isEmpty()) return false;

    int checkSize = qMin(data.size(), BINARY_SAMPLE_SIZE);

    // Check for known binary signatures
    for (int i = 0; BINARY_SIGNATURES[i] != nullptr; ++i) {
        int sigLen = qstrlen(BINARY_SIGNATURES[i]);
        if (checkSize >= sigLen && data.startsWith(BINARY_SIGNATURES[i])) {
            return true;
        }
    }

    // Check for excessive null bytes (strong indicator of binary)
    int nullCount = 0;
    for (int i = 0; i < checkSize; ++i) {
        if (data[i] == '\0') nullCount++;
    }
    if (nullCount > checkSize / 8) { // More than 12.5% null bytes
        return true;
    }

    // Check for high ratio of non-printable, non-whitespace characters
    int nonPrintable = 0;
    for (int i = 0; i < checkSize; ++i) {
        unsigned char c = data[i];
        if ((c < 32 && c != '\t' && c != '\n' && c != '\r') || c == 127) {
            nonPrintable++;
        }
    }
    if (nonPrintable > checkSize / 10) { // More than 10% non-printable
        return true;
    }

    return false;
}

qint64 FileLoaderWorker::estimateFileSize(const QString& path) const {
    QFileInfo info(path);
    return info.size();
}

// =============================================================================
// Checksum
// =============================================================================

QString FileLoaderWorker::computeChecksum(const QByteArray& data) const {
    QCryptographicHash hash(_options.checksumAlgorithm);
    hash.addData(data);
    return QString::fromLatin1(hash.result().toHex());
}

QString FileLoaderWorker::computeStreamingChecksum(QFile& file, qint64 totalSize) {
    QCryptographicHash hash(_options.checksumAlgorithm);
    QByteArray chunk;
    chunk.reserve(_options.chunkSize);

    qint64 processed = 0;
    int lastPercent = -1;

    file.seek(0);
    while (!file.atEnd()) {
        {
            QMutexLocker locker(&_mutex);
            if (_cancelled) {
                return QString();
            }
        }

        chunk = file.read(_options.chunkSize);
        if (chunk.isEmpty()) break;

        hash.addData(chunk);
        processed += chunk.size();

        if (totalSize > 0) {
            int percent = static_cast<int>((processed * 100) / totalSize);
            if (percent != lastPercent) {
                lastPercent = percent;
                emit checksumProgress(QString::fromLatin1(hash.result().toHex()), percent);
            }
        }
    }

    return QString::fromLatin1(hash.result().toHex());
}

// =============================================================================
// Error & warning helpers
// =============================================================================

void FileLoaderWorker::emitError(const QString& error, int code) {
    QMutexLocker locker(&_mutex);
    _running = false;
    locker.unlock();
    emit loadError(error, code);
}

void FileLoaderWorker::emitLargeFileWarning(qint64 fileSize, qint64 loadedSize) {
    emit largeFileWarning(fileSize, loadedSize, true);
}
