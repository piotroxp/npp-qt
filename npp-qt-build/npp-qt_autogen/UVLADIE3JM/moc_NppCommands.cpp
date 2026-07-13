/****************************************************************************
** Meta object code from reading C++ file 'NppCommands.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/NppCommands.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NppCommands.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11NppCommandsE_t {};
} // unnamed namespace

template <> constexpr inline auto NppCommands::qt_create_metaobjectdata<qt_meta_tag_ZN11NppCommandsE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NppCommands",
        "commandExecuted",
        "",
        "commandId",
        "focusChanged",
        "mainViewActive",
        "macroRecordingStateChanged",
        "isRecording",
        "fileNew",
        "fileOpen",
        "fileOpenFolder",
        "fileOpenFolderAsWorkspace",
        "fileOpenContainingFolder",
        "fileOpenInCommandPrompt",
        "fileOpenAsReadOnly",
        "fileReload",
        "fileSave",
        "fileSaveAs",
        "fileSaveCopyAs",
        "fileSaveAll",
        "fileRename",
        "fileDelete",
        "fileClose",
        "fileCloseAll",
        "fileCloseAllButCurrent",
        "fileCloseAllToLeft",
        "fileCloseAllToRight",
        "fileCloseAllUnchanged",
        "fileLoadSession",
        "fileSaveSession",
        "filePrint",
        "fileExit",
        "fileRestoreLastClosed",
        "editCut",
        "editCopy",
        "editPaste",
        "editDelete",
        "editSelectAll",
        "editBeginEndSelect",
        "editBeginEndSelectColumnMode",
        "editUndo",
        "editRedo",
        "editFullPathToClip",
        "editFileNameToClip",
        "editCurrentDirToClip",
        "editCopyAllNames",
        "editCopyAllPaths",
        "editInsertTab",
        "editRemoveTab",
        "editUpperCase",
        "editLowerCase",
        "editProperCase",
        "editSentenceCase",
        "editInvertCase",
        "editRandomCase",
        "editRemoveConsecutiveDupLines",
        "editRemoveAnyDupLines",
        "editSplitLines",
        "editJoinLines",
        "editLineUp",
        "editLineDown",
        "editRemoveEmptyLines",
        "editRemoveEmptyLinesWithBlank",
        "editBlankLineAbove",
        "editBlankLineBelow",
        "editSortLines",
        "sortType",
        "editBlockComment",
        "editBlockUncomment",
        "editStreamComment",
        "editStreamUncomment",
        "editAutoComplete",
        "editAutoCompletePath",
        "editAutoCompleteCurrentFile",
        "editFuncCallTip",
        "editFuncCallTipPrevious",
        "editFuncCallTipNext",
        "editInsertDateTimeShort",
        "editInsertDateTimeLong",
        "editInsertDateTimeCustom",
        "editTrimTrailing",
        "editTrimLeading",
        "editTrimBoth",
        "editEolToWs",
        "editTrimAll",
        "editTabToSpace",
        "editSpaceToTabAll",
        "editSpaceToTabLeading",
        "searchFind",
        "searchReplace",
        "searchFindNext",
        "searchFindPrev",
        "searchFindInFiles",
        "searchFindInProjects",
        "searchGoToLine",
        "searchGoToMatchingBrace",
        "searchSelectMatchingBraces",
        "searchMarkAllExt1",
        "searchMarkAllExt2",
        "searchMarkAllExt3",
        "searchMarkAllExt4",
        "searchMarkAllExt5",
        "searchClearAllMarks",
        "searchToggleBookmark",
        "searchNextBookmark",
        "searchPrevBookmark",
        "searchClearBookmarks",
        "searchCutMarkedLines",
        "searchCopyMarkedLines",
        "searchPasteMarkedLines",
        "searchDeleteMarkedLines",
        "searchDeleteUnmarkedLines",
        "searchInverseMarks",
        "searchFindCharInRange",
        "viewAlwaysOnTop",
        "viewFullScreen",
        "viewPostIt",
        "viewDistractionFree",
        "viewTabSpace",
        "viewEol",
        "viewAllCharacters",
        "viewNpc",
        "viewNpcCcuniEol",
        "viewIndentGuide",
        "viewWrapSymbol",
        "viewZoomIn",
        "viewZoomOut",
        "viewZoomRestore",
        "viewGotoStart",
        "viewGotoEnd",
        "viewSwitchToOtherView",
        "viewCloneToAnotherView",
        "viewGotoNewInstance",
        "viewLoadInNewInstance",
        "viewWrap",
        "viewHideLines",
        "viewFullScreenToggle",
        "viewSummary",
        "encodingAnsi",
        "encodingUTF8",
        "encodingUTF8BOM",
        "encodingUTF16BE",
        "encodingUTF16LE",
        "encodingConvertAnsi",
        "encodingConvertUTF8",
        "encodingConvertUTF8BOM",
        "encodingConvertUTF16BE",
        "encodingConvertUTF16LE",
        "languageMenu",
        "languageUserDefine",
        "languageOpenUdldDir",
        "settingsPreference",
        "settingsStyleConfig",
        "settingsShortcutMapper",
        "settingsImportPlugin",
        "settingsImportStyleThemes",
        "settingsEditContextMenu",
        "macroStartRecording",
        "macroStopRecording",
        "macroPlayback",
        "macroSaveCurrent",
        "macroRunMultiple",
        "runExecute",
        "runCmdLineArgs",
        "windowSwitchToDocument",
        "docIndex",
        "windowSortAscending",
        "windowSortDescending",
        "helpAbout",
        "helpDocumentation",
        "helpHomeSweetHome",
        "helpOnlineDoc",
        "columnEditor",
        "clipboardHistoryPanel",
        "charPanel",
        "toggleDocumentMap",
        "toggleFunctionList",
        "toggleFileBrowser",
        "toggleProjectPanel1",
        "toggleProjectPanel2",
        "toggleProjectPanel3",
        "toggleClipboardHistory",
        "toggleDocList",
        "findInFiles",
        "findInProjects",
        "findInFilelist",
        "replaceInFiles",
        "replaceInProjects",
        "replaceInFilelist",
        "findAllInOpenedDocs",
        "replaceAllInOpenedDocs",
        "toggleReadOnly",
        "setReadOnlyForAll",
        "clearReadOnlyForAll",
        "toggleRecording",
        "toggleMacroPlayback",
        "toggleMonitoring"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'commandExecuted'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'focusChanged'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'macroRecordingStateChanged'
        QtMocHelpers::SignalData<void(bool)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 },
        }}),
        // Slot 'fileNew'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileOpen'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileOpenFolder'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileOpenFolderAsWorkspace'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileOpenContainingFolder'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileOpenInCommandPrompt'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileOpenAsReadOnly'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileReload'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileSave'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileSaveAs'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileSaveCopyAs'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileSaveAll'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileRename'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileDelete'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileClose'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileCloseAll'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileCloseAllButCurrent'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileCloseAllToLeft'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileCloseAllToRight'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileCloseAllUnchanged'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileLoadSession'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileSaveSession'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'filePrint'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileExit'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'fileRestoreLastClosed'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editCut'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editCopy'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editPaste'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editDelete'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editSelectAll'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editBeginEndSelect'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editBeginEndSelectColumnMode'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editUndo'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editRedo'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editFullPathToClip'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editFileNameToClip'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editCurrentDirToClip'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editCopyAllNames'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editCopyAllPaths'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editInsertTab'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editRemoveTab'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editUpperCase'
        QtMocHelpers::SlotData<void()>(49, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editLowerCase'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editProperCase'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editSentenceCase'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editInvertCase'
        QtMocHelpers::SlotData<void()>(53, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editRandomCase'
        QtMocHelpers::SlotData<void()>(54, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editRemoveConsecutiveDupLines'
        QtMocHelpers::SlotData<void()>(55, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editRemoveAnyDupLines'
        QtMocHelpers::SlotData<void()>(56, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editSplitLines'
        QtMocHelpers::SlotData<void()>(57, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editJoinLines'
        QtMocHelpers::SlotData<void()>(58, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editLineUp'
        QtMocHelpers::SlotData<void()>(59, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editLineDown'
        QtMocHelpers::SlotData<void()>(60, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editRemoveEmptyLines'
        QtMocHelpers::SlotData<void()>(61, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editRemoveEmptyLinesWithBlank'
        QtMocHelpers::SlotData<void()>(62, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editBlankLineAbove'
        QtMocHelpers::SlotData<void()>(63, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editBlankLineBelow'
        QtMocHelpers::SlotData<void()>(64, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editSortLines'
        QtMocHelpers::SlotData<void(int)>(65, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 66 },
        }}),
        // Slot 'editBlockComment'
        QtMocHelpers::SlotData<void()>(67, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editBlockUncomment'
        QtMocHelpers::SlotData<void()>(68, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editStreamComment'
        QtMocHelpers::SlotData<void()>(69, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editStreamUncomment'
        QtMocHelpers::SlotData<void()>(70, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editAutoComplete'
        QtMocHelpers::SlotData<void()>(71, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editAutoCompletePath'
        QtMocHelpers::SlotData<void()>(72, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editAutoCompleteCurrentFile'
        QtMocHelpers::SlotData<void()>(73, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editFuncCallTip'
        QtMocHelpers::SlotData<void()>(74, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editFuncCallTipPrevious'
        QtMocHelpers::SlotData<void()>(75, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editFuncCallTipNext'
        QtMocHelpers::SlotData<void()>(76, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editInsertDateTimeShort'
        QtMocHelpers::SlotData<void()>(77, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editInsertDateTimeLong'
        QtMocHelpers::SlotData<void()>(78, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editInsertDateTimeCustom'
        QtMocHelpers::SlotData<void()>(79, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editTrimTrailing'
        QtMocHelpers::SlotData<void()>(80, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editTrimLeading'
        QtMocHelpers::SlotData<void()>(81, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editTrimBoth'
        QtMocHelpers::SlotData<void()>(82, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editEolToWs'
        QtMocHelpers::SlotData<void()>(83, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editTrimAll'
        QtMocHelpers::SlotData<void()>(84, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editTabToSpace'
        QtMocHelpers::SlotData<void()>(85, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editSpaceToTabAll'
        QtMocHelpers::SlotData<void()>(86, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'editSpaceToTabLeading'
        QtMocHelpers::SlotData<void()>(87, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchFind'
        QtMocHelpers::SlotData<void()>(88, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchReplace'
        QtMocHelpers::SlotData<void()>(89, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchFindNext'
        QtMocHelpers::SlotData<void()>(90, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchFindPrev'
        QtMocHelpers::SlotData<void()>(91, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchFindInFiles'
        QtMocHelpers::SlotData<void()>(92, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchFindInProjects'
        QtMocHelpers::SlotData<void()>(93, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchGoToLine'
        QtMocHelpers::SlotData<void()>(94, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchGoToMatchingBrace'
        QtMocHelpers::SlotData<void()>(95, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchSelectMatchingBraces'
        QtMocHelpers::SlotData<void()>(96, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchMarkAllExt1'
        QtMocHelpers::SlotData<void()>(97, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchMarkAllExt2'
        QtMocHelpers::SlotData<void()>(98, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchMarkAllExt3'
        QtMocHelpers::SlotData<void()>(99, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchMarkAllExt4'
        QtMocHelpers::SlotData<void()>(100, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchMarkAllExt5'
        QtMocHelpers::SlotData<void()>(101, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchClearAllMarks'
        QtMocHelpers::SlotData<void()>(102, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchToggleBookmark'
        QtMocHelpers::SlotData<void()>(103, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchNextBookmark'
        QtMocHelpers::SlotData<void()>(104, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchPrevBookmark'
        QtMocHelpers::SlotData<void()>(105, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchClearBookmarks'
        QtMocHelpers::SlotData<void()>(106, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchCutMarkedLines'
        QtMocHelpers::SlotData<void()>(107, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchCopyMarkedLines'
        QtMocHelpers::SlotData<void()>(108, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchPasteMarkedLines'
        QtMocHelpers::SlotData<void()>(109, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchDeleteMarkedLines'
        QtMocHelpers::SlotData<void()>(110, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchDeleteUnmarkedLines'
        QtMocHelpers::SlotData<void()>(111, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchInverseMarks'
        QtMocHelpers::SlotData<void()>(112, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'searchFindCharInRange'
        QtMocHelpers::SlotData<void()>(113, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewAlwaysOnTop'
        QtMocHelpers::SlotData<void()>(114, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewFullScreen'
        QtMocHelpers::SlotData<void()>(115, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewPostIt'
        QtMocHelpers::SlotData<void()>(116, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewDistractionFree'
        QtMocHelpers::SlotData<void()>(117, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewTabSpace'
        QtMocHelpers::SlotData<void()>(118, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewEol'
        QtMocHelpers::SlotData<void()>(119, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewAllCharacters'
        QtMocHelpers::SlotData<void()>(120, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewNpc'
        QtMocHelpers::SlotData<void()>(121, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewNpcCcuniEol'
        QtMocHelpers::SlotData<void()>(122, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewIndentGuide'
        QtMocHelpers::SlotData<void()>(123, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewWrapSymbol'
        QtMocHelpers::SlotData<void()>(124, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewZoomIn'
        QtMocHelpers::SlotData<void()>(125, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewZoomOut'
        QtMocHelpers::SlotData<void()>(126, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewZoomRestore'
        QtMocHelpers::SlotData<void()>(127, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewGotoStart'
        QtMocHelpers::SlotData<void()>(128, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewGotoEnd'
        QtMocHelpers::SlotData<void()>(129, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewSwitchToOtherView'
        QtMocHelpers::SlotData<void()>(130, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewCloneToAnotherView'
        QtMocHelpers::SlotData<void()>(131, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewGotoNewInstance'
        QtMocHelpers::SlotData<void()>(132, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewLoadInNewInstance'
        QtMocHelpers::SlotData<void()>(133, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewWrap'
        QtMocHelpers::SlotData<void()>(134, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewHideLines'
        QtMocHelpers::SlotData<void()>(135, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewFullScreenToggle'
        QtMocHelpers::SlotData<void()>(136, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'viewSummary'
        QtMocHelpers::SlotData<void()>(137, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingAnsi'
        QtMocHelpers::SlotData<void()>(138, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingUTF8'
        QtMocHelpers::SlotData<void()>(139, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingUTF8BOM'
        QtMocHelpers::SlotData<void()>(140, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingUTF16BE'
        QtMocHelpers::SlotData<void()>(141, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingUTF16LE'
        QtMocHelpers::SlotData<void()>(142, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingConvertAnsi'
        QtMocHelpers::SlotData<void()>(143, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingConvertUTF8'
        QtMocHelpers::SlotData<void()>(144, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingConvertUTF8BOM'
        QtMocHelpers::SlotData<void()>(145, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingConvertUTF16BE'
        QtMocHelpers::SlotData<void()>(146, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'encodingConvertUTF16LE'
        QtMocHelpers::SlotData<void()>(147, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'languageMenu'
        QtMocHelpers::SlotData<void()>(148, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'languageUserDefine'
        QtMocHelpers::SlotData<void()>(149, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'languageOpenUdldDir'
        QtMocHelpers::SlotData<void()>(150, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'settingsPreference'
        QtMocHelpers::SlotData<void()>(151, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'settingsStyleConfig'
        QtMocHelpers::SlotData<void()>(152, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'settingsShortcutMapper'
        QtMocHelpers::SlotData<void()>(153, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'settingsImportPlugin'
        QtMocHelpers::SlotData<void()>(154, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'settingsImportStyleThemes'
        QtMocHelpers::SlotData<void()>(155, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'settingsEditContextMenu'
        QtMocHelpers::SlotData<void()>(156, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'macroStartRecording'
        QtMocHelpers::SlotData<void()>(157, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'macroStopRecording'
        QtMocHelpers::SlotData<void()>(158, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'macroPlayback'
        QtMocHelpers::SlotData<void()>(159, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'macroSaveCurrent'
        QtMocHelpers::SlotData<void()>(160, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'macroRunMultiple'
        QtMocHelpers::SlotData<void()>(161, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'runExecute'
        QtMocHelpers::SlotData<void()>(162, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'runCmdLineArgs'
        QtMocHelpers::SlotData<void()>(163, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'windowSwitchToDocument'
        QtMocHelpers::SlotData<void(int)>(164, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 165 },
        }}),
        // Slot 'windowSortAscending'
        QtMocHelpers::SlotData<void()>(166, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'windowSortDescending'
        QtMocHelpers::SlotData<void()>(167, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'helpAbout'
        QtMocHelpers::SlotData<void()>(168, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'helpDocumentation'
        QtMocHelpers::SlotData<void()>(169, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'helpHomeSweetHome'
        QtMocHelpers::SlotData<void()>(170, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'helpOnlineDoc'
        QtMocHelpers::SlotData<void()>(171, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'columnEditor'
        QtMocHelpers::SlotData<void()>(172, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'clipboardHistoryPanel'
        QtMocHelpers::SlotData<void()>(173, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'charPanel'
        QtMocHelpers::SlotData<void()>(174, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleDocumentMap'
        QtMocHelpers::SlotData<void()>(175, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleFunctionList'
        QtMocHelpers::SlotData<void()>(176, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleFileBrowser'
        QtMocHelpers::SlotData<void()>(177, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleProjectPanel1'
        QtMocHelpers::SlotData<void()>(178, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleProjectPanel2'
        QtMocHelpers::SlotData<void()>(179, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleProjectPanel3'
        QtMocHelpers::SlotData<void()>(180, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleClipboardHistory'
        QtMocHelpers::SlotData<void()>(181, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleDocList'
        QtMocHelpers::SlotData<void()>(182, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'findInFiles'
        QtMocHelpers::SlotData<void()>(183, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'findInProjects'
        QtMocHelpers::SlotData<void()>(184, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'findInFilelist'
        QtMocHelpers::SlotData<void()>(185, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'replaceInFiles'
        QtMocHelpers::SlotData<void()>(186, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'replaceInProjects'
        QtMocHelpers::SlotData<void()>(187, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'replaceInFilelist'
        QtMocHelpers::SlotData<void()>(188, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'findAllInOpenedDocs'
        QtMocHelpers::SlotData<void()>(189, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'replaceAllInOpenedDocs'
        QtMocHelpers::SlotData<void()>(190, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleReadOnly'
        QtMocHelpers::SlotData<void()>(191, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setReadOnlyForAll'
        QtMocHelpers::SlotData<void()>(192, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'clearReadOnlyForAll'
        QtMocHelpers::SlotData<void()>(193, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleRecording'
        QtMocHelpers::SlotData<void()>(194, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleMacroPlayback'
        QtMocHelpers::SlotData<void()>(195, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleMonitoring'
        QtMocHelpers::SlotData<void()>(196, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NppCommands, qt_meta_tag_ZN11NppCommandsE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NppCommands::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppCommandsE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppCommandsE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11NppCommandsE_t>.metaTypes,
    nullptr
} };

void NppCommands::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NppCommands *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->commandExecuted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->focusChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->macroRecordingStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->fileNew(); break;
        case 4: _t->fileOpen(); break;
        case 5: _t->fileOpenFolder(); break;
        case 6: _t->fileOpenFolderAsWorkspace(); break;
        case 7: _t->fileOpenContainingFolder(); break;
        case 8: _t->fileOpenInCommandPrompt(); break;
        case 9: _t->fileOpenAsReadOnly(); break;
        case 10: _t->fileReload(); break;
        case 11: _t->fileSave(); break;
        case 12: _t->fileSaveAs(); break;
        case 13: _t->fileSaveCopyAs(); break;
        case 14: _t->fileSaveAll(); break;
        case 15: _t->fileRename(); break;
        case 16: _t->fileDelete(); break;
        case 17: _t->fileClose(); break;
        case 18: _t->fileCloseAll(); break;
        case 19: _t->fileCloseAllButCurrent(); break;
        case 20: _t->fileCloseAllToLeft(); break;
        case 21: _t->fileCloseAllToRight(); break;
        case 22: _t->fileCloseAllUnchanged(); break;
        case 23: _t->fileLoadSession(); break;
        case 24: _t->fileSaveSession(); break;
        case 25: _t->filePrint(); break;
        case 26: _t->fileExit(); break;
        case 27: _t->fileRestoreLastClosed(); break;
        case 28: _t->editCut(); break;
        case 29: _t->editCopy(); break;
        case 30: _t->editPaste(); break;
        case 31: _t->editDelete(); break;
        case 32: _t->editSelectAll(); break;
        case 33: _t->editBeginEndSelect(); break;
        case 34: _t->editBeginEndSelectColumnMode(); break;
        case 35: _t->editUndo(); break;
        case 36: _t->editRedo(); break;
        case 37: _t->editFullPathToClip(); break;
        case 38: _t->editFileNameToClip(); break;
        case 39: _t->editCurrentDirToClip(); break;
        case 40: _t->editCopyAllNames(); break;
        case 41: _t->editCopyAllPaths(); break;
        case 42: _t->editInsertTab(); break;
        case 43: _t->editRemoveTab(); break;
        case 44: _t->editUpperCase(); break;
        case 45: _t->editLowerCase(); break;
        case 46: _t->editProperCase(); break;
        case 47: _t->editSentenceCase(); break;
        case 48: _t->editInvertCase(); break;
        case 49: _t->editRandomCase(); break;
        case 50: _t->editRemoveConsecutiveDupLines(); break;
        case 51: _t->editRemoveAnyDupLines(); break;
        case 52: _t->editSplitLines(); break;
        case 53: _t->editJoinLines(); break;
        case 54: _t->editLineUp(); break;
        case 55: _t->editLineDown(); break;
        case 56: _t->editRemoveEmptyLines(); break;
        case 57: _t->editRemoveEmptyLinesWithBlank(); break;
        case 58: _t->editBlankLineAbove(); break;
        case 59: _t->editBlankLineBelow(); break;
        case 60: _t->editSortLines((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 61: _t->editBlockComment(); break;
        case 62: _t->editBlockUncomment(); break;
        case 63: _t->editStreamComment(); break;
        case 64: _t->editStreamUncomment(); break;
        case 65: _t->editAutoComplete(); break;
        case 66: _t->editAutoCompletePath(); break;
        case 67: _t->editAutoCompleteCurrentFile(); break;
        case 68: _t->editFuncCallTip(); break;
        case 69: _t->editFuncCallTipPrevious(); break;
        case 70: _t->editFuncCallTipNext(); break;
        case 71: _t->editInsertDateTimeShort(); break;
        case 72: _t->editInsertDateTimeLong(); break;
        case 73: _t->editInsertDateTimeCustom(); break;
        case 74: _t->editTrimTrailing(); break;
        case 75: _t->editTrimLeading(); break;
        case 76: _t->editTrimBoth(); break;
        case 77: _t->editEolToWs(); break;
        case 78: _t->editTrimAll(); break;
        case 79: _t->editTabToSpace(); break;
        case 80: _t->editSpaceToTabAll(); break;
        case 81: _t->editSpaceToTabLeading(); break;
        case 82: _t->searchFind(); break;
        case 83: _t->searchReplace(); break;
        case 84: _t->searchFindNext(); break;
        case 85: _t->searchFindPrev(); break;
        case 86: _t->searchFindInFiles(); break;
        case 87: _t->searchFindInProjects(); break;
        case 88: _t->searchGoToLine(); break;
        case 89: _t->searchGoToMatchingBrace(); break;
        case 90: _t->searchSelectMatchingBraces(); break;
        case 91: _t->searchMarkAllExt1(); break;
        case 92: _t->searchMarkAllExt2(); break;
        case 93: _t->searchMarkAllExt3(); break;
        case 94: _t->searchMarkAllExt4(); break;
        case 95: _t->searchMarkAllExt5(); break;
        case 96: _t->searchClearAllMarks(); break;
        case 97: _t->searchToggleBookmark(); break;
        case 98: _t->searchNextBookmark(); break;
        case 99: _t->searchPrevBookmark(); break;
        case 100: _t->searchClearBookmarks(); break;
        case 101: _t->searchCutMarkedLines(); break;
        case 102: _t->searchCopyMarkedLines(); break;
        case 103: _t->searchPasteMarkedLines(); break;
        case 104: _t->searchDeleteMarkedLines(); break;
        case 105: _t->searchDeleteUnmarkedLines(); break;
        case 106: _t->searchInverseMarks(); break;
        case 107: _t->searchFindCharInRange(); break;
        case 108: _t->viewAlwaysOnTop(); break;
        case 109: _t->viewFullScreen(); break;
        case 110: _t->viewPostIt(); break;
        case 111: _t->viewDistractionFree(); break;
        case 112: _t->viewTabSpace(); break;
        case 113: _t->viewEol(); break;
        case 114: _t->viewAllCharacters(); break;
        case 115: _t->viewNpc(); break;
        case 116: _t->viewNpcCcuniEol(); break;
        case 117: _t->viewIndentGuide(); break;
        case 118: _t->viewWrapSymbol(); break;
        case 119: _t->viewZoomIn(); break;
        case 120: _t->viewZoomOut(); break;
        case 121: _t->viewZoomRestore(); break;
        case 122: _t->viewGotoStart(); break;
        case 123: _t->viewGotoEnd(); break;
        case 124: _t->viewSwitchToOtherView(); break;
        case 125: _t->viewCloneToAnotherView(); break;
        case 126: _t->viewGotoNewInstance(); break;
        case 127: _t->viewLoadInNewInstance(); break;
        case 128: _t->viewWrap(); break;
        case 129: _t->viewHideLines(); break;
        case 130: _t->viewFullScreenToggle(); break;
        case 131: _t->viewSummary(); break;
        case 132: _t->encodingAnsi(); break;
        case 133: _t->encodingUTF8(); break;
        case 134: _t->encodingUTF8BOM(); break;
        case 135: _t->encodingUTF16BE(); break;
        case 136: _t->encodingUTF16LE(); break;
        case 137: _t->encodingConvertAnsi(); break;
        case 138: _t->encodingConvertUTF8(); break;
        case 139: _t->encodingConvertUTF8BOM(); break;
        case 140: _t->encodingConvertUTF16BE(); break;
        case 141: _t->encodingConvertUTF16LE(); break;
        case 142: _t->languageMenu(); break;
        case 143: _t->languageUserDefine(); break;
        case 144: _t->languageOpenUdldDir(); break;
        case 145: _t->settingsPreference(); break;
        case 146: _t->settingsStyleConfig(); break;
        case 147: _t->settingsShortcutMapper(); break;
        case 148: _t->settingsImportPlugin(); break;
        case 149: _t->settingsImportStyleThemes(); break;
        case 150: _t->settingsEditContextMenu(); break;
        case 151: _t->macroStartRecording(); break;
        case 152: _t->macroStopRecording(); break;
        case 153: _t->macroPlayback(); break;
        case 154: _t->macroSaveCurrent(); break;
        case 155: _t->macroRunMultiple(); break;
        case 156: _t->runExecute(); break;
        case 157: _t->runCmdLineArgs(); break;
        case 158: _t->windowSwitchToDocument((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 159: _t->windowSortAscending(); break;
        case 160: _t->windowSortDescending(); break;
        case 161: _t->helpAbout(); break;
        case 162: _t->helpDocumentation(); break;
        case 163: _t->helpHomeSweetHome(); break;
        case 164: _t->helpOnlineDoc(); break;
        case 165: _t->columnEditor(); break;
        case 166: _t->clipboardHistoryPanel(); break;
        case 167: _t->charPanel(); break;
        case 168: _t->toggleDocumentMap(); break;
        case 169: _t->toggleFunctionList(); break;
        case 170: _t->toggleFileBrowser(); break;
        case 171: _t->toggleProjectPanel1(); break;
        case 172: _t->toggleProjectPanel2(); break;
        case 173: _t->toggleProjectPanel3(); break;
        case 174: _t->toggleClipboardHistory(); break;
        case 175: _t->toggleDocList(); break;
        case 176: _t->findInFiles(); break;
        case 177: _t->findInProjects(); break;
        case 178: _t->findInFilelist(); break;
        case 179: _t->replaceInFiles(); break;
        case 180: _t->replaceInProjects(); break;
        case 181: _t->replaceInFilelist(); break;
        case 182: _t->findAllInOpenedDocs(); break;
        case 183: _t->replaceAllInOpenedDocs(); break;
        case 184: _t->toggleReadOnly(); break;
        case 185: _t->setReadOnlyForAll(); break;
        case 186: _t->clearReadOnlyForAll(); break;
        case 187: _t->toggleRecording(); break;
        case 188: _t->toggleMacroPlayback(); break;
        case 189: _t->toggleMonitoring(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NppCommands::*)(int )>(_a, &NppCommands::commandExecuted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NppCommands::*)(bool )>(_a, &NppCommands::focusChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NppCommands::*)(bool )>(_a, &NppCommands::macroRecordingStateChanged, 2))
            return;
    }
}

const QMetaObject *NppCommands::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NppCommands::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11NppCommandsE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NppCommands::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 190)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 190;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 190)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 190;
    }
    return _id;
}

// SIGNAL 0
void NppCommands::commandExecuted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void NppCommands::focusChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void NppCommands::macroRecordingStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
