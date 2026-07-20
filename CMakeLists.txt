cmake_minimum_required(VERSION 3.20)
project(NotepadMinusMinusQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ─── Qt6 ────────────────────────────────────────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Network Xml PrintSupport)
find_package(Qt6 QUIET COMPONENTS WebEngineWidgets)
if(Qt6WebEngineWidgets_FOUND)
    set(NPP_HAVE_WEBENGINE TRUE)
    message(STATUS "Qt6WebEngineWidgets found — HTML preview enabled")
else()
    set(NPP_HAVE_WEBENGINE FALSE)
    message(STATUS "Qt6WebEngineWidgets NOT found — HTML preview disabled")
endif()

# ─── QScintilla (no CMake configs on Arch Linux) ────────────────────────────
find_package(QScintilla6 QUIET)
if(NOT QScintilla6_FOUND)
    # Ubuntu /usr/include/x86_64-linux-gnu/qt6 layout — no CMake config
    find_library(QSCINTILLA_LIBRARY qscintilla2_qt6 PATHS /usr/lib/x86_64-linux-gnu)
    find_path(QSCINTILLA_INCLUDE_DIR Qsci/qsciscintilla.h
              PATHS /usr/include/x86_64-linux-gnu/qt6 /usr/include /usr/include/qt6)
    if(QSCINTILLA_LIBRARY AND QSCINTILLA_INCLUDE_DIR)
        set(QScintilla6_FOUND TRUE)
        set(QSCINTILLA_LIBRARIES ${QSCINTILLA_LIBRARY})
        set(QSCINTILLA_INCLUDE_DIRS ${QSCINTILLA_INCLUDE_DIR})
        message(STATUS "QScintilla6 found via fallback: ${QSCINTILLA_LIBRARY}")
    endif()
endif()
if(NOT QScintilla6_FOUND)
    message(FATAL_ERROR "QScintilla not found")
endif()

# ─── Platform ─────────────────────────────────────────────────────────────────
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WINDOWS TRUE)
endif()

# ─── Compiler flags ───────────────────────────────────────────────────────────
option(NPP_DEBUG_SANITIZERS "Enable ASAN/UBSAN sanitizers for debugging" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -O0 -g -fno-omit-frame-pointer)
    if(NPP_DEBUG_SANITIZERS)
        message(STATUS "Sanitizers ENABLED (ASAN + UBSAN)")
        add_compile_options(-fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    else()
        message(STATUS "Debug symbols enabled (no sanitizers — set NPP_DEBUG_SANITIZERS=ON to enable)")
    endif()
endif()

# ─── Source files ─────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/core/Buffer.cpp
    src/core/Parameters.cpp
    src/core/Application.cpp
    src/core/EditorCommandManager.cpp
    src/core/EncodingDetector.cpp
    src/core/FileManager.cpp
    src/core/LanguageManager.cpp
    src/core/SessionManager.cpp
    src/core/ShortcutManager.cpp
    src/MISC/Common/shortcut.cpp
    src/core/MacroManager.cpp
    src/core/MacroAction.cpp
    src/core/RecentFilesManager.cpp
    src/core/ThemeManager.cpp
    src/core/NppIO.cpp
    src/core/NppBigSwitch.cpp
    src/core/NppCommands.cpp
    src/core/Notepad_plus.cpp
    src/core/WorkspaceManager.cpp
    src/core/UdlManager.cpp
    src/editor/ScintillaEditor.cpp
    src/editor/ScintillaEditView.cpp
    src/editor/SyntaxHighlighterStub.cpp
    src/editor/DocTabView.cpp
    src/editor/ScintillaCtrls.cpp
    $<$<BOOL:${NPP_HAVE_WEBENGINE}>:src/editor/WebBrowserView.cpp>
    $<$<BOOL:${NPP_HAVE_WEBENGINE}>:src/editor/WebBrowserViewFactory.cpp>
    src/ui/MainWindow.cpp
    src/ui/TabBar.cpp
    src/ui/StatusBar.cpp
    src/ui/ToolBar.cpp
    src/ui/MenuBar.cpp
    src/ui/ColourPicker.cpp
    src/ui/ColourPopup.cpp
    src/ui/WinControls.cpp
    src/dialogs/FindReplaceDialog.cpp
    src/dialogs/GoToLineDialog.cpp
    src/dialogs/PreferenceDialog.cpp
    src/dialogs/AboutDialog.cpp
    src/dialogs/FindInFilesDialog.cpp
    src/dialogs/FindInFilesWorker.cpp
    src/dialogs/ShortcutMapperDialog.cpp
    src/dialogs/CommandPaletteDialog.cpp
    src/dialogs/LargeFileWarningDialog.cpp
    src/dialogs/FileReloadDialog.cpp
    src/dialogs/IncrementalSearchDialog.cpp
    src/dialogs/Printer.cpp
    src/dialogs/ColumnEditorDialog.cpp
    src/dialogs/UserDefineDialog.cpp
    src/dialogs/AnsiCharPanel.cpp
    src/dialogs/SmartHighlighter.cpp
    src/dialogs/FunctionCallTip.cpp
    src/dialogs/AutoCompletion.cpp
    src/dialogs/ClipboardHistoryPanel.cpp
    src/workers/FileLoaderWorker.cpp
    src/panels/DocumentMapPanel.cpp
    src/panels/FunctionListPanel.cpp
    src/panels/FunctionListXmlParser.cpp
    src/panels/FileBrowserPanel.cpp
    src/common/DpiManager.cpp
    src/common/MenuAdapter.cpp
    src/common/FileWatcherAdapter.cpp
    src/common/ShortcutAdapter.cpp
    src/plugins/PluginManager.cpp
    src/common/Util.cpp
    src/common/FileHelper.cpp
    src/common/StringHelper.cpp
    src/core/NppNotificationHandler.cpp
    src/core/NppSciNotificationBridge.cpp
    src/MISC/Common/NppNotification.cpp
    # src/ui/Accelerator.cpp            # disabled: duplicate impl in shortcut.cpp
    # src/ui/ScintillaAccelerator.cpp   # disabled: duplicate impl in shortcut.cpp
)

