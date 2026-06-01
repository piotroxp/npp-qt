// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

// Windows types are already defined via WindowsCompat.h and WindowsStubs.h
#include "MISC/Common/WindowsCompat.h"
#include "MISC/Common/WindowsStubs.h"

#include <algorithm>
#include <clocale>
// On Linux _locale_t is defined via glibc
#ifndef _locale_t
typedef __locale_t _locale_t;
#endif
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <exception>
#include <iterator>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
#include <limits>
#include <chrono>

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QClipboard>
#include <QUuid>

// Stub forward declarations
class Buffer;

// =============================================================================
// Original function declarations (Windows types stubbed for Linux)
// =============================================================================

std::wstring folderBrowser(void* parent, const std::wstring & title = L"", int outputCtrlID = 0, const wchar_t *defaultStr = nullptr);
std::wstring getFolderName(void* parent, const wchar_t *defaultDir = nullptr);

void printInt(int int2print);
void printStr(const wchar_t *str2print);
std::wstring commafyInt(size_t n);

void writeLog(const wchar_t* logFileName, const char* log2write);
void writeLog(const wchar_t* logFileName, const wchar_t* log2write);
int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep);
std::wstring purgeMenuItemString(const wchar_t* menuItemStr, bool keepAmpersand = false);
std::vector<std::wstring> tokenizeString(const std::wstring & tokenString, const char delim);

void ClientRectToScreenRect(void* hWnd, RECT* rect);
void ScreenRectToClientRect(void* hWnd, RECT* rect);

std::wstring string2wstring(const std::string& rString, unsigned int codepage = CP_UTF8);
std::string wstring2string(const std::wstring& rwString, unsigned int codepage = CP_UTF8);
bool isInList(const wchar_t* token, const wchar_t* list);
std::wstring BuildMenuFileName(int filenameLen, unsigned int pos, const std::wstring &filename, bool ordinalNumber = true);

std::string getFileContent(const wchar_t* file2read, bool* pbFailed = nullptr);
std::wstring relativeFilePathToFullFilePath(const wchar_t *relativeFilePath);
void writeFileContent(const wchar_t *file2write, const char *content2write);
bool matchInList(const wchar_t *fileName, const std::vector<std::wstring> & patterns);
bool matchInExcludeDirList(const wchar_t* dirName, const std::vector<std::wstring>& patterns, size_t level);
bool allPatternsAreExclusion(const std::vector<std::wstring>& patterns);

// =============================================================================
// WcharMbcsConvertor - Character encoding converter
// =============================================================================

class WcharMbcsConvertor final
{
public:
	static WcharMbcsConvertor& getInstance() {
		static WcharMbcsConvertor instance;
		return instance;
	}

	const wchar_t* char2wchar(const char* mbcs2Convert, size_t codepage, int lenMbcs = -1, int* pLenWc = nullptr, int* pBytesNotProcessed = nullptr);
	const wchar_t* char2wchar(const char* mbcs2Convert, size_t codepage, intptr_t* mstart, intptr_t* mend, int mbcsLen = 0);
	size_t getSizeW() const { return _wideCharStr.size(); }
	const char* wchar2char(const wchar_t* wcharStr2Convert, size_t codepage, int lenWc = -1, int* pLenMbcs = nullptr);
	const char* wchar2char(const wchar_t* wcharStr2Convert, size_t codepage, intptr_t* mstart, intptr_t* mend, int wcharLenIn = 0, int* lenOut = nullptr);
	size_t getSizeA() const { return _multiByteStr.size(); }

	const char* encode(unsigned int fromCodepage, unsigned int toCodepage, const char* txt2Encode, int lenIn = -1, int* pLenOut = nullptr, int* pBytesNotProcessed = nullptr);

protected:
	WcharMbcsConvertor() = default;
	~WcharMbcsConvertor() = default;

	WcharMbcsConvertor(const WcharMbcsConvertor&) = delete;
	WcharMbcsConvertor& operator= (const WcharMbcsConvertor&) = delete;
	WcharMbcsConvertor(WcharMbcsConvertor&&) = delete;
	WcharMbcsConvertor& operator= (WcharMbcsConvertor&&) = delete;

