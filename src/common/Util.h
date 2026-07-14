// Util.h - General utility functions for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include "Constants.h"
#include "Types.h"
#include <QString>
#include <QStringList>
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <optional>
#include <variant>

// ============================================================================
// Application startup / runtime info
// ============================================================================
struct AppInfo {
    std::string name    = APP_NAME;
    std::string version = APP_VERSION;
    std::string title   = APP_Title;
    std::string year    = APP_YEAR;
    std::string configDir;
    std::string pluginDir;
    std::string themeDir;
};

// ============================================================================
// Timing / profiling
// ============================================================================
class ScopedTimer {
public:
    using Clock = std::chrono::steady_clock;
    explicit ScopedTimer(const std::string& name = "block") : _name(name) { _start = Clock::now(); }
    ~ScopedTimer() {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - _start).count();
        printf("[TIMER] %s: %lld ms\n", _name.c_str(), (long long)elapsed);
    }
private:
    std::string _name;
    Clock::time_point _start;
};

// ============================================================================
// Process info
// ============================================================================
std::string getProcessId();
int getNumberOfProcessors();

// ============================================================================
// Environment
// ============================================================================
std::string getEnvironmentVariable(const std::string& name);
bool setEnvironmentVariable(const std::string& name, const std::string& value);
std::vector<std::string> getEnvironmentVariables();

// ============================================================================
// INI / Config file parsing
// ============================================================================
class IniParser {
public:
    bool load(const std::string& path);
    bool save(const std::string& path);
    std::string get(const std::string& section, const std::string& key,
                    const std::string& defaultVal = "") const;
    int getInt(const std::string& section, const std::string& key, int defaultVal = 0) const;
    bool getBool(const std::string& section, const std::string& key, bool defaultVal = false) const;
    void set(const std::string& section, const std::string& key, const std::string& val);
    void set(const std::string& section, const std::string& key, int val);
    void set(const std::string& section, const std::string& key, bool val);
    // QString overload — converts to UTF-8
    void set(const std::string& section, const std::string& key, const QString& val);
    // String list — stored as comma-separated values
    QStringList getStringList(const std::string& section, const std::string& key,
                              const QStringList& defaultVal = {}) const;
    void setStringList(const std::string& section, const std::string& key,
                       const QStringList& val);
    bool hasSection(const std::string& section) const;
    void removeSection(const std::string& section);
    const std::vector<std::string>& sections() const { return _sections; }

private:
    int findKey(const std::string& section, const std::string& key) const;

private:
    std::vector<std::string> _sections;
    // Each entry: (section, key) -> value  [no separator needed]
    std::vector<std::pair<std::string, std::string>> _lines;
    std::string _path;
};

std::string getSystemInfo();
std::string getCPUInfo();
int64_t getTotalPhysicalMemory();

// ============================================================================
// Hashing (MD5, SHA-256)
// ============================================================================
std::string md5String(const std::string& input);
std::string sha256String(const std::string& input);
std::string fileMD5(const std::string& path);

// ============================================================================
// URL / URI helpers
// ============================================================================
bool isValidUrl(const std::string& url);
std::string urlEncode(const std::string& s);
std::string urlDecode(const std::string& s);
std::string getUrlHost(const std::string& url);

// ============================================================================
// Clipboard
// ============================================================================
std::string getClipboardText();
bool setClipboardText(const std::string& text);
bool hasClipboardText();

// ============================================================================
// Logging
// ============================================================================
enum class LogLevel { Debug, Info, Warning, Error, Critical };
void logMessage(LogLevel level, const std::string& msg);
void logDebug(const std::string& msg);
void logInfo(const std::string& msg);
void logWarning(const std::string& msg);
void logError(const std::string& msg);
std::string getLogFilePath();
void setLogLevel(LogLevel level);
void setLogFile(const std::string& path);

// ============================================================================
// Assertion / Error handling
// ============================================================================
void assertionFailed(const char* expr, const char* file, int line);
void debugOutput(const std::string& msg);

#if defined(_DEBUG) || defined(DEBUG)
#define NPP_ASSERT(expr) ((expr) ? (void)0 : assertionFailed(#expr, __FILE__, __LINE__))
#else
#define NPP_ASSERT(expr) ((void)0)
#endif

// ============================================================================
// Conversion utilities
// ============================================================================
std::string toHexString(const void* data, size_t size);
std::string formatBytes(int64_t bytes);

// ============================================================================
// Thread-safe counter
// ============================================================================
template<typename T>
class AtomicCounter {
public:
    AtomicCounter(T initial = 0) : _value(initial) {}
    T increment() { return ++_value; }
    T decrement() { return --_value; }
    T get() const { return _value; }
    void set(T v) { _value = v; }
private:
    T _value;
};

// ============================================================================
// Circular buffer
// ============================================================================
template<typename T, size_t N>
class CircularBuffer {
public:
    bool push(const T& item) {
        _data[_end] = item;
        _end = (_end + 1) % N;
        if (_count == N) _start = (_start + 1) % N;
        else ++_count;
        return true;
    }
    bool pop(T& item) {
        if (_count == 0) return false;
        item = _data[_start];
        _start = (_start + 1) % N;
        --_count;
        return true;
    }
    void clear() { _count = 0; _start = 0; _end = 0; }
    size_t size() const { return _count; }
    bool empty() const { return _count == 0; }
    bool full() const { return _count == N; }

private:
    T _data[N];
    size_t _start = 0;
    size_t _end = 0;
    size_t _count = 0;
};

// ============================================================================
// RAII lock guard
// ============================================================================
template<typename Mutex>
class LockGuard {
public:
    explicit LockGuard(Mutex& m) : _mutex(m) { _mutex.lock(); }
    ~LockGuard() { _mutex.unlock(); }
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
private:
    Mutex& _mutex;
};

// ============================================================================
// Non-copyable base
// ============================================================================
class NonCopyable {
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

// ============================================================================
// Observer pattern
// ============================================================================
template<typename... Args>
class Observer {
public:
    using Callback = std::function<void(Args...)>;
    size_t addCallback(Callback cb) {
        size_t id = _nextId++;
        _callbacks[id] = std::move(cb);
        return id;
    }
    void removeCallback(size_t id) { _callbacks.erase(id); }
    void notify(Args... args) {
        for (auto& [id, cb] : _callbacks) cb(args...);
    }
private:
    size_t _nextId = 0;
    std::unordered_map<size_t, Callback> _callbacks;
};

// ============================================================================
// Version comparison
// ============================================================================
int compareVersion(const std::string& a, const std::string& b);

// ============================================================================
// Random
// ============================================================================
std::string generateRandomString(size_t length);

// ============================================================================
// Duration formatting
// ============================================================================
QString formatDuration(int64_t milliseconds);

// ============================================================================
// System utilities
// ============================================================================
int64_t getSystemUptime();  // seconds since boot
QString getDesktopEnvironment();  // "KDE", "GNOME", "XFCE", etc.

// ============================================================================
// Application launching
// ============================================================================
bool launchApplication(const QString& app, const QStringList& args, QString& error);

// ============================================================================
// File comparison
// ============================================================================
bool compareFiles(const QString& a, const QString& b);  // content equality

// ============================================================================
// File name validation
// ============================================================================
bool isValidFileName(const QString& name);

// ============================================================================
// EOL normalization
// ============================================================================
QString normalizeEOL(const QString& text, EolType eol);
