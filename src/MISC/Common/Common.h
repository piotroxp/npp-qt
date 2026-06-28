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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QLocale>
#include <QtCore/QMap>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QRegularExpression>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/Qt>
#include <QtGui/QFont>
#include <QtWidgets/QWidget>

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <unordered_set>
#include <vector>

// Forward declarations
class Buffer;
#include "ScintillaEditView.h"  // provides `using ScintillaEditView = ScintillaComponent;`
class Window;
struct recordedMacroStep;

QString folderBrowser(QWidget* parent, const QString& title = QString(),
                      int outputCtrlID = 0, const QString& defaultStr = QString());
QString getFolderName(QWidget* parent, const QString& defaultDir = QString());

void printInt(int value);
void printStr(const QString& str);
QString commafyInt(size_t n);

void writeLog(const QString& logFileName, const char* log);
void writeLog(const QString& logFileName, const QString& log);

QString purgeMenuItemString(const QString& menuItemStr, bool keepAmpersand = false);
QStringList tokenizeString(const QString& tokenString, const QChar delim);

void clientRectToScreen(QWidget* widget, QRect& rect);
void screenRectToClient(QWidget* widget, QRect& rect);

QString stringToWString(const QString& rString, const char* codec = "UTF-8");
QString wstringToString(const QString& rString, const char* codec = "UTF-8");

// Legacy names for source compatibility
inline QString string2wstring(const std::string& s, uintptr_t = 0) {
    return QString::fromLatin1(s.c_str());
}

// QString → QString (pass-through for Qt API; encoding codepage is ignored)
inline QString string2wstring(const QString& s, uintptr_t /*codepage*/ = 0) {
    return s;
}

inline std::string wstring2string(const QString& s, uintptr_t = 0) {
    return s.toLatin1().constData();
}

bool isInList(const QString& token, const QString& list);
QString buildMenuFileName(int filenameLen, unsigned int pos,
                          const QString& filename, bool ordinalNumber = true);

QByteArray getFileContent(const QString& file2read, bool* pbFailed = nullptr);
QString relativeFilePathToFullFilePath(const QString& relativeFilePath);
void writeFileContent(const QString& file2write, const QByteArray& content2write);

bool matchInList(const QString& fileName, const QStringList& patterns);
bool matchInExcludeDirList(const QString& dirName, const QStringList& patterns, size_t level);
bool allPatternsAreExclusion(const QStringList& patterns);

class WcharMbcsConvertor {
public:
    // Qt6 convenience helpers (defined in Common.cpp)
    static QString toQString(const QByteArray& bytes, const char* codec);
    static QByteArray fromQString(const QString& str, const char* codec);

    static WcharMbcsConvertor& getInstance() {
        static WcharMbcsConvertor instance;
        return instance;
    }

    const wchar_t* char2wchar(const char* mbcs, int codePage, int lenMbcs = -1,
                              int* pLenWc = nullptr, int* pBytesNotProcessed = nullptr);
    const wchar_t* char2wchar(const char* mbcs, int codePage,
                              intptr_t* mstart, intptr_t* mend, int mbcsLen = 0);
    size_t getSizeW() const { return _wideCharStr.size(); }

    const char* wchar2char(const wchar_t* wcharStr, int codePage, int lenWc = -1,
                            int* pLenMbcs = nullptr);
    const char* wchar2char(const wchar_t* wcharStr, int codePage,
                            intptr_t* mstart, intptr_t* mend, int wcharLenIn = 0,
                            int* lenOut = nullptr);
    size_t getSizeA() const { return _multiByteStr.size(); }

    const char* encode(int fromCodepage, int toCodepage, const char* txt2Encode,
                        int lenIn = -1, int* pLenOut = nullptr, int* pBytesNotProcessed = nullptr);

private:
    WcharMbcsConvertor() = default;
    ~WcharMbcsConvertor() = default;
    WcharMbcsConvertor(const WcharMbcsConvertor&) = delete;
    WcharMbcsConvertor& operator=(const WcharMbcsConvertor&) = delete;

    template <typename T>
    class StringBuffer {
    public:
        ~StringBuffer() { if (_allocLen) delete[] _str; }

        void sizeTo(size_t size) {
            if (_allocLen < size + 1) {
                if (_allocLen) delete[] _str;
                _allocLen = std::max<size_t>(size + 1, initSize);
                _str = new T[_allocLen] {};
            }
            _dataLen = size;
        }

        void empty() {
            if (_allocLen == 0) {
                _str = &_nullStr;
            } else {
                _str[0] = 0;
            }
            _dataLen = 0;
        }

