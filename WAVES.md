# npp-qt: Wave Plan — Updated 2026-07-13 Evening (v0.2.0)
_After v0.2.0 tag (d81f8a5) — CI, plugins, find-in-files, print, macros_

## Current State
- **Binary:** `build/NotepadMinusMinusQt` (1.9MB) — clean build ✅
- **Tests:** 3/5 passing (test_buffer/test_util hang — Qt static-dtor harness issue, pre-existing)
- **Source:** 75 `.cpp`, 144 total files
- **TODOs/stubs:** 0 in main codebase ✅
- **Tags:** `v0.1.0-semantic-lift`, `v0.2.0`

## Known Stubs (inert — linker only)
- `src/NppCommandsSlots_stub.cpp` (76 lines) — linker stubs for disabled .cpp files
- `src/NppDarkMode.cpp` (682 lines) — Win32 theming, not compiled in Qt6 build
- `src/core/Parameters.cpp` — some `return {}` stubs for NppParameters wrapper

## Gate Criteria
Every wave: `cmake --build build -j$(nproc)` must exit 0 before commit.

---

## Wave 16 — Code Folding + Auto-Indent + Bracket Highlight
**Goal:** IDE-quality editor features — fold regions, smart indent, bracket matching.

### Agent tasks

#### 1. Code Folding

`QsciScintilla` has built-in code folding via SCI. Enable it:

In `ScintillaEditor.cpp`, update constructor or `init()`:
```cpp
// Enable code folding
_editor->setFolding(QsciScintilla::BoxedTreeFoldStyle);
// Show fold margin
_editor->setMarginType(2, QsciScintilla::SymbolMargin);
_editor->setMarginWidth(2, "0");
_editor->setMarginSensitivity(2, true);

// Set fold markers
_editor->setMarkerForegroundColor(QColor("#cccccc"), 0);
_editor->setMarkerBackgroundColor(QColor("#e0e0e0"), 0);
_editor->setFoldMarkerColors(QColor("#cccccc"), QColor("#e0e0e0"));

// Connect fold click
connect(_editor, &QsciScintilla::marginClicked, this, [this](int margin, int line, Qt::KeyboardModifiers) {
    if (margin == 2) {
        _editor->toggleFold(line);
    }
});
```

Add `toggleFold(int line)` if not present:
```cpp
void ScintillaEditor::toggleFold(int line) {
    if (_editor->isFolded(line))
        _editor->foldLine(line);
    else
        _editor->unfoldLine(line);
}
```

Wire menu commands:
```cpp
// In onMenuCommand()
else if (cmd == "view.foldAll") {
    if (_activeEditor) _activeEditor->foldAll();
}
else if (cmd == "view.unfoldAll") {
    if (_activeEditor) _activeEditor->unfoldAll();
}
```

Add to `ScintillaEditor.h`:
```cpp
void foldAll();
void unfoldAll();
```

Implement:
```cpp
void ScintillaEditor::foldAll() {
    for (int line = _editor->lines() - 1; line >= 0; --line) {
        if (_editor->isFolded(line)) continue;
        int level = _editor->SendScintilla(SCI_GETFOLDLEVEL, line);
        if (level & SC_FOLDLEVELHEADERFLAG)
            _editor->foldLine(line);
    }
}

void ScintillaEditor::unfoldAll() {
    for (int line = 0; line < _editor->lines(); ++line) {
        if (_editor->isFolded(line))
            _editor->unfoldLine(line);
    }
}
```

#### 2. Auto-Indent

In `ScintillaEditor` constructor, enable:
```cpp
// Auto-indent
_editor->setAutoIndent(true);
_editor->setIndentationGuides(true);
_editor->setTabIndents(true);
_editor->setBackspaceUnindents(true);
_editor->setTabWidth(4);
_editor->setIndentationsUseTabs(false); // spaces by default
```

Also connect Enter/Tab:
```cpp
connect(_editor, &QsciScintilla::returnPressed, this, [this]() {
    // Get current line indentation
    int curLine = _editor->cursorLineNumber();
    QString lineText = _editor->text(curLine);
    int indent = 0;
    for (QChar c : lineText) {
        if (c == ' ') ++indent;
        else if (c == '\t') indent += 4;
        else break;
    }
    // Keep same indent on new line
    _editor->setIndent(curLine + 1, indent);
});
```

#### 3. Bracket Highlight

Enable brace matching:
```cpp
// Bracket matching
_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
// Highlight current line bracket
_editor->setHighlightGuide(true);

// Connect to update highlight on cursor change
connect(_editor, &QsciScintilla::cursorPositionChanged, this, [this](int line, int index) {
    _editor->matchBrackets();
});
```

