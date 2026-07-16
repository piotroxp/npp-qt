// ScintillaCtrls.cpp - Scintilla control helpers / utility functions
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ScintillaCtrls.h"
#include "ScintillaEditor.h"
#include <QApplication>
#include <QClipboard>
#include <Qsci/qsciscintilla.h>
#include <QMenu>
#include <QAction>
#include <QScrollBar>
#include <QPointer>
#include <QRegularExpression>
#include <QMetaMethod>
#include <QDebug>

// ============================================================================
// Internal globals for scroll sync
// ============================================================================
static QPointer<QsciScintilla> _syncPrimary;
static QPointer<QsciScintilla> _syncSecondary;
static bool _syncing = false;

// Helper: get full text of a line (including trailing newline)
static QString _getLineText(QsciScintilla* qsci, int line) {
    int start = qsci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line);
    int end   = qsci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line);
    if (end <= start) return QString();
    return qsci->text(start, end);
}

// ============================================================================
// wrapSelection — wrap selected text with before/after strings
// ============================================================================

void ScintillaCtrls::wrapSelection(ScintillaEditor* editor,
                                    const QString& before,
                                    const QString& after) {
    if (!editor) return;
    QsciScintilla* qsci = editor->qsciEditor();
    if (!qsci) return;

    QString selected = qsci->selectedText();
    if (selected.isEmpty()) {
        // No selection — insert wrapper pair and position cursor between
        qsci->insert(before + after);
        int curLine, curCol;
        qsci->getCursorPosition(&curLine, &curCol);
        int newCol = qMax(0, curCol - after.length());
        qsci->setCursorPosition(curLine, newCol);
    } else {
        qsci->replaceSelectedText(before + selected + after);
    }
}

// ============================================================================
// indentBlock — indent (or dedent) selected lines by N spaces
// ============================================================================

void ScintillaCtrls::indentBlock(ScintillaEditor* editor, int spaces) {
    if (!editor) return;
    QsciScintilla* qsci = editor->qsciEditor();
    if (!qsci) return;

    int startLine = 0, startCol = 0, endLine = 0, endCol = 0;
    qsci->getSelection(&startLine, &startCol, &endLine, &endCol);
    if (startLine < 0) {
        qsci->getCursorPosition(&startLine, &startCol);
        endLine = startLine;
        endCol = startCol;
    }

    qsci->beginUndoAction();

    if (spaces >= 0) {
        // Indent: prepend spaces to each line
        QString indent = QString(spaces, ' ');
        for (int line = startLine; line <= endLine; ++line)
            qsci->insertAt(indent, line, 0);
    } else {
        // Dedent: remove up to |spaces| spaces/tabs from each line
        int remove = -spaces;
        for (int line = startLine; line <= endLine; ++line) {
            int lineLen = qsci->lineLength(line);
            int removed = 0;
            while (removed < remove && removed < lineLen) {
                QString ch = qsci->text(
                    qsci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line) + removed, 1);
                if (ch == " " || ch == "\t") {
                    ++removed;
                } else {
                    break;
                }
            }
            if (removed > 0) {
                int posStart = qsci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line);
                qsci->SendScintilla(QsciScintilla::SCI_SETSELECTION, posStart, posStart + removed);
                qsci->removeSelectedText();
            }
        }
    }

    qsci->endUndoAction();
}

// ============================================================================
// duplicateLine — duplicate current line or selection below
// ============================================================================

