// StringHelper.h - String manipulation utilities for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <algorithm>
#include <cctype>
#include <cwctype>
#include <locale>
#include <sstream>
#include <iomanip>
#include <regex>
#include <QString>
#include <QStringList>

namespace StringHelper {

// ============================================================================
// Encoding helpers (UTF-8 / UTF-16 / UTF-32) - using Qt APIs
// ============================================================================
std::wstring        toWStr(const std::string& s);
std::string         toUtf8(const std::wstring& s);
std::u16string     toUtf16(const std::string& s);
std::string         utf16ToUtf8(const std::u16string& s);

// ============================================================================
// Case conversion
// ============================================================================
inline std::string  toLower(std::string_view s) {
    std::string r(s);
    std::transform(r.begin(), r.end(), r.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return r;
}
inline std::wstring toLower(std::wstring_view s) {
    std::wstring r(s);
    std::transform(r.begin(), r.end(), r.begin(), std::towlower);
    return r;
}
inline QString toLower(const QString& s) {
    return s.toLower();
}
inline std::string  toUpper(std::string_view s) {
    std::string r(s);
    std::transform(r.begin(), r.end(), r.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return r;
}
inline std::wstring toUpper(std::wstring_view s) {
    std::wstring r(s);
    std::transform(r.begin(), r.end(), r.begin(), std::towupper);
    return r;
}
inline QString toUpper(const QString& s) {
    return s.toUpper();
}

// ============================================================================
// Trim / Strip
// ============================================================================
inline std::string  trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) ++start;
    auto end = s.end();
    while (end != start && std::isspace(*(end - 1))) --end;
    return std::string(start, end);
}
inline std::wstring trim(const std::wstring& s) {
    auto start = s.begin();
    while (start != s.end() && std::iswspace(*start)) ++start;
    auto end = s.end();
    while (end != start && std::iswspace(*(end - 1))) --end;
    return std::wstring(start, end);
}
inline QString trim(const QString& s) {
    return s.trimmed();
}
inline std::string  trimLeft(const std::string& s) {
    auto start = std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); });
    return std::string(start, s.end());
}
inline std::string  trimRight(const std::string& s) {
    auto end = std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base();
    return std::string(s.begin(), end);
}
inline QString trimLeft(const QString& s) {
    return s.trimmed();
}
inline QString trimRight(const QString& s) {
    int len = s.length();
    int i = len - 1;
    while (i >= 0 && s[i].isSpace()) --i;
    return s.left(i + 1);
}

// ============================================================================
// Replace / Substitute
// ============================================================================
std::string  replaceAll(std::string s, std::string_view from, std::string_view to);
std::wstring replaceAll(std::wstring s, std::wstring_view from, std::wstring_view to);
std::string  replaceAll(std::string s, const std::vector<std::pair<std::string, std::string>>& replacements);
QString      replaceAll(const QString& s, const QString& from, const QString& to);

// ============================================================================
// Split / Join
// ============================================================================
std::vector<std::string>  split(std::string_view s, std::string_view delimiter, bool skipEmpty = true);
std::vector<std::wstring> split(std::wstring_view s, std::wstring_view delimiter, bool skipEmpty = true);
std::string                join(const std::vector<std::string>& parts, std::string_view separator);
QStringList                split(const QString& s, const QString& delimiter, bool skipEmpty = true);
QString                     join(const QStringList& parts, const QString& separator);

// ============================================================================
// StartsWith / EndsWith
// ============================================================================
inline bool startsWith(std::string_view s, std::string_view prefix) {
    return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}
inline bool startsWith(std::wstring_view s, std::wstring_view prefix) {
    return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}
inline bool startsWith(const QString& s, const QString& prefix) {
    return s.startsWith(prefix);
}
inline bool endsWith(std::string_view s, std::string_view suffix) {
    return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}
inline bool endsWith(std::wstring_view s, std::wstring_view suffix) {
    return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}
inline bool endsWith(const QString& s, const QString& suffix) {
    return s.endsWith(suffix);
}

// ============================================================================
// Contains
// ============================================================================
inline bool contains(std::string_view s, std::string_view sub) {
    return s.find(sub) != std::string_view::npos;
}
inline bool contains(std::wstring_view s, std::wstring_view sub) {
    return s.find(sub) != std::wstring_view::npos;
}
inline bool contains(const QString& s, const QString& sub) {
    return s.contains(sub);
}

// ============================================================================
// Format helpers
// ============================================================================
template<typename... Args>
std::string format(const std::string& fmt, Args... args) {
    size_t size = snprintf(nullptr, 0, fmt.c_str(), args...) + 1;
    std::string buf(size, '\0');
    snprintf(buf.data(), size, fmt.c_str(), args...);
    buf.pop_back();
    return buf;
}

