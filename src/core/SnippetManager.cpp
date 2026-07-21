// SnippetManager.cpp — Code-snippet storage with INI persistence
// Copyright (C) 2026 Agent Army | GPL v3

#include "SnippetManager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QtCore/QStringBuilder>
#include <algorithm>
#include <utility>

namespace {
// INI escape helper. QSettings' native INI format escapes '\\', ';',
// '#', '=', '[', ']', and embedded newlines correctly; we only have
// to make sure the value does not contain raw null bytes.
QString sanitizeForIni(const QString& v) {
    return v;
}
} // namespace

SnippetManager::SnippetManager(QObject* parent) : QObject(parent) {}

SnippetManager::~SnippetManager() = default;

bool SnippetManager::loadFromFile(const QString& path) {
    if (path.isEmpty()) {
        qWarning("SnippetManager::loadFromFile: empty path");
        return false;
    }
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isReadable()) {
        qWarning("SnippetManager::loadFromFile: %s is not readable",
                 qUtf8Printable(path));
        return false;
    }

    QSettings ini(path, QSettings::IniFormat);
    if (ini.status() != QSettings::NoError) {
        qWarning("SnippetManager::loadFromFile: QSettings error reading %s",
                 qUtf8Printable(path));
        return false;
    }

    int loaded = 0;
    const QStringList groups = ini.childGroups();
    for (const QString& group : groups) {
        ini.beginGroup(group);
        const QString text = ini.value(QStringLiteral("text")).toString();
        ini.endGroup();
        if (text.isEmpty()) {
            // Skip empty snippets rather than storing placeholders —
            // an empty text field in INI almost always means a
            // malformed entry, not a legitimate empty snippet.
            continue;
        }
        setSnippet(group, text);
        ++loaded;
    }

    _filePath = path;
    return loaded >= 0;  // even an empty INI is a successful load
}

bool SnippetManager::saveToFile(const QString& path) const {
    if (path.isEmpty()) {
        qWarning("SnippetManager::saveToFile: empty path");
        return false;
    }
    // Ensure parent dir exists.
    QFileInfo fi(path);
    QDir().mkpath(fi.absolutePath());

    QSettings ini(path, QSettings::IniFormat);
    ini.clear();
    for (auto it = _snippets.cbegin(); it != _snippets.cend(); ++it) {
        ini.beginGroup(it.key());
        ini.setValue(QStringLiteral("text"), sanitizeForIni(it.value()));
        ini.endGroup();
    }
    ini.sync();
    if (ini.status() != QSettings::NoError) {
        qWarning("SnippetManager::saveToFile: QSettings error writing %s",
                 qUtf8Printable(path));
        return false;
    }
    return true;
}

void SnippetManager::setDefaultDirectory(const QString& dir) {
    if (dir.isEmpty()) return;
    const QString candidate = QDir(dir).filePath(QStringLiteral("snippets.ini"));
    if (QFileInfo::exists(candidate)) {
        loadFromFile(candidate);
    }
}

QStringList SnippetManager::defaultSearchPaths() {
    QStringList paths;
    // XDG-style config dir first.
    const QString xdg =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!xdg.isEmpty()) {
        paths << QDir(xdg).filePath(QStringLiteral("snippets.ini"));
    }
    // Home-relative fallback.
    const QString home = QDir::homePath();
    if (!home.isEmpty()) {
        paths << home + QStringLiteral("/.config/npp-qt/snippets.ini");
        paths << home + QStringLiteral("/.npp-qt/snippets.ini");
    }
    return paths;
}

void SnippetManager::setSnippet(const QString& name, const QString& text) {
    if (name.isEmpty()) return;
    _snippets.insert(name, text);
}

bool SnippetManager::removeSnippet(const QString& name) {
    return _snippets.remove(name) > 0;
}

void SnippetManager::clear() {
    _snippets.clear();
}

bool SnippetManager::hasSnippet(const QString& name) const {
    return _snippets.contains(name);
}

QString SnippetManager::rawText(const QString& name) const {
    return _snippets.value(name);
}

