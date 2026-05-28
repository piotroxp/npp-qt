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

// Qt6 Translation - Notepad_plus.h
// Translated from Win32 MFC to Qt6

#pragma once
#define SC_CP_UTF8 65001

#include "WindowsCompat.h"
#include "Win32QtShim.h"
#include "NppConstants.h"
#include "ScintillaComponent/Buffer.h"
#include "WinControls/DockingWnd/Docking.h"
#include "WinControls/Grid/ShortcutMapper.h"
#include "AutoCompletion.h"
#include "SmartHighlighter.h"
#include "NativeLangSpeaker.h"
#include "LocalizationSwitcher.h"
#include "ScintillaComponent/DocTabView.h"
#include "ScintillaEditView.h"
#include "WinControls/SplitterContainer/Splitter.h"
#include "WinControls/ContextMenu/ContextMenu.h"
#include "ScintillaComponent/FindReplaceDlg.h"
#include "WinControls/AboutDlg/AboutDlg.h"
#include "WinControls/StaticDialog/RunDlg/RunDlg.h"
#include "ScintillaComponent/GoToLineDlg.h"
#include "ScintillaComponent/ColumnEditorDlg.h"
#include "WinControls/ColourPicker/WordStyleDlg.h"
#include "WinControls/Preference/preferenceDlg.h"
#include "WinControls/PluginsAdmin/pluginsAdmin.h"
#include "ScintillaComponent/DocumentPeeker.h"
#include "LastRecentFileList.h"
#include "WinControls/StatusBar/StatusBar.h"
#include "WinControls/ToolBar/ToolBar.h"
#include "WinControls/TabBar/TabBar.h"
#include "WinControls/StaticDialog/StaticDialog.h"
#include "WinControls/DocumentMap/documentMap.h"
#include "ScintillaComponent/ScintillaCtrls.h"
#include "ScintillaComponent/columnEditor.h"
#include "ScintillaComponent/FindReplaceDlg.h"
#include "ScintillaAccelerator.h"
#include "MISC/PluginsManager/PluginsManager.h"
#include "MISC/PluginsManager/PluginInterface.h"
#include "WinControls/PluginsAdmin/pluginsAdmin.h"
#include "WinControls/shortcut/shortcut.h"
#include "WinControls/shortcut/RunMacroDlg/RunMacroDlg.h"
#include "WinControls/Grid/ShortcutMapper.h"
#include "Macro.h"

// Forward declarations for dialogs - include their headers as needed
class RunMacroDlg;
class ShortcutMapper;
class ScintillaAccelerator;
class PluginsAdmin;
class columnEditor;


#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QSettings>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QtGui/QCloseEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QResizeEvent>
#include <QtCore/QWaitCondition>
#include <QtCore/QProcess>
#include <vector>
#include <memory>
#include <chrono>
#include <atomic>
class LocalizationSwitcher;
class Buffer;
class NppParameters;
class TaskListInfo;
class Macro;
class MacroShortcut;
class recordedMacroStep;
class Session;
class BufferViewInfo;
class FindersInfo;
class Finder;
class CustomFileDialog;
class DocumentPeeker;

// Constants matching original
#define MENU 0x01
#define TOOLBAR 0x02
#define MAIN_EDIT_ZONE true

// File transfer mode for cloning/moving documents
enum class FileTransferMode {
    TransferClone = 0x01,
    TransferMove = 0x02
};

// Window status bit flags
enum class WindowStatus : uint8_t {
    WindowMainActive = 0x01,
    WindowSubActive = 0x02,
    WindowBothActive = 0x03,
    WindowUserActive = 0x04,
    WindowMask = 0x07
};

// Trim operation types
enum class TrimOp {
    lineHeader = 0,
    lineTail = 1,
    lineBoth = 2,
    lineEol = 3
};

// Space/Tab conversion
enum class SpaceTab {
    tab2Space = 0,
    space2TabLeading = 1,
    space2TabAll = 2
};

// Comment modes
enum class CommentMode {
    cm_comment,
    cm_uncomment,
    cm_toggle
};

