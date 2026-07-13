// npp-qt: MiniDumper — Windows exception dumper (stub)
//
// Win32 version writes crash dumps via MiniDumpWriteDump API.
// Qt6/Linux version: stub. Production Linux builds use
// Qt's built-in crash handler or systemd-coredump.
#pragma once
#include <QString>

class MiniDumper {
public:
    // Install global exception handler (Win32 only)
    static void install(const QString& dumpPath = QString());

    // Uninstall (no-op on non-Windows)
    static void uninstall();

    // Check if dumps are enabled
    static bool isEnabled();

    // Write crash dump (stub on Linux; Win32 uses MiniDumpWriteDump)
    void writeDump(void* /* exInfo */) { /* no-op on Linux */ }
};
