// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 Translation - Notepad_plus.cpp
// Translated from Win32 MFC to Qt6

#include "Notepad_plus.h"
#include "Notepad_plus_Window.h"
#include "ScintillaEditView.h"
#include "Buffer.h"
#include "FileManager.h"
#include "Parameters.h"
#include "NppDarkMode.h"
#include "NppConstants.h"
#include "WinControls/OpenSaveFileDialog/CustomFileDialog.h"
#include "Utf8_16.h"
#include "EncodingMapper.h"
#include "NppXml.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QDateTime>
#include <QApplication>
#include <QStyleFactory>
#include <QInputDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QClipboard>
#include <QMimeData>
#include <QThread>
#include <QMutexLocker>
#include <QFileInfo>
#include <QDir>
#include <QProcessEnvironment>
#include <QStandardPaths>

// Stub resource IDs for Linux (replaced with icons from theme on Qt)
#define IDI_NEW_ICON 1000
#define IDI_NEW_ICON2 1001
#define IDI_OPEN_ICON 1002
#define IDI_OPEN_ICON2 1003
#define IDI_SAVE_ICON 1004
#define IDI_SAVE_DISABLE_ICON 1005
#define IDI_SAVE_ICON2 1006
#define IDI_SAVE_DISABLE_ICON2 1007
#define IDI_SAVEALL_ICON 1008
#define IDI_SAVEALL_DISABLE_ICON 1009
#define IDI_SAVEALL_ICON2 1010
#define IDI_SAVEALL_DISABLE_ICON2 1011
#define IDI_CLOSE_ICON 1012
#define IDI_CLOSE_ICON2 1013
#define IDI_CLOSE_ALL_ICON 1014
#define IDI_CLOSEALL_ICON2 1015
#define IDI_CLOSEALL_DISABLE_ICON 1016
#define IDI_CLOSEALL_DISABLE_ICON2 1017
#define IDI_NEW_ICON_DM 1018
#define IDI_NEW_ICON_DM2 1019
#define IDI_OPEN_ICON_DM 1020
#define IDI_OPEN_ICON_DM2 1021
#define IDI_SAVE_ICON_DM 1022
#define IDI_SAVE_DISABLE_ICON_DM 1023
#define IDI_SAVE_ICON_DM2 1024
#define IDI_SAVE_DISABLE_ICON_DM2 1025
#define IDI_SAVEALL_ICON_DM 1026
#define IDI_SAVEALL_DISABLE_ICON_DM 1027
#define IDI_SAVEALL_ICON_DM2 1028
#define IDI_SAVEALL_DISABLE_ICON_DM2 1029
#define IDI_CLOSE_ICON_DM 1030
#define IDI_CLOSE_ICON_DM2 1031
#define IDI_CLOSEALL_ICON_DM 1032
#define IDI_CLOSEALL_ICON_DM2 1033
#define IDI_CLOSEALL_DISABLE_ICON_DM 1034
#define IDI_CLOSEALL_DISABLE_ICON_DM2 1035
#define IDI_RENAME_ICON 1036
#define IDI_REDO_ICON 1037
#define IDI_UNDO_ICON 1038
#define IDI_LOG_ICON 1039
#define IDI_SETTING_ICON 1040
#define IDI_CUT_ICON 1041
#define IDI_COPY_ICON 1042
#define IDI_PASTE_ICON 1043
#define IDI_EOF_ICON 1044
#define IDI_DOCMAP_ICON 1045
#define IDI_RUN_ICON 1046
#define IDI_STOP_ICON 1047
#define IDI_FILESAVE 2000
#define IDI_FILEOPEN 2001
#define IDI_FILENEW 2002
#define IDI_CLOSEFILE 2003
#define IDI_SAVEALL 2004
#define IDI_CLOSEALL 2005
#define IDI_STOP 2006
#define IDI_FIND 2007
#define IDI_REPLACE 2008
#define IDI_GOTO 2009
#define IDI_TAB 2010
#define IDI_BACKWARDTAB 2011
#define IDI_WORDWRAP 2012
#define IDI_SETTING 2013
#define IDI_FAVORITE 2014
#define IDI_CLEARFAVORITE 2015
#define IDI_OPENFOLDERS 2016
#define IDI_DOCLIST 2017
#define IDI_FILEBROWSER 2018
#define IDI_DOCMAP 2019
#define IDI_FOLDER_AS_PROJECT_DIGIT_1 2020
#define IDI_FOLDER_AS_PROJECT_DIGIT_10 2029
#define IDI_FOLDEROPEN_ICON 2030
#define IDI_FOLDERCLOSE_ICON 2031
#define IDI_LOCK_ICON 2032
#define IDI_UNLOCK_ICON 2033
#define IDI_BLANK_ICON 2034
#define IDI_ADD_ICON 2035
#define IDI_REMOVE_ICON 2036
#define IDI_APPLICATION_ICON 2037
#define IDI_HISTORY_ICON 2038

