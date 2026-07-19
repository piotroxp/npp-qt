# HEARTBEAT.md — npp-qt (Notepad++ → Qt6/Linux)

**Project:** npp-qt — Notepad++ 8.9.6 → Qt6/Linux  
**Binary:** `build/NotepadMinusMinusQt` (Qt6/QScintilla2)  
**Build:** `cmake --build build` (Qt6 on gateway, 0 errors)  
**Tests:** `QT_QPA_PLATFORM=offscreen ctest --output-on-failure` → 7/7 suites pass  
**Branch:** `master` — canonical source of truth

---

## Current Status

**Build:** ✅ Clean — 0 errors (`make -j$(nproc)`)  
**Tests:** ✅ 7/7 unit suites pass  
**Binary (headless):** ✅ Exits cleanly (offscreen mode → `std::_Exit(0)`)  
**Binary (with xvfb):** ✅ Runs normally (window visible, no crash)  
**Keyboard shortcuts:** ✅ All 40 registered shortcuts wired to `dispatchCommand()`  
**Menu commands:** ✅ All 42 menu commands route to `dispatchCommand()` via `onMenuCommand()`  
**Toolbar commands:** ✅ All 36 toolbar commands route to `dispatchCommand()` via `onToolBarCommand()`  
**File open:** ✅ Files load and display correctly (`_bufferText` map populated after `loadFile()`)  
**Tab navigation:** ✅ Ctrl+1..8, Ctrl+Tab, Ctrl+Shift+Tab wired  
**Search shortcuts:** ✅ Find, Replace, FindNext, FindPrev, ToggleBookmark (Ctrl+F2), FindInFiles  
**View shortcuts:** ✅ ZoomIn/Out/Reset, FullScreen, AlwaysOnTop, WordWrap, ReadOnly, ShowAllChars  

---

## Command Routing Architecture

```
Menu action (setData("file.open"))
    → MenuBar emits menuCommand("file.open")
        → MainWindow::onMenuCommand(cmd)
            → dispatchCommand("file.open")

Toolbar button clicked
    → ToolBar emits toolBarCommand("file.open")
        → MainWindow::onToolBarCommand(cmd)
            → dispatchCommand("file.open")

Keyboard shortcut (Ctrl+O)
    → ShortcutAdapter registered shortcut fires
        → emits shortcutDispatchRequested("file.open")
            → MainWindow::onShortcutDispatch(cmd)
                → dispatchCommand("file.open")

dispatchCommand("file.open")
    → MainWindow routes to _fileManager.openFile()
        → Buffer created, file loaded, ScintillaEditor updated
```

**72 commands wired in `dispatchCommand()`.** All three input paths converge to one dispatcher.

---

## Known Unwired Functionality

The following are registered/triggered but lack functional implementations:

| Command | Source | Status | Notes |
|---------|--------|--------|-------|
| `file.print` | ShortcutAdapter | ⚠️ No-op | Registered shortcut, no print implementation |
| `edit.blockComment` | ToolBar | ⚠️ Stub | `// TODO: implement block comment` |
| `edit.toggleComment` | ToolBar | ⚠️ Stub | `// TODO: implement toggle comment` |
| `edit.indent` | ToolBar | ⚠️ Stub | `// TODO: implement indent` |
| `edit.outdent` | ToolBar | ⚠️ Stub | `// TODO: implement outdent` |
| `search.findInFiles` | ShortcutAdapter/Menu | ⚠️ Stub | `onFindInFiles()` exists but returns early |
| `search.incrementalSearch` | Menu | ⚠️ Stub | Dialog exists but no active wiring |
| `search.markAll` | Menu | ⚠️ Stub | No implementation |
| `search.count` | Menu | ⚠️ Stub | No implementation |
| `view.funcList` | ToolBar | ⚠️ Stub | FunctionListPanel exists but not shown |
| `view.documentMap` | Menu/ToolBar | ⚠️ Partial | DocumentMapPanel exists, toggle wired |
| `view.fileBrowser` | Menu/ToolBar | ⚠️ Partial | FileBrowserPanel exists, toggle wired |
| `settings.shortcutMapper` | Menu | ⚠️ Stub | ShortcutMapperDialog exists, no open call |
| `settings.toolbarCustomize` | ToolBar | ⚠️ Stub | No implementation |
| `macro.record/stop/play` | ToolBar | ⚠️ Stub | MacroManager exists but not wired |
| `tools.run` | ToolBar | ⚠️ Stub | No implementation |
| `language.menu` | ToolBar | ⚠️ Stub | No language-switch implementation |
| `encoding.menu` | ToolBar | ⚠️ Stub | No encoding-change implementation |
| `eol.menu` | ToolBar | ⚠️ Stub | No EOL-change implementation |

