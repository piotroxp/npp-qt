// npp-qt: AutoCompletion implementation
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/AutoCompletion.cpp
// Target: npp-qt/src/ScintillaComponent/AutoCompletion.cpp

#include "AutoCompletion.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <QDir>
#include <QFileInfo>

#include <SciLexer.h>
#include "NppXml.h"
#include <Scintilla.h>

#include "Buffer.h"
#include "Common.h"
#include "NppDarkMode.h"
#include "NppXml.h"
#include "Parameters.h"
#include "ScintillaComponent.h"

// XPM image data for function icon (dark theme)
static constexpr auto FUNC_IMG_ID = 1000;
static constexpr const char* xpmfn[]{
	"16 16 36 1 ",
	"u c None",
	"  c #131313",
	". c #252525",
	"X c #161616",
	"o c #202020",
	"O c #393939",
	"+ c #242424",
	"@ c #282828",
	"# c #4E4E4E",
	"$ c #343434",
	"% c #5B5B5B",
	"& c #5F5F5F",
	"* c #626262",
	"= c #404040",
	"- c #686868",
	"; c #434343",
	": c #464646",
	"> c #484848",
	", c #494949",
	"< c #515151",
	"1 c #929292",
	"2 c #9B9B9B",
	"3 c #636363",
	"4 c #656565",
	"5 c #AFAFAF",
	"6 c #B7B7B7",
	"7 c #757575",
	"8 c #CDCDCD",
	"9 c #858585",
	"0 c #868686",
	"q c #DDDDDD",
	"w c #E1E1E1",
	"e c #E9E9E9",
	"r c #EEEEEE",
	"t c #959595",
	"y c #F6F6F6",
	"uuuuuuuuuuuuuuuu",
	"uuuuu5o.:uuuuuuu",
	"uuuu8 $:.0uuuuuu",
	"uuuu2 uuuuuuuuuu",
	"uuu6$ 46uuuuuuuu",
	"uuuO   Ouuuuuuuu",
	"uuuu;#uuuuuuuuuu",
	"uuuu##u& 3uu<+uu",
	"uuuu#;0.@X0, >uu",
	"uuuu+>uuuoo >uuu",
	"uuuu >uuu* =uuuu",
	"uuuu 2uu, Xotuuu",
	"uuu# 4u< >9 %ouu",
	"u:,#X0uO>uu1 $ u",
	"u- +7uuuuuuuuuuu",
	"uuuuuuuuuuuuuuuu"
};

// XPM image data for function icon (light theme)
static constexpr const char* xpmfnDark[]{
	"16 16 36 1 ",
	"u c None",
	"  c #ECECEC",
	". c #DADADA",
	"X c #E9E9E9",
	"o c #DFDFDF",
	"O c #C6C6C6",
	"+ c #DBDBDB",
	"@ c #D7D7D7",
	"# c #B1B1B1",
	"$ c #CBCBCB",
	"% c #A4A4A4",
	"& c #A0A0A0",
	"* c #9D9D9D",
	"= c #404040",
	"- c #BFBFBF",
	"; c #BCBCBC",
	": c #B9B9B9",
	"> c #B7B7B7",
	", c #494949",
	"< c #B6B6B6",
	"1 c #6D6D6D",
	"2 c #646464",
	"3 c #9C9C9C",
	"4 c #9A9A9A",
	"5 c #505050",
	"6 c #484848",
	"7 c #8A8A8A",
	"8 c #323232",
	"9 c #7A7A7A",
	"0 c #797979",
	"q c #222222",
	"w c #1E1E1E",
	"e c #161616",
	"r c #111111",
	"t c #6A6A6A",
	"y c #090909",
	"uuuuuuuuuuuuuuuu",
	"uuuuu5o.:uuuuuuu",
	"uuuu8 $:.0uuuuuu",
	"uuuu2 uuuuuuuuuu",
	"uuu6$ 46uuuuuuuu",
	"uuuO   Ouuuuuuuu",
	"uuuu;#uuuuuuuuuu",
	"uuuu##u& 3uu<+uu",
	"uuuu#;0.@X0, >uu",
	"uuuu+>uuuoo >uuu",
	"uuuu >uuu* =uuuu",
	"uuuu 2uu, Xotuuu",
	"uuu# 4u< >9 %ouu",
	"u:,#X0uO>uu1 $ u",
	"u- +7uuuuuuuuuuu",
	"uuuuuuuuuuuuuuuu"
};

