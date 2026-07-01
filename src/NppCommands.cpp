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
#include "NppCommands.h"
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
            step->playBack();
        }
        else
        {
            // Menu command playback via NppCommands dispatch
            // TODO: route through NppCommands slots
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
// NppCommands — menu command bridge
// Each slot is connected to a Qt QAction signal from the menu system.
// =============================================================================

std::mutex command_mutex;

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

void NppCommands::updateStatusBar()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->updateStatusBar();
}


// ===========================================================================
// FILE COMMANDS
// ===========================================================================

void NppCommands::fileNew()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileNew();
}

void NppCommands::fileOpen()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileOpen();
}

void NppCommands::fileOpenFolder()
{
    // Open folder via file dialog, add to File Browser
    QString dir = QFileDialog::getExistingDirectory(
        _mainWindow, tr("Open Folder"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
    );
    if (dir.isEmpty())
        return;

    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        // Route through command dispatcher
        npp->command(IDM_FILE_OPENFOLDERASWORKSPACE);
    }
    Q_UNUSED(dir);
}

void NppCommands::fileOpenFolderAsWorkspace()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FILE_OPENFOLDERASWORKSPACE);
}

void NppCommands::fileOpenContainingFolder()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FILE_CONTAININGFOLDERASWORKSPACE);
}

void NppCommands::fileOpenInCommandPrompt()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FILE_OPEN_CMD);
}

void NppCommands::fileOpenAsReadOnly()
{
    // Opens file in default viewer (system file association)
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FILE_OPEN_DEFAULT_VIEWER);
}

void NppCommands::fileReload()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileReload();
}

void NppCommands::fileSave()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileSave();
}

void NppCommands::fileSaveAs()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileSaveAs();
}

void NppCommands::fileSaveCopyAs()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileSaveAs(BUFFER_INVALID, true);
}

void NppCommands::fileSaveAll()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileSaveAll();
}

void NppCommands::fileRename()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileRename();
}

void NppCommands::fileDelete()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fileDelete();
}

void NppCommands::fileClose()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        bool result = npp->fileClose();
        if (result)
            npp->checkDocState();
    }
}

void NppCommands::fileCloseAll()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        bool isSnapshotMode = NppParameters::getInstance().getNppGUI().isSnapshotMode();
        npp->fileCloseAll(isSnapshotMode, false);
        npp->checkDocState();
    }
}

void NppCommands::fileCloseAllButCurrent()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        npp->fileCloseAllButCurrent();
        npp->checkDocState();
    }
}

void NppCommands::fileCloseAllToLeft()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        npp->fileCloseAllToLeft();
        npp->checkDocState();
    }
}

void NppCommands::fileCloseAllToRight()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        npp->fileCloseAllToRight();
        npp->checkDocState();
    }
}

void NppCommands::fileCloseAllUnchanged()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        npp->fileCloseAllUnchanged();
        npp->checkDocState();
    }
}

void NppCommands::fileLoadSession()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FILE_LOADSESSION);
}

void NppCommands::fileSaveSession()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FILE_SAVESESSION);
}

void NppCommands::filePrint()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->filePrint(true);
}

void NppCommands::fileExit()
{
    if (_mainWindow)
        _mainWindow->close();
}

void NppCommands::fileRestoreLastClosed()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FILE_RESTORELASTCLOSEDFILE);
}


// ===========================================================================
// EDIT COMMANDS
// ===========================================================================

void NppCommands::editCut()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    std::lock_guard<std::mutex> lock(command_mutex);

    if (view->hasSelection())
    {
        view->execute(WM_CUT);
    }
    else if (NppParameters::getInstance().getSVP()._lineCopyCutWithoutSelection)
    {
        // Cut entire line with EOL
        view->execute(SCI_COPYALLOWLINE);
        view->execute(SCI_LINEDELETE);
    }
    if (_mainWindow)
        qobject_cast<Notepad_plus*>(_mainWindow)->checkClipboard();
}

void NppCommands::editCopy()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    std::lock_guard<std::mutex> lock(command_mutex);

    if (view->hasSelection())
    {
        view->execute(WM_COPY);
    }
    else if (NppParameters::getInstance().getSVP()._lineCopyCutWithoutSelection)
    {
        view->execute(SCI_COPYALLOWLINE);
    }
    if (_mainWindow)
        qobject_cast<Notepad_plus*>(_mainWindow)->checkClipboard();
}

