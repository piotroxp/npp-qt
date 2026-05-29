# SKILL-semantic-lift-validation

## Objective
Validate semantic lifting progress from `../notepad-plus-plus-translation` (Windows/PowerEditor stack) into `npp-qt` (Qt/C++ stack), with focus on preserving behavior while replacing Win32/WinForms-era UI patterns with Qt equivalents.

## Repos and Roles
- Source of truth: `../notepad-plus-plus-translation`
- Target implementation: `./` (`npp-qt`)
- Existing codemaps:
  - `CODEMAP.md` (target)
  - `../notepad-plus-plus-translation/CODEMAP.md` (source)

## Translation Direction
- UI framework migration: Win32/WinForms-style event/message patterns -> Qt signal/slot and widget model.
- Build migration: Visual Studio/GCC Windows-centric setup -> CMake + Qt6 cross-platform build.
- Platform APIs: direct Windows APIs -> `WindowsCompat`/`Win32QtShim` abstraction + Qt-native services.

## Validation Workflow
1. Define parity scope by module (File, Edit, Search, View, Encoding, Session, Plugins, Docking).
2. Map source modules in `PowerEditor/` to target modules in `src/` + `include/`.
3. Check API surface parity (classes, methods, actions, command IDs, menu bindings).
4. Check behavior parity (state transitions, edge-case handling, persistence semantics).
5. Check integration parity for Scintilla/Lexilla layers.
6. Record gaps and classify by severity.
7. Promote reusable checks into repeatable regression checklist.

## Current Gap Scan (High-Level)

### Critical Gaps
- Missing/partial implementation companions for several declared integration headers (header-only or scaffolded surfaces still present).
- Incomplete end-to-end editor workflow parity in `ScintillaComponent` layer (tab lifecycle, find/replace, buffer flow cohesion).
- Docking/panel subsystem not at source parity (Project/Function/File panels and panel manager interactions).

### High Priority Gaps
- Command routing parity: full action set and accelerator behavior from source menus not yet fully reflected in Qt action graph.
- Session/config parity: XML/settings persistence paths exist, but source-compatible completeness is not yet guaranteed.
- Plugin lifecycle parity: plugin discovery/load/command binding still behind source behavior.

### Medium Priority Gaps
- Full localization parity: translation catalogs exist, but source string coverage is incomplete.
- Function list and XML schema validation assets from source test corpus not yet systematically integrated into target validation pipeline.
- Encoding edge-case parity (legacy code pages/BOM handling) requires broader fixture-driven verification.

## Semantic Mapping Heuristics
- Preserve intent, not API shape: replace message handlers with slots while keeping action semantics identical.
- Keep command IDs/concepts stable where possible for traceability.
- Prefer Qt-native primitives over emulation unless behavior requires compatibility shim.
- Treat source tests/fixtures as executable specification for parity.

## Gap Register Template
Use this schema for each discovered gap:

- `Gap ID`:
- `Source Location`:
- `Target Location`:
- `Behavior Expected`:
- `Current Behavior`:
- `Severity` (Critical/High/Medium/Low):
- `Proposed Lift Strategy`:
- `Validation Method`:
- `Status` (Open/In Progress/Closed):

## Immediate Next Validation Targets
1. Build a one-to-one module matrix (`PowerEditor/*` -> `src/*`, `include/*`).
2. Diff command/action surface (menus + shortcuts) and enumerate missing handlers.
3. Port source FunctionList/XML validation fixtures into a runnable target-side test harness.
4. Verify Scintilla/Lexilla integration boundaries with compile + runtime smoke scenarios.

## Definition of Done for a Lifted Module
- Compiles in Qt6 target.
- Exposes equivalent user-visible behavior to source implementation.
- Persists/restores relevant settings/session data compatibly.
- Has at least one parity test or fixture-backed validation.
- Added to codemap and gap register with status updated.

## Agentic Execution Protocol (Until Build Works)

