// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 port includes
#include <QCoreApplication>
#include <QApplication>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QProcess>
#include <QThread>
#include <QElapsedTimer>
#include <QDesktopServices>
#include <QClipboard>
#include <QCursor>
#include <QScreen>
#include <QStandardPaths>
#include <QUrl>
#include <QMimeData>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontDatabase>
#include <QFileDialog>
#include <QInputDialog>
#include <QColor>
#include <QLibrary>
#include <QProcessEnvironment>
#include <QDateTime>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>

#include "Notepad_plus_Window.h"
#include "Notepad_plus.h"
#include "EncodingMapper.h"
#include "ShortcutMapper.h"
#include "TaskListDlg.h"
#include "clipboardFormats.h"
#include "VerticalFileSwitcher.h"
#include "documentMap.h"
#include "functionListPanel.h"
#include "ProjectPanel.h"
#include "fileBrowser.h"
#include "clipboardHistoryPanel.h"
#include "ansiCharPanel.h"
#include "Sorters.h"
#include "verifySignedfile.h"
#include "sha-256.h"
#include "sha512.h"
#include "SortLocale.h"
#include "dpiManagerV2.h"
#include "md5Dlgs.h"  // includes MISC/md5/md5.h (the proper MD5 class)

#include "NppConstants.h"

// Qt6 compat shims for removed Win32 APIs
#define WM_UNDO SCI_UNDO
#define SCI_COPYALLOWLINE npp_sci::SCI_COPYALLOWLINE
#define SCI_SELECTIONISRECTANGLE npp_sci::SCI_SELECTIONISRECTANGLE
#define SCI_GETRECTANGULARSELECTIONANCHOR npp_sci::SCI_GETRECTANGULARSELECTIONANCHOR
#define SCI_GETRECTANGULARSELECTIONCARET npp_sci::SCI_GETRECTANGULARSELECTIONCARET
#define SCI_GETRECTANGULARSELECTIONANCHORVIRTUALSPACE npp_sci::SCI_GETRECTANGULARSELECTIONANCHORVIRTUALSPACE
#define SCI_GETRECTANGULARSELECTIONCARETVIRTUALSPACE npp_sci::SCI_GETRECTANGULARSELECTIONCARETVIRTUALSPACE
#define SCI_CHOOSECARETX npp_sci::SCI_CHOOSECARETX
#define SC_CURSORNORMAL 0
#define SCI_MULTIPLESELECTADDEACH npp_sci::SCI_MULTIPLESELECTADDEACH
#define SCI_MULTIPLESELECTADDNEXT npp_sci::SCI_MULTIPLESELECTADDNEXT
#define SCI_DROPSELECTIONN npp_sci::SCI_DROPSELECTIONN
#define SCI_COPYRANGE npp_sci::SCI_COPYRANGE
#define SCI_LINESSPLIT npp_sci::SCI_LINESSPLIT
#define SCI_GETLINEINDENTATION npp_sci::SCI_GETLINEINDENTATION
#define SCI_STYLESETVISIBLE npp_sci::SCI_STYLESETVISIBLE
#define SCI_CONVERTEOLS npp_sci::SCI_CONVERTEOLS
#define SCI_CANUNDO npp_sci::SCI_CANUNDO
#define SCI_GETXOFFSET npp_sci::SCI_GETXOFFSET
#define SCI_TEXTWIDTH npp_sci::SCI_TEXTWIDTH
#define SCI_GETEDGECOLUMN npp_sci::SCI_GETEDGECOLUMN
#define SCI_GETEDGEMODE npp_sci::SCI_GETEDGEMODE
#define EDGE_NONE 0
#define EDGE_MULTILINE 1
#define SCI_SETSELECTION npp_sci::SCI_SETSELECTION
#define SCI_SETSELECTIONSTART npp_sci::SCI_SETSELECTIONSTART
#define SCI_SETSELECTIONEND npp_sci::SCI_SETSELECTIONEND
#define SC_SEL_STREAM npp_sci::SC_SEL_STREAM
#define SCI_GETSELECTIONMODE npp_sci::SCI_GETSELECTIONMODE
#define URL_INDIC npp_sci::URL_INDIC
#define SCE_ERR_ESCSEQ npp_sci::SCE_ERR_ESCSEQ
#define SCE_ERR_ESCSEQ_UNKNOWN npp_sci::SCE_ERR_ESCSEQ_UNKNOWN
#define SCI_STARTRECORD npp_sci::SCI_STARTRECORD
#define SCI_STOPRECORD npp_sci::SCI_STOPRECORD
#define SCI_SETCURSOR npp_sci::SCI_SETCURSOR
#define SCI_GETSELECTIONS npp_sci::SCI_GETSELECTIONS
#define SCI_POSITIONRELATIVE npp_sci::SCI_POSITIONRELATIVE
#define SCI_COUNTCHARACTERS npp_sci::SCI_COUNTCHARACTERS
#define SCI_ADDTEXT npp_sci::SCI_ADDTEXT
#define SCI_LINESJOIN npp_sci::SCI_LINESJOIN
#define SCI_GETCOLUMN npp_sci::SCI_GETCOLUMN
#define SCI_GETLINEENDPOSITION npp_sci::SCI_GETLINEENDPOSITION
#define SCI_GETSELECTIONNSTART npp_sci::SCI_GETSELECTIONNSTART
#define SCI_GETSELECTIONNEND npp_sci::SCI_GETSELECTIONNEND
#define SCI_SETTARGETRANGE npp_sci::SCI_SETTARGETRANGE
#define SCI_REPLACETARGET npp_sci::SCI_REPLACETARGET
#define SCI_GETSELECTIONLINESRANGE npp_sci::SCI_GETSELECTIONLINESRANGE
#define SCE_UNIVERSAL_FOUND_STYLE npp_sci::SCE_UNIVERSAL_FOUND_STYLE
#define SCE_UNIVERSAL_FOUND_STYLE_EXT1 npp_sci::SCE_UNIVERSAL_FOUND_STYLE_EXT1
#define SCE_UNIVERSAL_FOUND_STYLE_EXT2 npp_sci::SCE_UNIVERSAL_FOUND_STYLE_EXT2
#define SCE_UNIVERSAL_FOUND_STYLE_EXT3 npp_sci::SCE_UNIVERSAL_FOUND_STYLE_EXT3
#define SCE_UNIVERSAL_FOUND_STYLE_EXT4 npp_sci::SCE_UNIVERSAL_FOUND_STYLE_EXT4
#define SCE_UNIVERSAL_FOUND_STYLE_EXT5 npp_sci::SCE_UNIVERSAL_FOUND_STYLE_EXT5