// Visible GUI configuration
struct VisibleGUIConf {
    bool _isPostIt = false;
    bool _isFullScreen = false;
    bool _isDistractionFree = false;
    bool _isMenuShown = true;
    bool _isTabbarShown = true;
    bool _isAlwaysOnTop = false;
    bool _isStatusbarShown = true;
    bool _was2ViewModeOn = false;
    std::vector<void*> _pVisibleDockingContainers;
};

// Quote parameters
struct QuoteParams {
    enum class Speed { slow = 0, rapid, speedOfLight };
    
    QuoteParams() = default;
    QuoteParams(const wchar_t* quoter, Speed speed, bool shouldBeTrolling, 
                int encoding, int lang, const wchar_t* quote);
    
    void reset();
    
    const wchar_t* _quoter = nullptr;
    Speed _speed = Speed::rapid;
    bool _shouldBeTrolling = false;
    int _encoding = SC_CP_UTF8;
    int _lang = 0;
    const wchar_t* _quote = nullptr;
};

class Notepad_plus_Window;
class Notepad_plus : public QMainWindow {
    friend class Notepad_plus_Window;
    friend class MainWindow;
    friend class FileManager;

Q_OBJECT

public:
    Notepad_plus();
    ~Notepad_plus();

    // Initialization and message processing (Qt slots)
    bool init();
    void processCommand(int id);
    void killAllChildren();

    // View accessors
    ScintillaEditView* getCurrentEditView() const { return (_activeView == MAIN_VIEW) ? const_cast<ScintillaEditView*>(&_mainEditView) : const_cast<ScintillaEditView*>(&_subEditView); }
    ScintillaEditView* getNonCurrentEditView() const { return (_activeView == MAIN_VIEW) ? const_cast<ScintillaEditView*>(&_subEditView) : const_cast<ScintillaEditView*>(&_mainEditView); }
    ScintillaEditView& getMainEditView() { return _mainEditView; }
    ScintillaEditView& getSubEditView() { return _subEditView; }

    // Document operations
    enum class comment_mode { cm_comment, cm_uncomment, cm_toggle };
    
    void setTitle();
    void getTaskListInfo(void* tli);
    int getNbDirtyBuffer(int view);

    // Single buffer operations
    BufferID doOpen(const std::wstring& fileName, bool isRecursive = false, 
                   bool isReadOnly = false, int encoding = -1,
                   const wchar_t* backupFileName = nullptr,
                   uint64_t fileNameTimestamp = 0);
    bool doReload(BufferID id, bool alert = true);
    bool doSave(BufferID id, const wchar_t* filename, bool isSaveCopy = false);
    void doClose(BufferID id, int whichOne, bool doDeleteBackup = false);

    // File menu operations
    void fileOpen();
    void fileNew();
    bool fileReload();
    bool fileClose(BufferID id = BUFFER_INVALID, int curView = -1);
    bool fileCloseAll(bool doDeleteBackup, bool isSnapshotMode = false);
    bool fileCloseAllButCurrent();
    void fileCloseAllButPinned();
    bool fileCloseAllGiven(const std::vector<BufferViewInfo>& krvecBuffer);
    bool fileCloseAllToLeft();
    bool fileCloseAllToRight();
    bool fileCloseAllUnchanged();
    bool fileSave(BufferID id = BUFFER_INVALID);
    bool fileSaveAllConfirm();
    bool fileSaveAll();
    bool fileSaveSpecific(const std::wstring& fileNameToSave);
    bool fileSaveAs(BufferID id = BUFFER_INVALID, bool isSaveCopy = false);
    bool fileDelete(BufferID id = BUFFER_INVALID);
    bool fileRename(BufferID id = BUFFER_INVALID);
    bool fileRenameUntitledPluginAPI(BufferID id, const wchar_t* tabNewName);
    bool useFirstLineAsTabName(BufferID id);

    void unPinnedForAllBuffers();
    bool switchToFile(BufferID buffer);
    
    // Session operations
    bool isFileSession(const wchar_t* filename);
    bool isFileWorkspace(const wchar_t* filename);
    void filePrint(bool showDialog);
    void saveScintillasZoom();

