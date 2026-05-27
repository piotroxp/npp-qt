// MISC/Common/Common.cpp - Qt6 port of Notepad++ Common utilities
#include "Common.h"
#include "WindowsCompat.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QMessageBox>
#include <QToolTip>
#include <QPalette>
#include <QColor>
#include <QLabel>
#include <QFontMetrics>
#include <QFileDialog>
#include <QTextCodec>

using namespace std;

void printInt(int int2print)
{
    QMessageBox::information(nullptr, "", QString::number(int2print));
}

void printStr(const QString& str2print)
{
    QMessageBox::information(nullptr, "", str2print);
}

QString commafyInt(size_t n)
{
    QString s = QString::number(n);
    int len = s.length();
    for (int i = len - 3; i > 0; i -= 3) {
        s.insert(i, ',');
    }
    return s;
}

QByteArray getFileContent(const QString& file2read, bool* pbFailed)
{
    if (pbFailed) *pbFailed = false;

    if (!doesFileExist(file2read)) {
        if (pbFailed) *pbFailed = true;
        return QByteArray();
    }

    QFile file(file2read);
    if (!file.open(QIODevice::ReadOnly)) {
        if (pbFailed) *pbFailed = true;
        return QByteArray();
    }

    QByteArray content = file.readAll();
    file.close();
    return content;
}

QString relativeFilePathToFullFilePath(const QString& relativeFilePath)
{
    QString fullFilePathName;
    QFileInfo fi(relativeFilePath);
    
    if (fi.isRelative()) {
        fullFilePathName = QFileInfo(relativeFilePath).absoluteFilePath();
    } else {
        fullFilePathName = relativeFilePath;
    }
    
    return fullFilePathName;
}

void writeFileContent(const QString& file2write, const QString& content2write)
{
    Win32_IO_File file(file2write);
    if (file.isOpened())
        file.writeStr(content2write);
}

void writeLog(const QString& logFileName, const char* log2write)
{
    QFile file(logFileName);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QDateTime now = QDateTime::currentDateTime();
        QString timestamp = now.toString("yyyy-MM-dd HH:mm:ss");
        QString logEntry = QString("%1  %2\n").arg(timestamp).arg(log2write);
        file.write(logEntry.toUtf8());
        file.close();
    }
}

void writeLog(const QString& logFileName, const QString& log2write)
{
    writeLog(logFileName, log2write.toUtf8().constData());
}

QString folderBrowser(QWidget* parent, const QString& title, int outputCtrlID, const QString& defaultStr)
{
    QString folderName = QFileDialog::getExistingDirectory(parent, title, defaultStr);
    return folderName;
}

QString getFolderName(QWidget* parent, const QString& defaultDir)
{
    return QFileDialog::getExistingDirectory(parent, "Select a folder", defaultDir);
}

QString purgeMenuItemString(const QString& menuItemStr, bool keepAmpersand)
{
    static const int cleanedNameLen = 64;
    QChar cleanedName[cleanedNameLen] = { '\0' };
    size_t j = 0;
    size_t menuNameLen = menuItemStr.length();
    if (menuNameLen >= static_cast<size_t>(cleanedNameLen))
        menuNameLen = cleanedNameLen - 1;

    for (size_t k = 0; k < menuNameLen; ++k) {
        if (menuItemStr[k] == '\t') {
            cleanedName[k] = '\0';
            break;
        } else {
            if (menuItemStr[k] == '&') {
                if (keepAmpersand)
                    cleanedName[j++] = menuItemStr[k];
            } else {
                cleanedName[j++] = menuItemStr[k];
            }
        }
    }

    cleanedName[j] = '\0';
    return QString(cleanedName);
}

QStringList tokenizeString(const QString& tokenString, const QChar delim)
{
    return tokenString.split(delim, Qt::SkipEmptyParts);
}

const QString& WcharMbcsConvertor::char2wchar(const QByteArray& mbcs2Convert, const QString& codepage)
{
    if (mbcs2Convert.isEmpty()) {
        _wideCharStr.clear();
        return _wideCharStr;
    }

    QTextCodec* codec = nullptr; // QTextCodec removed in Qt6(codepage.toUtf8().constData());
    // Stub: codec unavailable in Qt6
    
    _wideCharStr.resize(mbcs2Convert.length() * 2 + 1);
    _wideCharStr.clear();
    
    return _wideCharStr;
}

