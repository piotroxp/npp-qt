// Semantic lift: Main window implementation
// Original: PowerEditor/src/Notepad_plus_Window.cpp
// Target: npp-qt/src/MainWindow.cpp

#include "MainWindow.h"
#include "Buffer.h"
#include "ScintillaComponent.h"
#include "menuCmdID.h"
#include "FindReplaceDlg.h"
#include "NppSciCompat.h"
#include "NppCommands.h"
#include "IconLoader.h"
#include "Notepad_plus.h"
#include "WinControls/RunMacroDlg.h"

#include <QShortcut>
#include <QDir>
#include <Qsci/qsciscintilla.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QCloseEvent>
#include <QStyle>
#include <QStyleOption>
#include <QMenu>

// Static member initialization
QWidget* MainWindow::gNppHWND = nullptr;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _settings(QSettings::IniFormat, QSettings::UserScope, "Notepad++", "Notepad++Qt")
{
    fprintf(stderr, "DBG MainWindow ctor: START\n"); fflush(stderr);
    setAcceptDrops(true);
    fprintf(stderr, "DBG MainWindow ctor: setupUi START\n"); fflush(stderr);
    setupUi();
    fprintf(stderr, "DBG MainWindow ctor: setupUi DONE\n"); fflush(stderr);
    fprintf(stderr, "DBG MainWindow ctor: loadSettings START\n"); fflush(stderr);
    loadSettings();
    fprintf(stderr, "DBG MainWindow ctor: loadSettings DONE\n"); fflush(stderr);
    gNppHWND = this;  // register main window handle
    fprintf(stderr, "DBG MainWindow ctor: END\n"); fflush(stderr);
}

MainWindow::~MainWindow()
{
    gNppHWND = nullptr;  // unregister on destruction
}

void MainWindow::init()
{
    newFile();
    loadSession();
    show();  // Qt6: show the main window (Win32 ::ShowWindow equivalent)
}

void MainWindow::setupUi()
{
    fprintf(stderr, "DBG setupUi: START\n"); fflush(stderr);
    setWindowTitle("Notepad++ (Qt)");
    resize(1200, 800);
    fprintf(stderr, "DBG setupUi: NppCommands START\n"); fflush(stderr);
    // Instantiate NppCommands FIRST — all actions/menus connect to it
    _pNppCommands = new NppCommands(this);
    _pNppCommands->setMainWindow(this);
    fprintf(stderr, "DBG setupUi: NppCommands DONE\n"); fflush(stderr);
    fprintf(stderr, "DBG setupUi: createActions START\n"); fflush(stderr);
    createActions();
    createShortcuts();
    fprintf(stderr, "DBG setupUi: createActions DONE\n"); fflush(stderr);
    fprintf(stderr, "DBG setupUi: createMenus START\n"); fflush(stderr);
    createMenus();
    fprintf(stderr, "DBG setupUi: createMenus DONE\n"); fflush(stderr);
    fprintf(stderr, "DBG setupUi: createToolBars START\n"); fflush(stderr);
    createToolBars();
    fprintf(stderr, "DBG setupUi: createToolBars DONE\n"); fflush(stderr);
    fprintf(stderr, "DBG setupUi: createStatusBar START\n"); fflush(stderr);
    createStatusBar();
    fprintf(stderr, "DBG setupUi: createStatusBar DONE\n"); fflush(stderr);
    fprintf(stderr, "DBG setupUi: createDocumentPane START\n"); fflush(stderr);
    createDocumentPane();   // creates _pDocTabBar with Scintilla editors
    fprintf(stderr, "DBG setupUi: createPanels START\n"); fflush(stderr);
    createPanels();
    fprintf(stderr, "DBG setupUi: createPanels END\n"); fflush(stderr);

    // FindReplaceDlg already created in constructor initializer list above
    // _pDocTabBar (QTabWidget) is the central widget — holds Scintilla editor tabs
    setCentralWidget(_pDocTabBar);
    fprintf(stderr, "DBG setupUi: setCentralWidget DONE\n"); fflush(stderr);

    // Qt6: QTabWidget::currentChanged and tabCloseRequested are protected signals.
    // Connect via the public QTabBar base class signals.
    QTabBar* docTabBar = qobject_cast<QTabBar*>(_pDocTabBar);
    connect(docTabBar, &QTabBar::currentChanged, this, &MainWindow::onTabChanged);
    connect(docTabBar, &QTabBar::tabCloseRequested, this, [this](int i) { closeFile(i); });
    fprintf(stderr, "DBG setupUi: tab signals CONNECTED\n"); fflush(stderr);

    // Initialize Find/Replace dialog (editor set lazily via setCurrentEditor())
    fprintf(stderr, "DBG setupUi: FindReplaceDlg START\n"); fflush(stderr);
    _pFindReplaceDlg = new FindReplaceDlg();
    _pFindReplaceDlg->setParent(this);
    _pFindReplaceDlg->hide();
    fprintf(stderr, "DBG setupUi: FindReplaceDlg DONE\n"); fflush(stderr);
    fprintf(stderr, "DBG setupUi: END\n"); fflush(stderr);
}

