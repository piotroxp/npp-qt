// StringHelper.cpp - String manipulation utilities implementation
#include "StringHelper.h"
#include "Constants.h"
#include "Types.h"
#include <QString>

namespace StringHelper {

// ============================================================================
// Encoding conversion - using Qt APIs (no codecvt)
// ============================================================================
std::wstring toWStr(const std::string& s) {
    if (s.empty()) return {};
    // Qt6: QString::toStdWString() handles UTF-8 → wchar_t correctly on all platforms
    return QString::fromUtf8(s.data(), static_cast<int>(s.size())).toStdWString();
}

std::string toUtf8(const std::wstring& s) {
    if (s.empty()) return {};
    // Qt6: use QStringView for iterator-pair construction
    QString qstr = QString::fromUtf16(
        reinterpret_cast<const char16_t*>(s.data()), static_cast<int>(s.size()));
    return qstr.toUtf8().constData();
}

std::u16string toUtf16(const std::string& s) {
    if (s.empty()) return {};
    QString qstr = QString::fromUtf8(s.data(), static_cast<int>(s.size()));
    return std::u16string(reinterpret_cast<const char16_t*>(qstr.utf16()),
                           static_cast<size_t>(qstr.size()));
}

std::string utf16ToUtf8(const std::u16string& s) {
    if (s.empty()) return {};
    QString qstr = QString::fromUtf16(s.data(), static_cast<int>(s.size()));
    return qstr.toUtf8().constData();
}

// ============================================================================
// Replace
// ============================================================================
std::string replaceAll(std::string s, std::string_view from, std::string_view to) {
    if (from.empty()) return s;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

std::wstring replaceAll(std::wstring s, std::wstring_view from, std::wstring_view to) {
    if (from.empty()) return s;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::wstring::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

std::string replaceAll(std::string s, const std::vector<std::pair<std::string, std::string>>& replacements) {
    for (const auto& [from, to] : replacements) {
        s = replaceAll(s, from, to);
    }
    return s;
}

QString replaceAll(const QString& s, const QString& from, const QString& to) {
    QString result = s;
    result.replace(from, to);
    return result;
}

// ============================================================================
// Split / Join
// ============================================================================
std::vector<std::string> split(std::string_view s, std::string_view delimiter, bool skipEmpty) {
    std::vector<std::string> result;
    size_t start = 0;
    while (start < s.size()) {
        size_t pos = s.find(delimiter, start);
        if (pos == std::string_view::npos) pos = s.size();
        std::string_view part = s.substr(start, pos - start);
        if (!skipEmpty || !part.empty()) result.emplace_back(part);
        start = pos + delimiter.size();
    }
    return result;
}

std::vector<std::wstring> split(std::wstring_view s, std::wstring_view delimiter, bool skipEmpty) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (start < s.size()) {
        size_t pos = s.find(delimiter, start);
        if (pos == std::wstring_view::npos) pos = s.size();
        std::wstring_view part = s.substr(start, pos - start);
        if (!skipEmpty || !part.empty()) result.emplace_back(part);
        start = pos + delimiter.size();
    }
    return result;
}

std::string join(const std::vector<std::string>& parts, std::string_view separator) {
    if (parts.empty()) return {};
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += separator;
        result += parts[i];
    }
    return result;
}

QStringList split(const QString& s, const QString& delimiter, bool skipEmpty) {
    if (delimiter.isEmpty()) {
        return QStringList() << s;
    }
    if (skipEmpty) {
        return s.split(delimiter, Qt::SkipEmptyParts);
    }
    return s.split(delimiter);
}

QString join(const QStringList& parts, const QString& separator) {
    return parts.join(separator);
}

// ============================================================================
// Levenshtein distance
// ============================================================================
int levenshteinDistance(const QString& a, const QString& b) {
    const int m = a.length();
    const int n = b.length();

    if (m == 0) return n;
    if (n == 0) return m;

    // Use two rows instead of full matrix for memory efficiency
    QVector<int> prevRow(n + 1);
    QVector<int> currRow(n + 1);

    // Initialize first row
    for (int j = 0; j <= n; ++j) {
        prevRow[j] = j;
    }

    for (int i = 1; i <= m; ++i) {
        currRow[0] = i;
        for (int j = 1; j <= n; ++j) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            currRow[j] = std::min({
                prevRow[j] + 1,      // deletion
                currRow[j - 1] + 1,  // insertion
                prevRow[j - 1] + cost // substitution
            });
        }
        std::swap(prevRow, currRow);
    }

    return prevRow[n];
}

// ============================================================================
// JSON escaping
// ============================================================================
QString escapeJson(const QString& s) {
    QString result;
    result.reserve(s.length() * 2);
    for (QChar c : s) {
        switch (c.unicode()) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c.unicode() < 0x20) {
                    result += QString("\\u%1").arg(static_cast<int>(c.unicode()), 4, 16, QChar('0'));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

// ============================================================================
// Shell escaping
// ============================================================================
QString escapeShell(const QString& s) {
    QString result;
    result.reserve(s.length() * 2);
    bool needsQuotes = false;

    for (QChar c : s) {
        if (c.isSpace() || c == '\'' || c == '"' || c == '$' ||
            c == '`' || c == '\\' || c == '!' || c == '#' ||
            c == '&' || c == '*' || c == '?' || c == '~' ||
            c == '(' || c == ')' || c == '[' || c == ']' ||
            c == '{' || c == '}' || c == '|' || c == '<' ||
            c == '>' || c == '^' || c == ';' || c == ':') {
            needsQuotes = true;
            break;
        }
    }

    if (needsQuotes) {
        result += '\'';
        for (QChar c : s) {
            if (c == '\'') result += "'\\''";
            else result += c;
        }
        result += '\'';
    } else {
        result = s;
    }
    return result;
}

// ============================================================================
// FileName / FileExt / FilePath (platform-agnostic)
// ============================================================================
std::string fileName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

std::wstring fileName(const std::wstring& path) {
    size_t pos = path.find_last_of(L"/\\");
    if (pos == std::wstring::npos) return path;
    return path.substr(pos + 1);
}

std::string fileExt(const std::string& path) {
    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos || pos == 0) return "";
    return path.substr(pos + 1);
}

std::string filePath(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return "";
    return path.substr(0, pos);
}

std::string normalizePath(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '\\', '/');
    // Collapse multiple slashes
    size_t pos = 0;
    while ((pos = result.find("//", pos)) != std::string::npos) {
        result.erase(pos, 1);
    }
    return result;
}

