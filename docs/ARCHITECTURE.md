# Architecture — Notepad--Qt

## Overview

Notepad--Qt is a semantic lift of Notepad++ from Win32/C++ to Qt6/C++. The architecture mirrors the original Notepad++ design philosophy — a single-window MDI editor with a dockable-panel layout, buffer-backed document model, and command-driven UI — implemented natively on Qt.

The application is built with **C++20**, **Qt6**, and **QScintilla** (the Qt port of the Scintilla editor component).

---

## Design Principles

1. **Single-window MDI** — One `QMainWindow` with a central tabbed editor and dockable panels.
2. **Buffer model** — All open documents live in a `Buffer` object managed by `FileManager`. The UI holds a pointer, never owns the buffer.
3. **Signal/slot reactive UI** — UI components (MenuBar, ToolBar, TabBar, StatusBar) are driven by `Application` signals; they emit command signals back which `Application` routes.
4. **Singleton managers** — `EncodingDetector`, `LanguageManager`, `EditorCommandManager`, `FileManager` are singleton objects initialized once at startup.
5. **No shared mutable state across threads** — All file I/O, encoding detection, and UI updates are serialised through `Application` or dispatched to a background thread pool via `FileLoaderWorker`.

---

## Startup Sequence

```
main()
  → QApplication construction (NotepadApp subclass handles FileOpen events)
  → parseArgs() — command-line: files, config dir, plugin dir, session
  → Application::instance().initialize()
      1. setupDirectories() — create ~/.config/notepad--qt/
      2. loadConfig() — QSettings from config dir
      3. new FileManager()
      4. new EncodingDetector()
      5. new LanguageManager()
      6. new SessionManager()
      7. new EditorCommandManager() → registerAll()
      8. new MacroManager()
      9. RecentFilesManager::instance()
      10. new UdlManager()
      11. setupUI()
          • MainWindow construction (creates QTabWidget, menus, toolbars, statusbar)
          • setupDockingPanels() — lazy panel construction (FileBrowserPanel, FunctionListPanel,
                                      DocumentMapPanel, ClipboardHistoryPanel)
      12. setupConnections() — connect Application signals to UI slots
      13. PluginManager::instance().loadPlugins() — scan plugin dir, dlopen each .so
      14. _state = AppState::Ready
      15. showMainWindow() — QMainWindow::show()
      16. restoreGeometry() / restoreState() from QSettings
      17. _incrementalSearch = new IncrementalSearchDialog()
  → for each command-line file: Application::openFile()
  → if no files: Application::newFile()
  → app.exec() — Qt event loop
  → on exit: Application::shutdown()
```

---

## The Buffer / Editor Model

### Types.h — Core ID Types

```cpp
using BufferID = Buffer*;      // A raw pointer to the Buffer object
#define BUFFER_INVALID nullptr

using EditorID = ScintillaEditor*;
#define EDITOR_INVALID nullptr

using ViewID = int;            // Split-view index (0 = main, 1 = sub)
#define VIEW_INVALID (-1)
```

### FileManager — Buffer Lifecycle

`FileManager` (`src/core/FileManager.h`) is the central registry for all open buffers.

```
FileManager
  _nextBufferId : BufferID            // monotonically increasing pointer value
  _buffers      : std::vector<Buffer*>
  _activeBuffer  : BufferID
  _bufferText   : std::unordered_map<BufferID, QString>  // in-memory text cache

createBuffer(fileName, isLargeFile)  → allocates Buffer*, returns BufferID
openFile(path)                       → createBuffer() + loadFileIntoBuffer()
createNewFile()                      → createBuffer() with DOC_UNNAMED status
duplicateBuffer(buffer)              → createBuffer() + copy text
closeFile(buffer)                    → remove from _buffers, destroy Buffer*
bufferFromId(id)                     → O(1) lookup in _buffers vector
```

### Buffer — Document State

`Buffer` (`src/core/Buffer.h`) is a `QObject` subclass that owns all metadata for a single document:

```
Buffer
  _id           : BufferID
  _document     : void*           // SciTe document pointer (QsciScintilla::document())
  _fullPathName : QString
  _fileName     : QString
  _compactFileName : QString

  _encoding     : EncodingType    // UTF-8, UTF-16 LE, Windows-1252, etc.
  _langType     : LangType        // L_CPP, L_PYTHON, etc.
  _eolFormat    : EolType         // EOL_LF, EOL_CRLF, EOL_CR
  _isDirty      : bool            // modified since last save
  _isModified   : bool
  _isSavePointDirty : bool

  _isUserReadOnly : bool
  _isFileReadOnly : bool
  _isLargeFile     : bool
  _totalFileSize    : qint64
  _isPartialLoad    : bool

  _referees    : QVector<ScintillaEditor*>   // editors showing this buffer (split view)
  _positions   : QVector<Position>           // per-editor scroll/cursor state
  _foldStates  : QVector<std::vector<size_t>>  // per-editor fold state

  _fileWatcher : QFileSystemWatcher*         // external change detection
  _isMonitoringOn : bool
```

