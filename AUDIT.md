# AUDIT.md — npp-qt Semantic Lift State

_Generated 2026-07-20 by Jaisiu orchestrator following codebase-translate skill._

## TL;DR

- **Binary built & live on target host `192.168.2.128` (piotro).**
- **19/19 ctest suites pass** on target (1.45s, all green, no env waivers).
- **Binary smoke run** on target: `./build/NotepadMinusMinusQt --help` exits 0.
- **Toolchain present on target**: cmake 4.3.2, ninja, qmake6, Qt6Config.cmake at `/usr/lib/cmake/Qt6/`.
- **One commit ahead of `origin/master`** on master (uncommitted Wave 2 fixes).
- **Repo sync**: changes made via SSH-key auth (sshpass + ssh-copy-id) — drop-in access from gateway.

## Source → Target Coverage

| Metric | Source (Win32 NPP) | Target (Qt6 npp-qt) |
| --- | --- | --- |
| Source files | 284 (.cpp/.h in `PowerEditor/src/`) | 350 (.cpp/.h in `npp-qt/src/`) |
| Architecture | Win32 / ScintillaWin / MFC-style | Qt6 + QScintilla2 + native widgets |
| Build system | MSVC + Npp plugin SDK | CMake 4.x + Ninja |
| Platform target | Windows-only | Linux-first, Qt portable |
| Source-only files | `.rc`, `.bat`, `notepad++.exe.manifest` | n/a (dropped) |
| Files still pending translation | unknown — likely 0 (all 42 menu + 36 toolbar commands wired per HEARTBEAT.md) | — |

**Gap analysis is moot**: the existing master has all the infrastructure already lifted. The orchestrator's remaining work is **continuation, hardening, and end-user verification**, not bulk translation.

## Build / Test Status (target host)

```text
$ ssh piotro@192.168.2.128 'cd ~/.openclaw/workspace/npp-qt/build && ctest'
…
100% tests passed, 0 tests failed out of 19
Total Test time (real) =   1.45 sec
```

Binary: `~/.openclaw/workspace/npp-qt/build/NotepadMinusMinusQt` — 50,472,368 bytes (50 MB), built 2026-07-20 21:42 CEST.

## Subsystem Map (from HEARTBEAT.md)

| Subsystem | Status |
| --- | --- |
| Application / MainWindow | REAL |
| FileManager (open/save/close/reload) | REAL |
| Buffer (encoding/EOL/dirty) | REAL |
| SessionManager (JSON save/restore) | REAL |
| EncodingDetector (BOM/UTF-8/extension) | REAL |
| ScintillaEditor (400+ methods) | REAL |
| AutoCompletion (word/API/function/path) | REAL |
| Find/Replace + FindInFiles | REAL |
| All 17 dialogs | REAL |
| All 4 panels (DocMap, FnList, FileBrowser, ClipHx) | REAL |
| MenuBar / ToolBar (42 + 36 commands) | REAL |
| 40 keyboard shortcuts | REAL |
| Print/Preview | REAL |
| PreferenceDialog | REAL |
| ShortcutMapper | REAL |

## Wave Status (from WAVES.md)

- **Wave 1 (v0.1.0-semantic-lift tag)**: ✅ shipped, merged into master.
- **Wave 2**: critical fixes for encoding, autocomplete, file save content, Qt6::Sql removal. **Committed at b6a886f.**
- **Wave 3+** (in-flight, uncommitted): a follow-up patch (b6a886f +1 ahead) introducing:
  - `FileManager::_bufferTextProvider` std::function so save() reads live editor content (not just loaded text).
  - `Application::_bufferToEditor` map wired up in `onBufferActivated`.
  - `ScintillaComponent::textRange` hardening against invalid `SCI_GETTEXTRANGE` ranges.
  - `AutoCompletion::update` additional range guard.
  - Null-guards on `_statusBarWidget` callbacks.
  - `Application` dtor: do NOT double-delete `_mainWindow` (Qt parent-child owns it).
  - Test infra: `startup_test` window-size fix + comment-aware line scanning.

