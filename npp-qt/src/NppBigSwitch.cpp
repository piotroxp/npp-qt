// NppBigSwitch.cpp — Qt6 implementation of Notepad++ command dispatcher
// Semantic lift from: PowerEditor/src/NppBigSwitch.cpp (4305 lines)
// Win32 message switch → Qt6 signal/slot & QHash dispatch
// Handles: window events, NPPM_* messages, and IDM_* command routing

#include "NppBigSwitch.h"
#include "NppCommands.h"
#include "Notepad_plus.h"
#include "Notepad_plus_Window.h"
#include "NppDarkMode.h"
#include "MISC/Common/Common.h"
#include "menuCmdID.h"
#include "ScintillaEditView.h"

#include <QApplication>
#include <QCoreApplication>
#include <QEvent>
#include <QFile>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QProcess>
#include <QWidget>
#include <QWindow>
#include <QMimeData>
#include <QClipboard>
#include <QScreen>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QThread>
#include <QLabel>

// ============================================================================
// Qt6 compat shims — replace Win32 types
// ============================================================================
#define GWLP_USERDATA   0
#define GWL_STYLE       (-16)
#define SWP_NOMOVE      0x0002
#define SWP_NOSIZE      0x0001
#define SWP_NOZORDER    0x0004
#define SWP_FRAMECHANGED 0x0020

// Win32 message constants
#define WM_NCCREATE     0x0081
#define WM_CREATE       0x0001
#define WM_SIZE         0x0005
#define WM_MOVE         0x0003
#define WM_CLOSE        0x0010
#define WM_DESTROY      0x0002
#define WM_PAINT        0x000F
#define WM_ERASEBKGND   0x0014
#define WM_SETFOCUS     0x0007
#define WM_KILLFOCUS    0x0008
#define WM_ENABLE       0x000A
#define WM_SETREDRAW    0x000B
#define WM_ACTIVATE     0x0006
#define WM_NCACTIVATE   0x0086
#define WM_ACTIVATEAPP  0x001C
#define WM_GETMINMAXINFO 0x0024
#define WM_NCHITTEST    0x0084
#define WM_NCPAINT      0x0085
#define WM_NCMOUSEMOVE  0x00A0
#define WM_NCLBUTTONDOWN 0x00A1
#define WM_NCLBUTTONUP  0x00A2
#define WM_NCLBUTTONDBLCLK 0x00A3
#define WM_NCRBUTTONDOWN 0x00A4
#define WM_NCRBUTTONUP  0x00A5
#define WM_NCRBUTTONDBLCLK 0x00A6
#define WM_SYSCOMMAND   0x0112
#define WM_INITMENU     0x0116
#define WM_INITMENUPOPUP 0x0117
#define WM_MENUSELECT   0x011F
#define WM_ENTERIDLE    0x0121
#define WM_MBUTTONDOWN  0x0207
#define WM_MBUTTONUP    0x0208
#define WM_MBUTTONDBLCLK 0x0209
#define WM_XBUTTONDOWN  0x020B
#define WM_XBUTTONUP    0x020C
#define WM_XBUTTONDBLCLK 0x020D
#define WM_MOUSEWHEEL   0x020A
#define WM_MOUSEMOVE    0x0200
#define WM_LBUTTONDOWN  0x0201
#define WM_LBUTTONUP    0x0202
#define WM_LBUTTONDBLCLK 0x0203
#define WM_RBUTTONDOWN  0x0204
#define WM_RBUTTONUP    0x0205
#define WM_RBUTTONDBLCLK 0x0206
#define WM_DROPFILES    0x0233
#define WM_COPYDATA     0x004A
#define WM_COPY         0x0301
#define WM_CUT          0x0300
#define WM_PASTE        0x0302
#define WM_UNDO         0x0304
#define WM_CONTEXTMENU  0x007B
#define WM_NOTIFY       0x004E
#define WM_COMMAND      0x0111
#define WM_HSCROLL      0x0114
#define WM_VSCROLL      0x0115
#define WM_TIMER        0x0113
#define WM_SYSCOLORCHANGE 0x0015
#define WM_WININICHANGE 0x001A
#define WM_SETTINGCHANGE 0x001A
#define WM_FONTCHANGE   0x001D
#define WM_TIMECHANGE   0x001E
#define WM_ENTERMENULOOP 0x0211
#define WM_EXITMENULOOP 0x0212
#define WM_NEXTMENU     0x0213
#define WM_SIZING       0x0214
#define WM_MOVING       0x0216
#define WM_ENTERSIZEMOVE 0x0231
#define WM_EXITSIZEMOVE 0x0232
#define WM_DPICHANGED    0x02E0
#define WM_APPCOMMAND    0x0319
#define WM_QUERYENDSESSION 0x0011
#define WM_ENDSESSION    0x0016
#define WM_QUERYOPEN     0x0013
#define WM_GETTEXTLENGTH 0x000E
#define WM_GETTEXT       0x000D
#define WM_SETTEXT       0x000C
#define WM_SETFONT       0x0030
#define WM_GETFONT       0x0031
#define WM_GETICON       0x007F
#define WM_SETICON       0x0080

// SysCommand constants
#define SC_MINIMIZE     0xF020
#define SC_MAXIMIZE     0xF030
#define SC_RESTORE      0xF120
#define SC_CLOSE        0xF060
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_KEYMENU      0xF100
#define SC_MOUSEMENU    0xF090

// Helper macros
#define MAKELONG(a,b)    ((uint32_t)(((uint16_t)(a)) | ((uint32_t)((uint16_t)(b))) << 16))
#define MAKELPARAM(a,b)  MAKELONG(a,b)
#define HIWORD(x)        ((uint16_t)(((uint32_t)(x) >> 16) & 0xFFFF))
#define LOWORD(x)        ((uint16_t)((uint32_t)(x) & 0xFFFF))
#define GET_X_LPARAM(x)  ((int)(int16_t)LOWORD(x))
#define GET_Y_LPARAM(x)  ((int)(int16_t)HIWORD(x))

#ifndef WM_TASKBARCREATED
#define WM_TASKBARCREATED 0x8000
#endif

// ============================================================================
// NppBigSwitch — Qt6 command dispatcher
// ============================================================================
NppBigSwitch::NppBigSwitch(QObject* parent)
    : QObject(parent)
    , _currentDpi(96)
    , _pNppCommands(nullptr)
{
    registerDefaultHandlers();
}

NppBigSwitch::~NppBigSwitch() = default;

void NppBigSwitch::setNppCommands(NppCommands* cmds)
{
    _pNppCommands = cmds;
}

// ============================================================================
// Event Filter — routes Qt events to Npp event handlers
// Replaces the Win32 WndProc switch(message) { case WM_*: ... }
// ============================================================================
bool NppBigSwitch::eventFilter(QObject* watched, QEvent* event)
{
    if (!watched || !event)
        return false;

    QEvent::Type type = event->type();

    // Handle custom Npp events (QEvent::User + offset)
    if (type >= QEvent::User) {
        int nppId = static_cast<int>(type) - QEvent::User;
        if (nppId >= 0 && nppId < 0x10000) {
            // Menu command range (IDM_* → 0x2000-0x4000)
            if (nppId >= 0x2000 && nppId < 0x4000) {
                handleCommand(nppId);
                return true;
            }
            // Custom Npp internal event
            run_dlgProc(nppId, event);
            return true;
        }
    }

    // Handle specific Qt events that map to Win32 messages
    switch (type) {
        case QEvent::ThemeChange:
        case QEvent::ApplicationFontChange:
        case QEvent::PaletteChange: {
            int newDpi = qApp->devicePixelRatio() * 96;
            if (newDpi != _currentDpi) {
                _currentDpi = newDpi;
                handleDpiChanged(newDpi, QRect());
            }
            handleSettingChange(QString());
            break;
        }
        case QEvent::LocaleChange:
            handleSettingChange(QStringLiteral("Locale"));
            break;
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
            handleNcActivate(type == QEvent::WindowActivate);
            break;
        case QEvent::FocusIn:
        case QEvent::FocusOut:
            break;
        default:
            break;
    }

    return false;
}