A single `Buffer` can be displayed in **multiple editors** simultaneously (split view). Each `ScintillaEditor` registers itself with the buffer via `addReference()`, and the buffer tracks per-editor scroll position, cursor position, and fold state in parallel arrays.

### File → Buffer → Editor Flow

```
openFile("foo.cpp")
  → FileManager::openFile(path)
      → EncodingDetector::detect(bytes) → EncodingType
      → LanguageManager::detectLanguage(path, bytes) → LangType
      → FileManager::createBuffer(path, isLargeFile)
          → new Buffer(this, id, path, isLargeFile)
          → buffer->setEncoding(detected)
          → buffer->setLangType(detected)
          → _buffers.push_back(buffer)
      → FileManager::loadFileIntoBuffer(id, path)
          → FileLoaderWorker (QThread) reads file
          → QFileSystemWatcher watches path
      → Application::openFile() → returns BufferID

Buffer opened
  → Application::onBufferOpened(BufferID)
      → MainWindow creates ScintillaEditor* for this BufferID
      → FileManager::addBufferReference(BufferID, ScintillaEditor*)
      → Buffer::addReference(editor)
      → MainWindow::addEditorTab(editor, buffer->getFileName())
          → DocTabView::addTab(editor, title)
```

### MainWindow Buffer↔Editor↔Tab Registry

`MainWindow` (`src/ui/MainWindow.h`) maintains three parallel maps:

```cpp
_tabToBuffer   : QMap<int, BufferID>      // tab index → buffer
_bufferToEditor: QMap<BufferID, ScintillaEditor*>  // buffer → editor
_bufferToTab   : QMap<BufferID, int>      // buffer → tab index
```

These maps are kept synchronized on tab switch, close, reorder, and buffer activation.

---

## The Plugin System

### PluginInterface.h

`src/plugins/PluginInterface.h` defines two API layers:

**1. Notepad--Qt native plugin API** (`NppQtPlugin` / `NppQtFuncs`):

```cpp
struct NppQtFuncs {
    const char* (*getCurrentText)(int* length);
    void (*setCurrentText)(const char* text, int length);
    int  (*getCurrentLine)();
    int  (*getCurrentColumn)();
    void (*setCursorPosition)(int line, int column);
    int  (*getBufferCount)();
    const char* (*getBufferPath)(int bufferId);
    void (*addToOutput)(const char* msg);
    void (*insertText)(const char* text);
    void (*replaceLine)(int line, const char* newText);
    void (*registerMenuItem)(const char* label, void (*callback)());
    void (*unregisterMenuItem)(const char* label);
    void (*setLanguage)(const char* langName);
    int  (*getVersion)();  // returns NPPQT_PLUGIN_API_VERSION
};

struct NppQtPlugin {
    int    apiVersion;           // Must be NPPQT_PLUGIN_API_VERSION (1)
    const char* pluginName;
    void (*init)(NppQtFuncs* funcs);
    void (*cleanup)();
};

#define NPPQT_PLUGIN(name, init_fn, cleanup_fn) \
    extern "C" NppQtPlugin nppqt_getPlugin() { \
        return NppQtPlugin{ ... }; }
```

**2. Legacy Notepad++ plugin compatibility** (stubs for porting existing NPP plugins):

```cpp
struct ShortcutKey { bool _isCtrl, _isAlt, _isShift; unsigned char _key; };
struct FuncItem { char _itemName[64]; void (*_pFunc)(); int _cmdID; ...; ShortcutKey* _pShKey; };
struct NppData { void* _nppHandle; void* _scintillaMainHandle; void* _scintillaSecondHandle; };

// Exports required by legacy NPP plugins
struct FuncItem* npp_getFunctionItems();
void npp_setInfo(NppData* nppData);
void npp_beNotified(void* notifyCode);
int  npp_messageProc(int message, int wParam, int lParam);
```

### PluginManager

`src/plugins/PluginManager.h`:

```
PluginManager
  _pluginDir   : QString           // ~/.config/notepad--qt/plugins/
  _plugins     : QVector<PluginInfo*>
  _pluginMap   : QMap<QString, PluginInfo*>
  _funcs       : NppQtFuncs        // function table handed to plugins
  _nppData     : NppData           // stub Notepad++ data
  _pluginMenu  : QMenu*

loadPlugins(pluginDir)
  → scanDirectory(pluginDir) for .so/.dll files
  → for each: QLibrary::load() → dlsym("nppqt_getPlugin") or dlsym("npp_getFunctionItems")
  → validatePlugin() — check apiVersion
  → plugin->init(&_funcs)         — plugin initializes with our API
  → register menu items in _pluginMenu
```