void ScintillaCtrls::duplicateLine(ScintillaEditor* editor) {
    if (!editor) return;
    QsciScintilla* qsci = editor->qsciEditor();
    if (!qsci) return;

    int startLine = 0, startCol = 0, endLine = 0, endCol = 0;
    qsci->getSelection(&startLine, &startCol, &endLine, &endCol);
    if (startLine < 0) {
        qsci->getCursorPosition(&startLine, &startCol);
        endLine = startLine;
        endCol = startCol;
    }

    qsci->beginUndoAction();

    if (startLine == endLine && startCol == endCol) {
        // No selection — duplicate the current line
        QString lineText = _getLineText(qsci, startLine);
        qsci->insertAt(lineText, startLine + 1, 0);
    } else {
        // Duplicate selected lines
        QString block;
        for (int line = startLine; line <= endLine; ++line) {
            if (!block.isEmpty())
                block += "\n";
            block += _getLineText(qsci, line);
        }
        qsci->insertAt(block, endLine + 1, 0);
    }

    qsci->endUndoAction();
}

// ============================================================================
// joinLines — merge selected lines into one
// ============================================================================

void ScintillaCtrls::joinLines(ScintillaEditor* editor) {
    if (!editor) return;
    QsciScintilla* qsci = editor->qsciEditor();
    if (!qsci) return;

    int startLine = 0, startCol = 0, endLine = 0, endCol = 0;
    qsci->getSelection(&startLine, &startCol, &endLine, &endCol);
    if (startLine < 0) return;
    if (startLine == endLine) return;  // Nothing to join

    int startPos = qsci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, startLine);
    int endPos   = qsci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, endLine);

    QString replacement;
    for (int line = startLine; line <= endLine; ++line) {
        int sp  = qsci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line);
        int ep  = qsci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line);
        int len = ep - sp;
        if (ep > sp) {
            QString lineText = qsci->text(sp, ep).trimmed();
            if (!replacement.isEmpty())
                replacement += " ";
            replacement += lineText;
        }
    }

    qsci->beginUndoAction();
    qsci->SendScintilla(static_cast<unsigned long>(QsciScintilla::SCI_SETSELECTION), static_cast<unsigned long>(startPos), static_cast<long>(endPos));
    qsci->replaceSelectedText(replacement);
    qsci->endUndoAction();
}

// ============================================================================
// moveLines — move selected block up or down by one line
// ============================================================================

void ScintillaCtrls::moveLines(ScintillaEditor* editor, bool up) {
    if (!editor) return;
    QsciScintilla* qsci = editor->qsciEditor();
    if (!qsci) return;

    int startLine = 0, startCol = 0, endLine = 0, endCol = 0;
    qsci->getSelection(&startLine, &startCol, &endLine, &endCol);
    if (startLine < 0) {
        qsci->getCursorPosition(&startLine, &startCol);
        endLine = startLine;
        endCol = startCol;
    }

    int lineCount = qsci->lines();

    if (up) {
        if (startLine == 0) return;
        _moveBlockUp(qsci, startLine, endLine);
    } else {
        if (endLine >= lineCount - 1) return;
        _moveBlockDown(qsci, startLine, endLine);
    }
}

void ScintillaCtrls::_moveBlockUp(QsciScintilla* qsci, int startLine, int endLine) {
    QStringList block;
    for (int line = startLine; line <= endLine; ++line)
        block.append(_getLineText(qsci, line));

    QString targetText = _getLineText(qsci, startLine - 1);

    qsci->beginUndoAction();

    int blockStartPos = qsci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, startLine);
    int afterEndLine  = qMin(endLine + 1, qsci->lines() - 1);
    int blockEndPos   = qsci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, afterEndLine);

    bool hasNext = (startLine <= qsci->lines() - 1);

    QString replacement = targetText;
    if (hasNext)
        replacement += "\n";
    replacement += block.join("\n");
    if (hasNext)
        replacement += "\n";

    qsci->SendScintilla(QsciScintilla::SCI_SETSELECTION, blockStartPos, blockEndPos);
    qsci->replaceSelectedText(replacement);

    qsci->endUndoAction();
}

