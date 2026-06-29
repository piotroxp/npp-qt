// STUB FILE — provides linker-needed implementations for disabled .cpp files
// All stubs are no-ops for Qt6 build verification

#include "NppCommands.h"
#include "NppIO.h"
#include "NppBigSwitch.h"
#include "WindowsDlg.h"
#include "WinControls/ClipboardHistory.h"

// === STUB CONSTRUCTORS/DESTRUCTORS ===
NppCommands::NppCommands(QObject* parent) : QObject(parent) {}
NppCommands::~NppCommands() {}
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

// === STUB SLOTS (NppCommands) ===

// File commands
void NppCommands::fileNew() {}
void NppCommands::fileOpen() {}
void NppCommands::fileOpenFolder() {}
void NppCommands::fileOpenFolderAsWorkspace() {}
void NppCommands::fileOpenContainingFolder() {}
void NppCommands::fileOpenInCommandPrompt() {}
void NppCommands::fileOpenAsReadOnly() {}
void NppCommands::fileReload() {}
void NppCommands::fileSave() {}
void NppCommands::fileSaveAs() {}
void NppCommands::fileSaveCopyAs() {}
void NppCommands::fileSaveAll() {}
void NppCommands::fileRename() {}
void NppCommands::fileDelete() {}
void NppCommands::fileClose() {}
void NppCommands::fileCloseAll() {}
void NppCommands::fileCloseAllButCurrent() {}
void NppCommands::fileCloseAllToLeft() {}
void NppCommands::fileCloseAllToRight() {}
void NppCommands::fileCloseAllUnchanged() {}
void NppCommands::fileLoadSession() {}
void NppCommands::fileSaveSession() {}
void NppCommands::filePrint() {}
void NppCommands::fileExit() {}
void NppCommands::fileRestoreLastClosed() {}

// Edit commands
void NppCommands::editCut() {}
void NppCommands::editCopy() {}
void NppCommands::editPaste() {}
void NppCommands::editDelete() {}
void NppCommands::editSelectAll() {}
void NppCommands::editBeginEndSelect() {}
void NppCommands::editBeginEndSelectColumnMode() {}
void NppCommands::editUndo() {}
void NppCommands::editRedo() {}
void NppCommands::editFullPathToClip() {}
void NppCommands::editFileNameToClip() {}
void NppCommands::editCurrentDirToClip() {}
void NppCommands::editCopyAllNames() {}
void NppCommands::editCopyAllPaths() {}
void NppCommands::editInsertTab() {}
void NppCommands::editRemoveTab() {}
void NppCommands::editUpperCase() {}
void NppCommands::editLowerCase() {}
void NppCommands::editProperCase() {}
void NppCommands::editSentenceCase() {}
void NppCommands::editInvertCase() {}
void NppCommands::editRandomCase() {}
void NppCommands::editRemoveConsecutiveDupLines() {}
void NppCommands::editRemoveAnyDupLines() {}
void NppCommands::editSplitLines() {}
void NppCommands::editJoinLines() {}
void NppCommands::editLineUp() {}
void NppCommands::editLineDown() {}
void NppCommands::editRemoveEmptyLines() {}
void NppCommands::editRemoveEmptyLinesWithBlank() {}
void NppCommands::editBlankLineAbove() {}
void NppCommands::editBlankLineBelow() {}
void NppCommands::editSortLines(int) {}
void NppCommands::editBlockComment() {}
void NppCommands::editBlockUncomment() {}
void NppCommands::editStreamComment() {}
void NppCommands::editStreamUncomment() {}
void NppCommands::editAutoComplete() {}
void NppCommands::editAutoCompletePath() {}
void NppCommands::editAutoCompleteCurrentFile() {}
void NppCommands::editFuncCallTip() {}
void NppCommands::editFuncCallTipPrevious() {}
void NppCommands::editFuncCallTipNext() {}
void NppCommands::editInsertDateTimeShort() {}
void NppCommands::editInsertDateTimeLong() {}
void NppCommands::editInsertDateTimeCustom() {}
void NppCommands::editTrimTrailing() {}
void NppCommands::editTrimLeading() {}
void NppCommands::editTrimBoth() {}
void NppCommands::editEolToWs() {}
void NppCommands::editTrimAll() {}
void NppCommands::editTabToSpace() {}
void NppCommands::editSpaceToTabAll() {}
void NppCommands::editSpaceToTabLeading() {}

