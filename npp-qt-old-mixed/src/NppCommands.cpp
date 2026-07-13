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

// Forward declarations to break circular includes
class PreferenceDialog;
class StyleConfigDialog;
class ShortcutMapperDialog;
class TaskListDlg;
class VerticalFileSwitcher;
class documentMap;
class functionListPanel;
class ProjectPanel;
class fileBrowser;
class clipboardHistoryPanel;
class Sorters;
class verifySignedfile;
class QTimer;

#include "NppConstants.h"
#include "ScintillaComponent.h"
#include "ScintillaComponent/UserDefineDialog.h"


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

        // playBack handles all macro types (scintilla + menu commands) internally
        step->playBack(_pEditView);
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
        // Launch file browser with the selected directory
        std::vector<std::wstring> folders;
        folders.push_back(dir.toStdWString());
        npp->launchFileBrowser(folders, std::wstring(), true);
    }
}

void NppCommands::fileOpenFolderAsWorkspace()
{
    // Open folder via file dialog and add to File Browser
    QString dir = QFileDialog::getExistingDirectory(
        _mainWindow, tr("Open Folder as Workspace"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
    );
    if (dir.isEmpty())
        return;

    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
    {
        std::vector<std::wstring> folders;
        folders.push_back(dir.toStdWString());
        npp->launchFileBrowser(folders, std::wstring(), true);
    }
}

void NppCommands::fileOpenContainingFolder()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return;

    Buffer* buf = npp->getCurrentBuffer();
    if (!buf)
        return;

    QString filePath = buf->filePath();
    if (filePath.isEmpty()) {
        QMessageBox::information(_mainWindow, tr("Open Containing Folder"),
                                 tr("No file path available. Save the file first."));
        return;
    }

    QString dir = QFileInfo(filePath).absolutePath();
    std::vector<std::wstring> folders;
    folders.push_back(dir.toStdWString());
    npp->launchFileBrowser(folders, std::wstring(), true);
}

void NppCommands::fileOpenInCommandPrompt()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return;

    Buffer* buf = npp->getCurrentBuffer();
    QString dir;
    if (buf && !buf->filePath().isEmpty()) {
        dir = QFileInfo(buf->filePath()).absolutePath();
    } else {
        dir = QDir::currentPath();
    }

    // Try common terminal emulators
    QStringList terminals = {"x-terminal-emulator", "gnome-terminal", "konsole", "xfce4-terminal", "qterminal"};
    QString terminal;
    for (const QString& t : terminals) {
        if (QStandardPaths::findExecutable(t).isEmpty() == false) {
            terminal = t;
            break;
        }
    }

    if (terminal.isEmpty()) {
        QMessageBox::information(_mainWindow, tr("Open in Terminal"),
                                 tr("No terminal emulator found on this system."));
        return;
    }

    // Start terminal in the file's directory
    if (terminal == "gnome-terminal") {
        QProcess::startDetached("gnome-terminal", QStringList() << "--working-directory=" + dir);
    } else if (terminal == "konsole") {
        QProcess::startDetached("konsole", QStringList() << "--workdir" << dir);
    } else if (terminal == "xfce4-terminal") {
        QProcess::startDetached("xfce4-terminal", QStringList() << "--working-directory=" + dir);
    } else {
        QProcess::startDetached(terminal, QStringList(), dir);
    }
}

void NppCommands::fileOpenAsReadOnly()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return;

    Buffer* buf = npp->getCurrentBuffer();
    if (!buf || buf->filePath().isEmpty()) {
        QMessageBox::information(_mainWindow, tr("Open with Default Application"),
                                 tr("No file path available. Save the file first."));
        return;
    }

    QUrl url = QUrl::fromLocalFile(buf->filePath());
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox::warning(_mainWindow, tr("Open with Default Application"),
                            tr("Failed to open the file with the default application."));
    }
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
        // Capture path before close for Restore Last Closed
        Buffer* buf = npp->getCurrentBuffer();
        if (buf && !buf->filePath().isEmpty())
            pushRecentClosed(buf->filePath());
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

void NppCommands::fileLoadSession() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    QString fileName = QFileDialog::getOpenFileName(nullptr,
        QStringLiteral("Load Session"), QStringLiteral(""),
        QStringLiteral("Notepad++ Session (*.session);;All Files (*.*)"));
    if (fileName.isEmpty()) return;
    Session session;
    bool ok = NppParameters::getInstance().loadSession(
        session, fileName.toStdWString().c_str(), false);
    if (ok) {
        npp->loadSession(session, false, fileName.toStdWString().c_str());
    } else {
        QMessageBox::warning(_mainWindow, QStringLiteral("Load Session"),
            QStringLiteral("Failed to load session from:\n%1").arg(fileName));
    }
}

void NppCommands::fileSaveSession() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    QString fileName = QFileDialog::getSaveFileName(nullptr,
        QStringLiteral("Save Session"), QStringLiteral(""),
        QStringLiteral("Notepad++ Session (*.session);;All Files (*.*)"));
    if (fileName.isEmpty()) return;
    const wchar_t* fn = fileName.toStdWString().c_str();
    npp->fileSaveSession(0, nullptr, fn);
}

bool NppCommands::filePrint()
{
    if (!_mainWindow)
        return false;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return false;

    // Delegate to Notepad_plus, which creates a Printer and calls doPrint().
    // Passing true = show print dialog.
    npp->filePrint(true);
    return true;
}

void NppCommands::fileExit()
{
    if (_mainWindow)
        _mainWindow->close();
}

void NppCommands::fileRestoreLastClosed() {
    if (_recentClosedFiles.isEmpty()) {
        QMessageBox::information(nullptr, QStringLiteral("Restore File"),
            QStringLiteral("No recently closed files to restore."));
        return;
    }
    QString path = _recentClosedFiles.dequeue();
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    if (!QFileInfo::exists(path)) {
        QMessageBox::warning(nullptr, QStringLiteral("Restore File"),
            QStringLiteral("File no longer exists:\n%1").arg(path));
        return;
    }
    npp->doOpen(path.toStdWString());
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
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    // Toggle between stream selection and column (rectangular) selection mode.
    // Begin/End Select in Notepad++ toggles the selection mode.
    long selMode = view->send(SCI_GETSELECTIONMODE);
    view->send(SCI_BEGINUNDOACTION);
    if (selMode == SC_SEL_STREAM)
        view->send(SCI_SETSELECTIONMODE, SC_SEL_RECTANGLE);
    else
        view->send(SCI_SETSELECTIONMODE, SC_SEL_STREAM);
    view->send(SCI_ENDUNDOACTION);
}

void NppCommands::editBeginEndSelectColumnMode()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    // Enter column/rectangular selection mode
    view->send(SCI_BEGINUNDOACTION);
    view->send(SCI_SETSELECTIONMODE, SC_SEL_RECTANGLE);
    view->send(SCI_ENDUNDOACTION);
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

void NppCommands::editFullPathToClip() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(!npp) return;
      Buffer* buf = npp->getCurrentBuffer();
      if(buf && !buf->filePath().isEmpty()) QApplication::clipboard()->setText(buf->filePath()); }

void NppCommands::editFileNameToClip() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(!npp) return;
      Buffer* buf = npp->getCurrentBuffer();
      if(buf && !buf->filePath().isEmpty()) QApplication::clipboard()->setText(QFileInfo(buf->filePath()).fileName()); }

void NppCommands::editCurrentDirToClip() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(!npp) return;
      Buffer* buf = npp->getCurrentBuffer();
      if(buf && !buf->filePath().isEmpty()) QApplication::clipboard()->setText(QFileInfo(buf->filePath()).absolutePath()); }