void ScintillaCtrls::_moveBlockDown(QsciScintilla* qsci, int startLine, int endLine) {
    QStringList block;
    for (int line = startLine; line <= endLine; ++line)
        block.append(_getLineText(qsci, line));

    QString targetText = _getLineText(qsci, endLine + 1);

    qsci->beginUndoAction();

    int blockStartPos = qsci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, startLine);
    int afterEndLine  = qMin(endLine + 1, qsci->lines() - 1);
    int blockEndPos   = qsci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, afterEndLine);

    QString replacement = block.join("\n") + "\n" + targetText;
    if (endLine < qsci->lines() - 1)
        replacement += "\n";

    qsci->SendScintilla(QsciScintilla::SCI_SETSELECTION, blockStartPos, blockEndPos);
    qsci->replaceSelectedText(replacement);

    qsci->endUndoAction();
}

// ============================================================================
// deleteLine — delete a specific line
// ============================================================================

void ScintillaCtrls::deleteLine(ScintillaEditor* editor, int line) {
    if (!editor) return;
    QsciScintilla* qsci = editor->qsciEditor();
    if (!qsci) return;

    int lineCount = qsci->lines();
    if (line < 0 || line >= lineCount) return;

    int lineStartPos = qsci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line);
    int deleteEndPos = qsci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line);
    if (line < lineCount - 1)
        deleteEndPos = qsci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line + 1);

    qsci->beginUndoAction();
    qsci->SendScintilla(QsciScintilla::SCI_SETSELECTION, lineStartPos, deleteEndPos);
    qsci->removeSelectedText();
    qsci->endUndoAction();
}

// ============================================================================
// changeCase — change case of selected text
// ============================================================================

void ScintillaCtrls::changeCase(ScintillaEditor* editor, CaseMode mode) {
    if (!editor) return;
    QsciScintilla* qsci = editor->qsciEditor();
    if (!qsci) return;

    QString selected = qsci->selectedText();
    if (selected.isEmpty()) return;

    switch (mode) {
    case CaseMode::Upper:
        selected = selected.toUpper();
        break;
    case CaseMode::Lower:
        selected = selected.toLower();
        break;
    case CaseMode::Title: {
        QStringList words = selected.split(' ');
        for (QString& word : words) {
            if (!word.isEmpty())
                word[0] = word[0].toUpper();
        }
        selected = words.join(' ');
        break;
    }
    }

    qsci->replaceSelectedText(selected);
}

// ============================================================================
// send — raw SCI message sender
// ============================================================================

intptr_t ScintillaCtrls::send(ScintillaEditor* editor, int msg,
                               int wParam, intptr_t lParam) {
    if (!editor) return 0;
    return editor->send(msg, wParam, lParam);
}

// ============================================================================
// Multiple view / scroll synchronization
// ============================================================================

QsciScintilla* ScintillaCtrls::_syncTarget(QsciScintilla* primary) {
    if (_syncPrimary == primary) return _syncSecondary;
    return nullptr;
}

void ScintillaCtrls::_syncScrollCallback(int, int, int, int, void*) {
    if (_syncing) return;
    if (!_syncPrimary || !_syncSecondary) return;

    // Get scroll position from primary
    double fraction = scrollPosition(_syncPrimary);
    _syncing = true;
    setScrollPosition(_syncSecondary, fraction);
    _syncing = false;
}

void ScintillaCtrls::syncScrolling(QsciScintilla* primary, QsciScintilla* secondary) {
    if (!primary || !secondary) return;
    _syncPrimary = primary;
    _syncSecondary = secondary;

    // Connect primary's scrollChanged to our callback using a lambda that can
    // survive the static context.  We use a small QObject wrapper approach.
    // For simplicity, install an event filter on the primary scrollbar.
    // Use scrollbar valueChanged instead of the non-existent updateRequest signal
    if (auto* sb = primary->verticalScrollBar()) {
        QObject::connect(sb, &QScrollBar::valueChanged, primary, [](int) {
            if (_syncing || !_syncPrimary || !_syncSecondary) return;
            double fraction = scrollPosition(_syncPrimary);
            _syncing = true;
            setScrollPosition(_syncSecondary, fraction);
            _syncing = false;
        });
    }
}

