cmake_minimum_required(VERSION 3.16)

enable_testing()
include(CTest)

# =============================================================================
# npp-qt test suite — Qt Test integration
# All test targets are built together; `make test` / ctest runs them.
# Canonical subdir structure (Phase1+):
#   src/core/         — Application, Buffer, EncodingDetector, FileManager, etc.
#   src/common/       — StringHelper, FileHelper, ScintillaComponent, etc.
#   src/dialogs/      — AutoCompletion, FindReplaceDialog, PreferenceDialog, etc.
#   src/editor/       — ScintillaEditor, SyntaxHighlighter
#   src/ui/           — MainWindow, MenuBar, StatusBar, TabBar, ToolBar
#   src/uchardet/    — charset detection
# =============================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Xml PrintSupport Test)

# QScintilla / Scintilla includes (must match main project)
find_path(SCINTILLA_INCLUDE Qsci/qsciscintilla.h PATHS /usr/include/x86_64-linux-gnu/qt6)
set(LEXILLA_INCLUDE "")  # not needed for test targets

set(CTEST_ENVIRONMENT "QT_QPA_PLATFORM=offscreen;LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$ENV{LD_LIBRARY_PATH}")

# ─── Test library (pure logic, no Widgets/Gui) ────────────────────────────────
#
# Source files that have no Qt widget dependency and can be unit-tested
# in a headless environment.
# npp-qt phase1 canonical locations:
#   src/core/EncodingDetector.cpp
#   src/common/StringHelper.cpp
#   src/common/FileHelper.cpp
#   src/uchardet/*.cpp

# Check for uchardet sources — build system sets USE_UCHARDET if real uchardet is available
set(UCHARDET_SOURCES "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../src/uchardet/CharsetDetector.cpp")
    set(UCHARDET_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/uchardet/CharsetDetector.cpp
    )
    # When uchardet is available, try to find the system uchardet library
    find_library(UCHARDET_LIBRARY NAMES uchardet)
    if(UCHARDET_LIBRARY)
        set(USE_UCHARDET TRUE)
        add_definitions(-DUSE_UCHARDET)
        message(STATUS "Using system uchardet library: ${UCHARDET_LIBRARY}")
    else()
        message(STATUS "uchardet sources found but library not found; using Qt fallback")
    endif()
else()
    message(STATUS "uchardet directory not found; charset detection will use Qt fallback only")
endif()

add_library(npp-qt-test-lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/EncodingDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/LanguageManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/common/StringHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/common/FileHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/common/Util.cpp
)
target_link_libraries(npp-qt-test-lib PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml ${QSCINTILLA_LIBRARY})
target_include_directories(npp-qt-test-lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/MISC/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/uchardet
    ${SCINTILLA_INCLUDE}
)

# ─── GUI test library (requires Qt6::Widgets/Gui) ─────────────────────────────
#
# Source files that need Qt6::Widgets or Qt6::Gui.
# XDG_RUNTIME_DIR is set at runtime via ctest ENVIRONMENT.

add_library(npp-qt-test-lib-gui STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/common/Util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/common/DpiManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/StatusBar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dialogs/AboutDialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dialogs/FileReloadDialog.cpp
)
set_target_properties(npp-qt-test-lib-gui PROPERTIES AUTOMOC ON)
target_link_libraries(npp-qt-test-lib-gui PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Xml Qt6::PrintSupport
    Qt6::Test
    ${QSCINTILLA_LIBRARY}
    npp-qt-test-lib
)
target_include_directories(npp-qt-test-lib-gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/MISC/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/uchardet
    ${SCINTILLA_INCLUDE}
    ${LEXILLA_INCLUDE}
)

# QScintilla library for ScintillaEditor
find_library(QSCINTILLA_LIBRARY NAMES qscintilla2_qt6 qscintilla2_qt5 qscintilla2 PATHS /usr/lib/x86_64-linux-gnu)
if(QSCINTILLA_LIBRARY)
    message(STATUS "QScintilla found: ${QSCINTILLA_LIBRARY}")
