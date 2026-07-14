// SmartHighlighter.h - Incremental search "highlight all matches" feature
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QObject>
#include <QColor>
#include <QVector>
#include <QString>

class ScintillaEditor;

// Scintilla indicator number used for smart highlighting
#define SMART_HILITE_INDICATOR 31

class SmartHighlighter : public QObject {
    Q_OBJECT

public:
    explicit SmartHighlighter(ScintillaEditor* editor, QObject* parent = nullptr);
    ~SmartHighlighter() override;

    /// Highlight all occurrences of `text` in the editor.
    /// If `caseSensitive` is false, performs a case-insensitive search.
    /// Emits matchCountChanged(int) when done.
    void highlight(const QString& text, bool caseSensitive);

    /// Clear all highlighting.
    void clear();

    /// Return the number of matches found in the last highlight() call.
    int getMatchCount() const { return _matchCount; }

    /// Set the highlight color (default: yellow-ish).
    void setHighlightColor(const QColor& color);

    /// Navigate to the next match from the current cursor position.
    void goToNextMatch();

    /// Navigate to the previous match from the current cursor position.
    void goToPreviousMatch();

signals:
    void matchCountChanged(int count);
    void currentMatchChanged(int index, int total);

private:
    void configureIndicator();
    void clearIndicators();
    int findAllMatches(const QString& text, bool caseSensitive);

    ScintillaEditor* _editor = nullptr;
    QColor _highlightColor;
    int    _indicatorNum = SMART_HILITE_INDICATOR;
    int    _matchCount = 0;
    int    _currentMatchIndex = -1;

    // Stored match positions (start, end) for navigation
    QVector<QPair<int, int>> _matchPositions;
};
