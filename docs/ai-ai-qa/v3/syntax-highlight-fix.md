# AI-AI QA Report — 2026-07-21 18:14–18:58 UTC

## Goal
Investigate and fix the "syntax highlighting does not work in npp-qt" report,
then verify all Preferences config options actually persist and apply at
runtime.

## Investigation (commits 26fe60e, 8c327b3)

### Root cause
`QFileInfo::suffix()` returns `"py"`. `LanguageManager`'s extension map uses
keys `".py"`. Lookup misses → `L_TEXT` → `createLexer()` returns nullptr →
no `QsciLexer` attached → plain text for **every** file. Silent failure.

### Fix (LanguageManager::getLanguageForExtension)
Normalize: accept bare or dotted suffix, case-insensitive. One hardening
unbreaks 4 callers (Buffer ctor, FileManager::detectLanguage,
FileManager::loadFile, LexerConfig) — none of the callers needed touching.

### Other bugs found and fixed
- `createLexer()` was missing L_BASH/L_SQL/L_TEX/L_DIFF/L_PHP/L_CSHARP/
  L_CMAKE/L_XAML → all returned nullptr → no highlighting for .sh/.sql/.
  tex/.diff/.php/.cs/.cmake/.xaml files.
- `L_OBJ → QsciLexerBash` was wrong (it colors like shell, not Objective-C).
  Falls back to `QsciLexerCPP` (closest grammar in QScintilla).
- `detectLanguageFromContent` classified `#!/bin/bash` as `L_BATCH`
  instead of `L_BASH`.
- `detectLanguage` did not handle filenames with no extension
  (`/tmp/Makefile`, `CMakeLists.txt`, `Dockerfile`). Added basename
  allowlist.

## Verification

### Functional (on real Wayland desktop via PDC)
- Launched npp-qt --profile dark /tmp/qa-input.py: process alive, all
  QScintilla lexers attached, syntax highlighting visible for Python
  keywords, strings, numbers, comments.
- Launched npp-qt with qa-input.cpp: preprocessor, keywords, numbers,
  std:: types, strings all colored. Document Map shows syntax-colored
  preview.
