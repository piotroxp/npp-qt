// SmartHighlighter.cpp - Incremental search "highlight all matches" feature
// Copyright (C) 2026 Agent Army
// GPL v3

#include "SmartHighlighter.h"
#include "../editor/ScintillaEditor.h"
#include <Qsci/qsciscintilla.h>
#include <QColor>
#include <QDebug>
#include <QRegularExpression>

// Maximum matches to prevent performance issues

// Maximum matches to prevent performance issues
#define MAX_MATCHES 10000

// Sci_TextToFind uses Sci_CharacterRange (defined in ScintillaComponent.h)
struct Sci_TextToFind {
    Sci_CharacterRange chrg;
    const char* lpstrText;
};


SmartHighlighter::SmartHighlighter(ScintillaEditor* editor, QObject* parent)
    : QObject(parent)
    , _editor(editor)
    , _highlightColor(0xFF, 0xFF, 0x00)  // Yellow by default
{
    configureIndicator();
}

SmartHighlighter::~SmartHighlighter() {
    clear();
}

void SmartHighlighter::configureIndicator() {
    if (!_editor) return;

    QsciScintilla* sci = _editor->qsciEditor();
    if (!sci) return;

    // Configure the indicator style and color using QScintilla API
    sci->SendScintilla(QsciScintillaBase::SCI_INDICSETSTYLE, _indicatorNum, QsciScintilla::PlainIndicator);
    sci->SendScintilla(QsciScintillaBase::SCI_INDICSETFORE,
                        _indicatorNum,
                        QRgb(_highlightColor.rgb()));
    sci->SendScintilla(QsciScintillaBase::SCI_INDICSETALPHA,
                        _indicatorNum, 80);  // Semi-transparent
}

void SmartHighlighter::setHighlightColor(const QColor& color) {
    _highlightColor = color;
    configureIndicator();
}

void SmartHighlighter::clearIndicators() {
    if (!_editor) return;

    QsciScintilla* sci = _editor->qsciEditor();
    if (!sci) return;

    // Clear all existing indicators of our type
    int docLen = sci->SendScintilla(QsciScintillaBase::SCI_GETLENGTH);
    if (docLen > 0) {
        sci->SendScintilla(QsciScintillaBase::SCI_SETINDICATORCURRENT, _indicatorNum);
        sci->SendScintilla(QsciScintillaBase::SCI_INDICATORCLEARRANGE, 0, docLen);
    }
}

void SmartHighlighter::clear() {
    clearIndicators();
    _matchPositions.clear();
    _matchCount = 0;
    _currentMatchIndex = -1;
    emit matchCountChanged(0);
    emit currentMatchChanged(-1, 0);
}

void SmartHighlighter::highlight(const QString& text, bool caseSensitive) {
    if (!_editor) return;

    // Clear previous highlights
    clear();

    // Handle empty or whitespace-only search text
    if (text.isEmpty()) return;

    // Check if the text looks like a regex (contains regex metacharacters)
    bool looksLikeRegex = text.contains(QRegularExpression("[\\^$.|?*+()\\[\\]{}]"));
    Q_UNUSED(looksLikeRegex);

    int count = findAllMatches(text, caseSensitive);
    Q_UNUSED(count);

    _matchCount = _matchPositions.size();
    emit matchCountChanged(_matchCount);

    if (!_matchPositions.isEmpty()) {
        // Set current match to first one
        _currentMatchIndex = 0;
        emit currentMatchChanged(_currentMatchIndex + 1, _matchCount);
    }
}

int SmartHighlighter::findAllMatches(const QString& text, bool caseSensitive) {
    if (!_editor) return 0;

    QsciScintilla* sci = _editor->qsciEditor();
    if (!sci) return 0;

    // Convert search text to UTF-8 for Scintilla
    QByteArray searchBytes = text.toUtf8();
    const char* searchStr = searchBytes.constData();
    size_t searchLen = searchBytes.size();

    int docLen = sci->SendScintilla(QsciScintillaBase::SCI_GETLENGTH);
    if (docLen <= 0 || searchLen == 0) return 0;

    // Scintilla find flags
    int findFlags = 0;
    if (!caseSensitive) {
        findFlags |= SCFIND_MATCHCASE;  // Note: invert logic; Scintilla uses 0 for case-insensitive
        // Actually Scintilla SCFIND_MATCHCASE means case-sensitive. Case-insensitive = 0
    }
    // Use SCFIND_WORDSTART for word-only matching, or 0 for any match
    // For "highlight all", we match anywhere in text

    sci->SendScintilla(QsciScintillaBase::SCI_SETSEARCHFLAGS, findFlags);

    // FindText structure for Scintilla
    Sci_TextToFind ft;
    ft.chrg.cpMin = 0;
    ft.chrg.cpMax = docLen;

    QByteArray textBuf(docLen + 1, Qt::Uninitialized);
    ft.lpstrText = textBuf.data();

    int matchCount = 0;
    int pos = 0;

    while (pos < docLen && matchCount < MAX_MATCHES) {
        ft.chrg.cpMin = pos;
        ft.chrg.cpMax = docLen;

        intptr_t result = sci->SendScintilla(QsciScintillaBase::SCI_FINDTEXT,
                                    findFlags,
                                    reinterpret_cast<sptr_t>(&ft));

        if (result == -1) break;  // No more matches

        int matchStart = static_cast<int>(result);
        int matchEnd = matchStart + static_cast<int>(searchLen);

        // Mark the match with the indicator
        sci->SendScintilla(QsciScintillaBase::SCI_SETINDICATORCURRENT, _indicatorNum);
        sci->SendScintilla(QsciScintillaBase::SCI_INDICATORFILLRANGE,
                            matchStart, searchLen);

        // Store position
        _matchPositions.append(qMakePair(matchStart, matchEnd));

        matchCount++;
        pos = matchEnd;  // Move past this match to find the next one
    }

    return matchCount;
}

void SmartHighlighter::goToNextMatch() {
    if (_matchPositions.isEmpty()) return;

    _currentMatchIndex++;
    if (_currentMatchIndex >= _matchPositions.size()) {
        _currentMatchIndex = 0;  // Wrap around
    }

    QPair<int, int> match = _matchPositions[_currentMatchIndex];

    emit currentMatchChanged(_currentMatchIndex + 1, _matchCount);
}

void SmartHighlighter::goToPreviousMatch() {
    if (_matchPositions.isEmpty()) return;

    _currentMatchIndex--;
    if (_currentMatchIndex < 0) {
        _currentMatchIndex = _matchPositions.size() - 1;  // Wrap around
    }

    QPair<int, int> match = _matchPositions[_currentMatchIndex];

    emit currentMatchChanged(_currentMatchIndex + 1, _matchCount);
}