void ScintillaCtrls::unsyncScrolling(QsciScintilla* primary) {
    if (!primary) return;
    if (_syncPrimary == primary) {
        _syncPrimary = nullptr;
        _syncSecondary = nullptr;
    }
}

double ScintillaCtrls::scrollPosition(QsciScintilla* sci) {
    if (!sci) return 0.0;
    QScrollBar* sb = sci->verticalScrollBar();
    if (!sb || sb->maximum() == 0) return 0.0;
    return static_cast<double>(sb->value()) / sb->maximum();
}

void ScintillaCtrls::setScrollPosition(QsciScintilla* sci, double fraction) {
    if (!sci) return;
    QScrollBar* sb = sci->verticalScrollBar();
    if (!sb) return;
    fraction = qBound(0.0, fraction, 1.0);
    sb->setValue(static_cast<int>(fraction * sb->maximum()));
}

double ScintillaCtrls::hScrollPosition(QsciScintilla* sci) {
    if (!sci) return 0.0;
    QScrollBar* sb = sci->horizontalScrollBar();
    if (!sb || sb->maximum() == 0) return 0.0;
    return static_cast<double>(sb->value()) / sb->maximum();
}

void ScintillaCtrls::setHScrollPosition(QsciScintilla* sci, double fraction) {
    if (!sci) return;
    QScrollBar* sb = sci->horizontalScrollBar();
    if (!sb) return;
    fraction = qBound(0.0, fraction, 1.0);
    sb->setValue(static_cast<int>(fraction * sb->maximum()));
}

// ============================================================================
// Indicators
// ============================================================================

void ScintillaCtrls::defineIndicator(QsciScintilla* sci, int indicator,
                                      QsciScintilla::IndicatorStyle style,
                                      const QColor& color) {
    if (!sci || indicator < 0 || indicator > 31) return;
    sci->indicatorDefine(style, indicator);
    sci->setIndicatorForegroundColor(color, indicator);
}

void ScintillaCtrls::applyStrikeIndicator(QsciScintilla* sci, int line,
                                         int startCol, int endCol) {
    if (!sci) return;
    sci->fillIndicatorRange(line, startCol, line, endCol, INDI_STRIKE);
}

void ScintillaCtrls::clearStrikeIndicator(QsciScintilla* sci, int line,
                                          int startCol, int endCol) {
    if (!sci) return;
    sci->clearIndicatorRange(line, startCol, line, endCol, INDI_STRIKE);
}

// ============================================================================
// Annotations
// ============================================================================

void ScintillaCtrls::addAnnotation(QsciScintilla* sci, int line,
                                    const QString& text, int style) {
    if (!sci || line < 0 || line >= sci->lines()) return;
    // SCI_ANNOTATIONSETTEXT = 2222, SCI_ANNOTATIONSETSTYLE = 2223
    // SCI_ANNOTATIONSETSTYLES = 2224
    QByteArray utf8 = text.toUtf8();
    sci->SendScintilla(2222, static_cast<unsigned long>(line), utf8.constData()); // SCI_ANNOTATIONSETTEXT
    sci->SendScintilla(2223, static_cast<unsigned long>(line), static_cast<unsigned long>(style)); // SCI_ANNOTATIONSETSTYLE
    // Show the annotation margin (margin 4)
    sci->SendScintilla(2226, 1); // SCI_ANNOTATIONSETVISIBLE = 2226, 1=Standard
    // Auto-size the margin
    sci->setMarginWidth(4, 0);
    Q_UNUSED(style);
}

void ScintillaCtrls::clearAnnotations(QsciScintilla* sci) {
    if (!sci) return;
    // SCI_ANNOTATIONCLEARALL = 2225
    sci->SendScintilla(2225, 0, static_cast<long>(0));
}

