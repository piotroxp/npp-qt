// ScintillaEditor.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ScintillaEditor.h"
#include "SyntaxHighlighter.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercustom.h>
#include <QVBoxLayout>
#include <QFont>
#include <QCoreApplication>

ScintillaEditor::ScintillaEditor(QWidget* parent)
    : QFrame(parent)
    , _editor(new QsciScintilla())
    , _highlighter(nullptr)  // QsciScintilla handles highlighting via lexers, not QSyntaxHighlighter
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_editor);
    layout->setSpacing(0);

    _editor->setUtf8(true);
    _editor->setEolMode(QsciScintilla::EolUnix);
    _editor->setMarginType(0, QsciScintilla::NumberMargin);
    _editor->setMarginWidth(0, "00000");
    _editor->setTabWidth(4);
    _editor->setIndentationsUseTabs(false);
    _editor->setAutoIndent(true);
    _editor->setCaretLineVisible(true);
    _editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);

    connect(_editor, &QsciScintilla::textChanged, this, &ScintillaEditor::textChanged);
    connect(_editor, &QsciScintilla::cursorPositionChanged, [this](int line, int col) {
        emit cursorPositionChanged(line, col);
    });
    connect(_editor, &QsciScintilla::modificationChanged, this, &ScintillaEditor::modificationChanged);
    connect(_editor, &QsciScintilla::selectionChanged, this, &ScintillaEditor::selectionChanged);
}

ScintillaEditor::~ScintillaEditor() = default;

void ScintillaEditor::setText(const QString& text) { _editor->setText(text); }
QString ScintillaEditor::text() const { return _editor->text(); }
void ScintillaEditor::setPlainText(const QString& text) { _editor->setText(text); }
QString ScintillaEditor::toPlainText() const { return _editor->text(); }

void ScintillaEditor::setLanguage(LangType lang) {
    _language = lang;
    if (_highlighter) _highlighter->setLanguage(lang);
}
void ScintillaEditor::setEncoding(EncodingType enc) { _encoding = enc; }
void ScintillaEditor::setEolType(EolType eol) {
    _eolType = eol;
    switch (eol) {
        case EolType::EOL_LF: _editor->setEolMode(QsciScintilla::EolUnix); break;
        case EolType::EOL_CRLF: _editor->setEolMode(QsciScintilla::EolWindows); break;
        case EolType::EOL_CR: _editor->setEolMode(QsciScintilla::EolMac); break;
        default: break;
    }
}

int ScintillaEditor::currentLine() const {
    int line = 0, col = 0;
    _editor->getCursorPosition(&line, &col);
    return line;
}
int ScintillaEditor::currentColumn() const {
    int line = 0, col = 0;
    _editor->getCursorPosition(&line, &col);
    return col;
}
int ScintillaEditor::lineCount() const { return _editor->lines(); }

void ScintillaEditor::gotoLine(int line, int col) { _editor->setCursorPosition(line, col); }
void ScintillaEditor::setCursorPosition(int line, int col) { _editor->setCursorPosition(line, col); }

QString ScintillaEditor::selectedText() const { return _editor->selectedText(); }
void ScintillaEditor::setSelection(int, int, int, int) { }
void ScintillaEditor::selectAll() { _editor->selectAll(true); }

void ScintillaEditor::copy() { _editor->copy(); }
void ScintillaEditor::cut() { _editor->cut(); }
void ScintillaEditor::paste() { _editor->paste(); }
void ScintillaEditor::deleteSelection() { _editor->removeSelectedText(); }

void ScintillaEditor::undo() { _editor->undo(); }
void ScintillaEditor::redo() { _editor->redo(); }
bool ScintillaEditor::isUndoAvailable() const { return _editor->isUndoAvailable(); }
bool ScintillaEditor::isRedoAvailable() const { return _editor->isRedoAvailable(); }
void ScintillaEditor::setModified(bool m) { _editor->setModified(m); }
bool ScintillaEditor::isModified() const { return _editor->isModified(); }
void ScintillaEditor::setReadOnly(bool ro) { _editor->setReadOnly(ro); }
bool ScintillaEditor::isReadOnly() const { return _editor->isReadOnly(); }

void ScintillaEditor::setFont(const QFont& font) { _editor->setFont(font); }
QFont ScintillaEditor::font() const { return _editor->font(); }

void ScintillaEditor::setTabWidth(int w) { _editor->setTabWidth(w); }
int ScintillaEditor::tabWidth() const { return _editor->tabWidth(); }
void ScintillaEditor::setUseTabs(bool u) { _editor->setIndentationsUseTabs(u); }
bool ScintillaEditor::useTabs() const { return _editor->indentationsUseTabs(); }
void ScintillaEditor::setIndent(int) { }
int ScintillaEditor::indent() const { return 0; }

