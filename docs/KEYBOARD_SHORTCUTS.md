# Keyboard Shortcuts Reference

Complete reference for Notepad--Qt keyboard shortcuts.

---

## File Operations

| Shortcut | Command | Notes |
|----------|---------|-------|
| `Ctrl+N` | New file | |
| `Ctrl+O` | Open file | |
| `Ctrl+S` | Save | |
| `Ctrl+Shift+S` | Save As | |
| `Ctrl+Alt+S` | Save a copy | |
| `Ctrl+W` | Close tab | |
| `Ctrl+Shift+W` | Close all tabs | |
| `Ctrl+F4` | Close tab | Same as `Ctrl+W` |
| `Ctrl+Shift+T` | Reopen last closed tab | |
| `Ctrl+P` | Print | ⚠️ Not fully wired |
| `Ctrl+Shift+P` | Command palette | Opens fuzzy-searchable command list |

---

## Edit Operations

| Shortcut | Command | Notes |
|----------|---------|-------|
| `Ctrl+Z` | Undo | |
| `Ctrl+Y` | Redo | |
| `Ctrl+X` | Cut | |
| `Ctrl+C` | Copy | |
| `Ctrl+V` | Paste | |
| `Ctrl+A` | Select all | |
| `Ctrl+D` | Duplicate line | |
| `Ctrl+L` | Delete current line | |
| `Ctrl+Shift+L` | Delete selected lines | |
| `Ctrl+T` | Transpose current line | Swap with previous |
| `Ctrl+Shift+Up` | Move line up | |
| `Ctrl+Shift+Down` | Move line down | |
| `Ctrl+I` | Split selected lines | |
| `Ctrl+J` | Join lines | |
| `Ctrl+Alt+B` | Block comment | |
| `Ctrl+Shift+Q` | Stream comment | |
| `Tab` | Indent | |
| `Shift+Tab` | Unindent | |
| `Ctrl+Backtab` | Unindent | |
| `Ctrl+U` | Convert to lowercase | |
| `Ctrl+Shift+U` | Convert to uppercase | |
| `Ctrl+Q` | Toggle line comment | |
| `Ctrl+Shift+K` | Insert blank line above | |
| `Ctrl+Enter` | Insert newline below | |
| `Ctrl+Shift+Enter` | Insert newline above | |
| `Alt+Shift+Arrow` | Column selection | |
| `Alt+Mouse drag` | Column selection | |

### Case Conversion

| Shortcut | Command |
|----------|---------|
| `Ctrl+U` | Lowercase |
| `Ctrl+Shift+U` | UPPERCASE |
| `Ctrl+Alt+U` | Title Case (proper case) |
| | Sentence case |
| | iNVERT cASE |
| | rAnDoM cAsE |

---

## Search & Replace

| Shortcut | Command |
|----------|---------|
| `Ctrl+F` | Find |
| `Ctrl+H` | Find / Replace |
| `F3` | Find next |
| `Shift+F3` | Find previous |
| `Ctrl+F3` | Find next (selection) |
| `Ctrl+Shift+F3` | Find previous (selection) |
| `Ctrl+Shift+I` | Incremental search |
| `Ctrl+D` | Select word under cursor |
| `Alt+Shift+E` | Find matching brace |
| `Ctrl+]` | Go to matching bracket |
| `Ctrl+Shift+]` | Select between matching brackets |
| `Ctrl+Alt+I` | Find in Files |
| `Ctrl+Shift+F` | Find in Files |

---

## Navigation

| Shortcut | Command | Notes |
|----------|---------|-------|
| `Ctrl+G` | Go to line | |
| `Ctrl+P` | Go to function (in Function List panel) | |
| `Ctrl+R` | Go to symbol (function) | |
| `Ctrl+Shift+R` | Go to reference | ⚠️ Not fully wired |
| `Home` | Start of line | |
| `End` | End of line | |
| `Ctrl+Home` | Start of document | |
| `Ctrl+End` | End of document | |
| `Ctrl+PageUp` | Previous tab | |
| `Ctrl+PageDown` | Next tab | |
| `Ctrl+Tab` | Switch to previous tab | |
| `Ctrl+Shift+Tab` | Switch to next tab | |
| `Ctrl+Shift+Space` | Function parameter hint | |
| `Ctrl+Space` | Auto-complete | |

---

## View

| Shortcut | Command |
|----------|---------|
| `F11` | Toggle fullscreen |
| `Ctrl+Wheel` | Zoom in/out |
| `Ctrl+0` | Reset zoom |
| `Ctrl+Shift+1` | Zoom to 1:1 |
| `Ctrl+Shift+2` | Zoom to 2:1 |
| `Ctrl+Shift+Fit Width` | Fit content to window width |
| `Ctrl+Shift+3` | Wrap to ruler |
| `Ctrl+Alt+R` | Toggle right-to-left |
| `View → Document Map` | Toggle document map panel |
| `View → Function List` | Toggle function list panel |
| `View → File Browser` | Toggle file browser panel |
| `View → Clipboard History` | Toggle clipboard history panel |

---

## Encoding