Use these instructions as a strict autonomous loop for an AI programming agent. The stop condition is a successful `npp-qt` build.

### Global Mission
- Keep iterating on semantic lift and implementation gaps until `npp-qt` configures and builds successfully with CMake.
- Do not stop at analysis-only milestones; always end a cycle with either a code change, a validated fix, or a clearly logged hard blocker.

### Hard Stop Condition
- `cmake -S . -B build` succeeds.
- `cmake --build build` succeeds.
- No compile or link errors remain.

### Safety and Scope Constraints
- Never delete large subsystems to make build pass.
- Never stub by silently dropping required behavior; temporary scaffolding is allowed only if marked with `TODO(parity)` and logged in gap register.
- Prefer smallest viable fix that preserves semantic parity direction.
- Keep changes focused; one cohesive problem cluster per commit-sized unit of work.

### Required Working Memory Files
- `CODEMAP.md`
- `SKILL-semantic-lift-validation.md` (this file)
- `TRANSLATION_SUMMARY.md`
- `GAP_REGISTER.md` (create if missing)
- `AGENT_PROGRESS.md` (create if missing; append-only run log)

### Iteration Loop
1. **Sync Context**
   - Read latest compiler errors and current open gaps.
   - Pick one highest-impact open gap that blocks configure/build.
2. **Plan Micro-batch**
   - Define 1-3 concrete edits with expected build impact.
   - Define validation commands for this micro-batch.
3. **Implement**
   - Apply code changes in target repo.
   - If source behavior is unclear, inspect `../notepad-plus-plus-translation` and mirror semantics in Qt style.
4. **Validate**
   - Run:
     - `cmake -S . -B build`
     - `cmake --build build -j`
   - Capture first failing error class if build still fails.
5. **Record**
   - Update `GAP_REGISTER.md` status fields.
   - Append to `AGENT_PROGRESS.md`:
     - timestamp
     - gap IDs touched
     - files changed
     - command results
     - next planned gap
6. **Repeat**
   - Continue loop until hard stop condition is met.

### Gap Prioritization Order (Strict)
1. Build/config blockers (CMake, missing symbols, missing source files, include/link failures).
2. Core editor workflow blockers (`ScintillaComponent`, buffer lifecycle, tab/view cohesion).
3. Command routing + shortcut parity required for core app operability.
4. Session/config persistence parity.
5. Docking/panel and plugin lifecycle parity.
6. Localization/test-fixture completeness.

### Build Failure Triage Policy
- Always fix the earliest error in the compiler/linker output first.
- If multiple errors share root cause, fix as one micro-batch.
- After each micro-batch, rebuild before touching unrelated modules.

### Source-to-Target Mapping Policy
- For each missing target behavior, log exact source reference path and symbol.
- Port behavior, not framework mechanics:
  - message maps/events -> Qt signals/slots
  - Win32 resource/menu ops -> Qt `QAction`/`QMenu` graph
  - registry/config access -> Qt settings/XML abstractions already established

### Required Artifacts

#### `GAP_REGISTER.md` Format
For each gap, maintain:
- `Gap ID`
- `Category` (Build, EditorCore, Commands, Persistence, Docking, Plugins, i18n, Tests)
- `Source Location`
- `Target Location`
- `Severity`
- `Blocker` (Yes/No)
- `Status` (Open/In Progress/Closed)
- `Evidence` (error text or behavior note)
- `Fix Summary`
- `Validation Result`

#### `AGENT_PROGRESS.md` Entry Format
- `Run Timestamp`:
- `Objective for Cycle`:
- `Changes Applied`:
- `Build Result`:
- `Open Blockers`:
- `Next Gap ID`:

### Definition of Done (Project-Level)
- `npp-qt` configures and builds successfully in local environment.
- All build-blocking gaps marked Closed in `GAP_REGISTER.md`.
- Remaining non-blocking parity gaps are documented with severity and next action.
- Codemap and progress artifacts reflect latest architecture and lift status.