void NppCommands::editCopyAllNames() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(!npp) return;
      QStringList names;
      for(int i=0;i<100;i++){ Buffer* b=npp->getBufferFromIndex(i); if(!b) break;
        if(!b->filePath().isEmpty()) names.append(QFileInfo(b->filePath()).fileName()); }
      if(!names.isEmpty()) QApplication::clipboard()->setText(names.join('\n')); }

void NppCommands::editCopyAllPaths() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(!npp) return;
      QStringList paths;
      for(int i=0;i<100;i++){ Buffer* b=npp->getBufferFromIndex(i); if(!b) break;
        if(!b->filePath().isEmpty()) paths.append(b->filePath()); }
      if(!paths.isEmpty()) QApplication::clipboard()->setText(paths.join('\n')); }

void NppCommands::editInsertTab() { auto* view = activeView(); if(view) view->send(2327, 0); }

void NppCommands::editRemoveTab()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    // Remove trailing tab from each selected line
    view->execute(SCI_BEGINUNDOACTION);
    long start = view->send(SCI_GETSELECTIONSTART);
    long end = view->send(SCI_GETSELECTIONEND);
    long startLine = view->send(SCI_LINEFROMPOSITION, start);
    long endLine = view->send(SCI_LINEFROMPOSITION, end);
    for (long line = startLine; line <= endLine; ++line) {
        size_t lineStart = view->send(SCI_POSITIONFROMLINE, line);
        size_t lineEnd = view->send(SCI_GETLINEENDPOSITION, line);
        if (lineEnd > lineStart) {
            size_t textEnd = lineEnd;
            // Check if last character is a tab
            char ch = static_cast<char>(view->send(SCI_GETCHARAT, textEnd - 1));
            if (ch == '\t') {
                view->send(SCI_SETTARGETSTART, textEnd - 1);
                view->send(SCI_SETTARGETEND, textEnd);
                view->send(SCI_REPLACETARGET, 0, 0);  // delete the tab
            }
        }
    }
    view->execute(SCI_ENDUNDOACTION);
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

void NppCommands::editProperCase() { auto* view = activeView(); if(!view) return; long s=view->send(2145),e=view->send(2146); if(e<=s) return; int l=e-s; QByteArray raw(l+1,'\0'); view->send(2162,0,reinterpret_cast<sptr_t>(raw.data())); QString t=QString::fromUtf8(raw.constData()); bool nw=true; for(int i=0;i<t.length();++i){ if(t[i].isSpace()) nw=true; else if(nw){ t[i]=t[i].toUpper(); nw=false; } else t[i]=t[i].toLower(); } view->send(2169,0,reinterpret_cast<sptr_t>(t.toUtf8().constData())); }

void NppCommands::editSentenceCase() { auto* view = activeView(); if(!view) return; long s=view->send(2145),e=view->send(2146); if(e<=s) return; int l=e-s; QByteArray raw(l+1,'\0'); view->send(2162,0,reinterpret_cast<sptr_t>(raw.data())); QString t=QString::fromUtf8(raw.constData()).toLower(); bool ns=true; for(int i=0;i<t.length();++i){ if(t[i]=='.'||t[i]=='!'||t[i]=='?') ns=true; else if(ns&&!t[i].isSpace()){ t[i]=t[i].toUpper(); ns=false; } } view->send(2169,0,reinterpret_cast<sptr_t>(t.toUtf8().constData())); }

void NppCommands::editInvertCase() { auto* view = activeView(); if(!view) return; long s=view->send(2145),e=view->send(2146); if(e<=s) return; int l=e-s; QByteArray raw(l+1,'\0'); view->send(2162,0,reinterpret_cast<sptr_t>(raw.data())); QString t=QString::fromUtf8(raw.constData()); for(int i=0;i<t.length();++i){ if(t[i].isLower()) t[i]=t[i].toUpper(); else if(t[i].isUpper()) t[i]=t[i].toLower(); } view->send(2169,0,reinterpret_cast<sptr_t>(t.toUtf8().constData())); }

void NppCommands::editRandomCase() { auto* view = activeView(); if(!view) return; long s=view->send(2145),e=view->send(2146); if(e<=s) return; int l=e-s; QByteArray raw(l+1,'\0'); view->send(2162,0,reinterpret_cast<sptr_t>(raw.data())); QString t=QString::fromUtf8(raw.constData()); for(int i=0;i<t.length();++i){ if(t[i].isLetter()) t[i]=(i%2==0)?t[i].toUpper():t[i].toLower(); } view->send(2169,0,reinterpret_cast<sptr_t>(t.toUtf8().constData())); }

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
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_BEGINUNDOACTION);
    // Get current line and go to its start
    size_t currentLine = view->getCurrentLineNumber();
    size_t lineStart = view->send(SCI_POSITIONFROMLINE, currentLine);
    // Insert a newline before the current line
    view->send(SCI_INSERTTEXT, lineStart, reinterpret_cast<sptr_t>(QStringLiteral("\n").utf16()));
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editBlankLineBelow()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_BEGINUNDOACTION);
    // Get current line and go to its end (after the EOL)
    size_t currentLine = view->getCurrentLineNumber();
    size_t lineEnd = view->send(SCI_GETLINEENDPOSITION, currentLine);
    // Insert a newline after the current line
    view->send(SCI_INSERTTEXT, lineEnd, reinterpret_cast<sptr_t>(QStringLiteral("\n").utf16()));
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editSortLines(int sortType)
{
    switch (sortType) {
        case 0:  editSortLexAsc();   break;
        case 1:  editSortLexDesc();  break;
        case 2:  editSortIntAsc();   break;
        case 3:  editSortIntDesc();  break;
        case 4:  editSortReversed(); break;
        default: editSortLexAsc();   break;
    }
}

void NppCommands::editSortLexAsc()   { editSortLinesImpl(ASC | LEX); }
void NppCommands::editSortLexDesc()  { editSortLinesImpl(DESC | LEX); }
void NppCommands::editSortIntAsc()   { editSortLinesImpl(ASC | INTEGER); }
void NppCommands::editSortIntDesc()  { editSortLinesImpl(DESC | INTEGER); }
void NppCommands::editSortReversed() { editSortLinesImpl(REVERSED); }

void NppCommands::editSortLinesImpl(int options)
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;

    long start = view->send(SCI_GETSELECTIONSTART);
    long end = view->send(SCI_GETSELECTIONEND);
    if (end <= start)
        return;

    int len = static_cast<int>(end - start);
    QByteArray raw(len + 1, '\0');
    view->send(SCI_GETSELTEXT, 0, reinterpret_cast<sptr_t>(raw.data()));
    raw.truncate(len);

    QString text = QString::fromUtf8(raw.constData());
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    bool asc = (options & ASC);
    bool numeric = (options & INTEGER);
    bool reversed = (options & REVERSED);

    if (numeric) {
        QVector<QPair<int, QString>> pairs;
        for (const QString& l : lines) {
            QRegularExpression re("(-?\\d+)");
            QRegularExpressionMatch match = re.match(l);
            int num = 0;
            if (match.hasMatch())
                num = match.captured(1).toInt();
            pairs.append({num, l});
        }
        if (reversed) {
            std::reverse(pairs.begin(), pairs.end());
        } else {
            if (asc) std::sort(pairs.begin(), pairs.end(), [](auto& a, auto& b){ return a.first < b.first; });
            else std::sort(pairs.begin(), pairs.end(), [](auto& a, auto& b){ return a.first > b.first; });
        }
        lines.clear();
        for (auto& p : pairs) lines.append(p.second);
    } else {
        if (reversed) {
            std::reverse(lines.begin(), lines.end());
        } else {
            if (asc) lines.sort();
            else {
                lines.sort();
                std::reverse(lines.begin(), lines.end());
            }
        }
    }

    QString sorted = lines.join('\n');
    view->send(SCI_TARGETFROMSELECTION);
    view->send(SCI_SETTARGETRANGE, start, end);
    view->send(SCI_REPLACETARGET, static_cast<uptr_t>(-1), reinterpret_cast<sptr_t>(sorted.toUtf8().constData()));
}

