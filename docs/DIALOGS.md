# Dialogs Reference

Reference for every modal and modeless dialog in Notepad--Qt. Each entry covers the dialog's purpose, key signals/slots, and implementation notes.

---

## Quick Reference Table

| Dialog | Class | Type | Lines | Notes |
|--------|-------|------|-------|-------|
| Find/Replace | `FindReplaceDialog` | Modeless | 942 | Full find/replace with regex, wrap, direction |
| Preference | `PreferenceDialog` | Modal | 1470 | Settings organized by category |
| User Define (UDL) | `UserDefineDialog` | Modal | 1185 | User-defined language editor |
| About | `AboutDialog` | Modal | 623 | Version, credits, license |
| Function Call Tip | `FunctionCallTip` | Modeless | 841 | Parameter hints popup |
| Find In Files | `FindInFilesDialog` | Modal | 496 | Multi-file search with worker |
| Find Chars In Range | `FindCharsInRangeDialog` | Modal | 527 | Character range search |
| Shortcut Mapper | `ShortcutMapperDialog` | Modal | 390 | Remap keyboard shortcuts |
| Command Palette | `CommandPaletteDialog` | Modeless | 385 | Fuzzy command search |
| Clipboard History | `ClipboardHistoryPanel` | Dockable | 753 | Panel (see [PANELS.md](PANELS.md)) |
| Smart Highlighter | `SmartHighlighter` | Modal | 193 | Word highlight persistence |
| Go To Line | `GoToLineDialog` | Modal | 358 | Jump to line/offset |
| Incremental Search | `IncrementalSearchDialog` | Modeless | 227 | Quick search bar |
| Printer | `Printer` | Modal | 288 | Print preview and settings |
| Column Editor | `ColumnEditorDialog` | Modal | 210 | Insert columnar text |
| Auto Completion | `AutoCompletion` | Modeless | 620 | Word/list/path completion |
| Large File Warning | `LargeFileWarningDialog` | Modal | 152 | Partial-load confirmation |
| Ansi Char Panel | `AnsiCharPanel` | Dockable | 256 | Insert special ANSI characters |
| File Reload | `FileReloadDialog` | Modal | 77 | Reload-changed-file prompt |

---

## Find / Replace (`FindReplaceDialog`)

The central search dialog. Modeless — it stays open while you edit.

**File:** `src/dialogs/FindReplaceDialog.cpp` (942 lines)
**Header:** `src/dialogs/FindReplaceDialog.h`

### Signals

| Signal | Payload | Description |
|--------|---------|-------------|
| `findNext(QString, FindFlags)` | text + flags | User pressed Find Next |
| `replace(QString, QString, ReplaceFlags)` | find, replace, flags | Replace one |
| `replaceAll(QString, QString, ReplaceFlags)` | find, replace, flags | Replace all |
| `findInFiles(QString, QString, FIFlags)` | dir + pattern | Find in files started |
| `searchClosed()` | — | Dialog closed |

### Slots

| Slot | Description |
|------|-------------|
| `findNext()` | Find the next occurrence |
| `replaceNext()` | Replace current and find next |
| `replaceAll()` | Replace all occurrences |
| `markAll()` | Highlight all matches |
| `clearMarks()` | Remove all highlights |
| `onOptionsChanged()` | Flags changed — update UI state |

### Flags

```cpp
enum FindFlags {
    FIND_MATCH_CASE   = 1 << 0,   // Case-sensitive
    FIND_WHOLE_WORD   = 1 << 1,   // Whole word only
    FIND_REGEXP       = 1 << 2,   // Regular expression
    FIND_WRAP         = 1 << 3,   // Wrap around end of file
    FIND_BACKWARD     = 1 << 4,   // Search backwards
    FIND_IN_SELECTION = 1 << 5,   // Search within selection only
    FIND_PURGE_BK     = 1 << 6,   // Purge bookmarks before marking
    FIND_ALL_LOOK_IN_HIDDEN = 1 << 7,  // Include hidden files
};
```

### How It Works

