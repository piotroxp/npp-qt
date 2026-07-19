# Core Systems Reference

Reference for the central subsystems: Buffer, FileManager, Encoding, Session, Language, Shortcuts, Parameters, and Workspace.

---

## Buffer (`src/core/Buffer`)

The document model — one `Buffer` per open file. All state associated with a file lives here.

**Files:** `Buffer.h`, `Buffer.cpp` (681 lines)

### What a Buffer Tracks

```cpp
class Buffer {
    int              _id;              // unique buffer ID
    QString          _filePath;        // absolute file path
    EncodingType     _encoding = UTF_8; // current encoding
    EolType          _eolType = EOL_LF;
    LangType         _langType = L_NORMAL;
    bool             _isDirty = false; // modified since last save
    bool             _isReadOnly = false;
    bool             _isUntitled = true; // no file on disk
    bool             _partialLoad = false;
    int              _partialLoadSize = 0;
    int              _references = 0;  // refcount (cloned views)
    QDateTime        _lastModified;    // file mtime at load time
    QByteArray       _fileDigest;      // MD5 for reload detection
};
```

### Key Methods

| Method | Description |
|--------|-------------|
| `id()` | Unique buffer identifier |
| `filePath()` | Absolute path to the file |
| `isDirty()` | Has unsaved changes |
| `setDirty(bool)` | Mark as modified |
| `isReadOnly()` | Read-only flag |
| `encoding()` | Current `EncodingType` |
| `setEncoding(EncodingType)` | Change encoding (does not recode) |
| `langType()` | Current `LangType` |
| `setLangType(LangType)` | Change language |
| `eolType()` | Current `EolType` |
| `setEolType(EolType)` | Change line ending |
| `setPartialLoad(bool, size)` | Mark as partially loaded |
| `reload()` | Reload file from disk, detect changes |
| `digest()` | MD5 of current file content |

### Dirty Tracking

`_isDirty` is set to `true` whenever:
- `setText()` is called with different content
- A character is inserted or deleted
- Encoding or EOL is changed

`_isDirty` is set to `false` when:
- `save()` succeeds
- `setDirty(false)` is called explicitly (e.g. after revert)

### Partial Load

Files over the threshold (default 5 MB) open in partial load mode:
1. Only the first `partialLoadSize` bytes are read into memory
2. `Buffer::loadRemaining(startOffset)` fetches more on demand
3. The status bar shows a progress banner

---

## FileManager (`src/core/FileManager`)

Registry of all open `Buffer` instances. Provides a singleton interface for opening, closing, and cloning buffers.

**File:** `FileManager.cpp` (804 lines)

### Singleton Access

```cpp
FileManager& fm = FileManager::instance();
```

### Key Methods

| Method | Description |
|--------|-------------|
| `getBuffer(int id)` | Get buffer by ID |
| `getBufferByPath(const QString&)` | Find existing buffer by path |
| `fileExists(const QString&)` | Check if path exists |
| `cloneBuffer(int id)` | Open same file in a new view |
| `closeBuffer(int id)` | Close a buffer |
| `closeAll()` | Close all buffers |
| `isFileMonitoring()` | File watcher active? |
| `setFileMonitoring(bool)` | Enable/disable file watching |

### File Watching

`FileManager` uses `QFileSystemWatcher` to detect external file changes:

```
FileManager::open(path)
  → QFileSystemWatcher.addPath(path)
  → onFileChanged(path)
      → if buffer exists and isDirty:
            → show FileReloadDialog
        else if not dirty:
            → buffer->reload()
```

### Large File Handling

```cpp
// Opens file, returns nullptr if user declines large file
Buffer* FileManager::openLargeFile(const QString& path);
```

Shows `LargeFileWarningDialog` for files over the threshold. If the user declines, returns `nullptr`.

---

## EncodingDetector (`src/core/EncodingDetector`)

Detects the character encoding of a file.

**File:** `EncodingDetector.cpp` (278 lines)

### Detection Pipeline

