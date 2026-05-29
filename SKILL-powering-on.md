# SKILL-powering-on

Purpose: a fast, aggressive workflow for pushing a large, partially ported codebase from red to greener states by removing blocker classes of errors in descending impact order.

## Intent

"Power on" means:
- Keep momentum high.
- Fix root-cause clusters, not one-off symptoms.
- Rebuild often and use compiler/linker feedback as the task queue.
- Prefer compatibility shims and minimal semantic-safe edits over broad refactors.

This is especially useful during platform ports (for example, Win32 -> Qt) where many symbols and APIs are in flux.

## Operating Principles

- Work top-down by build phase:
  1. Parse/compile blockers
  2. ODR/multiple-definition blockers
  3. Link-time undefined references
  4. Runtime correctness hardening
- Deduplicate errors into buckets before editing.
- Solve one bucket at a time, then rebuild.
- Keep edits localized and reversible.
- Avoid "warning polishing" until hard errors are gone.

## Loop (Power Cycle)

1. Run full build (`cmake --build build -j4`).
2. Extract only blockers (errors, undefined references, duplicate symbols).
3. Group into root-cause buckets.
4. Pick the highest-leverage bucket (touches many errors).
5. Apply minimal fix.
6. Rebuild immediately.
7. Repeat until linker progresses.

## Bucket Strategy

### A) Compile/API Drift
Typical issues:
- Signature mismatches between header and source
- Wrong Qt API usage after port
- Type drift between legacy and shim classes

Tactics:
- Make declarations match definitions exactly.
- Add compatibility wrappers where behavior is known.
- Prefer adapting call sites over deep subsystem rewrites.

### B) ODR / Multiple Definitions
Typical issues:
- Same symbol defined in multiple translation units
- Static storage defined in headers incorrectly

Tactics:
- Keep exactly one owning definition.
- Move shared storage to one `.cpp`, declaration only in headers.
- If duplicate implementation files exist, centralize implementation and keep others as stubs/includes.

### C) Undefined References
Typical issues:
- Declared methods never implemented
- MOC-generated symbols expecting missing slots/ctors/dtors
- Legacy classes present in headers but missing implementation body in port

Tactics:
- Prioritize classes that fan out into many missing symbols.
- Add temporary no-op/return-safe implementations to satisfy link first.
- Replace missing legacy paths with shim equivalents where semantics are acceptable.

## Safety Rails

- Do not revert unrelated user changes.
- Do not use destructive git reset/checkout operations.
- Preserve semantic intent; mark temporary stubs clearly by naming and location (not noisy comments).
- Re-check headers mirrored in multiple paths (`include/` vs `src/.../include`) to avoid split declarations.

## Example: npp-qt Powering-On Sequence

Observed progression in this repo:
- Compile blockers reduced by fixing class collisions, API drifts, and macro conflicts.
- ODR blockers reduced by consolidating `Win32_IO_File` implementation ownership and fixing static member definition style.
- Build moved from compile failures to link stage.
- Current primary queue is unresolved link symbols across docking, dialogs, and legacy feature classes.

Representative fixes that fit this skill:
- Header/source signature alignment (`NppXml`, `Parameters`, `Notepad_plus` function signatures).
- Shim class renaming to avoid MOC/type collisions.
- Single-definition ownership for shared symbols (`Win32_IO_File`, static counters).
- Minimal behavioral stubs for missing yet non-critical paths to unblock deeper modules.

## Prioritization Heuristic

When multiple linker clusters appear, prioritize by blast radius:
1. Foundational UI infrastructure (docking base classes)
2. Central app orchestrator methods (`Notepad_plus` handlers)
3. Frequently referenced dialogs (`FindReplaceDlg`, `GoToLineDlg`, `UserDefineDialog`)
4. Utility statics/constants (`TabBarPlus::_colors`, MD5 internals)

## Done Criteria Per Phase

- Compile phase done: no C++ compile errors.
- ODR phase done: no multiple definition linker errors.
- Link phase done: executable links successfully.
- Runtime phase done: app launches and critical flows do not crash immediately.

## Command Set

- Configure: `cmake -S . -B build`
- Build: `cmake --build build -j4`
- Fast rerun after edits: same build command; avoid clean builds unless state is corrupted.

## Anti-Patterns

- Chasing every warning while errors remain.
- Large refactors during red build conditions.
- Editing many subsystems before a rebuild.
- Creating parallel implementations without clear ownership.

## Practical Output Format (per cycle)

For each cycle, record:
- What failed (bucketed)
- What changed (files + intent)
- What moved (new build frontier)
- Next top blocker cluster

This keeps the process deterministic, reviewable, and fast under heavy porting pressure.
