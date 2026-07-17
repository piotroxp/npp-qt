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
# Canonical subdirs — all Qt-native source files live here (Phase1+)
# Each subdir uses file(GLOB_RECURSE so every .cpp/.h is automatically picked up.
# Old Win32 originals at src/X.cpp roots are deleted in Phase0.
# =============================================================================

file(GLOB_RECURSE CORE_SOURCES    "src/core/*.cpp"        "src/core/*.h")
file(GLOB_RECURSE DIALOG_SOURCES  "src/dialogs/*.cpp"     "src/dialogs/*.h")
file(GLOB_RECURSE EDITOR_SOURCES  "src/editor/*.cpp"      "src/editor/*.h")
file(GLOB_RECURSE PANELS_SOURCES  "src/panels/*.cpp"      "src/panels/*.h")
file(GLOB_RECURSE UI_SOURCES      "src/ui/*.cpp"          "src/ui/*.h")
file(GLOB_RECURSE WORKERS_SOURCES "src/workers/*.cpp"     "src/workers/*.h")
file(GLOB_RECURSE PLUGINS_SOURCES "src/plugins/*.cpp"     "src/plugins/*.h")
file(GLOB_RECURSE COMMON_SOURCES  "src/common/*.cpp"      "src/common/*.h")

# MISC/Common and MISC/PluginsManager — canonical home for shared utilities
file(GLOB_RECURSE MISC_COMMON_SOURCES     "src/MISC/Common/*.cpp"         "src/MISC/Common/*.h")
file(GLOB_RECURSE MISC_PLUGINS_SOURCES     "src/MISC/PluginsManager/*.cpp"  "src/MISC/PluginsManager/*.h")

# Third-party header-only libraries
file(GLOB_RECURSE THIRDPARTY_HEADERS "src/json/*.hpp" "src/pugixml/*.hpp")

# uchardet — Mozilla universal charset detector
file(GLOB_RECURSE UCHARDET_SOURCES  "src/uchardet/*.cpp"  "src/uchardet/*.h"
                                      "src/uchardet/*.tab"  "src/uchardet/*.hpp")

# =============================================================================
# Root-level files — only include if they exist (Phase0 may have deleted some)
# After Phase0 the only expected root-level .cpp/.h are:
#   main.cpp, AutoCompletion.cpp/.h, FindReplaceDlg.cpp/.h,
#   FunctionCallTip.cpp, Common.cpp, NppConstants.h, NppSciCompat.h
# All other Win32 originals (MainWindow.cpp, Buffer.cpp, NppDarkMode.cpp, etc.)
# are deleted in Phase0; their Qt-native replacements live in canonical subdirs.
# =============================================================================

# Entry point — must be first in SOURCES so it is compiled first
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    list(APPEND ROOT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
endif()

# Phase2A candidates — include if present (will move to dialogs/ later)
foreach(_file AutoCompletion FindReplaceDlg FunctionCallTip Common)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_file}.cpp")
        list(APPEND ROOT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/${_file}.cpp")
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_file}.h")
        list(APPEND ROOT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_file}.h")
    endif()
endforeach()

# Keep constants and Sci compatibility headers (not compiled — pure headers)
foreach(_file NppConstants NppSciCompat)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_file}.h")
        list(APPEND ROOT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/${_file}.h")
    endif()
endforeach()

# Any other root-level .cpp that slipped through Phase0
file(GLOB _remaining_root_cpp "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB _remaining_root_h   "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
# Exclude already-handled root files to avoid duplicates
list(FILTER _remaining_root_cpp EXCLUDE REGEX "main\\.cpp$|AutoCompletion\\.cpp$|FindReplaceDlg\\.cpp$|FunctionCallTip\\.cpp$|Common\\.cpp$")
list(FILTER _remaining_root_h   EXCLUDE REGEX "AutoCompletion\\.h$|FindReplaceDlg\\.h$|NppConstants\\.h$|NppSciCompat\\.h$")
if(_remaining_root_cpp)
    list(APPEND ROOT_SOURCES ${_remaining_root_cpp})
endif()
if(_remaining_root_h)
    list(APPEND ROOT_HEADERS ${_remaining_root_h})
endif()

# =============================================================================
# All sources — single authoritative list (entry point first)
# =============================================================================

set(SOURCES
    # Entry point
    ${ROOT_SOURCES}

    # Canonical subdirs
    ${CORE_SOURCES}
    ${DIALOG_SOURCES}
    ${EDITOR_SOURCES}
    ${PANELS_SOURCES}
    ${UI_SOURCES}
    ${WORKERS_SOURCES}
    ${PLUGINS_SOURCES}
    ${COMMON_SOURCES}
    ${MISC_COMMON_SOURCES}
    ${MISC_PLUGINS_SOURCES}

    # Third-party
    ${THIRDPARTY_HEADERS}
    ${UCHARDET_SOURCES}
)

set(HEADERS
    ${ROOT_HEADERS}
    ${CORE_SOURCES}
    ${DIALOG_SOURCES}
    ${EDITOR_SOURCES}
    ${PANELS_SOURCES}
    ${UI_SOURCES}
    ${WORKERS_SOURCES}
    ${PLUGINS_SOURCES}
    ${COMMON_SOURCES}
    ${MISC_COMMON_SOURCES}
    ${MISC_PLUGINS_SOURCES}
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
        "${CMAKE_CURRENT_SOURCE_DIR}/rgba_icons"
        "${CMAKE_CURRENT_BINARY_DIR}/rgba_icons"
    COMMENT "Copying rgba_icons/ to build directory"
)

# Scintilla and Lexilla headers (from notepad-plus-plus-translation workspace)
set(SCINTILLA_INCLUDE "/home/node/.openclaw/workspace-employee-blunt/notepad-plus-plus-translation/scintilla/include")
set(LEXILLA_INCLUDE "/home/node/.openclaw/workspace-employee-blunt/notepad-plus-plus-translation/lexilla/include")

target_include_directories(npp-qt PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/MISC/Common
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/editor
    ${CMAKE_SOURCE_DIR}/src/dialogs
    ${CMAKE_SOURCE_DIR}/src/panels
    ${CMAKE_SOURCE_DIR}/src/ui
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
