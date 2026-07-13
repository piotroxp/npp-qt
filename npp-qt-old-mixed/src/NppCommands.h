#pragma once
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

// Qt6 port — NppCommands.cpp
// Win32→Qt6 translation of PowerEditor/src/NppCommands.cpp
// Bridges all menu actions (File, Edit, Search, View, Encoding, Language,
// Settings, Macro, Run, Window, Help) to Qt slots.

#include <QCoreApplication>
#include <QApplication>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QVector>
#include <QRegularExpression>
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
#include <QWindow>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>

#include "Notepad_plus_Window.h"
#include "Notepad_plus.h"
#include "ScintillaComponent/Printer.h"
#include "NppCommands.h"
#include "EncodingMapper.h"
#include "ShortcutMapper.h"
#include "Dialogs/PreferenceDialog.h"
#include "Dialogs/StyleConfigDialog.h"
#include "Dialogs/ShortcutMapperDialog.h"
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
#include "md5Dlgs.h"

#include "NppConstants.h"
#include "ScintillaComponent.h"


// =============================================================================
// Qt6 compat shims for Win32/Scintilla APIs not yet in npp-qt
// =============================================================================

#define WM_UNDO SCI_UNDO
#define WM_COPY SCI_COPY
#define WM_CUT SCI_CUT
#define WM_CLEAR SCI_CLEAR
#define SCI_COPYALLOWLINE 2519  // not in npp_sci namespace; use raw value

// Missing constants as constexpr (raw values from Scintilla.h / NppSciCompat.h)
static constexpr int SEARCHNEXT = 2367;
static constexpr int SETSEARCHFLAGS = npp_sci::SCI_SETSEARCHFLAGS;  // 2198
static constexpr int LINEFROMPOSITION = npp_sci::SCI_LINEFROMPOSITION;  // 2166
static constexpr int SCWRAP_NONE = 0;
static constexpr int SCWRAP_WORD = 1;
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
#define SCI_LINESSPLIT 2289  // not in npp_sci namespace; use raw value
#define SCI_SHOWLINES 2226
#define SCI_HIDELINES 2227
#define SCI_GETLINEVISIBLE 2228
#define SCI_GETLINEENDPOSITION npp_sci::SCI_GETLINEENDPOSITION  // 2136
#define SCI_POSITIONFROMLINE 2167  // not in npp_sci namespace
#define SCI_GETLINEINDENTATION npp_sci::SCI_GETLINEINDENTATION
#define SCI_STYLESETVISIBLE npp_sci::SCI_STYLESETVISIBLE
#define SCI_CONVERTEOLS npp_sci::SCI_CONVERTEOLS
#define SCI_CANUNDO npp_sci::SCI_CANUNDO
#define SCI_GETXOFFSET npp_sci::SCI_GETXOFFSET
#define SCI_TEXTWIDTH npp_sci::SCI_TEXTWIDTH
#define SCI_GETEDGECOLUMN 2360  // not in npp_sci namespace; use raw value
#define SCI_GETEDGEMODE 2362    // not in npp_sci namespace; use raw value
#define EDGE_NONE 0
#define EDGE_MULTILINE 1
#define SCI_SETSELECTION npp_sci::SCI_SETSELECTION
#define SCI_SETSELECTIONSTART npp_sci::SCI_SETSELECTIONSTART
#define SCI_SETSELECTIONEND npp_sci::SCI_SETSELECTIONEND
#define SC_SEL_STREAM npp_sci::SC_SEL_STREAM
#define SC_SEL_RECTANGLE npp_sci::SC_SEL_RECTANGLE
#define SCI_GETSELECTIONMODE npp_sci::SCI_GETSELECTIONMODE
#define SCI_SETSELECTIONMODE 2422  // not in npp_sci namespace
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

// Fold constants
enum { fold_collapse = 0, fold_expand = 1 };

// Dir constants
enum { DIR_DOWN = 0, DIR_UP = 1 };

// Find status text
static const wchar_t* FIND_STATUS_END_REACHED_TEXT = L"Find: End of document reached";
static const wchar_t* FIND_STATUS_TOP_REACHED_TEXT = L"Find: Beginning of document reached";

// Missing constants
static const int CURRENTWORD_MAXLENGTH = 1024;
static const int FOLDERASWORKSPACE_NODE = 0;

// Sort options
static const int ASC = 1;
static const int DESC = 2;
static const int LEX = 4;
static const int INTEGER = 8;
static const int REVERSED = 16;
static const int GridState = 0;
static const int WindowActiveNORMAL = 0;

// Qt6 stubs for removed Win32 APIs
inline QString QStringFromWChar(const wchar_t* s) { return QString::fromWCharArray(s); }
inline QString GetLastErrorAsString(int) { return QStringLiteral("Error"); }
inline bool IsChild(QWidget*, QWidget*) { return false; }

