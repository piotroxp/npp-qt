# Troubleshooting Notepad--Qt

Common issues and how to diagnose and fix them.

---

## Crashes on Startup (with display)

This is the most common issue when running on a real display.

### Step 1: Get a backtrace with GDB

```bash
# Using the pre-built binary
gdb /home/piotro/npp-qt-fix-build/NotepadMinusMinusQt
# or using a freshly-built binary
gdb ./build/NotepadMinusMinusQt

# In GDB:
(gdb) run
# ... app crashes ...
(gdb) bt full
(gdb) thread apply all bt
(gdb) quit
```

Save the full backtrace. Look for:
- The crash location (`0x... in function()`)
- The call stack — the **first non-library frame** is usually the bug
- Any `QWidget` / `QMainWindow` construction that failed

### Step 2: Try the headless build

```bash
QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt --help
```
If headless works but GUI crashes, the issue is display-related (see below).

### Step 3: Check with ASAN

ASAN (Address Sanitizer) catches memory errors (use-after-free, double-free, buffer overflow).

```bash
# Build with ASAN
cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug -DNPP_DEBUG_SANITIZERS=ON
cmake --build build-asan -j$(nproc)

# Run
QT_QPA_PLATFORM=offscreen ./build-asan/NotepadMinusMinusQt
# or with display:
./build-asan/NotepadMinusMinusQt
```

ASAN prints a detailed report at crash time:
```
=================================================================
==12345==ERROR: AddressSanitizer: heap-use-after-free on address 0x...
#0 0x... in QMainWindow::show() main.cpp:42
#1 0x... in Application::showMainWindow() Application.cpp:123
```

### Step 4: Common crash causes

**Double `IncrementalSearchDialog` creation:**
If `Application::initialize()` crashes, check if `_incrementalSearch` is created twice (once in setupUI, once after showMainWindow). Both instances are parented to `_mainWindow`, causing a double-deletion.

**QOffscreenSurface crash (no display):**
If running with a display but `QT_QPA_PLATFORM` is set to `offscreen`, QScintilla tries to create an OpenGL surface that doesn't exist. Remove the env var.

**Stale plugin `.so` files:**
```bash
# Start without plugins
./build/NotepadMinusMinusQt --no-plugins
```

---

## Headless / Offscreen Issues

### "QOffscreenSurface: Failed to create" error

This means Qt tried to initialize OpenGL for the offscreen platform but couldn't.

```bash
# Check if offscreen works at all
QT_QPA_PLATFORM=offscreen ./build/NotepadMinusMinusQt --help
echo $?
```

If it fails with a surface error, the QScintilla editor component requires OpenGL. Solutions:

1. **Use a dummy X server** (Xvfb):
   ```bash
   # Install Xvfb
   sudo apt install xvfb
   # Run with virtual framebuffer
   xvfb-run ./build/NotepadMinusMinusQt file.txt
   ```

2. **Use the container's offscreen build** — the build in the Jaisiu container is built with headless support.

### "Cannot create QApplication" on remote machine

If you're SSH-ing to another machine:

```bash
# Forward X11
ssh -X user@host
./build/NotepadMinusMinusQt

# Or use SSH with trusted X11 forwarding
ssh -Y user@host
./build/NotepadMinusMinusQt
```

Also ensure `DISPLAY` is set:
```bash
echo $DISPLAY   # Should show something like :0 or :10
```

---

## Build Errors

### "Could not find Qt6"

```
CMake Error at CMakeLists.txt:10 (find_package):
  Could not find a package configuration file provided by "Qt6" with any
  of the name "Qt6Config.cmake" or "qt6/Qt6Config.cmake".
```

Fix:
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-tools-dev

# Check Qt6 is found
pkg-config --modversion Qt6Core   # should print version
```

### "Could not find qscintilla2"

```
Could not find qscintilla2
```

Fix:
```bash
sudo apt install libqscintilla2-qt6-dev

