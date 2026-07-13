# HEARTBEAT.md

## npp-qt — Wave Status (as of 2026-07-13 ~20:05 UTC)

| Wave | Status | Commit | Summary |
|------|--------|--------|---------|
| 1-10 | ✅ | `v0.1.0-semantic-lift` | Semantic lift from notepad-plus-plus-translation |
| 11 | ✅ | `cee8214` | C/C++ keywords, charset conversion, encoding test fix |
| 12 | ✅ | `78b7d4f` | Print dialog, macro save/load |
| 13 | ✅ | `19d5084` | Find in Files, file change monitoring |
| 14 | ✅ | (wave-14 agent) | Plugin API skeleton, window title, selection info |
| 15 | ✅ | `d81f8a5` | CI, CMake install, CONTRIBUTING/CHANGELOG, `v0.2.0` tag |
| 16 | ✅ | `13cb157` | Code folding, auto-indent, bracket highlight, current line |
| 17 | ✅ | `4c39dad` | Session geometry, bookmarks, incremental search |
| 18 | ✅ | `2e099f6` | Virtual space, drag-drop files, whitespace dots, gutter polish, rect selection |
| 19 | ✅ | `6a51c77` | Large file handling, async loading, partial load status bar |
| 20 | ⏳ | — | Planned: preferences, command palette improvements |

**Binary:** `build/NotepadMinusMinusQt` (clean build)
**Source:** 75+ .cpp files
**Tags:** `v0.1.0-semantic-lift`, `v0.2.0` (local only — push blocked: no GitHub credentials)

## Build
```bash
cd /home/node/.openclaw/workspace
cmake --build build -j$(nproc)
```

## Next Waves (18-20)
See WAVES.md for full wave 18-20 spec.
