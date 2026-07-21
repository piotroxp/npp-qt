// MockEditor.cpp — deterministic Scintilla mock for headless testing
// No native dependencies — runs in CI, headless, fast

#include "IEditor.h"
#include <QVector>

// ── MockEditor ─────────────────────────────────────────────────────────────

MockEditor::MockEditor() {
    _sendResponses[2005] = 0;   // SCI_GETLENGTH → 0 (empty buffer)
    _sendResponses[2013] = 0;   // SCI_GETEOLMODE → 0
}

QString MockEditor::text() const { return _text; }

void MockEditor::setText(const QString& text) {
    _text = text;
    _cursorPos = 0;
    _selStart = 0;
    _selEnd = 0;
    _lineCount = text.isEmpty() ? 1 : text.count('\n') + 1;
    emit textChanged();
}

QString MockEditor::selectedText() const {
    if (_selStart >= _text.size() || _selEnd > _text.size() || _selStart >= _selEnd)
        return QString();
    return _text.mid(_selStart, _selEnd - _selStart);
}

int MockEditor::lineCount() const { return _lineCount; }

QString MockEditor::lineText(int line) const {
    if (line < 0 || line >= _lineCount) return QString();
    const QStringList lines = _text.split('\n');
    return lines.value(line);
}

int MockEditor::cursorPosition() const { return _cursorPos; }

void MockEditor::setCursorPosition(int line, int column) {
    Q_UNUSED(line); Q_UNUSED(column);
    // Simple: position = sum of previous lines + column
    int pos = 0;
    const QStringList lines = _text.split('\n');
    for (int i = 0; i < line && i < lines.size(); ++i)
        pos += lines[i].size() + 1;
    pos += column;
    _cursorPos = qMin(pos, _text.size());
    emit cursorMoved(line, column);
}

long long MockEditor::send(int message, long long wParam, long long lParam) {
    _sendHistory.append({message, wParam, lParam});

    // Check custom responses first
    if (_sendResponses.contains(message))
        return _sendResponses[message];

    switch (static_cast<ScintillaMessage>(message)) {
        case ScintillaMessage::GetLength:
            return _text.size();
        case ScintillaMessage::GetTextLength:
            return _text.size();
        case ScintillaMessage::GetLineCount:
            return _lineCount;
        case ScintillaMessage::GetCurrentPos:
            return _cursorPos;
        case ScintillaMessage::GetAnchor:
            return _selStart;
        case ScintillaMessage::GetSelectionStart:
            return qMin(_selStart, _selEnd);
        case ScintillaMessage::GetSelectionEnd:
            return qMax(_selStart, _selEnd);
        case ScintillaMessage::LineFromPosition: {
            int pos = static_cast<int>(wParam);
            const QString prefix = _text.left(pos);
            return prefix.count('\n');
        }
        case ScintillaMessage::GetEOLMode:
            return 0; // CR+LF
        case ScintillaMessage::SetSel:
            _selStart = static_cast<int>(wParam);
            _selEnd = static_cast<int>(lParam);
            return 0;
        case ScintillaMessage::AutoCGetCurrent:
            return 0; // selected index
        case ScintillaMessage::GetCharAt: {
            int pos = static_cast<int>(wParam);
            if (pos < 0 || pos >= _text.size()) return 0;
            return _text[pos].unicode();
        }
        case ScintillaMessage::LineLength: {
            int line = static_cast<int>(wParam);
            if (line < 0 || line >= _lineCount) return 0;
            return lineText(line).size();
        }
        case ScintillaMessage::ReplaceSel: {
            const char* s = reinterpret_cast<const char*>(lParam);
            QString replacement = QString::fromUtf8(s);
            int start = qMin(_selStart, _selEnd);
            int end = qMax(_selStart, _selEnd);
            if (end > _text.size()) end = _text.size();
            _text.replace(start, end - start, replacement);
            emit textChanged();
            return 0;
        }
        default:
            return 0;
    }
}

long long MockEditor::send(const QByteArray& message) {
    if (_sendTextResponses.contains(message))
        return _sendTextResponses[message];
    Q_UNUSED(message);
    return 0;
}

int MockEditor::currentLine() const {
    return _text.left(_cursorPos).count('\n');
}

int MockEditor::currentColumn() const {
    int lastNewline = _text.lastIndexOf('\n', _cursorPos - 1);
    return _cursorPos - lastNewline - 1;
}

bool MockEditor::hasSelection() const { return _selStart != _selEnd; }
int MockEditor::selectionStart() const { return qMin(_selStart, _selEnd); }
int MockEditor::selectionEnd() const { return qMax(_selStart, _selEnd); }

void MockEditor::insertText(const QString& text) {
    _text.insert(_cursorPos, text);
    _cursorPos += text.size();
    emit textChanged();
}

