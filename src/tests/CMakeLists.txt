# src/tests/CMakeLists.txt - Unit tests for Notepad--Qt

enable_testing()

set(REL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(TEST_INC
    ${REL_ROOT}
    ${REL_ROOT}/common
    ${REL_ROOT}/core
    ${REL_ROOT}/editor
    ${REL_ROOT}/dialogs
    ${REL_ROOT}/uchardet
)

# ── MOC generation for Q_OBJECT classes not found by AUTOMOC ───────────────────
set(MOC_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/moc_gen")
file(MAKE_DIRECTORY "${MOC_GEN_DIR}")

function(add_moc HDR OUT_NAME)
    set(MOC_OUT "${MOC_GEN_DIR}/moc_${OUT_NAME}.cpp")
    add_custom_command(
        OUTPUT "${MOC_OUT}"
        COMMAND Qt6::moc ARGS "${REL_ROOT}/${HDR}" -o "${MOC_OUT}"
        DEPENDS "${REL_ROOT}/${HDR}"
        COMMENT "Running moc on ${HDR}"
        VERBATIM
    )
    list(APPEND MOC_SOURCES "${MOC_OUT}")
    set(MOC_SOURCES "${MOC_SOURCES}" PARENT_SCOPE)
endfunction()

add_moc("core/Application.h" Application)
add_moc("common/DpiManager.h" DpiManager)
add_moc("core/ThemeManager.h" ThemeManager)
add_moc("panels/FileBrowserPanel.h" FileBrowserPanel)
add_moc("panels/DocumentMapPanel.h" DocumentMapPanel)
add_moc("workers/FileLoaderWorker.h" FileLoaderWorker)

# ── Stub library — AUTOMOC OFF (our explicit moc provides all Q_OBJECT symbols) ───────
# Panel .cpp files are here (not test_core) so MOC vtables link into this SHARED lib.
add_library(test_stubs SHARED
    StubApplication.cpp
    ${MOC_SOURCES}
    ${REL_ROOT}/panels/FileBrowserPanel.cpp
    ${REL_ROOT}/workers/FileLoaderWorker.cpp
    ${REL_ROOT}/panels/DocumentMapPanel.cpp
)
set_target_properties(test_stubs PROPERTIES AUTOMOC OFF)
target_include_directories(test_stubs PUBLIC ${TEST_INC} ${REL_ROOT}/workers ${QSCINTILLA_INCLUDE_DIR})
target_link_libraries(test_stubs PUBLIC Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Xml Qt6::PrintSupport Qt6::Network)
target_compile_options(test_stubs PRIVATE -w)

# ── Test library (core sources only — no Q_OBJECT classes) ─────────────────────
add_library(test_core STATIC
    ${REL_ROOT}/core/Buffer.cpp
    ${REL_ROOT}/core/EncodingDetector.cpp
    ${REL_ROOT}/core/FileManager.cpp
    # CharsetDetector.cpp must be in test_core because EncodingDetector.h now
    # includes its header and instantiates CharsetDetector in detectWithHints.
    # The main app links it via UCHARDET_SOURCES glob in src/CMakeLists.txt,
    # but the test build needs the .cpp explicitly.
    ${REL_ROOT}/uchardet/CharsetDetector.cpp
    ${REL_ROOT}/core/LanguageManager.cpp
    ${REL_ROOT}/core/SessionManager.cpp
    ${REL_ROOT}/core/MacroManager.cpp
    ${REL_ROOT}/core/MacroAction.cpp
    ${REL_ROOT}/common/Util.cpp
    ${REL_ROOT}/common/StringHelper.cpp
    ${REL_ROOT}/common/FileHelper.cpp
    ${REL_ROOT}/common/ClipboardAdapter.cpp
    ${REL_ROOT}/common/MenuAdapter.cpp
    ${REL_ROOT}/common/FileWatcherAdapter.cpp
    ${REL_ROOT}/dialogs/LargeFileWarningDialog.cpp
    ${REL_ROOT}/dialogs/FindInFilesDialog.cpp
    ${REL_ROOT}/dialogs/FindInFilesWorker.cpp
    ${REL_ROOT}/dialogs/AutoCompletion.cpp
    ${REL_ROOT}/dialogs/FunctionCallTip.cpp
    ${REL_ROOT}/editor/SyntaxHighlighter.cpp
    ${REL_ROOT}/editor/SyntaxHighlighterImpl.cpp
)
target_link_libraries(test_core PUBLIC test_stubs Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network ${QSCINTILLA_LIBRARY})
target_include_directories(test_core PUBLIC ${TEST_INC} ${REL_ROOT}/workers ${QSCINTILLA_INCLUDE_DIR})
target_compile_options(test_core PRIVATE -w)

add_library(test_core_interface INTERFACE)
target_include_directories(test_core_interface INTERFACE ${TEST_INC} ${REL_ROOT}/workers)

macro(add_core_test TARGET)
    add_executable(${TARGET} ${ARGN})
    target_include_directories(${TARGET} PRIVATE ${TEST_INC} ${REL_ROOT}/workers ${QSCINTILLA_INCLUDE_DIR})
    target_link_libraries(${TARGET} PRIVATE test_core test_stubs Qt6::Core Qt6::Widgets Qt6::Gui)
endmacro()

# ── Test executables ───────────────────────────────────────────────────────────

add_executable(test_encoding_detector test_encoding_detector.cpp)
target_link_libraries(test_encoding_detector PRIVATE test_core)
add_test(NAME EncodingDetectorTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_encoding_detector>)

add_executable(test_string_helper test_string_helper.cpp)
target_link_libraries(test_string_helper PRIVATE test_core)
add_test(NAME StringHelperTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_string_helper>)

add_core_test(test_buffer test_buffer.cpp)
add_test(NAME BufferTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_buffer>)

add_executable(test_menus test_menus.cpp ${REL_ROOT}/common/MenuAdapter.cpp)
target_include_directories(test_menus PRIVATE ${TEST_INC} ${REL_ROOT}/workers)
target_link_libraries(test_menus PRIVATE Qt6::Widgets Qt6::Core)
add_test(NAME MenuTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_menus>)
set_tests_properties(MenuTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(test_shortcuts
    test_shortcuts.cpp
    ${REL_ROOT}/common/ShortcutAdapter.cpp
    ${REL_ROOT}/common/ShortcutAdapter.h
)
target_compile_definitions(test_shortcuts PRIVATE NPPQT_HAVE_SHORTCUT_MANAGER)
target_link_libraries(test_shortcuts PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Test)
add_test(NAME ShortcutAdapterTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_shortcuts>)
set_tests_properties(ShortcutAdapterTests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}")

add_executable(test_file_watcher
    test_file_watcher.cpp
    ${REL_ROOT}/common/FileWatcherAdapter.cpp
    ${REL_ROOT}/common/FileWatcherAdapter.h
)
target_link_libraries(test_file_watcher PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Test)
add_test(NAME FileWatcherAdapterTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_file_watcher>)

add_core_test(test_clipboard test_clipboard.cpp)
add_test(NAME ClipboardAdapterTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_clipboard>)

# 8 new test executables
add_core_test(test_sessions test_sessions.cpp)
add_test(NAME SessionTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_sessions>)
set_tests_properties(SessionTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_macros test_macros.cpp)
add_test(NAME MacroTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_macros>)
set_tests_properties(MacroTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_findinfiles test_findinfiles.cpp)
add_test(NAME FindInFilesTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_findinfiles>)
set_tests_properties(FindInFilesTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_clipboard_manager test_clipboard_manager.cpp)
add_test(NAME NamedClipboardTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_clipboard_manager>)
set_tests_properties(NamedClipboardTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(test_snippets test_snippets.cpp)
target_include_directories(test_snippets PRIVATE ${TEST_INC} ${REL_ROOT}/workers)
target_link_libraries(test_snippets PRIVATE Qt6::Core Qt6::Widgets)
add_test(NAME SnippetTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_snippets>)
set_tests_properties(SnippetTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_project_panels test_project_panels.cpp)
add_test(NAME ProjectPanelTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_project_panels>)
set_tests_properties(ProjectPanelTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_document_map test_document_map.cpp)
add_test(NAME DocumentMapTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_document_map>)
set_tests_properties(DocumentMapTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_close_all_but test_close_all_but.cpp)
add_test(NAME CloseAllButTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_close_all_but>)
set_tests_properties(CloseAllButTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_syntax_highlighter test_syntax_highlighter.cpp)
add_test(NAME SyntaxHighlighterTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_syntax_highlighter>)
set_tests_properties(SyntaxHighlighterTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_util test_util.cpp)
add_test(NAME UtilTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_util>)
set_tests_properties(UtilTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_autocompletion test_autocompletion.cpp)
add_test(NAME AutoCompletionTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_autocompletion>)
set_tests_properties(AutoCompletionTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_core_test(test_functioncalltip test_functioncalltip.cpp)
add_test(NAME FunctionCallTipTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_functioncalltip>)
set_tests_properties(FunctionCallTipTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(test_file_loader_worker test_file_loader_worker.cpp)
target_link_libraries(test_file_loader_worker PRIVATE test_core test_stubs Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network Qt6::Test)
add_test(NAME FileLoaderWorkerTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_file_loader_worker>)
add_executable(test_udl_keywords test_udl_keywords.cpp)
target_link_libraries(test_udl_keywords PRIVATE test_core Qt6::Core)
add_test(NAME UdlKeywordTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_udl_keywords>)
set_tests_properties(UdlKeywordTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# IncrementalSearchDialog requires ScintillaEditor — too heavy for test_core; requires
# integration test setup. Mark as pending until proper fixture exists.
# add_core_test(test_incremental_search test_incremental_search.cpp)
# add_test(NAME IncrementalSearchTests COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_incremental_search>)