void NppCommands::editBlockComment()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    // Get current language type
    Buffer* buf = view->getCurrentBuffer();
    LangType lang = buf ? buf->getLangType() : L_TEXT;

    // Determine comment prefix and whether it uses stream (/* */) or line (//) style
    QString prefix;
    bool useStream = false;
    switch (lang) {
        // C family: C, C++, C#, Java, JavaScript, PHP, Objective-C, Swift, Go, Rust
        case L_C: case L_CPP: case L_CS: case L_JAVA: case L_JAVASCRIPT:
        case L_PHP: case L_OBJC: case L_SWIFT: case L_GOLANG: case L_RUST:
        case L_TYPESCRIPT: case L_JSON: case L_YAML: case L_TOML:
        case L_CMAKE: case L_RC: case L_FLASH: case L_NSIS: case L_INNO:
        case L_D: case L_ESCRIPT: case L_AVS: case L_BLITZBASIC: case L_PUREBASIC:
        case L_FREEBASIC: case L_CSOUND: case L_MMIXAL: case L_GDSCRIPT:
        case L_RAKU: case L_SAS:
            prefix = "//";
            break;
        // Python, Shell, Perl, Ruby, Makefile, CMake, PowerShell, YAML, etc.
        case L_PYTHON: case L_PERL: case L_RUBY: case L_BASH:
        case L_MAKEFILE: case L_PS: case L_LUA: case L_R: case L_POWERSHELL:
        case L_BATCH: case L_AU3: case L_MARKDOWN: case L_TXT2TAGS:
        case L_TCL: case L_LISP: case L_SCHEME: case L_NIM:
        case L_COFFEESCRIPT: case L_FORTRAN: case L_FORTRAN_77:
        case L_ADA: case L_PROPS: case L_VERILOG: case L_VHDL: case L_FORTH:
        case L_CAML: case L_HASKELL: case L_ERLANG: case L_PASCAL:
        case L_SMALLTALK: case L_KIX: case L_OSCRIPT:
        case L_REBOL: case L_NNCRONTAB: case L_SPICE: case L_GUI4CLI:
            prefix = "#";
            break;
        // Assembly — semicolon
        case L_ASM:
            prefix = ";";
            break;
        // SQL — double dash
        case L_SQL: case L_MSSQL:
            prefix = "--";
            break;
        // HTML/XML/ASP — stream comment <!-- -->
        case L_HTML: case L_XML: case L_ASP: case L_JSP:
        case L_USER: case L_TEX: case L_LATEX:
        default:
            // Default to stream comment style for ambiguous/unknown languages
            useStream = true;
            break;
    }

    view->execute(SCI_BEGINUNDOACTION);

    if (useStream) {
        // Wrap selection with <!-- and -->
        long start = view->send(SCI_GETSELECTIONSTART);
        long end = view->send(SCI_GETSELECTIONEND);
        view->send(SCI_SETTARGETSTART, start);
        view->send(SCI_SETTARGETEND, end);
        view->send(SCI_REPLACETARGET, -1, reinterpret_cast<sptr_t>(QStringLiteral("<!--  -->").utf16()));
        // Insert <!-- before and  --> after
        view->send(SCI_INSERTTEXT, start, reinterpret_cast<sptr_t>(QStringLiteral("<!-- ").utf16()));
        // After inserting 5 chars, adjust end
        view->send(SCI_INSERTTEXT, end + 5, reinterpret_cast<sptr_t>(QStringLiteral(" -->").utf16()));
    } else {
        // Line comment: prepend to every selected line
        long start = view->send(SCI_GETSELECTIONSTART);
        long end = view->send(SCI_GETSELECTIONEND);
        long startLine = view->send(SCI_LINEFROMPOSITION, start);
        long endLine = view->send(SCI_LINEFROMPOSITION, end);
        // Ensure we also comment the line containing the end position if selection doesn't span EOL
        // Process lines from bottom to top to avoid offset shifting
        QByteArray prefixUtf8 = prefix.toUtf8();
        for (long line = endLine; line >= startLine; --line) {
            size_t lineStart = view->send(SCI_POSITIONFROMLINE, line);
            view->send(SCI_INSERTTEXT, lineStart, reinterpret_cast<sptr_t>(prefixUtf8.constData()));
        }
    }

    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editBlockUncomment()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    Buffer* buf = view->getCurrentBuffer();
    LangType lang = buf ? buf->getLangType() : L_TEXT;

    // Determine comment prefix
    QStringList prefixes;
    switch (lang) {
        case L_C: case L_CPP: case L_CS: case L_JAVA: case L_JAVASCRIPT:
        case L_PHP: case L_OBJC: case L_SWIFT: case L_GOLANG: case L_RUST:
        case L_TYPESCRIPT: case L_JSON: case L_YAML: case L_TOML:
        case L_CMAKE: case L_RC: case L_FLASH: case L_NSIS: case L_INNO:
        case L_D: case L_ESCRIPT: case L_GDSCRIPT: case L_COFFEESCRIPT:
            prefixes << "//";
            break;
        case L_PYTHON: case L_PERL: case L_RUBY: case L_BASH:
        case L_MAKEFILE: case L_PS: case L_LUA: case L_R: case L_POWERSHELL:
        case L_BATCH: case L_MARKDOWN: case L_TXT2TAGS:
        case L_TCL: case L_LISP: case L_SCHEME: case L_NIM:
        case L_FORTRAN: case L_FORTRAN_77: case L_ADA: case L_PROPS:
        case L_VERILOG: case L_VHDL: case L_FORTH: case L_CAML:
        case L_HASKELL: case L_ERLANG: case L_PASCAL: case L_SMALLTALK:
        case L_ASM: case L_KIX: case L_OSCRIPT: case L_REBOL:
            prefixes << "//" << "#" << ";";
            break;
        case L_SQL: case L_MSSQL:
            prefixes << "--";
            break;
        case L_HTML: case L_XML: case L_ASP: case L_JSP:
        case L_TEX: case L_LATEX:
            prefixes << "<!--" << "--";
            break;
        default:
            prefixes << "//" << "#" << "--";
            break;
    }

    view->execute(SCI_BEGINUNDOACTION);
    long start = view->send(SCI_GETSELECTIONSTART);
    long end = view->send(SCI_GETSELECTIONEND);
    long startLine = view->send(SCI_LINEFROMPOSITION, start);
    long endLine = view->send(SCI_LINEFROMPOSITION, end);

    // Process from top to bottom
    for (long line = startLine; line <= endLine; ++line) {
        size_t lineStart = view->send(SCI_POSITIONFROMLINE, line);
        // Get the first 10 chars of the line to check for comment prefix
        QByteArray lineBegin(11, '\0');
        for (int i = 0; i < 10; ++i) {
            int ch = view->send(SCI_GETCHARAT, lineStart + i);
            if (ch == 0 || ch == '\n' || ch == '\r')
                break;
            lineBegin[i] = static_cast<char>(ch);
        }
        QString lineBeginStr = QString::fromUtf8(lineBegin).trimmed();

        for (const QString& pfx : prefixes) {
            if (lineBeginStr.startsWith(pfx)) {
                // Remove the prefix
                size_t pfxEnd = lineStart + pfx.length();
                // Also remove any space after prefix
                int chAfter = view->send(SCI_GETCHARAT, pfxEnd);
                if (chAfter == ' ' || chAfter == '\t')
                    ++pfxEnd;
                view->send(SCI_SETTARGETSTART, lineStart);
                view->send(SCI_SETTARGETEND, pfxEnd);
                view->send(SCI_REPLACETARGET, 0, 0);
                break;
            }
        }
    }
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editStreamComment()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_BEGINUNDOACTION);
    long start = view->send(SCI_GETSELECTIONSTART);
    long end = view->send(SCI_GETSELECTIONEND);
    // Insert /* before and */ after selection
    view->send(SCI_INSERTTEXT, start, reinterpret_cast<sptr_t>(QStringLiteral("/* ").utf16()));
    // After inserting 3 chars, the end position shifts by 3
    view->send(SCI_INSERTTEXT, end + 3, reinterpret_cast<sptr_t>(QStringLiteral(" */").utf16()));
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::editStreamUncomment()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    view->execute(SCI_BEGINUNDOACTION);
    long start = view->send(SCI_GETSELECTIONSTART);
    long end = view->send(SCI_GETSELECTIONEND);

    // Look for /* ... */ around the selection
    // Check up to 3 chars before start for /*
    QString beforeText;
    for (int i = 0; i < 3; ++i) {
        int ch = view->send(SCI_GETCHARAT, start - i - 1);
        if (ch == 0)
            break;
        beforeText.prepend(QChar(static_cast<QChar::SpecialCharacter>(ch)));
    }

    // Check up to 3 chars after end for */
    QString afterText;
    for (int i = 0; i < 3; ++i) {
        int ch = view->send(SCI_GETCHARAT, end + i);
        if (ch == 0)
            break;
        afterText.append(QChar(static_cast<QChar::SpecialCharacter>(ch)));
    }

    bool changed = false;
    if (beforeText == "/*") {
        // Remove the /* before
        view->send(SCI_SETTARGETSTART, start - 2);
        view->send(SCI_SETTARGETEND, start);
        view->send(SCI_REPLACETARGET, 0, 0);
        changed = true;
    }
    if (afterText == "*/") {
        // Remove the */ after (position shifts if we removed /*)
        long newEnd = end + (changed ? -2 : 0);
        view->send(SCI_SETTARGETSTART, newEnd);
        view->send(SCI_SETTARGETEND, newEnd + 2);
        view->send(SCI_REPLACETARGET, 0, 0);
    }
    view->execute(SCI_ENDUNDOACTION);
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
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    if (view->isReadOnly())
        return;
    // Short date: "yyyy-MM-dd"
    QString dt = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd"));
    view->send(SCI_INSERTTEXT, view->send(SCI_GETCURRENTPOS),
               reinterpret_cast<sptr_t>(dt.utf16()));
}