void NppCommands::editPaste()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    std::lock_guard<std::mutex> lock(command_mutex);

    size_t nbSelections = view->execute(SCI_GETSELECTIONS);
    Buffer* buf = view->getCurrentBuffer();
    bool isRO = buf->isReadOnly();
    LRESULT selectionMode = view->execute(SCI_GETSELECTIONMODE);

    if (nbSelections > 1 && !isRO && selectionMode == SC_SEL_STREAM)
    {
        bool isPasteDone = view->pasteToMultiSelection();
        if (isPasteDone)
            return;
    }

    view->execute(SCI_PASTE);
    if (_mainWindow)
        qobject_cast<Notepad_plus*>(_mainWindow)->checkClipboard();
}

void NppCommands::editDelete()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(WM_CLEAR);
}

void NppCommands::editSelectAll()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_SELECTALL);
    if (_mainWindow)
        qobject_cast<Notepad_plus*>(_mainWindow)->checkClipboard();
}

void NppCommands::editBeginEndSelect()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_BEGINENDSELECT);
}

void NppCommands::editBeginEndSelectColumnMode()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_BEGINENDSELECT_COLUMNMODE);
}

void NppCommands::editUndo()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    std::lock_guard<std::mutex> lock(command_mutex);
    view->execute(WM_UNDO);
    if (_mainWindow)
    {
        auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
        if (npp)
        {
            npp->checkClipboard();
            npp->checkUndoState();
        }
    }
}

void NppCommands::editRedo()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    std::lock_guard<std::mutex> lock(command_mutex);
    view->execute(SCI_REDO);
    if (_mainWindow)
    {
        auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
        if (npp)
        {
            npp->checkClipboard();
            npp->checkUndoState();
        }
    }
}

void NppCommands::editFullPathToClip()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_FULLPATHTOCLIP);
}

void NppCommands::editFileNameToClip()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_FILENAMETOCLIP);
}

void NppCommands::editCurrentDirToClip()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_CURRENTDIRTOCLIP);
}

void NppCommands::editCopyAllNames()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_COPY_ALL_NAMES);
}

void NppCommands::editCopyAllPaths()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_COPY_ALL_PATHS);
}

void NppCommands::editInsertTab()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_INS_TAB);
}

void NppCommands::editRemoveTab()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_RMV_TAB);
}

void NppCommands::editUpperCase()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->convertSelectedTextToUpperCase();
}

void NppCommands::editLowerCase()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->convertSelectedTextToLowerCase();
}

void NppCommands::editProperCase()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_PROPERCASE_FORCE);
}

void NppCommands::editSentenceCase()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_SENTENCECASE_FORCE);
}

void NppCommands::editInvertCase()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_INVERTCASE);
}

void NppCommands::editRandomCase()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_RANDOMCASE);
}

void NppCommands::editRemoveConsecutiveDupLines()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_BEGINUNDOACTION);
    view->removeDuplicateLines();
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editRemoveAnyDupLines()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_BEGINUNDOACTION);
    view->removeDuplicateLines();
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editSplitLines()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    if (view->execute(SCI_GETSELECTIONS) == 1)
    {
        auto lineRange = view->getSelectionLinesRange();
        auto anchorPos = view->execute(SCI_POSITIONFROMLINE, lineRange.first);
        auto caretPos = view->execute(SCI_GETLINEENDPOSITION, lineRange.second);
        view->execute(SCI_SETSELECTION, caretPos, anchorPos);
        view->execute(SCI_TARGETFROMSELECTION);
        size_t edgeMode = view->execute(SCI_GETEDGEMODE);
        if (edgeMode == EDGE_NONE)
        {
            view->execute(SCI_LINESSPLIT, 0);
        }
        else
        {
            auto textWidth = view->execute(SCI_TEXTWIDTH, STYLE_DEFAULT, reinterpret_cast<sptr_t>("P"));
            auto edgeCol = view->execute(SCI_GETEDGECOLUMN);
            if (edgeMode == EDGE_MULTILINE)
            {
                NppParameters& nppParam = NppParameters::getInstance();
                ScintillaViewParams& svp = const_cast<ScintillaViewParams&>(nppParam.getSVP());
                edgeCol = svp._edgeMultiColumnPos.back();
            }
            ++edgeCol;
            view->execute(SCI_LINESSPLIT, textWidth * edgeCol);
        }
    }
}

void NppCommands::editJoinLines()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    auto lineRange = view->getSelectionLinesRange();
    if (lineRange.first != lineRange.second)
    {
        auto anchorPos = view->execute(SCI_POSITIONFROMLINE, lineRange.first);
        auto caretPos = view->execute(SCI_GETLINEENDPOSITION, lineRange.second);
        view->execute(SCI_SETSELECTION, caretPos, anchorPos);
        view->execute(SCI_TARGETFROMSELECTION);
        view->execute(SCI_LINESJOIN);
    }
}

