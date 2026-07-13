// ScintillaEditor.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ScintillaEditor.h"
#include "SyntaxHighlighter.h"
#include "../core/ThemeManager.h"
#include "../core/LanguageManager.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercustom.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexeryaml.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexertex.h>
#include <Qsci/qscilexerxml.h>
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

    // Pre-configure indicators for match highlighting.
    // Indicator 0: for current-selection highlight (yellow)
    _editor->indicatorDefine(QsciScintilla::FullBoxIndicator, 0);
    _editor->setIndicatorForegroundColor(QColor("#FFFF00"), 0);
    // Indicator 1: for "mark all" highlights (light yellow/amber)
    _editor->indicatorDefine(QsciScintilla::BoxIndicator, 1);
    _editor->setIndicatorForegroundColor(QColor("#FFFF00"), 1);

    connect(_editor, &QsciScintilla::textChanged, this, &ScintillaEditor::textChanged);
    connect(_editor, &QsciScintilla::cursorPositionChanged, [this](int line, int col) {
        emit cursorPositionChanged(line, col);
    });
    connect(_editor, &QsciScintilla::modificationChanged, this, &ScintillaEditor::modificationChanged);
    connect(_editor, &QsciScintilla::selectionChanged, this, &ScintillaEditor::selectionChanged);
}

ScintillaEditor::~ScintillaEditor() {
    // Explicitly delete _editor before the ScintillaEditBase/QFrame destructor runs.
    // This ensures Scintilla's internal heap allocations are freed while the
    // QApplication event loop is still alive.  Without this, ScintillaEditBase
    // may be destroyed *after* QApplication exits its event loop, causing
    // "free(): invalid size" when the CRT tries to free Scintilla's malloc'd
    // memory after the heap is already torn down.
    delete _editor;
    _editor = nullptr;
}

void ScintillaEditor::setText(const QString& text) { _editor->setText(text); }
QString ScintillaEditor::text() const { return _editor->text(); }
void ScintillaEditor::setPlainText(const QString& text) { _editor->setText(text); }
QString ScintillaEditor::toPlainText() const { return _editor->text(); }

void ScintillaEditor::setLanguage(LangType lang) {
    _language = lang;
    if (_highlighter) _highlighter->setLanguage(lang);

    // Use QsciScintilla's built-in lexers via LanguageManager
    QsciLexer* lexer = LanguageManager::instance().getLexer(lang);
    _editor->setLexer(lexer);
    if (lexer) {
        applyThemeToLexer(lexer);
    }
}

void ScintillaEditor::applyTheme(const QString& themeName) {
    ThemeManager::instance().loadTheme(themeName);
    ThemeColors c = ThemeManager::instance().currentThemeColors();

    _editor->setColor(QColor(c.foreground));
    _editor->setPaper(QColor(c.background));
    _editor->setCaretForegroundColor(QColor(c.caretColor));
    _editor->setCaretLineBackgroundColor(QColor(c.currentLineBackground));
    _editor->setSelectionBackgroundColor(QColor(c.selectionBackground));
    // Re-apply lexer colors if a lexer is active
    if (_editor->lexer()) {
        applyThemeToLexer(_editor->lexer());
    }
}

