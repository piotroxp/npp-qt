# HEARTBEAT.md — npp-qt (Notepad++ → Qt6/Linux)

**Project:** npp-qt — Notepad++ 8.9.6 → Qt6/Linux  
**Binary:** `build/NotepadMinusMinusQt` (Qt6/QScintilla2)  
**Build:** `cmake --build build` on build host (192.168.2.128) — clean, 0 errors  
**Tests:** `ctest --output-on-failure` → **19/19 suites pass** (1.22s)  
**Binary (headless):** ✅ Runs cleanly (offscreen mode → 5s auto-exit)  
**Branch:** `master` — canonical source of truth  
**Last Updated:** 2026-07-20

---

## Current Status

| Subsystem | Status | Notes |
|-----------|--------|-------|
| **Application / MainWindow** | ✅ REAL | Full init, menu/toolbar/shortcut routing |
| **FileManager** | ✅ REAL | open/save/close/reload all work |
| **Buffer** | ✅ REAL | Encoding, EOL, language, dirty tracking |
| **SessionManager** | ✅ REAL | JSON save/restore, recent files |
| **EncodingDetector** | ✅ REAL | BOM + UTF-8 + extension hint detection |
| **ScintillaEditor** | ✅ REAL | 400+ methods, full Qsci integration |
| **AutoCompletion** | ✅ REAL | Word/API/function/path completion + guards |
| **Find/Replace** | ✅ REAL | Native Scintilla search + indicators |
| **FindInFiles** | ✅ REAL | Background worker + results panel |
| **All 17 Dialogs** | ✅ REAL | All fully implemented |
| **All 4 Panels** | ✅ REAL | DocMap, FnList, FileBrowser, ClipboardHx |
| **MenuBar / ToolBar** | ✅ REAL | All 42 menu + 36 toolbar commands wired |
| **Keyboard Shortcuts** | ✅ REAL | 40 shortcuts registered and routed |
| **Print / Preview** | ✅ REAL | QPrinter + QPrintPreviewDialog |
| **PreferenceDialog** | ✅ REAL | Full tabbed settings UI |
| **ShortcutMapper** | ✅ REAL | Key grabber + conflict detection |

---

## CRITICAL FIXES APPLIED TODAY (2026-07-20)

### 1. ✅ File open — _bufferText populated (fixed previously)
`FileManager::openFile()` now stores decoded text in `_bufferText[buf->getID()]` so `getBufferText()` works immediately after open.

### 2. ✅ File save — content retrieval from _bufferText map
`FileManager::saveFile(BufferID, path)` now retrieves text from `_bufferText` map instead of saving empty string.
```cpp
QString content;
auto it = _bufferText.find(buffer);
if (it != _bufferText.end()) {
    content = it->second;
}
return saveFile(savePath, content, buf->getEncoding(), buf->getEolFormat());
```

### 3. ✅ Encoding garble bug — SCI_SETCODEPAGE now called
`ScintillaEditor::setEncoding()` now calls `SendScintilla(SCI_SETCODEPAGE, codepage)` for all EncodingType values. Non-UTF-8 files (Windows-1252, ISO-8859-1, etc.) will display correctly.

### 4. ✅ Autocomplete crash guards
- `AutoCompletion::showPathCompletion()` — `_constructionComplete` guard added
- `AutoCompletion::showEnvVarCompletion()` — `_constructionComplete` guard added
- `AutoCompletion::update()` — `_completionActive` guard added (prevents retrigger during active popup)

### 5. ✅ Qt6::Sql removed from CMakeLists.txt
`Qt6::Sql` was linked but never used (zero QSql calls in codebase). Removed to eliminate phantom dependency.

### 6. ✅ SessionManager validation
`applySession()` now validates version and checks file existence before applying.

---

## KNOWN GAPS (Non-Breaking)

| Gap | Severity | Description |
|-----|---------|-------------|
| **uchardet not integrated** | Medium | Statistical single-byte charset detection missing. Extension hints used as fallback. |
| **UDL Scintilla wiring** | Medium | UDL keywords stored but not applied to Scintilla lexer. |
| **FileWatcher reload prompt** | Low | External file change → auto-reload (no user prompt). |
| **captureCurrentSession** | Low | Shallow capture; Application::saveSession() does real work directly. |
| **Async file loading** | Low | `_loader`/`_loaderThread` declared but not wired. |

---

## Architecture

```
Application (singleton)
├── FileManager ─── Buffer[] + _bufferText map + EncodingDetector
├── SessionManager ─── NppSession JSON
├── LanguageManager ─── lexer selection
├── EditorCommandManager ─── 70+ commands
├── ShortcutManager ─── shortcut registry
├── MacroManager ─── record/playback
├── RecentFilesManager ─── QSettings-backed
├── UdlManager ─── UDL XML parser
├── MainWindow ─── QMainWindow
│   ├── MenuBar ─── 42 commands → dispatchCommand()
│   ├── ToolBar ─── 36 buttons → dispatchCommand()
│   ├── DocTabView ─── QTabWidget + TabData map
│   ├── StatusBar ─── position/encoding/EOL/cursor
│   └── ScintillaEditor (active) ─── QsciScintilla
├── FindReplaceDialog ─── Scintilla search
├── FindInFilesDialog + FindInFilesWorker ─── background thread
├── GoToLineDialog
├── IncrementalSearchDialog
├── AutoCompletion ─── word/API/function/path/snippet
├── FunctionCallTip ─── parameter calltips (navigates via Scintilla)
├── ClipboardHistoryPanel ─── dock + paste to editor
├── DocumentMapPanel ─── minimap
├── FunctionListPanel ─── function outline
├── FileBrowserPanel ─── file tree
├── PreferenceDialog
├── ShortcutMapperDialog
├── CommandPaletteDialog
└── AboutDialog
```

---

## Build / Test / Run

```bash
# Build host (192.168.2.128)
cd /home/piotro/.openclaw/workspace/npp-qt/build
cmake -DCMAKE_BUILD_TYPE=Release ..
ninja

# Test
cd build
QT_QPA_PLATFORM=offscreen ctest --output-on-failure

# Run headlessly
QT_QPA_PLATFORM=offscreen ./NotepadMinusMinusQt
# (auto-exits after 5s in headless mode)

# Run with file (headless)
QT_QPA_PLATFORM=offscreen ./NotepadMinusMinusQt /path/to/file.txt

# Run with display
./NotepadMinusMinusQt

# Open recent session
./NotepadMinusMinusQt --session ~/.config/notepad--qt/session.json
```