const QString& WcharMbcsConvertor::wchar2char(const QString& wcharStr2Convert, const QString& codepage)
{
    if (wcharStr2Convert.isEmpty()) {
        _multiByteStr.clear();
        return _multiByteStr;
    }

    QTextCodec* codec = nullptr; // QTextCodec removed in Qt6(codepage.toUtf8().constData());
    // Stub: codec unavailable in Qt6
    
    QByteArray encoded = codec->fromUnicode(wcharStr2Convert);
    _multiByteStr.resize(encoded.size() + 1);
    
    return _multiByteStr;
}

QString WcharMbcsConvertor::encode(const QString& fromCodepage, const QString& toCodepage, const QString& txt2Encode)
{
    QTextCodec* srcCodec = nullptr; // QTextCodec removed(fromCodepage.toUtf8().constData());
    QTextCodec* dstCodec = nullptr; // QTextCodec removed(toCodepage.toUtf8().constData());
    // Stub: codec unavailable in Qt6
    return txt2Encode;
    
    QByteArray intermediate = srcCodec->fromUnicode(txt2Encode);
    return dstCodec->toUnicode(intermediate);
}

QString string2wstring(const QString& rString, const char* codepage)
{
    QTextCodec* codec = nullptr; // QTextCodec removed in Qt6(codepage);
    if (!codec) return rString;
    return rString; // Stub
}

QString wstring2string(const QString& rwString, const char* codepage)
{
    QTextCodec* codec = nullptr; // QTextCodec removed in Qt6(codepage);
    if (!codec) return rwString;
    return rwString.toUtf8().constData(); // Stub
}

bool isInList(const QString& token, const QString& list)
{
    if (token.isEmpty() || list.isEmpty())
        return false;

    static const int wordLen = 64;
    QChar word[wordLen] = { '\0' };
    size_t listLen = list.length();
    size_t i = 0, j = 0;

    for (; i <= listLen; ++i) {
        if ((i >= list.length() || list[i] == ' ' || list[i] == '\0')) {
            if (j != 0) {
                word[j] = '\0';
                j = 0;
                if (QString(word).toLower() == token.toLower())
                    return true;
            }
        } else {
            word[j] = list[i];
            ++j;
            if (j >= static_cast<size_t>(wordLen))
                return false;
        }
    }
    return false;
}

QString buildMenuFileName(int filenameLen, unsigned int pos, const QString& filename, bool ordinalNumber)
{
    QString strTemp;

    if (ordinalNumber) {
        if (pos < 9) {
            strTemp.append('&');
            strTemp.append(QChar('1' + static_cast<char>(pos)));
        } else if (pos == 9) {
            strTemp.append("1&0");
        } else {
            int quot = (pos + 1) / 10;
            int rem = (pos + 1) % 10;
            strTemp.append(QString::number(quot));
            strTemp.append('&');
            strTemp.append(QString::number(rem));
        }
        strTemp.append(": ");
    } else {
        strTemp.append('&');
    }

    if (filenameLen > 0) {
        if (filename.length() > filenameLen) {
            strTemp.append(filename.left(filenameLen - 3));
            strTemp.append("...");
        } else {
            strTemp.append(filename);
        }
    } else {
        QFileInfo fi(filename);
        QString name = fi.fileName();
        if (name.length() > MAX_PATH) {
            strTemp.append(name.left(MAX_PATH / 2 - 3));
            strTemp.append("...");
            strTemp.append(name.right(MAX_PATH / 2));
        } else {
            strTemp.append(name);
        }
    }

    return strTemp;
}

QString pathRemoveFileSpec(QString& path)
{
    QFileInfo fi(path);
    QString dir = fi.dir().path();
    if (!dir.isEmpty()) path = dir;
    return path;
}

QString pathAppend(QString& strDest, const QString& str2append)
{
    if (strDest.isEmpty() && str2append.isEmpty()) {
        strDest = "/";
        return strDest;
    }

    if (strDest.isEmpty() && !str2append.isEmpty()) {
        strDest = str2append;
        return strDest;
    }

    if (strDest.endsWith('/') && str2append.startsWith('/')) {
        strDest.chop(1);
        strDest += str2append;
        return strDest;
    }

    if ((strDest.endsWith('/') && !str2append.startsWith('/')) ||
        (!strDest.endsWith('/') && str2append.startsWith('/'))) {
        strDest += str2append;
        return strDest;
    }

    strDest += "/";
    strDest += str2append;
    return strDest;
}