void NppCommands::editInsertDateTimeLong()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    if (view->isReadOnly())
        return;
    // Long date: "dddd, MMMM d, yyyy  h:mm:ss AP"
    QString dt = QDateTime::currentDateTime().toString(QStringLiteral("dddd, MMMM d, yyyy  h:mm:ss AP"));
    view->send(SCI_INSERTTEXT, view->send(SCI_GETCURRENTPOS),
               reinterpret_cast<sptr_t>(dt.utf16()));
}

void NppCommands::editInsertDateTimeCustom()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    if (view->isReadOnly())
        return;
    // Custom format: ask user or use a common default
    // Try to get from NppParameters if available, otherwise use ISO format
    QString dt = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    view->send(SCI_INSERTTEXT, view->send(SCI_GETCURRENTPOS),
               reinterpret_cast<sptr_t>(dt.utf16()));
}

void NppCommands::editTrimTrailing() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp) npp->doTrim(lineTail); }

void NppCommands::editTrimLeading() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp) npp->doTrim(lineHeader); }

void NppCommands::editTrimBoth() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp){ npp->doTrim(lineHeader); npp->doTrim(lineTail); } }

void NppCommands::editEolToWs() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp) npp->eol2ws(); }

void NppCommands::editTrimAll() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp){ npp->doTrim(lineHeader); npp->doTrim(lineTail); } }

void NppCommands::editTabToSpace() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp) npp->wsTabConvert(tab2Space); }

void NppCommands::editSpaceToTabAll() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp) npp->wsTabConvert(space2TabAll); }

void NppCommands::editSpaceToTabLeading() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp) npp->wsTabConvert(space2TabLeading); }


// ===========================================================================
// SEARCH COMMANDS
// ===========================================================================

void NppCommands::searchFind()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->_findReplaceDlg.doDialog(FIND_DLG);
}

void NppCommands::searchReplace()
{
    if (!_mainWindow)
        return;
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp)
        npp->_findReplaceDlg.doDialog(REPLACE_DLG);
}

void NppCommands::searchFindNext()
{
    auto* view = activeView();
    auto* npp2 = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!view || !npp2) return;
    const FindOption& opts = npp2->_findReplaceDlg.getCurrentOptions();
    if (opts._str2Search.empty()) return;

    // Build flags: match case, whole word, regex (extended/normal handled via string processing)
    unsigned int flags = 0;
    if (opts._isMatchCase) flags |= npp_sci::SCFIND_MATCHCASE;
    if (opts._isWholeWord) flags |= npp_sci::SCFIND_WHOLEWORD;
    if (opts._searchType == FindRegex) {
        flags |= npp_sci::SCFIND_REGEXP;
    }
    view->send(SCI_SETSEARCHFLAGS, flags);

    // Convert wstring → QString → UTF-8 for Scintilla
    QString qstr = QString::fromStdWString(opts._str2Search);
    if (opts._searchType == FindExtended) {
        // Expand \n \r \t sequences
        qstr.replace(QLatin1String("\\n"), QLatin1String("\n"));
        qstr.replace(QLatin1String("\\r"), QLatin1String("\r"));
        qstr.replace(QLatin1String("\\t"), QLatin1String("\t"));
    }
    QByteArray utf8 = qstr.toUtf8();
    view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(utf8.constData()));
}

void NppCommands::searchFindPrev()
{
    auto* view = activeView();
    auto* npp2 = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!view || !npp2) return;
    const FindOption& opts = npp2->_findReplaceDlg.getCurrentOptions();
    if (opts._str2Search.empty()) return;

    // Build flags: match case, whole word, regex
    unsigned int flags = 0;
    if (opts._isMatchCase) flags |= npp_sci::SCFIND_MATCHCASE;
    if (opts._isWholeWord) flags |= npp_sci::SCFIND_WHOLEWORD;
    if (opts._searchType == FindRegex) {
        flags |= npp_sci::SCFIND_REGEXP;
    }
    view->send(SCI_SETSEARCHFLAGS, flags);

    // Convert wstring → QString → UTF-8 for Scintilla
    QString qstr = QString::fromStdWString(opts._str2Search);
    if (opts._searchType == FindExtended) {
        qstr.replace(QLatin1String("\\n"), QLatin1String("\n"));
        qstr.replace(QLatin1String("\\r"), QLatin1String("\r"));
        qstr.replace(QLatin1String("\\t"), QLatin1String("\t"));
    }
    QByteArray utf8 = qstr.toUtf8();
    view->send(2368, 0, reinterpret_cast<sptr_t>(utf8.constData()));  // SEARCHPREV
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

void NppCommands::searchGoToLine() { auto* view = activeView(); if(!view) return; bool ok=false; int line=QInputDialog::getInt(nullptr,"Go to Line","Line number:",1,1,view->send(2006),1,&ok); if(ok) view->send(2024,line-1); }

void NppCommands::searchGoToMatchingBrace() { auto* view = activeView(); if(!view) return; long cp=view->send(2008), mp=view->send(36,cp,0); if(mp!=-1) view->send(2043,mp,mp+1); }

