// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include <QObject>
#include <QString>
#include <vector>
#include <string>
#include "common/ScintillaComponent.h"

struct MatchedCharInserted {
    MatchedCharInserted() = delete;
    char _c;
    size_t _pos;
    MatchedCharInserted(char c, size_t pos) : _c(c), _pos(pos) {}
};

class InsertedMatchedChars {
public:
    void init(ScintillaComponent* pEditView) { _pEditView = pEditView; }
    void add(MatchedCharInserted mci);
    bool isEmpty() const { return _insertedMatchedChars.empty(); }
    intptr_t search(char startChar, char endChar, size_t posToDetect);
private:
    std::vector<MatchedCharInserted> _insertedMatchedChars;
    ScintillaComponent* _pEditView = nullptr;
};

class AutoCompletion : public QObject
{
    Q_OBJECT
public:
    enum class AutocompleteColorIndex {
        autocompleteText,
        autocompleteBg,
        selectedText,
        selectedBg,
        calltipBg,
        calltipText,
        calltipHighlight
    };

    static void setColour(QRgb colour2Set, AutocompleteColorIndex i);

    explicit AutoCompletion(ScintillaComponent* pEditView);
    ~AutoCompletion() override;

    bool setLanguage(int language);
    bool showApiComplete();
    bool showWordComplete(bool autoInsert);
    bool showApiAndWordComplete();
    bool showFunctionComplete();
    void showPathCompletion();
    void insertMatchedChars(int character, int matchedPairConf);
    void update(int character);
    void callTipClick(size_t direction);
    void getCloseTag(char* closeTag, size_t closeTagLen, size_t caretPos, bool isHTML) const;

    static void setColour(int colour, int colorIndex);
    static void drawAutocomplete(const ScintillaComponent* pEditView);

private:
    bool showAutoComplete(int autocType, bool autoInsert);

    bool _funcCompletionActive = false;
    ScintillaComponent* _pEditView = nullptr;
    int _curLang = 0;
    bool _ignoreCase = true;
    std::vector<std::string> _keyWordArray;
    std::string _keyWords;
    size_t _keyWordMaxLen = 0;
    InsertedMatchedChars _insertedMatchedChars;
};
