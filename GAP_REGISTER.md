# GAP_REGISTER — npp-qt Power-On

## Build Status
**Status: CLOSED** — `cmake --build build -j8` succeeds with 0 errors.

## Gap Register

### GAP-001: Duplicate root-level header files causing multiple definition errors
- **Category**: Build
- **Source Location**: Root `/workspace/piotro/npp-qt/*.h` (multiple files)
- **Target Location**: `/workspace/piotro/npp-qt/include/` and `/workspace/piotro/npp-qt/src/`
- **Severity**: Critical
- **Blocker**: Yes
- **Status**: CLOSED
- **Evidence**: `Parameters.h`, `Notepad_plus.h`, `Buffer.h`, `NppXml.h` etc. existed in both root and include/ directories. `file(GLOB_RECURSE NPP_HEADERS ...)` picked up both, causing "multiple definition of `enum class NppEolType`", "redefinition of `class NppParameters`", etc.
- **Fix Summary**: Removed all duplicate root-level header files that were identical to files in include/ or src/. Removed duplicate root-level source files (Notepad_plus.cpp, Buffer.h, etc.) from root.
- **Validation Result**: Build completes successfully with 0 errors.

### GAP-002: CMake AUTOMOC missing autogen include path
- **Category**: Build
- **Source Location**: `CMakeLists.txt`
- **Target Location**: `npp-qt_autogen` directory
- **Severity**: Critical
- **Blocker**: Yes
- **Status**: CLOSED
- **Evidence**: MOC-generated headers (e.g., `SI4PISD35T/moc_fileBrowser.cpp`) not found because autogen directory not in compiler include path.
- **Fix Summary**: Added `target_include_directories(npp-qt PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/npp-qt_autogen>)` to CMakeLists.txt after `add_executable()`.
- **Validation Result**: MOC files are now found and build completes.

### GAP-003: Root-level source files compiled alongside src/ files
- **Category**: Build
- **Source Location**: Root `/workspace/piotro/npp-qt/*.cpp`
- **Target Location**: `/workspace/piotro/npp-qt/src/*.cpp`
- **Severity**: Critical
- **Blocker**: Yes
- **Status**: CLOSED
- **Evidence**: Root-level Notepad_plus.cpp, Buffer.h, ScintillaEditView.cpp, etc. would have been picked up by `file(GLOB_RECURSE NPP_CORE_SOURCES src/*.cpp`...`. Wait, actually `NPP_CORE_SOURCES` only glob-recurse from `src/*.cpp` so root-level .cpp files are NOT compiled. But root-level .h files ARE in NPP_HEADERS glob.
- **Fix Summary**: Removed duplicate root-level .h files (but root-level .cpp files were already not compiled by CMake).
- **Validation Result**: N/A (root .cpp not compiled, issue was root .h duplicates).

## Remaining Non-Blocking Gaps

### GAP-010: Notepad_plus.cpp has many missing/incompatible API calls
- **Category**: EditorCore / Commands
- **Severity**: High
- **Status**: Open
- **Evidence**: Multiple missing methods on NppParameters, wrong API signatures for dialog init(), missing TabBarPlus::doVertical(), missing ScintillaAccelerator methods, etc.
- **Proposed Lift Strategy**: Continue fixing Notepad_plus.cpp by adding shim/wrapper methods and fixing API mismatches. See AGENT_PROGRESS.md for details.
- **Validation Method**: Build + runtime smoke test.

### GAP-011: Missing QTranslator return value checks
- **Category**: Runtime
- **Severity**: Low (warnings only)
- **Status**: Open
- **Evidence**: `QTranslator::load()` return values ignored in main.cpp (nodiscard attribute)
- **Proposed Lift Strategy**: Add `(void)` casts or check return values.
- **Validation Method**: Build warning count reduction.

### GAP-012: Deprecated Qt6 API usage
- **Category**: Runtime
- **Severity**: Low (warnings only)
- **Status**: Open
- **Evidence**: `Qt::AA_EnableHighDpiScaling` deprecated; `QLibraryInfo::location()` deprecated
- **Proposed Lift Strategy**: Replace with Qt6 equivalents.
- **Validation Method**: Build warning count reduction.
