# Themes Reference

How themes work in Notepad--Qt, how to switch themes, and how to create custom themes.

---

## Built-in Themes

Notepad--Qt ships with 7 built-in themes, implemented as methods in `SyntaxHighlighter`:

| Theme | Style | Background | Accent |
|-------|-------|-----------|--------|
| **Default** | Light | White | Blue |
| **Dark** | Dark | `#1E1E1E` | `#007ACC` |
| **Dracula** | Dark | `#282A36` | `#BD93F9` |
| **Monokai** | Dark | `#272822` | `#F8F8F2` |
| **Nord** | Dark | `#2E3440` | `#88C0D0` |
| **One Dark** | Dark | `#282C34` | `#61AFEF` |
| **Solarized Dark** | Dark | `#002B36` | `#268BD2` |
| **Light** | Light | `#FAFAFA` | `#657B83` |

---

## Switching Themes

### Via Menu
```
Settings → Style Configurator → select theme → Apply
```

### Via Command Palette
```
Ctrl+Shift+P → "theme dark" → Enter
```

### Via Command Line
```bash
./build/NotepadMinusMinusQt --profile dark file.txt
./build/NotepadMinusMinusQt --profile monokai file.txt
./build/NotepadMinusMinusQt --profile nord file.txt
```

### Via Code
```cpp
// Apply a theme to the active editor
editor->applyDarkTheme();
editor->applyMonokaiTheme();
editor->applyNordTheme();

// Apply to all open editors (via Application signal)
application->loadTheme("monokai");
```

---

## How Theming Works

### Theme Signal Chain

```
Application::loadTheme(name)
  → emit themeChanged(name)
  → Application::onThemeChanged(name)
      → _themeObserver.notify(ThemeNotification{name})
      → for each open editor: editor->applyTheme(name)
      → MainWindow::setStyleSheet() (window chrome)
```

### SyntaxHighlighter Theme Methods

Each theme method sets foreground/background colors for the Scintilla editor:

```cpp
// Dark theme example (from SyntaxHighlighter::applyDarkTheme)
void SyntaxHighlighter::applyDarkTheme() {
    _themeName = "dark";
    setThemeColors("#D4D4D4", "#1E1E1E");  // text / background
    // Sets Scintilla lexer colors, selection, caret, etc.
}
```

### Scope of Theme Changes

A theme switch changes:
- **Editor background** — Scintilla viewport background
- **Editor foreground** — Default text color
- **Syntax colors** — Keywords, strings, comments, numbers, etc.
- **Selection** — Highlighted text background
- **Caret** — Cursor color
- **Current line** — Highlight background
- **Line numbers** — Color and background
- **Window chrome** — Menu bar, toolbar, status bar stylesheet

A theme switch does **not** change:
- Panel dock widget title bar (uses system theme)
- Dialog windows (use system palette)

---

## Custom Themes (JSON)

Custom theme files are stored in `~/.config/notepad--qt/themes/`.

### Theme File Format

```json
{
  "name": "My Theme",
  "background": "#1A1A2E",
  "foreground": "#EAEAEA",
  "selectionBackground": "#44475A",
  "caret": "#F8F8F2",
  "currentLineBackground": "#2D2D44",
  "lineNumberForeground": "#6272A4",
  "fontFamily": "monospace",
  "fontSize": 11,
  "lexerStyles": {
    "default":  { "foreground": "#EAEAEA" },
    "keyword":  { "foreground": "#FF79C6" },
    "string":   { "foreground": "#F1FA8C" },
    "comment":  { "foreground": "#6272A4", "italic": true },
    "number":   { "foreground": "#BD93F9" },
    "operator": { "foreground": "#FF79C6" },
    "identifier": { "foreground": "#50FA7B" }
  }
}
```

### Installing a Custom Theme

1. Create or download a `.json` theme file
2. Place it in `~/.config/notepad--qt/themes/`
3. Restart Notepad--Qt (or use `Ctrl+Shift+P` → "theme My Theme")

### Theme Configurator (UI)

The **Style Configurator** dialog (`Settings → Style Configurator`) allows you to:
- Select a base theme
- Override colors for individual lexer styles
- Preview changes live
- Save as a new custom theme

---

## Theme Priority

When `Application::loadTheme(name)` is called, themes are resolved in this order:

1. **Custom JSON theme** in `~/.config/notepad--qt/themes/<name>.json`
2. **Built-in theme** — `applyDarkTheme()`, `applyMonokaiTheme()`, etc. in `SyntaxHighlighter`
3. **Fallback** — Default (light) theme

---

## Importing NPP Themes

Notepad--Qt can import Notepad++ `.theme` / `stylers.xml` theme files:

```
Settings → Import → Import Notepad++ Theme
```

This copies the NPP XML theme to `~/.config/notepad--qt/themes/` and converts it to the JSON format.

**Note:** NPP uses `.xml`-based themes (`stylers.xml`). Notepad--Qt uses `.json`-based themes. The importer is basic — complex NPP themes may not convert perfectly.

---

## Dark Mode

Notepad--Qt's dark mode is not tied to the system dark mode. To match your system:

**GNOME / GTK:** Notepad--Qt uses Qt's native theme (not GTK). For a consistent look:
```bash
QT_QPA_PLATFORM_THEME=gtk3 ./build/NotepadMinusMinusQt
```

**KDE / Breeze:**
```bash
QT_QPA_PLATFORM_THEME=breeze ./build/NotepadMinusMinusQt
```

**Force light/dark regardless of system:**
```bash
QT_QPA_PLATFORM_THEME=fusion ./build/NotepadMinusMinusQt   # always light
```

---

## Known Limitations

- **NPP StylerArray** — The full NPP `StylerArray` / `ThemeManager` system (from `WinControls/ColourPicker/`) is not fully mapped. Some NPP themes with custom lexer styles may not apply correctly.
- **Panel styling** — Dock panels use system theme, not the selected editor theme.
- **Custom themes** — The custom theme JSON format is basic. Advanced NPP theme features (multiple font styles, border colors, indicator colors) are not yet supported.
- **Theme per language** — All languages use the same theme. Per-language theme overrides are not yet supported.
