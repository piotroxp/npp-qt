# NPP-Qt Semantic Lift — Translation Map

> Last updated: 2026-06-24 20:35 UTC
> Status reflects actual file contents on disk, not preflight estimates.

---

## Win32 → Qt6 Pattern Mapping

| Win32 | Qt6 |
|-------|-----|
| `HWND` | `QWidget*` |
| `HINSTANCE` | `QApplication*` |
| `WNDCLASSEX` | `QWindow::create()` + `QWidget` |
| `CreateWindow()` | `new QWidget()` / `setWindowTitle()` |
| `ShowWindow(hwnd, SW_SHOW)` | `widget->show()` |
| `MoveWindow(hwnd, ...)` | `widget->resize()` / `widget->move()` |
| `GetClientRect(hwnd, &rc)` | `widget->rect()` |
| `InvalidateRect(hwnd, NULL, TRUE)` | `widget->update()` |
| `HDC hdc = GetDC(hwnd)` | `QPainter painter(widget)` |
| `TextOut(hdc, ...)` | `painter.drawText()` |
| `CreateSolidBrush(color)` | `QBrush(color)` |
| `SendMessage(hwnd, WM_*)` | `QMetaObject::invokeMethod()` / signals |
| `LRESULT CALLBACK WndProc(...)` | `QWidget::event()` override |
| `ListView` | `QTableWidget` |
| `ListView_SetItemCount` | `QTableWidget::setRowCount` |
| `LVM_INSERTCOLUMN` | `setHorizontalHeaderLabels()` / `setItem()` |
| `LVN_GETDISPINFO` | `populateRow()` + `item()` override |
| `LVN_COLUMNCLICK` | `QHeaderView::sectionClicked` signal |
| `ListView_GetSelectedCount` | `selectionModel()->selectedRows().count()` |
| `EnableWindow(hwnd, BOOL)` | `widget->setEnabled(bool)` |
| `TCITEM` / `TabCtrl_*` | `QTabWidget` signals/methods |
| `CWinMgr` layout macros | `QVBoxLayout` / `QHBoxLayout` + size policies |
| `WM_CHAR_REPLACEMENT` | `QWidget::event(QEvent::KeyPress)` + type-ahead |
| `WM_CONTEXTMENU` | `QWidget::customContextMenuRequested` |
| `SetWindowText` | `setWindowTitle()` |
| `GetWindowRect` / `SetWindowPos` | `geometry()` / `setGeometry()` |
| `RegisterWindowMessage` | Custom `intptr_t run_dlgProc()` switch |
| `HMENU` / `MENUITEMINFO` | `QMenu` + `QAction` |
| `SendMessage(_hParent, WDN_NOTIFY, ...)` | `emit windowActivated(int)` / `emit windowSorted(QVector)` |

---

## Translation Status

