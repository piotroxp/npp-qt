# NPP-Qt Semantic Lift — Translation Map

## Win32 → Qt6 Pattern Mapping

| Win32 | Qt6 |
|-------|-----|
| `HWND` | `QWidget*` |
| `HINSTANCE` | `QApplication*` |
| `HDC` | `QPainter*` |
| `WNDCLASSEX` | `QWindow::create()` + `QWidget` |
| `CreateWindow()` | `new QWidget()` / `setWindowTitle()` |
| `ShowWindow(hwnd, SW_SHOW)` | `widget->show()` |
| `MoveWindow(hwnd, ...)` | `widget->resize()` / `widget->move()` |
| `SetFocus(hwnd)` | `widget->setFocus()` |
| `SendMessage(hwnd, WM_*)` | `QMetaObject::invokeMethod()` / signals |
| `LRESULT CALLBACK WndProc(...)` | `QWidget::event()` override |
| `GetClientRect(hwnd, &rc)` | `widget->rect()` |
| `InvalidateRect(hwnd, NULL, TRUE)` | `widget->update()` |
| `HDC hdc = GetDC(hwnd)` | `QPainter painter(widget)` |
| `TextOut(hdc, ...)` | `painter.drawText()` |
| `CreateSolidBrush(color)` | `QBrush(color)` |
| `SelectObject(hdc, brush)` | `painter.setBrush()` |
| `TCITEM` | `QTabWidget tab + userData` |
| `TabCtrl_*` messages | `QTabWidget` signals/methods |
| `DockingManager` | `DockingManager` (Qt widget) |
| `DockingCont` | `DockingContainer` (QTabWidget-based) |
| `StaticDialog` | `StaticDialog` (QWidget-based) |

## Translation Status

| Component | Source | Target | Status |
|-----------|--------|--------|--------|
| Window | WinControls/Window.h | WinControls/Window.h | ✅ DONE |
| TabBar | WinControls/TabBar/* | WinControls/TabBar.h/cpp | ✅ DONE |
| ToolBar | WinControls/ToolBar/* | WinControls/ToolBar.h/cpp | ✅ DONE |
| StatusBar | WinControls/StatusBar/* | WinControls/StatusBar.h/cpp | ✅ DONE |
| MainWindow | Notepad_plus_Window.cpp | MainWindow.h/cpp | ✅ DONE |
| Buffer | Buffer.h | Buffer.h | ✅ DONE |
| CMakeLists.txt | - | CMakeLists.txt | ✅ DONE |
| **DockingWnd** | WinControls/DockingWnd/* | WinControls/DockingWnd.h/cpp | ✅ LIFTED |
| **SplitterContainer** | WinControls/SplitterContainer/* | WinControls/SplitterContainer.h/cpp | ✅ LIFTED |
| **StaticDialog** | WinControls/StaticDialog/* | WinControls/StaticDialog.h/cpp | ✅ LIFTED |
| DockingWnd | WinControls/DockingWnd/* | - | ✅ DONE |
| StaticDialog | WinControls/StaticDialog/* | - | ✅ DONE |
| SplitterContainer | WinControls/SplitterContainer/* | - | ✅ DONE |
| ScintillaComponent | scintilla/qt/ | Partial | ⏳ PENDING |
| **RunDlg** | WinControls/StaticDialog/RunDlg/* | WinControls/StaticDialog.h/cpp | ✅ LIFTED |
| **AboutDlg** | WinControls/AboutDlg/* | WinControls/AboutDlg.h/cpp | ✅ LIFTED |
| DocumentMap | WinControls/DocumentMap/* | - | ⏳ PENDING |
| FunctionList | WinControls/FunctionList/* | - | ⏳ PENDING |
| FileBrowser | WinControls/FileBrowser/* | - | ⏳ PENDING |
| ProjectPanel | WinControls/ProjectPanel/* | - | ⏳ PENDING |
| Preference | WinControls/Preference/* | - | ⏳ PENDING |
| PluginsAdmin | WinControls/PluginsAdmin/* | - | ⏳ PENDING |
| ShortcutMapper | WinControls/Grid/ShortcutMapper/* | - | ⏳ PENDING |

## Project Structure

```
npp-qt/
├── CMakeLists.txt           ✅
├── src/
│   ├── main.cpp            ✅
│   ├── MainWindow.h/cpp    ✅
│   ├── Buffer.h            ✅
│   └── WinControls/
│       ├── Window.h        ✅ (header-only, inline impl)
│       ├── TabBar.h/cpp    ✅
│       ├── ToolBar.h/cpp   ✅
│       ├── StatusBar.h/cpp ✅
│       ├── DockingWnd.h/cpp ✅ (new)
│       ├── SplitterContainer.h/cpp ✅ (new)
│       └── StaticDialog.h/cpp ✅ (new)
└── SEMANTIC_LIFT_MAP.md    ✅
```

## Semantic Lift Notes

- **ScintillaEdit** is forward-declared in MainWindow.h — the Scintilla component is a separate lift target (sci_defines \| Scintilla.h)
- **DockingWnd** uses a custom tab-based container (QTabWidget + QStackedWidget) instead of raw Win32 tab controls
- **SplitterContainer** uses Qt's QSplitter for the heavy lifting, with a custom Splitter widget for specialized cases
- **StaticDialog** is now QWidget-based rather than QDialog, giving more flexibility for docking
- No compile-time check available in this environment — full verification requires Qt6 toolchain