// ============================================================================
// Numeric conversion
// ============================================================================
inline int toInt(const std::string& s, int def = 0) {
    try { return std::stoi(trim(s)); } catch (...) { return def; }
}
inline int64_t toLong(const std::string& s, int64_t def = 0) {
    try { return std::stoll(trim(s)); } catch (...) { return def; }
}
inline double toDouble(const std::string& s, double def = 0.0) {
    try { return std::stod(trim(s)); } catch (...) { return def; }
}
inline std::string fromInt(int v) { return std::to_string(v); }
inline std::string fromLong(long v) { return std::to_string(v); }
inline std::string fromDouble(double v, int precision = 6) {
    std::ostringstream oss;
    oss << std::setprecision(precision) << std::fixed << v;
    return oss.str();
}

// ============================================================================
// QString numeric conversion
// ============================================================================
inline int toInt(const QString& s, int def = 0) {
    bool ok = false;
    int val = s.toInt(&ok);
    return ok ? val : def;
}
inline int64_t toLong(const QString& s, int64_t def = 0) {
    bool ok = false;
    int64_t val = s.toLongLong(&ok);
    return ok ? val : def;
}
inline double toDouble(const QString& s, double def = 0.0) {
    bool ok = false;
    double val = s.toDouble(&ok);
    return ok ? val : def;
}
inline QString toString(int val) { return QString::number(val); }
inline QString toString(int64_t val) { return QString::number(val); }
inline QString toString(double val, int precision = 6) { return QString::number(val, 'f', precision); }
inline QString toString(bool val) { return val ? "true" : "false"; }

// ============================================================================
// QString padding
// ============================================================================
inline QString padLeft(const QString& s, int width, QChar fill = ' ') {
    return s.rightJustified(width, fill, true);
}
inline QString padRight(const QString& s, int width, QChar fill = ' ') {
    return s.leftJustified(width, fill, true);
}

// ============================================================================
// QString manipulation
// ============================================================================
inline QString reverse(const QString& s) {
    QString result = s;
    std::reverse(result.begin(), result.end());
    return result;
}
inline QString capitalize(const QString& s) {
    if (s.isEmpty()) return s;
    return s[0].toUpper() + s.mid(1).toLower();
}
inline QString titleCase(const QString& s) {
    QString result;
    bool capitalizeNext = true;
    for (QChar c : s) {
        if (c.isSpace() || c == '-' || c == '_') {
            capitalizeNext = true;
            result += c;
        } else if (capitalizeNext) {
            result += c.toUpper();
            capitalizeNext = false;
        } else {
            result += c.toLower();
        }
    }
    return result;
}
inline QString truncate(const QString& s, int maxLen, const QString& ellipsis = "...") {
    if (s.length() <= maxLen) return s;
    return s.left(maxLen - ellipsis.length()) + ellipsis;
}
inline bool isBlank(const QString& s) {
    for (const QChar& c : s) {
        if (!c.isSpace()) return false;
    }
    return true;
}

// ============================================================================
// Levenshtein distance
// ============================================================================
int levenshteinDistance(const QString& a, const QString& b);

// ============================================================================
// Escaping
// ============================================================================
QString escapeJson(const QString& s);
QString escapeShell(const QString& s);

// ============================================================================
// Path helpers (these delegate to FileHelper)
// ============================================================================
std::string  fileName(const std::string& path);
std::wstring fileName(const std::wstring& path);
std::string  fileExt(const std::string& path);
std::string  filePath(const std::string& path);
std::string  normalizePath(const std::string& path);

// ============================================================================
// Escaping / Unescaping
// ============================================================================
std::string escape(const std::string& s, const std::string& chars);
std::string unescape(const std::string& s);
std::string htmlEscape(const std::string& s);
std::string regexEscape(const std::string& s);

// ============================================================================
// Word wrap helper
// ============================================================================
std::vector<std::string> wordWrap(const std::string& text, size_t columnWidth);

// ============================================================================
// String comparison (locale-aware)
// ============================================================================
bool equalsIgnoreCase(const std::string& a, const std::string& b);
bool equalsIgnoreCase(const std::wstring& a, const std::wstring& b);

// ============================================================================
// Tab/space conversion
// ============================================================================
std::string tabsToSpaces(const std::string& text, size_t tabWidth);
std::string spacesToTabs(const std::string& text, size_t tabWidth);
std::string makeEolConsistent(std::string_view text, int eolMode);

// ============================================================================
// Indentation helpers
// ============================================================================
std::string indentLines(const std::string& text, size_t spaces, const std::string& chars = " \t");

} // namespace StringHelper
