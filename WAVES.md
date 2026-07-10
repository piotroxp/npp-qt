# npp-qt: 20-Wave Implementation Plan
# Target: Full Notepad-- Qt6 port from notepad-plus-plus-translation

## Build Command
```bash
cd /home/node/.openclaw/workspace && cmake --build build --target NotepadMinusMinusQt -j$(nproc) 2>&1 | tail -30
```
Binary: `/home/node/.openclaw/workspace/build/NotepadMinusMinusQt`
Workspace: `/home/node/.openclaw/workspace/src/`

## Canonical Source Reference
- **C++ source:** `/home/node/.openclaw/workspace-employee-blunt/notepad-plus-plus-translation/PowerEditor/src/`
- **NppPP GUI:** `/home/node/.openclaw/workspace-employee-blunt/notepad-plus-plus-translation/PowerEditor/src/WinControls/Preference/`
- **Key ref files:**
  - `NppBigSwitch.cpp`, `NppCommands.cpp`, `NppIO.cpp` (already ported)
  - `ScintillaComponent/Buffer.h`, `ScintillaComponent/FindReplaceDlg.cpp` (reference)
  - `WinControls/Preference/preferenceDlg.cpp` (reference for prefs)
  - `ScintillaEditView.cpp` (reference for search)

## READ BEFORE WRITING ANYTHING
1. `cd /home/node/.openclaw/workspace/src && ls -la */`
2. Read ALL relevant .h files before touching .cpp files
3. Read .cpp files before making changes
4. Always compile after changes: `cd /home/node/.openclaw/workspace && cmake --build build --target NotepadMinusMinusQt -j$(nproc) 2>&1 | tail -30`
5. Fix ALL compile errors before declaring wave complete
6. Keep existing API stable — extend, don't break

---

## Wave 1: Find/Replace — Count, Highlight-as-you-type, Direction
**Files:** `dialogs/FindReplaceDialog.{h,cpp}`, `editor/ScintillaEditor.{h,cpp}`, `core/Application.{h,cpp}`, `ui/StatusBar.{h,cpp}`

### What's there (read first):
- `ScintillaEditor::countMatches(text, options)` — exists at line ~170, returns count
- `ScintillaEditor::findNext(text, options, forward)` — exists at line ~131
- `ScintillaEditor::markAllMatches(text, options)` — exists at line ~185
- `Application::onCount()` — has `qDebug("[App] Count occurrences")` placeholder at line 902
- `StatusBar` — read `ui/StatusBar.h` for existing API

### Implement:
1. **FindReplaceDialog::text()** — getter for current find text from `_findCombo`
2. **FindReplaceDialog::currentOptions()** — return current FindOption from dialog state
3. **Count button** — add `_countBtn = new QPushButton("Count")` to dialog, wire to `onCount()`
4. **onCount()** in Application — call `ed->countMatches(text, options)`, show in StatusBar via `setMessage()`
5. **Highlight-as-you-type** — connect `_findCombo->lineEdit()->textChanged` to a 150ms debounce QTimer that calls `markAllMatches()`
6. **Direction toggle** — add `_directionGroup` (Up/Down radio buttons), pass to `findNext(forward)` 
7. **Count result in dialog** — update `_statusLabel` with match count

---

## Wave 2: PreferenceDialog — Replace ALL 8 Placeholder Pages
**Files:** `dialogs/PreferenceDialog.{h,cpp}`, `core/Parameters.{h,cpp}`

### What's there (read first):
- Lines 254, 276, 303, 326: `QLabel* placeholder` stubs (General, Editing, MISC, Cloud)
- Lines ~340-400: `createFileAssociationPage()`, `createShortcutPage()`, `createMarginsPage()`, `createBackupPage()` — all have `QLabel* placeholder`
- `buildSettings()` at ~line 240 returns early — only 2 of 8 pages are live
- `NppParameters` in `core/Parameters.h` — read for existing settings API