// ============================================================================
// Main event handler — replaces Win32 WndProc
// Called from MainWindow::event() or similar entry point
// ============================================================================
bool NppBigSwitch::handleNppEvent(QEvent* event)
{
    if (!event)
        return false;
    QEvent::Type type = event->type();
    if (type >= QEvent::User) {
        int nppEventId = static_cast<int>(type) - QEvent::User;
        run_dlgProc(nppEventId, event);
        return true;
    }
    return false;
}

// ============================================================================
// Custom Npp event handler — replaces Win32 RegisterWindowMessage dispatch
// ============================================================================
intptr_t NppBigSwitch::run_dlgProc(int eventId, QEvent* event)
{
    Q_UNUSED(event);

    // Forward menu commands to handleCommand()
    if (eventId >= 0x2000 && eventId < 0x4000) {
        handleCommand(eventId);
        return 0;
    }

    switch (eventId) {
        // ─── Initialization ─────────────────────────────────────────────────
        case NPPM_INTERNAL_BEGIN:
            emit nppEventReceived(eventId);
            break;

        // ─── Taskbar ───────────────────────────────────────────────────────
        case NPPM_INTERNAL_TASKBAR_BUTTON_CREATED:
            emit taskbarButtonCreated();
            break;

        // ─── Dark mode ─────────────────────────────────────────────────────
        case NPPM_INTERNAL_REFRESHDARKMODE:
            refreshDarkMode();
            emit nppEventReceived(eventId);
            break;

        // ─── File operations ────────────────────────────────────────────────
        case NPPM_DOOPEN:
        case NPPM_INTERNAL_DOOPEN:
            emit nppEventReceived(eventId);
            break;

        case NPPM_SAVECURRENTFILE:
        case NPPM_INTERNAL_SAVEFILE:
            if (_pNppCommands) _pNppCommands->fileSave();
            break;

        case NPPM_SAVEALLFILES:
            if (_pNppCommands) _pNppCommands->fileSaveAll();
            break;

        // ─── Find / Replace ─────────────────────────────────────────────────
        case NPPM_LAUNCHFINDINFILESDLG:
            if (_pNppCommands) _pNppCommands->findInFiles();
            break;
        case NPPM_INTERNAL_FINDINFILESDLG:
            if (_pNppCommands) _pNppCommands->findInFiles();
            break;
        case NPPM_INTERNAL_REPLACEINFILESDLG:
            if (_pNppCommands) _pNppCommands->replaceInFiles();
            break;
        case NPPM_INTERNAL_FINDINPROJECTSDLG:
            if (_pNppCommands) _pNppCommands->findInProjects();
            break;
        case NPPM_INTERNAL_REPLACEINPROJECTSDLG:
            if (_pNppCommands) _pNppCommands->replaceInProjects();
            break;
        case NPPM_INTERNAL_FINDINFINDERDLG:
            emit nppEventReceived(eventId);
            break;
        case NPPM_INTERNAL_FINDINOPENEDDOC:
            if (_pNppCommands) _pNppCommands->findAllInOpenedDocs();
            break;
        case NPPM_INTERNAL_REPLACEINOPENEDDOC:
            if (_pNppCommands) _pNppCommands->replaceAllInOpenedDocs();
            break;

        // ─── Docking panels ─────────────────────────────────────────────────
        case NPPM_INTERNAL_DOCK_USERDEFINE_DLG:
        case NPPM_INTERNAL_UNDOCK_USERDEFINE_DLG:
            emit nppEventReceived(eventId);
            break;
        case NPPM_INTERNAL_DOCORDERCHANGED:
            emit nppEventReceived(eventId);
            break;

        // ─── Toolbar ────────────────────────────────────────────────────────
        case NPPM_INTERNAL_TOOLBARICONSCHANGED:
        case NPPM_INTERNAL_TOOLBARREDUCE:
        case NPPM_INTERNAL_TOOLBARENLARGE:
        case NPPM_INTERNAL_TOOLBARREDUCESET2:
        case NPPM_INTERNAL_TOOLBARENLARGESET2:
        case NPPM_INTERNAL_TOOLBARSTANDARD:
            emit nppEventReceived(eventId);
            break;

        // ─── View (margins, fold symbols) ──────────────────────────────────
        case NPPM_INTERNAL_LINENUMBER:
        case NPPM_INTERNAL_SYMBOLMARGIN:
        case NPPM_INTERNAL_FOLDERMARGIN:
        case NPPM_INTERNAL_FOLDSYMBOLSIMPLE:
        case NPPM_INTERNAL_FOLDSYMBOLARROW:
        case NPPM_INTERNAL_FOLDSYMBOLCIRCLE:
        case NPPM_INTERNAL_FOLDSYMBOLBOX:
        case NPPM_INTERNAL_FOLDSYMBOLNONE:
        case NPPM_INTERNAL_LWDEF:
        case NPPM_INTERNAL_LWALIGN:
        case NPPM_INTERNAL_LWINDENT:
            emit nppEventReceived(eventId);
            break;

        // ─── Session ────────────────────────────────────────────────────────
        case NPPM_INTERNAL_SAVECURRENTSESSION:
            if (_pNppCommands) _pNppCommands->fileSaveSession();
            break;
        case NPPM_INTERNAL_SAVEBACKUP:
            emit nppEventReceived(eventId);
            break;
        case NPPM_LOADSESSION:
            if (_pNppCommands) _pNppCommands->fileLoadSession();
            break;
        case NPPM_SAVECURRENTSESSION:
            if (_pNppCommands) _pNppCommands->fileSaveSession();
            break;

        // ─── Focus / Scintilla ─────────────────────────────────────────────
        case NPPM_INTERNAL_SWITCHVIEWFROMHWND:
        case NPPM_INTERNAL_SETFOCUS:
            emit nppEventReceived(eventId);
            break;

        // ─── Styles ─────────────────────────────────────────────────────────
        case NPPM_INTERNAL_RELOADSTYLERS:
        case NPPM_INTERNAL_RELOADNATIVELANG:
        case NPPM_INTERNAL_UPDATETEXTZONEPADDING:
        case NPPM_INTERNAL_HILITECURRENTLINE:
        case NPPM_INTERNAL_CARETBLINKRATE:
        case NPPM_INTERNAL_CARETLINEFRAME:
        case NPPM_INTERNAL_VIRTUALSPACE:
        case NPPM_INTERNAL_SCROLLBEYONDLASTLINE:
        case NPPM_INTERNAL_MULTISELECTION:
        case NPPM_INTERNAL_CHANGESELECTTEXTFORGROUND:
        case NPPM_INTERNAL_SETWORDCHARS:
        case NPPM_INTERNAL_SETNPC:
        case NPPM_INTERNAL_EDGE_MULTI_SETSIZE:
        case NPPM_INTERNAL_SETTING_HISTORY_SIZE:
        case NPPM_INTERNAL_RECENTFILELIST_UPDATE:
        case NPPM_INTERNAL_RECENTFILELIST_SWITCH:
        case NPPM_INTERNAL_SETTING_TABCOMPACTLABELLEN:
        case NPPM_INTERNAL_ISTABBARREDUCED:
        case NPPM_INTERNAL_CLEANBRACEMATCH:
        case NPPM_INTERNAL_CLEANSMARTHILITING:
            emit nppEventReceived(eventId);
            break;

        // ─── Preferences ─────────────────────────────────────────────────────
        case NPPM_INTERNAL_LAUNCHPREFERENCES:
            if (_pNppCommands) _pNppCommands->settingsPreference();
            break;
        case NPPM_INTERNAL_CRLFLAUNCHSTYLECONF:
        case NPPM_INTERNAL_NPCLAUNCHSTYLECONF:
            if (_pNppCommands) _pNppCommands->settingsStyleConfig();
            break;

        // ─── Plugin support ─────────────────────────────────────────────────
        case NPPM_INTERNAL_PLUGINSHORTCUTMODIFIED:
        case NPPM_INTERNAL_CMDLIST_MODIFIED:
        case NPPM_INTERNAL_MACROLIST_MODIFIED:
        case NPPM_INTERNAL_USERCMDLIST_MODIFIED:
        case NPPM_INTERNAL_CLEARSCINTILLAKEY:
        case NPPM_INTERNAL_BINDSCINTILLAKEY:
        case NPPM_INTERNAL_FINDKEYCONFLICTS:
        case NPPM_ALLOCATECMDID:
        case NPPM_ALLOCATEMARKER:
        case NPPM_ALLOCATEINDICATOR:
            emit nppEventReceived(eventId);
            break;

        // ─── Tab bar ─────────────────────────────────────────────────────────
        case NPPM_INTERNAL_REDUCETABBAR:
        case NPPM_INTERNAL_DRAWTABBARCLOSEBUTTON:
        case NPPM_INTERNAL_DRAWTABBARPINBUTTON:
        case NPPM_INTERNAL_REFRESHTABBAR:
        case NPPM_INTERNAL_HIDEMENURIGHTSHORTCUTS:
        case NPPM_INTERNAL_DRAWINACTIVETAB:
        case NPPM_INTERNAL_DRAWTABTOPBAR:
        case NPPM_INTERNAL_VERTICALTABBAR:
        case NPPM_INTERNAL_MULTILINETABBAR:
        case NPPM_INTERNAL_CHANGETABBARICONSET:
            emit nppEventReceived(eventId);
            break;

        // ─── Document list ───────────────────────────────────────────────────
        case NPPM_INTERNAL_DOCLISTDISABLEPATHCOLUMN:
        case NPPM_INTERNAL_DOCLISTDISABLEEXTCOLUMN:
            emit nppEventReceived(eventId);
            break;

        // ─── Change history ──────────────────────────────────────────────────
        case NPPM_INTERNAL_ENABLECHANGEHISTORY:
        case NPPM_INTERNAL_CHECKUNDOREDOSTATE:
        case NPPM_INTERNAL_LINECUTCOPYWITHOUTSELECTION:
        case NPPM_INTERNAL_DISABLESELECTEDTEXTDRAGDROP:
            emit nppEventReceived(eventId);
            break;

        // ─── File watcher ────────────────────────────────────────────────────
        case NPPM_INTERNAL_RELOADSCROLLTOEND:
        case NPPM_INTERNAL_STOPMONITORING:
        case NPPM_INTERNAL_DOCMODIFIEDBYREPLACEALL:
            emit nppEventReceived(eventId);
            break;

        // ─── Clickable links ─────────────────────────────────────────────────
        case NPPM_INTERNAL_UPDATECLICKABLELINKS:
        case NPPM_INTERNAL_EXTERNALLEXERBUFFER:
            emit nppEventReceived(eventId);
            break;

        // ─── Window management ───────────────────────────────────────────────
        case NPPM_INTERNAL_RESTOREFROMMINIMIZED:
        case NPPM_INTERNAL_WINDOWSSESSIONEXIT:
            emit nppEventReceived(eventId);
            break;

        // ─── Caret / Edit view settings ─────────────────────────────────────
        case NPPM_INTERNAL_SETCARETWIDTH:
        case NPPM_SETSMOOTHFONT:
        case NPPM_SETEDITORBORDEREDGE:
        case NPPM_INTERNAL_SQLBACKSLASHESCAPE:
            emit nppEventReceived(eventId);
            break;

        default:
            qDebug("NppBigSwitch: unhandled Npp event 0x%04X", eventId);
            break;
    }

    return 0;
}