void ScintillaEditor::applyThemeToLexer(QsciLexer* lexer) {
    if (!lexer) return;
    ThemeColors c = ThemeManager::instance().currentThemeColors();

    // Set base colors for all text
    lexer->setColor(QColor(c.foreground));
    lexer->setPaper(QColor(c.background));

    // Use the editor font as the default for all styles
    QFont f = _editor->font();
    lexer->setFont(f);

    // Map ThemeColors to common QsciLexer style roles.
    // QsciLexer uses its own per-lexer style constants (e.g. QsciLexerCPP::Comment).
    // We cast to specific types and set colours for the most important categories.
    // For unknown lexers, the base colour set above is used as fallback.

    if (auto* cpp = dynamic_cast<QsciLexerCPP*>(lexer)) {
        cpp->setColor(QColor(c.keywordColor),  QsciLexerCPP::Keyword);
        cpp->setColor(QColor(c.stringColor),   QsciLexerCPP::DoubleQuotedString);
        cpp->setColor(QColor(c.stringColor),   QsciLexerCPP::SingleQuotedString);
        cpp->setColor(QColor(c.commentColor),  QsciLexerCPP::Comment);
        cpp->setColor(QColor(c.commentColor),  QsciLexerCPP::CommentLine);
        cpp->setColor(QColor(c.numberColor),   QsciLexerCPP::Number);
    } else if (auto* py = dynamic_cast<QsciLexerPython*>(lexer)) {
        py->setColor(QColor(c.keywordColor),  QsciLexerPython::Keyword);
        py->setColor(QColor(c.stringColor),   QsciLexerPython::DoubleQuotedString);
        py->setColor(QColor(c.stringColor),   QsciLexerPython::SingleQuotedString);
        py->setColor(QColor(c.commentColor),  QsciLexerPython::Comment);
        py->setColor(QColor(c.numberColor),   QsciLexerPython::Number);
    } else if (auto* js = dynamic_cast<QsciLexerJavaScript*>(lexer)) {
        js->setColor(QColor(c.keywordColor),  QsciLexerCPP::Keyword);
        js->setColor(QColor(c.stringColor),   QsciLexerCPP::DoubleQuotedString);
        js->setColor(QColor(c.stringColor),   QsciLexerCPP::SingleQuotedString);
        js->setColor(QColor(c.commentColor),  QsciLexerCPP::Comment);
        js->setColor(QColor(c.numberColor),   QsciLexerCPP::Number);
    } else if (auto* html = dynamic_cast<QsciLexerHTML*>(lexer)) {
        html->setColor(QColor(c.keywordColor),  QsciLexerHTML::Tag);
        html->setColor(QColor(c.stringColor),   QsciLexerHTML::HTMLDoubleQuotedString);
        html->setColor(QColor(c.stringColor),   QsciLexerHTML::HTMLSingleQuotedString);
        html->setColor(QColor(c.commentColor),  QsciLexerHTML::HTMLComment);
        html->setColor(QColor(c.numberColor),   QsciLexerHTML::HTMLNumber);
    } else if (auto* css = dynamic_cast<QsciLexerCSS*>(lexer)) {
        css->setColor(QColor(c.keywordColor),  QsciLexerCSS::ClassSelector);
        css->setColor(QColor(c.stringColor),   QsciLexerCSS::DoubleQuotedString);
        css->setColor(QColor(c.stringColor),   QsciLexerCSS::SingleQuotedString);
        css->setColor(QColor(c.commentColor),  QsciLexerCSS::Comment);
        css->setColor(QColor(c.numberColor),   QsciLexerCSS::Value);
    } else if (auto* json = dynamic_cast<QsciLexerJSON*>(lexer)) {
        json->setColor(QColor(c.keywordColor),  QsciLexerJSON::Property);
        json->setColor(QColor(c.stringColor),   QsciLexerJSON::String);
        json->setColor(QColor(c.numberColor),   QsciLexerJSON::Number);
    } else if (auto* mkdn = dynamic_cast<QsciLexerMarkdown*>(lexer)) {
        mkdn->setColor(QColor(c.keywordColor),  QsciLexerMarkdown::Special);
        mkdn->setColor(QColor(c.commentColor),  QsciLexerMarkdown::BlockQuote);
        mkdn->setColor(QColor(c.stringColor),  QsciLexerMarkdown::Link);
    } else if (auto* mkf = dynamic_cast<QsciLexerMakefile*>(lexer)) {
        mkf->setColor(QColor(c.keywordColor),  QsciLexerMakefile::Target);
        mkf->setColor(QColor(c.stringColor),   QsciLexerMakefile::Variable);
        mkf->setColor(QColor(c.commentColor),  QsciLexerMakefile::Comment);
    } else if (auto* bash = dynamic_cast<QsciLexerBash*>(lexer)) {
        bash->setColor(QColor(c.keywordColor),  QsciLexerBash::Keyword);
        bash->setColor(QColor(c.stringColor),   QsciLexerBash::DoubleQuotedString);
        bash->setColor(QColor(c.stringColor),   QsciLexerBash::SingleQuotedString);
        bash->setColor(QColor(c.commentColor),  QsciLexerBash::Comment);
        bash->setColor(QColor(c.numberColor),   QsciLexerBash::Number);
    } else if (auto* bat = dynamic_cast<QsciLexerBatch*>(lexer)) {
        bat->setColor(QColor(c.keywordColor),  QsciLexerBatch::Keyword);
        bat->setColor(QColor(c.stringColor),   QsciLexerBatch::Variable);
        bat->setColor(QColor(c.commentColor),  QsciLexerBatch::Comment);
    } else if (auto* lua = dynamic_cast<QsciLexerLua*>(lexer)) {
        lua->setColor(QColor(c.keywordColor),  QsciLexerLua::Keyword);
        lua->setColor(QColor(c.stringColor),   QsciLexerLua::String);
        lua->setColor(QColor(c.commentColor),  QsciLexerLua::Comment);
        lua->setColor(QColor(c.numberColor),   QsciLexerLua::Number);
    } else if (auto* ruby = dynamic_cast<QsciLexerRuby*>(lexer)) {
        ruby->setColor(QColor(c.keywordColor),  QsciLexerRuby::Keyword);
        ruby->setColor(QColor(c.stringColor),   QsciLexerRuby::DoubleQuotedString);
        ruby->setColor(QColor(c.stringColor),   QsciLexerRuby::SingleQuotedString);
        ruby->setColor(QColor(c.commentColor),  QsciLexerRuby::Comment);
        ruby->setColor(QColor(c.numberColor),   QsciLexerRuby::Number);
    } else if (auto* perl = dynamic_cast<QsciLexerPerl*>(lexer)) {
        perl->setColor(QColor(c.keywordColor),  QsciLexerPerl::Keyword);
        perl->setColor(QColor(c.stringColor),   QsciLexerPerl::DoubleQuotedString);
        perl->setColor(QColor(c.stringColor),   QsciLexerPerl::SingleQuotedString);
        perl->setColor(QColor(c.commentColor),  QsciLexerPerl::Comment);
        perl->setColor(QColor(c.numberColor),   QsciLexerPerl::Number);
    } else if (auto* yaml = dynamic_cast<QsciLexerYAML*>(lexer)) {
        yaml->setColor(QColor(c.keywordColor),  QsciLexerYAML::Keyword);
        yaml->setColor(QColor(c.commentColor),  QsciLexerYAML::Comment);
        yaml->setColor(QColor(c.numberColor),   QsciLexerYAML::Number);
    } else if (auto* diff = dynamic_cast<QsciLexerDiff*>(lexer)) {
        diff->setColor(QColor(c.stringColor),   QsciLexerDiff::LineAdded);
        diff->setColor(QColor(c.keywordColor),  QsciLexerDiff::LineRemoved);
        diff->setColor(QColor(c.commentColor),  QsciLexerDiff::Comment);
    } else if (auto* sql = dynamic_cast<QsciLexerSQL*>(lexer)) {
        sql->setColor(QColor(c.keywordColor),  QsciLexerSQL::Keyword);
        sql->setColor(QColor(c.stringColor),   QsciLexerSQL::DoubleQuotedString);
        sql->setColor(QColor(c.stringColor),   QsciLexerSQL::SingleQuotedString);
        sql->setColor(QColor(c.commentColor),  QsciLexerSQL::Comment);
        sql->setColor(QColor(c.numberColor),   QsciLexerSQL::Number);
    } else if (auto* tex = dynamic_cast<QsciLexerTeX*>(lexer)) {
        tex->setColor(QColor(c.keywordColor),  QsciLexerTeX::Command);
        tex->setColor(QColor(c.stringColor),   QsciLexerTeX::Text);
    } else if (auto* xml = dynamic_cast<QsciLexerXML*>(lexer)) {
        xml->setColor(QColor(c.keywordColor),  QsciLexerHTML::Tag);
        xml->setColor(QColor(c.stringColor),   QsciLexerHTML::HTMLDoubleQuotedString);
        xml->setColor(QColor(c.commentColor),  QsciLexerHTML::HTMLComment);
    }
    // L_TEXT and other unhandled lexers: base colour already set above
    Q_UNUSED(c);
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
void ScintillaEditor::setIndent(int indentLevel) {
    int line = 0, col = 0;
    _editor->getCursorPosition(&line, &col);
    _editor->setIndentation(line, indentLevel);
}
int ScintillaEditor::indent() const {
    int line = 0, col = 0;
    _editor->getCursorPosition(&line, &col);
    return _editor->indentation(line);
}

void ScintillaEditor::setLineNumberingEnabled(bool enabled) {
    _editor->setMarginWidth(0, enabled ? "00000" : "0");
}
bool ScintillaEditor::isLineNumberingEnabled() const { return _editor->marginWidth(0) != 0; }

void ScintillaEditor::setWhitespaceVisibility(bool visible) {
    _editor->setWhitespaceVisibility(visible
        ? QsciScintilla::WsVisible
        : QsciScintilla::WsInvisible);
}
void ScintillaEditor::setEolVisibility(bool) {
    // EOL character visibility in Scintilla requires SCI_SETVIEWWS.
    // Qsci does not expose this directly; EOL visibility is controlled
    // by the document's EOL mode set via setEolType(). No-op here.
}

void ScintillaEditor::setCaretLineVisibility(bool visible) {
    _editor->setCaretLineVisible(visible);
}
bool ScintillaEditor::isCaretLineVisible() const {
    return _editor->SendScintilla(QsciScintilla::SCI_GETCARETLINEVISIBLE) != 0;
}
void ScintillaEditor::setCaretWidth(int pixels) {
    _editor->setCaretWidth(pixels);
}
int ScintillaEditor::caretWidth() const {
    return _editor->SendScintilla(QsciScintilla::SCI_GETCARETWIDTH);
}
void ScintillaEditor::setCaretForegroundColor(const QColor& color) {
    _editor->setCaretForegroundColor(color);
}
void ScintillaEditor::setCaretLineBackgroundColor(const QColor& color) {
    _editor->setCaretLineBackgroundColor(color);
}

void ScintillaEditor::setWrapMode(bool wrap) {
    _editor->setWrapMode(wrap ? QsciScintilla::WrapWord : QsciScintilla::WrapNone);
}
bool ScintillaEditor::wrapMode() const { return _editor->wrapMode() != QsciScintilla::WrapNone; }

void ScintillaEditor::setMarkerLine(int line) { _editor->setMarkerBackgroundColor(Qt::yellow, line); }
void ScintillaEditor::clearMarkerLine() { }

void ScintillaEditor::findNext(const QString& text, FindOption options) {
    findNext(text, options, true);
}

void ScintillaEditor::findPrevious(const QString& text, FindOption options) {
    findNext(text, options, false);
}

void ScintillaEditor::findNext(const QString& text, FindOption options, bool forward) {
    if (text.isEmpty()) return;
    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;

    // Save cursor so we can detect wrapping
    int origLine = 0, origCol = 0;
    _editor->getCursorPosition(&origLine, &origCol);

    bool found = _editor->findFirst(text, re, cs, wo, true /*wrap*/,
                                    forward /*forward*/, -1, -1,
                                    true /*show*/, false /*posix*/, false /*cxx11*/);

    if (found) {
        int newLine = 0, newCol = 0;
        _editor->getCursorPosition(&newLine, &newCol);
        Q_UNUSED(origLine);
        Q_UNUSED(origCol);
        Q_UNUSED(newLine);
        Q_UNUSED(newCol);
    }
}

int ScintillaEditor::countMatches(const QString& text, FindOption options) {
    if (text.isEmpty()) return 0;
    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;

    int count = 0;
    int startLine = 0, startCol = 0;

    // Save cursor position
    _editor->getCursorPosition(&startLine, &startCol);

    // Search from the beginning of the document
    bool found = _editor->findFirst(text, re, cs, wo, true /*wrap*/,
                                    true /*forward*/, -1, -1,
                                    false /*no-show*/, false, false);
    while (found) {
        ++count;
        found = _editor->findFirst(text, re, cs, wo, true, true, -1, -1, false, false, false);
        if (count > 100000) break;  // Sanity limit
    }

    // Restore cursor
    _editor->setCursorPosition(startLine, startCol);

    return count;
}

int ScintillaEditor::replaceAll(const QString& find, const QString& rep, FindOption options) {
    if (find.isEmpty()) return 0;

    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;
    bool isRegex = re;

    if (isRegex) {
        // Use the SCI_TARGET approach for regex with backreference support.
        // This mirrors NPP's processRange → replaceTargetRegExMode pattern.
        int docLength = _editor->length();

        // Build search flags
        int flags = 0;
        if (cs) flags |= 0x04;        // SCFIND_MATCHCASE
        if (wo) flags |= 0x02;        // SCFIND_WHOLEWORD
        if (re) {
            flags |= 0x00200000;       // SCFIND_REGEXP
            flags |= 0x00020000;       // SCFIND_REGEXP_SKIPCRLFASONE
        }

        _editor->SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, flags);
        _editor->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);

        int replacedCount = 0;
        int targetStart = 0;
        int targetEnd = docLength;
        QByteArray findBa = find.toUtf8();
        QByteArray repBa = rep.toUtf8();

        while (targetStart < docLength) {
            _editor->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, targetStart);
            _editor->SendScintilla(QsciScintilla::SCI_SETTARGETEND, targetEnd);

            int foundPos = _editor->SendScintilla(
                QsciScintilla::SCI_SEARCHINTARGET,
                findBa.size(), findBa.constData());

            if (foundPos < 0) break;

            int foundEnd = _editor->SendScintilla(QsciScintilla::SCI_GETTARGETEND);
            Q_UNUSED(foundEnd);

            // SCI_REPLACETARGETRE expands backreferences (\1, \2, ...) in the
            // replacement string using matched groups from the target.
            int newLen = _editor->SendScintilla(
                QsciScintilla::SCI_REPLACETARGETRE,
                repBa.size(), repBa.constData());
            Q_UNUSED(newLen);

            ++replacedCount;

            // Advance past the replaced text
            targetStart = foundPos + newLen;
        }

        _editor->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);
        emit replaceAllDone(replacedCount);
        return replacedCount;
    } else {
        // Plain text replace-all: use findFirst loop
        int count = 0;
        bool found = _editor->findFirst(find, false, cs, wo, true, true,
                                        -1, -1, false, false, false);
        while (found) {
            _editor->replace(rep);
            ++count;
            found = _editor->findFirst(find, false, cs, wo, true, true,
                                       -1, -1, false, false, false);
        }
        emit replaceAllDone(count);
        return count;
    }
}