### Implement ALL 8 pages:
1. **General** — auto-indent, tab size/spaces, folder margin, title bar, new doc defaults (lang/enc/EOL)
2. **Editing** — multi-selection, caret blink/width, smooth caret, virtual space
3. **MISC** — file status auto-detection, backup mode/path, auto-save interval, recent files max
4. **Cloud** — settings dir override, cloud sync dropdown, recent files location
5. **File Associations** — list widget with extensions + app path, Add/Remove buttons
6. **Shortcut Mapper** — table of commands + key sequences, Edit/Delete buttons (opens small key-combo dialog)
7. **Margins** — line number width spinbox, symbol margin toggle, fold margin toggle, current line highlight toggle
8. **Backup/Auto-Save** — backup mode radio (none/simple/verbose), path selector, auto-save checkbox + interval

### Fix buildSettings() to add all 8 pages to the settings list (currently returns early at "General")
### Add save/load methods: `loadSettings()`, `saveSettings()` — read from NppParameters

---

## Wave 3: DocumentMapPanel — Scroll Sync, Buffer Switching, Current Line Highlight
**Files:** `panels/DocumentMapPanel.{h,cpp}`, `core/Application.{h,cpp}`

### What's there (read first):
- `DocumentMapPanel` constructor exists, `_editor` member, `_scrollBar` member
- `_mapEditor` — the minimap QsciScintilla
- `_viewZone` — the current-line highlight overlay
- `computeEditorFirstVisibleLine()` — exists at line ~319
- `applyEditorFirstVisibleLine()` — exists at line ~327
- `Application` constructor/destructor — find where panels are created and wired

### Implement:
1. **Scroll sync (unidirectional: editor → minimap)**
   - Connect ScintillaEditor signal `SendScintilla(SCI_GETFIRSTVISIBLELINE)` via QTimer 50ms
   - OR add `ScintillaEditor::firstVisibleLineChanged(int)` signal, emit from override
   - Call `DocumentMapPanel::updateViewZone(int firstLine)` on scroll

2. **Minimap scroll → editor sync (bidirectional)**
   - Connect `_scrollBar->valueChanged` to `applyEditorFirstVisibleLine(line)`
   - Only when user actually scrolls the minimap (avoid feedback loop with bool flag)

3. **Current-line highlight**
   - Track `int _currentLine` in DocumentMapPanel
   - On line change, use `_mapEditor->setMarkerBackgroundColor()` or `_mapEditor->indicatorDefine` to highlight current line in minimap

4. **Buffer switching**
   - Connect `Application::activeBufferChanged(BufferID)` to `DocumentMapPanel::onBufferChanged(BufferID)`
   - Sync minimap text from active editor: `_mapEditor->setText(activeEditor->text())`

5. **Application wiring**
   - In `Application::initialize()`, after `_docMapPanel = new DocumentMapPanel(...)`, wire all connections

---

## Wave 4: Shortcut Mapper Dialog
**Files:** `dialogs/ShortcutMapperDialog.{h,cpp}` (NEW), `ui/MainWindow.cpp`, `core/Application.cpp`

### What's there (read first):
- `MainWindow.cpp` line 466: `// TODO: open shortcut mapper`
- `Application.cpp` line 1138: `// TODO: implement shortcut mapper`
- `Parameters.h` — Shortcut and ShortcutMap structures
- `EditorCommandManager.h` — registered commands

### Implement:
1. **ShortcutMapperDialog** (new file pair):
   - QDialog with QTableWidget (columns: Command Name, Current Shortcut, Category)
   - Rows for all registered commands from EditorCommandManager
   - Double-click row → open small KeyComboDialog to record new shortcut
   - "Apply" saves to NppParameters shortcut map
   - "Close" cancels

2. **KeyComboDialog** (inner class or separate):
   - QLabel showing "Press a key combination..."
   - QLineEdit showing recorded keys (Ctrl+Shift+F etc.)
   - "Clear", "OK", "Cancel" buttons
   - Capture via `keyPressEvent()` override

3. **Wire in MainWindow**: `onMenuCommand("settings.shortcutMapper")` → show dialog
4. **Wire in Application**: `onShowShortcutMapper()` → show dialog

---

