// Util.cpp - General utility functions implementation
#include "Util.h"
#include "Constants.h"
#include "StringHelper.h"
#include "FileHelper.h"
#include <QString>
#include <QFile>
#include <QProcess>
#include <QGuiApplication>
#include <QClipboard>
#include <QSysInfo>
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
#include <sys/sysinfo.h>
#include <pwd.h>

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
    __builtin_trap();
}

void debugOutput(const std::string& msg) {
    fputs(msg.c_str(), stderr);
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
    std::string val = get(section, key, "");
    if (val.empty()) return defaultVal;

    if (val == "1" || val == "0") {
        return val == "1";
    }

    QString qval = QString::fromUtf8(val.c_str()).toLower();
    if (qval == "true" || qval == "yes" || qval == "on" || qval == "enabled") {
        return true;
    }
    if (qval == "false" || qval == "no" || qval == "off" || qval == "disabled") {
        return false;
    }

    return defaultVal;
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
    set(section, key, static_cast<std::string>(val ? "1" : "0"));
}

void IniParser::set(const std::string& section, const std::string& key, const QString& val) {
    set(section, key, std::string(val.toUtf8().constData()));
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
    snprintf(buf, sizeof(buf), "%d", getpid());
    return std::string(buf);
}

int getNumberOfProcessors() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

// ============================================================================
// Environment
// ============================================================================
std::string getEnvironmentVariable(const std::string& name) {
    const char* val = getenv(name.c_str());
    return val ? std::string(val) : "";
}

bool setEnvironmentVariable(const std::string& name, const std::string& value) {
    return setenv(name.c_str(), value.c_str(), 1) == 0;
}

std::vector<std::string> getEnvironmentVariables() {
    std::vector<std::string> result;
    for (char** env = environ; *env; ++env) {
        result.push_back(*env);
    }
    return result;
}

// ============================================================================
// System info
// ============================================================================
std::string getSystemInfo() {
    std::ostringstream oss;
#if defined(__APPLE__)
    oss << "macOS ";
    oss << QSysInfo::productVersion().toStdString();
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
    struct sysinfo si;
    if (sysinfo(&si) == 0) return (int64_t)si.totalram * si.mem_unit;
    return 0;
}

// ============================================================================
// Hashing
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
    return StringHelper::startsWith(std::string_view(url), "http://") ||
           StringHelper::startsWith(std::string_view(url), "https://") ||
           StringHelper::startsWith(std::string_view(url), "file://");
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
    QClipboard* clipboard = QGuiApplication::clipboard();
    if (!clipboard) return "";
    return clipboard->text().toStdString();
}

bool setClipboardText(const std::string& text) {
    QClipboard* clipboard = QGuiApplication::clipboard();
    if (!clipboard) return false;
    clipboard->setText(QString::fromStdString(text));
    return true;
}

bool hasClipboardText() {
    QClipboard* clipboard = QGuiApplication::clipboard();
    if (!clipboard) return false;
    return !clipboard->text().isEmpty();
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

// ============================================================================
// Duration formatting
// ============================================================================
QString formatDuration(int64_t ms) {
    if (ms < 0) return QString("0s");

    int64_t seconds = ms / 1000;
    int64_t minutes = seconds / 60;
    int64_t hours = minutes / 60;
    int64_t days = hours / 24;

    if (days > 0) {
        return QString("%1d %2h").arg(days).arg(hours % 24);
    }
    if (hours > 0) {
        return QString("%1h %2m").arg(hours).arg(minutes % 60);
    }
    if (minutes > 0) {
        return QString("%1m %2s").arg(minutes).arg(seconds % 60);
    }
    return QString("%1s").arg(seconds);
}

// ============================================================================
// System utilities
// ============================================================================
int64_t getSystemUptime() {
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        return static_cast<int64_t>(si.uptime);
    }
    return -1;
}