// XPM image data for box icon (dark theme)
static constexpr auto BOX_IMG_ID = 1001;
static constexpr const char* xpmbox[]{
	"16 16 33 1 ",
	"r c None",
	"  c #000000",
	". c #030303",
	"X c #101010",
	"o c #181818",
	"O c #202020",
	"+ c #282828",
	"@ c #191919",
	"# c #222222",
	"$ c #252525",
	"% c #484848",
	"& c #505050",
	"* c #606060",
	"= c #444444",
	"- c #474747",
	"; c #505050",
	": c #535353",
	"> c #565656",
	", c #979797",
	"< c #9A9A9A",
	"1 c #9F9F9F",
	"2 c #A7A7A7",
	"3 c #AFAFAF",
	"4 c #B7B7B7",
	"5 c #757575",
	"6 c #767676",
	"7 c #787878",
	"8 c #818181",
	"9 c #D7D7D7",
	"0 c #DFDFDF",
	"q c #E7E7E7",
	"w c #EFEFEF",
	"e c #979797",
	"rrrrrrrrrrrrrrrr",
	"rrrrrrrrrrrrrrrr",
	"rrre4;$  *rrrrrr",
	"r4@ $-rr6;X rrrr",
	"r1oX>rrrrr5+  rr",
	"r1*r%OrrrXO*&;rr",
	"r1*rrr==%rrr1;rr",
	"r1*rrrr$rrrr1;rr",
	"r1*rrrr$rrrr1;rr",
	"r1*rrrr$rrrr1;rr",
	"r1*rrrr$rrrr1;rr",
	"r3o<rrr$rrr8$;rr",
	"rr7#%rr$rr #;rrr",
	"rrrr,o.$.%:rrrrr",
	"rrrrrr7*7rrrrrrr",
	"rrrrrrrrrrrrrrrr"
};

// XPM image data for box icon (light theme)
static constexpr const char* xpmboxDark[]{
	"16 16 33 1 ",
	"r c None",
	"  c #FFFFFF",
	". c #FCFCFC",
	"X c #EFEFEF",
	"o c #E7E7E7",
	"O c #DFDFDF",
	"+ c #D7D7D7",
	"@ c #E6E6E6",
	"# c #DDDDDD",
	"$ c #DADADA",
	"% c #B7B7B7",
	"& c #AFAFAF",
	"* c #9F9F9F",
	"= c #BBBBBB",
	"- c #B8B8B8",
	"; c #AFAFAF",
	": c #ACACAC",
	"> c #A9A9A9",
	", c #686868",
	"< c #656565",
	"1 c #606060",
	"2 c #585858",
	"3 c #505050",
	"4 c #484848",
	"5 c #8A8A8A",
	"6 c #898989",
	"7 c #878787",
	"8 c #7E7E7E",
	"9 c #282828",
	"0 c #202020",
	"q c #181818",
	"w c #101010",
	"e c #686868",
	"rrrrrrrrrrrrrrrr",
	"rrrrrrrrrrrrrrrr",
	"rrre4;$  *rrrrrr",
	"r4@ $-rr6;X rrrr",
	"r1oX>rrrrr5+  rr",
	"r1*r%OrrrXO*&;rr",
	"r1*rrr==%rrr1;rr",
	"r1*rrrr$rrrr1;rr",
	"r1*rrrr$rrrr1;rr",
	"r1*rrrr$rrrr1;rr",
	"r1*rrrr$rrrr1;rr",
	"r3o<rrr$rrr8$;rr",
	"rr7#%rr$rr #;rrr",
	"rrrr,o.$.%:rrrrr",
	"rrrrrr7*7rrrrrrr",
	"rrrrrrrrrrrrrrrr"
};

static constexpr size_t tagMaxLen = 256;

// Static color initializers
QRgb AutoCompletion::_autocompleteText = qRgb(0x00, 0x00, 0x00);
QRgb AutoCompletion::_autocompleteBg    = qRgb(0xFF, 0xFF, 0xFF);
QRgb AutoCompletion::_selectedText      = qRgb(0xFF, 0xFF, 0xFF);
QRgb AutoCompletion::_selectedBg        = qRgb(0x00, 0x78, 0xD7);
QRgb AutoCompletion::_calltipBg         = qRgb(0xFF, 0xFF, 0xFF);
QRgb AutoCompletion::_calltipText       = qRgb(0x80, 0x80, 0x80);
QRgb AutoCompletion::_calltipHighlight  = qRgb(0x00, 0x00, 0x80);

// Helper: check if word is in array
static bool isInList(const std::string& word, const std::vector<std::string>& wordArray)
{
	return std::find(wordArray.begin(), wordArray.end(), word) != wordArray.end();
}

// Helper: check if string is all digits
static bool isAllDigits(const std::string& str)
{
	return std::all_of(str.begin(), str.end(),
		[](auto c) { return std::isdigit(static_cast<unsigned char>(c)); });
}

// Helper: sort word array case-insensitively
static void sortInsensitive(std::vector<std::string>& wordArray)
{
	std::sort(
		wordArray.begin(),
		wordArray.end(),
		[](const std::string& a, const std::string& b)
		{
			return lexicographical_compare(
				a.begin(), a.end(),
				b.begin(), b.end(),
				[](auto ch1, auto ch2)
				{
					return std::tolower(static_cast<unsigned char>(ch1))
						< std::tolower(static_cast<unsigned char>(ch2));
				}
			);
		}
	);
}

