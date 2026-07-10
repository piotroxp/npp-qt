# HEARTBEAT.md

## NPP-QT SEMANTIC LIFT — ALWAYS RESUME

**Repo:** `/home/node/.openclaw/workspace-employee-hart/npp-qt/`
**WAVES.md:** `./npp-qt/WAVES.md` — check for completion status

### Wave Completion Tracking

| Wave | Feature | Agent | Status | Notes |
|------|---------|-------|--------|-------|
| 1 | Build Survival | agent-wave1 | in_progress | cmake + fix all compile errors |
| 2 | Status Bar + Cursor/Encoding | agent-wave2 | pending | live display |
| 3 | Tab Bar + Multi-Document | agent-wave3 | pending | per-tab EditorView |
| 4 | Find/Replace Dialog | agent-wave4 | pending | real search |
| 5 | Menu Bar Language Actions | agent-wave5 | pending | wired language menu |
| 6 | Encoding Pantry + EOL | agent-wave6 | pending | Buffer.cpp UTF-16 + backups |
| 7 | Keyboard Shortcuts + Edit | agent-wave7 | pending | all edit menu actions |
| 8 | Advanced Editor Features | agent-wave8 | pending | bracket match, auto-indent, etc |
| 9 | File Operations Enhancements | agent-wave9 | pending | reload, save all, external mods |
| 10 | Final Verification + Polish | agent-wave10 | pending | build, valgrind, CI, README |

### After Each Wave
1. Build: `cd /home/node/.openclaw/workspace-employee-hart/npp-qt && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build`
2. Verify: `./build/notepad++ --version` or check binary exists + no crash
3. Run consistency: basic smoke test (open, type, save)
4. Update WAVES.md status column
5. If wave N passed, start wave N+1

### If an Agent Crashes or Dies
- Check git log for partial commits
- Resume from where it left off (read WAVES.md + diff the files)
- Re-spawn the same wave agent

### Priority Rules
- **This is the ONLY project.** Do not work on anything else.
- Keep going until Wave 10 is marked ✅ in WAVES.md
- Wake up → check WAVES.md → continue next incomplete wave
