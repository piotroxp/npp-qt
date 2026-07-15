# Architecture — Notepad--Qt

## Overview

Notepad--Qt is a semantic lift of Notepad++ from Win32/C++ to Qt6/C++. The architecture mirrors the original Notepad++ design philosophy — a single-window MDI editor with a dockable-panel layout, buffer-backed document model, and command-driven UI — implemented natively on Qt.

---

## Design Principles

1. **Single-window MDI** — One `QMainWindow` with a central tabbed editor and dockable panels.
2. **Buffer model** — All open documents live in a `Buffer` object managed by `FileManager`. The UI holds a pointer, never owns the buffer.
3. **Signal/slot reactive UI** — UI components (MenuBar, ToolBar, TabBar, StatusBar) are driven by `Application` signals; they emit command signals back which `Application` routes.
4. **Singleton managers** — `EncodingDetector`, `LanguageManager`, `EditorCommandManager`, `SessionManager` are singleton objects initialized once at startup.
5. **No shared mutable state across threads** — All file I/O, encoding detection, and UI updates are serialised through `Application` or dispatched to a background thread pool.

---

## Module Map

### `src/core/`

| Class | Responsibility |
|-------|---------------|
| `Application` | Singleton orchestrator. Owns managers, builds the UI, routes commands, handles shutdown. |
| `Buffer` | Document state: file path, encoding, language, modification flag, SciTe text storage, undo stack. |
| `FileManager` | Manages the buffer list, active buffer, open/close/new operations. |
| `EncodingDetector` | Detects file encoding (BOM, heuristic) using Qt text codec utilities. |
| `LanguageManager` | Maps file extensions/Shebang → `LexerMode` (Qsci lexer ID). |
| `SessionManager` | Loads/saves `.session` XML files: open files, active tab, window geometry, panel visibility. |
| `EditorCommandManager` | Registers and dispatches named commands to handlers on `ScintillaEditor` or `Application`. |

### `src/ui/`

| Class | Responsibility |
|-------|---------------|
| `MainWindow` | `QMainWindow`. Builds menu bar, tool bar, status bar, panels, central tab widget. Owns `Application`. |
| `MenuBar` | `QMenuBar`. Reads command registry from `EditorCommandManager`, builds menus, emits `commandInvoked(id)` signals. |
| `ToolBar` | `QToolBar`. Icon buttons wired to command IDs. |
| `TabBar` | `QTabBar` + `QTabWidget` adapter. Bidirectionally synced with `FileManager`'s buffer list. |
| `StatusBar` | `QStatusBar`. Shows cursor position, encoding, EOL mode, language, zoom. |

### `src/panels/`

| Class | Responsibility |
|-------|---------------|
| `FunctionListPanel` | `QDockWidget`. Parses buffer text with regex per language, builds a collapsible tree (functions/classes/structs), supports "go to function" on double-click. |
| `FileBrowserPanel` | `QDockWidget`. Recursive directory tree with `QFileSystemWatcher` for live updates. Supports filtering, context menu for open/delete/rename. |
| `DocumentMapPanel` | `QDockWidget`. Scaled-down `QsciScintilla` minimap, bidirectionally scroll-synced with the active editor. |
| `ClipboardHistoryPanel` | `QDockWidget`. Persistent clipboard history stack, re-paste any entry, clear history. |

### `src/editor/`

