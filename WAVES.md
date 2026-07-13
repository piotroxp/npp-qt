# npp-qt: Wave Plan — Updated 2026-07-13

## Architecture Summary
- **Binary:** `build/NotepadMinusMinusQt` (31MB) ✅ Clean build
- **Source:** `/home/node/.openclaw/workspace/src/`
- **Ref source:** `notepad-plus-plus-translation/` (original Win32)
- **Build:** `cd /home/node/.openclaw/workspace && cmake --build build -j$(nproc)`

## Gate Criteria
Every wave: `cmake --build build -j$(nproc)` must exit 0 before commit.

---

## Wave 5 — Menu Routing + StatusBar Finalization ✅
**Goal:** `Application::onMenuCommand()` processes all MenuBar string commands. StatusBar fully live.

### Agent tasks
1. **Implement `Application::onMenuCommand()`** — parse string commands from MenuBar:
   - `"file.new"` → `onNewFile()`
   - `"file.open"` → `onOpenFile()`
   - `"file.save"` → `onSaveFile()`
   - `"file.saveAs"` → `onSaveFileAs()`
   - `"file.saveAll"` → `onSaveAll()`
   - `"file.close"` → `onCloseFile()`
   - `"file.exit"` → `onExit()`
   - `"edit.undo/redo/cut/copy/paste/delete/selectAll"` → corresponding slots
   - `"edit.find"` → `onFind()`
   - `"edit.replace"` → `onReplace()`
   - `"edit.goto"` → `onGotoLine()`
   - `"search.findNext/Prev/Count/MarkAll/FindInFiles"`, etc.
   - `"view.*"` → `onToggleFullScreen`, `onToggleTabBar`, `onToggleStatusBar`, `onToggleToolBar`, etc.
   - `"encoding.utf8/utf8bom/utf16le/utf16be"` → `onConvertEncoding(EncodingType::...)`
   - `"encoding.charset.*"` → implement charset conversion or log unimplemented
   - `"language.*"` → `onSetLanguage(LangType::...)` — map string to LangType enum
   - `"settings.preferences"` → `onShowPreferences()`
   - `"settings.shortcutMapper"` → `onShowShortcutMapper()`
   - `"settings.commandPalette"` → `onShowCommandPalette()`
   - `"help.about"` → `onShowAbout()`
   - `"view.showTabBar/StatusBar/ToolBar"` → toggle and persist to AppOptions
   - Unknown commands → `qDebug() << "[onMenuCommand] Unknown command:" << cmd`
   - After processing, call `updateUI()` to sync window title and toolbar

2. **Wire StatusBar to Application signals** — in `setupConnections()`:
   - Connect Application signals `bufferActivated` → lambda that calls `setStatusBarEncoding`, `setStatusBarEol`, `setStatusBarLanguage`, `setStatusBarPosition`
   - Connect `ScintillaEditor::cursorPositionChanged` signal → StatusBar::setPosition
   - Connect `ScintillaEditor::modificationChanged` → update dirty indicator in window title
   - Ensure `_selLabel` in StatusBar is initialized (currently missing — add `new QLabel` in constructor)
   - Connect Application::setStatusBarText → `_statusBarWidget->setMessage`

3. **Fix StatusBar::_selLabel** — currently used in `setSelection()` but never initialized in constructor. Add it.

4. **Test:** Build passes, run with `QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt` — no unknown command warnings in stderr.

---

## Wave 6 — Editor-Tab Integration
**Goal:** Each QTabWidget tab owns a ScintillaEditor. Buffer switches update editor content. Tab lifecycle managed correctly.

### Agent tasks
1. **MainWindow tab lifecycle** — `onTabChanged(int index)`:
   - Get BufferID for tab at `index` from FileManager
   - Sync ScintillaEditor's text from Buffer's content
   - Set editor language/encoding/EOL from Buffer
   - Call `Application::setActiveBuffer(bufferID)`
   - Call `Application::setActiveEditor(editor)`

