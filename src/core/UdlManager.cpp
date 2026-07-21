// UdlManager.cpp - User-Defined Language manager implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "UdlManager.h"
#include "Buffer.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QProcess>

// ============================================================================
// Construction / Lifecycle
// ============================================================================

UdlManager::UdlManager(QObject* parent)
    : QObject(parent)
{
}

UdlManager::~UdlManager() {
    clearAll();
}

void UdlManager::clearAll() {
    _udls.clear();
    _extToUdl.clear();
}

// ============================================================================
// Loading
// ============================================================================

UdlLoadResult UdlManager::loadUdl(const QString& path) {
    QFile file(path);
    if (!file.exists()) {
        qWarning() << "[UdlManager] File not found:" << path;
        return UdlLoadResult::FileNotFound;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[UdlManager] Cannot open file:" << path;
        return UdlLoadResult::ParseError;
    }

    QByteArray data = file.readAll();
    file.close();

    QString xml = QString::fromUtf8(data);
    UdlDefinition def;
    if (!parseUdlXml(xml, def)) {
        qWarning() << "[UdlManager] Failed to parse UDL:" << path;
        return UdlLoadResult::ParseError;
    }

    if (def.name.isEmpty()) {
        qWarning() << "[UdlManager] No UserLang name found in:" << path;
        return UdlLoadResult::NoUserLangFound;
    }

    _udls[def.name] = def;

    // Build extension index
    for (const QString& ext : def.extensions) {
        _extToUdl[ext.toLower()] = def.name;
    }

    emit udlLoaded(def.name);
    return UdlLoadResult::Success;
}

int UdlManager::loadAllUdls(const QString& dir) {
    QDir d(dir);
    if (!d.exists())
        return 0;

    int count = 0;
    const QStringList entries = d.entryList({"*.udl.xml"}, QDir::Files);
    for (const QString& entry : entries) {
        QString fullPath = dir + "/" + entry;
        if (loadUdl(fullPath) == UdlLoadResult::Success)
            ++count;
    }

    if (count > 0)
        emit allUdlsLoaded(count);

    return count;
}

// ============================================================================
// Accessors
// ============================================================================

QStringList UdlManager::udlNames() const {
    return _udls.keys();
}

QStringList UdlManager::udlExtensions() const {
    return _extToUdl.keys();
}

QStringList UdlManager::extensionsForUdl(const QString& name) const {
    const UdlDefinition* def = getUdl(name);
    if (!def) return QStringList();
    return def->extensions;
}

UdlDefinition* UdlManager::getUdl(const QString& name) {
    auto it = _udls.find(name);
    if (it != _udls.end())
        return &it.value();
    return nullptr;
}

const UdlDefinition* UdlManager::getUdl(const QString& name) const {
    auto it = _udls.find(name);
    if (it != _udls.end())
        return &it.value();
    return nullptr;
}

QString UdlManager::findUdlForExtension(const QString& ext) const {
    auto it = _extToUdl.find(ext.toLower());
    if (it != _extToUdl.end())
        return it.value();
    return QString();
}

// ============================================================================
// Mutators
// ============================================================================

void UdlManager::setUdl(const QString& name, const UdlDefinition& def) {
    _udls[name] = def;
    // Rebuild extension index for this UDL
    for (const QString& ext : def.extensions) {
        _extToUdl[ext.toLower()] = name;
    }
}

// ============================================================================
// Apply to Buffer
// ============================================================================

bool UdlManager::applyToBuffer(Buffer* buffer, const QString& udlName) {
    if (!buffer) return false;
    UdlDefinition* def = getUdl(udlName);
    if (!def) return false;

    // Set language to user-defined
    buffer->setLangType(LangType::L_USER, def->name);
    buffer->langHasBeenSetFromMenu();
    return true;
}

// ============================================================================
// N++ Detection
// ============================================================================