void NppCommands::searchSelectMatchingBraces() { auto* view = activeView(); if(!view) return; long cp=view->send(2008), mp=view->send(36,cp,0); if(mp!=-1) view->send(2160,cp,mp+1); }

void NppCommands::searchMarkAllExt1() { auto* view = activeView(); if(!view) return; QString word = view->getWordOnCaret(); if(word.isEmpty()) return; view->send(SETSEARCHFLAGS, 0); QByteArray pat = word.toUtf8(); for(long pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData())); pos >= 0; pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData()))) { long line = view->send(LINEFROMPOSITION, pos); view->send(SCI_MARKERADD, line, (1<<0)); } }
void NppCommands::searchMarkAllExt2() { auto* view = activeView(); if(!view) return; QString word = view->getWordOnCaret(); if(word.isEmpty()) return; view->send(SETSEARCHFLAGS, 0); QByteArray pat = word.toUtf8(); for(long pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData())); pos >= 0; pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData()))) { long line = view->send(LINEFROMPOSITION, pos); view->send(SCI_MARKERADD, line, (1<<1)); } }
void NppCommands::searchMarkAllExt3() { auto* view = activeView(); if(!view) return; QString word = view->getWordOnCaret(); if(word.isEmpty()) return; view->send(SETSEARCHFLAGS, 0); QByteArray pat = word.toUtf8(); for(long pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData())); pos >= 0; pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData()))) { long line = view->send(LINEFROMPOSITION, pos); view->send(SCI_MARKERADD, line, (1<<2)); } }
void NppCommands::searchMarkAllExt4() { auto* view = activeView(); if(!view) return; QString word = view->getWordOnCaret(); if(word.isEmpty()) return; view->send(SETSEARCHFLAGS, 0); QByteArray pat = word.toUtf8(); for(long pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData())); pos >= 0; pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData()))) { long line = view->send(LINEFROMPOSITION, pos); view->send(SCI_MARKERADD, line, (1<<3)); } }
void NppCommands::searchMarkAllExt5() { auto* view = activeView(); if(!view) return; QString word = view->getWordOnCaret(); if(word.isEmpty()) return; view->send(SETSEARCHFLAGS, 0); QByteArray pat = word.toUtf8(); for(long pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData())); pos >= 0; pos = view->send(SEARCHNEXT, 0, reinterpret_cast<sptr_t>(pat.constData()))) { long line = view->send(LINEFROMPOSITION, pos); view->send(SCI_MARKERADD, line, (1<<4)); } }

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

void NppCommands::searchToggleBookmark() { auto* view = activeView(); if(!view) return; long line=view->send(2013); long mask=view->send(2041,line); if(mask&1) view->send(2048,line,0); else view->send(2049,line,1); }

void NppCommands::searchNextBookmark() { auto* view = activeView(); if(!view) return; long line=view->send(2013); long nline=view->send(2051,line+1); if(nline>=0) view->send(2024,nline); }

void NppCommands::searchPrevBookmark() { auto* view = activeView(); if(!view) return; long line=view->send(2013); long nline=view->send(2052,line-1); if(nline>=0) view->send(2024,nline); }

void NppCommands::searchClearBookmarks() { auto* view = activeView(); if(!view) return; long lines=view->send(2374); for(long i=0;i<lines;i++) view->send(2048,i,0); }

void NppCommands::searchCutMarkedLines() { auto* view = activeView(); if(!view) return; long lines=view->send(2374); for(long i=lines-1;i>=0;i--) if(view->send(2041,i)&1) view->send(2168,0,i+1); }

void NppCommands::searchCopyMarkedLines() { auto* view = activeView(); if(!view) return; QString text; long lines=view->send(2374); for(long i=0;i<lines;i++) if(view->send(2041,i)&1){ text+=view->getLine(i); text+='\n'; } if(!text.isEmpty()) QApplication::clipboard()->setText(text); }

void NppCommands::searchPasteMarkedLines() { auto* view = activeView(); if(!view) return; QString txt=QApplication::clipboard()->text(); if(!txt.isEmpty()) view->send(2178,0,reinterpret_cast<sptr_t>(txt.toUtf8().constData())); }

void NppCommands::searchDeleteMarkedLines() { auto* view = activeView(); if(!view) return; long lines=view->send(2374); for(long i=lines-1;i>=0;i--) if(view->send(2041,i)&1) view->send(2168,0,i+1); }

void NppCommands::searchDeleteUnmarkedLines() { auto* view = activeView(); if(!view) return; long lines=view->send(2374); for(long i=lines-1;i>=0;i--) if(!(view->send(2041,i)&1)) view->send(2168,0,i+1); }

void NppCommands::searchInverseMarks() { auto* view = activeView(); if(!view) return; long lines=view->send(2374); for(long i=0;i<lines;i++){ long m=view->send(2041,i); view->send(2049,i,m?0:1); } }

void NppCommands::searchFindCharInRange() { auto* view = activeView(); if(!view) return; bool ok=false; QString ch=QInputDialog::getText(nullptr,"Find Character","Enter character to find:",QLineEdit::Normal,QString(),&ok); if(ok&&!ch.isEmpty()){ long cp=view->send(2008); view->send(2198,0); view->send(2169,0,reinterpret_cast<sptr_t>(ch.utf16())); } }


// ===========================================================================
// VIEW COMMANDS
// ===========================================================================

void NppCommands::viewAlwaysOnTop() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    Qt::WindowFlags flags = npp->windowFlags();
    if (flags & Qt::WindowStaysOnTopHint) {
        npp->setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
    } else {
        npp->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    }
    npp->show();
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

void NppCommands::viewTabSpace() { auto* view=activeView(); if(view) view->showInvisibleChars(!view->isShownInvisibleChars()); }

void NppCommands::viewEol() { auto* view=activeView(); if(view) view->showEOL(!view->isShownEOL()); }

void NppCommands::viewAllCharacters() { auto* view=activeView(); if(view) view->showInvisibleChars(!view->isShownInvisibleChars()); }

void NppCommands::viewNpc() { auto* view=activeView(); if(view) view->showNpc(true); }

void NppCommands::viewNpcCcuniEol() { auto* view=activeView(); if(view) view->showCcUniEol(true); }

void NppCommands::viewIndentGuide() { auto* view=activeView(); if(view) view->showIndentGuide(!view->isShownIndentGuide()); }

void NppCommands::viewWrapSymbol()
{
    auto* view = activeView();
    if (!view)
        return;
    // Toggle wrap symbol (↪ indicator on wrapped lines)
    view->showWrapSymbol(!view->isWrapSymbolVisible());
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

void NppCommands::viewGotoStart() { auto* view=activeView(); if(view) view->send(2456,0); }

void NppCommands::viewGotoEnd() { auto* view=activeView(); if(view) view->send(2458,0); }

void NppCommands::viewSwitchToOtherView() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp) npp->switchEditViewTo(npp->otherView());
}

void NppCommands::viewCloneToAnotherView() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp) npp->docGotoAnotherEditView(TransferClone);
}
void NppCommands::viewGotoNewInstance() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp) npp->docOpenInNewInstance(TransferMove, 0, 0);
}

void NppCommands::viewLoadInNewInstance() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (npp) npp->docOpenInNewInstance(TransferClone, 0, 0);
}
void NppCommands::viewWrap() { auto* view=activeView(); if(view) view->setWrapMode(view->isWrap() ? SCWRAP_NONE : SCWRAP_WORD); }
void NppCommands::viewHideLines() {
    auto* view = activeView();
    if (!view) return;
    intptr_t line = view->send(SCI_LINEFROMPOSITION, view->send(SCI_GETCURRENTPOS));
    bool isVisible = view->send(SCI_GETLINEVISIBLE, line) != 0;
    if (isVisible) {
        view->send(SCI_HIDELINES, line, line);
    } else {
        view->send(SCI_SHOWLINES, line, line);
    }
}
void NppCommands::viewFullScreenToggle() { auto* npp = qobject_cast<Notepad_plus*>(_mainWindow); if(npp) npp->fullScreenToggle(); }