---

## Architecture

### Core (src/core/)
- `Application` — app lifecycle, singleton, menu/toolbar/signal routing
- `Buffer` — file buffer, encoding, EOL, dirty flag
- `FileManager` — open/save/reload, encoding detection, backup
- `EncodingDetector` — charset auto-detection
- `LanguageManager` — lexer selection, syntax highlighting
- `Parameters` / `NppParameters` — config persistence
- `SessionManager` — session save/restore
- `MacroManager` — macro record/playback (stub)
- `NppBigSwitch` — 70+ command IDs registered
- `NppCommands` — canonical ID dispatch to handlers

### Editor (src/editor/)
- `ScintillaEditor` — QsciScintilla wrapper, 400+ methods, zoom, bookmarks, find/replace, multi-caret, edge indicators
- `ScintillaEditView` — multi-view management, split view, DocTabView
- `SyntaxHighlighter` — Qt syntax highlighting on top of Scintilla
- `LexerManager` — 54 language definitions

### UI (src/ui/)
- `MainWindow` — main window, tab management, dispatchCommand(), 72 commands wired
- `MenuBar` — full N++ menu with 42 commands
- `ToolBar` — toolbar with 36 commands
- `StatusBar` — cursor position, encoding, EOL, line/column display
- `TabBar` — custom tab bar with context menu

### Dialogs (src/dialogs/)
- `FindReplaceDialog` — Find, Replace, FindInFiles, Mark, GoTo
- `PreferenceDialog` — full settings UI
- `ShortcutMapperDialog` — keyboard shortcut customization
- `GoToLineDialog` — line/offset navigation
- `IncrementalSearchDialog` — incremental search
- `AutoCompletion` — word/completion popup (600+ lines)
- `FunctionCallTip` — function parameter tips
- `AnsiCharPanel` / `FindCharsInRangeDialog` / `ColumnEditorDialog`
- `AboutDialog` / `RunDialog` / `MacroDialog` / `PluginManagerDialog`

### Panels (src/panels/)
- `FileBrowserPanel` — file tree with keyboard nav
- `FunctionListPanel` — parsed function list (loads N++ functionList XML)
- `DocumentMapPanel` — minimap with scroll sync
- `ClipboardHistoryPanel` — clipboard history

### Common (src/common/)
- `ShortcutAdapter` — Qt6 QShortcut registry, 40 standard shortcuts
- `MenuAdapter` — menu integration
- `FileWatcherAdapter` — QFileSystemWatcher wrapper
- `DpiManager` — DPI-aware UI scaling
- `ScintillaComponent` — base class for Scintilla access
- `NppSciCompat.h` — Scintilla `SCI_*` compatibility constants

### Workers (src/workers/)
- `FileLoaderWorker` — async file loading

---

## Quick Reference

```bash
# Canonical repo: ~/.openclaw/workspace/npp-qt
# Target branch: master
# Source (translation): ~/.openclaw/workspace/notepad-plus-plus-translation
cd ~/.openclaw/workspace/npp-qt

# Build
cmake --build build

# Test
ctest --output-on-failure

# Run headlessly (auto-exit, no display needed)
./build/NotepadMinusMinusQt

# Run with display
xvfb-run -a ./build/NotepadMinusMinusQt
```
