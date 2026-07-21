#pragma once
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QCryptographicHash>
#include <QMutex>
#include <QWaitCondition>
#include "../common/Types.h"

// Large file handling
struct FileLoadOptions {
    qint64 largeFileThreshold = 5 * 1024 * 1024;       // 5 MB - default large file threshold
    qint64 partialLoadSize = 50 * 1024 * 1024;        // 50 MB - load first N MB for partial load
    qint64 streamingThreshold = 100 * 1024 * 1024;     // 100 MB - use streaming for files > 100MB
    int chunkSize = 1024 * 1024;                       // 1 MB read chunks
    bool detectEncoding = true;
    bool computeChecksum = true;
    bool allowPartialLoad = true;
    bool loadRemoteFiles = true;
    bool loadFromStdin = false;
    QCryptographicHash::Algorithm checksumAlgorithm = QCryptographicHash::Sha256;
};

class FileLoaderWorker : public QObject {
    Q_OBJECT

public:
    enum class LoadSource {
        LocalFile,
        RemoteURL,
        Stdin
    };

    explicit FileLoaderWorker(const QString& path, QObject* parent = nullptr);
    explicit FileLoaderWorker(const QUrl& url, QObject* parent = nullptr);
    explicit FileLoaderWorker(QObject* parent = nullptr);
    ~FileLoaderWorker() override;

    // Configuration
    void setPath(const QString& path) { _path = path; _source = LoadSource::LocalFile; }
    void setUrl(const QUrl& url) { _url = url; _source = LoadSource::RemoteURL; }
    void setEncoding(const QString& encoding) { _requestedEncoding = encoding; }
    void setOptions(const FileLoadOptions& options) { _options = options; }
    void setLoadFromStdin(bool value) { _loadFromStdin = value; if (value) _source = LoadSource::Stdin; }

public slots:
    // Control — declared as slots so QMetaObject::invokeMethod from a
    // different thread (FileManager::openFileAsync) can dispatch them
    // via QueuedConnection.
    void start();
    void cancel();

    // Status
    bool isRunning() const { QMutexLocker locker(&_mutex); return _running; }
    bool isCancelled() const { QMutexLocker locker(&_mutex); return _cancelled; }
    qint64 loadedBytes() const { QMutexLocker locker(&_mutex); return _loadedBytes; }
    qint64 totalBytes() const { QMutexLocker locker(&_mutex); return _totalBytes; }

signals:
    // Progress reporting
    void progress(int percent);
    void progressDetail(const QString& status, int percent, qint64 loaded, qint64 total);

    // Encoding detection (emitted from worker thread)
    void encodingDetected(EncodingType encoding);

    // Load completion
    void loadComplete(const QString& content, EncodingType encoding, bool isPartial,
                      const QString& checksum = QString(), qint64 totalSize = 0);

    // Error handling
    void loadError(const QString& error, int errorCode = -1);

    // Large file warnings
    void largeFileWarning(qint64 fileSize, qint64 loadedSize, bool isPartial);
    void memoryWarning(quint64 estimatedMemory, quint64 availableMemory);

    // Cancellation
    void cancelled();

    // Checksum (emitted during load for large files)
    void checksumProgress(const QString& checksum, int percent);

private slots:
    void onNetworkFinished();
    void onNetworkError(QNetworkReply::NetworkError error);
    void onNetworkDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    // Core loading methods
    void loadLocalFile();
    void loadRemoteFile();
    void loadFromStdinStream();
    void loadLargeFileStreaming(QFile& file, qint64 totalSize);

    // Encoding detection
    EncodingType detectEncoding(const QByteArray& data) const;
    EncodingType detectEncodingFromData(const QByteArray& sample) const;
    QString encodingToString(EncodingType enc) const;

    // File analysis
    bool isBinaryFile(const QByteArray& data) const;
    qint64 estimateFileSize(const QString& path) const;

    // Decoding
    QString decodeContent(const QByteArray& data, EncodingType encoding) const;

    // Checksum
    QString computeChecksum(const QByteArray& data) const;
    QString computeStreamingChecksum(QFile& file, qint64 totalSize);

    // Error handling
    void emitError(const QString& error, int code = -1);
    void emitLargeFileWarning(qint64 fileSize, qint64 loadedSize);

    // Thread-safe state
    mutable QMutex _mutex;
    bool _running = false;
    bool _cancelled = false;

    // Source
    LoadSource _source = LoadSource::LocalFile;
    QString _path;
    QUrl _url;

    // Options
    FileLoadOptions _options;
    QString _requestedEncoding;
    bool _loadFromStdin = false;

    // Progress
    qint64 _loadedBytes = 0;
    qint64 _totalBytes = 0;
    int _lastReportedPercent = -1;

    // Network
    QNetworkAccessManager* _networkManager = nullptr;
    QNetworkReply* _networkReply = nullptr;

    // Stdin buffer
    QByteArray _stdinBuffer;
};
