// ScintillaEditView.cpp — Low-level Scintilla edit view implementation
// Provides a thin C++ wrapper around Scintilla's direct SCI_* messaging API.
// Copyright (C) 2026 Agent Army / GPL v3

#include "ScintillaEditView.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciscintillabase.h>
#include <QWidget>
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <algorithm>

ScintillaEditView::ScintillaEditView()
    : _widget(new QsciScintilla())
{
    // Configure basic defaults
    _widget->setUtf8(true);
    _widget->setEolMode(QsciScintilla::EolUnix);
    _widget->setTabWidth(4);
    _widget->setIndentationsUseTabs(false);
    _widget->setAutoIndent(true);
    _widget->setCaretLineVisible(true);

    // Virtual space: allow caret to move past end of line
    _widget->SendScintilla(QsciScintilla::SCI_SETVIRTUALSPACEOPTIONS, 1);
}

ScintillaEditView::~ScintillaEditView() {
    // Delete the widget; QApplication must still be alive
    delete _widget;
    _widget = nullptr;
}

// ---------------------------------------------------------------------------
// ScintillaComponent interface
// ---------------------------------------------------------------------------

intptr_t ScintillaEditView::send(int message, int wParam, intptr_t lParam) {
    return sendCommand(message, static_cast<uptr_t>(wParam), static_cast<sptr_t>(lParam));
}

// ---------------------------------------------------------------------------
// Direct SCI wrapper
// ---------------------------------------------------------------------------

intptr_t ScintillaEditView::sendCommand(int message, uptr_t wParam, sptr_t lParam) const {
    if (!_widget) return 0;
    return _widget->SendScintilla(static_cast<int>(message),
                                  static_cast<unsigned long>(wParam),
                                  static_cast<long>(lParam));
}

// ---------------------------------------------------------------------------
// Text access
// ---------------------------------------------------------------------------

QString ScintillaEditView::getText() const {
    if (!_widget) return QString();
    return _widget->text();
}

void ScintillaEditView::setText(const QString& text) {
    if (!_widget) return;
    _widget->setText(text);
}

QString ScintillaEditView::getLine(int line) const {
    if (!_widget) return QString();
    if (line < 0 || line >= _widget->lines()) return QString();
    // SCI_GETLINE: lParam = char* buffer, wParam = line number (0-indexed)
    // Returns number of bytes copied (excludes NUL terminator)
    int len = static_cast<int>(sendCommand(SCI_GETLINE,
                                          static_cast<uptr_t>(line), 0));
    if (len <= 0) return QString();
    QByteArray buf(len + 1, Qt::Uninitialized);
    buf[len] = '\0';
    // Actually retrieve the line by calling SCI_GETLINE again with buffer
    const_cast<ScintillaEditView*>(this)->sendCommand(
        SCI_GETLINE, static_cast<uptr_t>(line),
        reinterpret_cast<sptr_t>(buf.data()));
    buf.truncate(len);
    return QString::fromUtf8(buf);
}

QByteArray ScintillaEditView::getTextRange(int start, int end) const {
    if (!_widget || start >= end) return QByteArray();
    const int docLen = static_cast<int>(sendCommand(SCI_GETLENGTH, 0, 0));
    if (docLen <= 0) return QByteArray();
    // Clamp to document bounds — SCI_GETTEXTRANGE with invalid positions
    // (e.g. start < 0, end > docLen) triggers QsciScintilla::fatal().
    start = qBound(0, start, docLen);
    end   = qBound(0, end,   docLen);
    if (start >= end) return QByteArray();

    struct Sci_TextRange {
        struct Sci_CharacterRange {
            long cpMin;
            long cpMax;
        } chrg;
        char* lpstrText;
    };

    Sci_TextRange tr;
    tr.chrg.cpMin = static_cast<long>(start);
    tr.chrg.cpMax = static_cast<long>(end);
    QByteArray buf(end - start + 1, Qt::Uninitialized);
    tr.lpstrText = buf.data();

    const_cast<ScintillaEditView*>(this)->sendCommand(
        SCI_GETTEXTRANGE, 0, reinterpret_cast<sptr_t>(&tr));

    int actualLen = qstrlen(buf.data());
    buf.truncate(actualLen);
    return buf;
}

