cmake_minimum_required(VERSION 3.20)
project(NotepadMinusMinusQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Sql Network Xml PrintSupport)

# QScintilla detection (no CMake configs on Arch Linux)
find_package(QScintilla6 QUIET)
if(NOT QScintilla6_FOUND)
    find_library(QSCINTILLA_LIBRARY NAMES qscintilla2_qt6 qscintilla2)
    find_path(QSCINTILLA_INCLUDE_DIR NAMES Qsci/qsciscintilla.h PATHS /usr/include/qt6 /usr/include /usr/include/x86_64-linux-gnu/qt6)
    if(QSCINTILLA_LIBRARY AND QSCINTILLA_INCLUDE_DIR)
        set(QScintilla6_FOUND TRUE)
        set(QSCINTILLA_LIBRARIES ${QSCINTILLA_LIBRARY})
        set(QSCINTILLA_INCLUDE_DIRS ${QSCINTILLA_INCLUDE_DIR})
    endif()
endif()
if(NOT QScintilla6_FOUND)
    message(FATAL_ERROR "QScintilla not found")
endif()

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WINDOWS TRUE)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/core/Buffer.cpp
    src/core/Application.cpp
    src/core/EditorCommandManager.cpp
    src/core/EncodingDetector.cpp
    src/core/FileManager.cpp
    src/core/LanguageManager.cpp
    src/core/SessionManager.cpp
    src/editor/ScintillaEditor.cpp
    src/editor/SyntaxHighlighter.cpp
    src/ui/MainWindow.cpp
    src/ui/TabBar.cpp
    src/ui/StatusBar.cpp
    src/ui/ToolBar.cpp
    src/ui/MenuBar.cpp
    src/dialogs/FindReplaceDialog.cpp
    src/dialogs/GoToLineDialog.cpp
    src/dialogs/PreferenceDialog.cpp
    src/dialogs/AboutDialog.cpp
    src/panels/DocumentMapPanel.cpp
    src/panels/FunctionListPanel.cpp
    src/panels/FileBrowserPanel.cpp
    src/common/Util.cpp
    src/common/FileHelper.cpp
    src/common/StringHelper.cpp
)

set(HEADERS
    src/core/Buffer.h
    src/core/Application.h
    src/core/EditorCommandManager.h
    src/core/EncodingDetector.h
    src/core/FileManager.h
    src/core/LanguageManager.h
    src/core/SessionManager.h
    src/editor/ScintillaEditor.h
    src/editor/SyntaxHighlighter.h
    src/ui/MainWindow.h
    src/ui/TabBar.h
    src/ui/StatusBar.h
    src/ui/ToolBar.h
    src/ui/MenuBar.h
    src/dialogs/FindReplaceDialog.h
    src/dialogs/GoToLineDialog.h
    src/dialogs/PreferenceDialog.h
    src/dialogs/AboutDialog.h
    src/panels/DocumentMapPanel.h
    src/panels/FunctionListPanel.h
    src/panels/FileBrowserPanel.h
    src/common/Constants.h
    src/common/Types.h
    src/common/Util.h
    src/common/FileHelper.h
    src/common/StringHelper.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src src/common)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    Qt6::Sql
    Qt6::Network
    Qt6::Xml
    Qt6::PrintSupport
    ${QSCINTILLA_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${QSCINTILLA_INCLUDE_DIRS})

# Post-build copy to /workspace/
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        /workspace/NotepadMinusMinusQt
    COMMENT "Copying binary to /workspace/"
)

# Installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Testing
enable_testing()
find_package(Qt6 COMPONENTS Test REQUIRED)
add_subdirectory(src/tests)
