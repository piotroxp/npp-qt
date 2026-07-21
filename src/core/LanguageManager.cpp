// LanguageManager.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "LanguageManager.h"
#include <QHash>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexeroctave.h>
#include <Qsci/qscilexeryaml.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexertex.h>
#include <Qsci/qscilexerxml.h>
#include <algorithm>
#include <cctype>

LanguageManager::LanguageManager() { initDefaultMappings(); }
LanguageManager::~LanguageManager() = default;

void LanguageManager::initDefaultMappings() {
    _langNames[LangType::L_C] = "C";
    _langNames[LangType::L_CPP] = "C++";
    _langNames[LangType::L_JAVA] = "Java";
    _langNames[LangType::L_CS] = "C#";
    _langNames[LangType::L_OBJC] = "Objective-C";
    _langNames[LangType::L_HTML] = "HTML";
    _langNames[LangType::L_XML] = "XML";
    _langNames[LangType::L_PHP] = "PHP";
    _langNames[LangType::L_PYTHON] = "Python";
    _langNames[LangType::L_JS] = "JavaScript";
    _langNames[LangType::L_JSON] = "JSON";
    _langNames[LangType::L_CSS] = "CSS";
    _langNames[LangType::L_YAML] = "YAML";
    _langNames[LangType::L_MAKEFILE] = "Makefile";
    _langNames[LangType::L_RUBY] = "Ruby";
    _langNames[LangType::L_PERL] = "Perl";
    _langNames[LangType::L_LUA] = "Lua";
    _langNames[LangType::L_MARKDOWN] = "Markdown";
    _langNames[LangType::L_BATCH] = "Batch";
    _langNames[LangType::L_INI] = "INI";
    _langNames[LangType::L_TEXT] = "Normal";

    _extToLang[".c"] = LangType::L_C;
    _extToLang[".cpp"] = LangType::L_CPP;
    _extToLang[".cxx"] = LangType::L_CPP;
    _extToLang[".cc"] = LangType::L_CPP;
    _extToLang[".h"] = LangType::L_CPP;
    _extToLang[".hpp"] = LangType::L_CPP;
    _extToLang[".hh"] = LangType::L_CPP;
    _extToLang[".java"] = LangType::L_JAVA;
    _extToLang[".cs"] = LangType::L_CS;
    _extToLang[".m"] = LangType::L_OBJC;
    _extToLang[".mm"] = LangType::L_OBJC;
    _extToLang[".html"] = LangType::L_HTML;
    _extToLang[".htm"] = LangType::L_HTML;
    _extToLang[".xml"] = LangType::L_XML;
    _extToLang[".xaml"] = LangType::L_XAML;
    _extToLang[".php"] = LangType::L_PHP;
    _extToLang[".phtml"] = LangType::L_PHP;
    _extToLang[".py"] = LangType::L_PYTHON;
    _extToLang[".pyw"] = LangType::L_PYTHON;
    _extToLang[".pyx"] = LangType::L_PYTHON;
    _extToLang[".js"] = LangType::L_JS;
    _extToLang[".jsx"] = LangType::L_JS;
    _extToLang[".mjs"] = LangType::L_JS;
    _extToLang[".cjs"] = LangType::L_JS;
    _extToLang[".json"] = LangType::L_JSON;
    _extToLang[".css"] = LangType::L_CSS;
    _extToLang[".scss"] = LangType::L_CSS;
    _extToLang[".yaml"] = LangType::L_YAML;
    _extToLang[".yml"] = LangType::L_YAML;
    _extToLang[".makefile"] = LangType::L_MAKEFILE;
    _extToLang[".mk"] = LangType::L_MAKEFILE;
    _extToLang[".rb"] = LangType::L_RUBY;
    _extToLang[".pl"] = LangType::L_PERL;
    _extToLang[".pm"] = LangType::L_PERL;
    _extToLang[".lua"] = LangType::L_LUA;
    _extToLang[".md"] = LangType::L_MARKDOWN;
    _extToLang[".markdown"] = LangType::L_MARKDOWN;
    _extToLang[".bat"] = LangType::L_BATCH;
    _extToLang[".cmd"] = LangType::L_BATCH;
    _extToLang[".ini"] = LangType::L_INI;
    _extToLang[".sh"] = LangType::L_BASH;
    _extToLang[".bash"] = LangType::L_BASH;
    _extToLang[".zsh"] = LangType::L_BASH;
    _extToLang[".sql"] = LangType::L_SQL;
    _extToLang[".tex"] = LangType::L_TEX;
    _extToLang[".diff"] = LangType::L_DIFF;
    _extToLang[".patch"] = LangType::L_DIFF;
    _extToLang[".go"] = LangType::L_GO;
    _extToLang[".rs"] = LangType::L_RUST;
    _extToLang[".swift"] = LangType::L_SWIFT;
    _extToLang[".kt"] = LangType::L_KOTLIN;
    _extToLang[".kts"] = LangType::L_KOTLIN;
    _extToLang[".scala"] = LangType::L_SCALA;
    _extToLang[".ts"] = LangType::L_TS;
    _extToLang[".tsx"] = LangType::L_TS;
    _extToLang[".lisp"] = LangType::L_LISP;
    _extToLang[".clj"] = LangType::L_CLOJURE;
    _extToLang[".hs"] = LangType::L_HASKELL;
    _extToLang[".erl"] = LangType::L_ERLANG;
    _extToLang[".ex"] = LangType::L_ELIXIR;
    _extToLang[".exs"] = LangType::L_ELIXIR;
    _extToLang[".f90"] = LangType::L_FORTRAN;
    _extToLang[".f95"] = LangType::L_FORTRAN;
    _extToLang[".pas"] = LangType::L_PASCAL;
    _extToLang[".pp"] = LangType::L_PASCAL;
    _extToLang[".ada"] = LangType::L_ADA;
    _extToLang[".adb"] = LangType::L_ADA;
    _extToLang[".v"] = LangType::L_VERILOG;
    _extToLang[".sv"] = LangType::L_VERILOG;
    _extToLang[".vhd"] = LangType::L_VHDL;
    _extToLang[".vhdl"] = LangType::L_VHDL;
    _extToLang[".asm"] = LangType::L_ASM;
    _extToLang[".s"] = LangType::L_ASM;
    _extToLang[".tcl"] = LangType::L_TCL;
    _extToLang[".ps1"] = LangType::L_POWERSHELL;
    _extToLang[".psm1"] = LangType::L_POWERSHELL;
    _extToLang[".nsi"] = LangType::L_NSIS;
    _extToLang[".dockerfile"] = LangType::L_DOCKERFILE;
    _extToLang[".cmake"] = LangType::L_CMAKE;
    _extToLang[".r"] = LangType::L_R;
    _extToLang[".jl"] = LangType::L_JULIA;
}