# ─── Header files ─────────────────────────────────────────────────────────────
set(HEADERS
    src/core/Buffer.h
    src/core/Application.h
    src/core/ThemeManager.h
    src/core/NppIO.h
    src/core/EditorCommandManager.h
    src/core/EncodingDetector.h
    src/core/FileManager.h
    src/core/LanguageManager.h
    src/core/SessionManager.h
    src/core/ShortcutManager.h
    src/core/MacroManager.h
    src/core/WorkspaceManager.h
    src/core/UdlManager.h
    src/core/Parameters.h
    src/core/NppCommands.h
    src/core/NppBigSwitch.h
    src/core/Notepad_plus.h
    src/core/RecentFilesManager.h
    src/editor/ScintillaEditor.h
    src/editor/ScintillaEditView.h
    src/editor/SyntaxHighlighter.h
    src/editor/DocTabView.h
    src/editor/ScintillaCtrls.h
    $<$<BOOL:${NPP_HAVE_WEBENGINE}>:src/editor/WebBrowserView.h>
    $<$<BOOL:${NPP_HAVE_WEBENGINE}>:src/editor/WebBrowserViewFactory.h>
    src/ui/ColourPicker.h
    src/ui/ColourPopup.h
    src/ui/MainWindow.h
    src/ui/TabBar.h
    src/ui/StatusBar.h
    src/ui/ToolBar.h
    src/ui/MenuBar.h
    src/dialogs/FindReplaceDialog.h
    src/dialogs/FindInFilesWorker.h
    src/dialogs/GoToLineDialog.h
    src/dialogs/PreferenceDialog.h
    src/dialogs/AboutDialog.h
    src/dialogs/ShortcutMapperDialog.h
    src/dialogs/CommandPaletteDialog.h
    src/dialogs/LargeFileWarningDialog.h
    src/dialogs/FileReloadDialog.h
    src/dialogs/IncrementalSearchDialog.h
    src/dialogs/Printer.h
    src/dialogs/ColumnEditorDialog.h
    src/dialogs/UserDefineDialog.h
    src/dialogs/AnsiCharPanel.h
    src/dialogs/SmartHighlighter.h
    src/dialogs/FunctionCallTip.h
    src/dialogs/AutoCompletion.h
    src/dialogs/ClipboardHistoryPanel.h
    src/dialogs/FindInFilesDialog.h
    src/workers/FileLoaderWorker.h
    src/panels/DocumentMapPanel.h
    src/panels/FunctionListPanel.h
    src/panels/FunctionListXmlParser.h
    src/panels/FileBrowserPanel.h
    src/common/Constants.h
    src/common/Types.h
    src/common/Util.h
    src/common/FileHelper.h
    src/common/StringHelper.h
    src/common/DpiManager.h
    src/common/ShortcutAdapter.h
    src/common/MenuAdapter.h
    src/common/FileWatcherAdapter.h
    src/common/NonCopyable.h
    src/common/Observer.h
    src/common/PathHelper.h
    src/common/UrlHelper.h
    src/common/ScintillaComponent.h
    src/plugins/PluginInterface.h
    src/NppConstants.h
    src/MISC/Common/shortcut.h
    src/MISC/Common/keys.h
    src/MISC/Common/NppNotification.h
)

# ─── Executable ───────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${WINCONTROL_HEADERS})

# Suppress RPATH warning with Ninja generator
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN/../lib:/usr/lib/qt6/plugins"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    src/common
    src/dialogs
    src/ui
    src/panels
    src/core
    src/editor
    src/WinControls
    ${QSCINTILLA_INCLUDE_DIRS}
)

# ─── Link libraries ───────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    Qt6::Network
    Qt6::Xml
    Qt6::PrintSupport
    ${QSCINTILLA_LIBRARIES}
)

if(NPP_HAVE_WEBENGINE)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::WebEngineWidgets)
endif()
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY themes/ DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/notepad--qt/themes OPTIONAL)
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/notepad--qt)

# ─── Tests ───────────────────────────────────────────────────────────────────
enable_testing()
find_package(Qt6 COMPONENTS Test REQUIRED)
add_subdirectory(src/tests)
