cmake_minimum_required(VERSION 3.16)
project(npp-qt VERSION 8.9.6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# Add each target's source directory to the include path, so mocs_compilation.cpp
# (which lives in npp-qt_autogen/) can resolve relative includes like
# "YDZQWUPVU2/moc_Foo.cpp" via its own directory.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Disable automatic GCC/MinGW dependency tracking (-MD -MP) to prevent race conditions
# where the .d dependency file is read before it exists on first build
set(CMAKE_DEPEND_FLAGS "")

# Pre-create a phony .d file to prevent "No such file" errors on fresh builds
file(WRITE "${CMAKE_BINARY_DIR}/cmake_dependency_placeholder.d" "")

# WORKAROUND: AUTOMOC generates mocs_compilation.cpp with includes like
# "SUBDIR/moc_Foo.cpp" but doesn't add npp-qt_autogen/ to the include path.
# This causes compilation to fail. Also ensure Scintilla and other include paths
# are passed to the moc executable.
get_property(AUTOMOC_INCLUDE_DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY AUTOMOC_INCLUDE_DIRS)
# Always use both the binary autogen dir AND any inherited dirs
set(CMAKE_AUTOMOC_INCLUDE_DIRECTORIES
    ${CMAKE_BINARY_DIR}/npp-qt_autogen
    ${AUTOMOC_INCLUDE_DIRS}
)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Xml PrintSupport)
find_package(Qt6 COMPONENTS Svg QUIET)  # optional — SVG icons fail gracefully if absent
# QScintilla2 has no CMake config; link manually via pkg-config or library path
find_library(QSCINTILLA2_LIB NAMES qscintilla2_qt6 qscintilla2)
if(NOT QSCINTILLA2_LIB)
    message(FATAL_ERROR "QScintilla2 library not found — install libqscintilla2-qt6")
endif()
message(STATUS "QScintilla2: ${QSCINTILLA2_LIB}")

# =============================================================================
# WinControls — base infrastructure
# =============================================================================
set(WINCONTROLS_BASE
    src/WinControls/Window.h
    src/WinControls/TabBar.h
    src/WinControls/TabBar.cpp
    src/WinControls/TabBar/ControlsTab.h
    src/WinControls/TabBar/ControlsTab.cpp
    src/WinControls/TaskList/TaskListDlg_rc.h
    src/WinControls/VerticalFileSwitcher/VerticalFileSwitcher_rc.h
    src/WinControls/ToolBar.h
    src/WinControls/ToolBar.cpp
    src/WinControls/StatusBar.h
    src/WinControls/StatusBar.cpp
    src/WinControls/DockingWnd.h
    src/WinControls/DockingWnd.cpp
    src/WinControls/SplitterContainer.h
    src/WinControls/SplitterContainer.cpp
    src/WinControls/Splitter.cpp
    src/WinControls/StaticDialog.h
    src/WinControls/StaticDialog.cpp
)

# =============================================================================
# WinControls — dialogs
# =============================================================================
set(WINCONTROLS_DIALOGS
    src/WinControls/AboutDlg.h
    src/WinControls/AboutDlg.cpp
    src/WinControls/AboutDlg/URLCtrl.h
    src/WinControls/AboutDlg/URLCtrl.cpp
    src/WinControls/WindowsDlg.h
    src/WinControls/WindowsDlg.cpp
    src/WinControls/WinMgr.h
    src/WinControls/WinMgr.cpp
    src/WinControls/WinRect.h
    src/WinControls/WinRect.cpp
    src/WinControls/SizeableDlg.h
    src/WinControls/SizeableDlg.cpp
    src/WinControls/AnsiCharPanel.h
    src/WinControls/AnsiCharPanel.cpp
    src/WinControls/AnsiCharPanel/ListView.h
    src/WinControls/AnsiCharPanel/ListView.cpp
    src/WinControls/AnsiCharPanel/ansiCharPanel_rc.h
    src/WinControls/AnsiCharPanel/asciiListView.h
    src/WinControls/AnsiCharPanel/asciiListView.cpp
    src/WinControls/ColourPicker.h
    src/WinControls/ColourPicker.cpp
    src/WinControls/ColourPicker/ColourPopupResource.h
    src/WinControls/ColourPicker/ColourPopup.cpp
    src/WinControls/ColourPicker/WordStyleDlgRes.h
    src/WinControls/WordStyleDlg.h
    src/WinControls/WordStyleDlg.cpp
    src/WinControls/RunMacroDlg.h
    src/WinControls/RunMacroDlg.cpp
)

