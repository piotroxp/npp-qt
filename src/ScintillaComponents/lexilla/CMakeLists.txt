cmake_minimum_required(VERSION 3.16)
project(lexilla_static LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE LEXILLA_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Lexilla.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/lexers/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/lexlib/*.cxx"
)
file(GLOB_RECURSE LEXILLA_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_library(lexilla STATIC
    ${LEXILLA_SRC} ${LEXILLA_HDR}
)

target_compile_definitions(lexilla PRIVATE SCINTILLA LEXILLA)
target_include_directories(lexilla PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lexlib"
    "${CMAKE_CURRENT_SOURCE_DIR}/lexers"
    "${CMAKE_CURRENT_SOURCE_DIR}/lexilla"
    "${CMAKE_CURRENT_SOURCE_DIR}/../scintilla/include"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
)
