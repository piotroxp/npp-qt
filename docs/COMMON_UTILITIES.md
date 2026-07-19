# Common Utilities Reference

Reference for the shared utility classes in `src/common/`.

---

## StringHelper (`src/common/StringHelper`)

String manipulation utilities used throughout the codebase.

**Files:** `StringHelper.h`, `StringHelper.cpp` (465 lines)

### Key Functions

| Function | Description |
|----------|-------------|
| `trim(const QString&)` | Remove leading/trailing whitespace |
| `trimLeft(const QString&)` | Remove leading whitespace |
| `trimRight(const QString&)` | Remove trailing whitespace |
| `replace(const QString&, old, new)` | Replace all occurrences |
| `replaceFirst(const QString&, old, new)` | Replace first occurrence |
| `escape(const QString&)` | Escape special chars (e.g. for regex) |
| `unescape(const QString&)` | Unescape special chars |
| `spacesToTabs(const QString&, tabSize)` | Convert leading spaces to tabs |
| `tabsToSpaces(const QString&, tabSize)` | Convert leading tabs to spaces |
| `fileExt(const QString& path)` | Extract extension (e.g. `.cpp`) |
| `fileName(const QString& path)` | Extract filename (e.g. `foo.cpp`) |
| `fileDir(const QString& path)` | Extract directory (e.g. `/home/user/src`) |
| `normalizePath(const QString&)` | Normalize slashes and resolve `..` |
| `toHtmlEscaped(const QString&)` | Escape `&`, `<`, `>` for HTML |
| `toLower(const QString&)` | Convert to lowercase |
| `toUpper(const QString&)` | Convert to uppercase |
| `startsWith(const QString&, prefix)` | Check prefix |
| `endsWith(const QString&, suffix)` | Check suffix |
| `contains(const QString&, substr)` | Check if substring present |
| `join(const QStringList&, sep)` | Join with separator |
| `split(const QString&, sep)` | Split by separator |
| `isBlank(const QString&)` | True if only whitespace |
| `isNumber(const QString&)` | True if string is a number |

---

## FileHelper (`src/common/FileHelper`)

File system operations.

**Files:** `FileHelper.h`, `FileHelper.cpp` (640 lines)

### Key Functions

| Function | Description |
|----------|-------------|
| `exists(const QString& path)` | Does the file/directory exist? |
| `isFile(const QString& path)` | Is it a regular file? |
| `isDir(const QString& path)` | Is it a directory? |
| `size(const QString& path)` | File size in bytes |
| `readTextFile(path)` | Read entire file as `QString` |
| `readBinaryFile(path)` | Read entire file as `QByteArray` |
| `writeTextFile(path, content)` | Write `QString` to file |
| `writeBinaryFile(path, content)` | Write `QByteArray` to file |
| `appendTextFile(path, content)` | Append to file |
| `copy(source, dest)` | Copy file |
| `move(source, dest)` | Move/rename file |
| `remove(path)` | Delete file |
| `makeDir(path)` | Create directory |
| `makeDirRecursive(path)` | Create directory + parents |
| `removeDir(path)` | Remove empty directory |
| `removeDirRecursive(path)` | Remove directory and contents |
| `listDir(path, filter)` | List files matching filter |
| `listFiles(path, pattern)` | List files matching glob pattern |
| `listDirs(path)` | List subdirectories |
| `fileModifiedTime(path)` | `QDateTime` of last modification |
| `fileCreatedTime(path)` | `QDateTime` of creation |
| `filePermissions(path)` | `QFile::Permissions` flags |
| `setPermissions(path, perms)` | Set file permissions |
| `isSymLink(path)` | Is the file a symlink? |
| `readSymLink(path)` | Where does the symlink point? |

### Atomic Write

```cpp
// Write to a temp file, then rename (atomic on POSIX)
FileHelper::atomicWrite(path, content);
```

Uses `QTemporaryFile` internally, then `QFile::rename()` — ensures the target file is never in a partially-written state.

---

## FileWatcherAdapter (`src/common/FileWatcherAdapter`)

Wrapper around `QFileSystemWatcher` with debouncing and change coalescing.

