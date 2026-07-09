# Notepad--Qt

A faithful Qt/C++ reimplementation of Notepad++ for Linux, built from a semantic lift of the original C++ codebase into a modern Qt6 architecture.

> **Note:** This project is derived from the Notepad++ (GPL v3) source. The original upstream is [notepad-plus-plus/notepad-plus-plus](https://github.com/notepad-plus-plus/notepad-plus-plus).

---

## Features

- **Multi-tab editing** with Scintilla (QsciScintilla) text widget
- **Syntax highlighting** for 100+ languages via Lexer plugins
- **Encoding support** — UTF-8, UTF-16, ANSI, and 40+ legacy encodings
- **Session management** — save/restore open files, window positions, active tab
- **Find & Replace** — regex, whole-word, in-selection, multi-file (grep-style)
- **File browser** panel with live filesystem watching
- **Function list** panel — parsed from source files (C, Python, JS, and more)
- **Document map** minimap with scroll synchronization
- **Preferences** — 7 categories: General, Editing, Language, Search, Backup, Print, Advanced
- **Toolbar & MenuBar** — full keyboard shortcut support
- **Plugin architecture** — stubs in place for NppPlugin interface

---

## Building

### Prerequisites

```bash
Qt 6.4+ (with QtWidgets, QtGui, QtCore, QtNetwork, QtXml)
Qsci 3.x (QScintilla)
CMake 3.18+
C++20 compiler (GCC 11+ or Clang 14+)
```

### Build steps

```bash
# Clone
git clone https://github.com/your-fork/notepad-plus-plus-qt.git
cd notepad-plus-plus-qt

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Build
cmake --build build -j$(nproc)

# Run
./build/npp-qt
```

### Qt Creator

Open `CMakeLists.txt` in Qt Creator and configure with the Qt 6 kit.

---

## Project Structure

```
notepad-plus-plus-qt/
├── src/
│   ├── common/          # Utilities: strings, files, types, constants
│   ├── core/            # Application, Buffer, FileManager, Encoding, Language, Session
│   ├── dialogs/         # FindReplace, GoToLine, Preference, About, RunDlg
│   ├── editor/          # ScintillaEditor wrapper, SyntaxHighlighter
│   ├── panels/          # FunctionList, FileBrowser, DocumentMap
│   ├── ui/              # MainWindow, MenuBar, ToolBar, TabBar, StatusBar
│   └── tests/           # Unit tests (Catch2)
├── docs/                # Architecture, changelog
└── build/               # Build output (not in git)
```

---

## Architecture

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the full technical overview.

---

## Testing

```bash
cd build
ctest --output-on-failure
```

---

## Semantic Lift Status

This codebase was produced by a semantic lift — translating the original Notepad++ C++/Win32 API to Qt6/C++. The following major modules have been lifted:

| Module | Status |
|--------|--------|
| Core Application | ✅ Complete |
| UI (MainWindow, MenuBar, ToolBar, TabBar, StatusBar) | ✅ Complete |
| Buffer & FileManager | ✅ Complete |
| EncodingDetector & LanguageManager | ✅ Complete |
| SessionManager | ✅ Complete |
| Panels (FunctionList, FileBrowser, DocumentMap) | ✅ Complete |
| Find & Replace dialogs | ✅ Complete |
| Preference dialog | ✅ Complete |
| Parameters / Settings | 🔶 In progress |
| NppIO / File I/O | 🔶 In progress |
| NppCommands / NppBigSwitch | 🔶 In progress |
| Notepad_plus (orchestrator) | 🔶 In progress |
| WinControls (Win32→Qt widgets) | 🔶 Planned |
| ScintillaComponent | 🔶 Planned |
| Plugin system | 🔶 Planned |

---

## License

This project is GPL v3, like Notepad++. See [LICENSE](LICENSE).
