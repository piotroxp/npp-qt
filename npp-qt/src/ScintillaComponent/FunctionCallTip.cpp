// npp-qt: FunctionCallTip implementation
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/FunctionCallTip.cpp
// Target: npp-qt/src/ScintillaComponent/FunctionCallTip.cpp

#include "FunctionCallTip.h"

#include <cstring>
#include <locale>
#include <memory>
#include <sstream>
#include <vector>

#include <Scintilla.h>

#include "Common.h"
#include "NppXml.h"

struct Token {
	Token() = delete;
	explicit Token(char* tok, int len, bool isID) noexcept : token(tok), length(len), isIdentifier(isID) {}
	char* token;
	int length;
	bool isIdentifier;
};

struct FunctionValues {
	int lastIdentifier = -1;
	int lastFunctionIdentifier = -1;
	int param = 0;
	int scopeLevel = -1;
};

// Case-insensitive string comparison (Scintilla-style)
static int testNameNoCase(const char* name1, const char* name2, int len = -1)
{
	if (len == -1)
		len = 1024;

	for (int i = 0; i < len; ++i)
	{
		const char char1 = name1[i];
		const char char2 = name2[i];

		if (char1 == '\0' || char2 == '\0')
			return char1 - char2;

		if (char1 == char2)
			continue;

		if (std::tolower(static_cast<unsigned char>(char1)) != std::tolower(static_cast<unsigned char>(char2)))
			return char1 - char2;
	}
	return 0;
}

void FunctionCallTip::setLanguageXML(NppXml::Element xmlKeyword)
{
	if (isVisible())
		close();
	_xmlKeyword = xmlKeyword;
	reset();
	_funcName.reset(nullptr);
}

bool FunctionCallTip::updateCalltip(int ch, bool needShown)
{
	if (!needShown && ch != _start && ch != _param && !isVisible())
		return false;

	_curPos = _pEditView->send(SCI_GETCURRENTPOS);

	if (!getCursorFunction())
	{
		close();
		return false;
	}
	showCalltip();
	return true;
}

void FunctionCallTip::showNextOverload()
{
	if (!isVisible())
		return;

	if (_currentNbOverloads > 0)
		_currentOverload = (_currentOverload + 1) % _currentNbOverloads;

	showCalltip();
}

void FunctionCallTip::showPrevOverload()
{
	if (!isVisible())
		return;
	_currentOverload = _currentOverload > 0 ? (_currentOverload - 1) : (_currentNbOverloads - 1);
	showCalltip();
}

void FunctionCallTip::close()
{
	if (!isVisible() || !_selfActivated)
		return;

	_pEditView->send(SCI_CALLTIPCANCEL);
	_selfActivated = false;
	_currentOverload = 0;
}

bool FunctionCallTip::getCursorFunction()
{
	auto line = _pEditView->send(SCI_LINEFROMPOSITION, _curPos);
	intptr_t startpos = _pEditView->send(SCI_POSITIONFROMLINE, line);
	intptr_t endpos = _pEditView->send(SCI_GETLINEENDPOSITION, line);
	intptr_t len = endpos - startpos + 3;
	intptr_t offset = _curPos - startpos;
	static constexpr intptr_t maxLen = 256;

	if ((offset < 2) || (len >= maxLen))
	{
		reset();
		return false;
	}

	char lineData[maxLen]{};
	_pEditView->getLine(line, lineData, len);

	// Split line into tokens
	std::vector<Token> tokenVector;

	for (int i = 0; i < offset; ++i)
	{
		char ch = lineData[i];
		if (isBasicWordChar(ch) || isAdditionalWordChar(ch))
		{
			int tokenLen = 0;
			char* begin = lineData + i;
			while ((isBasicWordChar(ch) || isAdditionalWordChar(ch)) && i < offset)
			{
				++tokenLen;
				++i;
				ch = lineData[i];
			}
			tokenVector.push_back(Token(begin, tokenLen, true));
			--i;
		}
		else
		{
			if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r')
				tokenVector.push_back(Token(lineData + i, 1, false));
		}
	}

	size_t vsize = tokenVector.size();

	// Stack-based parsing for nested functions
	std::vector<FunctionValues> valueVec;
	FunctionValues curValue;
	int scopeLevel = 0;

	for (size_t i = 0; i < vsize; ++i)
	{
		const Token& curToken = tokenVector.at(i);
		if (curToken.isIdentifier)
		{
			curValue.lastIdentifier = static_cast<int>(i);
		}
		else
		{
			if (curToken.token[0] == _start)
			{
				++scopeLevel;
				FunctionValues newValue = curValue;
				valueVec.push_back(newValue);
				curValue.scopeLevel = scopeLevel;
				if (i > 0 && curValue.lastIdentifier == static_cast<int>(i) - 1)
				{
					curValue.lastFunctionIdentifier = curValue.lastIdentifier;
					curValue.param = 0;
				}
				else
				{
					curValue.lastFunctionIdentifier = -1;
				}
			}
			else if (curToken.token[0] == _param && curValue.lastFunctionIdentifier > -1)
			{
				++curValue.param;
			}
			else if (curToken.token[0] == _stop)
			{
				if (scopeLevel)
					--scopeLevel;
				if (!valueVec.empty())
				{
					curValue = valueVec.back();
					valueVec.pop_back();
				}
				else
				{
					curValue = FunctionValues();
				}
			}
			else if (curToken.token[0] == _terminal)
			{
				valueVec.clear();
				curValue = FunctionValues();
			}
		}
	}

	bool res = false;

	if (curValue.lastFunctionIdentifier == -1)
	{
		while (curValue.lastFunctionIdentifier == -1 && !valueVec.empty())
		{
			curValue = valueVec.back();
			valueVec.pop_back();
		}
	}

	if (curValue.lastFunctionIdentifier > -1)
	{
		Token funcToken = tokenVector.at(curValue.lastFunctionIdentifier);
		funcToken.token[funcToken.length] = 0;
		_currentParam = curValue.param;

		bool same = false;
		if (_funcName.get())
		{
			if (_ignoreCase)
				same = testNameNoCase(_funcName.get(), funcToken.token, static_cast<int>(std::strlen(_funcName.get()))) == 0;
			else
				same = std::strncmp(_funcName.get(), funcToken.token, std::strlen(_funcName.get())) == 0;
		}

		if (!same)
		{
			_funcName.reset(new char[funcToken.length + 1]);
			std::strcpy(_funcName.get(), funcToken.token);
			res = loadFunction();
		}
		else
		{
			res = true;
		}
	}
	return res;
}

