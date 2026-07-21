# tests/

This directory previously held an orphaned test target suite (TestLexer.cpp,
TestDialogs.cpp, integration_test.cpp, etc.) using CamelCase naming with its
own `CMakeLists.txt`. **It was never wired into the active build** — the
top-level `CMakeLists.txt` only does `add_subdirectory(src/tests)`.

All active tests live in `src/tests/test_*.cpp` (snake_case) under the
shared test framework (`TEST()` / `RUN()` / `EXPECT()` macros in
`test_common.h`). See `src/tests/CMakeLists.txt` for the registered test
binaries. They run via `ctest` from the build directory.

The legacy files are kept here as `.deprecated` for reference, but **will
not build** because:
1. `tests/CMakeLists.txt` is no longer included by the parent project.
2. They depend on a `test_stubs` API (`app().options().X`) that doesn't
   match the real `AppOptions` API in `src/core/Application.h`.
3. Some of them link against a `npp-qt-test-lib` / `npp-qt-test-lib-gui`
   that the active build no longer produces.

If you need to revive a test from here:
1. Read the file and extract the test cases you want.
2. Port the assertions to the `src/tests/test_*.cpp` style (snake_case,
   `TEST(...)` / `RUN(...)` / `EXPECT(...)` macros).
3. Register it in `src/tests/CMakeLists.txt` with `add_core_test(...)`.
4. Delete the `.deprecated` file.

## File mapping (legacy → active)

| Legacy file                        | Status                                                       |
|------------------------------------|--------------------------------------------------------------|
| TestLexer.cpp                      | Coverage ported to src/tests/test_lexer_regression.cpp       |
| TestEncodingDetector.cpp           | Not yet ported (npp-qt-test-pure target used npp-test-encoding) |
| TestStringHelper.cpp               | Not yet ported                                               |
| TestFileHelper.cpp                 | Not yet ported                                               |
| TestStatusBar.cpp                  | Not yet ported                                               |
| TestDialogs.cpp                    | Not yet ported                                               |
| integration_test.cpp               | Not yet ported                                               |
| integration_editor_test.cpp        | Not yet ported                                               |
| startup_test.cpp                   | Not yet ported                                               |
| stubs/TestStubs.{cpp,h}            | Stubs moved to src/tests/stubs/TestStubs.h                   |