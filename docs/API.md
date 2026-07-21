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

## Native Plugin API ()

### Overview

A plugin is a  file (Linux) that exports a single function:



Notepad--Qt loads the plugin via , calls , and registers the plugin's menu items.

### Plugin Entry Point



### Example Plugin

Minimal plugin that inserts a timestamp:



Build:


### Available Functions (NppQtFuncs)

| Function | Returns | Description |
|----------|---------|-------------|
|  |  | Get the full text of the active editor.  is set to the byte count. Do not free. |
|  |  | Replace all text in the active editor. |
|  |  | Zero-based line number of cursor. |
|  |  | Zero-based column number of cursor. |
|  |  | Move cursor to line/column (zero-based). |
|  |  | Number of open buffers. |
|  |  | Full path of buffer . Returns  for unnamed buffers. |
|  |  | Append  to the output panel. |
|  |  | Insert  at the current cursor position. |
|  |  | Replace the content of line  (zero-based) with . |
|  |  | Add a menu item under the Plugins menu.  is called when the item is triggered. |
|  |  | Remove a menu item. Call from . |
|  |  | Set the language of the active buffer.  matches NPP language names (e.g., , , ). |
|  |  | Returns  (currently ). |

---

## Legacy Notepad++ Plugin API (Stubs)

Notepad--Qt provides stub implementations of the Notepad++ plugin interface so existing NPP plugins can be compiled with minimal changes. This is a **compatibility layer**, not a fully functional NPP implementation.

### Required Exports

Legacy plugins must export these symbols:



### Key Differences from Real NPP

| NPP API | Notepad--Qt Status |
|---------|-------------------|
|  | ✅ Returns function list |
|  | ✅ Called with  |
|  | ⚠️ Stub — notification constants exist but handler is minimal |
|  structs | ⚠️ Stub — only a subset of NPP notifications are forwarded |
|  messages | ⚠️ Via  /  but not fully wired |
|  IDs | ⚠️ Subset — only  IDs that map to Qt equivalents |
|  | ✅ Via  callback |
|  shortcuts | ⚠️ Partially registered |

---

## Plugin Lifecycle



---

## Plugin Directory

Default: 

Override: 

Only  files (Linux) are loaded. Files with other extensions are ignored.

---

## Plugin Development Tips

### Debugging plugins



### Testing plugin loading



### Plugin config

Plugins can store config in .

---

## Status of the Plugin System

The plugin system is **partially implemented**. Key gaps:

-  — only a subset of NPP notifications forwarded
-  — most notification types not handled
- Plugin communication (plugins calling back into NPP) — limited
-  docking messages — not wired

See the semantic lift audit in [HEARTBEAT.md](../HEARTBEAT.md) for the full status.

---

## Application Command Reference

Commands are dispatched via `Application::onMenuCommand(const QString& cmd)`.
Each command is a dot-separated string, e.g. `"file.save"`.

### File Commands

| Command | Method | Description |
|---------|--------|-------------|
| `file.closeAll_BUT_THIS` | `closeAllFilesButCurrent()` | Close all open buffers except the active one. Prompts to save modified files. |
| `file.saveAsCopy` | `onSaveAsCopy()` | Save a copy of the active buffer to a new path. Does not change the active buffer's path. |

### Edit Commands

| Command | Method | Description |
|---------|--------|-------------|
| `edit.copyToClipboard` | `onCopyToNamedClipboard()` | Copy the current selection to a named clipboard. Prompts for a name. |
| `edit.moveToClipboard` | `onMoveToNamedClipboard()` | Cut the current selection to a named clipboard. Prompts for a name. |
| `edit.pasteFromClipboard` | `onPasteFromNamedClipboard()` | Paste from a named clipboard. Shows a list of stored clips. |

Named clips are stored in `QMap<QString, QString> _namedClips` and persist for the session.

### Search Commands

| Command | Method | Description |
|---------|--------|-------------|
| `search.replaceInFiles` | `onReplaceInFiles()` | Replace text across multiple files. Stub — shows "working in progress". |
| `search.findInProjects` | `onFindInProjects()` | Search within the current workspace/project. Stub — shows "working in progress". |
| `search.replaceInProjects` | `onReplaceInProjects()` | Replace text within the current workspace/project. Stub — shows "working in progress". |
