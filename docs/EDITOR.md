# Editor Reference

How the Scintilla-based editor works — ScintillaEditor, SyntaxHighlighter, LexerConfig, DocTabView, and WebBrowserView.

---

## Architecture Overview

```
Application
  └── MainWindow (QMainWindow)
        └── DocTabView (QTabWidget)
              ├── ScintillaEditor* tab 0
              ├── ScintillaEditor* tab 1
              └── ScintillaEditor* tab N
                    ├── SyntaxHighlighter   (QSyntaxHighlighter)
                    ├── QsciScintilla        (underlying Scintilla widget)
                    └── AnnotationOverlay
```

Each open file is a `ScintillaEditor` owned by a `DocTabView`. `SyntaxHighlighter` is a Qt `QSyntaxHighlighter` that wraps Scintilla's lexer API.

---

## ScintillaEditor (`src/editor/ScintillaEditor`)

The central editor widget. Wraps `QsciScintilla` and adds Notepad--Qt-specific behavior.

**Files:** `ScintillaEditor.h` (1150 lines), `ScintillaEditor.cpp`

### Constructor

```cpp
explicit ScintillaEditor(QWidget* parent = nullptr);
```

Creates the underlying `QsciScintilla` and connects signals for `modificationChanged`, `cursorPositionChanged`, `textChanged`, etc.

### Key Public Methods

#### Text Operations
| Method | Description |
|--------|-------------|
| `setText(const QString&)` | Set full document text |
| `setPlainText(const QString&)` | Same as setText |
| `insertText(const QString&)` | Insert at current cursor |
| `setLineText(line, text)` | Replace entire line |
| `getTextAsUtf8()` | Return document as UTF-8 `QByteArray` |
| `getLineText(line)` | Get text of a single line |

#### Navigation
| Method | Description |
|--------|-------------|
| `gotoLine(line, col=0)` | Move cursor and scroll to line |
| `setCursorPosition(line, col)` | Move cursor without scrolling |
| `getCursorPosition(&line, &col)` | Get current cursor position |
| `ensureLineVisible(line)` | Scroll to make line visible |

#### Selection
| Method | Description |
|--------|-------------|
| `setSelection(sLine, sCol, eLine, eCol)` | Set rectangular selection |
| `selectAll()` | Select entire document |
| `clearSelection()` | Clear current selection |
| `hasSelectedText()` | Check if anything is selected |
| `getSelection()` | Return selected text |
| `setColumnSelectionMode(bool)` | Enable column selection |
| `setRectangularSelectionEnabled(bool)` | Enable multi-cursor editing |

#### Language / Highlighting
| Method | Description |
|--------|-------------|
| `setLanguage(LangType)` | Apply lexer by language type |
| `setTheme(const QString&)` | Apply color theme |
| `getCurrentLexer()` | Return current `QsciLexer*` |

#### Folding
| Method | Description |
|--------|-------------|
| `foldAll()` | Collapse all folds |
| `unfoldAll()` | Expand all folds |
| `toggleFold(line)` | Toggle fold at line |
| `setAutomaticFolding(QsciScintilla::AutomaticFolding)` | Auto-fold on scroll |

#### Indicators (Highlights)
| Method | Description |
|--------|-------------|
| `setIndicatorDrawBehind(bool)` | Draw indicators behind text |
| `fillIndicatorRange(sLine, sCol, eLine, eCol, indicator)` | Highlight a range |
| `clearIndicatorRange(...)` | Remove highlight |
| `findIndicators(start, end)` | Find indicators in range |

#### Bookmarks
| Method | Description |
|--------|-------------|
| `setMarkerLine(line)` | Add bookmark marker to line |
| `clearMarkerLine()` | Remove bookmark from current line |
| `nextBookmark()` | Go to next bookmarked line |
| `prevBookmark()` | Go to previous bookmarked line |
| `clearBookmarks()` | Remove all bookmarks |

#### Configuration
| Method | Description |
|--------|-------------|
| `setFont(const QFont&)` | Set editor font |
| `setTabWidth(int)` | Tab width in spaces |
| `setUseTabs(bool)` | Use real tabs vs spaces |
| `setIndentWidth(int)` | Indent width |
| `setWrapMode(bool)` | Word wrap on/off |
| `setLineNumberingEnabled(bool)` | Show/hide line numbers |
| `setWhitespaceVisibility(Scintilla::WhitespaceVisibility)` | Show spaces/tabs |
| `setAutoIndent(bool)` | Auto-indent on Enter |
| `setBackspaceUnindents(bool)` | Backspace at line start unindents |