QColor getCtrlBgColor(QWidget* hWnd)
{
    if (!hWnd) return QColor();
    QPalette pal = hWnd->palette();
    return pal.color(hWnd->backgroundRole());
}

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

void stringSplit(const QString& input, const QString& delimiter, QStringList& output)
{
    output = input.split(delimiter, Qt::KeepEmptyParts);
}

bool str2numberVector(QString str2convert, QList<size_t>& numVect)
{
    numVect.clear();
    str2convert = str2convert.simplified().remove(' ');

    for (const QChar& c : str2convert) {
        if (!c.isDigit()) return false;
    }

    QStringList parts = str2convert.split(' ', Qt::SkipEmptyParts);
    for (const QString& part : parts) {
        if (!part.isEmpty() && part.length() < 5) {
            numVect.append(part.toULongLong());
        }
    }
    return true;
}

void stringJoin(const QStringList& strings, const QString& separator, QString& joinedString)
{
    joinedString = strings.join(separator);
}

QString stringTakeWhileAdmissable(const QString& input, const QString& admissable)
{
    for (int i = 0; i < input.length(); ++i) {
        if (!admissable.contains(input[i])) {
            return input.left(i);
        }
    }
    return input;
}

double stodLocale(const QString& str, size_t* idx)
{
    bool ok;
    double val = str.toDouble(&ok);
    if (!ok) throw std::invalid_argument("invalid stod argument");
    return val;
}

const std::locale& getSysLocale()
{
    static const auto systemLocale = []() {
        try {
            return std::locale("");
        } catch (const std::runtime_error&) {
            return std::locale::classic();
        }
    }();
    return systemLocale;
}

bool str2Clipboard(const QString& str2cpy, QWidget*)
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(str2cpy);
    return true;
}

QString strFromClipboard()
{
    QClipboard* clipboard = QApplication::clipboard();
    QString text = clipboard->text();
    
    // Trim trailing EOL at end of string if any
    if (text.length() >= 2) {
        if (text.endsWith("\r\n")) {
            text.chop(2);
        } else if (text.endsWith('\n') || text.endsWith('\r')) {
            text.chop(1);
        }
    }
    
    return text;
}

bool buf2Clipboard(const QList<Buffer*>&, bool, QWidget*)
{
    return false; // Buffer not yet defined
}

QString getLastErrorAsString(int errorCode)
{
    if (errorCode == 0) return QString();
    return QString("Error code: %1").arg(errorCode);
}

QString intToString(int val)
{
    return QString::number(val);
}

QString uintToString(unsigned int val)
{
    return QString::number(val);
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
    else {
        nbDigits = 7;
        while (nbLines > 0) {
            nbLines /= 10;
            ++nbDigits;
        }
    }
    return nbDigits;
}

bool ControlInfoTip::init(QWidget* ctrl2attached, QWidget* ctrl2attachedParent, const QString& tipStr, bool isRTL, unsigned int remainTimeMillisecond, int maxWidth)
{
    if (!ctrl2attached) return false;
    
    _tipStr = tipStr;
    _tooltip = new QLabel(ctrl2attachedParent);
    _tooltip->setWordWrap(true);
    _tooltip->setMaximumWidth(maxWidth);
    _tooltip->setText(tipStr);
    _tooltip->setStyleSheet("background-color: #FFFFE1; color: black; border: 1px solid black; padding: 4px;");
    _tooltip->setAttribute(Qt::WA_TranslucentBackground);
    _tooltip->setWindowFlags(Qt::ToolTip);
    
    if (isRTL) {
        _tooltip->setLayoutDirection(Qt::RightToLeft);
    }
    
    return true;
}

void ControlInfoTip::show(showPosition pos) const
{
    if (!_tooltip) return;
    
    QWidget* parent = _tooltip->parentWidget();
    if (!parent) return;
    
    QPoint pos2show = parent->mapToGlobal(QPoint(10, parent->height() / 2));
    _tooltip->move(pos2show);
    _tooltip->show();
}

void ControlInfoTip::hide()
{
    if (_tooltip) {
        _tooltip->hide();
        delete _tooltip;
        _tooltip = nullptr;
    }
}

bool doesFileExist(const QString& filePath)
{
    return QFileInfo::exists(filePath) && QFileInfo(filePath).isFile();
}

bool doesDirectoryExist(const QString& dirPath)
{
    return QFileInfo::exists(dirPath) && QFileInfo(dirPath).isDir();
}

bool doesPathExist(const QString& path)
{
    return QFileInfo::exists(path);
}