void ScintillaEditView::replaceSel(const QString& text) {
    if (!_widget) return;
    QByteArray utf8 = text.toUtf8();
    sendCommand(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>(utf8.constData()));
}

int ScintillaEditView::lineCount() const {
    if (!_widget) return 0;
    return _widget->lines();
}

int ScintillaEditView::textLength() const {
    return static_cast<int>(sendCommand(SCI_GETTEXTLENGTH, 0, 0));
}

void ScintillaEditView::convertEol(int eolMode) {
    sendCommand(SCI_CONVERTEOLS, static_cast<uptr_t>(eolMode), 0);
}

// ---------------------------------------------------------------------------
// Text transformations
// ---------------------------------------------------------------------------

void ScintillaEditView::convertCase(int operation) {
    // Valid operations: SCI_LOWERCASE (2342), SCI_UPPERCASE (2343), SCI_TITLECASE (2344)
    sendCommand(operation, 0);
}

void ScintillaEditView::joinLines() {
    // SCI_LINESJOIN: joins all lines in the selected range
    sendCommand(SCI_LINESJOIN, 0);
}

void ScintillaEditView::sortLines(int mode) {
    // SCI_SORTLINES: sorts selected lines
    // mode: SC_SORT_NUMERIC(1), SC_SORT_CASEINSENSITIVE(2),
    //       SC_SORT_PERCENTAGE(3), combined with SC_SORT_REVERSE(0x100)
    sendCommand(SCI_SORTLINES, static_cast<uptr_t>(mode), 0);
}

// ---------------------------------------------------------------------------
// Selection
// ---------------------------------------------------------------------------

void ScintillaEditView::setRectangularSelection(bool on) {
    if (on) {
        sendCommand(SCI_SETSELECTIONMODE, QsciScintillaBase::SC_SEL_RECTANGLE);
        sendCommand(SCI_SETVIRTUALSPACEOPTIONS, SCVS_USERACCESSIBLE);
    } else {
        sendCommand(SCI_SETSELECTIONMODE, QsciScintillaBase::SC_SEL_STREAM);
        sendCommand(SCI_SETVIRTUALSPACEOPTIONS, SCVS_NONE);
    }
}

int ScintillaEditView::selectionMode() const {
    return static_cast<int>(sendCommand(SCI_GETSELECTIONMODE, 0, 0));
}

bool ScintillaEditView::isRectangularSelection() const {
    return selectionMode() == QsciScintillaBase::SC_SEL_RECTANGLE;
}

// ---------------------------------------------------------------------------
// Word / identifier navigation
// ---------------------------------------------------------------------------

void ScintillaEditView::wordLeft() {
    sendCommand(SCI_WORDLEFT, 0);
}

void ScintillaEditView::wordRight() {
    sendCommand(SCI_WORDRIGHT, 0);
}

void ScintillaEditView::wordLeftEnd() {
    sendCommand(SCI_WORDLEFTEND, 0);
}

void ScintillaEditView::wordRightEnd() {
    sendCommand(SCI_WORDRIGHTEND, 0);
}

void ScintillaEditView::selectWord() {
    // Move to start of word, then extend selection to end of word
    int startPos = static_cast<int>(sendCommand(SCI_WORDSTARTPOSITION,
                                                 sendCommand(SCI_GETCURRENTPOS, 0, 0), 0));
    int endPos = static_cast<int>(sendCommand(SCI_WORDENDPOSITION,
                                               sendCommand(SCI_GETCURRENTPOS, 0, 0), 0));
    sendCommand(SCI_SETSELECTION, startPos, endPos);
}

// ---------------------------------------------------------------------------
// Line operations
// ---------------------------------------------------------------------------