### Key Signals

| Signal | Payload | Description |
|--------|---------|-------------|
| `modificationChanged(bool)` | dirty | Document was modified |
| `cursorPositionChanged(int, int)` | line, col | Cursor moved |
| `textChanged()` | — | Text was edited |
| `selectionChanged()` | — | Selection changed |
| `updateRequest(const QRect&, int)` | rect, dy | Viewport scroll |
| `doubleClicked(const QPoint&)` | pos | Double-click in editor |

---

## SyntaxHighlighter (`src/editor/SyntaxHighlighter`)

`QSyntaxHighlighter` subclass that drives Scintilla lexers. Does not do Qt's native syntax highlighting — it maps Qt highlighting rules to Scintilla `SCI_STYLESET*` messages.

**Files:** `SyntaxHighlighter.h` (141 lines), `SyntaxHighlighter.cpp` (2415 lines), `SyntaxHighlighterImpl.cpp` (920 lines)

### Theme Methods

Each built-in theme has a dedicated method:

```cpp
void applyDefaultTheme();   // Light
void applyDarkTheme();      // VS Code dark
void applyDraculaTheme();  // Dracula purple
void applyMonokaiTheme();  // Monokai
void applyNordTheme();     // Nord
void applyOneDarkTheme();  // Atom One Dark
void applySolarizedDarkTheme();
void applySolarizedLightTheme();
```

### Language Setup Methods

Each supported language has a `setup*()` method that configures Scintilla lexer colors:

```cpp
void setupCpp();       // C/C++
void setupPython();    // Python
void setupJavaScript(); // JavaScript/TypeScript
void setupLua();       // Lua
void setupGo();        // Go
void setupRust();      // Rust
void setupMarkdown();  // Markdown
void setupHtml();      // HTML
void setupXml();       // XML
void setupJava();      // Java
void setupRuby();      // Ruby
void setupPerl();      // Perl
void setupPhp();       // PHP
void setupSql();       // SQL
void setupJson();      // JSON
void setupYaml();      // YAML
void setupMakefile();  // Makefile
void setupCss();       // CSS
```

Each `setup*()` method sets Scintilla style IDs:
```cpp
// Example (from setupCpp)
setLexer(SCLEX_CPP);
setStyle(STYLE_DEFAULT,   "#D4D4D4", "#1E1E1E");  // fg, bg
setStyle(SCE_C_KEYWORD,  "#569CD6", nullptr);       // keywords
setStyle(SCE_C_STRING,   "#CE9178", nullptr);        // strings
setStyle(SCE_C_NUMBER,   "#B5CEA8", nullptr);        // numbers
setStyle(SCE_C_COMMENT,  "#6A9955", nullptr, QFont::StyleItalic);
// ... etc.
```

### Applying Themes

Themes are applied to all editors via the signal chain:

```
Application::loadTheme("monokai")
  → emit themeChanged("monokai")
  → Application::onThemeChanged("monokai")
      → for each open editor:
           editor->setTheme("monokai")
      → MainWindow::setStyleSheet(...)  // window chrome
```

---

## LexerConfig (`src/editor/LexerConfig`)

Singleton that maps between file extensions, content patterns, and `LangType` values.

**File:** `LexerConfig.cpp` (612 lines)

### Supported Languages (54 total)

| LangType | ID | Lexer | Extensions |
|----------|----|------|------------|
| `L_NORMAL` | 0 | None | — |
| `L_CPP` | 2 | `SCLEX_CPP` | `.c`, `.cpp`, `.cc`, `.cxx`, `.h`, `.hpp` |
| `L_HTML` | 6 | `SCLEX_HTML` | `.html`, `.htm`, `.xhtml` |
| `L_XML` | 7 | `SCLEX_XML` | `.xml`, `.xsl`, `.xslt`, `.svg` |
| `L_LUA` | 9 | `SCLEX_LUA` | `.lua` |
| `L_PERL` | 10 | `SCLEX_PERL` | `.pl`, `.pm` |
| `L_PHP` | 11 | `SCLEX_PHP` | `.php` |
| `L_RUBY` | 12 | `SCLEX_RUBY` | `.rb` |
| `L_PYTHON` | 13 | `SCLEX_PYTHON` | `.py`, `.pyw` |
| `L_JAVASCRIPT` | 14 | `SCLEX_CPP` | `.js`, `.jsx`, `.mjs` |
| `L_JSON` | 15 | `SCLEX_JSON` | `.json` |
| `L_YAML` | 17 | `SCLEX_YAML` | `.yml`, `.yaml` |
| `L_MAKEFILE` | 18 | `SCLEX_MAKEFILE` | `Makefile`, `makefile` |
| `L_MARKDOWN` | 20 | `SCLEX_MARKDOWN` | `.md`, `.markdown` |
| `L_GO` | 24 | `SCLEX_GO` | `.go` |
| `L_RUST` | 25 | `SCLEX_RUST` | `.rs` |
| `L_RUBY2` | 30 | `SCLEX_RUBY` | `.rb` |
| `L_R` | 49 | `SCLEX_R` | `.r`, `.R` |
| `L_REGEX` | 55 | `SCLEX_REGEX` | — |
| `L_SQL` | 57 | `SCLEX_SQL` | `.sql` |
| `L_REGISTRY` | 3000 | `SCLEX_REGISTRY` | `.reg` |

