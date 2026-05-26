# Architectural Summary: npp-qt

## Quantitative Snapshot (Current Tree)
Measured from `/home/piotro/npp-qt`, excluding `build/` artifacts:

- Total files: **484**
- Total lines: **173,224**
- Main source families:
  - `.cxx`: 174 files / 109,684 lines
  - `.cpp`: 89 files / 20,950 lines
  - `.h`: 186 files / 29,368 lines
- Key directories:
  - `src/`: 460 files / 167,366 lines
  - `include/`: 14 files / 4,708 lines
  - `resources/`: 7 files / 707 lines
  - `src/ScintillaComponents/scintilla/`: 102 files / 56,774 lines
  - `src/ScintillaComponent/`: 16 files / 1,343 lines
  - `src/MISC/`: 66 files / 6,066 lines

Representative architecture files (line counts):
- `include/Notepad_plus.h`: 790
- `src/Notepad_plus.cpp`: 843
- `src/Parameters.cpp`: 611
- `src/ScintillaComponent/ScintillaEditView.h`: 599
- `src/NppDarkMode.cpp`: 676
- `src/MainWindow.cpp`: 253
- `src/main.cpp`: 141
- `src/NppXml.cpp`: 176
- `include/NppApplication.h`: 59

## Executive View
This repository is an in-progress Qt6/C++17 port of Notepad++ (target version 8.9.4), with a hybrid architecture: substantial core editor/control scaffolding is present, but multiple layers remain partially translated or internally inconsistent. The codebase compiles a large source surface (`src/**/*.cpp`) and links Qt + Scintilla/Lexilla, but implementation maturity differs sharply across subsystems.

At a high level, architecture is organized into:
- **Application/bootstrap** (`src/main.cpp`, `include/NppApplication.h`)
- **UI shell/wrapper** (`src/MainWindow.cpp`, `include/MainWindow.h`, `include/Notepad_plus_Window.h`)
- **Core editor/controller** (`include/Notepad_plus.h`, `src/Notepad_plus.cpp`)
- **Settings/state persistence** (`include/Parameters.h`, `src/Parameters.cpp`)
- **Editing engine integration** (`src/ScintillaComponent/*`, `src/ScintillaComponents/scintilla/*`, `src/ScintillaComponents/lexilla/*`)
- **Support services** (dark mode, XML, encoding, process, hashing, etc.)

## Build and Dependency Topology
`CMakeLists.txt` drives a monolithic executable build:
- `project(npp-qt VERSION 8.9.4)` with C++17 and Qt automoc/autorcc/autouic.
- Qt dependencies: `Core`, `Gui`, `Widgets`, `PrintSupport`, `Network`, `Xml`, and `Core5Compat`.
- External in-tree components: Scintilla + Lexilla via subdirectories.
- Source collection uses `GLOB_RECURSE` over all `src/*.cpp`, which is broad and convenient but can unintentionally include unfinished translation units.

Architectural consequence: build configuration assumes a near-complete port surface, while runtime feature completion is uneven.

## Runtime Initialization and Control Flow
### Entry path
`src/main.cpp` initializes high-DPI attributes, creates `NppApplication`, loads translators, parses CLI options, loads settings, applies a dark palette conditionally, then creates and shows `MainWindow`.

CLI flags include multi-instance/read-only/session/line-column/top/noPlugin style options, but some options are parsed without full downstream behavior wiring.

### Application object
`include/NppApplication.h` subclasses `QApplication` and owns:
- translation loading
- settings path setup via `QSettings`

There is overlap with translation handling in `main.cpp`; translation loading happens in both places (constructor + entrypoint), indicating duplicated responsibilities.

## UI Layering Model
### Outer shell: `MainWindow`
`MainWindow` is a Qt `QMainWindow` wrapper that constructs menu/toolbar/statusbar and forwards many actions to a contained `_npp` object (`Notepad_plus`).

Intent: provide Qt-native frame/chrome while preserving original Notepad++ command semantics in `Notepad_plus`.

Current state:
- Menu and toolbar wiring exist for many commands.
- Several handlers are placeholders (`/* handled by scintilla */`, empty session/file open loops, simplified go-to/fullscreen/post-it).
- `activeEditor()` reaches into `_npp._pEditView`, implying tight coupling to `Notepad_plus` internals.

### Core controller: `Notepad_plus`
`Notepad_plus` remains the primary orchestration class, inheriting `QMainWindow` and exposing broad command-style methods (`fileOpen`, `fileSaveAll`, find/replace, session, macro, view operations, etc.).

Initialization in `src/Notepad_plus.cpp` sets up:
- main/sub Scintilla views
- document tabs
- splitter/docking foundations
- status/tool/menu infrastructure
- style/zoom/margins/wrap/selection behaviors
- singleton services (`NppParameters`, dark mode, localization)