```cpp
EncodingType EncodingDetector::detect(const QByteArray& rawBytes,
                                     const QString& filenameHint) {
    // 1. BOM check (fastest)
    if (auto enc = detectBOM(rawBytes)) return enc;

    // 2. UTF-8 validity check
    if (looksLikeUTF8(rawBytes)) return UTF_8;

    // 3. HTML/XML meta charset probe
    if (auto enc = detectHtmlCharset(rawBytes)) return enc;

    // 4. Single-byte heuristics (uchardet)
    EncodingType enc = uchardet.detect(rawBytes);
    if (enc != UNKNOWN) return enc;

    // 5. Default
    return UTF_8;
}
```

### BOM Detection

```cpp
detectBOM(rawBytes):
    "EF BB BF"  → UTF_8_BOM
    "FF FE"     → UTF_16_LE_BOM
    "FE FF"     → UTF_16_BE_BOM
    "FF FE 00 00" → UTF_32_LE_BOM
    "00 00 FE FF" → UTF_32_BE_BOM
    else → nullptr (no BOM)
```

### UTF-8 Validation

Validates the byte sequence without decoding to Unicode:
- Bytes `0x80–0xBF` must follow a lead byte
- Lead bytes must have the correct number of continuations
- Overlong encodings (e.g. `0xC0 0x80` for NUL) are rejected

### HTML/XML Probing

Scans the first 2 KB for:
```html
<meta charset="UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
<?xml version="1.0" encoding="ISO-8859-1"?>
```

---

## EncodingMapper (`src/core/EncodingMapper`)

Maps between Notepad++ encoding names, Qt codec names, and the internal `EncodingType` enum.

**File:** `EncodingMapper.cpp` (163 lines)

### Conversion

```cpp
QString EncodingMapper::toQtCodecName(EncodingType enc);
// "UTF-8" → "UTF-8"
// "Windows-1252" → "Windows-1252"
// "ISO-8859-1" → "ISO-8859-1"

EncodingType EncodingMapper::fromQtCodecName(const QString& codec);
// "UTF-8" → UTF_8
// "ISO-8859-1" → ISO_8859_1
```

### Supported Encodings

All encodings from `Types.h` are mapped to their Qt codec equivalents. Used by `Utf8_16.cpp` for save/load.

---

## Utf8_16 (`src/core/Utf8_16`)

Low-level text file I/O with encoding conversion. Handles read/write of UTF-8, UTF-16, and single-byte encodings.

**File:** `Utf8_16.cpp` (534 lines)

### Read Pipeline

```
FileManager opens file → QFile.readAll()
  → Utf8_16::decode(rawBytes, encoding)
      → strip BOM if present
      → decode to QByteArray (UTF-8 internal or UTF-16)
```

### Write Pipeline

```
editor.getTextAsUtf8() → QByteArray
  → Utf8_16::encode(bytes, targetEncoding)
      → convert from UTF-8 to target encoding
      → prepend BOM if needed
  → QFile.write(encodedBytes)
```

### EOL Conversion

```cpp
void Utf8_16::convertEol(QByteArray& content, EolType from, EolType to);
```

Scans for `\n`, `\r\n`, or `\r` and normalizes. Used when converting line endings on save.

---

## uchardet (`src/uchardet`)