# Verify
pkg-config --modversion Qt6Scintilla2
```

### "Ninja not found"

Either install Ninja or omit the `-G Ninja` flag:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release    # uses Make by default
cmake --build build -j$(nproc)
```

### Compiler errors in your changes

```bash
# Clean and rebuild
cmake --build build --target clean
cmake --build build -j$(nproc)
```

---

## Encoding Issues

### File opens with wrong encoding

```bash
# Check what encoding was detected
# Look in the status bar (bottom right of window)
# Encoding shown as "UTF-8", "Windows-1252", etc.
```

If wrong:
1. `Encoding` menu → select correct encoding
2. Close and reopen the file

### BOM not written on save

Only these encodings write BOM on save: `UTF-8-BOM`, `UTF-16-LE-BOM`, `UTF-16-BE-BOM`.

To force BOM for UTF-8: `Encoding → UTF-8 with BOM`

### Large file (>5 MB) loads slowly or partially

Large files use partial loading. The status bar shows:
```
Loading... 45% (2.3 MB / 5.1 MB)
```

If loading is interrupted, the file stays in partial mode. Close and reopen.

---

## Panel Issues

### Document Map not scrolling with editor

The Document Map uses a 60 fps debounce. If it stops syncing:
1. Toggle the panel off (`View → Document Map`) then back on
2. Check for console errors in GDB

### Function List not parsing

1. Make sure the language is set correctly (`Language → ...`)
2. The function list only works for languages with regex parsers: C/C++, Python, JavaScript, Rust, Go, PHP, Java, Ruby
3. Large files (>1 MB) may skip function list parsing

### File Browser not showing changes

The file browser uses `QFileSystemWatcher`. If files are created outside Notepad--Qt, the tree may not refresh automatically. Press `F5` in the File Browser panel to refresh.

---

## Plugin Issues

### Plugin crashes the app

```bash
# Start without plugins
./build/NotepadMinusMinusQt --no-plugins

# If that works, the plugin is the problem
# Move plugins out of the plugin directory
mv ~/.config/notepad--qt/plugins/ ~/.config/notepad--qt/plugins.bak/
```

### Plugin not loading

Plugins must export one of:
- `nppqt_getPlugin()` — native Notepad--Qt plugin
- `npp_getFunctionItems()` — legacy Notepad++ plugin (stub)

Check plugin errors:
```bash
# Run and look for plugin loading messages
./build/NotepadMinusMinusQt 2>&1 | grep -i plugin
```

---

## Keyboard Shortcuts Not Working

See the keyboard shortcuts table in [README.md](../README.md#keyboard-shortcuts).

Common DE conflicts:

| Shortcut | DE Issue | Workaround |
|----------|----------|------------|
| `Alt+Tab` | All DEs — window switcher | `Ctrl+Tab` for tabs |
| `F10` | KDE — menu bar | `F11` for fullscreen |
| `PrintScreen` | GNOME — screenshot | `Ctrl+Alt+P` |
| `Alt+Space` | KDE — krunner | Unavoidable on KDE |

---

## Performance Issues

### Editor is slow with large files

Notepad--Qt handles large files via partial loading (only visible portions are in memory). If still slow:
1. Disable syntax highlighting for the file (`Language → Normal Text`)
2. Close other tabs
3. Disable the Document Map panel

### Memory usage is high

Notepad--Qt uses one `Buffer` per tab. Each open file consumes memory proportional to its size. Close tabs you don't need.

### High CPU usage when idle

This may indicate a busy-wait loop or a timer firing too frequently. Check:
```bash
top -p $(pidof NotepadMinusMinusQt)
```
If CPU is consistently above 1%, there may be an infinite loop in a timer handler.

---

## Reporting Bugs

When reporting a bug, include:

1. **Command used** and **platform** (`uname -a`)
2. **Qt platform:** `echo $QT_QPA_PLATFORM`
3. **Binary:** pre-built or freshly built?
4. **GDB backtrace** (`bt full`) if it crashed
5. **ASAN report** if you have one
6. **Steps to reproduce**
7. **Expected vs actual behavior**
