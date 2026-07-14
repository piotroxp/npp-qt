// Util.cpp - General utility functions implementation
#include "Util.h"
#include "Constants.h"
#include "StringHelper.h"
#include "FileHelper.h"
#include <QString>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <shlobj.h>
#else
#include <sys/sysinfo.h>
#include <pwd.h>
#endif

// ============================================================================
// Logging
// ============================================================================
namespace {
    std::mutex        g_logMutex;
    std::string       g_logFilePath;
    LogLevel          g_logLevel = LogLevel::Info;

    const char* levelToString(LogLevel lvl) {
        switch (lvl) {
            case LogLevel::Debug:    return "DEBUG";
            case LogLevel::Info:     return "INFO";
            case LogLevel::Warning:  return "WARN";
            case LogLevel::Error:    return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
        }
        return "?";
    }
}

void logMessage(LogLevel level, const std::string& msg) {
    if (level < g_logLevel) return;
    std::lock_guard<std::mutex> lock(g_logMutex);
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    char timeBuf[64];
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&timeT));

    std::ostringstream oss;
    oss << "[" << timeBuf << "] [" << levelToString(level) << "] " << msg << "\n";

    FILE* out = stdout;
    if (!g_logFilePath.empty()) {
        FILE* f = fopen(g_logFilePath.c_str(), "a");
        if (f) { out = f; } else { out = stdout; }
    }
    fputs(oss.str().c_str(), out);
    if (out != stdout) fclose(out);
}

void logDebug(const std::string& msg) { logMessage(LogLevel::Debug, msg); }
void logInfo(const std::string& msg) { logMessage(LogLevel::Info, msg); }
void logWarning(const std::string& msg) { logMessage(LogLevel::Warning, msg); }
void logError(const std::string& msg) { logMessage(LogLevel::Error, msg); }
std::string getLogFilePath() { return g_logFilePath; }
void setLogLevel(LogLevel level) { g_logLevel = level; }
void setLogFile(const std::string& path) { g_logFilePath = path; }

// ============================================================================
// Assertion
// ============================================================================
void assertionFailed(const char* expr, const char* file, int line) {
    std::ostringstream oss;
    oss << "ASSERTION FAILED: " << expr << " at " << file << ":" << line;
    logError(oss.str());
#if defined(_WIN32) || defined(_WIN64)
    DebugBreak();
#else
    __builtin_trap();
#endif
}

void debugOutput(const std::string& msg) {
#if defined(_WIN32) || defined(_WIN64)
    OutputDebugStringA(msg.c_str());
#else
    fputs(msg.c_str(), stderr);
#endif
}

// ============================================================================
// INI Parser
// ============================================================================

static std::string iniKey(const std::string& section, const std::string& key) {
    return section + "\x1F" + key;
}

static std::string iniSectionFromKey(const std::string& compound) {
    size_t pos = compound.find("\x1F");
    if (pos == std::string::npos) return "";
    return compound.substr(0, pos);
}

bool IniParser::load(const std::string& path) {
    _path = path;
    std::ifstream fin(path);
    if (!fin) return false;
    _lines.clear();
    _sections.clear();
    std::string currentSection;
    std::string line;
    while (std::getline(fin, line)) {
        line = StringHelper::trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;
        if (line[0] == '[') {
            auto end = line.find(']');
            if (end != std::string::npos) {
                currentSection = StringHelper::trim(line.substr(1, end - 1));
                _sections.push_back(currentSection);
            }
        } else {
            auto eq = line.find('=');
            if (eq != std::string::npos) {
                std::string k = StringHelper::trim(line.substr(0, eq));
                std::string v = StringHelper::trim(line.substr(eq + 1));
                _lines.emplace_back(iniKey(currentSection, k), v);
            }
        }
    }
    return true;
}

bool IniParser::save(const std::string& path) {
    std::ofstream fout(path);
    if (!fout) return false;
    std::string currentSection;
    for (size_t i = 0; i < _lines.size(); ++i) {
        std::string s = iniSectionFromKey(_lines[i].first);
        std::string k = _lines[i].first.substr(s.size() + 1);
        if (s != currentSection) {
            if (i > 0) fout << "\n";
            if (!s.empty()) fout << "[" << s << "]\n";
            currentSection = s;
        }
        fout << k << "=" << _lines[i].second << "\n";
    }
    return fout.good();
}

std::string IniParser::get(const std::string& section, const std::string& key,
                             const std::string& defaultVal) const {
    std::string compound = iniKey(section, key);
    for (const auto& [k, v] : _lines) {
        if (k == compound) return v;
    }
    return defaultVal;
}

int IniParser::getInt(const std::string& section, const std::string& key, int defaultVal) const {
    return StringHelper::toInt(get(section, key), defaultVal);
}