| Shortcut | Command |
|----------|---------|
| `Ctrl+Shift+I` | Convert to UTF-8 (no BOM) |
| `Ctrl+Alt+Shift+I` | Convert to UTF-8 with BOM |
| `Ctrl+Alt+Shift+C` | Convert to ANSI |
| `Ctrl+Shift+G` | Convert to Windows-1252 |
| `Ctrl+Alt+Shift+G` | Convert to ISO-8859-1 |
| `Ctrl+Shift+E` | Show encoding in status bar |
| `Ctrl+Shift+N` | New file with encoding... |

---

## EOL (Line Endings)

| Shortcut | Command |
|----------|---------|
| `Ctrl+Alt+Shift+P` | Convert to Unix (LF) |
| `Ctrl+Alt+Shift+D` | Convert to Windows (CRLF) |
| `Ctrl+Alt+Shift+M` | Convert to Mac (CR) |
| `Ctrl+Alt+Shift+S` | Show EOL in status bar |

---

## Macro

| Shortcut | Command | Status |
|----------|---------|--------|
| `Ctrl+Shift+R` | Start/stop recording macro | ⚠️ Not wired |
| `Ctrl+Shift+P` | Play recorded macro | ⚠️ Not wired |
| `Ctrl+Alt+Shift+R` | Repeat last macro | ⚠️ Not wired |

---

## Tools

| Shortcut | Command | Status |
|----------|---------|--------|
| `F5` | Run | |
| `Ctrl+Shift+5` | Run (custom 1) | |
| `Ctrl+Alt+Shift+5` | Run (custom 2) | |
| `Ctrl+Alt+Shift+B` | Run in background | |
| `Ctrl+Alt+Shift+7` | Command palette | |

---

## Plugins

| Shortcut | Command |
|----------|---------|
| `Alt+Shift+F` | Run plugin command... |
| `Ctrl+Alt+Shift+P` | Settings → Plugin Admin | ⚠️ Not implemented |

---

## Window

| Shortcut | Command | Notes |
|----------|---------|-------|
| `Ctrl+Alt+S` | Toggle splitter | |
| `Ctrl+Alt+Left` | Move tab to previous view | |
| `Ctrl+Alt+Right` | Move tab to next view | |
| `Alt+0` | Activate tab 0 | |
| `Alt+1` | Activate tab 1 | ... |
| `Alt+9` | Activate tab 9 | |
| `Alt+Shift+0` | Move tab to group 1 | |
| `Alt+Shift+1` | Move tab to group 2 | |
| `Ctrl+Shift+N` | New window | ⚠️ Not wired |
| `Alt+F4` | Exit | |

---

## Settings

| Shortcut | Command | Notes |
|----------|---------|-------|
| `Ctrl+Alt+Shift+P` | Preferences | |
| `Ctrl+Shift+S` | Save all | |
| `Ctrl+Alt+Shift+O` | Open preferences | |
| `Ctrl+Alt+Shift+T` | Toggle toolbar | |
| `Ctrl+Alt+Shift+M` | Toggle menu bar | |
| `Ctrl+Alt+Shift+W` | Toggle word wrap | |
| `Ctrl+Alt+Shift+L` | Toggle line wrap | |
| `Ctrl+Alt+Shift+N` | Toggle line numbers | |
| `Ctrl+Alt+Shift+E` | Toggle all characters | |
| `Ctrl+Alt+Shift+V` | Toggle virtual space | |
| `Ctrl+Alt+Shift+A` | Toggle sync edit | |

---

## Desktop Environment Conflicts

Some shortcuts are intercepted by the host desktop environment before Qt sees them. These shortcuts may not work as expected:

| Shortcut | DE | Issue | Workaround |
|----------|-----|-------|-----------|
| `Ctrl+Alt+Backspace` | All | Terminates X server | App does not use it |
| `Alt+Tab` | All | Window switcher | Use `Ctrl+Tab` for tab nav |
| `Alt+Shift+Tab` | All | Window switcher (reverse) | Use `Ctrl+Shift+Tab` |
| `PrintScreen` | GNOME | Screenshot tool | Use `Ctrl+Alt+P` (settings) |
| `F10` | KDE | Menu bar access | Use `F11` (fullscreen) |
| `Ctrl+Alt+Arrow` | GNOME | Workspace switch | Use `Ctrl+PageUp/PageDown` |
| `Alt+F2` | GNOME | Application launcher | Use `Ctrl+Shift+P` (command palette) |
| `Alt+Space` | KDE | krunner | Unavoidable on KDE; use menu bar |
| `Meta` | GNOME | Activities/launcher | Use `Super` + arrow for window nav |

On **macOS**, `Alt` is `Option` and `Ctrl` is `Control`. The shortcuts use the physical key positions.

---

## Customizing Shortcuts

Shortcuts are stored in `~/.config/notepad--qt/shortcuts.json`.

Edit the file directly or use **Settings → Shortcut Mapper**.

To reset to defaults, delete `shortcuts.json` and restart.

```json
{
  "shortcuts": [
    { "command": "IDM_FILE_NEW", "shortcut": "Ctrl+N" },
    { "command": "IDM_EDIT_UNDO", "shortcut": "Ctrl+Z" }
  ]
}
```