// Qt6: QApplication* -> QWidget* conversion helpers
inline QWidget* qappAsWidget(QWidget* app) { return app; }

// Stub implementations for missing panel methods
namespace PanelStubs {
    inline bool isClosedStub() { return true; }
    inline void setClosedStub(bool) {}
    inline void grabFocusStub() {}
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
            step->playBack();
        }
        else
        {
            // Menu command — dispatch through NppCommands slots
            // route through NppCommands so menu commands are recorded if recording is active
            getNppCommands()->recordMacroStepMenu(step->_message);
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
// NppCommands — menu command bridge
// Each slot is connected to a Qt QAction signal from the menu system.
// =============================================================================

std::mutex command_mutex;

NppCommands::NppCommands(QObject* parent) : QObject(parent) {}

NppCommands::~NppCommands() {}

    void setMainWindow(MainWindow* mainWindow);

ScintillaEditView* NppCommands::activeView()
{
    if (!_mainWindow)
        return nullptr;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return nullptr;
    return npp->getActiveView();
}

bool NppCommands::isCurrentFileModified() const
{
    if (!_mainWindow)
        return false;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return false;
    Buffer* buf = npp->getActiveView()->getCurrentBuffer();
    return buf && buf->isDirty();
}

bool NppCommands::confirmSave(const QString& fileName)
{
    QMessageBox::StandardButton btn = QMessageBox::question(
        _mainWindow,
        tr("Save Changes?"),
        tr("Do you want to save changes to \"%1\"?").arg(fileName),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );
    return btn == QMessageBox::Save;
}

    void updateStatusBar();


// ===========================================================================
// FILE COMMANDS
// ===========================================================================

    void fileNew();

    void fileOpen();

    void fileOpenFolder();

    void fileOpenFolderAsWorkspace();

    void fileOpenContainingFolder();

    void fileOpenInCommandPrompt();

    void fileOpenAsReadOnly();

    void fileReload();

    void fileSave();

    void fileSaveAs();

    void fileSaveCopyAs();

    void fileSaveAll();

    void fileRename();

    void fileDelete();

    void fileClose();

    void fileCloseAll();

    void fileCloseAllButCurrent();

    void fileCloseAllToLeft();

    void fileCloseAllToRight();

    void fileCloseAllUnchanged();

    void fileLoadSession();

    void fileSaveSession();

bool NppCommands::filePrint()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return false;

    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return false;

    // Get the current document filename for the print job name
    Buffer* buf = view->getCurrentBuffer();
    QString jobName;
    if (buf) {
        const wchar_t* fn = buf->getFileName();
        if (fn && fn[0] != L'\0')
            jobName = QString::fromWCharArray(fn);
        else
            jobName = QStringLiteral("Untitled");
    } else {
        jobName = QStringLiteral("Untitled");
    }

    // Get selection range for partial document printing
    intptr_t startPos = view->execute(SCI_GETSELECTIONSTART);
    intptr_t endPos   = view->execute(SCI_GETSELECTIONEND);

    // Delegate to the Notepad_plus Printer pipeline
    npp->filePrint(true);

    // Note: Notepad_plus::filePrint(true) invokes Printer::doPrint() which
    // shows QPrintDialog and executes the print. We return true as long as
    // we had a valid view and could dispatch the command.
    // The Printer class handles dialog cancellation internally (returns 0 pages).
    return true;
}

    void fileExit();

    void fileRestoreLastClosed();


