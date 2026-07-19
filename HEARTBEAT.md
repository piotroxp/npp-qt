# HEARTBEAT.md — npp-qt (Notepad++ → Qt6/Linux)

**Project:** npp-qt — Notepad++ 8.9.6 → Qt6/Linux  
**Binary:** `build/NotepadMinusMinusQt` (44 MB Qt6/QScintilla2)  
**Build:** `cmake -B build -G Ninja && cmake --build build` (Qt6 on gateway)  
**Tests:** `ctest --output-on-failure` → 8/8 pass  

---

## Current Status

**Build:** ✅ Clean — 0 errors, 123 ninja targets  
**Tests:** ✅ 8/8 unit suites pass  
**Binary (--help):** ✅ Exits cleanly  
**Binary (headless launch):** ❌ Crashes after "No display detected" message  
**Binary (with display):** Not tested (gateway has no display)

The headless crash occurs after plugin initialization, inside Qt plugin loading.
`QOffscreenSurface` crash fixed; remaining crash is elsewhere in plugin/UI initialization.

---

## Architecture

- `src/core/` — Application, Buffer, FileManager, NppIO, Parameters, EncodingDetector, LanguageManager, ShortcutManager, ThemeManager, UdlManager, SessionManager, MacroManager, NppBigSwitch (command dispatcher — 70+ commands registered)
- `src/editor/` — ScintillaEditor, ScintillaEditView, DocTabView, ScintillaCtrls
- `src/ui/` — MainWindow, MenuBar
- `src/dialogs/` — FindReplaceDialog, PreferenceDialog, ShortcutMapperDialog, AnsiCharPanel, FindCharsInRangeDialog, CommandPaletteDialog
- `src/panels/` — FileBrowserPanel, FunctionListPanel, DocumentMapPanel, ClipboardHistoryPanel
- `src/plugins/` — PluginManager (stub)
- `src/common/` — Platform adapters (ClipboardAdapter, FileWatcherAdapter, MenuAdapter, ShortcutAdapter, DpiAdapter)
- `src/workers/` — FileLoaderWorker (async file loading)

---

## Known Issues

### 1. Headless launch crash (priority)
App crashes after "No display detected" message, during plugin initialization.
The "This plugin does not support propagateSizeHints()" warnings precede the crash.
The `QOffscreenSurface` crash is fixed; remaining crash is in plugin loading or UI init.
Fix requires debugging the exact crash point (GDB not available; need valgrind or strace).

### 2. App hangs in app.exec() (no display → offscreen mode)
Signal handlers (`g_sigintReceived`/`g_sigtermReceived`) are wired to a 500ms polling timer
that calls `app.quit()`, but the crash happens before the timer fires.
Once the crash is fixed, the timer mechanism should allow clean headless exit.

### 3. Missing/broken modules
- `Parameters.cpp`: Source 8929 lines → Target 550 lines (94% loss). Settings management incomplete.
- `Notepad_plus.cpp`: Source 9331 lines → Target 52 lines (99% loss). Main app logic mostly stubbed.
- `NppCommands.cpp`: Source 4513 lines → Target 820 lines (82% loss).
- `NppBigSwitch.cpp`: Source 4305 lines → ~350 lines. Dispatcher rewritten with 70+ canonical IDs from menuCmdID.h, all connected to verified Application methods. Forwarding stubs added for naming bridge (e.g. zoomIn→onZoomIn).
- `ScintillaEditView.cpp`: Source 4955 lines → Target 26KB (15% of source).
- `PreferenceDialog`: Source 58KB → Completely missing.
- `PluginsManager`: Source 28KB → Stub only.
- `FunctionListPanel`: Source 30KB → Partial.
- `BabyGrid` (77KB grid control): Missing — replaced with QTableWidget stub.
- `uchardet`/`pugixml`: 413KB XML/encoding libs → replaced with QTextCodec/QDomDocument.
- 47 WinControls files: Partial implementations.

---

## Quick Reference

```bash
# Build
cd /home/node/.openclaw/workspace/npp-qt
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build

# Test
cd build && ctest --output-on-failure

# Run headlessly (currently crashes after plugin init)
LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu ./build/NotepadMinusMinusQt

# Run with xvfb
xvfb-run -a ./build/NotepadMinusMinusQt

# Gate report
python3 skills/codebase-translate/scripts/translation_harness.py \
  --mode gate --project npp-qt --source-lang cpp --target-lang qt6 \
  --target-dir . --heartbeat-file ~/.openclaw/workspace/HEARTBEAT.md \
  --target-cmd build/NotepadMinusMinusQt
```