LangType LanguageManager::detectLanguage(const std::string& fileName, const std::string& firstLine) const {
    size_t dot = fileName.rfind('.');
    if (dot != std::string::npos) {
        std::string ext = fileName.substr(dot);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        auto it = _extToLang.find(ext);
        if (it != _extToLang.end()) return it->second;
    }
    if (!firstLine.empty()) return detectLanguageFromContent(firstLine);
    return LangType::L_TEXT;
}

LangType LanguageManager::detectLanguageFromContent(const std::string& content) const {
    if (content.find("#!/bin/bash") == 0 || content.find("#!/bin/sh") == 0 ||
        content.find("#!/usr/bin/env bash") == 0 || content.find("#!/usr/bin/env sh") == 0)
        return LangType::L_BASH;
    if (content.find("<?php") != std::string::npos) return LangType::L_PHP;
    if (content.find("<html") != std::string::npos) return LangType::L_HTML;
    if (content.find("{") != std::string::npos && content.find(":") != std::string::npos) return LangType::L_JSON;
    return LangType::L_TEXT;
}

std::string LanguageManager::getLanguageName(LangType lang) const {
    auto it = _langNames.find(lang);
    return it != _langNames.end() ? it->second : "Normal";
}

std::string LanguageManager::getLanguageExtension(LangType lang) const {
    switch (lang) {
        case LangType::L_CPP: return ".cpp";
        case LangType::L_JAVA: return ".java";
        case LangType::L_PYTHON: return ".py";
        case LangType::L_JS: return ".js";
        case LangType::L_HTML: return ".html";
        case LangType::L_CSS: return ".css";
        case LangType::L_MARKDOWN: return ".md";
        default: return ".txt";
    }
}