void ScintillaCtrls::clearAnnotation(QsciScintilla* sci, int line) {
    if (!sci || line < 0) return;
    sci->SendScintilla(2222, static_cast<unsigned long>(line), static_cast<const char*>(nullptr)); // SCI_ANNOTATIONSETTEXT with null clears
}

// ============================================================================
// Margin click handling
// ============================================================================

void ScintillaCtrls::setMarginSensitive(QsciScintilla* sci, int margin,
                                         bool sensitive) {
    if (!sci) return;
    sci->setMarginSensitivity(margin, sensitive);
}

void ScintillaCtrls::setMarginWidth(QsciScintilla* sci, int margin,
                                     int widthChars) {
    if (!sci) return;
    sci->setMarginWidth(margin, widthChars);
}

// ============================================================================
// Context menu customization
// ============================================================================

QMenu* ScintillaCtrls::buildContextMenu(ScintillaEditor* editor,
                                         QWidget* parent) {
    QMenu* menu = new QMenu(parent);

    QAction* undoAct = new QAction(QIcon::fromTheme("edit-undo"), QStringLiteral("Undo"), menu);
    QObject::connect(undoAct, &QAction::triggered, editor, &ScintillaEditor::undo);
    menu->addAction(undoAct);

    QAction* redoAct = new QAction(QIcon::fromTheme("edit-redo"), QStringLiteral("Redo"), menu);
    QObject::connect(redoAct, &QAction::triggered, editor, &ScintillaEditor::redo);
    menu->addAction(redoAct);

    menu->addSeparator();

    QAction* cutAct = new QAction(QIcon::fromTheme("edit-cut"), QStringLiteral("Cut"), menu);
    QObject::connect(cutAct, &QAction::triggered, editor, &ScintillaEditor::cut);
    menu->addAction(cutAct);

    QAction* copyAct = new QAction(QIcon::fromTheme("edit-copy"), QStringLiteral("Copy"), menu);
    QObject::connect(copyAct, &QAction::triggered, editor, &ScintillaEditor::copy);
    menu->addAction(copyAct);

    QAction* pasteAct = new QAction(QIcon::fromTheme("edit-paste"), QStringLiteral("Paste"), menu);
    QObject::connect(pasteAct, &QAction::triggered, editor, &ScintillaEditor::paste);
    menu->addAction(pasteAct);

    menu->addSeparator();

    QAction* selectAllAct = new QAction(QStringLiteral("Select All"), menu);
    QObject::connect(selectAllAct, &QAction::triggered, editor, &ScintillaEditor::selectAll);
    menu->addAction(selectAllAct);

    return menu;
}

void ScintillaCtrls::addWrapActionsToMenu(QMenu* menu,
                                          ScintillaEditor* editor) {
    if (!menu || !editor) return;
    QMenu* wrapMenu = menu->addMenu(QStringLiteral("Wrap With"));

    auto wrap = [editor](const QString& b, const QString& a) {
        wrapSelection(editor, b, a);
    };

    QAction* parenAct = wrapMenu->addAction(QStringLiteral("Parentheses  ( )"));
    QObject::connect(parenAct, &QAction::triggered, [wrap]() { wrap("(", ")"); });

    QAction* bracketAct = wrapMenu->addAction(QStringLiteral("Brackets  [ ]"));
    QObject::connect(bracketAct, &QAction::triggered, [wrap]() { wrap("[", "]"); });

    QAction* braceAct = wrapMenu->addAction(QStringLiteral("Braces  { }"));
    QObject::connect(braceAct, &QAction::triggered, [wrap]() { wrap("{", "}"); });

    QAction* quoteAct = wrapMenu->addAction(QStringLiteral("Single Quotes  ' '"));
    QObject::connect(quoteAct, &QAction::triggered, [wrap]() { wrap("'", "'"); });

    QAction* dquoteAct = wrapMenu->addAction(QStringLiteral("Double Quotes  \" \""));
    QObject::connect(dquoteAct, &QAction::triggered, [wrap]() { wrap("\"", "\""); });

    wrapMenu->addSeparator();

    QAction* commentAct = wrapMenu->addAction(QStringLiteral("C-style Comment  /* */"));
    QObject::connect(commentAct, &QAction::triggered, [wrap]() { wrap("/* ", " */"); });

    QAction* hashAct = wrapMenu->addAction(QStringLiteral("Line Comment  //"));
    QObject::connect(hashAct, &QAction::triggered, [wrap]() { wrap("// ", ""); });

    QAction* htmlAct = wrapMenu->addAction(QStringLiteral("HTML Comment  <!-- -->"));
    QObject::connect(htmlAct, &QAction::triggered, [wrap]() { wrap("<!-- ", " -->"); });
}