## Wave 5: Command Palette Dialog
**Files:** `dialogs/CommandPaletteDialog.{h,cpp}` (NEW), `ui/MainWindow.cpp`, `core/Application.cpp`

### What's there (read first):
- `MainWindow.cpp` line 468: `// TODO: open command palette`
- `Application.cpp` line 1143: `// TODO: implement command palette`
- `EditorCommandManager` — registered commands list
- `NppBigSwitch` — registered command IDs

### Implement:
1. **CommandPaletteDialog** (new file pair):
   - QDialog, frameless, centered, 500x400
   - QLineEdit at top (filter input)
   - QListWidget below (filtered command list)
   - As user types, filter commands by name match
   - Enter or click → execute command via NppBigSwitch
   - Escape → close
   - Arrow up/down → navigate list
   - Show command name + shortcut hint in each row

2. **Quick command data** — populate from EditorCommandManager + NppBigSwitch registered commands
3. **Wire**: MainWindow `Ctrl+Shift+P` or menu → show dialog
4. **Application::onShowCommandPalette()** → show dialog

---

## Wave 6: ScintillaEditor — Indentation and Whitespace Fixes
**Files:** `editor/ScintillaEditor.{h,cpp}`, `editor/SyntaxHighlighter.{h,cpp}`

### What's there (read first):
- Line 121: `setIndent(int) {}` — empty stub
- Line 123: `indent() const { return 0; }` — empty stub  
- Line 126: `setWhitespaceVisibility(bool) {}` — empty stub
- Line 127: `setEolVisibility(bool) {}` — empty stub
- Read `_editor` type (QsciScintilla*) usage in existing methods

### Implement:
1. **setIndent(int level)** — compute indent from level × tabWidth, call `_editor->setIndentation(level)`
2. **indent()** — return `_editor->indentation()` 
3. **setWhitespaceVisibility** — use `_editor->setWhitespaceSize(pixels)` or `setWhitespaceForegroundColor` + `setWhitespaceBackgroundColor` + `setWhitespaceVisibility`
4. **setEolVisibility** — use `_editor->setEolMode()` to show/hide line endings
5. **Add auto-indent support** — `setAutoIndent(bool)` → `_editor->setAutoIndentFunction(...)` or implement manual: on newLine, copy previous line's indentation
6. **Add setCaretLineVisibility(bool)** → `_editor->setCaretLineVisible(true/false)`
7. **Add setCaretWidth(int pixels)** → `_editor->setCaretWidth(pixels)`

---

## Wave 7: Util — MD5 and SHA-256 Implementations
**Files:** `common/Util.{h,cpp}`

### What's there (read first):
- Line 317: `// Simple placeholder - use a proper MD5 implementation`
- Line 323: `// Simple placeholder - use a proper SHA-256 implementation`
- `Util.h` — existing hash() methods
- Use Qt6 crypto: `QCryptographicHash` (built into QtCore, no external deps)

### Implement:
1. **MD5** — `QCryptographicHash::hash(data, QCryptographicHash::Md5)`
2. **SHA-256** — `QCryptographicHash::hash(data, QCryptographicHash::Sha256)`
3. **Replace the placeholder comments** with real implementations using QCryptographicHash
4. **File hashing** — add `hashFile(path, Algorithm)` → read file, hash, return hex string
5. **String hashing** convenience wrappers

---

## Wave 8: Parameters — Font List, Shortcut Finder, Other Stubs
**Files:** `core/Parameters.{h,cpp}`

### What's there (read first):
- Line 115: `return nullptr;` (getFontList or findShortcut stub)
- Line 167: `return nullptr;` (another stub)  
- `Parameters.h` — existing class structure, theme handling

### Implement:
1. **getFontList()** — return `QStringList` from `QFontDatabase::families()`, cache it
2. **findShortcut(commandName)** — look up in `_shortcutIndex` map, return Shortcut* or nullptr
3. **addShortcut(cmd, keyseq)** — add to map and index
4. **removeShortcut(cmd)** — remove from map and index
5. **getAllShortcuts()** — return all shortcuts for shortcut mapper
6. **Check for any other `return nullptr` or stub lines** and implement them properly

