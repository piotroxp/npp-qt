// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include "ScintillaComponent.h"

// =============================================================================
// MatchedCharInserted — tracks a single auto-inserted matched character
// =============================================================================
struct MatchedCharInserted {
    MatchedCharInserted() = delete;
    char _c;
    size_t _pos;
    MatchedCharInserted(char c, size_t pos) : _c(c), _pos(pos) {}
};

// =============================================================================
// InsertedMatchedChars — manages the stack of auto-inserted matched characters
// =============================================================================
class InsertedMatchedChars {
public:
    void init(ScintillaComponent* pEditView) { _pEditView = pEditView; }
    void removeInvalidElements(MatchedCharInserted mci);
    void add(MatchedCharInserted mci);
    bool isEmpty() const { return _insertedMatchedChars.empty(); }
    intptr_t search(char startChar, char endChar, size_t posToDetect);

private:
    std::vector<MatchedCharInserted> _insertedMatchedChars;
    ScintillaComponent* _pEditView = nullptr;
};

// =============================================================================
// MatchedPairConf — configuration for which matched pairs are enabled
// =============================================================================
struct MatchedPairConf {
    bool _doParentheses = true;
    bool _doBrackets = true;
    bool _doCurlyBrackets = true;
    bool _doQuotes = true;
    bool _doDoubleQuotes = true;
    bool _doHtmlXmlTag = true;
    // User-defined matched pairs (e.g., `<` → `>`, `( → )`)
    QMap<QChar, QChar> _matchedPairs;
};

// =============================================================================
// AutoCompletion — manages word, API, function, and path completion + matched chars
// =============================================================================
class AutoCompletion : public QObject
{
    Q_OBJECT
public:
    // Colour index for autocomplete/UI element theming
    enum class AutocompleteColorIndex {
        autocompleteText,
        autocompleteBg,
        selectedText,
        selectedBg,
        calltipBg,
        calltipText,
        calltipHighlight
    };

    // Autocomplete dispatch type
    enum AutocompleteType {
        autocFunc      = 0,  // API function completion
        autocFuncAndWord = 1, // API + word
        autocFuncBrief = 2,   // Brief (showApiComplete)
        autocWord      = 3   // Word only
    };

    explicit AutoCompletion(ScintillaComponent* pEditView);
    ~AutoCompletion() override;

    // Language setup — loads API keywords for the language
    bool setLanguage(int language);

    // --- Public entry points ---
    // Show autocomplete from the API/keyword list
    bool showApiComplete();
    // Show autocomplete from the current document's words
    bool showWordComplete(bool autoInsert);
    // Show autocomplete from both API and current document
    bool showApiAndWordComplete();
    // Show function calltip (parameter hints)
    bool showFunctionComplete();
    // Show path/file completion
    void showPathCompletion();

    // --- Matched character (auto-close bracket) handling ---
    // Insert matching character when user types an opening bracket/quote.
    // matchedPairConf: bitmask flags + optional user-defined pairs.
    void insertMatchedChars(int character, const MatchedPairConf& matchedPairConf);

    // --- Called on each character typed ---
    void update(int character);

    // --- Calltip navigation ---
    void callTipClick(size_t direction);

    // --- HTML/XML auto-close tag ---
    // Writes the closing tag for the current HTML/XML open tag into closeTag.
    void getCloseTag(char* closeTag, size_t closeTagLen, size_t caretPos, bool isHTML) const;

    // --- Colour/theming ---
    static void setColour(int colour, AutocompleteColorIndex i);
    static void drawAutocomplete(const ScintillaComponent* pEditView);

private:
    // Core autocomplete dispatcher
    bool showAutoComplete(AutocompleteType autocType, bool autoInsert);

    // Extract words from the document matching a prefix
    void getWordArray(std::vector<std::string>& wordArray,
                      const char* beginChars,
                      const char* excludeChars) const;

    // Private data
    bool _funcCompletionActive = false;
    ScintillaComponent* _pEditView = nullptr;
    int _curLang = 0;
    bool _ignoreCase = true;

    std::vector<std::string> _keyWordArray;  // cached keywords from API
    std::string _keyWords;                     // space-joined keywords string
    size_t _keyWordMaxLen = 0;

    InsertedMatchedChars _insertedMatchedChars;

    // Static colour defaults
    static int _autocompleteBg;
    static int _autocompleteText;
    static int _selectedBg;
    static int _selectedText;
    static int _calltipBg;
    static int _calltipText;
    static int _calltipHighlight;
};
