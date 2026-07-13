# Contributing to Notepad--Qt

## Building

```bash
git clone https://github.com/piotroxp/npp-qt.git
cd npp-qt
cmake -B build
cmake --build build -j$(nproc)
```

## Running

```bash
./build/NotepadMinusMinusQt
```

## Testing

```bash
ctest --test-dir build --output-on-failure
```

## Code Style

- Qt naming conventions: `camelCase` for methods, `PascalCase` for classes
- Use `nullptr` instead of `NULL` or `0`
- Prefer `QString::fromStdString()` over `QString::fromLatin1()` for std::string
- Use `std::optional<std::string>` for optional string returns
- Always check `BufferID` for validity: `if (!buf) return;`
- Use `qDebug()` for debug output, not `printf` or `std::cout`
- New source files must be added to CMakeLists.txt

## Submitting Changes

1. Branch from `master`
2. Run `cmake --build build -j$(nproc)` — must be clean
3. Commit with a clear message describing what changed
4. Push and open a PR

## Architecture Overview

- `src/core/` — Application, Buffer, FileManager, LanguageManager, EncodingDetector, SessionManager, MacroManager
- `src/editor/` — ScintillaEditor (QsciScintilla wrapper)
- `src/ui/` — MainWindow, MenuBar, StatusBar, TabBar
- `src/dialogs/` — FindReplaceDialog, FindInFilesDialog, PreferenceDialog, etc.
- `src/panels/` — FileBrowserPanel, FunctionListPanel, DocumentMapPanel
- `src/common/` — Util, StringHelper, FileHelper
- `src/plugins/` — PluginManager, PluginInterface