| Class | Responsibility |
|-------|---------------|
| `ScintillaEditor` | `QFrame` wrapping `QsciScintilla`. Provides the text widget with lexer, margin, annotation, and marker APIs. Exposes `executeCommand(cmd, arg)` for all Scintilla messages. |
| `ScintillaEditView` | Per-buffer editing view: owns a `ScintillaEditor` widget, syncs with `Buffer` state, handles split-view (clone). |
| `ScintillaCtrls` | Factory/manager for creating `QsciScintilla` instances with shared settings (margins, caret, selection colours). |
| `LexerConfig` | Central lexer configuration: maps `LexerMode` → `QsciLexer*` instance, applies theme colours. |
| `SyntaxHighlighter` | 54 language definitions via `setup*()` methods: `setupCpp()`, `setupPython()`, `setupJavaScript()`, `setupSql()`, `setupXml()`, `setupMarkdown()`, `setupRust()`, `setupGo()`, `setupLua()`, `setupRuby()`, `setupPhp()`, `setupPerl()`, `setupBash()`, `setupMakefile()`, `setupJson()`, `setupYaml()`, `setupToml()`, `setupIni()`, `setupDiff()`, `setupNfo()`, `setupPascal()`, `setupFortran()`, `setupLisp()`, `setupHaskell()`, `setupScala()`, `setupKotlin()`, `setupSwift()`, `setupDart()`, `setupR()`, `setupMatlab()`, `setupOctave()`, `setupJulia()`, `setupTex()`, `setupLaTeX()`, `setupMarkdown()`, `setupProperties()`, `setupNsis()`, `setupInnoSetup()`, `setupBatch()`, `setupPowerShell()`, `setupDockerfile()`, `setupCMake()`, `setupVim()`, `setupASM()`, `setupVerilog()`, `setupVHDL()`, `setupTCL()`, `setupCOBOL()`, `setupAda()`, `setupEiffel()`, `setupErlang()`, `setupClojure()`, `setupFSharp()`, `setupOCaml()`, `setupForth()`, `setupBrainfuck()`, `setupWhitespace()`, and `setupDefault()`. |
| `DocTabView` | `QTabWidget` adapter holding `ScintillaEditView` pages, bidirectionally synced with `FileManager`'s buffer list. |
| `WebBrowserView` | `QWebEngineView` panel for previewing rendered HTML/Markdown. Supports zoom, navigation, back/forward. |
| `WebBrowserViewFactory` | Factory creating `WebBrowserView` instances per buffer on demand. |

### `src/dialogs/`

| Class | Responsibility |
|-------|---------------|
| `FindReplaceDialog` | Modeless `QDialog`. Supports regex, whole-word, in-selection, counter (Find Next/Previous), Replace/Replace All, Mark. |
| `GoToLineDialog` | Simple `QDialog` with line number input and offset mode (absolute/relative/percentage). |
| `PreferenceDialog` | Tabbed `QDialog`. 7 categories: General, Editing, Language, Search, Backup, Print, Advanced. |
| `AboutDialog` | Static `QDialog` with app info, credits, license, and build info. |
| `CommandPaletteDialog` | `QDialog` triggered by Ctrl+Shift+P. Fuzzy search over all registered commands. |
| `UserDefineDialog` | `QDialog` for User Defined Language (UDL) editing — keywords, operators, comment styles, colour schemes. |
| `FunctionCallTip` | `QWidget` overlay showing function signatures, parameter highlighting, and overload navigation (↑/↓). |
| `AutoCompletion` | `QDialog` for word and API auto-completion popup. |
| `ClipboardHistoryPanel` | `QDialog` / docked panel showing clipboard stack with re-paste support. |
| `FindInFilesWorker` | Background worker for recursive search across directories with progress reporting. |
| `Printer` | Print preview and print-to-PDF via `QPrinter`. |
| `ShortcutMapperDialog` | `QDialog` for binding key combinations to command IDs. |
| `SmartHighlighter` | `QDialog` for multi-keyword markup (search result highlighting). |
| `IncrementalSearchDialog` | Lightweight inline search bar (Ctrl+I style). |
| `ColumnEditorDialog` | `QDialog` for inserting sequential numbers or character sequences column-wise. |
| `LargeFileWarningDialog` | `QDialog` shown before loading files >5 MB with partial-load option. |
| `FileReloadDialog` | `QDialog` shown when an open file is modified externally. |
| `AnsiCharPanel` | `QDialog` for inserting special ANSI characters by code point. |

### `src/workers/`

| Class | Responsibility |
|-------|---------------|
| `FileLoaderWorker` | `QThread` worker for async file I/O. Streams large files in chunks, reports progress, handles cancellation. Detects files >5 MB and triggers `LargeFileWarningDialog`. |

### `src/plugins/`

| Class | Responsibility |
|-------|---------------|
| `PluginManager` | Loads/symbol-lookups plugin DLLs, forwards `NppMenuCmd` IDs, manages plugin on/off state. |
| `PluginInterface` | Header-only interface matching Notepad++ plugin API (DLL export macros, `setInfo`/`getFuncs`). |

### `src/core/` (extended)