	template <class T> class StringBuffer final
	{
	public:
		~StringBuffer() { if (_allocLen) delete[] _str; }

		void sizeTo(size_t size) {
			if (_allocLen < size + 1)
			{
				if (_allocLen)
					delete[] _str;
				_allocLen = std::max<size_t>(size + 1, initSize);
				_str = new T[_allocLen]{};
			}
			_dataLen = size;
		}

		void empty() {
			static T nullStr = 0;
			if (_allocLen == 0)
				_str = &nullStr;
			else
				_str[0] = 0;
			_dataLen = 0;
		}

		size_t size() const { return _dataLen; }
		operator T* () { return _str; }
		operator const T* () const { return _str; }

	protected:
		static constexpr int initSize = 1024;
		size_t _allocLen = 0;
		size_t _dataLen = 0;
		T* _str = nullptr;
	};

	StringBuffer<char> _multiByteStr;
	StringBuffer<wchar_t> _wideCharStr;
};

// =============================================================================
// Remaining function declarations
// =============================================================================

std::wstring pathRemoveFileSpec(std::wstring & path);
std::wstring pathAppend(std::wstring &strDest, const std::wstring & str2append);
unsigned int getCtrlBgColor(void* hWnd);
std::wstring stringToUpper(std::wstring strToConvert);
std::wstring stringToLower(std::wstring strToConvert);
std::wstring stringReplace(std::wstring subject, const std::wstring& search, const std::wstring& replace);
void stringSplit(const std::wstring& input, const std::wstring& delimiter, std::vector<std::wstring>& output);
bool str2numberVector(std::wstring str2convert, std::vector<size_t>& numVect);
void stringJoin(const std::vector<std::wstring>& strings, const std::wstring& separator, std::wstring& joinedString);
std::wstring stringTakeWhileAdmissable(const std::wstring& input, const std::wstring& admissable);
double stodLocale(const std::wstring& str, _locale_t loc, size_t* idx = nullptr);
const std::locale& getSysLocale();

bool str2Clipboard(const std::wstring &str2cpy, void* hwnd);
std::wstring strFromClipboard();
bool buf2Clipboard(const std::vector<Buffer*>& buffers, bool isFullPath, void* hwnd);
std::wstring GetLastErrorAsString(unsigned int errorCode = 0);
std::wstring intToString(int val);
std::wstring uintToString(unsigned int val);
HWND createToolTip(int toolID, HWND hDlg, HINSTANCE hInst, wchar_t* pszText, bool isRTL);
HWND createToolTipRect(int toolID, HWND hWnd, HINSTANCE hInst, wchar_t* pszText, RECT rc);
bool isCertificateValidated(const std::wstring & fullFilePath, const std::wstring & subjectName2check);
bool isAssoCommandExisting(const wchar_t* FullPathName);
bool deleteFileOrFolder(const std::wstring& f2delete);
void getFilesInFolder(std::vector<std::wstring>& files, const std::wstring& extTypeFilter, const std::wstring& inFolder);

template<typename T> size_t vecRemoveDuplicates(std::vector<T>& vec, bool isSorted = false, bool canSort = false)
{
	if (!isSorted && canSort) {
		std::sort(vec.begin(), vec.end());
		isSorted = true;
	}
	if (isSorted) {
		auto it = std::unique(vec.begin(), vec.end());
		vec.resize(std::distance(vec.begin(), it));
	} else {
		std::unordered_set<T> seen;
		auto newEnd = std::remove_if(vec.begin(), vec.end(), [&seen](const T& value) {
			return !seen.insert(value).second;
		});
		vec.erase(newEnd, vec.end());
	}
	return vec.size();
}

void trim(std::wstring& str);
int nbDigitsFromNbLines(size_t nbLines);
std::wstring getDateTimeStrFrom(const std::wstring& dateTimeFormat, const SYSTEMTIME& st);
void* createFont(const wchar_t* fontName, int fontSize, bool isBold, void* hDestParent);
bool removeReadOnlyFlagFromFileAttributes(const wchar_t* fileFullPath);
bool toggleReadOnlyFlagFromFileAttributes(const wchar_t* fileFullPath, bool& isChangedToReadOnly);
bool isWin32NamespacePrefixedFileName(const std::wstring& fileName);
bool isWin32NamespacePrefixedFileName(const wchar_t* szFileName);
bool isUnsupportedFileName(const std::wstring& fileName);
bool isUnsupportedFileName(const wchar_t* szFileName);

// =============================================================================
// Version class
// =============================================================================

