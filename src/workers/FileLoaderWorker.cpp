#include "FileLoaderWorker.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

FileLoaderWorker::FileLoaderWorker(const QString& path, QObject* parent)
    : QObject(parent), _path(path) {}

void FileLoaderWorker::start() {
    QFile f(_path);
    QFileInfo info(f);

    if (!f.open(QIODevice::ReadOnly)) {
        emit finished(false, QString(), "Could not open file");
        return;
    }

    qint64 totalSize = info.size();
    constexpr qint64 CHUNK_SIZE = 1024 * 1024; // 1MB
    qint64 loaded = 0;
    QByteArray content;

    while (!f.atEnd()) {
        QByteArray chunk = f.read(CHUNK_SIZE);
        content.append(chunk);
        loaded += chunk.size();
        if (totalSize > 0) {
            int pct = static_cast<int>((loaded * 100) / totalSize);
            emit progress(pct);
        }
    }
    f.close();

    // Decode
    QString text;
    if (_encoding == "UTF-8" || _encoding == "UTF-8 BOM")
        text = QString::fromUtf8(content);
    else if (_encoding.startsWith("UTF-16"))
        text = QString::fromUtf8(content); // simplified
    else
        text = QString::fromLocal8Bit(content);

    emit finished(true, text, QString());
}