void NppCommands::editLineUp()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->currentLinesUp();
}

void NppCommands::editLineDown()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->currentLinesDown();
}

void NppCommands::editRemoveEmptyLines()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_BEGINUNDOACTION);
    view->removeEmptyLine(false);
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editRemoveEmptyLinesWithBlank()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_BEGINUNDOACTION);
    view->removeEmptyLine(true);
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editBlankLineAbove()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_BLANKLINEABOVECURRENT);
}

void NppCommands::editBlankLineBelow()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_BLANKLINEBELOWCURRENT);
}

void NppCommands::editSortLines(int sortType)
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        // Map sortType to Win32 command ID
        // sortType values match LPARAM passed from menu
        // This slot is called directly from menu actions
        Q_UNUSED(sortType);
    }
}

void NppCommands::editBlockComment()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_BLOCK_COMMENT);
}

void NppCommands::editBlockUncomment()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_BLOCK_UNCOMMENT);
}

void NppCommands::editStreamComment()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_STREAM_COMMENT);
}

void NppCommands::editStreamUncomment()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_STREAM_UNCOMMENT);
}

void NppCommands::editAutoComplete()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->showAutoComp();
}

void NppCommands::editAutoCompletePath()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->showPathCompletion();
}

void NppCommands::editAutoCompleteCurrentFile()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->autoCompFromCurrentFile(true);
}

void NppCommands::editFuncCallTip()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->showFunctionComp();
}

void NppCommands::editFuncCallTipPrevious()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->showFunctionNextHint(false);
}

void NppCommands::editFuncCallTipNext()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->showFunctionNextHint(true);
}

void NppCommands::editInsertDateTimeShort()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_INSERT_DATETIME_SHORT);
}

void NppCommands::editInsertDateTimeLong()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_INSERT_DATETIME_LONG);
}

void NppCommands::editInsertDateTimeCustom()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_INSERT_DATETIME_CUSTOMIZED);
}

void NppCommands::editTrimTrailing()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_TRIMTRAILING);
}

void NppCommands::editTrimLeading()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_TRIMLINEHEAD);
}

void NppCommands::editTrimBoth()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_TRIM_BOTH);
}

void NppCommands::editEolToWs()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_EOL2WS);
}

void NppCommands::editTrimAll()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_TRIMALL);
}

void NppCommands::editTabToSpace()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_TAB2SW);
}

void NppCommands::editSpaceToTabAll()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_SW2TAB_ALL);
}

void NppCommands::editSpaceToTabLeading()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_SW2TAB_LEADING);
}


// ===========================================================================
// SEARCH COMMANDS
// ===========================================================================

void NppCommands::searchFind()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_FIND);
}

void NppCommands::searchReplace()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_REPLACE);
}

void NppCommands::searchFindNext()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_FINDNEXT);
}

void NppCommands::searchFindPrev()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_FINDPREV);
}

void NppCommands::searchFindInFiles()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->findInFiles();
}

void NppCommands::searchFindInProjects()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->findInProjects();
}

void NppCommands::searchGoToLine()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_GOTOLINE);
}

void NppCommands::searchGoToMatchingBrace()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_GOTOMATCHINGBRACE);
}

void NppCommands::searchSelectMatchingBraces()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_SELECTMATCHINGBRACES);
}

void NppCommands::searchMarkAllExt1()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_MARKALLEXT1);
}

void NppCommands::searchMarkAllExt2()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_MARKALLEXT2);
}

void NppCommands::searchMarkAllExt3()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_MARKALLEXT3);
}

void NppCommands::searchMarkAllExt4()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_MARKALLEXT4);
}

void NppCommands::searchMarkAllExt5()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_MARKALLEXT5);
}

void NppCommands::searchClearAllMarks()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_EXT1);
    view->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_EXT2);
    view->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_EXT3);
    view->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_EXT4);
    view->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_EXT5);
}

void NppCommands::searchToggleBookmark()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_TOGGLE_BOOKMARK);
}

void NppCommands::searchNextBookmark()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_NEXT_BOOKMARK);
}

void NppCommands::searchPrevBookmark()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_PREV_BOOKMARK);
}

void NppCommands::searchClearBookmarks()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_CLEAR_BOOKMARKS);
}

void NppCommands::searchCutMarkedLines()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_CUTMARKEDLINES);
}

