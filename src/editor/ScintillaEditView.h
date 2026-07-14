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

// Scintilla message constants are inherited from NppSciCompat.h via ScintillaComponent.h.
// Duplicate definitions removed to avoid ODR conflicts.

class ScintillaEditView : public ScintillaComponent {
public:
    explicit ScintillaEditView();
    ~ScintillaEditView() override;

    // ---- ScintillaComponent interface ----
    intptr_t send(int message, int wParam = 0, intptr_t lParam = 0) override;

    // ---- Direct SCI wrapper ----

    /// Send an arbitrary SCI message with typed parameters
    intptr_t sendCommand(int message, uptr_t wParam = 0, sptr_t lParam = 0) const;

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
