// This file is part of Notepad++ project
// Stub for Linux - real implementations needed for full functionality

#include "MISC/Common/Common.h"
#include "ScintillaComponent/Buffer.h"
#include "WinControls/OpenSaveFileDialog/CustomFileDialog.h"
#include "MISC/Common/NppConstants.h"

#include <cstring>
#include <cstdlib>
#include <algorithm>

std::wstring folderBrowser(void*, const std::wstring&, int, const wchar_t*) { return {}; }
std::wstring getFolderName(void*, const wchar_t*) { return {}; }
void printInt(int) {}
void printStr(const wchar_t*) {}
std::wstring commafyInt(size_t n) { return std::to_wstring(n); }
void writeLog(const wchar_t*, const char*) {}
void writeLog(const wchar_t*, const wchar_t*) {}
int filter(unsigned int, struct _EXCEPTION_POINTERS*) { return 1; }
std::wstring purgeMenuItemString(const wchar_t* s, bool) { return s ? s : L""; }
std::vector<std::wstring> tokenizeString(const std::wstring&, char) { return {}; }
void ClientRectToScreenRect(void*, RECT*) {}
void ScreenRectToClientRect(void*, RECT*) {}

const wchar_t* WcharMbcsConvertor::char2wchar(const char* mbcs, size_t, int, int*, int*) {
    if (!mbcs) { _wideCharStr.empty(); return _wideCharStr; }
    _wideCharStr.sizeTo(strlen(mbcs) + 1);
    mbstowcs(_wideCharStr, mbcs, strlen(mbcs) + 1);
    return _wideCharStr;
}
const wchar_t* WcharMbcsConvertor::char2wchar(const char* mbcs, size_t, intptr_t*, intptr_t*, int) {
    return mbcs ? char2wchar(mbcs, 0, -1, nullptr, nullptr) : (_wideCharStr.empty(), (const wchar_t*)_wideCharStr);
}
const char* WcharMbcsConvertor::wchar2char(const wchar_t* w, size_t, int, int*) {
    if (!w) { _multiByteStr.empty(); return _multiByteStr; }
    int len = wcstombs(nullptr, w, 0);
    if (len > 0) { _multiByteStr.sizeTo(len + 1); wcstombs(_multiByteStr, w, len + 1); }
    return _multiByteStr;
}
const char* WcharMbcsConvertor::wchar2char(const wchar_t* w, size_t, intptr_t*, intptr_t*, int, int* l) {
    return w ? wchar2char(w, 0, -1, l) : (_multiByteStr.empty(), (const char*)_multiByteStr);
}
const char* WcharMbcsConvertor::encode(unsigned int, unsigned int, const char* t, int len, int* pL, int*) {
    if (!t) return nullptr;
    if (len == -1) len = strlen(t);
    _multiByteStr.sizeTo(len + 1);
    strncpy(_multiByteStr, t, len);
    _multiByteStr[len] = 0;
    if (pL) *pL = len;
    return _multiByteStr;
}

std::wstring string2wstring(const std::string& s, unsigned int) { return QString::fromUtf8(s.c_str()).toStdWString(); }
std::string wstring2string(const std::wstring& s, unsigned int) { return QString::fromStdWString(s).toUtf8().constData(); }
std::wstring pathRemoveFileSpec(std::wstring& p) { auto i = p.find_last_of(L"\\/"); if (i != std::wstring::npos) p.erase(i); return p; }
std::wstring pathAppend(std::wstring& d, const std::wstring& a) { if (!d.empty() && d.back()!=L'\\' && d.back()!=L'/') d+=L'\\'; d+=a; return d; }
unsigned int getCtrlBgColor(void*) { return 0; }
std::wstring stringToUpper(std::wstring s) { std::transform(s.begin(), s.end(), s.begin(), ::towupper); return s; }
std::wstring stringToLower(std::wstring s) { std::transform(s.begin(), s.end(), s.begin(), ::towlower); return s; }
std::wstring stringReplace(std::wstring s, const std::wstring& f, const std::wstring& r) { size_t p=0; while((p=s.find(f,p))!=std::string::npos){ s.replace(p,f.size(),r); p+=r.size(); } return s; }
void stringSplit(const std::wstring&, const std::wstring&, std::vector<std::wstring>&) {}
bool str2numberVector(std::wstring, std::vector<size_t>& v) { v.clear(); return true; }
void stringJoin(const std::vector<std::wstring>&, const std::wstring&, std::wstring&) {}
std::wstring stringTakeWhileAdmissable(const std::wstring& i, const std::wstring&) { return i; }
double stodLocale(const std::wstring& s, _locale_t, size_t* idx) { try { return std::stod(QString::fromStdWString(s).toStdString()); } catch(...) { if(idx)*idx=0; return 0; } }
const std::locale& getSysLocale() { static std::locale l("C"); return l; }
bool str2Clipboard(const std::wstring& s, void*) { QApplication::clipboard()->setText(QString::fromStdWString(s)); return true; }
std::wstring strFromClipboard() { return QApplication::clipboard()->text().toStdWString(); }
bool buf2Clipboard(const std::vector<Buffer*>&, bool, void*) { return true; }
std::wstring GetLastErrorAsString(unsigned int) { return L"no error"; }
std::wstring intToString(int v) { return std::to_wstring(v); }
std::wstring uintToString(unsigned int v) { return std::to_wstring(v); }
void* createToolTip(int, void*, void*, wchar_t*, bool) { return nullptr; }
void* createToolTipRect(int, void*, void*, wchar_t*, RECT) { return nullptr; }
bool isCertificateValidated(const std::wstring&, const std::wstring&) { return false; }
bool isAssoCommandExisting(const wchar_t*) { return false; }
bool deleteFileOrFolder(const std::wstring&) { return false; }
void getFilesInFolder(std::vector<std::wstring>&, const std::wstring&, const std::wstring&){}
bool matchInList(const wchar_t*, const std::vector<std::wstring>&) { return false; }
bool matchInExcludeDirList(const wchar_t*, const std::vector<std::wstring>&, size_t) { return false; }
bool allPatternsAreExclusion(const std::vector<std::wstring>&) { return true; }