# =============================================================================
# WinControls — panels (dockable/floating)
# =============================================================================
set(WINCONTROLS_PANELS
    src/WinControls/DocumentMap.h
    src/WinControls/DocumentMap.cpp
    src/WinControls/DocumentMap/documentMap_rc.h
    src/WinControls/DocumentMap/documentSnapshot_rc.h
    src/WinControls/FunctionList.h
    src/WinControls/FunctionList.cpp
    src/WinControls/FindCharsInRange.h
    src/WinControls/FindCharsInRange.cpp
    src/WinControls/FindResultPanel/FindResultPanel.h
    src/WinControls/FindResultPanel/FindResultPanel.cpp
)

# =============================================================================
# WinControls — advanced (preference, plugins, misc tools)
# =============================================================================
set(WINCONTROLS_ADVANCED
    src/WinControls/Preference.h
    src/WinControls/Preference.cpp
    src/WinControls/Preference/preference_rc.h
    src/WinControls/ClipboardHistory.h
    src/WinControls/ClipboardHistory/clipboardHistoryPanel.cpp
    src/WinControls/ClipboardHistory/clipboardHistoryPanel_rc.h
    src/WinControls/FileBrowser.h
    src/WinControls/FileBrowser/fileBrowser.cpp
    src/WinControls/FileBrowser/fileBrowser_rc.h
    src/WinControls/ContextMenu.h
    src/WinControls/ContextMenu.cpp
    src/WinControls/ToolTip.h
    src/WinControls/ToolTip.cpp
    src/WinControls/TrayIcon.h
    src/WinControls/TrayIcon.cpp
    src/WinControls/TreeView.h
    src/WinControls/TreeView.cpp
    src/WinControls/TaskList.h
    src/WinControls/TaskList.cpp
    # TaskListDlg is declared+defined in TaskList.h/.cpp
    src/WinControls/functionParser.h
    src/WinControls/functionParser.cpp
    src/WinControls/VerticalFileSwitcher.h
    src/WinControls/VerticalFileSwitcher.cpp
    src/WinControls/PluginsAdmin.h
    src/WinControls/PluginsAdmin.cpp
    src/WinControls/ProjectPanel.h
    src/WinControls/ProjectPanel.cpp
    src/WinControls/ProjectPanel/ProjectPanel_rc.h
    src/WinControls/ShortcutMapper.h
    src/WinControls/ShortcutMapper.cpp
)

# =============================================================================
# WinControls — BabyGrid spreadsheet control + wrappers
# =============================================================================
set(WINCONTROLS_BABYGRID
    src/WinControls/Grid/BabyGrid.h
    src/WinControls/Grid/BabyGrid.cpp
    src/WinControls/Grid/BabyGridWrapper.h
    src/WinControls/Grid/BabyGridWrapper.cpp
)

