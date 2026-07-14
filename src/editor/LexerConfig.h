// LexerConfig.h - Comprehensive lexer configuration and language detection
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QRegularExpression>
#include "../common/Types.h"

// ============================================================================
// LexerInfo — metadata for a single language / lexer
// ============================================================================
struct LexerInfo {
    /// Display name (e.g., "C++", "Python")
    QString name;

    /// All file extensions associated with this language (with dot, e.g., ".cpp")
    QStringList extensions;

    /// Notepad++ language ID (LangType enum value as int)
    int lexerId;

    /// Short lexer name for QsciLexer (e.g., "cpp", "python")
    QString lexerName;

    /// Shebang patterns that identify this language (e.g., "#!/bin/bash")
    QStringList shebangs;

    /// First-line regex patterns for content-based detection
    QStringList firstLinePatterns;

    /// Whether this language is case-sensitive for keywords
    bool caseSensitive = true;

    /// Default comment line prefix (e.g., "//" or "#")
    QString commentLine;

    /// Block comment start
    QString commentStart;
    /// Block comment end
    QString commentEnd;
};

// ============================================================================
// UDLPattern — User Defined Language regex pattern entry
// ============================================================================
struct UDLKeyword {
    QString keyword;
    int styleIndex = 0;
};

struct UDLPattern {
    int styleId = 0;
    QString name;
    QString regex;
    QColor foreground;
    QColor background;
    bool bold = false;
    bool italic = false;
    bool underline = false;
};

// ============================================================================
// LexerConfig — comprehensive lexer configuration manager
// ============================================================================
class LexerConfig {
public:
    // ---- Singleton ----
    static LexerConfig& instance();

    // ---- Built-in lexer registry ----

    /// Get all registered lexer info
    static const QVector<LexerInfo>& builtInLexers();

    /// Get lexer info by LangType
    static const LexerInfo* lexerInfo(LangType lang);

    /// Get lexer info by language name (case-insensitive)
    static const LexerInfo* lexerInfoByName(const QString& name);

    /// Get lexer info by file extension (with or without leading dot)
    static const LexerInfo* lexerInfoByExtension(const QString& ext);

    // ---- Detection ----

    /// Detect language from file extension
    static LangType detectFromExtension(const QString& filenameOrExt);

    /// Detect language from file content (shebang, XML decl, etc.)
    static LangType detectFromContent(const QString& content,
                                      const QString& filename = QString());

    /// Detect language from combined content and filename
    static LangType detect(const QString& filename, const QString& content);

    // ---- Language queries ----

    /// Get all extensions for a given language
    static QStringList extensionsFor(LangType lang);

    /// Get all supported file extensions (wildcard glob strings)
    static QStringList allExtensionsGlob();

    /// Get the display name for a language
    static QString displayName(LangType lang);

    /// Check if a language uses block comments
    static bool hasBlockComments(LangType lang);

    /// Get comment style for a language
    static void getCommentStyle(LangType lang,
                                QString* singleLine,
                                QString* blockStart,
                                QString* blockEnd);

    // ---- UDL (User Defined Language) ----

    /// Load UDL patterns from a Notepad++ config file path
    static QVector<UDLPattern> loadUDL(const QString& configPath);

    /// Register a custom lexer from UDL patterns
    static void registerUDLLexer(const QString& name,
                                  const QVector<UDLPattern>& patterns);

    // ---- Multi-language file detection ----

    /// Detect whether a .h file is C++ or Objective-C based on content
    static LangType detectHeaderLanguage(const QString& content);

    /// Detect whether a .m file is Objective-C or MATLAB based on content
    static LangType detectObjCOrMatlab(const QString& content);

    /// Check if content matches PHP embedded in HTML
    static bool isPHPCode(const QString& content);

    /// Check if content is a Makefile (special filename detection)
    static bool isMakefile(const QString& filename);

private:
    LexerConfig();
    void buildLexerTable();
    QVector<LexerInfo> _lexers;
    QMap<QString, const LexerInfo*> _extIndex;
    QMap<QString, const LexerInfo*> _nameIndex;
    QMap<QString, const LexerInfo*> _shebangIndex;
};