### Detection Pipeline

```cpp
LangType LangType = LexerConfig::detect(filename, content);
// Steps:
// 1. detectFromExtension(filename)  → check file extension
// 2. detectFromContent(content)       → heuristics (shebang, keywords)
// 3. detectHeaderLanguage(content)    → #include, using, import patterns
// 4. detectObjCOrMatlab(content)      → @interface, @implementation
// 5. isPHPCode(content)               → <?php ... ?>
// 6. isMakefile(filename)             → filename == "Makefile"
// 7. L_NORMAL                         → fallback
```

### Key Methods

| Method | Description |
|--------|-------------|
| `detect(filename, content)` | Full detection pipeline |
| `detectFromExtension(filename)` | Extension-based detection |
| `detectFromContent(content)` | Content heuristics |
| `lexerInfo(lang)` | Get lexer name + Scintilla ID for a language |
| `lexerInfoByName(name)` | Find language by display name |
| `lexerInfoByExtension(ext)` | Find language by extension |
| `extensionsFor(lang)` | All extensions for a language |
| `displayName(lang)` | Human-readable language name |
| `hasBlockComments(lang)` | Does the language support `/* */` comments? |
| `getCommentStyle(lang)` | Get line and block comment strings |
| `loadUDL(configPath)` | Load a user-defined language definition |
| `registerUDLLexer(name, pattern)` | Register a UDL lexer |

---

## DocTabView (`src/editor/DocTabView`)

`QTabWidget` subclass that manages multiple `ScintillaEditor` instances as tabs.

**Files:** `DocTabView.h`, `DocTabView.cpp` (622 lines)

### Key Methods

| Method | Description |
|--------|-------------|
| `addEditor(ScintillaEditor*)` | Add a new tab with an editor |
| `removeEditor(ScintillaEditor*)` | Close a tab |
| `setActiveEditor(ScintillaEditor*)` | Activate a specific tab |
| `activeEditor()` | Get the currently active editor |
| `currentEditor()` | Alias for `activeEditor()` |
| `editors()` | `QList<ScintillaEditor*>` of all open tabs |
| `switchToNext()` | Activate the next tab |
| `switchToPrev()` | Activate the previous tab |
| `closeAll()` | Close all tabs |
| `closeOthers(index)` | Close all tabs except index |

### Signals

| Signal | Payload | Description |
|--------|---------|-------------|
| `tabCloseRequested(int)` | index | User clicked X on tab |
| `currentChanged(int)` | index | Active tab changed |
| `editorModified(bool)` | dirty | Current editor's dirty state changed |
| `lastTabClosed()` | — | All tabs are closed |

### Tab Context Menu

Right-click on a tab shows:
- Close (`Ctrl+W`)
- Close All
- Close Others
- Pin/Unpin tab
- "Open in New Window" (⚠️ not wired)

---

## WebBrowserView (`src/editor/WebBrowserView`)

Embedded web view for previewing HTML/Markdown.

**Files:** `WebBrowserView.h` (426 lines), `WebBrowserViewFactory.cpp` (211 lines)

### Factory

`WebBrowserViewFactory::create()` returns a `WebBrowserView*`:
```cpp
WebBrowserView* WebBrowserViewFactory::create(QWidget* parent);
```

### Key Methods

| Method | Description |
|--------|-------------|
| `loadUrl(const QUrl&)` | Load a URL in the view |
| `setHtml(const QString&, const QUrl& base)` | Set HTML content directly |
| `reload()` | Reload current page |
| `back()` | Go back in history |
| `forward()` | Go forward in history |
| `runJavaScript(const QString&)` | Execute JS in the page |

### Use Cases

