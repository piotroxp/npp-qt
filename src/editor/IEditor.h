// IEditor.h — Editor interface for dependency injection / mocking
// Used by: AutoCompletion, FunctionCallTip, FindReplace, DocumentMap
// Mock implementation: MockEditor for tests
// Real implementation: ScintillaEditor (wraps QScintilla)

#ifndef IEDITOR_H
#define IEDITOR_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QVariant>

// Low-level Scintilla message types (mirrors SCI_*)
enum class ScintillaMessage {
    CharAdded             = 2008,
    AutoCSelected         = 2021,
    AutoCCancelled        = 2022,
    UpdateUI              = 2002,
    SetSel                = 2062,
    ReplaceSel            = 2010,
    GetTextLength         = 2006,
    GetText               = 2001,
    GetLine               = 2003,
    GetLineCount          = 2153,
    LineFromPosition      = 2167,
    PositionFromLine      = 2033,
    GetSelectionStart     = 2145,
    GetSelectionEnd       = 2146,
    LineLength            = 2134,
    AutoCSetSeparator     = 2088,
    AutoCShow             = 2110,
    AutoCCancel           = 2111,
    AutoCGetCurrent        = 2445,
    CallTipShow           = 2201,
    CallTipCancel         = 2202,
    CallTipSetHL          = 2205,
    GetCurrentPos         = 2009,
    GetAnchor             = 2017,
    GetEOLMode            = 2019,
    GetLength             = 2005,
    GetCharAt             = 2007,
    SetKeywords           = 2009,
    StyleSetFont          = 2052,
    SetLexerLanguage      = 4002,
};

// High-level editor interface — implemented by real ScintillaEditor
// and mocked by MockEditor in tests.
class IEditor : public QObject {
    Q_OBJECT

public:
    virtual ~IEditor() = default;

    // ── Text operations ──────────────────────────────────────────────────
    virtual QString text() const = 0;
    virtual void setText(const QString& text) = 0;
    virtual QString selectedText() const = 0;
    virtual int lineCount() const = 0;
    virtual QString lineText(int line) const = 0;
    virtual int cursorPosition() const = 0;
    virtual void setCursorPosition(int line, int column) = 0;

    // ── Scintilla message passthrough ─────────────────────────────────────
    // Used for: AutoComplete, CallTip, markers, annotations, etc.
    // Mock: records calls and returns configurable responses
    // Real: forwards to QScintilla widget
    virtual long long send(int message, long long wParam = 0, long long lParam = 0) = 0;
    virtual long long send(const QByteArray& message) = 0;

    // ── View state ───────────────────────────────────────────────────────
    virtual int currentLine() const = 0;
    virtual int currentColumn() const = 0;
    virtual bool hasSelection() const = 0;
    virtual int selectionStart() const = 0;
    virtual int selectionEnd() const = 0;

    // ── Content modification ──────────────────────────────────────────────
    virtual void insertText(const QString& text) = 0;
    virtual void replaceSelection(const QString& text) = 0;
    virtual void replaceLine(int line, const QString& newText) = 0;

    // ── Autocomplete / Calltip helpers ───────────────────────────────────
    virtual void showAutoComplete(const QStringList& items, int selIndex = -1) = 0;
    virtual void showCallTip(const QString& tip) = 0;
    virtual void cancelAutoComplete() = 0;
    virtual void cancelCallTip() = 0;
    virtual QString getWordAtCursor() const = 0;
    virtual QString getWordBeforeCursor() const = 0;

    // ── Language / lexer ─────────────────────────────────────────────────
    virtual void setLanguage(const QString& lang) = 0;
    virtual QString language() const = 0;
    virtual QStringList keywords(int set) const = 0;
    virtual void setKeywords(const QStringList& words, int set = 0) = 0;

    // ── Editor state snapshot (for test assertions) ─────────────────────
    virtual QVariantMap state() const = 0;  // text, cursor, selection, language

signals:
    // Emitted for every Scintilla notification
    void notification(const QVariantMap& n);
    // Convenience signals
    void charAdded(int ch);
    void autoCompleted(const QString& word);
    void autoCancelled();
    void cursorMoved(int line, int col);
    void textChanged();
    void callTipShown(const QString& tip);
    void callTipCancelled();
};

// ─────────────────────────────────────────────────────────────────────────────
// MockEditor — fully deterministic, no native dependencies
// Used in: headless tests, CI pipelines, fast feedback loops
// ─────────────────────────────────────────────────────────────────────────────
class MockEditor : public IEditor {
    Q_OBJECT
public:
    MockEditor();

    // IEditor interface
    QString text() const override;
    void setText(const QString& text) override;
    QString selectedText() const override;
    int lineCount() const override;
    QString lineText(int line) const override;
    int cursorPosition() const override;
    void setCursorPosition(int line, int column) override;
    long long send(int message, long long wParam = 0, long long lParam = 0) override;
    long long send(const QByteArray& message) override;
    int currentLine() const override;
    int currentColumn() const override;
    bool hasSelection() const override;
    int selectionStart() const override;
    int selectionEnd() const override;
    void insertText(const QString& text) override;
    void replaceSelection(const QString& text) override;
    void replaceLine(int line, const QString& newText) override;
    void showAutoComplete(const QStringList& items, int selIndex = -1) override;
    void showCallTip(const QString& tip) override;
    void cancelAutoComplete() override;
    void cancelCallTip() override;
    QString getWordAtCursor() const override;
    QString getWordBeforeCursor() const override;
    void setLanguage(const QString& lang) override;
    QString language() const override;
    QStringList keywords(int set) const override;
    void setKeywords(const QStringList& words, int set = 0) override;
    QVariantMap state() const override;

    // ── Test control API ─────────────────────────────────────────────────
    void setLineCount(int n);
    void setSendResponse(long long msg, long long response);
    void setSendResponse(const QByteArray& msg, long long response);
    void setKeywords(const QStringList& words, int set, const QStringList& keywords);
    void triggerNotification(int notifyCode, long long wParam = 0, long long lParam = 0);

    // History of all send() calls — for test assertions
    QVector<std::tuple<int, long long, long long>> sendHistory() const { return _sendHistory; }

private:
    QString _text;
    int _cursorPos = 0;
    int _selStart = 0;
    int _selEnd = 0;
    int _lineCount = 1;
    QString _language;
    QVector<std::tuple<int, long long, long long>> _sendHistory;
    QMap<long long, long long> _sendResponses;
    QMap<QByteArray, long long> _sendTextResponses;
    QStringList _autoCompleteItems;
    QString _callTipText;
    QMap<int, QStringList> _keywordSets;
};

#endif // IEDITOR_H