# =============================================================================
# WinControls — utility / infrastructure lift
# =============================================================================
set(WINCONTROLS_UTILS
    src/WinControls/DoubleBuffer.h
    src/WinControls/DoubleBuffer.cpp
    src/WinControls/ImageListSet.h
    src/WinControls/ImageListSet.cpp
    src/WinControls/ReadDirectoryChanges.h
    src/WinControls/ReadDirectoryChanges.cpp
    src/WinControls/ReadDirectoryChanges/ReadDirectoryChangesPrivate.h
    src/WinControls/ReadDirectoryChanges/ReadDirectoryChangesPrivate.cpp
    src/WinControls/ReadDirectoryChanges/targetver.h
    src/WinControls/ReadDirectoryChanges/ThreadSafeQueue.h
    src/WinControls/CustomFileDialog.h
    src/WinControls/CustomFileDialog.cpp
    # Resource ID constants (lifted from Win32 .rc.h → Qt6)
    src/WinControls/WindowsDlg/WindowsDlgRc.h
    src/WinControls/shortcut/RunMacroDlg_rc.h
    src/WinControls/shortcut/shortcutRc.h
    # Win32 shortcut grid (uses same IDs as ShortcutMapper_rc.h)
    src/WinControls/Grid/ShortcutMapper_rc.h
)

# =============================================================================
# Scintilla layers — editor components stacked on ScintillaEditBase
# (port of PowerEditor/src/ScintillaComponent/*)
# =============================================================================
set(SCINTILLA_LAYERS
    # ScintillaComponent — main editor widget (Qt6 Scintilla wrapper)
    src/ScintillaComponent.h
    src/ScintillaComponent.cpp

    # User Define Language dialog
    src/ScintillaComponent/UserDefineDialog.h
    src/ScintillaComponent/UserDefineDialog.cpp

    # UDL parser + custom lexer (User Defined Languages from .xml)
    src/ScintillaComponent/UserLangParser.h
    src/ScintillaComponent/UserLangParser.cpp
    src/ScintillaComponent/LexerUDL.h
    src/ScintillaComponent/LexerUDL.cpp
    src/ScintillaComponent/LexerRust.h
    src/ScintillaComponent/LexerRust.cpp
    src/ScintillaComponent/LexerGo.h
    src/ScintillaComponent/LexerGo.cpp
    src/ScintillaComponent/LexerPowerShell.h
    src/ScintillaComponent/LexerPowerShell.cpp

    # ScintillaEditView — type-alias header for Win32 compatibility (see ScintillaComponent.h)
    src/ScintillaEditView.h

    # ScintillaCtrls — editor factory (stub; implementation in ScintillaComponent.cpp)
    src/ScintillaCtrls.cpp

    # Buffer — document buffer management
    src/Buffer.h
    src/Buffer.cpp

    # DocTabView — document tab container
    src/DocTabView.h
    src/DocTabView.cpp

    # Find/Replace dialog
    src/FindReplaceDlg.h
    src/FindReplaceDlg.cpp

    # Auto-completion + function calltips
    src/AutoCompletion.h
    src/AutoCompletion.cpp
    src/FunctionCallTip.h
    src/FunctionCallTip.cpp

    # Go to line dialog
    src/GoToLineDlg.h
    src/GoToLineDlg.cpp

    # Smart/highlight all matching words
    src/SmartHighlighter.h
    src/SmartHighlighter.cpp

    # Column editor (numeric/text insertion in column mode)
    src/columnEditor.h
    src/columnEditor.cpp

    # Printer — print support (QPrinter)
    src/Printer.h
    src/Printer.cpp

    # XML tag matching highlighter
    src/xmlMatchedTagsHighlighter.h
    src/xmlMatchedTagsHighlighter.cpp

    # User-defined language dialog (in ScintillaComponent/)
)

# =============================================================================
# MISC — Common utilities
# =============================================================================
set(MISC_COMMON
    src/MISC/Common/Common.h
    src/MISC/Common/Common.cpp
    src/MISC/Common/Date.cpp
    src/MISC/Common/FileInterface.h
    src/MISC/Common/FileInterface.cpp
    src/MISC/Common/EncodingMapper.h
    src/MISC/Common/EncodingMapper.cpp
    src/MISC/Common/IDAllocator.h
    src/MISC/md5/md5Dlgs.cpp
)