// Fold constants (stub)
enum { fold_collapse = 0, fold_expand = 1 };

// Dir constants
enum { DIR_DOWN = 0, DIR_UP = 1 };

// Find status text (stub placeholders)
static const wchar_t* FIND_STATUS_END_REACHED_TEXT = L"Find: End of document reached";
static const wchar_t* FIND_STATUS_TOP_REACHED_TEXT = L"Find: Beginning of document reached";

// Missing constants
static const int CURRENTWORD_MAXLENGTH = 1024;
static const int FOLDERASWORKSPACE_NODE = 0;
static const int GridState = 0;
static const int WindowActiveNORMAL = 0;

// Qt6 stubs for removed Win32 APIs
inline QString QStringFromWChar(const wchar_t* s) { return QString::fromWCharArray(s); }
inline QString GetLastErrorAsString(int) { return QStringLiteral("Error"); }
inline bool IsChild(QWidget*, QWidget*) { return false; }

// Qt6: QApplication* -> QWidget* conversion helpers
inline QWidget* qappAsWidget(QApplication* app) { return app; }

// Stub implementations for missing panel methods
namespace PanelStubs {
    inline bool isClosedStub() { return true; }
    inline void setClosedStub(bool) {}
    inline void grabFocusStub() {}
}

// Stub for missing ScintillaEditView methods
namespace SciStubs {
    inline void addGenericText(ScintillaComponent*, const wchar_t*) {}
    inline bool getIndicatorRange(ScintillaComponent*, int, size_t*, size_t*, size_t*) { return false; }
    inline bool pasteToMultiSelection(ScintillaComponent*) { return false; }
    inline void removeAnyDuplicateLines(ScintillaComponent*) {}
    inline void showNpc(ScintillaComponent*, bool) {}
    inline void showCcUniEol(ScintillaComponent*, bool) {}
    inline void hideLines(ScintillaComponent*) {}
    inline void setWrapRestoreNeeded(ScintillaComponent*) {}
    inline void markedTextToClipboard(ScintillaComponent*, int) {}
    inline bool expandWordSelection(ScintillaComponent*) { return false; }
    inline void convertSelectedTextToUpperCase(ScintillaComponent*) {}
    inline void convertSelectedTextToLowerCase(ScintillaComponent*) {}
    inline void convertSelectedTextToNewerCase(ScintillaComponent*, int) {}
    inline bool getSelectionLinesRange(ScintillaComponent*, size_t*, size_t*) { return false; }
    inline void sortLines(ScintillaComponent*, int) {}
    inline void insertNewLineAboveCurrentLine(ScintillaComponent*) {}
    inline void insertNewLineBelowCurrentLine(ScintillaComponent*) {}
    inline void beginOrEndSelect(ScintillaComponent*, int) {}
    inline bool beginEndSelectedIsStarted(ScintillaComponent*) { return false; }
    inline bool getSelectedTextToWChar(ScintillaComponent*, wchar_t*, int) { return false; }
    inline bool getSelectedTextToMultiChar(ScintillaComponent*, char*, int) { return false; }
    inline int foldLevel(ScintillaComponent*, int) { return 0; }
    inline int getCurrentBufferId(ScintillaComponent*) { return 0; }
    inline void doUserDefineDlg(ScintillaComponent*, int) {}
}

