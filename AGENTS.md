# AGENTS.md — Master Branch Coherence Protocol

## Diagnosis: What's Wrong

The git history on `master` shows systematic problems from multiple AI agents working without coordination:

| Problem | Evidence |
|---------|----------|
| **Two agents colliding** | `AI Agent <agent@pryzm.ai>` and `Jaisiu Agent <agent@jaisiu>` both committed directly to master, interleaving conflicting work. |
| **Massive overwrite (~18K lines lost)** | `6ffbdb2` and `f831a12` share the message "Massive feature work" but diff shows 129 files changed, 18,367 DEL, 2,414 ADD. Entire subsystems (PluginManager, FindInFiles, ScintillaCtrls, etc.) wiped and recreated differently. |
| **Files lost, then restored** | `744e442` "restore FunctionListPanel.cpp (668 lines) from wave10 state" — a file was lost and had to be recovered via reflog. |
| **Build artifacts in history** | Compiled binary `NotepadMinusMinusQt` committed and removed 5×. `.o` files, `build/` dir, `.jaisiu/` session artifacts all committed. |
| **History rewritten** | 2 rebases + 7+ amends on master. Original commits (`fc86faa` etc.) only survive in reflog. Timeline reordered. |
| **Duplicate commits** | `545ca09` and `aa1b79b` are identical "feat(wave6)" commits (one has extra session artifacts). |
| **Sloppy messages** | `"em"`, `"npp-qt"`, `"Readme"`, `"clear agent workspace from npp-qt"` — no human review. |

---

## 1. Ground Rules (Every Session)

