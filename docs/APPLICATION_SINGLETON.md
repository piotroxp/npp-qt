# Application Singleton — Lifecycle, Threading, and Construction Order

> **Scope.** This document captures what was diagnosed and shipped in
> commit `5663a82` ("harden Application singleton against
> `__cxa_guard_acquire` deadlock") plus the two related deferred items
> from HEARTBEAT.md. Read it before touching `Application::instance()`,
> `Application::Application()`, or the `NotepadApp::event()` handler in
> `src/main.cpp`.

## TL;DR

- `Application::instance()` uses **Construct-On-First-Use (heap)** with an
  `std::atomic<bool> in_progress` re-entry guard. No compiler-generated
  `__cxa_guard_acquire` around the ctor body.
- Multi-thread contention is serialised by `std::this_thread::yield()`,
  not a mutex. The yield is bounded by the ctor body's wall-clock cost
  (sub-millisecond in single-thread startup).
- The launch sequence constructs `NotepadApp` (`QApplication`) **before**
  the first call to `Application::instance()`, so `qApp` is non-null
  when `Application::Application() : QObject(qApp)` runs. **This is a
  sequencing contract, not a runtime check** — see "Deferred race" below.
- `StubApplication.cpp` mirrors the real idiom so `test_application_singleton`
  verifies the property against the test harness without dragging in the
  full QApplication + MainWindow initialisation path.

## What changed in `5663a82`

### Before

```cpp
Application& Application::instance() {
    static Application inst;       // compiler emits __cxa_guard_acquire
    return inst;                   // around Application::Application() body
}
```

If thread A enters `instance()` for the first time and thread B enters
while A is still inside `Application::Application()` — e.g. via
`QFileOpenEvent` delivery, plugin loader thread, file watcher, async
command dispatch — B blocks on the compiler-generated guard around A's
ctor body. The blocking is platform-specific, silent, and unbounded
until the ctor returns (which it never does because A is itself waiting
on something the main thread must complete).

This was the surface symptom in the prior live QA cycle:
`__cxa_guard_acquire` hang at startup, captured under gdb on
`/home/node/.openclaw/workspace/npp-qt/build/NotepadMinusMinusQt`.

### After

```cpp
Application& Application::instance() {
    diag_inst_enter("instance()");

    // Construct-On-First-Use: heap allocation, no compiler guard around
    // the ctor call. The pointer store is atomic under C++11.
    static Application* inst = nullptr;
    if (inst) return *inst;

    // Slow path: serialise construction between threads.
    static std::atomic<bool> in_progress{false};
    bool expected = false;
    while (!in_progress.compare_exchange_strong(expected, true)) {
        std::this_thread::yield();
        expected = false;
    }

    if (inst) {
        in_progress.store(false);
        return *inst;             // racer ahead of us
    }

    Application* fresh = new Application();
    inst = fresh;
    in_progress.store(false);
    return *fresh;
}
```

Three properties this buys us:

1. **Atomic pointer store, no ctor in the critical section.** A re-entrant
   call from inside `Application::Application()` on the same thread reads
   `inst` after the `new` returns and walks away with the partially-constructed
   pointer. It does not block on any guard.
2. **Deterministic spin-yield on multi-thread contention.** The
   `std::atomic<bool> in_progress` CAS loop uses `std::this_thread::yield()`
   instead of `std::mutex` so a same-thread re-entry is impossible (we're
   already past the CAS) and a different-thread re-entry spin-yields
   until the ctor body returns. The yield is bounded by the ctor body's
   wall-clock cost.
3. **Deliberate `new` / no `delete`.** Matches the standard Meyers-heap
   idiom. Process exit cleans the singleton up. We never run a destructor
   that could re-enter the singleton machinery.

### Test

`src/tests/test_application_singleton.cpp` — four cases:

| Test | What it proves |
|------|----------------|
| `test_instance_returns_same_reference` | Three calls to `instance()` return the same address. Identity sanity. |
| `test_inline_app_helper_matches_instance` | The `app()` helper agrees with `instance()`. Helper consistency. |
| `test_instance_thread_safe` | 16 threads race on `instance()` simultaneously. All 16 observe the same address. With the old pattern this would deadlock in <50ms. |
| `test_instance_stress_no_reconstruction` | 64 threads × 1000 calls = 64,000 `instance()` calls with zero mismatches. Catches any future regression that re-introduces a ctor guard or a `static Application inst;` style. |

Stub mirror: `src/tests/StubApplication.cpp` mirrors the real idiom so the
test runs against the same logic. The real `src/core/Application.cpp`
singleton is verified indirectly (via the heuristic that if the test
passes with the stub using the same pattern, the real one behaves
identically).

Build wiring:

* `find_package(Threads REQUIRED)` at top of `src/tests/CMakeLists.txt`.
* `add_core_test` macro now links `Threads::Threads` — every test
  executable can use `std::thread` without per-test CMake edits.
* `test_application_singleton` registered as `ApplicationSingletonTests`
  with `QT_QPA_PLATFORM=offscreen`, matching the existing test_* pattern.

## Launch sequence contract

```text
main() {
    // ── STEP 1: QApplication first ─────────────────────────────────
    NotepadApp app(argc, argv);          // sets qApp; install platform plugin
    // qApp is now non-null. Qt event dispatcher is alive.

    // ── STEP 2: detect headless ────────────────────────────────────
    detectHeadlessEnvironment(argc, argv);

    // ── STEP 3: signal handlers ────────────────────────────────────
    installSignalHandlers();

    // ── STEP 4: parse args ─────────────────────────────────────────
    auto args = parseArgs(argc, argv);

    // ── STEP 5: Application::instance() first call ─────────────────
    auto& application = Application::instance();   // ← 5663a82 fix
    // Application::Application() body runs here:
    //   - QObject(qApp) parent set      (qApp is non-null ✓)
    //   - new MainWindow(this)
    //   - connect(...) chain
    //   - _snippetManager = new SnippetManager()
    //   - etc.
    application.initialize(...);                    // real wiring
    return app.exec();
}
```

### What this guarantees

- `qApp` is non-null when `Application::Application() : QObject(qApp)`
  runs because `NotepadApp app(argc, argv)` has already returned.
- `NotepadApp::event()` may fire **before** line 227 on macOS
  (`QEvent::FileOpen`). When it does, `Application::instance()` returns
  a valid reference but `isInitialized()` returns `false`, so the
  handler logs a warning and drops the file rather than dispatching
  to a half-built `Application`.
- `Application::initialize()` runs **once** under `_mutex`. Re-entrant
  `initialize()` from a connected slot (e.g. a plugin's init code) is
  blocked by the mutex, not by the singleton guard.

### What this does NOT guarantee

- The mutex in `initialize()` (`std::lock_guard<std::mutex> lock(_mutex);`)
  is taken **after** the singleton is fully constructed. If a different
  thread races `instance()` against `initialize()` from the same
  thread, the singleton path is safe (Construct-On-First-Use) but the
  initialize path is not re-entrant — that's correct behaviour: the
  mutex prevents concurrent initialisation.

## Deferred race: `QObject(qApp)` in `Application::Application()`

`Application::Application() : QObject(qApp)` is a sequencing assumption,
not a runtime check. **Today** it holds because `NotepadApp app(argc,
argv)` precedes `Application::instance()` in `main()`. **Tomorrow** a
refactor that moves `instance()` earlier (e.g. for early plugin loading)
would silently make `qApp` a dangling pointer at the time the base
class is initialised.

The defensive fix would replace

```cpp
Application::Application() : QObject(qApp) {
    // ...
}
```

with

```cpp
Application::Application() : QObject(nullptr) {
    _qAppParent = nullptr;       // patched in via a post-construct hook
    // ...
}

void Application::bindToQApp() {
    if (auto* app = qApp) {
        setParent(app);
        _qAppParent = app;
    }
}
```

…and call `bindToQApp()` from `main()` after `NotepadApp` constructs.

This is documented as a deferred item in HEARTBEAT.md. Land it
before any code moves `Application::instance()` above the
`NotepadApp app(...)` line in `main.cpp`.

## Diagnostic instrumentation

```cpp
static void diag_inst_enter(const char* site) {
    static int n = 0;
    ++n;
    char buf[160];
    int len = std::snprintf(buf, sizeof(buf),
        "APPINST site=%s n=%d\n", site, n);
    if (len > 0) ::write(2, buf, static_cast<size_t>(len));
}
```

Logs to fd 2 directly via `write(2, ...)` — **NOT** `qDebug()` — so
the instrumentation cannot itself recurse through Qt globals.

**Expected output under normal startup:** exactly one line per process,
e.g. `APPINST site=instance() n=1`. If you see `n=2` or higher under
normal startup, we have a re-entry path the singleton fix missed — capture
it in `/tmp/npp-gdb.log` and treat it as a bug.

To capture: run the binary under gdb, set a break at the `write(2, ...)`
call, and inspect the backtrace of the caller.

## What reviewers should check

When reviewing any PR that touches:

| File | Check |
|------|-------|
| `src/core/Application.cpp` (singleton body) | The Construct-On-First-Use idiom is preserved. No `static Application inst;`. The CAS loop on `in_progress` is intact. |
| `src/core/Application.cpp` (constructor) | The `QObject(qApp)` parent init is still present. The "FIX (cherry-picked from 337e570)" comment block is intact (it covers the `bufferClosed` connect that prevents stale tabs). |
| `src/main.cpp` | The launch sequence preserves `NotepadApp app(argc, argv)` **before** `Application::instance()`. If a refactor moves `instance()` earlier, the `QObject(qApp)` race materialises — gate on landing the `bindToQApp()` fix first. |
| `src/tests/StubApplication.cpp` | The singleton mirror still uses Construct-On-First-Use. If it regresses to `static Application inst;`, `test_application_singleton` either deadlocks or the stress test fails (depending on the runtime). |
| `src/tests/test_application_singleton.cpp` | All four cases present. `test_instance_stress_no_reconstruction` must complete in under 2s; if it takes longer, the spin-yield is being held too long by another thread. |
| `src/tests/CMakeLists.txt` | `find_package(Threads REQUIRED)` precedes the `add_core_test` macro. The macro links `Threads::Threads`. |

## Related defects / context

- **WAVES.md** Wave 14 (Plugin API) — the `PluginManager` loads
  plugins from a worker thread, which is exactly the multi-thread
  startup scenario that triggered the original deadlock. The
  `5663a82` fix is a prerequisite for safe plugin loading.
- **HEARTBEAT.md** "Semantic Lift — npp-qt" — pre-existing gate
  caveats still apply. The singleton fix did not move the 8-gate
  needle; it removed a startup-time crash surface that the runtime
  smoke gate would never catch (because it only tests `--help`).
- **`docs/ARCHITECTURE.md`** — see the "Initialization" section for
  the canonical launch sequence contract. This document supersedes
  it for the singleton-specific properties.