// Core auto-complete logic
bool AutoCompletion::showAutoComplete(AutocompleteType autocType, bool autoInsert)
{
	if (autocType == autocFunc && !_funcCompletionActive)
		return false;

	intptr_t curPos = _pEditView->send(SCI_GETCURRENTPOS);
	intptr_t startPos = _pEditView->send(SCI_WORDSTARTPOSITION, curPos, true);

	if (curPos == startPos)
		return false;

	const auto len = static_cast<size_t>(std::abs(curPos - startPos));

	std::string words;

	if (autocType == autocFunc)
	{
		if (len >= _keyWordMaxLen)
			return false;
	}
	else
	{
		static constexpr size_t bufSize = 256;
		if (len >= bufSize)
			return false;

		intptr_t endPos = _pEditView->send(SCI_WORDENDPOSITION, curPos, true);
		const auto lena = static_cast<size_t>(std::abs(endPos - startPos));
		if (lena >= bufSize)
			return false;

		char beginChars[bufSize]{};
		_pEditView->getGenericText(beginChars, bufSize, startPos, curPos);

		std::vector<std::string> wordArray;

		if (autocType == autocWord || autocType == autocFuncAndWord)
		{
			char excludeChars[bufSize]{};
			_pEditView->getGenericText(excludeChars, bufSize, startPos, endPos);
			getWordArray(wordArray, beginChars, excludeChars);
		}

		if (autocType == autocFuncBrief || autocType == autocFuncAndWord)
		{
			for (size_t i = 0, kwlen = _keyWordArray.size(); i < kwlen; ++i)
			{
				int compareResult = 0;
				if (_ignoreCase)
				{
					const std::string kwSufix = _keyWordArray[i].substr(0, len);
					compareResult = strcasecmp(beginChars, kwSufix.c_str());
				}
				else
				{
					compareResult = _keyWordArray[i].compare(0, len, beginChars);
				}

				if (compareResult == 0)
				{
					if (!isInList(_keyWordArray[i], wordArray))
						wordArray.push_back(_keyWordArray[i]);
				}
			}
		}

		if (!wordArray.size())
			return false;

		if (autocType == autocWord && wordArray.size() == 1 && autoInsert)
		{
			const size_t typeSeparatorPos = wordArray[0].find("\x1E");
			intptr_t replacedLength = _pEditView->replaceTarget(
				(typeSeparatorPos == std::string::npos) ?
				wordArray[0] : wordArray[0].substr(0, typeSeparatorPos),
				startPos, curPos
			);
			_pEditView->send(SCI_GOTOPOS, startPos + replacedLength);
			return true;
		}

		if (autocType == autocWord || autocType & autocFuncAndWord)
		{
			if (_ignoreCase)
				sortInsensitive(wordArray);
			else
				std::sort(wordArray.begin(), wordArray.end());
		}

		for (size_t i = 0, wordArrayLen = wordArray.size(); i < wordArrayLen; ++i)
		{
			words += wordArray[i];
			if (i != wordArrayLen - 1)
				words += " ";
		}
	}

	// Register images and show autocompletion
	if (NppDarkMode::isThemeDark())
	{
		if (!_isFxImageRegisteredDark)
		{
			_pEditView->send(SCI_CLEARREGISTEREDIMAGES);
			_pEditView->send(SCI_REGISTERIMAGE, FUNC_IMG_ID, reinterpret_cast<sptr_t>(xpmfnDark));
			_pEditView->send(SCI_REGISTERIMAGE, BOX_IMG_ID, reinterpret_cast<sptr_t>(xpmboxDark));
			_isFxImageRegistered = false;
			_isFxImageRegisteredDark = true;
		}
	}
	else
	{
		if (!_isFxImageRegistered)
		{
			_pEditView->send(SCI_CLEARREGISTEREDIMAGES);
			_pEditView->send(SCI_REGISTERIMAGE, FUNC_IMG_ID, reinterpret_cast<sptr_t>(xpmfn));
			_pEditView->send(SCI_REGISTERIMAGE, BOX_IMG_ID, reinterpret_cast<sptr_t>(xpmbox));
			_isFxImageRegistered = true;
			_isFxImageRegisteredDark = false;
		}
	}

	_pEditView->send(SCI_AUTOCSETTYPESEPARATOR, '\x1E');
	_pEditView->send(SCI_AUTOCSETSEPARATOR, ' ');
	_pEditView->send(SCI_AUTOCSETIGNORECASE, _ignoreCase);
	_pEditView->send(SCI_AUTOCSETCASEINSENSITIVEBEHAVIOUR, _ignoreCase);

	if (autocType == autocFunc)
		_pEditView->send(SCI_AUTOCSHOW, static_cast<int>(curPos - startPos), reinterpret_cast<sptr_t>(_keyWords.c_str()));
	else
		_pEditView->send(SCI_AUTOCSHOW, static_cast<int>(curPos - startPos), reinterpret_cast<sptr_t>(words.c_str()));

	return true;
}

bool AutoCompletion::showApiComplete()
{
	return showAutoComplete(autocFunc, false);
}

bool AutoCompletion::showApiAndWordComplete()
{
	return showAutoComplete(autocFuncAndWord, false);
}