2. **Buffer-to-editor sync** — when buffer is activated:
   - `Application::onBufferActivated(BufferID buffer)` loads content into the active editor
   - `ScintillaEditor::textChanged` → update the corresponding Buffer text back
   - Connect `ScintillaEditor::cursorPositionChanged` → update StatusBar position label
   - Connect `ScintillaEditor::modificationChanged` → update Buffer dirty flag and window title "*"

3. **New file tab** — `onNewFile()`:
   - Create Buffer via FileManager
   - Create new ScintillaEditor
   - Add tab to QTabWidget with "* Untitled" or incrementing name
   - Set as current tab

4. **Close tab** — `onTabCloseRequested(int index)`:
   - If buffer is dirty, prompt to save
   - Remove tab
   - Remove Buffer from FileManager
   - Activate adjacent tab

5. **Tab context menu** — right-click on TabBar:
   - "Close" → close this tab
   - "Close Other Tabs" → keep only this
   - "Close All Tabs" → close all
   - "Open Containing Folder" → open folder in file manager
   - "Copy Full Path" → copy path to clipboard
   - "Rename..." → rename tab (updates Buffer filename)

6. **Dirty indicator in tab** — add/remove `*` prefix when Buffer dirty state changes.

7. **Tab tooltip** — show full file path on hover.

8. **Test:** Build passes, `QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt` opens, creates tab, dirty indicator works.

---

## Wave 7 — File I/O: UTF-16 + Encoding + EOL
**Goal:** Full encoding pipeline: detect on open, convert on save, BOM handling correct.

### Agent tasks
1. **Fix `FileManager::loadFile()` UTF-16** — detect BOM first:
   ```
   if (data.startsWith("\xFF\xFE")) → UTF-16 LE
   if (data.startsWith("\xFE\xFF")) → UTF-16 BE
   if (data.startsWith("\xEF\xBB\xBF")) → UTF-8 BOM
   ```
   Use Qt's `QTextCodec` for proper UTF-16/UTF-32 decoding, not `fromUtf8`.

2. **Fix `FileManager::saveFile()` UTF-16**:
   - Use `QTextCodec::codecForUtf16()` for correct BOM + encoding
   - Ensure BOM is written for UTF-8_BOM and UTF-16 variants
   - Handle EOL conversion (CRLF/LF/CR) before encoding

3. **Encoding detection on open** — `EncodingDetector`:
   - Check BOM first (UTF-8, UTF-16 LE, UTF-16 BE)
   - Try UTF-8 (valid UTF-8 is also valid ASCII)
   - Fall back to local 8-bit (CP1252 on Windows, ISO-8859-1 elsewhere) or auto-detect
   - Return `EncodingType` enum
   - Wire into FileManager::openFile → detect → set Buffer encoding → feed to ScintillaEditor

4. **EOL detection on open** — `Buffer::detectEolFormat()` is already implemented (unused). Wire it into `loadFile`.

5. **Encoding conversion** — `Application::onConvertEncoding(EncodingType enc)`:
   - Get current editor text
   - Re-encode with new encoding
   - Set Buffer encoding
   - Update ScintillaEditor display (convert visible text)
   - Mark buffer dirty

6. **EOL conversion** — `onMenuCommand("eol:CRLF/LF/CR")`:
   - Convert document line endings
   - Update Buffer EOL type
   - Mark dirty

7. **Test:** Create file with UTF-16 Chinese text, save, reopen — content preserved. UTF-8 BOM preserved. CRLF file detected correctly.

---

## Wave 8 — Syntax Highlighting + Theme Integration
**Goal:** Full lexer pipeline, theme switching, dark mode.

### Agent tasks
1. **Buffer → Lexer wiring** — when `Buffer::setLangType(LangType)` is called:
   - Look up `QsciLexer*` for the `LangType` via `LanguageManager`
   - Call `_editor->setLexer(lexer)` on the active ScintillaEditor
   - Call `lexer->setDefaultColor`, `setDefaultPaper` from theme