- **HTML preview:** When an `.html` file is open, `View → Preview` opens a `WebBrowserView` tab showing the rendered page
- **Markdown preview:** When a `.md` file is open, `View → Preview` renders Markdown to HTML and displays it
- **PDF preview:** ⚠️ Not yet implemented

### Limitations

- JavaScript must be enabled via `settings.javascriptEnabled = true` (default: true)
- Local file access (`file://`) is restricted on some platforms
- No DevTools or inspector

---

## ScintillaCtrls (`src/editor/ScintillaCtrls`)

Collection of Scintilla control helpers — shared functionality across multiple Scintilla instances.

**File:** `ScintillaCtrls.cpp` (684 lines)

Contains utility functions and shared Scintilla state management used by:
- The main editor tabs
- The Document Map (read-only Scintilla view)
- The Function Call Tip popup
- The Incremental Search highlight

---

## MockEditor (`src/editor/MockEditor`)

Test double for `ScintillaEditor` — used in unit tests to avoid depending on the full Qt GUI stack.

**File:** `MockEditor.cpp` (285 lines)

Provides a minimal `QWidget` that implements the same `ScintillaEditor`-like interface for testing text operations without a display:

```cpp
class MockEditor {
    QString _text;
    int _cursorLine = 0;
    int _cursorCol = 0;
    bool _dirty = false;
public:
    void setText(const QString& t) { _text = t; }
    QString text() const { return _text; }
    void setCursorPosition(int line, int col) { _cursorLine = line; _cursorCol = col; }
    void insertText(const QString& t) { /* ... */ }
    void setLanguage(LangType) { /* no-op in mock */ }
};
```

---

## Indicators Reference

Scintilla indicators are used to highlight search results, bookmarks, errors, etc.

| Indicator ID | Purpose | Color |
|-------------|---------|-------|
| `INDICATOR_FIND` | Find/Replace highlight | Yellow |
| `INDICATOR_SMART_HIGHLIGHT_1` | Smart highlighter word 1 | Yellow |
| `INDICATOR_SMART_HIGHLIGHT_2` | Smart highlighter word 2 | Green |
| `INDICATOR_SMART_HIGHLIGHT_3` | Smart highlighter word 3 | Cyan |
| `INDICATOR_SPELL_CHECK` | Spell-check errors | Red wavy underline |
| `INDICATOR_BREAKPOINT` | Debug breakpoint | Red |
| `INDICATOR_CURRENT_LINE` | Current line highlight | Gray (alpha) |

---

## Scintilla Message Mapping

Notepad--Qt uses `QsciScintilla` (the Qt wrapper for Scintilla). Some direct Scintilla messages are sent via `sendSciMessage()`:

| Message | Use |
|---------|-----|
| `SCI_GETCURLINE` | Get the text of the current line |
| `SCI_GETCURRENTPOS` | Get cursor byte offset |
| `SCI_GETLINE` | Get text of a specific line |
| `SCI_INDICATORFILLRANGE` | Apply indicator to range |
| `SCI_INDICATORCLEARRANGE` | Remove indicator from range |
| `SCI_SEARCHINTARGET` | Search within a text range |
| `SCI_SETTARGETRANGE` | Set search region for replace |
| `SCI_REPLACETARGET` | Replace in search target |
| `SCI_MARKERADD` | Add a bookmark marker |
| `SCI_MARKERDELETE` | Remove a bookmark marker |
| `SCI_MARKERGET` | Get markers on a line |
| `SCI_GETFOLDLEVEL` | Get fold state of a line |
| `SCI_SETFOLDLEVEL` | Set fold state |

---

## Adding a New Language Lexer

1. **Add `LangType` enum value** in `src/common/Types.h`:
   ```cpp
   L_ZIG = 60,  // example
   ```

2. **Add lexer info** in `LexerConfig::buildLexerTable()`:
   ```cpp
   _lexerTable[LangType::L_ZIG] = {"Zig", SCLEX_ZIG, {".zig"}};
   ```

3. **Add setup method** in `SyntaxHighlighter.h/.cpp`:
   ```cpp
   void setupZig();
   ```

4. **Wire in `setLanguage()`** in `ScintillaEditor::setLanguage()`:
   ```cpp
   case L_ZIG:
       _highlighter->setupZig();
       break;
   ```

5. **Set Scintilla lexer ID** in `SyntaxHighlighter::setupZig()`:
   ```cpp
   setLexer(SCLEX_CONTAINER);  // or SCLEX_ZIG if available
   setStyle(STYLE_DEFAULT, "#D4D4D4", "#1E1E1E");
   // ... configure keywords, strings, comments
   ```
