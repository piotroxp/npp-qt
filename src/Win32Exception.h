// npp-qt: Win32 SEH exception handler (stub)
//
// Win32 version uses __try/__except SEH to catch access violations.
// Qt6/Linux version: not applicable; use signal handlers instead.
#pragma once

#include <QString>

// Win32Exception stub class for Linux builds
// Win32 SEH exceptions don't exist on Linux, but we keep this for source compatibility
class Win32Exception
{
public:
    Win32Exception(unsigned long code = 0) : _code(code) {}
    unsigned long code() const { return _code; }
    const char* what() const { return "Win32Exception"; }
    void* info() const { return nullptr; }  // stub - no crash dump info on Linux

    // Static stub functions for SEH handling (no-op on Linux)
    static void installHandler() { /* no-op on Linux */ }
    static void removeHandler() { /* no-op on Linux */ }
    static const char* exceptionName(unsigned long) { return "Exception"; }
    static bool isAccessViolation(unsigned long) { return false; }
    static QString formatContext(void*) { return QString(); }

private:
    unsigned long _code = 0;
};