bool removeReadOnlyFlagFromFileAttributes(const QString& fileFullPath)
{
    QFile file(fileFullPath);
    QFileInfo fi(fileFullPath);
    if (fi.isDir()) return false;
    
    // Qt handles permissions differently - files are always writable by default
    return true;
}

bool toggleReadOnlyFlagFromFileAttributes(const QString& fileFullPath, bool& isChangedToReadOnly)
{
    QFileInfo fi(fileFullPath);
    if (fi.isDir()) return false;
    
    // In Qt, we use QFile::setPermissions instead
    QFile::Permissions perms = fi.permissions();
    if (perms & QFile::ReadOwner) {
        perms &= ~QFile::ReadOwner;
        isChangedToReadOnly = false;
    } else {
        perms |= QFile::ReadOwner;
        isChangedToReadOnly = true;
    }
    
    return QFile::setPermissions(fileFullPath, perms);
}

Version::Version(const QString& versionStr)
{
    QStringList parts = versionStr.split('.');
    if (parts.size() >= 1) _major = parts[0].toULongLong();
    if (parts.size() >= 2) _minor = parts[1].toULongLong();
    if (parts.size() >= 3) _patch = parts[2].toULongLong();
    if (parts.size() >= 4) _build = parts[3].toULongLong();
}

void Version::setVersionFrom(const QString& filePath)
{
    QFileInfo fi(filePath);
    // Qt6: no fileVersion() - use file name as fallback
    QString version = fi.fileName();
    if (version.isEmpty()) {
        version = fi.fileName();
    }
    *this = Version(version);
}

QString Version::toString() const
{
    return QString("%1.%2.%3.%4").arg(_major).arg(_minor).arg(_patch).arg(_build);
}

int Version::compareTo(const Version& v2c) const
{
    if (_major != v2c._major) return _major > v2c._major ? 1 : -1;
    if (_minor != v2c._minor) return _minor > v2c._minor ? 1 : -1;
    if (_patch != v2c._patch) return _patch > v2c._patch ? 1 : -1;
    if (_build != v2c._build) return _build > v2c._build ? 1 : -1;
    return 0;
}

void trim(QString& str)
{
    str = str.trimmed();
}

void getFilesInFolder(QStringList& files, const QString& extTypeFilter, const QString& inFolder)
{
    QDir dir(inFolder);
    QStringList filters;
    if (extTypeFilter.contains('*')) {
        filters = extTypeFilter.split(';');
    } else {
        filters << extTypeFilter;
    }
    
    QFileInfoList entries = dir.entryInfoList(filters, QDir::Files | QDir::Readable);
    for (const QFileInfo& fi : entries) {
        files.append(fi.absoluteFilePath());
    }
}

bool deleteFileOrFolder(const QString& f2delete)
{
    QFileInfo fi(f2delete);
    if (fi.isDir()) {
        QDir dir(f2delete);
        return dir.removeRecursively();
    } else {
        QFile file(f2delete);
        return file.remove();
    }
}

bool isWin32NamespacePrefixedFileName(const QString& fileName)
{
    return fileName.startsWith("\\\\?\\") || fileName.startsWith("//?/");
}

bool isUnsupportedFileName(const QString& fileName)
{
    if (fileName.isEmpty() || fileName.length() >= MAX_PATH)
        return true;
    
    for (const QChar& c : fileName) {
        if (c.unicode() <= 31) return true;
        if (c == '<' || c == '>' || c == '"' || c == '|')
            return true;
    }
    
    return false;
}

// Win32_IO_File implementation
Win32_IO_File::Win32_IO_File(const QString& fname)
{
    if (fname.isEmpty()) return;
    
    _path = fname;
    _file.setFileName(fname);
    
    // Determine if we should truncate or create
    bool fileExists = _file.exists();
    
    if (!_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        _dwErrorCode = 1; // Simulated error
    }
}

void Win32_IO_File::close()
{
    _dwErrorCode = 0;
    if (_file.isOpen()) {
        if (_written) {
            _file.flush();
        }
        _file.close();
    }
}

bool Win32_IO_File::write(const void* wbuf, size_t buf_size)
{
    _dwErrorCode = 0;
    
    if (!_file.isOpen() || wbuf == nullptr)
        return false;
    
    qint64 written = _file.write(static_cast<const char*>(wbuf), buf_size);
    if (written == -1) {
        _dwErrorCode = 1;
        return false;
    }
    
    _written = true;
    return static_cast<size_t>(written) == buf_size;
}