---

## Wave 9: FileManager — Reload, File Watch, Save Operations
**Files:** `core/FileManager.{h,cpp}`

### What's there (read first):
- Line 284: `return nullptr;` — some method returns null
- Line 389: `return nullptr;` — another method
- Lines with `return false;` — error paths that may need real handling
- Read all return false paths to understand error conditions

### Implement:
1. **reloadBuffer(BufferID)** — re-read file from disk, detect changes, prompt if modified
2. **watchFile(path)** — set up QFileSystemWatcher for the file, connect to `fileChanged` signal → reload
3. **saveBuffer(BufferID, path)** — write buffer content to file with correct encoding
4. **saveBufferAs(BufferID, newPath)** — write buffer to new path, update Buffer's path
5. **detectRemoteChanges(BufferID)** — check if file on disk changed while buffer was open, offer reload
6. **removeFileFromWatcher(path)** — clean up file watcher on buffer close

---

## Wave 10: Buffer — Large File Handling, Dirty Detection
**Files:** `core/Buffer.{h,cpp}`

### What's there (read first):
- Line 544: `// TODO: Read from application settings for large file restrictions`
- Read Buffer.h for existing API

### Implement:
1. **Large file detection** — check file size vs configurable threshold (e.g., 10MB default)
2. **Large file mode** — if file > threshold, open in read-only mode or with limited lexing
3. **isDirty()** — track modification state properly (combine _isModified and _fileStatus)
4. **getFileSize()** — return cached file size
5. **updateTimestamp()** — update cached mtime
6. **detectExternalChanges()** — compare mtime/size with cached values, return true if changed
7. **notifyFileDeleted()** — handle case where file was deleted outside editor

---

## Wave 11: SessionManager — Full Session Persistence
**Files:** `core/SessionManager.{h,cpp}`, `core/Parameters.{h,cpp}`

### What's there (read first):
- `SessionManager.cpp` — read all methods
- `SessionManager.h` — existing API
- `Parameters.h` — session path access

### Implement:
1. **saveSession(path)** — save current open files, active file, cursor positions, panel states to JSON/XML
2. **loadSession(path)** — restore all of the above
3. **getLastSessionPath()** — return path to last-used session
4. **saveWindowState()** — save window geometry, panel sizes/docks to settings
5. **restoreWindowState()** — restore on startup
6. **saveRecentFiles()** — persist recent file list
7. **autoSaveSession()** — timer-based session backup every N minutes
8. **Check all return false paths** — implement proper error handling

---

## Wave 12: MainWindow — All Menu Command Stubs
**Files:** `ui/MainWindow.{h,cpp}`, `core/Application.{h,cpp}`

### What's there (read first):
- `dispatchCommand()` — read all unhandled command branches
- `MenuBar.cpp` — find all `onMenuCommand` stubs
- `Application.cpp` — find all `qDebug` placeholder slots
- `NppBigSwitch.cpp` — see which commands are registered

### Implement missing commands in Application.cpp:
1. **reload** — `onReload()` — reload current file from disk
2. **print** — `onPrint()` — invoke QPrinter dialog + render document
3. **deleteLine** — call Scintilla: delete current line
4. **joinLines** — join current line with next
5. **trimTrailing** — remove trailing whitespace from all lines
6. **trimLeading** — remove leading whitespace from all lines  
7. **sortAsc/Desc** — sort selected lines alphabetically
8. **sortIntAsc/Desc** — sort selected lines as integers
9. **openInFolder** — `QDesktopServices::openUrl(QUrl::fromLocalFile(path))`
10. **searchOnInternet** — search selected text on configured engine
11. **zoomIn/zoomOut/zoomRestore** — call Scintilla zoom methods
12. **toggleWordWrap** — toggle wrap mode
13. **toggleEol** — toggle EOL visibility
14. **unmarkAll** — clear all indicators/marks in current editor

---

## Wave 13: NppIO — All Return-false Stubs
**Files:** `core/NppIO.{h,cpp}`

