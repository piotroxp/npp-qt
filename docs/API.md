# Plugin API Reference

How to write a plugin for Notepad--Qt.

---

## Two API Layers

Notepad--Qt supports two plugin APIs:

| Layer | Purpose | Use this if... |
|-------|---------|----------------|
| **NppQt native** | Native Qt6 plugin API | Writing a new plugin from scratch |
| **Npp++ legacy** | Notepad++ plugin compatibility stubs | Porting an existing NPP plugin |

---

## Native Plugin API (`NppQtPlugin`)

### Overview

A plugin is a `.so` file (Linux) that exports a single function:

```cpp
extern "C" NppQtPlugin* nppqt_getPlugin();
```

Notepad--Qt loads the plugin via `QLibrary`, calls `init()`, and registers the plugin's menu items.

### Plugin Entry Point

```cpp
#include <NppQtPlugin>

struct NppQtFuncs {
    const char* (*getCurrentText)(int* length);
    void        (*setCurrentText)(const char* text, int length);
    int         (*getCurrentLine)();
    int         (*getCurrentColumn)();
    void        (*setCursorPosition)(int line, int column);
    int         (*getBufferCount)();
    const char* (*getBufferPath)(int bufferId);
    void        (*addToOutput)(const char* msg);
    void        (*insertText)(const char* text);
    void        (*replaceLine)(int line, const char* newText);
    void        (*registerMenuItem)(const char* label, void (*callback)());
    void        (*unregisterMenuItem)(const char* label);
    void        (*setLanguage)(const char* langName);
    int         (*getVersion)();  // returns NPPQT_PLUGIN_API_VERSION
};

struct NppQtPlugin {
    int    apiVersion;           // Must be NPPQT_PLUGIN_API_VERSION (1)
    const char* pluginName;       // Display name in the Plugins menu
    void (*init)(NppQtFuncs* funcs);     // Called once on load
    void (*cleanup)();                     // Called once on unload
};

// Macro to define the entry point
#define NPPQT_PLUGIN(name, init_fn, cleanup_fn) \
    extern "C" NppQtPlugin* nppqt_getPlugin() { \
        return NppQtPlugin{1, name, init_fn, cleanup_fn}; }
```

### Example Plugin

Minimal plugin that inserts a timestamp:

```cpp
// myplugin.cpp
#include "NppQtPlugin"

static NppQtFuncs* g_funcs = nullptr;

void insertTimestamp() {
    QDateTime now = QDateTime::currentDateTime();
    QString ts = now.toString(Qt::ISODate);
    g_funcs->insertText(ts.toUtf8().constData());
}

void init(NppQtFuncs* funcs) {
    g_funcs = funcs;
    funcs->registerMenuItem("Insert Timestamp", insertTimestamp);
    funcs->addToOutput("myplugin loaded\n");
}

void cleanup() {
    g_funcs->unregisterMenuItem("Insert Timestamp");
    g_funcs->addToOutput("myplugin unloaded\n");
}

NPPQT_PLUGIN("My Plugin", init, cleanup)
```

Build:
```bash
# Compile as a shared library
g++ -fPIC -shared -I/path/to/npp-qt/src/plugins myplugin.cpp -o libmyplugin.so
# Install to plugin directory
cp libmyplugin.so ~/.config/notepad--qt/plugins/
```

### Available Functions (NppQtFuncs)

| Function | Returns | Description |
|----------|---------|-------------|
| `getCurrentText(int* length)` | `const char*` | Get the full text of the active editor. `length` is set to the byte count. Do not free. |
| `setCurrentText(text, length)` | `void` | Replace all text in the active editor. |
| `getCurrentLine()` | `int` | Zero-based line number of cursor. |
| `getCurrentColumn()` | `int` | Zero-based column number of cursor. |
| `setCursorPosition(line, column)` | `void` | Move cursor to line/column (zero-based). |
| `getBufferCount()` | `int` | Number of open buffers. |
| `getBufferPath(bufferId)` | `const char*` | Full path of buffer `bufferId`. Returns `""` for unnamed buffers. |
| `addToOutput(msg)` | `void` | Append `msg` to the output panel. |
| `insertText(text)` | `void` | Insert `text` at the current cursor position. |
| `replaceLine(line, text)` | `void` | Replace the content of line `line` (zero-based) with `text`. |
| `registerMenuItem(label, callback)` | `void` | Add a menu item under the Plugins menu. `callback` is called when the item is triggered. |
| `unregisterMenuItem(label)` | `void` | Remove a menu item. Call from `cleanup()`. |
| `setLanguage(langName)` | `void` | Set the language of the active buffer. `langName` matches NPP language names (e.g., `"C++"`, `"Python"`, `"JavaScript"`). |
| `getVersion()` | `int` | Returns `NPPQT_PLUGIN_API_VERSION` (currently `1`). |