    // Save settings
    bool saveGUIParams();
    bool saveProjectPanelsParams();
    bool saveFileBrowserParam();
    bool saveColumnEditorParams();
    void saveDockingParams();
    void saveUserDefineLangs();
    void saveShortcuts();
    void saveSession(const Session& session);
    void saveCurrentSession();
    void saveFindHistory();

    void getCurrentOpenedFiles(Session& session, bool includeUntitledDoc = false);

    bool fileLoadSession(const wchar_t* fn = nullptr);
    const wchar_t* fileSaveSession(size_t nbFile, wchar_t** fileNames, 
                                   const wchar_t* sessionFile2save, 
                                   bool includeFileBrowser = false);
    const wchar_t* fileSaveSession(size_t nbFile = 0, wchar_t** fileNames = nullptr);

    // Comment operations
    bool doBlockComment(comment_mode currCommentMode);
    bool doStreamComment();
    bool undoStreamComment(bool tryBlockComment = true);

    // Macro operations
    bool addCurrentMacro();
    void macroPlayback(Macro macro, std::vector<void*>* pDocs4EndUAIn = nullptr);
    
    void loadLastSession();
    bool loadSession(Session& session, bool isSnapshotMode = false, 
                    const wchar_t* userCreatedSessionName = nullptr);

    // Dialog preparation
    void prepareBufferChangedDialog(Buffer* buffer);
    void notifyBufferChanged(Buffer* buffer, int mask);
    bool findInFinderFiles(FindersInfo* findInFolderInfo);

    // Find/Replace operations
    bool createFilelistForFiles(std::vector<std::wstring>& fileNames);
    bool createFilelistForProjects(std::vector<std::wstring>& fileNames);
    bool findInFiles();
    bool findInProjects();
    bool findInFilelist(std::vector<std::wstring>& fileList);
    bool replaceInFiles();
    bool replaceInProjects();
    bool replaceInFilelist(std::vector<std::wstring>& fileList);

    void setFindReplaceFolderFilter(const wchar_t* dir, const wchar_t* filters);
    std::vector<std::wstring> addNppComponents(const wchar_t* destDir, 
                                                const wchar_t* extFilterName, 
                                                const wchar_t* extFilter);
    std::vector<std::wstring> addNppPlugins(const wchar_t* extFilterName, 
                                            const wchar_t* extFilter);
    int getHtmlXmlEncoding(const wchar_t* fileName) const;

    // Buffer access
    Buffer* getCurrentBuffer() {
        return getCurrentEditView()->getCurrentBuffer();
    }

    // Quote features
    void launchDocumentBackupTask();
    int getQuoteIndexFrom(const wchar_t* quoter) const;
    void showQuoteFromIndex(int index) const;
    void showQuote(const QuoteParams* quote) const;

    std::wstring getPluginListVerStr() const {
        return L"1.0";  // Stub: no version string available without plugins
    }

    // UI management
    void minimizeDialogs();
    void restoreMinimizeDialogs();
    void refreshDarkMode(bool resetStyle = false);
    void refreshInternalPanelIcons();
    void changeReadOnlyUserModeForAllOpenedTabs(const bool ro);

    // Recent files management (MRU)
    LastRecentFileList& getRecentFiles() { return _lastRecentFileList; }
    
    // Menu actions - Qt slots
public slots:
    // File menu actions
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileSaveAll();
    void onFileClose();
    void onFileCloseAll();
    void onFileCloseAllButCurrent();
    void onFilePrint();
    void onFileExit();
    
    // Edit menu actions
    void onEditUndo();
    void onEditRedo();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditDelete();
    void onEditSelectAll();
    void onEditFind();
    void onEditReplace();
    
    // Search menu actions
    void onSearchFindNext();
    void onSearchFindPrev();
    void onSearchReplace();
    void onSearchGoToLine();
    
    // View menu actions
    void onViewZoomIn();
    void onViewZoomOut();
    void onViewFullScreen();
    void onViewPostIt();
    
    // Macro menu actions
    void onMacroStartRecord();
    void onMacroStopRecord();
    void onMacroPlayRecord();
    
    // Run menu actions
    void onRunCommand();
    
    // Language menu
    void onLanguageChanged(int langType);
    