void MainWindow::createActions()
{
    fprintf(stderr, "DBG createActions: START\n"); fflush(stderr);
    // === File toolbar actions — wired to NppCommands ===
    _pActNew = new QAction(globalIconLoader().loadWithTheme("new", "document-new"), tr("&New"), this);
    _pActNew->setShortcut(QKeySequence::New);
    connect(_pActNew, &QAction::triggered, _pNppCommands, &NppCommands::fileNew);

    _pActOpen = new QAction(globalIconLoader().loadWithTheme("open", "document-open"), tr("&Open..."), this);
    _pActOpen->setShortcut(QKeySequence::Open);
    connect(_pActOpen, &QAction::triggered, _pNppCommands, &NppCommands::fileOpen);

    _pActSave = new QAction(globalIconLoader().loadWithTheme("save", "document-save"), tr("&Save"), this);
    _pActSave->setShortcut(QKeySequence::Save);
    connect(_pActSave, &QAction::triggered, _pNppCommands, &NppCommands::fileSave);

    // File menu named actions
    _pActSaveAs = new QAction(globalIconLoader().loadWithTheme("save", "document-save-as"), tr("Save &As..."), this);
    _pActSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(_pActSaveAs, &QAction::triggered, _pNppCommands, &NppCommands::fileSaveAs);

    _pActReload = new QAction(tr("&Reload"), this);
    _pActReload->setShortcut(QKeySequence::Refresh);
    connect(_pActReload, &QAction::triggered, _pNppCommands, &NppCommands::fileReload);

    _pActClose = new QAction(tr("&Close"), this);
    _pActClose->setShortcut(QKeySequence::Close);
    connect(_pActClose, &QAction::triggered, _pNppCommands, &NppCommands::fileClose);

    // Ctrl+Shift+W for Close All (additive shortcut on the existing action)
    _pActCloseAll->setShortcut(QKeySequence(tr("Ctrl+Shift+W")));

    _pActCloseAllButCurrent = new QAction(tr("Close All &But Current"), this);
    connect(_pActCloseAllButCurrent, &QAction::triggered, _pNppCommands, &NppCommands::fileCloseAllButCurrent);

    _pActPrint = new QAction(globalIconLoader().loadWithTheme("print", "document-print"), tr("&Print..."), this);
    _pActPrint->setShortcut(QKeySequence::Print);
    connect(_pActPrint, &QAction::triggered, _pNppCommands, &NppCommands::filePrint);

    _pActExit = new QAction(tr("E&xit"), this);
    _pActExit->setShortcut(QKeySequence::Quit);
    connect(_pActExit, &QAction::triggered, _pNppCommands, &NppCommands::fileExit);

    // === Edit toolbar actions — wired to NppCommands ===
    _pActCut = new QAction(globalIconLoader().loadWithTheme("cut", "edit-cut"), tr("Cu&t"), this);
    _pActCut->setShortcut(QKeySequence::Cut);
    connect(_pActCut, &QAction::triggered, _pNppCommands, &NppCommands::editCut);

    _pActCopy = new QAction(globalIconLoader().loadWithTheme("copy", "edit-copy"), tr("&Copy"), this);
    _pActCopy->setShortcut(QKeySequence::Copy);
    connect(_pActCopy, &QAction::triggered, _pNppCommands, &NppCommands::editCopy);

    _pActPaste = new QAction(globalIconLoader().loadWithTheme("paste", "edit-paste"), tr("&Paste"), this);
    _pActPaste->setShortcut(QKeySequence::Paste);
    connect(_pActPaste, &QAction::triggered, _pNppCommands, &NppCommands::editPaste);

    _pActUndo = new QAction(globalIconLoader().loadWithTheme("undo", "edit-undo"), tr("&Undo"), this);
    _pActUndo->setShortcut(QKeySequence::Undo);
    connect(_pActUndo, &QAction::triggered, _pNppCommands, &NppCommands::editUndo);

    _pActRedo = new QAction(globalIconLoader().loadWithTheme("redo", "edit-redo"), tr("&Redo"), this);
    _pActRedo->setShortcut(QKeySequence::Redo);
    connect(_pActRedo, &QAction::triggered, _pNppCommands, &NppCommands::editRedo);

    // Edit menu named actions
    _pActDelete = new QAction(tr("&Delete"), this);
    _pActDelete->setShortcut(QKeySequence::Delete);
    connect(_pActDelete, &QAction::triggered, _pNppCommands, &NppCommands::editDelete);

    _pActSelectAll = new QAction(tr("Select &All"), this);
    _pActSelectAll->setShortcut(QKeySequence::SelectAll);
    connect(_pActSelectAll, &QAction::triggered, _pNppCommands, &NppCommands::editSelectAll);

        fprintf(stderr, "DBG createActions: File actions DONE\n"); fflush(stderr);
    // === Search toolbar actions — wired to NppCommands ===
    _pActFind = new QAction(globalIconLoader().loadWithTheme("find", "edit-find"), tr("&Find..."), this);
    _pActFind->setShortcut(QKeySequence::Find);
    connect(_pActFind, &QAction::triggered, _pNppCommands, &NppCommands::searchFind);

    _pActReplace = new QAction(tr("&Replace..."), this);
    _pActReplace->setShortcut(QKeySequence::Replace);
    connect(_pActReplace, &QAction::triggered, _pNppCommands, &NppCommands::searchReplace);

    _pActFindNext = new QAction(tr("Find &Next"), this);
    _pActFindNext->setShortcut(Qt::Key_F3);
    connect(_pActFindNext, &QAction::triggered, _pNppCommands, &NppCommands::searchFindNext);

    _pActFindPrev = new QAction(tr("Find &Previous"), this);
    _pActFindPrev->setShortcut(Qt::SHIFT | Qt::Key_F3);
    connect(_pActFindPrev, &QAction::triggered, _pNppCommands, &NppCommands::searchFindPrev);

    _pActGoToLine = new QAction(tr("&Go to..."), this);
    _pActGoToLine->setShortcut(tr("Ctrl+G"));
    connect(_pActGoToLine, &QAction::triggered, _pNppCommands, &NppCommands::searchGoToLine);

    // === View menu named actions ===
    _pActFullScreen = new QAction(tr("&Full Screen"), this);
    _pActFullScreen->setShortcut(QKeySequence::FullScreen);
    _pActFullScreen->setCheckable(true);
    connect(_pActFullScreen, &QAction::triggered, _pNppCommands, &NppCommands::viewFullScreenToggle);

    _pActWordWrap = new QAction(tr("&Word Wrap"), this);
    _pActWordWrap->setCheckable(true);
    connect(_pActWordWrap, &QAction::triggered, _pNppCommands, &NppCommands::viewWrap);

    _pActZoomIn = new QAction(tr("Zoom &In"), this);
    _pActZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(_pActZoomIn, &QAction::triggered, _pNppCommands, &NppCommands::viewZoomIn);

    _pActZoomOut = new QAction(tr("Zoom &Out"), this);
    _pActZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(_pActZoomOut, &QAction::triggered, _pNppCommands, &NppCommands::viewZoomOut);

    _pActZoomRestore = new QAction(tr("Restore Default Zoom"), this);
    _pActZoomRestore->setShortcut(tr("Ctrl+0"));
    connect(_pActZoomRestore, &QAction::triggered, _pNppCommands, &NppCommands::viewZoomRestore);

    _pActShowEol = new QAction(tr("Show &EOL"), this);
    _pActShowEol->setCheckable(true);
    connect(_pActShowEol, &QAction::triggered, _pNppCommands, &NppCommands::viewEol);

    _pActShowIndentGuide = new QAction(tr("Show &Indent Guide"), this);
    _pActShowIndentGuide->setCheckable(true);
    connect(_pActShowIndentGuide, &QAction::triggered, _pNppCommands, &NppCommands::viewIndentGuide);

    _pActShowAllChars = new QAction(tr("Show &All Characters"), this);
    _pActShowAllChars->setCheckable(true);
    connect(_pActShowAllChars, &QAction::triggered, _pNppCommands, &NppCommands::viewAllCharacters);

    // === Encoding menu named actions ===
    _pActEncAnsi = new QAction(tr("ANSI (Windows-1252)"), this);
    connect(_pActEncAnsi, &QAction::triggered, _pNppCommands, &NppCommands::encodingAnsi);

    _pActEncUTF8 = new QAction(tr("UTF-8"), this);
    connect(_pActEncUTF8, &QAction::triggered, _pNppCommands, &NppCommands::encodingUTF8);

    _pActEncUTF8BOM = new QAction(tr("UTF-8 with BOM"), this);
    connect(_pActEncUTF8BOM, &QAction::triggered, _pNppCommands, &NppCommands::encodingUTF8BOM);

    _pActEncUTF16LE = new QAction(tr("UTF-16 LE"), this);
    connect(_pActEncUTF16LE, &QAction::triggered, _pNppCommands, &NppCommands::encodingUTF16LE);

    _pActEncUTF16BE = new QAction(tr("UTF-16 BE"), this);
    connect(_pActEncUTF16BE, &QAction::triggered, _pNppCommands, &NppCommands::encodingUTF16BE);

    _pActEncConvAnsi = new QAction(tr("Convert to ANSI"), this);
    connect(_pActEncConvAnsi, &QAction::triggered, _pNppCommands, &NppCommands::encodingConvertAnsi);

    _pActEncConvUTF8 = new QAction(tr("Convert to UTF-8"), this);
    connect(_pActEncConvUTF8, &QAction::triggered, _pNppCommands, &NppCommands::encodingConvertUTF8);

    _pActEncConvUTF8BOM = new QAction(tr("Convert to UTF-8 with BOM"), this);
    connect(_pActEncConvUTF8BOM, &QAction::triggered, _pNppCommands, &NppCommands::encodingConvertUTF8BOM);

    _pActEncConvUTF16LE = new QAction(tr("Convert to UTF-16 LE"), this);
    connect(_pActEncConvUTF16LE, &QAction::triggered, _pNppCommands, &NppCommands::encodingConvertUTF16LE);

    _pActEncConvUTF16BE = new QAction(tr("Convert to UTF-16 BE"), this);
    connect(_pActEncConvUTF16BE, &QAction::triggered, _pNppCommands, &NppCommands::encodingConvertUTF16BE);

    // === Settings menu named actions ===
    _pActPreferences = new QAction(tr("&Preferences..."), this);
    connect(_pActPreferences, &QAction::triggered, _pNppCommands, &NppCommands::settingsPreference);

    _pActStyleConfig = new QAction(tr("&Style Configurator..."), this);
    connect(_pActStyleConfig, &QAction::triggered, _pNppCommands, &NppCommands::settingsStyleConfig);

    _pActShortcutMapper = new QAction(tr("&Shortcut Mapper..."), this);
    connect(_pActShortcutMapper, &QAction::triggered, _pNppCommands, &NppCommands::settingsShortcutMapper);

    // === Macro menu named actions ===
    _pActMacroStartRecord = new QAction(tr("&Start Recording"), this);
    connect(_pActMacroStartRecord, &QAction::triggered, _pNppCommands, &NppCommands::macroStartRecording);

    _pActMacroStopRecord = new QAction(tr("Stop &Recording"), this);
    connect(_pActMacroStopRecord, &QAction::triggered, _pNppCommands, &NppCommands::macroStopRecording);

    _pActMacroPlay = new QAction(tr("&Playback"), this);
    _pActMacroPlay->setShortcut(tr("Ctrl+Shift+P"));
    connect(_pActMacroPlay, &QAction::triggered, _pNppCommands, &NppCommands::macroPlayback);

    // === Run menu named actions ===
    _pActRunExec = new QAction(tr("&Execute..."), this);
    _pActRunExec->setShortcut(tr("F5"));
    connect(_pActRunExec, &QAction::triggered, _pNppCommands, &NppCommands::runExecute);

    _pActRunCmdLineArgs = new QAction(tr("&Command Line Arguments..."), this);
    connect(_pActRunCmdLineArgs, &QAction::triggered, _pNppCommands, &NppCommands::runCmdLineArgs);

    // === File menu - missing actions ===
    _pActSaveAll = new QAction(tr("Save All"), this);
    _pActSaveAll->setShortcut(tr("Ctrl+Alt+S"));
    connect(_pActSaveAll, &QAction::triggered, _pNppCommands, &NppCommands::fileSaveAll);

    _pActSaveCopyAs = new QAction(tr("Save Copy As..."), this);
    connect(_pActSaveCopyAs, &QAction::triggered, _pNppCommands, &NppCommands::fileSaveCopyAs);

    _pActOpenFolder = new QAction(tr("Open Folder..."), this);
    connect(_pActOpenFolder, &QAction::triggered, _pNppCommands, &NppCommands::fileOpenFolder);

    _pActOpenFolderAsWorkspace = new QAction(tr("Open Folder as Workspace..."), this);
    connect(_pActOpenFolderAsWorkspace, &QAction::triggered, _pNppCommands, &NppCommands::fileOpenFolderAsWorkspace);

    _pActOpenContainingFolder = new QAction(tr("Open Containing Folder"), this);
    connect(_pActOpenContainingFolder, &QAction::triggered, _pNppCommands, &NppCommands::fileOpenContainingFolder);

    _pActOpenInCmd = new QAction(tr("Open in Command Prompt"), this);
    connect(_pActOpenInCmd, &QAction::triggered, _pNppCommands, &NppCommands::fileOpenInCommandPrompt);

    _pActOpenReadOnly = new QAction(tr("Open as Read-Only"), this);
    connect(_pActOpenReadOnly, &QAction::triggered, _pNppCommands, &NppCommands::fileOpenAsReadOnly);

    _pActRename = new QAction(tr("Rename..."), this);
    connect(_pActRename, &QAction::triggered, _pNppCommands, &NppCommands::fileRename);

    _pActDelete_2 = new QAction(tr("Delete"), this);
    connect(_pActDelete_2, &QAction::triggered, _pNppCommands, &NppCommands::fileDelete);

    _pActCloseLeft = new QAction(tr("Close All to Left"), this);
    connect(_pActCloseLeft, &QAction::triggered, _pNppCommands, &NppCommands::fileCloseAllToLeft);

    _pActCloseRight = new QAction(tr("Close All to Right"), this);
    connect(_pActCloseRight, &QAction::triggered, _pNppCommands, &NppCommands::fileCloseAllToRight);

    _pActCloseUnchanged = new QAction(tr("Close All Unchanged"), this);
    connect(_pActCloseUnchanged, &QAction::triggered, _pNppCommands, &NppCommands::fileCloseAllUnchanged);

    _pActRestoreLastClosed = new QAction(tr("Restore Last Closed"), this);
    connect(_pActRestoreLastClosed, &QAction::triggered, _pNppCommands, &NppCommands::fileRestoreLastClosed);

    _pActLoadSession = new QAction(tr("Load Session..."), this);
    connect(_pActLoadSession, &QAction::triggered, _pNppCommands, &NppCommands::fileLoadSession);

    _pActSaveSession = new QAction(tr("Save Session..."), this);
    connect(_pActSaveSession, &QAction::triggered, _pNppCommands, &NppCommands::fileSaveSession);

    // === Edit menu - missing actions ===
    _pActToggleReadOnly = new QAction(tr("Toggle Read-Only"), this);
    _pActToggleReadOnly->setCheckable(true);
    connect(_pActToggleReadOnly, &QAction::triggered, _pNppCommands, &NppCommands::toggleReadOnly);

    // === View menu - missing actions ===
    _pActViewTabSpace = new QAction(tr("Show Tab Space"), this);
    _pActViewTabSpace->setCheckable(true);
    connect(_pActViewTabSpace, &QAction::triggered, _pNppCommands, &NppCommands::viewTabSpace);

    _pActViewNpc = new QAction(tr("Show NPC"), this);
    _pActViewNpc->setCheckable(true);
    connect(_pActViewNpc, &QAction::triggered, _pNppCommands, &NppCommands::viewNpc);

    _pActViewNpcCcuniEol = new QAction(tr("Show Npc + CC + Uni + EOL"), this);
    _pActViewNpcCcuniEol->setCheckable(true);
    connect(_pActViewNpcCcuniEol, &QAction::triggered, _pNppCommands, &NppCommands::viewNpcCcuniEol);

    _pActViewWrapSymbol = new QAction(tr("Wrap Symbol"), this);
    _pActViewWrapSymbol->setCheckable(true);
    connect(_pActViewWrapSymbol, &QAction::triggered, _pNppCommands, &NppCommands::viewWrapSymbol);

    _pActViewSummary = new QAction(tr("Summary"), this);
    connect(_pActViewSummary, &QAction::triggered, _pNppCommands, &NppCommands::viewSummary);

    _pActViewHideLines = new QAction(tr("Hide Lines"), this);
    connect(_pActViewHideLines, &QAction::triggered, _pNppCommands, &NppCommands::viewHideLines);

    // === Macro menu - missing actions ===
    _pActMacroStartStop = new QAction(tr("Start/Stop Recording"), this);
    _pActMacroStartStop->setShortcut(tr("Ctrl+Shift+R"));
    connect(_pActMacroStartStop, &QAction::triggered, _pNppCommands, &NppCommands::macroStartStopRecord);

    _pActMacroTogglePlayback = new QAction(tr("Playback"), this);
    connect(_pActMacroTogglePlayback, &QAction::triggered, _pNppCommands, &NppCommands::macroPlayback);

    // === Window menu - numbered document actions ===
    _pActWindowDoc1 = new QAction(tr("Document 1"), this);
    connect(_pActWindowDoc1, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(0); });
    _pActWindowDoc2 = new QAction(tr("Document 2"), this);
    connect(_pActWindowDoc2, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(1); });
    _pActWindowDoc3 = new QAction(tr("Document 3"), this);
    connect(_pActWindowDoc3, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(2); });
    _pActWindowDoc4 = new QAction(tr("Document 4"), this);
    connect(_pActWindowDoc4, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(3); });
    _pActWindowDoc5 = new QAction(tr("Document 5"), this);
    connect(_pActWindowDoc5, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(4); });
    _pActWindowDoc6 = new QAction(tr("Document 6"), this);
    connect(_pActWindowDoc6, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(5); });
    _pActWindowDoc7 = new QAction(tr("Document 7"), this);
    connect(_pActWindowDoc7, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(6); });
    _pActWindowDoc8 = new QAction(tr("Document 8"), this);
    connect(_pActWindowDoc8, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(7); });
    _pActWindowDoc9 = new QAction(tr("Document 9"), this);
    connect(_pActWindowDoc9, &QAction::triggered, [this]() { _pNppCommands->windowSwitchToDocument(8); });

    _pActWindowDocNext = new QAction(tr("Next Document"), this);
    _pActWindowDocNext->setShortcut(tr("Ctrl+Tab"));
    connect(_pActWindowDocNext, &QAction::triggered, [this]() {
        if (_pDocTabBar && _pDocTabBar->count() > 0) {
            int next = (_pDocTabBar->currentIndex() + 1) % _pDocTabBar->count();
            _pDocTabBar->setCurrentIndex(next);
        }
    });

    _pActWindowDocPrev = new QAction(tr("Previous Document"), this);
    _pActWindowDocPrev->setShortcut(tr("Ctrl+Shift+Tab"));
    connect(_pActWindowDocPrev, &QAction::triggered, [this]() {
        if (_pDocTabBar && _pDocTabBar->count() > 0) {
            int prev = (_pDocTabBar->currentIndex() - 1 + _pDocTabBar->count()) % _pDocTabBar->count();
            _pDocTabBar->setCurrentIndex(prev);
        }
    });

    _pActWindowMoveToNewInstance = new QAction(tr("Move to New Instance"), this);
    connect(_pActWindowMoveToNewInstance, &QAction::triggered, _pNppCommands, &NppCommands::viewGotoNewInstance);

    _pActWindowLoadInNewInstance = new QAction(tr("Load in New Instance"), this);
    connect(_pActWindowLoadInNewInstance, &QAction::triggered, _pNppCommands, &NppCommands::viewLoadInNewInstance);

    fprintf(stderr, "DBG createActions: END\n"); fflush(stderr);
}