# =============================================================================
# MISC — File associations dialog + CRC16 utility
# =============================================================================
set(MISC_REGEXT
    src/MISC/RegExt/regExtDlg.h
    src/MISC/RegExt/regExtDlg.cpp
    src/MISC/RegExt/regExtDlgRc.h
    src/MISC/crc16/Crc16.h
    src/MISC/md5/md5Dlgs_rc.h
)

# =============================================================================
# MISC — Plugins manager
# =============================================================================
set(MISC_PLUGINS
    src/MISC/PluginsManager/PluginsManager.h
    src/MISC/PluginsManager/PluginsManager.cpp
)

# =============================================================================
# MISC — Process management (run external programs)
# =============================================================================
set(MISC_PROCESS
    src/MISC/Process/Processus.h
    src/MISC/Process/Processus.cpp
)

# =============================================================================
# Top-level infrastructure files
# =============================================================================
set(INFRASTRUCTURE
    src/resource.h
    src/NppConstants.h
    src/NppXml.h
    src/NppNotification.h
    src/dpiManagerV2.h
    src/lastRecentFileList.h
    src/localization.h
    src/shortcut.h
    src/clipboardFormats.h
    src/keys.h
)

# =============================================================================
# Third-party header-only libraries
# =============================================================================
set(THIRDPARTY_HEADERS
    src/json/json.hpp
    src/pugixml/pugiconfig.hpp
)

# =============================================================================
# uchardet — Mozilla-derived universal charset detector (pure C++/NSPR stub)
# =============================================================================
set(UCHARDET_SOURCES
    src/uchardet/uchardet.cpp
    src/uchardet/nsUniversalDetector.cpp
    src/uchardet/nsCharSetProber.cpp
    src/uchardet/nsMBCSGroupProber.cpp
    src/uchardet/nsSBCSGroupProber.cpp
    src/uchardet/nsEscCharsetProber.cpp
    src/uchardet/nsLatin1Prober.cpp
    src/uchardet/nsSBCharSetProber.cpp
    src/uchardet/nsBig5Prober.cpp
    src/uchardet/nsEUCTWProber.cpp
    src/uchardet/nsEUCKRProber.cpp
    src/uchardet/nsEUCJPProber.cpp
    src/uchardet/nsGB2312Prober.cpp
    src/uchardet/nsHebrewProber.cpp
    src/uchardet/nsSJISProber.cpp
    src/uchardet/nsUTF8Prober.cpp
    src/uchardet/nsEscSM.cpp
    src/uchardet/nsMBCSSM.cpp
    src/uchardet/nsPkgInt.h
    src/uchardet/nsCodingStateMachine.h
    src/uchardet/CharDistribution.cpp
    src/uchardet/JpCntx.cpp
    # Language model files (no .cpp/.h extension — included as data)
    src/uchardet/LangBulgarianModel.cpp
    src/uchardet/LangCyrillicModel.cpp
    src/uchardet/LangGreekModel.cpp
    src/uchardet/LangHebrewModel.cpp
    src/uchardet/LangHungarianModel.cpp
    src/uchardet/LangThaiModel.cpp
    # Frequency tables (binary data included via #include)
    src/uchardet/Big5Freq.tab
    src/uchardet/EUCKRFreq.tab
    src/uchardet/EUCTWFreq.tab
    src/uchardet/GB2312Freq.tab
    src/uchardet/JISFreq.tab
)

