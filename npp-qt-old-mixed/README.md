# npp-qt — Notepad++ on Linux (Qt6 Port)

**Status:** Build passing · Tests mostly passing (127/130) · Translation in progress

## Overview

npp-qt is a port of [Notepad++](https://notepad-plus-plus.org/) from Win32/MFC to Qt6/C++ for Linux. The goal is a full-featured Notepad++ experience on Linux with feature parity to the Windows original.

- **Source:** Win32/MFC C++ (Notepad++ original)
- **Target:** Qt6 C++ (Linux native)
- **Build system:** CMake
- **Framework:** Qt6 with Scintilla editor widget

## Quick Start

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt install cmake build-essential qt6-base-dev qt6-base-private-dev \
  libqt6xmlpatterns6-dev qttools6-dev-tools

# Verify Qt6
qmake6 --version   # Should show Qt 6.x
cmake --version     # Should show CMake 3.x
```

### Build

```bash
# Clone (or use existing workspace)
git clone https://github.com/your-repo/npp-qt.git
cd npp-qt

# Configure and build
cmake -S . -B build
cmake --build build --parallel $(nproc)

# Run
./build/npp-qt
```

### Headless / CI Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
./build/npp-qt --help
```

### Run Tests

```bash
cd build/tests
export QT_QPA_PLATFORM=offscreen
export XDG_RUNTIME_DIR=/tmp/npp-runtime
mkdir -p $XDG_RUNTIME_DIR

# Run individual tests
./npp-test-utf8_16
./npp-test-dirwatch
./npp-test-darkmode
./npp-test-colour
./npp-test-lesdlgs
./npp-test-filedlg
./npp-test-iconset

# Or use ctest (requires consistent paths)
cd build && ctest --output-on-failure
```

## Current Status

| Component | Status | Notes |
|-----------|--------|-------|
| Build | ✅ Pass | Binary links and runs |
| Core editor | ✅ Pass | Scintilla widget functional |
| File I/O | ✅ Pass | UTF-8/16 BOM detection works |
| Directory watcher | ✅ Pass | 16 tests pass |
| Dark mode | ✅ Pass | 40 tests pass |
| Colour picker | ✅ Pass | 10 tests pass |
| LES dialogs | ✅ Pass | 9 tests pass |
| Custom file dialog | ✅ Pass | 15 tests pass |
| Image list/set | ✅ Pass | 30 tests pass |
| Encoding detection | ⚠️ 3 fail | Edge-case UTF-8/16 detection |
| Main window | 🔄 Partial | UI shell present, commands stubbed |
| Menu commands | 🔄 Partial | NppCommands partially translated |
| Notifications | 🔄 Partial | NppNotification stubbed |

## Project Structure

```
npp-qt/
├── src/                    # Qt6 source files (translated)
│   ├── MainWindow.cpp       # Main window + menu bar
│   ├── ScintillaComponent/ # Editor widget + dialogs
│   ├── WinControls/         # Qt widget wrappers
│   └── MISC/               # Utility code
├── tests/                   # QtTest unit tests
│   ├── TestUtf8_16.cpp     # Encoding detection tests
│   ├── TestDirectoryWatcher.cpp
│   └── ...
├── build/                   # CMake build output
├── SEMANTIC_LIFT_MAP.md    # Win32→Qt6 pattern reference
└── CMakeLists.txt          # Build configuration
```

## Key Win32 → Qt6 Mappings

| Win32 | Qt6 |
|-------|-----|
| `HWND` | `QWidget*` |
| `WM_PAINT` / `HDC` | `paintEvent()` + `QPainter` |
| `WM_COMMAND` | `QAction::triggered` signals |
| `CreateWindow()` | `QWidget` constructor + `show()` |
| `SetWindowText()` | `setWindowTitle()` |
| `SendMessage(hwnd, msg, w, l)` | `signal.emit()` / `slot()` call |
| `LRESULT CALLBACK` | `bool event(QEvent*)` override |
| `SetTimer()` | `QTimer` with `start()` / `stop()` |
| `GetDC()` / `ReleaseDC()` | `QPainter` with device lifetime |
| `InvalidateRect()` | `update()` |
| `LoadImage()` | `QIcon::fromTheme()` / `QPixmap()` |
| Registry (`RegOpenKeyEx`) | `QSettings` (INI format) |
| `CreateThread()` | `QThread` + `moveToThread()` |
| `strcpy()` / `sprintf()` | `QString` methods |

## What's Working

- Binary compiles and launches (`--help`, `--version`)
- Scintilla-based text editor widget
- UTF-8/16 BOM encoding detection (3 edge cases fail)
- Directory change monitoring
- Dark mode / theme support
- Colour picker dialog
- Custom file open/save dialogs
- Image resource management
- LES (line edit style) dialogs
- File encoding detection and conversion

## Known Issues

1. **NppCommands partial** — Menu command handlers partially stubbed; full translation in progress
2. **NppNotification stub** — Scintilla notification routing stubbed
3. **3 UTF-8 edge case failures** — Encoding detection for ASCII-ambiguous content
4. **No plugin system** — Plugins not yet ported
5. **No i18n** — Translation system not yet wired up

## Building on Remote Host

If building on a different host than the development environment:

```bash
ssh user@host
cd /path/to/npp-qt
cmake -S . -B build
cmake --build build --parallel $(nproc)
./build/npp-qt --help
```

## Contributing

Translation follows the Win32→Qt6 pattern map in `SEMANTIC_LIFT_MAP.md`. Key rules:

1. Every UI class with signals/slots needs `Q_OBJECT` macro
2. Use `#pragma once` instead of include guards
3. Subdirectory includes: `#include "SubDir/File.h"`
4. Win32 API calls → Qt equivalents (see mapping table)
5. After editing, rebuild and run tests before committing

## License

Same as Notepad++: GPL v3. See Notepad++ licensing terms.
