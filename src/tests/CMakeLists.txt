# src/tests/CMakeLists.txt - Unit tests for Notepad--Qt

enable_testing()

set(TEST_INC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/editor
)

# Test library — includes all core sources that need Buffer
add_library(test_core STATIC
    ${CMAKE_SOURCE_DIR}/src/core/Buffer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EncodingDetector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LanguageManager.cpp
    ${CMAKE_SOURCE_DIR}/src/common/Util.cpp
    ${CMAKE_SOURCE_DIR}/src/common/StringHelper.cpp
    ${CMAKE_SOURCE_DIR}/src/common/FileHelper.cpp
    ${CMAKE_SOURCE_DIR}/src/dialogs/LargeFileWarningDialog.cpp
)
target_include_directories(test_core PUBLIC ${TEST_INC})
target_link_libraries(test_core PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    ${QSCINTILLA_LIBRARIES}
)
target_include_directories(test_core SYSTEM PUBLIC ${QSCINTILLA_INCLUDE_DIRS})
target_compile_options(test_core PRIVATE -w)

add_library(test_core_interface INTERFACE)
target_include_directories(test_core_interface INTERFACE ${TEST_INC})

# EncodingDetector tests
add_executable(test_encoding_detector test_encoding_detector.cpp)
target_link_libraries(test_encoding_detector PRIVATE test_core)
add_test(NAME EncodingDetectorTests COMMAND test_encoding_detector)

# StringHelper tests
add_executable(test_string_helper test_string_helper.cpp)
target_link_libraries(test_string_helper PRIVATE test_core)
add_test(NAME StringHelperTests COMMAND test_string_helper)

# Buffer/FileManager tests
add_executable(test_buffer test_buffer.cpp)
target_link_libraries(test_buffer PRIVATE test_core)
add_test(NAME BufferTests COMMAND test_buffer)

# Util tests
add_executable(test_util test_util.cpp)
target_link_libraries(test_util PRIVATE test_core)
add_test(NAME UtilTests COMMAND test_util)
