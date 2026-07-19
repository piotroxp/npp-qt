# UI Subsystems Reference

Reference for the UI layer: MainWindow, TabBar, ToolBar, StatusBar, MenuBar, ColourPicker, and the docking system.

---

## MainWindow (`src/ui/MainWindow`)

The root `QMainWindow`. Owns all UI components, coordinates the toolbar, menu bar, status bar, tab bar, and dockable panels.

**Files:** `MainWindow.h` (185 lines), `MainWindow.cpp` (1643 lines)

### Ownership Hierarchy

```
MainWindow
  ├── MenuBar*          (owns the menu bar)
  ├── ToolBar*          (owns the toolbar)
  ├── DocTabView*       (owns the tab bar + ScintillaEditor tabs)
  ├── StatusBar*        (owns the status bar)
  ├── IncrementalSearchDialog*  (owns the incremental search bar)
  ├── QDockWidget* (FileBrowserPanel)
  ├── QDockWidget* (FunctionListPanel)
  ├── QDockWidget* (DocumentMapPanel)
  ├── QDockWidget* (ClipboardHistoryPanel)
  ├── QDockWidget* (AnsiCharPanel)
  └── QSplitter*        (main content splitter)
```

### Key Signals

| Signal | Payload | Description |
|--------|---------|-------------|
| `fileOpened(const QString&)` | path | A file was opened |
| `fileClosed(int bufferId)` | bufferId | A buffer was closed |
| `bufferActivated(int bufferId)` | bufferId | A tab was activated |
| `themeChanged(const QString&)` | name | Theme changed |

### Key Slots

| Slot | Description |
|------|-------------|
| `openFile(const QString& path)` | Open a file |
| `newFile()` | Create a new empty buffer |
| `saveFile()` | Save the active buffer |
| `saveFileAs()` | Save As dialog |
| `closeTab(int index)` | Close a specific tab |
| `gotoLine(int line, int col)` | Navigate to line |
| `toggleFullscreen()` | F11 fullscreen toggle |
| `showFindDialog()` | Open Find/Replace dialog |
| `showCommandPalette()` | Open command palette |
| `showPreferences()` | Open Preferences |

### Menu Bar Access

```cpp
MenuBar* menuBar = mainWindow->getMenuBar();
QMenu* fileMenu = menuBar->getFileMenu();
```

---

## TabBar (`src/ui/TabBar`)

Custom tab bar with drag-to-reorder, context menu, and close button.

**Files:** `TabBar.h` (252 lines), `TabBar.cpp` (1040 lines)

### Features

| Feature | Description |
|---------|-------------|
| Drag-to-reorder | Drag tabs to rearrange order |
| Close button | `×` on each tab |
| Middle-click close | Close tab on middle mouse button |
| Tab overflow | Scroll arrows when tabs don't fit |
| Tab rename | Double-click to rename (for untitled files) |
| Modified indicator | Bold or `•` prefix on dirty tabs |
| Context menu | Right-click for Close / Close Others / Close All / Pin |

### Key Methods

| Method | Description |
|--------|-------------|
| `addTab(const QString& label, int bufferId)` | Add a new tab |
| `removeTab(int index)` | Close a tab |
| `setActiveTab(int index)` | Activate tab by index |
| `moveTab(int from, int to)` | Reorder tab |
| `setModified(int index, bool)` | Show dirty indicator |
| `setLabel(int index, const QString&)` | Rename tab |

### Signals

| Signal | Payload | Description |
|--------|---------|-------------|
| `tabCloseRequested(int)` | index | User clicked × on tab |
| `tabMoved(int from, int to)` | from, to | Tab was dragged |
| `tabDoubleClicked(int)` | index | Double-click on tab |
| `tabMiddleClicked(int)` | index | Middle-click on tab |
| `activeTabChanged(int)` | index | Active tab changed |

### Guard Pattern

The TabBar `event()` filter guards against late events arriving after the underlying `_tabWidget` or `_tabBar` has been deleted:

```cpp
bool TabBar::event(QEvent* event) {
    if (!_tabWidget || !_tabBar) {
        event->accept();
        return true;  // eat the event
    }
    return QTabBar::event(event);
}
```

