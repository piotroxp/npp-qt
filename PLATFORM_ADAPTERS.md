# Platform Adapter Layer

This document describes the platform-abstraction adapters that allow the
Notepad--Qt codebase (Linux/Qt6) to provide the same functionality as the
original Win32 Notepad++ codebase.  Each adapter wraps the Qt6 equivalent of
the Win32 API used in the original source.

---

## Table of Contents

1. [ClipboardAdapter](#clipboardadapter)
2. [ClipboardFormat Constants](#clipboard-format-constants)
3. [Platform-Specific Behaviour Notes](#platform-specific-behaviour-notes)

---

## ClipboardAdapter

**File:** `src/common/ClipboardAdapter.h` / `ClipboardAdapter.cpp`

**Win32 source:** `PowerEditor/src/MISC/Common/Common.cpp` (`str2Clipboard`,
`strFromClipboard`) and `PowerEditor/src/WinControls/ClipboardHistory/clipboardHistoryPanel.cpp`
(`GetClipboardData` / `SetClipboardData` / `CF_UNICODETEXT`).

**Purpose:** Provides a platform-neutral clipboard interface wrapping
`QClipboard` / `QMimeData`.  One `ClipboardAdapter` instance wraps
`QGuiApplication::clipboard()` and exposes both Win32-semantics operations
(plain text) and MIME-level operations (arbitrary formats, HTML, images,
URIs).

### API surface

| Method | Qt6 equivalent | Win32 equivalent |
|---|---|---|
| `getText()` | `QClipboard::text()` | `GetClipboardData(CF_UNICODETEXT)` |
| `setText(str)` | `QClipboard::setText()` | `SetClipboardData(CF_UNICODETEXT)` |
| `getData(format)` | `QMimeData::data(format)` | `GetClipboardData(format)` |
| `setData(format, data)` | `QMimeData::setData(format, data)` | `SetClipboardData(format, hMem)` |
| `hasFormat(format)` | `QMimeData::hasFormat(format)` | `IsClipboardFormatAvailable(format)` |
| `formats()` | `QMimeData::formats()` | enumerate registered formats |
| `clear()` | `QClipboard::clear()` | `EmptyClipboard()` |
| `getImage()` | `QMimeData::imageData()` | `GetClipboardData(CF_BITMAP/CF_DIB)` |
| `setImage(img)` | `QClipboard::setImage()` | `SetClipboardData(CF_BITMAP/CF_DIB)` |
| `getHtml()` | `QMimeData::html()` | `GetClipboardData(CF_HTML)` |
| `setHtml(html, plain)` | `QMimeData::setHtml()` + `setText()` | `SetClipboardData(CF_HTML)` |
| `getUrls()` | `QMimeData::urls()` | `GetClipboardData(CF_HDROP)` |
| `setUrls(urls)` | `QMimeData::setUrls()` | `SetClipboardData(CF_HDROP)` |
| `pushClipboard()` | snapshot `QMimeData` formats | internal notepad++ "save clipboard" |
| `popClipboard()` | restore snapped `QMimeData` | internal notepad++ "restore clipboard" |
| `copyText(text)` | alias of `setText()` | `str2Clipboard()` |

### Usage example

```cpp
#include "common/ClipboardAdapter.h"

// In a class member or function:
ClipboardAdapter clipboard;

void copySelection(const QString& selected) {
    clipboard.copyText(selected);
}

void pasteIntoEditor(ScintillaEditor* editor) {
    QString text = clipboard.getText();
    if (!text.isEmpty()) {
        editor->insertText(text);
    }
}
```

### Integration points

- `src/dialogs/ClipboardHistoryPanel.cpp` — already uses `QClipboard` directly.
  Can be refactored to use `ClipboardAdapter` for consistent semantics.
- `src/core/NppCommands.cpp` — `searchOnInternet()` uses `QClipboard::text()`
  directly; may optionally switch to `ClipboardAdapter::getText()`.
- `src/ui/MainWindow.cpp` — `onCut()` / `onCopy()` / `onPaste()` delegate to
  `ScintillaEditor` (which calls Scintilla's clipboard); `ClipboardAdapter` is
  available for non-editor clipboard operations (e.g. copying file paths).

---

## Clipboard Format Constants

**File:** `src/clipboardFormats.h`

Maps Win32 `RegisterClipboardFormat()` identifiers to MIME-type strings used
by Qt6's `QMimeData`.

| Win32 constant / identifier | MIME type |
|---|---|
| `CF_UNICODETEXT` | `text/plain` |
| `CF_TEXT` | `text/plain;charset=us-ascii` |
| `CF_HTML` | `text/html` |
| `CF_RTF` | `text/rtf` |
| `CF_HDROP` | `text/uri-list` |
| `"Notepad++ Binary Length"` (registered format) | `application/x-npp-text-length` |

The `NppClipboardFormat` namespace provides compile-time string constants:

```cpp
#include "clipboardFormats.h"

// In clipboard operations:
adapter.setData(NppClipboardFormat::NppTextLenMime, byteArray);
```

---

## Platform-Specific Behaviour Notes

### Linux/X11

- **Clipboard selection:** Qt6/X11 uses the `CLIPBOARD` selection by default
  (the one managed by desktop clipboard managers such as `parcellite`,
  `clipman`, or the desktop environment's built-in manager).
- **Primary selection:** Qt6/X11 maps the X11 `PRIMARY` selection (mouse
  middle-click paste) to `QClipboard::Selection`.  This is distinct from
  `QClipboard::Clipboard`.  Notepad--Qt currently uses `Clipboard` only.
- **Format fidelity:** The X11 clipboard accepts multiple MIME formats
  simultaneously.  Round-trip of custom formats (e.g. `application/x-npp-*`)
  works reliably for data under a few kilobytes.

### Linux/Wayland

- **Environment:** Set `QT_QPA_PLATFORM=wayland` to force the Wayland backend.
- **Clipboard protocol:** Wayland compositors implement the
  `wl_data_device_manager` clipboard protocol.  The fidelity of multi-format
  clipboard data depends on the compositor.
  - **GNOME Shell** ( Mutter): full round-trip of `text/plain` + `text/html`.
    Custom MIME types may or may not survive depending on clipboard manager
    integration.
  - **KDE Plasma** (KWin): generally good fidelity for `text/plain`, `text/html`,
    and standard image formats.
  - **Sway / wlroots compositors**: varying support; custom MIME types are often
    dropped.  Prefer `text/plain` for operations that must work everywhere.
- **Recommendation:** When building features that rely on custom MIME types
  (e.g. the NPP binary-length format), prefer a plain-text fallback and document
  the degraded behaviour on Wayland.

### Testing clipboard in CI / headless environments

```bash
# Use the offscreen Qt platform plugin (no display server required):
export QT_QPA_PLATFORM=offscreen
./build/src/tests/test_clipboard
```

`QClipboard` is functional with the `offscreen` platform plugin, so all tests
run without a display.

---

## Adding new platform adapters

Each new adapter should follow this checklist:

1. **Create `src/common/PlatformAdapter.h` / `.cpp`** with:
   - A class wrapping the Qt6 equivalent
   - A table in this document mapping each Win32 call to its Qt6 equivalent
   - `constexpr` or `#define` format constants in `src/clipboardFormats.h`
2. **Add to `CMakeLists.txt`:** list the `.cpp` in `SOURCES`, the `.h` in
   `HEADERS`.
3. **Add to `src/tests/CMakeLists.txt`:** list the `.cpp` in `test_core`.
4. **Write a unit test** in `src/tests/test_<name>.cpp` using `QTEST_MAIN`.
5. **Document** in this file and in the class header comments.
6. **Build verification:**
   ```bash
   cmake -S /home/node/.openclaw/workspace/npp-qt -B /home/node/.openclaw/workspace/npp-qt/build
   cmake --build /home/node/.openclaw/workspace/npp-qt/build
   ./build/src/tests/test_<name>
   ```
7. **Smoke test:**
   ```bash
   ./build/NotepadMinusMinusQt --help
   ```