// Stub resource IDs for menu/toolbar resources
#define IDR_FILENEW 3000
#define IDR_FILEOPEN 3001
#define IDR_FILESAVE 3002
#define IDR_SAVEALL 3003
#define IDR_CLOSEFILE 3004
#define IDR_CLOSEALL 3005

#define MODEVENTMASK_OFF 0
#define TAB_VERTICAL 0x10
#define POS_VERTICAL 1
#define POS_HORIZONTAL 0

// Global time tracking
std::chrono::steady_clock::time_point g_nppStartTimePoint;
std::chrono::steady_clock::duration g_pluginsLoadingTime{};
std::atomic<bool> g_bNppExitFlag{false};

using namespace std;

// ToolBar button structure
struct ToolBarButtonUnit {
    int _command;
    int _normalIcon;
    int _disableIcon;
    int _altIcon1;
    int _altDisableIcon1;
    int _altIcon2;
    int _altDisableIcon2;
    int _altIcon3;
    int _altDisableIcon3;
    int _resourceIcon;
};

// Toolbar icons definition (simplified for Qt)
static constexpr ToolBarButtonUnit toolBarIcons[] = {
    {IDM_FILE_NEW, IDI_NEW_ICON, IDI_NEW_ICON, IDI_NEW_ICON2, IDI_NEW_ICON2, IDI_NEW_ICON_DM, IDI_NEW_ICON_DM, IDI_NEW_ICON_DM2, IDI_NEW_ICON_DM2, IDR_FILENEW},
    {IDM_FILE_OPEN, IDI_OPEN_ICON, IDI_OPEN_ICON, IDI_OPEN_ICON2, IDI_OPEN_ICON2, IDI_OPEN_ICON_DM, IDI_OPEN_ICON_DM, IDI_OPEN_ICON_DM2, IDI_OPEN_ICON_DM2, IDR_FILEOPEN},
    {IDM_FILE_SAVE, IDI_SAVE_ICON, IDI_SAVE_DISABLE_ICON, IDI_SAVE_ICON2, IDI_SAVE_DISABLE_ICON2, IDI_SAVE_ICON_DM, IDI_SAVE_DISABLE_ICON_DM, IDI_SAVE_ICON_DM2, IDI_SAVE_DISABLE_ICON_DM2, IDR_FILESAVE},
    {IDM_FILE_SAVEALL, IDI_SAVEALL_ICON, IDI_SAVEALL_DISABLE_ICON, IDI_SAVEALL_ICON2, IDI_SAVEALL_DISABLE_ICON2, IDI_SAVEALL_ICON_DM, IDI_SAVEALL_DISABLE_ICON_DM, IDI_SAVEALL_ICON_DM2, IDI_SAVEALL_DISABLE_ICON_DM2, IDR_SAVEALL},
    {IDM_FILE_CLOSE, IDI_CLOSE_ICON, IDI_CLOSE_ICON, IDI_CLOSE_ICON2, IDI_CLOSE_ICON2, IDI_CLOSE_ICON_DM, IDI_CLOSE_ICON_DM, IDI_CLOSE_ICON_DM2, IDI_CLOSE_ICON_DM2, IDR_CLOSEFILE},
    {IDM_FILE_CLOSEALL, IDI_CLOSEALL_ICON, IDI_CLOSEALL_ICON, IDI_CLOSEALL_ICON2, IDI_CLOSEALL_ICON2, IDI_CLOSEALL_ICON_DM, IDI_CLOSEALL_ICON_DM, IDI_CLOSEALL_ICON_DM2, IDI_CLOSEALL_ICON_DM2, IDR_CLOSEALL},
    // ... additional toolbar icons
};

// Constructor
Notepad_plus::Notepad_plus()
    : QMainWindow()
    , _autoCompleteMain(&_mainEditView)
    , _autoCompleteSub(&_subEditView)
    , _smartHighlighter(&_findReplaceDlg)
{
    // Initialize member variables
    _pMainWindow = nullptr;
    _pEditView = nullptr;
    _pNonEditView = nullptr;
    _pDocTab = nullptr;
    _pNonDocTab = nullptr;
    _pMainSplitter = nullptr;
    
    // Load dark mode
    NppParameters& nppParam = NppParameters::getInstance();
    NppDarkMode::initDarkMode();
    
    // Initialize native language
    NppXml::Document nativeLangDocRoot = nppParam.getNativeLang();
    _nativeLangSpeaker.init(nativeLangDocRoot);
    
    LocalizationSwitcher & localizationSwitcher = nppParam.getLocalizationSwitcher();
    const char *fn = _nativeLangSpeaker.getFileName();
    if (fn) {
        localizationSwitcher.setFileName(fn);
    }
    
    nppParam.setNativeLangSpeaker(&_nativeLangSpeaker);
    
    // Initialize toolbar theme
    NppXml::Document toolButtonsDocRoot = nppParam.getCustomizedToolButtons();
    if (toolButtonsDocRoot) {
        // Initialize toolbar with custom settings
    }
    
    // Check administrator status (always false on Linux)
    _isAdministrator = false;
    nppParam.setAdminMode(false);
    
    // Initialize file watcher
    _pFileWatcher = new QFileSystemWatcher(this);
    
    // Track start time
    g_nppStartTimePoint = std::chrono::steady_clock::now();
}

