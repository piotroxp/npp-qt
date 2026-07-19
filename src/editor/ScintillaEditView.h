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
#include <QVector>

// Scintilla message constants are inherited from NppSciCompat.h via ScintillaComponent.h.
// Duplicate definitions removed to avoid ODR conflicts.

// ============================================================================
// ScintillaEditView — Low-level SCI message wrapper
// ============================================================================
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

    // ---- Word / identifier navigation ----

    /// Move cursor one word left
    void wordLeft();

    /// Move cursor one word right
    void wordRight();

    /// Move cursor to end of word to the left
    void wordLeftEnd();

    /// Move cursor to end of word to the right
    void wordRightEnd();

    /// Select word at current position
    void selectWord();

    // ---- Line operations ----

    /// Move selected lines up by one position
    void moveLineUp();

    /// Move selected lines down by one position
    void moveLineDown();

    /// Duplicate the current line or selection below
    void duplicateLine();

    /// Delete the current line
    void deleteLine();

    /// Delete from cursor to end of line
    void deleteLineEnd();

    /// Join current line with the next one
    void lineJoin();

    /// Split current line at cursor position
    void lineCut();

    /// Transpose current line with previous line
    void lineTranspose();

    // ---- Indent / outdent ----

    /// Press Tab — indent current line or selection
    void tabKey();

    /// Press Shift+Tab — outdent current line or selection
    void backtabKey();

    // ---- Brace matching ----

    /// Highlight matching brace at current position
    void braceMatch();

    /// Highlight all matching braces in the visible range
    void braceHighlight();

    /// Find the matching brace position; returns true if found and sets endPos
    bool findMatchingBrace(int& endPos) const;

    // ---- Code folding ----

    /// Get the fold level of a line (0–2047)
    int getFoldLevel(int line) const;

    /// Toggle fold state at the given line
    void toggleFoldAt(int line);

    /// Collapse all folds in the document
    void collapseAllFolds();

    /// Unfold all folds in the document
    void unfoldAllFolds();

    /// Toggle all folds (collapse unfolded, unfold collapsed)
    void toggleAllFolds();

    /// Collapse folds at a specific level (1–foldDepth)
    void collapseLevel(int level);

    /// Unfold folds at a specific level
    void unfoldLevel(int level);

    /// Get the highest fold level present in the document
    int maxFoldLevel() const;

    // ---- Find / replace markers ----

    /// Add a find indicator mark at a position (for "mark all" style highlighting)
    /// indicator: which indicator to use (0–7)
    void addFindMark(int indicator, int line, int startCol, int endCol);

    /// Clear all find marks using the given indicator
    void clearAllFinds(int indicator);

    /// Add multiple find marks from a list of positions
    void addFindMarks(int indicator, const QVector<QPair<int, int>>& positions);

    // ---- Column mode / rectangular selection helpers ----

    /// Begin a column selection at (line, startCol) to (line, endCol)
    void columnEdit(int line, int startCol, int endCol);

    /// Set rectangular selection from startPos to endPos (character positions)
    void setColumnSelection(int startPos, int endPos);

    /// Insert text into every selected line in column mode (multi-caret)
    void columnInsert(const QString& text);

    /// Delete the selected column region
    void columnDelete();

    /// Check if the current selection is a column (rectangular) selection
    bool isColumnSelection() const;

    // ---- Virtual space ----

    /// Enable or disable virtual space (caret can move past line end)
    void setVirtualSpaceOptions(int mode);

    /// Get current virtual space option value
    int virtualSpaceOptions() const;

    // ---- Scrollbar ----

    /// Set the first visible line (scroll position)
    void setFirstVisibleLine(int line);

    /// Get the first visible line number
    int firstVisibleLine() const;

    /// Scroll the view by a number of lines (positive = down, negative = up)
    void scrollLines(int deltaLines);

    /// Scroll the view by a number of columns (positive = right)
    void scrollColumns(int deltaCols);

    // ---- Search / target ----

    /// Set the search target range [start, end)
    void setTarget(int start, int end);

    /// Search for text within the current target; returns position or -1
    int searchInTarget(const QString& text) const;

    /// Get the target range start position
    int targetStart() const;

    /// Get the target range end position
    int targetEnd() const;

    /// Get the length of the last found match
    int targetFoundLength() const;

    // ---- Marker management ----

    /// Add a marker (e.g. bookmark) at a line with the given marker number
    void addMarker(int line, int markerNum);

    /// Delete a marker at a line
    void deleteMarker(int line, int markerNum);

    /// Delete all markers of a given type
    void deleteAllMarkers(int markerNum);

    /// Check if a line has a specific marker
    bool hasMarker(int line, int markerNum) const;

    /// Get all lines that have a specific marker
    QVector<int> markersWithType(int markerNum) const;

    /// Define a marker with the given style and color
    void defineMarker(int markerNum, int style, const QColor& fore,
                      const QColor& back);

    // ---- Document information ----

    /// Get the current cursor position as a character offset from document start
    int currentPos() const;

    /// Get the anchor position of the main selection
    int anchorPosition() const;

    /// Set the selection range [anchor, caret)
    void setSelection(int anchor, int caret);

    /// Get the selection start position
    int selectionStart() const;

    /// Get the selection end position
    int selectionEnd() const;

    /// Clear all selections (set single caret at position)
    void clearSelections();

    /// Add another selection (for multi-caret editing)
    void addSelection(int anchor, int caret);

    /// Get the number of active selections
    int selectionCount() const;

    // ---- Direct access to the underlying QsciScintilla widget ----
    QsciScintilla* widget() const { return _widget; }
    QWidget* window() const { return _widget ? _widget->window() : nullptr; }

private:
    QsciScintilla* _widget = nullptr;
};