// === Global Keyboard Shortcuts (supplement actions set via QAction::setShortcut) ===
void MainWindow::createShortcuts()
{
    // NOTE: Ctrl+Tab / Ctrl+Shift+Tab / Ctrl+Alt+S are already set on
    // _pActWindowDocNext / _pActWindowDocPrev / _pActSaveAll actions.

    // Ctrl+PageUp / Ctrl+PageDown — MRU tab cycling (Notepad++ convention)
    // These are distinct from Ctrl+Tab (sequential cycle) so kept separate.
    auto* shortcutPageUp = new QShortcut(QKeySequence(tr("Ctrl+PageUp")), this);
    connect(shortcutPageUp, &QShortcut::activated, this, [this]() {
        int count = _pDocTabBar->count();
        if (count < 2)
            return;
        int prev = _pDocTabBar->currentIndex() - 1;
        if (prev < 0)
            prev = count - 1;
        _pDocTabBar->setCurrentIndex(prev);
    });

    auto* shortcutPageDown = new QShortcut(QKeySequence(tr("Ctrl+PageDown")), this);
    connect(shortcutPageDown, &QShortcut::activated, this, [this]() {
        int count = _pDocTabBar->count();
        if (count < 2)
            return;
        int next = (_pDocTabBar->currentIndex() + 1) % count;
        _pDocTabBar->setCurrentIndex(next);
    });

    // Ctrl+Shift+R — Macro Start/Stop Recording toggle
    auto* shortcutMacroRecord = new QShortcut(QKeySequence(tr("Ctrl+Shift+R")), this);
    connect(shortcutMacroRecord, &QShortcut::activated, _pNppCommands, &NppCommands::macroStartStopRecord);

    // Ctrl+U — Uppercase (Notepad++: Ctrl+U for upper, Ctrl+Shift+U for lower)
    // No accelerator on the Case menu items, so wire them as global shortcuts.
    auto* shortcutUpper = new QShortcut(QKeySequence(tr("Ctrl+U")), this);
    connect(shortcutUpper, &QShortcut::activated, _pNppCommands, &NppCommands::editUpperCase);

    // Ctrl+Shift+U — Lowercase
    auto* shortcutLower = new QShortcut(QKeySequence(tr("Ctrl+Shift+U")), this);
    connect(shortcutLower, &QShortcut::activated, _pNppCommands, &NppCommands::editLowerCase);

    // Escape — close find/replace dialog if visible, or exit full-screen
    auto* shortcutEscape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(shortcutEscape, &QShortcut::activated, this, [this]() {
        // Close find/replace dialog if it's open
        if (_pFindReplaceDlg && _pFindReplaceDlg->isVisible()) {
            _pFindReplaceDlg->hide();
            return;
        }
        // Exit full-screen / Distraction-Free mode if active
        if (_isFullScreen) {
            toggleFullScreen();
        }
    });
}

