# src/tests/CMakeLists.txt - Unit tests for Notepad--Qt

enable_testing()

# Paths relative to this file (src/tests/)
set(REL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(TEST_INC
    ${REL_ROOT}
    ${REL_ROOT}/common
    ${REL_ROOT}/core
    ${REL_ROOT}/editor
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
    ${REL_ROOT}/dialogs/LargeFileWarningDialog.cpp
)
target_include_directories(test_core PUBLIC ${TEST_INC})
target_link_libraries(test_core PUBLIC
    Qt6::Core
    Qt6::Widgets
    ${QSCINTILLA_LIBRARY}
)

# Ensure QScintilla headers are visible to test_core
target_include_directories(test_core PUBLIC
    ${QSCINTILLA_INCLUDE_DIR}
)
target_compile_options(test_core PRIVATE -w)

add_library(test_core_interface INTERFACE)
target_include_directories(test_core_interface INTERFACE ${TEST_INC})

# EncodingDetector tests
add_executable(test_encoding_detector ${CMAKE_CURRENT_SOURCE_DIR}/test_encoding_detector.cpp)
target_link_libraries(test_encoding_detector PRIVATE test_core)
add_test(NAME EncodingDetectorTests COMMAND test_encoding_detector)

# StringHelper tests
add_executable(test_string_helper ${CMAKE_CURRENT_SOURCE_DIR}/test_string_helper.cpp)
target_link_libraries(test_string_helper PRIVATE test_core)
add_test(NAME StringHelperTests COMMAND test_string_helper)

# Buffer/FileManager tests
add_executable(test_buffer ${CMAKE_CURRENT_SOURCE_DIR}/test_buffer.cpp)
target_link_libraries(test_buffer PRIVATE test_core)
add_test(NAME BufferTests COMMAND test_buffer)

# Util tests
add_executable(test_util ${CMAKE_CURRENT_SOURCE_DIR}/test_util.cpp)
target_link_libraries(test_util PRIVATE test_core)
add_test(NAME UtilTests COMMAND test_util)
