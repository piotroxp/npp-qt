# HEARTBEAT.md — npp-qt (Notepad++ → Qt6/Linux)

**Project:** npp-qt — Notepad++ 8.9.6 → Qt6/Linux  
**Binary:** `build/NotepadMinusMinusQt` (42 MB Qt6/QScintilla2)  
**Build:** `cmake --build build` (Qt6 on gateway, 0 errors, 100%)  
**Tests:** `QT_QPA_PLATFORM=offscreen ctest --output-on-failure` → 7/7 suites pass  

---

## Current Status

**Build:** ✅ Clean — 0 errors, 100% (22+ targets via cmake --build build)  
**Tests:** ✅ 7/7 unit suites pass (`QT_QPA_PLATFORM=offscreen ctest --output-on-failure`)  
**Binary (--help):** ✅ Exits with code 0  
**Binary (headless launch):** ✅ Exits cleanly after 5-second auto-exit (no display → offscreen mode → `std::_Exit(0)`)  
**Binary (with xvfb):** ✅ Runs normally (window visible, no crash)  
**Binary (file argument):** ✅ App starts and stays open with file argument

Headless detection uses `DISPLAY`/`WAYLAND_DISPLAY` env vars (replaced `QOffscreenSurface::create()` which SEGV'd). Plugin warnings ("This plugin does not support propagateSizeHints()") are non-fatal. `NppBigSwitch` now has 70+ commands registered with canonical menuCmdID.h IDs. `NppCommands` has canonical IDs and 80+ `on*()` handlers wired.

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

### 1. Plugin warnings in headless mode
Non-fatal Qt plugin warnings ("This plugin does not support propagateSizeHints()") appear
during startup. These are cosmetic — the app continues and exits cleanly.

### 2. Missing/broken modules (priority order)
- `Notepad_plus.cpp`: Source 9331 lines → Target ~52 lines (99% loss). Main app logic stubbed.
- `ScintillaEditView.cpp`: Source 4955 lines → ~30KB (15% of source). Core editor view implementation.
- `NppIO.cpp`: Source 3200+ lines → partial. File I/O subsystem.
- `PreferenceDialog`: Source 58KB → Missing. Preferences window needs full UI.
- `FunctionListPanel`: Source 30KB → Partial.
- `PluginsManager`: Source 28KB → Stub only.
- `BabyGrid` (77KB grid control): Missing — replaced with QTableWidget stub.
- `uchardet`/`pugixml`: 413KB → replaced with QTextCodec/QDomDocument (functional but less feature-complete).
- 47 WinControls files: Partial implementations.

---

## Quick Reference

```bash
# Build
cd /home/node/.openclaw/workspace/npp-qt
cmake --build build

# Test
cd build && ctest --output-on-failure
# Or (requires QT_QPA_PLATFORM=offscreen for GUI-dependent tests):
cd build && QT_QPA_PLATFORM=offscreen ctest --output-on-failure

# Run headlessly (exits after 5-second auto-exit)
LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu ./build/NotepadMinusMinusQt

# Run with xvfb
xvfb-run -a ./build/NotepadMinusMinusQt

# Gate report
python3 skills/codebase-translate/scripts/translation_harness.py \
  --mode gate --project npp-qt --source-lang cpp --target-lang qt6 \
  --target-dir . --heartbeat-file ~/.openclaw/workspace/HEARTBEAT.md \
  --target-cmd build/NotepadMinusMinusQt
```