### 1.1 Branch Discipline
- **NEVER commit directly to `master`.** All work MUST be on feature branches (`fix/`, `feat/`, `docs/`, `chore/`).
- Open a PR (even if you can't merge it) so the diff is visible.
- `master` is for clean, reviewed history only.

### 1.2 No History Rewriting
- **No `git rebase` on shared branches.** Ever.
- **No `git commit --amend`.** If you need to fix a commit, make a new fixup commit.
- Pull with `--ff-only`, never `--rebase`.

### 1.3 No Binary or Build Artifact Commits
- `build/`, `build-*/`, `b/`, `*.o`, `*.a`, `NotepadMinusMinusQt`, `Testing/`, `__pycache__/`, `.openclaw/`, `.jaisiu/` — NEVER commit these.
- `.gitignore` MUST be kept up to date. Verify with `git status --short` before every commit.
- If you see an artifact tagged as `A` (added) in your diff, STOP and fix `.gitignore` first.

### 1.4 Commit Message Standard
```
type(scope): brief summary in lowercase

type: fix, feat, docs, chore, refactor, test, quality
scope: which module (core, editor, dialogs, panels, build, tests, ci, docs)

Body explains why (not what). Wrap at 72 chars.
```

---

## 2. Recovery: Clean Up the Working Tree

### 2.1 Remove Stale Build Artifacts From Disk

There are multiple orphaned build directories. Nuke them:

```bash
rm -rf build/ build-asan/ build_fresh/ build_nppqt/ b/ build_test/
rm -rf __pycache__/
rm -f build.sh
```

> Run this AFTER verifying the canonical build path from CMakeLists.txt.

### 2.2 Ensure `.gitignore` Is Comprehensive

Check that `.gitignore` covers ALL artifact patterns:

```
build*/
build-*/
b/
*.o
*.a
*.so
NotepadMinusMinusQt
NotepadMinusMinusQt_autogen/
Testing/
__pycache__/
*.pyc
.openclaw/
.jaisiu/
.memory/
gource_*.mp4
avatars/
```

Commit this fixup as `chore(gitignore): cover all build artifacts and agent session dirs`.

---

## 3. Recovery: Investigate Lost Work

### 3.1 Check for Divergent Work Between AI Agents

Compare the two "Massive feature work" commits to identify function-level code that existed in one but not the other:

```bash
# Check what files existed in 6ffbdb2 but NOT in f831a12
diff <(git ls-tree -r --name-only 6ffbdb2 | sort) \
     <(git ls-tree -r --name-only f831a12 | sort) | grep "^<"

# Check files deleted in f831a12 that contained non-stub implementations
```

For each file that was present in `6ffbdb2` (AI Agent) but absent after `f831a12` (Jaisiu Agent):
- Read the `6ffbdb2` version: `git show 6ffbdb2:<path>`
- If it contained real logic (not Win32 stubs), consider porting it forward into the current tree.

### 3.2 FunctionListPanel

Current version may be incomplete. The file was restored from "wave10 state" in `744e442`. Check if the `6ffbdb2` version had a more complete implementation:

```bash
git show 6ffbdb2:src/panels/FunctionListPanel.cpp | wc -l
git show 6ffbdb2:src/panels/FunctionListPanel.h | wc -l
```

If the `6ffbdb2` version is more complete, merge the relevant logic into the current file.

### 3.3 Check Dangling Commits for Orphaned Work

```bash
git fsck --lost-found | grep "dangling commit"
```

For each dangling commit, inspect it:

```bash
git log --oneline <dangling_hash>
git show --stat <dangling_hash>
```

If it contains valuable work not present on master, cherry-pick it forward:

```bash
git cherry-pick <hash>
```

---

## 4. Recovery: Squash the Audit Trail (Optional)

If you want a clean history, this is the nuclear option. Only do this if you're prepared to force-push.

### 4.1 Squash Into Meaningful Chunks

```bash
# Interactive rebase back to the first real commit
git rebase -i 4a67174
```

Squash policy:
- Collapse chains of "fix fix fix" / "remove binary" / "restore" / "em" / "npp-qt" into their parent meaningful commits
- Keep meaningful `feat:`, `fix:`, `docs:`, `test:` commits distinct
- Drop commits that only add binary artifacts (the `.gitignore` fix already prevents re-occurrence)

### 4.2 Reorganize Author Attribution

Commits by `AI Agent <agent@pryzm.ai>` and `Jaisiu Agent <agent@jaisiu>` should be attributed to the human who reviewed them, or left as-is with a clear note that these are AI-generated. Use `git commit --amend --author=` if desired, but never rewrite history after pushing.

---

## 5. Forward Workflow: How to Lift to Coherence

### 5.1 Every Task Starts With `git pull --ff-only master`

```
git checkout master
git pull --ff-only origin master
git checkout -b feat/my-feature
```

### 5.2 Before Every Commit

Run the checklist:

```bash
# 1. Check for accidentally-tracked artifacts
git status --short

# 2. If you see *.o, build/, NotepadMinusMinusQt, etc.: STOP
#    Fix .gitignore and unstage: git rm --cached <path>

# 3. Build and test
cd build && cmake --build . && QT_QPA_PLATFORM=offscreen ctest --output-on-failure

# 4. Only then commit
git commit -m "type(scope): description"
```

### 5.3 After Every Feature

```bash
git checkout master
git pull --ff-only origin master
git checkout feat/my-feature
git rebase master   # only for YOUR feature branch, never master itself
# push and open PR
```

### 5.4 Code Review Checklist (for the human or another agent)

Before merging any branch into master, verify:

- [ ] No build artifacts in the diff (`git diff master..HEAD --stat | grep -E '\.o|build/|NotepadMinusMinusQt'`)
- [ ] No `.openclaw/`, `.jaisiu/`, or session artifacts
- [ ] Build passes clean (`cmake --build build`)
- [ ] All tests pass (`cd build && QT_QPA_PLATFORM=offscreen ctest --output-on-failure`)
- [ ] Commit messages follow the standard
- [ ] No `git rebase` or `amend` in the branch history
- [ ] No large binary blobs (`git diff master..HEAD --stat | grep -i bin` should be empty)

---

## 6. Module Inventory — Current State

This is the known state of each major module. Use it to prioritize work.

| Module | Status | What's Needed |
|--------|--------|---------------|
| `core/Application` | Mostly ported | ~991 lines vs upstream; some stubs remain |
| `core/Buffer` | Solid | Core buffer model complete |
| `core/Notepad_plus` | **Critical gap** | ~52 lines vs 9331 upstream (99% stubbed). The main app logic controller is missing. |
| `core/NppIO` | Partial | File I/O ported but incomplete (was rewritten between agents) |
| `core/FileManager` | Good | Full lifecycle working |
| `core/Parameters` | Partial | Config system needs work |
| `core/NppBigSwitch` | Good | 70+ command dispatch implemented |
| `editor/ScintillaEditor` | Good | Core editor working |
| `editor/ScintillaEditView` | ~15% | Most of the upstream view logic stubbed |
| `editor/SyntaxHighlighter` | Good | 54 languages implemented |
| `dialogs/` | Mixed | FindReplace works; PreferenceDialog (~58KB) is missing entirely; UserDefineDialog partial |
| `panels/FunctionListPanel` | Partial | Was restored from backup, may be incomplete vs `6ffbdb2` version |
| `panels/FileBrowserPanel` | Good | Working |
| `panels/DocumentMapPanel` | Good | Working |
| `plugins/PluginManager` | **Stub only** | ~28KB upstream, barely started |
| `WinControls/` (47 files) | Partial | Various Win32→Qt ports at different stages |
| `tests/` | 7/7 passing | Good coverage for core, needs expansion for UI/panels |

---

## 7. Quick Reference: Common Commands

```bash
# Build
cd build && cmake .. -G Ninja && ninja

# Build with sanitizers
mkdir -p build-asan && cd build-asan
cmake .. -G Ninja -DNPP_DEBUG_SANITIZERS=ON
ninja

# Run all tests
cd build && QT_QPA_PLATFORM=offscreen ctest --output-on-failure

# Run a single test
cd build && QT_QPA_PLATFORM=offscreen ./src/tests/test_buffer

# Check what work might be lost between agent versions
git diff --stat 6ffbdb2..f831a12 -- src/ | sort -k2

# Find files in old agent's tree not in current
comm -23 <(git ls-tree -r --name-only 6ffbdb2 | sort) \
          <(git ls-tree -r --name-only HEAD | sort)

# List dangling (potentially lost) commits
git fsck --lost-found 2>&1 | grep "dangling commit"

# View a dangling commit
git show --stat <hash>
```