std::string LanguageManager::getLexerName(LangType lang) const {
    switch (lang) {
        case LangType::L_CPP: return "cpp";
        case LangType::L_JAVA: return "java";
        case LangType::L_PYTHON: return "python";
        case LangType::L_JS: return "javascript";
        case LangType::L_HTML: return "html";
        case LangType::L_CSS: return "css";
        case LangType::L_JSON: return "json";
        case LangType::L_XML: return "xml";
        case LangType::L_MARKDOWN: return "markdown";
        case LangType::L_YAML: return "yaml";
        case LangType::L_RUBY: return "ruby";
        case LangType::L_PERL: return "perl";
        case LangType::L_PHP: return "php";
        case LangType::L_LUA: return "lua";
        case LangType::L_BATCH: return "batch";
        case LangType::L_MAKEFILE: return "makefile";
        case LangType::L_INI: return "ini";
        default: return "text";
    }
}

void LanguageManager::setLanguageForExtension(const std::string& ext, LangType lang) {
    _extToLang[ext] = lang;
}

LangType LanguageManager::getLanguageForExtension(const std::string& ext) const {
    // Normalize: callers (Buffer ctor, FileManager::detectLanguage) feed us
    // the bare suffix from QFileInfo::suffix() which excludes the leading dot.
    // Internal map keys include the dot (".py", ".cpp", ...). Without this
    // normalization every file ends up mapped to L_TEXT and no lexer is
    // attached, so syntax highlighting is silently disabled. Accept both forms
    // and lowercase the input so ".PY" / "PY" work too.
    if (ext.empty()) return LangType::L_TEXT;
    std::string e = ext;
    std::transform(e.begin(), e.end(), e.begin(), ::tolower);
    if (e.front() != '.') e.insert(e.begin(), '.');
    auto it = _extToLang.find(e);
    if (it != _extToLang.end()) return it->second;
    // Also try without the dot in case the key was inserted bare (defensive:
    // supports existing callers that prepended a dot via setLanguageForExtension).
    std::string bare = e.substr(1);
    it = _extToLang.find(bare);
    return it != _extToLang.end() ? it->second : LangType::L_TEXT;
}

std::vector<LangType> LanguageManager::getAllLanguages() const {
    std::vector<LangType> langs;
    for (auto& [lang, _] : _langNames) langs.push_back(lang);
    return langs;
}

std::unordered_map<int, std::string> LanguageManager::getKeywords(LangType lang) const {
    std::unordered_map<int, std::string> kws;
    switch (lang) {
        case LangType::L_CPP:
            kws[0] = "alignas alignof and and_eq asm auto bitand bitor bool break case char char16_t char32_t class compl const constexpr const_cast continue decltype default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new noexcept not not_eq null nullptr operator or or_eq private protected public register reinterpret_cast return short signed sizeof static static_assert static_cast struct switch template this thread_local throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq";
            kws[1] = "NULL TRUE FALSE";
            kws[2] = "__FILE__ __LINE__ __DATE__ __TIME__ __STDC__ __STDC_VERSION__";
            kws[3] = "int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t size_t ssize_t ptrdiff_t";
            break;
        case LangType::L_C:
            kws[0] = "auto break case char const continue default do double else enum extern float for goto if inline int long register return short signed sizeof static struct switch typedef union unsigned void volatile while _Bool _Complex _Imaginary";
            kws[1] = "NULL TRUE FALSE";
            kws[2] = "__FILE__ __LINE__ __DATE__ __TIME__ __STDC__";
            kws[3] = "int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t size_t";
            break;
        case LangType::L_JAVA:
            kws[0] = "abstract assert boolean break byte case catch char class const continue debugger default do double else enum extends final finally float for goto if implements import instanceof int interface long native new package private protected public return short static strictfp super switch synchronized this throw throws transient try void volatile while true false null";
            break;
        case LangType::L_PYTHON:
            kws[0] = "and as assert async await break class continue def del elif else except finally for from global if import in is lambda nonlocal not or pass raise return try while with yield True False None";
            kws[1] = "print input len range str int float list dict set tuple bool bytes";
            break;
        case LangType::L_JS:
            kws[0] = "async await break case catch class const continue debugger default delete do else enum eval export extends false finally for function if implements in instanceof interface let new null package private protected return static super switch this throw true try typeof var void while with yield";
            kws[1] = "console window document Array Boolean Date Error Function JSON Math Number Object RegExp String Symbol Map Set WeakMap WeakSet Promise";
            break;
        case LangType::L_BATCH:
            kws[0] = "if then else elif fi case esac for select while until do done in function time coproc subshell background bang negate export readonly declare local typeset unset shift set source return exit break continue eval exec builtin command pushd popd dirs logout";
            break;
        case LangType::L_USER: {
            // Pull keywords for the currently-active UDL (if any) from the
            // registry populated by Application via UdlManager on buffer
            // activation. Without an active UDL we fall back to an empty set
            // (no autocomplete).
            const QString udlName = _activeUdlName;
            if (udlName.isEmpty()) break;
            for (int i = 0; i < 9; ++i) {
                const QString s = getUdlKeywordSet(udlName, i);
                if (!s.isEmpty()) {
                    kws[i] = s.toStdString();
                }
            }
            break;
        }
        default: break;
    }
    return kws;
}