QStringList UdlManager::detectNppInstallPaths() const {
    QStringList candidates;

    // Linux (Wine)
    candidates << QDir::homePath() + "/.wine/drive_c/Program Files/Notepad++";
    candidates << QDir::homePath() + "/.wine/drive_c/Program Files (x86)/Notepad++";

    // Linux (flatpak)
    candidates << QDir::homePath() + "/.var/app/org.notepad_plus_plus.NotepadPlusPlus/data/notepad-plus-plus";
    candidates << "/var/lib/flatpak/app/org.notepad_plus_plus.NotepadPlusPlus/data/notepad-plus-plus";

    // Linux (native)
    candidates << "/usr/share/notepad-plus-plus";
    candidates << "/opt/notepad-plus-plus";

    // macOS
    candidates << "/Applications/Notepad++";

    // Windows
    candidates << "C:/Program Files/Notepad++";
    candidates << "C:/Program Files (x86)/Notepad++";

    // Try via command
    QProcess proc;
    proc.setProgram("which");
    proc.setArguments({"notepad++"});
    proc.start();
    if (proc.waitForFinished(2000)) {
        QString out = QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
        if (!out.isEmpty() && QFile::exists(out))
            candidates.prepend(QFileInfo(out).absolutePath());
    }

    QStringList result;
    for (const QString& path : candidates) {
        if (QFile::exists(path))
            result.append(path);
    }
    return result;
}

QString UdlManager::nppUdlDirectory() const {
    QStringList paths = detectNppInstallPaths();
    for (const QString& nppPath : paths) {
        QString udlDir = nppPath + "/notepad-plus-plus-translation/PowerEditor/bin/userDefineLangs";
        if (QDir(udlDir).exists())
            return udlDir;
    }
    return QString();
}

// ============================================================================
// XML Parsing
// ============================================================================

bool UdlManager::parseUdlXml(const QString& xml, UdlDefinition& out) {
    QXmlStreamReader reader(xml);

    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString name = reader.name().toString();

            if (name == "UserLang") {
                QXmlStreamAttributes attrs = reader.attributes();
                out.name = attrs.value("name").toString();
                QString extStr = attrs.value("ext").toString();
                if (!extStr.isEmpty()) {
                    out.extensions = extStr.split(' ', Qt::SkipEmptyParts);
                }
            }
            else if (name == "Keywords") {
                QXmlStreamAttributes attrs = reader.attributes();
                QString kwName = attrs.value("name").toString();
                QString keywords = reader.readElementText();

                if (kwName == "Keywords")
                    out.keywords.words0 = keywords;
                else if (kwName == "Keywords2")
                    out.keywords.words1 = keywords;
                else if (kwName == "Keywords3")
                    out.keywords.words2 = keywords;
                else if (kwName == "Keywords4")
                    out.keywords.words3 = keywords;
                else if (kwName == "Keywords5")
                    out.keywords.words4 = keywords;
                else if (kwName == "Keywords6")
                    out.keywords.words5 = keywords;
                else if (kwName == "Keywords7")
                    out.keywords.words6 = keywords;
                else if (kwName == "Keywords8")
                    out.keywords.words7 = keywords;
                else if (kwName == "Keywords9")
                    out.keywords.words8 = keywords;
            }
            else if (name == "Comment") {
                QXmlStreamAttributes attrs = reader.attributes();
                out.comment.lineComment = attrs.value("lineComment").toString();
                out.comment.streamCommentStart = attrs.value("comment").toString();
                out.comment.streamCommentEnd = attrs.value("comment").toString();  // simplified
            }
            else if (name == "Folder") {
                QXmlStreamAttributes attrs = reader.attributes();
                out.folder.folderOpen = attrs.value("in").toString();
                out.folder.folderClose = attrs.value("out").toString();
            }
            else if (name == "Operators") {
                out.operators = reader.readElementText();
            }
            else if (name == "StringChars") {
                out.stringChars = reader.readElementText();
            }
        }
    }

    return !reader.hasError() && !out.name.isEmpty();
}

QString UdlManager::toScintillaKeywords(const QString& spaceSeparated) const {
    // Scintilla uses space as separator
    return spaceSeparated;
}