// Stub for missing RunMacroDlg/GoToLineDlg/Finder methods
namespace DlgStubs {
    inline QWidget* getHSelfStub(QWidget* w) { return w; }
    inline bool isCreatedStub(QWidget* w) { return w != nullptr; }
    inline void doDialogStub(QWidget* w, bool) { if (w) w->show(); }
    inline void setSearchTextStub(QWidget* w, const wchar_t*) {}
    inline void regexBackwardMsgBoxStub(QWidget*) {}
    inline void gotoNextFoundResultStub(QWidget*) {}
    inline QWidget* getHFindResultsStub(QWidget* w) { return w; }
    inline void updateFinderScintillaForNpcStub(QWidget*) {}
    inline void setIndividualTabColourStub(QWidget*, int) {}
}

// Hash function stubs
inline QString hash_sha1(const wchar_t*) { return QString(); }
inline QString hash_sha256(const wchar_t*) { return QString(); }
inline QString hash_sha512(const wchar_t*) { return QString(); }

// Command class stub
class Command {
public:
    Command(const wchar_t*) {}
    void run(QWidget*, const wchar_t*) {}
};

// ISorter stub
class ISorter {
public:
    virtual ~ISorter() = default;
};

// Sorter stubs
class LexicographicSorter : public ISorter {};
class LexicographicCaseInsensitiveSorter : public ISorter {};
class IntegerSorter : public ISorter {};
class DecimalCommaSorter : public ISorter {};
class DecimalDotSorter : public ISorter {};
class LineLengthSorter : public ISorter {};
class ReverseSorter : public ISorter {};
class RandomSorter : public ISorter {};

// SortLocale stub
namespace SortLocaleStubs {
    inline void sort(std::vector<int>*, int, int) {}
}

// Macro recording step stub
void recordMacroStep(int) {}

// =============================================================================
// macroPlayback — plays back a recorded macro
// =============================================================================
void Notepad_plus::macroPlayback(Macro macro, std::vector<Document>* pDocs4EndUAIn)
{
    _playingBackMacro = true;

    std::vector<Document>* pDocs4EndUA = nullptr;
    if (pDocs4EndUAIn)
    {
        pDocs4EndUA = pDocs4EndUAIn;
    }
    else
    {
        pDocs4EndUA = new std::vector<Document>;
        if (!pDocs4EndUA)
            return;
    }

    Document prevSciDoc = 0;
    for (auto step = macro.begin(); step != macro.end(); ++step)
    {
        Document curSciDoc = _pEditView->getCurrentBuffer()->getDocument();
        if (curSciDoc != prevSciDoc)
        {
            if (std::find(pDocs4EndUA->begin(), pDocs4EndUA->end(), curSciDoc) == pDocs4EndUA->end())
            {
                _pEditView->execute(SCI_BEGINUNDOACTION);
                pDocs4EndUA->push_back(curSciDoc);
            }
            prevSciDoc = curSciDoc;
        }

        if (step->isScintillaMacro())
        {
            // playBack() is a stub in shortcut.cpp — full implementation pending
            step->playBack();
        }
        else
        {
            // Menu command playback — TODO: implement via command()
            (void)step->_message;
        }
    }

    if (!pDocs4EndUAIn)
    {
        Document invisSciDoc = _invisibleEditView.execute(SCI_GETDOCPOINTER);
        while (!pDocs4EndUA->empty())
        {
            Document doc = pDocs4EndUA->back();
            if (MainFileManager.getBufferFromDocument(doc) == BUFFER_INVALID)
            {
                // doc no longer exists
            }
            else
            {
                _invisibleEditView.execute(SCI_SETDOCPOINTER, 0, doc);
                _invisibleEditView.execute(SCI_ENDUNDOACTION);
            }
            pDocs4EndUA->pop_back();
        }
        _invisibleEditView.execute(SCI_SETDOCPOINTER, 0, invisSciDoc);

        delete pDocs4EndUA;
        pDocs4EndUA = nullptr;
    }

    _playingBackMacro = false;
}

