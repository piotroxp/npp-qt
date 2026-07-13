# Changelog

## [Unreleased]

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