void NppCommands::searchCopyMarkedLines()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_COPYMARKEDLINES);
}

void NppCommands::searchPasteMarkedLines()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_PASTEMARKEDLINES);
}

void NppCommands::searchDeleteMarkedLines()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_DELETEMARKEDLINES);
}

void NppCommands::searchDeleteUnmarkedLines()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_DELETEUNMARKEDLINES);
}

void NppCommands::searchInverseMarks()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_INVERSEMARKS);
}

void NppCommands::searchFindCharInRange()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SEARCH_FINDCHARINRANGE);
}


// ===========================================================================
// VIEW COMMANDS
// ===========================================================================

void NppCommands::viewAlwaysOnTop()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_ALWAYSONTOP);
}

void NppCommands::viewFullScreen()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->fullScreenToggle();
}

void NppCommands::viewPostIt()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->postItToggle();
}

void NppCommands::viewDistractionFree()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->distractionFreeToggle();
}

void NppCommands::viewTabSpace()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_TAB_SPACE);
}

void NppCommands::viewEol()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_EOL);
}

void NppCommands::viewAllCharacters()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_ALL_CHARACTERS);
}

void NppCommands::viewNpc()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_NPC);
}

void NppCommands::viewNpcCcuniEol()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_NPC_CCUNIEOL);
}

void NppCommands::viewIndentGuide()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_INDENT_GUIDE);
}

void NppCommands::viewWrapSymbol()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_WRAP_SYMBOL);
}

void NppCommands::viewZoomIn()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_ZOOMIN);
    if (_mainWindow)
    {
        auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
        if (npp)
            npp->syncZoom();
    }
}

void NppCommands::viewZoomOut()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_ZOOMOUT);
    if (_mainWindow)
    {
        auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
        if (npp)
            npp->syncZoom();
    }
}

void NppCommands::viewZoomRestore()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_SETZOOM, 0);
    if (_mainWindow)
    {
        auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
        if (npp)
            npp->syncZoom();
    }
}

void NppCommands::viewGotoStart()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_GOTO_START);
}

void NppCommands::viewGotoEnd()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_GOTO_END);
}

void NppCommands::viewSwitchToOtherView()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_SWITCHTO_OTHER_VIEW);
}

void NppCommands::viewCloneToAnotherView()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_CLONE_TO_ANOTHER_VIEW);
}

void NppCommands::viewGotoNewInstance()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_GOTO_NEW_INSTANCE);
}

void NppCommands::viewLoadInNewInstance()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_LOAD_IN_NEW_INSTANCE);
}

void NppCommands::viewWrap()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_WRAP);
}

void NppCommands::viewHideLines()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_HIDELINES);
}

void NppCommands::viewFullScreenToggle()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_FULLSCREENTOGGLE);
}

void NppCommands::viewSummary()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_SUMMARY);
}


// ===========================================================================
// ENCODING COMMANDS
// ===========================================================================

void NppCommands::encodingAnsi()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_ANSI);
}

void NppCommands::encodingUTF8()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_UTF_8);
}

void NppCommands::encodingUTF8BOM()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_UTF_8);
}

void NppCommands::encodingUTF16BE()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_UTF_16BE);
}

void NppCommands::encodingUTF16LE()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_UTF_16LE);
}

void NppCommands::encodingConvertAnsi()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_CONV2_ANSI);
}

void NppCommands::encodingConvertUTF8()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_CONV2_UTF_8);
}

void NppCommands::encodingConvertUTF8BOM()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_CONV2_AS_UTF_8);
}

void NppCommands::encodingConvertUTF16BE()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_CONV2_UTF_16BE);
}

void NppCommands::encodingConvertUTF16LE()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_FORMAT_CONV2_UTF_16LE);
}


// ===========================================================================
// LANGUAGE COMMANDS
// ===========================================================================

void NppCommands::languageMenu()
{
    // Language submenu - handled by QMenu::aboutToShow
    // Individual language commands are dispatched by ID
}

void NppCommands::languageUserDefine()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_LANG_USER_DLG);
}

void NppCommands::languageOpenUdldDir()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_LANG_OPENUDLDIR);
}


// ===========================================================================
// SETTINGS COMMANDS
// ===========================================================================

void NppCommands::settingsPreference()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SETTING_PREFERENCE);
}

void NppCommands::settingsStyleConfig()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_LANGSTYLE_CONFIG_DLG);
}

void NppCommands::settingsShortcutMapper()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SETTING_SHORTCUT_MAPPER);
}