// ===========================================================================
// EDIT COMMANDS
// ===========================================================================

    void editCut();

    void editCopy();

    void editPaste();

    void editDelete();

    void editSelectAll();

    void editBeginEndSelect();

    void editBeginEndSelectColumnMode();

    void editUndo();

    void editRedo();

    void editFullPathToClip() {
        Buffer* buf = npp->getCurrentBuffer();
        if(buf && !buf->filePath().isEmpty()) QApplication::clipboard()->setText(buf->filePath());
    }

    void editFileNameToClip() {
        Buffer* buf = npp->getCurrentBuffer();
        if(buf && !buf->filePath().isEmpty()) QApplication::clipboard()->setText(QFileInfo(buf->filePath()).fileName());
    }

    void editCurrentDirToClip() {
        Buffer* buf = npp->getCurrentBuffer();
        if(buf && !buf->filePath().isEmpty()) QApplication::clipboard()->setText(QFileInfo(buf->filePath()).absolutePath());
    }

    void editCopyAllNames() {
        QStringList names;
        for(int i=0;i<100;i++) {
            Buffer* b=npp->getBufferFromIndex(i);
            if(!b) break;
            if(!b->filePath().isEmpty()) names.append(QFileInfo(b->filePath()).fileName());
        }
        if(!names.isEmpty()) QApplication::clipboard()->setText(names.join('\n'));
    }

    void editCopyAllPaths() {
        QStringList paths;
        for(int i=0;i<100;i++) {
            Buffer* b=npp->getBufferFromIndex(i);
            if(!b) break;
            if(!b->filePath().isEmpty()) paths.append(b->filePath());
        }
        if(!paths.isEmpty()) QApplication::clipboard()->setText(paths.join('\n'));
    }

    void editInsertTab();

    void editRemoveTab();

    void editUpperCase();

    void editLowerCase();

    void editProperCase();

    void editSentenceCase();

    void editInvertCase();

    void editRandomCase();

    void editRemoveConsecutiveDupLines();

    void editRemoveAnyDupLines();

    void editSplitLines();

    void editJoinLines();

    void editLineUp();

    void editLineDown();

    void editRemoveEmptyLines();

    void editRemoveEmptyLinesWithBlank();

    void editBlankLineAbove();

    void editBlankLineBelow();

    void editSortLines(int sortType);

    void editSortLexAsc();
    void editSortLexDesc();
    void editSortIntAsc();
    void editSortIntDesc();
    void editSortReversed();

    void editSortLinesImpl(int options);

    void editBlockComment();

    void editBlockUncomment();

    void editStreamComment();

    void editStreamUncomment();

    void editAutoComplete();

    void editAutoCompletePath();

    void editAutoCompleteCurrentFile();

    void editFuncCallTip();

    void editFuncCallTipPrevious();

    void editFuncCallTipNext();

    void editInsertDateTimeShort();

    void editInsertDateTimeLong();

    void editInsertDateTimeCustom();

    void editTrimTrailing();

    void editTrimLeading();

    void editTrimBoth();

    void editEolToWs();

    void editTrimAll();

    void editTabToSpace();

    void editSpaceToTabAll();

    void editSpaceToTabLeading();


// ===========================================================================
// SEARCH COMMANDS
// ===========================================================================

    void searchFind();

    void searchReplace();

    void searchFindNext();

    void searchFindPrev();

    void searchFindInFiles();

    void searchFindInProjects();

    void searchGoToLine();

    void searchGoToMatchingBrace();

    void searchSelectMatchingBraces();

    void searchMarkAllExt1();
    void searchMarkAllExt2();
    void searchMarkAllExt3();
    void searchMarkAllExt4();
    void searchMarkAllExt5();

    void searchClearAllMarks();

    void searchToggleBookmark();

    void searchNextBookmark();

    void searchPrevBookmark();

    void searchClearBookmarks();

    void searchCutMarkedLines();

    void searchCopyMarkedLines();

    void searchPasteMarkedLines();

    void searchDeleteMarkedLines();

    void searchDeleteUnmarkedLines();

    void searchInverseMarks();

    void searchFindCharInRange();


// ===========================================================================
// VIEW COMMANDS
// ===========================================================================

    void viewAlwaysOnTop();

    void viewFullScreen();

    void viewPostIt();

    void viewDistractionFree();

    void viewTabSpace();

    void viewEol();

    void viewAllCharacters();

    void viewNpc();

    void viewNpcCcuniEol();

    void viewIndentGuide();

    void viewWrapSymbol();

    void viewZoomIn();

    void viewZoomOut();

    void viewZoomRestore();

    void viewGotoStart();

    void viewGotoEnd();

    void viewSwitchToOtherView();

    void viewCloneToAnotherView();
    void viewGotoNewInstance();

    void viewLoadInNewInstance();
    void viewWrap();
    void viewHideLines();
    void viewFullScreenToggle();

    void viewSummary();


// ===========================================================================
// ENCODING COMMANDS
// ===========================================================================

    void encodingAnsi();

    void encodingUTF8();

    void encodingUTF8BOM();

    void encodingUTF16BE();

    void encodingUTF16LE();

    void encodingConvertAnsi();

    void encodingConvertUTF8();

    void encodingConvertUTF8BOM();

    void encodingConvertUTF16BE();

    void encodingConvertUTF16LE();


// ===========================================================================
// LANGUAGE COMMANDS
// ===========================================================================

    void languageMenu();

    void languageUserDefine();

    void languageOpenUdldDir();


// ===========================================================================
// SETTINGS COMMANDS
// ===========================================================================

    void settingsPreference();

    void settingsStyleConfig();

    void settingsShortcutMapper();

    void settingsImportPlugin();

    void settingsImportStyleThemes();

    void settingsEditContextMenu();


// ===========================================================================
// MACRO COMMANDS
// ===========================================================================

    void macroStartRecording();

    void macroStopRecording();

