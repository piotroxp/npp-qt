// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
// FunctionCallTip: displays function parameter hints while typing
// Win32→Qt6: CallTip from Scintilla → Qt signal/slot + Scintilla SCI_CALLTIP
#include <memory>
#include <string>
#include <vector>
#include "ScintillaComponent.h"

using stringVec = std::vector<const char*>;

class FunctionCallTip
{
public:
    explicit FunctionCallTip(ScintillaComponent* pEditView);
    ~FunctionCallTip() = default;

    // Set XML keyword node from current language
    void setLanguageXML(void* xmlKeyword);

    // Update calltip based on character typed (ch) or explicit show (needShown=true)
    bool updateCalltip(int ch, bool needShown = false);

    // Navigate overloads
    void showNextOverload();
    void showPrevOverload();

    // Visibility check
    bool isVisible() const;

    // Hide calltip
    void close();

private:
    ScintillaComponent* _pEditView = nullptr;
    intptr_t _curPos = 0;
    intptr_t _startPos = 0;
    std::string _funcName;
    stringVec _retVals;
    std::vector<stringVec> _overloads;
    stringVec _descriptions;
    size_t _currentNbOverloads = 0;
    size_t _currentOverload = 0;
    size_t _currentParam = 0;
    char _start = '(';
    char _stop = ')';
    char _param = ',';
    std::string _additionalWordChar;
    bool _ignoreCase = true;
    bool _selfActivated = false;

    bool getCursorFunction();
    bool loadFunction();
    void showCalltip();
    void reset();
    void cleanup();

    static bool isBasicWordChar(char ch) {
        return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
    }
    bool isAdditionalWordChar(char ch) const {
        return _additionalWordChar.find(ch) != std::string::npos;
    }
};