void ScintillaEditView::moveLineUp() {
    // Save current selection/cursor
    int curPos = static_cast<int>(sendCommand(SCI_GETCURRENTPOS, 0, 0));
    int anchor = static_cast<int>(sendCommand(SCI_GETANCHOR, 0, 0));
    int line = static_cast<int>(sendCommand(SCI_LINEFROMPOSITION, static_cast<uptr_t>(curPos), 0));

    if (line <= 0) return; // Already at top

    sendCommand(SCI_BEGINUNDOACTION, 0);

    // Get the line above and current line
    int prevLineStart = static_cast<int>(sendCommand(SCI_POSITIONFROMLINE,
                                                     static_cast<uptr_t>(line - 1), 0));
    int prevLineEnd = static_cast<int>(sendCommand(SCI_GETLINEENDPOSITION,
                                                    static_cast<uptr_t>(line - 1), 0));
    int currLineStart = static_cast<int>(sendCommand(SCI_POSITIONFROMLINE,
                                                     static_cast<uptr_t>(line), 0));
    int currLineEnd = static_cast<int>(sendCommand(SCI_GETLINEENDPOSITION,
                                                   static_cast<uptr_t>(line), 0));
    // nextLineEnd: available for future multi-line move operations
    int prevLen = prevLineEnd - prevLineStart;
    int currLen = currLineEnd - currLineStart;

    QByteArray prevLineBuf(prevLen + 1, 0);
    QByteArray currLineBuf(currLen + 1, 0);

    sendCommand(SCI_GETLINE, static_cast<uptr_t>(line - 1),
                reinterpret_cast<sptr_t>(prevLineBuf.data()));
    sendCommand(SCI_GETLINE, static_cast<uptr_t>(line),
                reinterpret_cast<sptr_t>(currLineBuf.data()));

    // Replace prev line with curr line text
    sendCommand(SCI_SETTARGETSTART, prevLineStart);
    sendCommand(SCI_SETTARGETEND, prevLineEnd);
    sendCommand(SCI_REPLACETARGET, currLen, reinterpret_cast<sptr_t>(currLineBuf.data()));

    // Replace curr line with prev line text
    // Positions shift after first replacement
    int newCurrStart = prevLineStart + currLen;
    int newCurrEnd = newCurrStart + prevLen;
    sendCommand(SCI_SETTARGETSTART, newCurrStart);
    sendCommand(SCI_SETTARGETEND, newCurrEnd);
    sendCommand(SCI_REPLACETARGET, prevLen, reinterpret_cast<sptr_t>(prevLineBuf.data()));

    // Restore cursor
    int delta = currLen + 1; // includes newline
    int newCurPos = (curPos >= currLineStart) ? (curPos + delta - prevLen - 1)
                                              : (curPos - prevLen - 1);
    newCurPos = std::max(0, newCurPos);
    sendCommand(SCI_SETCURRENTPOS, static_cast<uptr_t>(newCurPos), 0);
    sendCommand(SCI_SETANCHOR, static_cast<uptr_t>(
        (anchor >= currLineStart) ? (anchor + delta - prevLen - 1)
                                  : (anchor - prevLen - 1)), 0);

    sendCommand(SCI_ENDUNDOACTION, 0);
}

void ScintillaEditView::moveLineDown() {
    int curPos = static_cast<int>(sendCommand(SCI_GETCURRENTPOS, 0, 0));
    int anchor = static_cast<int>(sendCommand(SCI_GETANCHOR, 0, 0));
    int line = static_cast<int>(sendCommand(SCI_LINEFROMPOSITION,
                                            static_cast<uptr_t>(curPos), 0));
    int totalLines = lineCount();
    if (line >= totalLines - 1) return;

    sendCommand(SCI_BEGINUNDOACTION, 0);

    int currLineStart = static_cast<int>(sendCommand(SCI_POSITIONFROMLINE,
                                                     static_cast<uptr_t>(line), 0));
    int currLineEnd = static_cast<int>(sendCommand(SCI_GETLINEENDPOSITION,
                                                   static_cast<uptr_t>(line), 0));
    int nextLineStart = static_cast<int>(sendCommand(SCI_POSITIONFROMLINE,
                                                      static_cast<uptr_t>(line + 1), 0));
    int nextLineEnd = static_cast<int>(sendCommand(SCI_GETLINEENDPOSITION,
                                                    static_cast<uptr_t>(line + 1), 0));

    int currLen = currLineEnd - currLineStart;
    int nextLen = nextLineEnd - nextLineStart;

    QByteArray currLineBuf(currLen + 1, 0);
    QByteArray nextLineBuf(nextLen + 1, 0);

    sendCommand(SCI_GETLINE, static_cast<uptr_t>(line),
                reinterpret_cast<sptr_t>(currLineBuf.data()));
    sendCommand(SCI_GETLINE, static_cast<uptr_t>(line + 1),
                reinterpret_cast<sptr_t>(nextLineBuf.data()));

    // Swap: next becomes curr, curr becomes next
    sendCommand(SCI_SETTARGETSTART, currLineStart);
    sendCommand(SCI_SETTARGETEND, nextLineEnd);
    QByteArray combined = nextLineBuf + currLineBuf;
    sendCommand(SCI_REPLACETARGET, combined.size(),
                reinterpret_cast<sptr_t>(combined.constData()));

    // Restore cursor
    int newCurPos = (curPos >= nextLineStart)
        ? (curPos - nextLen - 1)
        : (curPos + nextLen + 1);
    newCurPos = std::max(0, newCurPos);
    sendCommand(SCI_SETCURRENTPOS, static_cast<uptr_t>(newCurPos), 0);
    sendCommand(SCI_SETANCHOR, static_cast<uptr_t>(
        (anchor >= nextLineStart) ? (anchor - nextLen - 1)
                                  : (anchor + nextLen + 1)), 0);

    sendCommand(SCI_ENDUNDOACTION, 0);
}

