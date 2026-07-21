# Changelog

## [Unreleased]

### Fixed
- **Application singleton deadlock at `__cxa_guard_acquire`** —
  Replaced `static Application inst;` (compiler-emitted guard around
  the ctor) with Construct-On-First-Use heap idiom
  (`static Application* inst = nullptr; … inst = new Application();`).
  The atomic-pointer store has no ctor in its critical section, so a
  re-entrant call from inside `Application::Application()` returns
  the partially-constructed pointer rather than blocking on a guard
  that's still held. Multi-thread contention is serialised by an
  `std::atomic<bool> in_progress` flag with `std::this_thread::yield()`
  spin-wait instead of the compiler-generated guard's opaque blocking.
  This was the headline deadlock surfaced in the prior live QA cycle
  (QFileOpenEvent handler racing the static-init guard). New
  `test_application_singleton.cpp` exercises 16 concurrent + 64000
  contention-stress calls; the test would deadlock under the old
  pattern. `StubApplication.cpp`'s singleton mirror is updated to
  the same idiom so the test verifies the right contract.

### Added
- **SnippetsManager**: `src/core/SnippetManager.{h,cpp}` — INI-backed code-snippet
  storage with `$VAR` / `$1` / `$0` / `|` placeholder expansion and cursor-offset
  translation. Wired into `Application::instance()` and into
  `AutoCompletion::showSnippetCompletion()` so the snippet list is driven by
  the real manager instead of the previous stub fallback. Six unit test blocks
  in `src/tests/test_snippets.cpp` cover INI round-trip, hot reload, numbered
  and named tab stops, and cursor offset.
- **MacroAction::playback()** lifted from a `Q_UNUSED` stub to a real
  Scintilla dispatcher. Integer-param actions forward to
  `editor->qsciEditor()->SendScintilla(cmd, intParam)`. String-param actions
  (SCI_REPLACESEL / SCI_ADDTEXT / SCI_INSERTTEXT) route through SCI_REPLACESEL
  with the UTF-8 payload. Null-editor and null-qsciEditor() inputs are
  safe no-ops. New `test_macros.cpp` test cases
  (`test_macro_action_int_param_dispatches`,
  `test_macro_action_string_param_replaces_selection`,
  `test_macro_action_serialization_roundtrip`,
  `test_macro_action_null_editor_safe`) verify each branch reaches the
  underlying editor.

### Fixed
- **Misleading "stub" comments**: renamed three places where the word
  "stub" was used to describe code that is not stub-shaped:
  `Application::zoomIn/zoomOut/...` "forwarding stubs" → "forwarding bridges";
  `Application.h` "NppBigSwitch compatibility stubs" → "compatibility
  bridges"; `UserDefineDialog::setupOperatorTable` "Keyword table helpers
  (stubs resolved)" → "reset to defaults".
- **Application::onToolbarCustomize()** message no longer claims "not yet
  implemented"; it now points users at the existing Shortcut Mapper
  workflow until the dedicated toolbar-customize dialog lands.

### Build / docs
- **CONTRIBUTING.md** — development workflow, build instructions,
  test-suite overview, plugin authoring guide.
- **GitHub Actions CI** workflow (`.github/workflows/ci.yml`) —
  Ubuntu + Qt 6.5 + qtscintilla, build + ctest on push / PR.
- **CMake install target** — `install(TARGETS NotepadMinusMinusQt ...)`,
  themes in `share/notepad--qt/themes`, README in
  `share/doc/notepad--qt`.