void MainWindow::createMenus()
{
    _pMenuBar = menuBar();

    // === File menu ===
    QMenu* fileMenu = _pMenuBar->addMenu(tr("&File"));
    fileMenu->addAction(_pActNew);
    fileMenu->addAction(_pActOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(_pActSave);
    fileMenu->addAction(_pActSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(_pActReload);
    fileMenu->addAction(_pActClose);
    fileMenu->addAction(_pActCloseAll);
    fileMenu->addAction(_pActCloseAllButCurrent);
    fileMenu->addSeparator();
    fileMenu->addAction(_pActPrint);
    fileMenu->addSeparator();
    // File > Other Operations submenu
    QMenu* fileOpsMenu = fileMenu->addMenu(tr("Other Operations"));
    fileOpsMenu->addAction(_pActOpenFolder);
    fileOpsMenu->addAction(_pActOpenFolderAsWorkspace);
    fileOpsMenu->addAction(_pActOpenContainingFolder);
    fileOpsMenu->addAction(_pActOpenInCmd);
    fileOpsMenu->addAction(_pActOpenReadOnly);
    fileOpsMenu->addSeparator();
    fileOpsMenu->addAction(_pActSaveAll);
    fileOpsMenu->addAction(_pActSaveCopyAs);
    fileOpsMenu->addAction(_pActRename);
    fileOpsMenu->addAction(_pActDelete_2);
    fileOpsMenu->addSeparator();
    fileOpsMenu->addAction(_pActCloseLeft);
    fileOpsMenu->addAction(_pActCloseRight);
    fileOpsMenu->addAction(_pActCloseUnchanged);
    fileOpsMenu->addSeparator();
    fileOpsMenu->addAction(_pActRestoreLastClosed);
    fileOpsMenu->addSeparator();
    fileOpsMenu->addAction(_pActLoadSession);
    fileOpsMenu->addAction(_pActSaveSession);
    fileMenu->addSeparator();
    fileMenu->addAction(_pActExit);

    // === Edit menu ===
    QMenu* editMenu = _pMenuBar->addMenu(tr("&Edit"));
    editMenu->addAction(_pActUndo);
    editMenu->addAction(_pActRedo);
    editMenu->addSeparator();
    editMenu->addAction(_pActCut);
    editMenu->addAction(_pActCopy);
    editMenu->addAction(_pActPaste);
    editMenu->addSeparator();
    editMenu->addAction(_pActDelete);
    editMenu->addAction(_pActSelectAll);
    editMenu->addSeparator();
    editMenu->addAction(_pActToggleReadOnly);

    // Edit > Case submenu
    QMenu* caseMenu = editMenu->addMenu(tr("&Case"));
    caseMenu->addAction(tr("&Uppercase"), _pNppCommands, &NppCommands::editUpperCase);
    caseMenu->addAction(tr("&Lowercase"), _pNppCommands, &NppCommands::editLowerCase);
    caseMenu->addAction(tr("Proper &Case"), _pNppCommands, &NppCommands::editProperCase);
    caseMenu->addAction(tr("Sentence &case"), _pNppCommands, &NppCommands::editSentenceCase);
    caseMenu->addAction(tr("&Invert case"), _pNppCommands, &NppCommands::editInvertCase);
    caseMenu->addAction(tr("R&andom case"), _pNppCommands, &NppCommands::editRandomCase);

    // Edit > Line operations submenu
    QMenu* lineMenu = editMenu->addMenu(tr("&Line Operations"));
    lineMenu->addAction(tr("&Split Lines"), _pNppCommands, &NppCommands::editSplitLines);
    lineMenu->addAction(tr("&Join Lines"), _pNppCommands, &NppCommands::editJoinLines);
    lineMenu->addSeparator();
    lineMenu->addAction(tr("Remove &Empty Lines"), _pNppCommands, &NppCommands::editRemoveEmptyLines);
    lineMenu->addAction(tr("Remove Empty Lines with &Blank"), _pNppCommands, &NppCommands::editRemoveEmptyLinesWithBlank);
    lineMenu->addAction(tr("Blank Line &Above"), _pNppCommands, &NppCommands::editBlankLineAbove);
    lineMenu->addAction(tr("Blank Line &Below"), _pNppCommands, &NppCommands::editBlankLineBelow);

    // Edit > Sort Lines submenu
    QMenu* sortMenu = editMenu->addMenu(tr("Sort Lines"));
    sortMenu->addAction(tr("&Ascending (Lexicographically)"),
        _pNppCommands, &NppCommands::editSortLexAsc);
    sortMenu->addAction(tr("&Descending (Lexicographically)"),
        _pNppCommands, &NppCommands::editSortLexDesc);
    sortMenu->addAction(tr("&Integer Ascending"),
        _pNppCommands, &NppCommands::editSortIntAsc);
    sortMenu->addAction(tr("Integer &Descending"),
        _pNppCommands, &NppCommands::editSortIntDesc);
    sortMenu->addAction(tr("Re&verse"),
        _pNppCommands, &NppCommands::editSortReversed);

    // Edit > Trim submenu
    QMenu* trimMenu = editMenu->addMenu(tr("&Trim"));
    trimMenu->addAction(tr("Trim &Trailing Space"), _pNppCommands, &NppCommands::editTrimTrailing);
    trimMenu->addAction(tr("Trim &Leading Space"), _pNppCommands, &NppCommands::editTrimLeading);
    trimMenu->addAction(tr("Trim &Both Ends"), _pNppCommands, &NppCommands::editTrimBoth);
    trimMenu->addAction(tr("Trim A&ll"), _pNppCommands, &NppCommands::editTrimAll);

    // Edit > Convert submenu (tabs/spaces)
    QMenu* convMenu = editMenu->addMenu(tr("&Convert"));
    convMenu->addAction(tr("Tab to &Space"), _pNppCommands, &NppCommands::editTabToSpace);
    convMenu->addAction(tr("&Space to Tab (All)"), _pNppCommands, &NppCommands::editSpaceToTabAll);
    convMenu->addAction(tr("Space to Tab (&Leading)"), _pNppCommands, &NppCommands::editSpaceToTabLeading);

    // Edit > Comment submenu
    QMenu* commentMenu = editMenu->addMenu(tr("&Comment"));
    commentMenu->addAction(tr("Block Comment"), _pNppCommands, &NppCommands::editBlockComment);
    commentMenu->addAction(tr("Block Uncomment"), _pNppCommands, &NppCommands::editBlockUncomment);
    commentMenu->addAction(tr("Stream Comment"), _pNppCommands, &NppCommands::editStreamComment);
    commentMenu->addAction(tr("Stream Uncomment"), _pNppCommands, &NppCommands::editStreamUncomment);

    // Edit > Copy special submenu
    QMenu* copySpecMenu = editMenu->addMenu(tr("Copy to &Clipboard"));
    copySpecMenu->addAction(tr("Full File &Path"), _pNppCommands, &NppCommands::editFullPathToClip);
    copySpecMenu->addAction(tr("File &Name"), _pNppCommands, &NppCommands::editFileNameToClip);
    copySpecMenu->addAction(tr("Current &Directory"), _pNppCommands, &NppCommands::editCurrentDirToClip);
    copySpecMenu->addSeparator();
    copySpecMenu->addAction(tr("All &Names"), _pNppCommands, &NppCommands::editCopyAllNames);
    copySpecMenu->addAction(tr("All &Paths"), _pNppCommands, &NppCommands::editCopyAllPaths);

    // === Search menu ===
    QMenu* searchMenu = _pMenuBar->addMenu(tr("&Search"));
    searchMenu->addAction(_pActFind);
    searchMenu->addAction(_pActReplace);
    searchMenu->addSeparator();
    searchMenu->addAction(_pActFindNext);
    searchMenu->addAction(_pActFindPrev);
    searchMenu->addAction(_pActGoToLine);
    searchMenu->addSeparator();
    searchMenu->addAction(tr("Find &in Files..."), _pNppCommands, &NppCommands::searchFindInFiles);
    searchMenu->addAction(tr("Find &in Projects..."), _pNppCommands, &NppCommands::searchFindInProjects);

    // Search > Mark submenu
    QMenu* markMenu = searchMenu->addMenu(tr("&Mark"));
    markMenu->addAction(tr("&Toggle Bookmark"), _pNppCommands, &NppCommands::searchToggleBookmark);
    markMenu->addAction(tr("&Next Bookmark"), _pNppCommands, &NppCommands::searchNextBookmark);
    markMenu->addAction(tr("Pre&vious Bookmark"), _pNppCommands, &NppCommands::searchPrevBookmark);
    markMenu->addSeparator();
    markMenu->addAction(tr("Clear All &Bookmarks"), _pNppCommands, &NppCommands::searchClearBookmarks);
    markMenu->addAction(tr("Clear All &Marks"), _pNppCommands, &NppCommands::searchClearAllMarks);

    // === View menu ===
    QMenu* viewMenu = _pMenuBar->addMenu(tr("&View"));
    viewMenu->addAction(_pActFullScreen);
    viewMenu->addAction(_pActWordWrap);
    viewMenu->addSeparator();
    viewMenu->addAction(_pActZoomIn);
    viewMenu->addAction(_pActZoomOut);
    viewMenu->addAction(_pActZoomRestore);
    viewMenu->addSeparator();
    viewMenu->addAction(_pActShowAllChars);
    viewMenu->addAction(_pActShowEol);
    viewMenu->addAction(_pActShowIndentGuide);
    // View > Symbol submenu
    QMenu* symbolMenu = viewMenu->addMenu(tr("&Symbol"));
    symbolMenu->addAction(_pActViewTabSpace);
    symbolMenu->addAction(_pActViewNpc);
    symbolMenu->addAction(_pActViewNpcCcuniEol);
    symbolMenu->addAction(_pActViewWrapSymbol);
    viewMenu->addSeparator();
    viewMenu->addAction(_pActViewSummary);
    viewMenu->addAction(_pActViewHideLines);
    viewMenu->addSeparator();
    viewMenu->addAction(tr("Always on &Top"), _pNppCommands, &NppCommands::viewAlwaysOnTop);
    viewMenu->addAction(tr("&Post It"), _pNppCommands, &NppCommands::viewPostIt);
    viewMenu->addAction(tr("&Distraction Free"), _pNppCommands, &NppCommands::viewDistractionFree);
    viewMenu->addSeparator();
    viewMenu->addAction(tr("Switch to &Other View"), _pNppCommands, &NppCommands::viewSwitchToOtherView);
    viewMenu->addAction(tr("&Clone to Other View"), _pNppCommands, &NppCommands::viewCloneToAnotherView);

    // View > Document List submenu
    QMenu* docListMenu = viewMenu->addMenu(tr("Document List"));
    docListMenu->addAction(tr("Sort &Ascending"), _pNppCommands, &NppCommands::windowSortAscending);
    docListMenu->addAction(tr("Sort &Descending"), _pNppCommands, &NppCommands::windowSortDescending);

    // View > Panels submenu
    QMenu* panelsMenu = viewMenu->addMenu(tr("&Panels"));
    panelsMenu->addAction(tr("&Document Map"), _pNppCommands, &NppCommands::toggleDocumentMap);
    panelsMenu->addAction(tr("&Function List"), _pNppCommands, &NppCommands::toggleFunctionList);
    panelsMenu->addAction(tr("&File Browser"), _pNppCommands, &NppCommands::toggleFileBrowser);
    panelsMenu->addAction(tr("&Project Panel 1"), _pNppCommands, &NppCommands::toggleProjectPanel1);
    panelsMenu->addAction(tr("Project Panel &2"), _pNppCommands, &NppCommands::toggleProjectPanel2);
    panelsMenu->addAction(tr("Project Panel &3"), _pNppCommands, &NppCommands::toggleProjectPanel3);
    panelsMenu->addAction(tr("&Clipboard History"), _pNppCommands, &NppCommands::toggleClipboardHistory);

    // === Encoding menu ===
    QMenu* encodingMenu = _pMenuBar->addMenu(tr("&Encoding"));
    encodingMenu->addAction(_pActEncUTF8);
    encodingMenu->addAction(_pActEncUTF8BOM);
    encodingMenu->addAction(_pActEncUTF16LE);
    encodingMenu->addAction(_pActEncUTF16BE);
    encodingMenu->addAction(_pActEncAnsi);

    // Encoding > Convert submenu
    QMenu* encConvMenu = encodingMenu->addMenu(tr("&Convert to..."));
    encConvMenu->addAction(_pActEncConvUTF8);
    encConvMenu->addAction(_pActEncConvUTF8BOM);
    encConvMenu->addAction(_pActEncConvUTF16LE);
    encConvMenu->addAction(_pActEncConvUTF16BE);
    encConvMenu->addAction(_pActEncConvAnsi);

    // === Language menu ===
    QMenu* languageMenu = _pMenuBar->addMenu(tr("&Language"));
    _langMenu = languageMenu;

    // Helper lambda: add a language action that calls setLanguage()
    auto addLang = [this, languageMenu](const QString& label, LangType lang) {
        QAction* act = new QAction(label, languageMenu);
        act->setData(QVariant::fromValue(static_cast<int>(lang)));
        languageMenu->addAction(act);
        connect(act, &QAction::triggered, this, [this, act]() { setLanguage(act); });
    };

    // C/C++ family
    addLang("C (C)",      LangType::L_C);
    addLang("C++",        LangType::L_CPP);
    addLang("C#",         LangType::L_CS);
    addLang("Java",       LangType::L_JAVA);

    languageMenu->addSeparator();

    // Web
    addLang("HTML",       LangType::L_HTML);
    addLang("XML",        LangType::L_XML);
    addLang("PHP",        LangType::L_PHP);
    addLang("JavaScript", LangType::L_JAVASCRIPT);
    addLang("TypeScript", LangType::L_TYPESCRIPT);
    addLang("CSS",        LangType::L_CSS);
    addLang("JSON",       LangType::L_JSON);
    addLang("YAML",       LangType::L_YAML);
    addLang("Markdown",   LangType::L_MARKDOWN);

    languageMenu->addSeparator();

    // Scripting
    addLang("Python",     LangType::L_PYTHON);
    addLang("Bash/Shell", LangType::L_BASH);
    addLang("Lua",        LangType::L_LUA);
    addLang("Ruby",       LangType::L_RUBY);
    addLang("Perl",       LangType::L_PERL);

    languageMenu->addSeparator();

    // Other
    addLang("SQL",        LangType::L_SQL);
    addLang("Makefile",   LangType::L_MAKEFILE);
    addLang("INI",        LangType::L_INI);
    addLang("Batch",      LangType::L_BATCH);
    addLang("Diff",       LangType::L_DIFF);

    languageMenu->addSeparator();
    addLang("Normal Text", LangType::L_TEXT);

    languageMenu->addSeparator();
    languageMenu->addAction(tr("&User Defined..."), _pNppCommands, &NppCommands::languageUserDefine);
    languageMenu->addAction(tr("Open &UDL Folder..."), _pNppCommands, &NppCommands::languageOpenUdldDir);

    // === Settings menu ===
    QMenu* settingsMenu = _pMenuBar->addMenu(tr("Se&ttings"));
    settingsMenu->addAction(_pActPreferences);
    settingsMenu->addAction(_pActStyleConfig);
    settingsMenu->addAction(_pActShortcutMapper);
    settingsMenu->addSeparator();
    settingsMenu->addAction(tr("&Import Plugin..."), _pNppCommands, &NppCommands::settingsImportPlugin);
    settingsMenu->addAction(tr("Import &Style Themes..."), _pNppCommands, &NppCommands::settingsImportStyleThemes);
    settingsMenu->addAction(tr("Edit Popup &Context Menu"), _pNppCommands, &NppCommands::settingsEditContextMenu);

    // === Macro menu ===
    QMenu* macroMenu = _pMenuBar->addMenu(tr("&Macro"));
    macroMenu->addAction(_pActMacroStartRecord);
    macroMenu->addAction(_pActMacroStopRecord);
    macroMenu->addSeparator();
    macroMenu->addAction(_pActMacroPlay);
    macroMenu->addAction(tr("&Save Current Macro..."), _pNppCommands, &NppCommands::macroSaveCurrent);
    macroMenu->addAction(tr("&Run Multiple Macros..."), _pNppCommands, &NppCommands::macroRunMultiple);

    // Wire RunMacroDlg::runMacro signal → NppCommands::onRunMacro
    // _runMacroDlg lives in Notepad_plus (which IS this MainWindow instance)
    auto* nppCore = static_cast<Notepad_plus*>(this);
    connect(&nppCore->getRunMacroDlg(), &RunMacroDlg::runMacro,
            _pNppCommands, &NppCommands::onRunMacro);

    // === Run menu ===
    QMenu* runMenu = _pMenuBar->addMenu(tr("&Run"));
    runMenu->addAction(_pActRunExec);
    runMenu->addAction(_pActRunCmdLineArgs);

    // === Window menu ===
    QMenu* windowMenu = _pMenuBar->addMenu(tr("&Window"));
    windowMenu->addAction(tr("Switch to &Other View"), _pNppCommands, &NppCommands::viewSwitchToOtherView);
    windowMenu->addAction(tr("&Clone to Another View"), _pNppCommands, &NppCommands::viewCloneToAnotherView);
    windowMenu->addSeparator();
    windowMenu->addAction(_pActWindowDocNext);
    windowMenu->addAction(_pActWindowDocPrev);
    windowMenu->addSeparator();
    // Window > Document N submenu
    QMenu* docMenu = windowMenu->addMenu(tr("&Document"));
    docMenu->addAction(_pActWindowDoc1);
    docMenu->addAction(_pActWindowDoc2);
    docMenu->addAction(_pActWindowDoc3);
    docMenu->addAction(_pActWindowDoc4);
    docMenu->addAction(_pActWindowDoc5);
    docMenu->addAction(_pActWindowDoc6);
    docMenu->addAction(_pActWindowDoc7);
    docMenu->addAction(_pActWindowDoc8);
    docMenu->addAction(_pActWindowDoc9);
    windowMenu->addSeparator();
    windowMenu->addAction(_pActWindowMoveToNewInstance);
    windowMenu->addAction(_pActWindowLoadInNewInstance);

    // === Help menu ===
    QMenu* helpMenu = _pMenuBar->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About Notepad++"), _pNppCommands, &NppCommands::helpAbout);
    helpMenu->addSeparator();
    helpMenu->addAction(tr("&Documentation"), _pNppCommands, &NppCommands::helpDocumentation);
    helpMenu->addAction(tr("&Homepage"), _pNppCommands, &NppCommands::helpHomeSweetHome);
    helpMenu->addAction(tr("&Online Documentation"), _pNppCommands, &NppCommands::helpOnlineDoc);
}

void MainWindow::createToolBars()
{
    fprintf(stderr, "DBG createToolBars: START\n"); fflush(stderr);
    _pFileToolBar = addToolBar(tr("File"));
    _pFileToolBar->setObjectName("FileToolBar");
    _pFileToolBar->setMovable(false);
    _pFileToolBar->addAction(_pActNew);
    _pFileToolBar->addAction(_pActOpen);
    _pFileToolBar->addAction(_pActSave);

    _pEditToolBar = addToolBar(tr("Edit"));
    _pEditToolBar->setObjectName("EditToolBar");
    _pEditToolBar->setMovable(false);
    _pEditToolBar->addAction(_pActCut);
    _pEditToolBar->addAction(_pActCopy);
    _pEditToolBar->addAction(_pActPaste);
    _pEditToolBar->addSeparator();
    _pEditToolBar->addAction(_pActUndo);
    _pEditToolBar->addAction(_pActRedo);

    _pSearchToolBar = addToolBar(tr("Search"));
    _pSearchToolBar->setObjectName("SearchToolBar");
    _pSearchToolBar->setMovable(false);
    _pSearchToolBar->addAction(_pActFind);
    _pSearchToolBar->addAction(_pActReplace);
    fprintf(stderr, "DBG createToolBars: END\n"); fflush(stderr);
}

void MainWindow::createStatusBar()
{
    _pStatusBar = statusBar();

    _pPositionLabel = new QLabel(tr("Ln 1, Col 1"), this);
    _pStatusBar->addPermanentWidget(_pPositionLabel);

    _pLengthLabel = new QLabel(tr("0 chars"), this);
    _pStatusBar->addPermanentWidget(_pLengthLabel);

    _pEncodingLabel = new QLabel(tr("UTF-8"), this);
    _pStatusBar->addPermanentWidget(_pEncodingLabel);

    _pEolLabel = new QLabel(tr("Windows (CRLF)"), this);
    _pStatusBar->addPermanentWidget(_pEolLabel);

    _pLanguageLabel = new QLabel(tr("Normal text"), this);
    _pStatusBar->addPermanentWidget(_pLanguageLabel);
}

void MainWindow::createTabBar()
{
    // _pTabBar is created in setupUi() and used for the top-level tab bar.
    // Document tabs are managed by _pDocTabBar (Scintilla editor tabs).
    if (_pTabBar)
    {
        _pTabBar->init(this, false, false, 3);
        connect(_pTabBar, &TabBarPlus::tabDelete, this, [this]() { closeFile(); });
    }
}

void MainWindow::createDocumentPane()
{
    fprintf(stderr, "DBG createDocumentPane: line 1 - new QTabWidget\n"); fflush(stderr);
    _pDocTabBar = new QTabWidget(this);
    fprintf(stderr, "DBG createDocumentPane: line 2 - setTabsClosable\n"); fflush(stderr);
    _pDocTabBar->setTabsClosable(true);
    fprintf(stderr, "DBG createDocumentPane: line 3 - setDocumentMode\n"); fflush(stderr);
    _pDocTabBar->setDocumentMode(true);
    fprintf(stderr, "DBG createDocumentPane: line 4 - qobject_cast\n"); fflush(stderr);
    // Qt6: use QTabBar base class public signals
    QTabBar* docTabBar = qobject_cast<QTabBar*>(_pDocTabBar);
    fprintf(stderr, "DBG createDocumentPane: line 5 - connect currentChanged\n"); fflush(stderr);
    connect(docTabBar, &QTabBar::currentChanged, this, &MainWindow::onTabChanged);
    fprintf(stderr, "DBG createDocumentPane: line 6 - connect tabCloseRequested\n"); fflush(stderr);
    connect(docTabBar, &QTabBar::tabCloseRequested, this, [this](int i) { closeFile(i); });
    fprintf(stderr, "DBG createDocumentPane: END\n"); fflush(stderr);
}

void MainWindow::createPanels()
{
    fprintf(stderr, "DBG createPanels: START\n"); fflush(stderr);
    _pFileBrowserDock = new QDockWidget(tr("File Browser"), this);
    _pFileBrowserDock->setObjectName("FileBrowser");
    addDockWidget(Qt::LeftDockWidgetArea, _pFileBrowserDock);

    _pFunctionListDock = new QDockWidget(tr("Function List"), this);
    _pFunctionListDock->setObjectName("FunctionList");
    addDockWidget(Qt::RightDockWidgetArea, _pFunctionListDock);

    _pDocMapDock = new QDockWidget(tr("Document Map"), this);
    _pDocMapDock->setObjectName("DocumentMap");
    addDockWidget(Qt::RightDockWidgetArea, _pDocMapDock);

    _pProjectPanelDock = new QDockWidget(tr("Project"), this);
    _pProjectPanelDock->setObjectName("ProjectPanel");
    addDockWidget(Qt::LeftDockWidgetArea, _pProjectPanelDock);

    _pFileBrowserDock->hide();
    _pFunctionListDock->hide();
    _pDocMapDock->hide();
    _pProjectPanelDock->hide();
    fprintf(stderr, "DBG createPanels: END\n"); fflush(stderr);
}

void MainWindow::newFile()
{
    Buffer* buf = createNewBuffer();
    // Create a Scintilla editor widget for this document
    ScintillaComponent* editor = new ScintillaComponent(this);
    editor->init(this);
    // Show line number gutter by default
    editor->showMargin(SC_MARGE_LINENUMBER, true);
    int index = _pDocTabBar->addTab(editor, "Untitled");
    _pDocTabBar->setCurrentIndex(index);
    _scintillaEditors.append(editor);
    _pCurrentBuffer = buf;

    // Plain text by default for new files
    editor->setLexerLanguage(static_cast<int>(LangType::L_TEXT));

    // Connect cursor movement to status bar update
    connect(editor, &QsciScintilla::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    // Connect text modifications to status bar (length may change)
    connect(editor, &QsciScintilla::textChanged, this, &MainWindow::updateStatusBar);

    updateStatusBar();
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::homePath(),
        tr("All Files (*.*);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));
    if (!filePath.isEmpty()) {
        openFile(filePath);
    }
}

void MainWindow::openFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open file: %1").arg(path));
        return;
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    Buffer* buf = createNewBuffer();
    buf->setContent(content);
    buf->setFilePath(path);

    // Detect language from file extension and apply lexer
    QFileInfo fi(path);
    std::wstring ext = L"." + fi.suffix().toLower().toStdWString();
    LangType detectedLang = Buffer::langFromExtension(ext);
    buf->setLangType(detectedLang);
    buf->langHasBeenSetFromMenu();

    ScintillaComponent* editor = new ScintillaComponent(this);
    editor->init(this);
    // Show line number gutter by default
    editor->showMargin(SC_MARGE_LINENUMBER, true);
    editor->setText(content.toUtf8());
    editor->setLexerLanguage(static_cast<int>(detectedLang));
    int index = _pDocTabBar->addTab(editor, fi.fileName());
    _pDocTabBar->setCurrentIndex(index);
    _scintillaEditors.append(editor);
    _pCurrentBuffer = buf;

    // Connect cursor movement to status bar update
    connect(editor, &QsciScintilla::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    // Connect text modifications to status bar (length may change)
    connect(editor, &QsciScintilla::textChanged, this, &MainWindow::updateStatusBar);

    updateStatusBar();
}

void MainWindow::saveFile()
{
    if (!_pCurrentBuffer) return;
    if (_pCurrentBuffer->filePath().isEmpty()) {
        saveFileAs();
        return;
    }
    ScintillaComponent* editor = currentScintilla();
    QString text = editor ? editor->text() : "";
    QFile file(_pCurrentBuffer->filePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << text;
        file.close();
        _pCurrentBuffer->setDirty(false);
    }
}

void MainWindow::saveFileAs()
{
    if (!_pCurrentBuffer) return;
    QString filePath = QFileDialog::getSaveFileName(
        this, tr("Save File As"), QDir::homePath(),
        tr("All Files (*.*);;Text Files (*.txt)"));
    if (!filePath.isEmpty()) {
        _pCurrentBuffer->setFilePath(filePath);
        saveFile();
    }
}

void MainWindow::closeFile()
{
    closeFile(_pDocTabBar ? _pDocTabBar->currentIndex() : -1);
}

void MainWindow::closeFile(int index)
{
    if (index < 0) index = _pDocTabBar ? _pDocTabBar->currentIndex() : -1;
    if (index < 0 || !_pDocTabBar) return;

    if (index < _scintillaEditors.size()) {
        delete _scintillaEditors[index];
        _scintillaEditors.remove(index);
    }
    if (index < _buffers.size()) {
        delete _buffers[index];
        _buffers.remove(index);
    }
    _pDocTabBar->removeTab(index);
}

void MainWindow::exit() { close(); }

void MainWindow::undo()
{
    if (auto* ed = currentScintilla()) ed->undo();
}

void MainWindow::redo()
{
    if (auto* ed = currentScintilla()) ed->redo();
}

void MainWindow::cut()
{
    if (auto* ed = currentScintilla()) ed->cut();
}

void MainWindow::copy()
{
    if (auto* ed = currentScintilla()) ed->copy();
}

void MainWindow::paste()
{
    if (auto* ed = currentScintilla()) ed->paste();
}

void MainWindow::selectAll()
{
    if (auto* ed = currentScintilla()) ed->selectAll();
}

void MainWindow::find()
{
    if (_pFindReplaceDlg) {
        _pFindReplaceDlg->setCurrentEditor(currentScintilla());
        _pFindReplaceDlg->doDialog(FIND_DLG);
    }
}

void MainWindow::replace()
{
    if (_pFindReplaceDlg) {
        _pFindReplaceDlg->setCurrentEditor(currentScintilla());
        _pFindReplaceDlg->doDialog(REPLACE_DLG);
    }
}

void MainWindow::toggleFullScreen()
{
    if (_isFullScreen) {
        showNormal();
    } else {
        showFullScreen();
    }
    _isFullScreen = !_isFullScreen;
}

// toggleWordWrap — toggles line wrapping in the current editor tab
void MainWindow::toggleWordWrap()
{
    if (auto* ed = currentScintilla()) ed->wrapText(!ed->isWrap());
}

// zoomIn / zoomOut / resetZoom — adjust editor zoom factor
// NppCommands::viewZoomIn/Out/Restore handle the actual SCI_ZOOM command
void MainWindow::zoomIn() { _currentZoomFactor += 10; }
void MainWindow::zoomOut() { _currentZoomFactor -= 10; }
void MainWindow::resetZoom() { _currentZoomFactor = 100; }

void MainWindow::onTabChanged(int index)
{
    if (index >= 0 && index < _buffers.size())
        _pCurrentBuffer = _buffers[index];
    else
        _pCurrentBuffer = nullptr;
    // Sync current editor to FindReplaceDlg so search works on active tab
    if (_pFindReplaceDlg)
        _pFindReplaceDlg->setCurrentEditor(currentScintilla());
    updateStatusBar();
    emit documentChanged(index);
}

void MainWindow::updateStatusBar()
{
    if (!_pStatusBar) return;
    ScintillaComponent* ed = currentScintilla();
    if (!ed) return;

    // Use QsciScintilla's cursor position helpers (1-indexed line, 1-indexed column)
    int line = 0, col = 0;
    ed->getCursorPosition(&line, &col);
    if (_pPositionLabel) _pPositionLabel->setText(QString("Ln %1, Col %2").arg(line + 1).arg(col + 1));

    // Document length
    if (_pLengthLabel) _pLengthLabel->setText(QString("Len %1").arg(ed->send(SCI_GETLENGTH)));

    // Encoding
    if (_pEncodingLabel) _pEncodingLabel->setText("UTF-8");

    // EOL mode
    int eolMode = ed->eolMode();
    QString eolStr = (eolMode == npp_sci::SC_EOL_CRLF) ? "Windows (CRLF)"
                      : (eolMode == npp_sci::SC_EOL_LF) ? "Unix (LF)"
                      : (eolMode == npp_sci::SC_EOL_CR) ? "Mac (CR)" : "Windows (CRLF)";
    if (_pEolLabel) _pEolLabel->setText(eolStr);

    // Language
    QString langStr = "Normal text";
    if (_pCurrentBuffer) {
        switch (_pCurrentBuffer->getLangType()) {
            case LangType::L_CPP:     langStr = "C++"; break;
            case LangType::L_JAVA:    langStr = "Java"; break;
            case LangType::L_CS:      langStr = "C#"; break;
            case LangType::L_HTML:    langStr = "HTML"; break;
            case LangType::L_XML:     langStr = "XML"; break;
            case LangType::L_PYTHON:  langStr = "Python"; break;
            case LangType::L_JSON:    langStr = "JSON"; break;
            case LangType::L_MAKEFILE:langStr = "Makefile"; break;
            case LangType::L_INI:     langStr = "INI"; break;
            case LangType::L_BATCH:   langStr = "Batch"; break;
            case LangType::L_JAVASCRIPT: langStr = "JavaScript"; break;
            case LangType::L_PHP:     langStr = "PHP"; break;
            case LangType::L_SQL:     langStr = "SQL"; break;
            case LangType::L_RC:      langStr = "Resource"; break;
            case LangType::L_TEXT:    langStr = "Normal text"; break;
            default:                  langStr = "Normal text"; break;
        }
    }
    if (_pLanguageLabel) _pLanguageLabel->setText(langStr);
}

void MainWindow::updateTabBar() {}

void MainWindow::setLanguage(QAction* action)
{
    if (!action) return;
    bool ok = false;
    int langVal = action->data().toInt(&ok);
    if (ok)
        setLanguage(static_cast<LangType>(langVal));
}

void MainWindow::setLanguage(LangType lang)
{
    ScintillaComponent* ed = currentScintilla();
    if (!ed) return;
    ed->setLexerLanguage(static_cast<int>(lang));
    if (_pCurrentBuffer) {
        _pCurrentBuffer->setLangType(lang);
        _pCurrentBuffer->langHasBeenSetFromMenu();
    }
    updateStatusBar();
}

Buffer* MainWindow::createNewBuffer()
{
    Buffer* buf = new Buffer();
    _buffers.append(buf);
    return buf;
}

Buffer* MainWindow::getBufferFromIndex(int index) const
{
    if (index >= 0 && index < _buffers.size()) {
        return _buffers[index];
    }
    return nullptr;
}

void MainWindow::enableDarkMode(bool enable)
{
    _isDarkMode = enable;
    if (enable) {
        applyDarkModeStyle();
    } else {
        applyLightModeStyle();
    }
}

void MainWindow::applyDarkModeStyle()
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);
}

void MainWindow::applyLightModeStyle()
{
    qApp->setStyle(QStyleFactory::create("Windows"));
    qApp->setPalette(qApp->style()->standardPalette());
}

void MainWindow::loadSettings()
{
    _settings.beginGroup("General");
    restoreGeometry(_settings.value("geometry").toByteArray());
    restoreState(_settings.value("windowState").toByteArray());
    _settings.endGroup();
}

void MainWindow::saveSettings()
{
    _settings.beginGroup("General");
    _settings.setValue("geometry", saveGeometry());
    _settings.setValue("windowState", saveState());
    _settings.endGroup();
}

void MainWindow::loadSession()
{
    if (!_notepad_plus_plus_core)
        return;

    NppParameters& nppParam = NppParameters::getInstance();
    Session session;
    if (nppParam.loadSession(session, nullptr, false))
    {
        _notepad_plus_plus_core->loadSession(session, false, nppParam.getSessionPath().c_str());
    }
}

void MainWindow::saveSession()
{
    if (!_notepad_plus_plus_core)
        return;

    Session currentSession;
    _notepad_plus_plus_core->getCurrentOpenedFiles(currentSession, true);
    NppParameters::getInstance().writeSession(currentSession);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveSettings();
    saveSession();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        for (const QUrl& url : mimeData->urls()) {
            if (url.isLocalFile()) {
                openFile(url.toLocalFile());
            }
        }
    }
}

bool MainWindow::event(QEvent* event)
{
    return QMainWindow::event(event);
}

ScintillaComponent* MainWindow::currentScintilla() const
{
    int idx = _pDocTabBar ? _pDocTabBar->currentIndex() : -1;
    if (idx >= 0 && idx < _scintillaEditors.size())
        return _scintillaEditors[idx];
    return nullptr;
}

// NppCommands command dispatcher — routes IDM_* commands to NppCommands via QAction trigger
void MainWindow::command(int cmdId)
{
    if (!_pNppCommands)
        return;

    // Expanded command dispatcher — handles ALL IDM_* commands
    // Each case routes to the corresponding NppCommands slot
    switch (cmdId) {
        // =================================================================
        // FILE COMMANDS
        // =================================================================
        case IDM_FILE_NEW:                 _pNppCommands->fileNew(); break;
        case IDM_FILE_OPEN:                _pNppCommands->fileOpen(); break;
        case IDM_FILE_CLOSE:               _pNppCommands->fileClose(); break;
        case IDM_FILE_CLOSEALL:            _pNppCommands->fileCloseAll(); break;
        case IDM_FILE_CLOSEALL_BUT_CURRENT: _pNppCommands->fileCloseAllButCurrent(); break;
        case IDM_FILE_SAVE:                _pNppCommands->fileSave(); break;
        case IDM_FILE_SAVEAS:              _pNppCommands->fileSaveAs(); break;
        case IDM_FILE_SAVEALL:             _pNppCommands->fileSaveAll(); break;
        case IDM_FILE_SAVECOPYAS:          _pNppCommands->fileSaveCopyAs(); break;
        case IDM_FILE_RELOAD:              _pNppCommands->fileReload(); break;
        case IDM_FILE_PRINT:               _pNppCommands->filePrint(); break;
        case IDM_FILE_DELETE:              _pNppCommands->fileDelete(); break;
        case IDM_FILE_RENAME:              _pNppCommands->fileRename(); break;
        case IDM_FILE_EXIT:                _pNppCommands->fileExit(); break;
        case IDM_FILE_LOADSESSION:         _pNppCommands->fileLoadSession(); break;
        case IDM_FILE_SAVESESSION:         _pNppCommands->fileSaveSession(); break;
        case IDM_FILE_RESTORELASTCLOSEDFILE: _pNppCommands->fileRestoreLastClosed(); break;
        case IDM_FILE_OPENFOLDERASWORKSPACE: _pNppCommands->fileOpenFolderAsWorkspace(); break;
        case IDM_FILE_OPEN_FOLDER:        _pNppCommands->fileOpenFolder(); break;
        case IDM_FILE_OPEN_CMD:            _pNppCommands->fileOpenInCommandPrompt(); break;
        case IDM_FILE_CLOSEALL_TOLEFT:     _pNppCommands->fileCloseAllToLeft(); break;
        case IDM_FILE_CLOSEALL_TORIGHT:   _pNppCommands->fileCloseAllToRight(); break;
        case IDM_FILE_CLOSEALL_UNCHANGED: _pNppCommands->fileCloseAllUnchanged(); break;

        // =================================================================
        // EDIT COMMANDS
        // =================================================================
        case IDM_EDIT_CUT:                 _pNppCommands->editCut(); break;
        case IDM_EDIT_COPY:                _pNppCommands->editCopy(); break;
        case IDM_EDIT_UNDO:                _pNppCommands->editUndo(); break;
        case IDM_EDIT_REDO:                _pNppCommands->editRedo(); break;
        case IDM_EDIT_PASTE:               _pNppCommands->editPaste(); break;
        case IDM_EDIT_DELETE:               _pNppCommands->editDelete(); break;
        case IDM_EDIT_SELECTALL:            _pNppCommands->editSelectAll(); break;
        case IDM_EDIT_INS_TAB:             _pNppCommands->editInsertTab(); break;
        case IDM_EDIT_RMV_TAB:             _pNppCommands->editRemoveTab(); break;
        case IDM_EDIT_DUP_LINE:            _pNppCommands->editDuplicateLine(); break;
        case IDM_EDIT_TRANSPOSE_LINE:      _pNppCommands->editTransposeLine(); break;
        case IDM_EDIT_SPLIT_LINES:         _pNppCommands->editSplitLines(); break;
        case IDM_EDIT_JOIN_LINES:          _pNppCommands->editJoinLines(); break;
        case IDM_EDIT_LINE_UP:             _pNppCommands->editLineUp(); break;
        case IDM_EDIT_LINE_DOWN:           _pNppCommands->editLineDown(); break;
        case IDM_EDIT_UPPERCASE:           _pNppCommands->editUpperCase(); break;
        case IDM_EDIT_LOWERCASE:           _pNppCommands->editLowerCase(); break;
        case IDM_EDIT_PROPERCASE_FORCE:    _pNppCommands->editProperCase(); break;
        case IDM_EDIT_SENTENCECASE_FORCE:  _pNppCommands->editSentenceCase(); break;
        case IDM_EDIT_INVERTCASE:          _pNppCommands->editInvertCase(); break;
        case IDM_EDIT_RANDOMCASE:          _pNppCommands->editRandomCase(); break;
        case IDM_EDIT_BLOCK_COMMENT:       _pNppCommands->editBlockComment(); break;
        case IDM_EDIT_BLOCK_COMMENT_SET:   _pNppCommands->editBlockComment(); break;
        case IDM_EDIT_BLOCK_UNCOMMENT:      _pNppCommands->editBlockUncomment(); break;
        case IDM_EDIT_STREAM_COMMENT:       _pNppCommands->editStreamComment(); break;
        case IDM_EDIT_STREAM_UNCOMMENT:     _pNppCommands->editStreamUncomment(); break;
        case IDM_EDIT_TRIMTRAILING:         _pNppCommands->editTrimTrailing(); break;
        case IDM_EDIT_TRIMLINEHEAD:         _pNppCommands->editTrimLeading(); break;
        case IDM_EDIT_TRIM_BOTH:            _pNppCommands->editTrimBoth(); break;
        case IDM_EDIT_TRIMALL:             _pNppCommands->editTrimAll(); break;
        case IDM_EDIT_EOL2WS:             _pNppCommands->editEolToWs(); break;
        case IDM_EDIT_TAB2SW:              _pNppCommands->editTabToSpace(); break;
        case IDM_EDIT_SW2TAB_LEADING:      _pNppCommands->editSpaceToTabLeading(); break;
        case IDM_EDIT_SW2TAB_ALL:          _pNppCommands->editSpaceToTabAll(); break;
        case IDM_EDIT_REMOVEEMPTYLINES:    _pNppCommands->editRemoveEmptyLines(); break;
        case IDM_EDIT_REMOVEEMPTYLINESWITHBLANK: _pNppCommands->editRemoveEmptyLinesWithBlank(); break;
        case IDM_EDIT_BLANKLINEABOVECURRENT: _pNppCommands->editBlankLineAbove(); break;
        case IDM_EDIT_BLANKLINEBELOWCURRENT: _pNppCommands->editBlankLineBelow(); break;
        case IDM_EDIT_SORTLINES_LEXICOGRAPHIC_ASCENDING: _pNppCommands->editSortLexAsc(); break;
        case IDM_EDIT_SORTLINES_LEXICOGRAPHIC_DESCENDING: _pNppCommands->editSortLexDesc(); break;
        case IDM_EDIT_SORTLINES_INTEGER_ASCENDING: _pNppCommands->editSortIntAsc(); break;
        case IDM_EDIT_SORTLINES_INTEGER_DESCENDING: _pNppCommands->editSortIntDesc(); break;
        case IDM_EDIT_SORTLINES_REVERSE_ORDER: _pNppCommands->editSortReversed(); break;
        case IDM_EDIT_TOGGLEREADONLY:      _pNppCommands->toggleReadOnly(); break;
        case IDM_EDIT_FULLPATHTOCLIP:       _pNppCommands->editFullPathToClip(); break;
        case IDM_EDIT_FILENAMETOCLIP:       _pNppCommands->editFileNameToClip(); break;
        case IDM_EDIT_CURRENTDIRTOCLIP:     _pNppCommands->editCurrentDirToClip(); break;
        case IDM_EDIT_COPY_ALL_NAMES:       _pNppCommands->editCopyAllNames(); break;
        case IDM_EDIT_COPY_ALL_PATHS:       _pNppCommands->editCopyAllPaths(); break;
        case IDM_EDIT_INSERT_DATETIME_SHORT: _pNppCommands->editInsertDateTimeShort(); break;
        case IDM_EDIT_INSERT_DATETIME_LONG: _pNppCommands->editInsertDateTimeLong(); break;
        case IDM_EDIT_INSERT_DATETIME_CUSTOMIZED: _pNppCommands->editInsertDateTimeCustom(); break;
        case IDM_EDIT_COLUMNMODE:           _pNppCommands->columnEditor(); break;
        case IDM_EDIT_CHAR_PANEL:           _pNppCommands->charPanel(); break;
        case IDM_EDIT_CLIPBOARDHISTORY_PANEL: _pNppCommands->clipboardHistoryPanel(); break;
        case IDM_EDIT_AUTOCOMPLETE:         _pNppCommands->editAutoComplete(); break;
        case IDM_EDIT_AUTOCOMPLETE_CURRENTFILE: _pNppCommands->editAutoCompleteCurrentFile(); break;
        case IDM_EDIT_AUTOCOMPLETE_PATH:    _pNppCommands->editAutoCompletePath(); break;
        case IDM_EDIT_FUNCCALLTIP:          _pNppCommands->editFuncCallTip(); break;
        case IDM_EDIT_FUNCCALLTIP_PREVIOUS: _pNppCommands->editFuncCallTipPrevious(); break;
        case IDM_EDIT_FUNCCALLTIP_NEXT:     _pNppCommands->editFuncCallTipNext(); break;
        case IDM_EDIT_REMOVE_CONSECUTIVE_DUP_LINES: _pNppCommands->editRemoveConsecutiveDupLines(); break;
        case IDM_EDIT_REMOVE_ANY_DUP_LINES: _pNppCommands->editRemoveAnyDupLines(); break;
        case IDM_EDIT_SETREADONLYFORALLDOCS: _pNppCommands->setReadOnlyForAll(); break;
        case IDM_EDIT_CLEARREADONLYFORALLDOCS: _pNppCommands->clearReadOnlyForAll(); break;

        // =================================================================
        // SEARCH COMMANDS
        // =================================================================
        case IDM_SEARCH_FIND:              _pNppCommands->searchFind(); break;
        case IDM_SEARCH_FINDNEXT:          _pNppCommands->searchFindNext(); break;
        case IDM_SEARCH_FINDPREV:          _pNppCommands->searchFindPrev(); break;
        case IDM_SEARCH_REPLACE:           _pNppCommands->searchReplace(); break;
        case IDM_SEARCH_GOTOLINE:          _pNppCommands->searchGoToLine(); break;
        case IDM_SEARCH_GOTOMATCHINGBRACE: _pNppCommands->searchGoToMatchingBrace(); break;
        case IDM_SEARCH_SELECTMATCHINGBRACES: _pNppCommands->searchSelectMatchingBraces(); break;
        case IDM_SEARCH_FINDINFILES:       _pNppCommands->searchFindInFiles(); break;
        case IDM_SEARCH_TOGGLE_BOOKMARK:    _pNppCommands->searchToggleBookmark(); break;
        case IDM_SEARCH_NEXT_BOOKMARK:     _pNppCommands->searchNextBookmark(); break;
        case IDM_SEARCH_PREV_BOOKMARK:     _pNppCommands->searchPrevBookmark(); break;
        case IDM_SEARCH_CLEAR_BOOKMARKS:   _pNppCommands->searchClearBookmarks(); break;
        case IDM_SEARCH_MARKALLEXT1:       _pNppCommands->searchMarkAllExt1(); break;
        case IDM_SEARCH_MARKALLEXT2:       _pNppCommands->searchMarkAllExt2(); break;
        case IDM_SEARCH_MARKALLEXT3:       _pNppCommands->searchMarkAllExt3(); break;
        case IDM_SEARCH_MARKALLEXT4:       _pNppCommands->searchMarkAllExt4(); break;
        case IDM_SEARCH_MARKALLEXT5:       _pNppCommands->searchMarkAllExt5(); break;
        case IDM_SEARCH_CLEARALLMARKS:     _pNppCommands->searchClearAllMarks(); break;
        case IDM_SEARCH_CUTMARKEDLINES:    _pNppCommands->searchCutMarkedLines(); break;
        case IDM_SEARCH_COPYMARKEDLINES:   _pNppCommands->searchCopyMarkedLines(); break;
        case IDM_SEARCH_PASTEMARKEDLINES:  _pNppCommands->searchPasteMarkedLines(); break;
        case IDM_SEARCH_DELETEMARKEDLINES: _pNppCommands->searchDeleteMarkedLines(); break;
        case IDM_SEARCH_DELETEUNMARKEDLINES: _pNppCommands->searchDeleteUnmarkedLines(); break;
        case IDM_SEARCH_INVERSEMARKS:      _pNppCommands->searchInverseMarks(); break;
        case IDM_SEARCH_FINDCHARINRANGE:   _pNppCommands->searchFindCharInRange(); break;

        // =================================================================
        // VIEW COMMANDS
        // =================================================================
        case IDM_VIEW_FULLSCREENTOGGLE:    _pNppCommands->viewFullScreenToggle(); break;
        case IDM_VIEW_POSTIT:              _pNppCommands->viewPostIt(); break;
        case IDM_VIEW_DISTRACTIONFREE:     _pNppCommands->viewDistractionFree(); break;
        case IDM_VIEW_ZOOMIN:              _pNppCommands->viewZoomIn(); break;
        case IDM_VIEW_ZOOMOUT:             _pNppCommands->viewZoomOut(); break;
        case IDM_VIEW_ZOOMRESTORE:         _pNppCommands->viewZoomRestore(); break;
        case IDM_VIEW_EOL:                 _pNppCommands->viewEol(); break;
        case IDM_VIEW_ALL_CHARACTERS:      _pNppCommands->viewAllCharacters(); break;
        case IDM_VIEW_INDENT_GUIDE:        _pNppCommands->viewIndentGuide(); break;
        case IDM_VIEW_WRAP:                _pNppCommands->viewWrap(); break;
        case IDM_VIEW_WRAP_SYMBOL:         _pNppCommands->viewWrapSymbol(); break;
        case IDM_VIEW_TAB_SPACE:           _pNppCommands->viewTabSpace(); break;
        case IDM_VIEW_NPC:                 _pNppCommands->viewNpc(); break;
        case IDM_VIEW_NPC_CCUNIEOL:        _pNppCommands->viewNpcCcuniEol(); break;
        case IDM_VIEW_HIDELINES:           _pNppCommands->viewHideLines(); break;
        case IDM_VIEW_SUMMARY:             _pNppCommands->viewSummary(); break;
        case IDM_VIEW_ALWAYSONTOP:         _pNppCommands->viewAlwaysOnTop(); break;
        case IDM_VIEW_GOTO_ANOTHER_VIEW:   _pNppCommands->viewSwitchToOtherView(); break;
        case IDM_VIEW_CLONE_TO_ANOTHER_VIEW: _pNppCommands->viewCloneToAnotherView(); break;
        case IDM_VIEW_GOTO_NEW_INSTANCE:   _pNppCommands->viewGotoNewInstance(); break;
        case IDM_VIEW_LOAD_IN_NEW_INSTANCE: _pNppCommands->viewLoadInNewInstance(); break;
        case IDM_VIEW_GOTO_START:          _pNppCommands->viewGotoStart(); break;
        case IDM_VIEW_GOTO_END:            _pNppCommands->viewGotoEnd(); break;
        case IDM_VIEW_MONITORING:          _pNppCommands->toggleMonitoring(); break;
        case IDM_VIEW_DOC_MAP:            _pNppCommands->toggleDocumentMap(); break;
        case IDM_VIEW_FUNC_LIST:           _pNppCommands->toggleFunctionList(); break;
        case IDM_VIEW_FILEBROWSER:         _pNppCommands->toggleFileBrowser(); break;
        case IDM_VIEW_PROJECT_PANEL_1:     _pNppCommands->toggleProjectPanel1(); break;
        case IDM_VIEW_PROJECT_PANEL_2:     _pNppCommands->toggleProjectPanel2(); break;
        case IDM_VIEW_PROJECT_PANEL_3:     _pNppCommands->toggleProjectPanel3(); break;

        // =================================================================
        // MACRO COMMANDS
        // =================================================================
        case IDM_MACRO_STARTRECORDINGMACRO:  _pNppCommands->macroStartRecording(); break;
        case IDM_MACRO_STOPRECORDINGMACRO:   _pNppCommands->macroStopRecording(); break;
        case IDM_MACRO_PLAYBACKRECORDEDMACRO: _pNppCommands->macroPlayback(); break;
        case IDM_MACRO_SAVECURRENTMACRO:     _pNppCommands->macroSaveCurrent(); break;
        case IDM_MACRO_RUNMULTIMACRODLG:     _pNppCommands->macroRunMultiple(); break;

        // =================================================================
        // SETTINGS COMMANDS
        // =================================================================
        case IDM_SETTING_IMPORTPLUGIN:      _pNppCommands->settingsImportPlugin(); break;
        case IDM_SETTING_IMPORTSTYLETHEMES: _pNppCommands->settingsImportStyleThemes(); break;
        case IDM_SETTING_SHORTCUT_MAPPER:   _pNppCommands->settingsShortcutMapper(); break;
        case IDM_SETTING_EDITCONTEXTMENU:   _pNppCommands->settingsEditContextMenu(); break;
        case IDM_SETTING_PREFERENCE:        _pNppCommands->settingsPreference(); break;
        case IDM_LANGSTYLE_CONFIG_DLG:      _pNppCommands->settingsStyleConfig(); break;

        // =================================================================
        // ENCODING COMMANDS (Format menu)
        // =================================================================
        case IDM_FORMAT_UTF_8:              _pNppCommands->encodingUTF8(); break;
        case IDM_FORMAT_UTF_16BE:           _pNppCommands->encodingUTF16BE(); break;
        case IDM_FORMAT_UTF_16LE:           _pNppCommands->encodingUTF16LE(); break;
        case IDM_FORMAT_ANSI:               _pNppCommands->encodingAnsi(); break;
        case IDM_FORMAT_CONV2_ANSI:         _pNppCommands->encodingConvertAnsi(); break;
        case IDM_FORMAT_CONV2_UTF_8:        _pNppCommands->encodingConvertUTF8(); break;
        case IDM_FORMAT_CONV2_AS_UTF_8:     _pNppCommands->encodingConvertUTF8BOM(); break;
        case IDM_FORMAT_CONV2_UTF_16BE:     _pNppCommands->encodingConvertUTF16BE(); break;
        case IDM_FORMAT_CONV2_UTF_16LE:     _pNppCommands->encodingConvertUTF16LE(); break;
        case IDM_FORMAT_TODOS:              _pNppCommands->setFormatToDos(); break;
        case IDM_FORMAT_TOUNIX:             _pNppCommands->setFormatToUnix(); break;
        case IDM_FORMAT_TOMAC:              _pNppCommands->setFormatToMac(); break;

        // =================================================================
        // LANGUAGE COMMANDS
        // =================================================================
        case IDM_LANG_USER:                _pNppCommands->languageUserDefine(); break;
        case IDM_LANG_OPENUDLDIR:          _pNppCommands->languageOpenUdldDir(); break;

        // =================================================================
        // RUN COMMANDS
        // =================================================================
        case IDM_EXECUTE:                  _pNppCommands->runExecute(); break;
        case IDM_CMDLINEARGUMENTS:         _pNppCommands->runCmdLineArgs(); break;

        // =================================================================
        // HELP COMMANDS
        // =================================================================
        case IDM_HOMESWEETHOME:            _pNppCommands->helpHomeSweetHome(); break;
        case IDM_PROJECTPAGE:              _pNppCommands->helpDocumentation(); break;
        case IDM_ONLINEDOCUMENT:           _pNppCommands->helpOnlineDoc(); break;
        case IDM_ABOUT:                    _pNppCommands->helpAbout(); break;
        case IDM_DEBUGINFO:                _pNppCommands->showDebugInfo(); break;

        // =================================================================
        // WINDOW COMMANDS
        // =================================================================
        case IDM_WINDOW_SORT_FN_ASC:       _pNppCommands->windowSortAscending(); break;
        case IDM_WINDOW_SORT_FN_DSC:       _pNppCommands->windowSortDescending(); break;

        // =================================================================
        // TOOL COMMANDS (MD5, SHA)
        // =================================================================
        case IDM_TOOL_MD5_GENERATE:        _pNppCommands->generateMD5(); break;
        case IDM_TOOL_MD5_GENERATEFROMFILE: _pNppCommands->generateMD5FromFile(); break;
        case IDM_TOOL_MD5_GENERATEINTOCLIPBOARD: _pNppCommands->generateMD5ToClipboard(); break;
        case IDM_TOOL_SHA256_GENERATE:     _pNppCommands->generateSHA256(); break;
        case IDM_TOOL_SHA256_GENERATEFROMFILE: _pNppCommands->generateSHA256FromFile(); break;
        case IDM_TOOL_SHA256_GENERATEINTOCLIPBOARD: _pNppCommands->generateSHA256ToClipboard(); break;
        case IDM_TOOL_SHA512_GENERATE:     _pNppCommands->generateSHA512(); break;
        case IDM_TOOL_SHA512_GENERATEFROMFILE: _pNppCommands->generateSHA512FromFile(); break;
        case IDM_TOOL_SHA512_GENERATEINTOCLIPBOARD: _pNppCommands->generateSHA512ToClipboard(); break;

        // =================================================================
        // MISC / DOCUMENT LIST COMMANDS
        // =================================================================
        case IDM_DOCLIST_FILESCLOSE:       _pNppCommands->closeDocumentList(); break;
        case IDM_DOCLIST_FILESCLOSEOTHERS:  _pNppCommands->closeOtherDocuments(); break;
        case IDM_DOCLIST_COPYNAMES:        _pNppCommands->copyDocumentNames(); break;
        case IDM_DOCLIST_COPYPATHS:        _pNppCommands->copyDocumentPaths(); break;

        default: break;
    }
}

ScintillaEditView* MainWindow::getActiveView()
{
    // Single-view: active view is always the current scintilla tab
    return currentScintilla();
}
