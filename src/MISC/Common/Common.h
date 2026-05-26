// Common.h - Cross-platform Qt6 utilities (replaces Windows MFC)
#pragma once
#include "WindowsCompat.h"
#include <QString>
#include <QStringList>
#include <QList>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QClipboard>
#include <QColor>
#include <QFont>
#include <QWidget>
#include <QLabel>
#include <QFontMetrics>
#include <QLayout>
#include <QRegularExpression>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>

class Buffer; // forward declaration

// === WcharMbcsConvertor ===
class WcharMbcsConvertor {
public:
    const QString& char2wchar(const QByteArray& mbcs2Convert, const QString& codepage);
    const QString& wchar2char(const QString& wcharStr2Convert, const QString& codepage);
    QString encode(const QString& fromCodepage, const QString& toCodepage, const QString& txt2Encode);
private:
    QString _wideCharStr;
    QString _multiByteStr;
};

// === ControlInfoTip ===
class ControlInfoTip {
public:
    enum showPosition { right, left };
    bool init(QWidget* ctrl2attached, QWidget* ctrl2attachedParent,
              const QString& tipStr, bool isRTL,
              unsigned int remainTimeMillisecond, int maxWidth);
    void show(showPosition pos) const;
    void hide();
private:
    QString _tipStr;
    QLabel* _tooltip = nullptr;
    QWidget* _ctrl = nullptr;
    QWidget* _ctrlParent = nullptr;
};

// === Win32_IO_File ===
class Win32_IO_File {
public:
    explicit Win32_IO_File(const QString& fname);
    void close();
    bool write(const void* wbuf, size_t buf_size);
    bool isOpened() const { return _file.isOpen(); }
    bool writeStr(const QString& str);
    int getErrorCode() const { return _dwErrorCode; }
private:
    QString _path;
    QFile _file;
    int _dwErrorCode = 0;
    bool _written = false;
};

// === Free functions ===
QString folderBrowser(QWidget* parent, const QString& title = "", int outputCtrlID = 0, const QString& defaultStr = "");
QString getFolderName(QWidget* parent, const QString& defaultDir = "");

void printInt(int int2print);
void printStr(const QString& str2print);

QString commafyInt(size_t n);
QString purgeMenuItemString(const QString& menuItemStr, bool keepAmpersand = false);
QStringList tokenizeString(const QString& tokenString, const QChar delim);

QString string2wstring(const QString& rString, const char* codepage = "UTF-8");
QString wstring2string(const QString& rwString, const char* codepage = "UTF-8");

bool isInList(const QString& token, const QString& list);
QString buildMenuFileName(int filenameLen, unsigned int pos, const QString& filename, bool ordinalNumber = true);
bool isInList(const char* token, const char* list);
QString buildMenuFileName(int filenameLen, unsigned int pos, const char* filename, bool ordinalNumber = true);

void ClientRectToScreenRect(QWidget* hWnd, RECT* rect);
void ScreenRectToClientRect(QWidget* hWnd, RECT* rect);

QString pathRemoveFileSpec(QString& path);
QString pathAppend(QString& strDest, const QString& str2append);

QByteArray getFileContent(const QString& file2read, bool* pbFailed = nullptr);
QString relativeFilePathToFullFilePath(const QString& relativeFilePath);
void writeFileContent(const QString& file2write, const QString& content2write);
bool matchInList(const QString& fileName, const QString& patterns);
bool matchInExcludeDirList(const QString& dirName, const std::vector<QString>& patterns, size_t level);
bool allPatternsAreExclusion(const std::vector<QString>& patterns);

void writeLog(const QString& logFileName, const char* log2write);
void writeLog(const QString& logFileName, const QString& log2write);

QColor getCtrlBgColor(QWidget* hWnd);
QString stringToUpper(QString strToConvert);
QString stringToLower(QString strToConvert);
QString stringReplace(QString subject, const QString& search, const QString& replace);
void stringSplit(const QString& input, const QString& delimiter, QStringList& output);
bool str2numberVector(QString str2convert, QList<size_t>& numVect);
void stringJoin(const QStringList& strings, const QString& separator, QString& joinedString);
QString stringTakeWhileAdmissable(const QString& input, const QString& admissable);
void trim(QString& str);

bool str2Clipboard(const QString& str2cpy, QWidget* hwnd = nullptr);
QString strFromClipboard();
bool buf2Clipboard(const QList<Buffer*>&, bool, QWidget*);

QString getLastErrorAsString(int errorCode = 0);
QString intToString(int val);
QString uintToString(unsigned int val);
int nbDigitsFromNbLines(size_t nbLines);

bool doesFileExist(const QString& filePath);
bool doesDirectoryExist(const QString& dirPath);
bool doesPathExist(const QString& path);
bool removeReadOnlyFlagFromFileAttributes(const QString& fileFullPath);
bool toggleReadOnlyFlagFromFileAttributes(const QString& fileFullPath, bool& isChangedToReadOnly);

void getFilesInFolder(QStringList& files, const QString& extTypeFilter, const QString& inFolder);
bool deleteFileOrFolder(const QString& f2delete);
bool isWin32NamespacePrefixedFileName(const QString& fileName);
bool isUnsupportedFileName(const QString& fileName);
bool isWindowVisibleOnAnyMonitor(const RECT& rectWndIn);
bool isCoreWindows();

// === Version ===
class Version {
public:
    Version() = default;
    explicit Version(const QString& versionStr);
    void setVersionFrom(const QString& filePath);
    QString toString() const;
    static bool isNumber(const QString& s);
    int compareTo(const Version& v2c) const;
    bool operator<(const Version& v2c) const { return compareTo(v2c) == -1; }
    bool operator==(const Version& v2c) const { return compareTo(v2c) == 0; }
    bool empty() const { return _major == 0 && _minor == 0 && _patch == 0 && _build == 0; }
private:
    unsigned long _major = 0, _minor = 0, _patch = 0, _build = 0;
};

// === Template utilities ===
template<typename T>
size_t vecRemoveDuplicates(std::vector<T>& vec, bool isSorted = false, bool canSort = false) {
    if (!isSorted && canSort) {
        std::sort(vec.begin(), vec.end());
        isSorted = true;
    }
    if (isSorted) {
        auto it = std::unique(vec.begin(), vec.end());
        vec.resize(std::distance(vec.begin(), it));
    } else {
        std::unordered_set<T> seen;
        auto newEnd = std::remove_if(vec.begin(), vec.end(),
            [&seen](const T& value) { return !seen.insert(value).second; });
        vec.erase(newEnd, vec.end());
    }
    return vec.size();
}