void ScintillaEditor::setLineNumberingEnabled(bool enabled) {
    _editor->setMarginWidth(0, enabled ? "00000" : "0");
}
bool ScintillaEditor::isLineNumberingEnabled() const { return _editor->marginWidth(0) != 0; }

void ScintillaEditor::setWhitespaceVisibility(bool) { }
void ScintillaEditor::setEolVisibility(bool) { }

void ScintillaEditor::setWrapMode(bool wrap) {
    _editor->setWrapMode(wrap ? QsciScintilla::WrapWord : QsciScintilla::WrapNone);
}
bool ScintillaEditor::wrapMode() const { return _editor->wrapMode() != QsciScintilla::WrapNone; }

void ScintillaEditor::setMarkerLine(int line) { _editor->setMarkerBackgroundColor(Qt::yellow, line); }
void ScintillaEditor::clearMarkerLine() { }

void ScintillaEditor::findNext(const QString& text, FindOption options) {
    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;
    bool forward = true;
    bool showMessages = false;
    _editor->findFirst(text, re, cs, wo, true /*wrap*/, true /*forward*/,
                       -1, -1, true /*show*/, false /*posix*/, false /*cxx11*/);
}
void ScintillaEditor::findPrevious(const QString& text, FindOption options) {
    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;
    bool forward = false;
    bool showMessages = false;
    _editor->findFirst(text, re, cs, wo,
                       true /*wrap*/, false /*backward*/,
                       -1, -1, true /*show*/, false, false);
}
int ScintillaEditor::countMatches(const QString& text, FindOption options) {
    if (text.isEmpty()) return 0;
    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;
    int count = 0;
    int line = 0, col = 0;
    // Search the whole document; findFirst returns false when no more matches
    bool found = _editor->findFirst(text, re, cs, wo, true, true, -1, -1, false, false, false);
    while (found) {
        ++count;
        // Get current position and search from next position
        _editor->getCursorPosition(&line, &col);
        found = _editor->findFirst(text, re, cs, wo, true, true, -1, -1, false, false, false);
    }
    return count;
}
void ScintillaEditor::replaceAll(const QString& find, const QString& rep, FindOption options) {
    if (find.isEmpty()) return;
    int count = 0;
    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;
    bool found = _editor->findFirst(find, re, cs, wo, true, true, -1, -1, false, false, false);
    while (found) {
        _editor->replace(rep);
        ++count;
        found = _editor->findFirst(find, re, cs, wo, true, true, -1, -1, false, false, false);
    }
    emit replaceAllDone(count);
}
void ScintillaEditor::replace(const QString& replacement) {
    _editor->replace(replacement);
}

int ScintillaEditor::markAllMatches(const QString& text, FindOption options) {
    if (text.isEmpty()) return 0;
    // Configure indicator 1: box style with yellow background
    _editor->indicatorDefine(QsciScintilla::BoxIndicator, 1);
    _editor->setIndicatorForegroundColor(QColor("#FFFF00"), 1);
    
    // Clear all existing highlights
    _editor->clearIndicatorRange(0, 0, _editor->lines(), 0, 1);
    int count = 0;
    int startLine = 0, startIdx = 0;
    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;
    bool found = _editor->findFirst(text, re, cs, wo, true, true, 0, 0, false, false, false);
    while (found) {
        int sl = 0, si = 0, el = 0, ei = 0;
        _editor->getSelection(&sl, &si, &el, &ei);
        if (sl >= 0 && (sl < el || (sl == el && si < ei))) {
            _editor->fillIndicatorRange(sl, si, el, ei - si, 1);
            ++count;
        }
        found = _editor->findNext();
        int cl = 0, ci = 0;
        _editor->getCursorPosition(&cl, &ci);
        // Guard: if we've wrapped back to start, stop
        if (cl < startLine || (cl == startLine && ci <= startIdx)) break;
        if (count > 100000) break;  // sanity limit
    }
    return count;
}


void ScintillaEditor::setAnnotationsEnabled(bool) { }
void ScintillaEditor::addAnnotation(int, const QString&) { }
void ScintillaEditor::clearAnnotations() { }

void ScintillaEditor::setAutoIndent(bool enabled) { _editor->setAutoIndent(enabled); }
void ScintillaEditor::setBackspaceUnindents(bool) { }
void ScintillaEditor::setTabIndents(bool) { }
void ScintillaEditor::setRectangularSelectionEnabled(bool) { }
void ScintillaEditor::setMultipleSelectionEnabled(bool) { }

void ScintillaEditor::convertEol(EolType eol) {
    setEolType(eol);
}
