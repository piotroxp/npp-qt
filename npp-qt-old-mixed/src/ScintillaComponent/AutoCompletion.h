// npp-qt: AutoCompletion header
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/AutoCompletion.h
// Target: npp-qt/src/ScintillaComponent/AutoCompletion.h

#pragma once

#include <string>
#include <vector>
#include "FunctionCallTip.h"
#include "ScintillaComponent.h"

// Matched character tracking for auto-paired delimiters
struct MatchedCharInserted {
	MatchedCharInserted() = delete;
	char _c;
	size_t _pos;
	MatchedCharInserted(char c, size_t pos) : _c(c), _pos(pos) {}
};

class InsertedMatchedChars {
public:
	void init(ScintillaComponent* pEditView) { _pEditView = pEditView; }
	void removeInvalidElements(MatchedCharInserted mci);
	void add(MatchedCharInserted mci);
	bool isEmpty() const { return _insertedMatchedChars.size() == 0; }
	intptr_t search(char startChar, char endChar, size_t posToDetect);

private:
	std::vector<MatchedCharInserted> _insertedMatchedChars;
	ScintillaComponent* _pEditView = nullptr;
};

class AutoCompletion {
public:
	explicit AutoCompletion(ScintillaComponent* pEditView)
		: _pEditView(pEditView), _funcCalltip(pEditView) {
		_insertedMatchedChars.init(_pEditView);
	}

	~AutoCompletion() {
		delete _pXmlFile;
	}

	enum class AutocompleteColorIndex {
		autocompleteText,
		autocompleteBg,
		selectedText,
		selectedBg,
		calltipBg,
		calltipText,
		calltipHighlight
	};

	bool setLanguage(LangType language);

	// AutoComplete from the list
	bool showApiComplete();
	// WordCompletion from the current file
	bool showWordComplete(bool autoInsert);
	// AutoComplete from both the list and the current file
	bool showApiAndWordComplete();
	// Parameter display from the list
	bool showFunctionComplete();
	// Autocomplete from path.
	void showPathCompletion();

	void insertMatchedChars(int character, const MatchedPairConf& matchedPairConf);
	void update(int character);
	void callTipClick(size_t direction);
	void getCloseTag(char* closeTag, size_t closeTagLen, size_t caretPos, bool isHTML) const;

	static void setColour(QRgb colour2Set, AutocompleteColorIndex i);
	static void drawAutocomplete(const ScintillaComponent* pEditView);

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
	ScintillaComponent* _pEditView = nullptr;
	LangType _curLang = LangType::L_TEXT;
	NppXml::Document _pXmlFile = nullptr;
	NppXml::Element _xmlKeyword{};
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

	// Type of autocomplete function
	enum AutocompleteType {
		autocFunc,
		autocFuncAndWord,
		autocFuncBrief,
		autocWord
	};

	// AutoComplete code merged into 1 function
	bool showAutoComplete(AutocompleteType autocType, bool autoInsert);
};