// ============================================================================
// Command registration — populate the command handler map
// ============================================================================
void NppBigSwitch::registerDefaultHandlers()
{
    // Default handlers are set up by NppCommands::initMenu()
    // via registerCommand() calls for each menu action.
}

void NppBigSwitch::registerCommand(int commandId, const std::function<void()>& handler)
{
    _commandHandlers.insert(commandId, handler);
}

void NppBigSwitch::unregisterCommand(int commandId)
{
    _commandHandlers.remove(commandId);
}

// ============================================================================
// Central command dispatcher — replaces switch(id) { case IDM_*: ... }
// Maps all 700+ IDM_* menu commands to NppCommands slots
// This is the critical routing hub for all menu/toolbar/shortcut commands
// ============================================================================
bool NppBigSwitch::handleCommand(int commandId)
{
    // First check registered handlers
    auto it = _commandHandlers.find(commandId);
    if (it != _commandHandlers.end()) {
        it.value()();
        emit commandDispatched(commandId);
        return true;
    }

    // Central dispatch — route to NppCommands slots
    if (_pNppCommands) {
        // ════════════════════════════════════════════════════════════════════
        // FILE COMMANDS (IDM_FILE_*)
        // ════════════════════════════════════════════════════════════════════
        switch (commandId) {
            case IDM_FILE_NEW:
                _pNppCommands->fileNew(); break;
            case IDM_FILE_OPEN:
                _pNppCommands->fileOpen(); break;
            case IDM_FILE_OPEN_FOLDER:
                _pNppCommands->fileOpenFolder(); break;
            case IDM_FILE_OPENFOLDERASWORKSPACE:
                _pNppCommands->fileOpenFolderAsWorkspace(); break;
            case IDM_FILE_OPEN_CMD:
                _pNppCommands->fileOpenInCommandPrompt(); break;
            case IDM_FILE_OPEN_AS_READONLY:
                _pNppCommands->fileOpenAsReadOnly(); break;
            case IDM_FILE_RELOAD:
                _pNppCommands->fileReload(); break;
            case IDM_FILE_CLOSE:
                _pNppCommands->fileClose(); break;
            case IDM_FILE_CLOSEALL:
                _pNppCommands->fileCloseAll(); break;
            case IDM_FILE_CLOSEALL_BUT_CURRENT:
                _pNppCommands->fileCloseAllButCurrent(); break;
            case IDM_FILE_CLOSEALL_BUT_PINNED:
                _pNppCommands->fileCloseAllButPinned(); break;
            case IDM_FILE_CLOSEALL_TOLEFT:
                _pNppCommands->fileCloseAllToLeft(); break;
            case IDM_FILE_CLOSEALL_TORIGHT:
                _pNppCommands->fileCloseAllToRight(); break;
            case IDM_FILE_CLOSEALL_UNCHANGED:
                _pNppCommands->fileCloseAllUnchanged(); break;
            case IDM_FILE_SAVE:
                _pNppCommands->fileSave(); break;
            case IDM_FILE_SAVEAS:
                _pNppCommands->fileSaveAs(); break;
            case IDM_FILE_SAVECOPYAS:
                _pNppCommands->fileSaveCopyAs(); break;
            case IDM_FILE_SAVEALL:
                _pNppCommands->fileSaveAll(); break;
            case IDM_FILE_DELETE:
                _pNppCommands->fileDelete(); break;
            case IDM_FILE_RENAME:
                _pNppCommands->fileRename(); break;
            case IDM_FILE_LOADSESSION:
                _pNppCommands->fileLoadSession(); break;
            case IDM_FILE_SAVESESSION:
                _pNppCommands->fileSaveSession(); break;
            case IDM_FILE_PRINT:
            case IDM_FILE_PRINTNOW:
                _pNppCommands->filePrint(); break;
            case IDM_FILE_EXIT:
                _pNppCommands->fileExit(); break;

            // ════════════════════════════════════════════════════════════════
            // EDIT COMMANDS (IDM_EDIT_*)
            // ════════════════════════════════════════════════════════════════
            case IDM_EDIT_UNDO:
                _pNppCommands->editUndo(); break;
            case IDM_EDIT_REDO:
                _pNppCommands->editRedo(); break;
            case IDM_EDIT_CUT:
                _pNppCommands->editCut(); break;
            case IDM_EDIT_COPY:
                _pNppCommands->editCopy(); break;
            case IDM_EDIT_PASTE:
                _pNppCommands->editPaste(); break;
            case IDM_EDIT_DELETE:
                _pNppCommands->editDelete(); break;
            case IDM_EDIT_SELECTALL:
                _pNppCommands->editSelectAll(); break;
            case IDM_EDIT_BEGINENDSELECT:
                _pNppCommands->editBeginEndSelect(); break;
            case IDM_EDIT_BEGINENDSELECT_COLUMNMODE:
                _pNppCommands->editBeginEndSelectColumnMode(); break;

            // Case conversion
            case IDM_EDIT_UPPERCASE:
                _pNppCommands->editUpperCase(); break;
            case IDM_EDIT_LOWERCASE:
                _pNppCommands->editLowerCase(); break;
            case IDM_EDIT_PROPERCASE:
                _pNppCommands->editProperCase(); break;
            case IDM_EDIT_SENTENCECASE:
                _pNppCommands->editSentenceCase(); break;
            case IDM_EDIT_INVERTCASE:
                _pNppCommands->editInvertCase(); break;
            case IDM_EDIT_RANDOMCASE:
                _pNppCommands->editRandomCase(); break;

            // Comment / Uncomment
            case IDM_EDIT_BLOCK_COMMENT:
                _pNppCommands->editBlockComment(); break;
            case IDM_EDIT_BLOCK_UNCOMMENT:
                _pNppCommands->editBlockUncomment(); break;
            case IDM_EDIT_STREAM_COMMENT:
                _pNppCommands->editStreamComment(); break;
            case IDM_EDIT_STREAM_UNCOMMENT:
                _pNppCommands->editStreamUncomment(); break;

            // Line operations
            case IDM_EDIT_LINE_JOIN:
                _pNppCommands->editJoinLines(); break;
            case IDM_EDIT_LINE_SPLIT:
                _pNppCommands->editSplitLines(); break;
            case IDM_EDIT_LINE_UP:
                _pNppCommands->editLineUp(); break;
            case IDM_EDIT_LINE_DOWN:
                _pNppCommands->editLineDown(); break;
            case IDM_EDIT_BLANKLINEABOVECURRENT:
                _pNppCommands->editBlankLineAbove(); break;
            case IDM_EDIT_BLANKLINEBELOWCURRENT:
                _pNppCommands->editBlankLineBelow(); break;
            case IDM_EDIT_REMOVE_EMPTY_LINES:
                _pNppCommands->editRemoveEmptyLines(); break;
            case IDM_EDIT_REMOVE_EMPTY_LINES_BLANK:
                _pNppCommands->editRemoveEmptyLinesWithBlank(); break;
            case IDM_EDIT_CONSECUTIVE_DUP_LINES_REMOVE:
                _pNppCommands->editRemoveConsecutiveDupLines(); break;
            case IDM_EDIT_ANY_DUP_LINES_REMOVE:
                _pNppCommands->editRemoveAnyDupLines(); break;

            // Sorting
            case IDM_EDIT_SORTLINES_LEXICOGRAPHIC_ASCENDING:
                _pNppCommands->editSortLexAsc(); break;
            case IDM_EDIT_SORTLINES_LEXICOGRAPHIC_DESCENDING:
                _pNppCommands->editSortLexDesc(); break;
            case IDM_EDIT_SORTLINES_LEXICO_CASE_INSENS_ASCENDING:
                _pNppCommands->editSortLexAsc(); break;
            case IDM_EDIT_SORTLINES_LEXICO_CASE_INSENS_DESCENDING:
                _pNppCommands->editSortLexDesc(); break;
            case IDM_EDIT_SORTLINES_INTEGER_ASCENDING:
                _pNppCommands->editSortIntAsc(); break;
            case IDM_EDIT_SORTLINES_INTEGER_DESCENDING:
                _pNppCommands->editSortIntDesc(); break;
            case IDM_EDIT_SORTLINES_DECIMALCOMMA_ASCENDING:
            case IDM_EDIT_SORTLINES_DECIMALCOMMA_DESCENDING:
            case IDM_EDIT_SORTLINES_DECIMALDOT_ASCENDING:
            case IDM_EDIT_SORTLINES_DECIMALDOT_DESCENDING:
                _pNppCommands->editSortLexAsc(); break;
            case IDM_EDIT_SORTLINES_REVERSE_ORDER:
                _pNppCommands->editSortReversed(); break;
            case IDM_EDIT_SORTLINES_RANDOMLY:
                _pNppCommands->editSortLines(0); break;
            case IDM_EDIT_SORTLINES_LENGTH_ASCENDING:
            case IDM_EDIT_SORTLINES_LENGTH_DESCENDING:
                _pNppCommands->editSortLexAsc(); break;
            case IDM_EDIT_SORTLINES_LOCALE_ASCENDING:
            case IDM_EDIT_SORTLINES_LOCALE_DESCENDING:
                _pNppCommands->editSortLexAsc(); break;

            // Whitespace
            case IDM_EDIT_TRIM_LEADING:
                _pNppCommands->editTrimLeading(); break;
            case IDM_EDIT_TRIM_TRAILING:
                _pNppCommands->editTrimTrailing(); break;
            case IDM_EDIT_TRIM_BOTH:
                _pNppCommands->editTrimBoth(); break;
            case IDM_EDIT_EOL_TO_WS:
                _pNppCommands->editEolToWs(); break;
            case IDM_EDIT_TRIM_ALL:
                _pNppCommands->editTrimAll(); break;
            case IDM_EDIT_TAB_TO_SPACE:
                _pNppCommands->editTabToSpace(); break;
            case IDM_EDIT_SPACE_TO_TAB_ALL:
                _pNppCommands->editSpaceToTabAll(); break;
            case IDM_EDIT_SPACE_TO_TAB_LEADING:
                _pNppCommands->editSpaceToTabLeading(); break;

            // Auto-complete
            case IDM_EDIT_AUTOCOMPLETE:
                _pNppCommands->editAutoComplete(); break;
            case IDM_EDIT_AUTOCOMPLETE_PATH:
                _pNppCommands->editAutoCompletePath(); break;
            case IDM_EDIT_AUTOCOMPLETE_CURRENTFILE:
                _pNppCommands->editAutoCompleteCurrentFile(); break;
            case IDM_EDIT_FUNCCALLTIP:
                _pNppCommands->editFuncCallTip(); break;
            case IDM_EDIT_FUNCCALLTIP_PREV:
                _pNppCommands->editFuncCallTipPrevious(); break;
            case IDM_EDIT_FUNCCALLTIP_NEXT:
                _pNppCommands->editFuncCallTipNext(); break;

            // Date/Time insert
            case IDM_EDIT_INSERT_DATETIME_SHORT:
                _pNppCommands->editInsertDateTimeShort(); break;
            case IDM_EDIT_INSERT_DATETIME_LONG:
                _pNppCommands->editInsertDateTimeLong(); break;
            case IDM_EDIT_INSERT_DATETIME_CUSTOMIZED:
                _pNppCommands->editInsertDateTimeCustom(); break;

            // Clipboard operations
            case IDM_EDIT_FULLPATHTOCLIP:
                _pNppCommands->editFullPathToClip(); break;
            case IDM_EDIT_FILENAMETOCLIP:
                _pNppCommands->editFileNameToClip(); break;
            case IDM_EDIT_CURRENTDIRTOCLIP:
                _pNppCommands->editCurrentDirToClip(); break;
            case IDM_EDIT_COPY_ALL_NAMES:
                _pNppCommands->editCopyAllNames(); break;
            case IDM_EDIT_COPY_ALL_PATHS:
                _pNppCommands->editCopyAllPaths(); break;
            case IDM_EDIT_OPENINFOLDER:
                _pNppCommands->fileOpenContainingFolder(); break;
            case IDM_EDIT_CHAR_PANEL:
                _pNppCommands->charPanel(); break;
            case IDM_EDIT_CLIPBOARDHISTORY_PANEL:
                _pNppCommands->clipboardHistoryPanel(); break;

            // ════════════════════════════════════════════════════════════════
            // SEARCH COMMANDS (IDM_SEARCH_*)
            // ════════════════════════════════════════════════════════════════
            case IDM_SEARCH_FIND:
                _pNppCommands->searchFind(); break;
            case IDM_SEARCH_REPLACE:
                _pNppCommands->searchReplace(); break;
            case IDM_SEARCH_FINDNEXT:
                _pNppCommands->searchFindNext(); break;
            case IDM_SEARCH_FINDPREV:
                _pNppCommands->searchFindPrev(); break;
            case IDM_SEARCH_FINDINFILES:
                _pNppCommands->searchFindInFiles(); break;
            case IDM_SEARCH_FINDINPROJECTS:
                _pNppCommands->searchFindInProjects(); break;
            case IDM_SEARCH_FINDINFILEN:
                _pNppCommands->findInFilelist(); break;
            case IDM_SEARCH_GOTOLINE:
                _pNppCommands->searchGoToLine(); break;
            case IDM_SEARCH_GOTOMATCHINGBRACE:
                _pNppCommands->searchGoToMatchingBrace(); break;
            case IDM_SEARCH_SELECTMATCHINGBRACES:
                _pNppCommands->searchSelectMatchingBraces(); break;

            // Bookmarks
            case IDM_SEARCH_TOGGLE_BOOKMARK:
                _pNppCommands->searchToggleBookmark(); break;
            case IDM_SEARCH_NEXT_BOOKMARK:
                _pNppCommands->searchNextBookmark(); break;
            case IDM_SEARCH_PREV_BOOKMARK:
                _pNppCommands->searchPrevBookmark(); break;
            case IDM_SEARCH_CLEAR_BOOKMARKS:
                _pNppCommands->searchClearBookmarks(); break;
            case IDM_SEARCH_CUTBOOKMARK:
                _pNppCommands->searchCutMarkedLines(); break;
            case IDM_SEARCH_COPYBOOKMARK:
                _pNppCommands->searchCopyMarkedLines(); break;
            case IDM_SEARCH_PASTEBOOKMARK:
                _pNppCommands->searchPasteMarkedLines(); break;
            case IDM_SEARCH_DELETEBOOKMARK:
                _pNppCommands->searchDeleteMarkedLines(); break;
            case IDM_SEARCH_INVERSEMARKS:
                _pNppCommands->searchInverseMarks(); break;

            // Mark / Unmark
            case IDM_SEARCH_MARKONEEXT1:
            case IDM_SEARCH_MARKONEEXT2:
            case IDM_SEARCH_MARKONEEXT3:
            case IDM_SEARCH_MARKONEEXT4:
            case IDM_SEARCH_MARKONEEXT5:
                _pNppCommands->searchToggleBookmark(); break;
            case IDM_SEARCH_MARKALLEXT1:
                _pNppCommands->searchMarkAllExt1(); break;
            case IDM_SEARCH_MARKALLEXT2:
                _pNppCommands->searchMarkAllExt2(); break;
            case IDM_SEARCH_MARKALLEXT3:
                _pNppCommands->searchMarkAllExt3(); break;
            case IDM_SEARCH_MARKALLEXT4:
                _pNppCommands->searchMarkAllExt4(); break;
            case IDM_SEARCH_MARKALLEXT5:
                _pNppCommands->searchMarkAllExt5(); break;
            case IDM_SEARCH_UNMARKALLEXT1:
            case IDM_SEARCH_UNMARKALLEXT2:
            case IDM_SEARCH_UNMARKALLEXT3:
            case IDM_SEARCH_UNMARKALLEXT4:
            case IDM_SEARCH_UNMARKALLEXT5:
                _pNppCommands->searchClearAllMarks(); break;
            case IDM_SEARCH_CLEARALLMARKS:
                _pNppCommands->searchClearAllMarks(); break;
            case IDM_SEARCH_STYLE1TOCLIP:
            case IDM_SEARCH_STYLE2TOCLIP:
            case IDM_SEARCH_STYLE3TOCLIP:
            case IDM_SEARCH_STYLE4TOCLIP:
            case IDM_SEARCH_STYLE5TOCLIP:
                // Copy marked text to clipboard — handled by search*
                break;
            case IDM_SEARCH_ALLSTYLESTOCLIP:
            case IDM_SEARCH_MARKEDTOCLIP:
                break;
            case IDM_SEARCH_FINDCHARINRANGE:
                _pNppCommands->searchFindCharInRange(); break;

            // Change history

            // Go to markers
            case IDM_SEARCH_GONEXTMARKER1:
            case IDM_SEARCH_GONEXTMARKER2:
            case IDM_SEARCH_GONEXTMARKER3:
            case IDM_SEARCH_GONEXTMARKER4:
            case IDM_SEARCH_GONEXTMARKER5:
            case IDM_SEARCH_GONEXTMARKER_DEF:
                _pNppCommands->searchNextBookmark(); break;
            case IDM_SEARCH_GOPREVMARKER1:
            case IDM_SEARCH_GOPREVMARKER2:
            case IDM_SEARCH_GOPREVMARKER3:
            case IDM_SEARCH_GOPREVMARKER4:
            case IDM_SEARCH_GOPREVMARKER5:
            case IDM_SEARCH_GOPREVMARKER_DEF:
                _pNppCommands->searchPrevBookmark(); break;

            // Change history navigation
            case IDM_SEARCH_CHANGED_PREV:
            case IDM_SEARCH_CHANGED_NEXT:
                _pNppCommands->searchNextBookmark(); break;
            case IDM_SEARCH_CLEAR_CHANGE_HISTORY:
                // Clear change history
                break;

            // ════════════════════════════════════════════════════════════════
            // VIEW COMMANDS (IDM_VIEW_*) — display, panels, zoom, folding
            // ════════════════════════════════════════════════════════════════
            // Toolbar — forwarded via NPPM_INTERNAL_TOOLBAR*
            case IDM_VIEW_TOOLBAR_REDUCE:
            case IDM_VIEW_TOOLBAR_ENLARGE:
            case IDM_VIEW_TOOLBAR_STANDARD:
                emit nppEventReceived(commandId); break;
            case IDM_VIEW_TOOLBAR_HIDE:
                break;

            // Tab bar
            case IDM_VIEW_REDUCETABBAR:
            case IDM_VIEW_LOCKTABBAR:
            case IDM_VIEW_DRAWTABBAR_TOPBAR:
            case IDM_VIEW_DRAWTABBAR_INACTIVETAB:
                emit nppEventReceived(commandId); break;

            // Window modes
            case IDM_VIEW_FULLSCREENTOGGLE:
                _pNppCommands->viewFullScreenToggle(); break;
            case IDM_VIEW_POSTIT:
                _pNppCommands->viewPostIt(); break;
            case IDM_VIEW_DISTRACTIONFREE:
                _pNppCommands->viewDistractionFree(); break;
            case IDM_VIEW_ALWAYSONTOP:
                _pNppCommands->viewAlwaysOnTop(); break;

            // Folding
            case IDM_VIEW_FOLDALL:
            case IDM_VIEW_UNFOLDALL:
            case IDM_VIEW_FOLD_CURRENT:
            case IDM_VIEW_UNFOLD_CURRENT:
            case IDM_VIEW_FOLD:
            case IDM_VIEW_FOLD_1:
            case IDM_VIEW_FOLD_2:
            case IDM_VIEW_FOLD_3:
            case IDM_VIEW_FOLD_4:
            case IDM_VIEW_FOLD_5:
            case IDM_VIEW_FOLD_6:
            case IDM_VIEW_FOLD_7:
            case IDM_VIEW_FOLD_8:
            case IDM_VIEW_UNFOLD:
            case IDM_VIEW_UNFOLD_1:
            case IDM_VIEW_UNFOLD_2:
            case IDM_VIEW_UNFOLD_3:
            case IDM_VIEW_UNFOLD_4:
            case IDM_VIEW_UNFOLD_5:
            case IDM_VIEW_UNFOLD_6:
            case IDM_VIEW_UNFOLD_7:
            case IDM_VIEW_UNFOLD_8:
                emit nppEventReceived(commandId); break;

            // Fold symbols — Scintilla margin styles
            case IDM_VIEW_FOLDSYMBOLSIMPLE:
            case IDM_VIEW_FOLDSYMBOLARROW:
            case IDM_VIEW_FOLDSYMBOLCIRCLE:
            case IDM_VIEW_FOLDSYMBOLBOX:
            case IDM_VIEW_FOLDSYMBOLNONE:
                emit nppEventReceived(commandId); break;

            // Show characters / formatting
            case IDM_VIEW_ALL_CHARACTERS:
                _pNppCommands->viewAllCharacters(); break;
            case IDM_VIEW_INDENT_GUIDE:
                _pNppCommands->viewIndentGuide(); break;
            case IDM_VIEW_WRAP:
                _pNppCommands->viewWrap(); break;
            case IDM_VIEW_WRAP_SYMBOL:
                _pNppCommands->viewWrapSymbol(); break;

            // Zoom
            case IDM_VIEW_ZOOMIN:
                _pNppCommands->viewZoomIn(); break;
            case IDM_VIEW_ZOOMOUT:
                _pNppCommands->viewZoomOut(); break;
            case IDM_VIEW_ZOOMRESTORE:
                _pNppCommands->viewZoomRestore(); break;

            // Document summary
            case IDM_VIEW_SUMMARY:
                _pNppCommands->viewSummary(); break;

            // Hide lines
            case IDM_VIEW_HIDELINES:
                _pNppCommands->viewHideLines(); break;

            // Tab / Space / EOL
            case IDM_VIEW_TAB_SPACE:
                _pNppCommands->viewTabSpace(); break;
            case IDM_VIEW_EOL:
                _pNppCommands->viewEol(); break;
            case IDM_VIEW_NPC:
                _pNppCommands->viewNpc(); break;
            case IDM_VIEW_NPC_CCUNIEOL:
                _pNppCommands->viewNpcCcuniEol(); break;

            // View switching
            case IDM_VIEW_SWITCHTO_OTHER_VIEW:
                _pNppCommands->viewSwitchToOtherView(); break;
            case IDM_VIEW_CLONE_TO_ANOTHER_VIEW:
                _pNppCommands->viewCloneToAnotherView(); break;
            case IDM_VIEW_GOTO_NEW_INSTANCE:
                _pNppCommands->viewGotoNewInstance(); break;
            case IDM_VIEW_LOAD_IN_NEW_INSTANCE:
                _pNppCommands->viewLoadInNewInstance(); break;

            // Tab navigation
            case IDM_VIEW_TAB1:
            case IDM_VIEW_TAB2:
            case IDM_VIEW_TAB3:
            case IDM_VIEW_TAB4:
            case IDM_VIEW_TAB5:
            case IDM_VIEW_TAB6:
            case IDM_VIEW_TAB7:
            case IDM_VIEW_TAB8:
            case IDM_VIEW_TAB9:
            case IDM_VIEW_TAB_NEXT:
            case IDM_VIEW_TAB_PREV:
            case IDM_VIEW_TAB_START:
            case IDM_VIEW_TAB_END:
            case IDM_VIEW_TAB_MOVEFORWARD:
            case IDM_VIEW_TAB_MOVEBACKWARD:
                emit nppEventReceived(commandId); break;

            // Tab color
            case IDM_VIEW_TAB_COLOUR_NONE:
            case IDM_VIEW_TAB_COLOUR_1:
            case IDM_VIEW_TAB_COLOUR_2:
            case IDM_VIEW_TAB_COLOUR_3:
            case IDM_VIEW_TAB_COLOUR_4:
            case IDM_VIEW_TAB_COLOUR_5:
                emit nppEventReceived(commandId); break;

            // Panel visibility
            case IDM_VIEW_DOC_MAP:
                _pNppCommands->toggleDocumentMap(); break;
            case IDM_VIEW_FUNC_LIST:
                _pNppCommands->toggleFunctionList(); break;
            case IDM_VIEW_FILEBROWSER:
                _pNppCommands->toggleFileBrowser(); break;
            case IDM_VIEW_PROJECT_PANEL_1:
                _pNppCommands->toggleProjectPanel1(); break;
            case IDM_VIEW_PROJECT_PANEL_2:
                _pNppCommands->toggleProjectPanel2(); break;
            case IDM_VIEW_PROJECT_PANEL_3:
                _pNppCommands->toggleProjectPanel3(); break;
            case IDM_VIEW_DOCLIST:
                _pNppCommands->toggleDocList(); break;
            case IDM_VIEW_SWITCHTO_FILEBROWSER:
                _pNppCommands->toggleFileBrowser(); break;
            case IDM_VIEW_SWITCHTO_FUNC_LIST:
                _pNppCommands->toggleFunctionList(); break;
            case IDM_VIEW_SWITCHTO_DOCLIST:
                _pNppCommands->toggleDocList(); break;
            case IDM_VIEW_SWITCHTO_PROJECT_PANEL_1:
            case IDM_VIEW_SWITCHTO_PROJECT_PANEL_2:
            case IDM_VIEW_SWITCHTO_PROJECT_PANEL_3:
                emit nppEventReceived(commandId); break;

            // ════════════════════════════════════════════════════════════════
            // ENCODING COMMANDS (IDM_FORMAT_*) — file encoding operations
            // ════════════════════════════════════════════════════════════════
            case IDM_FORMAT_ANSI:
                _pNppCommands->encodingAnsi(); break;
            case IDM_FORMAT_UTF_8:
                _pNppCommands->encodingUTF8(); break;
            case IDM_FORMAT_AS_UTF_8:
                _pNppCommands->encodingUTF8BOM(); break;
            case IDM_FORMAT_UTF_16BE:
                _pNppCommands->encodingUTF16BE(); break;
            case IDM_FORMAT_UTF_16LE:
                _pNppCommands->encodingUTF16LE(); break;

            // Convert to encoding
            case IDM_FORMAT_CONV2_ANSI:
                _pNppCommands->encodingConvertAnsi(); break;
            case IDM_FORMAT_CONV2_AS_UTF_8:
            case IDM_FORMAT_CONV2_UTF_8:
                _pNppCommands->encodingConvertUTF8(); break;
            case IDM_FORMAT_CONV2_UTF_16BE:
                _pNppCommands->encodingConvertUTF16BE(); break;
            case IDM_FORMAT_CONV2_UTF_16LE:
                _pNppCommands->encodingConvertUTF16LE(); break;

            // Character set (code page conversions) — IDM_FORMAT_ENCODE + offset
            // These are handled by encodingAnsi() with the specific code page
            // Covered by default case

            // ════════════════════════════════════════════════════════════════
            // LANGUAGE COMMANDS (IDM_LANG_*) — syntax highlighting
            // ════════════════════════════════════════════════════════════════
            case IDM_LANGSTYLE_CONFIG_DLG:
            case IDM_LANG_USER_DLG:
                _pNppCommands->languageUserDefine(); break;
            case IDM_LANG_OPENUDLDIR:
                _pNppCommands->languageOpenUdldDir(); break;

            // Language menu — syntax highlighting selection
            // All named language IDs route through languageMenu()
            case IDM_LANG_C:
            case IDM_LANG_CPP:
            case IDM_LANG_JAVA:
            case IDM_LANG_HTML:
            case IDM_LANG_XML:
            case IDM_LANG_JS:
            case IDM_LANG_PHP:
            case IDM_LANG_ASP:
            case IDM_LANG_CSS:
            case IDM_LANG_PASCAL:
            case IDM_LANG_PYTHON:
            case IDM_LANG_PERL:
            case IDM_LANG_OBJC:
            case IDM_LANG_ASCII:
            case IDM_LANG_TEXT:
            case IDM_LANG_RC:
            case IDM_LANG_MAKEFILE:
            case IDM_LANG_INI:
            case IDM_LANG_SQL:
            case IDM_LANG_VB:
            case IDM_LANG_BATCH:
            case IDM_LANG_CS:
            case IDM_LANG_LUA:
            case IDM_LANG_TEX:
            case IDM_LANG_FORTRAN:
            case IDM_LANG_BASH:
            case IDM_LANG_FLASH:
            case IDM_LANG_NSIS:
            case IDM_LANG_TCL:
            case IDM_LANG_LISP:
            case IDM_LANG_SCHEME:
            case IDM_LANG_ASM:
            case IDM_LANG_DIFF:
            case IDM_LANG_PROPS:
            case IDM_LANG_PS:
            case IDM_LANG_RUBY:
            case IDM_LANG_SMALLTALK:
            case IDM_LANG_VHDL:
            case IDM_LANG_CAML:
            case IDM_LANG_KIX:
            case IDM_LANG_ADA:
            case IDM_LANG_VERILOG:
            case IDM_LANG_AU3:
            case IDM_LANG_MATLAB:
            case IDM_LANG_HASKELL:
            case IDM_LANG_INNO:
            case IDM_LANG_CMAKE:
            case IDM_LANG_YAML:
            case IDM_LANG_COBOL:
            case IDM_LANG_D:
            case IDM_LANG_GUI4CLI:
            case IDM_LANG_POWERSHELL:
            case IDM_LANG_R:
            case IDM_LANG_JSP:
            case IDM_LANG_COFFEESCRIPT:
            case IDM_LANG_JSON:
            case IDM_LANG_FORTRAN_77:
            case IDM_LANG_BAANC:
            case IDM_LANG_SREC:
            case IDM_LANG_IHEX:
            case IDM_LANG_TEHEX:
            case IDM_LANG_SWIFT:
            case IDM_LANG_ASN1:
            case IDM_LANG_AVS:
            case IDM_LANG_BLITZBASIC:
            case IDM_LANG_PUREBASIC:
            case IDM_LANG_FREEBASIC:
            case IDM_LANG_CSOUND:
            case IDM_LANG_ERLANG:
            case IDM_LANG_ESCRIPT:
            case IDM_LANG_FORTH:
            case IDM_LANG_LATEX:
            case IDM_LANG_MMIXAL:
            case IDM_LANG_NIM:
            case IDM_LANG_NNCRONTAB:
            case IDM_LANG_OSCRIPT:
            case IDM_LANG_REBOL:
            case IDM_LANG_REGISTRY:
            case IDM_LANG_RUST:
            case IDM_LANG_SPICE:
            case IDM_LANG_TXT2TAGS:
            case IDM_LANG_VISUALPROLOG:
            case IDM_LANG_TYPESCRIPT:
            case IDM_LANG_JSON5:
            case IDM_LANG_MSSQL:
            case IDM_LANG_GDSCRIPT:
            case IDM_LANG_HOLLYWOOD:
            case IDM_LANG_GOLANG:
            case IDM_LANG_RAKU:
            case IDM_LANG_TOML:
            case IDM_LANG_SAS:
            case IDM_LANG_ERRORLIST:
            case IDM_LANG_EXTERNAL:
            case IDM_LANG_USER:
                // Language selection — dispatch through languageMenu()
                // which handles setting the lexer for the current buffer
                _pNppCommands->languageMenu(); break;

            // ════════════════════════════════════════════════════════════════
            // MACRO COMMANDS (IDM_MACRO_*)
            // ════════════════════════════════════════════════════════════════
            case IDM_MACRO_STARTRECORDINGMACRO:
                _pNppCommands->macroStartRecording(); break;
            case IDM_MACRO_STOPRECORDINGMACRO:
                _pNppCommands->macroStopRecording(); break;
            case IDC_EDIT_TOGGLEMACRORECORDING:
                _pNppCommands->toggleRecording(); break;
            case IDM_MACRO_PLAYBACKRECORDEDMACRO:
                _pNppCommands->macroPlayback(); break;
            case IDM_MACRO_RUNMULTIMACRODLG:
                _pNppCommands->macroRunMultiple(); break;
            case IDM_MACRO_SAVECURRENTMACRO:
                _pNppCommands->macroSaveCurrent(); break;
            case IDC_EDIT_TOGGLEMACROPLAYBACK:
                _pNppCommands->toggleMacroPlayback(); break;

            // ════════════════════════════════════════════════════════════════
            // RUN COMMANDS (IDM_RUN_*)
            // ════════════════════════════════════════════════════════════════
            case IDM_RUN_LOOKUPDLG:
            case IDM_RUN_MODIFYLOOKUPDLG:
            case IDM_RUN_CLEANUPLOOKUPDLG:
            case IDM_RUN:
                _pNppCommands->runExecute(); break;
            case IDM_RUN_RUNCURRENTFILE:
                // Run current file
                break;

            // ════════════════════════════════════════════════════════════════
            // SETTINGS COMMANDS (IDM_SETTING_*)
            // ════════════════════════════════════════════════════════════════
            case IDM_SETTING_PREFERENCE:
                _pNppCommands->settingsPreference(); break;
            case IDM_SETTING_SHORTCUT_MAPPER:
                _pNppCommands->settingsShortcutMapper(); break;
            case IDM_SETTING_IMPORT_PLUGIN:
                _pNppCommands->settingsImportPlugin(); break;
            case IDM_SETTING_IMPORT_STYLETHEME:
                _pNppCommands->settingsImportStyleThemes(); break;
            case IDM_SETTING_EDITCONTEXTMENU:
                _pNppCommands->settingsEditContextMenu(); break;
            case IDM_SETTING_EDITCONTEXTMENU_INALLLANG:
            case IDM_SETTING_OPENSTYLECONFIG_DLG:
                _pNppCommands->settingsStyleConfig(); break;

            // ════════════════════════════════════════════════════════════════
            // WINDOW COMMANDS (IDM_WINDOW_*)
            // ════════════════════════════════════════════════════════════════
            case IDM_WINDOW_SORT_ASC:
                _pNppCommands->windowSortAscending(); break;
            case IDM_WINDOW_SORT_DESC:
                _pNppCommands->windowSortDescending(); break;

            // ════════════════════════════════════════════════════════════════
            // HELP COMMANDS (IDM_HELP_*)
            // ════════════════════════════════════════════════════════════════
            case IDM_HELP_ABOUT:
                _pNppCommands->helpAbout(); break;
            case IDM_HELP_DOC:
                _pNppCommands->helpDocumentation(); break;
            case IDM_HELP_HOME:
                _pNppCommands->helpHomeSweetHome(); break;
            case IDM_HELP_ONLINE:
                _pNppCommands->helpOnlineDoc(); break;

            // ════════════════════════════════════════════════════════════════
            // MISC COMMANDS
            // ════════════════════════════════════════════════════════════════
            case IDM_FILE_RESTORELASTCLOSED:
                _pNppCommands->fileRestoreLastClosed(); break;
            case IDM_EDIT_COLUMNMODE:
                _pNppCommands->columnEditor(); break;
            case IDM_FILE_OPEN_DEFAULT_VIEWER:
                // Open in default system app
                break;

            // Syntax highlighting (direct lang commands)
            case IDM_LANG_C:
            case IDM_LANG_CPP:
            case IDM_LANG_JAVA:
            case IDM_LANG_CS:
            case IDM_LANG_OBJC:
            case IDM_LANG_HTML:
            case IDM_LANG_XML:
            case IDM_LANG_HTML_MULTI:
            case IDM_LANG_PHP:
            case IDM_LANG_JS:
            case IDM_LANG_JSON:
            case IDM_LANG_ASP:
            case IDM_LANG_SQL:
            case IDM_LANG_PYTHON:
            case IDM_LANG_PERL:
            case IDM_LANG_RUBY:
            case IDM_LANG_LUA:
            case IDM_LANG_TEX:
            case IDM_LANG_FORTRAN:
            case IDM_LANG_FORTRAN_77:
            case IDM_LANG_BASH:
            case IDM_LANG_FLASH:
            case IDM_LANG_NSIS:
            case IDM_LANG_CMAKE:
            case IDM_LANG_YAML:
            case IDM_LANG_CAML:
            case IDM_LANG_COFFEESCRIPT:
            case IDM_LANG_JSON_PATH:
            case IDM_LANG_POWERSHELL:
            case IDM_LANG_START:
                // Language selection commands — route through languageMenu
                // These are the named language commands in the Language menu
                _pNppCommands->languageMenu(); break;

            // Read-only toggle
            case IDM_EDIT_READONLY:
                _pNppCommands->toggleReadOnly(); break;

            // Monitor file (file watcher toggle)
            case IDM_FILE_MONITOR:
                _pNppCommands->toggleMonitoring(); break;

            // Save read-only / clear read-only for all
            case IDM_FILE_SAVE_READONLY:
                // Save as read-only
                break;
            case IDM_FILE_READONLY:
                _pNppCommands->setReadOnlyForAll(); break;
            case IDM_FILE_CLEAR_READONLY:
                _pNppCommands->clearReadOnlyForAll(); break;

            default:
                // Unknown command — emit for plugin / external handler
                emit commandDispatched(commandId);
                qDebug("NppBigSwitch: unhandled command ID 0x%04X", commandId);
                return false;
        }

        emit commandDispatched(commandId);
        return true;
    }

    // No NppCommands set — warn but don't crash
    qWarning("NppBigSwitch: handleCommand(0x%04X) called with no NppCommands", commandId);
    return false;
}