// Search commands
void NppCommands::searchFind() {}
void NppCommands::searchReplace() {}
void NppCommands::searchFindNext() {}
void NppCommands::searchFindPrev() {}
void NppCommands::searchFindInFiles() {}
void NppCommands::searchFindInProjects() {}
void NppCommands::searchGoToLine() {}
void NppCommands::searchGoToMatchingBrace() {}
void NppCommands::searchSelectMatchingBraces() {}
void NppCommands::searchMarkAllExt1() {}
void NppCommands::searchMarkAllExt2() {}
void NppCommands::searchMarkAllExt3() {}
void NppCommands::searchMarkAllExt4() {}
void NppCommands::searchMarkAllExt5() {}
void NppCommands::searchClearAllMarks() {}
void NppCommands::searchToggleBookmark() {}
void NppCommands::searchNextBookmark() {}
void NppCommands::searchPrevBookmark() {}
void NppCommands::searchClearBookmarks() {}
void NppCommands::searchCutMarkedLines() {}
void NppCommands::searchCopyMarkedLines() {}
void NppCommands::searchPasteMarkedLines() {}
void NppCommands::searchDeleteMarkedLines() {}
void NppCommands::searchDeleteUnmarkedLines() {}
void NppCommands::searchInverseMarks() {}
void NppCommands::searchFindCharInRange() {}

// View commands
void NppCommands::viewAlwaysOnTop() {}
void NppCommands::viewFullScreen() {}
void NppCommands::viewPostIt() {}
void NppCommands::viewDistractionFree() {}
void NppCommands::viewTabSpace() {}
void NppCommands::viewEol() {}
void NppCommands::viewAllCharacters() {}
void NppCommands::viewNpc() {}
void NppCommands::viewNpcCcuniEol() {}
void NppCommands::viewIndentGuide() {}
void NppCommands::viewWrapSymbol() {}
void NppCommands::viewZoomIn() {}
void NppCommands::viewZoomOut() {}
void NppCommands::viewZoomRestore() {}
void NppCommands::viewGotoStart() {}
void NppCommands::viewGotoEnd() {}
void NppCommands::viewSwitchToOtherView() {}
void NppCommands::viewCloneToAnotherView() {}
void NppCommands::viewGotoNewInstance() {}
void NppCommands::viewLoadInNewInstance() {}
void NppCommands::viewWrap() {}
void NppCommands::viewHideLines() {}
void NppCommands::viewFullScreenToggle() {}
void NppCommands::viewSummary() {}

// Encoding commands
void NppCommands::encodingAnsi() {}
void NppCommands::encodingUTF8() {}
void NppCommands::encodingUTF8BOM() {}
void NppCommands::encodingUTF16BE() {}
void NppCommands::encodingUTF16LE() {}
void NppCommands::encodingConvertAnsi() {}
void NppCommands::encodingConvertUTF8() {}
void NppCommands::encodingConvertUTF8BOM() {}
void NppCommands::encodingConvertUTF16BE() {}
void NppCommands::encodingConvertUTF16LE() {}

// Language commands
void NppCommands::languageMenu() {}
void NppCommands::languageUserDefine() {}
void NppCommands::languageOpenUdldDir() {}

// Settings commands
void NppCommands::settingsPreference() {}
void NppCommands::settingsStyleConfig() {}
void NppCommands::settingsShortcutMapper() {}
void NppCommands::settingsImportPlugin() {}
void NppCommands::settingsImportStyleThemes() {}
void NppCommands::settingsEditContextMenu() {}

// Macro commands
void NppCommands::macroStartRecording() {}
void NppCommands::macroStopRecording() {}
void NppCommands::macroPlayback() {}
void NppCommands::macroSaveCurrent() {}
void NppCommands::macroRunMultiple() {}

// Run commands
void NppCommands::runExecute() {}
void NppCommands::runCmdLineArgs() {}

// Help commands
void NppCommands::helpAbout() {}
void NppCommands::helpDocumentation() {}
void NppCommands::helpHomeSweetHome() {}
void NppCommands::helpOnlineDoc() {}

// Window commands
void NppCommands::windowSwitchToDocument(int) {}
void NppCommands::windowSortAscending() {}
void NppCommands::windowSortDescending() {}

// Panel toggle commands
void NppCommands::toggleDocList() {}
void NppCommands::toggleClipboardHistory() {}
void NppCommands::toggleDocumentMap() {}
void NppCommands::toggleFileBrowser() {}
void NppCommands::toggleFunctionList() {}
void NppCommands::toggleProjectPanel1() {}
void NppCommands::toggleProjectPanel2() {}
void NppCommands::toggleProjectPanel3() {}
void NppCommands::toggleMacroPlayback() {}
void NppCommands::toggleRecording() {}
void NppCommands::toggleReadOnly() {}
void NppCommands::toggleMonitoring() {}
void NppCommands::setReadOnlyForAll() {}
void NppCommands::clearReadOnlyForAll() {}

// Find/replace in... commands
void NppCommands::findAllInOpenedDocs() {}
void NppCommands::replaceAllInOpenedDocs() {}
void NppCommands::findInFilelist() {}
void NppCommands::replaceInFilelist() {}
void NppCommands::findInProjects() {}
void NppCommands::replaceInProjects() {}
void NppCommands::findInFiles() {}
void NppCommands::replaceInFiles() {}

// Additional slots
void NppCommands::charPanel() {}
void NppCommands::columnEditor() {}
void NppCommands::clipboardHistoryPanel() {}

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