    // Settings menu
    void onPreferences();
    void onShortcutMapper();

signals:
    // NPPM_ signals for plugin API
    void nppBufferActivated(BufferID id, int view);
    void nppFileOpened(const QString& filePath);
    void nppFileClosed(BufferID id);
    void nppFileSaved(BufferID id);
    void nppDarkModeChanged(bool isDark);
    void nppLanguageChanged(int langType);
    void nppDocOrderChanged(int newOrder);
    
    // Status bar updates
    void statusBarTextChanged(const QString& text, int part);
    void statusBarDocTypeChanged(const QString& docType);
    void statusBarCurPosChanged(int position);
    void statusBarEolFormatChanged(const QString& format);
    void statusBarUnicodeTypeChanged(const QString& unicodeType);
    
    // File change notification
    void fileChangedOnDisk(const QString& filePath);

    // Signals for Scintilla view configuration
    void nppInternalSetCaretWidth(int width);
    void nppInternalSetCaretBlinkRate(int rate);
    void nppInternalEdgeMultiSetSize(int size);
    void nppInternalSetMultiSelection(int flags);

protected:
    // Qt event handlers
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    
    // Focus handling
    void focusInEvent(QFocusEvent* event) override;

private:
    Notepad_plus_Window* _pPublicInterface = nullptr;
    QWidget* _pMainWindow = nullptr;
    
    // Dock manager
    DockingManager _dockingManager;
    std::vector<int> _internalFuncIDs;

    // Auto completion
    AutoCompletion _autoCompleteMain;
    AutoCompletion _autoCompleteSub;

    // Smart highlighter
    SmartHighlighter _smartHighlighter;
    
    // Language support
    NativeLangSpeaker _nativeLangSpeaker;
    LocalizationSwitcher _localizationSwitcher;

    // Document tabs
    DocTabView _mainDocTab;
    DocTabView _subDocTab;
    DocTabView* _pDocTab = nullptr;
    DocTabView* _pNonDocTab = nullptr;

    // Scintilla editors
    // View identifiers (defined in Notepad_plus_msgs.h via PluginInterface.h)
#define MAIN_VIEW 0
#define SUB_VIEW 1

    ScintillaEditView _mainEditView;
    ScintillaEditView _subEditView;
    ScintillaEditView _invisibleEditView;
    ScintillaEditView _fileEditView;
    ScintillaEditView* _pEditView = nullptr;
    ScintillaEditView* _pNonEditView = nullptr;
    int _activeView = 0;

    // Splitters
    SplitterContainer* _pMainSplitter = nullptr;
    SplitterContainer _subSplitter;

    // Context menus
    ContextMenu _tabPopupMenu;
    ContextMenu _tabPopupDropMenu;
    ContextMenu _fileSwitcherMultiFilePopupMenu;

    // Toolbar
    QToolBar* _pToolBar = nullptr;
    
    // Status bar
    QStatusBar* _pStatusBar = nullptr;

    // ReBar (toolbar bands)
    void* _pRebarTop = nullptr;
    void* _pRebarBottom = nullptr;

    // Dialogs
    FindReplaceDlg _findReplaceDlg;
    FindReplaceDlg _findInFinderDlg;
    FindReplaceDlg _incrementFindDlg;
    AboutDlg _aboutDlg;
    void* _debugInfoDlg = nullptr;
    void* _cmdLineArgsDlg = nullptr;
    RunDlg _runDlg;
    void* _md5FromFilesDlg = nullptr;
    void* _md5FromTextDlg = nullptr;
    void* _sha2FromFilesDlg = nullptr;
    void* _sha2FromTextDlg = nullptr;
    void* _sha1FromFilesDlg = nullptr;
    void* _sha1FromTextDlg = nullptr;
    void* _sha512FromFilesDlg = nullptr;
    void* _sha512FromTextDlg = nullptr;
    GoToLineDlg _goToLineDlg;
    columnEditor _colEditorDlg;
    WordStyleDlg _configStyleDlg;
    PreferenceDlg _preference;
    void* _findCharsInRangeDlg = nullptr;
    PluginsAdmin _pluginsAdminDlg;
    DocumentPeeker _documentPeeker;