// Destructor
Notepad_plus::~Notepad_plus() {
    // Save session before destruction
    saveCurrentSession();
    
    // Save all settings
    saveGUIParams();
    saveDockingParams();
    
    // Destroy all child widgets in correct order
    killAllChildren();
    
    // Destroy parameters singleton
    NppParameters::getInstance().destroyInstance();
    
    delete _pFileWatcher;
}

// Main initialization
bool Notepad_plus::init() {
    NppParameters& nppParam = NppParameters::getInstance();
    NppGUI & nppGUI = nppParam.getNppGUI();
    
    // Setup menu bar
    setupMenuBar();
    
    // Setup toolbar
    setupToolBar();
    
    // Setup status bar
    setupStatusBar();
    
    // Initialize document tabs
    _pDocTab = &_mainDocTab;
    _pEditView = &_mainEditView;
    _pNonDocTab = &_subDocTab;
    _pNonEditView = &_subEditView;
    
    // Initialize Scintilla views
    _mainEditView.init(this);
    _subEditView.init(this);
    _invisibleEditView.init(this);
    _fileEditView.init(this);
    _fileEditView.execute(SCI_SETMODEVENTMASK, MODEVENTMASK_OFF);
    
    // Initialize file manager
    MainFileManager.init(this, &_fileEditView);
    
    // Set font list
    nppParam.setFontList(this);
    
    // Set initial window state
    _mainWindowStatus = static_cast<uint8_t>(WindowStatus::WindowMainActive);
    _activeView = MAIN_VIEW;
    
    // Get scintilla view parameters
    const ScintillaViewParams & svp = nppParam.getSVP();
    
    int tabBarStatus = nppGUI._tabStatus;
    
    // Initialize document tabs
    unsigned char buttonsStatus = 0;
    buttonsStatus |= (tabBarStatus & TAB_CLOSEBUTTON) ? 1 : 0;
    buttonsStatus |= (tabBarStatus & TAB_PINBUTTON) ? 2 : 0;
    
    _mainDocTab.init(this, &_mainEditView, buttonsStatus);
    _subDocTab.init(this, &_subEditView, buttonsStatus);

    _mainEditView.show();  // Qt show() instead of display()
    
    // Configure scintilla views
    _mainEditView.showMargin(ScintillaEditView::_SC_MARGE_LINENUMBER, svp._lineNumberMarginShow);
    _subEditView.showMargin(ScintillaEditView::_SC_MARGE_LINENUMBER, svp._lineNumberMarginShow);
    _mainEditView.showMargin(ScintillaEditView::_SC_MARGE_SYMBOL, svp._bookMarkMarginShow);
    _subEditView.showMargin(ScintillaEditView::_SC_MARGE_SYMBOL, svp._bookMarkMarginShow);
    
    _mainEditView.showIndentGuideLine(svp._indentGuideLineShow);
    _subEditView.showIndentGuideLine(svp._indentGuideLineShow);
    
    // Set caret properties
    emit nppInternalSetCaretWidth(0);
    emit nppInternalSetCaretBlinkRate(0);
    
    // Configure marker style
    _mainEditView.setMakerStyle(svp._folderStyle);
    _subEditView.setMakerStyle(svp._folderStyle);
    _mainEditView.defineDocType(_mainEditView.getCurrentBuffer()->getLangType());
    _subEditView.defineDocType(_subEditView.getCurrentBuffer()->getLangType());
    
    // Line wrap
    _mainEditView.setWrapMode(svp._lineWrapMethod);
    _subEditView.setWrapMode(svp._lineWrapMethod);
    
    // Scroll behavior
    _mainEditView.execute(SCI_SETENDATLASTLINE, !svp._scrollBeyondLastLine);
    _subEditView.execute(SCI_SETENDATLASTLINE, !svp._scrollBeyondLastLine);
    
    // Font quality
    if (svp._doSmoothFont) {
        _mainEditView.execute(SCI_SETFONTQUALITY, SC_EFF_QUALITY_LCD_OPTIMIZED);
        _subEditView.execute(SCI_SETFONTQUALITY, SC_EFF_QUALITY_LCD_OPTIMIZED);
    }
    
    // Border edge
    _mainEditView.setBorderEdge(svp._showBorderEdge);
    _subEditView.setBorderEdge(svp._showBorderEdge);
    
    // Caret line
    _mainEditView.execute(SCI_SETCARETLINEVISIBLEALWAYS, true);
    _subEditView.execute(SCI_SETCARETLINEVISIBLEALWAYS, true);
    
    // Wrap mode
    _mainEditView.wrap(svp._doWrap);
    _subEditView.wrap(svp._doWrap);
    
    // Edge multi selection
    emit nppInternalEdgeMultiSetSize(0);
    
    // Show settings
    _mainEditView.showEOL(svp._eolShow);
    _subEditView.showEOL(svp._eolShow);
    _mainEditView.showWSAndTab(svp._whiteSpaceShow);
    _subEditView.showWSAndTab(svp._whiteSpaceShow);
    _mainEditView.showWrapSymbol(svp._wrapSymbolShow);
    _subEditView.showWrapSymbol(svp._wrapSymbolShow);
    
    // Global styles
    _mainEditView.performGlobalStyles();
    _subEditView.performGlobalStyles();
    
    // Zoom settings
    _zoomOriginalValue = _pEditView->execute(SCI_GETZOOM);
    _mainEditView.execute(SCI_SETZOOM, svp._zoom);
    _subEditView.execute(SCI_SETZOOM, svp._zoomSync ? svp._zoom : svp._zoom2);
    
    // Multi-selection
    emit nppInternalSetMultiSelection(0);
    
    // Additional selection typing
    _mainEditView.execute(SCI_SETADDITIONALSELECTIONTYPING, true);
    _subEditView.execute(SCI_SETADDITIONALSELECTIONTYPING, true);
    
    // Virtual space
    int virtualSpaceOptions = SCVS_RECTANGULARSELECTION;
    if (svp._virtualSpace)
        virtualSpaceOptions |= SCVS_USERACCESSIBLE | SCVS_NOWRAPLINESTART;
    _mainEditView.execute(SCI_SETVIRTUALSPACEOPTIONS, virtualSpaceOptions);
    _subEditView.execute(SCI_SETVIRTUALSPACEOPTIONS, virtualSpaceOptions);
    
    // Multi-paste
    _mainEditView.execute(SCI_SETMULTIPASTE, SC_MULTIPASTE_EACH);
    _subEditView.execute(SCI_SETMULTIPASTE, SC_MULTIPASTE_EACH);
    
    // Auto-completion multi
    _mainEditView.execute(SCI_AUTOCSETMULTI, SC_MULTIAUTOC_EACH);
    _subEditView.execute(SCI_AUTOCSETMULTI, SC_MULTIAUTOC_EACH);
    
    // Mouse selection rectangular switch
    _mainEditView.execute(SCI_SETMOUSESELECTIONRECTANGULARSWITCH, true);
    _subEditView.execute(SCI_SETMOUSESELECTIONRECTANGULARSWITCH, true);
    
    // Automatic fold
    _mainEditView.execute(SCI_SETAUTOMATICFOLD, SC_AUTOMATICFOLD_SHOW | SC_AUTOMATICFOLD_CHANGE);
    _subEditView.execute(SCI_SETAUTOMATICFOLD, SC_AUTOMATICFOLD_SHOW | SC_AUTOMATICFOLD_CHANGE);
    
    // Padding
    _mainEditView.execute(SCI_SETMARGINLEFT, 0, svp._paddingLeft);
    _mainEditView.execute(SCI_SETMARGINRIGHT, 0, svp._paddingRight);
    _subEditView.execute(SCI_SETMARGINLEFT, 0, svp._paddingLeft);
    _subEditView.execute(SCI_SETMARGINRIGHT, 0, svp._paddingRight);
    
    // Monospace check
    _mainEditView.execute(SCI_STYLESETCHECKMONOSPACED, STYLE_DEFAULT, true);
    _subEditView.execute(SCI_STYLESETCHECKMONOSPACED, STYLE_DEFAULT, true);
    
    // Undo selection history
    _mainEditView.execute(SCI_SETUNDOSELECTIONHISTORY, SC_UNDO_SELECTION_HISTORY_ENABLED | SC_UNDO_SELECTION_HISTORY_SCROLL);
    _subEditView.execute(SCI_SETUNDOSELECTIONHISTORY, SC_UNDO_SELECTION_HISTORY_ENABLED | SC_UNDO_SELECTION_HISTORY_SCROLL);
    
    // Show document tabs
    _mainDocTab.show();  // Qt show() instead of display()

    // Tab bar direction
    if (nppGUI._tabStatus & TAB_VERTICAL)
        TabBarPlus::doVertical(true);  // enable vertical mode
    
    // Draw tab colors from styler
    drawTabbarColoursFromStylerArray();
    
    // Initialize colors
    const Style* pStyle = nppParam.getGlobalStylers().findByID(STYLE_DEFAULT);
    if (pStyle) {
        // These color-setter calls might not exist in NppParameters yet.
        // If they don't exist, add stubs in NppParameters.h/.cpp:
        //   setCurrentDefaultFgColor(QRgb) and setCurrentDefaultBgColor(QRgb)
        // For now, call dark mode colour setters if available
        drawAutocompleteColoursFromTheme(pStyle->_fgColor, pStyle->_bgColor);
    }
    
    // Initialize autocomplete
    AutoCompletion::drawAutocomplete(_pEditView);
    AutoCompletion::drawAutocomplete(_pNonEditView);
    
    // Document map colors
    drawDocumentMapColoursFromStylerArray();
    
    // Initialize splitter
    bool isVertical = (nppGUI._splitterPos == POS_VERTICAL);
    _subSplitter.create(&_mainDocTab, &_subDocTab, 50, SplitterMode::Horizontal, 50, isVertical);
    
    // Initialize status bar
    bool willBeShown = nppGUI._statusBarShow;
    _pStatusBar = statusBar();
    _pStatusBar->setVisible(willBeShown);
    
    // Initialize docking manager
    _pMainWindow = _pDocTab;
    _dockingManager.init();
    
    // Initialize plugin manager
    NppData nppData;
    nppData._nppHandle = this;
    nppData._scintillaMainHandle = _mainEditView.getHSelf();
    nppData._scintillaSecondHandle = _subEditView.getHSelf();
    
    _scintillaCtrls4Plugins.init();
    _pluginsManager.loadPlugins(nppParam.getPluginRootDir(),
    auto pluginsLoadingStartTP = std::chrono::steady_clock::now();
    _pluginsManager.loadPlugins(nppParam.getPluginRootDir(), 
                                 enablePluginAdmin ? &_pluginsAdminDlg.getAvailablePluginUpdateInfoList() : nullptr,
                                 enablePluginAdmin ? &_pluginsAdminDlg.getIncompatibleList() : nullptr);
    g_pluginsLoadingTime = std::chrono::steady_clock::now() - pluginsLoadingStartTP;
    
    // Initialize dialogs
    _configStyleDlg.init(this);
    _preference.init(this);
    _pluginsAdminDlg.init(this);
    _findReplaceDlg.init(nullptr, nullptr, nullptr);
    _findInFinderDlg.init(this);
    _incrementFindDlg.init(nullptr, nullptr, false);
    _goToLineDlg.init(this, &_pEditView);
    _colEditorDlg.init(this, &_pEditView);
    _aboutDlg.init(this);
    _runDlg.init(this);
    _runMacroDlg.init(this);
    
    // Initialize menus
    initMenus();
    
    // Initialize accelerators
    _accelerator.init();
    // _scintaccelerator manages Scintilla key bindings
    std::vector<QWidget*> scints;
    scints.push_back(_mainEditView.sci());
    scints.push_back(_subEditView.sci());
    _scintaccelerator.init(&scints, _pMainMenu, this);
    _scintaccelerator.updateKeys();
    
    // Check initial states
    checkSyncState();
    checkMacroState();
    checkDocState();
    checkClipboard();
    
    // Load last session if configured
    if (nppGUI._rememberLastSession && !nppGUI._isCmdlineNosessionActivated) {
        loadLastSession();
    }
    
    // Load initial buffers
    loadBufferIntoView(_mainEditView.getCurrentBufferID(), MAIN_VIEW);
    loadBufferIntoView(_subEditView.getCurrentBufferID(), SUB_VIEW);
    activateBuffer(_mainEditView.getCurrentBufferID(), MAIN_VIEW);
    activateBuffer(_subEditView.getCurrentBufferID(), SUB_VIEW);
    
    _mainEditView.grabFocus();
    
    return true;
}

// Setup menu bar
void Notepad_plus::setupMenuBar() {
    _pMainMenu = menuBar();
    
    NppParameters& nppParam = NppParameters::getInstance();
    
    // File menu
    QMenu* fileMenu = _pMainMenu->addMenu(tr("&File"));
    
    QAction* actionNew = fileMenu->addAction(tr("&New"), this, &Notepad_plus::onFileNew);
    actionNew->setShortcut(QKeySequence::New);
    actionNew->setData(IDM_FILE_NEW);
    
    QAction* actionOpen = fileMenu->addAction(tr("&Open..."), this, &Notepad_plus::onFileOpen);
    actionOpen->setShortcut(QKeySequence::Open);
    actionOpen->setData(IDM_FILE_OPEN);
    
    fileMenu->addSeparator();
    
    QAction* actionSave = fileMenu->addAction(tr("&Save"), this, &Notepad_plus::onFileSave);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setData(IDM_FILE_SAVE);
    
    QAction* actionSaveAs = fileMenu->addAction(tr("Save &As..."), this, &Notepad_plus::onFileSaveAs);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    actionSaveAs->setData(IDM_FILE_SAVEAS);
    
    QAction* actionSaveAll = fileMenu->addAction(tr("Save A&ll"), this, &Notepad_plus::onFileSaveAll);
    actionSaveAll->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    actionSaveAll->setData(IDM_FILE_SAVEALL);
    
    fileMenu->addSeparator();
    
    QAction* actionClose = fileMenu->addAction(tr("&Close"), this, &Notepad_plus::onFileClose);
    actionClose->setShortcut(QKeySequence::Close);
    actionClose->setData(IDM_FILE_CLOSE);
    
    QAction* actionCloseAll = fileMenu->addAction(tr("Close A&ll"), this, &Notepad_plus::onFileCloseAll);
    actionCloseAll->setData(IDM_FILE_CLOSEALL);
    
    fileMenu->addSeparator();
    
    QAction* actionPrint = fileMenu->addAction(tr("&Print..."), this, &Notepad_plus::onFilePrint);
    actionPrint->setShortcut(QKeySequence::Print);
    actionPrint->setData(IDM_FILE_PRINT);
    
    fileMenu->addSeparator();
    
    QAction* actionExit = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    actionExit->setShortcut(QKeySequence::Quit);
    actionExit->setData(IDM_FILE_EXIT);
    
    // Edit menu
    QMenu* editMenu = _pMainMenu->addMenu(tr("&Edit"));
    
    QAction* actionUndo = editMenu->addAction(tr("&Undo"), this, &Notepad_plus::onEditUndo);
    actionUndo->setShortcut(QKeySequence::Undo);
    actionUndo->setData(IDM_EDIT_UNDO);
    
    QAction* actionRedo = editMenu->addAction(tr("&Redo"), this, &Notepad_plus::onEditRedo);
    actionRedo->setShortcut(QKeySequence::Redo);
    actionRedo->setData(IDM_EDIT_REDO);
    
    editMenu->addSeparator();
    
    QAction* actionCut = editMenu->addAction(tr("Cu&t"), this, &Notepad_plus::onEditCut);
    actionCut->setShortcut(QKeySequence::Cut);
    actionCut->setData(IDM_EDIT_CUT);
    
    QAction* actionCopy = editMenu->addAction(tr("&Copy"), this, &Notepad_plus::onEditCopy);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionCopy->setData(IDM_EDIT_COPY);
    
    QAction* actionPaste = editMenu->addAction(tr("&Paste"), this, &Notepad_plus::onEditPaste);
    actionPaste->setShortcut(QKeySequence::Paste);
    actionPaste->setData(IDM_EDIT_PASTE);
    
    QAction* actionDelete = editMenu->addAction(tr("&Delete"), this, &Notepad_plus::onEditDelete);
    actionDelete->setShortcut(QKeySequence::Delete);
    actionDelete->setData(IDM_EDIT_DELETE);
    
    editMenu->addSeparator();
    
    QAction* actionSelectAll = editMenu->addAction(tr("Select A&ll"), this, &Notepad_plus::onEditSelectAll);
    actionSelectAll->setShortcut(QKeySequence::SelectAll);
    actionSelectAll->setData(IDM_EDIT_SELECTALL);
    
    // Search menu
    QMenu* searchMenu = _pMainMenu->addMenu(tr("&Search"));
    
    QAction* actionFind = searchMenu->addAction(tr("&Find..."), this, &Notepad_plus::onSearchFind);
    actionFind->setShortcut(QKeySequence::Find);
    actionFind->setData(IDM_SEARCH_FIND);
    
    QAction* actionReplace = searchMenu->addAction(tr("&Replace..."), this, &Notepad_plus::onSearchReplace);
    actionReplace->setShortcut(QKeySequence::Replace);
    actionReplace->setData(IDM_SEARCH_REPLACE);
    
    searchMenu->addSeparator();
    
    QAction* actionGoToLine = searchMenu->addAction(tr("&Go to Line..."), this, &Notepad_plus::onSearchGoToLine);
    actionGoToLine->setShortcut(QKeySequence(tr("Ctrl+G")));
    actionGoToLine->setData(IDM_SEARCH_GOTOLINE);
    
    // View menu
    QMenu* viewMenu = _pMainMenu->addMenu(tr("&View"));
    
    QAction* actionZoomIn = viewMenu->addAction(tr("Zoom &In"), this, &Notepad_plus::onViewZoomIn);
    actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    actionZoomIn->setData(IDM_VIEW_ZOOMIN);
    
    QAction* actionZoomOut = viewMenu->addAction(tr("Zoom &Out"), this, &Notepad_plus::onViewZoomOut);
    actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    actionZoomOut->setData(IDM_VIEW_ZOOMOUT);
    
    viewMenu->addSeparator();
    
    QAction* actionFullScreen = viewMenu->addAction(tr("&Full Screen"), this, &Notepad_plus::onViewFullScreen);
    actionFullScreen->setShortcut(QKeySequence::FullScreen);
    actionFullScreen->setData(IDM_VIEW_FULLSCREENTOGGLE);
    
    // Macro menu
    QMenu* macroMenu = _pMainMenu->addMenu(tr("&Macro"));
    
    QAction* actionStartRecord = macroMenu->addAction(tr("&Start Recording"), this, &Notepad_plus::onMacroStartRecord);
    actionStartRecord->setData(IDM_MACRO_STARTRECORDINGMACRO);
    
    QAction* actionStopRecord = macroMenu->addAction(tr("Sto&p Recording"), this, &Notepad_plus::onMacroStopRecord);
    actionStopRecord->setData(IDM_MACRO_STOPRECORDINGMACRO);
    
    QAction* actionPlayRecord = macroMenu->addAction(tr("&Playback"), this, &Notepad_plus::onMacroPlayRecord);
    actionPlayRecord->setData(IDM_MACRO_PLAYBACKRECORDEDMACRO);
    
    // Run menu
    QMenu* runMenu = _pMainMenu->addMenu(tr("&Run"));
    
    QAction* actionRun = runMenu->addAction(tr("&Run..."), this, &Notepad_plus::onRunCommand);
    actionRun->setShortcut(QKeySequence(tr("F5")));
    actionRun->setData(IDM_EXECUTE);
    
    // Settings menu
    QMenu* settingsMenu = _pMainMenu->addMenu(tr("&Settings"));
    
    QAction* actionPrefs = settingsMenu->addAction(tr("&Preferences..."), this, &Notepad_plus::onPreferences);
    actionPrefs->setData(IDM_SETTING_PREFERENCE);
    
    QAction* actionShortcutMapper = settingsMenu->addAction(tr("&Shortcut Mapper..."), this, &Notepad_plus::onShortcutMapper);
    actionShortcutMapper->setData(IDM_SETTING_SHORTCUT_MAPPER);
    
    // Help menu
    QMenu* helpMenu = _pMainMenu->addMenu(tr("&Help"));
    
    helpMenu->addAction(tr("&About Notepad++"), [this]() {
        _aboutDlg.show();
    })->setData(IDM_ABOUT);
    
    helpMenu->addAction(tr("&Documentation"), []() {
        QDesktopServices::openUrl(QUrl("https://npp-user-manual.org/"));
    });
    
    // Connect menu actions to command handler
    connect(_pMainMenu, &QMenuBar::triggered, [this](QAction* action) {
        if (action && action->data().isValid()) {
            int cmdId = action->data().toInt();
            if (cmdId > 0) {
                command(cmdId);
            }
        }
    });
}

// Setup toolbar
void Notepad_plus::setupToolBar() {
    _pToolBar = addToolBar(tr("Main Toolbar"));
    _pToolBar->setObjectName("MainToolBar");
    
    // Add toolbar buttons based on configuration
    NppParameters& nppParam = NppParameters::getInstance();
    bool showToolbar = nppParam.getNppGUI()._toolbarShow;
    _pToolBar->setVisible(showToolbar);
    
    // Add file operations
    _pToolBar->addAction(tr("New"), this, &Notepad_plus::onFileNew);
    _pToolBar->addAction(tr("Open"), this, &Notepad_plus::onFileOpen);
    _pToolBar->addAction(tr("Save"), this, &Notepad_plus::onFileSave);
    
    _pToolBar->addSeparator();
    
    // Add edit operations
    _pToolBar->addAction(tr("Cut"), this, &Notepad_plus::onEditCut);
    _pToolBar->addAction(tr("Copy"), this, &Notepad_plus::onEditCopy);
    _pToolBar->addAction(tr("Paste"), this, &Notepad_plus::onEditPaste);
    
    _pToolBar->addSeparator();
    
    // Add search
    _pToolBar->addAction(tr("Find"), this, &Notepad_plus::onSearchFind);
    _pToolBar->addAction(tr("Replace"), this, &Notepad_plus::onSearchReplace);
}

// Setup status bar
void Notepad_plus::setupStatusBar() {
    QStatusBar* sb = statusBar();
    
    // Document size
    sb->addPermanentWidget(new QLabel(tr("Ln 1, Col 1")), 0);
    
    // EOL format
    sb->addPermanentWidget(new QLabel(tr("UTF-8")), 0);
    
    // Encoding
    sb->addPermanentWidget(new QLabel(tr("Windows (CR LF)")), 0);
    
    // Type
    sb->addPermanentWidget(new QLabel(tr("INS")))->setObjectName("statusTypingMode");
}

// Kill all children
void Notepad_plus::killAllChildren() {
    // Destroy in reverse order of initialization
    
    // Save current session
    saveCurrentSession();
    
    // Destroy panels
    delete _pFuncList;
    _pFuncList = nullptr;
    delete _pDocMap;
    _pDocMap = nullptr;
    delete _pFileBrowser;
    _pFileBrowser = nullptr;
    delete _pProjectPanel_1;
    _pProjectPanel_1 = nullptr;
    delete _pProjectPanel_2;
    _pProjectPanel_2 = nullptr;
    delete _pProjectPanel_3;
    _pProjectPanel_3 = nullptr;
    delete _pDocumentListPanel;
    _pDocumentListPanel = nullptr;
    delete _pClipboardHistoryPanel;
    _pClipboardHistoryPanel = nullptr;
    delete _pAnsiCharPanel;
    _pAnsiCharPanel = nullptr;
    
    // Destroy scintilla views
    _fileEditView.destroy();
    _invisibleEditView.destroy();
    _subEditView.destroy();
    _mainEditView.destroy();
    
    // Destroy document tabs
    _subDocTab.destroy();
    _mainDocTab.destroy();
    
    // Destroy splitter
    if (_pMainSplitter) {
        _pMainSplitter->destroy();
        delete _pMainSplitter;
        _pMainSplitter = nullptr;
    }
    _subSplitter.destroy();
    
    // Destroy scintilla controls
    _scintillaCtrls4Plugins.destroy();
    
    // Destroy docking manager
    _dockingManager.destroy();
    
    // Destroy tray icon
    delete _pTrayIco;
    _pTrayIco = nullptr;
}

// Close event
void Notepad_plus::closeEvent(QCloseEvent* event) {
    NppParameters& nppParam = NppParameters::getInstance();
    
    // Check for unsaved changes
    bool hasDirty = false;
    for (size_t i = 0; i < MainFileManager.getNbBuffers(); ++i) {
        if (MainFileManager.getBufferByIndex(i)->isDirty()) {
            hasDirty = true;
            break;
        }
    }
    
    if (hasDirty) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
            tr("Notepad++"),
            tr("Some files have unsaved changes. Do you want to save them before closing?"),
            QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::SaveAll);
        
        if (reply == QMessageBox::SaveAll) {
            fileSaveAll();
        } else if (reply == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }
    
    // Save session
    saveCurrentSession();
    
    // Save GUI parameters
    saveGUIParams();
    
    // Kill all children
    killAllChildren();
    
    event->accept();
}

