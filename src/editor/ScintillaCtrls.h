// ScintillaCtrls.h - Scintilla control helpers / utility functions
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QString>
#include <Qsci/qsciscintilla.h>

class ScintillaEditor;

// ============================================================================
// ScintillaCtrls - static utility class for common Scintilla operations
// ============================================================================
class ScintillaCtrls {
public:
    // ------------------------------------------------------------------------
    // Wrap selected text with before/after strings (e.g. "/*", "*/" or "(",
    // ")")
    // ------------------------------------------------------------------------
    static void wrapSelection(ScintillaEditor* editor,
                               const QString& before,
                               const QString& after);

    // ------------------------------------------------------------------------
    // Indent (or dedent) selected block by N spaces
    // ------------------------------------------------------------------------
    static void indentBlock(ScintillaEditor* editor, int spaces);

    // ------------------------------------------------------------------------
    // Duplicate the current line (or selection) below
    // ------------------------------------------------------------------------
    static void duplicateLine(ScintillaEditor* editor);

    // ------------------------------------------------------------------------
    // Join selected lines into a single line (removes line breaks between
    // selections)
    // ------------------------------------------------------------------------
    static void joinLines(ScintillaEditor* editor);

    // ------------------------------------------------------------------------
    // Move selected lines up (up=true) or down (up=false) by one line
    // ------------------------------------------------------------------------
    static void moveLines(ScintillaEditor* editor, bool up);

    // ------------------------------------------------------------------------
    // Delete an entire line by line number
    // ------------------------------------------------------------------------
    static void deleteLine(ScintillaEditor* editor, int line);

    // ------------------------------------------------------------------------
    // Swap case of selected text: upper, lower, or title case
    // ------------------------------------------------------------------------
    enum class CaseMode { Upper, Lower, Title };
    static void changeCase(ScintillaEditor* editor, CaseMode mode);

    // ------------------------------------------------------------------------
    // Run command on Scintilla directly (sci_* messages)
    // ------------------------------------------------------------------------
    static intptr_t send(ScintillaEditor* editor, int msg, int wParam = 0,
                         intptr_t lParam = 0);

private:
    // Internal helpers
    static void _moveBlockUp(QsciScintilla* qsci, int startLine, int endLine);
    static void _moveBlockDown(QsciScintilla* qsci, int startLine, int endLine);
};
