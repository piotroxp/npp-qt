# Getting Started with Notepad--Qt

A quick-start guide for developers and power users getting up and running with Notepad--Qt.

---

## TL;DR

```bash
# 1. Install dependencies (Ubuntu/Debian)
sudo apt install qt6-base-dev libqscintilla2-qt6-dev cmake build-essential

# 2. Clone and build
git clone https://github.com/piotroxp/npp-qt.git
cd npp-qt
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# 3. Run
./build/NotepadMinusMinusQt file.txt

# 4. Run headless (no display required)
QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt --help
```

---

## Installation

### Requirements

| Dependency | Minimum | Recommended | Package (Ubuntu) |
|------------|---------|-------------|-----------------|
| Qt6 | 6.4 | 6.5+ | `qt6-base-dev` |
| QScintilla2 | 2.14 | 2.17+ | `libqscintilla2-qt6-dev` |
| CMake | 3.20 | 3.25+ | `cmake` |
| C++ compiler | GCC 11 | GCC 13 | `build-essential` |
| Ninja | — | latest | `ninja-build` |

### Platform-Specific Installers

**Arch Linux:**
```bash
sudo pacman -S qt6-base qscintilla cmake base-devel ninja
```

**macOS (Homebrew):**
```bash
brew install qt6 qscintilla2 cmake ninja
```

### Pre-built Binary

If you're on the prizm-laptop, a pre-built binary is at:
```
/home/piotro/npp-qt-fix-build/NotepadMinusMinusQt
```

---

## Building from Source

### Standard Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build -j$(nproc)
```

### Debug Build (with sanitizers)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DNPP_DEBUG_SANITIZERS=ON -G Ninja
cmake --build build -j$(nproc)

# Run under GDB
gdb ./build/NotepadMinusMinusQt
# (gdb) run [args]
# (gdb) bt   ← get backtrace on crash

# Run under Valgrind (memory leaks)
valgrind --leak-check=full ./build/NotepadMinusMinusQt
```

### Build Directories

The repo contains several pre-existing build directories:

| Directory | Purpose |
|-----------|---------|
| `build/` | Standard release build |
| `build-asan/` | Debug build with AddressSanitizer |
| `build_fresh/` | Clean build for testing |
| `build_nppqt/` | Alternate build |
| `b/` | Unknown — check before using |

### Build Errors

**Missing `qscintilla2`:**
```
Could not find qscintilla2
```
Fix: `sudo apt install libqscintilla2-qt6-dev`

**Qt6 not found:**
```
Could not find Qt6
```
Fix: `sudo apt install qt6-base-dev qt6-tools-dev`

**Ninja not found:** `-G Ninja` requires `ninja-build`. Omit `-G Ninja` to use Make.

---

## Running

### Normal Launch
```bash
./build/NotepadMinusMinusQt
./build/NotepadMinusMinusQt file.cpp file2.py
```

### Headless (CI/Server)
```bash
QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt file.txt
```
No display required. Useful for syntax checking, encoding detection, or running tests.

### With Display (explicit)
```bash
QT_QPA_PLATFORM=xcb ./build/NotepadMinusMinusQt file.txt
# or
QT_QPA_PLATFORM=wayland ./build/NotepadMinusMinusQt file.txt
```

---

## Configuration

Config is stored in `~/.config/notepad--qt/`:

| File | Purpose |
|------|---------|
| `config.json` | General settings (recent files, UI preferences) |
| `shortcuts.json` | Keyboard shortcut bindings |
| `session.json` | Last session (open files, window geometry) |
| `themes/` | Theme JSON files (dark, monokai, nord, etc.) |
| `plugins/` | Plugin `.so` files |
| `udl/` | User-defined language definitions |

### Custom Config Directory
```bash
./build/NotepadMinusMinusQt -c /path/to/config/dir file.txt
```

### Custom Plugin Directory
```bash
./build/NotepadMinusMinusQt -p /path/to/plugins/ file.txt
```

---

## Command-Line Options

See [docs/COMMAND_LINE.md](COMMAND_LINE.md) for the full reference.

Quick reference:
```
-h, --help              Show help
-r, --read-only        Open files read-only
-n, --new-instance     Always open a new instance
--no-plugins           Load without plugins
-c DIR, --config DIR   Use DIR as config directory
-p DIR, --plugin-dir DIR  Set plugin directory
-s FILE, --session FILE  Load session from FILE
--profile NAME         Use theme profile (dark/light)
```

---

## Testing

```bash
# Build and run all tests
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure

# Run specific test suites
ctest --test-dir build -R buffer --output-on-failure
ctest --test-dir build -R encoding --output-on-failure
ctest --test-dir build -R syntax --output-on-failure

# Verbose output
ctest --test-dir build -V
```

### Test Suites

| Suite | What it tests |
|-------|--------------|
| `EncodingDetectorTests` | BOM detection, UTF-8 validation, chardet fallback |
| `StringHelperTests` | trim, replace, escape, spacesToTabs, tabsToSpaces, fileExt |
| `BufferTests` | Buffer lifecycle, load/save, LineVector |
| `SyntaxHighlighterTests` | 40+ languages, theme system, custom rules |
| `ShortcutAdapterTests` | RegisterHotKey→QShortcut, register/unregister, conflicts |

---

## First Steps

1. **Open a file:** `File → Open` or `Ctrl+O`
2. **New file:** `File → New` or `Ctrl+N`
3. **Change language:** `Language → C++`, `Language → Python`, etc.
4. **Toggle panels:** `View → Document Map`, `View → Function List`
5. **Find/Replace:** `Ctrl+F` / `Ctrl+H`
6. **Command palette:** `Ctrl+Shift+P` (opens fuzzy-searchable command list)
7. **Preferences:** `Settings → Preferences`

---

## Troubleshooting

See [docs/TROUBLESHOOTING.md](TROUBLESHOOTING.md) for common issues.

Quick fixes:

**Crashes on startup (with display):**
```bash
# Run with GDB to get a backtrace
gdb ./build/NotepadMinusMinusQt
(gdb) run
(gdb) bt full
```

**Crashes on startup (headless):**
```bash
# Use ASAN build
cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug -DNPP_DEBUG_SANITIZERS=ON
cmake --build build-asan
QT_QPA_PLATFORM=offscreen ./build-asan/NotepadMinusMinusQt
```

**Shortcut not working:**
Some shortcuts are intercepted by the desktop environment. See the keyboard shortcuts table in [README.md](../README.md).