bool LanguageManager::isCaseSensitive(LangType lang) const {
    return lang != LangType::L_BATCH && lang != LangType::L_INI;
}

int LanguageManager::getIndentSize(LangType lang) const {
    (void)lang;
    return 4;
}

std::string LanguageManager::getCommentLine(LangType lang) const {
    switch (lang) {
        case LangType::L_PYTHON:
        case LangType::L_BATCH:
        case LangType::L_RUBY:
        case LangType::L_PERL:
            return "#";
        case LangType::L_MAKEFILE:
            return "#";
        case LangType::L_LUA:
            return "--";
        case LangType::L_HTML:
        case LangType::L_XML:
        case LangType::L_CSS:
            return "//";
        default:
            return "//";
    }
}

std::string LanguageManager::getCommentStart(LangType lang) const {
    switch (lang) {
        case LangType::L_CPP:
        case LangType::L_C:
        case LangType::L_JAVA:
        case LangType::L_CS:
        case LangType::L_JS:
        case LangType::L_JSON:
            return "/*";
        case LangType::L_PYTHON:
        case LangType::L_RUBY:
        case LangType::L_PERL:
        case LangType::L_LUA:
        case LangType::L_BATCH:
            return "";  // Single line only
        case LangType::L_HTML:
        case LangType::L_XML:
        case LangType::L_PHP:
        case LangType::L_CSS:
            return "<!--";
        default:
            return "/*";
    }
}

std::string LanguageManager::getCommentEnd(LangType lang) const {
    switch (lang) {
        case LangType::L_CPP:
        case LangType::L_C:
        case LangType::L_JAVA:
        case LangType::L_CS:
        case LangType::L_JS:
        case LangType::L_JSON:
            return "*/";
        case LangType::L_PYTHON:
        case LangType::L_RUBY:
        case LangType::L_PERL:
        case LangType::L_LUA:
        case LangType::L_BATCH:
            return "";  // Single line only
        case LangType::L_HTML:
        case LangType::L_XML:
        case LangType::L_PHP:
        case LangType::L_CSS:
            return "-->";
        default:
            return "*/";
    }
}

// Static convenience: detect language from file extension
LangType LanguageManager::detect(const std::string& fileExtension) {
    return instance().getLanguageForExtension(fileExtension);
}

// Map a string name to LangType
LangType LanguageManager::mapStringToLang(const QString& name) {
    static const QHash<QString, LangType> map = {
        {"c",           LangType::L_C},
        {"c++",         LangType::L_CPP},
        {"cpp",         LangType::L_CPP},
        {"java",        LangType::L_JAVA},
        {"csharp",      LangType::L_CS},
        {"c#",          LangType::L_CS},
        {"objective-c", LangType::L_OBJC},
        {"html",        LangType::L_HTML},
        {"xml",         LangType::L_XML},
        {"php",         LangType::L_PHP},
        {"python",      LangType::L_PYTHON},
        {"python3",     LangType::L_PYTHON},
        {"javascript",  LangType::L_JS},
        {"js",          LangType::L_JS},
        {"json",        LangType::L_JSON},
        {"css",         LangType::L_CSS},
        {"makefile",    LangType::L_MAKEFILE},
        {"markdown",    LangType::L_MARKDOWN},
        {"md",          LangType::L_MARKDOWN},
        {"ruby",        LangType::L_RUBY},
        {"rb",          LangType::L_RUBY},
        {"perl",        LangType::L_PERL},
        {"pl",          LangType::L_PERL},
        {"lua",         LangType::L_LUA},
        {"yaml",        LangType::L_YAML},
        {"yml",         LangType::L_YAML},
        {"batch",       LangType::L_BATCH},
        {"bat",         LangType::L_BATCH},
        {"ini",         LangType::L_INI},
        {"normal_text", LangType::L_TEXT},
        {"text",        LangType::L_TEXT},
    };
    return map.value(name.toLower(), LangType::L_TEXT);
}


// Create a QsciLexer for the given language (nullptr for L_TEXT/unknown)
QsciLexer* LanguageManager::getLexer(LangType lang) const {
    return createLexer(lang);
}