void NppCommands::viewSummary() {
    auto* view = activeView();
    if (!view) return;
    intptr_t length = view->send(SCI_GETLENGTH);
    intptr_t lineCount = view->send(SCI_GETLINECOUNT);
    intptr_t selStart = view->send(SCI_GETSELECTIONSTART);
    intptr_t selEnd = view->send(SCI_GETSELECTIONEND);
    intptr_t selLength = selEnd - selStart;
    intptr_t curLine = view->send(SCI_LINEFROMPOSITION, selStart) + 1;
    intptr_t lineStart = view->send(SCI_POSITIONFROMLINE, curLine - 1);
    intptr_t lineEnd = view->send(SCI_GETLINEENDPOSITION, curLine - 1);
    intptr_t curCol = selStart - lineStart + 1;
    intptr_t nonBlank = 0;
    for (intptr_t i = 0; i < lineCount; ++i) {
        intptr_t ls = view->send(SCI_POSITIONFROMLINE, i);
        intptr_t le = view->send(SCI_GETLINEENDPOSITION, i);
        if (le > ls) ++nonBlank;
    }
    QString info = QStringLiteral(
        "Document Summary\n"
        "----------------\n"
        "Total characters: %1\n"
        "Total lines: %2\n"
        "Non-blank lines: %3\n"
        "Selection length: %4 chars\n"
        "Cursor position: Line %5, Column %6"
    ).arg(length).arg(lineCount).arg(nonBlank).arg(selLength).arg(curLine).arg(curCol);
    QMessageBox::information(nullptr, QStringLiteral("Document Summary"), info);
}


// ===========================================================================
// ENCODING COMMANDS
// ===========================================================================

void NppCommands::encodingAnsi()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    Buffer* buf = view->getCurrentBuffer();
    if (buf) {
        buf->setUnicodeMode(UniMode::uni8Bit);
        view->execute(SCI_SETCODEPAGE, 0);
    }
}

void NppCommands::encodingUTF8()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    Buffer* buf = view->getCurrentBuffer();
    if (buf) {
        buf->setUnicodeMode(UniMode::uniUTF8_NoBOM);
        view->execute(SCI_SETCODEPAGE, SC_CP_UTF8);
    }
}

void NppCommands::encodingUTF8BOM()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    Buffer* buf = view->getCurrentBuffer();
    if (buf) {
        buf->setUnicodeMode(UniMode::uniCookie);
        view->execute(SCI_SETCODEPAGE, SC_CP_UTF8);
    }
}

void NppCommands::encodingUTF16BE()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    Buffer* buf = view->getCurrentBuffer();
    if (buf) {
        buf->setUnicodeMode(UniMode::uni16BE);
        view->execute(SCI_SETCODEPAGE, SC_CP_UTF8);
    }
}

void NppCommands::encodingUTF16LE()
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;
    Buffer* buf = view->getCurrentBuffer();
    if (buf) {
        buf->setUnicodeMode(UniMode::uni16LE);
        view->execute(SCI_SETCODEPAGE, SC_CP_UTF8);
    }
}

