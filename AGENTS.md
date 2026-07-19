# AGENTS.md — npp-qt

## Canonical Project Location

```
~/.openclaw/workspace/npp-qt   <- ALWAYS work here
branch: master                 <- NEVER commit directly to master; use feature branches
```

## Key Paths

| Path | Meaning |
|------|---------|
| `~/.openclaw/workspace/npp-qt` | Project root |
| `~/.openclaw/workspace/npp-qt/src/` | Source code |
| `~/.openclaw/workspace/npp-qt/docs/` | Documentation |
| `~/.openclaw/workspace/npp-qt/build/` | Canonical build dir |
| `~/.openclaw/workspace/notepad-plus-plus-translation` | Upstream Win32 reference |

## Branch Workflow

- ALWAYS work on feature branches: `fix/`, `feat/`, `docs/`, `chore/`
- Open a PR / branch so the diff is visible
- `master` is clean reviewed history only

```
git checkout master && git pull --ff-only origin master
git checkout -b feat/my-feature
# ... work ...
git push origin feat/my-feature
```

## Before Every Commit

```bash
# 1. Verify no build artifacts
git status --short

# 2. Build and test
cmake --build build && QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

## What NOT to Commit

```
build*/  build-*/  b/  *.o  *.a  *.so  NotepadMinusMinusQt
Testing/  __pycache__/  .openclaw/  .jaisiu/  .memory/
```

## Building

```bash
cmake -B build && cmake --build build -j$(nproc)
QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt --help
```

## Docs

All docs in `docs/`: README, ARCHITECTURE, CORE_SYSTEMS, EDITOR, DIALOGS,
UI_SUBSYSTEMS, KEYBOARD_SHORTCUTS, API, COMMON_UTILITIES, GETTING_STARTED,
TESTING, TROUBLESHOOTING, ENCODING, THEMES, PANELS, COMMAND_LINE

## Semantic Lift

~60-65% functional coverage vs upstream NPP. See HEARTBEAT.md.

## Quick Reference

```bash
# ASAN build
cmake -B build-asan -DNPP_DEBUG_SANITIZERS=ON
cmake --build build-asan

# Run tests
ctest --test-dir build --output-on-failure

# Debug crash
gdb ./build/NotepadMinusMinusQt
(gdb) run; (gdb) bt full
```
