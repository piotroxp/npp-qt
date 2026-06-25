// npp-qt: Stub definitions for dialogs not yet ported from Win32
// These classes are forward-declared in NppConstants.h but not yet implemented.
// Provides minimal stubs so the class Notepad_plus can compile.

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
};

// Minimal empty dialog for "Incremental Find" — placeholder
class FindIncrementDlg : public ScintillaComponent {
public:
    FindIncrementDlg() = default;
    virtual ~FindIncrementDlg() = default;
};

// Minimal stub for Hash (MD5/SHA) dialogs — placeholder
class HashFromFilesDlg : public ScintillaComponent {
public:
    HashFromFilesDlg() = default;
    virtual ~HashFromFilesDlg() = default;
};

class HashFromTextDlg : public ScintillaComponent {
public:
    HashFromTextDlg() = default;
    virtual ~HashFromTextDlg() = default;
};

// Minimal stub for Document Peeker (function list preview) — placeholder
class DocumentPeeker : public ScintillaComponent {
public:
    DocumentPeeker() = default;
    virtual ~DocumentPeeker() = default;
};

// Sci_CharacterRangeFull — defined in Scintilla.h (authoritative source)
// StubDialogs.h included by Notepad_plus.h before Scintilla.h,
// so Scintilla's definition wins. No duplicate needed here.
