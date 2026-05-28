cmake_minimum_required(VERSION 3.16)
project(npp-qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Core Qt components
find_package(Qt6 COMPONENTS Widgets PrintSupport REQUIRED)

# QScintilla for advanced editing (optional)
find_package(QScintillaLib COMPONENTS Qt6 QUIET)

# =============================================================================
# Main app sources
# =============================================================================
set(APP_SOURCES
    src/main.cpp
)

# =============================================================================
# LIFTED from notepad-plus-plus-translation
# =============================================================================

# Core lifted sources  
set(LIFTED_CORE
    src/Notepad_plus.cpp
    src/Parameters.cpp
    src/NppDarkMode.cpp
    src/NativeLangSpeaker.cpp
    src/EncodingMapper.cpp
    src/LastRecentFileList.cpp
    src/mainwindow.cpp
)

# Encoding / file handling
set(LIFTED_ENCODING
    src/Utf8_16.cpp
    src/NppXml.cpp
)

# ScintillaComponent (core - the editor widget)
set(LIFTED_SCINTILLA
    src/ScintillaComponent/ScintillaEditView.cpp
    src/ScintillaComponent/Buffer.cpp
    src/ScintillaComponent/ScintillaCtrls.cpp
    src/ScintillaComponent/GoToLineDlg.cpp
    src/ScintillaComponent/FindReplaceDlg.cpp
    src/ScintillaComponent/columnEditor.cpp
    src/ScintillaComponent/FindCharsInRange.cpp
    src/ScintillaComponent/DocTabView.cpp
    src/ScintillaComponent/FileManager.cpp
)

# WinControls (UI components)
set(LIFTED_WINCONTROLS
    src/WinControls/ContextMenu/ContextMenu.cpp
)

# MISC helpers
set(LIFTED_MISC
    src/MISC/Common/Common.cpp
)

# Combine all lifted
set(ALL_LIFTED ${LIFTED_CORE} ${LIFTED_ENCODING} ${LIFTED_SCINTILLA} ${LIFTED_WINCONTROLS} ${LIFTED_MISC})

# =============================================================================
# Include directories
# =============================================================================
include_directories(src)
include_directories(src/MISC/Common)
include_directories(src/MISC/PluginsManager)
include_directories(src/scintilla/include)
include_directories(src/lexilla/include)
include_directories(src/pugixml)
include_directories(src/WinControls)
include_directories(src/WinControls/DockingWnd)
include_directories(src/WinControls/StaticDialog)
include_directories(src/WinControls/TreeView)
include_directories(src/WinControls/ColourPicker)
include_directories(src/WinControls/ToolTip)
include_directories(src/WinControls/ImageListSet)
include_directories(src/WinControls/DoubleBuffer)
include_directories(src/WinControls/Window.h)

# =============================================================================
# Flags
# =============================================================================
if(QScintillaLib_Qt6_FOUND)
    message(STATUS "QScintilla found")
    add_definitions(-DHAVE_QSCINTILLA)
    list(APPEND APP_SOURCES src/editor.cpp src/highlighter.cpp)
else()
    add_definitions(-DNO_SCINTILLA)
    message(STATUS "QScintilla NOT found - using QPlainTextEdit fallback")
endif()

# =============================================================================
# Build
# =============================================================================
add_executable(npp-qt ${APP_SOURCES} ${ALL_LIFTED})
target_link_libraries(npp-qt PRIVATE Qt6::Widgets Qt6::PrintSupport)

if(QScintillaLib_Qt6_FOUND)
    target_link_libraries(npp-qt PRIVATE QScintillaLib::QScintilla)
endif()

install(TARGETS npp-qt DESTINATION bin)
