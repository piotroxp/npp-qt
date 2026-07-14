// LexerConfig.cpp - Comprehensive lexer configuration and language detection
// Copyright (C) 2026 Agent Army
// GPL v3

#include "LexerConfig.h"
#include "../core/LanguageManager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QColor>
#include <QCoreApplication>
#include <algorithm>
#include <cctype>

// ============================================================================
// Static lexer table
// ============================================================================

#define LANG_INFO(name, extensions, lexerId, lexerName, ...) \
    { QStringLiteral(name), \
      QStringList() << extensions, \
      int(lexerId), \
      QStringLiteral(lexerName), \
      __VA_ARGS__ }

static const QVector<LexerInfo>& _builtInLexers() {
    static QVector<LexerInfo> _lexers = {
        // C/C++ family
        { "C++",          {".cpp", ".cc", ".cxx", ".c++", ".h", ".hpp", ".hh", ".hxx", ".h++"},
          2, "cpp", { }, { }, true, "//", "/*", "*/" },
        { "C",            {".c", ".i"},
          1, "cpp", { }, { }, true, "//", "/*", "*/" },
        { "Java",         {".java"},
          3, "java", { }, { }, true, "//", "/*", "*/" },
        { "C#",           {".cs"},
          4, "csharp", { }, { }, true, "//", "/*", "*/" },
        { "Objective-C",  {".m", ".mm"},
          5, "bash", { }, { }, true, "//", "/*", "*/" },

        // Web
        { "HTML",         {".html", ".htm", ".hta", ".hta"},
          6, "html", { }, { }, false, "//", "<!--", "-->" },
        { "XML",          {".xml", ".xsl", ".xslt", ".svg", ".xsd", ".xul", ".rss", ".atom", ".kml"},
          7, "xml", { }, { "\\s*<\\?xml", "\\s*<!DOCTYPE\\s+html" }, false, "", "", "" },
        { "PHP",          {".php", ".php3", ".php4", ".php5", ".phtml"},
          11, "php", { }, { "\\s*<\\?php" }, false, "//", "/*", "*/" },
        { "JavaScript",   {".js", ".mjs", ".cjs", ".jsx", ".tsx"},
          14, "javascript", { }, { }, false, "//", "/*", "*/" },
        { "TypeScript",   {".ts", ".tsx"},
          14, "javascript", { }, { }, false, "//", "/*", "*/" },
        { "CSS",          {".css", ".scss", ".sass", ".less"},
          16, "css", { }, { }, false, "//", "/*", "*/" },

        // Scripting
        { "Python",      {".py", ".pyw", ".pyi", ".pyx", ".pxd"},
          13, "python", { "#!/usr/bin/env python", "#!/usr/bin/python" }, { }, true, "#", "", "" },
        { "Ruby",         {".rb", ".rbw", ".rake", ".gemspec"},
          12, "ruby", { "#!/usr/bin/env ruby", "#!/usr/bin/ruby" }, { }, true, "#", "", "" },
        { "Perl",         {".pl", ".pm", ".pod", ".t"},
          10, "perl", { "#!/usr/bin/perl", "#!/bin/perl" }, { }, true, "#", "", "" },
        { "Shell/Bash",   {".sh", ".bash", ".zsh", ".dash", ".ksh", ".csh", ".ash"},
          21, "batch", { "#!/bin/bash", "#!/bin/sh", "#!/usr/bin/env bash" }, { }, false, "#", "", "" },
        { "PowerShell",   {".ps1", ".psm1", ".psd1", ".ps1xml"},
          21, "batch", { "#!/usr/bin/env pwsh", "#!/usr/bin/pwsh" }, { }, false, "#", "", "" },
        { "Lua",          {".lua"},
          9, "lua", { "#!/usr/bin/lua", "#!/usr/local/bin/lua" }, { }, true, "--", "", "" },
        { "Tcl",          {".tcl", ".tk", ".itcl", ".itk"},
          -1, "tcl", { "#!/usr/bin/tclsh", "#!/bin/tclsh" }, { }, false, "#", "", "" },

        // Data / config
        { "JSON",         {".json", ".jsonc", ".jsonl", ".ndjson"},
          15, "json", { }, { "\\s*\\{" }, false, "", "", "" },
        { "YAML",         {".yaml", ".yml"},
          17, "yaml", { }, { "\\s*---", "\\s*\\w+:\\s" }, false, "#", "", "" },
        { "TOML",         {".toml"},
          -1, "yaml", { }, { "\\[.*\\]" }, false, "#", "", "" },
        { "INI",          {".ini", ".cfg", ".conf", ".properties", ".reg"},
          22, "properties", { }, { "\\[.*\\]" }, false, ";", "", "" },

        // Markup
        { "Markdown",     {".md", ".markdown", ".mdown", ".mkd", ".mkdn"},
          20, "markdown", { }, { "^#", "^\\*\\*" }, false, "", "", "" },
        { "LaTeX",        {".tex", ".latex", ".ltx"},
          -1, "tex", { }, { "^\\\\documentclass", "^\\\\begin\\{" }, true, "%", "", "" },
        { "PostScript",   {".ps", ".eps"},
          -1, "postscript", { }, { "^%!" }, false, "%", "", "" },

        // Build / toolchain
        { "Makefile",     {"Makefile", "makefile", ".mk", "GNUmakefile", "CMakeLists.txt"},
          18, "makefile", { }, { "^.PHONY:", "^\\w+:", "^\\t" }, false, "#", "", "" },
        { "CMake",        {".cmake", "CMakeLists.txt"},
          -1, "makefile", { }, { "^cmake_minimum_required", "^project\\(" }, false, "#", "", "" },
        { "NSIS",         {".nsi", ".nsh"},
          -1, "batch", { }, { "^!include", "^\\w+\\s*=" }, false, "#", "/*", "*/" },
        { "Batch",        {".bat", ".cmd", ".btm"},
          21, "batch", { }, { "^@?echo\\s", "^:\\ }, false, "REM", "", "" },

        // SQL / database
        { "SQL",          {".sql", ".ddl", ".dml"},
          -1, "sql", { }, { "^\\s*SELECT", "^\\s*INSERT", "^\\s*CREATE" }, true, "--", "/*", "*/" },

        // Functional / compiled
        { "Haskell",      {".hs", ".hs-boot", ".lhs"},
          -1, "haskell", { }, { "^\\s*module\\s", "^\\s*import\\s" }, true, "--", "{-", "-}" },
        { "Lisp",         {".lisp", ".lsp", ".el", ".scm", ".ss"},
          -1, "lisp", { "#!/usr/bin/env guile", "#!/usr/bin/guile" }, { "\\(" }, true, ";", "", "" },
        { "Scheme",       {".scm", ".ss"},
          -1, "lisp", { }, { "\\(\\s*define" }, true, ";", "", "" },
        { "Rust",         {".rs"},
          -1, "rust", { "#!/usr/bin/env rustc" }, { "^\\s*fn\\s", "^\\s*let\\s" }, true, "//", "/*", "*/" },
        { "Go",           {".go"},
          -1, "go", { }, { "^\\s*package\\s", "^\\s*func\\s" }, true, "//", "/*", "*/" },
        { "Kotlin",       {".kt", ".kts", ".ktm"},
          -1, "java", { }, { "^\\s*fun\\s", "^\\s*val\\s", "^\\s*class\\s" }, true, "//", "/*", "*/" },
        { "Swift",        {".swift"},
          -1, "java", { "#!/usr/bin/swift" }, { "^\\s*func\\s", "^\\s*let\\s" }, true, "//", "/*", "*/" },
        { "Julia",        {".jl"},
          -1, "python", { "#!/usr/bin/env julia" }, { "^\\s*function\\s", "^\\s*@", "^\\s*using\\s" }, true, "#", "", "" },
        { "R",            {".r", ".R", ".Rdata", ".rds"},
          -1, "r", { "#!/usr/bin/env Rscript" }, { "^\\s*<-", "^\\s*function\\s" }, true, "#", "", "" },
        { "MATLAB",       {".m", ".mat"},
          -1, "matlab", { }, { "^\\s*function\\s", "^\\s*if\\s", "^\\s*for\\s" }, true, "%", "%", "" },
        { "Fortran",      {".f", ".f90", ".f95", ".f03", ".for", ".f77"},
          -1, "fortran", { }, { "^\\s*PROGRAM\\s", "^\\s*SUBROUTINE\\s" }, false, "!", "", "" },
        { "Pascal",       {".pas", ".pp", ".inc", ".dpr"},
          -1, "pascal", { }, { "^\\s*program\\s", "^\\s*procedure\\s" }, true, "{", "{", "}" },
        { "Verilog",      {".v", ".sv", ".vhdl"},
          -1, "verilog", { }, { "^\\s*module\\s", "^\\s*always\\s*@" }, true, "//", "/*", "*/" },
        { "VHDL",         {".vhd", ".vhdl"},
          -1, "vhdl", { }, { "^\\s*entity\\s", "^\\s*architecture\\s" }, false, "--", "", "" },
        { "Assembly",     {".asm", ".s", ".S", ".a51", ".masm", ".nasm"},
          -1, "asm", { }, { "^\\s*\\w+:" }, false, ";", "", "" },
        { "ActionScript", {".as"},
          -1, "actionscript", { }, { "^\\s*package\\s", "^\\s*class\\s" }, true, "//", "/*", "*/" },
        { "ASP",          {".asp", ".asa"},
          -1, "html", { }, { "<%" }, false, "'", "", "" },
        { "AutoIt",       {".au3"},
          -1, "batch", { }, { "^\\s*Func\\s", "^\\s*While\\s" }, false, ";", "", "" },

        // Diff / patch
        { "Diff",        {".diff", ".patch", ".rej"},
          -1, "diff", { }, { "^---", "^\\+\\+\\+", "^@@" }, false, "", "", "" },

        // Normal text
        { "Normal Text", {".txt", ".text"},
          0, "text", { }, { }, false, "", "", "" },
    };
    return _lexers;
}

// ============================================================================
// Singleton
// ============================================================================

LexerConfig& LexerConfig::instance() {
    static LexerConfig cfg;
    return cfg;
}

LexerConfig::LexerConfig() {
    buildLexerTable();
}

void LexerConfig::buildLexerTable() {
    const QVector<LexerInfo>& src = _builtInLexers();
    _lexers = src;

    for (const LexerInfo& info : _lexers) {
        // Index by extension
        for (const QString& ext : info.extensions) {
            QString key = ext.toLower();
            if (key.startsWith("makefile") || key.endsWith("cmakelists.txt")) {
                // Special filenames, not extensions
                continue;
            }
            if (key.startsWith('.')) {
                _extIndex[key] = &info;
                _extIndex[key.mid(1)] = &info;  // Also without dot
            }
        }
        // Index by name
        _nameIndex[info.name.toLower()] = &info;
        // Index by shebang
        for (const QString& sb : info.shebangs) {
            if (!sb.isEmpty())
                _shebangIndex[sb.toLower()] = &info;
        }
    }

    // Handle Makefile specially (filename-based, not extension)
    {
        const LexerInfo makeInfo = { "Makefile", { "Makefile" },
            18, "makefile", { }, { }, false, "#", "", "" };
        _nameIndex["makefile"] = &makeInfo;
        _nameIndex["makefile".toLower()] = &makeInfo;
    }
}

const QVector<LexerInfo>& LexerConfig::builtInLexers() {
    return _builtInLexers();
}

const LexerInfo* LexerConfig::lexerInfo(LangType lang) {
    int id = static_cast<int>(lang);
    for (const LexerInfo& info : _builtInLexers()) {
        if (info.lexerId == id)
            return &info;
    }
    return nullptr;
}

const LexerInfo* LexerConfig::lexerInfoByName(const QString& name) {
    auto it = instance()._nameIndex.constFind(name.toLower());
    if (it != instance()._nameIndex.constEnd())
        return it.value();
    // Fallback: linear search
    for (const LexerInfo& info : _builtInLexers()) {
        if (info.name.toLower() == name.toLower())
            return &info;
    }
    return nullptr;
}

const LexerInfo* LexerConfig::lexerInfoByExtension(const QString& ext) {
    QString key = ext.toLower();
    if (!key.startsWith('.'))
        key = "." + key;

    // Special case: Makefile (no extension)
    if (ext.toLower() == "makefile" || ext.toLower() == "cmakelists.txt")
        return lexerInfoByName("Makefile");

    auto& self = instance();
    auto it = self._extIndex.constFind(key);
    if (it != self._extIndex.constEnd())
        return it.value();
    // Try without dot
    it = self._extIndex.constFind(key.mid(1));
    if (it != self._extIndex.constEnd())
        return it.value();
    return nullptr;
}

// ============================================================================
// Detection
// ============================================================================

LangType LexerConfig::detectFromExtension(const QString& filenameOrExt) {
    QString name = filenameOrExt;
    // Strip path
    int slash = qMax(name.lastIndexOf('/'), name.lastIndexOf('\\'));
    if (slash >= 0)
        name = name.mid(slash + 1);

    // Special filenames
    QString lowerName = name.toLower();
    if (lowerName == "makefile" || lowerName == "gnumakefile" || lowerName == "makefile.unix")
        return LangType::L_MAKEFILE;
    if (lowerName == "cmakelists.txt")
        return LangType::L_MAKEFILE;
    if (lowerName == "dockerfile")
        return LangType::L_DOCKERFILE;
    if (lowerName.endsWith(".nsi") || lowerName.endsWith(".nsh"))
        return LangType::L_USER;

    // Extract extension
    int dot = name.lastIndexOf('.');
    if (dot < 0)
        return LangType::L_TEXT;

    QString ext = name.mid(dot);

    // Check special extensions that QsciLexer may not have
    static const QMap<QString, LangType> specialExtMap = {
        {".rs",    LangType::L_C},   // Rust (QsciLexerRust if available)
        {".swift", LangType::L_JAVA},
        {".kt",    LangType::L_JAVA},
        {".kts",   LangType::L_JAVA},
        {".go",    LangType::L_C},
        {".r",     LangType::L_TEXT},
        {".R",     LangType::L_TEXT},
        {".jl",    LangType::L_PYTHON},
        {".scala", LangType::L_JAVA},
        {".hs",    LangType::L_TEXT},
        {".lhs",   LangType::L_TEXT},
        {".lisp",  LangType::L_TEXT},
        {".clj",   LangType::L_TEXT},
        {".erl",   LangType::L_TEXT},
        {".ex",    LangType::L_TEXT},
        {".exs",   LangType::L_TEXT},
        {".fs",    LangType::L_TEXT},
        {".fsx",   LangType::L_TEXT},
        {".nim",   LangType::L_TEXT},
        {".zig",   LangType::L_C},
        {".v",     LangType::L_TEXT},
        {".sv",    LangType::L_TEXT},
        {".vhd",   LangType::L_TEXT},
        {".vlog",  LangType::L_TEXT},
        {".ps1",   LangType::L_BATCH},
        {".psm1",  LangType::L_BATCH},
        {".psd1",  LangType::L_BATCH},
        {".sh",    LangType::L_BATCH},
        {".bash",  LangType::L_BATCH},
        {".zsh",   LangType::L_BATCH},
        {".fish",  LangType::L_BATCH},
        {".ash",   LangType::L_BATCH},
        {".csh",   LangType::L_BATCH},
        {".nix",   LangType::L_BATCH},
        {".cmake", LangType::L_MAKEFILE},
        {".toml",  LangType::L_YAML},
        {".yaml",  LangType::L_YAML},
        {".yml",   LangType::L_YAML},
        {".tcl",   LangType::L_TEXT},
        {".tk",    LangType::L_TEXT},
        {".tcl",   LangType::L_TEXT},
        {".asm",   LangType::L_TEXT},
        {".a51",   LangType::L_TEXT},
        {".masm",  LangType::L_TEXT},
        {".nasm",  LangType::L_TEXT},
        {".f",     LangType::L_TEXT},
        {".f90",   LangType::L_TEXT},
        {".f95",   LangType::L_TEXT},
        {".f03",   LangType::L_TEXT},
        {".for",   LangType::L_TEXT},
        {".pas",   LangType::L_TEXT},
        {".pp",    LangType::L_TEXT},
        {".lsp",   LangType::L_TEXT},
        {".scm",   LangType::L_TEXT},
        {".ss",    LangType::L_TEXT},
        {".as",    LangType::L_TEXT},
        {".au3",   LangType::L_BATCH},
        {".nsi",   LangType::L_USER},
        {".nsh",   LangType::L_USER},
    };

    auto sit = specialExtMap.constFind(ext.toLower());
    if (sit != specialExtMap.constEnd())
        return sit.value();

    // Use LanguageManager's built-in detection as fallback
    std::string extStr = ext.toStdString();
    return LanguageManager::detect(extStr);
}

LangType LexerConfig::detectFromContent(const QString& content,
                                        const QString& filename) {
    if (content.isEmpty())
        return LangType::L_TEXT;

    // ---- Shebang detection ----
    QString firstLine = content.section('\n', 0, 0).trimmed();
    if (firstLine.startsWith("#!")) {
        for (auto it = instance()._shebangIndex.constBegin();
             it != instance()._shebangIndex.constEnd(); ++it) {
            if (firstLine.toLower().contains(it.key())) {
                return static_cast<LangType>(it.value()->lexerId);
            }
        }
    }

    // ---- Content-based detection ----
    if (firstLine.startsWith("%!") || firstLine.startsWith("%PDF"))
        return LangType::L_TEXT; // PostScript/PDF, not needed

    if (content.contains("<?xml") && content.contains("<"))
        return LangType::L_XML;

    if (content.contains("<!DOCTYPE html") || content.contains("<html"))
        return LangType::L_HTML;

    if (content.contains("<?php"))
        return LangType::L_PHP;

    // Check for embedded languages (HTML with JS/CSS/PHP)
    if (content.contains("<script") && content.contains("function"))
        return LangType::L_HTML;

    // YAML: document start marker
    if (content.trimmed().startsWith("---"))
        return LangType::L_YAML;

    // Makefile
    if (filename.toLower().contains("makefile") ||
        content.contains(".PHONY") || content.contains("$(MAKE)"))
        return LangType::L_MAKEFILE;

    // LaTeX
    if (content.contains("\\documentclass") || content.contains("\\begin{document}"))
        return LangType::L_TEXT; // Use TeX lexer

    // NSIS
    if (content.contains("!include") && content.contains("Section"))
        return LangType::L_USER;

    // Dockerfile (special, no extension)
    if (filename.toLower() == "dockerfile" ||
        filename.toLower() == ".dockerignore")
        return LangType::L_BATCH;

    // Diff/Patch
    if (firstLine.startsWith("---") || firstLine.startsWith("diff -") ||
        content.contains("@@ -"))
        return LangType::L_TEXT; // Use Diff lexer

    // Shell scripts without shebang (heuristic)
    if (content.contains("if [") || content.contains("fi;") ||
        content.contains("export ") || content.contains("echo $"))
        return LangType::L_BATCH;

    // Python (indentation-based heuristic)
    if (content.contains("def ") || content.contains("import ") ||
        content.contains("from ") || content.contains("elif "))
        return LangType::L_PYTHON;

    // JSON (starts with { or [)
    QString trimmed = content.trimmed();
    if ((trimmed.startsWith('{') && trimmed.endsWith('}')) ||
        (trimmed.startsWith('[') && trimmed.endsWith(']')))
        return LangType::L_JSON;

    // SQL
    if (content.contains("SELECT ", Qt::CaseInsensitive) ||
        content.contains("INSERT INTO", Qt::CaseInsensitive) ||
        content.contains("CREATE TABLE", Qt::CaseInsensitive))
        return LangType::L_TEXT; // Use SQL lexer

    return LangType::L_TEXT;
}

LangType LexerConfig::detect(const QString& filename, const QString& content) {
    // Try extension first
    LangType fromExt = detectFromExtension(filename);
    if (fromExt != LangType::L_TEXT)
        return fromExt;

    // Try content
    return detectFromContent(content, filename);
}

// ============================================================================
// Language queries
// ============================================================================

QStringList LexerConfig::extensionsFor(LangType lang) {
    const LexerInfo* info = lexerInfo(lang);
    if (info)
        return info->extensions;
    return {};
}

QStringList LexerConfig::allExtensionsGlob() {
    QStringList globs;
    for (const LexerInfo& info : _builtInLexers()) {
        for (const QString& ext : info.extensions) {
            if (ext.startsWith('.')) {
                globs.append(QStringLiteral("*") + ext);
            } else {
                // Special filenames (Makefile, CMakeLists.txt)
                globs.append(ext);
            }
        }
    }
    return globs;
}

QString LexerConfig::displayName(LangType lang) {
    const LexerInfo* info = lexerInfo(lang);
    if (info)
        return info->name;
    return QStringLiteral("Normal Text");
}

bool LexerConfig::hasBlockComments(LangType lang) {
    const LexerInfo* info = lexerInfo(lang);
    if (!info) return false;
    return !info->commentStart.isEmpty();
}

void LexerConfig::getCommentStyle(LangType lang,
                                  QString* singleLine,
                                  QString* blockStart,
                                  QString* blockEnd) {
    const LexerInfo* info = lexerInfo(lang);
    if (!info) {
        if (singleLine) *singleLine = "//";
        if (blockStart) *blockStart = "/*";
        if (blockEnd) *blockEnd = "*/";
        return;
    }
    if (singleLine) *singleLine = info->commentLine;
    if (blockStart) *blockStart = info->commentStart;
    if (blockEnd) *blockEnd = info->commentEnd;
}

LangType LexerConfig::detectHeaderLanguage(const QString& content) {
    // Check for Objective-C indicators
    if (content.contains("@interface") || content.contains("@implementation") ||
        content.contains("@property") || content.contains("@synthesize") ||
        content.contains("@end") || content.contains("NS_") ||
        content.contains("#import <Foundation/") || content.contains("#import <Cocoa/"))
        return LangType::L_OBJC;

    // Check for C++ indicators
    if (content.contains("class ") || content.contains("namespace ") ||
        content.contains("template<") || content.contains("virtual ") ||
        content.contains("override") || content.contains("std::") ||
        content.contains("constexpr") || content.contains("nullptr"))
        return LangType::L_CPP;

    return LangType::L_CPP; // Default to C++ for .h files
}

LangType LexerConfig::detectObjCOrMatlab(const QString& content) {
    if (content.contains("@interface") || content.contains("@implementation") ||
        content.contains("@property") || content.contains("@end"))
        return LangType::L_OBJC;

    // MATLAB: function definitions, end keywords
    if (content.contains("function ") || content.contains("end") ||
        content.contains("clear ") || content.contains("close all"))
        return LangType::L_TEXT; // MATLAB lexer

    return LangType::L_OBJC;
}

bool LexerConfig::isPHPCode(const QString& content) {
    return content.contains("<?php") || content.contains("<?=");
}

bool LexerConfig::isMakefile(const QString& filename) {
    QString name = filename;
    int slash = qMax(name.lastIndexOf('/'), name.lastIndexOf('\\'));
    if (slash >= 0)
        name = name.mid(slash + 1);
    QString lower = name.toLower();
    return lower == "makefile" || lower == "gnumakefile" ||
           lower == "makefile.unix" || lower == "makefile.win" ||
           lower == "makefile.macosx" || lower == "makefile.sunos";
}

// ============================================================================
// UDL loading
// ============================================================================

QVector<UDLPattern> LexerConfig::loadUDL(const QString& configPath) {
    QVector<UDLPattern> patterns;
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return patterns;

    QTextStream in(&file);
    QString currentSection;
    UDLPattern currentPattern;
    bool inKeywords = false;
    int keywordIndex = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("[") && line.endsWith("]")) {
            // Save previous pattern
            if (!currentPattern.name.isEmpty())
                patterns.append(currentPattern);
            currentPattern = UDLPattern();

            currentSection = line.mid(1, line.length() - 2).toLower();
            inKeywords = (currentSection == "keywords");
            keywordIndex = 0;
            continue;
        }

        if (inKeywords) {
            if (line.contains('=')) {
                QStringList parts = line.split('=');
                keywordIndex = parts[0].trimmed().toInt();
            }
        }
    }

    if (!currentPattern.name.isEmpty())
        patterns.append(currentPattern);

    return patterns;
}

void LexerConfig::registerUDLLexer(const QString& name,
                                    const QVector<UDLPattern>& patterns) {
    // In a full implementation, this would register the UDL with the
    // LanguageManager and create a QsciLexerCustom for it.
    // For now, this is a placeholder.
    Q_UNUSED(name);
    Q_UNUSED(patterns);
}
