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
    _extToLang[".h"] = LangType::L_CPP;
    _extToLang[".hpp"] = LangType::L_CPP;
    _extToLang[".java"] = LangType::L_JAVA;
    _extToLang[".cs"] = LangType::L_CS;
    _extToLang[".m"] = LangType::L_OBJC;
    _extToLang[".html"] = LangType::L_HTML;
    _extToLang[".htm"] = LangType::L_HTML;
    _extToLang[".xml"] = LangType::L_XML;
    _extToLang[".xaml"] = LangType::L_XAML;
    _extToLang[".php"] = LangType::L_PHP;
    _extToLang[".py"] = LangType::L_PYTHON;
    _extToLang[".js"] = LangType::L_JS;
    _extToLang[".json"] = LangType::L_JSON;
    _extToLang[".css"] = LangType::L_CSS;
    _extToLang[".yaml"] = LangType::L_YAML;
    _extToLang[".yml"] = LangType::L_YAML;
    _extToLang[".makefile"] = LangType::L_MAKEFILE;
    _extToLang[".rb"] = LangType::L_RUBY;
    _extToLang[".pl"] = LangType::L_PERL;
    _extToLang[".lua"] = LangType::L_LUA;
    _extToLang[".md"] = LangType::L_MARKDOWN;
    _extToLang[".bat"] = LangType::L_BATCH;
    _extToLang[".cmd"] = LangType::L_BATCH;
    _extToLang[".ini"] = LangType::L_INI;
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
    if (content.find("#!/bin/bash") == 0 || content.find("#!/bin/sh") == 0) return LangType::L_BATCH;
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
    auto it = _extToLang.find(ext);
    return it != _extToLang.end() ? it->second : LangType::L_TEXT;
}

std::vector<LangType> LanguageManager::getAllLanguages() const {
    std::vector<LangType> langs;
    for (auto& [lang, _] : _langNames) langs.push_back(lang);
    return langs;
}

std::vector<std::string> LanguageManager::getKeywords(LangType lang) const {
    (void)lang;
    return {};
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
        case LangType::L_CS:        return new QsciLexerCSharp;
        case LangType::L_HTML:      return new QsciLexerHTML;
        case LangType::L_XML:       return new QsciLexerXML;
        case LangType::L_CSS:       return new QsciLexerCSS;
        case LangType::L_JS:        return new QsciLexerJavaScript;
        case LangType::L_JSON:      return new QsciLexerJSON;
        case LangType::L_MARKDOWN:  return new QsciLexerMarkdown;
        case LangType::L_MAKEFILE:  return new QsciLexerMakefile;
        case LangType::L_LUA:       return new QsciLexerLua;
        case LangType::L_BATCH:     return new QsciLexerBatch;
        case LangType::L_RUBY:      return new QsciLexerRuby;
        case LangType::L_PERL:      return new QsciLexerPerl;
        case LangType::L_PYTHON:    return new QsciLexerPython;
        case LangType::L_YAML:      return new QsciLexerYAML;
        case LangType::L_OBJC:      return new QsciLexerBash;     // Shell/ObjC scripts use Bash lexer
        default:                     return nullptr;
    }
}
