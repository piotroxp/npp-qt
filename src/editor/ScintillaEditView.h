// ScintillaEditView.h — Low-level Scintilla edit view wrapper
// Provides a thin C++ wrapper around Scintilla's direct SCI_* messaging API.
// This class is the foundation layer; ScintillaEditor builds on top of it.
// Copyright (C) 2026 Agent Army / GPL v3

#pragma once

#include "../common/ScintillaComponent.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciscintillabase.h>
#include <QString>
#include <QByteArray>
#include <QFont>
#include <QColor>

// Scintilla message constants (mirrored from Scintilla.h / SciDefines.h)
constexpr int SCI_GETTEXT            = 2001;
constexpr int SCI_SETTEXT            = 2002;
constexpr int SCI_GETLINE            = 2003;
constexpr int SCI_GETLINECOUNT       = 2004;
constexpr int SCI_GETSELECTIONSTART  = 2411;
constexpr int SCI_GETSELECTIONEND    = 2412;
constexpr int SCI_SETSELECTIONSTART  = 2413;
constexpr int SCI_SETSELECTIONEND    = 2414;
constexpr int SCI_REPLACESEL         = 2170;
constexpr int SCI_GETCURLINE         = 2012;
constexpr int SCI_GETTEXTLENGTH      = 2006;
constexpr int SCI_CONVERTEOLS        = 2030;
constexpr int SCI_LINESJOIN          = 2288;
constexpr int SCI_LINESSPLIT         = 2289;
constexpr int SCI_LOWERCASE          = 2342;
constexpr int SCI_UPPERCASE          = 2343;
constexpr int SCI_TITLECASE           = 2344;
constexpr int SCI_SORTLINES           = 2169;
constexpr int SCI_SETSELECTIONMODE    = 2579;
constexpr int SCI_GETSELECTIONMODE    = 2580;
constexpr int SC_SEL_STREAM           = 0;
constexpr int SC_SEL_RECTANGLE        = 1;
constexpr int SC_SEL_LINES            = 2;
constexpr int SC_SORT_NUMERIC         = 1;
constexpr int SC_SORT_CASEINSENSITIVE = 2;
constexpr int SC_SORT_PERCENTAGE       = 3;
constexpr int SC_SORT_REVERSE          = 0x100;

class ScintillaEditView : public ScintillaComponent {
public:
    explicit ScintillaEditView();
    ~ScintillaEditView() override;

    // ---- ScintillaComponent interface ----
    intptr_t send(int message, int wParam = 0, intptr_t lParam = 0) override;

    // ---- Direct SCI wrapper ----

    /// Send an arbitrary SCI message with typed parameters
    intptr_t sendCommand(int message, uptr_t wParam = 0, sptr_t lParam = 0);

    /// Get the entire document text as a QString
    QString getText() const;

    /// Replace all document text with the given string
    void setText(const QString& text);

    /// Get a single line (0-indexed) as a QString (excludes trailing \n)
    QString getLine(int line) const;

    /// Get raw bytes for a range [start, end)
    QByteArray getTextRange(int start, int end) const;

    /// Replace the current selection with the given text
    void replaceSel(const QString& text);

    /// Total line count in the document
    int lineCount() const;

    /// Document text length in bytes (excludes NUL terminator)
    int textLength() const;

    /// Convert EOL style of the document
    void convertEol(int eolMode);

    // ---- Text transformations ----

    /// Convert case: SCI_LOWERCASE | SCI_UPPERCASE | SCI_TITLECASE
    void convertCase(int operation);

    /// Join selected lines into a single line
    void joinLines();

    /// Sort selected lines (use SC_SORT_* flags)
    void sortLines(int mode);

    // ---- Selection ----

    /// Set rectangular selection mode
    void setRectangularSelection(bool on);

    /// Get current selection mode: SC_SEL_STREAM | SC_SEL_RECTANGLE | SC_SEL_LINES
    int selectionMode() const;

    /// Check if rectangular selection is active
    bool isRectangularSelection() const;

    // ---- Direct access to the underlying QsciScintilla widget ----
    QsciScintilla* widget() const { return _widget; }

private:
    QsciScintilla* _widget = nullptr;
};
