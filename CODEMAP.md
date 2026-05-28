# CODEMAP — npp-qt

## Purpose
Qt6/Linux semantic lift of Notepad++ with Scintilla/Lexilla embedded and Win32 compatibility shims.

## Top-Level Architecture
- `src/`: Primary C++ implementation.
- `include/`: Public/shared headers for app shell, compatibility, and core abstractions.
- `resources/`: Qt resources (`.qrc`), themes, and i18n (`.ts`) catalogs.
- `tests/`: Test assets and test code (present but still evolving).
- `cmake/` + `CMakeLists.txt`: Build orchestration.
- `src/ScintillaComponents/`: Vendored editing engine components integrated into Qt build.

## Build Graph
- Root `CMakeLists.txt` builds executable `npp-qt`.
- Adds subprojects:
  - `src/ScintillaComponents/lexilla`
  - `src/ScintillaComponents/scintilla`
- Links Qt6 (`Core`, `Gui`, `Widgets`, `PrintSupport`, `Network`, `Xml`, `Core5Compat`) + `scintilla_qt` + `lexilla`.

## Core Runtime Modules
- `src/main.cpp`: App bootstrap, main event loop startup.
- `src/MainWindow.cpp` + `include/MainWindow.h`: Main UI shell and command/action wiring.
- `src/Notepad_plus.cpp` + `include/Notepad_plus.h`: Notepad++ semantic core port surface.
- `src/Parameters.cpp` + `include/Parameters.h`: Settings/state persistence and app parameters.
- `src/NppXml.cpp` + `include/NppXml.h`: XML config/session handling.
- `src/NppDarkMode.cpp` + `include/NppDarkMode.h`: Theme/dark mode abstraction.
- `src/Utf8_16.cpp` + `include/Utf8_16.h`: Encoding conversion helpers.

## Editing Engine Integration
- `src/ScintillaComponent/`: Integration layer used by app code (views/dialogs/tab abstractions).
  - `ScintillaEditView.*`
  - `DocTabView.*`
  - `FindReplaceDlg.*`
  - `GoToLineDlg.*`
  - `Buffer.h` (buffer model interface surface)
- `src/ScintillaComponents/scintilla/`: Upstream Scintilla core + Qt platform adapter.
- `src/ScintillaComponents/lexilla/`: Lexer catalog and lexlib implementation.

## UI Controls and Panels
- `src/WinControls/`: Ported UI utility/control space.
  - `ClipboardHistory/`
  - `ProjectPanel/`
  - `Window/`
- Root/header-level dialogs also exist during ongoing refactor (legacy path parity).

## Cross-Platform and Compatibility Layer
- `include/WindowsCompat.h`
- `include/windows.h`
- `include/Win32QtShim.h`
These provide Win32 API semantic placeholders/shims required by lifted code paths.

## Localization and Theme Assets
- `resources/i18n/npp_en.ts`
- `resources/i18n/npp_pl.ts`
- `resources/themes/default.qss`
- `resources/npp-qt.qrc`

## Current Lift Characteristics
- Mixed maturity: some modules fully ported, others scaffolding/header-only.
- Directory naming reflects parallel evolution:
  - `src/ScintillaComponent` (integration)
  - `src/ScintillaComponents` (vendored engines)

## Candidate Gap Hotspots (vs full Notepad++ parity)
- Incomplete source companions for some declared headers.
- Command/action parity for all Notepad++ menu/shortcut flows.
- Full dockable panel ecosystem and plugin lifecycle.
- End-to-end configuration/session parity.
