cmake_minimum_required(VERSION 3.16)
project(scintilla_qt_lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt6Core5Compat REQUIRED)

# Scintilla core
file(GLOB_RECURSE SCI_CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx")
file(GLOB_RECURSE SCI_CORE_HDR "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Include project directories (from parent CMakeLists.txt)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
## included via root
# Qt platform
file(GLOB SCI_PLAT_QT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/qt/ScintillaEditBase/*.cpp")
file(GLOB SCI_PLAT_QT_HDR "${CMAKE_CURRENT_SOURCE_DIR}/qt/ScintillaEditBase/*.h")
# ScintillaEdit widget
file(GLOB SCI_EDIT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/qt/ScintillaEdit/ScintillaEdit.cpp"
                             "${CMAKE_CURRENT_SOURCE_DIR}/qt/ScintillaEdit/ScintillaDocument.cpp")
file(GLOB SCI_EDIT_HDR "${CMAKE_CURRENT_SOURCE_DIR}/qt/ScintillaEdit/ScintillaEdit.h"
                             "${CMAKE_CURRENT_SOURCE_DIR}/qt/ScintillaEdit/ScintillaDocument.h")

add_library(scintilla_qt STATIC
    ${SCI_CORE_SRC} ${SCI_CORE_HDR}
    ${SCI_PLAT_QT_SRC} ${SCI_PLAT_QT_HDR}
    ${SCI_EDIT_SRC} ${SCI_EDIT_HDR}
)

target_compile_definitions(scintilla_qt PRIVATE SCINTILLA_QT=1 MAKING_LIBRARY=1)

target_include_directories(scintilla_qt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qt/ScintillaEditBase>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qt/ScintillaEdit>
)

target_link_libraries(scintilla_qt PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Core5Compat)

# Propagate scintilla include to any dependent target (like lexilla)
target_include_directories(scintilla_qt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