For multi-line bracket highlight (rainbow brackets), use SCI:
```cpp
// Set bracket highlight foreground/background
_editor->setMatchedBraceBackgroundColor(QColor("#FFFF00"));  // yellow
_editor->setMatchedBraceForegroundColor(Qt::black);
_editor->setUnmatchedBraceBackgroundColor(QColor("#FF6600")); // orange
_editor->setUnmatchedBraceForegroundColor(Qt::white);
```

#### 4. Current Line Highlight (already exists? check)

Check if `_editor->setCaretLineVisible(true)` is set. If not, add:
```cpp
_editor->setCaretLineVisible(true);
```

### Build and test
```bash
cd /home/node/.openclaw/workspace
cmake --build build -j$(nproc) 2>&1 | grep -E "error:|FAILED"
```

---

## Wave 17 — Session Polish + Bookmarks + Incremental Search
**Goal:** Session remembers everything (window geometry, cursor, scroll); line bookmarks; incremental search bar.

### Agent tasks

#### 1. Session — full state (window geometry, cursor, scroll)

Update `SessionEntry` and `SessionManager`:

In `SessionManager.h`, update `SessionEntry`:
```cpp
struct SessionEntry {
    QString filePath;
    int cursorLine = 0;
    int cursorCol = 0;
    int scrollLine = 0;
    int firstVisibleLine = 0;  // scroll offset
    QString encoding;
    QString language;
    bool active = false;
    int tabIndex = -1;
};
```

In `Application::saveSession()`:
```cpp
bool Application::saveSession(const std::string& path) {
    // Save window geometry
    if (auto* win = MainWindow::instance()) {
        QSettings s;
        s.setValue("window/geometry", win->saveGeometry());
        s.setValue("window/state", win->saveState());
    }
    // ... existing buffer entries loop, now including scrollLine, firstVisibleLine, tabIndex
}
```

In `Application::loadSession()`:
```cpp
bool Application::loadSession(const std::string& path) {
    // Restore window geometry
    QSettings s;
    if (auto* win = MainWindow::instance()) {
        win->restoreGeometry(s.value("window/geometry").toByteArray());
        win->restoreState(s.value("window/state").toByteArray());
    }
    // ... existing buffer loading, now including cursor/scroll restoration
    for (const auto& e : entries) {
        BufferID buf = openFile(e.filePath.toStdString());
        if (buf && e.active) {
            setActiveBuffer(buf);
            ScintillaEditor* ed = getActiveEditor();
            if (ed) {
                ed->setCursorPosition(e.cursorLine, e.cursorCol);
                ed->SendScintilla(SCI_LINESCROLL, 0, e.firstVisibleLine);
            }
        }
    }
}
```

Add `#include <QSettings>` to SessionManager.cpp.

#### 2. Line Bookmarks

Add to `ScintillaEditor.h`:
```cpp
void toggleBookmark(int line);
void nextBookmark();
void prevBookmark();
void clearBookmarks();
QList<int> bookmarks() const { return _bookmarks; }
```

In `ScintillaEditor.cpp`:
```cpp
// Define bookmark marker (margin 1)
static const int BOOKMARK_MARKER = 0;

// In constructor:
_editor->setMarginType(1, QsciScintilla::SymbolMargin);
_editor->setMarginWidth(1, 16);
_editor->setMarginSensitivity(1, true);
_editor->setMarkerBackgroundColor(QColor("#0088FF"), BOOKMARK_MARKER);

// Connect margin click for bookmark toggle
connect(_editor, &QsciScintilla::marginClicked, this, [this](int margin, int line, Qt::KeyboardModifiers) {
    if (margin == 1) toggleBookmark(line);
});

void ScintillaEditor::toggleBookmark(int line) {
    if (_bookmarks.contains(line)) {
        _bookmarks.removeAll(line);
        _editor->markerDelete(line, BOOKMARK_MARKER);
    } else {
        _bookmarks.append(line);
        _editor->markerAdd(line, BOOKMARK_MARKER);
    }
}

void ScintillaEditor::nextBookmark() {
    int cur = _editor->cursorLineNumber();
    for (int line : _bookmarks) {
        if (line > cur) { _editor->setCursorPosition(line, 0); return; }
    }
}

void ScintillaEditor::prevBookmark() {
    int cur = _editor->cursorLineNumber();
    for (int i = _bookmarks.size() - 1; i >= 0; --i) {
        if (_bookmarks[i] < cur) { _editor->setCursorPosition(_bookmarks[i], 0); return; }
    }
}

void ScintillaEditor::clearBookmarks() {
    for (int line : _bookmarks) _editor->markerDelete(line, BOOKMARK_MARKER);
    _bookmarks.clear();
}
```

Add member: `QList<int> _bookmarks;`

