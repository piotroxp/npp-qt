// STUB FILE — provides linker-needed implementations for disabled .cpp files
// All stubs are no-ops for Qt6 build verification

#include "NppIO.h"
#include "NppBigSwitch.h"
#include "WindowsDlg.h"
#include "WinControls/ClipboardHistory.h"

// === STUB CONSTRUCTORS/DESTRUCTORS ===
NppIO::NppIO(QObject* parent) : QObject(parent) {}
NppIO::~NppIO() {}
FileWatchThread::FileWatchThread(const QString&, QObject*) {}
FileWatchThread::~FileWatchThread() {}
void FileWatchThread::run() {}
void FileWatchThread::requestStop() {}
bool FileWatchThread::waitHandle() const { return false; }
NppBigSwitch::NppBigSwitch(QObject*) {}
NppBigSwitch::~NppBigSwitch() {}
bool NppBigSwitch::eventFilter(QObject*, QEvent*) { return false; }
void WindowsDlg::onCopyName() {}
void WindowsDlg::onCopyPath() {}

// === NppIO SLOTS ===
void NppIO::onFileChanged(const QString&) {}
void NppIO::onFileWatchError(const QString&) {}


// === FileBrowser, ClipboardHistory, FunctionList stubs ===
#include "WinControls/FileBrowser.h"
#include "WinControls/FunctionList.h"


// === Date stubs ===
#include "Parameters.h"
Date::Date(int) noexcept {}
bool Date::operator<(const Date&) const { return false; }

// === More Parameters stubs ===
LangType NppParameters::getLangIDFromStr(const wchar_t*) { return LangType::L_TEXT; }
std::wstring NppParameters::getLocPathFromStr(const std::wstring&) { return {}; }

// === FileManager stubs ===
#include "Buffer.h"
Buffer* FileManager::getBufferByID(BufferID) { return nullptr; }

// === FINAL MISSING STUBS ===

// Notepad_plus core methods
#include "Notepad_plus.h"
BufferID Notepad_plus::doOpen(const std::wstring&, bool, bool, int, const wchar_t*, NppFileTime) { return 0; }
bool Notepad_plus::doReload(BufferID, bool) { return true; }
void Notepad_plus::doClose(BufferID, int, bool) {}
bool Notepad_plus::loadSession(Session&, bool, const wchar_t*) { return true; }

// NppParameters methods
LangType NppParameters::getLangFromExt(const wchar_t*) { return LangType::L_TEXT; }
const wchar_t* NppParameters::getLangExtFromLangType(LangType) const { return L""; }
bool NppParameters::getPluginCmdsFromXmlTree() { return true; }
bool NppParameters::writeFileBrowserSettings(const std::vector<std::wstring>&, const std::wstring&) { return true; }
QRgb NppParameters::getIndividualTabColor(int, bool, bool) { return 0; }

// PreferenceDlg
#include "WinControls/Preference.h"
void PreferenceDlg::init(QWidget*) {}

// StatusBar
#include "WinControls/StatusBar.h"
void StatusBar::display(int, const QString&, bool) {}

// ThemeSwitcher methods
void ThemeSwitcher::addDefaultThemeFromXml(const std::wstring&) {}
std::pair<std::wstring, std::wstring>& ThemeSwitcher::getElementFromIndex(size_t) { static std::pair<std::wstring, std::wstring> dummy; return dummy; }

// Free function
#include "MISC/Common/Common.h"
QString buildMenuFileName(int, unsigned int, const QString&, bool) { return {}; }