### What's there (read first):
- Lines 63, 77, 152, 164, 180, 186, 278, 289, 317, 354, 366, 372, 383, 392, 401, 414, 420, 425, 434 — many `return false;`
- `NppIO.h` — massive 764-line header, read carefully
- These are likely error branches for file encoding detection, BOM handling, EOL conversion

### Implement each return false case:
1. For each `return false;` branch, understand the context:
   - BOM detection failure → try alternative encoding
   - Encoding conversion failure → fallback to raw bytes
   - EOL detection failure → default to platform EOL
   - File write failure → report error, don't lose data
2. Add `qWarning()` logging for each error path (replace silent return false)
3. For critical paths (encoding detection), try multiple encodings before failing
4. Make file saves atomic: write to temp file, then rename
5. **CRITICAL**: Fix the `return false` in save operations — currently saves may silently fail

---

## Wave 14: SyntaxHighlighter — Complete Parser Definitions
**Files:** `editor/SyntaxHighlighter.{h,cpp}`

### What's there (read first):
- `SyntaxHighlighter.cpp` — existing lexer registration
- `LanguageManager.cpp` — existing language definitions
- `common/Constants.h` — existing lexer constants

### Implement:
1. **Complete language lexer map** — ensure ALL languages in NPP's `langs.model.xml` have a QsciLexer* mapping
   - Check `/home/node/.openclaw/workspace-employee-blunt/notepad-plus-plus-translation/PowerEditor/src/langs.model.xml`
   - Add missing lexers: Rust, Go, Swift, Kotlin, Objective-C, CMake, PowerShell, YAML, TOML, Julia, R, MATLAB, etc.
2. **Theme/color schemes** — map NPP theme colors (from XML) to QsciLexer style indices
3. **Foldable regions** — for languages with significant indentation (Python, YAML) or brace-based (C++, JS)
4. **Auto-detect improvements** — use file extension + content heuristics for better detection
5. **Code page support** — handle non-ASCII identifiers in lexers

---

## Wave 15: MacroManager — Record, Playback, Save/Load
**Files:** `core/MacroManager.{h,cpp}`

### What's there (read first):
- `MacroManager.cpp` (185 lines) — read all existing code
- `MacroManager.h` — existing API

### Implement:
1. **Recording** — `startRecording()` → capture each editor command with parameters
   - Store: command ID, cursor position before/after, affected text
2. **Stop recording** → `stopRecording()` → save to `_currentMacro`
3. **Playback** — `playbackMacro(Macro* m)` → replay each recorded action via EditorCommandManager
4. **Save macro** — `saveMacro(name)` → serialize to XML/JSON, store in config
5. **Load macro** — `loadMacros()` → deserialize on startup
6. **Delete macro** — remove from storage
7. **Macro editor dialog** (new `MacroEditDialog.{h,cpp}`):
   - List saved macros
   - Play / Delete / Rename buttons
   - Show macro contents (command sequence as readable text)

---

## Wave 16: AboutDialog — Enrichment
**Files:** `dialogs/AboutDialog.{h,cpp}`

### What's there (read first):
- `AboutDialog.cpp` (55 lines) — minimal implementation
- Read existing AboutDialog carefully

### Implement:
1. **Version info** — read from `CMakeLists.txt` or define VERSION string
2. **Credits section** — list open source components (Scintilla, Qt, etc.) with license info
3. **Qt version** — show `QT_VERSION_STR`
4. **Link to project page** — clickable link
5. **Build info** — build date, compiler, git commit hash (if available)
6. **Update check** button (optional — show "not implemented" message)

---

## Wave 17: Print / Page Setup Dialog
**Files:** `dialogs/PrintDialog.{h,cpp}` (NEW), `core/Application.{h,cpp}`

### Implement:
1. **PrintDialog** (new file pair):
   - QPrintPreviewDialog + QPrinter setup
   - Render Scintilla content via `print()` API or `QPainter` drawing
   - Header/footer with filename, page number, date
   - Color mode toggle: color / monochrome
   - Line numbers toggle