Wire menu commands:
```cpp
else if (cmd == "bookmark.toggle") {
    if (_activeEditor) _activeEditor->toggleBookmark(_activeEditor->cursorLineNumber());
}
else if (cmd == "bookmark.next") {
    if (_activeEditor) _activeEditor->nextBookmark();
}
else if (cmd == "bookmark.prev") {
    if (_activeEditor) _activeEditor->prevBookmark();
}
else if (cmd == "bookmark.clear") {
    if (_activeEditor) _activeEditor->clearBookmarks();
}
```

#### 3. Incremental Search

Create `src/dialogs/IncrementalSearchDialog.h/.cpp`:

A small toolbar/dock that appears at the top of the editor:
- Single-line input that searches as you type
- Shows match count (e.g. `3 of 12`)
- Enter → next match
- Shift+Enter → previous match
- Escape → close

Implementation uses the existing `findNext`/`findPrev` from FindReplaceDialog.

### Build and test
```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:|FAILED"
```

---

## Wave 18 — Virtual Space + Drag & Drop + Multi-Cursor Foundation
**Goal:** Better editing interactions — cursor past EOL, drag-and-drop, column editing polish.

### Agent tasks

#### 1. Virtual Space (cursor past end of line)

In `ScintillaEditor` constructor:
```cpp
// Allow cursor beyond end of line
_editor->setVirtualSpaceOptions(QsciScintilla::VirtualSpace::UserAccessible);
```

Wire menu command:
```cpp
else if (cmd == "view.toggleVirtualSpace") {
    if (_activeEditor) {
        auto opts = _activeEditor->qSciEditor()->virtualSpaceOptions();
        if (opts == QsciScintilla::VirtualSpace::UserAccessible)
            _activeEditor->qSciEditor()->setVirtualSpaceOptions(QsciScintilla::VirtualSpace::NoWrap);
        else
            _activeEditor->qSciEditor()->setVirtualSpaceOptions(QsciScintilla::VirtualSpace::UserAccessible);
    }
}
```

#### 2. Drag and Drop text

QsciScintilla supports this natively:
```cpp
_editor->setAcceptDrops(true);
_editor->setDropFlags(QsciScintilla::DropTarget);
```

Wire drop to open files:
```cpp
connect(_editor, &QsciScintilla::dropped, this, [this](const QMimeData* data) {
    if (data->hasUrls()) {
        for (const QUrl& url : data->urls()) {
            if (url.isLocalFile()) {
                QString path = url.toLocalFile();
                if (QFileInfo(path).isFile())
                    Application::instance().openFile(path.toStdString());
            }
        }
    }
});
```

#### 3. Smart home / smart end

```cpp
// Smart home: jump to first non-whitespace, thenBOL, then indent
_editor->setHomeKeyNavigation(true);
// Smart backspace
_editor->setBackspaceUnindents(true);
```

#### 4. Line numbering gutter enhancements

```cpp
// Thin line numbers
_editor->setMarginType(0, QsciScintilla::NumberMargin);
_editor->setMarginWidth(0, "0"); // auto-width
// Current line number in different color
_editor->setMarginLineNumbers(0, true);
```

#### 5. Whitespace visibility

```cpp
// Show tabs and spaces as dots
_editor->setWhitespaceVisibility(QsciScintilla::WsVisibleOnlyInIndent);
```

### Build
```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:|FAILED"
```

---

## Wave 19 — Large File Handling + Async Loading
**Goal:** Open large files (10MB+) without freezing the UI.

### Agent tasks

#### 1. Async file loading — background thread

Modify `FileManager::loadFile()` to detect large files and load asynchronously:

```cpp
bool FileManager::loadFile(const QString& path, QString& outContent, EncodingType encoding) {
    QFileInfo info(path);
    static constexpr qint64 LARGE_FILE_THRESHOLD = 5 * 1024 * 1024; // 5MB
    
    if (info.size() > LARGE_FILE_THRESHOLD) {
        // Emit signal for UI to show progress, load in background
        emit loadingLargeFile(path, info.size());
        // For now, fall through to sync load but log warning
        qDebug() << "[FileManager] Large file loading:" << path << "(" << info.size() << "bytes)";
    }
    // ... existing loading logic
}
```

Create `src/workers/FileLoaderWorker.h/.cpp`:

```cpp
class FileLoaderWorker : public QObject {
    Q_OBJECT
public:
    FileLoaderWorker(const QString& path, EncodingType encoding, QObject* parent = nullptr);
    void start();
signals:
    void progress(int percent);
    void finished(bool success, const QString& content);
private:
    QString _path;
    EncodingType _encoding;
};

void FileLoaderWorker::start() {
    // Read in chunks, emit progress
    QFile f(_path);
    // ... chunked read + decode
    emit finished(true, content);
}
```

#### 2. Lazy loading for very large files (100MB+)

For files > 100MB, load first 10MB only and show indicator:

