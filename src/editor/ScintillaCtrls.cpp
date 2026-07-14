// ScintillaCtrls.cpp - Scintilla control helpers / utility functions
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ScintillaCtrls.h"
#include "ScintillaEditor.h"
#include <QApplication>
#include <QClipboard>
#include <Qsci/qsciscintilla.h>

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
    qsci->SendScintilla(QsciScintilla::SCI_SETSELECTION, startPos, endPos);
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

    // "target line" (startLine-1) ends at end of that line; add \n to bring next line
    int targetEndPos = qsci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, startLine - 1);
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
