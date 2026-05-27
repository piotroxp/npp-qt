// AutoCompletion.h - Qt6 port
#pragma once
#include <QWidget>
#include <Qsci/qsciscintillabase.h>
typedef unsigned int COLORREF;
#include <vector>
#include <string>
#include "FunctionCallTip.h"

class ScintillaEditView;

// Matched character pair support
struct MatchedCharInserted {
    MatchedCharInserted() = delete;
    char _c;
    size_t _pos;
    MatchedCharInserted(char c, size_t pos) : _c(c), _pos(pos) {}
};

struct MatchedPairConf {
    char _matchChar;
    bool _doInsert;
    char _closeChar;
    int _priority;
};

class InsertedMatchedChars {
public:
    void init(ScintillaEditView* pEditView) { _pEditView = pEditView; }
    void removeInvalidElements(MatchedCharInserted mci) { Q_UNUSED(mci); }
    void add(MatchedCharInserted mci) { _insertedMatchedChars.push_back(mci); }
    bool isEmpty() const { return _insertedMatchedChars.empty(); }
    intptr_t search(char startChar, char endChar, size_t posToDetect) { Q_UNUSED(startChar); Q_UNUSED(endChar); Q_UNUSED(posToDetect); return -1; }
private:
    std::vector<MatchedCharInserted> _insertedMatchedChars;
    ScintillaEditView* _pEditView = nullptr;
};

class AutoCompletion {
public:
    explicit AutoCompletion(ScintillaEditView* pEditView) : _pEditView(pEditView), _funcCalltip(pEditView) {
        _insertedMatchedChars.init(_pEditView);
    }
    ~AutoCompletion() {}

    enum class AutocompleteColorIndex {
        autocompleteText, autocompleteBg,
        selectedText, selectedBg,
        calltipBg, calltipText, calltipHighlight
    };

    bool setLanguage(LangType language) { _curLang = language; return true; }
    bool showApiComplete() { return false; }
    bool showWordComplete(bool autoInsert) { Q_UNUSED(autoInsert); return false; }
    bool showApiAndWordComplete() { return false; }
    bool showFunctionComplete() { return false; }
    void showPathCompletion() {}
    void insertMatchedChars(int character, const MatchedPairConf& matchedPairConf) { Q_UNUSED(character); Q_UNUSED(matchedPairConf); }
    void update(int character) { Q_UNUSED(character); }
    void callTipClick(size_t direction) { Q_UNUSED(direction); }
    void getCloseTag(char* closeTag, size_t closeTagLen, size_t caretPos, bool isHTML) const { Q_UNUSED(closeTag); Q_UNUSED(closeTagLen); Q_UNUSED(caretPos); Q_UNUSED(isHTML); }
    static void setColour(QRgb colour2Set, AutocompleteColorIndex i) { Q_UNUSED(colour2Set); Q_UNUSED(i); }
    static void drawAutocomplete(const ScintillaEditView* pEditView) { Q_UNUSED(pEditView); }

protected:
    static QRgb _autocompleteBg;
    static QRgb _autocompleteText;
    static QRgb _selectedBg;
    static QRgb _selectedText;
    static QRgb _calltipBg;
    static QRgb _calltipText;
    static QRgb _calltipHighlight;

private:
    bool _funcCompletionActive = false;
    ScintillaEditView* _pEditView = nullptr;
    LangType _curLang = L_USER;
    void* _pXmlFile = nullptr;
    std::string _xmlKeyword;
    bool _isFxImageRegistered = false;
    bool _isFxImageRegisteredDark = false;
    InsertedMatchedChars _insertedMatchedChars;
    bool _ignoreCase = true;
    std::vector<std::string> _keyWordArray;
    std::string _keyWords;
    size_t _keyWordMaxLen = 0;
    FunctionCallTip _funcCalltip;

    const wchar_t* getApiFileName();
    void getWordArray(std::vector<std::string>& wordArray, const char* beginChars, const char* excludeChars) const;
    enum AutocompleteType { autocFunc, autocFuncAndWord, autocFuncBrief, autocWord };
    bool showAutoComplete(AutocompleteType autocType, bool autoInsert);
};