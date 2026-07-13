# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "RelWithDebInfo")
  file(REMOVE_RECURSE
  "CMakeFiles/npp-qt_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/npp-qt_autogen.dir/ParseCache.txt"
  "npp-qt_autogen"
  "tests/CMakeFiles/npp-qt-test-lib-gui_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-qt-test-lib-gui_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/npp-qt-test-lib_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-qt-test-lib_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/npp-test-colour_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-test-colour_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/npp-test-darkmode_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-test-darkmode_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/npp-test-dirwatch_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-test-dirwatch_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/npp-test-filedlg_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-test-filedlg_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/npp-test-iconset_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-test-iconset_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/npp-test-lesdlgs_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-test-lesdlgs_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/npp-test-utf8_16_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/npp-test-utf8_16_autogen.dir/ParseCache.txt"
  "tests/npp-qt-test-lib-gui_autogen"
  "tests/npp-qt-test-lib_autogen"
  "tests/npp-test-colour_autogen"
  "tests/npp-test-darkmode_autogen"
  "tests/npp-test-dirwatch_autogen"
  "tests/npp-test-filedlg_autogen"
  "tests/npp-test-iconset_autogen"
  "tests/npp-test-lesdlgs_autogen"
  "tests/npp-test-utf8_16_autogen"
  )
endif()