void ScintillaEditor::replace(const QString& replacement) {
    _editor->replace(replacement);
}

int ScintillaEditor::markAllMatches(const QString& text, FindOption options) {
    if (text.isEmpty()) return 0;

    // Ensure indicator 1 is configured
    _editor->indicatorDefine(QsciScintilla::BoxIndicator, 1);
    _editor->setIndicatorForegroundColor(QColor("#FFFF00"), 1);

    // Clear all existing indicator-1 highlights from entire document
    QsciScintilla* sci = _editor;
    sci->clearIndicatorRange(0, 0, sci->lines(), 0, 1);

    int count = 0;
    int startLine = 0, startCol = 0;
    _editor->getCursorPosition(&startLine, &startCol);

    bool cs = (options & FindOption::MatchCase) != FindOption::None;
    bool wo = (options & FindOption::WholeWord) != FindOption::None;
    bool re = (options & FindOption::Regex) != FindOption::None;

    // Start search from beginning (pos 0)
    bool found = _editor->findFirst(text, re, cs, wo, true /*wrap*/,
                                    true /*forward*/, -1, -1,
                                    false /*no-show*/, false, false);
    while (found) {
        int sl = 0, si = 0, el = 0, ei = 0;
        _editor->getSelection(&sl, &si, &el, &ei);
        if (sl >= 0 && (sl < el || (sl == el && si < ei))) {
            _editor->fillIndicatorRange(sl, si, el, ei - si, 1);
            ++count;
        }

        // Advance to next match
        found = _editor->findFirst(text, re, cs, wo, true, true, -1, -1,
                                   false, false, false);

        // Guard: if we've wrapped back to or before our start, stop
        int cl = 0, ci = 0;
        _editor->getCursorPosition(&cl, &ci);
        if (cl < startLine || (cl == startLine && ci <= startCol)) {
            if (count > 0) break;  // We got at least one wrap
        }
        if (count > 100000) break;  // Sanity limit
    }

    // Restore cursor
    _editor->setCursorPosition(startLine, startCol);

    return count;
}