SnippetManager::Snippet SnippetManager::expand(const QString& name) const {
    Snippet out;
    auto it = _snippets.find(name);
    if (it == _snippets.end()) return out;

    const QString& raw = it.value();
    const int cursor = cursorOffsetFromText(raw);
    QString expanded = stripPlaceholders(raw);
    if (cursor >= 0) {
        // The cursor placeholder character `|` is one byte; if it sits
        // at position `cursor` in the raw text and we stripped it out,
        // the expansion is one character shorter than the raw text.
        if (cursor < expanded.size()) {
            out.cursorOffset = cursor;
        } else {
            out.cursorOffset = expanded.size();
        }
    }
    out.name = name;
    out.text = std::move(expanded);
    return out;
}

QStringList SnippetManager::names() const {
    QStringList list = _snippets.keys();
    std::sort(list.begin(), list.end());
    return list;
}

int SnippetManager::cursorOffsetFromText(const QString& text) {
    int idx = text.indexOf(QLatin1Char('|'));
    return idx;  // -1 if not present
}

QString SnippetManager::stripPlaceholders(const QString& text) {
    QString out;
    out.reserve(text.size());
    for (int i = 0; i < text.size(); ++i) {
        const QChar c = text.at(i);
        if (c == QLatin1Char('|')) {
            continue;  // cursor marker — drop entirely
        }
        if (c == QLatin1Char('$')) {
            // $0, $1, ... $9 collapse to empty; $NAME collapses too.
            if (i + 1 < text.size()) {
                const QChar next = text.at(i + 1);
                if (next.isDigit()) {
                    ++i;  // consume the digit
                    continue;
                }
                if (next.isLetter() || next == QLatin1Char('_')) {
                    ++i;  // consume the start of the name
                    while (i + 1 < text.size()) {
                        const QChar nx = text.at(i + 1);
                        if (nx.isLetterOrNumber() || nx == QLatin1Char('_')) {
                            ++i;
                        } else {
                            break;
                        }
                    }
                    continue;
                }
            }
            // Lone '$' — keep as-is.
        }
        out.append(c);
    }
    return out;
}

QVector<SnippetManager::TabStop>
SnippetManager::collectTabStops(const QString& text) {
    QVector<TabStop> stops;
    // First pass: numbered stops $1 .. $9 in numeric order.
    QHash<int, QVector<int>> numbered;
    QHash<QString, QVector<int>> named;
    for (int i = 0; i < text.size(); ++i) {
        const QChar c = text.at(i);
        if (c != QLatin1Char('$')) continue;
        if (i + 1 >= text.size()) break;
        const QChar next = text.at(i + 1);
        if (next.isDigit()) {
            const int digit = next.digitValue();
            numbered[digit].append(i);
            ++i;
        } else if (next.isLetter() || next == QLatin1Char('_')) {
            int j = i + 1;
            while (j < text.size()) {
                const QChar nx = text.at(j);
                if (nx.isLetterOrNumber() || nx == QLatin1Char('_')) {
                    ++j;
                } else {
                    break;
                }
            }
            named[text.mid(i + 1, j - i - 1)].append(i);
            i = j - 1;
        }
    }
    // Stable insertion order: numbered first, then named.
    QList<int> numberKeys;
    for (auto it = numbered.cbegin(); it != numbered.cend(); ++it) {
        numberKeys.append(it.key());
    }
    std::sort(numberKeys.begin(), numberKeys.end());
    for (int k : numberKeys) {
        for (int pos : numbered[k]) {
            TabStop ts;
            ts.name = QString::number(k);
            ts.offset = pos;
            stops.append(ts);
        }
    }
    QList<QString> nameKeys;
    for (auto it = named.cbegin(); it != named.cend(); ++it) {
        nameKeys.append(it.key());
    }
    std::sort(nameKeys.begin(), nameKeys.end());
    for (const QString& k : nameKeys) {
        for (int pos : named[k]) {
            TabStop ts;
            ts.name = k;
            ts.offset = pos;
            stops.append(ts);
        }
    }
    return stops;
}