bool IniParser::getBool(const std::string& section, const std::string& key, bool defaultVal) const {
    auto val = get(section, "");
    if (val.empty()) return defaultVal;
    return val == "1" || StringHelper::equalsIgnoreCase(val, "true") ||
           StringHelper::equalsIgnoreCase(val, "yes");
}

void IniParser::set(const std::string& section, const std::string& key, const std::string& val) {
    std::string compound = iniKey(section, key);
    for (auto& [k, v] : _lines) {
        if (k == compound) { v = val; return; }
    }
    _lines.emplace_back(compound, val);
    if (!hasSection(section)) _sections.push_back(section);
}

void IniParser::set(const std::string& section, const std::string& key, int val) {
    set(section, key, std::to_string(val));
}

void IniParser::set(const std::string& section, const std::string& key, bool val) {
    set(section, key, val ? "1" : "0");
}

void IniParser::set(const std::string& section, const std::string& key, const QString& val) {
    set(section, key, val.toUtf8().constData());
}

QStringList IniParser::getStringList(const std::string& section, const std::string& key,
                                     const QStringList& defaultVal) const {
    std::string raw = get(section, key, "");
    if (raw.empty()) return defaultVal;
    QStringList out;
    for (const QString& s : QString::fromUtf8(raw.c_str()).split(',')) {
        QString trimmed = s.trimmed();
        if (!trimmed.isEmpty()) out.append(trimmed);
    }
    return out;
}

void IniParser::setStringList(const std::string& section, const std::string& key,
                              const QStringList& val) {
    set(section, key, val.join(",").toUtf8().constData());
}

bool IniParser::hasSection(const std::string& section) const {
    return std::find(_sections.begin(), _sections.end(), section) != _sections.end();
}

void IniParser::removeSection(const std::string& section) {
    std::string prefix = section + "\x1F";
    _lines.erase(
        std::remove_if(_lines.begin(), _lines.end(),
            [&](const auto& p) { return p.first.find(prefix) == 0; }),
        _lines.end());
    _sections.erase(std::remove(_sections.begin(), _sections.end(), section), _sections.end());
}


std::string getProcessId() {
    char buf[64];
#if defined(_WIN32) || defined(_WIN64)
    snprintf(buf, sizeof(buf), "%lu", GetCurrentProcessId());
#else
    snprintf(buf, sizeof(buf), "%d", getpid());
#endif
    return std::string(buf);
}

int getNumberOfProcessors() {
#if defined(_WIN32) || defined(_WIN64)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

// ============================================================================
// Environment
// ============================================================================
std::string getEnvironmentVariable(const std::string& name) {
    const char* val = getenv(name.c_str());
    return val ? std::string(val) : "";
}

bool setEnvironmentVariable(const std::string& name, const std::string& value) {
#if defined(_WIN32) || defined(_WIN64)
    return SetEnvironmentVariableA(name.c_str(), value.c_str()) != 0;
#else
    return setenv(name.c_str(), value.c_str(), 1) == 0;
#endif
}

std::vector<std::string> getEnvironmentVariables() {
    std::vector<std::string> result;
#if defined(_WIN32) || defined(_WIN64)
    char* env = GetEnvironmentStrings();
    if (env) {
        for (char* p = env; *p; ) {
            result.push_back(p);
            p += strlen(p) + 1;
        }
        FreeEnvironmentStrings(env);
    }
#else
    for (char** env = environ; *env; ++env) {
        result.push_back(*env);
    }
#endif
    return result;
}

// ============================================================================
// System info
// ============================================================================
std::string getSystemInfo() {
    std::ostringstream oss;
#if defined(_WIN32) || defined(_WIN64)
    oss << "Windows ";
    OSVERSIONINFOA vi = { sizeof(vi) };
    if (GetVersionExA(&vi))
        oss << vi.dwMajorVersion << "." << vi.dwMinorVersion;
#elif defined(__APPLE__)
    oss << "macOS";
#elif defined(__linux__)
    oss << "Linux";
#else
    oss << "Unknown";
#endif
    return oss.str();
}

std::string getCPUInfo() {
#if defined(__linux__)
    std::ifstream fin("/proc/cpuinfo");
    if (fin) {
        std::string line;
        while (std::getline(fin, line)) {
            if (line.find("model name") != std::string::npos) {
                auto pos = line.find(':');
                if (pos != std::string::npos) return StringHelper::trim(line.substr(pos + 1));
            }
        }
    }
#endif
    return "Unknown";
}

int64_t getTotalPhysicalMemory() {
#if defined(_WIN32) || defined(_WIN64)
    MEMORYSTATUSEX mse;
    mse.dwLength = sizeof(mse);
    if (GlobalMemoryStatusEx(&mse)) return mse.ullTotalPhys;
    return 0;
#elif defined(__linux__)
    struct sysinfo si;
    if (sysinfo(&si) == 0) return (int64_t)si.totalram * si.mem_unit;
#endif
    return 0;
}

// ============================================================================
// Hashing (simplified - for production would use proper crypto library)
// ============================================================================
std::string md5String(const std::string& input) {
    QByteArray data(input.c_str(), static_cast<int>(input.size()));
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);
    return hash.toHex().constData();
}

