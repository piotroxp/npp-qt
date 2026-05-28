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

# Core app sources (builds successfully)
set(CORE_SOURCES
    src/main.cpp
    src/mainwindow.cpp
)

# Include directories
include_directories(src)
include_directories(src/MISC/Common)
include_directories(src/MISC/PluginsManager)
include_directories(src/scintilla/include)
include_directories(src/lexilla/include)
include_directories(src/pugixml)

# Flags
if(QScintillaLib_Qt6_FOUND)
    message(STATUS "QScintilla found")
    add_definitions(-DHAVE_QSCINTILLA)
    list(APPEND CORE_SOURCES src/editor.cpp src/highlighter.cpp)
else()
    add_definitions(-DNO_SCINTILLA)
endif()

add_executable(npp-qt ${CORE_SOURCES})
target_link_libraries(npp-qt PRIVATE Qt6::Widgets Qt6::PrintSupport)

if(QScintillaLib_Qt6_FOUND)
    target_link_libraries(npp-qt PRIVATE QScintillaLib::QScintilla)
endif()

install(TARGETS npp-qt DESTINATION bin)