void AutoCompletion::getWordArray(std::vector<std::string>& wordArray, const char* beginChars, const char* excludeChars) const
{
	static constexpr size_t bufSize = 256;
	const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();

	if (nppGUI._autocIgnoreNumbers && isAllDigits(beginChars))
		return;

	std::string expr = "\\<";
	expr += beginChars;
	expr += "[^ \\t\\n\\r.,;:\"(){}=<>'+!?\\[\\]]+";

	size_t docLength = _pEditView->send(SCI_GETLENGTH);
	int flags = SCFIND_WORDSTART | SCFIND_REGEXP | SCFIND_POSIX;

	if (!_ignoreCase || (!_funcCompletionActive && _curLang == LangType::L_TEXT))
		flags |= SCFIND_MATCHCASE;

	_pEditView->send(SCI_SETSEARCHFLAGS, flags);
	intptr_t _ac_fpos = 0;
	intptr_t posFind = _pEditView->searchInTarget(expr, _ac_fpos, static_cast<size_t>(docLength));

	const std::string boxId = "\x1E" + std::to_string(BOX_IMG_ID);
	const std::string funcId = "\x1E" + std::to_string(FUNC_IMG_ID);

	while (posFind >= 0)
	{
		intptr_t wordStart = _pEditView->send(SCI_GETTARGETSTART);
		intptr_t wordEnd = _pEditView->send(SCI_GETTARGETEND);

		size_t foundTextLen = wordEnd - wordStart;
		if (foundTextLen < bufSize)
		{
			char w[bufSize]{};
			_pEditView->getGenericText(w, bufSize, wordStart, wordEnd);
			if (!excludeChars || (std::strncmp(w, excludeChars, bufSize) != 0))
			{
				if (_funcCompletionActive)
				{
					if (isInList(w + boxId, _keyWordArray))
					{
						if (!isInList(w + boxId, wordArray))
							wordArray.push_back(w + boxId);
					}
					else if (isInList(w + funcId, _keyWordArray))
					{
						if (!isInList(w + funcId, wordArray))
							wordArray.push_back(w + funcId);
					}
					else if (!isInList(w, wordArray))
					{
						wordArray.push_back(w);
					}
				}
				else
				{
					if (!isInList(w, wordArray))
						wordArray.push_back(w);
				}
			}
		}
		intptr_t _ac_tpos = wordEnd;
		posFind = _pEditView->searchInTarget(expr, _ac_tpos, static_cast<size_t>(docLength));
	}
}

static QString addTrailingSlash(const QString& path)
{
	if (!path.isEmpty() && path.back() != u'\\')
		return path + u'\\';
	return path;
}

static std::wstring addTrailingSlash(const std::wstring& path)
{
	if (path.length() >= 1 && path[path.length() - 1] == L'\\')
		return path;
	return path + L"\\";
}

static std::wstring removeTrailingSlash(const std::wstring& path)
{
	if (path.length() >= 1 && path[path.length() - 1] == L'\\')
		return path.substr(0, path.length() - 1);
	return path;
}

static bool isAllowedBeforeDriveLetter(wchar_t c)
{
	return c == L'\'' || c == L'"' || c == L'(' || std::iswspace(c);
}

static bool getRawPath(const std::wstring& input, std::wstring& rawPath_out)
{
	size_t lastOccurrence = input.rfind(L":");
	if (lastOccurrence == std::string::npos) return false;
	if (lastOccurrence == 0) return false;
	if (!std::iswalpha(input[lastOccurrence - 1])) return false;
	if (lastOccurrence >= 2 && !isAllowedBeforeDriveLetter(input[lastOccurrence - 2])) return false;

	rawPath_out = input.substr(lastOccurrence - 1);
	return true;
}

static bool getPathsForPathCompletion(const std::wstring& input, std::wstring& rawPath_out, std::wstring& pathToMatch_out)
{
	std::wstring rawPath;
	if (!getRawPath(input, rawPath))
		return false;

	QFileInfo fi(QString::fromStdWString(removeTrailingSlash(rawPath)));
	if (fi.exists())
		return false;

	QFileInfo di(QString::fromStdWString(rawPath));
	if (di.isDir())
	{
		rawPath_out = rawPath;
		pathToMatch_out = rawPath;
		return true;
	}

	size_t last_occurrence = rawPath.rfind(L"\\");
	if (last_occurrence == std::wstring::npos)
		return false;

	rawPath_out = rawPath;
	pathToMatch_out = rawPath.substr(0, last_occurrence);
	return true;
}

void AutoCompletion::showPathCompletion()
{
	std::wstring currentLine;
	{
		static constexpr intptr_t bufSize = 4096;
		auto buf = std::string(bufSize + 1, '\0');
		const intptr_t currentPos = _pEditView->send(SCI_GETCURRENTPOS);
		const auto startPos = std::max<intptr_t>(0, currentPos - bufSize);
		_pEditView->getGenericText(buf.data(), bufSize + 1, startPos, currentPos);
		const auto cp = static_cast<int>(_pEditView->send(SCI_GETCODEPAGE));
		currentLine = string2wstring(buf, cp);
	}

    std::wstring rawPath, pathToMatch;
    std::wstring _ac_curLine = currentLine;
	if (!getPathsForPathCompletion(_ac_curLine, rawPath, pathToMatch))
		return;

	QString autoCompleteEntries;
	{
		QDir dir(QString::fromStdWString(addTrailingSlash(pathToMatch)));
		if (!dir.exists())
			return;

		static constexpr unsigned int maxEntries = 2000;
		unsigned int counter = 0;

		QFileInfoList entries = dir.entryInfoList(
			QDir::AllEntries | QDir::NoDotAndDotDot,
			QDir::Name | QDir::DirsFirst
		);

		for (const QFileInfo& fi : entries)
		{
			if (++counter > maxEntries)
				break;
			if (!autoCompleteEntries.isEmpty())
				autoCompleteEntries.push_back(u'\n');
			autoCompleteEntries += QString::fromStdWString(addTrailingSlash(pathToMatch));
			autoCompleteEntries += fi.fileName();
			if (fi.isDir())
				autoCompleteEntries.push_back(u'\\');
		}
	}

	if (autoCompleteEntries.isEmpty())
		return;

	_pEditView->send(SCI_AUTOCSETSEPARATOR, '\n');
	_pEditView->send(SCI_AUTOCSETIGNORECASE, true);
	_pEditView->send(SCI_AUTOCSETCASEINSENSITIVEBEHAVIOUR, true);
	_pEditView->send(SCI_AUTOCSHOW, static_cast<int>(rawPath.length()), reinterpret_cast<sptr_t>(autoCompleteEntries.toUtf8().constData()));
}

