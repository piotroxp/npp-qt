# Notepad--Qt

A faithful, native **Qt6** port of [Notepad++](https://notepad-plus-plus.org/) for Linux, macOS, and Windows.

**Binary:** `./build/NotepadMinusMinusQt` (or `npp-qt` on some builds)  
**Test:** `ctest --test-dir build --output-on-failure`

---

## Features

### Editor Core
- **Multi-tab editing** — per-tab document buffers backed by `Buffer` objects, managed by `FileManager`
- **54 language syntax highlighting** via QScintilla: C, C++, Python, JavaScript, TypeScript, HTML, CSS, JSON, YAML, TOML, XML, Markdown, SQL, Rust, Go, Lua, Ruby, PHP, Perl, Bash, PowerShell, Batch, CMake, Dockerfile, Makefile, Java, Kotlin, Swift, Dart, R, MATLAB, Julia, Haskell, Scala, Clojure, F#, OCaml, Erlang, Pascal, Fortran, Lisp, Verilog, VHDL, ASM, LaTeX, NSIS, Inno Setup, Properties, INI, Diff, NFO, Vim, and more
- **Auto-completion** — word and API auto-completion popup
- **Function call tips** — parameter highlighting with overload navigation (↑/↓)
- **Column / Box selection mode** — rectangular selection via Alt+drag
- **Macro recording & playback** — record keystrokes and commands, replay from session
- **Multiple cursors** — Ctrl+Alt+↑/↓ to add cursor lines (QScintilla native)
- **Code folding** — fold/unfold blocks across all supported languages

### Encoding & Files
- **UTF-8, UTF-16 LE/BE, UTF-32, and legacy encodings** (Latin-1, Windows-1252, ISO-8859-2, etc.) with BOM detection and insertion
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
- **Function list panel** — collapsible tree of functions/classes/structs parsed by language-specific regex (also uses upstream Notepad++ `functionList.xml` parsers)
- **Document map panel** — scroll-synced minimap of the active buffer

### UI & Panels
- **Dark / Light mode** — theme switching via `ThemeManager`, applies to all widgets and lexers
- **File browser panel** — recursive directory tree with live `QFileSystemWatcher` updates
- **Clipboard history panel** — persistent clipboard stack with re-paste
- **Web browser view** — HTML/Markdown preview panel with zoom and navigation (requires Qt WebEngine)
- **Shortcut mapper** — bind any key combination to any command
- **User Defined Language (UDL) editor** — define custom syntax highlighting via `UserDefineDialog`
- **Recent files menu** — persisted recent files list
- **Status bar** — cursor position, encoding, EOL mode, language, zoom level
- **Printer / Print to PDF** — via `QPrinter`
- **Column editor** — insert sequential numbers or character sequences column-wise
- **Smart highlighter** — multi-keyword markup for search results
- **ANSI character panel** — insert special characters by code point
- **Bookmarks** — toggle, navigate next/prev, clear bookmarks per buffer
- **Tab color indicators** — color-code tabs for grouping

### Plugins
- **Plugin API** (`src/plugins/PluginInterface.h`) — `NppQtFuncs` function table for plugin→editor communication
- **Legacy Notepad++ compatibility** — `FuncItem`, `NppData`, `npp_getFunctionItems`/`npp_setInfo` stubs for porting existing NPP plugins
- Plugins are loaded from `~/.config/notepad--qt/plugins/` (Linux) via `QLibrary`

---

## Quick Start

```bash
git clone https://github.com/piotroxp/npp-qt.git
cd npp-qt

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Run
./build/NotepadMinusMinusQt

# Run tests
ctest --test-dir build --output-on-failure
```

**Requirements:**
- Qt 6.4+ (Widgets, Gui, Core, Xml, PrintSupport)
- QScintilla 6 (`libqscintilla2-qt6`)
- CMake 3.20+
- C++20 compiler (GCC 11+, Clang 14+)

On Arch Linux: `pacman -S qt6-base qscintilla cmake`

---

## Directory Layout

```
npp-qt/
├── CMakeLists.txt            # Top-level build (NotepadMinusMinusQt target)
├── src/
│   ├── main.cpp              # Entry point
│   ├── core/                  # Application, Buffer, FileManager, EncodingDetector,
│   │                          # LanguageManager, SessionManager, MacroManager,
│   │                          # NppCommands, NppIO, RecentFilesManager,
│   │                          # ShortcutManager, ThemeManager, UdlManager, WorkspaceManager
│   ├── editor/                # ScintillaEditor (QsciScintilla wrapper), ScintillaEditView,
│   │                          # DocTabView, ScintillaCtrls, LexerConfig, SyntaxHighlighter,
│   │                          # WebBrowserView, IEditor
│   ├── ui/                    # MainWindow, MenuBar, StatusBar, TabBar, ToolBar,
│   │                          # ColourPicker, ColourPopup, WinControls
│   ├── dialogs/               # FindReplaceDialog, PreferenceDialog, ShortcutMapperDialog,
│   │                          # CommandPaletteDialog, AboutDialog, GoToLineDialog,
│   │                          # FunctionCallTip, AutoCompletion, ClipboardHistoryPanel,
│   │                          # FindInFilesDialog, FindInFilesWorker, Printer,
│   │                          # UserDefineDialog, SmartHighlighter,
│   │                          # IncrementalSearchDialog, ColumnEditorDialog,
│   │                          # LargeFileWarningDialog, FileReloadDialog, AnsiCharPanel
│   ├── panels/                # FileBrowserPanel, FunctionListPanel,
│   │                          # FunctionListXmlParser, DocumentMapPanel
│   ├── workers/               # FileLoaderWorker (async file I/O with progress)
│   ├── plugins/               # PluginManager, PluginInterface
│   ├── common/                # Util, StringHelper, FileHelper, DpiManager, Constants,
│   │                          # Types, Observer, NonCopyable, PathHelper, UrlHelper,
│   │                          # ScintillaComponent
│   ├── MISC/Common/           # NppNotification, keys, shortcut
│   ├── uchardet/              # Mozilla universal charset detector
│   └── tests/                 # Catch2 unit tests (test_buffer, test_encoding_detector, etc.)
├── tests/                     # Integration/GUI tests (Qt Test)
├── docs/                      # Architecture and contributing docs
│   ├── ARCHITECTURE.md
│   └── CONTRIBUTING.md
├── rgba_icons/                # Icon assets
└── themes/                    # Color themes
```

---

## Architecture

See [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) for a detailed description of the startup sequence, buffer/editor model, plugin system, panel system, encoding system, and signal flow.

---

## Known Limitations

- **Plugin API**: The `NppQtFuncs` function table and plugin loader are implemented, but no production plugins are bundled yet. Notepad++ plugin compatibility is a future goal.
- **Windows/macOS**: Build is configured for Linux primary; Windows and macOS builds are experimental.
- **Some language lexers** may be unavailable depending on the installed QScintilla version.
- **Qt WebEngine** is optional — HTML/Markdown preview requires it at build time.

---

## Semantic Lift Notes

This codebase was generated via a **semantic lift** from Notepad++ (Win32/C++ → Qt6/C++). The translation approach:

| Original (Win32) | Target (Qt) |
|-----------------|------------|
| WinMain / HWND | `QApplication` / `QMainWindow` |
| `SendMessage(hwnd, SCI_*)` | `QsciScintilla::SendScintilla(QsciScintilla::Command, long)` |
| `HDC` / GDI drawing | Qt paint system (`QPainter`) |
| `std::vector<Buffer>` | `std::vector<Buffer*>` managed by `FileManager` |
| INI files | `QSettings` (JSON or INI backend) |
| `GetPrivateProfileString` | `QSettings::value` |
| Win32 file APIs | `QFile`, `QTextStream`, `QFileDevice` |
| `ShellExecute` | `QDesktopServices::openUrl` |
| `CreateFile` | `QFile` with `QFileDevice::OpenMode` |
| `FindFiles` | `QDir` + `QDirIterator` |
| Win32 registry | `QSettings` user config path |

The Scintilla editing engine is provided by **QScintilla** (`QsciScintilla`), a Qt port of the same Scintilla component used by Notepad++.

---

## Contributing

See [`docs/CONTRIBUTING.md`](docs/CONTRIBUTING.md) for coding standards, build instructions, testing guidelines, and the code-review process.

---

## License

GPL v3 — same as Notepad++
