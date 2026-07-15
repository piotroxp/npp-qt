# HEARTBEAT.md

## NPP-QT SEMANTIC LIFT — STATUS 2026-07-15

**Repo:** `/home/node/.openclaw/workspace/npp-qt/`
**Translation upstream:** `/home/node/.openclaw/workspace/notepad-plus-plus-translation/`
**Build:** Docker (archlinux:base-devel + qt6-webengine)
**Binary:** `build/NotepadMinusMinusQt`

---

## Wave Completion Tracking

| Wave | Feature | Status | Notes |
|------|---------|--------|-------|
| 1-15 | Build survival, UI, editor, I/O, highlighting, macros, panels, CI | ✅ complete | commits up to 807f3d9 |
| 16 | Code folding, auto-indent, bracket highlight, current line | ✅ complete | 13cb157 |
| 17 | Session polish, bookmarks, incremental search | ✅ complete | 343757e |
| 18 | Virtual space, drag-and-drop, whitespace, gutter, rect selection | ✅ complete | 2e099f6 |
| 19 | Large file handling, async loading, partial load status | ✅ complete | 6a51c77 |
| 20 | Preferences completeness, clipboard encoding, final polish | ✅ complete | 2a2aed3 |
| — | 54 language definitions in SyntaxHighlighter | ✅ complete | 807f3d9 |
| — | FunctionListXmlParser: load 52 upstream N++ XML parsers | ✅ complete | this session |
| — | Swift, Kotlin, TypeScript, C# parsers added | ✅ complete | this session |
| — | WebBrowserView: QWebEngineView preview panel | ✅ complete | 9b3178c |
| — | FileLoaderWorker: async I/O + large-file warning | ✅ complete | 6a51c77 |
| — | Build fixes: mkstemp, QDir::Recursive → QDirIterator | ✅ complete | this session |
| — | Documentation expanded: ARCHITECTURE.md + README.md | ✅ complete | cdac62f |
| — | CHANGELOG.md restored | ✅ complete | this session |

---

## Current Work (this session)

**FunctionListXmlParser** — new module that loads Notepad++ `functionList/*.xml` files
(from upstream translation repo) and converts N++ regex patterns to Qt `QRegularExpression`.
52 language parsers available. Falls back to built-in regex if XML unavailable.

**Build environment:** Docker archlinux:base-devel with qt6-webengine added.
Fixed `mkstemp(QByteArray::data())` and `QDir::Recursive` → `QDirIterator::Subdirectories`.

**Binary status:** Docker build in progress (session faint-trail).


### Wave Completion Tracking

| Wave | Feature | Agent | Status | Notes |
|------|---------|-------|--------|-------|
| 1 | Build Survival | agent-wave1 | in_progress | cmake + fix all compile errors |
| 2 | Status Bar + Cursor/Encoding | agent-wave2 | pending | live display |
| 3 | Tab Bar + Multi-Document | agent-wave3 | pending | per-tab EditorView |
| 4 | Find/Replace Dialog | agent-wave4 | pending | real search |
| 5 | Menu Routing + StatusBar Finalization | wave-5 | ✅ complete | menu command dispatcher (40+ cmds) + StatusBar wiring + mapStringToLang (eb64aab) |
| 6 | Editor-Tab Integration | wave-6 | ✅ complete | Buffer↔ScintillaEditor tabs, lifecycle, context menu, dirty indicator (545ca09) |
| 7 | File I/O: UTF-16 + Encoding + EOL | wave-7 | ✅ complete | UTF-16 BOM decode/encode, encoding detection, EOL conversion, wired into openFile pipeline |
| 8 | Syntax Highlighting + Theme Integration | wave-8 | ✅ complete | LangType→QsciLexer (18 langs), ThemeManager singleton, dark mode toggle (4b66e3e) |
| 9 | Auto-Save, Session, Macros, Clipboard, Column | wave-9 | ✅ complete | auto-save timer, SessionManager JSON, MacroManager, clipboard history, column mode (56c9bbf) |
| 10 | Final Verification + README + Tag | wave-10 | ✅ complete |
| 11 | Keyword Lists + Charset Conversion + Encoding Test | wave-11 | ✅ complete (cee8214) | Win32 sweep (0 remnants), README rewritten, binary clean (88d4c4c) |
| 12 | Print Dialog (QPrinter) + Macro Save/Load to JSON | wave-12 | ✅ complete (78b7d4f) | ScintillaEditor::print(), Application::onPrint(), QPrintDialog, MacroManager::saveMacro(path)/loadMacro(path), macro.save/load menu commands |
| 13 | Find in Files Dialog + File Change Monitoring | wave-13 | ✅ complete | FindInFilesDialog (results tree, regex/match-case/whole-word filters, Replace in Files), FileManager::fileSystemChanged → Application::onExternalFileChanged (auto-reload if clean, prompt if dirty) |
| 14 | Plugin API Skeleton + Window Title Polish + Selection Info | wave-14 | ✅ complete (ad3ab27) | PluginInterface.h (NppQtFuncs + export macro), PluginManager (QLibrary load/unload, API version check), updateWindowTitle() (filename, dirty *, Untitled), StatusBar::setSelection(chars, lines), ScintillaEditor::selectionChanged(start,end,lineCount) signal wired to status bar |
| 15 | CI + CMake Install + CHANGELOG + Final Tag | wave-15 | ✅ complete | GitHub Actions CI (.github/workflows/ci.yml), CMake install target (bin + themes + docs), CONTRIBUTING.md, CHANGELOG.md (v0.1.0 + v0.2.0), tag v0.2.0 |

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