# =============================================================================
# All sources — single authoritative list
# =============================================================================
set(SOURCES
    # Entry point
    src/main.cpp

    # Core — main window and document model
    src/MainWindow.h
    src/MainWindow.cpp

    # Scintilla edit layers — all ScintillaComponent files via ${SCINTILLA_LAYERS}
    ${SCINTILLA_LAYERS}

    # Scintilla base (abstract interface)
    src/ScintillaEditBase.h
    src/ScintillaEditBase.cpp

    # Dark mode
    src/NppDarkMode.h
    src/NppDarkMode.cpp

    # DarkMode subdirectory — Win32 dark mode stubs (Windows 10/11 uxtheme)
    # These bridge the Win32 ordinal-based dark mode API to NppDarkMode (Qt6)
    src/DarkMode/DarkMode.h
    src/DarkMode/DarkMode.cpp
    src/DarkMode/IatHook.h    # PE IAT hooking stubs (no-op on Linux)
    src/DarkMode/UAHMenuBar.h # UAH menu bar dark mode stubs (no-op on Linux)

    # Text encoding
    src/Utf8_16.h
    src/Utf8_16.cpp

    # Small dialogs
    src/lesDlgs.h
    src/lesDlgs.cpp

    # Run dialog (execute command)
    src/RunDlg.h
    src/RunDlg.cpp

    # MD5 checksum dialogs
    src/md5Dlgs.h
    src/md5Dlgs.cpp

    # Document snapshot (file change monitoring)
    src/documentSnapshot.h
    src/documentSnapshot.cpp

    # Process management
    src/Processus.h
    src/Processus.cpp

    # Menu command IDs
    src/menuCmdID.h

    # MISC infrastructure
    ${MISC_COMMON}
    ${MISC_PLUGINS}
    ${MISC_REGEXT}
    ${MISC_PROCESS}

    # Top-level infrastructure (headers)
    ${INFRASTRUCTURE}

    # New utility stubs
    src/MiniDumper.h
    src/Win32Exception.h
    src/Sorters.h
    src/SortLocale.h
    src/FileNameStringSplitter.h
    src/FileNameStringSplitter.cpp
    src/md5.h
    src/sha-256.h
    src/sha512.h
    src/calc_sha1.h
    src/verifySignedfile.h

    # Top-level infrastructure implementations

    # MISC/Common implementations (separate from forwarding headers at root)
    src/MISC/Common/NppNotification.cpp
    src/MISC/Common/dpiManagerV2.cpp
    src/MISC/Common/lastRecentFileList.cpp
    src/MISC/Common/localization.cpp
    src/MISC/Common/shortcut.cpp
    src/MISC/Common/clipboardFormats.h
    src/MISC/Common/keys.h

    # Icon loading infrastructure (3-tier: file → theme → placeholder)
    src/IconLoader.h
    src/IconLoader.cpp

    # Core application files — fully ported to Qt6
    src/Parameters.h
    src/Parameters.cpp
    src/NppIO.h
    # src/NppIO.cpp
    src/NppCommands.h
    src/NppCommands.cpp
    src/NppCommandsSlots_stub.cpp
    src/NppBigSwitch.h
    src/NppBigSwitch.cpp
    src/Notepad_plus.h
    src/Notepad_plus.cpp

    # WinControls (all layers)
    ${WINCONTROLS_BASE}
    ${WINCONTROLS_DIALOGS}
    ${WINCONTROLS_PANELS}
    ${WINCONTROLS_ADVANCED}
    ${WINCONTROLS_UTILS}
    ${WINCONTROLS_BABYGRID}

    # Settings dialogs
    src/Dialogs/PreferenceDialog.h
    src/Dialogs/PreferenceDialog.cpp
    src/Dialogs/StyleConfigDialog.h
    src/Dialogs/StyleConfigDialog.cpp
    src/Dialogs/ShortcutMapperDialog.h
    src/Dialogs/ShortcutMapperDialog.cpp

    # Third-party header-only libraries
    ${THIRDPARTY_HEADERS}

    # uchardet (Mozilla universal charset detector)
    ${UCHARDET_SOURCES}
)

