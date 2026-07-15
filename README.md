# Notepad--Qt

A faithful, native Qt6 port of [Notepad++](https://notepad-plus-plus.org/) for Linux and macOS.

**Build:** `cmake -B build && cmake --build build`  
**Run:** `./build/NotepadMinusMinusQt`  
**Test:** `ctest --test-dir build`

## Features

### Editor Core
- **Multi-tab editing** — per-tab document buffers backed by `Buffer` objects, managed by `FileManager`
- **54 language syntax highlighting** via QScintilla: C/C++, Python, JavaScript, TypeScript, HTML, CSS, JSON, YAML, TOML, XML, Markdown, SQL, Rust, Go, Lua, Ruby, PHP, Perl, Bash, PowerShell, Batch, CMake, Dockerfile, Makefile, Java, Kotlin, Swift, Dart, R, MATLAB, Julia, Haskell, Scala, Clojure, F#, OCaml, Erlang, Pascal, Fortran, Lisp, Verilog, VHDL, ASM, LaTeX, NSIS, Inno Setup, NSIS, Properties, INI, Diff, NFO, Vim, and more
- **Auto-completion** — word and API auto-completion popup
- **Function call tips** — parameter highlighting with overload navigation (↑/↓)
- **Column / Box selection mode** — rectangular selection via Alt+drag
- **Macro recording & playback** — record keystrokes and commands, replay from session
- **Multiple cursors** — Ctrl+Alt+↑/↓ to add cursor lines (QScintilla native)

### Encoding & Files
- **UTF-8, UTF-16 LE/BE, and legacy encodings** (Latin-1, Windows-1252, ISO-8859-2, etc.) with BOM detection and insertion
- **EOL conversion** — CRLF, LF, CR per-buffer
- **Large file support** — files >5 MB trigger a warning dialog with partial-load option; `FileLoaderWorker` streams content in background chunks
- **File change monitoring** — `QFileSystemWatcher` detects external modifications, prompts to reload
- **Auto-save** — configurable interval, persists to backup copies
- **Session persistence** — restore open files, active tab, window geometry, panel visibility on restart

### Search & Navigation
- **Find & Replace** — regex, whole-word, match-case, in-selection, Replace All, Mark results
- **Incremental search** — Ctrl+I inline search bar
- **Go to Line** — by absolute line, relative offset, or percentage
- **Find in Files** — background recursive directory search with progress reporting
- **Command palette** — Ctrl+Shift+P fuzzy search over all registered commands
- **Function list panel** — collapsible tree of functions/classes/structs parsed by language-specific regex
- **Document map panel** — scroll-synced minimap of the active buffer

### UI & Panels
- **Dark / Light mode** — theme switching via `ThemeManager`, applies to all widgets and lexers
- **File browser panel** — recursive directory tree with live `QFileSystemWatcher` updates
- **Clipboard history panel** — persistent clipboard stack with re-paste
- **Web browser view** — HTML/Markdown preview panel with zoom and navigation
- **Shortcut mapper** — bind any key combination to any command
- **User Defined Language (UDL) editor** — define custom syntax highlighting via `UserDefineDialog`
- **Recent files menu** — persisted recent files list
- **Status bar** — cursor position, encoding, EOL mode, language, zoom level
- **Printer / Print to PDF** — via `QPrinter`
- **Column editor** — insert sequential numbers or character sequences column-wise
- **Smart highlighter** — multi-keyword markup for search results
- **ANSI character panel** — insert special characters by code point

## Architecture

- `src/core/` — Application, Buffer, FileManager, EncodingDetector, LanguageManager, SessionManager, MacroManager, NppCommands, NppIO, RecentFilesManager, ShortcutManager, ThemeManager, UdlManager, WorkspaceManager
- `src/editor/` — ScintillaEditor (QsciScintilla wrapper), ScintillaEditView, DocTabView, ScintillaCtrls, LexerConfig, SyntaxHighlighter (54 languages), WebBrowserView
- `src/ui/` — MainWindow, MenuBar, StatusBar, TabBar, ToolBar, WinControls
- `src/dialogs/` — FindReplaceDialog, PreferenceDialog, ShortcutMapperDialog, CommandPaletteDialog, AboutDialog, GoToLineDialog, FunctionCallTip, AutoCompletion, ClipboardHistoryPanel, FindInFilesWorker, Printer, UserDefineDialog, SmartHighlighter, IncrementalSearchDialog, ColumnEditorDialog, LargeFileWarningDialog, FileReloadDialog, AnsiCharPanel
- `src/panels/` — FileBrowserPanel, FunctionListPanel, DocumentMapPanel, ClipboardHistoryPanel
- `src/common/` — Util, StringHelper, FileHelper, DpiManager, Constants, Types, Observer, NonCopyable, PathHelper, UrlHelper
- `src/workers/` — FileLoaderWorker (async file I/O with progress)
- `src/plugins/` — PluginManager, PluginInterface (Notepad++ plugin API headers)
- `src/MISC/Common/` — NppNotification, keys, shortcut

## Known Limitations

- Plugin API: not yet implemented (Notepad++ plugin compatibility is a future goal)
- Windows/macOS: build and runtime not yet tested on those platforms
- Some language lexers may be unavailable depending on QScintilla version

## Building

```bash
git clone https://github.com/piotroxp/npp-qt.git
cd npp-qt
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

## Testing

```bash
ctest --test-dir build --output-on-failure
```

## Contributing

This project was generated via a **semantic lift** from Notepad++ (Win32/C++ → Qt6/C++). The translation approach is:

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

## License

GPL v3 — same as Notepad++
