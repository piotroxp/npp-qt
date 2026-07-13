#pragma once
#include <QObject>
#include <QString>
#include <QByteArray>

class FileLoaderWorker : public QObject {
    Q_OBJECT
public:
    explicit FileLoaderWorker(const QString& path, QObject* parent = nullptr);
    void setEncoding(const QString& encoding) { _encoding = encoding; }
    void start();

signals:
    void progress(int percent);
    void finished(bool success, const QString& content, const QString& error);

private:
    QString _path;
    QString _encoding;
};