2. **PageSetupDialog** (or integrate into PrintDialog):
   - Margins, orientation, paper size
3. **Application::onPrint()** — show print dialog, print active editor
4. **Application::onPageSetup()** — show page setup
5. **Wire into MenuBar** and NppBigSwitch: `IDM_FILE_PRINT`

---

## Wave 18: Dark Mode / Theme Support
**Files:** `core/Parameters.{h,cpp}`, `ui/MainWindow.{h,cpp}`, `common/Constants.h`

### What's there (read first):
- `Parameters.h` — NppTheme struct
- Read `NppDarkMode.h` from the translation reference
- `/home/node/.openclaw/workspace-employee-blunt/notepad-plus-plus-translation/PowerEditor/src/NppDarkMode.cpp`

### Implement:
1. **Theme loading** — parse NPP's `themes/*.xml` files into NppTheme struct
2. **Apply theme to Scintilla** — map theme colors to QsciLexer style indices
3. **Dark mode detection** — use `QStyleHints::colorScheme()` to detect system dark mode
4. **Auto dark mode** — if system is dark, auto-apply dark theme
5. **Theme switcher** — in Preferences, add theme dropdown, apply on selection
6. **Apply to UI elements** — use QPalette to darken dock widgets, menu bars, status bars
7. **NppTheme → Qt palette** — map NPP's dark palette colors to Qt widget styling

---

## Wave 19: Window State Management
**Files:** `ui/MainWindow.{h,cpp}`, `core/Application.{h,cpp}`, `core/Parameters.{h,cpp}`

### Implement:
1. **saveWindowGeometry()** — save window position, size, maximized state to NppParameters
2. **restoreWindowGeometry()** — restore on startup
3. **saveDockState()** — save panel dock positions, sizes using `QMainWindow::saveState()`
4. **restoreDockState()** — restore using `QMainWindow::restoreState()`
5. **Fullscreen mode** — `onFullscreen()` toggle, hide all panels except editor
6. **Always on top** — `onAlwaysOnTop()` toggle
7. **Multi-monitor support** — save/restore which monitor the window is on
8. **Tab bar** — save open tabs, restore on startup (part of session but here as window state)

---

## Wave 20: Final Integration + Comprehensive Testing
**Files:** ALL — final cleanup pass

### Goals:
1. **Compile cleanly** — zero warnings if possible, zero errors
2. **All menu items wired** — every menu item in MenuBar has a handler
3. **All toolbar buttons wired** — every ToolBar action has a handler
4. **All panel signals wired** — DocumentMap, FunctionList, FileBrowser all connected
5. **Binary starts** — `./build/NotepadMinusMinusQt` launches without crash
6. **Open a file** — File → Open works, file loads in editor
7. **Syntax highlighting** — at least C++ and Python highlight correctly
8. **Search works** — Ctrl+F opens find dialog, find next works
9. **Status bar** — shows line/col, encoding, EOL
10. **Session restore** — reopen last session on startup

### Implementation:
1. **Run full build** and fix ALL remaining errors/warnings
2. **Audit MenuBar.cpp** — every `connect()` must have a corresponding handler
3. **Audit ToolBar.cpp** — every action must be wired
4. **Test all 20 core commands** manually (can use headless testing if possible)
5. **Document any remaining known limitations** in a `LIMITATIONS.md` file
6. **Update README.md** with current feature status
7. **Tag commit** as `v0.1.0-rc1` or similar

---

## Wave Dependency Notes
- Wave 3 (DocumentMap) depends on: Application panel wiring knowledge
- Wave 4 (Shortcut Mapper) depends on: Wave 2 (Preferences framework)  
- Wave 5 (Command Palette) depends on: EditorCommandManager already complete
- Wave 12 (MainWindow stubs) is the biggest integration wave
- Wave 20 is the final cleanup — all others are independent

## Success Criteria Per Wave
1. `cmake --build build --target NotepadMinusMinusQt -j$(nproc)` → 0 errors
2. Binary `./build/NotepadMinusMinusQt` starts without segfault
3. Changed functionality works as specified
4. No regression: previously working features still work
