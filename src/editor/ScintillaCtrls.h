// ScintillaCtrls.h - Scintilla control helpers / utility functions
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QString>
#include <Qsci/qsciscintilla.h>
#include <QColor>
#include <QPoint>
#include <QRect>

class ScintillaEditor;
class QsciScintilla;

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

    // =========================================================================
    // Multiple view support (split window)
    // =========================================================================

    /// Synchronize scrolling between two Scintilla editors
    static void syncScrolling(QsciScintilla* primary, QsciScintilla* secondary);

    /// Disable synchronized scrolling
    static void unsyncScrolling(QsciScintilla* primary);

    /// Get the vertical scrollbar position of an editor (as fraction 0.0–1.0)
    static double scrollPosition(QsciScintilla* sci);

    /// Set the vertical scrollbar position of an editor (as fraction 0.0–1.0)
    static void setScrollPosition(QsciScintilla* sci, double fraction);

    /// Get the horizontal scrollbar position of an editor (as fraction 0.0–1.0)
    static double hScrollPosition(QsciScintilla* sci);

    /// Set the horizontal scrollbar position of an editor
    static void setHScrollPosition(QsciScintilla* sci, double fraction);

    // =========================================================================
    // Indicators
    // =========================================================================

    /// Indicator constants compatible with SciTE/Notepad++
    enum StandardIndicator {
        INDI_STRIKE      = 8,   ///< Strikethrough indicator
        INDI_SPELLCHECK  = 9,   ///< Spell check squiggle indicator
        INDI_SEARCH      = 0,   ///< Current search match highlight
        INDI_MARKALL     = 1,   ///< "Mark all" search highlights
    };

    /// Define a standard indicator with style and color
    static void defineIndicator(QsciScintilla* sci, int indicator,
                                QsciScintilla::IndicatorStyle style,
                                const QColor& color);

    /// Apply strikethrough indicator to a range
    static void applyStrikeIndicator(QsciScintilla* sci, int line,
                                     int startCol, int endCol);

    /// Clear strikethrough indicator in a range
    static void clearStrikeIndicator(QsciScintilla* sci, int line,
                                     int startCol, int endCol);

    // =========================================================================
    // Annotations
    // =========================================================================

    /// Add a style annotation to a line (shown in the annotation margin)
    static void addAnnotation(QsciScintilla* sci, int line,
                              const QString& text, int style = 0);

    /// Clear all annotations in the document
    static void clearAnnotations(QsciScintilla* sci);

    /// Clear annotations on a specific line
    static void clearAnnotation(QsciScintilla* sci, int line);

    // =========================================================================
    // Margin click handling
    // =========================================================================

    /// Constants for standard margins
    enum StandardMargin {
        MarginLineNumbers = 0,   ///< Line number margin
        MarginSymbols    = 1,   ///< Bookmark/symbol margin
        MarginFolding   = 2,   ///< Code folding margin
        MarginText       = 3,   ///< Text margin / annotations
    };

    /// Set the click handler for a margin; uses event filter on the widget
    /// The handler is called with (margin, line, modifier) on click.
    /// Ownership of handler remains with caller.
    using MarginClickHandler = void (*)(int margin, int line, int state, void* ctx);
    static void setMarginClickHandler(QsciScintilla* sci, int margin,
                                      MarginClickHandler handler, void* ctx);

    /// Set whether a margin is sensitive to clicks
    static void setMarginSensitive(QsciScintilla* sci, int margin, bool sensitive);

    /// Set the width of a margin (0 hides it)
    static void setMarginWidth(QsciScintilla* sci, int margin, int widthChars);

    // =========================================================================
    // Context menu customization
    // =========================================================================

    /// Build a standard editor context menu
    static QMenu* buildContextMenu(ScintillaEditor* editor, QWidget* parent);

    /// Add standard "Wrap with..." submenu items to an existing menu
    static void addWrapActionsToMenu(QMenu* menu, ScintillaEditor* editor);

    /// Add standard indentation actions to a menu
    static void addIndentActionsToMenu(QMenu* menu, ScintillaEditor* editor);

    // =========================================================================
    // Call tips (lexer-specific parameter hints)
    // =========================================================================

    /// Show a call tip popup near the cursor
    static void showCallTip(QsciScintilla* sci, const QString& tip,
                            const QString& defn = QString());

    /// Cancel any active call tip
    static void cancelCallTip(QsciScintilla* sci);

    /// Register an argument list for a function (for call tip navigation)
    static void registerCallTipArgs(QsciScintilla* sci, const QString& functionName,
                                    int argc, const QStringList& argNames,
                                    const QString& doc = QString());

    // =========================================================================
    // Drag and drop text
    // =========================================================================

    /// Enable internal drag-and-drop of text within the editor
    static void setTextDragEnabled(QsciScintilla* sci, bool enabled);

    /// Get the drag-and-drop state
    static bool isTextDragEnabled(QsciScintilla* sci);

    // =========================================================================
    // Column / rectangular selection helpers
    // =========================================================================

    /// Check whether the current selection is rectangular
    static bool isRectangularSelection(QsciScintilla* sci);

    /// Begin a column selection from (startLine, startCol) to (endLine, endCol)
    static void startColumnSelection(QsciScintilla* sci,
                                    int startLine, int startCol,
                                    int endLine, int endCol);

    /// Get the bounding rectangle of the current column selection
    static QRect columnSelectionBounds(QsciScintilla* sci);

    // =========================================================================
    // Word/identifier operations
    // =========================================================================

    /// Select the word at the current cursor position
    static void selectWordAtCursor(QsciScintilla* sci);

    /// Get the word boundaries at the current cursor (start, end positions)
    static QPair<int, int> wordAtCursor(QsciScintilla* sci);

    /// Check if the cursor is inside a comment block
    static bool isInComment(QsciScintilla* sci);

    /// Check if the cursor is inside a string literal
    static bool isInString(QsciScintilla* sci);

private:
    // Internal helpers
    static void _moveBlockUp(QsciScintilla* qsci, int startLine, int endLine);
    static void _moveBlockDown(QsciScintilla* qsci, int startLine, int endLine);

    // Scroll sync data per primary editor
    static void _syncScrollCallback(int, int, int, int, void*);
    static QsciScintilla* _syncTarget(QsciScintilla* primary);
};