    // List of open dialogs
    std::vector<QWidget*> _modelessDlgs;

    // Recent files
    LastRecentFileList _lastRecentFileList;

    // Windows menu
    void* _pWindowsMenu = nullptr;
    QMenuBar* _pMainMenu = nullptr;

    bool _sysMenuEntering = false;
    bool _isAttemptingCloseOnQuit = false;

    // FullScreen/PostIt/DistractionFree
    VisibleGUIConf _beforeSpecialView;
    void fullScreenToggle();
    void postItToggle();
    void distractionFreeToggle();

    // Macro recording
    Macro _macro;
    bool _recordingMacro = false;
    bool _playingBackMacro = false;
    bool _recordingSaved = false;
    RunMacroDlg _runMacroDlg;

    // Shortcut mapper
    ShortcutMapper* _pShortcutMapper = nullptr;

    // Selection tracking
    bool _linkTriggered = true;
    bool _isFolding = false;
    void* _prevSelectedRange = nullptr;

    // Synchronized scrolling
    struct SyncInfo {
        intptr_t _line = 0;
        intptr_t _column = 0;
        bool _isSynScrollV = false;
        bool _isSynScrollH = false;
        bool doSync() const { return _isSynScrollV || _isSynScrollH; }
    } _syncInfo;

    bool _isUDDocked = false;

    // System tray
    void* _pTrayIco = nullptr;
    intptr_t _zoomOriginalValue = 0;

    // Accelerators
    Accelerator _accelerator;
    ScintillaAccelerator _scintaccelerator;

    // Plugin manager
    PluginsManager _pluginsManager;
    void* _pRestoreButton = nullptr;

    bool _isFileOpening = false;
    bool _isAdministrator = false;
    bool _isSyncingZoom = false;
    bool _isNppSessionSavedAtExit = false;

    // Scintilla controls for plugins
    ScintillaCtrls _scintillaCtrls4Plugins;

    std::vector<std::pair<int, int>> _hideLinesMarks;
    StyleArray _hotspotStyles;

    // Panels
    void* _pAnsiCharPanel = nullptr;
    void* _pClipboardHistoryPanel = nullptr;
    void* _pDocumentListPanel = nullptr;
    void* _pProjectPanel_1 = nullptr;
    void* _pProjectPanel_2 = nullptr;
    void* _pProjectPanel_3 = nullptr;
    void* _pFileBrowser = nullptr;
    DocumentMap* _pDocMap = nullptr;
    void* _pFuncList = nullptr;

    // File system watcher for monitoring
    QFileSystemWatcher* _pFileWatcher = nullptr;

    // Internal state
    uint8_t _mainWindowStatus = 0;
    int _multiSelectFlag = 0;

    // Document management private methods
    void dockUserDlg();
    void undockUserDlg();
    
    void showView(int whichOne);
    bool viewVisible(int whichOne);
    void hideView(int whichOne);
    void hideCurrentView();
    bool bothActive() { return (_mainWindowStatus & static_cast<uint8_t>(WindowStatus::WindowBothActive)) == static_cast<uint8_t>(WindowStatus::WindowBothActive); }
    bool reloadLang();
    bool loadStyles();

    int currentView() { return _activeView; }
    int otherView() { return (_activeView == MAIN_VIEW) ? SUB_VIEW : MAIN_VIEW; }
    int otherFromView(int whichOne) { return (whichOne == MAIN_VIEW) ? SUB_VIEW : MAIN_VIEW; }
    bool canHideView(int whichOne);
    bool isEmpty();
    int switchEditViewTo(int gid);
    void docGotoAnotherEditView(FileTransferMode mode);
    void docOpenInNewInstance(FileTransferMode mode, int x = 0, int y = 0);

    void loadBufferIntoView(BufferID id, int whichOne, bool dontClose = false);
    bool removeBufferFromView(BufferID id, int whichOne);
    bool activateBuffer(BufferID id, int whichOne, bool forceApplyHilite = false);
    void notifyBufferActivated(BufferID bufid, int view);
    void performPostReload(int whichOne);