// Drag enter event for file drops
void Notepad_plus::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

// Drop event for file drops
void Notepad_plus::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        dropFiles(urls);
        event->acceptProposedAction();
    }
}

// Drop files handler
void Notepad_plus::dropFiles(const QList<QUrl>& urls) {
    for (const QUrl& url : urls) {
        QString filePath = url.toLocalFile();
        if (!filePath.isEmpty()) {
            doOpen(filePath.toStdWString());
        }
    }
}

// Key press event
void Notepad_plus::keyPressEvent(QKeyEvent* event) {
    // Handle accelerator keys
    if (processFindAccel(event)) {
        return;
    }
    
    if (processTabSwitchAccel(event)) {
        return;
    }
    
    // Let base class handle it
    QMainWindow::keyPressEvent(event);
}

// Resize event
void Notepad_plus::resizeEvent(QResizeEvent* event) {
    // Notify docking manager
    QRect rc = rect();
    _dockingManager.resize(rc);
    
    // Update document map if visible
    if (_pDocMap) {
        // doMove() - no equivalent in Qt, resize handles it
        _pDocMap->reloadMap();
    }
    
    QMainWindow::resizeEvent(event);
}

// Update status bar
void Notepad_plus::updateStatusBar() {
    if (!_pStatusBar) return;
    
    Buffer* curBuf = _pEditView->getCurrentBuffer();
    if (!curBuf) return;
    
    // Line and column
    int line = _pEditView->getCurrentLineNumber() + 1;
    int col = _pEditView->getCurrentColumnNumber() + 1;
    QString posText = QString("Ln %1, Col %2").arg(line).arg(col);
    
    // Document size
    int64_t docLength = curBuf->docLength();
    QString sizeText;
    if (docLength < 1024) {
        sizeText = QString::number(docLength) + " chars";
    } else if (docLength < 1024 * 1024) {
        sizeText = QString::number(docLength / 1024.0, 'f', 1) + " KB";
    } else {
        sizeText = QString::number(docLength / (1024.0 * 1024.0), 'f', 1) + " MB";
    }
    
    // EOL format
    EolType eol = curBuf->getEolFormat();
    QString eolText;
    switch (eol) {
        case EolType::windows: eolText = "Windows (CR LF)"; break;
        case EolType::unix: eolText = "Unix (LF)"; break;
        case EolType::mac: eolText = "Macintosh (CR)"; break;
        default: eolText = "Mixed"; break;
    }
    
    // Encoding
    UniMode encoding = curBuf->getUnicodeMode();
    QString encText;
    switch (encoding) {
        case UniMode::uni8Bit: encText = QString::fromLatin1(curBuf->getEncodingLabel()); break;
        case UniMode::uniUTF8: encText = "UTF-8"; break;
        case UniMode::uniUTF8Bom: encText = "UTF-8-BOM"; break;
        case UniMode::uniUTF16LE: encText = "UTF-16 LE"; break;
        case UniMode::uniUTF16BE: encText = "UTF-16 BE"; break;
        default: encText = "ANSI"; break;
    }
    
    // Update status