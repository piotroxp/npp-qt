cmake_minimum_required(VERSION 3.16)

# =============================================================================
# npp-qt test suite — Qt Test integration
# All test targets are built together; `make test` / ctest runs them.
# =============================================================================

find_package(Qt6 REQUIRED COMPONENTS Test)
find_package(Qt6 REQUIRED COMPONENTS Xml)

# Global fallback: run all Qt tests headless.
# Individual tests can override via set_tests_properties() if needed.
set(CTEST_ENVIRONMENT "QT_QPA_PLATFORM=offscreen;LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$ENV{LD_LIBRARY_PATH}")

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

# MOC WindowBase for the test library (AUTOMOC misses it).
qt6_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/Window.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_Window.cpp)

# ---------------------------------------------------------------------------
# npp-qt-test-lib — pure-logic sources only.
# No Qt6::Widgets/Gui so tests run without XDG_RUNTIME_DIR.
# ---------------------------------------------------------------------------
add_library(npp-qt-test-lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Utf8_16.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges/ReadDirectoryChangesPrivate.cpp
)
target_link_libraries(npp-qt-test-lib PRIVATE Qt6::Core Qt6::Xml)
target_include_directories(npp-qt-test-lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/MISC
)

# ---------------------------------------------------------------------------
# npp-qt-test-lib-gui — sources that need Qt6::Widgets/Gui.
# This lib links Qt6::Widgets so the Qt GUI subsystem initializes.
# Its tests run with XDG_RUNTIME_DIR set via ctest ENVIRONMENT.
# ---------------------------------------------------------------------------
add_library(npp-qt-test-lib-gui STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/NppDarkMode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ImageListSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/StaticDialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/lesDlgs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ColourPicker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ColourPicker/ColourPopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ColourPicker/ColourPopupResource.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/CustomFileDialog.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_Window.cpp
)
target_link_libraries(npp-qt-test-lib-gui PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Xml Qt6::PrintSupport npp-qt-test-lib
)
target_include_directories(npp-qt-test-lib-gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/MISC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/ScintillaComponent
)

# ---------------------------------------------------------------------------
# Test macros
# ---------------------------------------------------------------------------
function(add_npp_test_pure NAME)
    add_executable(${NAME} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${NAME} PRIVATE Qt6::Core Qt6::Test Qt6::Xml npp-qt-test-lib)
    set_target_properties(${NAME} PROPERTIES BUILD_RPATH "/usr/lib/x86_64-linux-gnu")
    target_include_directories(${NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges
    )
    target_compile_definitions(${NAME} PRIVATE
        QT_DISABLE_DEPRECATED_BEFORE=0x060000 QCUSTOMBUILD_DEBUG=1
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
    # Disable Qt6::Test subprocess mode to prevent both the hang and the X display
    # dependency. Pure tests are unit-style and don't need test isolation.
    set_tests_properties(${NAME} PROPERTIES
        ENVIRONMENT "LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH};QT_QPA_PLATFORM=offscreen;QT_TEST_SUBPROCESS=no"
    )
endfunction()

function(add_npp_test_gui NAME)
    add_executable(${NAME} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${NAME} PRIVATE
        Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Test Qt6::Xml npp-qt-test-lib-gui
    )
    set_target_properties(${NAME} PROPERTIES BUILD_RPATH "/usr/lib/x86_64-linux-gnu")
    target_include_directories(${NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/WinControls/ReadDirectoryChanges
    )
    target_compile_definitions(${NAME} PRIVATE
        QT_DISABLE_DEPRECATED_BEFORE=0x060000 QCUSTOMBUILD_DEBUG=1
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
    set_tests_properties(${NAME} PROPERTIES
        ENVIRONMENT "LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH};XDG_RUNTIME_DIR=/tmp/npp-test-runtime;QT_QPA_PLATFORM=offscreen;QT_TEST_SUBPROCESS=no"
    )
endfunction()

# Pure unit tests — no Qt6::Widgets dependency, no XDG_RUNTIME_DIR needed.
# NOTE: Do NOT set QT_QPA_PLATFORM=offscreen for Qt6Test-based tests — it causes
# Qt6Test's subprocess runner to hang on exit (known Qt6 bug, not our code).
add_npp_test_pure(npp-test-utf8_16   TestUtf8_16.cpp)
add_npp_test_pure(npp-test-dirwatch TestDirectoryWatcher.cpp)

# GUI tests — Qt6::Widgets required; XDG_RUNTIME_DIR set at runtime via ctest
add_npp_test_gui(npp-test-darkmode  TestNppDarkMode.cpp)
add_npp_test_gui(npp-test-colour    TestColourPicker.cpp)
add_npp_test_gui(npp-test-lesdlgs  TestLesDlgs.cpp)
add_npp_test_gui(npp-test-filedlg  TestCustomFileDialog.cpp)
add_npp_test_gui(npp-test-iconset  TestImageListSet.cpp)
add_npp_test_gui(npp-test-plugins  TestPluginsManager.cpp)

include(CTest)