---

## ToolBar (`src/ui/ToolBar`)

Customizable toolbar with icon buttons, grouped by category.

**Files:** `ToolBar.h` (162 lines), `ToolBar.cpp` (715 lines)

### Button Groups

| Group | Buttons |
|-------|---------|
| File | New, Open, Save, Save All |
| Edit | Undo, Redo, Cut, Copy, Paste |
| Search | Find, Find Next, Replace, Go to Line |
| View | Fullscreen, Zoom In, Zoom Out, Zoom Reset |
| Encoding | UTF-8, Convert to... |
| EOL | LF, CRLF, CR |
| Run | Run (F5), Stop |

### Customization

The toolbar is customizable via **Settings → Preferences → Toolbar** (⚠️ dialog stub).

### Tooltip Format

```cpp
setToolTip("Open File\nCtrl+O");
```

---

## StatusBar (`src/ui/StatusBar`)

Bottom status bar showing document and application state.

**Files:** `StatusBar.h` (134 lines), `StatusBar.cpp` (406 lines)

### Segments

| Segment | Content | Example |
|---------|---------|---------|
| File info | Filename, dirty indicator | `foo.cpp •` |
| Position | Line, Column | `Ln 42, Col 8` |
| Encoding | Current encoding | `UTF-8` |
| EOL type | Current EOL | `Windows (CRLF)` |
| Language | Current language | `C++` |
| Selection | "Sel: N chars" when selection active | `Sel: 128 chars` |
| Zoom | Current zoom level | `100%` |

### Updating Segments

```cpp
statusBar->updatePosition(line, col);
statusBar->updateEncoding(UTF_8);
statusBar->updateEol(EOL_CRLF);
statusBar->updateLanguage(L_CPP);
statusBar->setFileName("foo.cpp", false);
```

---

## MenuBar (`src/ui/MenuBar`)

The main menu bar. Built lazily — each submenu is created on first access.

**Files:** `MenuBar.h` (108 lines), `MenuBar.cpp` (427 lines)

### Menu Structure

```
File
  New, Open..., Open Recent →, Close, Close All,
  ─────────────
  Save, Save As..., Save All,
  ─────────────
  Reload from Disk, Save Copy As...,
  ─────────────
  Print...,
  ─────────────
  Exit

Edit
  Undo, Redo,
  ─────────────
  Cut, Copy, Paste, Select All,
  ─────────────
  Delete, Duplicate Line, Join Lines,
  ─────────────
  Indent, Unindent,
  ─────────────
  Upper Case, Lower Case, Title Case, Sentence Case, iNVERT cASE, rAnDoM cAsE,
  ─────────────
  Column Editor...,
  ─────────────
  Insert..., Date/Time...,
  ─────────────
  Find/Replace →, Find Next, Find Previous,
  ─────────────
  Go to →, Command Palette...

Search
  Find..., Replace...,
  ─────────────
  Find Next, Find Previous, Find Selection Next,
  ─────────────
  Incremental Search, Find in Files...,
  ─────────────
  Go to Line...
  ─────────────
  Mark..., Clear All Marks,
  ─────────────
  Find Matching Brace, Select to Matching Brace

View
  Toggle Fullscreen,
  ─────────────
  Zoom In, Zoom Out, Zoom Reset (100%),
  ─────────────
  Word Wrap, Line Number, Symbol, White Space, Smart Highlighting
  ─────────────
  Document Map, Function List, File Browser, Clipboard History, Ansi Characters
  ─────────────
  Panels →, Tab Bar →

Encoding
  Convert to UTF-8, Convert to UTF-8 with BOM, Convert to... →
  ─────────────
  Character Sets → (Western European, Eastern European, etc.)

Language
  (dynamically populated from LexerConfig)

Settings
  Preferences..., Shortcut Mapper..., Style Configurator...,
  ─────────────
  Import... →, Export... →

Tools
  Run... (F5)

Window
  New Window,
  ─────────────
  Move/Tab..., Clone to Other View,
  ─────────────
  Toggle, Toggle, Toggle (splitter views),
  ─────────────
  Activate Tab 1–9

Plugins
  (dynamically populated from PluginManager)
```

