// FindInFilesWorker.h - Background worker for Find in Files
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QList>
#include "common/Types.h"

// ============================================================================
// FindInFilesWorker — threaded search worker
// ============================================================================
class FindInFilesWorker : public QObject {
    Q_OBJECT

public:
    explicit FindInFilesWorker(const QString& dir,
                                const QString& text,
                                const QStringList& extensions = QStringList(),
                                QObject* parent = nullptr);
    ~FindInFilesWorker() override;

    // Configure search options
    void setRecursive(bool recursive) { _recursive = recursive; }
    void setMaxResults(int max) { _maxResults = max; }

public slots:
    void run();

signals:
    void resultsReady(const QList<FindResult>& results);
    void progress(int current, int total);
    void finished();
    void error(const QString& message);

private:
    void searchDirectory(const QDir& dir, const QString& text,
                         QList<FindResult>& results, int depth);
    bool matchesFile(const QString& filePath) const;
    bool searchInFile(const QString& filePath, const QString& text,
                      QList<FindResult>& results);

    QString _dir;
    QString _text;
    QStringList _extensions;
    bool _recursive = true;
    int _maxResults = 10000;
};
