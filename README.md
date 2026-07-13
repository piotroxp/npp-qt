# Notepad--Qt

A faithful, native Qt6 port of [Notepad++](https://notepad-plus-plus.org/) for Linux and macOS.

**Build:** `cmake -B build && cmake --build build`  
**Run:** `./build/NotepadMinusMinusQt`  
**Test:** `ctest --test-dir build`

## Features

- Multi-tab editing with per-tab document buffers
- Full syntax highlighting via QScintilla (C/C++, Python, JavaScript, HTML, JSON, and 20+ languages)
- UTF-8, UTF-16 LE/BE, and legacy encodings with BOM handling
- EOL conversion (CRLF, LF, CR)
- Find & Replace with regex, whole-word, match-case, and Replace All
- Session persistence — restore open files on restart
- Auto-save (optional, configurable interval)
- Macro recording and playback
- Clipboard history
- Column/Box selection mode
- Dark mode / Light mode theming
- Keyboard shortcut mapper
- Command palette (Ctrl+Shift+P)
- File browser panel, Function list panel, Document map panel
- Recent files menu
- File change monitoring (external modifications detected)

## Architecture

- `src/core/` — Application, Buffer, FileManager, EncodingDetector, LanguageManager, SessionManager, MacroManager
- `src/editor/` — ScintillaEditor (QsciScintilla wrapper), SyntaxHighlighter
- `src/ui/` — MainWindow, MenuBar, StatusBar, TabBar, ToolBar
- `src/dialogs/` — FindReplaceDialog, PreferenceDialog, ShortcutMapperDialog, CommandPaletteDialog, AboutDialog
- `src/panels/` — FileBrowserPanel, FunctionListPanel, DocumentMapPanel
- `src/common/` — Util, StringHelper, FileHelper, DpiManager, Constants, Types

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

## License

GPL v3 — same as Notepad++