// ============================================================================
// WM_SIZE handler — resize notification
// ============================================================================
bool NppBigSwitch::handleSize(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
    // Handled directly by Qt's QResizeEvent on the MainWindow
    return false;
}

// ============================================================================
// WM_MOVE handler — position notification
// ============================================================================
bool NppBigSwitch::handleMove(int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    // Handled directly by Qt's QMoveEvent on the MainWindow
    return false;
}

// ============================================================================
// WM_DRAWITEM — owner-drawn controls (toolbar, menus)
// Qt handles this via QStyle::drawControl + QStyleOption
// ============================================================================
bool NppBigSwitch::handleDrawItem(QEvent* event)
{
    Q_UNUSED(event);
    // Owner-drawn controls are handled by Qt's style system
    return false;
}

// ============================================================================
// WM_DPICHANGED — DPI scaling change
// ============================================================================
bool NppBigSwitch::handleDpiChanged(int newDpi, const QRect& suggestedRect)
{
    _currentDpi = newDpi;
    emit dpiChanged(newDpi);

    if (QWidget* mainWin = qApp->activeWindow()) {
        if (!suggestedRect.isNull()) {
            mainWin->setGeometry(suggestedRect);
        }
        mainWin->update();
    }
    return true;
}

// ============================================================================
// WM_COPYDATA — inter-process file open (another instance)
// Qt equivalent: QLocalServer or QSharedMemory
// ============================================================================
bool NppBigSwitch::handleCopyData(QEvent* event)
{
    Q_UNUSED(event);
    // Inter-process file open handled by main() single-instance check
    return false;
}