bool AutoCompletion::showWordComplete(bool autoInsert)
{
	return showAutoComplete(autocWord, autoInsert);
}

bool AutoCompletion::showFunctionComplete()
{
	if (!_funcCompletionActive)
		return false;
	if (_funcCalltip->updateCalltip(0, true))
		return true;
	return false;
}

void AutoCompletion::getCloseTag(char* closeTag, size_t closeTagLen, size_t caretPos, bool isHTML) const
{
	if (isHTML)
	{
		size_t style = _pEditView->send(SCI_GETSTYLEAT, caretPos);
		if (style >= SCE_HJ_START)
			return;
	}

	char prev = static_cast<char>(_pEditView->send(SCI_GETCHARAT, caretPos - 2));
	char prevprev = static_cast<char>(_pEditView->send(SCI_GETCHARAT, caretPos - 3));

	if (prevprev == '-' && prev == '-')
		return;
	if (prev == '/')
		return;

	int flags = SCFIND_REGEXP | SCFIND_POSIX;
	_pEditView->send(SCI_SETSEARCHFLAGS, flags);
	static constexpr std::string_view tag2find = "<[^\\s>]*";

	std::string _ac_tag(tag2find);
	intptr_t _ac_cpos = caretPos;
	const intptr_t targetStart = _pEditView->searchInTarget(_ac_tag, _ac_cpos, static_cast<size_t>(0));
	if (targetStart < 0)
		return;

	intptr_t targetEnd = _pEditView->send(SCI_GETTARGETEND);
	intptr_t foundTextLen = targetEnd - targetStart;
	if (foundTextLen < 2)
		return;
	if (foundTextLen > static_cast<intptr_t>(closeTagLen) - 2)
		return;

	char tagHead[tagMaxLen]{};
	_pEditView->getText(tagHead, targetStart, targetEnd);

	if (tagHead[1] == '/') return;
	if (tagHead[1] == '?') return;
	if (std::strncmp(tagHead, "<!--", 4) == 0) return;

	if (isHTML)
	{
		static constexpr const char* disallowedTags[]{
			"area", "base", "br", "col", "embed", "hr", "img", "input",
			"keygen", "link", "meta", "param", "source", "track", "wbr", "!doctype"
		};
		size_t disallowedTagsLen = sizeof(disallowedTags) / sizeof(char*);
		for (size_t i = 0; i < disallowedTagsLen; ++i)
		{
			if (strncasecmp(tagHead + 1, disallowedTags[i], std::strlen(disallowedTags[i])) == 0)
				return;
		}
	}

	closeTag[0] = '<';
	closeTag[1] = '/';
	_pEditView->getText(closeTag + 2, targetStart + 1, targetEnd);
	closeTag[foundTextLen + 1] = '>';
	closeTag[foundTextLen + 2] = '\0';
}

void InsertedMatchedChars::removeInvalidElements(MatchedCharInserted mci)
{
	if (mci._c == '\n' || mci._c == '\r')
	{
		_insertedMatchedChars.clear();
	}
	else
	{
		for (int i = static_cast<int>(_insertedMatchedChars.size()) - 1; i >= 0; --i)
		{
			if (_insertedMatchedChars[i]._pos < mci._pos)
			{
				auto posToDetectLine = _pEditView->send(SCI_LINEFROMPOSITION, mci._pos);
				auto startPosLine = _pEditView->send(SCI_LINEFROMPOSITION, _insertedMatchedChars[i]._pos);
				if (posToDetectLine != startPosLine)
					_insertedMatchedChars.erase(_insertedMatchedChars.begin() + i);
			}
			else
			{
				_insertedMatchedChars.erase(_insertedMatchedChars.begin() + i);
			}
		}
	}
}

void InsertedMatchedChars::add(MatchedCharInserted mci)
{
	removeInvalidElements(mci);
	_insertedMatchedChars.push_back(mci);
}