    // Save dialogs
    int doSaveOrNot(const wchar_t* fn, bool isMulti = false);
    int doReloadOrNot(const wchar_t* fn, bool dirty);
    int doCloseOrNot(const wchar_t* fn);
    int doDeleteOrNot(const wchar_t* fn);
    int doSaveAll();

    // Menu state management
    void enableMenu(int cmdID, bool doEnable) const;
    void enableCommand(int cmdID, bool doEnable, int which) const;
    void checkClipboard();
    void checkDocState();
    void checkUndoState();
    void checkMacroState();
    void checkSyncState();
    void syncZoom();
    void setupColorSampleBitmapsOnMainMenuItems();
    void dropFiles(const QList<QUrl>& urls);
    void checkModifiedDocument(bool bCheckOnlyCurrentBuffer);

    void getMainClientRect(QRect& rc) const;
    void staticCheckMenuAndTB() const;
    void dynamicCheckMenuAndTB() const;
    void enableConvertMenuItems(int f) const;
    void checkUnicodeMenuItems() const;

    // Language operations
    std::wstring getLangDesc(int langType, bool getName = false);
    void setLangStatus(int langType);
    void setDisplayFormat(int f);
    void setUniModeText();
    void checkLangsMenu(int id) const;
    void setLanguage(int langType);
    int menuID2LangType(int cmdID);

    // Selection and search
    bool processIncrFindAccel(const QKeyEvent* event);
    bool processFindAccel(const QKeyEvent* event);
    bool processTabSwitchAccel(const QKeyEvent* event);

    void checkMenuItem(int itemID, bool willBeChecked);
    bool isConditionExprLine(intptr_t lineNumber);
    intptr_t findMachedBracePos(size_t startPos, size_t endPos, char targetSymbol, char matchedSymbol);
    void maintainIndentation(QChar ch);

    void addHotSpot(ScintillaEditView* view = nullptr);
    void removeAllHotSpot();

    // Bookmark operations
    void bookmarkAdd(intptr_t lineno = -1);
    void bookmarkDelete(size_t lineno = static_cast<size_t>(-1));
    bool bookmarkPresent(intptr_t lineno = -1);
    void bookmarkToggle(intptr_t lineno = -1);
    void bookmarkNext(bool forwardScan);
    void bookmarkClearAll();

    // Line operations
    void copyMarkedLines();
    void cutMarkedLines();
    void deleteMarkedLines(bool isMarked);
    void pasteToMarkedLines();
    void deleteMarkedline(size_t ln);
    void inverseMarks();
    void replaceMarkedline(size_t ln, const wchar_t* str);
    std::wstring getMarkedLine(size_t ln);
    void findMatchingBracePos(intptr_t& braceAtCaret, intptr_t& braceOpposite);
    bool braceMatch();

    // Document navigation
    void activateNextDoc(bool direction);
    void activateDoc(size_t pos);

    // Status bar updates
    void updateStatusBar();
    size_t getSelectedCharNumber(int uniMode);
    size_t getCurrentDocCharCount(int u);
    size_t getSelectedAreas();
    size_t getSelectedBytes();
    bool isFormatUnicode(int uniMode);
    int getBOMSize(int uniMode);

    // Auto-completion
    void showAutoComp();
    void autoCompFromCurrentFile(bool autoInsert = true);
    void showFunctionComp();
    void showPathCompletion();
    void showFunctionNextHint(bool isNext = true);

    // File operations
    void setCodePageForInvisibleView(const Buffer* pBuffer);
    bool replaceInOpenedFiles();
    bool findInOpenedFiles();
    bool findInCurrentFile(bool isEntireDoc);

    void getMatchedFileNames(const wchar_t* dir, size_t level, 
                             const std::vector<std::wstring>& patterns,
                             std::vector<std::wstring>& fileNames,
                             bool isRecursive, bool isInHiddenDir);
    void doSynScroll(QWidget* hW);
    void setWorkingDir(const wchar_t* dir);

    // UI helpers
    bool getIntegralDockingData(DockedWidgetData& dockData, int& iCont, bool& isVisible);
    int getLangFromMenuName(const wchar_t* langName);
    std::wstring getLangFromMenu(const Buffer* buf);

