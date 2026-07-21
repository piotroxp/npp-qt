// FindInFilesWorker.h - Background search worker for Find in Files
// Copyright (C) 2026 Agent Army | GPL v3

#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QThread>
#include <QFile>
#include "common/Types.h"
#include <QTextStream>

// Background worker that searches files in a directory tree
class FindInFilesWorker : public QObject {
    Q_OBJECT
public:
    explicit FindInFilesWorker(const QString& directory, const QString& text,
                                const QString& filters = "*",
                                bool caseSensitive = true, bool wholeWord = false,
                                bool regex = false);
    ~FindInFilesWorker();

public slots:
    void run();

signals:
    void resultsReady(const QList<FindResult>& results);
    void progress(const QString& currentFile, int filesSearched);
    void finished();

private:
    void searchDirectory(const QString& dir);
    void searchInFile(const QString& filePath);
    bool lineMatches(const QString& line) const;
    bool matchesPattern(const QString& text) const;
    bool matchesWholeWord(const QString& text) const;
    bool matchesRegex(const QString& text) const;

    QString _directory;
    QString _text;
    QStringList _filters;
    bool _caseSensitive;
    bool _wholeWord;
    bool _regex;

    QList<FindResult> _results;
    int _filesSearched = 0;
};