void ScintillaCtrls::addIndentActionsToMenu(QMenu* menu,
                                             ScintillaEditor* editor) {
    if (!menu || !editor) return;
    QMenu* indentMenu = menu->addMenu(QStringLiteral("Indent"));

    QAction* indentAct = indentMenu->addAction(QStringLiteral("Increase Indent"));
    QObject::connect(indentAct, &QAction::triggered, [editor]() {
        indentBlock(editor, 4);
    });

    QAction* dedentAct = indentMenu->addAction(QStringLiteral("Decrease Indent"));
    QObject::connect(dedentAct, &QAction::triggered, [editor]() {
        indentBlock(editor, -4);
    });
}

// ============================================================================
// Call tips
// ============================================================================

void ScintillaCtrls::showCallTip(QsciScintilla* sci, const QString& tip,
                                  const QString& defn) {
    if (!sci) return;
    QByteArray tipBytes = tip.toUtf8();
    QByteArray defnBytes = defn.toUtf8();
    // SCI_CALLTIPSHOW = 2200
    // lParam = text, wParam = position (0 = at caret)
    sci->SendScintilla(static_cast<unsigned long>(2200), static_cast<unsigned long>(0), tipBytes.constData());
    Q_UNUSED(defnBytes);
}

void ScintillaCtrls::cancelCallTip(QsciScintilla* sci) {
    if (!sci) return;
    // SCI_CALLTIPCANCEL = 2201
    sci->SendScintilla(static_cast<unsigned long>(2201), static_cast<unsigned long>(0), static_cast<long>(0));
}

void ScintillaCtrls::registerCallTipArgs(QsciScintilla* sci,
                                           const QString& functionName,
                                           int argc,
                                           const QStringList& argNames,
                                           const QString& doc) {
    Q_UNUSED(sci);
    Q_UNUSED(functionName);
    Q_UNUSED(argc);
    Q_UNUSED(argNames);
    Q_UNUSED(doc);
    // SCI_CALLTIPSETHLT = 2203 can highlight argument positions.
    // For full support, maintain a per-function map; here we just store doc.
}

// ============================================================================
// Drag and drop text
// ============================================================================

void ScintillaCtrls::setTextDragEnabled(QsciScintilla* sci, bool enabled) {
    if (!sci) return;
    // SCI_SETDRAGANDDROP = 2371
    // mode: 0=drag disabled, 1=drag normal, 2=drag with user drop
    sci->SendScintilla(static_cast<unsigned long>(2371), static_cast<unsigned long>(enabled ? 1 : 0), static_cast<long>(0));
}

bool ScintillaCtrls::isTextDragEnabled(QsciScintilla* sci) {
    if (!sci) return false;
    return sci->SendScintilla(static_cast<unsigned long>(2372), static_cast<unsigned long>(0), static_cast<long>(0)) != 0; // SCI_GETDRAGANDDROP
}

// ============================================================================
// Column / rectangular selection helpers
// ============================================================================

bool ScintillaCtrls::isRectangularSelection(QsciScintilla* sci) {
    if (!sci) return false;
    return sci->SendScintilla(static_cast<unsigned long>(QsciScintilla::SCI_GETSELECTIONMODE), static_cast<unsigned long>(0), static_cast<long>(0))
           == 1; // SC_SEL_RECTANGLE
}

