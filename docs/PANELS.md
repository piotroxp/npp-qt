# Panels Reference

How to use and configure each dockable panel in Notepad--Qt.

---

## Overview

Panels are dockable widgets that live in `QDockWidget` containers around the main editor. All panels are created lazily — they are instantiated on app startup but their content is populated only on first activation — so they have no performance cost until you use them.

Toggle panels from the **View → Panels** submenu or right-click the toolbar.

---

## Document Map

Shows a read-only minimap of the current file for quick navigation.

**How to open:** `View → Panels → Document Map` or `Ctrl+Shift+D`

### Features
- **Scroll sync** — Scrolls in sync with the main editor at 60 fps
- **Click to navigate** — Click anywhere in the minimap to jump to that line
- **Viewport rectangle** — A semi-transparent overlay shows the visible region
- **Syntax highlighting** — The minimap shows the same syntax highlighting as the editor
- **Dim inactive** — When another tab is active, the minimap dims

### Configuration

No configuration options currently. The minimap width is derived from the panel width.

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Click` | Jump to line |
| `Drag` | Scroll through document |

---

## Function List

Shows a collapsible tree of functions, classes, and other symbols in the current file, parsed from the source code using language-specific regex patterns.

**How to open:** `View → Panels → Function List`

### Supported Languages

The Function List uses regex parsers from Notepad++ upstream XML definitions. Supported languages include:

| Language | What it parses |
|----------|----------------|
| C / C++ | Functions, classes, structs, enums, namespaces, macros |
| Python | Classes, functions, async functions, decorators |
| JavaScript | Functions, classes, arrow functions, methods |
| Rust | Functions, structs, impl blocks, enums, traits |
| Go | Functions, methods, types, interfaces |
| PHP | Functions, classes, methods, traits |
| Java | Classes, methods, interfaces |
| Ruby | Classes, modules, methods, singleton methods |
| Lua | Functions |
| JSON | Keys (top-level) |
| XML | Tags (with attributes) |
| HTML | Tags, IDs, classes (when JavaScript plugin is available) |

### Features
- **Double-click** — Jump to the symbol definition
- **Collapse/expand** — Click `▸` to expand/collapse nodes
- **Filter** — Type in the panel search box to filter the tree
- **Language auto-detection** — Automatically re-parses when you switch tabs

### How Parsing Works

1. `FunctionListXmlParser` loads NPP's `functionListparsers.xml` from the config directory
2. `FunctionListXmlParser::parse(content, langType)` applies the language-specific regex
3. Results are displayed in a `QTreeWidget`
4. Double-click fires `FunctionListPanel::onItemDoubleClicked(item)` → `MainWindow::gotoLine(line)`

### Limitations
- Parsing is debounced (200 ms) to avoid re-parsing on every keystroke
- Large files (>1 MB) skip function list parsing
- User-defined language (UDL) parsers are not yet supported

---

## File Browser

A tree view of the filesystem, rooted at a chosen directory.

**How to open:** `View → Panels → File Browser`

### Features
- **Directory tree** — Expand/collapse folders
- **File watchers** — `QFileSystemWatcher` monitors the root directory for changes
- **Double-click to open** — Opens the file in the current editor tab
- **Context menu** — Right-click for New File, New Folder, Rename, Delete, Refresh
- **Hidden files** — Toggle with `View → Show Hidden Files`
- **Filter** — Type to filter files by name (e.g., `*.cpp`)

### Refresh
Press `F5` or right-click → **Refresh** to force a tree refresh.

External file creation/deletion is detected automatically via `QFileSystemWatcher`. If the tree doesn't update:
1. Press `F5` to manually refresh
2. Check if the file was created inside the watched directory

### Root Directory
By default, the File Browser roots at the directory of the current file. You can change the root:
- **Right-click → Set as Root** — Set the current folder as the root
- **Right-click → Open as New Root** — Browse and pick a folder
- **Right-click → Go to Parent** — Go up one directory level

---

## Clipboard History

Maintains a ring of copied text entries for pasting back.

**How to open:** `View → Panels → Clipboard History`

### Features
- **Entry ring** — Stores the last N clipboard entries (configurable in Preferences)
- **Click to paste** — Click an entry to paste it at the cursor
- **Double-click** — Same as click (paste immediately)
- **Clear** — Right-click → **Clear History**
- **Pin** — Right-click → **Pin** to keep an entry at the top of the list
- **Search** — Type in the search box to filter entries
- **Persistent** — History is saved across sessions (stored in config)

### How it Works

1. `ClipboardHistoryPanel` connects to `QApplication::clipboard()` changed signals
2. On each clipboard change, the new text is prepended to `_history` (up to `MAX_CLIPBOARD_HISTORY` entries)
3. Duplicate consecutive copies are collapsed
4. The panel displays `_history` as a `QListWidget`
5. Click fires `Application::onClipboardHistoryPaste(index)` → `ScintillaEditor::insertText()`

### Configuration
Max history size is set in `Settings → Preferences → Editing → Clipboard History Size`.

---

## Adding a New Panel

1. Create the class in `src/panels/YourPanel.h` and `src/panels/YourPanel.cpp`
2. Inherit from `QDockWidget`:
   ```cpp
   class YourPanel : public QDockWidget {
       Q_OBJECT
   public:
       explicit YourPanel(MainWindow* parent);
   signals:
       void navigateTo(int line, int column);
   };
   ```
3. Create the panel in `Application::setupDockingPanels()`:
   ```cpp
   _yourPanel = new YourPanel(_mainWindow);
   _mainWindow->addDockWidget(Qt::RightDockWidgetArea, _yourPanel);
   ```
4. Connect signals in `Application::setupConnections()`:
   ```cpp
   connect(_yourPanel, &YourPanel::navigateTo,
           _mainWindow, &MainWindow::gotoLine);
   ```
5. Wire the toggle action in `MenuBar.cpp` or `MainWindow::createViewMenu()`:
   ```cpp
   _viewMenu->addAction(_yourPanel->toggleViewAction());
   ```
6. Add to the session state so panel visibility is restored on restart (in `SessionManager`)