intptr_t InsertedMatchedChars::search(char startChar, char endChar, size_t posToDetect)
{
	if (isEmpty())
		return -1;
	auto posToDetectLine = _pEditView->send(SCI_LINEFROMPOSITION, posToDetect);

	for (auto i = static_cast<int>(_insertedMatchedChars.size()) - 1; i >= 0; --i)
	{
		if (_insertedMatchedChars[i]._c == startChar)
		{
			if (_insertedMatchedChars[i]._pos < posToDetect)
			{
				auto startPosLine = _pEditView->send(SCI_LINEFROMPOSITION, _insertedMatchedChars[i]._pos);
				if (posToDetectLine == startPosLine)
				{
					size_t endPos = _pEditView->send(SCI_GETLINEENDPOSITION, startPosLine);
					for (auto j = posToDetect; j <= endPos; ++j)
					{
						char aChar = static_cast<char>(_pEditView->send(SCI_GETCHARAT, j));
						if (aChar != ' ')
						{
							if (aChar == endChar)
							{
								_insertedMatchedChars.erase(_insertedMatchedChars.begin() + i);
								return j;
							}
							else
							{
								_insertedMatchedChars.erase(_insertedMatchedChars.begin() + i);
								return -1;
							}
						}
					}
				}
				else
				{
					_insertedMatchedChars.erase(_insertedMatchedChars.begin() + i);
				}
			}
			else
			{
				_insertedMatchedChars.erase(_insertedMatchedChars.begin() + i);
			}
		}
	}
	return -1;
}

void AutoCompletion::insertMatchedChars(int character, const MatchedPairConf& matchedPairConf)
{
	const size_t caretPos = _pEditView->send(SCI_GETCURRENTPOS);

	if (character > std::numeric_limits<char>::max())
	{
		if (!_insertedMatchedChars.isEmpty())
			_insertedMatchedChars.removeInvalidElements(MatchedCharInserted('\0', caretPos - 1));
		return;
	}

	const auto ch = static_cast<char>(character);
	const char* matchedChars = nullptr;

	char charPrev = static_cast<char>(_pEditView->send(SCI_GETCHARAT, caretPos - 2));
	char charNext = static_cast<char>(_pEditView->send(SCI_GETCHARAT, caretPos));

	bool isCharPrevBlank = (charPrev == ' ' || charPrev == '\t' || charPrev == '\n' || charPrev == '\r' || charPrev == '\0');
	size_t docLen = _pEditView->getCurrentDocLen();
	bool isCharNextBlank = (charNext == ' ' || charNext == '\t' || charNext == '\n' || charNext == '\r' || caretPos == docLen);
	bool isCharNextCloseSymbol = (charNext == ')' || charNext == ']' || charNext == '}');
	bool isInSandwich = (charPrev == '(' && charNext == ')') || (charPrev == '[' && charNext == ']') || (charPrev == '{' && charNext == '}');

	// User defined matched pairs
	for (const auto& matchedPair : matchedPairConf._matchedPairs)
	{
		if (matchedPair.first == ch && isCharNextBlank)
		{
			const char userMatchedChar[2]{ static_cast<char>(matchedPair.second.unicode()), '\0' };
			_pEditView->send(SCI_INSERTTEXT, caretPos, reinterpret_cast<sptr_t>(userMatchedChar));
			return;
		}
	}

	char closeTag[tagMaxLen]{};
	switch (ch)
	{
		case '(':
			if (matchedPairConf._doParentheses && (isCharNextBlank || isCharNextCloseSymbol))
			{
				matchedChars = ")";
				_insertedMatchedChars.add(MatchedCharInserted(ch, caretPos - 1));
			}
			break;
		case '[':
			if (matchedPairConf._doBrackets && (isCharNextBlank || isCharNextCloseSymbol))
			{
				matchedChars = "]";
				_insertedMatchedChars.add(MatchedCharInserted(ch, caretPos - 1));
			}
			break;
		case '{':
			if (matchedPairConf._doCurlyBrackets && (isCharNextBlank || isCharNextCloseSymbol))
			{
				matchedChars = "}";
				_insertedMatchedChars.add(MatchedCharInserted(ch, caretPos - 1));
			}
			break;
		case '"':
			if (matchedPairConf._doDoubleQuotes)
			{
				if (!_insertedMatchedChars.isEmpty())
				{
					intptr_t pos = _insertedMatchedChars.search('"', ch, caretPos);
					if (pos != -1)
					{
						_pEditView->send(SCI_DELETERANGE, pos, 1);
						_pEditView->send(SCI_GOTOPOS, pos);
						return;
					}
				}
				if ((isCharPrevBlank && isCharNextBlank) || isInSandwich ||
					(charPrev == '(' && isCharNextBlank) || (isCharPrevBlank && charNext == ')') ||
					(charPrev == '[' && isCharNextBlank) || (isCharPrevBlank && charNext == ']') ||
					(charPrev == '{' && isCharNextBlank) || (isCharPrevBlank && charNext == '}'))
				{
					matchedChars = "\"";
					_insertedMatchedChars.add(MatchedCharInserted(ch, caretPos - 1));
				}
			}
			break;
		case '\'':
			if (matchedPairConf._doQuotes)
			{
				if (!_insertedMatchedChars.isEmpty())
				{
					intptr_t pos = _insertedMatchedChars.search('\'', ch, caretPos);
					if (pos != -1)
					{
						_pEditView->send(SCI_DELETERANGE, pos, 1);
						_pEditView->send(SCI_GOTOPOS, pos);
						return;
					}
				}
				if ((isCharPrevBlank && isCharNextBlank) || isInSandwich ||
					(charPrev == '(' && isCharNextBlank) || (isCharPrevBlank && charNext == ')') ||
					(charPrev == '[' && isCharNextBlank) || (isCharPrevBlank && charNext == ']') ||
					(charPrev == '{' && isCharNextBlank) || (isCharPrevBlank && charNext == '}'))
				{
					matchedChars = "'";
					_insertedMatchedChars.add(MatchedCharInserted(ch, caretPos - 1));
				}
			}
			break;
		case '>':
			if (matchedPairConf._doHtmlXmlTag && (_curLang == LangType::L_HTML || _curLang == LangType::L_XML))
			{
				getCloseTag(closeTag, tagMaxLen, caretPos, _curLang == LangType::L_HTML);
				if (closeTag[0] != '\0')
					matchedChars = closeTag;
			}
			break;
		case ')':
		case ']':
		case '}':
			if (!_insertedMatchedChars.isEmpty())
			{
				char startChar = 0;
				if (ch == ')') { if (!matchedPairConf._doParentheses) return; startChar = '('; }
				else if (ch == ']') { if (!matchedPairConf._doBrackets) return; startChar = '['; }
				else { if (!matchedPairConf._doCurlyBrackets) return; startChar = '{'; }

				const intptr_t pos = _insertedMatchedChars.search(startChar, ch, caretPos);
				if (pos != -1)
				{
					_pEditView->send(SCI_DELETERANGE, pos, 1);
					_pEditView->send(SCI_GOTOPOS, pos);
				}
				return;
			}
			break;
		default:
			if (!_insertedMatchedChars.isEmpty())
				_insertedMatchedChars.removeInvalidElements(MatchedCharInserted(ch, caretPos - 1));
	}

	if (matchedChars)
		_pEditView->send(SCI_INSERTTEXT, caretPos, reinterpret_cast<sptr_t>(matchedChars));
}

