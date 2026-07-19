# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/NotepadMinusMinusQt_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/NotepadMinusMinusQt_autogen.dir/ParseCache.txt"
  "NotepadMinusMinusQt_autogen"
  "src/tests/CMakeFiles/test_buffer_autogen.dir/AutogenUsed.txt"
  "src/tests/CMakeFiles/test_buffer_autogen.dir/ParseCache.txt"
  "src/tests/CMakeFiles/test_clipboard_autogen.dir/AutogenUsed.txt"
  "src/tests/CMakeFiles/test_clipboard_autogen.dir/ParseCache.txt"
  "src/tests/CMakeFiles/test_core_autogen.dir/AutogenUsed.txt"
  "src/tests/CMakeFiles/test_core_autogen.dir/ParseCache.txt"
  "src/tests/CMakeFiles/test_encoding_detector_autogen.dir/AutogenUsed.txt"
  "src/tests/CMakeFiles/test_encoding_detector_autogen.dir/ParseCache.txt"
  "src/tests/CMakeFiles/test_file_watcher_autogen.dir/AutogenUsed.txt"
  "src/tests/CMakeFiles/test_file_watcher_autogen.dir/ParseCache.txt"
  "src/tests/CMakeFiles/test_menus_autogen.dir/AutogenUsed.txt"
  "src/tests/CMakeFiles/test_menus_autogen.dir/ParseCache.txt"
  "src/tests/CMakeFiles/test_shortcuts_autogen.dir/AutogenUsed.txt"
  "src/tests/CMakeFiles/test_shortcuts_autogen.dir/ParseCache.txt"
  "src/tests/CMakeFiles/test_string_helper_autogen.dir/AutogenUsed.txt"
  "src/tests/CMakeFiles/test_string_helper_autogen.dir/ParseCache.txt"
  "src/tests/test_buffer_autogen"
  "src/tests/test_clipboard_autogen"
  "src/tests/test_core_autogen"
  "src/tests/test_encoding_detector_autogen"
  "src/tests/test_file_watcher_autogen"
  "src/tests/test_menus_autogen"
  "src/tests/test_shortcuts_autogen"
  "src/tests/test_string_helper_autogen"
  )
endif()