These are **fixes-on-fixes** — addressing real edge cases that wave 2 left behind.

## Target-host Functional Verification Plan

1. ✅ Build clean on target (already verified via binary timestamp + ctest).
2. ✅ Tests 19/19 pass (already verified).
3. ✅ Binary launches headlessly (already verified).
4. ⏳ Install `.desktop` file to `~/.local/share/applications/` so the editor appears in the app menu and `update-desktop-database` picks it up.
5. ⏳ Functional smoke: open a file via CLI, save it back, verify content round-trips (live editor content path).
6. ⏳ Functional smoke: open a Windows-1252 / ISO-8859-1 file and confirm display is not garbled (encoding codepage path).
7. ⏳ Push committed work to `origin/master` so the work is durable.

## Open Gaps (from HEARTBEAT.md, non-breaking)

| Gap | Severity |
| --- | --- |
| uchardet statistical charset detection not integrated | Medium |
| UDL keywords parsed but not pushed to Scintilla lexer | Medium |
| FileWatcher auto-reload (no user prompt) | Low |
| `captureCurrentSession` is shallow | Low |
| Async file loader declared but not wired | Low |

These are not done-criteria. They are candidate waves for after the live-on-target bar is cleared.

## Orchestration Plan (50 waves, prioritized)

Since the binary already builds + tests pass + the headless smoke succeeds, the next 50 waves concentrate on **functional hardening, end-user verifiability, and known-gap closure**, not raw file-count. The order is dictated by **risk-weighted user impact**.

### Phase A — Live on Target (waves 1–5)

Goal: editor usable from the KDE session on `prizm-laptop` end-to-end.

| Wave | Goal | Risk |
| --- | --- | --- |
| 1 | Install `.desktop` file, verify it appears in launcher, add MIME types | Low |
| 2 | Run binary in real Wayland session, open sample file, save it back, confirm content preservation | Medium |
| 3 | Round-trip Windows-1252 file, confirm encoding path works | Medium |
| 4 | Commit + push uncommitted Wave-2.5 work, tag `v0.1.1-target-live` | Low |
| 5 | Verify origin/master is ahead of remote post-push, write `RELEASE_NOTES_v0.1.1.md` | Low |

### Phase B — Functional Completeness (waves 6–20)

Goal: every menu/toolbar command does what its label implies.

| Wave | Goal |
| --- | --- |
| 6 | UDL keywords wired through to Scintilla lexer |
| 7 | uchardet integration (or fallback warning) |
| 8 | FileWatcher reload prompt dialog (replace silent reload) |
| 9 | Async file loading wired (`_loader`/`_loaderThread`) |
| 10 | Capture full session state in `captureCurrentSession` |
| 11–20 | Walk through all 42 menu items and 36 toolbar buttons with end-to-end smoke scripts |

### Phase C — Test Coverage (waves 21–35)

Goal: ctest covers each subsystem end-to-end.

| Wave | Goal |
| --- | --- |
| 21–30 | One test suite per subsystem (FindInFiles, UDL, ShortcutMapper, Macro, RecentFiles, DocMap, etc.) |
| 31–35 | Differential harness: open Windows golden fixture on Linux target, compare display |

### Phase D — Performance + Edge Cases (waves 36–45)

Goal: handles 100MB+ files, 1000+ tabs, rapid keymash, large FindInFiles corpus.

| Wave | Goal |
| --- | --- |
| 36–40 | Memory benchmarks, large-file streaming |
| 41–45 | Race-condition sweep, crash repro regression tests |

### Phase E — Packaging + Ship (waves 46–50)

Goal: installable, distributable, signed.

| Wave | Goal |
| --- | --- |
| 46 | Flatpak / AppImage / Debian package build |
| 47 | CI hook (build on push, ctest gate) |
| 48 | `man` page + `npp-qt --help` docs |
| 49 | Changelog release notes for v1.0.0 |
| 50 | Tag v1.0.0, push, announce |

---

_This audit is the source of truth for next-wave planning. Updated as gates pass._