// ---------------------------------------------------------------------------
// Encoding conversion helper
// Copies all text from the document, changes the buffer's unicode mode,
// then re-inserts the text so Scintilla re-interprets the UTF-8 bytes through
// the new code page. This mirrors the Win32 copy/clear/convert/paste cycle.
// ---------------------------------------------------------------------------
void NppCommands::convertDocEncoding(UniMode newMode)
{
    ScintillaEditView* view = activeView();
    if (!view)
        return;

    Buffer* buf = view->getCurrentBuffer();
    if (!buf)
        return;

    // Warn about losing undo history (matches Win32 behavior)
    if (view->execute(SCI_CANUNDO)) {
        QMessageBox::StandardButton btn = QMessageBox::warning(
            _mainWindow,
            tr("Encoding Conversion"),
            tr("Converting the encoding will clear the undo history.\nAll changes will become non-undoable.\n\nContinue?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );
        if (btn != QMessageBox::Yes)
            return;
    }

    // Save current cursor position so we can restore it after the conversion
    view->execute(SCI_BEGINUNDOACTION);
    intptr_t currentPos = view->execute(SCI_GETCURRENTPOS);
    intptr_t anchorPos = view->execute(SCI_GETANCHOR);
    intptr_t firstVisibleLine = view->execute(SCI_GETFIRSTVISIBLELINE);

    // Get all text as UTF-8
    intptr_t docLen = view->execute(SCI_GETLENGTH);
    QByteArray textUtf8(docLen + 1, '\0');
    view->execute(SCI_GETTEXT, docLen + 1, reinterpret_cast<sptr_t>(textUtf8.data()));
    textUtf8.truncate(docLen);

    // Clear the document (but undo history is already warned about)
    view->execute(SCI_CLEARALL);

    // Change the buffer's unicode mode
    buf->setUnicodeMode(newMode);

    // Re-insert the text — Scintilla will encode it according to the new mode
    view->execute(SCI_ADDTEXT, textUtf8.size(), reinterpret_cast<sptr_t>(textUtf8.constData()));

    // Restore cursor position (clamp to valid range)
    intptr_t newLen = view->execute(SCI_GETLENGTH);
    if (currentPos > newLen)
        currentPos = newLen;
    if (anchorPos > newLen)
        anchorPos = newLen;

    view->execute(SCI_SETANCHOR, anchorPos);
    view->execute(SCI_GOTOPOS, currentPos);
    view->execute(SCI_SETFIRSTVISIBLELINE, firstVisibleLine);

    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::encodingConvertAnsi()
{
    convertDocEncoding(UniMode::uni8Bit);
}

void NppCommands::encodingConvertUTF8()
{
    convertDocEncoding(UniMode::uniUTF8_NoBOM);
}

void NppCommands::encodingConvertUTF8BOM()
{
    convertDocEncoding(UniMode::uniCookie);
}

void NppCommands::encodingConvertUTF16BE()
{
    convertDocEncoding(UniMode::uni16BE);
}

void NppCommands::encodingConvertUTF16LE()
{
    convertDocEncoding(UniMode::uni16LE);
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
    // Show the User Defined Language dialog
    // The dialog loads the UDL list from userDefineLangs/ directory
    UserDefineDialog* udd = UserDefineDialog::getInstance();
    udd->reloadLangList();
    udd->doDialog(false);
}

void NppCommands::languageOpenUdldDir()
{
    QString udlPath = QString::fromStdWString(NppParameters::getInstance().getUserDefineLangFolderPath());
    QDir dir(udlPath);
    if (!dir.exists())
        dir.mkpath(udlPath);
    QDesktopServices::openUrl(QUrl::fromLocalFile(udlPath));
}


// ===========================================================================
// SETTINGS COMMANDS
// ===========================================================================

void NppCommands::settingsPreference()
{
    PreferenceDialog dlg(qobject_cast<QWidget*>(_mainWindow));
    dlg.exec();
}

void NppCommands::settingsStyleConfig()
{
    StyleConfigDialog dlg(qobject_cast<QWidget*>(_mainWindow));
    dlg.exec();
}

void NppCommands::settingsShortcutMapper()
{
    ShortcutMapperDialog dlg(qobject_cast<QWidget*>(_mainWindow));
    dlg.exec();
}

void NppCommands::settingsImportPlugin()
{
    // Ask user to select plugin file(s)
    QStringList files = QFileDialog::getOpenFileNames(
        nullptr,
        tr("Import Plugin"),
        QDir::homePath(),
        tr("Plugin files (*.dll *.so);;All files (*)")
    );

    if (files.isEmpty())
        return;

    // Determine target plugins directory
    QString appDir = QCoreApplication::applicationDirPath();
    QString pluginsDir = appDir + QStringLiteral("/plugins");

    // Create plugins dir if it doesn't exist
    QDir().mkpath(pluginsDir);

    int imported = 0;
    for (const QString& srcPath : files) {
        QString fileName = QFileInfo(srcPath).fileName();
        QString destPath = pluginsDir + QStringLiteral("/") + fileName;

        if (QFile::copy(srcPath, destPath)) {
            imported++;
        } else {
            QMessageBox::warning(nullptr, tr("Import Plugin"),
                tr("Failed to copy %1 to plugins directory.").arg(fileName));
        }
    }

    if (imported > 0) {
        QMessageBox::information(nullptr, tr("Import Plugin"),
            tr("Successfully imported %n plugin(s). Restart Notepad++ to load them.", "", imported));
    }
}

void NppCommands::settingsImportStyleThemes()
{
    // Ask user to select theme XML file(s)
    QStringList files = QFileDialog::getOpenFileNames(
        nullptr,
        tr("Import Style Theme"),
        QDir::homePath(),
        tr("Style theme files (*.xml);;All files (*)")
    );

    if (files.isEmpty())
        return;

    // Determine target themes directory
    QString appDir = QCoreApplication::applicationDirPath();
    QString themesDir = appDir + QStringLiteral("/themes");

    // Create themes dir if it doesn't exist
    QDir().mkpath(themesDir);

    int imported = 0;
    for (const QString& srcPath : files) {
        QString fileName = QFileInfo(srcPath).fileName();
        QString destPath = themesDir + QStringLiteral("/") + fileName;

        if (QFile::copy(srcPath, destPath)) {
            imported++;
        } else {
            QMessageBox::warning(nullptr, tr("Import Style Theme"),
                tr("Failed to copy %1 to themes directory.").arg(fileName));
        }
    }

    if (imported > 0) {
        QMessageBox::information(nullptr, tr("Import Style Theme"),
            tr("Successfully imported %n theme(s). They will appear in the Style Configurator.", "", imported));
    }
}

void NppCommands::settingsEditContextMenu()
{
    // Context menu editing requires modifying contextMenu.xml
    // For now, open the config directory so users can edit it manually
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString contextMenuPath = configDir + QStringLiteral("/contextMenu.xml");

    // Create a default contextMenu.xml if it doesn't exist
    if (!QFile::exists(contextMenuPath)) {
        QFile file(contextMenuPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "<!-- Notepad++ Context Menu Configuration -->\n";
            out << "<!-- Edit this file to customize the right-click context menu -->\n";
            out << "<NotepadPlus>\n";
            out << "  <ContextMenu>\n";
            out << "    <!-- Example: <Item MenuId=\"...\" MenuName=\"...\" ShaderName=\"...\"/> -->\n";
            out << "  </ContextMenu>\n";
            out << "</NotepadPlus>\n";
            file.close();
        }
    }

    // Open the contextMenu.xml in the editor
    if (_mainWindow) {
        auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
        if (npp) {
            npp->fileOpen(contextMenuPath);
        }
    }

    QMessageBox::information(nullptr, tr("Edit Context Menu"),
        tr("The contextMenu.xml file has been opened for editing.\n"
           "Save the file and restart Notepad++ to apply changes."));
}


// ===========================================================================
// MACRO COMMANDS
// ===========================================================================

void NppCommands::macroStartRecording()
{
    if (!_mainWindow)
        return;
    startMacroRecording();
}

void NppCommands::macroStopRecording()
{
    if (!_mainWindow)
        return;
    stopMacroRecording();
}

// Toggle recording: starts if not recording, stops if recording
void NppCommands::macroStartStopRecord()
{
    if (_isMacroRecording)
        stopMacroRecording();
    else
        startMacroRecording();
}

void NppCommands::macroPlayback()
{
    if (_lastMacroSteps.isEmpty()) {
        QMessageBox::information(nullptr, "Macro", "No macro recorded yet. Record a macro first using the Start Recording button or menu.");
        return;
    }
    playMacro(_lastMacroSteps);
}

void NppCommands::macroSaveCurrent()
{
    if (_lastMacroSteps.isEmpty()) {
        QMessageBox::information(nullptr, "Macro", "No macro recorded yet to save.");
        return;
    }

    bool ok = false;
    QString name = QInputDialog::getText(nullptr, "Save Macro", "Enter a name for this macro:",
                                          QLineEdit::Normal, _currentMacroName, &ok);
    if (!ok || name.isEmpty())
        return;

    _currentMacroName = name;

    // Create a MacroShortcut and add it to the macro list in NppParameters
    Shortcut sc;
    sc.setName(name);
    MacroShortcut* macroShortcut = new MacroShortcut(sc, _lastMacroSteps, 0);
    NppParameters::getInstance().getMacroList().push_back(macroShortcut);

    QMessageBox::information(nullptr, "Macro", QString("Macro \"%1\" saved successfully.").arg(name));
}

void NppCommands::macroRunMultiple()
{
    // Show the Run Macro dialog — MainWindow holds the dialog instance
    _mainWindow->showRunMacroDialog();
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void NppCommands::startMacroRecording()
{
    _isMacroRecording = true;
    _currentMacroSteps.clear();
    _currentMacroName.clear();
    emit macroRecordingStateChanged(true);
}

void NppCommands::stopMacroRecording()
{
    _isMacroRecording = false;
    _lastMacroSteps = _currentMacroSteps;
    _currentMacroSteps.clear();
    emit macroRecordingStateChanged(false);
}

void NppCommands::recordMacroStep(int message, uintptr_t wParam, uintptr_t lParam)
{
    if (!_isMacroRecording)
        return;
    RecordedMacroStep step(message, wParam, lParam);
    _currentMacroSteps.append(step);
}

void NppCommands::recordMacroStepMenu(int commandId)
{
    if (!_isMacroRecording)
        return;
    RecordedMacroStep step(commandId);
    _currentMacroSteps.append(step);
}

void NppCommands::playMacro(const Macro& steps)
{
    auto* view = activeView();
    if (!view)
        return;

    view->execute(SCI_BEGINUNDOACTION);
    for (const RecordedMacroStep& step : steps) {
        RecordedMacroStep ncStep = step;  // copy to non-const
        ncStep.playBack(view);  // handles all types: scintilla messages + menu commands
    }
    view->execute(SCI_ENDUNDOACTION);
}

void NppCommands::runMacroFromDialog(int macroIndex, int times)
{
    const auto& macros = NppParameters::getInstance().getMacroList();
    if (macroIndex < 0 || static_cast<size_t>(macroIndex) >= macros.size()) {
        QMessageBox::warning(nullptr, "Macro",
            QStringLiteral("Invalid macro index: %1").arg(macroIndex));
        return;
    }

    const auto& macro = macros[static_cast<size_t>(macroIndex)];
    if (!macro) {
        QMessageBox::warning(nullptr, "Macro", QStringLiteral("Macro at index %1 is null.").arg(macroIndex));
        return;
    }

    for (int i = 0; i < times; ++i) {
        playMacro(macro->getMacro());
    }
}

void NppCommands::macroRunMultipleMacrosDialog()
{
    const auto& macros = NppParameters::getInstance().getMacroList();
    if (macros.empty()) {
        QMessageBox::information(nullptr, "Run Multiple Macros", "No saved macros found. Record and save a macro first.");
        return;
    }

    // Build a simple list of macro names for selection
    QStringList names;
    for (const auto& m : macros)
        names << m->getName();

    bool ok = false;
    QString selected = QInputDialog::getItem(nullptr, "Run Multiple Macros",
                                             "Select a macro to run:", names, 0, false, &ok);
    if (!ok || selected.isEmpty())
        return;

    // Find and play the selected macro
    for (const auto& m : macros) {
        if (m->getName() == selected) {
            playMacro(m->getMacro());
            break;
        }
    }
}


// ===========================================================================
// RUN COMMANDS
// ===========================================================================

void NppCommands::runExecute() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->_runDlg.show();
    npp->_runDlg.raise();
    npp->_runDlg.activateWindow();
}

void NppCommands::runCmdLineArgs()
{
    if (_mainWindow) {
        auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
        if (npp) {
            npp->getCmdLineArgsDlg().doDialog();
        }
    }
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
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    // Sort the document tabs alphabetically (ascending)
    auto* docTab = npp->_pDocTab;
    if (!docTab) return;
    int count = docTab->count();
    if (count < 2) return;
    QStringList names;
    for (int i = 0; i < count; ++i) {
        names << docTab->tabText(i);
    }
    names.sort(Qt::CaseInsensitive);
    // Reorder tabs by swapping
    QMap<QString, int> nameToIdx;
    for (int i = 0; i < count; ++i) {
        nameToIdx[docTab->tabText(i).toLower()] = i;
    }
    for (int newPos = 0; newPos < count; ++newPos) {
        QString targetName = names[newPos].toLower();
        int oldPos = nameToIdx[targetName];
        if (oldPos != newPos) {
            docTab->tabBar()->moveTab(oldPos, newPos);
            // Update map after move
            for (auto& k : nameToIdx.keys()) {
                if (nameToIdx[k] > oldPos && nameToIdx[k] <= newPos) nameToIdx[k]--;
                else if (nameToIdx[k] == oldPos) nameToIdx[k] = newPos;
            }
        }
    }
}

void NppCommands::windowSortDescending()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    // Sort the document tabs reverse-alphabetically (descending)
    auto* docTab = npp->_pDocTab;
    if (!docTab) return;
    int count = docTab->count();
    if (count < 2) return;
    QStringList names;
    for (int i = 0; i < count; ++i) {
        names << docTab->tabText(i);
    }
    names.sort(Qt::CaseInsensitive);
    std::reverse(names.begin(), names.end());
    QMap<QString, int> nameToIdx;
    for (int i = 0; i < count; ++i) {
        nameToIdx[docTab->tabText(i).toLower()] = i;
    }
    for (int newPos = 0; newPos < count; ++newPos) {
        QString targetName = names[newPos].toLower();
        int oldPos = nameToIdx[targetName];
        if (oldPos != newPos) {
            docTab->tabBar()->moveTab(oldPos, newPos);
            for (auto& k : nameToIdx.keys()) {
                if (nameToIdx[k] > oldPos && nameToIdx[k] <= newPos) nameToIdx[k]--;
                else if (nameToIdx[k] == oldPos) nameToIdx[k] = newPos;
            }
        }
    }
}


// ===========================================================================
// HELP COMMANDS
// ===========================================================================

void NppCommands::helpAbout()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->_aboutDlg.doDialog();
}

void NppCommands::helpDocumentation()
{
    QDesktopServices::openUrl(QUrl("https://npp-user-manual.org/"));
}

void NppCommands::helpHomeSweetHome()
{
    QDesktopServices::openUrl(QUrl("https://notepad-plus-plus.org/"));
}

void NppCommands::helpOnlineDoc()
{
    QDesktopServices::openUrl(QUrl("https://npp-user-manual.org/docs/"));
}


// ===========================================================================
// COLUMN EDITOR COMMANDS
// ===========================================================================

void NppCommands::columnEditor()
{
    if (!_mainWindow)
        return;

    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return;

    npp->getColumnEditorDlg().doDialog(false);
}


// ===========================================================================
// CLIPBOARD COMMANDS
// ===========================================================================

void NppCommands::clipboardHistoryPanel()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->launchClipboardHistoryPanel();
}

