cmake_minimum_required(VERSION 3.16)
project(npp-qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(QScintillaLib COMPONENTS Qt6 QUIET)

# Core features
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/editor.cpp
    src/highlighter.cpp
)

# QScintilla if available
if(QScintillaLib_Qt6_FOUND)
    message(STATUS "QScintilla found - enabling syntax highlighting")
    add_definitions(-DHAVE_QSCINTILLA)
endif()

add_executable(npp-qt ${SOURCES})
target_link_libraries(npp-qt PRIVATE Qt6::Widgets Qt6::PrintSupport)

if(QScintillaLib_Qt6_FOUND)
    target_link_libraries(npp-qt PRIVATE QScintillaLib::QScintilla)
endif()

install(TARGETS npp-qt DESTINATION bin)