void ScintillaEditView::duplicateLine() {
    // SCI_LINEDUPLICATE: duplicates the line containing the caret
    sendCommand(SCI_LINEDUPLICATE, 0);
}

void ScintillaEditView::deleteLine() {
    // SCI_LINEDELETE: deletes the line containing the caret
    sendCommand(SCI_LINEDELETE, 0);
}

void ScintillaEditView::deleteLineEnd() {
    // Delete from caret to end of line (not including newline)
    int curPos = static_cast<int>(sendCommand(SCI_GETCURRENTPOS, 0, 0));
    int lineEnd = static_cast<int>(sendCommand(SCI_GETLINEENDPOSITION,
                                               sendCommand(SCI_LINEFROMPOSITION,
                                                           static_cast<uptr_t>(curPos), 0), 0));
    if (lineEnd > curPos) {
        sendCommand(SCI_BEGINUNDOACTION, 0);
        sendCommand(SCI_SETSELECTION, static_cast<uptr_t>(curPos),
                    static_cast<sptr_t>(lineEnd));
        sendCommand(SCI_CLEAR, 0);
        sendCommand(SCI_ENDUNDOACTION, 0);
    }
}

void ScintillaEditView::lineJoin() {
    sendCommand(SCI_LINESJOIN, 0);
}

void ScintillaEditView::lineCut() {
    sendCommand(SCI_LINECUT, 0);
}

void ScintillaEditView::lineTranspose() {
    // SCI_LINETRANSPOSE: swaps the current line with the one above
    sendCommand(SCI_LINETRANSPOSE, 0);
}

// ---------------------------------------------------------------------------
// Indent / outdent
// ---------------------------------------------------------------------------

void ScintillaEditView::tabKey() {
    sendCommand(SCI_TAB, 0);
}

void ScintillaEditView::backtabKey() {
    sendCommand(SCI_BACKTAB, 0);
}

// ---------------------------------------------------------------------------
// Brace matching
// ---------------------------------------------------------------------------

void ScintillaEditView::braceMatch() {
    sendCommand(SCI_BRACEMATCH, -1);
}

void ScintillaEditView::braceHighlight() {
    // SCI_HIGHLIGHTBRACES: highlight matching braces within visible range
    sendCommand(SCI_HIGHLIGHTBRACES, 0);
}