- **ColourPicker widget**: Qt6 swatch button (`ColourPicker`) + popup grid (`ColourPopup`) with all 48 N++ palette colours and "More Colours…" button opening `QColorDialog`. Win32→Qt6 conversion: `HWND`/`CreateWindowEx` → `QWidget` constructor, `WM_PAINT`/`HDC` → `paintEvent`/`QPainter`, `WM_ERASEBKGND` → merged into `paintEvent`, `WM_LBUTTONDOWN/DBLCLK` → `mousePressEvent`/`mouseDoubleClickEvent`, `WM_RBUTTONDOWN` → right-click toggle, `Subclassing`/`WNDPROC` → Qt event overrides, `ChooseColor` → `QColorDialog`, `COLORREF`/`RGB()` → `QColor`.
- **AnsiCharPanel**: Character panel as `QDockWidget` with a 16×16 grid (`QTableWidget`) covering all 256 byte values, with a combo box to switch between 15 code pages (437, 850, 1250–1258, 874, 932, 936, 949, 950). Cells are colour-coded by character class (control/printable/extended). Double-click inserts the character into the active editor via `ScintillaEditView`. Win32→Qt6: `ListView` (commctrl) → `QTableWidget`, `WM_NOTIFY`/`NM_DBLCLK` → `itemDoubleClicked()` signal, `LoadString` → `tr()`.
- **ClipboardHistoryControl**: Clipboard monitoring widget using `QClipboard::changed`, `QListWidget` history list with search/filter, context menu (copy/paste/delete/clear), and `QSettings` persistence. Win32→Qt6: `OpenClipboard`/`GetClipboardData` → `QApplication::clipboard()->text()`, `SetClipboardViewer`/`ChangeClipboardChain` → `QClipboard::changed` signal, `SendDlgItemMessage(LB_*)` → `QListWidget` methods, `GlobalLock`/`GlobalUnlock` → `QByteArray`, `GetPrivateProfileString` → `QSettings`, `DrawText` on `HDC` → `QListWidgetItem` data roles.
- **Function List XML Parser**: Loads upstream Notepad++ `functionList/*.xml` parsers (52 languages) via `FunctionListXmlParser`. N++ regex converted to Qt-compatible patterns. Supports `classRange`, `function`, `functionName`, `functionSig`, `functionDisplay` elements.
- **54 Language Definitions**: Full `SyntaxHighlighter` with 54 `setup*()` methods covering all major languages.
- **New Language Parsers**: Added Swift, Kotlin, TypeScript, C# parsers to FunctionListPanel.
- **WebBrowserView**: `QWebEngineView` panel for HTML/Markdown preview with zoom and navigation.
- **FileLoaderWorker**: Async file I/O with progress reporting and large-file (>5 MB) warning dialog.
- **Docker Build**: Added `qt6-webengine` to Dockerfile. Fixed build errors (`mkstemp` `QByteArray`, `QDir::Recursive` → `QDirIterator::Subdirectories`).
- **Documentation**: Expanded `docs/ARCHITECTURE.md` with complete module maps; updated `README.md` with full feature list and semantic lift table.

### Fixed
- `FileHelper::getTempFileName`: Fixed `mkstemp` call — `QChar*` → `QByteArray::data()`.
- `FileHelper::listFiles`/`listDirs`: Fixed `QDir::Recursive` (removed in Qt6) → `QDirIterator` with `Subdirectories`.
- `LangType::L_OBJECTIVEC` → `L_OBJC` (correct enum name).
- Accidentally-deleted `CHANGELOG.md`, `HEARTBEAT.md`, `WAVES.md` restored from `d81f8a5a`.

## [0.2.0] — 2026-07-13

### Added
- **Find in Files**: Background grep across directory trees with regex, match-case, whole-word support. Results grouped by file, double-click to open at line. Replace in Files supported.
- **File Change Monitoring**: QFileSystemWatcher integration — auto-reloads clean files modified externally, prompts for dirty ones.
- **Plugin API Skeleton**: `NppQtFuncs` function table, `NppQtPlugin` struct, `NPPQT_PLUGIN` macro, `PluginManager` with `QLibrary`-based `.so/.dylib` loading from `~/.config/notepad--qt/plugins/`.
- **Window Title Polish**: Shows `filename - Notepad--Qt` with `*` prefix for dirty buffers.
- **Status Bar Selection Info**: Shows `"N chars, M lines sel"` when text is selected.
- **Keyword Lists**: C/C++, Java, Python, JavaScript, Shell keyword sets applied to lexers.
- **Charset Conversion**: Convert document to any Qt-supported charset (windows-1250, ISO-8859-1, etc.).
- **Print Dialog**: `QPrinter` + `QPrintDialog` integration via `file.print`.
- **Macro Save/Load**: Serialize/deserialize macro recordings to JSON.

### Fixed
- `tabsToSpaces`: Fixed tab→spaces conversion (was computing next-tab-stop incorrectly)
- `spacesToTabs`: Fixed spaces→tabs (was only replacing last space of group)
- `IniParser`: Completely rewritten with clean compound-key approach (`section\x1Fkey`)
- `FileManager::getBufferText`: Now stores and retrieves text from `_bufferText` map
- Zoom: `zoomIn()`/`zoomOut()`/`zoomReset()` properly track `_zoomLevel`
- StatusBar: Fixed `_selLabel` initialization crash

## [0.1.0] — 2026-07-13

### Added
- Semantic lift from `notepad-plus-plus-translation` — full Win32→Qt6 port
- 95 source files across core, editor, ui, dialogs, panels, common
- Multi-tab editing with per-tab document buffers
- Full syntax highlighting via QScintilla (18 languages)
- UTF-8, UTF-16 LE/BE, and legacy encodings with BOM handling
- EOL conversion (CRLF, LF, CR)
- Find & Replace with regex, whole-word, match-case
- Session persistence (JSON)
- Auto-save (configurable interval)
- Macro recording and playback
- Clipboard history (50 entries)
- Column/Box selection mode
- Dark mode / Light mode theming
- Keyboard shortcut mapper
- Command palette (Ctrl+Shift+P, 48 commands)
- File browser, Function list, Document map panels
- Recent files menu
- GitHub Actions CI workflow

### Notes
- Plugin API not yet implemented (compatibility is a future goal)
- `QsciLexerPHP` not available in QScintilla Qt6
- Some language lexers may be unavailable depending on QScintilla version
