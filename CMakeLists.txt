cmake_minimum_required(VERSION 3.16)
project(npp-qt VERSION 8.9.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets PrintSupport Network REQUIRED)
find_package(Qt6Core5Compat REQUIRED)
find_package(QScintillaLib COMPONENTS Qt6 QUIET)

add_subdirectory(src/ScintillaComponents/scintilla)
add_subdirectory(src/ScintillaComponents/lexilla)

set(NPP_CORE_SOURCES
    src/EncodingMapper.cpp
    src/MISC/Common/Common.cpp
    src/MISC/Common/FileInterface.cpp
    src/MISC/Common/SortLocale.cpp
    src/MISC/Common/verifySignedfile.cpp
    src/MISC/Exception/MiniDumper.cpp
    src/MISC/Exception/Win32Exception.cpp
    src/MISC/PluginsManager/PluginsManager.cpp
    src/MISC/Process/Processus.cpp
    src/MISC/RegExt/regExtDlg.cpp
    src/MISC/md5/md5.cpp
    src/MISC/md5/md5Dlgs.cpp
    src/MISC/sha1/calc_sha1.cpp
    src/MISC/sha1/sha1.cpp
    src/MISC/sha2/sha-256.cpp
    src/MISC/sha512/sha512.cpp
    src/MainWindow.cpp
    src/Notepad_plus.cpp
    src/Notepad_plus_Window.cpp
    src/NppBigSwitch.cpp
    src/NppCommands.cpp
    src/NppDarkMode.cpp
    src/NppIO.cpp
    src/NppNotification.cpp
    src/Parameters.cpp
    src/ScintillaComponent/AutoCompletion.cpp
    src/ScintillaComponent/Buffer.cpp
    src/ScintillaComponent/DocTabView.cpp
    src/ScintillaComponent/FindReplaceDlg.cpp
    src/ScintillaComponent/FunctionCallTip.cpp
    src/ScintillaComponent/GoToLineDlg.cpp
    src/ScintillaComponent/Printer.cpp
    src/ScintillaComponent/ScintillaCtrls.cpp
    src/ScintillaComponent/ScintillaEditView.cpp
    src/ScintillaComponent/SmartHighlighter.cpp
    src/ScintillaComponent/UserDefineDialog.cpp
    src/ScintillaComponent/columnEditor.cpp
    src/ScintillaComponent/xmlMatchedTagsHighlighter.cpp
    src/Utf8_16.cpp
    src/WinControls/AboutDlg/AboutDlg.cpp
    src/WinControls/AboutDlg/URLCtrl.cpp
    src/WinControls/AnsiCharPanel/ListView.cpp
    src/WinControls/AnsiCharPanel/ansiCharPanel.cpp
    src/WinControls/AnsiCharPanel/asciiListView.cpp
    src/WinControls/ClipboardHistory/clipboardHistoryPanel.cpp
    src/WinControls/ColourPicker/ColourPicker.cpp
    src/WinControls/ColourPicker/ColourPopup.cpp
    src/WinControls/ColourPicker/WordStyleDlg.cpp
    src/WinControls/ContextMenu/ContextMenu.cpp
    src/WinControls/DockingWnd/DockingCont.cpp
    src/WinControls/DockingWnd/DockingManager.cpp
    src/WinControls/DockingWnd/DockingSplitter.cpp
    src/WinControls/DockingWnd/Gripper.cpp
    src/WinControls/DocumentMap/documentMap.cpp
    src/WinControls/DocumentMap/documentSnapshot.cpp
    src/WinControls/DoubleBuffer/DoubleBuffer.cpp
    src/WinControls/FileBrowser/fileBrowser.cpp
    src/WinControls/FindCharsInRange/FindCharsInRange.cpp
    src/WinControls/FunctionList/functionListPanel.cpp
    src/WinControls/FunctionList/functionParser.cpp
    src/WinControls/Grid/BabyGrid.cpp
    src/WinControls/Grid/BabyGridWrapper.cpp
    src/WinControls/Grid/ShortcutMapper.cpp
    src/WinControls/ImageListSet/ImageListSet.cpp
    src/WinControls/OpenSaveFileDialog/CustomFileDialog.cpp
    src/WinControls/PluginsAdmin/pluginsAdmin.cpp
    src/WinControls/Preference/preferenceDlg.cpp
    src/WinControls/ProjectPanel/ProjectPanel.cpp
    src/WinControls/ReadDirectoryChanges/ReadDirectoryChanges.cpp
    src/WinControls/ReadDirectoryChanges/ReadDirectoryChangesPrivate.cpp
    src/WinControls/ReadDirectoryChanges/ReadFileChanges.cpp
    src/WinControls/SplitterContainer/Splitter.cpp
    src/WinControls/SplitterContainer/SplitterContainer.cpp
    src/WinControls/StaticDialog/RunDlg/RunDlg.cpp
    src/WinControls/StaticDialog/StaticDialog.cpp
    src/WinControls/StatusBar/StatusBar.cpp
    src/WinControls/TabBar/ControlsTab.cpp
    src/WinControls/TabBar/TabBar.cpp
    src/WinControls/TaskList/TaskList.cpp
    src/WinControls/TaskList/TaskListDlg.cpp
    src/WinControls/ToolBar/ToolBar.cpp
    src/WinControls/ToolTip/ToolTip.cpp
    src/WinControls/TrayIcon/trayIconControler.cpp
    src/WinControls/TreeView/TreeView.cpp
    src/WinControls/VerticalFileSwitcher/VerticalFileSwitcher.cpp
    src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.cpp
    src/WinControls/WindowsDlg/SizeableDlg.cpp
    src/WinControls/WindowsDlg/WinMgr.cpp
    src/WinControls/WindowsDlg/WinRect.cpp
    src/WinControls/WindowsDlg/WindowsDlg.cpp
    src/WinControls/shortcut/RunMacroDlg.cpp
    src/WinControls/shortcut/shortcut.cpp
    src/dpiManagerV2.cpp
    src/lastRecentFileList.cpp
    src/lesDlgs.cpp
    src/localization.cpp
    src/main.cpp
    src/pugixml/pugixml.cpp
    src/uchardet/CharDistribution.cpp
    src/uchardet/JpCntx.cpp
    src/uchardet/LangBulgarianModel.cpp
    src/uchardet/LangCyrillicModel.cpp
    src/uchardet/LangGreekModel.cpp
    src/uchardet/LangHebrewModel.cpp
    src/uchardet/LangHungarianModel.cpp
    src/uchardet/LangThaiModel.cpp
    src/uchardet/nsBig5Prober.cpp
    src/uchardet/nsCharSetProber.cpp
    src/uchardet/nsEUCJPProber.cpp
    src/uchardet/nsEUCKRProber.cpp
    src/uchardet/nsEUCTWProber.cpp
    src/uchardet/nsEscCharsetProber.cpp
    src/uchardet/nsEscSM.cpp
    src/uchardet/nsGB2312Prober.cpp
    src/uchardet/nsHebrewProber.cpp
    src/uchardet/nsLatin1Prober.cpp
    src/uchardet/nsMBCSGroupProber.cpp
    src/uchardet/nsMBCSSM.cpp
    src/uchardet/nsSBCSGroupProber.cpp
    src/uchardet/nsSBCharSetProber.cpp
    src/uchardet/nsSJISProber.cpp
    src/uchardet/nsUTF8Prober.cpp
    src/uchardet/nsUniversalDetector.cpp
    src/uchardet/uchardet.cpp
)