void NppCommands::settingsImportPlugin()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SETTING_IMPORTPLUGIN);
}

void NppCommands::settingsImportStyleThemes()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SETTING_IMPORTSTYLETHEMES);
}

void NppCommands::settingsEditContextMenu()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_SETTING_EDITCONTEXTMENU);
}


// ===========================================================================
// MACRO COMMANDS
// ===========================================================================

void NppCommands::macroStartRecording()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_MACRO_STARTRECORDINGMACRO);
}

void NppCommands::macroStopRecording()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_MACRO_STOPRECORDINGMACRO);
}

void NppCommands::macroPlayback()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_MACRO_PLAYBACKRECORDEDMACRO);
}

void NppCommands::macroSaveCurrent()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_MACRO_SAVECURRENTMACRO);
}

void NppCommands::macroRunMultiple()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_MACRO_RUNMULTIMACRODLG);
}


// ===========================================================================
// RUN COMMANDS
// ===========================================================================

void NppCommands::runExecute()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EXECUTE);
}

void NppCommands::runCmdLineArgs()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_CMDLINEARGUMENTS);
}


// ===========================================================================
// WINDOW COMMANDS
// ===========================================================================

void NppCommands::windowSwitchToDocument(int docIndex)
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->activateDoc(static_cast<size_t>(docIndex));
}

void NppCommands::windowSortAscending()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_WINDOW_SORT_FN_ASC);
}

void NppCommands::windowSortDescending()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_WINDOW_SORT_FN_DSC);
}


// ===========================================================================
// HELP COMMANDS
// ===========================================================================

void NppCommands::helpAbout()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_ABOUT);
}

void NppCommands::helpDocumentation()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_ONLINEDOCUMENT);
}

void NppCommands::helpHomeSweetHome()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_HOMESWEETHOME);
}

void NppCommands::helpOnlineDoc()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_PROJECTPAGE);
}


// ===========================================================================
// COLUMN EDITOR COMMANDS
// ===========================================================================

void NppCommands::columnEditor()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_COLUMNMODE);
}


// ===========================================================================
// CLIPBOARD COMMANDS
// ===========================================================================

void NppCommands::clipboardHistoryPanel()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_CLIPBOARDHISTORY_PANEL);
}

void NppCommands::charPanel()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_CHAR_PANEL);
}


// ===========================================================================
// PANEL COMMANDS
// ===========================================================================

void NppCommands::toggleDocumentMap()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_DOC_MAP);
}

void NppCommands::toggleFunctionList()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_FUNC_LIST);
}

void NppCommands::toggleFileBrowser()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_FILEBROWSER);
}

void NppCommands::toggleProjectPanel1()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_PROJECT_PANEL_1);
}

void NppCommands::toggleProjectPanel2()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_PROJECT_PANEL_2);
}

void NppCommands::toggleProjectPanel3()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_PROJECT_PANEL_3);
}

void NppCommands::toggleClipboardHistory()
{
    clipboardHistoryPanel();
}

void NppCommands::toggleDocList()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_DOCLIST);
}


// ===========================================================================
// FIND/REPLACE PANEL COMMANDS
// ===========================================================================

void NppCommands::findInFiles()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->findInFiles();
}

void NppCommands::findInProjects()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->findInProjects();
}

void NppCommands::findInFilelist()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        // findInFilelist needs file list argument
        // Triggered via command dispatcher
        npp->command(IDM_SEARCH_FINDINFILES);
    }
}

void NppCommands::replaceInFiles()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->replaceInFiles();
}

void NppCommands::replaceInProjects()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->replaceInProjects();
}

void NppCommands::replaceInFilelist()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        // replaceInFilelist needs file list argument
        npp->command(IDM_SEARCH_FINDINFILES);
    }
}

void NppCommands::findAllInOpenedDocs()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->findInOpenedFiles();
}

void NppCommands::replaceAllInOpenedDocs()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->replaceInOpenedFiles();
}


// ===========================================================================
// TOGGLE COMMANDS
// ===========================================================================

void NppCommands::toggleReadOnly()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_TOGGLEREADONLY);
}

void NppCommands::setReadOnlyForAll()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_SETREADONLYFORALLDOCS);
}

void NppCommands::clearReadOnlyForAll()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_EDIT_CLEARREADONLYFORALLDOCS);
}

void NppCommands::toggleRecording()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDC_EDIT_TOGGLEMACRORECORDING);
}

void NppCommands::toggleMacroPlayback()
{
    macroPlayback();
}

void NppCommands::toggleMonitoring()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->command(IDM_VIEW_MONITORING);
}
