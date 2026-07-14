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

intptr_t ScintillaEditView::sendCommand(int message, uptr_t wParam, sptr_t lParam) {
    if (!_widget) return 0;
    // QsciScintilla::SendScintilla accepts (int, uintptr_t, intptr_t) on Qt6
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
    // SCI_GETLINE: lParam = char* buffer, wParam = line number (0-indexed)
    // Returns number of bytes copied (excludes NUL terminator)
    // We need a temporary buffer
    if (line < 0 || line >= _widget->lines()) return QString();

    // Get the line length first
    int len = sendCommand(SCI_GETLINE, static_cast<uptr_t>(line), 0);
    if (len <= 0) return QString();

    QByteArray buf(len + 1, Qt::Uninitialized);
    buf[len] = '\0';
    // Copy the line bytes into buf using SCI_GETLINE
    // SCI_GETLINE returns bytes, writes to lParam buffer
    // We need to call it again with the buffer — use textRange instead
    // (SCI_GETLINE modifies the buffer at lParam and returns length)
    // For simplicity, use QsciScintilla API
    Q_UNUSED(len);
    QString qstr = _widget->text().section('\n', line, line);
    return qstr;
}

QByteArray ScintillaEditView::getTextRange(int start, int end) const {
    if (!_widget || start >= end) return QByteArray();

    // Use SCI_GETTEXTRANGE
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

    // We need a non-const this — use const_cast carefully
    const_cast<ScintillaEditView*>(this)->sendCommand(
        2612, // SCI_GETTEXTRANGE = 2612
        0, reinterpret_cast<sptr_t>(&tr));

    int actualLen = qstrlen(buf.data());
    buf.truncate(actualLen);
    return buf;
}

void ScintillaEditView::replaceSel(const QString& text) {
    if (!_widget) return;
    QByteArray utf8 = text.toUtf8();
    sendCommand(SCI_REPLACESEL, 0, utf8.constData());
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
    sendCommand(operation, 0, 0);
}

void ScintillaEditView::joinLines() {
    // SCI_LINESJOIN: joins all lines in the selected range
    sendCommand(SCI_LINESJOIN, 0, 0);
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
        sendCommand(SCI_SETSELECTIONMODE, SC_SEL_RECTANGLE, 0);
        sendCommand(20032, 1, 0); // SCI_SETVIRTUALSPACEOPTIONS = 20032
    } else {
        sendCommand(SCI_SETSELECTIONMODE, SC_SEL_STREAM, 0);
        sendCommand(20032, 0, 0);
    }
}

int ScintillaEditView::selectionMode() const {
    return static_cast<int>(sendCommand(SCI_GETSELECTIONMODE, 0, 0));
}

bool ScintillaEditView::isRectangularSelection() const {
    return selectionMode() == SC_SEL_RECTANGLE;
}
