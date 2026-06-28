// npp-qt: Stub definitions for dialogs not yet ported from Win32
// These classes are forward-declared in NppConstants.h but not yet implemented.
// Provides minimal stubs so the class Notepad_plus can compile.

// md5Dlgs.h (included via Notepad_plus.h) provides the real HashFromFilesDlg and HashFromTextDlg
// These are forward-declared here to avoid "incomplete type" errors in StubDialogs.h

#pragma once
#include "ScintillaComponent.h"

// Minimal empty dialog for "Find in Finder" — placeholder until full port
class FindInFinderDlg : public ScintillaComponent {
public:
    FindInFinderDlg() = default;
    virtual ~FindInFinderDlg() = default;
    void doDialog(void* launcher, bool isRTL = false) {}
    void initFromOptions() {}
    void writeOptions() {}
    void beginNewFilesSearch() {}
    void addSearchLine(const char*) {}
    void finishFilesSearch(int nbTotal, int filesCount, bool isEntire, FindersInfo::FindOption* = nullptr) {}
    std::vector<std::wstring> getResultFilePaths(bool) { return {}; }
};

// Minimal empty dialog for "Incremental Find" — placeholder
class FindIncrementDlg : public ScintillaComponent {
public:
    FindIncrementDlg() = default;
    virtual ~FindIncrementDlg() = default;
};

// HashFromFilesDlg and HashFromTextDlg are defined in MISC/md5/md5Dlgs.h
// (inherits from StaticDialog with full run_dlgProc implementation).
// We just forward-declare them here to avoid "incomplete type" errors.
class HashFromFilesDlg;
class HashFromTextDlg;

// Minimal stub for Document Peeker (function list preview) — placeholder
class DocumentPeeker : public ScintillaComponent {
public:
    DocumentPeeker() = default;
    virtual ~DocumentPeeker() = default;
};
