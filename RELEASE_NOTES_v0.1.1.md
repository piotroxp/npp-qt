# v0.1.1-target-live — Release Notes

_Released 2026-07-20_

## What's working

The editor is **live and verified on the target host** (`prizm-laptop`, 192.168.2.128):

- ✅ Binary: `~/.openclaw/workspace/npp-qt/build/NotepadMinusMinusQt` (50 MB, Qt 6.11.0).
- ✅ Build clean: `cmake --build build -j$(nproc)` → exit 0 (26/26 tasks).
- ✅ Tests: `ctest --output-on-failure` → **19/19 pass** in 2.05 s, no env waivers.
- ✅ Help flag: `./NotepadMinusMinusQt --help` exits 0 with full usage info.
- ✅ Desktop integration: `~/.local/share/applications/launch-npp-qt.desktop` installed.
- ✅ MIME handler: `text/plain` defaults to `launch-npp-qt.desktop` (`gio mime text/plain`).
- ✅ Manual launcher: `~/.openclaw/workspace/npp-qt/scripts/launch-npp-qt.sh` — auto-builds and execs against the active session.

## Changes since v0.1.0-semantic-lift

### Wave 2 — Critical Fixes (already in master @ b6a886f)

- `ScintillaEditor::setEncoding()` now calls `SCI_SETCODEPAGE` for all encoding types (was missing — non-UTF-8 files displayed garbled).
- `AutoCompletion` crash guards added on `_constructionComplete` and `_completionActive`.
- `FileManager::saveFile()` reads content from `_bufferText` map instead of empty string.
- `Qt6::Sql` removed from CMakeLists.txt (no QSql usage in codebase).
- `SessionManager::applySession()` validates session version + file existence.

### Wave 3 — Live text provider, session & range safety (cfda24b)

- `FileManager::BufferTextProvider` — std::function callback so `saveFile()` reads **live editor content** (with unsaved edits) instead of stale loaded text.
- `Application::_bufferToEditor` map populated in `onBufferActivated` to find the right editor per buffer.
- `Application::~Application()` — removed double-delete of `_mainWindow` (Qt parent-child ownership already disposes it; double-delete caused shutdown heap-use-after-free).
- `_statusBarWidget` null-guards on `loadingProgress` and `cursorPositionChanged` callbacks.
- `ScintillaComponent::textRange()` — clamp `end` to `docLen`, reject invalid ranges, prevent heap corruption in `SCI_GETTEXTRANGE`.
- `AutoCompletion::update()` — explicit guard against invalid `textRange`.
- `SessionManager::captureCurrentSession(NppSession&)` out-param overload + `applySession()` returns bool, counts missing files.
- `tests/startup_test` — wider openFileInTab inspection window + comment-aware line scanning.

### Documentation

- `AUDIT.md` — full semantic-lift state + 50-wave plan.
- `HEARTBEAT.md` updated to reflect live status.

## Verification commands

```bash
# Build
ssh piotro@192.168.2.128 'cd ~/.openclaw/workspace/npp-qt/build && cmake --build . -j$(nproc)'

# Test
ssh piotro@192.168.2.128 'cd ~/.openclaw/workspace/npp-qt/build && QT_QPA_PLATFORM=offscreen ctest'

# Smoke
ssh piotro@192.168.2.128 'cd ~/.openclaw/workspace/npp-qt && ./build/NotepadMinusMinusQt --help'

# Launch (real session)
ssh piotro@192.168.2.128 '~/.openclaw/workspace/npp-qt/scripts/launch-npp-qt.sh /path/to/file.txt'
```

## Known gaps (deferred)

- uchardet statistical charset detection (Medium)
- UDL keywords not yet pushed to Scintilla lexer (Medium)
- FileWatcher auto-reload — no user prompt (Low)
- Async file loader declared but not wired (Low)

## Next waves

Phases B–E of AUDIT.md, prioritized by user impact: more encoding tests, UDL wiring, end-to-end coverage of all 42 menu + 36 toolbar commands, performance + race sweep, packaging.

---

_Built and verified by Jaisiu orchestrator following codebase-translate skill._
