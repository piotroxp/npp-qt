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

// Qt6 port — NppCommands.h
// Win32→Qt6 translation of PowerEditor/src/NppCommands.h
// Bridges all menu actions (File, Edit, Search, View, Encoding, Language,
// Settings, Macro, Run, Window, Help) to Qt slots.

#include <QObject>
#include <QString>
#include <QQueue>
#include <QVector>

#include "NppConstants.h"
#include "ScintillaComponent.h"
#include "MISC/Common/shortcut.h"

// Forward declarations — do NOT include MainWindow.h (it includes NppCommands.h, creating a cycle)
class MainWindow;
class Buffer;

// =============================================================================
// NppCommands — menu command bridge
// Each slot is connected to a Qt QAction signal from the menu system.
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

    // FILE
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
    bool filePrint();
    void fileExit();
    void fileRestoreLastClosed();

    // EDIT
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

    // SEARCH
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

    // VIEW
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

    // ENCODING
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

    // LANGUAGE
    void languageMenu();
    void languageUserDefine();
    void languageOpenUdldDir();

    // SETTINGS
    void settingsPreference();
    void settingsStyleConfig();
    void settingsShortcutMapper();
    void settingsImportPlugin();
    void settingsImportStyleThemes();
    void settingsEditContextMenu();

    // MACRO
    void macroStartRecording();
    void macroStopRecording();
    void macroStartStopRecord();
    void macroPlayback();
    void macroSaveCurrent();
    void macroRunMultiple();
    void runMacroFromDialog(int macroIndex, int times);

    // Internal helpers
    void startMacroRecording();
    void stopMacroRecording();
    void recordMacroStep(int message, uintptr_t wParam, uintptr_t lParam);
    void recordMacroStepMenu(int commandId);
    void playMacro(const Macro& steps);
    void macroRunMultipleMacrosDialog();

    // RUN
    void runExecute();
    void runCmdLineArgs();

    // WINDOW
    void windowSwitchToDocument(int docIndex);
    void windowSortAscending();
    void windowSortDescending();

    // HELP
    void helpAbout();
    void helpDocumentation();
    void helpHomeSweetHome();
    void helpOnlineDoc();

    // COLUMN EDITOR
    void columnEditor();

    // CLIPBOARD
    void clipboardHistoryPanel();
    void charPanel();

    // PANEL
    void toggleDocumentMap();
    void toggleFunctionList();
    void toggleFileBrowser();
    void toggleProjectPanel1();
    void toggleProjectPanel2();
    void toggleProjectPanel3();
    void toggleClipboardHistory();
    void toggleDocList();

    // FIND/REPLACE PANEL
    void findInFiles();
    void findInProjects();
    void findInFilelist();
    void replaceInFiles();
    void replaceInProjects();
    void replaceInFilelist();
    void findAllInOpenedDocs();
    void replaceAllInOpenedDocs();

    // TOGGLE
    void toggleReadOnly();
    void setReadOnlyForAll();
    void clearReadOnlyForAll();
    void toggleRecording();
    void toggleMacroPlayback();
    void toggleMonitoring();

signals:
    void macroRecordingStateChanged(bool isRecording);

private:
    // Internal encoding conversion helper
    void convertDocEncoding(UniMode newMode);

    // Add a closed file path to the recent-closed queue
    void pushRecentClosed(const QString& path);

    MainWindow* _mainWindow = nullptr;
    bool _playingBackMacro = false;

    // Macro recording state
    bool _isMacroRecording = false;
    QString _currentMacroName;
    QVector<RecordedMacroStep> _currentMacroSteps;
    QVector<RecordedMacroStep> _lastMacroSteps;

    // Recent closed files (for fileRestoreLastClosed)
    QQueue<QString> _recentClosedFiles;
};