// Toggle recording: starts if not recording, stops if recording
    void macroStartStopRecord();

    void macroPlayback();

    void macroSaveCurrent();

    void macroRunMultiple();

    // Slot connected from RunMacroDlg::runMacro signal
    void runMacroFromDialog(int macroIndex, int times);

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

    void startMacroRecording();

    void stopMacroRecording();

    void recordMacroStep(int message, uintptr_t wParam, uintptr_t lParam);

    void recordMacroStepMenu(int commandId);

    void playMacro(const Macro& steps);

    void macroRunMultipleMacrosDialog();


// ===========================================================================
// RUN COMMANDS
// ===========================================================================

    void runExecute();

    void runCmdLineArgs();


// ===========================================================================
// WINDOW COMMANDS
// ===========================================================================

    void windowSwitchToDocument(int docIndex);

    void windowSortAscending();

    void windowSortDescending();


// ===========================================================================
// HELP COMMANDS
// ===========================================================================

    void helpAbout();

    void helpDocumentation();

    void helpHomeSweetHome();

    void helpOnlineDoc();


// ===========================================================================
// COLUMN EDITOR COMMANDS
// ===========================================================================

    void columnEditor();


// ===========================================================================
// CLIPBOARD COMMANDS
// ===========================================================================

    void clipboardHistoryPanel();

    void charPanel();


// ===========================================================================
// PANEL COMMANDS
// ===========================================================================

    void toggleDocumentMap();

    void toggleFunctionList();

    void toggleFileBrowser();

    void toggleProjectPanel1();
    void toggleProjectPanel2();
    void toggleProjectPanel3();

    void toggleClipboardHistory();

    void toggleDocList();


// ===========================================================================
// FIND/REPLACE PANEL COMMANDS
// ===========================================================================

    void findInFiles();

    void findInProjects();

    void findInFilelist();

    void replaceInFiles();

    void replaceInProjects();

    void replaceInFilelist();

    void findAllInOpenedDocs();

    void replaceAllInOpenedDocs();


// ===========================================================================
// TOGGLE COMMANDS
// ===========================================================================

    void toggleReadOnly();

    void setReadOnlyForAll();

    void clearReadOnlyForAll();

    void toggleRecording();

    void toggleMacroPlayback();

    void toggleMonitoring();

// =============================================================================
// NppCommands — Qt6 menu command bridge class
// =============================================================================
class NppCommands : public QObject
{
    Q_OBJECT

public:
    explicit NppCommands(QObject* parent = nullptr);
    ~NppCommands() override;

    void setMainWindow(MainWindow* mainWindow);
    ScintillaEditView* activeView();
    bool isCurrentFileModified() const;
    bool confirmSave(const QString& fileName);
    void updateStatusBar();

    // File commands
    void fileNew();
    void fileOpen();
    void fileOpenFolder();
    void fileOpenFolderAsWorkspace();
    void fileOpenContainingFolder();
    void fileOpenInCommandPrompt();
    void fileOpenAsReadOnly();
    void fileReload();
    bool fileSave();
    void fileSaveAs();
    void fileSaveCopyAs();
    void fileSaveAll();
    void fileRename();
    void fileDelete();
    void fileClose();
    void fileCloseAll();
    void fileCloseAllButCurrent();
    void fileCloseAllToLeft();
    void fileCloseAllToRight();
    void fileCloseAllUnchanged();
    void fileLoadSession();
    void fileSaveSession();
    bool filePrint();
    void fileExit();
    void fileRestoreLastClosed();

    // Edit commands
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editSelectAll();
    void editBeginEndSelect();
    void editBeginEndSelectColumnMode();
    void editUndo();
    void editRedo();
    void editFullPathToClip();
    void editFileNameToClip();
    void editCurrentDirToClip();
    void editCopyAllNames();
    void editCopyAllPaths();
    void editInsertTab();
    void editRemoveTab();
    void editUpperCase();
    void editLowerCase();
    void editProperCase();
    void editSentenceCase();
    void editInvertCase();
    void editRandomCase();
    void editRemoveConsecutiveDupLines();
    void editRemoveAnyDupLines();
    void editSplitLines();
    void editJoinLines();
    void editLineUp();
    void editLineDown();
    void editRemoveEmptyLines();
    void editRemoveEmptyLinesWithBlank();
    void editBlankLineAbove();
    void editBlankLineBelow();
    void editSortLines(int sortType);
    void editSortLexAsc();
    void editSortLexDesc();
    void editSortIntAsc();
    void editSortIntDesc();
    void editSortReversed();
    void editSortLinesImpl(int options);
    void editBlockComment();
    void editBlockUncomment();
    void editStreamComment();
    void editStreamUncomment();
    void editAutoComplete();
    void editAutoCompletePath();
    void editAutoCompleteCurrentFile();
    void editFuncCallTip();
    void editFuncCallTipPrevious();

private:
    MainWindow* _mainWindow = nullptr;
    bool _playingBackMacro = false;
};

