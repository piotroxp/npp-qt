# Contributing to Notepad--Qt

Thanks for your interest. Notepad--Qt is a Qt6 port of Notepad++ with a
plugin API, a SnippetsManager, macro recording/playback, and full
syntax highlighting for 50+ languages.

## Workflow

1. **Discuss first.** Open an issue or send a message describing what you
   want to change. Semantic-lift changes and stub-elimination work have
   specific conventions — see `WAVES.md` for the current wave plan and
   `docs/AGENTS.md` for the agent-driven workflow.
2. **Branch off `master`.** `master` is the only long-lived branch.
   Keep your commits focused: one logical change per commit.
3. **Commit messages.** Prefix with `npp-qt: ` (matching existing
   history). The body should explain *what* changed and *why*, not
   *how*. For lifts, name the stub you removed and the real
   implementation that replaced it.
4. **Push.** This repo's `origin` is `https://github.com/piotroxp/npp-qt.git`.
   Push from a machine with your GitHub credentials attached.
5. **Test before pushing.** Run `bash scripts/qa_smoke.sh` from your
   working tree — it builds, runs `ctest`, and runs a smoke launch.
   All tests must pass except the two known-flaky harness tests
   (`test_buffer` segfault, `test_util` hang — both are static-dtor
   ordering bugs in the test harness, not in production code).

## Build

```bash
# One-shot
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

Dependencies (Ubuntu 24.04):

```bash
sudo apt-get install -y \
    qt6-base-dev qt6-scintilla2-dev \
    qt6-webengine-dev \
    cmake build-essential ninja-build
```

The CMake build falls back to manual QScintilla discovery if
`QScintilla6_FOUND` is false (the case on Arch and Ubuntu LTS before
Qt6 shipped QScintilla CMake configs). See `CMakeLists.txt` lines
24–39 for the fallback.

## Tests

The test suite is split into two layers:

* **`test_stubs`** (shared lib) — MOC symbols and `Application` slot
  stubs so unit tests can link against `Application.h` without dragging
  in the full Qt event loop.
* **`test_core`** (shared lib) — the production source files needed
  by tests, plus Qt6 + QScintilla links. Tests link `test_core +
  test_stubs`.
* **`add_core_test(<name> <file>)`** — convenience macro that builds
  a single test executable with the right link line and registers it
  with CTest under offscreen QPA.

When you add a new component:

1. If it depends on `Application`, add it to `test_stubs`
   (`src/tests/CMakeLists.txt` line ~42).
2. If it's pure logic, add the `.cpp` to the `test_core` sources
   block (line ~62) so all tests can use it.
3. Add a test file using `add_core_test(<name> <file>.cpp)` and
   `add_test(NAME <Name>Tests COMMAND ${CMAKE_COMMAND} -E env
   QT_QPA_PLATFORM=offscreen $<TARGET_FILE:test_<name>>)`.

## Stub-elimination pass

The "semantic lift" is a sweep that replaces Win32-shaped or
stub-shaped code with real Qt6 production behaviour. Conventions:

* **No `Q_UNUSED(arg)` + `// TODO` body** — that's a stub. Either wire
  the arg up or remove the function.
* **No message-box placeholders** that say "not yet implemented" —
  either point at the existing workaround (Shortcut Mapper for
  toolbar customize) or implement the real dialog.
* **No `return false; // TODO`** in command handlers — fail loudly or
  succeed loudly, not silently.
* **Comments must not lie.** If a function calls the real
  implementation on every invocation, the comment must not call it a
  "stub".

## Plugin authoring

Plugins are dynamic libraries (`.so` on Linux, `.dylib` on macOS) that
export an `NppQtPlugin` struct via the `NPPQT_PLUGIN(name, init, cleanup)`
macro from `src/plugins/PluginInterface.h`. The host loads them from
`~/.config/notepad--qt/plugins/` at startup.

Minimum example:

```cpp
// plugin.cpp
#include <cstring>
#include <plugin/PluginInterface.h>

static void my_init(NppQtFuncs* funcs) {
    // funcs->insertText("hello from my plugin\n");
}

static void my_cleanup() { }

NPPQT_PLUGIN("My Plugin", my_init, my_cleanup)
```

CMake snippet:

```cmake
add_library(my_plugin MODULE plugin.cpp)
target_link_libraries(my_plugin PRIVATE Qt6::Core)
set_target_properties(my_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
```

The host's `PluginManager` validates the `apiVersion` field on load;
mismatches are reported to `pluginError` and the plugin is rejected.

## Coding style

* C++20.
* Tabs for indentation in `.cpp`/`.h` (matches existing source).
* `clang-format` not enforced; match the surrounding style.
* No magic numbers — name them as `constexpr` at file scope.
* `qDebug() <<` for runtime diagnostics; `qFatal()` only at the entry
  point of a test.
* Qt parent ownership for `QObject`s. No raw `new`/`delete` outside
  factories.

## Reporting issues

* Build errors: include the full `cmake` and `cmake --build` output,
  plus `cmake --version` and `qmake -v`.
* Runtime crashes: include a stack trace (run under `gdb` with
  `scripts/launch-npp-qt-gdb.sh`) and the input that triggered it.
* Test failures: include `ctest --output-on-failure` output and the
  build log if relevant.

## Release process

1. Cut a release branch from `master`.
2. Bump `project(NotepadMinusMinusQt VERSION ...)` in `CMakeLists.txt`.
3. Update `CHANGELOG.md`: move `[Unreleased]` entries to the new
   version section, set the date.
4. Tag with `git tag -a vX.Y.Z -m "..."`.
5. Push the tag: `git push origin vX.Y.Z`.
6. GitHub Actions (`.github/workflows/ci.yml`) builds and uploads
   the binary as a workflow artifact. Promote to a release manually.