2. **Theme system** — `ThemeManager`:
   - Load `.theme` JSON files from `~/.config/notepad--qt/themes/`
   - Parse colors: `foreground`, `background`, `selection`, `caret`, `lineHighlight`
   - Apply via `QsciScintilla::setColor()`, `setPaper()`, `setCaretForegroundColor()`, `setCaretLineBackgroundColor()`
   - For each lexer, apply lexer-specific style overrides from theme

3. **Dark mode toggle** — implement `onThemeChanged()`:
   - Switch between "default" (light) and "dark" theme profiles
   - Apply dark palette to all editors, panels, dialogs
   - Persist selection to config

4. **Per-language keywords** — `LanguageManager::getKeywords(LangType)`:
   - Return keyword list strings for SCI_SETKEYWORDS (0–15 sets)
   - Map common languages: C/C++ (Qt, WIN32, GL sets), Python, JavaScript, HTML, etc.

5. **Test:** Open a `.py` file → Python lexer applied. Switch theme → colors change. Dark mode → editor goes dark.

---

## Wave 9 — Auto-Save, Session, Macros, Advanced Edit
**Goal:** Features that make npp-qt actually useful as an editor.

### Agent tasks
1. **Auto-save** — `AppOptions::autoSave`:
   - Timer in Application: `QTimer::singleShot(options.autoSaveInterval * 60000, this, SLOT(autoSaveAll()))`
   - Save all dirty buffers
   - Option `autoSaveCurrentOnly` — save only active buffer
   - Restore unsaved buffers on startup (dirty = needs reload)

2. **Session persistence** — `SessionManager`:
   - Save: list of open buffers with paths, cursor positions, scroll offsets, language, encoding
   - Load: restore all buffers, cursor positions, editor scroll
   - Wire `Application::saveSession()` / `loadSession()` to JSON serialization

3. **Macro recording** — `MacroManager`:
   - Record: capture each edit action (insertText, deleteRange, etc.) with parameters
   - Store as sequence of `MacroAction` objects
   - Playback: replay action sequence
   - Save/load macros to/from XML
   - UI: toolbar buttons + menu items for Record/Stop/Playback

4. **Clipboard history** — `Application::_clipboardHistory`:
   - On each copy, store text in `_clipboardHistory`
   - Ctrl+Shift+V → show clipboard history popup
   - Persist to config file

5. **Column/Box selection mode** — ScintillaEditor:
   - Connect Alt+drag → `setSelectionMode(Qt::AltButton)` → `setVirtualSpaceOptions`
   - Show column selection indicator in status bar

6. **Test:** Record macro, playback, verify. Session save/reload. Auto-save fires correctly.

---

## Wave 10 — Final Integration + Verification + README
**Goal:** Everything works together. CI green. README reflects reality.

### Agent tasks
1. **Final build verification** — `cmake --build build -j$(nproc)` → 0 errors, 0 warnings as errors

2. **Run full test suite** — `ctest --test-dir build --output-on-failure` → all green

3. **Runtime smoke test** — `QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt --help`:
   - Exits cleanly
   - No segfaults
   - No `QWidget: must construct a QApplication before...` errors

4. **Update README.md** — reflect actual state:
   - Replace "Demo-ready" with accurate feature list
   - Document known limitations (no plugin API yet, no column editing UI, etc.)
   - Build instructions: `cmake -B build && cmake --build build`
   - Test instructions: `ctest --test-dir build`
   - Architecture overview

5. **Win32 type sweep** — `grep -rn "HWND\|HDC\|HINSTANCE\|CreateWindow\|ShowWindow\|MoveWindow\|SetWindowText" src/ --include="*.cpp"` → should return 0 results

6. **CMakeLists.txt audit** — all `src/` files listed, no missing entries

7. **Final commit** — tag or branch `v0.1.0-semantic-lift-complete`

---

## Build Command Reference
```bash
cd /home/node/.openclaw/workspace
cmake --build build -j$(nproc) 2>&1 | tail -5
```

## Test Command
```bash
cd /home/node/.openclaw/workspace
ctest --test-dir build --output-on-failure
```

## Binary
`/home/node/.openclaw/workspace/build/NotepadMinusMinusQt`