---

## The Panel System

Panels are `QDockWidget` subclasses, created lazily and toggled via View menu.

| Panel | Class | Lazy? | Key Features |
|-------|-------|-------|-------------|
| File Browser | `FileBrowserPanel` | Yes | `QTreeView` + `QFileSystemWatcher`, context menu |
| Function List | `FunctionListPanel` | Yes | Per-language regex parsing, `FunctionListXmlParser` (uses upstream NPP XML), collapsible tree, go-to on double-click |
| Document Map | `DocumentMapPanel` | Yes | Read-only `QsciScintilla` minimap, bidirectional scroll sync via `MapTextView` |
| Clipboard History | `ClipboardHistoryPanel` | Yes | Stack of clipboard entries, re-paste, clear history |

### Lazy Initialization

Panels are **not created** in `MainWindow` constructor. Instead, `Application::setupDockingPanels()` creates all four panels and parents them to `_mainWindow`, but their content is populated only on first activation via `setEditor()` / `refresh()`.

The `DocumentMapPanel` and `FunctionListPanel` use `QTimer::singleShot` debouncing to avoid redundant re-parsing on rapid buffer switches.

---

## The Encoding System

### EncodingType Enum (`Types.h`)

```cpp
enum class EncodingType : int {
    NONE         = -1,
    ANSI         = 0,
    UTF_8        = 1,
    UTF_8_BOM    = 2,
    UTF_16_LE    = 3,
    UTF_16_BE    = 4,
    UTF_32_LE    = 5,
    UTF_32_BE    = 6,
    EBCDIC       = 7,
    OEM          = 8,
    ASCII_7      = 9,
    UTF_16_LE_BOM = 10,
    UTF_16_BE_BOM = 11,
    ISO88591     = 12,
    Windows1252  = 13,
    Other        = 14,
};
```

### EncodingDetector (`src/core/EncodingDetector.h`)

```
EncodingDetector
  detect(bytes)                    → BOM check → UTF-8 validation → fallback
  detectBOM(bytes)                 → check first 4 bytes for BOM signature
  isValidUtf8(bytes)                → validate UTF-8 byte sequences
  detectSingleByteCharset(bytes)    → byte-frequency analysis (Latin-1, Windows-1252, etc.)
  detectFromMetaCharset(bytes)     → scan HTML/XML <meta charset="...">
  detectCJKLikelihood(bytes, hint) → statistical CJK detection
  toUtf8(bytes, from)              → convert any encoding to UTF-8
  fromUtf8(utf8, to)               → convert UTF-8 to target encoding
  detectEol(bytes)                 → count \n vs \r\n vs \r
```

### BOM Detection and Insertion

`EncodingDetector::detectBOM()` checks for these signatures:
- `\xEF\xBB\xBF` → `UTF_8_BOM`
- `\xFF\xFE` → `UTF_16_LE_BOM`
- `\xFE\xFF` → `UTF_16_BE_BOM`
- `\xFF\xFE\x00\x00` → `UTF_32_LE_BOM`
- `\x00\x00\xFE\xFF` → `UTF_32_BE_BOM`

On save, `NppIO` (`src/core/NppIO.cpp`) prepends the BOM bytes for `UTF_8_BOM`, `UTF_16_LE_BOM`, and `UTF_16_BE_BOM` encodings.

---

## Signal Flow

```
User input (key/menu/toolbar)
  → MenuBar / ToolBar emits commandInvoked(id)
      → Application::onMenuCommand(cmd)
          → Application::executeCommand(int commandId)
              → EditorCommandManager::dispatch(id, editor)
                  → ScintillaEditor::executeCommand(cmd, arg)
                      → QsciScintilla API
              OR  → Application or FileManager method
                  → Buffer / FileManager state change
                      → Buffer emits dirtyChanged / encodingChanged / languageChanged
                          → Application::onBufferChanged(BufferID)
                              → MainWindow::onBufferModified(...)
                                  → TabBar updates tab title (+ modified dot)
                                  → StatusBar updates encoding/language
                                  → FunctionListPanel refreshes (debounced)

File externally modified
  → QFileSystemWatcher emits fileChanged(path)
      → FileManager::_fileWatcherObserver.notify(FileNotification{path, ...})
          → Application::onExternalFileChanged(path)
              → FileReloadDialog prompts user
                  → Buffer::reload() or Buffer::setDeferredReload()

Theme change
  → Application::loadTheme(name)
      → Application::_themeObserver.notify(ThemeNotification{name})
          → ScintillaEditor::applyTheme(name) for all open editors
          → MainWindow stylesheet update
```

---

## Session Persistence

`SessionManager` (`src/core/SessionManager.h`) serializes to a JSON file at `~/.config/notepad--qt/session.json`:

