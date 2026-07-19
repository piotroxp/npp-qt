# Command-Line Reference

Complete reference for Notepad--Qt command-line arguments, environment variables, and IPC.

---

## Usage

```bash
NotepadMinusMinusQt [options] [files...]

# Examples
NotepadMinusMinusQt file.txt
NotepadMinusMinusQt -r file.txt              # read-only
NotepadMinusMinusQt -s session.json          # load session
NotepadMinusMinusQt --profile dark file.cpp  # dark theme
NotepadMinusMinusQt --no-plugins            # no plugins
```

---

## Options

### `-h`, `--help`
Show the built-in help text and exit.

### `-r`, `--read-only`
Open all files in read-only mode. The UI disables Save (`Ctrl+S`) and all modifications are discarded on close.

### `-n`, `--new-instance`
Always open a new application instance, even if an existing instance is running. By default, Notepad--Qt uses a singleton lock and sends files to the running instance.

### `--no-plugins`
Load the application without loading any plugins. Useful for diagnosing plugin-related crashes:
```bash
./build/NotepadMinusMinusQt --no-plugins file.txt
```

### `-c DIR`, `--config DIR`
Use `DIR` as the configuration directory instead of `~/.config/notepad--qt/`.
```bash
./build/NotepadMinusMinusQt -c /tmp/npp-test-config file.txt
# Config goes to /tmp/npp-test-config/
```

### `-p DIR`, `--plugin-dir DIR`
Override the default plugin directory (`~/.config/notepad--qt/plugins/`).
```bash
./build/NotepadMinusMinusQt -p /opt/npp-plugins/ file.txt
```

### `-s FILE`, `--session FILE`
Load a session file on startup. The session JSON must conform to the format in [docs/ARCHITECTURE.md](ARCHITECTURE.md#session-persistence).
```bash
./build/NotepadMinusMinusQt -s ~/my-session.json
```

### `--profile NAME`
Apply a named theme profile at startup. `NAME` must match a file in `~/.config/notepad--qt/themes/` (without the `.json` extension).
```bash
./build/NotepadMinusMinusQt --profile dark   # dark theme
./build/NotepadMinusMinusQt --profile nord   # Nord theme
./build/NotepadMinusMinusQt --profile monokai # Monokai theme
```

---

## Environment Variables

### `QT_QPA_PLATFORM`
Selects the Qt platform plugin. Must be set **before** `QApplication` construction.

| Value | Platform |
|-------|----------|
| `offscreen` | Headless (no display) |
| `xcb` | X11 (default on Linux) |
| `wayland` | Wayland |
| `windows` | Windows |

```bash
# Headless (for CI, server, or headless debugging)
QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt file.txt

# Explicit X11
QT_QPA_PLATFORM=xcb ./build/NotepadMinusMinusQt file.txt
```

### `QT_QPA_PLATFORM_THEME`
Overrides the Qt platform theme (e.g., GTK, Fusion, Breeze).

```bash
QT_QPA_PLATFORM_THEME=breeze ./build/NotepadMinusMinusQt
```

### `DISPLAY`
Used by Qt to connect to the X11 server. Required for GUI mode on a remote machine:
```bash
DISPLAY=:0 ./build/NotepadMinusMinusQt file.txt
```

### `XDG_CONFIG_HOME`
Overrides the base XDG config directory. Notepad--Qt stores config under `XDG_CONFIG_HOME/notepad--qt/`:
```bash
XDG_CONFIG_HOME=/tmp/config ./build/NotepadMinusMinusQt
# Config at /tmp/config/notepad--qt/
```

---

## Exit Codes

| Code | Meaning |
|------|---------|
| `0` | Normal exit |
| `1` | Error (file not found, bad arguments, init failure) |
| `134` (SIGABRT) | Unhandled exception (ASAN builds catch this) |
| `139` (SIGSEGV) | Crash |

Exit code is useful in scripts:
```bash
./build/NotepadMinusMinusQt file.txt
if [ $? -eq 0 ]; then
    echo "Opened successfully"
fi
```

---

## Session File Format

The session JSON format (`-s` / `--session`):

```json
{
  "version": 1,
  "buffers": [
    {
      "path": "/home/user/foo.cpp",
      "encoding": "UTF-8",
      "lang": "cpp",
      "scrollLine": 42,
      "cursorLine": 12,
      "cursorCol": 8
    },
    {
      "path": "/home/user/bar.py",
      "encoding": "UTF-8-BOM",
      "lang": "python"
    }
  ],
  "activeTab": 0,
  "windowGeometry": { "x": 100, "y": 100, "w": 1200, "h": 800 },
  "panelVisibility": {
    "fileBrowser": true,
    "functionList": false,
    "docMap": true,
    "clipboardHistory": false
  },
  "recentFiles": [
    "/home/user/old.txt",
    "/home/user/notes.md"
  ]
}
```

All fields are optional. Missing fields use defaults (empty geometry → centre on screen; missing panels → hidden).
