// Force AUTOMOC to pick up WindowBase (Q_OBJECT) for all test targets
#include "WinControls/Window.h"

// =============================================================================
// TestMain.cpp — Test runner entry point for the npp-qt test suite
// All individual test classes are registered via their respective
// #include "Test<Module>.moc" lines in each .cpp file.
// Run with: ./npp-test-<name>  or  ctest in the build directory
// =============================================================================

// NOTE: This file intentionally has no test code — each test executable
// is self-contained via QTEST_MAIN() in its .cpp file.
// See CMakeLists.txt for the test target definitions.
//
// Quick reference for running individual tests:
//   cd build && make npp-test-utf8_16 && ./npp-test-utf8_16
//
// Run all tests via CTest:
//   cd build && ctest --output-on-failure
//
// Verbose output:
//   ./npp-test-<name> -v2
//
// Specific test function:
//   ./npp-test-<name> -v2 -f test_functionName

// To add a new test, create tests/Test<Module>.cpp with QTEST_MAIN()
// and add a corresponding add_npp_test() call in tests/CMakeLists.txt.
