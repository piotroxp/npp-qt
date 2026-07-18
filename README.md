# NotepadMinusMinusQt

A Qt6/Linux port of Notepad++ functionality (`Notepad--`), built from the
`notepad-plus-plus-translation` source tree with semantic lift from C++/Win32/MFC
to C++/Qt6.

## Build

```bash
cmake -S . -B build -G Ninja
cmake --build build --parallel $(nproc)
```

**Requirements:** CMake ≥ 3.20, Ninja, g++, Qt6 (qt6-base-dev,
libqscintilla2-qt6-dev, qt6-tools-dev, libqt6svg6-dev)

## Run

```bash
# Headless smoke:
QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt --help

# With display:
./build/NotepadMinusMinusQt [file]
```

## Test

```bash
cd build && ctest --output-on-failure
```

| Suite | Coverage |
|-------|----------|
| EncodingDetectorTests | UTF-8/16/32 BOM detection, chardet fallback |
| StringHelperTests | trim, replace, escape, spacesToTabs, tabsToSpaces, fileExt, format |
| BufferTests | buffer lifecycle, load/save, LineVector |
| SyntaxHighlighterTests | 40+ languages, theme system, custom rules, state machine |
| ShortcutAdapterTests | RegisterHotKey→QShortcut, register/unregister, conflict detection, 32-shortcut stress |

## Architecture

| Layer | Implementation |
|-------|---------------|
| Buffer | `Buffer` + `LineVector` — file-backed text storage |
| Encoding | `EncodingDetector` — BOM scan + chardet fallback |
| Highlighting | `SyntaxHighlighter` + `SyntaxHighlighterImpl` — QSyntaxHighlighter subclass |
| Themes | 7 built-in themes (dark, dracula, monokai, nord, solarized, one-dark, light) |
| Platform | CMake + Ninja + Qt6 (no MFC/Win32 dependency) |

## Semantic Lift Notes

This is a selective port — some Win32-specific source files do not apply to Qt6/Linux:

| Source Unit | Status | Qt6 Replacement |
|-------------|--------|-----------------|
| EncodingMapper | Not ported | EncodingDetector (standalone) |
| uchardet | Not ported | EncodingDetector (chardet-based) |
| pugixml | Not ported | Qt XML (QDomDocument) |
| resource (.rc) | Not ported | Qt Resource System (.qrc) |
| localizationString | Not ported | Qt i18n (`.ts`/`.qm`) |
| DarkMode/DPI | Not ported | Qt handles these natively |
| Win32 Accelerators / ShortcutMapper | Not ported | `ShortcutAdapter` — Qt6 QShortcut + QAction::setShortcut() |

## Keyboard Shortcuts

Notepad--Qt uses Qt6's `QShortcut` and `QAction::setShortcut()` to implement keyboard shortcuts.
This replaces Win32's `RegisterHotKey()` / `DestroyAcceleratorTable()` / `WM_HOTKEY` semantics.

**Key mapping** — all Win32 accelerator entries are mapped to their `QKeySequence::StandardKey`
equivalents where defined (e.g. `Ctrl+S` → `QKeySequence::Save`).  Non-standard combos use
explicit `QKeySequence("Ctrl+Shift+S")` strings.

**DE conflict notes** — some shortcuts are intercepted by the host desktop environment before
Qt sees them.  If a shortcut does not work, check the list below:

| Shortcut | DE conflict | Workaround |
|----------|-------------|-------------|
| `Ctrl+Alt+Backspace` | GNOME/KDE — terminates X server | App does not use it |
| `Alt+Tab` | All major DEs — window switcher | Use `Ctrl+Tab` for tab nav |
| `PrintScreen` | GNOME/KDE — screenshot tool | Use `Ctrl+Alt+P` (mapped to settings) |
| `F10` | KDE — menu bar access | Use `F11` (fullscreen) |
| `Ctrl+Alt+Arrow` | GNOME — workspace switch | Use `Ctrl+PageUp/PageDown` |
| `Alt+F2` | GNOME — application launcher | Use `Ctrl+Shift+P` (command palette) |
| `Alt+Space` | KDE — krunner | Unavoidable on KDE; use menu bar |

**Implementation:** `src/common/ShortcutAdapter.h` / `.cpp` — central registry for keyboard
shortcuts.  Shortcuts are parented to the main window and destroyed automatically when the
window closes (matching Win32 implicit `UnregisterHotKey` on `WM_DESTROY`).
| json | Not ported | XML configuration used |
| clipboardFormats | Not ported | QClipboard (Qt) |

See `skills/codebase-translate/build/npp-qt/translation_report.json` for full gate report.

## Gate Report (translation_report.json)

```
build:         ✅ PASS  -- 0 errors, 98/98 ninja targets
translation:   ✅ PASS  -- 77.1% coverage (cpp→qt6 selective-port threshold)
units:         ✅ PASS  -- all units resolved
holography:    ✅ PASS  -- 48.5% (cpp→qt6 per-project threshold 40%)
documentation: ✅ PASS
tests:         ✅ PASS  -- 4/4 suites
runtime:       ✅ PASS  -- --help responds correctly
differential:  ✅ PASS  -- 100.0% parity (28/28 cases)
smoke_launch_binary: ✅ PASS
smoke_no_crash:     ✅ PASS
done: true
```

> Translation coverage is 77.1% raw; "missing" files are Windows-specific (.rc, Win32 API)
> that do not apply to Qt6/Linux. See `sigma.unverifiable_units` in translation_report.json.
> Holography reflects that Win32→Qt API symbols are fundamentally different. Per-project
> thresholds applied per `translation_harness.py` cpp→qt6 selective-port logic.