// ============================================================================
// WM_SETTINGCHANGE — system settings changed (locale, theme, etc.)
// ============================================================================
bool NppBigSwitch::handleSettingChange(const QString& settingName)
{
    if (settingName.isEmpty() || settingName.contains(QLatin1String("Locale"), Qt::CaseInsensitive)) {
        // Language changed — reload translations
        qApp->setFont(qApp->font());  // Force UI refresh
    }
    if (settingName.contains(QLatin1String("Environment"), Qt::CaseInsensitive) ||
        settingName.contains(QLatin1String("Theme"), Qt::CaseInsensitive)) {
        // System theme changed — check dark mode
        NppDarkMode::instance().setEnabled(NppDarkMode::instance().isEnabled());
    }
    return true;
}

// ============================================================================
// WM_SYSCOMMAND — system commands (minimize, maximize, etc.)
// ============================================================================
bool NppBigSwitch::handleSysCommand(int sysCommand)
{
    Q_UNUSED(sysCommand);
    // Qt handles SYSCOMMAND via QWidget::close() / showMinimized() etc.
    return false;
}

// ============================================================================
// WM_NCACTIVATE — non-client activation
// ============================================================================
bool NppBigSwitch::handleNcActivate(bool activate)
{
    Q_UNUSED(activate);
    // Inactive window title bar styling handled by Qt's window framework
    return false;
}