Character encoding detection using the uchardet library (Mozilla's encoding detection).

**File:** `uchardet/CharsetDetector.cpp` (400 lines)

### Usage

```cpp
CharsetDetector detector;
detector.handleData(rawBytes);
const char* encoding = detector.getCharset();
// Returns: "UTF-8", "windows-1252", "Shift_JIS", etc.
```

### How it Works

Uses byte-frequency analysis and n-gram statistics to guess the encoding. More accurate than simple heuristics for:
- Single-byte Western encodings (Windows-1252, ISO-8859-1, ISO-8859-2)
- CJK encodings (Shift_JIS, EUC-JP, GB18030)
- Cyrillic (KOI8-R, Windows-1251)
- ISO-8859-5

Not effective for:
- Pure ASCII (just returns the default)
- UTF-8 (use UTF-8 validation instead)

---

## LanguageManager (`src/core/LanguageManager`)

Manages the mapping between file extensions and `LangType`.

**File:** `LanguageManager.cpp` (346 lines)

### Key Methods

| Method | Description |
|--------|-------------|
| `detectLangType(const QString& path, const QString& content)` | Full detection |
| `setLangType(Buffer*, LangType)` | Assign language to a buffer |
| `getLangTypeIcon(LangType)` | Icon for the language |
| `getLangTypeDisplayName(LangType)` | Human-readable name |
| `getExtensionsForLang(LangType)` | Extensions for a language |

### Detection Priority

1. Extension match (e.g. `.py` → `L_PYTHON`)
2. Content heuristics (shebang `#!`, XML declaration, etc.)
3. Header file disambiguation (`.h` → C++ vs Objective-C)

---

## SessionManager (`src/core/SessionManager`)

Saves and restores the complete application state — open files, window geometry, panel visibility, recent files.

**File:** `SessionManager.cpp` (372 lines)

### Session File Format

See [COMMAND_LINE.md](COMMAND_LINE.md#session-file-format) for the JSON schema.

### What is Saved

| Section | Contents |
|---------|---------|
| `buffers` | Path, encoding, language, cursor position for each tab |
| `activeTab` | Index of the active tab |
| `windowGeometry` | x, y, width, height |
| `panelVisibility` | Which panels are shown |
| `recentFiles` | Last 20 opened files |
| `splitterState` | Sizes of the splitter panes |

### Auto-save

The session is saved automatically on:
- `QCloseEvent` (window close)
- Tab close (saves after every close)

And optionally on a timer (⚠️ timer not wired — see HEARTBEAT).

---

## Parameters (`src/core/Parameters`)

The global configuration store. Manages `config.json` and `shortcuts.json`.

**File:** `Parameters.cpp` (601 lines)

### What it Stores

```cpp
// ~/.config/notepad--qt/config.json
{
    "newDocDefaultEncoding": "UTF-8",
    "newDocDefaultEol": "LF",
    "newDocDefaultLanguage": "Normal",
    "tabSize": 4,
    "indentSize": 4,
    "useTabs": false,
    "autoDetectEncoding": true,
    "saveWithoutBOM": true,
    "fileHistorySize": 20,
    "openSaveDir": "lastDir",
    "checkModifiedFiles": true,
    "fileAutoDetectionMaxSize": 5242880,  // 5 MB
    "autoComplete": {
        "enabled": true,
        "fromLang": true,
        "fromSession": true,
        "minWordLen": 3
    },
    "findHistory": [...]
}
```

### Loading

```cpp
Parameters::load();  // reads config.json on startup
```

### Saving

```cpp
Parameters::save();  // writes config.json on close or after prefs apply
```

### Singleton

```cpp
Parameters& p = Parameters::instance();
```

---

## ShortcutManager (`src/core/ShortcutManager`)

Registry of all keyboard shortcuts. Handles conflict detection, storage, and merging of user-defined shortcuts.

**File:** `ShortcutManager.cpp` (364 lines)

### How Shortcuts Work

Win32 NPP used `RegisterHotKey()` (global system-wide hotkeys). Notepad--Qt uses `QShortcut` (per-widget Qt shortcuts) mapped via `ShortcutAdapter`:

```cpp
ShortcutManager::registerShortcut(command, keySequence, receiver, method)
  → ShortcutAdapter::register(keySeq)
      → QShortcut(shortcutKey, widget)
      → connect(QShortcut::activated, receiver, method)
```

### Conflict Detection

```cpp
ShortcutManager::hasConflict(keySeq)
  → checks _shortcutMap for existing binding
  → if conflict: returns the conflicting command name
```

Conflicts are resolved by:
1. User picks a different shortcut in the Shortcut Mapper
2. Or the new shortcut overwrites the old one (last-registered wins)

### Loading User Shortcuts

```cpp
ShortcutManager::loadShortcuts();
  → reads ~/.config/notepad--qt/shortcuts.json
  → merges with hardcoded defaults (file wins if key exists)
```

### Export

```cpp
ShortcutManager::saveShortcuts();
  → writes current shortcuts to shortcuts.json
```

---

## MacroManager (`src/core/MacroManager`)

Records and plays back keyboard macro sequences.

**File:** `MacroManager.cpp` (227 lines)

### Macro Actions

A macro is a sequence of `MacroAction` objects:

```cpp
struct MacroAction {
    int      message;   // Scintilla message ID or custom command ID
    QString  text;       // For text-insert actions
    int      wParam;    // Additional param
    int      lParam;    // Additional param
};
```

Examples:
- `Type: "hello"` — insert text
- `SCI_LINEDELETE` — delete current line
- `SCI_SELECTIONDUPLICATE` — duplicate line

### Lifecycle

```
startRecording()
  → _recording = true
  → _recordedActions.clear()
  → for each command:
       action = MacroAction::fromCommand(cmd)
       _recordedActions.append(action)

stopRecording()
  → _recording = false
  → macro = Macro{_recordedActions, name}

playback(macro)
  → for each action in macro.actions:
       executeAction(action)

saveMacro(name, path)
  → JSON serialize macro to file

loadMacro(path)
  → JSON deserialize to Macro object
```

### Status

⚠️ **Not fully wired.** `MacroManager` is present but `startRecordingMacro` / `stopRecordingMacro` are not connected to the menu. See HEARTBEAT.

---

## RecentFilesManager (`src/core/RecentFilesManager`)

Manages the recent files list shown in `File → Recent Files`.

**File:** `RecentFilesManager.cpp` (113 lines)

### Key Methods

| Method | Description |
|--------|-------------|
| `add(path)` | Prepend path to recent list |
| `remove(path)` | Remove a path from recent list |
| `getAll()` | `QStringList` of recent paths |
| `clear()` | Clear the list |
| `isEmpty()` | Any recent files? |

### Persistence

Loaded from `config.json` (`recentFiles` key) on startup, saved on each `add()` call.

### Max Size

Default: 20 files. Configurable in `Parameters` (`fileHistorySize`). When the list exceeds max, the oldest entry is removed.

---

## ThemeManager (`src/core/ThemeManager`)

Loads and manages theme JSON files.

**File:** `ThemeManager.cpp` (170 lines)

### Theme File Format

See [THEMES.md](THEMES.md#custom-themes-json).

### Loading

```cpp
Theme ThemeManager::loadTheme(const QString& name);
  → QFile themeFile(themesDir + "/" + name + ".json")
  → QJsonDocument::fromJson(file.readAll())
  → Theme t = parse(themeFile)
  → return t
```

### Applying

`Application::loadTheme(name)` emits `themeChanged(name)` → all open editors apply the new theme via their `SyntaxHighlighter`.

---

## WorkspaceManager (`src/core/WorkspaceManager`)

Manages project workspaces — sets of files grouped together.

**File:** `WorkspaceManager.cpp` (350 lines)

### Workspace File Format

```json
{
  "name": "My Project",
  "root": "/home/user/src",
  "files": [
    "/home/user/src/main.cpp",
    "/home/user/src/utils.cpp"
  ],
  "activeFile": "/home/user/src/main.cpp",
  "bookmarks": [
    { "file": "main.cpp", "line": 42 }
  ]
}
```

### Key Methods

| Method | Description |
|--------|-------------|
| `openWorkspace(path)` | Load workspace file |
| `saveWorkspace(path)` | Save current workspace |
| `closeWorkspace()` | Unload workspace |
| `addFile(path)` | Add file to workspace |
| `removeFile(path)` | Remove file from workspace |

### Status

⚠️ Partial — workspace files can be saved and loaded, but the File Browser integration is not fully wired.

---

## NppCommands (`src/core/NppCommands`)

Central command dispatcher — maps `menuCmdID.h` IDs to handler functions.

**File:** `NppCommands.cpp` (846 lines)

### Command Dispatch

```cpp
NppCommands::dispatch(int cmdID)
  → switch(cmdID):
      case IDM_FILE_NEW:          doFileNew(); break;
      case IDM_FILE_OPEN:         doFileOpen(); break;
      case IDM_EDIT_UNDO:         doEditUndo(); break;
      // ... ~200 more commands
```

### How it Works

`NppCommands` owns the mapping from NPP command IDs to handler methods. `MainWindow` connects menu actions and shortcut triggers to `dispatch(cmdID)`, which then calls the appropriate method.

This is the central hub for menu functionality — if a menu item doesn't work, the fix goes here or in the handler it calls.