```json
{
  "version": 1,
  "buffers": [
    { "path": "/home/user/foo.cpp", "encoding": "UTF-8", "lang": "cpp", "scrollLine": 42 },
    { "path": "/home/user/bar.py",  "encoding": "UTF-8-BOM", "lang": "python" }
  ],
  "activeTab": 1,
  "windowGeometry": { "x": 100, "y": 100, "w": 1200, "h": 800 },
  "panelVisibility": { "fileBrowser": true, "functionList": false, "docMap": true },
  "recentFiles": ["/home/user/old.txt", ...]
}
```

Saved on `Application::shutdown()` and `File → Save Session`. Restored in `Application::initialize()`.

---

## Editor Subsystem

### ScintillaEditor (`src/editor/ScintillaEditor.h`)

Wraps `QsciScintilla*` in a `QFrame`. Key responsibilities:

- Owns the `SyntaxHighlighter` (54 language definitions)
- Owns the `AutoCompletion` popup
- Exposes `send(message, wParam, lParam)` which delegates to `_editor->SendScintilla()`
- Tracks zoom level, bookmarks, language, encoding, EOL type

### DocTabView (`src/editor/DocTabView.h`)

`QTabWidget` subclass with:
- Tab pinning, color indicators, modified-dot support
- Drag-and-drop tab reordering
- Context menu (close, close others, close to left/right, pin, move left/right)
- Per-tab `TabData` map for auxiliary state

### ScintillaEditView (`src/editor/ScintillaEditView.h`)

Per-buffer view: owns a `ScintillaEditor*`, syncs with `Buffer` state. Handles split-view cloning.

### SyntaxHighlighter (`src/editor/SyntaxHighlighter.h`)

54 language definitions via `setup*()` methods. Each method configures lexer colors, keywords, comment styles. Language is selected by `LangType` enum mapping to `QsciLexer*` instances via `LexerConfig`.

### DocumentMapPanel (`src/panels/DocumentMapPanel.h`)

Uses `MapTextView` (a read-only `QsciScintilla`) as the minimap canvas. Scroll sync:
- Editor scrollbar → `onEditorScroll()` → update minimap first visible line
- Minimap click/drag → `onMiniMapClicked()` → jump editor to corresponding line
- A `QWidget` overlay paints the viewport region rectangle on top of the minimap

### FileLoaderWorker (`src/workers/FileLoaderWorker.h`)

`QThread` worker for async file I/O:
1. Opens file, reads in chunks (64 KB)
2. Reports progress via `loadingProgress(int)` signal
3. Detects encoding via `EncodingDetector::detect()`
4. On completion: `fileLoaded(success, bufferId, error)` signal
5. Cancelled if user closes the file before loading completes

---

## Testing

### Unit Tests (`src/tests/`)
Uses **Catch2** (`<catch2/catch.hpp>`):
- `test_buffer.cpp` — buffer creation, dirty flag, encoding, references
- `test_encoding_detector.cpp` — BOM detection, UTF-8 validation, EOL detection
- `test_string_helper.cpp` — trim, pad, word wrap, case conversion
- `test_util.cpp` — path normalisation, file helpers

### Integration/GUI Tests (`tests/`)
Uses **Qt Test** (`QTest`):
- `TestEncodingDetector.cpp`, `TestLexer.cpp`, `TestStringHelper.cpp`, `TestFileHelper.cpp`, `TestDialogs.cpp`, `TestStatusBar.cpp`, `startup_test.cpp`, `integration_test.cpp`, `integration_editor_test.cpp`

**Stubs** (`tests/stubs/TestStubs.cpp`): Provide minimal implementations of GUI components for headless testing (e.g., stub `QsciScintilla` where needed).

Run: `ctest --test-dir build --output-on-failure`

---

## Semantic Lift Reference

| Original (Win32) | Target (Qt) |
|-----------------|------------|
| WinMain / HWND | `QApplication` / `QMainWindow` |
| `SendMessage(hwnd, SCI_*)` | `QsciScintilla::SendScintilla(QsciScintilla::Command, long)` |
| `HDC` / GDI drawing | Qt paint system (`QPainter`) |
| `std::vector<Buffer>` | `std::vector<Buffer*>` managed by `FileManager` |
| INI files | `QSettings` (JSON or INI backend) |
| `GetPrivateProfileString` | `QSettings::value` |
| Win32 file APIs | `QFile`, `QTextStream`, `QFileDevice` |
| `ShellExecute` | `QDesktopServices::openUrl` |
| `CreateFile` | `QFile` with `QFileDevice::OpenMode` |
| `FindFiles` | `QDir` + `QDirIterator` |
| Win32 registry | `QSettings` user config path |

The Scintilla editing engine is provided by **QScintilla** (`QsciScintilla`), a Qt port of the same Scintilla component used by Notepad++.
