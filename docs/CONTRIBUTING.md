# Contributing to Notepad--Qt

Thank you for your interest in contributing! This document covers coding standards, build setup, testing, and the code-review process.

---

## Getting Started

### Prerequisites

| Dependency | Version | Notes |
|------------|---------|-------|
| Qt6 | ≥ 6.4 | Widgets, Gui, Core, Xml, PrintSupport |
| QScintilla2 | 6 | `libqscintilla2-qt6` |
| CMake | ≥ 3.20 | |
| C++ compiler | C++20 | GCC 11+, Clang 14+ |
| Catch2 | (header-only) | Bundled in `src/tests/` |
| Qt Test | ≥ 6.4 | For GUI tests |

On Arch Linux:
```bash
sudo pacman -S qt6-base qscintilla cmake base-devel
```

On Ubuntu/Debian:
```bash
sudo apt install qt6-base-dev libqscintilla2-qt6-dev cmake build-essential
```

### Build

```bash
# Clone
git clone https://github.com/piotroxp/npp-qt.git
cd npp-qt

# Configure (single-config Ninja or multi-config Make)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Run
./build/NotepadMinusMinusQt
```

For a debug build with sanitizers:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DNPP_DEBUG_SANITIZERS=ON
cmake --build build -j$(nproc)
```

### Build with Qt Creator

1. Open `CMakeLists.txt` as a project
2. Select Qt 6 kit
3. Configure → Build

### Run Tests

```bash
# Build tests first
cmake --build build --target all -j$(nproc)

# Run all tests
ctest --test-dir build --output-on-failure

# Run a specific test
ctest --test-dir build -R test_buffer --output-on-failure

# Run with verbose output
ctest --test-dir build -V
```

---

## Coding Standards

### Language & Style

- **C++20** — use concepts, `std::span`, `std::ranges` where appropriate
- **Qt conventions** — use `QObject` + signals/slots for reactive components; prefer `QString` over `std::string` for UI-facing code
- **No raw `new`/`delete`** — use `std::unique_ptr`, `std::shared_ptr`, or Qt parent-child ownership
- **No implicit sharing** — be aware that `QString`, `QByteArray` are implicitly shared; copy-on-write is fine
- **No `using namespace std`** in headers
- **Const-correctness** — mark member functions `const` where they do not mutate observable state

### Naming Conventions

| Construct | Convention | Example |
|-----------|-----------|---------|
| Classes | PascalCase | `ScintillaEditor`, `FileManager` |
| Structs | PascalCase | `BufferViewInfo`, `FindResult` |
| Member variables | `_camelCase` | `_activeBuffer`, `_mainWindow` |
| Methods / functions | camelCase | `openFile()`, `detectEncoding()` |
| Enums / enum values | PascalCase | `EncodingType::UTF_8_BOM` |
| Constants | `SCREAMING_SNAKE` | `MAX_CLIPBOARD_HISTORY` |
| Header guards | `PATH_FILENAME_H` | `CORE_APPLICATION_H` |
| Slot functions | `onEvent` | `onBufferOpened()`, `onTabCloseRequested()` |
| Signal functions | past/present verb | `bufferOpened`, `fileSaved` |

### File Organization

- **One class per header** — `Foo.h` declares class `Foo`; `Foo.cpp` implements it
- **Include order** (within each .cpp):
  1. Corresponding header (e.g., `"Application.h"`)
  2. Qt headers (`<QApplication>`, `<QString>`)
  3. Standard library (`<vector>`, `<algorithm>`)
  4. Project headers (`"common/Util.h"`, `"editor/ScintillaEditor.h"`)
- **Private implementation** — use the PIMPL pattern or `d->` pointer for binary stability when the header is exposed
- **No cyclic includes** — forward-declare where possible; use `// forward class Foo;`

### Signal / Slot Guidelines

- Signals are always `void` — they carry data as parameters
- Slots are named `onSubjectAction()` — e.g., `onBufferOpened()`
- Prefer `QObject::connect()` with `Qt::AutoConnection` (the default) unless you need explicit queued/直连 connections
- For cross-thread signaling, use `Qt::QueuedConnection` explicitly
- Prefer lambdas for one-shot connections; prefer member slots for repeated/reconnectable signals

### Error Handling

- Use `Q_UNREACHABLE()` for cases that should never occur (after `assert`-style checks)
- Use `logError()` / `logWarning()` from `Util.h` for user-visible or recoverable errors
- Use `qWarning()` / `qCritical()` for Qt-level diagnostics
- Never `throw` across module boundaries — catch at plugin/API boundaries

---

## Code Review Checklist

Before submitting a PR, verify:

- [ ] **Compiles** — `cmake --build build` succeeds with no errors or warnings
- [ ] **Tests pass** — `ctest --test-dir build --output-on-failure`
- [ ] **No new compiler warnings** — check the build output
- [ ] **No memory leaks** — run with `valgrind --leak-check=full ./build/NotepadMinusMinusQt` if you modified memory-management code
- [ ] **UI updates on tab switch** — test opening multiple files, switching tabs, editing, saving
- [ ] **Encoding roundtrip** — test opening a UTF-8 file, saving, re-opening
- [ ] **Panel toggles** — test View → Document Map, Function List, File Browser, Clipboard History
- [ ] **Theme switching** — if you modified any UI code, test dark/light mode
- [ ] **New API** — if you added a new public method to `Application` or any manager, document it in this ARCHITECTURE.md

