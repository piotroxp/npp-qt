// LanguageManager.h - Manages language/lexer detection and settings
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include "../common/Types.h"
#include <QString>
#include <string>
#include <vector>
#include <unordered_map>

class QsciLexer;  // forward declaration

class LanguageManager : public NonCopyable {
public:
    // Singleton access
    static LanguageManager& instance() { static LanguageManager m; return m; }

    // Create and return a QsciLexer for the given language (caller owns it, may be nullptr)
    QsciLexer* getLexer(LangType lang) const;

    // Map a string name (e.g. "c++", "python", "javascript") to LangType
    static LangType mapStringToLang(const QString& name);

    // Static convenience: detect language from file extension (e.g. ".cpp")
    static LangType detect(const std::string& fileExtension);

    // Create a QsciLexer for a given language (caller owns the lexer)
    static QsciLexer* createLexer(LangType lang);

    LanguageManager();
    ~LanguageManager();

    LangType detectLanguage(const std::string& fileName, const std::string& firstLine = "") const;
    LangType detectLanguageFromContent(const std::string& content) const;

    std::string getLanguageName(LangType lang) const;
    std::string getLanguageExtension(LangType lang) const;
    std::string getLexerName(LangType lang) const;

    void setLanguageForExtension(const std::string& ext, LangType lang);
    LangType getLanguageForExtension(const std::string& ext) const;

    std::vector<LangType> getAllLanguages() const;
    std::unordered_map<int, std::string> getKeywords(LangType lang) const;

    bool isCaseSensitive(LangType lang) const;
    int getIndentSize(LangType lang) const;

    // Comment symbol helpers (for Buffer integration)
    std::string getCommentLine(LangType lang) const;
    std::string getCommentStart(LangType lang) const;
    std::string getCommentEnd(LangType lang) const;

private:
    void initDefaultMappings();
    std::unordered_map<std::string, LangType> _extToLang;
    std::unordered_map<LangType, std::string> _langNames;
};