void MockEditor::replaceSelection(const QString& text) {
    send(static_cast<int>(ScintillaMessage::ReplaceSel), 0,
         reinterpret_cast<long long>(text.toUtf8().constData()));
}

void MockEditor::replaceLine(int line, const QString& newText) {
    if (line < 0 || line >= _lineCount) return;
    const QStringList lines = _text.split('\n');
    lines[line] = newText;
    _text = lines.join('\n');
    emit textChanged();
}

void MockEditor::showAutoComplete(const QStringList& items, int selIndex) {
    _autoCompleteItems = items;
    QVariantMap n;
    n["wParam"] = selIndex >= 0 ? selIndex : 0;
    n["text"] = items.join(" ");
    emit notification(n);
}

void MockEditor::showCallTip(const QString& tip) {
    _callTipText = tip;
    emit callTipShown(tip);
    QVariantMap n;
    n["text"] = tip;
    emit notification(n);
}

void MockEditor::cancelAutoComplete() {
    _autoCompleteItems.clear();
    emit autoCancelled();
    QVariantMap n;
    n["cancel"] = true;
    emit notification(n);
}

void MockEditor::cancelCallTip() {
    _callTipText.clear();
    emit callTipCancelled();
    QVariantMap n;
    n["callTipCancel"] = true;
    emit notification(n);
}

QString MockEditor::getWordAtCursor() const {
    int start = _cursorPos;
    while (start > 0 && (_text[start - 1].isLetterOrNumber() || _text[start - 1] == '_'))
        --start;
    int end = _cursorPos;
    while (end < _text.size() && (_text[end].isLetterOrNumber() || _text[end] == '_'))
        ++end;
    if (start >= end) return QString();
    return _text.mid(start, end - start);
}

QString MockEditor::getWordBeforeCursor() const {
    return getWordAtCursor(); // same logic
}

void MockEditor::setLanguage(const QString& lang) { _language = lang; }
QString MockEditor::language() const { return _language; }

QStringList MockEditor::keywords(int set) const {
    return _keywordSets.value(set);
}

void MockEditor::setKeywords(const QStringList& words, int set) {
    _keywordSets[set] = words;
}

QVariantMap MockEditor::state() const {
    QVariantMap s;
    s["text"] = _text;
    s["cursor"] = _cursorPos;
    s["line"] = currentLine();
    s["col"] = currentColumn();
    s["selStart"] = _selStart;
    s["selEnd"] = _selEnd;
    s["lineCount"] = _lineCount;
    s["language"] = _language;
    return s;
}

void MockEditor::setLineCount(int n) { _lineCount = n; }

void MockEditor::setSendResponse(long long msg, long long response) {
    _sendResponses[msg] = response;
}

void MockEditor::setSendResponse(const QByteArray& msg, long long response) {
    _sendTextResponses[msg] = response;
}

void MockEditor::triggerNotification(int notifyCode, long long wParam, long long lParam) {
    QVariantMap n;
    n["code"] = notifyCode;
    n["wParam"] = wParam;
    n["lParam"] = lParam;
    emit notification(n);

    if (notifyCode == 2008) { // SCN_CHARADDED
        emit charAdded(static_cast<int>(wParam));
    }
}

// ── Test fixture: IntegrationTestHarness ─────────────────────────────────────
// Spins up a real-ish Application environment with MockEditor for fast tests.
class IntegrationTestHarness {
public:
    IntegrationTestHarness() : _app(nullptr, 0, nullptr) {}

    // Constructs a fully wired AutoCompletion backed by MockEditor.
    // Pass the path to the AutoCompletion source for pattern checks.
    AutoCompletion* makeAutoCompletion(const QString& sourcePath = {}) {
        auto* mock = new MockEditor();
        _mock = mock;

        // Configure mock for autocomplete testing
        mock->setSendResponse(2005, 10); // SCI_GETLENGTH
        mock->setSendResponse(2006, 10); // SCI_GETTEXTLENGTH
        mock->setSendResponse(2009, 2008); // SCI_GETCURRENTPOS
        mock->setSendResponse(2017, 2010); // SCI_GETANCHOR
        mock->setSendResponse(2019, 0); // SCI_GETEOLMODE
        mock->setSendResponse(2007, 0); // SCI_GETCHARAT
        mock->setSendResponse(2445, 0); // SCI_AUTOCGETCURRENT

        // Keyword sets for language
        mock->setKeywords({"if", "else", "while", "for", "return", "def", "import"}, 0);
        mock->setKeywords({"int", "char", "void", "float", "double"}, 0);
        mock->setLanguage("cpp");

        auto* ac = new AutoCompletion(mock);

        // Also make a real one if you want
        return ac;
    }

    MockEditor* mock() const { return _mock; }

private:
    QCoreApplication _app;
    MockEditor* _mock = nullptr;
};

#endif // MOCKEDITOR_CPP
