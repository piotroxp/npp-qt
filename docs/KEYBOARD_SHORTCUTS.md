# Keyboard Shortcuts Reference

Complete reference for Notepad--Qt keyboard shortcuts.

---

## File Operations

| Shortcut | Command | Notes |
|----------|---------|-------|
|  | New file | |
|  | Open file | |
|  | Save | |
|  | Save As | |
|  | Save a copy | |
|  | Close tab | |
|  | Close all tabs | |
|  | Close tab | Same as  |
|  | Reopen last closed tab | |
|  | Print | ⚠️ Not fully wired |
|  | Command palette | Opens fuzzy-searchable command list |

---

## Edit Operations

| Shortcut | Command | Notes |
|----------|---------|-------|
|  | Undo | |
|  | Redo | |
|  | Cut | |
|  | Copy | |
|  | Paste | |
|  | Select all | |
|  | Duplicate line | |
|  | Delete current line | |
|  | Delete selected lines | |
|  | Transpose current line | Swap with previous |
|  | Move line up | |
|  | Move line down | |
|  | Split selected lines | |
|  | Join lines | |
|  | Block comment | |
|  | Stream comment | |
|  | Indent | |
|  | Unindent | |
|  | Unindent | |
|  | Convert to lowercase | |
|  | Convert to uppercase | |
|  | Toggle line comment | |
|  | Insert blank line above | |
|  | Insert newline below | |
|  | Insert newline above | |
|  | Column selection | |
|  | Column selection | |

### Case Conversion

| Shortcut | Command |
|----------|---------|
|  | Lowercase |
|  | UPPERCASE |
|  | Title Case (proper case) |
| | Sentence case |
| | iNVERT cASE |
| | rAnDoM cAsE |

---

## Search & Replace

| Shortcut | Command |
|----------|---------|
|  | Find |
|  | Find / Replace |
|  | Find next |
|  | Find previous |
|  | Find next (selection) |
|  | Find previous (selection) |
|  | Incremental search |
|  | Select word under cursor |
|  | Find matching brace |
|  | Go to matching bracket |
|  | Select between matching brackets |
|  | Find in Files |
|  | Find in Files |

---

## Navigation

| Shortcut | Command | Notes |
|----------|---------|-------|
|  | Go to line | |
|  | Go to function (in Function List panel) | |
|  | Go to symbol (function) | |
|  | Go to reference | ⚠️ Not fully wired |
|  | Start of line | |
|  | End of line | |
|  | Start of document | |
|  | End of document | |
|  | Previous tab | |
|  | Next tab | |
|  | Switch to previous tab | |
|  | Switch to next tab | |
|  | Function parameter hint | |
|  | Auto-complete | |

---

## View

| Shortcut | Command |
|----------|---------|
|  | Toggle fullscreen |
|  | Zoom in/out |
|  | Reset zoom |
|  | Zoom to 1:1 |
|  | Zoom to 2:1 |
|  | Fit content to window width |
|  | Wrap to ruler |
|  | Toggle right-to-left |
|  | Toggle document map panel |
|  | Toggle function list panel |
|  | Toggle file browser panel |
|  | Toggle clipboard history panel |

---

## Encoding

| Shortcut | Command |
|----------|---------|
|  | Convert to UTF-8 (no BOM) |
|  | Convert to UTF-8 with BOM |
|  | Convert to ANSI |
|  | Convert to Windows-1252 |
|  | Convert to ISO-8859-1 |
|  | Show encoding in status bar |
|  | New file with encoding... |

---

## EOL (Line Endings)

| Shortcut | Command |
|----------|---------|
|  | Convert to Unix (LF) |
|  | Convert to Windows (CRLF) |
|  | Convert to Mac (CR) |
|  | Show EOL in status bar |

---

## Macro

| Shortcut | Command | Status |
|----------|---------|--------|
|  | Start/stop recording macro | ⚠️ Not wired |
|  | Play recorded macro | ⚠️ Not wired |
|  | Repeat last macro | ⚠️ Not wired |

---

## Tools

| Shortcut | Command | Status |
|----------|---------|--------|
|  | Run | |
|  | Run (custom 1) | |
|  | Run (custom 2) | |
|  | Run in background | |
|  | Command palette | |

---

## Plugins

| Shortcut | Command |
|----------|---------|
|  | Run plugin command... |
|  | Settings → Plugin Admin | ⚠️ Not implemented |

---

## Window

| Shortcut | Command | Notes |
|----------|---------|-------|
|  | Toggle splitter | |
|  | Move tab to previous view | |
|  | Move tab to next view | |
|  | Activate tab 0 | |
|  | Activate tab 1 | ... |
|  | Activate tab 9 | |
|  | Move tab to group 1 | |
|  | Move tab to group 2 | |
|  | New window | ⚠️ Not wired |
|  | Exit | |

---

## Settings

| Shortcut | Command | Notes |
|----------|---------|-------|
|  | Preferences | |
|  | Save all | |
|  | Open preferences | |
|  | Toggle toolbar | |
|  | Toggle menu bar | |
|  | Toggle word wrap | |
|  | Toggle line wrap | |
|  | Toggle line numbers | |
|  | Toggle all characters | |
|  | Toggle virtual space | |
|  | Toggle sync edit | |

---

## Desktop Environment Conflicts

Some shortcuts are intercepted by the host desktop environment before Qt sees them. These shortcuts may not work as expected:

| Shortcut | DE | Issue | Workaround |
|----------|-----|-------|-----------|
|  | All | Terminates X server | App does not use it |
|  | All | Window switcher | Use  for tab nav |
|  | All | Window switcher (reverse) | Use  |
|  | GNOME | Screenshot tool | Use  (settings) |
|  | KDE | Menu bar access | Use  (fullscreen) |
|  | GNOME | Workspace switch | Use  |
|  | GNOME | Application launcher | Use  (command palette) |
|  | KDE | krunner | Unavoidable on KDE; use menu bar |
|  | GNOME | Activities/launcher | Use  + arrow for window nav |

On **macOS**,  is  and  is . The shortcuts use the physical key positions.

---

## Customizing Shortcuts

Shortcuts are stored in .

Edit the file directly or use **Settings → Shortcut Mapper**.

To reset to defaults, delete  and restart.