// =============================================================================
// command — menu command dispatcher (stub)
// All commands are handled via NppCommands slots connected to QAction signals.
// This stub prevents linker errors for the declared Notepad_plus::command(int).
// =============================================================================
void Notepad_plus::command(int)
{
    // Menu commands are dispatched via NppCommands slots connected to menu actions.
    // The full switch statement will be migrated incrementally.
}

// =============================================================================
// NppCommands slot implementations (stubs)
// These connect Qt menu actions to Notepad_plus commands.
// TODO: wire up to actual Notepad_plus methods via _mainWindow
// =============================================================================

NppCommands::NppCommands(QObject* parent) : QObject(parent) {}

NppCommands::~NppCommands() {}

void NppCommands::setMainWindow(MainWindow* mainWindow)
{
    _mainWindow = mainWindow;
}

ScintillaEditView* NppCommands::activeView()
{
    if (!_mainWindow)
        return nullptr;
    // TODO: implement using _mainWindow->getActiveView()
    return nullptr;
}

bool NppCommands::isCurrentFileModified() const
{
    return false;  // TODO
}

bool NppCommands::confirmSave(const QString&)
{
    return true;  // TODO
}

void NppCommands::updateStatusBar()
{
    // TODO
}

// === File commands ===
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

// === Edit commands ===
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

// === Search commands ===
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

// === View commands ===
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

// === Encoding commands ===
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

// === Language commands ===
void NppCommands::languageMenu() {}
void NppCommands::languageUserDefine() {}
void NppCommands::languageOpenUdldDir() {}

// === Settings commands ===
void NppCommands::settingsPreference() {}
void NppCommands::settingsStyleConfig() {}
void NppCommands::settingsShortcutMapper() {}
void NppCommands::settingsImportPlugin() {}
void NppCommands::settingsImportStyleThemes() {}
void NppCommands::settingsEditContextMenu() {}

// === Macro commands ===
void NppCommands::macroStartRecording() {}
void NppCommands::macroStopRecording() {}
void NppCommands::macroPlayback() {}
void NppCommands::macroSaveCurrent() {}
void NppCommands::macroRunMultiple() {}

// === Run commands ===
void NppCommands::runExecute() {}
void NppCommands::runCmdLineArgs() {}

// === Window commands ===
void NppCommands::windowSwitchToDocument(int) {}
void NppCommands::windowSortAscending() {}
void NppCommands::windowSortDescending() {}

// === Help commands ===
void NppCommands::helpAbout() {}
void NppCommands::helpDocumentation() {}
void NppCommands::helpHomeSweetHome() {}
void NppCommands::helpOnlineDoc() {}

// === Column editor commands ===
void NppCommands::columnEditor() {}

// === Clipboard commands ===
void NppCommands::clipboardHistoryPanel() {}
void NppCommands::charPanel() {}

// === Panel commands ===
void NppCommands::toggleDocumentMap() {}
void NppCommands::toggleFunctionList() {}
void NppCommands::toggleFileBrowser() {}
void NppCommands::toggleProjectPanel1() {}
void NppCommands::toggleProjectPanel2() {}
void NppCommands::toggleProjectPanel3() {}
void NppCommands::toggleClipboardHistory() {}
void NppCommands::toggleDocList() {}

// === Find/Replace panel commands ===
void NppCommands::findInFiles() {}
void NppCommands::findInProjects() {}
void NppCommands::findInFilelist() {}
void NppCommands::replaceInFiles() {}
void NppCommands::replaceInProjects() {}
void NppCommands::replaceInFilelist() {}
void NppCommands::findAllInOpenedDocs() {}
void NppCommands::replaceAllInOpenedDocs() {}

// === Toggle commands ===
void NppCommands::toggleReadOnly() {}
void NppCommands::setReadOnlyForAll() {}
void NppCommands::clearReadOnlyForAll() {}
void NppCommands::toggleRecording() {}
void NppCommands::toggleMacroPlayback() {}
void NppCommands::toggleMonitoring() {}