void ScintillaCtrls::startColumnSelection(QsciScintilla* sci,
                                           int startLine, int startCol,
                                           int endLine, int endCol) {
    if (!sci) return;
    sci->SendScintilla(static_cast<unsigned long>(QsciScintilla::SCI_SETSELECTIONMODE), static_cast<unsigned long>(1), static_cast<long>(0)); // SC_SEL_RECTANGLE
    sci->SendScintilla(static_cast<unsigned long>(QsciScintilla::SCI_SETVIRTUALSPACEOPTIONS), static_cast<unsigned long>(1), static_cast<long>(0)); // SCVS_USERACCESSIBLE

    int startPos = sci->SendScintilla(static_cast<unsigned long>(QsciScintilla::SCI_POSITIONFROMLINE), static_cast<unsigned long>(startLine));
    (void)(startCol);
    int endPos = sci->SendScintilla(static_cast<unsigned long>(QsciScintilla::SCI_POSITIONFROMLINE), static_cast<unsigned long>(endLine));
    (void)(endCol);

    sci->SendScintilla(static_cast<unsigned long>(QsciScintilla::SCI_SETSELECTION), static_cast<unsigned long>(startPos), static_cast<long>(endPos));
}

QRect ScintillaCtrls::columnSelectionBounds(QsciScintilla* sci) {
    if (!sci) return QRect();
    long startPos = sci->SendScintilla(QsciScintilla::SCI_GETSELECTIONNSTART);
    long endPos = sci->SendScintilla(QsciScintilla::SCI_GETSELECTIONNEND);
    long startLine = sci->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, static_cast<unsigned long>(startPos));
    long endLine = sci->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, static_cast<unsigned long>(endPos));
    long startLineStart = sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, static_cast<unsigned long>(startLine));
    long endLineStart = sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, static_cast<unsigned long>(endLine));
    return QRect(static_cast<int>(startPos - startLineStart), static_cast<int>(startLine),
                 static_cast<int>(endPos - endLineStart), static_cast<int>(endLine - startLine));
}

// ============================================================================
// Word / identifier operations
// ============================================================================

void ScintillaCtrls::selectWordAtCursor(QsciScintilla* sci) {
    if (!sci) return;
    long pos = sci->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS);
    long start = sci->SendScintilla(QsciScintilla::SCI_WORDSTARTPOSITION, static_cast<unsigned long>(pos));
    long end = sci->SendScintilla(QsciScintilla::SCI_WORDENDPOSITION, static_cast<unsigned long>(pos));
    sci->SendScintilla(QsciScintilla::SCI_SETSELECTION, static_cast<unsigned long>(start), static_cast<unsigned long>(end));
}

QPair<int, int> ScintillaCtrls::wordAtCursor(QsciScintilla* sci) {
    if (!sci) return qMakePair(-1, -1);
    long pos = sci->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS);
    long start = sci->SendScintilla(QsciScintilla::SCI_WORDSTARTPOSITION, static_cast<unsigned long>(pos));
    long end = sci->SendScintilla(QsciScintilla::SCI_WORDENDPOSITION, static_cast<unsigned long>(pos));
    return qMakePair(static_cast<int>(start), static_cast<int>(end));
}

bool ScintillaCtrls::isInComment(QsciScintilla* sci) {
    if (!sci) return false;
    long pos = sci->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS);
    long style = sci->SendScintilla(QsciScintilla::SCI_GETSTYLEAT, static_cast<unsigned long>(pos));
    return style == 1 || style == 2;
}

bool ScintillaCtrls::isInString(QsciScintilla* sci) {
    if (!sci) return false;
    long pos = sci->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS);
    long style = sci->SendScintilla(QsciScintilla::SCI_GETSTYLEAT, static_cast<unsigned long>(pos));
    return style == 5 || style == 6 || style == 7;
}