if(NOT WIN32)
    list(REMOVE_ITEM NPP_CORE_SOURCES src/WinControls/OpenSaveFileDialog/CustomFileDialog.cpp)
    list(APPEND NPP_CORE_SOURCES src/WinControls/OpenSaveFileDialog/CustomFileDialog_qt.cpp)
endif()

# Headers in include/ need MOC processing - add them explicitly for AUTOMOC to work.
# These are the headers that define Q_OBJECT classes requiring MOC generation.
set(NPP_MOC_HEADERS
    include/MainWindow.h
    include/NppApplication.h
)
add_executable(npp-qt ${NPP_CORE_SOURCES} ${NPP_MOC_HEADERS})

target_compile_definitions(npp-qt PRIVATE NPP_WINDOWS_COMPAT_HWND)

target_include_directories(npp-qt PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/MISC/Common
    ${CMAKE_SOURCE_DIR}/src/MISC/PluginsManager
    ${CMAKE_SOURCE_DIR}/src/scintilla/include
    ${CMAKE_SOURCE_DIR}/src/lexilla/include
    ${CMAKE_SOURCE_DIR}/src/pugixml
    ${CMAKE_SOURCE_DIR}/src/json
    ${CMAKE_SOURCE_DIR}/src/WinControls
    ${CMAKE_SOURCE_DIR}/src/WinControls/DockingWnd
    ${CMAKE_SOURCE_DIR}/src/WinControls/StaticDialog
    ${CMAKE_SOURCE_DIR}/src/WinControls/TreeView
    ${CMAKE_SOURCE_DIR}/src/WinControls/ColourPicker
    ${CMAKE_SOURCE_DIR}/src/WinControls/ToolTip
    ${CMAKE_SOURCE_DIR}/src/WinControls/ImageListSet
    ${CMAKE_SOURCE_DIR}/src/WinControls/DoubleBuffer
    ${CMAKE_SOURCE_DIR}/src/WinControls/Window
    ${CMAKE_SOURCE_DIR}/src/MISC/sha1
    ${CMAKE_SOURCE_DIR}/src/uchardet
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/npp-qt_autogen>
)

target_link_libraries(npp-qt PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::PrintSupport Qt6::Network Qt6::Core5Compat
    scintilla_qt lexilla
)

if(QScintillaLib_Qt6_FOUND)
    target_compile_definitions(npp-qt PRIVATE HAVE_QSCINTILLA)
    target_link_libraries(npp-qt PRIVATE QScintillaLib::QScintilla)
else()
    target_compile_definitions(npp-qt PRIVATE NO_SCINTILLA)
endif()
