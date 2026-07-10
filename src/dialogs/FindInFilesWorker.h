// FindInFilesWorker.h - Background search worker for Find in Files
// Copyright (C) 2026 Agent Army | GPL v3

#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QThread>
#include <QFile>
#include <QTextStream>

// Result of a single match in Find in Files
struct FindResult {
    QString filePath;
    int lineNumber;
    QString lineContent;
    int matchStart;
    int matchEnd;
    int column;   // character offset within the line

    FindResult() : lineNumber(0), matchStart(0), matchEnd(0), column(0) {}
    FindResult(const QString& path, int line, const QString& content, int start, int end)
        : filePath(path), lineNumber(line), lineContent(content),
          matchStart(start), matchEnd(end), column(start) {}
};

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