template<typename T> size_t vecRemoveDuplicates(std::vector<T>& v, bool s, bool cs) {
    if (!s && cs) std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end()); return v.size();
}
template size_t vecRemoveDuplicates<>(std::vector<int>&, bool, bool);
template size_t vecRemoveDuplicates<>(std::vector<unsigned int>&, bool, bool);
template size_t vecRemoveDuplicates<>(std::vector<char>&, bool, bool);
template size_t vecRemoveDuplicates<>(std::vector<wchar_t>&, bool, bool);

void trim(std::wstring&) {}
int nbDigitsFromNbLines(size_t n) { int d=1; while(n/=10) ++d; return d; }
std::wstring getDateTimeStrFrom(const std::wstring&, const SYSTEMTIME&) { return {}; }
void* createFont(const wchar_t*, int, bool, void*) { return nullptr; }
bool removeReadOnlyFlagFromFileAttributes(const wchar_t*) { return false; }
bool toggleReadOnlyFlagFromFileAttributes(const wchar_t*, bool& r) { r=false; return true; }
bool isWin32NamespacePrefixedFileName(const std::wstring&) { return false; }
bool isWin32NamespacePrefixedFileName(const wchar_t*) { return false; }
bool isUnsupportedFileName(const std::wstring&) { return false; }
bool isUnsupportedFileName(const wchar_t*) { return false; }

Version::Version(const std::wstring&) {}
void Version::setVersionFrom(const std::wstring&) {}
std::wstring Version::toString() const { return L"0.0.0.0"; }
int Version::compareTo(const Version&) const { return 0; }
bool Version::isCompatibleTo(const Version&, const Version&) const { return false; }

bool getDiskFreeSpaceWithTimeout(const wchar_t*, ULARGE_INTEGER* f, unsigned int, bool*) { f->QuadPart = 0; return false; }
bool getFileAttributesExWithTimeout(const wchar_t*, WIN32_FILE_ATTRIBUTE_DATA* a, unsigned int, bool*, unsigned int*) { a->dwFileAttributes = 0; return false; }
bool doesFileExist(const wchar_t* f, unsigned int, bool*) { return QFile::exists(QString::fromWCharArray(f)); }
bool doesDirectoryExist(const wchar_t* f, unsigned int, bool*) { return QFileInfo(QString::fromWCharArray(f)).isDir(); }
bool doesPathExist(const wchar_t* f, unsigned int, bool*) { return QFileInfo(QString::fromWCharArray(f)).exists(); }
bool isWindowVisibleOnAnyMonitor(const RECT&) { return false; }
bool isCoreWindows() { return false; }

bool ControlInfoTip::init(void*, void*, void*, const std::wstring&, bool, unsigned int, int) { return false; }
void ControlInfoTip::show(showPosition) const {}
void ControlInfoTip::hide() {}

unsigned int invokeNppUacOp(const std::wstring&) { return 0; }
bool fileTimeToYMD(const FILETIME&, int&) { return false; }
void expandEnv(std::wstring&) {}

std::string Utf8::encode(std::wstring_view sv) { return QString::fromStdWString(std::wstring(sv)).toUtf8().constData(); }
std::string getFileContent(const wchar_t* f, bool* pbFailed) {
    if (pbFailed) *pbFailed = false;
    QFile file(QString::fromWCharArray(f));
    if (!file.open(QIODevice::ReadOnly)) { if (pbFailed) *pbFailed = true; return {}; }
    QByteArray d = file.readAll();
    return d.constData();
}
std::wstring relativeFilePathToFullFilePath(const wchar_t* rel) { return QDir(QString::fromWCharArray(rel)).absolutePath().toStdWString(); }
void writeFileContent(const wchar_t* f, const char* c) { QFile file(QString::fromWCharArray(f)); if (file.open(QIODevice::WriteOnly)) file.write(c); }

namespace { std::wstring getWidowString() { return L"no error"; } }