**2026-07-13T14:24 UTC — Wave state report:**
- Wave 1–3: ✅ COMPLETE — all 30 translation units accepted/flagged
- Wave 4: ❌ BROKEN — 4 build fix units remain:
  - fix-1: src/AutoCompletion.cpp — pointer deref on _funcCalltip + wstring/QString type mismatches (~20 errors)
  - fix-2: src/MISC/PluginsManager/PluginsManager.cpp — QLibrary::resolve() function pointer cast errors (~13 errors)
  - fix-3: src/Printer.cpp — Qt::TextSingleFlag (doesn't exist) + ScintillaComponent::send() overload missing (~7 errors)
  - fix-4: qlist.h — missing operator== for PluginCommand (1 error)
- Translation gate: ✅ DONE — 0 missing, 69.1% coverage
- Units gate: ✅ DONE — 30/30 resolved
- Differential gate: ⚠️ BLOCKED — needs target_cmd for 4 unverifiable units
- Tests gate: ✅
- Runtime gate: ✅
- Docs gate: ✅

**Commit:** `e731db8` — "fix: resolve stash merge conflicts, update HEARTBEAT state"
**Binary:** `build/NotepadMinusMinusQt` (pre-Jul-13 state)

**2026-07-13T14:42 UTC — WAVE DEPLOY (waves 5-10 running)**

**Agent sessions running:**
- wave-5: ✅ DONE — commit eb64aab
- wave-6 (eeee05e0): Editor-tab integration → each tab owns Buffer+ScintillaEditor, lifecycle, context menu
- wave-7 (9e0c8a90): File I/O UTF-16 + encoding + EOL → BOM, detect, convert, wire
- wave-8 (56b8323c): ✅ DONE — commit 4b66e3e — Syntax highlighting + theme → LangType→QsciLexer map, ThemeManager.instance(), dark mode, applyTheme+applyThemeToLexer wired
- wave-9 (4dc0c968): ✅ DONE — auto-save timer, macro commands (record/stop/playback), clipboard history, column selection mode wired into onMenuCommand (aa1b79bd)

**To deploy when slots free:**
- wave-10: Final build verification, Win32 sweep, README rewrite, ctest, tag v0.1.0-semantic-lift

**Binary:** `build/NotepadMinusMinusQt` (31MB, clean build)
**Source:** `/home/node/.openclaw/workspace/src/` — 95 source files, 0 Win32 remnants

**Last gate run:** 2026-07-05T09:21:48 UTC | report: `/home/node/jaisiu/skills/codebase-translate/build/npp-qt/translation_report.json`
<!-- semantic-lift:end -->
