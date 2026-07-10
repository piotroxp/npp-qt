// UdlManager.h - User-Defined Language manager for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3
//
// Parses Notepad++ .udl.xml files from:
// $NPP_HOME/userDefineLangs/*.udl.xml
//
// UDL XML format:
// <NotepadPlus>
//   <UserLang name="MyLang" ext="ext1 ext2">
//     <Settings>...</Settings>
//     <Keywords name="Keywords">...</Keywords>    <!-- words0 -->
//     <Keywords name="Keywords2">...</Keywords>   <!-- words1 -->
//     ...
//     <Keywords name="Keywords8">...</Keywords>  <!-- words7 -->
//     <Comment>...</Comment>
//     <Operators>...</Operators>
//     <Folder>...</Folder>
//   </UserLang>
// </NotepadPlus>

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <vector>

class Buffer;

// ============================================================================
// UDL Definition
// ============================================================================
struct UdlKeywords {
    QString words0;  // Primary keywords
    QString words1;
    QString words2;
    QString words3;
    QString words4;
    QString words5;
    QString words6;
    QString words7;
    QString words8;  // Additional keyword set
};

struct UdlComment {
    QString lineComment;      // e.g. "//"
    QString streamCommentStart;
    QString streamCommentEnd;
};

struct UdlFolder {
    QString folderOpen;
    QString folderClose;
    QString folderMid;
    QString folderEnd;
};

struct UdlDefinition {
    QString name;
    QStringList extensions;
    UdlKeywords keywords;
    UdlComment comment;
    UdlFolder folder;
    QString operators;
    QString stringChars;
    QString lineNum;
    QString autoComplete;
    QString prefixes;
    bool inherit = false;
};

// ============================================================================
// Import Result
// ============================================================================
enum class UdlLoadResult {
    Success,
    FileNotFound,
    ParseError,
    NoUserLangFound
};

// ============================================================================
// UdlManager — Load and manage UDL definitions
// ============================================================================
class UdlManager : public QObject {
    Q_OBJECT

public:
    explicit UdlManager(QObject* parent = nullptr);
    ~UdlManager();

    // === Loading ===

    UdlLoadResult loadUdl(const QString& path);
    int loadAllUdls(const QString& dir);
    void clearAll();

    // === Accessors ===

    QStringList udlNames() const;
    QStringList udlExtensions() const;
    QStringList extensionsForUdl(const QString& name) const;
    UdlDefinition* getUdl(const QString& name);
    const UdlDefinition* getUdl(const QString& name) const;

    // === Apply to buffer ===

    bool applyToBuffer(Buffer* buffer, const QString& udlName);
    QString findUdlForExtension(const QString& ext) const;

    // === N++ detection ===

    QStringList detectNppInstallPaths() const;
    QString nppUdlDirectory() const;

signals:
    void udlLoaded(const QString& name);
    void udlUnloaded(const QString& name);
    void allUdlsLoaded(int count);

private:
    // XML parsing helpers
    bool parseUdlXml(const QString& xmlContent, UdlDefinition& out);
    bool parseKeywords(const QMap<QString, QString>& attrs, UdlDefinition& out);
    bool parseComment(const QMap<QString, QString>& attrs, UdlDefinition& out);
    bool parseFolder(const QMap<QString, QString>& attrs, UdlDefinition& out);

    // Convert UDL keywords to Scintilla keywords string (pipe-separated)
    QString toScintillaKeywords(const QString& spaceSeparated) const;

    QMap<QString, UdlDefinition> _udls;
    QMap<QString, QString> _extToUdl;  // extension → UDL name
};