---

## Adding a New Language Lexer

1. Add the `LangType` entry in `src/common/Types.h`:
   ```cpp
   L_MYLANG = N,  // add new entry in the enum
   ```
2. Add `setupMyLang()` method to `src/editor/SyntaxHighlighter.h/.cpp`:
   ```cpp
   void setupMyLang(QsciLexer* lexer, const QString& themeName);
   ```
3. Wire it in `LexerConfig::getLexer(LangType)`:
   ```cpp
   case LangType::L_MYLANG: return new QsciLexerMyLang(this);
   ```
4. Wire it in `SyntaxHighlighter::setLanguage(LangType)`:
   ```cpp
   case LangType::L_MYLANG: setupMyLang(lexer, theme); break;
   ```
5. Add `setupMyLang()` implementation in `SyntaxHighlighter.cpp` with keywords, comment markers, operators
6. Add `parseMyLang()` to `FunctionListPanel` if you want function-list support
7. Add tests in `src/tests/test_lexer.cpp` (or `tests/TestLexer.cpp`)
8. Update `docs/ARCHITECTURE.md` if you add a new panel or major component

---

## Adding a New Dialog or Panel

1. Place the header in `src/dialogs/` or `src/panels/`
2. Include `src/common/NonCopyable.h` if the class should not be copyable
3. Inherit from `QDialog` (dialogs) or `QDockWidget` (panels)
4. Parent the widget to `MainWindow*` in the constructor
5. Register the dialog in `Application::initialize()`:
   ```cpp
   _myDialog = new MyDialog(_mainWindow);
   ```
6. Connect dialog signals to `Application` slots in `setupConnections()`
7. Add `Q_OBJECT` macro to the class declaration for signal/slot support
8. Add unit tests for any non-GUI logic (e.g., parsing helpers)

---

## Adding a New Editor Command

1. Add the command ID in `src/menuCmdID.h` (or as an enum in the appropriate class)
2. Register the command in `EditorCommandManager::registerAll()`:
   ```cpp
   registerCommand(IDM_FILE_NEW, "File.New", &Application::onNewFile);
   ```
3. If the command is complex, create a dedicated method in `Application.cpp`:
   ```cpp
   void Application::onMyCommand() { ... }
   ```
4. Add the keyboard shortcut mapping in `ShortcutManager` or `Parameters.cpp`
5. Add a menu item in `MenuBar.cpp` or `createMenus()` in `MainWindow.cpp`

---

## Performance Notes

- **Large files** (>5 MB) are streamed via `FileLoaderWorker` in background threads — never block the UI thread
- **Function list parsing** is debounced (200 ms) in `FunctionListPanel` to avoid re-parsing on every keystroke
- **Document map updates** use a `QTimer` (16 ms / 60 fps) for smooth scroll sync
- **Lexing** is done asynchronously by QScintilla; call `lexer->setDocument(nullptr)` before destroying a lexer to avoid double-free
- **Buffer references** — split-view editors share the same `Buffer*`; never delete a `Buffer` while any `ScintillaEditor*` still references it

---

## Dependency Graph (Simplified)

```
main.cpp
  └─ Application (singleton)
       ├─ MainWindow
       │    ├─ MenuBar
       │    ├─ ToolBar
       │    ├─ StatusBar
       │    ├─ QTabWidget (DocTabView)
       │    │    └─ ScintillaEditor[] ←→ Buffer[]
       │    └─ QDockWidget[]
       │         ├─ FileBrowserPanel
       │         ├─ FunctionListPanel
       │         ├─ DocumentMapPanel
       │         └─ ClipboardHistoryPanel
       ├─ FileManager (singleton)
       │    ├─ Buffer[]   (document state)
       │    └─ FileLoaderWorker (QThread)
       ├─ EncodingDetector (singleton)
       ├─ LanguageManager (singleton)
       ├─ EditorCommandManager
       ├─ PluginManager (singleton)
       │    └─ NppQtFuncs (function table)
       ├─ SessionManager
       ├─ RecentFilesManager
       ├─ MacroManager
       ├─ UdlManager
       ├─ ThemeManager
       ├─ ShortcutManager
       └─ Dialogs[] (FindReplaceDialog, PreferenceDialog, etc.)
```

---

## Commit Message Format

```
<type>(<scope>): <short description>

<optional body>

<optional footer>
```

Types: `feat`, `fix`, `docs`, `refactor`, `test`, `chore`, `perf`

Examples:
```
feat(editor): add column selection mode
fix(encoding): correct UTF-8 BOM insertion order
docs(arch): describe buffer/editor model
test(buffer): add encoding roundtrip tests
refactor(FunctionListPanel): use lazy XML parser loading
```