// ============================================================================
// Escaping
// ============================================================================
std::string escape(const std::string& s, const std::string& chars) {
    std::string result;
    result.reserve(s.size() * 2);
    for (char c : s) {
        if (chars.find(c) != std::string::npos) {
            result += '\\';
            result += c;
        } else {
            result += c;
        }
    }
    return result;
}

std::string unescape(const std::string& s) {
    std::string result;
    result.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            ++i;
            result += s[i];
        } else {
            result += s[i];
        }
    }
    return result;
}

std::string htmlEscape(const std::string& s) {
    std::string result;
    result.reserve(s.size() * 2);
    for (char c : s) {
        switch (c) {
            case '<':  result += "&lt;"; break;
            case '>':  result += "&gt;"; break;
            case '&':  result += "&amp;"; break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default:   result += c; break;
        }
    }
    return result;
}

std::string regexEscape(const std::string& s) {
    static const std::string special = R"(\.^$|?*+()[]{}%\ )";
    std::string result;
    result.reserve(s.size() * 2);
    for (char c : s) {
        if (special.find(c) != std::string::npos) result += '\\';
        result += c;
    }
    return result;
}

// ============================================================================
// Word wrap
// ============================================================================
std::vector<std::string> wordWrap(const std::string& text, size_t columnWidth) {
    std::vector<std::string> lines;
    if (columnWidth == 0) { lines.push_back(text); return lines; }
    size_t start = 0;
    while (start < text.size()) {
        size_t end = start + columnWidth;
        if (end >= text.size()) {
            lines.emplace_back(text.substr(start));
            break;
        }
        // Find last space before end
        size_t lastSpace = text.rfind(' ', end);
        if (lastSpace == std::string::npos || lastSpace <= start) {
            lastSpace = text.find(' ', end);
            if (lastSpace == std::string::npos) lastSpace = text.size();
        }
        lines.emplace_back(text.substr(start, lastSpace - start));
        start = lastSpace + 1;
    }
    return lines;
}

// ============================================================================
// Locale-aware comparison
// ============================================================================
bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i])))
            return false;
    }
    return true;
}

bool equalsIgnoreCase(const std::wstring& a, const std::wstring& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::towlower(a[i]) != std::towlower(b[i])) return false;
    }
    return true;
}

// ============================================================================
// Tab/space conversion
// ============================================================================
std::string tabsToSpaces(const std::string& text, size_t tabWidth) {
    std::string result;
    result.reserve(text.size() * tabWidth);
    for (char c : text) {
        if (c == '\t') {
            result.append(tabWidth, ' ');
        } else {
            result += c;
        }
    }
    return result;
}

std::string spacesToTabs(const std::string& text, size_t tabWidth) {
    std::string result;
    result.reserve(text.size());
    size_t col = 0;
    size_t i = 0;
    while (i < text.size()) {
        char c = text[i];
        if (c == '\t') {
            result += '\t';
            col = (col / tabWidth + 1) * tabWidth;
            ++i;
        } else if (c == '\n' || c == '\r') {
            result += c;
            col = 0;
            ++i;
        } else if (c == ' ') {
            size_t spaceCount = 0;
            size_t j = i;
            while (j < text.size() && text[j] == ' ') {
                ++spaceCount;
                ++j;
            }
            size_t nextTabStop = (col / tabWidth + 1) * tabWidth;
            if (spaceCount == nextTabStop - col) {
                result += '\t';
                col = nextTabStop;
            } else {
                for (size_t k = 0; k < spaceCount; ++k) {
                    result += ' ';
                    ++col;
                }
            }
            i += spaceCount;
        } else {
            result += c;
            ++col;
            ++i;
        }
    }
    return result;
}


std::string makeEolConsistent(std::string_view text, int eolMode) {
    const char* eol = "\n";
    if (eolMode == static_cast<int>(EolType::EOL_CRLF)) eol = "\r\n";
    else if (eolMode == static_cast<int>(EolType::EOL_CR)) eol = "\r";
    std::string result;
    result.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\r') {
            if (i + 1 < text.size() && text[i + 1] == '\n') ++i;
            result += eol;
        } else if (text[i] == '\n') {
            result += eol;
        } else {
            result += text[i];
        }
    }
    return result;
}

// ============================================================================
// Indentation
// ============================================================================
std::string indentLines(const std::string& text, size_t spaces, const std::string& chars) {
    std::string indent(spaces, ' ');
    std::string result;
    std::istringstream iss(text);
    std::string line;
    bool first = true;
    while (std::getline(iss, line)) {
        if (!first) result += '\n';
        first = false;
        // Preserve existing indentation + add new
        size_t pos = 0;
        while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) ++pos;
        result += line.substr(0, pos);
        result += indent;
        result += line.substr(pos);
    }
    return result;
}

} // namespace StringHelper
