// npp-qt: FunctionCallTip header
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/FunctionCallTip.h
// Target: npp-qt/src/ScintillaComponent/FunctionCallTip.h

#pragma once

#include <locale>
#include <memory>
#include <string>
#include <vector>

#include <Scintilla.h>

#include "NppXml.h"
#include "ScintillaComponent.h"

using stringVec = std::vector<const char*>;

class FunctionCallTip {
	friend class AutoCompletion;
public:
	explicit FunctionCallTip(ScintillaComponent* pEditView) : _pEditView(pEditView) {}
	~FunctionCallTip() {}

	void setLanguageXML(NppXml::Element xmlKeyword);
	bool updateCalltip(int ch, bool needShown = false);
	void showNextOverload();
	void showPrevOverload();
	bool isVisible() const {
		return _pEditView ? _pEditView->send(SCI_CALLTIPACTIVE) != 0 : false;
	}
	void close();

public:
	// These are set by AutoCompletion during init
	char _start = '(';
	char _stop = ')';
	char _param = ',';
	char _terminal = ';';
	std::string _additionalWordChar;
	bool _ignoreCase = true;

private:
	ScintillaComponent* _pEditView = nullptr;
	NppXml::Element _xmlKeyword{};

	intptr_t _curPos = 0;
	intptr_t _startPos = 0;

	NppXml::Element _curFunction{};
	std::unique_ptr<char[]> _funcName = nullptr;
	stringVec _retVals;
	std::vector<stringVec> _overloads;
	stringVec _descriptions;
	size_t _currentNbOverloads = 0;
	size_t _currentOverload = 0;
	size_t _currentParam = 0;
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
