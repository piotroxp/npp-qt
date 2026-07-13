// ScintillaEditor.h - QScintilla wrapper for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QFrame>
#include <QString>
#include <QColor>
#include <vector>
#include <Qsci/qsciscintilla.h>
#include "../common/Types.h"
#include "common/NonCopyable.h"
#include "../core/ThemeManager.h"

class SyntaxHighlighter;
class Buffer;

class ScintillaEditor : public QFrame {
    Q_OBJECT

public:
    explicit ScintillaEditor(QWidget* parent = nullptr);
    ~ScintillaEditor() override;

    void setText(const QString& text);
    QString text() const;
    void setPlainText(const QString& text);
    QString toPlainText() const;

    void setLanguage(LangType lang);
    LangType language() const { return _language; }

    void setEncoding(EncodingType enc);
    EncodingType encoding() const { return _encoding; }

    void setEolType(EolType eol);
    EolType eolType() const { return _eolType; }

    int currentLine() const;
    int currentColumn() const;
    int lineCount() const;

    void gotoLine(int line, int col = 0);
    void setCursorPosition(int line, int col);

    QString selectedText() const;
    void setSelection(int startLine, int startCol, int endLine, int endCol);
    void selectAll();
    void clearSelection();

    void copy();
    void cut();
    void paste();
    void deleteSelection();

    void undo();
    void redo();
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    void setModified(bool m);
    bool isModified() const;

    void setReadOnly(bool ro);
    bool isReadOnly() const;

    void setFont(const QFont& font);
    QFont font() const;

    void setTabWidth(int width);
    int tabWidth() const;
    void setUseTabs(bool useTabs);
    bool useTabs() const;
    void setIndent(int indent);
    int indent() const;

    void setLineNumberingEnabled(bool enabled);
    bool isLineNumberingEnabled() const;

    void setWhitespaceVisibility(bool visible);

    void setWrapMode(bool wrap);
    bool wrapMode() const;

    void setMarkerLine(int line);
    void clearMarkerLine();

    void findNext(const QString& text, FindOption options);
    void findNext(const QString& text, FindOption options, bool forward);
    void findPrevious(const QString& text, FindOption options);
    int countMatches(const QString& text, FindOption options);
    int replaceAll(const QString& find, const QString& replace, FindOption options);
    void replace(const QString& replacement);
    int markAllMatches(const QString& text, FindOption options);

    // Regex replace-all using SCI_TARGET approach (supports backreferences)
    int replaceAllRegex(const QString& find, const QString& rep, FindOption options);

    void setAnnotationsEnabled(bool enabled);
    void addAnnotation(int line, const QString& text);
    void clearAnnotations();

    // Selection helpers
    void ensureLineVisible(int line);
    void getCursorPosition(int* line, int* col) const;

    // Indicator/selection helpers
    void clearIndicatorRange(int startLine, int startCol, int endLine, int endCol, int indicator);

    void setAutoIndent(bool enabled);
    void setBackspaceUnindents(bool enabled);
    void setTabIndents(bool enabled);
    void setRectangularSelectionEnabled(bool enabled);
    void setMultipleSelectionEnabled(bool enabled);
    void setColumnSelectionMode(bool on);

    void convertEol(EolType eol);

    SyntaxHighlighter* highlighter() const { return _highlighter; }

    // Access underlying QsciScintilla for advanced operations (e.g., document sharing)
    QsciScintilla* qsciEditor() const { return _editor; }

    void setEolVisibility(bool visible);
    void setCaretLineVisibility(bool visible);
    bool isCaretLineVisible() const;
    void setCaretWidth(int pixels);
    int caretWidth() const;
    void setCaretForegroundColor(const QColor& color);
    void setCaretLineBackgroundColor(const QColor& color);

    // Theme application — applies colors from ThemeManager to the editor surface
    void applyTheme(const QString& themeName);
    void applyThemeToLexer(QsciLexer* lexer);

signals:
    void textChanged();
    void cursorPositionChanged(int line, int col);
    void modificationChanged(bool modified);
    void selectionChanged();
    void updateUI();
    void replaceAllDone(int count);

private:
    QsciScintilla* _editor;
    SyntaxHighlighter* _highlighter;
    LangType _language = LangType::L_TEXT;
    EncodingType _encoding = EncodingType::UTF_8;
    EolType _eolType = EolType::EOL_LF;
};