endif()


# ─── Test helper macros ────────────────────────────────────────────────────────

function(add_npp_test_pure NAME)
    add_executable(${NAME} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${NAME} PRIVATE Qt6::Core Qt6::Test Qt6::Xml npp-qt-test-lib)
    set_target_properties(${NAME} PROPERTIES
        BUILD_RPATH "/usr/lib/x86_64-linux-gnu"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        AUTOMOC ON
        AUTORCC OFF
        AUTOUIC OFF
    )
    target_include_directories(${NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/common
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/MISC/Common
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/uchardet
        ${CMAKE_BINARY_DIR}/${NAME}_autogen/include  # Qt AUTOMOC .moc files
    )
    target_compile_definitions(${NAME} PRIVATE
        QT_DISABLE_DEPRECATED_BEFORE=0x060000
        TEST_BUILD
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
    set_tests_properties(${NAME} PROPERTIES
        ENVIRONMENT "LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH};QT_QPA_PLATFORM=offscreen;QT_TEST_SUBPROCESS=no"
    )
endfunction()

function(add_npp_test_gui NAME)
    add_executable(${NAME} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${NAME} PRIVATE
        Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Test Qt6::Xml Qt6::PrintSupport npp-qt-test-lib-gui
    )
    set_target_properties(${NAME} PROPERTIES
        BUILD_RPATH "/usr/lib/x86_64-linux-gnu"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        AUTOMOC ON
    )
    target_include_directories(${NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/common
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/panels
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/MISC/Common
        ${CMAKE_BINARY_DIR}/${NAME}_autogen/include  # Qt AUTOMOC .moc files
    )
    target_compile_definitions(${NAME} PRIVATE
        QT_DISABLE_DEPRECATED_BEFORE=0x060000
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
    set_tests_properties(${NAME} PROPERTIES
        ENVIRONMENT "LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH};XDG_RUNTIME_DIR=/tmp/npp-test-runtime;QT_QPA_PLATFORM=offscreen;QT_TEST_SUBPROCESS=no"
    )
endfunction()

# ─── Test cases ───────────────────────────────────────────────────────────────
#
# Phase2 note: Original Win32 tests referenced:
#   TestUtf8_16        → uses src/Utf8_16.cpp          (deleted — port needed)
#   TestNppDarkMode    → uses src/NppDarkMode.cpp      (deleted — use src/common/)
#   TestColourPicker   → uses src/WinControls/ColourPicker/*  (deleted)
#   TestLesDlgs        → uses src/lesDlgs.cpp         (deleted)
#   TestCustomFileDialog → uses src/WinControls/CustomFileDialog.cpp (deleted)
#   TestDirectoryWatcher → uses src/WinControls/ReadDirectoryChanges/* (deleted)
#   TestImageListSet   → uses src/WinControls/ImageListSet.cpp (deleted)
#   TestMain           → test harness
#
# New tests below use canonical phase1 sources. Win32 tests need porting.

add_npp_test_pure(npp-test-encoding  TestEncodingDetector.cpp)
add_npp_test_pure(npp-test-string   TestStringHelper.cpp)
add_npp_test_pure(npp-test-file     TestFileHelper.cpp)

add_npp_test_gui(npp-test-lexer     TestLexer.cpp)
add_npp_test_gui(npp-test-statusbar TestStatusBar.cpp)
add_npp_test_gui(npp-test-dialogs   TestDialogs.cpp)

# ─── Startup / integration smoke tests ─────────────────────────────────────────
# Headless tests that verify source patterns and build configuration.
# These tests run without a display using QT_QPA_PLATFORM=offscreen.
add_npp_test_pure(npp-test-startup startup_test.cpp)
add_npp_test_pure(npp-test-integration integration_test.cpp)
add_npp_test_pure(npp-test-integration-editor integration_editor_test.cpp)