### Keyboard Navigation

Menu bar can be navigated with `Alt+<letter>` (underlined letter) and arrow keys.

### Adding a Menu Item

```cpp
// In MenuBar::create<Menu>Menu()
auto action = new QAction("My Command", this);
action->setShortcut(QKeySequence("Ctrl+Shift+M"));
_mainWindow->connect(action, &QAction::triggered,
                    _mainWindow, &MainWindow::onMyCommand);
_menu->addAction(action);
```

---

## ColourPopup (`src/ui/ColourPopup`)

Color picker popup for the toolbar and UDL editor.

**File:** `ColourPopup.cpp` (245 lines)

### Layout

```
┌─────────────────────────────┐
│  Grid of preset colors      │
│  (24 basic colors)          │
├─────────────────────────────┤
│  Custom color: [________]   │
│  [Pick from screen]        │
├─────────────────────────────┤
│  Recent colors              │
└─────────────────────────────┘
```

---

## ColourPicker (`src/ui/ColourPicker`)

Full color selection dialog.

**File:** `ColourPicker.cpp` (121 lines)

Provides a full HSV color wheel + saturation/value square + hue slider + alpha slider.

---

## WinControls (`src/ui/WinControls`)

Stub module for Win32→Qt6 port compatibility. Contains Qt wrappers for MFC Win32 controls.

**Files:** `WinControls.h`, `WinControls.cpp` (150 lines)

Currently a compatibility shim — maps Win32 window styles to Qt equivalents. Not all Win32 control types are wrapped.

---

## Docking System

Notepad--Qt uses Qt's built-in `QDockWidget` system for panels.

### Panel Registration

```cpp
// In Application::setupDockingPanels()
_fileBrowserPanel = new FileBrowserPanel(_mainWindow);
_fileBrowserDock = new QDockWidget(tr("File Browser"), _mainWindow);
_fileBrowserDock->setWidget(_fileBrowserPanel);
_mainWindow->addDockWidget(Qt::RightDockWidgetArea, _fileBrowserDock);
```

### Panel Toggling

Panels are toggled via their `toggleViewAction()`:

```cpp
_viewMenu->addAction(_fileBrowserDock->toggleViewAction());
```

`toggleViewAction()` is provided by `QDockWidget` automatically — it creates a menu item with a checkbox that reflects the panel's visibility.

### Splitter Layout

Panels are organized in a `QSplitter`:

```
MainWindow
  ├── MenuBar
  ├── ToolBar
  ├── DocTabView  ←  (left, main area)
  │     └── [ScintillaEditor tabs]
  ├── QSplitter (horizontal)
  │     ├── DocTabView (left, ~70%)
  │     └── RightPanelStack (right, ~30%)
  │           ├── FileBrowserDock
  │           ├── FunctionListDock
  │           ├── DocMapDock
  │           └── ClipboardHistoryDock
  └── StatusBar
```

### Tabified Panels

When multiple panels are docked to the same area, they appear as tabs:

```
┌──────────────────────────────────────────────────────────────┐
│ [DocMap] [Function List] [Clipboard History]     [×] │
```

Click the panel's tab to switch. Drag a panel's title bar to a different area to re-dock.

---

## Keyboard Shortcut Resolution

Keyboard shortcuts are resolved in this order of precedence:

1. **User shortcuts** — `~/.config/notepad--qt/shortcuts.json` (highest priority)
2. **Hardcoded shortcuts** — `ShortcutManager::registerDefaultShortcuts()`
3. **QKeySequence** on `QAction` objects (fallback)

When a key is pressed:
1. `MainWindow::event()` receives the key event
2. `ShortcutManager` checks if it matches any registered shortcut
3. If matched, the corresponding action/command is dispatched
4. If not matched, the event is passed to the focused widget

### DE Shortcut Conflicts

See [KEYBOARD_SHORTCUTS.md](KEYBOARD_SHORTCUTS.md#desktop-environment-conflicts).
