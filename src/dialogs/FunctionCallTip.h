// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include <string>
#include <vector>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QStringList>
#include <QObject>
#include "common/ScintillaComponent.h"

using stringVec = std::vector<const char*>;

struct FunctionOverload {
    std::string signature;       // e.g. "void foo(int a, const std::string& b)"
    std::string brief;           // one-line description
    std::string documentation;  // full doc comment
    std::string returnType;
    int paramCount = 0;
};

class FunctionCallTip : public QObject {
    Q_OBJECT

public:
    explicit FunctionCallTip(ScintillaComponent* pEditView, QObject* parent = nullptr);
    ~FunctionCallTip() = default;

    void setLanguageXML(void* xmlKeyword);

    /// Trigger calltip manually (Ctrl+Shift+Space or after opening paren).
    bool updateCalltip(int ch, bool needShown = false);

    /// Navigate overloads.
    void showNextOverload();
    void showPrevOverload();

    /// Force-show calltip for a specific function (manual trigger).
    void showForFunction(const QString& funcName, const std::vector<FunctionOverload>& overloads);

    /// Dismiss the calltip (Escape, or click outside).
    void close();

    /// Check if calltip is currently visible.
    bool isVisible() const;

    /// Whether the calltip stays visible after click (sticky mode).
    bool isSticky() const { return _sticky; }
    void setSticky(bool v) { _sticky = v; }

    /// Highlight a specific parameter index inside the calltip.
    void highlightParam(int paramIndex);

    /// Set the current parameter (from cursor position) to highlight.
    void setCurrentParam(size_t param) { _currentParam = param; }
    size_t currentParam() const { return _currentParam; }

    /// Whether auto-show on '(' is enabled.
    bool autoTrigger() const { return _autoTrigger; }
    void setAutoTrigger(bool v) { _autoTrigger = v; }

    /// Theme colours.
    void setForeground(const QColor& c);
    void setBackground(const QColor& c);
    void setHighlightColor(const QColor& c);

    QColor foreground() const { return _fgColor; }
    QColor background() const { return _bgColor; }
    QColor highlightColor() const { return _hlColor; }

Q_SIGNALS:
    /// Emitted when user dismisses the calltip.
    void dismissed();
    /// Emitted when the highlighted parameter changes.
    void paramHighlightChangedSignal(int paramIndex);

public:
    /// Called when the highlighted parameter changes (for internal chaining).
    void paramHighlightChanged(int paramIndex);
    /// Called when user dismisses the calltip (for internal chaining).
    void onDismissed();

private:
    bool getCursorFunction();
    bool loadFunction();
    void showCalltip();
    void reset();
    void cleanup();
    void applyStyling();
    void updateParamHighlight();

    ScintillaComponent* _pEditView = nullptr;
    intptr_t _curPos = 0;
    intptr_t _startPos = 0;

    std::string _funcName;
    std::string _fullSignature;
    stringVec _retVals;
    std::vector<stringVec> _overloads;
    stringVec _descriptions;
    size_t _currentNbOverloads = 0;
    size_t _currentOverload = 0;
    size_t _currentParam = 0;

    /// All known overloads for the current function.
    std::vector<FunctionOverload> _overloadList;

    char _start = '(';
    char _stop = ')';
    char _param = ',';
    char _terminal = ';';
    std::string _additionalWordChar;
    bool _ignoreCase = true;
    bool _selfActivated = false;

    /// Sticky mode: calltip stays visible on click.
    bool _sticky = false;
    /// Auto-trigger after '(' character.
    bool _autoTrigger = true;
    /// Last calltip position (for position tracking).
    QPoint _lastCalltipPos;

    /// Theme colours.
    QColor _fgColor = QColor(0xFF, 0xFF, 0xFF);
    QColor _bgColor = QColor(0x2A, 0x2A, 0x3A);
    QColor _hlColor = QColor(0x33, 0xAA, 0xFF);

    static bool isBasicWordChar(char ch) {
        return std::isalnum(ch, std::locale::classic()) || ch == '_';
    }

    bool isAdditionalWordChar(char ch) const {
        return _additionalWordChar.find(ch) != std::string::npos;
    }
};
