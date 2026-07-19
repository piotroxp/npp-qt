# Core Systems Reference

Reference for the central subsystems: Buffer, FileManager, Encoding, Session, Language, Shortcuts, Parameters, and Workspace.

---

## Buffer ()

The document model — one  per open file. All state associated with a file lives here.

**Files:** ,  (681 lines)

### What a Buffer Tracks



### Key Methods

| Method | Description |
|--------|-------------|
|  | Unique buffer identifier |
|  | Absolute path to the file |
|  | Has unsaved changes |
|  | Mark as modified |
|  | Read-only flag |
|  | Current  |
|  | Change encoding (does not recode) |
|  | Current  |
|  | Change language |
|  | Current  |
|  | Change line ending |
|  | Mark as partially loaded |
|  | Reload file from disk, detect changes |
|  | MD5 of current file content |

### Dirty Tracking

 is set to  whenever:
-  is called with different content
- A character is inserted or deleted
- Encoding or EOL is changed

 is set to  when:
-  succeeds
-  is called explicitly (e.g. after revert)

### Partial Load

Files over the threshold (default 5 MB) open in partial load mode:
1. Only the first  bytes are read into memory
2.  fetches more on demand
3. The status bar shows a progress banner

---

## FileManager ()

Registry of all open  instances. Provides a singleton interface for opening, closing, and cloning buffers.

**File:**  (804 lines)

### Singleton Access



### Key Methods

| Method | Description |
|--------|-------------|
|  | Get buffer by ID |
|  | Find existing buffer by path |
|  | Check if path exists |
|  | Open same file in a new view |
|  | Close a buffer |
|  | Close all buffers |
|  | File watcher active? |
|  | Enable/disable file watching |

### File Watching

 uses  to detect external file changes:



### Large File Handling



Shows  for files over the threshold. If the user declines, returns .

---

## EncodingDetector ()

Detects the character encoding of a file.

**File:**  (278 lines)

### Detection Pipeline



### BOM Detection



### UTF-8 Validation

Validates the byte sequence without decoding to Unicode:
- Bytes  must follow a lead byte
- Lead bytes must have the correct number of continuations
- Overlong encodings (e.g.  for NUL) are rejected

### HTML/XML Probing

Scans the first 2 KB for:


---

## EncodingMapper ()

Maps between Notepad++ encoding names, Qt codec names, and the internal  enum.

**File:**  (163 lines)

### Conversion



### Supported Encodings

All encodings from  are mapped to their Qt codec equivalents. Used by  for save/load.

---

## Utf8_16 ()

Low-level text file I/O with encoding conversion. Handles read/write of UTF-8, UTF-16, and single-byte encodings.

**File:**  (534 lines)

### Read Pipeline



### Write Pipeline



### EOL Conversion



Scans for , , or  and normalizes. Used when converting line endings on save.

---

## uchardet ()

Character encoding detection using the uchardet library (Mozilla's encoding detection).

**File:**  (400 lines)

### Usage



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

## LanguageManager ()

Manages the mapping between file extensions and .

**File:**  (346 lines)

### Key Methods

| Method | Description |
|--------|-------------|
|  | Full detection |
|  | Assign language to a buffer |
|  | Icon for the language |
|  | Human-readable name |
|  | Extensions for a language |

### Detection Priority

1. Extension match (e.g.  → )
2. Content heuristics (shebang , XML declaration, etc.)
3. Header file disambiguation ( → C++ vs Objective-C)

---

## SessionManager ()

Saves and restores the complete application state — open files, window geometry, panel visibility, recent files.

**File:**  (372 lines)

### Session File Format

See [COMMAND_LINE.md](COMMAND_LINE.md#session-file-format) for the JSON schema.

### What is Saved

| Section | Contents |
|---------|---------|
|  | Path, encoding, language, cursor position for each tab |
|  | Index of the active tab |
|  | x, y, width, height |
|  | Which panels are shown |
|  | Last 20 opened files |
|  | Sizes of the splitter panes |

### Auto-save

The session is saved automatically on:
-  (window close)
- Tab close (saves after every close)

And optionally on a timer (⚠️ timer not wired — see HEARTBEAT).

---

## Parameters ()

The global configuration store. Manages  and .

**File:**  (601 lines)

### What it Stores



### Loading



### Saving



### Singleton



---

## ShortcutManager ()

Registry of all keyboard shortcuts. Handles conflict detection, storage, and merging of user-defined shortcuts.

**File:**  (364 lines)

### How Shortcuts Work

Win32 NPP used  (global system-wide hotkeys). Notepad--Qt uses  (per-widget Qt shortcuts) mapped via :



### Conflict Detection



Conflicts are resolved by:
1. User picks a different shortcut in the Shortcut Mapper
2. Or the new shortcut overwrites the old one (last-registered wins)

### Loading User Shortcuts



### Export



---

## MacroManager ()

Records and plays back keyboard macro sequences.

**File:**  (227 lines)

### Macro Actions

A macro is a sequence of  objects:



Examples:
-  — insert text
-  — delete current line
-  — duplicate line

### Lifecycle



### Status

⚠️ **Not fully wired.**  is present but  /  are not connected to the menu. See HEARTBEAT.

---

## RecentFilesManager ()

Manages the recent files list shown in .

**File:**  (113 lines)

### Key Methods

| Method | Description |
|--------|-------------|
|  | Prepend path to recent list |
|  | Remove a path from recent list |
|  |  of recent paths |
|  | Clear the list |
|  | Any recent files? |

### Persistence

Loaded from  ( key) on startup, saved on each  call.

### Max Size

Default: 20 files. Configurable in  (). When the list exceeds max, the oldest entry is removed.

---

## ThemeManager ()

Loads and manages theme JSON files.

**File:**  (170 lines)

### Theme File Format

See [THEMES.md](THEMES.md#custom-themes-json).

### Loading



### Applying

 emits  → all open editors apply the new theme via their .

---

## WorkspaceManager ()

Manages project workspaces — sets of files grouped together.

**File:**  (350 lines)

### Workspace File Format



### Key Methods

| Method | Description |
|--------|-------------|
|  | Load workspace file |
|  | Save current workspace |
|  | Unload workspace |
|  | Add file to workspace |
|  | Remove file from workspace |

### Status

⚠️ Partial — workspace files can be saved and loaded, but the File Browser integration is not fully wired.

---

## NppCommands ()

Central command dispatcher — maps  IDs to handler functions.

**File:**  (846 lines)

### Command Dispatch



### How it Works

 owns the mapping from NPP command IDs to handler methods.  connects menu actions and shortcut triggers to , which then calls the appropriate method.

This is the central hub for menu functionality — if a menu item doesn't work, the fix goes here or in the handler it calls.