**Files:** `FileWatcherAdapter.h`, `FileWatcherAdapter.cpp` (373 lines)

### Why a Wrapper?

`QFileSystemWatcher` fires duplicate events rapidly when a file is modified (e.g. by a text editor auto-save). `FileWatcherAdapter` debounces and coalesces these:

```cpp
// Without debouncing: 10+ events on one save
// With debouncing: 1 event, 200ms after last change
watcher->addFile(path);
connect(watcher, &FileWatcherAdapter::fileChanged,
        this, &MyClass::onFileChanged);

// Debounce: 200ms delay before firing
// Coalesce: multiple rapid changes → 1 event
```

### Key Methods

| Method | Description |
|--------|-------------|
| `addPath(const QString&)` | Watch a file |
| `removePath(const QString&)` | Stop watching |
| `addDirectory(const QString&)` | Watch a directory recursively |
| `setDebounceMs(int)` | Set debounce delay (default: 200 ms) |

### Signals

| Signal | Payload | Description |
|--------|---------|-------------|
| `fileChanged(const QString&)` | path | File changed (debounced) |
| `fileCreated(const QString&)` | path | New file in watched directory |
| `fileDeleted(const QString&)` | path | File was deleted |

---

## ClipboardAdapter (`src/common/ClipboardAdapter`)

Qt clipboard wrapper with history support.

**Files:** `ClipboardAdapter.h`, `ClipboardAdapter.cpp` (322 lines)

### Key Methods

| Method | Description |
|--------|-------------|
| `text()` | Get clipboard text |
| `setText(const QString&)` | Set clipboard text |
| `image()` | Get clipboard image |
| `setImage(const QImage&)` | Set clipboard image |
| `clear()` | Clear clipboard |
| `history()` | `QStringList` of clipboard history |
| `addToHistory(const QString&)` | Add entry to history |
| `clearHistory()` | Clear clipboard history |

### History Behavior

The clipboard history is maintained separately from the Qt clipboard. When `setText()` is called:
1. The text is placed on the clipboard
2. The text is prepended to the history list (duplicates are removed)
3. History is limited to `MAX_HISTORY` entries (default: 50)

---

## PathHelper (`src/common/PathHelper`)

Path manipulation utilities.

**Files:** `PathHelper.h`, `PathHelper.cpp` (168 lines)

### Key Functions

| Function | Description |
|----------|-------------|
| `absolute(const QString&)` | Make path absolute |
| `join(const QString&, ...)` | Join path components |
| `normalize(const QString&)` | Normalize path (resolve `.`, `..`) |
| `relative(base, target)` | Relative path from base to target |
| `isAbsolute(const QString&)` | Is the path absolute? |
| `isRelative(const QString&)` | Is the path relative? |
| `canonical(const QString&)` | Canonical path (resolves symlinks) |
| `dirname(const QString&)` | Parent directory |
| `basename(const QString&)` | Filename without directory |
| `extname(const QString&)` | Extension (with dot) |
| `stem(const QString&)` | Filename without extension |

### Path Separators

```cpp
// Always use / internally
QString p = PathHelper::join("home", "user", "file.txt");
// → "home/user/file.txt"
```

---

## UrlHelper (`src/common/UrlHelper`)

URL parsing and manipulation.

**Files:** `UrlHelper.h`, `UrlHelper.cpp` (115 lines)

### Key Functions

| Function | Description |
|----------|-------------|
| `parse(const QString&)` | Parse URL into components |
| `scheme(url)` | `http`, `file`, etc. |
| `host(url)` | Hostname |
| `port(url)` | Port number |
| `path(url)` | Path component |
| `query(url)` | Query string |
| `fragment(url)` | Fragment (after `#`) |
| `isLocal(const QString&)` | Is it a local `file://` URL? |
| `toLocalFile(const QString&)` | Convert `file://` to local path |
| `fromLocalFile(const QString&)` | Convert local path to `file://` |

---

## ShortcutAdapter (`src/common/ShortcutAdapter`)

Maps Win32 `RegisterHotKey()` hotkeys to Qt `QShortcut`.

**Files:** `ShortcutAdapter.h`, `ShortcutAdapter.cpp` (302 lines)

### Why?

