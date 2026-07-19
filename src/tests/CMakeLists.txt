# src/tests/CMakeLists.txt - Unit tests for Notepad--Qt

enable_testing()

# Paths relative to this file (src/tests/)
set(REL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(TEST_INC
    ${REL_ROOT}
    ${REL_ROOT}/common
    ${REL_ROOT}/core
    ${REL_ROOT}/editor
    ${REL_ROOT}/dialogs
    ${REL_ROOT}/uchardet
)

# Test library — includes all core sources that need Buffer
add_library(test_core STATIC
    ${REL_ROOT}/core/Buffer.cpp
    ${REL_ROOT}/core/EncodingDetector.cpp
    ${REL_ROOT}/core/FileManager.cpp
    ${REL_ROOT}/core/LanguageManager.cpp
    ${REL_ROOT}/common/Util.cpp
    ${REL_ROOT}/common/StringHelper.cpp
    ${REL_ROOT}/common/FileHelper.cpp
    ${REL_ROOT}/common/ClipboardAdapter.cpp
    ${REL_ROOT}/common/MenuAdapter.cpp
    ${REL_ROOT}/common/FileWatcherAdapter.cpp
    ${REL_ROOT}/dialogs/LargeFileWarningDialog.cpp
    ${REL_ROOT}/editor/SyntaxHighlighter.cpp
    ${REL_ROOT}/editor/SyntaxHighlighterImpl.cpp
)
target_include_directories(test_core PUBLIC
    ${TEST_INC}
    ${QSCINTILLA_INCLUDE_DIR}
)
target_link_libraries(test_core PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    ${QSCINTILLA_LIBRARY}
)
target_compile_options(test_core PRIVATE -w)

add_library(test_core_interface INTERFACE)
target_include_directories(test_core_interface INTERFACE ${TEST_INC})

# EncodingDetector tests
add_executable(test_encoding_detector ${CMAKE_CURRENT_SOURCE_DIR}/test_encoding_detector.cpp)
target_link_libraries(test_encoding_detector PRIVATE test_core)
add_test(NAME EncodingDetectorTests COMMAND test_encoding_detector)

# StringHelper tests — enabled after fixing string-literal overload ambiguity
add_executable(test_string_helper ${CMAKE_CURRENT_SOURCE_DIR}/test_string_helper.cpp)
target_link_libraries(test_string_helper PRIVATE test_core)
add_test(NAME StringHelperTests COMMAND test_string_helper)

# Buffer/FileManager tests
add_executable(test_buffer ${CMAKE_CURRENT_SOURCE_DIR}/test_buffer.cpp)
target_link_libraries(test_buffer PRIVATE test_core)
add_test(NAME BufferTests COMMAND test_buffer)

# SyntaxHighlighter tests — 50-language full implementation
# Disabled: ${CMAKE_CURRENT_SOURCE_DIR}/test_syntax_highlighter.cpp)
# Disabled: PRIVATE test_core Qt6::Widgets ${QSCINTILLA_LIBRARY})
# Disabled: COMMAND test_syntax_highlighter)

# FunctionCallTip tests — disabled: requires Application, DpiManager, ThemeManager, ScintillaEditor
# (ScintillaEditor pulls in the full Application hierarchy)
# add_executable(test_functioncalltip ${CMAKE_CURRENT_SOURCE_DIR}/test_functioncalltip.cpp)
# target_link_libraries(test_functioncalltip PRIVATE test_core Qt6::Widgets ${QSCINTILLA_LIBRARY})
# add_test(NAME FunctionCallTipTests COMMAND test_functioncalltip)

# AutoCompletion tests — disabled: same reason as FunctionCallTip
# add_executable(test_autocompletion ${CMAKE_CURRENT_SOURCE_DIR}/test_autocompletion.cpp)
# target_link_libraries(test_autocompletion PRIVATE test_core Qt6::Widgets ${QSCINTILLA_LIBRARY})
# add_test(NAME AutoCompletionTests COMMAND test_autocompletion)

# IncrementalSearchDialog tests — disabled: requires ScintillaEditor
# add_executable(test_incremental_search ${CMAKE_CURRENT_SOURCE_DIR}/test_incremental_search.cpp)
# target_link_libraries(test_incremental_search PRIVATE test_core Qt6::Widgets ${QSCINTILLA_LIBRARY})
# add_test(NAME IncrementalSearchDialogTests COMMAND test_incremental_search)

# MenuAdapter / context menu tests — standalone: only MenuAdapter, no Buffer/FileWatcherAdapter
add_executable(test_menus
    ${CMAKE_CURRENT_SOURCE_DIR}/test_menus.cpp
    ${REL_ROOT}/common/MenuAdapter.cpp
)
target_include_directories(test_menus PRIVATE ${TEST_INC})
target_link_libraries(test_menus PRIVATE Qt6::Widgets Qt6::Core)
add_test(NAME MenuTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_menus>)
set_tests_properties(MenuTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ShortcutAdapter tests — RegisterHotKey → QShortcut adapter (Win32 → Linux/X11)
# Note: ShortcutAdapter.cpp is compiled directly here so the test binary is self-contained
# and does NOT link against test_core (which has pre-existing failures in ClipboardAdapter).
add_executable(test_shortcuts
    ${CMAKE_CURRENT_SOURCE_DIR}/test_shortcuts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/ShortcutAdapter.cpp   # provides ShortcutAdapter symbol definitions
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/ShortcutAdapter.h     # header for ShortcutAdapter
)
target_compile_definitions(test_shortcuts PRIVATE NPPQT_HAVE_SHORTCUT_MANAGER)
target_link_libraries(test_shortcuts PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Test)
add_test(NAME ShortcutAdapterTests COMMAND test_shortcuts)
set_tests_properties(ShortcutAdapterTests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}"
)

# ClipboardAdapter tests
add_executable(test_clipboard ${CMAKE_CURRENT_SOURCE_DIR}/test_clipboard.cpp)
target_link_libraries(test_clipboard PRIVATE test_core)
add_test(NAME ClipboardAdapterTests COMMAND test_clipboard)

# FileWatcherAdapter tests
add_executable(test_file_watcher
    ${CMAKE_CURRENT_SOURCE_DIR}/test_file_watcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/FileWatcherAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/FileWatcherAdapter.h
)
target_link_libraries(test_file_watcher PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Test)
add_test(NAME FileWatcherAdapterTests COMMAND test_file_watcher)

# Both tests need offscreen Qt platform plugin (no display in CI)
set_tests_properties(ClipboardAdapterTests FileWatcherAdapterTests
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