void AutoCompletion::update(int character)
{
	if (!character)
		return;

	// --- Matched-character auto-close: brackets, quotes, HTML tags ---
	{
		MatchedPairConf conf;
		conf._doParentheses = true;
		conf._doBrackets = true;
		conf._doCurlyBrackets = true;
		conf._doQuotes = true;
		conf._doDoubleQuotes = true;
		conf._doHtmlXmlTag = (_curLang == LangType::L_HTML || _curLang == LangType::L_XML);
		insertMatchedChars(character, conf);
	}

	const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();

	// Gate: if function completion mode and not already active, skip word completion
	if (!_funcCompletionActive && nppGUI._autocStatus == nppGUI.autoc_func)
		return;

	// Handle calltip: if visible or funcParams mode, update it
	if (nppGUI._funcParams || _funcCalltip->isVisible())
	{
		if (_funcCalltip->updateCalltip(character, true))
			return;
	}

	// Don't interrupt an already-active autocomplete
	if (!nppGUI._autocBrief && _pEditView->send(SCI_AUTOCACTIVE) != 0)
		return;

	// Get current word using Qt API
	QString word = _pEditView->getWordOnCaret();
	QByteArray wordBytes = word.toUtf8();
	if (static_cast<size_t>(wordBytes.length()) < size_t{ nppGUI._autocFromLen })
		return;

	// Dispatch by autoc status
	if (nppGUI._autocStatus == nppGUI.autoc_word)
		showWordComplete(false);
	else if (nppGUI._autocStatus == nppGUI.autoc_func)
		showApiComplete();
	else if (nppGUI._autocStatus == nppGUI.autoc_both)
		showApiAndWordComplete();
}

void AutoCompletion::callTipClick(size_t direction)
{
	if (!_funcCompletionActive)
		return;

	if (direction == 1)
		_funcCalltip->showPrevOverload();
	else if (direction == 2)
		_funcCalltip->showNextOverload();
}