- Launched npp-qt with qa-input.txt: plain (correct — plain text should
  be plain; proves the fix doesn't false-positive).

### Test suite (ctest, QT_QPA_PLATFORM=offscreen)
- Before: 23/23 passing.
- After: 25/25 passing, 0.48s total.
- New tests added: 80+ assertions across dot-less-suffix detection,
  case-insensitivity, per-LangType lexer creation, full-filename
  detection, IniParser round-trip for every saveConfig field.

### Config audit (manual structural + functional)
- Every AppOptions field is wired through UI ↔ memory ↔ disk.
  Cross-referenced dialog load/save against loadConfig/saveConfig.
  Result: 0 silent-data-loss bugs.
- IniParser round-trip tested for bool/int/string/stringlist across
  every section in the real config.xml (General, Editor, Margins,
  Backup, ShortcutMapper, Search).

## Commits
- 26fe60e fix(core): enable syntax highlighting for all source-file
  extensions (the main fix)
- 8c327b3 feat(syntax): detect Makefile/CMakeLists/Dockerfile by
  basename; add regression & config tests

## Files
- src/core/LanguageManager.cpp — main fix
- src/core/Application.h — loadConfig() public (was private)
- src/tests/test_lexer_regression.cpp — new (20 assertions)
- src/tests/test_config_persistence.cpp — new (60+ assertions)
- src/tests/CMakeLists.txt — registered new tests

## Notes for next session
- `tests/TestLexer.cpp` (CamelCase) is the OLD inactive test file from
  before the test build moved to src/tests/. The active test build uses
  src/tests/test_*.cpp (snake_case). Don't be confused if both exist.
- PDC screenshot tool returns full-desktop PNG (1.3MB) — vision render
  of full-desktop images was unreliable in this session. Crop to <100KB
  JPEG via Pillow first if you need visual confirmation.

## Round 2 (2026-07-21 19:22–21:30 UTC)

Continued from the user's "continue!" prompt after the syntax-highlight
fix landed. Three follow-up tracks.

### 1. Two parallel config systems (deep audit)

`Parameters.cpp` provides `NppParameters` — a full duplicate of config
storage using `QSettings("notepad--qt", "config")` (writes to
`~/.config/notepad--qt/config.ini`), with parallel structs `NppGUI`,
`ScintillaViewParams`, `FindHistory` that overlap with `Application::AppOptions`.

Cross-referenced every read site of `NppParameters::getInstance()`:

  - `Application::exportSettingsToJson` — legacy NPP-compat export only
  - `Application::importSettingsFromJson` — legacy NPP-compat import only
  - `Application::loadShortcuts` / `saveShortcuts` — used during export
  - `NppParameters::addRecentFile` — writes to QSettings directly

**Finding: `NppParameters::load()` is never called. `NppParameters::save()`
is never called.** The only QSettings writes that actually hit disk are
from `addRecentFile`. All the `_nppGUI`, `_scintillaViewParams`,
`_findHistory` fields stay at struct defaults forever.

The Preferences dialog (`src/dialogs/PreferenceDialog.cpp`) has zero
references to `NppParameters::getInstance()` — it talks exclusively to
`Application::_options`. So the duplicate is **dead code for runtime**.

**Impact:** none on user-visible config persistence (the real path
works). But it's ~700 lines of dead code with confusingly parallel
struct layouts. Recommendation: either delete `NppParameters` entirely,
or wire `Application::initialize()` to call `NppParameters::load()`
after `Application::loadConfig()` and route `_nppGUI` reads through
the real `_options`. Out of scope for this round.

### 2. SIGTERM does not save config (real bug, real fix)

`src/main.cpp` had a signal handler that called `std::_Exit(EXIT_SUCCESS)`
after SIGTERM/SIGINT. `std::_Exit` skips destructors, so
`Application::shutdown()` never ran on non-graceful termination, so
`Application::saveConfig()` never wrote `config.xml`. All preferences
were silently lost.

Fix: replaced `std::_Exit` with `QCoreApplication::quit()`. Returns
from `app.exec()` cleanly, runs the post-exec shutdown path that calls
`Application::shutdown()`. Verified on the live binary: SIGTERM now
produces a 618-byte config.xml with all default sections.

Note: a unit test would have been the right coverage here, but
`src/core/Application.cpp` is intentionally not in `test_core.a` (the
test build uses the stub `Application` from TestStubs). Wiring the
real Application into the test build is a much larger surface change.
The functional SSH verification is documented in
docs/ai-ai-qa/v3/syntax-highlight-fix.md.

### 3. Orphaned legacy tests/ directory

`tests/TestLexer.cpp` (CamelCase, 750 lines) + 8 siblings +
`tests/CMakeLists.txt` were never part of the active build (top-level
CMakeLists only does `add_subdirectory(src/tests)`). Anyone running
`ninja` from the build dir would never see them, and the `add_npp_test_gui`
function inside the orphaned CMakeLists used `EXCLUDE_FROM_ALL` so even
inside that subdir they wouldn't build by default.

Renamed all of them to `.deprecated` suffix rather than deleting —
keeps the history intact, makes the "not built" status obvious at a
glance. Added `tests/README.md` mapping legacy → active files.

Coverage from `TestLexer.cpp` was already ported to
`src/tests/test_lexer_regression.cpp` in commit `8c327b3`. Other
files (TestDialogs, TestEncodingDetector, etc.) flagged as "Not yet
ported" — anyone reviving them should follow the recipe in
`tests/README.md`.

### 4. Cosmetic: parallel-agent toolbar objectName changes

`src/editor/WebBrowserView.cpp`, `src/panels/FileBrowserPanel.cpp`,
`src/ui/MainWindow.cpp` had objectName additions from a parallel agent
(visible in `git diff` but not in any commit message). Left untouched —
not part of this work.

### Summary of commits this round

  - `831fdac` chore(tests): rename orphaned tests/ subtree to .deprecated suffix
  - `b5b57fe` fix(shutdown): SIGTERM now triggers Application::shutdown() and saves config.xml

### Test status

  - `ctest`: 25/25 pass, 0.42s
  - `ninja`: clean build, no warnings (except the pre-existing
    unused-argc/argv in `detectHeadlessEnvironment`)
  - Live binary: SIGTERM-clean-shutdown verified via SSH + config.xml
    inspection