set(HEADERS
    # Core
    src/MainWindow.h
    src/NppDarkMode.h
    src/DarkMode/DarkMode.h
    src/DarkMode/IatHook.h
    src/DarkMode/UAHMenuBar.h
    src/MISC/PluginsManager/PluginInterface.h
    src/Utf8_16.h
    src/lesDlgs.h
    src/menuCmdID.h
    src/Notepad_plus_msgs.h
    src/ScintillaEditBase.h

    # All ScintillaComponent files
    ${SCINTILLA_LAYERS}

    # Parameters + commands (large files)
    src/Parameters.h
    src/Notepad_plus.h
    src/NppCommands.h
    src/NppBigSwitch.h
    src/NppIO.h

    # Infrastructure
    ${INFRASTRUCTURE}

    # MISC
    ${MISC_COMMON}
    ${MISC_PLUGINS}
    ${MISC_REGEXT}

    # WinControls
    ${WINCONTROLS_BASE}
    ${WINCONTROLS_DIALOGS}
    ${WINCONTROLS_PANELS}
    ${WINCONTROLS_ADVANCED}
    ${WINCONTROLS_UTILS}
    ${WINCONTROLS_BABYGRID}
    src/WinControls/FindResultPanel/FindResultPanel.h

    # Third-party header-only libraries
    ${THIRDPARTY_HEADERS}
)

add_executable(npp-qt WIN32
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(npp-qt
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
        Qt6::Core
        Qt6::Xml
        Qt6::PrintSupport
        $<$<TARGET_EXISTS:Qt6::Svg>:Qt6::Svg>
        ${QSCINTILLA2_LIB}
)

# Post-build: copy rgba_icons/ next to binary so IconLoader finds them at runtime
add_custom_command(TARGET npp-qt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/rgba_icons"
        "${CMAKE_CURRENT_BINARY_DIR}/rgba_icons"
    COMMENT "Copying rgba_icons/ to build directory"
)

# Post-build: copy functionList/ XML rules for FunctionList panel
add_custom_command(TARGET npp-qt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/functionList"
        "${CMAKE_CURRENT_BINARY_DIR}/functionList"
    COMMENT "Copying functionList/ XML rules to build directory"
)

# Scintilla and Lexilla headers (from notepad-plus-plus-translation workspace)
set(SCINTILLA_INCLUDE "/home/piotro/.openclaw/workspace/notepad-plus-plus-translation/scintilla/include")
set(LEXILLA_INCLUDE "/home/piotro/.openclaw/workspace/notepad-plus-plus-translation/lexilla/include")

target_include_directories(npp-qt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/MISC/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/WinControls
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/npp-qt_autogen  # WORKAROUND: AUTOMOC mocs_compilation.cpp includes "SUBDIR/moc_Foo.cpp"
    /usr/include/x86_64-linux-gnu/qt6/QtPrintSupport
    ${SCINTILLA_INCLUDE}
    ${LEXILLA_INCLUDE}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(npp-qt PRIVATE QT_DEBUG)
endif()

if(WIN32)
    add_compile_definitions(_UNICODE)
endif()

# -----------------------------------------------------------------------
# Install bundled icon set (SVG + PNG fallbacks)
# -----------------------------------------------------------------------
install(DIRECTORY rgba_icons/
        DESTINATION "${CMAKE_INSTALL_DATADIR}/npp-qt/icons"
        FILES_MATCHING PATTERN "*.svg" PATTERN "*.png")

install(DIRECTORY functionList/
        DESTINATION "${CMAKE_INSTALL_DATADIR}/npp-qt/functionList")

# -----------------------------------------------------------------------
# Tests — enable if Qt6::Test is available (Qt 6.4+)
# -----------------------------------------------------------------------
find_package(Qt6 COMPONENTS Test QUIET)
if(Qt6_FOUND)
    enable_testing()
    add_subdirectory(tests)
    message(STATUS "Tests enabled (Qt6::Test)")
else()
    message(STATUS "Qt6::Test not found — tests disabled")
endif()