1. Dialog is modeless — created once, `show()`/`hide()` toggled via `Ctrl+F`/`Ctrl+H`
2. `onFindNextRequested()` is connected to the Scintilla editor's search mechanism
3. Searches use `SCI_SEARCHINTARGET` (Scintilla's in-buffer search)
4. Marked lines use `SCI_MARKERADD` with `MARKER_MARGIN`
5. Find history is stored in `config.json` under `findHistory`

---

## Find In Files (`FindInFilesDialog`)

**File:** `src/dialogs/FindInFilesDialog.cpp` (496 lines)
**Worker:** `src/dialogs/FindInFilesWorker.cpp` (127 lines)

### Flags

```cpp
enum FIFlags {
    FIF_RECURSIVE       = 1 << 0,  // Descend into subdirectories
    FIF_INHIDDEN        = 1 << 1,  // Include hidden files
    FIF_WHOLE_WORD      = 1 << 2,  // Match whole words
    FIF_MATCH_CASE      = 1 << 3,  // Case-sensitive
    FIF_REGEXP          = 1 << 4,  // Regular expression
    FIF_FILTER          = 1 << 5,  // Use "filters" field (e.g. *.cpp)
    FIF_NO_FOLLOW_LNK   = 1 << 6,  // Do not follow symlinks
};
```

### Worker Architecture

`FindInFilesWorker` runs on a `QThread` pool thread:
```
Dialog start
  → create FindInFilesWorker(path, pattern, flags)
  → QThreadPool::start(worker)
  → worker scans directory tree
  → for each file matching filter:
       read file → search for pattern
       emit resultReady(file, line, column, lineText)
  → emit searchFinished(count)
  → Results displayed in a tree: file → match lines
```

---

## Preference Dialog (`PreferenceDialog`)

The settings center. Organized into pages, each backed by a preference data model.

**File:** `src/dialogs/PreferenceDialog.cpp` (1470 lines)

### Pages (many are stubs — see HEARTBEAT)

| Page | Status | Notes |
|------|--------|-------|
| General | ✅ Working | New document settings, default language, default encoding |
| Editing | ⚠️ Partial | Tab settings, line number margin |
| MISC → Settings | ⚠️ Partial | Various NPP settings |
| Auto-completion | ⚠️ Partial | Word completion, function completion |
| Multi-Instance | ⚠️ Partial | |
| Recent Files History | ⚠️ Partial | |
| Default Directory | ⚠️ Partial | |
| File Association | ⚠️ Partial | |
| Printer | ❌ Stub | |
| Backup | ❌ Stub | |
| Auto-detection | ❌ Stub | |
| Language | ❌ Stub | |
| Highlighting | ❌ Stub | |
| Margins / Border | ❌ Stub | |
| Search Engine | ❌ Stub | |

### Saving Preferences

Preferences are stored in `~/.config/notepad--qt/config.json`. On Apply:
1. Each page validates its inputs
2. `Parameters::save()` serializes to JSON
3. Signal `preferencesChanged()` fires
4. Subsystems (editors, panels, menu items) re-read their config

---

## User Define Language (`UserDefineDialog`)

Full UDL editor for creating custom syntax highlighting without touching lexer source code.

**File:** `src/dialogs/UserDefineDialog.cpp` (1185 lines)
**Manager:** `src/core/UdlManager.cpp`

### UDL Properties

| Tab | Purpose |
|-----|---------|
| Keywords | Lists of keywords (up to 3 sets with different colors) |
| Numbers | Number regex pattern |
| Strings | String delimiters (single, double, escape sequences) |
| Comments | Line comments (`//`, `#`) and block comments (`/* */`) |
| Operators | Characters treated as operators |
| Folding | Fold marker keywords, indentation-based folding |
| Stylers | Per-element foreground/background/bold/italic |

### How UDL Works

1. User configures styles in the dialog
2. On OK, `UdlManager::save(userLang)` serializes to `~/.config/notepad--qt/udl/<name>.udl`
3. `LexerConfig::getLexer("udl:<name>")` returns a Scintilla lexer ID
4. `ScintillaEditor::applyUdlStyling(name)` applies per-element styles via `SCI_STYLESET*` messages

---

## Function Call Tip (`FunctionCallTip`)

Displays function parameter hints as you type (similar to IntelliSense).

**File:** `src/dialogs/FunctionCallTip.cpp` (841 lines)

### Trigger

Fires when `(` is typed in a supported language. Calls `SciFuncTip::showTip(line, pos)` which displays a tooltip window below the cursor.

### Behavior

- Shows function signature: `int printf(const char* format, ...)`
- Highlights the current parameter in bold
- `Tab` moves to next parameter
- Closing `)` hides the tip
- Auto-hides after 10 seconds of inactivity

### Supported Languages

C, C++, JavaScript, Python, PHP, Java — function signatures are hard-coded in `SciFuncTip`.

---

## Go To Line (`GoToLineDialog`)

Jump to a specific line number or character offset.

**File:** `src/dialogs/GoToLineDialog.cpp` (358 lines)

### Features

- Line number input (1-indexed display, 0-indexed internally)
- Character offset mode (e.g. `+1234` from start, `-567` from end)
- Shows total line count in the label
- Guards against out-of-range input (`event()` handler, `setVisible()` re-entrancy lock)

```cpp
void GoToLineDialog::goTo(int line) {
    if (!_editor) return;
    int maxLine = _editor->lines();
    line = qBound(0, line, maxLine - 1);
    _editor->setCursorPosition(line, 0);
    _editor->ensureLineVisible(line);
}
```

---

## Incremental Search (`IncrementalSearchDialog`)

A minimal floating search bar — appears below the toolbar, searches as you type.

**File:** `src/dialogs/IncrementalSearchDialog.cpp` (227 lines)

### Behavior

1. Opens with `Ctrl+Shift+I` or from the Search menu
2. Text typed immediately triggers `findNext(text)` — no Enter required
3. `F3` / `Shift+F3` navigates results
4. Matching text is highlighted with `SC_MARK_UNDERLINE` in the editor
5. `Escape` hides the dialog
6. Spot markers (`gotoNextSpot`, `gotoPrevSpot`) are not yet wired

---

## Smart Highlighter (`SmartHighlighter`)

Persist word highlights across sessions — marks all occurrences of selected words.

**File:** `src/dialogs/SmartHighlighter.cpp` (193 lines)

### Features

- Up to 3 simultaneous highlight words (colored: yellow, green, cyan)
- Highlights visible in all open documents
- List of highlight words persisted in `config.json`
- **Not yet wired:** re-apply highlights on file open

### Limitations

Highlights are lost when the file is closed and reopened. See [HEARTBEAT.md](../HEARTBEAT.md).

---

## Command Palette (`CommandPaletteDialog`)

Fuzzy-searchable list of all Notepad--Qt commands.

**File:** `src/dialogs/CommandPaletteDialog.cpp` (385 lines)

### Features

- Opens with `Ctrl+Shift+P`
- Fuzzy search: `buf` matches "Buffer — New", `fp` matches "File — Print"
- Commands include: menu items, editor commands, plugin commands
- `Enter` executes the selected command
- `↑`/`↓` navigates results
- Shows keyboard shortcut next to each command

### Implementation

- `EditorCommandManager::getAllCommands()` returns the command list
- `QSortFilterProxyModel` does fuzzy matching
- Commands are `QAction` objects from the menu bar

---

## Shortcut Mapper (`ShortcutMapperDialog`)

UI for remapping keyboard shortcuts.

**File:** `src/dialogs/ShortcutMapperDialog.cpp` (390 lines)

### Structure

Three columns:
1. **Category** — Files, Edit, Search, View, Encoding, Language, Settings, Tools, Macro, Run, Window, Plugins
2. **Shortcut** — Current key binding for each command in the category
3. **New Shortcut** — Override via `ShortcutMapper` widget

### How Shortcuts are Stored

```cpp
// ~/.config/notepad--qt/shortcuts.json
{
  "shortcuts": [
    { "command": "IDM_FILE_NEW", "shortcut": "Ctrl+N", "category": "File" },
    { "command": "IDM_EDIT_UNDO", "shortcut": "Ctrl+Z", "category": "Edit" }
  ]
}
```

`ShortcutManager` merges file shortcuts with hardcoded defaults on startup.

---

## Auto Completion (`AutoCompletion`)

Provides word, function, and path completion while typing.

**File:** `src/dialogs/AutoCompletion.cpp` (620 lines)

### Completion Types

| Type | Trigger | Source |
|------|---------|--------|
| Word | 3+ chars typed | All words in the current document |
| Function | `(` typed | Function signatures from `FunctionCallTip` |
| Path | `/` or `\` in string | File paths in the current directory |
| Snippet | Tab | Predefined code snippets (not yet implemented) |

### Configuration

| Setting | Default | Description |
|---------|---------|-------------|
| Auto-complete delay | 200 ms | Delay before list appears |
| Minimum word length | 3 | Chars typed before auto-complete fires |
| From language | true | Include keywords from the current language |
| From opened files | true | Include words from all open buffers |

---

## Large File Warning (`LargeFileWarningDialog`)

Shown when opening a file over the threshold (default 5 MB).

**File:** `src/dialogs/LargeFileWarningDialog.cpp` (152 lines)

### Options

| Option | Effect |
|--------|--------|
| Open normally | Load the entire file |
| Load partially (first N MB) | Partial load mode — only the beginning is in memory |
| Don't open | Cancel the open |

### Partial Load

When partial mode is selected, `Buffer` sets `_partialLoad = true`. Only the first `_partialLoadSize` bytes are read. A banner in the editor shows:
```
Loading... 45% (2.3 MB / 5.1 MB)
```

`Buffer::loadRemaining()` can be called to load more on demand.

---

## About Dialog (`AboutDialog`)

**File:** `src/dialogs/AboutDialog.cpp` (623 lines)

Shows:
- Notepad--Qt name and version
- Qt version (`qVersion()`)
- QScintilla version
- Link to the GitHub repository
- Credits for NPP upstream and Qt
- GPL license text

---

## Column Editor (`ColumnEditorDialog`)

Insert text at a specific column across multiple lines (column mode).

**File:** `src/dialogs/ColumnEditorDialog.cpp` (210 lines)

### Use Case

With a column selection active:
1. `Alt+C` opens the Column Editor
2. Enter text to insert (e.g. `// `)
3. Click OK — the text is inserted at the column position on every selected line

---

## AnsiCharPanel (`AnsiCharPanel`)

Dockable panel for inserting special characters.

**File:** `src/dialogs/AnsiCharPanel.cpp` (256 lines)

Displays a grid of special characters organized by category:
- Box drawing characters
- Mathematical symbols
- Currency symbols
- Accented characters (Latin-1 supplement)
- Arrows

Click a character to insert it at the cursor.

---

## File Reload (`FileReloadDialog`)

Shown when an open file is modified externally.

**File:** `src/dialogs/FileReloadDialog.cpp` (77 lines)

| Option | Effect |
|--------|--------|
| Yes | Reload the file (discard local changes) |
| No | Keep the local version |
| Cancel | Cancel the reload |

Uses `QMessageBox` internally.

---

## Find Chars In Range (`FindCharsInRangeDialog`)

Find all characters that fall within a specified Unicode range.

**File:** `src/dialogs/FindCharsInRangeDialog.cpp` (527 lines)

### Use Case

Useful for finding:
- Non-ASCII characters (range `0x80–0xFF`)
- CJK characters (`0x4E00–0x9FFF`)
- Emoji (`0x1F300–0x1F9FF`)

Results are shown as a list of lines containing at least one character in the range.

---

## Printer (`Printer`)

Print preview and print configuration.

**File:** `src/dialogs/Printer.cpp` (288 lines)

### Status

Dialog UI is present. Qt `QPrinter` pipeline is not fully wired. Printing currently outputs blank pages.

See [HEARTBEAT.md](../HEARTBEAT.md) for current coverage.

### Options in the UI

| Setting | Description |
|---------|-------------|
| Orientation | Portrait / Landscape |
| Paper size | A4, Letter, Legal, etc. |
| Margins | Top, bottom, left, right in mm |
| Header/Footer | Filename, page number, date |
| Line numbers | Print line numbers in left margin |
| Color mode | Color / Grayscale / Inverted |

---

## Adding a New Dialog

1. Create files in `src/dialogs/YourDialog.h` and `src/dialogs/YourDialog.cpp`
2. Inherit from `QDialog` or `QWidget`:
   ```cpp
   class YourDialog : public QDialog {
       Q_OBJECT
   public:
       explicit YourDialog(QWidget* parent = nullptr);
   signals:
       void accepted(QString result);
   };
   ```
3. Register with the menu in `ui/MenuBar.cpp`:
   ```cpp
   void MenuBar::createSearchMenu() {
       _searchMenu->addAction(_mainWindow->getYourDialogAction());
   }
   ```
4. For modeless dialogs, create once and toggle visibility:
   ```cpp
   // In Application or MainWindow
   _yourDialog = new YourDialog(this);
   connect(action, &QAction::triggered, [this]() {
       _yourDialog->setVisible(!_yourDialog->isVisible());
   });
   ```
5. Add keyboard shortcut in `ShortcutManager::registerDefaultShortcuts()`:
   ```cpp
   registerShortcut("Your Dialog", Qt::Ctrl | Qt::Shift | Qt::Key_Y,
                   &_yourDialog, SLOT(toggle()));
   ```
