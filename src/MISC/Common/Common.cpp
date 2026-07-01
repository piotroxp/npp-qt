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

// Ported to Qt6

#include "Common.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>
#include <sstream>

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFontInfo>
#include <QMessageBox>
#include <QPalette>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QStringConverter>
#include "NppTextCodec.h"
#include <QUrl>

#include "Buffer.h"

// using namespace std; -- not used in Qt port

// ============================================================================
// Utility functions
// ============================================================================

void printInt(int int2print)
{
    qDebug().nospace() << "[Npp Qt] printInt:" << int2print;
}

void printStr(const QString& str2print)
{
    qDebug().nospace() << "[Npp Qt] printStr:" << str2print;
}

QString commafyInt(size_t n)
{
    QLocale locale;
    return locale.toString(static_cast<qulonglong>(n));
}

// ============================================================================
// File I/O
// ============================================================================

QByteArray getFileContent(const QString& file2read, bool* pbFailed)
{
    if (pbFailed)
        *pbFailed = false;

    if (!doesFileExist(file2read, 0, nullptr))
    {
        if (pbFailed)
            *pbFailed = true;
        return QByteArray();
    }

    QFile file(file2read);
    if (!file.open(QIODevice::ReadOnly))
    {
        if (pbFailed)
            *pbFailed = true;
        return QByteArray();
    }

    QByteArray data = file.readAll();
    file.close();
    return data;
}

QString relativeFilePathToFullFilePath(const QString& relativeFilePath)
{
    QFileInfo fi(relativeFilePath);
    if (fi.isAbsolute())
        return relativeFilePath;
    return fi.absoluteFilePath();
}

void writeFileContent(const QString& file2write, const QByteArray& content2write)
{
    QFile file(file2write);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(content2write);
        file.close();
    }
}

void writeLog(const QString& logFileName, const QString& log2write)
{
    QFile file(logFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QDateTime now = QDateTime::currentDateTime();
        QString line = now.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        line += QStringLiteral("  ");
        line += log2write;
        line += QStringLiteral("\n");
        file.write(line.toUtf8());
        file.close();
    }
}

void writeLog(const QString& logFileName, const char* log2write)
{
    writeLog(logFileName, QString::fromLatin1(log2write));
}

// ============================================================================
// Folder browsing
// ============================================================================

QString folderBrowser(QWidget* parent, const QString& title, int /*outputCtrlID*/,
                     const QString& defaultStr)
{
    Q_UNUSED(parent);
    QString dir = defaultStr.isEmpty() ? QDir::homePath() : defaultStr;
    return QFileDialog::getExistingDirectory(parent, title.isEmpty() ? QStringLiteral("Select a folder") : title, dir);
}

QString getFolderName(QWidget* parent, const QString& defaultDir)
{
    return folderBrowser(parent, QStringLiteral("Select a folder"), 0, defaultDir);
}

// ============================================================================
// Coordinate conversion
// ============================================================================

void ClientRectToScreenRect(QWidget* widget, QRect& rect)
{
    QPoint topLeft(rect.topLeft());
    QPoint bottomRight(rect.bottomRight());
    topLeft = widget->mapToGlobal(topLeft);
    bottomRight = widget->mapToGlobal(bottomRight);
    rect = QRect(topLeft, bottomRight);
}

void ScreenRectToClientRect(QWidget* widget, QRect& rect)
{
    QPoint topLeft(rect.topLeft());
    QPoint bottomRight(rect.bottomRight());
    topLeft = widget->mapFromGlobal(topLeft);
    bottomRight = widget->mapFromGlobal(bottomRight);
    rect = QRect(topLeft, bottomRight);
}

// ============================================================================
// String utilities
// ============================================================================

QVector<QString> tokenizeString(const QString& tokenString, const QChar delim)
{
    return tokenString.split(delim, Qt::SkipEmptyParts);
}