        size_t size() const { return _dataLen; }
        operator T* () { return _str; }
        operator const T* () const { return _str; }

    private:
        static constexpr int initSize = 1024;
        size_t _allocLen = 0;
        size_t _dataLen = 0;
        T* _str = nullptr;
        static T _nullStr;
    };

    StringBuffer<char> _multiByteStr;
    StringBuffer<wchar_t> _wideCharStr;
};

template <> /*static*/ wchar_t WcharMbcsConvertor::StringBuffer<wchar_t>::_nullStr = 0;

QString pathRemoveFileSpec(QString& path);
QString pathAppend(QString& strDest, const QString& str2append);
// Overload for std::wstring + const wchar_t* (Windows compatibility shim)
inline std::wstring& pathAppend(std::wstring& strDest, const wchar_t* str2append)
{
    strDest += str2append;
    return strDest;
}

// Overload for std::wstring + std::wstring
inline std::wstring& pathAppend(std::wstring& strDest, const std::wstring& str2append)
{
    strDest += str2append;
    return strDest;
}
QColor getCtrlBgColor(QWidget* widget);
QString stringToUpper(QString strToConvert);
QString stringToLower(QString strToConvert);
QString stringReplace(QString subject, const QString& search, const QString& replace);
void stringSplit(const QString& input, const QString& delimiter, QStringList& output);
bool str2numberVector(const QString& str2convert, QVector<size_t>& numVect);
void stringJoin(const QStringList& strings, const QString& separator, QString& joinedString);
QString stringTakeWhileAdmissible(const QString& input, const QString& admissible);
double stodLocale(const QString& str, size_t* idx = nullptr);

const QLocale& getSysLocale();

bool str2Clipboard(const QString& str2cpy, QWidget* widget = nullptr);
QString strFromClipboard(QWidget* widget = nullptr);
bool buf2Clipboard(const QVector<Buffer*>& buffers, bool isFullPath, QWidget* widget = nullptr);

QString getLastErrorAsString(int errorCode = 0);

QString intToString(int val);
QString uintToString(unsigned int val);

// Tooltip stubs — use QToolTip instead
inline QWidget* createToolTip(int, QWidget*, QWidget*, const QString&, bool = false) {
    return nullptr; // Use QToolTip::showText() directly
}
inline QWidget* createToolTipRect(int, QWidget*, QWidget*, const QString&, const QRect&) {
    return nullptr;
}

// Certificate validation (stub)
bool isCertificateValidated(const QString& fullFilePath, const QString& subjectName2check);
// File association (stub on non-Win32)
bool isAssoCommandExisting(const QString& fullPathName);

// File/folder deletion
bool deleteFileOrFolder(const QString& f2delete);

void getFilesInFolder(QStringList& files, const QString& extTypeFilter, const QString& inFolder);

template <typename T>
size_t vecRemoveDuplicates(QVector<T>& vec, bool isSorted = false, bool canSort = false)
{
    if (!isSorted && canSort) {
        std::sort(vec.begin(), vec.end());
        isSorted = true;
    }

    if (isSorted) {
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    } else {
        std::unordered_set<T> seen;
        auto newEnd = std::remove_if(vec.begin(), vec.end(),
            [&seen](const T& value) { return !seen.insert(value).second; });
        vec.erase(newEnd, vec.end());
    }
    return static_cast<size_t>(vec.size());
}

void trim(QString& str);

int nbDigitsFromNbLines(size_t nbLines);

QString getDateTimeStrFrom(const QString& dateTimeFormat, const QDateTime& dt);

// Font helpers (stub on Qt — use QFont directly)
inline QFont createFont(const QString& fontName, int fontSize, bool isBold,
                         QWidget* = nullptr) {
    QFont f(fontName);
    f.setPointSize(fontSize);
    f.setBold(isBold);
    return f;
}

bool removeReadOnlyFlagFromFileAttributes(const QString& fileFullPath);
bool toggleReadOnlyFlagFromFileAttributes(const QString& fileFullPath, bool& isChangedToReadOnly);

bool isWin32NamespacePrefixedFileName(const QString& fileName);
inline bool isWin32NamespacePrefixedFileName(const char*) { return false; }
bool isUnsupportedFileName(const QString& fileName);
inline bool isUnsupportedFileName(const char*) { return false; }

class Version {
public:
    Version() = default;
    explicit Version(const QString& versionStr);
    explicit Version(const std::wstring& versionStr);

    void setVersionFrom(const QString& filePath);
    QString toString() const;
    static bool isNumber(const QString& s) {
        if (s.isEmpty()) return false;
        return std::none_of(s.begin(), s.end(),
            [](auto c) { return !c.isDigit(); });
    }

