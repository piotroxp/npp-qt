// FindInFilesWorker.cpp - Background search worker for Find in Files
// Copyright (C) 2026 Agent Army | GPL v3

#include "FindInFilesWorker.h"
#include <QDir>
#include <QStringConverter>
#include <QFileInfo>
#include <QRegularExpression>
#include <QtDebug>

FindInFilesWorker::FindInFilesWorker(const QString& directory, const QString& text,
                                     const QString& filters, bool caseSensitive,
                                     bool wholeWord, bool regex)
    : _directory(directory), _text(text),
      _caseSensitive(caseSensitive), _wholeWord(wholeWord), _regex(regex)
{
    if (!filters.isEmpty() && filters != "*") {
        _filters = filters.split(' ', Qt::SkipEmptyParts);
    } else {
        _filters = {"*"};
    }
}

FindInFilesWorker::~FindInFilesWorker() = default;

void FindInFilesWorker::run() {
    searchDirectory(_directory);
    emit resultsReady(_results);
    emit finished();
}

void FindInFilesWorker::searchDirectory(const QString& dir) {
    QDir qdir(dir);
    QFileInfoList entries = qdir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& info : entries) {
        if (info.isDir()) {
            // Recurse into subdirectories (limit depth to avoid runaway traversal)
            static int depth = 0;
            if (depth < 20)
                searchDirectory(info.filePath());
            continue;
        }

        // Check file extension against filters
        bool matchesFilter = false;
        for (const QString& filter : _filters) {
            QRegularExpression re = QRegularExpression::fromWildcard(filter, Qt::CaseInsensitive);
            if (re.match(info.fileName()).hasMatch()) {
                matchesFilter = true;
                break;
            }
        }
        if (!matchesFilter && _filters.first() != "*") {
            // If no filters set, accept all; otherwise only matched above
        }

        searchInFile(info.filePath());
        ++_filesSearched;
        emit progress(info.filePath(), _filesSearched);
    }
}

void FindInFilesWorker::searchInFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    int lineNumber = 0;

    while (!in.atEnd()) {
        ++lineNumber;
        QString line = in.readLine();

        if (!lineMatches(line))
            continue;

        // Find all match positions in this line
        Qt::CaseSensitivity searchFlags = _caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
        int pos = 0;
        while ((pos = line.indexOf(_text, pos, searchFlags)) != -1) {
            int endPos = pos + _text.length();

            // Apply whole-word constraint
            if (_wholeWord) {
                bool validStart = (pos == 0 || !line[pos - 1].isLetterOrNumber());
                bool validEnd   = (endPos >= line.length() || !line[endPos].isLetterOrNumber());
                if (!validStart || !validEnd) {
                    pos = endPos;
                    continue;
                }
            }

            _results.append(FindResult{filePath, lineNumber, pos, line, {}});
            pos = endPos;

            // Limit matches per file to avoid huge result sets
            if (_results.size() >= 1000)
                return;
        }
    }
}

bool FindInFilesWorker::lineMatches(const QString& line) const {
    if (_regex) {
        return matchesRegex(line);
    }
    return matchesPattern(line);
}

bool FindInFilesWorker::matchesPattern(const QString& line) const {
    Qt::CaseSensitivity cs = _caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    return line.contains(_text, cs);
}

bool FindInFilesWorker::matchesRegex(const QString& line) const {
    QRegularExpression::PatternOptions opts = QRegularExpression::NoPatternOption;
    if (!_caseSensitive)
        opts |= QRegularExpression::CaseInsensitiveOption;

    QRegularExpression re(_text, opts);
    return re.match(line).hasMatch();
}