---

## Legacy Notepad++ Plugin API (Stubs)

Notepad--Qt provides stub implementations of the Notepad++ plugin interface so existing NPP plugins can be compiled with minimal changes. This is a **compatibility layer**, not a fully functional NPP implementation.

### Required Exports

Legacy plugins must export these symbols:

```cpp
struct FuncItem {
    char        _itemName[64];   // Menu item label
    void        (*_pFunc)();     // Callback function
    int         _cmdID;          // Command ID (assigned by NPP)
    ShortcutKey* _pShKey;        // Keyboard shortcut (can be nullptr)
    bool        _init2Check;     // Checked on init?
};

struct ShortcutKey {
    bool    _isCtrl;
    bool    _isAlt;
    bool    _isShift;
    unsigned char _key;          // Virtual key code
};

struct NppData {
    void*   _nppHandle;         // NPP instance handle
    void*   _scintillaMainHandle;  // Main editor handle
    void*   _scintillaSecondHandle; // Secondary editor handle (split view)
};

// Required exports
FuncItem*    npp_getFunctionItems();     // Returns array of menu items
void         npp_setInfo(NppData*);       // Called with NPP data
void         npp_beNotified(void* notifyCode); // Called on NPP notifications
int          npp_messageProc(int message, int wParam, int lParam); // Window message proc
```

### Key Differences from Real NPP

| NPP API | Notepad--Qt Status |
|---------|-------------------|
| `npp_getFunctionItems()` | ✅ Returns function list |
| `npp_setInfo()` | ✅ Called with `NppData` |
| `npp_beNotified()` | ⚠️ Stub — notification constants exist but handler is minimal |
| `NppNotification` structs | ⚠️ Stub — only a subset of NPP notifications are forwarded |
| `SCI_*` messages | ⚠️ Via `scintillaMainHandle` / `scintillaSecondHandle` but not fully wired |
| `NppMenuCmd` IDs | ⚠️ Subset — only `IDM_*` IDs that map to Qt equivalents |
| `pluginCommand` | ✅ Via `_pFunc` callback |
| `_pShKey` shortcuts | ⚠️ Partially registered |

---

## Plugin Lifecycle

```
App startup
  → PluginManager::loadPlugins()
      → scan ~/.config/notepad--qt/plugins/ for .so/.dll files
      → QLibrary::load() each file
      → dlsym("nppqt_getPlugin")  (try native first)
      → dlsym("npp_getFunctionItems")  (try NPP legacy)
      → plugin->init(&_funcs)
      → for each FuncItem: add menu item to Plugins menu

User triggers plugin menu item
  → _pFunc() callback fires

App shutdown
  → PluginManager::unloadPlugins()
      → plugin->cleanup()
      → QLibrary::unload()
```

---

## Plugin Directory

Default: `~/.config/notepad--qt/plugins/`

Override: `./NotepadMinusMinusQt -p /path/to/plugins/`

Only `.so` files (Linux) are loaded. Files with other extensions are ignored.

---

## Plugin Development Tips

### Debugging plugins

```bash
# Run without plugins first to confirm the app works
./NotepadMinusMinusQt --no-plugins

# Use GDB to debug
gdb ./NotepadMinusMinusQt
(gdb) run --no-plugins
(gdb) # set breakpoints in plugin code
```

### Testing plugin loading

```bash
# Check what plugins are loaded
./NotepadMinusMinusQt 2>&1 | grep -i plugin
```

### Plugin config

Plugins can store config in `~/.config/notepad--qt/plugins/<plugin-name>/`.

---

## Status of the Plugin System

The plugin system is **partially implemented**. Key gaps:

- `npp_beNotified()` — only a subset of NPP notifications forwarded
- `NppNotification` — most notification types not handled
- Plugin communication (plugins calling back into NPP) — limited
- `NPPDM_*` docking messages — not wired

See the semantic lift audit in [HEARTBEAT.md](../HEARTBEAT.md) for the full status.
