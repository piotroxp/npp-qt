// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include <string>
#include <vector>
#include <QString>
#include "common/ScintillaComponent.h"

using stringVec = std::vector<const char*>;

class FunctionCallTip {
public:
    explicit FunctionCallTip(ScintillaComponent* pEditView);
    ~FunctionCallTip() = default;

    void setLanguageXML(void* xmlKeyword);
    bool updateCalltip(int ch, bool needShown = false);
    void showNextOverload();
    void showPrevOverload();
    bool isVisible() const;
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
    char _terminal = ';';
    std::string _additionalWordChar;
    bool _ignoreCase = true;
    bool _selfActivated = false;

    bool getCursorFunction();
    bool loadFunction();
    void showCalltip();
    void reset();
    void cleanup();

    static bool isBasicWordChar(char ch) {
        return std::isalnum(ch, std::locale::classic()) || ch == '_';
    }

    bool isAdditionalWordChar(char ch) const {
        return _additionalWordChar.find(ch) != std::string::npos;
    }
};