NPP used `RegisterHotKey()` for system-wide global shortcuts. Notepad--Qt uses `QShortcut` for in-app shortcuts. `ShortcutAdapter` provides a compatibility layer:

```cpp
// NPP style (Win32):
RegisterHotKey(hwnd, id, MOD_CONTROL | MOD_SHIFT, VK_F);
→ WM_HOTKEY message

// Notepad--Qt style (Qt):
ShortcutAdapter::registerShortcut(keySeq, receiver, method);
// → QShortcut(shortcutKey, widget)
// → connect(activated, receiver, method)
```

### Key Methods

| Method | Description |
|--------|-------------|
| `registerShortcut(keySeq, receiver, method)` | Register a Qt shortcut |
| `unregisterShortcut(keySeq)` | Remove a shortcut |
| `hasShortcut(keySeq)` | Is a shortcut registered? |
| `getCommand(keySeq)` | Get the command for a shortcut |

---

## DpiManager (`src/common/DpiManager`)

DPI-aware scaling for high-DPI displays.

**Files:** `DpiManager.h`, `DpiManager.cpp` (142 lines)

### Key Methods

| Method | Description |
|--------|-------------|
| `dpiScale(qreal)` | Scale a value by the screen DPI ratio |
| `logicalDpi()` | Logical (system-reported) DPI |
| `physicalDpi()` | Physical (hardware) DPI |
| `devicePixelRatio()` | `QWindow::devicePixelRatio()` |
| `setBaseDpi(int)` | Override base DPI (for testing) |

### Usage

```cpp
// Scale font sizes for high-DPI
int size = DpiManager::dpiScale(12);  // 12pt → 18pt on 150% DPI

// Scale widget dimensions
QSize sz = DpiManager::dpiScale(QSize(100, 50));
```

---

## Constants (`src/common/Constants.h`)

Global constants used throughout the application.

```cpp
namespace NppConstants {
    constexpr int DEFAULT_TAB_WIDTH = 4;
    constexpr int DEFAULT_INDENT_SIZE = 4;
    constexpr int DEFAULT_FILE_HISTORY_SIZE = 20;
    constexpr int MAX_FILE_SIZE_FOR_PARTIAL = 5 * 1024 * 1024;  // 5 MB
    constexpr int MAX_RECENT_FILES = 50;
    constexpr int MAX_CLIPBOARD_HISTORY = 50;
    constexpr int MAX_UNDO_STEPS = 10000;
    constexpr int AUTO_COMPLETE_DELAY_MS = 200;
    constexpr int FUNC_LIST_DEBOUNCE_MS = 200;
    constexpr int DOC_MAP_FPS = 60;
    constexpr int SCROLL_DEBOUNCE_MS = 16;
    const QString APP_NAME = "Notepad--Qt";
    const QString ORG_NAME = "notepad-plus-plus";
    const QString CONFIG_DIR = ".config/notepad--qt";
    const QString THEME_DIR = "themes";
    const QString PLUGIN_DIR = "plugins";
    const QString UDL_DIR = "udl";
}
```

---

## NonCopyable (`src/common/NonCopyable.h`)

Base class to prevent copy construction and copy assignment.

```cpp
class NonCopyable {
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

// Usage:
class MySingleton : public NonCopyable { ... };
```

---

## Observer (`src/common/Observer.h`)

Simple signal-slot observer pattern.

```cpp
// Publish
class Subject {
    Observer _obs;
public:
    void subscribe(Callback cb) { _obs.add(cb); }
    void notify() { _obs.emit(); }
};

// Subscribe
subject.subscribe([]() {
    qDebug() << "changed!";
});
```

---

## ScintillaComponent (`src/common/ScintillaComponent.h`)

Base class for all Scintilla-wrapping widgets. Provides `sendSci()` for sending messages to the underlying Scintilla instance.

```cpp
class ScintillaComponent {
protected:
    QsciScintilla* _sci;
    void sendSci(int msg, int wParam = 0, long lParam = 0);
    long sendSciWithResult(int msg, int wParam, long lParam);
};
```

All classes that wrap Scintilla (`ScintillaEditor`, `DocumentMapPanel`, `IncrementalSearchDialog`, etc.) inherit from this.