bool isInList(const QString& token, const QString& list)
{
    if (token.isEmpty() || list.isEmpty())
        return false;

    QVector<QString> tokens = list.split(' ', Qt::SkipEmptyParts);
    for (const QString& word : tokens) {
        if (token.compare(word, Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

QString purgeMenuItemString(const QString& menuItemStr, bool keepAmpersand)
{
    QString result;
    for (int k = 0; k < menuItemStr.length(); ++k)
    {
        if (menuItemStr[k] == '\t')
        {
            break; // stop at tab
        }
        else if (menuItemStr[k] == '&')
        {
            if (keepAmpersand)
                result.push_back(menuItemStr[k]);
            // else skip
        }
        else
        {
            result.push_back(menuItemStr[k]);
        }
    }
    return result;
}

// ============================================================================
// WcharMbcsConvertor
// ============================================================================

// Static result string for returning const wchar_t* without dangling pointers
static QString _staticResultStr;

const wchar_t* WcharMbcsConvertor::char2wchar(const char* mbcs2Convert, int codepage,
    int lenMbcs, int* pLenWc, int* pBytesNotProcessed)
{
    if (!mbcs2Convert)
    {
        _staticResultStr.clear();
        if (pLenWc) *pLenWc = 0;
        if (pBytesNotProcessed) *pBytesNotProcessed = 0;
        return _staticResultStr.isEmpty() ? nullptr : reinterpret_cast<const wchar_t*>(_staticResultStr.utf16());
    }

    if (lenMbcs == 0 || (lenMbcs == -1 && mbcs2Convert[0] == 0))
    {
        _staticResultStr.clear();
        if (pLenWc) *pLenWc = 0;
        if (pBytesNotProcessed) *pBytesNotProcessed = 0;
        return nullptr;
    }

    // Qt6: use QTextDecoder for codepage conversion
    QTextCodec* codec = QTextCodec::codecForMib(codepage);
    if (!codec)
        codec = QTextCodec::codecForName("UTF-8");
    if (!codec)
    {
        _staticResultStr.clear();
        return nullptr;
    }

    QTextCodec::ConverterState state;
    int inputLen = (lenMbcs == -1) ? static_cast<int>(qstrlen(mbcs2Convert)) : lenMbcs;
    _staticResultStr = codec->toUnicode(mbcs2Convert, inputLen, &state);

    if (pLenWc)
        *pLenWc = _staticResultStr.length();
    if (pBytesNotProcessed)
        *pBytesNotProcessed = state.remainingChars;

    return reinterpret_cast<const wchar_t*>(_staticResultStr.utf16());
}

const wchar_t* WcharMbcsConvertor::char2wchar(const char* mbcs2Convert, int codepage,
    intptr_t* mstart, intptr_t* mend, int mbcsLen)
{
    if (!mbcs2Convert)
    {
        _staticResultStr.clear();
        *mstart = 0;
        *mend = 0;
        return nullptr;
    }

    if (mbcsLen == 0 || (mbcsLen == -1 && mbcs2Convert[0] == 0))
    {
        _staticResultStr.clear();
        *mstart = 0;
        *mend = 0;
        return nullptr;
    }

    QTextCodec* codec = QTextCodec::codecForMib(codepage);
    if (!codec)
        codec = QTextCodec::codecForName("UTF-8");
    if (!codec)
    {
        _staticResultStr.clear();
        *mstart = 0;
        *mend = 0;
        return nullptr;
    }

    int inputLen = (mbcsLen == -1) ? static_cast<int>(qstrlen(mbcs2Convert)) : mbcsLen;
    QTextCodec::ConverterState state;
    _staticResultStr = codec->toUnicode(mbcs2Convert, inputLen, &state);

    // Convert byte offsets to character offsets
    if (*mstart < inputLen && *mend <= inputLen)
    {
        QTextCodec::ConverterState s1, s2;
        codec->toUnicode(mbcs2Convert, static_cast<int>(*mstart), &s1);
        codec->toUnicode(mbcs2Convert, static_cast<int>(*mend), &s2);
        *mstart -= s1.remainingChars;
        *mend -= s2.remainingChars;

        if (*mstart >= _staticResultStr.length() || *mend > _staticResultStr.length())
        {
            *mstart = 0;
            *mend = 0;
        }
    }

    return reinterpret_cast<const wchar_t*>(_staticResultStr.utf16());
}

// Static result buffers (avoids dangling pointer from returning member reference)
static QByteArray _staticMbResult;

const char* WcharMbcsConvertor::wchar2char(const wchar_t* wcharStr2Convert,
    int codepage, int lenWc, int* pLenMbcs)
{
    if (!wcharStr2Convert)
    {
        _staticMbResult.clear();
        if (pLenMbcs) *pLenMbcs = 0;
        return nullptr;
    }

    _staticResultStr = QString::fromWCharArray(wcharStr2Convert, lenWc);
    QTextCodec* codec = QTextCodec::codecForMib(codepage);
    if (!codec)
        codec = QTextCodec::codecForName("UTF-8");
    if (!codec)
    {
        _staticMbResult.clear();
        if (pLenMbcs) *pLenMbcs = 0;
        return nullptr;
    }

    _staticMbResult = codec->fromUnicode(_staticResultStr);
    if (pLenMbcs)
        *pLenMbcs = _staticMbResult.size();
    return _staticMbResult.constData();
}

const char* WcharMbcsConvertor::wchar2char(const wchar_t* wcharStr2Convert,
    int codepage, intptr_t* mstart, intptr_t* mend, int wcharLenIn, int* lenOut)
{
    if (!wcharStr2Convert)
    {
        _staticMbResult.clear();
        *mstart = 0;
        *mend = 0;
        return nullptr;
    }

    _staticResultStr = QString::fromWCharArray(wcharStr2Convert);
    QStringView view(_staticResultStr);
    if (wcharLenIn != -1)
        view = QStringView(_staticResultStr).left(wcharLenIn);

    QTextCodec* codec = QTextCodec::codecForMib(codepage);
    if (!codec)
        codec = QTextCodec::codecForName("UTF-8");
    if (!codec)
    {
        _staticMbResult.clear();
        *mstart = 0;
        *mend = 0;
        return nullptr;
    }

    _staticMbResult = codec->fromUnicode(view);
    int len = _staticMbResult.size();

    // Convert character offsets to byte offsets
    if (*mstart < static_cast<intptr_t>(_staticResultStr.length()) &&
        *mend < static_cast<intptr_t>(_staticResultStr.length()))
    {
        QTextCodec::ConverterState stateStart, stateEnd;
        codec->fromUnicode(_staticResultStr.left(static_cast<int>(*mstart)), &stateStart);
        codec->fromUnicode(_staticResultStr.left(static_cast<int>(*mend)), &stateEnd);
        *mstart = stateStart.stateData().size();
        *mend = stateEnd.stateData().size();

        if (*mstart >= len || *mend > len)
        {
            *mstart = 0;
            *mend = 0;
        }
    }

    if (lenOut)
        *lenOut = len;
    return _staticMbResult.constData();
}

const char* WcharMbcsConvertor::encode(int fromCodepage, int toCodepage,
    const char* txt2Encode, int lenIn, int* pLenOut, int* pBytesNotProcessed)
{
    // Convert from source encoding to UTF-16 (QString), then to target encoding
    const wchar_t* wide = char2wchar(txt2Encode, fromCodepage, lenIn, nullptr, pBytesNotProcessed);
    if (!wide)
    {
        _staticMbResult.clear();
        if (pLenOut) *pLenOut = 0;
        return nullptr;
    }

    QTextCodec* toCodec = QTextCodec::codecForMib(toCodepage);
    if (!toCodec)
        toCodec = QTextCodec::codecForName("UTF-8");
    if (!toCodec)
    {
        _staticMbResult.clear();
        if (pLenOut) *pLenOut = 0;
        return nullptr;
    }

    _staticMbResult = toCodec->fromUnicode(_staticResultStr);
    if (pLenOut)
        *pLenOut = _staticMbResult.size();
    return _staticMbResult.constData();
}

QString WcharMbcsConvertor::toQString(const QByteArray& bytes, const char* codec)
{
    if (!codec || !codec[0])
        return QString::fromUtf8(bytes);
    QTextCodec* c = QTextCodec::codecForName(codec);
    if (c)
        return c->toUnicode(bytes);
    return QString::fromUtf8(bytes);
}

QByteArray WcharMbcsConvertor::fromQString(const QString& str, const char* codec)
{
    if (!codec || !codec[0])
        return str.toUtf8();
    QTextCodec* c = QTextCodec::codecForName(codec);
    if (c)
        return c->fromUnicode(str);
    return str.toUtf8();
}

// ============================================================================
// string2wstring / wstring2string
// ============================================================================

QString string2wstring(const QString& rString, int codepage)
{
    if (codepage == 0 || codepage == -1)
        return rString;

    QTextCodec* codec = QTextCodec::codecForMib(codepage);
    if (!codec)
        return rString;
    return codec->toUnicode(rString.toLatin1());
}

QString wstring2string(const QString& rwString, int codepage)
{
    if (codepage == 0 || codepage == -1)
        return rwString;

    QTextCodec* codec = QTextCodec::codecForMib(codepage);
    if (!codec)
        return rwString.toLatin1();
    return codec->fromUnicode(rwString);
}

// ============================================================================
// Escapes ampersands in file name for menu display
// ============================================================================

static QString convertFileName(QString::const_iterator beg, QString::const_iterator end)
{
    QString strTmp;
    for (auto it = beg; it != end; ++it)
    {
        if (*it == '&')
            strTmp.push_back('&');
        strTmp.push_back(*it);
    }
    return strTmp;
}

// ============================================================================
// intToString / uintToString
// ============================================================================

QString intToString(int val)
{
    return QString::number(val);
}

QString uintToString(unsigned int val)
{
    return QString::number(val);
}

// ============================================================================
// BuildMenuFileName
// ============================================================================

QString BuildMenuFileName(int filenameLen, unsigned int pos, const QString& filename, bool ordinalNumber)
{
    QString strTemp;

    if (ordinalNumber)
    {
        if (pos < 9)
        {
            strTemp.push_back('&');
            strTemp.push_back(QChar('1' + pos));
        }
        else if (pos == 9)
        {
            strTemp.append(QStringLiteral("1&0"));
        }
        else
        {
            int quot = (pos + 1) / 10;
            int rem = (pos + 1) % 10;
            strTemp.append(uintToString(quot));
            strTemp.push_back('&');
            strTemp.append(uintToString(rem));
        }
        strTemp.append(QStringLiteral(": "));
    }
    else
    {
        strTemp.push_back('&');
    }

    if (filenameLen > 0)
    {
        // Truncate filename to filenameLen characters
        QString truncated = filename;
        if (truncated.length() > filenameLen)
        {
            // Truncate in the middle for long paths
            int half = (filenameLen - 3) / 2;
            truncated = truncated.left(half) + QStringLiteral("...") + truncated.right(half);
        }
        strTemp.append(convertFileName(truncated.constBegin(), truncated.constEnd()));
    }
    else
    {
        // filenameLen <= 0: use full name or part of it
        if (filenameLen == 0)
        {
            // Show filename only (no path)
            QFileInfo fi(filename);
            strTemp.append(convertFileName(fi.fileName().constBegin(), fi.fileName().constEnd()));
        }
        else
        {
            // Show as much as possible
            if (filename.length() < 2048)
                strTemp.append(convertFileName(filename.constBegin(), filename.constEnd()));
            else
            {
                // Very long path: truncate in the middle
                int half = 512;
                strTemp.append(convertFileName(filename.constBegin(), filename.constBegin() + half));
                strTemp.append(QStringLiteral("..."));
                strTemp.append(convertFileName(filename.constEnd() - half, filename.constEnd()));
            }
        }
    }

    return strTemp;
}

// ============================================================================
// pathRemoveFileSpec / pathAppend
// ============================================================================

QString pathRemoveFileSpec(QString& path)
{
    if (path.isEmpty())
        return path;

    QFileInfo fi(path);
    if (fi.isDir())
        return fi.absolutePath();

    int lastBackslash = path.lastIndexOf('\\');
    int lastSlash = path.lastIndexOf('/');
    int lastSep = qMax(lastBackslash, lastSlash);

    if (lastSep == -1)
    {
        // No separator found
        if (path.length() >= 2 && path[1] == ':')
            path = path.left(2) + QStringLiteral(":");
        else
            path.clear();
    }
    else
    {
        if (lastSep == 2 && path.length() >= 3 && path[1] == ':')
            path = path.left(3); // "C:\foo" → "C:\"
        else if (lastSep == 0)
            path = path.left(1); // "\foo" → "\"
        else
            path = path.left(lastSep);
    }
    return path;
}

QString pathAppend(QString& strDest, const QString& str2append)
{
    if (strDest.isEmpty() && str2append.isEmpty())
    {
        strDest = QStringLiteral("\\");
        return strDest;
    }

    if (strDest.isEmpty())
    {
        strDest = str2append;
        return strDest;
    }

    bool destEndsWithSep = strDest.endsWith('\\') || strDest.endsWith('/');
    bool appendStartsWithSep = str2append.startsWith('\\') || str2append.startsWith('/');

    if (destEndsWithSep && appendStartsWithSep)
    {
        strDest.chop(1);
        strDest += str2append;
    }
    else if (!destEndsWithSep && !appendStartsWithSep)
    {
        strDest += '\\';
        strDest += str2append;
    }
    else
    {
        strDest += str2append;
    }

    return strDest;
}

// ============================================================================
// getCtrlBgColor
// ============================================================================

QColor getCtrlBgColor(QWidget* widget)
{
    if (!widget)
        return QColor();

    // Try to get the actual background color of the widget
    QPalette pal = widget->palette();
    QColor bg = pal.color(QPalette::Window);
    return bg;
}

// ============================================================================
// String manipulation
// ============================================================================

QString stringToUpper(QString strToConvert)
{
    return strToConvert.toUpper();
}

QString stringToLower(QString strToConvert)
{
    return strToConvert.toLower();
}

QString stringReplace(QString subject, const QString& search, const QString& replace)
{
    return subject.replace(search, replace);
}

void stringSplit(const QString& input, const QString& delimiter, QVector<QString>& output)
{
    output = input.split(delimiter, Qt::SkipEmptyParts);
}

bool str2numberVector(const QString& str2convert, QVector<size_t>& numVect)
{
    numVect.clear();

    for (QChar c : str2convert)
    {
        if (!c.isDigit() && c != ' ')
            return false;
    }

    QVector<QString> v = str2convert.split(' ', Qt::SkipEmptyParts);
    for (const QString& i : v)
    {
        if (!i.isEmpty() && i.length() < 5)
            numVect.push_back(i.toULongLong());
    }
    return true;
}

void stringJoin(const QVector<QString>& strings, const QString& separator, QString& joinedString)
{
    joinedString = strings.join(separator);
}

QString stringTakeWhileAdmissable(const QString& input, const QString& admissable)
{
    for (int i = 0; i < input.length(); ++i)
    {
        if (!admissable.contains(input[i]))
            return input.left(i);
    }
    return input;
}

// ============================================================================
// Locale
// ============================================================================

double stodLocale(const QString& str, const QLocale& loc, size_t* idx)
{
    bool ok = false;
    double result = loc.toDouble(str, &ok);
    if (!ok)
        throw std::invalid_argument("invalid stod argument");
    if (idx)
        *idx = str.length();
    return result;
}

const QLocale& getSysLocale()
{
    static const QLocale systemLocale = QLocale();
    return systemLocale;
}

// ============================================================================
// Clipboard
// ============================================================================

bool str2Clipboard(const QString& str2cpy, QWidget* /*parent*/)
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(str2cpy, QClipboard::Clipboard);
    return true;
}

QString strFromClipboard()
{
    QClipboard* clipboard = QApplication::clipboard();
    QString text = clipboard->text(QClipboard::Clipboard);

    // Trim EOL at end of string
    if (text.length() < 2)
        return text;

    // Remove trailing \r\n or \n or \r
    QChar last1 = text[text.length() - 1];
    QChar last2 = text[text.length() - 2];

    if (last2 == '\r' && last1 == '\n')
    {
        // \r\n — remove both
        return text.left(text.length() - 2);
    }
    else if ((last2 != '\r' && last2 != '\n') && (last1 == '\r' || last1 == '\n'))
    {
        // X\r or X\n — remove last char
        return text.left(text.length() - 1);
    }
    else if (last2 == '\r' || last2 == '\n')
    {
        // \r\r or \n\n or \n\r — keep them
        return text;
    }

    // length >= 3 — remove EOL at end
    QString eol2 = text.right(2);
    if (eol2 == QStringLiteral("\r\n") || eol2 == QStringLiteral("\n\r"))
    {
        return text.left(text.length() - 2);
    }
    QString eol1 = text.right(1);
    if (eol1 == QStringLiteral("\r") || eol1 == QStringLiteral("\n"))
    {
        return text.left(text.length() - 1);
    }

    return text;
}

// ============================================================================
// buf2Clipboard
// ============================================================================

bool buf2Clipboard(const QVector<Buffer*>& buffers, bool isFullPath, QWidget* /*parent*/)
{
    const QString crlf = QStringLiteral("\r\n");
    QString selection;
    for (const Buffer* buf : buffers)
    {
        if (buf)
        {
            const QString* fileName = nullptr;
            // TODO: depends on Buffer class
            // QString name = isFullPath ? buf->getFullPathName() : buf->getFileName();
            Q_UNUSED(isFullPath);
            Q_UNUSED(fileName);
        }
    }

    if (!selection.isEmpty())
        return str2Clipboard(selection, nullptr);
    return false;
}

// ============================================================================
// Pattern matching
// ============================================================================

// Convert a simple DOS-style pattern (e.g. "*.txt") to a QRegularExpression
static QRegularExpression dosPatternToRegex(const QString& pattern)
{
    QString regex;
    regex.reserve(pattern.length() * 2);

    for (int i = 0; i < pattern.length(); ++i)
    {
        QChar c = pattern[i];
        if (c == '*')
        {
            regex += QStringLiteral(".*");
        }
        else if (c == '?')
        {
            regex += '.';
        }
        else if (c == '.' || c == '+' || c == '^' || c == '$' || c == '|' ||
                 c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
                 c == '\\')
        {
            regex += '\\';
            regex += c;
        }
        else
        {
            regex += c;
        }
    }

    return QRegularExpression(regex, QRegularExpression::CaseInsensitiveOption);
}

bool matchInList(const QString& fileName, const QVector<QString>& patterns)
{
    for (const QString& pattern : patterns)
    {
        if (pattern.length() > 1 && pattern[0] == '!')
        {
            // Exclusion pattern
            QString p = pattern.mid(1); // remove '!'
            QRegularExpression re = dosPatternToRegex(p);
            if (re.match(fileName).hasMatch())
                return false;
        }
        else
        {
            QRegularExpression re = dosPatternToRegex(pattern);
            if (re.match(fileName).hasMatch())
                return true;
        }
    }
    return false;
}

bool matchInExcludeDirList(const QString& dirName, const QVector<QString>& patterns, size_t level)
{
    for (const QString& pattern : patterns)
    {
        if (pattern.length() > 3 && pattern[0] == '!' && pattern[1] == '+' &&
            (pattern[2] == '\\' || pattern[2] == '/'))
        {
            // !+\folderPattern: recursive exclusion
            QString p = pattern.mid(3);
            QRegularExpression re = dosPatternToRegex(p);
            if (re.match(dirName).hasMatch())
                return true;
        }
        else if (pattern.length() > 2 && pattern[0] == '!' &&
                 (pattern[1] == '\\' || pattern[1] == '/'))
        {
            // !\folderPattern: first-level only exclusion
            if (level == 1)
            {
                QString p = pattern.mid(2);
                QRegularExpression re = dosPatternToRegex(p);
                if (re.match(dirName).hasMatch())
                    return true;
            }
        }
    }
    return false;
}

bool allPatternsAreExclusion(const QVector<QString>& patterns)
{
    for (const QString& pattern : patterns)
    {
        if (!pattern.isEmpty() && pattern[0] != '!')
            return false;
    }
    return true;
}

// ============================================================================
// Error string
// ============================================================================

QString getLastErrorAsString(int errorCode)
{
    if (errorCode == 0)
        return QString();

    // Use QSystemLibrary or simple strerror for POSIX platforms
    // In Qt, we can use strerror
    QString msg = QStringLiteral("(Error code: %1)\n").arg(errorCode);
    return msg;
}

// ============================================================================
// File existence
// ============================================================================

bool doesFileExist(const QString& filePath, unsigned int, bool*)
{
    return QFileInfo::exists(filePath) && QFileInfo(filePath).isFile();
}

bool doesDirectoryExist(const QString& dirPath)
{
    return QFileInfo::exists(dirPath) && QFileInfo(dirPath).isDir();
}

bool doesDirectoryExist(const QString& dirPath, unsigned int, bool*)
{
    return QFileInfo::exists(dirPath) && QFileInfo(dirPath).isDir();
}
bool doesPathExist(const QString& path)
{
    return QFileInfo::exists(path);
}

bool doesPathExist(const QString& path, unsigned int, bool* exists)
{
    if (exists) *exists = QFileInfo::exists(path);
    return QFileInfo::exists(path);
}
// ============================================================================
// Path helpers
// ============================================================================

bool isWin32NamespacePrefixedFileName(const QString& fileName)
{
    return fileName.startsWith(QStringLiteral("\\\\?\\")) ||
           fileName.startsWith(QStringLiteral("//?/"));
}

bool isUnsupportedFileName(const QString& fileName)
{
    if (fileName.isEmpty() || fileName.length() >= 260)
        return true;

    // Strip namespace prefix if present
    QString name = fileName;
    if (isWin32NamespacePrefixedFileName(name))
        name = name.mid(4);

    // Check for trailing space or dot (namespace prefix case)
    if (name.endsWith('.') && isWin32NamespacePrefixedFileName(fileName))
        return false; // allowed via namespace prefix
    if (name.endsWith(' '))
        return true;

    // Check for invalid ASCII characters
    for (QChar c : name)
    {
        if (c.unicode() <= 31)
            return true;
        if (c == '<' || c == '>' || c == '"' || c == '|')
            return true;
    }

    // Extract filename without path
    int pos = name.lastIndexOf('\\');
    if (pos == -1)
        pos = name.lastIndexOf('/');
    QString fileNameOnly = (pos == -1) ? name : name.mid(pos + 1);
    if (fileNameOnly.endsWith('.'))
        fileNameOnly.chop(1);

    if (fileNameOnly.length() > 4)
        return false;

    // Check reserved names
    QString upper = fileNameOnly.toUpper();
    static const QStringList reserved = {
        QStringLiteral("CON"),  QStringLiteral("PRN"),  QStringLiteral("AUX"),
        QStringLiteral("NUL"),
        QStringLiteral("COM1"), QStringLiteral("COM2"), QStringLiteral("COM3"),
        QStringLiteral("COM4"), QStringLiteral("COM5"), QStringLiteral("COM6"),
        QStringLiteral("COM7"), QStringLiteral("COM8"), QStringLiteral("COM9"),
        QStringLiteral("LPT1"), QStringLiteral("LPT2"), QStringLiteral("LPT3"),
        QStringLiteral("LPT4"), QStringLiteral("LPT5"), QStringLiteral("LPT6"),
        QStringLiteral("LPT7"), QStringLiteral("LPT8"), QStringLiteral("LPT9")
    };

    return reserved.contains(upper);
}

bool isCoreWindows()
{
    // Qt: detect Windows Core (Server Core) — stub, always return false on non-Windows
    // On Windows, this would check the registry
    return false;
}

bool isCertificateValidated(const QString& /*fullFilePath*/, const QString& /*subjectName2check*/)
{
    // Certificate validation requires Windows CryptoAPI — stub
    return false;
}

bool isAssoCommandExisting(const QString& fullPathName)
{
    // Check if file has a shell association that opens something other than Notepad++
    // Qt: use QSettings to check file associations
    QFileInfo fi(fullPathName);
    QString ext = fi.suffix();
    if (ext.isEmpty())
        return false;

    QSettings assoc( QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.%1\\UserChoice"),
                     QSettings::NativeFormat);
    QString progId = assoc.value(QStringLiteral("ProgId")).toString();
    if (progId.isEmpty())
        return false;

    QSettings cmd( QStringLiteral("HKEY_CURRENT_USER\\Software\\Classes\\%1\\shell\\open\\command").arg(progId),
                   QSettings::NativeFormat);
    QString cmdStr = cmd.value(QStringLiteral(".")).toString();

    return !cmdStr.contains(QStringLiteral("notepad++"), Qt::CaseInsensitive);
}

bool deleteFileOrFolder(const QString& f2delete)
{
    QFileInfo fi(f2delete);
    if (fi.isDir())
    {
        QDir dir(f2delete);
        return dir.removeRecursively();
    }
    else
    {
        return QFile::remove(f2delete);
    }
}

void getFilesInFolder(QVector<QString>& files, const QString& extTypeFilter, const QString& inFolder)
{
    QDir dir(inFolder);
    QStringList filters;
    if (extTypeFilter == QStringLiteral("*.*") || extTypeFilter == QStringLiteral("*"))
        filters = { QStringLiteral("*") };
    else
        filters = extTypeFilter.split(';', Qt::SkipEmptyParts);

    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    for (const QFileInfo& fi : dir.entryInfoList())
    {
        files.push_back(fi.absoluteFilePath());
    }
}

void getFilesInFolder(std::vector<std::wstring>& files, const std::wstring& extTypeFilter, const std::wstring& inFolder)
{
    QDir dir(QString::fromStdWString(inFolder));
    QStringList filters;
    QString ext = QString::fromStdWString(extTypeFilter);
    if (ext == QStringLiteral("*.*") || ext == QStringLiteral("*"))
        filters = { QStringLiteral("*") };
    else
        filters = ext.split(';', Qt::SkipEmptyParts);

    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    for (const QFileInfo& fi : dir.entryInfoList())
    {
        files.push_back(fi.absoluteFilePath().toStdWString());
    }
}

void trim(QString& str)
{
    str = str.trimmed();
}

int nbDigitsFromNbLines(size_t nbLines)
{
    int nbDigits = 0;
    if (nbLines < 10) nbDigits = 1;
    else if (nbLines < 100) nbDigits = 2;
    else if (nbLines < 1000) nbDigits = 3;
    else if (nbLines < 10000) nbDigits = 4;
    else if (nbLines < 100000) nbDigits = 5;
    else if (nbLines < 1000000) nbDigits = 6;
    else
    {
        nbDigits = 7;
        size_t n = nbLines / 10000000;
        while (n)
        {
            n /= 10;
            ++nbDigits;
        }
    }
    return nbDigits;
}

QString getDateTimeStrFrom(const QString& dateTimeFormat, const QDateTime& dt)
{
    return dt.toString(dateTimeFormat);
}

// ============================================================================
// Font
// ============================================================================
// createFont is defined inline in Common.h — no-op here (remove redefinition)

bool removeReadOnlyFlagFromFileAttributes(const QString& fileFullPath)
{
    QFile::Permissions perms = QFile::permissions(fileFullPath);
    return QFile::setPermissions(fileFullPath, perms | QFile::ReadOwner | QFile::ReadUser);
}

bool toggleReadOnlyFlagFromFileAttributes(const QString& fileFullPath, bool& isChangedToReadOnly)
{
    QFile::Permissions perms = QFile::permissions(fileFullPath);
    bool currentlyReadOnly = !(perms & QFile::WriteOwner);

    QFile::Permissions newPerms;
    if (currentlyReadOnly)
        newPerms = perms | QFile::WriteOwner | QFile::WriteUser;
    else
        newPerms = perms & ~(QFile::WriteOwner | QFile::WriteUser);

    bool ok = QFile::setPermissions(fileFullPath, newPerms);
    if (ok)
        isChangedToReadOnly = !currentlyReadOnly;
    return ok;
}

// ============================================================================
// Version
// ============================================================================

Version::Version(const QString& versionStr)
{
    try
    {
        QVector<QString> ss = versionStr.split('.', Qt::SkipEmptyParts);

        if (ss.size() > 4)
            throw QString(versionStr);

        if (ss.size() > 0) _major = ss[0].toInt();
        if (ss.size() > 1) _minor = ss[1].toInt();
        if (ss.size() > 2) _patch = ss[2].toInt();
        if (ss.size() > 3) _build = ss[3].toInt();
    }
    catch (...) {
        _major = 0;
        _minor = 0;
        _patch = 0;
        _build = 0;
    }
}

Version::Version(const std::wstring& versionStr)
    : Version(QString::fromWCharArray(versionStr.c_str()))
{}

void Version::setVersionFrom(const QString& filePath)
{
    if (filePath.isEmpty() || !doesFileExist(filePath))
        return;
    // Qt has no built-in way to read PE file version — would need QLibrary or external
    Q_UNUSED(filePath);
}

QString Version::toString() const
{
    if (_build == 0 && _patch == 0 && _minor == 0 && _major == 0)
        return QString();
    else if (_build == 0 && _patch == 0 && _minor == 0)
        return QString::number(_major);
    else if (_build == 0 && _patch == 0)
        return QStringLiteral("%1.%2").arg(_major).arg(_minor);
    else if (_build == 0)
        return QStringLiteral("%1.%2.%3").arg(_major).arg(_minor).arg(_patch);
    return QStringLiteral("%1.%2.%3.%4").arg(_major).arg(_minor).arg(_patch).arg(_build);
}

int Version::compareTo(const Version& v2c) const
{
    if (_major > v2c._major) return 1;
    else if (_major < v2c._major) return -1;
    else {
        if (_minor > v2c._minor) return 1;
        else if (_minor < v2c._minor) return -1;
        else {
            if (_patch > v2c._patch) return 1;
            else if (_patch < v2c._patch) return -1;
            else {
                if (_build > v2c._build) return 1;
                else if (_build < v2c._build) return -1;
                else return 0;
            }
        }
    }
}

bool Version::isCompatibleTo(const Version& from, const Version& to) const
{
    return isCompatibleTo(from, true, to, true);
}

bool Version::isCompatibleTo(const Version& minVer, bool minInclusive, const Version& maxVer, bool maxInclusive) const
{
    if (empty())
        return false;
    if (minVer.empty() && maxVer.empty())
        return true;
    if (minVer.empty() && (maxInclusive ? *this <= maxVer : *this < maxVer))
        return true;
    if (maxVer.empty() && (minInclusive ? *this >= minVer : *this > minVer))
        return true;
    bool aboveMin = minInclusive ? *this >= minVer : *this > minVer;
    bool belowMax = maxInclusive ? *this <= maxVer : *this < maxVer;
    return aboveMin && belowMax;
}

// ScopedCOMInit is a Windows COM stub — no-op on Linux
// class ScopedCOMInit { public: ScopedCOMInit() {} ~ScopedCOMInit() {} };

bool ControlInfoTip::init(QWidget*, QWidget*, const QString&, bool, unsigned int, int)
{
    _widget = nullptr;
    return false;
}
void ControlInfoTip::show(showPosition) const {}
void ControlInfoTip::hide() { _widget = nullptr; }

bool fileTimeToYMD(const QDateTime&, int& yyyymmdd)
{
    yyyymmdd = 0;
    return false;
}

void expandEnv(QString& path2Expand)
{
    const auto env = QProcessEnvironment::systemEnvironment();
    for (const QString& key : env.keys()) {
        path2Expand.replace(QStringLiteral("%") + key + QStringLiteral("%"), env.value(key));
    }
}

std::wstring GetLastErrorAsString(unsigned long errorCode)
{
    if (errorCode == 0)
        return std::wstring(L"No error");
    // On non-Windows, return a generic error string
    return std::wstring(L"Error code: ") + std::to_wstring(errorCode);
}