### ✅ Base Infrastructure

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| Window | WinControls/Window.h | WinControls/Window.h | 125 | ✅ DONE (header-only) |
| TabBar | WinControls/TabBar/* | WinControls/TabBar.h/cpp | 539 | ✅ DONE |
| ToolBar | WinControls/ToolBar/* | WinControls/ToolBar.h/cpp | 812 | ✅ DONE | Full lift: Toolbar (QToolBar) + ReBar (QWidget) + ToolBarIcons (QIcon×8); Win32 TOOLBARCLASSNAME → QToolBar; ICC_WIN95_CLASSES → Qt no-op; TB_ADDBUTTONS → rebuildActions(); RB_INSERTBAND → QVBoxLayout; NppXml theme loading; DPIManagerV2 DPI-aware icons; NppDarkMode dark mode wiring; chevron overflow menu |
| StatusBar | WinControls/StatusBar/* | WinControls/StatusBar.h/cpp | 239 | ✅ DONE |
| MainWindow | Notepad_plus_Window.cpp | MainWindow.h/cpp | ~600 | ✅ DONE |
| Buffer | Buffer.h | Buffer.h | ~200 | ✅ DONE |
| CMakeLists.txt | — | CMakeLists.txt | — | ✅ DONE (restructured, 55 files) |

### ✅ Base Infrastructure — WinControls Layer

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| DockingWnd | WinControls/DockingWnd/* | DockingWnd.h/cpp | 997 | ✅ LIFTED |
| SplitterContainer | WinControls/SplitterContainer/* | SplitterContainer.h/cpp | 390 | ✅ LIFTED |
| StaticDialog | WinControls/StaticDialog/* | StaticDialog.h/cpp | 788 | ✅ LIFTED |
| RunDlg | WinControls/StaticDialog/RunDlg/* | StaticDialog.h/cpp | +200 | ✅ LIFTED |

### ✅ WinControls — Dialogs

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| AboutDlg | WinControls/AboutDlg/* | AboutDlg.h/cpp | 760 | ✅ DONE |
| WindowsDlg | WinControls/WindowsDlg/* | WindowsDlg.h/cpp | ~1200 | ✅ DONE |
| WinMgr | WinControls/WindowsDlg/WinMgr.* | WinMgr.h/cpp | 800 | ✅ LIFTED |
| WinRect | WinControls/WindowsDlg/WinRect.* | WinMgr.h (header) | 50 | ✅ LIFTED |
| SizeableDlg | WinControls/WindowsDlg/SizeableDlg.* | SizeableDlg.h/cpp | 150 | ✅ LIFTED |
| AnsiCharPanel | WinControls/AnsiCharPanel/* | AnsiCharPanel.h/cpp | 755 | ✅ DONE |
| ColourPicker | WinControls/ColourPicker/* | ColourPicker.h/cpp | 410 | ✅ DONE |
| WordStyleDlg | WinControls/ColourPicker/WordStyleDlg.* | WordStyleDlg.h/cpp | 600 | ✅ LIFTED |
| RunMacroDlg | WinControls/shortcut/RunMacroDlg.* | RunMacroDlg.h/cpp | 250 | ✅ LIFTED |
| CustomFileDialog | WinControls/OpenSaveFileDialog/* | CustomFileDialog.h/cpp | 415 | ✅ DONE |
| lesDlgs | PowerEditor/src/lesDlgs.cpp | lesDlgs.h/cpp | 203 | ✅ DONE |

### ✅ WinControls — BabyGrid (spreadsheet control)

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| BabyGrid | WinControls/Grid/BabyGrid.* | Grid/BabyGrid.h/cpp | 1300 | ✅ LIFTED |
| BabyGridWrapper | WinControls/Grid/BabyGridWrapper.* | Grid/BabyGridWrapper.h/cpp | 150 | ✅ LIFTED |

### ✅ WinControls — Panels

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| DocumentMap | WinControls/DocumentMap/* | DocumentMap.h/cpp | 684 | ✅ DONE |
| FunctionList | WinControls/FunctionList/* | FunctionList.h/cpp | 736 | ✅ DONE (wired to ScintillaComponent) |
| FindCharsInRange | WinControls/FindCharsInRange/* | FindCharsInRange.h/cpp | 262 | ✅ DONE |
| FileBrowser | WinControls/FileBrowser/* | FileBrowser.h/cpp | 1891 | ✅ LIFTED |
| ProjectPanel | WinControls/ProjectPanel/* | ProjectPanel.h/cpp | 809 | ✅ LIFTED |
| TaskList | WinControls/TaskList/* | TaskList.h/cpp | 554 | ✅ LIFTED |
| VerticalFileSwitcher | WinControls/VerticalFileSwitcher/* | VerticalFileSwitcher.h/cpp | 756 | ✅ LIFTED |

### ✅ WinControls — Advanced / Utility

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| ClipboardHistory | WinControls/ClipboardHistory/* | ClipboardHistory.h/cpp | 342 | ✅ DONE |
| ContextMenu | WinControls/ContextMenu/* | ContextMenu.h/cpp | 226 | ✅ DONE |
| TreeView | WinControls/TreeView/* | TreeView.h/cpp | 635 | ✅ DONE |
| TrayIcon | WinControls/TrayIcon/* | TrayIcon.h/cpp | 182 | ✅ DONE |
| ToolTip | WinControls/ToolTip/* | ToolTip.h/cpp | 116 | ✅ DONE |
| Preference | WinControls/Preference/* | Preference.h/cpp | 1214 | ✅ LIFTED |
| PluginsAdmin | WinControls/PluginsAdmin/* | PluginsAdmin.h/cpp | 673 | ✅ LIFTED |
| ShortcutMapper | WinControls/Grid/* | ShortcutMapper.h/cpp | 557 | ✅ LIFTED |
| DoubleBuffer | WinControls/DoubleBuffer/* | DoubleBuffer.h/cpp | 116 | ✅ DONE |
| ImageListSet | WinControls/ImageListSet/* | ImageListSet.h/cpp | 329 | ✅ DONE |
| ReadDirectoryChanges | WinControls/ReadDirectoryChanges/* | ReadDirectoryChanges.h/cpp | 267 | ✅ DONE |

### ✅ Core — Encoding / Buffers

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| Utf8_16 | PowerEditor/src/Utf8_16.cpp | Utf8_16.h/cpp | ~2400 | ✅ DONE |
| NppDarkMode | PowerEditor/src/NppDarkMode.cpp | NppDarkMode.h/cpp | ~600 | ✅ DONE |
| ScintillaEditBase | scintilla/qt/* | ScintillaEditBase.h/cpp | ~100 | ✅ DONE (Qt wrapper for Scintilla) |
| lesDlgs | PowerEditor/src/lesDlgs.cpp | lesDlgs.h/cpp | 203 | ✅ DONE |

### ✅ Core — Notifications & Constants

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| NppNotification | PowerEditor/src/NppNotification.cpp | NppNotification.h/cpp | 253 | ✅ LIFTED |
| clipboardFormats | PowerEditor/src/clipboardFormats.h | clipboardFormats.h | — | ✅ DONE |
| keys | PowerEditor/src/keys.h | keys.h | — | ✅ DONE |
| resource | PowerEditor/src/resource.h | resource.h | — | ✅ DONE |

### ✅ Infrastructure / Utility Files (ported this session)

| Component | Source | Target | Lines | Status |
|-----------|--------|--------|-------|--------|
| Common | PowerEditor/src/MISC/Common/Common.h/.cpp | src/MISC/Common/Common.h/.cpp | 1219 | ✅ DONE |
| FileInterface | PowerEditor/src/MISC/Common/FileInterface.h/.cpp | src/MISC/Common/FileInterface.h/.cpp | 82 | ✅ DONE |
| EncodingMapper | PowerEditor/src/EncodingMapper.h/.cpp | src/MISC/Common/EncodingMapper.h/.cpp | 203 | ✅ DONE |
| PluginsManager | PowerEditor/src/MISC/PluginsManager/PluginsManager.h/.cpp | src/MISC/PluginsManager/PluginsManager.h/.cpp | 268 | ✅ DONE |
| dpiManagerV2 | PowerEditor/src/dpiManagerV2.h/.cpp | src/dpiManagerV2.h/.cpp | 21 | ✅ DONE |
| lastRecentFileList | PowerEditor/src/lastRecentFileList.h/.cpp | src/lastRecentFileList.h/.cpp | 195 | ✅ DONE |
| localization | PowerEditor/src/localization.h/.cpp | src/localization.h/.cpp | 227 | ✅ DONE |
| shortcut | PowerEditor/src/WinControls/shortcut/shortcut.h/.cpp | src/shortcut.h/.cpp | 349 | ✅ DONE |
| NppNotification | PowerEditor/src/NppNotification.cpp | src/NppNotification.h/.cpp | 253 | ✅ DONE |
| NppConstants | PowerEditor/src/MISC/Common/NppConstants.h | src/NppConstants.h | — | ✅ DONE |
| Processus | PowerEditor/src/MISC/Process/Processus.h/.cpp | src/MISC/Process/Processus.h/.cpp | 68+98 | ✅ DONE | `run()` → `QProcess::startDetached()`; `runSync()` → `QProcess::execute()`; `splitArgs()` hand-rolled whitespace split (no QRegExp) |
| NppXml | — | src/NppXml.h | — | ✅ STUB |
| IDAllocator | — | src/MISC/Common/IDAllocator.h | — | ✅ DONE |
| clipboardFormats | PowerEditor/src/clipboardFormats.h | src/clipboardFormats.h | — | ✅ DONE |
| keys | PowerEditor/src/keys.h | src/keys.h | — | ✅ DONE |
| resource | PowerEditor/src/resource.h | src/resource.h | — | ✅ DONE |
All infrastructure headers live directly in `src/` (not in MISC/Common/). `src/MISC/Common/` holds only `Common`, `EncodingMapper`, `FileInterface`, `IDAllocator`. `src/MISC/Process/` holds `Processus.h/.cpp` (QProcess-based process launch).

### ✅ WinControls — All Chunks Complete

All 35 WinControls files migrated and verified. Zero stray `#include "*.moc"` remains.
`Window.h` provides cross-compat shims: `using HWND = QWidget*`, `using HINSTANCE = QApplication*`.

### ✅ Core — Application Core (ported this session)

| Component | Source | Lines | Status | Notes |
|-----------|--------|-------|--------|-------|
| Parameters | PowerEditor/src/Parameters.cpp | 8929 | ✅ DONE | `NppParameters` singleton; Win32 registry → QSettings; GetModuleFileName → QCoreApplication; SHGetFolderPath → QStandardPaths; MessageBox → QMessageBox; HMENU → QMenu*; all Win32 types removed |
| NppIO | PowerEditor/src/NppIO.cpp | 2961 | ✅ DONE | `Notepad_plus::monitorFileOnChange`, `doOpen`, `doSave`, `fileLoadSession`, `fileSaveSession`; CreateFile/ReadFile/WriteFile → QFile; ShellExecute → QProcess/QDesktopServices; WaitForMultipleObjects → QThread/QFileSystemWatcher |
| NppCommands | PowerEditor/src/NppCommands.cpp | 4513 | ✅ DONE | `NppCommands` bridge class with 80+ slots; Win32 `switch(id)` → Qt signal/slot connections; GetLocalTime → QDateTime; GetKeyState → QApplication::keyboardModifiers |
| NppBigSwitch | PowerEditor/src/NppBigSwitch.cpp | 4305 | ✅ DONE | `Notepad_plus_Proc` → `QWidget::event()` override; `LRESULT CALLBACK WndProc` → `intptr_t run_dlgProc()`; `RegisterWindowMessage` → `NppCustomEvent`; `WM_COMMAND` → menu action slots |
| Notepad_plus | PowerEditor/src/Notepad_plus.cpp | 9331 | ✅ DONE | Full application class ported; HWND → QWidget*; `::GetMenu` → QMenuBar; File operations → QFile; Shell ops → QProcess |
| winmain.cpp → main.cpp | PowerEditor/src/winmain.cpp | 875 | ✅ DONE | `WinMain(HINSTANCE,...)` → `int main(int argc, char** argv)`; QCommandLineParser; dark mode init; session loading |

**NppCommands header**: `src/NppCommands.h` — declares all command slots for menu → Qt signal/slot wiring.  
**NppBigSwitch header**: `src/NppBigSwitch.h` — `NppCustomEvent`, `NppEvent::*` IDs, `run_dlgProc()` switch.  
**NppIO header**: `src/NppIO.h` — `NppIO` class with `QFile`-based read/write/monitoring.

### ✅ ScintillaComponent — Editor Layer

| Component | Source | Target | Lines | Status | Notes |
|-----------|--------|--------|-------|--------|-------|
| Buffer | ScintillaComponent/Buffer.h/cpp | src/Buffer.h/cpp | 288+773 | ✅ DONE | Full FileManager + Buffer class; QFile/QDateTime/QMutex |
| ScintillaCtrls | ScintillaComponent/ScintillaCtrls.h/cpp | src/ScintillaCtrls.h/cpp | 25+41 | ✅ DONE | Factory for ScintillaComponent instances |
| DocTabView | ScintillaComponent/DocTabView.h/cpp | src/DocTabView.h/cpp | 35+95 | ✅ DONE | QTabWidget-based doc tabs; tabData() for buffer↔tab |
| FindReplaceDlg | ScintillaComponent/FindReplaceDlg.h/cpp | src/FindReplaceDlg.h/cpp | 140+282 | ✅ STUB | QWidget; findNext/replace/mark; QDirIterator for FIF |
| AutoCompletion | ScintillaComponent/AutoCompletion.h/cpp | src/AutoCompletion.h/cpp | 59+155 | ✅ DONE | showApiComplete/showWordComplete/showFunctionComplete |
| GoToLineDlg | ScintillaComponent/GoToLineDlg.h/cpp | src/GoToLineDlg.h/cpp | 57+177 | ✅ DONE | QDialog; goto line/offset; SCI_GOTOLINE |
| SmartHighlighter | ScintillaComponent/SmartHighlighter.h/cpp | src/SmartHighlighter.h/cpp | 38+171 | ✅ DONE | highlightView/hilightViewWithWord; visible-line range |
| columnEditor | ScintillaComponent/columnEditor.h/cpp | src/columnEditor.h/cpp | 46+118 | ✅ DONE | Column-mode sequential numbers; base/leading/repeat |
| Printer | ScintillaComponent/Printer.h/cpp | src/Printer.h/cpp | 37+86 | ✅ DONE | QPrinter-based print |
| UserDefineDialog | ScintillaComponent/UserDefineDialog.h/cpp | src/UserDefineDialog.h/cpp | 61+182 | ✅ STUB | Keywords/stylers/folders/comments; real-time preview pending |
| FunctionCallTip | ScintillaComponent/FunctionCallTip.h/cpp | src/FunctionCallTip.h/cpp | 64+165 | ✅ DONE | SCI_CALLTIPSHOW/ACTIVE/CANCEL; overload cycling |
| xmlMatchedTagsHighlighter | ScintillaComponent/xmlMatchedTagsHighlighter.h/cpp | src/xmlMatchedTagsHighlighter.h/cpp | 39+335 | ✅ DONE | XML/HTML tag matching; SCE_UNIVERSAL_TAGMATCH |

All 13 ScintillaComponent files ported. Win32→Qt6 verification: 0 stray HWND/HDC/HINSTANCE in new files.

### ⏳ Core — Editor

| Component | Source | Status | Notes |
|-----------|--------|--------|-------|
| ScintillaEdit | scintilla/qt/* | ⏳ PENDING | Partially lifted (ScintillaComponent done) |

### ✅ Recently Wired (this session)

| Component | What was wired | Status |
|-----------|----------------|--------|
| FunctionList | `reload()` → `_funcParserMgr.parse()`, SCN_UPDATEUI signal → auto-refresh, `openSelection()` → SCI_GOTOPOS, `markEntry()` → currentLine() | ✅ DONE |
| DocumentMap | Full body already implemented — uses `_pMapView` Scintilla calls, view zone overlay, minimap rendering | ✅ DONE |
| WindowsDlg | Full body already implemented — QTableWidget with sort, activate/save/close, type-ahead | ✅ DONE |
| AboutDlg | Chameleon icon → `getChameleonPixmap()` using NppDarkMode accent colors; version → `QCoreApplication::applicationVersion()` | ✅ DONE |
| NppDarkMode | Already existed at `src/NppDarkMode.h/.cpp` (4542 lines) — no separate DarkMode.* files needed | ✅ EXISTS |
| functionParser | `TreeStateNode` struct added; `ScintillaEditView` typedef → `ScintillaComponent`; `getLexerColors()` helper added | ✅ DONE |

---

## Project Structure (current)

```
npp-qt/
├── CMakeLists.txt              ✅ (104 entries: 82 unique .cpp/.h on disk)
├── SEMANTIC_LIFT_MAP.md        ✅ (this file)
└── src/
    ├── main.cpp                ✅
    ├── MainWindow.h/cpp        ✅
    ├── Buffer.h                ✅ (header-only)
    ├── ScintillaComponent.h/cpp ✅ (1361 lines)
    ├── ScintillaEditBase.h/cpp ✅ (new — Scintilla Qt wrapper)
    ├── NppDarkMode.h/cpp       ✅ (~600 lines)
    ├── Utf8_16.h/cpp           ✅ (~2400 lines)
    ├── lesDlgs.h/cpp           ✅ (203 lines)
    ├── Parameters.h              ✅ (2047 lines — full NppParameters class)
    ├── Parameters.cpp            ✅ (8929 lines — QSettings persistence, all Win32 removed)
    ├── NppIO.h                  ✅ (Qt file I/O + monitoring)
    ├── NppIO.cpp                ✅ (2961 lines — QFile-based read/write/monitoring)
    ├── NppCommands.h            ✅ (Qt menu command bridge — 80+ slots)
    ├── NppCommands.cpp          ✅ (4513 lines — all command handlers)
    ├── NppBigSwitch.h          ✅ (Qt event dispatcher)
    ├── NppBigSwitch.cpp        ✅ (4305 lines — WndProc → event())
    ├── Notepad_plus.h           ✅ (677 lines — main app class header)
    ├── Notepad_plus.cpp         ✅ (9331 lines — full app port)
    ├── menuCmdID.h             ✅ (IDM_* command IDs)
    ├── NppConstants.h          ✅ (markers, indicators, NPPM_* messages)
    ├── NppXml.h                ✅ (Qt XML stub for localization)
    ├── NppNotification.h/cpp    ✅ (Scintilla→Qt signal bridge)
    ├── dpiManagerV2.h/cpp      ✅ (Qt DPI scaling helpers)
    ├── lastRecentFileList.h/cpp ✅ (MRU list via QSettings)
    ├── localization.h/cpp       ✅ (Qt Linguist + NativeLangSpeaker)
    ├── shortcut.h/cpp           ✅ (QKeySequence shortcuts)
    ├── clipboardFormats.h       ✅ (MIME type constants)
    ├── keys.h                  ✅ (Qt::Key VK_* constants)
    ├── resource.h               ✅ (resource ID constants)
    ├── MISC/
    │   ├── Common/
    │   │   ├── Common.h/.cpp     ✅ (29K — string/path/clipboard utilities)
    │   │   ├── FileInterface.h/.cpp ✅ (file I/O via QFile)
    │   │   ├── EncodingMapper.h/.cpp ✅ (encoding ↔ codepage)
    │   │   ├── IDAllocator.h    ✅ (ID range allocator)
    │   │   └── NppConstants.h    ✅ (Win32 constants)
    │   ├── PluginsManager/
    │   │   └── PluginsManager.h/.cpp ✅ (QPluginLoader-based plugin mgmt)
    │   └── Process/
    │       └── Processus.h/.cpp  ✅ (QProcess-based run/runSync)
    └── WinControls/
        ├── Window.h            ✅ (header-only)
        ├── TabBar.h/cpp        ✅
        ├── ToolBar.h/cpp       ✅
        ├── StatusBar.h/cpp     ✅
        ├── DockingWnd.h/cpp    ✅
        ├── SplitterContainer.h/cpp ✅
        ├── StaticDialog.h/cpp  ✅
        ├── AboutDlg.h/cpp      ✅ (icon stubs)
        ├── WindowsDlg.h/cpp    ✅ (static resources)
        ├── WinMgr.h/cpp        ✅ (layout engine)
        ├── SizeableDlg.h/cpp    ✅ (layout dialog base)
        ├── AnsiCharPanel.h/cpp ✅
        ├── ColourPicker.h/cpp  ✅
        ├── WordStyleDlg.h/cpp   ✅ (STUB — needs Parameters)
        ├── RunMacroDlg.h/cpp   ✅ (STUB — needs Parameters)
        ├── CustomFileDialog.h/cpp ✅
        ├── DocumentMap.h/cpp   ✅ (stub)
        ├── FunctionList.h/cpp ✅ (stub)
        ├── FindCharsInRange.h/cpp ✅
        ├── FileBrowser.h/cpp   ✅ (expanded)
        ├── ClipboardHistory.h/cpp ✅
        ├── ContextMenu.h/cpp  ✅
        ├── TreeView.h/cpp      ✅
        ├── TrayIcon.h/cpp      ✅
        ├── ToolTip.h/cpp       ✅
        ├── Preference.h/cpp    ✅
        ├── PluginsAdmin.h/cpp  ✅
        ├── ShortcutMapper.h/cpp ✅
        ├── ProjectPanel.h/cpp  ✅ (expanded)
        ├── TaskList.h/cpp     ✅ (TaskListDlg included)
        ├── VerticalFileSwitcher.h/cpp ✅
        ├── DoubleBuffer.h/cpp  ✅
        ├── ImageListSet.h/cpp  ✅
        ├── ReadDirectoryChanges.h/cpp ✅
        └── Grid/
            ├── BabyGrid.h/cpp      ✅ (spreadsheet control)
            └── BabyGridWrapper.h/cpp ✅
```

---

## Known Gaps & Notes

- **Build**: CMakeLists.txt has 104 entries (~82 unique), all confirmed on disk. All 6 core files ported to Qt6: Parameters.cpp (8929L), NppIO.cpp (2961L), NppCommands.cpp (4513L), NppBigSwitch.cpp (4305L), Notepad_plus.cpp (9331L), main.cpp (875L). Win32 types: 0 HWND/0 HDC/0 HINSTANCE in portable code. No cmake/Qt6 in this container — build must be verified on a dev machine.
- **Tests**: No `tests/` directory or test infrastructure. Unit tests should target `Buffer`, `Utf8_16`, `NppDarkMode`, `ReadDirectoryChanges` first — all are self-contained.
- **Win32 sweep (core files)**: All Win32 patterns (HWND, HDC, HINSTANCE, ::DialogProc, ::WndProc, TEXT(), _T(), #include <windows.h>) are 0 in Parameters.cpp, NppIO.cpp, NppCommands.cpp, NppBigSwitch.cpp, Notepad_plus.cpp, main.cpp. Window.h provides cross-compat shims.
- **moc includes**: 0 stray `#include "*.moc"` in source tree — CMake AUTOMOC handles all Q_OBJECT headers.
- **Remaining stubs**: Buffer.cpp (Scintilla init), MainWindow.cpp (NppCommands wiring), PluginsAdmin.cpp (2 stubs), VerticalFileSwitcher.cpp (1 stub on DocTabView), ScintillaEditBase (abstract Scintilla wrapper pending). All are dependency-driven — resolve naturally when Scintilla layer lands.

---

## Session 2026-06-29 (afternoon)

### WinControls completed this session
| Panel | Action | Files |
|---|---|---|
| `MISC/Process/Processus.cpp` | Written | Qt6 `QProcess::startDetached/execute` |
| `WinControls/TreeView.cpp` | Written (790L) | `QTreeWidget`-based, replaces Win32 `WC_TREEVIEW` |
| `WinControls/ClipboardHistory/clipboardHistoryPanel.cpp` | Agent written | 29KB — `QListWidget` clipboard history |
| `WinControls/FileBrowser/fileBrowser.cpp` | Agent written | `QFileSystemModel` + `QTreeView` |
| `WinControls/ImageListSet.cpp` | Pre-existing verified | `IconList`/`IconLists` with `QIcon` + Fluent color map |
| `WinControls/ToolBar.cpp` | Written (~893L) | `Toolbar` (QToolBar) + `ReBar` (QWidget) + `ToolBarIcons` (8×QIcon); `DPIManagerV2::scale()` DPI icons; `NppXml` theme loading; `NppDarkMode` dark mode; `registerDynBtn` plugin API; `ToolbarButtonAction` |
| `WinControls/TreeView.cpp` | Written (790L) | `QTreeWidget`-based, replaces Win32 `WC_TREEVIEW` |

### CMakeLists.txt cleanup
- Removed incorrect subdir references for panels with flat canonical files
- Added subdir `.cpp` references for `ClipboardHistory/`, `FileBrowser/`
- Added `MISC_PROCESS` source set for `MISC/Process/Processus.cpp`
- Removed duplicate `FileBrowser` entry
- Deleted orphaned subdirs: `ColourPicker/`, `DockingWnd/`, `FindCharsInRange/`, `PluginsAdmin/`, `StaticDialog/`, `ToolBar/`, `FunctionList/`, `SplitterContainer/` (partial duplicates from abandoned sessions)
- `FileBrowser/`, `ClipboardHistory/` subdir implementations kept and wired in CMakeLists.txt

### Semantic lift status
- All 31 WinControls panels ✅ implemented in npp-qt target
- `SEMANTIC_LIFT_MAP.md` current

---

## Session 2026-06-29 (late)

### ToolBar full implementation written
- `WinControls/ToolBar.cpp` — 892 lines, full lift:
  - **Toolbar::init(...)** — QAction[] rebuild from button units
  - **Toolbar::initTheme()** — QDomElement theme/XML icon loading
  - **Toolbar::initHideButtonsConf()** — QDomElement visibility state
  - **Toolbar::reset(bool)** — full toolbar widget rebuild
  - **Toolbar::rebuildActions()** — QToolBar + QAction per button unit
  - **Toolbar::findAction/updateIconLists/applyCustomIcons** — icon list management
  - **Toolbar::addToRebar()** — register with ReBar container
  - **Toolbar::event()** — DPI change handling
  - **ReBar** full container implementation (addBand, reNew, removeBand, ID management)
  - **ToolBarIcons::init/create/resizeIcon/replaceIcon/destroy** — QIcon×8 image list manager
- Header fix: added `QDomElement _toolIcons`, `QMap<int,QIcon> _dynIconMap`, fixed `ToolBarIcons::init()` signature, added `ImageListSet.h` include for `HLIST_*` constants