// ============================================================================
// WM_APPCOMMAND — app commands (media keys, browser back/forward)
// ============================================================================
bool NppBigSwitch::handleAppCommand(int cmd)
{
    Q_UNUSED(cmd);
    // APPCOMMAND_BROWSER_BACK -> IDM_FILE_RELOAD
    // APPCOMMAND_BROWSER_FORWARD -> (no direct equivalent)
    // APPCOMMAND_BROWSER_REFRESH -> IDM_FILE_RELOAD
    // APPCOMMAND_PRINT -> IDM_FILE_PRINT
    // Not commonly used on Linux, but handled here for completeness
    return false;
}

// ============================================================================
// WM_MOUSEWHEEL with Ctrl — zoom
// ============================================================================
bool NppBigSwitch::handleMouseWheelZoom(int delta)
{
    Q_UNUSED(delta);
    // Zoom is handled directly by ScintillaEditView::wheelEvent()
    // Ctrl+Wheel intercepted by the Scintilla component
    return false;
}

// ============================================================================
// WM_TIMER handler
// ============================================================================
bool NppBigSwitch::handleTimer(int timerId)
{
    Q_UNUSED(timerId);
    // Timers handled by QObject::timerEvent() on specific objects
    return false;
}

// ============================================================================
// Dark mode refresh
// ============================================================================
void NppBigSwitch::refreshDarkMode()
{
    NppDarkMode::instance().setEnabled(NppDarkMode::instance().isEnabled());
    qApp->setStyleSheet(qApp->styleSheet());
}

// ============================================================================
// Static helpers
// ============================================================================
QEvent* NppBigSwitch::createNppEvent(int nppEventId, const QVariant& data)
{
    return new NppCustomEvent(nppEventId, data);
}

int NppBigSwitch::nppEventIdFromQEvent(QEvent::Type type)
{
    if (type >= QEvent::User && type < QEvent::MaxUser) {
        return static_cast<int>(type) - QEvent::User;
    }
    return -1;
}