void ScintillaEditor::clearSelection() {
    int line = 0, col = 0;
    _editor->getCursorPosition(&line, &col);
    _editor->setSelection(line, col, line, col);
}

void ScintillaEditor::ensureLineVisible(int line) {
    _editor->ensureLineVisible(line);
}

void ScintillaEditor::getCursorPosition(int* line, int* col) const {
    _editor->getCursorPosition(line, col);
}

void ScintillaEditor::clearIndicatorRange(int startLine, int startCol,
                                          int endLine, int endCol, int indicator) {
    _editor->clearIndicatorRange(startLine, startCol, endLine, endCol, indicator);
}

void ScintillaEditor::setAnnotationsEnabled(bool) { }
void ScintillaEditor::addAnnotation(int, const QString&) { }
void ScintillaEditor::clearAnnotations() { }

void ScintillaEditor::setAutoIndent(bool enabled) { _editor->setAutoIndent(enabled); }
void ScintillaEditor::setBackspaceUnindents(bool) { }
void ScintillaEditor::setTabIndents(bool) { }
void ScintillaEditor::setRectangularSelectionEnabled(bool) { }
void ScintillaEditor::setMultipleSelectionEnabled(bool) { }

void ScintillaEditor::setColumnSelectionMode(bool on) {
    if (on) {
        _editor->setSelectionMode(QsciScintilla::SelectionMode::ColumnSelection);
        _editor->setVirtualSpaceOptions(QsciScintilla::VirtualSpace::UserAccessible);
    } else {
        _editor->setSelectionMode(QsciScintilla::SelectionMode::NormalSelection);
        _editor->setVirtualSpaceOptions(QsciScintilla::VirtualSpace::NoWrap);
    }
}

void ScintillaEditor::convertEol(EolType eol) {
    setEolType(eol);
}
