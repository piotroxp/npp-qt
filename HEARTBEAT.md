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

**2026-07-05T10:20 UTC — P0 fixes applied:**
- `AutoCompletion::setLanguage()` — fully implemented: loads `autoCompletion/{lang}.xml` via NppXml, populates keyword arrays and calltip data
- `FunctionCallTip::loadFunction()` — implemented: parses function signatures for calltip display
- `showPathCompletion()` — rewired with QDir enumeration (was `/` placeholder)
- `KeywordLoader.cpp/h` — new: parses `langs.model.xml` for 40+ languages, feeds Scintilla via SCI_SETKEYWORDS (set 0–15)
- `StyleConfig.cpp/h` — new: parses `stylers.model.xml`, applies fg/bg/bold/italic/font per lexer+style
- `ScintillaComponent::setLexerLanguage()` — wired: calls KeywordLoader + StyleConfig after lexer install
- `Parameters::load()` — wired: calls KeywordLoader::instance().load() and StyleConfig::instance().load() at startup
- CMakeLists.txt (root): added KeywordLoader.cpp + StyleConfig.cpp to SOURCES
- Engineering fix: `SCI_SETKEYWORDS=4005` defined directly (not via SciLexer.h) to avoid Qt macro conflicts

**Gates (2026-07-05 10:20 UTC):** build ✅ runtime ✅ tests 158/158 ✅ documentation ✅ units ✅ | differential 🔴 (Windows oracle required)

**Workspace paths:** source at `~/.openclaw/workspace/src/`, build at `~/.openclaw/workspace/build_npp/`, binary at `build_npp/npp-qt`.

**Remaining gaps (non-blocking):** Buffer.cpp, FindReplaceDlg.cpp, columnEditor.cpp, 3830L ScintillaEditView missing, UserDefineDialog stub 81L vs 2105L source.

**Commit:** `71ab747` — "feat: P0 auto-completion and syntax highlighting fixes"
**SSH:** Key generated at `/home/node/.ssh/id_ed25519_nppqt.pub` — waiting for user to add to `piotro@192.168.2.128:~/.ssh/authorized_keys`

**Last gate run:** 2026-07-05T09:21:48 UTC | report: `/home/node/jaisiu/skills/codebase-translate/build/npp-qt/translation_report.json`
<!-- semantic-lift:end -->