QString getDesktopEnvironment() {
    const char* xdgDe = getenv("XDG_CURRENT_DESKTOP");
    if (xdgDe) {
        QString de = QString(xdgDe).toLower();
        if (de.contains("kde")) return "KDE";
        if (de.contains("gnome")) return "GNOME";
        if (de.contains("xfce")) return "XFCE";
        if (de.contains("lxde")) return "LXDE";
        if (de.contains("lxqt")) return "LXQt";
        if (de.contains("mate")) return "MATE";
        if (de.contains("cinnamon")) return "Cinnamon";
        if (de.contains("unity")) return "Unity";
        if (de.contains("budgie")) return "Budgie";
        if (de.contains("deepin")) return "Deepin";
        if (de.contains("pantheon")) return "Pantheon";
        return QString(xdgDe);
    }

    const char* session = getenv("DESKTOP_SESSION");
    if (session) {
        QString de = QString(session).toLower();
        if (de.contains("kde") || de.contains("plasma")) return "KDE";
        if (de.contains("gnome")) return "GNOME";
        if (de.contains("xfce")) return "XFCE";
        if (de.contains("lxde")) return "LXDE";
        if (de.contains("lxqt")) return "LXQt";
        if (de.contains("mate")) return "MATE";
        if (de.contains("cinnamon")) return "Cinnamon";
        return QString(session);
    }

    if (getenv("KDE_FULL_SESSION") || getenv("KDE_SESSION_VERSION")) return "KDE";
    if (getenv("GNOME_DESKTOP_SESSION_ID")) return "GNOME";
    if (getenv("XFCE4")) return "XFCE";
    return "Unknown";
}

// ============================================================================
// Application launching
// ============================================================================
bool launchApplication(const QString& app, const QStringList& args, QString& error) {
    QProcess* process = new QProcess();
    QStringList allArgs = args;
    process->setProgram(app);
    process->setArguments(allArgs);
    process->start();
    if (!process->waitForStarted(5000)) {
        error = QString("Failed to start process: %1").arg(process->errorString());
        delete process;
        return false;
    }
    process->disconnect();
    delete process;
    return true;
}

// ============================================================================
// File comparison
// ============================================================================
bool compareFiles(const QString& a, const QString& b) {
    QFile fa(a);
    QFile fb(b);
    if (!fa.open(QIODevice::ReadOnly) || !fb.open(QIODevice::ReadOnly)) {
        return false;
    }
    if (fa.size() != fb.size()) return false;

    const int chunkSize = 65536;
    QByteArray ca, cb;
    while (!fa.atEnd() && !fb.atEnd()) {
        ca = fa.read(chunkSize);
        cb = fb.read(chunkSize);
        if (ca != cb) return false;
    }
    return true;
}

// ============================================================================
// File name validation
// ============================================================================
bool isValidFileName(const QString& name) {
    if (name.isEmpty()) return false;
    static const QString invalidChars = "/\\:*?\"<>|";
    for (const QChar& c : invalidChars) {
        if (name.contains(c)) return false;
    }
    static const QStringList reserved = {"CON", "PRN", "AUX", "NUL",
        "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
        "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};
    QString upperName = name.toUpper();
    if (reserved.contains(upperName)) return false;
    if (name.endsWith('.') || name.endsWith(' ')) return false;
    if (name.length() > 255) return false;
    return true;
}

// ============================================================================
// EOL normalization
// ============================================================================
QString normalizeEOL(const QString& text, EolType eol) {
    QString result = text;
    QString eolStr;
    switch (eol) {
        case EolType::EOL_CRLF: eolStr = "\r\n"; break;
        case EolType::EOL_LF: eolStr = "\n"; break;
        case EolType::EOL_CR: eolStr = "\r"; break;
        default: eolStr = "\n"; break;
    }

    result.replace("\r\n", "\n");
    result.replace("\r", "\n");
    result.replace("\n", eolStr);
    return result;
}