void NppCommands::charPanel()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->launchAnsiCharPanel();
}


// ===========================================================================
// PANEL COMMANDS
// ===========================================================================

void NppCommands::toggleDocumentMap() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    auto* docMap = npp->getDocMap();
    if (!docMap) return;
    docMap->display(docMap->isClosed());
}

void NppCommands::toggleFunctionList() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    auto* funcList = npp->getFuncList();
    if (!funcList) return;
    funcList->display(funcList->isClosed());
}

void NppCommands::toggleFileBrowser() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    auto* fb = npp->getFileBrowser();
    if (!fb) return;
    fb->display(fb->isClosed());
}

void NppCommands::toggleProjectPanel1()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->launchProjectPanel(IDM_VIEW_PROJECT_PANEL_1, &npp->_pProjectPanel_1, 0);
}
void NppCommands::toggleProjectPanel2()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->launchProjectPanel(IDM_VIEW_PROJECT_PANEL_2, &npp->_pProjectPanel_2, 1);
}
void NppCommands::toggleProjectPanel3()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->launchProjectPanel(IDM_VIEW_PROJECT_PANEL_3, &npp->_pProjectPanel_3, 2);
}

void NppCommands::toggleClipboardHistory()
{
    clipboardHistoryPanel();
}

void NppCommands::toggleDocList() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    auto* docList = npp->getDocList();
    if (!docList) return;
    docList->display(docList->isClosed());
}


// ===========================================================================
// FIND/REPLACE PANEL COMMANDS
// ===========================================================================

void NppCommands::findInFiles()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->getFindReplaceDlg().doDialog(FINDINFILES_DLG);
}

void NppCommands::findInProjects()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return;
    npp->findInProjects();
}

void NppCommands::findInFilelist()
{
    // Reuse the Find-in-Files tab but search in currently open buffers
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->getFindReplaceDlg().showFifTab();
}

void NppCommands::replaceInFiles()
{
    // Show the Replace-in-Files tab
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->getFindReplaceDlg().showFifTab();
}

void NppCommands::replaceInProjects()
{
    // Replace in Projects: delegate to the Find/Replace panel in replace mode.
    // findInProjects() shows the panel for the project file list.
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp)
        return;
    npp->getFindReplaceDlg().doDialog(REPLACEINFILES_DLG);
}

void NppCommands::replaceInFilelist()
{
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    npp->getFindReplaceDlg().showFifTab();
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

void NppCommands::toggleReadOnly() { auto* view = activeView(); if(view) view->send(SCI_SETREADONLY, !view->send(SCI_GETREADONLY)); }

void NppCommands::setReadOnlyForAll() { auto* view = activeView(); if(view) view->send(SCI_SETREADONLY, 1); }

void NppCommands::clearReadOnlyForAll() { auto* view = activeView(); if(view) view->send(SCI_SETREADONLY, 0); }

void NppCommands::toggleRecording()
{
    macroStartStopRecord();
}

void NppCommands::toggleMacroPlayback()
{
    macroPlayback();
}

void NppCommands::toggleMonitoring() {
    auto* npp = qobject_cast<Notepad_plus*>(_mainWindow);
    if (!npp) return;
    Buffer* buf = npp->getCurrentBuffer();
    if (!buf) return;
    if (buf->isMonitoringOn()) {
        buf->stopMonitoring();
        QMessageBox::information(_mainWindow, QStringLiteral("File Monitoring"),
            QStringLiteral("File monitoring has been stopped."));
    } else {
        buf->startMonitoring();
        QMessageBox::information(_mainWindow, QStringLiteral("File Monitoring"),
            QStringLiteral("File monitoring is active. The document will be reloaded if it changes externally."));
    }
}
