cmake_minimum_required(VERSION 3.16)

# =============================================================================
# npp-qt test suite — Qt Test integration
# All test targets are built together; `make test` / ctest runs them.
# =============================================================================

# Require Qt Test component (supplied by the Qt6 package found in parent)
find_package(Qt6 REQUIRED COMPONENTS Test)
# Also need Xml for ToolBar.h (QDomDocument)
find_package(Qt6 REQUIRED COMPONENTS Xml)

# Collect test source files
set(TEST_SOURCES
    TestUtf8_16.cpp
    TestNppDarkMode.cpp
    TestColourPicker.cpp
    TestLesDlgs.cpp
    TestCustomFileDialog.cpp
    TestDirectoryWatcher.cpp
    TestImageListSet.cpp
    TestMain.cpp
)

# ---------------------------------------------------------------------------
# Explicitly MOC WindowBase for the test library (AUTOMOC misses it).
# ---------------------------------------------------------------------------
qt6_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/Window.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_Window.cpp)

# ---------------------------------------------------------------------------
# Test helper library — contains pure-logic and widget-free source files
# that tests need to link against.
# Qt6::Xml is included for ToolBar.h (QDomDocument stubs).
# ---------------------------------------------------------------------------
add_library(npp-qt-test-lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Utf8_16.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges/ReadDirectoryChangesPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/NppDarkMode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ImageListSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/StaticDialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/lesDlgs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ColourPicker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/CustomFileDialog.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_Window.cpp
)
target_link_libraries(npp-qt-test-lib PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Xml Qt6::PrintSupport)
target_include_directories(npp-qt-test-lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/MISC
)

# Convenience macro: add one test target
function(add_npp_test NAME)
    add_executable(${NAME} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${NAME}
        PRIVATE
            Qt6::Core
            Qt6::Widgets
            Qt6::Gui
            Qt6::Test
            Qt6::Xml
            npp-qt-test-lib
    )
    target_include_directories(${NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges
    )
    target_compile_definitions(${NAME} PRIVATE
        QT_DISABLE_DEPRECATED_BEFORE=0x060000
        QCUSTOMBUILD_DEBUG=1
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

# --- Unit tests (pure logic, no GUI) ---

add_npp_test(npp-test-utf8_16   TestUtf8_16.cpp)
add_npp_test(npp-test-darkmode  TestNppDarkMode.cpp)
add_npp_test(npp-test-colour    TestColourPicker.cpp)
add_npp_test(npp-test-lesdlgs  TestLesDlgs.cpp)
add_npp_test(npp-test-filedlg  TestCustomFileDialog.cpp)
add_npp_test(npp-test-dirwatch TestDirectoryWatcher.cpp)
add_npp_test(npp-test-iconset  TestImageListSet.cpp)

# Enable test reporting via CTest
include(CTest)
