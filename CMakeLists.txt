cmake_minimum_required(VERSION 3.16)
project(npp-qt LANGUAGES CXX VERSION 8.9.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include paths (must be before add_executable)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Network Xml)
find_package(Qt6Core5Compat REQUIRED)

# Scintilla subdirectories (lexilla before scintilla since scintilla depends on it)
add_subdirectory(src/ScintillaComponents/lexilla)
add_subdirectory(src/ScintillaComponents/scintilla)

# Core source files
file(GLOB_RECURSE NPP_CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE NPP_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# Create resources if they exist
set(NPP_RESOURCES "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/npp-qt.qrc")
    list(APPEND NPP_RESOURCES resources/npp-qt.qrc)
endif()

# Executable
add_executable(npp-qt WIN32
    ${NPP_CORE_SOURCES}
    ${NPP_HEADERS}
    ${NPP_RESOURCES}
)

# Link libraries
target_link_libraries(npp-qt PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets
    Qt6::PrintSupport Qt6::Network Qt6::Xml
    Qt6::Core5Compat
    scintilla_qt lexilla
)

# Platform-specific definitions
if(WIN32)
    target_compile_definitions(npp-qt PRIVATE
        QT_NO_KEYWORDS NOMINMAX _UNICODE UNICODE
    )
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(npp-qt PRIVATE LINUX)
    target_compile_options(npp-qt PRIVATE -Wall -Wextra)
endif()

install(TARGETS npp-qt DESTINATION bin)