    int compareTo(const Version& v2c) const;
    bool operator<(const Version& v2c) const { return compareTo(v2c) == -1; }
    bool operator<=(const Version& v2c) const { int r = compareTo(v2c); return r == -1 || r == 0; }
    bool operator>(const Version& v2c) const { return compareTo(v2c) == 1; }
    bool operator>=(const Version& v2c) const { int r = compareTo(v2c); return r == 1 || r == 0; }
    bool operator==(const Version& v2c) const { return compareTo(v2c) == 0; }
    bool operator!=(const Version& v2c) const { return compareTo(v2c) != 0; }

    bool empty() const { return _major == 0 && _minor == 0 && _patch == 0 && _build == 0; }
    bool isCompatibleTo(const Version& from, const Version& to) const;
    bool isCompatibleTo(const Version& minVer, bool minInclusive, const Version& maxVer, bool maxInclusive) const;

    // Accessors for private members
    int major() const { return _major; }
    int minor() const { return _minor; }
    int patch() const { return _patch; }
    int build() const { return _build; }

private:
    int _major = 0;
    int _minor = 0;
    int _patch = 0;
    int _build = 0;
};

// File/directory existence (timeout variants — Qt always synchronous)
bool doesFileExist(const QString& filePath, unsigned int = 0, bool* = nullptr);
inline bool doesFileExist(const char* filePath, unsigned int timeout = 0, bool* timedOut = nullptr) {
    return doesFileExist(QString::fromUtf8(filePath), timeout, timedOut);
}
inline bool doesFileExist(const wchar_t* filePath, unsigned int timeout = 0, bool* timedOut = nullptr) {
    return doesFileExist(QString::fromStdWString(filePath), timeout, timedOut);
}
bool doesDirectoryExist(const QString& dirPath, unsigned int = 0, bool* = nullptr);
bool doesPathExist(const QString& path, unsigned int = 0, bool* = nullptr);

// Disk space (stub — use QStorageInfo)
inline bool getDiskFreeSpaceWithTimeout(const QString&, quint64*,
                                        unsigned int = 0, bool* = nullptr) { return false; }

// Window visibility (stub on non-Win32)
bool isWindowVisibleOnAnyMonitor(const QRect& rect);

// OS detection
bool isCoreWindows();

// Tooltip widget (stub — use QToolTip)
class ControlInfoTip {
public:
    ControlInfoTip() = default;
    ~ControlInfoTip() { hide(); }

    bool init(QWidget* ctrl2attached, QWidget* ctrl2attachedParent,
              const QString& tipStr, bool isRTL = false,
              unsigned int remainTimeMs = 0, int maxWidth = 200);
    bool isValid() const { return _widget != nullptr; }
    QWidget* getTipWidget() const { return _widget; }
    enum class showPosition { beginning, middle, end };
    void show(showPosition pos = showPosition::middle) const;
    void hide();

private:
    QWidget* _widget = nullptr;
};

// UAC elevation (stub on non-Win32)
inline int invokeNppUacOp(const QString&) { return 0; }
bool fileTimeToYMD(const QDateTime& ft, int& yyyymmdd);
void expandEnv(QString& path2Expand);
std::wstring GetLastErrorAsString(unsigned long errorCode = 0);

// Windows-safe wcscpy_s shim → std::wcscpy on non-Win32
#ifndef wcscpy_s
inline wchar_t* wcscpy_s(wchar_t* dest, size_t destSize, const wchar_t* src) {
    return std::wcscpy(dest, src);
}
inline wchar_t* wcscpy_s(wchar_t* dest, const wchar_t* src) {
    return std::wcscpy(dest, src);
}
#endif

// =============================================================================
// Win32 Shell / CRT API stubs for Linux
// These provide Windows-compatible interfaces without the Windows headers.
// =============================================================================

// Windows Shell PathFindExtension — find the file extension in a path
inline const wchar_t* PathFindExtension(const wchar_t* path) {
    if (!path) return path;
    const wchar_t* ext = wcsrchr(path, L'.');
    return ext ? ext : path + wcslen(path);
}
inline QString PathFindExtension(const QString& path) {
    int dot = path.lastIndexOf('.');
    return dot >= 0 ? path.mid(dot) : QString();
}

// Windows CRT _wfopen — wide-character file open, mapped to POSIX fopen
inline FILE* _wfopen(const wchar_t* path, const wchar_t* mode) {
    return fopen(QString::fromWCharArray(path).toLocal8Bit().constData(),
                 QString::fromWCharArray(mode).toLocal8Bit().constData());
}