bool FunctionCallTip::loadFunction()
{
	reset();
	_curFunction = {};

	for (NppXml::Element funcNode = _xmlKeyword;
		funcNode;
		funcNode = NppXml::nextSiblingElement(funcNode, "KeyWord"))
	{
		const char* name = NppXml::attribute(funcNode, "name");
		if (!name)
			continue;

		int compVal = 0;
		if (_ignoreCase)
			compVal = testNameNoCase(name, _funcName.get());
		else
			compVal = std::strcmp(name, _funcName.get());

		if (compVal == 0)
		{
			const char* val = NppXml::attribute(funcNode, "func");
			if (val)
			{
				if (std::strcmp(val, "yes") == 0)
				{
					_curFunction = funcNode;
					break;
				}
				else
				{
					return false;
				}
			}
		}
	}

	if (!_curFunction)
		return false;

	stringVec paramVec;

	for (NppXml::Element overloadNode = NppXml::firstChildElement(_curFunction, "Overload");
		overloadNode;
		overloadNode = NppXml::nextSiblingElement(overloadNode, "Overload"))
	{
		const char* retVal = NppXml::attribute(overloadNode, "retVal");
		if (!retVal)
			continue;
		_retVals.push_back(retVal);

		const char* description = NppXml::attribute(overloadNode, "descr");
		if (description)
			_descriptions.push_back(description);
		else
			_descriptions.push_back("");

		for (NppXml::Element paramNode = NppXml::firstChildElement(overloadNode, "Param");
			paramNode;
			paramNode = NppXml::nextSiblingElement(paramNode, "Param"))
		{
			const char* param = NppXml::attribute(paramNode, "name");
			if (!param)
				continue;
			paramVec.push_back(param);
		}
		_overloads.push_back(paramVec);
		paramVec.clear();
	}

	_currentNbOverloads = _overloads.size();
	if (_currentNbOverloads == 0)
		return false;

	return true;
}

void FunctionCallTip::showCalltip()
{
	if (_currentNbOverloads == 0)
		return;

	const stringVec& params = _overloads.at(_currentOverload);
	size_t psize = params.size() + 1;
	if (_currentParam >= psize)
	{
		size_t osize = _overloads.size();
		for (size_t i = 0; i < osize; ++i)
		{
			psize = _overloads.at(i).size() + 1;
			if (_currentParam < psize)
			{
				_currentOverload = i;
				break;
			}
		}
	}

	std::stringstream callTipText;

	if (_currentNbOverloads > 1)
		callTipText << "\001" << _currentOverload + 1 << " of " << _currentNbOverloads << "\002";

	callTipText << _retVals.at(_currentOverload) << ' ' << _funcName << ' ' << _start;

	int highlightstart = 0;
	int highlightend = 0;
	size_t nbParams = params.size();
	for (size_t i = 0; i < nbParams; ++i)
	{
		if (i == _currentParam)
			highlightstart = static_cast<int>(callTipText.str().length());
		callTipText << params.at(i);
		if (i < nbParams - 1)
			callTipText << _param << ' ';
		if (i == _currentParam)
			highlightend = static_cast<int>(callTipText.str().length());
	}

	callTipText << _stop;
	if (_descriptions.at(_currentOverload)[0])
		callTipText << "\n" << _descriptions.at(_currentOverload);

	if (isVisible())
		_pEditView->send(SCI_CALLTIPCANCEL);
	else
		_startPos = _curPos;

	_pEditView->showCallTip(_startPos, callTipText.str());

	_selfActivated = true;
	if (highlightstart != highlightend)
		_pEditView->send(SCI_CALLTIPSETHLT, highlightstart, highlightend);
}

void FunctionCallTip::reset()
{
	_currentOverload = 0;
	_currentParam = 0;
	_startPos = 0;
	_overloads.clear();
	_currentNbOverloads = 0;
	_retVals.clear();
	_descriptions.clear();
}

void FunctionCallTip::cleanup()
{
	reset();
	_funcName.reset(nullptr);
	_pEditView = nullptr;
}