bool AutoCompletion::setLanguage(LangType language)
{
	if (_curLang == language && _pXmlFile)
		return true;

	_curLang = language;

	// Build path to API file: <appPath>/autoCompletion/<apiFileName>.xml
	QString appPath = QCoreApplication::applicationDirPath();
	QString apiFileName = getApiFileName();
	QString xmlPath = appPath + "/autoCompletion/" + apiFileName + ".xml";

	if (_pXmlFile)
		delete _pXmlFile;

	_pXmlFile = NppXml::NewDocument();
	_funcCompletionActive = NppXml::loadFile(_pXmlFile, xmlPath);

	NppXml::Element autoNode{};
	if (_funcCompletionActive)
	{
		_funcCompletionActive = false;
		NppXml::Element node = NppXml::firstChildElement(_pXmlFile, "NotepadPlus");
		if (!node)
			return false;

		autoNode = node = NppXml::firstChildElement(node, "AutoComplete");
		if (!node)
			return false;

		node = NppXml::firstChildElement(node, "KeyWord");
		if (!node)
			return false;

		_xmlKeyword = node;
		_funcCompletionActive = true;
	}

	if (_funcCompletionActive)
	{
		_ignoreCase = true;
		_funcCalltip->_start = '(';
		_funcCalltip->_stop = ')';
		_funcCalltip->_param = ',';
		_funcCalltip->_terminal = ';';
		_funcCalltip->_ignoreCase = true;
		_funcCalltip->_additionalWordChar.clear();

		NppXml::Element elem = NppXml::firstChildElement(autoNode, "Environment");
		if (!elem.isNull())
		{
			QString val;
			val = NppXml::attribute(elem, "ignoreCase");
			if (val.compare(QStringLiteral("no"), Qt::CaseInsensitive) == 0)
			{
				_ignoreCase = false;
				_funcCalltip->_ignoreCase = false;
			}
			val = NppXml::attribute(elem, "startFunc");
			if (!val.isEmpty())
				_funcCalltip->_start = val[0].toLatin1();
			val = NppXml::attribute(elem, "stopFunc");
			if (!val.isEmpty())
				_funcCalltip->_stop = val[0].toLatin1();
			val = NppXml::attribute(elem, "paramSeparator");
			if (!val.isEmpty())
				_funcCalltip->_param = val[0].toLatin1();
			val = NppXml::attribute(elem, "terminal");
			if (!val.isEmpty())
				_funcCalltip->_terminal = val[0].toLatin1();
			val = NppXml::attribute(elem, "additionalWordChar");
			if (!val.isEmpty())
				_funcCalltip->_additionalWordChar = val.toStdString();
		}
	}

	if (_funcCompletionActive)
		_funcCalltip->setLanguageXML(nullptr);
	else
		_funcCalltip->setLanguageXML({});

	_keyWords.clear();
	_keyWordArray.clear();

	if (_funcCompletionActive)
	{
		for (NppXml::Element funcNode = _xmlKeyword;
			!funcNode.isNull();
			funcNode = NppXml::nextSiblingElement(funcNode, "KeyWord"))
		{
				QString nameQ = NppXml::attribute(funcNode, "name");
				if (!nameQ.isEmpty())
				{
					QByteArray nameBytes = nameQ.toLatin1();
					const char* name = nameBytes.constData();
					const size_t len = std::strlen(name);
					if (len > 0)
					{
						std::string word = name;
						std::string imgid = "\x1E";
						QString funcQ = NppXml::attribute(funcNode, "func");
						if (funcQ == QStringLiteral("yes"))
							imgid += std::to_string(FUNC_IMG_ID);
						else
							imgid += std::to_string(BOX_IMG_ID);
						word += imgid;
						_keyWordArray.push_back(word);
						if (len > _keyWordMaxLen)
							_keyWordMaxLen = len;
					}
				}
		}

		if (_ignoreCase)
			sortInsensitive(_keyWordArray);
		else
			std::sort(_keyWordArray.begin(), _keyWordArray.end());

		for (size_t i = 0, len = _keyWordArray.size(); i < len; ++i)
		{
			_keyWords.append(_keyWordArray[i]);
			_keyWords.append(" ");
		}
	}
	return _funcCompletionActive;
}

QString AutoCompletion::getApiFileName() const
{
	if (_curLang == LangType::L_USER)
	{
		const Buffer* currentBuf = _pEditView->getCurrentBuffer();
		if (currentBuf->isUserDefineLangExt())
			return QString::fromWCharArray(currentBuf->getUserDefineLangName());
	}

	if (_curLang >= LangType::L_EXTERNAL && _curLang < NppParameters::getInstance().L_END)
	{
		return QString::fromStdString(NppParameters::getInstance().getELCFromIndex(_curLang - LangType::L_EXTERNAL)->_name);
	}

	// For built-in languages, use NppParameters lang name lookup
	LangType lang = _curLang;
	if (lang > LangType::L_EXTERNAL)
		lang = LangType::L_TEXT;
	if (lang == LangType::L_JAVASCRIPT)
		lang = LangType::L_JS_EMBEDDED;
	// Use the global _langNameInfoArray (declared in ScintillaComponent.h)
	return QString::fromLatin1(::_langNameInfoArray[static_cast<int>(lang)]._langName);
}

void AutoCompletion::setColour(QRgb colour2Set, AutocompleteColorIndex i)
{
	switch (i)
	{
		case AutocompleteColorIndex::autocompleteText:  _autocompleteText = colour2Set; break;
		case AutocompleteColorIndex::autocompleteBg:    _autocompleteBg = colour2Set; break;
		case AutocompleteColorIndex::selectedText:     _selectedText = colour2Set; break;
		case AutocompleteColorIndex::selectedBg:       _selectedBg = colour2Set; break;
		case AutocompleteColorIndex::calltipBg:        _calltipBg = colour2Set; break;
		case AutocompleteColorIndex::calltipText:      _calltipText = colour2Set; break;
		case AutocompleteColorIndex::calltipHighlight: _calltipHighlight = colour2Set; break;
	}
}

void AutoCompletion::drawAutocomplete(const ScintillaComponent* pEditView)
{
	pEditView->setElementColour(SC_ELEMENT_LIST, _autocompleteText);
	pEditView->setElementColour(SC_ELEMENT_LIST_BACK, _autocompleteBg);
	pEditView->setElementColour(SC_ELEMENT_LIST_SELECTED, _selectedText);
	pEditView->setElementColour(SC_ELEMENT_LIST_SELECTED_BACK, _selectedBg);

	pEditView->send(SCI_CALLTIPSETBACK, _calltipBg);
	pEditView->send(SCI_CALLTIPSETFORE, _calltipText);
	pEditView->send(SCI_CALLTIPSETFOREHLT, _calltipHighlight);
}
