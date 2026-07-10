// FindInFilesWorker.cpp - Background worker for Find in Files
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FindInFilesWorker.h"
#include <QDebug>

// ============================================================================
// Construction
// ============================================================================

FindInFilesWorker::FindInFilesWorker(const QString& dir,
                                     const QString& text,
                                     const QStringList& extensions,
                                     QObject* parent)
    : QObject(parent)
    , _dir(dir)
    , _text(text)
    , _extensions(extensions)
{
}

FindInFilesWorker::~FindInFilesWorker() = default;

// ============================================================================
// Main Run
// ============================================================================

void FindInFilesWorker::run() {
    QList<FindResult> results;

    if (!_recursive) {
        // Non-recursive: only top-level files
        QDir d(_dir);
        const QFileInfoList entries = d.entryInfoList(QDir::Files);
        for (const QFileInfo& fi : entries) {
            if (results.size() >= _maxResults) break;
            searchInFile(fi.absoluteFilePath(), _text, results);
        }
    } else {
        // Recursive search
        searchDirectory(QDir(_dir), _text, results, 0);
    }

    emit resultsReady(results);
    emit finished();
}

// ============================================================================
// Directory Search
// ============================================================================

void FindInFilesWorker::searchDirectory(const QDir& dir, const QString& text,
                                          QList<FindResult>& results, int depth) {
    if (depth > 10) return;  // max recursion depth
    if (results.size() >= _maxResults) return;

    const QFileInfoList entries = dir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Name | QDir::IgnoreCase);

    for (const QFileInfo& fi : entries) {
        if (results.size() >= _maxResults) break;

        if (fi.isDir()) {
            // Skip hidden and common non-source directories
            if (fi.fileName().startsWith('.') ||
                fi.fileName() == "node_modules" ||
                fi.fileName() == "__pycache__" ||
                fi.fileName() == "build" ||
                fi.fileName() == "target") {
                continue;
            }
            searchDirectory(QDir(fi.absoluteFilePath()), text, results, depth + 1);
        } else {
            if (matchesFile(fi.absoluteFilePath())) {
                searchInFile(fi.absoluteFilePath(), text, results);
            }
        }
    }
}

bool FindInFilesWorker::matchesFile(const QString& filePath) const {
    if (_extensions.isEmpty()) {
        // Default: skip binary/compiled file types
        static QStringList skipExts = {
            "exe","dll","so","dylib","o","obj","a","lib",
            "png","jpg","jpeg","gif","ico","bmp","svg","ttf","otf","woff","woff2","eot",
            "mp3","wav","mp4","avi","mkv","mov","flac","ogg",
            "zip","tar","gz","bz2","xz","rar","7z",
            "pdf","doc","docx","xls","xlsx","ppt","pptx",
            "db","sqlite","mdb"
        };
        QString ext = QFileInfo(filePath).suffix().toLower();
        if (skipExts.contains(ext))
            return false;
        return true;
    }

    QString ext = QFileInfo(filePath).suffix().toLower();
    return _extensions.contains(ext, Qt::CaseInsensitive);
}

bool FindInFilesWorker::searchInFile(const QString& filePath,
                                      const QString& text,
                                      QList<FindResult>& results) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    // Quick binary check
    QByteArray sample = file.read(256);
    if (sample.contains('\0')) return false;
    file.seek(0);

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    int lineNum = 0;
    while (!in.atEnd()) {
        ++lineNum;
        QString line = in.readLine();

        int idx = line.indexOf(text, 0, Qt::CaseInsensitive);
        if (idx >= 0) {
            FindResult r;
            r.filePath = filePath;
            r.lineNumber = lineNum;
            r.lineContent = line;
            r.column = idx;
            results.append(r);

            if (results.size() >= _maxResults)
                return true;
        }
    }

    return true;
}
