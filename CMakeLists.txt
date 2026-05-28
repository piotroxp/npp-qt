cmake_minimum_required(VERSION 3.16)
project(npp-qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Core Qt components
find_package(Qt6 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(Qt6Core5Compat REQUIRED)

# QScintilla for advanced editing (optional)
find_package(QScintillaLib COMPONENTS Qt6 QUIET)

# Bundled Scintilla Qt library
add_subdirectory(src/ScintillaComponents/scintilla)


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
)

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
    src/Notepad_plus_Window.cpp
    src/NppBigSwitch.cpp
    src/NppCommands.cpp
    src/NppIO.cpp
    src/NppNotification.cpp
    src/dpiManagerV2.cpp
    src/lastRecentFileList.cpp
    src/lesDlgs.cpp
    src/localization.cpp
    src/winmain.cpp
    src/ScintillaComponent/AutoCompletion.cpp
    src/ScintillaComponent/FunctionCallTip.cpp
    src/ScintillaComponent/Printer.cpp
    src/ScintillaComponent/SmartHighlighter.cpp
    src/ScintillaComponent/xmlMatchedTagsHighlighter.cpp
    src/MISC/sha1/calc_sha1.cpp
    src/uchardet/uchardet.cpp
    src/uchardet/nsUniversalDetector.cpp
    src/uchardet/CharDistribution.cpp
    src/uchardet/JpCntx.cpp
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
    src/uchardet/LangBulgarianModel.cpp
    src/uchardet/LangCyrillicModel.cpp
    src/uchardet/LangGreekModel.cpp
    src/uchardet/LangHebrewModel.cpp
    src/uchardet/LangHungarianModel.cpp
    src/uchardet/LangThaiModel.cpp
)

# Combine all lifted
set(ALL_LIFTED ${LIFTED_CORE} ${LIFTED_ENCODING} ${LIFTED_SCINTILLA} ${LIFTED_WINCONTROLS} ${LIFTED_MISC})

# =============================================================================
# Include directories
# =============================================================================
include_directories(src . include)
include_directories(.)
include_directories(include)
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
include_directories(src/WinControls/Window)
include_directories(src/MISC/sha1)
include_directories(src/uchardet)

# =============================================================================
# Flags
# =============================================================================
if(QScintillaLib_Qt6_FOUND)
    message(STATUS "QScintilla found")
    add_definitions(-DHAVE_QSCINTILLA)
    list(APPEND APP_SOURCES src/editor.cpp src/highlighter.cpp)
else()
    add_definitions(-DNO_SCINTILLA)
endif()

# =============================================================================
# Build
# =============================================================================
add_executable(npp-qt ${APP_SOURCES} ${ALL_LIFTED})
target_link_libraries(npp-qt PRIVATE Qt6::Widgets Qt6::PrintSupport Qt6::Core5Compat scintilla_qt)

if(QScintillaLib_Qt6_FOUND)
    target_link_libraries(npp-qt PRIVATE QScintillaLib::QScintilla)
endif()