Architectural pattern: **god-controller / façade over subsystems**, matching the original Notepad++ style.

## Editing Subsystem
### Scintilla integration layers
There are two relevant directories:
- `src/ScintillaComponents/scintilla/*`: embedded Scintilla + Qt wrapper implementation.
- `src/ScintillaComponent/*`: Notepad++ integration wrappers (`ScintillaEditView`, tab views, dialogs, controls).

`ScintillaEditView` is the key abstraction over Scintilla messages and editing operations (selection/text/search/replace/autocomplete/etc.), preserving message-based API style (`execute(SCI_...)`).

Design characteristic: a compatibility-oriented adapter retaining Notepad++ concepts while using Qt widgets underneath.

## Persistence and Configuration Architecture
`NppParameters` (singleton) is the central state repository for:
- GUI flags and layout preferences
- recent files and history
- editor view params (margins, wrap, zoom, whitespace, EOL visibility)
- dark mode settings
- shortcuts, user-defined languages, find history

Persistence strategy:
- `.ini` via `QSettings` for many scalar settings
- XML files for shortcuts, language definitions, and history
- settings folder discovery supports portable-style local `config/` and standard app config fallback

Architectural role: **global configuration/state service**, heavily coupled to UI/editor startup.

## Support Subsystems
- **Dark mode** (`src/NppDarkMode.cpp`): centralized theme state + palette + stylesheet application; recursive child widget theming.
- **XML utilities** (`src/NppXml.cpp`): custom lightweight DOM-like node tree + stream parser/writer helpers.
- **Encoding and conversion** (`src/Utf8_16.*`, encoding mapper modules): intended for Notepad++ encoding behavior parity.
- **Misc services** under `src/MISC/*`: hashing, regex trees, process wrappers, exception handling, file browser pieces.

## Architectural Strengths
- Clear intent to preserve Notepad++ mental model and command taxonomy.
- Scintilla/Lexilla are integrated as first-class in-tree dependencies.
- Configuration/state surface is broad and maps many classic Notepad++ settings.
- Portability layer (`WindowsCompat`, shims) enables gradual migration from Win32 idioms.

## Structural Risks and Inconsistencies
1. **Dual/overlapping main-window architecture**
   - `MainWindow` wraps `Notepad_plus`, while `Notepad_plus` itself is also a `QMainWindow`.
   - This can produce ownership/lifecycle ambiguity and duplicated frame responsibilities.

2. **Type model drift around buffers/file manager**
   - `include/FileManager.h` defines `BufferID` as `int`.
   - `src/ScintillaComponent/Buffer.h` defines `BufferID` as `Buffer*` and includes a different `FileManager` singleton model.
   - This is a major architectural inconsistency that can destabilize interfaces and compile/runtime behavior.

3. **Translation maturity mismatch**
   - Core classes expose extensive APIs, but many pathways are stubs/placeholders.
   - Summary document (`TRANSLATION_SUMMARY.md`) explicitly marks several critical pieces incomplete.

4. **Potential duplication in translation/bootstrap concerns**
   - Translator loading appears in both `NppApplication` and `main.cpp`.

5. **Monolithic controller complexity**
   - `Notepad_plus` centralizes many concerns (UI, file/session logic, editor state, theming hooks), increasing coupling and maintenance cost.

## Implicit Architectural Direction
Current code suggests a **compatibility-first porting strategy**:
- preserve original APIs and semantics,
- shim Win32 concepts into Qt equivalents,
- defer deep refactor until feature parity stabilizes.

This is pragmatic for migration, but technical debt is concentrated in adapter layers and duplicated abstractions.

## Practical Component Map
- **Bootstrap**: `src/main.cpp`, `include/NppApplication.h`
- **Outer shell**: `include/MainWindow.h`, `src/MainWindow.cpp`
- **Core controller**: `include/Notepad_plus.h`, `src/Notepad_plus.cpp`
- **State/config**: `include/Parameters.h`, `src/Parameters.cpp`
- **Editor integration**: `src/ScintillaComponent/ScintillaEditView.h` (+ related components)
- **Engine dependencies**: `src/ScintillaComponents/scintilla`, `src/ScintillaComponents/lexilla`
- **Theme/XML/encoding**: `src/NppDarkMode.cpp`, `src/NppXml.cpp`, `src/Utf8_16.cpp`, `src/MISC/Encoding/*`

## Overall Assessment
Architecturally, this is a substantial but transitional port: core scaffolding and subsystem boundaries are recognizable and aligned with Notepad++ heritage, yet the project remains in a **partially converged state** with key model inconsistencies (especially buffer/file-manager typing) and mixed implementation completeness.

It is faithful to upstream design intent, but not yet fully coherent as a finished Qt-native architecture.
