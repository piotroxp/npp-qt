cmake_minimum_required(VERSION 3.16)

# =============================================================================
# npp-qt test suite — Qt Test integration
# All test targets are built together; `make test` / ctest runs them.
# =============================================================================

# Require Qt Test component (supplied by the Qt6 package found in parent)
find_package(Qt6 REQUIRED COMPONENTS Test)

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

# Convenience macro: add one test target
function(add_npp_test NAME)
    add_executable(${NAME} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${NAME}
        PRIVATE
            Qt6::Core
            Qt6::Widgets
            Qt6::Gui
            Qt6::Test
    )
    target_include_directories(${NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls
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