```cpp
static constexpr qint64 VERY_LARGE_THRESHOLD = 100 * 1024 * 1024;
if (info.size() > VERY_LARGE_THRESHOLD) {
    qDebug() << "[FileManager] Very large file — partial load:" << path;
    // Load first portion, set _isPartialLoad flag on Buffer
    buffer->setPartialLoad(true);
    buffer->setTotalLength(info.size());
}
```

Add to `Buffer.h`:
```cpp
bool isPartialLoad() const { return _partialLoad; }
void setPartialLoad(bool v) { _partialLoad = v; }
qint64 totalLength() const { return _totalLength; }
void setTotalLength(qint64 v) { _totalLength = v; }
```
With members:
```cpp
bool _partialLoad = false;
qint64 _totalLength = 0;
```

#### 3. Update status bar for partial loads

In `Application::onBufferActivated()`:
```cpp
Buffer* buf = _fileManager->getBuffer(buffer);
if (buf && buf->isPartialLoad()) {
    _statusBarWidget->setMessage(
        QString("Partial load: %1 / %2 MB").arg(buf->documentLength() / 1e6, 0, 'f', 1)
                                            .arg(buf->totalLength() / 1e6, 0, 'f', 1));
}
```

### Build
```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:|FAILED"
```

---

## Wave 20 — Preferences Completeness + Encoding Auto-Detect on Paste + Polish
**Goal:** Finish missing preferences; paste encoding detection; final UX polish.

### Agent tasks

#### 1. Preferences — all settings wired

Check `src/dialogs/PreferenceDialog.cpp`. Many categories may be empty stubs. For each category:
- **General**: Language, auto-update check, multi-instance toggle
- **Editor**: Tab size, indent size, tab/space preference, line numbering, word wrap, smart home, virtual space
- **Appearance**: Theme, toolbar, tab bar, status bar visibility
- **Search**: Default search options (match case, whole word, regex defaults)
- **Backup**: Auto-save interval, session restore
- **Print**: Header/footer, color print, page range

Each preference change should immediately apply to the editor and persist to `QSettings`.

#### 2. Encoding auto-detection on paste

In `ScintillaEditor`, intercept paste:
```cpp
connect(_editor, &QsciScintilla::pasteAvailable, this, [this]() {
    // Get clipboard text
    QString text = QApplication::clipboard()->text();
    // Detect encoding
    QByteArray bytes = text.toUtf8();
    EncodingType enc = EncodingDetector::instance().detect(bytes);
    // Update status bar with detected encoding
    Application::instance().setStatusBarEncoding(
        QString::fromStdString(encodingToString(enc)));
});
```

Better approach — override paste:
```cpp
connect(_editor, &QsciScintilla::modificationChanged, this, [this]() {
    // Check if last change was a paste (look at undo stack)
});
```

Actually, simpler: just detect the encoding of the clipboard on paste and update the status bar:
```cpp
connect(QApplication::clipboard(), &QClipboard::changed, this, [this](QClipboard::Mode mode) {
    if (mode == QClipboard::Clipboard) {
        QString text = QApplication::clipboard()->text();
        if (!text.isEmpty()) {
            EncodingType enc = EncodingDetector::instance().detect(text.toUtf8());
            if (enc != EncodingType::ANSI && enc != EncodingType::UTF_8) {
                // Show notification
                qDebug() << "[clipboard] Detected encoding:" << (int)enc;
            }
        }
    }
});
```

#### 3. Encoding auto-detection for opened files

Wire `LanguageManager::detectLanguage()` when opening files:
```cpp
BufferID FileManager::openFile(const QString& path, bool readOnly) {
    // ... existing code ...
    if (buf) {
        // Auto-detect language from extension
        LangType lang = LanguageManager::instance().detectLanguage(path.toStdString());
        buf->setLangType(lang);
        // Also detect from content for extensionless files
        if (lang == LangType::L_TEXT) {
            QString firstLine;
            QTextStream s(&f);
            firstLine = s.readLine();
            lang = LanguageManager::instance().detectLanguageFromContent(firstLine.toStdString());
            buf->setLangType(lang);
        }
    }
}
```

#### 4. Window state persistence (already in Wave 17)

Ensure `MainWindow` saves geometry on close:
```cpp
// In MainWindow closeEvent
void MainWindow::closeEvent(QCloseEvent* event) {
    QSettings s;
    s.setValue("window/geometry", saveGeometry());
    s.setValue("window/state", saveState());
    event->accept();
}
```

### Build and test
```bash
cmake --build build -j$(nproc) 2>&1 | grep -E "error:|FAILED"
```

---

## Build Command
```bash
cd /home/node/.openclaw/workspace
cmake --build build -j$(nproc)
```

## Test Command
```bash
ctest --test-dir build --output-on-failure
```

## Binary
`/home/node/.openclaw/workspace/build/NotepadMinusMinusQt` (1.9MB)
