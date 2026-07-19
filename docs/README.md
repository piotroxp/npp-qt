# Notepad--Qt Documentation

Comprehensive documentation for Notepad--Qt (NotepadMinusMinusQt), a Qt6/Linux port of Notepad++.

---

## User Documentation

| Document | What it's for |
|----------|--------------|
| [GETTING_STARTED.md](GETTING_STARTED.md) | Installation, building, first run |
| [COMMAND_LINE.md](COMMAND_LINE.md) | All CLI flags, env vars, exit codes, session file format |
| [KEYBOARD_SHORTCUTS.md](KEYBOARD_SHORTCUTS.md) | Complete shortcut reference by category |
| [PANELS.md](PANELS.md) | Document Map, Function List, File Browser, Clipboard History |
| [ENCODING.md](ENCODING.md) | Encoding detection, BOM, conversion, EOL |
| [THEMES.md](THEMES.md) | Built-in themes, switching, custom themes, dark mode |
| [TROUBLESHOOTING.md](TROUBLESHOOTING.md) | Crash diagnosis, build errors, encoding issues, performance |

---

## Developer Documentation

| Document | What it's for |
|----------|--------------|
| [ARCHITECTURE.md](ARCHITECTURE.md) | System design, startup sequence, buffer model, signal flow |
| [API.md](API.md) | Plugin SDK (native + legacy NPP compatibility), lifecycle |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Coding standards, adding languages/dialogs/commands |

---

## Quick Links

### User Quick Reference

```bash
# Install
sudo apt install qt6-base-dev libqscintilla2-qt6-dev cmake build-essential

# Build
cmake -B build && cmake --build build -j$(nproc)

# Run
./build/NotepadMinusMinusQt file.txt

# Run headless (no display)
QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt file.txt

# Run with specific theme
./build/NotepadMinusMinusQt --profile dark file.txt

# Run without plugins
./build/NotepadMinusMinusQt --no-plugins file.txt
```

### Developer Quick Reference

```bash
# Debug crash
gdb ./build/NotepadMinusMinusQt
(gdb) run
(gdb) bt full

# Build with ASAN
cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug -DNPP_DEBUG_SANITIZERS=ON
cmake --build build-asan
./build-asan/NotepadMinusMinusQt

# Run tests
ctest --test-dir build --output-on-failure

# Add a new language lexer
# 1. Add LangType in src/common/Types.h
# 2. Add setupMylang() in src/editor/SyntaxHighlighter.cpp
# 3. Wire in LexerConfig::getLexer() and SyntaxHighlighter::setLanguage()
# 4. See CONTRIBUTING.md
```

---

## Source Map

| Directory | Contents |
|-----------|----------|
| `src/core/` | Application, Buffer, FileManager, EncodingDetector, SessionManager, ThemeManager, ShortcutManager |
| `src/dialogs/` | All modal dialogs (Find/Replace, Preferences, GoTo, etc.) |
| `src/panels/` | Dockable panels (Function List, File Browser, Doc Map, Clipboard History) |
| `src/editor/` | ScintillaEditor, SyntaxHighlighter, DocTabView, WebBrowserView, AutoCompletion |
| `src/ui/` | MainWindow, MenuBar, ToolBar, StatusBar, TabBar |
| `src/plugins/` | PluginManager, PluginInterface.h |
| `src/common/` | Types, Util, StringHelper, FileHelper, Ini |
| `src/workers/` | FileLoaderWorker (async file I/O) |
| `src/MISC/` | EncodingMapper, PluginsManager, ConfigFile, FileManager (NPP compat) |
| `src/uchardet/` | Character encoding detection library |
| `tests/` | Qt Test GUI tests |
| `src/tests/` | Catch2 unit tests |

---

## Key Classes

| Class | File | Purpose |
|-------|------|--------|
| `Application` | `core/Application.h` | Singleton; owns all managers, UI, state machine |
| `MainWindow` | `ui/MainWindow.h` | `QMainWindow`; owns all UI components |
| `Buffer` | `core/Buffer.h` | Document state (encoding, language, dirty, references) |
| `FileManager` | `core/FileManager.h` | Buffer registry; open/close/create buffer |
| `ScintillaEditor` | `editor/ScintillaEditor.h` | `QsciScintilla` wrapper; owns SyntaxHighlighter |
| `SyntaxHighlighter` | `editor/SyntaxHighlighter.h` | 54 language definitions + 7 themes |
| `EncodingDetector` | `core/EncodingDetector.h` | BOM check → UTF-8 validation → chardet |
| `SessionManager` | `core/SessionManager.h` | JSON session save/load |
| `PluginManager` | `plugins/PluginManager.h` | Plugin loading, function table |
| `ShortcutManager` | `core/ShortcutManager.h` | Key binding registry, conflict detection |
| `FunctionListPanel` | `panels/FunctionListPanel.h` | Symbol tree, XML-based parser |
| `DocumentMapPanel` | `panels/DocumentMapPanel.h` | Scintilla minimap, scroll sync |

---

## Getting Help

- **Startup crash?** → [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
- **Building fails?** → [TROUBLESHOOTING.md#build-errors](TROUBLESHOOTING.md#build-errors)
- **Keyboard shortcut not working?** → [KEYBOARD_SHORTCUTS.md](KEYBOARD_SHORTCUTS.md#desktop-environment-conflicts)
- **Encoding problem?** → [ENCODING.md](ENCODING.md)
- **Writing a plugin?** → [API.md](API.md)
- **Adding a language?** → [CONTRIBUTING.md](CONTRIBUTING.md#adding-a-new-language-lexer)
- **Understanding the architecture?** → [ARCHITECTURE.md](ARCHITECTURE.md)
- **Not listed?** → Ask in the project issue tracker