class Version final
{
public:
	Version() = default;
	explicit Version(const std::wstring& versionStr);
	void setVersionFrom(const std::wstring& filePath);
	std::wstring toString() const;
	static bool isNumber(const std::wstring& s) {
		const auto& loc = std::locale::classic();
		return !s.empty() && std::find_if(s.begin(), s.end(),
			[&loc](wchar_t c) { return !std::isdigit(c, loc); }) == s.end();
	}
	int compareTo(const Version& v2c) const;
	bool operator < (const Version& v2c) const { return compareTo(v2c) == -1; }
	bool operator <= (const Version& v2c) const { int r = compareTo(v2c); return r == -1 || r == 0; }
	bool operator > (const Version& v2c) const { return compareTo(v2c) == 1; }
	bool operator >= (const Version& v2c) const { int r = compareTo(v2c); return r == 1 || r == 0; }
	bool operator == (const Version& v2c) const { return compareTo(v2c) == 0; }
	bool operator != (const Version& v2c) const { return compareTo(v2c) != 0; }
	bool empty() const { return _major == 0 && _minor == 0 && _patch == 0 && _build == 0; }
	bool isCompatibleTo(const Version& from, const Version& to) const;

private:
	unsigned long _major = 0;
	unsigned long _minor = 0;
	unsigned long _patch = 0;
	unsigned long _build = 0;
};

// =============================================================================
// Disk / file attribute stubs
// =============================================================================

bool getDiskFreeSpaceWithTimeout(const wchar_t* dirPath, ULARGE_INTEGER* freeBytesForUser,
	unsigned int milliSec2wait = 0, bool* isTimeoutReached = nullptr);
bool getFileAttributesExWithTimeout(const wchar_t* filePath, WIN32_FILE_ATTRIBUTE_DATA* fileAttr,
	unsigned int milliSec2wait = 0, bool* isTimeoutReached = nullptr, unsigned int* pdwWin32ApiError = nullptr);
bool doesFileExist(const wchar_t* filePath, unsigned int milliSec2wait = 0, bool* isTimeoutReached = nullptr);
bool doesDirectoryExist(const wchar_t* dirPath, unsigned int milliSec2wait = 0, bool* isTimeoutReached = nullptr);
bool doesPathExist(const wchar_t* path, unsigned int milliSec2wait = 0, bool* isTimeoutReached = nullptr);
bool isWindowVisibleOnAnyMonitor(const RECT& rectWndIn);
bool isCoreWindows();

// =============================================================================
// ControlInfoTip - Windows tooltip class, stubbed for Linux
// =============================================================================

class ControlInfoTip final
{
public:
	ControlInfoTip() = default;
	~ControlInfoTip() { hide(); }
	bool init(HINSTANCE hInst, HWND ctrl2attached, HWND ctrl2attachedParent, const std::wstring& tipStr, bool isRTL, unsigned int remainTimeMillisecond = 0, int maxWidth = 200);
	bool isValid() const { return _hWndInfoTip != nullptr; }
	HWND getTipHandle() const { return _hWndInfoTip; }
	enum showPosition { beginning, middle, end };
	void show(showPosition pos = middle) const;
	void hide();

private:
	HWND _hWndInfoTip = nullptr;
	TOOLINFO _toolInfo = {};

	ControlInfoTip(const ControlInfoTip&) = delete;
	ControlInfoTip& operator=(const ControlInfoTip&) = delete;
};

unsigned int invokeNppUacOp(const std::wstring& strCmdLineParams);
bool fileTimeToYMD(const FILETIME& ft, int& yyyymmdd);
void expandEnv(std::wstring& path2Expand);

// =============================================================================
// ScopedCOMInit - COM initialization, no-op on Linux
// =============================================================================

class ScopedCOMInit final
{
public:
	ScopedCOMInit() {
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		_bInitialized = true;
	}
	~ScopedCOMInit() {
		if (_bInitialized) { _bInitialized = false; CoUninitialize(); }
	}
	bool isInitialized() const { return _bInitialized; }

private:
	bool _bInitialized = false;
	ScopedCOMInit(const ScopedCOMInit&) = delete;
	ScopedCOMInit& operator=(const ScopedCOMInit&) = delete;
};

// =============================================================================
// Utf8 class
// =============================================================================

class Utf8 final
{
public:
	static std::string encode(std::wstring_view sv);
};

// WIN32_FILE_ATTRIBUTE_DATA and TOOLINFO are defined in WindowsCompat.h