std::string sha256String(const std::string& input) {
    QByteArray data(input.c_str(), static_cast<int>(input.size()));
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hash.toHex().constData();
}

std::string fileMD5(const std::string& path) {
    auto content = FileHelper::readFile(path);
    if (!content) return "";
    return md5String(*content);
}

// ============================================================================
// URL helpers
// ============================================================================
bool isValidUrl(const std::string& url) {
    return StringHelper::startsWith(url, "http://") ||
           StringHelper::startsWith(url, "https://") ||
           StringHelper::startsWith(url, "file://");
}

std::string urlEncode(const std::string& s) {
    std::string result;
    result.reserve(s.size() * 3);
    for (unsigned char c : s) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else {
            char buf[8];
            snprintf(buf, sizeof(buf), "%%%02X", c);
            result += buf;
        }
    }
    return result;
}

std::string urlDecode(const std::string& s) {
    std::string result;
    result.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            unsigned int val;
            sscanf(s.c_str() + i + 1, "%02x", &val);
            result += static_cast<char>(val);
            i += 2;
        } else if (s[i] == '+') {
            result += ' ';
        } else {
            result += s[i];
        }
    }
    return result;
}

std::string getUrlHost(const std::string& url) {
    size_t start = url.find("://");
    if (start == std::string::npos) return "";
    start += 3;
    size_t end = url.find('/', start);
    if (end == std::string::npos) end = url.size();
    return url.substr(start, end - start);
}

// ============================================================================
// Clipboard
// ============================================================================
std::string getClipboardText() {
#if defined(_WIN32) || defined(_WIN64)
    if (!OpenClipboard(nullptr)) return "";
    HANDLE data = GetClipboardData(CF_TEXT);
    if (!data) { CloseClipboard(); return ""; }
    const char* text = static_cast<const char*>(GlobalLock(data));
    std::string result(text ? text : "");
    GlobalUnlock(data);
    CloseClipboard();
    return result;
#else
    // Use xclip or xsel or pbpaste
    FILE* f = popen("xclip -selection clipboard -o 2>/dev/null", "r");
    if (!f) f = popen("xsel --clipboard 2>/dev/null", "r");
    if (!f) f = popen("pbpaste 2>/dev/null", "r");
    if (!f) return "";
    std::string result;
    char buf[256];
    while (size_t n = fread(buf, 1, sizeof(buf) - 1, f)) {
        result.append(buf, n);
    }
    pclose(f);
    return result;
#endif
}

bool setClipboardText(const std::string& text) {
#if defined(_WIN32) || defined(_WIN64)
    if (!OpenClipboard(nullptr)) return false;
    EmptyClipboard();
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!mem) { CloseClipboard(); return false; }
    memcpy(GlobalLock(mem), text.data(), text.size() + 1);
    GlobalUnlock(mem);
    bool ok = SetClipboardData(CF_TEXT, mem) != 0;
    CloseClipboard();
    return ok;
#else
    FILE* f = popen("xclip -selection clipboard 2>/dev/null", "w");
    if (!f) f = popen("xsel --clipboard 2>/dev/null", "w");
    if (!f) f = popen("pbcopy 2>/dev/null", "w");
    if (!f) return false;
    fwrite(text.data(), 1, text.size(), f);
    pclose(f);
    return true;
#endif
}

bool hasClipboardText() {
    return !getClipboardText().empty();
}

// ============================================================================
// Misc
// ============================================================================
std::string toHexString(const void* data, size_t size) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    const unsigned char* p = static_cast<const unsigned char*>(data);
    for (size_t i = 0; i < size; ++i) {
        oss << std::setw(2) << static_cast<unsigned>(p[i]);
    }
    return oss.str();
}

std::string formatBytes(int64_t bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
    return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
}

int compareVersion(const std::string& a, const std::string& b) {
    auto aParts = StringHelper::split(a, ".");
    auto bParts = StringHelper::split(b, ".");
    size_t maxLen = std::max(aParts.size(), bParts.size());
    for (size_t i = 0; i < maxLen; ++i) {
        int aVal = i < aParts.size() ? StringHelper::toInt(aParts[i]) : 0;
        int bVal = i < bParts.size() ? StringHelper::toInt(bParts[i]) : 0;
        if (aVal < bVal) return -1;
        if (aVal > bVal) return 1;
    }
    return 0;
}

std::string generateRandomString(size_t length) {
    static const char charset[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i)
        result += charset[dist(gen)];
    return result;
}
