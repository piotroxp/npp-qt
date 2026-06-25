// npp-qt: Qt6 port of Notepad++ NppCommands
// Semantic lift from: PowerEditor/src/NppCommands.cpp
// NppCommands bridges all menu actions to slots in the Qt port

#pragma once

#include <QObject>
#include <QMenu>
#include <QMenuBar>
#include <QKeySequence>
#include <QVector>
#include <memory>
#include "functionParser.h"  // ScintillaEditView alias (using ScintillaComponent)

// Forward declarations
class Notepad_plus;
class NppParameters;
class MainWindow;

/**
 * NppCommands — bridges menu actions to Notepad_plus commands
 *
 * In the Qt port, all menu actions (File, Edit, Search, View, etc.)
 * are connected to NppCommands slots via Qt's signal/slot mechanism.
 *
 * The Win32 switch(id) in NppCommands::command() is replaced by individual
 * named slots that are connected to QAction signals.
 */
class NppCommands : public QObject
{
    Q_OBJECT

public:
    explicit NppCommands(QObject* parent = nullptr);
    ~NppCommands();

    // Initialize connections between menu actions and command slots
    void setMainWindow(MainWindow* mainWindow);

    // Public command interface (called from menu actions)
public slots:
    // === File commands ===
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
    void filePrint();
    void fileExit();
    void fileRestoreLastClosed();

    // === Edit commands ===
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

    // === Search commands ===
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

    // === View commands ===
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

    // === Encoding commands ===
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

    // === Language commands ===
    void languageMenu();
    void languageUserDefine();
    void languageOpenUdldDir();

    // === Settings commands ===
    void settingsPreference();
    void settingsStyleConfig();
    void settingsShortcutMapper();
    void settingsImportPlugin();
    void settingsImportStyleThemes();
    void settingsEditContextMenu();

    // === Macro commands ===
    void macroStartRecording();
    void macroStopRecording();
    void macroPlayback();
    void macroSaveCurrent();
    void macroRunMultiple();

    // === Run commands ===
    void runExecute();
    void runCmdLineArgs();

    // === Window commands ===
    void windowSwitchToDocument(int docIndex);
    void windowSortAscending();
    void windowSortDescending();

    // === Help commands ===
    void helpAbout();
    void helpDocumentation();
    void helpHomeSweetHome();
    void helpOnlineDoc();

    // === Column editor commands ===
    void columnEditor();

    // === Clipboard commands ===
    void clipboardHistoryPanel();
    void charPanel();

    // === Panel commands ===
    void toggleDocumentMap();
    void toggleFunctionList();
    void toggleFileBrowser();
    void toggleProjectPanel1();
    void toggleProjectPanel2();
    void toggleProjectPanel3();
    void toggleClipboardHistory();
    void toggleDocList();

    // === Find/Replace panel commands ===
    void findInFiles();
    void findInProjects();
    void findInFilelist();
    void replaceInFiles();
    void replaceInProjects();
    void replaceInFilelist();
    void findAllInOpenedDocs();
    void replaceAllInOpenedDocs();

    // === Toggle commands ===
    void toggleReadOnly();
    void setReadOnlyForAll();
    void clearReadOnlyForAll();
    void toggleRecording();
    void toggleMacroPlayback();
    void toggleMonitoring();

signals:
    // Emitted when a command is executed
    void commandExecuted(int commandId);

    // Emitted when focus changes between views
    void focusChanged(bool mainViewActive);

    // Emitted for macro recording state changes
    void macroRecordingStateChanged(bool isRecording);

private:
    // Helper: get the active Scintilla view
    ScintillaEditView* activeView();

    // Helper: check if a file is modified
    bool isCurrentFileModified() const;

    // Helper: show save confirmation dialog
    bool confirmSave(const QString& fileName);

    // Helper: update status bar
    void updateStatusBar();

    MainWindow* _mainWindow = nullptr;
    bool _isMacroRecording = false;
    std::vector<QKeySequence> _recentSearches;
};