    std::wstring exts2Filters(const std::wstring& exts, int maxExtsLen = -1);
    int setFileOpenSaveDlgFilters(CustomFileDialog& fDlg, bool showAllExt, int langType = -1);
    Style* getStyleFromName(const wchar_t* styleName);
    bool dumpFiles(const wchar_t* outdir, const wchar_t* fileprefix = L"");
    void drawTabbarColoursFromStylerArray();
    void drawAutocompleteColoursFromTheme(COLORREF fgColor, COLORREF bgColor);
    void drawDocumentMapColoursFromStylerArray();

    std::vector<std::wstring> loadCommandlineParams(const wchar_t* commandLine, 
                                                     const void* pCmdParams);
    bool noOpenedDoc();
    bool goToPreviousIndicator(int indicID2Search, bool isWrap = true) const;
    bool goToNextIndicator(int indicID2Search, bool isWrap = true) const;
    int wordCount();

    // Whitespace operations
    void wsTabConvert(SpaceTab whichWay);
    void doTrim(TrimOp whichPart);
    void eol2ws();
    void removeEmptyLine(bool isBlankContained);
    void removeDuplicateLines();

    // Panel launchers
    void launchAnsiCharPanel();
    void launchClipboardHistoryPanel();
    void launchDocumentListPanel(bool changeFromBtnCmd = false);
    void changeDocumentListIconSet(bool changeFromBtnCmd);
    void checkProjectMenuItem();
    void launchProjectPanel(int cmdID, void** ppProjPanel, int panelID);
    void launchDocMap();
    void launchFunctionList();
    void launchFileBrowser(const std::vector<std::wstring>& folders, 
                          const std::wstring& selectedItemPath, 
                          bool fromScratch = false);
    void showAllQuotes();

    // Background threads
    static DWORD WINAPI threadTextPlayer(void* text2display);
    static DWORD WINAPI threadTextTroller(void* params);
    static int getRandomAction(int ranNum);
    static bool deleteBack(ScintillaEditView* pCurrentView, BufferID targetBufID);
    static bool deleteForward(ScintillaEditView* pCurrentView, BufferID targetBufID);
    static bool selectBack(ScintillaEditView* pCurrentView, BufferID targetBufID);
    static int getRandomNumber(int rangeMax = -1);

    // Backup thread
    // File monitoring is handled via QFileSystemWatcher (see _pFileWatcher member)
    // The old Win32 CreateThread/monitorFileOnChange approach is deprecated
    struct MonitorInfo {
        MonitorInfo(Buffer* buf, QWidget* nppHandle);
        Buffer* _buffer = nullptr;
        QWidget* _nppHandle = nullptr;
    };

    void monitoringStartOrStopAndUpdateUI(Buffer* pBuf, bool isStarting);
    void createMonitoringThread(Buffer* pBuf);
    void updateCommandShortcuts();

    // Icon generation
    QPixmap generateSolidColourMenuItemIcon(COLORREF colour);

    // History operations
    void clearChangesHistory(int iView);
    void changedHistoryGoTo(int idGoTo);

    // Menu creation
    QMenu* createMenuFromMenu(QMenu* hSourceMenu, const std::vector<int>& commandIds);
    bool notifyTBShowMenu(void* lpnmtb, const char* menuPosId);
    bool notifyTBShowMenu(void* lpnmtb, const char* menuPosId, 
                          const std::vector<int>& cmdIDs);

    int getIcoID(void* panel);
    void loadPanelIcon(void* hInst, void* panel, void* phIcon);
    void refreshPanelIcon(void* hInst, void* panel);

    // Scintilla notification handler
    bool notify(void* notification);
    
    // Command handler (from menu/toolbar)
    void command(int id);
    
    // Menu bar and toolbar setup
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    
    // Restore saved session
    void loadBufferIntoView(BufferID id, int whichOne);
};


// Stub definitions for remaining incomplete types to allow compilation
#ifndef STUB_CLASSES_DEFINED
#define STUB_CLASSES_DEFINED
class PluginsAdminDlg : public QDialog {
    Q_OBJECT
public:
    PluginsAdminDlg(QWidget* p = nullptr) : QDialog(p) {}
};
#endif  // STUB_CLASSES_DEFINED