bool ScintillaEditView::findMatchingBrace(int& endPos) const {
    int curPos = static_cast<int>(sendCommand(SCI_GETCURRENTPOS, 0, 0));
    int match = static_cast<int>(const_cast<ScintillaEditView*>(this)->sendCommand(
        SCI_BRACEMATCH, static_cast<uptr_t>(curPos - 1), 0));
    if (match >= 0) {
        endPos = match;
        return true;
    }
    // Try one position ahead
    match = static_cast<int>(const_cast<ScintillaEditView*>(this)->sendCommand(
        SCI_BRACEMATCH, static_cast<uptr_t>(curPos), 0));
    if (match >= 0) {
        endPos = match;
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Code folding
// ---------------------------------------------------------------------------

int ScintillaEditView::getFoldLevel(int line) const {
    if (!_widget || line < 0 || line >= _widget->lines()) return 0;
    return static_cast<int>(sendCommand(SCI_GETFOLDLEVEL,
                                       static_cast<uptr_t>(line), 0)) & 0x0FFF;
}

void ScintillaEditView::toggleFoldAt(int line) {
    if (!_widget || line < 0 || line >= _widget->lines()) return;
    // Only toggle if this line has a fold point (header line)
    int level = getFoldLevel(line);
    if (level & QsciScintillaBase::SC_FOLDLEVELHEADERFLAG) {
        _widget->foldLine(line);
    }
}

void ScintillaEditView::collapseAllFolds() {
    if (!_widget) return;
    _widget->foldAll(false);
}

void ScintillaEditView::unfoldAllFolds() {
    if (!_widget) return;
    _widget->foldAll(true);
}

void ScintillaEditView::toggleAllFolds() {
    if (!_widget) return;
    // Collapse/fold all headers
    for (int line = 0; line < _widget->lines(); ++line) {
        int level = getFoldLevel(line);
        if (level & QsciScintillaBase::SC_FOLDLEVELHEADERFLAG) {
            int headerLine = line;
            int ln = headerLine;
            bool isExpanded = (getFoldLevel(ln) & QsciScintillaBase::SC_FOLDLEVELHEADERFLAG) != 0;
            (void)isExpanded;
            // Check if it's expanded
            if ((getFoldLevel(ln) & QsciScintillaBase::SC_FOLDLEVELHEADERFLAG) &&
                (sendCommand(SCI_GETFOLDEXPANDED, static_cast<uptr_t>(ln), 0) == 0)) {
                // Already collapsed, unfold it
                sendCommand(SCI_SETFOLDEXPANDED, static_cast<uptr_t>(ln), 1);
                sendCommand(SCI_FOLDCHILDREN, static_cast<uptr_t>(ln),
                            QsciScintillaBase::SC_FOLDACTION_EXPAND);
            }
        }
    }
    // Actually fold all using Scintilla's built-in
    _widget->foldAll(false);
}

void ScintillaEditView::collapseLevel(int level) {
    if (!_widget) return;
    // Fold all nodes at or below the given level
    for (int line = 0; line < _widget->lines(); ++line) {
        int lvl = getFoldLevel(line);
        int depth = lvl & QsciScintillaBase::SC_FOLDLEVELNUMBERMASK;
        if ((lvl & QsciScintillaBase::SC_FOLDLEVELHEADERFLAG) && depth >= static_cast<int>(level)) {
            sendCommand(SCI_SETFOLDEXPANDED, static_cast<uptr_t>(line), 0);
            sendCommand(SCI_FOLDCHILDREN, static_cast<uptr_t>(line), QsciScintillaBase::SC_FOLDACTION_CONTRACT);
        }
    }
}

void ScintillaEditView::unfoldLevel(int level) {
    if (!_widget) return;
    for (int line = 0; line < _widget->lines(); ++line) {
        int lvl = getFoldLevel(line);
        int depth = lvl & QsciScintillaBase::SC_FOLDLEVELNUMBERMASK;
        if (depth >= static_cast<int>(level)) {
            sendCommand(SCI_SETFOLDEXPANDED, static_cast<uptr_t>(line), 1);
            sendCommand(SCI_FOLDCHILDREN, static_cast<uptr_t>(line), QsciScintillaBase::SC_FOLDACTION_EXPAND);
        }
    }
}

int ScintillaEditView::maxFoldLevel() const {
    if (!_widget) return 0;
    int maxLevel = 0;
    for (int line = 0; line < _widget->lines(); ++line) {
        int level = getFoldLevel(line);
        int depth = level & QsciScintillaBase::SC_FOLDLEVELNUMBERMASK;
        if (depth > maxLevel) maxLevel = depth;
    }
    return maxLevel;
}

// ---------------------------------------------------------------------------
// Find / replace markers
// ---------------------------------------------------------------------------

void ScintillaEditView::addFindMark(int indicator, int line, int startCol, int endCol) {
    if (!_widget || indicator < 0 || indicator > 7) return;
    int startPos = static_cast<int>(sendCommand(SCI_POSITIONFROMLINE,
                                                static_cast<uptr_t>(line), 0)) + startCol;
    int endPos = static_cast<int>(sendCommand(SCI_POSITIONFROMLINE,
                                              static_cast<uptr_t>(line), 0)) + endCol;
    _widget->fillIndicatorRange(line, startCol, line, endCol, indicator);
    Q_UNUSED(startPos);
    Q_UNUSED(endPos);
}

void ScintillaEditView::clearAllFinds(int indicator) {
    if (!_widget) return;
    _widget->clearIndicatorRange(0, 0, _widget->lines(), 0, indicator);
}

void ScintillaEditView::addFindMarks(int indicator,
                                      const QVector<QPair<int, int>>& positions) {
    for (const auto& pos : positions) {
        addFindMark(indicator, pos.first, 0, pos.second);
    }
}

// ---------------------------------------------------------------------------
// Column mode / rectangular selection helpers
// ---------------------------------------------------------------------------

void ScintillaEditView::columnEdit(int line, int startCol, int endCol) {
    // Set rectangular selection
    sendCommand(SCI_SETSELECTIONMODE, QsciScintillaBase::SC_SEL_RECTANGLE);
    sendCommand(SCI_SETVIRTUALSPACEOPTIONS, SCVS_USERACCESSIBLE);
    int startPos = static_cast<int>(sendCommand(SCI_POSITIONFROMLINE,
                                               static_cast<uptr_t>(line), 0)) + startCol;
    int endPos = static_cast<int>(sendCommand(SCI_POSITIONFROMLINE,
                                             static_cast<uptr_t>(line), 0)) + endCol;
    sendCommand(SCI_SETSELECTION, static_cast<uptr_t>(startPos),
                static_cast<uptr_t>(endPos));
}

void ScintillaEditView::setColumnSelection(int startPos, int endPos) {
    sendCommand(SCI_SETSELECTIONMODE, QsciScintillaBase::SC_SEL_RECTANGLE);
    sendCommand(SCI_SETSELECTION, static_cast<uptr_t>(startPos),
                static_cast<uptr_t>(endPos));
}

void ScintillaEditView::columnInsert(const QString& text) {
    if (!_widget) return;
    QByteArray utf8 = text.toUtf8();
    // For each selection in turn (multi-caret), insert the text
    int selCount = static_cast<int>(sendCommand(SCI_GETSELECTIONS, 0, 0));
    sendCommand(SCI_BEGINUNDOACTION, 0);
    for (int i = 0; i < selCount; ++i) {
        int start = static_cast<int>(sendCommand(SCI_GETSELECTIONNSTART, i, 0));
        int end = static_cast<int>(sendCommand(SCI_GETSELECTIONNEND, i, 0));
        sendCommand(SCI_SETTARGETSTART, start);
        sendCommand(SCI_SETTARGETEND, end);
        sendCommand(SCI_REPLACETARGET, utf8.size(),
                    reinterpret_cast<sptr_t>(utf8.constData()));
    }
    sendCommand(SCI_ENDUNDOACTION, 0);
    Q_UNUSED(selCount);
}

void ScintillaEditView::columnDelete() {
    // Delete all selections (column mode)
    sendCommand(SCI_CLEARSELECTIONS, 0);
}

bool ScintillaEditView::isColumnSelection() const {
    return selectionMode() == QsciScintillaBase::SC_SEL_RECTANGLE;
}

// ---------------------------------------------------------------------------
// Virtual space
// ---------------------------------------------------------------------------

void ScintillaEditView::setVirtualSpaceOptions(int mode) {
    sendCommand(SCI_SETVIRTUALSPACEOPTIONS, static_cast<uptr_t>(mode), 0);
}

int ScintillaEditView::virtualSpaceOptions() const {
    return static_cast<int>(sendCommand(SCI_GETVIRTUALSPACEOPTIONS, 0, 0));
}

// ---------------------------------------------------------------------------
// Scrollbar
// ---------------------------------------------------------------------------

void ScintillaEditView::setFirstVisibleLine(int line) {
    sendCommand(SCI_SETFIRSTVISIBLELINE, static_cast<uptr_t>(line), 0);
}

int ScintillaEditView::firstVisibleLine() const {
    return static_cast<int>(sendCommand(SCI_GETFIRSTVISIBLELINE, 0, 0));
}

void ScintillaEditView::scrollLines(int deltaLines) {
    int current = firstVisibleLine();
    setFirstVisibleLine(current + deltaLines);
}

void ScintillaEditView::scrollColumns(int deltaCols) {
    sendCommand(SCI_LINESCROLL, static_cast<uptr_t>(deltaCols), 0);
}

// ---------------------------------------------------------------------------
// Search / target
// ---------------------------------------------------------------------------

void ScintillaEditView::setTarget(int start, int end) {
    sendCommand(SCI_SETTARGETSTART, static_cast<uptr_t>(start), 0);
    sendCommand(SCI_SETTARGETEND, static_cast<uptr_t>(end), 0);
}

int ScintillaEditView::searchInTarget(const QString& text) const {
    if (!_widget) return -1;
    QByteArray utf8 = text.toUtf8();
    return static_cast<int>(const_cast<ScintillaEditView*>(this)->sendCommand(
        SCI_SEARCHINTARGET, utf8.size(),
        reinterpret_cast<sptr_t>(utf8.constData())));
}

int ScintillaEditView::targetStart() const {
    return static_cast<int>(sendCommand(SCI_GETTARGETSTART, 0, 0));
}

int ScintillaEditView::targetEnd() const {
    return static_cast<int>(sendCommand(SCI_GETTARGETEND, 0, 0));
}

int ScintillaEditView::targetFoundLength() const {
    return static_cast<int>(sendCommand(SCI_GETTARGETEND, 0, 0)
                            - sendCommand(SCI_GETTARGETSTART, 0, 0));
}

// ---------------------------------------------------------------------------
// Marker management
// ---------------------------------------------------------------------------

void ScintillaEditView::addMarker(int line, int markerNum) {
    if (!_widget) return;
    _widget->markerAdd(line, markerNum);
}

void ScintillaEditView::deleteMarker(int line, int markerNum) {
    if (!_widget) return;
    _widget->markerDelete(line, markerNum);
}

void ScintillaEditView::deleteAllMarkers(int markerNum) {
    if (!_widget) return;
    _widget->markerDeleteAll(markerNum);
}

bool ScintillaEditView::hasMarker(int line, int markerNum) const {
    if (!_widget) return false;
    return (_widget->markersAtLine(line) & (1 << markerNum)) != 0;
}

QVector<int> ScintillaEditView::markersWithType(int markerNum) const {
    QVector<int> lines;
    if (!_widget) return lines;
    int totalLines = _widget->lines();
    for (int line = 0; line < totalLines; ++line) {
        if ((_widget->markersAtLine(line) & (1 << markerNum)) != 0)
            lines.append(line);
    }
    return lines;
}

void ScintillaEditView::defineMarker(int markerNum, int style,
                                      const QColor& fore, const QColor& back) {
    if (!_widget) return;
    _widget->markerDefine(static_cast<QsciScintilla::MarkerSymbol>(style), markerNum);
    _widget->setMarkerForegroundColor(fore, markerNum);
    _widget->setMarkerBackgroundColor(back, markerNum);
}

// ---------------------------------------------------------------------------
// Document information
// ---------------------------------------------------------------------------

int ScintillaEditView::currentPos() const {
    return static_cast<int>(sendCommand(SCI_GETCURRENTPOS, 0, 0));
}

int ScintillaEditView::anchorPosition() const {
    return static_cast<int>(sendCommand(SCI_GETANCHOR, 0, 0));
}

void ScintillaEditView::setSelection(int anchor, int caret) {
    sendCommand(SCI_SETSELECTION, static_cast<uptr_t>(anchor),
                static_cast<uptr_t>(caret));
}

int ScintillaEditView::selectionStart() const {
    return static_cast<int>(sendCommand(SCI_GETSELECTIONNSTART, 0, 0));
}

int ScintillaEditView::selectionEnd() const {
    return static_cast<int>(sendCommand(SCI_GETSELECTIONNEND, 0, 0));
}

void ScintillaEditView::clearSelections() {
    sendCommand(SCI_CLEARSELECTIONS, 0);
}

void ScintillaEditView::addSelection(int anchor, int caret) {
    sendCommand(SCI_ADDSELECTION, static_cast<uptr_t>(anchor),
                static_cast<uptr_t>(caret));
}

int ScintillaEditView::selectionCount() const {
    return static_cast<int>(sendCommand(SCI_GETSELECTIONS, 0, 0));
}