QsciLexer* LanguageManager::createLexer(LangType lang) {
    switch (lang) {
        case LangType::L_CPP:
        case LangType::L_C:        return new QsciLexerCPP;
        case LangType::L_JAVA:      return new QsciLexerJava;
        case LangType::L_CS:
        case LangType::L_CSHARP:    return new QsciLexerCSharp;
        case LangType::L_HTML:      return new QsciLexerHTML;
        case LangType::L_XML:
        case LangType::L_XAML:      return new QsciLexerXML;
        case LangType::L_CSS:       return new QsciLexerCSS;
        case LangType::L_JS:        return new QsciLexerJavaScript;
        case LangType::L_JSON:      return new QsciLexerJSON;
        case LangType::L_MARKDOWN:  return new QsciLexerMarkdown;
        case LangType::L_MAKEFILE:
        case LangType::L_CMAKE:     return new QsciLexerMakefile;
        case LangType::L_LUA:       return new QsciLexerLua;
        case LangType::L_BATCH:     return new QsciLexerBatch;
        case LangType::L_BASH:      return new QsciLexerBash;
        case LangType::L_RUBY:      return new QsciLexerRuby;
        case LangType::L_PERL:      return new QsciLexerPerl;
        case LangType::L_PYTHON:    return new QsciLexerPython;
        case LangType::L_YAML:      return new QsciLexerYAML;
        case LangType::L_OBJC:      return new QsciLexerCPP;     // QScintilla ships no ObjC lexer; fall back to C++ (closest grammar)
        case LangType::L_SQL:       return new QsciLexerSQL;
        case LangType::L_TEX:       return new QsciLexerTeX;
        case LangType::L_DIFF:      return new QsciLexerDiff;
        // PHP/HTML have no dedicated QScintilla lexer; falling back to HTML is
        // a best-effort (highlights tags within <?php ... ?> blocks). Returning
        // nullptr would mean plain text — better to give partial coloring.
        case LangType::L_PHP:       return new QsciLexerHTML;
        // Languages without QScintilla coverage -> plain text highlighting.
        case LangType::L_GO:
        case LangType::L_RUST:
        case LangType::L_SWIFT:
        case LangType::L_KOTLIN:
        case LangType::L_SCALA:
        case LangType::L_TS:
        case LangType::L_LISP:
        case LangType::L_CLOJURE:
        case LangType::L_HASKELL:
        case LangType::L_ERLANG:
        case LangType::L_ELIXIR:
        case LangType::L_FORTRAN:
        case LangType::L_PASCAL:
        case LangType::L_ADA:
        case LangType::L_VERILOG:
        case LangType::L_VHDL:
        case LangType::L_ASM:
        case LangType::L_TCL:
        case LangType::L_POWERSHELL:
        case LangType::L_NSIS:
        case LangType::L_NSIS2:
        case LangType::L_DOCKERFILE:
        case LangType::L_R:
        case LangType::L_JULIA:
        case LangType::L_MATLAB:
        case LangType::L_NFO:
        case LangType::L_REGEX:
        case LangType::L_GRAPHVIZ:
        case LangType::L_FSHARP:
        case LangType::L_GROOVY:
        case LangType::L_WEBASSEMBLY:
        case LangType::L_PROPERTIES:
            // No QScintilla coverage — leave as plain text (lexer == nullptr).
            return nullptr;
        case LangType::L_TEXT:
        case LangType::L_USER:
        case LangType::L_EXTERNAL:
        case LangType::L_REGISTRY:
        default:
            return nullptr;
    }
}

// ============================================================================
// UDL keyword registry
// ============================================================================

void LanguageManager::registerUdlKeywords(const QString& udlName,
                                           const std::array<QString, 9>& keywordSets) {
    if (udlName.isEmpty()) return;
    _udlKeywordSets[udlName] = keywordSets;
}

bool LanguageManager::hasUdlKeywords(const QString& udlName) const {
    if (udlName.isEmpty()) return false;
    auto it = _udlKeywordSets.find(udlName);
    if (it == _udlKeywordSets.end()) return false;
    // Treat as "has keywords" only if at least one set is non-empty.
    for (const auto& s : it->second) {
        if (!s.isEmpty()) return true;
    }
    return false;
}

QString LanguageManager::getUdlKeywordSet(const QString& udlName, int setIndex) const {
    if (setIndex < 0 || setIndex >= 9) return QString();
    auto it = _udlKeywordSets.find(udlName);
    if (it == _udlKeywordSets.end()) return QString();
    return it->second[setIndex];
}