| Class | Responsibility |
|-------|---------------|
| `NppBigSwitch` | Central switch-case mapping `NppMenuCmd` enum → human-readable name strings + debug logging. |
| `NppCommands` | Concrete implementations for menu commands not delegated to `EditorCommandManager`. |
| `NppIO` | All file read/write: encoding-aware `QTextStream` wrapping, BOM insertion, atomic writes. |
| `RecentFilesManager` | Manages the recent files list, persisted to `QSettings`. |
| `MacroManager` | Records/replays `MacroAction` sequences (keystrokes + commands) stored as JSON. |
| `MacroAction` | Represents a single recorded action (type, position, text, optional string arg). |
| `Parameters` | Global `QSettings` access wrapper — reads/writes all preference keys. |
| `ShortcutManager` | Maps `QKeySequence` → `NppMenuCmd` IDs; resolves conflicts. |
| `ThemeManager` | Loads/switches between dark and light theme palettes, applies to all widgets. |
| `UdlManager` | Persists User Defined Language definitions to/from XML. |
| `WorkspaceManager` | Manages workspace/project files (`.workspace` JSON) grouping open files. |

### `src/common/`

| Class | Responsibility |
|-------|---------------|
| `Util` | Path normalisation, file existence checks, OS detection, string conversion utilities. |
| `StringHelper` | Clipboard, case conversion, trim, pad, word-wrap, line/column utilities. |
| `FileHelper` | File copy, backup naming, temp file, file size, MIME type utilities. |
| `Constants` | Global constants: app name, version, default settings. |
| `Observer` | Minimal observer pattern (signal/slot via Qt's built-in mechanism). |
| `NonCopyable` | `Q_DISABLE_COPY_MOVE` base. |

---

## Signal Flow

```
User input (key/menu/toolbar)
  → MenuBar / ToolBar emits commandInvoked(id)
      → Application::handleCommand(id)
          → EditorCommandManager::dispatch(id, editor)
              → ScintillaEditor::executeCommand(cmd, arg)
                  → QsciScintilla API
          OR  → Application or FileManager method
                  → Buffer / FileManager state change
                      → Application emits bufferChanged(buffer)
                          → TabBar / StatusBar / FunctionListPanel update
```

---

## File Lifecycle

```
open / new
  → FileManager::openFile(path) or ::newFile()
      → EncodingDetector::detect(path) → Encoding
          → LanguageManager::detectLanguage(path, content) → LexerMode
              → Buffer(path, encoding, lang)
                  → FileManager::addBuffer(buffer)
                      → Application emits bufferOpened(buffer)
                          → MainWindow creates ScintillaEditor widget
                              → TabBar adds tab
```

---

## Session Persistence

`SessionManager` writes/reads `.session` XML:

```xml
<Session>
  <Files>
    <File path="..." encoding="..." lang="..." />
  </Files>
  <ActiveTab index="0" />
  <WindowGeometry x=".." y=".." w=".." h=".." />
  <PanelVisibility funcList="true" fileBrowser="false" docMap="true" />
</Session>
```

---

## Testing Strategy

Unit tests via **Catch2** (`src/tests/`):
- `test_buffer.cpp` — buffer creation, dirty flag, encoding
- `test_encoding_detector.cpp` — BOM detection, heuristic
- `test_string_helper.cpp` — trim, pad, word wrap, case
- `test_util.cpp` — path normalisation, file helpers

Run: `ctest --output-on-failure` in the build directory.

---

## Semantic Lift Notes

This codebase was generated via a **semantic lift** — each original Notepad++ C++/Win32 module was translated to a Qt equivalent:

| Original (Win32) | Target (Qt) |
|-----------------|------------|
| WinMain / HWND | QApplication / QMainWindow |
| `SendMessage(hwnd, SCI_*)` | `QsciScintilla::SendScintilla(QsciScintilla::Command, long)` |
| `HDC` / GDI drawing | Qt paint system (`QPainter`) |
| `std::vector<Buffer>` | `std::vector<std::shared_ptr<Buffer>>` managed by FileManager |
| INI files | `QSettings` (JSON or INI backend) |
| `GetPrivateProfileString` | `QSettings::value` |
| Win32 file APIs | `QFile`, `QTextStream`, `QFileDevice` |
| `ShellExecute` | `QDesktopServices::openUrl` |
| `CreateFile` | `QFile` with `QFileDevice::OpenMode` |
| `FindFiles` | `QDir` + `QDirIterator` |
| Win32 registry | `QSettings` user config path |

The Scintilla editing engine is provided by **QScintilla** (`QsciScintilla`), a Qt port of the same Scintilla component used by Notepad++.
