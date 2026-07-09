// LanguageManager.h - Manages language/lexer detection and settings
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "common/NonCopyable.h"
#include "../common/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

class LanguageManager : public NonCopyable {
public:
    // Singleton access
    static LanguageManager& instance() { static LanguageManager m; return m; }

    // Static convenience: detect language from file extension (e.g. ".cpp")
    static LangType detect(const std::string& fileExtension);

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
    std::vector<std::string> getKeywords(LangType lang) const;

    bool isCaseSensitive(LangType lang) const;
    int getIndentSize(LangType lang) const;

private:
    void initDefaultMappings();
    std::unordered_map<std::string, LangType> _extToLang;
    std::unordered_map<LangType, std::string> _langNames;
};
