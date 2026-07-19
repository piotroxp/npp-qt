# CMake generated Testfile for 
# Source directory: /workspace/.openclaw/workspace/npp-qt/src/tests
# Build directory: /workspace/.openclaw/workspace/npp-qt/build_test/src/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[EncodingDetectorTests]=] "/workspace/.openclaw/workspace/npp-qt/build_test/src/tests/test_encoding_detector")
set_tests_properties([=[EncodingDetectorTests]=] PROPERTIES  _BACKTRACE_TRIPLES "/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;51;add_test;/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;0;")
add_test([=[StringHelperTests]=] "/workspace/.openclaw/workspace/npp-qt/build_test/src/tests/test_string_helper")
set_tests_properties([=[StringHelperTests]=] PROPERTIES  _BACKTRACE_TRIPLES "/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;56;add_test;/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;0;")
add_test([=[BufferTests]=] "/workspace/.openclaw/workspace/npp-qt/build_test/src/tests/test_buffer")
set_tests_properties([=[BufferTests]=] PROPERTIES  _BACKTRACE_TRIPLES "/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;61;add_test;/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;0;")
add_test([=[MenuTests]=] "/usr/bin/cmake" "-E" "env" "QT_QPA_PLATFORM=offscreen" "/workspace/.openclaw/workspace/npp-qt/build_test/src/tests/test_menus")
set_tests_properties([=[MenuTests]=] PROPERTIES  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" _BACKTRACE_TRIPLES "/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;91;add_test;/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;0;")
add_test([=[ShortcutAdapterTests]=] "/workspace/.openclaw/workspace/npp-qt/build_test/src/tests/test_shortcuts")
set_tests_properties([=[ShortcutAdapterTests]=] PROPERTIES  ENVIRONMENT "QT_QPA_PLATFORM=offscreen;LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:" _BACKTRACE_TRIPLES "/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;104;add_test;/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;0;")
add_test([=[ClipboardAdapterTests]=] "/workspace/.openclaw/workspace/npp-qt/build_test/src/tests/test_clipboard")
set_tests_properties([=[ClipboardAdapterTests]=] PROPERTIES  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" _BACKTRACE_TRIPLES "/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;112;add_test;/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;0;")
add_test([=[FileWatcherAdapterTests]=] "/workspace/.openclaw/workspace/npp-qt/build_test/src/tests/test_file_watcher")
set_tests_properties([=[FileWatcherAdapterTests]=] PROPERTIES  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" _BACKTRACE_TRIPLES "/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;121;add_test;/workspace/.openclaw/workspace/npp-qt/src/tests/CMakeLists.txt;0;")
